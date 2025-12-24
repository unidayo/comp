/* Pollard's Rho法による素因数分解 O(n^1/4)*/
class PollardsRho {
   public:
    static vector<ull> factorization(ull n) {
        vector<ull> factors;
        if (n <= 1) return factors;

        while (n % 2 == 0) {
            factors.push_back(2);
            n >>= 1;
        }

        ull i = 3;
        while (i * i <= n && i < 1000) {
            while (n % i == 0) {
                factors.push_back(i);
                n /= i;
            }
            i += 2;
        }
        while (n > 1) {
            if (miller_rabin(n)) {
                factors.push_back(n);
                n = 1;
            } else {
                ull g = pollards_rho(n);
                while (n % g == 0) {
                    factors.push_back(g);
                    n /= g;
                }
            }
        }

        if (n > 1) factors.push_back(n);
        ranges::sort(factors);
        return factors;
    }

   private:
    static bool miller_rabin(ull n) {
        if (n <= 1) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;

        vector<ull> A = {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL};

        ull d = n - 1;
        int s = 0;
        while (d % 2 == 0) {
            s++;
            d >>= 1;
        }

        for (ull a : A) {
            a %= n;
            if (a == 0) continue;
            ull x = pow_mod64(a, d, n);
            if (x == 1) continue;
            for (int i = 1; x != n - 1; ++i) {
                if (i >= s) return false;
                x = (uvll)x * x % n;
            }
        }
        return true;
    }

    static ull mod_mul(ull a, ull b, ull mod) { return (uvll)a * b % mod; }

    inline static ull f(ull x, ull c, ull mod) { return (mod_mul(x, x, mod) + c) % mod; }

    static ull pollards_rho(ull n) {
        static mt19937_64 rng(random_device{}());
        if (n % 2 == 0) return 2;
        ull m = sqrt(sqrt(sqrt(n)));

        while (true) {
            ull c = uniform_int_distribution<ull>(1, n - 2)(rng);
            ull y = 2, r = 1, q = 1, g = 1, x, ys;

            while (g == 1) {
                x = y;
                for (ull i = 0; i < r; i++) y = f(y, c, n);

                ull k = 0;
                while (k < r && g == 1) {
                    ys = y;
                    ull lim = min(m, r - k);
                    for (ull i = 0; i < lim; i++) {
                        y = f(y, c, n);
                        q = mod_mul(q, (x > y ? x - y : y - x), n);
                    }
                    g = gcd(q, n);
                    k += m;
                }
                r <<= 1;
            }

            if (g == n) {
                g = 1;
                while (g == 1) {
                    ys = f(ys, c, n);
                    g = gcd((x > ys ? x - ys : ys - x), n);
                }
            }

            if (g != n) {
                if (miller_rabin(g)) return g;
                if (miller_rabin(n / g)) return n / g;
                return pollards_rho(g);
            }
        }
    }
};