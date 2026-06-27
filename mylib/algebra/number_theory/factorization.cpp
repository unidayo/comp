vector<ll> factorization(ll n) {
    if (n == 1) return {};
    if (n <= 3) return {n};
    vector<ll> factors;
    for (; n % 2 == 0; n >>= 1) {
        factors.push_back(2);
    }
    for (ll i = 3; i * i <= n; i += 2) {
        while (n % i == 0) {
            factors.push_back(i);
            n /= i;
        }
    }
    if (n > 1) factors.push_back(n);
    return factors;
}
