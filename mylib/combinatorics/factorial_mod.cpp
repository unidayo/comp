// 最終更新: 2026-07-12 14:27:44 JST
long long factorial_mod(ll n, ll m) {
    ll ret = 1;
    for (int i = 1; i <= n; ++i) {
        ret *= i;
        ret %= m;
    }
    return ret;
}
