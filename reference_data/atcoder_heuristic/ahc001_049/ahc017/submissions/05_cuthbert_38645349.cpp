// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")
// #pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2")  // SIMD


#pragma GCC optimize("unroll-loops")

#undef _GLIBCXX_DEBUG                 // disable run-time bound checking, etc
#pragma GCC optimize("Ofast,inline")  // Ofast = O3,fast-math,allow-store-data-races,no-protect-parens

#pragma GCC target("bmi,bmi2,lzcnt,popcnt")                       // bit manipulation
#pragma GCC target("movbe")                                       // byte swap
#pragma GCC target("aes,pclmul,rdrnd")                            // encryption
#pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2")  // SIMD
                                                                  //
#define NDEBUG

// #define FLAG_PRINT_SCORE
// #define FLAG_PRINT_SCORE_END
// #define FLAG_PRINT_NN

// ---------------------------------------- template ----------------------------------------

// #define _GLIBCXX_DEBUG
#include <bits/stdc++.h>
#include <x86intrin.h>

using namespace std;
using ll = long long;
using ull = unsigned long long;

const int N_MAX = 1000;
const int INF = 1000000000;
const ll INFLL = 1ll << 60;
// const int TIME_LIMIT = 59700000;
const int TIME_LIMIT = 5980;
// const int TIME_LIMIT = 3970;


template <class T>
bool chmin(T& a, const T& b) {
    if (a > b) {
        a = b;
        return 1;
    } else
        return 0;
}
template <class T>
bool chmax(T& a, const T& b) {
    if (a < b) {
        a = b;
        return 1;
    } else
        return 0;
}

auto start = chrono::system_clock::now();
int get_time() {
    int elapsed = (int)(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start).count());
    return elapsed;
}
int get_time_remaining() {
    int time_remaining = TIME_LIMIT - get_time();
    return time_remaining;
}
void print_time() {
    int elapsed = get_time();
    cerr << "total time: " << elapsed << endl;
}

// ---------------------------------------- all classes ----------------------------------------
class Edge {
public:
    int idx;
    int u, v;
    ll dist;

    Edge() = default;
    ~Edge() = default;

    Edge(int idx, int u, int v, ll dist);
    int dest(int init);

    bool operator<(const Edge& e) const;
    bool operator>(const Edge& e) const;
    bool operator==(const Edge& e) const;
};


// https://nyaannyaan.github.io/library/data-structure/radix-heap.hpp
// for fast dijkstra

template <typename Key, typename Val>
struct RadixHeap {
    using uint = typename make_unsigned<Key>::type;
    static constexpr int bit = sizeof(Key) * 8;
    array<vector<pair<uint, Val>>, bit + 1> vs;
    array<uint, bit + 1> ms;

    int s;
    uint last;

    RadixHeap() : s(0), last(0) { fill(begin(ms), end(ms), uint(-1)); }

    bool empty() const { return s == 0; }

    int size() const { return s; }

    __attribute__((target("lzcnt"))) inline uint64_t getbit(uint a) const {
        return 64 - _lzcnt_u64(a);
    }

    void push(const uint& key, const Val& val) {
        s++;
        uint64_t b = getbit(key ^ last);
        vs[b].emplace_back(key, val);
        ms[b] = min(key, ms[b]);
    }

    pair<uint, Val> pop() {
        if (ms[0] == uint(-1)) {
            int idx = 1;
            while (ms[idx] == uint(-1))
                idx++;
            last = ms[idx];
            for (auto& p : vs[idx]) {
                uint64_t b = getbit(p.first ^ last);
                vs[b].emplace_back(p);
                ms[b] = min(p.first, ms[b]);
            }
            vs[idx].clear();
            ms[idx] = uint(-1);
        }
        --s;
        auto res = vs[0].back();
        vs[0].pop_back();
        if (vs[0].empty())
            ms[0] = uint(-1);
        return res;
    }
};

// ---------------------------------------- all functions ----------------------------------------
static uint32_t randxor();
int RandomRange(int n);
vector<int> RandomPermutation(int n);
void read_input();
void initialize();
bool check_ans(const vector<int>& ans);
void print_ans(const vector<int>& ans);
// vector<ll> calc_score_vec(const vector<int>& ans);
ll calc_score(const vector<int>& ans, int NN = -1);
vector<ll> calc_score_lowerbound_vec();
ll calc_score_lowerbound();
pair<ll, vector<Edge*>> DualDijkstraSlow0128(const int& s, const int& t, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse);
vector<ll> DijkstraSlow0128(const int& s, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse = vector<bool>());
vector<ll> DijkstraFast0128(const int& s, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse = vector<bool>());
vector<ll> DijkstraFastFast0128(const int& s, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse = vector<bool>());
vector<ll> DijkstraRadixHeap0128(const int& s, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse = vector<bool>());

void solve0128_1();
vector<int> solve0128_2();
vector<int> solve0129_1();

// ---------------------------------------- random ----------------------------------------

static uint32_t randxor() {
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;

    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

int RandomRange(int n) {
    assert(0 <= n);
    // return (randxor() / 123) % n;
    return randxor() % n;
}

vector<int> RandomPermutation(int n) {
    assert(0 <= n);
    vector<int> V(n);
    iota(V.begin(), V.end(), 0);

    vector<int> ret(n);
    for (int i = n - 1; i >= 0; i--) {
        int r = RandomRange(i + 1);
        ret[i] = V[r];
        if (r != i)
            swap(V[i], V[r]);
    }
    return ret;
}

// ---------------------------------------- classes ----------------------------------------


struct UnionFind {
    vector<int> data;

    UnionFind(int sz) {
        data.assign(sz, -1);
    }

    bool unite(int x, int y) {
        x = find(x), y = find(y);
        if (x == y)
            return (false);
        if (data[x] > data[y])
            swap(x, y);
        data[x] += data[y];
        data[y] = x;
        return (true);
    }

    int find(int k) {
        if (data[k] < 0)
            return (k);
        return (data[k] = find(data[k]));
    }

    bool same(int x, int y) {
        return find(x) == find(y);
    }

    int size(int k) {
        return (-data[find(k)]);
    }
};

// Edge::Edge() = default;
Edge::Edge(int idx, int u, int v, ll dist) : idx(idx), u(u), v(v), dist(dist) {
    if (u > v)
        swap(u, v);
}

int Edge::dest(int init) {
    assert(init == u || init == v);
    return init ^ u ^ v;
}

bool Edge::operator<(const Edge& e) const {
    if (dist == e.dist)
        return idx < e.idx;
    else
        return dist < e.dist;
}
bool Edge::operator>(const Edge& e) const {
    if (dist == e.dist)
        return idx > e.idx;
    else
        return dist > e.dist;
}
bool Edge::operator==(const Edge& e) const {
    return idx == e.idx;
}
// };


// ---------------------------------------- variables ----------------------------------------

int N, M, D, K;
vector<int> U, V;
vector<ll> W;
vector<int> X, Y;

vector<int> SquareDistanceFromCenter;

// Edge E[4000];
vector<Edge> Edges;
vector<vector<Edge*>> Graph;
vector<vector<Edge*>> Mat;
vector<vector<ll>> MinDistMatrix;
vector<ll> MinDistMatrixSum;
vector<Edge*> EdgesSorted;
vector<int> VertexSorted;
vector<vector<ll>> MDistInitial;

// ---------------------------------------- basic functions ----------------------------------------

void read_input() {
    cin >> N >> M >> D >> K;
    cerr << N << " " << M << " " << D << " " << K << endl;
    U.resize(M);
    V.resize(M);
    W.resize(M);
    for (int i = 0; i < M; i++) {
        cin >> U[i] >> V[i] >> W[i];
        U[i]--;
        V[i]--;
    }
    X.resize(N);
    Y.resize(N);
    for (int i = 0; i < N; i++) {
        cin >> X[i] >> Y[i];
        X[i] -= 500;
        Y[i] -= 500;
    }
}

void initialize() {
    Edges.resize(M);
    Graph.resize(N);
    Mat.assign(N, vector<Edge*>(N, nullptr));
    for (int i = 0; i < M; i++) {
        Edges[i] = Edge{i, U[i], V[i], W[i]};
        Graph[Edges[i].u].emplace_back(&Edges[i]);
        Graph[Edges[i].v].emplace_back(&Edges[i]);
        // cerr << Edges[i].u << " " << Edges[i].v << endl;
        Mat[Edges[i].u][Edges[i].v] = &Edges[i];
        Mat[Edges[i].v][Edges[i].u] = &Edges[i];
    }

    MinDistMatrix.assign(N, vector<ll>(N));
    MinDistMatrixSum.resize(N, 0ll);
    MDistInitial.resize(N);
    for (int i = 0; i < N; i++) {
        auto Vdist = DijkstraFastFast0128(i, Graph);
        MDistInitial[i] = Vdist;
        for (int j = 0; j < N; j++) {
            MinDistMatrix[i][j] = Vdist[j];
            MinDistMatrixSum[i] += Vdist[j];
        }
    }

    SquareDistanceFromCenter.resize(N);
    for (int i = 0; i < N; i++) {
        SquareDistanceFromCenter[i] = X[i] * X[i] + Y[i] * Y[i];
        // SquareDistanceFromCenter[i] = (X[i] + 500) * (X[i] + 500) + Y[i] * Y[i];
    }
    {
        EdgesSorted.resize(M);
        vector<pair<int, int>> tmp(M);
        for (int i = 0; i < M; i++) {
            Edge* e = &Edges[i];
            tmp[i] = {SquareDistanceFromCenter[e->u] + SquareDistanceFromCenter[e->v], i};
            // tmp[i] = {X[e->u] + X[e->v], i};
            // tmp[i] = {(X[e->u] + 500) * (X[e->u] + 500) + (X[e->v] + 500) * (X[e->v] + 500), i};
            // tmp[i] = {min(SquareDistanceFromCenter[e->u], SquareDistanceFromCenter[e->v]), i};
        }
        sort(tmp.begin(), tmp.end());
        // sort(tmp.begin(), tmp.end(), greater{});
        for (int i = 0; i < M; i++) {
            EdgesSorted[i] = &Edges[tmp[i].second];
        }
    }

    {
        VertexSorted.resize(N);
        vector<pair<int, int>> tmp(N);
        for (int i = 0; i < N; i++) {
            // if (i < 10)
            //     tmp[i] = {0, i};
            // else
            tmp[i] = {SquareDistanceFromCenter[i], i};
            // tmp[i] = {SquareDistanceFromCenter[i] + RandomRange(1000), i};
            // tmp[i] = {i, i};
        }
        sort(tmp.begin(), tmp.end());
        for (int i = 0; i < N; i++) {
            VertexSorted[i] = tmp[i].second;
            // if (i % 2 == 0)
            //     VertexSorted[i] = tmp[i / 2].second;
            // else
            //     VertexSorted[i] = tmp[N - 1 - i / 2].second;
        }
    }
}

// ---------------------------------------- medium functions ----------------------------------------


bool check_ans(const vector<int>& ans) {
    assert((int)ans.size() == M);
    vector<int> cnt(D);
    for (auto&& d : ans) {
        if (d < 0 || D <= d) {
            cerr << "d in not range [0,D)" << endl;
            return false;
        }
        cnt[d]++;
    }
    if (*max_element(cnt.begin(), cnt.end()) > K) {
        cerr << "too much edges in one day" << endl;
        return false;
    }
    return true;
}

void print_ans(const vector<int>& ans) {
    if (!check_ans(ans)) {
        cerr << "invalid answer" << endl;
    }
    for (auto&& d : ans) {
        cout << d + 1 << " ";
    }
    cout << endl;
}

// vector<ll> calc_score_vec(const vector<int>& ans) {
ll calc_score(const vector<int>& ans, int NN) {
    if (NN < 0)
        NN = N;
    if (!check_ans(ans)) {
        cerr << "invalid answer" << endl;
    }
    vector<vector<int>> day_edge(D);
    for (int i = 0; i < M; i++) {
        int d = ans[i];
        if (d >= 0)
            day_edge[d].emplace_back(i);
    }

    vector<ll> scores(M);
    vector<bool> edge_nouse(M, false);
    ll score_raw = 0;
    for (int d = 0; d < D; d++) {
        for (auto&& idx : day_edge[d]) {
            edge_nouse[idx] = true;
        }


        ll dscore_raw = 0;
        // for (int i = 0; i < N; i++) {
        vector<int> Perm;
        if (NN == N) {
            Perm.resize(M);
            iota(Perm.begin(), Perm.end(), 0);
        } else
            Perm = RandomPermutation(N);
        for (int k = 0; k < NN; k++) {
            int i = Perm[k];

            // auto Vdist = DijkstraSlow0128(i, Graph, edge_nouse);
            auto Vdist = DijkstraFastFast0128(i, Graph, edge_nouse);
            // auto Vdist = DijkstraRadixHeap0128(i, Graph, edge_nouse);
            for (int j = 0; j < N; j++) {
                // dscore_raw += Vdist[j] - MinDistMatrix[i][j];
                dscore_raw += Vdist[j];
            }
            dscore_raw -= MinDistMatrixSum[i];
        }
        dscore_raw = ll((double)dscore_raw / NN * N + 0.5);

        score_raw += dscore_raw;
        for (auto&& idx : day_edge[d]) {
            edge_nouse[idx] = false;
        }
    }

    ll score = ll((double)score_raw / (N * (N - 1) * D) * 1000 + 0.5);

    return score;
}

void print_score(const vector<int>& ans) {
#ifdef FLAG_PRINT_SCORE
    cerr << "time : " << get_time() << "  score : " << calc_score(ans, N) << endl;
#endif  // FLAG_PRINT_SCORE
    return;
}

// ll calc_score(const vector<int>& ans) {
//     vector<ll> score = calc_score_vec(ans);
//     return accumulate(score.begin(), score.end(), 0ll);
// }

vector<ll> calc_score_lowerbound_vec() {
    static vector<ll> scores;
    if (!scores.empty())
        return scores;

    scores.resize(M);
    vector<bool> edge_nouse(M, false);
    for (int i = 0; i < M; i++) {
        edge_nouse[i] = true;
        Edge* e = &Edges[i];
        scores[i] = DualDijkstraSlow0128(e->u, e->v, Graph, edge_nouse).first;
        edge_nouse[i] = false;
    }
    return scores;
}

ll calc_score_lowerbound() {
    static ll score = 0;
    if (score > 0)
        return score;
    auto score_vec = calc_score_lowerbound_vec();
    score = accumulate(score_vec.begin(), score_vec.end(), 0ll);
    return score;
}

// pair<int, vector<int>> DualDijkstraFast0128(const int& s, const int& t, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse) {
//     static vector<bool> seen(N, false);
//     static vector<ll> Vdist(N, INFLL);
//     static vector<int> bef_start_s(N, -1);
//     static vector<int> bef_start_t(N, -1);

//     using P = tuple<ll, int, bool>;  // dist, vertex, s_start
//     priority_queue<P, vector<P>, greater<P>> que;

//     Vdist[s] = 0;
//     Vdist[t] = 0;
//     que.emplace(0, s, true);
//     que.emplace(0, t, false);

//     while (!que.empty()) {
//         auto [dist_now, now, s_start] = que.top();
//         que.pop();
//         if (seen[now])
//             continue;
//         seen[now] = true;
//     }
// }

pair<ll, vector<Edge*>> DualDijkstraSlow0128(const int& s, const int& t, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse = vector<bool>()) {
    // returns (distance_or_route, route(edge))

    vector<bool> seen(N, false);
    vector<ll> Vdist(N, INFLL);
    vector<Edge*> bef(N, nullptr);

    using P = pair<ll, int>;  // dist, vertex
    priority_queue<P, vector<P>, greater<P>> que;

    Vdist[s] = 0;
    que.emplace(0, s);

    bool is_connected = false;

    while (!que.empty()) {
        auto [dist_now, now] = que.top();
        que.pop();
        if (seen[now])
            continue;
        seen[now] = true;

        if (now == t) {
            is_connected = true;
            break;
        }

        for (auto&& e : Graph[now]) {
            if (!edge_nouse.empty() && edge_nouse[e->idx])
                continue;
            int nxt = e->dest(now);
            if (seen[nxt])
                continue;
            ll dist_nxt = dist_now + e->dist;
            if (chmin(Vdist[nxt], dist_nxt)) {
                bef[nxt] = &(*e);
                que.emplace(dist_nxt, nxt);
            }
        }
    }

    if (!is_connected) {
        return {INFLL, vector<Edge*>()};
    }

    ll dist_total = Vdist[t];

    vector<Edge*> route;
    int v = t;
    while (true) {
        Edge* e = bef[v];
        if (e == nullptr)
            break;
        route.emplace_back(e);
        v = e->dest(v);
    }

    return {dist_total, route};
}

vector<ll> DijkstraSlow0128(const int& s, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse) {
    vector<bool> seen(N, false);
    vector<ll> Vdist(N, (ll)INF);

    using P = pair<ll, int>;  // dist, vertex
    priority_queue<P, vector<P>, greater<P>> que;

    Vdist[s] = 0;
    que.emplace(0, s);

    while (!que.empty()) {
        auto [dist_now, now] = que.top();
        que.pop();
        if (seen[now])
            continue;
        seen[now] = true;


        for (auto&& e : Graph[now]) {
            if (!edge_nouse.empty() && edge_nouse[e->idx])
                continue;
            int nxt = e->dest(now);
            if (seen[nxt])
                continue;
            ll dist_nxt = dist_now + e->dist;
            if (chmin(Vdist[nxt], dist_nxt)) {
                que.emplace(dist_nxt, nxt);
            }
        }
    }

    return Vdist;
}


vector<ll> DijkstraFast0128(const int& s, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse) {
    const int bucket_size = 10000;
    const int bucket_num = 1000;

    static vector<bool> seen(N, false);
    vector<ll> Vdist(N, (ll)INF);

    using P = pair<ll, int>;  // dist, vertex

    // priority_queue<P, vector<P>, greater<P>> que;
    // static vector<vector<P>> bucket(bucket_num, vector<P>(1000));
    static P bucket[bucket_num][1000] = {};
    vector<int> Vbucket_cnt(bucket_num, 0);
    // if (bucket.size() == 0) {
    //     bucket.resize(bucket_num);
    //     for (int n = 0; n < bucket_num; n++) {
    //         bucket[n].reserve(1000);
    //     }
    // }
    // bucket.resize(bucket_num);


    Vdist[s] = 0;
    // que.emplace(0, s);
    // bucket[0].emplace_back(0, s);
    bucket[0][Vbucket_cnt[0]++] = {0, s};

    int cnt = 0;
    // int max_bucket = 0;

    for (int n = 0; n < bucket_num; n++) {
        // for (auto&& [dist_now, now] : bucket[n]) {
        for (int i = 0; i < Vbucket_cnt[n]; i++) {
            auto& [dist_now, now] = bucket[n][i];

            if (seen[now] || dist_now > Vdist[now])
                continue;
            seen[now] = true;
            cnt++;
            if (cnt == N)
                break;

            for (auto&& e : Graph[now]) {
                if (!edge_nouse.empty() && edge_nouse[e->idx])
                    continue;
                int nxt = e->dest(now);
                if (seen[nxt])
                    continue;
                ll dist_nxt = dist_now + e->dist;
                if (chmin(Vdist[nxt], dist_nxt)) {
                    // bucket[dist_nxt / bucket_size].emplace_back(dist_nxt, nxt);
                    int nxt_bucket_num = int(dist_nxt / bucket_size);
                    bucket[nxt_bucket_num][Vbucket_cnt[nxt_bucket_num]++] = {dist_nxt, nxt};
                    // chmax(max_bucket, int(dist_nxt / bucket_size));
                }
            }
        }
        if (cnt == N)
            break;
    }

    // cleanup
    // for (int n = 0; n < bucket_num; n++) {
    //     bucket[n].clear();
    // }
    // fill(bucket.begin(), bucket.end(), vector<P>());
    fill(seen.begin(), seen.end(), false);


    return Vdist;
}

ll DijkstraFastSum0128(const int& s, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse) {
    auto Vdist = DijkstraFast0128(s, Graph, edge_nouse);
    return accumulate(Vdist.begin(), Vdist.end(), 0ll);
}

vector<ll> DijkstraFastFast0128(const int& s, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse) {
    const int bucket_size = 10000;
    // const int bucket_size = 1 << 13;
    const int bucket_num = 300;

    vector<ll> Vdist(N, (ll)INF);

    const int Nshift = 10;
    const ll mask = (1 << Nshift) - 1;

    static ll bucket[bucket_num][1000] = {};
    // static array<array<ll, 1000>, bucket_num> bucket{};
    int Vbucket_cnt[bucket_num] = {};

    Vdist[s] = 0;
    bucket[0][Vbucket_cnt[0]++] = s;

    int cnt = 0;

    for (int n = 0; n < bucket_num; n++) {
        for (int i = 0; i < Vbucket_cnt[n]; i++) {
            ll hashed = bucket[n][i];
            ll dist_now = ll(hashed >> Nshift);
            int now = int(hashed & mask);

            if (dist_now != Vdist[now])
                continue;
            cnt++;
            if (cnt == N)
                break;

            for (auto&& e : Graph[now]) {
                if (!edge_nouse.empty() && edge_nouse[e->idx])
                    continue;
                int nxt = e->dest(now);
                ll dist_nxt = dist_now + e->dist;
                if (chmin(Vdist[nxt], dist_nxt)) {
                    int nxt_bucket_num = int(dist_nxt / bucket_size);
                    bucket[nxt_bucket_num][Vbucket_cnt[nxt_bucket_num]++] = (ll(dist_nxt) << Nshift) ^ ll(nxt);
                }
            }
        }
        if (cnt == N)
            break;
    }

    return Vdist;
}

ll DijkstraFastFastSum0128(const int& s, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse) {
    auto Vdist = DijkstraFastFast0128(s, Graph, edge_nouse);
    return accumulate(Vdist.begin(), Vdist.end(), 0ll);
}


vector<ll> DijkstraRadixHeap0128(const int& s, const vector<vector<Edge*>>& Graph, const vector<bool>& edge_nouse) {
    vector<ll> Vdist(N, (ll)INF);

    RadixHeap<ll, int> que;

    Vdist[s] = 0ll;
    que.push(0ll, s);

    while (!que.empty()) {
        auto [dist_now, now] = que.pop();
        if ((ll)dist_now != (ll)Vdist[now])
            continue;

        for (auto&& e : Graph[now]) {
            if (!edge_nouse.empty() && edge_nouse[e->idx])
                continue;
            int nxt = e->dest(now);
            ll dist_nxt = dist_now + e->dist;
            if (chmin(Vdist[nxt], dist_nxt)) {
                que.push(dist_nxt, nxt);
            }
        }
    }

    return Vdist;
}


ll DijkstraCostNoUseFlagUpdate0130(const int& s, const vector<vector<Edge*>>& Graph,
    vector<bool>& edge_nouse, Edge* e, vector<ll>& Vdist, const bool flag_update) {
    // Vdist has to be the distance vector with "Graph" and "edge_nouse"
    // Calculates +cost when not using e

    // if e is not used in current Vdist;
    if (abs(Vdist[e->u] - Vdist[e->v]) != e->dist)
        return 0ll;

    // e is used in current Vdist;

    ll dcost = 0;

    // 識別子
    // static int t = 0;
    // t++;

    int u = (Vdist[e->u] > Vdist[e->v] ? e->u : e->v);  // far end of e

    assert((int)edge_nouse.size() == M);
    assert(edge_nouse[e->idx] == false);
    edge_nouse[e->idx] = true;

    const int bucket_size = 10000;
    // const int bucket_size = 1 << 13;
    const int bucket_num = 200;

    assert((int)Vdist.size() == N);
    // vector<ll> Vdist(N, (ll)INF);
    // if ((int)Vdist.size() == 0)
    //     Vdist.assign(N, (ll)INF);

    const int Nshift = 10;
    const ll mask = (1 << Nshift) - 1;

    static ll bucket[bucket_num][1000] = {};
    // static array<array<ll, 1000>, bucket_num> bucket{};
    int Vbucket_cnt[bucket_num] = {};

    // Vdist[s] = 0;
    // bucket[0][Vbucket_cnt[0]++] = s;

    // check vertex that need to be updated
    int need_update_cnt = 0;
    static int need_update[N_MAX] = {};
    static bool need_update_flag[N_MAX] = {};

    need_update[need_update_cnt++] = u;
    need_update_flag[u] = true;

    int idx = 0;
    while (idx != need_update_cnt) {
        int now = need_update[idx];
        for (auto&& e : Graph[now]) {
            if (edge_nouse[e->idx])
                continue;
            int nxt = e->dest(now);
            if (need_update_flag[nxt])
                continue;
            if (Vdist[now] + e->dist != Vdist[nxt])
                continue;
            need_update[need_update_cnt++] = nxt;
            need_update_flag[nxt] = true;
        }
        idx++;
    }

    static ll Vdist_backup[N_MAX] = {};
    for (int idx = 0; idx < need_update_cnt; idx++) {
        int now = need_update[idx];
        Vdist_backup[idx] = Vdist[now];
        // dcost -= Vdist[now];
    }

    // initialize backet

    // int cnt0 = 0;
    for (int idx = 0; idx < need_update_cnt; idx++) {
        int now = need_update[idx];
        Vdist[now] = (ll)INF;
        // int nxt_best = -1;
        for (auto&& e : Graph[now]) {
            if (edge_nouse[e->idx])
                continue;
            int nxt = e->dest(now);
            if (need_update_flag[nxt])
                continue;
            // if (chmin(dist_best, Vdist[nxt] + e->dist))
            //     nxt_best = nxt;
            chmin(Vdist[now], Vdist[nxt] + e->dist);
        }
        if (Vdist[now] == (ll)INF)
            continue;
        // cnt0++;
        int now_bucket_num = int(Vdist[now] / bucket_size);
        bucket[now_bucket_num][Vbucket_cnt[now_bucket_num]++] = (ll(Vdist[now]) << Nshift) | ll(now);
    }
    // cerr << cnt0 << endl;


    int cnt = 0;

    for (int n = 0; n < bucket_num; n++) {
        for (int i = 0; i < Vbucket_cnt[n]; i++) {
            ll hashed = bucket[n][i];
            ll dist_now = ll(hashed >> Nshift);
            int now = int(hashed & mask);

            if (dist_now != Vdist[now])
                continue;
            // dcost += dist_now;

            cnt++;
            // if (cnt == N)
            //     break;
            need_update_flag[now] = false;

            for (auto&& e : Graph[now]) {
                if (edge_nouse[e->idx])
                    continue;
                int nxt = e->dest(now);
                if (need_update_flag[nxt] == false)
                    continue;
                ll dist_nxt = dist_now + e->dist;
                if (chmin(Vdist[nxt], dist_nxt)) {
                    int nxt_bucket_num = int(dist_nxt / bucket_size);
                    bucket[nxt_bucket_num][Vbucket_cnt[nxt_bucket_num]++] = (ll(dist_nxt) << Nshift) | ll(nxt);
                }
            }
        }
        // if (cnt == N)
        //     break;
    }

    edge_nouse[e->idx] = false;

    for (int idx = 0; idx < need_update_cnt; idx++) {
        int now = need_update[idx];
        need_update_flag[now] = false;
    }
    for (int idx = 0; idx < need_update_cnt; idx++) {
        int now = need_update[idx];
        dcost += Vdist[now];
    }
    for (int idx = 0; idx < need_update_cnt; idx++) {
        dcost -= Vdist_backup[idx];
    }

    if (flag_update == false) {
        for (int idx = 0; idx < need_update_cnt; idx++) {
            int now = need_update[idx];
            Vdist[now] = Vdist_backup[idx];
        }
    }

    // if (cnt != need_update_cnt) {
    //     // 断絶
    // }

    // assert(cnt == need_update_cnt);
    // #ifndef NDEBUG
    //     if (cnt != need_update_cnt)
    //         cerr << cnt << " != " << need_update_cnt << endl;
    // #endif  // !NDEBUG

    return dcost;
}

ll DijkstraCostUseAlwaysUpdate0205(const int& s, const vector<vector<Edge*>>& Graph,
    vector<bool>& edge_nouse, Edge* e, vector<ll>& Vdist) {
    // Vdist has to be the distance vector with "Graph" and "edge_nouse"
    // Calculates -cost when using e

    // if e is not used in current Vdist;
    if (abs(Vdist[e->u] - Vdist[e->v]) < e->dist)
        return 0ll;

    // e is used in current Vdist;

    ll dcost = 0;

    // 識別子
    // static int t = 0;
    // t++;

    int u = (Vdist[e->u] > Vdist[e->v] ? e->u : e->v);  // far end of e
    int v = e->dest(u);                                 // near end of e

    assert((int)edge_nouse.size() == M);
    // assert(edge_nouse[e->idx] == true);

    const int bucket_size = 10000;
    const int bucket_num = 200;

    assert((int)Vdist.size() == N);

    const int Nshift = 10;
    const ll mask = (1 << Nshift) - 1;

    static ll bucket[bucket_num][1000] = {};
    int Vbucket_cnt[bucket_num] = {};

    dcost += Vdist[v] + e->dist - Vdist[u];
    Vdist[u] = Vdist[v] + e->dist;
    int u_bucket_num = int(Vdist[u] / bucket_size);
    bucket[u_bucket_num][Vbucket_cnt[u_bucket_num]++] = (Vdist[u] << Nshift) | ll(u);

    // int cnt = 0;

    // bool seen[N_MAX] = {};

    for (int n = 0; n < bucket_num; n++) {
        for (int i = 0; i < Vbucket_cnt[n]; i++) {
            ll hashed = bucket[n][i];
            ll dist_now = ll(hashed >> Nshift);
            int now = int(hashed & mask);

            if (dist_now != Vdist[now])
                continue;

            for (auto&& e : Graph[now]) {
                if (edge_nouse[e->idx])
                    continue;
                int nxt = e->dest(now);
                ll dist_nxt = dist_now + e->dist;
                ll dist_memo = Vdist[nxt];
                if (chmin(Vdist[nxt], dist_nxt)) {
                    dcost += Vdist[nxt] - dist_memo;
                    int nxt_bucket_num = int(dist_nxt / bucket_size);
                    bucket[nxt_bucket_num][Vbucket_cnt[nxt_bucket_num]++] = (ll(dist_nxt) << Nshift) | ll(nxt);
                }
            }
        }
        // if (cnt == N)
        //     break;
    }

    return dcost;
}

ll cost_edge(Edge* e, const vector<vector<Edge*>>& Graph, vector<bool>& edge_nouse) {
    ll cost = 0;
    edge_nouse[e->idx] = true;
    // auto Vdist1 = DijkstraFast0128(e->u, Graph, edge_nouse);
    // auto Vdist2 = DijkstraFast0128(e->v, Graph, edge_nouse);
    auto Vdist1 = DijkstraFastFast0128(e->u, Graph, edge_nouse);
    auto Vdist2 = DijkstraFastFast0128(e->v, Graph, edge_nouse);
    cost += accumulate(Vdist1.begin(), Vdist1.end(), 0ll);
    cost += accumulate(Vdist2.begin(), Vdist2.end(), 0ll);
    for (int i = 0; i < N; i++) {
        ll dcost = min(Vdist1[i], Vdist2[i] + e->dist) + min(Vdist2[i], Vdist1[i] + e->dist);
        cost -= dcost;
    }
    edge_nouse[e->idx] = false;
    return cost;
}

// ---------------------------------------- advanced functions ----------------------------------------

// ---------------------------------------- solver ----------------------------------------

void solve0128_1() {
    vector<pair<int, vector<Edge*>>> MinimumDistanceAndRouteWhenExcludingEdge(M);
    vector<bool> edge_nouse(M, false);
    for (int i = 0; i < M; i++) {
        edge_nouse[i] = true;
        Edge* e = &Edges[i];
        MinimumDistanceAndRouteWhenExcludingEdge[i] = DualDijkstraSlow0128(e->u, e->v, Graph, edge_nouse);
        edge_nouse[i] = false;
    }

    vector<int> Perm = RandomPermutation(M);

    vector<int> ans(M, -1);
    vector<vector<int>> use_list(D);
    vector<vector<bool>> no_use_flag(D, vector<bool>(M, false));

    using P = tuple<int, int, int>;  // (num_clash, num_increase, num_include)
    for (auto&& idx1 : Perm) {
        vector<P> cost(D);
        auto& route1 = MinimumDistanceAndRouteWhenExcludingEdge[idx1].second;
        for (int d = 0; d < D; d++) {
            auto& [num_clash, num_increase, num_include] = cost[d];
            num_include = 1;

            int dnum_clash = 0;
            int dnum_increase = 0;

            for (auto&& idx2 : use_list[d]) {
                num_include++;

                auto& route2 = MinimumDistanceAndRouteWhenExcludingEdge[idx2].second;
                dnum_clash += find(route1.begin(), route1.end(), &Edges[idx2]) != route1.end();
                dnum_clash += find(route2.begin(), route2.end(), &Edges[idx1]) != route2.end();
            }
            for (auto&& e : route1) {
                if (no_use_flag[d][e->idx])
                    dnum_increase++;
            }

            if (num_include > K)
                dnum_clash = INF;

            num_clash += dnum_clash;
            num_increase += dnum_increase;
        }

        int d = int(min_element(cost.begin(), cost.end()) - cost.begin());
        ans[idx1] = d;

        use_list[d].emplace_back(idx1);
        for (auto&& e : route1) {
            no_use_flag[d][e->idx] = true;
        }
    }

    print_time();
    print_ans(ans);
    print_time();

    vector<ll> scores;
    int cnt = 100;
    for (int i = 0; i < cnt; i++) {
        scores.emplace_back(calc_score(ans, 10));
    }
    double score_ave = (double)accumulate(scores.begin(), scores.end(), 0ll) / cnt;
    double score_var = 0.;
    for (auto&& score : scores) {
        score_var += ((double)score - score_ave) * ((double)score - score_ave) / (cnt - 1);
    }
    double score_std = sqrt(score_var);
    cerr << score_ave << " " << score_std << endl;
    cerr << "ratio : " << score_std / score_ave << endl;


    print_time();
    cerr << "score : " << calc_score(ans) << endl;
    // cerr << "optimal score : " << calc_score_lowerbound() << endl;
    print_time();
}

vector<int> solve0128_2() {
    vector<int> ans(M, -1);
    vector<vector<int>> ans_rev(D);
    vector<int> Perm(M);
    for (int i = 0; i < M; i++) {
        Perm[i] = EdgesSorted[i]->idx;
    }
    // {
    //     vector<pair<int, int>> tmp(M);
    //     for (int i = 0; i < M; i++) {
    //         Edge* e = &Edges[i];
    //         tmp[i] = {SquareDistanceFromCenter[e->u] + SquareDistanceFromCenter[e->v], i};
    //         // tmp[i] = {min(SquareDistanceFromCenter[e->u], SquareDistanceFromCenter[e->v]), i};
    //     }
    //     sort(tmp.begin(), tmp.end());
    //     for (int i = 0; i < M; i++) {
    //         Perm[i] = tmp[i].second;
    //     }
    // }


    // for (auto&& idx : Perm) {
    for (int _ = 0; _ < M; _++) {
        int idx = Perm[_];
        Edge* e = &Edges[idx];
        if (_ == 0 || get_time() > TIME_LIMIT) {
            // if (_ >= 1428) {
            // cerr << _ << endl;
            // if (_ == 0) {
            vector<int> Vcnt(D);
            for (int d = 0; d < D; d++) {
                Vcnt[d] = (int)ans_rev[d].size();

                // 孤立点がないか確認
                bool is_isolated = false;
                for (auto& u : {e->u, e->v}) {
                    int dim = (int)Graph[u].size();
                    int dim_current = 0;
                    for (auto&& f : Graph[u]) {
                        if (ans[f->idx] == -1)
                            continue;
                        dim_current += ans[f->idx] == d;
                    }
                    if (dim - dim_current == 1)
                        is_isolated = true;
                }
                if (is_isolated)
                    Vcnt[d] = INF;
            }
            int d = int(min_element(Vcnt.begin(), Vcnt.end()) - Vcnt.begin());
            ans[idx] = d;
            ans_rev[d].emplace_back(idx);
            continue;
        }

        bool flag_considered_alledge = false;

        // vector<ll> original_cost(D, 0ll);
        // vector<ll> new_cost(D, 0ll);
        vector<ll> diff_cost(D, 0ll);
        for (int d = 0; d < D; d++) {
            if ((int)ans_rev[d].size() >= K) {
                diff_cost[d] = INF;
                continue;
            }
            if (flag_considered_alledge && (int)ans_rev[d].size() == 0) {
                diff_cost[d] = INF;
                continue;
            }
            if ((int)ans_rev[d].size() == 0)
                flag_considered_alledge = true;

            vector<bool> edge_nouse(M, false);
            for (auto&& i : ans_rev[d]) {
                edge_nouse[i] = true;
            }

            diff_cost[d] = cost_edge(e, Graph, edge_nouse);
        }

        int d = 0;
        ll min_cost = diff_cost[0];
        for (int i = 1; i < D; i++) {
            if (chmin(min_cost, diff_cost[i]))
                d = i;
            else if (min_cost == diff_cost[i] && ans_rev[i].size() < ans_rev[d].size())
                d = i;
        }

        // int d = int(min_element(diff_cost.begin(), diff_cost.end()) - diff_cost.begin());

        ans[idx] = d;
        ans_rev[d].emplace_back(idx);
        // cerr << d << " ";
    }
    // cerr << endl;

    return ans;

    // print_ans(ans);
    // print_time();
    // cerr << "score : " << calc_score(ans, N) << endl;
}

vector<int> solve0129_1() {
    auto ans = solve0128_2();
    if (get_time_remaining() < 20) {
        return ans;
    }

    print_score(ans);

    ans.assign(M, -1);

    // naive local search
    // 1つずつ更新

    vector<vector<bool>> Medge_nouse(D, vector<bool>(M, false));
    for (int idx = 0; idx < M; idx++) {
        int d = ans[idx];
        if (d >= 0)
            Medge_nouse[d][idx] = true;
    }

    vector<int> ans_cnt(D);
    for (int idx = 0; idx < M; idx++) {
        int d = ans[idx];
        if (d >= 0)
            ans_cnt[d]++;
    }


    bool flag_break = false;
    int epoch = 0;
    while (true) {
        for (int _ = 0; _ < M; _++) {
            if (get_time_remaining() < 20) {
                flag_break = true;
                break;
            }

            Edge* e = EdgesSorted[_];

            int d_bef = ans[e->idx];
            int idx = e->idx;

            if (d_bef >= 0) {
                ans_cnt[d_bef]--;
                Medge_nouse[d_bef][idx] = false;
            }

            // int NN = min(epoch * 4, N);
            int NN = 30;
            int step = 1;
            int initial_idx = 0;
            if (NN != 0) {
                step = N / NN;
                // initial_idx = RandomRange(step);
                initial_idx = epoch % step;
            }

            vector<ll> diff_cost(D, 0ll);
            for (int d = 0; d < D; d++) {
                if (ans_cnt[d] >= K) {
                    diff_cost[d] = INF;
                    continue;
                }


                ll cost = 0;
                Medge_nouse[d][e->idx] = true;
                auto Vdist1 = DijkstraFastFast0128(e->u, Graph, Medge_nouse[d]);
                auto Vdist2 = DijkstraFastFast0128(e->v, Graph, Medge_nouse[d]);
                cost += accumulate(Vdist1.begin(), Vdist1.end(), 0ll);
                cost += accumulate(Vdist2.begin(), Vdist2.end(), 0ll);
                for (int i = 0; i < N; i++) {
                    ll dcost = min(Vdist1[i], Vdist2[i] + e->dist) + min(Vdist2[i], Vdist1[i] + e->dist);
                    cost -= dcost;
                }
                for (int i = initial_idx; i < NN * step + initial_idx; i += step) {
                    vector<ll> Vdist = DijkstraFastFast0128(i, Graph, Medge_nouse[d]);
                    ll dcost = 0;
                    dcost -= accumulate(Vdist.begin(), Vdist.end(), 0ll);
                    for (int idx2 = 0; idx2 < N; idx2++) {
                        dcost += min({Vdist[idx2], Vdist[e->u] + e->dist + Vdist2[idx2], Vdist[e->v] + e->dist + Vdist1[idx2]});
                    }
                    cost -= dcost;
                }
                Medge_nouse[d][e->idx] = false;

                diff_cost[d] = cost;


                // diff_cost[d] += cost_edge(e, Graph, Medge_nouse[d]);
                // for (int i = initial_idx; i < NN * step + initial_idx; i += step) {
                //     vector<ll> tmp = DijkstraFastFast0128(i, Graph, Medge_nouse[d]);
                //     diff_cost[d] -= accumulate(tmp.begin(), tmp.end(), 0ll) - MinDistMatrixSum[i];
                // }
                // Medge_nouse[d][idx] = true;
                // for (int i = initial_idx; i < NN * step + initial_idx; i += step) {
                //     vector<ll> tmp = DijkstraFastFast0128(i, Graph, Medge_nouse[d]);
                //     diff_cost[d] += accumulate(tmp.begin(), tmp.end(), 0ll) - MinDistMatrixSum[i];
                // }
                // Medge_nouse[d][idx] = false;
            }
            int d_new = 0;
            ll min_cost = diff_cost[0];
            for (int i = 1; i < D; i++) {
                if (chmin(min_cost, diff_cost[i]))
                    d_new = i;
                else if (min_cost == diff_cost[i] && ans_cnt[i] < ans_cnt[d_new])
                    d_new = i;
            }

            ans_cnt[d_new]++;
            ans[e->idx] = d_new;
            Medge_nouse[d_new][idx] = true;
            // cerr << diff_cost[d_new] << endl;
        }

        print_score(ans);

        if (flag_break)
            break;
        epoch++;
    }


    return ans;
}

vector<int> solve0130_1() {
    auto ans = solve0128_2();
    if (get_time_remaining() < 20) {
        return ans;
    }
    print_score(ans);
    ans.assign(M, -1);
    // vector<int> ans(M, -1);

    // naive local search
    // 1つずつ更新

    vector<vector<bool>> Medge_nouse(D, vector<bool>(M, false));
    for (int idx = 0; idx < M; idx++) {
        int d = ans[idx];
        if (d >= 0)
            Medge_nouse[d][idx] = true;
    }

    vector<int> ans_cnt(D);
    for (int idx = 0; idx < M; idx++) {
        int d = ans[idx];
        if (d >= 0)
            ans_cnt[d]++;
    }

    // int NN = N;
    // int NN = 100;
    // int NN = min(N, (int)(40000 / (pow((double)N / 500, 7) * D)));
    // int NN = min(N, (int)(200000 / (pow((double)N / 500, 7) * D)));
    // int NN = 200 / D;
    int NN = N;
    // int step = N / NN;
    vector<vector<vector<ll>>> Mdist(D, vector(NN, vector(N, 0ll)));

    bool flag_break = false;
    int epoch = 0;
    while (true) {
        if (epoch == 5)
            break;
        // int initial_idx = epoch % step;
        vector<int> Vs(NN);
        // vector<int> Vs_flag(N);
        int Vs_rev[N_MAX] = {};
        fill(Vs_rev, Vs_rev + N_MAX, INF);
        for (int i = 0; i < NN; i++) {
            // Vs[i] = initial_idx + step * i;
            Vs[i] = VertexSorted[N - 1 - i];
            Vs_rev[Vs[i]] = i;
            // Vs_flag[Vs[i]] = true;
            // cerr << Vs[i] << endl;
        }

        for (int d = 0; d < D; d++) {
            for (int i = 0; i < NN; i++) {
                if (get_time_remaining() < 20) {
                    flag_break = true;
                    break;
                }
                int s = Vs[i];
                Mdist[d][i] = DijkstraFastFast0128(s, Graph, Medge_nouse[d]);
            }
            if (flag_break)
                break;
        }

        int time_begin = get_time();
        int NN_cum = 0;
        for (int _ = 0; _ < M; _++) {
            if (_ >= 30) {
                int time_consumed = get_time() - time_begin;
                int time_left = TIME_LIMIT - get_time() - 200;
                int NN_cum_fixed = NN_cum + 100 * NN + _ * NN / 8;
                double time_per_NN = (double)time_consumed / NN_cum_fixed;
                chmin(NN, int((double)time_left / time_per_NN / (M - _)));
                if (NN <= 5)
                    NN = 5;
            }
            // if (_ % 20 == 0)
            //     cerr << _ << " " << NN << endl;
            NN_cum += NN;
            // if (get_time_remaining() <= 3000)
            //     chmin(NN, 30);
            // if (get_time_remaining() <= 1000)
            //     // chmin(NN, 10);
            //     chmin(NN, 0);

            Edge* e = EdgesSorted[_];
            int d_bef = ans[e->idx];
            int idx = e->idx;
            if (get_time_remaining() < 20) {
                vector<int> Vcnt(D);
                for (int d = 0; d < D; d++) {
                    // Vcnt[d] = (int)ans_rev[d].size();
                    Vcnt[d] = ans_cnt[d];

                    // 孤立点がないか確認
                    bool is_isolated = false;
                    for (auto& u : {e->u, e->v}) {
                        int dim = (int)Graph[u].size();
                        int dim_current = 0;
                        for (auto&& f : Graph[u]) {
                            if (ans[f->idx] == -1)
                                continue;
                            dim_current += ans[f->idx] == d;
                        }
                        if (dim - dim_current == 1)
                            is_isolated = true;
                    }
                    if (is_isolated)
                        Vcnt[d] = INF;
                }
                int d = int(min_element(Vcnt.begin(), Vcnt.end()) - Vcnt.begin());
                ans[idx] = d;
                ans_cnt[d]++;
                continue;
            }


            if (d_bef >= 0) {
                ans_cnt[d_bef]--;
                Medge_nouse[d_bef][idx] = false;
            }

            vector<ll> diff_cost(D, 0ll);
            for (int d = 0; d < D; d++) {
                if (ans_cnt[d] >= K) {
                    diff_cost[d] = INFLL;
                    continue;
                }

                ll& cost = diff_cost[d];

                Medge_nouse[d][e->idx] = true;

                int u = e->u, v = e->v;
                // if (Vs_rev[u] >= NN && Vs_rev[v] >= NN) {
                auto Vdist1 = DijkstraFastFast0128(e->u, Graph, Medge_nouse[d]);
                auto Vdist2 = DijkstraFastFast0128(e->v, Graph, Medge_nouse[d]);
                cost += accumulate(Vdist1.begin(), Vdist1.end(), 0ll);
                cost += accumulate(Vdist2.begin(), Vdist2.end(), 0ll);
                for (int i = 0; i < N; i++) {
                    ll dcost = min(Vdist1[i], Vdist2[i] + e->dist) + min(Vdist2[i], Vdist1[i] + e->dist);
                    cost -= dcost;
                }
                // }

                // OLD
                for (int i = 0; i < NN; i++) {
                    int s = Vs[i];
                    if (s == u || s == v)
                        continue;
                    vector<ll> Vdist = DijkstraFastFast0128(s, Graph, Medge_nouse[d]);
                    ll dcost = 0;
                    dcost -= accumulate(Vdist.begin(), Vdist.end(), 0ll);
                    for (int idx2 = 0; idx2 < N; idx2++) {
                        dcost += min({Vdist[idx2], Vdist[e->u] + e->dist + Vdist2[idx2], Vdist[e->v] + e->dist + Vdist1[idx2]});
                    }
                    cost -= dcost;
                }
                // END

                Medge_nouse[d][e->idx] = false;
            }
            // if (get_time_remaining() >= 200)
            if (false)
                for (int d = 0; d < D; d++) {
                    if (ans_cnt[d] >= K) {
                        diff_cost[d] = INFLL;
                        continue;
                    }
                    ll& cost = diff_cost[d];
                    for (int i = 0; i < NN; i++) {
                        int s = Vs[i];
                        cost += DijkstraCostNoUseFlagUpdate0130(s, Graph, Medge_nouse[d], e, Mdist[d][i], false);
                    }
                }
            int d_new = 0;
            ll min_cost = diff_cost[0];
            for (int i = 1; i < D; i++) {
                if (chmin(min_cost, diff_cost[i]))
                    d_new = i;
                else if (min_cost == diff_cost[i] && ans_cnt[i] < ans_cnt[d_new])
                    d_new = i;
            }

            // if (get_time_remaining() >= 200)
            if (false)
                for (int i = 0; i < NN; i++) {
                    int s = Vs[i];
                    DijkstraCostNoUseFlagUpdate0130(s, Graph, Medge_nouse[d_new], e, Mdist[d_new][i], true);
                }

            ans_cnt[d_new]++;
            ans[e->idx] = d_new;
            Medge_nouse[d_new][idx] = true;
            // cerr << diff_cost[d_new] << endl;

            // cerr << _ << " " << get_time() - time_begin << endl;
        }

        print_score(ans);

        if (flag_break)
            break;
        epoch++;
    }


    return ans;
}

void test_0130() {
    // test DijkstraFastFast0128

    vector<bool> edge_nouse(M, false);
    Edge* e = &Edges[100];

    for (int i = 1; i < N; i += 20) {
        edge_nouse[i] = true;
    }


    auto Vdist0 = DijkstraFastFast0128(0, Graph, edge_nouse);
    edge_nouse[e->idx] = true;
    auto Vdist1 = DijkstraFastFast0128(0, Graph, edge_nouse);
    ll dcost1 = 0;
    for (int i = 0; i < N; i++) {
        dcost1 += Vdist1[i] - Vdist0[i];
    }

    edge_nouse[e->idx] = false;
    auto Vdist0_backup = Vdist0;
    ll dcost0 = DijkstraCostNoUseFlagUpdate0130(0, Graph, edge_nouse, e, Vdist0, false);
    // assert(Vdist0 == Vdist1);
    assert(Vdist0 == Vdist0_backup);
    ll dcost00 = DijkstraCostNoUseFlagUpdate0130(0, Graph, edge_nouse, e, Vdist0, true);
    assert(Vdist0 == Vdist1);
    assert(dcost0 == dcost1);
    assert(dcost0 == dcost00);
    cerr << dcost0 << " " << dcost00 << endl;

    return;
}


vector<int> solve0131_1() {
    auto ans = solve0128_2();
    if (get_time_remaining() < 20) {
        return ans;
    }
    print_score(ans);
    ans.assign(M, -1);
    // vector<int> ans(M, -1);

    vector<vector<int>> Medge_cnt(M, vector<int>(M, 0));
    vector<int> Vedge_cnt(M, 0);
    // vector<int> Vflag(M, -1);
    for (int u = 0; u < N; u++) {
        vector<ll> Vdist = DijkstraFastFast0128(u, Graph);
        // int cnt = 0;
        vector<int> use_edges;
        use_edges.reserve(N - 1);
        for (int i = 0; i < M; i++) {
            Edge* e = &Edges[i];
            if (abs(Vdist[e->u] - Vdist[e->v]) == e->dist)
                use_edges.emplace_back(e->idx);
        }
        for (int i = 0; i < (int)use_edges.size(); i++) {
            for (int j = 0; j < i; j++) {
                Medge_cnt[use_edges[i]][use_edges[j]]++;
                Medge_cnt[use_edges[j]][use_edges[i]]++;
            }
        }
        for (int i = 0; i < (int)use_edges.size(); i++) {
            Vedge_cnt[use_edges[i]]++;
        }
    }

    vector<tuple<double, int, int>> Vsort;
    Vsort.reserve(M * (M - 1) / 2);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < i; j++) {
            Vsort.emplace_back((double)Medge_cnt[i][j] / (Vedge_cnt[i] + Vedge_cnt[j]) * 2, i, j);
        }
    }
    sort(Vsort.begin(), Vsort.end(), greater{});


    UnionFind uf(M);
    for (int i = 0;; i++) {
        auto [d, u, v] = Vsort[i];
        // if (d < 0.99)
        break;
        // cerr << d << " " << u << " " << v << endl;
        if (!uf.same(u, v) && uf.size(u) + uf.size(v) <= 4)
            uf.unite(u, v);
    }
    map<int, vector<int>> mp_cluster;
    for (int i = 0; i < M; i++) {
        mp_cluster[uf.find(i)].emplace_back(i);
    }
    int MM = (int)mp_cluster.size();
    vector<vector<int>> Vedge_cluster;
    Vedge_cluster.reserve(MM);
    for (auto [r, V] : mp_cluster) {
        // cerr << " " << V.size() << endl;
        Vedge_cluster.emplace_back(V);
    }
    cerr << "size = " << mp_cluster.size() << " / " << M << endl;

    vector<pair<double, int>> Vedge_cluster_pair(MM);
    for (int i = 0; i < MM; i++) {
        double x = 0, y = 0;
        int NN = (int)Vedge_cluster[i].size();
        for (auto&& idx : Vedge_cluster[i]) {
            x += X[Edges[idx].u];
            x += X[Edges[idx].v];
            y += Y[Edges[idx].u];
            y += Y[Edges[idx].v];
        }
        x /= NN;
        y /= NN;
        Vedge_cluster_pair[i] = {x * x + y * y, i};
    }
    sort(Vedge_cluster_pair.begin(), Vedge_cluster_pair.end());
    vector<int> Vedge_cluster_order(MM);
    int cnt = 0;
    for (int i = 0; i < MM; i++) {
        Vedge_cluster_order[i] = Vedge_cluster_pair[i].second;
        cnt += (int)Vedge_cluster[i].size();
    }
    cerr << cnt << endl;


    // naive local search
    // 1つずつ更新

    vector<vector<bool>> Medge_nouse(D, vector<bool>(M, false));
    for (int idx = 0; idx < M; idx++) {
        int d = ans[idx];
        if (d >= 0)
            Medge_nouse[d][idx] = true;
    }

    vector<int> ans_cnt(D);
    for (int idx = 0; idx < M; idx++) {
        int d = ans[idx];
        if (d >= 0)
            ans_cnt[d]++;
    }

    // return ans;

    // int NN = N;
    // int NN = 10;
    int NN = 100;
    // int NN = min(N, (int)(40000 / (pow((double)N / 500, 7) * D)));
    // int NN = min(N, (int)(200000 / (pow((double)N / 500, 7) * D)));
    // int NN = 200 / D;
    // int NN = N;
    // int step = N / NN;
    vector<vector<vector<ll>>> Mdist(D, vector(NN, vector(N, 0ll)));

    bool flag_break = false;
    int epoch = 0;
    while (true) {
        if (epoch == 1)
            break;
        // int initial_idx = epoch % step;
        vector<int> Vs(NN);
        // vector<int> Vs_flag(N);
        int Vs_rev[N_MAX] = {};
        fill(Vs_rev, Vs_rev + N_MAX, INF);
        for (int i = 0; i < NN; i++) {
            // Vs[i] = initial_idx + step * i;
            Vs[i] = VertexSorted[N - 1 - i];
            Vs_rev[Vs[i]] = i;
            // Vs_flag[Vs[i]] = true;
            // cerr << Vs[i] << endl;
        }

        for (int d = 0; d < D; d++) {
            for (int i = 0; i < NN; i++) {
                if (get_time_remaining() < 20) {
                    flag_break = true;
                    break;
                }
                int s = Vs[i];
                Mdist[d][i] = DijkstraFastFast0128(s, Graph, Medge_nouse[d]);
            }
            if (flag_break)
                break;
        }

        int time_begin = get_time();
        int NN_cum = 0;
        for (int _ = 0; _ < MM; _++) {
            if (_ % 10 == 0)
                cerr << _ << " " << NN << " " << get_time() << endl;
            if (_ >= 30) {
                int time_consumed = get_time() - time_begin;
                int time_left = TIME_LIMIT - get_time() - 200;
                int NN_cum_fixed = NN_cum + 100 * NN + _ * NN / 8;
                double time_per_NN = (double)time_consumed / NN_cum_fixed;
                chmin(NN, int((double)time_left / time_per_NN / (M - _)));
                if (NN <= 5)
                    NN = 5;
            }
            // if (_ % 20 == 0)
            //     cerr << _ << " " << NN << endl;
            NN_cum += NN;
            // if (get_time_remaining() <= 3000)
            //     chmin(NN, 30);
            // if (get_time_remaining() <= 1000)
            //     // chmin(NN, 10);
            //     chmin(NN, 0);

            // Edge* e = EdgesSorted[_];
            // int d_bef = ans[e->idx];
            // int idx = e->idx;
            // if (get_time_remaining() < 20) {
            // NEW
            // if (false) {
            //     vector<int> Vcnt(D);
            //     for (int d = 0; d < D; d++) {
            //         // Vcnt[d] = (int)ans_rev[d].size();
            //         Vcnt[d] = ans_cnt[d];

            //         // 孤立点がないか確認
            //         bool is_isolated = false;
            //         for (auto& u : {e->u, e->v}) {
            //             int dim = (int)Graph[u].size();
            //             int dim_current = 0;
            //             for (auto&& f : Graph[u]) {
            //                 if (ans[f->idx] == -1)
            //                     continue;
            //                 dim_current += ans[f->idx] == d;
            //             }
            //             if (dim - dim_current == 1)
            //                 is_isolated = true;
            //         }
            //         if (is_isolated)
            //             Vcnt[d] = INF;
            //     }
            //     int d = int(min_element(Vcnt.begin(), Vcnt.end()) - Vcnt.begin());
            //     ans[idx] = d;
            //     ans_cnt[d]++;
            //     continue;
            // }

            vector<int>& Vedge = Vedge_cluster[Vedge_cluster_order[_]];
            // cerr << X[Edges[Vedge[0]].u] << " " << Y[Edges[Vedge[0]].v] << endl;

            // if (d_bef >= 0) {
            //     ans_cnt[d_bef]--;
            //     Medge_nouse[d_bef][idx] = false;
            // }

            vector<ll> diff_cost(D, 0ll);
            for (int d = 0; d < D; d++) {
                if (ans_cnt[d] + (int)Vedge.size() >= K) {
                    diff_cost[d] = INFLL;
                    continue;
                }

                ll& cost = diff_cost[d];

                for (auto&& idx : Vedge) {
                    Medge_nouse[d][idx] = true;
                }


                // Medge_nouse[d][e->idx] = true;

                // int u = e->u, v = e->v;
                // // if (Vs_rev[u] >= NN && Vs_rev[v] >= NN) {
                // auto Vdist1 = DijkstraFastFast0128(e->u, Graph, Medge_nouse[d]);
                // auto Vdist2 = DijkstraFastFast0128(e->v, Graph, Medge_nouse[d]);
                // cost += accumulate(Vdist1.begin(), Vdist1.end(), 0ll);
                // cost += accumulate(Vdist2.begin(), Vdist2.end(), 0ll);
                // for (int i = 0; i < N; i++) {
                //     ll dcost = min(Vdist1[i], Vdist2[i] + e->dist) + min(Vdist2[i], Vdist1[i] + e->dist);
                //     cost -= dcost;
                // }
                // // }

                for (int i = 0; i < NN; i++) {
                    int s = Vs[i];
                    vector<ll> Vdist = DijkstraFastFast0128(s, Graph, Medge_nouse[d]);
                    ll dcost = 0;
                    dcost -= accumulate(Vdist.begin(), Vdist.end(), 0ll);
                    cost -= dcost;
                }
                for (auto&& idx : Vedge) {
                    Medge_nouse[d][idx] = false;
                }
                for (int i = 0; i < NN; i++) {
                    int s = Vs[i];
                    vector<ll> Vdist = DijkstraFastFast0128(s, Graph, Medge_nouse[d]);
                    ll dcost = 0;
                    dcost += accumulate(Vdist.begin(), Vdist.end(), 0ll);
                    cost -= dcost;
                }

                // Medge_nouse[d][e->idx] = false;
            }
            // if (get_time_remaining() >= 200)
            // if (false)
            //     for (int d = 0; d < D; d++) {
            //         if (ans_cnt[d] >= K) {
            //             diff_cost[d] = INFLL;
            //             continue;
            //         }
            //         ll& cost = diff_cost[d];
            //         for (int i = 0; i < NN; i++) {
            //             int s = Vs[i];
            //             cost += DijkstraCostNoUseFlagUpdate0130(s, Graph, Medge_nouse[d], e, Mdist[d][i], false);
            //         }
            //     }
            int d_new = 0;
            ll min_cost = diff_cost[0];
            for (int i = 1; i < D; i++) {
                if (chmin(min_cost, diff_cost[i]))
                    d_new = i;
                else if (min_cost == diff_cost[i] && ans_cnt[i] < ans_cnt[d_new])
                    d_new = i;
            }

            // if (get_time_remaining() >= 200)
            // if (false)
            //     for (int i = 0; i < NN; i++) {
            //         int s = Vs[i];
            //         DijkstraCostNoUseFlagUpdate0130(s, Graph, Medge_nouse[d_new], e, Mdist[d_new][i], true);
            //     }


            ans_cnt[d_new] += (int)Vedge.size();
            for (auto&& idx : Vedge) {
                ans[idx] = d_new;
                Medge_nouse[d_new][idx] = true;
            }
            // ans_cnt[d_new]++;
            // ans[e->idx] = d_new;
            // Medge_nouse[d_new][idx] = true;
            // cerr << diff_cost[d_new] << endl;

            // cerr << _ << " " << get_time() - time_begin << endl;
        }

        print_score(ans);

        if (flag_break)
            break;
        epoch++;
    }


    return ans;
}

vector<int> solve0204_1() {
    auto ans = solve0128_2();
    if (get_time_remaining() < 20) {
        return ans;
    }
    print_score(ans);
    ans.assign(M, -1);
    // vector<int> ans(M, -1);

    // naive local search
    // 1つずつ更新

    vector<vector<bool>> Medge_nouse(D, vector<bool>(M, false));
    for (int idx = 0; idx < M; idx++) {
        int d = ans[idx];
        if (d >= 0)
            Medge_nouse[d][idx] = true;
    }

    vector<int> ans_cnt(D);
    for (int idx = 0; idx < M; idx++) {
        int d = ans[idx];
        if (d >= 0)
            ans_cnt[d]++;
    }

    // int NN = N;
    // int NN = 100;
    int NN = min(N, (int)(40000 / (pow((double)N / 500, 7) * D)));
    // int NN = min(N, (int)(200000 / (pow((double)N / 500, 7) * D)));
    // int NN = 200 / D;
    // int NN = N;
    // int step = N / NN;
    vector<vector<vector<ll>>> Mdist(D, vector(NN, vector(N, 0ll)));

    bool flag_break = false;
    int epoch = 0;
    while (true) {
        if (epoch == 5)
            break;
        // int initial_idx = epoch % step;
        vector<int> Vs(NN);
        // vector<int> Vs_flag(N);
        int Vs_rev[N_MAX] = {};
        fill(Vs_rev, Vs_rev + N_MAX, INF);
        for (int i = 0; i < NN; i++) {
            // Vs[i] = initial_idx + step * i;
            Vs[i] = VertexSorted[N - 1 - i];
            Vs_rev[Vs[i]] = i;
            // Vs_flag[Vs[i]] = true;
            // cerr << Vs[i] << endl;
        }

        for (int d = 0; d < D; d++) {
            for (int i = 0; i < NN; i++) {
                if (get_time_remaining() < 20) {
                    flag_break = true;
                    break;
                }
                int s = Vs[i];
                Mdist[d][i] = DijkstraFastFast0128(s, Graph, Medge_nouse[d]);
            }
            if (flag_break)
                break;
        }

        int time_begin = get_time();
        int NN_cum = 0;
        for (int _ = 0; _ < M; _++) {
            if (_ >= 30) {
                int time_consumed = get_time() - time_begin;
                int time_left = TIME_LIMIT - get_time() - 200;
                int NN_cum_fixed = NN_cum + 100 * NN + _ * NN / 8;
                double time_per_NN = (double)time_consumed / NN_cum_fixed;
                chmin(NN, int((double)time_left / time_per_NN / (M - _)));
                if (NN <= 5)
                    NN = 5;
            }
            // if (_ % 20 == 0)
            //     cerr << _ << " " << NN << endl;
            NN_cum += NN;
            // if (get_time_remaining() <= 3000)
            //     chmin(NN, 30);
            // if (get_time_remaining() <= 1000)
            //     // chmin(NN, 10);
            //     chmin(NN, 0);

            Edge* e = EdgesSorted[_];
            int d_bef = ans[e->idx];
            int idx = e->idx;
            if (get_time_remaining() < 20) {
                vector<int> Vcnt(D);
                for (int d = 0; d < D; d++) {
                    // Vcnt[d] = (int)ans_rev[d].size();
                    Vcnt[d] = ans_cnt[d];

                    // 孤立点がないか確認
                    bool is_isolated = false;
                    for (auto& u : {e->u, e->v}) {
                        int dim = (int)Graph[u].size();
                        int dim_current = 0;
                        for (auto&& f : Graph[u]) {
                            if (ans[f->idx] == -1)
                                continue;
                            dim_current += ans[f->idx] == d;
                        }
                        if (dim - dim_current == 1)
                            is_isolated = true;
                    }
                    if (is_isolated)
                        Vcnt[d] = INF;
                }
                int d = int(min_element(Vcnt.begin(), Vcnt.end()) - Vcnt.begin());
                ans[idx] = d;
                ans_cnt[d]++;
                continue;
            }


            if (d_bef >= 0) {
                ans_cnt[d_bef]--;
                Medge_nouse[d_bef][idx] = false;
            }

            vector<ll> diff_cost(D, 0ll);
            for (int d = 0; d < D; d++) {
                if (ans_cnt[d] >= K) {
                    diff_cost[d] = INFLL;
                    continue;
                }

                ll& cost = diff_cost[d];

                Medge_nouse[d][e->idx] = true;

                int u = e->u, v = e->v;
                // if (Vs_rev[u] >= NN && Vs_rev[v] >= NN) {
                auto Vdist1 = DijkstraFastFast0128(e->u, Graph, Medge_nouse[d]);
                auto Vdist2 = DijkstraFastFast0128(e->v, Graph, Medge_nouse[d]);
                cost += accumulate(Vdist1.begin(), Vdist1.end(), 0ll);
                cost += accumulate(Vdist2.begin(), Vdist2.end(), 0ll);
                for (int i = 0; i < N; i++) {
                    ll dcost = min(Vdist1[i], Vdist2[i] + e->dist) + min(Vdist2[i], Vdist1[i] + e->dist);
                    cost -= dcost;
                }
                // }

                // OLD
                for (int i = 0; i < NN; i++) {
                    int s = Vs[i];
                    if (s == u || s == v)
                        continue;
                    vector<ll> Vdist = DijkstraFastFast0128(s, Graph, Medge_nouse[d]);
                    ll dcost = 0;
                    dcost -= accumulate(Vdist.begin(), Vdist.end(), 0ll);
                    for (int idx2 = 0; idx2 < N; idx2++) {
                        dcost += min({Vdist[idx2], Vdist[e->u] + e->dist + Vdist2[idx2], Vdist[e->v] + e->dist + Vdist1[idx2]});
                    }
                    cost -= dcost;
                }
                // END

                Medge_nouse[d][e->idx] = false;
            }
            // if (get_time_remaining() >= 200)
            if (false)
                for (int d = 0; d < D; d++) {
                    if (ans_cnt[d] >= K) {
                        diff_cost[d] = INFLL;
                        continue;
                    }
                    ll& cost = diff_cost[d];
                    for (int i = 0; i < NN; i++) {
                        int s = Vs[i];
                        cost += DijkstraCostNoUseFlagUpdate0130(s, Graph, Medge_nouse[d], e, Mdist[d][i], false);
                    }
                }
            int d_new = 0;
            ll min_cost = diff_cost[0];
            for (int i = 1; i < D; i++) {
                if (chmin(min_cost, diff_cost[i]))
                    d_new = i;
                else if (min_cost == diff_cost[i] && ans_cnt[i] < ans_cnt[d_new])
                    d_new = i;
            }

            // if (min_cost == 0) {
            //     cerr << epoch << " " << _ << endl;
            // }

            // if (get_time_remaining() >= 200)
            if (false)
                for (int i = 0; i < NN; i++) {
                    int s = Vs[i];
                    DijkstraCostNoUseFlagUpdate0130(s, Graph, Medge_nouse[d_new], e, Mdist[d_new][i], true);
                }

            ans_cnt[d_new]++;
            ans[e->idx] = d_new;
            Medge_nouse[d_new][idx] = true;
            // cerr << diff_cost[d_new] << endl;

            // cerr << _ << " " << get_time() - time_begin << endl;
        }

        print_score(ans);

        if (flag_break)
            break;
        epoch++;
    }


    return ans;
}


vector<int> solve0205_1() {
    // auto ans = solve0128_2();
    // if (get_time_remaining() < 20) {
    //     return ans;
    // }
    // print_score(ans);
    // ans.assign(M, -1);
    vector<int> ans(M, -1);

    // naive local search
    // 1つずつ更新

    vector<vector<bool>> Medge_nouse(D, vector<bool>(M, false));
    for (int idx = 0; idx < M; idx++) {
        int d = ans[idx];
        if (d >= 0)
            Medge_nouse[d][idx] = true;
    }

    vector<int> ans_cnt(D);
    for (int idx = 0; idx < M; idx++) {
        int d = ans[idx];
        if (d >= 0)
            ans_cnt[d]++;
    }

    // int NN_initial = N;
    // int NN_initial = min(N, 200);
    // int NN_initial = 100;
    // int NN_initial = 30;
    // int NN_initial = 120 - int((N - 500) / 10);
    // int NN_initial = 200 - int((N - 500) / 10);
    int NN_initial = 0;


    int NN = NN_initial;
    // int NN = N;
    // int NN = min(N, 500);
    // int NN = 300;
    // int NN = N / 3;
    // int NN = 100;
    // int NN = min(N, (int)(40000 / (pow((double)N / 500, 7) * D)));
    // int NN = min(N, (int)(200000 / (pow((double)N / 500, 7) * D)));
    // int NN = 200 / D;
    // int NN = N;
    // int step = N / NN;
    vector<vector<vector<ll>>> Mdist(D, vector(N, vector(N, 0ll)));

    bool flag_break = false;
    int epoch = 0;
    while (true) {
        // if (epoch == 1)
        //     break;
        // int initial_idx = epoch % step;
        vector<int> Vs(N);
        // vector<int> Vs_flag(N);
        int Vs_rev[N_MAX] = {};
        fill(Vs_rev, Vs_rev + N_MAX, INF);
        for (int i = 0; i < N; i++) {
            // Vs[i] = initial_idx + step * i;
            Vs[i] = VertexSorted[N - 1 - i];
            Vs_rev[Vs[i]] = i;
            // Vs_flag[Vs[i]] = true;
            // cerr << Vs[i] << endl;
        }

        if (epoch == 0) {
            NN_initial = 40;
            // NN_initial = (epoch + 2) * 30;
            NN = NN_initial;
            for (int d = 0; d < D; d++) {
                for (int i = 0; i < NN_initial; i++) {
                    int s = Vs[i];
                    Mdist[d][i] = MDistInitial[s];
                }
                // for (int i = 0; i < NN_initial; i++) {
                //     if (get_time_remaining() < 20) {
                //         flag_break = true;
                //         break;
                //     }
                //     int s = Vs[i];
                //     Mdist[d][i] = DijkstraFastFast0128(s, Graph, Medge_nouse[d]);
                //     // assert(Mdist[d][i] == MDistInitial[s]);
                // }
                // if (flag_break)
                //     break;
            }
        } else {
            // update NN_initial
            int NN_initial_bef = NN_initial;
            NN_initial = min((epoch + 1) * 30, N);
            // NN_initial = min((epoch + 1) * 50, N);
            for (int d = 0; d < D; d++) {
                for (int i = NN_initial_bef; i < NN_initial; i++) {
                    if (get_time_remaining() < 20) {
                        flag_break = true;
                        break;
                        }
                        int s = Vs[i];
                        Mdist[d][i] = DijkstraFastFast0128(s, Graph, Medge_nouse[d]);
                    }
                    if (flag_break)
                        break;
                }
            }

            int time_begin = get_time();
            int NN_cum = 0;
            for (int _ = 0; _ < M; _++) {
                if (epoch == 0 && _ >= 50) {
                    int time_consumed = get_time() - time_begin;
                    ll time_left = TIME_LIMIT / 2 - get_time();
                    ll NN_cum_fixed = NN_cum + 100 * NN + _ * NN;
                    // NN_cum_fixed *= 2;
                    double time_per_NN = (double)time_consumed / (double)NN_cum_fixed;
                    chmin(NN, int(min(ll((double)time_left / time_per_NN / (M - _)), ll(N))));
                    if (NN <= 10)
                        NN = 10;
                }
                if (epoch >= 1)
                    NN = NN_initial;
                    // NN = 100;
                    // NN = N;

                    // NN = min(NN_initial, (epoch + 1) * 40);


                    // NN = NN_initial;
                    // NN = (epoch + 1) * 30;
                    // chmin(NN, N);

#ifdef FLAG_PRINT_NN
                if (_ % 100 == 0)
                    cerr
                        << _ << " " << NN << " " << get_time() << endl;

#endif  // FLAG_PRINT_NN

                NN_cum += NN;
                // if (get_time_remaining() <= 3000)
                //     chmin(NN, 30);
                // if (get_time_remaining() <= 1000)
                //     // chmin(NN, 10);
                //     chmin(NN, 0);

                Edge* e = EdgesSorted[_];
                int d_bef = ans[e->idx];
                int idx = e->idx;
                if (get_time_remaining() < 20) {
                    if (epoch >= 1) {
                        flag_break = true;
                        break;
                    }
                    vector<int> Vcnt(D);
                    for (int d = 0; d < D; d++) {
                        // Vcnt[d] = (int)ans_rev[d].size();
                        Vcnt[d] = ans_cnt[d];

                        // 孤立点がないか確認
                        bool is_isolated = false;
                        for (auto& u : {e->u, e->v}) {
                            int dim = (int)Graph[u].size();
                            int dim_current = 0;
                            for (auto&& f : Graph[u]) {
                                if (ans[f->idx] == -1)
                                    continue;
                                dim_current += ans[f->idx] == d;
                            }
                            if (dim - dim_current == 1)
                                is_isolated = true;
                        }
                        if (is_isolated)
                            Vcnt[d] = INF;
                    }
                    int d = int(min_element(Vcnt.begin(), Vcnt.end()) - Vcnt.begin());
                    ans[idx] = d;
                    ans_cnt[d]++;
                    continue;
                }


                if (d_bef >= 0) {
                    ans_cnt[d_bef]--;
                    // for (int i = 0; i < NN; i++) {
                    //     int s = Vs[i];
                    //     DijkstraCostUseAlwaysUpdate0205(s, Graph, Medge_nouse[d_bef], e, Mdist[d_bef][i]);
                    // }
                    Medge_nouse[d_bef][idx] = false;
                }

                vector<ll> diff_cost(D, 0ll);
                // if (false)
                for (int d = 0; d < D; d++) {
                    if (ans_cnt[d] >= K) {
                        diff_cost[d] = INFLL;
                        continue;
                    }

                    continue;

                    ll& cost = diff_cost[d];

                    // int u = e->u, v = e->v;
                    // for (int i = 0; i < NN; i++) {
                    //     int s = Vs[i];
                    //     cost += abs(Mdist[d][s][u] - Mdist[d][s][v]) == e->dist;
                    // }


                    Medge_nouse[d][e->idx] = true;

                    // if (Vs_rev[u] >= NN && Vs_rev[v] >= NN) {
                    auto Vdist1 = DijkstraFastFast0128(e->u, Graph, Medge_nouse[d]);
                    auto Vdist2 = DijkstraFastFast0128(e->v, Graph, Medge_nouse[d]);
                    cost += accumulate(Vdist1.begin(), Vdist1.end(), 0ll);
                    cost += accumulate(Vdist2.begin(), Vdist2.end(), 0ll);
                    for (int i = 0; i < N; i++) {
                        ll dcost = min(Vdist1[i], Vdist2[i] + e->dist) + min(Vdist2[i], Vdist1[i] + e->dist);
                        cost -= dcost;
                    }
                    // }

                    // OLD
                    // for (int i = 0; i < NN; i++) {
                    //     int s = Vs[i];
                    //     if (s == u || s == v)
                    //         continue;
                    //     vector<ll> Vdist = DijkstraFastFast0128(s, Graph, Medge_nouse[d]);
                    //     ll dcost = 0;
                    //     dcost -= accumulate(Vdist.begin(), Vdist.end(), 0ll);
                    //     for (int idx2 = 0; idx2 < N; idx2++) {
                    //         dcost += min({Vdist[idx2], Vdist[e->u] + e->dist + Vdist2[idx2], Vdist[e->v] + e->dist + Vdist1[idx2]});
                    //     }
                    //     cost -= dcost;
                    // }
                    // END

                    Medge_nouse[d][e->idx] = false;
                }
                // if (get_time_remaining() >= 20)
                // if (false)

                ll min_cost = INFLL;
                if (d_bef >= 0) {
                    int d = d_bef;
                    ll& cost = diff_cost[d];
                    for (int i = 0; i < NN_initial; i++) {
                        int s = Vs[i];
                        ll tmp = DijkstraCostUseAlwaysUpdate0205(s, Graph, Medge_nouse[d], e, Mdist[d][i]);
                        if (i < NN)
                            cost -= tmp;
                    }
                    min_cost = cost;
                }
                for (int d = 0; d < D; d++) {
                    if (ans_cnt[d] >= K) {
                        diff_cost[d] = INFLL;
                        continue;
                    }
                    ll& cost = diff_cost[d];
                    if (d == d_bef) {
                        continue;
                        // for (int i = 0; i < NN_initial; i++) {
                        //     int s = Vs[i];
                        //     ll tmp = DijkstraCostUseAlwaysUpdate0205(s, Graph, Medge_nouse[d], e, Mdist[d][i]);
                        //     if (i < NN)
                        //         cost -= tmp;
                        // }
                    } else {
                        for (int i = 0; i < NN; i++) {
                            int s = Vs[i];
                            cost += DijkstraCostNoUseFlagUpdate0130(s, Graph, Medge_nouse[d], e, Mdist[d][i], false);
                            if (cost > min_cost)
                                break;
                        }
                        chmin(min_cost, cost);
                    }
                }


                int d_new = 0;
                // ll min_cost = diff_cost[0];
                min_cost = diff_cost[0];
                for (int i = 1; i < D; i++) {
                    if (chmin(min_cost, diff_cost[i]))
                        d_new = i;
                    else if (min_cost == diff_cost[i] && ans_cnt[i] < ans_cnt[d_new])
                        d_new = i;
                }

             // if (get_time_remaining() >= 20)
                    // if (false)
                    for (int i = 0; i < NN_initial; i++) {
                        // for (int i = 0; i < NN_initial; i++) {
                        int s = Vs[i];
                        DijkstraCostNoUseFlagUpdate0130(s, Graph, Medge_nouse[d_new], e, Mdist[d_new][i], true);
                    }

                ans_cnt[d_new]++;
                ans[e->idx] = d_new;
                Medge_nouse[d_new][idx] = true;
                // cerr << diff_cost[d_new] << endl;

                // if (d_bef == d_new)
                //     cerr << "# " << endl;
                // else
                //     cerr << " #" << endl;

                // cerr << _ << " " << get_time() - time_begin << endl;
            }

            cerr << "epoch : " << epoch << endl;
            print_score(ans);

            if (flag_break)
                break;
            epoch++;
        }


        return ans;
}
// ---------------------------------------- main ----------------------------------------

int main() {
    start = chrono::system_clock::now();
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    read_input();
    initialize();

    // test_0130();
    // return 0;

    auto ans = solve0205_1();

    print_ans(ans);

#ifdef FLAG_PRINT_SCORE_END
    cerr << "time : " << get_time() << "  score : " << calc_score(ans, N) << endl;
    print_time();
#endif  // FLAG_PRINT_SCORE_END
}
