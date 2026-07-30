// 最終更新: 2026-07-12 14:27:44 JST
ll isqrt(ll n) {
    assert(n >= 0);
    if (n == 0) return 0;
    ll c = 64 - countl_zero((ull)n);
    c = (c - 1) >> 1;
    ll c2 = 64 - countl_zero((ull)c);
    ll a = 1;
    ll d = 0;
    for (ll s = c2 - 1; s >= 0; --s) {
        ll e = d;
        d = c >> s;
        a = (a << (d - e - 1)) + (n >> (2 * c - e - d + 1)) / a;
    }
    return a > n / a ? a - 1 : a;
}
