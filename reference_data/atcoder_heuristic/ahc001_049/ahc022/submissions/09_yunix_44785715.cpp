
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <chrono>
#include <algorithm>
#include <random>
#include <map>
#include <set>
#include <queue>
#include <time.h>
#include <chrono>
#include <cmath>
#include <climits>
#include <bitset>
#include <time.h>
#include <cassert>
#include <iosfwd>
#include <cstdio>

// #include <atcoder/mincostflow>

using namespace std;
// using namespace atcoder;
using Ps = pair<short, short>;
using vec_int = vector<int>;
using P = pair<int, int>;
using P2 = pair<P, P>;
using P3 = pair<float, int>;
using T = tuple<int, int, int>;
using T2 = tuple<float, int, int>;
using T3 = tuple<float, int, int, int, int>;
using T4 = tuple<int, int, int, int>;
using T5 = tuple<int, int, int, int, int>;
using TT = tuple<int, int, int>;
using ll = long long;
using uc = unsigned char;
using ld = long double;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#pragma GCC optimize("Ofast")
// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

// ios_base::sync_with_stdio(false);
int L, N, S;
vec_int Y, X;

// 各種パラメータ
/*
ld sigma_div = 2.1;
ld sigma_div2 = 5.5;
int S_threshold = 600;
ld precision_room1 = 0.85;
ld precision_room2 = 0.95;
ld precision_room3 = 0.7;
ld prob_update1 = 0.9;
ld prob_update2 = 0.8;
ld posterior_param1 = -6.1;
ld posterior_param2 = -4.1;
*/
ld sigma_div = 2.2336217257057545;
ld sigma_div2 = 5.740466625078945;
int S_threshold = 556;
ld precision_room1 = 0.8054755068932072;
ld precision_room2 = 0.8935849981755233;
ld precision_room3 = 0.9795067139442285;
ld precision_room4 = 0.74963014233463;
ld prob_update1 = 0.9272437040657722;
ld prob_update2 = 0.9160598524530417;
ld prob_update3 = 0.8125148803549518;
ld posterior_param1 = -5.656432274904744;
ld posterior_param2 = -4.026459749168571;
ld cost_penalty = 1.0508552647407308;




vec_int dx_vec = {-4, 0, 4};
// = {(int)-sqrt(L), 0, (int)sqrt(L)};
vec_int dy_vec = {-4, 0, 4};
//= {(int)-sqrt(L), 0, (int)sqrt(L)};
// vec_int dx_vec = {-2, -1, 0, 1, 2};
// vec_int dy_vec = {-2, -1, 0, 1, 2};
vec_int dx_vec2 = {-2, -1, 0, 1, 2};
vec_int dy_vec2 = {-2, -1, 0, 1, 2};
std::mt19937 mt{12345};
// std::mt19937 mt{time(NULL)};
int RAND_ARR_LEN = 400000;
int RAND_RANGE = 1000000000;
float TIME_LIMIT = 4000;
float INV_TIME_LIMIT = 1. / 1000.;
const int JOB_MAX_LEN = 1003;
std::uniform_int_distribution<int> dist(1, RAND_RANGE);
std::normal_distribution<ld> normal_dist(0., 1.);

int WAITING_TASK = 1001;
double T0, T1;

ld exp_cache[1001];
ld erf_cache[1001];

void calc_exp_value()
{
    rep(diff, 1001)
    {
        ld sigma = S;
        exp_cache[diff] = (1. / sqrt(2 * M_PI * sigma * sigma)) * exp(-pow(diff, 2) / (2 * sigma * sigma));
        ld nsigma = (ld)diff / (ld)S;
        erf_cache[diff] = erf(nsigma); // 1000-muまたはmuを入れるとOK
    }
}

void remove_val_and_pop_from_last(vec_int &A, int val)
{
    for (int i = 0; i < A.size(); i++)
    {
        if (A.at(i) == val)
        {
            A.at(i) = A.at(A.size() - 1);
            A.pop_back();
            return;
        }
    }
    throw runtime_error("trying to remove non-existing value");
}

class Rand
{
private:
    int count = 0;
    int count_normal = 0;
    vec_int rand_arr;
    vector<ld> rand_normal_arr;

public:
    Rand(){
        rep(i, RAND_ARR_LEN){
            rand_arr.push_back(dist(mt));
    rand_normal_arr.push_back(normal_dist(mt));
}
}
;
int get();
ld get_normal();
int get_rand(int from, int to);
float get_float();
}
;

int Rand::get()
{
    int num = rand_arr.at(count);
    count += 1;
    count %= RAND_ARR_LEN;
    return num;
}

ld Rand::get_normal()
{
    ld num = rand_normal_arr[count_normal];
    count_normal += 1;
    count_normal %= RAND_ARR_LEN;
    return num;
}

int Rand::get_rand(int from, int to)
{
    int diff = to - from;
    int num = get() % diff;
    return num + from;
}

float Rand::get_float()
{
    // 0~1の間の一様乱数
    return (float)get() / (float)RAND_RANGE;
}

Rand ro;

class PseudoSet
{
private:
    vec_int index_arr;

public:
    vec_int data;
    PseudoSet(){};
    PseudoSet(int value_range)
    {
        index_arr = vec_int(value_range, -1);
    }

    bool count(int value);
    void insert(int value);
    void erase(int value);
    vec_int get_data() { return data; };
    int size() { return data.size(); };
    int get_random() { return data.at(ro.get_rand(0, size())); };
};

bool PseudoSet::count(int value)
{
    return index_arr[value] != -1;
}

void PseudoSet::insert(int value)
{
    if (count(value))
        return;
    index_arr[value] = data.size();
    data.push_back(value);
}

void PseudoSet::erase(int value)
{
    if (!count(value))
    {
        // throw runtime_error("no existing value:"+to_string(value));
        return;
    }
    int tail_value = data[data.size() - 1];
    if (value == tail_value)
    {
        data.pop_back();
        index_arr[value] = -1;
    }
    else
    {
        index_arr[tail_value] = index_arr[value];
        index_arr[value] = -1;
        data[index_arr[tail_value]] = tail_value;
        data.pop_back();
    }
}

float get_time(bool init)
{
    static std::chrono::system_clock::time_point start;
    if (init)
    {
        start = std::chrono::system_clock::now();
    }
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); // 処理に要した時間をミリ秒に変換
}

double current_tmp(double current_time, double T0, double T1)
{
    return T1 + (T0 - T1) * (TIME_LIMIT - current_time) / (TIME_LIMIT);
    // double start_time = TIME_LIMIT * 0;
    // double x = (current_time - start_time) / (TIME_LIMIT);
    // return pow(T1, 1 - x) * pow(T0, x);
}

bool is_valid_transition(int diff, double current_time, double T0, float T1)
{
    float t = current_tmp(current_time, T0, T1);
    float rand = ro.get_float();
    // diffが正だったら常にアクセぷと
    return rand < exp(((float)diff) / t);
}

double current_tmp2(double current_time, double T0, double T1)
{
    return T1 + (T0 - T1) * (2500 - current_time) / (2500);
    // double start_time = TIME_LIMIT * 0;
    // double x = (current_time - start_time) / (TIME_LIMIT);
    // return pow(T1, 1 - x) * pow(T0, x);
}

bool is_valid_transition2(int diff, double current_time, double T0, float T1)
{
    float t = current_tmp2(current_time, T0, T1);
    float rand = ro.get_float();
    // diffが正だったら常にアクセぷと
    return rand < exp(((float)diff) / t);
}

ld calc_exp(int mu, int val)
{
    // この計算はあらかじめキャッシュにしておくべき
    ld sigma = S;
    return (1. / sqrt(2 * M_PI * sigma * sigma)) * exp(-pow(val - mu, 2) / (2 * sigma * sigma));
}

namespace internal
{
    template <class T>
    struct simple_queue
    {
        std::vector<T> payload;
        int pos = 0;
        void reserve(int n) { payload.reserve(n); }
        int size() const { return int(payload.size()) - pos; }
        bool empty() const { return pos == int(payload.size()); }
        void push(const T &t) { payload.push_back(t); }
        T &front() { return payload[pos]; }
        void clear()
        {
            payload.clear();
            pos = 0;
        }
        void pop() { pos++; }
    };
    template <class E>
    struct csr
    {
        std::vector<int> start;
        std::vector<E> elist;
        explicit csr(int n, const std::vector<std::pair<int, E>> &edges)
            : start(n + 1), elist(edges.size())
        {
            for (auto e : edges)
            {
                start[e.first + 1]++;
            }
            for (int i = 1; i <= n; i++)
            {
                start[i] += start[i - 1];
            }
            auto counter = start;
            for (auto e : edges)
            {
                elist[counter[e.first]++] = e.second;
            }
        }
    };
}
template <class Cap, class Cost>
struct mcf_graph
{
public:
    mcf_graph() {}
    explicit mcf_graph(int n) : _n(n) {}

    int add_edge(int from, int to, Cap cap, Cost cost)
    {
        assert(0 <= from && from < _n);
        assert(0 <= to && to < _n);
        assert(0 <= cap);
        assert(0 <= cost);
        int m = int(_edges.size());
        _edges.push_back({from, to, cap, 0, cost});
        return m;
    }

    struct edge
    {
        int from, to;
        Cap cap, flow;
        Cost cost;
    };

    edge get_edge(int i)
    {
        int m = int(_edges.size());
        assert(0 <= i && i < m);
        return _edges[i];
    }
    std::vector<edge> edges() { return _edges; }

    std::pair<Cap, Cost> flow(int s, int t)
    {
        return flow(s, t, std::numeric_limits<Cap>::max());
    }
    std::pair<Cap, Cost> flow(int s, int t, Cap flow_limit)
    {
        return slope(s, t, flow_limit).back();
    }
    std::vector<std::pair<Cap, Cost>> slope(int s, int t)
    {
        return slope(s, t, std::numeric_limits<Cap>::max());
    }
    std::vector<std::pair<Cap, Cost>> slope(int s, int t, Cap flow_limit)
    {
        assert(0 <= s && s < _n);
        assert(0 <= t && t < _n);
        assert(s != t);

        int m = int(_edges.size());
        std::vector<int> edge_idx(m);

        auto g = [&]()
        {
            std::vector<int> degree(_n), redge_idx(m);
            std::vector<std::pair<int, _edge>> elist;
            elist.reserve(2 * m);
            for (int i = 0; i < m; i++)
            {
                auto e = _edges[i];
                edge_idx[i] = degree[e.from]++;
                redge_idx[i] = degree[e.to]++;
                elist.push_back({e.from, {e.to, -1, e.cap - e.flow, e.cost}});
                elist.push_back({e.to, {e.from, -1, e.flow, -e.cost}});
            }
            auto _g = internal::csr<_edge>(_n, elist);
            for (int i = 0; i < m; i++)
            {
                auto e = _edges[i];
                edge_idx[i] += _g.start[e.from];
                redge_idx[i] += _g.start[e.to];
                _g.elist[edge_idx[i]].rev = redge_idx[i];
                _g.elist[redge_idx[i]].rev = edge_idx[i];
            }
            return _g;
        }();

        auto result = slope(g, s, t, flow_limit);

        for (int i = 0; i < m; i++)
        {
            auto e = g.elist[edge_idx[i]];
            _edges[i].flow = _edges[i].cap - e.cap;
        }

        return result;
    }

private:
    int _n;
    std::vector<edge> _edges;

    // inside edge
    struct _edge
    {
        int to, rev;
        Cap cap;
        Cost cost;
    };

    std::vector<std::pair<Cap, Cost>> slope(internal::csr<_edge> &g,
                                            int s,
                                            int t,
                                            Cap flow_limit)
    {
        // variants (C = maxcost):
        // -(n-1)C <= dual[s] <= dual[i] <= dual[t] = 0
        // reduced cost (= e.cost + dual[e.from] - dual[e.to]) >= 0 for all edge

        // dual_dist[i] = (dual[i], dist[i])
        std::vector<std::pair<Cost, Cost>> dual_dist(_n);
        std::vector<int> prev_e(_n);
        std::vector<bool> vis(_n);
        struct Q
        {
            Cost key;
            int to;
            bool operator<(Q r) const { return key > r.key; }
        };
        std::vector<int> que_min;
        std::vector<Q> que;
        auto dual_ref = [&]()
        {
            for (int i = 0; i < _n; i++)
            {
                dual_dist[i].second = std::numeric_limits<Cost>::max();
            }
            std::fill(vis.begin(), vis.end(), false);
            que_min.clear();
            que.clear();

            // que[0..heap_r) was heapified
            size_t heap_r = 0;

            dual_dist[s].second = 0;
            que_min.push_back(s);
            while (!que_min.empty() || !que.empty())
            {
                int v;
                if (!que_min.empty())
                {
                    v = que_min.back();
                    que_min.pop_back();
                }
                else
                {
                    while (heap_r < que.size())
                    {
                        heap_r++;
                        std::push_heap(que.begin(), que.begin() + heap_r);
                    }
                    v = que.front().to;
                    std::pop_heap(que.begin(), que.end());
                    que.pop_back();
                    heap_r--;
                }
                if (vis[v])
                    continue;
                vis[v] = true;
                if (v == t)
                    break;
                // dist[v] = shortest(s, v) + dual[s] - dual[v]
                // dist[v] >= 0 (all reduced cost are positive)
                // dist[v] <= (n-1)C
                Cost dual_v = dual_dist[v].first, dist_v = dual_dist[v].second;
                for (int i = g.start[v]; i < g.start[v + 1]; i++)
                {
                    auto e = g.elist[i];
                    if (!e.cap)
                        continue;
                    // |-dual[e.to] + dual[v]| <= (n-1)C
                    // cost <= C - -(n-1)C + 0 = nC
                    Cost cost = e.cost - dual_dist[e.to].first + dual_v;
                    if (dual_dist[e.to].second - dist_v > cost)
                    {
                        Cost dist_to = dist_v + cost;
                        dual_dist[e.to].second = dist_to;
                        prev_e[e.to] = e.rev;
                        if (dist_to == dist_v)
                        {
                            que_min.push_back(e.to);
                        }
                        else
                        {
                            que.push_back(Q{dist_to, e.to});
                        }
                    }
                }
            }
            if (!vis[t])
            {
                return false;
            }

            for (int v = 0; v < _n; v++)
            {
                if (!vis[v])
                    continue;
                // dual[v] = dual[v] - dist[t] + dist[v]
                //         = dual[v] - (shortest(s, t) + dual[s] - dual[t]) +
                //         (shortest(s, v) + dual[s] - dual[v]) = - shortest(s,
                //         t) + dual[t] + shortest(s, v) = shortest(s, v) -
                //         shortest(s, t) >= 0 - (n-1)C
                dual_dist[v].first -= dual_dist[t].second - dual_dist[v].second;
            }
            return true;
        };
        Cap flow = 0;
        Cost cost = 0, prev_cost_per_flow = -1;
        std::vector<std::pair<Cap, Cost>> result = {{Cap(0), Cost(0)}};
        while (flow < flow_limit)
        {
            if (!dual_ref())
                break;
            Cap c = flow_limit - flow;
            for (int v = t; v != s; v = g.elist[prev_e[v]].to)
            {
                c = std::min(c, g.elist[g.elist[prev_e[v]].rev].cap);
            }
            for (int v = t; v != s; v = g.elist[prev_e[v]].to)
            {
                auto &e = g.elist[prev_e[v]];
                e.cap += c;
                g.elist[e.rev].cap -= c;
            }
            Cost d = -dual_dist[s].first;
            flow += c;
            cost += c * d;
            if (prev_cost_per_flow == d)
            {
                result.pop_back();
            }
            result.push_back({flow, cost});
            prev_cost_per_flow = d;
        }
        return result;
    }
};




class State
{
private:
    int max_val;
    int min_val;
    int ds;
    void set_val(int x, int y, int dx, int dy, int val);

    void set_large_S(int height);
    void set_small_S(int height);
    vector<int> space_cells;
    ll calc_placement_cost();
    int offset_x = 0;
    int offset_y = 0;
    int small_offset1 = 0;
    int small_offset2 = 0;
    void optimize_offset();

public:
    State(int height);
    State(int height, int offset_x, int offset_y);
    State(int height, int offset_x, int offset_y, int small_offset1, int small_offset2);
    State(bool init){};

    int get_val(int x, int y, int dx, int dy);
    void output();
    ll placement_cost;
    static int determine_height();
};

ll State::calc_placement_cost()
{
    ll result = 0;
    rep(i, L) rep(j, L)
    {
        int diff = abs(space_cells[i * L + j] - space_cells[((i + 1) % L) * L + j]);
        result += diff * diff;
        int diff2 = abs(space_cells[i * L + j] - space_cells[i * L + (j + 1) % L]);
        result += diff2 * diff2;
    }
    return result;
}

State::State(int height)
{
    offset_x = 0;
    offset_y = 0;
    space_cells = vector<int>(L * L, -1);
    if (S > 1)
    {
        set_large_S(height);
    }
    else
    {
        set_small_S(height);
    }
    placement_cost = calc_placement_cost();
}
State::State(int height, int offset_x, int offset_y) : offset_x(offset_x), offset_y(offset_y)
{
    space_cells = vector<int>(L * L, -1);
    if (S > 1)
    {
        set_large_S(height);
    }
    else
    {
        set_small_S(height);
    }
    placement_cost = calc_placement_cost();
}
State::State(int height, int offset_x, int offset_y, int small_offset1, int small_offset2) : offset_x(offset_x), offset_y(offset_y), small_offset1(small_offset1), small_offset2(small_offset2)
{
    space_cells = vector<int>(L * L, -1);
    if (S > 1)
    {
        set_large_S(height);
    }
    else
    {
        set_small_S(height);
    }
    placement_cost = calc_placement_cost();
}

int State::determine_height()
{
    int height;
    if (S > 600)
    {
        height = 1000;
    }
    else if (300 < S && S <= 600)
    {
        height = 400 + (S - 300) * 2;
        if (S < 400)
        {
            height *= 1.5;
        }
        if (S == 400)
        {
            height *= 1.3;
        }
        else if (S == 441)
        {
            height *= 1.2;
        }
        else if (S == 484 || height == 529)
        {
            height *= 1.1;
        }
    }
    else if (200 < S && S <= 300)
    {
        height = 200 + (S - 200) * 2;
        height *= 1.2;
        if (S < 230)
        {
            height *= 2;
        }
        else if (S == 256)
        {
            height *= 1.8;
        }
        else if (S == 289)
        {
            height *= 1.5;
        }
    }
    else if (S <= 200)
    {
        height = S * 2.3;
        if (height < 150)
        {
            height *= 1.3;
        }
        map<int, double> mult_map = {
            {4, 5.0},
            {9, 4.0},
            {16, 3.2},
            {25, 2.5},
            {36, 2.1},
            {49, 1.9},
            {64, 1.7},
            {81, 1.6},
            {100, 1.4},
            {121, 1.3},
            {144, 1.2},
        };
        if (mult_map.count(S))
        {
            height *= mult_map[S];
        }
    }
    return height;
}

void State::set_large_S(int height)
{
    if (height > 700 && L > 18)
    {
        optimize_offset();
        ld sigma = (ld)L / sigma_div;
        rep(i, L) rep(j, L)
        {
            int x = (i + offset_x + L) % L;
            int y = (j + offset_y + L) % L;
            ld diff1 = pow(min(abs(x - (ld)L / 5.), abs(x - L - (ld)L / 5.)), 2) + pow(min(abs(y - (ld)L / 5.), abs(y - L - (ld)L / 5.)), 2);
            // space_cells[i * L + j] += height * exp(-diff1 / 2. / (sigma * sigma));
            space_cells[i * L + j] = max((ld)height * (1 - sqrt(diff1) / (sigma)), (ld)0.);
            if (x == L * 1 / 3 + L / 10 + small_offset1 && y == L * 1 / 3 + L / 10 + small_offset1)
            {
                space_cells[i * L + j] = height;
            }
            if (x == (L * 1 / 8 - L / 10 - small_offset2 + L) % L && y == (L * 1 / 8 - L / 10 - small_offset2 + L) % L)
            {
                space_cells[i * L + j] = height;
            }
        }

        return;
    }

    ld sigma = max((ld)(L / sigma_div2), (ld)2.);
    // height = 285;
    rep(i, L)
    {
        rep(j, L)
        {
            int x = (i + offset_x) % L;
            int y = (j + offset_y) % L;

            space_cells[i * L + j] = 0;

            ld diff1 = pow(min(abs(x - (ld)L / 5.), abs(x - L - (ld)L / 5.)), 2) + pow(min(abs(y - (ld)L / 5.), abs(y - L - (ld)L / 5.)), 2);
            ld diff2 = pow(min(abs(x - (ld)L * 2. / 3.), abs(x + L - (ld)L * 2. / 3.)), 2) + pow(min(abs(y - (ld)L * 4. / 5.), abs(L + y - (ld)L * 4. / 5.)), 2);
            if (height > 500)
            {
                space_cells[i * L + j] += height * exp(-diff1 / 2. / (sigma * sigma));
                if (x == L * 1 / 3 + 1 && y == L * 1 / 3 + 1)
                {
                    space_cells[i * L + j] = height;
                }
            }
            else if (S < 200 && false)
            {
                space_cells[i * L + j] = 500;
                space_cells[i * L + j] += height * exp(-diff1 / 2. / (sigma * sigma));
                space_cells[i * L + j] -= height * exp(-diff2 / 2. / (sigma * sigma));
                if (x == L * 1 / 2 && y == L * 1 / 2)
                {
                    space_cells[i * L + j] += S * 2;
                }
                if (x == L - 1 && y == L - 1)
                {
                    space_cells[i * L + j] -= S * 2;
                }
            }
            else
            {
                space_cells[i * L + j] = 500;
                space_cells[i * L + j] += height * exp(-diff1 / 2. / (sigma * sigma));
                space_cells[i * L + j] -= height * exp(-diff2 / 2. / (sigma * sigma));

                if (x == L * 1 / 3 + 1 && y == L * 1 / 3 + 1)
                {
                    // space_cells[i * L + j] = min(500 + height, space_cells[i * L + j] + 3 * S);
                    space_cells[i * L + j] = 500 + height;
                }

                if (x == L * 5 / 6 + 2 && y == L * 5 / 6 + 2)
                {

                    // space_cells[i * L + j] = max(500 - height, space_cells[i * L + j] - 3 * S);
                    space_cells[i * L + j] = 500 - height;
                    // space_cells[i * L + j] = 500 - height;
                }
                /*
                if (x == L * 1 / 2 && y == L * 1 / 2)
                {
                    space_cells[i * L + j] += S * 2;
                }
                if (x == L - 1 && y == L - 1)
                {
                    space_cells[i * L + j] -= S * 2;
                }
                */
            }
            space_cells[i * L + j] = max(0, space_cells[i * L + j]);
            space_cells[i * L + j] = min(1000, space_cells[i * L + j]);
        }
    }
}

void State::set_small_S(int max_height)
{
    // ds = min(2 * S / 3, 4);
    rep(i, L) rep(j, L)
    {
        double distance = sqrt(pow(i - L / 2, 2) + pow(j - L / 2, 2)) / ((double)L / 2.);
        double height = (exp(-pow(distance, 2.0)) - 0.14) * max_height / 0.86 - max_height / 0.86 / 2;
        space_cells[i * L + j] = 500 + height;
        // space_cells[i * L + j] += space_cells[i * L + j] < 500 ? -S : S;
        space_cells[i * L + j] = max(0, space_cells[i * L + j]);
        space_cells[i * L + j] = min(1000, space_cells[i * L + j]);
    }
}

int State::get_val(int x, int y, int dx, int dy)
{
    int nx = (x + dx + 5 * L) % L;
    int ny = (y + dy + 5 * L) % L;
    return space_cells[nx * L + ny];
}
void State::set_val(int x, int y, int dx, int dy, int val)
{
    int nx = (x + dx + 5 * L) % L;
    int ny = (y + dy + 5 * L) % L;
    space_cells[nx * L + ny] = max(min(val, 1000), 0);
}

void State::output()
{
    rep(i, L)
    {
        rep(j, L)
        {
            cout << space_cells[i * L + j] << " ";
        }
        cout << endl;
    }
}

int distance_between(int i, int j)
{
    int val1 = max(i, j);
    int val2 = min(i, j);
    return min(L - (val1 - val2), val1 - val2);
}

void State::optimize_offset()
{
    int pos1 = L * 1 / 3 + L / 10;
    int pos2 = L * 1 / 8 - L / 10;

    int best_score = INT_MAX;
    int best_x;
    int best_y;
    rep(i, L)
    {
        rep(j, L)
        {
            int xpos1 = (pos1 + i + L) % L;
            int ypos1 = (pos1 + j + L) % L;
            int xpos2 = (pos2 + i + L) % L;
            int ypos2 = (pos2 + j + L) % L;
            int score = 0;

            rep(k, N)
            {
                int distance1 = distance_between(xpos1, X[k]) + distance_between(ypos1, Y[k]);
                int distance2 = distance_between(xpos2, X[k]) + distance_between(ypos2, Y[k]);
                score += min(distance1, distance2);
            }
            // if (i == 0 && j == 0)
            // cerr << "initial score:" << score << endl;
            if (best_score > score)
            {
                best_score = score;
                best_x = i;
                best_y = j;
            }
        }
    }
    offset_x = L - best_x;
    offset_y = L - best_y;

    // cerr << "best score:" << best_score << endl;
}




class ScorePredictor
{
private:
    vector<vector<P>> best_pos_to_search = vector<vector<P>>(N, vector<P>(N, make_pair(0, 0)));
    State *state;
    int measure(int answer_hole, int hole, int dx, int dy);
    ld update_probability_for_all(vector<ld> &probabilities, int dx, int dy, int observation, int answer_hole);
    ld calc_probability(int original_value, int observation);
    void measure_until_clear(int answer_hole, vector<ld> &probabilities, ld precision, ld &measurement_cost, int &tot_count);
    double calculate_score(vec_int &solution);
    double try_swap(int hole_id1, int hole_id2, vector<int> &solution);
    double calc_hole_score(int hole_id, int sol);
    vector<vector<tuple<int, int, int>>> observations = vector<vector<tuple<int, int, int>>>(N);
    vector<vector<ld>> posteriors = vector<vector<ld>>(N, vector<ld>(N, 0.));

public:
    map<P, int> error_dict;
    ScorePredictor(State *state) : state(state)
    {
        for (int exit1 = 0; exit1 < N; exit1++)
        {
            for (int exit2 = exit1 + 1; exit2 < N; exit2++)
            {
                double max_gain = -1;
                int max_dx = -INT_MAX;
                int max_dy = -INT_MAX;
                for (int i = -L / 2 - 1; i <= L / 2 + 1; i++)
                {
                    for (int j = -L / 2 - 1; j <= L / 2 + 1; j++)
                    {
                        int cost = (10 + abs(i) + abs(j));
                        if (S < 9 && cost > 16)
                            continue;
                        if (S == 1 && cost > 14)
                            continue;

                        int val1 = state->get_val(X[exit1], Y[exit1], i, j);
                        int val2 = state->get_val(X[exit2], Y[exit2], i, j);
                        int diff = abs(val1 - val2);
                        ld gain = (ld)diff / sqrt((ld)cost * cost_penalty);
                        if (gain > max_gain)
                        {
                            max_gain = gain;
                            max_dx = i;
                            max_dy = j;
                        }
                    }
                }
                best_pos_to_search[exit1][exit2] = make_pair(max_dx, max_dy);
                best_pos_to_search[exit2][exit1] = make_pair(max_dx, max_dy);
            }
        }
    };
    ll predict_score();
};

int ScorePredictor::measure(int answer_hole, int hole, int dx, int dy)
{
    int val = state->get_val(X[answer_hole], Y[answer_hole], dx, dy);
    // valを中心にしてσでサンプリング
    ld value_raw = ro.get_normal() * (ld)S + val;
    observations[answer_hole].push_back(make_tuple((int)value_raw, dx, dy));
    return max(0, min(1000, (int)value_raw));
}

double ScorePredictor::calc_hole_score(int hole, int exit)
{
    return posteriors[hole][exit];
}

double ScorePredictor::calculate_score(vector<int> &solution)
{
    double score = 0;
    rep(hole_id, N)
    {
        score += calc_hole_score(hole_id, solution[hole_id]);
    }
    return score;
}
// ここは定義が混乱しているので注意
double ScorePredictor::try_swap(int hole1, int hole2, vector<int> &solution)
{
    double result = 0;
    result -= calc_hole_score(hole1, solution[hole1]);
    result -= calc_hole_score(hole2, solution[hole2]);

    result += calc_hole_score(hole1, solution[hole2]);
    result += calc_hole_score(hole2, solution[hole1]);
    return result;
}
void set_first_second(vector<ld> &probs, int &first, int &second)
{
    int tmp1 = -1;
    int tmp2 = -1;
    ld val1 = -1;
    ld val2 = -1;
    rep(i, N)
    {
        if (probs[i] > val1)
        {
            val2 = val1;
            tmp2 = tmp1;
            tmp1 = i;
            val1 = probs[i];
        }
        else if (probs[i] > val2)
        {
            tmp2 = i;
            val2 = probs[i];
        }
    }
    first = tmp1;
    second = tmp2;
}

ld ScorePredictor::calc_probability(
    int mu, int observation)
{
    ld result = 1.;
    if (observation == 0)
    {
        result *= (1. - erf_cache[mu]) / 2.;
    }
    else if (observation == 1000)
    {
        result *= (1. - erf_cache[1000 - mu]) / 2.;
    }
    else
    {
        result *= exp_cache[abs(mu - observation)];
    }
    return result;
}

ld ScorePredictor::update_probability_for_all(
    vector<ld> &probabilities,
    int dx, int dy, int observation, int answer_hole)
{
    // rep(i, N)
    ld prob_sum = 0;
    rep(hole_id, N)
    {
        int original_value = state->get_val(X[hole_id], Y[hole_id], dx, dy);

        ld prob = calc_probability(original_value, observation);
        probabilities[hole_id] *= prob;
        prob_sum += probabilities[hole_id];
        posteriors[answer_hole][hole_id] += log(prob);
    }

    rep(hole_id, N)
    {
        probabilities[hole_id] /= prob_sum;
        if (!isfinite(probabilities[hole_id]))
            probabilities[hole_id] = 0.;
    }
    return 1.;
}

/*
vector<pair<int, ld>> get_flow2(vector<vector<ld>> &posteriors)
{
    vector<int> result(N, -1);
    vector<int> assignment(N, -1);
    vector<vector<pair<ld, int>>> vecs;
    rep(i, N)
    {
        auto vec = vector<pair<ld, int>>();
        rep(j, N)
        {
            vec.push_back(make_pair(posteriors[i][j], j));
        }
        sort(vec.begin(), vec.end());
        vecs.push_back(vec);
    }

    rep(i, N)
    {
        int exit;
        for (int j = 0; j < N; j++)
        {
            exit = vecs[i][j].second;
            if (assignment[exit] != -1)
                continue;
        }
        result[i] = exit;
        assignment[exit] = i;
    }
    int count = 0;
    while (count < 3000)
    {
        int flag = 0;
        queue<int> que;
        rep(i, N)
        {
            que.push(i);
        }
        set<int> nexts;
        while (!que.empty())
        {
            int i = que.front();
            que.pop();
            rep(j, N)
            {
                int before = posteriors[i][result[i]] + posteriors[j][result[j]];
                int after = posteriors[i][result[j]] + posteriors[j][result[i]];
                if (after > before)
                {
                    swap(assignment[result[i]], assignment[result[j]]);
                    swap(result[i], result[j]);
                    nexts.insert(i);
                    nexts.insert(j);
                }
            }
        }
        for (auto num : nexts)
        {
            que.push(num);
        }
        count++;
    }

    vector<pair<int, ld>> result2;
    rep(i, N)
    {
        result2.push_back(make_pair(result[i], posteriors[i][result[i]]));
    }
    return result2;
}
*/

vector<pair<int, ld>> get_flow2(vector<vector<ld>> &posteriors)
{

    mcf_graph<int, ll> graph(2 * N + 2);
    rep(i, N)
    {
        graph.add_edge(2 * N, i, 1, 0.);
        graph.add_edge(i + N, 2 * N + 1, 1, 0.);
        // ld prob_sum = accumulate(probabilities[i].begin(), probabilities[i].end(), (ld)0.);
        auto iter = max_element(posteriors[i].begin(), posteriors[i].end());
        ld max_posterior = *iter;

        rep(j, N)
        {
            if (posteriors[i][j] < max_posterior - 20)
                continue;
            ld cost;
            if (isfinite(posteriors[i][j]))
            {
                cost = (ll)(-posteriors[i][j] * 10000);
            }
            else
            {
                cost = 100000000;
            }
            // cerr << i << " " << j << "cost:" << cost << endl;
            graph.add_edge(i, j + N, 1, cost);
        }
    }
    // 流せるだけ流す
    auto tmp = graph.flow(2 * N, 2 * N + 1);
    vector<pair<int, ld>> result(N);
    auto edges = graph.edges();
    for (auto edge : edges)
    {
        if (edge.flow == 1 && edge.from != 2 * N && edge.to != 2 * N + 1)
        {
            int from = edge.from;
            int to = edge.to - N;
            result.at(from) = make_pair(to, edge.cost);
        }
    }
    return result;
}

ll ScorePredictor::predict_score()
{

    observations.clear();
    observations = vector<vector<tuple<int, int, int>>>(N);
    ld measurement_cost = 0;
    vector<vector<ld>> probabilities(N, vector<ld>(N, 1.));
    double precision_room = S < 400 ? precision_room1 : precision_room2;
    if (S == 1)
    {
        precision_room = precision_room3;
    }

    vector<ld> precision_vec(N, precision_room);
    int tot_count = 0;
    rep(answer_hole, N)
    {
        probabilities[(answer_hole + 1) % N] = probabilities[answer_hole];
        measure_until_clear(answer_hole, probabilities[answer_hole], precision_vec[answer_hole], measurement_cost, tot_count);

        ld prob_sum = accumulate(probabilities[answer_hole].begin(), probabilities[answer_hole].end(), 0.);
        rep(j, N)
        {
            if (S <= 100)
            {
                probabilities[(answer_hole + 1) % N][j] *= (1. - probabilities[answer_hole][j] / prob_sum * prob_update1);
            }
            else if (S <= 400)
            {
                probabilities[(answer_hole + 1) % N][j] *= (1. - probabilities[answer_hole][j] / prob_sum * prob_update2);
            }
            else
            {
                probabilities[(answer_hole + 1) % N][j] *= (1. - probabilities[answer_hole][j] / prob_sum * prob_update3);
            }
        }
        prob_sum = accumulate(probabilities[(answer_hole + 1) % N].begin(), probabilities[(answer_hole + 1) % N].end(), 0.);
        rep(j, N)
        {
            probabilities[(answer_hole + 1) % N][j] /= prob_sum;
        }
    }
    // return pow(10, 14) / (state->placement_cost + measurement_cost * 1.2 + pow(10, 5));
    int error = 0;
    while (tot_count < 9900)
    {
        auto flow_vec = get_flow2(posteriors);
        // auto flow_vec = get_flow2(probabilities);
        set<int> holes_to_check;
        map<int, int> tmp_dict;
        vec_int tmp_ans = vec_int(N, -1);
        error = 0;
        rep(i, N)
        {
            tmp_ans[i] = flow_vec[i].first;
            tmp_dict[flow_vec[i].first] = i;
            if (flow_vec[i].first != i)
                error++;
        }
        rep(i, N)
        {
            // if (exp(flow_vec[i].second - 10000) > 1. - precision_room)
            if (1. - probabilities[i][flow_vec[i].first] > 1. - precision_room)
            {
                holes_to_check.insert(i);

                // 本来は別の場所に行きたかったはず
                auto iter = max_element(probabilities[i].begin(), probabilities[i].end());
                auto ind = distance(probabilities[i].begin(), iter);
                holes_to_check.insert(tmp_dict[ind]);
            }
        }
        if (holes_to_check.size() == 0)
        {
            /**/
            rep(i, N) for (int j = i + 1; j < N; j++)
            {
                if (i == j)
                    continue;
                // 互いが互いに間違える確率が高かったらアウト
                int exit1 = flow_vec[i].first; // hole iの暫定的なトップの出口
                int exit2 = flow_vec[j].first;
                if (try_swap(i, j, tmp_ans) > posterior_param1 || S == 1 && try_swap(i, j, tmp_ans) > posterior_param2)
                {
                    if (ro.get_rand(0, 2) == 0)
                    {
                        holes_to_check.insert(i);
                    }
                    else
                    {
                        holes_to_check.insert(j);
                    }
                }
            }
        }
        if (holes_to_check.size() == 0)
        {
            break;
        }

        for (auto hole : holes_to_check)
        {
            ld precision = 1. - (1. - precision_vec[hole]) / 2.;
            precision_vec[hole] = precision;
            measure_until_clear(hole, probabilities[hole], precision_vec[hole], measurement_cost, tot_count);
        }
    }
    ld score = pow(10, 14) / (state->placement_cost + measurement_cost + pow(10, 5));
    if (tot_count < 9990)
    {
        error = 0;
    }

    return score * pow(0.8, error);
}

void ScorePredictor::measure_until_clear(int answer_hole, vector<ld> &probabilities, ld precision, ld &measurement_cost, int &tot_count)
{
    int first = -1;
    int second = -1;
    int count = 0;
    while (tot_count < 10000)
    {

        int dx, dy;
        ld max_diff = -1;
        if (count == 0)
        {
            dx = 0;
            dy = 0;
        }
        else
        {
            tie(dx, dy) = best_pos_to_search[first][second];
        }
        measurement_cost += 100 * (10 + abs(dx) + abs(dy));
        count++;
        tot_count++;
        int measured = measure(answer_hole, first, dx, dy);
        ld prob_sum = update_probability_for_all(probabilities, dx, dy, measured, answer_hole);
        set_first_second(probabilities, first, second);

        if (probabilities[first] > precision || tot_count >= 10000)
        {
            if (first != answer_hole)
            {
                error_dict[make_pair(first, answer_hole)]++;
            }
            break;
        }
    }
}



class Estimator
{
private:
    State *state;
    int make_a_query(int hole_id, int dx, int dy);
    ld calc_probability(int original_value, int observation);
    void update_probability_for_all(vector<ld> &probabilities, int dx, int dy, int observation, vector<pair<ld, int>> &candidates, int hole);
    vec_int used_holes = vec_int(N, 0);
    double calculate_score(vec_int &solution);
    double calc_hole_score(int hole_id, int sol);

public:
    Estimator(State *state) : state(state){};
    vector<vector<tuple<int, int, int>>> observations = vector<vector<tuple<int, int, int>>>(N, vector<tuple<int, int, int>>());
    int tot_query_count = 0;
    void query_and_estimate(int hole, vector<ld> &probabilities, ld precision, bool initial);
    vector<int> sa(vector<int> initial_solution);
    double try_swap(int hole_id1, int hole_id2, vector<int> &solution);
    vector<vector<ld>> posteriors = vector<vector<ld>>(N, vector<ld>(N, 0.));
};

int Estimator::make_a_query(int hole_id, int dx, int dy)
{
    tot_query_count++;
    cout << hole_id << " " << dx << " " << dy << endl;
    int result;
    cin >> result;
    observations[hole_id].push_back(make_tuple(result, dx, dy));
    return result;
}

ld Estimator::calc_probability(
    int mu, int observation)
{
    ld result = 1.;
    if (observation == 0)
    {
        result *= (1. - erf_cache[mu]) / 2.;
    }
    else if (observation == 1000)
    {
        result *= (1. - erf_cache[1000 - mu]) / 2.;
    }
    else
    {
        result *= exp_cache[abs(mu - observation)];
    }
    return result;
}

void Estimator::update_probability_for_all(
    vector<ld> &probabilities,
    int dx, int dy, int observation,
    vector<pair<ld, int>> &candidates, int hole)
{
    // rep(i, N)
    for (auto tmp : candidates)
    {
        int exit_id = tmp.second;
        int original_value = state->get_val(X[exit_id], Y[exit_id], dx, dy);

        ld prob = calc_probability(original_value, observation);
        // cerr << "prob:" << prob << endl;
        probabilities[exit_id] *= prob;
        posteriors[hole][exit_id] += log(prob);
        // cerr << "posterior:" << posteriors[hole][exit_id] << endl;
    }
}

vector<pair<int, ll>> get_flow(vector<vector<ld>> &posteriors)
{
    /*
    vector<int> result(N, -1);
    vector<int> assignment(N, -1);
    vector<vector<pair<ld, int>>> vecs;
    rep(i, N)
    {
        auto vec = vector<pair<ld, int>>();
        rep(j, N)
        {
            vec.push_back(make_pair(posteriors[i][j], j));
        }
        sort(vec.begin(), vec.end());
        vecs.push_back(vec);
    }

    rep(i, N)
    {
        int exit;
        for (int j = 0; j < N; j++)
        {
            exit = vecs[i][j].second;
            if (assignment[exit] != -1)
                continue;
        }
        result[i] = exit;
        assignment[exit] = i;
    }
    while (true)
    {
        int flag = 0;
        rep(i, N)
        {
            rep(j, N)
            {
                int before = posteriors[i][result[i]] + posteriors[j][result[j]];
                int after = posteriors[i][result[j]] + posteriors[j][result[i]];
                if (after > before)
                {
                    swap(assignment[result[i]], assignment[result[j]]);
                    swap(result[i], result[j]);
                    flag = 1;
                }
            }
        }

        if (flag == 0)
            break;
    }

    vector<pair<int, ld>> result2;
    rep(i, N)
    {
        result2.push_back(make_pair(result[i], posteriors[i][result[i]]));
    }
    return result2;
    */

    mcf_graph<int, ll> graph(2 * N + 2);
    rep(i, N)
    {
        graph.add_edge(2 * N, i, 1, 0.);
        graph.add_edge(i + N, 2 * N + 1, 1, 0.);
        auto iter = max_element(posteriors[i].begin(), posteriors[i].end());
        ld max_posterior = *iter;

        rep(j, N)
        {
            ll cost;
            if (posteriors[i][j] < max_posterior - 30)
                continue;
            // cerr << i << " " << j << " " << posteriors[i][j] << endl;
            if (isfinite(-posteriors[i][j]))
            {
                cost = (ll) - (posteriors[i][j] * 100000);
            }
            else
            {
                cost = 1000000000;
            }

            // cost = -probabilities[i][j];
            // cost = max(-posteriors[i][j], (ld)0);
            graph.add_edge(i, j + N, 1, cost);
        }
    }
    // 流せるだけ流す
    auto tmp = graph.flow(2 * N, 2 * N + 1);
    vector<pair<int, ll>> result(N);
    auto edges = graph.edges();
    for (auto edge : edges)
    {
        if (edge.flow == 1 && edge.from != 2 * N && edge.to != 2 * N + 1)
        {
            int from = edge.from;
            int to = edge.to - N;
            result.at(from) = make_pair(to, edge.cost);
        }
    }
    return result;
}

void normalize(vector<ld> &probabilities, ld top_value)
{
    rep(i, N)
    {
        probabilities[i] /= top_value;
        if (!isfinite(probabilities[i]))
            probabilities[i] = 0.;
    }
}

void Estimator::query_and_estimate(int hole_id, vector<ld> &probabilities, ld precision, bool initial)
{
    int max_query = (10000 - tot_query_count) / (N - hole_id);
    // vector<ld> probabilities(N, 1.);
    vector<int> query_counts(9, 0);
    vector<pair<ld, int>> candidates;
    rep(i, N)
    {
        candidates.push_back(make_pair(probabilities[i], i));
    }

    sort(candidates.begin(), candidates.end());
    reverse(candidates.begin(), candidates.end());
    // 初回では全部に対してクエリを投げておく
    int count = 0;
    int dx, dy;
    int prev_dx = 0;
    int prev_dy = 0;
    while (tot_query_count < 10000 && (!initial || count < max_query))
    {

        if (count == 0 && initial)
        {
            dx = 0;
            dy = 0;
        }
        else
        {
            int hole1 = candidates[0].second;
            int hole2 = candidates[1].second;
            int hole3 = candidates[2].second;
            int max_dx = INT_MAX;
            int max_dy = INT_MAX;
            double max_diff = -1;
            for (int i = -L / 2 - 1; i <= L / 2 + 1; i++)
            {
                for (int j = -L / 2 - 1; j <= L / 2 + 1; j++)
                {
                    int cost = abs(i) + abs(j) + 10;
                    /*
                    if (S < 9 && cost > 16)
                        continue;
                        */
                    if (S == 1 && cost > 14)
                        continue;
                    double diff1 = abs(state->get_val(X[hole1], Y[hole1], i, j) - state->get_val(X[hole2], Y[hole2], i, j));
                    double diff2 = abs(state->get_val(X[hole1], Y[hole1], i, j) - state->get_val(X[hole3], Y[hole3], i, j));
                    double diff = diff1 * diff1 * candidates[1].first + diff2 * diff2 * candidates[2].first;
                    // double diff = diff1;

                    // diff = diff / (cost * 1.2);
                    diff = diff / ((ld)cost * cost_penalty);
                    if (max_diff < diff)
                    {
                        max_diff = diff;
                        max_dx = i;
                        max_dy = j;
                    }
                }
            }
            dx = max_dx;
            dy = max_dy;
        }
        prev_dx = dx;
        prev_dy = dy;

        /*
        rep(i, 6)
        {
            // cout << "#" << i << ":" << candidates[i].second << " prob:" << candidates[i].first << endl;
        }
        */
        int val = make_a_query(hole_id, dx, dy);
        update_probability_for_all(probabilities, dx, dy, val, candidates, hole_id);
        ld prob_sum = accumulate(probabilities.begin(), probabilities.end(), 0.);
        normalize(probabilities, prob_sum);
        ld probability_sum = 0.;
        for (int i = 0; i < candidates.size(); i++)
        {
            int id = candidates[i].second;
            candidates[i] = make_pair(probabilities[id], id);
            probability_sum += probabilities[id];
        }

        sort(candidates.begin(), candidates.end());
        reverse(candidates.begin(), candidates.end());

        if (candidates[0].first / probability_sum > precision)
        {
            break;
        }

        count++;
    }
    return;
}

double Estimator::calc_hole_score(int hole, int exit)
{
    return posteriors[hole][exit];
    /*
    double result = 0;
    for (auto tmp : observations[sol])
    {
        auto [measured_value, dx, dy] = tmp;
        int original_value = state->get_val(X[hole_id], Y[hole_id], dx, dy);
        ld prob = calc_probability(original_value, measured_value);
        result += log(prob);
    }
    return result;
    */
}

double Estimator::calculate_score(vector<int> &solution)
{
    double score = 0;
    rep(hole_id, N)
    {
        score += calc_hole_score(hole_id, solution[hole_id]);
    }
    return score;
}

// ここは定義が混乱しているので注意
double Estimator::try_swap(int hole1, int hole2, vector<int> &solution)
{
    double result = 0;
    result -= calc_hole_score(hole1, solution[hole1]);
    result -= calc_hole_score(hole2, solution[hole2]);

    result += calc_hole_score(hole1, solution[hole2]);
    result += calc_hole_score(hole2, solution[hole1]);
    return result;
}

vector<int> Estimator::sa(vector<int> initial_solution)
{

    vec_int solution = initial_solution;
    vec_int best_solution = solution;

    double score = calculate_score(solution);
    cerr << "initial score:" << score << endl;

    while (true)
    {
        float ct = get_time(false);
        if (ct > TIME_LIMIT * 0.95)
            break;
        int hole1 = ro.get_rand(0, N);
        int hole2 = ro.get_rand(0, N - 1);
        hole2 += hole2 >= hole1 ? 1 : 0;
        double diff = try_swap(hole1, hole2, solution);
        if (diff > 0)
        {
            swap(solution[hole1], solution[hole2]);
            best_solution = solution;
            score += diff;

            cerr << "score:" << score << " " << calculate_score(solution) << endl;
        }
    }

    return best_solution;
}



void get_input()
{
    cerr << "get input" << endl;
    cin >> L >> N >> S;
    X = vec_int(N);
    Y = vec_int(N);
    rep(i, N) cin >> X.at(i) >> Y.at(i);
    cerr << L << " " << N << " " << S << endl;
}

int main(int argc, char *argv[])
{
#ifdef OPTUNA_STUDY
    cerr << "optuna study, override parameters" << endl;
    // T0 = stod(argv[1]);
    // T1 = stod(argv[2]);
    sigma_div = (ld)stod(argv[1]);
    sigma_div2 = (ld)stod(argv[2]);
    S_threshold = (int)stoi(argv[3]);
    precision_room1 = (ld)stod(argv[4]);
    precision_room2 = (ld)stod(argv[5]);
    precision_room3 = (ld)stod(argv[6]);
    precision_room4 = (ld)stod(argv[7]);
    prob_update1 = (ld)stod(argv[8]);
    prob_update2 = (ld)stod(argv[9]);
    prob_update3 = (ld)stod(argv[10]);
    posterior_param1 = (ld)stod(argv[11]);
    posterior_param2 = (ld)stod(argv[12]);
    cost_penalty = (ld)stod(argv[13]);
#endif
    get_time(true);
    std::ios::sync_with_stdio(false);
    cin.tie(0);
    cerr << "start" << endl;
    get_input();
    calc_exp_value();
    if (S > 100)
    {
        T0 = max(4 * S, S * S / 25);
    }
    else
    {
        T0 = 1;
    }
    T1 = 0;

    dx_vec = {(int)-sqrt(L), 0, (int)sqrt(L)};
    dy_vec = {(int)-sqrt(L), 0, (int)sqrt(L)};

    vector<vector<ld>> probabilities(N, vector<ld>(N, 1. / (ld)N));
    auto state = State(true);
    if (S < 4)
    {
        // state = State(0);
        auto best_state = State(true);
        double best_score = 0;
        int best_height = -1;
        vector<ld> best_scores;
        int cnt = 0;
        while (true)
        {
            cnt++;
            if (get_time(false) > 2500)
                break;
            // int max_val = S < 200 ? 500 : 1000;
            // int height = State::determine_height() * (0.5 + ro.get_float() * 1.5);
            // height = max(S, height);
            // height = min(1000, height);
            int height = ro.get_rand(10, min(1000, L * 6));
            if (cnt == 1)
            {
                height = max(min(S * 5, 1000), (int)(L * 4.0 * sqrt(sqrt(S))));
            }
            double score_sum = 0;
            auto tmp_state = State(height);
            auto predictor = ScorePredictor(&tmp_state);
            vector<ld> scores;
            rep(i, 10)
            {
                if (get_time(false) > 2500)
                {
                    goto OUT1;
                }
                ld score = predictor.predict_score();
                score_sum += score;
                scores.push_back(score);
            }
            if (best_score < score_sum)
            {
                best_score = score_sum;
                best_state = tmp_state;
                best_height = height;
                best_scores = scores;
            }
            // cerr << "score_sum:" << score_sum << " height:" << height << endl;
        }
    OUT1:
        state = best_state;
        cerr << cnt << " "
             << "best_height:" << best_height << " best_score:" << best_score / best_scores.size() << endl;
        for (auto val : best_scores)
        {
            cerr << val << " ";
        }
        cerr << endl;
    }
    else if (S > S_threshold) // 600くらい
    {
        auto best_state = State(true);
        double best_score = 0;
        vector<ld> best_scores;
        int cnt = 0;
        while (true)
        {
            cnt++;
            if (get_time(false) > 2500)
                break;

            double score_sum = 0;
            auto tmp_state = State(1000, 0, 0, ro.get_rand(0, 3) - 1, ro.get_rand(0, 3) - 1);
            auto predictor = ScorePredictor(&tmp_state);
            vector<ld> scores;
            rep(i, 10)
            {
                if (get_time(false) > 2500)
                {
                    goto OUT2;
                }

                ld score = predictor.predict_score();
                score_sum += score;
                scores.push_back(score);
            }
            if (best_score < score_sum)
            {
                best_score = score_sum;
                best_state = tmp_state;
                best_scores = scores;
            }
            // cerr << "score_sum:" << score_sum << " height:" << height << endl;
        }
    OUT2:
        state = best_state;
        cerr << cnt << " "
             << " best_score:" << best_score / best_scores.size() << endl;
        for (auto val : best_scores)
        {
            cerr << val << " ";
        }
        cerr << endl;
    }
    else
    {
        auto best_state = State(true);
        double best_score = 0;
        int best_height = -1;
        vector<ld> best_scores;
        int cnt = 0;
        int default_height = State::determine_height();
        while (true)
        {
            cnt++;
            if (get_time(false) > 2500)
                break;

            // int max_val = S < 200 ? 500 : 1000;
            // int height = State::determine_height() * (0.5 + ro.get_float() * 1.5);
            // height = max(S, height);
            // height = min(1000, height);
            int height = ro.get_rand(S * 1.2, min(1000, 20 * S));
            if (height < default_height * 0.5 || height > default_height * 2)
                continue;

            if (cnt == 1)
            {
                height = default_height;
            }
            double score_sum = 0;
            auto tmp_state = State(height);
            auto predictor = ScorePredictor(&tmp_state);
            vector<ld> scores;
            rep(i, 10)
            {
                if (get_time(false) > 2500)
                {
                    goto OUT3;
                }

                ld score = predictor.predict_score();
                score_sum += score;
                scores.push_back(score);
            }
            if (best_score < score_sum)
            {
                best_score = score_sum;
                best_state = tmp_state;
                best_height = height;
                best_scores = scores;
            }
            // cerr << "score_sum:" << score_sum << " height:" << height << endl;
        }
    OUT3:
        state = best_state;
        cerr << cnt << " "
             << "best_height:" << best_height << " best_score:" << best_score / best_scores.size() << endl;
        for (auto val : best_scores)
        {
            cerr << val << " ";
        }
        cerr << endl;
    }
    /*
    auto predictor = ScorePredictor(&state);
    ld score = 0;
    rep(i, 100)
    {
        score += predictor.predict_score();
    }
    */

    state.output();

    auto estimator = Estimator(&state);
    // double precision_room = S < 400 ? precision_room1 : precision_room2;
    double precision_room;
    if (S == 1)
    {
        precision_room = precision_room4;
    }
    else if (S <= 100)
    {
        precision_room = precision_room1;
    }
    else if (S <= 400)
    {

        precision_room = precision_room2;
    }
    else
    {
        precision_room = precision_room3;
    }
    vector<ld> precision_vec(N, precision_room);
    rep(i, N)
    {
        probabilities[(i + 1) % N] = probabilities[i];

        estimator.query_and_estimate(i, probabilities[i], precision_vec[i], true);

        ld prob_sum = accumulate(probabilities[i].begin(), probabilities[i].end(), 0.);
        rep(j, N)
        {
            if (S <= 100)
            {
                probabilities[(i + 1) % N][j] *= (1. - probabilities[i][j] / prob_sum * prob_update1);
            }
            else if (S <= 400)
            {
                probabilities[(i + 1) % N][j] *= (1. - probabilities[i][j] / prob_sum * prob_update2);
            }
            else
            {
                probabilities[(i + 1) % N][j] *= (1. - probabilities[i][j] / prob_sum * prob_update3);
            }
        }
        prob_sum = accumulate(probabilities[(i + 1) % N].begin(), probabilities[(i + 1) % N].end(), 0.);
        rep(j, N)
        {
            probabilities[(i + 1) % N][j] /= prob_sum;
        }
    }

    /*
    rep(i, N)
    {
        probabilities[(i + 1) % N] = probabilities[i];
        estimator.query_and_estimate(i, probabilities[i], precision_vec[i]);
        ld prob_sum = accumulate(probabilities[i].begin(), probabilities[i].end(), 0.);
        rep(j, N)
        {
            probabilities[(i + 1) % N][j] *= (1. - probabilities[i][j] / prob_sum * 0.8);
        }
        prob_sum = accumulate(probabilities[(i + 1) % N].begin(), probabilities[(i + 1) % N].end(), 0.);
        rep(j, N)
        {
            probabilities[(i + 1) % N][j] /= prob_sum;
        }
    }
    */

    int ct = 0;
    vector<pair<int, ll>> flow_vec;
    while (estimator.tot_query_count < 9900)
    {
        float ct = get_time(false);
        flow_vec = get_flow(estimator.posteriors);
        if (ct > 3800)
            break;
        // auto flow_vec = get_flow(estimator.posteriors);
        vec_int tmp_ans = vec_int(N, -1);
        for (int i = 0; i < N; i++)
        {
            tmp_ans[i] = flow_vec[i].first;
        }

        set<int> holes_to_check;
        map<int, int> tmp_dict;
        rep(i, N)
        {
            tmp_dict[flow_vec[i].first] = i;
        }
        rep(i, N)
        {
            // if (exp(flow_vec[i].second - 10000) > 1. - precision_room)
            /**/
            if (1. - probabilities[i][flow_vec[i].first] > 1. - precision_room)
            {

                // 本来は別の場所に行きたかったはず
                auto iter = max_element(probabilities[i].begin(), probabilities[i].end());
                auto ind = distance(probabilities[i].begin(), iter);
                holes_to_check.insert(i);
                // holes_to_check.insert(tmp_dict[ind]);
            }
        }
        rep(i, N) for (int j = i + 1; j < N; j++)
        {
            // 互いが互いに間違える確率が高かったらアウト
            int exit1 = flow_vec[i].first; // hole iの暫定的なトップの出口
            int exit2 = flow_vec[j].first;
            ld prob = estimator.try_swap(i, j, tmp_ans);
            if (prob > posterior_param1 || (S == 1 && prob > posterior_param2))
            {
                if (ro.get_rand(0, 2) == 0)
                {
                    holes_to_check.insert(i);
                }
                else
                {
                    holes_to_check.insert(j);
                }
            }
        }

        ct = get_time(false);
        if (holes_to_check.size() == 0)
            break;
        for (auto hole : holes_to_check)
        {
            ld precision;
            if (ct < 3200)
            {
                precision = 1. - (1. - precision_vec[hole]) * 1. / 2.;
            }
            else
            {
                precision = 1. - (1. - precision_vec[hole]) * 1. / 4.;
            }
            precision_vec[hole] = precision;
            estimator.query_and_estimate(hole, probabilities[hole], precision_vec[hole], false);
        }
    }
    // auto flow_vec2 = get_flow(estimator.posteriors);
    vec_int ans = vec_int(N, -1);
    for (int i = 0; i < N; i++)
    {
        ans[i] = flow_vec[i].first;
    }

    // ans = estimator.sa(ans);

    cerr
        << "time:" << get_time(false) << endl;
    cout << "-1 -1 -1" << endl;
    rep(i, N)
    {
        cout << ans[i] << endl;
    }

    return 0;
}
