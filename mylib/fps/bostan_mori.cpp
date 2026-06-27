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
        auto QQm = convolution(Q, Qm);
        int qs2 = ssize(Q);
        for (int i = 0; i < qs2; ++i) {
            Q[i] = QQm[2 * i];
        }
        N >>= 1;
    }
    return P[0] / Q[0];
}
