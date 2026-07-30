// 最終更新: 2026-07-28 22:02:44 JST
// 依存: mylib/algebra/number_theory/pollards_rho_factorization.cpp
// オイラーのトーシェント関数 φ(n) を計算する。
// φ(n) は 1 以上 n 以下で n と互いに素な整数の個数。特に φ(1) = 1。

// 1個の正の整数 n に対する φ(n) を返す。
// 素因数 p ごとに φ(n) = n * Π(1 - 1/p) を適用する。
// 素因数分解には Miller-Rabin 素数判定と Brent 型 Pollard's rho 法を使う。
ll euler_phi(ll n) {
    assert(n > 0);

    vector<ull> factors = PollardsRho::factorization((ull)n);
    ll result = n;
    ull previous = 0;
    for (ull prime : factors) {
        if (prime == previous) continue;
        result -= result / (ll)prime;
        previous = prime;
    }
    return result;
}

// 0 以上 limit 以下の φ(i) を返す。返却値の 0 番目は 0、1 番目は 1。
// 線形篩により計算量 O(limit)、追加領域 O(limit)。
vector<ll> euler_phi_table(ll limit) {
    assert(0 <= limit && limit <= numeric_limits<int>::max());

    int size = (int)limit;
    vector<ll> phi((size_t)size + 1);
    if (size == 0) return phi;

    phi[1] = 1;
    vector<int> primes;
    if (size >= 2) {
        primes.reserve((size_t)(size / max(1.0L, log((long double)size))));
    }

    for (int value = 2; value <= size; ++value) {
        if (phi[value] == 0) {
            phi[value] = value - 1;
            primes.push_back(value);
        }
        for (int prime : primes) {
            ll composite = (ll)value * prime;
            if (composite > size) break;
            if (value % prime == 0) {
                phi[(size_t)composite] = phi[value] * prime;
                break;
            }
            phi[(size_t)composite] = phi[value] * (prime - 1);
        }
    }
    return phi;
}
