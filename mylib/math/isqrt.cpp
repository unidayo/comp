ll isqrt(ll n) {
    if (n == 0) return 0;
    ll c = 64 - clz(n);
    c = (c - 1) >> 1;
    ll c2 = 64 - clz(c);
    ll a = 1;
    ll d = 0;
    for (ll s = c2 - 1; s >= 0; --s) {
        ll e = d;
        d = c >> s;
        a = (a << (d - e - 1)) + (n >> (2 * c - e - d + 1)) / a;
    }
    return a * a > n ? a - 1 : a;
}
