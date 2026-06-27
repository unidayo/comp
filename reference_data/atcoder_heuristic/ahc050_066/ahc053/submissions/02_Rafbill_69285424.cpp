// -*- compile-command: "make" -*-
#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
 #pragma GCC target "tune=native"
 #include <bits/stdc++.h>
 #include <sys/time.h>
 #include <immintrin.h>
 #include <x86intrin.h>
 #include <ext/pb_ds/assoc_container.hpp>
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
//          Copyright Malte Skarupke 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See http://www.boost.org/LICENSE_1_0.txt)
namespace detail
{
template<typename count_type, typename It, typename OutIt, typename ExtractKey>
void counting_sort_impl(It begin, It end, OutIt out_begin, ExtractKey && extract_key)
{
    count_type counts[256] = {};
    for (It it = begin; it != end; ++it)
    {
        ++counts[extract_key(*it)];
    }
    count_type total = 0;
    for (count_type & count : counts)
    {
        count_type old_count = count;
        count = total;
        total += old_count;
    }
    for (; begin != end; ++begin)
    {
        std::uint8_t key = extract_key(*begin);
        out_begin[counts[key]++] = std::move(*begin);
    }
}
template<typename It, typename OutIt, typename ExtractKey>
void counting_sort_impl(It begin, It end, OutIt out_begin, ExtractKey && extract_key)
{
    counting_sort_impl<std::uint64_t>(begin, end, out_begin, extract_key);
}
inline bool to_unsigned_or_bool(bool b)
{
    return b;
}
inline unsigned char to_unsigned_or_bool(unsigned char c)
{
    return c;
}
inline unsigned char to_unsigned_or_bool(signed char c)
{
    return static_cast<unsigned char>(c) + 128;
}
inline unsigned char to_unsigned_or_bool(char c)
{
    return static_cast<unsigned char>(c);
}
inline std::uint16_t to_unsigned_or_bool(char16_t c)
{
    return static_cast<std::uint16_t>(c);
}
inline std::uint32_t to_unsigned_or_bool(char32_t c)
{
    return static_cast<std::uint32_t>(c);
}
inline std::uint32_t to_unsigned_or_bool(wchar_t c)
{
    return static_cast<std::uint32_t>(c);
}
inline unsigned short to_unsigned_or_bool(short i)
{
    return static_cast<unsigned short>(i) + static_cast<unsigned short>(1 << (sizeof(short) * 8 - 1));
}
inline unsigned short to_unsigned_or_bool(unsigned short i)
{
    return i;
}
inline unsigned int to_unsigned_or_bool(int i)
{
    return static_cast<unsigned int>(i) + static_cast<unsigned int>(1 << (sizeof(int) * 8 - 1));
}
inline unsigned int to_unsigned_or_bool(unsigned int i)
{
    return i;
}
inline unsigned long to_unsigned_or_bool(long l)
{
    return static_cast<unsigned long>(l) + static_cast<unsigned long>(1l << (sizeof(long) * 8 - 1));
}
inline unsigned long to_unsigned_or_bool(unsigned long l)
{
    return l;
}
inline unsigned long long to_unsigned_or_bool(long long l)
{
    return static_cast<unsigned long long>(l) + static_cast<unsigned long long>(1ll << (sizeof(long long) * 8 - 1));
}
inline unsigned long long to_unsigned_or_bool(unsigned long long l)
{
    return l;
}
inline std::uint32_t to_unsigned_or_bool(float f)
{
    union
    {
        float f;
        std::uint32_t u;
    } as_union = { f };
    std::uint32_t sign_bit = -std::int32_t(as_union.u >> 31);
    return as_union.u ^ (sign_bit | 0x80000000);
}
inline std::uint64_t to_unsigned_or_bool(double f)
{
    union
    {
        double d;
        std::uint64_t u;
    } as_union = { f };
    std::uint64_t sign_bit = -std::int64_t(as_union.u >> 63);
    return as_union.u ^ (sign_bit | 0x8000000000000000);
}
template<typename T>
inline size_t to_unsigned_or_bool(T * ptr)
{
    return reinterpret_cast<size_t>(ptr);
}
template<size_t>
struct SizedRadixSorter;
template<>
struct SizedRadixSorter<1>
{
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        counting_sort_impl(begin, end, buffer_begin, [&](auto && o)
        {
            return to_unsigned_or_bool(extract_key(o));
        });
        return true;
    }
    static constexpr size_t pass_count = 2;
};
template<>
struct SizedRadixSorter<2>
{
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        std::ptrdiff_t num_elements = end - begin;
        if (num_elements <= (1ll << 32))
            return sort_inline<uint32_t>(begin, end, buffer_begin, buffer_begin + num_elements, extract_key);
        else
            return sort_inline<uint64_t>(begin, end, buffer_begin, buffer_begin + num_elements, extract_key);
    }
    template<typename count_type, typename It, typename OutIt, typename ExtractKey>
    static bool sort_inline(It begin, It end, OutIt out_begin, OutIt out_end, ExtractKey && extract_key)
    {
        count_type counts0[256] = {};
        count_type counts1[256] = {};
        for (It it = begin; it != end; ++it)
        {
            uint16_t key = to_unsigned_or_bool(extract_key(*it));
            ++counts0[key & 0xff];
            ++counts1[(key >> 8) & 0xff];
        }
        count_type total0 = 0;
        count_type total1 = 0;
        for (int i = 0; i < 256; ++i)
        {
            count_type old_count0 = counts0[i];
            count_type old_count1 = counts1[i];
            counts0[i] = total0;
            counts1[i] = total1;
            total0 += old_count0;
            total1 += old_count1;
        }
        for (It it = begin; it != end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it));
            out_begin[counts0[key]++] = std::move(*it);
        }
        for (OutIt it = out_begin; it != out_end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 8;
            begin[counts1[key]++] = std::move(*it);
        }
        return false;
    }
    static constexpr size_t pass_count = 3;
};
template<>
struct SizedRadixSorter<4>
{
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        std::ptrdiff_t num_elements = end - begin;
        if (num_elements <= (1ll << 32))
            return sort_inline<uint32_t>(begin, end, buffer_begin, buffer_begin + num_elements, extract_key);
        else
            return sort_inline<uint64_t>(begin, end, buffer_begin, buffer_begin + num_elements, extract_key);
    }
    template<typename count_type, typename It, typename OutIt, typename ExtractKey>
    static bool sort_inline(It begin, It end, OutIt out_begin, OutIt out_end, ExtractKey && extract_key)
    {
        count_type counts0[256] = {};
        count_type counts1[256] = {};
        count_type counts2[256] = {};
        count_type counts3[256] = {};
        for (It it = begin; it != end; ++it)
        {
            uint32_t key = to_unsigned_or_bool(extract_key(*it));
            ++counts0[key & 0xff];
            ++counts1[(key >> 8) & 0xff];
            ++counts2[(key >> 16) & 0xff];
            ++counts3[(key >> 24) & 0xff];
        }
        count_type total0 = 0;
        count_type total1 = 0;
        count_type total2 = 0;
        count_type total3 = 0;
        for (int i = 0; i < 256; ++i)
        {
            count_type old_count0 = counts0[i];
            count_type old_count1 = counts1[i];
            count_type old_count2 = counts2[i];
            count_type old_count3 = counts3[i];
            counts0[i] = total0;
            counts1[i] = total1;
            counts2[i] = total2;
            counts3[i] = total3;
            total0 += old_count0;
            total1 += old_count1;
            total2 += old_count2;
            total3 += old_count3;
        }
        for (It it = begin; it != end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it));
            out_begin[counts0[key]++] = std::move(*it);
        }
        for (OutIt it = out_begin; it != out_end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 8;
            begin[counts1[key]++] = std::move(*it);
        }
        for (It it = begin; it != end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 16;
            out_begin[counts2[key]++] = std::move(*it);
        }
        for (OutIt it = out_begin; it != out_end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 24;
            begin[counts3[key]++] = std::move(*it);
        }
        return false;
    }
    static constexpr size_t pass_count = 5;
};
template<>
struct SizedRadixSorter<8>
{
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        std::ptrdiff_t num_elements = end - begin;
        if (num_elements <= (1ll << 32))
            return sort_inline<uint32_t>(begin, end, buffer_begin, buffer_begin + num_elements, extract_key);
        else
            return sort_inline<uint64_t>(begin, end, buffer_begin, buffer_begin + num_elements, extract_key);
    }
    template<typename count_type, typename It, typename OutIt, typename ExtractKey>
    static bool sort_inline(It begin, It end, OutIt out_begin, OutIt out_end, ExtractKey && extract_key)
    {
        count_type counts0[256] = {};
        count_type counts1[256] = {};
        count_type counts2[256] = {};
        count_type counts3[256] = {};
        count_type counts4[256] = {};
        count_type counts5[256] = {};
        count_type counts6[256] = {};
        count_type counts7[256] = {};
        for (It it = begin; it != end; ++it)
        {
            uint64_t key = to_unsigned_or_bool(extract_key(*it));
            ++counts0[key & 0xff];
            ++counts1[(key >> 8) & 0xff];
            ++counts2[(key >> 16) & 0xff];
            ++counts3[(key >> 24) & 0xff];
            ++counts4[(key >> 32) & 0xff];
            ++counts5[(key >> 40) & 0xff];
            ++counts6[(key >> 48) & 0xff];
            ++counts7[(key >> 56) & 0xff];
        }
        count_type total0 = 0;
        count_type total1 = 0;
        count_type total2 = 0;
        count_type total3 = 0;
        count_type total4 = 0;
        count_type total5 = 0;
        count_type total6 = 0;
        count_type total7 = 0;
        for (int i = 0; i < 256; ++i)
        {
            count_type old_count0 = counts0[i];
            count_type old_count1 = counts1[i];
            count_type old_count2 = counts2[i];
            count_type old_count3 = counts3[i];
            count_type old_count4 = counts4[i];
            count_type old_count5 = counts5[i];
            count_type old_count6 = counts6[i];
            count_type old_count7 = counts7[i];
            counts0[i] = total0;
            counts1[i] = total1;
            counts2[i] = total2;
            counts3[i] = total3;
            counts4[i] = total4;
            counts5[i] = total5;
            counts6[i] = total6;
            counts7[i] = total7;
            total0 += old_count0;
            total1 += old_count1;
            total2 += old_count2;
            total3 += old_count3;
            total4 += old_count4;
            total5 += old_count5;
            total6 += old_count6;
            total7 += old_count7;
        }
        for (It it = begin; it != end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it));
            out_begin[counts0[key]++] = std::move(*it);
        }
        for (OutIt it = out_begin; it != out_end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 8;
            begin[counts1[key]++] = std::move(*it);
        }
        for (It it = begin; it != end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 16;
            out_begin[counts2[key]++] = std::move(*it);
        }
        for (OutIt it = out_begin; it != out_end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 24;
            begin[counts3[key]++] = std::move(*it);
        }
        for (It it = begin; it != end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 32;
            out_begin[counts4[key]++] = std::move(*it);
        }
        for (OutIt it = out_begin; it != out_end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 40;
            begin[counts5[key]++] = std::move(*it);
        }
        for (It it = begin; it != end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 48;
            out_begin[counts6[key]++] = std::move(*it);
        }
        for (OutIt it = out_begin; it != out_end; ++it)
        {
            std::uint8_t key = to_unsigned_or_bool(extract_key(*it)) >> 56;
            begin[counts7[key]++] = std::move(*it);
        }
        return false;
    }
    static constexpr size_t pass_count = 9;
};
template<typename>
struct RadixSorter;
template<>
struct RadixSorter<bool>
{
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        size_t false_count = 0;
        for (It it = begin; it != end; ++it)
        {
            if (!extract_key(*it))
                ++false_count;
        }
        size_t true_position = false_count;
        false_count = 0;
        for (; begin != end; ++begin)
        {
            if (extract_key(*begin))
                buffer_begin[true_position++] = std::move(*begin);
            else
                buffer_begin[false_count++] = std::move(*begin);
        }
        return true;
    }
    static constexpr size_t pass_count = 2;
};
template<>
struct RadixSorter<signed char> : SizedRadixSorter<sizeof(signed char)>
{
};
template<>
struct RadixSorter<unsigned char> : SizedRadixSorter<sizeof(unsigned char)>
{
};
template<>
struct RadixSorter<signed short> : SizedRadixSorter<sizeof(signed short)>
{
};
template<>
struct RadixSorter<unsigned short> : SizedRadixSorter<sizeof(unsigned short)>
{
};
template<>
struct RadixSorter<signed int> : SizedRadixSorter<sizeof(signed int)>
{
};
template<>
struct RadixSorter<unsigned int> : SizedRadixSorter<sizeof(unsigned int)>
{
};
template<>
struct RadixSorter<signed long> : SizedRadixSorter<sizeof(signed long)>
{
};
template<>
struct RadixSorter<unsigned long> : SizedRadixSorter<sizeof(unsigned long)>
{
};
template<>
struct RadixSorter<signed long long> : SizedRadixSorter<sizeof(signed long long)>
{
};
template<>
struct RadixSorter<unsigned long long> : SizedRadixSorter<sizeof(unsigned long long)>
{
};
template<>
struct RadixSorter<float> : SizedRadixSorter<sizeof(float)>
{
};
template<>
struct RadixSorter<double> : SizedRadixSorter<sizeof(double)>
{
};
template<>
struct RadixSorter<char> : SizedRadixSorter<sizeof(char)>
{
};
template<>
struct RadixSorter<wchar_t> : SizedRadixSorter<sizeof(wchar_t)>
{
};
template<>
struct RadixSorter<char16_t> : SizedRadixSorter<sizeof(char16_t)>
{
};
template<>
struct RadixSorter<char32_t> : SizedRadixSorter<sizeof(char32_t)>
{
};
template<typename K, typename V>
struct RadixSorter<std::pair<K, V>>
{
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        bool first_result = RadixSorter<V>::sort(begin, end, buffer_begin, [&](auto && o)
        {
            return extract_key(o).second;
        });
        auto extract_first = [&](auto && o)
        {
            return extract_key(o).first;
        };
        if (first_result)
        {
            return !RadixSorter<K>::sort(buffer_begin, buffer_begin + (end - begin), begin, extract_first);
        }
        else
        {
            return RadixSorter<K>::sort(begin, end, buffer_begin, extract_first);
        }
    }
    static constexpr size_t pass_count = RadixSorter<K>::pass_count + RadixSorter<V>::pass_count;
};
template<typename K, typename V>
struct RadixSorter<const std::pair<K, V> &>
{
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        bool first_result = RadixSorter<V>::sort(begin, end, buffer_begin, [&](auto && o) -> const V &
        {
            return extract_key(o).second;
        });
        auto extract_first = [&](auto && o) -> const K &
        {
            return extract_key(o).first;
        };
        if (first_result)
        {
            return !RadixSorter<K>::sort(buffer_begin, buffer_begin + (end - begin), begin, extract_first);
        }
        else
        {
            return RadixSorter<K>::sort(begin, end, buffer_begin, extract_first);
        }
    }
    static constexpr size_t pass_count = RadixSorter<K>::pass_count + RadixSorter<V>::pass_count;
};
template<size_t I, size_t S, typename Tuple>
struct TupleRadixSorter
{
    using NextSorter = TupleRadixSorter<I + 1, S, Tuple>;
    using ThisSorter = RadixSorter<typename std::tuple_element<I, Tuple>::type>;
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt out_begin, OutIt out_end, ExtractKey && extract_key)
    {
        bool which = NextSorter::sort(begin, end, out_begin, out_end, extract_key);
        auto extract_i = [&](auto && o)
        {
            return std::get<I>(extract_key(o));
        };
        if (which)
            return !ThisSorter::sort(out_begin, out_end, begin, extract_i);
        else
            return ThisSorter::sort(begin, end, out_begin, extract_i);
    }
    static constexpr size_t pass_count = ThisSorter::pass_count + NextSorter::pass_count;
};
template<size_t I, size_t S, typename Tuple>
struct TupleRadixSorter<I, S, const Tuple &>
{
    using NextSorter = TupleRadixSorter<I + 1, S, const Tuple &>;
    using ThisSorter = RadixSorter<typename std::tuple_element<I, Tuple>::type>;
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt out_begin, OutIt out_end, ExtractKey && extract_key)
    {
        bool which = NextSorter::sort(begin, end, out_begin, out_end, extract_key);
        auto extract_i = [&](auto && o) -> decltype(auto)
        {
            return std::get<I>(extract_key(o));
        };
        if (which)
            return !ThisSorter::sort(out_begin, out_end, begin, extract_i);
        else
            return ThisSorter::sort(begin, end, out_begin, extract_i);
    }
    static constexpr size_t pass_count = ThisSorter::pass_count + NextSorter::pass_count;
};
template<size_t I, typename Tuple>
struct TupleRadixSorter<I, I, Tuple>
{
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It, It, OutIt, OutIt, ExtractKey &&)
    {
        return false;
    }
    static constexpr size_t pass_count = 0;
};
template<size_t I, typename Tuple>
struct TupleRadixSorter<I, I, const Tuple &>
{
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It, It, OutIt, OutIt, ExtractKey &&)
    {
        return false;
    }
    static constexpr size_t pass_count = 0;
};
template<typename... Args>
struct RadixSorter<std::tuple<Args...>>
{
    using SorterImpl = TupleRadixSorter<0, sizeof...(Args), std::tuple<Args...>>;
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        return SorterImpl::sort(begin, end, buffer_begin, buffer_begin + (end - begin), extract_key);
    }
    static constexpr size_t pass_count = SorterImpl::pass_count;
};
template<typename... Args>
struct RadixSorter<const std::tuple<Args...> &>
{
    using SorterImpl = TupleRadixSorter<0, sizeof...(Args), const std::tuple<Args...> &>;
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        return SorterImpl::sort(begin, end, buffer_begin, buffer_begin + (end - begin), extract_key);
    }
    static constexpr size_t pass_count = SorterImpl::pass_count;
};
template<typename T, size_t S>
struct RadixSorter<std::array<T, S>>
{
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        auto buffer_end = buffer_begin + (end - begin);
        bool which = false;
        for (size_t i = S; i > 0; --i)
        {
            auto extract_i = [&, i = i - 1](auto && o)
            {
                return extract_key(o)[i];
            };
            if (which)
                which = !RadixSorter<T>::sort(buffer_begin, buffer_end, begin, extract_i);
            else
                which = RadixSorter<T>::sort(begin, end, buffer_begin, extract_i);
        }
        return which;
    }
    static constexpr size_t pass_count = RadixSorter<T>::pass_count * S;
};
template<typename T>
struct RadixSorter<const T> : RadixSorter<T>
{
};
template<typename T>
struct RadixSorter<T &> : RadixSorter<const T &>
{
};
template<typename T>
struct RadixSorter<T &&> : RadixSorter<T>
{
};
template<typename T>
struct RadixSorter<const T &> : RadixSorter<T>
{
};
template<typename T>
struct RadixSorter<const T &&> : RadixSorter<T>
{
};
// these structs serve two purposes
// 1. they serve as illustration for how to implement the to_radix_sort_key function
// 2. they help produce better error messages. with these overloads you get the
//    error message "no matching function for call to to_radix_sort(your_type)"
//    without these examples, you'd get the error message "to_radix_sort_key was
//    not declared in this scope" which is a much less useful error message
struct ExampleStructA { int i; };
struct ExampleStructB { float f; };
inline int to_radix_sort_key(ExampleStructA a) { return a.i; }
inline float to_radix_sort_key(ExampleStructB b) { return b.f; }
template<typename T, typename Enable = void>
struct FallbackRadixSorter : RadixSorter<decltype(to_radix_sort_key(std::declval<T>()))>
{
    using base = RadixSorter<decltype(to_radix_sort_key(std::declval<T>()))>;
    template<typename It, typename OutIt, typename ExtractKey>
    static bool sort(It begin, It end, OutIt buffer_begin, ExtractKey && extract_key)
    {
        return base::sort(begin, end, buffer_begin, [&](auto && a) -> decltype(auto)
        {
            return to_radix_sort_key(extract_key(a));
        });
    }
};
template<typename...>
struct nested_void
{
 using type = void;
};
template<typename... Args>
using void_t = typename nested_void<Args...>::type;
template<typename T>
struct has_subscript_operator_impl
{
 template<typename U, typename = decltype(std::declval<U>()[0])>
 static std::true_type test(int);
 template<typename>
 static std::false_type test(...);
 using type = decltype(test<T>(0));
};
template<typename T>
using has_subscript_operator = typename has_subscript_operator_impl<T>::type;
template<typename T>
struct FallbackRadixSorter<T, void_t<decltype(to_unsigned_or_bool(std::declval<T>()))>>
    : RadixSorter<decltype(to_unsigned_or_bool(std::declval<T>()))>
{
};
template<typename T>
struct RadixSorter : FallbackRadixSorter<T>
{
};
template<typename T>
size_t radix_sort_pass_count = RadixSorter<T>::pass_count;
template<typename It, typename Func>
inline void unroll_loop_four_times(It begin, size_t iteration_count, Func && to_call)
{
    size_t loop_count = iteration_count / 4;
    size_t remainder_count = iteration_count - loop_count * 4;
    for (; loop_count > 0; --loop_count)
    {
        to_call(begin);
        ++begin;
        to_call(begin);
        ++begin;
        to_call(begin);
        ++begin;
        to_call(begin);
        ++begin;
    }
    switch(remainder_count)
    {
    case 3:
        to_call(begin);
        ++begin;
    case 2:
        to_call(begin);
        ++begin;
    case 1:
        to_call(begin);
    }
}
template<typename It, typename F>
inline It custom_std_partition(It begin, It end, F && func)
{
    for (;; ++begin)
    {
        if (begin == end)
            return end;
        if (!func(*begin))
            break;
    }
    It it = begin;
    for(++it; it != end; ++it)
    {
        if (!func(*it))
            continue;
        std::iter_swap(begin, it);
        ++begin;
    }
    return begin;
}
struct PartitionInfo
{
    PartitionInfo()
        : count(0)
    {
    }
    union
    {
        size_t count;
        size_t offset;
    };
    size_t next_offset;
};
template<size_t>
struct UnsignedForSize;
template<>
struct UnsignedForSize<1>
{
    typedef uint8_t type;
};
template<>
struct UnsignedForSize<2>
{
    typedef uint16_t type;
};
template<>
struct UnsignedForSize<4>
{
    typedef uint32_t type;
};
template<>
struct UnsignedForSize<8>
{
    typedef uint64_t type;
};
template<typename T>
struct SubKey;
template<size_t Size>
struct SizedSubKey
{
    template<typename T>
    static auto sub_key(T && value, void *)
    {
        return to_unsigned_or_bool(value);
    }
    typedef SubKey<void> next;
    using sub_key_type = typename UnsignedForSize<Size>::type;
};
template<typename T>
struct SubKey<const T> : SubKey<T>
{
};
template<typename T>
struct SubKey<T &> : SubKey<T>
{
};
template<typename T>
struct SubKey<T &&> : SubKey<T>
{
};
template<typename T>
struct SubKey<const T &> : SubKey<T>
{
};
template<typename T>
struct SubKey<const T &&> : SubKey<T>
{
};
template<typename T, typename Enable = void>
struct FallbackSubKey
    : SubKey<decltype(to_radix_sort_key(std::declval<T>()))>
{
    using base = SubKey<decltype(to_radix_sort_key(std::declval<T>()))>;
    template<typename U>
    static decltype(auto) sub_key(U && value, void * data)
    {
        return base::sub_key(to_radix_sort_key(value), data);
    }
};
template<typename T>
struct FallbackSubKey<T, void_t<decltype(to_unsigned_or_bool(std::declval<T>()))>>
    : SubKey<decltype(to_unsigned_or_bool(std::declval<T>()))>
{
};
template<typename T>
struct SubKey : FallbackSubKey<T>
{
};
template<>
struct SubKey<bool>
{
    template<typename T>
    static bool sub_key(T && value, void *)
    {
        return value;
    }
    typedef SubKey<void> next;
    using sub_key_type = bool;
};
template<>
struct SubKey<void>;
template<>
struct SubKey<unsigned char> : SizedSubKey<sizeof(unsigned char)>
{
};
template<>
struct SubKey<unsigned short> : SizedSubKey<sizeof(unsigned short)>
{
};
template<>
struct SubKey<unsigned int> : SizedSubKey<sizeof(unsigned int)>
{
};
template<>
struct SubKey<unsigned long> : SizedSubKey<sizeof(unsigned long)>
{
};
template<>
struct SubKey<unsigned long long> : SizedSubKey<sizeof(unsigned long long)>
{
};
template<typename T>
struct SubKey<T *> : SizedSubKey<sizeof(T *)>
{
};
template<typename F, typename S, typename Current>
struct PairSecondSubKey : Current
{
    static decltype(auto) sub_key(const std::pair<F, S> & value, void * sort_data)
    {
        return Current::sub_key(value.second, sort_data);
    }
    using next = typename std::conditional<std::is_same<SubKey<void>, typename Current::next>::value, SubKey<void>, PairSecondSubKey<F, S, typename Current::next>>::type;
};
template<typename F, typename S, typename Current>
struct PairFirstSubKey : Current
{
    static decltype(auto) sub_key(const std::pair<F, S> & value, void * sort_data)
    {
        return Current::sub_key(value.first, sort_data);
    }
    using next = typename std::conditional<std::is_same<SubKey<void>, typename Current::next>::value, PairSecondSubKey<F, S, SubKey<S>>, PairFirstSubKey<F, S, typename Current::next>>::type;
};
template<typename F, typename S>
struct SubKey<std::pair<F, S>> : PairFirstSubKey<F, S, SubKey<F>>
{
};
template<size_t Index, typename First, typename... More>
struct TypeAt : TypeAt<Index - 1, More..., void>
{
};
template<typename First, typename... More>
struct TypeAt<0, First, More...>
{
    typedef First type;
};
template<size_t Index, typename Current, typename First, typename... More>
struct TupleSubKey;
template<size_t Index, typename Next, typename First, typename... More>
struct NextTupleSubKey
{
    using type = TupleSubKey<Index, Next, First, More...>;
};
template<size_t Index, typename First, typename Second, typename... More>
struct NextTupleSubKey<Index, SubKey<void>, First, Second, More...>
{
    using type = TupleSubKey<Index + 1, SubKey<Second>, Second, More...>;
};
template<size_t Index, typename First>
struct NextTupleSubKey<Index, SubKey<void>, First>
{
    using type = SubKey<void>;
};
template<size_t Index, typename Current, typename First, typename... More>
struct TupleSubKey : Current
{
    template<typename Tuple>
    static decltype(auto) sub_key(const Tuple & value, void * sort_data)
    {
        return Current::sub_key(std::get<Index>(value), sort_data);
    }
    using next = typename NextTupleSubKey<Index, typename Current::next, First, More...>::type;
};
template<size_t Index, typename Current, typename First>
struct TupleSubKey<Index, Current, First> : Current
{
    template<typename Tuple>
    static decltype(auto) sub_key(const Tuple & value, void * sort_data)
    {
        return Current::sub_key(std::get<Index>(value), sort_data);
    }
    using next = typename NextTupleSubKey<Index, typename Current::next, First>::type;
};
template<typename First, typename... More>
struct SubKey<std::tuple<First, More...>> : TupleSubKey<0, SubKey<First>, First, More...>
{
};
struct BaseListSortData
{
    size_t current_index;
    size_t recursion_limit;
    void * next_sort_data;
};
template<typename It, typename ExtractKey>
struct ListSortData : BaseListSortData
{
    void (*next_sort)(It, It, std::ptrdiff_t, ExtractKey &, void *);
};
template<typename CurrentSubKey, typename T>
struct ListElementSubKey : SubKey<typename std::decay<decltype(std::declval<T>()[0])>::type>
{
    using base = SubKey<typename std::decay<decltype(std::declval<T>()[0])>::type>;
    using next = ListElementSubKey;
    template<typename U>
    static decltype(auto) sub_key(U && value, void * sort_data)
    {
        BaseListSortData * list_sort_data = static_cast<BaseListSortData *>(sort_data);
        const T & list = CurrentSubKey::sub_key(value, list_sort_data->next_sort_data);
        return base::sub_key(list[list_sort_data->current_index], list_sort_data->next_sort_data);
    }
};
template<typename T>
struct ListSubKey
{
    using next = SubKey<void>;
    using sub_key_type = T;
    static const T & sub_key(const T & value, void *)
    {
        return value;
    }
};
template<typename T>
struct FallbackSubKey<T, typename std::enable_if<has_subscript_operator<T>::value>::type> : ListSubKey<T>
{
};
template<typename It, typename ExtractKey>
inline void StdSortFallback(It begin, It end, ExtractKey & extract_key)
{
    std::sort(begin, end, [&](auto && l, auto && r){ return extract_key(l) < extract_key(r); });
}
template<std::ptrdiff_t StdSortThreshold, typename It, typename ExtractKey>
inline bool StdSortIfLessThanThreshold(It begin, It end, std::ptrdiff_t num_elements, ExtractKey & extract_key)
{
    if (num_elements <= 1)
        return true;
    if (num_elements >= StdSortThreshold)
        return false;
    StdSortFallback(begin, end, extract_key);
    return true;
}
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename SubKeyType = typename CurrentSubKey::sub_key_type>
struct InplaceSorter;
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, size_t NumBytes, size_t Offset = 0>
struct UnsignedInplaceSorter
{
    static constexpr size_t ShiftAmount = (((NumBytes - 1) - Offset) * 8);
    template<typename T>
    inline static uint8_t current_byte(T && elem, void * sort_data)
    {
        return CurrentSubKey::sub_key(elem, sort_data) >> ShiftAmount;
    }
    template<typename It, typename ExtractKey>
    static void sort(It begin, It end, std::ptrdiff_t num_elements, ExtractKey & extract_key, void (*next_sort)(It, It, std::ptrdiff_t, ExtractKey &, void *), void * sort_data)
    {
        if (num_elements < AmericanFlagSortThreshold)
            american_flag_sort(begin, end, extract_key, next_sort, sort_data);
        else
            ska_byte_sort(begin, end, extract_key, next_sort, sort_data);
    }
    template<typename It, typename ExtractKey>
    static void american_flag_sort(It begin, It end, ExtractKey & extract_key, void (*next_sort)(It, It, std::ptrdiff_t, ExtractKey &, void *), void * sort_data)
    {
        PartitionInfo partitions[256];
        for (It it = begin; it != end; ++it)
        {
            ++partitions[current_byte(extract_key(*it), sort_data)].count;
        }
        size_t total = 0;
        uint8_t remaining_partitions[256];
        int num_partitions = 0;
        for (int i = 0; i < 256; ++i)
        {
            size_t count = partitions[i].count;
            if (!count)
                continue;
            partitions[i].offset = total;
            total += count;
            partitions[i].next_offset = total;
            remaining_partitions[num_partitions] = i;
            ++num_partitions;
        }
        if (num_partitions > 1)
        {
            uint8_t * current_block_ptr = remaining_partitions;
            PartitionInfo * current_block = partitions + *current_block_ptr;
            uint8_t * last_block = remaining_partitions + num_partitions - 1;
            It it = begin;
            It block_end = begin + current_block->next_offset;
            It last_element = end - 1;
            for (;;)
            {
                PartitionInfo * block = partitions + current_byte(extract_key(*it), sort_data);
                if (block == current_block)
                {
                    ++it;
                    if (it == last_element)
                        break;
                    else if (it == block_end)
                    {
                        for (;;)
                        {
                            ++current_block_ptr;
                            if (current_block_ptr == last_block)
                                goto recurse;
                            current_block = partitions + *current_block_ptr;
                            if (current_block->offset != current_block->next_offset)
                                break;
                        }
                        it = begin + current_block->offset;
                        block_end = begin + current_block->next_offset;
                    }
                }
                else
                {
                    size_t offset = block->offset++;
                    std::iter_swap(it, begin + offset);
                }
            }
        }
        recurse:
        if (Offset + 1 != NumBytes || next_sort)
        {
            size_t start_offset = 0;
            It partition_begin = begin;
            for (uint8_t * it = remaining_partitions, * end = remaining_partitions + num_partitions; it != end; ++it)
            {
                size_t end_offset = partitions[*it].next_offset;
                It partition_end = begin + end_offset;
                std::ptrdiff_t num_elements = end_offset - start_offset;
                if (!StdSortIfLessThanThreshold<StdSortThreshold>(partition_begin, partition_end, num_elements, extract_key))
                {
                    UnsignedInplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, NumBytes, Offset + 1>::sort(partition_begin, partition_end, num_elements, extract_key, next_sort, sort_data);
                }
                start_offset = end_offset;
                partition_begin = partition_end;
            }
        }
    }
    template<typename It, typename ExtractKey>
    static void ska_byte_sort(It begin, It end, ExtractKey & extract_key, void (*next_sort)(It, It, std::ptrdiff_t, ExtractKey &, void *), void * sort_data)
    {
        PartitionInfo partitions[256];
        for (It it = begin; it != end; ++it)
        {
            ++partitions[current_byte(extract_key(*it), sort_data)].count;
        }
        uint8_t remaining_partitions[256];
        size_t total = 0;
        int num_partitions = 0;
        for (int i = 0; i < 256; ++i)
        {
            size_t count = partitions[i].count;
            if (count)
            {
                partitions[i].offset = total;
                total += count;
                remaining_partitions[num_partitions] = i;
                ++num_partitions;
            }
            partitions[i].next_offset = total;
        }
        for (uint8_t * last_remaining = remaining_partitions + num_partitions, * end_partition = remaining_partitions + 1; last_remaining > end_partition;)
        {
            last_remaining = custom_std_partition(remaining_partitions, last_remaining, [&](uint8_t partition)
            {
                size_t & begin_offset = partitions[partition].offset;
                size_t & end_offset = partitions[partition].next_offset;
                if (begin_offset == end_offset)
                    return false;
                unroll_loop_four_times(begin + begin_offset, end_offset - begin_offset, [partitions = partitions, begin, &extract_key, sort_data](It it)
                {
                    uint8_t this_partition = current_byte(extract_key(*it), sort_data);
                    size_t offset = partitions[this_partition].offset++;
                    std::iter_swap(it, begin + offset);
                });
                return begin_offset != end_offset;
            });
        }
        if (Offset + 1 != NumBytes || next_sort)
        {
            for (uint8_t * it = remaining_partitions + num_partitions; it != remaining_partitions; --it)
            {
                uint8_t partition = it[-1];
                size_t start_offset = (partition == 0 ? 0 : partitions[partition - 1].next_offset);
                size_t end_offset = partitions[partition].next_offset;
                It partition_begin = begin + start_offset;
                It partition_end = begin + end_offset;
                std::ptrdiff_t num_elements = end_offset - start_offset;
                if (!StdSortIfLessThanThreshold<StdSortThreshold>(partition_begin, partition_end, num_elements, extract_key))
                {
                    UnsignedInplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, NumBytes, Offset + 1>::sort(partition_begin, partition_end, num_elements, extract_key, next_sort, sort_data);
                }
            }
        }
    }
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, size_t NumBytes>
struct UnsignedInplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, NumBytes, NumBytes>
{
    template<typename It, typename ExtractKey>
    inline static void sort(It begin, It end, std::ptrdiff_t num_elements, ExtractKey & extract_key, void (*next_sort)(It, It, std::ptrdiff_t, ExtractKey &, void *), void * next_sort_data)
    {
        next_sort(begin, end, num_elements, extract_key, next_sort_data);
    }
};
template<typename It, typename ExtractKey, typename ElementKey>
size_t CommonPrefix(It begin, It end, size_t start_index, ExtractKey && extract_key, ElementKey && element_key)
{
    const auto & largest_match_list = extract_key(*begin);
    size_t largest_match = largest_match_list.size();
    if (largest_match == start_index)
        return start_index;
    for (++begin; begin != end; ++begin)
    {
        const auto & current_list = extract_key(*begin);
        size_t current_size = current_list.size();
        if (current_size < largest_match)
        {
            largest_match = current_size;
            if (largest_match == start_index)
                return start_index;
        }
        if (element_key(largest_match_list[start_index]) != element_key(current_list[start_index]))
            return start_index;
        for (size_t i = start_index + 1; i < largest_match; ++i)
        {
            if (element_key(largest_match_list[i]) != element_key(current_list[i]))
            {
                largest_match = i;
                break;
            }
        }
    }
    return largest_match;
}
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename ListType>
struct ListInplaceSorter
{
    using ElementSubKey = ListElementSubKey<CurrentSubKey, ListType>;
    template<typename It, typename ExtractKey>
    static void sort(It begin, It end, ExtractKey & extract_key, ListSortData<It, ExtractKey> * sort_data)
    {
        size_t current_index = sort_data->current_index;
        void * next_sort_data = sort_data->next_sort_data;
        auto current_key = [&](auto && elem) -> decltype(auto)
        {
            return CurrentSubKey::sub_key(extract_key(elem), next_sort_data);
        };
        auto element_key = [&](auto && elem) -> decltype(auto)
        {
            return ElementSubKey::base::sub_key(elem, sort_data);
        };
        sort_data->current_index = current_index = CommonPrefix(begin, end, current_index, current_key, element_key);
        It end_of_shorter_ones = std::partition(begin, end, [&](auto && elem)
        {
            return current_key(elem).size() <= current_index;
        });
        std::ptrdiff_t num_shorter_ones = end_of_shorter_ones - begin;
        if (sort_data->next_sort && !StdSortIfLessThanThreshold<StdSortThreshold>(begin, end_of_shorter_ones, num_shorter_ones, extract_key))
        {
            sort_data->next_sort(begin, end_of_shorter_ones, num_shorter_ones, extract_key, next_sort_data);
        }
        std::ptrdiff_t num_elements = end - end_of_shorter_ones;
        if (!StdSortIfLessThanThreshold<StdSortThreshold>(end_of_shorter_ones, end, num_elements, extract_key))
        {
            void (*sort_next_element)(It, It, std::ptrdiff_t, ExtractKey &, void *) = static_cast<void (*)(It, It, std::ptrdiff_t, ExtractKey &, void *)>(&sort_from_recursion);
            InplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, ElementSubKey>::sort(end_of_shorter_ones, end, num_elements, extract_key, sort_next_element, sort_data);
        }
    }
    template<typename It, typename ExtractKey>
    static void sort_from_recursion(It begin, It end, std::ptrdiff_t, ExtractKey & extract_key, void * next_sort_data)
    {
        ListSortData<It, ExtractKey> offset = *static_cast<ListSortData<It, ExtractKey> *>(next_sort_data);
        ++offset.current_index;
        --offset.recursion_limit;
        if (offset.recursion_limit == 0)
        {
            StdSortFallback(begin, end, extract_key);
        }
        else
        {
            sort(begin, end, extract_key, &offset);
        }
    }
    template<typename It, typename ExtractKey>
    static void sort(It begin, It end, std::ptrdiff_t, ExtractKey & extract_key, void (*next_sort)(It, It, std::ptrdiff_t, ExtractKey &, void *), void * next_sort_data)
    {
        ListSortData<It, ExtractKey> offset;
        offset.current_index = 0;
        offset.recursion_limit = 16;
        offset.next_sort = next_sort;
        offset.next_sort_data = next_sort_data;
        sort(begin, end, extract_key, &offset);
    }
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey>
struct InplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, bool>
{
    template<typename It, typename ExtractKey>
    static void sort(It begin, It end, std::ptrdiff_t, ExtractKey & extract_key, void (*next_sort)(It, It, std::ptrdiff_t, ExtractKey &, void *), void * sort_data)
    {
        It middle = std::partition(begin, end, [&](auto && a){ return !CurrentSubKey::sub_key(extract_key(a), sort_data); });
        if (next_sort)
        {
            next_sort(begin, middle, middle - begin, extract_key, sort_data);
            next_sort(middle, end, end - middle, extract_key, sort_data);
        }
    }
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey>
struct InplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, uint8_t> : UnsignedInplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, 1>
{
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey>
struct InplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, uint16_t> : UnsignedInplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, 2>
{
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey>
struct InplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, uint32_t> : UnsignedInplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, 4>
{
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey>
struct InplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, uint64_t> : UnsignedInplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, 8>
{
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename SubKeyType, typename Enable = void>
struct FallbackInplaceSorter;
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename SubKeyType>
struct InplaceSorter : FallbackInplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, SubKeyType>
{
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename SubKeyType>
struct FallbackInplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, SubKeyType, typename std::enable_if<has_subscript_operator<SubKeyType>::value>::type>
 : ListInplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey, SubKeyType>
{
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey>
struct SortStarter;
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold>
struct SortStarter<StdSortThreshold, AmericanFlagSortThreshold, SubKey<void>>
{
    template<typename It, typename ExtractKey>
    static void sort(It, It, std::ptrdiff_t, ExtractKey &, void *)
    {
    }
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey>
struct SortStarter
{
    template<typename It, typename ExtractKey>
    static void sort(It begin, It end, std::ptrdiff_t num_elements, ExtractKey & extract_key, void * next_sort_data = nullptr)
    {
        if (StdSortIfLessThanThreshold<StdSortThreshold>(begin, end, num_elements, extract_key))
            return;
        void (*next_sort)(It, It, std::ptrdiff_t, ExtractKey &, void *) = static_cast<void (*)(It, It, std::ptrdiff_t, ExtractKey &, void *)>(&SortStarter<StdSortThreshold, AmericanFlagSortThreshold, typename CurrentSubKey::next>::sort);
        if (next_sort == static_cast<void (*)(It, It, std::ptrdiff_t, ExtractKey &, void *)>(&SortStarter<StdSortThreshold, AmericanFlagSortThreshold, SubKey<void>>::sort))
            next_sort = nullptr;
        InplaceSorter<StdSortThreshold, AmericanFlagSortThreshold, CurrentSubKey>::sort(begin, end, num_elements, extract_key, next_sort, next_sort_data);
    }
};
template<std::ptrdiff_t StdSortThreshold, std::ptrdiff_t AmericanFlagSortThreshold, typename It, typename ExtractKey>
void inplace_radix_sort(It begin, It end, ExtractKey & extract_key)
{
    using SubKey = SubKey<decltype(extract_key(*begin))>;
    SortStarter<StdSortThreshold, AmericanFlagSortThreshold, SubKey>::sort(begin, end, end - begin, extract_key);
}
struct IdentityFunctor
{
    template<typename T>
    decltype(auto) operator()(T && i) const
    {
        return std::forward<T>(i);
    }
};
}
template<typename It, typename ExtractKey>
static void ska_sort(It begin, It end, ExtractKey && extract_key)
{
    detail::inplace_radix_sort<128, 1024>(begin, end, extract_key);
}
template<typename It>
static void ska_sort(It begin, It end)
{
    ska_sort(begin, end, detail::IdentityFunctor());
}
template<typename It, typename OutIt, typename ExtractKey>
bool ska_sort_copy(It begin, It end, OutIt buffer_begin, ExtractKey && key)
{
    std::ptrdiff_t num_elements = end - begin;
    if (num_elements < 128 || detail::radix_sort_pass_count<typename std::result_of<ExtractKey(decltype(*begin))>::type> >= 8)
    {
        ska_sort(begin, end, key);
        return false;
    }
    else
        return detail::RadixSorter<typename std::result_of<ExtractKey(decltype(*begin))>::type>::sort(begin, end, buffer_begin, key);
}
template<typename It, typename OutIt>
bool ska_sort_copy(It begin, It end, OutIt buffer_begin)
{
    return ska_sort_copy(begin, end, buffer_begin, detail::IdentityFunctor());
}

const f64 TL = 1.95;
timer TIMER;
const int N = 500;
const int M = 50;
const i64 L = (i64)(1e15 - 2 * 1e12);
const i64 U = (i64)(1e15 + 2 * 1e12);
void read() {
  { i64 n,m,l,u; cin>>n>>m>>l>>u;
    do { if(!(n == N)) { throw runtime_error("main.cpp" ":" "21" " Assertion failed: " "n == N"); } } while(0);
    do { if(!(m == M)) { throw runtime_error("main.cpp" ":" "22" " Assertion failed: " "m == M"); } } while(0);
    do { if(!(l == L)) { throw runtime_error("main.cpp" ":" "23" " Assertion failed: " "l == L"); } } while(0);
    do { if(!(u == U)) { throw runtime_error("main.cpp" ":" "24" " Assertion failed: " "u == U"); } } while(0);
  }
}
struct history_list {
  int l, r;
};
const int MAX_HISTORY = 1<<22;
history_list history_pool[MAX_HISTORY];
int history_next = 0;
int make_history(int l, int r) {
  history_pool[history_next] = { l, r };
  return history_next++;
}
struct value_with_history {
  i64 value;
  int history;
  bool operator<(value_with_history const& other) const {
    return value < other.value;
  }
};
value_with_history add(value_with_history v1, value_with_history v2) {
  return {v1.value+v2.value, make_history(v1.history, v2.history)};
}
const int BITS = 18;
const int MAX_SIZE = 100'000;
const int BASE_SIZE = 40'000;
const int POST_SIZE = 55'000;
const int NUM_USE = 8;
const int NREPEAT_INNER = 2;
vector<value_with_history> find_subset
(int SIZE,
 i64 goal, int size, vector<value_with_history> const& A,
 bool root = false) {
  i64 rgoal = goal/NUM_USE*size;
  if(size == 0) {
    return {{0, 0}};
  }else if(size == 1) {
    vector<value_with_history> ret;
    for(i32 i = 0; i < (i32)(A.size()); ++i) ret.push_back(A[i]);
    ska_sort(begin(ret), end(ret), [&](value_with_history const& v) { return v.value; });
    return ret;
  }else if(size == 2) {
    vector<value_with_history> ret;
    ret.reserve(A.size()*A.size());
    for(i32 i = 0; i < (i32)(A.size()); ++i) for(i32 j = 0; j < (i32)(i); ++j) ret.push_back(add(A[i], A[j]));
    ska_sort(begin(ret), end(ret), [&](value_with_history const& v) { return v.value; });
    return ret;
  }else if(A.size() > BITS) {
    vector<value_with_history> L, R;
    for(i32 i = 0; i < (i32)(A.size()); ++i) {
      if(i&1) L.push_back(A[i]);
      else R.push_back(A[i]);
    }
    auto SL = find_subset(SIZE, goal, size / 2, L);
    auto SR = find_subset(SIZE, goal, (size+1) / 2, R);
    static tuple<i64,int,int> Q1[MAX_SIZE];
    static tuple<i64,int,int> Q2[MAX_SIZE];
    int nq1 = 0, nq2 = 0;
    auto cmp = [&](tuple<i64,int,int> const& a, tuple<i64,int,int> const& b) __attribute__((always_inline)) {
      return get<0>(a) > get<0>(b);
    };
    auto Q1_push = [&](tuple<i64,int,int> t) __attribute__((always_inline)) {
      Q1[nq1++] = t;
      push_heap(Q1, Q1+nq1, cmp);
    };
    auto Q1_pop = [&]() __attribute__((always_inline)) {
      pop_heap(Q1, Q1+nq1, cmp);
      nq1 -= 1;
    };
    auto Q2_push = [&](tuple<i64,int,int> t) __attribute__((always_inline)) {
      Q2[nq2++] = t;
      push_heap(Q2, Q2+nq2, cmp);
    };
    auto Q2_pop = [&]() __attribute__((always_inline)) {
      pop_heap(Q2, Q2+nq2, cmp);
      nq2 -= 1;
    };
    if(root) {
      int j = SR.size()-1;
      i64 best = 1e18;
      int bi=-1,bj=-1;
      for(i32 i = 0; i < (i32)(SL.size()); ++i) {
        while(j>=0 && SL[i].value+SR[j].value > rgoal) {
          j -= 1;
        }
        if(j >= 0) {
          auto v = abs(SL[i].value+SR[j].value - rgoal);
          if(v < best) {
            best = v; bi = i; bj = j;
          }
        }
        if(j+1 < (int)SR.size()) {
          auto v = abs(SL[i].value+SR[j+1].value - rgoal);
          if(v < best) {
            best = v; bi = i; bj = j+1;
          }
        }
      }
      vector<value_with_history> ret;
      ret.push_back(add(SL[bi],SR[bj]));
      return ret;
    }
    // min_queue<tuple<i64, int, int>> Q1, Q2;
    int j = SR.size()-1;
    for(i32 i = 0; i < (i32)(SL.size()); ++i) {
      while(j>=0 && SL[i].value+SR[j].value > rgoal) {
        j -= 1;
      }
      if(j >= 0) {
        Q1_push({ abs(SL[i].value+SR[j].value - rgoal), i, j });
      }
      if(j+1 < (int)SR.size()) {
        Q2_push({ abs(SL[i].value+SR[j+1].value - rgoal), i, j+1 });
      }
    }
    vector<value_with_history> ret;
    ret.reserve(SIZE);
    while((int)ret.size() < SIZE) {
      if(nq1 > 0 && nq2 > 0) {
        if(get<0>(Q1[0]) < get<0>(Q2[0])) {
          auto [r,i,j] = Q1[0]; Q1_pop();
          ret.push_back(add(SL[i],SR[j]));
          if(j-1 >= 0) Q1_push({ abs(SL[i].value+SR[j-1].value - rgoal), i, j-1 });
        }else{
          auto [r,i,j] = Q2[0]; Q2_pop();
          ret.push_back(add(SL[i],SR[j]));
          if(j+1 < (int)SR.size()) Q2_push({ abs(SL[i].value+SR[j+1].value - rgoal), i, j+1 });
        }
      }else if(nq1 > 0) {
        auto [r,i,j] = Q1[0]; Q1_pop();
        ret.push_back(add(SL[i],SR[j]));
        if(j-1 >= 0) Q1_push({ abs(SL[i].value+SR[j-1].value - rgoal), i, j-1 });
      }else if(nq2 > 0) {
        auto [r,i,j] = Q2[0]; Q2_pop();
        ret.push_back(add(SL[i],SR[j]));
        if(j+1 < (int)SR.size()) Q2_push({ abs(SL[i].value+SR[j+1].value - rgoal), i, j+1 });
      }else{
        break;
      }
    }
    ska_sort(begin(ret), end(ret), [&](value_with_history const& v) { return v.value; });
    return ret;
  }else{
    vector<value_with_history> ret;
    ret.reserve(SIZE);
    for(i32 m = 0; m < (i32)(bit(A.size())); ++m) if((int)popcount(m) == size) {
      value_with_history sum = {0,0};
      for(i32 i = 0; i < (i32)(A.size()); ++i) if(m&bit(i)) sum = add(sum, A[i]);
      ret.push_back(sum);
    }
    ska_sort(begin(ret), end(ret), [&](value_with_history const& v) { return v.value; });
    return ret;
  }
}
vector<int> solve_once(vector<value_with_history> const& A, vector<i64> const& B)
{
  vector<int> solution(N, 0);
  vector<int> available(N, 1);
  vector<int> I(M); iota(begin(I), end(I), 0);
  i64 mid = (U+2*L)/3;
  sort(begin(I), end(I), [&](int i, int j) {
    return abs(B[i]-mid) > abs(B[j]-mid);
  });
  for(int i : I) {
    history_next = 1;
    auto best = U;
    auto besth = 0;
    for(i32 step = 0; step < (i32)(NREPEAT_INNER); ++step) {
      vector<value_with_history> X;
      for(i32 i = 0; i < (i32)(N); ++i) if(available[i]) X.push_back(A[i]);
      rng.shuffle(X);
      auto R = find_subset(BASE_SIZE, B[i], NUM_USE, X, true);
      for(auto x : R) {
        auto v = abs(x.value-B[i]);
        if(v < best) {
          best = v;
          besth = x.history;
        }
      }
    }
    do { cerr << "main.cpp" ":" "221" "  {" << "best" << "} = {"; debug_impl_seq(best); cerr << endl << flush; } while(0);
    vector<int> S;
    auto recon = [&](auto&& recon, int i) -> void {
      if(i == 0) return;
      if(i < 0) {
        S.push_back(-1-i);
        return;
      }else{
        auto h = history_pool[i];
        recon(recon, h.l);
        recon(recon, h.r);
      }
    };
    recon(recon, besth);
    for(int x : S) {
      do { if(!(available[x])) { throw runtime_error("main.cpp" ":" "238" " Assertion failed: " "available[x]"); } } while(0);
      available[x] = 0;
      solution[x] = i+1;
    }
  }
  return solution;
}
void solve() {
  vector<value_with_history> A(N);
  const i64 LL = L - 2.2e12;
  const i64 UU = U + 2.2e12;
  RNG lrng; lrng.reset(43);
  for(i32 i = 0; i < (i32)(N); ++i) A[i] = {
    (i64)lrng.randomRange64(LL/NUM_USE, UU/NUM_USE),
    -1-i
  };
  for(i32 i = 0; i < (i32)(N); ++i) cout << A[i].value << ' ';
  cout << endl;
  vector<i64> B(M);
  for(i32 i = 0; i < (i32)(M); ++i) cin>>B[i];
  vector<int> solution = solve_once(A,B);
  while(TIMER.elapsed() < TL) {
    vector<i64> sums(M);
    for(i32 i = 0; i < (i32)(N); ++i) if(solution[i] > 0) sums[solution[i]-1] += A[i].value;
    vector<tuple<i64, i64>> D;
    for(i32 j = 0; j < (i32)(M); ++j) {
      i64 d = abs(B[j]-sums[j]);
      D.push_back({d,j});
    }
    pdqsort(begin(D), end(D), greater<>());
    D.resize(2);
    int i = get<1>(rng.sample(D));
    vector<int> available(N, 1);
    for(i32 j = 0; j < (i32)(N); ++j) if(solution[j] > 0 && solution[j] != i+1) available[j] = 0;
    i64 cursum = 0;
    for(i32 j = 0; j < (i32)(N); ++j) if(solution[j] == i+1) cursum += A[j].value;
    i64 curval = abs(cursum-B[i]);
    history_next = 1;
    auto best = curval+1;
    auto besth = 0;
    vector<value_with_history> X;
    for(i32 i = 0; i < (i32)(N); ++i) if(available[i]) X.push_back(A[i]);
    rng.shuffle(X);
    auto R = find_subset(POST_SIZE, B[i], NUM_USE, X, true);
    for(auto x : R) {
      auto v = abs(x.value-B[i]);
      if(v < best) {
        best = v;
        besth = x.history;
      }
    }
    if(besth == 0) continue;
    do { cerr << "main.cpp" ":" "303" "  {" << "\"found\", curval-best" << "} = {"; debug_impl_seq("found", curval-best); cerr << endl << flush; } while(0);
    for(i32 j = 0; j < (i32)(N); ++j) if(solution[j] == i+1) solution[j] = 0;
    vector<int> S;
    auto recon = [&](auto&& recon, int i) -> void {
      if(i == 0) return;
      if(i < 0) {
        S.push_back(-1-i);
        return;
      }else{
        auto h = history_pool[i];
        recon(recon, h.l);
        recon(recon, h.r);
      }
    };
    recon(recon, besth);
    for(int x : S) {
      do { if(!(available[x])) { throw runtime_error("main.cpp" ":" "322" " Assertion failed: " "available[x]"); } } while(0);
      solution[x] = i+1;
    }
  }
  for(i32 i = 0; i < (i32)(N); ++i) cout << solution[i] << ' ';
  cout << endl;
}
int main() {
  cerr << setprecision(5) << fixed;
  TIMER.reset();
  read();
  solve();
  cerr << "[DATA] time = " << (i32)(TIMER.elapsed()*1000) << endl;
  return 0;
}
