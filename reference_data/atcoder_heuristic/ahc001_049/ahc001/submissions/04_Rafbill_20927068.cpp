#pragma GCC optimize ("O3,inline,omit-frame-pointer,no-asynchronous-unwind-tables,fast-math")


#include <stdio.h>
 #include <bits/stdc++.h>
 #include <ext/pb_ds/assoc_container.hpp>
 #include <bits/stdc++.h>
 #include <sys/time.h>








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
using pii = tuple<int, int>;
using piii = tuple<int, int, int>;
using piiii = tuple<int, int, int, int>;
using vi = vector<int>;
using vii = vector<pii>;
using viii = vector<piii>;
using vvi = vector<vi>;
using vvii = vector<vii>;
using vviii = vector<viii>;

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




template<class T>
void print_collection(ostream& out, T const& x);

template<class T, size_t... I>
void print_tuple(ostream& out, T const& a, index_sequence<I...>);
template<class... A>
ostream& operator<<(ostream& out, tuple<A...> const& x) {
  print_tuple(out, x, index_sequence_for<A...>{});
  return out;
}

template<class A, class B>
ostream& operator<<(ostream& out, pair<A, B> const& a) {
  return out << tie(a.first, a.second);
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



static inline void debug_impl_seq() {
  cerr << "}";
}

template <class T, class... V>
void debug_impl_seq(T const& t, V const&... v) {
  cerr << t;
  if(sizeof...(v)) { cerr << ", "; }
  debug_impl_seq(v...);
}
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

void random_reset(long long int seed = chrono::steady_clock::now().time_since_epoch().count()) { xorshift128_s.reset(seed); }

uint32_t randomInt32() {
  return xorshift128_s.xorshift128();
}

uint64_t randomInt64() {
  return (((uint64_t)randomInt32())<<32ll) | ((uint64_t)randomInt32());
}

long long int random(long long int r) {
  return xorshift128_s.xorshift128()%r;
}

long long int random(long long int l, long long int r) {
  return l + random(r-l+1);
}

const float TL1 = 4.6;
const float TL2 = 4.9;
chrono::high_resolution_clock::time_point t_begin;
float t_getElapsed() {
  return chrono::duration<float>(chrono::high_resolution_clock::now() - t_begin).count();
}

const int B = 10;

bool startOpt = false;

struct ad {
  int ix, x, y, r;

  void read() { cin>>x>>y>>r; }
};

const int M = 9999;
const int N = 200;
int n;
ad A[N];

struct rect {
  int x1, y1, x2, y2;

  void pr() const { cout << x1 << " " << y1 << " " << x2+1 << " " << y2+1 << endl; }
  void from_ad(ad const& a) { x1 = x2 = a.x; y1 = y2 = a.y; };

  int sizeX() const { return (x2-x1+1); }
  int sizeY() const { return (y2-y1+1); }
  int size() const { return (x2-x1+1)*(y2-y1+1); }

  float score(ad const& a) const {
    float x = size();
    float y = a.r;
    return 1.0 - pow(1.0 - (min(x,y) / max(x,y)), 2);
  }
};

ostream& operator<<(ostream& os, rect const& r) {
  return os << make_tuple(r.x1,r.y1,r.x2,r.y2);
}

struct sol {
  rect S[N];
  float sc;

  rect const& operator[](int i) const { return S[i]; }
  rect& operator[](int i) { return S[i]; }

  void calc() {
    sc = 0;
    for(int i = 0; i < (int)(n); ++i) sc += S[i].score(A[i]);
  }
};

void read() {
  cin>>n;
  for(int i = 0; i < (int)(n); ++i) { A[i].ix = i; A[i].read(); }
}


inline float prescore_leaf(float a, float b) __attribute__((always_inline));
inline float prescore_leaf(float a, float b) {
  return min(a,b) / max(a,b);
}

inline float score_leaf(float a, float b) __attribute__((always_inline));
inline float score_leaf(float a, float b) {
  return 1 - pow(1 - prescore_leaf(a, b), 2);
}

inline float score_leaf_min(float a, float b) __attribute__((always_inline));
inline float score_leaf_min(float a, float b) {
  return 1 - pow(1 - prescore_leaf(min(a, b), b), 2);
}

tuple<int, int> solve_leaf(int x, int y, int r) {
  tuple<int, int> best = make_tuple(1, 1);
  float bestSc = prescore_leaf(1, r);
  int j = y;
  for(int i = (1); i <= (int)(x); ++i) {
    float sc = prescore_leaf(i*j,r);
    while(j > 1) { float sc0 = prescore_leaf(i*(j-1),r); if(sc0 > sc) { sc = sc0; j -= 1; } else break; }
    if(sc > bestSc) {
      best = make_tuple(i,j);
      bestSc = sc;
    }
  }
  return best;
}


struct cut_tree;
using cut_tree_ptr = int;
cut_tree& deref_ptr(cut_tree_ptr p);

float curBest = 0;

struct cut_tree {
  cut_tree_ptr l,r;

  float sc;
  union {
    int leaf;
    struct {
      int16_t cutDir;
      int16_t cut;
    };
  };

  cut_tree() = default;
  cut_tree(int leaf_, float sc_) : l(-1), r(-1), sc(sc_), leaf(leaf_) { }

  cut_tree(cut_tree_ptr l_, cut_tree_ptr r_,
           int cutDir_, int cut_)
    : l(l_), r(r_), sc(deref_ptr(l).sc + deref_ptr(r).sc), cutDir(cutDir_), cut(cut_) {
  }

  bool operator<(cut_tree const& o) const { return sc < o.sc; }
  bool operator>(cut_tree const& o) const { return sc > o.sc; }
};

void opt_cut_tree(sol& S, cut_tree const& t) {
  vector<cut_tree> ft;
  auto dfs = letrec([&](auto dfs, cut_tree t) -> int {
    if(t.l == -1) {
      ft.emplace_back(t);
      return ft.size()-1;
    }else{
      t.l = dfs(deref_ptr(t.l));
      t.r = dfs(deref_ptr(t.r));
      ft.emplace_back(t);
      return ft.size()-1;
    }
  });
  int root = dfs(t);

  auto calc_score = letrec([&](auto calc_score, int ix, int x1, int x2, int y1, int y2, ad* I, int ni) -> float {
    cut_tree& t = ft[ix];
    if(t.l == -1) {

      ad const& a = I[0];

      return score_leaf(min(a.r, (x2-x1+1) * (y2-y1+1)), a.r);
    }else{
     if(t.cutDir == 0) {
       int nl = distance(I, partition(I, I+ni, [&](ad const& a) { return a.x <= t.cut; }));
       return calc_score(t.l, x1, t.cut, y1, y2, I, nl) +
         calc_score(t.r, t.cut+1, x2, y1, y2, I+nl, ni-nl);
     }else{
       int nl = distance(I, partition(I, I+ni, [&](ad const& a) { return a.y <= t.cut; }));
       return calc_score(t.l, x1, x2, y1, t.cut, I, nl) +
         calc_score(t.r, x1, x2, t.cut+1, y2, I+nl, ni-nl);
     }
    }
  });

  auto opt = letrec([&](auto opt, bool& imp, int ix, int x1, int x2, int y1, int y2, ad* I, int ni) -> void {
    cut_tree& t = ft[ix];
    t.sc = calc_score(ix, x1, x2, y1, y2, I, ni);
    if(t.l == -1) return;
    if(t.cutDir == 0) {
      int nl = distance(I, partition(I, I+ni, [&](ad const& a) { return a.x <= t.cut; }));
      while(1) {
        t.cut -= 1;
        int nl2 = distance(I, partition(I, I+ni, [&](ad const& a) { return a.x <= t.cut; }));
        if(nl2 != nl) { t.cut += 1; break; }
        float sc2 = calc_score(t.l, x1, t.cut, y1, y2, I, nl) +
          calc_score(t.r, t.cut+1, x2, y1, y2, I+nl, ni-nl);
        if(sc2 > t.sc) { t.sc = sc2; nl = nl2; }
        else { t.cut += 1; break; }
      }
      while(1) {
        t.cut += 1;
        int nl2 = distance(I, partition(I, I+ni, [&](ad const& a) { return a.x <= t.cut; }));
        if(nl2 != nl) { t.cut -= 1; break; }
        float sc2 = calc_score(t.l, x1, t.cut, y1, y2, I, nl) +
         calc_score(t.r, t.cut+1, x2, y1, y2, I+nl, ni-nl);
        if(sc2 > t.sc) { t.sc = sc2; nl = nl2; }
        else { t.cut -= 1; break; }
      }
    }else{
      int nl = distance(I, partition(I, I+ni, [&](ad const& a) { return a.y <= t.cut; }));
      while(1) {
        t.cut -= 1;
        int nl2 = distance(I, partition(I, I+ni, [&](ad const& a) { return a.y <= t.cut; }));
        if(nl2 != nl) { t.cut += 1; break; }
        float sc2 = calc_score(t.l, x1, x2, y1, t.cut, I, nl) +
         calc_score(t.r, x1, x2, t.cut+1, y2, I+nl, ni-nl);
        if(sc2 > t.sc) { t.sc = sc2; nl = nl2; }
        else { t.cut += 1; break; }
      }
      while(1) {
        t.cut += 1;
        int nl2 = distance(I, partition(I, I+ni, [&](ad const& a) { return a.y <= t.cut; }));
        if(nl2 != nl) { t.cut -= 1; break; }
        float sc2 = calc_score(t.l, x1, x2, y1, t.cut, I, nl) +
         calc_score(t.r, x1, x2, t.cut+1, y2, I+nl, ni-nl);
        if(sc2 > t.sc) { t.sc = sc2; nl = nl2; }
        else { t.cut -= 1; break; }
      }
    }

    if(t.cutDir == 0) {
      int nl = distance(I, partition(I, I+ni, [&](ad const& a) { return a.x <= t.cut; }));
      opt(imp, t.l, x1, t.cut, y1, y2, I, nl);
      opt(imp, t.r, t.cut+1, x2, y1, y2, I+nl, ni-nl);
      t.sc = ft[t.l].sc + ft[t.r].sc;
    }else{
      int nl = distance(I, partition(I, I+ni, [&](ad const& a) { return a.y <= t.cut; }));
      opt(imp, t.l, x1, x2, y1, t.cut, I, nl);
      opt(imp, t.r, x1, x2, t.cut+1, y2, I+nl, ni-nl);
      t.sc = ft[t.l].sc + ft[t.r].sc;
    }
  });

  static ad I[N]; for(int i = 0; i < (int)(n); ++i) I[i] = A[i];
  while(1) { bool imp = 0; opt(imp, root, 0,M, 0,M, I,n); if(!imp) break; }




  auto makeSol = letrec([&](auto makeSol, int ix, int x1, int x2, int y1, int y2) -> void {
    auto const& t = ft[ix];
    if(t.l != -1) {
      if(t.cutDir == 0) {
        makeSol(t.l, x1, t.cut, y1, y2);
        makeSol(t.r, t.cut+1, x2, y1, y2);
      }else{
        makeSol(t.l, x1, x2, y1, t.cut);
        makeSol(t.r, x1, x2, t.cut+1, y2);
      }
    }else{
      int i = t.leaf;
      int x, y; tie(x,y) = solve_leaf(x2-x1+1, y2-y1+1, A[i].r);
      x--; y--;
      S[i].from_ad(A[i]);
      while(x && S[i].x1 > x1) { x -= 1; S[i].x1 -= 1; }
      while(x && S[i].x2 < x2) { x -= 1; S[i].x2 += 1; }
      while(y && S[i].y1 > y1) { y -= 1; S[i].y1 -= 1; }
      while(y && S[i].y2 < y2) { y -= 1; S[i].y2 += 1; }
    }
  });

  makeSol(root, 0,M, 0,M);
}

const int CUT_TREE_POOLSIZE = 25'000'000;
cut_tree cut_tree_pool[CUT_TREE_POOLSIZE];
int cut_tree_nfree;

inline cut_tree& deref_ptr(cut_tree_ptr p) {
  return cut_tree_pool[p];
}

struct memory_limit{};
template<class... Args>
cut_tree_ptr make_ptr(Args&&... args) {
  if(cut_tree_nfree == CUT_TREE_POOLSIZE) throw memory_limit{};
  int i = cut_tree_nfree++;
  cut_tree_pool[i] = cut_tree(forward<Args>(args)...);
  return i;
}

void fillSolApprox(sol& S, int x1, int x2, int y1, int y2, cut_tree const& t) {
  if(t.l != -1) {
    if(t.cutDir == 0) {
      fillSolApprox(S, x1, t.cut, y1, y2, deref_ptr(t.l));
      fillSolApprox(S, t.cut+1, x2, y1, y2, deref_ptr(t.r));
    }else{
      fillSolApprox(S, x1, x2, y1, t.cut, deref_ptr(t.l));
      fillSolApprox(S, x1, x2, t.cut+1, y2, deref_ptr(t.r));
    }
  }else{
    int i = t.leaf;
    S[i].x1 = x1;
    S[i].x2 = x2;
    S[i].y1 = y1;
    S[i].y2 = y2;
  }
}

void fillSol(sol& S, int x1, int x2, int y1, int y2, cut_tree const& t) {
  if(t.l != -1) {
    if(t.cutDir == 0) {
      fillSol(S, x1, t.cut, y1, y2, deref_ptr(t.l));
      fillSol(S, t.cut+1, x2, y1, y2, deref_ptr(t.r));
    }else{
      fillSol(S, x1, x2, y1, t.cut, deref_ptr(t.l));
      fillSol(S, x1, x2, t.cut+1, y2, deref_ptr(t.r));
    }
  }else{
    int i = t.leaf;
    int x, y; tie(x,y) = solve_leaf(x2-x1+1, y2-y1+1, A[i].r);
    x--; y--;
    S[i].from_ad(A[i]);
    while(x && S[i].x1 > x1) { x -= 1; S[i].x1 -= 1; }
    while(x && S[i].x2 < x2) { x -= 1; S[i].x2 += 1; }
    while(y && S[i].y1 > y1) { y -= 1; S[i].y1 -= 1; }
    while(y && S[i].y2 < y2) { y -= 1; S[i].y2 += 1; }
  }
}

template<class F>
inline void partX(int x1, int x2, int y1, int y2, ad* I, int ni, F&& f){
  pdqsort(I, I+ni, [&](ad const& i, ad const& j){ return i.x < j.x; });

  int szl = 0, szr = 0;
  for(int i0 = 0; i0 < (int)(ni); ++i0) szr += I[i0].r;

  for(int i = 0; i < (int)(ni-1); ++i) {
    szl += I[i].r; szr -= I[i].r;
    if(I[i+1].x == I[i].x) continue;
    int ax1 = I[i].x;
    int ax2 = I[i+1].x - 1;
    int ax3 = ax2+1;
    int ax4 = ax1-1;

    int b = -1;
    int c = -1;
    auto add = [&](int b_, int c_) { if(b_>=b) { b=b_; c=c_; } };

    if(szl - (y2-y1+1) * (ax2-x1+1) <= 0) {
      ax3 = max(ax1, (szl-1)/(y2-y1+1)+x1);
      add(max(0, szr - (y2-y1+1) * (x2-ax3)), ax3);
    }else{
      add(max(0, szl - (y2-y1+1) * (ax2-x1+1)) + max(0, szr - (y2-y1+1) * (x2-ax2)), ax2);
    }

    if(szr - (y2-y1+1) * (x2-ax1) <= 0) {
      ax4 = min(ax2, x2 - (szr-1) / (y2-y1+1) - 1);
      add(max(0, szl - (y2-y1+1) * (ax4-x1+1)), ax4);
    }else{
      add(max(0, szl - (y2-y1+1) * (ax1-x1+1)) + max(0, szr - (y2-y1+1) * (x2-ax1)), ax1);
    }

    if(ax4+1 <= ax3-1) {
      add(max(0, szl - (y2-y1+1) * ((ax4+ax3)/2-x1+1)) + max(0, szr - (y2-y1+1) * (x2-(ax4+ax3)/2)), (ax4+ax3)/2);
    }

    f(b,c);
  }
}

template<class F>
inline void partY(int x1, int x2, int y1, int y2, ad* I, int ni, F&& f){
  pdqsort(I, I+ni, [&](ad const& i, ad const& j){ return i.y < j.y; });
  int szl = 0, szr = 0;
  for(int i0 = 0; i0 < (int)(ni); ++i0) szr += I[i0].r;

  for(int i = 0; i < (int)(ni-1); ++i) {
    szl += I[i].r; szr -= I[i].r;
    if(I[i+1].y == I[i].y) continue;
    int ay1 = I[i].y;
    int ay2 = I[i+1].y - 1;
    int ay3 = ay2+1;
    int ay4 = ay1-1;

    int b = -1;
    int c = -1;
    auto add = [&](int b_, int c_) { if(b_>=b) { b=b_; c=c_; } };

    if(szl - (x2-x1+1) * (ay2-y1+1) <= 0) {
      ay3 = max(ay1, (szl-1)/(x2-x1+1)+y1);
      add(max(0, szr - (x2-x1+1) * (y2-ay3)), ay3);
    }else{
      add(max(0, szl - (x2-x1+1) * (ay2-y1+1)) + max(0, szr - (x2-x1+1) * (y2-ay2)), ay2);
    }

    if(szr - (x2-x1+1) * (y2-ay1) <= 0) {
      ay4 = min(ay2, y2 - (szr-1) / (x2-x1+1) - 1);
      add(max(0, szl - (x2-x1+1) * (ay4-y1+1)), ay4);
    }else{
      add(max(0, szl - (x2-x1+1) * (ay1-y1+1)) + max(0, szr - (x2-x1+1) * (y2-ay1)), ay1);
    }

    if(ay4+1 <= ay3-1) {
      add(max(0, szl - (x2-x1+1) * ((ay4+ay3)/2-y1+1)) + max(0, szr - (x2-x1+1) * (y2-(ay4+ay3)/2)), (ay4+ay3)/2);
    }

    f(b,c);
  }
}

struct timeout{ };

template <typename T>
T rotr(T x, unsigned k) {
    return (x >> k) | (x << (8U * sizeof(T) - k));
}

inline uint64_t hash_int(uint64_t x) noexcept {
  auto h1 = x * (uint64_t)(0xA24BAED4963EE407);
  auto h2 = rotr(x, 32U) * (uint64_t)(0x9FB21C651E98DF25);
  auto h = rotr(h1 + h2, 32U);
  return h;
}

const int HSIZE = 23;
struct { uint64_t key; int value; } H[1<<HSIZE];

inline int peekH(uint64_t key) {
  uint64_t h = hash_int(key);
  h &= (1<<HSIZE)-1;
  for(;;) {
    if(H[h].key == 0) {
      return -1;
    }else{
      if(H[h].key == key) return H[h].value;
      h += 1;
      h &= (1<<HSIZE)-1;
    }
  }
}

inline int getH(uint64_t key, bool& found) {
  uint64_t h = hash_int(key);
  h &= (1<<HSIZE)-1;
  for(;;) {
    if(H[h].key == 0) {
      if(cut_tree_nfree == CUT_TREE_POOLSIZE) throw memory_limit{};
      int value = cut_tree_nfree++;
      H[h].key = key; H[h].value = value;
      return value;
    }else{
      if(H[h].key == key) { found = 1; return H[h].value; }
      h += 1;
      h &= (1<<HSIZE)-1;
    }
  }
}

inline int setH(uint64_t key, int value) {
  uint64_t h = hash_int(key);
  h &= (1<<HSIZE)-1;
  for(;;) {
    if(H[h].key == 0 || H[h].key == key) {
      H[h].key = key;
      H[h].value = value;
      return value;
    }else{
      h += 1;
      h &= (1<<HSIZE)-1;
    }
  }
}

void optX(int x1, int x2, int y1, int y2, ad* I, int ni, int cut, int nl, cut_tree& t){
  static sol S;
  fillSolApprox(S,x1,x2,y1,y2,t);


  int nl2 = distance(I,partition(I,I+nl, [&](auto const& a) { return S[a.ix].x2 == cut; }));
  int nr2 = distance(I,partition(I+nl,I+ni, [&](auto const& a) { return S[a.ix].x1 == cut+1; }));

  int lo = x1, hi = x2;

  float sc0 = 0;
  for(int i = (0); i <= (int)(nl2-1); ++i) { smax(lo,I[i].x); }
  for(int i = (nl2); i <= (int)(nl-1); ++i) { sc0 += score_leaf_min(S[I[i].ix].size(), I[i].r); }
  for(int i = (nl); i <= (int)(nr2-1); ++i) { smin(hi,I[i].x-1); }
  for(int i = (nr2); i <= (int)(ni-1); ++i) { sc0 += score_leaf_min(S[I[i].ix].size(), I[i].r); }

  auto f = [&](int x) -> float {
    float sc = sc0;
    for(int i = (0); i <= (int)(nl2-1); ++i) {
      auto const& s = S[I[i].ix];
      sc += score_leaf_min(s.sizeY() * (x-s.x1+1), I[i].r);
    }
    for(int i = (nl); i <= (int)(nr2-1); ++i) {
      auto const& s = S[I[i].ix];
      sc += score_leaf_min(s.sizeY() * (s.x2-x+1), I[i].r);
    }
    return sc;
  };

  while(hi > lo) {
    int mi = (lo+hi)/2;
    if(f(mi+1) > f(mi)) {
      lo = mi+1;
    }else{
      hi = mi;
    }
  }

  t.sc = f(lo); t.cut = lo;
}

void optY(int x1, int x2, int y1, int y2, ad* I, int ni, int cut, int nl, cut_tree& t){
  static sol S;
  fillSolApprox(S,x1,x2,y1,y2,t);


  int nl2 = distance(I,partition(I,I+nl, [&](auto const& a) { return S[a.ix].y2 == cut; }));
  int nr2 = distance(I,partition(I+nl,I+ni, [&](auto const& a) { return S[a.ix].y1 == cut+1; }));

  int lo = y1, hi = y2;

  float sc0 = 0;
  for(int i = (0); i <= (int)(nl2-1); ++i) { smax(lo,I[i].y); }
  for(int i = (nl2); i <= (int)(nl-1); ++i) { sc0 += score_leaf_min(S[I[i].ix].size(), I[i].r); }
  for(int i = (nl); i <= (int)(nr2-1); ++i) { smin(hi,I[i].y-1); }
  for(int i = (nr2); i <= (int)(ni-1); ++i) { sc0 += score_leaf_min(S[I[i].ix].size(), I[i].r); }

  auto f = [&](int y) -> float {
    float sc = sc0;
    for(int i = (0); i <= (int)(nl2-1); ++i) {
      auto const& s = S[I[i].ix];
      sc += score_leaf_min(s.sizeX() * (y-s.y1+1), I[i].r);
    }
    for(int i = (nl); i <= (int)(nr2-1); ++i) {
      auto const& s = S[I[i].ix];
      sc += score_leaf_min(s.sizeX() * (s.y2-y+1), I[i].r);
    }
    return sc;
  };

  while(hi > lo) {
    int mi = (lo+hi)/2;
    if(f(mi+1) > f(mi)) {
      lo = mi+1;
    }else{
      hi = mi;
    }
  }

  t.sc = f(lo); t.cut = lo;
}

cut_tree_ptr part(int x1, int x2, int y1, int y2, ad* I, int ni) {
  if(ni == 1) {
    ad const& a = I[0];
    float sc = score_leaf(min((x2-x1+1)*(y2-y1+1), a.r), a.r);
    return make_ptr(a.ix, sc);
  }



  uint64_t key = 1 + ((((uint64_t)x1)) | (((uint64_t)x2)<<16) | (((uint64_t)y1)<<32) | (((uint64_t)y2)<<48));
  bool found = 0;
  cut_tree_ptr tix = getH(key, found);
  if(found) return tix;

  tuple<int,int,int> cuts[2] = {{1<<30,-1,-1}, {1<<30,-1,-1}};
  partX(x1,x2,y1,y2, I,ni, [&](int d, int x) { smin(cuts[0], make_tuple(d,0,x)); });
  partY(x1,x2,y1,y2, I,ni, [&](int d, int x) { smin(cuts[1], make_tuple(d,1,x)); });
  if(cuts[0] > cuts[1]) swap(cuts[0],cuts[1]);
  int nc = 0; for(int i = 0; i < (int)(2); ++i) if(get<1>(cuts[i]) != -1) nc++;
  if(ni > B) smin(nc, 1);
  if(get<0>(cuts[0]) == get<0>(cuts[1]) &&
     ((get<1>(cuts[0])) ^ (x2-x1 > y2-y1 ? 1 : 0))) swap(cuts[0],cuts[1]);

  cut_tree& res = deref_ptr(tix); res.l = res.r = -2; res.sc = -1e6;

  for(int ic = 0; ic < (int)(nc); ++ic) {
    int dir,cut; tie(ignore,dir,cut) = cuts[ic];
    if(dir == 0) {
      int nl = distance(I, partition(I, I+ni, [&](ad const& a) { return a.x <= cut; }));
      auto cl = part(x1, cut, y1, y2, I, nl);
      if(res.sc > 0 && nl - deref_ptr(cl).sc > n - curBest) continue;
      if(deref_ptr(cl).sc + (ni-nl) < res.sc) continue;
      auto cr = part(cut+1, x2, y1, y2, I+nl, ni - nl);
      cut_tree t(cl, cr, 0, cut);
      if(n < 100 || startOpt || ni <= B) optX(x1,x2,y1,y2,I,ni,cut,nl,t);
      if(t.sc > res.sc) res = t;
    }else{
      int nl = distance(I, partition(I, I+ni, [&](ad const& a) { return a.y <= cut; }));
      auto cl = part(x1, x2, y1, cut, I, nl);
      if(res.sc > 0 && nl - deref_ptr(cl).sc > n - curBest) continue;
      if(deref_ptr(cl).sc + (ni-nl) < res.sc) continue;
      auto cr = part(x1, x2, cut+1, y2, I+nl, ni - nl);
      cut_tree t(cl, cr, 1, cut);
      if(n < 100 || startOpt || ni <= B) optY(x1,x2,y1,y2,I,ni,cut,nl,t);
      if(t.sc > res.sc) res = t;
    }
  }

  return tix;
}


float fastpart(int x1, int x2, int y1, int y2, ad* I, int ni) {
  if(ni == 1) {
    ad const& a = I[0];
    return score_leaf(min((x2-x1+1)*(y2-y1+1), a.r), a.r);
  }







  tuple<int,int,int> cut0 = {1<<30,-1,-1};
  partX(x1,x2,y1,y2, I,ni, [&](int d, int x) { smin(cut0, make_tuple(d,0,x)); });
  partY(x1,x2,y1,y2, I,ni, [&](int d, int x) { smin(cut0, make_tuple(d,1,x)); });
  { int dir,cut; tie(ignore,dir,cut) = cut0;
    if(dir == 0) {
      int nl = distance(I, partition(I, I+ni, [&](ad const& a) { return a.x <= cut; }));
      return fastpart(x1, cut, y1, y2, I, nl) +
        fastpart(cut+1, x2, y1, y2, I+nl, ni - nl);
    }else{
      int nl = distance(I, partition(I, I+ni, [&](ad const& a) { return a.y <= cut; }));
      return fastpart(x1, x2, y1, cut, I, nl)+
        fastpart(x1, x2, cut+1, y2, I+nl, ni - nl);
    }
  }
}


void localopt(sol& S){
  while(1) {
    bool imp = 0;
    for(int i = 0; i < (int)(n); ++i) if(S[i].size() < A[i].r) {
      if(t_getElapsed() > TL2) break;
      {
        while(S[i].size() < A[i].r) {
          float delta = score_leaf((S[i].x2-S[i].x1+2) * (S[i].y2-S[i].y1+1), A[i].r) - S[i].score(A[i]);
          if(S[i].x1 == 0) goto l_end_x1;
          if(delta <= 1e-6) goto l_end_x1;
          for(int j = 0; j < (int)(n); ++j) if(j != i) {
            if(max(S[i].y1, S[j].y1) <= min(S[i].y2, S[j].y2) && S[i].x1 == S[j].x2+1) {
              if(S[j].x2 == A[j].x) goto l_end_x1;
              delta += score_leaf((S[j].x2-S[j].x1) * (S[j].y2-S[j].y1+1), A[j].r) - S[j].score(A[j]);
              if(delta <= 1e-6) goto l_end_x1;
            }
          }
          imp = 1;
          for(int j = 0; j < (int)(n); ++j) if(j != i) {
            if(max(S[i].y1, S[j].y1) <= min(S[i].y2, S[j].y2) && S[i].x1 == S[j].x2+1) {
              S[j].x2 -= 1;
            }
          }
          S[i].x1 -= 1;
        }
        l_end_x1:;
      }

      {
        while(S[i].size() < A[i].r) {
          float delta = score_leaf((S[i].x2-S[i].x1+2) * (S[i].y2-S[i].y1+1), A[i].r) - S[i].score(A[i]);
          if(S[i].x2 == M) goto l_end_x2;
          if(delta <= 1e-6) goto l_end_x2;
          for(int j = 0; j < (int)(n); ++j) if(j != i) {
            if(max(S[i].y1, S[j].y1) <= min(S[i].y2, S[j].y2) && S[i].x2+1 == S[j].x1) {
              if(S[j].x1 == A[j].x) goto l_end_x2;
              delta += score_leaf((S[j].x2-S[j].x1) * (S[j].y2-S[j].y1+1), A[j].r) - S[j].score(A[j]);
              if(delta <= 1e-6) goto l_end_x2;
            }
          }
          imp = 1;
          for(int j = 0; j < (int)(n); ++j) if(j != i) {
            if(max(S[i].y1, S[j].y1) <= min(S[i].y2, S[j].y2) && S[i].x2+1 == S[j].x1) {
              S[j].x1 += 1;
            }
          }
          S[i].x2 += 1;
        }
      l_end_x2:;
      }

      {
        while(S[i].size() < A[i].r) {
          float delta = score_leaf((S[i].y2-S[i].y1+2) * (S[i].x2-S[i].x1+1), A[i].r) - S[i].score(A[i]);
          if(S[i].y1 == 0) goto l_end_y1;
          if(delta <= 1e-6) goto l_end_y1;
          for(int j = 0; j < (int)(n); ++j) if(j != i) {
            if(max(S[i].x1, S[j].x1) <= min(S[i].x2, S[j].x2) && S[i].y1 == S[j].y2+1) {
              if(S[j].y2 == A[j].y) goto l_end_y1;
              delta += score_leaf((S[j].y2-S[j].y1) * (S[j].x2-S[j].x1+1), A[j].r) - S[j].score(A[j]);
              if(delta <= 1e-6) goto l_end_y1;
            }
          }
          imp = 1;
          for(int j = 0; j < (int)(n); ++j) if(j != i) {
            if(max(S[i].x1, S[j].x1) <= min(S[i].x2, S[j].x2) && S[i].y1 == S[j].y2+1) {
              S[j].y2 -= 1;
            }
          }
          S[i].y1 -= 1;
        }
      l_end_y1:;
      }

      {
        while(S[i].size() < A[i].r) {
          float delta = score_leaf((S[i].y2-S[i].y1+2) * (S[i].x2-S[i].x1+1), A[i].r) - S[i].score(A[i]);
          if(S[i].y2 == M) goto l_end_y2;
          if(delta <= 1e-6) goto l_end_y2;
          for(int j = 0; j < (int)(n); ++j) if(j != i) {
            if(max(S[i].x1, S[j].x1) <= min(S[i].x2, S[j].x2) && S[i].y2+1 == S[j].y1) {
              if(S[j].y1 == A[j].y) goto l_end_y2;
              delta += score_leaf((S[j].y2-S[j].y1) * (S[j].x2-S[j].x1+1), A[j].r) - S[j].score(A[j]);
              if(delta <= 1e-6) goto l_end_y2;
            }
          }
          imp = 1;
          for(int j = 0; j < (int)(n); ++j) if(j != i) {
            if(max(S[i].x1, S[j].x1) <= min(S[i].x2, S[j].x2) && S[i].y2+1 == S[j].y1) {
              S[j].y1 += 1;
            }
          }
          S[i].y2 += 1;
        }
      l_end_y2:;
      }

    }
    if(!imp) break;
  }
}

const double vbeta = 2.0;
const int nbeta = 32;
double pbeta[nbeta];

int nsolve_tree = 0;
struct solve_tree;
using solve_tree_ptr = solve_tree*;
hash_map<uint64_t, solve_tree_ptr> solve_tree_map;

solve_tree_ptr get_solve_tree(int x1_, int x2_, int y1_, int y2_, ad* I_, int ni_);

struct solve_tree {
  int x1, x2, y1, y2;
  vector<ad> I;

  bool ready, done;

  using cut_t = tuple<float, int, int, solve_tree_ptr, solve_tree_ptr>;
  vector<cut_t> cuts;
  min_queue<tuple<float, int>> Q;
  int qsize = 0;

  cut_tree_ptr ct;

  solve_tree(int x1_, int x2_, int y1_, int y2_, ad* I_, int ni_) {
    nsolve_tree += 1;
    x1 = x1_; x2 = x2_; y1 = y1_; y2 = y2_;
    I.reserve(ni_); for(int i = 0; i < (int)(ni_); ++i) I.push_back(I_[i]);

    ready = false;
    done = false;

    ct = part(x1,x2,y1,y2,I_,ni_);

    if(I.size() <= 3) { done = 1; }
  }

  vector<cut_tree> trees() const {
    vector<cut_tree> ts;
    for(int i = 0; i < (int)(cuts.size()); ++i) {
      int dir,cut; solve_tree_ptr tl, tr; tie(ignore,dir,cut,tl,tr) = cuts[i];
      if(tl) {
        ts.emplace_back(tl->ct,tr->ct,dir,cut);
      }
    }
    pdqsort(begin(ts), end(ts), [&](auto const& a, auto const& b) { return a.sc > b.sc; });
    return ts;
  }

  void prepare(bool root = false) {
    ready = 1;

    cuts.reserve(2*(I.size()-1));
    partX(x1,x2, y1,y2, I.data(),I.size(), [&](int d, int cut) {
      cuts.emplace_back(d,0,cut, nullptr, nullptr);
    });
    partY(x1,x2, y1,y2, I.data(),I.size(), [&](int d, int cut) {
      cuts.emplace_back(d,1,cut, nullptr, nullptr);
    });
    pdqsort(begin(cuts), end(cuts));

    if(cuts.size() > nbeta && !root) cuts.resize(nbeta);

    for(auto &p : cuts) {
      int dir, cut; tie(ignore, dir, cut, ignore, ignore) = p;
      float sc;
      if(dir == 0) {
        int nl = distance(begin(I), partition(begin(I), end(I), [&](ad const& a) { return a.x <= cut; }));
        sc = fastpart(x1, cut, y1, y2, I.data(), nl) +
          fastpart(cut+1, x2, y1, y2, I.data()+nl, I.size() - nl);
      }else{
        int nl = distance(begin(I), partition(begin(I), end(I), [&](ad const& a) { return a.y <= cut; }));
        sc = fastpart(x1, x2, y1, cut, I.data(), nl)+
          fastpart(x1, x2, cut+1, y2, I.data()+nl, I.size() - nl);
      }
      p = make_tuple(sc, dir, cut, nullptr, nullptr);
    }
    pdqsort(begin(cuts), end(cuts), greater<>());

    qsize = min<int>(cuts.size(),nbeta);
    for(int i = 0; i < (int)(qsize); ++i) Q.push(make_tuple(1+i, i));
  }

  void testImprovement(int const& oldtl, int const& oldtr, int const& dir, int const& cut, solve_tree_ptr& tl, solve_tree_ptr& tr, cut_t& c) {
    cut_tree t(tl->ct, tr->ct, dir, cut);

    if(startOpt && (oldtl != tl->ct || oldtr != tr->ct)) {
      if(dir == 0) {
        int nl = distance(begin(I), partition(begin(I), end(I), [&](ad const& a) { return a.x <= cut; }));
        optX(x1,x2,y1,y2,I.data(),I.size(),cut,nl,t);
      }else{
        int nl = distance(begin(I), partition(begin(I), end(I), [&](ad const& a) { return a.y <= cut; }));
        optY(x1,x2,y1,y2,I.data(),I.size(),cut,nl,t);
      }
    }

    if(t.sc > deref_ptr(ct).sc) {
      ct = make_ptr(t);
      uint64_t key = 1 + ((((uint64_t)x1)) | (((uint64_t)x2)<<16) | (((uint64_t)y1)<<32) | (((uint64_t)y2)<<48));
      setH(key, ct);
    }
    c = make_tuple(t.sc,dir,cut,tl,tr);
  };


  bool search() {
    if(deref_ptr(ct).sc > I.size()-1e-6) { done = true; return false; }
    if(done) return false;
    if(t_getElapsed() > TL1) throw timeout{};
    if(!ready) prepare();
    while(!Q.empty()) {
      if(t_getElapsed() > TL1) throw timeout{};
      float d; int i; tie(d,i) = Q.top(); Q.pop();
      if(i >= qsize) continue;

      int dir,cut; solve_tree_ptr tl, tr; tie(ignore,dir,cut,tl,tr) = cuts[i];
      int oldtl = -1, oldtr = -1;

      if(tl) {
        if(tl->done && tr->done) {
          testImprovement(oldtl, oldtr, dir, cut, tl, tr, cuts[i]);
          for(int j = i; j+1 < (int)cuts.size(); ++j) {
            swap(cuts[j],cuts[j+1]);
          }
          qsize -= 1;
          Q.push(make_tuple(d+pbeta[i],i)); continue;
        }

        oldtl = tl->ct; oldtr = tr->ct;
        tl->search();
        tr->search();
      }else{
        if(dir == 0) {
          int nl = distance(begin(I), partition(begin(I), end(I), [&](ad const& a) { return a.x <= cut; }));
          tl = get_solve_tree(x1,cut, y1,y2, I.data(), nl);
          tr = get_solve_tree(cut+1,x2, y1,y2, I.data()+nl, I.size()-nl);
        }else{
          int nl = distance(begin(I), partition(begin(I), end(I), [&](ad const& a) { return a.y <= cut; }));
          tl = get_solve_tree(x1,x2, y1,cut, I.data(), nl);
          tr = get_solve_tree(x1,x2, cut+1,y2, I.data()+nl, I.size()-nl);
        }
      }

      testImprovement(oldtl, oldtr, dir, cut, tl, tr, cuts[i]);

      for(int j = i-1; j>=0; --j) {
        if(get<0>(cuts[j]) < get<0>(cuts[j+1])) swap(cuts[j],cuts[j+1]);
        else break;
      }

      Q.push(make_tuple(d+pbeta[i],i));
      return true;
    }
    done = true;
    return false;
  }
};

solve_tree_ptr get_solve_tree(int x1, int x2, int y1, int y2, ad* I, int ni) {
  uint64_t key = 1 + ((((uint64_t)x1)) | (((uint64_t)x2)<<16) | (((uint64_t)y1)<<32) | (((uint64_t)y2)<<48));
  auto it = solve_tree_map.find(key);
  if(it != end(solve_tree_map)) return it->second;
  return solve_tree_map[key] = new solve_tree(x1,x2,y1,y2,I,ni);
}

void solve(){
  static ad I[N]; for(int i = 0; i < (int)(n); ++i) I[i] = A[i];
  solve_tree t(0,M, 0,M, I,n);
  t.prepare(true);

  try {
    int niter = 0;
    while(1) {
      if(t_getElapsed() > TL1/2) startOpt = 1;
      niter++;
      if(!t.search()) break;
      if(deref_ptr(t.ct).sc > curBest){
        curBest = deref_ptr(t.ct).sc;

      }
    }
  }catch(timeout) {
  }catch(memory_limit){
  }

  do { cerr << "main.cpp" ":" "901" "  {" << "t_getElapsed()" << "} = {"; debug_impl_seq(t_getElapsed()); cerr << endl; } while(0);;

  vector<cut_tree> trees2 = t.trees();
  vector<float> scs;
  for(auto const& t : trees2) {
    scs.emplace_back(t.sc);
  }
  do { cerr << "main.cpp" ":" "908" "  {" << "scs" << "} = {"; debug_impl_seq(scs); cerr << endl; } while(0);;

  sol bestSol; for(int i = 0; i < (int)(n); ++i) bestSol.S[i].from_ad(A[i]); bestSol.calc();
  for(auto const& t : trees2) {
    if(t_getElapsed() > TL2) break;
    { sol S;
      opt_cut_tree(S, t);
      localopt(S);
      S.calc();
      if(S.sc > bestSol.sc) bestSol = S;
    }
    { sol S;
      fillSol(S, 0,M,0,M,t);
      localopt(S);
      S.calc();
      if(S.sc > bestSol.sc) bestSol = S;
    }
  }

  bestSol.calc();
  for(int i = 0; i < (int)(n); ++i) bestSol[i].pr();
  cerr << "score = " << (int)round(1e9 * bestSol.sc / (float)n) << endl;
  do { cerr << "main.cpp" ":" "930" "  {" << "nsolve_tree" << "} = {"; debug_impl_seq(nsolve_tree); cerr << endl; } while(0);;
  do { cerr << "main.cpp" ":" "931" "  {" << "solve_tree_map.size()" << "} = {"; debug_impl_seq(solve_tree_map.size()); cerr << endl; } while(0);;

}


int main() {
  for(int i = 0; i < (int)(nbeta); ++i) pbeta[i] = pow(1 + i, vbeta);
  t_begin = chrono::high_resolution_clock::now();
  read();
  solve();
  return 0;
}
