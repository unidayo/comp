// -*- compile-command: "make" -*-
#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
 #pragma GCC target "tune=native"
 #include <bits/stdc++.h>
 #include <sys/time.h>
 #include <immintrin.h>
 #include <x86intrin.h>
 #include <ext/pb_ds/assoc_container.hpp>
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
template <typename K, typename V, typename Hash = uint64_hash>
using hash_map = __gnu_pbds::gp_hash_table<K, V, Hash>;
template <typename K, typename Hash = uint64_hash>
using hash_set = hash_map<K, __gnu_pbds::null_type, Hash>;
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

const f64 TL = 1.95;
timer TIMER;
const int N = 100;
const int L = 500'000;
int T[N];
void read() {
  { int n,l; cin>>n>>l;
    do { if(!(n==N&&l==L)) { throw runtime_error("main.cpp" ":" "15" " Assertion failed: " "n==N&&l==L"); } } while(0);
  }
  for(i32 i = 0; i < (i32)(N); ++i) cin>>T[i];
}
f64 goalp[N];
u64 visited_hash[N];
u64 nopen_hash[N];
void init(){
  for(i32 i = 0; i < (i32)(N); ++i) goalp[i] = 1.0 * T[i]/L;
  for(i32 i = 0; i < (i32)(N); ++i) visited_hash[i] = rng.randomInt64();
  for(i32 i = 0; i < (i32)(N); ++i) nopen_hash[i] = rng.randomInt64();
}
const int MAX_OPEN = 24;
struct open_item {
  f64 x;
  int i;
  bool operator<(open_item const& o) const { return x < o.x; }
};
struct beam_state {
  int a,b;
  bitset<N> visited;
  int nopen;
  open_item open[MAX_OPEN+1];
  int graph[2*N];
  f64 score;
  u64 h;
  void reset(int a_, int b_) {
    for(i32 i = 0; i < (i32)(2*N); ++i) graph[i] = 0;
    h = visited_hash[a_] ^ visited_hash[b_];
    a = a_;
    b = b_;
    visited = 0;
    visited[a] = 1;
    visited[b] = 1;
    nopen = 0;
    if(goalp[a] != 0) add_open(a, goalp[a]);
    if(goalp[b] != 0) add_open(b, goalp[b]);
    score = 0;
  }
  void add_open(int i, f64 x) {
    open[nopen++] = { .x = x/2, .i = 2*i };
    open[nopen++] = { .x = x/2, .i = 2*i+1 };
  }
  void erase(int i, int to) {
    graph[open[i].i] = to;
    nopen -= 1;
    for(i32 j = (i); j <= (i32)(nopen-1); ++j) open[j] = open[j+1];
  }
  void finish() {
    auto bestm = 0;
    f64 best_sc = 1e9;
    for(i32 m = 0; m < (i32)(1<<nopen); ++m) {
      f64 suma = 0, sumb = 0;
      for(i32 i = 0; i < (i32)(nopen); ++i) if(m&(1<<i)) {
        suma += open[i].x;
      }else{
        sumb += open[i].x;
      }
      f64 sc = score + abs(suma-goalp[a]) + abs(sumb-goalp[b]);
      if(sc < best_sc) {
        best_sc = sc;
        bestm = m;
      }
    }
    score = best_sc;
    for(i32 i = 0; i < (i32)(nopen); ++i) {
      if(bestm&(1<<i)) graph[open[i].i] = a;
      else graph[open[i].i] = b;
    }
  }
};
struct candidate {
  int ia;
  f64 score;
  int x;
  int i1, i2, i3, i4;
  f64 val;
  int nopen;
};
const int HASH_BITS = 22;
const u64 HASH_SIZE = 1<<HASH_BITS;
const u64 HASH_MASK = HASH_SIZE-1;
u64 hash_table[HASH_SIZE];
f64 get_bonus(int nopen, int i) {
  return -1e-4 * min(2*(N-i)+1, nopen);
}
struct state {
  int graph[N][2];
  f64 estimate() const {
    f64 est[N];
    for(i32 i = 0; i < (i32)(N); ++i) est[i] = goalp[i];
    for(i32 step = 0; step < (i32)(8); ++step) {
      f64 new_est[N];
      for(i32 i = 0; i < (i32)(N); ++i) new_est[i] = 0;
      for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(2); ++j) new_est[graph[i][j]] += est[i] / 2.0;
      for(i32 i = 0; i < (i32)(N); ++i) est[i] = new_est[i];
    }
    f64 result = 0;
    for(i32 i = 0; i < (i32)(N); ++i) result += abs(est[i] - goalp[i]);
    return result;
  }
  f64 simulate() const {
    int count[N];
    for(i32 i = 0; i < (i32)(N); ++i) count[i] = 0;
    int at = 0;
    for(i32 i = 0; i < (i32)(L); ++i) {
      count[at] += 1;
      at = graph[at][(count[at]^1)&1];
    }
    f64 result = 0;
    for(i32 i = 0; i < (i32)(N); ++i) result += abs(count[i] - T[i]);
    return 1.0 * result / L;
  }
  int check_connected() const {
    bitset<N> vis = 0;
    auto dfs = [&](auto dfs, int i) -> void {
      if(vis[i]) return;
      vis[i] = 1;
      for(i32 j = 0; j < (i32)(2); ++j) dfs(dfs, graph[i][j]);
    };
    dfs(dfs, 0);
    return vis.count();
  }
  void print() {
    for(i32 i = 0; i < (i32)(N); ++i) {
      for(i32 j = 0; j < (i32)(2); ++j) cout << graph[i][j] << ' ';
      cout << endl;
    }
  }
};
state solve_beam() {
  do { cerr << "main.cpp" ":" "170" "  {" << "T[0]" << "} = {"; debug_impl_seq(T[0]); cerr << endl << flush; } while(0);
  vector<int> TT(T, T+N);
  sort(begin(TT), end(TT));
  do { cerr << "main.cpp" ":" "173" "  {" << "TT" << "} = {"; debug_impl_seq(TT); cerr << endl << flush; } while(0);
  vector<beam_state> BEAM_A;
  vector<beam_state> NEW_BEAM;
  vector<int> I(N);
  iota(begin(I), end(I), 0);
  sort(begin(I), end(I), [&](int i, int j){
    return T[i]<T[j];
  });
  for(i32 i = 0; i < (i32)(8); ++i) for(i32 j = 0; j < (i32)(i); ++j) {
    BEAM_A.emplace_back(); BEAM_A.back().reset(I[N-1-i], I[N-1-j]);
  }
  const int BW = 160;
  vector<candidate> BEAM_B;
  for(i32 i = 0; i < (i32)(N-2); ++i) {
    BEAM_B.clear();
    auto best = *min_element(begin(BEAM_A), end(BEAM_A), [&](auto const& a, auto const& b) {
      return a.score < b.score;
    });
    do { cerr << "main.cpp" ":" "197" "  {" << "i, BEAM_A.size(), best.score, best.nopen" << "} = {"; debug_impl_seq(i, BEAM_A.size(), best.score, best.nopen); cerr << endl << flush; } while(0);
    int na = BEAM_A.size();
    for(i32 ia = 0; ia < (i32)(na); ++ia) {
      auto const& sa = BEAM_A[ia];
      int nreach2 = 0;
      static tuple<f64, int, int> reach2[MAX_OPEN*MAX_OPEN];
      for(i32 i1 = 0; i1 < (i32)(sa.nopen); ++i1) for(i32 i2 = 0; i2 < (i32)(i1); ++i2) {
        reach2[nreach2++] = make_tuple(sa.open[i1].x+sa.open[i2].x, i1, i2);
      }
      pdqsort(reach2, reach2+nreach2);
      int nreach3 = 0;
      tuple<f64, int, int, int> reach3[MAX_OPEN*MAX_OPEN*MAX_OPEN];
      for(i32 i1 = 0; i1 < (i32)(sa.nopen); ++i1) for(i32 i2 = 0; i2 < (i32)(i1); ++i2) for(i32 i3 = 0; i3 < (i32)(i2); ++i3) {
        reach3[nreach3++] = make_tuple(sa.open[i1].x+sa.open[i2].x+sa.open[i3].x, i1, i2, i3);
      }
      pdqsort(reach3, reach3+nreach3);
      for(i32 x = 0; x < (i32)(N); ++x) if(!sa.visited[x]) {
        if(sa.nopen+1 < MAX_OPEN) {
          BEAM_B.emplace_back(candidate {
              .ia = ia,
              .score = sa.score + abs(goalp[x]),
              .x = x,
              .i1 = -1,
              .i2 = -1,
              .i3 = -1,
              .i4 = -1,
              .val = 0,
              .nopen = sa.nopen+2,
            });
        }
        if(sa.nopen < MAX_OPEN) {
          int i = distance(sa.open,
                           lower_bound(sa.open, sa.open+sa.nopen,
                                       open_item { .x = goalp[x], .i = -1 } ));
          if(i>0) i -= 1;
          if(i+1 < sa.nopen
             && abs(sa.open[i+1].x-goalp[x]) < abs(sa.open[i].x-goalp[x])) {
            i += 1;
          }
          if(i < sa.nopen) {
            BEAM_B.emplace_back(candidate {
                .ia = ia,
                .score = sa.score + abs(sa.open[i].x - goalp[x]),
                .x = x,
                .i1 = i,
                .i2 = -1,
                .i3 = -1,
                .i4 = -1,
                .val = sa.open[i].x,
                .nopen = sa.nopen+1,
              });
          }
        }
        { int i = distance(reach2,
                           lower_bound(reach2, reach2+nreach2, make_tuple(goalp[x],-1,-1)));
          if(i>0) i -= 1;
          if(i+1 < nreach2
             && abs(get<0>(reach2[i+1])-goalp[x]) < abs(get<0>(reach2[i])-goalp[x])) {
            i += 1;
          }
          if(i < nreach2) {
            BEAM_B.emplace_back(candidate {
                .ia = ia,
                .score = sa.score + abs(get<0>(reach2[i]) - goalp[x]),
                .x = x,
                .i1 = get<1>(reach2[i]),
                .i2 = get<2>(reach2[i]),
                .i3 = -1,
                .i4 = -1,
                .val = get<0>(reach2[i]),
                .nopen = sa.nopen,
              });
          }
        }
        { int i = distance(reach3,
                           lower_bound(reach3,reach3+nreach3, make_tuple(goalp[x],-1,-1,-1)));
          if(i>0) i -= 1;
          if(i+1 < nreach3
             && abs(get<0>(reach3[i+1])-goalp[x]) < abs(get<0>(reach3[i])-goalp[x])) {
            i += 1;
          }
          if(i < nreach3) {
            BEAM_B.emplace_back(candidate {
                .ia = ia,
                .score = sa.score + abs(get<0>(reach3[i]) - goalp[x]),
                .x = x,
                .i1 = get<1>(reach3[i]),
                .i2 = get<2>(reach3[i]),
                .i3 = get<3>(reach3[i]),
                .i4 = -1,
                .val = get<0>(reach3[i]),
                .nopen = sa.nopen-1,
              });
          }
        }
      }
    }
    pdqsort(begin(BEAM_B), end(BEAM_B),
         [&](auto const& a, auto const& b) {
           return a.score + get_bonus(a.nopen, i) < b.score + get_bonus(b.nopen, i);
         });
    NEW_BEAM.clear();
    NEW_BEAM.reserve(BW);
    for(auto const& c : BEAM_B) {
      if(NEW_BEAM.size() == BW) break;
      auto h = BEAM_A[c.ia].h ^ visited_hash[c.x];
      if(i <= 95 && hash_table[h&HASH_MASK] == h) continue;
      hash_table[h&HASH_MASK] = h;
      NEW_BEAM.emplace_back();
      auto& sb = NEW_BEAM.back();
      sb = BEAM_A[c.ia];
      sb.h = h;
      sb.visited[c.x] = 1;
      sb.score = c.score;
      if(c.i1 != -1) sb.erase(c.i1, c.x);
      if(c.i2 != -1) sb.erase(c.i2, c.x);
      if(c.i3 != -1) sb.erase(c.i3, c.x);
      if(c.i4 != -1) sb.erase(c.i4, c.x);
      sb.add_open(c.x, c.val);
    }
    swap(BEAM_A, NEW_BEAM);
  }
  beam_state best_state;
  f64 best_score = 2.0;
  for(auto sa : BEAM_A) {
    sa.finish();
    if(sa.score < best_score) {
      best_score = sa.score;
      best_state = sa;
    }
  }
  do { cerr << "main.cpp" ":" "344" "  {" << "best_score" << "} = {"; debug_impl_seq(best_score); cerr << endl << flush; } while(0);
  do { cerr << "main.cpp" ":" "345" "  {" << "1e6 * (1-best_score)" << "} = {"; debug_impl_seq(1e6 * (1-best_score)); cerr << endl << flush; } while(0);
  state S;
  for(i32 i = 0; i < (i32)(N); ++i) {
    for(i32 j = 0; j < (i32)(2); ++j) {
      S.graph[i][j] = best_state.graph[2*i+j];
    }
  }
  return S;
}
void solve() {
  state S = solve_beam();
  do { cerr << "main.cpp" ":" "358" "  {" << "TIMER.elapsed()" << "} = {"; debug_impl_seq(TIMER.elapsed()); cerr << endl << flush; } while(0);
  f64 t0 = TIMER.elapsed();
  f64 remain = TL - t0;
  auto con_target = S.check_connected();
  do { cerr << "main.cpp" ":" "364" "  {" << "S.check_connected()" << "} = {"; debug_impl_seq(S.check_connected()); cerr << endl << flush; } while(0);
  { int iter = 0;
    state best_S = S;
    auto best_score = best_S.simulate();
    auto result_est = S.estimate();
    while(1) {
      iter += 1;
      if(iter % 512 == 0) {
        auto new_score = S.simulate();
        if(new_score < best_score) {
          best_score = new_score;
          do { cerr << "main.cpp" ":" "378" "  {" << "best_score" << "} = {"; debug_impl_seq(best_score); cerr << endl << flush; } while(0);
          best_S = S;
        }
        if(TIMER.elapsed() > t0 + remain/2) break;
      }
      int i = rng.random32(N);
      int j = rng.random32(N);
      if(i == j) continue;
      int a = rng.random32(2);
      int b = rng.random32(2);
      swap(S.graph[i][a], S.graph[j][b]);
      auto new_result = S.estimate();
      if((new_result-result_est) <= 2e-5 && S.check_connected() == con_target) {
        result_est = new_result;
      }else{
        swap(S.graph[i][a], S.graph[j][b]);
      }
    }
    do { cerr << "main.cpp" ":" "396" "  {" << "iter" << "} = {"; debug_impl_seq(iter); cerr << endl << flush; } while(0);
    S = best_S;
  }
  { auto result = S.simulate();
    do { cerr << "main.cpp" ":" "401" "  {" << "result" << "} = {"; debug_impl_seq(result); cerr << endl << flush; } while(0);
    int iter = 0;
    while(TIMER.elapsed() < TL) {
      iter += 1;
      int i = iter % N;
      swap(S.graph[i][0], S.graph[i][1]);
      auto new_result = S.simulate();
      if(new_result <= result) {
        if(new_result < result) do { cerr << "main.cpp" ":" "409" "  {" << "new_result" << "} = {"; debug_impl_seq(new_result); cerr << endl << flush; } while(0);
        result = new_result;
      }else{
        swap(S.graph[i][0], S.graph[i][1]);
      }
    }
    do { cerr << "main.cpp" ":" "415" "  {" << "1e6 * (1-result/2)" << "} = {"; debug_impl_seq(1e6 * (1-result/2)); cerr << endl << flush; } while(0);
    do { cerr << "main.cpp" ":" "416" "  {" << "iter" << "} = {"; debug_impl_seq(iter); cerr << endl << flush; } while(0);
  }
  // debug(result);
  S.print();
}
int main() {
  rng.reset(time(0));
  read();
  init();
  solve();
  cerr << "[DATA] time = " << (int)(TIMER.elapsed()*1000) << endl;
  return 0;
}
