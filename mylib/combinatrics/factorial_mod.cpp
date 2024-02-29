long long factorial_mod(ll n, ll m) {
    ll ret = 1;
    for (int i = 1; i <= n; ++i) {
        ret *= i;
        ret %= m;
    }
    return ret;
}