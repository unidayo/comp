#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>
#include <bitset>
#include <functional>
#include <tuple>

#include <ctime>
#include <random>
#include <cassert>
#include <set>
using namespace std;
#define ALL(t) (t).begin(),(t).end()

#ifdef DEB
#define dump(x)  cerr << #x << " = " << (x) << endl
#define prl cerr<<"called:"<< __LINE__<<endl
template<class T> void debug(T a,T b){ for(;a!=b;++a) cerr<<*a<<' ';cerr<<endl;}

#else
template<class T> void debug(T a,T b){ }
#define dump(x)  
#define prl 
#endif
template<class T> bool chmin(T& a,const T& b) { if(a>b) {a=b; return true;} return false;}
template<class T> bool chmax(T& a,const T& b) { if(a<b) {a=b; return true;} return false; }
std::ostream& operator << ( std::ostream& out, 
                const std::pair< size_t, size_t >& rhs )
{
    out << "(" << rhs.first << ", " << rhs.second << ")";
    return out;
}

// block free space
using pii = pair<int, int>;
using ll = long long;
int T, K, H, W, i0;
vector<int> S, D;
vector<vector<bool>> h, v;
vector<vector<vector<pii> > > adj;
int adjG[20][20];
int dx[]={0, 1, 0, -1}, dy[]={1, 0, -1, 0};
std::random_device seed_gen;
std::mt19937 engine(0);
std::uniform_int_distribution<> randint(0, 1000000000-1);
std::uniform_real_distribution<> randreal(0.0, 1.0);

// void assert(bool a) {
//     assert(a);
// }

using BS = bitset<101>;
BS low64, high64;
BS lowmask[101];
int MSB(const bitset<101>& x) {
    // returns most significant bit position starting from 0
    if (!x.any()) return 0;
    assert(x.any());
    if ((x&high64).any()) {
        unsigned long long y = (x>>64).to_ullong();
        return (63 - __builtin_clzll(y)) + 64;
    } else {
        return 63 - __builtin_clzll(x.to_ullong());
    }
}

int MLB(const BS& x) {
    // returns the lowest bit position starting from 0
    assert(x.any());
    if ((x & low64).any()) {
        return __builtin_ctzll((x&low64).to_ullong());
    } else {
        return __builtin_ctzll((x>>64).to_ullong()) + 64;
    }
}

void read_input() {
    cin >> T >> H >> W >> i0;
    h.resize(H - 1, vector(W, false));
    for (int i = 0; i < H - 1; ++i) {
        string s; cin >> s;
        for (int j = 0; j < W; ++j) if (s[j] == '1') h[i][j] = true;
    }
    v.resize(H, vector(W - 1, false));
    for (int i = 0; i < H; ++i) {
        string s; cin >> s;
        for (int j = 0; j < W - 1; ++j) if (s[j] == '1') v[i][j] = true;
    }
    cin >> K;
    S.resize(K);
    D.resize(K);
    for (int i = 0; i < K; ++i) cin >> S[i] >> D[i];
}
const double EPS = 1e-4;
struct Node {
    int i, j;
    vector<bool> pref;
    vector<pii> chs;
    pii par;
    int depth;
    int subt, rem_subt;
    bool constrain, constrain_ch;
};
vector<vector<Node> > graph;
vector<pii> calc_leaf() {
    vector covered_cnt(H, vector<int>(W));
    vector visited(H, vector<bool>(W));
    graph.resize(H, vector<Node>(W));
    struct Cell {
        int y, x;
        double cover_score;
        int cover_cells;
        int deg;
        int wall_score;
        int depth;
        int dir;
        bool same_dir;
        int n_turns;
        bool operator < (const Cell& cell) const {
            // true if self is worse
            if (deg != cell.deg) {
                return deg < cell.deg;
            }
            // if (-depth + wall_score != -cell.depth + cell.wall_score) {
            //     return -depth + wall_score < -cell.depth + cell.wall_score;
            // }

            if (n_turns != cell.n_turns) {
                return n_turns > cell.n_turns;
            }
            // if (abs(cover_score - cell.cover_score) > EPS)
            //     return cover_score < cell.cover_score;
            if (depth != cell.depth) {
                return depth > cell.depth;
            }
            if (cover_cells != cell.cover_cells) {
                return cover_cells < cell.cover_cells;
            }

            // if (wall_score != cell.wall_score) {
            //     return wall_score < cell.wall_score;
            // }

            if (same_dir != cell.same_dir) {
                return !same_dir;
            }

            return false;
        }
    };
    auto calc_cover_score=[&](int y, int x) {
        double cover_score = 0;
        for (auto& p: adj[y][x]) {
            auto [py, px] = p;
            if (visited[py][px]) continue;
            cover_score += pow(0.1, covered_cnt[py][px]);
        }
        return cover_score;
    };
    auto calc_cover_cell=[&](int y, int x) {
        int cover_cell = 0;
        for (auto& p: adj[y][x]) {
            auto [py, px] = p;
            if (covered_cnt[py][px] > 0) continue;
            ++cover_cell;
        }
        return cover_cell;
    };
    auto calc_deg=[&](int y, int x) {
        int score = adj[y][x].size();
        return score;
    };
    auto calc_wall_score=[&](int y, int x) {
        int dx[]={0, 1, 0, -1}, dy[]={1, 0, -1, 0};
        int res = 0;
        for (int d = 0; d < 4; ++d) {
            int dist = 0;
            int cy = y, cx = x;
            auto is_adj=[&]() {
                for (auto& p: adj[cy][cx]) {
                    auto [py, px] = p;
                    if (py == cy + dy[d] && px == cx + dx[d]) {
                        return true;
                    }
                }
                return false;
            };
            while (is_adj()) {
                cx += dx[d];
                cy += dy[d];
                ++dist;
            }
            int score[8]={-10, 6, -1, -1, 1, 0, 0, 0};
            if (dist < 8) res += score[dist];
        }
        return res;
    };
    auto make_cell=[&](int y, int x, int depth, int dir, bool same_dir, int n_turns) {
        return Cell{y, x, calc_cover_score(y, x), calc_cover_cell(y, x), calc_deg(y, x), calc_wall_score(y, x), depth, dir, same_dir, n_turns};
    };
    auto get_dir=[&](int dy, int dx) {
        if (dx == 1) return 0;
        if (dy == -1) return 1;
        if (dx == -1) return 2;
        return 3;
    };

    priority_queue<Cell> pq;
    pq.push(make_cell(i0, 0, 0, 0, false, 0));
    covered_cnt[i0][0] = true;
    graph[i0][0].par = {-1, -1};

    vector<pii> leaf;
    // re-think tree generation algorithm
    // bottom-up?

    int max_depth = 0;
    while (!pq.empty()) {
        Cell cell = pq.top();
        pq.pop();
        int y = cell.y, x = cell.x;
        bool debug = false;
        if (debug) {
            fprintf(stderr, "%d %d %f %d %d\n", y, x, cell.cover_score, cell.n_turns, cell.deg);
        }

        if (  abs(cell.cover_score - calc_cover_score(cell.y, cell.x)) > EPS || 
            cell.cover_cells != calc_cover_cell(cell.y, cell.x)) {
            pq.push(make_cell(cell.y, cell.x, cell.depth, cell.dir, cell.same_dir, cell.n_turns));
            continue;
        }
        chmax(max_depth, cell.depth);
        assert(!visited[y][x]);
        visited[y][x] = true;

        bool isleaf = true;
        for (auto& p: adj[y][x]) {
            auto [py, px] = p;
            int d = get_dir(py-y, px-x);
            if (covered_cnt[py][px] == 0) {
                isleaf = false;
                graph[y][x].chs.push_back({py, px});

                graph[py][px].depth = graph[y][x].depth + 1;
                graph[py][px].par = {y, x};
                pq.push(make_cell(py, px, cell.depth + 1, d, cell.dir != d, cell.n_turns + (cell.dir != d)));
            }
            covered_cnt[py][px]++;
        }
        if (isleaf) {
            leaf.emplace_back(y, x);
        }
    }
    fprintf(stderr, "max depth = %d\n", max_depth);
    // Create simple model to predict the score and
    // add pertubation to the score;
    // get the best tree among searched tree
    return leaf;
}

void print_grid(vector<pii> leaf) {
    vector grid(H, string(W, '.'));
    for (auto l: leaf) {
        grid[l.first][l.second] = 'L';
    }
    for (int i = 0; i < H; ++i) cerr<<grid[i]<<endl;
    fprintf(stderr, "leaf: %d, non-leaf: %d\n", (int)leaf.size(), H*W-(int)leaf.size());
}

int get_subtsize(int y, int x) {
    int maxch = 0;
    graph[y][x].subt = 1;
    for (auto ch: graph[y][x].chs) {
        auto [py, px] = ch;
        int tmp = get_subtsize(py, px);
        maxch = max(maxch, tmp);
        graph[y][x].subt += tmp;
    }
    const int TREE_SIZE_THRES = 20;
    if (maxch < TREE_SIZE_THRES && graph[y][x].subt >= TREE_SIZE_THRES) {
        graph[y][x].constrain = true;
        // move constrained node and YAMANOBORI
    }
    return graph[y][x].subt;
}


void assign_pref() {
    get_subtsize(i0, 0);
    for (int i = 0; i < H; ++i) for (int j = 0; j < W; ++j) {
        graph[i][j].rem_subt = graph[i][j].subt;
    }
}

void dfs_order(int y, int x, vector<pii>& order) {
    vector<pii> chs = graph[y][x].chs;
    sort(ALL(chs), [&](pii a, pii b) {
        return graph[a.first][a.second].subt > graph[b.first][b.second].subt;
    });
    for (auto ch: chs) {
        dfs_order(ch.first, ch.second, order);
    }
    order.push_back({y, x});
}

namespace arti {
    int low[20][20], ord[20][20];
    int used[20];
    int cnt;
    // TODO: use linked list not vector
    // TODO: use array instead of vector
    void init(vector<int>& is_arti) {
        cnt = 0;
        for (int i = 0; i < H; ++i) is_arti[i] = used[i] = 0;
    }
    void dfs(int y, int x, int py, int px, vector<int>& grid, vector<int>& is_arti) {
        used[y] |= (1<<x);
        ord[y][x] = cnt++;
        low[y][x] = ord[y][x];
        for (int d = 0; d < 4; ++d) {
            if (!(adjG[y][x]>>d&1)) continue;
            int cy = y + dy[d], cx = x+dx[d];
            if (!(grid[cy]>>cx&1)) continue;
            if (!(used[cy]>>cx&1)) {
                dfs(cy, cx, y, x, grid, is_arti);
                chmin(low[y][x], low[cy][cx]);
                if (ord[y][x] <= low[cy][cx]) is_arti[y] |= (1<<x);
            } else if (cy!=py || cx!=px) {
                chmin(low[y][x], ord[cy][cx]);
            }
        }
    }
}

void calc_is_arti(vector<int> &grid, vector<int>& is_arti) {
    arti::init(is_arti);
    if (grid[i0]&1) arti::dfs(i0, 0, -1, -1, grid, is_arti);
}

pii get_removable(vector<int> &grid, vector<int>& is_arti, int& is_latest) {
    if (!is_latest) {
        is_latest = 1;
        calc_is_arti(grid, is_arti);
    }

    int pcnt = 0;
    for (int i = 0; i < H; ++i) {
        pcnt += __builtin_popcount((~is_arti[i]) & grid[i]);
    }
    if (pcnt == 0) return {-1, -1};
    int r = randint(engine) % pcnt;
    for (int i = 0; i < H; ++i) {
        int bit = ((~is_arti[i]) & grid[i]);
        int p = __builtin_popcount(bit);
        if (r - p < 0) {
            for (int j = 0; j < W; ++j) if (bit>>j&1) {
                if (r == 0) return {i, j};
                --r;
            }
        } else {
            r -= p;
        }
    }
    assert(false);
}

pii get_append(vector<int> &grid, vector<int>& adjgrid) {
    // fix the case when no cell is on the grid
    vector<pii> cands;
    int pcnt = 0;
    for (int i = 0; i < H; ++i) {
        int bit = (~grid[i]) & adjgrid[i];
        pcnt += __builtin_popcount(bit);
    }
    if (pcnt == 0) return {-1, -1};
    int r = randint(engine) % pcnt;
    for (int i = 0; i < H; ++i) {
        int bit = (~grid[i]) & adjgrid[i];
        int p = __builtin_popcount(bit);
        if (r - p < 0) {
            for (int j = 0; j < W; ++j) if (bit>>j&1) {
                if (r == 0) return {i, j};
                --r;
            }
        } else {
            r -= p;
        }
    }
    assert(false);
}


void plan(auto initial_clock, int MAXT) {
    int upper_limit = 0;

    struct Edge{
        int to, id;
    };

    vector<int> S_total(T+1), D_total(T+1), len_total(T+1);
    vector<int> D_counter(T+1);
    vector<int> D_value(T+1);
    vector surplus(T+1, vector<int>(T+1));
    vector deficit(T+1, vector<int>(T+1));

    vector DS_total(T+1, vector<int>(T+1));
    for (int i = 0; i < K; ++i) {
        int s = S[i] - 1, d = D[i];
        S_total[s]++;
        D_total[d]++;
        ++DS_total[d][s];
        len_total[d-s]++;
        upper_limit += d - s;
    }

    static vector division(H, vector<bitset<101> >(W));
    vector block_cnt(T+1, vector<int>(T+1));//[D][S]

    auto refresh=[&](int d) {
        int b_sum = 0;
        int counter = 0;
        for (int s = 0; s < d; ++s) {
            b_sum += block_cnt[d][s];
            int new_using = min(b_sum, DS_total[d][s]);
            b_sum -= new_using;
            counter += new_using * (d-s);
            surplus[d][d-s] = b_sum;
            deficit[d][d-s] = DS_total[d][s] - new_using;
        }
        int dcount = counter - D_counter[d];
        D_counter[d] = counter;
        return dcount;
    };
    
    for (int i = 0; i < H; ++i) for (int j = 0; j < W; ++j) {
        division[i][j][T] = true;
        block_cnt[T][0]++;
    }
    int profit = 0;
    for (int t = 1; t <= T; ++t) profit += refresh(t);
    auto get_lr=[&](int t, int y, int x) {
        assert(1 <= t);
        assert(t < T);
        int l2 = MSB(division[y][x] & lowmask[t]), r2 = MLB(division[y][x] >> (t+1)) + (t+1);
        // int l = 0, r = T;
        // for (int i = t - 1; i > 0; --i) {
        //     if (division[y][x][i]) {
        //         l = i;
        //         break;
        //     }
        // }
    
        // for (int i = t+1; i < T; ++i) {
        //     if (division[y][x][i]) {
        //         r = i;
        //         break;
        //     }
        // }
        // assert(l==l2);
        // assert(r==r2);
        return make_pair(l2, r2);
    };
    int sum_profit = profit;
    struct Split {
        int vy, vx;
        int t;
        bool disabled;
    };
    static vector<Split> splits;
    auto oneupdate=[&](int t, int s, int sign) {
        block_cnt[t][s] += sign;
        int len = t - s;
        if (sign == 1) {
            for (int i = len; i > 0; --i){
                if (deficit[t][i] > 0) {
                    --deficit[t][i];
                    D_counter[t] += i;
                    return i;
                } else {
                    ++surplus[t][i];
                }
            }
            return 0;
        } else {
            for (int i = len; i > 0; --i) {
                if (deficit[t][i] < DS_total[t][t-i]) {
                    if (surplus[t][i] == 0) {
                        ++deficit[t][i];
                        D_counter[t] -= i;
                        return -i;
                    }
                }
                assert(surplus[t][i] > 0);
                --surplus[t][i];
            }
            return 0;
        }
        assert(false);
    };
    static auto eval_delta=[&](int y, int x, int t, int sign) {
        auto [l, r] = get_lr(t, y, x);
        int delta = oneupdate(t, l, sign);
        delta += oneupdate(r, t, sign);
        delta += oneupdate(r, l, -sign);
        return delta;
    };
    auto apply_split=[&](int cy, int cx, int t, int mxsize = H*W) {
        bitset<101> changed;

        int length = 0;
        while (~cy && length < mxsize) {

            if (!division[cy][cx][t]) {
                const int sign = 1;
                auto [l, r] = get_lr(t, cy, cx);
                block_cnt[t][l] += sign;
                block_cnt[r][t] += sign;
                block_cnt[r][l] -= sign;
                changed[r] = true;
                changed[t] = true;
            }
            division[cy][cx][t] = true;
            auto [ny, nx] = graph[cy][cx].par;
            cy = ny;
            cx = nx;
            ++length;
        }
        return changed;
    };

    {
        set<int> highT;

        bool update = true;
        bool isdebug;
        int lastt = T;
        int t_cnt = 0;
        const int MAX_INITIAL_TURNS = 10 * T;
        while(t_cnt <= MAX_INITIAL_TURNS) { 
            auto choose_t=[&]() {
                // if (new_division >= 90) {
                if (highT.empty()) {
                    if (!update) {
                        return -1;
                    }
                    update = false;
                    dump("renewing...");
                    for (int i = 1; i <= T; ++i) highT.insert(i);
                }
                auto it = highT.lower_bound(lastt);
                auto it2 = it;
                if (it == highT.begin()) it2 = (--highT.end());
                else --it2;
                int res = *it2;
                return res;

            };
            int t = choose_t();
            if (t == -1) break;
            lastt = t;
            dump(t);
            isdebug = (t == 99);

            int mx_profit = -1, vy, vx;

            const int INF = 1e5;
            std::function<void(int, int, int)> dfs;
            dfs=[&](int y, int x, int deltasum) {
                int delta = division[y][x][t] ? 0 : eval_delta(y, x, t, 1);
                deltasum = delta + deltasum;

                if (mx_profit < deltasum) {
                    mx_profit = deltasum;
                    vy = y;
                    vx = x;
                }

                for (auto ch: graph[y][x].chs) {
                    auto [cy, cx] = ch;

                    dfs(cy, cx, deltasum);
                }
                if (!division[y][x][t]) eval_delta(y, x, t, -1);
            };
            dfs(i0, 0, 0);
            if (mx_profit <= 0) {
                highT.erase(t);
                continue;
            }
            dump(t);
            dump(vy);dump(vx);
            dump(graph[vy][vx].depth);
            dump(mx_profit);

            update = true;

            set<pii> vs = {{vy, vx}};
            bitset<101> changed;
            for (auto v: vs) {
                auto [cy, cx] = v;
                splits.push_back(Split{cy, cx, t, false});
                changed |= apply_split(cy, cx, t);
            }

            int delta = 0;
            for (int i = 0; i <= T; ++i) if (changed[i]) {
                delta += refresh(i);
            }
            dump(delta);
            assert(mx_profit == delta);
            profit += mx_profit;
            ++t_cnt;
        }
        dump(t_cnt);
    }
    if (true) {
        int deltaprof = 0;
        int deltaprofmax = 0;
        int okcnt = 0, ngcnt = 0;
        static vector grid(T+1, vector<int>(H)), adjgrid(T+1, vector<int>(H));
        auto best_grid = grid;
        int best_renew_cnt = 0;
        static vector is_arti(T+1, vector<int>(H));
        static vector<int> arti_latest(T+1);
        // adjgrid contains grid
        static vector adjcnt(T+1, vector<vector<int> >(H, vector<int>(W)));

        static auto count_deg=[&](int y, int x, int t) {
            int deg = 0;
            for (auto& p: adj[y][x]) {
                auto [py, px] = p;
                if (grid[t][py]>>px&1) {
                    ++deg;
                }
            }
            return deg;
        };
        static auto get_adj=[&](int y, int x, int t) {
            for (auto& p: adj[y][x]) {
                auto [py, px] = p;
                if (grid[t][py]>>px&1) {
                    return make_pair(py, px);
                }
            }
            return make_pair(-1, -1);
        };
        static auto addcell=[&](int y, int x, int t) {
            grid[t][y] |= (1<<x);
            for (auto p: adj[y][x]) {
                auto [py, px] = p;
                adjgrid[t][py] |= (1<<px);
                ++adjcnt[t][py][px];
            }
        };
        for (int t = 1; t <= T; ++t) {
            adjcnt[t][i0][0] = 1;
            adjgrid[t][i0] |= 1;
        }
        for (int i = 0; i <= T; ++i) {
            for (int j = 0; j < H; ++j) for (int k = 0; k < W; ++k) {
                if (division[j][k][i]) {
                    grid[i][j] |= (1 << k);
                    addcell(j, k, i);
                }
            }
        }
        static int degcnt_add[5]={}, degcnt_rem[5]={};
        for (int tt = 0; ; ++tt) {
            struct Action {
                int type;
                int t, y, x;
                int calc_effect() {
                    if (type == 0) {
                        return eval_delta(y, x, t, -1);
                    } else if (type == 1) {
                        return eval_delta(y, x, t, 1);
                    } else {
                        assert(false);
                    }
                }
                void rollback() {
                    if (type == 0) {
                        eval_delta(y, x, t, 1);
                    } else if (type == 1) {
                        eval_delta(y, x, t, -1);
                    } else {
                        assert(false);
                    }
                }
                void adopt() {
                    if (type == 0) {
                        arti_latest[t] = 0;
                        division[y][x][t] = false;
                        grid[t][y] &= ~(1<<x);
                        for (auto p: adj[y][x]) {
                            auto [py, px] = p;
                            --adjcnt[t][py][px];
                            if (adjcnt[t][py][px] == 0)
                                adjgrid[t][py] &= ~(1<<px);
                        }
                    } else if (type == 1) { // add
                        division[y][x][t] = true;
                        addcell(y, x, t);

                        if (adjcnt[t][y][x] == 1) {
                            auto [py, px] = get_adj(y, x, t);
                            if (py == -1) {
                                assert(y == i0 && x == 0);
                            } else {
                                is_arti[t][py] |= (1<<px);
                            }
                        } else {
                            arti_latest[t] = 0;
                        }
                    } else {
                        assert(false);
                    }
                }
            };
            Action action;
            double rest_turn = (MAXT - tt) * 1.0 / MAXT;
            if (randint(engine) % 2 == 0) {
                // removal
                auto getTYX=[&]() {
                    while (true) {
                        int t = randint(engine) % (T-1) + 1;

                        auto [y, x] = get_removable(grid[t], is_arti[t], arti_latest[t]);
                        if (y == -1) continue;
                        return make_tuple(t, y, x);
                    }
                };
                auto [t, y, x] = getTYX();
                action = Action{0, t, y, x};
            } else {
                auto getTYX=[&]() {
                    while (true) {
                        int t = randint(engine) % (T-1) + 1;

                        auto [y, x] = get_append(grid[t], adjgrid[t]);
                        if (y == -1) continue;
                        return make_tuple(t, y, x);
                    }
                };
                auto [t, y, x] = getTYX();
                action = Action{1, t, y, x};
            }
            auto delta = action.calc_effect();
            const double TEMPMAX = 2.0, TEMPMIN=0.0;
            const double temperature = max(0.0, rest_turn * (TEMPMAX-TEMPMIN) + TEMPMIN);
            // dump(tt);dump(action.type);fprintf(stderr, "delta.dcount = %d\n", delta);dump(deltaprof);
            if (delta >= 0 || exp(delta  / temperature) > randreal(engine)) {
                action.adopt();
                deltaprof += delta;
                ++okcnt;
            } else {
                ++ngcnt;
                action.rollback();
            }
            if (chmax(deltaprofmax, deltaprof)) {
                ++best_renew_cnt;
                best_grid = grid;
            }
            if (tt % 10000 == 999) {
                dump(okcnt);
                dump(ngcnt);
                okcnt = ngcnt = 0;
                auto now = clock();
                double time_passed = (now - initial_clock) / (double) CLOCKS_PER_SEC;
                dump(time_passed);
                dump(tt);
                dump(deltaprof);
                dump(deltaprofmax);
                if (time_passed >= 1.95) {
                    break;
                }
                // if (tt >= MAXT) {
                //     break;
                // }
            }
        }
        dump(best_renew_cnt);
        dump(deltaprof);dump(deltaprofmax);
        profit += deltaprofmax;
        for (int t = 0; t <= T; ++t) for (int i = 0; i < H; ++i) for (int j = 0; j < W; ++j) {
            division[i][j][t] = (best_grid[t][i]>>j&1);
        }
    }

    struct Plan {
        int y, x;
        int id;
        int start;
    };

    vector blocks(T+1, vector<vector<pii> >(T+1));
    vector<Plan> plans;

    for (int i = 0; i < H; ++i) for (int j = 0; j < W; ++j) {
        int s = 0;
        for (int t = 1; t <= T; ++t) {
            if (division[i][j][t]){
                blocks[s][t].push_back({i, j});
                s = t;
            }
        }
    }

    vector plants_SD(T+1, vector<vector<int> >(T+1));
    for (int i = 0; i < K; ++i) {
        int s = S[i]-1, d = D[i];
        plants_SD[s][d].push_back(i);
    }
    cerr << "original: ";
    cerr << "S = ";
    debug(ALL(S_total));
    cerr << "D = ";
    debug(ALL(D_total));
    cerr << "len = ";
    debug(ALL(len_total));

    for (int d = 0; d <= T; ++d) {
        vector<tuple<int, int, int> > available;
        for (int s = 0; s < d; ++s) {
            for (auto e: blocks[s][d]) {
                auto [y, x] = e;
                available.push_back({y, x, s});
            }
            while (!plants_SD[s][d].empty() && !available.empty()) {
                auto [y, x, start] = available.back();
                int id = plants_SD[s][d].back();
                available.pop_back();
                plants_SD[s][d].pop_back();
                --S_total[s];
                --D_total[d];
                --len_total[d-s];
                plans.push_back({y, x, id, start});
            }
        }
    }

    cout << plans.size() << endl;
    vector<bool> used_plant(K);
    for (auto e: plans) {
        used_plant[e.id] = true;
        printf("%d %d %d %d\n", e.id+1, e.y, e.x, e.start + 1);
    }
    dump(profit);

    cerr << "S = ";
    debug(ALL(S_total));
    cerr << "D = ";
    debug(ALL(D_total));
    cerr << "len = ";
    debug(ALL(len_total));

    int K2 = K - plans.size();
    cerr << K2 << endl;
    for (int i = 0; i < K; ++i) if(!used_plant[i]){
        fprintf(stderr, "%d %d\n", S[i], D[i]);
    }
}
int main(int argc, char *argv[]) {
    for (int i = 0; i < 64; ++i) {
        low64[i] = true;
        if (i + 64 <= 100) high64[i+64] = true;
    }
    for (int i = 0; i <= 100; ++i) {
        for (int j = 0; j < i; ++j) lowmask[i][j] = true;
    }

    BS x;
    x[30] = true;
    dump(MSB(x));
    dump(MLB(x));
    auto initial_clock = clock();
    read_input();

    // construct a graph
    adj.resize(H, vector(W, vector<pair<int, int>>()));
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            if (i + 1 < H && !h[i][j]) {
                // no waterway between (i, j) and (i + 1, j)
                adj[i][j].emplace_back(i + 1, j);
                adj[i + 1][j].emplace_back(i, j);
            }
            if (j + 1 < W && !v[i][j]) {
                // no waterway between (i, j) and (i, j + 1)
                adj[i][j].emplace_back(i, j + 1);
                adj[i][j + 1].emplace_back(i, j);
            }
        }
    }
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            for (int d = 0; d < 4; ++d) {
                for (auto nxt: adj[i][j]) {
                    auto [py, px] = nxt;
                    if (py == i + dy[d] && px == j + dx[d]) {
                        adjG[i][j] |= (1<<d);
                    }
                }
            }
        }
    }
    int MAXT = argc >= 2 ? atoi(argv[1]) : 3500000;
    auto leaf = calc_leaf();
    print_grid(leaf);
    assign_pref();
    plan(initial_clock, MAXT);
    return 0;
}
