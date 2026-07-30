// 最終更新: 2026-07-12 15:18:35 JST
// 上限 limit 以下の整数を何度も素因数分解するための最小素因数表。
// 構築は線形篩で O(limit)、1回の分解は素因数の個数を k として O(k)。
struct factorization {
  public:
    explicit factorization(ll limit) {
        assert(1 <= limit && limit <= numeric_limits<int>::max());
        _limit = (int)limit;
        _smallest_prime.resize((size_t)_limit + 1);
        vector<int> primes;
        if (limit >= 2) primes.reserve((size_t)(limit / max(1.0L, log((long double)limit))));

        for (int value = 2; value <= _limit; ++value) {
            if (_smallest_prime[value] == 0) {
                _smallest_prime[value] = value;
                primes.push_back(value);
            }
            for (int prime : primes) {
                ll composite = (ll)value * prime;
                if (composite > _limit || prime > _smallest_prime[value]) break;
                _smallest_prime[(size_t)composite] = prime;
            }
        }
    }

    vector<ll> operator()(ll x) const {
        assert(1 <= x && x <= _limit);
        vector<ll> factors;
        factors.reserve(16);
        int value = (int)x;
        while (value > 1) {
            int prime = _smallest_prime[value];
            factors.push_back(prime);
            value /= prime;
        }
        return factors;
    }

  private:
    int _limit;
    vector<int> _smallest_prime;
};
