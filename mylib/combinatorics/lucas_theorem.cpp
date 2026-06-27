// Lucas の定理により C(n, r) mod p を求める。
// p は素数である必要がある。
// 構築: O(p) time / O(p) memory, C: O(log_p(n)) time
class LucasTheorem {
   private:
    long long mod;
    std::vector<long long> fac, ifac;

    static std::size_t table_size(long long prime) {
        assert(prime >= 2);
        return (std::size_t)prime;
    }

    long long mul(long long a, long long b) const {
        return (long long)((__int128)a * b % mod);
    }

    long long power(long long a, long long n) const {
        long long res = 1;
        while (n > 0) {
            if (n & 1) res = mul(res, a);
            a = mul(a, a);
            n >>= 1;
        }
        return res;
    }

    long long small_C(long long n, long long r) const {
        if (r < 0 || r > n) return 0;
        return mul(fac[n], mul(ifac[r], ifac[n - r]));
    }

   public:
    explicit LucasTheorem(long long prime)
        : mod(prime), fac(table_size(prime)), ifac(table_size(prime)) {
        fac[0] = 1;
        for (long long i = 1; i < mod; ++i) {
            fac[i] = mul(fac[i - 1], i);
        }
        ifac[mod - 1] = power(fac[mod - 1], mod - 2);
        for (long long i = mod - 1; i > 0; --i) {
            ifac[i - 1] = mul(ifac[i], i);
        }
    }

    long long C(long long n, long long r) const {
        if (n < 0 || r < 0 || r > n) return 0;

        long long res = 1;
        while (n > 0 || r > 0) {
            long long ni = n % mod;
            long long ri = r % mod;
            if (ri > ni) return 0;
            res = mul(res, small_C(ni, ri));
            n /= mod;
            r /= mod;
        }
        return res;
    }

    long long modulus() const { return mod; }
};
