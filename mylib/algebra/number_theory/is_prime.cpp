// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algebra/number_theory/is_prime.cpp
// signed 64-bit整数 n が素数かを返す。
// 小さい値は30-wheel試し割り、大きい値は決定的Miller-Rabinで判定する。
bool is_prime(ll n) {
    if (n < 2) return false;

    constexpr ll small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (ll p : small_primes) {
        if (n == p) return true;
        if (n % p == 0) return false;
    }

    // 小さい値では、__int128の剰余を繰り返すMiller-Rabinより試し割りが速い。
    constexpr ll trial_division_limit = 4'000'000LL;
    if (n < trial_division_limit) {
        constexpr int wheel_steps[8] = {4, 2, 4, 2, 4, 6, 2, 6};
        ll candidate = 41;
        int wheel_index = 1;  // 41 の次は 43（差分2）
        while (candidate * candidate <= n) {
            if (n % candidate == 0) return false;
            candidate += wheel_steps[wheel_index];
            wheel_index = (wheel_index + 1) & 7;
        }
        return true;
    }

    auto multiply_mod = [n](ull a, ull b) -> ull { return (ull)((__uint128_t)a * b % (ull)n); };
    auto power_mod = [&](ull base, ull exponent) {
        ull result = 1;
        while (exponent > 0) {
            if (exponent & 1) result = multiply_mod(result, base);
            base = multiply_mod(base, base);
            exponent >>= 1;
        }
        return result;
    };

    ull d = (ull)n - 1;
    int s = countr_zero(d);
    d >>= s;

    // この7底で unsigned 64-bit 全域に対して決定的。
    constexpr ull bases[] = {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL};
    for (ull base : bases) {
        base %= (ull)n;
        if (base == 0) continue;
        ull x = power_mod(base, d);
        if (x == 1 || x == (ull)n - 1) continue;

        bool witness = true;
        for (int r = 1; r < s; ++r) {
            x = multiply_mod(x, x);
            if (x == (ull)n - 1) {
                witness = false;
                break;
            }
        }
        if (witness) return false;
    }
    return true;
}
