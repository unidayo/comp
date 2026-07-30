// 最終更新: 2026-07-28 22:20:09 JST
// 依存: mylib/algebra/number_theory/pollards_rho_factorization.cpp
// mod における a の乗法位数を返す。gcd(a, mod) != 1 なら -1 を返す。
// すなわち、a^k ≡ 1 (mod mod) を満たす最小の正整数 k を求める。
//
// mod は 2 以上の signed 64-bit 整数、a は signed 64-bit 整数とする。
// 素因数分解には Miller-Rabin 素数判定と Brent 型 Pollard's rho 法を使う。
// Pollard's rho の一般的な目安を用いれば期待計算量は O(mod^(1/4))。
// mod と φ(mod) が素因数分解済みなら、残りの処理は O(log^2(mod))。
ll multiplicative_order(ll a, ll mod) {
    assert(mod >= 2);

    a %= mod;
    if (a < 0) a += mod;
    if (gcd(a, mod) != 1) return -1;

    // mod の分解結果から φ(mod) = mod * Π(1 - 1/p) を計算する。
    vector<ull> modulus_factors = PollardsRho::factorization((ull)mod);
    ll phi = mod;
    ull previous = 0;
    for (ull prime : modulus_factors) {
        if (prime == previous) continue;
        phi -= phi / (ll)prime;
        previous = prime;
    }

    vector<ull> phi_factors = PollardsRho::factorization((ull)phi);
    auto power_mod = [mod](ll base, ll exponent) {
        ll result = 1;
        while (exponent > 0) {
            if (exponent & 1) result = (ll)((__int128_t)result * base % mod);
            base = (ll)((__int128_t)base * base % mod);
            exponent >>= 1;
        }
        return result;
    };

    // 位数は φ(mod) の約数。割れる素因数を一つずつ取り除く。
    ll order = phi;
    previous = 0;
    for (ull factor : phi_factors) {
        if (factor == previous) continue;
        ll prime = (ll)factor;
        while (order % prime == 0 && power_mod(a, order / prime) == 1) {
            order /= prime;
        }
        previous = factor;
    }
    return order;
}
