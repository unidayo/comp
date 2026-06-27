use problem::Input;
use solver::Solver;

#[allow(dead_code)]
mod beam {
    //! ビームサーチライブラリ
    //! [rhooさんの記事](https://qiita.com/rhoo/items/2f647e32f6ff2c6ee056)を大いに参考にさせて頂きました。
    //! ありがとうございます……。
    //!
    //! # Usage
    //!
    //! ```
    //! use cp_lib_rs::beam;
    //!
    //! // 状態のうち、差分計算を行わない部分
    //! #[derive(Clone, Default)]
    //! struct SmallState;
    //!
    //! impl beam::SmallState for SmallState {
    //!     type Score = i64;
    //!     type Hash = u64;
    //!     type LargeState = LargeState;
    //!     type Action = usize;
    //!
    //!     fn raw_score(&self) -> <Self as beam::SmallState>::Score { todo!() }
    //!     fn hash(&self) -> <Self as beam::SmallState>::Hash { todo!() }
    //!     fn apply(&self, state: &mut <Self as beam::SmallState>::LargeState) { todo!() }
    //!     fn rollback(&self, state: &mut <Self as beam::SmallState>::LargeState) { todo!() }
    //!     fn action(&self) -> <Self as beam::SmallState>::Action { todo!() }
    //! }
    //!
    //! // 状態のうち、差分計算を行う部分
    //! struct LargeState;
    //!
    //! // 次の行動を生成する構造体
    //! struct ActionGenerator;
    //!
    //! impl beam::ActGen<SmallState> for ActionGenerator {
    //!     fn generate(&self, small_state: &SmallState, large_state: &<SmallState as beam::SmallState>::LargeState, actions: &mut Vec<SmallState>) {
    //!         // 状態を生成してactionsに追加する
    //!         for _ in 0..0 {
    //!            actions.push(SmallState);
    //!         }
    //!     }
    //! }
    //!
    //! fn beam() -> Vec<usize> {
    //!     let large_state = LargeState;
    //!     let small_state = SmallState;
    //!     let action_generator = ActionGenerator;
    //!     let mut beam = beam::BeamSearch::new(large_state, small_state, action_generator);
    //!
    //!     let deduplicator = beam::NoOpDeduplicator;
    //!     let beam_width = beam::FixedBeamWidthSuggester::new(100);
    //!     let (actions, score) = beam.run(2500, beam_width, deduplicator);
    //!
    //!     actions
    //! }
    //!
    //! ```
    //!
    use std::{
        cmp::Reverse,
        fmt::Display,
        hash::Hash,
        ops::{Index, IndexMut},
        time::Instant,
    };

    use rustc_hash::FxHashSet;

    /// コピー可能な小さい状態を表すトレイト
    pub trait SmallState {
        type Score: Ord + Display;
        type Hash: Hash + Eq;
        type LargeState;
        type Action;

        /// ビームサーチ用スコア（大きいほど良い）
        /// デフォルトでは生スコアをそのまま返す
        fn beam_score(&self) -> Self::Score {
            self.raw_score()
        }

        // 生スコア
        fn raw_score(&self) -> Self::Score;

        /// ハッシュ値
        fn hash(&self) -> Self::Hash;

        /// stateにこの差分を作用させる
        fn apply(&self, state: &mut Self::LargeState);

        /// stateに作用させたこの差分をロールバックする
        fn rollback(&self, state: &mut Self::LargeState);

        /// 実行した行動を返す
        fn action(&self) -> Self::Action;
    }

    /// 現在のstateからの遷移先を列挙するトレイト
    pub trait ActGen<S: SmallState> {
        /// 現在のstateからの遷移先をnext_satesに格納する
        fn generate(&self, small_state: &S, large_state: &S::LargeState, next_states: &mut Vec<S>);
    }

    /// ビームの次の遷移候補
    #[derive(Clone)]
    struct Cancidate<S: SmallState + Clone> {
        /// 実行後のsmall_state
        small_state: S,
        /// 親となるノードのインデックス
        parent: NodeIndex,
    }

    impl<S: SmallState + Clone> Cancidate<S> {
        fn new(small_state: S, parent: NodeIndex) -> Self {
            Self {
                small_state,
                parent,
            }
        }

        fn to_node(
            self,
            child: NodeIndex,
            left_sibling: NodeIndex,
            right_sibling: NodeIndex,
        ) -> Node<S> {
            Node {
                small_state: self.small_state,
                parent: self.parent,
                child,
                left_sibling,
                right_sibling,
            }
        }
    }

    /// 重複除去を行うトレイト
    pub trait Deduplicator<S: SmallState> {
        /// 重複除去に使った情報をクリアし、次の重複除去の準備をする
        fn clear(&mut self);

        /// 重複チェックを行い、残すべきならtrue、重複していればfalseを返す
        fn filter(&mut self, state: &S) -> bool;
    }

    /// 重複除去を行わず素通しするDeduplicator
    pub struct NoOpDeduplicator;

    impl<S: SmallState> Deduplicator<S> for NoOpDeduplicator {
        fn clear(&mut self) {
            // do nothing
        }

        fn filter(&mut self, _state: &S) -> bool {
            // 常に素通しする
            true
        }
    }

    /// 同じハッシュ値を持つ状態を1つだけに制限するDeduplicator
    pub struct HashSingleDeduplicator<S: SmallState> {
        set: FxHashSet<S::Hash>,
    }

    impl<S: SmallState> HashSingleDeduplicator<S> {
        pub fn new() -> Self {
            Self {
                set: FxHashSet::default(),
            }
        }
    }

    impl<S: SmallState> Deduplicator<S> for HashSingleDeduplicator<S> {
        fn clear(&mut self) {
            self.set.clear();
        }

        fn filter(&mut self, state: &S) -> bool {
            // ハッシュが重複していなければ通す
            self.set.insert(state.hash())
        }
    }

    /// ビーム幅を提案するトレイト
    pub trait BeamWidthSuggester {
        // 現在のターン数を受け取り、ビーム幅を提案する
        fn suggest(&mut self) -> usize;
    }

    /// 常に固定のビーム幅を返すBeamWidthSuggester
    pub struct FixedBeamWidthSuggester {
        width: usize,
    }

    impl FixedBeamWidthSuggester {
        pub fn new(width: usize) -> Self {
            Self { width }
        }
    }

    impl BeamWidthSuggester for FixedBeamWidthSuggester {
        fn suggest(&mut self) -> usize {
            self.width
        }
    }

    /// ベイズ推定+カルマンフィルタにより適切なビーム幅を計算するBeamWidthSuggester。
    /// 1ターンあたりの実行時間が正規分布に従うと仮定し、+3σ分の余裕を持ってビーム幅を決める。
    ///
    /// ## モデル
    ///
    /// カルマンフィルタを適用するにあたって、以下のモデルを考える。
    ///
    /// - `i` ターン目のビーム幅1あたりの所要時間の平均値 `t_i` が正規分布 `N(μ_i, σ_i^2)` に従うと仮定する。
    ///   - 各ターンに観測される所要時間が `N(μ_i, σ_i^2)` に従うのではなく、所要時間の**平均値**が `N(μ_i, σ_i^2)` に従うとしている点に注意。
    ///     - すなわち `μ_i` は所要時間の平均値の平均値であり、所要時間の平均値が `μ_i` を中心とした確率分布を形成しているものとしている。ややこしい。
    ///   - この `μ_i` , `σ_i^2` をベイズ推定によって求めたい。
    /// - 所要時間 `t_i` は `t_{i+1}=t_i+N(0, α^2)` により更新されるものとする。
    ///   - `N(0, α^2)` は標準偏差 `α` のノイズを意味する。お気持ちとしては「実行時間がターン経過に伴ってちょっとずつ変わっていくことがあるよ」という感じ。
    ///   - `α` は既知の定数とし、適当に決める。
    ///   - 本来は問題に合わせたちゃんとした更新式にすべき（ターン経過に伴って線形に増加するなど）なのだが、事前情報がないため大胆に仮定する。
    /// - 所要時間の観測値 `τ_i` は `τ_i=t_i+N(0, β^2)` により得られるものとする。
    ///   - `β` は既知の定数とし、適当に決める。
    ///   - 本来この `β` も推定できると嬉しいのだが、取扱いが煩雑になるためこちらも大胆に仮定する。
    ///
    /// ## モデルの初期化
    ///
    /// - `μ_0` は実行時間制限を `T` 、標準ビーム幅を `W` 、実行ターン数を `M` として、 `μ_0=T/WM` などとすればよい。
    /// - `σ_0` は適当に `σ_0=0.1μ_0` とする。ここは標準ビーム幅にどのくらい自信があるかによる。
    /// - `α` は適当に `α=0.01μ_0` とする。定数は本当に勘。多分問題に合わせてちゃんと考えた方が良い。
    /// - `β` は `σ_0=0.05μ_0` とする。適当なベンチマーク問題で標準偏差を取ったらそのくらいだったため。
    ///
    /// ## モデルの更新
    ///
    /// 以下のように更新をかけていく。
    ///
    /// 1. `t_0=N(μ_0, σ_0^2)` と初期化する。
    /// 2. `t_1=t_0+N(0, α^2)` とし、事前分布 `t_1=N(μ_1, σ_1^2)=N(μ_0, σ_0^2+α^2)` を得る。ここはベイズ更新ではなく単純な正規分布の合成でよい。
    /// 3. `τ_1` が観測されるので、ベイズ更新して事後分布 `N(μ_1', σ_1^2')` を得る。
    /// 4. 同様に `t_2=N(μ_2, σ_2^2)` を得る。
    /// 5. `τ_2` を用いてベイズ更新。以下同様。
    ///
    /// ## 適切なビーム幅の推定
    ///
    /// - 余裕を持って、99.8%程度の確率（+3σ）で実行時間制限に収まるようなビーム幅にしたい。
    /// - ここで、 `t_i=t_{i+1}=･･･=t_M=N(μ_i, σ_i^2)` と大胆仮定する。
    ///   - `α` によって `t_i` がどんどん変わってしまうと考えるのは保守的すぎるため。
    /// - すると残りターン数 `M_i=M-i` として、 `Στ_i=N(M_i*μ_i, M_i*σ_i^2)` となる。
    /// - したがって、残り時間を `T_i` として `W(M_i*μ_i+3(σ_i√M_i))≦T_i` となる最大の `W` を求めればよく、 `W=floor(T_i/(M_i*μ_i+3(σ_i√M_i)))` となる。
    /// - 最後に、念のため適当な `W_min` , `W_max` でclampしておく。
    pub struct BayesianBeamWidthSuggester {
        /// ビーム幅1あたりの所要時間の平均値の平均値μ_i（逐次更新される）
        mean_sec: f64,
        /// ビーム幅1あたりの所要時間の平均値の分散σ_i^2（逐次更新される）
        variance_sec: f64,
        /// 1ターンごとに状態に作用するノイズの大きさを表す分散α^2（定数）
        variance_state_sec: f64,
        /// 観測時に乗るノイズの大きさを表す分散β^2（定数）
        variance_observe_sec: f64,
        /// 問題の実行時間制限T
        time_limit_sec: f64,
        /// 現在のターン数i
        current_turn: usize,
        /// 最大ターン数M
        max_turn: usize,
        /// ウォームアップターン数（最初のXターン分の情報は採用せずに捨てる）
        warmup_turn: usize,
        /// 最小ビーム幅W_min
        min_beam_width: usize,
        /// 最大ビーム幅W_max
        max_beam_width: usize,
        /// 現在のビーム幅W_i
        current_beam_width: usize,
        /// ログの出力インターバル（0にするとログを出力しなくなる）
        verbose_interval: usize,
        /// ビーム開始時刻
        start_time: Instant,
        /// 前回の計測時刻
        last_time: Instant,
    }

    impl BayesianBeamWidthSuggester {
        pub fn new(
            max_turn: usize,
            warmup_turn: usize,
            time_limit_sec: f64,
            standard_beam_width: usize,
            min_beam_width: usize,
            max_beam_width: usize,
            verbose_interval: usize,
        ) -> Self {
            assert!(
                max_turn * standard_beam_width > 0,
                "ターン数とビーム幅設定が不正です。"
            );
            assert!(
                min_beam_width > 0,
                "最小のビーム幅は正の値でなければなりません。"
            );
            assert!(
                min_beam_width <= max_beam_width,
                "最大のビーム幅は最小のビーム幅以上でなければなりません。"
            );

            let mean_sec = time_limit_sec / (max_turn * standard_beam_width) as f64;

            // 雑にσ=10%ズレると仮定
            let stddev_sec = 0.1 * mean_sec;
            let variance_sec = stddev_sec * stddev_sec;
            let stddev_state_sec = 0.01 * mean_sec;
            let variance_state_sec = stddev_state_sec * stddev_state_sec;
            let stddev_observe_sec = 0.05 * mean_sec;
            let variance_observe_sec = stddev_observe_sec * stddev_observe_sec;

            Self {
                mean_sec,
                variance_sec,
                time_limit_sec,
                variance_state_sec,
                variance_observe_sec,
                current_turn: 0,
                min_beam_width,
                max_beam_width,
                verbose_interval,
                max_turn,
                warmup_turn,
                current_beam_width: 0,
                start_time: Instant::now(),
                last_time: Instant::now(),
            }
        }

        fn update_state(&mut self) {
            // N(0, α^2)のノイズが乗る
            self.variance_sec += self.variance_state_sec;
        }

        fn update_distribution(&mut self, duration_sec: f64) {
            let old_mean = self.mean_sec;
            let old_variance = self.variance_sec;
            let noise_variance = self.variance_observe_sec;

            self.mean_sec = (old_mean * noise_variance + old_variance * duration_sec)
                / (noise_variance + old_variance);
            self.variance_sec = old_variance * noise_variance / (old_variance + noise_variance);
        }

        fn calc_safe_beam_width(&self) -> usize {
            let remaining_turn = (self.max_turn - self.current_turn) as f64;
            let elapsed_time = (Instant::now() - self.start_time).as_secs_f64();
            let remaining_time = self.time_limit_sec - elapsed_time;

            // 平均値の分散σ^2と観測ノイズβ^2が乗ってくると考える
            let variance_total = self.variance_sec + self.variance_observe_sec;

            // N(ξ, η^2)からのサンプリングをK回繰り返すとN(Kξ, Kη^2)となる（はず）
            let mean = remaining_turn * self.mean_sec;
            let variance = remaining_turn * variance_total;
            let stddev = variance.sqrt();

            // 3σの余裕を持たせる
            const SIGMA_COEF: f64 = 3.0;
            let needed_time_per_width = mean + SIGMA_COEF * stddev;
            let beam_width = ((remaining_time / needed_time_per_width) as usize)
                .max(self.min_beam_width)
                .min(self.max_beam_width);

            if self.verbose_interval != 0 && self.current_turn % self.verbose_interval == 0 {
                let stddev_per_run = (self.max_turn as f64 * variance_total).sqrt();
                let stddev_per_turn = variance_total.sqrt();

                eprintln!(
                "turn: {:4}, beam width: {:4}, pase: {:.3}±{:.3}ms/run, iter time: {:.3}±{:.3}ms",
                self.current_turn,
                beam_width,
                self.mean_sec * (beam_width * self.max_turn) as f64 * 1e3,
                stddev_per_run * beam_width as f64 * 1e3,
                self.mean_sec * beam_width as f64 * 1e3,
                stddev_per_turn * beam_width as f64 * 1e3
            );
            }

            beam_width
        }
    }

    impl BeamWidthSuggester for BayesianBeamWidthSuggester {
        fn suggest(&mut self) -> usize {
            assert!(
                self.current_turn < self.max_turn,
                "規定ターン終了後にsuggest()が呼び出されました。"
            );

            if self.current_turn >= self.warmup_turn {
                let elapsed = (Instant::now() - self.last_time).as_secs_f64();
                let elapsed_per_beam = elapsed / self.current_beam_width as f64;
                self.update_state();
                self.update_distribution(elapsed_per_beam);
            }

            self.last_time = Instant::now();
            let beam_width = self.calc_safe_beam_width();
            self.current_beam_width = beam_width;
            self.current_turn += 1;
            beam_width
        }
    }

    /// ビームサーチ木のノード
    #[derive(Debug, Default, Clone)]
    struct Node<S: SmallState> {
        /// 実行後のsmall_state
        small_state: S,
        /// （N分木と考えたときの）親ノード
        parent: NodeIndex,
        /// （二重連鎖木と考えたときの）子ノード
        child: NodeIndex,
        /// （二重連鎖木と考えたときの）左の兄弟ノード
        left_sibling: NodeIndex,
        /// （二重連鎖木と考えたときの）右の兄弟ノード
        right_sibling: NodeIndex,
    }

    impl<S: SmallState> Node<S> {
        fn new(
            small_state: S,
            parent: NodeIndex,
            child: NodeIndex,
            left_sibling: NodeIndex,
            right_sibling: NodeIndex,
        ) -> Self {
            Self {
                small_state,
                parent,
                child,
                left_sibling,
                right_sibling,
            }
        }
    }

    /// NodeVec用のindex
    /// 型安全性と、indexの内部的な型(u32 or u16)の変更を容易にすることが目的
    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
    struct NodeIndex(usize);

    impl NodeIndex {
        /// 何も指していないことを表す定数
        const NULL: NodeIndex = NodeIndex(!0);
    }

    impl Default for NodeIndex {
        fn default() -> Self {
            Self::NULL
        }
    }

    impl From<usize> for NodeIndex {
        fn from(value: usize) -> Self {
            Self(value)
        }
    }

    impl Into<usize> for NodeIndex {
        fn into(self) -> usize {
            self.0 as usize
        }
    }

    /// Nodeのコレクション
    #[derive(Debug)]
    struct NodeVec<S: SmallState> {
        nodes: Vec<Node<S>>,
        free_indices: Vec<usize>,
    }

    impl<S: SmallState + Default + Clone> NodeVec<S> {
        fn new(capacity: usize) -> Self {
            Self {
                nodes: vec![Default::default(); capacity],
                free_indices: (0..capacity).rev().collect(),
            }
        }

        fn push(&mut self, node: Node<S>) -> NodeIndex {
            let index = self
                .free_indices
                .pop()
                .expect("ノードプールの容量制限に達しました。");

            self.nodes[index] = node;

            NodeIndex::from(index)
        }

        fn delete(&mut self, index: NodeIndex) {
            self.free_indices.push(index.into());
        }
    }

    impl<S: SmallState> Index<NodeIndex> for NodeVec<S> {
        type Output = Node<S>;

        fn index(&self, index: NodeIndex) -> &Self::Output {
            let index: usize = index.into();
            self.nodes.index(index)
        }
    }

    impl<S: SmallState> IndexMut<NodeIndex> for NodeVec<S> {
        fn index_mut(&mut self, index: NodeIndex) -> &mut Self::Output {
            let index: usize = index.into();
            self.nodes.index_mut(index)
        }
    }

    /// 保持する最大ノード数
    const MAX_NODES: usize = 131072;

    #[derive(Debug)]
    pub struct BeamSearch<S: SmallState, G: ActGen<S>> {
        act_gen: G,
        nodes: NodeVec<S>,
        current_index: NodeIndex,
        leaves: Vec<NodeIndex>,
        next_leaves: Vec<NodeIndex>,
        action_buffer: Vec<S>,
    }

    impl<S: SmallState + Default + Clone, G: ActGen<S>> BeamSearch<S, G> {
        /// ビーム木を指定された容量で初期化する
        pub fn new(act_gen: G) -> Self {
            let nodes = NodeVec::new(MAX_NODES);

            Self {
                act_gen,
                nodes,
                current_index: NodeIndex(0),
                leaves: vec![],
                next_leaves: vec![],
                action_buffer: vec![],
            }
        }

        fn init(&mut self, small_state: S) {
            // 参照局所性が高くなるようにお祈りしながらソートする
            self.leaves.sort_unstable();

            while let Some(index) = self.leaves.pop() {
                self.nodes.delete(index);
            }

            self.nodes.push(Node::new(
                small_state,
                NodeIndex::NULL,
                NodeIndex::NULL,
                NodeIndex::NULL,
                NodeIndex::NULL,
            ));

            self.current_index = NodeIndex(0);
            self.leaves.push(NodeIndex(0));
            self.next_leaves.clear();
            self.action_buffer.clear();
        }

        pub fn run<W: BeamWidthSuggester, P: Deduplicator<S>>(
            &mut self,
            mut large_state: S::LargeState,
            small_state: S,
            max_turn: usize,
            mut beam_width_suggester: W,
            mut deduplicator: P,
        ) -> (Vec<S::Action>, S::Score) {
            // 初期化
            self.init(small_state);

            let mut candidates = vec![];
            let mut indices = vec![];

            for turn in 0..max_turn {
                let beam_width = beam_width_suggester.suggest();
                candidates.clear();
                self.dfs(&mut candidates, true, &mut large_state);

                if turn + 1 == max_turn {
                    break;
                }

                assert_ne!(
                    candidates.len(),
                    0,
                    "次の状態の候補が見つかりませんでした。"
                );

                // 重複除去を行ったのち、次の遷移先を確定させる
                indices.clear();
                indices.extend(0..candidates.len());
                glidesort::sort_by_key(&mut indices, |&i| {
                    Reverse(candidates[i].small_state.beam_score())
                });

                deduplicator.clear();
                self.update_tree(
                    indices
                        .iter()
                        .map(|&i| candidates[i].clone())
                        .filter(|c| deduplicator.filter(&c.small_state))
                        .take(beam_width),
                );
            }

            let Cancidate {
                small_state,
                parent,
                ..
            } = candidates
                .into_iter()
                .max_by_key(|c| c.small_state.raw_score())
                .expect("最終状態となる候補が見つかりませんでした。");

            // 操作列の復元
            let mut actions = self.restore_actions(parent);
            actions.push(small_state.action());
            (actions, small_state.raw_score())
        }

        /// ノードを追加する
        fn add_node(&mut self, candidate: Cancidate<S>) {
            let parent = candidate.parent;
            let node_index = self.nodes.push(candidate.to_node(
                NodeIndex::NULL,
                NodeIndex::NULL,
                NodeIndex::NULL,
            ));

            // 親の子、すなわち一番左にいる兄弟ノード
            let sibling = self.nodes[parent].child;

            // 既に兄弟がいる場合、その左側に入る
            if sibling != NodeIndex::NULL {
                self.nodes[sibling].left_sibling = node_index;
            }

            // 兄弟を1マス右に押し出して、自分が一番左に入る
            self.next_leaves.push(node_index);
            self.nodes[parent].child = node_index;
            self.nodes[node_index].right_sibling = sibling;
        }

        /// 指定されたインデックスのノードを削除する
        /// 必要に応じてビーム木の辺を繋ぎ直す
        fn remove_node(&mut self, mut index: NodeIndex) {
            loop {
                let Node {
                    left_sibling,
                    right_sibling,
                    parent,
                    ..
                } = self.nodes[index];
                self.nodes.delete(index);

                // 親は生きているはず
                assert_ne!(parent, NodeIndex::NULL, "rootノードを消そうとしています。");

                // もう兄弟がいなければ親へ
                if left_sibling == NodeIndex::NULL && right_sibling == NodeIndex::NULL {
                    index = parent;
                    continue;
                }

                // 左右の連結リストを繋ぎ直す
                if left_sibling != NodeIndex::NULL {
                    self.nodes[left_sibling].right_sibling = right_sibling;
                } else {
                    self.nodes[parent].child = right_sibling;
                }

                if right_sibling != NodeIndex::NULL {
                    self.nodes[right_sibling].left_sibling = left_sibling;
                }

                return;
            }
        }

        /// DFSでビームサーチ木を走査し、次の状態の一覧をcandidatesに詰める
        /// ビームサーチ木が一本道の場合は戻る必要がないため、is_single_pathで管理
        fn dfs(
            &mut self,
            candidates: &mut Vec<Cancidate<S>>,
            is_single_path: bool,
            state: &mut S::LargeState,
        ) {
            // 葉ノードであれば次の遷移を行う
            if self.nodes[self.current_index].child == NodeIndex::NULL {
                self.act_gen.generate(
                    &self.nodes[self.current_index].small_state,
                    &state,
                    &mut self.action_buffer,
                );

                while let Some(state) = self.action_buffer.pop() {
                    candidates.push(Cancidate::new(state, self.current_index));
                }

                return;
            }

            let current_index = self.current_index;
            let mut child_index = self.nodes[current_index].child;
            let next_is_single_path =
                is_single_path & (self.nodes[child_index].right_sibling == NodeIndex::NULL);

            // デバッグ用
            //let prev_state = self.state.clone();

            // 兄弟ノードを全て走査する
            loop {
                self.current_index = child_index;
                self.nodes[child_index].small_state.apply(state);
                self.dfs(candidates, next_is_single_path, state);

                if !next_is_single_path {
                    self.nodes[child_index].small_state.rollback(state);

                    // デバッグ用
                    //assert!(prev_state == self.state);
                }

                child_index = self.nodes[child_index].right_sibling;

                if child_index == NodeIndex::NULL {
                    break;
                }
            }

            if !next_is_single_path {
                self.current_index = current_index;
            }
        }

        /// 木を更新する
        /// 具体的には以下の処理を行う
        ///
        /// - 新しいcandidatesを葉に追加する
        /// - 1ターン前のノードであって葉のノード（今後参照されないノード）を削除する
        fn update_tree(&mut self, candidates: impl Iterator<Item = Cancidate<S>>) {
            self.next_leaves.clear();
            for candidate in candidates {
                self.add_node(candidate);
            }

            for i in 0..self.leaves.len() {
                let node_index = self.leaves[i];

                if self.nodes[node_index].child == NodeIndex::NULL {
                    self.remove_node(node_index);
                }
            }

            std::mem::swap(&mut self.leaves, &mut self.next_leaves);
        }

        /// 操作列を復元する
        fn restore_actions(&self, mut index: NodeIndex) -> Vec<S::Action> {
            let mut actions = vec![];

            while self.nodes[index].parent != NodeIndex::NULL {
                actions.push(self.nodes[index].small_state.action());
                index = self.nodes[index].parent;
            }

            actions.reverse();
            actions
        }
    }

    #[cfg(test)]
    mod test {
        //! TSPをビームサーチで解くテスト
        use super::{ActGen, BeamSearch, FixedBeamWidthSuggester, NoOpDeduplicator};

        #[derive(Debug, Clone)]
        struct Input {
            n: usize,
            distances: Vec<Vec<i32>>,
        }

        impl Input {
            fn gen_testcase() -> Self {
                let n = 4;
                let distances = vec![
                    vec![0, 2, 3, 10],
                    vec![2, 0, 1, 3],
                    vec![3, 1, 0, 2],
                    vec![10, 3, 2, 0],
                ];

                Self { n, distances }
            }
        }

        #[derive(Debug, Clone, Copy)]
        struct SmallState {
            distance: i32,
            position: usize,
            visited_count: usize,
        }

        impl SmallState {
            fn new(distance: i32, position: usize, visited_count: usize) -> Self {
                Self {
                    distance,
                    position,
                    visited_count,
                }
            }
        }

        impl Default for SmallState {
            fn default() -> Self {
                Self {
                    distance: 0,
                    position: 0,
                    visited_count: 1,
                }
            }
        }

        impl super::SmallState for SmallState {
            type Score = i32;
            type Hash = u64;
            type LargeState = LargeState;
            type Action = usize;

            fn raw_score(&self) -> Self::Score {
                self.distance
            }

            fn beam_score(&self) -> Self::Score {
                // 大きいほど良いとする
                -self.distance
            }

            fn hash(&self) -> Self::Hash {
                // 適当に0を返す
                0
            }

            fn apply(&self, state: &mut Self::LargeState) {
                // 現在地を訪問済みにする
                state.visited[self.position] = true;
            }

            fn rollback(&self, state: &mut Self::LargeState) {
                // 現在地を未訪問にする
                state.visited[self.position] = false;
            }

            fn action(&self) -> Self::Action {
                self.position
            }
        }

        #[derive(Debug, Clone)]
        struct LargeState {
            visited: Vec<bool>,
        }

        impl LargeState {
            fn new(n: usize) -> Self {
                let mut visited = vec![false; n];
                visited[0] = true;
                Self { visited }
            }
        }

        #[derive(Debug, Clone)]
        struct ActionGenerator<'a> {
            input: &'a Input,
        }

        impl<'a> ActionGenerator<'a> {
            fn new(input: &'a Input) -> Self {
                Self { input }
            }
        }

        impl<'a> ActGen<SmallState> for ActionGenerator<'a> {
            fn generate(
                &self,
                small_state: &SmallState,
                large_state: &LargeState,
                next_states: &mut Vec<SmallState>,
            ) {
                if small_state.visited_count == self.input.n {
                    // 頂点0に戻るしかない
                    let next_pos = 0;
                    let next_dist =
                        small_state.distance + self.input.distances[small_state.position][0];
                    let next_visited_count = small_state.visited_count + 1;
                    let next_state = SmallState::new(next_dist, next_pos, next_visited_count);
                    next_states.push(next_state);
                    return;
                }

                // 未訪問の頂点に移動
                for next_pos in 0..self.input.n {
                    if large_state.visited[next_pos] {
                        continue;
                    }

                    let next_dist =
                        small_state.distance + self.input.distances[small_state.position][next_pos];
                    let next_visited_count = small_state.visited_count + 1;
                    let next_state = SmallState::new(next_dist, next_pos, next_visited_count);
                    next_states.push(next_state);
                }
            }
        }

        #[test]
        fn beam_tsp_test() {
            let input = Input::gen_testcase();
            let small_state = SmallState::default();
            let large_state = LargeState::new(input.n);
            let action_generator = ActionGenerator::new(&input);
            let mut beam = BeamSearch::new(action_generator);

            // hashを適当に全て0としているため、重複除去は行わない
            let deduplicator = NoOpDeduplicator;
            let beam_width = FixedBeamWidthSuggester::new(10);

            let (actions, score) =
                beam.run(large_state, small_state, input.n, beam_width, deduplicator);

            eprintln!("score: {}", score);
            eprintln!("actions: {:?}", actions);
            assert_eq!(score, 10);
            assert!(actions == vec![1, 3, 2, 0] || actions == vec![2, 3, 1, 0]);
        }
    }
}
mod problem {
    mod atcoder_judge {
        use proconio::input_interactive;

        use super::{Judge, Measure, Op};

        #[derive(Debug, Clone)]
        pub struct AtCoderJudge {
            query_cnt: usize,
        }

        #[allow(dead_code)]
        impl AtCoderJudge {
            pub fn new() -> Self {
                Self { query_cnt: 0 }
            }
        }

        impl Judge for AtCoderJudge {
            fn query(&mut self, ops: &[Op]) -> Measure {
                self.query_cnt += 1;

                println!("{}", ops.len());

                for op in ops {
                    println!("{}", op);
                }

                input_interactive! {
                    width: u32,
                    height: u32,
                }

                Measure::new(height, width)
            }

            fn query_cnt(&self) -> usize {
                self.query_cnt
            }

            fn rects(&self) -> Option<&[super::Rect]> {
                None
            }
        }
    }
    pub mod params {
        use nalgebra::{DMatrix, DVector, DVectorView};
        use std::{cell::RefCell, env, fmt::Display, rc::Rc, str::FromStr};

        thread_local! {
            static PARAMS: Rc<RefCell<Params>> = Rc::new(RefCell::new(Params::from_env()));
        }

        #[derive(Debug, Clone)]
        pub struct Params {
            pub arrange_count: usize,
            pub query_annealing_duration_sec: f64,
            pub mcmc_init_duration_sec: f64,
            pub beam_mcts_duration_ratio: f64,
            pub mcmc_duration_ratio: f64,
            pub mcts_turn: usize,
            pub mcts_expansion_threshold: usize,
            pub mcts_candidates_count: usize,
            pub parallel_score_mul: f32,
            pub width_buf: f64,
            pub ucb1_tuned_coef: f32,
            pub touching_threshold: usize,
            pub invalid_cnt_threshold: usize,
        }

        impl Params {
            pub(super) fn new(n: usize, t: usize, sigma: f64) -> Self {
                let arrange_count = ParamSuggester::gen_arrange_count_pred(t)
                    .suggest(n, t, sigma)
                    .round() as usize;
                let query_annealing_duration_sec =
                    ParamSuggester::gen_query_annealing_duration_sec().suggest(n, t, sigma);
                let mcmc_init_duration_sec =
                    ParamSuggester::gen_mcmc_init_duration_sec().suggest(n, t, sigma);
                let beam_mcts_duration_ratio =
                    ParamSuggester::gen_beam_mcts_duration_ratio().suggest(n, t, sigma);
                let mcmc_duration_ratio =
                    ParamSuggester::gen_mcmc_duration_ratio().suggest(n, t, sigma);
                let mcts_turn =
                    ParamSuggester::gen_mcts_turn().suggest(n, t, sigma).round() as usize;
                let mcts_expansion_threshold = ParamSuggester::gen_mcts_expansion_threshold()
                    .suggest(n, t, sigma)
                    .round() as usize;
                let mcts_candidates_count = ParamSuggester::gen_mcts_candidates_count()
                    .suggest(n, t, sigma)
                    .round() as usize;
                let parallel_score_mul =
                    ParamSuggester::gen_parallel_score_mul().suggest(n, t, sigma) as f32;
                let width_buf = ParamSuggester::gen_width_buf().suggest(n, t, sigma);
                let ucb1_tuned_coef =
                    ParamSuggester::gen_ucb1_tuned_coef().suggest(n, t, sigma) as f32;
                let touching_threshold = ParamSuggester::gen_touching_threshold()
                    .suggest(n, t, sigma)
                    .round() as usize;
                let invalid_cnt_threshold = ParamSuggester::gen_invalid_cnt_threshold()
                    .suggest(n, t, sigma)
                    .round() as usize;

                Self {
                    arrange_count,
                    query_annealing_duration_sec,
                    mcmc_init_duration_sec,
                    beam_mcts_duration_ratio,
                    mcmc_duration_ratio,
                    mcts_turn,
                    mcts_expansion_threshold,
                    mcts_candidates_count,
                    parallel_score_mul,
                    width_buf,
                    ucb1_tuned_coef,
                    touching_threshold,
                    invalid_cnt_threshold,
                }
            }

            #[allow(dead_code)]
            pub(super) fn from_env() -> Self {
                let arrange_count = get_env("AHC_ARRANGE_COUNT", 10);
                let query_annealing_duration_sec = get_env("AHC_QUERY_ANNEALING_DURATION_SEC", 0.3);
                let mcmc_init_duration_sec = get_env("AHC_MCMC_INIT_DURATION_SEC", 0.1);
                let beam_mcts_duration_ratio = get_env("AHC_BEAM_MCTS_DURATION_RATIO", 0.5);
                let mcmc_duration_ratio = get_env("AHC_MCMC_DURATION_RATIO", 0.1);
                let mcts_turn = get_env("AHC_MCTS_TURN", 15);
                let mcts_expansion_threshold = get_env("AHC_MCTS_EXPANSION_THRESHOLD", 3);
                let mcts_candidates_count = get_env("AHC_MCTS_CANDIDATES_COUNT", 4);
                let parallel_score_mul = get_env("AHC_PARALLEL_SCORE_MUL", 0.9);
                let width_buf = get_env("AHC_WIDTH_BUF", 1.1);
                let ucb1_tuned_coef = get_env("AHC_UCB1_TUNED_COEF", 1.0);
                let touching_threshold = get_env("AHC_TOUCHING_THRESHOLD", 16);
                let invalid_cnt_threshold = get_env("AHC_INVALID_CNT_THRESHOLD", 10);

                Self {
                    arrange_count,
                    query_annealing_duration_sec,
                    mcmc_init_duration_sec,
                    beam_mcts_duration_ratio,
                    mcmc_duration_ratio,
                    mcts_turn,
                    mcts_expansion_threshold,
                    mcts_candidates_count,
                    parallel_score_mul,
                    width_buf,
                    ucb1_tuned_coef,
                    touching_threshold,
                    invalid_cnt_threshold,
                }
            }

            pub fn get() -> Rc<RefCell<Self>> {
                PARAMS.with(|p| p.clone())
            }
        }

        impl Display for Params {
            fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
                writeln!(f, "arrange_count: {}", self.arrange_count)?;
                writeln!(
                    f,
                    "query_annealing_duration_sec: {}",
                    self.query_annealing_duration_sec
                )?;
                writeln!(f, "mcmc_init_duration_sec: {}", self.mcmc_init_duration_sec)?;
                writeln!(
                    f,
                    "beam_mcts_duration_ratio: {}",
                    self.beam_mcts_duration_ratio
                )?;
                writeln!(f, "mcmc_duration_ratio: {}", self.mcmc_duration_ratio)?;
                writeln!(f, "mcts_turn: {}", self.mcts_turn)?;
                writeln!(
                    f,
                    "mcts_expansion_threshold: {}",
                    self.mcts_expansion_threshold
                )?;
                writeln!(f, "mcts_candidates_count: {}", self.mcts_candidates_count)?;
                writeln!(f, "parallel_score_mul: {}", self.parallel_score_mul)?;
                writeln!(f, "width_buf: {}", self.width_buf)?;
                writeln!(f, "ucb1_tuned_coef: {}", self.ucb1_tuned_coef)?;
                writeln!(f, "touching_threshold: {}", self.touching_threshold)?;
                writeln!(f, "invalid_cnt_threshold: {}", self.invalid_cnt_threshold)
            }
        }

        // パラメータチューニングその1
        const N1: &[u8] = b"Fl/xFV/xtT8d1EEd1EGtP8VXfMVXfOU/6qAO6qAO6j/btm3btm3bP8VXfMVXfOU/fMVXfMVX3D+amZmZmZm5P9u2bdu2bcs/JUmSJEmS5D9CHdRBHdThP6EO6qAO6uA/HdRBHdRB7T+w+Iqv+IrvP83MzMzMzOw/kiRJkiRJ0j+w+Iqv+IrvPzuogzqog+o/HdRBHdRBrT9YfMVXfMXXP+qgDuqgDuo/LL7iK77i6z+3bdu2bdvmP/EVX/EVX+E/mpmZmZmZ2T9YfMVXfMXnP5qZmZmZmek/kiRJkiRJwj9mZmZmZmbmPxZf8RVf8cU/hDqogzqo4z9YfMVXfMXXP5IkSZIkSdI/HdRBHdRBvT+hDuqgDurgP4uv+Iqv+Oo/6qAO6qAO6j/UQR3UQR3EP7dt27Zt2+Y/X/EVX/EVzz9QB3VQB3XgP5IkSZIkScI/+Yqv+Iqv6D+SJEmSJEniPzMzMzMzM9M/UAd1UAd1wD9QB3VQB3XgP1/xFV/xFe8/fMVXfMVX7D+SJEmSJEmyP7dt27Zt29Y/UAd1UAd1wD8AAAAAAADgP/EVX/EVX+E/MzMzMzMz4z+amZmZmZm5PzMzMzMzM9M/O6iDOqiD2j/btm3btm3rP1h8xVd8xdc/27Zt27Zt6z+amZmZmZnJP27btm3btu0/dVAHdVAH5T8P6qAO6qDuP1AHdVAHddA/mpmZmZmZuT9u27Zt27btP/EVX/EVX9E/mpmZmZmZyT+SJEmSJEniP4Q6qIM6qOM/X/EVX/EVzz++4iu+4ivuPw==";
        const T1: &[u8] = b"k2DD4m+s3L/CU5lGkvrqP80YAXd+r8W/cRodhGTk6z8sPbRVMqrgP2iapt7KPNM/Yw21Xila3D8Em7cuz03lPwj8awCFXd6/G+rBNVJ58D+R2zQRYmnSP51eEMEtmPM/ktkJmojJ6z+SFkS7i6rhvzbGmVqqpcY/zG2Yr7Gu2L+uNzc+ZtzkP+TgEmghmLM/9RMjIAvG8D9vZP8B3OfQvygzznoMis+/RiLz124I7z94Ogl+RizQPwj+72zOj8E/TJi/7CPz2T/xNZuRWaDRP73c1BJlut4/zyYnGooU3j/J9fkmOavgP4CzUUeIhO4/2Efx9osP5L8RCCR1Evnjv/EWsxbOl9q/2zaZ1o5J2L//3DL2WHSnv6+GlbQTPNm/lG83Uro12T+XrRZk8v3Tv8rErlDUgus/CwOteuqT8T/ikQx8hZ/zPz4ZzFsdQ6q/ZuArt+Y09D9Zsb9wcYPhP7a5uMOIdsA/X8ebsdX+uD+sZw8zuA/Tv480E9nCiOW/mpxHhUSk8z9kAi00kYzjv0wbO1FUBe8/iyHFFQF/5D+LpH8DjgHpP5p2sFND7vI/ziYnGooU3r8v49gy7KzoP0tV3qR4798/ixIJnuI1kT9RFIH6rljwP/PaJj4MlOc/TPQM95R27T8bivukYcKmP2a/WDWPsPE/r1Rt+c9zsz9VxQTNEs7gP+PZijztk8q/Pv5lGQOF4j9cu1r9CFnWv9flxxbB9+0/K52o6XZl47/RQHBeX+jhv51IDnlLGOg/v8g5jxue4D/CXhzCi2HUPw==";
        const SIGMA1: &[u8] = b"FBtVwmI25T/NhaQpFWfrP3BeTTwrGu0/buydgprl2z+ASXkPDG/oP5vlYxX6Ea0/bJsxLpFa5j/SlIqzD5/lP452K637Ftc/LR+r0I/o2j/fT42XbhLjP5KmVJx9+Ow/cWPvFNQs6z9TKs4+fIbVPzhCGb0ta+0/d3d3d3d36z/45tXEs6LBP5aPVehHdO0/q9CP6Npm6D/ZqBIWs+nlP+ZjFfoRXcs/XI/C9Shc1z9eJ1ft6UzeP+HBPFH/Ruw/VqEf0bXNqD9OYhBYObToP02pOPvwGbY/ltZ9i//Z0j+/WPKLJb/YPzn78Bl2Bcg/tA+fYVeAzD/n+6nx0k3SPxtVwmI2Pe8/xI29U1CzvD8LtmALtmDTP+MSqaUI0u0/+hFd24xx7T/vgeENB83nP1S9wF2UZOY/kceXAQWi2D92Bci95kKiPwu2YAu2YO8/wYN5ov6N2D/iM+wKkHvNPxyhjN6WteI/4C5KMudo4z967sgJsb7CP7lqT2eyMNI/lKv2dCYL6z+BuyjJnKPdP2So7DB1ue0/sEyDU73AvT/MpueOnBDjP4Y3HDR//dY/2vTckRNi6T+uR+F6FK7fP+mTPumTPtE/ZRqc6gXu7j9GtvP91HjRPz5WoR/RtdU/juM4juM43j+grSH8POTlP8CkvAeGN9Q/v+vkqj2d7T+W1n2L/9mKP+sr0+BUL+w/pHA9Ctej4D9y1Z7OZGHUP7Q1hJ+HPOo/rPZ0Jgsjzj9d24xxidTaPyfE+so0OMU/xPrKNDjV0z/xrGgk4JvnPw==";
        const ARRANGE_COUNT: &[u8] = b"AAAAAAAAKEAAAAAAAAAiQAAAAAAAACBAAAAAAAAAKEAAAAAAAAAoQAAAAAAAABhAAAAAAAAAJkAAAAAAAAAmQAAAAAAAACRAAAAAAAAAJEAAAAAAAAAUQAAAAAAAACJAAAAAAAAAJEAAAAAAAAAkQAAAAAAAABRAAAAAAAAAIEAAAAAAAAAcQAAAAAAAABRAAAAAAAAAGEAAAAAAAAAcQAAAAAAAABhAAAAAAAAAIkAAAAAAAAAcQAAAAAAAABxAAAAAAAAAHEAAAAAAAAAYQAAAAAAAACBAAAAAAAAAGEAAAAAAAAAmQAAAAAAAABhAAAAAAAAAIEAAAAAAAAAmQAAAAAAAACRAAAAAAAAAGEAAAAAAAAAiQAAAAAAAABxAAAAAAAAAJEAAAAAAAAAiQAAAAAAAACZAAAAAAAAAJkAAAAAAAAAgQAAAAAAAABhAAAAAAAAAIkAAAAAAAAAgQAAAAAAAABxAAAAAAAAAGEAAAAAAAAAgQAAAAAAAABxAAAAAAAAAIkAAAAAAAAAmQAAAAAAAACxAAAAAAAAAIEAAAAAAAAAiQAAAAAAAACJAAAAAAAAAGEAAAAAAAAAYQAAAAAAAACRAAAAAAAAAHEAAAAAAAAAgQAAAAAAAACJAAAAAAAAAKEAAAAAAAAAUQAAAAAAAACBAAAAAAAAAGEAAAAAAAAAYQAAAAAAAACRAAAAAAAAAGEAAAAAAAAAcQAAAAAAAACJAAAAAAAAAJkAAAAAAAAAkQAAAAAAAACxAAAAAAAAAGEAAAAAAAAAoQA==";
        const QUERY_ANNEALING_DURATION_SEC: &[u8] = b"95Ax5HV6uz+3nNK9X8nOP6+Jp+1vPsM/8Z35ADfkzz81Wvg+2CzTP8OVCxg25sQ/vHuCZ4ks0D/TCxE5KYvGPzMzMzMzM9M/7Bdt7yu1wD8qLNiB7PDHP1bxcEKBgMo/9FFvDmzZzj8+CvboZyTGP9e20LPpps4/91pqZ50SxD9Y1BNo1ZvRP4YswqmhjNA/Dg7xZMCpwT/Y0/SeXUixPzpZhzJs5sg/cgXTCFsGxj9F+ZmeXg3EP1pmguSRfsw/7uB5HK3pyz8FC+w0drjQPx0CRSC26sk/wuvkd5gazj//deaVL9PGP73g1UI43bc/VpLUTGhZzj/ZdzaXJzDJP9ioFYvi/sI/lhvwSFUUxj/0o2dmntjDPweMqrc+l6s/CjDJjdBA0T/aKEeigMPSP89BEJPe6s8/WahZoGRwsz9jals8llWtPx67V3qUTr4/zBrTzQPnzz9YjBMe6HvSP2IEKEEUt9E/1R0pElKG0j+ajEe+hyq7P1cVEKGEic0/SnsNoo7U0D8uSWeXbx+6P36tw90I0bs/NMWh/G5Wzj+OG0vxkSzOP52BphNFOcQ/CIZpPYUYwz9FcvPt6VXKP0A78trSXNE/GkAgin5v0T/QLFzG0u7DP7CF+l1XXsc/P2Z9DtHEtj/px5df4ce2Py80Kc9UX9A/ys5mmUZ00D+nk5QbmXrOPzMzMzMzM9M/Ila+UBDBzD+87emTYL7QP8qFPN6sFMg/AUybWbRGrj/KWocZz3zBP+I3brKyc7E/sU1IqXZ+zj+YgVDeSbjOPw==";
        const MCMC_INIT_DURATION_SEC: &[u8] = b"ScJ8YM4psT/tiBZJzAO1P95kK0vp+Lg/hHAk6uejwj+GM/ppmRDDP/mO2FYjpa0/H3LT0KcXsT+T3LXX4neyP5qZmZmZmbk/1qQ4gi/Brj837NwNXL+2PyuN1iDBSb0/YC4zKvUFwj9sS6fzPE+4PwugocifsLY/Cc+W9IaptT/hs8Apu5uwPxEyelGDGbw/X2dHaC+QwD/gw5mNkXK2P9W2kz7xKcA/XsE4GHxdtD+6W3DJswaxP4WCsGULnbg/vSuxHGQCuD+M263c7P28PzHSjy4cYLg/v83e5/ZrvD8uYDZZpQ++P8sVK0jZzrs/ZwGSG9musT+SGcpMoFW6P1Td1zhgarQ/nZwCByRNrj+T4Tx3Zzq1P+nPDyITB68/xB1QdS+BtD+iQI3qDGWxP/UazHkynMI/z6FArLIQsj+P+24KSayzP8tWUETXJro/qOAjVBsBvT/AuPqgDGe2P4xovpqkT8E/nuQCBXmyuj9mHdtszXOwP9J7WP7nV7A/RIAUs08dsD+ySDg2wrG7P695WTnZm7M/BEWKB2d0wj+AnfkMLBW4P/EagZ8tA8A/VWO0Be3kuj9FnXssE8C4P+AU82HfdrM/PoSVHd1Ztz9QEdVNygXAP2IJjvsOAMM/cHN14eh+wT+x31ZMqZiyP1wWaoXD6cE/eKf9+IsRwj9JjbHRRnW0P5qZmZmZmbk/eY8/Axvluz8QXz8+hrC6Pwt9LNwHX7o/Wv6XZ+l4vz/9Nl/mLxCxP8tl37tvArU/zeqq3llTvD+sBfsJGLq/Pw==";
        const BEAM_MCTS_DURATION_RATIO: &[u8] = b"TjrjHFMH5T8HtloAoLjfP9RGpI5Q5eE/4MAPWcRQ4T/09y/8zq3fPzdQvh3b3OU/FNdbk7TD5D8CXNliZTvfPwAAAAAAAOA/p+IipVA14D8CRALIcKvWPxjleqj4cOU/UtfUJyZ/4z+Fn3ReYezjP2QLJMJRBd4/G+f0G+q/4D/+x4Lg4nTiPzi37xmr5t0/jwwsEsDs3j/jcvh7t1TgP7YS6p/OaeI/EEqmjz1t4j+BMmepkDLfPz6MonZvNdg/yigehuFV2z882OKbT4HTP/NYFSGDIeI/aTvk5hh63z8CY4DVWoPjP1w4kZ/MUto/gFaiJw8x4z/alGc7xNLdP2kgLcBW9uM/5apDlfQF5D+n15cBCtvgP45mtkPz/N8/O+EbGfoD5j9r91SjjV3kPyqTKdnL+uU/RayeUyjQ5D9T44jEHdHeP+X2OX7ZYOM/yZrLoIw85D87/68a7wzgP3rCqFiPXuE//aW5xzgQ4T8RRUkxUEfaP5nF8tC6Sd8/Ud9Uu78y5j8ai3XrjL/lP18NBl9AXdw/5IiPyP+g2j9SctT8LQPkP7y8SgwDCOA/198WWm+M4j9U/OJ3BA/gP0Df3CsYxt4/0BTPKwzx3T+FWTtYe6fhP8rB1DlBm+A/Vo0fjl7M5T/ScuktHqTkPxKQK+0ecOA/zfD/f67/3D90Sxg/N5XfPwAAAAAAAOA/1gHKTz9n4T+YiGAZoAPhPynC1fa66uM/IxffeBRd4T8nnHYBPCHgPwtYEnViM+E/OlM3nhl82j9D7ywEKbPjPw==";
        const MCMC_DURATION_RATIO: &[u8] = b"I4mDuInsxT9rK780xTO+P+NVThxkZME/lC6bTV4MwD/qnEJTLvCwP5xocJQSrbM/eLcdwlfQwj/goDqyBIXAP5qZmZmZmbk/kLqKgfEBsj8rEp/7LnWqPxfsq22AIq4/IMQ5aJLHxT/vVumQZmCxP+V6uf74Cro/1ktLT1hTuD+65OlMu3G9P7qeu1jy1sE/1IifptGZsD9yZh8Eu+CtP62inbvOfaA/8h3PaQpMuz8Dx0L2VAaxP3EoH3X8bsk/UTg5GqIbwj/cm/WnRkWtP2vDCv4EzbA/igTa2h0CqD+mKNrurcqzP58VAsOwZ7s/EvBA31vKrj/JN+07PUO9PxRx7RIa1cE/eIl1BIierz8QXdF1H0C0P0hA01NOeqw/MBr2MknXuD9SRVwv27q/P3ZnPLpeM78/lAq5gQqTrT8XokQ5XOW4P7RlJVaj1Lo//WV9oDNXtT/25v2xB2+zP6VnxA2Hzac/PpGYp3L4wj+E3SMES1mtP2tpEumCZ6g/Nrn/gjz6vj+uMPcBp6XAP93wieGTM8Q/dp5jYJ2Brz98hNtyJKHGPyM99CRugrM/lsGMapy9oj/V/zm2G+C1P6u355rbT74/HFj+3SvWxz9L6eOJbEusP+o9laUJ3qU/8LD3QHgstz+2pIiXDoLJP6QUMuBBX8Y/eK3zTZKCwj+CZCIWrPKxP5qZmZmZmbk/LQ+8ugfWuz9QsgIvYjvJPxd+SdD8Ybs/G9PpNw8EyD/Wu3MaoOnBP0y0BxWIjas/qa5vpEaxvz+oRB7w63G7Pw==";
        const MCTS_TURN: &[u8] = b"AAAAAAAALEAAAAAAAAAyQAAAAAAAADNAAAAAAAAAM0AAAAAAAAAwQAAAAAAAACZAAAAAAAAAMkAAAAAAAAAsQAAAAAAAAC5AAAAAAAAAJkAAAAAAAAAxQAAAAAAAADJAAAAAAAAAKEAAAAAAAAA2QAAAAAAAADNAAAAAAAAAMEAAAAAAAAAqQAAAAAAAADNAAAAAAAAAMkAAAAAAAAAwQAAAAAAAADRAAAAAAAAAKEAAAAAAAAAzQAAAAAAAADNAAAAAAAAAMkAAAAAAAAA1QAAAAAAAAChAAAAAAAAALEAAAAAAAAAoQAAAAAAAACxAAAAAAAAAM0AAAAAAAAAwQAAAAAAAADdAAAAAAAAALEAAAAAAAAAyQAAAAAAAADRAAAAAAAAAKkAAAAAAAAAuQAAAAAAAAChAAAAAAAAALkAAAAAAAAAxQAAAAAAAADVAAAAAAAAAKEAAAAAAAAAzQAAAAAAAADBAAAAAAAAALEAAAAAAAAAyQAAAAAAAADVAAAAAAAAAJkAAAAAAAAAoQAAAAAAAADBAAAAAAAAALEAAAAAAAAAyQAAAAAAAADFAAAAAAAAANEAAAAAAAAAuQAAAAAAAADBAAAAAAAAAMUAAAAAAAAAoQAAAAAAAAC5AAAAAAAAAJkAAAAAAAAAsQAAAAAAAAChAAAAAAAAAMkAAAAAAAAAsQAAAAAAAAC5AAAAAAAAAM0AAAAAAAAAqQAAAAAAAADBAAAAAAAAALEAAAAAAAAAzQAAAAAAAACZAAAAAAAAALEAAAAAAAAAoQA==";
        const MCTS_EXPANSION_THRESHOLD: &[u8] = b"AAAAAAAAEEAAAAAAAAAIQAAAAAAAAABAAAAAAAAAAEAAAAAAAAAAQAAAAAAAAAhAAAAAAAAAFEAAAAAAAAAIQAAAAAAAAAhAAAAAAAAAFEAAAAAAAAAAQAAAAAAAABBAAAAAAAAACEAAAAAAAAAAQAAAAAAAABRAAAAAAAAAFEAAAAAAAAAIQAAAAAAAABRAAAAAAAAAEEAAAAAAAAAAQAAAAAAAAABAAAAAAAAA8D8AAAAAAAAQQAAAAAAAAAhAAAAAAAAACEAAAAAAAAAQQAAAAAAAAPA/AAAAAAAACEAAAAAAAAAAQAAAAAAAABBAAAAAAAAAFEAAAAAAAAAIQAAAAAAAABRAAAAAAAAACEAAAAAAAAAIQAAAAAAAABRAAAAAAAAA8D8AAAAAAAAIQAAAAAAAAABAAAAAAAAACEAAAAAAAAAIQAAAAAAAABRAAAAAAAAACEAAAAAAAAAUQAAAAAAAABRAAAAAAAAAAEAAAAAAAAAAQAAAAAAAABBAAAAAAAAAFEAAAAAAAAAQQAAAAAAAAAhAAAAAAAAAAEAAAAAAAAAIQAAAAAAAABRAAAAAAAAAEEAAAAAAAAAUQAAAAAAAAAhAAAAAAAAACEAAAAAAAADwPwAAAAAAAAhAAAAAAAAACEAAAAAAAAAIQAAAAAAAAPA/AAAAAAAA8D8AAAAAAADwPwAAAAAAAAhAAAAAAAAAEEAAAAAAAAAAQAAAAAAAABBAAAAAAAAAEEAAAAAAAAAIQAAAAAAAABBAAAAAAAAACEAAAAAAAAAQQA==";
        const MCTS_CANDIDATES_COUNT: &[u8] = b"AAAAAAAAEEAAAAAAAAAIQAAAAAAAABRAAAAAAAAACEAAAAAAAAAIQAAAAAAAABRAAAAAAAAACEAAAAAAAAAIQAAAAAAAABBAAAAAAAAAEEAAAAAAAAAIQAAAAAAAAAhAAAAAAAAAEEAAAAAAAAAQQAAAAAAAAAhAAAAAAAAAFEAAAAAAAAAQQAAAAAAAAAhAAAAAAAAAEEAAAAAAAAAQQAAAAAAAAAhAAAAAAAAACEAAAAAAAAAQQAAAAAAAAAhAAAAAAAAACEAAAAAAAAAIQAAAAAAAABRAAAAAAAAAEEAAAAAAAAAQQAAAAAAAABhAAAAAAAAACEAAAAAAAAAIQAAAAAAAABBAAAAAAAAAEEAAAAAAAAAIQAAAAAAAABRAAAAAAAAACEAAAAAAAAAQQAAAAAAAAAhAAAAAAAAACEAAAAAAAAAQQAAAAAAAABBAAAAAAAAACEAAAAAAAAAIQAAAAAAAABBAAAAAAAAAEEAAAAAAAAAIQAAAAAAAAAhAAAAAAAAAFEAAAAAAAAAQQAAAAAAAABBAAAAAAAAAFEAAAAAAAAAQQAAAAAAAAAhAAAAAAAAAFEAAAAAAAAAQQAAAAAAAABBAAAAAAAAAEEAAAAAAAAAQQAAAAAAAABBAAAAAAAAAEEAAAAAAAAAQQAAAAAAAAAhAAAAAAAAAEEAAAAAAAAAQQAAAAAAAABBAAAAAAAAACEAAAAAAAAAIQAAAAAAAAAhAAAAAAAAAEEAAAAAAAAAQQAAAAAAAAAhAAAAAAAAACEAAAAAAAAAQQA==";
        const PARALLEL_SCORE_MUL: &[u8] = b"aWYdM2Rz6z/hbX1aJqnrP7xjGEhjxe4/SpZ7jx7k5z+HyR0E2oXqP4IoC9ZfYOg/8jGIaDGR5z+JlHqWA9vtP83MzMzMzOw/9nog2ahW5z8U4UArloXoP+aiPGMzeOc/LlKyoEOf6D9nv2cUl2vuP+NaaeRY4O4/RMH6kYMh7j8yTO7BmpXsP6HhRLP18e0/4gqjYlAJ6j+oxD1/vvfsPwCad650uOo/FnK7Q1d36z8Lgp3he23sPwW1EWMuNuo/CIxVHKQZ7z8UCXv9JNzvP4RMEl7MlO0/oB+ShcY+7T+6VVWccIjrP7o5YMsRnuw/bLoA5zId6z/ljU0xMLvsP9YAWxlf6ug/S2RZHyj57T+rlAmKz1DoP67T/2Q1ROk/bj1KWltz6D+u3UmemaHsP+VtlzIMmOw/q1YV3Dr27D/Qis13LTrvP36DLMrJ7Og/eFfr23RI7T/U6MjQDGLoP+3WJu+pdOw/ESH1+m467j9bKrv87k3uPzextb57Ze0/0H8DrhKt5z9qJi5KGtXpP9bji4ZY+ec/zUCA6KOy6z9iKWhu0QPuP6FRv3c/9OY/OcCdDD2f7z9+e4bOQ1PnP3EwNmNqCuk//pN6pWHh7D/ZHfrukHfoP3S/qDza+es/CWHev/Bt6D9eMXcgabjpP0vQ/6Lr++c/Mw7xzpjW6j+bNnDEbWXrP83MzMzMzOw/htsY65zh6T8gDuZfsMvsP4n0AbRuSe0/guNVgNIH6j9gSusR8bXtP7G0TkzAGOk/JTo7e1kJ7D/oWNtvElDtPw==";
        const WIDTH_BUF: &[u8] = b"BX8871YY8j+SE/XQE1PxPyUSAP24KvI/BBm8L8u68D9/dis9jMPxP2Ik4wZsGPE/xH//jKK18T9hL1iHRkfxP5qZmZmZmfE/y+ETjk+X8T8oXZ1V3a3xPw5qGGx1mvE/qmAxM2KK8D/ATT7OiZzxP/axm/HFMPE/upGsEzWH8T+jjL46/y/xP2ATupOYEPI/ijoaqifn8D9SRrdenlHyPzumifPV0vE//HAIME1R8T9zz1ZqlTrxPzc9zETn/fE/Z+yxwafQ8D89ciSGk3DxP+D+kYxb8vA/qy/CXHuI8T9QF+u8vznxPx+KB+1p1vA/C9HmyMJu8T/lZxf4urLxP6qzoO6oZfI/Oh3pt5WG8T98zbaVWxjxP1eXvAJayvE/AmaGu7lt8T+ho14l//nxPwuBV2N+pvA/KzDWxILF8D/azsKcTXjwP3qHrU2OxPE/wd0iyjZj8D+TKRg9bJ7wP039uG9pHfE/K1cOG1ps8T+SmO4YNvXxP9E90EX1vPE/vEW5rZLE8D9/VqFTNUrxP72px1tpCfE/EHL3AibN8D+GU8OlXXfxP7QkbLWH9PA/p/Fzgy9j8j/fSTjTQrbxPzs55eSs1/E/ZIifaYP18T/b/x4FE87wP5PfbHKpmfE/jSkDbckb8T9KfIk9rdDxPz2hRvtNEvE/pQZfCCdj8T8n0NyO8jrxP5qZmZmZmfE//1QmoTXC8T9xFkpSHqDxP0crAfT2ufE/FCllMwEm8T9RU8qaS6vxP0aimCW98fA/sutf19ES8T8+4Mkj2xfyPw==";
        const UCB1_TUNED_COEF: &[u8] = b"0NEtiC9D5z81OaQZ8QzQP0BA0nhkytM/byT+hef7rj/I2kVpeA3uP4Umj2YWYrg/ytNOgz7NxD/dSfBxsBHjPwAAAAAAAOA/d/SSv3mI3j9eaxY7wWjhP+DuJSLyM8U/gHplJlBxxj+e+bkf+RLRP+5VKGh96Nc/D/LtE6f55D+/lbjI5LvAP1YEdP/yJds/ZedwecNsyD9tR46mczvcP7/xQSEnWL8/xf9trNJZ5D8+bo0a9nS8Pwtc50qaJNE/bg3tTf7Wwz8+Ypu6CZjNP6WvzcwxAMQ/V2T3y0Da2z/sikbdKUK/P5Fg1Evat9A/QAMuHWjyzT/ihMmM0JrcPzWnnRAKpeQ/vdcEhfTB2j+3T0tTq4HEP2uHpupitN8/YNoxAWtK6T/aLUgArejhP6DjEU+N8eo/4/1Vy9OUxD9+WqU3uI21P3vqGFDB690/zoOLZdcR4D9EmV+yjRC/P6vbK1Noido/alNCubcn3z+Ur0nJpvTDP9bWCrpIlMk/q2iS46/Dsz8j9GQPnnTiPxhzIXA118o/LsRdJEmeyj+AuXks7fyxP5gO/rvdocM/GmmPe1Qb5T8Kvaimuw/bP31dLYtwXsQ/YZ63SjHJzT8n007MyS2/PxqKVNzlb9s/mRyuXSJu6D+Gr6ZBKY7gPxX8aootb9s/3vsvUMy/zj/vYZJOz1KvPwAAAAAAAOA/zPxBxhiPzz99ljGjKYDlPxkatL5RM+A/+4RsNbZG2z8LgJ2feT7dPzNopt1JmdY/4QJz/6Rt4T9e8TYbuUTDPw==";
        const PARAM_ARRANGE_COUNT: &[u8] = b"jRBJnsXimj9wgcTetawKQIcxcuL8y7A/NCUOC8Bw+j8=";
        const PARAM_QUERY_ANNEALING_DURATION_SEC: &[u8] =
            b"7A62czmYhD/GjKRcUWyHPz2Fb6tOYOY/VSGPVoWIhD8=";
        const PARAM_MCMC_INIT_DURATION_SEC: &[u8] = b"8BWA5jGPjz+moQ71mJOEP2dbHJFP8u8/+GYKJceEhD8=";
        const PARAM_BEAM_MCTS_DURATION_RATIO: &[u8] =
            b"b3ogiF+OhD9RRq4VLFyKP9sih4iUQ+0/L+rx20SGhD8=";
        const PARAM_MCMC_DURATION_RATIO: &[u8] = b"R6R94sHBiT/DaGF8jlaOP1gBiEh1pe8/xIaGuSqGhD8=";
        const PARAM_MCTS_TURN: &[u8] = b"e6ZVr0qEnz9RfXVpEPMcQPV2DK0dk+o/rHilJnNUFUA=";
        const PARAM_MCTS_EXPANSION_THRESHOLD: &[u8] =
            b"3oDEaG7ThD9+42LRvRf2PyZ/9efGWYw/29vFm0ncpz8=";
        const PARAM_MCTS_CANDIDATES_COUNT: &[u8] = b"i4Q0dFZzhz/Hz4A2vy/QP65UrBlRXcA/NU+sS//I0D8=";
        const PARAM_PARALLEL_SCORE_MUL: &[u8] = b"WrcOSSP4hD/mENpZ046EP9NDoYc4Wu8/8dfS3NuQhD8=";
        const PARAM_WIDTH_BUF: &[u8] = b"QCLB65GMhj/N8DHQy/mLP5d6NK2Hw+w/VqFp6Bp+hD8=";
        const PARAM_UCB1_TUNED_COEF: &[u8] = b"+GmSQpB9jD/hPYjWRKmEP23WXKJNVdI/4gh3743KoD8=";

        // パラメータチューニングその2
        const N2: &[u8] = b"dVAHdVAH1T/FV3zFV3zlP5qZmZmZmdk/xVd8xVd85T9QB3VQB3XgPx3UQR3UQe0/SZIkSZIk6T8d1EEd1EG9P1AHdVAHdeA/WHzFV3zFxz+SJEmSJEnCPx3UQR3UQY0/LL7iK77i6z++4iu+4ivuP8VXfMVXfOU/i6/4iq/46j9QB3VQB3XQP1/xFV/xFc8/O6iDOqiD6j8d1EEd1EGtP1/xFV/xFd8/kiRJkiRJ0j+amZmZmZm5P8VXfMVXfOU/Fl/xFV/xpT8d1EEd1EHtP7dt27Zt2+Y/HdRBHdRBjT8d1EEd1EHtP1/xFV/xFc8/MzMzMzMz0z/UQR3UQR3UPzMzMzMzM9M/1EEd1EEd1D9f8RVf8RXPP1/xFV/xFe8/";
        const T2: &[u8] = b"0sAVd2z+4T/CGzt64F3av7i5uMOIdsC/Pcqa1h4bsD8QRCPNk6/Jv23rCKxuN/A/bOak0z3O9D9ceD0pEYrkv01dsEL6seo/TJCzvZ054j+Md2WPDvnxP3uNy7KjRew/CjPy9w/78D91yLW0ogP1Pz3KmtYeG7A/2IeyASnVvT+4p5QH9GLvP1onCpmaVeM/tyi5mpmbxj+oYhHAMAqvv3mzs2wcmc+/jF9zA9cD7D+Qn4pLTkXTP3wkTBMhg+A/mq4KZxAd6z+85vpnqKvVvxax0g97IeO/6ncJLSNN3T/N+Pm/J5Hjv694m7URLtw/oj/39/ng4j/Gx/5AKwbTP0iZR7Y8K9G/7AavX48S37/AaT/hewbqP1IZaUS59/A/";
        const SIGMA2: &[u8] = b"RB5fBhSI2j+r0I/o2mbMPxSuR+F6FNY/A+RecyFp5j82qoTFbHriP8taHRM8mOM/AUzKe2B42z+ASXkPDG/YPz0K16NwPeI/lD7pkz7pwz/Gkl8s+cWSP3E9CtejcOU/PVH/Riij7z/gdXLVns7gP9SZLIy4sbc/V4Dcay4k4T9vy1odEzy4Pz1R/0Yoo7c/VAmL2fTckT/ZqBIWs+ntPw2VHaYut+M/4+zDZ9gVwD8ijy8DCkTpPw4tsp3vp+I/PCsaCfjmxT/1b4QyelveP/8gQ2WHqds/u2/xP1tD2D8PwKS8B4brP3Fj7xTULOc/Exaz6bkj1z+73P4gQ2XXP/hT46WbxMA/QobKDlOX5z94nVy1pzOpPw4tsp3vp+4/";
        const TOUCHING_THRESHOLD: &[u8] = b"AAAAAAAAKkAAAAAAAAAuQAAAAAAAACxAAAAAAAAALkAAAAAAAAAwQAAAAAAAACxAAAAAAAAALkAAAAAAAAAsQAAAAAAAAC5AAAAAAAAALkAAAAAAAAAwQAAAAAAAAC5AAAAAAAAAKkAAAAAAAAAwQAAAAAAAACxAAAAAAAAALkAAAAAAAAAwQAAAAAAAACpAAAAAAAAAMEAAAAAAAAAuQAAAAAAAAC5AAAAAAAAALEAAAAAAAAAuQAAAAAAAAC5AAAAAAAAAKkAAAAAAAAAsQAAAAAAAAC5AAAAAAAAALkAAAAAAAAAsQAAAAAAAADBAAAAAAAAAMEAAAAAAAAAsQAAAAAAAAChAAAAAAAAALEAAAAAAAAAwQAAAAAAAACxA";
        const INVALID_CNT_THRESHOLD: &[u8] = b"AAAAAAAACEAAAAAAAAAAAAAAAAAAAPA/AAAAAAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA8D8AAAAAAAAAQAAAAAAAAPA/AAAAAAAAAAAAAAAAAAAIQAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAPA/AAAAAAAAAEAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAPA/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAPA/AAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAA8D8AAAAAAADwPwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA8D8AAAAAAAAAAAAAAAAAAABA";
        const PARAM_TOUCHING_THRESHOLD: &[u8] = b"3y2hNpd9hD8BOESjrfyGP0jIT9FtfJQ/JZSIIIei8D8=";
        const PARAM_INVALID_CNT_THRESHOLD: &[u8] = b"pO6mDR+EhD+oodlNJ8KEP5yeQHXbxI4/URmMuSZ17D8=";

        pub struct ParamSuggester {
            x_matrix: DMatrix<f64>,
            y_vector: DVector<f64>,
            hyper_param: DVector<f64>,
            y_inv_trans: fn(f64) -> f64,
            lower: f64,
            upper: f64,
        }

        impl ParamSuggester {
            fn new(
                hyper_param: DVector<f64>,
                x_matrix: DMatrix<f64>,
                y_vector: DVector<f64>,
                y_inv_trans: fn(f64) -> f64,
                lower: f64,
                upper: f64,
            ) -> Self {
                Self {
                    hyper_param,
                    x_matrix,
                    y_vector,
                    y_inv_trans,
                    lower,
                    upper,
                }
            }

            fn gen_x1_matrix() -> DMatrix<f64> {
                let n = DVector::from_vec(decode_base64(N1)).transpose();
                let t = DVector::from_vec(decode_base64(T1)).transpose();
                let sigma = DVector::from_vec(decode_base64(SIGMA1)).transpose();

                let x_matrix = DMatrix::from_rows(&[n, t, sigma]);

                x_matrix
            }

            fn gen_x2_matrix() -> DMatrix<f64> {
                let n = DVector::from_vec(decode_base64(N2)).transpose();
                let t = DVector::from_vec(decode_base64(T2)).transpose();
                let sigma = DVector::from_vec(decode_base64(SIGMA2)).transpose();

                let x_matrix = DMatrix::from_rows(&[n, t, sigma]);

                x_matrix
            }

            pub fn gen_arrange_count_pred(t: usize) -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_ARRANGE_COUNT));
                let y_vector = DVector::from_vec(decode_base64(ARRANGE_COUNT));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    5.0,
                    (t - 1) as f64,
                )
            }

            pub fn gen_query_annealing_duration_sec() -> Self {
                let hyper_param =
                    DVector::from_vec(decode_base64(PARAM_QUERY_ANNEALING_DURATION_SEC));
                let y_vector = DVector::from_vec(decode_base64(QUERY_ANNEALING_DURATION_SEC));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    0.05,
                    0.3,
                )
            }

            pub fn gen_mcmc_init_duration_sec() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_MCMC_INIT_DURATION_SEC));
                let y_vector = DVector::from_vec(decode_base64(MCMC_INIT_DURATION_SEC));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    0.05,
                    0.15,
                )
            }

            pub fn gen_beam_mcts_duration_ratio() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_BEAM_MCTS_DURATION_RATIO));
                let y_vector = DVector::from_vec(decode_base64(BEAM_MCTS_DURATION_RATIO));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    0.3,
                    0.7,
                )
            }

            pub fn gen_mcmc_duration_ratio() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_MCMC_DURATION_RATIO));
                let y_vector = DVector::from_vec(decode_base64(MCMC_DURATION_RATIO));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    0.03,
                    0.2,
                )
            }

            pub fn gen_mcts_turn() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_MCTS_TURN));
                let y_vector = DVector::from_vec(decode_base64(MCTS_TURN));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    0.8,
                    20.0,
                )
            }

            pub fn gen_mcts_expansion_threshold() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_MCTS_EXPANSION_THRESHOLD));
                let y_vector = DVector::from_vec(decode_base64(MCTS_EXPANSION_THRESHOLD));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    1.0,
                    5.0,
                )
            }

            pub fn gen_mcts_candidates_count() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_MCTS_CANDIDATES_COUNT));
                let y_vector = DVector::from_vec(decode_base64(MCTS_CANDIDATES_COUNT));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    2.0,
                    6.0,
                )
            }

            pub fn gen_parallel_score_mul() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_PARALLEL_SCORE_MUL));
                let y_vector = DVector::from_vec(decode_base64(PARALLEL_SCORE_MUL));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    0.7,
                    1.0,
                )
            }

            pub fn gen_width_buf() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_WIDTH_BUF));
                let y_vector = DVector::from_vec(decode_base64(WIDTH_BUF));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    1.02,
                    1.15,
                )
            }

            pub fn gen_ucb1_tuned_coef() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_UCB1_TUNED_COEF));
                let y_vector = DVector::from_vec(decode_base64(UCB1_TUNED_COEF));
                Self::new(
                    hyper_param,
                    Self::gen_x1_matrix(),
                    y_vector,
                    |x| x,
                    0.05,
                    1.0,
                )
            }

            fn gen_touching_threshold() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_TOUCHING_THRESHOLD));
                let y_vector = DVector::from_vec(decode_base64(TOUCHING_THRESHOLD));
                Self::new(
                    hyper_param,
                    Self::gen_x2_matrix(),
                    y_vector,
                    |x| x,
                    12.0,
                    16.0,
                )
            }

            fn gen_invalid_cnt_threshold() -> Self {
                let hyper_param = DVector::from_vec(decode_base64(PARAM_INVALID_CNT_THRESHOLD));
                let y_vector = DVector::from_vec(decode_base64(INVALID_CNT_THRESHOLD));
                Self::new(
                    hyper_param,
                    Self::gen_x2_matrix(),
                    y_vector,
                    |x| x,
                    0.0,
                    3.0,
                )
            }

            pub fn suggest(&self, n: usize, t: usize, sigma: f64) -> f64 {
                let t = (t as f64 / n as f64).ln();
                let n = (n - 30) as f64 / 70.0;
                let sigma = (sigma - 1000.0) / 9000.0;

                let len = self.x_matrix.shape().1;
                let y_mean = self.y_vector.mean();
                let y_mean = DVector::from_element(self.y_vector.len(), y_mean);
                let new_x = DMatrix::from_vec(3, 1, vec![n, t, sigma]);
                let noise = DMatrix::from_diagonal_element(len, len, self.hyper_param[3]);

                let k = self.calc_kernel_matrix(&self.x_matrix, &self.x_matrix) + noise;
                let kk = self.calc_kernel_matrix(&self.x_matrix, &new_x);

                let kernel_lu = k.lu();
                let new_y = kk.transpose() * kernel_lu.solve(&(&self.y_vector - &y_mean)).unwrap();

                (self.y_inv_trans)(new_y[(0, 0)] + y_mean[(0, 0)]).clamp(self.lower, self.upper)
            }

            fn calc_kernel_matrix(&self, x1: &DMatrix<f64>, x2: &DMatrix<f64>) -> DMatrix<f64> {
                let n = x1.shape().1;
                let m = x2.shape().1;
                let mut kernel = DMatrix::zeros(n, m);

                for i in 0..n {
                    for j in 0..m {
                        kernel[(i, j)] = self.gaussian_kernel(&x1.column(i), &x2.column(j));
                    }
                }

                kernel
            }

            fn gaussian_kernel(&self, x1: &DVectorView<f64>, x2: &DVectorView<f64>) -> f64 {
                let t1 = self.hyper_param[0];
                let t2 = self.hyper_param[1];
                let t3 = self.hyper_param[2];

                let diff = x1 - x2;
                let norm_diff = diff.dot(&diff);
                let dot = x1.dot(&x2);
                t1 * dot + t2 * (-norm_diff / t3).exp()
            }
        }

        fn decode_base64(data: &[u8]) -> Vec<f64> {
            const BASE64_MAP: &[u8] =
                b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            let mut stream = vec![];

            let mut cursor = 0;

            while cursor + 4 <= data.len() {
                let mut buffer = 0u32;

                for i in 0..4 {
                    let c = data[cursor + i];
                    let shift = 6 * (3 - i);

                    for (i, &d) in BASE64_MAP.iter().enumerate() {
                        if c == d {
                            buffer |= (i as u32) << shift;
                        }
                    }
                }

                for i in 0..3 {
                    let shift = 8 * (2 - i);
                    let value = (buffer >> shift) as u8;
                    stream.push(value);
                }

                cursor += 4;
            }

            let mut result = vec![];
            cursor = 0;

            while cursor + 8 <= stream.len() {
                let p = stream.as_ptr() as *const f64;
                let x = unsafe { *p.offset(cursor as isize / 8) };
                result.push(x);
                cursor += 8;
            }

            result
        }

        fn get_env<T: FromStr>(name: &str, default: T) -> T {
            env::var(name)
                .ok()
                .and_then(|s| s.parse().ok())
                .unwrap_or(default)
        }
    }
    mod self_judge {
        use super::{Input, Judge, Rect};
        use crate::{
            problem::{Dir, Measure},
            util::ChangeMinMax,
        };
        use proconio::input_interactive;

        pub struct SelfJudge {
            input: Input,
            turn: usize,
            score: u32,
            rects: Vec<Rect>,
            errors: Vec<(i32, i32)>,
        }

        #[allow(dead_code)]
        impl SelfJudge {
            pub fn read(input: &Input) -> Self {
                input_interactive! {
                    rects: [(u32, u32); input.rect_cnt()],
                    errors: [(i32, i32); input.query_cnt()],
                }

                let rects = rects.into_iter().map(|(w, h)| Rect::new(h, w)).collect();

                Self::new(input.clone(), rects, errors)
            }

            fn new(input: Input, rects: Vec<Rect>, errors: Vec<(i32, i32)>) -> Self {
                let score = rects.iter().map(|r| r.width + r.height).sum();

                Self {
                    input,
                    turn: 0,
                    score,
                    errors,
                    rects,
                }
            }

            pub fn score(&self) -> u32 {
                self.score
            }
        }

        impl Judge for SelfJudge {
            fn query(&mut self, query: &[super::Op]) -> super::Measure {
                assert!(self.turn < self.input.query_cnt());
                println!("{}", query.len());

                for op in query {
                    println!("{}", op);
                }

                let mut width = 0;
                let mut height = 0;
                let mut prev = -1;
                let mut pos = vec![P0; self.input.rect_cnt()];

                for (t, &op) in query.iter().enumerate() {
                    assert!(
                        prev.change_max(op.rect_idx as i32),
                        "rect_idx must be in ascending order.",
                    );

                    if let Some(base) = op.base {
                        assert!(pos[base].t >= 0, "Rectangle {} is not placed.", base);
                    }

                    let Rect {
                        width: w,
                        height: h,
                    } = self.rects[op.rect_idx as usize];
                    let mut w = w as i32;
                    let mut h = h as i32;

                    if op.rotate {
                        std::mem::swap(&mut w, &mut h);
                    }

                    match op.dir {
                        Dir::Up => {
                            let x1 = match op.base {
                                Some(base) => pos[base].x2,
                                None => 0,
                            };
                            let x2 = x1 + w;
                            let y1 = pos
                                .iter()
                                .filter_map(|p| {
                                    if p.t >= 0 && x1.max(p.x1) < x2.min(p.x2) {
                                        Some(p.y2)
                                    } else {
                                        None
                                    }
                                })
                                .max()
                                .unwrap_or(0);
                            let y2 = y1 + h;
                            pos[op.rect_idx] = Pos::new(x1, x2, y1, y2, op.rotate, t as i32);
                        }
                        Dir::Left => {
                            let y1 = match op.base {
                                Some(base) => pos[base].y2,
                                None => 0,
                            };
                            let y2 = y1 + h;
                            let x1 = pos
                                .iter()
                                .filter_map(|p| {
                                    if p.t >= 0 && y1.max(p.y1) < y2.min(p.y2) {
                                        Some(p.x2)
                                    } else {
                                        None
                                    }
                                })
                                .max()
                                .unwrap_or(0);
                            let x2 = x1 + w;
                            pos[op.rect_idx] = Pos::new(x1, x2, y1, y2, op.rotate, t as i32);
                        }
                    }

                    width.change_max(pos[op.rect_idx].x2);
                    height.change_max(pos[op.rect_idx].y2);
                }

                let measured_w = (width + self.errors[self.turn].0).clamp(1, 1_000_000_000);
                let measured_h = (height + self.errors[self.turn].1).clamp(1, 1_000_000_000);
                let mut score = (width + height) as u32;

                for i in 0..self.input.rect_cnt() {
                    if pos[i].t < 0 {
                        score += self.rects[i].width + self.rects[i].height;
                    }
                }

                self.score.change_min(score);
                self.turn += 1;

                if self.turn == self.input.query_cnt() {
                    eprintln!("Score = {}", self.score);
                }

                Measure::new(measured_h as u32, measured_w as u32)
            }

            fn query_cnt(&self) -> usize {
                self.turn
            }

            fn rects(&self) -> Option<&[Rect]> {
                Some(&self.rects)
            }
        }

        #[derive(Debug, Clone, Copy)]
        #[allow(dead_code)]
        struct Pos {
            x1: i32,
            x2: i32,
            y1: i32,
            y2: i32,
            r: bool,
            t: i32,
        }

        impl Pos {
            const fn new(x1: i32, x2: i32, y1: i32, y2: i32, r: bool, t: i32) -> Self {
                Self {
                    x1,
                    x2,
                    y1,
                    y2,
                    r,
                    t,
                }
            }
        }

        const P0: Pos = Pos::new(-1, -1, -1, -1, false, -1);
    }

    use params::Params;
    use proconio::input_interactive;
    use std::{fmt::Display, time::Instant};

    #[derive(Debug, Clone)]
    pub struct Input {
        rect_cnt: usize,
        query_cnt: usize,
        std_dev: f64,
        rect_measures: Vec<Measure>,
        since: Instant,
    }

    impl Input {
        pub const MAX_RECT_CNT: usize = 100;
        pub const MAX_RECT_SIZE: u32 = 100000;
        pub const MIN_RECT_SIZE: u32 = 10000;

        pub fn read() -> Self {
            input_interactive! {
                rect_cnt: usize,
                query_cnt: usize,
                std_dev: f64,
            }

            let since = Instant::now();
            let mut rectangles = Vec::with_capacity(rect_cnt);

            for _ in 0..rect_cnt {
                input_interactive! {
                    width: u32,
                    height: u32,
                }

                rectangles.push(Measure::new(height, width));
            }

            let params = Params::new(rect_cnt, query_cnt, std_dev);
            eprintln!("[Params]");
            eprintln!("{}", params);

            *Params::get().borrow_mut() = params;

            Self {
                rect_cnt,
                query_cnt,
                std_dev,
                rect_measures: rectangles,
                since,
            }
        }

        pub fn rect_cnt(&self) -> usize {
            self.rect_cnt
        }

        pub fn query_cnt(&self) -> usize {
            self.query_cnt
        }

        pub fn std_dev(&self) -> f64 {
            self.std_dev
        }

        pub fn rect_measures(&self) -> &[Measure] {
            &self.rect_measures
        }

        pub fn since(&self) -> Instant {
            self.since
        }
    }

    #[derive(Debug, Clone, Copy, Default, PartialEq, Eq)]
    pub struct Rect {
        height: u32,
        width: u32,
    }

    impl Rect {
        pub fn new(height: u32, width: u32) -> Self {
            Self { height, width }
        }

        pub fn height(&self) -> u32 {
            self.height
        }

        pub fn width(&self) -> u32 {
            self.width
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    pub struct Measure {
        height: u32,
        width: u32,
    }

    impl Measure {
        fn new(height: u32, width: u32) -> Self {
            Self { height, width }
        }

        pub fn height(&self) -> u32 {
            self.height
        }

        pub fn width(&self) -> u32 {
            self.width
        }
    }

    impl Display for Measure {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "{} x {}", self.height, self.width)
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    pub enum Dir {
        #[default]
        Up = 0x00,
        Left = 0x01,
    }

    impl Display for Dir {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            match self {
                Dir::Up => write!(f, "U"),
                Dir::Left => write!(f, "L"),
            }
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    pub struct Op {
        rect_idx: usize,
        rotate: bool,
        dir: Dir,
        base: Option<usize>,
    }

    impl Op {
        pub fn new(rect_idx: usize, rotate: bool, dir: Dir, base: Option<usize>) -> Self {
            Self {
                rect_idx,
                rotate,
                dir,
                base,
            }
        }

        pub fn rect_idx(&self) -> usize {
            self.rect_idx
        }

        pub fn rotate(&self) -> bool {
            self.rotate
        }

        pub fn dir(&self) -> Dir {
            self.dir
        }

        pub fn base(&self) -> Option<usize> {
            self.base
        }
    }

    impl Display for Op {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(
                f,
                "{} {} {}",
                self.rect_idx,
                if self.rotate { 1 } else { 0 },
                self.dir
            )?;

            match self.base {
                Some(base) => write!(f, " {}", base),
                None => write!(f, " -1"),
            }
        }
    }

    pub trait Judge {
        fn query(&mut self, query: &[Op]) -> Measure;
        fn query_cnt(&self) -> usize;
        fn rects(&self) -> Option<&[Rect]>;
    }

    #[cfg(feature = "local")]
    pub fn gen_judge(input: &Input) -> impl Judge {
        self_judge::SelfJudge::read(input)
    }

    #[cfg(not(feature = "local"))]
    pub fn gen_judge(_input: &Input) -> impl Judge {
        atcoder_judge::AtCoderJudge::new()
    }
}
#[allow(dead_code)]
mod sa {
    //! 焼きなましライブラリ

    use itertools::Itertools;
    use rand::Rng;
    use rand_pcg::Pcg64Mcg;
    use std::{
        cell::RefCell,
        fmt::{Debug, Display},
        rc::Rc,
        time::Instant,
    };

    /// 焼きなましの状態
    pub trait State {
        type Env;
        type Score: Score + Clone + PartialEq + Debug;

        /// 生スコア（大きいほど良い）
        fn score(&self, env: &Self::Env) -> Self::Score;
    }

    pub trait Score {
        /// 焼きなまし用スコア（大きいほど良い）
        /// デフォルトでは生スコアをそのまま返す
        fn annealing_score(&self, _progress: f64) -> f64 {
            self.raw_score() as f64
        }

        /// 生スコア
        fn raw_score(&self) -> f64;
    }

    /// 単一の値からなるスコア
    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
    pub struct SingleScore(pub i64);

    impl Score for SingleScore {
        fn raw_score(&self) -> f64 {
            self.0 as f64
        }
    }

    /// 焼きなましの近傍
    ///
    /// * 受理パターンの流れ: `preprocess()` -> `eval()` -> `postprocess()`
    /// * 却下パターンの流れ: `preprocess()` -> `eval()` -> `rollback()`
    pub trait Neighbor {
        type Env;
        type State: State<Env = Self::Env>;

        /// `eval()` 前の変形操作を行う
        fn preprocess(&mut self, _env: &Self::Env, _state: &mut Self::State);

        /// 変形後の状態の評価を行う
        ///
        /// # Arguments
        ///
        /// * `env` - 環境
        /// * `state` - 状態
        /// * `progress` - 焼きなましの進捗（[0, 1]の範囲をとる）
        /// * `threshold` - 近傍採用の閾値。新しいスコアがこの値を下回る場合はrejectされる
        ///
        /// # Returns
        ///
        /// 現在の状態のスコア。スコアが `threshold` を下回ることが明らかな場合は `None` を返すことで評価の打ち切りを行うことができる。
        ///
        /// 評価の打ち切りについては[焼きなまし法での評価関数の打ち切り](https://qiita.com/not522/items/cd20b87157d15850d31c)を参照。
        fn eval(
            &mut self,
            env: &Self::Env,
            state: &Self::State,
            _progress: f64,
            _threshold: f64,
        ) -> Option<<Self::State as State>::Score> {
            Some(state.score(env))
        }

        /// `eval()` 後の変形操作を行う（2-optの区間reverse処理など）
        fn postprocess(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State);

        /// `preprocess()` で変形した `state` をロールバックする
        fn rollback(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State);
    }

    /// 焼きなましの近傍を生成する構造体
    pub trait NeighborGenerator {
        type Env;
        type State: State;

        /// 近傍を生成する
        fn generate(
            &self,
            env: &Self::Env,
            state: &Self::State,
            rng: &mut impl Rng,
        ) -> Box<dyn Neighbor<Env = Self::Env, State = Self::State>>;
    }

    /// 焼きなましの統計データ
    #[derive(Debug, Clone, Copy)]
    pub struct AnnealingStatistics {
        all_iter: usize,
        accepted_count: usize,
        updated_count: usize,
        init_score: f64,
        final_score: f64,
    }

    impl AnnealingStatistics {
        fn new(init_score: f64) -> Self {
            Self {
                all_iter: 0,
                accepted_count: 0,
                updated_count: 0,
                init_score,
                final_score: init_score,
            }
        }
    }

    impl Display for AnnealingStatistics {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            writeln!(f, "===== annealing =====")?;
            writeln!(f, "init score : {}", self.init_score)?;
            writeln!(f, "score      : {}", self.final_score)?;
            writeln!(f, "all iter   : {}", self.all_iter)?;
            writeln!(f, "accepted   : {}", self.accepted_count)?;
            writeln!(f, "updated    : {}", self.updated_count)?;

            Ok(())
        }
    }

    #[derive(Debug, Clone)]
    pub struct Annealer {
        /// 開始温度
        start_temp: f64,
        /// 終了温度
        end_temp: f64,
        /// 乱数シード
        seed: u128,
        /// 時間計測を行うインターバル
        clock_interval: usize,
    }

    impl Annealer {
        pub fn new(start_temp: f64, end_temp: f64, seed: u128, clock_interval: usize) -> Self {
            Self {
                start_temp,
                end_temp,
                seed,
                clock_interval,
            }
        }

        pub fn run<E, S: State<Env = E> + Clone, G: NeighborGenerator<Env = E, State = S>>(
            &self,
            env: &E,
            mut state: S,
            neighbor_generator: &G,
            duration_sec: f64,
        ) -> (S, AnnealingStatistics) {
            let mut best_state = state.clone();
            let mut current_score = state.score(&env);
            let mut best_score = current_score.annealing_score(1.0);

            let mut diagnostics = AnnealingStatistics::new(current_score.raw_score());
            let mut rng = Pcg64Mcg::new(self.seed);
            let threshold_generator = ThresholdGenerator::get_singleton();
            let mut threshold_generator = threshold_generator.borrow_mut();
            threshold_generator.set_pos(rng.gen());

            let duration_inv = 1.0 / duration_sec;
            let since = Instant::now();

            let mut progress = 0.0;
            let mut temperature = self.start_temp;

            loop {
                diagnostics.all_iter += 1;

                if diagnostics.all_iter % self.clock_interval == 0 {
                    progress = (Instant::now() - since).as_secs_f64() * duration_inv;
                    temperature = f64::powf(self.start_temp, 1.0 - progress)
                        * f64::powf(self.end_temp, progress);

                    if progress >= 1.0 {
                        break;
                    }
                }

                // 変形
                let mut neighbor = neighbor_generator.generate(env, &state, &mut rng);
                neighbor.preprocess(env, &mut state);

                // スコア計算
                let threshold =
                    threshold_generator.next(current_score.annealing_score(progress), temperature);
                let Some(new_score) = neighbor.eval(env, &state, progress, threshold) else {
                    // 明らかに閾値に届かない場合はreject
                    neighbor.rollback(env, &mut state);
                    debug_assert_eq!(state.score(&env), current_score);
                    continue;
                };

                if new_score.annealing_score(progress) >= threshold {
                    // 解の更新
                    neighbor.postprocess(env, &mut state);
                    debug_assert_eq!(state.score(&env), new_score);

                    current_score = new_score;
                    diagnostics.accepted_count += 1;

                    let new_score = current_score.annealing_score(1.0);

                    if best_score < new_score {
                        best_score = new_score;
                        best_state = state.clone();
                        diagnostics.updated_count += 1;
                    }
                } else {
                    neighbor.rollback(env, &mut state);
                }
            }

            diagnostics.final_score = best_state.score(&env).raw_score();

            (best_state, diagnostics)
        }
    }

    /// 焼きなましにおける評価関数の打ち切り基準となる次の閾値を返す構造体
    ///
    /// 参考: [焼きなまし法での評価関数の打ち切り](https://qiita.com/not522/items/cd20b87157d15850d31c)
    struct ThresholdGenerator {
        iter: usize,
        log_randoms: Vec<f64>,
    }

    impl ThresholdGenerator {
        const LEN: usize = 1 << 16;
        thread_local! {
            static THRESHOLD_GENERATOR: Rc<RefCell<ThresholdGenerator>> = Rc::new(RefCell::new(ThresholdGenerator::new(42)));
        }

        fn new(seed: u128) -> Self {
            let mut rng = Pcg64Mcg::new(seed);
            let log_randoms = (0..Self::LEN)
                .map(|_| rng.gen_range(0.0f64..1.0).ln())
                .collect_vec();

            Self {
                iter: 0,
                log_randoms,
            }
        }

        /// 評価関数の打ち切り基準となる次の閾値を返す
        fn next(&mut self, prev_score: f64, temperature: f64) -> f64 {
            let threshold = prev_score + temperature * self.log_randoms[self.iter % Self::LEN];
            self.iter += 1;
            threshold
        }

        fn set_pos(&mut self, pos: usize) {
            self.iter = pos % Self::LEN;
        }

        fn get_singleton() -> Rc<RefCell<Self>> {
            Self::THRESHOLD_GENERATOR.with(|cell| cell.clone())
        }
    }

    #[cfg(test)]
    mod test {
        use itertools::Itertools;
        use rand::Rng;

        use super::{Annealer, Neighbor, Score};

        #[derive(Debug, Clone)]
        struct Input {
            n: usize,
            distances: Vec<Vec<i32>>,
        }

        impl Input {
            fn gen_testcase() -> Self {
                let n = 4;
                let distances = vec![
                    vec![0, 2, 3, 10],
                    vec![2, 0, 1, 3],
                    vec![3, 1, 0, 2],
                    vec![10, 3, 2, 0],
                ];

                Self { n, distances }
            }
        }

        #[derive(Debug, Clone)]
        struct State {
            order: Vec<usize>,
            dist: i32,
        }

        impl State {
            fn new(input: &Input) -> Self {
                let mut order = (0..input.n).collect_vec();
                order.push(0);
                let dist = order
                    .iter()
                    .tuple_windows()
                    .map(|(&prev, &next)| input.distances[prev][next])
                    .sum();

                Self { order, dist }
            }
        }

        impl super::State for State {
            type Env = Input;
            type Score = Dist;

            fn score(&self, _env: &Self::Env) -> Self::Score {
                Dist(self.dist)
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq)]
        struct Dist(i32);

        impl Score for Dist {
            fn annealing_score(&self, _progress: f64) -> f64 {
                // 大きい方が良いとするため符号を反転
                -self.0 as f64
            }

            fn raw_score(&self) -> f64 {
                self.0 as f64
            }
        }

        struct TwoOpt {
            begin: usize,
            end: usize,
            new_dist: Option<i32>,
        }

        impl TwoOpt {
            fn new(begin: usize, end: usize) -> Self {
                Self {
                    begin,
                    end,
                    new_dist: None,
                }
            }
        }

        impl Neighbor for TwoOpt {
            type Env = Input;
            type State = State;

            fn preprocess(&mut self, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }

            fn eval(
                &mut self,
                env: &Self::Env,
                state: &Self::State,
                _progress: f64,
                _threshold: f64,
            ) -> Option<<Self::State as super::State>::Score> {
                let v0 = state.order[self.begin - 1];
                let v1 = state.order[self.begin];
                let v2 = state.order[self.end - 1];
                let v3 = state.order[self.end];

                let d00 = env.distances[v0][v1];
                let d01 = env.distances[v0][v2];
                let d10 = env.distances[v2][v3];
                let d11 = env.distances[v1][v3];

                let new_dist = state.dist - d00 - d10 + d01 + d11;
                self.new_dist = Some(new_dist);

                Some(Dist(new_dist))
            }

            fn postprocess(self: Box<Self>, _env: &Self::Env, state: &mut Self::State) {
                state.order[self.begin..self.end].reverse();
                state.dist = self
                    .new_dist
                    .expect("postprocess()を呼ぶ前にeval()を呼んでください。");
            }

            fn rollback(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }
        }

        struct NeighborGenerator;

        impl super::NeighborGenerator for NeighborGenerator {
            type Env = Input;
            type State = State;

            fn generate(
                &self,
                _env: &Self::Env,
                state: &Self::State,
                rng: &mut impl Rng,
            ) -> Box<dyn Neighbor<Env = Self::Env, State = Self::State>> {
                loop {
                    let begin = rng.gen_range(1..state.order.len());
                    let end = rng.gen_range(1..state.order.len());

                    if begin + 2 <= end {
                        return Box::new(TwoOpt::new(begin, end));
                    }
                }
            }
        }

        #[test]
        fn annealing_tsp_test() {
            let input = Input::gen_testcase();
            let state = State::new(&input);
            let annealer = Annealer::new(1e1, 1e-1, 42, 1000);
            let neighbor_generator = NeighborGenerator;

            let (state, diagnostics) = annealer.run(&input, state, &neighbor_generator, 0.1);

            eprintln!("{}", diagnostics);

            eprintln!("score: {}", state.dist);
            eprintln!("state.dist: {:?}", state.order);
            assert_eq!(state.dist, 10);
            assert!(state.order == vec![0, 1, 3, 2, 0] || state.order == vec![0, 2, 3, 1, 0]);
        }
    }
}
pub mod solver {
    mod arranger {
        pub(super) mod mcts {
            use itertools::{izip, Itertools};
            use rand::Rng;

            use crate::{
                problem::{params::Params, Dir, Input, Op},
                solver::simd::*,
                util::{BitSetIterU128, ChangeMinMax},
            };
            use core::f32;
            use std::{arch::x86_64::*, u64};

            pub struct MCTSArranger;

            impl MCTSArranger {
                pub fn arrange(
                    &mut self,
                    input: &Input,
                    start_ops: &[Op],
                    rects: SimdRectSet,
                    rng: &mut impl Rng,
                    duration_sec: f64,
                ) -> Vec<Op> {
                    let since = std::time::Instant::now();
                    let mut best_score = f32::NEG_INFINITY;
                    let mut best_ops = vec![];

                    let mut state = unsafe { State::new(input.clone(), rects) };
                    state.apply_init_ops(start_ops);

                    let mut root = Node::new(Action::default());
                    let mut score_sum = 0.0;

                    while since.elapsed().as_secs_f64() < duration_sec {
                        let mut state = state.clone();
                        let (score, _) =
                            root.evaluate(&mut state, &mut best_score, &mut best_ops, rng);
                        score_sum += score;
                    }

                    eprintln!("total_iter: {}", root.total_count);
                    eprintln!("average_score: {}", score_sum / root.total_count as f32);

                    best_ops.pop();
                    best_ops.reverse();
                    best_ops
                }
            }

            #[derive(Debug, Clone)]
            struct Node {
                action: Action,
                node_score: Vec<f32>,
                node_count: Vec<f32>,
                score_squared_sum: Vec<f32>,
                total_count: usize,
                is_expanded: bool,
                is_valid: Vec<bool>,
                candidates: Vec<Option<Box<Node>>>,
            }

            impl Node {
                fn new(action: Action) -> Self {
                    let candidates_count = Params::get().borrow().mcts_candidates_count;
                    let node_score = vec![0.0; candidates_count];
                    let node_count = vec![0.0; candidates_count];
                    let score_squared_sum = vec![0.0; candidates_count];
                    let is_valid = vec![false; candidates_count];
                    let candidates = vec![None; candidates_count];

                    Self {
                        action,
                        node_score,
                        node_count,
                        score_squared_sum,
                        total_count: 0,
                        is_expanded: false,
                        is_valid,
                        candidates,
                    }
                }

                fn evaluate(
                    &mut self,
                    state: &mut State,
                    best_score: &mut f32,
                    best_ops: &mut Vec<Op>,
                    rng: &mut impl Rng,
                ) -> (f32, bool) {
                    // stateがactionによって変更されないうちに展開を行う（帰りがけにやるとダメ）
                    if self.total_count == Params::get().borrow().mcts_expansion_threshold {
                        self.expand(state);
                    }

                    let (score, updated) = if state.turn == state.rect_count {
                        // ゲーム終了
                        self.evaluate_end_game(state, best_score, best_ops)
                    } else if self.is_expanded {
                        // 子ノードを選んで再帰的に評価
                        let child_index = self.choose_next_child();
                        let child = self.candidates[child_index].as_mut().unwrap();
                        child.action.apply(state);
                        let (score, updated) = child.evaluate(state, best_score, best_ops, rng);

                        self.node_score[child_index] += score;
                        self.node_count[child_index] += 1.0;
                        self.score_squared_sum[child_index] += score * score;
                        (score, updated)
                    } else {
                        // Bottom-Left法でプレイアウト
                        let action = state.gen_min_topline(rng.gen_bool(0.5));
                        action.apply(state);
                        self.playout(state, best_score, best_ops, action.op, rng)
                    };

                    self.total_count += 1;

                    if updated {
                        best_ops.push(self.action.op);
                    }

                    (score, updated)
                }

                fn evaluate_end_game(
                    &mut self,
                    state: &mut State,
                    best_score: &mut f32,
                    best_ops: &mut Vec<Op>,
                ) -> (f32, bool) {
                    let score = unsafe { state.calc_score() };

                    let score_updated = best_score.change_max(score);

                    // スコアが更新されたら手順を更新
                    if score_updated {
                        best_ops.clear();
                    }

                    return (score, score_updated);
                }

                fn playout(
                    &mut self,
                    state: &mut State,
                    best_score: &mut f32,
                    best_ops: &mut Vec<Op>,
                    op: Op,
                    rng: &mut impl Rng,
                ) -> (f32, bool) {
                    let (score, updated) = if state.turn == state.rect_count {
                        let score = unsafe { state.calc_score() };
                        let score_updated = best_score.change_max(score);

                        // スコアが更新されたら手順をクリア
                        if score_updated {
                            best_ops.clear();
                        }

                        (score, score_updated)
                    } else {
                        let action = state.gen_min_topline(rng.gen_bool(0.5));
                        action.apply(state);
                        self.playout(state, best_score, best_ops, action.op, rng)
                    };

                    // スコアが更新されたら手順を更新
                    if updated {
                        best_ops.push(op);
                    }

                    (score, updated)
                }

                fn choose_next_child(&self) -> usize {
                    let mut best_ucb1 = f32::NEG_INFINITY;
                    let mut best_index = 0;

                    for i in 0..Params::get().borrow().mcts_candidates_count {
                        if !self.is_valid[i] {
                            continue;
                        }

                        if self.node_count[i] == 0.0 {
                            return i;
                        }

                        // UCB1-Tuned
                        let count = self.node_count[i];
                        let score = self.node_score[i];
                        let total_count = self.total_count as f32;
                        let inv_count = 1.0 / count;
                        let total_count_ln = total_count.ln();

                        let sq_average = self.score_squared_sum[i] / count;
                        let average_score = score / count;
                        let average_sq = average_score * average_score;
                        let variance = sq_average - average_sq;

                        let var = variance + (2.0 * total_count_ln * inv_count).sqrt();
                        let ucb1 = average_score
                            + Params::get().borrow().ucb1_tuned_coef
                                * (var.min(0.25) * total_count_ln * inv_count).sqrt();

                        if best_ucb1.change_max(ucb1) {
                            best_index = i;
                        }
                    }

                    best_index
                }

                fn expand(&mut self, state: &State) {
                    self.is_expanded = true;

                    if state.turn == state.rect_count {
                        return;
                    }

                    let actions = state.gen_all_actions();
                    let mut candidates: Vec<Option<Action>> =
                        vec![None; Params::get().borrow().mcts_candidates_count];

                    for action in actions {
                        let mut worst_bottom_left = action.top_value();
                        let mut worst_index = None;

                        for index in 0..Params::get().borrow().mcts_candidates_count {
                            let bl = match candidates[index] {
                                Some(ref candidate) => candidate.top_value(),
                                None => std::u32::MAX,
                            };

                            if worst_bottom_left.change_max(bl) {
                                worst_index = Some(index);
                            }
                        }

                        if let Some(index) = worst_index {
                            candidates[index] = Some(action);
                            self.is_valid[index] = true;
                        }
                    }

                    self.candidates = candidates
                        .into_iter()
                        .map(|action| action.map(|action| Box::new(Node::new(action))))
                        .collect_vec();
                }
            }

            #[derive(Debug, Clone)]
            struct State {
                // 256bitのレジスタをunsigned 16bit integer x16で使う
                rects_h: [AlignedU16; Input::MAX_RECT_CNT],
                rects_w: [AlignedU16; Input::MAX_RECT_CNT],
                widths: AlignedU16,
                heights: AlignedU16,
                width_limit: AlignedU16,
                height_limit: AlignedU16,
                placements_x0: [AlignedU16; Input::MAX_RECT_CNT],
                placements_x1: [AlignedU16; Input::MAX_RECT_CNT],
                placements_y0: [AlignedU16; Input::MAX_RECT_CNT],
                placements_y1: [AlignedU16; Input::MAX_RECT_CNT],
                turn: usize,
                rect_count: usize,
                avaliable_base_up: u128,
                min_rect_size: AlignedU16,
                score_coef: f32, // 1 / (√Σ(w_i * h_i) * PARALLEL_CNT) で正規化するための値
            }

            impl State {
                // スコア係数
                thread_local!(static SCORE_MUL: [AlignedF32; 2] = {
                    let score_mul = Params::get().borrow().parallel_score_mul;
                    [
                        AlignedF32(std::array::from_fn(|i| score_mul.powi(i as i32))),
                        AlignedF32(std::array::from_fn(|i| {
                            score_mul.powi((i + AVX2_F32_W) as i32)
                        })),
                    ]
                });

                #[target_feature(enable = "avx,avx2")]
                unsafe fn new(input: Input, rects: SimdRectSet) -> Self {
                    let mut rects_h = [AlignedU16::ZERO; Input::MAX_RECT_CNT];
                    let mut rects_w = [AlignedU16::ZERO; Input::MAX_RECT_CNT];

                    for i in 0..input.rect_cnt() {
                        rects_h[i] = AlignedU16(rects.heights[i]);
                        rects_w[i] = AlignedU16(rects.widths[i]);
                    }

                    let placements = [AlignedU16::ZERO; Input::MAX_RECT_CNT];

                    // 最初から幅を確保しておく
                    let mut areas = [0; AVX2_U16_W];

                    for (h, w) in izip!(&rects.heights, &rects.widths) {
                        for i in 0..AVX2_U16_W {
                            let w = w[i] as u64;
                            let h = h[i] as u64;
                            areas[i] += w * h;
                        }
                    }

                    // 少し余裕を持たせる
                    let default_width = AlignedU16(
                        areas.map(|a| (a as f64 * Params::get().borrow().width_buf).sqrt() as u16),
                    );

                    let width_limit = default_width;
                    let height_limit = AlignedU16([u16::MAX / 2; AVX2_U16_W]);

                    let wh_lower_bound = 2.0
                        * areas.iter().map(|a| (*a as f32).sqrt()).sum::<f32>()
                        / AVX2_U16_W as f32;
                    let score_coef = Self::SCORE_MUL.with(|score_mul| {
                        let sum1 = unsafe { horizontal_add_f32(score_mul[0].load()) };
                        let sum2 = unsafe { horizontal_add_f32(score_mul[1].load()) };
                        sum1 + sum2
                    });

                    let score_coef = 1.0 / (wh_lower_bound * score_coef);

                    let min_rect_size = unsafe {
                        let mut min = _mm256_set1_epi16(i16::MAX);

                        for size in rects_h[..input.rect_cnt()]
                            .iter()
                            .chain(rects_w[..input.rect_cnt()].iter())
                        {
                            min = _mm256_min_epu16(min, size.load());
                        }

                        let mut min_rect_size: [u16; AVX2_U16_W] = [0; AVX2_U16_W];
                        _mm256_storeu_si256(min_rect_size.as_mut_ptr() as *mut __m256i, min);

                        AlignedU16(min_rect_size)
                    };

                    Self {
                        rects_h,
                        rects_w,
                        widths: AlignedU16::ZERO,
                        heights: AlignedU16::ZERO,
                        width_limit,
                        height_limit,
                        placements_x0: placements.clone(),
                        placements_x1: placements.clone(),
                        placements_y0: placements.clone(),
                        placements_y1: placements.clone(),
                        turn: 0,
                        avaliable_base_up: 0,
                        min_rect_size,
                        rect_count: input.rect_cnt(),
                        score_coef,
                    }
                }

                fn apply_init_ops(&mut self, ops: &[Op]) {
                    for &op in ops {
                        // 無効な候補の列挙
                        let up_xor = self.get_up_invalid_bases(self.turn);

                        // 生成
                        let rotates = op.rotate();
                        let base = op.base();
                        let action = match op.dir() {
                            Dir::Left => unreachable!("Left is not supported"),
                            Dir::Up => self.gen_up_action(base, rotates, up_xor),
                        }
                        .unwrap();

                        action.apply(self);
                    }
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn calc_score(&self) -> f32 {
                    // スコアの昇順にソートした上で、減衰させながら和を取る
                    // （期待値を最大化するよりは上振れを狙いたいため）
                    Self::SCORE_MUL.with(|score_mul| {
                        let height = self.heights.load();
                        let width = self.widths.load();

                        let sum = _mm256_add_epi16(height, width);
                        let score = bitonic_sort_u16(sum);

                        // u16 x 16 -> (u32 x 8) x 2 -> (f32 x 8) x 2
                        let score_low = _mm256_castsi256_si128(score);
                        let score_high = _mm256_extracti128_si256(score, 1);
                        let score_u32_low = _mm256_cvtepu16_epi32(score_low);
                        let score_u32_high = _mm256_cvtepu16_epi32(score_high);
                        let score_f32_low = _mm256_cvtepi32_ps(score_u32_low);
                        let score_f32_high = _mm256_cvtepi32_ps(score_u32_high);

                        // 用意していた係数とかける
                        let score_mul_low = score_mul[0].load();
                        let score_mul_high = score_mul[1].load();
                        let score_low = _mm256_mul_ps(score_f32_low, score_mul_low);
                        let score_high = _mm256_mul_ps(score_f32_high, score_mul_high);
                        let score = _mm256_add_ps(score_low, score_high);
                        let score = horizontal_add_f32(score);
                        let normalized_score = score * self.score_coef;

                        1.0 - normalized_score
                    })
                }

                fn gen_all_actions(&self) -> Vec<Action> {
                    // 無効な候補の列挙
                    let up_xor = self.get_up_invalid_bases(self.turn);
                    let up_cands = self.avaliable_base_up & !up_xor;

                    // 生成
                    let rotates = [false, true];
                    let mut actions = vec![];

                    for rotate in rotates {
                        actions.extend(self.gen_up_action(None, rotate, up_xor));

                        for base in BitSetIterU128::new(up_cands) {
                            actions.extend(self.gen_up_action(Some(base), rotate, up_xor));
                        }
                    }

                    actions
                }

                fn gen_min_topline(&self, rotate: bool) -> Action {
                    // 無効な候補の列挙
                    let up_xor = self.get_up_invalid_bases(self.turn);
                    let up_cands = self.avaliable_base_up & !up_xor;

                    // Toplineが最も低くなる場所を探す
                    // 壁に寄せたUPは常にvalid
                    let mut best_action = self.gen_up_action(None, rotate, up_xor).unwrap();
                    let mut best_score = best_action.top_value();

                    for action in BitSetIterU128::new(up_cands)
                        .map(|base| self.gen_up_action(Some(base), rotate, up_xor))
                        .flatten()
                    {
                        let score = action.top_value();
                        if best_score.change_min(score) {
                            best_action = action;
                        }
                    }

                    best_action
                }

                fn gen_up_action(
                    &self,
                    base: Option<usize>,
                    rotate: bool,
                    available_base_up_xor: u128,
                ) -> Option<Action> {
                    // 下からrectを置くので、水平・垂直を入れ替える
                    let turn = self.turn;
                    let rect_h = self.rects_w[turn].into();
                    let rect_w = self.rects_h[turn].into();
                    let placements_x0 = &self.placements_y0[..turn];
                    let placements_x1 = &self.placements_y1[..turn];
                    let placements_y0 = &self.placements_x0[..turn];
                    let placements_y1 = &self.placements_x1[..turn];
                    let heights = self.widths.into();
                    let widths = self.heights.into();
                    let width_limit = self.height_limit;
                    let height_limit = self.width_limit;

                    unsafe {
                        self.gen_action(
                            turn,
                            base,
                            rotate,
                            rect_h,
                            rect_w,
                            placements_x0,
                            placements_x1,
                            placements_y0,
                            placements_y1,
                            heights,
                            widths,
                            available_base_up_xor,
                            width_limit,
                            height_limit,
                            true,
                        )
                    }
                }

                /// 右からrectを置く候補を生成する
                /// 下から置く場合はx, yをflipして呼び出し、返り値を再度flipすればよい
                #[target_feature(enable = "avx,avx2")]
                unsafe fn gen_action(
                    &self,
                    turn: usize,
                    base: Option<usize>,
                    rotate: bool,
                    mut rect_h: AlignedU16,
                    mut rect_w: AlignedU16,
                    placements_x0: &[AlignedU16],
                    placements_x1: &[AlignedU16],
                    placements_y0: &[AlignedU16],
                    placements_y1: &[AlignedU16],
                    heights: AlignedU16,
                    widths: AlignedU16,
                    available_base_up_xor: u128,
                    width_limit: AlignedU16,
                    height_limit: AlignedU16,
                    flip: bool,
                ) -> Option<Action> {
                    if rotate {
                        std::mem::swap(&mut rect_w, &mut rect_h);
                    }
                    let rect_w = rect_w.load();
                    let rect_h = rect_h.load();

                    let y0 = match base {
                        Some(index) => placements_y1[index].load(),
                        None => _mm256_setzero_si256(),
                    };
                    let y1 = _mm256_add_epi16(y0, rect_h);

                    // 長方形がどこに置かれるかを調べる
                    let mut x0 = _mm256_setzero_si256();

                    for (p_x1, p_y0, p_y1) in izip!(placements_x1, placements_y0, placements_y1) {
                        // やっていることはジャッジコードと同じ
                        //
                        // let x = if y0.max(p_y0) < y1.min(p_y1) {
                        //     p_x1
                        // } else {
                        //     0
                        // };
                        //
                        // x0 = x0.max(x);
                        let max_y0 = _mm256_max_epu16(y0, p_y0.load());
                        let min_y1 = _mm256_min_epu16(y1, p_y1.load());

                        let gt = _mm256_cmpgt_epi16(min_y1, max_y0);
                        let x = _mm256_and_si256(p_x1.load(), gt);
                        x0 = _mm256_max_epu16(x0, x);
                    }

                    let x1 = _mm256_add_epi16(x0, rect_w);

                    // 1つでもリミット超えたらNG
                    let width_limit = _mm256_cmpgt_epi16(x1, width_limit.load());
                    let height_limit = _mm256_cmpgt_epi16(y1, height_limit.load());
                    let over_limit = _mm256_or_si256(width_limit, height_limit);
                    let over_limit = _mm256_movemask_epi8(over_limit);

                    if over_limit != 0 {
                        return None;
                    }

                    // 側面がピッタリくっついているかチェック
                    let is_touching = match base {
                        Some(base) => {
                            // 側面が対象の長方形とmin(w0, w1) / 4以上隣接していることを要求する
                            let p_x0 = placements_x0[base].load();
                            let p_x1 = placements_x1[base].load();

                            // 隣接長さを求める
                            let max_x0 = _mm256_max_epu16(x0, p_x0);
                            let min_x1 = _mm256_min_epu16(x1, p_x1);
                            let touching_len = _mm256_subs_epu16(min_x1, max_x0);

                            // min(配置した長方形の幅, 対象の長方形の幅) / 4を要求値とする
                            let p_width = _mm256_sub_epi16(p_x1, p_x0);
                            let min_edge = _mm256_min_epu16(rect_w, p_width);
                            let required_len = _mm256_srli_epi16(min_edge, 2);

                            // 要求値と比較
                            let gt = _mm256_cmpgt_epi16(touching_len, required_len);
                            gt
                        }
                        None => {
                            // a == a を行うことで全bitが1になる
                            _mm256_cmpeq_epi16(x0, x0)
                        }
                    };

                    // ピッタリくっついているものが閾値未満だったらNG
                    // 右シフトで0xffを0x01に変換してから足すことで個数をカウントできる
                    let is_touching = _mm256_srli_epi16(is_touching, 15);
                    let touching_count = horizontal_add_u16(is_touching) as usize;

                    if touching_count < Params::get().borrow().touching_threshold {
                        return None;
                    }

                    let heights = heights.load();
                    let widths = widths.load();
                    let new_width = _mm256_max_epu16(x1, widths);
                    let new_height = _mm256_max_epu16(y1, heights);

                    let op = Op::new(turn, rotate, Dir::Left, base);

                    let avaliable_base_up_xor = available_base_up_xor | (1 << turn);

                    Some(Action::new(
                        x0.into(),
                        x1.into(),
                        y0.into(),
                        y1.into(),
                        new_width.into(),
                        new_height.into(),
                        op,
                        avaliable_base_up_xor,
                        flip,
                    ))
                }

                fn get_up_invalid_bases(&self, turn: usize) -> u128 {
                    if turn == 0 {
                        return 0;
                    }

                    // x, yを反転させて呼び出す
                    let prev_turn = turn - 1;
                    let placements_x1 = &self.placements_y1;
                    let placements_y1 = &self.placements_x1;

                    let new_x1 = self.placements_y1[prev_turn];
                    let new_y0 = self.placements_x0[prev_turn];
                    let new_y1 = self.placements_x1[prev_turn];
                    let available_flag = self.avaliable_base_up;
                    let width_limit = self.height_limit;
                    let min_rect_size = self.min_rect_size;

                    unsafe {
                        Self::get_invalid_bases(
                            placements_x1,
                            placements_y1,
                            new_x1,
                            new_y0,
                            new_y1,
                            available_flag,
                            width_limit,
                            min_rect_size,
                        )
                    }
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn get_invalid_bases(
                    placements_x1: &[AlignedU16],
                    placements_y1: &[AlignedU16],
                    new_x1: AlignedU16,
                    new_y0: AlignedU16,
                    new_y1: AlignedU16,
                    available_flag: u128,
                    width_limit: AlignedU16,
                    min_rect_size: AlignedU16,
                ) -> u128 {
                    unsafe {
                        let mut invalid_flag = 0;
                        let new_x1 = new_x1.load();
                        let new_y0 = new_y0.load();
                        let new_y1 = new_y1.load();
                        let width_limit = width_limit.load();
                        let min_rect_size = min_rect_size.load();

                        for rect_i in BitSetIterU128::new(available_flag) {
                            let xi1 = placements_x1[rect_i].load();
                            let yi1 = placements_y1[rect_i].load();

                            // 条件1: rect_iにピッタリくっつけられる可能性がないとダメ
                            //        つまりrect_iの右側に邪魔者がいるとダメ
                            // Yi1 + MIN_RECT_SIZE
                            let yi1pl0 = _mm256_add_epi16(yi1, min_rect_size);

                            // max(yi1, yj0) < min(yi1 + l0, yj1)
                            let max = _mm256_max_epu16(yi1, new_y0);
                            let min = _mm256_min_epu16(yi1pl0, new_y1);
                            let pred_y = _mm256_cmpgt_epi16(min, max);

                            // x_i1 < x_j1
                            let invalid_base = _mm256_cmpgt_epi16(new_x1, xi1);

                            // 条件2: 置いたときにwidth_limitを超えるとダメ
                            let next_x = _mm256_add_epi16(new_x1, min_rect_size);
                            let invalid_width_limit = _mm256_cmpgt_epi16(next_x, width_limit);

                            // invalidなものが閾値を超えていたらNG
                            let invalid = _mm256_or_si256(invalid_base, invalid_width_limit);
                            let invalid = _mm256_and_si256(pred_y, invalid);
                            let invalid = _mm256_srli_epi16(invalid, 15);
                            let invalid_cnt = horizontal_add_u16(invalid) as usize;
                            let is_invalid =
                                invalid_cnt > Params::get().borrow().invalid_cnt_threshold;
                            invalid_flag |= (is_invalid as u128) << rect_i;
                        }

                        invalid_flag
                    }
                }
            }

            #[derive(Debug, Clone, Copy, Default)]
            struct Action {
                placements_x0: AlignedU16,
                placements_x1: AlignedU16,
                placements_y0: AlignedU16,
                placements_y1: AlignedU16,
                new_widths: AlignedU16,
                new_heights: AlignedU16,
                op: Op,
                avaliable_base_up_xor: u128,
            }

            impl Action {
                fn new(
                    mut placements_x0: AlignedU16,
                    mut placements_x1: AlignedU16,
                    mut placements_y0: AlignedU16,
                    mut placements_y1: AlignedU16,
                    mut new_widths: AlignedU16,
                    mut new_heights: AlignedU16,
                    mut op: Op,
                    avaliable_base_up_xor: u128,
                    flip: bool,
                ) -> Self {
                    if flip {
                        std::mem::swap(&mut placements_x0, &mut placements_y0);
                        std::mem::swap(&mut placements_x1, &mut placements_y1);
                        std::mem::swap(&mut new_widths, &mut new_heights);
                        let dir = match op.dir() {
                            Dir::Left => Dir::Up,
                            Dir::Up => Dir::Left,
                        };

                        op = Op::new(op.rect_idx(), op.rotate(), dir, op.base());
                    }

                    Self {
                        placements_x0,
                        placements_x1,
                        placements_y0,
                        placements_y1,
                        new_widths,
                        new_heights,
                        op,
                        avaliable_base_up_xor,
                    }
                }

                fn apply(&self, state: &mut State) {
                    state.placements_x0[state.turn] = self.placements_x0;
                    state.placements_x1[state.turn] = self.placements_x1;
                    state.placements_y0[state.turn] = self.placements_y0;
                    state.placements_y1[state.turn] = self.placements_y1;
                    state.widths = self.new_widths;
                    state.heights = self.new_heights;
                    state.turn += 1;
                    state.avaliable_base_up ^= self.avaliable_base_up_xor;
                }

                fn top_value(&self) -> u32 {
                    unsafe {
                        let y1_sum = horizontal_add_u16(self.placements_y1.load());
                        y1_sum
                    }
                }
            }
        }
        pub(super) mod multi_beam_simd {
            use crate::{
                beam::{self, BayesianBeamWidthSuggester},
                problem::{params::Params, Dir, Input, Op},
                solver::simd::*,
                util::BitSetIterU128,
            };
            use itertools::izip;
            use ordered_float::OrderedFloat;
            use std::arch::x86_64::*;

            /// 多変量正規分布からN個の長方形を16インスタンス生成し、それぞれについて並行に操作を行うビームサーチ。
            /// 考え方は粒子フィルタなどと同じで、非線形性を考慮するために多数のインスタンスでシミュレートする。
            /// 内部的にAVX2を使用して高速化している。
            pub struct MultiBeamArrangerSimd;

            impl MultiBeamArrangerSimd {
                pub fn arrange(
                    &mut self,
                    input: &Input,
                    end_turn: usize,
                    rects: SimdRectSet,
                    duration_sec: f64,
                ) -> Vec<Op> {
                    let since = std::time::Instant::now();

                    let large_state = unsafe { LargeState::new(input.clone(), rects) };
                    let small_state = SmallState::default();
                    let act_gen = ActGen::new();

                    let remaining_time = (duration_sec - since.elapsed().as_secs_f64()).max(0.01);
                    let mut beam = beam::BeamSearch::new(act_gen);
                    let standard_beam_width = 100_000 / (input.rect_cnt() as usize);
                    let beam_width_suggester = BayesianBeamWidthSuggester::new(
                        input.rect_cnt(),
                        5,
                        remaining_time,
                        standard_beam_width,
                        1,
                        30000,
                        0,
                    );
                    let deduplicator = beam::NoOpDeduplicator;
                    let (ops, _) = beam.run(
                        large_state,
                        small_state,
                        end_turn,
                        beam_width_suggester,
                        deduplicator,
                    );

                    ops
                }
            }

            #[derive(Debug, Clone)]
            struct LargeState {
                // 256bitのレジスタをunsigned 16bit integer x16で使う
                rects_h: [AlignedU16; Input::MAX_RECT_CNT],
                rects_w: [AlignedU16; Input::MAX_RECT_CNT],
                widths: AlignedU16,
                heights: AlignedU16,
                width_limit: AlignedU16,
                height_limit: AlignedU16,
                placements_x0: [AlignedU16; Input::MAX_RECT_CNT],
                placements_x1: [AlignedU16; Input::MAX_RECT_CNT],
                placements_y0: [AlignedU16; Input::MAX_RECT_CNT],
                placements_y1: [AlignedU16; Input::MAX_RECT_CNT],
                turn: usize,
                avaliable_base_up: u128,
                min_rect_size: AlignedU16,
            }

            impl LargeState {
                #[target_feature(enable = "avx,avx2")]
                unsafe fn new(input: Input, rects: SimdRectSet) -> Self {
                    let mut rects_h = [AlignedU16::ZERO; Input::MAX_RECT_CNT];
                    let mut rects_w = [AlignedU16::ZERO; Input::MAX_RECT_CNT];

                    for i in 0..input.rect_cnt() {
                        rects_h[i] = AlignedU16(rects.heights[i]);
                        rects_w[i] = AlignedU16(rects.widths[i]);
                    }

                    let placements = [AlignedU16::ZERO; Input::MAX_RECT_CNT];

                    // 最初から幅を確保しておく
                    let mut areas = [0; AVX2_U16_W];

                    for (h, w) in izip!(&rects.heights, &rects.widths) {
                        for i in 0..AVX2_U16_W {
                            let w = w[i] as u64;
                            let h = h[i] as u64;
                            areas[i] += w * h;
                        }
                    }

                    // 少し余裕を持たせる
                    let default_width = AlignedU16(
                        areas.map(|a| (a as f64 * Params::get().borrow().width_buf).sqrt() as u16),
                    );

                    let width_limit = default_width;
                    let height_limit = AlignedU16([u16::MAX / 2; AVX2_U16_W]);

                    let min_rect_size = unsafe {
                        let mut min = _mm256_set1_epi16(i16::MAX);

                        for size in rects_h[..input.rect_cnt()]
                            .iter()
                            .chain(rects_w[..input.rect_cnt()].iter())
                        {
                            min = _mm256_min_epu16(min, size.load());
                        }

                        let mut min_rect_size: [u16; AVX2_U16_W] = [0; AVX2_U16_W];
                        _mm256_storeu_si256(min_rect_size.as_mut_ptr() as *mut __m256i, min);

                        AlignedU16(min_rect_size)
                    };

                    Self {
                        rects_h,
                        rects_w,
                        widths: default_width,
                        heights: AlignedU16::ZERO,
                        width_limit,
                        height_limit,
                        placements_x0: placements.clone(),
                        placements_x1: placements.clone(),
                        placements_y0: placements.clone(),
                        placements_y1: placements.clone(),
                        turn: 0,
                        avaliable_base_up: 0,
                        min_rect_size,
                    }
                }
            }

            #[derive(Debug, Clone)]
            struct SmallState {
                placements_x0: AlignedU16,
                placements_x1: AlignedU16,
                placements_y0: AlignedU16,
                placements_y1: AlignedU16,
                old_widths: AlignedU16,
                old_heights: AlignedU16,
                new_widths: AlignedU16,
                new_heights: AlignedU16,
                op: Op,
                score: f32,
                avaliable_base_up_xor: u128,
            }

            impl SmallState {
                fn new(
                    mut placements_x0: AlignedU16,
                    mut placements_x1: AlignedU16,
                    mut placements_y0: AlignedU16,
                    mut placements_y1: AlignedU16,
                    mut old_widths: AlignedU16,
                    mut old_heights: AlignedU16,
                    mut new_widths: AlignedU16,
                    mut new_heights: AlignedU16,
                    mut op: Op,
                    avaliable_base_up_xor: u128,
                    flip: bool,
                ) -> Self {
                    if flip {
                        std::mem::swap(&mut placements_x0, &mut placements_y0);
                        std::mem::swap(&mut placements_x1, &mut placements_y1);
                        std::mem::swap(&mut old_widths, &mut old_heights);
                        std::mem::swap(&mut new_widths, &mut new_heights);
                        let dir = match op.dir() {
                            Dir::Left => Dir::Up,
                            Dir::Up => Dir::Left,
                        };

                        op = Op::new(op.rect_idx(), op.rotate(), dir, op.base());
                    }

                    let score = unsafe { Self::calc_score(new_heights) };

                    Self {
                        placements_x0,
                        placements_x1,
                        placements_y0,
                        placements_y1,
                        old_widths,
                        old_heights,
                        new_widths,
                        new_heights,
                        op,
                        score,
                        avaliable_base_up_xor,
                    }
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn calc_score(height: AlignedU16) -> f32 {
                    // スコアの昇順にソートした上で、減衰させながら和を取る
                    // （期待値を最大化するよりは上振れを狙いたいため）
                    thread_local!(static SCORE_MUL: [AlignedF32; 2] = {
                        let score_mul = Params::get().borrow().parallel_score_mul;
                        [
                            AlignedF32(std::array::from_fn(|i| score_mul.powi(i as i32))),
                            AlignedF32(std::array::from_fn(|i| {
                                score_mul.powi((i + AVX2_F32_W) as i32)
                            })),
                        ]
                    });

                    SCORE_MUL.with(|score_mul| {
                        let score = bitonic_sort_u16(height.load());

                        // u16 x 16 -> (u32 x 8) x 2 -> (f32 x 8) x 2
                        let score_low = _mm256_castsi256_si128(score);
                        let score_high = _mm256_extracti128_si256(score, 1);
                        let score_u32_low = _mm256_cvtepu16_epi32(score_low);
                        let score_u32_high = _mm256_cvtepu16_epi32(score_high);
                        let score_f32_low = _mm256_cvtepi32_ps(score_u32_low);
                        let score_f32_high = _mm256_cvtepi32_ps(score_u32_high);

                        // 用意していた係数とかける
                        let score_mul_low = score_mul[0].load();
                        let score_mul_high = score_mul[1].load();
                        let score_low = _mm256_mul_ps(score_f32_low, score_mul_low);
                        let score_high = _mm256_mul_ps(score_f32_high, score_mul_high);
                        let score = _mm256_add_ps(score_low, score_high);
                        let score = horizontal_add_f32(score);
                        -score
                    })
                }
            }

            impl Default for SmallState {
                fn default() -> Self {
                    Self {
                        placements_x0: AlignedU16::ZERO,
                        placements_x1: AlignedU16::ZERO,
                        placements_y0: AlignedU16::ZERO,
                        placements_y1: AlignedU16::ZERO,
                        old_widths: AlignedU16::ZERO,
                        old_heights: AlignedU16::ZERO,
                        new_widths: AlignedU16::ZERO,
                        new_heights: AlignedU16::ZERO,
                        op: Default::default(),
                        score: Default::default(),
                        avaliable_base_up_xor: Default::default(),
                    }
                }
            }

            impl beam::SmallState for SmallState {
                type Score = OrderedFloat<f32>;
                type Hash = u32;
                type LargeState = LargeState;
                type Action = Op;

                fn raw_score(&self) -> Self::Score {
                    // 最終ターンは縦と横からスコアを再計算
                    let score_h = unsafe {
                        Self::calc_score(self.new_heights) + Self::calc_score(self.new_widths)
                    };
                    OrderedFloat(score_h)
                }

                fn beam_score(&self) -> Self::Score {
                    OrderedFloat(self.score)
                }

                fn hash(&self) -> Self::Hash {
                    0
                }

                fn apply(&self, state: &mut Self::LargeState) {
                    state.placements_x0[state.turn] = self.placements_x0;
                    state.placements_x1[state.turn] = self.placements_x1;
                    state.placements_y0[state.turn] = self.placements_y0;
                    state.placements_y1[state.turn] = self.placements_y1;
                    state.widths = self.new_widths;
                    state.heights = self.new_heights;
                    state.turn += 1;
                    state.avaliable_base_up ^= self.avaliable_base_up_xor;
                }

                fn rollback(&self, state: &mut Self::LargeState) {
                    // placementsは削除しなくても良い
                    state.widths = self.old_widths;
                    state.heights = self.old_heights;
                    state.turn -= 1;
                    state.avaliable_base_up ^= self.avaliable_base_up_xor;
                }

                fn action(&self) -> Self::Action {
                    self.op
                }
            }

            struct ActGen;

            impl ActGen {
                fn new() -> Self {
                    Self
                }

                // 以前は左向きのrectを置いたりしていたので、その名残で残っている
                fn gen_up_cand(
                    &self,
                    large_state: &LargeState,
                    base: Option<usize>,
                    rotate: bool,
                    available_base_up_xor: u128,
                ) -> Option<SmallState> {
                    // 下からrectを置くので、水平・垂直を入れ替える
                    let turn = large_state.turn;
                    let rect_h = large_state.rects_w[turn].into();
                    let rect_w = large_state.rects_h[turn].into();
                    let placements_x0 = &large_state.placements_y0[..turn];
                    let placements_x1 = &large_state.placements_y1[..turn];
                    let placements_y0 = &large_state.placements_x0[..turn];
                    let placements_y1 = &large_state.placements_x1[..turn];
                    let heights = large_state.widths.into();
                    let widths = large_state.heights.into();
                    let width_limit = large_state.height_limit;
                    let height_limit = large_state.width_limit;

                    unsafe {
                        self.gen_cand(
                            turn,
                            base,
                            rotate,
                            rect_h,
                            rect_w,
                            placements_x0,
                            placements_x1,
                            placements_y0,
                            placements_y1,
                            heights,
                            widths,
                            available_base_up_xor,
                            width_limit,
                            height_limit,
                            true,
                        )
                    }
                }

                /// 右からrectを置く候補を生成する
                /// 下から置く場合はx, yをflipして呼び出し、返り値を再度flipすればよい
                #[target_feature(enable = "avx,avx2")]
                unsafe fn gen_cand(
                    &self,
                    turn: usize,
                    base: Option<usize>,
                    rotate: bool,
                    mut rect_h: AlignedU16,
                    mut rect_w: AlignedU16,
                    placements_x0: &[AlignedU16],
                    placements_x1: &[AlignedU16],
                    placements_y0: &[AlignedU16],
                    placements_y1: &[AlignedU16],
                    heights: AlignedU16,
                    widths: AlignedU16,
                    available_base_up_xor: u128,
                    width_limit: AlignedU16,
                    height_limit: AlignedU16,
                    flip: bool,
                ) -> Option<SmallState> {
                    if rotate {
                        std::mem::swap(&mut rect_w, &mut rect_h);
                    }
                    let rect_w = rect_w.load();
                    let rect_h = rect_h.load();

                    let y0 = match base {
                        Some(index) => placements_y1[index].load(),
                        None => _mm256_setzero_si256(),
                    };
                    let y1 = _mm256_add_epi16(y0, rect_h);

                    // 長方形がどこに置かれるかを調べる
                    let mut x0 = _mm256_setzero_si256();

                    for (p_x1, p_y0, p_y1) in izip!(placements_x1, placements_y0, placements_y1) {
                        // やっていることはジャッジコードと同じ
                        //
                        // let x = if y0.max(p_y0) < y1.min(p_y1) {
                        //     p_x1
                        // } else {
                        //     0
                        // };
                        //
                        // x0 = x0.max(x);
                        let max_y0 = _mm256_max_epu16(y0, p_y0.load());
                        let min_y1 = _mm256_min_epu16(y1, p_y1.load());

                        let gt = _mm256_cmpgt_epi16(min_y1, max_y0);
                        let x = _mm256_and_si256(p_x1.load(), gt);
                        x0 = _mm256_max_epu16(x0, x);
                    }

                    let x1 = _mm256_add_epi16(x0, rect_w);

                    // 1つでもリミット超えたらNG
                    let width_limit = _mm256_cmpgt_epi16(x1, width_limit.load());
                    let height_limit = _mm256_cmpgt_epi16(y1, height_limit.load());
                    let over_limit = _mm256_or_si256(width_limit, height_limit);
                    let over_limit = _mm256_movemask_epi8(over_limit);

                    if over_limit != 0 {
                        return None;
                    }

                    // 側面がピッタリくっついているかチェック
                    let is_touching = match base {
                        Some(base) => {
                            // 側面が対象の長方形とmin(w0, w1) / 4以上隣接していることを要求する
                            let p_x0 = placements_x0[base].load();
                            let p_x1 = placements_x1[base].load();

                            // 隣接長さを求める
                            let max_x0 = _mm256_max_epu16(x0, p_x0);
                            let min_x1 = _mm256_min_epu16(x1, p_x1);
                            let touching_len = _mm256_subs_epu16(min_x1, max_x0);

                            // min(配置した長方形の幅, 対象の長方形の幅) / 4を要求値とする
                            let p_width = _mm256_sub_epi16(p_x1, p_x0);
                            let min_edge = _mm256_min_epu16(rect_w, p_width);
                            let required_len = _mm256_srli_epi16(min_edge, 2);

                            // 要求値と比較
                            let gt = _mm256_cmpgt_epi16(touching_len, required_len);
                            gt
                        }
                        None => {
                            // a == a を行うことで全bitが1になる
                            _mm256_cmpeq_epi16(x0, x0)
                        }
                    };

                    // ピッタリくっついているものが閾値未満だったらNG
                    // 右シフトで0xffを0x01に変換してから足すことで個数をカウントできる
                    let is_touching = _mm256_srli_epi16(is_touching, 15);
                    let touching_count = horizontal_add_u16(is_touching) as usize;

                    if touching_count < Params::get().borrow().touching_threshold {
                        return None;
                    }

                    let heights = heights.load();
                    let widths = widths.load();
                    let new_width = _mm256_max_epu16(x1, widths);
                    let new_height = _mm256_max_epu16(y1, heights);

                    let op = Op::new(turn, rotate, Dir::Left, base);

                    let avaliable_base_up_xor = available_base_up_xor | (1 << turn);

                    Some(SmallState::new(
                        x0.into(),
                        x1.into(),
                        y0.into(),
                        y1.into(),
                        widths.into(),
                        heights.into(),
                        new_width.into(),
                        new_height.into(),
                        op,
                        avaliable_base_up_xor,
                        flip,
                    ))
                }

                fn get_up_invalid_bases(large_state: &LargeState, turn: usize) -> u128 {
                    if turn == 0 {
                        return 0;
                    }

                    // x, yを反転させて呼び出す
                    let prev_turn = turn - 1;
                    let placements_x1 = &large_state.placements_y1;
                    let placements_y1 = &large_state.placements_x1;

                    let new_x1 = large_state.placements_y1[prev_turn];
                    let new_y0 = large_state.placements_x0[prev_turn];
                    let new_y1 = large_state.placements_x1[prev_turn];
                    let available_flag = large_state.avaliable_base_up;
                    let width_limit = large_state.height_limit;
                    let min_rect_size = large_state.min_rect_size;

                    unsafe {
                        Self::get_invalid_bases(
                            placements_x1,
                            placements_y1,
                            new_x1,
                            new_y0,
                            new_y1,
                            available_flag,
                            width_limit,
                            min_rect_size,
                        )
                    }
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn get_invalid_bases(
                    placements_x1: &[AlignedU16],
                    placements_y1: &[AlignedU16],
                    new_x1: AlignedU16,
                    new_y0: AlignedU16,
                    new_y1: AlignedU16,
                    available_flag: u128,
                    width_limit: AlignedU16,
                    min_rect_size: AlignedU16,
                ) -> u128 {
                    unsafe {
                        let mut invalid_flag = 0;
                        let new_x1 = new_x1.load();
                        let new_y0 = new_y0.load();
                        let new_y1 = new_y1.load();
                        let width_limit = width_limit.load();
                        let min_rect_size = min_rect_size.load();

                        for rect_i in BitSetIterU128::new(available_flag) {
                            let xi1 = placements_x1[rect_i].load();
                            let yi1 = placements_y1[rect_i].load();

                            // 条件1: rect_iにピッタリくっつけられる可能性がないとダメ
                            //        つまりrect_iの右側に邪魔者がいるとダメ
                            // Yi1 + MIN_RECT_SIZE
                            let yi1pl0 = _mm256_add_epi16(yi1, min_rect_size);

                            // max(yi1, yj0) < min(yi1 + l0, yj1)
                            let max = _mm256_max_epu16(yi1, new_y0);
                            let min = _mm256_min_epu16(yi1pl0, new_y1);
                            let pred_y = _mm256_cmpgt_epi16(min, max);

                            // x_i1 < x_j1
                            let invalid_base = _mm256_cmpgt_epi16(new_x1, xi1);

                            // 条件2: 置いたときにwidth_limitを超えるとダメ
                            let next_x = _mm256_add_epi16(new_x1, min_rect_size);
                            let invalid_width_limit = _mm256_cmpgt_epi16(next_x, width_limit);

                            // invalidなものが閾値を超えていたらNG
                            let invalid = _mm256_or_si256(invalid_base, invalid_width_limit);
                            let invalid = _mm256_and_si256(pred_y, invalid);
                            let invalid = _mm256_srli_epi16(invalid, 15);
                            let invalid_cnt = horizontal_add_u16(invalid) as usize;
                            let is_invalid =
                                invalid_cnt > Params::get().borrow().invalid_cnt_threshold;
                            invalid_flag |= (is_invalid as u128) << rect_i;
                        }

                        invalid_flag
                    }
                }
            }

            impl beam::ActGen<SmallState> for ActGen {
                fn generate(
                    &self,
                    _small_state: &SmallState,
                    large_state: &<SmallState as beam::SmallState>::LargeState,
                    next_states: &mut Vec<SmallState>,
                ) {
                    // 無効な候補の列挙
                    let invalid_bases_up =
                        Self::get_up_invalid_bases(large_state, large_state.turn);
                    let avaliable_bases_up = large_state.avaliable_base_up & !invalid_bases_up;

                    // 生成
                    let rotates = [false, true];

                    for rotate in rotates {
                        // Up
                        next_states.extend(self.gen_up_cand(
                            &large_state,
                            None,
                            rotate,
                            invalid_bases_up,
                        ));

                        for i in BitSetIterU128::new(avaliable_bases_up) {
                            next_states.extend(self.gen_up_cand(
                                &large_state,
                                Some(i),
                                rotate,
                                invalid_bases_up,
                            ));
                        }
                    }
                }
            }
        }
    }
    mod estimator {
        pub mod gauss {
            mod annealing {
                use super::GaussEstimator;
                use crate::{
                    problem::{Dir, Input, Op, Rect},
                    sa::{self},
                    solver::estimator::Placement,
                    util::ChangeMinMax,
                };
                use itertools::Itertools;
                use nalgebra::DVector;
                use rand::prelude::*;
                use smallvec::SmallVec;
                use std::u32;

                pub(super) fn solve(
                    estimator: &GaussEstimator,
                    duration: f64,
                    rng: &mut impl Rng,
                ) -> (Vec<Op>, DVector<f64>, DVector<f64>) {
                    let env = Env::new(estimator.clone());
                    let state = State::init(&env, rng);
                    let annealer = sa::Annealer::new(1e4, 1e2, rng.gen(), 1);
                    let (state, _stats) = annealer.run(&env, state, &NeighGen, duration);

                    //eprintln!("{}", stats);

                    state.to_op_and_edges()
                }

                struct Env {
                    estimator: GaussEstimator,
                    rects: Vec<Rect>,
                }

                impl Env {
                    fn new(estimator: GaussEstimator) -> Self {
                        let rects = (0..estimator.rect_cnt)
                            .map(|i| {
                                let height = estimator.mean_height()[i].round() as u32;
                                let width = estimator.mean_width()[i].round() as u32;
                                let height =
                                    height.clamp(Input::MIN_RECT_SIZE, Input::MAX_RECT_SIZE);
                                let width = width.clamp(Input::MIN_RECT_SIZE, Input::MAX_RECT_SIZE);
                                Rect::new(height, width)
                            })
                            .collect_vec();

                        Self { estimator, rects }
                    }
                }

                #[derive(Debug, Clone)]
                struct State {
                    placements: Vec<Option<(Dir, bool)>>,
                    pivot: usize,
                    calculator_v: ScoreCalculator,
                    calculator_h: ScoreCalculator,
                }

                impl State {
                    fn new(env: &Env, placements: Vec<Option<(Dir, bool)>>, pivot: usize) -> Self {
                        let (vec_v, vec_h) = Self::get_box_vec(&env, &placements, pivot);
                        let calculator_v = ScoreCalculator::new(&env.estimator, vec_v);
                        let calculator_h = ScoreCalculator::new(&env.estimator, vec_h);

                        Self {
                            placements,
                            pivot,
                            calculator_v,
                            calculator_h,
                        }
                    }

                    fn init(env: &Env, rng: &mut impl Rng) -> Self {
                        let mut placements = (0..env.estimator.rect_cnt)
                            .map(|_| {
                                let rotate = rng.gen_bool(0.5);
                                Some(if rng.gen_bool(0.5) {
                                    (Dir::Left, rotate)
                                } else {
                                    (Dir::Up, rotate)
                                })
                            })
                            .collect_vec();
                        placements[0] = Some((Dir::Up, false));
                        let pivot = 0;

                        Self::new(env, placements, pivot)
                    }

                    fn get_box_vec(
                        env: &Env,
                        directions: &[Option<(Dir, bool)>],
                        pivot: usize,
                    ) -> (DVector<f64>, DVector<f64>) {
                        let mut parents_v = vec![None; env.estimator.rect_cnt];
                        let mut parents_h = vec![None; env.estimator.rect_cnt];
                        let mut placements_v = SmallVec::<[(Placement, usize, usize); 5]>::new();
                        let mut placements_h = SmallVec::<[(Placement, usize, usize); 5]>::new();
                        let mut vec_v = DVector::zeros(env.estimator.rect_cnt * 2);
                        let mut vec_h = DVector::zeros(env.estimator.rect_cnt * 2);

                        let mut x = 0;
                        let mut y = 0;
                        let mut pivot_y = u32::MAX / 2;

                        // v, h方向に置いている箱のうち、壁にピッタリくっついているものの番号
                        let mut last_h = None;
                        let mut last_v = None;

                        let mut max_x = 0;
                        let mut max_y = 0;
                        let mut max_x_i = !0;
                        let mut max_y_i = !0;

                        for (i, dir) in directions.iter().enumerate() {
                            let &Some((dir, rotate)) = dir else {
                                continue;
                            };

                            let rect = &env.rects[i];

                            match dir {
                                Dir::Up => {
                                    let (w, h, vec_i) = if rotate {
                                        (rect.height(), rect.width(), i + env.estimator.rect_cnt)
                                    } else {
                                        (rect.width(), rect.height(), i)
                                    };

                                    let x0 = 0;
                                    let x1 = x0 + w;

                                    if y >= Input::MAX_RECT_SIZE + pivot_y {
                                        // 絶対に他の箱と干渉しない

                                        // pivotの場合は水平方向のrootとなる
                                        if pivot == i {
                                            pivot_y = y;
                                            last_h = Some(i);
                                            x = x1;
                                        }

                                        y += h;
                                        parents_v[i] = last_v;
                                        last_v = Some(i);

                                        if max_x.change_max(x1) {
                                            max_x_i = i;
                                        }

                                        if max_y.change_max(y) {
                                            max_y_i = i;
                                        }

                                        continue;
                                    }

                                    let mut y0 = y;
                                    let mut parent = last_v;

                                    if let Some((y, p)) = placements_v
                                        .iter()
                                        .rev()
                                        .filter_map(|(p, i, _)| {
                                            if p.x0 < w {
                                                Some((p.y1, Some(*i)))
                                            } else {
                                                None
                                            }
                                        })
                                        .max()
                                    {
                                        if y0.change_max(y) {
                                            parent = p;
                                        }
                                    }

                                    // pivotの場合は水平方向のrootとなる
                                    if pivot == i {
                                        // vec_vにカウントされるかどうかは未確定
                                        pivot_y = y0;
                                        last_h = Some(i);
                                        x = x1;
                                    }

                                    let y1 = y0 + h;
                                    y = y1;
                                    last_v = Some(i);
                                    parents_v[i] = parent;

                                    if max_x.change_max(x1) {
                                        max_x_i = i;
                                    }

                                    if max_y.change_max(y) {
                                        max_y_i = i;
                                    }

                                    if pivot_y < y0 && y0 < Input::MAX_RECT_SIZE + pivot_y {
                                        let pl = Placement::new(x0, x1, y0, y1);
                                        placements_h.push((pl, i, vec_i));
                                    }
                                }
                                Dir::Left => {
                                    let (w, h, vec_i) = if rotate {
                                        (rect.height(), rect.width(), i)
                                    } else {
                                        (rect.width(), rect.height(), i + env.estimator.rect_cnt)
                                    };

                                    assert!(pivot_y != !0);

                                    let y0 = pivot_y;
                                    let y1 = y0 + h;

                                    if x >= Input::MAX_RECT_SIZE {
                                        // 絶対に他の箱と干渉しない
                                        x += w;
                                        parents_h[i] = last_h;
                                        last_h = Some(i);

                                        if max_x.change_max(x) {
                                            max_x_i = i;
                                        }

                                        if max_y.change_max(y1) {
                                            max_y_i = i;
                                        }

                                        continue;
                                    }

                                    let mut x0 = x;
                                    let mut parent = last_h;

                                    if let Some((x, p)) = placements_h
                                        .iter()
                                        .rev()
                                        .filter_map(|(p, i, _)| {
                                            if p.y0 < h + pivot_y {
                                                Some((p.x1, Some(*i)))
                                            } else {
                                                None
                                            }
                                        })
                                        .max()
                                    {
                                        if x0.change_max(x) {
                                            parent = p;
                                        }
                                    }

                                    let x1 = x0 + w;
                                    x = x1;
                                    last_h = Some(i);
                                    parents_h[i] = parent;
                                    parents_v[i] = parents_v[pivot];

                                    if max_x.change_max(x) {
                                        max_x_i = i;
                                    }

                                    if max_y.change_max(y1) {
                                        max_y_i = i;
                                    }

                                    if 0 < x0 && x0 < Input::MAX_RECT_SIZE {
                                        let pl = Placement::new(x0, x1, y0, y1);
                                        placements_v.push((pl, i, vec_i));
                                    }
                                }
                            }
                        }

                        let mut v = Some(max_y_i);

                        while let Some(i) = v {
                            let vec_i = if directions[i].unwrap().1 {
                                i + env.estimator.rect_cnt
                            } else {
                                i
                            };
                            vec_v[vec_i] = 1.0;
                            v = parents_v[i];
                        }

                        let mut v = Some(max_x_i);

                        while let Some(i) = v {
                            let vec_i = if directions[i].unwrap().1 {
                                i
                            } else {
                                i + env.estimator.rect_cnt
                            };
                            vec_h[vec_i] = 1.0;
                            v = parents_h[i];
                        }

                        (vec_v, vec_h)
                    }

                    fn to_op_and_edges(&self) -> (Vec<Op>, DVector<f64>, DVector<f64>) {
                        let mut ops = vec![];
                        let mut prev = None;
                        let mut base = None;

                        for (i, &placement) in self.placements.iter().enumerate() {
                            let Some((dir, rotate)) = placement else {
                                continue;
                            };

                            if i == self.pivot {
                                base = prev;
                            }

                            prev = Some(i);

                            let base = match dir {
                                Dir::Up => None,
                                Dir::Left => base,
                            };

                            let op = Op::new(i, rotate, dir, base);
                            ops.push(op);
                        }

                        (
                            ops,
                            self.calculator_v.v.clone(),
                            self.calculator_h.v.clone(),
                        )
                    }
                }

                impl sa::State for State {
                    type Env = Env;
                    type Score = ScoreF64;

                    fn score(&self, _env: &Self::Env) -> Self::Score {
                        ScoreF64(self.calculator_v.score() + self.calculator_h.score())
                    }
                }

                #[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
                struct ScoreF64(f64);

                impl sa::Score for ScoreF64 {
                    fn raw_score(&self) -> f64 {
                        self.0
                    }
                }

                #[derive(Debug, Clone)]
                struct ScoreCalculator {
                    v: DVector<f64>,
                    sigma_c_sum: DVector<f64>,
                    tr_numerator: f64,
                    tr_denominator: f64,
                }

                impl ScoreCalculator {
                    fn new(estimator: &GaussEstimator, v: DVector<f64>) -> Self {
                        let tr_numerator = (&estimator.variance * &v).norm_squared();
                        let tr_denominator = (v.transpose() * &estimator.variance * &v)[0]
                            + estimator.measure_variance;
                        let sigma_c_sum = &estimator.variance * &v;

                        Self {
                            v,
                            sigma_c_sum,
                            tr_numerator,
                            tr_denominator,
                        }
                    }

                    fn update(&mut self, estimator: &GaussEstimator, new_v: &mut DVector<f64>) {
                        let delta = &*new_v - &self.v;

                        for (i, &dv) in delta.iter().enumerate() {
                            if dv == 0.0 {
                                continue;
                            }

                            self.tr_numerator +=
                                2.0 * dv * (estimator.variance.column(i).dot(&self.sigma_c_sum));
                            self.tr_numerator +=
                                dv * dv * estimator.variance.column(i).norm_squared();

                            self.tr_denominator +=
                                2.0 * dv * (estimator.variance.column(i).dot(&self.v));
                            self.tr_denominator += dv * dv * estimator.variance[(i, i)];

                            self.sigma_c_sum += dv * &estimator.variance.column(i);

                            // swapする
                            let temp = new_v[i];
                            new_v[i] = self.v[i];
                            self.v[i] = temp;
                        }
                    }

                    fn score(&self) -> f64 {
                        self.tr_numerator / self.tr_denominator
                    }
                }

                struct NeighGen;

                impl sa::NeighborGenerator for NeighGen {
                    type Env = Env;
                    type State = State;

                    fn generate(
                        &self,
                        env: &Self::Env,
                        state: &Self::State,
                        rng: &mut impl Rng,
                    ) -> Box<dyn sa::Neighbor<Env = Self::Env, State = Self::State>>
                    {
                        loop {
                            if rng.gen_bool(0.9) {
                                return ChangeDirNeigh::gen(env, state, rng);
                            } else {
                                if let Some(neigh) = ChangePivotNeigh::gen(env, state, rng) {
                                    return neigh;
                                }
                            }
                        }
                    }
                }

                #[derive(Debug, Clone)]
                struct ChangeDirNeigh {
                    index: usize,
                    placement: Option<(Dir, bool)>,
                    old_placement: Option<(Dir, bool)>,
                    vec_v: Option<DVector<f64>>,
                    vec_h: Option<DVector<f64>>,
                }

                impl ChangeDirNeigh {
                    fn gen(
                        env: &Env,
                        state: &State,
                        rng: &mut impl Rng,
                    ) -> Box<dyn sa::Neighbor<Env = Env, State = State>> {
                        loop {
                            let index = rng.gen_range(0..env.estimator.rect_cnt);
                            let placement = if rng.gen_bool(0.1) && index != state.pivot {
                                None
                            } else {
                                let dir = if rng.gen_bool(0.5) {
                                    Dir::Up
                                } else {
                                    Dir::Left
                                };

                                if index <= state.pivot && dir == Dir::Left {
                                    continue;
                                }

                                let rotate = rng.gen_bool(0.5);

                                Some((dir, rotate))
                            };

                            if state.placements[index] != placement {
                                let mut placements = state.placements.clone();
                                placements[index] = placement;

                                return Box::new(Self {
                                    index,
                                    placement,
                                    old_placement: state.placements[index],
                                    vec_v: None,
                                    vec_h: None,
                                });
                            }
                        }
                    }
                }

                impl sa::Neighbor for ChangeDirNeigh {
                    type Env = Env;
                    type State = State;

                    fn preprocess(&mut self, env: &Self::Env, state: &mut Self::State) {
                        state.placements[self.index] = self.placement;

                        let (mut vec_v, mut vec_h) =
                            State::get_box_vec(&env, &state.placements, state.pivot);
                        state.calculator_v.update(&env.estimator, &mut vec_v);
                        state.calculator_h.update(&env.estimator, &mut vec_h);

                        self.vec_v = Some(vec_v);
                        self.vec_h = Some(vec_h);
                    }

                    fn postprocess(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State) {
                        // do nothing
                    }

                    fn rollback(self: Box<Self>, env: &Self::Env, state: &mut Self::State) {
                        state.placements[self.index] = self.old_placement;

                        state
                            .calculator_v
                            .update(&env.estimator, &mut self.vec_v.unwrap());
                        state
                            .calculator_h
                            .update(&env.estimator, &mut self.vec_h.unwrap());
                    }
                }

                #[derive(Debug, Clone)]
                struct ChangePivotNeigh {
                    old_pivot: usize,
                    new_pivot: usize,
                    vec_v: Option<DVector<f64>>,
                    vec_h: Option<DVector<f64>>,
                }

                impl ChangePivotNeigh {
                    fn gen(
                        env: &Env,
                        state: &State,
                        rng: &mut impl Rng,
                    ) -> Option<Box<dyn sa::Neighbor<Env = Env, State = State>>>
                    {
                        let new_pivot = if rng.gen_bool(0.5) {
                            (0..state.pivot)
                                .rev()
                                .filter(|&i| state.placements[i].is_some())
                                .next()
                        } else {
                            (state.pivot + 1..env.estimator.rect_cnt)
                                .filter(|&i| state.placements[i].is_some())
                                .next()
                        };

                        let Some(new_pivot) = new_pivot else {
                            return None;
                        };

                        if state.placements[new_pivot].unwrap().0 == Dir::Left {
                            return None;
                        }

                        let old_pivot = state.pivot;

                        Some(Box::new(Self {
                            old_pivot,
                            new_pivot,
                            vec_v: None,
                            vec_h: None,
                        }))
                    }
                }

                impl sa::Neighbor for ChangePivotNeigh {
                    type Env = Env;
                    type State = State;

                    fn preprocess(&mut self, env: &Self::Env, state: &mut Self::State) {
                        state.pivot = self.new_pivot;

                        let (mut vec_v, mut vec_h) =
                            State::get_box_vec(&env, &state.placements, state.pivot);
                        state.calculator_v.update(&env.estimator, &mut vec_v);
                        state.calculator_h.update(&env.estimator, &mut vec_h);

                        self.vec_v = Some(vec_v);
                        self.vec_h = Some(vec_h);
                    }

                    fn postprocess(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State) {
                        // do nothing
                    }

                    fn rollback(self: Box<Self>, env: &Self::Env, state: &mut Self::State) {
                        state.pivot = self.old_pivot;

                        state
                            .calculator_v
                            .update(&env.estimator, &mut self.vec_v.unwrap());
                        state
                            .calculator_h
                            .update(&env.estimator, &mut self.vec_h.unwrap());
                    }
                }
            }

            use super::{RectStdDev, Sampler};
            use crate::{
                problem::{Input, Op, Rect},
                solver::simd::{round_u16, SimdRectSet, AVX2_U16_W},
            };
            use itertools::Itertools as _;
            use nalgebra::{Cholesky, DMatrix, DVector, Matrix1};
            use rand::prelude::*;
            use rand_distr::StandardNormal;

            #[derive(Debug, Clone)]
            pub struct GaussEstimator {
                /// 平均ベクトル
                mean: DVector<f64>,

                /// 分散共分散行列
                variance: DMatrix<f64>,

                /// 計測誤差
                measure_variance: f64,

                /// 矩形の数
                rect_cnt: usize,
            }

            impl GaussEstimator {
                pub fn new(input: &Input) -> Self {
                    // 長方形ごとに縦横2次元
                    let rect_cnt = input.rect_cnt();
                    let len = rect_cnt * 2;

                    // モンテカルロにより求めた事前分布
                    const MEAN: f64 = 65000.0;
                    const STDDEV: f64 = 21280.0;

                    let mean = DVector::from_element(len, MEAN);
                    let variance = DMatrix::from_diagonal_element(len, len, STDDEV * STDDEV);

                    Self {
                        mean,
                        variance,
                        measure_variance: input.std_dev().powi(2),
                        rect_cnt,
                    }
                }

                pub fn update(&mut self, observation: Observation1d) {
                    let y = Matrix1::new(observation.len as f64);
                    let c = observation.edges;

                    let c_t = c.transpose();
                    let syx = &c_t * &self.variance;
                    let sxy = syx.transpose();
                    let syy = &c_t * &self.variance * &c + Matrix1::new(self.measure_variance);

                    let inv_syy = syy.try_inverse().unwrap();

                    let mean = &self.mean + &sxy * &inv_syy * (&y - &c_t * &self.mean);
                    let variance = &self.variance - &sxy * &inv_syy * &syx;

                    self.mean = mean;
                    self.variance = variance;
                }

                pub fn get_sampler(&self) -> GaussSampler {
                    GaussSampler::new(self)
                }

                pub fn dump_estimated(&self, actual_rects: Option<&[Rect]>) {
                    let mean_heights = self.mean_height();
                    let mean_widths = self.mean_width();
                    let variance_heights = self.variance_height();
                    let variance_widths = self.variance_width();

                    for i in 0..self.rect_cnt {
                        let std_dev_h = variance_heights[i].sqrt();
                        let std_dev_w = variance_widths[i].sqrt();
                        eprint!(
                            "{:>02} {:>6.0} ± {:>5.0} / {:>6.0} ± {:>5.0}",
                            i, mean_heights[i], std_dev_h, mean_widths[i], std_dev_w
                        );

                        if let Some(rects) = actual_rects {
                            let rect = rects[i];
                            let sigma_h = (rect.height() as f64 - mean_heights[i]) / std_dev_h;
                            let sigma_w = (rect.width() as f64 - mean_widths[i]) / std_dev_w;
                            eprintln!(
                                " (actual: {:>6.0} ({:+>5.2}σ) / {:>6.0} ({:+>5.2}σ))",
                                rect.height(),
                                sigma_h,
                                rect.width(),
                                sigma_w
                            );
                        } else {
                            eprintln!();
                        }
                    }
                }

                pub fn get_next_placements(
                    &self,
                    duration: f64,
                    rng: &mut impl Rng,
                ) -> (Vec<Op>, DVector<f64>, DVector<f64>) {
                    annealing::solve(self, duration, rng)
                }

                pub fn mean(&self) -> &[f64] {
                    self.mean.as_slice()
                }

                pub fn mean_height(&self) -> &[f64] {
                    &self.mean()[..self.rect_cnt]
                }

                pub fn mean_width(&self) -> &[f64] {
                    &self.mean()[self.rect_cnt..]
                }

                pub fn variance_diag(&self) -> Vec<f64> {
                    self.variance.diagonal().iter().copied().collect()
                }

                pub fn variance_height(&self) -> Vec<f64> {
                    self.variance_diag()[..self.rect_cnt].to_vec()
                }

                pub fn variance_width(&self) -> Vec<f64> {
                    self.variance_diag()[self.rect_cnt..].to_vec()
                }

                pub(crate) fn rect_std_dev(&self) -> RectStdDev {
                    let std_dev_h = self
                        .variance_height()
                        .into_iter()
                        .map(|v| v.sqrt())
                        .collect();
                    let std_dev_w = self
                        .variance_width()
                        .into_iter()
                        .map(|v| v.sqrt())
                        .collect();
                    RectStdDev::new(std_dev_w, std_dev_h)
                }
            }

            #[derive(Debug, Clone)]
            pub struct Observation1d {
                len: u32,
                edges: DVector<f64>,
            }

            impl Observation1d {
                pub fn new(len: u32, edges: DVector<f64>) -> Self {
                    Self { len, edges }
                }

                pub fn single(input: &Input, len: u32, rect_i: usize, is_width: bool) -> Self {
                    let mut edges = DVector::zeros(input.rect_cnt() * 2);
                    edges[rect_i + if is_width { input.rect_cnt() } else { 0 }] = 1.0;
                    Self::new(len, edges)
                }
            }

            pub struct GaussSampler<'a> {
                estimator: &'a GaussEstimator,
                cholesky: Cholesky<f64, nalgebra::Dyn>,
            }

            impl<'a> GaussSampler<'a> {
                fn new(estimator: &'a GaussEstimator) -> Self {
                    let cholesky = estimator.variance.clone().cholesky().unwrap();

                    Self {
                        estimator,
                        cholesky,
                    }
                }
            }

            impl Sampler for GaussSampler<'_> {
                fn sample(&mut self, rng: &mut impl Rng) -> SimdRectSet {
                    let mut heights = vec![[0; AVX2_U16_W]; self.estimator.rect_cnt];
                    let mut widths = vec![[0; AVX2_U16_W]; self.estimator.rect_cnt];

                    for simd_i in 0..AVX2_U16_W {
                        let dist = StandardNormal;
                        let values: Vec<f64> = (0..self.estimator.mean.len())
                            .map(|_| dist.sample(rng))
                            .collect_vec();
                        let values =
                            &self.cholesky.l() * DVector::from_vec(values) + &self.estimator.mean;

                        for rect_i in 0..self.estimator.rect_cnt {
                            let height = (values[rect_i].round() as u32)
                                .clamp(Input::MIN_RECT_SIZE, Input::MAX_RECT_SIZE);
                            let width = (values[rect_i + self.estimator.rect_cnt] as u32)
                                .clamp(Input::MIN_RECT_SIZE, Input::MAX_RECT_SIZE);
                            heights[rect_i][simd_i] = round_u16(height);
                            widths[rect_i][simd_i] = round_u16(width);
                        }
                    }

                    SimdRectSet::new(heights, widths)
                }
            }
        }
        pub mod mcmc {
            use super::{Observation2d, RectStdDev};
            use crate::{
                problem::{
                    Dir::{Left, Up},
                    Input, Judge, Op, Rect,
                },
                solver::simd::{
                    expand_u16, round_i16, round_u16, AlignedF32, AlignedU16, SimdRectSet,
                    AVX2_F32_W, AVX2_U16_W,
                },
            };
            use core::arch::x86_64::*;
            use itertools::{izip, Itertools};
            use rand::prelude::*;
            use rand_distr::Normal;

            pub(crate) struct MCMCSampler {
                env: Env,
                state: State,
            }

            impl MCMCSampler {
                pub(crate) fn new(
                    input: &Input,
                    observations: Vec<Observation2d>,
                    rects: SimdRectSet,
                    rect_std_dev: RectStdDev,
                    init_duration: f64,
                    rng: &mut impl Rng,
                    judge: &impl Judge,
                ) -> Self {
                    let env = Env::new(
                        input,
                        observations,
                        input.rect_cnt(),
                        input.std_dev(),
                        rect_std_dev,
                    );
                    let heights = rects.heights.iter().map(|&h| AlignedU16(h)).collect();
                    let widths = rects.widths.iter().map(|&w| AlignedU16(w)).collect();
                    let state = State::new(&env, widths, heights);
                    let mut states = mcmc(&env, state, init_duration, rng);
                    unsafe {
                        Self::dump_estimated(&states, judge.rects(), input.rect_cnt());
                    }

                    Self {
                        env,
                        state: states.pop().unwrap(),
                    }
                }

                pub(crate) fn update(&mut self, observation: Observation2d) {
                    self.env.observations.push(observation);
                    self.state.log_likelihood =
                        unsafe { self.state.calc_log_likelihood(&self.env) };
                }

                pub(crate) fn sample(&mut self, duration: f64, rng: &mut impl Rng) -> SimdRectSet {
                    self.state = mcmc(&self.env, self.state.clone(), duration, rng)
                        .pop()
                        .unwrap();

                    let mut heights = vec![];
                    let mut widths = vec![];

                    for (h, w) in izip!(&self.state.rect_h, &self.state.rect_w) {
                        heights.push(h.0);
                        widths.push(w.0);
                    }

                    SimdRectSet::new(heights, widths)
                }

                unsafe fn dump_estimated(
                    states: &[State],
                    actual_rects: Option<&[Rect]>,
                    rect_cnt: usize,
                ) {
                    let Some(rects) = actual_rects else { return };
                    let mut sum_w = vec![0; rect_cnt];
                    let mut sum_h = vec![0; rect_cnt];
                    let count = states.len();

                    for state in states {
                        for (i, (w, h)) in izip!(&state.rect_w, &state.rect_h).enumerate() {
                            for j in 0..AVX2_U16_W {
                                sum_w[i] += expand_u16(w.0[j]) as u64;
                                sum_h[i] += expand_u16(h.0[j]) as u64;
                            }
                        }
                    }

                    let mean_h = sum_h
                        .iter()
                        .map(|&s| s as f64 / (count * AVX2_U16_W as usize) as f64)
                        .collect_vec();
                    let mean_w = sum_w
                        .iter()
                        .map(|&s| s as f64 / (count * AVX2_U16_W as usize) as f64)
                        .collect_vec();

                    let mut sum_var_w = vec![0.0; rect_cnt];
                    let mut sum_var_h = vec![0.0; rect_cnt];

                    for state in states {
                        for (i, (w, h)) in izip!(&state.rect_w, &state.rect_h).enumerate() {
                            for j in 0..AVX2_U16_W {
                                sum_var_w[i] += (expand_u16(w.0[j]) as f64 - mean_w[i]).powi(2);
                                sum_var_h[i] += (expand_u16(h.0[j]) as f64 - mean_h[i]).powi(2);
                            }
                        }
                    }

                    let var_w = sum_var_w
                        .iter()
                        .map(|&s| s / (count * AVX2_U16_W as usize) as f64)
                        .collect_vec();
                    let var_h = sum_var_h
                        .iter()
                        .map(|&s| s / (count * AVX2_U16_W as usize) as f64)
                        .collect_vec();

                    let std_dev_w = var_w.iter().map(|&v| v.sqrt()).collect_vec();
                    let std_dev_h = var_h.iter().map(|&v| v.sqrt()).collect_vec();

                    eprintln!("[MCMC]");

                    for i in 0..rect_cnt {
                        eprint!(
                            "{:>02} {:>6.0} ± {:>5.0} / {:>6.0} ± {:>5.0}",
                            i, mean_h[i], std_dev_h[i], mean_w[i], std_dev_w[i]
                        );

                        let rect = rects[i];
                        let sigma_h = (rect.height() as f64 - mean_h[i] as f64) / std_dev_h[i];
                        let sigma_w = (rect.width() as f64 - mean_w[i] as f64) / std_dev_w[i];
                        eprintln!(
                            " (actual: {:>6.0} ({:+>5.2}σ) / {:>6.0} ({:+>5.2}σ))",
                            rect.height(),
                            sigma_h,
                            rect.width(),
                            sigma_w
                        );
                    }
                }
            }

            struct Env {
                observations: Vec<Observation2d>,
                init_measured_heights: Vec<AlignedF32>,
                init_measured_widths: Vec<AlignedF32>,
                rect_cnt: usize,
                std_dev: f64,
                rect_std_dev: RectStdDev,
            }

            impl Env {
                fn new(
                    input: &Input,
                    observations: Vec<Observation2d>,
                    rect_cnt: usize,
                    std_dev: f64,
                    rect_std_dev: RectStdDev,
                ) -> Self {
                    let init_measured_heights = input
                        .rect_measures()
                        .iter()
                        .map(|m| {
                            let h = round_u16(m.height()) as f32;
                            AlignedF32([h; AVX2_F32_W])
                        })
                        .collect_vec();
                    let init_measured_widths = input
                        .rect_measures()
                        .iter()
                        .map(|m| {
                            let w = round_u16(m.width()) as f32;
                            AlignedF32([w; AVX2_F32_W])
                        })
                        .collect_vec();

                    Self {
                        observations,
                        init_measured_heights,
                        init_measured_widths,
                        rect_cnt,
                        std_dev,
                        rect_std_dev,
                    }
                }
            }

            #[derive(Debug, Clone)]
            struct State {
                rect_w: Vec<AlignedU16>,
                rect_h: Vec<AlignedU16>,
                log_likelihood: [AlignedF32; 2],
            }

            impl State {
                fn new(env: &Env, rect_w: Vec<AlignedU16>, rect_h: Vec<AlignedU16>) -> Self {
                    let log_likelihood = [AlignedF32::default(); 2];

                    let mut s = Self {
                        rect_w,
                        rect_h,
                        log_likelihood,
                    };

                    unsafe {
                        s.log_likelihood = s.calc_log_likelihood(env);
                    }

                    s
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn calc_log_likelihood(&self, env: &Env) -> [AlignedF32; 2] {
                    let mut log_likelihood_low = _mm256_setzero_ps();
                    let mut log_likelihood_high = _mm256_setzero_ps();
                    let inv_std_dev =
                        _mm256_set1_ps(1.0 / round_u16(env.std_dev.round() as u32) as f32);

                    // 初期計測から
                    for (rect_h, rect_w, measured_h, measured_w) in izip!(
                        self.rect_h.iter(),
                        self.rect_w.iter(),
                        env.init_measured_heights.iter(),
                        env.init_measured_widths.iter()
                    ) {
                        // u16 x 16 -> (u32 x 8) x 2 -> (f32 x 8) x 2
                        let width = rect_w.load();
                        let height = rect_h.load();

                        let width_low = _mm256_castsi256_si128(width);
                        let width_high = _mm256_extracti128_si256(width, 1);
                        let width_u32_low = _mm256_cvtepu16_epi32(width_low);
                        let width_u32_high = _mm256_cvtepu16_epi32(width_high);
                        let width_f32_low = _mm256_cvtepi32_ps(width_u32_low);
                        let width_f32_high = _mm256_cvtepi32_ps(width_u32_high);

                        let height_low = _mm256_castsi256_si128(height);
                        let height_high = _mm256_extracti128_si256(height, 1);
                        let height_u32_low = _mm256_cvtepu16_epi32(height_low);
                        let height_u32_high = _mm256_cvtepu16_epi32(height_high);
                        let height_f32_low = _mm256_cvtepi32_ps(height_u32_low);
                        let height_f32_high = _mm256_cvtepi32_ps(height_u32_high);

                        let observed_x = measured_w.load();
                        let observed_y = measured_h.load();
                        let x_diff_low = _mm256_sub_ps(observed_x, width_f32_low);
                        let x_diff_high = _mm256_sub_ps(observed_x, width_f32_high);
                        let y_diff_low = _mm256_sub_ps(observed_y, height_f32_low);
                        let y_diff_high = _mm256_sub_ps(observed_y, height_f32_high);

                        let x_diff_low = _mm256_mul_ps(x_diff_low, inv_std_dev);
                        let x_diff_high = _mm256_mul_ps(x_diff_high, inv_std_dev);
                        let y_diff_low = _mm256_mul_ps(y_diff_low, inv_std_dev);
                        let y_diff_high = _mm256_mul_ps(y_diff_high, inv_std_dev);

                        // logP' = (x - μ)^2 + logP
                        log_likelihood_low =
                            _mm256_fmadd_ps(x_diff_low, x_diff_low, log_likelihood_low);
                        log_likelihood_high =
                            _mm256_fmadd_ps(x_diff_high, x_diff_high, log_likelihood_high);
                        log_likelihood_low =
                            _mm256_fmadd_ps(y_diff_low, y_diff_low, log_likelihood_low);
                        log_likelihood_high =
                            _mm256_fmadd_ps(y_diff_high, y_diff_high, log_likelihood_high);
                    }

                    // 配置結果から
                    let mut x0_buf = [AlignedU16::default(); Input::MAX_RECT_CNT];
                    let mut x1_buf = [AlignedU16::default(); Input::MAX_RECT_CNT];
                    let mut y0_buf = [AlignedU16::default(); Input::MAX_RECT_CNT];
                    let mut y1_buf = [AlignedU16::default(); Input::MAX_RECT_CNT];
                    let mut x0_buf = &mut x0_buf[..env.rect_cnt];
                    let mut x1_buf = &mut x1_buf[..env.rect_cnt];
                    let mut y0_buf = &mut y0_buf[..env.rect_cnt];
                    let mut y1_buf = &mut y1_buf[..env.rect_cnt];

                    for observation in env.observations.iter() {
                        let (width, height) = if observation.is_2d {
                            self.pack_2d(
                                &observation.operations,
                                &self.rect_w,
                                &self.rect_h,
                                &mut x0_buf,
                                &mut x1_buf,
                                &mut y0_buf,
                                &mut y1_buf,
                            )
                        } else {
                            let base = observation
                                .operations
                                .iter()
                                .flat_map(|op| op.base())
                                .next();

                            self.pack_1d(
                                &observation.operations,
                                &self.rect_w,
                                &self.rect_h,
                                &mut x0_buf,
                                &mut x1_buf,
                                &mut y0_buf,
                                &mut y1_buf,
                                base,
                            )
                        };

                        // u16 x 16 -> (u32 x 8) x 2 -> (f32 x 8) x 2
                        let width = width.load();
                        let height = height.load();

                        let width_low = _mm256_castsi256_si128(width);
                        let width_high = _mm256_extracti128_si256(width, 1);
                        let width_u32_low = _mm256_cvtepu16_epi32(width_low);
                        let width_u32_high = _mm256_cvtepu16_epi32(width_high);
                        let width_f32_low = _mm256_cvtepi32_ps(width_u32_low);
                        let width_f32_high = _mm256_cvtepi32_ps(width_u32_high);

                        let height_low = _mm256_castsi256_si128(height);
                        let height_high = _mm256_extracti128_si256(height, 1);
                        let height_u32_low = _mm256_cvtepu16_epi32(height_low);
                        let height_u32_high = _mm256_cvtepu16_epi32(height_high);
                        let height_f32_low = _mm256_cvtepi32_ps(height_u32_low);
                        let height_f32_high = _mm256_cvtepi32_ps(height_u32_high);

                        let observed_x = _mm256_set1_ps(round_u16(observation.len_x) as f32);
                        let observed_y = _mm256_set1_ps(round_u16(observation.len_y) as f32);

                        let x_diff_low = _mm256_sub_ps(observed_x, width_f32_low);
                        let x_diff_high = _mm256_sub_ps(observed_x, width_f32_high);
                        let y_diff_low = _mm256_sub_ps(observed_y, height_f32_low);
                        let y_diff_high = _mm256_sub_ps(observed_y, height_f32_high);

                        let x_diff_low = _mm256_mul_ps(x_diff_low, inv_std_dev);
                        let x_diff_high = _mm256_mul_ps(x_diff_high, inv_std_dev);
                        let y_diff_low = _mm256_mul_ps(y_diff_low, inv_std_dev);
                        let y_diff_high = _mm256_mul_ps(y_diff_high, inv_std_dev);

                        // logP' = (x - μ)^2 + logP
                        log_likelihood_low =
                            _mm256_fmadd_ps(x_diff_low, x_diff_low, log_likelihood_low);
                        log_likelihood_high =
                            _mm256_fmadd_ps(x_diff_high, x_diff_high, log_likelihood_high);
                        log_likelihood_low =
                            _mm256_fmadd_ps(y_diff_low, y_diff_low, log_likelihood_low);
                        log_likelihood_high =
                            _mm256_fmadd_ps(y_diff_high, y_diff_high, log_likelihood_high);
                    }

                    // 対数尤度の-0.5は最後にかければよい
                    let neg_inv_2 = _mm256_set1_ps(-0.5);
                    let log_likelihood_low = _mm256_mul_ps(log_likelihood_low, neg_inv_2);
                    let log_likelihood_high = _mm256_mul_ps(log_likelihood_high, neg_inv_2);

                    [log_likelihood_low.into(), log_likelihood_high.into()]
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn pack_1d(
                    &self,
                    ops: &[Op],
                    rect_w: &[AlignedU16],
                    rect_h: &[AlignedU16],
                    x0: &mut [AlignedU16],
                    x1: &mut [AlignedU16],
                    y0: &mut [AlignedU16],
                    y1: &mut [AlignedU16],
                    base: Option<usize>,
                ) -> (AlignedU16, AlignedU16) {
                    const MAX_RECT_SIZE: u16 = round_u16(100000);
                    let max_rect_size = _mm256_set1_epi16(MAX_RECT_SIZE as i16);
                    let mut width = _mm256_setzero_si256();
                    let mut height = _mm256_setzero_si256();

                    x0.fill(AlignedU16::default());
                    x1.fill(AlignedU16::default());
                    y0.fill(AlignedU16::default());
                    y1.fill(AlignedU16::default());

                    // 座標がこれ以上になったら衝突判定をしなくても大丈夫という閾値
                    // y座標はbase次第なので、baseが壁でない場合は0を入れる
                    let threshold_x = max_rect_size;
                    let mut threshold_y = match base {
                        Some(_) => _mm256_setzero_si256(),
                        None => max_rect_size,
                    };

                    for &op in ops.iter() {
                        // 全長方形がmax_rect_size以上ならば安全なので、頑張って判定
                        // x, y方向それぞれ閾値未満か（未満なら判定が必要）
                        let x_ng = _mm256_cmpgt_epi16(threshold_x, width);
                        let y_ng = _mm256_cmpgt_epi16(threshold_y, height);

                        // DirectionがLeftなら全て0、Upなら全て1のマスク
                        let y_mask = _mm256_set1_epi8(op.dir() as i8 - 1);

                        // 判定したい方をを取り出して判定
                        // 16並列全て条件を満たすのであればflagは0になる（不等号の向きに注意）
                        let ng = _mm256_blendv_epi8(x_ng, y_ng, y_mask);
                        let flag = _mm256_movemask_epi8(ng);
                        let rect_i = op.rect_idx();

                        if flag == 0 {
                            // 座標がmax_rect_size以上なので衝突判定不要
                            // 高さ、幅どちらを取るかビット演算で求める
                            // rotate_maskはrotateがtrueなら全て0、falseなら全て1
                            let not_rotate_mask = _mm256_set1_epi8((op.rotate() as i8) - 1);
                            let height_mask = _mm256_xor_si256(y_mask, not_rotate_mask);
                            let index = op.rect_idx();
                            let len = _mm256_blendv_epi8(
                                rect_h[index].load(),
                                rect_w[index].load(),
                                height_mask,
                            );

                            // 高さ・幅を更新
                            // 0を足すかlenを足すかをマスク演算で分岐
                            let w_add = _mm256_andnot_si256(y_mask, len);
                            let h_add = _mm256_and_si256(y_mask, len);
                            width = _mm256_add_epi16(width, w_add);
                            height = _mm256_add_epi16(height, h_add);
                        } else {
                            // 真面目に衝突判定をする
                            // こちらが呼ばれる回数は少ないためちょっと処理をサボっても大丈夫
                            let rotate = op.rotate();

                            match op.dir() {
                                Left => {
                                    let rect_h = rect_h[rect_i].load();
                                    let rect_w = rect_w[rect_i].load();
                                    let heights = &mut height;
                                    let widths = &mut width;

                                    self.pack_one_2d(
                                        op.base(),
                                        rotate,
                                        rect_i,
                                        rect_h,
                                        rect_w,
                                        x0,
                                        x1,
                                        y0,
                                        y1,
                                        heights,
                                        widths,
                                    );
                                }
                                Up => {
                                    // x, y座標を反転させる
                                    let (rect_h, rect_w) =
                                        (rect_w[rect_i].load(), rect_h[rect_i].load());
                                    let (heights, widths) = (&mut width, &mut height);

                                    self.pack_one_2d(
                                        op.base(),
                                        rotate,
                                        rect_i,
                                        rect_h,
                                        rect_w,
                                        y0,
                                        y1,
                                        x0,
                                        x1,
                                        heights,
                                        widths,
                                    );
                                }
                            };
                        }

                        // base == rect_index である場合、INFとしていたthreshold_yを更新
                        // 併せて衝突判定用にx0, x1, y1も更新
                        if Some(op.rect_idx()) == base {
                            x0[rect_i] = _mm256_setzero_si256().into();
                            x1[rect_i] = rect_w[rect_i];
                            y0[rect_i] = _mm256_sub_epi16(height, rect_h[rect_i].load()).into();
                            y1[rect_i] = height.into();
                            threshold_y = _mm256_add_epi16(height, max_rect_size);
                        }
                    }

                    (width.into(), height.into())
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn pack_2d(
                    &self,
                    ops: &[Op],
                    rect_w: &[AlignedU16],
                    rect_h: &[AlignedU16],
                    x0: &mut [AlignedU16],
                    x1: &mut [AlignedU16],
                    y0: &mut [AlignedU16],
                    y1: &mut [AlignedU16],
                ) -> (AlignedU16, AlignedU16) {
                    // pack_1dとほぼ同じ
                    // 毎回愚直に衝突判定をするためシンプル
                    // 2次元的に詰め込む回数は少ないので計算量的になんとかなるはず
                    let mut width = _mm256_setzero_si256();
                    let mut height = _mm256_setzero_si256();

                    x0.fill(AlignedU16::default());
                    x1.fill(AlignedU16::default());
                    y0.fill(AlignedU16::default());
                    y1.fill(AlignedU16::default());

                    for &op in ops.iter() {
                        // 真面目に衝突判定をする
                        let rotate = op.rotate();
                        let rect_i = op.rect_idx();

                        match op.dir() {
                            Left => {
                                let rect_h = rect_h[rect_i].load();
                                let rect_w = rect_w[rect_i].load();
                                let heights = &mut height;
                                let widths = &mut width;

                                self.pack_one_2d(
                                    op.base(),
                                    rotate,
                                    rect_i,
                                    rect_h,
                                    rect_w,
                                    x0,
                                    x1,
                                    y0,
                                    y1,
                                    heights,
                                    widths,
                                );
                            }
                            Up => {
                                // x, y座標を反転させる
                                let (rect_h, rect_w) =
                                    (rect_w[rect_i].load(), rect_h[rect_i].load());
                                let (heights, widths) = (&mut width, &mut height);

                                self.pack_one_2d(
                                    op.base(),
                                    rotate,
                                    rect_i,
                                    rect_h,
                                    rect_w,
                                    y0,
                                    y1,
                                    x0,
                                    x1,
                                    heights,
                                    widths,
                                );
                            }
                        };
                    }

                    (width.into(), height.into())
                }

                #[target_feature(enable = "avx,avx2")]
                unsafe fn pack_one_2d(
                    &self,
                    base: Option<usize>,
                    rotate: bool,
                    rect_i: usize,
                    mut rect_h: __m256i,
                    mut rect_w: __m256i,
                    placements_x0: &mut [AlignedU16],
                    placements_x1: &mut [AlignedU16],
                    placements_y0: &mut [AlignedU16],
                    placements_y1: &mut [AlignedU16],
                    heights: &mut __m256i,
                    widths: &mut __m256i,
                ) {
                    if rotate {
                        std::mem::swap(&mut rect_w, &mut rect_h);
                    }

                    let y0 = match base {
                        Some(index) => placements_y1[index].load(),
                        None => _mm256_setzero_si256(),
                    };
                    let y1 = _mm256_add_epi16(y0, rect_h);

                    // 長方形がどこに置かれるかを調べる
                    let mut x0 = _mm256_setzero_si256();

                    for (p_x1, p_y0, p_y1) in izip!(
                        placements_x1[..rect_i].iter(),
                        placements_y0[..rect_i].iter(),
                        placements_y1[..rect_i].iter()
                    ) {
                        // やっていることはジャッジコードと同じ
                        //
                        // let x = if y0.max(p_y0) < y1.min(p_y1) {
                        //     p_x1
                        // } else {
                        //     0
                        // };
                        //
                        // x0 = x0.max(x);
                        let max_y0 = _mm256_max_epu16(y0, p_y0.load());
                        let min_y1 = _mm256_min_epu16(y1, p_y1.load());

                        let gt = _mm256_cmpgt_epi16(min_y1, max_y0);
                        let x = _mm256_and_si256(p_x1.load(), gt);
                        x0 = _mm256_max_epu16(x0, x);
                    }

                    let x1 = _mm256_add_epi16(x0, rect_w);

                    placements_x0[rect_i] = x0.into();
                    placements_x1[rect_i] = x1.into();
                    placements_y0[rect_i] = y0.into();
                    placements_y1[rect_i] = y1.into();

                    *widths = _mm256_max_epu16(*widths, x1);
                    *heights = _mm256_max_epu16(*heights, y1);
                }
            }

            struct Neighbor {
                delta: [i16; AVX2_U16_W],
                rect_i: usize,
                is_width: bool,
            }

            impl Neighbor {
                fn new(delta: [i16; AVX2_U16_W], rect_i: usize, is_width: bool) -> Self {
                    Self {
                        delta,
                        rect_i,
                        is_width,
                    }
                }

                fn gen(env: &Env, state: &State, rng: &mut impl Rng) -> Self {
                    let rect_i = rng.gen_range(0..env.rect_cnt);
                    let is_width = rng.gen_bool(0.5);
                    let std_dev = if is_width {
                        env.rect_std_dev.widths[rect_i]
                    } else {
                        env.rect_std_dev.heights[rect_i]
                    };
                    let current = if is_width {
                        state.rect_w[rect_i]
                    } else {
                        state.rect_h[rect_i]
                    };

                    let mut delta = [0; AVX2_U16_W];
                    const LOWER_BOUND: u16 = round_u16(Input::MIN_RECT_SIZE);
                    const UPPER_BOUND: u16 = round_u16(Input::MAX_RECT_SIZE);

                    for i in 0..AVX2_U16_W {
                        let dist = Normal::new(0.0, std_dev).unwrap();

                        delta[i] = loop {
                            let d = round_i16(dist.sample(rng).round() as i32);
                            let new_x = current.0[i].wrapping_add_signed(d);

                            if d != 0 && LOWER_BOUND <= new_x && new_x <= UPPER_BOUND {
                                break d;
                            }
                        }
                    }

                    Self::new(delta, rect_i, is_width)
                }
            }

            fn mcmc(env: &Env, mut state: State, duration: f64, rng: &mut impl Rng) -> Vec<State> {
                let since = std::time::Instant::now();
                let mut all_iter = 0;
                let mut accepted = 0;
                let mut rejected = 0;
                let mut all_states = vec![];
                const SAMPLE_INTERVAL: usize = 20;

                loop {
                    if since.elapsed().as_secs_f64() >= duration {
                        break;
                    }

                    // 変形
                    let neighbor = Neighbor::gen(env, &state, rng);

                    for i in 0..AVX2_U16_W {
                        if neighbor.is_width {
                            state.rect_w[neighbor.rect_i].0[i] = state.rect_w[neighbor.rect_i].0[i]
                                .wrapping_add_signed(neighbor.delta[i]);
                        } else {
                            state.rect_h[neighbor.rect_i].0[i] = state.rect_h[neighbor.rect_i].0[i]
                                .wrapping_add_signed(neighbor.delta[i]);
                        }
                    }

                    // 対数尤度計算
                    let new_log_likelihood = unsafe { state.calc_log_likelihood(env) };

                    for i in 0..AVX2_U16_W {
                        let (j, k) = (i / AVX2_F32_W, i % AVX2_F32_W);
                        let prev_log_likelihood = state.log_likelihood[j].0[k];

                        if rng.gen_range(0.0..1.0)
                            < (-(prev_log_likelihood - new_log_likelihood[j].0[k])).exp()
                        {
                            accepted += 1;
                            state.log_likelihood[j].0[k] = new_log_likelihood[j].0[k];
                        } else {
                            rejected += 1;
                            if neighbor.is_width {
                                state.rect_w[neighbor.rect_i].0[i] = state.rect_w[neighbor.rect_i]
                                    .0[i]
                                    .wrapping_add_signed(-neighbor.delta[i]);
                            } else {
                                state.rect_h[neighbor.rect_i].0[i] = state.rect_h[neighbor.rect_i]
                                    .0[i]
                                    .wrapping_add_signed(-neighbor.delta[i]);
                            }
                        }
                    }

                    all_iter += 1;

                    if all_iter % SAMPLE_INTERVAL == 0 {
                        all_states.push(state.clone());
                    }
                }

                eprintln!("mcmc_iter: {}", all_iter);
                eprintln!("mcmc accepted: {} / {}", accepted, accepted + rejected);

                all_states.push(state);
                all_states
            }
        }

        use super::simd::SimdRectSet;
        use crate::problem::Op;
        use gauss::GaussEstimator;
        use nalgebra::DVector;
        use rand::prelude::*;

        pub(super) fn get_placements(
            estimator: &GaussEstimator,
            duration: f64,
            rng: &mut impl Rng,
        ) -> (Vec<Op>, DVector<f64>, DVector<f64>) {
            estimator.get_next_placements(duration, rng)
        }

        #[derive(Debug, Clone, Copy)]
        struct Placement {
            x0: u32,
            x1: u32,
            y0: u32,
            y1: u32,
        }

        impl Placement {
            fn new(x0: u32, x1: u32, y0: u32, y1: u32) -> Self {
                assert!(x0 < x1);
                assert!(y0 < y1);
                Self { x0, x1, y0, y1 }
            }
        }

        pub(super) trait Sampler {
            fn sample(&mut self, rng: &mut impl Rng) -> SimdRectSet;
        }

        #[derive(Debug, Clone)]
        pub(crate) struct Observation2d {
            operations: Vec<Op>,
            len_x: u32,
            len_y: u32,
            is_2d: bool,
        }

        impl Observation2d {
            pub(super) fn new(
                operations: Vec<Op>,
                len_x: u32,
                len_y: u32,
                is_square: bool,
            ) -> Self {
                Self {
                    operations,
                    len_x,
                    len_y,
                    is_2d: is_square,
                }
            }
        }

        #[derive(Debug, Clone)]
        pub(crate) struct RectStdDev {
            widths: Vec<f64>,
            heights: Vec<f64>,
        }

        impl RectStdDev {
            fn new(widths: Vec<f64>, heights: Vec<f64>) -> Self {
                Self { widths, heights }
            }
        }
    }
    #[allow(dead_code)]
    mod simd {
        use std::arch::x86_64::*;

        pub(super) const AVX2_U16_W: usize = 16;
        pub(super) const AVX2_F32_W: usize = 8;

        pub(super) const fn round_u16(x: u32) -> u16 {
            // 座標の最大値は2^23 = 8_388_608とする（さすがに大丈夫やろ……）
            // これを16bitに収めるためには、7bit右シフトすればよい（128単位で丸められる）
            // 事前に1 << 6を足しておくと四捨五入になる
            ((x + (1 << 6)) >> 7) as u16
        }

        pub(super) const fn round_i16(x: i32) -> i16 {
            ((x + (1 << 6)) >> 7) as i16
        }

        pub(super) const fn expand_u16(x: u16) -> u32 {
            (x as u32) << 7
        }

        #[target_feature(enable = "avx,avx2")]
        pub(super) unsafe fn horizontal_add_u16(x: __m256i) -> u32 {
            unsafe {
                let low = _mm256_castsi256_si128(x);
                let high = _mm256_extracti128_si256(x, 1);

                let low32 = _mm256_cvtepu16_epi32(low);
                let high32 = _mm256_cvtepu16_epi32(high);
                let x = _mm256_add_epi32(low32, high32);

                let low = _mm256_castsi256_si128(x);
                let high = _mm256_extracti128_si256(x, 1);
                let x = _mm_add_epi32(low, high);

                let x = _mm_hadd_epi32(x, x);
                let x = _mm_hadd_epi32(x, x);
                let x = _mm_extract_epi32(x, 0);

                x as u32
            }
        }

        #[target_feature(enable = "avx,avx2")]
        pub(super) unsafe fn horizontal_and_u16(x: __m256i) -> u16 {
            unsafe {
                let low = _mm256_castsi256_si128(x);
                let high = _mm256_extracti128_si256(x, 1);
                let x = _mm_and_si128(low, high);

                let x = _mm_and_si128(x, _mm_srli_si128::<2>(x));
                let x = _mm_and_si128(x, _mm_srli_si128::<4>(x));
                let x = _mm_and_si128(x, _mm_srli_si128::<8>(x));
                let x = _mm_extract_epi16(x, 0);
                x as u16
            }
        }

        #[target_feature(enable = "avx,avx2")]
        pub(super) unsafe fn horizontal_or_u16(x: __m256i) -> u16 {
            unsafe {
                let low = _mm256_castsi256_si128(x);
                let high = _mm256_extracti128_si256(x, 1);
                let x = _mm_or_si128(low, high);

                let x = _mm_or_si128(x, _mm_srli_si128::<2>(x));
                let x = _mm_or_si128(x, _mm_srli_si128::<4>(x));
                let x = _mm_or_si128(x, _mm_srli_si128::<8>(x));
                let x = _mm_extract_epi16(x, 0);
                x as u16
            }
        }

        #[target_feature(enable = "avx,avx2")]
        pub(super) unsafe fn horizontal_xor_u16(x: __m256i) -> u16 {
            unsafe {
                let low = _mm256_castsi256_si128(x);
                let high = _mm256_extracti128_si256(x, 1);
                let x = _mm_xor_si128(low, high);
                let x = _mm_xor_si128(x, _mm_srli_si128::<2>(x));
                let x = _mm_xor_si128(x, _mm_srli_si128::<4>(x));
                let x = _mm_xor_si128(x, _mm_srli_si128::<8>(x));
                let x = _mm_extract_epi32(x, 0);
                x as u16
            }
        }

        #[target_feature(enable = "avx,avx2")]
        pub(super) unsafe fn horizontal_add_f32(x: __m256) -> f32 {
            unsafe {
                let low = _mm256_castps256_ps128(x);
                let high = _mm256_extractf128_ps(x, 1);
                let x = _mm_add_ps(low, high);

                let x = _mm_hadd_ps(x, x);
                let x = _mm_hadd_ps(x, x);
                let x = _mm_extract_ps(x, 0);

                // f32として再解釈
                f32::from_bits(x as u32)
            }
        }

        #[target_feature(enable = "avx,avx2")]
        pub(super) unsafe fn bitonic_sort_u16(x: __m256i) -> __m256i {
            unsafe {
                // 上位128bitが全て1、下位128bitが全て0のマスク
                let _mm_setzero_si128 = _mm_setzero_si128();
                let zero = _mm_setzero_si128;
                let one = _mm_cmpeq_epi16(zero, zero);
                let zero_one_mask = _mm256_set_m128i(one, zero);

                // block 1
                // block 1-1
                let x = block1(x);

                // block 2
                let x = block2(x);

                // block 3
                let x = block3(x, zero_one_mask);

                // block 4
                block4(x)
            }
        }

        #[target_feature(enable = "avx,avx2")]
        unsafe fn block1(x: __m256i) -> __m256i {
            unsafe {
                let shuffled = shuffle256_1_u16(x);
                let min = _mm256_min_epu16(x, shuffled);
                let max = _mm256_max_epu16(x, shuffled);
                let x = _mm256_blend_epi16::<0b01100110>(min, max);
                x
            }
        }

        #[target_feature(enable = "avx,avx2")]
        unsafe fn block2(x: __m256i) -> __m256i {
            unsafe {
                // block 2-1
                let shuffled = shuffle256_2_u16(x);
                let (min, max) = min_max_u16(x, shuffled);
                let x = _mm256_blend_epi16::<0b00111100>(min, max);

                // block 2-2
                let shuffled = shuffle256_1_u16(x);
                let (min, max) = min_max_u16(x, shuffled);
                let x = _mm256_blend_epi16::<0b01011010>(min, max);
                x
            }
        }

        #[target_feature(enable = "avx,avx2")]
        unsafe fn block3(x: __m256i, zero_one_mask: __m256i) -> __m256i {
            unsafe {
                // block 3-1
                // 上位だけxorをすることで、大小関係を逆転させる
                let x = _mm256_xor_si256(x, zero_one_mask);
                let shuffled = shuffle256_4_u16(x);
                let (min, max) = min_max_u16(x, shuffled);
                let x = _mm256_blend_epi16::<0b11110000>(min, max);

                // block 3-2
                let shuffled = shuffle256_2_u16(x);
                let (min, max) = min_max_u16(x, shuffled);
                let x = _mm256_blend_epi16::<0b11001100>(min, max);

                // block 3-3
                let shuffled = shuffle256_1_u16(x);
                let (min, max) = min_max_u16(x, shuffled);
                let x = _mm256_blend_epi16::<0b10101010>(min, max);

                // 反転させたので元に戻す
                let x = _mm256_xor_si256(x, zero_one_mask);
                x
            }
        }

        #[target_feature(enable = "avx,avx2")]
        unsafe fn block4(x: __m256i) -> __m256i {
            unsafe {
                // block 4-1
                // 比較演算 + ビットマスクが速そうだが、
                // 符号なし16bit整数の比較命令がないので2つのxmmレジスタに分割して処理
                let low = _mm256_castsi256_si128(x);
                let high = _mm256_extracti128_si256(x, 1);
                let min = _mm_min_epu16(low, high);
                let max = _mm_max_epu16(low, high);
                let x = _mm256_set_m128i(max, min);

                // block 4-2
                let shuffled = shuffle256_4_u16(x);
                let (min, max) = min_max_u16(x, shuffled);
                let x = _mm256_blend_epi16::<0b11110000>(min, max);

                // block 4-3
                let shuffled = shuffle256_2_u16(x);
                let (min, max) = min_max_u16(x, shuffled);
                let x = _mm256_blend_epi16::<0b11001100>(min, max);

                // block 4-4
                let shuffled = shuffle256_1_u16(x);
                let (min, max) = min_max_u16(x, shuffled);
                let x = _mm256_blend_epi16::<0b10101010>(min, max);
                x
            }
        }

        #[target_feature(enable = "avx,avx2")]
        unsafe fn min_max_u16(x: __m256i, y: __m256i) -> (__m256i, __m256i) {
            unsafe {
                let min = _mm256_min_epu16(x, y);
                let max = _mm256_max_epu16(x, y);
                (min, max)
            }
        }

        #[target_feature(enable = "avx,avx2")]
        unsafe fn shuffle256_1_u16(x: __m256i) -> __m256i {
            unsafe {
                // 16bitについてはhi/lo分けないとシャッフルできない
                let shuffled = _mm256_shufflehi_epi16::<0b10110001>(x);
                let shuffled = _mm256_shufflelo_epi16::<0b10110001>(shuffled);
                shuffled
            }
        }

        #[target_feature(enable = "avx,avx2")]
        unsafe fn shuffle256_2_u16(x: __m256i) -> __m256i {
            unsafe {
                // 16bit整数を2つまとめて32bitと見なせば良い
                _mm256_shuffle_epi32::<0b10110001>(x)
            }
        }

        #[target_feature(enable = "avx,avx2")]
        unsafe fn shuffle256_4_u16(x: __m256i) -> __m256i {
            unsafe {
                // 16bit整数を2つまとめて32bitと見なし、2つ飛びで交換すれば良い
                _mm256_shuffle_epi32::<0b01001110>(x)
            }
        }

        /// 32byte境界にアライン
        #[repr(align(32))]
        #[derive(Debug, Clone, Copy, Default, PartialEq, Eq, PartialOrd, Ord, Hash)]
        pub(super) struct AlignedU16(pub [u16; AVX2_U16_W]);

        impl AlignedU16 {
            pub const ZERO: Self = Self([0; AVX2_U16_W]);

            pub fn load(&self) -> __m256i {
                unsafe { _mm256_load_si256(self.0.as_ptr() as *const __m256i) }
            }

            pub fn store(&mut self, x: __m256i) {
                unsafe { _mm256_store_si256(self.0.as_mut_ptr() as *mut __m256i, x) }
            }
        }

        impl From<__m256i> for AlignedU16 {
            fn from(x: __m256i) -> Self {
                let mut ret = Self([0; AVX2_U16_W]);
                ret.store(x);
                ret
            }
        }

        /// 32byte境界にアライン
        #[repr(align(32))]
        #[derive(Debug, Clone, Copy, Default, PartialEq, PartialOrd)]
        pub(super) struct AlignedF32(pub [f32; AVX2_F32_W]);

        impl AlignedF32 {
            pub const ZERO: Self = Self([0.0; AVX2_F32_W]);

            pub fn load(&self) -> __m256 {
                unsafe { _mm256_load_ps(self.0.as_ptr() as *const f32) }
            }

            pub fn store(&mut self, x: __m256) {
                unsafe { _mm256_store_ps(self.0.as_mut_ptr() as *mut f32, x) }
            }
        }

        impl From<__m256> for AlignedF32 {
            fn from(x: __m256) -> Self {
                let mut ret = Self([0.0; AVX2_F32_W]);
                ret.store(x);
                ret
            }
        }

        impl Into<__m256> for AlignedF32 {
            fn into(self) -> __m256 {
                self.load()
            }
        }

        #[derive(Debug, Clone)]
        pub struct SimdRectSet {
            /// 長方形の幅を16bit x 16個packしたもの
            pub(super) heights: Vec<[u16; AVX2_U16_W]>,

            /// 長方形の高さを16bit x 16個packしたもの
            pub(super) widths: Vec<[u16; AVX2_U16_W]>,
        }

        impl SimdRectSet {
            pub(super) fn new(
                heights: Vec<[u16; AVX2_U16_W]>,
                widths: Vec<[u16; AVX2_U16_W]>,
            ) -> Self {
                Self { heights, widths }
            }
        }

        #[cfg(test)]
        mod test {
            use rand::{thread_rng, Rng};

            use super::*;

            #[test]
            fn horizontal_xor_16_test() {
                let mut rng = thread_rng();

                for _ in 0..100 {
                    let data: [u16; 16] = core::array::from_fn(|_| rng.gen());
                    let x = unsafe { _mm256_loadu_si256(data.as_ptr() as *const __m256i) };
                    let xor = unsafe { horizontal_xor_u16(x) };

                    let mut expected = 0u16;

                    for &x in data.iter() {
                        expected ^= x;
                    }

                    assert_eq!(xor, expected);
                }
            }

            #[test]
            fn horizontal_add_16_test() {
                let mut rng = thread_rng();

                for _ in 0..100 {
                    let data: [u16; 16] = core::array::from_fn(|_| rng.gen());
                    let x = unsafe { _mm256_loadu_si256(data.as_ptr() as *const __m256i) };
                    let sum = unsafe { horizontal_add_u16(x) };

                    let mut expected = 0;

                    for &x in data.iter() {
                        expected += x as u32;
                    }

                    assert_eq!(sum, expected);
                }
            }

            #[test]
            fn horizontal_and_16_test() {
                let mut rng = thread_rng();

                for _ in 0..100 {
                    let mask: u16 = rng.gen();
                    let data: [u16; 16] = core::array::from_fn(|_| rng.gen::<u16>() | mask);
                    let x = unsafe { _mm256_loadu_si256(data.as_ptr() as *const __m256i) };
                    let and = unsafe { horizontal_and_u16(x) };

                    let mut expected = 0xffff;

                    for &x in data.iter() {
                        expected &= x;
                    }

                    assert_eq!(and, expected);
                }
            }

            #[test]
            fn horizontal_or_16_test() {
                let mut rng = thread_rng();

                for _ in 0..100 {
                    let mask = rng.gen::<u16>();
                    let data: [u16; 16] = core::array::from_fn(|_| rng.gen::<u16>() & mask);
                    let x = unsafe { _mm256_loadu_si256(data.as_ptr() as *const __m256i) };
                    let or = unsafe { horizontal_or_u16(x) };

                    let mut expected = 0x0000;

                    for &x in data.iter() {
                        expected |= x;
                    }

                    assert_eq!(or, expected);
                }
            }

            #[test]
            fn horizontal_add_f32_test() {
                let mut rng = thread_rng();

                for _ in 0..100 {
                    let data: [f32; 8] = core::array::from_fn(|_| rng.gen_range(0.0..1.0f32));
                    let x = unsafe { _mm256_loadu_ps(data.as_ptr() as *const f32) };
                    let sum = unsafe { horizontal_add_f32(x) };

                    let mut expected = 0.0;

                    for &x in data.iter() {
                        expected += x;
                    }

                    let min = expected.min(sum);
                    let max = expected.max(sum);
                    let relative_error = (max - min) / max;

                    assert!(relative_error < 1e-4);
                }
            }

            #[test]
            fn bitonic_sort_u16_test() {
                let mut rng = thread_rng();

                for _ in 0..100 {
                    let data: [u16; 16] = core::array::from_fn(|_| rng.gen());
                    let x = unsafe { _mm256_loadu_si256(data.as_ptr() as *const __m256i) };
                    let sorted = unsafe { bitonic_sort_u16(x) };

                    let mut expected: [u16; 16] = [0; 16];
                    expected.copy_from_slice(&data);
                    expected.sort_unstable();

                    let mut actual: [u16; 16] = [0; 16];
                    unsafe { _mm256_storeu_si256(actual.as_mut_ptr() as *mut __m256i, sorted) };

                    assert_eq!(actual, expected);
                }
            }
        }
    }
    pub mod solver01 {
        use super::{
            estimator::gauss::{GaussEstimator, Observation1d},
            Solver,
        };
        use crate::{
            problem::{params::Params, Input, Judge},
            solver::{
                arranger::{mcts::MCTSArranger, multi_beam_simd::MultiBeamArrangerSimd},
                estimator::{self, mcmc, Observation2d, Sampler as _},
            },
        };
        use rand::SeedableRng;
        use rand_pcg::Pcg64Mcg;

        pub struct Solver01;

        impl Solver for Solver01 {
            fn solve(&self, input: &Input, mut judge: impl Judge) {
                let mut rng = Pcg64Mcg::from_entropy();
                let mut estimator = GaussEstimator::new(input);

                for (i, &rect) in input.rect_measures().iter().enumerate() {
                    estimator.update(Observation1d::single(input, rect.height(), i, false));
                    estimator.update(Observation1d::single(input, rect.width(), i, true));
                }

                eprintln!("[Init]");
                estimator.dump_estimated(judge.rects());

                let arrange_trial_count = Params::get().borrow().arrange_count;
                let duration = Params::get().borrow().query_annealing_duration_sec
                    / (input.query_cnt() - arrange_trial_count) as f64;
                let mut observations = vec![];

                for _ in 0..input.query_cnt() - arrange_trial_count {
                    let (ops, edges_v, edges_h) =
                        estimator::get_placements(&estimator, duration, &mut rng);
                    let measure = judge.query(&ops);
                    let observation_x = Observation1d::new(measure.width(), edges_h);
                    let observation_y = Observation1d::new(measure.height(), edges_v);

                    estimator.update(observation_x);
                    estimator.update(observation_y);
                    observations.push(Observation2d::new(
                        ops,
                        measure.width(),
                        measure.height(),
                        false,
                    ));
                }

                eprintln!("[Final]");
                estimator.dump_estimated(judge.rects());

                let mut gauss_sampler = estimator.get_sampler();

                let mut beam_arranger = MultiBeamArrangerSimd;
                let mut mcts_arranger = MCTSArranger;

                let gauss_rects = gauss_sampler.sample(&mut rng);
                let rect_std_dev = estimator.rect_std_dev();

                let mut mcmc_sampler = mcmc::MCMCSampler::new(
                    input,
                    observations.clone(),
                    gauss_rects.clone(),
                    rect_std_dev,
                    Params::get().borrow().mcmc_init_duration_sec,
                    &mut rng,
                    &mut judge,
                );

                for i in 0..arrange_trial_count {
                    let remaining_arrange_count = arrange_trial_count - i;
                    let duration = (2.95 - input.since().elapsed().as_secs_f64())
                        / remaining_arrange_count as f64;

                    let search_ratio = 1.0 - Params::get().borrow().mcmc_duration_ratio;
                    let beam_duration =
                        duration * search_ratio * Params::get().borrow().beam_mcts_duration_ratio;
                    let mcts_duration = duration
                        * search_ratio
                        * (1.0 - Params::get().borrow().beam_mcts_duration_ratio);
                    let mcmc_duration = duration * Params::get().borrow().mcmc_duration_ratio;
                    let first_step_turn = input.rect_cnt() - Params::get().borrow().mcts_turn;

                    let sampled_rects = mcmc_sampler.sample(mcmc_duration, &mut rng);

                    let ops1 = beam_arranger.arrange(
                        &input,
                        first_step_turn,
                        sampled_rects.clone(),
                        beam_duration,
                    );

                    let ops2 = mcts_arranger.arrange(
                        &input,
                        &ops1,
                        sampled_rects,
                        &mut rng,
                        mcts_duration,
                    );

                    let mut ops = ops1;
                    ops.extend_from_slice(&ops2);

                    let measure = judge.query(&ops);

                    if i < arrange_trial_count - 1 {
                        let observation =
                            Observation2d::new(ops, measure.width(), measure.height(), true);
                        mcmc_sampler.update(observation);
                    }
                }
            }
        }
    }

    use crate::problem::{Input, Judge};

    pub trait Solver {
        fn solve(&self, input: &Input, judge: impl Judge);
    }
}
#[allow(dead_code)]
mod util {
    //! よく使われるユーティリティ関数をまとめたモジュール

    use num::PrimInt;

    /// 最小値と最大値を更新するトレイト
    ///
    /// # Examples
    ///
    /// ```
    /// use cp_lib_rs::util::ChangeMinMax;
    ///
    /// let mut x = 10;
    /// assert!(x.change_min(3));
    /// assert_eq!(x, 3);
    /// ```
    #[allow(dead_code)]
    pub trait ChangeMinMax {
        fn change_min(&mut self, v: Self) -> bool;
        fn change_max(&mut self, v: Self) -> bool;
    }

    impl<T: PartialOrd> ChangeMinMax for T {
        fn change_min(&mut self, v: T) -> bool {
            *self > v && {
                *self = v;
                true
            }
        }

        fn change_max(&mut self, v: T) -> bool {
            *self < v && {
                *self = v;
                true
            }
        }
    }

    /// 多次元配列を作成する
    ///
    /// # Examples
    ///
    /// ```
    /// use cp_lib_rs::mat;
    ///
    /// let a = mat![0; 4; 3];
    /// assert_eq!(a, vec![vec![0; 3]; 4]);
    /// ```
    #[macro_export]
    macro_rules! mat {
	($($e:expr),*) => { vec![$($e),*] };
	($($e:expr,)*) => { vec![$($e),*] };
	($e:expr; $d:expr) => { vec![$e; $d] };
	($e:expr; $d:expr $(; $ds:expr)+) => { vec![mat![$e $(; $ds)*]; $d] };
}

    /// 整数の二分探索を行う
    ///
    /// # Examples
    ///
    /// ```
    /// use cp_lib_rs::util::binary_search;
    ///
    /// let result = binary_search(0, 10, |x| x * x <= 5);
    /// assert_eq!(result, 2);
    /// ```
    #[allow(dead_code)]
    pub fn binary_search<T: PrimInt>(ok: T, ng: T, f: impl Fn(T) -> bool) -> T {
        let mut ok = ok;
        let mut ng = ng;

        while ok.max(ng) - ok.min(ng) > T::one() {
            let mid = (ok + ng) / (T::one() << 1);

            if f(mid) {
                ok = mid;
            } else {
                ng = mid;
            }
        }

        ok
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    pub struct BitSetIterU128 {
        flag: u128,
    }

    impl BitSetIterU128 {
        pub fn new(flag: u128) -> Self {
            Self { flag }
        }
    }

    impl Iterator for BitSetIterU128 {
        type Item = usize;

        fn next(&mut self) -> Option<Self::Item> {
            while self.flag != 0 {
                let lsb = self.flag.trailing_zeros() as usize;
                self.flag ^= self.flag & -(self.flag as i128) as u128;
                return Some(lsb);
            }

            None
        }
    }

    #[cfg(test)]
    mod test {
        use super::*;

        #[test]
        fn binary_search_test() {
            assert_eq!(binary_search(0, 10, |x| x * x <= 5), 2);
            assert_eq!(binary_search(10, 0, |x| x * x >= 5), 3);
        }

        #[test]
        fn bitset_iter_u128_test() {
            let mut iter = BitSetIterU128 {
                flag: 0b100001101010,
            };
            assert_eq!(iter.next(), Some(1));
            assert_eq!(iter.next(), Some(3));
            assert_eq!(iter.next(), Some(5));
            assert_eq!(iter.next(), Some(6));
            assert_eq!(iter.next(), Some(11));
            assert_eq!(iter.next(), None);
        }
    }
}

fn main() {
    let input = Input::read();
    let judge = problem::gen_judge(&input);
    let solver = solver::solver01::Solver01;
    solver.solve(&input, judge);
    eprintln!("Elapsed: {:?}", input.since().elapsed());
}
