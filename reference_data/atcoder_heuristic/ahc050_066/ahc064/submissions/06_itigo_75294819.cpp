#include <algorithm>
#include <array>
#include <chrono>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>
using namespace std;

static const int R = 10;
static const int N = 100;
static const int LINES = 20;
static const int CAP_D = 15;
static const int CAP_S = 20;
static const int MOVE_KEY_SIZE = 2 * R * R * 32;

struct EvalWeights {
    string name = "baseline";
    long long wBlocks = -1000000LL;
    long long wComplete = 90000LL;
    long long wReady = 0LL;
    long long wReadyWeighted = 25000LL;
    long long wEndpoint = 6000LL;
    long long wLocked = 3000LL;
    long long wCorrectPos = 2500LL;
    long long wCorrectLine = 800LL;
    long long wSumSq = 800LL;
    long long wHome = 250LL;
    long long wCrowd = -1500LL;
    long long beamCorrectPos = 5000LL;
    long long beamLocked = 2000LL;
    long long beamHome = 0LL;
    long long beamReadyWeighted = 0LL;
};

struct Move {
    int type, i, j, k;
};
struct Turn {
    vector<Move> ops;
};

struct State {
    array<array<unsigned char, CAP_S>, LINES> a{};
    array<unsigned char, LINES> len{};
};

struct Block {
    int line;
    int start;
    int len;
    int l, r;
    bool isD;
};

struct Features {
    long long eval = 0;
    int blocks = 0;
    int complete = 0;
    int ready = 0;
    int readyWeighted = 0;
    int locked = 0;
    int correctLine = 0;
    int correctPos = 0;
    int sumSq = 0;
    int endpoint = 0;
    int home = 0;
    int crowd = 0;
};

struct Analysis {
    vector<Block> blocks;
    array<int, N> bid{};
    array<int, R> dTail{};
    array<int, R> sFront{};
    array<int, R> lockedPrefix{};
    Features f;
};

EvalWeights gEval;

string getEnvString(const char* key, const string& fallback = "") {
    const char* value = getenv(key);
    return value ? string(value) : fallback;
}

double getEnvDouble(const char* key, double fallback) {
    const char* value = getenv(key);
    if (!value || !*value) return fallback;
    char* end = nullptr;
    double parsed = strtod(value, &end);
    return (end && *end == '\0') ? parsed : fallback;
}

int getEnvInt(const char* key, int fallback) {
    const char* value = getenv(key);
    if (!value || !*value) return fallback;
    char* end = nullptr;
    long parsed = strtol(value, &end, 10);
    return (end && *end == '\0') ? (int)parsed : fallback;
}

void overrideEnvLongLong(const char* key, long long& dest) {
    const char* value = getenv(key);
    if (!value || !*value) return;
    char* end = nullptr;
    long long parsed = strtoll(value, &end, 10);
    if (end && *end == '\0') dest = parsed;
}

EvalWeights makeEvalWeights(const string& profile) {
    EvalWeights w;
    w.name = profile;
    if (profile == "merge") {
        w.wBlocks = -1100000LL;
        w.wComplete = 100000LL;
        w.wReady = 6000LL;
        w.wReadyWeighted = 32000LL;
        w.wEndpoint = 9000LL;
        w.wLocked = 2000LL;
        w.wCorrectPos = 1500LL;
        w.wCorrectLine = 400LL;
        w.wSumSq = 1100LL;
        w.wHome = 150LL;
        w.wCrowd = -1200LL;
        w.beamCorrectPos = 3500LL;
        w.beamLocked = 1200LL;
        w.beamReadyWeighted = 300LL;
    } else if (profile == "finish") {
        w.wBlocks = -900000LL;
        w.wComplete = 120000LL;
        w.wReady = 2000LL;
        w.wReadyWeighted = 18000LL;
        w.wEndpoint = 3500LL;
        w.wLocked = 7000LL;
        w.wCorrectPos = 6000LL;
        w.wCorrectLine = 1200LL;
        w.wSumSq = 500LL;
        w.wHome = 1000LL;
        w.wCrowd = -2200LL;
        w.beamCorrectPos = 9000LL;
        w.beamLocked = 4500LL;
        w.beamHome = 150LL;
    } else if (profile == "home") {
        w.wBlocks = -1000000LL;
        w.wComplete = 100000LL;
        w.wReady = 3000LL;
        w.wReadyWeighted = 22000LL;
        w.wEndpoint = 5000LL;
        w.wLocked = 3500LL;
        w.wCorrectPos = 3000LL;
        w.wCorrectLine = 1000LL;
        w.wSumSq = 700LL;
        w.wHome = 1400LL;
        w.wCrowd = -3500LL;
        w.beamCorrectPos = 6000LL;
        w.beamLocked = 2500LL;
        w.beamHome = 200LL;
    } else if (profile == "simple") {
        w.wBlocks = -1000000LL;
        w.wComplete = 80000LL;
        w.wReady = 0LL;
        w.wReadyWeighted = 20000LL;
        w.wEndpoint = 0LL;
        w.wLocked = 4000LL;
        w.wCorrectPos = 3000LL;
        w.wCorrectLine = 0LL;
        w.wSumSq = 1000LL;
        w.wHome = 0LL;
        w.wCrowd = 0LL;
        w.beamCorrectPos = 6000LL;
        w.beamLocked = 2500LL;
    } else if (profile == "simpleish") {
        w.wBlocks = -1000000LL;
        w.wComplete = 90000LL;
        w.wReady = 0LL;
        w.wReadyWeighted = 25000LL;
        w.wEndpoint = 0LL;
        w.wLocked = 3000LL;
        w.wCorrectPos = 2500LL;
        w.wCorrectLine = 0LL;
        w.wSumSq = 1000LL;
        w.wHome = 0LL;
        w.wCrowd = 0LL;
        w.beamCorrectPos = 5000LL;
        w.beamLocked = 2000LL;
    } else if (profile == "hybrid1") {
        w.wBlocks = -1000000LL;
        w.wComplete = 90000LL;
        w.wReady = 4000LL;
        w.wReadyWeighted = 28000LL;
        w.wEndpoint = 0LL;
        w.wLocked = 3000LL;
        w.wCorrectPos = 2500LL;
        w.wCorrectLine = 0LL;
        w.wSumSq = 1000LL;
        w.wHome = 0LL;
        w.wCrowd = 0LL;
        w.beamCorrectPos = 5000LL;
        w.beamLocked = 2000LL;
        w.beamReadyWeighted = 150LL;
    } else if (profile == "hybrid2") {
        w.wBlocks = -1000000LL;
        w.wComplete = 90000LL;
        w.wReady = 2000LL;
        w.wReadyWeighted = 26000LL;
        w.wEndpoint = 0LL;
        w.wLocked = 3000LL;
        w.wCorrectPos = 2500LL;
        w.wCorrectLine = 0LL;
        w.wSumSq = 1000LL;
        w.wHome = 0LL;
        w.wCrowd = 0LL;
        w.beamCorrectPos = 5000LL;
        w.beamLocked = 2000LL;
        w.beamReadyWeighted = 100LL;
    } else if (profile == "ready") {
        w.wBlocks = -1000000LL;
        w.wComplete = 95000LL;
        w.wReady = 12000LL;
        w.wReadyWeighted = 26000LL;
        w.wEndpoint = 8000LL;
        w.wLocked = 2800LL;
        w.wCorrectPos = 2200LL;
        w.wCorrectLine = 600LL;
        w.wSumSq = 850LL;
        w.wHome = 200LL;
        w.wCrowd = -1400LL;
        w.beamCorrectPos = 4500LL;
        w.beamLocked = 1800LL;
        w.beamReadyWeighted = 400LL;
    } else if (profile == "spread") {
        w.wBlocks = -1000000LL;
        w.wComplete = 100000LL;
        w.wReady = 4000LL;
        w.wReadyWeighted = 23000LL;
        w.wEndpoint = 4500LL;
        w.wLocked = 3200LL;
        w.wCorrectPos = 2600LL;
        w.wCorrectLine = 900LL;
        w.wSumSq = 750LL;
        w.wHome = 500LL;
        w.wCrowd = -5000LL;
        w.beamCorrectPos = 5500LL;
        w.beamLocked = 2200LL;
        w.beamHome = 80LL;
    } else {
        w.name = "baseline";
    }

    overrideEnvLongLong("AHC064_W_BLOCKS", w.wBlocks);
    overrideEnvLongLong("AHC064_W_COMPLETE", w.wComplete);
    overrideEnvLongLong("AHC064_W_READY", w.wReady);
    overrideEnvLongLong("AHC064_W_READY_WEIGHTED", w.wReadyWeighted);
    overrideEnvLongLong("AHC064_W_ENDPOINT", w.wEndpoint);
    overrideEnvLongLong("AHC064_W_LOCKED", w.wLocked);
    overrideEnvLongLong("AHC064_W_CORRECT_POS", w.wCorrectPos);
    overrideEnvLongLong("AHC064_W_CORRECT_LINE", w.wCorrectLine);
    overrideEnvLongLong("AHC064_W_SUM_SQ", w.wSumSq);
    overrideEnvLongLong("AHC064_W_HOME", w.wHome);
    overrideEnvLongLong("AHC064_W_CROWD", w.wCrowd);
    overrideEnvLongLong("AHC064_BEAM_CORRECT_POS", w.beamCorrectPos);
    overrideEnvLongLong("AHC064_BEAM_LOCKED", w.beamLocked);
    overrideEnvLongLong("AHC064_BEAM_HOME", w.beamHome);
    overrideEnvLongLong("AHC064_BEAM_READY_WEIGHTED", w.beamReadyWeighted);
    return w;
}

long long evaluateFeatures(const Features& f, const EvalWeights& w) {
    long long eval = 0;
    eval += w.wBlocks * f.blocks;
    eval += w.wComplete * f.complete;
    eval += w.wReady * f.ready;
    eval += w.wReadyWeighted * f.readyWeighted;
    eval += w.wEndpoint * f.endpoint;
    eval += w.wLocked * f.locked;
    eval += w.wCorrectPos * f.correctPos;
    eval += w.wCorrectLine * f.correctLine;
    eval += w.wSumSq * f.sumSq;
    eval += w.wHome * f.home;
    eval += w.wCrowd * f.crowd;
    return eval;
}

static uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

static array<array<uint64_t, CAP_S + 1>, LINES> HASH_LEN;
static array<array<array<uint64_t, N>, CAP_S>, LINES> HASH_POS;

void initHashTables() {
    uint64_t seed = 0x3141592653589793ULL;
    for (int l = 0; l < LINES; l++) {
        for (int len = 0; len <= CAP_S; len++) {
            HASH_LEN[l][len] = splitmix64(seed++);
        }
        for (int p = 0; p < CAP_S; p++) {
            for (int id = 0; id < N; id++) {
                HASH_POS[l][p][id] = splitmix64(seed++);
            }
        }
    }
}

uint64_t hashState(const State& st) {
    uint64_t h = 0x123456789abcdef0ULL;
    for (int l = 0; l < LINES; l++) {
        h ^= HASH_LEN[l][st.len[l]];
        for (int p = 0; p < st.len[l]; p++) {
            h ^= HASH_POS[l][p][st.a[l][p]];
        }
    }
    return h;
}

inline bool adjId(int x, int y) {
    return x + 1 == y && x / 10 == y / 10;
}

int lockedPrefixOf(const State& st, int r) {
    int m = 0;
    while (m < (int)st.len[r] && m < 10 && (int)st.a[r][m] == 10 * r + m) m++;
    return m;
}

bool applyMove(State& st, const Move& mv) {
    int dl = mv.i;
    int sl = R + mv.j;
    int k = mv.k;
    if (k <= 0) return false;
    if (mv.type == 0) {
        if (st.len[dl] < k) return false;
        if (st.len[sl] + k > CAP_S) return false;
        array<unsigned char, CAP_S> seg{};
        int start = st.len[dl] - k;
        memcpy(seg.data(), st.a[dl].data() + start, k);
        st.len[dl] -= k;
        memmove(st.a[sl].data() + k, st.a[sl].data(), st.len[sl]);
        memcpy(st.a[sl].data(), seg.data(), k);
        st.len[sl] += k;
    } else {
        if (st.len[sl] < k) return false;
        if (st.len[dl] + k > CAP_D) return false;
        array<unsigned char, CAP_S> seg{};
        memcpy(seg.data(), st.a[sl].data(), k);
        memmove(st.a[sl].data(), st.a[sl].data() + k, st.len[sl] - k);
        st.len[sl] -= k;
        int start = st.len[dl];
        memcpy(st.a[dl].data() + start, seg.data(), k);
        st.len[dl] += k;
    }
    return true;
}

bool compatibleWithBatch(const vector<Move>& ops, const Move& mv) {
    for (const auto& e : ops) {
        if (e.i == mv.i || e.j == mv.j) return false;
        if (e.i < mv.i) {
            if (!(e.j < mv.j)) return false;
        } else if (e.i > mv.i) {
            if (!(e.j > mv.j)) return false;
        }
    }
    return true;
}

bool applyTurn(State& st, const Turn& tr) {
    // validate first on the original state
    bool usedD[R] = {}, usedS[R] = {};
    for (int x = 0; x < (int)tr.ops.size(); x++) {
        const auto& mv = tr.ops[x];
        if (mv.type < 0 || mv.type > 1 || mv.i < 0 || mv.i >= R || mv.j < 0 || mv.j >= R || mv.k <= 0) return false;
        if (usedD[mv.i] || usedS[mv.j]) return false;
        usedD[mv.i] = usedS[mv.j] = true;
        for (int y = 0; y < x; y++) {
            const auto& e = tr.ops[y];
            if (e.i < mv.i && !(e.j < mv.j)) return false;
            if (e.i > mv.i && !(e.j > mv.j)) return false;
        }
        if (mv.type == 0) {
            if (st.len[mv.i] < mv.k) return false;
            if (st.len[R + mv.j] + mv.k > CAP_S) return false;
        } else {
            if (st.len[R + mv.j] < mv.k) return false;
            if (st.len[mv.i] + mv.k > CAP_D) return false;
        }
    }
    for (const auto& mv : tr.ops) {
        if (!applyMove(st, mv)) return false;
    }
    return true;
}

Analysis analyze(const State& st) {
    Analysis an;
    an.blocks.reserve(64);
    an.bid.fill(-1);
    an.dTail.fill(-1);
    an.sFront.fill(-1);
    for (int r = 0; r < R; r++) an.lockedPrefix[r] = lockedPrefixOf(st, r);

    for (int line = 0; line < LINES; line++) {
        int p = 0;
        int firstBlock = -1;
        int lastBlock = -1;
        while (p < st.len[line]) {
            int q = p + 1;
            while (q < st.len[line] && adjId(st.a[line][q - 1], st.a[line][q])) q++;
            Block b{line, p, q - p, st.a[line][p], st.a[line][q - 1], line < R};
            int idb = (int)an.blocks.size();
            an.blocks.push_back(b);
            if (firstBlock == -1) firstBlock = idb;
            lastBlock = idb;
            for (int t = p; t < q; t++) an.bid[st.a[line][t]] = idb;
            p = q;
        }
        if (line < R) an.dTail[line] = lastBlock;
        else an.sFront[line - R] = firstBlock;
    }

    Features f;
    f.blocks = (int)an.blocks.size();
    for (const Block& b : an.blocks) {
        f.sumSq += b.len * b.len;
        int g = b.l / 10;
        if (b.len == 10 && b.l % 10 == 0) f.complete++;
        if (b.isD && b.line == g) f.home += b.len * b.len + 3 * b.len;
        if (!b.isD && b.line - R == g) f.home += b.len * b.len;
        if (b.isD && b.start + b.len == st.len[b.line] && b.r % 10 != 9) f.endpoint += b.len;
        if (!b.isD && b.start == 0 && b.l % 10 != 0) f.endpoint += b.len;
        if (!b.isD && b.start == 0 && b.l % 10 == 0) f.endpoint += 2 * b.len;
    }
    for (int i = 0; i < R; i++) {
        f.locked += an.lockedPrefix[i];
        for (int p = 0; p < st.len[i]; p++) {
            int id = st.a[i][p];
            if (id / 10 == i) f.correctLine++;
            if (p < 10 && id == 10 * i + p) f.correctPos++;
        }
        if (st.len[i] > 12) f.crowd += (st.len[i] - 12) * (st.len[i] - 12);
        if (st.len[R + i] > 18) f.crowd += (st.len[R + i] - 18) * (st.len[R + i] - 18);
    }
    int readyWeighted = 0;
    for (int i = 0; i < R; i++) if (an.dTail[i] != -1) {
        const Block& A = an.blocks[an.dTail[i]];
        for (int j = 0; j < R; j++) if (an.sFront[j] != -1) {
            const Block& B = an.blocks[an.sFront[j]];
            if (adjId(A.r, B.l)) {
                f.ready++;
                readyWeighted += 1 + A.len + B.len;
            }
        }
    }

    f.readyWeighted = readyWeighted;
    f.eval = evaluateFeatures(f, gEval);
    an.f = f;
    return an;
}

bool isFinal(const State& st) {
    for (int r = 0; r < R; r++) {
        if (st.len[r] != 10) return false;
        for (int p = 0; p < 10; p++) if ((int)st.a[r][p] != 10 * r + p) return false;
        if (st.len[R + r] != 0) return false;
    }
    return true;
}

struct RawCand {
    Move mv;
    long long bonus;
};
struct Cand {
    Move mv;
    long long score;
};

uint64_t moveKey(const Move& mv) {
    return (((uint64_t)mv.type * 10 + mv.i) * 10 + mv.j) * 32 + mv.k;
}

struct CandidateGenerator {
    const State& st;
    const Analysis& an;
    long long baseEval;
    vector<RawCand> raw;
    array<short, MOVE_KEY_SIZE> bestRaw;

    CandidateGenerator(const State& st_, const Analysis& an_) : st(st_), an(an_), baseEval(an_.f.eval) {
        bestRaw.fill(-1);
        raw.reserve(512);
    }

    bool canMove(const Move& mv) const {
        if (mv.k <= 0 || mv.i < 0 || mv.i >= R || mv.j < 0 || mv.j >= R) return false;
        if (mv.type == 0) {
            if (st.len[mv.i] < mv.k) return false;
            if (st.len[R + mv.j] + mv.k > CAP_S) return false;
            int start = st.len[mv.i] - mv.k;
            // Do not destroy a correct prefix.  Garbage after a completed prefix may be removed.
            if (start < an.lockedPrefix[mv.i]) return false;
        } else {
            if (st.len[R + mv.j] < mv.k) return false;
            if (st.len[mv.i] + mv.k > CAP_D) return false;
            if (an.lockedPrefix[mv.i] == 10 && st.len[mv.i] == 10) return false;
        }
        return true;
    }

    void add(const Move& mv, long long bonus) {
        if (!canMove(mv)) return;
        int key = (int)moveKey(mv);
        int idx = bestRaw[key];
        if (idx < 0) {
            idx = (int)raw.size();
            bestRaw[key] = (short)idx;
            raw.push_back({mv, bonus});
        } else {
            raw[idx].bonus = max(raw[idx].bonus, bonus);
        }
    }

    vector<int> targetsDToS(int d, int start, int k, int limit) const {
        vector<pair<int,int>> sc;
        sc.reserve(R);
        if (k <= 0 || start < 0 || start >= st.len[d]) return {};
        int firstBid = an.bid[st.a[d][start]];
        int lastBid  = an.bid[st.a[d][st.len[d] - 1]];
        const Block& first = an.blocks[firstBid];
        const Block& last = an.blocks[lastBid];
        int home1 = first.l / 10, home2 = last.l / 10;
        for (int s = 0; s < R; s++) {
            if (st.len[R + s] + k > CAP_S) continue;
            int h = 0;
            if (st.len[R + s] == 0) h += 900;
            if (s == home1) h += 700;
            if (s == home2) h += 500;
            if (an.sFront[s] != -1) {
                const Block& sf = an.blocks[an.sFront[s]];
                if (adjId(last.r, sf.l)) h += 10000 + (last.len + sf.len) * 200;
                if (adjId(first.r, sf.l)) h += 500;
            }
            h += (CAP_S - st.len[R + s]) * 10;
            h -= abs(d - s) * 5;
            sc.push_back({-h, s});
        }
        sort(sc.begin(), sc.end());
        vector<int> res;
        res.reserve(limit);
        for (auto [neg, s] : sc) {
            res.push_back(s);
            if ((int)res.size() >= limit) break;
        }
        return res;
    }

    vector<int> targetsSToD(int s, int k, int limit) const {
        vector<pair<int,int>> sc;
        sc.reserve(R);
        if (k <= 0 || k > st.len[R + s]) return {};
        int firstBid = an.bid[st.a[R + s][0]];
        int lastBid  = an.bid[st.a[R + s][k - 1]];
        const Block& first = an.blocks[firstBid];
        const Block& last = an.blocks[lastBid];
        int home1 = first.l / 10, home2 = last.l / 10;
        for (int d = 0; d < R; d++) {
            if (st.len[d] + k > CAP_D) continue;
            if (an.lockedPrefix[d] == 10 && st.len[d] == 10) continue;
            int h = 0;
            if (st.len[d] == 0) h += 700;
            if (d == home2) h += 650;
            if (d == home1) h += 350;
            int lp = an.lockedPrefix[d];
            if (lp < 10 && first.l == 10 * d + lp) h += 15000 + first.len * 500;
            if (an.dTail[d] != -1) {
                const Block& dt = an.blocks[an.dTail[d]];
                if (adjId(dt.r, first.l)) h += 10000 + (dt.len + first.len) * 200;
            }
            h += (CAP_D - st.len[d]) * 8;
            h -= abs(d - s) * 5;
            sc.push_back({-h, d});
        }
        sort(sc.begin(), sc.end());
        vector<int> res;
        res.reserve(limit);
        for (auto [neg, d] : sc) {
            res.push_back(d);
            if ((int)res.size() >= limit) break;
        }
        return res;
    }

    void generateRaw() {
        // 1. Immediate endpoint merges.
        for (int i = 0; i < R; i++) if (an.dTail[i] != -1) {
            const Block& A = an.blocks[an.dTail[i]];
            for (int j = 0; j < R; j++) if (an.sFront[j] != -1) {
                const Block& B = an.blocks[an.sFront[j]];
                if (!adjId(A.r, B.l)) continue;
                long long bonus = 8000000LL + 150000LL * (A.len + B.len) + 20000LL * (A.len + B.len) * (A.len + B.len);
                add({0, i, j, A.len}, bonus + (A.l % 10 == 0 ? 50000 : 0));
                add({1, i, j, B.len}, bonus + (B.r % 10 == 9 ? 50000 : 0));
            }
        }

        // 2. Lock the final prefix whenever a wanted block is already at a siding front.
        for (int d = 0; d < R; d++) {
            int lp = an.lockedPrefix[d];
            if (lp >= 10 || st.len[d] != lp) continue;
            int want = 10 * d + lp;
            for (int s = 0; s < R; s++) if (an.sFront[s] != -1) {
                const Block& B = an.blocks[an.sFront[s]];
                if (B.l == want) {
                    int take = min(B.len, 10 - lp);
                    add({1, d, s, take}, 7000000LL + 400000LL * take);
                }
            }
        }

        // 3. Generic endpoint relocation.  This is important for changing growth direction.
        for (int d = 0; d < R; d++) if (an.dTail[d] != -1) {
            const Block& B = an.blocks[an.dTail[d]];
            int start = B.start;
            int k = B.len;
            for (int s : targetsDToS(d, start, k, 4)) {
                long long bonus = 300000LL + 20000LL * B.len;
                if (B.l % 10 != 0) bonus += 20000LL * B.len; // good as siding front for left extension
                add({0, d, s, k}, bonus);
            }
        }
        for (int s = 0; s < R; s++) if (an.sFront[s] != -1) {
            const Block& B = an.blocks[an.sFront[s]];
            int k = B.len;
            for (int d : targetsSToD(s, k, 4)) {
                long long bonus = 300000LL + 20000LL * B.len;
                if (B.r % 10 != 9) bonus += 20000LL * B.len; // good as departure tail for right extension
                add({1, d, s, k}, bonus);
            }
        }

        // 4. Exposure moves for every missing final adjacency x | x+1.
        for (int x = 0; x + 1 < N; x++) {
            if (x / 10 != (x + 1) / 10) continue;
            int ba = an.bid[x], bb = an.bid[x + 1];
            if (ba == bb) continue;
            const Block& A = an.blocks[ba];
            const Block& B = an.blocks[bb];
            long long base = 900000LL + 50000LL * (A.len + B.len) + 5000LL * (A.len + B.len) * (A.len + B.len);

            // Put A at a departure tail.
            if (A.isD) {
                int d = A.line;
                int start = A.start + A.len;
                int k = st.len[d] - start;
                if (k > 0) {
                    for (int s : targetsDToS(d, start, k, 3)) {
                        add({0, d, s, k}, base - 1000LL * k);
                    }
                }
            } else {
                int s = A.line - R;
                int k = A.start + A.len;
                if (k > 0) {
                    for (int d : targetsSToD(s, k, 3)) {
                        add({1, d, s, k}, base - 1000LL * k);
                    }
                }
            }

            // Put B at a siding front.
            if (B.isD) {
                int d = B.line;
                int start = B.start;
                int k = st.len[d] - start;
                if (k > 0) {
                    for (int s : targetsDToS(d, start, k, 3)) {
                        add({0, d, s, k}, base - 1000LL * k);
                    }
                }
            } else {
                int s = B.line - R;
                int k = B.start;
                if (k > 0) {
                    for (int d : targetsSToD(s, k, 3)) {
                        add({1, d, s, k}, base - 1000LL * k);
                    }
                }
            }
        }
    }

    vector<Cand> finish(int maxCand) {
        generateRaw();
        vector<Cand> cands;
        cands.reserve(raw.size());
        for (const auto& rc : raw) {
            State ns = st;
            if (!applyMove(ns, rc.mv)) continue;
            Features nf = analyze(ns).f;
            long long sc = nf.eval - baseEval + rc.bonus - 200LL * rc.mv.k;
            cands.push_back({rc.mv, sc});
        }
        auto cmp = [](const Cand& a, const Cand& b){ return a.score > b.score; };
        if ((int)cands.size() > maxCand) {
            partial_sort(cands.begin(), cands.begin() + maxCand, cands.end(), cmp);
            cands.resize(maxCand);
        } else {
            sort(cands.begin(), cands.end(), cmp);
        }
        return cands;
    }
};

vector<Turn> makeBatches(const vector<Cand>& cands, mt19937_64& rng) {
    vector<Turn> out;
    out.reserve(72);
    auto addBatch = [&](vector<Move> ops) {
        if (ops.empty()) return;
        sort(ops.begin(), ops.end(), [](const Move& a, const Move& b){
            if (a.i != b.i) return a.i < b.i;
            return a.j < b.j;
        });
        out.push_back({ops});
    };

    int singles = min<int>(24, cands.size());
    for (int t = 0; t < singles; t++) addBatch({cands[t].mv});

    // Exact maximum-weight non-crossing matching on the 10x10 grid.
    // Greedy matchings are good at selecting one outstanding operation, but can miss
    // a set of medium-value independent operations.  The DP below chooses the best
    // increasing (i,j) matching after compressing candidates by pair.
    auto addDpMatching = [&](long long parallelBonus, long long noiseAmp) {
        int bestIdx[R][R];
        long long bestW[R][R];
        for (int i = 0; i < R; i++) for (int j = 0; j < R; j++) {
            bestIdx[i][j] = -1;
            bestW[i][j] = LLONG_MIN / 4;
        }
        int M = min<int>(90, cands.size());
        for (int idx = 0; idx < M; idx++) {
            const Move& mv = cands[idx].mv;
            long long noise = 0;
            if (noiseAmp > 0) {
                noise = (long long)(rng() % (2 * noiseAmp + 1)) - noiseAmp;
            }
            long long w = cands[idx].score + parallelBonus + noise;
            if (w > bestW[mv.i][mv.j]) {
                bestW[mv.i][mv.j] = w;
                bestIdx[mv.i][mv.j] = idx;
            }
        }

        long long dp[R + 1][R + 1];
        unsigned char par[R + 1][R + 1];
        for (int i = 0; i <= R; i++) for (int j = 0; j <= R; j++) {
            dp[i][j] = LLONG_MIN / 4;
            par[i][j] = 0;
        }
        dp[0][0] = 0;
        for (int i = 0; i <= R; i++) {
            for (int j = 0; j <= R; j++) {
                if (dp[i][j] <= LLONG_MIN / 8) continue;
                if (i < R && dp[i][j] > dp[i + 1][j]) {
                    dp[i + 1][j] = dp[i][j];
                    par[i + 1][j] = 1;
                }
                if (j < R && dp[i][j] > dp[i][j + 1]) {
                    dp[i][j + 1] = dp[i][j];
                    par[i][j + 1] = 2;
                }
                if (i < R && j < R && bestIdx[i][j] >= 0) {
                    long long val = dp[i][j] + bestW[i][j];
                    if (val > dp[i + 1][j + 1]) {
                        dp[i + 1][j + 1] = val;
                        par[i + 1][j + 1] = 3;
                    }
                }
            }
        }

        vector<Move> ops;
        int i = R, j = R;
        while (i > 0 || j > 0) {
            if (par[i][j] == 3) {
                int idx = bestIdx[i - 1][j - 1];
                if (idx >= 0 && bestW[i - 1][j - 1] > 0) ops.push_back(cands[idx].mv);
                i--;
                j--;
            } else if (par[i][j] == 1) {
                i--;
            } else if (par[i][j] == 2) {
                j--;
            } else {
                // Should only happen at (0,0), but keep it robust against ties.
                if (i > 0) i--;
                else if (j > 0) j--;
            }
        }
        addBatch(ops);
    };

    // Deterministic DP and a few perturbed DP matchings.  These are cheap because R=10.
    addDpMatching(20000LL, 0LL);
    const int DP_TRIALS = getEnvInt("AHC064_DP_TRIALS", 6);
    for (int tr = 0; tr < DP_TRIALS; tr++) {
        long long temp = 1800000LL / (tr + 2);
        addDpMatching(12000LL, temp);
    }

    // Deterministic greedy.
    {
        vector<Move> ops;
        for (int idx = 0; idx < (int)cands.size(); idx++) {
            if (compatibleWithBatch(ops, cands[idx].mv)) ops.push_back(cands[idx].mv);
        }
        addBatch(ops);
    }

    // Seeded greedies: force one strong single move, then fill the rest.
    // This recovers batches that deterministic greedy misses when the best global
    // operation conflicts with several slightly weaker but strategically important seeds.
    const int SEEDED = getEnvInt("AHC064_SEEDED_GREEDY", 16);
    for (int seed = 1; seed < min<int>((int)cands.size(), SEEDED + 1); seed++) {
        vector<Move> ops;
        ops.push_back(cands[seed].mv);
        for (int idx = 0; idx < (int)cands.size(); idx++) {
            if (idx == seed) continue;
            if (compatibleWithBatch(ops, cands[idx].mv)) ops.push_back(cands[idx].mv);
        }
        addBatch(ops);
    }

    // Several randomized greedy matchings.
    const int TRIALS = getEnvInt("AHC064_BATCH_TRIALS", 20);
    int M = min<int>(90, cands.size());
    for (int tr = 0; tr < TRIALS; tr++) {
        vector<pair<long long,int>> key;
        key.reserve(M);
        long long temp = 3000000LL / (tr + 2);
        for (int t = 0; t < M; t++) {
            long long noise = (long long)(rng() % (2 * temp + 1)) - temp;
            key.push_back({-(cands[t].score + noise), t});
        }
        sort(key.begin(), key.end());
        vector<Move> ops;
        for (auto [neg, idx] : key) {
            const Move& mv = cands[idx].mv;
            if (compatibleWithBatch(ops, mv)) ops.push_back(mv);
        }
        addBatch(ops);
    }

    // Deduplicate turn keys.
    vector<Turn> uniq;
    uniq.reserve(out.size());
    vector<uint64_t> seen;
    seen.reserve(out.size());
    for (auto& tr : out) {
        uint64_t sig = 0x123456789abcdef0ULL;
        for (auto& mv : tr.ops) {
            sig ^= splitmix64((uint64_t)moveKey(mv) + 0x9e3779b97f4a7c15ULL * (sig | 1ULL));
        }
        bool dup = false;
        for (uint64_t x : seen) {
            if (x == sig) {
                dup = true;
                break;
            }
        }
        if (!dup) {
            seen.push_back(sig);
            uniq.push_back(tr);
        }
    }
    return uniq;
}

bool appendSingle(vector<Turn>& plan, State& st, const Move& mv, int limitTurns) {
    if ((int)plan.size() >= limitTurns) return false;
    Turn tr; tr.ops.push_back(mv);
    if (!applyTurn(st, tr)) return false;
    plan.push_back(tr);
    return true;
}

int findCarLine(const State& st, int id, int& pos) {
    for (int l = 0; l < LINES; l++) {
        for (int p = 0; p < st.len[l]; p++) {
            if ((int)st.a[l][p] == id) { pos = p; return l; }
        }
    }
    pos = -1;
    return -1;
}

int chooseSWithCapacity(const State& st, int k, int prefer = -1) {
    if (prefer >= 0 && st.len[R + prefer] + k <= CAP_S) return prefer;
    int best = -1, bestFree = -1;
    for (int s = 0; s < R; s++) {
        int free = CAP_S - st.len[R + s];
        if (free >= k && free > bestFree) bestFree = free, best = s;
    }
    return best;
}

int chooseSWithAnyFree(const State& st) {
    int best = -1, bestFree = 0;
    for (int s = 0; s < R; s++) {
        int free = CAP_S - st.len[R + s];
        if (free > bestFree) bestFree = free, best = s;
    }
    return best;
}

int chooseTempD(const State& st, int dest) {
    int best = -1, bestFree = 0;
    for (int d = 0; d < R; d++) if (d != dest) {
        int free = CAP_D - st.len[d];
        if (free > bestFree) bestFree = free, best = d;
    }
    return best;
}

// Robust exact finisher.  It is intentionally conservative: one operation per turn.
pair<bool, vector<Turn>> greedyFinish(State input, int limitTurns) {
    State st = input;
    vector<Turn> plan;

    // Remove everything after the currently correct prefix of each departure line.
    for (int d = 0; d < R; d++) {
        while (true) {
            int keep = lockedPrefixOf(st, d);
            if (st.len[d] <= keep) break;
            int k = st.len[d] - keep;
            int s = chooseSWithCapacity(st, k, d);
            if (s >= 0) {
                if (!appendSingle(plan, st, {0, d, s, k}, limitTurns)) return {false, {}};
            } else {
                int s2 = chooseSWithAnyFree(st);
                if (s2 < 0) return {false, {}};
                int kk = min<int>(k, CAP_S - st.len[R + s2]);
                if (kk <= 0) return {false, {}};
                if (!appendSingle(plan, st, {0, d, s2, kk}, limitTurns)) return {false, {}};
            }
        }
    }

    for (int d = 0; d < R; d++) {
        while (lockedPrefixOf(st, d) < 10) {
            int m = lockedPrefixOf(st, d);
            int want = 10 * d + m;
            int pos = -1;
            int line = findCarLine(st, want, pos);
            if (line < 0) return {false, {}};

            if (line < R) {
                // Should rarely happen after normalization. Move the suffix containing want to a siding.
                int k = st.len[line] - pos;
                int s = chooseSWithCapacity(st, k, want / 10);
                if (s < 0) {
                    s = chooseSWithAnyFree(st);
                    if (s < 0) return {false, {}};
                    k = min<int>(k, CAP_S - st.len[R + s]);
                }
                if (!appendSingle(plan, st, {0, line, s, k}, limitTurns)) return {false, {}};
                continue;
            }

            int s = line - R;
            vector<pair<int,int>> chunks; // temp D, length, in front-to-back order.
            int rem = pos;
            while (rem > 0) {
                int td = chooseTempD(st, d);
                if (td < 0) return {false, {}};
                int free = CAP_D - st.len[td];
                if (free <= 0) return {false, {}};
                int kk = min(rem, free);
                if (!appendSingle(plan, st, {1, td, s, kk}, limitTurns)) return {false, {}};
                chunks.push_back({td, kk});
                rem -= kk;
            }

            if (st.len[R + s] == 0 || (int)st.a[R + s][0] != want) return {false, {}};
            int take = 1;
            while (take < st.len[R + s] && take < 10 - m && adjId(st.a[R + s][take - 1], st.a[R + s][take])) take++;
            if (!appendSingle(plan, st, {1, d, s, take}, limitTurns)) return {false, {}};

            for (int idx = (int)chunks.size() - 1; idx >= 0; idx--) {
                int td = chunks[idx].first;
                int kk = chunks[idx].second;
                if (!appendSingle(plan, st, {0, td, s, kk}, limitTurns)) return {false, {}};
            }
        }
    }

    if (!isFinal(st)) return {false, {}};
    return {true, plan};
}

long long searchScoreOf(const Features& f) {
    long long sc = f.eval;
    sc += gEval.beamCorrectPos * f.correctPos;
    sc += gEval.beamLocked * f.locked;
    sc += gEval.beamHome * f.home;
    sc += gEval.beamReadyWeighted * f.readyWeighted;
    return sc;
}

struct TreeNode {
    State st;
    int parent = -1;
    Turn turn;
    int depth = 0;
    long long score = 0;
    int blocks = 0;
    int complete = 0;
    uint64_t hash = 0;
};

bool betterNode(const TreeNode& a, const TreeNode& b) {
    if (a.score != b.score) return a.score > b.score;
    if (a.blocks != b.blocks) return a.blocks < b.blocks;
    if (a.complete != b.complete) return a.complete > b.complete;
    return a.depth < b.depth;
}

vector<Turn> reconstruct(const vector<TreeNode>& nodes, int idx) {
    vector<Turn> path;
    while (idx >= 0 && nodes[idx].parent != -1) {
        const TreeNode& nd = nodes[idx];
        path.push_back(nd.turn);
        idx = nd.parent;
    }
    reverse(path.begin(), path.end());
    return path;
}

struct DepthBucket {
    vector<int> ids;
};

bool pushBucket(DepthBucket& bucket, const vector<TreeNode>& nodes, int width, int nodeId) {
    auto& ids = bucket.ids;
    for (int idx = 0; idx < (int)ids.size(); idx++) {
        int cur = ids[idx];
        if (nodes[cur].hash != nodes[nodeId].hash) continue;
        if (!betterNode(nodes[nodeId], nodes[cur])) return false;
        ids.erase(ids.begin() + idx);
        break;
    }

    int pos = 0;
    while (pos < (int)ids.size() && betterNode(nodes[ids[pos]], nodes[nodeId])) pos++;
    if ((int)ids.size() >= width && pos >= width) return false;
    ids.insert(ids.begin() + pos, nodeId);
    if ((int)ids.size() > width) ids.pop_back();
    return true;
}

int popBestNode(DepthBucket& bucket) {
    if (bucket.ids.empty()) return -1;
    int id = bucket.ids.front();
    bucket.ids.erase(bucket.ids.begin());
    return id;
}

bool verifyPlan(State st, const vector<Turn>& plan, bool needFinal) {
    if ((int)plan.size() > 4000) return false;
    for (const auto& tr : plan) {
        if (tr.ops.empty() || (int)tr.ops.size() > R) return false;
        if (!applyTurn(st, tr)) return false;
    }
    return !needFinal || isFinal(st);
}

bool canApplyTurnOnState(const State& st, const vector<Move>& ops) {
    if (ops.empty() || (int)ops.size() > R) return false;
    State tmp = st;
    Turn tr{ops};
    return applyTurn(tmp, tr);
}

vector<Turn> compressPlan(const State& init, const vector<Turn>& plan, bool needFinal) {
    if (plan.empty()) return plan;
    vector<Move> seq;
    seq.reserve(plan.size() * 3);
    for (const Turn& tr : plan) {
        for (const Move& mv : tr.ops) seq.push_back(mv);
    }

    State st = init;
    vector<Turn> res;
    vector<Move> cur;
    cur.reserve(R);
    auto flush = [&]() -> bool {
        if (cur.empty()) return true;
        Turn tr{cur};
        if (!applyTurn(st, tr)) return false;
        res.push_back(std::move(tr));
        cur.clear();
        return true;
    };

    for (const Move& mv : seq) {
        vector<Move> trial = cur;
        if (compatibleWithBatch(trial, mv)) {
            trial.push_back(mv);
            if (canApplyTurnOnState(st, trial)) {
                cur.push_back(mv);
                continue;
            }
        }
        if (!flush()) return plan;
        vector<Move> single{mv};
        if (!canApplyTurnOnState(st, single)) return plan;
        cur.push_back(mv);
    }
    if (!flush()) return plan;
    if (!verifyPlan(init, res, needFinal)) return plan;
    return res.size() <= plan.size() ? res : plan;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    gEval = makeEvalWeights(getEnvString("AHC064_PROFILE", "hybrid1"));
    initHashTables();

    int Rin;
    if (!(cin >> Rin)) return 0;
    State init;
    for (int r = 0; r < R; r++) {
        init.len[r] = 10;
        for (int c = 0; c < 10; c++) {
            int x; cin >> x;
            init.a[r][c] = (unsigned char)x;
        }
    }
    for (int s = 0; s < R; s++) init.len[R + s] = 0;

    auto startTime = chrono::steady_clock::now();
    auto elapsed = [&]() -> double {
        return chrono::duration<double>(chrono::steady_clock::now() - startTime).count();
    };

    // Guaranteed exact baseline.
    auto fb = greedyFinish(init, 4000);
    vector<Turn> bestPlan;
    if (fb.first) bestPlan = fb.second;
    else bestPlan.clear(); // should not happen

    mt19937_64 rng(20260426ULL);

    Analysis initAn = analyze(init);
    TreeNode root;
    root.st = init;
    root.parent = -1;
    root.depth = 0;
    root.score = searchScoreOf(initAn.f);
    root.blocks = initAn.f.blocks;
    root.complete = initAn.f.complete;
    root.hash = hashState(init);
    vector<TreeNode> nodes;
    nodes.reserve(80000);
    nodes.push_back(root);

    const double TIME_LIMIT = getEnvDouble("AHC064_TIME_LIMIT", 1.85);
    const int BEAM_WIDTH = getEnvInt("AHC064_BEAM_WIDTH", 44);
    const int MAX_DEPTH = getEnvInt("AHC064_MAX_DEPTH", 210);
    const int MAX_CAND = getEnvInt("AHC064_MAX_CAND", 64);
    const int TAIL_CHECK_PER_EXPAND = getEnvInt("AHC064_TAIL_CHECK_PER_EXPAND", getEnvInt("AHC064_TAIL_CHECK", 3));
    const int TAIL_TRY_LIMIT = getEnvInt("AHC064_TAIL_TRY_LIMIT", 600);

    vector<DepthBucket> frontier(MAX_DEPTH + 1);
    frontier[0].ids.push_back(0);

    int tailTries = 0;
    while (elapsed() <= TIME_LIMIT) {
        bool progressed = false;
        for (int depth = 0; depth < MAX_DEPTH; depth++) {
            if (elapsed() > TIME_LIMIT) break;
            if (!bestPlan.empty() && depth + 1 >= (int)bestPlan.size()) break;

            int nodeId = popBestNode(frontier[depth]);
            if (nodeId < 0) continue;
            progressed = true;

            State st = nodes[nodeId].st;
            Analysis an = analyze(st);
            CandidateGenerator gen(st, an);
            vector<Cand> cands = gen.finish(MAX_CAND);
            if (cands.empty()) continue;

            vector<Turn> batches = makeBatches(cands, rng);
            vector<int> accepted;
            accepted.reserve(batches.size());

            for (const Turn& tr : batches) {
                State ns = st;
                if (!applyTurn(ns, tr)) continue;
                Analysis na = analyze(ns);

                TreeNode child;
                child.st = ns;
                child.parent = nodeId;
                child.turn = tr;
                child.depth = depth + 1;
                child.score = searchScoreOf(na.f);
                child.blocks = na.f.blocks;
                child.complete = na.f.complete;
                child.hash = hashState(ns);

                int childId = (int)nodes.size();
                nodes.push_back(std::move(child));
                if (!pushBucket(frontier[depth + 1], nodes, BEAM_WIDTH, childId)) continue;
                accepted.push_back(childId);
            }
            if (accepted.empty()) continue;

            auto childCmp = [&](int lhs, int rhs) {
                return betterNode(nodes[lhs], nodes[rhs]);
            };
            stable_sort(accepted.begin(), accepted.end(), childCmp);

            for (int childId : accepted) {
                if (isFinal(nodes[childId].st)) {
                    vector<Turn> path = reconstruct(nodes, childId);
                    if (bestPlan.empty() || path.size() < bestPlan.size()) bestPlan = std::move(path);
                }
            }

            int checks = min<int>(TAIL_CHECK_PER_EXPAND, accepted.size());
            for (int idx = 0; idx < checks; idx++) {
                if (elapsed() > TIME_LIMIT || tailTries >= TAIL_TRY_LIMIT) break;
                int childId = accepted[idx];
                int remain = bestPlan.empty() ? 4000 : (int)bestPlan.size() - nodes[childId].depth - 1;
                if (remain <= 0) continue;
                tailTries++;
                auto tail = greedyFinish(nodes[childId].st, remain);
                if (!tail.first) continue;
                vector<Turn> path = reconstruct(nodes, childId);
                path.insert(path.end(), tail.second.begin(), tail.second.end());
                if (bestPlan.empty() || path.size() < bestPlan.size()) bestPlan = std::move(path);
            }
        }
        if (!progressed) break;
    }

    if (bestPlan.empty() || !verifyPlan(init, bestPlan, true)) {
        auto fb2 = greedyFinish(init, 4000);
        bestPlan = fb2.second;
    }

    bestPlan = compressPlan(init, bestPlan, true);

    cout << bestPlan.size() << '\n';
    for (const Turn& tr : bestPlan) {
        cout << tr.ops.size() << '\n';
        for (const Move& mv : tr.ops) {
            cout << mv.type << ' ' << mv.i << ' ' << mv.j << ' ' << mv.k << '\n';
        }
    }
    return 0;
}
