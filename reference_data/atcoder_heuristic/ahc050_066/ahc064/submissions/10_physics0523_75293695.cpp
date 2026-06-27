#pragma GCC target("avx2")
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")

#include <bits/stdc++.h>
using namespace std;

static constexpr int R = 10;
static constexpr int DCAP = 15;
static constexpr int SCAP = 20;

struct Op {
    int type, i, j, k;
};

struct Turn {
    int n = 0;
    Op op[R];
};

struct State {
    unsigned char d[R][DCAP]{};
    unsigned char s[R][SCAP]{};
    unsigned char dl[R]{};
    unsigned char sl[R]{};
};

struct Analysis {
    int prog[R]{};
    bool clean[R]{};
    int minP = 10, maxP = 0;
    int sumP = 0;
    int dirty = 0;
    int cleanCnt = 0;
    int completeCnt = 0;

    // current car for row r = 10*r + prog[r]
    // locType: 0=departure, 1=siding, -1=finished/unknown
    int locType[R]{};
    int locA[R]{};
    int locB[R]{};

    int topCurrentRowOfS[R]{};  // -1 if siding top is not a current car
    bool rowWaitingTop[R]{};    // current car of row r is at the top of some siding
    int firstCurPosS[R]{};      // first current car in siding j
    int firstCurRowS[R]{};
};

struct Node {
    State st;
    long long score = 0;
    int parent = -1;
    Turn turn;
    uint64_t h = 0;
};

struct Cand {
    int type, i, j, k;
    int base;
    int help; // row mainly helped by this operation, -1 if none
    int kind; // 0=final load, 1=expose, 2=clear, 3=unblock, 4=small cleanup
};

static inline int row_of(int x) { return x / 10; }
static inline int pos_of(int x) { return x % 10; }

int pref_len(const State& st, int i) {
    int p = 0;
    int n = st.dl[i];
    while (p < n && p < 10 && (int)st.d[i][p] == 10 * i + p) ++p;
    return p;
}

bool is_complete(const State& st) {
    for (int i = 0; i < R; ++i) {
        if (st.dl[i] != 10) return false;
        for (int c = 0; c < 10; ++c) {
            if ((int)st.d[i][c] != 10 * i + c) return false;
        }
    }
    return true;
}

uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

uint64_t hash_state(const State& st) {
    uint64_t h = 0x24c0ffee12345678ULL;
    for (int i = 0; i < R; ++i) {
        h ^= splitmix64((uint64_t)st.dl[i] + 911ULL * (i + 1));
        for (int p = 0; p < st.dl[i]; ++p) {
            h ^= splitmix64((uint64_t)st.d[i][p] + 131ULL * p + 10007ULL * i);
        }
    }
    for (int j = 0; j < R; ++j) {
        h ^= splitmix64((uint64_t)st.sl[j] + 1709ULL * (j + 1));
        for (int p = 0; p < st.sl[j]; ++p) {
            h ^= splitmix64((uint64_t)st.s[j][p] + 197ULL * p + 20011ULL * j);
        }
    }
    return h;
}

Analysis analyze_state(const State& st) {
    Analysis a;
    for (int i = 0; i < R; ++i) {
        a.locType[i] = -1;
        a.locA[i] = a.locB[i] = -1;
        a.topCurrentRowOfS[i] = -1;
        a.firstCurPosS[i] = -1;
        a.firstCurRowS[i] = -1;
        a.rowWaitingTop[i] = false;
    }

    for (int i = 0; i < R; ++i) {
        a.prog[i] = pref_len(st, i);
        a.clean[i] = ((int)st.dl[i] == a.prog[i]);
        a.minP = min(a.minP, a.prog[i]);
        a.maxP = max(a.maxP, a.prog[i]);
        a.sumP += a.prog[i];
        a.dirty += max(0, (int)st.dl[i] - a.prog[i]);
        if (a.clean[i]) ++a.cleanCnt;
        if (a.clean[i] && a.prog[i] == 10) ++a.completeCnt;
    }

    auto is_current = [&](int x) -> bool {
        int r = row_of(x), p = pos_of(x);
        return a.prog[r] < 10 && p == a.prog[r];
    };

    for (int r = 0; r < R; ++r) {
        if (a.prog[r] >= 10) continue;
        int target = 10 * r + a.prog[r];

        bool found = false;
        for (int i = 0; i < R && !found; ++i) {
            for (int p = 0; p < st.dl[i]; ++p) {
                if ((int)st.d[i][p] == target) {
                    a.locType[r] = 0;
                    a.locA[r] = i;
                    a.locB[r] = p;
                    found = true;
                    break;
                }
            }
        }

        for (int j = 0; j < R && !found; ++j) {
            for (int p = 0; p < st.sl[j]; ++p) {
                if ((int)st.s[j][p] == target) {
                    a.locType[r] = 1;
                    a.locA[r] = j;
                    a.locB[r] = p;
                    if (p == 0) a.rowWaitingTop[r] = true;
                    found = true;
                    break;
                }
            }
        }
    }

    for (int j = 0; j < R; ++j) {
        if (st.sl[j] > 0) {
            int x = st.s[j][0];
            if (is_current(x)) {
                a.topCurrentRowOfS[j] = row_of(x);
                a.rowWaitingTop[row_of(x)] = true;
            }
        }

        for (int p = 0; p < st.sl[j]; ++p) {
            int x = st.s[j][p];
            if (is_current(x)) {
                a.firstCurPosS[j] = p;
                a.firstCurRowS[j] = row_of(x);
                break;
            }
        }
    }

    return a;
}

static inline bool is_current_id(int x, const Analysis& a) {
    int r = row_of(x), p = pos_of(x);
    return a.prog[r] < 10 && p == a.prog[r];
}

static inline bool is_near_id(int x, const Analysis& a, int delta = 2) {
    int r = row_of(x), p = pos_of(x);
    return a.prog[r] < 10 && p >= a.prog[r] && p <= min(9, a.prog[r] + delta);
}


static inline bool good_pair_id(int x, int y) {
    return row_of(x) == row_of(y) && pos_of(y) == pos_of(x) + 1;
}

struct AdjInfo {
    int total = 0;
    int nearTotal = 0;
    int currentPair = 0;
    int sidingTopRun = 0;
    int rowAdj[R]{};
    int rowCur[R]{};
};

AdjInfo calc_adj_info(const State& st, const Analysis& a) {
    AdjInfo res;

    auto add_pair = [&](int x, int y) {
        if (!good_pair_id(x, y)) return;

        int r = row_of(x);
        int p = pos_of(x);

        // Pairs already deep inside the fixed prefix are already rewarded by prog.
        if (p + 1 < a.prog[r]) return;

        ++res.total;
        ++res.rowAdj[r];

        if (p >= a.prog[r]) ++res.nearTotal;

        if (p == a.prog[r]) {
            ++res.currentPair;
            ++res.rowCur[r];
        }
    };

    for (int i = 0; i < R; ++i) {
        for (int p = 0; p + 1 < st.dl[i]; ++p) {
            add_pair(st.d[i][p], st.d[i][p + 1]);
        }
    }

    for (int j = 0; j < R; ++j) {
        for (int p = 0; p + 1 < st.sl[j]; ++p) {
            add_pair(st.s[j][p], st.s[j][p + 1]);
        }

        if (st.sl[j] > 0) {
            int x = st.s[j][0];
            int r = row_of(x);
            if (a.clean[r] && a.prog[r] < 10 && x == 10 * r + a.prog[r]) {
                int run = 0;
                while (run < st.sl[j] &&
                       a.prog[r] + run < 10 &&
                       (int)st.s[j][run] == 10 * r + a.prog[r] + run) {
                    ++run;
                }
                if (run >= 2) res.sidingTopRun += run;
            }
        }
    }

    return res;
}

int moved_block_adj_bonus_d2s(const State& st, const Analysis& a, int i, int j, int k) {
    int bonus = 0;
    int start = (int)st.dl[i] - k;

    for (int t = 0; t + 1 < k; ++t) {
        int x = st.d[i][start + t];
        int y = st.d[i][start + t + 1];
        if (good_pair_id(x, y)) {
            int r = row_of(x);
            int p = pos_of(x);

            if (p + 1 >= a.prog[r]) {
                bonus += 8500;
                if (p == a.prog[r]) bonus += 22000;
                if (a.prog[r] == a.minP) bonus += 7000;
            }
        }
    }

    // New siding order is moved_block followed by the old siding top.
    if (st.sl[j] > 0 && k > 0) {
        int x = st.d[i][start + k - 1];
        int y = st.s[j][0];

        if (good_pair_id(x, y)) {
            int r = row_of(x);
            int p = pos_of(x);

            if (p + 1 >= a.prog[r]) {
                bonus += 18000;
                if (p == a.prog[r]) bonus += 36000;
                if (a.clean[r]) bonus += 12000;
            }
        }
    }

    return bonus;
}

int moved_block_adj_bonus_s2d(const State& st, const Analysis& a, int i, int j, int k) {
    int bonus = 0;

    for (int t = 0; t + 1 < k; ++t) {
        int x = st.s[j][t];
        int y = st.s[j][t + 1];

        if (good_pair_id(x, y)) {
            int r = row_of(x);
            int p = pos_of(x);

            if (p + 1 >= a.prog[r]) {
                bonus += 9000;
                if (p == a.prog[r]) bonus += 26000;
                if (a.clean[r]) bonus += 15000;
            }
        }
    }

    // New departure order is old_d followed by moved_block.
    if (st.dl[i] > 0 && k > 0) {
        int x = st.d[i][st.dl[i] - 1];
        int y = st.s[j][0];

        if (good_pair_id(x, y)) {
            int r = row_of(x);
            int p = pos_of(x);

            if (p + 1 >= a.prog[r]) {
                bonus += 24000;
                if (i == r && p + 1 == a.prog[r]) bonus += 50000;
                if (a.clean[r]) bonus += 22000;
            }
        }
    }

    return bonus;
}

int lag_bonus(const Analysis& a, int r) {
    if (r < 0 || r >= R || a.prog[r] >= 10) return 0;

    int lag = a.maxP - a.prog[r];
    int rem = 10 - a.prog[r];

    int b = 1200 * rem + 2600 * lag;
    if (a.prog[r] == a.minP) b += 14000;
    else if (a.prog[r] == a.minP + 1) b += 6500;

    return b;
}

long long eval_state(const State& st, int depth) {
    Analysis a = analyze_state(st);
    AdjInfo adj = calc_adj_info(st, a);

    int sortedP[R];
    for (int i = 0; i < R; ++i) sortedP[i] = a.prog[i];
    sort(sortedP, sortedP + R);

    long long balance = 0;
    for (int i = 0; i < R; ++i) {
        // Smaller progress receives a larger coefficient.
        balance += 1LL * (R - i) * sortedP[i];
    }

    int loadable = 0;
    int needSum = 0;
    int needMax = 0;
    int buriedDepth = 0;

    for (int r = 0; r < R; ++r) {
        if (a.prog[r] >= 10) continue;

        int cost = 0;
        if (!a.clean[r]) cost += 3;

        if (a.locType[r] == 1) {
            int dep = a.locB[r];
            if (dep == 0 && a.clean[r]) {
                ++loadable;
            } else {
                cost += 2 + dep / 4;
                buriedDepth += dep;
            }
        } else if (a.locType[r] == 0) {
            cost += 2;
            int src = a.locA[r], pos = a.locB[r];
            if (pos > a.prog[src]) cost += 1;
        } else {
            cost += 12;
        }

        int weight = 1 + (a.maxP - a.prog[r]) + (10 - a.prog[r]) / 2;
        needSum += cost * weight;
        needMax = max(needMax, cost);
    }

    long long sc = 0;
    sc += 1000000LL * a.sumP;
    sc += 52000LL * balance;
    sc += 260000LL * loadable;
    sc += 65000LL * a.cleanCnt;
    sc += 360000LL * a.completeCnt;
    sc -= 30000LL * a.dirty;
    sc -= 16000LL * needSum;
    sc -= 65000LL * needMax;
    sc -= 2500LL * buriedDepth;
    sc += 28000LL * adj.total;
    sc += 52000LL * adj.nearTotal;
    sc += 115000LL * adj.currentPair;
    sc += 145000LL * adj.sidingTopRun;

    for (int r = 0; r < R; ++r) {
        if (a.prog[r] < 10) {
            sc += 6000LL * adj.rowAdj[r] * (10 - a.prog[r]);
            sc += 26000LL * adj.rowCur[r] * (1 + a.maxP - a.prog[r]);
        }
    }

    sc -= 1000LL * depth;

    if (is_complete(st)) sc += 1000000000000000LL;
    return sc;
}

void apply_turn(State& st, const Turn& t) {
    for (int z = 0; z < t.n; ++z) {
        const Op& op = t.op[z];

        if (op.type == 0) {
            int i = op.i, j = op.j, k = op.k;
            unsigned char buf[SCAP];

            int start = (int)st.dl[i] - k;
            for (int x = 0; x < k; ++x) buf[x] = st.d[i][start + x];

            st.dl[i] -= k;

            for (int x = (int)st.sl[j] - 1; x >= 0; --x) {
                st.s[j][x + k] = st.s[j][x];
            }

            for (int x = 0; x < k; ++x) st.s[j][x] = buf[x];
            st.sl[j] += k;
        } else {
            int i = op.i, j = op.j, k = op.k;
            unsigned char buf[SCAP];

            for (int x = 0; x < k; ++x) buf[x] = st.s[j][x];

            for (int x = k; x < st.sl[j]; ++x) {
                st.s[j][x - k] = st.s[j][x];
            }

            st.sl[j] -= k;

            int start = st.dl[i];
            for (int x = 0; x < k; ++x) st.d[i][start + x] = buf[x];
            st.dl[i] += k;
        }
    }
}

bool valid_turn_basic(const State& st, const Turn& t) {
    if (t.n <= 0 || t.n > R) return false;

    bool usedD[R] = {};
    bool usedS[R] = {};

    for (int z = 0; z < t.n; ++z) {
        const Op& op = t.op[z];

        if (op.type < 0 || op.type > 1) return false;
        if (op.i < 0 || op.i >= R || op.j < 0 || op.j >= R || op.k <= 0) return false;
        if (usedD[op.i] || usedS[op.j]) return false;

        usedD[op.i] = true;
        usedS[op.j] = true;

        if (op.type == 0) {
            if (st.dl[op.i] < op.k) return false;
            if (st.sl[op.j] + op.k > SCAP) return false;
        } else {
            if (st.sl[op.j] < op.k) return false;
            if (st.dl[op.i] + op.k > DCAP) return false;
        }
    }

    for (int a = 0; a < t.n; ++a) {
        for (int b = a + 1; b < t.n; ++b) {
            int i1 = t.op[a].i, j1 = t.op[a].j;
            int i2 = t.op[b].i, j2 = t.op[b].j;

            if (i1 < i2 && !(j1 < j2)) return false;
            if (i2 < i1 && !(j2 < j1)) return false;
        }
    }

    return true;
}

vector<Cand> gen_candidates(const State& st, const Analysis& a) {
    vector<Cand> cand;
    cand.reserve(1400);

    auto add = [&](int type, int i, int j, int k, int base, int help, int kind) {
        if (k <= 0 || i < 0 || i >= R || j < 0 || j >= R) return;

        if (type == 0) {
            if (st.dl[i] < k || st.sl[j] + k > SCAP) return;
        } else {
            if (st.sl[j] < k || st.dl[i] + k > DCAP) return;
        }

        if (base > 0) cand.push_back({type, i, j, k, base, help, kind});
    };

    // 1. Deterministic final loads: if a siding top is exactly the next car,
    // load the whole consecutive run.
    for (int j = 0; j < R; ++j) {
        if (st.sl[j] == 0) continue;

        int x = st.s[j][0];
        int r = row_of(x), p = pos_of(x);

        if (a.prog[r] < 10 && p == a.prog[r] && a.clean[r] && st.dl[r] < DCAP) {
            int k = 0;

            while (k < st.sl[j] &&
                   a.prog[r] + k < 10 &&
                   (int)st.s[j][k] == 10 * r + a.prog[r] + k &&
                   (int)st.dl[r] + k + 1 <= DCAP) {
                ++k;
            }

            if (k > 0) {
                int base = 170000 + 15500 * k + 3 * lag_bonus(a, r);
                if (k >= 2) base += 90000 + 35000 * (k - 1);
                if (k >= 3) base += 70000;
                if (a.prog[r] == a.minP) base += 70000;
                add(1, r, j, k, base, r, 0);
            }
        }
    }

    // 2. Departure -> siding. Preserve the already correct prefix,
    // and expose current/near-current cars.
    for (int i = 0; i < R; ++i) {
        int dirty = (int)st.dl[i] - a.prog[i];
        if (dirty <= 0) continue;

        bool useK[DCAP + 1] = {};

        auto addK = [&](int k) {
            if (1 <= k && k <= dirty && k <= DCAP) useK[k] = true;
        };

        addK(dirty); // clear this row
        addK(1);     // small peel

        if (dirty <= 6) {
            for (int k = 2; k <= dirty; ++k) addK(k);
        }

        for (int q = a.prog[i]; q < st.dl[i]; ++q) {
            int x = st.d[i][q];
            if (is_current_id(x, a) || is_near_id(x, a, 2)) {
                addK((int)st.dl[i] - q);
            }
        }

        for (int k = 1; k <= dirty; ++k) {
            if (!useK[k]) continue;

            int start = (int)st.dl[i] - k;
            int top = st.d[i][start];

            int tr = row_of(top), tp = pos_of(top);
            int help = -1;
            int kind = 4;
            int base = 0;

            if (is_current_id(top, a)) {
                help = tr;
                kind = 1;
                base = 52000 + 2 * lag_bonus(a, tr);

                if (a.clean[tr]) base += 28000;
                else base += 7000;

                // Clear own dirty part and expose the next car.
                if (i == tr && start == a.prog[i]) base += 36000;

                base += 450 * k;
            } else if (is_near_id(top, a, 2)) {
                help = tr;
                kind = 1;

                int dist = tp - a.prog[tr];
                base = 15000 + lag_bonus(a, tr) / 2 - 3600 * dist + 250 * k;
            } else if (k == dirty) {
                help = i;
                kind = 2;
                base = 14000 + lag_bonus(a, i);

                if (a.rowWaitingTop[i]) base += 62000;
                if (a.locType[i] == 1) base += 22000;

                // Completed row has extra garbage; remove it.
                if (a.prog[i] == 10) base += 36000;

                base += 320 * k;
            } else if (k == 1) {
                kind = 4;
                base = 3000 + 450 * dirty;
            } else {
                kind = 4;
                base = 1200 + 250 * k;
            }

            for (int j = 0; j < R; ++j) {
                if ((int)st.sl[j] + k > SCAP) continue;

                int w = base;
                w += moved_block_adj_bonus_d2s(st, a, i, j, k);

                if (st.sl[j] == 0) {
                    w += 4500;
                } else {
                    int y = st.s[j][0];

                    if (is_current_id(y, a)) {
                        int yr = row_of(y);
                        w -= a.clean[yr] ? 72000 : 30000;
                        if (a.prog[yr] == a.minP) w -= 25000;
                    } else if (is_near_id(y, a, 1)) {
                        w -= 4500;
                    }

                    w += (SCAP - (int)st.sl[j]) * 80;
                }

                if (w > 0) add(0, i, j, k, w, help, kind);
            }
        }
    }

    // 3. Siding -> departure as temporary blocker removal.
    for (int j = 0; j < R; ++j) {
        if (st.sl[j] == 0) continue;

        int top = st.s[j][0];
        bool topCur = is_current_id(top, a);

        // If top is loadable, final-load candidate already covers it.
        // Do not bury it as a blocker.
        if (topCur) {
            int r = row_of(top);
            if (a.clean[r]) continue;
        }

        int d = a.firstCurPosS[j];
        int rhelp = a.firstCurRowS[j];
        bool hasCurrentBehind = (d > 0);

        if (!hasCurrentBehind) {
            // Low-priority housekeeping only.
            // Prefer dirty departure rows as buffers.
            int small = min<int>(2, st.sl[j]);

            for (int i = 0; i < R; ++i) {
                int room = DCAP - (int)st.dl[i];
                if (room <= 0) continue;

                int k = min(small, room);
                int w = 1700 + 180 * k;
                w += moved_block_adj_bonus_s2d(st, a, i, j, k) / 2;

                if (!a.clean[i]) w += 3800;
                else w -= 4500 + lag_bonus(a, i) / 2;

                if (a.rowWaitingTop[i]) w -= 30000;

                if (w > 0) add(1, i, j, k, w, -1, 4);
            }

            continue;
        }

        for (int i = 0; i < R; ++i) {
            int room = DCAP - (int)st.dl[i];
            if (room <= 0) continue;

            bool useK[DCAP + 1] = {};

            auto addK = [&](int k) {
                if (1 <= k && k <= d && k <= room) useK[k] = true;
            };

            addK(min(d, room));
            addK(1);
            addK(min(d, min(room, 3)));
            addK(min(d, min(room, 5)));

            for (int k = 1; k <= min(d, room); ++k) {
                if (!useK[k]) continue;

                int w = 39000 + 2 * lag_bonus(a, rhelp) + 950 * k;
                w += moved_block_adj_bonus_s2d(st, a, i, j, k);

                if (a.clean[rhelp]) w += 21000;
                else w += 5000;

                if (k == d) w += 42000;       // current car becomes top next turn
                else w -= 4200 * (d - k);     // still buried

                if (a.clean[i]) {
                    w -= 16000;

                    if (a.prog[i] < 10) w -= 12000 + lag_bonus(a, i) / 2;
                    else w -= 8000;

                    if (a.rowWaitingTop[i]) w -= 55000;
                    if (i == rhelp) w -= 75000;
                } else {
                    w += 7800;
                }

                w += (room - k) * 250;

                if (w > 0) add(1, i, j, k, w, rhelp, 3);
            }
        }
    }

    return cand;
}

uint32_t op_hash_noise(const Cand& c, int mode, int focus) {
    uint32_t x = 2166136261u;

    auto mix = [&](uint32_t v) {
        x ^= v;
        x *= 16777619u;
    };

    mix((uint32_t)c.type + 3u);
    mix((uint32_t)c.i + 17u);
    mix((uint32_t)c.j + 31u);
    mix((uint32_t)c.k + 47u);
    mix((uint32_t)c.kind + 59u);
    mix((uint32_t)(mode + 11) * 101u);
    mix((uint32_t)(focus + 23) * 1009u);

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return x;
}

long long adjusted_weight(const Cand& c, const Analysis& a, int mode, int focus) {
    long long w = c.base;

    if (focus >= 0) {
        if (c.help == focus) {
            w += 95000 + 3LL * lag_bonus(a, focus);

            if (c.kind == 0) w += 70000;
            if (c.kind == 1 || c.kind == 3) w += 32000;
        } else if (c.help >= 0) {
            w -= 6000;
        }
    }

    if (mode == 1) {            // final-load heavy
        if (c.kind == 0) w += 90000;
        if (c.kind == 2 && c.help >= 0 && a.rowWaitingTop[c.help]) w += 30000;
    } else if (mode == 2) {     // expose departure cars
        if (c.kind == 1) w += 52000;
        if (c.type == 0) w += 7000;
    } else if (mode == 3) {     // unblock sidings
        if (c.kind == 3) w += 56000;
        if (c.type == 1 && c.kind != 0) w += 7000;
    } else if (mode == 4) {     // clear dirty target rows
        if (c.kind == 2) w += 62000;
        if (c.kind == 0) w += 35000;
    } else if (mode == 5) {     // mild deterministic diversification
        int noise = (int)(op_hash_noise(c, mode, focus) % 20001) - 10000;
        w += noise;

        if (c.kind == 1 || c.kind == 3) w += 12000;
    }

    if (c.help >= 0 && a.prog[c.help] == a.minP) w += 16000;

    return w;
}

Turn select_turn_dp(const vector<Cand>& cand, const Analysis& a, int mode, int focus) {
    struct Cell {
        bool has = false;
        long long w = LLONG_MIN / 4;
        Op op{};
    };

    Cell best[R][R];

    for (const Cand& c : cand) {
        long long w = adjusted_weight(c, a, mode, focus);

        if (w <= 1000) continue;

        if (!best[c.i][c.j].has || w > best[c.i][c.j].w) {
            best[c.i][c.j].has = true;
            best[c.i][c.j].w = w;
            best[c.i][c.j].op = {c.type, c.i, c.j, c.k};
        }
    }

    long long dp[R + 1][R + 1];
    unsigned char ch[R + 1][R + 1];

    for (int i = 0; i <= R; ++i) {
        for (int j = 0; j <= R; ++j) {
            dp[i][j] = 0;
            ch[i][j] = 0;
        }
    }

    for (int i = R - 1; i >= 0; --i) {
        for (int j = R - 1; j >= 0; --j) {
            long long val = dp[i + 1][j];
            unsigned char choice = 0;

            if (dp[i][j + 1] > val) {
                val = dp[i][j + 1];
                choice = 1;
            }

            if (best[i][j].has) {
                long long tv = best[i][j].w + dp[i + 1][j + 1];
                if (tv > val) {
                    val = tv;
                    choice = 2;
                }
            }

            dp[i][j] = val;
            ch[i][j] = choice;
        }
    }

    Turn ret;
    int i = 0, j = 0;

    while (i < R && j < R) {
        if (ch[i][j] == 2) {
            ret.op[ret.n++] = best[i][j].op;
            ++i;
            ++j;
        } else if (ch[i][j] == 0) {
            ++i;
        } else {
            ++j;
        }
    }

    return ret;
}

vector<Turn> reconstruct(const vector<vector<Node>>& layers, int depth, int idx) {
    vector<Turn> ans;
    ans.reserve(depth);

    for (int d = depth; d >= 1; --d) {
        ans.push_back(layers[d][idx].turn);
        idx = layers[d][idx].parent;
    }

    reverse(ans.begin(), ans.end());
    return ans;
}

bool can_add_op_to_turn(const Turn& t, const Op& op) {
    if (t.n >= R) return false;

    for (int z = 0; z < t.n; ++z) {
        const Op& e = t.op[z];

        if (e.i == op.i || e.j == op.j) return false;
        if (e.i < op.i && !(e.j < op.j)) return false;
        if (op.i < e.i && !(op.j < e.j)) return false;
    }

    return true;
}

vector<Turn> reschedule_turns(const vector<Turn>& ans) {
    vector<Op> ops;
    ops.reserve(ans.size() * R);

    for (const Turn& t : ans) {
        for (int z = 0; z < t.n; ++z) ops.push_back(t.op[z]);
    }

    vector<Turn> sched;

    int lastD[R], lastS[R];
    fill(lastD, lastD + R, -1);
    fill(lastS, lastS + R, -1);

    for (const Op& op : ops) {
        int ready = max(lastD[op.i], lastS[op.j]) + 1;
        int place = -1;

        for (int t = ready; t < (int)sched.size(); ++t) {
            if (can_add_op_to_turn(sched[t], op)) {
                place = t;
                break;
            }
        }

        if (place == -1) {
            place = (int)sched.size();
            sched.push_back(Turn());
        }

        sched[place].op[sched[place].n++] = op;
        lastD[op.i] = place;
        lastS[op.j] = place;
    }

    vector<Turn> compact;
    compact.reserve(sched.size());

    for (const Turn& t : sched) {
        if (t.n > 0) compact.push_back(t);
    }

    return compact;
}

bool validate_solution(const State& init, const vector<Turn>& ans) {
    if (ans.size() > 4000) return false;

    State st = init;

    for (const Turn& t : ans) {
        if (!valid_turn_basic(st, t)) return false;

        apply_turn(st, t);

        for (int i = 0; i < R; ++i) {
            if (st.dl[i] > DCAP || st.sl[i] > SCAP) return false;
        }
    }

    return is_complete(st);
}

vector<Turn> beam_solve(const State& init, double time_limit_sec) {
    const int MAX_DEPTH = 70;
    const int BEAM_WIDTH = 2500;

    auto start = chrono::steady_clock::now();

    vector<vector<Node>> layers;
    layers.reserve(MAX_DEPTH + 1);

    Node root;
    root.st = init;
    root.score = eval_state(init, 0);
    root.h = hash_state(init);

    layers.push_back(vector<Node>{root});

    vector<Turn> bestComplete;

    for (int depth = 0; depth < MAX_DEPTH; ++depth) {
        double elapsed = chrono::duration<double>(chrono::steady_clock::now() - start).count();
        if (elapsed > time_limit_sec) break;

        const vector<Node>& cur = layers.back();

        vector<Node> nxt;
        nxt.reserve(cur.size() * 9);

        unordered_map<uint64_t, int> seen;
        seen.reserve(cur.size() * 12 + 100);

        for (int pi = 0; pi < (int)cur.size(); ++pi) {
            if ((pi & 31) == 0) {
                double now = chrono::duration<double>(chrono::steady_clock::now() - start).count();
                if (now > time_limit_sec) break;
            }

            const State& st = cur[pi].st;

            if (is_complete(st)) {
                bestComplete = reconstruct(layers, depth, pi);
                return bestComplete;
            }

            Analysis a = analyze_state(st);
            vector<Cand> cand = gen_candidates(st, a);

            if (cand.empty()) continue;

            int rows[R];
            iota(rows, rows + R, 0);

            sort(rows, rows + R, [&](int x, int y) {
                if (a.prog[x] != a.prog[y]) return a.prog[x] < a.prog[y];
                return lag_bonus(a, x) > lag_bonus(a, y);
            });

            vector<pair<int, int>> modes;
            modes.reserve(10);

            modes.push_back({0, -1});
            modes.push_back({1, -1});

            int addedFocus = 0;
            for (int z = 0; z < R && addedFocus < 3; ++z) {
                int r = rows[z];

                if (a.prog[r] < 10) {
                    modes.push_back({0, r});
                    ++addedFocus;
                }
            }

            modes.push_back({2, -1});
            modes.push_back({3, -1});
            modes.push_back({4, -1});
            modes.push_back({5, rows[0]});

            for (auto [mode, focus] : modes) {
                Turn turn = select_turn_dp(cand, a, mode, focus);

                if (turn.n == 0) continue;
                if (!valid_turn_basic(st, turn)) continue;

                State ns = st;
                apply_turn(ns, turn);

                uint64_t h = hash_state(ns);
                long long sc = eval_state(ns, depth + 1);

                auto it = seen.find(h);

                if (it != seen.end()) {
                    int id = it->second;

                    if (sc <= nxt[id].score) continue;

                    nxt[id].st = ns;
                    nxt[id].score = sc;
                    nxt[id].parent = pi;
                    nxt[id].turn = turn;
                    nxt[id].h = h;
                } else {
                    int id = (int)nxt.size();
                    seen[h] = id;

                    Node nd;
                    nd.st = ns;
                    nd.score = sc;
                    nd.parent = pi;
                    nd.turn = turn;
                    nd.h = h;

                    nxt.push_back(nd);
                }
            }
        }

        if (nxt.empty()) break;

        sort(nxt.begin(), nxt.end(), [](const Node& a, const Node& b) {
            if (a.score != b.score) return a.score > b.score;
            return a.h < b.h;
        });

        if ((int)nxt.size() > BEAM_WIDTH) nxt.resize(BEAM_WIDTH);

        layers.push_back(move(nxt));

        int nd = depth + 1;
        for (int i = 0; i < (int)layers.back().size(); ++i) {
            if (is_complete(layers.back()[i].st)) {
                bestComplete = reconstruct(layers, nd, i);
                return bestComplete;
            }
        }
    }

    return bestComplete;
}

// Safe deterministic fallback. It is not short, but prevents WA if beam fails.
vector<Turn> fallback_solve(State init) {
    vector<Turn> ans;
    State st = init;

    Turn first;

    for (int i = 0; i < R; ++i) {
        if (st.dl[i] > 0) {
            first.op[first.n++] = {0, i, i, (int)st.dl[i]};
        }
    }

    if (first.n > 0) {
        apply_turn(st, first);
        ans.push_back(first);
    }

    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < 10; ++c) {
            int target = 10 * r + c;
            int sj = -1, pos = -1;

            for (int j = 0; j < R; ++j) {
                for (int q = 0; q < st.sl[j]; ++q) {
                    if ((int)st.s[j][q] == target) {
                        sj = j;
                        pos = q;
                        break;
                    }
                }

                if (sj != -1) break;
            }

            while (pos > 0) {
                int bestD = -1, roomD = -1;

                for (int i = 0; i < R; ++i) {
                    int room = DCAP - (int)st.dl[i];

                    if (room > roomD) {
                        roomD = room;
                        bestD = i;
                    }
                }

                int bestS = -1, roomS = -1;

                for (int j = 0; j < R; ++j) {
                    if (j == sj) continue;

                    int room = SCAP - (int)st.sl[j];

                    if (room > roomS) {
                        roomS = room;
                        bestS = j;
                    }
                }

                int k = min(pos, min(roomD, roomS));
                if (k <= 0) k = 1;

                Turn t1;
                t1.op[t1.n++] = {1, bestD, sj, k};
                apply_turn(st, t1);
                ans.push_back(t1);

                Turn t2;
                t2.op[t2.n++] = {0, bestD, bestS, k};
                apply_turn(st, t2);
                ans.push_back(t2);

                pos -= k;
            }

            Turn load;
            load.op[load.n++] = {1, r, sj, 1};
            apply_turn(st, load);
            ans.push_back(load);
        }
    }

    return ans;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int Rin;
    if (!(cin >> Rin)) return 0;

    State init;

    for (int i = 0; i < R; ++i) {
        init.dl[i] = 10;
        init.sl[i] = 0;

        for (int c = 0; c < 10; ++c) {
            int x;
            cin >> x;
            init.d[i][c] = (unsigned char)x;
        }
    }

    if (is_complete(init)) {
        cout << 0 << '\n';
        return 0;
    }

    vector<Turn> ans = beam_solve(init, 1.98);

    if (ans.empty() || !validate_solution(init, ans)) {
        ans = fallback_solve(init);
    }

    vector<Turn> packed = reschedule_turns(ans);

    if (validate_solution(init, packed) && packed.size() <= ans.size()) {
        ans = move(packed);
    }

    if (!validate_solution(init, ans)) {
        ans = fallback_solve(init);

        packed = reschedule_turns(ans);
        if (validate_solution(init, packed) && packed.size() <= ans.size()) {
            ans = move(packed);
        }
    }

    cout << ans.size() << '\n';

    for (const Turn& t : ans) {
        cout << t.n << '\n';

        for (int z = 0; z < t.n; ++z) {
            const Op& op = t.op[z];
            cout << op.type << ' ' << op.i << ' ' << op.j << ' ' << op.k << '\n';
        }
    }

    return 0;
}