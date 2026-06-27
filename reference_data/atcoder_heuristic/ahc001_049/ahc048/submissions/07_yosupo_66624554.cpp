// #undef YOSUPO_LOCAL

#include <xmmintrin.h>
#include <queue>
#include <ranges>
#if 0 and !defined(__clang__)
#include <vector>
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#pragma GCC optimize("Ofast")
#endif


#include <array>
#include <cassert>
#include <cstddef>
#include <string>
#include <utility>

namespace yosupo {

struct Coord {
  public:
    constexpr Coord() : d{0, 0} {}
    constexpr Coord(int _r, int _c) : d{_r, _c} {}
    constexpr Coord(const std::pair<int, int>& p) : d{p.first, p.second} {}

    bool operator==(const Coord& other) const { return d == other.d; }

    Coord& operator+=(const Coord& other) {
        d[0] += other.d[0];
        d[1] += other.d[1];
        return *this;
    }
    Coord operator+(const Coord& other) const {
        Coord result = *this;
        result += other;
        return result;
    }

    Coord& operator-=(const Coord& other) {
        d[0] -= other.d[0];
        d[1] -= other.d[1];
        return *this;
    }
    Coord operator-(const Coord& other) const {
        Coord result = *this;
        result -= other;
        return result;
    }

    int& r() { return d[0]; }
    int& c() { return d[1]; }
    const int& r() const { return d[0]; }
    const int& c() const { return d[1]; }

    int& operator[](int index) {
        if (index == 0) return d[0];
        if (index == 1) return d[1];
        assert(false);
    }
    const int& operator[](int index) const {
        if (index == 0) return d[0];
        if (index == 1) return d[1];
        assert(false);
    }

    operator std::pair<int, int>() const { return std::make_pair(d[0], d[1]); }

    std::string dump() const {
        return "(" + std::to_string(d[0]) + ", " + std::to_string(d[1]) + ")";
    }

    Coord move4(int dir) const {
        static constexpr std::array<Coord, 4> d4 = {Coord(0, 1), Coord(1, 0),
                                                    Coord(0, -1), Coord(-1, 0)};
        return *this + d4[dir];
    }

    auto move4() const {
        return std::views::iota(0, 4) | std::views::transform([this](int dir) {
                   return this->move4(dir);
               });
    }

    Coord move8(int dir) const {
        static constexpr std::array<Coord, 8> d8 = {
            Coord(0, 1),  Coord(1, 1),   Coord(1, 0),  Coord(1, -1),
            Coord(0, -1), Coord(-1, -1), Coord(-1, 0), Coord(-1, 1)};
        assert(0 <= dir && dir < 8);
        return *this + d8[dir];
    }

    auto move8() const {
        return std::views::iota(0, 8) | std::views::transform([this](int dir) {
                   return this->move8(dir);
               });
    }

    bool contain(const Coord& t) const {
        return 0 <= t.r() && t.r() < r() && 0 <= t.c() && t.c() < c();
    }

    struct CellsRangeView {
        struct Iterator {
            using value_type = Coord;
            using difference_type = std::ptrdiff_t;

            int h, w, r, c;

            value_type operator*() const { return Coord{r, c}; }

            Iterator& operator++() {
                if (++c == w) {
                    c = 0;
                    ++r;
                }
                return *this;
            }
            Iterator operator++(int) {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const Iterator& other) const {
                return r == other.r && c == other.c;
            }
        };

        Iterator begin() const { return Iterator{h, w, 0, 0}; }
        Iterator end() const { return Iterator{h, w, h, 0}; }
        int h, w;
    };
    CellsRangeView cells() const { return CellsRangeView{r(), c()}; }

  private:
    std::array<int, 2> d;
};

}  // namespace yosupo

#include <algorithm>
#include <concepts>
#include <map>
#include <set>
#include <tuple>
#include <vector>


#include <cstdint>

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

}  // namespace yosupo

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

#include <stdio.h>
#include <unistd.h>

#include <bit>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <type_traits>



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
#include <limits>
#include <random>


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





const double EPS = 1e-8;
const double ALLOW_ERR = 0.9 * 1e-6;

inline int cmp(double x, double y) {
    if (std::abs(x - y) < EPS) return 0;
    return (x < y) ? -1 : 1;
}
inline bool less(double x, double y) { return cmp(x, y) < 0; }
inline bool equal(double x, double y) { return cmp(x, y) == 0; }
inline bool greater(double x, double y) { return cmp(x, y) > 0; }

struct Point3 {
    Point3() : p({}) {}
    Point3(const std::array<double, 3>& _p) : p(_p) {}

    double& operator[](int i) { return p[i]; }
    const double& operator[](int i) const { return p[i]; }

    friend Point3 operator+(const Point3& a, const Point3& b) {
        return Point3({a.p[0] + b.p[0], a.p[1] + b.p[1], a.p[2] + b.p[2]});
    }
    friend Point3 operator-(const Point3& a, const Point3& b) {
        return Point3({a.p[0] - b.p[0], a.p[1] - b.p[1], a.p[2] - b.p[2]});
    }
    friend Point3 operator*(const Point3& a, double b) {
        return Point3({a.p[0] * b, a.p[1] * b, a.p[2] * b});
    }
    friend Point3 operator/(const Point3& a, double b) {
        assert(!equal(b, 0));
        return Point3({a.p[0] / b, a.p[1] / b, a.p[2] / b});
    }
    Point3& operator+=(const Point3& other) { return *this = *this + other; }
    Point3& operator-=(const Point3& other) { return *this = *this - other; }
    Point3& operator*=(double b) { return *this = *this * b; }
    Point3& operator/=(double b) { return *this = *this / b; }

    Point3 operator-() const {
        return Point3({-p[0], -p[1], -p[2]});
    }

    double abs2() const { return p[0] * p[0] + p[1] * p[1] + p[2] * p[2]; }
    double abs() const { return std::sqrt(abs2()); }

    std::string dump() const {
        return "[" + std::to_string(p[0]) + ", " + std::to_string(p[1]) + ", " + std::to_string(p[2]) + "]";
    }

  private:
    std::array<double, 3> p;
};
inline Point3 crs(const Point3& a, const Point3& b) {
    return Point3({
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    });
}
inline double dot(const Point3& a, const Point3& b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
inline double det(const std::array<Point3, 3>& ps) { return dot(ps[0], crs(ps[1], ps[2])); }

inline double dist(const Point3& a, const Point3& b) { return (a - b).abs(); }

struct Pool {
    Pool() : c_a({0, 0, 0}), a(0) {}
    explicit Pool(const Point3& _c, double _a = 1.0) : c_a(_c * _a), a(_a) {}

    Point3 color() const { return equal(a, 0) ? Point3() : c_a / a; }
    double amount() const { return a; }

    Pool& operator*=(double n) {
        c_a *= n;
        a *= n;
        return *this;
    }
    Pool operator*(double n) const { return Pool(*this) *= n; }
    Pool& operator/=(double n) {
        assert(!equal(n, 0));
        c_a /= n;
        a /= n;
        return *this;        
    }
    Pool operator/(double n) const { return Pool(*this) /= n; }

    friend Pool operator+(const Pool& l, const Pool& r) {
        Pool res;
        res.c_a = l.c_a + r.c_a;
        res.a = l.a + r.a;
        return res;
    }
    Pool& operator+=(const Pool& other) {
        return *this = *this + other;
    }

    Pool picked() const {
        assert(a > 1 - ALLOW_ERR);
        double new_a = std::max(a - 1, 0.0);
        if (equal(new_a, 0)) {
            return Pool();
        }
        return *this * (new_a / a);
    }

    std::string dump() const {
        auto c = color();
        return "[" + c.dump() + " : " + std::to_string(a) + "]";
    }
  private:
    Point3 c_a; // color * amount
    double a; // amount
};

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

inline yosupo::Scanner scanner = yosupo::Scanner(stdin);
inline yosupo::Printer printer = yosupo::Printer(stdout);

const int TL = 2800;

// grid size
const int N = 20;
const yosupo::Coord BOARD_SIZE = yosupo::Coord(N, N);

// number of colors
const int M = []() {
    int dummy, m;
    scanner.read(dummy, m);
    return m;
}();

// number of queries
const int Q = 1000;

// number of max turns
const int TURN = []() {
    int dummy, t;
    scanner.read(dummy, t);
    return t;
}();


const int RAW_D = []() {
    int d;
    scanner.read(d);
    return d;
}();
// cost of an add operation
const double ADD_COST = RAW_D / 10000.0;

const std::vector<Point3> COLORS = []() {
    std::vector<Point3> colors(M);
    for (int i = 0; i < M; i++) {
        std::string x, y, z;
        scanner.read(x, y, z);
        colors[i][0] = stod(x);
        colors[i][1] = stod(y);
        colors[i][2] = stod(z);
    }
    return colors;
}();

const std::array<Point3, Q> TARGETS = []() {
    std::array<Point3, Q> targets;
    for (int i = 0; i < Q; i++) {
        std::string x, y, z;
        scanner.read(x, y, z);
        targets[i][0] = stod(x);
        targets[i][1] = stod(y);
        targets[i][2] = stod(z);
    }
    return targets;
}();

// global utils
inline yosupo::Random gen = yosupo::Random(12345);
inline yosupo::StopWatch global_sw = yosupo::StopWatch();

#include <numeric>



#include <iostream>


template <class T, int B = (1 << 11)> struct RatioFinder {
    std::vector<double> keys;
    std::vector<T> values;
    std::array<int, B + 1> bucket_start;

    RatioFinder(const std::vector<std::pair<double, T>>& entries) {
        for (const auto& entry : entries) {
            keys.push_back(entry.first);
            values.push_back(entry.second);
        }

        // values should be sorted
        assert(std::is_sorted(keys.begin(), keys.end()));
        // ratios should be in [0, 1]
        assert(!keys.empty() && 0.0 <= keys.front() &&
               keys.back() <= 1.0);

        // precompute bucket boundaries
        int j = 0;
        for (int i = 0; i <= B; i++) {
            double bucket_val = 1.0 * i / B;
            while (j < int(this->keys.size()) &&
                   this->keys[j] < bucket_val) {
                j++;
            }
            bucket_start[i] = j;
        }
    }

    int find_nearest_idx(double x) const {
        if (x <= keys.front()) {
            return 0;
        }
        if (keys.back() <= x) {
            return int(keys.size()) - 1;
        }

        int bucket = std::min(B - 1, int(x * B));
        int start = bucket_start[bucket];
        int end = bucket_start[bucket + 1];

        auto it = lower_bound(keys.begin() + start, keys.begin() + end, x);
        auto prev = it - 1;
        assert(*prev <= x && x <= *it);
        if (x - *prev <= *it - x) {
            return int(prev - keys.begin());
        } else {
            return int(it - keys.begin());
        }
    }

    double find_nearest(double x) const { return keys[find_nearest_idx(x)]; }

    std::pair<double, T> find_nearest_entry(double x) const {
        int idx = find_nearest_idx(x);
        return {keys[idx], values[idx]};
    }
};


inline void print_point_log(Point3 target, double dist) {
    std::cerr << "POINT: " << target[0] << " " << target[1] << " " << target[2]
              << " " << dist << std::endl;
}

struct OP {
    int ty = 0;                  // nop, add, pick, drop, wall
    yosupo::Coord p = {-1, -1};  // position
    int c = -1;                  // for add
    int dir = -1;                // for wall

    static OP nop() { return {.ty = 0}; }
    static OP add(const yosupo::Coord& p, int c) {
        assert(BOARD_SIZE.contain(p));
        return {.ty = 1, .p = p, .c = c};
    }
    static OP pick(const yosupo::Coord& p) {
        assert(BOARD_SIZE.contain(p));
        return {.ty = 2, .p = p};
    }
    static OP drop(const yosupo::Coord& p) {
        assert(BOARD_SIZE.contain(p));
        return {.ty = 3, .p = p};
    }
    static OP wall(const yosupo::Coord& p, int dir) {
        assert(0 <= dir && dir < 4);
        assert(yosupo::Coord(N, N).contain(p));
        assert(yosupo::Coord(N, N).contain(p.move4(dir)));
        return {.ty = 4, .p = p, .dir = dir};
    }
    static OP wall(const yosupo::Coord& p0, const yosupo::Coord& p1) {
        for (int d = 0; d < 4; d++) {
            if (p0.move4(d) == p1) {
                return wall(p0, d);
            }
        }
        assert(false);
    }

    std::string dump() const {
        if (ty == 0) {
            return "OP(nop)";
        } else if (ty == 1) {
            return "OP(add:" + p.dump() + ", " + std::to_string(c) + ")";
        } else if (ty == 2) {
            return "OP(pick:" + p.dump() + ")";
        } else if (ty == 3) {
            return "OP(drop:" + p.dump() + ")";
        } else if (ty == 4) {
            return "OP(wall:" + p.dump() + ", " + std::to_string(dir) + ")";
        }
        assert(false);
    }
};

struct Board {
    std::array<std::array<bool, 2 * N - 1>, 2 * N - 1> walls = {};

    // return the wall is added or not
    bool flip(const yosupo::Coord& p1, const yosupo::Coord& p2) {
        auto diff = p1 - p2;
        assert(abs(diff.r()) + abs(diff.c()) ==
               1);  // p1 and p2 must be 4-adjacent

        walls[p1.r() + p2.r()][p1.c() + p2.c()] =
            !walls[p1.r() + p2.r()][p1.c() + p2.c()];
        return walls[p1.r() + p2.r()][p1.c() + p2.c()];
    }

    bool has_wall(const yosupo::Coord& p1, const yosupo::Coord& p2) const {
        auto diff = p1 - p2;
        assert(abs(diff.r()) + abs(diff.c()) == 1);
        return walls[p1.r() + p2.r()][p1.c() + p2.c()];
    }

    static Board all_walls() {
        Board board;
        for (int r = 0; r < N - 1; r++) {
            for (int c = 0; c < N; c++) {
                board.flip(yosupo::Coord(r, c), yosupo::Coord(r + 1, c));
            }
        }
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N - 1; c++) {
                board.flip(yosupo::Coord(r, c), yosupo::Coord(r, c + 1));
            }
        }
        return board;
    }
};

struct Answer {
    Board init;
    std::vector<OP> ops;
};

struct Simulator {
    Board board;
    std::vector<OP> ops;  // operations (filter out nop)

    Pool get_point_pool(const yosupo::Coord& c) const {
        assert(BOARD_SIZE.contain(c));
        int id = ids[c.r()][c.c()];
        return pools[id] / caps[id];
    }

    Simulator(const Board& initial_board) : board(initial_board) {
        update();
        pools.assign(caps.size(), Pool());
    }

    void action(const OP& op, bool debug = false) {
        if (op.ty == 0) return;
        turn++;

        ops.push_back(op);

        if (op.ty == 1) {
            num_add_ops++;

            int id = ids[op.p.r()][op.p.c()];
            double current_vol = pools[id].amount();
            double capacity = caps[id];
            double remaining_cap = capacity - current_vol;

            double add_vol = std::min(1.0, std::max(0.0, remaining_cap));
            pools[id] += Pool(COLORS[op.c], add_vol);
        } else if (op.ty == 2) {
            assert(num_pick_ops < int(TARGETS.size()));

            int id = ids[op.p.r()][op.p.c()];
            double cur_dist = dist(pools[id].color(), TARGETS[num_pick_ops]);
            if (debug) {
                print_point_log(TARGETS[num_pick_ops], cur_dist);
            }
            total_dist += cur_dist;
            pools[id] = pools[id].picked();

            num_pick_ops++;
        } else if (op.ty == 3) {
            int id = ids[op.p.r()][op.p.c()];
            pools[id] = Pool(pools[id].color(),
                             std::max(0.0, pools[id].amount() - 1.0));
        } else if (op.ty == 4) {
            const yosupo::Coord& p0 = op.p;
            const yosupo::Coord& p1 = op.p.move4(op.dir);

            auto pre_ids = ids;
            auto pre_pools = pools;
            auto pre_caps = caps;
            int pre_id0 = pre_ids[p0.r()][p0.c()];
            int pre_id1 = pre_ids[p1.r()][p1.c()];

            board.flip(op.p, op.p.move4(op.dir));
            update();
            pools = std::vector<Pool>(this->caps.size());

            int new_id0 = this->ids[p0.r()][p0.c()];
            int new_id1 = this->ids[p1.r()][p1.c()];

            bool is_split = (pre_id0 == pre_id1) && (new_id0 != new_id1);
            bool is_merge = (pre_id0 != pre_id1) && (new_id0 == new_id1);

            if (is_split) {
                const auto& pool = pre_pools[pre_id0];
                int c0 = this->caps[new_id0];
                int c1 = this->caps[new_id1];

                pools[new_id0] = pool * (1.0 * c0 / (c0 + c1));
                pools[new_id1] = pool * (1.0 * c1 / (c0 + c1));
            } else if (is_merge) {
                pools[new_id0] = pre_pools[pre_id0] + pre_pools[pre_id1];
            }

            std::vector<bool> visited(int(pre_caps.size()), false);
            if (is_split) {
                visited[pre_id0] = true;
            }
            if (is_merge) {
                visited[pre_id0] = true;
                visited[pre_id1] = true;
            }

            for (const auto& p : BOARD_SIZE.cells()) {
                int r = p.r();
                int c = p.c();
                int pre_id = pre_ids[r][c];
                if (visited[pre_id]) continue;
                visited[pre_id] = true;
                int new_id = this->ids[r][c];
                pools[new_id] = pre_pools[pre_id];
            }
        } else {
            assert(false);
        }
    }

    int remaining_amount() const { return num_add_ops - num_pick_ops; }

    double dist_sum() const { return total_dist; }

    int score() const {
        int term_ops_cost = RAW_D * remaining_amount();
        int term_dist_cost = (int)(std::round(10000.0 * dist_sum()));
        int current_score = 1 + term_ops_cost + term_dist_cost;

        return current_score;
    }

    int turn = 0;
    double total_dist = 0.0;
    int num_add_ops = 0;
    int num_pick_ops = 0;

  private:
    std::array<std::array<int, N>, N> ids;  // Component ID for each cell (r, c)
    std::vector<int> caps;  // Capacity (number of cells) for each component ID
    std::vector<Pool> pools;  // Pool (color, amount) for each component ID

    void update() {
        for (const auto& p : BOARD_SIZE.cells()) {
            ids[p.r()][p.c()] = -1;
        }
        caps.clear();

        int counter = 0;
        std::vector<yosupo::Coord> que;
        for (const auto& p : BOARD_SIZE.cells()) {
            if (ids[p.r()][p.c()] != -1) continue;

            que.clear();
            que.push_back(p);

            ids[p.r()][p.c()] = counter;

            int head = 0;
            while (head < int(que.size())) {
                yosupo::Coord curr = que[head++];
                for (int d = 0; d < 4; d++) {
                    yosupo::Coord next_p = curr.move4(d);
                    if (!yosupo::Coord(N, N).contain(next_p)) continue;
                    if (ids[next_p.r()][next_p.c()] != -1) continue;

                    yosupo::Coord wall_coord =
                        yosupo::Coord(curr.r() * 2, curr.c() * 2).move4(d);
                    bool wall_exists =
                        board.walls[wall_coord.r()][wall_coord.c()];
                    if (wall_exists) continue;
                    ids[next_p.r()][next_p.c()] = counter;
                    que.push_back(next_p);
                }
            }

            caps.push_back(int(que.size()));
            counter++;
        }
    }
};

inline void print_answer(const Answer& answer) {
    const auto& init = answer.init;
    // print vertical walls
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N - 1; c++) {
            if (c) printer.write(' ');
            printer.write(
                init.has_wall(yosupo::Coord(r, c), yosupo::Coord(r, c + 1))
                    ? 1
                    : 0);
        }
        printer.writeln();
    }
    // print horizontal walls
    for (int r = 0; r < N - 1; r++) {
        for (int c = 0; c < N; c++) {
            if (c) printer.write(' ');
            printer.write(
                init.has_wall(yosupo::Coord(r, c), yosupo::Coord(r + 1, c))
                    ? 1
                    : 0);
        }
        printer.writeln();
    }
    // print operations
    const auto& ops = answer.ops;
    for (const auto& op : ops) {
        if (op.ty == 0) {
            // nop
            continue;
        } else if (op.ty == 1) {  // add
            printer.write(1, ' ', op.p.r(), ' ', op.p.c(), ' ', op.c);
        } else if (op.ty == 2) {  // pick (deliver)
            printer.write(2, ' ', op.p.r(), ' ', op.p.c());
        } else if (op.ty == 3) {  // drop (discard)
            printer.write(3, ' ', op.p.r(), ' ', op.p.c());
        } else if (op.ty == 4) {  // wall
            auto c2 = op.p.move4(op.dir);
            printer.write(4, ' ', op.p.r(), ' ', op.p.c(), ' ', c2.r(), ' ',
                          c2.c());
        }
        printer.writeln();
    }
    printer.flush();
}


#include <bitset>


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






namespace Board7 {

const int K = 7;
const int L = 57;

// Generated from boards/7.png
const std::array<std::string, N> VERTICAL_WALLS = {
    "0010000000000000000", "1100101000000000101", "1101111100000001011",
    "1111111110101000011", "1111111111111100001", "1111111111110101000",
    "1111111111110101000", "1111111111111110100", "1111111111111101100",
    "1111110111111100100", "1011110111011101010", "1011111111001001001",
    "1111111111100000001", "1111111111011000011", "1111011110001100111",
    "1111011100011111111", "1111111000010111111", "1111110000000011111",
    "1110100000000001111", "0100000000000000100",
};
const std::array<std::string, N> HORIZONTAL_WALLS = {
    "01111111111111111110", "00110000111111111100", "00000000011111111000",
    "00000000000000111100", "00000000000000011110", "00000000000011100111",
    "00000000000000011110", "00000000000000000011", "00000000000000110110",
    "00000011000000011011", "01100000001100100100", "00000000000111111110",
    "00000000000011111100", "00000000001110111000", "00001100011100010000",
    "00000000111000001000", "00000001111111000000", "00000011111111100100",
    "00011111111111110010",
};

const Board INIT_BOARD = []() {
    Board board;
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N - 1; c++) {
            if (VERTICAL_WALLS[r][c] == '1') {
                board.flip(yosupo::Coord(r, c), yosupo::Coord(r, c + 1));
            }
        }
    }
    for (int r = 0; r < N - 1; r++) {
        for (int c = 0; c < N; c++) {
            if (HORIZONTAL_WALLS[r][c] == '1') {
                board.flip(yosupo::Coord(r, c), yosupo::Coord(r + 1, c));
            }
        }
    }
    return board;
}();

const std::set<std::pair<int, int>> EDGE0 = {{0, 1}, {0, 3}, {0, 5}, {1, 4},
                                             {1, 6}, {2, 3}, {2, 5}, {3, 4},
                                             {3, 6}, {5, 6}};
const std::set<std::pair<int, int>> EDGE1 = {{0, 2}, {0, 6}, {4, 6}};
const std::set<std::pair<int, int>> EDGE2 = {{1, 3}, {5, 3}};
const std::set<std::pair<int, int>> EDGE3 = {{3, 1}, {3, 5}};

const std::array<std::array<yosupo::Coord, L>, K> PATHS = []() {
    std::array<std::array<yosupo::Coord, L>, K> paths;
    int pcount = 0;

    std::array<std::array<bool, N>, N> vis = {};
    for (auto p : BOARD_SIZE.cells()) {
        if (vis[p.r()][p.c()]) continue;
        int count = 0;
        for (auto q : p.move4()) {
            if (!BOARD_SIZE.contain(q)) continue;
            if (INIT_BOARD.has_wall(p, q)) continue;
            if (!vis[q.r()][q.c()]) {
                count++;
            }
        }
        if (count != 1) continue;

        std::vector<yosupo::Coord> path;
        while (true) {
            path.push_back(p);
            vis[p.r()][p.c()] = true;
            bool upd = false;
            for (auto q : p.move4()) {
                if (!BOARD_SIZE.contain(q)) continue;
                if (INIT_BOARD.has_wall(p, q)) continue;
                if (vis[q.r()][q.c()]) continue;
                upd = true;
                p = q;
                break;
            }
            if (!upd) break;
        }
        assert(path.size() == L);
        std::copy(path.begin(), path.end(), paths[pcount].begin());
        pcount++;
    }
    assert(pcount == K);

    auto check11 = [&](int l, int r) {
        for (auto p : {paths[l][0], paths[l][L - 1]}) {
            for (auto q : {paths[r][0], paths[r][L - 1]}) {
                if (p == q) continue;
                auto diff = p - q;
                if (abs(diff.r()) + abs(diff.c()) == 1) {
                    return true;
                }
            }
        }
        return false;
    };
    auto check12 = [&](int l, int r) {
        for (auto p : {paths[l][0], paths[l][L - 1]}) {
            for (auto q : {paths[r][1], paths[r][L - 2]}) {
                if (p == q) continue;
                auto diff = p - q;
                if (abs(diff.r()) + abs(diff.c()) == 1) {
                    return true;
                }
            }
        }
        return false;
    };

    for (auto [l, r] : EDGE0) {
        assert(check11(l, r));
    }
    for (auto [l, r] : EDGE1) {
        assert(check12(l, r) && check12(r, l));
    }
    for (auto [l, r] : EDGE2) {
        assert(check12(l, r));
    }
    for (auto [l, r] : EDGE3) {
        assert(check12(r, l));
    }
    return paths;
}();

const std::vector<std::pair<double, std::pair<int, int>>> MIX_RATIOS0 = []() {
    std::vector<std::pair<double, std::pair<int, int>>> mix_ratios;
    mix_ratios.push_back({1.0, {0, 0}});
    for (int l = 1; l <= L; l++) {
        for (int r = 1; r <= L; r++) {
            double lf = 1.0 * l / L;
            double rf = 1.0 * r / L;
            double rate = lf * rf / (lf + rf);
            mix_ratios.push_back({1.0 - rate, {l, r}});
        }
    }
    std::ranges::sort(mix_ratios);
    return mix_ratios;
}();

const std::vector<std::pair<double, std::pair<int, int>>> MIX_RATIOS1 = []() {
    // filter (1, 1) from MIX_RATIOS0
    auto mix_ratios = MIX_RATIOS0;
    std::erase_if(mix_ratios, [](const auto& p) {
        return p.second == std::make_pair(1, 1);
    });
    return mix_ratios;
}();

const std::vector<std::pair<double, std::pair<int, int>>> MIX_RATIOS2 = []() {
    // filter (*, 1) from MIX_RATIOS0
    auto mix_ratios = MIX_RATIOS0;
    std::erase_if(mix_ratios,
                  [](const auto& p) { return p.second.second == 1; });
    return mix_ratios;
}();

const std::vector<std::pair<double, std::pair<int, int>>> MIX_RATIOS3 = []() {
    // filter (1, *) from MIX_RATIOS0
    auto mix_ratios = MIX_RATIOS0;
    std::erase_if(mix_ratios,
                  [](const auto& p) { return p.second.first == 1; });
    return mix_ratios;
}();

const std::vector<std::pair<double, std::pair<int, int>>> HALF_MIX_RATIOS0 =
    []() {
        auto mix_ratios = MIX_RATIOS0;
        std::erase_if(mix_ratios, [](const auto& p) {
            return (p.second.first != L && p.second.second != L);
        });
        return mix_ratios;
    }();

const std::vector<std::pair<double, std::pair<int, int>>> HALF_MIX_RATIOS1 =
    []() {
        // filter (1, 1) from MIX_RATIOS0
        auto mix_ratios = HALF_MIX_RATIOS0;
        std::erase_if(mix_ratios, [](const auto& p) {
            return p.second == std::make_pair(1, 1);
        });
        return mix_ratios;
    }();

const std::vector<std::pair<double, std::pair<int, int>>> HALF_MIX_RATIOS2 =
    []() {
        // filter (*, 1) from MIX_RATIOS0
        auto mix_ratios = HALF_MIX_RATIOS0;
        std::erase_if(mix_ratios,
                      [](const auto& p) { return p.second.second == 1; });
        return mix_ratios;
    }();

const std::vector<std::pair<double, std::pair<int, int>>> HALF_MIX_RATIOS3 =
    []() {
        // filter (1, *) from MIX_RATIOS0
        auto mix_ratios = HALF_MIX_RATIOS0;
        std::erase_if(mix_ratios,
                      [](const auto& p) { return p.second.first == 1; });
        return mix_ratios;
    }();

inline auto to_values =
    [](const std::vector<std::pair<double, std::pair<int, int>>>& mix_ratios) {
        std::vector<double> values;
        values.reserve(mix_ratios.size());
        for (const auto& p : mix_ratios) {
            values.push_back(p.first);
        }
        return values;
    };

const std::array<std::vector<std::pair<double, std::pair<int, int>>>, 4>
    MIX_RATIOS = {MIX_RATIOS0, MIX_RATIOS1, MIX_RATIOS2, MIX_RATIOS3};

const std::array<RatioFinder<std::pair<int, int>>, 4> RATIO_FINDERS = {
    RatioFinder<std::pair<int, int>>(MIX_RATIOS0),
    RatioFinder<std::pair<int, int>>(MIX_RATIOS1),
    RatioFinder<std::pair<int, int>>(MIX_RATIOS2),
    RatioFinder<std::pair<int, int>>(MIX_RATIOS3),
};

const std::array<RatioFinder<std::pair<int, int>>, 4> HALF_RATIO_FINDERS = {
    RatioFinder<std::pair<int, int>>(HALF_MIX_RATIOS0),
    RatioFinder<std::pair<int, int>>(HALF_MIX_RATIOS1),
    RatioFinder<std::pair<int, int>>(HALF_MIX_RATIOS2),
    RatioFinder<std::pair<int, int>>(HALF_MIX_RATIOS3),
};

struct MergeAct {
    int len;
    std::array<int, 4> pool_id;
    std::array<int, 3> par, ratio;
};

const std::vector<MergeAct> MERGE_ACTIONS = []() {
    std::vector<MergeAct> merge_actions;
    std::array<int, 4> pool_id;
    std::array<int, 3> par;
    auto dfs = [&](auto self, int i) -> void {
        if (i == 4) {
            // pool_id should be unique
            if (pool_id[0] == -1) return;
            for (int l = 0; l < 4; l++) {
                for (int r = l + 1; r < 4; r++) {
                    if (pool_id[l] != -1 && pool_id[l] == pool_id[r]) return;
                    if (pool_id[l] == -1 && pool_id[r] != -1) return;
                }
            }
            for (int j = 1; j < 4; j++) {
                if (pool_id[j] == -1 && par[j - 1] != 0) return;
            }

            int len = 0;
            for (auto id : pool_id) {
                if (id != -1) {
                    len++;
                }
            }
            std::array<int, 3> ratio = {-1, -1, -1};
            for (int j = 1; j < 4; j++) {
                int l = pool_id[par[j - 1]], r = pool_id[j];
                if (r == -1) continue;
                if (EDGE0.contains({l, r}) || EDGE0.contains({r, l})) {
                    ratio[j - 1] = 0;
                } else if (EDGE1.contains({l, r}) || EDGE1.contains({r, l})) {
                    ratio[j - 1] = 1;
                } else if (EDGE2.contains({l, r})) {
                    ratio[j - 1] = 2;
                } else if (EDGE3.contains({l, r})) {
                    ratio[j - 1] = 3;
                } else {
                    return;
                }
            }

            merge_actions.push_back(
                {.len = len, .pool_id = pool_id, .par = par, .ratio = ratio});
            return;
        }
        for (int k = -1; k < K; k++) {
            pool_id[i] = k;
            if (i == 0) {
                self(self, i + 1);
            } else {
                for (int j = 0; j < i; j++) {
                    par[i - 1] = j;
                    self(self, i + 1);
                }
            }
        }
    };
    dfs(dfs, 0);
    return merge_actions;
}();

const std::vector<MergeAct> MERGE_ACTIONS_PAR0 = []() {
    auto merge_actions = MERGE_ACTIONS;
    std::erase_if(merge_actions, [](const MergeAct& act) {
        return !(act.par[0] <= 0 && act.par[1] <= 0 && act.par[2] <= 0);
    });
    return merge_actions;
}();

const std::vector<MergeAct> MERGE_ACTIONS_THREE = []() {
    auto merge_actions = MERGE_ACTIONS;
    std::erase_if(merge_actions,
                  [](const MergeAct& act) { return act.pool_id[2] != -1; });
    return merge_actions;
}();

const std::vector<MergeAct> MERGE_ACTIONS_THREE_PAR0 = []() {
    auto merge_actions = MERGE_ACTIONS_PAR0;
    std::erase_if(merge_actions,
                  [](const MergeAct& act) { return act.pool_id[2] != -1; });
    return merge_actions;
}();

inline void approx(std::array<Point3, K>& pools,
                   const Point3& target,
                   const MergeAct& act) {
    for (int i = 0; i < 4; i++) {
        if (act.pool_id[i] == -1) {
            continue;
        }
        pools[act.pool_id[i]] -= target;
    }

    if (act.pool_id[3] != -1) {
        const int& par = act.par[2];
        assert(0 <= par && par <= 2);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[3]];
        const auto& crs_rem = crs(pools[act.pool_id[(par + 1) % 3]],
                                  pools[act.pool_id[(par + 2) % 3]]);

        double dot0 = dot(p0 - p1, crs_rem);
        double dot1 = dot(p1, crs_rem);

        if (!equal(dot0, 0)) {
            double x = -dot1 / dot0;
            auto res = RATIO_FINDERS[act.ratio[2]].find_nearest(x);
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }
    if (act.pool_id[2] != -1) {
        const int& par = act.par[1];
        assert(0 <= par && par <= 1);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[2]];
        const Point3& u1 = crs(p1 - p0, pools[act.pool_id[par ^ 1]]);
        const Point3& u2 = crs(p1, pools[act.pool_id[par ^ 1]]);

        if (!equal(u1.abs(), 0.0)) {
            double x = dot(u1, u2) / (u1.abs() * u1.abs());

            auto res = RATIO_FINDERS[act.ratio[1]].find_nearest(x);
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }
    if (act.pool_id[1] != -1) {
        const int& par = act.par[0];
        assert(par == 0);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[1]];

        Point3 u = p1 - p0;
        if (!equal(u.abs2(), 0.0)) {
            double x = dot(p1, u) / u.abs2();

            auto res = RATIO_FINDERS[act.ratio[0]].find_nearest(x);
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (act.pool_id[i] == -1) {
            continue;
        }
        pools[act.pool_id[i]] += target;
    }
}

inline void approx_half(std::array<Point3, K>& pools,
                        const Point3& target,
                        const MergeAct& act) {
    for (int i = 0; i < 4; i++) {
        if (act.pool_id[i] == -1) {
            continue;
        }
        pools[act.pool_id[i]] -= target;
    }

    if (act.pool_id[3] != -1) {
        const int& par = act.par[2];
        assert(0 <= par && par <= 2);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[3]];
        const auto& crs_rem = crs(pools[act.pool_id[(par + 1) % 3]],
                                  pools[act.pool_id[(par + 2) % 3]]);

        double dot0 = dot(p0 - p1, crs_rem);
        double dot1 = dot(p1, crs_rem);

        if (!equal(dot0, 0)) {
            double x = -dot1 / dot0;
            auto res = HALF_RATIO_FINDERS[act.ratio[2]].find_nearest(x);
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }
    if (act.pool_id[2] != -1) {
        const int& par = act.par[1];
        assert(0 <= par && par <= 1);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[2]];
        const Point3& u1 = crs(p1 - p0, pools[act.pool_id[par ^ 1]]);
        const Point3& u2 = crs(p1, pools[act.pool_id[par ^ 1]]);

        if (!equal(u1.abs(), 0.0)) {
            double x = dot(u1, u2) / (u1.abs() * u1.abs());

            auto res = HALF_RATIO_FINDERS[act.ratio[1]].find_nearest(x);
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }
    if (act.pool_id[1] != -1) {
        const int& par = act.par[0];
        assert(par == 0);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[1]];

        Point3 u = p1 - p0;
        if (!equal(u.abs2(), 0.0)) {
            double x = dot(p1, u) / u.abs2();

            auto res = HALF_RATIO_FINDERS[act.ratio[0]].find_nearest(x);
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (act.pool_id[i] == -1) {
            continue;
        }
        pools[act.pool_id[i]] += target;
    }
}

inline void approx_twice(std::array<Point3, K>& pools,
                         const Point3& target,
                         const MergeAct& act) {
    for (int i = 0; i < 4; i++) {
        if (act.pool_id[i] == -1) {
            continue;
        }
        pools[act.pool_id[i]] -= target;
    }

    auto find = [&](const RatioFinder<std::pair<int, int>>& finder, double x) {
        double y = 1e9;
        for (auto& y0 : finder.keys) {
            // y0 * y1 = x
            // y1 = x / y0
            double y1 = finder.find_nearest(x / y0);
            if (std::abs(y0 * y1 - x) < std::abs(y - x)) {
                y = y0 * y1;
            }
        }
        return y;
    };

    if (act.pool_id[3] != -1) {
        const int& par = act.par[2];
        assert(0 <= par && par <= 2);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[3]];
        const auto& crs_rem = crs(pools[act.pool_id[(par + 1) % 3]],
                                  pools[act.pool_id[(par + 2) % 3]]);

        double dot0 = dot(p0 - p1, crs_rem);
        double dot1 = dot(p1, crs_rem);

        if (!equal(dot0, 0)) {
            double x = -dot1 / dot0;
            // auto res = RATIO_FINDERS[act.ratio[2]].find_nearest(x);
            auto res = find(RATIO_FINDERS[act.ratio[2]], x);
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }
    if (act.pool_id[2] != -1) {
        const int& par = act.par[1];
        assert(0 <= par && par <= 1);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[2]];
        const Point3& u1 = crs(p1 - p0, pools[act.pool_id[par ^ 1]]);
        const Point3& u2 = crs(p1, pools[act.pool_id[par ^ 1]]);

        if (!equal(u1.abs(), 0.0)) {
            double x = dot(u1, u2) / u1.abs2();

            // auto res = RATIO_FINDERS[act.ratio[1]].find_nearest(x);
            auto res = find(RATIO_FINDERS[act.ratio[1]], x);
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }
    if (act.pool_id[1] != -1) {
        const int& par = act.par[0];
        assert(par == 0);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[1]];

        Point3 u = p1 - p0;
        if (!equal(u.abs2(), 0.0)) {
            double x = dot(p1, u) / u.abs2();

            // auto res = RATIO_FINDERS[act.ratio[0]].find_nearest(x);
            auto res = find(RATIO_FINDERS[act.ratio[0]], x);
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (act.pool_id[i] == -1) {
            continue;
        }
        pools[act.pool_id[i]] += target;
    }
}

inline std::array<std::pair<int, int>, 3> approx_strategy(
    std::array<Point3, K>& pools,
    const Point3& target,
    const MergeAct& act) {
    for (int i = 0; i < 4; i++) {
        if (act.pool_id[i] == -1) {
            continue;
        }
        pools[act.pool_id[i]] -= target;
    }

    std::array<std::pair<int, int>, 3> strategy = {};

    if (act.pool_id[3] != -1) {
        const int& par = act.par[2];
        assert(0 <= par && par <= 2);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[3]];
        const auto& crs_rem = crs(pools[act.pool_id[(par + 1) % 3]],
                                  pools[act.pool_id[(par + 2) % 3]]);

        double dot0 = dot(p0 - p1, crs_rem);
        double dot1 = dot(p1, crs_rem);

        if (!equal(dot0, 0)) {
            double x = -dot1 / dot0;
            auto [res, a] = RATIO_FINDERS[act.ratio[2]].find_nearest_entry(x);
            strategy[2] = a;
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }
    if (act.pool_id[2] != -1) {
        const int& par = act.par[1];
        assert(0 <= par && par <= 1);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[2]];
        const Point3& u1 = crs(p1 - p0, pools[act.pool_id[par ^ 1]]);
        const Point3& u2 = crs(p1, pools[act.pool_id[par ^ 1]]);

        if (!equal(u1.abs(), 0.0)) {
            double x = dot(u1, u2) / (u1.abs() * u1.abs());

            auto [res, a] = RATIO_FINDERS[act.ratio[1]].find_nearest_entry(x);
            strategy[1] = a;
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }
    if (act.pool_id[1] != -1) {
        const int& par = act.par[0];
        assert(par == 0);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[1]];

        Point3 u = p1 - p0;
        if (!equal(u.abs2(), 0.0)) {
            double x = dot(p1, u) / u.abs2();

            auto [res, a] = RATIO_FINDERS[act.ratio[0]].find_nearest_entry(x);
            strategy[0] = a;
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (act.pool_id[i] == -1) {
            continue;
        }
        pools[act.pool_id[i]] += target;
    }
    return strategy;
}

inline std::array<std::pair<int, int>, 3> approx_halfs_strategy(
    std::array<Point3, K>& pools,
    const Point3& target,
    const MergeAct& act) {
    for (int i = 0; i < 4; i++) {
        if (act.pool_id[i] == -1) {
            continue;
        }
        pools[act.pool_id[i]] -= target;
    }

    std::array<std::pair<int, int>, 3> strategy = {};

    if (act.pool_id[3] != -1) {
        const int& par = act.par[2];
        assert(0 <= par && par <= 2);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[3]];
        const auto& crs_rem = crs(pools[act.pool_id[(par + 1) % 3]],
                                  pools[act.pool_id[(par + 2) % 3]]);

        double dot0 = dot(p0 - p1, crs_rem);
        double dot1 = dot(p1, crs_rem);

        if (!equal(dot0, 0)) {
            double x = -dot1 / dot0;
            auto [res, a] =
                HALF_RATIO_FINDERS[act.ratio[2]].find_nearest_entry(x);
            strategy[2] = a;
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }
    if (act.pool_id[2] != -1) {
        const int& par = act.par[1];
        assert(0 <= par && par <= 1);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[2]];
        const Point3& u1 = crs(p1 - p0, pools[act.pool_id[par ^ 1]]);
        const Point3& u2 = crs(p1, pools[act.pool_id[par ^ 1]]);

        if (!equal(u1.abs(), 0.0)) {
            double x = dot(u1, u2) / (u1.abs() * u1.abs());

            auto [res, a] =
                HALF_RATIO_FINDERS[act.ratio[1]].find_nearest_entry(x);
            strategy[1] = a;
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }
    if (act.pool_id[1] != -1) {
        const int& par = act.par[0];
        assert(par == 0);
        auto& p0 = pools[act.pool_id[par]];
        auto& p1 = pools[act.pool_id[1]];

        Point3 u = p1 - p0;
        if (!equal(u.abs2(), 0.0)) {
            double x = dot(p1, u) / u.abs2();

            auto [res, a] =
                HALF_RATIO_FINDERS[act.ratio[0]].find_nearest_entry(x);
            strategy[0] = a;
            auto np0 = p0 * res + p1 * (1.0 - res);
            auto np1 = p0 * (1.0 - res) + p1 * res;
            p0 = np0;
            p1 = np1;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (act.pool_id[i] == -1) {
            continue;
        }
        pools[act.pool_id[i]] += target;
    }
    return strategy;
}

}  // namespace Board7

namespace t_large_beam_search {

using namespace Board7;

struct Action {
    int merge_act_id = -1;
    array<int, 4> used;
};

struct State {
    double score;
    double dist;

    Action action;

    // state
    array<Point3, K> pools;
    uint used;

    int pre_k;
    int pre_idx;

    bool operator<(const State& other) const { return score < other.score; }

    double raw_score() const {
        double sum = dist;
        sum += ADD_COST * std::popcount(used);
        return sum;
    }
};

inline V<State> beam() {
    array<array<V<State>, K>, Q + 1> states_list;
    states_list[0][0].push_back({.score = 0.0,
                                 .dist = 0.0,
                                 .action = {},
                                 .pools = {},
                                 .used = 0,
                                 .pre_k = -1,
                                 .pre_idx = -1});

    V<double> est_score(Q + 1);
    double total_freq_sum = 0.0;
    
    auto succ_query = [&](int q, double freq = 1.0) {
        total_freq_sum += freq;
        const Point3& target = TARGETS[q];

        for (auto& states : states_list[q + 1]) {
            states.clear();
        }

        array<int, K> WIDTH_LIST = {2, 2, 3, 5, 20, 20, 10};
        for (auto& x : WIDTH_LIST) x = max(1, int(freq * x));

        array<priority_queue<State>, K> pq;
        array<int, 4> used;

        auto succ = [&](const State& state, int pre_k, int pre_idx) {
            for (int act_id : iota(0, int(MERGE_ACTIONS_PAR0.size()))) {
                const auto& act = MERGE_ACTIONS_PAR0[act_id];

                auto cur_used = state.used;
                for (int i : iota(1, act.len)) {
                    cur_used |= (1 << act.pool_id[i]);
                }
                cur_used &= ~(1 << act.pool_id[0]);

                const int cur_k = std::popcount(cur_used);
                if (int(pq[cur_k].size()) == WIDTH_LIST[cur_k] &&
                    pq[cur_k].top().score <= state.score) {
                    continue;
                }

                auto cur_pool = state.pools;
                for (int i : iota(0, act.len)) {
                    if (!(state.used & (1 << act.pool_id[i]))) {
                        int use = random(0, M - 1, gen);
                        used[i] = use;
                        cur_pool[act.pool_id[i]] = COLORS[use];
                    }
                }

                approx(cur_pool, target, act);
                double cur_dist = (cur_pool[act.pool_id[0]] - target).abs();
                double cur_score = state.score + cur_dist;

                if (int(pq[cur_k].size()) == WIDTH_LIST[cur_k]) {
                    if (pq[cur_k].top().score <= cur_score) {
                        continue;
                    } else {
                        pq[cur_k].pop();
                    }
                }
                pq[cur_k].push({
                    .score = cur_score,
                    .dist = state.dist + cur_dist,
                    .action = {.merge_act_id = act_id, .used = used},
                    .pools = cur_pool,
                    .used = cur_used,
                    .pre_k = pre_k,
                    .pre_idx = pre_idx,
                });
            }
        };

        for (int k : iota(0, K) | reverse) {
            const auto& states = states_list[q][k];
            for (int pre_idx : iota(0, int(states.size()))) {
                succ(states[pre_idx], k, pre_idx);
            }
        }

        for (int k : iota(0, K)) {
            while (!pq[k].empty()) {
                auto state = pq[k].top();
                pq[k].pop();
                states_list[q + 1][k].push_back(state);
            }
            ranges::reverse(states_list[q + 1][k]);
            if (states_list[q + 1][k].empty()) continue;
            dbg(q, k, states_list[q + 1][k].front().score,
                states_list[q + 1][k].front().dist);
        }

        est_score[q + 1] = 1e9;
        for (const auto& states : states_list[q + 1]) {
            if (states.empty()) continue;
            chmin(est_score[q + 1], states.front().score);
        }
    };

    V<double> want_freq(Q, 1.0);
    for (int q : iota(990, Q)) {
        want_freq[q] = (q - 990) / 10.0 * 9.0 + 1.0;
    }

    dbg(want_freq);

    int system_count = 0;
    {
        int q = 0;

        double base_ratio = 0.5;

        int start_ms = global_sw.msecs();
        auto re_estimate = [&]() {
            if (total_freq_sum <= 1.0) return;  // not enough data

            int cur_ms = global_sw.msecs();
            double estimated_speed = (cur_ms - start_ms) / total_freq_sum;
            double estimated_remaining_cap = (TL - cur_ms) / estimated_speed;
            double remaining_freq_sum = 0;
            for (int i : iota(q, Q)) {
                remaining_freq_sum += want_freq[i];
            }
            remaining_freq_sum *= 1.5;

            base_ratio = estimated_remaining_cap / remaining_freq_sum;
            base_ratio = std::clamp(base_ratio, 0.01, 3.0);

            dbg(cur_ms, start_ms, total_freq_sum, estimated_speed, base_ratio,
                estimated_remaining_cap, remaining_freq_sum);
            // dbg(want_freq);
            // dbg(want_freq);
        };

        int next_error = 20;
        int error_count = 0;
        while (q < Q) {
            re_estimate();
            dbg(q, want_freq[q], base_ratio);
            std::cerr << "q: " << q
                      << " freq: " << max(0.1, want_freq[q] * base_ratio)
                      << " base_ratio: " << base_ratio << std::endl;
            succ_query(q, max(0.1, want_freq[q] * base_ratio));
            if (next_error <= q) {
                double avg = est_score[q + 1] / (q + 1);

                double error_ratio = 1.0 * error_count / (q + 1);
                const double TARGET_ERROR_RATE = 0.1;
                if (error_ratio >= TARGET_ERROR_RATE) continue;
                double threshold_rate =
                    10.0 * (1.0 - log((TARGET_ERROR_RATE - error_ratio) /
                                      TARGET_ERROR_RATE));
                dbg("======================================", q, avg,
                    error_count, error_ratio, threshold_rate);
                if (est_score[q + 1] - est_score[q] > avg * threshold_rate) {
                    // 異常検知　システムを起動します
                    next_error = q + 1;
                    error_count++;

                    dbg("!!!!!!!!!!!", q, est_score[q + 1], est_score[q], avg);
                    system_count++;
                    const int BACK = 6;
                    for (int i : iota(0, BACK + 1)) {
                        want_freq[q - i] = 6.0;
                        // succ_query(q - q2, base_freq * 6.0);
                    }
                    q -= BACK;
                    re_estimate();
                    continue;
                }
            }
            q++;
        }
    }

    State best_state;
    double best_score = 1e9;
    for (const auto& states : states_list[Q]) {
        for (const auto& state : states) {
            if (state.raw_score() < best_score) {
                best_state = state;
                best_score = state.raw_score();
            }
        }
    }
    dbg(best_score, system_count);

    V<State> result = {best_state};
    for (int q : iota(0, Q) | reverse) {
        best_state = states_list[q][best_state.pre_k][best_state.pre_idx];
        result.push_back(best_state);
    }
    ranges::reverse(result);
    return result;
}

inline Answer solve() {
    auto states = beam();
    dbg(states.size());

    auto mix = [&](int l, int r, int l_len, int r_len) -> V<OP> {
        if (l_len == 0 || r_len == 0) {
            return {};
        }
        for (bool l_rev : {false, true}) {
            for (bool r_rev : {false, true}) {
                auto lwall =
                    l_len == L
                        ? OP::nop()
                        : OP::wall(PATHS[l][l_rev ? (L - 1) - (l_len - 1)
                                                  : l_len - 1],
                                   PATHS[l][l_rev ? (L - 1) - (l_len) : l_len]);
                auto rwall =
                    r_len == L
                        ? OP::nop()
                        : OP::wall(PATHS[r][r_rev ? (L - 1) - (r_len - 1)
                                                  : r_len - 1],
                                   PATHS[r][r_rev ? (L - 1) - (r_len) : r_len]);

                for (int i : iota(0, l_len)) {
                    for (int j : iota(0, r_len)) {
                        auto p = PATHS[l][l_rev ? (L - 1) - i : i];
                        auto q = PATHS[r][r_rev ? (L - 1) - j : j];
                        auto diff = p - q;
                        if (abs(diff.r()) + abs(diff.c()) != 1) continue;
                        V<OP> ops;
                        ops.push_back(lwall);
                        ops.push_back(rwall);
                        ops.push_back(OP::wall(p, q));
                        ops.push_back(OP::wall(p, q));
                        ops.push_back(lwall);
                        ops.push_back(rwall);
                        return ops;
                    }
                }
            }
        }
        assert(false);
    };

    V<OP> ops;
    for (int q : iota(0, Q)) {
        const Point3& target = TARGETS[q];

        const auto& act = states[q + 1].action;
        const auto& mrg_act = MERGE_ACTIONS_PAR0[act.merge_act_id];
        auto pools = states[q].pools;
        int used = states[q].used;
        for (int i : iota(0, 4)) {
            if (mrg_act.pool_id[i] == -1) {
                continue;
            }
            if (!(used & (1 << mrg_act.pool_id[i]))) {
                ops.push_back(
                    OP::add(PATHS[mrg_act.pool_id[i]][0], act.used[i]));
                pools[mrg_act.pool_id[i]] = COLORS[act.used[i]];
            }
        }
        auto strategy = approx_strategy(pools, target, mrg_act);
        for (int i : iota(0, 3) | reverse) {
            auto [l, r] = strategy[i];
            auto ops2 = mix(mrg_act.pool_id[mrg_act.par[i]],
                            mrg_act.pool_id[1 + i], l, r);
            ops.insert(ops.end(), ops2.begin(), ops2.end());
        }
        ops.push_back(OP::pick(PATHS[mrg_act.pool_id[0]][0]));
    }

    return {INIT_BOARD, ops};
}

}  // namespace beam_search




// 14000 <= T
namespace t_mid_beam_search {

using namespace Board7;

struct Action {
    int merge_act_id = -1;
    array<int, 4> used;
};

struct State {
    double score;
    double dist;

    Action action;

    // state
    array<Point3, K> pools;
    uint used;

    int pre_k;
    int pre_idx;

    bool operator<(const State& other) const { return score < other.score; }

    double raw_score() const {
        double sum = dist;
        sum += ADD_COST * std::popcount(used);
        return sum;
    }
};

inline V<State> beam() {
    array<array<V<State>, K>, Q + 1> states_list;
    states_list[0][0].push_back({.score = 0.0,
                                 .dist = 0.0,
                                 .action = {},
                                 .pools = {},
                                 .used = 0,
                                 .pre_k = -1,
                                 .pre_idx = -1});

    V<double> est_score(Q + 1);
    double total_freq_sum = 0.0;

    auto succ_query = [&](int q, double freq = 1.0) {
        total_freq_sum += freq;

        const Point3& target = TARGETS[q];

        for (auto& states : states_list[q + 1]) {
            states.clear();
        }

        array<int, K> WIDTH_LIST = {2, 2, 3, 5, 10, 20, 20};
        for (auto& x : WIDTH_LIST) x = max(1, int(freq * x));

        array<priority_queue<State>, K> pq;
        array<int, 4> used;

        auto succ = [&](const State& state, int pre_k, int pre_idx) {
            for (int act_id : iota(0, int(MERGE_ACTIONS_PAR0.size()))) {
                const auto& act = MERGE_ACTIONS_PAR0[act_id];

                auto cur_used = state.used;
                for (int i : iota(1, act.len)) {
                    cur_used |= (1 << act.pool_id[i]);
                }
                cur_used &= ~(1 << act.pool_id[0]);

                const int cur_k = std::popcount(cur_used);
                if (int(pq[cur_k].size()) == WIDTH_LIST[cur_k] &&
                    pq[cur_k].top().score <= state.score) {
                    continue;
                }

                auto cur_pool = state.pools;
                for (int i : iota(0, act.len)) {
                    if (!(state.used & (1 << act.pool_id[i]))) {
                        int use = random(0, M - 1, gen);
                        used[i] = use;
                        cur_pool[act.pool_id[i]] = COLORS[use];
                    }
                }

                approx_half(cur_pool, target, act);
                double cur_dist = (cur_pool[act.pool_id[0]] - target).abs();
                double cur_score = state.score + cur_dist;

                if (int(pq[cur_k].size()) == WIDTH_LIST[cur_k]) {
                    if (pq[cur_k].top().score <= cur_score) {
                        continue;
                    } else {
                        pq[cur_k].pop();
                    }
                }
                pq[cur_k].push({
                    .score = cur_score,
                    .dist = state.dist + cur_dist,
                    .action = {.merge_act_id = act_id, .used = used},
                    .pools = cur_pool,
                    .used = cur_used,
                    .pre_k = pre_k,
                    .pre_idx = pre_idx,
                });
            }
        };

        for (int k : iota(0, K) | reverse) {
            const auto& states = states_list[q][k];
            for (int pre_idx : iota(0, int(states.size()))) {
                succ(states[pre_idx], k, pre_idx);
            }
        }

        for (int k : iota(0, K)) {
            while (!pq[k].empty()) {
                auto state = pq[k].top();
                pq[k].pop();
                states_list[q + 1][k].push_back(state);
            }
            ranges::reverse(states_list[q + 1][k]);
            if (states_list[q + 1][k].empty()) continue;
            dbg(q, k, states_list[q + 1][k].front().score,
                states_list[q + 1][k].front().dist);
        }

        est_score[q + 1] = 1e9;
        for (const auto& states : states_list[q + 1]) {
            if (states.empty()) continue;
            chmin(est_score[q + 1], states.front().score);
        }
    };

    V<double> want_freq(Q, 1.0);
    for (int q : iota(990, Q)) {
        want_freq[q] = (q - 990) / 10.0 * 9.0 + 1.0;
    }

    dbg(want_freq);

    int system_count = 0;
    {
        int q = 0;

        double base_ratio = 0.5;

        int start_ms = global_sw.msecs();
        auto re_estimate = [&]() {
            if (total_freq_sum <= 1.0) return;  // not enough data

            int cur_ms = global_sw.msecs();
            double estimated_speed = (cur_ms - start_ms) / total_freq_sum;
            double estimated_remaining_cap = (TL - cur_ms) / estimated_speed;
            double remaining_freq_sum = 0;
            for (int i : iota(q, Q)) {
                remaining_freq_sum += want_freq[i];
            }
            remaining_freq_sum *= 1.5;

            base_ratio = estimated_remaining_cap / remaining_freq_sum;
            base_ratio = std::clamp(base_ratio, 0.01, 3.0);

            dbg(cur_ms, start_ms, total_freq_sum, estimated_speed, base_ratio,
                estimated_remaining_cap, remaining_freq_sum);
            // dbg(want_freq);
            // dbg(want_freq);
        };

        int next_error = 20;
        int error_count = 0;
        while (q < Q) {
            re_estimate();
            dbg(q, want_freq[q], base_ratio);
            std::cerr << "q: " << q
                      << " freq: " << max(0.1, want_freq[q] * base_ratio)
                      << " base_ratio: " << base_ratio
                      << std::endl;
            succ_query(q, max(0.1, want_freq[q] * base_ratio));
            if (next_error <= q) {
                double avg = est_score[q + 1] / (q + 1);

                double error_ratio = 1.0 * error_count / (q + 1);
                const double TARGET_ERROR_RATE = 0.1;
                if (error_ratio >= TARGET_ERROR_RATE) continue;
                double threshold_rate =
                    10.0 * (1.0 - log((TARGET_ERROR_RATE - error_ratio) /
                                      TARGET_ERROR_RATE));
                dbg("======================================", q, avg,
                    error_count, error_ratio, threshold_rate);
                if (est_score[q + 1] - est_score[q] > avg * threshold_rate) {
                    // 異常検知　システムを起動します
                    next_error = q + 1;
                    error_count++;

                    dbg("!!!!!!!!!!!", q, est_score[q + 1], est_score[q], avg);
                    system_count++;
                    const int BACK = 6;
                    for (int i : iota(0, BACK + 1)) {
                        want_freq[q - i] = 6.0;
                        // succ_query(q - q2, base_freq * 6.0);
                    }
                    q -= BACK;
                    re_estimate();
                    continue;
                }
            }
            q++;
        }
    }

    State best_state;
    double best_score = 1e9;
    for (const auto& states : states_list[Q]) {
        for (const auto& state : states) {
            if (state.raw_score() < best_score) {
                best_state = state;
                best_score = state.raw_score();
            }
        }
    }
    dbg(best_score, system_count);

    V<State> result = {best_state};
    for (int q : iota(0, Q) | reverse) {
        best_state = states_list[q][best_state.pre_k][best_state.pre_idx];
        result.push_back(best_state);
    }
    ranges::reverse(result);
    return result;
}

inline Answer solve() {
    auto states = beam();
    dbg(states.size());

    auto mix = [&](int l, int r, int l_len, int r_len) -> V<OP> {
        if (l_len == 0 || r_len == 0) {
            return {};
        }
        for (bool l_rev : {false, true}) {
            for (bool r_rev : {false, true}) {
                auto lwall =
                    l_len == L
                        ? OP::nop()
                        : OP::wall(PATHS[l][l_rev ? (L - 1) - (l_len - 1)
                                                  : l_len - 1],
                                   PATHS[l][l_rev ? (L - 1) - (l_len) : l_len]);
                auto rwall =
                    r_len == L
                        ? OP::nop()
                        : OP::wall(PATHS[r][r_rev ? (L - 1) - (r_len - 1)
                                                  : r_len - 1],
                                   PATHS[r][r_rev ? (L - 1) - (r_len) : r_len]);

                for (int i : iota(0, l_len)) {
                    for (int j : iota(0, r_len)) {
                        auto p = PATHS[l][l_rev ? (L - 1) - i : i];
                        auto q = PATHS[r][r_rev ? (L - 1) - j : j];
                        auto diff = p - q;
                        if (abs(diff.r()) + abs(diff.c()) != 1) continue;
                        V<OP> ops;
                        ops.push_back(lwall);
                        ops.push_back(rwall);
                        ops.push_back(OP::wall(p, q));
                        ops.push_back(OP::wall(p, q));
                        ops.push_back(lwall);
                        ops.push_back(rwall);
                        return ops;
                    }
                }
            }
        }
        assert(false);
    };

    V<OP> ops;
    for (int q : iota(0, Q)) {
        const Point3& target = TARGETS[q];

        const auto& act = states[q + 1].action;
        const auto& mrg_act = MERGE_ACTIONS_PAR0[act.merge_act_id];
        auto pools = states[q].pools;
        int used = states[q].used;
        for (int i : iota(0, 4)) {
            if (mrg_act.pool_id[i] == -1) {
                continue;
            }
            if (!(used & (1 << mrg_act.pool_id[i]))) {
                ops.push_back(
                    OP::add(PATHS[mrg_act.pool_id[i]][0], act.used[i]));
                pools[mrg_act.pool_id[i]] = COLORS[act.used[i]];
            }
        }
        auto strategy = approx_halfs_strategy(pools, target, mrg_act);
        for (int i : iota(0, 3) | reverse) {
            auto [l, r] = strategy[i];
            auto ops2 = mix(mrg_act.pool_id[mrg_act.par[i]],
                            mrg_act.pool_id[1 + i], l, r);
            ops.insert(ops.end(), ops2.begin(), ops2.end());
        }
        ops.push_back(OP::pick(PATHS[mrg_act.pool_id[0]][0]));
    }

    return {INIT_BOARD, ops};
}

}  // namespace t_mid_beam_search



namespace t_small_brute {

inline Answer long_cycle() {
    const int L = N * N;  // length of the cycle
    const int BN = N;     // number of blocks
    const int BS = N;     // size of each block
    assert(BN * BS == L);

    const array<Coord, 3 * L> coords = [&]() {
        array<Coord, 3 * L> coords;
        int cnt = 0;
        for (int r : iota(0, N)) {
            for (int c : iota(0, N)) {
                int c2 = (r % 2 == 0) ? c : N - 1 - c;
                if (1 <= r && r < N - 1 && c2 == 0)
                    continue;  // skip the first column
                coords[cnt++] = Coord(r, c2);
            }
        }
        for (int r : iota(1, N - 1) | reverse) {
            coords[cnt++] = Coord(r, 0);
        }
        assert(cnt == L);
        for (int i : iota(0, L)) {
            coords[i + L] = coords[i];
            coords[i + 2 * L] = coords[i];
        }
        return coords;
    }();

    array<array<int, N>, N> p2idx;
    for (int i : iota(0, L)) {
        p2idx[coords[i].r()][coords[i].c()] = i;
    }

    Board init_board = Board::all_walls();

    // remove the walls on the cycle
    for (int i : iota(0, L)) {
        dbg(coords[i], coords[i + 1]);
        init_board.flip(coords[i], coords[i + 1]);
    }

    Simulator sim(init_board);

    for (int q : iota(0, Q)) {
        if (global_sw.msecs() > 2800) {
            // time limit
            return {init_board, {}};
        }
        double add_cost = ADD_COST * max(0.0, 1.0 * (q - 900) / Q);
        const auto& target = TARGETS[q];

        // shift colors
        const V<Point3> colors = [&]() {
            V<Point3> buf;
            for (int i : iota(0, M)) {
                buf.push_back(COLORS[i] - target);
            }
            return buf;
        }();
        const V<double> inv_abs_colors = [&]() {
            V<double> buf;
            for (int i : iota(0, M)) {
                buf.push_back(1.0 / colors[i].abs());
            }
            return buf;
        }();

        // current state
        const array<Pool, 2 * L> pools = [&]() {
            array<Pool, 2 * L> buf;
            for (int i : iota(0, L)) {
                buf[i] = sim.get_point_pool(coords[i]);
                buf[i + L] = buf[i];
            }
            return buf;
        }();
        const array<bool, 2 * L> walls = [&]() {
            array<bool, 2 * L> buf;
            for (int i : iota(0, L)) {
                buf[i] = sim.board.has_wall(coords[i + L - 1], coords[i]);
                buf[i + L] = buf[i];
            }
            return buf;
        }();

        const array<Point3, L> ps = [&]() {
            array<Point3, L> buf;
            for (int i : iota(0, L)) {
                auto pool = pools[i];
                buf[i] = (pool.color() - target) * pool.amount();
            }
            return buf;
        }();
        const array<Point3, 2 * L + 1> ps_sum = [&]() {
            array<Point3, 2 * L + 1> buf;
            buf[0] = Point3();
            for (int i : iota(0, 2 * L)) {
                buf[i + 1] = buf[i] + ps[i % L];
            }
            return buf;
        }();
        const array<double, 2 * L + 1> amount_sum = [&]() {
            array<double, 2 * L + 1> buf;
            buf[0] = 0;
            for (int i : iota(0, 2 * L)) {
                buf[i + 1] = buf[i] + pools[i % L].amount();
            }
            return buf;
        }();

        const int TURN_LIMIT = [&]() {
            int cur_turn = sim.turn;
            for (int i : iota(0, L)) {
                if (walls[i]) {
                    cur_turn++;
                }
            }
            int rem_turn =
                (TURN - cur_turn) - 2 * (Q - q);  // at least 2 turns per query

            int rem_cur_turn = ceil_div(rem_turn, (Q - q));
            if (q < 50) {
                chmax(rem_cur_turn, 10);
            }
            chmin(rem_cur_turn, rem_turn);

            return rem_cur_turn;
        }();
        dbg(q, TURN_LIMIT);

        double best_score = 1e9;
        V<OP> best_ops;

        int LIMIT_LEN = 2 * BS;

        // add + pick
        for (int l : iota(0, L)) {
            for (int d : iota(2, LIMIT_LEN)) {
                int r = (l + d) % L;

                int est_turn = (!walls[l] ? 2 : 0) + (!walls[r] ? 2 : 0);
                if (est_turn > TURN_LIMIT) {
                    continue;
                }

                Point3 p = (ps_sum[l + d] - ps_sum[l]);
                double amount = (amount_sum[l + d] - amount_sum[l]);
                if (greater(amount + 1.0, d)) continue;

                int use = -1;
                {
                    double mx = 1e9;
                    for (int i : iota(0, M)) {
                        // find arg between colors[i] and p
                        double v = dot(colors[i], p) * inv_abs_colors[i];
                        if (less(v, mx)) {
                            mx = v;
                            use = i;
                        }
                    }
                }

                double ratio = equal(colors[use].abs(), 0.0)
                                   ? 1.0
                                   : p.abs() / colors[use].abs();
                // ratio ~ len / d
                int len = int(round(ratio * d));
                len = std::clamp(len, 2, d);
                assert(len <= d);

                if (est_turn + 2 > TURN_LIMIT) {
                    len = d;
                }
                while (len < d) {
                    auto cur_amount = amount * (1.0 * len / d) + 1.0;
                    if (greater(cur_amount, len)) {
                        len++;
                        continue;
                    }
                    break;
                }

                if (len != d) {
                    est_turn += 2;
                }

                auto cur_p = p * (1.0 * len / d) + colors[use];
                auto cur_amount = amount * (1.0 * len / d) + 1.0;
                if (greater(cur_amount, len)) {
                    continue;
                }

                double cur_dist = cur_p.abs() / cur_amount;
                double cur_score = cur_dist;

                if (!less(cur_score, best_score)) continue;
                chmin(best_score, cur_score);

                V<OP> nops;
                if (!walls[l]) {
                    nops.push_back(OP::wall(coords[l - 1 + L], coords[l]));
                }
                if (!walls[r]) {
                    nops.push_back(OP::wall(coords[r - 1 + L], coords[r]));
                }
                for (int i : iota(1, d)) {
                    int j = (l + i) % L;
                    if (!walls[j]) continue;
                    nops.push_back(OP::wall(coords[j - 1 + L], coords[j]));
                }

                if (len < d) {
                    nops.push_back(
                        OP::wall(coords[l + len - 1], coords[l + len]));
                }

                nops.push_back(OP::add(coords[l], use));
                nops.push_back(OP::pick(coords[l]));

                best_ops = nops;
            }
        }

        // add + add + pick
        for (int l : iota(0, L)) {
            for (int d : iota(2, LIMIT_LEN)) {
                int r = (l + d) % L;

                int est_turn = (!walls[l] ? 2 : 0) + (!walls[r] ? 2 : 0);
                if (est_turn > TURN_LIMIT) {
                    continue;
                }

                Point3 p = (ps_sum[l + d] - ps_sum[l]);
                double amount = (amount_sum[l + d] - amount_sum[l]);

                int use0 = -1;
                if (d >= BS && less(amount, ADD_COST < 0.1 ? 3.0 : 1.5)) {
                    use0 = random(0, M - 1, gen);
                    p += colors[use0];
                    amount += 1.0;
                    if (greater(amount, d)) continue;

                    // TODO: without adding when TURN_LIMIT is small?
                    est_turn++;
                    if (est_turn > TURN_LIMIT) {
                        continue;
                    }
                } else {
                    continue;
                }

                int use = -1;
                {
                    double mx = 1e9;
                    for (int i : iota(0, M)) {
                        // find arg between colors[i] and p
                        double v = dot(colors[i], p) * inv_abs_colors[i];
                        if (less(v, mx)) {
                            mx = v;
                            use = i;
                        }
                    }
                }

                double ratio = equal(colors[use].abs(), 0.0)
                                   ? 1.0
                                   : p.abs() / colors[use].abs();
                // double ratio = p.abs() / colors[use].abs();
                //  ratio ~ len / d
                int len = int(round(ratio * d));
                len = min(len, d);
                len = std::clamp(len, 2, d);

                if (est_turn + 2 > TURN_LIMIT) {
                    len = d;
                }
                if (len != d) {
                    est_turn += 2;
                }

                auto cur_p = p * (1.0 * len / d) + colors[use];
                auto cur_amount = amount * (1.0 * len / d) + 1.0;
                if (greater(cur_amount, len)) continue;

                double cur_dist = cur_p.abs() / cur_amount;
                double cur_score = cur_dist + (use0 != -1 ? add_cost : 0.0);

                if (!less(cur_score, best_score)) continue;
                chmin(best_score, cur_score);

                V<OP> nops;
                if (!walls[l]) {
                    nops.push_back(OP::wall(coords[l - 1 + L], coords[l]));
                }
                if (!walls[r]) {
                    nops.push_back(OP::wall(coords[r - 1 + L], coords[r]));
                }
                for (int i : iota(1, d)) {
                    int j = (l + i) % L;
                    if (!walls[j]) continue;
                    nops.push_back(OP::wall(coords[j - 1 + L], coords[j]));
                }

                if (use0 != -1) nops.push_back(OP::add(coords[l], use0));
                if (len < d) {
                    nops.push_back(
                        OP::wall(coords[l + len - 1], coords[l + len]));
                }

                nops.push_back(OP::add(coords[l], use));
                nops.push_back(OP::pick(coords[l]));

                best_ops = nops;
            }
        }

        for (int l : iota(0, L)) {
            for (int d : iota(1, LIMIT_LEN)) {
                int r = (l + d) % L;

                int est_turn = (!walls[l] ? 2 : 0) + (!walls[r] ? 2 : 0);
                if (est_turn > TURN_LIMIT) {
                    continue;
                }

                const Point3 p = (ps_sum[l + d] - ps_sum[l]);
                const double amount = (amount_sum[l + d] - amount_sum[l]);

                if (less(amount, 1.0)) continue;

                double cur_dist = p.abs() / amount;
                double cur_score = cur_dist - add_cost;
                if (!less(cur_score, best_score)) continue;
                chmin(best_score, cur_score);

                V<OP> nops;
                if (!walls[l]) {
                    nops.push_back(OP::wall(coords[l - 1 + L], coords[l]));
                }
                if (!walls[r]) {
                    nops.push_back(OP::wall(coords[r - 1 + L], coords[r]));
                }
                for (int i : iota(1, d)) {
                    int j = (l + i) % L;
                    if (!walls[j]) continue;
                    nops.push_back(OP::wall(coords[j - 1 + L], coords[j]));
                }
                nops.push_back(OP::pick(coords[l]));
                best_ops = nops;
            }
        }

        double cur_dist_sum = sim.dist_sum();
        for (const auto& op : best_ops) {
            sim.action(op);
        }
        double new_dist_sum = sim.dist_sum();
        dbg(q, best_score, new_dist_sum, new_dist_sum - cur_dist_sum);
    }

    dbg(sim.dist_sum(), sim.score(), sim.turn);

    return {init_board, sim.ops};
}

}


void print_answer_debug(Answer answer) {
    Simulator sim(answer.init);
    for (const auto& op : answer.ops) {
        sim.action(op, true);
    }
}

int main() {
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_OFF);

    Answer ans;
    if (TURN < 14000) {
        double best_score = 1e9;
        while (global_sw.msecs() < 2800) {
            auto cur_ans = t_small_brute::long_cycle();
            Simulator sim(cur_ans.init);
            for (const auto& op : cur_ans.ops) {
                sim.action(op, true);
            }
            if (sim.num_pick_ops < Q) continue;
            double cur_score = sim.score();
            if (cur_score < best_score) {
                best_score = cur_score;
                dbg("new best", best_score);
                ans = cur_ans;
            }
        }
    } else if (TURN < 20000) {
        ans = t_mid_beam_search::solve();
    } else {
        ans = t_large_beam_search::solve();
    }

    print_answer(ans);

    dbg("used", global_sw.msecs());
    assert(global_sw.msecs() < 3000);
    return 0;
}
