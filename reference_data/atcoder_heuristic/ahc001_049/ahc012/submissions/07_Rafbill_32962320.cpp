





 #pragma GCC optimize ("O3,inline,omit-frame-pointer,no-asynchronous-unwind-tables,fast-math")
 #include <functional>
 #include <cmath>
 #include <algorithm>
 #include <numeric>
 #include <queue>
 #include <vector>
 #include <map>
 #include <set>
 #include <chrono>
 #include <iostream>
 #include <cstring>
 #include <cstdarg>
 #include <climits>
 #include <bitset>
 #include <stdio.h>
 #include <sys/types.h>
 #include <ifaddrs.h>
 #include <netinet/in.h>
 #include <unistd.h>
 #include <sys/socket.h>
 #include <sys/time.h>
 #include <sys/resource.h>
 #include <sys/stat.h>
 #include <signal.h>
 #include <netdb.h>
 #include <cstdint>
 #include <cinttypes>
 #include <x86intrin.h>



namespace std
{
  typedef long unsigned int size_t;
  typedef long int ptrdiff_t;


  typedef decltype(nullptr) nullptr_t;

}
namespace std
{
  inline namespace __cxx11 __attribute__((__abi_tag__ ("cxx11"))) { }
}
namespace __gnu_cxx
{
  inline namespace __cxx11 __attribute__((__abi_tag__ ("cxx11"))) { }
}


extern "C" {


extern void __assert_fail (const char *__assertion, const char *__file,
      unsigned int __line, const char *__function)
     noexcept (true) __attribute__ ((__noreturn__));


extern void __assert_perror_fail (int __errnum, const char *__file,
      unsigned int __line, const char *__function)
     noexcept (true) __attribute__ ((__noreturn__));




extern void __assert (const char *__assertion, const char *__file, int __line)
     noexcept (true) __attribute__ ((__noreturn__));


}
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

        int limit = 0;
        for (Iter cur = begin + 1; cur != end; ++cur) {
            if (limit > partial_insertion_sort_limit) return false;

            Iter sift = cur;
            Iter sift_1 = cur - 1;


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
        int unknown_left = (last - first) - ((num_r || num_l) ? block_size : 0);
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
template<class T>
void print_collection(ostream& out, T const& x);

template<class A>
ostream& operator<<(ostream& out, vector<A> const& x) { print_collection(out, x); return out; }
template<class A, size_t N>
ostream& operator<<(ostream& out, array<A, N> const& x) { print_collection(out, x); return out; }
template<class A>
ostream& operator<<(ostream& out, deque<A> const& x) { print_collection(out, x); return out; }
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



thread_local static struct xorshift128_t {
  uint32_t x, y, z, w;

  xorshift128_t(uint64_t seed = __rdtsc()) {
    reset(seed);
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


  using result_type = uint32_t;
  constexpr uint32_t min(){ return numeric_limits<uint32_t>::min(); }
  constexpr uint32_t max(){ return numeric_limits<uint32_t>::max(); }
  uint32_t operator()() { return randomInt32(); }

  long long int operator()(long long int m) { return random(m); }

  inline uint32_t randomInt32() {
    return xorshift128();
  }

  inline uint64_t randomInt64() {
    return (((uint64_t)randomInt32())<<32ll) | ((uint64_t)randomInt32());
  }

  inline long long int random(long long int r) {
    return randomInt64()%r;
  }

  inline long long int random(long long int l, long long int r) {
    return l + random(r-l+1);
  }

  inline double randomDouble() {
    return (double)randomInt32() / 4294967296.0;
  }

  inline float randomFloat() {
    return (float)randomInt32() / 4294967296.0;
  }
} RNG;



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




static inline void debug_impl_seq() {
  cerr << "}";
}

template <class T, class... V>
void debug_impl_seq(T const& t, V const&... v) {
  cerr << t;
  if(sizeof...(v)) { cerr << ", "; }
  debug_impl_seq(v...);
}

const double TL = 2.9;
timer TM;

using scalar = long long int;

struct pt {
  pt(){}
  pt(scalar x_, scalar y_) : x(x_), y(y_) { }

  bool operator==(pt const& a) const { return tie(x, y) == tie(a.x,a.y); }
  bool operator!=(pt const& a) const { return tie(x, y) != tie(a.x,a.y); }
  bool operator<(pt const& a) const { return tie(x, y) < tie(a.x,a.y); }
  bool operator<=(pt const& a) const { return tie(x, y) <= tie(a.x,a.y); }
  bool operator>(pt const& a) const { return tie(x, y) > tie(a.x,a.y); }
  bool operator>=(pt const& a) const { return tie(x, y) >= tie(a.x,a.y); }

  pt& operator+=(pt const& a) { x += a.x; y += a.y; return *this; }
  pt operator+(pt const& a) const { pt b = *this; b += a; return b; }
  pt& operator-=(pt const& a) { x -= a.x; y -= a.y; return *this; }
  pt operator-(pt const& a) const { pt b = *this; b -= a; return b; }
  pt& operator/=(scalar const& a) { x /= a; y /= a; return *this; }
  pt operator/(scalar const& a) const { pt b = *this; b /= a; return b; }

  scalar dot(pt const& a) const { return x*a.x+y*a.y; }
  scalar norm2() const { return x*x+y*y; }
  scalar det(pt const& a) const { return x*a.y - y*a.x; }

  scalar x, y;
};

inline pt operator*(scalar const& a, pt const& b) {
  return pt(a*b.x, a*b.y);
}

struct angle_cmp {
  pt u, uv;
  angle_cmp(pt u_, pt v_) : u(u_), uv(v_-u_) { }

  bool operator()(pt const& a, pt const& b) const {
    pt da = a - u, db = b - u;
    auto detb = uv.det(db);
    if (detb == 0 && db.dot(uv) >= 0) return false;
    auto deta = uv.det(da);
    if (deta == 0 && da.dot(uv) >= 0) return true;
    if (deta * detb >= 0) {
      return da.det(db) > 0;
    }
    return deta > 0;
  }
};

ostream& operator<<(ostream& os, pt const& p) {
  return os << make_tuple(p.x,p.y);
}

const int N = 5500;
const int M = 10;
const int K = 100;
int n,k;
int A[M+1];
pt B[N];

int suma;

int best_sc = 0;
vector<tuple<pt,pt>> best_sol;


void read(){
  cin>>n>>k;
  for(int i = (1); i <= (int)(M); ++i) cin>>A[i];
  for(int i = 0; i < (int)(n); ++i) {
    int x,y; cin>>x>>y;
    B[i] = pt(x,y);
  }
  suma = 0;
  for(int i = (1); i <= (int)(M); ++i) suma += A[i];
}

struct state {
  int B[M+1];

  int fromx, fromy;

  int score;
  int waste;
  int remaining;

  void clear(){
    score = -1e9;
    waste = 1e9;
    remaining = n;
    fromx = -1; fromy = -1;
    for(int i = (0); i <= (int)(M); ++i) B[i] = A[i];
  }

  void reset(){
    score = 0;
    waste = 0;
    remaining = n;
    fromx = -1; fromy = -1;
    for(int i = (0); i <= (int)(M); ++i) B[i] = A[i];
  }

  int heuristic() const {
    return waste;
  }

  int possible() const {
    int sc = score;
    int rem = remaining;
    for(int i = (1); i <= (int)(M); ++i) {
      int d = max(0,min(rem/i,B[i]));
      rem -= d*i;
      sc += d;
    }
    return sc;
  }
};

template<class F>
void opt_cuts(vector<int> const& I, vector<int> const& J, vector<int> cuti, vector<int> cutj, F&& f) {
  vector<int> RI(n); for(int i = 0; i < (int)(n); ++i) RI[I[i]] = i;
  vector<int> RJ(n); for(int i = 0; i < (int)(n); ++i) RJ[J[i]] = i;

  int nci = cuti.size();
  int ncj = cutj.size();

  static int B[M+1];
  static int CNT[K+1][K+1];
  static int ATI[N];
  static int ATJ[N];
  for(int i = 0; i < (int)(nci+1); ++i) for(int j = 0; j < (int)(ncj+1); ++j) CNT[i][j] = 0;
  for(int i = 0; i < (int)(n); ++i) ATI[i] = 0; for(int i = 0; i < (int)(nci); ++i) for(int j = (cuti[i]+1); j <= (int)(i+1<nci?cuti[i+1]:n-1); ++j) ATI[I[j]] = i+1;
  for(int i = 0; i < (int)(n); ++i) ATJ[i] = 0; for(int i = 0; i < (int)(ncj); ++i) for(int j = (cutj[i]+1); j <= (int)(i+1<ncj?cutj[i+1]:n-1); ++j) ATJ[J[j]] = i+1;

  for(int i = 0; i < (int)(M+1); ++i) B[i] = 0;
  for(int i = 0; i < (int)(n); ++i) CNT[ATI[i]][ATJ[i]] += 1;
  for(int i = 0; i < (int)(nci+1); ++i) for(int j = 0; j < (int)(ncj+1); ++j) if(CNT[i][j] <= 10) B[CNT[i][j]] += 1;
  int score = 0;
  for(int i = 0; i < (int)(M+1); ++i) score += min(A[i],B[i]);

  int oscore = score;
  do { cerr << "main.cpp" ":" "146" "  {" << "score" << "} = {"; debug_impl_seq(score); cerr << endl << flush; } while(0);

  float MAX_ITER = 1'000'000;
  for(int iter = 0; iter < (int)(MAX_ITER); ++iter) {
    if(iter % 100000 == 0) do { cerr << "main.cpp" ":" "150" "  {" << "iter, score" << "} = {"; debug_impl_seq(iter, score); cerr << endl << flush; } while(0);
    if(RNG.random(2)) {
      int i = RNG.random(nci);
      int d = RNG.random(2);
      if(d == 0) {
        if((i == 0 && cuti[i] == 0) || (i > 0 && cuti[i]-1 < cuti[i-1])) continue;
        int x = I[cuti[i]];

        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
        CNT[ATI[x]][ATJ[x]] -= 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        ATI[x] += 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
        CNT[ATI[x]][ATJ[x]] += 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        cuti[i] -= 1;

        int nscore = 0;
        for(int i = 0; i < (int)(M+1); ++i) nscore += min(A[i],B[i]);
        int delta = score - nscore;
        if(nscore >= score) {
          score = nscore;
        }else{
          cuti[i] += 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
          CNT[ATI[x]][ATJ[x]] -= 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
          ATI[x] -= 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
          CNT[ATI[x]][ATJ[x]] += 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        }

      }else{
        if((i == nci-1 && cuti[i] == n-2) || (i < nci-1 && cuti[i]+1 > cuti[i+1])) continue;
        int x = I[cuti[i]+1];

        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
        CNT[ATI[x]][ATJ[x]] -= 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        ATI[x] -= 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
        CNT[ATI[x]][ATJ[x]] += 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        cuti[i] += 1;

        int nscore = 0;
        for(int i = 0; i < (int)(M+1); ++i) nscore += min(A[i],B[i]);
        int delta = score - nscore;
        if(nscore >= score) {
          score = nscore;
        }else{
          cuti[i] -= 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
          CNT[ATI[x]][ATJ[x]] -= 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
          ATI[x] += 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
          CNT[ATI[x]][ATJ[x]] += 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        }

      }
    }else{
      int i = RNG.random(ncj);
      int d = RNG.random(2);
      if(d == 0) {
        if((i == 0 && cutj[i] == 0) || (i > 0 && cutj[i]-1 < cutj[i-1])) continue;
        int x = J[cutj[i]];

        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
        CNT[ATI[x]][ATJ[x]] -= 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        ATJ[x] += 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
        CNT[ATI[x]][ATJ[x]] += 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        cutj[i] -= 1;

        int nscore = 0;
        for(int i = 0; i < (int)(M+1); ++i) nscore += min(A[i],B[i]);
        int delta = score - nscore;
        if(nscore >= score) {
          score = nscore;
        }else{
          cutj[i] += 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
          CNT[ATI[x]][ATJ[x]] -= 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
          ATJ[x] -= 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
          CNT[ATI[x]][ATJ[x]] += 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        }

      }else{
        if((i == ncj-1 && cutj[i] == n-2) || (i < ncj-1 && cutj[i]+1 > cutj[i+1])) continue;
        int x = J[cutj[i]+1];

        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
        CNT[ATI[x]][ATJ[x]] -= 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        ATJ[x] -= 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
        CNT[ATI[x]][ATJ[x]] += 1;
        if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        cutj[i] += 1;

        int nscore = 0;
        for(int i = 0; i < (int)(M+1); ++i) nscore += min(A[i],B[i]);
        int delta = score - nscore;
        if(nscore >= score) {
          score = nscore;
        }else{
          cutj[i] -= 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
          CNT[ATI[x]][ATJ[x]] -= 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
          ATJ[x] += 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] -= 1;
          CNT[ATI[x]][ATJ[x]] += 1;
          if(CNT[ATI[x]][ATJ[x]] <= 10) B[CNT[ATI[x]][ATJ[x]]] += 1;
        }

      }

    }
  }

  do { cerr << "main.cpp" ":" "279" "  {" << "oscore, score" << "} = {"; debug_impl_seq(oscore, score); cerr << endl << flush; } while(0);

  f(score,cuti,cutj);
}

template<class F>
void solve_inner(vector<int> const& I, vector<int> const& J, F&& f) {
  vector<int> RJ(n); for(int i = 0; i < (int)(n); ++i) RJ[J[i]] = i;
  int nc = 10 + RNG.random(50);
  nc = n / (n / nc);
  vector<int> col(n);
  for(int i = 0; i < (int)(n); ++i) col[RJ[I[i]]] = i / (n / nc);
  vector<int> cuti;
  for(int i = 0; i < (int)(nc); ++i) if((i+1)*(n/nc) + 1 < n) cuti.emplace_back((i+1)*(n/nc)-1);
  do { cerr << "main.cpp" ":" "293" "  {" << "nc" << "} = {"; debug_impl_seq(nc); cerr << endl << flush; } while(0);

  int rk = k - nc;
  static state S[N+1][K+2];
  for(int i = 0; i < (int)(n+1); ++i) for(int j = 0; j < (int)(rk+2); ++j) S[i][j].clear();
  S[0][0].reset();
  vector<int> cnt(nc+2);

  state best; best.reset();



  for(int j = 0; j < (int)(rk+1); ++j) {
    if(TM.elapsed() > TL) return;
    for(int i = 0; i < (int)(n); ++i) {
      state T = S[i][j];
      if(T.score < 0) continue;

      T.fromx = i; T.fromy = j;
      cnt.assign(nc+2,0);
      int to = min(n,i + nc * 4);
      for(int l = i; l < to; ++l) {

        if(cnt[col[l]] > 0) {
          if(cnt[col[l]] <= 10) {
            T.B[cnt[col[l]]] += 1;
            if(T.B[cnt[col[l]]] > 0) T.score -= 1;
            else T.waste -= cnt[col[l]];
          }else{
            T.waste -= cnt[col[l]];
          }
        }
        cnt[col[l]] += 1;
        if(cnt[col[l]] <= 10) {
          if(T.B[cnt[col[l]]] > 0) T.score += 1;
          else T.waste += cnt[col[l]];
          T.B[cnt[col[l]]] -= 1;
        }else{
          T.waste += cnt[col[l]];
        }
        T.remaining -= 1;
        if(T.heuristic() < S[l+1][j+1].heuristic()) {
          S[l+1][j+1] = T;
        }
      }
    }
  }


  int bi = 0, bj = 0, bsc = 0;
  for(int j = 0; j < (int)(rk+2); ++j) if(S[n][j].score > bsc) {
    bi = n; bj = j; bsc = S[n][j].score;
  }
  if(bsc == 0) return;
  vector<int> cutj;
  while(bi != 0) {
    state const& T = S[bi][bj];
    bi = T.fromx; bj = T.fromy;
    if(bi > 0) cutj.emplace_back(bi-1);
  }
  reverse(begin(cutj), end(cutj));
  opt_cuts(I,J,cuti,cutj,f);
}

void solve(){
  int niter = 0;

  while(TM.elapsed() < TL) {
    pt ab1 = pt(RNG.random(1000), RNG.random(1000));
    pt ab2 = pt(RNG.random(1000), RNG.random(1000));
    vector<int> I(n),J(n);
    iota(begin(I), end(I),0); pdqsort(begin(I), end(I),[&](int i, int j){ return B[i].dot(ab1) < B[j].dot(ab1); });
    iota(begin(J), end(J),0); pdqsort(begin(J), end(J),[&](int i, int j){ return B[i].dot(ab2) < B[j].dot(ab2); });
    { bool ok = 1;
      for(int i = 0; i < (int)(n-1); ++i) if(B[I[i]].dot(ab1) == B[I[i+1]].dot(ab1)) ok = 0;
      for(int i = 0; i < (int)(n-1); ++i) if(B[J[i]].dot(ab2) == B[J[i+1]].dot(ab2)) ok = 0;
      if(!ok) continue;
    }
    solve_inner(I,J, [&](int sc, vector<int> const& cuti, vector<int> const& cutj) -> void {
      if(sc > best_sc) {
        vector<tuple<pt,pt>> sol;
        for(int i : cuti) {
          auto a = B[I[i]] + 100000*pt(ab1.y,-ab1.x);
          auto b = B[I[i+1]] - 100000*pt(ab1.y,-ab1.x);
          sol.emplace_back(a,b);
        }
        for(int i : cutj) {
          auto a = B[J[i]] + 100000*pt(ab2.y,-ab2.x);
          auto b = B[J[i+1]] - 100000*pt(ab2.y,-ab2.x);
          sol.emplace_back(a,b);
        }
        best_sc = sc;
        best_sol = sol;
      }
    });
    niter += 1;
  }
  do { cerr << "main.cpp" ":" "390" "  {" << "niter" << "} = {"; debug_impl_seq(niter); cerr << endl << flush; } while(0);

  do { cerr << "main.cpp" ":" "392" "  {" << "1000000.0 * best_sc / suma" << "} = {"; debug_impl_seq(1000000.0 * best_sc / suma); cerr << endl << flush; } while(0);
  cout << best_sol.size() << endl;
  for(auto pq : best_sol){
    pt p,q; tie(p,q) = pq;
    cout << p.x << " " << p.y << " " << q.x << " " << q.y << endl;
  }
}

int main() {
  TM.reset();
  read();
  solve();

  return 0;
}
