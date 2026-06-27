#include <bits/stdc++.h>
// clang-format off
using namespace std;
#define debug1(a) { cerr<<#a<<":"<<a<<endl; }
#define debug2(a,b) { cerr<<#a<<":"<<a<<" "<<#b<<":"<<b<<endl; }
#define debug3(a,b,c) { cerr<<#a<<":"<<a<<" "<<#b<<":"<<b<<" "<<#c<<":"<<c<<endl; }
#define debug4(a,b,c,d) { cerr<<#a<<":"<<a<<" "<<#b<<":"<<b<<" "<<#c<<":"<<c<<" "<<#d<<":"<<d<<endl; }

using ull = unsigned long long;
using pii = pair<int,int>;
using tii = tuple<int,int,int>;
struct point {int i;int j;};
bool operator==(const point &lhs, const point &rhs) { return (lhs.i == rhs.i && lhs.j == rhs.j); }
bool operator!=(const point &lhs, const point &rhs) { return !(lhs == rhs); }
bool operator<(const point &lhs, const point &rhs) {
    if (lhs.i == rhs.i){return lhs.j<rhs.j;}
    return lhs.i<rhs.i;
}
std::ostream &operator<<(std::ostream &os, point &pt) {
    string s;    s.push_back('(');s = s + to_string(pt.i);s = s + ", ";s = s + to_string(pt.j);s.push_back(')');return os << s;
};

const int inf =1e9;
#ifdef ONLINE_JUDGE
const int LOCAL = 0;
#else
const int LOCAL = 1;
#endif
// clang-format on

int SPACE = 17;

int MINPOWER = 20;

vector<double> CHECK_BIAS = {2.4, 1.0};
vector<int> CHECK_LIMIT = {44, 170, 300};

double MONTECARLO_WEIGHT_POW = 2.6;

double MUST_OPEN_BOTTOM_DIFF = 1.05;
double MUST_OPEN_TOP_DIFF = 1.0;
double MUST_OPEN_FIRST_TOP_RATIO = 0.7;

double MONTECARLO_END_TIME = 4700;
int MONTECARLO_ITER = 100;
double MONTECARLO_CANEND_CHECK_RATIO = 0.14;
int MONTECARLO_CANEND_LAST_LOOP = 20;

int MAKE_ROUTE_GREEDY_ITER = 20;

void setparam(int c, int w, int k) {
    int logc = 0;
    while ((1 << logc) < c) {
        logc++;
    }
    vector<int> MINPOWER_TABLE = {
        5,
        5,
        8,
        12,
        12,
        20,
        25,
        35,
    };
    MINPOWER = MINPOWER_TABLE[logc];

    vector<double> CHECK_BIAS_FIRST_TABLE = {
        3.0,
        3.0,
        2.4,
        2.4,
        2.2,
        2.2,
        2.0,
        1.6,
    };
    CHECK_BIAS[0] = CHECK_BIAS_FIRST_TABLE[logc];

    vector<double> MONTECARLO_CANEND_CHECK_RATIO_TABLE = {
        0.12,
        0.12,
        0.14,
        0.14,
        0.17,
        0.17,
        0.17,
        0.30,
    };
    MONTECARLO_CANEND_CHECK_RATIO = MONTECARLO_CANEND_CHECK_RATIO_TABLE[logc];

    vector<double> MONTECARLO_WEIGHT_POW_TABLE = {
        3.8,
        3.4,
        3.0,
        2.8,
        2.6,
        2.4,
        2.4,
        2.0,
    };
    MONTECARLO_WEIGHT_POW = MONTECARLO_WEIGHT_POW_TABLE[logc];

    vector<double> MUST_OPEN_TOP_DIFF_TABLE = {
        1.0,
        1.0,
        1.0,
        1.0,
        1.0,
        1.05,
        1.1,
        1.15,
    };
    MUST_OPEN_TOP_DIFF = MUST_OPEN_TOP_DIFF_TABLE[logc];
}

namespace marathon {
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
}  // namespace marathon

class unionfind {
   public:
    vector<int> partition;
    vector<int> rank;
    vector<int> siz;
    int n;
    unionfind(int n_) {
        partition.resize(n_);
        rank.resize(n_);
        siz.resize(n_);
        for (int x = 0; x < n_; x++) {
            partition[x] = x;
            rank[x] = 0;
            siz[x] = 1;
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
        int sx = siz[x];
        int sy = siz[y];
        if (rank[x] < rank[y]) {
            partition[x] = y;
        } else {
            partition[y] = x;
            if (rank[x] == rank[y]) {
                rank[x]++;
            }
        }
        siz[find(x)] = sx + sy;
    }
    bool same(int x, int y) {
        return find(x) == find(y);
    }
};

const int N = 200;
const int N2 = 40000;
const int PX = 5000;
const int PM = 10;
int C;
vector<point> Waters;
vector<point> Houses;
vector<vector<bool>> HasWater(N, vector<bool>(N));
vector<vector<bool>> HasHouse(N, vector<bool>(N));

bool ingrid(int i, int j) {
    return 0 <= i && i < N && 0 <= j && j < N;
}
bool ingrid(point p) {
    return 0 <= p.i && p.i < N && 0 <= p.j && p.j < N;
}
int _p(int i, int j) {
    return i * N + j;
}
int _p(point p) {
    return p.i * N + p.j;
}
vector<point> mvs = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

struct srange_t {
    // 各点の頑丈さについての知識は[下限,上限]の区間として表せる。掘るごとに区間が更新される。
    int bottom;
    int top;
};
struct history_t {
    vector<tuple<point, int, int>> ops;
    vector<vector<int>> total_power;
    vector<vector<int>> total_checked;
    vector<vector<srange_t>> sturdiness_range;
    vector<vector<bool>> opened;

    history_t() {
        total_power.resize(N, vector<int>(N));
        total_checked.resize(N, vector<int>(N));
        sturdiness_range.resize(N, vector<srange_t>(N, {PM, PX}));
        opened.resize(N, vector<bool>(N));
    }
};
namespace local {
vector<vector<int>> init_sturdiness;
vector<vector<int>> sturdiness;
unionfind uf(1);
void local_init() {
    uf = unionfind(N2);
    init_sturdiness.resize(N, vector<int>(N));
    sturdiness.resize(N, vector<int>(N));
}
int request(int i, int j, int p) {
    assert(ingrid(i, j));
    assert(1 <= p && p <= PX);
    sturdiness[i][j] -= p;
    if (sturdiness[i][j] <= 0) {
        for (auto mv : mvs) {
            int ni = i + mv.i;
            int nj = j + mv.j;
            if (ingrid(ni, nj) && sturdiness[ni][nj] <= 0) {
                uf.unite(_p(i, j), _p(ni, nj));
            }
        }
        for (auto h : Houses) {
            bool hok = false;
            for (auto w : Waters) {
                if (uf.same(_p(h), _p(w))) hok = true;
            }
            if (!hok) {
                return 1;
            }
        }
        return 2;
    } else {
        return 0;
    }
}
void output_result(history_t &history) {
    int totalcost = 0;
    for (auto his : history.ops) {
        point pt;
        int power;
        int resp;
        tie(pt, power, resp) = his;
        totalcost += power + C;
    }
    cerr << "score==" << totalcost << " ops==" << history.ops.size() << " time==" << marathon::now() <<  //
        " Waters==" << Waters.size() << " Houses==" << Houses.size() << " ConstantStamina==" << C << endl;
}
};  // namespace local

int request(int i, int j, int p, history_t &history) {
    cout << i << " " << j << " " << p << endl;
    int resp = 0;
    if (LOCAL) {
        resp = local::request(i, j, p);
    } else {
        cin >> resp;
    }
    assert(resp >= 0);
    history.ops.push_back({{i, j}, p, resp});
    history.total_power[i][j] += p;
    if (resp >= 1) {
        history.opened[i][j] = true;
        history.sturdiness_range[i][j].top = min(history.sturdiness_range[i][j].top, history.total_power[i][j]);
    } else {
        history.sturdiness_range[i][j].bottom = max(history.sturdiness_range[i][j].bottom, history.total_power[i][j] + 1);
    }
    if (resp >= 2) {
        if (LOCAL) {
            local::output_result(history);
        }
        exit(0);
    }
    return resp;
}

namespace dp {
// 残耐久の上限に対して最も良いパワーを期待値DPで求めておく。
vector<int> powers(PX + 1, PX);
void init() {
    vector<double> dp(PX + 1, inf);
    dp[0] = 0;
    powers[0] = 0;
    for (int top = 1; top <= PX; top++) {
        for (int power = 1; power <= top; power++) {
            // 確率 power / top で破壊できる (一様分布である前提)
            double prob = 1.0 * power / top;

            double cur = 0;
            cur += prob * (power + C);                            // 1回で破壊できる
            cur += (1.0 - prob) * (power + C + dp[top - power]);  // topを更新してもう一度

            if (dp[top] > cur) {
                powers[top] = power;
                dp[top] = cur;
            }
        }
    }
}
};  // namespace dp

namespace solver {
int calc_distance(point a, point b) {
    return abs(a.i - b.i) + abs(a.j - b.j);
}

void must_open(point pt, history_t &history, int top, int bottom) {
    // 破壊できるまで繰り返し掘る。パワーは期待値DPで求めた値を使う。
    int curtop = top;
    bool first = true;
    while (!history.opened[pt.i][pt.j]) {
        int power = 0;
        if (first) {
            first = false;
            // 初回パワーは根拠はないがこれがいいらしい。普通は dp::powers[top - bottom] + bottom; にすると思う
            power = round(top * MUST_OPEN_FIRST_TOP_RATIO + bottom * (1.0 - MUST_OPEN_FIRST_TOP_RATIO));
            power = min(power, PX);
        } else if (curtop > 0) {
            power = dp::powers[curtop];
        }
        power = max(power, MINPOWER);  // 予想が外れていた時用に最小パワーを決めておく
        request(pt.i, pt.j, power, history);
        curtop -= power;
    }
}
void must_open_mid(point pt, history_t &history) {
    if (history.opened[pt.i][pt.j]) return;

    queue<point> que;
    que.push(pt);
    vector<vector<bool>> done(N, vector<bool>(N));
    done[pt.i][pt.j] = true;
    vector<point> nears;
    while (que.size() && nears.size() < 1) {
        point u = que.front();
        que.pop();
        if (u != pt && history.opened[u.i][u.j]) {
            nears.push_back(u);
        }
        for (auto mv : mvs) {
            point v = {u.i + mv.i, u.j + mv.j};
            if (!ingrid(v)) continue;
            if (done[v.i][v.j]) continue;
            done[v.i][v.j] = true;
            que.push(v);
        }
    }

    if (nears.size() >= 1) {
        // 近い位置の頑丈さ範囲から現在の位置の頑丈さ範囲を推定して掘る TODO 近傍を1点より多くする
        point p = nears[0];
        int top = round(MUST_OPEN_TOP_DIFF * history.sturdiness_range[p.i][p.j].top);
        top = min(top, PX);
        int bottom = round(MUST_OPEN_BOTTOM_DIFF * history.sturdiness_range[p.i][p.j].bottom);
        bottom = max(bottom, PM);
        must_open(pt, history, top, bottom);
    } else {
        auto rng = history.sturdiness_range[pt.i][pt.j];
        must_open(pt, history, rng.top, rng.bottom);
    }
}

void check_limit(point pt, history_t &history, int lim) {
    // 合計パワーの上限ありで試し掘り。パワーは期待値DPで求めた値を使う。
    // TODO DPで求めたのは必ず破壊するためのパワー配分であり、データをとるためとは目的が違うはずだが...
    int curtop = lim;
    while (!history.opened[pt.i][pt.j]) {
        int power = dp::powers[curtop];
        power = max(power, MINPOWER);
        request(pt.i, pt.j, power, history);
        curtop -= power;
        if (curtop <= 0) break;
    }

    history.total_checked[pt.i][pt.j]++;
}
pair<double, vector<bool>> greedy(vector<point> &nodes, vector<vector<tii>> &edgmap, int m) {
    double totalcost = 0;
    int n = nodes.size();
    vector<int> waters;
    vector<bool> iswater(n);
    vector<int> houses;
    for (int u = 0; u < n; u++) {
        point pt = nodes[u];
        if (HasWater[pt.i][pt.j]) {
            iswater[u] = true;
            waters.push_back(u);
        }
        if (HasHouse[pt.i][pt.j]) {
            houses.push_back(u);
        }
    }
    vector<bool> con(m);

    // 各家について水源に到達するまでダイクストラを行う
    // TODO順番を固定するか、全ての中で一番いいやつを選んだほうがいいかも
    shuffle(houses.begin(), houses.end(), marathon::engine);
    for (int h : houses) {
        const ull BIT = 16;
        const ull MASK = (1 << BIT) - 1;
        priority_queue<ull, vector<ull>, greater<ull>> pq;
        pq.push(h);
        vector<int> frm(n, -1);
        vector<int> costs(n, inf);
        vector<bool> done(n);
        int wt = -1;
        while (pq.size()) {
            ull cu = pq.top();
            pq.pop();
            int cost = (cu >> BIT);
            int u = (cu & MASK);
            if (done[u]) continue;
            done[u] = true;
            costs[u] = cost;
            if (iswater[u]) {
                wt = u;
                break;
            }
            for (auto e : edgmap[u]) {
                int eid, w, v;
                tie(eid, w, v) = e;
                ull nc = cost + w;
                if (costs[v] <= nc) continue;
                costs[v] = nc;
                frm[v] = u;
                pq.push((nc << BIT) + ull(v));
            }
        }
        totalcost += costs[wt];
        int cur = wt;
        while (cur != h) {
            iswater[cur] = true;  // 水源と接続されたノードは実質水源であるとみなす
            for (auto e : edgmap[cur]) {
                int eid, v;
                double d;
                tie(eid, d, v) = e;
                if (v != frm[cur]) continue;
                con[eid] = true;
                break;
            }
            cur = frm[cur];
        }
        iswater[h] = true;
    }
    return {totalcost, con};
}

void solve() {
    // 斜めの格子上に代表点をとって代表点をグラフとする
    // モンテカルロでコストを無作為に割り当てて解を作ってみて、有用そうな代表点を探し、その代表点を掘ってみてコストを推定する
    auto history = history_t();

    vector<point> nodes;
    vector<pii> edgs;
    int n = 0;
    int m = 0;
    {
        // 代表点でグラフ作る
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                // 代表点：斜めの格子(右端・下端を含む)・水源・家
                if (((i % SPACE == 0 || i == N - 1) && (j % SPACE == 0 || j == N - 1)) || ((i + SPACE / 2) % SPACE == 0 && (j + SPACE / 2) % SPACE == 0) || HasWater[i][j] || HasHouse[i][j]) {
                    nodes.push_back({i, j});
                }
            }
        }
        n = nodes.size();
        for (int u = 0; u < n; u++) {
            for (int v = u + 1; v < n; v++) {
                if (calc_distance(nodes[u], nodes[v]) <= SPACE + 1) {
                    edgs.push_back({u, v});
                }
            }
        }
        m = edgs.size();
    }
    {
        // モンテカルロのシミュレーションで有力な代表点を選んで試し掘り、を繰り返す
        int montecarlo_loop_count = 0;
        for (int montecarlo_loop = inf; montecarlo_loop > 0 && marathon::now() < MONTECARLO_END_TIME; montecarlo_loop--) {
            montecarlo_loop_count++;

            bool canend = true;
            {
                // 開通した頂点のみをつないで全ての家が水源に接続可能なら終了可能
                auto uf = unionfind(n);
                for (auto e : edgs) {
                    point u = nodes[e.first];
                    point v = nodes[e.second];
                    if (history.opened[u.i][u.j] && history.opened[v.i][v.j]) uf.unite(e.first, e.second);
                }
                vector<int> waters;
                vector<int> houses;
                for (int u = 0; u < n; u++) {
                    point pt = nodes[u];
                    if (HasWater[pt.i][pt.j]) {
                        waters.push_back(u);
                    }
                    if (HasHouse[pt.i][pt.j]) {
                        houses.push_back(u);
                    }
                }
                for (auto h : houses) {
                    bool ok = false;
                    for (auto w : waters) {
                        if (uf.same(h, w)) ok = true;
                    }
                    if (!ok) {
                        canend = false;
                        break;
                    }
                }
                if (canend) {
                    // 終了可能になったらあと20回程度回す
                    montecarlo_loop = min(montecarlo_loop, MONTECARLO_CANEND_LAST_LOOP);
                }
            }

            int montecarlo_iter = MONTECARLO_ITER;
            int montecarlo_maxcheck = 1;

            // TLE回避
            if (marathon::now() > MONTECARLO_END_TIME * 0.9) {
                montecarlo_iter = round(MONTECARLO_ITER * 0.1);
                montecarlo_maxcheck = 3;
            } else if (marathon::now() > MONTECARLO_END_TIME * 0.4) {
                montecarlo_maxcheck = 2;
            }

            vector<int> used_node_montecarlo(n);
            for (int it = 0; it < montecarlo_iter; it++) {
                // コストを無作為に決定して貪欲に解く、を繰り返し
                vector<double> weights(n);
                for (int u = 0; u < n; u++) {
                    point up = nodes[u];
                    double top = history.sturdiness_range[up.i][up.j].top;
                    double bottom = history.sturdiness_range[up.i][up.j].bottom;
                    // 一様乱数ではなく適当にスケーリングする
                    weights[u] = pow(marathon::uniform(pow(bottom, 1.0 / MONTECARLO_WEIGHT_POW), pow(top, 1.0 / MONTECARLO_WEIGHT_POW)), MONTECARLO_WEIGHT_POW);
                }
                vector<vector<tii>> edgmap(n);
                for (int eid = 0; eid < m; eid++) {
                    auto e = edgs[eid];
                    int u = e.first;
                    int v = e.second;
                    int d = calc_distance(nodes[u], nodes[v]);
                    int w = d * (weights[u] + weights[v]) / 2;
                    edgmap[u].push_back({eid, w, v});
                    edgmap[v].push_back({eid, w, u});
                }

                vector<bool> con = greedy(nodes, edgmap, m).second;
                vector<bool> used_node(n);
                for (int eid = 0; eid < m; eid++) {
                    if (con[eid]) {
                        auto e = edgs[eid];
                        int u = e.first;
                        int v = e.second;
                        used_node[u] = true;
                        used_node[v] = true;
                    }
                }
                for (int u = 0; u < n; u++) {
                    // 代表点ごとに道が通った回数をカウント
                    used_node_montecarlo[u] += used_node[u];
                }
            }

            // 試し掘りする代表点の評価値
            vector<pair<double, int>> usedscore;
            for (int u = 0; u < n; u++) {
                point pt = nodes[u];
                if (history.opened[pt.i][pt.j]) continue;

                // モンテカルロのシミュレーションで道が通った回数が多い点を優先
                pair<double, int> s = {used_node_montecarlo[u], u};

                // 今まであまり掘ってない点を優先
                int pt_checked = history.total_checked[pt.i][pt.j];
                double bias = pt_checked < int(CHECK_BIAS.size()) ? CHECK_BIAS[pt_checked] : CHECK_BIAS.back();
                s.first *= bias;

                // 終了可能状態では、評価値が低い点はスキップする
                double ratio = 1.0 * s.first / montecarlo_iter;
                if (canend && ratio - 1e-3 < MONTECARLO_CANEND_CHECK_RATIO) continue;

                usedscore.push_back(s);
            }
            sort(usedscore.rbegin(), usedscore.rend());

            int checked = 0;
            for (auto us : usedscore) {
                // 試し掘りして頑丈さを確認. これまで掘った回数に応じて今回掘るパワー上限を変える
                point pt = nodes[us.second];
                int pt_checked = history.total_checked[pt.i][pt.j];
                int limit = pt_checked < int(CHECK_LIMIT.size()) ? CHECK_LIMIT[pt_checked] : CHECK_LIMIT.back();
                check_limit(pt, history, limit);
                checked++;
                if (checked >= montecarlo_maxcheck) break;
            }
        }
        debug1(montecarlo_loop_count);
    }

    vector<pair<point, point>> connection;
    {
        // 最終的に使うグラフ上の経路を決める
        vector<double> weights(n);
        for (int u = 0; u < n; u++) {
            point up = nodes[u];

            if (history.opened[up.i][up.j]) {
                int top = history.sturdiness_range[up.i][up.j].top;
                int bottom = history.sturdiness_range[up.i][up.j].bottom;
                weights[u] = (top + bottom) / 2;
            } else {
                // 開通していない点は5000とする
                weights[u] = PX;
            }
        }
        vector<vector<tii>> edgmap(n);
        for (int eid = 0; eid < m; eid++) {
            auto e = edgs[eid];
            int u = e.first;
            int v = e.second;
            int d = calc_distance(nodes[u], nodes[v]);
            int w = d * (weights[u] + weights[v]) / 2;
            edgmap[u].push_back({eid, w, v});
            edgmap[v].push_back({eid, w, u});
        }

        vector<bool> bestcon;
        double bestcost = 1e18;
        for (int greedy_iter = 0; greedy_iter < MAKE_ROUTE_GREEDY_ITER; greedy_iter++) {
            //  最後だけ乱択貪欲を何回かやる
            double totalcost = 0;
            vector<bool> con;
            tie(totalcost, con) = greedy(nodes, edgmap, m);
            if (bestcost > totalcost) {
                bestcost = totalcost;
                bestcon = con;
            }
        }
        for (int eid = 0; eid < m; eid++) {
            if (bestcon[eid]) {
                auto e = edgs[eid];
                int u = e.first;
                int v = e.second;
                connection.push_back({nodes[u], nodes[v]});
            }
        }
    }
    {
        // 代表点の隙間を掘る
        for (auto con : connection) {
            point cur = con.first;
            point goal = con.second;
            vector<point> pts;
            pts.push_back(cur);
            while (true) {
                vector<point> nxts;
                if (cur.i > goal.i) {
                    nxts.push_back({cur.i - 1, cur.j});
                }
                if (cur.i < goal.i) {
                    nxts.push_back({cur.i + 1, cur.j});
                }
                if (cur.j > goal.j) {
                    nxts.push_back({cur.i, cur.j - 1});
                }
                if (cur.j < goal.j) {
                    nxts.push_back({cur.i, cur.j + 1});
                }

                // 斜めのときは縦横交互
                point nxt = pts.size() % 2 == 0 ? nxts.front() : nxts.back();

                pts.push_back(nxt);
                if (nxt == goal) {
                    break;
                }
                cur = nxt;
            }
            for (auto pt : pts) {
                must_open_mid(pt, history);
            }
        }
    }
}
}  // namespace solver

int main() {
    marathon::marathon_init();
    int n_, w_, k_;
    cin >> n_ >> w_ >> k_ >> C;
    if (LOCAL) {
        local::local_init();
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cin >> local::init_sturdiness[i][j];
            }
        }
        local::sturdiness = local::init_sturdiness;
    }
    for (int i = 0; i < w_; i++) {
        int a, b;
        cin >> a >> b;
        Waters.push_back({a, b});
        HasWater[a][b] = true;
    }
    for (int i = 0; i < k_; i++) {
        int a, b;
        cin >> a >> b;
        Houses.push_back({a, b});
        HasHouse[a][b] = true;
    }
    setparam(C, w_, k_);

    dp::init();
    solver::solve();
    return 0;
}
