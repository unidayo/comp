// #undef YOSUPO_LOCAL
#define NDEBUG

#include <bit>
#include <queue>
#if 0 and !defined(__clang__)
#include <vector>
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#pragma GCC optimize("Ofast")
#endif




#include <algorithm>
#include <cassert>
#include <vector>

namespace atcoder {

// Implement (union by size) + (path compression)
// Reference:
// Zvi Galil and Giuseppe F. Italiano,
// Data structures and algorithms for disjoint set union problems
struct dsu {
  public:
    dsu() : _n(0) {}
    explicit dsu(int n) : _n(n), parent_or_size(n, -1) {}

    int merge(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        int x = leader(a), y = leader(b);
        if (x == y) return x;
        if (-parent_or_size[x] < -parent_or_size[y]) std::swap(x, y);
        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;
        return x;
    }

    bool same(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        return leader(a) == leader(b);
    }

    int leader(int a) {
        assert(0 <= a && a < _n);
        return _leader(a);
    }

    int size(int a) {
        assert(0 <= a && a < _n);
        return -parent_or_size[leader(a)];
    }

    std::vector<std::vector<int>> groups() {
        std::vector<int> leader_buf(_n), group_size(_n);
        for (int i = 0; i < _n; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<int>> result(_n);
        for (int i = 0; i < _n; i++) {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < _n; i++) {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [&](const std::vector<int>& v) { return v.empty(); }),
            result.end());
        return result;
    }

  private:
    int _n;
    // root node: -1 * component size
    // otherwise: parent
    std::vector<int> parent_or_size;

    int _leader(int a) {
        if (parent_or_size[a] < 0) return a;
        return parent_or_size[a] = _leader(parent_or_size[a]);
    }
};

}  // namespace atcoder



#include <cstddef>
#include <iterator>
#include <string>
#include <utility>


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

#include <initializer_list>


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

    void fill(const T& val) { std::fill_n(d, h * w, val); }

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



inline Random gen(123456);
inline StopWatch global_sw;

inline Scanner sc = Scanner(stdin);
inline Printer pr = Printer(stdout);

const int N = []() {
    int n;
    sc.read(n);
    return n;
}();

const int K = []() {
    int k;
    sc.read(k);
    return k;
}();

const int T = []() {
    int t;
    sc.read(t);
    return t;
}();

const Vector2D<bool> WALL_V = []() {
    Vector2D<bool> wall_v(N, N - 1);
    for (int i = 0; i < N; i++) {
        std::string s;
        sc.read(s);
        for (int j = 0; j < N - 1; j++) {
            wall_v[{i, j}] = (s[j] == '1');
        }
    }
    return wall_v;
}();

const Vector2D<bool> WALL_H = []() {
    Vector2D<bool> wall_h(N - 1, N);
    for (int i = 0; i < N - 1; i++) {
        std::string s;
        sc.read(s);
        for (int j = 0; j < N; j++) {
            wall_h[{i, j}] = (s[j] == '1');
        }
    }
    return wall_h;
}();

const V<Coord> TARGETS = []() {
    V<Coord> targets;
    targets.reserve(K);
    for (int i = 0; i < K; i++) {
        int r, c;
        sc.read(r, c);
        targets.push_back(Coord(r, c));
    }
    return targets;
}();

const Coord SIZE = Coord(N, N);

const Vector2D<array<Coord, 4>> MOVE_TO = []() {
    Vector2D<array<Coord, 4>> move_to(N, N);
    for (auto p : SIZE.cells()) {
        const int r = p.r();
        const int c = p.c();
        auto& dest = move_to[p];
        for (int d = 0; d < 4; d++) {
            bool can = false;
            if (d == 0) {
                can = (c + 1 < N) && !WALL_V[{r, c}];
            } else if (d == 1) {
                can = (r + 1 < N) && !WALL_H[{r, c}];
            } else if (d == 2) {
                can = (c > 0) && !WALL_V[{r, c - 1}];
            } else {
                can = (r > 0) && !WALL_H[{r - 1, c}];
            }
            dest[d] = can ? p.move4(d) : p;
        }
    }
    return move_to;
}();

const Vector2D<Vector2D<int>> DIST = []() {
    Vector2D<Vector2D<int>> dist(N, N, Vector2D<int>(N, N, -1));

    V<Coord> q(N * N);
    for (auto s : SIZE.cells()) {
        auto& cur = dist[s];
        cur[s] = 0;
        int head = 0, tail = 0;
        q[tail++] = s;
        while (head < tail) {
            Coord v = q[head++];
            int nd = cur[v] + 1;
            for (int d = 0; d < 4; d++) {
                Coord nxt = MOVE_TO[v][d];
                if (nxt == v) continue;
                if (cur[nxt] != -1) continue;
                cur[nxt] = nd;
                q[tail++] = nxt;
            }
        }
    }
    return dist;
}();

const int MIN_STEPS = []() {
    int total = 0;
    for (int i = 0; i + 1 < K; i++) {
        total += DIST[TARGETS[i]][TARGETS[i + 1]];
    }
    return total;
}();

const array<array<int, 4>, 24> DIR_PERMS = []() {
    array<array<int, 4>, 24> perms;
    array<int, 4> base = {0, 1, 2, 3};
    int idx = 0;
    do {
        perms[idx] = base;
        idx++;
    } while (std::next_permutation(base.begin(), base.end()));    
    assert(idx == 24);
    return perms;
}();


struct Rule {
    int s = -1;  // new state
    int c = -1;  // new color
    int m = -1;  // move

    bool operator==(const Rule& other) const {
        return s == other.s && c == other.c && m == other.m;
    }

    string dump() const {
        string st = "(";
        st += std::to_string(s);
        st += " ";
        st += std::to_string(c);
        st += " ";
        st += "?RDLUS"[m + 1];
        st += ")";
        return st;
    }

    bool can_merge(const Rule& other) const {
        if (m != -1 && other.m != -1 && m != other.m) {
            return false;
        }
        if (s != -1 && other.s != -1 && s != other.s) {
            return false;
        }
        if (c != -1 && other.c != -1 && c != other.c) {
            return false;
        }
        return true;
    }

    Rule merge(const Rule& other) const {
        assert(can_merge(other));

        Rule res = *this;
        if (other.m != -1) {
            res.m = other.m;
        }
        if (other.s != -1) {
            res.s = other.s;
        }
        if (other.c != -1) {
            res.c = other.c;
        }
        return res;
    }
};

struct Answer {
    int num_s = -1, num_c = -1;  // number of state, number of color
    int state;                   // initial state
    Vector2D<int> color;         // initial color
    Vector2D<Rule>
        rules;  // rule table (state, color) -> (new state, new color, dir)

    bool is_valid() const {
        if (!(0 <= num_s && 0 <= num_c)) return false;
        if (!(0 <= state && state < num_s)) return false;
        if (!(color.h == N && color.w == N)) return false;
        if (!(rules.h == num_s && rules.w == num_c)) return false;

        for (auto p : SIZE.cells()) {
            int col = color[p];
            if (!(0 <= col && col < num_c)) return false;
        }
        for (int s = 0; s < num_s; s++) {
            for (int c = 0; c < num_c; c++) {
                Rule rule = rules[{s, c}];
                if (!(-1 <= rule.s && rule.s < num_s)) return false;
                if (!(-1 <= rule.c && rule.c < num_c)) return false;
                if (!(-1 <= rule.m && rule.m <= 4)) return false;
            }
        }
        return true;
    }

    void validate() const {
        assert(1 <= num_s);
        assert(1 <= num_c);
        assert(0 <= state && state < num_s);
        assert(color.h == N && color.w == N);
        assert(rules.h == num_s && rules.w == num_c);

        for (auto p : SIZE.cells()) {
            int col = color[p];
            assert(-1 <= col && col < num_c);
        }

        for (int s = 0; s < num_s; s++) {
            for (int c = 0; c < num_c; c++) {
                Rule rule = rules[{s, c}];
                assert(-1 <= rule.s && rule.s < num_s);
                assert(-1 <= rule.c && rule.c < num_c);
                assert(-1 <= rule.m && rule.m <= 4);
            }
        }

        Vector2D<int> board = color;

        Coord pos = TARGETS.front();
        int s = state;

        int visited = 1;
        for (int step = 0; step < T && visited < K; step++) {
            int c = board[pos];
            assert(s != -1);
            assert(c != -1);
            auto rule = rules[{s, c}];

            s = rule.s;
            board[pos] = rule.c;

            assert(rule.m != -1);
            pos = MOVE_TO[pos][rule.m];
            if (pos == TARGETS[visited]) {
                visited++;
                if (visited == K) {
                    return;
                }
            }
        }
        assert(false);
    }

    int score() const {
        if (num_s == -1 || num_c == -1) return TEN(9);
        validate();
        return num_s + num_c;
    }
};

inline void print_answer(const Answer& answer) {
    answer.validate();

    V<tuple<int, int, int, int, int>> output_rules;
    {
        Vector2D<bool> used(answer.num_s, answer.num_c, false);

        Vector2D<int> board = answer.color;

        Coord pos = TARGETS.front();
        int s = answer.state;

        int visited = 1;

        for (int step = 0; step < T && visited < K; step++) {
            int c = board[pos];
            assert(s != -1);
            assert(c != -1);            
            auto rule = answer.rules[{s, c}];

            if (!used[{s, c}]) {
                used[{s, c}] = true;
                output_rules.emplace_back(s, c, rule.s, rule.c, rule.m);
            }

            s = rule.s;
            board[pos] = rule.c;
            assert(rule.m != -1);
            pos = MOVE_TO[pos][rule.m];
            if (pos == TARGETS[visited]) {
                visited++;
                if (visited == K) {
                    break;
                }
            }
        }
    }

    pr.writeln(answer.num_c, ' ', answer.num_s, ' ', output_rules.size());

    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (c) pr.write(' ');
            int col = answer.color[{r, c}];
            if (col == -1) col = 0;
            pr.write(col);
        }
        pr.writeln();
    }
    auto f = [&](int s) {
        return (s - answer.state + answer.num_s) % answer.num_s;
    };

    for (const auto& [s, c, ns, nc, m] : output_rules) {
        pr.writeln(c, ' ', f(s), ' ', nc == -1 ? 0 : nc, ' ', f(ns == -1 ? 0 : ns), ' ', "RDLUS"[m]);
    }
}

// dir
// - 0:right
// - 1:down
// - 2:left
// - 3:up

struct Ticker {
    int period;
    int ticked;
    Ticker(int _period) {
        period = _period;
        ticked = -1;
    }
    bool tick(int msecs) {
        if (ticked == -1 || msecs - ticked >= period) {
            ticked = msecs;
            return true;
        }
        return false;
    };
};

struct Step {
    Coord pos;
    int move;

    string dump() const {
        return "(" + pos.dump() + "," + "?RDLUS"[move + 1] + ")";
    }
};

void validate_steps(const V<Step>& steps) {
    int len = int(steps.size());
    assert(1 <= len && len <= T);
    for (int i : iota(0, len - 1)) {
        assert(steps[i + 1].pos == MOVE_TO[steps[i].pos][steps[i].move]);
    }
    assert(MOVE_TO[steps.back().pos][steps.back().move] == TARGETS.back());
    int visited = 0;
    for (int i : iota(0, len)) {
        if (steps[i].pos == TARGETS[visited]) {
            visited++;
            if (visited == K - 1) break;
        }
    }
    assert(visited == K - 1);
}

struct Path {
    int len = 0;
    V<Step> steps;
    V<int> moves;
    V<Coord> pos;
    V<int> nxt_visit;
    V<int> pre_visit;

    Path(const V<Step>& steps) {
        len = int(steps.size());
        this->steps = steps;
        pos.resize(len);
        moves.resize(len);
        for (int t : iota(0, len)) {
            pos[t] = steps[t].pos;
            moves[t] = steps[t].move;
        }

        nxt_visit.resize(len);
        pre_visit.resize(len);
        {
            Vector2D<int> last(N, N, -1);
            for (int t : iota(0, len)) {
                pre_visit[t] = last[pos[t]];
                last[pos[t]] = t;
            }
        }
        {
            Vector2D<int> last(N, N, -1);
            for (int t : iota(0, len) | reverse) {
                nxt_visit[t] = last[pos[t]];
                last[pos[t]] = t;
            }
        }
    }
};

V<Step> find_good_step_v4() {
    StopWatch sw;

    // actual cost
    auto calc_cost = [&](const VV<Step>& steps) {
        int total = 0;
        for (const auto& step : steps) {
            total += int(step.size());
        }
        assert(total <= T);

        Vector2D<int> dir(N, N, 4);
        int used = 0;
        for (int k : iota(0, K - 1) | reverse) {
            for (const auto& step : steps[k] | reverse) {
                if (dir[step.pos] == 4 || dir[step.pos] == step.move) {
                    dir[step.pos] = step.move;
                } else {
                    dir[step.pos] = -1;
                    used++;
                }
            }
        }
        return used;
    };

    VV<Step> steps(K - 1);
    for (int k : iota(0, K - 1)) {
        Coord p = TARGETS[k];
        while (p != TARGETS[k + 1]) {
            for (int d : iota(0, 4)) {
                Coord q = MOVE_TO[p][d];
                if (DIST[q][TARGETS[k + 1]] + 1 != DIST[p][TARGETS[k + 1]]) {
                    continue;
                }
                steps[k].push_back(Step{p, d});
                p = q;
                break;
            }
        }
    }

    int total_spfa_step = 0;

    Vector2D<pair<int, int>> ldir(N, N);
    Vector2D<array<int, 4>> rdir_cnt(N, N);
    Vector2D<u8> rdir_mask(N, N);
    array<int, 16> mask_to_rdir;
    mask_to_rdir[0] = 4;
    for (uint i : iota(1, 16)) {
        int pc = std::popcount(i);
        if (pc == 1) {
            mask_to_rdir[i] = std::countr_zero(i);
        } else {
            mask_to_rdir[i] = -1;
        }
    }

    auto cost = [&](Coord p, int d) {
        int c = 0;
        if (mask_to_rdir[rdir_mask[p]] == -1) {
            c = 1;
        } else if (mask_to_rdir[rdir_mask[p]] == 4) {
            if (ldir[p].first != d) {
                c = ldir[p].second;
            }
        } else {
            if (mask_to_rdir[rdir_mask[p]] != 4 &&
                mask_to_rdir[rdir_mask[p]] != d) {
                c = 1;
            }
            if (ldir[p].first != d &&
                (mask_to_rdir[rdir_mask[p]] == 4 ||
                 mask_to_rdir[rdir_mask[p]] == ldir[p].first)) {
                c += ldir[p].second;
            }
        }
        return c;
    };

    // simple deque
    vector<Coord> que(500); // >= N * N

    Vector2D<pair<int, int>> dist(N, N);  // (essential, total)
    Vector2D<int> prev_st(N, N, -1);
    Vector2D<bool> inq(N, N, false);
    vector<pair<Coord, int>> trace;
    auto improve = [&](int k) -> void {
        const auto start = TARGETS[k];
        const auto goal = TARGETS[k + 1];

        int max_steps = T;
        for (int i : iota(0, K - 1)) {
            if (i == k) continue;
            max_steps -= int(steps[i].size());
        }
        dist.fill(pair(TEN(6), TEN(6)));
        dist[start] = {0, 0};

        int l = 0, r = 0;
        auto push_front = [&](Coord p) {
            l--;
            if (l == -1) l += 500;
            que[l] = p;
        };
        auto push_back = [&](Coord p) {
            que[r] = p;
            r++;
            if (r == 500) r = 0;
        };
        auto pop_front = [&]() {
            Coord p = que[l];
            l++;
            if (l == 500) l = 0;
            return p;
        };
        push_back(start);
        inq[start] = true;

        int goal_st = -1;

        trace.clear();
        prev_st[start] = -1;
        while (l != r) {
            if (l == r) break;
            total_spfa_step++;

            Coord p = pop_front();
            inq[p] = false;

            int st = int(trace.size());
            trace.push_back({p, prev_st[p]});

            auto cur_dist = dist[p];
            if (p == goal) {
                goal_st = st;
            }

            if (cur_dist.second == max_steps) continue;
            if (dist[goal] <= pair(cur_dist.first, cur_dist.second + 1)) {
                continue;
            }

            for (int d : DIR_PERMS[random(0, 23, gen)]) {
                Coord nxt = MOVE_TO[p][d];
                if (nxt == p) continue;
                if (cur_dist.second + 1 + DIST[goal][nxt] > max_steps) continue;

                pair<int, int> n_dist = {cur_dist.first + cost(p, d),
                                         cur_dist.second + 1};

                if (n_dist < dist[nxt]) {
                    dist[nxt] = n_dist;
                    prev_st[nxt] = st;
                    if (!inq[nxt]) {
                        if (l != r && n_dist < dist[que[l]]) {
                            push_front(nxt);
                        } else {
                            push_back(nxt);
                        }
                        inq[nxt] = true;
                    }
                }
            }
        }
        if (goal_st == -1) return;
        int st = goal_st;
        steps[k].clear();
        while (st != 0) {
            auto p = trace[st].first;
            int nst = trace[st].second;
            auto q = trace[nst].first;
            int mv = -1;
            for (int d : iota(0, 4)) {
                if (MOVE_TO[q][d] == p) {
                    mv = d;
                    break;
                }
            }
            steps[k].push_back(Step{q, mv});
            st = nst;
        }
        ranges::reverse(steps[k]);
    };

    auto to_random_path = [&](int k) {
        steps[k].clear();

        Coord p = TARGETS[k];
        while (p != TARGETS[k + 1]) {
            for (int d : DIR_PERMS[random(0, 23, gen)]) {
                Coord q = MOVE_TO[p][d];
                if (DIST[q][TARGETS[k + 1]] + 1 != DIST[p][TARGETS[k + 1]]) {
                    continue;
                }
                steps[k].push_back(Step{p, d});
                p = q;
                break;
            }
        }
    };

    int current_cost = calc_cost(steps);

    VV<Step> best_steps = steps;
    int best_cost = current_cost;

    const int TL = 1500;
    const double TEMP_START = 20.0;
    const double TEMP_END = 0.01;

    int total_iter = 0;
    int accept_iter = 0;
    Ticker ticker(100);
    while (sw.msecs() < TL) {
        ldir.fill({-1, 0});
        for (int k : iota(0, K - 1)) {
            for (const auto& step : steps[k]) {
                rdir_cnt[step.pos][step.move]++;
                rdir_mask[step.pos] |= 1 << step.move;
            }
        }
        for (int k : iota(0, K - 1)) {
            int msecs = sw.msecs();
            if (msecs >= TL) break;
            if (ticker.tick(msecs)) {
                dbg("hill climb anneal", msecs, best_cost, current_cost,
                    accept_iter, total_iter, total_spfa_step);
            }
            const double progress = min(1.0, msecs / double(TL));
            const double temp = TEMP_START + (TEMP_END - TEMP_START) * progress;

            for (const auto& step : steps[k]) {
                rdir_cnt[step.pos][step.move]--;
                if (rdir_cnt[step.pos][step.move] == 0) {
                    rdir_mask[step.pos] &= ~(1 << step.move);
                }
            }

            V<Step> backup = steps[k];
            int op = random(0, 4, gen);

            int pre_step_cost = 0;
            for (const auto& s : steps[k]) {
                pre_step_cost += cost(s.pos, s.move);
            }

            if (op == 0) {
                to_random_path(k);
            } else {
                improve(k);
            }

            int post_step_cost = 0;
            for (const auto& s : steps[k]) {
                post_step_cost += cost(s.pos, s.move);
            }
            int new_cost = current_cost + (post_step_cost - pre_step_cost);

            bool accept = new_cost <= current_cost;
            if (!accept) {
                double prob = std::exp((current_cost - new_cost) / temp);
                if (yosupo::random_01(gen) < prob) accept = true;
            }

            if (accept) {
                accept_iter++;
                current_cost = new_cost;
                if (new_cost < best_cost) {
                    best_cost = new_cost;
                    best_steps = steps;
                }
            } else {
                steps[k] = std::move(backup);
            }
            total_iter++;

            for (const auto& step : steps[k]) {
                if (ldir[step.pos].first == step.move) {
                    ldir[step.pos].second++;
                } else {
                    ldir[step.pos] = {step.move, 1};
                }
            }
        }
    }
    dbg("hill climb anneal end", best_cost, total_iter, accept_iter);

    V<Step> flattend;
    for (int i : iota(0, K - 1)) {
        flattend.insert(flattend.end(), best_steps[i].begin(),
                        best_steps[i].end());
    }
    validate_steps(flattend);
    return flattend;
}

V<Coord> grouping_fast(const Path& path,
                       V<int> force_nxt_c,
                       int num_s,
                       int num_c) {
    const int len = path.len;
    assert(len);
    assert(int(force_nxt_c.size()) == len);

    V<Coord> nodes(len);
    Vector2D<Rule> rules(num_s, num_c, Rule(-1, -1, -1));
    Vector2D<array<Coord, 5>> to_node(num_s + 1, num_c + 1 + 4);
    for (int s : iota(0, num_s + 1)) {
        for (int c : iota(0, num_c + 1 + 4)) {
            to_node[{s, c}].fill({-1, -1});
        }
    }

    auto get_rule = [&](int t) {
        int nxt_s = (t == len - 1) ? -1 : nodes[t + 1].r();
        int nxt_c =
            (path.nxt_visit[t] == -1) ? -1 : nodes[path.nxt_visit[t]].c();
        if (force_nxt_c[t] != -1) {
            assert(nxt_c == -1);
            nxt_c = force_nxt_c[t] + num_c;
        }
        return Rule{.s = nxt_s, .c = nxt_c, .m = path.moves[t]};
    };

    array<array<Coord, 5>, 5> unused4;
    for (int r : iota(-1, 4)) {
        for (int c : iota(-1, 4)) {
            unused4[r + 1][c + 1] = {r == -1 ? 0 : r, c == -1 ? 0 : c};
        }
    }

    for (int t = len - 1; t >= 0; --t) {
        const auto new_rule = get_rule(t);

        Coord node = {-1, -1};
        [&]() {
            for (int f : iota(0, 3)) {
                int new_s = new_rule.s;
                int new_c = new_rule.c;
                if (f & 1) new_s = -1;
                if (f & 2) new_c = -1;
                auto trg = to_node[{new_s + 1, new_c + 1}][new_rule.m + 1];
                if (trg != Coord(-1, -1)) {
                    auto rule = rules[trg];
                    assert(rule.s == new_s);
                    assert(rule.c == new_c);
                    assert(rule.m == new_rule.m);
                    assert(rule.can_merge(new_rule));
                    node = trg;
                    return;
                }
            }
            for (int f : iota(0, 4)) {
                int ps_m = (t - 1 >= 0) ? path.moves[t - 1] : -1;
                int pc_m = (path.pre_visit[t] != -1)
                               ? path.moves[path.pre_visit[t]]
                               : -1;

                if (f & 1) ps_m = -1;
                if (f & 2) pc_m = -1;
                auto& cand = unused4[ps_m + 1][pc_m + 1];
                while (cand.c() < num_c) {
                    if (cand.r() >= num_s) {
                        cand.r() %= (ps_m == -1) ? 1 : 4;
                        cand.c() += (pc_m == -1) ? 1 : 4;
                        continue;
                    }
                    auto& rule = rules[cand];
                    if (rule.can_merge(new_rule)) {
                        node = cand;
                        return;
                    }
                    cand.r() += (ps_m == -1) ? 1 : 4;
                }
            }
        }();

        if (node == pair(-1, -1)) {
            return {};
        }

        nodes[t] = node;

        auto rule = rules[node];
        assert(rule.can_merge(new_rule));
        to_node[{rule.s + 1, rule.c + 1}][rule.m + 1] = {-1, -1};
        rule = rule.merge(new_rule);
        rules[node] = rule;
        assert(
            (to_node[{rule.s + 1, rule.c + 1}][rule.m + 1] == Coord(-1, -1)));
        to_node[{rule.s + 1, rule.c + 1}][rule.m + 1] = node;
    }

    return nodes;
}

V<Coord> make_scan_list(int num_s, int num_c, Random& rng) {
    V<Coord> scan_list;
    scan_list.reserve(num_s * num_c);
    for (int c : iota(0, num_c)) {
        for (int s : iota(0, num_s)) {
            scan_list.push_back({s, c});
        }
        std::shuffle(scan_list.begin() + c * num_s,
                     scan_list.begin() + (c + 1) * num_s, rng);
    }
    return scan_list;
}

V<Coord> grouping_mid(const Path& path,
                      V<int> force_nxt_c,
                      int num_s,
                      int num_c,
                      bool heavy,
                      const V<Coord>* scan_list_opt = nullptr,
                      int* survived_turns = nullptr) {
    const int len = path.len;
    assert(len);
    assert(int(force_nxt_c.size()) == len);

    V<Coord> nodes(len);
    Vector2D<Rule> rules(num_s, num_c, Rule(-1, -1, -1));
    Vector2D<array<Coord, 5>> to_node(num_s + 1, num_c + 1 + 4);
    for (int s : iota(0, num_s + 1)) {
        for (int c : iota(0, num_c + 1 + 4)) {
            to_node[{s, c}].fill({-1, -1});
        }
    }
    Vector2D<int> used(num_s, num_c, 0);
    V<Coord> scan_list_buf;
    const V<Coord>* scan_list = scan_list_opt;
    if (!scan_list) {
        scan_list_buf = make_scan_list(num_s, num_c, gen);
        scan_list = &scan_list_buf;
    } else {
        assert(int(scan_list->size()) == num_s * num_c);
    }
    int unused_cnt = 0;
    // Coord unused = {0, 0};

    auto set_rule = [&](const Coord& node, const Rule& rule) {
        if (rules[node] != Rule(-1, -1, -1)) {
            assert((to_node[{rules[node].s + 1, rules[node].c + 1}]
                           [rules[node].m + 1] == node));
        }
        to_node[{rules[node].s + 1, rules[node].c + 1}][rules[node].m + 1] = {
            -1, -1};
        rules[node] = rule;
        if (rules[node] != Rule(-1, -1, -1)) {
            assert((to_node[{rules[node].s + 1, rules[node].c + 1}]
                           [rules[node].m + 1] == Coord(-1, -1)));
        }
        to_node[{rules[node].s + 1, rules[node].c + 1}][rules[node].m + 1] =
            node;
    };

    auto get_rule = [&](int t) {
        int nxt_s = (t == len - 1) ? -1 : nodes[t + 1].r();
        int nxt_c =
            (path.nxt_visit[t] == -1) ? -1 : nodes[path.nxt_visit[t]].c();
        if (force_nxt_c[t] != -1) {
            assert(nxt_c == -1);
            nxt_c = force_nxt_c[t] + num_c;
        }
        return Rule{.s = nxt_s, .c = nxt_c, .m = path.moves[t]};
    };

    map<int, int> maybe_good, hit_good;
    for (int t = len - 1; t >= 0; --t) {
        Coord node = {-1, -1};
        [&]() {
            {
                const auto new_rule = get_rule(t);
                for (int f : iota(0, 3)) {
                    int new_s = new_rule.s;
                    int new_c = new_rule.c;
                    if (f & 1) new_s = -1;
                    if (f & 2) new_c = -1;
                    auto trg = to_node[{new_s + 1, new_c + 1}][new_rule.m + 1];
                    if (trg != Coord(-1, -1)) {
                        auto rule = rules[trg];
                        assert(rule.s == new_s);
                        assert(rule.c == new_c);
                        assert(rule.m == new_rule.m);
                        assert(rule.can_merge(new_rule));
                        node = trg;
                        hit_good[0]++;
                        return;
                    }
                }
            }
            if (heavy) {
                int nxt = path.nxt_visit[t];
                if (nxt != -1 && used[nodes[nxt]] == 1) {
                    const auto orig_new_rule = get_rule(t);
                    const auto nxt_rule = get_rule(nxt);

                    for (int nxt_c : iota(0, num_c)) {
                        if (nxt_c == nodes[nxt].c()) continue;

                        auto new_nxt_node = Coord{nodes[nxt].r(), nxt_c};

                        if (!nxt_rule.can_merge(rules[new_nxt_node])) continue;
                        auto new_nxt_rule = nxt_rule.merge(rules[new_nxt_node]);

                        auto new_rule = orig_new_rule;
                        new_rule.c = nxt_c;

                        if (new_nxt_rule.can_merge(new_rule)) {
                            node = new_nxt_node;
                        } else {
                            for (int f : iota(0, 3)) {
                                int new_s = new_rule.s;
                                int new_c = new_rule.c;
                                if (f & 1) new_s = -1;
                                if (f & 2) new_c = -1;
                                auto trg = to_node[{new_s + 1, new_c + 1}]
                                                  [new_rule.m + 1];
                                if (trg == Coord(-1, -1) || trg == nodes[nxt]) {
                                    continue;
                                }
                                auto rule = rules[trg];
                                assert(rule.s == new_s);
                                assert(rule.c == new_c);
                                assert(rule.m == new_rule.m);
                                assert(rule.can_merge(new_rule));
                                node = trg;
                                break;
                            }
                        }

                        if (node != Coord(-1, -1)) {
                            used[nodes[nxt]]--;
                            set_rule(nodes[nxt], Rule(-1, -1, -1));
                            nodes[nxt].c() = nxt_c;
                            set_rule(nodes[nxt], new_nxt_rule);
                            used[nodes[nxt]]++;
                            assert((rules[node] != Rule(-1, -1, -1)));
                            assert(get_rule(t) == new_rule);
                            hit_good[1]++;
                            return;
                        }
                    }
                    maybe_good[-2]++;
                }
                {
                    int f = 0;
                    if (nxt != -1 && used[nodes[nxt]] == 1) {
                        f |= 2;
                    } else if (nxt != -1) {
                        bool ng = false;
                        for (int t2 : iota(t + 1, len)) {
                            if (nodes[t2] != nodes[nxt]) continue;
                            if (path.pre_visit[t2] != -1 &&
                                t < path.pre_visit[t2]) {
                                ng = true;
                                break;
                            }
                        }
                        if (!ng) {
                            // ok actually
                            f |= 4;
                        }
                    }
                    if (t + 1 < len && used[nodes[t + 1]] == 1) {
                        f |= 1;
                    }
                    maybe_good[f]++;
                }

                if (t - 1 >= 0) {
                    const auto new_rule = get_rule(t);
                    for (int s : iota(0, num_s)) {
                        auto pre_rule = get_rule(t - 1);
                        pre_rule.s = s;
                        bool ok = false;
                        if (pre_rule == new_rule) {
                            ok = true;
                        } else {
                            for (int f : iota(0, 3)) {
                                int new_s = pre_rule.s;
                                int new_c = pre_rule.c;
                                if (f & 1) new_s = -1;
                                if (f & 2) new_c = -1;
                                auto trg = to_node[{new_s + 1, new_c + 1}]
                                                  [pre_rule.m + 1];
                                if (trg != Coord(-1, -1)) {
                                    ok = true;
                                    break;
                                }
                            }
                        }
                        if (!ok) continue;

                        for (int c : iota(0, num_c)) {
                            auto& rule = rules[{s, c}];
                            if (rule.can_merge(new_rule)) {
                                node = {s, c};
                                hit_good[2]++;
                                return;
                            }
                        }
                    }
                }
            }
            {
                const auto new_rule = get_rule(t);
                while (unused_cnt < int(scan_list->size())) {
                    const auto& cand = (*scan_list)[unused_cnt];
                    unused_cnt++;
                    auto& rule = rules[cand];
                    if (rule.can_merge(new_rule)) {
                        node = cand;
                        hit_good[3]++;
                        return;
                    }
                }
            }
        }();

        if (node == pair(-1, -1)) {
            // dbg(maybe_good);
            // dbg(hit_good);
            if (survived_turns) *survived_turns = len - t - 1;
            return {};
        }

        const auto new_rule = get_rule(t);
        nodes[t] = node;

        auto rule = rules[node];
        assert(rule.can_merge(new_rule));
        set_rule(node, rule.merge(new_rule));
        used[node]++;
    }
    // dbg(maybe_good);
    // dbg(hit_good);
    if (survived_turns) *survived_turns = len;
    return nodes;
}

struct ScanListHCResult {
    int survived = 0;
    V<Coord> scan_list;
    V<Coord> nodes;
};

ScanListHCResult hill_climb_scan_list(const Path& path,
                                      const V<int>& force_nxt_c,
                                      int num_s,
                                      int num_c,
                                      bool heavy,
                                      int time_limit_ms) {
    Random rng(987654321);

    auto eval = [&](const V<Coord>& scan) {
        int survived = 0;
        auto nodes = grouping_mid(path, force_nxt_c, num_s, num_c, heavy, &scan,
                                  &survived);
        return pair<int, V<Coord>>(survived, std::move(nodes));
    };

    V<Coord> cur_scan = make_scan_list(num_s, num_c, rng);
    auto [cur_survived, cur_nodes] = eval(cur_scan);

    ScanListHCResult best{cur_survived, cur_scan, cur_nodes};
    dbg("scan_list_hc_init", cur_survived, num_s, num_c, heavy);

    StopWatch sw;
    int iter = 0;
    const double TEMP_START = 5.0;
    const double TEMP_END = 0.05;
    while (sw.msecs() < time_limit_ms && best.survived < path.len) {
        iter++;
        auto pre_scan = cur_scan;

        if (uniform_bool(rng)) {
            int i = random(0, int(cur_scan.size()) - 2, rng);
            int j = random(0, int(cur_scan.size()) - 1, rng);
            if (i == j) continue;
            std::swap(cur_scan[i], cur_scan[j]);
        } else {
            // random cur_scan rotate by std::rotate
            auto [l, r] = uniform_pair(0, int(cur_scan.size()) - 1, rng);
            r++;
            int k = random(l + 1, r - 1, rng);
            std::rotate(cur_scan.begin() + l, cur_scan.begin() + k,
                        cur_scan.begin() + r);
        }
        auto [cand_survived, cand_nodes] = eval(cur_scan);

        int delta = cand_survived - cur_survived;
        const double progress =
            min(1.0, sw.msecs() / double(max(1, time_limit_ms)));
        const double temp = TEMP_START + (TEMP_END - TEMP_START) * progress;
        bool accept = delta >= 0 || (temp > 0 && yosupo::random_01(rng) <
                                                     std::exp(delta / temp));

        if (accept) {
            cur_survived = cand_survived;
            cur_nodes = std::move(cand_nodes);
            if (cur_survived > best.survived ||
                (cur_survived == best.survived &&
                 (!cur_nodes.empty() && best.nodes.empty()))) {
                best = {cur_survived, cur_scan, cur_nodes};
            }
        } else {
            cur_scan = std::move(pre_scan);
        }
    }
    dbg("scan_list_hc_iter", iter, "limit", time_limit_ms);
    return best;
}

V<Coord> calc_sqrt_assign(const Path& path) {
    const int len = path.len;
    assert(len);
    int l = 1;
    while (l * l < len) l++;
    V<Coord> nodes(len);
    for (int i : iota(0, len)) {
        nodes[i] = {i / l, i % l};
    }
    return nodes;
}

Answer solve(const Path& path) {
    const int len = path.len;
    int all_same_len = 0;
    int cut_len = 0;
    int empty_pos = 0;

    Vector2D<V<pair<int, int>>> moves(N, N);
    for (int t : iota(0, path.len)) {
        moves[path.pos[t]].push_back({t, path.moves[t]});
    }

    Vector2D<int> force_color(N, N, -1);
    V<bool> core_step(len);
    V<int> force_step_c(len, -1);
    for (auto p : SIZE.cells()) {
        if (moves[p].empty()) {
            empty_pos++;
            continue;
        }
        int last_move = moves[p].back().second;
        int r = int(moves[p].size());
        while (r >= 1 && moves[p][r - 1].second == last_move) {
            r--;
        }

        if (r == 0) {
            // all same
            all_same_len += int(moves[p].size());
            force_color[p] = last_move;
            continue;
        }

        int m = int(moves[p].size());
        cut_len += m - r;
        for (int i : iota(0, r)) {
            core_step[moves[p][i].first] = true;
        }
        force_step_c[moves[p][r - 1].first] = last_move;
    }

    V<Step> core_steps;
    V<int> force_nxt_c;
    for (int t : iota(0, len)) {
        if (core_step[t]) {
            core_steps.push_back(path.steps[t]);
            force_nxt_c.push_back(force_step_c[t]);
        }
    }
    Path core_path(core_steps);
    dbg(len, core_path.len, all_same_len, cut_len, empty_pos);

    auto gen_answer = [&](pair<int, int> best_size,
                          V<Coord> best_nodes) -> Answer {
        Answer answer;
        answer.num_s = best_size.first;
        answer.num_c = best_size.second + 4;
        answer.rules =
            Vector2D<Rule>(answer.num_s, answer.num_c, Rule{-1, -1, -1});
        answer.color = Vector2D<int>(N, N, -1);

        if (best_nodes.empty()) {
            answer.state = 0;
        } else {
            answer.state = best_nodes.empty() ? 0 : best_nodes.front().r();

            for (int t : iota(0, int(core_path.len)) | reverse) {
                auto new_rule = Rule{
                    .s = (t + 1 == core_path.len) ? -1 : best_nodes[t + 1].r(),
                    .c = (core_path.nxt_visit[t] == -1)
                             ? -1
                             : best_nodes[core_path.nxt_visit[t]].c(),
                    .m = core_path.moves[t],
                };
                if (force_nxt_c[t] != -1) {
                    assert(new_rule.c == -1);
                    new_rule.c = force_nxt_c[t] + best_size.second;
                }
                auto& rule = answer.rules[best_nodes[t]];
                assert(rule.can_merge(new_rule));
                rule = rule.merge(new_rule);
                answer.color[core_path.pos[t]] = best_nodes[t].c();
            }
            // core_pathの末尾から追加で移動床を使う可能性があるのでstateをセットする必要がある
            if (answer.rules[best_nodes.back()].s == -1) {
                answer.rules[best_nodes.back()].s = 0;
            }
        }

        // force color
        for (int s : iota(0, answer.num_s)) {
            for (int c : iota(0, 4)) {
                answer.rules[{s, c + best_size.second}] =
                    Rule{s, c + best_size.second, c};
            }
        }
        for (auto p : SIZE.cells()) {
            if (force_color[p] != -1) {
                assert(answer.color[p] == -1);
                answer.color[p] = force_color[p] + best_size.second;
            }
        }

        answer.validate();
        return answer;
    };

    if (core_path.len == 0) {
        return gen_answer({1, 0}, {});
    }

    auto shrink_nodes = [&](V<Coord>& nodes) -> pair<int, int> {
        V<bool> sv, cv;
        for (auto& node : nodes) {
            int s = node.r();
            int c = node.c();
            while (s >= (int)sv.size()) sv.push_back(false);
            while (c >= (int)cv.size()) cv.push_back(false);
            sv[s] = true;
            cv[c] = true;
        }
        V<int> s_map(sv.size(), -1), c_map(cv.size(), -1);
        int ns = 0, nc = 0;
        for (int i = 0; i < (int)sv.size(); i++) {
            if (sv[i]) {
                s_map[i] = ns++;
            }
        }
        for (int i = 0; i < (int)cv.size(); i++) {
            if (cv[i]) {
                c_map[i] = nc++;
            }
        }
        for (auto& node : nodes) {
            node = {s_map[node.r()], c_map[node.c()]};
        }
        return {ns, nc};
    };

    auto best_nodes = calc_sqrt_assign(core_path);
    auto best_size = shrink_nodes(best_nodes);

    dbg("initial size (sqrt decomp)", best_size,
        best_size.first + best_size.second);

    // optimize by grouping_fast
    for (int jump : {8, 4, 1}) {
        while (true) {
            int target = best_size.first + best_size.second - jump;
            if (target <= 1) break;
            bool find = false;

            for (int d : {0, 1, -1, 2, -2, 3, -3}) {
                if (jump != 1 && d != 0) continue;

                int s = target / 2 - d, c = target - s;
                if (s <= 0 || c <= 0) continue;

                auto nodes = grouping_fast(core_path, force_nxt_c, s, c);
                if (nodes.empty()) {
                    continue;
                }
                auto size = shrink_nodes(nodes);
                assert(size.first <= s && size.second <= c);

                find = true;
                best_size = size;
                dbg("found better size", best_size,
                    best_size.first + best_size.second);
                best_nodes = nodes;
                break;
            }
            if (!find) {
                dbg("failed to find", jump);
                break;
            }
        }
    }

    dbg("after grouping_fast", global_sw.msecs(), best_size,
        best_size.first + best_size.second);

    // optimize by grouping_mid
    for (int jump : {4, 1}) {
        while (true) {
            int target = best_size.first + best_size.second - jump;
            if (target <= 1) break;

            bool find = false;
            for (int d : {0, 1, -1, 2, -2, 3, -3}) {
                if (jump != 1 && d != 0) continue;

                int s = target / 2 - d, c = target - s;
                if (s <= 0 || c <= 0) continue;

                auto nodes = grouping_mid(core_path, force_nxt_c, s, c, false);
                if (nodes.empty()) {
                    continue;
                }
                auto size = shrink_nodes(nodes);
                assert(size.first <= s && size.second <= c);

                find = true;
                best_size = size;
                dbg("found better size", best_size,
                    best_size.first + best_size.second);
                best_nodes = nodes;
                break;
            }
            if (!find) {
                dbg("failed to find", jump);
                break;
            }
        }
    }
    dbg("after grouping_mid", global_sw.msecs(), best_size,
        best_size.first + best_size.second);

    // optimize by grouping_mid with heavy
    for (int jump : {2, 1}) {
        while (true) {
            int target = best_size.first + best_size.second - jump;
            if (target <= 1) break;

            bool find = false;
            for (int d : {0, 1, -1}) {
                if (jump != 1 && d != 0) continue;

                int s = target / 2 - d, c = target - s;
                if (s <= 0 || c <= 0) continue;

                auto nodes = grouping_mid(core_path, force_nxt_c, s, c, true);
                if (nodes.empty()) {
                    continue;
                }
                auto size = shrink_nodes(nodes);
                assert(size.first <= s && size.second <= c);

                find = true;
                best_size = size;
                dbg("found better size", best_size,
                    best_size.first + best_size.second);
                best_nodes = nodes;
                break;
            }
            if (!find) {
                dbg("failed to find", jump);
                break;
            }
        }
    }
    dbg("after grouping_heavy", global_sw.msecs(), best_size,
        best_size.first + best_size.second);

    while (true) {
        int target = best_size.first + best_size.second - 1;
        if (target == 0) break;
        if (global_sw.msecs() >= 1950) break;

        int tl = 1950 - global_sw.msecs();
        dbg(tl);
        for (int d : {0}) {
            int s = target / 2 - d, c = target - s;
            if (s <= 0 || c <= 0) continue;

            auto res =
                hill_climb_scan_list(core_path, force_nxt_c, s, c, true, tl);
            dbg("scan_list_hc_result", target, s, c, res.survived,
                core_path.len, res.nodes.empty());

            if (res.survived != core_path.len || res.nodes.empty()) continue;
            auto nodes = res.nodes;
            auto size = shrink_nodes(nodes);
            assert(size.first <= s && size.second <= c);
            if (size.first + size.second < best_size.first + best_size.second) {
                best_size = size;
                dbg("found better size", best_size,
                    best_size.first + best_size.second);
                best_nodes = nodes;
            }
            break;
        }
    }

    return gen_answer(best_size, best_nodes);
}

Path gen_moves() {
    V<Step> steps = find_good_step_v4();
    return Path(steps);
}

int main() {
    auto moves = gen_moves();
    dbg("PATH GENERATED", global_sw.msecs());
    auto path = Path(moves);
    auto answer = solve(path);
    dbg("ANSWER CALCULATED", global_sw.msecs(), answer.score());
    print_answer(answer);
    return 0;
}
