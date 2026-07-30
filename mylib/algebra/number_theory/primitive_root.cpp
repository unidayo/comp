// 最終更新: 2026-07-28 19:00:12 JST
// 依存: mylib/algebra/number_theory/pollards_rho_factorization.cpp
// mod を法とする最小の正の原始根を返す。存在しない場合は -1 を返す。
// 原始根が存在するのは mod = 2, 4, p^k, 2*p^k（p は奇素数）の場合に限る。
//
// mod は 2 以上の signed 64-bit 整数とする。
// 素因数分解には Miller-Rabin 素数判定と Brent 型 Pollard's rho 法を使う。
// 候補 g 1個の判定は、φ(mod) の異なる素因数の個数を k として
// O(k log(mod))。最小原始根を r とすると、候補探索全体は O(r k log(mod))。
ll primitive_root(ll mod) {
    assert(mod >= 2);
    if (mod == 2) return 1;
    if (mod == 4) return 3;

    auto distinct_prime_factors = [](ll value) {
        vector<ull> factors = PollardsRho::factorization((ull)value);
        factors.erase(unique(factors.begin(), factors.end()), factors.end());
        return factors;
    };

    // 存在条件を確認し、mod の唯一の奇素因数 p を得る。
    ll odd_part = mod;
    if (odd_part % 2 == 0) odd_part /= 2;
    if (odd_part % 2 == 0) return -1;

    vector<ull> modulus_factors = distinct_prime_factors(odd_part);
    if (modulus_factors.size() != 1) return -1;
    ll odd_prime = (ll)modulus_factors[0];

    ll phi = odd_part / odd_prime * (odd_prime - 1);
    vector<ull> phi_factors = distinct_prime_factors(phi);

    auto power_mod = [mod](ll base, ll exponent) {
        ll result = 1;
        while (exponent > 0) {
            if (exponent & 1) result = (ll)((__int128_t)result * base % mod);
            base = (ll)((__int128_t)base * base % mod);
            exponent >>= 1;
        }
        return result;
    };

    // 乗法群が巡回群なので、全ての q | φ(mod) に対して
    // g^(φ(mod)/q) != 1 なら g の位数は φ(mod)。
    for (ll candidate = 2; candidate < mod; ++candidate) {
        if (gcd(candidate, mod) != 1) continue;
        bool is_primitive = true;
        for (ull prime : phi_factors) {
            if (power_mod(candidate, phi / (ll)prime) == 1) {
                is_primitive = false;
                break;
            }
        }
        if (is_primitive) return candidate;
    }
    return -1;
}
