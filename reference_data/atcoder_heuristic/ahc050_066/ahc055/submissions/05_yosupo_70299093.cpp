// #undef YOSUPO_LOCAL

#include <bitset>
#if 0 and !defined(__clang__)
#include <vector>
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#pragma GCC optimize("Ofast")
#endif

#include <iostream>

#include <stdio.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>



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

    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<!std::is_same<char, U>::value>* = nullptr>
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

inline std::array<std::array<char, 2>, 100> Printer::small = [] {
    std::array<std::array<char, 2>, 100> table;
    for (int i = 0; i <= 99; i++) {
        table[i][1] = char('0' + (i % 10));
        table[i][0] = char('0' + (i / 10 % 10));
    }
    return table;
}();
inline std::array<unsigned long long, 20> Printer::tens = [] {
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

#include <cmath>
#include <concepts>
#include <limits>
#include <random>
#include <utility>



namespace yosupo {

using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using i128 = __int128;
using u128 = unsigned __int128;

using f32 = float;
using f64 = double;

}  // namespace yosupo

namespace yosupo {

struct Xoshiro256StarStar {
  public:
    using result_type = u64;
    Xoshiro256StarStar() : Xoshiro256StarStar(0) {}
    explicit Xoshiro256StarStar(u64 seed) {
        // use splitmix64
        // Reference: http://xoshiro.di.unimi.it/splitmix64.c
        for (int i = 0; i < 4; i++) {
            u64 z = (seed += 0x9e3779b97f4a7c15);
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
            z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
            s[i] = z ^ (z >> 31);
        }
    }

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return -1; }

    result_type operator()() {
        const u64 result_starstar = rotl(s[1] * 5, 7) * 9;

        const u64 t = s[1] << 17;

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
    static u64 rotl(const u64 x, int k) { return (x << k) | (x >> (64 - k)); }

    std::array<u64, 4> s;
};

// https://github.com/wangyi-fudan/wyhash
struct WYRand {
  public:
    using result_type = u64;
    explicit WYRand(u64 seed) : s(seed) {}

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return -1; }

    result_type operator()() {
        s += 0x2d358dccaa6c78a5;
        auto x = (u128)s * (s ^ 0x8bb84b93962eacc9);
        return (u64)(x ^ (x >> 64));
    }

  private:
    uint64_t s;
};
using Random = WYRand;
inline Random get_random() { return Random(std::random_device()()); }

namespace internal {
inline Random global_gen = get_random();
}
inline Random& global_gen() { return internal::global_gen; }

template <class G>
concept random_64 = std::uniform_random_bit_generator<G> &&
                    std::same_as<u64, std::invoke_result_t<G&>> &&
                    G::min() == u64(0) && G::max() == u64(-1);

namespace internal {

// random choice from [0, upper]
template <random_64 G> u64 uniform_u64(u64 upper, G& gen) {
    if (upper == 0) return 0;
    u64 mask = (std::bit_floor(upper) << 1) - 1;
    while (true) {
        u64 r = gen() & mask;
        if (r <= upper) return r;
    }
}

// random choice from [0, upper], faster than uniform_u64
template <random_64 G> u64 random_u64(u64 upper, G& gen) {
    return (u64)(((u128)(upper) + 1) * gen() >> 64);
}

}  // namespace internal

template <class T, random_64 G> T uniform(T lower, T upper, G& gen) {
    return T(lower + internal::uniform_u64(u64(upper) - u64(lower), gen));
}
template <class T> T uniform(T lower, T upper) {
    return uniform(lower, upper, global_gen());
}

template <std::unsigned_integral T, random_64 G> T uniform(G& gen) {
    return T(gen());
}
template <std::signed_integral T, random_64 G> T uniform(G& gen) {
    return T(gen() + (u64)std::numeric_limits<T>::min());
}
template <class T, random_64 G>
    requires requires {
        { T::mod() } -> std::integral;
    }
T uniform(G& gen) {
    return T(uniform(0, T::mod() - 1, gen));
}
template <class T> T uniform() { return uniform<T>(global_gen()); }

template <class T, random_64 G> T random(T lower, T upper, G& gen) {
    return T(lower + internal::random_u64(u64(upper) - u64(lower), gen));
}
template <class T> T random(T lower, T upper) {
    return random(lower, upper, global_gen());
}

template <random_64 G> bool uniform_bool(G& gen) { return gen() & 1; }
inline bool uniform_bool() { return uniform_bool(global_gen()); }

// select 2 elements from [lower, uppper]
template <class T, random_64 G>
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

// random 0.0 <= X < 1.0
template <class G> inline double random_01(G& gen) {
    constexpr double inv = 1.0 / ((double)(u64(1) << 63) * 2);
    return double(gen()) * inv;
}
inline double random_01() { return random_01(global_gen()); }

}  // namespace yosupo

#include <chrono>

namespace yosupo {

struct StopWatch {
    std::chrono::steady_clock::time_point begin;

    StopWatch() : begin(std::chrono::steady_clock::now()) {}

    int msecs() {
        auto now = std::chrono::steady_clock::now();
        return int(
            duration_cast<std::chrono::milliseconds>(now - begin).count());
    }
};

}  // namespace yosupo


#include <map>
#include <queue>
#include <ranges>
#include <set>


#include <cstddef>
#include <tuple>


namespace yosupo {

inline std::string dump(const std::string& t) { return t; }
inline std::string dump(const char* t) { return t; }

template <std::integral T> std::string dump(T t) { return std::to_string(t); }

inline std::string dump(const u128& t) {
    if (t == 0) {
        return "0";
    }

    std::string s;
    u128 x = t;
    while (x) {
        s += char(x % 10 + '0');
        x /= 10;
    }
    std::ranges::reverse(s);
    return s;
}

inline std::string dump(const i128& t) {
    if (t < 0) {
        return "-" + dump((u128)(-t));
    } else {
        return dump((u128)(t));
    }
}

template <std::floating_point T> std::string dump(T t) {
    return std::to_string(t);
}

template <class T>
    requires requires(T t) { t.dump(); }
std::string dump(T t);
template <class T>
    requires(!requires(T t) { t.dump(); }) && (requires(T t) { t.val(); })
std::string dump(T t);

template <class T, std::size_t N> std::string dump(const std::array<T, N>&);
template <class T> std::string dump(const std::vector<T>&);
template <class T1, class T2> std::string dump(const std::pair<T1, T2>&);
template <class K, class V> std::string dump(const std::map<K, V>&);
template <class T> std::string dump(const std::set<T>&);
template <class... Ts> std::string dump(const std::tuple<Ts...>& t);

template <class T>
    requires requires(T t) { t.dump(); }
std::string dump(T t) {
    return dump(t.dump());
}

template <class T>
    requires(!requires(T t) { t.dump(); }) && (requires(T t) { t.val(); })
std::string dump(T t) {
    return dump(t.val());
}

template <class T, std::size_t N> std::string dump(const std::array<T, N>& a) {
    std::string s = "[";
    for (size_t i = 0; i < N; i++) {
        if (i) {
            s += ", ";
        }
        s += dump(a[i]);
    }
    s += "]";
    return s;
}

template <class T> std::string dump(const std::vector<T>& v) {
    std::string s = "[";
    for (std::size_t i = 0; i < v.size(); ++i) {
        s += dump(v[i]);
        if (i + 1 != v.size()) {
            s += ", ";
        }
    }
    s += "]";
    return s;
}

template <class T1, class T2> std::string dump(const std::pair<T1, T2>& p) {
    std::string s = "(";
    s += dump(p.first);
    s += ", ";
    s += dump(p.second);
    s += ")";
    return s;
}

template <class K, class V> std::string dump(const std::map<K, V>& m) {
    std::string s = "{";
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it != m.begin()) {
            s += ", ";
        }
        s += dump(it->first);
        s += ": ";
        s += dump(it->second);
    }
    s += "}";
    return s;
}

template <class T> std::string dump(const std::set<T>& s) {
    std::string str = "{";
    for (auto it = s.begin(); it != s.end(); ++it) {
        if (it != s.begin()) {
            str += ", ";
        }
        str += dump(*it);
    }
    str += "}";
    return str;
}

template <class... Ts> std::string dump(const std::tuple<Ts...>& t) {
    std::string s = "(";
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        ((s += dump(std::get<I>(t)) + ((I < sizeof...(Ts) - 1) ? ", " : "")),
         ...);
    }(std::make_index_sequence<sizeof...(Ts)>());
    s += ")";
    return s;
}

}  // namespace yosupo

#include <functional>
#include <span>

namespace yosupo {

template <class T> bool chmin(T& a, const T& b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}

template <class T> bool chmax(T& a, const T& b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}

template <class T> T floor_div(T x, T y) {
    auto d = x / y;
    auto r = x % y;
    if (r == 0) return d;
    if ((r > 0) == (y > 0)) return d;
    return d - 1;
}
template <class T> T ceil_div(T x, T y) {
    auto d = x / y;
    auto r = x % y;
    if (r == 0) return d;
    if ((r > 0) == (y > 0)) return d + 1;
    return d;
}

template <std::ranges::input_range R>
std::vector<std::ranges::range_value_t<R>> to_vec(R&& r) {
    auto common = r | std::views::common;
    return std::vector(common.begin(), common.end());
}

template <class T, class Comp = std::equal_to<>>
void dedup(std::vector<T>& v, Comp comp = Comp{}) {
    auto it = std::ranges::unique(v, comp);
    v.erase(it.begin(), it.end());
}

template <size_t N, class T> std::span<T, N> subspan(std::span<T> a, int idx) {
    return a.subspan(idx).template first<N>();
}

inline auto rep(int l, int r) {
    if (l > r) return std::views::iota(l, l);
    return std::views::iota(l, r);
}

}  // namespace yosupo
using namespace yosupo;

using std::abs, std::pow, std::sqrt;
using std::array, std::vector, std::string, std::queue, std::deque;
using std::countl_zero, std::countl_one, std::countr_zero, std::countr_one;
using std::istream, std::ostream, std::cerr, std::endl;
using std::min, std::max, std::swap;
using std::pair, std::tuple, std::bitset;
using std::popcount;
using std::priority_queue, std::set, std::multiset, std::map;
using std::views::iota, std::views::reverse;

namespace ranges = std::ranges;
using ranges::sort, ranges::copy_n;

using uint = unsigned int;
using ll = long long;
using ull = unsigned long long;
constexpr ll TEN(int n) { return (n == 0) ? 1 : 10 * TEN(n - 1); }
template <class T> using V = vector<T>;
template <class T> using VV = V<V<T>>;

#ifdef YOSUPO_LOCAL

struct PrettyOS {
    ostream& os;
    bool first;

    template <class T> auto operator<<(T&& x) {
        if (!first) os << ", ";
        first = false;
        os << yosupo::dump(x);
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

Scanner sc = Scanner(stdin);
Printer pr = Printer(stdout);
Random gen = Random(123456789);
StopWatch global_sw;

#ifdef YOSUPO_LOCAL
const double SPEED_FREQ = 0.6;
#else
const double SPEED_FREQ = 1.0;
#endif

const int N = 200;

const array<int, N> H = []() {
    int n;
    sc.read(n);
#ifdef YOSUPO_LOCAL
    assert(n == N);
#endif

    array<int, N> h{};
    for (int i = 0; i < N; i++) {
        sc.read(h[i]);
    }
    return h;
}();

const int TOTAL_H = []() {
    int total_h = 0;
    for (int i : iota(0, N)) {
        total_h += H[i];
    }
    return total_h;
}();

const array<int, N> C = []() {
    array<int, N> c{};
    for (int i = 0; i < N; i++) {
        sc.read(c[i]);
        if (c[i] < 0) c[i] = 0;
    }
    return c;
}();

const array<array<int, N>, N> A = []() {
    array<array<int, N>, N> a{};
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sc.read(a[i][j]);
        }
    }
    return a;
}();

const array<int, N + 1> SUM_C = []() {
    array<int, N + 1> sum_c;
    sum_c[0] = 0;
    for (int i : iota(0, N)) {
        sum_c[i + 1] = sum_c[i] + C[i];
    }
    return sum_c;
}();

const V<int> W_OF = []() {
    V<int> w;
    for (int weapon = 0; weapon < N; weapon++) {
        for (int k = 0; k < C[weapon]; k++) {
            w.push_back(weapon);
        }
    }
    return w;
}();

const int VARS = (int)W_OF.size();

using Assign = V<int>;  // target box id or -1(don't use)

bool is_dag_orig(const Assign& assign) {
    array<array<char, N>, N> has{};
    array<int, N> indeg{};
    array<V<int>, N> g{};
    for (int id = 0; id < VARS; id++) {
        int j = assign[id];
        if (j < 0) continue;
        int i = W_OF[id];
        if (!has[i][j]) {
            has[i][j] = 1;
            g[i].push_back(j);
            indeg[j]++;
        }
    }
    deque<int> q;
    for (int u = 0; u < N; u++) {
        if (indeg[u] == 0) q.push_back(u);
    }
    int seen = 0;
    while (!q.empty()) {
        int u = q.front();
        q.pop_front();
        seen++;
        for (int v : g[u]) {
            if (--indeg[v] == 0) q.push_back(v);
        }
    }
    return seen == N;
}

bool is_dag(const Assign& assign) {
    array<int, N> indeg = {};
    for (int id = 0; id < VARS; id++) {
        int j = assign[id];
        if (j < 0) continue;
        indeg[j]++;
    }
    array<int, N> st = {};
    int l = 0, r = 0;
    for (int u = 0; u < N; u++) {
        if (indeg[u] == 0) {
            st[r++] = u;
        }
    }
    if (r == N) return true;
    while (l != r) {
        int u = st[l++];
        for (int id = SUM_C[u]; id < SUM_C[u + 1]; id++) {
            int v = assign[id];
            if (v < 0) continue;
            if (--indeg[v] == 0) {
                st[r++] = v;
            }
        }
    }
    return r == N;
}

bool can_reach(const Assign& assign, int from, int to) {
    assert(from != to);

    array<bool, N> visited = {};
    array<int, N> st = {};

    int r = 0;
    visited[from] = true;
    st[r++] = from;

    for (int l = 0; l < r; l++) {
        int u = st[l];
        for (int id = SUM_C[u]; id < SUM_C[u + 1]; id++) {
            int v = assign[id];
            if (v < 0) continue;
            if (v == to) return true;
            if (!visited[v]) {
                visited[v] = true;
                st[r++] = v;
            }
        }
    }
    return false;
}

int calc_score(const Assign& assign) {
    array<int, N> used = {};
    int total = 0;
    for (int id = 0; id < VARS; id++) {
        int j = assign[id];
        if (j < 0) continue;

        total++;
        int i = W_OF[id];
        used[j] += A[i][j];
    }
    for (int j = 0; j < N; j++) {
        int use = max(H[j] - used[j], 0);
        total += use;
    }
    return total;
}

constexpr int WEIGHT_TABLE_BUCKETS = 4000;

struct WeightTable {
    array<array<int, WEIGHT_TABLE_BUCKETS>, N> buckets{};
    array<int, N> size{};
};

const WeightTable CUMW = []() {
    WeightTable wt;
    array<int, N> idx;
    for (int i : iota(0, N)) {
        idx[i] = i;
    }
    for (int i = 0; i < N; i++) {
        sort(idx.begin(), idx.end(), [&](int x, int y) {
            return A[i][x] > A[i][y];
        });
        int ptr = 0;
        for (int j : iota(0, N)) {
            int cnt = A[i][idx[j]];
            for (int _ : iota(0, cnt)) {
                wt.buckets[i][ptr++] = idx[j];
            }
        }
        assert(ptr <= WEIGHT_TABLE_BUCKETS);
        wt.size[i] = ptr;
    }
    return wt;
}();

static inline int sample_box(int weapon) {
    int r = random(0, CUMW.size[weapon] - 1, gen);
    return CUMW.buckets[weapon][r];
}

void print_answer(const Assign& assign) {
    array<array<int, N>, N> uses{};
    array<array<char, N>, N> edge{};
    array<int, N> indeg{};
    for (int id = 0; id < VARS; id++) {
        int j = assign[id];
        if (j < 0) continue;
        int i = W_OF[id];
        uses[i][j]++;
        if (!edge[i][j]) {
            edge[i][j] = 1;
            indeg[j]++;
        }
    }

    array<int, N> hp = H;

    deque<int> q;
    for (int u = 0; u < N; u++) {
        if (indeg[u] == 0) q.push_back(u);
    }

    V<pair<int, int>> actions;

    while (!q.empty()) {
        int i = q.front();
        q.pop_front();

        if (hp[i] > 0) {
            int need = hp[i];
            for (int k = 0; k < need; k++) {
                actions.emplace_back(-1, i);
            }
            hp[i] = 0;
        }

        for (int j = 0; j < N; j++) {
            int cnt_use = uses[i][j];
            if (cnt_use == 0) continue;
            int dmg = A[i][j];
            while (cnt_use > 0 && hp[j] > 0) {
                actions.emplace_back(i, j);
                hp[j] = max(0, hp[j] - dmg);
                cnt_use--;
            }
            uses[i][j] = cnt_use;
        }

        for (int j = 0; j < N; j++) {
            if (edge[i][j]) {
                if (--indeg[j] == 0) q.push_back(j);
            }
        }
    }

    for (int j = 0; j < N; j++) {
        while (hp[j] > 0) {
            actions.emplace_back(-1, j);
            hp[j]--;
        }
    }

    for (auto [w, b] : actions) {
        pr.write(w);
        pr.write(' ');
        pr.writeln(b);
    }
}

int main() {
    Assign assign(VARS, -1);
    int cur = calc_score(assign);
    int best = cur;
    Assign best_assign = assign;

    StopWatch sw;
    array<int, 4> cnt = {};

    array<int, N> weapon_dmg = {};
    int sum_weapon_dmg = 0;
    int sum_used_weapon = 0;

    auto effect = [&](int h, int dmg) {
        int u = min(h, dmg);
        //if (u >= 200) u = u * 12 / 10;
        return u;
    };
    auto cng_target = [&](int box, int weapon, int old_target, int new_target) {
        if (old_target != -1) {
            sum_used_weapon--;
            sum_weapon_dmg -= effect(H[old_target], weapon_dmg[old_target]);
            weapon_dmg[old_target] -= A[box][old_target];
            sum_weapon_dmg += effect(H[old_target], weapon_dmg[old_target]);
        }

        assign[weapon] = new_target;

        if (new_target != -1) {
            sum_used_weapon++;
            sum_weapon_dmg -= effect(H[new_target], weapon_dmg[new_target]);
            weapon_dmg[new_target] += A[box][new_target];
            sum_weapon_dmg += effect(H[new_target], weapon_dmg[new_target]);
        }
    };

    const double T0 = 100.0;
    const double T1 = 1.0;
    const double TL_MSEC = int(1970 * SPEED_FREQ); // * 2;

    while (true) {
        int elapsed = sw.msecs();
        if (elapsed >= TL_MSEC) break;

        double progress = 1.0 * elapsed / TL_MSEC;
        //double temp = T0 + (T1 - T0) * progress;
        double temp = T0 * pow(T1 / T0, progress);

        for (int _ : iota(0, 1000)) {
            cnt[0]++;

            // exp((cur - nxt) / temp) >= random01
            // cur - nxt >= temp * log(random01)
            // nxt <= cur - temp * log(random01)

            int threshold = cur - int(temp * log(random_01(gen)));

            int id = random(0, VARS - 1, gen);
            int weapon = W_OF[id];
            int old_target = assign[id];
            int new_target =
                (random(0, 999, gen) < 100) ? -1 : sample_box(weapon);
            if (new_target == weapon || new_target == old_target) continue;

            cnt[1]++;

            cng_target(weapon, id, old_target, new_target);

            int nxt = TOTAL_H - sum_weapon_dmg + sum_used_weapon;

            auto check_dag = [&]() {
                if (new_target == -1) return true;
                for (int i = SUM_C[new_target]; i < SUM_C[new_target + 1]; i++) {
                    if (i == id) continue;
                    if (assign[i] == new_target) {
                        return true;
                    }
                }
                if (can_reach(assign, new_target, weapon)) {
                    return false;
                }
                return true;
            };
            auto check = [&]() {
                if (nxt > threshold) return false;
                cnt[2]++;

                //assert(check_dag() == is_dag(assign));
                
                if (!check_dag()) return false;
                cnt[3]++;

                return true;
            };

            if (!check()) {
                //assign[id] = old_target;
                cng_target(weapon, id, new_target, old_target);
                continue;
            }

            cur = nxt;
            if (cur < best) {
                best = cur;
                best_assign = assign;
                //dbg(elapsed, progress, best);
            }
        }
    }
    std::cerr << "counter: " << cnt[0] << " " << cnt[1] << " " << cnt[2] << " " << cnt[3] << std::endl;

    print_answer(best_assign);

    return 0;
}
