#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
//#pragma GCC optimize("Ofast")
//#undef LOCAL


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


#include <cassert>
#include <numeric>
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

#include <array>
#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <type_traits>

namespace yosupo {

struct Xoshiro256StarStar {
  public:
    using result_type = uint64_t;
    Xoshiro256StarStar() : Xoshiro256StarStar(0) {}
    explicit Xoshiro256StarStar(uint64_t seed) {
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
    static uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    std::array<uint64_t, 4> s;
};

namespace internal {

template <class G> uint64_t uniform(uint64_t upper, G& gen) {
    static_assert(std::is_same<uint64_t, typename G::result_type>::value, "");
    static_assert(G::min() == 0, "");
    static_assert(G::max() == uint64_t(-1), "");
    if (!(upper & (upper + 1))) {
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
using namespace yosupo;

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <utility>

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

struct StopWatch {
    bool f = false;
    clock_t st;
    void start() {
        f = true;
        st = clock();
    }
    int msecs() {
        assert(f);
        return int(1LL * (clock() - st) * 1000 / CLOCKS_PER_SEC);
    }
};

Scanner sc = Scanner(stdin);
Printer pr = Printer(stdout);

struct P {
    int r, c;
    P() : r(0), c(0) {}
    P(int _r, int _c): r(_r), c(_c) {}

    friend P operator-(const P& l, const P& r) {
        return P(l.r - r.r, l.c - r.c);
    }

    int abs() const {
        return std::abs(r) + std::abs(c);
    }

    friend ostream& operator<<(ostream& os, const P& p) {
        return os << "(" << p.r << ", " << p.c << ")";
    }
};

const int A = 26;
const int N = 15;
const int M = 200;

int SR, SC;

array<array<int, N>, N> G;
array<array<int, 5>, M> T;

//
array<V<P>, A> pos;
array<array<int, M>, M + 1> need;

int min_score(P src, P dst, const V<int>& mid) {
    using ST = pair<P, int>;

    V<ST> st, nst;
    st.push_back({src, 0});

    for (auto c : mid) {
        nst.clear();
        for (auto p : pos[c]) {
            int sco = TEN(9);
            for (auto s: st) {
                sco = min(sco, (s.first - p).abs() + 1 + s.second);
            }
            nst.push_back({p, sco});
        }
        st = nst;
    }

    int ans = TEN(9);
    for (auto s : st) {
        ans = min(ans, (s.first - dst).abs() + 1 + s.second);
    }
    return ans;
}

V<int> min_scores(P src, int dst, const V<int>& mid) {
    using ST = pair<P, int>;

    static V<ST> st, nst;
    st.clear();
    st.push_back({src, 0});

    for (auto c : mid) {
        nst.clear();
        for (auto p : pos[c]) {
            int sco = TEN(9);
            for (auto s : st) {
                sco = min(sco, (s.first - p).abs() + 1 + s.second);
            }
            nst.push_back({p, sco});
        }
        st = nst;
    }

    V<int> ans;
    for (auto p : pos[dst]) {
        int now = TEN(9);
        for (auto s : st) {
            now = min(now, (s.first - p).abs() + 1 + s.second);
        }
        ans.push_back(now);
    }
    return ans;
}

V<P> min_path(P src, P dst, V<int> mid) {
    int k = int(mid.size());

    int score = TEN(9);
    V<P> ans;

    int now = 0;
    V<P> path;
    path.push_back(src);
    auto dfs = [&](auto self, int i) -> void {
        if (i == k) {
            int now2 = now + (path.back() - dst).abs() + 1;
            path.push_back(dst);

            if (now2 < score) {
                score = now2;
                ans = path;
            }

            path.pop_back();
            return;
        }

        for (auto p : pos[mid[i]]) {
            int pnow = now;
            now += (path.back() - p).abs() + 1;
            path.push_back(p);
            self(self, i + 1);      
            path.pop_back();
            now = pnow;
        }
    };

    dfs(dfs, 0);
    return {ans.begin() + 1, ans.end()};
}

uint32_t xor32(void) {
  static uint32_t y = 2463534242;
  y = y ^ (y << 13);
  y = y ^ (y >> 17);
  y = y ^ (y << 5);
  return y;
}

int rand_bool() {
    return xor32() % 2;
}
int rand_int(int up) {
    // [0, u)
    return (int)((uint64_t(xor32()) * up) >> 32);
}
int rand_int(int lw, int up) {
    return rand_int(up - lw) + lw;
}

V<P> solve() {
    StopWatch sw;
    sw.start();
    
    V<int> v(M), vs(M + 1);
    V<P> to_p;
    V<int> id_2_t;
    for (int i = 0; i < M; i++) {
        v[i] = int(pos[T[i][4]].size());
        for (int j = 0; j < v[i]; j++) {
            to_p.push_back({i, j});
            id_2_t.push_back(i);
        }
    }
    for (int i = 0; i < M; i++) {
        vs[i + 1] = vs[i] + v[i];   
    }
    to_p.push_back({M, 0});
    id_2_t.push_back(M);
    int S = vs.back(), VN = S + 1;

    V<int> costs_1d;
    costs_1d.reserve((VN + 1) * VN);
    {
        VV<int> costs(VN + 1, V<int>(VN));

        for (int i = 0; i <= M; i++) {
            for (int j = 0; j < M; j++) {
                if (i == j) continue;
                V<int> mid(T[j].begin() + (5 - need[i][j]), T[j].begin() + 4);

                int num = (i < M) ? v[i] : 1;
                for (int k = 0; k < num; k++) {
                    int id1 = vs[i] + k;
                    P src = (i < M) ? pos[T[i][4]][k] : P(SR, SC);

                    V<int> scores = min_scores(src, T[j][4], mid);
                    for (int l = 0; l < v[j]; l++) {
                        int id2 = vs[j] + l;
                        costs[id1][id2] = scores[l];
                    }
                }
            }
        }

        for (int i = 0; i <= VN; i++) {
            for (int j = 0; j < VN; j++) {
                costs_1d.push_back(costs[i][j]);
            }
        }
    }
    auto costs = [&](int i, int j) {
        return costs_1d[i * VN + j];
    };

    dbg(sw.msecs());

    auto to_id = [&](int i, int j) { return vs[i] + j; };

    V<int> route;
    route.push_back(to_id(M, 0));
    for (int i = 0; i < M; i++) {
        route.push_back(to_id(i, 0));
    }
    route.push_back(to_id(M, 0));


    auto eval_score = [&]() {
        int score = 0;
        for (int i = 0; i < M; i++) {
            score += costs(route[i], route[i + 1]);
        }
        return score;
    };

    int score = eval_score();

    int times = 0;
    while (true) {
        const int END_MS = 2000 - 50;
        int msecs = sw.msecs();
        if (msecs > END_MS) break;

        double start_tmp = 1;
        double end_tmp = 0.005;

        double tmp = ((END_MS - msecs) * start_tmp + msecs * end_tmp) / END_MS;

        const int MAX_INC = 300;
        array<double, MAX_INC> probs;
        for (int i = 0; i < MAX_INC; i++) {
            probs[i] = exp(-i / tmp) * pow(2, 32);
        }

        auto eval = [&](int dec) {
            if (dec >= 0 || probs[-dec] > xor32()) {
                score -= dec;
                return true;
            } else {
                return false;
            }
        };

        for (int ph = 0; ph < 100000; ph++) {
            times++;

            //int ty = rand_bool();
            int ty = (ph % 16);

            if (ty == 0 || ty == 5 || ty == 11) {
                int i = rand_int(1, M + 1);
                int ri = id_2_t[route[i]];
                int j = rand_int(0, v[ri]);

                int lid = route[i - 1];
                int rid = route[i + 1];

                int id0 = route[i];
                int id1 = to_id(ri, j);

                int sco0 = costs(lid, id0) + costs(id0, rid);
                int sco1 = costs(lid, id1) + costs(id1, rid);

                if (eval(sco0 - sco1)) {
                    route[i] = id1;
                }
            } else {
/*                
                int l = uniform(1, M - 2);
                int m = uniform(l + 1, M - 1);
                int r = uniform(m + 1, M);
*/

/*
                int l, m, r;
                do {
                    l = rand_int(1, M + 1);
                    m = rand_int(1, M + 1);
                    r = rand_int(1, M + 1);
                } while (!(l < m && m < r));
*/
                int m = rand_int(2, M);
                int l = rand_int(1, m);
                int r = rand_int(m + 1, M + 1);


                int id0 = route[l - 1];
                int id1 = route[l];
                int id2 = route[m - 1];
                int id3 = route[m];
                int id4 = route[r - 1];
                int id5 = route[r];

                int sco0 = costs(id0, id1) + costs(id2, id3) + costs(id4, id5);
                int sco1 = costs(id0, id3) + costs(id4, id1) + costs(id2, id5);

                if (eval(sco0 - sco1)) {
                    rotate(route.begin() + l, route.begin() + m, route.begin() + r);
                }
            }
        }

        //dbg("score: ", tmp, score);
    }

    assert(score == eval_score());
    dbg(score, times);

    // restore
    V<P> ans;
    {
        P bk = {SR, SC};
        int bi = M;
        for (auto id : route) {
            auto p = to_p[id];
            if (p.r == M) continue;
            int i = p.r;
            P dst = pos[T[i][4]][p.c];
            V<int> mid(T[i].begin() + (5 - need[bi][i]), T[i].begin() + 4);
            auto path = min_path(bk, dst, mid);
            ans.insert(ans.end(), path.begin(), path.end());

            bk = dst;
            bi = i;
        }
    }
    return ans;
}

void init() {    
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            pos[G[r][c]].push_back({r, c});
        }
    }

    for (int i = 0; i < M; i++) {
        need[i].fill(-1);
        for (int j = 0; j < M; j++) {
            for (int mt = 5; mt >= 0; mt--) {
                bool ok = true;
                for (int k = 0; k < mt; k++) {
                    if (T[i][5 - mt + k] != T[j][k]) ok = false;
                }
                if (ok) {
                    need[i][j] = 5 - mt;
                    break;
                }
            }
        }
    }
    need[M].fill(5);
}

int main() {
    int n, m;
    sc.read(n, m);
    assert(n == N && m == M);
    sc.read(SR, SC);
    for (int r = 0; r < N; r++) {
        string s;
        sc.read(s);
        for (int c = 0; c < N; c++) {
            G[r][c] = s[c] - 'A';
        }
    }

    for (int i = 0; i < M; i++) {
        string s;
        sc.read(s);
        for (int j = 0; j < 5; j++) {
            T[i][j] = s[j] - 'A';
        }
    }

    init();

    auto ans = solve();
    
    int score = 0;
    P bk = {SR, SC};
    for (auto p : ans) {
        pr.writeln(p.r, ' ', p.c);

        score += (bk - p).abs() + 1;
        bk = p;
    }

    dbg("SCORE:", score);


    return 0;
}
