// 最終更新: 2026-07-28 22:02:44 JST
// 依存: mylib/algebra/number_theory/pollards_rho_factorization.cpp
// 数論的メビウス関数 μ(n) を計算する。
// μ(1) = 1。n が平方因子を持てば 0、異なる k 個の素数の積なら (-1)^k。

// 1個の正の整数 n に対する μ(n) を返す。
// 素因数分解には Miller-Rabin 素数判定と Brent 型 Pollard's rho 法を使う。
int mobius(ll n) {
    assert(n > 0);

    vector<ull> factors = PollardsRho::factorization((ull)n);
    if (adjacent_find(factors.begin(), factors.end()) != factors.end()) return 0;
    return factors.size() % 2 == 0 ? 1 : -1;
}

// 0 以上 limit 以下の μ(i) を返す。返却値の 0 番目は 0、1 番目は 1。
// 線形篩により計算量 O(limit)、追加領域 O(limit)。
vector<int> mobius_table(ll limit) {
    assert(0 <= limit && limit <= numeric_limits<int>::max());

    int size = (int)limit;
    vector<int> mu((size_t)size + 1);
    if (size == 0) return mu;

    mu[1] = 1;
    vector<bool> composite((size_t)size + 1);
    vector<int> primes;
    if (size >= 2) {
        primes.reserve((size_t)(size / max(1.0L, log((long double)size))));
    }

    for (int value = 2; value <= size; ++value) {
        if (!composite[value]) {
            mu[value] = -1;
            primes.push_back(value);
        }
        for (int prime : primes) {
            ll next = (ll)value * prime;
            if (next > size) break;
            composite[(size_t)next] = true;
            if (value % prime == 0) {
                mu[(size_t)next] = 0;
                break;
            }
            mu[(size_t)next] = -mu[value];
        }
    }
    return mu;
}
