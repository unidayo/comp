// 最終更新: 2026-10-14 18:14:44 JST
// 依存: mylib/acl/convolution.cpp
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/fps/bostan_mori.cpp
/* Bostan–Mori 法
 与えられた生成関数
 F(x) = P(x) / Q(x)
 の x^n の係数を O(M(d) log n) で求める */
template <typename Tp>
Tp BostanMori(vector<Tp> &P, vector<Tp> &Q, ll N) {
    assert(N >= 0);
    while (ssize(Q) && Q.back() == 0) Q.pop_back();
    assert(ssize(Q) > 0);
    int h = 0;
    while (Q[h] == 0) h++;
    N += h;
    Q.erase(Q.begin(), Q.begin() + h);
    if (ssize(Q) == 1) return N < ssize(P) ? P[N] / Q[0] : 0;
    while (N) {
        vector<Tp> Qm = Q;
        int qs = ssize(Qm);
        for (int i = 1; i < qs; i += 2) {
            Qm[i] = -Qm[i];
        }
        auto PQm = convolution(P, Qm);
        P.resize((ssize(PQm) + 1 - (N & 1)) / 2);
        int ps = ssize(P);
        for (int i = 0; i < ps; ++i) {
            P[i] = PQm[i * 2 + (N & 1)];
        }
        if (qs < 256) {
            auto product = convolution(Q, Qm);
            for (int i = 0; i < qs; ++i) Q[i] = product[2 * i];
        } else {
            // Q(x)Q(-x) = E(x^2)^2 - x^2 O(x^2)^2。
            // 半分の長さの畳み込み2回に分け、大次数のNTTを軽くする。
            vector<Tp> even((qs + 1) / 2), odd(qs / 2);
            for (int i = 0; i < qs; ++i) {
                if (i & 1) odd[i / 2] = Q[i];
                else even[i / 2] = Q[i];
            }
            auto even_squared = convolution(even, even);
            auto odd_squared = convolution(odd, odd);
            for (int i = 0; i < qs; ++i) {
                Q[i] = i < ssize(even_squared) ? even_squared[i] : Tp(0);
                if (i != 0 && i - 1 < ssize(odd_squared)) Q[i] -= odd_squared[i - 1];
            }
        }
        N >>= 1;
    }
    return P[0] / Q[0];
}
