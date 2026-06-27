#include <bits/stdc++.h>
using namespace std;

// ===================== Fast RNG =====================
struct RNG {
    uint64_t x;
    explicit RNG(uint64_t seed = 88172645463325252ULL) : x(seed ? seed : 88172645463325252ULL) {}
    inline uint64_t next_u64() {
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        return x * 2685821657736338717ULL;
    }
    inline int next_int(int mod) {
        return (int)(next_u64() % (uint64_t)mod);
    }
    inline double next_double() {
        return (next_u64() >> 11) * (1.0 / 9007199254740992.0);
    }
};

static inline uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

// ===================== Timer =====================
struct Timer {
    using clock = chrono::steady_clock;
    clock::time_point st;
    Timer() : st(clock::now()) {}
    inline double elapsed() const {
        return chrono::duration<double>(clock::now() - st).count();
    }
};

// ===================== FastSet (open addressing with lazy clear) =====================
struct FastSet {
    vector<uint64_t> key;
    vector<uint32_t> stamp;
    uint32_t cur = 1;
    int mask = 0;
    int sz = 0;

    void init(int expected_size) {
        int cap = 1;
        while (cap < expected_size * 2)
            cap <<= 1;  // load <= 0.5
        key.assign(cap, 0);
        stamp.assign(cap, 0);
        mask = cap - 1;
        cur = 1;
        sz = 0;
    }

    inline void clear() {
        sz = 0;
        cur++;
        if (cur == 0) {
            fill(stamp.begin(), stamp.end(), 0);
            cur = 1;
        }
    }

    inline int size() const { return sz; }

    inline bool contains(uint64_t x) const {
        uint64_t h = splitmix64(x);
        int i = (int)(h & (uint64_t)mask);
        while (stamp[i] == cur) {
            if (key[i] == x)
                return true;
            i = (i + 1) & mask;
        }
        return false;
    }

    inline bool insert(uint64_t x) {
        uint64_t h = splitmix64(x);
        int i = (int)(h & (uint64_t)mask);
        while (stamp[i] == cur) {
            if (key[i] == x)
                return false;
            i = (i + 1) & mask;
        }
        stamp[i] = cur;
        key[i] = x;
        sz++;
        return true;
    }
};

// ===================== Problem-derived helpers =====================
static inline uint64_t pack_sig(int len, uint64_t bits) {
    return (bits << 6) | (uint64_t)len;  // len <= 63
}

// ========================= Tunables =========================
static constexpr int MAXD = 28;
static constexpr int D_BUILD = 22;
static constexpr int BEAM_WIDTH = 18000;
static constexpr int MAX_SEGS_PER_SHOP = 12000;

static constexpr int FLIP_MAX = 55;
static constexpr int TARGET_FLIPS_INIT = 12;
static constexpr int PAINT_BUDGET_INIT = 260;
static constexpr int INIT_FLIPS_QUOTA = 8;
static constexpr int FLIP_INTERVAL = 170;

static constexpr int FLIP_CAND = 80;
static constexpr double SA_ORDER_T0 = 5.0;
static constexpr double SA_ORDER_T1 = 0.01;

static constexpr int SA_PATHS_PER_SHOP = 320;
static constexpr int SA_MAXLEN = 14;
static constexpr int CAND_POOL = 80;
static constexpr int SA_FLIP_ITERS = 6500;
static constexpr double SA_FLIP_T0 = 5.0;
static constexpr double SA_FLIP_T1 = 0.25;

// ============================================================

struct Segment {
    uint8_t dest = 0;
    uint8_t cost = 0;
    uint8_t first = 0;
    uint8_t treeLen = 0;
    array<uint8_t, MAXD> moves{};
    array<uint8_t, MAXD> trees{};
};

struct Node {
    int prev = -1;
    int cur = -1;
    int parent = -1;
    uint8_t depth = 0;
    uint64_t h = 0;
};

struct ShopPool {
    int depth = 0;
    vector<Segment> segs;
    vector<vector<int>> group;
};

struct Params {
    int finalR = 45;
    double gamma = 1.6;
    int rankLimit = 70;
    int costW = 250;
    int balanceW = 2;
    int noise = 8;
};

struct Solver {
    int N = 0, M = 0, K = 0, T = 0;
    vector<vector<int>> g;
    vector<pair<int, int>> edges;
    vector<pair<int, int>> coords;

    vector<ShopPool> pools;
    vector<int> rankTree;

    uint64_t polyP = 1000003ULL;
    RNG rng;

    inline int encode_state(int prev, int cur) const { return (prev + 1) * N + cur; }

    uint64_t compute_seed() const {
        uint64_t h = 1469598103934665603ULL;
        auto mix = [&](uint64_t v) {
            h ^= v;
            h *= 1099511628211ULL;
        };
        mix((uint64_t)N);
        mix((uint64_t)M);
        mix((uint64_t)K);
        mix((uint64_t)T);
        for (auto [a, b] : edges) {
            mix((uint64_t)a);
            mix((uint64_t)b);
        }
        for (auto [x, y] : coords) {
            mix((uint64_t)x);
            mix((uint64_t)y);
        }
        return h;
    }

    // ===== random neighbor excluding prev (uniform via rejection) =====
    inline int rand_next_nb(int prev, int cur) {
        const auto& adj = g[cur];
        int deg = (int)adj.size();
        if (deg == 0)
            return -1;
        if (deg == 1) {
            int v = adj[0];
            return (v == prev ? -1 : v);
        }
        int nxt;
        do {
            nxt = adj[rng.next_int(deg)];
        } while (nxt == prev);
        return nxt;
    }

    // ===================== Pool builder (beam sampling) =====================
    ShopPool build_pool(int s, int depthMax) {
        ShopPool pool;
        pool.depth = depthMax;
        pool.segs.clear();
        pool.segs.reserve(MAX_SEGS_PER_SHOP);
        pool.group.assign(N, {});

        // ★ unordered_set を全廃：seen を FastSet 1個に集約
        static FastSet seen;
        static bool seen_init = false;
        if (!seen_init) {
            seen.init(MAX_SEGS_PER_SHOP + 2048);
            seen_init = true;
        } else {
            seen.clear();
        }

        vector<Node> nodes;
        nodes.reserve((size_t)BEAM_WIDTH * (depthMax + 2) + 1024);
        nodes.push_back(Node{-1, s, -1, 0, 1469598103934665603ULL});

        vector<int> frontier;
        frontier.reserve(BEAM_WIDTH);
        frontier.push_back(0);

        vector<int> next;
        next.reserve(BEAM_WIDTH);

        auto add_segment_from_node = [&](int nid) {
            uint8_t tmp[MAXD];
            int len = 0;
            int curId = nid;
            while (curId != 0 && curId >= 0 && len < MAXD) {
                tmp[len++] = (uint8_t)nodes[curId].cur;  // dest -> ... -> first
                curId = nodes[curId].parent;
            }
            if (len <= 0)
                return;

            Segment seg;
            seg.dest = tmp[0];
            seg.cost = (uint8_t)len;
            seg.first = tmp[len - 1];

            // reverse without std::reverse
            for (int i = 0; i < len; i++)
                seg.moves[i] = tmp[len - 1 - i];

            uint8_t tlen = 0;
            for (int i = 0; i < len; i++) {
                int v = (int)seg.moves[i];
                if (v < K)
                    break;
                seg.trees[tlen++] = (uint8_t)v;
            }
            seg.treeLen = tlen;
            pool.segs.push_back(seg);
        };

        for (int depth = 1; depth <= depthMax; depth++) {
            next.clear();
            int candidates = 0;

            for (int idx : frontier) {
                const Node& nd = nodes[idx];
                const auto& adj = g[nd.cur];
                for (int v : adj) {
                    if (v == nd.prev)
                        continue;
                    uint64_t nh = nd.h * polyP + (uint64_t)(v + 1);

                    if (v < K) {
                        if ((int)pool.segs.size() >= MAX_SEGS_PER_SHOP)
                            continue;

                        // dest を混ぜる（destごとのユニーク性）
                        uint64_t key = nh ^ (0x9e3779b97f4a7c15ULL * (uint64_t)(v + 1));
                        if (!seen.insert(key))
                            continue;

                        nodes.push_back(Node{nd.cur, v, idx, (uint8_t)depth, nh});
                        add_segment_from_node((int)nodes.size() - 1);
                        continue;
                    }

                    candidates++;
                    if ((int)next.size() < BEAM_WIDTH) {
                        nodes.push_back(Node{nd.cur, v, idx, (uint8_t)depth, nh});
                        next.push_back((int)nodes.size() - 1);
                    } else {
                        int j = rng.next_int(candidates);
                        if (j < BEAM_WIDTH) {
                            nodes.push_back(Node{nd.cur, v, idx, (uint8_t)depth, nh});
                            next[j] = (int)nodes.size() - 1;
                        }
                    }
                }
            }

            frontier.swap(next);
            if (frontier.empty())
                break;
            if ((int)pool.segs.size() >= MAX_SEGS_PER_SHOP && depth >= 7)
                break;
        }

        for (int i = 0; i < (int)pool.segs.size(); i++) {
            pool.group[pool.segs[i].first].push_back(i);
        }
        for (int v = 0; v < N; v++) {
            auto& vec = pool.group[v];
            if (vec.empty())
                continue;
            sort(vec.begin(), vec.end(), [&](int a, int b) {
                const auto& A = pool.segs[a];
                const auto& B = pool.segs[b];
                if (A.cost != B.cost)
                    return A.cost < B.cost;
                if (A.treeLen != B.treeLen)
                    return A.treeLen < B.treeLen;
                if (A.dest != B.dest)
                    return A.dest < B.dest;
                return a < b;
            });
        }
        return pool;
    }

    // ===================== Random-walk frequency estimate (tree importance) =====================
    vector<int> estimate_tree_freq_rw() {
        vector<int> freq(N, 0);
        int samplePerShop = 1200;
        int L = 14;
        for (int s = 0; s < K; s++) {
            for (int it = 0; it < samplePerShop; it++) {
                int prev = -1, cur = s;
                for (int step = 0; step < L; step++) {
                    int nxt = rand_next_nb(prev, cur);
                    if (nxt < 0)
                        break;
                    prev = cur;
                    cur = nxt;
                    if (cur >= K)
                        freq[cur]++;
                    else
                        break;
                }
            }
        }
        return freq;
    }

    // ===================== SA: choose initial flip set (proxy objective) =====================
    struct SAPath {
        uint8_t dest = 0, len = 0;
        uint64_t bits = 0;
    };

    vector<int> choose_flips_SA(const vector<int>& order) {
        int pool = min(CAND_POOL, (int)order.size());
        vector<int> cand(order.begin(), order.begin() + pool);
        if (cand.empty())
            return {};

        vector<char> inPool(N, 0);
        for (int v : cand)
            inPool[v] = 1;

        vector<SAPath> paths;
        paths.reserve((size_t)K * SA_PATHS_PER_SHOP);

        vector<vector<pair<int, uint64_t>>> treeAffects(N);

        // pathOccs をフラット化（vector確保を激減）
        vector<uint32_t> occStart;
        vector<pair<int, uint64_t>> occFlat;
        occStart.reserve((size_t)K * SA_PATHS_PER_SHOP + 1);
        occFlat.reserve((size_t)K * SA_PATHS_PER_SHOP * 3);
        occStart.push_back(0);

        for (int s = 0; s < K; s++) {
            for (int it = 0; it < SA_PATHS_PER_SHOP; it++) {
                int prev = -1, cur = s;
                uint8_t len = 0;
                int seq[SA_MAXLEN];
                int seqlen = 0;

                for (int step = 0; step < SA_MAXLEN; step++) {
                    int nxt = rand_next_nb(prev, cur);
                    if (nxt < 0)
                        break;
                    prev = cur;
                    cur = nxt;

                    if (cur >= K) {
                        if (len < 63) {
                            seq[seqlen++] = cur;
                            len++;
                        }
                    } else {
                        if (len == 0)
                            break;

                        SAPath p;
                        p.dest = (uint8_t)cur;
                        p.len = len;
                        p.bits = 0;
                        paths.push_back(p);
                        int pid = (int)paths.size() - 1;

                        // occurrences (unordered_map廃止：len<=14なので線形で十分)
                        pair<int, uint64_t> occs_local[SA_MAXLEN];
                        int ocn = 0;
                        for (int i = 0; i < seqlen; i++) {
                            int v = seq[i];
                            if (!inPool[v])
                                continue;
                            uint64_t mask = (1ULL << i);
                            int j = 0;
                            for (; j < ocn; j++) {
                                if (occs_local[j].first == v) {
                                    occs_local[j].second |= mask;
                                    break;
                                }
                            }
                            if (j == ocn) {
                                occs_local[ocn++] = {v, mask};
                            }
                        }

                        for (int j = 0; j < ocn; j++) {
                            int v = occs_local[j].first;
                            uint64_t m = occs_local[j].second;
                            occFlat.push_back({v, m});
                            treeAffects[v].push_back({pid, m});
                        }
                        occStart.push_back((uint32_t)occFlat.size());
                        break;
                    }
                }
            }
        }

        int P = (int)paths.size();
        if (P == 0) {
            vector<int> ret;
            for (int i = 0; i < min(TARGET_FLIPS_INIT, pool); i++)
                ret.push_back(cand[i]);
            return ret;
        }

        vector<char> x(N, 0);
        for (int i = 0; i < min(TARGET_FLIPS_INIT, pool); i++)
            x[cand[i]] = 1;

        vector<int> ones, zeros;
        vector<int> posOnes(N, -1), posZeros(N, -1);
        ones.reserve(TARGET_FLIPS_INIT);
        zeros.reserve(pool);

        auto add_vec = [&](vector<int>& vec, vector<int>& pos, int v) {
            pos[v] = (int)vec.size();
            vec.push_back(v);
        };
        auto remove_vec = [&](vector<int>& vec, vector<int>& pos, int v) {
            int i = pos[v];
            int last = vec.back();
            vec[i] = last;
            pos[last] = i;
            vec.pop_back();
            pos[v] = -1;
        };
        auto flip_var = [&](int v) {
            if (x[v]) {
                x[v] = 0;
                remove_vec(ones, posOnes, v);
                add_vec(zeros, posZeros, v);
            } else {
                x[v] = 1;
                remove_vec(zeros, posZeros, v);
                add_vec(ones, posOnes, v);
            }
        };

        for (int v : cand)
            (x[v] ? add_vec(ones, posOnes, v) : add_vec(zeros, posZeros, v));

        // freqMap は一旦 unordered_map のまま（ここは1回だけなので主戦場ではない）
        struct U64FastHash {
            size_t operator()(uint64_t x) const noexcept {
                x ^= x >> 33;
                x *= 0xff51afd7ed558ccdULL;
                x ^= x >> 33;
                x *= 0xc4ceb9fe1a85ec53ULL;
                x ^= x >> 33;
                return (size_t)x;
            }
        };
        vector<unordered_map<uint64_t, int, U64FastHash>> freqMap(K);
        vector<int> uniq(K, 0);
        for (int i = 0; i < K; i++)
            freqMap[i].reserve(4096);

        auto inc_sig = [&](int dest, uint64_t sig, long long& obj) {
            auto& mp = freqMap[dest];
            auto it = mp.find(sig);
            if (it == mp.end()) {
                mp.emplace(sig, 1);
                uniq[dest]++;
                obj++;
            } else
                it->second++;
        };
        auto dec_sig = [&](int dest, uint64_t sig, long long& obj) {
            auto& mp = freqMap[dest];
            auto it = mp.find(sig);
            it->second--;
            if (it->second == 0) {
                mp.erase(it);
                uniq[dest]--;
                obj--;
            }
        };

        // init bits
        for (int pid = 0; pid < P; pid++) {
            uint64_t bits = 0;
            uint32_t L = occStart[pid], R = occStart[pid + 1];
            for (uint32_t i = L; i < R; i++) {
                int v = occFlat[i].first;
                uint64_t mask = occFlat[i].second;
                if (x[v])
                    bits |= mask;
            }
            paths[pid].bits = bits;
        }

        long long obj = 0;
        for (int i = 0; i < K; i++) {
            freqMap[i].clear();
            uniq[i] = 0;
        }

        for (int pid = 0; pid < P; pid++) {
            uint64_t sig = pack_sig(paths[pid].len, paths[pid].bits);
            inc_sig(paths[pid].dest, sig, obj);
        }

        vector<uint64_t> dm(P, 0);
        vector<int> touched;
        touched.reserve(4096);

        auto do_swap = [&](int a, int b) {
            touched.clear();
            for (auto [pid, mask] : treeAffects[a]) {
                if (dm[pid] == 0)
                    touched.push_back(pid);
                dm[pid] ^= mask;
            }
            for (auto [pid, mask] : treeAffects[b]) {
                if (dm[pid] == 0)
                    touched.push_back(pid);
                dm[pid] ^= mask;
            }
            for (int pid : touched) {
                uint64_t delta = dm[pid];
                dm[pid] = 0;
                if (delta == 0)
                    continue;
                int dest = paths[pid].dest;
                uint64_t oldSig = pack_sig(paths[pid].len, paths[pid].bits);
                dec_sig(dest, oldSig, obj);
                paths[pid].bits ^= delta;
                uint64_t newSig = pack_sig(paths[pid].len, paths[pid].bits);
                inc_sig(dest, newSig, obj);
            }
            flip_var(a);
            flip_var(b);
        };

        long long bestObj = obj;
        vector<char> bestX = x;

        const double logratio = log(SA_FLIP_T1 / SA_FLIP_T0);
        for (int it = 0; it < SA_FLIP_ITERS; it++) {
            double t = SA_FLIP_T0 * exp(logratio * (double)it / (double)SA_FLIP_ITERS);
            int a = ones[rng.next_int((int)ones.size())];
            int b = zeros[rng.next_int((int)zeros.size())];
            long long before = obj;
            do_swap(a, b);
            long long delta = obj - before;

            bool accept = false;
            if (delta >= 0)
                accept = true;
            else {
                double prob = exp((double)delta / t);
                if (rng.next_double() < prob)
                    accept = true;
            }
            if (!accept)
                do_swap(a, b);
            else if (obj > bestObj) {
                bestObj = obj;
                bestX = x;
            }
        }

        vector<int> flips;
        flips.reserve(TARGET_FLIPS_INIT);
        for (int v : cand)
            if (bestX[v])
                flips.push_back(v);
        if ((int)flips.size() > TARGET_FLIPS_INIT)
            flips.resize(TARGET_FLIPS_INIT);
        if ((int)flips.size() < TARGET_FLIPS_INIT) {
            for (int v : cand)
                if (!bestX[v]) {
                    flips.push_back(v);
                    if ((int)flips.size() == TARGET_FLIPS_INIT)
                        break;
                }
        }
        return flips;
    }

    // ===================== Simulator =====================
    struct Simulator {
        const Solver* sol;
        int N, K, T;

        vector<char> isR;
        int Rcnt = 0;
        int initR = 0;

        int pos = 0;
        int forbidden = -1;
        uint8_t coneLen = 0;
        uint64_t coneBits = 0;
        int steps = 0;
        bool needReset = false;

        vector<FastSet> used;
        vector<uint16_t> ptr;

        // BFS cache with stamps
        int S = 0;
        vector<int> dist;
        vector<int> parent;
        vector<uint32_t> vis;
        uint32_t curStamp = 1;

        // decode tables (division/mod 排除)
        vector<uint16_t> prev1Of;  // prev+1
        vector<uint16_t> curOf;

        // reusable buffers
        vector<int> q;
        vector<int> tmpPath;

        vector<int> flipRank;

        // heap buffer for pick_segment
        struct Item {
            long long m;
            int first;
            uint16_t idxInGroup;
            int segIdx;
        };
        vector<Item> heapBuf;

        explicit Simulator(const Solver* s) : sol(s), N(s->N), K(s->K), T(s->T) {
            isR.assign(N, 0);
            used.assign(K, {});
            int expected = max(2048, T + 32);
            for (int i = 0; i < K; i++)
                used[i].init(expected);
            ptr.assign((size_t)K * (size_t)N, 0);

            S = (N + 1) * N;
            dist.assign(S, 0);
            parent.assign(S, -1);
            vis.assign(S, 0);
            curStamp = 1;

            prev1Of.assign(S, 0);
            curOf.assign(S, 0);
            for (int p = -1; p < N; p++) {
                int base = (p + 1) * N;
                uint16_t p1 = (uint16_t)(p + 1);
                for (int u = 0; u < N; u++) {
                    int id = base + u;
                    prev1Of[id] = p1;
                    curOf[id] = (uint16_t)u;
                }
            }

            q.reserve(S);
            tmpPath.reserve(64);
            heapBuf.reserve(128);

            flipRank.assign(N, (int)1e9);
        }

        inline int encode_state(int prev, int cur) const { return (prev + 1) * N + cur; }

        inline void stamp_next() {
            curStamp++;
            if (curStamp == 0) {
                fill(vis.begin(), vis.end(), 0);
                curStamp = 1;
            }
        }

        inline int get_dist_id(int id) const {
            return (vis[id] == curStamp ? dist[id] : INT_MAX);
        }

        inline void reconstruct_path(int startId, int endId, vector<int>& out) {
            out.clear();
            int curId = endId;
            while (curId != startId && curId >= 0) {
                out.push_back((int)curOf[curId]);
                curId = parent[curId];
            }
            reverse(out.begin(), out.end());
        }

        void nb_bfs_all(int startPrev, int startCur) {
            stamp_next();
            q.clear();
            int start = encode_state(startPrev, startCur);
            vis[start] = curStamp;
            dist[start] = 0;
            parent[start] = -1;
            q.push_back(start);

            size_t head = 0;
            while (head < q.size()) {
                int id = q[head++];
                int p = (int)prev1Of[id] - 1;
                int u = (int)curOf[id];
                int du = dist[id];
                for (int v : sol->g[u]) {
                    if (v == p)
                        continue;
                    int nid = encode_state(u, v);
                    if (vis[nid] != curStamp) {
                        vis[nid] = curStamp;
                        dist[nid] = du + 1;
                        parent[nid] = id;
                        q.push_back(nid);
                    }
                }
            }
        }

        bool path_to_any_shop(int startPrev, int startCur, vector<int>& out) {
            stamp_next();
            q.clear();
            int start = encode_state(startPrev, startCur);
            vis[start] = curStamp;
            dist[start] = 0;
            parent[start] = -1;
            q.push_back(start);

            int goal = -1;
            size_t head = 0;
            while (head < q.size()) {
                int id = q[head++];
                int u = (int)curOf[id];
                int d = dist[id];
                if (u < K && d > 0) {
                    goal = id;
                    break;
                }
                int p = (int)prev1Of[id] - 1;
                for (int v : sol->g[u]) {
                    if (v == p)
                        continue;
                    int nid = encode_state(u, v);
                    if (vis[nid] != curStamp) {
                        vis[nid] = curStamp;
                        dist[nid] = d + 1;
                        parent[nid] = id;
                        q.push_back(nid);
                    }
                }
            }
            if (goal < 0) {
                out.clear();
                return false;
            }
            reconstruct_path(start, goal, out);
            return true;
        }

        inline uint16_t& ptr_ref(int s, int first) {
            return ptr[(size_t)s * (size_t)N + (size_t)first];
        }

        inline void reset_all_ptrs() {
            fill(ptr.begin(), ptr.end(), 0);
            needReset = false;
        }

        inline void reset_all_state() {
            fill(isR.begin(), isR.end(), 0);
            for (int i = 0; i < K; i++)
                used[i].clear();
            reset_all_ptrs();

            Rcnt = 0;
            initR = 0;
            pos = 0;
            forbidden = -1;
            coneLen = 0;
            coneBits = 0;
            steps = 0;
            needReset = false;

            fill(flipRank.begin(), flipRank.end(), (int)1e9);
        }

        inline void emit(int a, vector<int>* out, bool record) {
            if (record && out)
                out->push_back(a);
        }

        inline void do_move(int to, vector<int>* out, bool record) {
            emit(to, out, record);
            steps++;

            int from = pos;
            pos = to;
            forbidden = from;

            if (pos >= K) {
                if (coneLen < 63) {
                    if (isR[pos])
                        coneBits |= (1ULL << coneLen);
                    coneLen++;
                }
            } else {
                uint64_t sig = pack_sig((int)coneLen, coneBits);
                used[pos].insert(sig);
                coneLen = 0;
                coneBits = 0;
                if (needReset)
                    reset_all_ptrs();
            }
        }

        inline void do_flip(vector<int>* out, bool record) {
            if (steps >= T)
                return;
            if (pos < K)
                return;
            if (isR[pos])
                return;
            if (Rcnt >= FLIP_MAX)
                return;
            emit(-1, out, record);
            steps++;
            isR[pos] = 1;
            Rcnt++;
            needReset = true;
        }

        inline int allowed_quota() const {
            int q = INIT_FLIPS_QUOTA + steps / FLIP_INTERVAL;
            if (q > FLIP_MAX)
                q = FLIP_MAX;
            return q;
        }

        inline int desired_R(const Params& p) const {
            if (T <= 0)
                return initR;
            double t = (double)steps / (double)T;
            double q = pow(t, p.gamma);
            int fr = min(p.finalR, FLIP_MAX);
            int want = (int)llround((double)initR + (double)(fr - initR) * q);
            if (want < initR)
                want = initR;
            if (want > fr)
                want = fr;
            int quota = allowed_quota();
            if (want > quota)
                want = quota;
            return want;
        }

        inline bool should_flip_here(int v, const Params& p) const {
            int want = desired_R(p);
            if (Rcnt >= want)
                return false;
            bool important = (sol->rankTree[v] <= p.rankLimit);
            bool behind = (want - Rcnt >= 6);
            bool inEarly = (flipRank[v] < want + 4);
            return (important && inEarly) || behind;
        }

        inline uint64_t sig_from_segment(const Segment& seg) const {
            uint64_t bits = 0;
            for (int i = 0; i < (int)seg.treeLen; i++) {
                int v = (int)seg.trees[i];
                if (isR[v])
                    bits |= (1ULL << i);
            }
            return pack_sig((int)seg.treeLen, bits);
        }

        int pick_segment(int s, int remaining, const Params& p, RNG& noiseRng) {
            auto comp = [](const Item& a, const Item& b) {
                if (a.m != b.m)
                    return a.m > b.m;  // min-heap
                return a.segIdx > b.segIdx;
            };

            const ShopPool& pool = sol->pools[s];
            heapBuf.clear();

            for (int v : sol->g[s]) {
                if (v == forbidden)
                    continue;
                const auto& grp = pool.group[v];
                if (grp.empty())
                    continue;
                uint16_t pi = ptr_ref(s, v);
                if (pi >= grp.size())
                    continue;
                int segIdx = grp[pi];
                const Segment& seg = pool.segs[segIdx];
                if ((int)seg.cost > remaining)
                    continue;

                long long m = (long long)seg.cost * (long long)p.costW
                              + (long long)used[seg.dest].size() * (long long)p.balanceW;
                if (p.noise > 0)
                    m += noiseRng.next_int(p.noise + 1);

                heapBuf.push_back(Item{m, v, pi, segIdx});
                push_heap(heapBuf.begin(), heapBuf.end(), comp);
            }

            while (!heapBuf.empty()) {
                pop_heap(heapBuf.begin(), heapBuf.end(), comp);
                Item it = heapBuf.back();
                heapBuf.pop_back();

                int first = it.first;
                if ((int)ptr_ref(s, first) != (int)it.idxInGroup)
                    continue;  // stale

                const auto& grp = pool.group[first];
                int segLocalIdx = grp[it.idxInGroup];
                const Segment& seg = pool.segs[segLocalIdx];

                if ((int)seg.cost > remaining) {
                    ptr_ref(s, first)++;
                    continue;
                }

                uint64_t sig = sig_from_segment(seg);
                if (!used[seg.dest].contains(sig)) {
                    ptr_ref(s, first)++;
                    return segLocalIdx;
                }

                // advance
                ptr_ref(s, first)++;
                uint16_t np = ptr_ref(s, first);
                if (np < grp.size()) {
                    int nseg = grp[np];
                    const Segment& s2 = pool.segs[nseg];
                    if ((int)s2.cost <= remaining) {
                        long long m = (long long)s2.cost * (long long)p.costW
                                      + (long long)used[s2.dest].size() * (long long)p.balanceW;
                        if (p.noise > 0)
                            m += noiseRng.next_int(p.noise + 1);
                        heapBuf.push_back(Item{m, first, np, nseg});
                        push_heap(heapBuf.begin(), heapBuf.end(), comp);
                    }
                }
            }
            return -1;
        }

        void paint_stage(const vector<int>& flipOrder, vector<int>* out, bool record) {
            int paintTargets = min(TARGET_FLIPS_INIT, (int)flipOrder.size());
            if (paintTargets <= 0)
                return;

            vector<int> candidates;
            candidates.reserve(paintTargets);
            for (int i = 0; i < paintTargets; i++) {
                if (flipOrder[i] >= K && flipOrder[i] < N)
                    candidates.push_back(flipOrder[i]);
            }

            while (steps < T && steps < PAINT_BUDGET_INIT && Rcnt < (int)candidates.size() && Rcnt < FLIP_MAX) {
                nb_bfs_all(forbidden, pos);
                int startId = encode_state(forbidden, pos);

                int bestV = -1, bestEnd = -1, bestD = INT_MAX;
                for (int v : candidates) {
                    if (isR[v])
                        continue;
                    for (int pidx = 0; pidx <= N; pidx++) {
                        int id = pidx * N + v;
                        int d = get_dist_id(id);
                        if (d < bestD) {
                            bestD = d;
                            bestV = v;
                            bestEnd = id;
                        }
                    }
                }
                if (bestV < 0 || bestD == INT_MAX)
                    break;
                if (steps + bestD + 1 > PAINT_BUDGET_INIT)
                    break;
                if (steps + bestD + 1 > T)
                    break;

                reconstruct_path(startId, bestEnd, tmpPath);
                for (int to : tmpPath) {
                    if (steps >= T || steps >= PAINT_BUDGET_INIT)
                        break;
                    do_move(to, out, record);
                }
                if (steps < T && steps < PAINT_BUDGET_INIT && pos == bestV)
                    do_flip(out, record);
            }

            if (pos >= K && steps < T) {
                if (path_to_any_shop(forbidden, pos, tmpPath)) {
                    for (int to : tmpPath) {
                        if (steps >= T)
                            break;
                        do_move(to, out, record);
                    }
                }
            }
            reset_all_ptrs();
            initR = Rcnt;
        }

        struct SimResult {
            long long score = 0;
            int steps = 0;
        };

        SimResult simulate(const vector<int>& flipOrder, const Params& p, uint64_t noiseSeed,
            vector<int>* out, bool record) {
            reset_all_state();

            for (int i = 0; i < (int)flipOrder.size(); i++) {
                int v = flipOrder[i];
                if (v >= 0 && v < N)
                    flipRank[v] = i;
            }

            RNG noiseRng(splitmix64(noiseSeed ^ 0xA7F00DULL) | 1ULL);

            paint_stage(flipOrder, out, record);

            while (steps < T) {
                if (pos >= K) {
                    if (!path_to_any_shop(forbidden, pos, tmpPath) || tmpPath.empty()) {
                        int to = -1;
                        for (int v : sol->g[pos])
                            if (v != forbidden) {
                                to = v;
                                break;
                            }
                        if (to < 0)
                            to = sol->g[pos][0];
                        do_move(to, out, record);
                        if (pos >= K && !isR[pos] && should_flip_here(pos, p) && steps < T)
                            do_flip(out, record);
                    } else {
                        for (int to : tmpPath) {
                            if (steps >= T)
                                break;
                            do_move(to, out, record);
                            if (pos >= K && !isR[pos] && should_flip_here(pos, p) && steps < T)
                                do_flip(out, record);
                        }
                    }
                    continue;
                }

                int s = pos;
                int remaining = T - steps;
                int segIdx = pick_segment(s, remaining, p, noiseRng);

                if (segIdx >= 0) {
                    const Segment& seg = sol->pools[s].segs[segIdx];
                    for (int i = 0; i < (int)seg.cost && steps < T; i++) {
                        int to = (int)seg.moves[i];
                        do_move(to, out, record);
                        if (steps >= T)
                            break;

                        if (pos >= K && !isR[pos] && should_flip_here(pos, p)) {
                            // ★ appearsLater の O(L^2) チェックを削除（速度優先）
                            do_flip(out, record);
                        }
                    }
                    continue;
                }

                {
                    int want = desired_R(p);
                    bool needMore = (Rcnt < want);
                    if (needMore && steps + 2 <= T) {
                        int limitRank = min((int)flipOrder.size(), want + 8);
                        if (limitRank < min((int)flipOrder.size(), INIT_FLIPS_QUOTA))
                            limitRank = min((int)flipOrder.size(), INIT_FLIPS_QUOTA);

                        nb_bfs_all(forbidden, pos);
                        int startId = encode_state(forbidden, pos);

                        int bestV = -1, bestEnd = -1, bestD = INT_MAX;
                        for (int r = 0; r < limitRank; r++) {
                            int v = flipOrder[r];
                            if (v < K || v >= N)
                                continue;
                            if (isR[v])
                                continue;
                            if (!(sol->rankTree[v] <= p.rankLimit || (want - Rcnt >= 6)))
                                continue;
                            for (int pidx = 0; pidx <= N; pidx++) {
                                int id = pidx * N + v;
                                int d = get_dist_id(id);
                                if (d < bestD) {
                                    bestD = d;
                                    bestV = v;
                                    bestEnd = id;
                                }
                            }
                        }

                        if (bestV >= 0 && bestD != INT_MAX && steps + bestD + 1 <= T) {
                            reconstruct_path(startId, bestEnd, tmpPath);
                            for (int to : tmpPath) {
                                if (steps >= T)
                                    break;
                                do_move(to, out, record);
                            }
                            if (steps < T && pos == bestV)
                                do_flip(out, record);
                            continue;
                        }
                    }
                }

                int to = -1;
                for (int v : sol->g[s]) {
                    if (v == forbidden)
                        continue;
                    if (v < K) {
                        to = v;
                        break;
                    }
                }
                if (to < 0) {
                    for (int v : sol->g[s])
                        if (v != forbidden) {
                            to = v;
                            break;
                        }
                }
                if (to < 0)
                    to = sol->g[s][0];
                do_move(to, out, record);
                if (pos >= K && !isR[pos] && should_flip_here(pos, p) && steps < T)
                    do_flip(out, record);
            }

            long long score = 0;
            for (int i = 0; i < K; i++)
                score += used[i].size();
            return SimResult{score, steps};
        }
    };

    // ===================== SA: optimize flipOrder =====================
    vector<int> sa_optimize_order(Simulator& sim,
        const vector<int>& initOrder,
        const vector<int>& candidateTrees,
        const Params& baseParams,
        const Timer& timer,
        double endTime,
        uint64_t seedBase) {

        vector<int> cur = initOrder;
        vector<int> best = initOrder;

        Params pEval = baseParams;
        pEval.noise = 0;

        long long curScore = sim.simulate(cur, pEval, seedBase, nullptr, false).score;
        long long bestScore = curScore;

        double start = timer.elapsed();
        double denom = max(1e-9, endTime - start);
        const double logratio = log(SA_ORDER_T1 / SA_ORDER_T0);

        while (timer.elapsed() < endTime) {
            double now = timer.elapsed();
            double prog = (now - start) / denom;
            if (prog < 0)
                prog = 0;
            if (prog > 1)
                prog = 1;
            double temp = SA_ORDER_T0 * exp(logratio * prog);

            vector<int> nxt = cur;

            double r = rng.next_double();
            if (r < 0.45) {
                int i = rng.next_int((int)nxt.size());
                int j = rng.next_int((int)nxt.size());
                if (i != j)
                    swap(nxt[i], nxt[j]);
            } else if (r < 0.75) {
                int i = rng.next_int((int)nxt.size());
                int j = rng.next_int((int)nxt.size());
                if (i != j) {
                    int v = nxt[i];
                    nxt.erase(nxt.begin() + i);
                    nxt.insert(nxt.begin() + j, v);
                }
            } else {
                int i = rng.next_int((int)nxt.size());
                int v = candidateTrees[rng.next_int((int)candidateTrees.size())];
                if (v >= K && v < N) {
                    int p = -1;
                    for (int j = 0; j < (int)nxt.size(); j++)
                        if (nxt[j] == v) {
                            p = j;
                            break;
                        }
                    if (p >= 0)
                        swap(nxt[i], nxt[p]);
                    else
                        nxt[i] = v;
                }
            }

            long long nxtScore = sim.simulate(nxt, pEval, seedBase, nullptr, false).score;
            long long diff = nxtScore - curScore;

            bool accept = false;
            if (diff >= 0)
                accept = true;
            else {
                double prob = exp((double)diff / max(1e-9, temp));
                if (rng.next_double() < prob)
                    accept = true;
            }

            if (accept) {
                cur = std::move(nxt);
                curScore = nxtScore;
                if (curScore > bestScore) {
                    bestScore = curScore;
                    best = cur;
                }
            }
        }
        return best;
    }

    // ===================== Param mutation (light) =====================
    Params mutate_params(const Params& cur, double progress01) {
        Params p = cur;
        int which = rng.next_int(5);
        if (which == 0) {
            int step = (rng.next_int(2) ? 5 : -5);
            if (rng.next_int(5) == 0)
                step *= 2;
            p.finalR = max(20, min(80, p.finalR + step));
        } else if (which == 1) {
            double mul = exp(((rng.next_double() * 2.0) - 1.0) * 0.16);
            p.gamma = max(0.7, min(2.9, p.gamma * mul));
        } else if (which == 2) {
            int step = (rng.next_int(2) ? 10 : -10);
            p.costW = max(80, min(520, p.costW + step));
        } else if (which == 3) {
            int step = (rng.next_int(2) ? 1 : -1);
            p.balanceW = max(0, min(8, p.balanceW + step));
        } else {
            int step = (rng.next_int(2) ? 8 : -8);
            p.rankLimit = max(15, min(89, p.rankLimit + step));
        }
        int baseNoise = 10;
        p.noise = (int)llround(baseNoise * (1.0 - 0.65 * progress01));
        return p;
    }

    Params sa_optimize_params(Simulator& sim,
        const vector<int>& fixedOrder,
        const Params& initP,
        const Timer& timer,
        double endTime,
        uint64_t seedBase) {

        Params cur = initP, bestP = initP;

        Params evalP = cur;
        evalP.noise = 0;
        long long curScore = sim.simulate(fixedOrder, evalP, seedBase, nullptr, false).score;
        long long bestScore = curScore;

        double t0 = 5.0, t1 = 0.01;
        double start = timer.elapsed();
        const double logratio = log(t1 / t0);

        int iters = 0;
        while (timer.elapsed() < endTime) {
            double now = timer.elapsed();
            double prog = 0.0;
            if (endTime > start + 1e-9)
                prog = (now - start) / (endTime - start);
            if (prog < 0)
                prog = 0;
            if (prog > 1)
                prog = 1;
            double temp = t0 * exp(logratio * prog);

            Params nxt = mutate_params(cur, prog);

            Params nxtEval = nxt;
            nxtEval.noise = 0;
            uint64_t sseed = splitmix64(seedBase + (uint64_t)iters * 0x9e3779b97f4a7c15ULL);
            long long sc = sim.simulate(fixedOrder, nxtEval, sseed, nullptr, false).score;
            long long delta = sc - curScore;

            bool accept = false;
            if (delta >= 0)
                accept = true;
            else {
                double prob = exp((double)delta / temp);
                if (rng.next_double() < prob)
                    accept = true;
            }
            if (accept) {
                cur = nxt;
                curScore = sc;
            }
            if (sc > bestScore) {
                bestScore = sc;
                bestP = nxt;
            }
            iters++;
        }
        return bestP;
    }

    static inline void append_int(string& s, int x) {
        if (x == 0) {
            s.push_back('0');
            s.push_back('\n');
            return;
        }
        if (x < 0) {
            s.push_back('-');
            x = -x;
        }
        char buf[16];
        int n = 0;
        while (x > 0) {
            buf[n++] = char('0' + (x % 10));
            x /= 10;
        }
        for (int i = n - 1; i >= 0; i--)
            s.push_back(buf[i]);
        s.push_back('\n');
    }

    void run() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);

        cin >> N >> M >> K >> T;
        g.assign(N, {});
        edges.clear();
        edges.reserve(M);

        for (int i = 0; i < M; i++) {
            int a, b;
            cin >> a >> b;
            g[a].push_back(b);
            g[b].push_back(a);
            edges.push_back({a, b});
        }
        coords.clear();
        coords.reserve(N);
        for (int i = 0; i < N; i++) {
            int x, y;
            cin >> x >> y;
            coords.push_back({x, y});
        }

        Timer timer;

        // ★ 1950ms ギリギリまで使う：出力分の安全マージンだけ確保
        const double TIME_LIMIT = 1.95;
        const double OUT_MARGIN = 0.008;  // ここを 0.004〜0.012 で詰めるとさらにギリギリにできる
        const double DEADLINE = TIME_LIMIT - OUT_MARGIN;

        uint64_t baseSeed = compute_seed();
        uint64_t timeSeed = chrono::high_resolution_clock::now().time_since_epoch().count();
        rng = RNG(splitmix64(baseSeed ^ timeSeed));

        polyP = 1000003ULL + (rng.next_u64() | 1ULL);

        pools.assign(K, {});
        for (int s = 0; s < K; s++) {
            pools[s] = build_pool(s, D_BUILD);
            if (timer.elapsed() > DEADLINE * 0.70)
                break;  // 少し攻めて 70%
        }

        vector<int> freqPool(N, 0);
        for (int s = 0; s < (int)pools.size(); s++) {
            for (const auto& seg : pools[s].segs) {
                for (int i = 0; i < (int)seg.treeLen; i++)
                    freqPool[seg.trees[i]]++;
            }
        }
        vector<int> freqRW = estimate_tree_freq_rw();

        int maxPool = 1, maxRW = 1;
        for (int v = K; v < N; v++) {
            maxPool = max(maxPool, freqPool[v]);
            maxRW = max(maxRW, freqRW[v]);
        }
        double scale = 0.25 * (double)maxPool / (double)maxRW;

        vector<int> rankedTrees;
        rankedTrees.reserve(max(0, N - K));
        for (int v = K; v < N; v++)
            rankedTrees.push_back(v);
        sort(rankedTrees.begin(), rankedTrees.end(), [&](int a, int b) {
            double sa = (double)freqPool[a] + scale * (double)freqRW[a];
            double sb = (double)freqPool[b] + scale * (double)freqRW[b];
            if (sa != sb)
                return sa > sb;
            return a < b;
        });

        rankTree.assign(N, (int)1e9);
        for (int i = 0; i < (int)rankedTrees.size(); i++)
            rankTree[rankedTrees[i]] = i;

        vector<int> initTargets = choose_flips_SA(rankedTrees);

        vector<int> candTrees;
        for (int i = 0; i < (int)rankedTrees.size() && (int)candTrees.size() < FLIP_CAND; i++)
            candTrees.push_back(rankedTrees[i]);

        if (candTrees.empty()) {
            int pos = 0, forb = -1;
            string out;
            out.reserve((size_t)T * 4);
            for (int t = 0; t < T; t++) {
                int to = -1;
                for (int v : g[pos])
                    if (v != forb) {
                        to = v;
                        break;
                    }
                if (to < 0)
                    to = g[pos][0];
                append_int(out, to);
                forb = pos;
                pos = to;
            }
            cout << out;
            return;
        }

        vector<int> initOrder;
        initOrder.reserve(FLIP_MAX);
        vector<char> inOrd(N, 0);
        for (int v : initTargets) {
            if (v >= K && v < N && !inOrd[v]) {
                initOrder.push_back(v);
                inOrd[v] = 1;
                if ((int)initOrder.size() == FLIP_MAX)
                    break;
            }
        }
        for (int v : rankedTrees) {
            if ((int)initOrder.size() == FLIP_MAX)
                break;
            if (!inOrd[v]) {
                initOrder.push_back(v);
                inOrd[v] = 1;
            }
        }
        for (int v = K; v < N && (int)initOrder.size() < FLIP_MAX; v++)
            if (!inOrd[v])
                initOrder.push_back(v);

        Params p0;
        Simulator sim(this);

        // 時間配分（残りをきっちり使い切るのは最後の rollouts に任せる）
        double reserveFinal = 0.10;
        double reserveParam = 0.32;
        double endOrder = DEADLINE - (reserveParam + reserveFinal);
        if (endOrder < timer.elapsed() + 0.05)
            endOrder = timer.elapsed();  // 余裕なければスキップ
        double endParam = DEADLINE - reserveFinal;

        uint64_t evalSeed = splitmix64(baseSeed ^ 0xC0FFEEULL);

        vector<int> bestOrder = initOrder;
        if (timer.elapsed() < endOrder - 1e-6) {
            bestOrder = sa_optimize_order(sim, initOrder, candTrees, p0, timer, endOrder, evalSeed);
        }

        Params bestP = p0;
        if (timer.elapsed() < endParam - 1e-6) {
            bestP = sa_optimize_params(sim, bestOrder, p0, timer, endParam, evalSeed ^ 0x1234567ULL);
        }

        vector<int> bestOut;
        long long bestScore = -1;

        // ★ 残り時間を見積もってギリギリまで multi-start を回す
        double avgSim = 0.03;  // 初期推定
        int simCnt = 0;
        int tries = 0;

        while (true) {
            double now = timer.elapsed();
            double left = DEADLINE - now;
            if (left <= 0.0)
                break;

            double est = (simCnt > 0 ? avgSim : 0.03);
            if (left < est * 1.10)
                break;  // 近すぎたら次を回さない（TLE防止）

            uint64_t sseed = splitmix64(baseSeed + (uint64_t)tries * 0x9e3779b97f4a7c15ULL + rng.next_u64());
            vector<int> out;
            out.reserve(T + 16);

            double t0s = timer.elapsed();
            auto res = sim.simulate(bestOrder, bestP, sseed, &out, true);
            double t1s = timer.elapsed();
            double dt = t1s - t0s;

            avgSim = (avgSim * simCnt + dt) / (simCnt + 1);
            simCnt++;

            if (res.score > bestScore) {
                bestScore = res.score;
                bestOut.swap(out);
            }
            tries++;
        }

        if (bestOut.empty()) {
            vector<int> out;
            out.reserve(T + 16);
            auto res = sim.simulate(bestOrder, bestP, evalSeed, &out, true);
            bestScore = res.score;
            bestOut.swap(out);
        }

        string outStr;
        outStr.reserve((size_t)bestOut.size() * 4);
        for (int a : bestOut)
            append_int(outStr, a);
        cout << outStr;

        cerr << "[DATA] score = " << bestScore << endl;
    }
};

int main() {
    Solver s;
    s.run();
    return 0;
}
