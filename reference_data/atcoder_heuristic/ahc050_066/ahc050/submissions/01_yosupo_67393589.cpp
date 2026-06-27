// #undef YOSUPO_LOCAL

#if 0 and !defined(__clang__)
#include <vector>
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#pragma GCC optimize("Ofast")
#endif


#include <array>
#include <cassert>
#include <initializer_list>
#include <string>


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

    bool contains(const Coord& t) const {
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

using f32 = float;
using f64 = double;

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

template <class T, int H, int W> struct Array2D {
    static_assert(H >= 0 && W >= 0);
    std::array<T, H * W> d;

    Array2D() : d{} {}
    Array2D(const T& val) { d.fill(val); }

    Array2D(std::initializer_list<std::initializer_list<T>> list) {
        assert(static_cast<int>(list.size()) == H);
        int i = 0;
        for (const auto& inner_list : list) {
            assert(static_cast<int>(inner_list.size()) == W);
            std::copy_n(inner_list.begin(), W, d.data() + i * W);
            i++;
        }
    }

    T& operator[](const Coord& idx) {
        assert(0 <= idx.r() && idx.r() < H);
        assert(0 <= idx.c() && idx.c() < W);
        return d[idx.r() * W + idx.c()];
    }
    const T& operator[](const Coord& idx) const {
        assert(0 <= idx.r() && idx.r() < H);
        assert(0 <= idx.c() && idx.c() < W);
        return d[idx.r() * W + idx.c()];
    }

    static Array2D e() {
        static_assert(H == W);
        Array2D mat;
        for (int i = 0; i < H; i++) {
            mat[{i, i}] = T(1);
        }
        return mat;
    }

    Array2D& operator+=(const Array2D& rhs) {
        for (int i = 0; i < H * W; i++) {
            d[i] += rhs.d[i];
        }
        return *this;
    }

    Array2D& operator-=(const Array2D& rhs) {
        for (int i = 0; i < H * W; i++) {
            d[i] -= rhs.d[i];
        }
        return *this;
    }

    friend Array2D operator+(const Array2D& lhs, const Array2D& rhs) {
        return Array2D(lhs) += rhs;
    }
    friend Array2D operator-(const Array2D& lhs, const Array2D& rhs) {
        return Array2D(lhs) -= rhs;
    }

    template <int K>
    friend Array2D<T, H, K> operator*(const Array2D& lhs,
                                      const Array2D<T, W, K>& rhs) {
        Array2D<T, H, K> res;
        Array2D<T, K, W> rhs_t = rhs.transpose();
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < K; j++) {
                for (int k = 0; k < W; k++) {
                    res[{i, j}] += lhs[{i, k}] * rhs_t[{j, k}];
                }
            }
        }
        return res;
    }

    Array2D& operator*=(const Array2D& r) {
        static_assert(H == W);
        return *this = *this * r;
    }

    Array2D<T, W, H> transpose() const {
        Array2D<T, W, H> res;
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                res[{j, i}] = (*this)[{i, j}];
            }
        }
        return res;
    }

    Array2D pow(long long n) const {
        static_assert(H == W);
        Array2D x = *this, r = e();
        while (n) {
            if (n & 1) r *= x;
            x *= x;
            n >>= 1;
        }
        return r;
    }

    std::string dump() const {
        std::string res;
        for (int i = 0; i < H; i++) {
            if (i) res += "\n";
            for (int j = 0; j < W; j++) {
                if (j) res += " ";
                res += yosupo::dump((*this)[{i, j}]);
            }
        }
        return res;
    }

    constexpr int height() const { return H; }
    constexpr int width() const { return W; }
};

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

#include <atcoder/scc>
#include <functional>


#include <bitset>
#include <cmath>
#include <iostream>
#include <queue>


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

constexpr int N = 40;
const int M = []() {
    int dummy, m;
    sc.read(dummy, m);
    return m;
}();
const int TURN = N * N - M;

const Array2D<bool, N, N> BOARD = []() {
    Array2D<bool, N, N> board;

    for (int i : iota(0, N)) {
        string s;
        sc.read(s);
        for (int j : iota(0, N)) {
            board[{i, j}] = (s[j] == '#');
        }
    }
    return board;
}();

struct State {
    double survival_prob = 1.0;
    double score = 0.0;
    array<uint64_t, N> used0;  // used0[r] & (1ULL << c) = (r, c) is used
    array<uint64_t, N> used1;  // used1[c] & (1ULL << r) = (r, c) is used
    Array2D<double, N, N> probs;

    State() {
        // Initialize used arrays with initial rocks
        for (int r : iota(0, N)) {
            used0[r] = 0;
            used1[r] = 0;
        }
        
        for (int r : iota(0, N)) {
            for (int c : iota(0, N)) {
                if (BOARD[{r, c}]) {
                    used0[r] |= (1ULL << c);
                    used1[c] |= (1ULL << r);
                }
            }
        }

        // Initialize probabilities for each empty cell
        double total_empty = N * N - M;
        for (int r : iota(0, N)) {
            for (int c : iota(0, N)) {
                if (!BOARD[{r, c}]) {
                    probs[{r, c}] = 1.0 / total_empty;
                } else {
                    probs[{r, c}] = 0.0;
                }
            }
        }

        robot_move();
    }

    void robot_move() {
        Array2D<double, N, N> new_probs;
        for (int r : iota(0, N)) {
            for (int c : iota(0, N)) {
                new_probs[{r, c}] = 0.0;
            }
        }

        // Process each row
        for (int r : iota(0, N)) {
            int start = 0;
            while (start < N) {
                // Skip rocks
                while (start < N && (used0[r] & (1ULL << start))) start++;
                if (start >= N) break;

                // Find continuous empty segment
                int end = start;
                while (end < N && !(used0[r] & (1ULL << end))) end++;

                // Calculate sum of probabilities in this segment
                double sum = 0.0;
                for (int c = start; c < end; c++) {
                    sum += probs[{r, c}];
                }

                // Distribute to endpoints (1/4 probability each direction)
                if (sum > 0) {
                    new_probs[{r, start}] += sum * 0.25;
                    new_probs[{r, end - 1}] += sum * 0.25;
                }

                start = end;
            }
        }

        // Process each column
        for (int c : iota(0, N)) {
            int start = 0;
            while (start < N) {
                // Skip rocks
                while (start < N && (used1[c] & (1ULL << start))) start++;
                if (start >= N) break;

                // Find continuous empty segment
                int end = start;
                while (end < N && !(used1[c] & (1ULL << end))) end++;

                // Calculate sum of probabilities in this segment
                double sum = 0.0;
                for (int r = start; r < end; r++) {
                    sum += probs[{r, c}];
                }

                // Distribute to endpoints (1/4 probability each direction)
                if (sum > 0) {
                    new_probs[{start, c}] += sum * 0.25;
                    new_probs[{end - 1, c}] += sum * 0.25;
                }

                start = end;
            }
        }

        probs = new_probs;
    }

    // put a rock at position c
    void to_next(Coord c) {
        assert(!(used0[c.r()] & (1ULL << c.c())));

        // Calculate survival probability (robot survives if it's not at
        // position c)
        double robot_at_c = probs[{c.r(), c.c()}];
        survival_prob -= robot_at_c;

        // Update score (we get 1 yen if robot survives)
        score += survival_prob;

        // Mark position as used
        used0[c.r()] |= (1ULL << c.c());
        used1[c.c()] |= (1ULL << c.r());
        probs[{c.r(), c.c()}] = 0.0;

        // First, simulate robot movement
        robot_move();
    }

    int to_problem_score() const {
        // Score = round(10^6 * E / (N^2 - M - 1))
        // where E is the expected value of final prize money
        double denominator = N * N - M - 1;
        return static_cast<int>(round(1000000.0 * score / denominator));
    }

    int count_components() const {
        Array2D<bool, N, N> visited;
        for (int r : iota(0, N)) {
            for (int c : iota(0, N)) {
                visited[{r, c}] = false;
            }
        }

        int components = 0;

        for (int r : iota(0, N)) {
            for (int c : iota(0, N)) {
                if (!visited[{r, c}] && !(used0[r] & (1ULL << c))) {
                    components++;

                    // BFS to mark all cells in this component
                    queue<Coord> q;
                    q.push(Coord{r, c});
                    visited[{r, c}] = true;

                    while (!q.empty()) {
                        Coord curr = q.front();
                        q.pop();

                        for (Coord next : curr.move4()) {
                            if (next.r() >= 0 && next.r() < N &&
                                next.c() >= 0 && next.c() < N &&
                                !visited[{next.r(), next.c()}] &&
                                !(used0[next.r()] & (1ULL << next.c()))) {
                                visited[{next.r(), next.c()}] = true;
                                q.push(next);
                            }
                        }
                    }
                }
            }
        }
        return components;
    }

  private:
    Array2D<int, N, N> coord_to_idx;
    array<double, N * N> cell_probs;  // Probability of each cell being empty
    array<array<int, 4>, N * N> transitions;
    array<Coord, N * N> empty_cells;

  public:
    // Fast lookahead that simulates X turns and returns variance
    double simulate_variance(int lookahead_turns, const vector<Coord>& sorted_empty_cells) {
        using D = double;

        // Build coordinate to index mapping and cell probabilities
        int n_cells = 0;
        for (const Coord& coord : sorted_empty_cells) {
            if (!(used0[coord.r()] & (1ULL << coord.c()))) {
                coord_to_idx[{coord.r(), coord.c()}] = n_cells;
                empty_cells[n_cells] = coord;
                cell_probs[n_cells] = probs[{coord.r(), coord.c()}];
                n_cells++;
            }
        }
        if (n_cells <= 1) return 0.0;

        for (int i = 0; i < n_cells; i++) {
            Coord pos = empty_cells[i];
            int r = pos.r(), c = pos.c();

            // Direction 0: right (c+1)
            transitions[i][0] = i;  // Default: stay at same position
            if (c + 1 < N) {
                uint64_t row_mask = used0[r];
                uint64_t right_mask =
                    ~((1ULL << (c + 1)) - 1);  // Clear bits 0 to c
                uint64_t blocked = row_mask & right_mask;
                int first_block = std::countr_zero(blocked);
                // If no blocks (first_block == 64), go to rightmost; otherwise
                // go just before first block
                int target_c = (first_block == 64) ? N - 1 : first_block - 1;
                if (target_c > c) {
                    transitions[i][0] = coord_to_idx[{r, target_c}];
                }
            }

            // Direction 1: down (r+1)
            transitions[i][1] = i;
            if (r + 1 < N) {
                uint64_t col_mask = used1[c];
                uint64_t down_mask = ~((1ULL << (r + 1)) - 1);
                uint64_t blocked = col_mask & down_mask;
                int first_block = std::countr_zero(blocked);
                int target_r = (first_block == 64) ? N - 1 : first_block - 1;
                if (target_r > r) {
                    transitions[i][1] = coord_to_idx[{target_r, c}];
                }
            }

            // Direction 2: left (c-1)
            transitions[i][2] = i;
            if (c > 0) {
                uint64_t row_mask = used0[r];
                uint64_t left_mask = (1ULL << c) - 1;  // Only bits 0 to c-1
                uint64_t blocked = row_mask & left_mask;
                int last_block =
                    (blocked == 0) ? -1 : 63 - std::countl_zero(blocked);
                int target_c = last_block + 1;
                if (target_c < c) {
                    transitions[i][2] = coord_to_idx[{r, target_c}];
                }
            }

            // Direction 3: up (r-1)
            transitions[i][3] = i;
            if (r > 0) {
                uint64_t col_mask = used1[c];
                uint64_t up_mask = (1ULL << r) - 1;
                uint64_t blocked = col_mask & up_mask;
                int last_block =
                    (blocked == 0) ? -1 : 63 - std::countl_zero(blocked);
                int target_r = last_block + 1;
                if (target_r < r) {
                    transitions[i][3] = coord_to_idx[{target_r, c}];
                }
            }
            //ranges::sort(transitions[i]);
        }

        /*
                // Build SCC graph
                atcoder::scc_graph scc_graph(n_cells);
                for (int i = 0; i < n_cells; i++) {
                    for (int dir = 0; dir < 4; dir++) {
                        if (transitions[i][dir] != i) {  // Skip self-loops
           initially scc_graph.add_edge(i, transitions[i][dir]);
                        }
                    }
                }

                // Get SCCs in topological order
                auto sccs = scc_graph.scc();

                // Process each SCC in topological order
                for (const auto& scc : sccs) {
                    // Simulate robot movements by processing each SCC
           separately for (int scc_round = 0; scc_round < lookahead_turns;
           scc_round++) {
                        // First, distribute probability from current SCC to
           transitions for (int node : scc) { if (cell_probs[node] <= 0)
           continue; D prob = D(cell_probs[node] * 0.25); cell_probs[node] =
           0.0;

                            for (int dir = 0; dir < 4; dir++) {
                                cell_probs[transitions[node][dir]] += prob;
                            }
                        }
                    }
                }
        */

        for (int _ : iota(0, lookahead_turns)) {
            // First, distribute probability from current SCC to transitions
            for (int node : iota(0, n_cells)) {
                if (cell_probs[node] <= 1e-5) continue;
                D prob = D(cell_probs[node] * 0.25);
                cell_probs[node] = 0.0;

                for (int dir = 0; dir < 4; dir++) {
                    cell_probs[transitions[node][dir]] += prob;
                }
            }
        }

        // Calculate variance like smt
        double sum = 0.0;
        for (int i = 0; i < n_cells; i++) {
            double p = cell_probs[i];
            if (p <= 0) continue;
            sum += p * p; //pow(p, 3.0);
        }

        //double mean = sum / count;
        return sqrt(sum); //sqrt(sum_sq / count - mean * mean);
    }
};

int main() {
    State state = State();

    // Create list of empty positions sorted by distance from center
    vector<Coord> empty_positions;
    for (int r : iota(0, N)) {
        for (int c : iota(0, N)) {
            if (!BOARD[{r, c}]) {
                empty_positions.push_back(Coord{r, c});
            }
        }
    }
    assert((int)empty_positions.size() == TURN);
    
    // Strategy: maximize probability variance by looking ahead
    vector<Coord> sequence;
    //const int K = 20;  // Number of candidates to consider

    for (int i = 0; i < TURN; i++) {
        dbg(i);
        double t = 1.0 * (i + 1) / TURN;

        // Adjust lookahead turns: start from 50, end at 10
        int lookahead_turns = static_cast<int>(std::lerp(40.0, 10.0, t) * 1.1);
        const int K = static_cast<int>(std::lerp(30.0, 5.0, t) * 2.1);
        
        // Create current sorted empty cells for efficient simulate_variance
        vector<Coord> sorted_empty_cells = empty_positions;
        ranges::sort(sorted_empty_cells, [](const Coord& a, const Coord& b) {
            return a.r() + a.c() < b.r() + b.c();
        });

        // Get k candidates with lowest probability
        vector<pair<double, Coord>> candidates;
        for (const Coord& c : empty_positions) {
            candidates.push_back({state.probs[{c.r(), c.c()}], c});
        }
        ranges::sort(candidates, [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        // Evaluate top K candidates by simulating ahead
        Coord best_pos = candidates[0].second;
        double best_score = -1e9;

        int num_candidates = min(K, (int)candidates.size());
        //int components_before = state.count_components();

        for (int k = 0; k < num_candidates; k++) {
            Coord c = candidates[k].second;

            // Save original probability (used0/used1 are always false for empty
            // cells)
            double original_prob = state.probs[{c.r(), c.c()}];

            // Temporarily place rock at candidate position
            state.used0[c.r()] |= (1ULL << c.c());
            state.used1[c.c()] |= (1ULL << c.r());
            state.probs[{c.r(), c.c()}] = 0.0;

            // Simulate state after placing rock
            //int components_after = state.count_components();
            // Use fast lookahead simulation
            double variance = state.simulate_variance(lookahead_turns, sorted_empty_cells);

            // Restore original state (reset bits and restore probability)
            state.used0[c.r()] &= ~(1ULL << c.c());
            state.used1[c.c()] &= ~(1ULL << c.r());
            state.probs[{c.r(), c.c()}] = original_prob;


            //int component_delta = components_after - components_before;

            // Combined score: maximize variance, minimize component increase
            double freq1 = pow(t, 1.0) * 10.0;
            //double freq2 = pow(1.0 - t, 10.0) * 0.00001;
            double score = 0.0;
            score += variance;
            score -= freq1 * candidates[k].first;
            //score -= freq2 * component_delta;

            if (score > best_score) {
                best_score = score;
                best_pos = c;
            }
        }

        // Output and place rock at best position
        pr.writeln(best_pos.r(), ' ', best_pos.c());
        sequence.push_back(best_pos);

        // Remove the placed position from empty_positions
        empty_positions.erase(
            ranges::find(empty_positions, best_pos)
        );

        // Update state (without robot_move since we already did it)
        state.to_next(best_pos);
    }

    // Output final score
    dbg(state.score);
    dbg(state.to_problem_score());

    return 0;
}
