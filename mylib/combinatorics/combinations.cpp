long long comb_mod(ll n, ll r, ll m) {
    if (n < r) { return 0; }
    ll res = 1;
    ll imul = 1;
    r = min(n - r, r);
    for (ll i = 0; i < r; ++i) {
        res *= n - i;
        imul *= r - i;
        res %= m;
        imul %= m;
    }
    ll inv = pow_mod(imul, m - 2, m);
    res = (res * inv) % m;
    return res;
}