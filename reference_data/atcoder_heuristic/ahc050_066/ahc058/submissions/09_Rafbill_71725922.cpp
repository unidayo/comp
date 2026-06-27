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
const int BW = 4;
timer TIMER;
const int N = 10;
const int L = 4;
const int T = 500;
const int K = 1;
i64 A[N];
i64 C[N][L];
void read() {
  { int n,l,t,k; cin>>n>>l>>t>>k;
    do { if(!(n == N)) { throw runtime_error("main.cpp" ":" "25" " Assertion failed: " "n == N"); } } while(0);
    do { if(!(l == L)) { throw runtime_error("main.cpp" ":" "26" " Assertion failed: " "l == L"); } } while(0);
    do { if(!(t == T)) { throw runtime_error("main.cpp" ":" "27" " Assertion failed: " "t == T"); } } while(0);
    do { if(!(k == K)) { throw runtime_error("main.cpp" ":" "28" " Assertion failed: " "k == K"); } } while(0);
  }
  for(i32 i = 0; i < (i32)(N); ++i) cin>>A[i];
  for(i32 l = 0; l < (i32)(L); ++l) for(i32 i = 0; i < (i32)(N); ++i) cin>>C[i][l];
}
i64 coeff[N][L][T+1];
void init_coeff() {
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(L); ++j) {
    u64 B[L];
    for(i32 x = 0; x < (i32)(L); ++x) B[x] = 0;
    B[j] = 1;
    u64 cur = 0;
    for(i32 step = 0; step < (i32)(T+1); ++step) {
      coeff[i][j][step] = cur;
      for(i32 x = 0; x < (i32)(L); ++x) {
        if(x == 0) {
          cur += B[x]*A[i];
        }else{
          B[x-1] += B[x];
        }
      }
    }
  }
}
u64 hash_count[N][L][T+1];
void init() {
  init_coeff();
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(L); ++j) for(i32 t = 0; t < (i32)(T+1); ++t) hash_count[i][j][t] = rng.randomInt64();
}
struct beam_history {
  int i,j;
  int prev;
};
const int MAX_HISTORY = 1<<24;
beam_history* history_pool = 0;
int next_history = 0;
int make_history(int i, int j, int h) {
  history_pool[next_history] = {i,j,h};
  return next_history++;
}
struct beam_state {
  int turn;
  u64 a;
  u64 h = 0;
  u64 B[N][L];
  u64 P[N][L];
  int hist;
  void reset() {
    turn = 0;
    a = 1;
    h = 0;
    hist = -1;
    for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(L); ++j) B[i][j] = 1;
    for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(L); ++j) P[i][j] = 0;
  }
  array<u64, N> eval() const {
    array<u64, N> res;
    for(i32 i = 0; i < (i32)(N); ++i) {
      res[i] = 0;
      u64 p = 1;
      for(i32 j = 0; j < (i32)(L); ++j) {
        p *= P[i][j];
        res[i] += p * B[i][j] * coeff[i][j][T-turn];
      }
    }
    return res;
  }
  void step() {
    turn += 1;
    for(i32 i = 0; i < (i32)(N); ++i) {
      for(i32 j = 0; j < (i32)(L); ++j) {
        if(j == 0) a += A[i] * B[i][j] * P[i][j];
        else B[i][j-1] += B[i][j] * P[i][j];
      }
    }
  }
  void add_history(array<int,2> p) {
    hist = make_history(p[0],p[1],hist);
  }
};
const int HASH_BITS = 24;
const int HASH_SIZE = 1<<HASH_BITS;
const int HASH_MASK = HASH_SIZE-1;
u64* HS = 0;
// array<u64, N> res; 
// FOR(i, N) {
//   res[i] = 0;
//   u64 p = 1;
//   FOR(j, L) {
//     p *= P[i][j];
//     res[i] += p * B[i][j] * coeff[i][j][T-turn];
//   }
// }
// return res;
f64 eval_sol(vector<array<int,2>> const& all_used, array<int,2> const* S) {
  u64 a;
  u64 B[N][L];
  u64 P[N][L];
  a = 1;
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(L); ++j) B[i][j] = 1;
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(L); ++j) P[i][j] = 0;
  for(i32 step = 0; step < (i32)(T); ++step) {
    auto [i,j] = S[step];
    if(i != -1) {
      if((1+P[i][j]) * C[i][j] > a) return 1;
      a -= (1+P[i][j]) * C[i][j];
      P[i][j] += 1;
    }
    for(i32 i = 0; i < (i32)((int)all_used.size()-1); ++i) {
      auto [x,y] = all_used[i];
      if(y == 0) a += A[x] * B[x][y] * P[x][y];
      else B[x][y-1] += B[x][y] * P[x][y];
    }
  }
  return log2(a);
}
bool compact_sol(array<int,2> const* S, array<int,2> *OUT) {
  u64 a;
  u64 B[N][L];
  u64 P[N][L];
  a = 1;
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(L); ++j) B[i][j] = 1;
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(L); ++j) P[i][j] = 0;
  int cur = 0;
  for(i32 i = 0; i < (i32)(T); ++i) OUT[i] = {-1,-1};
  for(i32 step = 0; step < (i32)(T); ++step) {
    auto [i,j] = S[step];
    if(i != -1) {
      while((1+P[i][j]) * C[i][j] > a) {
        OUT[cur] = {-1,-1};
        cur += 1;
        if(cur == T) return false;
        for(i32 x = 0; x < (i32)(N); ++x) {
          for(i32 y = 0; y < (i32)(L); ++y) {
            if(y == 0) a += A[x] * B[x][y] * P[x][y];
            else B[x][y-1] += B[x][y] * P[x][y];
          }
        }
      }
      OUT[cur] = {i,j};
      cur += 1;
      if(cur > T) return false;
      a -= (1+P[i][j]) * C[i][j];
      P[i][j] += 1;
      for(i32 x = 0; x < (i32)(N); ++x) {
        for(i32 y = 0; y < (i32)(L); ++y) {
          if(y == 0) a += A[x] * B[x][y] * P[x][y];
          else B[x][y-1] += B[x][y] * P[x][y];
        }
      }
    }
  }
  return true;
}
vector<array<int,2>> postprocess(vector<array<int,2>> S0, f32 until) {
  array<int,2> S[T];
  array<int,2> tmp[T];
  array<int,2> bestS[T];
  for(i32 i = 0; i < (i32)(T); ++i) S[i] = S0[i];
  for(i32 i = 0; i < (i32)(T); ++i) bestS[i] = S[i];
  bool used[N][L];
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(L); ++j) used[i][j] = 0;
  for(auto [i,j] : S) {
    used[i][j] = 1;
    if(j+1 < L) used[i][j+1] = 1;
  }
  vector<array<int,2>> all_used;
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(L); ++j) if(used[i][j]) all_used.push_back({i,j});
  all_used.push_back({-1,-1});
  f64 score = eval_sol(all_used, S);
  f64 best_score = score;
  f64 temp0 = 1e-1;
  f64 temp1 = 1e-5;
  auto t0 = TIMER.elapsed();
  i64 iters = 0;
  i64 accepts = 0;
  while(t0 < until) {
    f64 done = (TIMER.elapsed()-t0) / (until-t0);
    if(done > 1) break;
    f64 temp = temp0*pow(temp1/temp0, pow(done, 0.8));
    if(iters % 128 == 0) {
      if(compact_sol(S, tmp)) {
        for(i32 i = 0; i < (i32)(T); ++i) S[i] = tmp[i];
        score = eval_sol(all_used, S);
      }
    }
    auto accept = [&](f64 new_score) {
      auto delta = new_score - score;
      return delta >= 0 || -delta <= temp * rng.randomDouble();
    };
    int ty = rng.random32(100);
    if(ty < 40) {
      const int K = 2;
      int i = rng.random32(T);
      int j = clamp<int>(i-K+rng.random32(2*K+1), 0, T-1);
      if(i == j) continue;
      if(S[i] == S[j]) continue;
      swap(S[i],S[j]);
      auto [x,y] = rng.sample(all_used);
      auto old = S[j];
      S[j] = {x,y};
      iters += 1;
      f64 new_score = eval_sol(all_used, S);
      if(accept(new_score)) {
        accepts += 1;
        score = new_score;
      }else{
        S[j] = old;
        swap(S[i],S[j]);
      }
    }else if(false) {
      int i = rng.random32(T);
      auto [x,y] = rng.sample(all_used);
      auto old = S[i];
      S[i] = {x,y};
      iters += 1;
      f64 new_score = eval_sol(all_used, S);
      if(accept(new_score)) {
        accepts += 1;
        score = new_score;
      }else{
        S[i] = old;
      }
    }else if(ty < 100) {
      const int K = 10;
      int k = 3+rng.random32(K);
      array<int,2> OLD[20];
      int i = rng.random32(T-k);
      for(i32 j = 0; j < (i32)(k); ++j) OLD[j] = S[i+j];
      rng.shuffle(S+i, S+i+k);
      f64 new_score = eval_sol(all_used, S);
      if(accept(new_score)) {
        accepts += 1;
        score = new_score;
      }else{
        for(i32 j = 0; j < (i32)(k); ++j) S[i+j] = OLD[j];
      }
    }
    if(score > best_score) {
      best_score = score;
      for(i32 i = 0; i < (i32)(T); ++i) bestS[i] = S[i];
    }
  }
  do{}while(0);
  do{}while(0);
  if(compact_sol(bestS, S)) {
    return vector<array<int,2>>(S, S+T);
  }else{
    do { if(!(false)) { throw runtime_error("main.cpp" ":" "307" " Assertion failed: " "false"); } } while(0);
  }
}
vector<array<int,2> > init_beam() {
  vector<beam_state> BEAM_A;
  vector<beam_state> BEAM_B;
  vector<array<int,2>> BEAM_F;
  vector<array<u64,N>> BEAM_H;
  vector<u64> BEAM_H2;
  vector<int> BEAM_I;
  BEAM_A.emplace_back(); BEAM_A.back().reset();
  for(i32 step = 0; step < (i32)(T); ++step) {
    u64 best = 0;
    u64 besteval = 0;
    for(auto const& s : BEAM_A) best = max(best, s.a);
    // debug(step, BEAM_A.size(), log2(best));
    BEAM_B.clear();
    BEAM_H.clear();
    BEAM_F.clear();
    BEAM_I.clear();
    for(auto const& sa : BEAM_A) {
      for(i32 i = 0; i < (i32)(N); ++i) {
        for(i32 j = 0; j < (i32)(L); ++j) {
          if(sa.a >= C[i][j] * (1+sa.P[i][j])) {
            auto& sb = BEAM_B.emplace_back(sa);
            sb.a -= C[i][j] * (1+sa.P[i][j]);
            sb.P[i][j] += 1;
            sb.h ^= hash_count[i][j][sb.P[i][j]];
            sb.step();
            BEAM_F.push_back({i,j});
            BEAM_H.push_back(sb.eval());
            BEAM_H2.push_back(sb.a);
            for(auto v : BEAM_H.back()) BEAM_H2.back() += v;
            BEAM_I.push_back(BEAM_I.size());
          }
          if(sa.P[i][j] == 0) break;
        }
      }
      {
        auto& sb = BEAM_B.emplace_back(sa);
        sb.step();
        BEAM_H.push_back(sb.eval());
        BEAM_I.push_back(BEAM_I.size());
        BEAM_F.push_back({-1,-1});
      }
    }
    BEAM_A.clear();
    int curbw = 0;
    u64 lhash = rng.randomInt64();
    pdqsort(begin(BEAM_I),
            end(BEAM_I),
            [&](int i, int j) { return BEAM_B[i].a>BEAM_B[j].a; });
    curbw += BW;
    for(int i : BEAM_I) {
      auto const& s = BEAM_B[i];
      auto h = s.h ^ lhash;
      if(HS[h&HASH_MASK] != h) {
        HS[h&HASH_MASK] = h;
        BEAM_A.push_back(s);
        BEAM_A.back().add_history(BEAM_F[i]);
        if((int)BEAM_A.size() == curbw) break;
      }
    }
    {
      // lhash = rng.randomInt64();
      curbw += BW;
      pdqsort(begin(BEAM_I),
              end(BEAM_I),
              [&](int i, int j) { return BEAM_H2[i]>BEAM_H2[j]; });
      for(int i : BEAM_I) {
        auto const& s = BEAM_B[i];
        auto h = s.h ^ lhash;
        if(HS[h&HASH_MASK] != h) {
          HS[h&HASH_MASK] = h;
          BEAM_A.push_back(s);
          BEAM_A.back().add_history(BEAM_F[i]);
          if((int)BEAM_A.size() == curbw) break;
        }
      }
    }
    for(i32 x = (N-1); x >= (i32)(0); --x) {
      // lhash = rng.randomInt64();
      curbw += BW;
      pdqsort(begin(BEAM_I),
              end(BEAM_I),
              [&](int i, int j) { return BEAM_H[i][x]>BEAM_H[j][x]; });
      for(int i : BEAM_I) {
        auto const& s = BEAM_B[i];
        auto h = s.h ^ lhash;
        if(HS[h&HASH_MASK] != h) {
          HS[h&HASH_MASK] = h;
          BEAM_A.push_back(s);
          BEAM_A.back().add_history(BEAM_F[i]);
          if((int)BEAM_A.size() == curbw) break;
        }
      }
    }
  }
  int h = min_element
    (begin(BEAM_A), end(BEAM_A),
     [&](auto const& x, auto const& y) { return x.a > y.a; })
    ->hist;
  vector<array<int,2>> SOL;
  while(h != -1) {
    auto e = history_pool[h];
    SOL.push_back({e.i,e.j});
    h = e.prev;
  }
  reverse(begin(SOL), end(SOL));
  return SOL;
}
void solve() {
  auto SOL = init_beam();
  do{}while(0);
  SOL = postprocess(SOL, TL);
  for(i32 step = 0; step < (i32)(T); ++step) {
    auto [i,j] = SOL[step];
    if(i == -1) cout << -1 << "\n";
    else cout << j << " " << i << "\n";
  }
  cout << flush;
}
int main() {
  ios::sync_with_stdio(false);
  cerr << setprecision(6) << fixed;
  TIMER.reset();
  HS = new u64[HASH_SIZE];
  history_pool = new beam_history[MAX_HISTORY];
  read();
  init();
  solve();
  cerr << "[DATA] time = " << (i32)(TIMER.elapsed()*1000) << endl;
  return 0;
}
