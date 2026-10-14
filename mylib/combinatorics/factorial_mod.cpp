// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/combinatorics/factorial_mod.cpp
long long factorial_mod(ll n, ll m) {
    ll ret = 1;
    for (int i = 1; i <= n; ++i) {
        ret *= i;
        ret %= m;
    }
    return ret;
}
