// 最終更新: 2026-07-12 15:31:02 JST
// 二項係数 C(n, r) を1件だけ O(min(r, n-r)) で計算する。

// 法を使わずに C(n, r) を返す。
// 組み込み整数型では、答えが型 T の範囲内に収まる必要がある。
// 任意精度整数型を T に指定すれば、大きな正確値も計算できる。
template <class T = long long>
T combination(ll n, ll r) {
    assert(n >= 0);
    if (r < 0 || n < r) return T{};
    r = min(r, n - r);

    T result = T{1};
    for (ll i = 1; i <= r; ++i) {
        ll numerator = n - r + i;
        if constexpr (is_integral_v<T> && is_signed_v<T>) {
            __int128_t next = (__int128_t)result * numerator / i;
            assert(next <= numeric_limits<T>::max());
            result = (T)next;
        } else if constexpr (is_integral_v<T> && is_unsigned_v<T>) {
            __uint128_t next = (__uint128_t)result * (ull)numerator / (ull)i;
            assert(next <= numeric_limits<T>::max());
            result = (T)next;
        } else {
            // cpp_intなど、乗算と正確な整数除算ができる型を想定する。
            result = result * T{numerator} / T{i};
        }
    }
    return result;
}

// 素数 mod における C(n, r) を返す。
// 階乗に mod の倍数を含まない 0 <= n < mod が前提。
ll combination_mod_prime(ll n, ll r, ll mod) {
    assert(mod >= 2);
    assert(0 <= n && n < mod);
    if (r < 0 || n < r) return 0;
    r = min(r, n - r);

    auto multiply_mod = [mod](ll a, ll b) { return (ll)((__int128_t)a * b % mod); };
    auto power_mod = [&](ll base, ll exponent) {
        ll result = 1 % mod;
        while (exponent > 0) {
            if (exponent & 1) result = multiply_mod(result, base);
            base = multiply_mod(base, base);
            exponent >>= 1;
        }
        return result;
    };

    ll numerator = 1;
    ll denominator = 1;
    for (ll i = 1; i <= r; ++i) {
        numerator = multiply_mod(numerator, n - r + i);
        denominator = multiply_mod(denominator, i);
    }
    return multiply_mod(numerator, power_mod(denominator, mod - 2));
}
