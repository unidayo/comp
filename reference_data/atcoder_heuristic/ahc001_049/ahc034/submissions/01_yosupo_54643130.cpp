#line 1 "A/main.cpp"
#include <algorithm>
#define YOSUPO_AVX2_PRAGMA
// #undef YOSUPO_LOCAL

#line 2 "/home/vscode/yosupo-library/src/yosupo/fastio.hpp"

#include <unistd.h>
#line 5 "/home/vscode/yosupo-library/src/yosupo/fastio.hpp"
#include <array>
#include <bit>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#line 2 "/home/vscode/yosupo-library/src/yosupo/internal_type_traits.hpp"

#line 5 "/home/vscode/yosupo-library/src/yosupo/internal_type_traits.hpp"

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
#line 2 "/home/vscode/yosupo-library/src/yosupo/stopwatch.hpp"

#include <chrono>

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
#line 1 "/home/vscode/ac-library/atcoder/maxflow.hpp"



#line 6 "/home/vscode/ac-library/atcoder/maxflow.hpp"
#include <limits>
#include <queue>
#line 9 "/home/vscode/ac-library/atcoder/maxflow.hpp"

#line 1 "/home/vscode/ac-library/atcoder/internal_queue.hpp"



#line 5 "/home/vscode/ac-library/atcoder/internal_queue.hpp"

namespace atcoder {

namespace internal {

template <class T> struct simple_queue {
    std::vector<T> payload;
    int pos = 0;
    void reserve(int n) { payload.reserve(n); }
    int size() const { return int(payload.size()) - pos; }
    bool empty() const { return pos == int(payload.size()); }
    void push(const T& t) { payload.push_back(t); }
    T& front() { return payload[pos]; }
    void clear() {
        payload.clear();
        pos = 0;
    }
    void pop() { pos++; }
};

}  // namespace internal

}  // namespace atcoder


#line 11 "/home/vscode/ac-library/atcoder/maxflow.hpp"

namespace atcoder {

template <class Cap> struct mf_graph {
  public:
    mf_graph() : _n(0) {}
    explicit mf_graph(int n) : _n(n), g(n) {}

    int add_edge(int from, int to, Cap cap) {
        assert(0 <= from && from < _n);
        assert(0 <= to && to < _n);
        assert(0 <= cap);
        int m = int(pos.size());
        pos.push_back({from, int(g[from].size())});
        int from_id = int(g[from].size());
        int to_id = int(g[to].size());
        if (from == to) to_id++;
        g[from].push_back(_edge{to, to_id, cap});
        g[to].push_back(_edge{from, from_id, 0});
        return m;
    }

    struct edge {
        int from, to;
        Cap cap, flow;
    };

    edge get_edge(int i) {
        int m = int(pos.size());
        assert(0 <= i && i < m);
        auto _e = g[pos[i].first][pos[i].second];
        auto _re = g[_e.to][_e.rev];
        return edge{pos[i].first, _e.to, _e.cap + _re.cap, _re.cap};
    }
    std::vector<edge> edges() {
        int m = int(pos.size());
        std::vector<edge> result;
        for (int i = 0; i < m; i++) {
            result.push_back(get_edge(i));
        }
        return result;
    }
    void change_edge(int i, Cap new_cap, Cap new_flow) {
        int m = int(pos.size());
        assert(0 <= i && i < m);
        assert(0 <= new_flow && new_flow <= new_cap);
        auto& _e = g[pos[i].first][pos[i].second];
        auto& _re = g[_e.to][_e.rev];
        _e.cap = new_cap - new_flow;
        _re.cap = new_flow;
    }

    Cap flow(int s, int t) {
        return flow(s, t, std::numeric_limits<Cap>::max());
    }
    Cap flow(int s, int t, Cap flow_limit) {
        assert(0 <= s && s < _n);
        assert(0 <= t && t < _n);
        assert(s != t);

        std::vector<int> level(_n), iter(_n);
        internal::simple_queue<int> que;

        auto bfs = [&]() {
            std::fill(level.begin(), level.end(), -1);
            level[s] = 0;
            que.clear();
            que.push(s);
            while (!que.empty()) {
                int v = que.front();
                que.pop();
                for (auto e : g[v]) {
                    if (e.cap == 0 || level[e.to] >= 0) continue;
                    level[e.to] = level[v] + 1;
                    if (e.to == t) return;
                    que.push(e.to);
                }
            }
        };
        auto dfs = [&](auto self, int v, Cap up) {
            if (v == s) return up;
            Cap res = 0;
            int level_v = level[v];
            for (int& i = iter[v]; i < int(g[v].size()); i++) {
                _edge& e = g[v][i];
                if (level_v <= level[e.to] || g[e.to][e.rev].cap == 0) continue;
                Cap d =
                    self(self, e.to, std::min(up - res, g[e.to][e.rev].cap));
                if (d <= 0) continue;
                g[v][i].cap += d;
                g[e.to][e.rev].cap -= d;
                res += d;
                if (res == up) return res;
            }
            level[v] = _n;
            return res;
        };

        Cap flow = 0;
        while (flow < flow_limit) {
            bfs();
            if (level[t] == -1) break;
            std::fill(iter.begin(), iter.end(), 0);
            Cap f = dfs(dfs, t, flow_limit - flow);
            if (!f) break;
            flow += f;
        }
        return flow;
    }

    std::vector<bool> min_cut(int s) {
        std::vector<bool> visited(_n);
        internal::simple_queue<int> que;
        que.push(s);
        while (!que.empty()) {
            int p = que.front();
            que.pop();
            visited[p] = true;
            for (auto e : g[p]) {
                if (e.cap && !visited[e.to]) {
                    visited[e.to] = true;
                    que.push(e.to);
                }
            }
        }
        return visited;
    }

  private:
    int _n;
    struct _edge {
        int to, rev;
        Cap cap;
    };
    std::vector<std::pair<int, int>> pos;
    std::vector<std::vector<_edge>> g;
};

}  // namespace atcoder


#line 2 "/home/vscode/yosupo-library/src/yosupo/random.hpp"

#line 8 "/home/vscode/yosupo-library/src/yosupo/random.hpp"
#include <random>
#line 10 "/home/vscode/yosupo-library/src/yosupo/random.hpp"

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

// https://github.com/wangyi-fudan/wyhash
struct WYRand {
  public:
    using result_type = uint64_t;
    explicit WYRand(uint64_t seed) : s(seed) {}

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return -1; }

    result_type operator()() {
        s += 0x2d358dccaa6c78a5;
        auto x = (unsigned __int128)s * (s ^ 0x8bb84b93962eacc9);
        return (uint64_t)(x ^ (x >> 64));
    }

  private:
    uint64_t s;
};
using Random = WYRand;
inline Random& global_gen() {
    static Random gen(
        std::chrono::steady_clock::now().time_since_epoch().count());
    return gen;
}

template <class G>
concept random_64 = std::uniform_random_bit_generator<G> &&
                    std::same_as<uint64_t, std::invoke_result_t<G&>> &&
                    G::min() == uint64_t(0) && G::max() == uint64_t(-1);

namespace internal {

// random choice from [0, upper]
template <random_64 G> uint64_t uniform_u64(uint64_t upper, G& gen) {
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

// random choice from [0, upper], faster than uniform_u64
template <random_64 G> uint64_t random_u64(uint64_t upper, G& gen) {
    return (uint64_t)(((unsigned __int128)(upper) + 1) * gen() >> 64);
}

}  // namespace internal

template <class T, random_64 G> T uniform(T lower, T upper, G& gen) {
    return T(lower +
             internal::uniform_u64(uint64_t(upper) - uint64_t(lower), gen));
}
template <class T> T uniform(T lower, T upper) {
    return uniform(lower, upper, global_gen());
}
template <class T, random_64 G> T random(T lower, T upper, G& gen) {
    return T(lower +
             internal::random_u64(uint64_t(upper) - uint64_t(lower), gen));
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

// random value in the interval (0.0, 1.0]
template <class G> inline double open_closed_01(G& gen) {
    union {
        uint64_t i;
        double f;
    } u = {0xbff0000000000000 | (gen() >> 12)};
    return 2.0 + u.f;
}
inline double open_closed_01() {
    return open_closed_01(global_gen());
}

}  // namespace yosupo
#line 2 "A/lemon.hpp"

/* -*- mode: C++; indent-tabs-mode: nil; -*-
 *
 * This file is a part of LEMON, a generic C++ optimization library.
 *
 * Copyright (C) 2003-2013
 * Egervary Jeno Kombinatorikus Optimalizalasi Kutatocsoport
 * (Egervary Research Group on Combinatorial Optimization, EGRES).
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies. For
 * precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */
#line 22 "A/lemon.hpp"
#include <cmath>
#line 24 "A/lemon.hpp"
#include <utility>
#line 26 "A/lemon.hpp"

/// \addtogroup min_cost_flow_algs
/// @{

/// \brief Implementation of the primal Network Simplex algorithm
/// for finding a \ref min_cost_flow "minimum cost flow".
///
/// \ref NetworkSimplex implements the primal Network Simplex algorithm
/// for finding a \ref min_cost_flow "minimum cost flow"
/// \cite amo93networkflows, \cite dantzig63linearprog,
/// \cite kellyoneill91netsimplex.
/// This algorithm is a highly efficient specialized version of the
/// linear programming simplex method directly for the minimum cost
/// flow problem.
///
/// In general, \ref NetworkSimplex and \ref CostScaling are the fastest
/// implementations available in LEMON for solving this problem.
/// (For more information, see \ref min_cost_flow_algs "the module page".)
/// Furthermore, this class supports both directions of the supply/demand
/// inequality constraints. For more information, see \ref SupplyType.
///
/// Most of the parameters of the problem (except for the digraph)
/// can be given using separate functions, and the algorithm can be
/// executed using the \ref run() function. If some parameters are not
/// specified, then default values will be used.
///
/// \tparam GR The digraph type the algorithm runs on.
/// \tparam V The number type used for flow amounts, capacity bounds
/// and supply values in the algorithm. By default, it is \c int.
/// \tparam C The number type used for costs and potentials in the
/// algorithm. By default, it is the same as \c V.
///
/// \warning Both \c V and \c C must be signed number types.
/// \warning All input data (capacities, supply values, and costs) must
/// be integer.
///
/// \note %NetworkSimplex provides five different pivot rule
/// implementations, from which the most efficient one is used
/// by default. For more information, see \ref PivotRule.
template <class Digraph, typename V = int, typename C = V> class NetworkSimplex {
public:
    using Node = int;
    using Arc = int;
    static const int INVALID = -1;
    /// The type of the flow amounts, capacity bounds and supply values
    typedef V Value;
    /// The type of the arc costs
    typedef C Cost;

public:
    /// \brief Problem type constants for the \c run() function.
    ///
    /// Enum type containing the problem type constants that can be
    /// returned by the \ref run() function of the algorithm.
    enum ProblemType {
        /// The problem has no feasible solution (flow).
        INFEASIBLE,
        /// The problem has optimal solution (i.e. it is feasible and
        /// bounded), and the algorithm has found optimal flow and node
        /// potentials (primal and dual solutions).
        OPTIMAL,
        /// The objective function of the problem is unbounded, i.e.
        /// there is a directed cycle having negative total cost and
        /// infinite upper bound.
        UNBOUNDED
    };

    /// \brief Constants for selecting the type of the supply constraints.
    ///
    /// Enum type containing constants for selecting the supply type,
    /// i.e. the direction of the inequalities in the supply/demand
    /// constraints of the \ref min_cost_flow "minimum cost flow problem".
    ///
    /// The default supply type is \c GEQ, the \c LEQ type can be
    /// selected using \ref supplyType().
    /// The equality form is a special case of both supply types.
    enum SupplyType {
        /// This option means that there are <em>"greater or equal"</em>
        /// supply/demand constraints in the definition of the problem.
        GEQ,
        /// This option means that there are <em>"less or equal"</em>
        /// supply/demand constraints in the definition of the problem.
        LEQ
    };

    /// \brief Constants for selecting the pivot rule.
    ///
    /// Enum type containing constants for selecting the pivot rule for
    /// the \ref run() function.
    ///
    /// \ref NetworkSimplex provides five different implementations for
    /// the pivot strategy that significantly affects the running time
    /// of the algorithm.
    /// According to experimental tests conducted on various problem
    /// instances, \ref BLOCK_SEARCH "Block Search" and
    /// \ref ALTERING_LIST "Altering Candidate List" rules turned out
    /// to be the most efficient.
    /// Since \ref BLOCK_SEARCH "Block Search" is a simpler strategy that
    /// seemed to be slightly more robust, it is used by default.
    /// However, another pivot rule can easily be selected using the
    /// \ref run() function with the proper parameter.
    enum PivotRule {

        /// The \e First \e Eligible pivot rule.
        /// The next eligible arc is selected in a wraparound fashion
        /// in every iteration.
        FIRST_ELIGIBLE,

        /// The \e Best \e Eligible pivot rule.
        /// The best eligible arc is selected in every iteration.
        BEST_ELIGIBLE,

        /// The \e Block \e Search pivot rule.
        /// A specified number of arcs are examined in every iteration
        /// in a wraparound fashion and the best eligible arc is selected
        /// from this block.
        BLOCK_SEARCH,

        /// The \e Candidate \e List pivot rule.
        /// In a major iteration a candidate list is built from eligible arcs
        /// in a wraparound fashion and in the following minor iterations
        /// the best eligible arc is selected from this list.
        CANDIDATE_LIST,

        /// The \e Altering \e Candidate \e List pivot rule.
        /// It is a modified version of the Candidate List method.
        /// It keeps only a few of the best eligible arcs from the former
        /// candidate list and extends this list in every iteration.
        ALTERING_LIST
    };

private:
    using IntVector = std::vector<int>;
    using ValueVector = std::vector<Value>;
    using CostVector = std::vector<Cost>;
    using CharVector = std::vector<signed char>;
    // Note: vector<signed char> is used instead of vector<ArcState> and
    // vector<ArcDirection> for efficiency reasons

    // State constants for arcs
    enum ArcState { STATE_UPPER = -1, STATE_TREE = 0, STATE_LOWER = 1 };

    // Direction constants for tree arcs
    enum ArcDirection { DIR_DOWN = -1, DIR_UP = 1 };

private:
    // Data related to the underlying digraph
    const Digraph &_graph;
    int _node_num;
    int _arc_num;
    int _all_arc_num;
    int _search_arc_num;

    // Parameters of the problem
    bool _has_lower;
    SupplyType _stype;
    Value _sum_supply;

    // Data structures for storing the digraph
    IntVector _source;
    IntVector _target;
    bool _arc_mixing;

    // Node and arc data
    ValueVector _lower;
    ValueVector _upper;
    ValueVector _cap;
    CostVector _cost;
    ValueVector _supply;
    ValueVector _flow;
    CostVector _pi;

    // Data for storing the spanning tree structure
    IntVector _parent;
    IntVector _pred;
    IntVector _thread;
    IntVector _rev_thread;
    IntVector _succ_num;
    IntVector _last_succ;
    CharVector _pred_dir;
    CharVector _state;
    IntVector _dirty_revs;
    int _root;

    // Temporary data used in the current pivot iteration
    int in_arc, join, u_in, v_in, u_out, v_out;
    Value delta;

    const Value MAX;

public:
    /// \brief Constant for infinite upper bounds (capacities).
    ///
    /// Constant for infinite upper bounds (capacities).
    /// It is \c std::numeric_limits<Value>::infinity() if available,
    /// \c std::numeric_limits<Value>::max() otherwise.
    const Value INF;

private:
    // Implementation of the First Eligible pivot rule
    class FirstEligiblePivotRule {
    private:
        // References to the NetworkSimplex class
        const IntVector &_source;
        const IntVector &_target;
        const CostVector &_cost;
        const CharVector &_state;
        const CostVector &_pi;
        int &_in_arc;
        int _search_arc_num;

        // Pivot rule data
        int _next_arc;

    public:
        // Constructor
        FirstEligiblePivotRule(NetworkSimplex &ns)
            : _source(ns._source), _target(ns._target), _cost(ns._cost), _state(ns._state), _pi(ns._pi), _in_arc(ns.in_arc), _search_arc_num(ns._search_arc_num), _next_arc(0) {}

        // Find next entering arc
        bool findEnteringArc() {
            Cost c;
            for (int e = _next_arc; e != _search_arc_num; ++e) {
                c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                if (c < 0) {
                    _in_arc = e;
                    _next_arc = e + 1;
                    return true;
                }
            }
            for (int e = 0; e != _next_arc; ++e) {
                c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                if (c < 0) {
                    _in_arc = e;
                    _next_arc = e + 1;
                    return true;
                }
            }
            return false;
        }

    }; // class FirstEligiblePivotRule

    // Implementation of the Best Eligible pivot rule
    class BestEligiblePivotRule {
    private:
        // References to the NetworkSimplex class
        const IntVector &_source;
        const IntVector &_target;
        const CostVector &_cost;
        const CharVector &_state;
        const CostVector &_pi;
        int &_in_arc;
        int _search_arc_num;

    public:
        // Constructor
        BestEligiblePivotRule(NetworkSimplex &ns)
            : _source(ns._source), _target(ns._target), _cost(ns._cost), _state(ns._state), _pi(ns._pi), _in_arc(ns.in_arc), _search_arc_num(ns._search_arc_num) {}

        // Find next entering arc
        bool findEnteringArc() {
            Cost c, min = 0;
            for (int e = 0; e != _search_arc_num; ++e) {
                c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                if (c < min) {
                    min = c;
                    _in_arc = e;
                }
            }
            return min < 0;
        }

    }; // class BestEligiblePivotRule

    // Implementation of the Block Search pivot rule
    class BlockSearchPivotRule {
    private:
        // References to the NetworkSimplex class
        const IntVector &_source;
        const IntVector &_target;
        const CostVector &_cost;
        const CharVector &_state;
        const CostVector &_pi;
        int &_in_arc;
        int _search_arc_num;

        // Pivot rule data
        int _block_size;
        int _next_arc;

    public:
        // Constructor
        BlockSearchPivotRule(NetworkSimplex &ns)
            : _source(ns._source), _target(ns._target), _cost(ns._cost), _state(ns._state), _pi(ns._pi), _in_arc(ns.in_arc), _search_arc_num(ns._search_arc_num), _next_arc(0) {
            // The main parameters of the pivot rule
            const double BLOCK_SIZE_FACTOR = 1.0;
            const int MIN_BLOCK_SIZE = 10;

            _block_size = std::max(int(BLOCK_SIZE_FACTOR * std::sqrt(double(_search_arc_num))), MIN_BLOCK_SIZE);
        }

        // Find next entering arc
        bool findEnteringArc() {
            Cost c, min = 0;
            int cnt = _block_size;
            int e;
            for (e = _next_arc; e != _search_arc_num; ++e) {
                c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                if (c < min) {
                    min = c;
                    _in_arc = e;
                }
                if (--cnt == 0) {
                    if (min < 0) goto search_end;
                    cnt = _block_size;
                }
            }
            for (e = 0; e != _next_arc; ++e) {
                c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                if (c < min) {
                    min = c;
                    _in_arc = e;
                }
                if (--cnt == 0) {
                    if (min < 0) goto search_end;
                    cnt = _block_size;
                }
            }
            if (min >= 0) return false;

        search_end:
            _next_arc = e;
            return true;
        }

    }; // class BlockSearchPivotRule

    // Implementation of the Candidate List pivot rule
    class CandidateListPivotRule {
    private:
        // References to the NetworkSimplex class
        const IntVector &_source;
        const IntVector &_target;
        const CostVector &_cost;
        const CharVector &_state;
        const CostVector &_pi;
        int &_in_arc;
        int _search_arc_num;

        // Pivot rule data
        IntVector _candidates;
        int _list_length, _minor_limit;
        int _curr_length, _minor_count;
        int _next_arc;

    public:
        /// Constructor
        CandidateListPivotRule(NetworkSimplex &ns)
            : _source(ns._source), _target(ns._target), _cost(ns._cost), _state(ns._state), _pi(ns._pi), _in_arc(ns.in_arc), _search_arc_num(ns._search_arc_num), _next_arc(0) {
            // The main parameters of the pivot rule
            const double LIST_LENGTH_FACTOR = 0.25;
            const int MIN_LIST_LENGTH = 10;
            const double MINOR_LIMIT_FACTOR = 0.1;
            const int MIN_MINOR_LIMIT = 3;

            _list_length = std::max(int(LIST_LENGTH_FACTOR * std::sqrt(double(_search_arc_num))), MIN_LIST_LENGTH);
            _minor_limit = std::max(int(MINOR_LIMIT_FACTOR * _list_length), MIN_MINOR_LIMIT);
            _curr_length = _minor_count = 0;
            _candidates.resize(_list_length);
        }

        /// Find next entering arc
        bool findEnteringArc() {
            Cost min, c;
            int e;
            if (_curr_length > 0 && _minor_count < _minor_limit) {
                // Minor iteration: select the best eligible arc from the
                // current candidate list
                ++_minor_count;
                min = 0;
                for (int i = 0; i < _curr_length; ++i) {
                    e = _candidates[i];
                    c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                    if (c < min) {
                        min = c;
                        _in_arc = e;
                    } else if (c >= 0) {
                        _candidates[i--] = _candidates[--_curr_length];
                    }
                }
                if (min < 0) return true;
            }

            // Major iteration: build a new candidate list
            min = 0;
            _curr_length = 0;
            for (e = _next_arc; e != _search_arc_num; ++e) {
                c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                if (c < 0) {
                    _candidates[_curr_length++] = e;
                    if (c < min) {
                        min = c;
                        _in_arc = e;
                    }
                    if (_curr_length == _list_length) goto search_end;
                }
            }
            for (e = 0; e != _next_arc; ++e) {
                c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                if (c < 0) {
                    _candidates[_curr_length++] = e;
                    if (c < min) {
                        min = c;
                        _in_arc = e;
                    }
                    if (_curr_length == _list_length) goto search_end;
                }
            }
            if (_curr_length == 0) return false;

        search_end:
            _minor_count = 1;
            _next_arc = e;
            return true;
        }

    }; // class CandidateListPivotRule

    // Implementation of the Altering Candidate List pivot rule
    class AlteringListPivotRule {
    private:
        // References to the NetworkSimplex class
        const IntVector &_source;
        const IntVector &_target;
        const CostVector &_cost;
        const CharVector &_state;
        const CostVector &_pi;
        int &_in_arc;
        int _search_arc_num;

        // Pivot rule data
        int _block_size, _head_length, _curr_length;
        int _next_arc;
        IntVector _candidates;
        CostVector _cand_cost;

        // Functor class to compare arcs during sort of the candidate list
        class SortFunc {
        private:
            const CostVector &_map;

        public:
            SortFunc(const CostVector &map) : _map(map) {}
            bool operator()(int left, int right) { return _map[left] < _map[right]; }
        };

        SortFunc _sort_func;

    public:
        // Constructor
        AlteringListPivotRule(NetworkSimplex &ns)
            : _source(ns._source), _target(ns._target), _cost(ns._cost), _state(ns._state), _pi(ns._pi), _in_arc(ns.in_arc), _search_arc_num(ns._search_arc_num), _next_arc(0), _cand_cost(ns._search_arc_num), _sort_func(_cand_cost) {
            // The main parameters of the pivot rule
            const double BLOCK_SIZE_FACTOR = 1.0;
            const int MIN_BLOCK_SIZE = 10;
            const double HEAD_LENGTH_FACTOR = 0.01;
            const int MIN_HEAD_LENGTH = 3;

            _block_size = std::max(int(BLOCK_SIZE_FACTOR * std::sqrt(double(_search_arc_num))), MIN_BLOCK_SIZE);
            _head_length = std::max(int(HEAD_LENGTH_FACTOR * _block_size), MIN_HEAD_LENGTH);
            _candidates.resize(_head_length + _block_size);
            _curr_length = 0;
        }

        // Find next entering arc
        bool findEnteringArc() {
            // Check the current candidate list
            int e;
            Cost c;
            for (int i = 0; i != _curr_length; ++i) {
                e = _candidates[i];
                c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                if (c < 0) {
                    _cand_cost[e] = c;
                } else {
                    _candidates[i--] = _candidates[--_curr_length];
                }
            }

            // Extend the list
            int cnt = _block_size;
            int limit = _head_length;

            for (e = _next_arc; e != _search_arc_num; ++e) {
                c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                if (c < 0) {
                    _cand_cost[e] = c;
                    _candidates[_curr_length++] = e;
                }
                if (--cnt == 0) {
                    if (_curr_length > limit) goto search_end;
                    limit = 0;
                    cnt = _block_size;
                }
            }
            for (e = 0; e != _next_arc; ++e) {
                c = _state[e] * (_cost[e] + _pi[_source[e]] - _pi[_target[e]]);
                if (c < 0) {
                    _cand_cost[e] = c;
                    _candidates[_curr_length++] = e;
                }
                if (--cnt == 0) {
                    if (_curr_length > limit) goto search_end;
                    limit = 0;
                    cnt = _block_size;
                }
            }
            if (_curr_length == 0) return false;

        search_end:

            // Perform partial sort operation on the candidate list
            int new_length = std::min(_head_length + 1, _curr_length);
            std::partial_sort(_candidates.begin(), _candidates.begin() + new_length, _candidates.begin() + _curr_length, _sort_func);

            // Select the entering arc and remove it from the list
            _in_arc = _candidates[0];
            _next_arc = e;
            _candidates[0] = _candidates[new_length - 1];
            _curr_length = new_length - 1;
            return true;
        }

    }; // class AlteringListPivotRule

public:
    /// \brief Constructor.
    ///
    /// The constructor of the class.
    ///
    /// \param graph The digraph the algorithm runs on.
    /// \param arc_mixing Indicate if the arcs will be stored in a
    /// mixed order in the internal data structure.
    /// In general, it leads to similar performance as using the original
    /// arc order, but it makes the algorithm more robust and in special
    /// cases, even significantly faster. Therefore, it is enabled by default.
    NetworkSimplex(const Digraph &graph, bool arc_mixing = true)
        : _graph(graph), _arc_mixing(arc_mixing), MAX(std::numeric_limits<Value>::max()), INF(std::numeric_limits<Value>::has_infinity ? std::numeric_limits<Value>::infinity() : MAX) {
        // Check the number types
        assert(std::numeric_limits<Value>::is_signed);
        assert(std::numeric_limits<Cost>::is_signed);

        // Reset data structures
        reset();
    }

    /// \name Parameters
    /// The parameters of the algorithm can be specified using these
    /// functions.

    /// @{

    /// \brief Set the lower bounds on the arcs.
    ///
    /// This function sets the lower bounds on the arcs.
    /// If it is not used before calling \ref run(), the lower bounds
    /// will be set to zero on all arcs.
    ///
    /// \param map An arc map storing the lower bounds.
    /// Its \c Value type must be convertible to the \c Value type
    /// of the algorithm.
    ///
    /// \return <tt>(*this)</tt>
    template <typename LowerMap> NetworkSimplex &lowerMap(const LowerMap &map) {
        _has_lower = true;
        for (int a = 0; a < _arc_num; a++) { _lower[a] = map[a]; }
        return *this;
    }

    /// \brief Set the upper bounds (capacities) on the arcs.
    ///
    /// This function sets the upper bounds (capacities) on the arcs.
    /// If it is not used before calling \ref run(), the upper bounds
    /// will be set to \ref INF on all arcs (i.e. the flow value will be
    /// unbounded from above).
    ///
    /// \param map An arc map storing the upper bounds.
    /// Its \c Value type must be convertible to the \c Value type
    /// of the algorithm.
    ///
    /// \return <tt>(*this)</tt>
    template <typename UpperMap> NetworkSimplex &upperMap(const UpperMap &map) {
        for (int a = 0; a < _arc_num; a++) { _upper[a] = map[a]; }
        return *this;
    }

    /// \brief Set the costs of the arcs.
    ///
    /// This function sets the costs of the arcs.
    /// If it is not used before calling \ref run(), the costs
    /// will be set to \c 1 on all arcs.
    ///
    /// \param map An arc map storing the costs.
    /// Its \c Value type must be convertible to the \c Cost type
    /// of the algorithm.
    ///
    /// \return <tt>(*this)</tt>
    template <typename CostMap> NetworkSimplex &costMap(const CostMap &map) {
        for (int a = 0; a < _arc_num; a++) { _cost[a] = map[a]; }
        return *this;
    }

    /// \brief Set the supply values of the nodes.
    ///
    /// This function sets the supply values of the nodes.
    /// If neither this function nor \ref stSupply() is used before
    /// calling \ref run(), the supply of each node will be set to zero.
    ///
    /// \param map A node map storing the supply values.
    /// Its \c Value type must be convertible to the \c Value type
    /// of the algorithm.
    ///
    /// \return <tt>(*this)</tt>
    ///
    /// \sa supplyType()
    template <typename SupplyMap> NetworkSimplex &supplyMap(const SupplyMap &map) {
        for (int n = 0; n < _node_num; n++) { _supply[n] = map[n]; }
        return *this;
    }

    /// \brief Set single source and target nodes and a supply value.
    ///
    /// This function sets a single source node and a single target node
    /// and the required flow value.
    /// If neither this function nor \ref supplyMap() is used before
    /// calling \ref run(), the supply of each node will be set to zero.
    ///
    /// Using this function has the same effect as using \ref supplyMap()
    /// with a map in which \c k is assigned to \c s, \c -k is
    /// assigned to \c t and all other nodes have zero supply value.
    ///
    /// \param s The source node.
    /// \param t The target node.
    /// \param k The required amount of flow from node \c s to node \c t
    /// (i.e. the supply of \c s and the demand of \c t).
    ///
    /// \return <tt>(*this)</tt>
    NetworkSimplex &stSupply(const Node &s, const Node &t, Value k) {
        for (int i = 0; i != _node_num; ++i) { _supply[i] = 0; }
        _supply[s] = k;
        _supply[t] = -k;
        return *this;
    }

    /// \brief Set the type of the supply constraints.
    ///
    /// This function sets the type of the supply/demand constraints.
    /// If it is not used before calling \ref run(), the \ref GEQ supply
    /// type will be used.
    ///
    /// For more information, see \ref SupplyType.
    ///
    /// \return <tt>(*this)</tt>
    NetworkSimplex &supplyType(SupplyType supply_type) {
        _stype = supply_type;
        return *this;
    }

    /// @}

    /// \name Execution Control
    /// The algorithm can be executed using \ref run().

    /// @{

    /// \brief Run the algorithm.
    ///
    /// This function runs the algorithm.
    /// The paramters can be specified using functions \ref lowerMap(),
    /// \ref upperMap(), \ref costMap(), \ref supplyMap(), \ref stSupply(),
    /// \ref supplyType().
    /// For example,
    /// \code
    ///   NetworkSimplex<ListDigraph> ns(graph);
    ///   ns.lowerMap(lower).upperMap(upper).costMap(cost)
    ///     .supplyMap(sup).run();
    /// \endcode
    ///
    /// This function can be called more than once. All the given parameters
    /// are kept for the next call, unless \ref resetParams() or \ref reset()
    /// is used, thus only the modified parameters have to be set again.
    /// If the underlying digraph was also modified after the construction
    /// of the class (or the last \ref reset() call), then the \ref reset()
    /// function must be called.
    ///
    /// \param pivot_rule The pivot rule that will be used during the
    /// algorithm. For more information, see \ref PivotRule.
    ///
    /// \return \c INFEASIBLE if no feasible flow exists,
    /// \n \c OPTIMAL if the problem has optimal solution
    /// (i.e. it is feasible and bounded), and the algorithm has found
    /// optimal flow and node potentials (primal and dual solutions),
    /// \n \c UNBOUNDED if the objective function of the problem is
    /// unbounded, i.e. there is a directed cycle having negative total
    /// cost and infinite upper bound.
    ///
    /// \see ProblemType, PivotRule
    /// \see resetParams(), reset()
    ProblemType run(PivotRule pivot_rule = BLOCK_SEARCH) {
        if (!init()) return INFEASIBLE;
        return start(pivot_rule);
    }

    /// \brief Reset all the parameters that have been given before.
    ///
    /// This function resets all the paramaters that have been given
    /// before using functions \ref lowerMap(), \ref upperMap(),
    /// \ref costMap(), \ref supplyMap(), \ref stSupply(), \ref supplyType().
    ///
    /// It is useful for multiple \ref run() calls. Basically, all the given
    /// parameters are kept for the next \ref run() call, unless
    /// \ref resetParams() or \ref reset() is used.
    /// If the underlying digraph was also modified after the construction
    /// of the class or the last \ref reset() call, then the \ref reset()
    /// function must be used, otherwise \ref resetParams() is sufficient.
    ///
    /// For example,
    /// \code
    ///   NetworkSimplex<ListDigraph> ns(graph);
    ///
    ///   // First run
    ///   ns.lowerMap(lower).upperMap(upper).costMap(cost)
    ///     .supplyMap(sup).run();
    ///
    ///   // Run again with modified cost map (resetParams() is not called,
    ///   // so only the cost map have to be set again)
    ///   cost[e] += 100;
    ///   ns.costMap(cost).run();
    ///
    ///   // Run again from scratch using resetParams()
    ///   // (the lower bounds will be set to zero on all arcs)
    ///   ns.resetParams();
    ///   ns.upperMap(capacity).costMap(cost)
    ///     .supplyMap(sup).run();
    /// \endcode
    ///
    /// \return <tt>(*this)</tt>
    ///
    /// \see reset(), run()
    NetworkSimplex &resetParams() {
        for (int i = 0; i != _node_num; ++i) { _supply[i] = 0; }
        for (int i = 0; i != _arc_num; ++i) {
            _lower[i] = 0;
            _upper[i] = INF;
            _cost[i] = 1;
        }
        _has_lower = false;
        _stype = GEQ;
        return *this;
    }

    /// \brief Reset the internal data structures and all the parameters
    /// that have been given before.
    ///
    /// This function resets the internal data structures and all the
    /// paramaters that have been given before using functions \ref lowerMap(),
    /// \ref upperMap(), \ref costMap(), \ref supplyMap(), \ref stSupply(),
    /// \ref supplyType().
    ///
    /// It is useful for multiple \ref run() calls. Basically, all the given
    /// parameters are kept for the next \ref run() call, unless
    /// \ref resetParams() or \ref reset() is used.
    /// If the underlying digraph was also modified after the construction
    /// of the class or the last \ref reset() call, then the \ref reset()
    /// function must be used, otherwise \ref resetParams() is sufficient.
    ///
    /// See \ref resetParams() for examples.
    ///
    /// \return <tt>(*this)</tt>
    ///
    /// \see resetParams(), run()
    NetworkSimplex &reset() {
        // Resize vectors
        _node_num = _graph.countNodes();
        _arc_num = _graph.countArcs();
        int all_node_num = _node_num + 1;
        int max_arc_num = _arc_num + 2 * _node_num;

        _source.resize(max_arc_num);
        _target.resize(max_arc_num);

        _lower.resize(_arc_num);
        _upper.resize(_arc_num);
        _cap.resize(max_arc_num);
        _cost.resize(max_arc_num);
        _supply.resize(all_node_num);
        _flow.resize(max_arc_num);
        _pi.resize(all_node_num);

        _parent.resize(all_node_num);
        _pred.resize(all_node_num);
        _pred_dir.resize(all_node_num);
        _thread.resize(all_node_num);
        _rev_thread.resize(all_node_num);
        _succ_num.resize(all_node_num);
        _last_succ.resize(all_node_num);
        _state.resize(max_arc_num);

        // Copy the graph
        if (_arc_mixing && _node_num > 1 and false) {
            // Store the arcs in a mixed order
            const int skip = std::max(_arc_num / _node_num, 3);
            int i = 0, j = 0;
            for (int a = 0; a < _arc_num; a++) {
                _source[i] = _graph.source(a);
                _target[i] = _graph.target(a);
                if ((i += skip) >= _arc_num) i = ++j;
            }
        } else {
            // Store the arcs in the original order
            int i = 0;
            for (int a = 0; a < _arc_num; ++a, ++i) {
                _source[i] = _graph.source(a);
                _target[i] = _graph.target(a);
            }
        }

        // Reset parameters
        resetParams();
        return *this;
    }

    /// @}

    template <typename Number = Cost> Number totalCost() const {
        Number c = 0;
        for (Arc a = 0; a < _arc_num; a++) c += Number(_flow[a]) * Number(_cost[a]);
        return c;
    }

    Value flow(const Arc &a) const { return _flow[a]; }

    /// \brief Copy the flow values (the primal solution) into the
    /// given map.
    ///
    /// This function copies the flow value on each arc into the given
    /// map. The \c Value type of the algorithm must be convertible to
    /// the \c Value type of the map.
    ///
    /// \pre \ref run() must be called before using this function.
    template <typename FlowMap> void flowMap(FlowMap &map) const {
        for (int a = 0; a < _arc_num; a++) { map.set(a, _flow[a]); }
    }
    ValueVector flowMap() const { return _flow; }

    /// \brief Return the potential (dual value) of the given node.
    ///
    /// This function returns the potential (dual value) of the
    /// given node.
    ///
    /// \pre \ref run() must be called before using this function.
    Cost potential(const Node &n) const { return _pi[n]; }

    /// \brief Copy the potential values (the dual solution) into the
    /// given map.
    ///
    /// This function copies the potential (dual value) of each node
    /// into the given map.
    /// The \c Cost type of the algorithm must be convertible to the
    /// \c Value type of the map.
    ///
    /// \pre \ref run() must be called before using this function.
    template <typename PotentialMap> void potentialMap(PotentialMap &map) const {
        for (int n = 0; n < _graph.V; n++) { map.set(n, _pi[n]); }
    }
    CostVector potentialMap() const { return _pi; }

    /// @}

private:
    // Initialize internal data structures
    bool init() {
        if (_node_num == 0) return false;

        // Check the sum of supply values
        _sum_supply = 0;
        for (int i = 0; i != _node_num; ++i) { _sum_supply += _supply[i]; }
        if (!((_stype == GEQ && _sum_supply <= 0) || (_stype == LEQ && _sum_supply >= 0))) return false;

        // Check lower and upper bounds
        // LEMON_DEBUG(checkBoundMaps(), "Upper bounds must be greater or equal to the lower bounds");

        // Remove non-zero lower bounds
        if (_has_lower) {
            for (int i = 0; i != _arc_num; ++i) {
                Value c = _lower[i];
                if (c >= 0) {
                    _cap[i] = _upper[i] < MAX ? _upper[i] - c : INF;
                } else {
                    _cap[i] = _upper[i] < MAX + c ? _upper[i] - c : INF;
                }
                _supply[_source[i]] -= c;
                _supply[_target[i]] += c;
            }
        } else {
            for (int i = 0; i != _arc_num; ++i) { _cap[i] = _upper[i]; }
        }

        // Initialize artifical cost
        Cost ART_COST;
        if (std::numeric_limits<Cost>::is_exact) {
            ART_COST = std::numeric_limits<Cost>::max() / 2 + 1;
        } else {
            ART_COST = 0;
            for (int i = 0; i != _arc_num; ++i) {
                if (_cost[i] > ART_COST) ART_COST = _cost[i];
            }
            ART_COST = (ART_COST + 1) * _node_num;
        }

        // Initialize arc maps
        for (int i = 0; i != _arc_num; ++i) {
            _flow[i] = 0;
            _state[i] = STATE_LOWER;
        }

        // Set data for the artificial root node
        _root = _node_num;
        _parent[_root] = -1;
        _pred[_root] = -1;
        _thread[_root] = 0;
        _rev_thread[0] = _root;
        _succ_num[_root] = _node_num + 1;
        _last_succ[_root] = _root - 1;
        _supply[_root] = -_sum_supply;
        _pi[_root] = 0;

        // Add artificial arcs and initialize the spanning tree data structure
        if (_sum_supply == 0) {
            // EQ supply constraints
            _search_arc_num = _arc_num;
            _all_arc_num = _arc_num + _node_num;
            for (int u = 0, e = _arc_num; u != _node_num; ++u, ++e) {
                _parent[u] = _root;
                _pred[u] = e;
                _thread[u] = u + 1;
                _rev_thread[u + 1] = u;
                _succ_num[u] = 1;
                _last_succ[u] = u;
                _cap[e] = INF;
                _state[e] = STATE_TREE;
                if (_supply[u] >= 0) {
                    _pred_dir[u] = DIR_UP;
                    _pi[u] = 0;
                    _source[e] = u;
                    _target[e] = _root;
                    _flow[e] = _supply[u];
                    _cost[e] = 0;
                } else {
                    _pred_dir[u] = DIR_DOWN;
                    _pi[u] = ART_COST;
                    _source[e] = _root;
                    _target[e] = u;
                    _flow[e] = -_supply[u];
                    _cost[e] = ART_COST;
                }
            }
        } else if (_sum_supply > 0) {
            // LEQ supply constraints
            _search_arc_num = _arc_num + _node_num;
            int f = _arc_num + _node_num;
            for (int u = 0, e = _arc_num; u != _node_num; ++u, ++e) {
                _parent[u] = _root;
                _thread[u] = u + 1;
                _rev_thread[u + 1] = u;
                _succ_num[u] = 1;
                _last_succ[u] = u;
                if (_supply[u] >= 0) {
                    _pred_dir[u] = DIR_UP;
                    _pi[u] = 0;
                    _pred[u] = e;
                    _source[e] = u;
                    _target[e] = _root;
                    _cap[e] = INF;
                    _flow[e] = _supply[u];
                    _cost[e] = 0;
                    _state[e] = STATE_TREE;
                } else {
                    _pred_dir[u] = DIR_DOWN;
                    _pi[u] = ART_COST;
                    _pred[u] = f;
                    _source[f] = _root;
                    _target[f] = u;
                    _cap[f] = INF;
                    _flow[f] = -_supply[u];
                    _cost[f] = ART_COST;
                    _state[f] = STATE_TREE;
                    _source[e] = u;
                    _target[e] = _root;
                    _cap[e] = INF;
                    _flow[e] = 0;
                    _cost[e] = 0;
                    _state[e] = STATE_LOWER;
                    ++f;
                }
            }
            _all_arc_num = f;
        } else {
            // GEQ supply constraints
            _search_arc_num = _arc_num + _node_num;
            int f = _arc_num + _node_num;
            for (int u = 0, e = _arc_num; u != _node_num; ++u, ++e) {
                _parent[u] = _root;
                _thread[u] = u + 1;
                _rev_thread[u + 1] = u;
                _succ_num[u] = 1;
                _last_succ[u] = u;
                if (_supply[u] <= 0) {
                    _pred_dir[u] = DIR_DOWN;
                    _pi[u] = 0;
                    _pred[u] = e;
                    _source[e] = _root;
                    _target[e] = u;
                    _cap[e] = INF;
                    _flow[e] = -_supply[u];
                    _cost[e] = 0;
                    _state[e] = STATE_TREE;
                } else {
                    _pred_dir[u] = DIR_UP;
                    _pi[u] = -ART_COST;
                    _pred[u] = f;
                    _source[f] = u;
                    _target[f] = _root;
                    _cap[f] = INF;
                    _flow[f] = _supply[u];
                    _state[f] = STATE_TREE;
                    _cost[f] = ART_COST;
                    _source[e] = _root;
                    _target[e] = u;
                    _cap[e] = INF;
                    _flow[e] = 0;
                    _cost[e] = 0;
                    _state[e] = STATE_LOWER;
                    ++f;
                }
            }
            _all_arc_num = f;
        }

        return true;
    }

    // Check if the upper bound is greater than or equal to the lower bound
    // on each arc.
    bool checkBoundMaps() {
        for (int j = 0; j != _arc_num; ++j) {
            if (_upper[j] < _lower[j]) return false;
        }
        return true;
    }

    // Find the join node
    void findJoinNode() {
        int u = _source[in_arc];
        int v = _target[in_arc];
        while (u != v) {
            if (_succ_num[u] < _succ_num[v]) {
                u = _parent[u];
            } else {
                v = _parent[v];
            }
        }
        join = u;
    }

    // Find the leaving arc of the cycle and returns true if the
    // leaving arc is not the same as the entering arc
    bool findLeavingArc() {
        // Initialize first and second nodes according to the direction
        // of the cycle
        int first, second;
        if (_state[in_arc] == STATE_LOWER) {
            first = _source[in_arc];
            second = _target[in_arc];
        } else {
            first = _target[in_arc];
            second = _source[in_arc];
        }
        delta = _cap[in_arc];
        int result = 0;
        Value c, d;
        int e;

        // Search the cycle form the first node to the join node
        for (int u = first; u != join; u = _parent[u]) {
            e = _pred[u];
            d = _flow[e];
            if (_pred_dir[u] == DIR_DOWN) {
                c = _cap[e];
                d = c >= MAX ? INF : c - d;
            }
            if (d < delta) {
                delta = d;
                u_out = u;
                result = 1;
            }
        }

        // Search the cycle form the second node to the join node
        for (int u = second; u != join; u = _parent[u]) {
            e = _pred[u];
            d = _flow[e];
            if (_pred_dir[u] == DIR_UP) {
                c = _cap[e];
                d = c >= MAX ? INF : c - d;
            }
            if (d <= delta) {
                delta = d;
                u_out = u;
                result = 2;
            }
        }

        if (result == 1) {
            u_in = first;
            v_in = second;
        } else {
            u_in = second;
            v_in = first;
        }
        return result != 0;
    }

    // Change _flow and _state vectors
    void changeFlow(bool change) {
        // Augment along the cycle
        if (delta > 0) {
            Value val = _state[in_arc] * delta;
            _flow[in_arc] += val;
            for (int u = _source[in_arc]; u != join; u = _parent[u]) { _flow[_pred[u]] -= _pred_dir[u] * val; }
            for (int u = _target[in_arc]; u != join; u = _parent[u]) { _flow[_pred[u]] += _pred_dir[u] * val; }
        }
        // Update the state of the entering and leaving arcs
        if (change) {
            _state[in_arc] = STATE_TREE;
            _state[_pred[u_out]] = (_flow[_pred[u_out]] == 0) ? STATE_LOWER : STATE_UPPER;
        } else {
            _state[in_arc] = -_state[in_arc];
        }
    }

    // Update the tree structure
    void updateTreeStructure() {
        int old_rev_thread = _rev_thread[u_out];
        int old_succ_num = _succ_num[u_out];
        int old_last_succ = _last_succ[u_out];
        v_out = _parent[u_out];

        // Check if u_in and u_out coincide
        if (u_in == u_out) {
            // Update _parent, _pred, _pred_dir
            _parent[u_in] = v_in;
            _pred[u_in] = in_arc;
            _pred_dir[u_in] = u_in == _source[in_arc] ? DIR_UP : DIR_DOWN;

            // Update _thread and _rev_thread
            if (_thread[v_in] != u_out) {
                int after = _thread[old_last_succ];
                _thread[old_rev_thread] = after;
                _rev_thread[after] = old_rev_thread;
                after = _thread[v_in];
                _thread[v_in] = u_out;
                _rev_thread[u_out] = v_in;
                _thread[old_last_succ] = after;
                _rev_thread[after] = old_last_succ;
            }
        } else {
            // Handle the case when old_rev_thread equals to v_in
            // (it also means that join and v_out coincide)
            int thread_continue = old_rev_thread == v_in ? _thread[old_last_succ] : _thread[v_in];

            // Update _thread and _parent along the stem nodes (i.e. the nodes
            // between u_in and u_out, whose parent have to be changed)
            int stem = u_in;             // the current stem node
            int par_stem = v_in;         // the new parent of stem
            int next_stem;               // the next stem node
            int last = _last_succ[u_in]; // the last successor of stem
            int before, after = _thread[last];
            _thread[v_in] = u_in;
            _dirty_revs.clear();
            _dirty_revs.push_back(v_in);
            while (stem != u_out) {
                // Insert the next stem node into the thread list
                next_stem = _parent[stem];
                _thread[last] = next_stem;
                _dirty_revs.push_back(last);

                // Remove the subtree of stem from the thread list
                before = _rev_thread[stem];
                _thread[before] = after;
                _rev_thread[after] = before;

                // Change the parent node and shift stem nodes
                _parent[stem] = par_stem;
                par_stem = stem;
                stem = next_stem;

                // Update last and after
                last = _last_succ[stem] == _last_succ[par_stem] ? _rev_thread[par_stem] : _last_succ[stem];
                after = _thread[last];
            }
            _parent[u_out] = par_stem;
            _thread[last] = thread_continue;
            _rev_thread[thread_continue] = last;
            _last_succ[u_out] = last;

            // Remove the subtree of u_out from the thread list except for
            // the case when old_rev_thread equals to v_in
            if (old_rev_thread != v_in) {
                _thread[old_rev_thread] = after;
                _rev_thread[after] = old_rev_thread;
            }

            // Update _rev_thread using the new _thread values
            for (int i = 0; i != int(_dirty_revs.size()); ++i) {
                int u = _dirty_revs[i];
                _rev_thread[_thread[u]] = u;
            }

            // Update _pred, _pred_dir, _last_succ and _succ_num for the
            // stem nodes from u_out to u_in
            int tmp_sc = 0, tmp_ls = _last_succ[u_out];
            for (int u = u_out, p = _parent[u]; u != u_in; u = p, p = _parent[u]) {
                _pred[u] = _pred[p];
                _pred_dir[u] = -_pred_dir[p];
                tmp_sc += _succ_num[u] - _succ_num[p];
                _succ_num[u] = tmp_sc;
                _last_succ[p] = tmp_ls;
            }
            _pred[u_in] = in_arc;
            _pred_dir[u_in] = u_in == _source[in_arc] ? DIR_UP : DIR_DOWN;
            _succ_num[u_in] = old_succ_num;
        }

        // Update _last_succ from v_in towards the root
        int up_limit_out = _last_succ[join] == v_in ? join : -1;
        int last_succ_out = _last_succ[u_out];
        for (int u = v_in; u != -1 && _last_succ[u] == v_in; u = _parent[u]) { _last_succ[u] = last_succ_out; }

        // Update _last_succ from v_out towards the root
        if (join != old_rev_thread && v_in != old_rev_thread) {
            for (int u = v_out; u != up_limit_out && _last_succ[u] == old_last_succ; u = _parent[u]) {
                _last_succ[u] = old_rev_thread;
            }
        } else if (last_succ_out != old_last_succ) {
            for (int u = v_out; u != up_limit_out && _last_succ[u] == old_last_succ; u = _parent[u]) {
                _last_succ[u] = last_succ_out;
            }
        }

        // Update _succ_num from v_in to join
        for (int u = v_in; u != join; u = _parent[u]) { _succ_num[u] += old_succ_num; }
        // Update _succ_num from v_out to join
        for (int u = v_out; u != join; u = _parent[u]) { _succ_num[u] -= old_succ_num; }
    }

    // Update potentials in the subtree that has been moved
    void updatePotential() {
        Cost sigma = _pi[v_in] - _pi[u_in] - _pred_dir[u_in] * _cost[in_arc];
        int end = _thread[_last_succ[u_in]];
        for (int u = u_in; u != end; u = _thread[u]) { _pi[u] += sigma; }
    }

    // Heuristic initial pivots
    bool initialPivots() {
        Value curr, total = 0;
        std::vector<Node> supply_nodes, demand_nodes;
        for (int u = 0; u < _node_num; ++u) {
            curr = _supply[u];
            if (curr > 0) {
                total += curr;
                supply_nodes.push_back(u);
            } else if (curr < 0) {
                demand_nodes.push_back(u);
            }
        }
        if (_sum_supply > 0) total -= _sum_supply;
        if (total <= 0) return true;

        IntVector arc_vector;
        if (_sum_supply >= 0) {
            if (supply_nodes.size() == 1 && demand_nodes.size() == 1) {
                // Perform a reverse graph search from the sink to the source
                std::vector<char> reached(_node_num, false);
                Node s = supply_nodes[0], t = demand_nodes[0];
                std::vector<Node> stack;
                reached[t] = true;
                stack.push_back(t);
                while (!stack.empty()) {
                    Node u, v = stack.back();
                    stack.pop_back();
                    if (v == s) break;
                    // for (InArcIt a(_graph, v); a != INVALID; ++a) {
                    for (auto a : _graph.in_eids[v]) {
                        if (reached[u = _graph.source(a)]) continue;
                        int j = a;
                        if (_cap[j] >= total) {
                            arc_vector.push_back(j);
                            reached[u] = true;
                            stack.push_back(u);
                        }
                    }
                }
            } else {
                // Find the min. cost incoming arc for each demand node
                for (int i = 0; i != int(demand_nodes.size()); ++i) {
                    Node v = demand_nodes[i];
                    Cost c, min_cost = std::numeric_limits<Cost>::max();
                    Arc min_arc = INVALID;
                    for (auto a : _graph.in_eids[v]) {
                        // for (InArcIt a(_graph, v); a != INVALID; ++a) {
                        c = _cost[a];
                        if (c < min_cost) {
                            min_cost = c;
                            min_arc = a;
                        }
                    }
                    if (min_arc != INVALID) { arc_vector.push_back(min_arc); }
                }
            }
        } else {
            // Find the min. cost outgoing arc for each supply node
            for (Node u : supply_nodes) {
                Cost c, min_cost = std::numeric_limits<Cost>::max();
                Arc min_arc = INVALID;
                for (auto a : _graph.out_eids[u]) {
                    c = _cost[a];
                    if (c < min_cost) {
                        min_cost = c;
                        min_arc = a;
                    }
                }
                if (min_arc != INVALID) { arc_vector.push_back(min_arc); }
            }
        }

        // Perform heuristic initial pivots
        for (int i = 0; i != int(arc_vector.size()); ++i) {
            in_arc = arc_vector[i];
            if (_state[in_arc] * (_cost[in_arc] + _pi[_source[in_arc]] - _pi[_target[in_arc]]) >= 0) continue;
            findJoinNode();
            bool change = findLeavingArc();
            if (delta >= MAX) return false;
            changeFlow(change);
            if (change) {
                updateTreeStructure();
                updatePotential();
            }
        }
        return true;
    }

    // Execute the algorithm
    ProblemType start(PivotRule pivot_rule) {
        // Select the pivot rule implementation
        switch (pivot_rule) {
        case FIRST_ELIGIBLE: return start<FirstEligiblePivotRule>();
        case BEST_ELIGIBLE: return start<BestEligiblePivotRule>();
        case BLOCK_SEARCH: return start<BlockSearchPivotRule>();
        case CANDIDATE_LIST: return start<CandidateListPivotRule>();
        case ALTERING_LIST: return start<AlteringListPivotRule>();
        }
        return INFEASIBLE; // avoid warning
    }

    template <typename PivotRuleImpl> ProblemType start() {
        PivotRuleImpl pivot(*this);

        // Perform heuristic initial pivots
        if (!initialPivots()) return UNBOUNDED;

        // Execute the Network Simplex algorithm
        while (pivot.findEnteringArc()) {
            findJoinNode();
            bool change = findLeavingArc();
            if (delta >= MAX) return UNBOUNDED;
            changeFlow(change);
            if (change) {
                updateTreeStructure();
                updatePotential();
            }
        }

        // Check feasibility
        for (int e = _search_arc_num; e != _all_arc_num; ++e) {
            if (_flow[e] != 0) return INFEASIBLE;
        }

        // Transform the solution and the supply map to the original form
        if (_has_lower) {
            for (int i = 0; i != _arc_num; ++i) {
                Value c = _lower[i];
                if (c != 0) {
                    _flow[i] += c;
                    _supply[_source[i]] += c;
                    _supply[_target[i]] -= c;
                }
            }
        }

        // Shift potentials to meet the requirements of the GEQ/LEQ type
        // optimality conditions
        if (_sum_supply == 0) {
            if (_stype == GEQ) {
                Cost max_pot = -std::numeric_limits<Cost>::max();
                for (int i = 0; i != _node_num; ++i) {
                    if (_pi[i] > max_pot) max_pot = _pi[i];
                }
                if (max_pot > 0) {
                    for (int i = 0; i != _node_num; ++i) _pi[i] -= max_pot;
                }
            } else {
                Cost min_pot = std::numeric_limits<Cost>::max();
                for (int i = 0; i != _node_num; ++i) {
                    if (_pi[i] < min_pot) min_pot = _pi[i];
                }
                if (min_pot < 0) {
                    for (int i = 0; i != _node_num; ++i) _pi[i] -= min_pot;
                }
            }
        }

        return OPTIMAL;
    }

}; // class NetworkSimplex
#include <iostream>
#define PROBLEM "https://judge.yosupo.jp/problem/min_cost_b_flow"

std::ostream &operator<<(std::ostream &os, const __int128 &x) {
    __int128 tmp = x;
    if (tmp == 0) return os << 0;
    std::vector<int> ds;
    if (tmp < 0) {
        os << '-';
        while (tmp) {
            int d = tmp % 10;
            if (d > 0) d -= 10;
            ds.emplace_back(-d), tmp = (tmp - d) / 10;
        }
    } else {
        while (tmp) ds.emplace_back(tmp % 10), tmp /= 10;
    }
    std::reverse(ds.begin(), ds.end());
    for (auto i : ds) os << i;
    return os;
}

template <typename Capacity = long long, typename Weight = long long> struct lemon_mcf_graph {

    struct Digraph {
        const int V;
        int E;
        std::vector<std::vector<int>> in_eids, out_eids;
        std::vector<std::pair<int, int>> arcs;
        Digraph(int V = 0) : V(V), E(0), in_eids(V), out_eids(V){};
        int add_edge(int s, int t) {
            assert(0 <= s and s < V);
            assert(0 <= t and t < V);
            in_eids[t].push_back(E), out_eids[s].push_back(E), arcs.emplace_back(s, t), E++;
            return E - 1;
        }
        int countNodes() const { return V; }
        int countArcs() const { return E; }
        int source(int arcid) const { return arcs[arcid].first; }
        int target(int arcid) const { return arcs[arcid].second; } // eid 番目の辺の始点・終点
    };

    using NS = NetworkSimplex<Digraph, Capacity, Weight>;

    struct edge {
        int eid;
        int from, to;
        Capacity lo, hi;
        Weight weight;
    };

    int n;
    std::vector<edge> Edges;
    std::vector<Capacity> bs;
    lemon_mcf_graph(int V = 0) : n(V), bs(V), infeasible(false) {}
    void add_edge(int s, int t, Capacity lower, Capacity upper, Weight weight) {
        assert(s >= 0 and s < n);
        assert(t >= 0 and t < n);
        int idnow = Edges.size();
        Edges.push_back({idnow, s, t, lower, upper, weight});
    }
    void set_supply(int v, Capacity b) {
        assert(v >= 0 and v < n);
        bs[v] = b;
    }
    std::vector<Capacity> flow;
    std::vector<Capacity> potential;

    template <typename RetVal = __int128> RetVal solve() {
        flow.clear();
        potential.clear();
        Digraph graph(n + 1);

        std::vector<Capacity> supplies(graph.countNodes());
        std::vector<Capacity> lowers(Edges.size());
        std::vector<Capacity> uppers(Edges.size());
        std::vector<Weight> weights(Edges.size());

        for (int i = 0; i < n; i++) supplies[i] = bs[i];
        for (auto e : Edges) {
            int arc = graph.add_edge(e.from, e.to);
            lowers[arc] = e.lo;
            uppers[arc] = e.hi;
            weights[arc] = e.weight;
        }

        NS network_simplex(graph);
        network_simplex.supplyMap(supplies).costMap(weights).lowerMap(lowers).upperMap(uppers);
        auto status = network_simplex.run();

        if (status == NS::INFEASIBLE) {
            infeasible = true;
            return 0;
        } else {
            flow = network_simplex.flowMap();
            flow.resize(Edges.size());
            potential = network_simplex.potentialMap();
            potential.resize(n);
            RetVal ret = network_simplex.template totalCost<RetVal>();
            return ret;
        }
    }
    bool infeasible;
};
#line 10 "A/main.cpp"

using namespace yosupo;

inline static StopWatch global_sw;

#line 2 "A/template.hpp"

#ifdef YOSUPO_AVX2_PRAGMA
#line 5 "A/template.hpp"
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#endif

#line 11 "A/template.hpp"
#include <bitset>
#line 14 "A/template.hpp"
#include <cstdio>
#line 17 "A/template.hpp"
#include <map>
#line 19 "A/template.hpp"
#include <ranges>
#include <set>
#line 24 "A/template.hpp"

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

inline ostream& operator<<(ostream& os, __int128_t x) {
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
    ranges::reverse(s);
    return os << s;
}
inline ostream& operator<<(ostream& os, __uint128_t x) {
    if (x == 0) {
        return os << "0";
    }
    string s;
    while (x) {
        s += char(x % 10 + '0');
        x /= 10;
    }
    ranges::reverse(s);
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
#line 16 "A/main.cpp"

Scanner sc = Scanner(stdin);
Printer pr = Printer(stdout);

const int N = 20;

struct P {
    int r, c;

    P operator+(const P& rhs) const { return P(r + rhs.r, c + rhs.c); }
    P operator+=(const P& rhs) { return *this = *this + rhs; }

    bool operator==(const P& rhs) const { return r == rhs.r && c == rhs.c; }
    bool operator<(const P& rhs) const { return std::tie(r, c) < std::tie(rhs.r, rhs.c); }

    int near4(const P& rhs) const {
        return abs(r - rhs.r) + abs(c - rhs.c) == 1;
    }
};

const array<P, 4> D4 = {
    P(0, 1),
    P(1, 0),
    P(0, -1),
    P(-1, 0),
};

using B = array<array<int, N>, N>;

const B BOARD = []() {
    int n;
    sc.read(n);
    assert(n == N);
    B b;
    for (int i : iota(0, N)) {
        for (int j : iota(0, N)) {
            sc.read(b[i][j]);
        }
    }
    return b;
}();
const int BASE = []() {
    int sum = 0;
    for (int r : iota(0, N)) {
        for (int c : iota(0, N)) {
            sum += max(0, BOARD[r][c]);
        }
    }
    return sum;
}();

// move -> load
struct Action {
    P p;
    int d;
    Action(P _p, int _d) : p(_p), d(_d) {}
};

int calc_cost(const V<Action>& actions) {
    auto board = BOARD;
    P pos = P(0, 0);
    int now_d = 0, cost = 0;
    for (auto action : actions) {
        P npos = action.p;
        int d = action.d;

        // move
        if (pos != npos) {
            assert(abs(pos.r - npos.r) + abs(pos.c - npos.c) <= 1);
            cost += 100 + now_d;
            pos = npos;
        }

        // load
        now_d += d;
        assert(now_d >= 0);
        board[pos.r][pos.c] -= d;
        cost += abs(d);
    }

    for (int r : iota(0, N)) {
        for (int c : iota(0, N)) {
            if (board[r][c] != 0) return TEN(9);
        }
    }

    return cost;
}

void output(const V<Action>& actions) {
    P pos = P(0, 0);
    for (auto action : actions) {
        P npos = action.p;
        int d = action.d;

        // move
        if (pos != npos) {
            bool ok = false;
            for (int dir : iota(0, 4)) {
                if (pos + D4[dir] == npos) {
                    ok = true;
                    pr.writeln("RDLU"[dir]);
                    break;
                }
            }
            pos = npos;
            assert(ok);
        }

        // load
        if (d > 0) {
            pr.writeln("+", d);
        } else if (d < 0) {
            pr.writeln(d);
        }        
    }
}

// cost, actions
pair<int, V<Action>> calc_best_actions(const V<P>& ps) {
    assert(ps[0] == P(0, 0));
    int n = int(ps.size());
    for (int i : iota(1, n)) {
        assert(ps[i - 1].near4(ps[i]));
    }

    static map<V<P>, pair<int, V<Action>>> dp;
    if (dp.count(ps)) return dp[ps];

    lemon_mcf_graph<int, int> g(n + N * N);

    //int sv = n + N * N, tv = sv + 1;
    //atcoder::mcf_graph<int, int> g(n + N * N + 2);
    for (int r : iota(0, N)) {
        for (int c : iota(0, N)) {
            g.set_supply(n + r * N + c, BOARD[r][c]);
        }
    }

    //V<int> in_e(n), out_e(n);
    for (int i : iota(0, n)) {
        int id = n + ps[i].r * N + ps[i].c;
        g.add_edge(id, i, 0, TEN(9), 1);
        g.add_edge(i, id, 0, TEN(9), 1);
    }

    for (int i : iota(0, n - 1)) {
        g.add_edge(i, i + 1, 0, TEN(9), 1);
    }

    g.solve<int>();
    if (g.infeasible) return dp[ps] = {TEN(9), {}};

    //dbg(ps);
    V<Action> actions;
    int cost = 0;
    int now_d = 0;
    for (int i : iota(0, n)) {
        if (i && ps[i - 1] != ps[i]) {
            cost += 100 + now_d;
        }
        //int d = g.get_edge(in_e[i]).flow - g.get_edge(out_e[i]).flow;
        int d = g.flow[2 * i]- g.flow[2 * i + 1];
        cost += abs(d);
        now_d += d;
        actions.push_back({ps[i], d});
    }
    //dbg(cost, 2 * TEN(9) * BASE / cost);
    return dp[ps] = {cost, actions};
}

V<P> opt(int min_cost, V<P> min_ps, int time) {
    assert(min_cost == calc_best_actions(min_ps).first);

    StopWatch sw;
    int iter = 0;

    while (true) {
        int msecs = sw.msecs();
        if (msecs > time) break;

        auto check = [&](const V<P>& ps) {
            if (ps == min_ps) return false;

            // check
            iter++;
            auto [cost, actions] = calc_best_actions(ps);
            if (cost == TEN(9)) return false;
            assert(cost == calc_cost(actions));
            if (cost <= min_cost) {
                dbg(msecs, "/", time, " : ", min_cost, "->", cost);
                min_cost = cost;
                min_ps = ps;
                return true;
            }
            return false;
        };
        V<P> ps = min_ps;

        int ty = uniform(0, 99);
        if (ty <= 5) {
            for (int _ : iota(0, 5)) {
                ps.pop_back();
                if (!check(ps)) {
                    break;
                }
            }
        } else {
            int n = int(ps.size());
            [&]() {
                for (int _ : iota(0, n)) {
                    int i = uniform(0, n - 4);
                    for (int phase : iota(0, 5)) {
                        if (i - phase < 0 || int(ps.size()) <= i - phase + 3 || !ps[i - phase].near4(ps[i - phase + 3])) break;
                        ps.erase(ps.begin() + i - phase + 1, ps.begin() + i - phase + 3);
                        if (!check(ps)) {
                            return;
                        }
                    }
                }
            }();
        }
    }

    dbg(min_cost, iter, time, 1.0 * time / iter);

    return min_ps;
}

V<pair<int, V<P>>> gen() {
    VV<P> paths;
    {
        V<P> ps;

        for (int r : iota(0, N)) {
            if (r % 2 == 0) {
                for (int c : iota(0, N)) {
                    ps.push_back(P(r, c));
                }
            } else {
                for (int c : iota(0, N) | reverse) {
                    ps.push_back(P(r, c));
                }
            }
        }

        paths.push_back(ps);
    }

    if (true) {
        for (int phase : iota(1, N / 2)) {
            V<P> ps;

            for (int r : iota(0, 2 * phase)) {
                if (r % 2 == 0) {
                    for (int c : iota(0, N - 1)) {
                        ps.push_back(P(r, c));
                    }
                } else {
                    for (int c : iota(0, N - 1) | reverse) {
                        ps.push_back(P(r, c));
                    }
                }
            }

            for (int c : iota(0, N)) {
                if (c % 2 == 0) {
                    for (int r : iota(2 * phase, N)) {
                        ps.push_back(P(r, c));
                    }
                } else {
                    for (int r : iota(2 * phase, N) | reverse) {
                        ps.push_back(P(r, c));
                    }
                }
            }
            for (int r : iota(0, 2 * phase) | reverse) {
                ps.push_back(P(r, N - 1));
            }

            paths.push_back(ps);
        }
    }

    if (false) {
        V<P> path;
        for (int phase : iota(0, 2)) {
            V<P> path0;
            P now = (phase == 0 ? P(0, 0) : P(N - 1, 0));
            int dir = (phase == 0 ? 0 : 3);
            path0.push_back(now);
            for (int len : iota(1, N + 1) | reverse) {
                for (int _ : iota(0, min(N - 1, len))) {
                    now += D4[dir];
                    path0.push_back(now);
                }
                dir = (dir + 1) % 4;
            }
            if (phase == 0) {
                path = path0;
            } else {
                ranges::reverse(path0);
                path.insert(path.end(), path0.begin(), path0.end());
            }
        }
        paths.push_back(path);
    }

    {
        VV<P> paths2;
        for (auto path : paths) {
            for (int _ : iota(0, 2)) {
                for (int _ : iota(0, 4)) {
                    for (auto& p : path) {
                        p = P(p.c, N - 1 - p.r);
                    }
                    paths2.push_back(path);
                }
                for (auto& p : path) {
                    swap(p.r, p.c);
                }
            }
        }
        paths = paths2;
    }
    sort(paths);
    paths.erase(std::unique(paths.begin(), paths.end()), paths.end());
    dbg(paths.size());

    {
        // validation
        auto corner = [&](P p) {
            return (p.r == 0 || p.r == N - 1) && (p.c == 0 || p.c == N - 1);
        };
        for (auto path : paths) {
            assert(corner(path.front()));
            assert(corner(path.back()));
            int n = int(path.size());
            for (int i : iota(0, n - 1)) {
                assert(path[i].near4(path[i + 1]));
            }
        }
    }

    V<pair<int, V<P>>> preds;

    for (auto path1 : paths) {
        if (path1.front() != P(0, 0)) continue;
        for (auto path2 : paths) {
            if (path1.back() != path2.front()) continue;
            V<P> path = path1;
            path.insert(path.end(), path2.begin(), path2.end());
            path.erase(std::unique(path.begin(), path.end()), path.end());
            int cost = calc_best_actions(path).first;
            preds.push_back({cost, path});
        }
    }
    sort(preds, [&](auto l, auto r) {
        return l.first < r.first;
    });
    dbg(preds[0].first, preds.size(), global_sw.msecs());
    return preds;
}

int main() {
    int min_cost = TEN(9);
    V<P> min_ps;

    auto check = [&](V<P> ps) {
        auto [cost, actions] = calc_best_actions(ps);
        if (cost == TEN(9)) return;
        assert(cost == calc_cost(actions));
        if (cost < min_cost) {
            dbg(min_cost, "->", cost);
            min_cost = cost;
            min_ps = ps;
        }
    };

    const int K = 4;
    const int MSECS1 = 1500;
    const int MSECS_LAST = 1900;
    auto preds = gen();
    if (preds.size() > K) preds.resize(K);

    int k = int(preds.size());
    for (int i : iota(0, k)) {
        auto [cost, ps] = preds[i];
        int rem = MSECS1 - global_sw.msecs();
        ps = opt(cost, ps, rem / (k - i));
        check(ps);
    }

    {
        // final
        min_ps = opt(min_cost, min_ps, MSECS_LAST - global_sw.msecs());
    }
    auto [cost, actions] = calc_best_actions(min_ps);
    dbg(min_cost, cost, calc_cost(actions));

    output(actions);
    return 0;
}
