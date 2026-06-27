#line 1 "src/A/main.cpp"
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
//#pragma GCC optimize("Ofast")
//#undef LOCAL

#include <valarray>

#line 2 "/home/vscode/yosupo-library/src/yosupo/fastio.hpp"

#include <unistd.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cstring>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include <bit>
#include <cstdint>

#line 2 "/home/vscode/yosupo-library/src/yosupo/internal_type_traits.hpp"

#line 4 "/home/vscode/yosupo-library/src/yosupo/internal_type_traits.hpp"
#include <numeric>
#line 6 "/home/vscode/yosupo-library/src/yosupo/internal_type_traits.hpp"

namespace yosupo {

namespace internal {

template <class T>
using is_signed_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value ||
                                  std::is_same<T, __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int128 =
    typename std::conditional<std::is_same<T, __uint128_t>::value ||
                                  std::is_same<T, unsigned __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using make_unsigned_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value,
                              __uint128_t,
                              unsigned __int128>;

template <class T>
using is_integral =
    typename std::conditional<std::is_integral<T>::value ||
                                  internal::is_signed_int128<T>::value ||
                                  internal::is_unsigned_int128<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_signed_int = typename std::conditional<(is_integral<T>::value &&
                                                 std::is_signed<T>::value) ||
                                                    is_signed_int128<T>::value,
                                                std::true_type,
                                                std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<(is_integral<T>::value &&
                               std::is_unsigned<T>::value) ||
                                  is_unsigned_int128<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<
    is_signed_int128<T>::value,
    make_unsigned_int128<T>,
    typename std::conditional<std::is_signed<T>::value,
                              std::make_unsigned<T>,
                              std::common_type<T>>::type>::type;

template <class T>
using is_integral_t = std::enable_if_t<is_integral<T>::value>;

template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;

template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;

template <class T> using to_unsigned_t = typename to_unsigned<T>::type;

}  // namespace internal

}  // namespace yosupo
#line 17 "/home/vscode/yosupo-library/src/yosupo/fastio.hpp"

namespace yosupo {

struct Scanner {
  public:
    Scanner(const Scanner&) = delete;
    Scanner& operator=(const Scanner&) = delete;

    Scanner(FILE* fp) : fd(fileno(fp)) { line[0] = 127; }

    void read() {}
    template <class H, class... T> void read(H& h, T&... t) {
        bool f = read_single(h);
        assert(f);
        read(t...);
    }

    int read_unsafe() { return 0; }
    template <class H, class... T> int read_unsafe(H& h, T&... t) {
        bool f = read_single(h);
        if (!f) return 0;
        return 1 + read_unsafe(t...);
    }

    int close() { return ::close(fd); }

  private:
    static constexpr int SIZE = 1 << 15;

    int fd = -1;
    std::array<char, SIZE + 1> line;
    int st = 0, ed = 0;
    bool eof = false;

    bool read_single(std::string& ref) {
        if (!skip_space()) return false;
        ref = "";
        while (true) {
            char c = top();
            if (c <= ' ') break;
            ref += c;
            st++;
        }
        return true;
    }
    bool read_single(double& ref) {
        std::string s;
        if (!read_single(s)) return false;
        ref = std::stod(s);
        return true;
    }

    template <class T,
              std::enable_if_t<std::is_same<T, char>::value>* = nullptr>
    bool read_single(T& ref) {
        if (!skip_space<50>()) return false;
        ref = top();
        st++;
        return true;
    }

    template <class T,
              internal::is_signed_int_t<T>* = nullptr,
              std::enable_if_t<!std::is_same<T, char>::value>* = nullptr>
    bool read_single(T& sref) {
        using U = internal::to_unsigned_t<T>;
        if (!skip_space<50>()) return false;
        bool neg = false;
        if (line[st] == '-') {
            neg = true;
            st++;
        }
        U ref = 0;
        do {
            ref = 10 * ref + (line[st++] & 0x0f);
        } while (line[st] >= '0');
        sref = neg ? -ref : ref;
        return true;
    }
    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<!std::is_same<U, char>::value>* = nullptr>
    bool read_single(U& ref) {
        if (!skip_space<50>()) return false;
        ref = 0;
        do {
            ref = 10 * ref + (line[st++] & 0x0f);
        } while (line[st] >= '0');
        return true;
    }

    bool reread() {
        if (ed - st >= 50) return true;
        if (st > SIZE / 2) {
            std::memmove(line.data(), line.data() + st, ed - st);
            ed -= st;
            st = 0;
        }
        if (eof) return false;
        auto u = ::read(fd, line.data() + ed, SIZE - ed);
        if (u == 0) {
            eof = true;
            line[ed] = '\0';
            u = 1;
        }
        ed += int(u);
        line[ed] = char(127);
        return true;
    }

    char top() {
        if (st == ed) {
            bool f = reread();
            assert(f);
        }
        return line[st];
    }

    template <int TOKEN_LEN = 0> bool skip_space() {
        while (true) {
            while (line[st] <= ' ') st++;
            if (ed - st > TOKEN_LEN) return true;
            if (st > ed) st = ed;
            for (auto i = st; i < ed; i++) {
                if (line[i] <= ' ') return true;
            }
            if (!reread()) return false;
        }
    }
};

struct Printer {
  public:
    template <char sep = ' ', bool F = false> void write() {}
    template <char sep = ' ', bool F = false, class H, class... T>
    void write(const H& h, const T&... t) {
        if (F) write_single(sep);
        write_single(h);
        write<true>(t...);
    }
    template <char sep = ' ', class... T> void writeln(const T&... t) {
        write<sep>(t...);
        write_single('\n');
    }

    Printer(FILE* _fp) : fd(fileno(_fp)) {}
    ~Printer() { flush(); }

    int close() {
        flush();
        return ::close(fd);
    }

    void flush() {
        if (pos) {
            auto res = ::write(fd, line.data(), pos);
            assert(res != -1);
            pos = 0;
        }
    }

  private:
    static std::array<std::array<char, 2>, 100> small;
    static std::array<unsigned long long, 20> tens;

    static constexpr size_t SIZE = 1 << 15;
    int fd;
    std::array<char, SIZE> line;
    size_t pos = 0;
    std::stringstream ss;

    template <class T,
              std::enable_if_t<std::is_same<char, T>::value>* = nullptr>
    void write_single(const T& val) {
        if (pos == SIZE) flush();
        line[pos++] = val;
    }

    template <class T,
              internal::is_signed_int_t<T>* = nullptr,
              std::enable_if_t<!std::is_same<char, T>::value>* = nullptr>
    void write_single(const T& val) {
        using U = internal::to_unsigned_t<T>;
        if (val == 0) {
            write_single('0');
            return;
        }
        if (pos > SIZE - 50) flush();
        U uval = val;
        if (val < 0) {
            write_single('-');
            uval = -uval;
        }
        write_unsigned(uval);
    }

    template <class U, internal::is_unsigned_int_t<U>* = nullptr>
    void write_single(U uval) {
        if (uval == 0) {
            write_single('0');
            return;
        }
        if (pos > SIZE - 50) flush();

        write_unsigned(uval);
    }

    static int calc_len(uint64_t x) {
        int i = ((63 - std::countl_zero(x)) * 3 + 3) / 10;
        if (x < tens[i])
            return i;
        else
            return i + 1;
    }

    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<2 >= sizeof(U)>* = nullptr>
    void write_unsigned(U uval) {
        size_t len = calc_len(uval);
        pos += len;

        char* ptr = line.data() + pos;
        while (uval >= 100) {
            ptr -= 2;
            memcpy(ptr, small[uval % 100].data(), 2);
            uval /= 100;
        }
        if (uval >= 10) {
            memcpy(ptr - 2, small[uval].data(), 2);
        } else {
            *(ptr - 1) = char('0' + uval);
        }
    }

    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<4 == sizeof(U)>* = nullptr>
    void write_unsigned(U uval) {
        std::array<char, 8> buf;
        memcpy(buf.data() + 6, small[uval % 100].data(), 2);
        memcpy(buf.data() + 4, small[uval / 100 % 100].data(), 2);
        memcpy(buf.data() + 2, small[uval / 10000 % 100].data(), 2);
        memcpy(buf.data() + 0, small[uval / 1000000 % 100].data(), 2);

        if (uval >= 100000000) {
            if (uval >= 1000000000) {
                memcpy(line.data() + pos, small[uval / 100000000 % 100].data(),
                       2);
                pos += 2;
            } else {
                line[pos] = char('0' + uval / 100000000);
                pos++;
            }
            memcpy(line.data() + pos, buf.data(), 8);
            pos += 8;
        } else {
            size_t len = calc_len(uval);
            memcpy(line.data() + pos, buf.data() + (8 - len), len);
            pos += len;
        }
    }

    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<8 == sizeof(U)>* = nullptr>
    void write_unsigned(U uval) {
        size_t len = calc_len(uval);
        pos += len;

        char* ptr = line.data() + pos;
        while (uval >= 100) {
            ptr -= 2;
            memcpy(ptr, small[uval % 100].data(), 2);
            uval /= 100;
        }
        if (uval >= 10) {
            memcpy(ptr - 2, small[uval].data(), 2);
        } else {
            *(ptr - 1) = char('0' + uval);
        }
    }

    template <
        class U,
        std::enable_if_t<internal::is_unsigned_int128<U>::value>* = nullptr>
    void write_unsigned(U uval) {
        static std::array<char, 50> buf;
        size_t len = 0;
        while (uval > 0) {
            buf[len++] = char((uval % 10) + '0');
            uval /= 10;
        }
        std::reverse(buf.begin(), buf.begin() + len);
        memcpy(line.data() + pos, buf.data(), len);
        pos += len;
    }

    void write_single(const std::string& s) {
        for (char c : s) write_single(c);
    }
    void write_single(const char* s) {
        size_t len = strlen(s);
        for (size_t i = 0; i < len; i++) write_single(s[i]);
    }
    template <class T> void write_single(const std::vector<T>& val) {
        auto n = val.size();
        for (size_t i = 0; i < n; i++) {
            if (i) write_single(' ');
            write_single(val[i]);
        }
    }
};

std::array<std::array<char, 2>, 100> Printer::small = [] {
    std::array<std::array<char, 2>, 100> table;
    for (int i = 0; i <= 99; i++) {
        table[i][1] = char('0' + (i % 10));
        table[i][0] = char('0' + (i / 10 % 10));
    }
    return table;
}();
std::array<unsigned long long, 20> Printer::tens = [] {
    std::array<unsigned long long, 20> table;
    for (int i = 0; i < 20; i++) {
        table[i] = 1;
        for (int j = 0; j < i; j++) {
            table[i] *= 10;
        }
    }
    return table;
}();

}  // namespace yosupo
#line 2 "/home/vscode/yosupo-library/src/yosupo/modint.hpp"

#line 1 "/home/vscode/ac-library/atcoder/modint.hpp"



#line 7 "/home/vscode/ac-library/atcoder/modint.hpp"

#ifdef _MSC_VER
#include <intrin.h>
#endif

#line 1 "/home/vscode/ac-library/atcoder/internal_math.hpp"



#include <utility>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace atcoder {

namespace internal {

// @param m `1 <= m`
// @return x mod m
constexpr long long safe_mod(long long x, long long m) {
    x %= m;
    if (x < 0) x += m;
    return x;
}

// Fast modular multiplication by barrett reduction
// Reference: https://en.wikipedia.org/wiki/Barrett_reduction
// NOTE: reconsider after Ice Lake
struct barrett {
    unsigned int _m;
    unsigned long long im;

    // @param m `1 <= m`
    explicit barrett(unsigned int m) : _m(m), im((unsigned long long)(-1) / m + 1) {}

    // @return m
    unsigned int umod() const { return _m; }

    // @param a `0 <= a < m`
    // @param b `0 <= b < m`
    // @return `a * b % m`
    unsigned int mul(unsigned int a, unsigned int b) const {
        // [1] m = 1
        // a = b = im = 0, so okay

        // [2] m >= 2
        // im = ceil(2^64 / m)
        // -> im * m = 2^64 + r (0 <= r < m)
        // let z = a*b = c*m + d (0 <= c, d < m)
        // a*b * im = (c*m + d) * im = c*(im*m) + d*im = c*2^64 + c*r + d*im
        // c*r + d*im < m * m + m * im < m * m + 2^64 + m <= 2^64 + m * (m + 1) < 2^64 * 2
        // ((ab * im) >> 64) == c or c + 1
        unsigned long long z = a;
        z *= b;
#ifdef _MSC_VER
        unsigned long long x;
        _umul128(z, im, &x);
#else
        unsigned long long x =
            (unsigned long long)(((unsigned __int128)(z)*im) >> 64);
#endif
        unsigned long long y = x * _m;
        return (unsigned int)(z - y + (z < y ? _m : 0));
    }
};

// @param n `0 <= n`
// @param m `1 <= m`
// @return `(x ** n) % m`
constexpr long long pow_mod_constexpr(long long x, long long n, int m) {
    if (m == 1) return 0;
    unsigned int _m = (unsigned int)(m);
    unsigned long long r = 1;
    unsigned long long y = safe_mod(x, m);
    while (n) {
        if (n & 1) r = (r * y) % _m;
        y = (y * y) % _m;
        n >>= 1;
    }
    return r;
}

// Reference:
// M. Forisek and J. Jancina,
// Fast Primality Testing for Integers That Fit into a Machine Word
// @param n `0 <= n`
constexpr bool is_prime_constexpr(int n) {
    if (n <= 1) return false;
    if (n == 2 || n == 7 || n == 61) return true;
    if (n % 2 == 0) return false;
    long long d = n - 1;
    while (d % 2 == 0) d /= 2;
    constexpr long long bases[3] = {2, 7, 61};
    for (long long a : bases) {
        long long t = d;
        long long y = pow_mod_constexpr(a, t, n);
        while (t != n - 1 && y != 1 && y != n - 1) {
            y = y * y % n;
            t <<= 1;
        }
        if (y != n - 1 && t % 2 == 0) {
            return false;
        }
    }
    return true;
}
template <int n> constexpr bool is_prime = is_prime_constexpr(n);

// @param b `1 <= b`
// @return pair(g, x) s.t. g = gcd(a, b), xa = g (mod b), 0 <= x < b/g
constexpr std::pair<long long, long long> inv_gcd(long long a, long long b) {
    a = safe_mod(a, b);
    if (a == 0) return {b, 0};

    // Contracts:
    // [1] s - m0 * a = 0 (mod b)
    // [2] t - m1 * a = 0 (mod b)
    // [3] s * |m1| + t * |m0| <= b
    long long s = b, t = a;
    long long m0 = 0, m1 = 1;

    while (t) {
        long long u = s / t;
        s -= t * u;
        m0 -= m1 * u;  // |m1 * u| <= |m1| * s <= b

        // [3]:
        // (s - t * u) * |m1| + t * |m0 - m1 * u|
        // <= s * |m1| - t * u * |m1| + t * (|m0| + |m1| * u)
        // = s * |m1| + t * |m0| <= b

        auto tmp = s;
        s = t;
        t = tmp;
        tmp = m0;
        m0 = m1;
        m1 = tmp;
    }
    // by [3]: |m0| <= b/g
    // by g != b: |m0| < b/g
    if (m0 < 0) m0 += b / s;
    return {s, m0};
}

// Compile time primitive root
// @param m must be prime
// @return primitive root (and minimum in now)
constexpr int primitive_root_constexpr(int m) {
    if (m == 2) return 1;
    if (m == 167772161) return 3;
    if (m == 469762049) return 3;
    if (m == 754974721) return 11;
    if (m == 998244353) return 3;
    int divs[20] = {};
    divs[0] = 2;
    int cnt = 1;
    int x = (m - 1) / 2;
    while (x % 2 == 0) x /= 2;
    for (int i = 3; (long long)(i)*i <= x; i += 2) {
        if (x % i == 0) {
            divs[cnt++] = i;
            while (x % i == 0) {
                x /= i;
            }
        }
    }
    if (x > 1) {
        divs[cnt++] = x;
    }
    for (int g = 2;; g++) {
        bool ok = true;
        for (int i = 0; i < cnt; i++) {
            if (pow_mod_constexpr(g, (m - 1) / divs[i], m) == 1) {
                ok = false;
                break;
            }
        }
        if (ok) return g;
    }
}
template <int m> constexpr int primitive_root = primitive_root_constexpr(m);

// @param n `n < 2^32`
// @param m `1 <= m < 2^32`
// @return sum_{i=0}^{n-1} floor((ai + b) / m) (mod 2^64)
unsigned long long floor_sum_unsigned(unsigned long long n,
                                      unsigned long long m,
                                      unsigned long long a,
                                      unsigned long long b) {
    unsigned long long ans = 0;
    while (true) {
        if (a >= m) {
            ans += n * (n - 1) / 2 * (a / m);
            a %= m;
        }
        if (b >= m) {
            ans += n * (b / m);
            b %= m;
        }

        unsigned long long y_max = a * n + b;
        if (y_max < m) break;
        // y_max < m * (n + 1)
        // floor(y_max / m) <= n
        n = (unsigned long long)(y_max / m);
        b = (unsigned long long)(y_max % m);
        std::swap(m, a);
    }
    return ans;
}

}  // namespace internal

}  // namespace atcoder


#line 1 "/home/vscode/ac-library/atcoder/internal_type_traits.hpp"



#line 7 "/home/vscode/ac-library/atcoder/internal_type_traits.hpp"

namespace atcoder {

namespace internal {

#ifndef _MSC_VER
template <class T>
using is_signed_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value ||
                                  std::is_same<T, __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int128 =
    typename std::conditional<std::is_same<T, __uint128_t>::value ||
                                  std::is_same<T, unsigned __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using make_unsigned_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value,
                              __uint128_t,
                              unsigned __int128>;

template <class T>
using is_integral = typename std::conditional<std::is_integral<T>::value ||
                                                  is_signed_int128<T>::value ||
                                                  is_unsigned_int128<T>::value,
                                              std::true_type,
                                              std::false_type>::type;

template <class T>
using is_signed_int = typename std::conditional<(is_integral<T>::value &&
                                                 std::is_signed<T>::value) ||
                                                    is_signed_int128<T>::value,
                                                std::true_type,
                                                std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<(is_integral<T>::value &&
                               std::is_unsigned<T>::value) ||
                                  is_unsigned_int128<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<
    is_signed_int128<T>::value,
    make_unsigned_int128<T>,
    typename std::conditional<std::is_signed<T>::value,
                              std::make_unsigned<T>,
                              std::common_type<T>>::type>::type;

#else

template <class T> using is_integral = typename std::is_integral<T>;

template <class T>
using is_signed_int =
    typename std::conditional<is_integral<T>::value && std::is_signed<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<is_integral<T>::value &&
                                  std::is_unsigned<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<is_signed_int<T>::value,
                                              std::make_unsigned<T>,
                                              std::common_type<T>>::type;

#endif

template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;

template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;

template <class T> using to_unsigned_t = typename to_unsigned<T>::type;

}  // namespace internal

}  // namespace atcoder


#line 14 "/home/vscode/ac-library/atcoder/modint.hpp"

namespace atcoder {

namespace internal {

struct modint_base {};
struct static_modint_base : modint_base {};

template <class T> using is_modint = std::is_base_of<modint_base, T>;
template <class T> using is_modint_t = std::enable_if_t<is_modint<T>::value>;

}  // namespace internal

template <int m, std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint : internal::static_modint_base {
    using mint = static_modint;

  public:
    static constexpr int mod() { return m; }
    static mint raw(int v) {
        mint x;
        x._v = v;
        return x;
    }

    static_modint() : _v(0) {}
    template <class T, internal::is_signed_int_t<T>* = nullptr>
    static_modint(T v) {
        long long x = (long long)(v % (long long)(umod()));
        if (x < 0) x += umod();
        _v = (unsigned int)(x);
    }
    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    static_modint(T v) {
        _v = (unsigned int)(v % umod());
    }

    unsigned int val() const { return _v; }

    mint& operator++() {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    mint& operator--() {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    mint& operator+=(const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        _v -= rhs._v;
        if (_v >= umod()) _v += umod();
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        unsigned long long z = _v;
        z *= rhs._v;
        _v = (unsigned int)(z % umod());
        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        if (prime) {
            assert(_v);
            return pow(umod() - 2);
        } else {
            auto eg = internal::inv_gcd(_v, m);
            assert(eg.first == 1);
            return eg.second;
        }
    }

    friend mint operator+(const mint& lhs, const mint& rhs) {
        return mint(lhs) += rhs;
    }
    friend mint operator-(const mint& lhs, const mint& rhs) {
        return mint(lhs) -= rhs;
    }
    friend mint operator*(const mint& lhs, const mint& rhs) {
        return mint(lhs) *= rhs;
    }
    friend mint operator/(const mint& lhs, const mint& rhs) {
        return mint(lhs) /= rhs;
    }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) {
        return lhs._v != rhs._v;
    }

  private:
    unsigned int _v;
    static constexpr unsigned int umod() { return m; }
    static constexpr bool prime = internal::is_prime<m>;
};

template <int id> struct dynamic_modint : internal::modint_base {
    using mint = dynamic_modint;

  public:
    static int mod() { return (int)(bt.umod()); }
    static void set_mod(int m) {
        assert(1 <= m);
        bt = internal::barrett(m);
    }
    static mint raw(int v) {
        mint x;
        x._v = v;
        return x;
    }

    dynamic_modint() : _v(0) {}
    template <class T, internal::is_signed_int_t<T>* = nullptr>
    dynamic_modint(T v) {
        long long x = (long long)(v % (long long)(mod()));
        if (x < 0) x += mod();
        _v = (unsigned int)(x);
    }
    template <class T, internal::is_unsigned_int_t<T>* = nullptr>
    dynamic_modint(T v) {
        _v = (unsigned int)(v % mod());
    }

    unsigned int val() const { return _v; }

    mint& operator++() {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    mint& operator--() {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    mint& operator+=(const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        _v += mod() - rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        _v = bt.mul(_v, rhs._v);
        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        auto eg = internal::inv_gcd(_v, mod());
        assert(eg.first == 1);
        return eg.second;
    }

    friend mint operator+(const mint& lhs, const mint& rhs) {
        return mint(lhs) += rhs;
    }
    friend mint operator-(const mint& lhs, const mint& rhs) {
        return mint(lhs) -= rhs;
    }
    friend mint operator*(const mint& lhs, const mint& rhs) {
        return mint(lhs) *= rhs;
    }
    friend mint operator/(const mint& lhs, const mint& rhs) {
        return mint(lhs) /= rhs;
    }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) {
        return lhs._v != rhs._v;
    }

  private:
    unsigned int _v;
    static internal::barrett bt;
    static unsigned int umod() { return bt.umod(); }
};
template <int id> internal::barrett dynamic_modint<id>::bt(998244353);

using modint998244353 = static_modint<998244353>;
using modint1000000007 = static_modint<1000000007>;
using modint = dynamic_modint<-1>;

namespace internal {

template <class T>
using is_static_modint = std::is_base_of<internal::static_modint_base, T>;

template <class T>
using is_static_modint_t = std::enable_if_t<is_static_modint<T>::value>;

template <class> struct is_dynamic_modint : public std::false_type {};
template <int id>
struct is_dynamic_modint<dynamic_modint<id>> : public std::true_type {};

template <class T>
using is_dynamic_modint_t = std::enable_if_t<is_dynamic_modint<T>::value>;

}  // namespace internal

}  // namespace atcoder


#line 4 "/home/vscode/yosupo-library/src/yosupo/modint.hpp"

#include <iostream>

namespace atcoder {

template <int MOD>
std::ostream& operator<<(std::ostream& os, const static_modint<MOD>& x) {
    return os << x.val();
}

template <int ID>
std::ostream& operator<<(std::ostream& os, const dynamic_modint<ID>& x) {
    return os << x.val();
}

}  // namespace atcoder

namespace yosupo {

template <int MOD> using static_modint = atcoder::static_modint<MOD>;

template <int ID> using dynamic_modint = atcoder::dynamic_modint<ID>;

using modint998244353 = atcoder::modint998244353;
using modint1000000007 = atcoder::modint1000000007;
using modint = atcoder::modint;

// modint of 2^61 - 1
struct modint61 {
    using mint = modint61;

  public:
    static constexpr long long mod() { return (1ULL << 61) - 1; }
    static mint raw(long long v) {
        mint x;
        x._v = v;
        return x;
    }

    modint61() : _v(0) {}

    template <class T, atcoder::internal::is_signed_int_t<T>* = nullptr>
    modint61(T v) {
        long long x = (long long)(v % (long long)(umod()));
        if (x < 0) x += umod();
        _v = (unsigned long long)(x);
    }
    template <class T, atcoder::internal::is_unsigned_int_t<T>* = nullptr>
    modint61(T v) {
        _v = (unsigned long long)(v % umod());
    }

    unsigned long long val() const { return _v; }

    mint& operator++() {
        _v++;
        if (_v == umod()) _v = 0;
        return *this;
    }
    mint& operator--() {
        if (_v == 0) _v = umod();
        _v--;
        return *this;
    }
    mint operator++(int) {
        mint result = *this;
        ++*this;
        return result;
    }
    mint operator--(int) {
        mint result = *this;
        --*this;
        return result;
    }

    mint& operator+=(const mint& rhs) {
        _v += rhs._v;
        if (_v >= umod()) _v -= umod();
        return *this;
    }
    mint& operator-=(const mint& rhs) {
        _v -= rhs._v;
        if (_v >= umod()) _v += umod();
        return *this;
    }
    mint& operator*=(const mint& rhs) {
        __uint128_t t = (__uint128_t) _v * rhs._v;

        _v = (unsigned long long)((t >> 61) + (t & umod()));
        _v = (_v >= umod()) ? _v - umod() : _v;

        return *this;
    }
    mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

    mint operator+() const { return *this; }
    mint operator-() const { return mint() - *this; }

    mint pow(long long n) const {
        assert(0 <= n);
        mint x = *this, r = 1;
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }
    mint inv() const {
        assert(_v);
        return pow(umod() - 2);
    }

    friend mint operator+(const mint& lhs, const mint& rhs) {
        return mint(lhs) += rhs;
    }
    friend mint operator-(const mint& lhs, const mint& rhs) {
        return mint(lhs) -= rhs;
    }
    friend mint operator*(const mint& lhs, const mint& rhs) {
        return mint(lhs) *= rhs;
    }
    friend mint operator/(const mint& lhs, const mint& rhs) {
        return mint(lhs) /= rhs;
    }
    friend bool operator==(const mint& lhs, const mint& rhs) {
        return lhs._v == rhs._v;
    }
    friend bool operator!=(const mint& lhs, const mint& rhs) {
        return lhs._v != rhs._v;
    }

  private:
    unsigned long long _v;
    static constexpr unsigned long long umod() { return mod(); }
};

}  // namespace yosupo
#line 2 "/home/vscode/yosupo-library/src/yosupo/random.hpp"

#line 6 "/home/vscode/yosupo-library/src/yosupo/random.hpp"
#include <chrono>
#line 9 "/home/vscode/yosupo-library/src/yosupo/random.hpp"

namespace yosupo {

struct Xoshiro256StarStar {
  public:
    using result_type = uint64_t;
    Xoshiro256StarStar() : Xoshiro256StarStar(0) {}
    explicit Xoshiro256StarStar(uint64_t seed) {
        // use splitmix64
        // Reference: http://xoshiro.di.unimi.it/splitmix64.c
        for (int i = 0; i < 4; i++) {
            uint64_t z = (seed += 0x9e3779b97f4a7c15);
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            s[i] = z ^ (z >> 31);
        }
    }

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return -1; }

    result_type operator()() {
        const uint64_t result_starstar = rotl(s[1] * 5, 7) * 9;

        const uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result_starstar;
    }

  private:
    // Use xoshiro256**
    // Refereces: http://xoshiro.di.unimi.it/xoshiro256starstar.c
    static uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    std::array<uint64_t, 4> s;
};

namespace internal {

// random choice from [0, upper]
template <class G> uint64_t uniform(uint64_t upper, G& gen) {
    static_assert(std::is_same<uint64_t, typename G::result_type>::value, "");
    static_assert(G::min() == 0, "");
    static_assert(G::max() == uint64_t(-1), "");
    if (!(upper & (upper + 1))) {
        // b = 00..0011..11
        return gen() & upper;
    }
    int log = 63 - std::countl_zero(upper);
    uint64_t mask = (log == 63) ? ~0ULL : (1ULL << (log + 1)) - 1;
    while (true) {
        uint64_t r = gen() & mask;
        if (r <= upper) return r;
    }
}

}  // namespace internal

inline Xoshiro256StarStar& global_gen() {
    static Xoshiro256StarStar gen(
        std::chrono::steady_clock::now().time_since_epoch().count());
    return gen;
}

template <class T, class G> T uniform(T lower, T upper, G& gen) {
    return T(lower + internal::uniform(uint64_t(upper) - uint64_t(lower), gen));
}
template <class T> T uniform(T lower, T upper) {
    return uniform(lower, upper, global_gen());
}

template <class G> bool uniform_bool(G& gen) {
    return internal::uniform(1, gen) == 1;
}
inline bool uniform_bool() { return uniform_bool(global_gen()); }

template <class T, class G>
std::pair<T, T> uniform_pair(T lower, T upper, G& gen) {
    assert(upper - lower >= 1);
    T a, b;
    do {
        a = uniform(lower, upper, gen);
        b = uniform(lower, upper, gen);
    } while (a == b);
    if (a > b) std::swap(a, b);
    return {a, b};
}
template <class T> std::pair<T, T> uniform_pair(T lower, T upper) {
    return uniform_pair(lower, upper, global_gen());
}

}  // namespace yosupo
#line 2 "/home/vscode/yosupo-library/src/yosupo/stopwatch.hpp"

#line 4 "/home/vscode/yosupo-library/src/yosupo/stopwatch.hpp"

namespace yosupo {

struct StopWatch {
    std::chrono::steady_clock::time_point begin;

    StopWatch() : begin(std::chrono::steady_clock::now()) {}
    
    int msecs() {
        auto now = std::chrono::steady_clock::now();
        return int(duration_cast<std::chrono::milliseconds>(now - begin).count());
    }
};

}
#line 11 "src/A/main.cpp"
using namespace yosupo;

using mint = modint998244353;

#line 4 "/home/vscode/Algorithm/src/base.hpp"
#include <bitset>
#line 6 "/home/vscode/Algorithm/src/base.hpp"
#include <complex>
#include <cstdio>
#line 10 "/home/vscode/Algorithm/src/base.hpp"
#include <map>
#line 12 "/home/vscode/Algorithm/src/base.hpp"
#include <queue>
#include <set>
#line 15 "/home/vscode/Algorithm/src/base.hpp"
#include <unordered_map>
#include <unordered_set>
#line 18 "/home/vscode/Algorithm/src/base.hpp"
#include <memory>
#line 20 "/home/vscode/Algorithm/src/base.hpp"

using namespace std;

using uint = unsigned int;
using ll = long long;
using ull = unsigned long long;
constexpr ll TEN(int n) { return (n == 0) ? 1 : 10 * TEN(n - 1); }
template <class T> using V = vector<T>;
template <class T> using VV = V<V<T>>;

#ifdef LOCAL

ostream& operator<<(ostream& os, __int128_t x) {
    if (x < 0) {
        os << "-";
        x *= -1;
    }
    if (x == 0) {
        return os << "0";
    }
    string s;
    while (x) {
        s += char(x % 10 + '0');
        x /= 10;
    }
    reverse(s.begin(), s.end());
    return os << s;
}
ostream& operator<<(ostream& os, __uint128_t x) {
    if (x == 0) {
        return os << "0";
    }
    string s;
    while (x) {
        s += char(x % 10 + '0');
        x /= 10;
    }
    reverse(s.begin(), s.end());
    return os << s;
}

template <class T, class U>
ostream& operator<<(ostream& os, const pair<T, U>& p);
template <class T> ostream& operator<<(ostream& os, const V<T>& v);
template <class T> ostream& operator<<(ostream& os, const deque<T>& v);
template <class T, size_t N>
ostream& operator<<(ostream& os, const array<T, N>& a);
template <class T> ostream& operator<<(ostream& os, const set<T>& s);
template <class T, class U>
ostream& operator<<(ostream& os, const map<T, U>& m);

template <class T, class U>
ostream& operator<<(ostream& os, const pair<T, U>& p) {
    return os << "P(" << p.first << ", " << p.second << ")";
}

template <class T> ostream& operator<<(ostream& os, const V<T>& v) {
    os << "[";
    bool f = false;
    for (auto d : v) {
        if (f) os << ", ";
        f = true;
        os << d;
    }
    return os << "]";
}

template <class T> ostream& operator<<(ostream& os, const deque<T>& v) {
    os << "[";
    bool f = false;
    for (auto d : v) {
        if (f) os << ", ";
        f = true;
        os << d;
    }
    return os << "]";
}
template <class T, size_t N>
ostream& operator<<(ostream& os, const array<T, N>& a) {
    os << "[";
    bool f = false;
    for (auto d : a) {
        if (f) os << ", ";
        f = true;
        os << d;
    }
    return os << "]";
}

template <class T> ostream& operator<<(ostream& os, const set<T>& s) {
    os << "{";
    bool f = false;
    for (auto d : s) {
        if (f) os << ", ";
        f = true;
        os << d;
    }
    return os << "}";
}
template <class T> ostream& operator<<(ostream& os, const multiset<T>& s) {
    os << "{";
    bool f = false;
    for (auto d : s) {
        if (f) os << ", ";
        f = true;
        os << d;
    }
    return os << "}";
}

template <class T, class U>
ostream& operator<<(ostream& os, const map<T, U>& s) {
    os << "{";
    bool f = false;
    for (auto p : s) {
        if (f) os << ", ";
        f = true;
        os << p.first << ": " << p.second;
    }
    return os << "}";
}

struct PrettyOS {
    ostream& os;
    bool first;

    template <class T> auto operator<<(T&& x) {
        if (!first) os << ", ";
        first = false;
        os << x;
        return *this;
    }
};
template <class... T> void dbg0(T&&... t) {
    (PrettyOS{cerr, true} << ... << t);
}
#define dbg(...)                                            \
    do {                                                    \
        cerr << __LINE__ << " : " << #__VA_ARGS__ << " = "; \
        dbg0(__VA_ARGS__);                                  \
        cerr << endl;                                       \
    } while (false);
#else
#define dbg(...)
#endif
#line 16 "src/A/main.cpp"

Scanner sc = Scanner(stdin);
Printer pr = Printer(stdout);

const int N = 9;
const int M = 20;
const int K = 81;

using Board = array<array<mint, N>, N>;
using Stamp = array<array<mint, 3>, 3>;
using Query = array<array<array<int, M>, 7>, 7>;

const Board board = []() {
    int dummy;
    sc.read(dummy, dummy, dummy);

    Board a;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int x;
            sc.read(x);
            a[i][j] = x;
        }
    }

    return a;
}();

const array<Stamp, M> stamps = []() {
    array<Stamp, M> s;
    for (int i = 0; i < M; i++) {
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                int x;
                sc.read(x);
                s[i][r][c] = x;
            }
        }
    }
    return s;
}();

static Xoshiro256StarStar gen(123456);
static StopWatch sw;

void add(Board& b, int r, int c, int i, mint f) {
    if (f.val() == 0) return;
    for (int dr = 0; dr < 3; dr++) {
        for (int dc = 0; dc < 3; dc++) {
            b[r + dr][c + dc] += mint(f) * stamps[i][dr][dc];
        }
    }
};

void add1(Board& b, int r, int c, int i) {
    for (int dr = 0; dr < 3; dr++) {
        for (int dc = 0; dc < 3; dc++) {
            b[r + dr][c + dc] += stamps[i][dr][dc];
        }
    }
};
void dec1(Board& b, int r, int c, int i) {
    for (int dr = 0; dr < 3; dr++) {
        for (int dc = 0; dc < 3; dc++) {
            b[r + dr][c + dc] -= stamps[i][dr][dc];
        }
    }
};

ll calc_score(Query queries) {
    Board a = board;
    for (int r = 0; r <= 6; r++) {
        for (int c = 0; c <= 6; c++) {
            for (int i = 0; i < M; i++) {
                add(a, r, c, i, queries[r][c][i]);
            }
        }
    }
    ll score = 0;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            score += a[r][c].val();
        }
    }
    return score;
}

Board gen_board(Query queries) {
    Board a = board;
    for (int r = 0; r <= 6; r++) {
        for (int c = 0; c <= 6; c++) {
            for (int i = 0; i < M; i++) {
                add(a, r, c, i, queries[r][c][i]);
            }
        }
    }
    return a;
}

void check(Board actual, Query queries) {
    Board a = board;
    for (int r = 0; r <= 6; r++) {
        for (int c = 0; c <= 6; c++) {
            for (int i = 0; i < M; i++) {
                add(a, r, c, i, queries[r][c][i]);
            }
        }
    }
    assert(actual == a);
}

Query solve_wave() {
    struct State {
        Board a;
        Query queries;
        ll score;
    };

    const int B = 15;
    VV<State> states(K + 1);
    auto shrink = [&]() {
        for (int k = 0; k <= K; k++) {
            if (int(states[k].size()) > B) {
                sort(states[k].begin(), states[k].end(),
                     [&](const State& l, const State& r) {
                         return l.score > r.score;
                     });
                states[k].resize(B);
            }
        }
    };
    states[0].push_back(State{board, {}, 0});

    int step = 0;
    auto ok_k = [&](int k) {
        return true;
    };

    auto succ0 = [&](int r, int c) {
        int st = sw.msecs();

        VV<State> nstates(K + 1);
        for (int k = 0; k < K; k++) {
            if (!ok_k(k)) continue;
            for (const auto& state : states[k]) {
                Board a = state.a;
                for (int x = -1; x < M; x++) {
                    if (x != -1 && step > 20) continue;
                    if (x != -1) add1(a, r, c, x);

                    for (int y = x; y < M; y++) {
                        if (y != -1) add1(a, r, c, y);

                        for (int z = y; z < M; z++) {
                            int nk = k;
                            if (x != -1) nk++;
                            if (y != -1) nk++;
                            if (z != -1) nk++;
                            if (nk > K) continue;

                            if (z != -1) add1(a, r, c, z);

                            ll nscore = state.score;
                            nscore += a[r][c].val();

                            if (int(nstates[nk].size()) < B ||
                                nstates[nk].back().score < nscore) {
                                Query queries = state.queries;
                                if (x != -1) queries[r][c][x]++;
                                if (y != -1) queries[r][c][y]++;
                                if (z != -1) queries[r][c][z]++;

                                nstates[nk].push_back({a, queries, nscore});
                                sort(nstates[nk].begin(), nstates[nk].end(),
                                    [&](const State& lhs, const State& rhs) {
                                        return lhs.score > rhs.score;
                                    });
                                if (nstates[nk].size() == B + 1) {
                                    nstates[nk].pop_back();
                                }
                            }

                            if (z != -1) dec1(a, r, c, z);
                        }

                        if (y != -1) dec1(a, r, c, y);
                    }
                    if (x != -1) dec1(a, r, c, x);
                }
            }
        }
        states = nstates;

        dbg(step, r, c, sw.msecs() - st);
        step++;
    };
    auto succ1 = [&](int r) {
        int st = sw.msecs();

        VV<State> nstates(K + 1);
        for (int k = 0; k <= K; k++) {
            if (!ok_k(k)) continue;
            for (const auto& state : states[k]) {
                Board a = state.a;
                for (int x = -1; x < M; x++) {
                    if (x != -1) add1(a, r, 6, x);

                    for (int y = x; y < M; y++) {
                        if (y != -1) add1(a, r, 6, y);                        

                        for (int z = y; z < M; z++) {

                            int nk = k;
                            if (x != -1) nk++;
                            if (y != -1) nk++;
                            if (z != -1) nk++;
                            if (nk > K) continue;

                            if (z != -1) add1(a, r, 6, z);

                            ll nscore = state.score;
                            nscore += a[r][6].val();
                            nscore += a[r][7].val();
                            nscore += a[r][8].val();

                            if (int(nstates[nk].size()) < B ||
                                nstates[nk].back().score < nscore) {
                                Query queries = state.queries;
                                if (x != -1) queries[r][6][x]++;
                                if (y != -1) queries[r][6][y]++;
                                if (z != -1) queries[r][6][z]++;

                                nstates[nk].push_back({a, queries, nscore});
                                sort(nstates[nk].begin(), nstates[nk].end(),
                                        [&](const State& lhs,
                                            const State& rhs) {
                                            return lhs.score > rhs.score;
                                        });
                                if (nstates[nk].size() == B + 1) {
                                    nstates[nk].pop_back();
                                }
                            }
                            if (z != -1) dec1(a, r, 6, z);
                        }

                        if (y != -1) dec1(a, r, 6, y);
                    }

                    if (x != -1) dec1(a, r, 6, x);
                }
            }
        }
        states = nstates;

        dbg(step, r, sw.msecs() - st);
        step++;
    };

    auto succ2 = [&](int c) {
        int st = sw.msecs();

        VV<State> nstates(K + 1);
        for (int k = 0; k < K; k++) {
            if (!ok_k(k)) continue;
            for (const auto& state : states[k]) {
                Board a = state.a;
                for (int x = -1; x < M; x++) {
                    if (x != -1) add1(a, 6, c, x);
                    for (int y = x; y < M; y++) {
                        if (y != -1) add1(a, 6, c, y);
                        for (int z = y; z < M; z++) {
                            int nk = k;
                            if (x != -1) nk++;
                            if (y != -1) nk++;
                            if (z != -1) nk++;
                            if (nk > K) continue;

                            if (z != -1) add1(a, 6, c, z);

                            ll nscore = state.score;
                            nscore += a[6][c].val();
                            nscore += a[7][c].val();
                            nscore += a[8][c].val();

                            if (int(nstates[nk].size()) < B ||
                                nstates[nk].back().score < nscore) {
                                Query queries = state.queries;
                                if (x != -1) queries[6][c][x]++;
                                if (y != -1) queries[6][c][y]++;
                                if (z != -1) queries[6][c][z]++;

                                nstates[nk].push_back({a, queries, nscore});
                                sort(nstates[nk].begin(), nstates[nk].end(),
                                     [&](const State& lhs, const State& rhs) {
                                         return lhs.score > rhs.score;
                                     });
                                if (nstates[nk].size() == B + 1) {
                                    nstates[nk].pop_back();
                                }
                            }

                            if (z != -1) dec1(a, 6, c, z);
                        }
                        if (y != -1) dec1(a, 6, c, y);
                    }
                    if (x != -1) dec1(a, 6, c, x);
                }
            }
        }
        states = nstates;

        dbg(step, c, sw.msecs() - st);
        step++;
    };

    for (int ph = 0; ph < 6; ph++) {
        for (int c = ph; c < 6; c++) {
            succ0(ph, c);
        }
        succ1(ph);
        for (int r = ph + 1; r < 6; r++) {
            succ0(r, ph);
        }
        succ2(ph);
    }
/*
    for (int r = 0; r + 3 < 9; r++) {
        for (int c = 0; c + 3 < 9; c++) {
            succ0(r, c);
        }
        succ1(r);
    }
    for (int c = 0; c + 3 < 9; c++) {
        dbg("C", c);
        succ2(c);
    }
  */  

/*
    for (int r = 0; r + 3 < 9; r++) {
        for (int c = 0; c + 3 < 9; c++) {
            succ0(r, c);
        }
    }
    for (int r = 0; r + 3 < 9; r++) {
        dbg("R", r);
        succ1(r);
    }
    for (int c = 0; c + 3 < 9; c++) {
        dbg("C", c);
        succ2(c);
    }
*/

    int st = sw.msecs();

    ll gc = 0;
    Query best_queries = {};
    ll best = calc_score(best_queries);
    for (int k = K; k >= 0; k--) {
        for (const auto& state : states[k]) {
            if (state.score + 998244352LL * 9 <= best) continue;

            Board a = state.a;
            for (int x = -1; x < M; x++) {
                if (x != -1) add1(a, 6, 6, x);                
                for (int y = x; y < M; y++) {
                    if (y != -1) add1(a, 6, 6, y);
                    for (int z = y; z < M; z++) {
                        if (z != -1) add1(a, 6, 6, z);
                        for (int w = z; w < M; w++) {
                            if (w != -1) add1(a, 6, 6, w);
                            for (int l = w; l < M; l++) {

                                int nk = k;
                                if (x != -1) nk++;
                                if (y != -1) nk++;
                                if (z != -1) nk++;
                                if (w != -1) nk++;
                                if (l != -1) nk++;
                                if (nk > K) continue;

                                if (l != -1) add1(a, 6, 6, l);

                                ll nscore = state.score;
                                for (int r = 6; r < 9; r++) {
                                    for (int c = 6; c < 9; c++) {
                                        nscore += a[r][c].val();
                                    }
                                }

                                if (best < nscore) {
                                    Query queries = state.queries;
                                    if (x != -1) queries[6][6][x]++;
                                    if (y != -1) queries[6][6][y]++;
                                    if (z != -1) queries[6][6][z]++;
                                    if (w != -1) queries[6][6][w]++;
                                    if (l != -1) queries[6][6][l]++;

                                    best = nscore;
                                    best_queries = queries;
                                }
                                if (l != -1) dec1(a, 6, 6, l);
                            }
                            if (w != -1) dec1(a, 6, 6, w);
                        }
                        if (z != -1) dec1(a, 6, 6, z);
                    }
                    if (y != -1) dec1(a, 6, 6, y);
                }
                if (x != -1) dec1(a, 6, 6, x);
            }
        }
    }

    dbg("final", sw.msecs() - st);
    return best_queries;
}




Query greedy_opt(Query queries) {
    ll score = calc_score(queries);
    Board a = gen_board(queries);

    Query best_queries = queries;
    ll best = score;

    for (int ph = 0; ph < 1000; ph++) {
        int r = uniform(0, 6, gen);
        int c = uniform(0, 6, gen);
        array<int, M> pre_q = queries[r][c];
        ll pre = 0;
        for (int dr = 0; dr < 3; dr++) {
            for (int dc = 0; dc < 3; dc++) {
                pre += a[r + dr][c + dc].val();
            }
        }
        for (int i = 0; i < M; i++) {
            add(a, r, c, i, -mint(pre_q[i]));
        }
        for (int i = 0; i < M; i++) {
            add(a, r, c, i, 1);
            ll now = 0;
            for (int dr = 0; dr < 3; dr++) {
                for (int dc = 0; dc < 3; dc++) {
                    now += a[r + dr][c + dc].val();
                }
            }
            add(a, r, c, i, -1);

            ll inc = now - pre;

            if (inc >= 0) {
                score += inc;
                pre = now;
                queries[r][c].fill(0);
                queries[r][c][i]++;
            }
        }

        for (int i = 0; i < M; i++) {
            add(a, r, c, i, mint(queries[r][c][i]));
        }
        if (best < score) {
            dbg(best, score, ph, r, c);
            best = score;
            best_queries = queries;
        }
    }

    assert(calc_score(best_queries) == best);
    dbg(calc_score(queries));
    return best_queries;
}


Query solve() {
    Query queries = solve_wave();
    dbg("score: ", calc_score(queries));
    queries = greedy_opt(queries);
    return queries;
}


int main() {
    auto queries = solve();

    dbg("SCORE: ", calc_score(queries));

    V<tuple<int, int, int>> ans;
    for (int r = 0; r <= 6; r++) {
        for (int c = 0; c <= 6; c++) {
            for (int i = 0; i < M; i++) {
                for (int k = 0; k < queries[r][c][i]; k++) {
                    ans.push_back({i, r, c});
                }
            }
        }
    }

    pr.writeln(ans.size());
    for (auto [i, r, c] : ans) {
        pr.writeln(i, ' ', r, ' ', c);
    }
}
