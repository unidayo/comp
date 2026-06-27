#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>

using namespace std;
using Clock = chrono::steady_clock;
using TimePoint = chrono::time_point<Clock>;

static constexpr int MAX_L = 10005;
static constexpr int MAX_POS = 400;
static constexpr int INF16 = numeric_limits<uint16_t>::max();

struct Parent {
    int32_t prevIdx;
    int8_t m;
    int8_t d;
    bool isIns;
};

struct Op {
    int m;
    int d;
};

struct AddedOp {
    int t;
    int m;
    int d;
};

struct BeamState {
    vector<Op> ops;
    int eval;
};

static auto readInt() -> int {
    int x;
    cin >> x;
    return x;
}

static bool isCloudRun() {
    return getenv("K_SERVICE") != nullptr;
}

static vector<vector<vector<int>>> generateBelts(int N, bool flipH, bool flipV) {
    vector<vector<vector<int>>> belts;
    belts.reserve(N);

    for (int i = 0; i < N; i += 2) {
        vector<vector<int>> hBelt;
        hBelt.reserve(2 * N);
        if (!flipH) {
            for (int j = 0; j < N; j++) {
                hBelt.push_back({i, j});
            }
            for (int j = N - 1; j >= 0; j--) {
                hBelt.push_back({i + 1, j});
            }
        } else {
            for (int j = N - 1; j >= 0; j--) {
                hBelt.push_back({i, j});
            }
            for (int j = 0; j < N; j++) {
                hBelt.push_back({i + 1, j});
            }
        }
        belts.push_back(std::move(hBelt));
    }

    for (int j = 0; j < N; j += 2) {
        vector<vector<int>> vBelt;
        vBelt.reserve(2 * N);
        if (!flipV) {
            for (int i = 0; i < N; i++) {
                vBelt.push_back({i, j});
            }
            for (int i = N - 1; i >= 0; i--) {
                vBelt.push_back({i, j + 1});
            }
        } else {
            for (int i = N - 1; i >= 0; i--) {
                vBelt.push_back({i, j});
            }
            for (int i = 0; i < N; i++) {
                vBelt.push_back({i, j + 1});
            }
        }
        belts.push_back(std::move(vBelt));
    }

    return belts;
}

static uint64_t hashOps(const vector<Op>& ops) {
    uint64_t h = 14695981039346656037ULL;
    for (auto const& op : ops) {
        h ^= static_cast<uint64_t>(op.m);
        h *= 1099511628211ULL;
        h ^= static_cast<uint64_t>(op.d + 2);
        h *= 1099511628211ULL;
    }
    return h;
}

static pair<vector<vector<int>>, bool> greedyExtract(int N, const vector<vector<int>>& grid, const vector<vector<vector<int>>>& belts, const TimePoint& startTime, chrono::milliseconds timeLimit) {
    if (belts.empty()) {
        return {{}, true};
    }

    const array<int, 2> exitCell = {0, N / 2};
    const int exitIdx = exitCell[0] * 20 + exitCell[1];
    const int numBelts = static_cast<int>(belts.size());

    array<array<int, 2>, MAX_POS> cellBeltsFlat;
    array<int, MAX_POS> cellBeltsCount;
    cellBeltsCount.fill(0);
    for (int m = 0; m < numBelts; m++) {
        const auto& belt = belts[m];
        for (auto const& cell : belt) {
            const int idx = cell[0] * 20 + cell[1];
            if (cellBeltsCount[idx] < 2) {
                cellBeltsFlat[idx][cellBeltsCount[idx]] = m;
                cellBeltsCount[idx]++;
            }
        }
    }

    static int nextPos[MAX_POS][20];
    static int prevPos[MAX_POS][20];
    for (int i = 0; i < MAX_POS; i++) {
        for (int m = 0; m < 20; m++) {
            nextPos[i][m] = -1;
            prevPos[i][m] = -1;
        }
    }
    for (int m = 0; m < numBelts; m++) {
        const auto& belt = belts[m];
        int L = static_cast<int>(belt.size());
        for (int i = 0; i < L; i++) {
            const auto& cell = belt[i];
            const int idx = cell[0] * 20 + cell[1];
            const auto& nextCell = belt[(i + 1) % L];
            const auto& prevCell = belt[(i - 1 + L) % L];
            nextPos[idx][m] = nextCell[0] * 20 + nextCell[1];
            prevPos[idx][m] = prevCell[0] * 20 + prevCell[1];
        }
    }

    array<int, MAX_POS> initPosFlat;
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            initPosFlat[grid[r][c]] = r * 20 + c;
        }
    }

    array<int, MAX_POS> staticDist;
    staticDist.fill(1e9);
    staticDist[exitIdx] = 0;
    array<int, MAX_POS> q;
    int qh = 0, qt = 0;
    q[qt++] = exitIdx;
    while (qh < qt) {
        int u = q[qh++];
        for (int m = 0; m < cellBeltsCount[u]; m++) {
            int beltId = cellBeltsFlat[u][m];
            int nxt = nextPos[u][beltId];
            if (nxt != -1 && staticDist[nxt] > staticDist[u] + 1) {
                staticDist[nxt] = staticDist[u] + 1;
                q[qt++] = nxt;
            }
            int prv = prevPos[u][beltId];
            if (prv != -1 && staticDist[prv] > staticDist[u] + 1) {
                staticDist[prv] = staticDist[u] + 1;
                q[qt++] = prv;
            }
        }
    }

    vector<uint16_t> dist(MAX_POS * MAX_L, INF16);
    vector<Parent> parent(MAX_POS * MAX_L);
    vector<int> touched;
    touched.reserve(10000);
    vector<int32_t> dequeBuf(4000000);
    vector<uint32_t> protectedMask(MAX_L);
    array<int, MAX_POS> boxPos;
    vector<AddedOp> addedOps;
    addedOps.reserve(100);

    vector<BeamState> beam;
    beam.push_back(BeamState{vector<Op>(), 0});
    beam[0].ops.reserve(100);
    bool completed = true;
    const auto timeCutoff = timeLimit - chrono::milliseconds(20);

    for (int k = 0; k < N * N; k++) {
        auto globalElapsed = chrono::duration_cast<chrono::milliseconds>(Clock::now() - startTime);
        if (globalElapsed > timeCutoff) {
            completed = false;
            break;
        }

        auto remaining = timeLimit - globalElapsed;
        int beamWidth = 24;
        int numBranches = 4;
        if (remaining < chrono::milliseconds(1200)) {
            beamWidth = 18;
            numBranches = 4;
        }
        if (remaining < chrono::milliseconds(800)) {
            beamWidth = 12;
            numBranches = 2;
        }
        if (remaining < chrono::milliseconds(450)) {
            beamWidth = 6;
            numBranches = 1;
        }
        if (remaining < chrono::milliseconds(220)) {
            beamWidth = 1;
            numBranches = 1;
        }

        vector<BeamState> nextBeam;
        nextBeam.reserve(beamWidth * numBranches);
        array<uint64_t, 256> seenHash;
        int seenCount = 0;
        
        for (auto& state : beam) {
            const vector<Op>& ops = state.ops;
            int L = static_cast<int>(ops.size());

            fill(protectedMask.begin(), protectedMask.begin() + L + 1, 0);

            for (int i = 0; i < k; i++) {
                boxPos[i] = initPosFlat[i];
            }
            int nextExtract = 0;

            for (int t = 0; t < L; t++) {
                for (int i = nextExtract; i < k; i++) {
                    int idx = boxPos[i];
                    protectedMask[t] |= (1u << cellBeltsFlat[idx][0]) | (1u << cellBeltsFlat[idx][1]);
                }
                int m = ops[t].m;
                int dir = ops[t].d;
                if (dir == 1) {
                    for (int i = nextExtract; i < k; i++) {
                        int nxt = nextPos[boxPos[i]][m];
                        if (nxt != -1) {
                            boxPos[i] = nxt;
                        }
                    }
                } else {
                    for (int i = nextExtract; i < k; i++) {
                        int prv = prevPos[boxPos[i]][m];
                        if (prv != -1) {
                            boxPos[i] = prv;
                        }
                    }
                }
                while (nextExtract < k && boxPos[nextExtract] == exitIdx) {
                    nextExtract++;
                }
            }

            bool earlyFail = false;
            for (int branch = 0; branch < numBranches; branch++) {
                if (chrono::duration_cast<chrono::milliseconds>(Clock::now() - startTime) > timeCutoff) {
                    completed = false;
                    earlyFail = true;
                    break;
                }
                touched.clear();
                int startIdx = initPosFlat[k];
                int32_t startUIdx = 0 * 400 + startIdx;
                int head = 2000000;
                int tail = 2000000;
                head--;
                dequeBuf[head] = startUIdx;
                dist[startUIdx] = 0;
                touched.push_back(startUIdx);
                bool reached = false;
                int bfsSteps = 0;

                while (head < tail) {
                    if ((++bfsSteps & 1023) == 0 &&
                        chrono::duration_cast<chrono::milliseconds>(Clock::now() - startTime) > timeCutoff) {
                        completed = false;
                        break;
                    }
                    int32_t uIdx = dequeBuf[head++];
                    int t = static_cast<int>(uIdx / 400);
                    int posIdx = static_cast<int>(uIdx % 400);
                    uint16_t du = dist[uIdx];

                    if (posIdx == exitIdx && t == L) {
                        reached = true;
                        break;
                    }

                    if (t < L) {
                        int m = ops[t].m;
                        int d = ops[t].d;
                        int nPosIdx = posIdx;
                        if (d == 1) {
                            int nxt = nextPos[posIdx][m];
                            if (nxt != -1) {
                                nPosIdx = nxt;
                            }
                        } else {
                            int prv = prevPos[posIdx][m];
                            if (prv != -1) {
                                nPosIdx = prv;
                            }
                        }
                        int32_t nUIdx = static_cast<int32_t>((t + 1) * 400 + nPosIdx);
                        if (du < dist[nUIdx]) {
                            if (dist[nUIdx] == INF16) {
                                touched.push_back(nUIdx);
                            }
                            dist[nUIdx] = du;
                            parent[nUIdx] = Parent{uIdx, static_cast<int8_t>(m), static_cast<int8_t>(d), false};
                            head--;
                            dequeBuf[head] = nUIdx;
                        }
                    }

                    int m0 = cellBeltsFlat[posIdx][0];
                    int m1 = cellBeltsFlat[posIdx][1];
                    if (branch >= 2) {
                        swap(m0, m1);
                    }
                    const bool reverseDirOrder = (branch & 1) != 0;
                    const int32_t baseUIdx = static_cast<int32_t>(t * 400);
                    const uint16_t nd = static_cast<uint16_t>(du + 1);
                    auto pushInserted = [&](int nPosIdx, int beltId, int dir) {
                        if (nPosIdx == -1) {
                            return;
                        }
                        int32_t nUIdx = baseUIdx + nPosIdx;
                        if (nd < dist[nUIdx]) {
                            if (dist[nUIdx] == INF16) {
                                touched.push_back(nUIdx);
                            }
                            dist[nUIdx] = nd;
                            parent[nUIdx] = Parent{uIdx, static_cast<int8_t>(beltId), static_cast<int8_t>(dir), true};
                            dequeBuf[tail++] = nUIdx;
                        }
                    };

                    uint32_t mask = protectedMask[t];
                    if ((mask & (1u << m0)) == 0) {
                        if (!reverseDirOrder) {
                            pushInserted(nextPos[posIdx][m0], m0, 1);
                            pushInserted(prevPos[posIdx][m0], m0, -1);
                        } else {
                            pushInserted(prevPos[posIdx][m0], m0, -1);
                            pushInserted(nextPos[posIdx][m0], m0, 1);
                        }
                    }

                    if ((mask & (1u << m1)) == 0) {
                        if (!reverseDirOrder) {
                            pushInserted(nextPos[posIdx][m1], m1, 1);
                            pushInserted(prevPos[posIdx][m1], m1, -1);
                        } else {
                            pushInserted(prevPos[posIdx][m1], m1, -1);
                            pushInserted(nextPos[posIdx][m1], m1, 1);
                        }
                    }
                }

                for (int idx : touched) {
                    dist[idx] = INF16;
                }
                if (!reached) {
                    earlyFail = true;
                    break;
                }

                int32_t currIdx = static_cast<int32_t>(L * 400 + exitIdx);
                addedOps.clear();
                while (currIdx != startUIdx) {
                    Parent const& p = parent[currIdx];
                    if (p.isIns) {
                        int t = static_cast<int>(currIdx / 400);
                        addedOps.push_back(AddedOp{t, static_cast<int>(p.m), static_cast<int>(p.d)});
                    }
                    currIdx = p.prevIdx;
                }
                reverse(addedOps.begin(), addedOps.end());

                vector<Op> newOps;
                newOps.reserve(L + addedOps.size());
                int old_t = 0;
                for (auto const& add : addedOps) {
                    while (old_t < add.t) {
                        newOps.push_back(ops[old_t]);
                        old_t++;
                    }
                    newOps.push_back(Op{add.m, add.d});
                }
                while (old_t < L) {
                    newOps.push_back(ops[old_t]);
                    old_t++;
                }

                uint64_t h = hashOps(newOps);
                bool seen = false;
                for (int i = 0; i < seenCount; i++) {
                    if (seenHash[i] == h) {
                        seen = true;
                        break;
                    }
                }
                if (seen) {
                    continue;
                }
                seenHash[seenCount++] = h;

                const int W_LEN = 10000;
                const int W_BRANCH = 1;
                int eval = static_cast<int>(newOps.size()) * W_LEN + branch * W_BRANCH;

                int numEval = 0;
                array<int, 5> evalBoxPos;
                for (int i = k + 1; i < 400 && numEval < 5; i++) {
                    evalBoxPos[numEval++] = initPosFlat[i];
                }
                for (auto const& op : newOps) {
                    int m = op.m;
                    int dir = op.d;
                    if (dir == 1) {
                        for (int i = 0; i < numEval; i++) {
                            int nxt = nextPos[evalBoxPos[i]][m];
                            if (nxt != -1) {
                                evalBoxPos[i] = nxt;
                            }
                        }
                    } else {
                        for (int i = 0; i < numEval; i++) {
                            int prv = prevPos[evalBoxPos[i]][m];
                            if (prv != -1) {
                                evalBoxPos[i] = prv;
                            }
                        }
                    }
                }
                for (int i = 0; i < numEval; i++) {
                    eval += staticDist[evalBoxPos[i]] * 10;
                }
                nextBeam.push_back(BeamState{std::move(newOps), eval});
            }
            if (earlyFail) {
                break;
            }
        }

        sort(nextBeam.begin(), nextBeam.end(), [](auto const& a, auto const& b) {
            return a.eval < b.eval;
        });
        if (static_cast<int>(nextBeam.size()) > beamWidth) {
            nextBeam.resize(beamWidth);
        }
        if (!nextBeam.empty()) {
            beam = std::move(nextBeam);
        } else {
            completed = false;
            break;
        }
    }

    vector<Op> bestOps;
    if (!beam.empty()) {
        bestOps = beam[0].ops;
    }

    vector<vector<int>> res;
    res.reserve(bestOps.size());
    for (auto const& op : bestOps) {
        res.push_back({op.m, op.d});
    }
    return {res, completed};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N = readInt();
    vector<vector<int>> grid(N, vector<int>(N));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = readInt();
        }
    }

    chrono::milliseconds timeLimit(1900);
    if (isCloudRun()) {
        timeLimit *= 2;
    }
    TimePoint startTime = Clock::now();

    vector<vector<vector<int>>> bestBelts;
    vector<vector<int>> bestOps;
    int bestOpsLen = numeric_limits<int>::max();
    bool bestCompleted = false;

    for (int layout = 0; layout < 4; layout++) {
        chrono::milliseconds elapsed = chrono::duration_cast<chrono::milliseconds>(Clock::now() - startTime);
        if (layout > 0 && timeLimit - elapsed < chrono::milliseconds(30)) {
            break;
        }
        bool flipH = (layout & 1) != 0;
        bool flipV = (layout & 2) != 0;
        auto belts = generateBelts(N, flipH, flipV);
        auto [ops, completed] = greedyExtract(N, grid, belts, startTime, timeLimit);

        if (layout == 0 || (completed && (!bestCompleted || static_cast<int>(ops.size()) < bestOpsLen))) {
            bestBelts = std::move(belts);
            bestOps = ops;
            bestOpsLen = static_cast<int>(ops.size());
            bestCompleted = completed;
        }
    }

    cout << bestBelts.size() << '\n';
    for (auto const& belt : bestBelts) {
        cout << belt.size();
        for (auto const& cell : belt) {
            cout << ' ' << cell[0] << ' ' << cell[1];
        }
        cout << '\n';
    }
    cout << bestOps.size() << '\n';
    for (auto const& op : bestOps) {
        cout << op[0] << ' ' << op[1] << '\n';
    }
    return 0;
}
