#include <atcoder/all>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <type_traits>
//#include <unordered_map>
//#include "robin_hood.h"
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef _MSC_VER
#include <intrin.h>
#endif
#ifdef __GNUC__
#include <x86intrin.h>
#endif

#ifdef __clang__
#pragma clang attribute push(__attribute__((target("arch=skylake"))),          \
                             apply_to = function)
#elif defined(__GNUC__)
#pragma GCC target(                                                            \
    "sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,tune=native")
#pragma GCC optimize("Ofast")
#endif

// ========================== macroes ==========================

//#define NDEBUG

#define rep(i, n) for (auto i = 0; (i) < (n); (i)++)
#define rep1(i, n) for (auto i = 1; (i) <= (n); (i)++)
#define rep3(i, s, n) for (auto i = (s); (i) < (n); (i)++)

#define ASSERT_RANGE(value, left, right)                                       \
    ASSERT((left <= value) && (value < right),                                 \
           "`%s` (%d) is out of range [%d, %d)", #value, value, left, right)

#define CHECK(var)                                                             \
    do {                                                                       \
        std::cout << #var << '=' << var << endl;                               \
    } while (false)

// ========================== utils ==========================

using namespace std;
using ll = long long;
constexpr double PI = 3.1415926535897932;

template <class T, class S> inline bool chmin(T& m, S q) {
    if (m > q) {
        m = q;
        return true;
    } else
        return false;
}

template <class T, class S> inline bool chmax(T& m, const S q) {
    if (m < q) {
        m = q;
        return true;
    } else
        return false;
}

// 乱数
struct Random {
    using ull = unsigned long long;
    // unsigned seed;
    ull seed;
    inline Random(const unsigned& seed_) : seed(seed_) { assert(seed != 0u); }
    const inline ull& next() {
        // seed ^= seed << 13;
        // seed ^= seed >> 17;
        // seed ^= seed << 5;
        seed ^= seed << 7;
        seed ^= seed >> 9;
        return seed;
    }
    // (0.0, 1.0)
    inline double random() {
        return (double)(unsigned)next() * (1.0 / (double)0x100000000ull);
    }
    // [0, right)
    inline int randint(const int& right) {
        return (ull)(unsigned)next() * right >> 32;
    }
    // [left, right)
    inline int randint(const int& left, const int& right) {
        return ((ull)(unsigned)next() * (right - left) >> 32) + left;
    }
    template <class Container> inline auto choice(const Container& weight) {
        using T = typename Container::value_type;
        auto sum_weight = (T)0;
        for (const auto w : weight)
            sum_weight += w;
        if constexpr (is_floating_point_v<T>) {
            auto r = random() * sum_weight;
            auto cum_weight = (T)0;
            for (auto i = 0; i < (int)weight.size(); i++) {
                cum_weight += weight[i];
                if (r < cum_weight)
                    return i;
            }
            return (int)weight.size() - 1;
        } else {
            auto r = randint(sum_weight);
            auto cum_weight = (T)0;
            for (auto i = 0; i < (int)weight.size(); i++) {
                cum_weight += weight[i];
                if (r < cum_weight)
                    return i;
            }
            assert(false);
            return (int)weight.size() - 1;
        }
    }
};

// 2 次元ベクトル
template <typename T> struct Vec2 {
    /*
    y 軸正は下方向
    x 軸正は右方向
    回転は時計回りが正（y 軸正を上と考えると反時計回りになる）
    */
    T y, x;
    constexpr inline Vec2() = default;
    constexpr Vec2(const T& arg_y, const T& arg_x) : y(arg_y), x(arg_x) {}
    inline Vec2(const Vec2&) = default;            // コピー
    inline Vec2(Vec2&&) = default;                 // ムーブ
    inline Vec2& operator=(const Vec2&) = default; // 代入
    inline Vec2& operator=(Vec2&&) = default;      // ムーブ代入
    template <typename S>
    constexpr inline Vec2(const Vec2<S>& v) : y((T)v.y), x((T)v.x) {}
    inline Vec2 operator+(const Vec2& rhs) const {
        return Vec2(y + rhs.y, x + rhs.x);
    }
    inline Vec2 operator+(const T& rhs) const { return Vec2(y + rhs, x + rhs); }
    inline Vec2 operator-(const Vec2& rhs) const {
        return Vec2(y - rhs.y, x - rhs.x);
    }
    template <typename S> inline Vec2 operator*(const S& rhs) const {
        return Vec2(y * rhs, x * rhs);
    }
    inline Vec2 operator*(const Vec2& rhs) const { // x + yj とみなす
        return Vec2(x * rhs.y + y * rhs.x, x * rhs.x - y * rhs.y);
    }
    template <typename S> inline Vec2 operator/(const S& rhs) const {
        assert(rhs != 0.0);
        return Vec2(y / rhs, x / rhs);
    }
    inline Vec2 operator/(const Vec2& rhs) const { // x + yj とみなす
        return (*this) * rhs.inv();
    }
    inline Vec2& operator+=(const Vec2& rhs) {
        y += rhs.y;
        x += rhs.x;
        return *this;
    }
    inline Vec2& operator-=(const Vec2& rhs) {
        y -= rhs.y;
        x -= rhs.x;
        return *this;
    }
    template <typename S> inline Vec2& operator*=(const S& rhs) const {
        y *= rhs;
        x *= rhs;
        return *this;
    }
    inline Vec2& operator*=(const Vec2& rhs) { return *this = (*this) * rhs; }
    inline Vec2& operator/=(const Vec2& rhs) { return *this = (*this) / rhs; }
    inline bool operator!=(const Vec2& rhs) const {
        return x != rhs.x || y != rhs.y;
    }
    inline bool operator==(const Vec2& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    inline void rotate(const double& rad) { *this = rotated(rad); }
    inline Vec2<double> rotated(const double& rad) const {
        return (*this) * rotation(rad);
    }
    static inline Vec2<double> rotation(const double& rad) {
        return Vec2(sin(rad), cos(rad));
    }
    static inline Vec2<double> rotation_deg(const double& deg) {
        return rotation(PI * deg / 180.0);
    }
    inline Vec2<double> rounded() const {
        return Vec2<double>(round(y), round(x));
    }
    inline Vec2<double> inv() const { // x + yj とみなす
        const double norm_sq = l2_norm_square();
        assert(norm_sq != 0.0);
        return Vec2(-y / norm_sq, x / norm_sq);
    }
    inline double l2_norm() const { return sqrt(x * x + y * y); }
    inline double l2_norm_square() const { return x * x + y * y; }
    inline T l1_norm() const { return std::abs(x) + std::abs(y); }
    inline double abs() const { return l2_norm(); }
    inline double phase() const { // [-PI, PI) のはず
        return atan2(y, x);
    }
    inline double phase_deg() const { // [-180, 180) のはず
        return phase() * (180.0 / PI);
    }
    inline bool operator<(const Vec2& rhs) const {
        return y != rhs.y ? y < rhs.y : x < rhs.x;
    }
};
template <typename T, typename S>
inline Vec2<T> operator*(const S& lhs, const Vec2<T>& rhs) {
    return rhs * lhs;
}
template <typename T> ostream& operator<<(ostream& os, const Vec2<T>& vec) {
    os << vec.y << ' ' << vec.x;
    return os;
}

// 2 次元配列
template <class T, int height, int width> struct Board {
    array<T, height * width> data;
    template <class Int> constexpr inline auto& operator[](const Vec2<Int>& p) {
        return data[width * p.y + p.x];
    }
    template <class Int>
    constexpr inline const auto& operator[](const Vec2<Int>& p) const {
        return data[width * p.y + p.x];
    }
    template <class Int>
    constexpr inline auto& operator[](const initializer_list<Int>& p) {
        return data[width * *p.begin() + *(p.begin() + 1)];
    }
    template <class Int>
    constexpr inline const auto&
    operator[](const initializer_list<Int>& p) const {
        return data[width * *p.begin() + *(p.begin() + 1)];
    }
    constexpr inline void Fill(const T& fill_value) {
        fill(data.begin(), data.end(), fill_value);
    }
    void Print() const {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                cout << data[width * y + x] << " \n"[x == width - 1];
            }
        }
    }
};

// キュー
template <class T, int max_size> struct Queue {
    array<T, max_size> data;
    int left, right;
    inline Queue() : data(), left(0), right(0) {}
    inline Queue(initializer_list<T> init)
        : data(init.begin(), init.end()), left(0), right(init.size()) {}

    inline bool empty() const { return left == right; }
    inline void push(const T& value) {
        data[right] = value;
        right++;
    }
    inline void pop() { left++; }
    const inline T& front() const { return data[left]; }
    template <class... Args> inline void emplace(const Args&... args) {
        data[right] = T(args...);
        right++;
    }
    inline void clear() {
        left = 0;
        right = 0;
    }
    inline int size() const { return right - left; }
};

// スタック // コンストラクタ・デストラクタはちゃんと呼ぶ //
// 改修してからまともにテストしてないので注意
template <class T, int max_size> struct Stack {
    using value_type = T;
    array<char, max_size * sizeof(T)>
        buffer; // コンストラクタが呼ばれることを防ぐために char 型
    int right;
    inline Stack() : right(0) {} // buffer は初期化しない
    inline Stack(const int& n) : right(0) { resize(n); } // O(n)
    inline Stack(const int& n, const T& val) : right(0) { resize(n, val); }
    inline Stack(const initializer_list<T>& init) : right(0) {
        for (const auto& value : init)
            push(value);
    }
    inline Stack(const Stack& rhs) : right(0) { // コピー
        for (; right < rhs.right; right++)
            new (&data()[right]) T(rhs.data()[right]);
    }
    template <class S> inline Stack(const Stack<S, max_size>& rhs) : right() {
        for (; right < rhs.right; right++)
            new (&data()[right]) T(rhs.data()[right]);
    }
    template <class Container> Stack& operator=(const Container& rhs) {
        assert(rhs.size() <= max_size);
        clear();
        for (; right < rhs.right; right++)
            data()[right] = rhs[right];
        return *this;
    }
    Stack& operator=(Stack&&) = default;
    inline array<T, max_size>& data() { return *(array<T, max_size>*)&buffer; }
    inline array<T, max_size>& data() const {
        return *(array<T, max_size>*)&buffer;
    }
    inline bool empty() const { return 0 == right; }
    inline void push(const T& value) {
        // //#ifdef ONLINE_JUDGE
        // if (right == max_size)
        //     this_thread::sleep_for(chrono::seconds(30));

        // //#endif
        assert(right < max_size);
        new (&data()[right++]) T(value);
    }
    inline T pop() { // これだけ標準ライブラリと仕様が違う
        assert(right > 0);
        auto tmp = data()[--right];
        data()[right].~T();
        return tmp;
    }
    const inline T& top() const { return data()[right - 1]; }
    template <class... Args> inline void emplace(const Args&... args) {
        assert(right < max_size);
        new (&data()[right++]) T(args...);
    }
    inline void clear() {
        while (right > 0)
            data()[--right].~T();
    }
    inline void insert(const int& idx, const T& value) {
        assert(0 <= idx && idx <= right);
        assert(right < max_size);
        push(value); // コンストラクタを呼ぶ
        for (int i = right - 1; i > idx; i--) {
            data()[i] = data()[i - 1];
        }
        data()[idx] = value;
    }
    inline void del(const int& idx) { // erase
        assert(0 <= idx && idx < right);
        right--;
        for (int i = idx; i < right; i++)
            data()[i] = data()[i + 1];
        data()[right].~T();
    }
    inline int index(const T& value) const {
        for (int i = 0; i < right; i++)
            if (value == data()[i])
                return i;
        return -1;
    }
    inline void remove(const T& value) {
        int idx = index(value);
        assert(idx != -1);
        del(idx);
    }
    inline void resize(const int& sz) {
        assert(0 <= sz && sz <= max_size);
        while (right < sz)
            new (&data()[right++]) T();
        while (right > sz)
            data()[--right].~T();
    }
    inline void resize(const int& sz, const T& fill_value) {
        assert(0 <= sz && sz <= max_size);
        while (right < sz)
            new (&data()[right++]) T(fill_value);
        while (right > sz)
            data()[--right].~T();
    }
    inline int size() const { return right; }
    inline T& operator[](const int& n) {
        assert(0 <= n && n < right);
        return data()[n];
    }
    inline const T& operator[](const int n) const {
        assert(0 <= n && n < right);
        return data()[n];
    }
    inline T* begin() { return (T*)&buffer; }
    inline const T* begin() const { return (const T*)&buffer; }
    inline T* end() { return begin() + right; }
    inline const T* end() const { return begin() + right; }
    inline T& front() {
        assert(right > 0);
        return data()[0];
    }
    const inline T& front() const {
        assert(right > 0);
        return data()[0];
    }
    inline T& back() {
        assert(right > 0);
        return data()[right - 1];
    }
    const inline T& back() const {
        assert(right > 0);
        return data()[right - 1];
    }
    inline bool contains(const T& value) const {
        for (const auto& dat : *this) {
            if (value == dat)
                return true;
        }
        return false;
    }
    inline void sort() { std::sort(begin(), end()); }
    template <class Compare> inline void sort(const Compare& comp) {
        std::sort(begin(), end(), comp);
    }
    inline void deduplicate() {
        sort();
        resize(std::unique(begin(), end()) - begin());
    }
    inline void Print(ostream& os = cout, const string& sep = " ") {
        for (int i = 0; i < right; i++) {
            os << data()[i] << (i == right - 1 ? "" : sep);
        }
        os << endl;
    }
};

template <class T, int size = 0x100000, class KeyType = unsigned>
struct MinimumHashMap {
    // ハッシュの値が size 以下
    array<T, size> data;
    Stack<int, size> used;
    constexpr static KeyType mask = size - 1;
    inline MinimumHashMap() {
        static_assert((size & size - 1) == 0, "not pow of 2");
        memset(&data[0], (unsigned char)-1, sizeof(data));
    }
    inline T& operator[](const KeyType& key) {
        if (data[key] == (T)-1)
            used.push(key);
        return data[key];
    }
    inline void clear() {
        for (const auto& key : used)
            data[key] = (T)-1;
        used.right = 0;
    }
};

// 時間 (秒)
inline double Time() {
    return static_cast<double>(
               chrono::duration_cast<chrono::nanoseconds>(
                   chrono::steady_clock::now().time_since_epoch())
                   .count()) *
           1e-9;
}

// 2 分法
template <class Container, class T>
inline int SearchSorted(const Container& vec, const T& a) {
    return lower_bound(vec.begin(), vec.end(), a) - vec.begin();
}

// argsort
template <typename T, int n, typename result_type, bool reverse = false>
inline auto Argsort(const array<T, n>& vec) {
    array<result_type, n> res;
    iota(res.begin(), res.end(), 0);
    sort(res.begin(), res.end(),
         [&](const result_type& l, const result_type& r) {
             return reverse ? vec[l] > vec[r] : vec[l] < vec[r];
         });
    return res;
}

// popcount  // SSE 4.2 を使うべき
inline int Popcount(const unsigned int& x) {
#ifdef _MSC_VER
    return (int)__popcnt(x);
#else
    return __builtin_popcount(x);
#endif
}
inline int Popcount(const unsigned long long& x) {
#ifdef _MSC_VER
    return (int)__popcnt64(x);
#else
    return __builtin_popcountll(x);
#endif
}

// x >> n & 1 が 1 になる最小の n ( x==0 は未定義 )
inline int CountRightZero(const unsigned int& x) {
#ifdef _MSC_VER
    unsigned long r;
    _BitScanForward(&r, x);
    return (int)r;
#else
    return __builtin_ctz(x);
#endif
}
inline int CountRightZero(const unsigned long long& x) {
#ifdef _MSC_VER
    unsigned long r;
    _BitScanForward64(&r, x);
    return (int)r;
#else
    return __builtin_ctzll(x);
#endif
}

inline double MonotonicallyIncreasingFunction(const double& h,
                                              const double& x) {
    // 0 < h < 1
    // f(0) = 0, f(1) = 1, f(0.5) = h
    assert(h > 0.0 && h < 1.0);
    if (h == 0.5)
        return x;
    const double& a = (1.0 - 2.0 * h) / (h * h);
    return expm1(log1p(a) * x) / a;
}

inline double MonotonicFunction(const double& start, const double& end,
                                const double& h, const double& x) {
    // h: x = 0.5 での進捗率
    return MonotonicallyIncreasingFunction(h, x) * (end - start) + start;
}

template <typename T> struct Slice {
    T *left, *right;
    inline Slice(T* const& l, T* const& r) : left(l), right(r) {}
    inline T* begin() { return left; }
    inline const T* begin() const { return (const T*)left; }
    inline T* end() { return right; }
    inline const T* end() const { return (const T*)right; }
    inline int size() const { return distance(left, right); }
    inline T& operator[](const int& idx) { return left[idx]; }
    inline const T& operator[](const int& idx) const { return left[idx]; }
};

struct Edge {
    int from, to;
    bool operator<(const Edge& rhs) const {
        return make_pair(from, to) < make_pair(rhs.from, rhs.to);
    }
};

template <int max_n, int max_m> struct Graph {
    int n, m;
    Stack<int, max_m> edges;
    Stack<int, max_n + 1> lefts;

    Graph() = default;
    template <class Container>
    Graph(const int& n_, const Container& arg_edges) {
        n = n_;
        m = arg_edges.size();
        static Stack<Edge, max_m> edges_;
        edges_.clear();
        for (const auto& e : arg_edges) {
            edges_.push(e);
        }
        edges_.sort();
        edges.resize(m);
        lefts.resize(n + 1);
        for (int i = 0; i < m; i++) {
            edges[i] = edges_[i].to;
        }
        auto idx_edges = 0;
        for (int v = 0; v <= n; v++) {
            lefts[v] = idx_edges;
            while (idx_edges < m && edges_[idx_edges].from == v) {
                idx_edges++;
            }
        }
    }
    inline Slice<int> operator[](const int& v) {
        return Slice<int>(edges.begin() + lefts[v],
                          edges.begin() + lefts[v + 1]);
    }
    inline Slice<int> operator[](const int& v) const {
        return Slice<int>(edges.begin() + lefts[v],
                          edges.begin() + lefts[v + 1]);
    }
};

#ifdef _MSC_VER
inline unsigned int __builtin_clz(const unsigned int& x) {
    unsigned long r;
    _BitScanReverse(&r, x);
    return 31 - r;
}
inline unsigned long long __builtin_clzll(const unsigned long long& x) {
    unsigned long r;
    _BitScanReverse64(&r, x);
    return 63 - r;
}
#pragma warning(disable : 4146)
#endif
/*
iwi 先生の radix heap (https://github.com/iwiwi/radix-heap)
The MIT License (MIT)
Copyright (c) 2015 Takuya Akiba
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions: The above copyright notice and this
permission notice shall be included in all copies or substantial portions of the
Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
namespace radix_heap {
namespace internal {
template <bool Is64bit> class find_bucket_impl;

template <> class find_bucket_impl<false> {
  public:
    static inline constexpr size_t find_bucket(uint32_t x, uint32_t last) {
        return x == last ? 0 : 32 - __builtin_clz(x ^ last);
    }
};

template <> class find_bucket_impl<true> {
  public:
    static inline constexpr size_t find_bucket(uint64_t x, uint64_t last) {
        return x == last ? 0 : 64 - __builtin_clzll(x ^ last);
    }
};

template <typename T> inline constexpr size_t find_bucket(T x, T last) {
    return find_bucket_impl<sizeof(T) == 8>::find_bucket(x, last);
}

template <typename KeyType, bool IsSigned> class encoder_impl_integer;

template <typename KeyType> class encoder_impl_integer<KeyType, false> {
  public:
    typedef KeyType key_type;
    typedef KeyType unsigned_key_type;

    inline static constexpr unsigned_key_type encode(key_type x) { return x; }

    inline static constexpr key_type decode(unsigned_key_type x) { return x; }
};

template <typename KeyType> class encoder_impl_integer<KeyType, true> {
  public:
    typedef KeyType key_type;
    typedef typename std::make_unsigned<KeyType>::type unsigned_key_type;

    inline static constexpr unsigned_key_type encode(key_type x) {
        return static_cast<unsigned_key_type>(x) ^
               (unsigned_key_type(1) << unsigned_key_type(
                    std::numeric_limits<unsigned_key_type>::digits - 1));
    }

    inline static constexpr key_type decode(unsigned_key_type x) {
        return static_cast<key_type>(
            x ^ (unsigned_key_type(1)
                 << (std::numeric_limits<unsigned_key_type>::digits - 1)));
    }
};

template <typename KeyType, typename UnsignedKeyType>
class encoder_impl_decimal {
  public:
    typedef KeyType key_type;
    typedef UnsignedKeyType unsigned_key_type;

    inline static constexpr unsigned_key_type encode(key_type x) {
        return raw_cast<key_type, unsigned_key_type>(x) ^
               ((-(raw_cast<key_type, unsigned_key_type>(x) >>
                   (std::numeric_limits<unsigned_key_type>::digits - 1))) |
                (unsigned_key_type(1)
                 << (std::numeric_limits<unsigned_key_type>::digits - 1)));
    }

    inline static constexpr key_type decode(unsigned_key_type x) {
        return raw_cast<unsigned_key_type, key_type>(
            x ^
            (((x >> (std::numeric_limits<unsigned_key_type>::digits - 1)) - 1) |
             (unsigned_key_type(1)
              << (std::numeric_limits<unsigned_key_type>::digits - 1))));
    }

  private:
    template <typename T, typename U> union raw_cast {
      public:
        constexpr raw_cast(T t) : t_(t) {}
        operator U() const { return u_; }

      private:
        T t_;
        U u_;
    };
};

template <typename KeyType>
class encoder
    : public encoder_impl_integer<KeyType, std::is_signed<KeyType>::value> {};
template <>
class encoder<float> : public encoder_impl_decimal<float, uint32_t> {};
template <>
class encoder<double> : public encoder_impl_decimal<double, uint64_t> {};
} // namespace internal

template <typename KeyType, typename EncoderType = internal::encoder<KeyType>>
class radix_heap {
  public:
    typedef KeyType key_type;
    typedef EncoderType encoder_type;
    typedef typename encoder_type::unsigned_key_type unsigned_key_type;

    radix_heap() : size_(0), last_(), buckets_() {
        buckets_min_.fill(std::numeric_limits<unsigned_key_type>::max());
    }

    void push(key_type key) {
        const unsigned_key_type x = encoder_type::encode(key);
        assert(last_ <= x);
        ++size_;
        const size_t k = internal::find_bucket(x, last_);
        buckets_[k].emplace_back(x);
        buckets_min_[k] = std::min(buckets_min_[k], x);
    }

    key_type top() {
        pull();
        return encoder_type::decode(last_);
    }

    void pop() {
        pull();
        buckets_[0].pop_back();
        --size_;
    }

    size_t size() const { return size_; }

    bool empty() const { return size_ == 0; }

    void clear() {
        size_ = 0;
        last_ = key_type();
        for (auto& b : buckets_)
            b.clear();
        buckets_min_.fill(std::numeric_limits<unsigned_key_type>::max());
    }

    void swap(radix_heap<KeyType, EncoderType>& a) {
        std::swap(size_, a.size_);
        std::swap(last_, a.last_);
        buckets_.swap(a.buckets_);
        buckets_min_.swap(a.buckets_min_);
    }

  private:
    size_t size_;
    unsigned_key_type last_;
    std::array<std::vector<unsigned_key_type>,
               std::numeric_limits<unsigned_key_type>::digits + 1>
        buckets_;
    std::array<unsigned_key_type,
               std::numeric_limits<unsigned_key_type>::digits + 1>
        buckets_min_;

    void pull() {
        assert(size_ > 0);
        if (!buckets_[0].empty())
            return;

        size_t i;
        for (i = 1; buckets_[i].empty(); ++i)
            ;
        last_ = buckets_min_[i];

        for (unsigned_key_type x : buckets_[i]) {
            const size_t k = internal::find_bucket(x, last_);
            buckets_[k].emplace_back(x);
            buckets_min_[k] = std::min(buckets_min_[k], x);
        }
        buckets_[i].clear();
        buckets_min_[i] = std::numeric_limits<unsigned_key_type>::max();
    }
};

template <typename KeyType, typename ValueType,
          typename EncoderType = internal::encoder<KeyType>>
class pair_radix_heap {
  public:
    typedef KeyType key_type;
    typedef ValueType value_type;
    typedef EncoderType encoder_type;
    typedef typename encoder_type::unsigned_key_type unsigned_key_type;

    pair_radix_heap() : size_(0), last_(), buckets_() {
        buckets_min_.fill(std::numeric_limits<unsigned_key_type>::max());
    }

    void push(key_type key, const value_type& value) {
        const unsigned_key_type x = encoder_type::encode(key);
        assert(last_ <= x);
        ++size_;
        const size_t k = internal::find_bucket(x, last_);
        buckets_[k].emplace_back(x, value);
        buckets_min_[k] = std::min(buckets_min_[k], x);
    }

    void push(key_type key, value_type&& value) {
        const unsigned_key_type x = encoder_type::encode(key);
        assert(last_ <= x);
        ++size_;
        const size_t k = internal::find_bucket(x, last_);
        buckets_[k].emplace_back(x, std::move(value));
        buckets_min_[k] = std::min(buckets_min_[k], x);
    }

    template <class... Args> void emplace(key_type key, Args&&... args) {
        const unsigned_key_type x = encoder_type::encode(key);
        assert(last_ <= x);
        ++size_;
        const size_t k = internal::find_bucket(x, last_);
        buckets_[k].emplace_back(std::piecewise_construct,
                                 std::forward_as_tuple(x),
                                 std::forward_as_tuple(args...));
        buckets_min_[k] = std::min(buckets_min_[k], x);
    }

    key_type top_key() {
        pull();
        return encoder_type::decode(last_);
    }

    value_type& top_value() {
        pull();
        return buckets_[0].back().second;
    }

    void pop() {
        pull();
        buckets_[0].pop_back();
        --size_;
    }

    size_t size() const { return size_; }

    bool empty() const { return size_ == 0; }

    void clear() {
        size_ = 0;
        last_ = key_type();
        for (auto& b : buckets_)
            b.clear();
        buckets_min_.fill(std::numeric_limits<unsigned_key_type>::max());
    }

    void swap(pair_radix_heap<KeyType, ValueType, EncoderType>& a) {
        std::swap(size_, a.size_);
        std::swap(last_, a.last_);
        buckets_.swap(a.buckets_);
        buckets_min_.swap(a.buckets_min_);
    }

  private:
    size_t size_;
    unsigned_key_type last_;
    std::array<std::vector<std::pair<unsigned_key_type, value_type>>,
               std::numeric_limits<unsigned_key_type>::digits + 1>
        buckets_;
    std::array<unsigned_key_type,
               std::numeric_limits<unsigned_key_type>::digits + 1>
        buckets_min_;

    void pull() {
        assert(size_ > 0);
        if (!buckets_[0].empty())
            return;

        size_t i;
        for (i = 1; buckets_[i].empty(); ++i)
            ;
        last_ = buckets_min_[i];

        for (size_t j = 0; j < buckets_[i].size(); ++j) {
            const unsigned_key_type x = buckets_[i][j].first;
            const size_t k = internal::find_bucket(x, last_);
            buckets_[k].emplace_back(std::move(buckets_[i][j]));
            buckets_min_[k] = std::min(buckets_min_[k], x);
        }
        buckets_[i].clear();
        buckets_min_[i] = std::numeric_limits<unsigned_key_type>::max();
    }
};
} // namespace radix_heap

// clang-format off
//                 ______  _____                 ______                _________
//  ______________ ___  /_ ___(_)_______         ___  /_ ______ ______ ______  /
//  __  ___/_  __ \__  __ \__  / __  __ \        __  __ \_  __ \_  __ \_  __  /
//  _  /    / /_/ /_  /_/ /_  /  _  / / /        _  / / // /_/ // /_/ // /_/ /
//  /_/     \____/ /_.___/ /_/   /_/ /_/ ________/_/ /_/ \____/ \____/ \__,_/
//                                      _/_____/
//
// Fast & memory efficient hashtable based on robin hood hashing for C++11/14/17/20
// https://github.com/martinus/robin-hood-hashing
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2021 Martin Ankerl <http://martin.ankerl.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef ROBIN_HOOD_H_INCLUDED
#define ROBIN_HOOD_H_INCLUDED
#define ROBIN_HOOD_VERSION_MAJOR 3
#define ROBIN_HOOD_VERSION_MINOR 11
#define ROBIN_HOOD_VERSION_PATCH 5
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#if __cplusplus >= 201703L
#include <string_view>
#endif
#ifdef ROBIN_HOOD_LOG_ENABLED
#include <iostream>
#define ROBIN_HOOD_LOG(...) std::cout << __FUNCTION__ << "@" << __LINE__ << ": " << __VA_ARGS__ << std::endl;
#else
#define ROBIN_HOOD_LOG(x)
#endif
#ifdef ROBIN_HOOD_TRACE_ENABLED
#include <iostream>
#define ROBIN_HOOD_TRACE(...) std::cout << __FUNCTION__ << "@" << __LINE__ << ": " << __VA_ARGS__ << std::endl;
#else
#define ROBIN_HOOD_TRACE(x)
#endif
#ifdef ROBIN_HOOD_COUNT_ENABLED
#include <iostream>
#define ROBIN_HOOD_COUNT(x) ++counts().x;
namespace robin_hood { struct Counts { uint64_t shiftUp{}; uint64_t shiftDown{}; }; inline std::ostream& operator<<(std::ostream& os, Counts const& c) { return os << c.shiftUp << " shiftUp" << std::endl << c.shiftDown << " shiftDown" << std::endl; } static Counts& counts() { static Counts counts{}; return counts; } }
#else
#define ROBIN_HOOD_COUNT(x)
#endif
#define ROBIN_HOOD(x) ROBIN_HOOD_PRIVATE_DEFINITION_##x()
#define ROBIN_HOOD_UNUSED(identifier)
#if SIZE_MAX == UINT32_MAX
#define ROBIN_HOOD_PRIVATE_DEFINITION_BITNESS() 32
#elif SIZE_MAX == UINT64_MAX
#define ROBIN_HOOD_PRIVATE_DEFINITION_BITNESS() 64
#else
#error Unsupported bitness
#endif
#ifdef _MSC_VER
#define ROBIN_HOOD_PRIVATE_DEFINITION_LITTLE_ENDIAN() 1
#define ROBIN_HOOD_PRIVATE_DEFINITION_BIG_ENDIAN() 0
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_LITTLE_ENDIAN() (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define ROBIN_HOOD_PRIVATE_DEFINITION_BIG_ENDIAN() (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#endif
#ifdef _MSC_VER
#define ROBIN_HOOD_PRIVATE_DEFINITION_NOINLINE() __declspec(noinline)
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_NOINLINE() __attribute__((noinline))
#endif
#if !defined(__cpp_exceptions) && !defined(__EXCEPTIONS) && !defined(_CPPUNWIND)
#define ROBIN_HOOD_PRIVATE_DEFINITION_HAS_EXCEPTIONS() 0
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_HAS_EXCEPTIONS() 1
#endif
#if !defined(ROBIN_HOOD_DISABLE_INTRINSICS)
#ifdef _MSC_VER
#if ROBIN_HOOD(BITNESS) == 32
#define ROBIN_HOOD_PRIVATE_DEFINITION_BITSCANFORWARD() _BitScanForward
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_BITSCANFORWARD() _BitScanForward64
#endif
#include <intrin.h>
#pragma intrinsic(ROBIN_HOOD(BITSCANFORWARD))
#define ROBIN_HOOD_COUNT_TRAILING_ZEROES(x) [](size_t mask) noexcept -> int { unsigned long index; return ROBIN_HOOD(BITSCANFORWARD)(&index, mask) ? static_cast<int>(index) : ROBIN_HOOD(BITNESS); }(x)
#else
#if ROBIN_HOOD(BITNESS) == 32
#define ROBIN_HOOD_PRIVATE_DEFINITION_CTZ() __builtin_ctzl
#define ROBIN_HOOD_PRIVATE_DEFINITION_CLZ() __builtin_clzl
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_CTZ() __builtin_ctzll
#define ROBIN_HOOD_PRIVATE_DEFINITION_CLZ() __builtin_clzll
#endif
#define ROBIN_HOOD_COUNT_LEADING_ZEROES(x) ((x) ? ROBIN_HOOD(CLZ)(x) : ROBIN_HOOD(BITNESS))
#define ROBIN_HOOD_COUNT_TRAILING_ZEROES(x) ((x) ? ROBIN_HOOD(CTZ)(x) : ROBIN_HOOD(BITNESS))
#endif
#endif
#ifndef __has_cpp_attribute
#define __has_cpp_attribute(x) 0
#endif
#if __has_cpp_attribute(clang::fallthrough)
#define ROBIN_HOOD_PRIVATE_DEFINITION_FALLTHROUGH() [[clang::fallthrough]]
#elif __has_cpp_attribute(gnu::fallthrough)
#define ROBIN_HOOD_PRIVATE_DEFINITION_FALLTHROUGH() [[gnu::fallthrough]]
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_FALLTHROUGH()
#endif
#ifdef _MSC_VER
#define ROBIN_HOOD_LIKELY(condition) condition
#define ROBIN_HOOD_UNLIKELY(condition) condition
#else
#define ROBIN_HOOD_LIKELY(condition) __builtin_expect(condition, 1)
#define ROBIN_HOOD_UNLIKELY(condition) __builtin_expect(condition, 0)
#endif
#ifdef _MSC_VER
#ifdef _NATIVE_WCHAR_T_DEFINED
#define ROBIN_HOOD_PRIVATE_DEFINITION_HAS_NATIVE_WCHART() 1
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_HAS_NATIVE_WCHART() 0
#endif
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_HAS_NATIVE_WCHART() 1
#endif
#ifdef _MSC_VER
#if _MSC_VER <= 1900
#define ROBIN_HOOD_PRIVATE_DEFINITION_BROKEN_CONSTEXPR() 1
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_BROKEN_CONSTEXPR() 0
#endif
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_BROKEN_CONSTEXPR() 0
#endif
#if defined(__GNUC__) && __GNUC__ < 5
#define ROBIN_HOOD_IS_TRIVIALLY_COPYABLE(...) __has_trivial_copy(__VA_ARGS__)
#else
#define ROBIN_HOOD_IS_TRIVIALLY_COPYABLE(...) std::is_trivially_copyable<__VA_ARGS__>::value
#endif
#define ROBIN_HOOD_PRIVATE_DEFINITION_CXX() __cplusplus
#define ROBIN_HOOD_PRIVATE_DEFINITION_CXX98() 199711L
#define ROBIN_HOOD_PRIVATE_DEFINITION_CXX11() 201103L
#define ROBIN_HOOD_PRIVATE_DEFINITION_CXX14() 201402L
#define ROBIN_HOOD_PRIVATE_DEFINITION_CXX17() 201703L
#if ROBIN_HOOD(CXX) >= ROBIN_HOOD(CXX17)
#define ROBIN_HOOD_PRIVATE_DEFINITION_NODISCARD() [[nodiscard]]
#else
#define ROBIN_HOOD_PRIVATE_DEFINITION_NODISCARD()
#endif
namespace robin_hood {
#if ROBIN_HOOD(CXX) >= ROBIN_HOOD(CXX14)
#define ROBIN_HOOD_STD std
#else
namespace ROBIN_HOOD_STD { template <class T> struct alignment_of : std::integral_constant< std::size_t, alignof(typename std::remove_all_extents<T>::type)> {}; template <class T, T... Ints> class integer_sequence { public: using value_type = T; static_assert(std::is_integral<value_type>::value, "not integral type"); static constexpr std::size_t size() noexcept { return sizeof...(Ints); } }; template <std::size_t... Inds> using index_sequence = integer_sequence<std::size_t, Inds...>; namespace detail_ { template <class T, T Begin, T End, bool> struct IntSeqImpl { using TValue = T; static_assert(std::is_integral<TValue>::value, "not integral type"); static_assert(Begin >= 0 && Begin < End, "unexpected argument (Begin<0 || Begin<=End)"); template <class, class> struct IntSeqCombiner; template <TValue... Inds0, TValue... Inds1> struct IntSeqCombiner<integer_sequence<TValue, Inds0...>, integer_sequence<TValue, Inds1...>> { using TResult = integer_sequence<TValue, Inds0..., Inds1...>; }; using TResult = typename IntSeqCombiner< typename IntSeqImpl<TValue, Begin, Begin + (End - Begin) / 2, (End - Begin) / 2 == 1>::TResult, typename IntSeqImpl<TValue, Begin + (End - Begin) / 2, End, (End - Begin + 1) / 2 == 1>::TResult>::TResult; }; template <class T, T Begin> struct IntSeqImpl<T, Begin, Begin, false> { using TValue = T; static_assert(std::is_integral<TValue>::value, "not integral type"); static_assert(Begin >= 0, "unexpected argument (Begin<0)"); using TResult = integer_sequence<TValue>; }; template <class T, T Begin, T End> struct IntSeqImpl<T, Begin, End, true> { using TValue = T; static_assert(std::is_integral<TValue>::value, "not integral type"); static_assert(Begin >= 0, "unexpected argument (Begin<0)"); using TResult = integer_sequence<TValue, Begin>; }; } template <class T, T N> using make_integer_sequence = typename detail_::IntSeqImpl<T, 0, N, (N - 0) == 1>::TResult; template <std::size_t N> using make_index_sequence = make_integer_sequence<std::size_t, N>; template <class... T> using index_sequence_for = make_index_sequence<sizeof...(T)>; }
#endif
namespace detail {
#if ROBIN_HOOD(BITNESS) == 64
using SizeT = uint64_t;
#else
using SizeT = uint32_t;
#endif
template <typename T> T rotr(T x, unsigned k) { return (x >> k) | (x << (8U * sizeof(T) - k)); } template <typename T> inline T reinterpret_cast_no_cast_align_warning(void* ptr) noexcept { return reinterpret_cast<T>(ptr); } template <typename T> inline T reinterpret_cast_no_cast_align_warning(void const* ptr) noexcept { return reinterpret_cast<T>(ptr); } template <typename E, typename... Args> [[noreturn]] ROBIN_HOOD(NOINLINE)
#if ROBIN_HOOD(HAS_EXCEPTIONS)
void doThrow(Args&&... args) { throw E(std::forward<Args>(args)...); }
#else
void doThrow(Args&&... ROBIN_HOOD_UNUSED(args) ) { abort(); }
#endif
template <typename E, typename T, typename... Args> T* assertNotNull(T* t, Args&&... args) { if (ROBIN_HOOD_UNLIKELY(nullptr == t)) { doThrow<E>(std::forward<Args>(args)...); } return t; } template <typename T> inline T unaligned_load(void const* ptr) noexcept { T t; std::memcpy(&t, ptr, sizeof(T)); return t; } template <typename T, size_t MinNumAllocs = 4, size_t MaxNumAllocs = 256> class BulkPoolAllocator { public: BulkPoolAllocator() noexcept = default; BulkPoolAllocator( const BulkPoolAllocator& ROBIN_HOOD_UNUSED(o) ) noexcept : mHead(nullptr), mListForFree(nullptr) {} BulkPoolAllocator(BulkPoolAllocator&& o) noexcept : mHead(o.mHead), mListForFree(o.mListForFree) { o.mListForFree = nullptr; o.mHead = nullptr; } BulkPoolAllocator& operator=(BulkPoolAllocator&& o) noexcept { reset(); mHead = o.mHead; mListForFree = o.mListForFree; o.mListForFree = nullptr; o.mHead = nullptr; return *this; } BulkPoolAllocator& operator=( const BulkPoolAllocator& ROBIN_HOOD_UNUSED(o) ) noexcept { return *this; } ~BulkPoolAllocator() noexcept { reset(); } void reset() noexcept { while (mListForFree) { T* tmp = *mListForFree; ROBIN_HOOD_LOG("std::free") std::free(mListForFree); mListForFree = reinterpret_cast_no_cast_align_warning<T**>(tmp); } mHead = nullptr; } T* allocate() { T* tmp = mHead; if (!tmp) { tmp = performAllocation(); } mHead = *reinterpret_cast_no_cast_align_warning<T**>(tmp); return tmp; } void deallocate(T* obj) noexcept { *reinterpret_cast_no_cast_align_warning<T**>(obj) = mHead; mHead = obj; } void addOrFree(void* ptr, const size_t numBytes) noexcept { if (numBytes < ALIGNMENT + ALIGNED_SIZE) { ROBIN_HOOD_LOG("std::free") std::free(ptr); } else { ROBIN_HOOD_LOG("add to buffer") add(ptr, numBytes); } } void swap(BulkPoolAllocator<T, MinNumAllocs, MaxNumAllocs>& other) noexcept { using std::swap; swap(mHead, other.mHead); swap(mListForFree, other.mListForFree); } private: ROBIN_HOOD(NODISCARD) size_t calcNumElementsToAlloc() const noexcept { auto tmp = mListForFree; size_t numAllocs = MinNumAllocs; while (numAllocs * 2 <= MaxNumAllocs && tmp) { auto x = reinterpret_cast<T***>(tmp); tmp = *x; numAllocs *= 2; } return numAllocs; } void add(void* ptr, const size_t numBytes) noexcept { const size_t numElements = (numBytes - ALIGNMENT) / ALIGNED_SIZE; auto data = reinterpret_cast<T**>(ptr); auto x = reinterpret_cast<T***>(data); *x = mListForFree; mListForFree = data; auto* const headT = reinterpret_cast_no_cast_align_warning<T*>( reinterpret_cast<char*>(ptr) + ALIGNMENT); auto* const head = reinterpret_cast<char*>(headT); for (size_t i = 0; i < numElements; ++i) { *reinterpret_cast_no_cast_align_warning<char**>( head + i * ALIGNED_SIZE) = head + (i + 1) * ALIGNED_SIZE; } *reinterpret_cast_no_cast_align_warning<T**>( head + (numElements - 1) * ALIGNED_SIZE) = mHead; mHead = headT; } ROBIN_HOOD(NOINLINE) T* performAllocation() { size_t const numElementsToAlloc = calcNumElementsToAlloc(); size_t const bytes = ALIGNMENT + ALIGNED_SIZE * numElementsToAlloc; ROBIN_HOOD_LOG("std::malloc " << bytes << " = " << ALIGNMENT << " + " << ALIGNED_SIZE << " * " << numElementsToAlloc) add(assertNotNull<std::bad_alloc>(std::malloc(bytes)), bytes); return mHead; }
#if ROBIN_HOOD(CXX) >= ROBIN_HOOD(CXX14)
static constexpr size_t ALIGNMENT = (std::max)(std::alignment_of<T>::value, std::alignment_of<T*>::value);
#else
static const size_t ALIGNMENT = (ROBIN_HOOD_STD::alignment_of<T>::value > ROBIN_HOOD_STD::alignment_of<T*>::value) ? ROBIN_HOOD_STD::alignment_of<T>::value : +ROBIN_HOOD_STD::alignment_of<T*>::value;
#endif
static constexpr size_t ALIGNED_SIZE = ((sizeof(T) - 1) / ALIGNMENT + 1) * ALIGNMENT; static_assert(MinNumAllocs >= 1, "MinNumAllocs"); static_assert(MaxNumAllocs >= MinNumAllocs, "MaxNumAllocs"); static_assert(ALIGNED_SIZE >= sizeof(T*), "ALIGNED_SIZE"); static_assert(0 == (ALIGNED_SIZE % sizeof(T*)), "ALIGNED_SIZE mod"); static_assert(ALIGNMENT >= sizeof(T*), "ALIGNMENT"); T* mHead{nullptr}; T** mListForFree{nullptr}; }; template <typename T, size_t MinSize, size_t MaxSize, bool IsFlat> struct NodeAllocator; template <typename T, size_t MinSize, size_t MaxSize> struct NodeAllocator<T, MinSize, MaxSize, true> { void addOrFree(void* ptr, size_t ROBIN_HOOD_UNUSED(numBytes) ) noexcept { ROBIN_HOOD_LOG("std::free") std::free(ptr); } }; template <typename T, size_t MinSize, size_t MaxSize> struct NodeAllocator<T, MinSize, MaxSize, false> : public BulkPoolAllocator<T, MinSize, MaxSize> {}; namespace swappable {
#if ROBIN_HOOD(CXX) < ROBIN_HOOD(CXX17)
using std::swap; template <typename T> struct nothrow { static const bool value = noexcept(swap(std::declval<T&>(), std::declval<T&>())); };
#else
template <typename T> struct nothrow { static const bool value = std::is_nothrow_swappable<T>::value; };
#endif
} } struct is_transparent_tag {}; template <typename T1, typename T2> struct pair { using first_type = T1; using second_type = T2; template <typename U1 = T1, typename U2 = T2, typename = typename std::enable_if< std::is_default_constructible<U1>::value && std::is_default_constructible<U2>::value>::type> constexpr pair() noexcept(noexcept(U1()) && noexcept(U2())) : first(), second() {} explicit constexpr pair(std::pair<T1, T2> const& o) noexcept(noexcept(T1( std::declval<T1 const&>())) && noexcept(T2(std::declval<T2 const&>()))) : first(o.first), second(o.second) {} explicit constexpr pair(std::pair<T1, T2>&& o) noexcept( noexcept(T1(std::move(std::declval<T1&&>()))) && noexcept( T2(std::move(std::declval<T2&&>())))) : first(std::move(o.first)), second(std::move(o.second)) {} constexpr pair(T1&& a, T2&& b) noexcept( noexcept(T1(std::move(std::declval<T1&&>()))) && noexcept( T2(std::move(std::declval<T2&&>())))) : first(std::move(a)), second(std::move(b)) {} template <typename U1, typename U2> constexpr pair(U1&& a, U2&& b) noexcept( noexcept(T1(std::forward<U1>(std::declval<U1&&>()))) && noexcept( T2(std::forward<U2>(std::declval<U2&&>())))) : first(std::forward<U1>(a)), second(std::forward<U2>(b)) {} template <typename... U1, typename... U2>
#if !ROBIN_HOOD(BROKEN_CONSTEXPR)
constexpr
#endif
pair(std::piecewise_construct_t , std::tuple<U1...> a, std::tuple<U2...> b) noexcept(noexcept(pair(std::declval<std::tuple<U1...>&>(), std::declval<std::tuple<U2...>&>(), ROBIN_HOOD_STD::index_sequence_for< U1...>(), ROBIN_HOOD_STD::index_sequence_for< U2...>()))) : pair(a, b, ROBIN_HOOD_STD::index_sequence_for<U1...>(), ROBIN_HOOD_STD::index_sequence_for<U2...>()) { } template <typename... U1, size_t... I1, typename... U2, size_t... I2> pair(std::tuple<U1...>& a, std::tuple<U2...>& b, ROBIN_HOOD_STD::index_sequence<I1...> , ROBIN_HOOD_STD::index_sequence<I2...> ) noexcept( noexcept(T1(std::forward<U1>(std::get<I1>( std::declval<std::tuple< U1...>&>()))...)) && noexcept(T2(std:: forward<U2>(std::get<I2>( std::declval<std::tuple< U2...>&>()))...))) : first(std::forward<U1>(std::get<I1>(a))...), second(std::forward<U2>(std::get<I2>(b))...) { (void)a; (void)b; } void swap(pair<T1, T2>& o) noexcept((detail::swappable::nothrow<T1>::value) && (detail::swappable::nothrow<T2>::value)) { using std::swap; swap(first, o.first); swap(second, o.second); } T1 first; T2 second; }; template <typename A, typename B> inline void swap(pair<A, B>& a, pair<A, B>& b) noexcept( noexcept(std::declval<pair<A, B>&>().swap(std::declval<pair<A, B>&>()))) { a.swap(b); } template <typename A, typename B> inline constexpr bool operator==(pair<A, B> const& x, pair<A, B> const& y) { return (x.first == y.first) && (x.second == y.second); } template <typename A, typename B> inline constexpr bool operator!=(pair<A, B> const& x, pair<A, B> const& y) { return !(x == y); } template <typename A, typename B> inline constexpr bool operator<(pair<A, B> const& x, pair<A, B> const& y) noexcept( noexcept(std::declval<A const&>() < std::declval<A const&>()) && noexcept(std::declval<B const&>() < std::declval<B const&>())) { return x.first < y.first || (!(y.first < x.first) && x.second < y.second); } template <typename A, typename B> inline constexpr bool operator>(pair<A, B> const& x, pair<A, B> const& y) { return y < x; } template <typename A, typename B> inline constexpr bool operator<=(pair<A, B> const& x, pair<A, B> const& y) { return !(x > y); } template <typename A, typename B> inline constexpr bool operator>=(pair<A, B> const& x, pair<A, B> const& y) { return !(x < y); } inline size_t hash_bytes(void const* ptr, size_t len) noexcept { static constexpr uint64_t m = UINT64_C(0xc6a4a7935bd1e995); static constexpr uint64_t seed = UINT64_C(0xe17a1465); static constexpr unsigned int r = 47; auto const* const data64 = static_cast<uint64_t const*>(ptr); uint64_t h = seed ^ (len * m); size_t const n_blocks = len / 8; for (size_t i = 0; i < n_blocks; ++i) { auto k = detail::unaligned_load<uint64_t>(data64 + i); k *= m; k ^= k >> r; k *= m; h ^= k; h *= m; } auto const* const data8 = reinterpret_cast<uint8_t const*>(data64 + n_blocks); switch (len & 7U) { case 7: h ^= static_cast<uint64_t>(data8[6]) << 48U; ROBIN_HOOD(FALLTHROUGH); case 6: h ^= static_cast<uint64_t>(data8[5]) << 40U; ROBIN_HOOD(FALLTHROUGH); case 5: h ^= static_cast<uint64_t>(data8[4]) << 32U; ROBIN_HOOD(FALLTHROUGH); case 4: h ^= static_cast<uint64_t>(data8[3]) << 24U; ROBIN_HOOD(FALLTHROUGH); case 3: h ^= static_cast<uint64_t>(data8[2]) << 16U; ROBIN_HOOD(FALLTHROUGH); case 2: h ^= static_cast<uint64_t>(data8[1]) << 8U; ROBIN_HOOD(FALLTHROUGH); case 1: h ^= static_cast<uint64_t>(data8[0]); h *= m; ROBIN_HOOD(FALLTHROUGH); default: break; } h ^= h >> r; return static_cast<size_t>(h); } inline size_t hash_int(uint64_t x) noexcept { x ^= x >> 33U; x *= UINT64_C(0xff51afd7ed558ccd); x ^= x >> 33U; return static_cast<size_t>(x); } template <typename T, typename Enable = void> struct hash : public std::hash<T> { size_t operator()(T const& obj) const noexcept(noexcept( std::declval<std::hash<T>>().operator()(std::declval<T const&>()))) { auto result = std::hash<T>::operator()(obj); return hash_int(static_cast<detail::SizeT>(result)); } }; template <typename CharT> struct hash<std::basic_string<CharT>> { size_t operator()(std::basic_string<CharT> const& str) const noexcept { return hash_bytes(str.data(), sizeof(CharT) * str.size()); } };
#if ROBIN_HOOD(CXX) >= ROBIN_HOOD(CXX17)
template <typename CharT> struct hash<std::basic_string_view<CharT>> { size_t operator()(std::basic_string_view<CharT> const& sv) const noexcept { return hash_bytes(sv.data(), sizeof(CharT) * sv.size()); } };
#endif
template <class T> struct hash<T*> { size_t operator()(T* ptr) const noexcept { return hash_int(reinterpret_cast<detail::SizeT>(ptr)); } }; template <class T> struct hash<std::unique_ptr<T>> { size_t operator()(std::unique_ptr<T> const& ptr) const noexcept { return hash_int(reinterpret_cast<detail::SizeT>(ptr.get())); } }; template <class T> struct hash<std::shared_ptr<T>> { size_t operator()(std::shared_ptr<T> const& ptr) const noexcept { return hash_int(reinterpret_cast<detail::SizeT>(ptr.get())); } }; template <typename Enum> struct hash<Enum, typename std::enable_if<std::is_enum<Enum>::value>::type> { size_t operator()(Enum e) const noexcept { using Underlying = typename std::underlying_type<Enum>::type; return hash<Underlying>{}(static_cast<Underlying>(e)); } };
#define ROBIN_HOOD_HASH_INT(T) template <> struct hash<T> { size_t operator()(T const& obj) const noexcept { return hash_int(static_cast<uint64_t>(obj)); } }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
ROBIN_HOOD_HASH_INT(bool); ROBIN_HOOD_HASH_INT(char); ROBIN_HOOD_HASH_INT(signed char); ROBIN_HOOD_HASH_INT(unsigned char); ROBIN_HOOD_HASH_INT(char16_t); ROBIN_HOOD_HASH_INT(char32_t);
#if ROBIN_HOOD(HAS_NATIVE_WCHART)
ROBIN_HOOD_HASH_INT(wchar_t);
#endif
ROBIN_HOOD_HASH_INT(short); ROBIN_HOOD_HASH_INT(unsigned short); ROBIN_HOOD_HASH_INT(int); ROBIN_HOOD_HASH_INT(unsigned int); ROBIN_HOOD_HASH_INT(long); ROBIN_HOOD_HASH_INT(long long); ROBIN_HOOD_HASH_INT(unsigned long); ROBIN_HOOD_HASH_INT(unsigned long long);
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
namespace detail { template <typename T> struct void_type { using type = void; }; template <typename T, typename = void> struct has_is_transparent : public std::false_type {}; template <typename T> struct has_is_transparent<T, typename void_type<typename T::is_transparent>::type> : public std::true_type {}; template <typename T> struct WrapHash : public T { WrapHash() = default; explicit WrapHash(T const& o) noexcept( noexcept(T(std::declval<T const&>()))) : T(o) {} }; template <typename T> struct WrapKeyEqual : public T { WrapKeyEqual() = default; explicit WrapKeyEqual(T const& o) noexcept( noexcept(T(std::declval<T const&>()))) : T(o) {} }; template <bool IsFlat, size_t MaxLoadFactor100, typename Key, typename T, typename Hash, typename KeyEqual> class Table : public WrapHash<Hash>, public WrapKeyEqual<KeyEqual>, detail::NodeAllocator< typename std::conditional< std::is_void<T>::value, Key, robin_hood::pair< typename std::conditional<IsFlat, Key, Key const>::type, T>>::type, 4, 16384, IsFlat> { public: static constexpr bool is_flat = IsFlat; static constexpr bool is_map = !std::is_void<T>::value; static constexpr bool is_set = !is_map; static constexpr bool is_transparent = has_is_transparent<Hash>::value && has_is_transparent<KeyEqual>::value; using key_type = Key; using mapped_type = T; using value_type = typename std::conditional< is_set, Key, robin_hood::pair< typename std::conditional<is_flat, Key, Key const>::type, T>>::type; using size_type = size_t; using hasher = Hash; using key_equal = KeyEqual; using Self = Table<IsFlat, MaxLoadFactor100, key_type, mapped_type, hasher, key_equal>; private: static_assert(MaxLoadFactor100 > 10 && MaxLoadFactor100 < 100, "MaxLoadFactor100 needs to be >10 && < 100"); using WHash = WrapHash<Hash>; using WKeyEqual = WrapKeyEqual<KeyEqual>; static constexpr size_t InitialNumElements = sizeof(uint64_t); static constexpr uint32_t InitialInfoNumBits = 5; static constexpr uint8_t InitialInfoInc = 1U << InitialInfoNumBits; static constexpr size_t InfoMask = InitialInfoInc - 1U; static constexpr uint8_t InitialInfoHashShift = 0; using DataPool = detail::NodeAllocator<value_type, 4, 16384, IsFlat>; using InfoType = uint32_t; template <typename M, bool> class DataNode {}; template <typename M> class DataNode<M, true> final { public: template <typename... Args> explicit DataNode( M& ROBIN_HOOD_UNUSED(map) , Args&&... args) noexcept(noexcept(value_type(std:: forward<Args>( args)...))) : mData(std::forward<Args>(args)...) {} DataNode( M& ROBIN_HOOD_UNUSED(map) , DataNode<M, true>&& n) noexcept(std::is_nothrow_move_constructible<value_type>:: value) : mData(std::move(n.mData)) {} void destroy(M& ROBIN_HOOD_UNUSED(map) ) noexcept {} void destroyDoNotDeallocate() noexcept {} value_type const* operator->() const noexcept { return &mData; } value_type* operator->() noexcept { return &mData; } const value_type& operator*() const noexcept { return mData; } value_type& operator*() noexcept { return mData; } template <typename VT = value_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_map, typename VT::first_type&>::type getFirst() noexcept { return mData.first; } template <typename VT = value_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_set, VT&>::type getFirst() noexcept { return mData; } template <typename VT = value_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_map, typename VT::first_type const&>::type getFirst() const noexcept { return mData.first; } template <typename VT = value_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_set, VT const&>::type getFirst() const noexcept { return mData; } template <typename MT = mapped_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_map, MT&>::type getSecond() noexcept { return mData.second; } template <typename MT = mapped_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_set, MT const&>::type getSecond() const noexcept { return mData.second; } void swap(DataNode<M, true>& o) noexcept(noexcept( std::declval<value_type>().swap(std::declval<value_type>()))) { mData.swap(o.mData); } private: value_type mData; }; template <typename M> class DataNode<M, false> { public: template <typename... Args> explicit DataNode(M& map, Args&&... args) : mData(map.allocate()) { ::new (static_cast<void*>(mData)) value_type(std::forward<Args>(args)...); } DataNode(M& ROBIN_HOOD_UNUSED(map) , DataNode<M, false>&& n) noexcept : mData(std::move(n.mData)) {} void destroy(M& map) noexcept { mData->~value_type(); map.deallocate(mData); } void destroyDoNotDeallocate() noexcept { mData->~value_type(); } value_type const* operator->() const noexcept { return mData; } value_type* operator->() noexcept { return mData; } const value_type& operator*() const { return *mData; } value_type& operator*() { return *mData; } template <typename VT = value_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_map, typename VT::first_type&>::type getFirst() noexcept { return mData->first; } template <typename VT = value_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_set, VT&>::type getFirst() noexcept { return *mData; } template <typename VT = value_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_map, typename VT::first_type const&>::type getFirst() const noexcept { return mData->first; } template <typename VT = value_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_set, VT const&>::type getFirst() const noexcept { return *mData; } template <typename MT = mapped_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_map, MT&>::type getSecond() noexcept { return mData->second; } template <typename MT = mapped_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<is_map, MT const&>::type getSecond() const noexcept { return mData->second; } void swap(DataNode<M, false>& o) noexcept { using std::swap; swap(mData, o.mData); } private: value_type* mData; }; using Node = DataNode<Self, IsFlat>; ROBIN_HOOD(NODISCARD) key_type const& getFirstConst(Node const& n) const noexcept { return n.getFirst(); } ROBIN_HOOD(NODISCARD) key_type const& getFirstConst(key_type const& k) const noexcept { return k; } template <typename Q = mapped_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<!std::is_void<Q>::value, key_type const&>::type getFirstConst(value_type const& vt) const noexcept { return vt.first; } template <typename M, bool UseMemcpy> struct Cloner; template <typename M> struct Cloner<M, true> { void operator()(M const& source, M& target) const { auto const* const src = reinterpret_cast<char const*>(source.mKeyVals); auto* tgt = reinterpret_cast<char*>(target.mKeyVals); auto const numElementsWithBuffer = target.calcNumElementsWithBuffer(target.mMask + 1); std::copy(src, src + target.calcNumBytesTotal(numElementsWithBuffer), tgt); } }; template <typename M> struct Cloner<M, false> { void operator()(M const& s, M& t) const { auto const numElementsWithBuffer = t.calcNumElementsWithBuffer(t.mMask + 1); std::copy(s.mInfo, s.mInfo + t.calcNumBytesInfo(numElementsWithBuffer), t.mInfo); for (size_t i = 0; i < numElementsWithBuffer; ++i) { if (t.mInfo[i]) { ::new (static_cast<void*>(t.mKeyVals + i)) Node(t, *s.mKeyVals[i]); } } } }; template <typename M, bool IsFlatAndTrivial> struct Destroyer {}; template <typename M> struct Destroyer<M, true> { void nodes(M& m) const noexcept { m.mNumElements = 0; } void nodesDoNotDeallocate(M& m) const noexcept { m.mNumElements = 0; } }; template <typename M> struct Destroyer<M, false> { void nodes(M& m) const noexcept { m.mNumElements = 0; auto const numElementsWithBuffer = m.calcNumElementsWithBuffer(m.mMask + 1); for (size_t idx = 0; idx < numElementsWithBuffer; ++idx) { if (0 != m.mInfo[idx]) { Node& n = m.mKeyVals[idx]; n.destroy(m); n.~Node(); } } } void nodesDoNotDeallocate(M& m) const noexcept { m.mNumElements = 0; auto const numElementsWithBuffer = m.calcNumElementsWithBuffer(m.mMask + 1); for (size_t idx = 0; idx < numElementsWithBuffer; ++idx) { if (0 != m.mInfo[idx]) { Node& n = m.mKeyVals[idx]; n.destroyDoNotDeallocate(); n.~Node(); } } } }; struct fast_forward_tag {}; template <bool IsConst> class Iter { private: using NodePtr = typename std::conditional<IsConst, Node const*, Node*>::type; public: using difference_type = std::ptrdiff_t; using value_type = typename Self::value_type; using reference = typename std::conditional<IsConst, value_type const&, value_type&>::type; using pointer = typename std::conditional<IsConst, value_type const*, value_type*>::type; using iterator_category = std::forward_iterator_tag; Iter() = default; template <bool OtherIsConst, typename = typename std::enable_if< IsConst && !OtherIsConst>::type> Iter(Iter<OtherIsConst> const& other) noexcept : mKeyVals(other.mKeyVals), mInfo(other.mInfo) {} Iter(NodePtr valPtr, uint8_t const* infoPtr) noexcept : mKeyVals(valPtr), mInfo(infoPtr) {} Iter(NodePtr valPtr, uint8_t const* infoPtr, fast_forward_tag ROBIN_HOOD_UNUSED(tag) ) noexcept : mKeyVals(valPtr), mInfo(infoPtr) { fastForward(); } template <bool OtherIsConst, typename = typename std::enable_if< IsConst && !OtherIsConst>::type> Iter& operator=(Iter<OtherIsConst> const& other) noexcept { mKeyVals = other.mKeyVals; mInfo = other.mInfo; return *this; } Iter& operator++() noexcept { mInfo++; mKeyVals++; fastForward(); return *this; } Iter operator++(int) noexcept { Iter tmp = *this; ++(*this); return tmp; } reference operator*() const { return **mKeyVals; } pointer operator->() const { return &**mKeyVals; } template <bool O> bool operator==(Iter<O> const& o) const noexcept { return mKeyVals == o.mKeyVals; } template <bool O> bool operator!=(Iter<O> const& o) const noexcept { return mKeyVals != o.mKeyVals; } private: void fastForward() noexcept { size_t n = 0; while (0U == (n = detail::unaligned_load<size_t>(mInfo))) { mInfo += sizeof(size_t); mKeyVals += sizeof(size_t); }
#if defined(ROBIN_HOOD_DISABLE_INTRINSICS)
if (ROBIN_HOOD_UNLIKELY(0U == detail::unaligned_load<uint32_t>(mInfo))) { mInfo += 4; mKeyVals += 4; } if (ROBIN_HOOD_UNLIKELY(0U == detail::unaligned_load<uint16_t>(mInfo))) { mInfo += 2; mKeyVals += 2; } if (ROBIN_HOOD_UNLIKELY(0U == *mInfo)) { mInfo += 1; mKeyVals += 1; }
#else
#if ROBIN_HOOD(LITTLE_ENDIAN)
auto inc = ROBIN_HOOD_COUNT_TRAILING_ZEROES(n) / 8;
#else
auto inc = ROBIN_HOOD_COUNT_LEADING_ZEROES(n) / 8;
#endif
mInfo += inc; mKeyVals += inc;
#endif
} friend class Table<IsFlat, MaxLoadFactor100, key_type, mapped_type, hasher, key_equal>; NodePtr mKeyVals{nullptr}; uint8_t const* mInfo{nullptr}; }; template <typename HashKey> void keyToIdx(HashKey&& key, size_t* idx, InfoType* info) const { auto h = static_cast<uint64_t>(WHash::operator()(key)); h *= mHashMultiplier; h ^= h >> 33U; *info = mInfoInc + static_cast<InfoType>((h & InfoMask) >> mInfoHashShift); *idx = (static_cast<size_t>(h) >> InitialInfoNumBits) & mMask; } void next(InfoType* info, size_t* idx) const noexcept { *idx = *idx + 1; *info += mInfoInc; } void nextWhileLess(InfoType* info, size_t* idx) const noexcept { while (*info < mInfo[*idx]) { next(info, idx); } } void shiftUp(size_t startIdx, size_t const insertion_idx) noexcept( std::is_nothrow_move_assignable<Node>::value) { auto idx = startIdx; ::new (static_cast<void*>(mKeyVals + idx)) Node(std::move(mKeyVals[idx - 1])); while (--idx != insertion_idx) { mKeyVals[idx] = std::move(mKeyVals[idx - 1]); } idx = startIdx; while (idx != insertion_idx) { ROBIN_HOOD_COUNT(shiftUp) mInfo[idx] = static_cast<uint8_t>(mInfo[idx - 1] + mInfoInc); if (ROBIN_HOOD_UNLIKELY(mInfo[idx] + mInfoInc > 0xFF)) { mMaxNumElementsAllowed = 0; } --idx; } } void shiftDown(size_t idx) noexcept( std::is_nothrow_move_assignable<Node>::value) { mKeyVals[idx].destroy(*this); while (mInfo[idx + 1] >= 2 * mInfoInc) { ROBIN_HOOD_COUNT(shiftDown) mInfo[idx] = static_cast<uint8_t>(mInfo[idx + 1] - mInfoInc); mKeyVals[idx] = std::move(mKeyVals[idx + 1]); ++idx; } mInfo[idx] = 0; mKeyVals[idx].~Node(); } template <typename Other> ROBIN_HOOD(NODISCARD) size_t findIdx(Other const& key) const { size_t idx{}; InfoType info{}; keyToIdx(key, &idx, &info); do { if (info == mInfo[idx] && ROBIN_HOOD_LIKELY(WKeyEqual::operator()( key, mKeyVals[idx].getFirst()))) { return idx; } next(&info, &idx); if (info == mInfo[idx] && ROBIN_HOOD_LIKELY(WKeyEqual::operator()( key, mKeyVals[idx].getFirst()))) { return idx; } next(&info, &idx); } while (info <= mInfo[idx]); return mMask == 0 ? 0 : static_cast<size_t>(std::distance( mKeyVals, reinterpret_cast_no_cast_align_warning<Node*>(mInfo))); } void cloneData(const Table& o) { Cloner<Table, IsFlat && ROBIN_HOOD_IS_TRIVIALLY_COPYABLE(Node)>()( o, *this); } void insert_move(Node&& keyval) { if (0 == mMaxNumElementsAllowed && !try_increase_info()) { throwOverflowError(); } size_t idx{}; InfoType info{}; keyToIdx(keyval.getFirst(), &idx, &info); while (info <= mInfo[idx]) { idx = idx + 1; info += mInfoInc; } auto const insertion_idx = idx; auto const insertion_info = static_cast<uint8_t>(info); if (ROBIN_HOOD_UNLIKELY(insertion_info + mInfoInc > 0xFF)) { mMaxNumElementsAllowed = 0; } while (0 != mInfo[idx]) { next(&info, &idx); } auto& l = mKeyVals[insertion_idx]; if (idx == insertion_idx) { ::new (static_cast<void*>(&l)) Node(std::move(keyval)); } else { shiftUp(idx, insertion_idx); l = std::move(keyval); } mInfo[insertion_idx] = insertion_info; ++mNumElements; } public: using iterator = Iter<false>; using const_iterator = Iter<true>; Table() noexcept(noexcept(Hash()) && noexcept(KeyEqual())) : WHash(), WKeyEqual() { ROBIN_HOOD_TRACE(this) } explicit Table( size_t ROBIN_HOOD_UNUSED(bucket_count) , const Hash& h = Hash{}, const KeyEqual& equal = KeyEqual{}) noexcept(noexcept(Hash(h)) && noexcept(KeyEqual(equal))) : WHash(h), WKeyEqual(equal) { ROBIN_HOOD_TRACE(this) } template <typename Iter> Table(Iter first, Iter last, size_t ROBIN_HOOD_UNUSED(bucket_count) = 0, const Hash& h = Hash{}, const KeyEqual& equal = KeyEqual{}) : WHash(h), WKeyEqual(equal) { ROBIN_HOOD_TRACE(this) insert(first, last); } Table(std::initializer_list<value_type> initlist, size_t ROBIN_HOOD_UNUSED(bucket_count) = 0, const Hash& h = Hash{}, const KeyEqual& equal = KeyEqual{}) : WHash(h), WKeyEqual(equal) { ROBIN_HOOD_TRACE(this) insert(initlist.begin(), initlist.end()); } Table(Table&& o) noexcept : WHash(std::move(static_cast<WHash&>(o))), WKeyEqual(std::move(static_cast<WKeyEqual&>(o))), DataPool(std::move(static_cast<DataPool&>(o))) { ROBIN_HOOD_TRACE(this) if (o.mMask) { mHashMultiplier = std::move(o.mHashMultiplier); mKeyVals = std::move(o.mKeyVals); mInfo = std::move(o.mInfo); mNumElements = std::move(o.mNumElements); mMask = std::move(o.mMask); mMaxNumElementsAllowed = std::move(o.mMaxNumElementsAllowed); mInfoInc = std::move(o.mInfoInc); mInfoHashShift = std::move(o.mInfoHashShift); o.init(); } } Table& operator=(Table&& o) noexcept { ROBIN_HOOD_TRACE(this) if (&o != this) { if (o.mMask) { destroy(); mHashMultiplier = std::move(o.mHashMultiplier); mKeyVals = std::move(o.mKeyVals); mInfo = std::move(o.mInfo); mNumElements = std::move(o.mNumElements); mMask = std::move(o.mMask); mMaxNumElementsAllowed = std::move(o.mMaxNumElementsAllowed); mInfoInc = std::move(o.mInfoInc); mInfoHashShift = std::move(o.mInfoHashShift); WHash::operator=(std::move(static_cast<WHash&>(o))); WKeyEqual::operator=(std::move(static_cast<WKeyEqual&>(o))); DataPool::operator=(std::move(static_cast<DataPool&>(o))); o.init(); } else { clear(); } } return *this; } Table(const Table& o) : WHash(static_cast<const WHash&>(o)), WKeyEqual(static_cast<const WKeyEqual&>(o)), DataPool(static_cast<const DataPool&>(o)) { ROBIN_HOOD_TRACE(this) if (!o.empty()) { auto const numElementsWithBuffer = calcNumElementsWithBuffer(o.mMask + 1); auto const numBytesTotal = calcNumBytesTotal(numElementsWithBuffer); ROBIN_HOOD_LOG("std::malloc " << numBytesTotal << " = calcNumBytesTotal(" << numElementsWithBuffer << ")") mHashMultiplier = o.mHashMultiplier; mKeyVals = static_cast<Node*>(detail::assertNotNull<std::bad_alloc>( std::malloc(numBytesTotal))); mInfo = reinterpret_cast<uint8_t*>(mKeyVals + numElementsWithBuffer); mNumElements = o.mNumElements; mMask = o.mMask; mMaxNumElementsAllowed = o.mMaxNumElementsAllowed; mInfoInc = o.mInfoInc; mInfoHashShift = o.mInfoHashShift; cloneData(o); } } Table& operator=(Table const& o) { ROBIN_HOOD_TRACE(this) if (&o == this) { return *this; } if (o.empty()) { if (0 == mMask) { return *this; } destroy(); init(); WHash::operator=(static_cast<const WHash&>(o)); WKeyEqual::operator=(static_cast<const WKeyEqual&>(o)); DataPool::operator=(static_cast<DataPool const&>(o)); return *this; } Destroyer<Self, IsFlat && std::is_trivially_destructible<Node>::value>{} .nodes(*this); if (mMask != o.mMask) { if (0 != mMask) { ROBIN_HOOD_LOG("std::free") std::free(mKeyVals); } auto const numElementsWithBuffer = calcNumElementsWithBuffer(o.mMask + 1); auto const numBytesTotal = calcNumBytesTotal(numElementsWithBuffer); ROBIN_HOOD_LOG("std::malloc " << numBytesTotal << " = calcNumBytesTotal(" << numElementsWithBuffer << ")") mKeyVals = static_cast<Node*>(detail::assertNotNull<std::bad_alloc>( std::malloc(numBytesTotal))); mInfo = reinterpret_cast<uint8_t*>(mKeyVals + numElementsWithBuffer); } WHash::operator=(static_cast<const WHash&>(o)); WKeyEqual::operator=(static_cast<const WKeyEqual&>(o)); DataPool::operator=(static_cast<DataPool const&>(o)); mHashMultiplier = o.mHashMultiplier; mNumElements = o.mNumElements; mMask = o.mMask; mMaxNumElementsAllowed = o.mMaxNumElementsAllowed; mInfoInc = o.mInfoInc; mInfoHashShift = o.mInfoHashShift; cloneData(o); return *this; } void swap(Table& o) { ROBIN_HOOD_TRACE(this) using std::swap; swap(o, *this); } void clear() { ROBIN_HOOD_TRACE(this) if (empty()) { return; } Destroyer<Self, IsFlat && std::is_trivially_destructible<Node>::value>{} .nodes(*this); auto const numElementsWithBuffer = calcNumElementsWithBuffer(mMask + 1); uint8_t const z = 0; std::fill(mInfo, mInfo + calcNumBytesInfo(numElementsWithBuffer), z); mInfo[numElementsWithBuffer] = 1; mInfoInc = InitialInfoInc; mInfoHashShift = InitialInfoHashShift; } ~Table() { ROBIN_HOOD_TRACE(this) destroy(); } bool operator==(const Table& other) const { ROBIN_HOOD_TRACE(this) if (other.size() != size()) { return false; } for (auto const& otherEntry : other) { if (!has(otherEntry)) { return false; } } return true; } bool operator!=(const Table& other) const { ROBIN_HOOD_TRACE(this) return !operator==(other); } template <typename Q = mapped_type> typename std::enable_if<!std::is_void<Q>::value, Q&>::type operator[](const key_type& key) { ROBIN_HOOD_TRACE(this) auto idxAndState = insertKeyPrepareEmptySpot(key); switch (idxAndState.second) { case InsertionState::key_found: break; case InsertionState::new_node: ::new (static_cast<void*>(&mKeyVals[idxAndState.first])) Node(*this, std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple()); break; case InsertionState::overwrite_node: mKeyVals[idxAndState.first] = Node(*this, std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple()); break; case InsertionState::overflow_error: throwOverflowError(); } return mKeyVals[idxAndState.first].getSecond(); } template <typename Q = mapped_type> typename std::enable_if<!std::is_void<Q>::value, Q&>::type operator[](key_type&& key) { ROBIN_HOOD_TRACE(this) auto idxAndState = insertKeyPrepareEmptySpot(key); switch (idxAndState.second) { case InsertionState::key_found: break; case InsertionState::new_node: ::new (static_cast<void*>(&mKeyVals[idxAndState.first])) Node( *this, std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple()); break; case InsertionState::overwrite_node: mKeyVals[idxAndState.first] = Node( *this, std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple()); break; case InsertionState::overflow_error: throwOverflowError(); } return mKeyVals[idxAndState.first].getSecond(); } template <typename Iter> void insert(Iter first, Iter last) { for (; first != last; ++first) { insert(value_type(*first)); } } void insert(std::initializer_list<value_type> ilist) { for (auto&& vt : ilist) { insert(std::move(vt)); } } template <typename... Args> std::pair<iterator, bool> emplace(Args&&... args) { ROBIN_HOOD_TRACE(this) Node n{*this, std::forward<Args>(args)...}; auto idxAndState = insertKeyPrepareEmptySpot(getFirstConst(n)); switch (idxAndState.second) { case InsertionState::key_found: n.destroy(*this); break; case InsertionState::new_node: ::new (static_cast<void*>(&mKeyVals[idxAndState.first])) Node(*this, std::move(n)); break; case InsertionState::overwrite_node: mKeyVals[idxAndState.first] = std::move(n); break; case InsertionState::overflow_error: n.destroy(*this); throwOverflowError(); break; } return std::make_pair( iterator(mKeyVals + idxAndState.first, mInfo + idxAndState.first), InsertionState::key_found != idxAndState.second); } template <typename... Args> iterator emplace_hint(const_iterator position, Args&&... args) { (void)position; return emplace(std::forward<Args>(args)...).first; } template <typename... Args> std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args) { return try_emplace_impl(key, std::forward<Args>(args)...); } template <typename... Args> std::pair<iterator, bool> try_emplace(key_type&& key, Args&&... args) { return try_emplace_impl(std::move(key), std::forward<Args>(args)...); } template <typename... Args> iterator try_emplace(const_iterator hint, const key_type& key, Args&&... args) { (void)hint; return try_emplace_impl(key, std::forward<Args>(args)...).first; } template <typename... Args> iterator try_emplace(const_iterator hint, key_type&& key, Args&&... args) { (void)hint; return try_emplace_impl(std::move(key), std::forward<Args>(args)...) .first; } template <typename Mapped> std::pair<iterator, bool> insert_or_assign(const key_type& key, Mapped&& obj) { return insertOrAssignImpl(key, std::forward<Mapped>(obj)); } template <typename Mapped> std::pair<iterator, bool> insert_or_assign(key_type&& key, Mapped&& obj) { return insertOrAssignImpl(std::move(key), std::forward<Mapped>(obj)); } template <typename Mapped> iterator insert_or_assign(const_iterator hint, const key_type& key, Mapped&& obj) { (void)hint; return insertOrAssignImpl(key, std::forward<Mapped>(obj)).first; } template <typename Mapped> iterator insert_or_assign(const_iterator hint, key_type&& key, Mapped&& obj) { (void)hint; return insertOrAssignImpl(std::move(key), std::forward<Mapped>(obj)) .first; } std::pair<iterator, bool> insert(const value_type& keyval) { ROBIN_HOOD_TRACE(this) return emplace(keyval); } iterator insert(const_iterator hint, const value_type& keyval) { (void)hint; return emplace(keyval).first; } std::pair<iterator, bool> insert(value_type&& keyval) { return emplace(std::move(keyval)); } iterator insert(const_iterator hint, value_type&& keyval) { (void)hint; return emplace(std::move(keyval)).first; } size_t count(const key_type& key) const { ROBIN_HOOD_TRACE(this) auto kv = mKeyVals + findIdx(key); if (kv != reinterpret_cast_no_cast_align_warning<Node*>(mInfo)) { return 1; } return 0; } template <typename OtherKey, typename Self_ = Self> typename std::enable_if<Self_::is_transparent, size_t>::type count(const OtherKey& key) const { ROBIN_HOOD_TRACE(this) auto kv = mKeyVals + findIdx(key); if (kv != reinterpret_cast_no_cast_align_warning<Node*>(mInfo)) { return 1; } return 0; } bool contains(const key_type& key) const { return 1U == count(key); } template <typename OtherKey, typename Self_ = Self> typename std::enable_if<Self_::is_transparent, bool>::type contains(const OtherKey& key) const { return 1U == count(key); } template <typename Q = mapped_type> typename std::enable_if<!std::is_void<Q>::value, Q&>::type at(key_type const& key) { ROBIN_HOOD_TRACE(this) auto kv = mKeyVals + findIdx(key); if (kv == reinterpret_cast_no_cast_align_warning<Node*>(mInfo)) { doThrow<std::out_of_range>("key not found"); } return kv->getSecond(); } template <typename Q = mapped_type> typename std::enable_if<!std::is_void<Q>::value, Q const&>::type at(key_type const& key) const { ROBIN_HOOD_TRACE(this) auto kv = mKeyVals + findIdx(key); if (kv == reinterpret_cast_no_cast_align_warning<Node*>(mInfo)) { doThrow<std::out_of_range>("key not found"); } return kv->getSecond(); } const_iterator find(const key_type& key) const { ROBIN_HOOD_TRACE(this) const size_t idx = findIdx(key); return const_iterator{mKeyVals + idx, mInfo + idx}; } template <typename OtherKey> const_iterator find(const OtherKey& key, is_transparent_tag ) const { ROBIN_HOOD_TRACE(this) const size_t idx = findIdx(key); return const_iterator{mKeyVals + idx, mInfo + idx}; } template <typename OtherKey, typename Self_ = Self> typename std::enable_if< Self_::is_transparent, const_iterator>::type find(const OtherKey& key) const { ROBIN_HOOD_TRACE(this) const size_t idx = findIdx(key); return const_iterator{mKeyVals + idx, mInfo + idx}; } iterator find(const key_type& key) { ROBIN_HOOD_TRACE(this) const size_t idx = findIdx(key); return iterator{mKeyVals + idx, mInfo + idx}; } template <typename OtherKey> iterator find(const OtherKey& key, is_transparent_tag ) { ROBIN_HOOD_TRACE(this) const size_t idx = findIdx(key); return iterator{mKeyVals + idx, mInfo + idx}; } template <typename OtherKey, typename Self_ = Self> typename std::enable_if<Self_::is_transparent, iterator>::type find(const OtherKey& key) { ROBIN_HOOD_TRACE(this) const size_t idx = findIdx(key); return iterator{mKeyVals + idx, mInfo + idx}; } iterator begin() { ROBIN_HOOD_TRACE(this) if (empty()) { return end(); } return iterator(mKeyVals, mInfo, fast_forward_tag{}); } const_iterator begin() const { ROBIN_HOOD_TRACE(this) return cbegin(); } const_iterator cbegin() const { ROBIN_HOOD_TRACE(this) if (empty()) { return cend(); } return const_iterator(mKeyVals, mInfo, fast_forward_tag{}); } iterator end() { ROBIN_HOOD_TRACE(this) return iterator{reinterpret_cast_no_cast_align_warning<Node*>(mInfo), nullptr}; } const_iterator end() const { ROBIN_HOOD_TRACE(this) return cend(); } const_iterator cend() const { ROBIN_HOOD_TRACE(this) return const_iterator{ reinterpret_cast_no_cast_align_warning<Node*>(mInfo), nullptr}; } iterator erase(const_iterator pos) { ROBIN_HOOD_TRACE(this) return erase(iterator{const_cast<Node*>(pos.mKeyVals), const_cast<uint8_t*>(pos.mInfo)}); } iterator erase(iterator pos) { ROBIN_HOOD_TRACE(this) auto const idx = static_cast<size_t>(pos.mKeyVals - mKeyVals); shiftDown(idx); --mNumElements; if (*pos.mInfo) { return pos; } return ++pos; } size_t erase(const key_type& key) { ROBIN_HOOD_TRACE(this) size_t idx{}; InfoType info{}; keyToIdx(key, &idx, &info); do { if (info == mInfo[idx] && WKeyEqual::operator()(key, mKeyVals[idx].getFirst())) { shiftDown(idx); --mNumElements; return 1; } next(&info, &idx); } while (info <= mInfo[idx]); return 0; } void rehash(size_t c) { reserve(c, true); } void reserve(size_t c) { reserve(c, false); } void compact() { ROBIN_HOOD_TRACE(this) auto newSize = InitialNumElements; while (calcMaxNumElementsAllowed(newSize) < mNumElements && newSize != 0) { newSize *= 2; } if (ROBIN_HOOD_UNLIKELY(newSize == 0)) { throwOverflowError(); } ROBIN_HOOD_LOG("newSize > mMask + 1: " << newSize << " > " << mMask << " + 1") if (newSize < mMask + 1) { rehashPowerOfTwo(newSize, true); } } size_type size() const noexcept { ROBIN_HOOD_TRACE(this) return mNumElements; } size_type max_size() const noexcept { ROBIN_HOOD_TRACE(this) return static_cast<size_type>(-1); } ROBIN_HOOD(NODISCARD) bool empty() const noexcept { ROBIN_HOOD_TRACE(this) return 0 == mNumElements; } float max_load_factor() const noexcept { ROBIN_HOOD_TRACE(this) return MaxLoadFactor100 / 100.0F; } float load_factor() const noexcept { ROBIN_HOOD_TRACE(this) return static_cast<float>(size()) / static_cast<float>(mMask + 1); } ROBIN_HOOD(NODISCARD) size_t mask() const noexcept { ROBIN_HOOD_TRACE(this) return mMask; } ROBIN_HOOD(NODISCARD) size_t calcMaxNumElementsAllowed(size_t maxElements) const noexcept { if (ROBIN_HOOD_LIKELY(maxElements <= (std::numeric_limits<size_t>::max)() / 100)) { return maxElements * MaxLoadFactor100 / 100; } return (maxElements / 100) * MaxLoadFactor100; } ROBIN_HOOD(NODISCARD) size_t calcNumBytesInfo(size_t numElements) const noexcept { return numElements + sizeof(uint64_t); } ROBIN_HOOD(NODISCARD) size_t calcNumElementsWithBuffer(size_t numElements) const noexcept { auto maxNumElementsAllowed = calcMaxNumElementsAllowed(numElements); return numElements + (std::min)(maxNumElementsAllowed, (static_cast<size_t>(0xFF))); } ROBIN_HOOD(NODISCARD) size_t calcNumBytesTotal(size_t numElements) const {
#if ROBIN_HOOD(BITNESS) == 64
return numElements * sizeof(Node) + calcNumBytesInfo(numElements);
#else
auto const ne = static_cast<uint64_t>(numElements); auto const s = static_cast<uint64_t>(sizeof(Node)); auto const infos = static_cast<uint64_t>(calcNumBytesInfo(numElements)); auto const total64 = ne * s + infos; auto const total = static_cast<size_t>(total64); if (ROBIN_HOOD_UNLIKELY(static_cast<uint64_t>(total) != total64)) { throwOverflowError(); } return total;
#endif
} private: template <typename Q = mapped_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<!std::is_void<Q>::value, bool>::type has(const value_type& e) const { ROBIN_HOOD_TRACE(this) auto it = find(e.first); return it != end() && it->second == e.second; } template <typename Q = mapped_type> ROBIN_HOOD(NODISCARD) typename std::enable_if<std::is_void<Q>::value, bool>::type has(const value_type& e) const { ROBIN_HOOD_TRACE(this) return find(e) != end(); } void reserve(size_t c, bool forceRehash) { ROBIN_HOOD_TRACE(this) auto const minElementsAllowed = (std::max)(c, mNumElements); auto newSize = InitialNumElements; while (calcMaxNumElementsAllowed(newSize) < minElementsAllowed && newSize != 0) { newSize *= 2; } if (ROBIN_HOOD_UNLIKELY(newSize == 0)) { throwOverflowError(); } ROBIN_HOOD_LOG("newSize > mMask + 1: " << newSize << " > " << mMask << " + 1") if (forceRehash || newSize > mMask + 1) { rehashPowerOfTwo(newSize, false); } } void rehashPowerOfTwo(size_t numBuckets, bool forceFree) { ROBIN_HOOD_TRACE(this) Node* const oldKeyVals = mKeyVals; uint8_t const* const oldInfo = mInfo; const size_t oldMaxElementsWithBuffer = calcNumElementsWithBuffer(mMask + 1); initData(numBuckets); if (oldMaxElementsWithBuffer > 1) { for (size_t i = 0; i < oldMaxElementsWithBuffer; ++i) { if (oldInfo[i] != 0) { insert_move(std::move(oldKeyVals[i])); oldKeyVals[i].~Node(); } } if (oldKeyVals != reinterpret_cast_no_cast_align_warning<Node*>(&mMask)) { if (forceFree) { std::free(oldKeyVals); } else { DataPool::addOrFree( oldKeyVals, calcNumBytesTotal(oldMaxElementsWithBuffer)); } } } } ROBIN_HOOD(NOINLINE) void throwOverflowError() const {
#if ROBIN_HOOD(HAS_EXCEPTIONS)
throw std::overflow_error("robin_hood::map overflow");
#else
abort();
#endif
} template <typename OtherKey, typename... Args> std::pair<iterator, bool> try_emplace_impl(OtherKey&& key, Args&&... args) { ROBIN_HOOD_TRACE(this) auto idxAndState = insertKeyPrepareEmptySpot(key); switch (idxAndState.second) { case InsertionState::key_found: break; case InsertionState::new_node: ::new (static_cast<void*>(&mKeyVals[idxAndState.first])) Node(*this, std::piecewise_construct, std::forward_as_tuple(std::forward<OtherKey>(key)), std::forward_as_tuple(std::forward<Args>(args)...)); break; case InsertionState::overwrite_node: mKeyVals[idxAndState.first] = Node(*this, std::piecewise_construct, std::forward_as_tuple(std::forward<OtherKey>(key)), std::forward_as_tuple(std::forward<Args>(args)...)); break; case InsertionState::overflow_error: throwOverflowError(); break; } return std::make_pair( iterator(mKeyVals + idxAndState.first, mInfo + idxAndState.first), InsertionState::key_found != idxAndState.second); } template <typename OtherKey, typename Mapped> std::pair<iterator, bool> insertOrAssignImpl(OtherKey&& key, Mapped&& obj) { ROBIN_HOOD_TRACE(this) auto idxAndState = insertKeyPrepareEmptySpot(key); switch (idxAndState.second) { case InsertionState::key_found: mKeyVals[idxAndState.first].getSecond() = std::forward<Mapped>(obj); break; case InsertionState::new_node: ::new (static_cast<void*>(&mKeyVals[idxAndState.first])) Node(*this, std::piecewise_construct, std::forward_as_tuple(std::forward<OtherKey>(key)), std::forward_as_tuple(std::forward<Mapped>(obj))); break; case InsertionState::overwrite_node: mKeyVals[idxAndState.first] = Node(*this, std::piecewise_construct, std::forward_as_tuple(std::forward<OtherKey>(key)), std::forward_as_tuple(std::forward<Mapped>(obj))); break; case InsertionState::overflow_error: throwOverflowError(); break; } return std::make_pair( iterator(mKeyVals + idxAndState.first, mInfo + idxAndState.first), InsertionState::key_found != idxAndState.second); } void initData(size_t max_elements) { mNumElements = 0; mMask = max_elements - 1; mMaxNumElementsAllowed = calcMaxNumElementsAllowed(max_elements); auto const numElementsWithBuffer = calcNumElementsWithBuffer(max_elements); auto const numBytesTotal = calcNumBytesTotal(numElementsWithBuffer); ROBIN_HOOD_LOG("std::calloc " << numBytesTotal << " = calcNumBytesTotal(" << numElementsWithBuffer << ")") mKeyVals = reinterpret_cast<Node*>( detail::assertNotNull<std::bad_alloc>(std::malloc(numBytesTotal))); mInfo = reinterpret_cast<uint8_t*>(mKeyVals + numElementsWithBuffer); std::memset(mInfo, 0, numBytesTotal - numElementsWithBuffer * sizeof(Node)); mInfo[numElementsWithBuffer] = 1; mInfoInc = InitialInfoInc; mInfoHashShift = InitialInfoHashShift; } enum class InsertionState { overflow_error, key_found, new_node, overwrite_node }; template <typename OtherKey> std::pair<size_t, InsertionState> insertKeyPrepareEmptySpot(OtherKey&& key) { for (int i = 0; i < 256; ++i) { size_t idx{}; InfoType info{}; keyToIdx(key, &idx, &info); nextWhileLess(&info, &idx); while (info == mInfo[idx]) { if (WKeyEqual::operator()(key, mKeyVals[idx].getFirst())) { return std::make_pair(idx, InsertionState::key_found); } next(&info, &idx); } if (ROBIN_HOOD_UNLIKELY(mNumElements >= mMaxNumElementsAllowed)) { if (!increase_size()) { return std::make_pair(size_t(0), InsertionState::overflow_error); } continue; } auto const insertion_idx = idx; auto const insertion_info = info; if (ROBIN_HOOD_UNLIKELY(insertion_info + mInfoInc > 0xFF)) { mMaxNumElementsAllowed = 0; } while (0 != mInfo[idx]) { next(&info, &idx); } if (idx != insertion_idx) { shiftUp(idx, insertion_idx); } mInfo[insertion_idx] = static_cast<uint8_t>(insertion_info); ++mNumElements; return std::make_pair(insertion_idx, idx == insertion_idx ? InsertionState::new_node : InsertionState::overwrite_node); } return std::make_pair(size_t(0), InsertionState::overflow_error); } bool try_increase_info() { ROBIN_HOOD_LOG("mInfoInc=" << mInfoInc << ", numElements=" << mNumElements << ", maxNumElementsAllowed=" << calcMaxNumElementsAllowed(mMask + 1)) if (mInfoInc <= 2) { return false; } mInfoInc = static_cast<uint8_t>(mInfoInc >> 1U); ++mInfoHashShift; auto const numElementsWithBuffer = calcNumElementsWithBuffer(mMask + 1); for (size_t i = 0; i < numElementsWithBuffer; i += 8) { auto val = unaligned_load<uint64_t>(mInfo + i); val = (val >> 1U) & UINT64_C(0x7f7f7f7f7f7f7f7f); std::memcpy(mInfo + i, &val, sizeof(val)); } mInfo[numElementsWithBuffer] = 1; mMaxNumElementsAllowed = calcMaxNumElementsAllowed(mMask + 1); return true; } bool increase_size() { if (0 == mMask) { initData(InitialNumElements); return true; } auto const maxNumElementsAllowed = calcMaxNumElementsAllowed(mMask + 1); if (mNumElements < maxNumElementsAllowed && try_increase_info()) { return true; } ROBIN_HOOD_LOG("mNumElements=" << mNumElements << ", maxNumElementsAllowed=" << maxNumElementsAllowed << ", load=" << (static_cast<double>(mNumElements) * 100.0 / (static_cast<double>(mMask) + 1))) if (mNumElements * 2 < calcMaxNumElementsAllowed(mMask + 1)) { nextHashMultiplier(); rehashPowerOfTwo(mMask + 1, true); } else { rehashPowerOfTwo((mMask + 1) * 2, false); } return true; } void nextHashMultiplier() { mHashMultiplier += UINT64_C(0xc4ceb9fe1a85ec54); } void destroy() { if (0 == mMask) { return; } Destroyer<Self, IsFlat && std::is_trivially_destructible<Node>::value>{} .nodesDoNotDeallocate(*this); if (mKeyVals != reinterpret_cast_no_cast_align_warning<Node*>(&mMask)) { ROBIN_HOOD_LOG("std::free") std::free(mKeyVals); } } void init() noexcept { mKeyVals = reinterpret_cast_no_cast_align_warning<Node*>(&mMask); mInfo = reinterpret_cast<uint8_t*>(&mMask); mNumElements = 0; mMask = 0; mMaxNumElementsAllowed = 0; mInfoInc = InitialInfoInc; mInfoHashShift = InitialInfoHashShift; } uint64_t mHashMultiplier = UINT64_C(0xc4ceb9fe1a85ec53); Node* mKeyVals = reinterpret_cast_no_cast_align_warning<Node*>(&mMask); uint8_t* mInfo = reinterpret_cast<uint8_t*>(&mMask); size_t mNumElements = 0; size_t mMask = 0; size_t mMaxNumElementsAllowed = 0; InfoType mInfoInc = InitialInfoInc; InfoType mInfoHashShift = InitialInfoHashShift; }; } template <typename Key, typename T, typename Hash = hash<Key>, typename KeyEqual = std::equal_to<Key>, size_t MaxLoadFactor100 = 80> using unordered_flat_map = detail::Table<true, MaxLoadFactor100, Key, T, Hash, KeyEqual>; template <typename Key, typename T, typename Hash = hash<Key>, typename KeyEqual = std::equal_to<Key>, size_t MaxLoadFactor100 = 80> using unordered_node_map = detail::Table<false, MaxLoadFactor100, Key, T, Hash, KeyEqual>; template <typename Key, typename T, typename Hash = hash<Key>, typename KeyEqual = std::equal_to<Key>, size_t MaxLoadFactor100 = 80> using unordered_map = detail::Table< sizeof(robin_hood::pair<Key, T>) <= sizeof(size_t) * 6 && std::is_nothrow_move_constructible<robin_hood::pair<Key, T>>::value && std::is_nothrow_move_assignable<robin_hood::pair<Key, T>>::value, MaxLoadFactor100, Key, T, Hash, KeyEqual>; template <typename Key, typename Hash = hash<Key>, typename KeyEqual = std::equal_to<Key>, size_t MaxLoadFactor100 = 80> using unordered_flat_set = detail::Table<true, MaxLoadFactor100, Key, void, Hash, KeyEqual>; template <typename Key, typename Hash = hash<Key>, typename KeyEqual = std::equal_to<Key>, size_t MaxLoadFactor100 = 80> using unordered_node_set = detail::Table<false, MaxLoadFactor100, Key, void, Hash, KeyEqual>; template <typename Key, typename Hash = hash<Key>, typename KeyEqual = std::equal_to<Key>, size_t MaxLoadFactor100 = 80> using unordered_set = detail::Table<sizeof(Key) <= sizeof(size_t) * 6 && std::is_nothrow_move_constructible<Key>::value && std::is_nothrow_move_assignable<Key>::value, MaxLoadFactor100, Key, void, Hash, KeyEqual>; }
#endif
// clang-format on

// =========================== ここまでライブラリ ===========================

// 方針
// 問題を与えて、部分的に解く関数を作る

const auto T0 = Time();
using Point = Vec2<int>;
using Tiles = Board<signed char, 10, 10>;
using HashType = unsigned long long;
constexpr auto kL = 1;
constexpr auto kU = 2;
constexpr auto kR = 4;
constexpr auto kD = 8;
static auto rng = Random((913418416u ^ (unsigned)(Time() * 10000)) | 1u);
const auto kBoxDrawings =
    array<string, 16>{" ", "╸", "╹", "┛", "╺", "━", "┗", "┻",
                      "╻", "┓", "┃", "┫", "┏", "┳", "┣", "╋"};
auto zobrist_table = array<array<array<HashType, 16>, 10>, 10>();

// 状態の描画
void PrintTiles(const int h, const int w, const Tiles& b) {
    for (auto y = 0; y < h; y++) {
        for (auto x = 0; x < w; x++) {
            cout << (int)b[{y, x}] << " ";
        }
        cout << endl;
    }
    for (auto y = 0; y < h; y++) {
        for (auto x = 0; x < w; x++) {
            cout << kBoxDrawings[b[{y, x}]];
        }
        cout << endl;
    }
}

// ランダムに全域木をつくる
auto RandomSpaningTree(const int n) {
    struct Edge {
        Point from, to;
    };
    auto edges = vector<Edge>();
    for (auto i = 0; i < n; i++) {
        for (auto j = 0; j < n; j++) {
            if (i + 1 < n && !(i + 1 == n - 1 && j == n - 1))
                edges.push_back({{i, j}, {i + 1, j}});
            if (j + 1 < n && !(i == n - 1 && j + 1 == n - 1))
                edges.push_back({{i, j}, {i, j + 1}});
        }
    }
    auto uf = atcoder::dsu(n * n);
    auto tiles = Tiles();
    auto n_added_edges = 0;
    for (auto idx_edges = 0; n_added_edges < n * n - 2; idx_edges++) {
        const auto r = rng.randint(idx_edges, (int)edges.size());
        swap(edges[idx_edges], edges[r]);
        const auto& edge = edges[idx_edges];
        if (!uf.same(edge.from.y * n + edge.from.x,
                     edge.to.y * n + edge.to.x)) {
            uf.merge(edge.from.y * n + edge.from.x, edge.to.y * n + edge.to.x);
            if (edge.from.y + 1 == edge.to.y) {
                tiles[edge.from] |= 8;
                tiles[edge.to] |= 2;
            } else {
                tiles[edge.from] |= 4;
                tiles[edge.to] |= 1;
            }
            n_added_edges++;
        }
    }
    return tiles;
}

// タイルの個数を数える
inline auto ComputeStat(const int u, const int d, const int l, const int r,
                        const Tiles& tiles) {
    auto stat = array<int, 16>();
    for (auto y = u; y < d; y++) {
        for (auto x = l; x < r; x++) {
            stat[tiles[{y, x}]]++;
        }
    }
    return stat;
}

inline auto ComputeStat(const int h, const int w, const Tiles& tiles) {
    return ComputeStat(0, h, 0, w, tiles);
}

// どこかの辺を切って、どこかの辺を足す
auto RandomTargetTree(const int n, const array<int, 16>& target_stat) {
    struct Edge {
        Point from, to;
    };
    auto tiles = RandomSpaningTree(n);
    auto stat = array<int, 16>();
    for (auto y = 0; y < n; y++) {
        for (auto x = 0; x < n; x++) {
            stat[tiles[{y, x}]]++;
        }
    }
    for (auto trial = 0ll;; trial++) {
        if (trial % 128 == 0) { // パラメータ
            tiles = RandomSpaningTree(n);
            stat = ComputeStat(n, n, tiles);
        }
        {
            // 切る
            auto best = 1;
            auto best_edges = vector<Edge>();
            for (auto y = 0; y < n; y++) {
                for (auto x = 0; x < n; x++) {
                    if (x < n - 1 && tiles[{y, x}] & kR) {
                        auto gain = 0;

                        const auto l = Point{y, x};
                        const auto tile_l = tiles[l];
                        gain += stat[tile_l] > target_stat[tile_l];
                        const auto new_tile_l = tile_l ^ kR;
                        assert(new_tile_l < tile_l);
                        gain += stat[new_tile_l] < target_stat[new_tile_l];

                        const auto r = Point{y, x + 1};
                        const auto tile_r = tiles[r];
                        gain += stat[tile_r] > target_stat[tile_r];
                        const auto new_tile_r = tile_r ^ kL;
                        assert(new_tile_r < tile_r);
                        gain += stat[new_tile_r] < target_stat[new_tile_r];

                        if (gain == best) {
                            best_edges.push_back(Edge{l, r});
                        } else if (gain > best) {
                            best = gain;
                            best_edges.clear();
                            best_edges.push_back({l, r});
                        }
                    }
                    if (y < n - 1 && tiles[{y, x}] & kD) {
                        auto gain = 0;

                        const auto u = Point{y, x};
                        const auto tile_u = tiles[u];
                        gain += stat[tile_u] > target_stat[tile_u];
                        const auto new_tile_u = tile_u ^ kD;
                        assert(new_tile_u < tile_u);
                        gain += stat[new_tile_u] < target_stat[new_tile_u];

                        const auto d = Point{y + 1, x};
                        const auto tile_d = tiles[d];
                        gain += stat[tile_d] > target_stat[tile_d];
                        const auto new_tile_d = tile_d ^ kU;
                        assert(new_tile_d < tile_d);
                        gain += stat[new_tile_d] < target_stat[new_tile_d];

                        if (gain == best) {
                            best_edges.push_back({u, d});
                        } else if (gain > best) {
                            best = gain;
                            best_edges.clear();
                            best_edges.push_back({u, d});
                        }
                    }
                }
            }
            if (best_edges.size() == 0) {
                // cout << "trial=" << trial % 1024 << endl;
                return tiles;
            }

            const auto rnd = rng.randint((int)best_edges.size());
            const auto edge = best_edges[rnd];
            if (edge.from.x < edge.to.x) {
                auto& tile_l = tiles[edge.from];
                auto& tile_r = tiles[edge.to];
                stat[tile_l]--;
                stat[tile_r]--;
                tile_l ^= kR;
                tile_r ^= kL;
                stat[tile_l]++;
                stat[tile_r]++;
            } else {
                auto& tile_u = tiles[edge.from];
                auto& tile_d = tiles[edge.to];
                stat[tile_u]--;
                stat[tile_d]--;
                tile_u ^= kD;
                tile_d ^= kU;
                stat[tile_u]++;
                stat[tile_d]++;
            }
        }
        {
            // 足す
            // DFS
            auto region = Tiles();
            auto region_cnt = 1;
            for (auto y = 0; y < n; y++) {
                for (auto x = 0; x < n; x++) {
                    if (region[{y, x}])
                        continue;
                    region[{y, x}] = region_cnt;
                    auto stk = vector<Point>{Point{y, x}};
                    while (stk.size()) {
                        const auto v = stk.back();
                        stk.pop_back();
                        const auto tile = tiles[v];
                        if (tile & kD) {
                            const auto u = Point{v.y + 1, v.x};
                            if (region[u] == 0) {
                                region[u] = region_cnt;
                                stk.push_back(u);
                            }
                        }
                        if (tile & kR) {
                            const auto u = Point{v.y, v.x + 1};
                            if (region[u] == 0) {
                                region[u] = region_cnt;
                                stk.push_back(u);
                            }
                        }
                        if (tile & kU) {
                            const auto u = Point{v.y - 1, v.x};
                            if (region[u] == 0) {
                                region[u] = region_cnt;
                                stk.push_back(u);
                            }
                        }
                        if (tile & kL) {
                            const auto u = Point{v.y, v.x - 1};
                            if (region[u] == 0) {
                                region[u] = region_cnt;
                                stk.push_back(u);
                            }
                        }
                    }
                    region_cnt++;
                }
            }
            assert(region_cnt == 4);

            auto best = 0;
            auto best_edges = vector<Edge>();
            for (auto y = 0; y < n; y++) {
                for (auto x = 0; x < n; x++) {
                    if (x < n - 1 && region[{y, x}] != region[{y, x + 1}]) {
                        auto gain = 0;

                        const auto l = Point{y, x};
                        const auto tile_l = tiles[l];
                        gain += stat[tile_l] > target_stat[tile_l];
                        const auto new_tile_l = tile_l ^ kR;
                        assert(new_tile_l > tile_l);
                        gain += stat[new_tile_l] < target_stat[new_tile_l];

                        const auto r = Point{y, x + 1};
                        const auto tile_r = tiles[r];
                        gain += stat[tile_r] > target_stat[tile_r];
                        const auto new_tile_r = tile_r ^ kL;
                        assert(new_tile_r > tile_r);
                        gain += stat[new_tile_r] < target_stat[new_tile_r];

                        if (gain == best) {
                            best_edges.push_back(Edge{l, r});
                        } else if (gain > best) {
                            best = gain;
                            best_edges.clear();
                            best_edges.push_back({l, r});
                        }
                    }
                    if (y < n - 1 && region[{y, x}] != region[{y + 1, x}]) {
                        auto gain = 0;

                        const auto u = Point{y, x};
                        const auto tile_u = tiles[u];
                        gain += stat[tile_u] > target_stat[tile_u];
                        const auto new_tile_u = tile_u ^ kD;
                        assert(new_tile_u > tile_u);
                        gain += stat[new_tile_u] < target_stat[new_tile_u];

                        const auto d = Point{y + 1, x};
                        const auto tile_d = tiles[d];
                        gain += stat[tile_d] > target_stat[tile_d];
                        const auto new_tile_d = tile_d ^ kU;
                        assert(new_tile_d > tile_d);
                        gain += stat[new_tile_d] < target_stat[new_tile_d];

                        if (gain == best) {
                            best_edges.push_back({u, d});
                        } else if (gain > best) {
                            best = gain;
                            best_edges.clear();
                            best_edges.push_back({u, d});
                        }
                    }
                }
            }
            assert(best_edges.size() > 0);

            const auto rnd = rng.randint((int)best_edges.size());
            const auto edge = best_edges[rnd];
            if (edge.from.x < edge.to.x) {
                auto& tile_l = tiles[edge.from];
                auto& tile_r = tiles[edge.to];
                stat[tile_l]--;
                stat[tile_r]--;
                tile_l ^= kR;
                tile_r ^= kL;
                stat[tile_l]++;
                stat[tile_r]++;
            } else {
                auto& tile_u = tiles[edge.from];
                auto& tile_d = tiles[edge.to];
                stat[tile_u]--;
                stat[tile_d]--;
                tile_u ^= kD;
                tile_d ^= kU;
                stat[tile_u]++;
                stat[tile_d]++;
            }
        }
    }
}

// ハンガリアン法
// 参考: https://ei1333.github.io/luzhiled/snippets/graph/hungarian.html
auto Hungarian(const int h, const int w, const Board<int, 50, 50>& A) {
    // A は 1-based
    using T = int;
    const T infty = numeric_limits<T>::max();
    const int H = h + 1;
    const int W = w + 1;
    static auto P = array<int, 50>();
    fill(P.begin(), P.begin() + W, 0);
    static auto way = array<int, 50>();
    fill(way.begin(), way.begin() + W, 0);
    static auto U = array<int, 50>(); // 行 y の引いた数
    fill(U.begin(), U.begin() + H, 0);
    static auto V = array<int, 50>(); // 列 x の引いた数
    fill(V.begin(), V.begin() + W, 0);
    static auto minV = array<int, 50>(); // 列内の最小値
    static auto used = array<bool, 50>();

    for (int y = 1; y < H; y++) {
        P[0] = y;
        fill(minV.begin(), minV.begin() + W, infty);
        fill(used.begin(), used.begin() + W, false);
        int x0 = 0;
        while (P[x0] != 0) {
            const int y0 = P[x0];
            used[x0] = true;
            T delta = infty; // まだ処理してない部分全体の最小値
            int x1 = 0;      // delta の場所
            for (int x = 1; x < W; x++) {
                if (used[x])
                    continue;
                const T curr = A[{y0, x}] - U[y0] - V[x];
                if (curr < minV[x])
                    minV[x] = curr, way[x] = x0;
                if (minV[x] < delta)
                    delta = minV[x], x1 = x;
            }
            for (int x = 0; x < W; x++) {
                if (used[x])
                    U[P[x]] += delta, V[x] -= delta;
                else
                    minV[x] -= delta;
            }
            x0 = x1;
        }
        do {
            P[x0] = P[way[x0]];
            x0 = way[x0];
        } while (x0 != 0);
    }

    return -V[0];
}

auto ComputePositions(const int u, const int d, const int l, const int r,
                      const Tiles& tiles,
                      const array<signed char, 17>& tile_type_separations) {
    auto result = array<unsigned char, 100>();
    auto indices = tile_type_separations;
    for (auto y = u; y < d; y++)
        for (auto x = l; x < r; x++)
            result[indices[tiles[{y, x}]]++] = (unsigned char)(y << 4 | x);
    return result;
}

auto ComputePositions(const int h, const int w, const Tiles& tiles,
                      const array<signed char, 17>& tile_type_separations) {
    return ComputePositions(0, h, 0, w, tiles, tile_type_separations);
}

struct PartialProblem {
    int H, W;
    Tiles tiles;                    // 初期状態 H x W
    Tiles target_tiles;             // 目標 (H x W 全部)
    int box_u, box_d, box_l, box_r; // とりあえず揃えたい場所
    array<int, 16> stat;            // 各タイルの数
    array<int, 16> target_box_stat; // 揃えたい場所の各タイルの数
    array<unsigned char, 100> target_box_positions; // 揃えたいタイルの位置たち
    array<signed char, 17> tile_type_separations; // stat の累積和
    array<signed char, 17>
        target_box_tile_type_separations; // target_box_stat の累積和
    int id;

    PartialProblem() = default;
    inline PartialProblem(const int a_H, const int a_W, const Tiles& a_tiles,
                          const Tiles& a_target_tiles, const int u, const int d,
                          const int l, const int r)
        : H(a_H), W(a_W), tiles(a_tiles), target_tiles(a_target_tiles),
          box_u(u), box_d(d), box_l(l), box_r(r) {
        stat = ComputeStat(H, W, tiles);
        assert(stat == ComputeStat(H, W, target_tiles));
        target_box_stat = ComputeStat(u, d, l, r, target_tiles);
        tile_type_separations[0] = 0;
        target_box_tile_type_separations[0] = 0;
        for (auto i = 0; i < 16; i++) {
            tile_type_separations[i + 1] = tile_type_separations[i] + stat[i];
            target_box_tile_type_separations[i + 1] =
                target_box_tile_type_separations[i] + target_box_stat[i];
        }
        target_box_positions = ComputePositions(
            u, d, l, r, target_tiles, target_box_tile_type_separations);
    }

    inline auto BoxSize() const { return (box_d - box_u) * (box_r - box_l); }

    inline auto IsLastProblem() const {
        return box_d - box_u == H && box_r - box_l == W;
    }

    // 次の問題たち (TODO: 適宜増やしたりする)
    inline auto Partials(const Tiles& result_tiles) const {
        auto result = vector<PartialProblem>();
        auto new_tiles = Tiles(); // 今の解から得られる次の初期状態
        auto new_target_tiles = Tiles();
        const auto free_u = box_d == H ? 0 : box_d;
        const auto free_d = box_u == 0 ? H : box_u;
        const auto free_l = box_r == W ? 0 : box_r;
        const auto free_r = box_l == 0 ? W : box_l;
        const auto new_H = free_d - free_u;
        const auto new_W = free_r - free_l;
        for (auto y = 0; y < new_H; y++) {
            for (auto x = 0; x < new_W; x++) {
                new_tiles[{y, x}] = result_tiles[{free_u + y, free_l + x}];
                new_target_tiles[{y, x}] =
                    target_tiles[{free_u + y, free_l + x}];
            }
        }

        // 全部
        result.emplace_back(new_H, new_W, new_tiles, new_target_tiles, 0, new_H,
                            0, new_W);
        if (new_H > 3) {
            // 上
            for (auto x = 0; x < new_W; x++)
                if (new_target_tiles[{0, x}] == 0)
                    goto skip_up;
            result.emplace_back(new_H, new_W, new_tiles, new_target_tiles, 0, 1,
                                0, new_W);
        skip_up:
            // 下
            for (auto x = 0; x < new_W; x++)
                if (new_target_tiles[{new_H - 1, x}] == 0)
                    goto skip_down;
            result.emplace_back(new_H, new_W, new_tiles, new_target_tiles,
                                new_H - 1, new_H, 0, new_W);
        skip_down:;
        }
        if (new_W > 3) {
            // 左
            for (auto y = 0; y < new_H; y++)
                if (new_target_tiles[{y, 0}] == 0 || new_target_tiles[{y, 1}])
                    goto skip_left;
            result.emplace_back(new_H, new_W, new_tiles, new_target_tiles, 0,
                                new_H, 0, 1);
        skip_left:
            // 右
            for (auto y = 0; y < new_H; y++)
                if (new_target_tiles[{y, new_W - 1}] == 0)
                    goto skip_right;
            result.emplace_back(new_H, new_W, new_tiles, new_target_tiles, 0,
                                new_H, new_W - 1, new_W);
        skip_right:;
        }

        return result;
    }
};

struct PartialProblemResult {
    bool success;
    vector<char> path;
    Tiles tiles;

    void Print() const {
        for (const auto& c : path) {
            cout << c;
        }
        cout << endl;
    }
};

auto ComputeHash(const int h, const int w, const Tiles& tiles) {
    auto result = (HashType)0;
    for (auto y = 0; y < h; y++)
        for (auto x = 0; x < w; x++)
            result ^= zobrist_table[y][x][tiles[{y, x}]];
    return result;
}

// ヒューリスティック関数の計算
auto ComputeH(const array<unsigned char, 100>& positions, const int tile_type,
              const PartialProblem& problem) {
    // tiles は使わず positions だけ見る
    assert(tile_type != 0);
    static auto cost_matrix = Board<int, 50, 50>();
    if (problem.target_box_stat[tile_type] == 0)
        return 0;
    const auto coef = [&](const auto target_y, const auto target_x) {
        // auto tmp =
        //     abs(target_x - problem.W / 2) + abs(target_y - problem.H / 2) +
        //     1;

        const auto center = problem.target_box_positions[0];

        // auto tmp = abs(target_y - (center >> 4)) +
        //            abs(target_x - (center & 0b1111)) + 1;

        // auto tmp = max(abs(target_y - (center >> 4)),
        //                abs(target_x - (center & 0b1111)));

        return 1; //(int)(tmp * tmp);
    };
    for (auto i = 0; i < problem.stat[tile_type]; i++) {
        const auto yx = positions[problem.tile_type_separations[tile_type] + i];
        const auto y = yx >> 4;
        const auto x = yx & 0b1111;
        for (auto j = 0; j < problem.target_box_stat[tile_type]; j++) {
            const auto target_yx =
                problem.target_box_positions
                    [problem.target_box_tile_type_separations[tile_type] + j];
            const auto target_y = target_yx >> 4;
            const auto target_x = target_yx & 0b1111;
            cost_matrix[{j + 1, i + 1}] =
                (abs(y - target_y) + abs(x - target_x)) *
                coef(target_y, target_x);
        }
    }
    assert(problem.target_box_stat[tile_type] <= problem.stat[tile_type]);
    return Hungarian(problem.target_box_stat[tile_type],
                     problem.stat[tile_type], cost_matrix);
}

static constexpr auto kBufferSize = 2500000;

struct PartialState {
    Tiles tiles;
    HashType hash; // 衝突するとどうなる？？？
    short f;       // g + h
    short g;       // 現在までの距離
    short h;       // ヒューリスティック関数
    array<short, 16> hs; // ヒューリスティック関数 (タイルの種類ごと)
    short h2;
    array<unsigned char, 100>
        positions; // タイルの種類ごとの場所 y/x 4 ビットずつ
    int parent;
    int problem_id;
    static unsigned char temporal_move;
    static Stack<PartialState, kBufferSize> buffer;
    static constexpr auto kH2Coef = 1;

    // 初期状態生成
    static PartialState InitialState(const PartialProblem& problem) {
        const auto positions = ComputePositions(
            problem.H, problem.W, problem.tiles, problem.tile_type_separations);
        auto h = (short)0;
        auto hs = array<short, 16>();
        for (auto i = 1; i < 16; i++)
            h += hs[i] = ComputeH(positions, i, problem);
        auto res = PartialState{
            problem.tiles,
            ComputeHash(problem.H, problem.W, problem.tiles) ^ problem.id,
            h,
            0,
            h,
            hs,
            -999,
            positions,
            -1,
            problem.id};
        res.h2 = res.H2(problem);
        return res;
    }

    // positions だけ一時的に変化させる
    inline void
    MoveTemporarily(const Point to,
                    const array<signed char, 17>& tile_type_separations) {
        const auto tile_type = tiles[to];
        const auto to_char = (unsigned char)(to.y << 4 | to.x);
        swap(tiles[to], tiles[{positions[0] >> 4, positions[0] & 0b1111}]);
        for (auto i = tile_type_separations[tile_type];
             i < tile_type_separations[tile_type + 1]; i++) {
            if (positions[i] == to_char) {
                temporal_move = i;
                swap(positions[0], positions[i]);
                return;
            }
        }
        assert(false);
    }

    // 一時的な変化を戻す
    inline void MoveBack() {
        swap(positions[0], positions[temporal_move]);
        swap(tiles[{positions[temporal_move] >> 4,
                    positions[temporal_move] & 0b1111}],
             tiles[{positions[0] >> 4, positions[0] & 0b1111}]);
        temporal_move = 0xff;
    }

    inline auto H2Point(const PartialProblem& problem, const Point& p0,
                        const Point& p1) const {
        auto result = 0;

        const auto y0 = p0.y;
        const auto x0 = p0.x;
        const auto c0_same = tiles[{y0, x0}] == problem.target_tiles[{y0, x0}];
        const auto u0_same = y0 == 0 || tiles[{y0 - 1, x0}] ==
                                            problem.target_tiles[{y0 - 1, x0}];
        const auto d0_same =
            y0 == problem.H - 1 ||
            tiles[{y0 + 1, x0}] == problem.target_tiles[{y0 + 1, x0}];
        const auto l0_same = x0 == 0 || tiles[{y0, x0 - 1}] ==
                                            problem.target_tiles[{y0, x0 - 1}];
        const auto r0_same =
            x0 == problem.W - 1 ||
            tiles[{y0, x0 + 1}] == problem.target_tiles[{y0, x0 + 1}];
        result += c0_same != u0_same;
        result += c0_same != d0_same;
        result += c0_same != l0_same;
        result += c0_same != r0_same;

        const auto y1 = p1.y;
        const auto x1 = p1.x;
        const auto c1_same = tiles[{y1, x1}] == problem.target_tiles[{y1, x1}];
        const auto u1_same = y1 == 0 || tiles[{y1 - 1, x1}] ==
                                            problem.target_tiles[{y1 - 1, x1}];
        const auto d1_same =
            y1 == problem.H - 1 ||
            tiles[{y1 + 1, x1}] == problem.target_tiles[{y1 + 1, x1}];
        const auto l1_same = x1 == 0 || tiles[{y1, x1 - 1}] ==
                                            problem.target_tiles[{y1, x1 - 1}];
        const auto r1_same =
            x1 == problem.W - 1 ||
            tiles[{y1, x1 + 1}] == problem.target_tiles[{y1, x1 + 1}];
        result += c1_same != u1_same;
        result += c1_same != d1_same;
        result += c1_same != l1_same;
        result += c1_same != r1_same;

        result -= c0_same != c1_same;

        return result * kH2Coef;
    }

    inline int H2(const PartialProblem& problem) const {
        // return 0;
        auto result = 0;
        for (auto y = 0; y <= problem.H; y++) {
            for (auto x = 0; x <= problem.W; x++) {
                if (x != problem.W) {
                    const auto u_same =
                        y == 0 ? true
                               : tiles[{y - 1, x}] ==
                                     problem.target_tiles[{y - 1, x}];
                    const auto d_same =
                        y == problem.H
                            ? true
                            : tiles[{y, x}] == problem.target_tiles[{y, x}];
                    result += u_same != d_same;
                }
                if (y != problem.H) {
                    const auto l_same =
                        x == 0 ? true
                               : tiles[{y, x - 1}] ==
                                     problem.target_tiles[{y, x - 1}];
                    const auto r_same =
                        x == problem.W
                            ? true
                            : tiles[{y, x}] == problem.target_tiles[{y, x}];
                    result += l_same != r_same;
                }
            }
        }

        return result * kH2Coef;

        //==============================
        //     // 上
        //     for (auto y = 0; y < problem.H; y++) {
        //         for (auto x = 0; x < problem.W; x++) {
        //             if (problem.target_tiles[{y, x}] == 0 ||
        //                 tiles[{y, x}] != problem.target_tiles[{y, x}]) {
        //                 result += y;
        //                 goto end_u;
        //             }
        //         }
        //     }
        // end_u:
        //     // 下
        //     for (auto y = problem.H - 1; y >= 0; y--) {
        //         for (auto x = 0; x < problem.W; x++) {
        //             if (problem.target_tiles[{y, x}] == 0 ||
        //                 tiles[{y, x}] != problem.target_tiles[{y, x}]) {
        //                 result += problem.H - 1 - y;
        //                 goto end_d;
        //             }
        //         }
        //     }
        // end_d:
        //     // 左
        //     for (auto x = 0; x < problem.W; x++) {
        //         for (auto y = 0; y < problem.H; y++) {
        //             if (problem.target_tiles[{y, x}] == 0 ||
        //                 tiles[{y, x}] != problem.target_tiles[{y, x}]) {
        //                 result += x;
        //                 goto end_l;
        //             }
        //         }
        //     }
        // end_l:
        //     // 右
        //     for (auto x = problem.W - 1; x >= 0; x--) {
        //         for (auto y = 0; y < problem.H; y++) {
        //             if (problem.target_tiles[{y, x}] == 0 ||
        //                 tiles[{y, x}] != problem.target_tiles[{y, x}]) {
        //                 result += problem.W - 1 - x;
        //                 goto end_r;
        //             }
        //         }
        //     }
        // end_r:

        // return result * -10;

        // ===========================================
        {
            auto scores = Board<bool, 10, 10>();
            for (auto y = 0; y < problem.H; y++) {
                for (auto x = 0; x < problem.W; x++) {
                    if (tiles[{y, x}] == problem.target_tiles[{y, x}] &&
                        (y == 0 || tiles[{y - 1, x}] ==
                                       problem.target_tiles[{y - 1, x}]) &&
                        (x == 0 || tiles[{y, x - 1}] ==
                                       problem.target_tiles[{y, x - 1}])) {
                        scores[{y, x}] = true;
                    } else
                        break;
                }
                for (auto x = problem.W - 1; x >= 0; x--) {
                    if (tiles[{y, x}] == problem.target_tiles[{y, x}] &&
                        (y == 0 || tiles[{y - 1, x}] ==
                                       problem.target_tiles[{y - 1, x}]) &&
                        (x == problem.W - 1 ||
                         tiles[{y, x + 1}] ==
                             problem.target_tiles[{y, x + 1}])) {
                        scores[{y, x}] = true;
                    } else
                        break;
                }
            }

            for (auto y = problem.H - 1; y >= 0; y--) {
                for (auto x = 0; x < problem.W; x++) {
                    if (tiles[{y, x}] == problem.target_tiles[{y, x}] &&
                        (y == problem.H - 1 ||
                         tiles[{y + 1, x}] ==
                             problem.target_tiles[{y + 1, x}]) &&
                        (x == 0 || tiles[{y, x - 1}] ==
                                       problem.target_tiles[{y, x - 1}])) {
                        scores[{y, x}] = true;
                    } else
                        break;
                }
                for (auto x = problem.W - 1; x >= 0; x--) {
                    if (tiles[{y, x}] == problem.target_tiles[{y, x}] &&
                        (y == problem.H - 1 ||
                         tiles[{y + 1, x}] ==
                             problem.target_tiles[{y + 1, x}]) &&
                        (x == problem.W - 1 ||
                         tiles[{y, x + 1}] ==
                             problem.target_tiles[{y, x + 1}])) {
                        scores[{y, x}] = true;
                    } else
                        break;
                }
            }

            auto result = 0;
            for (auto y = 0; y < problem.H; y++) {
                for (auto x = 0; x < problem.W; x++) {
                    result -= scores[{y, x}];
                }
            }

            return result;
        }
    }

    inline auto Path() const {
        auto result = vector<char>();
        auto idx = parent;
        auto u = positions[0];
        while (idx >= 0) {
            const auto v = buffer[idx].positions[0];
            auto d = u - v;
            switch (d) {
            case 1:
                result.push_back('R');
                break;
            case -1:
                result.push_back('L');
                break;
            case 16:
                result.push_back('D');
                break;
            case -16:
                result.push_back('U');
                break;
            default:
                cout << "????? " << d;
                assert(false);
            }
            idx = buffer[idx].parent;
            u = v;
        }
        reverse(result.begin(), result.end());
        return result;
    }

    inline void Print(const int h, const int w) const {
        // cout << "State" << endl;
        // PrintTiles(h, w, tiles);
        // cout << "hash=" << hash << endl;
        // cout << "f=" << f << endl;
        // cout << "g=" << g << endl;
        // cout << "h=" << this->h << endl;
        // cout << "hs=";
        // for (const auto hi : hs)
        //     cout << hi << ",";
        // cout << endl;
        // cout << "parent=" << parent << endl;
    }
};
unsigned char PartialState::temporal_move;
auto PartialState::buffer = Stack<PartialState, kBufferSize>(); // 500 MB
auto& state_buffer = PartialState::buffer;
constexpr auto sz_mb = sizeof(state_buffer) / 1024 / 1024;
static_assert(sz_mb < 800);
static constexpr auto kProblems = 200; // パラメータ
auto problem_buffer = Stack<PartialProblem, kProblems>();
auto global_succeeded = false;
struct PartialStateAction {
    HashType hash; // 遷移先の状態のハッシュ
    int parent;
    short changed_h;
    short h2;
    unsigned char to; // 0 のタイルの移動先 y/x 4 ビットずつ

    inline auto G() const { return state_buffer[parent].g + 1; }
    inline auto H() const {
        const auto& state = state_buffer[parent];
        return state.h + changed_h -
               state.hs[state.tiles[{to >> 4, to & 0b1111}]];
    }
    inline auto HSum() const { return H() + h2 * 1; }
    inline auto F() const { return G() + H(); }

    inline auto ToState(const PartialProblem& problem) const {
        const auto& old_state = state_buffer[parent];
        auto state = PartialState();
        state.tiles = old_state.tiles;

        const auto tmp = state.tiles[{old_state.positions[0] >> 4,
                                      old_state.positions[0] & 0b1111}] == 0;
        assert(tmp);
        const auto tile_type = state.tiles[{to >> 4, to & 0b1111}];
        assert(tile_type != 0);
        state.tiles[{old_state.positions[0] >> 4,
                     old_state.positions[0] & 0b1111}] = tile_type;
        state.tiles[{to >> 4, to & 0b1111}] = 0;
        state.hash = hash;
        state.parent = parent;
        state.positions = old_state.positions;
        for (auto i = problem.tile_type_separations[tile_type];
             i < problem.tile_type_separations[tile_type + 1]; i++) {
            if (state.positions[i] == to) {
                swap(state.positions[0], state.positions[i]);
                goto break_ok;
            }
        }
        assert(false);
    break_ok:
        state.g = G();
        state.h = H();
        state.f = state.g + state.h;
        state.hs = old_state.hs;
        state.hs[tile_type] = changed_h;
        state.h2 = h2;
        state.problem_id = problem.id;
        return state;
    }
};

// ビームサーチ
static auto SolvePartial(const vector<int> problem_ids) {
    state_buffer.clear();

    auto searched = robin_hood::unordered_set<HashType>();

    const auto n = problem_buffer[problem_ids[0]].H;
    const auto kBeamWidth = 512 * (10 * 10 * 10 * 10) / (n * n * n * n);
    // const auto kBeamWidth = 1024 * (10 * 10 * 10) / (n * n * n); //
    //  パラメータ
    static auto next_state_actions = Stack<PartialStateAction, 32000 * 2>();
    next_state_actions.clear();

    for (const auto& problem_id : problem_ids) {
        const auto& problem = problem_buffer[problem_id];

        // 初期状態
        auto initial_state = PartialState::InitialState(problem);
        state_buffer.push(initial_state);
        searched.insert(initial_state.hash);

        // 初期状態からの遷移をキューに入れる
        for (const auto d : {Point{0, 1}, {1, 0}, {0, -1}, {-1, 0}}) {
            auto action = PartialStateAction();
            auto p_char = initial_state.positions[0];
            const auto v = Point{p_char >> 4, p_char & 0b1111};
            const auto u = v + d;
            if (!(0 <= u.x && u.x < problem.W && 0 <= u.y && u.y < problem.H)) {
                continue;
            }
            action.parent = problem_id;
            action.to = (unsigned char)(u.y << 4 | u.x);
            const auto tile_type = initial_state.tiles[u];
            action.hash = initial_state.hash ^
                          zobrist_table[v.y][v.x][tile_type] ^
                          zobrist_table[u.y][u.x][tile_type];
            assert(initial_state.tiles[v] == 0);
            initial_state.MoveTemporarily(u, problem.tile_type_separations);
            action.changed_h =
                ComputeH(initial_state.positions, tile_type, problem);
            action.h2 = initial_state.H2(problem);
            initial_state.MoveBack();
            assert(initial_state.tiles[v] == 0);
            next_state_actions.push(action);
            searched.insert(action.hash);
        }
    }

    for (auto step = 0;; step++) {
        if (next_state_actions.size() == 0) {
            return PartialProblemResult{false};
        }
        if ((global_succeeded && Time() - T0 > 2.7) || step >= (n * n * n)) {
            return PartialProblemResult{false};
        }
        if (next_state_actions.size() > kBeamWidth) {
            nth_element(
                next_state_actions.begin(),
                next_state_actions.begin() + kBeamWidth,
                next_state_actions.end(),
                [](const PartialStateAction& l, const PartialStateAction& r) {
                    return l.HSum() < r.HSum();
                });
            next_state_actions.resize(kBeamWidth);
        }

        if (step % 30 == 1) {
            // const auto& state =
            //     next_state_actions[rng.randint(next_state_actions.size())];
            // cout << "step " << step << endl;
            // const auto& problem =
            //     problem_buffer[state_buffer[state.parent].problem_id];
            // state.ToState(problem).Print(problem.H, problem.W);

            // auto problem_id_counts = std::map<int, int>();
            // for (auto& a : next_state_actions)
            //     problem_id_counts[state_buffer[a.parent].problem_id]++;
            // cout << "problem_id_counts:";
            // for (auto& [problem_id, count] : problem_id_counts)
            //     cout << " " << problem_id << "(" << count << "),";
            // cout << endl;
        }

        const auto state_actions = next_state_actions;
        next_state_actions.clear();

        for (const auto& state_action : state_actions) {
            const auto idx_state = state_buffer.size();
            const auto& problem =
                problem_buffer[state_buffer[state_action.parent].problem_id];
            state_buffer.push(state_action.ToState(problem));
            auto& state = state_buffer.back();
            if (state.h == 0) {
                // state.Print(problem.H, problem.W)
                return PartialProblemResult{true, state.Path(), state.tiles};
            }

            for (const auto& d : {Point{0, 1}, {1, 0}, {0, -1}, {-1, 0}}) {
                auto action = PartialStateAction();
                const auto p_char = state.positions[0];
                const auto v = Point{p_char >> 4, p_char & 0b1111};
                const auto u = v + d;
                if (state.parent != -1 &&
                    state_buffer[state.parent].positions[0] ==
                        (unsigned char)(u.y << 4 | u.x))
                    continue;
                if (!(0 <= u.x && u.x < problem.W && 0 <= u.y &&
                      u.y < problem.H))
                    continue;
                action.parent = idx_state;
                action.to = (unsigned char)(u.y << 4 | u.x);
                const auto tile_type = state.tiles[u];
                action.hash = state.hash ^ zobrist_table[v.y][v.x][tile_type] ^
                              zobrist_table[u.y][u.x][tile_type];
                if (searched.contains(action.hash))
                    continue;
                searched.insert(action.hash);
                action.h2 = state.h2;

                action.h2 -= state.H2Point(problem, v, u);
                state.MoveTemporarily(u, problem.tile_type_separations);
                action.changed_h =
                    ComputeH(state.positions, tile_type, problem);
                action.h2 += state.H2Point(problem, v, u);
                // assert(action.h2 == state.H2(problem));

                state.MoveBack();

                next_state_actions.push(action);
            }
        }
    }
}

struct Input {
    int N, T;
    Tiles tiles;

    void Read() {
        cin >> N >> T;
        string tmp;
        for (auto y = 0; y < N; y++) {
            cin >> tmp;
            for (auto x = 0; x < N; x++) {
                tiles[{y, x}] =
                    tmp[x] <= '9' ? tmp[x] - '0' : tmp[x] - 'a' + 10;
            }
        }
    }
};

auto input = Input();

auto TestSolvePartial() {
    // 問題を設定
    auto problem_ids = vector<int>();
    problem_buffer.clear();
    for (int i = 0; i < kProblems; i++) {
        const auto input_stat = ComputeStat(input.N, input.N, input.tiles);
        const auto target_tiles = RandomTargetTree(input.N, input_stat);

        const auto problem =
            PartialProblem(input.N, input.N, input.tiles, target_tiles, 0,
                           input.N, 0, input.N);
        problem_buffer.push(problem);
        problem_buffer.back().id = problem_buffer.size() - 1;
        // const auto h = PartialState::InitialState(problem).h;
        // cout << "h=" << h << endl;
        problem_ids.push_back(i);
    }

    const auto result = SolvePartial(problem_ids);

    return result;
}

void Solve() {
    auto best_result = PartialProblemResult{};
    auto min_length = input.T + 1;
    global_succeeded = false;
    do {
        const auto res = TestSolvePartial();
        if (!res.success)
            continue;
        global_succeeded = true;
        if (chmin(min_length, (int)res.path.size())) {
            best_result = res;
        }
    } while (min_length > input.T || Time() - T0 < 2.7);
    best_result.Print();
}

auto Initialize() {
    input.Read();
    for (auto&& t : zobrist_table)
        for (auto&& tt : t)
            for (auto i = 1; i < 16; i++)
                tt[i] = rng.next();
}

auto TestTargetPatterns() {
    const auto input_stat = ComputeStat(input.N, input.N, input.tiles);
    for (int trial = 0; trial < 5; trial++) {
        const auto target_tiles = RandomTargetTree(input.N, input_stat);
        PrintTiles(input.N, input.N, target_tiles);
    }
}

int main() {
    Initialize();
    // TestSolvePartial();
    Solve();
}

// 最後に↓を貼る
#ifdef __clang__
#pragma clang attribute pop
#endif
// 最後に↑を貼る