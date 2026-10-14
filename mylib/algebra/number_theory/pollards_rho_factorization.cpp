// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algebra/number_theory/pollards_rho_factorization.cpp
// Brent型Pollard's Rho法による unsigned 64-bit整数の素因数分解。
// 返す素因数は重複を含む昇順。
class PollardsRho {
  public:
    static vector<ull> factorization(ull n) {
        vector<ull> factors;
        if (n <= 1) return factors;

        // Rho法へ渡す前に、頻出する小さい因子を低い固定費で取り除く。
        constexpr ull small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
        for (ull prime : small_primes) {
            while (n % prime == 0) {
                factors.push_back(prime);
                n /= prime;
            }
        }

        ull random_state = splitmix64(n ^ 0x243f6a8885a308d3ULL);
        factor_rec(n, factors, random_state);
        ranges::sort(factors);
        return factors;
    }

  private:
    static ull multiply_mod(ull a, ull b, ull mod) { return (ull)((__uint128_t)a * b % mod); }

    static ull power_mod(ull base, ull exponent, ull mod) {
        ull result = 1;
        while (exponent > 0) {
            if (exponent & 1) result = multiply_mod(result, base, mod);
            base = multiply_mod(base, base, mod);
            exponent >>= 1;
        }
        return result;
    }

    static bool miller_rabin(ull n) {
        if (n < 2) return false;
        constexpr ull small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
        for (ull prime : small_primes) {
            if (n == prime) return true;
            if (n % prime == 0) return false;
        }

        ull d = n - 1;
        int s = countr_zero(d);
        d >>= s;
        constexpr ull bases[] = {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL};
        for (ull base : bases) {
            base %= n;
            if (base == 0) continue;
            ull x = power_mod(base, d, n);
            if (x == 1 || x == n - 1) continue;

            bool composite = true;
            for (int r = 1; r < s; ++r) {
                x = multiply_mod(x, x, n);
                if (x == n - 1) {
                    composite = false;
                    break;
                }
            }
            if (composite) return false;
        }
        return true;
    }

    static ull splitmix64(ull x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    static ull next_random(ull &state) {
        state = splitmix64(state);
        return state;
    }

    static ull iterate(ull x, ull c, ull mod) {
        return (ull)(((__uint128_t)x * x + c) % mod);
    }

    static ull pollards_rho(ull n, ull &random_state) {
        if (n % 2 == 0) return 2;
        if (n % 3 == 0) return 3;
        constexpr ull batch_size = 128;

        while (true) {
            ull y = next_random(random_state) % (n - 2) + 2;
            ull c = next_random(random_state) % (n - 1) + 1;
            ull x = 0, saved_y = 0;
            ull radius = 1;
            ull divisor = 1;

            while (divisor == 1) {
                x = y;
                for (ull i = 0; i < radius; ++i) y = iterate(y, c, n);

                for (ull offset = 0; offset < radius && divisor == 1; offset += batch_size) {
                    saved_y = y;
                    ull product = 1;
                    ull count = min(batch_size, radius - offset);
                    for (ull i = 0; i < count; ++i) {
                        y = iterate(y, c, n);
                        ull difference = x > y ? x - y : y - x;
                        product = multiply_mod(product, difference, n);
                    }
                    divisor = gcd(product, n);
                }
                radius <<= 1;
            }

            if (divisor == n) {
                do {
                    saved_y = iterate(saved_y, c, n);
                    ull difference = x > saved_y ? x - saved_y : saved_y - x;
                    divisor = gcd(difference, n);
                } while (divisor == 1);
            }
            if (divisor != n) return divisor;
        }
    }

    static void factor_rec(ull n, vector<ull> &factors, ull &random_state) {
        if (n == 1) return;
        if (miller_rabin(n)) {
            factors.push_back(n);
            return;
        }
        ull divisor = pollards_rho(n, random_state);
        factor_rec(divisor, factors, random_state);
        factor_rec(n / divisor, factors, random_state);
    }
};
