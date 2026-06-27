#include <bits/stdc++.h>
using namespace std;

struct Pt {
    int r, c;
};
static inline int manhattan(const Pt& a, const Pt& b) {
    return abs(a.r - b.r) + abs(a.c - b.c);
}
static constexpr Pt START{0, 0};

// -------------------- fast RNG (xorshift) --------------------
struct XorShift {
    uint32_t x = 2463534242u;
    uint32_t next_u32() {
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return x;
    }
    // [0,1)
    float next_f() {
        return (next_u32() >> 8) * (1.0f / 16777216.0f);
    }
    int next_int(int lo, int hi) {  // inclusive
        return lo + (int)(next_u32() % (uint32_t)(hi - lo + 1));
    }
};

enum class InsertType
{
    LEAF_GAP,
    WRAP_SUBTREE
};
struct BestMoveInit {
    long long delta = (1LL << 60);
    int add_id = -1;
    InsertType type = InsertType::LEAF_GAP;
    int gap = -1;
    int wrap_id = -1;
    int l = -1, r = -1;
    int ori = 0;
};

struct Elem {
    int id;
    Pt p;
};

static inline int total_K(const vector<Elem>& seq) {
    if (seq.empty())
        return 0;
    int K = manhattan(START, seq[0].p);
    for (int i = 1; i < (int)seq.size(); i++) {
        K += manhattan(seq[i - 1].p, seq[i].p);
    }
    return K;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;

    vector<vector<string>> a(N, vector<string>(N));
    unordered_map<string, int> mp;
    mp.reserve(N * N * 2);

    vector<array<Pt, 2>> pos;
    vector<int> cnt;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cin >> a[i][j];
            auto it = mp.find(a[i][j]);
            int id;
            if (it == mp.end()) {
                id = (int)mp.size();
                mp.emplace(a[i][j], id);
                pos.push_back({Pt{-1, -1}, Pt{-1, -1}});
                cnt.push_back(0);
            } else {
                id = it->second;
            }
            int k = cnt[id]++;
            pos[id][k] = Pt{i, j};
        }
    }
    int P = (int)pos.size();  // = N*N/2
    int n = 2 * P;

    // -------------------- initial solution: retroactive greedy --------------------
    XorShift rng;
    vector<char> used(P, 0);

    // Start with one random id as a leaf
    int first_id = rng.next_int(0, P - 1);
    used[first_id] = 1;

    vector<Pt> coords;
    coords.reserve(n);
    coords.push_back(pos[first_id][0]);
    coords.push_back(pos[first_id][1]);

    auto apply_init_insert = [&](const BestMoveInit& mv) {
        int id = mv.add_id;
        Pt p0 = pos[id][0];
        Pt p1 = pos[id][1];
        Pt A = (mv.ori == 0 ? p0 : p1);
        Pt B = (mv.ori == 0 ? p1 : p0);

        if (mv.type == InsertType::LEAF_GAP) {
            int g = mv.gap;
            coords.insert(coords.begin() + g, A);
            coords.insert(coords.begin() + g + 1, B);
        } else {
            // wrap subtree [l..r]
            int l = mv.l, r = mv.r;
            coords.insert(coords.begin() + l, A);
            coords.insert(coords.begin() + r + 2, B);
        }
    };

    // retroactive: add remaining ids one by one with best delta among leaf/wrap
    for (int step = 1; step < P; step++) {
        BestMoveInit bestGlobal;

        for (int cand = 0; cand < P; cand++) {
            if (used[cand])
                continue;
            BestMoveInit bestCand;
            bestCand.add_id = cand;

            Pt p0 = pos[cand][0];
            Pt p1 = pos[cand][1];

            int M = (int)coords.size();

            // leaf: choose any gap
            for (int g = 0; g <= M; g++) {
                Pt prev = (g == 0 ? START : coords[g - 1]);
                bool hasNext = (g < M);
                Pt nxt = hasNext ? coords[g] : Pt{-1, -1};

                int removed = hasNext ? manhattan(prev, nxt) : 0;

                for (int ori = 0; ori < 2; ori++) {
                    Pt A = (ori == 0 ? p0 : p1);
                    Pt B = (ori == 0 ? p1 : p0);
                    int added = 0;
                    added += manhattan(prev, A);
                    added += manhattan(A, B);
                    if (hasNext)
                        added += manhattan(B, nxt);

                    long long delta = (long long)added - (long long)removed;
                    if (delta < bestCand.delta) {
                        bestCand.delta = delta;
                        bestCand.type = InsertType::LEAF_GAP;
                        bestCand.gap = g;
                        bestCand.ori = ori;
                    }
                }
            }

            // wrap: choose subtree by selecting some existing id interval
            for (int wrap = 0; wrap < step; wrap++) {
                // find interval of wrap-th used id
                int u = -1;
                for (int t = 0, c = 0; t < P; t++) {
                    if (used[t]) {
                        if (c == wrap) {
                            u = t;
                            break;
                        }
                        c++;
                    }
                }
                if (u < 0)
                    continue;

                // locate in coords by scanning id nodes
                Pt u0 = pos[u][0], u1 = pos[u][1];
                int l = -1, r = -1;
                for (int i = 0; i < M; i++) {
                    if (coords[i].r == u0.r && coords[i].c == u0.c)
                        l = i;
                    if (coords[i].r == u1.r && coords[i].c == u1.c)
                        r = i;
                }
                if (l < 0 || r < 0)
                    continue;
                if (l > r)
                    swap(l, r);

                Pt prev = (l == 0 ? START : coords[l - 1]);
                Pt first = coords[l];
                Pt last = coords[r];
                bool hasNext = (r + 1 < M);
                Pt nxt = hasNext ? coords[r + 1] : Pt{-1, -1};

                int removed = manhattan(prev, first) + (hasNext ? manhattan(last, nxt) : 0);

                for (int ori = 0; ori < 2; ori++) {
                    Pt A = (ori == 0 ? p0 : p1);
                    Pt B = (ori == 0 ? p1 : p0);
                    int added = 0;
                    added += manhattan(prev, A);
                    added += manhattan(A, first);
                    added += manhattan(last, B);
                    if (hasNext)
                        added += manhattan(B, nxt);

                    long long delta = (long long)added - (long long)removed;
                    if (delta < bestCand.delta) {
                        bestCand.delta = delta;
                        bestCand.type = InsertType::WRAP_SUBTREE;
                        bestCand.wrap_id = u;
                        bestCand.l = l;
                        bestCand.r = r;
                        bestCand.ori = ori;
                    }
                }
            }

            if (bestCand.delta < bestGlobal.delta)
                bestGlobal = bestCand;
        }

        used[bestGlobal.add_id] = 1;
        apply_init_insert(bestGlobal);
    }

    // Build initial sequence (id labels reconstructed by matching positions; heuristic but consistent)
    vector<int> id_of_coord;
    id_of_coord.reserve((int)coords.size());
    vector<vector<pair<int, int>>> at(N, vector<pair<int, int>>(N, {-1, -1}));
    for (int id = 0; id < P; id++) {
        at[pos[id][0].r][pos[id][0].c] = {id, 0};
        at[pos[id][1].r][pos[id][1].c] = {id, 1};
    }
    for (auto& pt : coords) {
        id_of_coord.push_back(at[pt.r][pt.c].first);
    }

    vector<Elem> seq;
    seq.reserve(n);
    for (int i = 0; i < n; i++) {
        seq.push_back(Elem{id_of_coord[i], coords[i]});
    }

    // occ indices
    vector<array<int, 2>> occ(P, {-1, -1});
    auto rebuild_occ = [&]() {
        vector<int> seen(P, 0);
        for (int i = 0; i < n; i++) {
            int id = seq[i].id;
            int k = seen[id]++;
            if (k < 2)
                occ[id][k] = i;
        }
    };
    rebuild_occ();

    int curK = total_K(seq);
    int bestK = curK;

    vector<Pt> best_coords;
    best_coords.reserve(n);
    for (auto& e : seq)
        best_coords.push_back(e.p);

    cerr << "[DATA] score_initial = " << curK << "\n";

    // -------------------- SA --------------------
    const double T0 = 0.8;
    const double T1 = 0.4;
    const double TL_SEC = 1.62;

    // fast exp table
    static constexpr float MAX_X = 20.0f;
    static constexpr int EXP_SCALE = 1024;
    static constexpr int EXP_SIZE = (int)(MAX_X * EXP_SCALE) + 1;
    static array<float, EXP_SIZE> exp_table;
    static bool exp_inited = false;
    if (!exp_inited) {
        for (int i = 0; i < EXP_SIZE; i++)
            exp_table[i] = expf(-(float)i / (float)EXP_SCALE);
        exp_inited = true;
    }

    auto t0 = chrono::high_resolution_clock::now();
    auto now_sec = [&]() -> double {
        auto t = chrono::high_resolution_clock::now();
        return chrono::duration<double>(t - t0).count();
    };

    const double logT0 = log(T0), logT1 = log(T1);

    auto accept_delta = [&](int delta, float invT) -> bool {
        if (delta <= 0)
            return true;
        float x = (float)delta * invT;
        if (x >= MAX_X)
            return false;
        int idx = (int)(x * (float)EXP_SCALE);
        return rng.next_f() < exp_table[idx];
    };

    auto pt_at = [&](int idx) -> Pt {
        if (idx < 0)
            return START;
        return seq[idx].p;
    };

    auto resync_curK = [&]() {
        curK = total_K(seq);
    };

    // -------------------- logs/counters --------------------
    long long iters = 0, acc = 0;
    long long try_reloc = 0, acc_reloc = 0;
    long long try_relocrev = 0, acc_relocrev = 0;
    long long try_loc = 0, acc_loc = 0;
    long long try_glo = 0, acc_glo = 0;
    long long try_ep = 0, acc_ep = 0;
    long long try_rev = 0, acc_rev = 0;
    long long try_flip = 0, acc_flip = 0;
    long long try_swap = 0, acc_swap = 0;
    long long try_swaprev = 0, acc_swaprev = 0;

    double next_log = 0.24;

    // SA loop
    const double invTL = 1.0 / TL_SEC;
    double el = 0.0;
    float invT = 1.0f / (float)T0;

    static constexpr int TIME_CHECK_INTERVAL = 512;  // power of two
    while (true) {
        if ((iters & (TIME_CHECK_INTERVAL - 1)) == 0) {
            el = now_sec();
            if (el >= TL_SEC)
                break;

            while (el >= next_log) {
                next_log += 0.24;
                cerr << "[DATA] score_best = " << bestK << "\n";
                cerr << "[DATA] score_cur = " << curK << "\n";
            }

            double x = el * invTL;
            float T = (float)exp(logT0 + (logT1 - logT0) * x);
            invT = 1.0f / T;
        }

        iters++;
        uint32_t pick = rng.next_u32() % 1000;

        if (pick < 500) {
            // relocate(simple)
            try_reloc++;
            int id = rng.next_int(0, P - 1);
            int i = occ[id][0];
            int j = occ[id][1];
            int l = min(i, j), r = max(i, j);
            int len = r - l + 1;
            if (len == n)
                continue;

            int gap2 = rng.next_int(0, n - len);
            if (gap2 == l)
                continue;

            // calc delta (same formula)
            int gap1 = (gap2 <= l ? gap2 : gap2 + len);

            Pt A = pt_at(l - 1);
            Pt B = pt_at(l);
            Pt C = pt_at(r);
            bool hasRnext = (r + 1 < n);
            Pt D = hasRnext ? pt_at(r + 1) : Pt{-1, -1};

            int old_rem = manhattan(A, B) + (hasRnext ? manhattan(C, D) : 0);
            int new_rem = (hasRnext ? manhattan(A, D) : 0);

            Pt E = pt_at(gap1 - 1);
            bool hasNextI = (gap1 < n);
            Pt F = hasNextI ? pt_at(gap1) : Pt{-1, -1};

            int old_ins = (hasNextI ? manhattan(E, F) : 0);
            int new_ins = manhattan(E, B) + (hasNextI ? manhattan(C, F) : 0);

            int delta = (new_rem + new_ins) - (old_rem + old_ins);

            if (!accept_delta(delta, invT))
                continue;

            acc++;
            acc_reloc++;

            // apply relocate
            vector<Elem> block(seq.begin() + l, seq.begin() + r + 1);
            seq.erase(seq.begin() + l, seq.begin() + r + 1);
            seq.insert(seq.begin() + gap2, block.begin(), block.end());

            curK += delta;
            rebuild_occ();

            if (curK < bestK) {
                bestK = curK;
                best_coords.clear();
                for (auto& e : seq)
                    best_coords.push_back(e.p);
            }

        } else if (pick < 540) {
            // relocate_local (multi-try)
            try_loc++;
            int id = rng.next_int(0, P - 1);
            int i = occ[id][0];
            int j = occ[id][1];
            int l = min(i, j), r = max(i, j);
            int len = r - l + 1;
            if (len == n)
                continue;

            const int W = 28;
            int lo = max(0, l - W);
            int hi = min(n - len, l + W);
            if (lo == hi && lo == l)
                continue;

            const int TR = 5;
            int bestD = INT_MAX;
            int bestG = -1;
            for (int t = 0; t < TR; t++) {
                int g = rng.next_int(lo, hi);
                if (g == l)
                    continue;

                int gap1 = (g <= l ? g : g + len);

                Pt A = pt_at(l - 1);
                Pt B = pt_at(l);
                Pt C = pt_at(r);
                bool hasRnext = (r + 1 < n);
                Pt D = hasRnext ? pt_at(r + 1) : Pt{-1, -1};

                int old_rem = manhattan(A, B) + (hasRnext ? manhattan(C, D) : 0);
                int new_rem = (hasRnext ? manhattan(A, D) : 0);

                Pt E = pt_at(gap1 - 1);
                bool hasNextI = (gap1 < n);
                Pt F = hasNextI ? pt_at(gap1) : Pt{-1, -1};

                int old_ins = (hasNextI ? manhattan(E, F) : 0);
                int new_ins = manhattan(E, B) + (hasNextI ? manhattan(C, F) : 0);

                int d = (new_rem + new_ins) - (old_rem + old_ins);
                if (d < bestD) {
                    bestD = d;
                    bestG = g;
                }
            }
            if (bestG < 0)
                continue;
            if (!accept_delta(bestD, invT))
                continue;

            acc++;
            acc_loc++;

            // apply
            vector<Elem> block(seq.begin() + l, seq.begin() + r + 1);
            seq.erase(seq.begin() + l, seq.begin() + r + 1);
            seq.insert(seq.begin() + bestG, block.begin(), block.end());

            curK += bestD;
            rebuild_occ();

            if (curK < bestK) {
                bestK = curK;
                best_coords.clear();
                for (auto& e : seq)
                    best_coords.push_back(e.p);
            }

        } else if (pick < 570) {
            // relocate_global (multi-try)
            try_glo++;
            int id = rng.next_int(0, P - 1);
            int i = occ[id][0];
            int j = occ[id][1];
            int l = min(i, j), r = max(i, j);
            int len = r - l + 1;
            if (len == n)
                continue;

            const int TR = 6;
            int bestD = INT_MAX;
            int bestG = -1;
            for (int t = 0; t < TR; t++) {
                int g = rng.next_int(0, n - len);
                if (g == l)
                    continue;

                int gap1 = (g <= l ? g : g + len);

                Pt A = pt_at(l - 1);
                Pt B = pt_at(l);
                Pt C = pt_at(r);
                bool hasRnext = (r + 1 < n);
                Pt D = hasRnext ? pt_at(r + 1) : Pt{-1, -1};

                int old_rem = manhattan(A, B) + (hasRnext ? manhattan(C, D) : 0);
                int new_rem = (hasRnext ? manhattan(A, D) : 0);

                Pt E = pt_at(gap1 - 1);
                bool hasNextI = (gap1 < n);
                Pt F = hasNextI ? pt_at(gap1) : Pt{-1, -1};

                int old_ins = (hasNextI ? manhattan(E, F) : 0);
                int new_ins = manhattan(E, B) + (hasNextI ? manhattan(C, F) : 0);

                int d = (new_rem + new_ins) - (old_rem + old_ins);
                if (d < bestD) {
                    bestD = d;
                    bestG = g;
                }
            }
            if (bestG < 0)
                continue;
            if (!accept_delta(bestD, invT))
                continue;

            acc++;
            acc_glo++;

            vector<Elem> block(seq.begin() + l, seq.begin() + r + 1);
            seq.erase(seq.begin() + l, seq.begin() + r + 1);
            seq.insert(seq.begin() + bestG, block.begin(), block.end());

            curK += bestD;
            rebuild_occ();

            if (curK < bestK) {
                bestK = curK;
                best_coords.clear();
                for (auto& e : seq)
                    best_coords.push_back(e.p);
            }

        } else if (pick < 620) {
            // relocate + optional reverse (multi-try)
            try_relocrev++;
            int id = rng.next_int(0, P - 1);
            int i = occ[id][0];
            int j = occ[id][1];
            int l = min(i, j), r = max(i, j);
            int len = r - l + 1;
            if (len == n)
                continue;

            const int TR = 7;
            int bestD = INT_MAX;
            int bestG = -1;
            bool bestRev = false;

            for (int t = 0; t < TR; t++) {
                int g = rng.next_int(0, n - len);
                if (g == l)
                    continue;

                // gap1 is insertion index in the original seq before erasing (same as other relocate moves)
                int gap1 = (g <= l ? g : g + len);

                Pt A = pt_at(l - 1);
                Pt B = pt_at(l);
                Pt C = pt_at(r);
                bool hasRnext = (r + 1 < n);
                Pt D = hasRnext ? pt_at(r + 1) : Pt{-1, -1};

                int old_rem = manhattan(A, B) + (hasRnext ? manhattan(C, D) : 0);
                int new_rem = (hasRnext ? manhattan(A, D) : 0);

                Pt E = pt_at(gap1 - 1);
                bool hasNextI = (gap1 < n);
                Pt F = hasNextI ? pt_at(gap1) : Pt{-1, -1};

                int old_ins = (hasNextI ? manhattan(E, F) : 0);

                // no-reverse
                {
                    int new_ins = manhattan(E, B) + (hasNextI ? manhattan(C, F) : 0);
                    int d = (new_rem + new_ins) - (old_rem + old_ins);
                    if (d < bestD) {
                        bestD = d;
                        bestG = g;
                        bestRev = false;
                    }
                }
                // reverse (internal cost unchanged; only boundary connections change)
                {
                    int new_ins = manhattan(E, C) + (hasNextI ? manhattan(B, F) : 0);
                    int d = (new_rem + new_ins) - (old_rem + old_ins);
                    if (d < bestD) {
                        bestD = d;
                        bestG = g;
                        bestRev = true;
                    }
                }
            }

            if (bestG < 0)
                continue;
            if (!accept_delta(bestD, invT))
                continue;

            acc++;
            acc_relocrev++;

            vector<Elem> block(seq.begin() + l, seq.begin() + r + 1);
            if (bestRev)
                reverse(block.begin(), block.end());
            seq.erase(seq.begin() + l, seq.begin() + r + 1);
            seq.insert(seq.begin() + bestG, block.begin(), block.end());

            curK += bestD;
            rebuild_occ();

            if (curK < bestK) {
                bestK = curK;
                best_coords.clear();
                for (auto& e : seq)
                    best_coords.push_back(e.p);
            }

        } else if (pick < 850) {
            // endpoints move
            try_ep++;

            int id = rng.next_int(0, P - 1);
            int i0 = occ[id][0];
            int i1 = occ[id][1];
            if (i0 == i1)
                continue;

            int p = min(i0, i1), q = max(i0, i1);
            // nodeP is at p, nodeQ at q in original
            const int len2 = n - 2;

            auto get_pt = [&](int idx) -> Pt {
                if (idx < 0)
                    return START;
                return seq[idx].p;
            };

            // delta_remove
            int oldRem = 0, newRem = 0;
            if (q == p + 1) {
                Pt A = (p == 0 ? START : get_pt(p - 1));
                Pt Pp = get_pt(p);
                Pt Pq = get_pt(q);
                bool hasD = (q + 1 < n);
                Pt D = hasD ? get_pt(q + 1) : Pt{-1, -1};

                oldRem += manhattan(A, Pp);
                oldRem += manhattan(Pp, Pq);
                if (hasD)
                    oldRem += manhattan(Pq, D);
                if (hasD)
                    newRem += manhattan(A, D);
            } else {
                Pt A = (p == 0 ? START : get_pt(p - 1));
                Pt Pp = get_pt(p);
                Pt B = get_pt(p + 1);
                Pt C = get_pt(q - 1);
                Pt Pq = get_pt(q);
                bool hasD = (q + 1 < n);
                Pt D = hasD ? get_pt(q + 1) : Pt{-1, -1};

                oldRem += manhattan(A, Pp);
                oldRem += manhattan(Pp, B);
                oldRem += manhattan(C, Pq);
                if (hasD)
                    oldRem += manhattan(Pq, D);

                newRem += manhattan(A, B);
                if (hasD)
                    newRem += manhattan(C, D);
            }
            int delta_remove = newRem - oldRem;

            auto map_red_to_org = [&](int idx) -> int {
                int o = idx;
                if (o >= p)
                    o++;
                if (o >= q)
                    o++;
                return o;
            };
            auto get_red_pt = [&](int idx) -> Pt {
                return seq[map_red_to_org(idx)].p;
            };

            auto adj_gap = [&](int g) -> int {
                int res = g;
                if (g > p)
                    res--;
                if (g > q)
                    res--;
                return res;
            };

            struct Cand {
                int g1, g2;
                bool openIsP;
                int delta;
                bool valid;
            };

            Cand best{0, 0, true, INT_MAX, false};

            // Capture endpoints elements (including coordinates!)
            Elem elemP = seq[p];
            Elem elemQ = seq[q];

            auto eval_cand = [&](int g1, int g2, bool openIsP) -> Cand {
                if (!(0 <= g1 && g1 <= g2 && g2 <= len2))
                    return {0, 0, true, 0, false};

                Pt openPt = openIsP ? elemP.p : elemQ.p;
                Pt closePt = openIsP ? elemQ.p : elemP.p;

                Pt E = (g1 == 0 ? START : get_red_pt(g1 - 1));
                bool hasH = (g2 < len2);
                Pt H = hasH ? get_red_pt(g2) : Pt{-1, -1};

                int oldIns = 0, newIns = 0;
                if (g1 == g2) {
                    if (hasH)
                        oldIns += manhattan(E, H);
                    newIns += manhattan(E, openPt);
                    newIns += manhattan(openPt, closePt);
                    if (hasH)
                        newIns += manhattan(closePt, H);
                } else {
                    Pt F = get_red_pt(g1);
                    Pt G = get_red_pt(g2 - 1);
                    oldIns += manhattan(E, F);
                    if (hasH)
                        oldIns += manhattan(G, H);

                    newIns += manhattan(E, openPt);
                    newIns += manhattan(openPt, F);
                    newIns += manhattan(G, closePt);
                    if (hasH)
                        newIns += manhattan(closePt, H);
                }

                int delta = delta_remove + (newIns - oldIns);
                return {g1, g2, openIsP, delta, true};
            };

            const int TR = 4;
            for (int rep = 0; rep < TR; rep++) {
                int g = (len2 == 0 ? 0 : rng.next_int(0, len2));
                bool openIsP = ((rng.next_u32() & 1u) == 0u);
                Cand c = eval_cand(g, g, openIsP);
                if (c.valid && c.delta < best.delta)
                    best = c;

                if (len2 > 0) {
                    int y = rng.next_int(0, P - 2);
                    if (y >= id)
                        y++;
                    int y0 = occ[y][0];
                    int y1 = occ[y][1];
                    int yl = min(y0, y1);
                    int yr = max(y0, y1);
                    int g1 = adj_gap(yl);
                    int g2 = adj_gap(yr + 1);
                    bool openIsP2 = ((rng.next_u32() & 1u) == 0u);
                    Cand c2 = eval_cand(g1, g2, openIsP2);
                    if (c2.valid && c2.delta < best.delta)
                        best = c2;
                }
            }

            if (!best.valid)
                continue;
            int delta = best.delta;
            if (!accept_delta(delta, invT))
                continue;

            acc++;
            acc_ep++;

            // apply: remove endpoints q then p
            Elem open = best.openIsP ? elemP : elemQ;
            Elem close = best.openIsP ? elemQ : elemP;

            seq.erase(seq.begin() + q);
            seq.erase(seq.begin() + p);

            seq.insert(seq.begin() + best.g1, open);
            seq.insert(seq.begin() + best.g2 + 1, close);

            curK += delta;
            rebuild_occ();

            if (curK < bestK) {
                bestK = curK;
                best_coords.clear();
                for (auto& e : seq)
                    best_coords.push_back(e.p);
            }

        } else if (pick < 875) {
            // reverse a whole matched block of an id
            try_rev++;
            int id = rng.next_int(0, P - 1);
            int i = occ[id][0];
            int j = occ[id][1];
            int l = min(i, j), r = max(i, j);
            int len = r - l + 1;
            if (len <= 1)
                continue;

            Pt prev = pt_at(l - 1);
            Pt first = pt_at(l);
            Pt last = pt_at(r);
            bool hasNext = (r + 1 < n);
            Pt nxt = hasNext ? pt_at(r + 1) : Pt{-1, -1};

            int old = manhattan(prev, first) + (hasNext ? manhattan(last, nxt) : 0);
            int neu = manhattan(prev, last) + (hasNext ? manhattan(first, nxt) : 0);
            int delta = neu - old;

            if (!accept_delta(delta, invT))
                continue;

            acc++;
            acc_rev++;

            reverse(seq.begin() + l, seq.begin() + r + 1);
            curK += delta;
            rebuild_occ();

            if (curK < bestK) {
                bestK = curK;
                best_coords.clear();
                for (auto& e : seq)
                    best_coords.push_back(e.p);
            }

        } else if (pick < 960) {
            // flip coords inside an id
            try_flip++;
            int id = rng.next_int(0, P - 1);
            int i = occ[id][0];
            int j = occ[id][1];
            if (i == j)
                continue;

            Pt pi = seq[i].p;
            Pt pj = seq[j].p;

            int edges[4] = {i, i + 1, j, j + 1};
            sort(edges, edges + 4);
            int uniq[4], m = 0;
            for (int k = 0; k < 4; k++) {
                int b = edges[k];
                if (b < 0 || b > n - 1)
                    continue;
                if (m == 0 || uniq[m - 1] != b)
                    uniq[m++] = b;
            }

            auto coord_after = [&](int idx) -> Pt {
                if (idx == i)
                    return pj;
                if (idx == j)
                    return pi;
                return seq[idx].p;
            };

            int oldSum = 0, newSum = 0;
            for (int t = 0; t < m; t++) {
                int b = uniq[t];
                Pt left = (b == 0 ? START : seq[b - 1].p);
                Pt right = seq[b].p;
                oldSum += manhattan(left, right);

                Pt left2 = (b == 0 ? START : coord_after(b - 1));
                Pt right2 = coord_after(b);
                newSum += manhattan(left2, right2);
            }

            int delta = newSum - oldSum;
            if (!accept_delta(delta, invT))
                continue;

            acc++;
            acc_flip++;

            swap(seq[i].p, seq[j].p);
            curK += delta;

            if (curK < bestK) {
                bestK = curK;
                best_coords.clear();
                for (auto& e : seq)
                    best_coords.push_back(e.p);
            }

        } else if (pick < 985) {
            // swap_blocks + optional reverse for each block
            try_swaprev++;

            bool found = false;
            int l1 = 0, r1 = 0, l2 = 0, r2 = 0;

            for (int rep = 0; rep < 12; rep++) {
                int id1 = rng.next_int(0, P - 1);
                int id2 = rng.next_int(0, P - 1);
                if (id1 == id2)
                    continue;

                int i1 = occ[id1][0];
                int j1 = occ[id1][1];
                int i2 = occ[id2][0];
                int j2 = occ[id2][1];

                int L1 = min(i1, j1), R1 = max(i1, j1);
                int L2 = min(i2, j2), R2 = max(i2, j2);

                if (max(L1, L2) <= min(R1, R2))
                    continue;  // overlap
                if (L1 < L2) {
                    l1 = L1;
                    r1 = R1;
                    l2 = L2;
                    r2 = R2;
                } else {
                    l1 = L2;
                    r1 = R2;
                    l2 = L1;
                    r2 = R1;
                }
                if (!(r1 < l2))
                    continue;
                found = true;
                break;
            }
            if (!found)
                continue;

            int midLen = l2 - (r1 + 1);

            Pt Aend = pt_at(l1 - 1);
            Pt Bstart0 = pt_at(l1);
            Pt Bend0 = pt_at(r1);
            Pt Dstart0 = pt_at(l2);
            Pt Dend0 = pt_at(r2);

            bool Cnon = (midLen > 0);
            Pt Cstart = Cnon ? pt_at(r1 + 1) : Pt{-1, -1};
            Pt Cend = Cnon ? pt_at(l2 - 1) : Pt{-1, -1};

            bool hasE = (r2 + 1 < n);
            Pt Estart = hasE ? pt_at(r2 + 1) : Pt{-1, -1};

            int oldSum = 0;
            oldSum += manhattan(Aend, Bstart0);
            if (Cnon) {
                oldSum += manhattan(Bend0, Cstart);
                oldSum += manhattan(Cend, Dstart0);
            } else {
                oldSum += manhattan(Bend0, Dstart0);
            }
            if (hasE)
                oldSum += manhattan(Dend0, Estart);

            int bestD = INT_MAX;
            bool bestRev1 = false, bestRev2 = false;

            for (int mask = 0; mask < 4; mask++) {
                bool rev1 = (mask & 1) != 0;
                bool rev2 = (mask & 2) != 0;

                Pt Bstart = rev1 ? Bend0 : Bstart0;
                Pt Bend = rev1 ? Bstart0 : Bend0;

                Pt Dstart = rev2 ? Dend0 : Dstart0;
                Pt Dend = rev2 ? Dstart0 : Dend0;

                int newSum = 0;
                newSum += manhattan(Aend, Dstart);
                if (Cnon) {
                    newSum += manhattan(Dend, Cstart);
                    newSum += manhattan(Cend, Bstart);
                } else {
                    newSum += manhattan(Dend, Bstart);
                }
                if (hasE)
                    newSum += manhattan(Bend, Estart);

                int delta = newSum - oldSum;
                if (delta < bestD) {
                    bestD = delta;
                    bestRev1 = rev1;
                    bestRev2 = rev2;
                }
            }

            if (!accept_delta(bestD, invT))
                continue;

            acc++;
            acc_swaprev++;

            // apply swap blocks with reversals
            vector<Elem> block1(seq.begin() + l1, seq.begin() + r1 + 1);
            vector<Elem> block2(seq.begin() + l2, seq.begin() + r2 + 1);
            if (bestRev1)
                reverse(block1.begin(), block1.end());
            if (bestRev2)
                reverse(block2.begin(), block2.end());

            vector<Elem> newseq;
            newseq.reserve(n);
            newseq.insert(newseq.end(), seq.begin(), seq.begin() + l1);           // A
            newseq.insert(newseq.end(), block2.begin(), block2.end());            // D (swapped in)
            newseq.insert(newseq.end(), seq.begin() + r1 + 1, seq.begin() + l2);  // C
            newseq.insert(newseq.end(), block1.begin(), block1.end());            // B
            newseq.insert(newseq.end(), seq.begin() + r2 + 1, seq.end());         // E

            seq.swap(newseq);

            curK += bestD;
            rebuild_occ();

            if (curK < bestK) {
                bestK = curK;
                best_coords.clear();
                for (auto& e : seq)
                    best_coords.push_back(e.p);
            }

        } else {
            // swap_blocks
            try_swap++;

            bool found = false;
            int l1 = 0, r1 = 0, l2 = 0, r2 = 0;

            for (int rep = 0; rep < 10; rep++) {
                int id1 = rng.next_int(0, P - 1);
                int id2 = rng.next_int(0, P - 1);
                if (id1 == id2)
                    continue;

                int i1 = occ[id1][0];
                int j1 = occ[id1][1];
                int i2 = occ[id2][0];
                int j2 = occ[id2][1];

                int L1 = min(i1, j1), R1 = max(i1, j1);
                int L2 = min(i2, j2), R2 = max(i2, j2);

                if (max(L1, L2) <= min(R1, R2))
                    continue;  // overlap
                if (L1 < L2) {
                    l1 = L1;
                    r1 = R1;
                    l2 = L2;
                    r2 = R2;
                } else {
                    l1 = L2;
                    r1 = R2;
                    l2 = L1;
                    r2 = R1;
                }
                if (!(r1 < l2))
                    continue;
                found = true;
                break;
            }
            if (!found)
                continue;

            int midLen = l2 - (r1 + 1);

            Pt Aend = pt_at(l1 - 1);
            Pt Bstart = pt_at(l1);
            Pt Bend = pt_at(r1);
            Pt Dstart = pt_at(l2);
            Pt Dend = pt_at(r2);

            bool Cnon = (midLen > 0);
            Pt Cstart = Cnon ? pt_at(r1 + 1) : Pt{-1, -1};
            Pt Cend = Cnon ? pt_at(l2 - 1) : Pt{-1, -1};

            bool hasE = (r2 + 1 < n);
            Pt Estart = hasE ? pt_at(r2 + 1) : Pt{-1, -1};

            int oldSum = 0, newSum = 0;
            oldSum += manhattan(Aend, Bstart);
            if (Cnon) {
                oldSum += manhattan(Bend, Cstart);
                oldSum += manhattan(Cend, Dstart);
            } else {
                oldSum += manhattan(Bend, Dstart);
            }
            if (hasE)
                oldSum += manhattan(Dend, Estart);

            newSum += manhattan(Aend, Dstart);
            if (Cnon) {
                newSum += manhattan(Dend, Cstart);
                newSum += manhattan(Cend, Bstart);
            } else {
                newSum += manhattan(Dend, Bstart);
            }
            if (hasE)
                newSum += manhattan(Bend, Estart);

            int delta = newSum - oldSum;
            if (!accept_delta(delta, invT))
                continue;

            acc++;
            acc_swap++;

            // apply swap blocks
            vector<Elem> newseq;
            newseq.reserve(n);
            // A
            newseq.insert(newseq.end(), seq.begin(), seq.begin() + l1);
            // block2
            newseq.insert(newseq.end(), seq.begin() + l2, seq.begin() + r2 + 1);
            // mid
            newseq.insert(newseq.end(), seq.begin() + r1 + 1, seq.begin() + l2);
            // block1
            newseq.insert(newseq.end(), seq.begin() + l1, seq.begin() + r1 + 1);
            // E
            newseq.insert(newseq.end(), seq.begin() + r2 + 1, seq.end());

            seq.swap(newseq);

            curK += delta;
            rebuild_occ();

            if (curK < bestK) {
                bestK = curK;
                best_coords.clear();
                for (auto& e : seq)
                    best_coords.push_back(e.p);
            }
        }

        if ((iters & 16383LL) == 0)
            resync_curK();
    }

    cerr << "[INFO] iters=" << iters << " acc=" << acc << " acc_rate="
         << (iters ? (double)acc / (double)iters : 0.0) << "\n";
    cerr << "[INFO] relocate      try=" << try_reloc << " acc=" << acc_reloc << "\n";
    cerr << "[INFO] relocate_rev  try=" << try_relocrev << " acc=" << acc_relocrev << "\n";
    cerr << "[INFO] relocate_loc  try=" << try_loc << " acc=" << acc_loc << "\n";
    cerr << "[INFO] relocate_glo  try=" << try_glo << " acc=" << acc_glo << "\n";
    cerr << "[INFO] endpoints     try=" << try_ep << " acc=" << acc_ep << "\n";
    cerr << "[INFO] reverse_block try=" << try_rev << " acc=" << acc_rev << "\n";
    cerr << "[INFO] flip          try=" << try_flip << " acc=" << acc_flip << "\n";
    cerr << "[INFO] swap_blocks   try=" << try_swap << " acc=" << acc_swap << "\n";
    cerr << "[INFO] swap_rev      try=" << try_swaprev << " acc=" << acc_swaprev << "\n";

    cerr << "[DATA] score = " << bestK << "\n";

    // output best (moves + Z only)
    string out;
    out.reserve(50000);
    Pt cur = START;
    for (const Pt& t : best_coords) {
        while (cur.r < t.r) {
            out.push_back('D');
            out.push_back('\n');
            cur.r++;
        }
        while (cur.r > t.r) {
            out.push_back('U');
            out.push_back('\n');
            cur.r--;
        }
        while (cur.c < t.c) {
            out.push_back('R');
            out.push_back('\n');
            cur.c++;
        }
        while (cur.c > t.c) {
            out.push_back('L');
            out.push_back('\n');
            cur.c--;
        }
        out.push_back('Z');
        out.push_back('\n');
    }
    cout << out;
    return 0;
}
