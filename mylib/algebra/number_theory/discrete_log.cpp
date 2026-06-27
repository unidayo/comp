ll isqrt(ll n) {
    if (n == 0) return 0;
    ll c = 64 - countl_zero((ull)n);
    c = (c - 1) >> 1;
    ll c2 = 64 - countl_zero(ull(c));
    ll a = 1;
    ll d = 0;
    for (ll s = c2 - 1; s >= 0; --s) {
        ll e = d;
        d = c >> s;
        a = (a << (d - e - 1)) + (n >> (2 * c - e - d + 1)) / a;
    }
    return a * a > n ? a - 1 : a;
}

ll discrete_log(ll a, ll b, const ll mod) {
    // Solves the discrete logarithm problem: find x such that a^x ≡ b (mod mod).
    // Returns x if a solution exists, otherwise returns -1.
    ll n = isqrt(mod) + 1;
    unordered_map<ll, ll> baby_step;
    ll e = 1;
    if ((e % mod) == b) return 0;
    for (ll i = 1; i < n + 1; ++i) {
        e = e * a % mod;
        if (e == b) return i;
        baby_step[b * e % mod] = i;
    }
    ll y = e;
    for (ll i = 2; i < n + 2; ++i) {
        e = e * y % mod;
        if (baby_step.contains(e)) {
            if (pow_mod32(a, i * n - baby_step[e], mod) == b) {
                return n * i - baby_step[e];
            } else {
                return -1;
            }
        }
    }
    return -1;
}