// -*- compile-command: "make" -*-
#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
 #pragma GCC target "tune=native"
 #include <bits/stdc++.h>
 #include <sys/time.h>
 #include <immintrin.h>
 #include <x86intrin.h>
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
const f64 F = 1.4;
const f64 TL = 1.95 * F;
timer TIMER;
const int BW = 25;
const int BW_INNER = 4;
const int BW_KEEP = 4;
const int BW_F = 48;
const int BW_INNER_F = 10;
const int BW_KEEP_F = 10;
const int N = 10;
const int NN = 100;
int initial[N][N];
void read(){
  { int n; cin>>n; do { if(!(n == N)) { throw runtime_error("main.cpp" ":" "26" " Assertion failed: " "n == N"); } } while(0); }
  for(i32 i = 0; i < (i32)(N); ++i) {
    for(i32 j = 0; j < (i32)(N); ++j) {
      cin>>initial[i][j];
    }
  }
}
f32 dist(int i, int j) {
  return abs(i-j);
}
void init() {
}
// TODO: also consider distance in moves.
struct history_list {
  int dir, i, j, r;
  int prev;
};
const int MAX_HISTORY = 1<<25;
history_list* history_pool;
int next_history = 0;
int make_history(history_list l) {
  history_pool[next_history] = l;
  return next_history++;
}
struct state {
  u8 na[N];
  u8 a[N][15];
  u8 nb[N];
  u8 b[N][20];
  array<u8,3> where[NN];
  int score;
  int history;
  void copy_from(state const& base) {
    *this = base;
    // FOR(i, N) {
    //   na[i] = base.na[i];
    //   FOR(j, na[i]) a[i][j] = base.a[i][j];
    // }
    // FOR(i, N) {
    //   nb[i] = base.nb[i];
    //   FOR(j, nb[i]) b[i][j] = base.b[i][j];
    // }
    // FOR(i, NN) where[i] = base.where[i];
    // score = base.score;
    // history = base.history;
  }
  void reset() {
    for(i32 i = 0; i < (i32)(N); ++i) {
      na[i] = N; nb[i] = 0;
    }
    for(i32 i = 0; i < (i32)(N); ++i) {
      for(i32 j = 0; j < (i32)(N); ++j) {
        a[i][j] = initial[i][j];
        where[initial[i][j]] = {0,(u8)i,(u8)j};
      }
    }
    score = eval();
    history = -1;
  }
  bool is_end(array<u8,3> const& x) const {
    auto [xe,xi,xj] = x;
    if(xe == 0) {
      return xj+1 == na[xi];
    }else{
      return xj+1 == nb[xi];
    }
  }
  void check() const {
    int sc = eval();
    if(sc != score) {
      do{}while(0);
    }
    do { if(!(sc == score)) { throw runtime_error("main.cpp" ":" "111" " Assertion failed: " "sc == score"); } } while(0);
  }
  int score_diff(array<u8,3> const& x, array<u8,3> const& y) const {
    auto [xe,xi,xj] = x;
    auto [ye,yi,yj] = y;
    if(xe == ye) {
      if(xi == yi) {
        // Same track
        //  Already merged
        if(xe == 0 && xj+1==yj) return 0;
        if(xe == 1 && xj==yj+1) return 0;
        //  Separated
        return 300000;
      }else{
        return 200000;
      }
    }else{
      if(xe==0&&is_end(x)&&is_end(y)) {
        // Can be merged in 1 move.
        return 100000;
      }else{
        // Can be merged in 2 moves.
        return 200000;
      }
    }
  }
  int score_diff1(int i) const {
    auto [xe,xi,xj] = where[i];
    if(xe == 1) return 1 + pow(abs(i/N-xi), 2);
    else if(xi == i) return 0;
    else return 2 + pow(abs(i/N-xi), 2);
  }
  int eval() const {
    int r = 0;
    for(i32 i = 0; i < (i32)(NN); ++i) if(i%N != (N-1)) {
      r += score_diff(where[i],where[i+1]);
    }
    for(i32 i = 0; i < (i32)(NN); ++i) r += score_diff1(i);
    return r;
  }
  void move1_btoa_fast(int i, int j) {
    nb[j] -= 1;
    where[b[j][nb[j]]] = {0,(u8)i,na[i]};
    a[i][na[i]] = b[j][nb[j]];
    na[i] += 1;
  }
  void move1_atob(int i, int j) {
    int delta = 0;
    bitset<NN> changes = 0;
    static int Q[NN]; int nq = 0;
    auto add = [&](int i) __attribute__((always_inline)) {
      if(!changes[i]) {
        changes[i] = 1;
        Q[nq++] = i;
      }
    };
    auto add1 = [&](int i) {
      if(i%N > 0) add(i-1);
      if(i%N < N-1) add(i);
    };
    if(na[i] > 1) add1(a[i][na[i]-2]);
    add1(a[i][na[i]-1]);
    if(nb[j] > 0) add1(b[j][nb[j]-1]);
    for(i32 iq = 0; iq < (i32)(nq); ++iq) delta -= score_diff(where[Q[iq]], where[Q[iq]+1]);
    delta -= score_diff1(a[i][na[i]-1]);
    na[i] -= 1;
    where[a[i][na[i]]] = {1,(u8)j,nb[j]};
    b[j][nb[j]] = a[i][na[i]];
    nb[j] += 1;
    for(i32 iq = 0; iq < (i32)(nq); ++iq) delta += score_diff(where[Q[iq]], where[Q[iq]+1]);
    delta += score_diff1(b[j][nb[j]-1]);
    score += delta;
    // debug(score);
    // check();
  }
  void move1_btoa(int i, int j) {
    int delta = 0;
    bitset<NN> changes = 0;
    static int Q[NN]; int nq = 0;
    auto add = [&](int i) __attribute__((always_inline)) {
      if(!changes[i]) {
        changes[i] = 1;
        Q[nq++] = i;
      }
    };
    auto add1 = [&](int i) {
      if(i%N > 0) add(i-1);
      if(i%N < N-1) add(i);
    };
    if(na[i] > 0) add1(a[i][na[i]-1]);
    add1(b[j][nb[j]-1]);
    if(nb[j] > 1) add1(b[j][nb[j]-2]);
    for(i32 iq = 0; iq < (i32)(nq); ++iq) delta -= score_diff(where[Q[iq]], where[Q[iq]+1]);
    delta -= score_diff1(b[j][nb[j]-1]);
    nb[j] -= 1;
    where[b[j][nb[j]]] = {0,(u8)i,na[i]};
    a[i][na[i]] = b[j][nb[j]];
    na[i] += 1;
    for(i32 iq = 0; iq < (i32)(nq); ++iq) delta += score_diff(where[Q[iq]], where[Q[iq]+1]);
    delta += score_diff1(a[i][na[i]-1]);
    score += delta;
    // debug(score);
    // check();
  }
  void move_atob(int i, int j, int cnt) {
    for(i32 k = 0; k < (i32)(cnt); ++k) move1_atob(i,j);
  }
  void move_btoa(int i, int j, int cnt) {
    for(i32 k = 0; k < (i32)(cnt); ++k) move1_btoa(i,j);
  }
  void print() const {
    cerr << "====================" << endl;
    for(i32 i = 0; i < (i32)(N); ++i) {
      cerr << "a" << i << ": ";
      for(i32 j = 0; j < (i32)(na[i]); ++j) {
        cerr << setw(2) << (int)a[i][j] << ' ';
      }
      cerr << endl;
    }
    cerr << "====================" << endl;
    for(i32 i = 0; i < (i32)(N); ++i) {
      cerr << "b" << i << ": ";
      for(i32 j = 0; j < (i32)(nb[i]); ++j) {
        cerr << setw(2) << (int)b[i][j] << ' ';
      }
      cerr << endl;
    }
    cerr << "====================" << endl;
  }
};
struct cand {
  f32 value;
  int index;
  bool operator<(cand const& o) const { return value < o.value; }
};
const f32 randomf = 100.0;
template<int BW>
struct beam_queue {
  int size;
  state states[BW];
  max_queue<cand> Q;
  int count;
  void reset() {
    size = 0;
    while(!Q.empty()) Q.pop();
    count = 0;
  }
  void add0(state const& S) {
    f32 v = S.score + randomf * rng.randomFloat();
    if(size < BW) {
      states[size].copy_from(S);
      Q.push(cand {v, size});
      size += 1;
    }else if(v < Q.top().value) {
      auto [sc, ix] = Q.top(); Q.pop();
      states[ix].copy_from(S);
      Q.push(cand {v, ix});
    }
  }
  void add(state const& S, history_list h) {
    f32 v = S.score + randomf * rng.randomFloat();
    if(size < BW) {
      states[size].copy_from(S);
      states[size].history = make_history(h);
      Q.push(cand {v, size});
      size += 1;
    }else if(v < Q.top().value) {
      auto [sc, ix] = Q.top(); Q.pop();
      states[ix].copy_from(S);
      states[ix].history = make_history(h);
      Q.push(cand {v, ix});
    }
  }
  state best() const{
    int bi = 0;
    for(i32 i = 0; i < (i32)(size); ++i) if(states[i].score < states[bi].score) bi = i;
    return states[bi];
  }
};
template<int BW, int BW2, class F>
void generate_moves(state const& S, F&& f) {
  static beam_queue<BW> QS[N+1][N+1];
  static beam_queue<BW2> OUT;
  OUT.reset();
  for(i32 i = 0; i < (i32)(N+1); ++i) for(i32 j = 0; j < (i32)(N+1); ++j) QS[i][j].reset();
  QS[0][0].add(S, {-1,-1,-1,-1,S.history});
  static state TMP;
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) {
    auto add = [&](history_list l) {
      OUT.add(TMP, l);
      QS[i+1][j+1].add(TMP, l);
    };
    for(i32 i0 = 0; i0 < (i32)(i+1); ++i0) for(i32 j0 = 0; j0 < (i32)(j+1); ++j0) {
      for(i32 ix = 0; ix < (i32)(QS[i0][j0].size); ++ix) {
        state const& T = QS[i0][j0].states[ix];
        TMP.copy_from(T);
        for(i32 r = (1); r <= (i32)(T.na[i]); ++r) {
          if(TMP.nb[j] == 20) break;
          TMP.move1_atob(i,j);
          add({0,i,j,r,TMP.history});
        }
        TMP.copy_from(T);
        for(i32 r = (1); r <= (i32)(T.nb[j]); ++r) {
          if(TMP.na[i] == 15) break;
          TMP.move1_btoa(i,j);
          add({1,i,j,r,TMP.history});
        }
      }
    }
  }
  for(i32 i = 0; i < (i32)(OUT.size); ++i) f(OUT.states[i]);
  // debug(OUT.size);
  // S = OUT.best();
}
struct fin_state {
  int na[N];
  int a[N][N];
  int nb[N];
  int b[N][N];
  array<int,3> where[N];
  int score;
  int history;
  void from_state(state const& S) {
    for(i32 i = 0; i < (i32)(N); ++i) na[i] = nb[i] = 0;
    for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(S.na[i]); ++j) if(S.a[i][j]%N == 0) {
      where[S.a[i][j]/N] = {0,i,na[i]};
      a[i][na[i]++] = S.a[i][j]/N;
    }
    for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(S.nb[i]); ++j) if(S.b[i][j]%N == 0) {
      where[S.b[i][j]/N] = {1,i,nb[i]};
      b[i][nb[i]++] = S.b[i][j]/N;
    }
    history = S.history;
    score = eval();
  }
  int score_diff(int i) const {
    auto [xe,xi,xj] = where[i];
    if(xe == 1) return 1000 + pow(i-xi,2);
    else if(xi == i) return 0;
    else return 2000 + pow(i-xi,2);
  }
  int eval() const {
    int sc = 0;
    for(i32 i = 0; i < (i32)(N); ++i) {
      sc += score_diff(i);
    }
    return sc;
  }
  void move1_atob(int i, int j) {
    na[i] -= 1;
    where[a[i][na[i]]] = {1,j,nb[j]};
    b[j][nb[j]] = a[i][na[i]];
    nb[j] += 1;
    score = eval();
  }
  void move1_btoa(int i, int j) {
    nb[j] -= 1;
    where[b[j][nb[j]]] = {0,i,na[i]};
    a[i][na[i]] = b[j][nb[j]];
    na[i] += 1;
    score = eval();
  }
  void print() const {
    cerr << "====================" << endl;
    for(i32 i = 0; i < (i32)(N); ++i) {
      cerr << "a" << i << ": ";
      for(i32 j = 0; j < (i32)(na[i]); ++j) {
        cerr << setw(2) << a[i][j] << ' ';
      }
      cerr << endl;
    }
    cerr << "====================" << endl;
    for(i32 i = 0; i < (i32)(N); ++i) {
      cerr << "b" << i << ": ";
      for(i32 j = 0; j < (i32)(nb[i]); ++j) {
        cerr << setw(2) << b[i][j] << ' ';
      }
      cerr << endl;
    }
    cerr << "====================" << endl;
  }
};
template<int BW>
struct fin_beam_queue {
  int size;
  fin_state states[BW];
  max_queue<cand> Q;
  void reset() {
    size = 0;
    while(!Q.empty()) Q.pop();
  }
  void add0(fin_state const& S) {
    f32 v = S.score + rng.randomFloat();
    if(size < BW) {
      states[size] = S;
      Q.push(cand {v, size});
      size += 1;
    }else if(v < Q.top().value) {
      auto [sc, ix] = Q.top(); Q.pop();
      states[ix] = S;
      Q.push(cand {v, ix});
    }
  }
  void add(fin_state const& S, history_list h) {
    f32 v = S.score + rng.randomFloat();
    if(size < BW) {
      states[size] = S;
      states[size].history = make_history(h);
      Q.push(cand {v, size});
      size += 1;
    }else if(v < Q.top().value) {
      auto [sc, ix] = Q.top(); Q.pop();
      states[ix] = S;
      states[ix].history = make_history(h);
      Q.push(cand {v, ix});
    }
  }
  fin_state best() const{
    int bi = 0;
    for(i32 i = 0; i < (i32)(size); ++i) if(states[i].score < states[bi].score) bi = i;
    return states[bi];
  }
};
template<int BW, int BWK, class F>
void generate_fin_moves(fin_state const& S, F&& f) {
  static fin_beam_queue<BW> QS[N+1][N+1];
  static fin_beam_queue<BWK> OUT;
  OUT.reset();
  for(i32 i = 0; i < (i32)(N+1); ++i) for(i32 j = 0; j < (i32)(N+1); ++j) QS[i][j].reset();
  QS[0][0].add(S, {-1,-1,-1,-1,S.history});
  static fin_state TMP;
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) {
    auto add = [&](history_list l) {
      OUT.add(TMP, l);
      QS[i+1][j+1].add(TMP, l);
    };
    for(i32 i0 = 0; i0 < (i32)(i+1); ++i0) for(i32 j0 = 0; j0 < (i32)(j+1); ++j0) {
      for(i32 ix = 0; ix < (i32)(QS[i0][j0].size); ++ix) {
        fin_state const& T = QS[i0][j0].states[ix];
        TMP = T;
        for(i32 r = (1); r <= (i32)(T.na[i]); ++r) {
          if(TMP.nb[j] == 2) break;
          TMP.move1_atob(i,j);
          add({0,i,j,N*r,TMP.history});
        }
        TMP = T;
        for(i32 r = (1); r <= (i32)(T.nb[j]); ++r) {
          if(TMP.na[i] == 1) break;
          TMP.move1_btoa(i,j);
          add({1,i,j,N*r,TMP.history});
        }
      }
    }
  }
  for(i32 i = 0; i < (i32)(OUT.size); ++i) f(OUT.states[i]);
  // debug(OUT.size);
  // S = OUT.best();
}
vector<vector<array<int,4>>> best_ans;
void save_ans(vector<vector<array<int,4>>> const& ans) {
  if(best_ans.empty() || ans.size() < best_ans.size()) {
    best_ans = ans;
  }
}
void print_ans(vector<vector<array<int,4>>> const& ans) {
  cout << ans.size() << "\n";
  for(auto const& v : ans) {
    cout << v.size() << "\n";
    for(auto [a,b,c,d] : v) {
      cout << a << " " << b << " " << c << " " << d << "\n";
    }
  }
  cout << flush;
}
void reconstruct(int h) {
  vector<vector<array<int,4>>> ans;
  ans.emplace_back();
  while(h != -1) {
    auto e = history_pool[h];
    h = e.prev;
    if(e.dir == -1) ans.emplace_back();
    else {
      ans.back().push_back({e.dir,e.i,e.j,e.r});
    }
  }
  ans.pop_back();
  reverse(begin(ans), end(ans));
  save_ans(ans);
}
void solve_beam() {
  next_history = 0;
  vector<state> S;
  vector<fin_state> FS;
  beam_queue<BW> Q;
  fin_beam_queue<BW_F> FQ;
  S.resize(1);
  S[0].reset();
  int iter = 0;
  int besth = -1;
  while(1) {
    iter += 1;
    Q.reset();
    FQ.reset();
    for(auto const& T : FS) {
      generate_fin_moves<BW_INNER_F,BW_KEEP_F>(T, [&](fin_state const& R) {
        FQ.add0(R);
      });
    }
    if(iter == 0) {
      for(auto const& T : S) {
        generate_moves<BW,BW>(T, [&](state const& R) {
          if(R.score > 100000) {
            Q.add0(R);
          }else{
            static fin_state TMP;
            TMP.from_state(R);
            FQ.add0(TMP);
          }
        });
      }
    }else{
      for(auto const& T : S) {
        generate_moves<BW_INNER, BW_KEEP>(T, [&](state const& R) {
          if(R.score > 100000) {
            Q.add0(R);
          }else{
            static fin_state TMP;
            TMP.from_state(R);
            FQ.add0(TMP);
          }
        });
      }
    }
    S.clear();
    FS.clear();
    for(i32 i = 0; i < (i32)(Q.size); ++i) {
      S.push_back(Q.states[i]);
    }
    for(i32 i = 0; i < (i32)(FQ.size); ++i) {
      if(FQ.states[i].score == 0) {
        besth = FQ.states[i].history;
      }
      FS.push_back(FQ.states[i]);
    }
    int sc0 = 1e9, sc1 = 1e9;
    if(!S.empty()) sc0 = S[0].score;
    if(!FS.empty()) sc1 = FS[0].score;
    cerr << "iter=" << iter << " : " << make_tuple(sc0,sc1) << ", elapsed = " << TIMER.elapsed() << ", states=" << make_tuple(S.size(),FS.size()) << endl;
    // FOR(i, S.size()) S[i].print();
    if(besth != -1) break;
  }
  if(besth != -1) reconstruct(besth);
}
void solve() {
  solve_beam();
  cerr << "length: " << best_ans.size() << endl;
  cerr << "[DATA] time = " << (i32)(TIMER.elapsed()*1000) << endl;
  print_ans(best_ans);
}
int main() {
  ios::sync_with_stdio(false);
  cerr << setprecision(6) << fixed;
  TIMER.reset();
  read();
  init();
  history_pool = new history_list[MAX_HISTORY];
  solve();
  return 0;
}
