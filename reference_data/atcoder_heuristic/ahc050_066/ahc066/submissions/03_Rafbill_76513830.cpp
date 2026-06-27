// -*- compile-command: "make" -*-
#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
 #pragma GCC target "tune=native"
 #include <bits/stdc++.h>
 #include <sys/time.h>
 #include <immintrin.h>
 #include <x86intrin.h>
 #include <boost/archive/iterators/binary_from_base64.hpp>
 #include <boost/archive/iterators/transform_width.hpp>
// INCLUDE <ext/pb_ds/assoc_container.hpp>
 #include <immintrin.h>
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

double get_param(const char* env_var) {
  char* ret = getenv(env_var);
  assert (ret != nullptr);
  return stod(std::string(ret));
}
const int NC = 8;
const i32 BIG = 1<<16;
const i32 VERYBIG = (int)(1<<29);
const i32 INF = VERYBIG;
timer TIMER;
const f32 TIME_SCALE = 1;
const f64 BEAM_TL = 0.9 * TIME_SCALE;
const f64 TL_CLEANUP_IFPOST = 1.2 * TIME_SCALE;
const f64 TL_IFPOST = 1.5 * TIME_SCALE;
const f64 TL_CLEANUP_NOPOST = 1.45 * TIME_SCALE;
const f64 TL_NOPOST = 1.95 * TIME_SCALE;
const f64 TL_POST = 1.95 * TIME_SCALE;
const f64 TL = 1.95 * TIME_SCALE;
const int MAXN = 20;
const int MAXM = 2*MAXN;
const int MAXNN = MAXN*MAXN;
int n, nn;
int m;
int t;
int srcs[MAXM];
int tgts[MAXM];
int srcs_and_tgts[2*MAXM];
bool is_src_tgt[MAXNN];
u8 tgti[MAXNN];
u8 srci[MAXNN];
u8 walls[MAXNN];
u8 graph[MAXNN];
array<int,4> dxy;
const char* dc = "FLRSPM";
const int MAXL = 48;
const int MAXL_by_n[21] = {
  32,32,32,32,32,
  32,32,32,32,32,
  32,32,32,32,32,
  32,40,40,40,44,
  48,
};
void read(){
  cin>>n>>m>>t;
  nn = n*n;
  dxy = {n, 1, -n, -1};
  cerr << "[DATA] n = " << n << endl;
  cerr << "[DATA] m = " << m << endl;
  cerr << "[DATA] nm = " << n*m << endl;
  cerr << "[DATA] nnm = " << n*n*m << endl;
  cerr << "[DATA] t = " << t << endl;
  for(i32 x = 0; x < (i32)(n); ++x) for(i32 y = 0; y < (i32)(n-1); ++y) {
    char b; cin>>b;
    if(b == '1') {
      walls[x*n+y] |= bit(1);
      walls[x*n+y+1] |= bit(3);
    }
  }
  for(i32 x = 0; x < (i32)(n-1); ++x) for(i32 y = 0; y < (i32)(n); ++y) {
    char b; cin>>b;
    if(b == '1') {
      walls[x*n+y] |= bit(0);
      walls[(x+1)*n+y] |= bit(2);
    }
  }
  for(i32 u = 0; u < (i32)(nn); ++u) graph[u] = 0;
  for(i32 x = 0; x < (i32)(n); ++x) for(i32 y = 0; y < (i32)(n); ++y) {
      if(x+1 < n) {
        graph[x*n+y] |= bit(0);
        graph[(x+1)*n+y] |= bit(2);
      }
      if(y+1 < n) {
        graph[x*n+y] |= bit(1);
        graph[x*n+(y+1)] |= bit(3);
      }
    }
  for(i32 u = 0; u < (i32)(nn); ++u) graph[u] &= ~walls[u];
  for(i32 u = 0; u < (i32)(m); ++u) {
    int x,y;
    cin>>x>>y; srcs[u] = x*n+y;
    cin>>x>>y; tgts[u] = x*n+y;
  }
  for(i32 i = 0; i < (i32)(nn); ++i) srci[i] = 255;
  for(i32 i = 0; i < (i32)(m); ++i) srci[srcs[i]] = i;
  for(i32 i = 0; i < (i32)(nn); ++i) tgti[i] = 255;
  for(i32 i = 0; i < (i32)(m); ++i) tgti[tgts[i]] = i;
  for(i32 i = 0; i < (i32)(m); ++i) srcs_and_tgts[i] = srcs[i];
  for(i32 i = 0; i < (i32)(m); ++i) srcs_and_tgts[i+m] = tgts[i];
  for(i32 i = 0; i < (i32)(2*m); ++i) is_src_tgt[srcs_and_tgts[i]] = 1;
}
int DO_F[MAXNN*4];
int DO_FN[8][MAXNN*4];
int DO_L[MAXNN*4];
int DO_R[MAXNN*4];
u64 hash_at[MAXNN*4];
u64 hash_to[MAXNN*4][MAXNN*4];
f64 entropy_count[MAXNN*4+1];
template<class F>
__attribute__((always_inline)) inline
void iter_moves(int vd, F&& f) {
  f(DO_F[vd], 0);
  f(DO_L[vd], 1);
  f(DO_R[vd], 2);
}
int dist[4*MAXNN][4*MAXNN];
int dist_T[4*MAXNN][4*MAXNN];
array<int,2> closest_to_src[MAXM][4*MAXNN];
void init_dist() {
  for(i32 u = 0; u < (i32)(4*nn); ++u) {
    static int Q[4*MAXNN];
    for(i32 v = 0; v < (i32)(4*nn); ++v) dist[u][v] = INF;
    int nq = 0;
    dist[u][u] = 0; Q[nq++] = u;
    for(i32 iq = 0; iq < (i32)(nq); ++iq) {
      int v = Q[iq];
      iter_moves(v, [&](int w, int) {
        if(dist[u][v]+1<dist[u][w]) {
          dist[u][w] = dist[u][v]+1;
          Q[nq++] = w;
        }
      });
    }
    do { if(!(nq == 4*nn)) { throw runtime_error("main.cpp" ":" "150" " Assertion failed: " "nq == 4*nn"); } } while(0);
  }
  for(i32 u = 0; u < (i32)(4*nn); ++u) for(i32 v = 0; v < (i32)(4*nn); ++v) dist_T[u][v] = dist[v][u];
  for(i32 u = 0; u < (i32)(m); ++u) {
    for(i32 v = 0; v < (i32)(4*nn); ++v) {
      int di = INF;
      for(i32 d = 0; d < (i32)(4); ++d) di = min(di, dist_T[4*srcs[u]+d][v]);
      closest_to_src[u][v] = {di,v};
    }
    sort(closest_to_src[u],closest_to_src[u]+4*nn);
  }
}
void init(){
  for(i32 u = 0; u < (i32)(nn); ++u) for(i32 d = 0; d < (i32)(4); ++d) {
    if(graph[u]&bit(d)) DO_F[4*u+d] = (4*(u+dxy[d])+d);
    else DO_F[4*u+d] = 4*u+d;
  }
  for(i32 u = 0; u < (i32)(nn); ++u) for(i32 d = 0; d < (i32)(4); ++d) {
    DO_L[4*u+d] = 4*u+(d+1)%4;
    DO_R[4*u+d] = 4*u+(d+3)%4;
  }
  for(i32 u = 0; u < (i32)(nn*4); ++u) DO_FN[0][u] = u;
  for(i32 i = 0; i < (i32)(7); ++i) for(i32 u = 0; u < (i32)(nn*4); ++u) DO_FN[i+1][u] = DO_F[DO_FN[i][u]];
  for(i32 u = 0; u < (i32)(nn*4); ++u) hash_at[u] = rng.randomInt64();
  for(i32 u = 0; u < (i32)(nn*4); ++u) for(i32 v = 0; v < (i32)(nn*4); ++v) hash_to[u][v] = rng.randomInt64();
  init_dist();
  for(i32 u = 0; u < (i32)(nn*4+1); ++u) {
    if(u == 0) entropy_count[u] = 0;
    else {
      f64 p = 1.0*u/(4*nn);
      entropy_count[u] = p * log(p);
    }
  }
}
int ans_size(vector<int> const& ans) {
  int msize = 0;
  int rmsize = -1;
  int len = 0;
  for(int x : ans) {
    if(x <= 3) { len += 1; if(rmsize != -1) rmsize += 1; }
    else if(x == 4) { len += msize; if(rmsize != -1) rmsize += msize; }
    else if(x == 5) {
      if(rmsize == -1) { rmsize = 0; }
      else { msize = rmsize; rmsize = -1; }
    }
  }
  return len;
}
vector<tuple<int, vector<int>, vector<int>>> solutions_to_reprocess;
int nreprocess = 0;
vector<int> best_ans;
int best_len = -1;
bool save_ans(vector<int> const& ans, int len) {
  if(best_ans.empty() || ans.size() < best_ans.size()) {
    if(ans_size(ans) > t) {
      cerr << "fail ans: " << ans.size() << " " << ans_size(ans) << "/" << t << endl;
      return false;
    }
    best_ans = ans;
    best_len = len;
    cerr << "new best: " << best_ans.size() << endl;
  }
  return true;
}
void print_best() {
  cerr << "[DATA] est_score = " << best_ans.size() << endl;
  cerr << "[DATA] len = " << best_len << endl;
  if(best_ans.empty()) cout << "BAD!" << endl;
  for(int i : best_ans) cout << dc[i] << '\n';
  cout << flush;
}
struct history_list {
  i32 prev;
  i32 move;
};
const int MAX_HISTORY = 1<<25;
history_list *history_pool;
int next_history = 0;
int make_history(history_list l) {
  history_pool[next_history] = l;
  return next_history++;
}
struct macro {
  int len;
  int h;
  int front, back;
  bool start_s;
  u16 to[MAXNN*4];
  void reset() {
    len = 0;
    for(i32 u = 0; u < (i32)(nn*4); ++u) to[u] = u;
    h = -1;
    front = back = 0;
    start_s = false;
  }
  f64 entropy() const {
    static int count[MAXNN*4];
    for(i32 u = 0; u < (i32)(nn*4); ++u) count[u] = 0;
    for(i32 u = 0; u < (i32)(nn*4); ++u) count[to[u]] += 1;
    f64 ret = 0;
    for(i32 u = 0; u < (i32)(nn*4); ++u) ret += entropy_count[count[u]];
    return - ret;
  }
  f64 eval_srcs() const {
    static int count[MAXNN*4];
    for(i32 u = 0; u < (i32)(nn*4); ++u) count[u] = 0;
    for(i32 u = 0; u < (i32)(nn*4); ++u) count[to[u]] += 1;
    f64 ret = 0;
    for(i32 u = 0; u < (i32)(m); ++u) {
      for(i32 v = 0; v < (i32)(nn*4); ++v) if(count[closest_to_src[u][v][1]]) {
        ret += closest_to_src[u][v][0];
        break;
      }
    }
    return ret;
  }
  u64 hash() const {
    u64 h = 0;
    for(i32 u = 0; u < (i32)(2*m); ++u) for(i32 d = 0; d < (i32)(4); ++d) h ^= hash_to[u*4+d][to[srcs_and_tgts[u]*4+d]];
    return h;
  }
  void copyheaderfrom(macro const& o) {
    len = o.len;
    h = o.h;
    front = o.front;
    back = o.back;
    start_s = o.start_s;
  }
  void copyfrom(macro const& o) {
    copyheaderfrom(o);
    for(i32 u = 0; u < (i32)(nn*4); ++u) to[u] = o.to[u];
  }
  void F(macro const& o) {
    copyheaderfrom(o);
    len += 1;
    h = make_history({h, 0});
    for(i32 u = 0; u < (i32)(nn*4); ++u) to[u] = DO_F[o.to[u]];
    back = 0;
  }
  void L(macro const& o) {
    copyheaderfrom(o);
    len += 1;
    h = make_history({h, 1});
    for(i32 u = 0; u < (i32)(nn*4); ++u) to[u] = DO_L[o.to[u]];
    back = 1;
  }
  void R(macro const& o) {
    copyheaderfrom(o);
    len += 1;
    h = make_history({h, 2});
    for(i32 u = 0; u < (i32)(nn*4); ++u) to[u] = DO_R[o.to[u]];
    back = 2;
  }
  void F_(macro const& o) {
    copyheaderfrom(o);
    len += 1;
    for(i32 u = 0; u < (i32)(nn*4); ++u) to[u] = o.to[DO_F[u]];
    h = make_history({h, 3});
    front = 0;
  }
  void L_(macro const& o) {
    copyheaderfrom(o);
    len += 1;
    for(i32 u = 0; u < (i32)(nn*4); ++u) to[u] = o.to[DO_L[u]];
    h = make_history({h, 4});
    front = 1;
  }
  void R_(macro const& o) {
    copyheaderfrom(o);
    len += 1;
    for(i32 u = 0; u < (i32)(nn*4); ++u) to[u] = o.to[DO_R[u]];
    h = make_history({h, 5});
    front = 2;
  }
  void do_move(int mv) {
    static macro M;
    if(mv == 0) M.F(*this);
    if(mv == 1) M.L(*this);
    if(mv == 2) M.R(*this);
    if(mv == 3) M.F_(*this);
    if(mv == 4) M.L_(*this);
    if(mv == 5) M.R_(*this);
    copyfrom(M);
  }
};
int
bfs2_with_macro
(int src, int tgt, macro const& M)
{
  // TODO: optimize !!!
  static int QS[4][MAXNN*4];
  static int D[MAXNN*4];
  static int last[MAXNN*4];
  static int date = 0;
  date += 1;
  int nq[4] = {0,0,0,0};
  for(i32 d = 0; d < (i32)(4); ++d) {
    int cost = 0;
    int u = src*4+d;
    { int v = M.to[u];
      if(last[v] != date || cost < D[v]) {
        last[v] = date;
        D[v] = cost;
        QS[cost][nq[cost]++] = v;
      }
    }
    { int v = DO_F[u];
      if(last[v] != date || cost+1 < D[v]) {
        last[v] = date;
        D[v] = cost+1;
        QS[cost+1][nq[cost+1]++] = v;
      }
    }
  }
  for(int d = 0; d < 4 || nq[d%4] != 0; d++) {
    int* Q0 = QS[d%4];
    int* Q1 = QS[(d+1)%4];
    int* Q2 = QS[(d+2)%4];
    int& nq0 = nq[d%4];
    int& nq1 = nq[(d+1)%4];
    int& nq2 = nq[(d+2)%4];
    for(i32 iq = 0; iq < (i32)(nq0); ++iq) {
      int u = Q0[iq];
      if(u/4 == tgt) return d;
      if(D[u] != d) continue;
      iter_moves(u, [&](int v, int) __attribute__((always_inline)) {
        if(last[v] != date || d+1 < D[v]) {
          last[v] = date;
          D[v] = d + 1;
          Q1[nq1++] = v;
        }
      });
      { int v = M.to[u];
        if(last[v] != date || d+2 < D[v]) {
          last[v] = date;
          D[v] = d + 2;
          Q2[nq2++] = v;
        }
      }
    }
    nq0 = 0;
  }
  do { if(!(false)) { throw runtime_error("main.cpp" ":" "415" " Assertion failed: " "false"); } } while(0);
  return 0;
}
static inline u64 hor_u64_avx2(__m256i v) {
  __m128i lo = _mm256_castsi256_si128(v);
  __m128i hi = _mm256_extracti128_si256(v, 1);
  __m128i s = _mm_or_si128(lo, hi);
  return (u64)_mm_cvtsi128_si64(s) | (u64)_mm_extract_epi64(s, 1);
}
f64 eval2_tgts(macro& M) {
  alignas(64)
  static u64 seen[MAXNN*4];
  alignas(64)
  static u64 visit[4][MAXNN*4];
  alignas(64)
  static u64 cand1[MAXNN*4];
  alignas(64)
  static u64 cand2[MAXNN*4];
  alignas(64)
  static u64 tgtmask[MAXNN];
  for(i32 u = 0; u < (i32)(4*nn); ++u) seen[u] = 0;
  for(i32 r = 0; r < (i32)(4); ++r) for(i32 u = 0; u < (i32)(4*nn); ++u) visit[r][u] = 0;
  for(i32 u = 0; u < (i32)(nn); ++u) tgtmask[u] = 0;
  u64 rem = 0;
  for(i32 q = 0; q < (i32)(m); ++q) {
    u64 b = bit(q);
    rem |= b;
    tgtmask[tgts[q]] |= b;
  }
  for(i32 q = 0; q < (i32)(m); ++q) {
    u64 b = bit(q);
    for(i32 d = 0; d < (i32)(4); ++d) {
      int u = srcs[q]*4 + d;
      int v = M.to[u];
      if(!(seen[v] & b)) {
        seen[v] |= b;
        visit[0][v] |= b;
      }
    }
  }
  for(i32 q = 0; q < (i32)(m); ++q) {
    u64 b = bit(q);
    for(i32 d = 0; d < (i32)(4); ++d) {
      int u = srcs[q]*4 + d;
      int v = DO_F[u];
      if(!(seen[v] & b)) {
        seen[v] |= b;
        visit[1][v] |= b;
      }
    }
  }
  int ret = 0;
  __m256i zerov = _mm256_setzero_si256();
  alignas(32) u64 tmpBits[4];
  for(int di = 0; rem; di += 1){
    int r = di&3;
    // FOR(u, 4*nn) {
    //   u64 bits = visit[r][u] & rem;
    //   visit[r][u] = 0;
    //   u64 hit = bits & tgtmask[u / 4];
    //   ret += di * popcount(hit);
    //   rem &= ~hit;
    //   bits &= ~hit;
    //   iter_moves(u, [&](int v, int) ALWAYS_INLINE {
    //     cand1[v] |= bits;
    //   });
    //   { int v = M.to[u];
    //     cand2[M.to[u]] |= bits;
    //   }
    // }
    // FOR(v, 4*nn) {
    //   u64 a = cand1[v] & ~seen[v];
    //   seen[v] |= a;
    //   visit[(di+1)&3][v] |= a;
    //   u64 b = cand2[v] & ~seen[v];
    //   seen[v] |= b;
    //   visit[(di+2)&3][v] |= b;
    //   cand1[v] = cand2[v] = 0;
    // }
    for(int u = 0; u < 4*nn; u += 4) {
      __m256i vis = _mm256_load_si256((const __m256i*)(visit[r] + u));
      _mm256_store_si256((__m256i*)(visit[r] + u), zerov);
      __m256i remv = _mm256_set1_epi64x((long long)rem);
      __m256i bitsv = _mm256_and_si256(vis, remv);
      __m256i tgtv = _mm256_set1_epi64x((long long)tgtmask[u/4]);
      __m256i hitv = _mm256_and_si256(bitsv, tgtv);
      u64 allhit = hor_u64_avx2(hitv);
      ret += di * popcount(allhit);
      rem &= ~allhit;
      _mm256_store_si256((__m256i*)tmpBits, bitsv);
      for(i32 d = 0; d < (i32)(4); ++d) {
        u64 bits = tmpBits[d] & rem;
        iter_moves(u+d, [&](int v, int) __attribute__((always_inline)) {
          cand1[v] |= bits;
        });
        {
          int v = M.to[u+d];
          cand2[v] |= bits;
        }
      }
    }
    const int r1 = (di+1) & 3;
    const int r2 = (di+2) & 3;
    for(int v = 0; v < 4*nn; v += 4) {
      __m256i seenv = _mm256_load_si256((const __m256i*)&seen[v]);
      __m256i c1v = _mm256_load_si256((const __m256i*)&cand1[v]);
      __m256i c2v = _mm256_load_si256((const __m256i*)&cand2[v]);
      __m256i v1v = _mm256_load_si256((const __m256i*)&visit[r1][v]);
      __m256i v2v = _mm256_load_si256((const __m256i*)&visit[r2][v]);
      __m256i av = _mm256_andnot_si256(seenv, c1v);
      seenv = _mm256_or_si256(seenv, av);
      v1v = _mm256_or_si256(v1v, av);
      __m256i bv = _mm256_andnot_si256(seenv, c2v);
      seenv = _mm256_or_si256(seenv, bv);
      v2v = _mm256_or_si256(v2v, bv);
      _mm256_store_si256((__m256i*)&seen[v], seenv);
      _mm256_store_si256((__m256i*)&visit[r1][v], v1v);
      _mm256_store_si256((__m256i*)&visit[r2][v], v2v);
      _mm256_store_si256((__m256i*)&cand1[v], zerov);
      _mm256_store_si256((__m256i*)&cand2[v], zerov);
    }
  }
  return ret;
}
f64 eval(macro& M) {
  // int ret = 0;
  // FOR(u, m) {
  //   ret += bfs2_with_macro(srcs[u], tgts[u], M);
  // }
  // runtime_assert(eval2_tgts(M) == ret);
  // return ret+M.len;
  f64 ret = 0;
  ret += eval2_tgts(M);
  ret += M.len;
  ret += 0.10 * M.eval_srcs();
  return ret;
}
vector<int> get_move_seq(int h) {
  vector<int> AMS;
  while(h != -1) {
    auto e = history_pool[h];
    AMS.push_back(e.move);
    h = e.prev;
  }
  reverse(begin(AMS), end(AMS));
  return AMS;
}
vector<int> flatten_move_seq(vector<int> const& MS) {
  vector<int> pref, suff;
  for(int i : MS) {
    if(i < 3) suff.push_back(i);
    else pref.push_back(i-3);
  }
  reverse(begin(pref), end(pref)); pref.insert(end(pref),begin(suff), end(suff));
  return pref;
}
const int MAXBW = 1<<13;
const int MAXMACROS = 16 * MAXBW;
macro *MACROS;
int BEAMA[MAXBW];
int BEAMB[MAXBW * 6];
f64 BEAMH[MAXBW * 6];
int BEAMI[MAXBW * 6];
vector<tuple<f64,int>>
solve_beam() {
  vector<tuple<f64,int>> OUT;
  int nm = 1;
  int na = 1;
  MACROS[0].reset();
  BEAMA[0] = 0;
  unordered_set<u64> HS;
  for(i32 len = (1); len <= (i32)(MAXL_by_n[n]-1); ++len) {
    f64 t0 = TIMER.elapsed();
    f64 t1 = t0 + (BEAM_TL - t0) / (MAXL_by_n[n] - len);
    int nb = 0;
    for(i32 ia = 0; ia < (i32)(na); ++ia) {
      int indexa = BEAMA[ia];
      auto push = [&]() {
        BEAMB[nb] = nm;
        BEAMI[nb] = nb;
        BEAMH[nb] = eval(MACROS[nm]);
        OUT.emplace_back(BEAMH[nb], MACROS[nm].h);
        nm += 1;
        nm %= MAXMACROS;
        nb += 1;
      };
      if(TIMER.elapsed() > t1) break;
      MACROS[nm].F(MACROS[indexa]);
      if(HS.insert(MACROS[nm].hash()).second) push();
      if(MACROS[indexa].back == 0) {
        if(TIMER.elapsed() > t1) break;
        MACROS[nm].L(MACROS[indexa]);
        if(HS.insert(MACROS[nm].hash()).second) push();
      }
      if(MACROS[indexa].back == 0) {
        if(TIMER.elapsed() > t1) break;
        MACROS[nm].R(MACROS[indexa]);
        if(HS.insert(MACROS[nm].hash()).second) push();
      }
      if(TIMER.elapsed() > t1) break;
      MACROS[nm].F_(MACROS[indexa]);
      if(HS.insert(MACROS[nm].hash()).second) push();
      if(MACROS[indexa].front == 0) {
        if(TIMER.elapsed() > t1) break;
        MACROS[nm].L_(MACROS[indexa]);
        if(HS.insert(MACROS[nm].hash()).second) push();
      }
      if(MACROS[indexa].front == 0) {
        if(TIMER.elapsed() > t1) break;
        MACROS[nm].R_(MACROS[indexa]);
        if(HS.insert(MACROS[nm].hash()).second) push();
      }
    }
    rng.shuffle(BEAMI, BEAMI+nb);
    pdqsort(BEAMI, BEAMI+nb, [&](int i, int j){
      return BEAMH[i] < BEAMH[j];
    });
    na = 0;
    for(i32 i = 0; i < (i32)(nb); ++i) {
      BEAMA[na] = BEAMB[BEAMI[i]];
      na += 1;
      if(na == MAXBW) break;
    }
    cerr
      << "len = " << len
      << ", beam_size = " << na
      << ", costs = " << BEAMH[BEAMI[0]] << " .. " << BEAMH[BEAMI[nb-1]]
      << endl;
  }
  // pdqsort(all(OUT));
  return OUT;
}
void
bfs3
(int src, array<int,2>* OUT, array<array<u16,3>,2>* FOUT,
 macro const& M, bool is_carrying, bool with_s)
{
  static int QS[4][MAXNN*4];
  for(i32 i = 0; i < (i32)(nn*4); ++i) OUT[i][with_s] = INF;
  int nq[4] = {0,0,0,0};
  for(i32 d = 0; d < (i32)(4); ++d) {
    int cost;
    if(d == 0) cost = 0;
    else if(d == 2) cost = 2;
    else cost = 1;
    int u = (src/4)*4 + ((src+d)%4);
    if(with_s) {
      int v = M.to[u]; int mv = 4+NC+with_s;
      if(cost < OUT[v][with_s]/BIG) {
        OUT[v][with_s] = cost*BIG + (cost+1+M.len);
        QS[cost][nq[cost]++] = v;
        FOUT[v][with_s] = {(u16)0xFFFF,(u16)mv,(u16)(2*d)};
      }
    }else{
      int v = M.to[u]; int mv = 4+NC+with_s;
      if(cost+1 < OUT[v][with_s]/BIG) {
        OUT[v][with_s] = (cost+1)*BIG+(cost+1+M.len);
        QS[cost+1][nq[cost+1]++] = v;
        FOUT[v][with_s] = {(u16)0xFFFF,(u16)mv,(u16)(2*d+1)};
      }
    }
    { int v = DO_F[u]; int mv = 0;
      if(cost+1 < OUT[v][with_s]/BIG) {
        OUT[v][with_s] = (cost+1)*BIG+(cost+1);
        QS[cost+1][nq[cost+1]++] = v;
        FOUT[v][with_s] = {(u16)0xFFFF,(u16)mv,(u16)(2*d+1)};
      }
    }
  }
  for(int d = 0; d < 4 || nq[d%4] != 0; d++) {
    int* Q0 = QS[d%4];
    int* Q1 = QS[(d+1)%4];
    int* Q2 = QS[(d+2)%4];
    int& nq0 = nq[d%4];
    int& nq1 = nq[(d+1)%4];
    int& nq2 = nq[(d+2)%4];
    for(i32 iq = 0; iq < (i32)(nq0); ++iq) {
      int u = Q0[iq];
      if(OUT[u][with_s]/BIG != d) continue;
      iter_moves(u, [&](int v, int mv) __attribute__((always_inline)) {
        if(d+1 < OUT[v][with_s]/BIG) {
          OUT[v][with_s] = OUT[u][with_s] + BIG + 1;
          FOUT[v][with_s] = {(u16)u,(u16)mv,0};
          Q1[nq1++] = v;
        }
      });
      if(with_s) {
        if(!is_carrying && !is_src_tgt[u/4]) {
          int v = M.to[u]; int mv = 4 + NC + with_s;
          if(d + 1 < OUT[v][with_s]/BIG) {
            OUT[v][with_s] = OUT[u][with_s] + BIG + (M.len+1);
            FOUT[v][with_s] = {(u16)u,(u16)mv,0};
            Q1[nq1++] = v;
          }
        }else{
          int v = M.to[u]; int mv = 4 + NC + with_s;
          if(d+2 < OUT[v][with_s]/BIG) {
            OUT[v][with_s] = OUT[u][with_s] + 2*BIG + (M.len+2);
            FOUT[v][with_s] = {(u16)u,(u16)mv,1};
            Q2[nq2++] = v;
          }
        }
      }else{
        int v = M.to[u]; int mv = 4 + NC + with_s;
        if(d+1 < OUT[v][with_s]/BIG) {
          OUT[v][with_s] = OUT[u][with_s] + BIG + M.len;
          FOUT[v][with_s] = {(u16)u,(u16)mv,0};
          Q1[nq1++] = v;
        }
      }
    }
    nq0 = 0;
  }
}
alignas(64)
array<int,2> gdist0[MAXNN*4*NC];
array<array<int,3>,2> gfrom0[MAXNN*4*NC];
void
bfs4
(int src, macro const& M, vector<int> const& MS0, bool with_s)
{
  auto MS = flatten_move_seq(MS0);
  min_queue<array<int,3>> Q;
  for(i32 i = 0; i < (i32)(nn*4*NC); ++i) gdist0[i][with_s] = INF;
  for(i32 i = 0; i < (i32)(nn*4*NC); ++i) gfrom0[i][with_s] = {0xFFFF,0xFFFF,0xFFFF};
  vector<int> lens_from(NC-1, with_s ? 1 : 0);
  lens_from[0] += M.len;
  for(i32 c = (1); c <= (i32)(NC-2); ++c) {
    int i = 0;
    while(i < (int)MS.size()) {
      bool pat = true;
      for(i32 j = 0; j < (i32)(c); ++j) if(i+j >= (int)MS.size() || MS[i+j] != 0) pat = false;
      if(pat) {
        lens_from[c] += 1;
        i += c;
      }else{
        lens_from[c] += 1;
        i += 1;
      }
    }
  }
  auto add = [&](int d, int u, int c, array<int,3> f) {
    if(d < gdist0[NC*u+c][with_s]) {
      gdist0[NC*u+c][with_s] = d;
      Q.push({d,u,c});
      gfrom0[NC*u+c][with_s] = f;
    }
  };
  add(0,src,0,{0xFFFF,0xFFFF,0xFFFF});
  while(!Q.empty()) {
    auto [d,u,c] = Q.top(); Q.pop();
    if(d != gdist0[u*NC+c][with_s]) continue;
    iter_moves(u, [&](int v, int mv) __attribute__((always_inline)) {
      add(d+BIG+1, v, c, {NC*u+c, mv, 0});
    });
    if(c == 0) {
      for(i32 nc = (1); nc <= (i32)(NC-2); ++nc) {
        add(d+BIG*(2+nc)+nc, DO_FN[nc][u],nc, {NC*u+c, 4+nc, 0});
      }
      add(d+BIG*(2+M.len+with_s)+(M.len+with_s), u,NC-1, {NC*u+c, 255, 0});
    }else if(c == NC-1) {
      if(with_s) {
        add(d+BIG*(1+is_src_tgt[u/4])+(M.len+with_s)+is_src_tgt[u/4],
            M.to[u],NC-1, {NC*u+c, 4+NC+with_s, is_src_tgt[u/4]});
      }else{
        add(d+BIG+(M.len+with_s), M.to[u],NC-1, {NC*u+c, 4+NC+with_s, 0});
      }
    }else{
      add(d+BIG+c, DO_FN[c][u],c, {NC*u+c, 4+c, 0});
      add(d+BIG*(2+lens_from[c])+(M.len+with_s),u,NC-1, {NC*u+c, 255, 0});
    }
  }
}
alignas(64)
array<int,2> gdist[8*MAXM][4*MAXNN];
array<array<u16,3>,2> gfrom[8*MAXM][4*MAXNN];
bool solve2(vector<int> const& MS, bool do_cleanup) {
  static macro M;
  M.reset();
  for(i32 i = 0; i < (i32)(MS.size()); ++i) M.do_move(MS[i]);
  for(i32 mi = 0; mi < (i32)(2); ++mi) {
    if(TIMER.elapsed() > TL) return false;
    bfs4(1, M, MS, mi);
    for(i32 i = 0; i < (i32)(m); ++i) for(i32 d = 0; d < (i32)(4); ++d) {
      if(TIMER.elapsed() > TL) return false;
      bfs3(4*srcs[i]+d, gdist[4*i+d], gfrom[4*i+d], M, 1, mi);
    }
    for(i32 i = 0; i < (i32)(m); ++i) for(i32 d = 0; d < (i32)(4); ++d) {
      if(TIMER.elapsed() > TL) return false;
      bfs3(4*tgts[i]+d, gdist[4*(i+m)+d], gfrom[4*(i+m)+d], M, 0, mi);
    }
  }
  // TODO: check that the actual size is exactly the size given to cleanup.
  auto cleanup_score = [&](int r) __attribute__((always_inline)) -> int {
    int a = r/BIG;
    int b = r%BIG;
    if(!do_cleanup) return a;
    if(b > t) {
      return (b-t) * BIG + a;
    }else{
      return a;
    }
  };
  auto eval_route = [&](int* route){
    int at = -1;
    __m256i dpv = _mm256_setzero_si256();
    // const __m256i idx   = _mm256_setr_epi32(1, 0, 3, 2, 5, 4, 7, 6);
    const __m256i one = _mm256_set1_epi32(1*BIG+1);
    const __m256i three = _mm256_set1_epi32(3*BIG+1);
    auto step = [&](int to) __attribute__((always_inline)) {
      const int src = srcs_and_tgts[to];
      if(at == -1) {
        dpv = _mm256_setr_epi32
          (gdist0[(src*4 + 0) * NC + NC - 1][0],
           gdist0[(src*4 + 0) * NC + NC - 1][1],
           gdist0[(src*4 + 1) * NC + NC - 1][0],
           gdist0[(src*4 + 1) * NC + NC - 1][1],
           gdist0[(src*4 + 2) * NC + NC - 1][0],
           gdist0[(src*4 + 2) * NC + NC - 1][1],
           gdist0[(src*4 + 3) * NC + NC - 1][0],
           gdist0[(src*4 + 3) * NC + NC - 1][1]);
      } else {
        const int at4 = 4*at;
        const int src4 = 4*src;
        const __m128i lo = _mm256_castsi256_si128(dpv);
        const __m128i hi = _mm256_extracti128_si256(dpv, 1);
        const __m256i dp0 = _mm256_broadcastq_epi64(lo);
        const __m256i dp1 = _mm256_broadcastq_epi64(_mm_srli_si128(lo, 8));
        const __m256i dp2 = _mm256_broadcastq_epi64(hi);
        const __m256i dp3 = _mm256_broadcastq_epi64(_mm_srli_si128(hi, 8));
        const __m256i gd0 = _mm256_load_si256((const __m256i*)&gdist[at4+0][src4][0]);
        const __m256i gd1 = _mm256_load_si256((const __m256i*)&gdist[at4+1][src4][0]);
        const __m256i gd2 = _mm256_load_si256((const __m256i*)&gdist[at4+2][src4][0]);
        const __m256i gd3 = _mm256_load_si256((const __m256i*)&gdist[at4+3][src4][0]);
        __m256i v = _mm256_add_epi32(dp0, gd0);
        v = _mm256_min_epi32(v, _mm256_add_epi32(dp1, gd1));
        v = _mm256_min_epi32(v, _mm256_add_epi32(dp2, gd2));
        v = _mm256_min_epi32(v, _mm256_add_epi32(dp3, gd3));
        v = _mm256_add_epi32(v, one);
        // const __m256i a = _mm256_permutevar8x32_epi32(v, idx);
        const __m256i a = _mm256_shuffle_epi32(v, 0xB1);
        dpv = _mm256_min_epi32(v, _mm256_add_epi32(a, three));
      }
      at = to;
    };
    for(i32 i = 0; i < (i32)(m); ++i) {
      step(route[i]);
      step(route[i]+m);
    }
    const __m128i lo = _mm256_castsi256_si128(dpv);
    const __m128i hi = _mm256_extracti128_si256(dpv, 1);
    __m128i x = _mm_min_epi32(lo, hi);
    x = _mm_min_epi32(x, _mm_shuffle_epi32(x, _MM_SHUFFLE(2, 3, 0, 1)));
    x = _mm_min_epi32(x, _mm_shuffle_epi32(x, _MM_SHUFFLE(1, 0, 3, 2)));
    return cleanup_score(_mm_cvtsi128_si32(x));
  };
  static int route[MAXM];
  static int tmp_route[MAXM];
  static int best_route[MAXM];
  for(i32 i = 0; i < (i32)(m); ++i) route[i] = i;
  rng.shuffle(route, route+m);
  int score = eval_route(route);
  for(i32 i = 0; i < (i32)(m); ++i) best_route[i] = route[i];
  int best_score = score;
  const int ITERS_by_m[41] = {
    0,0,0,0,0,
    2,4,8,8,8, 8,8,12,12,14,
    14,18,18,34,34, 40,40,40,42,44,
    44,44,44,54,54, 54,54,54,54,54,
    54,54,54,54,54, 54
  };
  const int ITERS = 1000 * ITERS_by_m[m];
  if(m <= 7) {
    for(i32 i = 0; i < (i32)(m); ++i) route[i] = i;
    int count = 0;
    do {
      int score = eval_route(route);
      if(score < best_score) {
        best_score = score;
        count = 0;
      }
      if(score == best_score) {
        count += 1;
        if(rng.random32(count) == 0) {
          for(i32 i = 0; i < (i32)(m); ++i) best_route[i] = route[i];
        }
      }
    } while(next_permutation(route, route+m));
  }else{
    const f32 temp0 = 4.0;
    const f32 temp1 = 0.8;
    int count = 0;
    for(i32 iter = 0; iter < (i32)(ITERS); ++iter) {
      if(iter % 1024 == 0) {
        if(TIMER.elapsed() > TL) break;
      }
      f32 done = 1.0*iter/ITERS;
      f32 temp = temp0*pow(temp1/temp0, done);
      { for(i32 i = 0; i < (i32)(m); ++i) tmp_route[i] = route[i];
        i32 i = rng.random32(m+1);
        i32 j = rng.random32(m+1);
        i32 k = rng.random32(m+1);
        if(i>j) swap(i, j);
        if(j>k) swap(j, k);
        if(i>j) swap(i, j);
        if(i == j || j == k) continue;
        rotate(tmp_route+i, tmp_route+j, tmp_route+k);
      }
      int new_score = eval_route(tmp_route);
      int delta = new_score - score;
      if(delta <= 0 || delta < temp * rng.randomDouble()) {
        score = new_score;
        for(i32 i = 0; i < (i32)(m); ++i) route[i] = tmp_route[i];
      }
      if(score < best_score) {
        best_score = score;
        for(i32 i = 0; i < (i32)(m); ++i) best_route[i] = route[i];
      }
    }
  }
  auto reconstruct = [&](int* route){
    int at = -1;
    array<int,2> dp[4][2];
    array<int,2> newdp[4][2];
    auto make_moves0 = [&](int mi, int h, int y) {
      vector<int> stack;
      while(1) {
        auto [z,mv,s] = gfrom0[y][mi];
        if(z == 0xFFFF) break;
        y = z;
        stack.push_back(mv);
        if(s) stack.push_back(3);
      }
      reverse(begin(stack), end(stack));
      for(int mv : stack) if(mv != 255) h = make_history({h,mv});
      return h;
    };
    auto make_moves = [&](int mi, int h, int x, int y) {
      vector<int> stack;
      while(y != 0xFFFF) {
        auto [z,mv,s] = gfrom[x][y][mi];
        y = z;
        stack.push_back(mv);
        if(s) {
          if(s&1) {
            stack.push_back(3);
          }
          s /= 2;
          if(s == 0) { }
          if(s == 1) { stack.push_back(1); }
          if(s == 2) { stack.push_back(1); stack.push_back(1); }
          if(s == 3) { stack.push_back(2); }
        }
      }
      reverse(begin(stack), end(stack));
      for(int mv : stack) if(mv != 255) h = make_history({h,mv});
      return h;
    };
    auto step = [&](int to) {
      if(at == -1) {
        for(i32 d = 0; d < (i32)(4); ++d) for(i32 mi = 0; mi < (i32)(2); ++mi) {
          dp[d][mi] = { gdist0[(4*srcs_and_tgts[to]+d)*NC+NC-1][mi],
                        make_moves0(mi, -1, (4*srcs_and_tgts[to]+d)*NC+NC-1) };
        }
      }else{
        for(i32 d2 = 0; d2 < (i32)(4); ++d2) for(i32 mi = 0; mi < (i32)(2); ++mi) {
          newdp[d2][mi] = {INF, -1};
        }
        for(i32 d1 = 0; d1 < (i32)(4); ++d1) for(i32 d2 = 0; d2 < (i32)(4); ++d2) for(i32 mi = 0; mi < (i32)(2); ++mi) {
          newdp[d2][mi] =
            min
            (newdp[d2][mi],
             {dp[d1][mi][0] + gdist[4*at+d1][4*srcs_and_tgts[to]+d2][mi],
              make_moves(mi, dp[d1][mi][1], 4*at+d1, 4*srcs_and_tgts[to]+d2)
             });
        }
        for(i32 d = 0; d < (i32)(4); ++d) for(i32 mi = 0; mi < (i32)(2); ++mi) dp[d][mi] = {newdp[d][mi][0] + BIG+1, newdp[d][mi][1]};
        for(i32 d = 0; d < (i32)(4); ++d) for(i32 i = 0; i < (i32)(2); ++i) dp[d][i] = min(dp[d][i], {dp[d][i^1][0] + 3*BIG+1, dp[d][i^1][1]});
      }
      at = to;
    };
    for(i32 i = 0; i < (i32)(m); ++i) {
      step(route[i]);
      step(route[i]+m);
    }
    array<int,2> ret = {INF,-1};
    for(i32 d = 0; d < (i32)(4); ++d) for(i32 mi = 0; mi < (i32)(2); ++mi) ret = min(ret, dp[d][mi]);
    ret[0] = cleanup_score(ret[0]);
    do { if(!(best_score == ret[0])) { throw runtime_error("main.cpp" ":" "1088" " Assertion failed: " "best_score == ret[0]"); } } while(0);
    int h = ret[1];
    vector<int> ans;
    while(h != -1) {
      auto e = history_pool[h];
      ans.push_back(e.move);
      h = e.prev;
    }
    reverse(begin(ans), end(ans));
    vector<int> curmacro;
    vector<int> rans;
    auto changemacro = [&](vector<int> const& newmacro) {
      if(newmacro.size()+1 == curmacro.size()) {
        for(i32 i = 0; i < (i32)(newmacro.size()); ++i) do { if(!(newmacro[i] == curmacro[i+1])) { throw runtime_error("main.cpp" ":" "1106" " Assertion failed: " "newmacro[i] == curmacro[i+1]"); } } while(0);
        do { if(!(curmacro[0] == 3)) { throw runtime_error("main.cpp" ":" "1107" " Assertion failed: " "curmacro[0] == 3"); } } while(0);
        rans.push_back(3);
        rans.push_back(4);
      }else{
        int i = 0;
        while(i < (int)newmacro.size()) {
          bool pat = !curmacro.empty();
          for(i32 j = 0; j < (i32)(curmacro.size()); ++j) {
            if(i+j >= (int)newmacro.size() || newmacro[i+j] != curmacro[j]) {
              pat = false;
            }
          }
          if(pat) {
            rans.push_back(4);
            i += curmacro.size();
          }else{
            rans.push_back(newmacro[i]);
            i += 1;
          }
        }
      }
      curmacro = newmacro;
    };
    for(int x : ans) {
      if(x >= 4) {
        x -= 4;
        vector<int> newmacro;
        if(x < NC) {
          for(i32 i = 0; i < (i32)(x); ++i) newmacro.push_back(0);
        }else{
          x -= NC;
          do { if(!(x == 0 || x == 1)) { throw runtime_error("main.cpp" ":" "1139" " Assertion failed: " "x == 0 || x == 1"); } } while(0);
          newmacro = flatten_move_seq(MS);
          if(x) newmacro.insert(begin(newmacro), 3);
        }
        if(newmacro == curmacro) {
          rans.push_back(4);
        }else{
          rans.push_back(5);
          changemacro(newmacro);
          rans.push_back(5);
        }
      }else{
        rans.push_back(x);
      }
    }
    rans.push_back(3);
    if((best_ans.empty() || best_score < (int)best_ans.size())
       && best_score < (int)rans.size()) {
      do{}while(0);
    }
    do { if(!(best_score >= (int)rans.size())) { throw runtime_error("main.cpp" ":" "1160" " Assertion failed: " "best_score >= (int)rans.size()"); } } while(0);
    if(ans_size(rans) <= t) {
      solutions_to_reprocess.emplace_back(rans.size(), MS, vector<int>(route, route+m));
    }
    return save_ans(rans, M.len);
  };
  return reconstruct(best_route);
}
array<int,2> calc_macro_dist(vector<int> const& curmacro, vector<int> const& newmacro) {
  vector<array<int,2>> D(newmacro.size()+1, {INF,-1});
  D[0] = {2, make_history({-1, 5})};
  for(i32 i = 0; i < (i32)(newmacro.size()); ++i) {
    { bool pat = !curmacro.empty();
      for(i32 j = 0; j < (i32)(curmacro.size()); ++j) {
        if(i+j >= (int)newmacro.size() || newmacro[i+j] != curmacro[j]) {
          pat = false;
        }
      }
      if(pat) {
        D[i+curmacro.size()] =
          min(D[i+curmacro.size()],
              {D[i][0]+1, make_history({D[i][1], 4})});
      }
    }
    if(!curmacro.empty() && curmacro[0] == 3) {
      bool pat = !curmacro.empty();
      for(i32 j = 0; j < (i32)((int)curmacro.size()-1); ++j) {
        if(i+j >= (int)newmacro.size() || newmacro[i+j] != curmacro[1+j]) {
          pat = false;
        }
      }
      if(pat) {
        D[i+curmacro.size()-1] =
          min(D[i+curmacro.size()-1],
              {D[i][0]+2, make_history({make_history({D[i][1], 3}), 4})});
      }
    }
    D[i+1] =
      min(D[i+1],
          {D[i][0]+1, make_history({D[i][1], newmacro[i]})});
  }
  return {D[newmacro.size()][0], make_history({D[newmacro.size()][1], 5})};
}
void reprocess(vector<int> MS0, vector<int> const& route) {
  auto MS = flatten_move_seq(MS0);
  vector<vector<int>> seqs;
  seqs.push_back({});
  int opt = 1;
  // option 1: all left subsequences of 3M and M, plus the ones from the beam search history
  if(opt == 1) {
    for(i32 c = (2); c <= (i32)(NC-1); ++c) {
      vector<int> seq(c, 0);
      seqs.push_back(seq);
    }
    vector<int> cur;
    for(int i : MS0) {
      if(i < 3) cur.push_back(i);
      else cur.insert(begin(cur),i-3);
      seqs.push_back(cur);
    }
    MS.insert(begin(MS), 3);
    for(i32 r = 0; r < (i32)(MS.size()); ++r) for(i32 l = 0; l < (i32)(r+1); ++l) if(l <= 1) {
      vector<int> seq;
      for(i32 i = (l); i <= (i32)(r); ++i) seq.push_back(MS[i]);
      seqs.push_back(seq);
    }
  }
  // option 2: all left subsequences of 3M, (also M)
  if(opt == 2) {
    for(i32 c = (2); c <= (i32)(NC-1); ++c) {
      vector<int> seq(c, 0);
      seqs.push_back(seq);
    }
    seqs.push_back(MS);
    MS.insert(begin(MS), 3);
    for(i32 r = 0; r < (i32)(MS.size()); ++r) for(i32 l = 0; l < (i32)(r+1); ++l) if(l <= 0) {
      vector<int> seq;
      for(i32 i = (l); i <= (i32)(r); ++i) seq.push_back(MS[i]);
      seqs.push_back(seq);
    }
  }
  // option 3: all subsequences of 3MS
  if(opt == 3) {
    MS.insert(begin(MS), 3);
    for(i32 r = 0; r < (i32)(MS.size()); ++r) for(i32 l = 0; l < (i32)(r+1); ++l) {
      vector<int> seq;
      for(i32 i = (l); i <= (i32)(r); ++i) seq.push_back(MS[i]);
      seqs.push_back(seq);
    }
  }
  set<vector<int>> all_seqs(begin(seqs), end(seqs));
  seqs.assign(begin(all_seqs), end(all_seqs));
  do { if(!(!seqs.empty() && seqs[0].empty())) { throw runtime_error("main.cpp" ":" "1267" " Assertion failed: " "!seqs.empty() && seqs[0].empty()"); } } while(0);
  int nmacro = seqs.size();
  vector<macro> M(nmacro);
  for(i32 i = 0; i < (i32)(nmacro); ++i) {
    M[i].reset();
    for(int x : seqs[i]) {
      if(x == 3) M[i].start_s = true;
      else M[i].do_move(x);
    }
  }
  // TODO: note that we can go from SX to X
  // by doing SSX...
  vector<vector<array<int,2>>> macro_dist(nmacro, vector<array<int,2>>(nmacro));
  for(i32 i = 0; i < (i32)(nmacro); ++i) for(i32 j = 0; j < (i32)(nmacro); ++j) {
    macro_dist[i][j] =
      (i == j) ? array<int,2>{1, make_history({-1,4})} : calc_macro_dist(seqs[i],seqs[j]);
  }
  vector<vector<int>> G_macros(nmacro);
  for(i32 i = 0; i < (i32)(nmacro); ++i) for(i32 j = 0; j < (i32)(nmacro); ++j) {
    if(i == 0
       || (int)seqs[j].size() >= (int)MS.size()-1
       || macro_dist[i][j][0] < 2+(int)seqs[j].size()
       || i == j
       ) {
      G_macros[i].push_back(j);
    }
  }
  i32 nstates = 2*m * 4*nn * nmacro;
  // TODO: fail immediately if nstates is too large.
  do{}while(0);
  auto encode_state = [&](i32 stage, i32 pos, i32 macro) {
    return (stage*4*nn+pos)*nmacro + macro;
  };
  auto decode_state = [&](int ix) -> array<int,3> {
    return {ix/(nmacro*4*nn), ix/nmacro%(4*nn), ix%nmacro};
  };
  vector<int> D(nstates, best_ans.size()+1);
  vector<int> F(nstates, -1);
  vector<vector<int>> QS(best_ans.size()+1);
  int solv = best_ans.size()+1;
  int solv_from = -1;
  auto addcheck = [&](int d, i32 stage, i32 pos, i32 macro, i32 from) __attribute__((always_inline)) {
    if(stage == 2*m) {
      if(d < solv) {
        solv = d;
        solv_from = from;
      }
      return;
    }
    int ix = encode_state(stage,pos,macro);
    if(d < D[ix]) {
      D[ix] = d;
      F[ix] = from;
      QS[d].push_back(ix);
    }
  };
  auto add = [&](int d, i32 stage, i32 pos, i32 macro, i32 from) __attribute__((always_inline)) {
    int ix = encode_state(stage,pos,macro);
    if(d < D[ix]) {
      D[ix] = d;
      F[ix] = from;
      QS[d].push_back(ix);
    }
  };
  add(0, 0, 1, 0, -1);
  int nvis = 0;
  for(int d = 0; d < (int)QS.size(); ++d) {
    for(int ix : QS[d]) {
      nvis += 1;
      if(nvis % 1024 == 0) {
        if(TIMER.elapsed() > TL_POST) {
          goto l_end;
        }
      }
      if(d != D[ix]) continue;
      auto [stage, u, macro] = decode_state(ix);
      iter_moves(u, [&](int v, int) {
        add(d+1, stage, v, macro, ix);
      });
      bool can_progress = false;
      if(stage%2 == 0) {
        if(u/4 == srcs[route[stage/2]]) {
          can_progress = true;
        }
      }else{
        if(u/4 == tgts[route[stage/2]]) {
          can_progress = true;
        }
      }
      if(can_progress) {
        addcheck(d+1, stage+1, u, macro, ix);
      }
      for(int j : G_macros[macro]) {
        if(M[j].start_s) {
          if(can_progress) {
            addcheck(d+macro_dist[macro][j][0], stage+1, M[j].to[u], j, ix);
          }else if(stage%2 == 0 && !is_src_tgt[u/4]) {
            add(d+macro_dist[macro][j][0], stage, M[j].to[u], j, ix);
          }else{
            add(d+1+macro_dist[macro][j][0], stage, M[j].to[u], j, ix);
          }
        }else{
          add(d+macro_dist[macro][j][0], stage, M[j].to[u], j, ix);
        }
      }
    }
  }
 l_end:;
  if(solv_from != -1) {
    nreprocess += 1;
    vector<int> cur;
    for(int h = solv_from; h != -1; h = F[h]) {
      cur.push_back(h);
    }
    reverse(begin(cur), end(cur));
    vector<int> ans;
    auto play_macro = [&](int macro, int j) {
      int h = macro_dist[macro][j][1];
      vector<int> seq;
      while(h != -1) {
        auto e = history_pool[h];
        seq.push_back(e.move);
        h = e.prev;
      }
      reverse(begin(seq), end(seq));
      ans.insert(end(ans), begin(seq), end(seq));
    };
    for(i32 i = 0; i < (i32)((int)cur.size()-1); ++i) {
      auto [stage, u, macro] = decode_state(cur[i]);
      auto next_ix = cur[i+1];
      bool can_progress = false;
      if(next_ix == encode_state(stage, DO_F[u], macro)) {
        ans.push_back(0);
        goto l_found;
      }
      if(next_ix == encode_state(stage, DO_L[u], macro)) {
        ans.push_back(1);
        goto l_found;
      }
      if(next_ix == encode_state(stage, DO_R[u], macro)) {
        ans.push_back(2);
        goto l_found;
      }
      if(stage%2 == 0) {
        if(u/4 == srcs[route[stage/2]]) {
          can_progress = true;
        }
      }else{
        if(u/4 == tgts[route[stage/2]]) {
          can_progress = true;
        }
      }
      if(can_progress) {
        if(next_ix == encode_state(stage+1, u, macro)) {
          ans.push_back(3);
          goto l_found;
        }
      }
      for(int j : G_macros[macro]) {
        if(M[j].start_s) {
          if(can_progress) {
            if(next_ix == encode_state(stage+1, M[j].to[u], j)) {
              play_macro(macro, j);
              goto l_found;
            }
          }else if(stage%2 == 0 && !is_src_tgt[u/4]) {
            if(next_ix == encode_state(stage, M[j].to[u], j)) {
              play_macro(macro, j);
              goto l_found;
            }
          }else{
            if(next_ix == encode_state(stage, M[j].to[u], j)) {
              ans.push_back(3);
              play_macro(macro, j);
              goto l_found;
            }
          }
        }else{
          if(next_ix == encode_state(stage, M[j].to[u], j)) {
            play_macro(macro, j);
            goto l_found;
          }
        }
      }
      do { if(!(false)) { throw runtime_error("main.cpp" ":" "1473" " Assertion failed: " "false"); } } while(0);
    l_found:;
    }
    ans.push_back(3);
    do{}while(0);
    do { if(!(solv == (int)ans.size())) { throw runtime_error("main.cpp" ":" "1479" " Assertion failed: " "solv == (int)ans.size()"); } } while(0);
    save_ans(ans, MS.size());
  }else{
    cerr << "time or length limit reached" << endl;
  }
}
void solve() {
  auto MSS = solve_beam();
  auto beam_time = TIMER.elapsed();
  cerr << "[DATA] beam_time = " << beam_time << endl;
  rng.shuffle(MSS);
  pdqsort(begin(MSS), end(MSS));
  int maxl = MAXL_by_n[n];
  bool use_post = n*n*m < 4500;
  const f64 tl_cleanup = use_post ? TL_CLEANUP_IFPOST : TL_CLEANUP_NOPOST;
  const f64 tl = use_post ? TL_IFPOST : TL_NOPOST;
  for(i32 i = 0; i < (i32)(MSS.size()); ++i) {
    if(TIMER.elapsed() > tl) break;
    auto const& [cost, h] = MSS[i];
    auto MS = get_move_seq(h);
    if((int)MS.size() > maxl) continue;
    bool b = solve2(MS, TIMER.elapsed() < tl_cleanup);
    if(!b) {
      maxl = MS.size()-1;
      cerr << "reduce maxl to " << maxl << endl;
    }
  }
  if(use_post) {
    sort(begin(solutions_to_reprocess), end(solutions_to_reprocess));
    for(auto const& [_d, MS, route] : solutions_to_reprocess) {
      if(TIMER.elapsed() > TL) {
        break;
      }
      reprocess(MS, route);
    }
  }
  print_best();
}
// TODO: remember best solutions
// recompute best paths using full search.
int main() {
  ios::sync_with_stdio(false);
  cerr << setprecision(6) << fixed;
  TIMER.reset();
  history_pool = new history_list[MAX_HISTORY];
  MACROS = new macro[MAXMACROS];
  read();
  init();
  solve();
  cerr << "[DATA] nreprocess = " << nreprocess << endl;
  cerr << "[DATA] time = " << (i32)(TIMER.elapsed()*1000) << endl;
  return 0;
}
// TODO: on 10'000 test cases, maximum good maxl depending on n,m,t.
