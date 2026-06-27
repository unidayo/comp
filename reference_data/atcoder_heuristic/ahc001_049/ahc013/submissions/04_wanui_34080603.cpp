#include <bits/stdc++.h>
// clang-format off
using namespace std;
void print0(){}; template<typename H,typename... T> void print0(H h,T... t){cout<<h;print0(t...);}
void print(){print0("\n");}; template<typename H,typename... T>void print(H h,T... t){print0(h);if(sizeof...(T)>0)print0(" ");print(t...);}
void perr0(){}; template<typename H,typename... T> void perr0(H h,T... t){cerr<<h;perr0(t...);}
void perr(){perr0("\n");}; template<typename H,typename... T>void perr(H h,T... t){perr0(h);if(sizeof...(T)>0)perr0(" ");perr(t...);}
void ioinit() { cout<<fixed<<setprecision(15); cerr<<fixed<<setprecision(6); ios_base::sync_with_stdio(0); cin.tie(0); }

struct point {int8_t i;int8_t j;};
bool operator==(const point &lhs, const point &rhs) { return (lhs.i == rhs.i && lhs.j == rhs.j); }
bool operator!=(const point &lhs, const point &rhs) { return !(lhs == rhs); }
bool operator<(const point &lhs, const point &rhs) {
    if (lhs.i == rhs.i){return lhs.j<rhs.j;}
    return lhs.i<rhs.i;
}
std::ostream &operator<<(std::ostream &os, point &pt) {
    string s; s.push_back('('); s = s + to_string(pt.i); s = s + ", "; s = s + to_string(pt.j); s.push_back(')'); return os << s;
};
using pii = pair<int, int>;
// clang-format on

const int DEBUG = 0;
const double END_TIME = 2800;

int COMPUTER_PENALTY = 14;
int WALK_PENALTY = 10;

int INITIAL_POINT_TWO_RATIO = 50;
int SELECT_COLOR_BIAS_LIMIT = 20;

int ESCAPE_DEPTH_LIMIT = 3;
int ESCAPE_LEVEL_LIMIT = 5;
int TO_ESCAPE_LIMIT = 5;
int SRC_COMP_LIMIT = 10;
const int PENALTY_LIMIT = 300;

//
const int NX = 48;
const int NX2 = 2304;
const int KX = 5;
//
int N, K, K100, N2;
int tbl_init[NX][NX];

void set_param() {
    // 密な盤面かどうかでハイパーパラメータ値を切り替え
    bool is_dense = false;
    is_dense |= (K == 2 && N <= 16);
    is_dense |= (K == 3 && N <= 20);
    is_dense |= (K == 4 && N <= 23);
    is_dense |= (K == 5 && N <= 26);

    if (is_dense) {
        ESCAPE_LEVEL_LIMIT = 6;
        ESCAPE_DEPTH_LIMIT = 3;
        INITIAL_POINT_TWO_RATIO = 20;
    } else {
        ESCAPE_LEVEL_LIMIT = 5;
        ESCAPE_DEPTH_LIMIT = 3;
        INITIAL_POINT_TWO_RATIO = 80;
    }
}

/*
TODO

* 「2周目が外枠を通る」形が強そう。意図的にその形を作る
-- 外枠を先に埋める。邪魔なやつを先にどける
-- 「 と 」 の形に2色を最初に埋めてから始める
* 密盤面専用ソルバー
-- 焼きなまし系？
-- 別色をクラスタに加える遷移もいれとく
-- まずは後編集が簡単
* BFSなどの高速化
- 「何回目のBFSか」をコスト値にもって、N2 テーブルを作り直さないやつ
* lineデータ構造は始点と終点だけあれば復元できる
* ダイクストラのコストを調整し、良い次状態を選ぶ
- できれば、線によって潰される空白マスが少ない方がよい
*/

const int inf = 1e9;
mt19937 engine(0);
clock_t start_time;
double now() {
    return 1000.0 * (clock() - start_time) / CLOCKS_PER_SEC;
}
void marathon_init() {
    start_time = clock();
    random_device seed_gen;
    engine.seed(seed_gen());
}
double uniform(double x, double y) {
    const int RND = 1e8;
    double mean = (x + y) / 2.0;
    double dif = y - mean;
    double p = double(engine() % RND) / RND;
    return mean + dif * (1.0 - 2.0 * p);
}
int randint(int mn, int mx) {
    int rng = mx - mn + 1;
    return mn + (engine() % rng);
}
class unionfind {
   public:
    vector<int> partition;
    int n;
    int select_repr(int x, int y) {
        return min(x, y);
    }
    unionfind(int n_) {
        partition.resize(n_);
        for (int x = 0; x < n_; x++) {
            partition[x] = x;
        }
        n = n_;
    }
    int find(int x) {
        if (partition[x] == x) {
            return x;
        } else {
            partition[x] = find(partition[x]);
            return partition[x];
        }
    }
    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            return;
        }
        int win = select_repr(x, y);
        int lose = win == x ? y : x;
        partition[lose] = win;
    }
    bool same(int x, int y) {
        return find(x) == find(y);
    }
};

int _p(point p) {
    return p.i * N + p.j;
}
int _q(int i, int j) {
    return i * N + j;
}
point _rp(int p) {
    return {p / N, p % N};
}
bool validpt(point p) {
    return (0 <= p.i && p.i < N && 0 <= p.j && p.j < N);
}
int rand_dir_id = 0;
vector<vector<point>> rand_dirs;
vector<point> *get_rand_dirs() {
    rand_dir_id = (rand_dir_id + 1) % rand_dirs.size();
    return &rand_dirs[rand_dir_id];
}
void datainit() {
    vector<point> src_dirs = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    sort(src_dirs.begin(), src_dirs.end());
    do {
        for (int i = 0; i < 10; i++) {
            rand_dirs.push_back(src_dirs);
        }
    } while (next_permutation(src_dirs.begin(), src_dirs.end()));
    shuffle(rand_dirs.begin(), rand_dirs.end(), engine);
}
vector<point> line_route(point s, point t) {
    // vectorを作る分やや重い
    vector<point> route;
    if (s.i == t.i) {
        if (s.j < t.j) {
            for (int8_t k = s.j; k <= t.j; k++) {
                route.push_back({s.i, k});
            }
            return route;
        } else {
            for (int8_t k = s.j; k >= t.j; k--) {
                route.push_back({s.i, k});
            }
            return route;
        }
    } else if (s.j == t.j) {
        if (s.i < t.i) {
            for (int8_t k = s.i; k <= t.i; k++) {
                route.push_back({k, s.j});
            }
            return route;
        } else {
            for (int8_t k = s.i; k >= t.i; k--) {
                route.push_back({k, s.j});
            }
            return route;
        }
    }
    assert(false);
}

struct state {
    vector<int> c;
    int total_move;
    vector<bool> online;
    vector<int> colors;
    vector<vector<point>> moves;
    vector<vector<point>> lines;  // TODO vector<pair<point,point>> のほうがいいのでは
    void push_move(vector<point> &move) {
        int m = int(move.size());
        this->total_move += m - 1;

        for (int i = 1; i < m; i++) {
            point pre = move[i - 1];
            point cur = move[i];
            swap(this->colors[_p(pre)], this->colors[_p(cur)]);
        }
        this->moves.push_back(move);
    }
    void pop_move() {
        int n = this->moves.size();
        int m = this->moves[n - 1].size();
        this->total_move -= m - 1;

        for (int i = m - 2; i >= 0; i--) {
            point pre = this->moves[n - 1][i + 1];
            point cur = this->moves[n - 1][i];
            swap(this->colors[_p(pre)], this->colors[_p(cur)]);
        }
        this->moves.pop_back();
    }
    void push_line(vector<point> &line) {
        for (auto p : line) {
            this->online[_p(p)] = true;
        }
        this->lines.push_back(line);
    }
    int get_opnum() {
        return this->total_move + int(this->lines.size());
    }
};

pair<bool, state> init_cluster(state &pre_st) {
    if (pre_st.get_opnum() >= K100) {
        return {false, pre_st};
    }
    vector<int> pts;
    for (int i = 0; i < N2; i++) {
        if (!pre_st.online[i] && pre_st.colors[i] >= 0) pts.push_back(i);
    }
    if (pts.size() < 2) {
        return {false, pre_st};
    }

    state st = pre_st;
    if (pre_st.get_opnum() == 0 && engine() % 100 < INITIAL_POINT_TWO_RATIO) {
        // 2色同時
        for (int i = 0; i < 100; i++) {
            int x = pts[engine() % pts.size()];
            int y = pts[engine() % pts.size()];
            if (st.colors[x] != st.colors[y]) {
                st.c.clear();
                st.c.push_back(st.colors[x]);
                st.c.push_back(st.colors[y]);
                st.online[x] = true;
                st.online[y] = true;
                return {true, st};
            }
        }
    }
    {  // 1色
        int x = pts[engine() % pts.size()];
        st.c.clear();
        st.c.push_back(st.colors[x]);
        st.online[x] = true;
        return {true, st};
    }
}

namespace dijkstra {

// pushするコストは、pop済みのコスト以上の値のみ可能

int costs[NX2];
bool done[NX2];
point frm[NX2];
//
const int PENALTY_LIMIT_2 = PENALTY_LIMIT + 10;
const int PQ_SIZE = NX2 * 4 + 10;
point pq[PENALTY_LIMIT_2][PQ_SIZE];

int pq_num[PENALTY_LIMIT_2];
int curcost = 0;
int curind = 0;
int max_cost = -1;

void init() {
    curcost = 0;
    curind = 0;
    max_cost = -1;
    for (int i = 0; i < PENALTY_LIMIT_2; i++) {
        pq_num[i] = 0;
    }
    for (int i = 0; i < N2; i++) {
        costs[i] = inf;
        done[i] = false;
        frm[i] = {-1, -1};
    }
}

bool pq_can_pop() {
    return (curcost < max_cost || (curcost == max_cost && curind < pq_num[curcost]));
}
pair<int, point> pq_pop() {
    while (curind >= pq_num[curcost]) {
        curind = 0;
        curcost++;
    }

    point pt = pq[curcost][curind];
    curind++;
    return {curcost, pt};
}
void pq_push(int cost, point p) {
    pq[cost][pq_num[cost]] = p;
    pq_num[cost]++;
    max_cost = max(cost, max_cost);
}
}  // namespace dijkstra

vector<vector<point>> bfs_escape(point comp, vector<bool> &online, unordered_set<int> &reserved, vector<int> &colors, int level) {
    // BFSでonline・reserverdでない空白マスに移動させる
    point goal = {-1, -1};
    queue<point> que;
    que.push(comp);
    unordered_map<int, int> costs;
    unordered_map<int, int> frm;
    costs[_p(comp)] = 0;
    while (que.size()) {
        point u = que.front();
        que.pop();
        if (!reserved.count(_p(u)) && !online[_p(u)]) {
            goal = u;
            break;  // 予約されてない空白マスに移動できれば成功
        }
        if (costs[_p(u)] == ESCAPE_DEPTH_LIMIT) continue;
        for (auto dir : *get_rand_dirs()) {
            point v = {dir.i + u.i, dir.j + u.j};
            if (!validpt(v)) continue;
            if (colors[_p(v)] >= 0) continue;
            if (costs.count(_p(v))) continue;
            costs[_p(v)] = costs[_p(u)] + 1;
            frm[_p(v)] = _p(u);
            que.push(v);
        }
    }
    if (goal.i < 0) {
        if (level < ESCAPE_LEVEL_LIMIT) {
            for (auto dir : *get_rand_dirs()) {
                // 隣のやつをどかす // TODO レベル増やしたときMAX_DEPTH減らす
                point ncomp = {comp.i + dir.i, comp.j + dir.j};
                if (!validpt(ncomp)) continue;
                if (online[_p(ncomp)]) continue;
                int np = _p(ncomp);
                if (reserved.count(_p(ncomp))) continue;
                reserved.insert(np);
                auto mv1 = bfs_escape(ncomp, online, reserved, colors, level + 1);
                reserved.erase(np);

                if (mv1.size()) {
                    mv1.push_back({{comp, ncomp}});
                    return mv1;
                }
            }
        }
        return {};
    }
    vector<int> movei = {_p(goal)};
    int init = _p(comp);
    while (true) {
        int cur = movei.back();
        int nxt = frm[cur];
        movei.push_back(nxt);
        if (nxt == init) break;
    }
    reverse(movei.begin(), movei.end());
    vector<point> move;
    for (auto mv : movei) {
        move.push_back(_rp(mv));
    }
    return {move};
}

bool connect_computer(point src_comp, vector<point> &move, state &st, int now_color) {
    vector<point> to_escape;
    for (auto pt : move) {
        if (pt != src_comp && st.colors[_p(pt)] >= 0) to_escape.push_back(pt);
    }

    point tgt = move.back();  // 目的地
    if (st.online[_p(tgt)]) {
        // TODO コピペやめる
        // 目的地を含む既存の線
        pair<point, point> tgt_line = {{-1, -1}, {-1, -1}};
        for (auto line : st.lines) {  // TODO やや効率悪い
            auto p = line.front();
            auto q = line.back();
            if (p.i == q.i && tgt.i == p.i && ((p.j < tgt.j && tgt.j < q.j) || (q.j < tgt.j && tgt.j < p.j))) {
                tgt_line = {p, q};
                break;
            }
            if (p.j == q.j && tgt.j == p.j && ((p.i < tgt.i && tgt.i < q.i) || (q.i < tgt.i && tgt.i < p.i))) {
                tgt_line = {p, q};
                break;
            }
        }
        if (to_escape.size() >= TO_ESCAPE_LIMIT) {
            return false;
        }

        unordered_set<int> reserved;
        for (auto pt : move) reserved.insert(_p(pt));

        // 間にあるコンピュータを退避させる
        int mvnum = 0;
        bool escape_ok = true;
        for (auto comp : to_escape) {
            auto ems = bfs_escape(comp, st.online, reserved, st.colors, 0);
            if (ems.size() <= 0) {
                escape_ok = false;
                break;
            }
            for (auto em : ems) {
                st.push_move(em);
                mvnum++;
            }
        }

        int mvlen = int(move.size()) - 1;
        if (!escape_ok || st.get_opnum() + mvlen + 1 > K100) {
            for (int i = 0; i < mvnum; i++) {
                st.pop_move();
            }
            return false;
        }

        st.push_move(move);
        vector<vector<point>> newlines;
        for (auto line : st.lines) {
            auto p = line.front();
            auto q = line.back();
            if (tgt_line == pair<point, point>{p, q}) continue;
            newlines.push_back(line);
        }
        newlines.push_back(line_route(tgt_line.first, tgt));
        newlines.push_back(line_route(tgt, tgt_line.second));
        st.lines = newlines;
        return true;
    } else {
        // 目的地に到達できる開始点
        vector<point> base_escape;
        point base = {-1, -1};
        int min_pena = inf;
        for (auto dir : *get_rand_dirs()) {
            vector<point> d_escape;
            int pena = 0;
            point cur = tgt;
            while (true) {
                point nxt = {dir.i + cur.i, dir.j + cur.j};
                if (!validpt(nxt)) break;
                int c = st.colors[_p(nxt)];
                if (st.online[_p(nxt)]) {
                    if (c == now_color && min_pena > pena) {
                        min_pena = pena;
                        base = nxt;
                        base_escape = d_escape;
                    }
                    break;
                }
                if (c >= 0) {
                    pena++;
                    d_escape.push_back(nxt);
                }
                cur = nxt;
            }
        }
        if (base.i < 0) return false;
        for (auto p : base_escape) to_escape.push_back(p);
        if (to_escape.size() >= TO_ESCAPE_LIMIT) {
            return false;
        }
        vector<point> line = line_route(base, tgt);
        unordered_set<int> reserved;
        for (auto pt : line) reserved.insert(_p(pt));
        for (auto pt : move) reserved.insert(_p(pt));

        // 間にあるコンピュータを退避させる
        int mvnum = 0;
        bool escape_ok = true;
        for (auto comp : to_escape) {
            auto ems = bfs_escape(comp, st.online, reserved, st.colors, 0);
            if (ems.size() <= 0) {
                escape_ok = false;
                break;
            }
            for (auto em : ems) {
                st.push_move(em);
                mvnum++;
            }
        }

        int mvlen = int(move.size()) - 1;
        if (!escape_ok || st.get_opnum() + mvlen + 1 > K100) {
            for (int i = 0; i < mvnum; i++) {
                st.pop_move();
            }
            return false;
        }

        st.push_move(move);
        st.push_line(line);
        return true;
    }
}

void set_dijkstra_starts(vector<int> &init_costs, state &st, int now_color) {
    // 既存クラスタの直線状にあって間にコンピュータがない場所を列挙
    for (int8_t i = 0; i < N; i++) {
        for (int8_t j = 0; j < N; j++) {
            if (!(st.online[_q(i, j)] && st.colors[_q(i, j)] == now_color)) continue;
            for (auto dir : *get_rand_dirs()) {
                point cur = {i, j};
                int cost = 0;
                while (true) {
                    point nxt = {dir.i + cur.i, dir.j + cur.j};
                    if (!validpt(nxt)) break;
                    if (st.online[_p(nxt)]) break;
                    int c = st.colors[_p(nxt)];
                    if (c == now_color) {
                        init_costs[_p(nxt)] = min(init_costs[_p(nxt)], cost);
                        break;
                    } else if (c >= 0) {
                        cost += COMPUTER_PENALTY;
                        init_costs[_p(nxt)] = min(init_costs[_p(nxt)], cost);
                    } else {
                        init_costs[_p(nxt)] = min(init_costs[_p(nxt)], cost);
                    }
                    cur = nxt;
                }
            }
        }
    }
    // 対象の色をつなぐケーブル上も開始点にする
    for (auto line : st.lines) {
        auto p = line.front();
        auto q = line.back();
        if (st.colors[_p(p)] != now_color || st.colors[_p(q)] != now_color) continue;
        for (auto pt : line) {
            if (pt == p || pt == q) continue;
            init_costs[_p(pt)] = 0;
        }
    }
}

bool run_dijkstra(state &st, vector<int> &init_costs, int candidate_computers, int now_color) {
    dijkstra::init();
    for (int i = 0; i < N2; i++) {
        if (init_costs[i] < PENALTY_LIMIT) {
            dijkstra::pq_push(init_costs[i], _rp(i));
            dijkstra::costs[i] = init_costs[i];
        }
    }
    int src_comp_searched = 0;
    bool success = false;
    int src_comp_limit = min(SRC_COMP_LIMIT, candidate_computers);
    while (dijkstra::pq_can_pop()) {
        point u;
        int cost;
        tie(cost, u) = dijkstra::pq_pop();
        if (dijkstra::done[_p(u)]) continue;
        dijkstra::done[_p(u)] = true;
        if (st.colors[_p(u)] == now_color && !st.online[_p(u)]) {
            // 復元
            vector<point> move = {u};
            if (dijkstra::frm[_p(u)].i >= 0) {
                while (true) {
                    point cur = move.back();
                    point nxt = dijkstra::frm[_p(cur)];
                    move.push_back(nxt);
                    if (dijkstra::frm[_p(nxt)].i < 0) break;
                }
            }
            // コストが小さいコンピュータから順に、そのコンピュータをクラスタに取り込めるか試す。取り込み成功したら終了。
            success |= connect_computer(u, move, st, now_color);
            src_comp_searched++;
            if (success || src_comp_searched >= src_comp_limit) {
                return success;
            } else {
                continue;
            }
        }
        for (auto dir : *get_rand_dirs()) {
            point v = {u.i + dir.i, u.j + dir.j};
            if (!validpt(v)) continue;
            if (st.online[_p(v)] && st.colors[_p(v)] >= 0) continue;
            int nc = cost + WALK_PENALTY;
            if (st.colors[_p(v)] >= 0 && st.colors[_p(v)] != now_color) nc += COMPUTER_PENALTY;
            if (PENALTY_LIMIT < nc) continue;
            if (dijkstra::costs[_p(v)] <= nc) continue;
            dijkstra::costs[_p(v)] = nc;
            dijkstra::frm[_p(v)] = u;
            dijkstra::pq_push(nc, v);
        }
    }
    return success;
}
state greedy(state init_state) {
    state st = init_state;

    int iteration = 0;
    set<int> color_failure;
    int select_color_ratio = randint(50 - SELECT_COLOR_BIAS_LIMIT, 50 + SELECT_COLOR_BIAS_LIMIT);
    while (true) {
        if (st.get_opnum() >= K100) break;
        if (color_failure.size() >= st.c.size()) break;

        iteration++;
        int now_color = (st.c.size() == 2 && engine() % 100 < select_color_ratio) ? st.c[1] : st.c[0];

        int candidate_computers = 0;
        for (int i = 0; i < N2; i++) {
            if (!st.online[i] && st.colors[i] == now_color) candidate_computers++;
        }

        // ダイクストラ開始点
        vector<int> init_costs(N2, inf);
        set_dijkstra_starts(init_costs, st, now_color);

        // 多始点ダイクストラ
        bool success = run_dijkstra(st, init_costs, candidate_computers, now_color);
        if (!success) {
            color_failure.insert(now_color);
        }
    }

    return st;
}
void output(state &st) {
    vector<pair<point, point>> mvops;
    for (auto move : st.moves) {
        int m = move.size();
        for (int j = 1; j < m; j++) {
            mvops.push_back({move[j - 1], move[j]});
        }
    }

    if (DEBUG) {
        for (int opnum = 1; opnum < int(mvops.size() + st.lines.size()); opnum++) {
            int r = opnum;
            print(min(r, int(mvops.size())));
            for (int j = 0; j < int(mvops.size()); j++) {
                if (r <= 0) break;
                auto op = mvops[j];
                print(int(op.first.i), int(op.first.j), int(op.second.i), int(op.second.j));
                r--;
            }
            print(min(r, int(st.lines.size())));
            for (int j = 0; j < int(st.lines.size()); j++) {
                if (r <= 0) break;
                auto line = st.lines[j];
                print(int(line.front().i), int(line.front().j), int(line.back().i), int(line.back().j));
                r--;
            }
        }
    }
    print(mvops.size());
    for (auto op : mvops) {
        print(int(op.first.i), int(op.first.j), int(op.second.i), int(op.second.j));
    }

    print(st.lines.size());
    for (auto line : st.lines) {
        print(int(line.front().i), int(line.front().j), int(line.back().i), int(line.back().j));
    }
}
tuple<int, int, int> evaluate(state &st) {
    vector<pair<point, point>> mvops;
    for (auto move : st.moves) {
        int m = move.size();
        for (int j = 1; j < m; j++) {
            mvops.push_back({move[j - 1], move[j]});
        }
    }
    if (DEBUG) {
        assert(mvops.size() == st.total_move);
    }
    int total_op = mvops.size() + st.lines.size();
    int score = 0;
    if (total_op > K100) score = -inf;

    unionfind uf(N2);
    vector<bool> online(N2);
    for (auto line : st.lines) {
        point s = line.front();
        point t = line.back();
        if (st.colors[_p(s)] < 0 || st.colors[_p(t)] < 0) {
            score = -inf;
        }
        for (auto p : line) {
            if (p == s || p == t) continue;
            if (st.colors[_p(p)] >= 0) score = -inf;
            if (online[_p(p)]) score = -inf;
            online[_p(p)] = true;
        }

        uf.unite(_p(s), _p(t));
    }
    vector<point> comps;
    for (int8_t i = 0; i < N; i++) {
        for (int8_t j = 0; j < N; j++) {
            if (st.colors[_q(i, j)] >= 0) comps.push_back({i, j});
        }
    }
    int m = comps.size();
    for (int i = 0; i < m; i++) {
        for (int j = i + 1; j < m; j++) {
            point x = comps[i];
            point y = comps[j];
            if (uf.same(_p(x), _p(y))) {
                if (st.colors[_p(x)] == st.colors[_p(y)]) {
                    score++;
                } else {
                    score--;
                }
            }
        }
    }
    if (score < 0) score = -1;
    return {mvops.size(), st.lines.size(), score};
}
int main() {
    marathon_init();
    ioinit();
    cin >> N >> K;
    K100 = K * 100;
    N2 = N * N;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            char c;
            cin >> c;
            // 色は0~K-1 に直す。空白は-1
            tbl_init[i][j] = int(c - '0') - 1;
        }
    }
    datainit();
    set_param();
    state best_st;
    int best_score = -inf;
    int simulation_iter = 0;
    for (simulation_iter = 0; now() < END_TIME; simulation_iter++) {
        state st;
        st.colors.resize(N2);
        st.online.resize(N2);
        st.total_move = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                st.colors[_q(i, j)] = tbl_init[i][j];
            }
        }

        while (true) {
            state st2;
            bool has_op_rest;
            tie(has_op_rest, st2) = init_cluster(st);
            if (!has_op_rest) {
                st = st2;
                break;
            }
            auto st3 = greedy(st2);
            swap(st3, st);
        }

        int mvops, conops, score;
        tie(mvops, conops, score) = evaluate(st);
        if (best_score < score) {
            best_score = score;
            best_st = st;
        }
    }
    {
        output(best_st);
        int mvops, conops, score;
        tie(mvops, conops, score) = evaluate(best_st);
        perr0("move_op:", mvops, " connect_op:", conops, " total_op:", (mvops + conops), " score:", score, " time:", int(now()), " simulation_iter:", simulation_iter, "\n");
    }
    return 0;
}
