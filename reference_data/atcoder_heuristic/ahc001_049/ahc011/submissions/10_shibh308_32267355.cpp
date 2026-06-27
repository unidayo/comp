#ifndef HAND
#pragma GCC target("avx512f")
#pragma GCC optimize("O2")
#pragma GCC optimize("unroll-loops")
#endif

#include <bits/stdc++.h>
// #include "atcoder/all"

using namespace std;

using i64 = long long;

const i64 MOD = 1e9 + 7;
const i64 INF = i64(1e18);

template <typename T>
bool chmin(T& x, T y){
    if(x > y){
        x = y;
        return true;
    }
    return false;
}

template <typename T>
bool chmax(T& x, T y){
    if(x < y){
        x = y;
        return true;
    }
    return false;
}

int case_num = 0;

uint32_t rnd(){
    static uint32_t x = 123456789, y = 362436069, z = 521288629, w = 0;
    uint32_t t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    return w;
}

double rnd_double(){
    return 1.0 * rnd() / numeric_limits<uint32_t>::max();
}

// sq_border#define OPTIMIZE

namespace params{

int DEFAULT_WIDTH_[5] = {1000, 1000, 500, 100, 100};
// double assignment_time_border[5] = {1.38, 0.85, 0.75, 0.10, 0.22};
// int splits[5] = {3, 4, 2, 1, 1};
double assignment_time_border[5] = {1.2, 1.0, 0.5, 0.5, 0.3};
int splits[5] = {3, 1, 1, 1, 1};
double man_sum_coef = 0.90;
double man_sum_pow_coef = 0.83;
double linear_conf_coef = 9.00;
double pos_dist_coef = 0.0915;
double conn_cnt_coef[5] = {10.5, 10.5, 10.5, 10.5, 14.2};
double conn_cnt_pow[5] = {2.0, 2.0, 2.0, 1.67, 2.2};
double first_dist_1 = 1.0;
double first_dist_2 = 1.32;

int back_100 = 100;
int width_100 = 100;
int cn_10 = 10;
int kk_10 = 10;
int width2_1000 = 1000;



#ifdef OPTIMIZE
double TIME_BORDER = 3.80;
#elif HAND
double TIME_BORDER = 6.80;
#else
double TIME_BORDER = 2.80;
#endif

void load_params(){
    cin >> back_100 >> width_100 >> cn_10 >> kk_10 >> width2_1000;
    for(int i = 0; i < 5; ++i){
        // assignment_time_border[i] = a;
        // splits[i] = b;
        assignment_time_border[i] *= TIME_BORDER / 2.80;
    }
}

}


// doc: https://shibh308.github.io/library/library/lib/classes/hashmap.cpp.html
template <typename T, typename U, T del = numeric_limits<T>::max(), T null = numeric_limits<T>::max() - 1>
struct HashMap{
    static constexpr __int128_t z = 0xf332ac987401cba5;
    uint64_t n, q, d;

    vector<pair<T, U>> v;

    HashMap() : n(0), q(0), d(1),  v(2, make_pair(null, U())){
    }

    inline uint64_t hash(T key){return uint64_t((z * __int128_t(key)) >> (64 - d)) & ((1LL << d) - 1);}

    pair<uint64_t, bool> find(T x){
        for(uint64_t i = hash(x); v[i].first != null; i = (i + 1) & ((1 << d) - 1))
            if(v[i].first == x)
                return make_pair(i, true);
        return make_pair(U(), false);
    }

    bool add(T x, U val){
        if(find(x).second)
            return false;
        if(((q + 1) << 1) > (1 << d) || (1 << d) < 3 * n)
            resize();
        uint64_t i = hash(x);
        for(; v[i].first != null && v[i].first != del; i = (i + 1) & ((1 << d) - 1));
        q += (v[i].first == null);
        ++n;
        v[i] = make_pair(x, val);
        return true;
    }

    bool erase(T x){
        uint64_t i = hash(x);
        for(; v[i].first != null && v[i].first != x; i = (i + 1) & ((1 << d) - 1));
        if(v[i].first == null)
            return false;
        --n;
        v[i] = make_pair(del, U());
        return true;
    }

    void resize(){
        ++d;
        vector<pair<T, U>> old_table;
        q = n;
        swap(old_table, v);
        v.assign(1 << d, make_pair(null, U()));
        n = 0;
        for(int i = 0; i < old_table.size(); ++i)
            if(old_table[i].first != null && old_table[i].first != del)
                add(old_table[i].first, old_table[i].second);
    }
};


// doc: https://shibh308.github.io/library/library/lib/classes/primaldual.cpp.html
template <typename T, typename U>
struct PrimalDual{
    struct Edge{
        int to, rev;
        U cap;
        T cost;
        Edge(int to, U cap, T cost, int rev) :
                to(to), rev(rev), cap(cap), cost(cost){}
    };
    vector<vector<Edge>> edges;
    T _inf;
    vector<T> potential, min_cost;
    vector<int> prev_v, prev_e;

    PrimalDual(int n) : edges(n), _inf(numeric_limits<T>::max()){}

    void add(int from, int to, U cap, T cost){
        edges[from].emplace_back(to, cap, cost, static_cast<int>(edges[to].size()));
        edges[to].emplace_back(from, 0, -cost, static_cast<int>(edges[from].size()) - 1);
    }

    T solve(int s, int t, U flow){
        int n = edges.size();
        T ret = 0;
        priority_queue<pair<T,int>, vector<pair<T,int>>, greater<pair<T,int>>> que;
        potential.assign(n, 0);
        prev_v.assign(n, -1);
        prev_e.assign(n, -1);
        while(flow > 1e-6){
            min_cost.assign(n, _inf);
            que.emplace(0, s);
            min_cost[s] = 0;
            while(!que.empty()){
                T fl;
                int pos;
                tie(fl, pos) = que.top();
                que.pop();
                if(min_cost[pos] != fl)
                    continue;
                for(int i = 0; i < edges[pos].size(); ++i){
                    auto& ed = edges[pos][i];
                    T nex = fl + ed.cost + potential[pos] - potential[ed.to];
                    if(ed.cap > 0 && min_cost[ed.to] > nex){
                        min_cost[ed.to] = nex;
                        prev_v[ed.to] = pos;
                        prev_e[ed.to] = i;
                        que.emplace(min_cost[ed.to], ed.to);
                    }
                }
            }
            if(min_cost[t] == _inf)
                return -1;
            for(int i = 0; i < n; ++i)
                potential[i] += min_cost[i];
            U add_flow = flow;
            for(int x = t; x != s; x = prev_v[x])
                add_flow = min(add_flow, edges[prev_v[x]][prev_e[x]].cap);
            flow -= add_flow;
            ret += add_flow * potential[t];
            for(int x = t; x != s; x = prev_v[x]){
                auto& ed = edges[prev_v[x]][prev_e[x]];
                ed.cap -= add_flow;
                edges[x][ed.rev].cap += add_flow;
            }
        }
        return ret;
    }
};


clock_t st;

int n, turn_max;
int st_x = 0, st_y = 0;
vector<string> inp;
vector<vector<int>> a;

vector<int> dx{0, -1, 0, 1};
vector<int> dy{-1, 0, 1, 0};
string dstr = "LURD";

int man_dist(int u, int v){
    return abs((u / n) - (v / n)) + abs((u % n) - (v % n));
};


vector<int> target, target_inv;
priority_queue<tuple<double, decltype(target), decltype(target_inv)>> tups;

void spanning_tree(){
    a.clear();
    a.resize(n, vector<int>(n, 0));
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            if('a' <= inp[i][j] && inp[i][j] <= 'f'){
                a[i][j] = int(inp[i][j] - 'a') + 10;
            }
            else{
                a[i][j] = int(inp[i][j] - '0');
            }
            if(a[i][j] == 0){
                st_x = i;
                st_y = j;
            }
        }
    }
    vector<vector<int>> bin(16, vector<int>());
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            bin[a[i][j]].emplace_back(i * n + j);
        }
    }

    uint8_t remain[16];
    uint8_t remain_bits = 0;
    for(int i = 0; i < 16; ++i){
        remain[i] = 0;
    }
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            ++remain[a[i][j]];
            if(a[i][j] & 8){
                ++remain_bits;
            }
        }
    }

    vector<vector<int>> ma(n, vector<int>(n, -1));
    vector<int8_t> conn(n, 0);
    for(int i = 0; i < n; ++i){
        conn[i] = -1;
    }

    auto calc = [](int nx, int ny){
        int mov_target_x = target[nx * n + ny] / n;
        int mov_target_y = target[nx * n + ny] % n;
        int lc = 0;
        if(nx == mov_target_x){
            for(int i = 0; i < n; ++i){
                if(i == ny){
                    continue;
                }
                int np = nx * n + i;
                int np_tile = np;
                if(np_tile == st_x * n + st_y){
                    continue;
                }
                int np_target = target[np_tile];
                int np_x = np_target / n;
                int np_y = np_target % n;
                if(nx == np_x && ((ny < i) != (mov_target_y < np_y))){
                    // printf("(%d,%d)[%d,%d] - (%d,%d)[%d,%d]\n", nx, ny, mov_target_x, mov_target_y, nx, i, np_x, np_y);
                    lc++;
                }
            }
        }
        if(ny == mov_target_y){
            for(int i = 0; i < n; ++i){
                if(i == nx){
                    continue;
                }
                int np = i * n + ny;
                int np_tile = np;
                if(np_tile == st_x * n + st_y){
                    continue;
                }
                int np_target = target[np_tile];
                int np_x = np_target / n;
                int np_y = np_target % n;
                if(ny == np_y && ((nx < i) != (mov_target_x < np_x))){
                    // printf("(%d,%d)[%d,%d] - (%d,%d)[%d,%d]\n", nx, ny, mov_target_x, mov_target_y, i, ny, np_x, np_y);
                    lc++;
                }
            }
        }
        return lc;
    };

    tups.emplace(INF, decltype(target)(), decltype(target_inv)());
    auto cand_check = [&](){
        target.assign(n * n, -1);
        target_inv.assign(n * n, -1);
        vector<int> bin_cn(16, 0);
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < n; ++j){
                target[bin[ma[i][j]][bin_cn[ma[i][j]]++]] = i * n + j;
            }
        }
        for(int i = 0; i < n * n; ++i){
            target_inv[target[i]] = i;
        }
        int par = 0;
        for(int i = 0; i < n * n; ++i){
            ma[i / n][i % n] = a[target_inv[i] / n][target_inv[i] % n];
        }
        auto tt = target;
        for(int ii = 0; ii < n * n; ++ii){
            if(tt[ii] == ii){
                continue;
            }
            for(int jj = 0; jj < n * n; ++jj){
                if(tt[jj] == ii){
                    ++par;
                    swap(tt[jj], tt[ii]);
                    break;
                }
            }
        }
        if(par % 2 != int(man_dist(st_x * n + st_y, target[st_x * n + st_y])) % 2){
            bool fl = false;
            for(int pp = 1; pp < 16; ++pp){
                for(int ii = 0; ii < bin[pp].size(); ++ii){
                    for(int jj = ii + 1; jj < bin[pp].size(); ++jj){
                        swap(target_inv[target[bin[pp][ii]]], target_inv[target[bin[pp][jj]]]);
                        swap(target[bin[pp][ii]], target[bin[pp][jj]]);
                        fl = true;
                        break;
                    }
                    if(fl){
                        break;
                    }
                }
                if(fl){
                    break;
                }
            }
        }

        auto dist_func = [&](int i, int j){
            if(i == st_x * n + st_y){
                return 0.0;
            }
            int di = man_dist(i, target[j]);
            return
                    + params::first_dist_2 * di * di
                    + params::first_dist_1 * di;
        };

        auto clc = [&](){
            int par = 0;
            auto tt = target;
            for(int ii = 0; ii < n * n; ++ii){
                if(tt[ii] == ii){
                    continue;
                }
                for(int jj = 0; jj < n * n; ++jj){
                    if(tt[jj] == ii){
                        ++par;
                        swap(tt[jj], tt[ii]);
                        break;
                    }
                }
            }
            double sc = 0;
            int md = 0;
            for(int i = 0; i < n * n; ++i){
                md += man_dist(i, target[i]);
                sc += dist_func(i, i);
            }
            for(int i = 0; i < n * n; ++i){
                if(i != st_x * n + st_y){
                    sc += params::linear_conf_coef * calc(i / n, i % n);
                }
            }
            return make_pair((par % 2 == int(man_dist(st_x * n + st_y, target[st_x * n + st_y])) % 2), sc);
        };

        bool first_res = clc().first;

        PrimalDual<int, double> pr(2 * n * n + 2);
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < n; ++j){
                pr.add(2 * n * n, i * n + j, 1, 0);
                pr.add(n * n + i * n + j, 2 * n * n + 1, 1, 0);
            }
        }
        for(int p = 0; p < 16; ++p){
            for(int ii = 0; ii < bin[p].size(); ++ii){
                int i = bin[p][ii];
                for(int jj = 0; jj < bin[p].size(); ++jj){
                    int j = bin[p][jj];
                    double dist = dist_func(i, j);
                    pr.add(i, n * n + j, 1, dist);
                }
            }
        }
        auto solve_res = pr.solve(2 * n * n, 2 * n * n + 1, n * n);
        // cout << "solve: " << solve_res << endl;

        auto bef_target = target;

        for(int i = 0; i < n * n; ++i){
            for(auto e : pr.edges[i]){
                if(n * n <= e.to && e.to < 2 * n * n){
                    if(e.cap == 0){
                        // cout << "SELECT: " << i << " => " << bef_target[e.to - n * n] << endl;
                        target[i] = bef_target[e.to - n * n];
                        target_inv[target[i]] = i;
                    }
                }
            }
        }
        bool mid_res = clc().first;
        if(!mid_res){
            tuple<double, int, int> mi(MOD, -1, -1);
            for(int p = 1; p < 16; ++p){
                for(int ii = 0; ii < bin[p].size(); ++ii){
                    int i = bin[p][ii];
                    for(int jj = ii + 1; jj < bin[p].size(); ++jj){
                        int j = bin[p][jj];
                        double bef_dist = dist_func(i, i) + dist_func(j, j);
                        double aft_dist = dist_func(i, j) + dist_func(j, i);
                        chmin(mi, make_tuple(aft_dist - bef_dist, i, j));
                    }
                }
            }
            auto[r, i, j] = mi;
            swap(target[i], target[j]);
            swap(target_inv[target[i]], target_inv[target[j]]);
        }
        auto [last_res, score] = clc();
        tups.emplace(make_tuple(score, target, target_inv));
        while(tups.size() >= 10){
            tups.pop();
        }
    };

    int cn = 0;
    if(n != 6){
        function<int(int, int, bool)> f = [&](int ij, int down_mask, bool right_mask){
            if(tups.size() != 1 && ++cn % 10000 == 0 &&
               double(clock() - st) / CLOCKS_PER_SEC > params::assignment_time_border[n - 6]){
                return -1;
            }
            if(ij == n * n){
                cand_check();
                if(double(clock() - st) / CLOCKS_PER_SEC > params::assignment_time_border[n - 6]){
                    return -1;
                }
                return 1;
            }
            int i = ij / n;
            int j = ij % n;
            if(remain_bits > max(0, n * (n - 1) - ij)){
                return 0;
            }

            auto bef_conn = conn;

            bool left_mask = right_mask;
            bool up_mask = down_mask & (1 << j);
            int mo = left_mask + (int(up_mask) << 1);
            vector<int> v = {0, 1, 2, 3};
            // random_shuffle(v.begin(), v.end());
            sort(v.begin(), v.end(), [&](auto a, auto b){
                return remain[(a << 2) + mo] > remain[(b << 2) + mo];
            });
            bool res_fl = false;
            for(auto pp : v){
                // {
                int p = (pp << 2) + mo;
                // for(int p = mo; p < 16; p += 4){
                if(remain[p] == 0){
                    continue;
                }
                if(((p & 8) && i == n - 1) || ((p & 4) && j == n - 1)){
                    continue;
                }
                ma[i][j] = p;
                int n_down_mask = down_mask;
                int n_right_mask = (p & 4);

                conn = bef_conn;
                int up_conn = conn[j];
                int left_conn = j == 0 ? -1 : conn[j - 1];
                if(up_mask && left_mask){
                    if(up_conn == left_conn){
                        continue;
                    }
                    for(int k = 0; k < n; ++k){
                        if(conn[k] == left_conn){
                            conn[k] = up_conn;
                        }
                    }
                }
                /*
                if(up_mask && !left_mask){
                    conn[j] = up_conn;
                }
                 */
                else if(!up_mask && left_mask){
                    conn[j] = left_conn;
                }
                else if(!up_mask && !left_mask){
                    if(p == 0){
                        conn[j] = -1;
                    }else{
                        int bi = 0;
                        for(int k = 0; k < n; ++k){
                            if(conn[k] != -1 && j != k){
                                bi |= (1 << conn[k]);
                            }
                        }
                        conn[j] = __builtin_ffs(~bi) - 1;
                    }
                }
                bool fl = (p == 0) || (p & 12) || (i * n + j + remain[0] == n * n - 1);
                for(int k = 0; k < n && !fl; ++k){
                    if(conn[k] == conn[j]){
                        int ii = (k <= j ? i : i - 1);
                        int q = ma[ii][k];
                        fl |= (q & 8);
                    }
                }
                if(!fl){
                    continue;
                }
                --remain[p];
                if(p & 8){
                    --remain_bits;
                }
                n_down_mask &= ~(1 << j);
                if(p & 8){
                    n_down_mask |= (1 << j);
                }
                int res = f(ij + 1, n_down_mask, n_right_mask);
                res_fl |= res;
                ++remain[p];
                if(p & 8){
                    ++remain_bits;
                }
                if(res == -1){
                    return -1;
                }
                if(res == 1 && (ij >= n)){
                    return 1;
                }
            }
            ma[i][j] = -1;
            return int(res_fl);
        };
        f(0, 0, 0);
    }
    else{
        int ty = 0;
        function<int(int,int,bool)> f = [&](int ij, int down_mask, bool right_mask){
            if(tups.size() != 1 && ++cn % 10000 == 0 && double(clock() - st) / CLOCKS_PER_SEC > params::assignment_time_border[n - 6]){
                return -1;
            }
            if(ij == n * n){
                cand_check();
                if(double(clock() - st) / CLOCKS_PER_SEC > params::assignment_time_border[n - 6]){
                    return -1;
                }
                return 1;
            }
            int i = ij / n;
            int j = ij % n;
            if(remain_bits > max(0, n * (n - 1) - ij)){
                return 0;
            }

            auto bef_conn = conn;

            bool left_mask = right_mask;
            bool up_mask = down_mask & (1 << j);
            int mo = left_mask + (int(up_mask) << 1);
            vector<int> v = {0, 1, 2, 3};
            if(ty){
                random_shuffle(v.begin(), v.end());
            }
            else{
                sort(v.begin(), v.end(), [&](auto a, auto b){
                    return remain[(a << 2) + mo] > remain[(b << 2) + mo];
                });
            }
            bool res_fl = false;
            for(auto pp : v){
                // {
                int p = (pp << 2) + mo;
                // for(int p = mo; p < 16; p += 4){
                if(remain[p] == 0){
                    continue;
                }
                if(((p & 8) && i == n - 1) || ((p & 4) && j == n - 1)){
                    continue;
                }
                ma[i][j] = p;
                int n_down_mask = down_mask;
                int n_right_mask = (p & 4);

                conn = bef_conn;
                int up_conn = conn[j];
                int left_conn = j == 0 ? -1 : conn[j - 1];
                if(up_mask && left_mask){
                    if(up_conn == left_conn){
                        continue;
                    }
                    for(int k = 0; k < n; ++k){
                        if(conn[k] == left_conn){
                            conn[k] = up_conn;
                        }
                    }
                }
                /*
                if(up_mask && !left_mask){
                    conn[j] = up_conn;
                }
                 */
                else if(!up_mask && left_mask){
                    conn[j] = left_conn;
                }
                else if(!up_mask && !left_mask){
                    if(p == 0){
                        conn[j] = -1;
                    }
                    else{
                        int bi = 0;
                        for(int k = 0; k < n; ++k){
                            if(conn[k] != -1 && j != k){
                                bi |= (1 << conn[k]);
                            }
                        }
                        conn[j] = __builtin_ffs(~bi) - 1;
                    }
                }
                bool fl = (p == 0) || (p & 12) || (i * n + j + remain[0] == n * n - 1);
                for(int k = 0; k < n && !fl; ++k){
                    if(conn[k] == conn[j]){
                        int ii = (k <= j ? i : i - 1);
                        int q = ma[ii][k];
                        fl |= (q & 8);
                    }
                }
                if(!fl){
                    continue;
                }
                --remain[p];
                if(p & 8){
                    --remain_bits;
                }
                n_down_mask &= ~(1 << j);
                if(p & 8){
                    n_down_mask |= (1 << j);
                }
                int res = f(ij + 1, n_down_mask, n_right_mask);
                res_fl |= res;
                ++remain[p];
                if(p & 8){
                    ++remain_bits;
                }
                if(res == -1){
                    return -1;
                }
                if(res == 1){
                    return 1;
                }
            }
            ma[i][j] = -1;
            return int(res_fl);
        };
        while(double(clock() - st) / CLOCKS_PER_SEC <= params::assignment_time_border[n - 6]){
            f(0, 0, 0);
            conn.assign(n, -1);
            ty = true;
        }

    }

}

struct StateTwo{
    int prev;
    int prev_d;
};

struct State{
    int pos;
    int prev;
    uint8_t v[100];
    bitset<100> bs;
    int man_sum;
    int man_sum_pow;
    int pos_dist;
    int linear_conf;
    double value;
    uint64_t hash;
    int prev_d;
    State(int pos, int turn ,int prev, bitset<100> bs, int man_sum, int man_sum_pow, int pos_dist, int linear_conf, double value, uint64_t hash, int prev_d) :
            pos(pos), prev(prev), bs(bs), man_sum(man_sum), man_sum_pow(man_sum_pow), pos_dist(pos_dist), linear_conf(linear_conf), value(value), hash(hash), prev_d(prev_d){}
    friend bool operator<(const State& a, const State& b){
        return a.value < b.value;
    }
    friend bool operator>(const State& a, const State& b){
        return a.value > b.value;
    }
};

pair<double,string> search(){
    bool end_fl = false;

    vector<vector<uint64_t>> hash_mask(n * n, vector<uint64_t>(n * n));
    for(int i = 0; i < n * n; ++i){
        for(int j = 0; j < n * n; ++j){
            hash_mask[i][j] = rnd() | (i64(rnd()) << 32);
        }
    }

    double man_sum = 0;
    double ima_max = INF;

    int ma_sum_border = MOD;

    auto conn_cnt = [&](const State* s){
        deque<int> q;
        bitset<100> w = ~s->bs;
        w.set(s->pos);
        for(int i = w._Find_first(); i < n * n; i = w._Find_next(i)){
            q.emplace_back(i);
        }
        q.emplace_back(s->pos);
        bitset<100> bs;
        int cn = 0;
        while(!q.empty()){
            int i = q.front();
            q.pop_front();
            if(w[i]){
                w.reset(i);
                ++cn;
            }
            for(int d = 0; d < 4; ++d){
                int nx = i / n + dx[d];
                int ny = i % n + dy[d];
                int np = nx * n + ny;
                if(nx < 0 || ny < 0 || nx >= n || ny >= n || !w[np]){
                    continue;
                }
                w.reset(np);
                q.emplace_front(np);
            }
        }
        return cn;
    };

    auto calc_value = [&](int turn, const State* s){
        using namespace params;
        // 終盤用評価関数
        double val;
        if(turn >= ma_sum_border){
            val = man_sum_coef * s->man_sum + man_sum_pow_coef * s->man_sum_pow + linear_conf_coef * s->linear_conf + pos_dist_coef * s->pos_dist
                  + (conn_cnt(s) - 1) * conn_cnt_coef[n - 6];
            // + pos_dist_coef * sqrt(s->pos_dist)
            // + 5.0 * var
            // + 2.0 * (mx + my)
            ;
        }
        else{
            val = man_sum_coef * s->man_sum + man_sum_pow_coef * s->man_sum_pow + linear_conf_coef * s->linear_conf + pos_dist_coef * s->pos_dist;
        }
        /*
        if(end_fl){
            val += (rnd_double() * 1 - 0.5);
        }
         */
        return val;
    };

    auto calc = [](auto nex, int nx, int ny){
        int mov_target_x = target[nex->v[nx * n + ny]] / n;
        int mov_target_y = target[nex->v[nx * n + ny]] % n;
        int lc = 0;
        if(nx == mov_target_x){
            for(int i = 0; i < n; ++i){
                if(i == ny){
                    continue;
                }
                int np = nx * n + i;
                int np_tile = nex->v[np];
                if(np_tile == st_x * n + st_y){
                    continue;
                }
                int np_target = target[np_tile];
                int np_x = np_target / n;
                int np_y = np_target % n;
                if(nx == np_x && ((ny < i) != (mov_target_y < np_y))){
                    // printf("(%d,%d)[%d,%d] - (%d,%d)[%d,%d]\n", nx, ny, mov_target_x, mov_target_y, nx, i, np_x, np_y);
                    lc++;
                }
            }
        }
        if(ny == mov_target_y){
            for(int i = 0; i < n; ++i){
                if(i == nx){
                    continue;
                }
                int np = i * n + ny;
                int np_tile = nex->v[np];
                if(np_tile == st_x * n + st_y){
                    continue;
                }
                int np_target = target[np_tile];
                int np_x = np_target / n;
                int np_y = np_target % n;
                if(ny == np_y && ((nx < i) != (mov_target_x < np_x))){
                    // printf("(%d,%d)[%d,%d] - (%d,%d)[%d,%d]\n", nx, ny, mov_target_x, mov_target_y, i, ny, np_x, np_y);
                    lc++;
                }
            }
        }
        return lc;
    };

    uint64_t init_hash = 0;
    for(int i = 0; i < n * n; ++i){
        init_hash ^= hash_mask[i][i];
    }

    double man_sum2 = 0;
    for(int i = 0; i < n * n; ++i){
        if(st_x * n + st_y != i){
            man_sum += man_dist(i, target[i]);
            man_sum2 += man_dist(i, target[i]) * man_dist(i, target[i]);
        }
    }
    int st_pos = st_x * n + st_y;
    int init_pos_dist = man_dist(st_pos, target[st_pos]);
    bitset<100> _bs;
    for(int i = 0; i < n * n; ++i){
        _bs[i] = (target[i] == i);
    }
    auto init_state = State(st_x * n + st_y, 0, -1, _bs, man_sum, man_sum2, init_pos_dist, 0, 0.0, init_hash, -1);
    for(int i = 0; i < n * n; ++i){
        init_state.v[i] = i;
    }
    conn_cnt(&init_state);

    int init_linear_con = 0;
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            if(i == st_x && j == st_y){
                continue;
            }
            init_linear_con += calc(&init_state, i, j);
        }
    }
    assert(init_linear_con % 2 == 0);
    init_linear_con /= 2;
    init_state.linear_conf = init_linear_con;

    init_state.value = calc_value(0, &init_state);

    HashMap<i64, int> hash_set;

    auto make_next_state = [&](int turn, const State* state, int prev_rank, int d){
        if((d ^ 2) == state->prev_d){
            return (optional<State>)(nullopt);
        }
        int x = state->pos / n;
        int y = state->pos % n;
        assert(state->v[state->pos] == st_x * n + st_y);
        int nx = x + dx[d];
        int ny = y + dy[d];
        int ns = nx * n + ny;
        if(nx < 0 || ny < 0 || nx >= n || ny >= n){
            return (optional<State>)(nullopt);
        }
        auto nex = State(*state);
        nex.prev_d = d;
        nex.prev = prev_rank;
        nex.pos = ns;

        nex.hash ^= hash_mask[state->pos][nex.v[state->pos]];
        nex.hash ^= hash_mask[nex.pos][nex.v[nex.pos]];

        nex.hash ^= hash_mask[state->pos][nex.v[nex.pos]];
        nex.hash ^= hash_mask[nex.pos][nex.v[state->pos]];

        int mov_tile = nex.v[nex.pos];
        int mov_target = target[mov_tile];

        int before_linear_conf = calc(&nex, nx, ny);

        int bd = man_dist(nex.pos, mov_target);
        int ad = man_dist(state->pos, mov_target);
        double bef_dist2 = bd * bd;
        double aft_dist2 = ad * ad;

        swap(nex.v[state->pos], nex.v[nex.pos]);
        int after_linear_conf = calc(&nex, x, y);

        nex.man_sum += ad - bd;
        nex.man_sum_pow += aft_dist2 - bef_dist2;//  + (end_fl ? 1. : 0) * (aft_cn - bef_cn);

        nex.pos_dist = man_dist(nex.pos, target[st_pos]);
        nex.linear_conf += after_linear_conf - before_linear_conf;
        nex.bs[state->pos] = (target[nex.v[state->pos]] == state->pos);
        nex.bs[nex.pos] = (target[nex.v[nex.pos]] == nex.pos);
        nex.value = calc_value(turn + 1, &nex);
        return optional<State>(move(nex));
    };

    int DEFAULT_WIDTH = params::DEFAULT_WIDTH_[n - 6];
    constexpr int WIDTH_STEP = 1000;
    int WIDTH = DEFAULT_WIDTH;

    bool break_fl = false;

    vector<vector<StateTwo>> que(turn_max + 1);
    vector<priority_queue<State, vector<State>, greater<State>>> wait_que(turn_max + 1);
    wait_que[0].emplace(init_state);

    pair<State,int> best_state(wait_que[0].top(), 0);

    vector<int> see(turn_max + 1, 0);

    int bef_back_pos = -1;

    int back = params::back_100; // 100000; // 100

    while(!break_fl){
        for(int k = 0; k < turn_max - 10 && !break_fl;){
            if(double(clock() - st) / CLOCKS_PER_SEC >= params::TIME_BORDER){
                break_fl = true;
                break;
            }
            // cout << k << " : " << see[k] << " " << WIDTH << endl;
            if(k >= ma_sum_border){
                WIDTH = see[k] + params::width_100;
            }
            if(k - best_state.second >= 30){
                // k = 0;
                WIDTH = see[k - 1] + WIDTH_STEP;
                k = max(0, min(best_state.second - back, ma_sum_border));// - 50));
                if(WIDTH >= 2100 && k < ma_sum_border){
                    params::conn_cnt_coef[n - 6] *= params::conn_cnt_pow[n - 6];
                    ma_sum_border = k;
                    best_state.first.value = calc_value(best_state.second, &best_state.first);
                    for(int kk = k; kk < turn_max - 10; ++kk){
                        priority_queue<State, vector<State>, greater<State>> nex_que;
                        int cn = 0;
                        // TODO: konohen kakunin suru?
                        while(!wait_que[kk].empty() && (kk - k <= params::kk_10 || ++cn <= params::cn_10)){
                            if(double(clock() - st) / CLOCKS_PER_SEC >= params::TIME_BORDER){
                                break_fl = true;
                                break;
                            }
                            auto p = move(wait_que[kk].top());
                            wait_que[kk].pop();
                            double v = calc_value(kk, &p);
                            p.value = v;
                            nex_que.emplace(move(p));
                        }
                        wait_que[kk] = move(nex_que);
                    }
                    hash_set = HashMap<i64, int>();
                    if(break_fl){
                        break;
                    }
                }
            }
            if(end_fl){
                if(k >= best_state.second){
                    WIDTH = WIDTH + params::width2_1000;
                    // hash_set.clear();
                    k = 0;
                }
            }
            bool fl = false;
            while((!fl || see[k] < WIDTH) && !wait_que[k].empty()){
                fl = true;
                if(double(clock() - st) / CLOCKS_PER_SEC >= params::TIME_BORDER){
                    break_fl = true;
                    break;
                }
                auto _state = move(wait_que[k].top());
                auto state = &_state;
                wait_que[k].pop();
                auto iter = hash_set.find(_state.hash);
                if(!iter.second){
                    hash_set.add(_state.hash, k);
                }
                else if(hash_set.v[iter.first].second <= k){
                    // cout << "continu" << endl;
                    continue;
                }
                else{
                    hash_set.v[iter.first].second = k;
                }
                ++see[k];
                que[k].emplace_back(StateTwo{state->prev, state->prev_d});

                bool is_upd = (abs(state->man_sum) < 1e-6) && (abs(best_state.first.man_sum) < 1e-6);
                if((is_upd && k <= best_state.second) || (abs(best_state.first.man_sum) > 1e-6 && state->value < best_state.first.value)){
                    best_state = make_pair(*state, k);
                    WIDTH = DEFAULT_WIDTH;
                    if(abs(state->man_sum) < 1e-6){
                        end_fl = true;
                        // chmin(params::TIME_BORDER, double(clock() - st) / CLOCKS_PER_SEC + 3.0);
                        break;
                    }
                    // back = 10;
                }
                for(int d = 0; d < 4; ++d){
                    assert(que[k].size() == see[k]);
                    auto nex = make_next_state(k, state, see[k] - 1, d);
                    if(nex){
                        double val = nex.value().value;
                        wait_que[k + 1].emplace(move(nex.value()));
                    }
                }
            }
            ++k;
        }
    }

    auto _state = best_state.first;
    int tu = best_state.second;
    vector<int> mov{_state.prev_d};
    auto state = que[tu - 1][_state.prev];
    --tu;
    double ms = best_state.first.man_sum;
    while(state.prev_d != -1){
        mov.emplace_back(state.prev_d);
        state = que[tu - 1][state.prev];
        --tu;
    }
    reverse(mov.begin(), mov.end());
    string s;
    for(int i = 0; i < mov.size(); ++i){
        s += dstr[mov[i]];
    }
    if(abs(ms) > 1e-6){
        return make_pair(0.0, s);
    }
    else{
        double per = round(500000 * (2.0 - 1.0 * s.size() / turn_max));
        return make_pair(per, s);
    }
}

void solve(){

    if(n == 10){
        params::back_100 = 120;
        params::width_100 = 404;
        params::cn_10 = 12;
        params::kk_10 = 6;
        params::width2_1000 = 358;
    }
    if(n == 9){
        params::back_100 = 136;
        params::width_100 = 141;
        params::cn_10 = 15;
        params::kk_10 = 12;
        params::width2_1000 = 1139;
    }
    spanning_tree();

    int cn = max(1, min(int(tups.size()) - 1, params::splits[n - 6]));

    double start = double(clock() - st) / CLOCKS_PER_SEC;
    double final = params::TIME_BORDER;

    pair<double,string> res(0.0, "U");

    while(tups.size() > cn){
        tups.pop();
    }
    for(int i = 0; i < cn; ++i){
        auto tup = move(tups.top());
        tups.pop();
        params::TIME_BORDER = start + (final - start) * (i + 1) / cn;

        vector<vector<int>> ma(n, vector<int>(n));
        assert(get<0>(tup) <= 1e9);
        target = get<1>(tup);
        target_inv = get<2>(tup);
        auto res_ = search();
        chmax(res, res_);
    }
#ifdef HAND
    cout << "score: " << res.first << endl;
#endif
    cout << res.second << endl;
#ifdef OPTIMIZE
    cerr << res.first << endl;
#endif
}


signed main(){

    st = clock();

#ifndef HAND
    // TODO: input
    cin >> n >> turn_max;
    inp.resize(n);
    for(int i = 0; i < n; ++i){
        cin >> inp[i];
    }
    solve();
#else
#ifdef OPTIMIZE
    params::load_params();
    int j = 0;
    cin >> j;
#else
    // TODO: case 349 BUG!!!!
    // for(auto j : vector<int>{14, 25, 31})
    int j = 0;
    // for(int j = ; j < 100; ++j)
#endif
    {
        cout << "Case: " << j << endl;
        case_num = j;
        string t;
        int jj = j;
        for(int k = 0; k < 4; ++k){
            t += (jj % 10) + '0';
            jj /= 10;
        }
        reverse(t.begin(), t.end());
        ifstream ifs("./tools/in/" + t + ".txt");
        assert(ifs.is_open());
        // TODO: input
        ifs >> n >> turn_max;
        inp.clear();
        inp.resize(n);
        for(int i = 0; i < n; ++i){
            ifs >> inp[i];
        }
        st = clock();
        solve();
    }
#endif

}
