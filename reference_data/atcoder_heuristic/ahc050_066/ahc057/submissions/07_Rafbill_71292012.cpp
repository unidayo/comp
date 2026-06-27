// -*- compile-command: "make" -*-
#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
 #pragma GCC target "tune=native"
 #include <bits/stdc++.h>
 #include <sys/time.h>
 #include <immintrin.h>
 #include <x86intrin.h>
// INCLUDE <ext/pb_ds/assoc_container.hpp>
 #include <immintrin.h>
using namespace std;
// Macros
using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
template <class T> using min_queue = priority_queue<T, vector<T>, greater<T>>;
template <class T> using max_queue = priority_queue<T>;
struct uint64_hash {
  static inline uint64_t rotr(uint64_t x, unsigned k) {
    return (x >> k) | (x << (8U * sizeof(uint64_t) - k));
  }
  static inline uint64_t hash_int(uint64_t x) noexcept {
    auto h1 = x * (uint64_t)(0xA24BAED4963EE407);
    auto h2 = rotr(x, 32U) * (uint64_t)(0x9FB21C651E98DF25);
    auto h = rotr(h1 + h2, 32U);
    return h;
  }
  size_t operator()(uint64_t x) const {
    static const uint64_t FIXED_RANDOM = std::chrono::steady_clock::now().time_since_epoch().count();
    return hash_int(x + FIXED_RANDOM);
  }
};
// template <typename K, typename V, typename Hash = uint64_hash>
// using hash_map = __gnu_pbds::gp_hash_table<K, V, Hash>;
// template <typename K, typename Hash = uint64_hash>
// using hash_set = hash_map<K, __gnu_pbds::null_type, Hash>;
// Types
template<class T>
using min_queue = priority_queue<T, vector<T>, greater<T>>;
template<class T>
using max_queue = priority_queue<T>;
// Printing
template<class T>
void print_collection(ostream& out, T const& x);
template<class T, size_t... I>
void print_tuple(ostream& out, T const& a, index_sequence<I...>);
namespace std {
  template<class... A>
  ostream& operator<<(ostream& out, tuple<A...> const& x) {
    print_tuple(out, x, index_sequence_for<A...>{});
    return out;
  }
  template<class... A>
  ostream& operator<<(ostream& out, pair<A...> const& x) {
    print_tuple(out, x, index_sequence_for<A...>{});
    return out;
  }
  template<class A, size_t N>
  ostream& operator<<(ostream& out, array<A, N> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, vector<A> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, deque<A> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, multiset<A> const& x) { print_collection(out, x); return out; }
  template<class A, class B>
  ostream& operator<<(ostream& out, multimap<A, B> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, set<A> const& x) { print_collection(out, x); return out; }
  template<class A, class B>
  ostream& operator<<(ostream& out, map<A, B> const& x) { print_collection(out, x); return out; }
  template<class A, class B>
  ostream& operator<<(ostream& out, unordered_set<A> const& x) { print_collection(out, x); return out; }
}
template<class T, size_t... I>
void print_tuple(ostream& out, T const& a, index_sequence<I...>){
  using swallow = int[];
  out << '(';
  (void)swallow{0, (void(out << (I == 0? "" : ", ") << get<I>(a)), 0)...};
  out << ')';
}
template<class T>
void print_collection(ostream& out, T const& x) {
  int f = 0;
  out << '[';
  for(auto const& i: x) {
    out << (f++ ? "," : "");
    out << i;
  }
  out << "]";
}
// Random
struct RNG {
  uint64_t s[2];
  RNG(u64 seed) {
    reset(seed);
  }
  RNG() {
    reset(time(0));
  }
  using result_type = u32;
  constexpr u32 min(){ return numeric_limits<u32>::min(); }
  constexpr u32 max(){ return numeric_limits<u32>::max(); }
  u32 operator()() { return randomInt32(); }
  static __attribute__((always_inline)) inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
  }
  inline void reset(u64 seed) {
    struct splitmix64_state {
      u64 s;
      u64 splitmix64() {
        u64 result = (s += 0x9E3779B97f4A7C15);
        result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
        result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
        return result ^ (result >> 31);
      }
    };
    splitmix64_state sm { seed };
    s[0] = sm.splitmix64();
    s[1] = sm.splitmix64();
  }
  uint64_t next() {
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];
    const uint64_t result = rotl(s0 * 5, 7) * 9;
    s1 ^= s0;
    s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
    s[1] = rotl(s1, 37); // c
    return result;
  }
  inline u32 randomInt32() {
    return next();
  }
  inline u64 randomInt64() {
    return next();
  }
  inline u32 random32(u32 r) {
    return (((u64)randomInt32())*r)>>32;
  }
  inline u64 random64(u64 r) {
    return randomInt64()%r;
  }
  inline u32 randomRange32(u32 l, u32 r) {
    return l + random32(r-l+1);
  }
  inline u64 randomRange64(u64 l, u64 r) {
    return l + random64(r-l+1);
  }
  inline double randomDouble() {
    return (double)randomInt32() / 4294967296.0;
  }
  inline float randomFloat() {
    return (float)randomInt32() / 4294967296.0;
  }
  inline double randomRangeDouble(double l, double r) {
    return l + randomDouble() * (r-l);
  }
  template<class T, size_t N>
  void shuffle(array<T, N>& v) {
    for(i32 i = N; i > 1; i--) {
      i32 p = random32(i);
      swap(v[i-1],v[p]);
    }
  }
  template<class T>
  void shuffle(vector<T>& v) {
    i32 sz = v.size();
    for(i32 i = sz; i > 1; i--) {
      i32 p = random32(i);
      swap(v[i-1],v[p]);
    }
  }
  template<class T>
  void shuffle(T* fr, T* to) {
    i32 sz = distance(fr,to);
    for(int i = sz; i > 1; i--) {
      int p = random32(i);
      swap(fr[i-1],fr[p]);
    }
  }
  template<class T>
  inline int sample_index(vector<T> const& v) {
    return random32(v.size());
  }
  template<class T>
  inline T sample(vector<T> const& v) {
    return v[sample_index(v)];
  }
} rng;
// Timer
struct timer {
  chrono::high_resolution_clock::time_point t_begin;
  timer() {
    t_begin = chrono::high_resolution_clock::now();
  }
  void reset() {
    t_begin = chrono::high_resolution_clock::now();
  }
  float elapsed() const {
    return chrono::duration<float>(chrono::high_resolution_clock::now() - t_begin).count();
  }
};
// Util
template<class T>
T& smin(T& x, T const& y) { x = min(x,y); return x; }
template<class T>
T& smax(T& x, T const& y) { x = max(x,y); return x; }
template<typename T>
int sgn(T val) {
  if(val < 0) return -1;
  if(val > 0) return 1;
  return 0;
}
static inline
string int_to_string(int val, int digits = 0) {
  string s = to_string(val);
  reverse(begin(s), end(s));
  while((int)s.size() < digits) s.push_back('0');
  reverse(begin(s), end(s));
  return s;
}
// Debug
static inline void debug_impl_seq() {
  cerr << "}";
}
template <class T, class... V>
void debug_impl_seq(T const& t, V const&... v) {
  cerr << t;
  if(sizeof...(v)) { cerr << ", "; }
  debug_impl_seq(v...);
}
// Bits
__attribute__((always_inline)) inline
u64 bit(u64 x) { return 1ull<<x; }
__attribute__((always_inline)) inline
u64 popcount(u64 x) { return __builtin_popcountll(x); }
__attribute__((always_inline)) inline
void setbit(u64& a, u32 b, u64 value = 1) {
  a = (a&~bit(b)) | (value<<b);
}
__attribute__((always_inline)) inline
u64 getbit(u64 a, u32 b) {
  return (a>>b)&1;
}
__attribute__((always_inline)) inline
u64 lsb(u64 a) {
  return __builtin_ctzll(a);
}
__attribute__((always_inline)) inline
int msb(uint64_t bb) {
  return __builtin_clzll(bb) ^ 63;
}
// more
template<class T, size_t SZ>
struct smallvec {
  int size;
  T elems[SZ];
  __attribute__((always_inline)) inline void reset() { size = 0; }
  __attribute__((always_inline)) inline void push(T const& x) { elems[size++] = x; }
  __attribute__((always_inline)) inline void pop() { size -= 1; }
  T& operator[](size_t index) { return elems[index]; }
  T const& operator[](size_t index) const { return elems[index]; }
};
template<size_t N>
struct static_union_find {
  int A[N];
  void reset(int n = N) {
    for(i32 i = 0; i < (i32)(n); ++i) A[i] = i;
  }
  i32 find(i32 a) {
    return A[a] == a ? a : A[a] = find(A[a]);
  }
  bool unite(i32 a, i32 b) {
    a = find(a); b = find(b);
    if(a != b) {
      A[a] = b;
      return true;
    }else{
      return false;
    }
  }
};
struct union_find {
  vector<i32> A, S;
  union_find(i32 n = 0) : A(n) {
    iota(begin(A), end(A), 0);
  }
  i32 find(i32 a) {
    return A[a] == a ? a : A[a] = find(A[a]);
  }
  bool unite(i32 a, i32 b) {
    a = find(a); b = find(b);
    if(a != b) {
      A[a] = b;
      return true;
    }else{
      return false;
    }
  }
};
template<int N>
struct fast_queue {
  int nqueue;
  int queue[N];
  int queue_pos[N];
  void clear() {
    nqueue = 0;
  }
  __attribute__((always_inline)) inline void add(int x) {
    queue_pos[x] = nqueue;
    queue[nqueue++] = x;
  }
  __attribute__((always_inline)) inline void remove(int x) {
    int i = queue_pos[x];
    swap(queue[i], queue[nqueue-1]);
    queue_pos[queue[i]] = i;
    nqueue -= 1;
  }
};
/*
    pdqsort.h - Pattern-defeating quicksort.
    Copyright (c) 2015 Orson Peters
    This software is provided 'as-is', without any express or implied warranty. In no event will the
    authors be held liable for any damages arising from the use of this software.
    Permission is granted to anyone to use this software for any purpose, including commercial
    applications, and to alter it and redistribute it freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not claim that you wrote the
       original software. If you use this software in a product, an acknowledgment in the product
       documentation would be appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be misrepresented as
       being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/
namespace pdqsort_detail {
    enum {
        // Partitions below this size are sorted using insertion sort.
        insertion_sort_threshold = 24,
        // Partitions above this size use Tukey's ninther to select the pivot.
        ninther_threshold = 128,
        // When we detect an already sorted partition, attempt an insertion sort that allows this
        // amount of element moves before giving up.
        partial_insertion_sort_limit = 8,
        // Must be multiple of 8 due to loop unrolling, and < 256 to fit in unsigned char.
        block_size = 64,
        // Cacheline size, assumes power of two.
        cacheline_size = 64
    };
    template<class T> struct is_default_compare : std::false_type { };
    template<class T> struct is_default_compare<std::less<T>> : std::true_type { };
    template<class T> struct is_default_compare<std::greater<T>> : std::true_type { };
    // Returns floor(log2(n)), assumes n > 0.
    template<class T>
    inline int log2(T n) {
        int log = 0;
        while (n >>= 1) ++log;
        return log;
    }
    // Sorts [begin, end) using insertion sort with the given comparison function.
    template<class Iter, class Compare>
    inline void insertion_sort(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;
        if (begin == end) return;
        for (Iter cur = begin + 1; cur != end; ++cur) {
            Iter sift = cur;
            Iter sift_1 = cur - 1;
            // Compare first so we can avoid 2 moves for an element already positioned correctly.
            if (comp(*sift, *sift_1)) {
                T tmp = std::move(*sift);
                do { *sift-- = std::move(*sift_1); }
                while (sift != begin && comp(tmp, *--sift_1));
                *sift = std::move(tmp);
            }
        }
    }
    // Sorts [begin, end) using insertion sort with the given comparison function. Assumes
    // *(begin - 1) is an element smaller than or equal to any element in [begin, end).
    template<class Iter, class Compare>
    inline void unguarded_insertion_sort(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;
        if (begin == end) return;
        for (Iter cur = begin + 1; cur != end; ++cur) {
            Iter sift = cur;
            Iter sift_1 = cur - 1;
            // Compare first so we can avoid 2 moves for an element already positioned correctly.
            if (comp(*sift, *sift_1)) {
                T tmp = std::move(*sift);
                do { *sift-- = std::move(*sift_1); }
                while (comp(tmp, *--sift_1));
                *sift = std::move(tmp);
            }
        }
    }
    // Attempts to use insertion sort on [begin, end). Will return false if more than
    // partial_insertion_sort_limit elements were moved, and abort sorting. Otherwise it will
    // successfully sort and return true.
    template<class Iter, class Compare>
    inline bool partial_insertion_sort(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;
        if (begin == end) return true;
        int limit = 0;
        for (Iter cur = begin + 1; cur != end; ++cur) {
            if (limit > partial_insertion_sort_limit) return false;
            Iter sift = cur;
            Iter sift_1 = cur - 1;
            // Compare first so we can avoid 2 moves for an element already positioned correctly.
            if (comp(*sift, *sift_1)) {
                T tmp = std::move(*sift);
                do { *sift-- = std::move(*sift_1); }
                while (sift != begin && comp(tmp, *--sift_1));
                *sift = std::move(tmp);
                limit += cur - sift;
            }
        }
        return true;
    }
    template<class Iter, class Compare>
    inline void sort2(Iter a, Iter b, Compare comp) {
        if (comp(*b, *a)) std::iter_swap(a, b);
    }
    // Sorts the elements *a, *b and *c using comparison function comp.
    template<class Iter, class Compare>
    inline void sort3(Iter a, Iter b, Iter c, Compare comp) {
        sort2(a, b, comp);
        sort2(b, c, comp);
        sort2(a, b, comp);
    }
    template<class T>
    inline T* align_cacheline(T* p) {
        std::size_t ip = reinterpret_cast<std::size_t>(p);
        ip = (ip + cacheline_size - 1) & -cacheline_size;
        return reinterpret_cast<T*>(ip);
    }
    template<class Iter>
    inline void swap_offsets(Iter first, Iter last,
                             unsigned char* offsets_l, unsigned char* offsets_r,
                             int num, bool use_swaps) {
        typedef typename std::iterator_traits<Iter>::value_type T;
        if (use_swaps) {
            // This case is needed for the descending distribution, where we need
            // to have proper swapping for pdqsort to remain O(n).
            for (int i = 0; i < num; ++i) {
                std::iter_swap(first + offsets_l[i], last - offsets_r[i]);
            }
        } else if (num > 0) {
            Iter l = first + offsets_l[0]; Iter r = last - offsets_r[0];
            T tmp(std::move(*l)); *l = std::move(*r);
            for (int i = 1; i < num; ++i) {
                l = first + offsets_l[i]; *r = std::move(*l);
                r = last - offsets_r[i]; *l = std::move(*r);
            }
            *r = std::move(tmp);
        }
    }
    // Partitions [begin, end) around pivot *begin using comparison function comp. Elements equal
    // to the pivot are put in the right-hand partition. Returns the position of the pivot after
    // partitioning and whether the passed sequence already was correctly partitioned. Assumes the
    // pivot is a median of at least 3 elements and that [begin, end) is at least
    // insertion_sort_threshold long. Uses branchless partitioning.
    template<class Iter, class Compare>
    inline std::pair<Iter, bool> partition_right_branchless(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;
        // Move pivot into local for speed.
        T pivot(std::move(*begin));
        Iter first = begin;
        Iter last = end;
        // Find the first element greater than or equal than the pivot (the median of 3 guarantees
        // this exists).
        while (comp(*++first, pivot));
        // Find the first element strictly smaller than the pivot. We have to guard this search if
        // there was no element before *first.
        if (first - 1 == begin) while (first < last && !comp(*--last, pivot));
        else while ( !comp(*--last, pivot));
        // If the first pair of elements that should be swapped to partition are the same element,
        // the passed in sequence already was correctly partitioned.
        bool already_partitioned = first >= last;
        if (!already_partitioned) {
            std::iter_swap(first, last);
            ++first;
        }
        // The following branchless partitioning is derived from "BlockQuicksort: How Branch
        // Mispredictions don’t affect Quicksort" by Stefan Edelkamp and Armin Weiss.
        unsigned char offsets_l_storage[block_size + cacheline_size];
        unsigned char offsets_r_storage[block_size + cacheline_size];
        unsigned char* offsets_l = align_cacheline(offsets_l_storage);
        unsigned char* offsets_r = align_cacheline(offsets_r_storage);
        int num_l, num_r, start_l, start_r;
        num_l = num_r = start_l = start_r = 0;
        while (last - first > 2 * block_size) {
            // Fill up offset blocks with elements that are on the wrong side.
            if (num_l == 0) {
                start_l = 0;
                Iter it = first;
                for (unsigned char i = 0; i < block_size;) {
                    offsets_l[num_l] = i++; num_l += !comp(*it, pivot); ++it;
                    offsets_l[num_l] = i++; num_l += !comp(*it, pivot); ++it;
                    offsets_l[num_l] = i++; num_l += !comp(*it, pivot); ++it;
                    offsets_l[num_l] = i++; num_l += !comp(*it, pivot); ++it;
                    offsets_l[num_l] = i++; num_l += !comp(*it, pivot); ++it;
                    offsets_l[num_l] = i++; num_l += !comp(*it, pivot); ++it;
                    offsets_l[num_l] = i++; num_l += !comp(*it, pivot); ++it;
                    offsets_l[num_l] = i++; num_l += !comp(*it, pivot); ++it;
                }
            }
            if (num_r == 0) {
                start_r = 0;
                Iter it = last;
                for (unsigned char i = 0; i < block_size;) {
                    offsets_r[num_r] = ++i; num_r += comp(*--it, pivot);
                    offsets_r[num_r] = ++i; num_r += comp(*--it, pivot);
                    offsets_r[num_r] = ++i; num_r += comp(*--it, pivot);
                    offsets_r[num_r] = ++i; num_r += comp(*--it, pivot);
                    offsets_r[num_r] = ++i; num_r += comp(*--it, pivot);
                    offsets_r[num_r] = ++i; num_r += comp(*--it, pivot);
                    offsets_r[num_r] = ++i; num_r += comp(*--it, pivot);
                    offsets_r[num_r] = ++i; num_r += comp(*--it, pivot);
                }
            }
            // Swap elements and update block sizes and first/last boundaries.
            int num = std::min(num_l, num_r);
            swap_offsets(first, last, offsets_l + start_l, offsets_r + start_r,
                         num, num_l == num_r);
            num_l -= num; num_r -= num;
            start_l += num; start_r += num;
            if (num_l == 0) first += block_size;
            if (num_r == 0) last -= block_size;
        }
        int l_size = 0, r_size = 0;
        int unknown_left = (last - first) - ((num_r || num_l) ? block_size : 0);
        if (num_r) {
            // Handle leftover block by assigning the unknown elements to the other block.
            l_size = unknown_left;
            r_size = block_size;
        } else if (num_l) {
            l_size = block_size;
            r_size = unknown_left;
        } else {
            // No leftover block, split the unknown elements in two blocks.
            l_size = unknown_left/2;
            r_size = unknown_left - l_size;
        }
        // Fill offset buffers if needed.
        if (unknown_left && !num_l) {
            start_l = 0;
            Iter it = first;
            for (unsigned char i = 0; i < l_size;) {
                offsets_l[num_l] = i++; num_l += !comp(*it, pivot); ++it;
            }
        }
        if (unknown_left && !num_r) {
            start_r = 0;
            Iter it = last;
            for (unsigned char i = 0; i < r_size;) {
                offsets_r[num_r] = ++i; num_r += comp(*--it, pivot);
            }
        }
        int num = std::min(num_l, num_r);
        swap_offsets(first, last, offsets_l + start_l, offsets_r + start_r, num, num_l == num_r);
        num_l -= num; num_r -= num;
        start_l += num; start_r += num;
        if (num_l == 0) first += l_size;
        if (num_r == 0) last -= r_size;
        // We have now fully identified [first, last)'s proper position. Swap the last elements.
        if (num_l) {
            offsets_l += start_l;
            while (num_l--) std::iter_swap(first + offsets_l[num_l], --last);
            first = last;
        }
        if (num_r) {
            offsets_r += start_r;
            while (num_r--) std::iter_swap(last - offsets_r[num_r], first), ++first;
            last = first;
        }
        // Put the pivot in the right place.
        Iter pivot_pos = first - 1;
        *begin = std::move(*pivot_pos);
        *pivot_pos = std::move(pivot);
        return std::make_pair(pivot_pos, already_partitioned);
    }
    // Partitions [begin, end) around pivot *begin using comparison function comp. Elements equal
    // to the pivot are put in the right-hand partition. Returns the position of the pivot after
    // partitioning and whether the passed sequence already was correctly partitioned. Assumes the
    // pivot is a median of at least 3 elements and that [begin, end) is at least
    // insertion_sort_threshold long.
    template<class Iter, class Compare>
    inline std::pair<Iter, bool> partition_right(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;
        // Move pivot into local for speed.
        T pivot(std::move(*begin));
        Iter first = begin;
        Iter last = end;
        // Find the first element greater than or equal than the pivot (the median of 3 guarantees
        // this exists).
        while (comp(*++first, pivot));
        // Find the first element strictly smaller than the pivot. We have to guard this search if
        // there was no element before *first.
        if (first - 1 == begin) while (first < last && !comp(*--last, pivot));
        else while ( !comp(*--last, pivot));
        // If the first pair of elements that should be swapped to partition are the same element,
        // the passed in sequence already was correctly partitioned.
        bool already_partitioned = first >= last;
        // Keep swapping pairs of elements that are on the wrong side of the pivot. Previously
        // swapped pairs guard the searches, which is why the first iteration is special-cased
        // above.
        while (first < last) {
            std::iter_swap(first, last);
            while (comp(*++first, pivot));
            while (!comp(*--last, pivot));
        }
        // Put the pivot in the right place.
        Iter pivot_pos = first - 1;
        *begin = std::move(*pivot_pos);
        *pivot_pos = std::move(pivot);
        return std::make_pair(pivot_pos, already_partitioned);
    }
    // Similar function to the one above, except elements equal to the pivot are put to the left of
    // the pivot and it doesn't check or return if the passed sequence already was partitioned.
    // Since this is rarely used (the many equal case), and in that case pdqsort already has O(n)
    // performance, no block quicksort is applied here for simplicity.
    template<class Iter, class Compare>
    inline Iter partition_left(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;
        T pivot(std::move(*begin));
        Iter first = begin;
        Iter last = end;
        while (comp(pivot, *--last));
        if (last + 1 == end) while (first < last && !comp(pivot, *++first));
        else while ( !comp(pivot, *++first));
        while (first < last) {
            std::iter_swap(first, last);
            while (comp(pivot, *--last));
            while (!comp(pivot, *++first));
        }
        Iter pivot_pos = last;
        *begin = std::move(*pivot_pos);
        *pivot_pos = std::move(pivot);
        return pivot_pos;
    }
    template<class Iter, class Compare, bool Branchless>
    inline void pdqsort_loop(Iter begin, Iter end, Compare comp, int bad_allowed, bool leftmost = true) {
        typedef typename std::iterator_traits<Iter>::difference_type diff_t;
        // Use a while loop for tail recursion elimination.
        while (true) {
            diff_t size = end - begin;
            // Insertion sort is faster for small arrays.
            if (size < insertion_sort_threshold) {
                if (leftmost) insertion_sort(begin, end, comp);
                else unguarded_insertion_sort(begin, end, comp);
                return;
            }
            // Choose pivot as median of 3 or pseudomedian of 9.
            diff_t s2 = size / 2;
            if (size > ninther_threshold) {
                sort3(begin, begin + s2, end - 1, comp);
                sort3(begin + 1, begin + (s2 - 1), end - 2, comp);
                sort3(begin + 2, begin + (s2 + 1), end - 3, comp);
                sort3(begin + (s2 - 1), begin + s2, begin + (s2 + 1), comp);
                std::iter_swap(begin, begin + s2);
            } else sort3(begin + s2, begin, end - 1, comp);
            // If *(begin - 1) is the end of the right partition of a previous partition operation
            // there is no element in [begin, end) that is smaller than *(begin - 1). Then if our
            // pivot compares equal to *(begin - 1) we change strategy, putting equal elements in
            // the left partition, greater elements in the right partition. We do not have to
            // recurse on the left partition, since it's sorted (all equal).
            if (!leftmost && !comp(*(begin - 1), *begin)) {
                begin = partition_left(begin, end, comp) + 1;
                continue;
            }
            // Partition and get results.
            std::pair<Iter, bool> part_result =
                Branchless ? partition_right_branchless(begin, end, comp)
                           : partition_right(begin, end, comp);
            Iter pivot_pos = part_result.first;
            bool already_partitioned = part_result.second;
            // Check for a highly unbalanced partition.
            diff_t l_size = pivot_pos - begin;
            diff_t r_size = end - (pivot_pos + 1);
            bool highly_unbalanced = l_size < size / 8 || r_size < size / 8;
            // If we got a highly unbalanced partition we shuffle elements to break many patterns.
            if (highly_unbalanced) {
                // If we had too many bad partitions, switch to heapsort to guarantee O(n log n).
                if (--bad_allowed == 0) {
                    std::make_heap(begin, end, comp);
                    std::sort_heap(begin, end, comp);
                    return;
                }
                if (l_size >= insertion_sort_threshold) {
                    std::iter_swap(begin, begin + l_size / 4);
                    std::iter_swap(pivot_pos - 1, pivot_pos - l_size / 4);
                    if (l_size > ninther_threshold) {
                        std::iter_swap(begin + 1, begin + (l_size / 4 + 1));
                        std::iter_swap(begin + 2, begin + (l_size / 4 + 2));
                        std::iter_swap(pivot_pos - 2, pivot_pos - (l_size / 4 + 1));
                        std::iter_swap(pivot_pos - 3, pivot_pos - (l_size / 4 + 2));
                    }
                }
                if (r_size >= insertion_sort_threshold) {
                    std::iter_swap(pivot_pos + 1, pivot_pos + (1 + r_size / 4));
                    std::iter_swap(end - 1, end - r_size / 4);
                    if (r_size > ninther_threshold) {
                        std::iter_swap(pivot_pos + 2, pivot_pos + (2 + r_size / 4));
                        std::iter_swap(pivot_pos + 3, pivot_pos + (3 + r_size / 4));
                        std::iter_swap(end - 2, end - (1 + r_size / 4));
                        std::iter_swap(end - 3, end - (2 + r_size / 4));
                    }
                }
            } else {
                // If we were decently balanced and we tried to sort an already partitioned
                // sequence try to use insertion sort.
                if (already_partitioned && partial_insertion_sort(begin, pivot_pos, comp)
                                        && partial_insertion_sort(pivot_pos + 1, end, comp)) return;
            }
            // Sort the left partition first using recursion and do tail recursion elimination for
            // the right-hand partition.
            pdqsort_loop<Iter, Compare, Branchless>(begin, pivot_pos, comp, bad_allowed, leftmost);
            begin = pivot_pos + 1;
            leftmost = false;
        }
    }
}
template<class Iter, class Compare>
inline void pdqsort(Iter begin, Iter end, Compare comp) {
    if (begin == end) return;
    pdqsort_detail::pdqsort_loop<Iter, Compare,
        pdqsort_detail::is_default_compare<typename std::decay<Compare>::type>::value &&
        std::is_arithmetic<typename std::iterator_traits<Iter>::value_type>::value>(
        begin, end, comp, pdqsort_detail::log2(end - begin));
}
template<class Iter>
inline void pdqsort(Iter begin, Iter end) {
    typedef typename std::iterator_traits<Iter>::value_type T;
    pdqsort(begin, end, std::less<T>());
}
template<class Iter, class Compare>
inline void pdqsort_branchless(Iter begin, Iter end, Compare comp) {
    if (begin == end) return;
    pdqsort_detail::pdqsort_loop<Iter, Compare, true>(
        begin, end, comp, pdqsort_detail::log2(end - begin));
}
template<class Iter>
inline void pdqsort_branchless(Iter begin, Iter end) {
    typedef typename std::iterator_traits<Iter>::value_type T;
    pdqsort_branchless(begin, end, std::less<T>());
}
const f64 TIME_FACTOR = 1.0;
const f64 TL = 1.95 * TIME_FACTOR;
timer TIMER;
const int N = 300;
const int T = 1000;
const int M = 10;
const int K = 30;
const int L = 1e5;
const f64 EPS = 1e-12;
struct vec2f {
  f64 x, y;
  vec2f() { x = y = 0.0; }
  vec2f(f64 x_, f64 y_) { x = x_; y = y_; }
  auto operator<=>(vec2f const&) const = default;
  vec2f operator+(vec2f const& z) const {
    auto r = *this; r += z; return r;
  }
  vec2f operator-(vec2f const& z) const {
    auto r = *this; r -= z; return r;
  }
  vec2f& operator-=(vec2f const& z) {
    x -= z.x;
    y -= z.y;
    return *this;
  }
  vec2f& operator+=(vec2f const& z) {
    x += z.x;
    y += z.y;
    return *this;
  }
  vec2f operator*(f64 const& z) const {
    vec2f r = *this; r *= z; return r;
  }
  vec2f operator/(f64 const& z) const {
    vec2f r = *this; r /= z; return r;
  }
  vec2f& operator*=(f64 const& z) {
    x *= z;
    y *= z;
    return *this;
  }
  vec2f& operator/=(f64 const& z) {
    x /= z;
    y /= z;
    return *this;
  }
  f64 norm2() const {
    return x*x+y*y;
  }
  f64 norm() const {
    return sqrt(x*x+y*y);
  }
  void normalize() {
    if(x < 0) x += L;
    if(x > L) x -= L;
    if(y < 0) y += L;
    if(y > L) y -= L;
  }
};
f64 dot(vec2f const& a, vec2f const& b) {
  return a.x*b.x+a.y*b.y;
}
static inline
vec2f operator*(f64 const& x, vec2f const& v) {
  return vec2f(x*v.x,x*v.y);
}
static inline
ostream& operator<<(ostream& os, vec2f const& v) {
  return os << make_tuple(v.x,v.y);
}
f64 dist(vec2f const& p, vec2f const& q) {
  auto pq = (p-q);
  return round(sqrt(pow(min(pq.x,L-pq.x),2) + pow(min(pq.y,L-pq.y),2)));
}
vec2f initial_pos[N];
vec2f initial_speed[N];
void read() {
  { int n,t,m,k,l;
    cin>>n>>t>>m>>k>>l;
    do { if(!(n == N)) { throw runtime_error("main.cpp" ":" "110" " Assertion failed: " "n == N"); } } while(0);
    do { if(!(t == T)) { throw runtime_error("main.cpp" ":" "111" " Assertion failed: " "t == T"); } } while(0);
    do { if(!(m == M)) { throw runtime_error("main.cpp" ":" "112" " Assertion failed: " "m == M"); } } while(0);
    do { if(!(k == K)) { throw runtime_error("main.cpp" ":" "113" " Assertion failed: " "k == K"); } } while(0);
    do { if(!(l == L)) { throw runtime_error("main.cpp" ":" "114" " Assertion failed: " "l == L"); } } while(0);
  }
  for(i32 i = 0; i < (i32)(N); ++i) {
    cin>>initial_pos[i].x;
    cin>>initial_pos[i].y;
    cin>>initial_speed[i].x;
    cin>>initial_speed[i].y;
  }
}
void init() {
}
struct history_list {
  int t,i1,i2;
  int l,r;
};
const int MAX_HISTORY = 1<<20;
history_list HISTORY_POOL[MAX_HISTORY];
int next_history = 0;
int make_history(int t, int i1, int i2, int l, int r) {
  int id = next_history++;
  HISTORY_POOL[id] = { t, i1,i2,l,r };
  return id;
}
struct component {
  int when;
  f64 cost;
  f64 total_cost;
  int who[N];
  vec2f pos[N];
  vec2f speed;
  int history;
};
f32 env_param(char const* name, f32 value) {
  if(char const* param = std::getenv(name)) {
    value = stof(param);
  }
  cerr << name << ": " << value << endl;
  return value;
}
int BEFORE_2 = env_param("AHC_BEFORE_2", 500);
int BEFORE_4 = env_param("AHC_BEFORE_4", 700);
int BEFORE_8 = env_param("AHC_BEFORE_8", 900);
int BEFORE_16 = env_param("AHC_BEFORE_16", 975);
f64 SPEED_2 = env_param("AHC_SPEED_2", 0.5);
f64 SPEED_4 = env_param("AHC_SPEED_4", 0.4);
f64 SPEED_8 = env_param("AHC_SPEED_8", 0.3);
f64 SPEED_16 = env_param("AHC_SPEED_16", 0.1);
int befores(int attempt, int size) {
  if(attempt == 0) {
    if(size == 30) return T-1;
    if(size == 16 || size == 15) return 986;
    if(size == 8 || size == 7) return 920;
    if(size == 3 || size == 4) return 659;
    if(size == 2) return 339;
  }else if(attempt == 1) {
    if(size == 30) return T-1;
    if(size == 16 || size == 15) return 989;
    if(size == 8 || size == 7) return 815;
    if(size == 3 || size == 4) return 652;
    if(size == 2) return 394;
  }else{
    if(size == 30) return T-1;
    if(size == 16 || size == 15) return BEFORE_16;
    if(size == 8 || size == 7) return BEFORE_8;
    if(size == 3 || size == 4) return BEFORE_4;
    if(size == 2) return BEFORE_2;
  }
  do { throw runtime_error("main.cpp" ":" "190" " impossible"); } while(0);
}
f64 speed_factor(int attempt, int size) {
  if(attempt == 0) {
    if(size == 30) return 0.0;
    if(size == 16) return 0.11683604743995257;
    if(size == 8) return 0.8811102866588563;
    if(size == 4) return 0.3113660826017919;
    if(size == 2) return 0.24300557988009372;
  }else if(attempt == 1) {
    if(size == 30) return 0.0;
    if(size == 16) return 0.03347211529634468;
    if(size == 8) return 0.740691256285192;
    if(size == 4) return 0.3277500329460136;
    if(size == 2) return 0.33386568961652596;
  }else{
    if(size == 30) return 0.0;
    if(size == 16) return SPEED_16;
    if(size == 8) return SPEED_8;
    if(size == 4) return SPEED_4;
    if(size == 2) return SPEED_2;
  }
  do { throw runtime_error("main.cpp" ":" "213" " impossible"); } while(0);
}
f64 TF_2 = env_param("AHC_TF_2", 4);
f64 TF_4 = env_param("AHC_TF_4", 4);
f64 TF_8 = env_param("AHC_TF_8", 4);
f64 TF_16 = env_param("AHC_TF_16", 4);
f64 time_factor(int attempt, int size) {
  if(size == 30) return 0.0;
  if(size == 16) return TF_16;
  if(size == 8) return TF_8;
  if(size == 4) return TF_4;
  if(size == 2) return TF_2;
  do { throw runtime_error("main.cpp" ":" "227" " impossible"); } while(0);
}
component merge_comps
(int attempt, int size,
 component const& c1, component const& c2, bool calc_history = false) {
  int size1 = ((size+1)/2);
  int size2 = (size/2);
  int before = max({c1.when,c2.when,befores(attempt, size)});
  f64 best_dist = 1e9;
  int best_t = 0, best_i1 = 0, best_i2 = 0;
  for(i32 i1 = 0; i1 < (i32)(size1); ++i1) for(i32 i2 = 0; i2 < (i32)(size2); ++i2) {
    auto f = [&](int t) {
      vec2f p1 = c1.pos[i1] + (t-c1.when) * c1.speed;
      vec2f p2 = c2.pos[i2] + (t-c2.when) * c2.speed;
      p1.normalize();
      p2.normalize();
      return dist(p1,p2);
    };
    int lo = max(c1.when, c2.when), hi = before;
    for(i32 t = (lo); t <= (i32)(hi); ++t) {
      auto d = f(t);
      if(d < best_dist) {
        best_dist = d;
        best_t = t;
        best_i1 = i1;
        best_i2 = i2;
      }
    }
  }
  component out;
  out.when = best_t;
  out.cost = best_dist;
  out.total_cost = c1.total_cost+c2.total_cost+best_dist;
  for(i32 i = 0; i < (i32)(size1); ++i) out.pos[i] = c1.pos[i] + (best_t-c1.when) * c1.speed;
  for(i32 i = 0; i < (i32)(size2); ++i) out.pos[size1+i] = c2.pos[i] + (best_t-c2.when) * c2.speed;
  for(i32 i = 0; i < (i32)(size1); ++i) out.who[i] = c1.who[i];
  for(i32 i = 0; i < (i32)(size2); ++i) out.who[size1+i] = c2.who[i];
  out.speed = (c1.speed * (f64)size1 + c2.speed * (f64)size2) / (size1+size2);
  if(calc_history) {
    out.history =
      make_history
      (best_t, c1.who[best_i1], c2.who[best_i2],
       c1.history, c2.history);
  }
  return out;
}
struct sa_state {
  int attempt;
  int nlayer[K+1];
  int layers[K+1][N];
  array<int,2> parent[K+1][N];
  array<int,2> children[K+1][N];
  component comps[K+1][N];
  f64 score;
  int build_item(int size, int h, int root) {
    int id = nlayer[size]++;
    if(size != 1) {
      do { if(!(h != -1)) { throw runtime_error("main.cpp" ":" "294" " Assertion failed: " "h != -1"); } } while(0);
      auto e = HISTORY_POOL[h];
      int x = build_item((size+1)/2, e.l, e.i1);
      int y = build_item(size/2, e.r, e.i2);
      children[size][id] = {x,y};
      parent[(size+1)/2][x] = {size,id};
      parent[size/2][y] = {size,id};
      comps[size][id] = merge_comps(attempt, size, comps[(size+1)/2][x], comps[size/2][y]);
    }else{
      comps[1][id].when = 0;
      comps[1][id].cost = 0;
      comps[1][id].total_cost = 0;
      comps[1][id].pos[0] = initial_pos[root];
      comps[1][id].who[0] = root;
      comps[1][id].speed = initial_speed[root];
      comps[1][id].history = -1;
    }
    parent[size][id] = {K+1,0};
    return id;
  }
  void reset(int attempt_) {
    attempt = attempt_;
    score = 0;
    for(i32 i = 0; i < (i32)(K+1); ++i) nlayer[i] = 0;
  }
  void recompute_at(int size, int i, bool with_hist = false) {
    auto [x,y] = children[size][i];
    comps[size][i]
      = merge_comps(attempt, size, comps[(size+1)/2][x], comps[size/2][y], with_hist);
  }
  void swap_parents(int size, int i, int j) {
    auto [sizei,pi] = parent[size][i];
    auto [sizej,pj] = parent[size][j];
    swap(parent[size][i],parent[size][j]);
    for(i32 ic = 0; ic < (i32)(2); ++ic) if(size == (sizei+1-ic)/2 && children[sizei][pi][ic] == i) {
      children[sizei][pi][ic] = j;
    }
    for(i32 ic = 0; ic < (i32)(2); ++ic) if(size == (sizej+1-ic)/2 && children[sizej][pj][ic] == j) {
      children[sizej][pj][ic] = i;
    }
    while(make_tuple(sizei,pi) != make_tuple(sizej,pj)) {
      if(sizei < sizej) {
        recompute_at(sizei,pi);
        auto [sizei_,pi_] = parent[sizei][pi];
        sizei = sizei_;
        pi = pi_;
      }else{
        recompute_at(sizej,pj);
        auto [sizej_,pj_] = parent[sizej][pj];
        sizej = sizej_;
        pj = pj_;
      }
    }
    while(sizei != K+1) {
      recompute_at(sizei,pi);
      auto [sizei_,pi_] = parent[sizei][pi];
      sizei = sizei_;
      pi = pi_;
    }
  }
  void transition() {
    int sizes[7] = {1,2,3,4,7,8,15};
    int size = sizes[2+rng.random32(5)];
    int i = rng.random32(nlayer[size]);
    int j = rng.random32(nlayer[size]);
    if(i == j || parent[size][i] == parent[size][j]) return;
    swap_parents(size,i,j);
    f64 new_score = 0;
    for(i32 i = 0; i < (i32)(M); ++i) new_score += comps[K][i].total_cost;
    if(new_score <= score) {
      score = new_score;
    }else{
      swap_parents(size,i,j);
      f64 old_score = 0;
      for(i32 i = 0; i < (i32)(M); ++i) old_score += comps[K][i].total_cost;
      if(abs(old_score-score) > 1e-6) {
        do{}while(0);
        do { if(!(false)) { throw runtime_error("main.cpp" ":" "378" " Assertion failed: " "false"); } } while(0);
      }
    }
    // check();
  }
  vector<int> recon() {
    for(i32 size = (2); size <= (i32)(30); ++size) for(i32 i = 0; i < (i32)(nlayer[size]); ++i) {
      recompute_at(size,i,true);
    }
    vector<int> H;
    for(i32 i = 0; i < (i32)(nlayer[K]); ++i) {
      H.push_back(comps[K][i].history);
    }
    return H;
  }
  void check() {
    for(i32 size = (2); size <= (i32)(30); ++size) for(i32 i = 0; i < (i32)(nlayer[size]); ++i) {
      recompute_at(size,i);
    }
    f64 old_score = 0;
    for(i32 i = 0; i < (i32)(M); ++i) old_score += comps[K][i].total_cost;
    do{}while(0);
    do { if(!(abs(old_score - score) < 1e-6)) { throw runtime_error("main.cpp" ":" "402" " Assertion failed: " "abs(old_score - score) < 1e-6"); } } while(0);
  }
};
// https://judge.yosupo.jp/submission/232967
namespace blossom {
    const int N = 403 * 2;
    const int inf = 1e18;
    struct Q{ int u, v, w; } e[N][N];
    vector<int> p[N];
    int n, m = 0, id, h, t, lk[N], sl[N], st[N], f[N], b[N][N], s[N], ed[N], q[N], lab[N];
    void upd(int u, int v) { if (!sl[v] || (lab[e[u][v].u] + lab[e[u][v].v] - 2 * e[e[u][v].u][e[u][v].v].w) < (lab[e[sl[v]][v].u] + lab[e[sl[v]][v].v] - 2 * e[e[sl[v]][v].u][e[sl[v]][v].v].w)) sl[v] = u; }
    void ss(int v) {
        sl[v] = 0;
        for (int u = 1; u <= n; ++u) if (e[u][v].w > 0 && st[u] != v && !s[st[u]]) upd(u, v);
    }
    void ins(int u){ if (u <= n) q[++t] = u; else for (int v : p[u]) ins(v); }
    void ch(int u, int w) { st[u] = w; if (u > n) for (int v : p[u]) ch(v, w); }
    int gr(int u, int v) {
        if ((v = find(begin(p[u]), end(p[u]), v) - p[u].begin()) & 1) {
            reverse(1 + begin(p[u]), end(p[u]));
            return (int)p[u].size() - v;
        }
        return v;
    }
    void stm(int u, int v) {
        lk[u] = e[u][v].v;
        if (u <= n) return; Q w = e[u][v];
        int x = b[u][w.u], y = gr(u,x);
        for (int i = 0; i < y; ++i) stm(p[u][i], p[u][i^1]);
        stm(x, v); rotate(p[u].begin(), y+begin(p[u]), end(p[u]));
    }
    void aug(int u, int v) {
        int w = st[lk[u]];stm(u, v);if (!w) return;
        stm(w, st[f[w]]);
        aug(st[f[w]], w);
    }
    int lca(int u, int v) {
        for (id++; u|v; swap(u, v)) {
            if (!u) continue;if(ed[u] == id) return u;
            ed[u] = id; if (u = st[lk[u]]) u = st[f[u]];
        }
        return 0;
    }
    void add(int u, int a, int v) {
        int x = n + 1; while (x <= m && st[x]) ++x;
        if (x > m) ++m;
        lab[x] = s[x] = st[x] = 0;
        lk[x] = lk[a];
        p[x].clear();
        p[x].push_back(a);
        for (int i = u, j = 0; i != a; i=st[f[j]]) p[x].push_back(i), p[x].push_back(j=st[lk[i]]), ins(j); reverse(1+begin(p[x]), end(p[x]));for (int i = v, j = 0; i != a; i=st[f[j]]) p[x].push_back(i), p[x].push_back(j=st[lk[i]]), ins(j);
        ch(x, x); for (int i = 1; i <= m; ++i) e[x][i].w = e[i][x].w = 0;
        fill(b[x], b[x]+n+1, 0);
        for (int u : p[x]) {
            for (int v = 1; v <= m; ++v) if (!e[x][v].w || (lab[e[u][v].u] + lab[e[u][v].v] - 2 * e[e[u][v].u][e[u][v].v].w) < (lab[e[x][v].u] + lab[e[x][v].v] - 2 * e[e[x][v].u][e[x][v].v].w)) e[x][v] = e[u][v], e[v][x] = e[v][u];
            for (int v = 1; v <= n; ++v) if (b[u][v]) b[x][v] = u;
        }
        ss(x);
    }
    void ex(int u) {
        for (int x : p[u]) ch(x, x);
        int a = b[u][e[u][f[u]].u], r = gr(u, a);
        for (int i = 0; i < r; i += 2) {
            int x = p[u][i], y = p[u][i + 1];
            f[x] = e[y][x].u; s[x] = 1; s[y] = 0; sl[x] = 0; ss(y); ins(y);
        }
        s[a] = 1; f[a] = f[u];
        for (int i = r + 1; i < p[u].size(); ++i) s[p[u][i]] = -1, ss(p[u][i]);
        st[u] = 0;
    }
    bool on(const Q &e) {
        int u = st[e.u], v = st[e.v], a;
        if (s[v] == -1) {
            f[v] = e.u, s[v] = 1, a = st[lk[v]], sl[v] = sl[a] = s[a] = 0, ins(a);
        } else if (!s[v]) {
            a = lca(u, v); if (!a) return aug(u, v), aug(v, u), 1; else add(u, a, v);
        }
        return 0;
    }
    bool bfs() {
        fill(s, s+m+1, -1);fill(sl, sl+m+1, 0); // s is filled with -1
        h = 1, t = 0; for (int i = 1; i <= m; ++i) if (st[i] == i && !lk[i]) f[i] = s[i] = 0, ins(i);
        if (h > t) return 0;
        while (1) {
            while (h <= t) {
                int u = q[h++];
                if (s[st[u]] != 1) {
                    for (int v = 1; v <= n; ++v) if (e[u][v].w > 0 && st[u] != st[v]) {
                        if ((lab[e[u][v].u] + lab[e[u][v].v] - 2 * e[e[u][v].u][e[u][v].v].w)) upd(u, st[v]); else if (on(e[u][v])) return 1;
                    }
                }
            }
            int x = inf;
            for (int i = n+1; i <= m; ++i) if (st[i] == i && s[i] == 1) x = min(x, lab[i]/2);
            for (int i = 1; i <= m; ++i) if (st[i] == i && sl[i] && s[i] != 1) x = min(x, (lab[e[sl[i]][i].u] + lab[e[sl[i]][i].v] - 2 * e[e[sl[i]][i].u][e[sl[i]][i].v].w)>>s[i]+1);
            for (int i = 1; i <= n; ++i) if (~s[st[i]]) if ((lab[i] += (s[st[i]] * 2 - 1) * x) <=0) return 0;
            for (int i = n + 1; i <= m; ++i) if (st[i] == i && ~s[st[i]]) lab[i] += (2 - 4 * s[st[i]]) * x;
            h = 1, t = 0;
            for (int i = 1; i <= m; ++i) if (st[i] == i && sl[i] && st[sl[i]] != i && !(lab[e[sl[i]][i].u] + lab[e[sl[i]][i].v] - 2 * e[e[sl[i]][i].u][e[sl[i]][i].v].w) && on(e[sl[i]][i])) return 1;
            for (int i = n+1; i <= m; ++i) if (st[i] == i && s[i] == 1 && !lab[i]) ex(i);
        }
    }
    pair<int, vector<array<int, 2>>> run(int N, vector<array<int, 3>> edges) {
        for (auto &[u, v, w] : edges) ++u, ++v;
        fill(ed, ed+m+1, 0);
        fill(lk, lk+m+1, 0);
        n = m = N;
        id = 0;
        iota(st + 1, st + n + 1, 1);
        int wm = 0, weight = 0;
        for (int i = 1; i <= n; ++i) for (int j = 1; j <= n; ++j) e[i][j] = {i,j,0};
        for (auto [u, v, w] : edges) wm = max(wm, e[v][u].w = e[u][v].w = max(e[u][v].w, w));
        for (int i = 1; i <= n; ++i) p[i].clear();
        for (int i = 1; i <= n; ++i) for (int j = 1; j <= n; ++j) b[i][j] = i==j?i:0;
        fill_n(lab+1, n, wm); while (bfs());
        vector<array<int, 2>> matching;
        for (int i = 1; i <= n; ++i) if (i < lk[i]) weight += e[i][lk[i]].w, matching.push_back({i - 1, lk[i] - 1});
        return {weight, matching};
    }
}
void merge_components_unary
(int attempt, int size, int count,
 vector<component> const& comps,
 vector<component> &comps2,
 vector<component> &comps1)
{
 vector<array<int,3>> edges;
  int n = comps.size();
  int m = n - 2*count;
  do{}while(0);
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(i); ++j) {
    auto c = merge_comps(attempt, size, comps[i], comps[j]);
    edges.push_back({i,j,(int)(1e6 - c.cost
                        + speed_factor(attempt, size) * c.speed.norm2()
                        + time_factor(attempt, size) * (1000 - c.when)
                        )});
  }
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(m); ++j) {
    edges.push_back({i,n+j,(int)(1e6
                          + speed_factor(attempt, size) * comps[i].speed.norm2()
                          + time_factor(attempt, size) * (1000 - comps[i].when)
                          )});
  }
  auto [ans, ma] = blossom::run(n + m + 1, edges);
  for(auto [i,j] : ma) {
    if(j < n) {
      comps2.push_back(merge_comps(attempt, size, comps[i], comps[j], 1));
    }else{
      comps1.push_back(comps[i]);
    }
  }
}
void merge_components_binary
(int attempt, int size,
 vector<component> const& comps2,
 vector<component> const& comps1,
 vector<component> &comps4,
 vector<component> &comps3)
{
 vector<array<int,3>> edges;
  int n = comps2.size();
  int m = comps1.size();
  do{}while(0);
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(i); ++j) {
    auto c = merge_comps(attempt, size, comps2[i], comps2[j]);
    edges.push_back({i,j,(int)(1e6 - c.cost
                        + speed_factor(attempt, size) * c.speed.norm2()
                        + time_factor(attempt, size) * (1000 - c.when)
                        )});
  }
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(m); ++j) {
    auto c = merge_comps(attempt, size-1, comps2[i], comps1[j]);
    edges.push_back({i,n+j,(int)(1e6 - c.cost
                          + speed_factor(attempt, size) * c.speed.norm2()
                          + time_factor(attempt, size) * (1000 - c.when)
                          )});
  }
  auto [ans, ma] = blossom::run(n + m + 1, edges);
  for(auto [i,j] : ma) {
    if(j < n) {
      comps4.push_back(merge_comps(attempt, size, comps2[i], comps2[j], true));
    }else{
      comps3.push_back(merge_comps(attempt, size-1, comps2[i], comps1[j - n], true));
    }
  }
}
f64 eval_merges(vector<array<int,3>> merges) {
  int t = 0;
  vector<vec2f> pos(N);
  vector<vec2f> speed(N);
  vector<int> component(N);
  for(i32 i = 0; i < (i32)(N); ++i) {
    component[i] = i;
    pos[i] = initial_pos[i];
    speed[i] = initial_speed[i];
  }
  f64 cost = 0;
  for(auto [t2, i, j] : merges) {
    while(t < t2) {
      t += 1;
      for(i32 i = 0; i < (i32)(N); ++i) {
        pos[i] += speed[component[i]];
        pos[i].normalize();
      }
    }
    cost += dist(pos[i],pos[j]);
    // debug(t2,i,j,dist(pos[i],pos[j]), cost);
    vector<int> L, R;
    for(i32 x = 0; x < (i32)(N); ++x) if(component[x] == component[i]) {
      L.push_back(x);
    }
    for(i32 x = 0; x < (i32)(N); ++x) if(component[x] == component[j]) {
      R.push_back(x);
    }
    auto new_speed
      = (speed[component[i]] * L.size() + speed[component[j]] * R.size()) / (L.size()+R.size());
    for(auto x : R) component[x] = component[i];
    speed[component[i]] = new_speed;
  }
  return cost;
}
void solve() {
  f64 best_score = 1e100;
  vector<array<int,3>> best_merges;
  for(i32 attempt = 0; attempt < (i32)(2); ++attempt) {
    vector<component> comps1;
    for(i32 i = 0; i < (i32)(N); ++i) {
      comps1.emplace_back();
      auto& c = comps1.back();
      c.when = 0;
      c.pos[0] = initial_pos[i];
      c.who[0] = i;
      c.speed = initial_speed[i];
      c.cost = 0;
      c.history = -1;
    }
    vector<component> comps2, comps1_;
    merge_components_unary(attempt, 2, 140, comps1, comps2, comps1_);
    do{}while(0);
    do{}while(0);
    vector<component> comps4, comps3;
    merge_components_binary(attempt, 4, comps2, comps1_, comps4, comps3);
    do{}while(0);
    vector<component> comps8, comps7;
    merge_components_binary(attempt, 8, comps4, comps3, comps8, comps7);
    do{}while(0);
    vector<component> comps16, comps15;
    merge_components_binary(attempt, 16, comps8, comps7, comps16, comps15);
    do{}while(0);
    vector<component> comps30, comps15_;
    merge_components_unary(attempt, 30, 10, comps15, comps30, comps15_);
    do{}while(0);
    static sa_state S;
    S.reset(attempt);
    for(auto c : comps30) {
      S.build_item(K, c.history, -1);
    }
    S.score = 0;
    for(i32 i = 0; i < (i32)(M); ++i) S.score += S.comps[K][i].total_cost;
    do{}while(0);
    auto best = S.score;
    while(TIMER.elapsed() < (1+attempt) * (TL / 2)) {
      S.transition();
      if(S.score < best) {
        best = S.score;
        do{}while(0);
      }
    }
    auto H = S.recon();
    vector<array<int,3>> merges;
    auto recon = [&](auto&& recon, int h) -> void {
      if(h == -1) return;
      auto e = HISTORY_POOL[h];
      merges.push_back({e.t,e.i1,e.i2});
      recon(recon, e.l);
      recon(recon, e.r);
    };
    for(auto h : H) recon(recon, h);
    // for(auto c : comps30) recon(recon, c.history);
    sort(begin(merges), end(merges));
    auto new_score = eval_merges(merges);
    do{}while(0);
    if(new_score < best_score) {
      best_score = new_score;
      best_merges = merges;
    }
  }
  do{}while(0);
  do { if(!(best_merges.size() == N-M)) { throw runtime_error("main.cpp" ":" "710" " Assertion failed: " "best_merges.size() == N-M"); } } while(0);
  for(auto [t,i,j] : best_merges) {
    cout << t << " " << i << " " << j << endl;
  }
}
int main() {
  ios::sync_with_stdio(false);
  cerr << setprecision(6) << fixed;
  TIMER.reset();
  read();
  init();
  solve();
  cerr << "[DATA] time = " << (i32)(TIMER.elapsed()*1000) << endl;
  return 0;
}
