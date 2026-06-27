// #undef YOSUPO_LOCAL

#if 0 and !defined(__clang__)
#include <vector>
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#pragma GCC optimize("Ofast")
#endif


#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include <vector>


#include <algorithm>
#include <array>
#include <concepts>
#include <map>
#include <set>
#include <tuple>


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



#include <bit>
#include <cassert>
#include <cmath>
#include <limits>
#include <random>
#include <type_traits>


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

#include <stdio.h>
#include <unistd.h>

#include <cctype>
#include <cstdio>
#include <cstring>
#include <sstream>



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

#include <ranges>

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
Random gen = Random(12345);
StopWatch global_sw;

// dir infomation
// 0:R, 1:D, 2:L, 3:U

const int N = 30;
const int M = 10; // num of robots
const int K = 10; // num of buttons

const array<Coord, M> ROBOTS = []() {
    int dummy;
    sc.read(dummy, dummy, dummy); // N, M, K
    array<Coord, M> robots;
    for (int i : iota(0, M)) {
        int r, c;
        sc.read(r, c);
        robots[i] = Coord(r, c);
    }
    return robots;
}();

const array<bitset<N - 1>, N> WALL_H = []() {
    array<bitset<N - 1>, N> walls;
    for (int i : iota(0, N)) {
        string s;
        sc.read(s);

        for (int j : iota(0, N - 1)) {
            walls[i][j] = (s[j] == '1');
        }
    }
    return walls;
}();

const array<bitset<N>, N - 1> WALL_W = []() {
    array<bitset<N>, N - 1> walls;
    for (int j : iota(0, N - 1)) {
        string s;
        sc.read(s);

        for (int i : iota(0, N)) {
            walls[j][i] = (s[i] == '1');
        }
    }
    return walls;
}();

Coord next_pos(const Coord& c, int dir) {
    if (dir == -1) return c;
    if (dir == 0) {
        if (c.c() == N - 1 || WALL_H[c.r()][c.c()]) return c;
    }
    if (dir == 1) {
        if (c.r() == N - 1 || WALL_W[c.r()][c.c()]) return c;
    }
    if (dir == 2) {
        if (c.c() == 0 || WALL_H[c.r()][c.c() - 1]) return c;
    }
    if (dir == 3) {
        if (c.r() == 0 || WALL_W[c.r() - 1][c.c()]) return c;
    }
    return c.move4(dir);
}

// assign[i][j] = (0, 1, 2, 3, -1) = direction or skip
using Assign = array<array<int, M>, K>;
// array of the id of pushed buttons
using Actions = V<int>;

void print_answer(const Assign& assign, const Actions& actions) {
    for (int b = 0; b < K; b++) {
        for (int i = 0; i < M; i++) {
            if (i) pr.write(' ');
            int d = assign[b][i];
            assert(-1 <= d && d < 4);
            pr.write("SRDLU"[d + 1]);
        }
        pr.writeln();
    }
    for (int x : actions) {
        assert(0 <= x && x < K);
        pr.writeln(x);
    }
}

int score_formula(int R, int T) {
    if (R == 0) return 10000 - T;
    return N * N - R;
}

struct State {
    array<Coord, M> robots;
    array<bitset<N>, N> vis;
    State() : robots(ROBOTS), vis({}) {
        for (int i = 0; i < M; i++) vis[robots[i].r()][robots[i].c()] = true;
    }

    void apply(const array<int, M>& dirs) {
        for (int i = 0; i < M; i++) {
            int dir = dirs[i];
            robots[i] = next_pos(robots[i], dir);
            vis[robots[i].r()][robots[i].c()] = true;
        }
    }

    int count_unvisited() const {
        int R = 0;
        for (int i = 0; i < N; i++) for (int j = 0; j < N; j++) if (!vis[i][j]) R++;
        return R;
    }
};

int calc_score(const Assign& assign, const Actions& actions) {
    int T = int(actions.size());
    assert(T <= 2000);

    State state;
    for (int b : actions) {
        state.apply(assign[b]);
    }
    int R = state.count_unvisited();
    return score_formula(R, T);
}

// 最近傍未訪問への BFS（単一ロボ)
static inline vector<int> bfs_path(const array<bitset<N>, N>& vis, Coord s) {
    vector<vector<int>> dist(N, vector<int>(N, -1));
    vector<vector<Coord>> par(N, vector<Coord>(N, {-1, -1}));
    vector<vector<int>> pdir(N, vector<int>(N, -1));
    queue<Coord> q;
    q.push(s);
    dist[s.r()][s.c()] = 0;

    Coord tgt{-1, -1};
    while (!q.empty()) {
        auto p = q.front();
        q.pop();
        if (!vis[p.r()][p.c()]) {
            tgt = p;
            break;
        }
        for (int d = 0; d < 4; d++) {
            auto np = next_pos(p, d);
            if (p == np) continue;
            int nr = np.r(), nc = np.c();
            if (dist[nr][nc] != -1) continue;
            dist[nr][nc] = dist[p.r()][p.c()] + 1;
            par[nr][nc] = p;
            pdir[nr][nc] = d;
            q.push(np);
        }
    }
    vector<int> path;
    if (tgt == Coord{-1, -1}) return path;
    for (auto cur = tgt; !(cur == s);) {
        int d = pdir[cur.r()][cur.c()];
        path.push_back(d);
        cur = par[cur.r()][cur.c()];
    }
    ranges::reverse(path);
    return path;
}

const array<ull, 1000> HASHES = []() {
    array<ull, 1000> hashes;
    for (int i = 0; i < 1000; i++) {
        hashes[i] = uniform<ull>(gen);
    }
    return hashes;
}();

// ---------- パラメータ / 山登り ----------
struct Params {
    int A = 0, B = 0, C = 0, D = 0, Kstep = 4, Lrep = 15;  // 6^A, 7^B, K, L
    array<array<int, M>, 10> a0_7{};          // 0..7 の割当 (dir or -1)
};


static inline Params random_initial_params() {
    Params P;
    P.A = uniform(0, 20, gen);
    P.B = uniform(0, 20, gen);
    P.C = uniform(0, 20, gen);
    P.D = uniform(0, 20, gen);
    P.Kstep = uniform(3, 8, gen);
    int target = uniform(200, 300, gen);
    int front = P.A + P.B;
    P.Lrep = max(1, (target - front) / (4 * (P.Kstep + 1)));

    // 構造化ランダム： (0,2) / (3,5) は対向、(1,4) は直交寄り、6/7
    // は初動（S混ぜる）
    auto opp = [](int d) { return d ^ 2; };  // L<->R(0^2), D<->U(1^2=3)
    for (int r = 0; r < M; r++)
        for (int b = 0; b < 8; b++) P.a0_7[b][r] = -1;

    auto coin = [&]() { return uniform_bool(gen); };
    for (int r = 0; r < M; r++) {
        if (coin()) {                 // 0/2 横
            int d0 = coin() ? 0 : 2;  // L or R
            P.a0_7[0][r] = d0;
            P.a0_7[2][r] = opp(d0);
            P.a0_7[1][r] = coin() ? 1 : 3;  // D or U
        } else {                            // 0/2 縦
            int d0 = coin() ? 1 : 3;        // D or U
            P.a0_7[0][r] = d0;
            P.a0_7[2][r] = opp(d0);
            P.a0_7[1][r] = coin() ? 0 : 2;  // L or R
        }
        if (coin()) {  // 3/5 横
            int d3 = coin() ? 0 : 2;
            P.a0_7[3][r] = d3;
            P.a0_7[5][r] = opp(d3);
            P.a0_7[4][r] = coin() ? 1 : 3;
        } else {  // 3/5 縦
            int d3 = coin() ? 1 : 3;
            P.a0_7[3][r] = d3;
            P.a0_7[5][r] = opp(d3);
            P.a0_7[4][r] = coin() ? 0 : 2;
        }
        // 6/7 は 2/5 で S、3/5 でランダム方向
        if (uniform(0, 5, gen) >= 2) {
            P.a0_7[6][r] = uniform(0, 3, gen);
        }
        if (uniform(0, 5, gen) >= 2) {
            P.a0_7[7][r] = uniform(0, 3, gen);
        }
        if (uniform(0, 5, gen) >= 2) {
            P.a0_7[8][r] = uniform(0, 3, gen);
        }
        if (uniform(0, 5, gen) >= 2) {
            P.a0_7[9][r] = uniform(0, 3, gen);
        }
    }
    return P;
}


static inline void mutate(Params& P) {
    int typ = uniform(0, 99, gen);
    if (typ < 20) {
        int abc = uniform(0, 3, gen);
        if (abc == 0) {
            P.A = max(0, P.A + uniform(-2, 2, gen));
        } else if (abc == 1) {
            P.B = max(0, P.B + uniform(-2, 2, gen));
        } else if (abc == 2) {
            P.C = max(0, P.C + uniform(-2, 2, gen));
        } else {
            P.D = max(0, P.D + uniform(-2, 2, gen));
        }
    } else if (typ < 40) {
        P.Kstep = max(1, min(12, P.Kstep + uniform(-1, 1, gen)));
    } else if (typ < 60) {
        P.Lrep = max(1, P.Lrep + (uniform(-1, 1, gen)));
    } else {
        int b = uniform(0, 8, gen);
        int r = uniform(0, M - 1, gen);
        P.a0_7[b][r] = uniform(-1, 3, gen);
    }
}

// ----- 1 個の Params を評価して (Assign,Actions) を構築 -----
static inline pair<Assign, Actions> build_solution(const Params& P) {
    const int TMAX = 2 * N * N;

    Assign assign{};
    // 0..7 を埋める。8/9 は一旦 S
    for (int b : iota(0, K)) {
        for (int i : iota(0, K)) {
            assign[i][b] = -1;
        }
    }
    for (int b : iota(0, 9)) {
        for (int i : iota(0, K)) {
            assign[b][i] = P.a0_7[b][i];
        }
    }

    Actions seq;
    
    State state;
    auto press = [&](int b) {
        state.apply(assign[b]);
        seq.push_back(b);
    };

    // 前段: 6^A, 7^B, (0^K,1,2^K,1)^L, (3^K,4,5^K,4)^L
    for (int t = 0; t < P.A; t++) {
        press(6);
    }
    for (int t = 0; t < P.B; t++) {
        press(7);
    }

    for (int t = 0; t < P.Lrep; t++) {
        for (int k = 0; k < P.Kstep; k++)
            press(0);
        press(1);
        for (int k = 0; k < P.Kstep; k++)
            press(2);
        press(1);
    }

    for (int t = 0; t < P.C; t++) {
        press(8);
    }
    for (int t = 0; t < P.D; t++) {
        press(9);
    }

    for (int t = 0; t < P.Lrep; t++) {
        for (int k = 0; k < P.Kstep; k++)
            press(3);
        press(4);
        for (int k = 0; k < P.Kstep; k++)
            press(5);
        press(4);
    }

    // すでに TMAX ならここで返す
    if ((int)seq.size() >= TMAX) return {assign, seq};

    // --- 単一ロボ BFS 回収：全ロボ試してベストを採用 ---
    long long bestScore = -1;
    Assign bestAssign = assign;
    Actions bestSeq = seq;

    auto dir_to_btn = [&](const Assign& A, int FIN) {
        array<int, 4> map{};
        map.fill(-1);
        for (int b = 0; b < 8; b++) {
            int d = A[b][FIN];
            if (d >= 0 && map[d] == -1) map[d] = b;  // 0..7 を優先
        }
        return map;
    };

    for (int fin = 0; fin < M; fin++) {
        bool used[4] = {false, false, false, false};
        int kinds = 0;
        for (int b = 0; b < 8; b++) {
            int d = assign[fin][b];
            if (d >= 0 && !used[d]) {
                used[d] = true;
                kinds++;
            }
        }
        if (kinds < 4) continue;  // 1 種類しか無いならスキップ

        Assign A2 = assign;
        // 8/9 は FIN のみ割り当て、他ロボは S
        for (int r = 0; r < M; r++) A2[9][r] = -1;
        vector<int> missing;
        for (int d = 0; d < 4; d++)
            if (!used[d]) missing.push_back(d);
        if (!missing.empty()) A2[9][fin] = missing[0];
        //if ((int)missing.size() >= 2) A2[9][fin] = missing[1];

        auto map = dir_to_btn(A2, fin);
        auto set_if_missing = [&](int d, int bidx) {
            if (d >= 0 && map[d] == -1) map[d] = bidx;
        };
        //if (A2[8][fin] >= 0) set_if_missing(A2[8][fin], 8);
        if (A2[9][fin] >= 0) set_if_missing(A2[9][fin], 9);

        bool ok = true;
        for (int d = 0; d < 4; d++)
            if (map[d] == -1) {
                ok = false;
                break;
            }
        if (!ok) continue;

        // シミュレータを front 終了時点から複製

        State st2 = state;
        Actions sq2 = seq;

        auto press2 = [&](int b) {
            if ((int)sq2.size() >= TMAX) return;
            st2.apply(A2[b]);
            sq2.push_back(b);
        };

        while ((int)sq2.size() < TMAX) {
            auto path = bfs_path(st2.vis, st2.robots[fin]);
            if (path.empty()) break;
            for (int d : path) {
                if ((int)sq2.size() >= TMAX) break;
                press2(map[d]);
            }
            if (st2.count_unvisited() == 0) break;
        }

        int R = st2.count_unvisited();
        int T = (int)sq2.size();
        auto score = score_formula(R, T);
        if (score > bestScore) {
            bestScore = score;
            bestAssign = A2;
            bestSeq = sq2;
        }
    }
    dbg(bestScore);

    return {bestAssign, bestSeq};
}

static inline double est_score_lightweight(double progress, const Params& P, double threshold) {
    Assign assign{};
    // 0..7 を埋める。8/9 は一旦 S
    for (int b : iota(0, K)) {
        for (int i : iota(0, K)) {
            assign[i][b] = -1;
        }
    }
    for (int b : iota(0, 9)) {
        for (int i : iota(0, K)) {
            assign[b][i] = P.a0_7[b][i];
        }
    }

    int T = 0;
    State state;
    auto press = [&](int b) {
        state.apply(assign[b]);
        T++;
    };

    // 前段: 6^A, 7^B, (0^K,1,2^K,1)^L, (3^K,4,5^K,4)^L
    for (int t = 0; t < P.A; t++) {
        press(6);
    }
    for (int t = 0; t < P.B; t++) {
        press(7);
    }

    for (int t = 0; t < P.Lrep; t++) {
        for (int k = 0; k < P.Kstep; k++) press(0);
        press(1);
        for (int k = 0; k < P.Kstep; k++) press(2);
        press(1);
    }

    for (int t = 0; t < P.C; t++) {
        press(8);
    }
    for (int t = 0; t < P.D; t++) {
        press(9);
    }

    for (int t = 0; t < P.Lrep; t++) {
        for (int k = 0; k < P.Kstep; k++) press(3);
        press(4);
        for (int k = 0; k < P.Kstep; k++) press(5);
        press(4);
    }
    if (10000 - T < threshold) return -1;

    int R = state.count_unvisited();

    double score = (R == 0) ? (double)(10000 - T) : (10000 - (T + R + 20 * pow(progress, 1.3)));
    return score;
}

// -------- solve(): 焼きなましでベスト解を作る --------
pair<Assign, Actions> solve(const int& TL) {
    Params cur = random_initial_params();
    auto cur_score = est_score_lightweight(0.0, cur, -1);
    
    Params best_params = cur;
    auto best_score = cur_score;

    dbg(best_score);

    int times = 0;
    // 温度パラメータ
    StopWatch sw;
    double start_temp = 2.0;
    double end_temp = 0.1;
    while (true) {
        times++;

        int msecs = sw.msecs();
        if (msecs >= TL) break;


        // 温度の計算（線形減衰）
        double progress = msecs / (double)TL;
        double temp = start_temp * (1.0 - progress) + end_temp * progress;
        
        Params cand = cur;
        mutate(cand);
        
        // 前段長が TMAX を大きく超えそうなら L を縮める
        int frontLen = cand.A + cand.B + 4 * cand.Lrep * (cand.Kstep + 1);
        if (frontLen >= 2 * N * N) cand.Lrep = max(1, cand.Lrep - 1);

        // 軽量スコア関数で評価
        double threshold = cur_score + temp * log(random_01(gen));


        double score = est_score_lightweight(progress, cand, threshold);

        // 焼きなまし判定
        bool accept = score >= threshold;

        if (accept) {
            cur = cand;
            cur_score = score;
            
            // ベスト更新
            if (score > best_score) {
                best_score = score;
                best_params = cand;
                dbg(best_score, times, temp);
            }
        }
    }
    dbg(times);

    // 最後に一回だけbuild_solutionを呼ぶ
    auto best = build_solution(best_params);
    dbg(calc_score(best.first, best.second));
    
    return best;
}

int main() {
    int best_score = -1;
    Assign best_assign;
    Actions best_actions;

    const int TRY = 3;
    const int TL = 1950 / TRY;  // 余裕を持って 2s 未満
    for (int _ : iota(0, TRY)) {
        auto [assign, actions] = solve(TL);
        int score = calc_score(assign, actions);
        if (score > best_score) {
            best_score = score;
            best_assign = assign;
            best_actions = actions;
        }
    }

    dbg(best_score);

    // F**king optimization
    // while (best_actions.size()) {
    //     auto best_actions2 = best_actions;
    //     best_actions2.pop_back();
    //     int score = calc_score(best_assign, best_actions2);
    //     if (score >= best_score) {
    //         best_score = score;
    //         best_actions = best_actions2;
    //     } else {
    //         break;
    //     }
    // }
    // dbg(best_score);

    print_answer(best_assign, best_actions);

    return 0;
}
