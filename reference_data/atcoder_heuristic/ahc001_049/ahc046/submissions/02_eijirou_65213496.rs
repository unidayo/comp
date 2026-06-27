use std::process::exit;

use proconio::input;
use rand::{Rng, SeedableRng};
use rand_pcg::Pcg64Mcg;

const TIME_LIMIT_SEC: f64 = 1.95;
const N: usize = 20;
const M: usize = 40;
const UDLR: [char; 4] = ['U', 'D', 'L', 'R'];
const DXDY: [(usize, usize); 4] = [(!0, 0), (1, 0), (0, !0), (0, 1)];

fn in_grid(x: usize, y: usize) -> bool {
    x < N && y < N
}

fn main() {
    get_time_sec();

    let input = read_input();

    let config = Config {
        max_turn: 800,
        beam_width: 2700,
        nodes_capacity: 0,
    };

    let state = State::new(&input);
    let actions = beam_search(&config, state).unwrap();

    let mut put_blocks = vec![vec![]; actions.len()];
    for action in &actions {
        if action.op != 'S' {
            continue;
        }
        if action.bt != !0 {
            let ux = actions[action.bt].tx;
            let uy = actions[action.bt].ty;
            let i = DXDY
                .iter()
                .position(|&(dx, dy)| {
                    ux.wrapping_add(dx) == action.bx && uy.wrapping_add(dy) == action.by
                })
                .unwrap();
            put_blocks[action.bt].push(UDLR[i]);
        }
    }
    for i in 0..actions.len() {
        println!("{} {}", actions[i].op, actions[i].dir);
        for &dir in &put_blocks[i] {
            println!("A {}", dir);
        }
    }

    exit(0);
}

type Cost = i32;
type Hash = u64;
type CandidateIndex = u32;
type NodeIndex = u32;

// beam search setting
// capacity = 0 is OK.
#[derive(Debug, Clone)]
pub struct Config {
    max_turn: usize,
    beam_width: usize,
    nodes_capacity: usize,
}

// data for a state transition
// Try to minimize memory usage.
#[derive(Debug, Clone, PartialEq, Eq, Default)]
pub struct Action {
    op: char,
    dir: char,
    sx: usize,
    sy: usize,
    tx: usize,
    ty: usize,
    bt: usize,
    bx: usize,
    by: usize,
    can_put_changes: Vec<(usize, usize, usize, usize)>,
}

// data for evaluation
// Try to minimize memory usage.
#[derive(Debug, Clone, Default)]
struct Evaluator {
    x_dist: usize,
    y_dist: usize,
    target_id: usize,
    num_blocks: usize,
    can_put_blocks: usize,
}

impl Evaluator {
    // the lower, the better
    fn evaluate(&self) -> Cost {
        let x_dist = self.x_dist as Cost;
        let y_dist = self.y_dist as Cost;
        let target_id = self.target_id as Cost;
        let basic_cost = x_dist + y_dist - 40 * target_id;
        100 * basic_cost - 15 * self.can_put_blocks as Cost + self.num_blocks as Cost
    }
}

// data for an expansion node
#[derive(Debug, Clone)]
struct Candidate {
    action: Action,
    evaluator: Evaluator,
    cost: Cost,
    hash: Hash,
    parent: NodeIndex,
}

// erasable max priority queue
#[derive(Debug, Clone)]
struct SegmentTree {
    n: usize,
    log: usize,
    size: usize,
    d: Vec<(Cost, CandidateIndex)>,
}

impl SegmentTree {
    fn new(n: usize) -> Self {
        let mut ret = Self {
            n: 0,
            log: 0,
            size: 0,
            d: Vec::new(),
        };
        ret.init(n);
        ret
    }

    fn init(&mut self, n: usize) {
        self.n = n;
        self.size = n.next_power_of_two();
        self.log = self.size.trailing_zeros() as usize;
        self.d.clear();
        self.d.resize(2 * self.size, (Cost::MIN, CandidateIndex::MAX));
    }

    fn build(&mut self, candidates: &Vec<Candidate>) {
        self.init(candidates.len());
        for i in 0..self.n {
            self.d[self.size + i] = (candidates[i].cost, i as CandidateIndex);
        }
        for i in (1..self.size).rev() {
            self.update(i);
        }
    }

    fn set(&mut self, p: usize, cost: Cost) {
        debug_assert!(p < self.n);
        let q = p + self.size;
        self.d[q] = (cost, p as CandidateIndex);
        for i in 1..=self.log {
            self.update(q >> i);
        }
    }

    fn top(&self) -> (Cost, CandidateIndex) {
        self.d[1]
    }

    fn update(&mut self, k: usize) {
        let (c0, p0) = self.d[2 * k];
        let (c1, p1) = self.d[2 * k + 1];
        self.d[k] = if c0 >= c1 { (c0, p0) } else { (c1, p1) };
    }
}

// select beam width candidates from better evaluation
// It remains only the best one for same hash candidates.
#[derive(Debug, Clone)]
struct Selector {
    beam_width: usize,
    candidates: Vec<Candidate>,
    hash_to_index: rustc_hash::FxHashMap<Hash, CandidateIndex>,
    segtree: SegmentTree,
    finished_candidate: Option<Candidate>,
}

impl Selector {
    fn new(beam_width: usize) -> Self {
        Self {
            beam_width,
            candidates: Vec::with_capacity(beam_width),
            hash_to_index: rustc_hash::FxHashMap::default(),
            segtree: SegmentTree::new(beam_width),
            finished_candidate: None,
        }
    }

    // add a candidate
    // finished = true iff it can get feasible solution through the candidate in the turn minimizing problem.
    // It returns true iff it provisionally accept the candidate.
    #[inline(always)]
    fn push(&mut self, candidate: Candidate, finished: bool) -> bool {
        let cost = candidate.cost;
        if finished {
            match &self.finished_candidate {
                Some(best_candidate) => {
                    if cost < best_candidate.cost {
                        self.finished_candidate = Some(candidate);
                        return true;
                    } else {
                        return false;
                    }
                }
                None => {
                    self.finished_candidate = Some(candidate);
                    return true;
                }
            }
        }
        if self.built_segtree() && cost >= self.segtree.top().0 {
            return false;
        }
        let hash = candidate.hash;
        match self.hash_to_index.get(&hash) {
            Some(&p) => {
                // contains a candidate with same hash
                let p = p as usize;
                debug_assert_eq!(self.candidates[p].hash, hash);
                if cost < self.candidates[p].cost {
                    self.candidates[p] = candidate;
                    if self.built_segtree() {
                        self.segtree.set(p, cost);
                    }
                    true
                } else {
                    false
                }
            }
            None => {
                // does not contain a candidate with same hash
                if self.built_segtree() {
                    let p = self.segtree.top().1 as usize;
                    self.hash_to_index.remove(&self.candidates[p].hash);
                    self.hash_to_index.insert(hash, p as CandidateIndex);
                    self.candidates[p] = candidate;
                    self.segtree.set(p, cost);
                } else {
                    let p = self.candidates.len();
                    self.hash_to_index.insert(hash, p as CandidateIndex);
                    self.candidates.push(candidate);
                    if self.built_segtree() {
                        // candidates become full
                        self.segtree.build(&self.candidates);
                    }
                }
                true
            }
        }
    }

    fn get_best_candidate(&self) -> Candidate {
        self.candidates
            .iter()
            .min_by_key(|candidate| candidate.cost)
            .unwrap()
            .clone()
    }

    fn clear(&mut self) {
        self.candidates.clear();
        self.hash_to_index.clear();
        self.finished_candidate = None;
    }

    fn built_segtree(&self) -> bool {
        self.candidates.len() == self.beam_width
    }
}

#[derive(Debug, Clone)]
struct MultiSelectors {
    beam_width: usize,
    selectors: std::collections::VecDeque<Selector>,
    step_max: usize,
}

impl MultiSelectors {
    fn new(beam_width: usize) -> Self {
        Self {
            beam_width,
            selectors: std::collections::VecDeque::new(),
            step_max: 0,
        }
    }

    #[inline(always)]
    fn push_candidate(&mut self, candidate: Candidate, finished: bool, step: usize) -> bool {
        debug_assert!(step >= 1);
        while self.selectors.len() < step {
            self.selectors.push_back(Selector::new(self.beam_width));
        }
        if self.selectors[step - 1].push(candidate, finished) {
            if self.step_max < step {
                self.step_max = step;
            }
            true
        } else {
            false
        }
    }

    fn reset_step_max(&mut self) {
        self.step_max = 1;
    }

    fn pop_front(&mut self) -> Selector {
        debug_assert!(!self.selectors.is_empty());
        self.selectors.pop_front().unwrap()
    }

    fn push_back(&mut self, mut selector: Selector) {
        selector.clear();
        self.selectors.push_back(selector);
    }
}

#[derive(Debug, Clone)]
pub struct State {
    turn: usize,
    targets: Vec<(usize, usize)>,
    block: [[bool; N]; N],
    can_put: [[usize; N]; N],
    hashes: [[u64; N]; N],
}

// data updated in depth first search
impl State {
    fn new(input: &Input) -> Self {
        let mut rng = Pcg64Mcg::seed_from_u64(0);
        let mut hashes = [[0; N]; N];
        for i in 0..N {
            for j in 0..N {
                hashes[i][j] = rng.gen();
            }
        }
        Self {
            turn: 0,
            targets: input.targets.clone(),
            block: [[false; N]; N],
            can_put: [[!0; N]; N],
            hashes,
        }
    }

    // return the initial value of Evaluator and Hash
    fn get_initial_data(&self) -> (Action, Evaluator, Hash) {
        let action = Action {
            op: ' ',
            dir: ' ',
            sx: 0,
            sy: 0,
            tx: self.targets[0].0,
            ty: self.targets[0].1,
            bt: !0,
            bx: !0,
            by: !0,
            can_put_changes: vec![],
        };
        let evaluator = Evaluator {
            x_dist: 0,
            y_dist: 0,
            target_id: 0,
            num_blocks: 0,
            can_put_blocks: 0,
        };
        let hash = 0;
        (action, evaluator, hash)
    }

    // add candidates to selector
    // argument
    // - evaluator: current evaluator
    // - hash: current hash
    // - parent: current node ID (parent node ID for next state)
    fn expand(
        &self,
        action: &Action,
        evaluator: &Evaluator,
        hash: Hash,
        parent: NodeIndex,
        selector: &mut MultiSelectors,
    ) {
        let sx = action.tx;
        let sy = action.ty;

        // move to adjacent square
        for d in 0..4 {
            let (dx, dy) = DXDY[d];
            let tx = sx.wrapping_add(dx);
            let ty = sy.wrapping_add(dy);
            if !in_grid(tx, ty) || self.block[tx][ty] {
                continue;
            }
            let action = Action {
                op: 'M',
                dir: UDLR[d],
                sx,
                sy,
                tx,
                ty,
                bt: !0,
                bx: !0,
                by: !0,
                can_put_changes: vec![],
            };
            let candidate = self.make_candidate(&evaluator, action, hash, parent);
            let finished = candidate.evaluator.target_id == M;
            selector.push_candidate(candidate, finished, 1);
        }

        // slide to block
        for d in 0..4 {
            let (dx, dy) = DXDY[d];
            let mut tx = sx.wrapping_add(dx);
            let mut ty = sy.wrapping_add(dy);
            while in_grid(tx, ty) && !self.block[tx][ty] {
                let ux = tx.wrapping_add(dx);
                let uy = ty.wrapping_add(dy);
                if !in_grid(ux, uy) || self.block[ux][uy] || self.can_put[ux][uy] != !0 {
                    let mut action = Action {
                        op: 'S',
                        dir: UDLR[d],
                        sx,
                        sy,
                        tx,
                        ty,
                        bt: !0,
                        bx: !0,
                        by: !0,
                        can_put_changes: vec![],
                    };
                    let step = if in_grid(ux, uy) && !self.block[ux][uy] && self.can_put[ux][uy] != !0 {
                        // put a block
                        action.bt = self.can_put[ux][uy];
                        action.bx = ux;
                        action.by = uy;
                        2
                    } else {
                        1
                    };
                    let candidate = self.make_candidate(&evaluator, action, hash, parent);
                    let finished = candidate.evaluator.target_id == M;
                    selector.push_candidate(candidate, finished, step);
                }
                tx = ux;
                ty = uy;
            }
        }

        // remove block
        for d in 0..4 {
            let (dx, dy) = DXDY[d];
            let bx = sx.wrapping_add(dx);
            let by = sy.wrapping_add(dy);
            if !in_grid(bx, by) || !self.block[bx][by] {
                continue;
            }
            let action = Action {
                op: 'A',
                dir: UDLR[d],
                sx,
                sy,
                tx: sx,
                ty: sy,
                bt: 0,
                bx,
                by,
                can_put_changes: vec![],
            };
            let candidate = self.make_candidate(&evaluator, action, hash, parent);
            selector.push_candidate(candidate.clone(), false, 1);
        }
    }

    #[inline(always)]
    fn make_candidate(
        &self,
        evaluator: &Evaluator,
        mut action: Action,
        mut hash: Hash,
        parent: NodeIndex,
    ) -> Candidate {
        let target_id = evaluator.target_id;
        let num_blocks = evaluator.num_blocks;
        let (gx, gy) = self.targets[target_id];
        let x_dist = action.tx.abs_diff(gx);
        let y_dist = action.ty.abs_diff(gy);
        let num_blocks = if action.op == 'A' {
            num_blocks - 1
        } else if action.bt == !0 {
            num_blocks
        } else {
            num_blocks + 1
        };
        let mut evaluator = if x_dist == 0 && y_dist == 0 {
            if target_id == M - 1 {
                // finished
                Evaluator {
                    x_dist: 0,
                    y_dist: 0,
                    target_id: M,
                    num_blocks,
                    can_put_blocks: evaluator.can_put_blocks,
                }
            } else {
                let (gx, gy) = self.targets[target_id + 1];
                let x_dist = action.tx.abs_diff(gx);
                let y_dist = action.ty.abs_diff(gy);
                Evaluator {
                    x_dist,
                    y_dist,
                    target_id: target_id + 1,
                    num_blocks,
                    can_put_blocks: evaluator.can_put_blocks,
                }
            }
        } else {
            Evaluator {
                x_dist,
                y_dist,
                target_id,
                num_blocks,
                can_put_blocks: evaluator.can_put_blocks,
            }
        };
        hash ^= (self.hashes[action.sx][action.sy]) >> 1;
        hash ^= (self.hashes[action.tx][action.ty]) >> 1;
        if action.bx != !0 {
            hash ^= self.hashes[action.bx][action.by];
        }

        for x in action.sx.min(action.tx)..=action.sx.max(action.tx) {
            for y in action.sy.min(action.ty)..=action.sy.max(action.ty) {
                if self.can_put[x][y] != !0 {
                    action.can_put_changes.push((x, y, self.can_put[x][y], !0));
                    evaluator.can_put_blocks -= 1;
                }
            }
        }
        for &(dx, dy) in &DXDY {
            let ux = action.tx.wrapping_add(dx);
            let uy = action.ty.wrapping_add(dy);
            if !in_grid(ux, uy) {
                continue;
            }
            if ux == action.bx && uy == action.by {
                action
                    .can_put_changes
                    .push((ux, uy, self.can_put[ux][uy], !0));
            } else {
                action
                    .can_put_changes
                    .push((ux, uy, self.can_put[ux][uy], self.turn));
                if self.can_put[ux][uy] == !0 {
                    evaluator.can_put_blocks += 1;
                }
            }
        }

        let cost = evaluator.evaluate();

        Candidate {
            action,
            evaluator,
            cost,
            hash,
            parent,
        }
    }

    fn move_forward(&mut self, action: &Action) {
        self.turn += 1;
        if action.bt != !0 {
            self.block[action.bx][action.by] = !self.block[action.bx][action.by];
        }
        for &(x, y, s, t) in &action.can_put_changes {
            // debug_assert_eq!(self.can_put[x][y], s);
            self.can_put[x][y] = t;
        }
    }

    fn move_backward(&mut self, action: &Action) {
        self.turn -= 1;
        if action.bt != !0 {
            self.block[action.bx][action.by] = !self.block[action.bx][action.by];
        }
        for &(x, y, s, t) in action.can_put_changes.iter().rev() {
            // debug_assert_eq!(self.can_put[x][y], t);
            self.can_put[x][y] = s;
        }
    }
}

#[derive(Debug, Clone)]
pub struct ObjectPool<T> {
    data: Vec<T>,
    garbage: Vec<usize>,
}

impl<T: Default> ObjectPool<T> {
    pub fn with_capacity(capacity: usize) -> ObjectPool<T> {
        ObjectPool {
            data: Vec::with_capacity(capacity),
            garbage: Vec::new(),
        }
    }

    // push `item`, then return the index
    pub fn push(&mut self, item: T) -> usize {
        if let Some(i) = self.garbage.pop() {
            self.data[i] = item;
            i
        } else {
            self.data.push(item);
            self.data.len() - 1
        }
    }

    pub fn pull(&mut self) -> usize {
        if let Some(i) = self.garbage.pop() {
            i
        } else {
            self.data.push(T::default());
            self.data.len() - 1
        }
    }

    // remove the item at position `index`
    pub fn remove(&mut self, index: usize) {
        self.garbage.push(index);
    }

    pub fn clear(&mut self) {
        self.data.clear();
        self.garbage.clear();
    }
}

impl<T> std::ops::Index<usize> for ObjectPool<T> {
    type Output = T;

    fn index(&self, index: usize) -> &Self::Output {
        &self.data[index]
    }
}

impl<T> std::ops::IndexMut<usize> for ObjectPool<T> {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        &mut self.data[index]
    }
}

// node of doubly chained tree
#[derive(Debug, Clone, Default)]
struct Node {
    action: Action,
    evaluator: Evaluator,
    hash: Hash,
    parent: NodeIndex,
    child: NodeIndex,
    left: NodeIndex,
    right: NodeIndex,
    active: bool,
}

impl Node {
    fn root(action: Action, evaluator: Evaluator, hash: Hash) -> Self {
        Self {
            action,
            evaluator,
            hash,
            parent: !0,
            child: !0,
            left: !0,
            right: !0,
            active: true,
        }
    }

    fn new(candidate: &Candidate, right: NodeIndex) -> Self {
        Self {
            action: candidate.action.clone(),
            evaluator: candidate.evaluator.clone(),
            hash: candidate.hash,
            parent: candidate.parent,
            child: !0,
            left: !0,
            right,
            active: true,
        }
    }
}

// doubly chained tree
#[derive(Debug, Clone)]
struct Tree {
    state: State,
    nodes: ObjectPool<Node>,
    root: NodeIndex,
    remove_nodes: std::collections::VecDeque<Vec<NodeIndex>>,
}

impl Tree {
    fn new(state: State, config: &Config) -> Self {
        let (action, evaluator, hash) = state.get_initial_data();
        let mut nodes = ObjectPool::with_capacity(config.nodes_capacity);
        let root = nodes.push(Node::root(action, evaluator, hash)) as NodeIndex;
        let remove_nodes = std::collections::VecDeque::new();
        Self { state, nodes, root, remove_nodes }
    }

    // add candidates while dfs
    fn dfs(&mut self, multi_selectors: &mut MultiSelectors) {
        self.remove_useless_nodes();
        self.update_root();

        let mut v = self.root;

        if !self.nodes[v as usize].active {
            // no active nodes
            return;
        }

        loop {
            v = self.move_to_leaf(v);

            multi_selectors.reset_step_max();
            self.state.expand(
                &self.nodes[v as usize].action,
                &self.nodes[v as usize].evaluator,
                self.nodes[v as usize].hash,
                v,
                multi_selectors,
            );
            while self.remove_nodes.len() < multi_selectors.step_max {
                self.remove_nodes.push_back(Vec::new());
            }
            self.remove_nodes[multi_selectors.step_max - 1].push(v);

            v = self.move_to_ancestor(v);
            if v == self.root {
                break;
            }
        }
    }

    // get path from the root to the node `v`
    fn get_path(&self, mut v: NodeIndex) -> Vec<Action> {
        // dbg!(self.nodes.data.capacity());

        let mut path = Vec::new();
        while self.nodes[v as usize].parent != !0 {
            path.push(self.nodes[v as usize].action.clone());
            v = self.nodes[v as usize].parent;
        }
        path.reverse();
        path
    }

    // add new node
    fn add_leaf(&mut self, candidate: &Candidate) -> NodeIndex {
        let parent = candidate.parent;
        let sibling = self.nodes[parent as usize].child;
        let v = self.nodes.push(Node::new(candidate, sibling)) as NodeIndex;

        self.nodes[parent as usize].child = v;

        if sibling != !0 {
            self.nodes[sibling as usize].left = v;
        }
        // activate ancestors
        let mut u = parent;
        while !self.nodes[u as usize].active {
            self.nodes[u as usize].active = true;
            if u == self.root {
                break;
            }
            u = self.nodes[u as usize].parent;
        }
        v
    }

    // move to the leftist node in the subtree rooted at `v`
    fn move_to_leaf(&mut self, mut v: NodeIndex) -> NodeIndex {
        debug_assert!(self.nodes[v as usize].active);
        let mut child = self.nodes[v as usize].child;
        while child != !0 {
            // move to right while the node is inactive
            while !self.nodes[child as usize].active {
                child = self.nodes[child as usize].right;
                debug_assert_ne!(child, !0);
            }
            self.nodes[v as usize].active = false;
            v = child;
            self.state.move_forward(&self.nodes[child as usize].action);
            child = self.nodes[child as usize].child;
        }
        debug_assert!(self.nodes[v as usize].active);
        self.nodes[v as usize].active = false;
        v
    }

    // move to the ancestor of `v` which has the right child
    fn move_to_ancestor(&mut self, mut v: NodeIndex) -> NodeIndex {
        while v != self.root {
            self.state.move_backward(&self.nodes[v as usize].action);

            // move to right while the node is inactive
            let mut u = self.nodes[v as usize].right;
            while u != !0 {
                if self.nodes[u as usize].active {
                    self.state.move_forward(&self.nodes[u as usize].action);
                    return u;
                }
                u = self.nodes[u as usize].right;
            }

            v = self.nodes[v as usize].parent;
        }
        self.root
    }

    // do not round trip the direct road
    fn update_root(&mut self) {
        let mut child = self.nodes[self.root as usize].child;
        while child != !0 && self.nodes[child as usize].right == !0 {
            self.root = child;
            self.state.move_forward(&self.nodes[child as usize].action);
            child = self.nodes[child as usize].child;
        }
    }

    // remove useless nodes
    fn remove_useless_nodes(&mut self) {
        if self.remove_nodes.is_empty() {
            return;
        }
        let mut remove_nodes_front = self.remove_nodes.pop_front().unwrap();
        for &v in &remove_nodes_front {
            if self.nodes[v as usize].child == !0 {
                self.remove_leaf(v);
            }
        }
        remove_nodes_front.clear();
        self.remove_nodes.push_back(remove_nodes_front);
    }

    // remove the node `v` and its ancestors while they have no child
    fn remove_leaf(&mut self, mut v: NodeIndex) {
        loop {
            let left = self.nodes[v as usize].left;
            let right = self.nodes[v as usize].right;
            if left == !0 {
                let parent = self.nodes[v as usize].parent;
                debug_assert_ne!(parent, !0, "root node cannot be removed");
                self.nodes.remove(v as usize);
                self.nodes[parent as usize].child = right;
                if right != !0 {
                    self.nodes[right as usize].left = !0;
                    break;
                }
                v = parent;
            } else {
                self.nodes.remove(v as usize);
                self.nodes[left as usize].right = right;
                if right != !0 {
                    self.nodes[right as usize].left = left;
                }
                break;
            }
        }
    }
}

pub fn beam_search(config: &Config, state: State) -> Option<Vec<Action>> {
    let mut tree = Tree::new(state, config);
    let mut multi_selectors = MultiSelectors::new(config.beam_width);

    for turn in 0..config.max_turn {
        // add candidates to selector
        tree.dfs(&mut multi_selectors);

        let selector = multi_selectors.pop_front();
        if let Some(candidate) = &selector.finished_candidate {
            // find the feasible solution in turn minimizing problem
            let mut ret = tree.get_path(candidate.parent);
            ret.push(candidate.action.clone());
            return Some(ret);
        }
        if turn == config.max_turn - 1 {
            // the last turn
            let best_candidate = selector.get_best_candidate();
            let mut ret = tree.get_path(best_candidate.parent);
            ret.push(best_candidate.action);
            return Some(ret);
        }
        // add new nodes
        for candidate in &selector.candidates {
            tree.add_leaf(candidate);
        }

        multi_selectors.push_back(selector);
    }

    unreachable!();
}

struct Input {
    targets: Vec<(usize, usize)>,
}

fn read_input() -> Input {
    input! {
        _n: usize,
        _m: usize,
        targets: [(usize, usize); M],
    }
    Input { targets }
}

pub trait ChangeMinMax {
    fn chmin(&mut self, x: Self) -> bool;
    fn chmax(&mut self, x: Self) -> bool;
}

impl<T: PartialOrd> ChangeMinMax for T {
    fn chmin(&mut self, x: T) -> bool {
        *self > x && {
            *self = x;
            true
        }
    }

    fn chmax(&mut self, x: T) -> bool {
        *self < x && {
            *self = x;
            true
        }
    }
}

pub fn get_time_sec() -> f64 {
    static mut STIME: f64 = -1.0;
    let t = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap();
    let ms = t.as_secs() as f64 + t.subsec_nanos() as f64 * 1e-9;
    unsafe {
        if STIME < 0.0 {
            STIME = ms;
        }
        ms - STIME
    }
}
