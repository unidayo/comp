#include <bits/stdc++.h>
using namespace std;

static const int N = 20;
static const int BN = 10;
static const int L = 200;
static const int V = 400;
static const int INF = 1e9;

struct Coord
{
    int r, c;
    bool operator==(const Coord &o) const { return r == o.r && c == o.c; }
    bool operator<(const Coord &o) const { return r != o.r ? r < o.r : c < o.c; }
};

struct Candidate
{
    vector<Coord> rail;
    vector<Coord> buf;
    int buildCost = INF;
};

struct SimResult
{
    bool ok = false;
    vector<pair<int, int>> ops;
};

int a[N][N];

vector<pair<int, int>> make_base_seq()
{
    vector<pair<int, int>> s;

    for (int c = 0; c < BN; c++)
        s.push_back({0, c});

    for (int c = BN - 1; c >= 1; c--)
    {
        if (c == BN - 1)
        {
            for (int r = 1; r < BN; r++)
                s.push_back({r, c});
        }
        else
        {
            int r = s.back().first;
            s.push_back({r, c});

            if (r == BN - 1)
            {
                for (int rr = BN - 2; rr >= 1; rr--)
                    s.push_back({rr, c});
            }
            else
            {
                for (int rr = 2; rr < BN; rr++)
                    s.push_back({rr, c});
            }
        }
    }

    s.push_back({BN - 1, 0});
    for (int r = BN - 2; r >= 1; r--)
        s.push_back({r, 0});

    return s;
}

pair<int, int> transform_block(pair<int, int> p, int t)
{
    int r = p.first;
    int c = p.second;
    int n = BN;

    switch (t)
    {
    case 0:
        return {r, c};
    case 1:
        return {r, n - 1 - c};
    case 2:
        return {n - 1 - r, c};
    case 3:
        return {n - 1 - r, n - 1 - c};
    case 4:
        return {c, r};
    case 5:
        return {c, n - 1 - r};
    case 6:
        return {n - 1 - c, r};
    case 7:
        return {n - 1 - c, n - 1 - r};
    }

    return {r, c};
}

// 0: U, 1: D, 2: L, 3: R
int side_from_delta(int dr, int dc)
{
    if (dr == -1)
        return 0;
    if (dr == 1)
        return 1;
    if (dc == -1)
        return 2;
    return 3;
}

pair<int, int> port_cell(int side, int bit)
{
    if (side == 0)
        return {0, bit}; // U
    if (side == 1)
        return {1, bit}; // D
    if (side == 2)
        return {bit, 0}; // L
    return {bit, 1};     // R
}

struct LocalTrans
{
    int ib, ob;
    pair<int, int> p, q;
};

vector<LocalTrans> allowed_trans(int inSide, int outSide)
{
    vector<LocalTrans> res;

    for (int ib = 0; ib < 2; ib++)
    {
        for (int ob = 0; ob < 2; ob++)
        {
            auto p = port_cell(inSide, ib);
            auto q = port_cell(outSide, ob);

            int d = abs(p.first - q.first) + abs(p.second - q.second);
            if (p != q && d == 1)
            {
                res.push_back({ib, ob, p, q});
            }
        }
    }

    return res;
}

optional<Candidate> build_candidate(int transId, bool rev, bool penaltyZero)
{
    vector<pair<int, int>> base = make_base_seq();
    vector<pair<int, int>> seq;

    for (auto p : base)
        seq.push_back(transform_block(p, transId));
    if (rev)
        reverse(seq.begin(), seq.end());

    if ((int)seq.size() != 100)
        return nullopt;

    {
        set<pair<int, int>> st(seq.begin(), seq.end());
        if ((int)st.size() != 100)
            return nullopt;
    }

    for (int k = 0; k < 100; k++)
    {
        auto x = seq[k];
        auto y = seq[(k + 1) % 100];

        if (abs(x.first - y.first) + abs(x.second - y.second) != 1)
        {
            return nullopt;
        }
    }

    vector<int> inSide(100), outSide(100);

    for (int k = 0; k < 100; k++)
    {
        auto cur = seq[k];
        auto prv = seq[(k + 99) % 100];
        auto nxt = seq[(k + 1) % 100];

        inSide[k] = side_from_delta(prv.first - cur.first, prv.second - cur.second);
        outSide[k] = side_from_delta(nxt.first - cur.first, nxt.second - cur.second);
    }

    int bestCost = INF;
    vector<int> bestBits;

    for (int start = 0; start < 2; start++)
    {
        int dp[101][2];
        int par[101][2];

        for (int i = 0; i <= 100; i++)
        {
            for (int b = 0; b < 2; b++)
            {
                dp[i][b] = INF;
                par[i][b] = -1;
            }
        }

        dp[0][start] = 0;

        for (int k = 0; k < 100; k++)
        {
            auto [br, bc] = seq[k];

            // exit (0,10) belongs to block (0,5), local (0,0).
            bool hasExitBlock = (br == 0 && bc == 5);

            auto ats = allowed_trans(inSide[k], outSide[k]);

            for (int prevBit = 0; prevBit < 2; prevBit++)
            {
                if (dp[k][prevBit] >= INF)
                    continue;

                for (auto tr : ats)
                {
                    if (tr.ib != prevBit)
                        continue;

                    Coord c1{2 * br + tr.p.first, 2 * bc + tr.p.second};
                    Coord c2{2 * br + tr.q.first, 2 * bc + tr.q.second};

                    if (hasExitBlock)
                    {
                        bool ok = false;
                        if (c1.r == 0 && c1.c == 10)
                            ok = true;
                        if (c2.r == 0 && c2.c == 10)
                            ok = true;
                        if (!ok)
                            continue;
                    }

                    int add = 0;
                    Coord cs[2] = {c1, c2};

                    for (Coord cc : cs)
                    {
                        if (a[cc.r][cc.c] < 200)
                            add++;

                        // box 0 が rail 上にいると parking 中に早期搬出されて
                        // 第 2 バッチが少し崩れることがあるので、別候補として避ける。
                        if (penaltyZero && a[cc.r][cc.c] == 0 && !(cc.r == 0 && cc.c == 10))
                        {
                            add += 1000;
                        }
                    }

                    int nb = tr.ob;

                    if (dp[k + 1][nb] > dp[k][prevBit] + add)
                    {
                        dp[k + 1][nb] = dp[k][prevBit] + add;
                        par[k + 1][nb] = prevBit;
                    }
                }
            }
        }

        if (dp[100][start] < bestCost)
        {
            bestCost = dp[100][start];

            vector<int> bits(100);
            int cur = start;

            for (int k = 99; k >= 0; k--)
            {
                bits[k] = cur;
                cur = par[k + 1][cur];
            }

            bestBits = bits;
        }
    }

    if (bestCost >= INF)
        return nullopt;

    Candidate cand;
    cand.buildCost = bestCost;
    cand.rail.reserve(200);
    cand.buf.reserve(200);

    for (int k = 0; k < 100; k++)
    {
        int ib = bestBits[(k + 99) % 100];
        int ob = bestBits[k];

        auto ats = allowed_trans(inSide[k], outSide[k]);

        bool found = false;
        LocalTrans chosen;

        for (auto tr : ats)
        {
            if (tr.ib == ib && tr.ob == ob)
            {
                chosen = tr;
                found = true;
                break;
            }
        }

        if (!found)
            return nullopt;

        auto [br, bc] = seq[k];

        Coord r1{2 * br + chosen.p.first, 2 * bc + chosen.p.second};
        Coord r2{2 * br + chosen.q.first, 2 * bc + chosen.q.second};

        cand.rail.push_back(r1);
        cand.rail.push_back(r2);

        pair<int, int> lp = chosen.p;
        pair<int, int> lq = chosen.q;

        auto pair_buffer = [&](pair<int, int> loc) -> Coord
        {
            if (lp.first == lq.first)
            {
                // rail domino is horizontal.
                int otherRow = 1 - lp.first;
                return Coord{2 * br + otherRow, 2 * bc + loc.second};
            }
            else
            {
                // rail domino is vertical.
                int otherCol = 1 - lp.second;
                return Coord{2 * br + loc.first, 2 * bc + otherCol};
            }
        };

        cand.buf.push_back(pair_buffer(lp));
        cand.buf.push_back(pair_buffer(lq));
    }

    if ((int)cand.rail.size() != 200 || (int)cand.buf.size() != 200)
        return nullopt;

    set<pair<int, int>> usedRail, usedBuf;
    bool hasExit = false;

    for (auto c : cand.rail)
    {
        if (c.r < 0 || c.r >= 20 || c.c < 0 || c.c >= 20)
            return nullopt;
        usedRail.insert({c.r, c.c});

        if (c.r == 0 && c.c == 10)
            hasExit = true;
    }

    for (auto c : cand.buf)
    {
        if (c.r < 0 || c.r >= 20 || c.c < 0 || c.c >= 20)
            return nullopt;
        usedBuf.insert({c.r, c.c});
    }

    if ((int)usedRail.size() != 200)
        return nullopt;
    if ((int)usedBuf.size() != 200)
        return nullopt;
    if (!hasExit)
        return nullopt;

    for (auto p : usedRail)
    {
        if (usedBuf.count(p))
            return nullopt;
    }

    for (int i = 0; i < 200; i++)
    {
        Coord x = cand.rail[i];
        Coord y = cand.rail[(i + 1) % 200];

        if (abs(x.r - y.r) + abs(x.c - y.c) != 1)
            return nullopt;

        Coord b = cand.buf[i];
        if (abs(x.r - b.r) + abs(x.c - b.c) != 1)
            return nullopt;
    }

    return cand;
}

SimResult simulate_candidate(const Candidate &cand)
{
    SimResult res;

    int exitIdx = -1;
    for (int i = 0; i < L; i++)
    {
        if (cand.rail[i].r == 0 && cand.rail[i].c == 10)
        {
            exitIdx = i;
        }
    }

    if (exitIdx < 0)
        return res;

    vector<int> rail(L), buf(L);

    for (int i = 0; i < L; i++)
    {
        rail[i] = a[cand.rail[i].r][cand.rail[i].c];
        buf[i] = a[cand.buf[i].r][cand.buf[i].c];
    }

    int need = 0;

    auto remove_check = [&]()
    {
        if (need < V && rail[exitIdx] == need)
        {
            rail[exitIdx] = -1;
            need++;
        }
    };

    auto do_swap = [&](int p)
    {
        swap(rail[p], buf[p]);
        res.ops.push_back({1 + p, 1});
        remove_check();
    };

    auto do_rot = [&](int dir)
    {
        vector<int> nr(L, -1);

        if (dir == 1)
        {
            for (int i = 0; i < L; i++)
            {
                nr[(i + 1) % L] = rail[i];
            }
        }
        else
        {
            for (int i = 0; i < L; i++)
            {
                nr[(i + L - 1) % L] = rail[i];
            }
        }

        rail.swap(nr);
        res.ops.push_back({0, dir});
        remove_check();
    };

    // Initial removal.
    remove_check();

    auto in_batch = [](int x, int base, int end)
    {
        return x >= base && x < end;
    };

    int rounds = 0;

    while (need < V && (int)res.ops.size() <= 100000 && rounds < 10)
    {
        int base = need;
        int end = min(V, base + L);

        // parking pass:
        // batch の箱が rail 上にあれば buffer へ退避する。
        while (true)
        {
            vector<pair<int, int>> targets;

            for (int p = 0; p < L; p++)
            {
                if (in_batch(rail[p], base, end))
                {
                    targets.push_back({rail[p], p});
                }
            }

            if (targets.empty())
                break;

            vector<int> frees;

            for (int p = 0; p < L; p++)
            {
                if (buf[p] == -1 || !in_batch(buf[p], base, end))
                {
                    frees.push_back(p);
                }
            }

            if ((int)frees.size() < (int)targets.size())
                return res;

            auto make_assign = [&](int dir)
            {
                vector<int> freeNow = frees;
                vector<int> assigned(V, -1);

                int maxDist = 0;

                sort(targets.begin(), targets.end(), [](auto x, auto y)
                     { return x.second < y.second; });

                for (auto [x, s] : targets)
                {
                    int bestIdx = -1;
                    int bestDist = INF;

                    for (int idx = 0; idx < (int)freeNow.size(); idx++)
                    {
                        int p = freeNow[idx];

                        int dist;
                        if (dir == 1)
                            dist = (p - s + L) % L;
                        else
                            dist = (s - p + L) % L;

                        if (dist < bestDist)
                        {
                            bestDist = dist;
                            bestIdx = idx;
                        }
                    }

                    int p = freeNow[bestIdx];
                    freeNow.erase(freeNow.begin() + bestIdx);

                    assigned[x] = p;
                    maxDist = max(maxDist, bestDist);
                }

                int score = maxDist + (int)targets.size();
                return pair<int, vector<int>>(score, assigned);
            };

            auto [scoreP, assignP] = make_assign(1);
            auto [scoreM, assignM] = make_assign(-1);

            int dir;
            vector<int> assigned;

            if (scoreP <= scoreM)
            {
                dir = 1;
                assigned = move(assignP);
            }
            else
            {
                dir = -1;
                assigned = move(assignM);
            }

            vector<char> pending(V, 0);
            int pendingCnt = 0;

            for (auto [x, s] : targets)
            {
                if (!pending[x])
                {
                    pending[x] = 1;
                    pendingCnt++;
                }
            }

            for (int step = 0; step <= L && pendingCnt > 0; step++)
            {
                vector<int> sws;

                for (int p = 0; p < L; p++)
                {
                    int x = rail[p];

                    if (x >= 0 && x < V && pending[x] && assigned[x] == p)
                    {
                        sws.push_back(p);
                    }
                }

                for (int p : sws)
                {
                    int x = rail[p];

                    if (x >= 0 && x < V && pending[x] && assigned[x] == p)
                    {
                        do_swap(p);

                        if (pending[x])
                        {
                            pending[x] = 0;
                            pendingCnt--;
                        }
                    }
                }

                // 退避前に出口で出てしまった箱を pending から消す。
                vector<char> present(V, 0);
                for (int x : rail)
                {
                    if (x >= 0)
                        present[x] = 1;
                }

                for (auto [x, s] : targets)
                {
                    if (pending[x] && !present[x])
                    {
                        pending[x] = 0;
                        pendingCnt--;
                    }
                }

                if (pendingCnt == 0)
                    break;

                do_rot(dir);
            }
        }

        // loading pass:
        // buffer 上の batch 箱を、搬出順になる slot に入れる。
        vector<int> targetSlot(V, -1);

        for (int x = base; x < end; x++)
        {
            int offset = x - base + 1;
            targetSlot[x] = (exitIdx - offset) % L;
            if (targetSlot[x] < 0)
                targetSlot[x] += L;
        }

        for (int t = 0; t < L; t++)
        {
            vector<int> sws;

            for (int p = 0; p < L; p++)
            {
                int x = buf[p];

                if (in_batch(x, base, end))
                {
                    int finalPos = (p + (L - t)) % L;

                    if (finalPos == targetSlot[x])
                    {
                        sws.push_back(p);
                    }
                }
            }

            for (int p : sws)
            {
                int x = buf[p];

                if (in_batch(x, base, end))
                {
                    int finalPos = (p + (L - t)) % L;

                    if (finalPos == targetSlot[x])
                    {
                        do_swap(p);
                    }
                }
            }

            do_rot(1);
        }

        // output phase.
        int cnt = 0;

        while (need < end && cnt < 2 * L)
        {
            do_rot(1);
            cnt++;
        }

        rounds++;
    }

    res.ok = (need == V && (int)res.ops.size() <= 100000);
    return res;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int inputN;
    cin >> inputN;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            cin >> a[i][j];
        }
    }

    vector<Candidate> cands;
    set<vector<pair<int, int>>> seen;

    for (int penalty = 0; penalty < 2; penalty++)
    {
        for (int t = 0; t < 8; t++)
        {
            for (int rev = 0; rev < 2; rev++)
            {
                auto oc = build_candidate(t, rev, penalty == 1);
                if (!oc)
                    continue;

                Candidate cand = *oc;

                vector<pair<int, int>> key;
                for (auto c : cand.rail)
                {
                    key.push_back({c.r, c.c});
                }

                if (seen.insert(key).second)
                {
                    cands.push_back(cand);
                }
            }
        }
    }

    Candidate bestCand;
    SimResult bestSim;
    bool have = false;

    for (const auto &cand : cands)
    {
        SimResult sim = simulate_candidate(cand);

        if (!sim.ok)
            continue;

        if (!have || sim.ops.size() < bestSim.ops.size())
        {
            have = true;
            bestCand = cand;
            bestSim = move(sim);
        }
    }

    // 念のための fallback。
    if (!have)
    {
        for (const auto &cand : cands)
        {
            SimResult sim = simulate_candidate(cand);

            if (!sim.ops.empty())
            {
                have = true;
                bestCand = cand;
                bestSim = move(sim);
                break;
            }
        }
    }

    int M = 1 + L;

    cout << M << '\n';

    // belt 0: main rail
    cout << L;
    for (auto c : bestCand.rail)
    {
        cout << ' ' << c.r << ' ' << c.c;
    }
    cout << '\n';

    // belts 1..200: pair swaps
    for (int p = 0; p < L; p++)
    {
        cout << 2
             << ' ' << bestCand.rail[p].r << ' ' << bestCand.rail[p].c
             << ' ' << bestCand.buf[p].r << ' ' << bestCand.buf[p].c
             << '\n';
    }

    cout << bestSim.ops.size() << '\n';

    for (auto [m, d] : bestSim.ops)
    {
        cout << m << ' ' << d << '\n';
    }

    return 0;
}