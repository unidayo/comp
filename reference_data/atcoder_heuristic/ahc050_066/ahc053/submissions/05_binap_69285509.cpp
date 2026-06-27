#include <bits/stdc++.h>
using namespace std;

// ---- 3枚組の格納を軽量化（sum と 3 index の packed だけ） ----
struct TP {
    long long sum;   // A[a]+A[b]+A[c]
    uint32_t pack;   // a | (b<<10) | (c<<20)  (N<=500 → 10bit×3=30bitで十分)
};

static inline uint32_t pack3(int a, int b, int c){
    // 事前に a<b<c に整列してから渡す前提
    return (uint32_t)a | ((uint32_t)b<<10) | ((uint32_t)c<<20);
}

static inline void unpack3(uint32_t p, int& a, int& b, int& c){
    a =  (int)( p        & 1023u);
    b =  (int)((p >> 10) & 1023u);
    c =  (int)((p >> 20) & 1023u);
}

static inline long long llabs_fast(long long x){
    return x >= 0 ? x : -x;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    long long L, U;
    if (!(cin >> N >> M >> L >> U)) return 0;

    // ---- 1) A_i を [L/6*0.99, U/6*1.01] の整数一様乱数で生成して出力 ----
    mt19937_64 rng((uint64_t)chrono::high_resolution_clock::now().time_since_epoch().count());

    long double lowd  = ((long double)L / 6.0L) * 0.99L;
    long double highd = ((long double)U / 6.0L) * 1.01L;
    long long lowA  = (long long)floor(lowd);
    long long highA = (long long)ceil(highd);
    // [1, U] にクリップ
    lowA  = max(1LL, lowA);
    highA = min(U, highA);
    if (lowA > highA) lowA = highA;

    uniform_int_distribution<long long> distA(lowA, highA);

    vector<long long> A(N);
    for (int i = 0; i < N; ++i) A[i] = distA(rng);

    for (int i = 0; i < N; ++i) {
        if (i) cout << ' ';
        cout << A[i];
    }
    cout << '\n' << flush;  // 対話型前提で flush

    // ---- 2) B を受け取る ----
    vector<long long> B(M);
    for (int j = 0; j < M; ++j) cin >> B[j];

    // ---- 3) ランダム3枚組を 1e6 回サンプリング → (sum, pack) を配列化・ソート ----
    const int SAMPLES = 5000000; // 1e6
    vector<TP> triples;
    triples.reserve((N >= 3) ? SAMPLES : 0);

    if (N >= 3) {
        uniform_int_distribution<int> distIdx(0, N-1);
        triples.clear();
        triples.resize(SAMPLES);

        for (int s = 0; s < SAMPLES; ++s) {
            int a = distIdx(rng);
            int b = distIdx(rng); while (b == a) b = distIdx(rng);
            int c = distIdx(rng); while (c == a || c == b) c = distIdx(rng);

            // a<b<c に整列
            if (a > b) swap(a, b);
            if (b > c) swap(b, c);
            if (a > b) swap(a, b);

            long long sum = A[a] + A[b] + A[c];
            triples[s] = TP{sum, pack3(a,b,c)};
        }
        sort(triples.begin(), triples.end(),
             [](const TP& x, const TP& y){ return x.sum < y.sum; });
    }

    // ---- 4) 逆引き: カード -> 含まれる3枚組index（ソート後の index） ----
    vector<vector<int>> incident(N);
    if (!triples.empty()) {
        for (int t = 0; t < (int)triples.size(); ++t) {
            int a,b,c; unpack3(triples[t].pack, a,b,c);
            incident[a].push_back(t);
            incident[b].push_back(t);
            incident[c].push_back(t);
        }
    }

    // invalid: 3枚組の有効フラグ（0=有効,1=無効）
    vector<uint8_t> invalid(triples.size(), 0);
    // used: カードの使用済み（0/1）
    vector<uint8_t> used(N, 0);
    // 出力割当: 0=捨てる, 1..M=山
    vector<int> X(N, 0);

    auto invalidate_card = [&](int v){
        if (used[v]) return;
        used[v] = 1;
        for (int t : incident[v]) invalid[t] = 1;
    };

    // ---- 5) メイン: 各 B_j について 2-pointer で「2つの3枚組=6枚」探索 ----
    const long long INF = (long long)4e18;

    for (int j = 0; j < M; ++j) {
        if (triples.empty()) break; // N<3 などの保険

        long long T = B[j];

        int l = 0, r = (int)triples.size() - 1;

        // 有効端に寄せる
        while (l < r && invalid[l]) ++l;
        while (l < r && invalid[r]) --r;

        long long bestDiff = INF;
        int bestL = -1, bestR = -1;

        while (l < r) {
            // 現在ペアの合計
            long long cur = triples[l].sum + triples[r].sum;
            long long d = llabs_fast(cur - T);

            // 改善見込みがある時だけ「重い判定」（unpack + used/重複チェック）
            if (d < bestDiff) {
                int a1,b1,c1, a2,b2,c2;
                unpack3(triples[l].pack, a1,b1,c1);
                unpack3(triples[r].pack, a2,b2,c2);

                // 6枚が未使用 & 相互に重複なし
                if (!used[a1] && !used[b1] && !used[c1] &&
                    !used[a2] && !used[b2] && !used[c2] &&
                    a1!=a2 && a1!=b2 && a1!=c2 &&
                    b1!=a2 && b1!=b2 && b1!=c2 &&
                    c1!=a2 && c1!=b2 && c1!=c2) {
                    bestDiff = d; bestL = l; bestR = r;
                    if (bestDiff == 0) break; // ピタリなら即決
                }
            }

            // 2-pointer 本体 + invalid をまとめてスキップ
            if (cur < T) {
                do { ++l; } while (l < r && invalid[l]);
            } else {
                do { --r; } while (l < r && invalid[r]);
            }
        }

        // ベストが見つかれば 6枚を採用して、関係する3枚組を一括無効化
        if (bestL != -1) {
            int a1,b1,c1, a2,b2,c2;
            unpack3(triples[bestL].pack, a1,b1,c1);
            unpack3(triples[bestR].pack, a2,b2,c2);

            // 念のため最終確認
            if (!used[a1] && !used[b1] && !used[c1] &&
                !used[a2] && !used[b2] && !used[c2] &&
                a1!=a2 && a1!=b2 && a1!=c2 &&
                b1!=a2 && b1!=b2 && b1!=c2 &&
                c1!=a2 && c1!=b2 && c1!=c2) {

                // 山 j(1-index) に割当
                X[a1]=X[b1]=X[c1]=j+1;
                X[a2]=X[b2]=X[c2]=j+1;

                // 使用確定 → 逆引きで関係3枚組を一括無効化
                invalidate_card(a1); invalidate_card(b1); invalidate_card(c1);
                invalidate_card(a2); invalidate_card(b2); invalidate_card(c2);
            }
        }
        // 見つからなければこの山は空のまま（0）
    }

    // ---- 6) 割当を出力（残りは 0=捨てる）----
    for (int i = 0; i < N; ++i) {
        if (i) cout << ' ';
        cout << X[i];
    }
    cout << '\n' << flush;

    return 0;
}
