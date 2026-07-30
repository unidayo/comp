// 最終更新: 2026-07-28 19:00:12 JST
// 依存: mylib/acl/fenwicktree.cpp
// P の辞書順順位（1-indexed）を返す
// mod == 0 のときは mod なし
ll perm_lexical_order(const vector<ll>& P, ll mod = 0) {
    int N = (int)P.size();

    // factorial
    vector<ll> fact(N + 1);
    fact[0] = 1;
    if (mod == 0) {
        for (int i = 1; i <= N; i++) fact[i] = fact[i - 1] * i;
    } else {
        for (int i = 1; i <= N; i++) fact[i] = fact[i - 1] * i % mod;
    }

    // 座標圧縮
    vector<ll> xs = P;
    sort(xs.begin(), xs.end());
    xs.erase(unique(xs.begin(), xs.end()), xs.end());

    int M = xs.size();
    unordered_map<ll, int> D;
    for (int i = 0; i < M; i++) D[xs[i]] = i;

    // 出現回数
    vector<ll> C(M, 0);
    for (ll x : P) C[D[x]]++;

    // Fenwick Tree
    fenwick_tree<ll> fw(M);
    for (int i = 0; i < M; i++) fw.add(i, C[i]);

    ll order = 0;
    for (int i = 0; i < N; i++) {
        int cx = D[P[i]];
        ll cnt = fw.sum(0, cx);  // cx 未満の残数
        ll add = cnt * fact[N - i - 1];
        if (mod != 0) add %= mod;
        order += add;
        if (mod != 0) order %= mod;
        fw.add(cx, -1);
    }

    return order + 1;
}
