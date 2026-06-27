// #undef YOSUPO_LOCAL

#include <algorithm>
#if 0 and !defined(__clang__)
#include <vector>
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#pragma GCC optimize("Ofast")
#endif


#include <stdio.h>
#include <unistd.h>

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

#include <initializer_list>


#include <cstddef>
#include <ranges>
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

#include <concepts>
#include <map>
#include <set>
#include <tuple>



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

namespace yosupo {

template <class T> struct Vector2D {
    int h, w;
    T* d;

    Vector2D() : h(0), w(0), d(new T[0]()) {}
    Vector2D(int _h, int _w) : h(_h), w(_w), d(new T[h * w]()) {}
    Vector2D(int _h, int _w, const T& val) : h(_h), w(_w), d(new T[h * w]) {
        std::fill_n(d, h * w, val);
    }

    Vector2D(std::initializer_list<std::initializer_list<T>> list)
        : h(static_cast<int>(list.size())), w(0), d(nullptr) {
        if (h > 0) {
            w = static_cast<int>(list.begin()->size());
            d = new T[h * w];
            int i = 0;
            for (const auto& inner_list : list) {
                assert(static_cast<int>(inner_list.size()) == w);
                std::copy_n(inner_list.begin(), w, d + i * w);
                i++;
            }
        }
    }

    Vector2D(const Vector2D& other) : h(other.h), w(other.w), d(new T[h * w]) {
        std::copy_n(other.d, h * w, d);
    }
    Vector2D& operator=(const Vector2D& other) {
        if (this != &other) {
            delete[] d;
            h = other.h;
            w = other.w;
            d = new T[h * w];
            std::copy_n(other.d, h * w, d);
        }
        return *this;
    }
    ~Vector2D() { delete[] d; }

    T& operator[](const Coord& idx) { return d[idx.r() * w + idx.c()]; }
    const T& operator[](const Coord& idx) const {
        return d[idx.r() * w + idx.c()];
    }

    static Vector2D e(int n) {
        Vector2D mat(n, n);
        for (int i = 0; i < n; i++) {
            mat[{i, i}] = T(1);
        }
        return mat;
    }

    Vector2D& operator+=(const Vector2D& rhs) {
        assert(h == rhs.h);
        assert(w == rhs.w);
        for (int i = 0; i < h * w; i++) {
            d[i] += rhs.d[i];
        }
        return *this;
    }

    Vector2D& operator-=(const Vector2D& rhs) {
        assert(h == rhs.h);
        assert(w == rhs.w);
        for (int i = 0; i < h * w; i++) {
            d[i] -= rhs.d[i];
        }
        return *this;
    }
    friend Vector2D operator+(const Vector2D& lhs, const Vector2D& rhs) {
        return Vector2D(lhs) += rhs;
    }
    friend Vector2D operator-(const Vector2D& lhs, const Vector2D& rhs) {
        return Vector2D(lhs) -= rhs;
    }

    friend Vector2D operator*(const Vector2D& lhs, const Vector2D& rhs) {
        assert(lhs.w == rhs.h);
        Vector2D res(lhs.h, rhs.w);
        Vector2D rhs_t = rhs.transpose();
        for (int i = 0; i < lhs.h; i++) {
            for (int j = 0; j < rhs.w; j++) {
                for (int k = 0; k < lhs.w; k++) {
                    res[{i, j}] += lhs[{i, k}] * rhs_t[{j, k}];
                }
            }
        }
        return res;
    }
    Vector2D& operator*=(const Vector2D& r) { return *this = *this * r; }

    Vector2D transpose() const {
        Vector2D res(w, h);
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                res[{j, i}] = (*this)[{i, j}];
            }
        }
        return res;
    }

    Vector2D pow(long long n) const {
        assert(h == w);
        Vector2D x = *this, r = e(h);
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }

    std::string dump() const {
        std::string res;
        for (int i = 0; i < h; i++) {
            if (i) res += "\n";
            for (int j = 0; j < w; j++) {
                if (j) res += " ";
                res += yosupo::dump((*this)[{i, j}]);
            }
        }
        return res;
    }
};

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

#include <iterator>




namespace yosupo {

namespace internal {

// simple hash based on FNV-hash + wymix (of wyhash)
inline u64 wymix(u64 a, u64 b) {
    u128 c = (u128)a * b;
    return u64(c) ^ u64(c >> 64);
}
struct Hasher {
    u64 a, b;

    auto update64(u64 x) { a = wymix(a ^ x, MULTIPLE); }

    u64 digest() { return wymix(a, b); }

    template <std::integral T>
        requires(sizeof(T) <= 8)
    void update(const T& x) {
        update64(x);
    }

    template <class T>
        requires(std::same_as<T, i128> || std::same_as<T, u128>)
    void update(const T& x) {
        update64(u64(x));
        update64(u64((u128)(x) >> 64));
    }

    // pair
    template <class S, class T> void update(const std::pair<S, T>& x) {
        update(x.first);
        update(x.second);
    }

    // tuple
    template <class... Args> auto update(const std::tuple<Args...>& x) {
        std::apply([&](const auto&... y) { (update(y), ...); }, x);
    }

    // array
    template <class T, size_t N> auto update(const std::array<T, N>& x) {
        for (const auto& y : x) {
            update(y);
        }
    }

    // vector
    template <class T> auto update(const std::vector<T>& x) {
        update(x.size());
        for (const auto& y : x) {
            update(y);
        }
    }

    // string
    auto update(const std::string& x) {
        update(x.size());
        // TODO: should handle as 8-byte chunks
        for (const auto& y : x) {
            update(y);
        }
    }

    // set
    template <class T> auto update(const std::set<T>& x) {
        update(x.size());
        for (const auto& y : x) {
            update(y);
        }
    }

    // map
    template <class T, class U> auto update(const std::map<T, U>& x) {
        update(x.size());
        for (const auto& y : x) {
            update(y);
        }
    }

    const u64 MULTIPLE = 6364136223846793005;
};

}  // namespace internal

struct Hasher {
    inline static Random hash_gen = get_random();

    const u64 a = uniform<u64>(hash_gen);
    const u64 b = uniform<u64>(hash_gen);

    template <class T> u64 operator()(const T& x) const {
        internal::Hasher h{a, b};
        h.update(x);
        return h.digest();
    }
};

}  // namespace yosupo

namespace yosupo {

template <class K, class D, class H = Hasher> struct IncrementalHashMap {
  public:
    using Data = std::pair<K, D>;

  private:
    struct Iterator {
      public:
        using difference_type = i32;
        using value_type = Data;
        using pointer = Data*;
        using reference = Data&;
        using iterator_category = std::forward_iterator_tag;

        IncrementalHashMap& _mp;
        u32 _pos;
        Iterator(IncrementalHashMap& mp, u32 pos) : _mp(mp), _pos(pos) {}

        std::pair<K, D>& operator*() const { return _mp.data[_pos]; }
        std::pair<K, D>* operator->() const { return &_mp.data[_pos]; }

        Iterator& operator++() {
            _pos = _mp.next_bucket(_pos + 1);
            return *this;
        }
        Iterator operator++(int) {
            auto result = *this;
            ++*this;
            return result;
        }

        bool operator==(const Iterator& rhs) const { return _pos == rhs._pos; }
        bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }
    };

    struct ConstIterator {
      public:
        using difference_type = i32;
        using value_type = const Data;
        using pointer = const Data*;
        using reference = const Data&;
        using iterator_category = std::forward_iterator_tag;

        const IncrementalHashMap& _mp;
        u32 _pos;
        ConstIterator(const IncrementalHashMap& mp, u32 pos)
            : _mp(mp), _pos(pos) {}

        const std::pair<K, D>& operator*() const { return _mp.data[_pos]; }
        const std::pair<K, D>* operator->() const { return &_mp.data[_pos]; }

        ConstIterator& operator++() {
            _pos = _mp.next_bucket(_pos + 1);
            return *this;
        }
        ConstIterator operator++(int) {
            auto result = *this;
            ++*this;
            return result;
        }

        bool operator==(const ConstIterator& rhs) const {
            return _pos == rhs._pos;
        }
        bool operator!=(const ConstIterator& rhs) const {
            return !(*this == rhs);
        }
    };

  public:
    IncrementalHashMap(size_t s, const H& _h = H())
        : h(_h),
          mask((1 << s) - 1),
          filled(0),
          used(mask + 1),
          data(mask + 1) {}
    IncrementalHashMap(const H& _h = H()) : IncrementalHashMap(2, _h) {}

    Iterator begin() { return Iterator(*this, next_bucket(0)); }
    Iterator end() { return Iterator(*this, mask + 1); }
    ConstIterator begin() const { return ConstIterator(*this, next_bucket(0)); }
    ConstIterator end() const { return ConstIterator(*this, mask + 1); }
    ConstIterator cbegin() const { return begin(); }
    ConstIterator cend() const { return end(); }

    using iterator = Iterator;
    using const_iterator = ConstIterator;

    D& operator[](const K& k) {
        u32 i = start_bucket(k);
        while (used[i] && data[i].first != k) {
            i = (i + 1) & mask;
        }
        if (!used[i]) {
            if (filled * 2 > mask) {
                rehash();
                return (*this)[k];
            }
            filled++;
            used[i] = true;
            data[i] = {k, D()};
        }
        return data[i].second;
    }

    Iterator find(const K& k) {
        u32 i = start_bucket(k);
        while (used[i] && data[i].first != k) {
            i = (i + 1) & mask;
        }
        if (!used[i]) return end();
        return Iterator(*this, i);
    }

    ConstIterator find(const K& k) const {
        u32 i = start_bucket(k);
        while (used[i] && data[i].first != k) {
            i = (i + 1) & mask;
        }
        if (!used[i]) return cend();
        return ConstIterator(*this, i);
    }

    size_t count(const K& k) const { return this->find(k) != cend() ? 1 : 0; }
    bool contains(const K& k) const { return this->find(k) != cend(); }

    size_t size() const { return filled; }

    std::string dump() const {
        std::string s = "{";
        bool first = true;
        for (const auto& [key, val] : *this) {
            if (!first) {
                s += ", ";
            }
            first = false;
            s += yosupo::dump(key);
            s += ": ";
            s += yosupo::dump(val);
        }
        s += "}";
        return s;
    }

  private:
    Hasher h;

    u32 mask, filled;  // data.size() == 1 << s

    std::vector<bool> used;
    std::vector<Data> data;

    void rehash() {
        u32 pmask = mask;
        mask = mask * 2 + 1;
        filled = 0;
        auto pused = std::exchange(used, std::vector<bool>(mask + 1));
        auto pdata = std::exchange(data, std::vector<Data>(mask + 1));
        for (u32 i = 0; i <= pmask; i++) {
            if (pused[i]) {
                (*this)[pdata[i].first] = pdata[i].second;
            }
        }
    }

    u32 start_bucket(const K& k) const { return (u32)(h(k)) & mask; }

    u32 next_bucket(u32 i) const {
        while (i <= mask && !used[i]) i++;
        return i;
    }
};

}  // namespace yosupo


namespace yosupo {

template <class T> struct FlattenVector {
    std::vector<T> v;
    std::vector<int> start;
    FlattenVector(int n, const std::vector<std::pair<int, T>>& _v)
        : start(n + 1) {
        for (const auto& x : _v) {
            start[x.first + 1]++;
        }
        for (int i = 1; i <= n; i++) {
            start[i] += start[i - 1];
        }
        v = std::vector<T>(start[n]);

        auto pos = start;
        for (const auto& x : _v) {
            v[pos[x.first]] = x.second;
            pos[x.first]++;
        }
    }

    auto at(int i) {
        return v | std::ranges::views::take(start[i + 1]) |
               std::ranges::views::drop(start[i]);
    }
    auto at(int i) const {
        return v | std::ranges::views::take(start[i + 1]) |
               std::ranges::views::drop(start[i]);
    }

    template <class Pred> int erase_if(Pred pred) {
        int n = int(start.size()) - 1;

        int removed = 0;
        int r = start[0];
        for (int i = 0; i < n; i++) {
            int l = r;
            r = start[i + 1];
            start[i + 1] = start[i];
            for (int j = l; j < r; j++) {
                std::pair<int, T> p = {i, v[j]};
                if (pred(p)) {
                    removed++;
                } else {
                    v[start[i + 1]] = v[j];
                    start[i + 1]++;
                }
            }
        }
        v.resize(start[n]);
        return removed;
    }
};

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


#include <bitset>
#include <iostream>
#include <queue>


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

StopWatch global_sw = StopWatch();

#ifdef YOSUPO_LOCAL
const int TL = 3000; // somehow my PC is slow
#else
const int TL = 1800;
#endif

const int N = 36;
const int M = 12;
const int L = TEN(6);

struct TARGET {
    string raw_s;
    array<int, M> s;
    int len;

    int point;

    array<array<int, 6>, M> next;

    TARGET() {}
    TARGET(string _s, int _point) : raw_s(_s), point(_point) {
        len = int(raw_s.size());
        s.fill(-1);
        for (int i : iota(0, len)) {
            s[i] = raw_s[i] - 'a';
        }

        for (int i : iota(0, M)) {
            next[i].fill(-1);
        }
        for (int i : iota(0, len)) {
            for (int j : iota(0, 6)) {
                string t = raw_s.substr(0, i);
                t += char('a' + j);
                next[i][j] = -1;
                for (int nlen = i + 1; nlen >= 0; nlen--) {
                    if (t.substr(i + 1 - nlen, nlen) == raw_s.substr(0, nlen)) {
                        next[i][j] = nlen;
                        break;
                    }
                }
                assert(next[i][j] != -1);
            }
        }

    }    
};

array<TARGET, N> TARGETS = []() {
    int dummy;
    sc.read(dummy, dummy, dummy);

    array<TARGET, N> targets;
    for (int i = 0; i < N; i++) {
        string s;
        int x;
        sc.read(s, x);
        targets[i] = {s, x};
    }
    return targets;
}();

struct Answer {
    array<int, M> state;
    array<array<int, M>, M> mat;
};

double calc_probability(const Answer& ans, const TARGET& trg) {
    int trg_len = trg.len;

    // 1) 状態空間の列挙 (len, u) -> idx
    using P = pair<int, int>;
    IncrementalHashMap<P, int> states;

    P init = (ans.state[0] == trg.s[0]) ? P(1, 0) : P(0, 0);
    V<pair<P, int>> st;
    st.push_back({init, 0});
    states[init] = 0;

    V<pair<pair<int, int>, double>> edges;

    int n = 1;
    while (st.size()) {
        auto [len_u, i] = st.back();
        st.pop_back();
        auto [len, u] = len_u;

        for (int v = 0; v < M; v++) {
            if (ans.mat[u][v] == 0) continue;

            int new_len = trg.next[len][ans.state[v]];
            if (new_len != trg_len) {
                P new_st = {new_len, v};
                int j = -1;
                auto it = states.find(new_st);
                if (it != states.end()) {
                    j = it->second;
                } else {
                    j = n;
                    n++;
                    states[new_st] = j;
                    st.push_back({new_st, j});
                }
                edges.push_back({{i, j}, ans.mat[u][v] / 100.0});
            }
        }
    }

    // 2) 遷移行列 X の構築
    Vector2D<double> X(n, n, 0.0);
    for (const auto& edge : edges) {
        auto [i, j] = edge.first;
        auto w = edge.second;

        X[{i, j}] += w;
    }

    // 3) 行列累乗 X^(L-1)
    X = X.pow(L - 1);

    // 4) 初期状態と最終確率
    double ret = 1.0;
    for (int i : iota(0, n)) {
        ret -= X[{0, i}];
    }
    double res = std::clamp(ret, 0.0, 1.0);

    return res;
}

double calc_approx_probability(const Answer& ans, const TARGET& trg) {
    const int trg_len = trg.len;

    // 1) 状態空間の列挙 (len, u) -> idx
    using P = pair<int, int>;
    IncrementalHashMap<P, int> states;

    P init = (ans.state[0] == trg.s[0]) ? P(1, 0) : P(0, 0);
    V<pair<P, int>> st;

    st.push_back({init, 0});
    states[init] = 0;

    static V<pair<int, pair<int, double>>> edges;
    edges.clear();

    bool has_end = false;

    int n = 1;
    while (st.size()) {
        auto [len_u, i] = st.back();
        st.pop_back();
        auto [len, u] = len_u;

        for (int v = 0; v < M; v++) {
            if (ans.mat[u][v] == 0) continue;

            int new_len = trg.next[len][ans.state[v]];
            if (new_len == trg_len) {
                has_end = true;
                continue;
            }
            P new_st = {new_len, v};
            int j = -1;
            auto it = states.find(new_st);
            if (it != states.end()) {
                j = it->second;
            } else {
                j = n;
                n++;
                states[new_st] = j;
                st.push_back({new_st, j});
            }
            edges.push_back({i, {j, ans.mat[u][v] / 100.0}});
        }
    }
    if (!has_end) {
        return 0.0;
    }

    // 2) 遷移行列 X の構築
    V<double> X(n, 0.0), Y(n, 0.0);
    X[0] = 1.0;

    auto fedges = FlattenVector(n, edges);

    const int TURN = 80;
    for (int _ : iota(0, TURN)) {
        ranges::fill(Y, 0.0);
        for (int i : iota(0, n)) {
            for (const auto& edge : fedges.at(i)) {
                auto [j, w] = edge;
                Y[j] += X[i] * w;
            }
        }
        swap(X, Y);
    }

    double sum = 0.0;
    for (int i : iota(0, n)) {
        sum += X[i];
    }
    sum = std::clamp(sum, 0.0, 1.0);

    double res = 1 - pow(sum, L / TURN);
    
    return std::clamp(res, 0.0, 1.0);
}

double calc_answer(const Answer& ans, const V<TARGET>& targets) {
    double ret = 0.0;
    for (auto& trg : targets) {
        double p = calc_probability(ans, trg);
        ret += p * trg.point;
    }
    return ret;
}
double calc_answer(const Answer& ans) {
    double ret = 0.0;
    for (auto& trg : TARGETS) {
        double p = calc_probability(ans, trg);
        ret += p * trg.point;
    }
    return ret;
}

double calc_approx_answer(const Answer& ans, const V<TARGET>& targets) {
    double ret = 0.0;
    for (auto& trg : targets) {
        double p = calc_approx_probability(ans, trg);
        ret += p * trg.point;
    }
    return ret;
}

void print_answer(const Answer& ans) {
    for (int i : iota(0, M)) {
        pr.write(char('a' + ans.state[i]));
        for (int j : iota(0, M)) {
            pr.write(' ');
            pr.write(ans.mat[i][j]);
        }
        pr.writeln();
    }
}

array<int, M> approx(const array<double, M>& v) {
    assert(v.size() == M);
    array<int, M> cnt = {};

    for (int _ : iota(0, 100)) {
        int idx = 0;
        double max = -1;
        for (int i : iota(0, M)) {
            double p = v[i] / (cnt[i] + 1);
            if (max < p) {
                max = p;
                idx = i;
            }
        }
        cnt[idx]++;
    }    
    return cnt;
}

Answer opt_path(const V<TARGET>& targets, int time_limit) {
    int k = int(targets.size());

    int offset = random(0, M - 1);
    V<array<int, M>> pos(k);
    for (int i : iota(0, k)) {
        const auto& trg = targets[i];
        pos[i].fill(-1);
        for (int j : iota(0, trg.len)) {
            pos[i][j] = 2 * trg.s[j] + random(0, 1);
        }
    }
    V<double> ws(k);
    for (int i : iota(0, k)) {
        const auto& trg = targets[i];
        ws[i] = pow(trg.point, 1.1);
    }

    auto get_answer = [&]() {
        Answer ans;
        for (int i : iota(0, M)) {
            ans.state[(i + offset) % M] = (i / 2);
        }

        Vector2D<double> w(M, M);
        for (int i : iota(0, k)) {
            int len = targets[i].len;
            for (int j : iota(1, len)) {
                int p0 = pos[i][j - 1];
                int p1 = pos[i][j];
                w[{p0, p1}] += ws[i];
            }
        }

        for (int i : iota(0, M)) {
            array<double, M> v;
            for (int j : iota(0, M)) {
                v[j] = w[{i, j}];
            }
            array<int, M> cnt = approx(v);
            for (int j : iota(0, M)) {
                ans.mat[(i + offset) % M][(j + offset) % M] = cnt[j];
            }
        }
        return ans;
    };
    auto get_score = [&]() {
        return calc_approx_answer(get_answer(), targets);
    };


    double score = get_score();

    double max_score = score;
    Answer max_answer = get_answer();

    dbg(max_score);

    const double START_TEMP = 100;
    const double END_TEMP = 1;

    double start_time = global_sw.msecs();

    int phase = 0;
    while (true) {
        double used = global_sw.msecs();
        if (used > time_limit) break;

        double temp = START_TEMP + (END_TEMP - START_TEMP) * (used - start_time) / (time_limit - start_time);

        int ty = uniform(0, 99);

        auto check = [&]() {
            double cur_score = get_score();
            if (max_score < cur_score) {
                max_score = cur_score;
                max_answer = get_answer();
                dbg("NEW BEST:", phase, max_score);
            }

            if (score <= cur_score) { //} || exp((cur_score - score) / temp) > random_01()) {
                score = cur_score;
                return true;
            }
            return false;
        };

        if (ty < 5) {
            int p_offset = offset;
            offset = random(0, M - 1);
            if (!check()) {
                offset = p_offset;
            }

            // double cur_score = get_score();
            // if (max_score < cur_score) {
            //     max_score = cur_score;
            //     dbg("NEW BEST:", phase, max_score);
            // } else {
            //     offset = p_offset;
            // }
        } else if (ty < 80) {
            int i = random(0, k - 1);
            int j = random(0, targets[i].len - 1);

            pos[i][j] ^= 1;
            if (!check()) {
                pos[i][j] ^= 1;
            }
            // double cur_score = get_score();
            // if (max_score < cur_score) {
            //     max_score = cur_score;
            //     dbg("NEW BEST:", phase, max_score);
            // } else {
            //     pos[i][j] ^= 1;
            // }
        } else {
            int i = random(0, k - 1);
            double pre_w = ws[i];
            double freq = 0.8 + random_01() * 0.4;
            ws[i] *= freq;

            if (!check()) {
                ws[i] = pre_w;
            }

            // double cur_score = get_score();
            // if (max_score < cur_score) {
            //     max_score = cur_score;
            //     dbg("NEW BEST:", phase, max_score);
            // } else {
            //     ws[i] = pre_w;
            // }
        }

        phase++;
    }

    dbg(phase, max_score);
    return max_answer;
}


int main() {
    Answer best_ans;
    double best_score = -1;

    V<TARGET> targets = {TARGETS.begin(), TARGETS.end()};
    sort(targets.begin(), targets.end(),
         [](const TARGET& a, const TARGET& b) { return a.point > b.point; });
    targets.resize(8);

    {
        const int TIMES = 5;
        for (int phase : iota(0, TIMES)) {
            auto ans = opt_path(targets, TL * (phase + 1) / TIMES);
            double score = calc_answer(ans);
            dbg("OPT_PATH", score);
            if (best_score < score) {
                best_score = score;
                best_ans = ans;
            }
        }
    }

    dbg("BEST SCORE", best_score);

    print_answer(best_ans);
    return 0;
}
