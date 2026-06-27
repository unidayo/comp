


 #include <stdio.h>
 #include <bits/stdc++.h>
 #include <ext/pb_ds/assoc_container.hpp>







namespace pdqsort_detail {
    enum {

        insertion_sort_threshold = 24,


        ninther_threshold = 128,



        partial_insertion_sort_limit = 8,


        block_size = 64,


        cacheline_size = 64

    };


    template<class T> struct is_default_compare : std::false_type { };
    template<class T> struct is_default_compare<std::less<T>> : std::true_type { };
    template<class T> struct is_default_compare<std::greater<T>> : std::true_type { };



    template<class T>
    inline int log2(T n) {
        int log = 0;
        while (n >>= 1) ++log;
        return log;
    }


    template<class Iter, class Compare>
    inline void insertion_sort(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;
        if (begin == end) return;

        for (Iter cur = begin + 1; cur != end; ++cur) {
            Iter sift = cur;
            Iter sift_1 = cur - 1;


            if (comp(*sift, *sift_1)) {
                T tmp = std::move(*sift);

                do { *sift-- = std::move(*sift_1); }
                while (sift != begin && comp(tmp, *--sift_1));

                *sift = std::move(tmp);
            }
        }
    }



    template<class Iter, class Compare>
    inline void unguarded_insertion_sort(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;
        if (begin == end) return;

        for (Iter cur = begin + 1; cur != end; ++cur) {
            Iter sift = cur;
            Iter sift_1 = cur - 1;


            if (comp(*sift, *sift_1)) {
                T tmp = std::move(*sift);

                do { *sift-- = std::move(*sift_1); }
                while (comp(tmp, *--sift_1));

                *sift = std::move(tmp);
            }
        }
    }




    template<class Iter, class Compare>
    inline bool partial_insertion_sort(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;
        if (begin == end) return true;

        std::size_t limit = 0;
        for (Iter cur = begin + 1; cur != end; ++cur) {
            Iter sift = cur;
            Iter sift_1 = cur - 1;


            if (comp(*sift, *sift_1)) {
                T tmp = std::move(*sift);

                do { *sift-- = std::move(*sift_1); }
                while (sift != begin && comp(tmp, *--sift_1));

                *sift = std::move(tmp);
                limit += cur - sift;
            }

            if (limit > partial_insertion_sort_limit) return false;
        }

        return true;
    }

    template<class Iter, class Compare>
    inline void sort2(Iter a, Iter b, Compare comp) {
        if (comp(*b, *a)) std::iter_swap(a, b);
    }


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






    template<class Iter, class Compare>
    inline std::pair<Iter, bool> partition_right_branchless(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;


        T pivot(std::move(*begin));
        Iter first = begin;
        Iter last = end;



        while (comp(*++first, pivot));



        if (first - 1 == begin) while (first < last && !comp(*--last, pivot));
        else while ( !comp(*--last, pivot));



        bool already_partitioned = first >= last;
        if (!already_partitioned) {
            std::iter_swap(first, last);
            ++first;
        }



        unsigned char offsets_l_storage[block_size + cacheline_size];
        unsigned char offsets_r_storage[block_size + cacheline_size];
        unsigned char* offsets_l = align_cacheline(offsets_l_storage);
        unsigned char* offsets_r = align_cacheline(offsets_r_storage);
        int num_l, num_r, start_l, start_r;
        num_l = num_r = start_l = start_r = 0;

        while (last - first > 2 * block_size) {

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


            int num = std::min(num_l, num_r);
            swap_offsets(first, last, offsets_l + start_l, offsets_r + start_r,
                         num, num_l == num_r);
            num_l -= num; num_r -= num;
            start_l += num; start_r += num;
            if (num_l == 0) first += block_size;
            if (num_r == 0) last -= block_size;
        }

        int l_size = 0, r_size = 0;
        int unknown_left = (int)(last - first) - ((num_r || num_l) ? block_size : 0);
        if (num_r) {

            l_size = unknown_left;
            r_size = block_size;
        } else if (num_l) {
            l_size = block_size;
            r_size = unknown_left;
        } else {

            l_size = unknown_left/2;
            r_size = unknown_left - l_size;
        }


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


        Iter pivot_pos = first - 1;
        *begin = std::move(*pivot_pos);
        *pivot_pos = std::move(pivot);

        return std::make_pair(pivot_pos, already_partitioned);
    }






    template<class Iter, class Compare>
    inline std::pair<Iter, bool> partition_right(Iter begin, Iter end, Compare comp) {
        typedef typename std::iterator_traits<Iter>::value_type T;


        T pivot(std::move(*begin));

        Iter first = begin;
        Iter last = end;



        while (comp(*++first, pivot));



        if (first - 1 == begin) while (first < last && !comp(*--last, pivot));
        else while ( !comp(*--last, pivot));



        bool already_partitioned = first >= last;




        while (first < last) {
            std::iter_swap(first, last);
            while (comp(*++first, pivot));
            while (!comp(*--last, pivot));
        }


        Iter pivot_pos = first - 1;
        *begin = std::move(*pivot_pos);
        *pivot_pos = std::move(pivot);

        return std::make_pair(pivot_pos, already_partitioned);
    }





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


        while (true) {
            diff_t size = end - begin;


            if (size < insertion_sort_threshold) {
                if (leftmost) insertion_sort(begin, end, comp);
                else unguarded_insertion_sort(begin, end, comp);
                return;
            }


            diff_t s2 = size / 2;
            if (size > ninther_threshold) {
                sort3(begin, begin + s2, end - 1, comp);
                sort3(begin + 1, begin + (s2 - 1), end - 2, comp);
                sort3(begin + 2, begin + (s2 + 1), end - 3, comp);
                sort3(begin + (s2 - 1), begin + s2, begin + (s2 + 1), comp);
                std::iter_swap(begin, begin + s2);
            } else sort3(begin + s2, begin, end - 1, comp);






            if (!leftmost && !comp(*(begin - 1), *begin)) {
                begin = partition_left(begin, end, comp) + 1;
                continue;
            }


            std::pair<Iter, bool> part_result =
                Branchless ? partition_right_branchless(begin, end, comp)
                           : partition_right(begin, end, comp);
            Iter pivot_pos = part_result.first;
            bool already_partitioned = part_result.second;


            diff_t l_size = pivot_pos - begin;
            diff_t r_size = end - (pivot_pos + 1);
            bool highly_unbalanced = l_size < size / 8 || r_size < size / 8;


            if (highly_unbalanced) {

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


                if (already_partitioned && partial_insertion_sort(begin, pivot_pos, comp)
                                        && partial_insertion_sort(pivot_pos + 1, end, comp)) return;
            }



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
using pii = tuple<int,int>;
using vi = vector<int>;
using vvi = vector<vi>;
using vii = vector<pii>;
using vvii = vector<vii>;

template <class T> using min_queue = priority_queue<T, vector<T>, greater<T>>;
template <class T> using max_queue = priority_queue<T>;



template<class T>
void print_collection(ostream& out, T const& x);

template<class A>
ostream& operator<<(ostream& out, vector<A> const& x) { print_collection(out, x); return out; }
template<class A, int N>
ostream& operator<<(ostream& out, array<A, N> const& x) { print_collection(out, x); return out; }
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
template<class A>
ostream& operator<<(ostream& out, unordered_set<A> const& x) { print_collection(out, x); return out; }
template<class A, class B>
ostream& operator<<(ostream& out, unordered_map<A, B> const& x) { print_collection(out, x); return out; }


template<class T, size_t... I>
void print_tuple(ostream& out, T const& a, index_sequence<I...>);
template<class... A>
ostream& operator<<(ostream& out, tuple<A...> const& x) {
  print_tuple(out, x, index_sequence_for<A...>{});
  return out;
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



static struct xorshift128_t {
  uint32_t x, y, z, w;

  xorshift128_t() {
    reset(1);
  }

  inline void reset(uint64_t seed) {

    struct splitmix64_state {
      uint64_t s;

      uint64_t splitmix64() {
        uint64_t result = (s += 0x9E3779B97f4A7C15);
        result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
        result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
        return result ^ (result >> 31);
      }
    };

    splitmix64_state s { seed };

    x = (uint32_t)s.splitmix64();
    y = (uint32_t)s.splitmix64();
    z = (uint32_t)s.splitmix64();
    w = (uint32_t)s.splitmix64();
  }

  inline uint32_t xorshift128() {
    uint32_t t = x;
    t ^= t << 11;
    t ^= t >> 8;
    x = y; y = z; z = w;
    w ^= w >> 19;
    w ^= t;
    return w;
  }
} xorshift128_s;

static inline void random_reset(long long int seed = chrono::steady_clock::now().time_since_epoch().count()) { xorshift128_s.reset(seed); }

static inline uint32_t randomInt32() {
  return xorshift128_s.xorshift128();
}

static inline uint64_t randomInt64() {
  return (((uint64_t)randomInt32())<<32ll) | ((uint64_t)randomInt32());
}

static inline long long int random(long long int r) {
  return xorshift128_s.xorshift128()%r;
}

static inline long long int random(long long int l, long long int r) {
  return l + random(r-l+1);
}

static inline double randomDouble() {
  return (double)random(2147483648ll) / 2147483648.0;
}



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



template<class Fun>
class letrec_result {
  Fun fun_;
  public:
    template<class T>
    explicit letrec_result(T &&fun): fun_(forward<T>(fun)) {}

    template<class ...Args>
    decltype(auto) operator()(Args &&...args) {
      return fun_(ref(*this), forward<Args>(args)...);
    }
};

template<class Fun>
decltype(auto) letrec(Fun &&fun) {
  return letrec_result<decay_t<Fun>>(forward<Fun>(fun));
}



template<class T>
T& smin(T& x, T const& y) { x = min(x,y); return x; }

template <class T>
T& smax(T& x, T const& y) { x = max(x, y); return x; }



struct union_find {
  vector<int> A;

  union_find(int n = 0) : A(n) {
    iota(begin(A), end(A), 0);
  }

  int find(int a) {
    return A[a] == a ? a : A[a] = find(A[a]);
  }

  bool unite(int a, int b) {
    a = find(a); b = find(b);
    if(a == b) return false;
    A[a] = b;
    return true;
  }
};



static inline void debug_impl_seq() {
  cerr << "}";
}

template <class T, class... V>
void debug_impl_seq(T const& t, V const&... v) {
  cerr << t;
  if(sizeof...(v)) { cerr << ", "; }
  debug_impl_seq(v...);
}
struct sa_t {
  float time_limit;
  float done;

  float temp0;
  float alpha;
  timer t;

  sa_t() {}

  sa_t(double t0, double a, double tl) {
    temp0 = t0;
    alpha = a;
    time_limit = tl;
    updateDone();
  }

  void updateDone() {
    done = t.elapsed() / time_limit;
  }

  bool accept(float x) {
    bool r = (x < 0) || randomDouble() * pow(1-done, alpha) * temp0 > x;
    return r;
  }
};



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


using gzFile = int;
int gzread(gzFile, unsigned char*, int){assert(0);return 0;}

timer satTimer;
double satTL = 1.0;
struct TimeLimitException{};

using namespace std;


namespace Glucose {




  class OutOfMemoryException{};
  static inline void* xrealloc(void *ptr, size_t size)
  {
    void* mem = realloc(ptr, size);
    if (mem == NULL && errno == ENOMEM){
      throw OutOfMemoryException();
    }else {
      return mem;
    }
  }


}







namespace Glucose {






template<class T>
struct vec {
    T* data;
    int sz;
    int cap;


    vec<T>& operator = (vec<T>& ) { assert(0); return *this; }
             vec (vec<T>& ) { assert(0); }


    static inline int imax (int x, int y) { int mask = (y-x) >> (sizeof(int)*8-1); return (x&mask) + (y&(~mask)); }

    static inline void nextCap(int& cap){ cap += ((cap >> 1) + 2) & ~1; }

public:

    vec() : data(NULL) , sz(0) , cap(0) { }
    explicit vec(int size) : data(NULL) , sz(0) , cap(0) { growTo(size); }
    vec(int size, const T& pad) : data(NULL) , sz(0) , cap(0) { growTo(size, pad); }
   ~vec() { clear(true); }


    operator T* (void) { return data; }


    int size (void) const { return sz; }
    void shrink (int nelems) { assert(nelems <= sz); for (int i = 0; i < nelems; i++) sz--, data[sz].~T(); }
    void shrink_ (int nelems) { assert(nelems <= sz); sz -= nelems; }
    int capacity (void) const { return cap; }
    void capacity (int min_cap);
    void growTo (int size);
    void growTo (int size, const T& pad);
    void clear (bool dealloc = false);


    void push (void) { if (sz == cap) capacity(sz+1); new (&data[sz]) T(); sz++; }
    void push (const T& elem) { if (sz == cap) capacity(sz+1); data[sz++] = elem; }
    void push_ (const T& elem) { assert(sz < cap); data[sz++] = elem; }
    void pop (void) { assert(sz > 0); sz--, data[sz].~T(); }





    const T& last (void) const { return data[sz-1]; }
    T& last (void) { return data[sz-1]; }


    const T& operator [] (int index) const { return data[index]; }
    T& operator [] (int index) { return data[index]; }


    void copyTo(vec<T>& copy) const { copy.clear(); copy.growTo(sz); for (int i = 0; i < sz; i++) copy[i] = data[i]; }
    void moveTo(vec<T>& dest) { dest.clear(true); dest.data = data; dest.sz = sz; dest.cap = cap; data = NULL; sz = 0; cap = 0; }
    void memCopyTo(vec<T>& copy) const{
        copy.capacity(cap);
        copy.sz = sz;
        memcpy(copy.data,data,sizeof(T)*cap);
    }

};


template<class T>
void vec<T>::capacity(int min_cap) {
    if (cap >= min_cap) return;
    int add = imax((min_cap - cap + 1) & ~1, ((cap >> 1) + 2) & ~1);
    if (add > INT_MAX - cap || (((data = (T*)::realloc(data, (cap += add) * sizeof(T))) == NULL) && errno == ENOMEM))
        throw OutOfMemoryException();
 }


template<class T>
void vec<T>::growTo(int size, const T& pad) {
    if (sz >= size) return;
    capacity(size);
    for (int i = sz; i < size; i++) data[i] = pad;
    sz = size; }


template<class T>
void vec<T>::growTo(int size) {
    if (sz >= size) return;
    capacity(size);
    for (int i = sz; i < size; i++) new (&data[i]) T();
    sz = size; }


template<class T>
void vec<T>::clear(bool dealloc) {
    if (data != NULL){
        for (int i = 0; i < sz; i++) data[i].~T();
        sz = 0;
        if (dealloc) free(data), data = NULL, cap = 0; } }


}

namespace Glucose {
template<class V, class T>
static inline void remove(V& ts, const T& t)
{
    int j = 0;
    for (; j < ts.size() && ts[j] != t; j++);
    assert(j < ts.size());
    for (; j < ts.size()-1; j++) ts[j] = ts[j+1];
    ts.pop();
}


template<class V, class T>
static inline bool find(V& ts, const T& t)
{
    int j = 0;
    for (; j < ts.size() && ts[j] != t; j++);
    return j < ts.size();
}







template<class T>
static inline void copy(const T& from, T& to)
{
    to = from;
}


template<class T>
static inline void copy(const vec<T>& from, vec<T>& to, bool append = false)
{
    if (!append)
        to.clear();
    for (int i = 0; i < from.size(); i++){
        to.push();
        copy(from[i], to.last());
    }
}

template<class T>
static inline void append(const vec<T>& from, vec<T>& to){ copy(from, to, true); }


}

namespace Glucose {




template<class T>
class RegionAllocator
{
    T* memory;
    uint32_t sz;
    uint32_t cap;
    uint32_t wasted_;

    void capacity(uint32_t min_cap);

 public:

    typedef uint32_t Ref;
    enum { Ref_Undef = UINT32_MAX };
    enum { Unit_Size = sizeof(uint32_t) };

    explicit RegionAllocator(uint32_t start_cap = 1024*1024) : memory(NULL), sz(0), cap(0), wasted_(0){ capacity(start_cap); }
    ~RegionAllocator()
    {
        if (memory != NULL)
            ::free(memory);
    }


    uint32_t size () const { return sz; }
    uint32_t getCap () const { return cap;}
    uint32_t wasted () const { return wasted_; }

    Ref alloc (int size);
    void free (int size) { wasted_ += size; }


    T& operator[](Ref r) { assert(r >= 0 && r < sz); return memory[r]; }
    const T& operator[](Ref r) const { assert(r >= 0 && r < sz); return memory[r]; }

    T* lea (Ref r) { assert(r >= 0 && r < sz); return &memory[r]; }
    const T* lea (Ref r) const { assert(r >= 0 && r < sz); return &memory[r]; }
    Ref ael (const T* t) { assert((void*)t >= (void*)&memory[0] && (void*)t < (void*)&memory[sz-1]);
        return (Ref)(t - &memory[0]); }

    void moveTo(RegionAllocator& to) {
        if (to.memory != NULL) ::free(to.memory);
        to.memory = memory;
        to.sz = sz;
        to.cap = cap;
        to.wasted_ = wasted_;

        memory = NULL;
        sz = cap = wasted_ = 0;
    }

    void copyTo(RegionAllocator& to) const {

        to.memory = (T*)xrealloc(to.memory, sizeof(T)*cap);
        memcpy(to.memory,memory,sizeof(T)*cap);
        to.sz = sz;
        to.cap = cap;
        to.wasted_ = wasted_;
    }



};

template<class T>
void RegionAllocator<T>::capacity(uint32_t min_cap)
{
    if (cap >= min_cap) return;

    uint32_t prev_cap = cap;
    while (cap < min_cap){




        uint32_t delta = ((cap >> 1) + (cap >> 3) + 2) & ~1;
        cap += delta;

        if (cap <= prev_cap)
            throw OutOfMemoryException();
    }


    assert(cap > 0);
    memory = (T*)xrealloc(memory, sizeof(T)*cap);
}


template<class T>
typename RegionAllocator<T>::Ref
RegionAllocator<T>::alloc(int size)
{

    assert(size > 0);
    capacity(sz + size);

    uint32_t prev_sz = sz;
    sz += size;


    if (sz < prev_sz)
        throw OutOfMemoryException();

    return prev_sz;
}



}


namespace Glucose {

  class Clone {
  public:
    virtual Clone* clone() const = 0;
  };
};


namespace Glucose {





template<class Comp>
class Heap {
    Comp lt;
    vec<int> heap;
    vec<int> indices;


    static inline int left (int i) { return i*2+1; }
    static inline int right (int i) { return (i+1)*2; }
    static inline int parent(int i) { return (i-1) >> 1; }



    void percolateUp(int i)
    {
        int x = heap[i];
        int p = parent(i);

        while (i != 0 && lt(x, heap[p])){
            heap[i] = heap[p];
            indices[heap[p]] = i;
            i = p;
            p = parent(p);
        }
        heap [i] = x;
        indices[x] = i;
    }


    void percolateDown(int i)
    {
        int x = heap[i];
        while (left(i) < heap.size()){
            int child = right(i) < heap.size() && lt(heap[right(i)], heap[left(i)]) ? right(i) : left(i);
            if (!lt(heap[child], x)) break;
            heap[i] = heap[child];
            indices[heap[i]] = i;
            i = child;
        }
        heap [i] = x;
        indices[x] = i;
    }


  public:
    Heap(const Comp& c) : lt(c) { }

    int size () const { return heap.size(); }
    bool empty () const { return heap.size() == 0; }
    bool inHeap (int n) const { return n < indices.size() && indices[n] >= 0; }
    int operator[](int index) const { assert(index < heap.size()); return heap[index]; }


    void decrease (int n) { assert(inHeap(n)); percolateUp (indices[n]); }
    void increase (int n) { assert(inHeap(n)); percolateDown(indices[n]); }

    void copyTo(Heap& copy) const {heap.copyTo(copy.heap);indices.copyTo(copy.indices);}


    void update(int n)
    {
        if (!inHeap(n))
            insert(n);
        else {
            percolateUp(indices[n]);
            percolateDown(indices[n]); }
    }


    void insert(int n)
    {
        indices.growTo(n+1, -1);
        assert(!inHeap(n));

        indices[n] = heap.size();
        heap.push(n);
        percolateUp(indices[n]);
    }


    int removeMin()
    {
        int x = heap[0];
        heap[0] = heap.last();
        indices[heap[0]] = 0;
        indices[x] = -1;
        heap.pop();
        if (heap.size() > 1) percolateDown(0);
        return x;
    }



    void build(vec<int>& ns) {
        for (int i = 0; i < heap.size(); i++)
            indices[heap[i]] = -1;
        heap.clear();

        for (int i = 0; i < ns.size(); i++){
            indices[ns[i]] = i;
            heap.push(ns[i]); }

        for (int i = heap.size() / 2 - 1; i >= 0; i--)
            percolateDown(i);
    }

    void clear(bool dealloc = false)
    {
        for (int i = 0; i < heap.size(); i++)
            indices[heap[i]] = -1;
        heap.clear(dealloc);
    }
};



}


namespace Glucose {





static inline uint32_t hash(uint32_t x){ return x; }
static inline uint32_t hash(uint64_t x){ return (uint32_t)x; }
static inline uint32_t hash(int32_t x) { return (uint32_t)x; }
static inline uint32_t hash(int64_t x) { return (uint32_t)x; }

template<class K> struct Hash { uint32_t operator()(const K& k) const { return hash(k); } };
template<class K> struct Equal { bool operator()(const K& k1, const K& k2) const { return k1 == k2; } };

template<class K> struct DeepHash { uint32_t operator()(const K* k) const { return hash(*k); } };
template<class K> struct DeepEqual { bool operator()(const K* k1, const K* k2) const { return *k1 == *k2; } };






static const int nprimes = 25;
static const int primes [nprimes] = { 31, 73, 151, 313, 643, 1291, 2593, 5233, 10501, 21013, 42073, 84181, 168451, 337219, 674701, 1349473, 2699299, 5398891, 10798093, 21596719, 43193641, 86387383, 172775299, 345550609, 691101253 };





template<class K, class D, class H = Hash<K>, class E = Equal<K> >
class Map {
 public:
    struct Pair { K key; D data; };

 public:
    H hash;
    E equals;

    vec<Pair>* table;
    int cap;
    int size;


    Map<K,D,H,E>& operator = (Map<K,D,H,E>& ) { assert(0); }
                   Map (Map<K,D,H,E>& ) { assert(0); }

    bool checkCap(int new_size) const { return new_size > cap; }

    int32_t index (const K& k) const { return hash(k) % cap; }
    void _insert (const K& k, const D& d) {
        vec<Pair>& ps = table[index(k)];
        ps.push(); ps.last().key = k; ps.last().data = d; }

    void rehash () {
        const vec<Pair>* old = table;

        int old_cap = cap;
        int newsize = primes[0];
        for (int i = 1; newsize <= cap && i < nprimes; i++)
           newsize = primes[i];

        table = new vec<Pair>[newsize];
        cap = newsize;

        for (int i = 0; i < old_cap; i++){
            for (int j = 0; j < old[i].size(); j++){
                _insert(old[i][j].key, old[i][j].data); }}

        delete [] old;


    }


 public:

    Map () : table(NULL), cap(0), size(0) {}
    Map (const H& h, const E& e) : hash(h), equals(e), table(NULL), cap(0), size(0){}
    ~Map () { delete [] table; }


    const D& operator [] (const K& k) const
    {
        assert(size != 0);
        const D* res = NULL;
        const vec<Pair>& ps = table[index(k)];
        for (int i = 0; i < ps.size(); i++)
            if (equals(ps[i].key, k))
                res = &ps[i].data;
        assert(res != NULL);
        return *res;
    }


    D& operator [] (const K& k)
    {
        assert(size != 0);
        D* res = NULL;
        vec<Pair>& ps = table[index(k)];
        for (int i = 0; i < ps.size(); i++)
            if (equals(ps[i].key, k))
                res = &ps[i].data;
        assert(res != NULL);
        return *res;
    }


    void insert (const K& k, const D& d) { if (checkCap(size+1)) rehash(); _insert(k, d); size++; }
    bool peek (const K& k, D& d) const {
        if (size == 0) return false;
        const vec<Pair>& ps = table[index(k)];
        for (int i = 0; i < ps.size(); i++)
            if (equals(ps[i].key, k)){
                d = ps[i].data;
                return true; }
        return false;
    }

    bool has (const K& k) const {
        if (size == 0) return false;
        const vec<Pair>& ps = table[index(k)];
        for (int i = 0; i < ps.size(); i++)
            if (equals(ps[i].key, k))
                return true;
        return false;
    }


    void remove(const K& k) {
        assert(table != NULL);
        vec<Pair>& ps = table[index(k)];
        int j = 0;
        for (; j < ps.size() && !equals(ps[j].key, k); j++);
        assert(j < ps.size());
        ps[j] = ps.last();
        ps.pop();
        size--;
    }

    void clear () {
        cap = size = 0;
        delete [] table;
        table = NULL;
    }

    int elems() const { return size; }
    int bucket_count() const { return cap; }


    void moveTo(Map& other){
        delete [] other.table;

        other.table = table;
        other.cap = cap;
        other.size = size;

        table = NULL;
        size = cap = 0;
    }


    const vec<Pair>& bucket(int i) const { return table[i]; }
};


}


namespace Glucose {



template<class T>
class Queue {
    vec<T> buf;
    int first;
    int end;

public:
    typedef T Key;

    Queue() : buf(1), first(0), end(0) {}

    void clear (bool dealloc = false) { buf.clear(dealloc); buf.growTo(1); first = end = 0; }
    int size () const { return (end >= first) ? end - first : end - first + buf.size(); }



    const T& operator [] (int index) const { assert(index >= 0); assert(index < size()); return buf[(first + index) % buf.size()]; }
    T& operator [] (int index) { assert(index >= 0); assert(index < size()); return buf[(first + index) % buf.size()]; }

    T peek () const { assert(first != end); return buf[first]; }
    void pop () { assert(first != end); first++; if (first == buf.size()) first = 0; }


    void copyTo(Queue<T>& copy) const {
        copy.first = first;
        copy.end = end;
        buf.memCopyTo(copy.buf);
    }


    void insert(T elem) {
        buf[end++] = elem;
        if (end == buf.size()) end = 0;
        if (first == end){
            vec<T> tmp((buf.size()*3 + 1) >> 1);

            int i = 0;
            for (int j = first; j < buf.size(); j++) tmp[i++] = buf[j];
            for (int j = 0 ; j < end ; j++) tmp[i++] = buf[j];
            first = 0;
            end = buf.size();
            tmp.moveTo(buf);
        }
    }
};



}


namespace Glucose {

template<class T>
struct LessThan_default {
    bool operator () (T x, T y) { return x < y; }
};


template <class T, class LessThan>
void selectionSort(T* array, int size, LessThan lt)
{
    int i, j, best_i;
    T tmp;

    for (i = 0; i < size-1; i++){
        best_i = i;
        for (j = i+1; j < size; j++){
            if (lt(array[j], array[best_i]))
                best_i = j;
        }
        tmp = array[i]; array[i] = array[best_i]; array[best_i] = tmp;
    }
}
template <class T> static inline void selectionSort(T* array, int size) {
    selectionSort(array, size, LessThan_default<T>()); }

template <class T, class LessThan>
void sort(T* array, int size, LessThan lt)
{
    if (size <= 15)
        selectionSort(array, size, lt);

    else{
        T pivot = array[size / 2];
        T tmp;
        int i = -1;
        int j = size;

        for(;;){
            do i++; while(lt(array[i], pivot));
            do j--; while(lt(pivot, array[j]));

            if (i >= j) break;

            tmp = array[i]; array[i] = array[j]; array[j] = tmp;
        }

        sort(array , i , lt);
        sort(&array[i], size-i, lt);
    }
}
template <class T> static inline void sort(T* array, int size) {
    sort(array, size, LessThan_default<T>()); }






template <class T, class LessThan> void sort(vec<T>& v, LessThan lt) {
    sort((T*)v, v.size(), lt); }
template <class T> void sort(vec<T>& v) {
    sort(v, LessThan_default<T>()); }



}


namespace Glucose {
typedef int Var;



struct Lit {
    int x;


    friend Lit mkLit(Var var, bool sign);

    bool operator == (Lit p) const { return x == p.x; }
    bool operator != (Lit p) const { return x != p.x; }
    bool operator < (Lit p) const { return x < p.x; }
};


inline Lit mkLit (Var var, bool sign = false) { Lit p; p.x = var + var + (int)sign; return p; }
inline Lit operator ~(Lit p) { Lit q; q.x = p.x ^ 1; return q; }
inline Lit operator ^(Lit p, bool b) { Lit q; q.x = p.x ^ (unsigned int)b; return q; }
inline bool sign (Lit p) { return p.x & 1; }
inline int var (Lit p) { return p.x >> 1; }


inline int toInt (Var v) { return v; }
inline int toInt (Lit p) { return p.x; }
inline Lit toLit (int i) { Lit p; p.x = i; return p; }




const Lit lit_Undef = { -2 };
const Lit lit_Error = { -1 };
class lbool {
    uint8_t value;

public:
    explicit lbool(uint8_t v) : value(v) { }

    lbool() : value(0) { }
    explicit lbool(bool x) : value(!x) { }

    bool operator == (lbool b) const { return ((b.value&2) & (value&2)) | (!(b.value&2)&(value == b.value)); }
    bool operator != (lbool b) const { return !(*this == b); }
    lbool operator ^ (bool b) const { return lbool((uint8_t)(value^(uint8_t)b)); }

    lbool operator && (lbool b) const {
        uint8_t sel = (this->value << 1) | (b.value << 3);
        uint8_t v = (0xF7F755F4 >> sel) & 3;
        return lbool(v); }

    lbool operator || (lbool b) const {
        uint8_t sel = (this->value << 1) | (b.value << 3);
        uint8_t v = (0xFCFCF400 >> sel) & 3;
        return lbool(v); }

    friend int toInt (lbool l);
    friend lbool toLbool(int v);
};
inline int toInt (lbool l) { return l.value; }
inline lbool toLbool(int v) { return lbool((uint8_t)v); }




class Clause;
typedef RegionAllocator<uint32_t>::Ref CRef;




class Clause {
    struct {
      unsigned mark : 2;
      unsigned learnt : 1;
      unsigned szWithoutSelectors : 19;
      unsigned canbedel : 1;
      unsigned extra_size : 2;
      unsigned size : 21;
      unsigned seen : 1;
      unsigned reloced : 1;
      unsigned exported : 2;
      unsigned oneWatched : 1;
      unsigned lbd : 13;
    } header;

    union { Lit lit; float act; uint32_t abs; CRef rel; } data[0];

    friend class ClauseAllocator;


    template<class V>
    Clause(const V& ps, int _extra_size, bool learnt) {
 assert(_extra_size < (1<<2));
        header.mark = 0;
        header.learnt = learnt;
        header.extra_size = _extra_size;
        header.reloced = 0;
        header.size = ps.size();
 header.lbd = 0;
 header.canbedel = 1;
 header.exported = 0;
 header.oneWatched = 0;
 header.seen = 0;
        for (int i = 0; i < ps.size(); i++)
            data[i].lit = ps[i];

        if (header.extra_size > 0){
   if (header.learnt)
                data[header.size].act = 0;
            else
                calcAbstraction();
   if (header.extra_size > 1) {
       data[header.size+1].abs = 0;
   }
 }
    }

public:
    void calcAbstraction() {
        assert(header.extra_size > 0);
        uint32_t abstraction = 0;
        for (int i = 0; i < size(); i++)
            abstraction |= 1 << (var(data[i].lit) & 31);
        data[header.size].abs = abstraction; }

    int size () const { return header.size; }
    void shrink (int i) { assert(i <= size());
      if (header.extra_size > 0) {
          data[header.size-i] = data[header.size];
          if (header.extra_size > 1) {
       data[header.size-i-1] = data[header.size-1];
          }
      }
    header.size -= i; }
    void pop () { shrink(1); }
    bool learnt () const { return header.learnt; }
    bool has_extra () const { return header.extra_size > 0; }
    uint32_t mark () const { return header.mark; }
    void mark (uint32_t m) { header.mark = m; }
    const Lit& last () const { return data[header.size-1].lit; }

    bool reloced () const { return header.reloced; }
    CRef relocation () const { return data[0].rel; }
    void relocate (CRef c) { header.reloced = 1; data[0].rel = c; }



    Lit& operator [] (int i) { return data[i].lit; }
    Lit operator [] (int i) const { return data[i].lit; }
    operator const Lit* (void) const { return (Lit*)data; }

    float& activity () { assert(header.extra_size > 0); return data[header.size].act; }
    uint32_t abstraction () const { assert(header.extra_size > 0); return data[header.size].abs; }


    bool wasImported() const {return header.extra_size > 1;}
    uint32_t importedFrom () const { assert(header.extra_size > 1); return data[header.size + 1].abs;}
    void setImportedFrom(uint32_t ifrom) {assert(header.extra_size > 1); data[header.size+1].abs = ifrom;}

    Lit subsumes (const Clause& other) const;
    void strengthen (Lit p);
    void setLBD(int i) {if (i < (1<<(13 -1))) header.lbd = i; else header.lbd = (1<<(13 -1));}

    unsigned int lbd () const { return header.lbd; }
    void setCanBeDel(bool b) {header.canbedel = b;}
    bool canBeDel() {return header.canbedel;}
    void setSeen(bool b) {header.seen = b;}
    bool getSeen() {return header.seen;}
    void setExported(unsigned int b) {header.exported = b;}
    unsigned int getExported() {return header.exported;}
    void setOneWatched(bool b) {header.oneWatched = b;}
    bool getOneWatched() {return header.oneWatched;}
    void setSizeWithoutSelectors (unsigned int n) {header.szWithoutSelectors = n; }
    unsigned int sizeWithoutSelectors () const { return header.szWithoutSelectors; }

};






const CRef CRef_Undef = RegionAllocator<uint32_t>::Ref_Undef;
class ClauseAllocator : public RegionAllocator<uint32_t>
{
    static int clauseWord32Size(int size, int extra_size){
        return (sizeof(Clause) + (sizeof(Lit) * (size + extra_size))) / sizeof(uint32_t); }
 public:
    bool extra_clause_field;

    ClauseAllocator(uint32_t start_cap) : RegionAllocator<uint32_t>(start_cap), extra_clause_field(false){}
    ClauseAllocator() : extra_clause_field(false){}

    void moveTo(ClauseAllocator& to){
        to.extra_clause_field = extra_clause_field;
        RegionAllocator<uint32_t>::moveTo(to); }

    template<class Lits>
    CRef alloc(const Lits& ps, bool learnt = false, bool imported = false)
    {
        assert(sizeof(Lit) == sizeof(uint32_t));
        assert(sizeof(float) == sizeof(uint32_t));

        bool use_extra = learnt | extra_clause_field;
        int extra_size = imported?3:(use_extra?1:0);
        CRef cid = RegionAllocator<uint32_t>::alloc(clauseWord32Size(ps.size(), extra_size));
        new (lea(cid)) Clause(ps, extra_size, learnt);

        return cid;
    }


    Clause& operator[](Ref r) { return (Clause&)RegionAllocator<uint32_t>::operator[](r); }
    const Clause& operator[](Ref r) const { return (Clause&)RegionAllocator<uint32_t>::operator[](r); }
    Clause* lea (Ref r) { return (Clause*)RegionAllocator<uint32_t>::lea(r); }
    const Clause* lea (Ref r) const { return (Clause*)RegionAllocator<uint32_t>::lea(r); }
    Ref ael (const Clause* t){ return RegionAllocator<uint32_t>::ael((uint32_t*)t); }

    void free(CRef cid)
    {
        Clause& c = operator[](cid);
        RegionAllocator<uint32_t>::free(clauseWord32Size(c.size(), c.has_extra()));
    }

    void reloc(CRef& cr, ClauseAllocator& to)
    {
        Clause& c = operator[](cr);

        if (c.reloced()) { cr = c.relocation(); return; }

        cr = to.alloc(c, c.learnt(), c.wasImported());
        c.relocate(cr);



        to[cr].mark(c.mark());
        if (to[cr].learnt()) {
   to[cr].activity() = c.activity();
   to[cr].setLBD(c.lbd());
   to[cr].setExported(c.getExported());
   to[cr].setOneWatched(c.getOneWatched());
   to[cr].setSeen(c.getSeen());
   to[cr].setSizeWithoutSelectors(c.sizeWithoutSelectors());
   to[cr].setCanBeDel(c.canBeDel());
   if (c.wasImported()) {
             to[cr].setImportedFrom(c.importedFrom());
   }
 }
        else if (to[cr].has_extra()) to[cr].calcAbstraction();
    }
};





template<class Idx, class Vec, class Deleted>
class OccLists
{
    vec<Vec> occs;
    vec<char> dirty;
    vec<Idx> dirties;
    Deleted deleted;

 public:
    OccLists(const Deleted& d) : deleted(d) {}

    void init (const Idx& idx){ occs.growTo(toInt(idx)+1); dirty.growTo(toInt(idx)+1, 0); }

    Vec& operator[](const Idx& idx){ return occs[toInt(idx)]; }
    Vec& lookup (const Idx& idx){ if (dirty[toInt(idx)]) clean(idx); return occs[toInt(idx)]; }

    void cleanAll ();
    void copyTo(OccLists &copy) const {

 copy.occs.growTo(occs.size());
 for(int i = 0;i<occs.size();i++)
     occs[i].memCopyTo(copy.occs[i]);
 dirty.memCopyTo(copy.dirty);
 dirties.memCopyTo(copy.dirties);
    }

    void clean (const Idx& idx);
    void smudge (const Idx& idx){
        if (dirty[toInt(idx)] == 0){
            dirty[toInt(idx)] = 1;
            dirties.push(idx);
        }
    }

    void clear(bool free = true){
        occs .clear(free);
        dirty .clear(free);
        dirties.clear(free);
    }
};


template<class Idx, class Vec, class Deleted>
void OccLists<Idx,Vec,Deleted>::cleanAll()
{
    for (int i = 0; i < dirties.size(); i++)

        if (dirty[toInt(dirties[i])])
            clean(dirties[i]);
    dirties.clear();
}


template<class Idx, class Vec, class Deleted>
void OccLists<Idx,Vec,Deleted>::clean(const Idx& idx)
{
    Vec& vec = occs[toInt(idx)];
    int i, j;
    for (i = j = 0; i < vec.size(); i++)
        if (!deleted(vec[i]))
            vec[j++] = vec[i];
    vec.shrink(i - j);
    dirty[toInt(idx)] = 0;
}






template<class T>
class CMap
{
    struct CRefHash {
        uint32_t operator()(CRef cr) const { return (uint32_t)cr; } };

    typedef Map<CRef, T, CRefHash> HashTable;
    HashTable map;

 public:

    void clear () { map.clear(); }
    int size () const { return map.elems(); }



    void insert (CRef cr, const T& t){ map.insert(cr, t); }
    void growTo (CRef cr, const T& t){ map.insert(cr, t); }
    void remove (CRef cr) { map.remove(cr); }
    bool has (CRef cr, T& t) { return map.peek(cr, t); }


    const T& operator [] (CRef cr) const { return map[cr]; }
    T& operator [] (CRef cr) { return map[cr]; }


    int bucket_count() const { return map.bucket_count(); }
    const vec<typename HashTable::Pair>& bucket(int i) const { return map.bucket(i); }


    void moveTo(CMap& other){ map.moveTo(other.map); }
};
inline Lit Clause::subsumes(const Clause& other) const
{


    assert(!header.learnt); assert(!other.header.learnt);
    assert(header.extra_size > 0); assert(other.header.extra_size > 0);
    if (other.header.size < header.size || (data[header.size].abs & ~other.data[other.header.size].abs) != 0)
        return lit_Error;

    Lit ret = lit_Undef;
    const Lit* c = (const Lit*)(*this);
    const Lit* d = (const Lit*)other;

    for (unsigned i = 0; i < header.size; i++) {

        for (unsigned j = 0; j < other.header.size; j++)
            if (c[i] == d[j])
                goto ok;
            else if (ret == lit_Undef && c[i] == ~d[j]){
                ret = c[i];
                goto ok;
            }


        return lit_Error;
    ok:;
    }

    return ret;
}

inline void Clause::strengthen(Lit p)
{
    remove(*this, p);
    calcAbstraction();
}


}


namespace Glucose {

template <class T>
class bqueue {
    vec<T> elems;
    int first;
 int last;
 unsigned long long sumofqueue;
 int maxsize;
 int queuesize;
 bool expComputed;
 double exp,value;
public:
 bqueue(void) : first(0), last(0), sumofqueue(0), maxsize(0), queuesize(0),expComputed(false) { }

 void initSize(int size) {growTo(size);exp = 2.0/(size+1);}

 void push(T x) {
   expComputed = false;
  if (queuesize==maxsize) {
   assert(last==first);
   sumofqueue -= elems[last];
   if ((++last) == maxsize) last = 0;
  } else
   queuesize++;
  sumofqueue += x;
  elems[first] = x;
  if ((++first) == maxsize) {first = 0;last = 0;}
 }

 T peek() { assert(queuesize>0); return elems[last]; }
 void pop() {sumofqueue-=elems[last]; queuesize--; if ((++last) == maxsize) last = 0;}

 unsigned long long getsum() const {return sumofqueue;}
 unsigned int getavg() const {return (unsigned int)(sumofqueue/((unsigned long long)queuesize));}
 int maxSize() const {return maxsize;}
 double getavgDouble() const {
   double tmp = 0;
   for(int i=0;i<elems.size();i++) {
     tmp+=elems[i];
   }
   return tmp/elems.size();
 }
 int isvalid() const {return (queuesize==maxsize);}

 void growTo(int size) {
  elems.growTo(size);
  first=0; maxsize=size; queuesize = 0;last = 0;
  for(int i=0;i<size;i++) elems[i]=0;
 }

 double getAvgExp() {
   if(expComputed) return value;
   double a=exp;
   value = elems[first];
   for(int i = first;i<maxsize;i++) {
     value+=a*((double)elems[i]);
     a=a*exp;
   }
   for(int i = 0;i<last;i++) {
     value+=a*((double)elems[i]);
     a=a*exp;
   }
   value = value*(1-exp)/(1-a);
   expComputed = true;
   return value;


 }
 void fastclear() {first = 0; last = 0; queuesize=0; sumofqueue=0;}

    int size(void) { return queuesize; }

    void clear(bool dealloc = false) { elems.clear(dealloc); first = 0; maxsize=0; queuesize=0;sumofqueue=0;}

    void copyTo(bqueue &dest) const {
        dest.last = last;
        dest.sumofqueue = sumofqueue;
        dest.maxsize = maxsize;
        dest.queuesize = queuesize;
        dest.expComputed = expComputed;
        dest.exp = exp;
        dest.value = value;
        dest.first = first;
        elems.copyTo(dest.elems);
    }
};
}
namespace Glucose {




class Solver : public Clone {

    friend class SolverConfiguration;

public:



    Solver();
    Solver(const Solver &s);

    virtual ~Solver();




    virtual Clone* clone() const {
        return new Solver(*this);
    }



    virtual Var newVar (bool polarity = true, bool dvar = true);
    bool addClause (const vec<Lit>& ps);
    bool addEmptyClause();
    bool addClause (Lit p);
    bool addClause (Lit p, Lit q);
    bool addClause (Lit p, Lit q, Lit r);
    virtual bool addClause_( vec<Lit>& ps);




    bool simplify ();
    bool solve (const vec<Lit>& assumps);
    lbool solveLimited (const vec<Lit>& assumps);
    bool solve ();
    bool solve (Lit p);
    bool solve (Lit p, Lit q);
    bool solve (Lit p, Lit q, Lit r);
    bool okay () const;


    void toDimacs (FILE* f, const vec<Lit>& assumps);
    void toDimacs (const char *file, const vec<Lit>& assumps);
    void toDimacs (FILE* f, Clause& c, vec<Var>& map, Var& max);
    void toDimacs (const char* file);
    void toDimacs (const char* file, Lit p);
    void toDimacs (const char* file, Lit p, Lit q);
    void toDimacs (const char* file, Lit p, Lit q, Lit r);


    void printLit(Lit l);
    void printClause(CRef c);
    void printInitialClause(CRef c);



    void setPolarity (Var v, bool b);
    void setDecisionVar (Var v, bool b);



    lbool value (Var x) const;
    lbool value (Lit p) const;
    lbool modelValue (Var x) const;
    lbool modelValue (Lit p) const;
    int nAssigns () const;
    int nClauses () const;
    int nLearnts () const;
    int nVars () const;
    int nFreeVars () const;

    inline char valuePhase(Var v) {return polarity[v];}


    void setIncrementalMode();
    void initNbInitialVars(int nb);
    void printIncrementalStats();
    bool isIncremental();


    void setConfBudget(int64_t x);
    void setPropBudget(int64_t x);
    void budgetOff();
    void interrupt();
    void clearInterrupt();



    virtual void garbageCollect();
    void checkGarbage(double gf);
    void checkGarbage();



    vec<lbool> model;
    vec<Lit> conflict;




    int verbosity;
    int verbEveryConflicts;
    int showModel;


    double K;
    double R;
    double sizeLBDQueue;
    double sizeTrailQueue;


    int firstReduceDB;
    int incReduceDB;
    int specialIncReduceDB;
    unsigned int lbLBDFrozenClause;


    int lbSizeMinimizingClause;
    unsigned int lbLBDMinimizingClause;


    double var_decay;
    double max_var_decay;
    double clause_decay;
    double random_var_freq;
    double random_seed;
    int ccmin_mode;
    int phase_saving;
    bool rnd_pol;
    bool rnd_init_act;


    double garbage_frac;


    FILE* certifiedOutput;
    bool certifiedUNSAT;



    uint32_t panicModeLastRemoved, panicModeLastRemovedShared;

    bool useUnaryWatched;
    bool promoteOneWatchedClause;




    virtual void parallelImportClauseDuringConflictAnalysis(Clause &c,CRef confl);
    virtual bool parallelImportClauses();
    virtual void parallelImportUnaryClauses();
    virtual void parallelExportUnaryClause(Lit p);
    virtual void parallelExportClauseDuringSearch(Clause &c);
    virtual bool parallelJobIsFinished();
    virtual bool panicModeIsEnabled();




    uint64_t nbPromoted;
    uint64_t originalClausesSeen;
    uint64_t sumDecisionLevels;

    uint64_t nbRemovedClauses,nbRemovedUnaryWatchedClauses, nbReducedClauses,nbDL2,nbBin,nbUn,nbReduceDB,solves, starts, decisions, rnd_decisions, propagations, conflicts,conflictsRestarts,nbstopsrestarts,nbstopsrestartssame,lastblockatrestart;
    uint64_t dec_vars, clauses_literals, learnts_literals, max_literals, tot_literals;

public:

    long curRestart;


    struct VarData { CRef reason; int level; };
    static inline VarData mkVarData(CRef cr, int l){ VarData d = {cr, l}; return d; }

    struct Watcher {
        CRef cref;
        Lit blocker;
        Watcher(CRef cr, Lit p) : cref(cr), blocker(p) {}
        bool operator==(const Watcher& w) const { return cref == w.cref; }
        bool operator!=(const Watcher& w) const { return cref != w.cref; }






    };

    struct WatcherDeleted
    {
        const ClauseAllocator& ca;
        WatcherDeleted(const ClauseAllocator& _ca) : ca(_ca) {}
        bool operator()(const Watcher& w) const { return ca[w.cref].mark() == 1; }
    };

    struct VarOrderLt {
        const vec<double>& activity;
        bool operator () (Var x, Var y) const { return activity[x] > activity[y]; }
        VarOrderLt(const vec<double>& act) : activity(act) { }
    };




    int lastIndexRed;
    bool ok;
    double cla_inc;
    vec<double> activity;
    double var_inc;
    OccLists<Lit, vec<Watcher>, WatcherDeleted>
                        watches;
    OccLists<Lit, vec<Watcher>, WatcherDeleted>
                        watchesBin;
    OccLists<Lit, vec<Watcher>, WatcherDeleted>
                        unaryWatches;
    vec<CRef> clauses;
    vec<CRef> learnts;
    vec<CRef> unaryWatchedClauses;

    vec<lbool> assigns;
    vec<char> polarity;
    vec<char> decision;
    vec<Lit> trail;
    vec<int> nbpos;
    vec<int> trail_lim;
    vec<VarData> vardata;
    int qhead;
    int simpDB_assigns;
    int64_t simpDB_props;
    vec<Lit> assumptions;
    Heap<VarOrderLt> order_heap;
    double progress_estimate;
    bool remove_satisfied;
    bool reduceOnSize;
    int reduceOnSizeSize;
    vec<unsigned int> permDiff;



    vec<Lit> lastDecisionLevel;

    ClauseAllocator ca;

    int nbclausesbeforereduce;


    bqueue<unsigned int> trailQueue,lbdQueue;
    float sumLBD;
    int sumAssumptions;
    CRef lastLearntClause;





    vec<char> seen;
    vec<Lit> analyze_stack;
    vec<Lit> analyze_toclear;
    vec<Lit> add_tmp;
    unsigned int MYFLAG;


    double max_learnts;
    double learntsize_adjust_confl;
    int learntsize_adjust_cnt;



    int64_t conflict_budget;
    int64_t propagation_budget;
    bool asynch_interrupt;


    int incremental;
    int nbVarsInitialFormula;
    double totalTime4Sat,totalTime4Unsat;
    int nbSatCalls,nbUnsatCalls;
    vec<int> assumptionPositions,initialPositions;




    void insertVarOrder (Var x);
    Lit pickBranchLit ();
    void newDecisionLevel ();
    void uncheckedEnqueue (Lit p, CRef from = CRef_Undef);
    bool enqueue (Lit p, CRef from = CRef_Undef);
    CRef propagate ();
    CRef propagateUnaryWatches(Lit p);
    void cancelUntil (int level);
    void analyze (CRef confl, vec<Lit>& out_learnt, vec<Lit> & selectors, int& out_btlevel,unsigned int &nblevels,unsigned int &szWithoutSelectors);
    void analyzeFinal (Lit p, vec<Lit>& out_conflict);
    bool litRedundant (Lit p, uint32_t abstract_levels);
    lbool search (int nof_conflicts);
    virtual lbool solve_ (bool do_simp = true, bool turn_off_simp = false);
    virtual void reduceDB ();
    void removeSatisfied (vec<CRef>& cs);
    void rebuildOrderHeap ();



    void varDecayActivity ();
    void varBumpActivity (Var v, double inc);
    void varBumpActivity (Var v);
    void claDecayActivity ();
    void claBumpActivity (Clause& c);



    void attachClause (CRef cr);
    void detachClause (CRef cr, bool strict = false);
    void detachClausePurgatory(CRef cr, bool strict = false);
    void attachClausePurgatory(CRef cr);
    void removeClause (CRef cr, bool inPurgatory = false);
    bool locked (const Clause& c) const;
    bool satisfied (const Clause& c) const;

    unsigned int computeLBD(const vec<Lit> & lits,int end=-1);
    unsigned int computeLBD(const Clause &c);
    void minimisationWithBinaryResolution(vec<Lit> &out_learnt);

    virtual void relocAll (ClauseAllocator& to);



    int decisionLevel () const;
    uint32_t abstractLevel (Var x) const;
    virtual inline bool moreReason(Lit, vec<Lit>&, vec<Lit>&, int&) { return false; }
    virtual inline bool moreReason(Lit) { return true; }
    virtual inline bool moreConflict(vec<Lit>&, vec<Lit>&, int&) { return false; }
    CRef reason (Var x) const;
    int level (Var x) const;
    double progressEstimate () const;
    bool withinBudget () const;
    inline bool isSelector(Var v) {return (incremental && v>nbVarsInitialFormula);}

public:




    static inline double drand(double& seed) {
        seed *= 1389796;
        int q = (int)(seed / 2147483647);
        seed -= (double)q * 2147483647;
        return seed / 2147483647; }


    static inline int irand(double& seed, int size) {
        return (int)(drand(seed) * size); }
};





inline CRef Solver::reason(Var x) const { return vardata[x].reason; }
inline int Solver::level (Var x) const { return vardata[x].level; }

inline void Solver::insertVarOrder(Var x) {
    if (!order_heap.inHeap(x) && decision[x]) order_heap.insert(x); }

inline void Solver::varDecayActivity() { var_inc *= (1 / var_decay); }
inline void Solver::varBumpActivity(Var v) { varBumpActivity(v, var_inc); }
inline void Solver::varBumpActivity(Var v, double inc) {
    if ( (activity[v] += inc) > 1e100 ) {

        for (int i = 0; i < nVars(); i++)
            activity[i] *= 1e-100;
        var_inc *= 1e-100; }


    if (order_heap.inHeap(v))
        order_heap.decrease(v); }

inline void Solver::claDecayActivity() { cla_inc *= (1 / clause_decay); }
inline void Solver::claBumpActivity (Clause& c) {
        if ( (c.activity() += cla_inc) > 1e20 ) {

            for (int i = 0; i < learnts.size(); i++)
                ca[learnts[i]].activity() *= 1e-20;
            cla_inc *= 1e-20; } }

inline void Solver::checkGarbage(void){ return checkGarbage(garbage_frac); }
inline void Solver::checkGarbage(double gf){
    if (ca.wasted() > ca.size() * gf)
        garbageCollect(); }


inline bool Solver::enqueue (Lit p, CRef from) { return value(p) != (Glucose::lbool((uint8_t)2)) ? value(p) != (Glucose::lbool((uint8_t)1)) : (uncheckedEnqueue(p, from), true); }
inline bool Solver::addClause (const vec<Lit>& ps) { ps.copyTo(add_tmp); return addClause_(add_tmp); }
inline bool Solver::addEmptyClause () { add_tmp.clear(); return addClause_(add_tmp); }
inline bool Solver::addClause (Lit p) { add_tmp.clear(); add_tmp.push(p); return addClause_(add_tmp); }
inline bool Solver::addClause (Lit p, Lit q) { add_tmp.clear(); add_tmp.push(p); add_tmp.push(q); return addClause_(add_tmp); }
inline bool Solver::addClause (Lit p, Lit q, Lit r) { add_tmp.clear(); add_tmp.push(p); add_tmp.push(q); add_tmp.push(r); return addClause_(add_tmp); }
 inline bool Solver::locked (const Clause& c) const {
   if(c.size()>2)
     return value(c[0]) == (Glucose::lbool((uint8_t)0)) && reason(var(c[0])) != CRef_Undef && ca.lea(reason(var(c[0]))) == &c;
   return
     (value(c[0]) == (Glucose::lbool((uint8_t)0)) && reason(var(c[0])) != CRef_Undef && ca.lea(reason(var(c[0]))) == &c)
     ||
     (value(c[1]) == (Glucose::lbool((uint8_t)0)) && reason(var(c[1])) != CRef_Undef && ca.lea(reason(var(c[1]))) == &c);
 }
inline void Solver::newDecisionLevel() { trail_lim.push(trail.size()); }

inline int Solver::decisionLevel () const { return trail_lim.size(); }
inline uint32_t Solver::abstractLevel (Var x) const { return 1 << (level(x) & 31); }
inline lbool Solver::value (Var x) const { return assigns[x]; }
inline lbool Solver::value (Lit p) const { return assigns[var(p)] ^ sign(p); }
inline lbool Solver::modelValue (Var x) const { return model[x]; }
inline lbool Solver::modelValue (Lit p) const { return model[var(p)] ^ sign(p); }
inline int Solver::nAssigns () const { return trail.size(); }
inline int Solver::nClauses () const { return clauses.size(); }
inline int Solver::nLearnts () const { return learnts.size(); }
inline int Solver::nVars () const { return vardata.size(); }
inline int Solver::nFreeVars () const { return (int)dec_vars - (trail_lim.size() == 0 ? trail.size() : trail_lim[0]); }
inline void Solver::setPolarity (Var v, bool b) { polarity[v] = b; }
inline void Solver::setDecisionVar(Var v, bool b)
{
    if ( b && !decision[v]) dec_vars++;
    else if (!b && decision[v]) dec_vars--;

    decision[v] = b;
    insertVarOrder(v);
}
inline void Solver::setConfBudget(int64_t x){ conflict_budget = conflicts + x; }
inline void Solver::setPropBudget(int64_t x){ propagation_budget = propagations + x; }
inline void Solver::interrupt(){ asynch_interrupt = true; }
inline void Solver::clearInterrupt(){ asynch_interrupt = false; }
inline void Solver::budgetOff(){ conflict_budget = propagation_budget = -1; }
inline bool Solver::withinBudget() const {
    return !asynch_interrupt &&
           (conflict_budget < 0 || conflicts < (uint64_t)conflict_budget) &&
           (propagation_budget < 0 || propagations < (uint64_t)propagation_budget); }




inline bool Solver::solve () { budgetOff(); assumptions.clear(); return solve_() == (Glucose::lbool((uint8_t)0)); }
inline bool Solver::solve (Lit p) { budgetOff(); assumptions.clear(); assumptions.push(p); return solve_() == (Glucose::lbool((uint8_t)0)); }
inline bool Solver::solve (Lit p, Lit q) { budgetOff(); assumptions.clear(); assumptions.push(p); assumptions.push(q); return solve_() == (Glucose::lbool((uint8_t)0)); }
inline bool Solver::solve (Lit p, Lit q, Lit r) { budgetOff(); assumptions.clear(); assumptions.push(p); assumptions.push(q); assumptions.push(r); return solve_() == (Glucose::lbool((uint8_t)0)); }
inline bool Solver::solve (const vec<Lit>& assumps){ budgetOff(); assumps.copyTo(assumptions); return solve_() == (Glucose::lbool((uint8_t)0)); }
inline lbool Solver::solveLimited (const vec<Lit>& assumps){ assumps.copyTo(assumptions); return solve_(); }
inline bool Solver::okay () const { return ok; }

inline void Solver::toDimacs (const char* file){ vec<Lit> as; toDimacs(file, as); }
inline void Solver::toDimacs (const char* file, Lit p){ vec<Lit> as; as.push(p); toDimacs(file, as); }
inline void Solver::toDimacs (const char* file, Lit p, Lit q){ vec<Lit> as; as.push(p); as.push(q); toDimacs(file, as); }
inline void Solver::toDimacs (const char* file, Lit p, Lit q, Lit r){ vec<Lit> as; as.push(p); as.push(q); as.push(r); toDimacs(file, as); }







inline void Solver::printLit(Lit l)
{
    printf("%s%d:%c", sign(l) ? "-" : "", var(l)+1, value(l) == (Glucose::lbool((uint8_t)0)) ? '1' : (value(l) == (Glucose::lbool((uint8_t)1)) ? '0' : 'X'));
}


inline void Solver::printClause(CRef cr)
{
  Clause &c = ca[cr];
    for (int i = 0; i < c.size(); i++){
        printLit(c[i]);
        printf(" ");
    }
}

inline void Solver::printInitialClause(CRef cr)
{
  Clause &c = ca[cr];
    for (int i = 0; i < c.size(); i++){
      if(!isSelector(var(c[i]))) {
 printLit(c[i]);
        printf(" ");
      }
    }
}



struct reduceDB_lt {
    ClauseAllocator& ca;

    reduceDB_lt(ClauseAllocator& ca_) : ca(ca_) {
    }

    bool operator()(CRef x, CRef y) {


        if (ca[x].size() > 2 && ca[y].size() == 2) return 1;

        if (ca[y].size() > 2 && ca[x].size() == 2) return 0;
        if (ca[x].size() == 2 && ca[y].size() == 2) return 0;


        if (ca[x].lbd() > ca[y].lbd()) return 1;
        if (ca[x].lbd() < ca[y].lbd()) return 0;



        return ca[x].activity() < ca[y].activity();



    }
};


}

using namespace Glucose;


namespace Glucose {




static const int buffer_size = 1048576;


class StreamBuffer {
    gzFile in;
    unsigned char buf[buffer_size];
    int pos;
    int size;

    void assureLookahead() {
        if (pos >= size) {
            pos = 0;
            size = gzread(in, buf, sizeof(buf)); } }

public:
    explicit StreamBuffer(gzFile i) : in(i), pos(0), size(0) { assureLookahead(); }

    int operator * () const { return (pos >= size) ? EOF : buf[pos]; }
    void operator ++ () { pos++; assureLookahead(); }
    int position () const { return pos; }
};





  static inline bool isEof(StreamBuffer& in) { return *in == EOF; }
static inline bool isEof(const char* in) { return *in == '\0'; }





template<class B>
static void skipWhitespace(B& in) {
    while ((*in >= 9 && *in <= 13) || *in == 32)
        ++in; }


template<class B>
static void skipLine(B& in) {
    for (;;){
        if (isEof(in)) return;
        if (*in == '\n') { ++in; return; }
        ++in; } }

template<class B>
static double parseDouble(B& in) {
    bool neg= false;
 double accu = 0.0;
 double currentExponent = 1;
 int exponent;

    skipWhitespace(in);
    if(*in == EOF) return 0;
    if (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '1' || *in > '9') printf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
 accu = (double)(*in - '0');
 ++in;
 if (*in != '.') printf("PARSE ERROR! Unexpected char: %c\n", *in),exit(3);
 ++in;
 currentExponent = 0.1;
  while (*in >= '0' && *in <= '9') {
    accu = accu + currentExponent * ((double)(*in - '0')),
          currentExponent /= 10,
          ++in;
  }
 if (*in != 'e') printf("PARSE ERROR! Unexpected char: %c\n", *in),exit(3);
 ++in;
 exponent = parseInt(in);
 accu *= pow(10,exponent);
 return neg ? -accu:accu;
}


template<class B>
static int parseInt(B& in) {
    int val = 0;
    bool neg = false;
    skipWhitespace(in);
    if (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') fprintf(stderr, "PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
        val = val*10 + (*in - '0'),
        ++in;
    return neg ? -val : val; }




template<class B>
static bool match(B& in, const char* str) {
    int i;
    for (i = 0; str[i] != '\0'; i++)
        if (in[i] != str[i])
            return false;

    in += i;

    return true;
}


template<class B>
static bool eagerMatch(B& in, const char* str) {
    for (; *str != '\0'; ++str, ++in)
        if (*str != *in)
            return false;
    return true; }




}


namespace Glucose {





extern void parseOptions (int& argc, char** argv, bool strict = false);
extern void printUsageAndExit(int argc, char** argv, bool verbose = false);
extern void setUsageHelp (const char* str);
extern void setHelpPrefixStr (const char* str);






class Option
{
 public:
    const char* name;
    const char* description;
    const char* category;
    const char* type_name;

    static vec<Option*>& getOptionList () { static vec<Option*> options; return options; }
    static const char*& getUsageString() { static const char* usage_str; return usage_str; }
    static const char*& getHelpPrefixString() { static const char* help_prefix_str = ""; return help_prefix_str; }

    struct OptionLt {
        bool operator()(const Option* x, const Option* y) {
            int test1 = strcmp(x->category, y->category);
            return test1 < 0 || (test1 == 0 && strcmp(x->type_name, y->type_name) < 0);
        }
    };

    Option(const char* name_,
           const char* desc_,
           const char* cate_,
           const char* type_) :
      name (name_)
    , description(desc_)
    , category (cate_)
    , type_name (type_)
    {
        getOptionList().push(this);
    }

 public:
    virtual ~Option() {}

    virtual bool parse (const char* str) = 0;
    virtual void help (bool verbose = false) = 0;

    friend void parseOptions (int& argc, char** argv, bool strict);
    friend void printUsageAndExit (int argc, char** argv, bool verbose);
    friend void setUsageHelp (const char* str);
    friend void setHelpPrefixStr (const char* str);
};






struct IntRange {
    int begin;
    int end;
    IntRange(int b, int e) : begin(b), end(e) {}
};

struct Int64Range {
    int64_t begin;
    int64_t end;
    Int64Range(int64_t b, int64_t e) : begin(b), end(e) {}
};

struct DoubleRange {
    double begin;
    double end;
    bool begin_inclusive;
    bool end_inclusive;
    DoubleRange(double b, bool binc, double e, bool einc) : begin(b), end(e), begin_inclusive(binc), end_inclusive(einc) {}
};






class DoubleOption : public Option
{
 public:
    DoubleRange range;
    double value;

 public:
    DoubleOption(const char* c, const char* n, const char* d, double def = double(), DoubleRange r = DoubleRange(-HUGE_VAL, false, HUGE_VAL, false))
        : Option(n, d, c, "<double>"), range(r), value(def) {

    }

    operator double (void) const { return value; }
    operator double& (void) { return value; }
    DoubleOption& operator=(double x) { value = x; return *this; }

    virtual bool parse(const char* str){
        const char* span = str;

        if (!match(span, "-")) return false;
        match(span, "-");
        if (!match(span, name) || !match(span, "="))
            return false;

        char* end;
        double tmp = strtod(span, &end);

        if (end == NULL)
            return false;
        else if (tmp >= range.end && (!range.end_inclusive || tmp != range.end)){
            fprintf(stderr, "ERROR! value <%s> is too large for option \"%s\".\n", span, name);
            exit(1);
        }else if (tmp <= range.begin && (!range.begin_inclusive || tmp != range.begin)){
            fprintf(stderr, "ERROR! value <%s> is too small for option \"%s\".\n", span, name);
            exit(1); }

        value = tmp;


        return true;
    }

    virtual void help (bool verbose = false){
        fprintf(stderr, "  -%-12s = %-8s %c%4.2g .. %4.2g%c (default: %g)\n",
                name, type_name,
                range.begin_inclusive ? '[' : '(',
                range.begin,
                range.end,
                range.end_inclusive ? ']' : ')',
                value);
        if (verbose){
            fprintf(stderr, "\n        %s\n", description);
            fprintf(stderr, "\n");
        }
    }
};






class IntOption : public Option
{
 public:
    IntRange range;
    int32_t value;

 public:
    IntOption(const char* c, const char* n, const char* d, int32_t def = int32_t(), IntRange r = IntRange(INT32_MIN, INT32_MAX))
        : Option(n, d, c, "<int32>"), range(r), value(def) {}

    operator int32_t (void) const { return value; }
    operator int32_t& (void) { return value; }
    IntOption& operator= (int32_t x) { value = x; return *this; }

    virtual bool parse(const char* str){
        const char* span = str;

        if (!match(span, "-")) return false;
        match(span, "-");
        if (!match(span, name) || !match(span, "="))
            return false;

        char* end;
        int32_t tmp = strtol(span, &end, 10);

        if (end == NULL)
            return false;
        else if (tmp > range.end){
            fprintf(stderr, "ERROR! value <%s> is too large for option \"%s\".\n", span, name);
            exit(1);
        }else if (tmp < range.begin){
            fprintf(stderr, "ERROR! value <%s> is too small for option \"%s\".\n", span, name);
            exit(1); }

        value = tmp;

        return true;
    }

    virtual void help (bool verbose = false){
        fprintf(stderr, "  -%-12s = %-8s [", name, type_name);
        if (range.begin == INT32_MIN)
            fprintf(stderr, "imin");
        else
            fprintf(stderr, "%4d", range.begin);

        fprintf(stderr, " .. ");
        if (range.end == INT32_MAX)
            fprintf(stderr, "imax");
        else
            fprintf(stderr, "%4d", range.end);

        fprintf(stderr, "] (default: %d)\n", value);
        if (verbose){
            fprintf(stderr, "\n        %s\n", description);
            fprintf(stderr, "\n");
        }
    }
};





class Int64Option : public Option
{
 public:
    Int64Range range;
    int64_t value;

 public:
    Int64Option(const char* c, const char* n, const char* d, int64_t def = int64_t(), Int64Range r = Int64Range(INT64_MIN, INT64_MAX))
        : Option(n, d, c, "<int64>"), range(r), value(def) {}

    operator int64_t (void) const { return value; }
    operator int64_t& (void) { return value; }
    Int64Option& operator= (int64_t x) { value = x; return *this; }

    virtual bool parse(const char* str){
        const char* span = str;

        if (!match(span, "-")) return false;
        match(span, "-");
        if (!match(span, name) || !match(span, "="))
            return false;

        char* end;
        int64_t tmp = strtoll(span, &end, 10);

        if (end == NULL)
            return false;
        else if (tmp > range.end){
            fprintf(stderr, "ERROR! value <%s> is too large for option \"%s\".\n", span, name);
            exit(1);
        }else if (tmp < range.begin){
            fprintf(stderr, "ERROR! value <%s> is too small for option \"%s\".\n", span, name);
            exit(1); }

        value = tmp;

        return true;
    }

    virtual void help (bool verbose = false){
        fprintf(stderr, "  -%-12s = %-8s [", name, type_name);
        if (range.begin == INT64_MIN)
            fprintf(stderr, "imin");
        else
            fprintf(stderr, "%4" "ld", range.begin);

        fprintf(stderr, " .. ");
        if (range.end == INT64_MAX)
            fprintf(stderr, "imax");
        else
            fprintf(stderr, "%4" "ld", range.end);

        fprintf(stderr, "] (default: %" "ld"")\n", value);
        if (verbose){
            fprintf(stderr, "\n        %s\n", description);
            fprintf(stderr, "\n");
        }
    }
};






class StringOption : public Option
{
    const char* value;
 public:
    StringOption(const char* c, const char* n, const char* d, const char* def = NULL)
        : Option(n, d, c, "<string>"), value(def) {}

    operator const char* (void) const { return value; }
    operator const char*& (void) { return value; }
    StringOption& operator= (const char* x) { value = x; return *this; }

    virtual bool parse(const char* str){
        const char* span = str;

        if (!match(span, "-")) return false;
        match(span, "-");
        if (!match(span, name) || !match(span, "="))
            return false;

        value = span;
        return true;
    }

    virtual void help (bool verbose = false){
        fprintf(stderr, "  -%-10s = %8s\n", name, type_name);
        if (verbose){
            fprintf(stderr, "\n        %s\n", description);
            fprintf(stderr, "\n");
        }
    }
};





class EnumOption : public Option
{
    int value;
    std::vector<std::string> enumSet;
 public:
    EnumOption(const char* c, const char* n, const char* d, const char* enumSet_, int def = 0)
        : Option(n, d, c, "<enum>"), value(def) {
        for(int i = 0, from = 0;; i++) {
            if(enumSet_[i] == '|' || enumSet_[i] == '\0') {
                enumSet.push_back(std::string(enumSet_, from, i-from));
                from = i+1;
            }
            if(enumSet_[i] == '\0') break;
        }
        assert(static_cast<unsigned>(value) < enumSet.size());
    }

    operator const char* (void) const { return enumSet[value].c_str(); }

    EnumOption& operator=(const char* x) {
        for(unsigned i = 0; i < enumSet.size(); i++)
            if(enumSet[i] == x) { value = i; break; }
        return *this;
        }

    virtual bool parse(const char* str){
        const char* span = str;

        if (!match(span, "-")) return false;
        match(span, "-");
        if (!match(span, name) || !match(span, "="))
            return false;

        value = -1;
        *this = span;
        return value != -1;
    }

    virtual void help (bool verbose = false){
        fprintf(stderr, "  -%-10s = %8s {", name, type_name);
        for(unsigned i = 0; i < enumSet.size(); i++) {
            if(i != 0) fprintf(stderr, ", ");
            fprintf(stderr, "%s", enumSet[i].c_str());
        }
        fprintf(stderr, "} (default: %s)\n", enumSet[value].c_str());

        if (verbose){
            fprintf(stderr, "\n        %s\n", description);
            fprintf(stderr, "\n");
        }
    }
};





class BoolOption : public Option
{
    bool value;

 public:
    BoolOption(const char* c, const char* n, const char* d, bool v)
        : Option(n, d, c, "<bool>"), value(v) {}

    operator bool (void) const { return value; }
    operator bool& (void) { return value; }
    BoolOption& operator=(bool b) { value = b; return *this; }

    virtual bool parse(const char* str){
        const char* span = str;

        if (match(span, "-")){
            match(span, "-");
            bool b = !match(span, "no-");

            if (strcmp(span, name) == 0){
                value = b;
                return true; }
        }

        return false;
    }

    virtual void help (bool verbose = false){

        fprintf(stderr, "  -%s, -no-%s", name, name);

        fprintf(stderr, " ");
        fprintf(stderr, "(default: %s)\n", value ? "on" : "off");
        if (verbose){
            fprintf(stderr, "\n        %s\n", description);
            fprintf(stderr, "\n");
        }
    }
};


}

namespace Glucose {
  inline double cpuTime(void) { return satTimer.elapsed(); }
}

static const char* _cat = "CORE";
static const char* _cr = "CORE -- RESTART";
static const char* _cred = "CORE -- REDUCE";
static const char* _cm = "CORE -- MINIMIZE";




static DoubleOption opt_K(_cr, "K", "The constant used to force restart", 0.8, DoubleRange(0, false, 1, false));
static DoubleOption opt_R(_cr, "R", "The constant used to block restart", 1.4, DoubleRange(1, false, 5, false));
static IntOption opt_size_lbd_queue(_cr, "szLBDQueue", "The size of moving average for LBD (restarts)", 50, IntRange(10, INT32_MAX));
static IntOption opt_size_trail_queue(_cr, "szTrailQueue", "The size of moving average for trail (block restarts)", 5000, IntRange(10, INT32_MAX));

static IntOption opt_first_reduce_db(_cred, "firstReduceDB", "The number of conflicts before the first reduce DB", 2000, IntRange(0, INT32_MAX));
static IntOption opt_inc_reduce_db(_cred, "incReduceDB", "Increment for reduce DB", 300, IntRange(0, INT32_MAX));
static IntOption opt_spec_inc_reduce_db(_cred, "specialIncReduceDB", "Special increment for reduce DB", 1000, IntRange(0, INT32_MAX));
static IntOption opt_lb_lbd_frozen_clause(_cred, "minLBDFrozenClause", "Protect clauses if their LBD decrease and is lower than (for one turn)", 30, IntRange(0, INT32_MAX));

static IntOption opt_lb_size_minimzing_clause(_cm, "minSizeMinimizingClause", "The min size required to minimize clause", 30, IntRange(3, INT32_MAX));
static IntOption opt_lb_lbd_minimzing_clause(_cm, "minLBDMinimizingClause", "The min LBD required to minimize clause", 6, IntRange(3, INT32_MAX));


static DoubleOption opt_var_decay(_cat, "var-decay", "The variable activity decay factor (starting point)", 0.8, DoubleRange(0, false, 1, false));
static DoubleOption opt_max_var_decay(_cat, "max-var-decay", "The variable activity decay factor", 0.95, DoubleRange(0, false, 1, false));
static DoubleOption opt_clause_decay(_cat, "cla-decay", "The clause activity decay factor", 0.999, DoubleRange(0, false, 1, false));
static DoubleOption opt_random_var_freq(_cat, "rnd-freq", "The frequency with which the decision heuristic tries to choose a random variable", 0, DoubleRange(0, true, 1, true));
static DoubleOption opt_random_seed(_cat, "rnd-seed", "Used by the random variable selection", 91648253, DoubleRange(0, false, HUGE_VAL, false));
static IntOption opt_ccmin_mode(_cat, "ccmin-mode", "Controls conflict clause minimization (0=none, 1=basic, 2=deep)", 2, IntRange(0, 2));
static IntOption opt_phase_saving(_cat, "phase-saving", "Controls the level of phase saving (0=none, 1=limited, 2=full)", 2, IntRange(0, 2));
static BoolOption opt_rnd_init_act(_cat, "rnd-init", "Randomize the initial activity", false);
static DoubleOption opt_garbage_frac(_cat, "gc-frac", "The fraction of wasted memory allowed before a garbage collection is triggered", 0.20, DoubleRange(0, false, HUGE_VAL, false));





Solver::Solver() :



verbosity(0)
, showModel(0)
, K(opt_K)
, R(opt_R)
, sizeLBDQueue(opt_size_lbd_queue)
, sizeTrailQueue(opt_size_trail_queue)
, firstReduceDB(opt_first_reduce_db)
, incReduceDB(opt_inc_reduce_db)
, specialIncReduceDB(opt_spec_inc_reduce_db)
, lbLBDFrozenClause(opt_lb_lbd_frozen_clause)
, lbSizeMinimizingClause(opt_lb_size_minimzing_clause)
, lbLBDMinimizingClause(opt_lb_lbd_minimzing_clause)
, var_decay(opt_var_decay)
, max_var_decay(opt_max_var_decay)
, clause_decay(opt_clause_decay)
, random_var_freq(opt_random_var_freq)
, random_seed(opt_random_seed)
, ccmin_mode(opt_ccmin_mode)
, phase_saving(opt_phase_saving)
, rnd_pol(false)
, rnd_init_act(opt_rnd_init_act)
, garbage_frac(opt_garbage_frac)
, certifiedOutput(NULL)
, certifiedUNSAT(false)
, panicModeLastRemoved(0), panicModeLastRemovedShared(0)
, useUnaryWatched(false)
, promoteOneWatchedClause(true)


, nbPromoted(0)
, originalClausesSeen(0)
, sumDecisionLevels(0)
, nbRemovedClauses(0), nbRemovedUnaryWatchedClauses(0), nbReducedClauses(0), nbDL2(0), nbBin(0), nbUn(0), nbReduceDB(0)
, solves(0), starts(0), decisions(0), rnd_decisions(0), propagations(0), conflicts(0), conflictsRestarts(0)
, nbstopsrestarts(0), nbstopsrestartssame(0), lastblockatrestart(0)
, dec_vars(0), clauses_literals(0), learnts_literals(0), max_literals(0), tot_literals(0)
, curRestart(1)

, ok(true)
, cla_inc(1)
, var_inc(1)
, watches(WatcherDeleted(ca))
, watchesBin(WatcherDeleted(ca))
, unaryWatches(WatcherDeleted(ca))
, qhead(0)
, simpDB_assigns(-1)
, simpDB_props(0)
, order_heap(VarOrderLt(activity))
, progress_estimate(0)
, remove_satisfied(true)
, reduceOnSize(false)
, reduceOnSizeSize(12)
,lastLearntClause(CRef_Undef)


, conflict_budget(-1)
, propagation_budget(-1)
, asynch_interrupt(false)
, incremental(false)
, nbVarsInitialFormula(INT32_MAX)
, totalTime4Sat(0.)
, totalTime4Unsat(0.)
, nbSatCalls(0)
, nbUnsatCalls(0)
{
    MYFLAG = 0;


    lbdQueue.initSize(sizeLBDQueue);
    trailQueue.initSize(sizeTrailQueue);
    sumLBD = 0;
    nbclausesbeforereduce = firstReduceDB;
}





Solver::Solver(const Solver &s) :
  verbosity(s.verbosity)
, showModel(s.showModel)
, K(s.K)
, R(s.R)
, sizeLBDQueue(s.sizeLBDQueue)
, sizeTrailQueue(s.sizeTrailQueue)
, firstReduceDB(s.firstReduceDB)
, incReduceDB(s.incReduceDB)
, specialIncReduceDB(s.specialIncReduceDB)
, lbLBDFrozenClause(s.lbLBDFrozenClause)
, lbSizeMinimizingClause(s.lbSizeMinimizingClause)
, lbLBDMinimizingClause(s.lbLBDMinimizingClause)
, var_decay(s.var_decay)
, max_var_decay(s.max_var_decay)
, clause_decay(s.clause_decay)
, random_var_freq(s.random_var_freq)
, random_seed(s.random_seed)
, ccmin_mode(s.ccmin_mode)
, phase_saving(s.phase_saving)
, rnd_pol(s.rnd_pol)
, rnd_init_act(s.rnd_init_act)
, garbage_frac(s.garbage_frac)
, certifiedOutput(NULL)
, certifiedUNSAT(false)
, panicModeLastRemoved(s.panicModeLastRemoved), panicModeLastRemovedShared(s.panicModeLastRemovedShared)
, useUnaryWatched(s.useUnaryWatched)
, promoteOneWatchedClause(s.promoteOneWatchedClause)


, nbPromoted(s.nbPromoted)
, originalClausesSeen(s.originalClausesSeen)
, sumDecisionLevels(s.sumDecisionLevels)
, nbRemovedClauses(s.nbRemovedClauses), nbRemovedUnaryWatchedClauses(s.nbRemovedUnaryWatchedClauses)
, nbReducedClauses(s.nbReducedClauses), nbDL2(s.nbDL2), nbBin(s.nbBin), nbUn(s.nbUn), nbReduceDB(s.nbReduceDB)
, solves(s.solves), starts(s.starts), decisions(s.decisions), rnd_decisions(s.rnd_decisions)
, propagations(s.propagations), conflicts(s.conflicts), conflictsRestarts(s.conflictsRestarts)
, nbstopsrestarts(s.nbstopsrestarts), nbstopsrestartssame(s.nbstopsrestartssame)
, lastblockatrestart(s.lastblockatrestart)
, dec_vars(s.dec_vars), clauses_literals(s.clauses_literals)
, learnts_literals(s.learnts_literals), max_literals(s.max_literals), tot_literals(s.tot_literals)
, curRestart(s.curRestart)

, ok(true)
, cla_inc(s.cla_inc)
, var_inc(s.var_inc)
, watches(WatcherDeleted(ca))
, watchesBin(WatcherDeleted(ca))
, unaryWatches(WatcherDeleted(ca))
, qhead(s.qhead)
, simpDB_assigns(s.simpDB_assigns)
, simpDB_props(s.simpDB_props)
, order_heap(VarOrderLt(activity))
, progress_estimate(s.progress_estimate)
, remove_satisfied(s.remove_satisfied)
, reduceOnSize(s.reduceOnSize)
, reduceOnSizeSize(s.reduceOnSizeSize)
,lastLearntClause(CRef_Undef)


, conflict_budget(s.conflict_budget)
, propagation_budget(s.propagation_budget)
, asynch_interrupt(s.asynch_interrupt)
, incremental(s.incremental)
, nbVarsInitialFormula(s.nbVarsInitialFormula)
, totalTime4Sat(s.totalTime4Sat)
, totalTime4Unsat(s.totalTime4Unsat)
, nbSatCalls(s.nbSatCalls)
, nbUnsatCalls(s.nbUnsatCalls)
{

    s.ca.copyTo(ca);
    ca.extra_clause_field = s.ca.extra_clause_field;


     MYFLAG = 0;


    sumLBD = s.sumLBD;
    nbclausesbeforereduce = s.nbclausesbeforereduce;


    s.watches.copyTo(watches);
    s.watchesBin.copyTo(watchesBin);
    s.unaryWatches.copyTo(unaryWatches);
    s.assigns.memCopyTo(assigns);
    s.vardata.memCopyTo(vardata);
    s.activity.memCopyTo(activity);
    s.seen.memCopyTo(seen);
    s.permDiff.memCopyTo(permDiff);
    s.polarity.memCopyTo(polarity);
    s.decision.memCopyTo(decision);
    s.trail.memCopyTo(trail);
    s.order_heap.copyTo(order_heap);
    s.clauses.memCopyTo(clauses);
    s.learnts.memCopyTo(learnts);

    s.lbdQueue.copyTo(lbdQueue);
    s.trailQueue.copyTo(trailQueue);

}

Solver::~Solver() {
}
void Solver::setIncrementalMode() {
  incremental = true;
}


void Solver::initNbInitialVars(int nb) {
  nbVarsInitialFormula = nb;
}

bool Solver::isIncremental() {
  return incremental;
}
Var Solver::newVar(bool sign, bool dvar) {
    int v = nVars();
    watches .init(mkLit(v, false));
    watches .init(mkLit(v, true));
    watchesBin .init(mkLit(v, false));
    watchesBin .init(mkLit(v, true));
    unaryWatches .init(mkLit(v, false));
    unaryWatches .init(mkLit(v, true));
    assigns .push((Glucose::lbool((uint8_t)2)));
    vardata .push(mkVarData(CRef_Undef, 0));
    activity .push(rnd_init_act ? drand(random_seed) * 0.00001 : 0);
    seen .push(0);
    permDiff .push(0);
    polarity .push(sign);
    decision .push();
    trail .capacity(v + 1);
    setDecisionVar(v, dvar);
    return v;
}

bool Solver::addClause_(vec<Lit>& ps) {

    assert(decisionLevel() == 0);
    if (!ok) return false;


    sort(ps);

    vec<Lit> oc;
    oc.clear();

    Lit p;
    int i, j, flag = 0;
    if (certifiedUNSAT) {
        for (i = j = 0, p = lit_Undef; i < ps.size(); i++) {
            oc.push(ps[i]);
            if (value(ps[i]) == (Glucose::lbool((uint8_t)0)) || ps[i] == ~p || value(ps[i]) == (Glucose::lbool((uint8_t)1)))
                flag = 1;
        }
    }

    for (i = j = 0, p = lit_Undef; i < ps.size(); i++)
        if (value(ps[i]) == (Glucose::lbool((uint8_t)0)) || ps[i] == ~p)
            return true;
        else if (value(ps[i]) != (Glucose::lbool((uint8_t)1)) && ps[i] != p)
            ps[j++] = p = ps[i];
    ps.shrink(i - j);

    if (flag && (certifiedUNSAT)) {
        for (i = j = 0, p = lit_Undef; i < ps.size(); i++)
            fprintf(certifiedOutput, "%i ", (var(ps[i]) + 1) * (-2 * sign(ps[i]) + 1));
        fprintf(certifiedOutput, "0\n");

        fprintf(certifiedOutput, "d ");
        for (i = j = 0, p = lit_Undef; i < oc.size(); i++)
            fprintf(certifiedOutput, "%i ", (var(oc[i]) + 1) * (-2 * sign(oc[i]) + 1));
        fprintf(certifiedOutput, "0\n");
    }


    if (ps.size() == 0)
        return ok = false;
    else if (ps.size() == 1) {
        uncheckedEnqueue(ps[0]);
        return ok = (propagate() == CRef_Undef);
    } else {
        CRef cr = ca.alloc(ps, false);
        clauses.push(cr);
        attachClause(cr);
    }

    return true;
}

void Solver::attachClause(CRef cr) {
    const Clause& c = ca[cr];

    assert(c.size() > 1);
    if (c.size() == 2) {
        watchesBin[~c[0]].push(Watcher(cr, c[1]));
        watchesBin[~c[1]].push(Watcher(cr, c[0]));
    } else {
        watches[~c[0]].push(Watcher(cr, c[1]));
        watches[~c[1]].push(Watcher(cr, c[0]));
    }
    if (c.learnt()) learnts_literals += c.size();
    else clauses_literals += c.size();
}

void Solver::attachClausePurgatory(CRef cr) {
    const Clause& c = ca[cr];

    assert(c.size() > 1);
    unaryWatches[~c[0]].push(Watcher(cr, c[1]));

}

void Solver::detachClause(CRef cr, bool strict) {
    const Clause& c = ca[cr];

    assert(c.size() > 1);
    if (c.size() == 2) {
        if (strict) {
            remove(watchesBin[~c[0]], Watcher(cr, c[1]));
            remove(watchesBin[~c[1]], Watcher(cr, c[0]));
        } else {

            watchesBin.smudge(~c[0]);
            watchesBin.smudge(~c[1]);
        }
    } else {
        if (strict) {
            remove(watches[~c[0]], Watcher(cr, c[1]));
            remove(watches[~c[1]], Watcher(cr, c[0]));
        } else {

            watches.smudge(~c[0]);
            watches.smudge(~c[1]);
        }
    }
    if (c.learnt()) learnts_literals -= c.size();
    else clauses_literals -= c.size();
}




void Solver::detachClausePurgatory(CRef cr, bool strict) {
    const Clause& c = ca[cr];

    assert(c.size() > 1);
    if (strict)
        remove(unaryWatches[~c[0]], Watcher(cr, c[1]));
    else
        unaryWatches.smudge(~c[0]);
}

void Solver::removeClause(CRef cr, bool inPurgatory) {

    Clause& c = ca[cr];

    if (certifiedUNSAT) {
        fprintf(certifiedOutput, "d ");
        for (int i = 0; i < c.size(); i++)
            fprintf(certifiedOutput, "%i ", (var(c[i]) + 1) * (-2 * sign(c[i]) + 1));
        fprintf(certifiedOutput, "0\n");
    }

    if (inPurgatory)
        detachClausePurgatory(cr);
    else
        detachClause(cr);

    if (locked(c)) vardata[var(c[0])].reason = CRef_Undef;
    c.mark(1);
    ca.free(cr);
}

bool Solver::satisfied(const Clause& c) const {
    if(incremental)
        return (value(c[0]) == (Glucose::lbool((uint8_t)0))) || (value(c[1]) == (Glucose::lbool((uint8_t)0)));


    for (int i = 0; i < c.size(); i++)
        if (value(c[i]) == (Glucose::lbool((uint8_t)0)))
            return true;
    return false;
}





inline unsigned int Solver::computeLBD(const vec<Lit> & lits,int end) {
  int nblevels = 0;
  MYFLAG++;

  if(incremental) {
    if(end==-1) end = lits.size();
    int nbDone = 0;
    for(int i=0;i<lits.size();i++) {
      if(nbDone>=end) break;
      if(isSelector(var(lits[i]))) continue;
      nbDone++;
      int l = level(var(lits[i]));
      if (permDiff[l] != MYFLAG) {
 permDiff[l] = MYFLAG;
 nblevels++;
      }
    }
  } else {
    for(int i=0;i<lits.size();i++) {
      int l = level(var(lits[i]));
      if (permDiff[l] != MYFLAG) {
 permDiff[l] = MYFLAG;
 nblevels++;
      }
    }
  }

  if (!reduceOnSize)
    return nblevels;
  if (lits.size() < reduceOnSizeSize) return lits.size();
    return lits.size() + nblevels;
}

inline unsigned int Solver::computeLBD(const Clause &c) {
  int nblevels = 0;
  MYFLAG++;

  if(incremental) {
     unsigned int nbDone = 0;
    for(int i=0;i<c.size();i++) {
      if(nbDone>=c.sizeWithoutSelectors()) break;
      if(isSelector(var(c[i]))) continue;
      nbDone++;
      int l = level(var(c[i]));
      if (permDiff[l] != MYFLAG) {
 permDiff[l] = MYFLAG;
 nblevels++;
      }
    }
  } else {
    for(int i=0;i<c.size();i++) {
      int l = level(var(c[i]));
      if (permDiff[l] != MYFLAG) {
 permDiff[l] = MYFLAG;
 nblevels++;
      }
    }
  }

  if (!reduceOnSize)
    return nblevels;
  if (c.size() < reduceOnSizeSize) return c.size();
    return c.size() + nblevels;

}




void Solver::minimisationWithBinaryResolution(vec<Lit> &out_learnt) {


    unsigned int lbd = computeLBD(out_learnt);
    Lit p = ~out_learnt[0];

    if (lbd <= lbLBDMinimizingClause) {
        MYFLAG++;

        for (int i = 1; i < out_learnt.size(); i++) {
            permDiff[var(out_learnt[i])] = MYFLAG;
        }

        vec<Watcher>& wbin = watchesBin[p];
        int nb = 0;
        for (int k = 0; k < wbin.size(); k++) {
            Lit imp = wbin[k].blocker;
            if (permDiff[var(imp)] == MYFLAG && value(imp) == (Glucose::lbool((uint8_t)0))) {
                nb++;
                permDiff[var(imp)] = MYFLAG - 1;
            }
        }
        int l = out_learnt.size() - 1;
        if (nb > 0) {
            nbReducedClauses++;
            for (int i = 1; i < out_learnt.size() - nb; i++) {
                if (permDiff[var(out_learnt[i])] != MYFLAG) {
                    Lit p = out_learnt[l];
                    out_learnt[l] = out_learnt[i];
                    out_learnt[i] = p;
                    l--;
                    i--;
                }
            }

            out_learnt.shrink(nb);

        }
    }
}




void Solver::cancelUntil(int level) {
    if (decisionLevel() > level) {
        for (int c = trail.size() - 1; c >= trail_lim[level]; c--) {
            Var x = var(trail[c]);
            assigns [x] = (Glucose::lbool((uint8_t)2));
            if (phase_saving > 1 || ((phase_saving == 1) && c > trail_lim.last())) {
                polarity[x] = sign(trail[c]);
            }
            insertVarOrder(x);
        }
        qhead = trail_lim[level];
        trail.shrink(trail.size() - trail_lim[level]);
        trail_lim.shrink(trail_lim.size() - level);
    }
}





Lit Solver::pickBranchLit() {
    Var next = (-1);


    if (drand(random_seed) < random_var_freq && !order_heap.empty()) {
        next = order_heap[irand(random_seed, order_heap.size())];
        if (value(next) == (Glucose::lbool((uint8_t)2)) && decision[next])
            rnd_decisions++;
    }


    while (next == (-1) || value(next) != (Glucose::lbool((uint8_t)2)) || !decision[next])
        if (order_heap.empty()) {
            next = (-1);
            break;
        } else {
            next = order_heap.removeMin();
        }

    return next == (-1) ? lit_Undef : mkLit(next, rnd_pol ? drand(random_seed) < 0.5 : polarity[next]);
}
void Solver::analyze(CRef confl, vec<Lit>& out_learnt,vec<Lit>&selectors, int& out_btlevel,unsigned int &lbd,unsigned int &szWithoutSelectors) {
    int pathC = 0;
    Lit p = lit_Undef;




    out_learnt.push();
    int index = trail.size() - 1;
    do {
        assert(confl != CRef_Undef);
        if(!moreConflict(out_learnt, selectors, pathC)) {
            Clause& c = ca[confl];


            if (p != lit_Undef && c.size() == 2 && value(c[0]) == (Glucose::lbool((uint8_t)1))) {

                assert(value(c[1]) == (Glucose::lbool((uint8_t)0)));
                Lit tmp = c[0];
                c[0] = c[1], c[1] = tmp;
            }

            if (c.learnt()) {
                parallelImportClauseDuringConflictAnalysis(c,confl);
                claBumpActivity(c);
             } else {
                if (!c.getSeen()) {
                    originalClausesSeen++;
                    c.setSeen(true);
                }
            }


            if (c.learnt() && c.lbd() > 2) {
                unsigned int nblevels = computeLBD(c);
                if (nblevels + 1 < c.lbd()) {
                    if (c.lbd() <= lbLBDFrozenClause) {
                        c.setCanBeDel(false);
                    }

                    c.setLBD(nblevels);
                }
            }


            for (int j = (p == lit_Undef) ? 0 : 1; j < c.size(); j++) {
                Lit q = c[j];

                if (!seen[var(q)]) {
                    if (level(var(q)) == 0) {
                    } else {
                        if(!isSelector(var(q)))
                            varBumpActivity(var(q));
                        seen[var(q)] = 1;
                        if (level(var(q)) >= decisionLevel()) {
                            pathC++;

                            if (!isSelector(var(q)) && (reason(var(q)) != CRef_Undef) && ca[reason(var(q))].learnt())
                                lastDecisionLevel.push(q);
                        } else {
                            if(isSelector(var(q))) {
                                assert(value(q) == (Glucose::lbool((uint8_t)1)));
                                selectors.push(q);
                            } else
                                out_learnt.push(q);
                       }
                    }
                }
            }
        }

        bool ret;
        do {

            while (!seen[var(trail[index--])]);
            p = trail[index + 1];
            ret = moreReason(p, out_learnt, selectors, pathC);
            confl = reason(var(p));
            seen[var(p)] = 0;
            pathC--;
        }while(ret);

    } while (pathC > 0);
    out_learnt[0] = ~p;



    int i, j;

    for (int i = 0; i < selectors.size(); i++)
        out_learnt.push(selectors[i]);

    out_learnt.copyTo(analyze_toclear);
    if (ccmin_mode == 2) {
        uint32_t abstract_level = 0;
        for (i = 1; i < out_learnt.size(); i++)
            abstract_level |= abstractLevel(var(out_learnt[i]));

        for (i = j = 1; i < out_learnt.size(); i++)
            if (reason(var(out_learnt[i])) == CRef_Undef || !litRedundant(out_learnt[i], abstract_level))
                out_learnt[j++] = out_learnt[i];

    } else if (ccmin_mode == 1) {
        for (i = j = 1; i < out_learnt.size(); i++) {
            Var x = var(out_learnt[i]);

            if (reason(x) == CRef_Undef)
                out_learnt[j++] = out_learnt[i];
            else {
                Clause& c = ca[reason(var(out_learnt[i]))];

                for (int k = ((c.size() == 2) ? 0 : 1); k < c.size(); k++)
                    if (!seen[var(c[k])] && level(var(c[k])) > 0) {
                        out_learnt[j++] = out_learnt[i];
                        break;
                    }
            }
        }
    } else
        i = j = out_learnt.size();

    max_literals += out_learnt.size();
    out_learnt.shrink(i - j);
    tot_literals += out_learnt.size();
    if (!incremental && out_learnt.size() <= lbSizeMinimizingClause) {
        minimisationWithBinaryResolution(out_learnt);
    }


    if (out_learnt.size() == 1)
        out_btlevel = 0;
    else {
        int max_i = 1;

        for (int i = 2; i < out_learnt.size(); i++)
            if (level(var(out_learnt[i])) > level(var(out_learnt[max_i])))
                max_i = i;

        Lit p = out_learnt[max_i];
        out_learnt[max_i] = out_learnt[1];
        out_learnt[1] = p;
        out_btlevel = level(var(p));
    }
   if(incremental) {
      szWithoutSelectors = 0;
      for(int i=0;i<out_learnt.size();i++) {
 if(!isSelector(var((out_learnt[i])))) szWithoutSelectors++;
 else if(i>0) break;
      }
    } else
      szWithoutSelectors = out_learnt.size();


    lbd = computeLBD(out_learnt,out_learnt.size()-selectors.size());


    if (lastDecisionLevel.size() > 0) {
        for (int i = 0; i < lastDecisionLevel.size(); i++) {
            if (ca[reason(var(lastDecisionLevel[i]))].lbd() < lbd)
                varBumpActivity(var(lastDecisionLevel[i]));
        }
        lastDecisionLevel.clear();
    }



    for (int j = 0; j < analyze_toclear.size(); j++) seen[var(analyze_toclear[j])] = 0;
    for (int j = 0; j < selectors.size(); j++) seen[var(selectors[j])] = 0;
}





bool Solver::litRedundant(Lit p, uint32_t abstract_levels) {
    analyze_stack.clear();
    analyze_stack.push(p);
    int top = analyze_toclear.size();
    while (analyze_stack.size() > 0) {
        assert(reason(var(analyze_stack.last())) != CRef_Undef);
        Clause& c = ca[reason(var(analyze_stack.last()))];
        analyze_stack.pop();
        if (c.size() == 2 && value(c[0]) == (Glucose::lbool((uint8_t)1))) {
            assert(value(c[1]) == (Glucose::lbool((uint8_t)0)));
            Lit tmp = c[0];
            c[0] = c[1], c[1] = tmp;
        }

        for (int i = 1; i < c.size(); i++) {
            Lit p = c[i];
            if (!seen[var(p)]) {
                if (level(var(p)) > 0) {
                    if (reason(var(p)) != CRef_Undef && (abstractLevel(var(p)) & abstract_levels) != 0) {
                        seen[var(p)] = 1;
                        analyze_stack.push(p);
                        analyze_toclear.push(p);
                    } else {
                        for (int j = top; j < analyze_toclear.size(); j++)
                            seen[var(analyze_toclear[j])] = 0;
                        analyze_toclear.shrink(analyze_toclear.size() - top);
                        return false;
                    }
                }
            }
        }
    }

    return true;
}
void Solver::analyzeFinal(Lit p, vec<Lit>& out_conflict) {
    out_conflict.clear();
    out_conflict.push(p);

    if (decisionLevel() == 0)
        return;

    seen[var(p)] = 1;

    for (int i = trail.size() - 1; i >= trail_lim[0]; i--) {
        Var x = var(trail[i]);
        if (seen[x]) {
            if(!moreReason(trail[i])) {
                if (reason(x) == CRef_Undef) {
                    assert(level(x) > 0);
                    out_conflict.push(~trail[i]);
                } else {
                    Clause& c = ca[reason(x)];



                    for (int j = ((c.size() == 2) ? 0 : 1); j < c.size(); j++)
                        if (level(var(c[j])) > 0)
                            seen[var(c[j])] = 1;
                }
            }

            seen[x] = 0;
        }
    }

    seen[var(p)] = 0;
}

void Solver::uncheckedEnqueue(Lit p, CRef from) {
    assert(value(p) == (Glucose::lbool((uint8_t)2)));
    assigns[var(p)] = lbool(!sign(p));
    vardata[var(p)] = mkVarData(from, decisionLevel());
    trail.push_(p);
}
CRef Solver::propagate() {
  {static long long int npropiter = 0;
    npropiter++;
    if((npropiter&((1ll<<14)-1)) == 0) {
      if(cpuTime() > satTL) throw TimeLimitException{};
    }
  }
    CRef confl = CRef_Undef;
    int num_props = 0;

    watches.cleanAll();
    watchesBin.cleanAll();
    unaryWatches.cleanAll();
    while (qhead < trail.size()) {
        Lit p = trail[qhead++];
        vec<Watcher>& ws = watches[p];
        Watcher *i, *j, *end;
        num_props++;



        vec<Watcher>& wbin = watchesBin[p];

        for (int k = 0; k < wbin.size(); k++) {

            Lit imp = wbin[k].blocker;

            if (value(imp) == (Glucose::lbool((uint8_t)1))) {
                return wbin[k].cref;
            }

            if (value(imp) == (Glucose::lbool((uint8_t)2))) {
                uncheckedEnqueue(imp, wbin[k].cref);
            }
        }


        for (i = j = (Watcher*) ws, end = i + ws.size(); i != end;) {

            Lit blocker = i->blocker;
            if (value(blocker) == (Glucose::lbool((uint8_t)0))) {
                *j++ = *i++;
                continue;
            }


            CRef cr = i->cref;
            Clause& c = ca[cr];
            assert(!c.getOneWatched());
            Lit false_lit = ~p;
            if (c[0] == false_lit)
                c[0] = c[1], c[1] = false_lit;
            assert(c[1] == false_lit);
            i++;


            Lit first = c[0];
            Watcher w = Watcher(cr, first);
            if (first != blocker && value(first) == (Glucose::lbool((uint8_t)0))) {

                *j++ = w;
                continue;
            }
     if(incremental) {
       int choosenPos = -1;
       for (int k = 2; k < c.size(); k++) {

  if (value(c[k]) != (Glucose::lbool((uint8_t)1))){
    if(decisionLevel()>assumptions.size()) {
      choosenPos = k;
      break;
    } else {
      choosenPos = k;

      if(value(c[k])==(Glucose::lbool((uint8_t)0)) || !isSelector(var(c[k]))) {
        break;
      }
    }

  }
       }
       if(choosenPos!=-1) {
  c[1] = c[choosenPos]; c[choosenPos] = false_lit;
  watches[~c[1]].push(w);
  goto NextClause; }
     } else {
       for (int k = 2; k < c.size(); k++) {

  if (value(c[k]) != (Glucose::lbool((uint8_t)1))){
    c[1] = c[k]; c[k] = false_lit;
    watches[~c[1]].push(w);
    goto NextClause; }
       }
     }


            *j++ = w;
            if (value(first) == (Glucose::lbool((uint8_t)1))) {
                confl = cr;
                qhead = trail.size();

                while (i < end)
                    *j++ = *i++;
            } else {
                uncheckedEnqueue(first, cr);


            }
NextClause:
            ;
        }
        ws.shrink(i - j);


        if (useUnaryWatched && confl == CRef_Undef) {
            confl = propagateUnaryWatches(p);

        }

    }



    propagations += num_props;
    simpDB_props -= num_props;

    return confl;
}
CRef Solver::propagateUnaryWatches(Lit p) {
    CRef confl= CRef_Undef;
    Watcher *i, *j, *end;
    vec<Watcher>& ws = unaryWatches[p];
    for (i = j = (Watcher*) ws, end = i + ws.size(); i != end;) {

        Lit blocker = i->blocker;
        if (value(blocker) == (Glucose::lbool((uint8_t)0))) {
            *j++ = *i++;
            continue;
        }


        CRef cr = i->cref;
        Clause& c = ca[cr];
        assert(c.getOneWatched());
        Lit false_lit = ~p;
        assert(c[0] == false_lit);



        i++;
        Watcher w = Watcher(cr, c[0]);
        for (int k = 1; k < c.size(); k++) {
            if (value(c[k]) != (Glucose::lbool((uint8_t)1))) {
                c[0] = c[k];
                c[k] = false_lit;
                unaryWatches[~c[0]].push(w);
                goto NextClauseUnary;
            }
        }


        *j++ = w;

        confl = cr;
        qhead = trail.size();

        while (i < end)
            *j++ = *i++;



        if (promoteOneWatchedClause) {
            nbPromoted++;

            int maxlevel = -1;
            int index = -1;
            for (int k = 1; k < c.size(); k++) {
                assert(value(c[k]) == (Glucose::lbool((uint8_t)1)));
                assert(level(var(c[k])) <= level(var(c[0])));
                if (level(var(c[k])) > maxlevel) {
                    index = k;
                    maxlevel = level(var(c[k]));
                }
            }
            detachClausePurgatory(cr, true);
            assert(index != -1);
            Lit tmp = c[1];
            c[1] = c[index], c[index] = tmp;
            attachClause(cr);



            ca[cr].setOneWatched(false);
            ca[cr].setExported(2);
        }
NextClauseUnary:
        ;
    }
    ws.shrink(i - j);

    return confl;
}
void Solver::reduceDB()
{

  int i, j;
  nbReduceDB++;
  sort(learnts, reduceDB_lt(ca));


  if(ca[learnts[learnts.size() / 2]].lbd()<=3) nbclausesbeforereduce +=specialIncReduceDB;

  if(ca[learnts.last()].lbd()<=5) nbclausesbeforereduce +=specialIncReduceDB;





  int limit = learnts.size() / 2;

  for (i = j = 0; i < learnts.size(); i++){
    Clause& c = ca[learnts[i]];
    if (c.lbd()>2 && c.size() > 2 && c.canBeDel() && !locked(c) && (i < limit)) {
      removeClause(learnts[i]);
      nbRemovedClauses++;
    }
    else {
      if(!c.canBeDel()) limit++;
      c.setCanBeDel(true);
      learnts[j++] = learnts[i];
    }
  }
  learnts.shrink(i - j);
  checkGarbage();
}


void Solver::removeSatisfied(vec<CRef>& cs) {

    int i, j;
    for (i = j = 0; i < cs.size(); i++) {
        Clause& c = ca[cs[i]];


        if (satisfied(c))
            if (c.getOneWatched())
                removeClause(cs[i], true);
            else
                removeClause(cs[i]);
        else
            cs[j++] = cs[i];
    }
    cs.shrink(i - j);
}

void Solver::rebuildOrderHeap() {
    vec<Var> vs;
    for (Var v = 0; v < nVars(); v++)
        if (decision[v] && value(v) == (Glucose::lbool((uint8_t)2)))
            vs.push(v);
    order_heap.build(vs);

}
bool Solver::simplify() {
    assert(decisionLevel() == 0);

    if (!ok) return ok = false;
    else {
        CRef cr = propagate();
        if (cr != CRef_Undef) {
            return ok = false;
        }
    }


    if (nAssigns() == simpDB_assigns || (simpDB_props > 0))
        return true;


    removeSatisfied(learnts);
    removeSatisfied(unaryWatchedClauses);
    if (remove_satisfied)
        removeSatisfied(clauses);
    checkGarbage();
    rebuildOrderHeap();

    simpDB_assigns = nAssigns();
    simpDB_props = clauses_literals + learnts_literals;

    return true;
}
lbool Solver::search(int) {
    assert(ok);
    int backtrack_level;
    int conflictC = 0;
    vec<Lit> learnt_clause, selectors;
    unsigned int nblevels,szWithoutSelectors = 0;
    bool blocked = false;
    starts++;
    for (;;) {
        if (decisionLevel() == 0) {
            parallelImportUnaryClauses();

            if (parallelImportClauses())
                return (Glucose::lbool((uint8_t)1));

        }
        CRef confl = propagate();

        if (confl != CRef_Undef) {
            if(parallelJobIsFinished())
                return (Glucose::lbool((uint8_t)2));


            sumDecisionLevels += decisionLevel();

            conflicts++;
            conflictC++;
            conflictsRestarts++;
           if (conflicts % 5000 == 0 && var_decay < max_var_decay)
                var_decay += 0.01;

            if (verbosity >= 1 && conflicts % verbEveryConflicts == 0) {
                printf("c | %8d   %7d    %5d | %7d %8d %8d | %5d %8d   %6d %8d | %6.3f %% |\n",
                        (int) starts, (int) nbstopsrestarts, (int) (conflicts / starts),
                        (int) dec_vars - (trail_lim.size() == 0 ? trail.size() : trail_lim[0]), nClauses(), (int) clauses_literals,
                        (int) nbReduceDB, nLearnts(), (int) nbDL2, (int) nbRemovedClauses, progressEstimate()*100);
            }
            if (decisionLevel() == 0) {
                return (Glucose::lbool((uint8_t)1));

            }

            trailQueue.push(trail.size());

            if (conflictsRestarts > 10000 && lbdQueue.isvalid() && trail.size() > R * trailQueue.getavg()) {
                lbdQueue.fastclear();
                nbstopsrestarts++;
                if (!blocked) {
                    lastblockatrestart = starts;
                    nbstopsrestartssame++;
                    blocked = true;
                }
            }

            learnt_clause.clear();
            selectors.clear();

            analyze(confl, learnt_clause, selectors, backtrack_level, nblevels,szWithoutSelectors);



            lbdQueue.push(nblevels);
            sumLBD += nblevels;

            cancelUntil(backtrack_level);

            if (certifiedUNSAT) {
                for (int i = 0; i < learnt_clause.size(); i++)
                    fprintf(certifiedOutput, "%i ", (var(learnt_clause[i]) + 1) *
                        (-2 * sign(learnt_clause[i]) + 1));
                fprintf(certifiedOutput, "0\n");
            }


            if (learnt_clause.size() == 1) {
                uncheckedEnqueue(learnt_clause[0]);
                nbUn++;
                parallelExportUnaryClause(learnt_clause[0]);
            } else {
                CRef cr = ca.alloc(learnt_clause, true);
                ca[cr].setLBD(nblevels);
                ca[cr].setOneWatched(false);
  ca[cr].setSizeWithoutSelectors(szWithoutSelectors);
                if (nblevels <= 2) nbDL2++;
                if (ca[cr].size() == 2) nbBin++;
                learnts.push(cr);
                attachClause(cr);
                lastLearntClause = cr;
                parallelExportClauseDuringSearch(ca[cr]);
                claBumpActivity(ca[cr]);
                uncheckedEnqueue(learnt_clause[0], cr);

            }
            varDecayActivity();
            claDecayActivity();


        } else {

            if (
                    (lbdQueue.isvalid() && ((lbdQueue.getavg() * K) > (sumLBD / conflictsRestarts)))) {
                lbdQueue.fastclear();
                progress_estimate = progressEstimate();
                int bt = 0;
                if(incremental)
                    bt = (decisionLevel()<assumptions.size()) ? decisionLevel() : assumptions.size();
                cancelUntil(bt);
                return (Glucose::lbool((uint8_t)2));
            }



            if (decisionLevel() == 0 && !simplify()) {
                return (Glucose::lbool((uint8_t)1));
            }

            if (conflicts >= ((unsigned int) curRestart * nbclausesbeforereduce)) {

                if (learnts.size() > 0) {
                    curRestart = (conflicts / nbclausesbeforereduce) + 1;
                    reduceDB();
                    if (!panicModeIsEnabled())
                        nbclausesbeforereduce += incReduceDB;
                }
            }

            lastLearntClause = CRef_Undef;
            Lit next = lit_Undef;
            while (decisionLevel() < assumptions.size()) {

                Lit p = assumptions[decisionLevel()];
                if (value(p) == (Glucose::lbool((uint8_t)0))) {

                    newDecisionLevel();
                } else if (value(p) == (Glucose::lbool((uint8_t)1))) {
                    analyzeFinal(~p, conflict);
                    return (Glucose::lbool((uint8_t)1));
                } else {
                    next = p;
                    break;
                }
            }

            if (next == lit_Undef) {

                decisions++;
                next = pickBranchLit();
                if (next == lit_Undef) {
                    printf("c last restart ## conflicts  :  %d %d \n", conflictC, decisionLevel());

                    return (Glucose::lbool((uint8_t)0));
                }
            }


            newDecisionLevel();
            uncheckedEnqueue(next);
        }
    }
}

double Solver::progressEstimate() const {
    double progress = 0;
    double F = 1.0 / nVars();

    for (int i = 0; i <= decisionLevel(); i++) {
        int beg = i == 0 ? 0 : trail_lim[i - 1];
        int end = i == decisionLevel() ? trail.size() : trail_lim[i];
        progress += pow(F, i) * (end - beg);
    }

    return progress / nVars();
}

void Solver::printIncrementalStats() {

    printf("c---------- Glucose Stats -------------------------\n");
    printf("c restarts              : %" "lu""\n", starts);
    printf("c nb ReduceDB           : %" "lu""\n", nbReduceDB);
    printf("c nb removed Clauses    : %" "lu""\n", nbRemovedClauses);
    printf("c nb learnts DL2        : %" "lu""\n", nbDL2);
    printf("c nb learnts size 2     : %" "lu""\n", nbBin);
    printf("c nb learnts size 1     : %" "lu""\n", nbUn);

    printf("c conflicts             : %" "lu""\n", conflicts);
    printf("c decisions             : %" "lu""\n", decisions);
    printf("c propagations          : %" "lu""\n", propagations);

  printf("\nc SAT Calls             : %d in %g seconds\n",nbSatCalls,totalTime4Sat);
  printf("c UNSAT Calls           : %d in %g seconds\n",nbUnsatCalls,totalTime4Unsat);

    printf("c--------------------------------------------------\n");
}



lbool Solver::solve_(bool, bool)
{

    if(incremental && certifiedUNSAT) {
    printf("Can not use incremental and certified unsat in the same time\n");
    exit(-1);
  }

    model.clear();
    conflict.clear();
    if (!ok) return (Glucose::lbool((uint8_t)1));
    double curTime = cpuTime();
    if(curTime > satTL) throw TimeLimitException{};

    solves++;



    lbool status = (Glucose::lbool((uint8_t)2));
    if(!incremental && verbosity>=1) {
      printf("c ========================================[ MAGIC CONSTANTS ]==============================================\n");
      printf("c | Constants are supposed to work well together :-)                                                      |\n");
      printf("c | however, if you find better choices, please let us known...                                           |\n");
      printf("c |-------------------------------------------------------------------------------------------------------|\n");
      printf("c |                                |                                |                                     |\n");
      printf("c | - Restarts:                    | - Reduce Clause DB:            | - Minimize Asserting:               |\n");
      printf("c |   * LBD Queue    : %6d      |   * First     : %6d         |    * size < %3d                     |\n",lbdQueue.maxSize(),nbclausesbeforereduce,lbSizeMinimizingClause);
      printf("c |   * Trail  Queue : %6d      |   * Inc       : %6d         |    * lbd  < %3d                     |\n",trailQueue.maxSize(),incReduceDB,lbLBDMinimizingClause);
      printf("c |   * K            : %6.2f      |   * Special   : %6d         |                                     |\n",K,specialIncReduceDB);
      printf("c |   * R            : %6.2f      |   * Public :  (lbd)< %2d     |                                     |\n",R,lbLBDFrozenClause);
      printf("c |                                |                                |                                     |\n");
      printf("c ==================================[ Search Statistics (every %6d conflicts) ]=========================\n",verbEveryConflicts);
      printf("c |                                                                                                       |\n");

      printf("c |          RESTARTS           |          ORIGINAL         |              LEARNT              | Progress |\n");
      printf("c |       NB   Blocked  Avg Cfc |    Vars  Clauses Literals |   Red   Learnts    LBD2  Removed |          |\n");
      printf("c =========================================================================================================\n");
    }


    int curr_restarts = 0;
    while (status == (Glucose::lbool((uint8_t)2))){
      status = search(0);

        if (!withinBudget()) break;
        curr_restarts++;
    }

    if (!incremental && verbosity >= 1)
      printf("c =========================================================================================================\n");

    if (certifiedUNSAT){
      if (status == (Glucose::lbool((uint8_t)1)))
 fprintf(certifiedOutput, "0\n");
      fclose(certifiedOutput);
    }


    if (status == (Glucose::lbool((uint8_t)0))){

        model.growTo(nVars());
        for (int i = 0; i < nVars(); i++) model[i] = value(i);
    }else if (status == (Glucose::lbool((uint8_t)1)) && conflict.size() == 0)
        ok = false;



    cancelUntil(0);


    double finalTime = cpuTime();
    if(finalTime > satTL) throw TimeLimitException{};
    if(status==(Glucose::lbool((uint8_t)0))) {
        nbSatCalls++;
        totalTime4Sat +=(finalTime-curTime);
    }
    if(status==(Glucose::lbool((uint8_t)1))) {
        nbUnsatCalls++;
        totalTime4Unsat +=(finalTime-curTime);
    }


    return status;

}
static Var mapVar(Var x, vec<Var>& map, Var& max) {
    if (map.size() <= x || map[x] == -1) {
        map.growTo(x + 1, -1);
        map[x] = max++;
    }
    return map[x];
}

void Solver::toDimacs(FILE* f, Clause& c, vec<Var>& map, Var& max) {
    if (satisfied(c)) return;

    for (int i = 0; i < c.size(); i++)
        if (value(c[i]) != (Glucose::lbool((uint8_t)1)))
            fprintf(f, "%s%d ", sign(c[i]) ? "-" : "", mapVar(var(c[i]), map, max) + 1);
    fprintf(f, "0\n");
}

void Solver::toDimacs(const char *file, const vec<Lit>& assumps) {
    FILE* f = fopen(file, "wr");
    if (f == NULL)
        fprintf(stderr, "could not open file %s\n", file), exit(1);
    toDimacs(f, assumps);
    fclose(f);
}

void Solver::toDimacs(FILE* f, const vec<Lit>&) {

    if (!ok) {
        fprintf(f, "p cnf 1 2\n1 0\n-1 0\n");
        return;
    }

    vec<Var> map;
    Var max = 0;



    int cnt = 0;
    for (int i = 0; i < clauses.size(); i++)
        if (!satisfied(ca[clauses[i]]))
            cnt++;

    for (int i = 0; i < clauses.size(); i++)
        if (!satisfied(ca[clauses[i]])) {
            Clause& c = ca[clauses[i]];
            for (int j = 0; j < c.size(); j++)
                if (value(c[j]) != (Glucose::lbool((uint8_t)1)))
                    mapVar(var(c[j]), map, max);
        }


    cnt += assumptions.size();

    fprintf(f, "p cnf %d %d\n", max, cnt);

    for (int i = 0; i < assumptions.size(); i++) {
        assert(value(assumptions[i]) != (Glucose::lbool((uint8_t)1)));
        fprintf(f, "%s%d 0\n", sign(assumptions[i]) ? "-" : "", mapVar(var(assumptions[i]), map, max) + 1);
    }

    for (int i = 0; i < clauses.size(); i++)
        toDimacs(f, ca[clauses[i]], map, max);

    if (verbosity > 0)
        printf("Wrote %d clauses with %d variables.\n", cnt, max);
}





void Solver::relocAll(ClauseAllocator& to) {



    watches.cleanAll();
    watchesBin.cleanAll();
    unaryWatches.cleanAll();
    for (int v = 0; v < nVars(); v++)
        for (int s = 0; s < 2; s++) {
            Lit p = mkLit(v, s);

            vec<Watcher>& ws = watches[p];
            for (int j = 0; j < ws.size(); j++)
                ca.reloc(ws[j].cref, to);
            vec<Watcher>& ws2 = watchesBin[p];
            for (int j = 0; j < ws2.size(); j++)
                ca.reloc(ws2[j].cref, to);
            vec<Watcher>& ws3 = unaryWatches[p];
            for (int j = 0; j < ws3.size(); j++)
                ca.reloc(ws3[j].cref, to);
        }



    for (int i = 0; i < trail.size(); i++) {
        Var v = var(trail[i]);

        if (reason(v) != CRef_Undef && (ca[reason(v)].reloced() || locked(ca[reason(v)])))
            ca.reloc(vardata[v].reason, to);
    }



    for (int i = 0; i < learnts.size(); i++)
        ca.reloc(learnts[i], to);



    for (int i = 0; i < clauses.size(); i++)
        ca.reloc(clauses[i], to);

    for (int i = 0; i < unaryWatchedClauses.size(); i++)
        ca.reloc(unaryWatchedClauses[i], to);
}



void Solver::garbageCollect() {


    ClauseAllocator to(ca.size() - ca.wasted());

    relocAll(to);
    if (verbosity >= 2)
        printf("|  Garbage collection:   %12d bytes => %12d bytes             |\n",
            ca.size() * ClauseAllocator::Unit_Size, to.size() * ClauseAllocator::Unit_Size);
    to.moveTo(ca);
}







bool Solver::panicModeIsEnabled() {
    return false;
}

void Solver::parallelImportUnaryClauses() {
}

bool Solver::parallelImportClauses() {
    return false;
}


void Solver::parallelExportUnaryClause(Lit) {
}
void Solver::parallelExportClauseDuringSearch(Clause &) {
}

bool Solver::parallelJobIsFinished() {

    return false;
}

void Solver::parallelImportClauseDuringConflictAnalysis(Clause &,CRef) {
}
namespace aspino {

template <class T>
void shuffle(vec<T>& v, double random_seed) {
    for(int i = 0; i < v.size(); i++) {
        int j = Glucose::Solver::irand(random_seed, v.size());
        T tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}

template <class T>
void reverse(vec<T>& v) {
    for(int i = 0, j = v.size()-1; i < v.size() / 2; i++, j--) {
        T tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}

template <class T>
void quickSort(vec<T>& arr, int left, int right) {
  sort(arr.data+left,arr.data+right);
}

template <class T>
void sort(vec<T>& v) {
    if(v.size() > 1) quickSort(v, 0, v.size()-1);
}

}


namespace aspino {

  using Glucose::lbool;
  using Glucose::Lit;
  using Glucose::Var;
  using Glucose::mkLit;
  using Glucose::vec;
  using Glucose::CRef;
  using Glucose::CRef_Undef;
  using Glucose::lit_Undef;
  using Glucose::ClauseAllocator;
  using Glucose::Clause;

  ostream& operator<<(ostream& o, lbool l);
  ostream& operator<<(ostream& o, const Lit& l);
  ostream& operator<<(ostream& o, const Clause& c);
  template <class T> ostream& operator<<(ostream& o, const vec<T>& v) {
    o << "[ ";
    for(int i = 0; i < v.size(); i++)
      o << v[i] << " ";
    return o << "]";
  }

  class AbstractSolver {
  public:
    AbstractSolver() {}
    virtual ~AbstractSolver() {}

    virtual void interrupt() = 0;

    virtual void parse(gzFile in) = 0;
    virtual bool simplify() = 0;
    virtual bool eliminate(bool turn_off_elim) = 0;
    virtual bool okay() const = 0;
    virtual lbool solve() = 0;
    virtual lbool solve(int n) = 0;
  };

}


namespace Glucose {




template<class B, class Solver>
static void readClause(B& in, Solver& S, vec<Lit>& lits) {
    int parsed_lit, var;
    lits.clear();
    for (;;){
        parsed_lit = parseInt(in);
        if (parsed_lit == 0) break;
        var = abs(parsed_lit)-1;
        while (var >= S.nVars()) S.newVar();
        lits.push( (parsed_lit > 0) ? mkLit(var) : ~mkLit(var) );
    }
}

template<class B, class Solver>
static void parse_DIMACS_main(B& in, Solver& S) {
    vec<Lit> lits;
    int vars = 0;
    int clauses = 0;
    int cnt = 0;
    for (;;){
        skipWhitespace(in);
        if (*in == EOF) break;
        else if (*in == 'p'){
            if (eagerMatch(in, "p cnf")){
                vars = parseInt(in);
                clauses = parseInt(in);



            }else{
                printf("PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
            }
        } else if (*in == 'c' || *in == 'p')
            skipLine(in);
        else{
            cnt++;
            readClause(in, S, lits);
            S.addClause_(lits); }
    }
    if (vars != S.nVars())
        fprintf(stderr, "WARNING! DIMACS header mismatch: wrong number of variables.\n");
    if (cnt != clauses)
        fprintf(stderr, "WARNING! DIMACS header mismatch: wrong number of clauses.\n");
}



template<class Solver>
static void parse_DIMACS(gzFile input_stream, Solver& S) {
    StreamBuffer in(input_stream);
    parse_DIMACS_main(in, S); }


}



namespace Glucose {




class SimpSolver : public Solver {
 public:


    SimpSolver();
    ~SimpSolver();

    SimpSolver(const SimpSolver &s);





    virtual Clone* clone() const {
        return new SimpSolver(*this);
    }




    virtual Var newVar (bool polarity = true, bool dvar = true);
    bool addClause (const vec<Lit>& ps);
    bool addEmptyClause();
    bool addClause (Lit p);
    bool addClause (Lit p, Lit q);
    bool addClause (Lit p, Lit q, Lit r);
    virtual bool addClause_( vec<Lit>& ps);
    bool substitute(Var v, Lit x);



    void setFrozen (Var v, bool b);
    bool isEliminated(Var v) const;



    bool solve (const vec<Lit>& assumps, bool do_simp = true, bool turn_off_simp = false);
    lbool solveLimited(const vec<Lit>& assumps, bool do_simp = true, bool turn_off_simp = false);
    bool solve ( bool do_simp = true, bool turn_off_simp = false);
    bool solve (Lit p , bool do_simp = true, bool turn_off_simp = false);
    bool solve (Lit p, Lit q, bool do_simp = true, bool turn_off_simp = false);
    bool solve (Lit p, Lit q, Lit r, bool do_simp = true, bool turn_off_simp = false);
    bool eliminate (bool turn_off_elim = false);



    virtual void garbageCollect();
    int parsing;
    int grow;
    int clause_lim;

    int subsumption_lim;
    double simp_garbage_frac;

    bool use_asymm;
    bool use_rcheck;
    bool use_elim;


    int merges;
    int asymm_lits;
    int eliminated_vars;




    struct ElimLt {
        const vec<int>& n_occ;
        explicit ElimLt(const vec<int>& no) : n_occ(no) {}



        uint64_t cost (Var x) const { return (uint64_t)n_occ[toInt(mkLit(x))] * (uint64_t)n_occ[toInt(~mkLit(x))]; }
        bool operator()(Var x, Var y) const { return cost(x) < cost(y); }






    };

    struct ClauseDeleted {
        const ClauseAllocator& ca;
        explicit ClauseDeleted(const ClauseAllocator& _ca) : ca(_ca) {}
        bool operator()(const CRef& cr) const { return ca[cr].mark() == 1; } };



    int elimorder;
    bool use_simplification;
    vec<uint32_t> elimclauses;
    vec<char> touched;
    OccLists<Var, vec<CRef>, ClauseDeleted>
                        occurs;
    vec<int> n_occ;
    Heap<ElimLt> elim_heap;
    Queue<CRef> subsumption_queue;
    vec<char> frozen;
    vec<char> eliminated;
    int bwdsub_assigns;
    int n_touched;



    CRef bwdsub_tmpunit;



    virtual lbool solve_ (bool do_simp = true, bool turn_off_simp = false);
    bool asymm (Var v, CRef cr);
    bool asymmVar (Var v);
    void updateElimHeap (Var v);
    void gatherTouchedClauses ();
    bool merge (const Clause& _ps, const Clause& _qs, Var v, vec<Lit>& out_clause);
    bool merge (const Clause& _ps, const Clause& _qs, Var v, int& size);
    bool backwardSubsumptionCheck (bool verbose = false);
    bool eliminateVar (Var v);
    void extendModel ();

    void removeClause (CRef cr,bool inPurgatory=false);
    bool strengthenClause (CRef cr, Lit l);
    void cleanUpClauses ();
    bool implied (const vec<Lit>& c);
    virtual void relocAll (ClauseAllocator& to);
};






inline bool SimpSolver::isEliminated (Var v) const { return eliminated[v]; }
inline void SimpSolver::updateElimHeap(Var v) {
    assert(use_simplification);

    if (elim_heap.inHeap(v) || (!frozen[v] && !isEliminated(v) && value(v) == (Glucose::lbool((uint8_t)2))))
        elim_heap.update(v); }


inline bool SimpSolver::addClause (const vec<Lit>& ps) { ps.copyTo(add_tmp); return addClause_(add_tmp); }
inline bool SimpSolver::addEmptyClause() { add_tmp.clear(); return addClause_(add_tmp); }
inline bool SimpSolver::addClause (Lit p) { add_tmp.clear(); add_tmp.push(p); return addClause_(add_tmp); }
inline bool SimpSolver::addClause (Lit p, Lit q) { add_tmp.clear(); add_tmp.push(p); add_tmp.push(q); return addClause_(add_tmp); }
inline bool SimpSolver::addClause (Lit p, Lit q, Lit r) { add_tmp.clear(); add_tmp.push(p); add_tmp.push(q); add_tmp.push(r); return addClause_(add_tmp); }
inline void SimpSolver::setFrozen (Var v, bool b) { frozen[v] = (char)b; if (use_simplification && !b) { updateElimHeap(v); } }

inline bool SimpSolver::solve ( bool do_simp, bool turn_off_simp) { budgetOff(); assumptions.clear(); return solve_(do_simp, turn_off_simp) == (Glucose::lbool((uint8_t)0)); }
inline bool SimpSolver::solve (Lit p , bool do_simp, bool turn_off_simp) { budgetOff(); assumptions.clear(); assumptions.push(p); return solve_(do_simp, turn_off_simp) == (Glucose::lbool((uint8_t)0)); }
inline bool SimpSolver::solve (Lit p, Lit q, bool do_simp, bool turn_off_simp) { budgetOff(); assumptions.clear(); assumptions.push(p); assumptions.push(q); return solve_(do_simp, turn_off_simp) == (Glucose::lbool((uint8_t)0)); }
inline bool SimpSolver::solve (Lit p, Lit q, Lit r, bool do_simp, bool turn_off_simp) { budgetOff(); assumptions.clear(); assumptions.push(p); assumptions.push(q); assumptions.push(r); return solve_(do_simp, turn_off_simp) == (Glucose::lbool((uint8_t)0)); }
inline bool SimpSolver::solve (const vec<Lit>& assumps, bool do_simp, bool turn_off_simp){
    budgetOff(); assumps.copyTo(assumptions); return solve_(do_simp, turn_off_simp) == (Glucose::lbool((uint8_t)0)); }

inline lbool SimpSolver::solveLimited (const vec<Lit>& assumps, bool do_simp, bool turn_off_simp){
    assumps.copyTo(assumptions); return solve_(do_simp, turn_off_simp); }


}


static const char* _cat2 = "SIMP";

static BoolOption opt_use_asymm (_cat2, "asymm", "Shrink clauses by asymmetric branching.", false);
static BoolOption opt_use_rcheck (_cat2, "rcheck", "Check if a clause is already implied. (costly)", false);
static BoolOption opt_use_elim (_cat2, "elim", "Perform variable elimination.", true);
static IntOption opt_grow (_cat2, "grow", "Allow a variable elimination step to grow by a number of clauses.", 0);
static IntOption opt_clause_lim (_cat2, "cl-lim", "Variables are not eliminated if it produces a resolvent with a length above this limit. -1 means no limit", 20, IntRange(-1, INT32_MAX));
static IntOption opt_subsumption_lim (_cat2, "sub-lim", "Do not check if subsumption against a clause larger than this. -1 means no limit.", 1000, IntRange(-1, INT32_MAX));
static DoubleOption opt_simp_garbage_frac(_cat2, "simp-gc-frac", "The fraction of wasted memory allowed before a garbage collection is triggered during simplification.", 0.5, DoubleRange(0, false, HUGE_VAL, false));






SimpSolver::SimpSolver() :
   Solver()
  , grow (opt_grow)
  , clause_lim (opt_clause_lim)
  , subsumption_lim (opt_subsumption_lim)
  , simp_garbage_frac (opt_simp_garbage_frac)
  , use_asymm (opt_use_asymm)
  , use_rcheck (opt_use_rcheck)
  , use_elim (opt_use_elim)
  , merges (0)
  , asymm_lits (0)
  , eliminated_vars (0)
  , elimorder (1)
  , use_simplification (true)
  , occurs (ClauseDeleted(ca))
  , elim_heap (ElimLt(n_occ))
  , bwdsub_assigns (0)
  , n_touched (0)
{
    vec<Lit> dummy(1,lit_Undef);
    ca.extra_clause_field = true;
    bwdsub_tmpunit = ca.alloc(dummy);
    remove_satisfied = false;
}


SimpSolver::~SimpSolver()
{
}



SimpSolver::SimpSolver(const SimpSolver &s) : Solver(s)
  , grow (s.grow)
  , clause_lim (s.clause_lim)
  , subsumption_lim (s.subsumption_lim)
  , simp_garbage_frac (s.simp_garbage_frac)
  , use_asymm (s.use_asymm)
  , use_rcheck (s.use_rcheck)
  , use_elim (s.use_elim)
  , merges (s.merges)
  , asymm_lits (s.asymm_lits)
  , eliminated_vars (s.eliminated_vars)
  , elimorder (s.elimorder)
  , use_simplification (s.use_simplification)
  , occurs (ClauseDeleted(ca))
  , elim_heap (ElimLt(n_occ))
  , bwdsub_assigns (s.bwdsub_assigns)
  , n_touched (s.n_touched)
{

    vec<Lit> dummy(1,lit_Undef);
    ca.extra_clause_field = true;
    bwdsub_tmpunit = ca.alloc(dummy);
    remove_satisfied = false;



    s.elimclauses.memCopyTo(elimclauses);
    s.touched.memCopyTo(touched);
    s.occurs.copyTo(occurs);
    s.n_occ.memCopyTo(n_occ);
    s.elim_heap.copyTo(elim_heap);
    s.subsumption_queue.copyTo(subsumption_queue);
    s.frozen.memCopyTo(frozen);
    s.eliminated.memCopyTo(eliminated);

    use_simplification = s.use_simplification;
    bwdsub_assigns = s.bwdsub_assigns;
    n_touched = s.n_touched;
    bwdsub_tmpunit = s.bwdsub_tmpunit;
    qhead = s.qhead;
    ok = s.ok;
}



Var SimpSolver::newVar(bool sign, bool dvar) {
    Var v = Solver::newVar(sign, dvar);
    frozen .push((char)false);
    eliminated.push((char)false);

    if (use_simplification){
        n_occ .push(0);
        n_occ .push(0);
        occurs .init(v);
        touched .push(0);
        elim_heap .insert(v);
    }
    return v; }

lbool SimpSolver::solve_(bool do_simp, bool turn_off_simp)
{
    vec<Var> extra_frozen;
    lbool result = (Glucose::lbool((uint8_t)0));
    do_simp &= use_simplification;

    if (do_simp){

        for (int i = 0; i < assumptions.size(); i++){
            Var v = var(assumptions[i]);


            assert(!isEliminated(v));

            if (!frozen[v]){

                setFrozen(v, true);
                extra_frozen.push(v);
            } }

        result = lbool(eliminate(turn_off_simp));
    }

    if (result == (Glucose::lbool((uint8_t)0)))
        result = Solver::solve_();



    if (result == (Glucose::lbool((uint8_t)0)))
        extendModel();

    if (do_simp)

        for (int i = 0; i < extra_frozen.size(); i++)
            setFrozen(extra_frozen[i], false);


    return result;
}



bool SimpSolver::addClause_(vec<Lit>& ps)
{

    for (int i = 0; i < ps.size(); i++)
        assert(!isEliminated(var(ps[i])));

    int nclauses = clauses.size();

    if (use_rcheck && implied(ps))
        return true;

    if (!Solver::addClause_(ps))
        return false;

    if(!parsing && certifiedUNSAT) {
      for (int i = 0; i < ps.size(); i++)
        fprintf(certifiedOutput, "%i " , (var(ps[i]) + 1) * (-2 * sign(ps[i]) + 1) );
      fprintf(certifiedOutput, "0\n");
    }

    if (use_simplification && clauses.size() == nclauses + 1){
        CRef cr = clauses.last();
        const Clause& c = ca[cr];







        subsumption_queue.insert(cr);
        for (int i = 0; i < c.size(); i++){
            occurs[var(c[i])].push(cr);
            n_occ[toInt(c[i])]++;
            touched[var(c[i])] = 1;
            n_touched++;
            if (elim_heap.inHeap(var(c[i])))
                elim_heap.increase(var(c[i]));
        }
    }

    return true;
}



void SimpSolver::removeClause(CRef cr,bool inPurgatory)
{
    const Clause& c = ca[cr];

    if (use_simplification)
        for (int i = 0; i < c.size(); i++){
            n_occ[toInt(c[i])]--;
            updateElimHeap(var(c[i]));
            occurs.smudge(var(c[i]));
        }

    Solver::removeClause(cr,inPurgatory);
}


bool SimpSolver::strengthenClause(CRef cr, Lit l)
{
    Clause& c = ca[cr];
    assert(decisionLevel() == 0);
    assert(use_simplification);



    subsumption_queue.insert(cr);

    if (certifiedUNSAT) {
      for (int i = 0; i < c.size(); i++)
        if (c[i] != l) fprintf(certifiedOutput, "%i " , (var(c[i]) + 1) * (-2 * sign(c[i]) + 1) );
      fprintf(certifiedOutput, "0\n");
    }

    if (c.size() == 2){
        removeClause(cr);
        c.strengthen(l);
    }else{
        if (certifiedUNSAT) {
          fprintf(certifiedOutput, "d ");
          for (int i = 0; i < c.size(); i++)
            fprintf(certifiedOutput, "%i " , (var(c[i]) + 1) * (-2 * sign(c[i]) + 1) );
          fprintf(certifiedOutput, "0\n");
        }

        detachClause(cr, true);
        c.strengthen(l);
        attachClause(cr);
        remove(occurs[var(l)], cr);
        n_occ[toInt(l)]--;
        updateElimHeap(var(l));
    }

    return c.size() == 1 ? enqueue(c[0]) && propagate() == CRef_Undef : true;
}



bool SimpSolver::merge(const Clause& _ps, const Clause& _qs, Var v, vec<Lit>& out_clause)
{
    merges++;
    out_clause.clear();

    bool ps_smallest = _ps.size() < _qs.size();
    const Clause& ps = ps_smallest ? _qs : _ps;
    const Clause& qs = ps_smallest ? _ps : _qs;

    for (int i = 0; i < qs.size(); i++){
        if (var(qs[i]) != v){
            for (int j = 0; j < ps.size(); j++)
                if (var(ps[j]) == var(qs[i])) {
                    if (ps[j] == ~qs[i])
                        return false;
                    else
                        goto next;
                }
            out_clause.push(qs[i]);
        }
        next:;
    }

    for (int i = 0; i < ps.size(); i++)
        if (var(ps[i]) != v)
            out_clause.push(ps[i]);

    return true;
}



bool SimpSolver::merge(const Clause& _ps, const Clause& _qs, Var v, int& size)
{
    merges++;

    bool ps_smallest = _ps.size() < _qs.size();
    const Clause& ps = ps_smallest ? _qs : _ps;
    const Clause& qs = ps_smallest ? _ps : _qs;
    const Lit* __ps = (const Lit*)ps;
    const Lit* __qs = (const Lit*)qs;

    size = ps.size()-1;

    for (int i = 0; i < qs.size(); i++){
        if (var(__qs[i]) != v){
            for (int j = 0; j < ps.size(); j++)
                if (var(__ps[j]) == var(__qs[i])) {
                    if (__ps[j] == ~__qs[i])
                        return false;
                    else
                        goto next;
                }
            size++;
        }
        next:;
    }

    return true;
}


void SimpSolver::gatherTouchedClauses()
{
    if (n_touched == 0) return;

    int i,j;
    for (i = j = 0; i < subsumption_queue.size(); i++)
        if (ca[subsumption_queue[i]].mark() == 0)
            ca[subsumption_queue[i]].mark(2);

    for (i = 0; i < touched.size(); i++)
        if (touched[i]){
            const vec<CRef>& cs = occurs.lookup(i);
            for (j = 0; j < cs.size(); j++)
                if (ca[cs[j]].mark() == 0){
                    subsumption_queue.insert(cs[j]);
                    ca[cs[j]].mark(2);
                }
            touched[i] = 0;
        }

    for (i = 0; i < subsumption_queue.size(); i++)
        if (ca[subsumption_queue[i]].mark() == 2)
            ca[subsumption_queue[i]].mark(0);

    n_touched = 0;
}


bool SimpSolver::implied(const vec<Lit>& c)
{
    assert(decisionLevel() == 0);

    trail_lim.push(trail.size());
    for (int i = 0; i < c.size(); i++)
        if (value(c[i]) == (Glucose::lbool((uint8_t)0))){
            cancelUntil(0);
            return false;
        }else if (value(c[i]) != (Glucose::lbool((uint8_t)1))){
            assert(value(c[i]) == (Glucose::lbool((uint8_t)2)));
            uncheckedEnqueue(~c[i]);
        }

    bool result = propagate() != CRef_Undef;
    cancelUntil(0);
    return result;
}



bool SimpSolver::backwardSubsumptionCheck(bool)
{

    int subsumed = 0;
    int deleted_literals = 0;
    assert(decisionLevel() == 0);

    while (subsumption_queue.size() > 0 || bwdsub_assigns < trail.size()){


        if (asynch_interrupt){
            subsumption_queue.clear();
            bwdsub_assigns = trail.size();
            break; }


        if (subsumption_queue.size() == 0 && bwdsub_assigns < trail.size()){
            Lit l = trail[bwdsub_assigns++];
            ca[bwdsub_tmpunit][0] = l;
            ca[bwdsub_tmpunit].calcAbstraction();
            subsumption_queue.insert(bwdsub_tmpunit); }

        CRef cr = subsumption_queue.peek(); subsumption_queue.pop();
        Clause& c = ca[cr];

        if (c.mark()) continue;




        assert(c.size() > 1 || value(c[0]) == (Glucose::lbool((uint8_t)0)));


        Var best = var(c[0]);
        for (int i = 1; i < c.size(); i++)
            if (occurs[var(c[i])].size() < occurs[best].size())
                best = var(c[i]);


        vec<CRef>& _cs = occurs.lookup(best);
        CRef* cs = (CRef*)_cs;

        for (int j = 0; j < _cs.size(); j++)
            if (c.mark())
                break;
            else if (!ca[cs[j]].mark() && cs[j] != cr && (subsumption_lim == -1 || ca[cs[j]].size() < subsumption_lim)){
                Lit l = c.subsumes(ca[cs[j]]);

                if (l == lit_Undef)
                    subsumed++, removeClause(cs[j]);
                else if (l != lit_Error){
                    deleted_literals++;

                    if (!strengthenClause(cs[j], ~l))
                        return false;


                    if (var(l) == best)
                        j--;
                }
            }
    }

    return true;
}


bool SimpSolver::asymm(Var v, CRef cr)
{
    Clause& c = ca[cr];
    assert(decisionLevel() == 0);

    if (c.mark() || satisfied(c)) return true;

    trail_lim.push(trail.size());
    Lit l = lit_Undef;
    for (int i = 0; i < c.size(); i++)
        if (var(c[i]) != v && value(c[i]) != (Glucose::lbool((uint8_t)1)))
            uncheckedEnqueue(~c[i]);
        else
            l = c[i];

    if (propagate() != CRef_Undef){
        cancelUntil(0);
        asymm_lits++;
        if (!strengthenClause(cr, l))
            return false;
    }else
        cancelUntil(0);

    return true;
}


bool SimpSolver::asymmVar(Var v)
{
    assert(use_simplification);

    const vec<CRef>& cls = occurs.lookup(v);

    if (value(v) != (Glucose::lbool((uint8_t)2)) || cls.size() == 0)
        return true;

    for (int i = 0; i < cls.size(); i++)
        if (!asymm(v, cls[i]))
            return false;

    return backwardSubsumptionCheck();
}


static void mkElimClause(vec<uint32_t>& elimclauses, Lit x)
{
    elimclauses.push(toInt(x));
    elimclauses.push(1);
}


static void mkElimClause(vec<uint32_t>& elimclauses, Var v, Clause& c)
{
    int first = elimclauses.size();
    int v_pos = -1;



    for (int i = 0; i < c.size(); i++){
        elimclauses.push(toInt(c[i]));
        if (var(c[i]) == v)
            v_pos = i + first;
    }
    assert(v_pos != -1);



    uint32_t tmp = elimclauses[v_pos];
    elimclauses[v_pos] = elimclauses[first];
    elimclauses[first] = tmp;


    elimclauses.push(c.size());
}



bool SimpSolver::eliminateVar(Var v)
{
    assert(!frozen[v]);
    assert(!isEliminated(v));
    assert(value(v) == (Glucose::lbool((uint8_t)2)));



    const vec<CRef>& cls = occurs.lookup(v);
    vec<CRef> pos, neg;
    for (int i = 0; i < cls.size(); i++)
        (find(ca[cls[i]], mkLit(v)) ? pos : neg).push(cls[i]);




    int cnt = 0;
    int clause_size = 0;

    for (int i = 0; i < pos.size(); i++)
        for (int j = 0; j < neg.size(); j++)
            if (merge(ca[pos[i]], ca[neg[j]], v, clause_size) &&
                (++cnt > cls.size() + grow || (clause_lim != -1 && clause_size > clause_lim)))
                return true;


    eliminated[v] = true;
    setDecisionVar(v, false);
    eliminated_vars++;

    if (pos.size() > neg.size()){
        for (int i = 0; i < neg.size(); i++)
            mkElimClause(elimclauses, v, ca[neg[i]]);
        mkElimClause(elimclauses, mkLit(v));
    }else{
        for (int i = 0; i < pos.size(); i++)
            mkElimClause(elimclauses, v, ca[pos[i]]);
        mkElimClause(elimclauses, ~mkLit(v));
    }



    vec<Lit>& resolvent = add_tmp;
    for (int i = 0; i < pos.size(); i++)
        for (int j = 0; j < neg.size(); j++)
            if (merge(ca[pos[i]], ca[neg[j]], v, resolvent) && !addClause_(resolvent))
                return false;

    for (int i = 0; i < cls.size(); i++)
        removeClause(cls[i]);


    occurs[v].clear(true);


    if (watches[ mkLit(v)].size() == 0) watches[ mkLit(v)].clear(true);
    if (watches[~mkLit(v)].size() == 0) watches[~mkLit(v)].clear(true);

    return backwardSubsumptionCheck();
}


bool SimpSolver::substitute(Var v, Lit x)
{
    assert(!frozen[v]);
    assert(!isEliminated(v));
    assert(value(v) == (Glucose::lbool((uint8_t)2)));

    if (!ok) return false;

    eliminated[v] = true;
    setDecisionVar(v, false);
    const vec<CRef>& cls = occurs.lookup(v);

    vec<Lit>& subst_clause = add_tmp;
    for (int i = 0; i < cls.size(); i++){
        Clause& c = ca[cls[i]];

        subst_clause.clear();
        for (int j = 0; j < c.size(); j++){
            Lit p = c[j];
            subst_clause.push(var(p) == v ? x ^ sign(p) : p);
        }


        if (!addClause_(subst_clause))
            return ok = false;

       removeClause(cls[i]);

   }

    return true;
}


void SimpSolver::extendModel()
{
    int i, j;
    Lit x;

    if(model.size()==0) model.growTo(nVars());

    for (i = elimclauses.size()-1; i > 0; i -= j){
        for (j = elimclauses[i--]; j > 1; j--, i--)
            if (modelValue(toLit(elimclauses[i])) != (Glucose::lbool((uint8_t)1)))
                goto next;

        x = toLit(elimclauses[i]);
        model[var(x)] = lbool(!sign(x));
    next:;
    }
}


bool SimpSolver::eliminate(bool turn_off_elim)
{
    if (!simplify()) {
        ok = false;
        return false;
    }
    else if (!use_simplification)
        return true;




    int toPerform = clauses.size()<=4800000;

    if(!toPerform) {
      printf("c Too many clauses... No preprocessing\n");
    }

    while (toPerform && (n_touched > 0 || bwdsub_assigns < trail.size() || elim_heap.size() > 0)){

        gatherTouchedClauses();

        if ((subsumption_queue.size() > 0 || bwdsub_assigns < trail.size()) &&
            !backwardSubsumptionCheck(true)){
            ok = false; goto cleanup; }


        if (asynch_interrupt){
            assert(bwdsub_assigns == trail.size());
            assert(subsumption_queue.size() == 0);
            assert(n_touched == 0);
            elim_heap.clear();
            goto cleanup; }


        for (int cnt = 0; !elim_heap.empty(); cnt++){
            Var elim = elim_heap.removeMin();

            if (asynch_interrupt) break;

            if (isEliminated(elim) || value(elim) != (Glucose::lbool((uint8_t)2))) continue;




            if (use_asymm){

                bool was_frozen = frozen[elim];
                frozen[elim] = true;
                if (!asymmVar(elim)){
                    ok = false; goto cleanup; }
                frozen[elim] = was_frozen; }



            if (use_elim && value(elim) == (Glucose::lbool((uint8_t)2)) && !frozen[elim] && !eliminateVar(elim)){
                ok = false; goto cleanup; }

            checkGarbage(simp_garbage_frac);
        }

        assert(subsumption_queue.size() == 0);
    }
 cleanup:


    if (turn_off_elim){
        touched .clear(true);
        occurs .clear(true);
        n_occ .clear(true);
        elim_heap.clear(true);
        subsumption_queue.clear(true);

        use_simplification = false;
        remove_satisfied = true;
        ca.extra_clause_field = false;


        rebuildOrderHeap();
        garbageCollect();
    }else{

        cleanUpClauses();
        checkGarbage();
    }






    return ok;


}


void SimpSolver::cleanUpClauses()
{
    occurs.cleanAll();
    int i,j;
    for (i = j = 0; i < clauses.size(); i++)
        if (ca[clauses[i]].mark() == 0)
            clauses[j++] = clauses[i];
    clauses.shrink(i - j);
}






void SimpSolver::relocAll(ClauseAllocator& to)
{
    if (!use_simplification) return;



    for (int i = 0; i < nVars(); i++){
        vec<CRef>& cs = occurs[i];
        for (int j = 0; j < cs.size(); j++)
            ca.reloc(cs[j], to);
    }



    for (int i = 0; i < subsumption_queue.size(); i++)
        ca.reloc(subsumption_queue[i], to);



    ca.reloc(bwdsub_tmpunit, to);
}


void SimpSolver::garbageCollect()
{


    ClauseAllocator to(ca.size() - ca.wasted());

    cleanUpClauses();
    to.extra_clause_field = ca.extra_clause_field;
    relocAll(to);
    Solver::relocAll(to);



    to.moveTo(ca);
}


namespace aspino {

class SatSolver : public AbstractSolver, public Glucose::SimpSolver {
public:
    SatSolver() : inVars(0), status((Glucose::lbool((uint8_t)2))) { certifiedUNSAT = false; parsing = false;}
    virtual ~SatSolver() {}

    virtual void interrupt() { Glucose::SimpSolver::interrupt(); }

  virtual void parse(gzFile in);
    virtual bool simplify() { return Glucose::SimpSolver::simplify(); }
    virtual bool eliminate(bool turn_off_elim) { return Glucose::SimpSolver::eliminate(turn_off_elim); }
    virtual bool okay() const { return Glucose::SimpSolver::okay(); }
    virtual lbool solve();
    virtual lbool solve(int n);
    void printStatus() const;
    void copyModel();
    void printModel() const;

    inline void setAssumptions(vec<Lit>& lits) { lits.moveTo(assumptions); }
    using Glucose::SimpSolver::value;
    using Glucose::SimpSolver::model;

    inline void nInVars(int value) { inVars = value; }
    inline int nInVars() const { return inVars; }

    inline int nextToPropagateByUnit() const { return qhead; }
    inline Lit mainTrail(int idx) const { return trail[idx]; }
    inline int mainTrailLimit(int level) const { return trail_lim[level]; }

    virtual inline void newVar() { Glucose::SimpSolver::newVar(); }
    using Glucose::SimpSolver::nVars;
    using Glucose::SimpSolver::addClause;
    using Glucose::SimpSolver::setFrozen;
    using Glucose::SimpSolver::conflict;
    using Glucose::SimpSolver::level;

    inline void cancelUntil(int level) { Glucose::SimpSolver::cancelUntil(level); onCancel(); }

public:
    virtual inline CRef morePropagate() { return CRef_Undef; }
    virtual inline void onCancel() {}

    void quickSort(int left, int right);
    void solve_();
    lbool search(int nof_conflicts);
    static double luby(double y, int x);
    void learnClauseFromModel();

    int inVars;
    int inClauses;
    lbool status;
    bool sortAssumptions;

public:
    lbool enumerateByBlockingClauses(int n);
    lbool enumerateByAssumption(int n);
};

  Glucose::IntOption option_n("MAIN", "n", "Number of desired solutions. Non-positive integers are interpreted as unbounded.\n", 1, Glucose::IntRange(0, INT32_MAX));
void SatSolver::parse(gzFile in_) {
    Glucose::StreamBuffer in(in_);

    vec<Lit> lits;
    int vars = 0;
    int clauses = 0;
    int count = 0;
    for(;;) {
        skipWhitespace(in);
        if(*in == EOF) break;
        if(*in == 'p') {
            if(eagerMatch(in, "p cnf")) {
                vars = parseInt(in);
                clauses = parseInt(in);
            }
            else {
                cerr << "PARSE ERROR! Unexpected char: " << static_cast<char>(*in) << endl, exit(3);
            }
        }
        else if(*in == 'c')
            skipLine(in);
        else {
            count++;
            readClause(in, *this, lits);
            addClause(lits);
        }
    }
    if(nVars() != vars)
        cerr << "WARNING! DIMACS header mismatch: wrong number of variables." << endl;
    nInVars(min(vars, nVars()));
    if(count != clauses)
        cerr << "WARNING! DIMACS header mismatch: wrong number of clauses." << endl;
    inVars = nVars();

    if(option_n != 1) {
        for(int i = 0; i < nVars(); i++) setFrozen(i, true);
    }
}


}

namespace aspino {

template<class B>
int64_t parseLong(B& in);

class WeightConstraint {
    friend ostream& operator<<(ostream& out, const WeightConstraint& wc) { return out << wc.toString(); }
public:
    inline WeightConstraint() : bound(0) {}
    inline WeightConstraint(WeightConstraint& wc) : bound(wc.bound), loosable(wc.loosable) { wc.lits.moveTo(lits); wc.coeffs.moveTo(coeffs); }
    ~WeightConstraint() { lits.clear(); coeffs.clear(); trail.clear(); }

    inline void clear() { lits.clear(); coeffs.clear(); bound = 0; }
    string toString() const;

    inline int size() const { assert(lits.size() == coeffs.size()); return lits.size(); }
    inline void shrink_(int n) { lits.shrink_(n); coeffs.shrink_(n); }
    inline void pop() { lits.pop(); coeffs.pop(); }

    vec<Lit> lits;
    vec<int64_t> coeffs;
    int64_t bound;
    int64_t loosable;
    vec<int> trail;
    int first;
};

class CardinalityConstraint {
    friend ostream& operator<<(ostream& out, const CardinalityConstraint& cc) { return out << cc.toString(); }
public:
    inline CardinalityConstraint() : bound(0) {}
    inline CardinalityConstraint(CardinalityConstraint& cc) : bound(cc.bound), loosable(cc.loosable) { cc.lits.moveTo(lits); }
    ~CardinalityConstraint() { lits.clear(); trail.clear(); }

    inline void clear() { lits.clear(); bound = 0; }
    string toString() const;

    inline int size() const { return lits.size(); }
    inline void shrink_(int n) { lits.shrink_(n); }
    inline void pop() { lits.pop(); }

    vec<Lit> lits;
    int bound;
    int loosable;
    vec<Lit> trail;
};

class PseudoBooleanSolver : public SatSolver {
public:
    static const CRef CRef_MoreConflict;

    PseudoBooleanSolver() : moreConflictWC(NULL), moreConflictCC(NULL), nextToPropagate(0) {}
    virtual ~PseudoBooleanSolver();

    virtual void newVar();


  virtual void parse(gzFile in);
  bool readConstraint(Glucose::StreamBuffer& in, WeightConstraint& wc);

    bool addConstraint(WeightConstraint& wc);
    bool addEquality(WeightConstraint& wc);
    void attach(WeightConstraint& wc);

    bool addConstraint(CardinalityConstraint& wc);
    bool addEquality(CardinalityConstraint& wc);
    void attach(CardinalityConstraint& wc);

    void freeze();

public:
    virtual CRef morePropagate();
    CRef morePropagate(Lit lit);
    void restore(WeightConstraint& wc);
    void restore(CardinalityConstraint& wc);
    CRef checkConflict(Lit lit, WeightConstraint& wc, int pos);
    CRef checkConflict(Lit lit, CardinalityConstraint& wc);
    CRef checkInference(Lit lit, WeightConstraint& wc, int pos);
    CRef checkInference(Lit lit, CardinalityConstraint& wc);

    virtual bool moreReason(Lit lit, vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC);
    void _moreReasonCC(Lit lit, vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC);
    void _moreReasonWC(Lit lit, vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC);
    virtual bool moreReason(Lit lit);
    void _moreReasonCC(Lit lit);
    void _moreReasonWC(Lit lit);
    virtual bool moreConflict(vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC);
    void _moreConflictCC(vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC);
    void _moreConflictWC(vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC);
    virtual void onCancel();

    vec<CardinalityConstraint*> cconstraints;
    vec< vec<CardinalityConstraint*> > cpropagators[2];

    vec<WeightConstraint*> wconstraints;
    vec< vec<WeightConstraint*> > wpropagators[2];
    vec< vec<int> > positions[2];

    vec<WeightConstraint*> moreReasonWC;
    vec<CardinalityConstraint*> moreReasonCC;
    vec<int> moreReasonTrailSize;
    vec<Var> moreReasonVars;
    WeightConstraint* moreConflictWC;
    CardinalityConstraint* moreConflictCC;
    Lit moreConflictLit;

    vec<bool> propagated;
    int nextToPropagate;

public:
    static int64_t gcd(int64_t u, int64_t v);
};

bool PseudoBooleanSolver::readConstraint(Glucose::StreamBuffer& in, WeightConstraint& wc) {
    int var;
    bool ret = false;
    wc.clear();
    for (;;){
        if(*in == '>' || *in == '=') {
            if(*in == '=') ret = true;
            else {
                ++in;
                if(*in != '=') cerr << "PARSE ERROR! Unexpected char: " << static_cast<char>(*in) << endl, exit(3);
            }
            ++in;
            skipWhitespace(in);
            wc.bound = parseLong(in);
            skipWhitespace(in);
            if(*in != ';') cerr << "PARSE ERROR! Unexpected char: " << static_cast<char>(*in) << endl, exit(3);
            ++in;
            return ret;
        }

        wc.coeffs.push(parseLong(in));
        skipWhitespace(in);
        if(*in != 'x') cerr << "PARSE ERROR! Unexpected char: " << static_cast<char>(*in) << endl, exit(3);
        ++in;
        var = parseInt(in);
        if(var-- <= 0) cerr << "PARSE ERROR! Variable with non-positive index: x" << var+1 << endl, exit(3);
        while(var >= nVars()) newVar();
        wc.lits.push(mkLit(var));
        skipWhitespace(in);
    }
}

void PseudoBooleanSolver::parse(gzFile in_) {
    Glucose::StreamBuffer in(in_);

    WeightConstraint wc;
    for(;;) {
        skipWhitespace(in);
        if(*in == EOF) break;
        if(*in == '*')
            skipLine(in);
        else {
            if(readConstraint(in, wc))
                addEquality(wc);
            else
                addConstraint(wc);
            morePropagate();
        }
    }
    inVars = nVars();
    freeze();
}


}

namespace aspino {

  class MaxSatSolver : public PseudoBooleanSolver {
  public:
    MaxSatSolver();
    virtual ~MaxSatSolver();

    virtual void interrupt();


    virtual void parse(gzFile in);
    void addWeightedClause(vec<Lit>& lits, int64_t weight);

    virtual lbool solve();
    virtual lbool solve(int) { return solve(); }

    lbool solveWithUpperBound(int ub);

    void onTick();

  Lit newLit() { weights.push(0); return mkLit(Glucose::SimpSolver::newVar()); }

public:
    int64_t upperbound;
    int64_t lowerbound;

    vec<Lit> softLiterals;
    vec<int64_t> weights;
    vec<bool> satisfiableSoftLiterals;

    int lastSoftLiteral;

    uint64_t lastConflict;
    uint64_t lastPropagation;
    double lastCallCpuTime;
    double timeBudget;

    void quickSort(int left, int right);

    void sameSoftVar(Lit soft, int64_t weight);

    void removeSoftLiteralsAtLevelZero();
    bool tautology(Var var, CRef c1, CRef c2);
    void bce();
    void preprocess();

    void hardening();
    void setAssumptions(int64_t limit);
    int64_t computeNextLimit(int64_t limit) const;
    void solveCurrentLevel();
    void solve_();

    int64_t nextUp(int64_t curr) const;
    int64_t computeConflictWeight() const;

    void updateLowerBound(int64_t limit);
    void updateUpperBound();

    void progressionBinaryFind(int64_t limit);

    void trim();
    void progressionMinimize(int64_t limit);
    void biprogressionMinimize(int64_t limit);
    void binaryMinimize(int64_t limit);
    void progressionBinaryMinimize(int64_t limit);
    void minimize();

    void (MaxSatSolver::*corestrat)(int64_t);
    void corestrat_one(int64_t limit);
    void corestrat_one_2(int64_t limit);
    void corestrat_one_wc(int64_t limit);
    void corestrat_one_neg(int64_t limit);
    void corestrat_one_neg_wc(int64_t limit);
    void corestrat_one_pmres(int64_t limit);
    void corestrat_one_pmres_2(int64_t limit);
    void corestrat_pmres(int64_t limit);
    void corestrat_pmres_reverse(int64_t limit);
    void corestrat_pmres_split_conj(int64_t limit);
    void corestrat_pmreslog(int64_t limit);
    void corestrat_kdyn(int64_t limit);
    void corestrat_wone(int64_t limit);
    void corestrat_wone_gcd(int64_t limit);
    void corestrat_wone_remainder(int64_t limit);

    enum DisjunctCores {NO = 0, PRE, ALL};
    DisjunctCores disjcores;

    vec<vec<Lit>*> cores;
};



void MaxSatSolver::parse(gzFile in_) {
    Glucose::StreamBuffer in(in_);

    bool weighted = false;
    int64_t top = -1;
    int64_t weight = 1;

    vec<Lit> lits;
    int vars = 0;
    int count = 0;
    for(;;) {
        skipWhitespace(in);
        if(*in == EOF) break;
        if(*in == 'p') {
            ++in;
            if(*in != ' ') cerr << "PARSE ERROR! Unexpected char: " << static_cast<char>(*in) << endl, exit(3);
            ++in;
            if(*in == 'w') { weighted = true; ++in; }

            if(eagerMatch(in, "cnf")) {
                vars = parseInt(in);
                inClauses = parseInt(in);
                if(weighted && *in != '\n') top = parseLong(in);

                nInVars(vars);
                while(weights.size() < nInVars()) weights.push(0);
                while(nVars() < nInVars()) newVar();
            }
            else {
                cerr << "PARSE ERROR! Unexpected char: " << static_cast<char>(*in) << endl, exit(3);
            }
        }
        else if(*in == 'c')
            skipLine(in);
        else {
            count++;
            if(weighted) weight = parseLong(in);
            readClause(in, *this, lits);
            if(weight == top) addClause_(lits);
            else addWeightedClause(lits, weight);
        }
    }
    if(count != inClauses)
        cerr << "WARNING! DIMACS header mismatch: wrong number of clauses." << endl, exit(3);
}



}

Glucose::EnumOption option_mode("MAIN", "mode", "How to interpret input.\n", "asp|sat|maxsat|pmaxsat|pbs|qbf|circumscription|fairsat|ltl|tgds");

Glucose::BoolOption option_print_model("MAIN", "print-model", "Print model if found.", false);


Glucose::EnumOption option_sat_enumeration("SAT", "sat-enumeration", "Enumeration algorithm.\n", "assumptions|blocking-clauses");

namespace aspino {

lbool SatSolver::solve() {
    conflict.clear();
    cancelUntil(0);

    solves++;

    solve_();

    return status;
}

lbool SatSolver::solve(int n) {
    if(strcmp(option_sat_enumeration, "assumptions") == 0) return enumerateByAssumption(n);
    if(strcmp(option_sat_enumeration, "blocking-clauses") == 0) return enumerateByBlockingClauses(n);
    assert(0);
    return (Glucose::lbool((uint8_t)2));
}

lbool SatSolver::enumerateByBlockingClauses(int n) {
    conflict.clear();
    cancelUntil(0);

    solves++;

    lbool ret = (Glucose::lbool((uint8_t)1));
    int count = 0;
    for(;;) {
        solve_();
        if(status == (Glucose::lbool((uint8_t)1))) {
            if(conflict.size() == 0) ok = false;
            break;
        }
        assert(status == (Glucose::lbool((uint8_t)0)));
        if(++count == 1) printStatus();

        if(option_print_model) {
            cerr << "c Model " << count << endl;
            copyModel();
            printModel();
        }
        ret = (Glucose::lbool((uint8_t)0));
        if(--n == 0) break;
        if(decisionLevel() == 0) break;
        learnClauseFromModel();
    }
    if(ret == (Glucose::lbool((uint8_t)1))) printStatus();
    cerr << "c Models " << count << endl;
    return ret;
}

lbool SatSolver::enumerateByAssumption(int n) {
    cancelUntil(0);

    int nAss = assumptions.size();

    vec<Lit> branchingLits;
    vec<bool> branchingFlags;

    lbool ret = (Glucose::lbool((uint8_t)1));

    int count = 0;
    while(true) {
        assumptions.shrink_(assumptions.size() - nAss);
        assert(assumptions.size() == nAss);
        for(int i = 0; i < branchingLits.size(); i++) assumptions.push(branchingLits[i]);

        solve_();

        if(status == (Glucose::lbool((uint8_t)0))) {
            for(int i = assumptions.size(); i < decisionLevel(); i++) {
                branchingLits.push(mainTrail(mainTrailLimit(i)));
                branchingFlags.push(false);
            }

            if(++count == 1) printStatus();
            ret = (Glucose::lbool((uint8_t)0));
            if(option_print_model) {
                cerr << "c Model " << count << endl;
                copyModel();
                printModel();
            }
            if(--n == 0) break;
        }
        else if(status == (Glucose::lbool((uint8_t)1))) {

            Var v = conflict.size() > 0 ? var(conflict[0]) : -1;
            while(branchingLits.size() > 0 && v != var(branchingLits.last())) {
                branchingLits.pop();
                branchingFlags.pop();
            }
        }


        while(branchingLits.size() > 0 && branchingFlags.last()) {
            branchingLits.pop();
            branchingFlags.pop();
        }

        if(branchingLits.size() == 0) break;


        branchingLits[branchingLits.size()-1] = ~branchingLits[branchingLits.size()-1];
        branchingFlags[branchingFlags.size()-1] = true;

        cancelUntil(branchingLits.size()-1);
    }

    cerr << "c Models " << count << endl;

    return ret;
}


void SatSolver::quickSort(int left, int right) {
    int i = left, j = right;
    Lit tmp;
    assert((left + right) / 2 >= 0);
                                                                                                                                                 ;
    double pivot = activity[var(assumptions[(left + right) / 2])];


    while (i <= j) {
        while (activity[var(assumptions[i])] < pivot)
              i++;
        while (activity[var(assumptions[j])] > pivot)
              j--;
        if (i <= j) {
              tmp = assumptions[i];
              assumptions[i] = assumptions[j];
              assumptions[j] = tmp;
              i++;
              j--;
        }
    };


    if (left < j)
        quickSort(left, j);
    if (i < right)
        quickSort(i, right);
}

void SatSolver::solve_() {
    if(!ok) { status = (Glucose::lbool((uint8_t)1)); return; }

    status = (Glucose::lbool((uint8_t)2));


    int curr_restarts = 0;
    while(status == (Glucose::lbool((uint8_t)2))) {




        status = search(0);
        if(cpuTime() > satTL) throw TimeLimitException();
        if(!withinBudget()) break;
        curr_restarts++;

    }

    if(status == (Glucose::lbool((uint8_t)1)) && conflict.size() == 0)
        ok = false;
}


lbool SatSolver::search(int)
{
    assert(ok);
    int backtrack_level;
    int conflictC = 0;
    vec<Lit> learnt_clause,selectors;
    unsigned int nblevels,szWoutSelectors;
    bool blocked=false;
    starts++;
    for (;;){
        CRef confl;
        do {
            confl = propagate();
            if(confl != CRef_Undef) break;
            confl = morePropagate();
            if(confl != CRef_Undef) break;
        }while(nextToPropagateByUnit() < trail.size());

        if (confl != CRef_Undef){

   conflicts++; conflictC++;conflictsRestarts++;
   if(conflicts%5000==0 && var_decay<0.95)
            var_decay += 0.01;

   if (decisionLevel() == 0) {
     return (Glucose::lbool((uint8_t)1));

   }

   trailQueue.push(trail.size());

   if( conflictsRestarts>10000 && lbdQueue.isvalid() && trail.size()>R*trailQueue.getavg()) {
     lbdQueue.fastclear();
     nbstopsrestarts++;
     if(!blocked) {lastblockatrestart=starts;nbstopsrestartssame++;blocked=true;}
   }

            learnt_clause.clear();
     selectors.clear();
            analyze(confl, learnt_clause, selectors,backtrack_level,nblevels,szWoutSelectors);

     lbdQueue.push(nblevels);
     sumLBD += nblevels;


            cancelUntil(backtrack_level);
            if (learnt_clause.size() == 1){
       uncheckedEnqueue(learnt_clause[0]);nbUn++;
            }else{
                CRef cr = ca.alloc(learnt_clause, true);
  ca[cr].setLBD(nblevels);
  ca[cr].setSizeWithoutSelectors(szWoutSelectors);
  if(nblevels<=2) nbDL2++;
  if(ca[cr].size()==2) nbBin++;
                learnts.push(cr);
                attachClause(cr);

                claBumpActivity(ca[cr]);
                uncheckedEnqueue(learnt_clause[0], cr);
            }
            varDecayActivity();
            claDecayActivity();


        }else{

   if ( asynch_interrupt ||
       ( lbdQueue.isvalid() && ((lbdQueue.getavg()*K) > (sumLBD / conflictsRestarts)))) {
     lbdQueue.fastclear();
     progress_estimate = progressEstimate();
     int bt = 0;
     if(incremental) {
       bt = (decisionLevel()<assumptions.size()) ? decisionLevel() : assumptions.size();
     }
     cancelUntil(bt);
     return (Glucose::lbool((uint8_t)2)); }



   if (decisionLevel() == 0 && !simplify()) {
     return (Glucose::lbool((uint8_t)1));
   }

     if(static_cast<unsigned>(conflicts) >= curRestart* nbclausesbeforereduce)
       {

  assert(learnts.size()>0);
  curRestart = (conflicts/ nbclausesbeforereduce)+1;
  reduceDB();
  nbclausesbeforereduce += incReduceDB;
       }

            Lit next = lit_Undef;
            while (decisionLevel() < assumptions.size()){

                Lit p = assumptions[decisionLevel()];
                if (value(p) == (Glucose::lbool((uint8_t)0))){

                    newDecisionLevel();
                }else if (value(p) == (Glucose::lbool((uint8_t)1))){
                    analyzeFinal(~p, conflict);
                    return (Glucose::lbool((uint8_t)1));
                }else{
                    next = p;
                    break;
                }
            }

            if (next == lit_Undef){

                decisions++;
                next = pickBranchLit();

                if (next == lit_Undef){

    return (Glucose::lbool((uint8_t)0));
  }
            }


            newDecisionLevel();
            uncheckedEnqueue(next);
        }
    }
}

void SatSolver::printStatus() const {
    cerr << "s " << (status == (Glucose::lbool((uint8_t)0)) ? "SATISFIABLE" : status == (Glucose::lbool((uint8_t)1)) ? "UNSATISFIABLE" : "INDETERMINATE") << endl;
}

void SatSolver::copyModel() {

    model.growTo(nVars());
    for (int i = 0; i < nVars(); i++) model[i] = value(i);
    Glucose::SimpSolver::extendModel();
}

void SatSolver::printModel() const {
    assert(model.size() >= nInVars());




}

void SatSolver::learnClauseFromModel() {
    vec<Lit> lits;
    lits.growTo(trail_lim.size());
    for(int i = 0; i < lits.size(); i++) {
        Lit lit = trail[trail_lim[i]];
        assert(reason(var(lit)) == CRef_Undef);
        assert(level(var(lit)) > 0);
        lits[lits.size() - level(var(lit))] = ~lit;
    }
                                                       ;
    cancelUntil(decisionLevel()-1);
    if (lits.size() == 1)
        uncheckedEnqueue(lits[0]);
    else {
        CRef cr = ca.alloc(lits, true);
        clauses.push(cr);
        attachClause(cr);

        uncheckedEnqueue(lits[0], cr);
    }
}

double SatSolver::luby(double y, int x) {
    int size, seq;
    for (size = 1, seq = 0; size < x+1; seq++, size = 2*size+1);

    while (size-1 != x){
        size = (size-1)>>1;
        seq--;
        x = x % size;
    }

    return pow(y, seq);
}

}


namespace aspino {

const CRef PseudoBooleanSolver::CRef_MoreConflict = CRef_Undef - 1;


template<class B>
int64_t parseLong(B& in) {
    int64_t val = 0;
    bool neg = false;
    skipWhitespace(in);
    if (*in == '-') neg = true, ++in;
    else if (*in == '+') ++in;
    if (*in < '0' || *in > '9') fprintf(stderr, "PARSE ERROR! Unexpected char: %c\n", *in), exit(3);
    while (*in >= '0' && *in <= '9')
        val = val*10 + (*in - '0'),
        ++in;
    return neg ? -val : val; }







string WeightConstraint::toString() const {
    stringstream ss;
    if(lits.size() == 0)
        ss << "0 ";
    else {
        for(int i = 0; i < lits.size(); i++)
            ss << (coeffs[i] >= 0 ? "+" : "") << coeffs[i] << "x" << lits[i] << " ";
    }
    ss << ">= " << bound;
    return ss.str();
}

string CardinalityConstraint::toString() const {
    stringstream ss;
    if(lits.size() == 0)
        ss << "0 ";
    else {
        for(int i = 0; i < lits.size(); i++)
            ss << "+x" << lits[i] << " ";
    }
    ss << ">= " << bound;
    return ss.str();
}

PseudoBooleanSolver::~PseudoBooleanSolver() {
    cpropagators[0].clear();
    cpropagators[1].clear();
    wpropagators[0].clear();
    wpropagators[1].clear();

    for(int i = 0; i < cconstraints.size(); i++) delete cconstraints[i];
    cconstraints.clear();

    for(int i = 0; i < wconstraints.size(); i++) delete wconstraints[i];
    wconstraints.clear();
}

void PseudoBooleanSolver::freeze() {
    for(int i = 0; i < cconstraints.size(); i++)
        for(int j = 0; j < cconstraints[i]->size(); j++)
            setFrozen(var(cconstraints[i]->lits[j]), true);
    for(int i = 0; i < wconstraints.size(); i++)
        for(int j = 0; j < wconstraints[i]->size(); j++)
            setFrozen(var(wconstraints[i]->lits[j]), true);
}

bool PseudoBooleanSolver::addConstraint(WeightConstraint& wc) {
    assert(decisionLevel() == 0);
    assert(wc.size() > 0);

                                                  ;


    int j = 0;
    for(int i = 0; i < wc.size(); i++) {
        if(wc.coeffs[i] == 0) continue;
        if(value(wc.lits[i]) != (Glucose::lbool((uint8_t)2))) {
            assert(level(var(wc.lits[i])) == 0);
            if(value(wc.lits[i]) == (Glucose::lbool((uint8_t)0)))
                wc.bound -= wc.coeffs[i];
            continue;
        }
        if(wc.coeffs[i] < 0) {
            wc.coeffs[j] = -wc.coeffs[i];
            wc.bound += wc.coeffs[j];
            wc.lits[j++] = ~wc.lits[i];
        }
        else {
            wc.coeffs[j] = wc.coeffs[i];
            wc.lits[j++] = wc.lits[i];
        }
    }
    wc.shrink_(wc.size()-j);
                                                                                         ;


    if(wc.bound <= 0) {
                                                               ;
        return true;
    }


    int64_t d = wc.bound;
    int64_t sum = 0;
    j = 0;
    for(int i = 0; i < wc.size(); i++) {
        wc.coeffs[j] = wc.coeffs[i] > wc.bound ? wc.bound : wc.coeffs[i];
        d = gcd(wc.coeffs[j], d);
        sum += wc.coeffs[j];
        wc.lits[j++] = wc.lits[i];
    }
                                                     ;


    if(sum < wc.bound) {
                                                         ;
        return ok = false;
    }

    if(d > 1) {
        wc.bound /= d;
        sum /= d;
        for(int i = 0; i < wc.size(); i++)
            wc.coeffs[i] /= d;
    }
                                                                   ;


    int n = wc.size();
    while(n > 0) {
       int newn = 0;
       for(int i = 1; i < n; i++) {
          if(wc.coeffs[i-1] > wc.coeffs[i]) {
             int64_t ctmp = wc.coeffs[i-1];
             wc.coeffs[i-1] = wc.coeffs[i];
             wc.coeffs[i] = ctmp;

             Lit ltmp = wc.lits[i-1];
             wc.lits[i-1] = wc.lits[i];
             wc.lits[i] = ltmp;

             newn = i;
          }
       }
       n = newn;
    }
                                                  ;


    wc.loosable = sum - wc.bound;
    assert(wc.loosable >= 0);
    while(wc.size() > 0) {
        if(wc.coeffs.last() <= wc.loosable) break;
                                                                                       ;
        if(!addClause(wc.lits.last())) return ok = false;
        wc.bound -= wc.coeffs.last();
        wc.pop();
    }
    if(wc.bound <= 0) {
                                                               ;
        return true;
    }
    assert(wc.bound > 0);

    assert(wc.size() > 1);
                                                              ;


    if(wc.bound <= wc.coeffs[0]) {
                                                        ;
        return addClause(wc.lits);
    }

    attach(wc);

    return true;
}

bool PseudoBooleanSolver::addEquality(WeightConstraint& wc) {
    WeightConstraint wc2;
    wc2.lits.growTo(wc.size());
    wc2.coeffs.growTo(wc.size());
    for(int i = 0; i < wc.size(); i++) {
        wc2.lits[i] = wc.lits[i];
        wc2.coeffs[i] = -wc.coeffs[i];
    }
    wc2.bound = -wc.bound;

    return addConstraint(wc) && addConstraint(wc2);
}

void PseudoBooleanSolver::attach(WeightConstraint& wc) {
                                              ;

    WeightConstraint* ref = new WeightConstraint(wc);
    ref->first = ref->size() - 1;
    wconstraints.push(ref);

    for(int i = 0; i < ref->size(); i++) {
        wpropagators[1-sign(ref->lits[i])][var(ref->lits[i])].push(ref);
        positions[1-sign(ref->lits[i])][var(ref->lits[i])].push(i);
    }
}

bool PseudoBooleanSolver::addConstraint(CardinalityConstraint& cc) {
    assert(decisionLevel() == 0);
    assert(cc.size() > 0);

                                                  ;


    int j = 0;
    for(int i = 0; i < cc.size(); i++) {
        if(value(cc.lits[i]) != (Glucose::lbool((uint8_t)2))) {
            assert(level(var(cc.lits[i])) == 0);
            if(value(cc.lits[i]) == (Glucose::lbool((uint8_t)0)))
                cc.bound--;
            continue;
        }
        cc.lits[j++] = cc.lits[i];
    }
    cc.shrink_(cc.size()-j);
                                                                   ;


    if(cc.bound <= 0) {
                                                               ;
        return true;
    }


    if(cc.size() < cc.bound) {
                                                         ;
        return ok = false;
    }


    cc.loosable = cc.size() - cc.bound;
    assert(cc.loosable >= 0);
    while(cc.size() > 0) {
        if(1 <= cc.loosable) break;
                                                                                       ;
        if(!addClause(cc.lits.last())) return ok = false;
        cc.bound--;
        cc.pop();
    }
    if(cc.bound <= 0) {
                                                               ;
        return true;
    }
    assert(cc.bound > 0);

    assert(cc.size() > 1);
                                                              ;


    if(cc.bound <= 1) {
                                                        ;
        return addClause(cc.lits);
    }

    attach(cc);

    return true;
}

bool PseudoBooleanSolver::addEquality(CardinalityConstraint& cc) {
    CardinalityConstraint cc2;
    cc2.lits.growTo(cc.size());
    for(int i = 0; i < cc.size(); i++) cc2.lits[i] = ~cc.lits[i];
    cc2.bound = -cc.bound + cc2.size();
    return addConstraint(cc) && addConstraint(cc2);
}

void PseudoBooleanSolver::attach(CardinalityConstraint& cc) {
                                              ;

    CardinalityConstraint* ref = new CardinalityConstraint(cc);
    cconstraints.push(ref);

    for(int i = 0; i < ref->size(); i++) cpropagators[1-sign(ref->lits[i])][var(ref->lits[i])].push(ref);
}

CRef PseudoBooleanSolver::morePropagate() {
    while(nextToPropagate < nextToPropagateByUnit()) {
        Lit lit = mainTrail(nextToPropagate++);
        CRef ret = morePropagate(lit);
        if(ret != CRef_Undef) return ret;
    }

    return CRef_Undef;
}

CRef PseudoBooleanSolver::morePropagate(Lit lit) {
                                                                   ;

    vec<CardinalityConstraint*>& c = cpropagators[sign(lit)][var(lit)];
    for(int i = 0; i < c.size(); i++) {
        CardinalityConstraint& cc = *c[i];

                                          ;
        restore(cc);
        CRef ret = checkConflict(lit, cc);
        if(ret != CRef_Undef) return ret;
        ret = checkInference(lit, cc);
        if(ret != CRef_Undef) return ret;
    }

    vec<WeightConstraint*>& p = wpropagators[sign(lit)][var(lit)];
    vec<int>& ppos = positions[sign(lit)][var(lit)];
    for(int i = 0; i < p.size(); i++) {
        WeightConstraint& wc = *p[i];
        int pos = ppos[i];

                                          ;
        restore(wc);
        CRef ret = checkConflict(lit, wc, pos);
        if(ret != CRef_Undef) return ret;
        ret = checkInference(lit, wc, pos);
        if(ret != CRef_Undef) return ret;
    }

    propagated[var(lit)] = true;
    return CRef_Undef;
}

void PseudoBooleanSolver::restore(WeightConstraint& wc) {
                                                                                                                                              ;
    for(;;) {
        if(wc.trail.size() == 0) break;
        int idx = wc.trail.last();
        if(propagated[var(wc.lits[idx])] && value(wc.lits[idx]) == (Glucose::lbool((uint8_t)1))) break;
                                                                                                                                                                ;
        wc.trail.pop();
        wc.loosable += wc.coeffs[idx];
        if(wc.first < idx) {
                                                                             ;
            wc.first = idx;
        }
    }
}

void PseudoBooleanSolver::restore(CardinalityConstraint& cc) {
                                                                                                                   ;
    for(;;) {
        if(cc.trail.size() == 0) break;
        Lit last = cc.trail.last();
        if(propagated[var(last)] && value(last) == (Glucose::lbool((uint8_t)1))) break;
                                                                                                                                           ;
        cc.trail.pop();
        cc.loosable++;
    }
}

CRef PseudoBooleanSolver::checkConflict(Lit lit, WeightConstraint& wc, int pos) {
    if(wc.loosable < wc.coeffs[pos]) {
                                           ;
        moreConflictWC = &wc;
        moreConflictLit = ~lit;
        return CRef_MoreConflict;
    }
    return CRef_Undef;
}

CRef PseudoBooleanSolver::checkConflict(Lit lit, CardinalityConstraint& cc) {
    if(cc.loosable < 1) {
                                           ;
        moreConflictCC = &cc;
        moreConflictLit = ~lit;
        return CRef_MoreConflict;
    }
    return CRef_Undef;
}

CRef PseudoBooleanSolver::checkInference(Lit lit, WeightConstraint& wc, int pos) {
                                                                                                                                                                        ;
    wc.trail.push(pos);
    wc.loosable -= wc.coeffs[pos];
    for(int j = wc.first; j >= 0; j--) {
        if(wc.coeffs[j] <= wc.loosable) break;
        Lit wlit = wc.lits[j];
        if(value(wlit) == (Glucose::lbool((uint8_t)0))) continue;
        if(value(wlit) == (Glucose::lbool((uint8_t)2))) {
                                                                                            ;
            assert(moreReasonWC[var(wlit)] == NULL);
            moreReasonWC[var(wlit)] = &wc;
            moreReasonTrailSize[var(wlit)] = wc.trail.size();
            moreReasonVars.push(var(wlit));
            uncheckedEnqueue(wlit);
        }
        else if(wlit != ~lit && !propagated[var(wlit)]) {
                                                                         ;
            moreConflictWC = &wc;
            moreConflictLit = wlit;
            return CRef_MoreConflict;
        }
    }
    while(value(wc.lits[wc.first]) == (Glucose::lbool((uint8_t)1))) if(--wc.first < 0) break;

    return CRef_Undef;
}

CRef PseudoBooleanSolver::checkInference(Lit lit, CardinalityConstraint& cc) {
                                                                                                                                                   ;
    cc.trail.push(~lit);
    cc.loosable--;
    assert(cc.loosable >= 0);

    if(cc.loosable == 0) {
        for(int i = 0; i < cc.size(); i++) {
            Lit clit = cc.lits[i];
            if(value(clit) == (Glucose::lbool((uint8_t)0))) continue;
            if(value(clit) == (Glucose::lbool((uint8_t)2))) {
                                                                                                ;
                assert(moreReasonCC[var(clit)] == NULL);
                moreReasonCC[var(clit)] = &cc;
                moreReasonTrailSize[var(clit)] = cc.trail.size();
                moreReasonVars.push(var(clit));
                uncheckedEnqueue(clit);
            }
            else if(clit != ~lit && !propagated[var(clit)]) {
                                                                             ;
                moreConflictCC = &cc;
                moreConflictLit = clit;
                return CRef_MoreConflict;
            }
        }
    }

    return CRef_Undef;
}

void PseudoBooleanSolver::newVar() {
    SatSolver::newVar();
    cpropagators[0].push();
    cpropagators[1].push();
    wpropagators[0].push();
    wpropagators[1].push();
    positions[0].push();
    positions[1].push();
    moreReasonWC.push(NULL);
    moreReasonCC.push(NULL);
    moreReasonTrailSize.push();
    propagated.push(false);
}

bool PseudoBooleanSolver::moreReason(Lit lit, vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC) {
    if(moreReasonCC[var(lit)] != NULL) { _moreReasonCC(lit, out_learnt, selectors, pathC); return true; }
    if(moreReasonWC[var(lit)] != NULL) { _moreReasonWC(lit, out_learnt, selectors, pathC); return true; }
    return false;
}

void PseudoBooleanSolver::_moreReasonCC(Lit lit, vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC) {
    assert(decisionLevel() != 0);
    assert(reason(var(lit)) == CRef_Undef);
    CardinalityConstraint& cc = *moreReasonCC[var(lit)];
    int trailSize = moreReasonTrailSize[var(lit)];
    assert(trailSize >= 0);
    assert(trailSize <= cc.trail.size());
    while(--trailSize >= 0) {
        Lit q = cc.trail[trailSize];
        assert(value(q) == (Glucose::lbool((uint8_t)1)));
        assert(level(var(q)) <= level(var(lit)));

        if(seen[var(q)]) continue;
        if(level(var(q)) == 0) continue;

        if(!isSelector(var(q)))
            varBumpActivity(var(q));

        seen[var(q)] = 1;

        if(level(var(q)) >= decisionLevel()) {
            pathC++;

            if(!isSelector(var(q)) && (reason(var(q)) != CRef_Undef) && ca[reason(var(q))].learnt())
                lastDecisionLevel.push(q);
        }
        else {
            if(isSelector(var(q))) {
                assert(value(q) == (Glucose::lbool((uint8_t)1)));
                selectors.push(q);
            }
            else
                out_learnt.push(q);
        }
    }
}

void PseudoBooleanSolver::_moreReasonWC(Lit lit, vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC) {
    assert(decisionLevel() != 0);
    assert(reason(var(lit)) == CRef_Undef);
    WeightConstraint& wc = *moreReasonWC[var(lit)];
    int trailSize = moreReasonTrailSize[var(lit)];
    assert(trailSize >= 0);
    assert(trailSize <= wc.trail.size());
    while(--trailSize >= 0) {
        Lit q = wc.lits[wc.trail[trailSize]];
        assert(value(q) == (Glucose::lbool((uint8_t)1)));
        assert(level(var(q)) <= level(var(lit)));

        if(seen[var(q)]) continue;
        if(level(var(q)) == 0) continue;

        if(!isSelector(var(q)))
            varBumpActivity(var(q));

        seen[var(q)] = 1;

        if(level(var(q)) >= decisionLevel()) {
            pathC++;

            if(!isSelector(var(q)) && (reason(var(q)) != CRef_Undef) && ca[reason(var(q))].learnt())
                lastDecisionLevel.push(q);
        }
        else {
            if(isSelector(var(q))) {
                assert(value(q) == (Glucose::lbool((uint8_t)1)));
                selectors.push(q);
            }
            else
                out_learnt.push(q);
        }
    }
}

bool PseudoBooleanSolver::moreReason(Lit lit) {
    if(moreReasonCC[var(lit)] != NULL) { _moreReasonCC(lit); return true; }
    if(moreReasonWC[var(lit)] != NULL) { _moreReasonWC(lit); return true; }
    return false;
}

void PseudoBooleanSolver::_moreReasonCC(Lit lit) {
    assert(decisionLevel() != 0);
    assert(reason(var(lit)) == CRef_Undef);
    CardinalityConstraint& cc = *moreReasonCC[var(lit)];
    int trailSize = moreReasonTrailSize[var(lit)];
    assert(trailSize >= 0);
    assert(trailSize <= cc.trail.size());
    while(--trailSize >= 0) {
        Lit l = cc.trail[trailSize];
        assert(value(l) == (Glucose::lbool((uint8_t)1)));
        assert(level(var(l)) <= level(var(lit)));
        if(level(var(l)) == 0) continue;
        seen[var(l)] = 1;
    }
}

void PseudoBooleanSolver::_moreReasonWC(Lit lit) {
    assert(decisionLevel() != 0);
    assert(reason(var(lit)) == CRef_Undef);
    WeightConstraint& wc = *moreReasonWC[var(lit)];
    int trailSize = moreReasonTrailSize[var(lit)];
    assert(trailSize >= 0);
    assert(trailSize <= wc.trail.size());
    while(--trailSize >= 0) {
        Lit l = wc.lits[wc.trail[trailSize]];
        assert(value(l) == (Glucose::lbool((uint8_t)1)));
        assert(level(var(l)) <= level(var(lit)));
        if(level(var(l)) == 0) continue;
        seen[var(l)] = 1;
    }
}

bool PseudoBooleanSolver::moreConflict(vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC) {
    if(moreConflictCC != NULL) { _moreConflictCC(out_learnt, selectors, pathC); return true; }
    if(moreConflictWC != NULL) { _moreConflictWC(out_learnt, selectors, pathC); return true; }
    return false;
}

void PseudoBooleanSolver::_moreConflictCC(vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC) {
    assert(decisionLevel() != 0);

    if(!seen[var(moreConflictLit)] && level(var(moreConflictLit)) > 0) {
        if(!isSelector(var(moreConflictLit))) varBumpActivity(var(moreConflictLit));
        seen[var(moreConflictLit)] = 1;
        assert(level(var(moreConflictLit)) == decisionLevel());
        pathC++;

        if(!isSelector(var(moreConflictLit)) && (reason(var(moreConflictLit)) != CRef_Undef) && ca[reason(var(moreConflictLit))].learnt())
            lastDecisionLevel.push(moreConflictLit);
    }

    CardinalityConstraint& cc = *moreConflictCC;
    moreConflictCC = NULL;
    int trailSize = cc.trail.size();
    assert(trailSize >= 0);
    while(--trailSize >= 0) {
        Lit q = cc.trail[trailSize];
        assert(value(q) == (Glucose::lbool((uint8_t)1)));

        if(seen[var(q)]) continue;
        if(level(var(q)) == 0) continue;

        if(!isSelector(var(q)))
            varBumpActivity(var(q));

        seen[var(q)] = 1;

        if(level(var(q)) >= decisionLevel()) {
            pathC++;

            if(!isSelector(var(q)) && (reason(var(q)) != CRef_Undef) && ca[reason(var(q))].learnt())
                lastDecisionLevel.push(q);
        }
        else {
            if(isSelector(var(q))) {
                assert(value(q) == (Glucose::lbool((uint8_t)1)));
                selectors.push(q);
            }
            else
                out_learnt.push(q);
        }
    }
}

void PseudoBooleanSolver::_moreConflictWC(vec<Lit>& out_learnt, vec<Lit>& selectors, int& pathC) {
    assert(decisionLevel() != 0);

    if(!seen[var(moreConflictLit)] && level(var(moreConflictLit)) > 0) {
        if(!isSelector(var(moreConflictLit))) varBumpActivity(var(moreConflictLit));
        seen[var(moreConflictLit)] = 1;
        assert(level(var(moreConflictLit)) == decisionLevel());
        pathC++;

        if(!isSelector(var(moreConflictLit)) && (reason(var(moreConflictLit)) != CRef_Undef) && ca[reason(var(moreConflictLit))].learnt())
            lastDecisionLevel.push(moreConflictLit);
    }

    WeightConstraint& wc = *moreConflictWC;
    moreConflictWC = NULL;
    int trailSize = wc.trail.size();
    assert(trailSize >= 0);
    while(--trailSize >= 0) {
        Lit q = wc.lits[wc.trail[trailSize]];
        assert(value(q) == (Glucose::lbool((uint8_t)1)));

        if(seen[var(q)]) continue;
        if(level(var(q)) == 0) continue;

        if(!isSelector(var(q)))
            varBumpActivity(var(q));

        seen[var(q)] = 1;

        if(level(var(q)) >= decisionLevel()) {
            pathC++;

            if(!isSelector(var(q)) && (reason(var(q)) != CRef_Undef) && ca[reason(var(q))].learnt())
                lastDecisionLevel.push(q);
        }
        else {
            if(isSelector(var(q))) {
                assert(value(q) == (Glucose::lbool((uint8_t)1)));
                selectors.push(q);
            }
            else
                out_learnt.push(q);
        }
    }
}

void PseudoBooleanSolver::onCancel() {
                                                           ;
    while(nextToPropagate > nextToPropagateByUnit()) {
        int v = var(mainTrail(--nextToPropagate));
        propagated[v] = false;
    }

    while(moreReasonVars.size() > 0) {
        Var v = moreReasonVars.last();
        if(level(v) <= decisionLevel()) break;
        moreReasonVars.pop();
        moreReasonWC[v] = NULL;
        moreReasonCC[v] = NULL;
        moreReasonTrailSize[v] = -1;
    }
}

int64_t PseudoBooleanSolver::gcd(int64_t a, int64_t b) {
    assert(a > 0);
    assert(b > 0);

    if(a <= b)
        b = b % a;

    int64_t tmp;
    while(b > 0) {
        tmp = a;
        a = b;
        b = tmp % b;
    }
    return a;
}

}


namespace aspino {

  ostream& operator<<(ostream& o, lbool l) {
    if(l == (Glucose::lbool((uint8_t)0))) return o << "T";
    if(l == (Glucose::lbool((uint8_t)1))) return o << "F";
    if(l == (Glucose::lbool((uint8_t)2))) return o << "U";
    assert(0);
    return o;
  }

  ostream& operator<<(ostream& o, const Lit& l) {
    return o << (sign(l) ? "~" : "") << (var(l)+1);
  }

  ostream& operator<<(ostream& o, const Clause& c) {
    o << "[ ";
    for(int i = 0; i < c.size(); i++)
      o << c[i] << " ";
    return o << "]";
  }

}

using Glucose::Map;

Glucose::EnumOption option_maxsat_strat("MAXSAT", "maxsat-strat", "Set optimization strategy.", "one|one-2|one-neg|one-wc|one-neg-wc|one-pmres|one-pmres-2|pmres|pmres-reverse|pmres-log|pmres-split-conj|kdyn|wone|wone-gcd|wone-remainder");
Glucose::EnumOption option_maxsat_disjcores("MAXSAT", "maxsat-disjcores", "Set disjunct unsatisfiable cores policy.", "no|pre|all", 1);

Glucose::BoolOption option_maxsat_printmodel("MAXSAT", "maxsat-print-model", "Print optimal model if found.", true);

Glucose::IntOption option_maxsat_tag("MAXSAT", "maxsat-tag", "Parameter for maxsat-strat.", 16, Glucose::IntRange(2, INT32_MAX));

namespace aspino {
MaxSatSolver::MaxSatSolver() : lowerbound(0), lastConflict(0), lastPropagation(0), lastCallCpuTime(0.0) {
    if(strcmp(option_maxsat_strat, "one") == 0) corestrat = &MaxSatSolver::corestrat_one;
    else if(strcmp(option_maxsat_strat, "one-2") == 0) corestrat = &MaxSatSolver::corestrat_one_2;
    else if(strcmp(option_maxsat_strat, "one-neg") == 0) corestrat = &MaxSatSolver::corestrat_one_neg;
    else if(strcmp(option_maxsat_strat, "one-wc") == 0) corestrat = &MaxSatSolver::corestrat_one_wc;
    else if(strcmp(option_maxsat_strat, "one-neg-wc") == 0) corestrat = &MaxSatSolver::corestrat_one_neg_wc;
    else if(strcmp(option_maxsat_strat, "one-pmres") == 0) corestrat = &MaxSatSolver::corestrat_one_pmres;
    else if(strcmp(option_maxsat_strat, "one-pmres-2") == 0) corestrat = &MaxSatSolver::corestrat_one_pmres_2;
    else if(strcmp(option_maxsat_strat, "pmres") == 0) corestrat = &MaxSatSolver::corestrat_pmres;
    else if(strcmp(option_maxsat_strat, "pmres-reverse") == 0) corestrat = &MaxSatSolver::corestrat_pmres_reverse;
    else if(strcmp(option_maxsat_strat, "pmres-split-conj") == 0) corestrat = &MaxSatSolver::corestrat_pmres_split_conj;
    else if(strcmp(option_maxsat_strat, "pmres-log") == 0) corestrat = &MaxSatSolver::corestrat_pmreslog;
    else if(strcmp(option_maxsat_strat, "kdyn") == 0) corestrat = &MaxSatSolver::corestrat_kdyn;
    else if(strcmp(option_maxsat_strat, "wone") == 0) corestrat = &MaxSatSolver::corestrat_wone;
    else if(strcmp(option_maxsat_strat, "wone-gcd") == 0) corestrat = &MaxSatSolver::corestrat_wone_gcd;
    else if(strcmp(option_maxsat_strat, "wone-remainder") == 0) corestrat = &MaxSatSolver::corestrat_wone_remainder;
    else assert(0);

    if(strcmp(option_maxsat_disjcores, "no") == 0) disjcores = NO;
    else if(strcmp(option_maxsat_disjcores, "pre") == 0) disjcores = PRE;
    else if(strcmp(option_maxsat_disjcores, "all") == 0) disjcores = ALL;
    else assert(0);

    setIncrementalMode();
}

MaxSatSolver::~MaxSatSolver() {
}

void MaxSatSolver::interrupt() {
    cerr << "s UNKNOWN" << endl;
    if(upperbound != INT64_MAX && option_maxsat_printmodel) printModel();
    assert(0);
}

void MaxSatSolver::sameSoftVar(Lit soft, int64_t weight) {
    assert(weights[var(soft)] != 0);
    assert(decisionLevel() == 0);
    int pos = 0;
    for(int i = 0; i < softLiterals.size(); i++, pos++) if(var(softLiterals[i]) == var(soft)) break;
    assert(pos < softLiterals.size());

    if(softLiterals[pos] == soft) {
        weights[var(soft)] += weight;
        return;
    }

    if(weights[var(soft)] == weight) {
        updateLowerBound(weight);
        setFrozen(var(soft), false);
        softLiterals[pos] = softLiterals[softLiterals.size()-1];
        softLiterals.shrink_(1);
        weights[var(soft)] = 0;
    }
    else if(weights[var(soft)] < weight) {
        updateLowerBound(weights[var(soft)]);
        softLiterals[pos] = soft;
        weights[var(soft)] = weight - weights[var(soft)];
    }
    else {
        assert(weights[var(soft)] > weight);
        updateLowerBound(weight);
        weights[var(soft)] -= weight;
    }
}

void MaxSatSolver::addWeightedClause(vec<Lit>& lits, int64_t weight) {
    if(weight == 0) return;

    Lit soft;
    if(lits.size() == 1)
        soft = lits[0];
    else {
        newVar();
        weights.push(0);
        soft = mkLit(nVars()-1);
        lits.push(~soft);
        addClause_(lits);
    }

    assert(weights.size() == nVars());
    if(weights[var(soft)] != 0) {
        sameSoftVar(soft, weight);
        return;
    }

    softLiterals.push(soft);
                                             ;
    weights[var(soft)] = weight;
    setFrozen(var(soft), true);
}

void MaxSatSolver::hardening() {
    cancelUntil(0);
    int j = 0;
    for(int i = 0; i < softLiterals.size(); i++) {
        int64_t w = weights[var(softLiterals[i])];
        if(w + lowerbound >= upperbound) {
            addClause(softLiterals[i]);
                                                                                       ;
            weights[var(softLiterals[i])] = 0;
            continue;
        }
        softLiterals[j++] = softLiterals[i];
    }
    softLiterals.shrink_(softLiterals.size()-j);
}

void MaxSatSolver::setAssumptions(int64_t limit) {
    assumptions.clear();
    cancelUntil(0);
    int j = 0;
    for(int i = 0; i < softLiterals.size(); i++) {
        int64_t w = weights[var(softLiterals[i])];
        if(w == 0) continue;

        softLiterals[j++] = softLiterals[i];
        if(var(softLiterals[i]) >= lastSoftLiteral) continue;
        if(w >= limit) assumptions.push(softLiterals[i]);
    }
    softLiterals.shrink_(softLiterals.size()-j);
}

int64_t MaxSatSolver::computeNextLimit(int64_t limit) const {
    int64_t next = limit;
    for(int i = 0; i < softLiterals.size(); i++) {
        int64_t w = weights[var(softLiterals[i])];
        if(w == 0) continue;
        if(w >= limit) continue;
        if(next == limit || w > next) next = w;
    }
    return next;
}

bool MaxSatSolver::tautology(Var v, CRef c1, CRef c2) {
    Clause& a = ca[c1];
    Clause& b = ca[c2];
    for(int i = 0; i < a.size(); i++) {
        if(var(a[i]) == v) continue;
        for(int j = 0; j < b.size(); j++) {
            if(var(b[j]) == v) continue;
            if(var(a[i]) == var(b[j]) && sign(a[i]) != sign(b[j])) { return true; }
        }
    }
    return false;
}

void MaxSatSolver::bce() {
                                           ;

    bool repeat = false;

    for(;;) {
        vec<CRef> occ[nVars()][2];
        int ii = 0;
        for(int i = 0; i < clauses.size(); i++) {
            Clause& clause = ca[clauses[i]];
            if(clause.mark()) continue;
            clauses[ii++] = clauses[i];
            for(int j = 0; j < clause.size(); j++) {

                occ[var(clause[j])][sign(clause[j])].push(clauses[i]);
            }
        }
        clauses.shrink_(clauses.size()-ii);


        for(int i = 0; i < nVars(); i++) {
            if(occ[i][0].size() == 0 && occ[i][1].size() == 0) {

                weights[i] = 0;
                continue;
            }

            if(occ[i][0].size() == 0 || occ[i][1].size() == 0) {
                if(weights[i] == 0) {

                    addClause(mkLit(i, occ[i][1].size() == 0));
                }
                else {
                    for(int j = 0; j < softLiterals.size(); j++) {
                        if(var(softLiterals[j]) == i) {
                            if(occ[i][sign(softLiterals[j])] != 0) {

                                addClause(mkLit(i, occ[i][1].size() == 0));
                                weights[i] = 0;
                            }
                            break;
                        }
                    }
                }
            }


            for(int s = 0; s <= 1; s++) {
                if(weights[i] > 0) {
                    bool skip = false;
                    for(int j = 0; j < softLiterals.size(); j++) if(var(softLiterals[j]) == i && sign(softLiterals[j]) != s) { skip = true; break; }
                    if(skip) continue;
                }

                for(int j = 0; j < occ[i][s].size(); j++) {
                    bool ok = true;
                    for(int k = 0; k < occ[i][1-s].size(); k++) if(!tautology(i, occ[i][s][j], occ[i][1-s][k])) { ok = false; break; }
                    if(ok) { removeClause(occ[i][s][j]); repeat = true; }
                }
            }

        }
        if(!repeat) break;
        repeat = false;

    }
}

void MaxSatSolver::preprocess() {
    assert(decisionLevel() == 0);
    if(softLiterals.size() == 0) return;
                                      ;



                                                                    ;
    vec<bool> signs(nVars());
    for(int i = 0; i < softLiterals.size(); i++) {
        if(weights[var(softLiterals[i])] != weights[var(softLiterals[0])]) {
                                                                                              ;
            return;
        }
        signs[var(softLiterals[i])] = sign(softLiterals[i]);
    }

                                                                            ;
    vec<vec<CRef>*> clausesPartition;
    vec<int> sizes;
    Map<int, int> sizeMap;
    for(int i = 0; i < clauses.size(); i++) {
        Clause& clause = ca[clauses[i]];
        assert(clause.size() >= 2);
        if(!sizeMap.has(clause.size())) {
            sizes.push(clause.size());
            sizeMap.insert(clause.size(), clausesPartition.size());
            clausesPartition.push(new vec<CRef>());
        }
        clausesPartition[sizeMap[clause.size()]]->push(clauses[i]);
    }
    Glucose::sort(sizes);

                                                                            ;
    for(int i = 0; i < sizes.size(); i++) {
                                                                                 ;
        vec<CRef>& clauses = *clausesPartition[sizeMap[sizes[i]]];
        for(int j = 0; j < clauses.size(); j++) {
            Clause& clause = ca[clauses[j]];
            assert(clause.size() == sizes[i]);

            int64_t min = INT64_MAX;
            for(int k = 0; k < clause.size(); k++) {
                if(value(clause[k]) == (Glucose::lbool((uint8_t)1))) continue;
                if(weights[var(clause[k])] == 0 || signs[var(clause[k])] == sign(clause[k])) { min = INT64_MAX; break; }
                if(weights[var(clause[k])] < min) min = weights[var(clause[k])];
            }
            if(min == INT64_MAX) continue;

            conflict.clear();
            for(int k = 0; k < clause.size(); k++) if(value(clause[k]) != (Glucose::lbool((uint8_t)1))) conflict.push(clause[k]);
                                                                                                     ;
            updateLowerBound(min);
            (this->*corestrat)(min);
        }
    }

                                                ;
    for(int i = 0; i < clausesPartition.size(); i++) delete clausesPartition[i];
}

void MaxSatSolver::removeSoftLiteralsAtLevelZero() {
    assert(decisionLevel() == 0);
    int j = 0;
    for(int i = 0; i < softLiterals.size(); i++) {
        if(value(softLiterals[i]) != (Glucose::lbool((uint8_t)2))) {
            if(value(softLiterals[i]) == (Glucose::lbool((uint8_t)1))) {
                                                                                                                                         ;
                updateLowerBound(weights[var(softLiterals[i])]);
            }
            weights[var(softLiterals[i])] = 0;
            continue;
        }
        softLiterals[j++] = softLiterals[i];
    }
    softLiterals.shrink_(softLiterals.size() - j);
}

  lbool MaxSatSolver::solveWithUpperBound(int ub) {
    inClauses = clauses.size();
    upperbound = INT64_MAX;
    PseudoBooleanSolver::solve();
    if(status == (Glucose::lbool((uint8_t)1))) { cerr << "s UNSATISFIABLE" << endl; return (Glucose::lbool((uint8_t)1)); }
    if(status == (Glucose::lbool((uint8_t)0))) updateUpperBound();
    upperbound = min<int>(upperbound,ub+1);
    cancelUntil(0);

    removeSoftLiteralsAtLevelZero();
    preprocess();
    hardening();

    solve_();
                                                                           ;

    if(upperbound == INT64_MAX) { cerr << "s UNSATISFIABLE" << endl; return (Glucose::lbool((uint8_t)1)); }

    if(lowerbound < upperbound) {
      if(!ok) status = (Glucose::lbool((uint8_t)1));
      else {
                                      ;
        setAssumptions(1);
        PseudoBooleanSolver::solve();
                                                                ;
      }
      if(status == (Glucose::lbool((uint8_t)0))) updateUpperBound();
      else lowerbound = upperbound;
    }
                                                                           ;

    assert(lowerbound == upperbound);
    assert(upperbound < INT64_MAX);

    if(option_maxsat_printmodel) printModel();
    return (Glucose::lbool((uint8_t)0));
  }


lbool MaxSatSolver::solve() {




    inClauses = clauses.size();
    upperbound = INT64_MAX;




    PseudoBooleanSolver::solve();



    if(status == (Glucose::lbool((uint8_t)1))) { cerr << "s UNSATISFIABLE" << endl; return (Glucose::lbool((uint8_t)1)); }
    if(status == (Glucose::lbool((uint8_t)0))) updateUpperBound();
    cancelUntil(0);

    removeSoftLiteralsAtLevelZero();
    preprocess();
    hardening();

    solve_();
                                                                           ;

    if(upperbound == INT64_MAX) { cerr << "s UNSATISFIABLE" << endl; return (Glucose::lbool((uint8_t)1)); }

    if(lowerbound < upperbound) {
        if(!ok) status = (Glucose::lbool((uint8_t)1));
        else {
                                          ;
            setAssumptions(1);



            PseudoBooleanSolver::solve();
                                                                    ;
        }
        if(status == (Glucose::lbool((uint8_t)0))) updateUpperBound();
        else lowerbound = upperbound;
    }
                                                                           ;

    assert(lowerbound == upperbound);
    assert(upperbound < INT64_MAX);



    if(option_maxsat_printmodel) printModel();
    return (Glucose::lbool((uint8_t)0));
}

int64_t MaxSatSolver::nextUp(int64_t curr) const {
    int64_t res = INT64_MAX;
    for(int i = 0; i < softLiterals.size(); i++)
        if(weights[var(softLiterals[i])] > curr && weights[var(softLiterals[i])] < res)
            res = weights[var(softLiterals[i])];
    return res;
}

void MaxSatSolver::quickSort(int left, int right) {
  ::sort(softLiterals.data+left,softLiterals.data+right,[&](Lit const& a, Lit const& b) {
      return activity[var(a)]<activity[var(b)];
    });
}

void MaxSatSolver::solve_() {
    int vars = nVars();
    lastSoftLiteral = disjcores == NO ? INT_MAX : vars;

    int64_t limit = computeNextLimit(INT64_MAX);
    int64_t nextLimit;

    vec<vec<Lit>*> cores;
    int cacheSize = 10000000;

    vec<int> seen;
    int seenValue = 0;

    for(;;) {
        if(softLiterals.size() > 1) quickSort(0, softLiterals.size()-1);

        hardening();
        if(lowerbound == upperbound) return;
        setAssumptions(limit);

        while(seen.size() < nVars()) seen.push(0);
        seenValue++;
        int count = assumptions.size();
        int ii = 0;
        for(int i = 0; i < cores.size(); i++) {
            vec<Lit>& core = *cores[i];
            for(int j = 0; j < core.size(); j++) if(weights[var(core[j])] == 0) core.clear();
            if(core.size() == 0) { delete cores[i]; continue; }
            cores[ii++] = cores[i];
        }
        cores.shrink_(cores.size() - ii);

        ii = INT_MAX;
        for(int i = 0; i < cores.size(); i++) {
            vec<Lit>& core = *cores[cores.size()-i-1];
            if(core.size() < ii) ii = core.size();
            int min = 0;
            for(int j = 0; j < core.size(); j++) {
                if(seen[var(core[j])] == seenValue) { min = -1; break; }
                if(weights[var(core[j])] < weights[var(core[min])]) min = j;
            }
            if(min == -1) continue;
            seen[var(core[min])] = seenValue;

            int jj = 0;
            for(int j = 0; j < assumptions.size(); j++) {
                if(var(assumptions[j]) == var(core[min])) continue;
                assumptions[jj++] = assumptions[j];
            }
            assumptions.shrink_(assumptions.size()-jj);
        }
        if(ii <= cacheSize) { ii = -1; goto DO_SOMETHING;}


                                                                          ;

        lastConflict = conflicts;
        lastPropagation = propagations;
        lastCallCpuTime = Glucose::cpuTime();

                                                                                                                                                                                                                                                                       ;
                                                          ;

        sortAssumptions = true;

        if(assumptions.size() == 0 && upperbound != INT64_MAX) status = (Glucose::lbool((uint8_t)2));
        else
             PseudoBooleanSolver::solve();

        sortAssumptions = false;

        if(status != (Glucose::lbool((uint8_t)1))) {
            if(status == (Glucose::lbool((uint8_t)0))) updateUpperBound();

            if(lastSoftLiteral == vars && vars < nVars()) {
                lastSoftLiteral = INT_MAX;
                                                               ;
                continue;
            }

            nextLimit = computeNextLimit(limit);
            if(nextLimit == limit) {
                                                                                                   ;
                if(count != assumptions.size()) {
                    cacheSize = cacheSize * 2;
                    DO_SOMETHING:
                    int min = -1;
                    for(int i = 0; i < cores.size(); i++) {
                        vec<Lit>& core = *cores[i];
                        if(core.size() == 0) continue;
                        if(min == -1 || core.size() < cores[min]->size()) min = i;
                    }

                    if(min == -1) return;

                    cores[min]->moveTo(conflict);
                    progressionMinimize(computeConflictWeight());
                    trim();

                    if(conflict.size() > cacheSize) cacheSize = conflict.size();

                    int64_t w = computeConflictWeight();
                    updateLowerBound(w);

                    assert(conflict.size() > 0);
                                                                                                           ;
                    if(conflict.size() == 1) weights[var(conflict.last())] = 0;
                    else (this->*corestrat)(w);

                    if(ii != -1)
                        limit = computeNextLimit(INT64_MAX);
                    continue;
                }
                return;
            }

                                                                                                         ;
            limit = nextLimit;
            lastSoftLiteral = disjcores == NO ? INT_MAX : vars;

            continue;
        }

        cancelUntil(0);
                                                                       ;
                                                    ;

        if(conflict.size() == 0) return;

        assert(decisionLevel() == 0);
        progressionMinimize(computeConflictWeight());



        trim();

        if(Glucose::cpuTime() < 600.0 && conflict.size() > cacheSize) {
                                                                                                                       ;
            cacheSize += 1 + cacheSize / 100;

            cores.push(new vec<Lit>());
            conflict.moveTo(*cores.last());
            continue;
        }

        int64_t w = computeConflictWeight();
        updateLowerBound(w);

        if(conflict.size() != 0){
          assert(conflict.size() > 0);
                                                                                                 ;
          if(conflict.size() == 1) weights[var(conflict.last())] = 0;
          else (this->*corestrat)(w);
        }
    }
}

int64_t MaxSatSolver::computeConflictWeight() const {
    int64_t min = INT64_MAX;
    for(int i = 0; i < conflict.size(); i++) if(weights[var(conflict[i])] < min) min = weights[var(conflict[i])];
    return min;
}

void MaxSatSolver::updateLowerBound(int64_t limit) {
    lowerbound += limit;

    if(cpuTime() > satTL) throw TimeLimitException();
}

void MaxSatSolver::updateUpperBound() {
    assert(status == (Glucose::lbool((uint8_t)0)));
    int64_t newupperbound = lowerbound;
    for(int i = 0; i < softLiterals.size(); i++)
        if(value(softLiterals[i]) == (Glucose::lbool((uint8_t)1))) newupperbound += weights[var(softLiterals[i])];
    if(newupperbound < upperbound) {
        upperbound = newupperbound;
        copyModel();
                                              ;

        if(cpuTime() > satTL) throw TimeLimitException();
    }
}

void MaxSatSolver::progressionBinaryFind(int64_t limit) {
    assert(decisionLevel() == 0);

    vec<Lit> allAssumptions;
    assumptions.moveTo(allAssumptions);
    vec<Lit> remainingAssumptions;

    satisfiableSoftLiterals.growTo(nVars(), false);
    for(int i = 0; i < allAssumptions.size(); i++) {
        assert(var(allAssumptions[i]) < satisfiableSoftLiterals.size());
        if(satisfiableSoftLiterals[var(allAssumptions[i])])
            assumptions.push(allAssumptions[i]);
        else
            remainingAssumptions.push(allAssumptions[i]);
    }
    assumptions.copyTo(allAssumptions);
    for(int i = 0; i < remainingAssumptions.size(); i++) allAssumptions.push(remainingAssumptions[i]);
    const int progressionFrom = 1;
    int progression = progressionFrom;
    int fixed = assumptions.size();
    int sat = fixed;

    while(lowerbound + limit < upperbound) {
        if(fixed + progression >= allAssumptions.size()) {
            assert(sat <= assumptions.size());
            assumptions.shrink_(assumptions.size() - sat);
            break;
        }

                                                                                                                                          ;

        for(int i = assumptions.size(); i < fixed + progression; i++) {
            assert(i < allAssumptions.size());
            assumptions.push(allAssumptions[i]);
        }



        sumLBD = 0.0;
        conflictsRestarts = 0;

        assert(decisionLevel() == 0);

        PseudoBooleanSolver::solve();




        if(status == (Glucose::lbool((uint8_t)1))) {
                                                                       ;
            cancelUntil(0);
            return;
        }
        else {
                                                                    ;
            if(status == (Glucose::lbool((uint8_t)0))) {
                for(int i = sat; i < assumptions.size(); i++) satisfiableSoftLiterals[var(assumptions[i])] = true;
                sat = assumptions.size();
                updateUpperBound();
            }







            progression *= 2;
        }
        cancelUntil(0);
    }


    fixed = assumptions.size();
    int toAdd = (allAssumptions.size() - fixed) / 2;
    while(lowerbound + limit < upperbound) {
        if(toAdd == 0) break;

                                                                                                                                 ;

        for(int i = assumptions.size(); i < fixed + toAdd; i++) {
            assert(i < allAssumptions.size());
            assumptions.push(allAssumptions[i]);
        }



        sumLBD = 0.0;
        conflictsRestarts = 0;

        assert(decisionLevel() == 0);

        PseudoBooleanSolver::solve();



        if(status == (Glucose::lbool((uint8_t)1))) {
                                                                       ;
            return;
        }
        else {
                                                                    ;
            if(status == (Glucose::lbool((uint8_t)0))) {
                updateUpperBound();
            }







            fixed += toAdd;
            toAdd /= 2;
        }
        cancelUntil(0);
    }

    sumLBD = 0.0;
    conflictsRestarts = 0;
    allAssumptions.moveTo(assumptions);
                                                                                                ;
    assert(decisionLevel() == 0);

    PseudoBooleanSolver::solve();

}

void MaxSatSolver::trim() {
    assert(decisionLevel() == 0);

    if(conflict.size() <= 1) return;

    int counter = 0;

    do{
        counter++;
        assumptions.clear();
        for(int i = 0; i < conflict.size(); i++) assumptions.push(~conflict[i]);

        PseudoBooleanSolver::solve();

        assert(status == (Glucose::lbool((uint8_t)1)));
                                                                                                       ;
                                                    ;
        cancelUntil(0);
        if(conflict.size() <= 1) return;
    }while(assumptions.size() > conflict.size());

    if(counter % 2 == 1) for(int i = 0; i < assumptions.size(); i++) conflict[i] = ~assumptions[i];

    assert(conflict.size() > 1);
}

void MaxSatSolver::progressionMinimize(int64_t limit) {
    assert(decisionLevel() == 0);
    if(conflict.size() <= 1) return;

    double cpuTime = Glucose::cpuTime() - lastCallCpuTime;
    uint64_t budget = conflicts - lastConflict;
    uint64_t budgetMin = budget / cpuTime;
    if(budget > budget * 30 / cpuTime) budget = budget * 30 / cpuTime;
                                                                                                                                                                                             ;
    trim();
    if(budget == 0) return;

    vec<Lit> core;
    conflict.moveTo(core);

    vec<Lit> allAssumptions;
    for(int i = 0; i < core.size(); i++) allAssumptions.push(~core[i]);

    assumptions.clear();
    const int progressionFrom = 1;
    int progression = progressionFrom;
    int fixed = 0;
    while(lowerbound + limit < upperbound) {
        if(fixed + progression >= allAssumptions.size()) {
            if(progression == progressionFrom) break;
            progression = progressionFrom;
            fixed = assumptions.size();
            if(budget > budgetMin) budget /= 2;
            continue;
        }

                                                                                           ;

        int prec = assumptions.size();
        for(int i = assumptions.size(); i < fixed + progression; i++) {
            assert(i < allAssumptions.size());
            assumptions.push(allAssumptions[i]);
        }

        float sumLBD_ = sumLBD;
        uint64_t conflictsRestarts_ = conflictsRestarts;
        sumLBD = 0;
        conflictsRestarts = 0;
        setConfBudget(budget);

        PseudoBooleanSolver::solve();

        budgetOff();
        sumLBD += sumLBD_;
        conflictsRestarts += conflictsRestarts_;

        if(status == (Glucose::lbool((uint8_t)1))) {
                                                                             ;
            progression = progressionFrom;
            if(budget > budgetMin) budget /= 2;

            assumptions.moveTo(core);
            cancelUntil(0);
            trim();
            core.moveTo(assumptions);
            conflict.moveTo(core);

            int j = 0;
            for(int i = 0, k = core.size() - 1; i < prec; i++) {
                if(k < 0) break;
                if(assumptions[i] != ~core[k]) continue;
                assumptions[j++] = assumptions[i];
                k--;
            }
            assumptions.shrink_(assumptions.size() - j);
            fixed = assumptions.size();

            j = 0;
            for(int i = 0, k = core.size() - 1; i < allAssumptions.size(); i++) {
                if(k < 0) break;
                if(allAssumptions[i] != ~core[k]) continue;
                allAssumptions[j++] = allAssumptions[i];
                k--;
            }
            allAssumptions.shrink_(allAssumptions.size() - j);
        }
        else {
                                                                    ;
            progression *= 2;
            if(status == (Glucose::lbool((uint8_t)0))) {
                updateUpperBound();
            }
        }
        cancelUntil(0);
    }
    core.moveTo(conflict);
}

void MaxSatSolver::biprogressionMinimize(int64_t limit) {
    assert(decisionLevel() == 0);
    if(conflict.size() <= 1) return;

    double cpuTime = Glucose::cpuTime() - lastCallCpuTime;
    uint64_t budget = conflicts - lastConflict;
    if(budget > budget * 30 / cpuTime) budget = budget * 30 / cpuTime;
                                                                                                                                                                                             ;
    trim();
    if(budget == 0) return;

    vec<Lit> core;
    conflict.moveTo(core);

    vec<Lit> allAssumptions;
    for(int i = 0; i < core.size(); i++) allAssumptions.push(~core[i]);

    assumptions.clear();
    const int progressionFrom = 1;
    int progression = progressionFrom;
    bool increasing = true;
    int fixed = 0;
    while(lowerbound + limit < upperbound) {
        if(increasing && fixed + progression >= allAssumptions.size()) {
            increasing = false;


            progression /= 2;
            fixed = assumptions.size();

            continue;
        }
        if(progression == 0) break;
        if(!increasing && fixed + progression >= allAssumptions.size()) { progression /= 2; continue; }

                                                                                           ;

        int prec = assumptions.size();
        for(int i = assumptions.size(); i < fixed + progression; i++) {
            assert(i < allAssumptions.size());
            assumptions.push(allAssumptions[i]);
        }

        float sumLBD_ = sumLBD;
        uint64_t conflictsRestarts_ = conflictsRestarts;
        sumLBD = 0;
        conflictsRestarts = 0;
        setConfBudget(budget);

        PseudoBooleanSolver::solve();

        budgetOff();
        sumLBD += sumLBD_;
        conflictsRestarts += conflictsRestarts_;

        if(status == (Glucose::lbool((uint8_t)1))) {
                                                                             ;

            increasing = false;
            progression /= 2;

            assumptions.moveTo(core);
            cancelUntil(0);
            trim();
            core.moveTo(assumptions);
            conflict.moveTo(core);


            int j = 0;
            for(int i = 0, k = core.size() - 1; i < prec; i++) {
                if(k < 0) break;
                if(assumptions[i] != ~core[k]) continue;
                assumptions[j++] = assumptions[i];
                k--;
            }
            assumptions.shrink_(assumptions.size() - j);
            fixed = assumptions.size();

            j = 0;
            for(int i = 0, k = core.size() - 1; i < allAssumptions.size(); i++) {
                if(k < 0) break;
                if(allAssumptions[i] != ~core[k]) continue;
                allAssumptions[j++] = allAssumptions[i];
                k--;
            }
            allAssumptions.shrink_(allAssumptions.size() - j);
        }
        else {
                                                                    ;
            progression = increasing ? progression * 2 : progression / 2;
            if(!increasing && status == (Glucose::lbool((uint8_t)2))) fixed = assumptions.size();
            if(status == (Glucose::lbool((uint8_t)0))) updateUpperBound();
        }
        cancelUntil(0);
    }
    core.moveTo(conflict);
}

void MaxSatSolver::progressionBinaryMinimize(int64_t limit) {
    assert(decisionLevel() == 0);
    if(conflict.size() <= 1) return;

    double cpuTime = Glucose::cpuTime() - lastCallCpuTime;
    uint64_t budgetConflics = conflicts - lastConflict;
    if(budgetConflics > budgetConflics * 30 / cpuTime) budgetConflics = budgetConflics * 30 / cpuTime;
                                                                                                                                                     ;
    trim();
    if(conflicts == lastConflict) return;

    vec<Lit> core;
    conflict.moveTo(core);

    vec<Lit> allAssumptions;
    for(int i = 0; i < core.size(); i++) allAssumptions.push(~core[i]);

    assumptions.clear();
    const int progressionFrom = 1;
    int progression = progressionFrom;
    vec<Lit> sat;
    while(lowerbound + limit < upperbound) {
        if(progression >= allAssumptions.size()) break;

                                                                                                                     ;

        int prec = assumptions.size();
        for(int i = assumptions.size(); i < progression; i++) {
            assert(i < allAssumptions.size());
            assumptions.push(allAssumptions[i]);
        }

        float sumLBD_ = sumLBD;
        uint64_t conflictsRestarts_ = conflictsRestarts;
        sumLBD = 0;
        conflictsRestarts = 0;
        setConfBudget(budgetConflics);

        PseudoBooleanSolver::solve();

        budgetOff();
        sumLBD += sumLBD_;
        conflictsRestarts += conflictsRestarts_;
        if(status == (Glucose::lbool((uint8_t)1))) {
                                                                             ;

            assumptions.moveTo(core);
            cancelUntil(0);
            trim();
            core.moveTo(assumptions);
            conflict.moveTo(core);


            int j = 0;
            for(int i = 0, k = core.size() - 1; i < prec; i++) {
                if(k < 0) break;
                if(assumptions[i] != ~core[k]) continue;
                assumptions[j++] = assumptions[i];
                k--;
            }
            assumptions.shrink_(assumptions.size() - j);

            j = 0;
            for(int i = 0, k = core.size() - 1; i < allAssumptions.size(); i++) {
                if(k < 0) break;
                if(allAssumptions[i] != ~core[k]) continue;
                allAssumptions[j++] = allAssumptions[i];
                k--;
            }
            allAssumptions.shrink_(allAssumptions.size() - j);

            j = 0;
            for(int i = 0, k = core.size() - 1; i < sat.size(); i++) {
                if(k < 0) break;
                if(sat[i] != ~core[k]) continue;
                sat[j++] = sat[i];
                k--;
            }
            sat.shrink_(sat.size() - j);

            break;
        }

                                                                ;
        progression *= 2;
        if(status == (Glucose::lbool((uint8_t)0))) {
            while(sat.size() < assumptions.size()) sat.push(assumptions[sat.size()]);
            updateUpperBound();
        }
        cancelUntil(0);
    }

    sat.moveTo(assumptions);
    int fixed = assumptions.size();
    int toAdd = (core.size() - fixed) / 2;
    while(lowerbound + limit < upperbound) {
        if(toAdd <= core.size() / 10) break;

                                                                                                                                     ;

        int prec = assumptions.size();
        for(int i = assumptions.size(); i < fixed + toAdd; i++) {
                                                                                                                  ;
            assumptions.push(allAssumptions[i]);
        }

        float sumLBD_ = sumLBD;
        uint64_t conflictsRestarts_ = conflictsRestarts;
        sumLBD = 0;
        conflictsRestarts = 0;
        setConfBudget(budgetConflics);

        PseudoBooleanSolver::solve();

        budgetOff();
        sumLBD += sumLBD_;
        conflictsRestarts += conflictsRestarts_;
        if(status == (Glucose::lbool((uint8_t)1))) {
                                                                             ;

            assumptions.moveTo(core);
            cancelUntil(0);
            trim();
            core.moveTo(assumptions);
            conflict.moveTo(core);

            int j = 0;
            for(int i = 0, k = core.size() - 1; i < prec; i++) {
                if(k < 0) break;
                if(assumptions[i] != ~core[k]) continue;
                assumptions[j++] = assumptions[i];
                k--;
            }
            assumptions.shrink_(assumptions.size() - j);
            fixed = assumptions.size();

            j = 0;
            for(int i = 0, k = core.size() - 1; i < allAssumptions.size(); i++) {
                if(k < 0) break;
                if(allAssumptions[i] != ~core[k]) continue;
                allAssumptions[j++] = allAssumptions[i];
                k--;
            }
            allAssumptions.shrink_(allAssumptions.size() - j);
                                                                                           ;

            toAdd = (core.size() - fixed) / 2;
        }
        else {
                                                                    ;
            fixed += toAdd;
            toAdd /= 2;
            if(status == (Glucose::lbool((uint8_t)0))) updateUpperBound();
        }
        cancelUntil(0);
    }

    core.moveTo(conflict);
}
void MaxSatSolver::binaryMinimize(int64_t limit) {
    assert(decisionLevel() == 0);
    if(conflict.size() <= 1) return;

    double cpuTime = Glucose::cpuTime() - lastCallCpuTime;
    uint64_t budget = conflicts - lastConflict;
    if(budget > budget * 60 / cpuTime) budget = budget * 60 / cpuTime;
                                                                                                                                                                                                               ;
    trim();
    if(budget == 0) return;

    vec<Lit> core;
    conflict.moveTo(core);

    vec<Lit> allAssumptions;
    for(int i = 0; i < core.size(); i++) allAssumptions.push(~core[i]);

    assumptions.clear();
    int toAdd = core.size() / 2;
    int fixed = 0;
    while(lowerbound + limit < upperbound) {
        if(toAdd == 0) break;

                                                                                        ;

        int prec = assumptions.size();
        for(int i = assumptions.size(); i < fixed + toAdd; i++) {
            assert(i < allAssumptions.size());
            assumptions.push(allAssumptions[i]);
        }

        setConfBudget(budget);

        PseudoBooleanSolver::solve();

        budgetOff();
        if(status == (Glucose::lbool((uint8_t)1))) {
                                                                             ;

            assumptions.moveTo(core);
            cancelUntil(0);
            trim();
            core.moveTo(assumptions);
            conflict.moveTo(core);

            int j = 0;
            for(int i = 0, k = core.size() - 1; i < prec; i++) {
                if(k < 0) break;
                if(assumptions[i] != ~core[k]) continue;
                assumptions[j++] = assumptions[i];
                k--;
            }
            assumptions.shrink_(assumptions.size() - j);
            fixed = assumptions.size();

            j = 0;
            for(int i = 0, k = core.size() - 1; i < allAssumptions.size(); i++) {
                if(k < 0) break;
                if(allAssumptions[i] != ~core[k]) continue;
                allAssumptions[j++] = allAssumptions[i];
                k--;
            }
            allAssumptions.shrink_(allAssumptions.size() - j);

            toAdd = (core.size() - fixed) / 2;
        }
        else {
                                                                    ;
            fixed += toAdd;
            toAdd /= 2;
            if(status == (Glucose::lbool((uint8_t)0))) {
                updateUpperBound();
            }
        }
        cancelUntil(0);
    }
    core.moveTo(conflict);
}

void MaxSatSolver::minimize() {
    assert(decisionLevel() == 0);
    if(conflict.size() <= 1) return;

    static int64_t previousConflicts = 0;
    int64_t budget = 25 * (conflicts - previousConflicts) / 100;

    static vec<lbool> required;
    required.growTo(nVars(), (Glucose::lbool((uint8_t)1)));
    for(int i = 0; i < conflict.size(); i++) required[var(conflict[i])] = (Glucose::lbool((uint8_t)1));

    vec<Lit> core;
    conflict.moveTo(core);

    int requiredAssumptions = 0;

    for(;;) {
        assumptions.shrink_(assumptions.size()-requiredAssumptions);
        int nUnknown = 0;
        for(int i = 0; i < core.size(); i++) {
            if(required[var(core[i])] != (Glucose::lbool((uint8_t)2))) continue;
            assumptions.push(~core[i]);
            nUnknown++;
        }
        Lit last = lit_Undef;
        for(int i = 0; i < core.size(); i++) {
            if(required[var(core[i])] != (Glucose::lbool((uint8_t)1))) continue;
            if(last != lit_Undef) assumptions.push(~last);
            last = core[i];
        }
        if(last == lit_Undef) break;

                                                                                                                                                                                                                                                                                                           ;
        setConfBudget(budget);

        PseudoBooleanSolver::solve();



        if(status == (Glucose::lbool((uint8_t)1))) {



            conflict.moveTo(core);

            reverse(core);

            continue;
        }

        required[var(last)] = status;
        if(status == (Glucose::lbool((uint8_t)0))) {
            assumptions[requiredAssumptions++] = ~last;
        }
    }

    core.moveTo(conflict);
    cancelUntil(0);

    budgetOff();
    previousConflicts = conflicts;
}
void MaxSatSolver::corestrat_one(int64_t limit) {
    assert(decisionLevel() == 0);
                                          ;
    CardinalityConstraint cc;
    cc.bound = conflict.size() - 1;
    while(conflict.size() > 0) {
        weights[var(conflict.last())] -= limit;
        cc.lits.push(~conflict.last());
        conflict.pop();
    }
    assert(conflict.size() == 0);
    for(int i = 0; i < cc.bound; i++) {
        newVar();
        if(i != 0) addClause(~softLiterals.last(), mkLit(nVars()-1));
        setFrozen(nVars()-1, true);
        weights.push(limit);
        softLiterals.push(mkLit(nVars()-1));
        cc.lits.push(~mkLit(nVars()-1));
    }

    assert(cc.size() > 1);
    addConstraint(cc);
}

void MaxSatSolver::corestrat_one_2(int64_t limit) {
    assert(decisionLevel() == 0);
                                            ;
    CardinalityConstraint cc;
    cc.bound = conflict.size() - 1;
    while(conflict.size() > 0) {
        weights[var(conflict.last())] -= limit;
        cc.lits.push(~conflict.last());
        conflict.pop();
    }
    assert(conflict.size() == 0);
    for(int i = 0; i < cc.bound; i++) {
        newVar();
        if(i != 0) addClause(~softLiterals.last(), mkLit(nVars()-1));
        setFrozen(nVars()-1, true);
        weights.push(limit);
        softLiterals.push(mkLit(nVars()-1));
        cc.lits.push(~mkLit(nVars()-1));
    }

    assert(cc.size() > 1);
    addEquality(cc);
}

void MaxSatSolver::corestrat_one_wc(int64_t limit) {
    assert(decisionLevel() == 0);
                                               ;
    WeightConstraint wc;
    wc.bound = conflict.size() - 1;
    while(conflict.size() > 0) {
        weights[var(conflict.last())] -= limit;
        wc.lits.push(~conflict.last());
        wc.coeffs.push(1);
        conflict.pop();
    }
    assert(conflict.size() == 0);
    for(int i = 0; i < wc.bound; i++) {
        newVar();
        if(i != 0) addClause(~softLiterals.last(), mkLit(nVars()-1));
        setFrozen(nVars()-1, true);
        weights.push(limit);
        softLiterals.push(mkLit(nVars()-1));
        wc.lits.push(~mkLit(nVars()-1));
        wc.coeffs.push(1);
    }

    assert(wc.size() > 1);
    addConstraint(wc);
}

void MaxSatSolver::corestrat_one_neg(int64_t limit) {
    assert(decisionLevel() == 0);
                                                ;
    CardinalityConstraint cc;
    cc.bound = conflict.size() - 1;
    while(conflict.size() > 0) {
        weights[var(conflict.last())] -= limit;
        cc.lits.push(~conflict.last());
        conflict.pop();
    }
    assert(conflict.size() == 0);
    for(int i = 0; i < cc.bound; i++) {
        newVar();
        if(i != 0) addClause(~softLiterals.last(), ~mkLit(nVars()-1));
        setFrozen(nVars()-1, true);
        weights.push(limit);
        softLiterals.push(~mkLit(nVars()-1));
        cc.lits.push(mkLit(nVars()-1));
    }

    assert(cc.size() > 1);
    addConstraint(cc);
}

void MaxSatSolver::corestrat_one_neg_wc(int64_t limit) {
    assert(decisionLevel() == 0);
                                                    ;
    WeightConstraint wc;
    wc.bound = conflict.size() - 1;
    while(conflict.size() > 0) {
        weights[var(conflict.last())] -= limit;
        wc.lits.push(~conflict.last());
        wc.coeffs.push(1);
        conflict.pop();
    }
    assert(conflict.size() == 0);
    for(int i = 0; i < wc.bound; i++) {
        newVar();
        if(i != 0) addClause(~softLiterals.last(), ~mkLit(nVars()-1));
        setFrozen(nVars()-1, true);
        weights.push(limit);
        softLiterals.push(~mkLit(nVars()-1));
        wc.lits.push(mkLit(nVars()-1));
        wc.coeffs.push(1);
    }

    assert(wc.size() > 1);
    addConstraint(wc);
}

void MaxSatSolver::corestrat_one_pmres(int64_t limit) {
    assert(decisionLevel() == 0);

                                                ;

    const int b = option_maxsat_tag * 2;
    const int m = ceil(2.0 * conflict.size() / (b-2.0));
    const int N = ceil(
            (
                conflict.size()
                + conflict.size() - 1
                + 2 * (m-1)
            ) / (m * 2.0)
        );

                                                                                      ;

    Lit prec = lit_Undef;
    for(;;) {
        assert(conflict.size() > 0);

        CardinalityConstraint cc;

        int i = N;
        if(prec != lit_Undef) { cc.lits.push(prec); i--; }
        for(; i > 0; i--) {
            if(conflict.size() == 0) break;
            weights[var(conflict.last())] -= limit;
            cc.lits.push(~conflict.last());
            conflict.pop();
        }
        assert(cc.size() > 0);
        cc.bound = cc.size()-1;

        if(conflict.size() > 0) cc.bound++;

        for(i = 0; i < cc.bound; i++) {
            newVar();
            setFrozen(nVars()-1, true);
            cc.lits.push(~mkLit(nVars()-1));
            if(i != 0) addClause(~mkLit(nVars()-2), mkLit(nVars()-1));
            if(i == 0 && conflict.size() > 0) {
                weights.push(0);
                prec = mkLit(nVars()-1);





            }
            else {
                weights.push(limit);
                softLiterals.push(mkLit(nVars()-1));
            }
        }

                                                                 ;
        addConstraint(cc);

        if(conflict.size() == 0) break;
    }

    assert(conflict.size() == 0);
}

void MaxSatSolver::corestrat_one_pmres_2(int64_t limit) {
    assert(decisionLevel() == 0);
                                                  ;

    const int N = option_maxsat_tag;

    Lit prec = lit_Undef;
    vec<Lit> lits;
    for(;;) {
        assert(conflict.size() > 0);

        CardinalityConstraint cc;

        int i = N;
        if(prec != lit_Undef) { cc.lits.push(prec); lits.push(~prec); i--; }
        for(; i > 0; i--) {
            if(conflict.size() == 0) break;
            weights[var(conflict.last())] -= limit;
            cc.lits.push(~conflict.last());
            lits.push(conflict.last());
            conflict.pop();
        }
        assert(cc.size() > 0);
        cc.bound = cc.size()-1;

        for(i = 0; i < cc.bound; i++) {
            newVar();
            cc.lits.push(~mkLit(nVars()-1));
            if(i != 0) addClause(~mkLit(nVars()-2), mkLit(nVars()-1));
            setFrozen(nVars()-1, true);
            weights.push(limit);
            softLiterals.push(mkLit(nVars()-1));
        }

        addConstraint(cc);

        if(conflict.size() == 0) break;

        newVar();
        weights.push(0);
        prec = mkLit(nVars()-1);
        for(int j = 0; j < lits.size(); j++) addClause(~lits[j], ~prec);
        lits.push(prec);
        addClause(lits);
        lits.clear();
    }

    assert(conflict.size() == 0);
}

void MaxSatSolver::corestrat_pmres(int64_t limit) {
    assert(decisionLevel() == 0);
                                            ;

    Lit prec = lit_Undef;
    vec<Lit> lits;
    while(conflict.size() > 0) {
        weights[var(conflict.last())] -= limit;

        if(prec == lit_Undef) prec = ~conflict.last();
        else {

            newVar();
            setFrozen(nVars()-1, true);
            weights.push(limit);
            softLiterals.push(mkLit(nVars()-1));
            lits.push(prec);
            lits.push(~conflict.last());
            lits.push(mkLit(nVars()-1, true));
            addClause(lits);
            lits.clear();
            addClause(~prec, mkLit(nVars()-1));
            addClause(conflict.last(), mkLit(nVars()-1));

            if(conflict.size() > 1) {

                newVar();
                weights.push(0);
                addClause(prec, mkLit(nVars()-1, true));
                addClause(~conflict.last(), mkLit(nVars()-1, true));
                lits.push(~prec);
                lits.push(conflict.last());
                lits.push(mkLit(nVars()-1));
                addClause(lits);
                lits.clear();

                prec = mkLit(nVars()-1);
            }
        }

        conflict.pop();
    }
    assert(conflict.size() == 0);
}

void MaxSatSolver::corestrat_pmres_reverse(int64_t limit) {
    assert(decisionLevel() == 0);
                                                      ;

    Lit prec = lit_Undef;
    softLiterals.growTo(softLiterals.size() + conflict.size() - 1);
    int pos = softLiterals.size();
    vec<Lit> lits;
    while(conflict.size() > 0) {
        weights[var(conflict.last())] -= limit;

        if(prec == lit_Undef) prec = ~conflict.last();
        else {

            newVar();
            setFrozen(nVars()-1, true);
            weights.push(limit);
            softLiterals[--pos] = mkLit(nVars()-1);
            lits.push(prec);
            lits.push(~conflict.last());
            lits.push(mkLit(nVars()-1, true));
            addClause(lits);
            lits.clear();
            addClause(~prec, mkLit(nVars()-1));
            addClause(conflict.last(), mkLit(nVars()-1));

            if(conflict.size() > 1) {

                newVar();
                weights.push(0);
                addClause(prec, mkLit(nVars()-1, true));
                addClause(~conflict.last(), mkLit(nVars()-1, true));
                lits.push(~prec);
                lits.push(conflict.last());
                lits.push(mkLit(nVars()-1));
                addClause(lits);
                lits.clear();

                prec = mkLit(nVars()-1);
            }
        }

        conflict.pop();
    }
    assert(conflict.size() == 0);
}

void MaxSatSolver::corestrat_pmres_split_conj(int64_t limit) {
    assert(decisionLevel() == 0);
                                                       ;

    vec<Lit> lits;
    for(int i = 0; i < conflict.size(); i++) {
        weights[var(conflict[i])] -= limit;

        if(i == 0) continue;

        newVar();
        setFrozen(nVars()-1, true);
        weights.push(limit);
        softLiterals.push(mkLit(nVars()-1));

        for(int j = 0; j < i; j++) {
            lits.push(mkLit(nVars()-1, true));
            lits.push(~conflict[j]);
            lits.push(~conflict[i]);
            addClause_(lits);
            lits.clear();
        }

        addClause(mkLit(nVars()-1), conflict[i]);
        lits.push(mkLit(nVars()-1));
        for(int j = 0; j < i; j++) lits.push(conflict[j]);
        addClause_(lits);
        lits.clear();
    }
}

void MaxSatSolver::corestrat_pmreslog(int64_t limit) {
    assert(decisionLevel() == 0);
                                               ;

    vec<Lit> lits;
    while(conflict.size() > 1) {
        int j = 0;
        for(int i = 0; i < conflict.size(); i += 2) {
            if(i+1 == conflict.size()) { conflict[j++] = conflict[i]; break; }

            weights[var(conflict[i])] -= limit;
            assert(weights[var(conflict[i])] >= 0);
            weights[var(conflict[i+1])] -= limit;
            assert(weights[var(conflict[i+1])] >= 0);


            newVar();
            setFrozen(nVars()-1, true);
            weights.push(limit);
            softLiterals.push(mkLit(nVars()-1));
            lits.push(~conflict[i]);
            lits.push(~conflict[i+1]);
            lits.push(mkLit(nVars()-1, true));
            addClause(lits);
            lits.clear();
            addClause(conflict[i], mkLit(nVars()-1));
            addClause(conflict[i+1], mkLit(nVars()-1));

            if(conflict.size() <= 2) return;


            newVar();
            weights.push(limit);
            addClause(~conflict[i], mkLit(nVars()-1, true));
            addClause(~conflict[i+1], mkLit(nVars()-1, true));
            lits.push(conflict[i]);
            lits.push(conflict[i+1]);
            lits.push(mkLit(nVars()-1));
            addClause(lits);
            lits.clear();

            conflict[j++] = mkLit(nVars()-1, true);
        }
        conflict.shrink_(conflict.size()-j);
    }
    assert(conflict.size() == 1);
    weights[var(conflict[0])] -= limit;
}

void MaxSatSolver::corestrat_kdyn(int64_t limit) {
    assert(decisionLevel() == 0);
                                           ;

    const int b = conflict.size() <= 2 ? 8 : ceil(log10(conflict.size()) * 16);
    const int m = ceil(2.0 * conflict.size() / (b-2.0));
    const int N = ceil(
            (
                conflict.size()
                + conflict.size() - 1
                + 2 * (m-1)
            ) / (m * 2.0)
        );

                                                                                      ;

    Lit prec = lit_Undef;
    for(;;) {
        assert(conflict.size() > 0);

        CardinalityConstraint cc;

        int i = N;
        if(prec != lit_Undef) { cc.lits.push(prec); i--; }
        for(; i > 0; i--) {
            if(conflict.size() == 0) break;
            weights[var(conflict.last())] -= limit;
            cc.lits.push(~conflict.last());
            conflict.pop();
        }
        assert(cc.size() > 0);
        cc.bound = cc.size()-1;

        if(conflict.size() > 0) cc.bound++;

        for(i = 0; i < cc.bound; i++) {
            newVar();
            setFrozen(nVars()-1, true);
            cc.lits.push(~mkLit(nVars()-1));
            if(i != 0) addClause(~mkLit(nVars()-2), mkLit(nVars()-1));
            if(i == 0 && conflict.size() > 0) {
                weights.push(0);
                prec = mkLit(nVars()-1);
            }
            else {
                weights.push(limit);
                softLiterals.push(mkLit(nVars()-1));
            }
        }

                                                                 ;
        addConstraint(cc);

        if(conflict.size() == 0) break;
    }

    assert(conflict.size() == 0);
}

void MaxSatSolver::corestrat_wone(int64_t limit) {
    assert(decisionLevel() == 0);
                                           ;
    WeightConstraint wc;
    wc.bound = -limit;
    while(conflict.size() > 0) {
        wc.lits.push(~conflict.last());
        wc.coeffs.push(weights[var(conflict.last())]);
        wc.bound += weights[var(conflict.last())];
        weights[var(conflict.last())] = 0;
        conflict.pop();
    }
    assert(conflict.size() == 0);
    int64_t n = floor(log2(wc.bound));
    int64_t max = wc.bound;
    for(int64_t i = 0; i <= n; i++) {
        assert(max > 0);
        newVar();
        setFrozen(nVars()-1, true);
        softLiterals.push(mkLit(nVars()-1));

        weights.push(1ll << i);
        if(weights.last() > max) weights.last() = max;
        wc.lits.push(~mkLit(nVars()-1));
        wc.coeffs.push(weights.last());
        max -= weights.last();
    }
    assert(wc.size() > 1);
    addConstraint(wc);
}

void MaxSatSolver::corestrat_wone_gcd(int64_t limit) {
    assert(decisionLevel() == 0);
                                               ;
    int64_t d = limit;
    WeightConstraint wc;
    wc.bound = -limit;
    while(conflict.size() > 0) {
        d = gcd(d, weights[var(conflict.last())]);
        wc.lits.push(~conflict.last());
        wc.coeffs.push(weights[var(conflict.last())]);
        wc.bound += weights[var(conflict.last())];
        weights[var(conflict.last())] = 0;
        conflict.pop();
    }
    assert(conflict.size() == 0);
    int64_t n = floor(log2(wc.bound/d));
    int64_t max = wc.bound;
    for(int64_t i = 0; i <= n; i++) {
        assert(max > 0);
        newVar();
        setFrozen(nVars()-1, true);
        softLiterals.push(mkLit(nVars()-1));

        weights.push((1ll << i) * d);
        if(weights.last() > max) weights.last() = max;
        wc.lits.push(~mkLit(nVars()-1));
        wc.coeffs.push(weights.last());
        max -= weights.last();
    }
    assert(wc.size() > 1);
    addConstraint(wc);
}

void MaxSatSolver::corestrat_wone_remainder(int64_t limit) {
    assert(decisionLevel() == 0);
                                                     ;
    WeightConstraint wc;
    wc.bound = conflict.size() - 1;
    while(conflict.size() > 0) {
        wc.lits.push(~conflict.last());
        wc.coeffs.push(1);
        weights[var(conflict.last())] -= limit;
        conflict.pop();
    }
    assert(conflict.size() == 0);
    int64_t n = floor(log2(wc.bound));
    int64_t max = wc.bound * limit;
    for(int64_t i = 0; i <= n; i++) {
        assert(max > 0);
        newVar();
        setFrozen(nVars()-1, true);
        softLiterals.push(mkLit(nVars()-1));

        weights.push((1ll << i) * limit);
        if(weights.last() > max) weights.last() = max;
        wc.lits.push(~mkLit(nVars()-1));
        wc.coeffs.push(weights.last() / limit);
        max -= weights.last();
    }
    assert(wc.size() > 1);
    addConstraint(wc);
}

}

const float TL0 = 1.0/2;
const float TL1 = 1.5/2;
const float TL2 = 2.9/2;

timer TM;

const int N = 70;

int n;
int si,sj;

int A[N][N];

vii nodes;
int NA[N*N];
int IN[N][N];
int ND[N*N][N*N];

vvi nodeG;
vvii nodeGD;
vii cover[N][N];

vvi goals;
vvi nodeGoals;

bool inGraph(int x, int y) {
  return (0<=x&&x<n&&0<=y&&y<n&&A[x][y]);
}

template<class F>
void iter4(int x, int y, F&& f) {
  if(x+1<n && A[x+1][y]) f(x+1,y,1,0,'D');
  if(x!=0 && A[x-1][y]) f(x-1,y,-1,0,'U');
  if(y+1<n && A[x][y+1]) f(x,y+1,0,1,'R');
  if(y!=0 && A[x][y-1]) f(x,y-1,0,-1,'L');
}

void read(istream& is) {
  is >> n >> si >> sj;
  for(int i = 0; i < (int)(n); ++i) for(int j = 0; j < (int)(n); ++j) {
    char c; cin >> c;
    A[i][j] = (c == '#'?0:(c-'0'));
  }
}

void calcD(){
  int nv = nodes.size();
  for(int a = 0; a < (int)(nv); ++a) {
    static int E[N][N];
    for(int i = 0; i < (int)(n); ++i) for(int j = 0; j < (int)(n); ++j) E[i][j] = 1<<30;
    min_queue<tuple<int,int,int> > Q;
    int i0,j0; tie(i0,j0) = nodes[a];
    Q.push(make_tuple(0,i0,j0));
    E[i0][j0] = 0;
    while(!Q.empty()) {
      int d,i,j; tie(d,i,j) = Q.top(); Q.pop();
      if(IN[i][j] != -1) {
        ND[a][IN[i][j]] = d - A[i][j];
      }
      iter4(i,j,[&](int i2, int j2,int,int,char){
        int d2 = d + A[i2][j2];
        if(d2 < E[i2][j2]) {
          E[i2][j2] = d2;
          Q.push(make_tuple(d2,i2,j2));
        }
      });
    }
  }
}

void init() {
  for(int i = 0; i < (int)(n); ++i) for(int j = 0; j < (int)(n); ++j) if(inGraph(i,j)) {
    IN[i][j] = -1;
    bitset<2> cnt = 0;
    iter4(i,j,[&](int,int,int di,int dj,char) {
      if(di) cnt[0] = 1;
      if(dj) cnt[1] = 1;
    });
    if(cnt.count() == 2 || make_tuple(i,j) == make_tuple(si,sj)) {
      IN[i][j] = nodes.size();
      NA[IN[i][j]] = A[i][j];
      nodes.emplace_back(i,j);
    }
  }
  int nv = nodes.size();

  nodeG.assign(nv,vi());
  nodeGD.assign(nv,vii());
  for(int a = 0; a < (int)(nv); ++a) {
    int i,j; tie(i,j) = nodes[a];
    iter4(i,j,[&](int i2, int j2, int di, int dj,char) {
      int to = -1;
      int dist = 0;
      while(inGraph(i2,j2)) {
        if(to == -1) dist += A[i2][j2];
        if(IN[i2][j2] != -1 && to == -1) {
          to = IN[i2][j2];
        }
        cover[i][j].emplace_back(i2,j2);
        i2 += di;
        j2 += dj;
      }
      if(to != -1) {
        nodeG[a].emplace_back(to);
        nodeGD[a].emplace_back(to,dist);
      }
    });
  }

  { map<pii, vector<int> > W;
    for(int a = 0; a < (int)(nv); ++a) {
      int i,j; tie(i,j) = nodes[a];
      for(auto p : cover[i][j]) {
        W[p].emplace_back(a);
      }
    }

    set<vector<int>> W2;
    for(auto& p : W) {
      sort(begin(p.second), end(p.second));
      W2.insert(p.second);
    }

    goals.assign(begin(W2), end(W2));
  }

  { nodeGoals.assign(nv, vi());
    for(int i = 0; i < (int)((int)goals.size()); ++i) for(int j : goals[i]) nodeGoals[j].emplace_back(i);
  }

  calcD();
}

vi solve_maxsat(bool coeff, float tl){
  using namespace aspino;

  MaxSatSolver solver;

  auto hs = [&](vector<Lit> const& w) {
    vec<Lit> v; for(auto const& x : w) v.push(x);
    solver.addClause(v);
  };

  auto s1 = [&](int w, Lit a){
    vec<Lit> v; v.push(a);
    solver.addWeightedClause(v,w);
  };

  int nv = nodes.size();

  solver.nInVars(nv);
  while(solver.weights.size() < solver.nInVars()) solver.weights.push(0);
  while(solver.nVars() < solver.nInVars()) solver.newVar();

  for(int a = 0; a < (int)(nv); ++a) {
    s1((coeff ? NA[a] : 1),~mkLit(a));
  }

  for(int a = 0; a < (int)(nv); ++a) if(nodes[a] == make_tuple(si,sj)) {
    hs({mkLit(a)});
  }

  for(int a = 0; a < (int)(nv); ++a) {
    vector<Lit> c;
    c.emplace_back(~mkLit(a));
    for(int b : nodeG[a]) c.emplace_back(mkLit(b));
    hs(c);
  }

  for(auto const& p : goals) {
    vector<Lit> c;
    for(auto x : p) c.emplace_back(mkLit(x));
    hs(c);
  }

  try {
    satTimer.reset();
    satTL = tl - TM.elapsed();

    solver.eliminate(true);
    if(!solver.okay()) {
      do { if(!("UNSAT")) { throw runtime_error("main.cpp" ":" "184" " Assertion failed: " "\"UNSAT\""); } } while(0);
    }
    solver.solve();
  }catch(TimeLimitException const&){
  }

  do { cerr << "main.cpp" ":" "190" "  {" << "satTimer.elapsed()" << "} = {"; debug_impl_seq(satTimer.elapsed()); cerr << endl << flush; } while(0);

  vi R;
  for(int i = 0; i < (int)(nv); ++i) if(solver.modelValue(i) == (Glucose::lbool((uint8_t)0))) {
    R.emplace_back(i);
  }
  return R;
}

int IR[N][N];

void calcIR(vi const& R) {
  int nr = R.size();
  for(int i = 0; i < (int)(n); ++i) for(int j = 0; j < (int)(n); ++j) IR[i][j] = -1;
  for(int a = 0; a < (int)(nr); ++a) {
    int i,j; tie(i,j) = nodes[R[a]];
    IR[i][j] = a;
  }
}


void reconstruct1(vi&P, pii const& p, pii const& q) {
  static int E[N][N];
  for(int i = 0; i < (int)(n); ++i) for(int j = 0; j < (int)(n); ++j) E[i][j] = 1<<30;
  static pii from[N][N];

  min_queue<tuple<int,int,int> > Q;
  int i0,j0; tie(i0,j0) = p;
  Q.push(make_tuple(0,i0,j0));
  E[i0][j0] = 0;

  while(!Q.empty()) {
    int d,i,j; tie(d,i,j) = Q.top(); Q.pop();
    iter4(i,j,[&](int i2, int j2,int,int,char){
      int d2 = d + A[i2][j2];
      if(d2 < E[i2][j2]) {
        E[i2][j2] = d2;
        from[i2][j2] = make_tuple(i,j);
        Q.push(make_tuple(d2,i2,j2));
      }
    });
  }

  int i1,j1; tie(i1,j1) = q;
  vi S;
  while(E[i1][j1] != 0) {
    if(IN[i1][j1] != -1) S.emplace_back(IN[i1][j1]);
    auto p = from[i1][j1];
    tie(i1,j1) = p;
  }
  reverse(begin(S), end(S));
  P.insert(end(P), begin(S), end(S));
}

vi reconstruct(vii const& path) {
  vi P;
  P.emplace_back(IN[si][sj]);
  for(int i = 0; i < (int)((int)path.size()-1); ++i) {
    reconstruct1(P, path[i],path[i+1]);
  }
  return P;
}

vi solve_tsp(vi const& R, float tl){
  int nr = R.size();
  calcIR(R);

  vvi D(nr, vi(nr));
  for(int i = 0; i < (int)(nr); ++i) for(int j = 0; j < (int)(nr); ++j) D[i][j] = ND[R[i]][R[j]];

  int sa = IR[si][sj];
  assert(sa != -1);

  vi I;
  for(int i = 0; i < (int)(nr); ++i) if(i != sa) I.emplace_back(i);
  random_shuffle(begin(I), end(I));
  int ni = I.size();

  int score = 0;
  score += D[sa][I[0]];
  for(int i = 0; i < (int)(ni-1); ++i) score += D[I[i]][I[i+1]];
  score += D[I[ni-1]][sa];

  int bestScore = score;
  vi bestI = I;

  auto updBest = [&](){
    if(score < bestScore) {
      bestScore = score;
      bestI = I;
    }
  };

  do { cerr << "main.cpp" ":" "283" "  {" << "TM.elapsed(), tl" << "} = {"; debug_impl_seq(TM.elapsed(), tl); cerr << endl << flush; } while(0);
  sa_t SA(sqrt(score), 1.0, tl - TM.elapsed());

  auto saSwap2 = [&](){
    int i = 0, j = 0;
    while(i >= j) {
      i = random(ni);
      j = random(ni);
    }

    int delta = 0;
    if(i == 0) {
      delta -= D[sa][I[i]];
      delta += D[sa][I[j]];
    }else{
      delta -= D[I[i-1]][I[i]];
      delta += D[I[i-1]][I[j]];
    }

    if(j == ni-1) {
      delta -= D[I[j]][sa];
      delta += D[I[i]][sa];
    }else{
      delta -= D[I[j]][I[j+1]];
      delta += D[I[i]][I[j+1]];
    }

    if(SA.accept(delta)) {
      score += delta;
      reverse(begin(I)+i, begin(I)+j+1);
      updBest();
    }
  };

  for(long long niter = 0;; niter++) {
    if(niter % (1<<10) == 0) {
      if(SA.done > 1.0) break;
      SA.updateDone();
    }
    if(niter % (1<<20) == 0){
      do { cerr << "main.cpp" ":" "323" "  {" << "niter, bestScore, score" << "} = {"; debug_impl_seq(niter, bestScore, score); cerr << endl << flush; } while(0);
    }
    saSwap2();
  }

  vii path;
  path.emplace_back(nodes[R[sa]]);
  for(int i = 0; i < (int)(ni); ++i) path.emplace_back(nodes[R[bestI[i]]]);
  path.emplace_back(nodes[R[sa]]);

  return reconstruct(path);
}

tuple<int,string> solve_recon(vi P, float tl) {
  int nv = nodes.size();
  int np = P.size();
  int ng = goals.size();

  vi cntG;
  auto calcCntG = [&](){
    cntG.assign(ng,0);
    for(int i : P) for(int j : nodeGoals[i]) cntG[j]++;
  };
  calcCntG();

  for(int mgs = (2); mgs <= (int)(20); ++mgs) for(int l = (2); l <= (int)(16); ++l) {
    if(TM.elapsed() > tl) break;
    for(int i = 0; i < (int)(np-l-1); ++i) {
      if(TM.elapsed() > tl) break;



      vi gs;
      { for(int j = (i+1); j <= (int)(i+l); ++j) {
          for(int k : nodeGoals[P[j]]) {
            cntG[k]--;
            if(cntG[k] == 0) gs.emplace_back(k);
          }
        }
        for(int j = (i+1); j <= (int)(i+l); ++j) {
          for(int k : nodeGoals[P[j]]) {
            cntG[k]++;
          }
        }
      }

      if((int)gs.size() > mgs) continue;

      vi IG(ng,-1);
      for(int i = 0; i < (int)(gs.size()); ++i) IG[gs[i]] = i;

      int goald = 0;
      for(int j = (i); j <= (int)(i+l-1); ++j) goald += ND[P[j]][P[j+1]] + NA[P[j+1]];



      vector<uint32_t> masks(nv, (1ll<<gs.size())-1);
      for(int a = 0; a < (int)(nv); ++a) {
        for(int j : nodeGoals[a]) if(IG[j] != -1) {
            masks[a] &= ~(1ll<<IG[j]);
          }
      }



      vector<hash_map<uint64_t, uint64_t>> dp(goald);
      dp[0][((uint64_t)((1ll<<gs.size())-1)<<32) | (uint64_t)P[i]] = 0;

      auto recon = [&](int d, uint64_t p) {
        vi K;
        while(d != 0) {
          uint64_t from = dp[d][p];
          uint32_t u = p;
          K.emplace_back(u);
          uint32_t v = from;
          int dv = d - NA[u] - ND[u][v];
          d = dv; p = from;
        }
        reverse(begin(K), end(K));

        vi NP;
        for(int j = (0); j <= (int)(i); ++j) NP.emplace_back(P[j]);
        for(int j : K) NP.emplace_back(j);
        for(int j = (i+l+1); j <= (int)(np-1); ++j) NP.emplace_back(P[j]);

        P = NP;
        np = P.size();
        calcCntG();
      };

      for(int d = 0; d < (int)(goald); ++d) for(auto const& p : dp[d]) {
        uint32_t mask = (p.first >> 32);
        uint32_t u = p.first;

        if(mask == 0 && u == P[i+l]) {
          do { cerr << "main.cpp" ":" "418" "  {" << "\"OK\", d ,goald" << "} = {"; debug_impl_seq("OK", d ,goald); cerr << endl << flush; } while(0);
          recon(d, p.first);
          goto l_end;
        }

        for(auto vd : nodeGD[u]) {
          int v,dd; tie(v,dd) = vd;
          int d2 = d+dd;
          if(d2 > goald-1) continue;
          dp[d2][((uint64_t)(mask&masks[v])<<32) | (uint64_t)v] = p.first;
        }
      }
      l_end:;
    }
  }

  int sc = NA[P[0]];
  for(int i = 0; i < (int)(np-1); ++i) sc += ND[P[i]][P[i+1]] + NA[P[i+1]];

  string out;
  for(int i = 0; i < (int)(np-1); ++i) {
    int i1,j1; tie(i1,j1) = nodes[P[i]];
    int i2,j2; tie(i2,j2) = nodes[P[i+1]];
    while(i1 < i2) { out += 'D'; i1++; }
    while(i1 > i2) { out += 'U'; i1--; }
    while(j1 < j2) { out += 'R'; j1++; }
    while(j1 > j2) { out += 'L'; j1--; }
  }
  return make_tuple(sc,out);
}

void solve(){
  int sc1, sc2;
  string s1,s2;
  tie(sc1,s1) = solve_recon(solve_tsp(solve_maxsat(0, TL0), TL1), TL2);
  tie(sc2,s2) = solve_recon(solve_tsp(solve_maxsat(1, 1.5+TL0), 1.5+TL1), 1.5+TL2);
  if(sc1 < sc2) {
    cout << s1 << endl;
  }else{
    cout << s2 << endl;
  }
}

int main(int argc, char** argv) {
  (void) argc; (void) argv;

  TM.reset();
  random_reset(9824698624);

  read(cin);
  init();
  solve();

  return 0;
}
