struct factorization {
public:
    explicit factorization(ll n) {
        _sieve = vector<ll>(n + 1);
        iota(_sieve.begin(), _sieve.end(), 0);
        for (ll i = 2; i <= n; i += 2) _sieve[i] = 2;
        for (ll i = 3; i <= n; ++i) {
            if (_sieve[i] == i) {
                for (ll j = i * i; j <= n; j += i) {
                    if (_sieve[j] == j) _sieve[j] = i;
                }
            }
        }
    }

    vector<ll> operator()(ll x) {
        vector<ll> factors;
        while (x > 1) {
            ll k = _sieve[x];
            factors.push_back(k);
            x /= k;
        }
        return factors;
    }

private:
    vector<ll> _sieve;
};