





 #pragma GCC optimize ("O3,inline,omit-frame-pointer,no-asynchronous-unwind-tables,fast-math")
 #include <functional>
 #include <cmath>
 #include <algorithm>
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








const double TL1 = 2.4;
const double TL2 = 2.9;


timer TM;
const char* printchars[] = {u8"X",u8"╴",u8"╵",u8"┘",u8"╶",u8"─",u8"└",u8"┴",u8"╷",u8"┐",u8"│",u8"┤",u8"┌",u8"┬",u8"├",u8"┼"};

const int N = 10;
const int NN = N*N;

int n,nn;

uint8_t A0[N][N];
uint8_t A[8][NN];

void read(){
  cin>>n; nn = n*n;
  int t; cin>>t;
  for(int i = 0; i < (int)(n); ++i) for(int j = 0; j < (int)(n); ++j) {
    char c; cin>>c;
    if('0' <= c && c <= '9') A0[i][j] = c-'0';
    else A0[i][j] = 10+c-'a';
  }
}

int sym_dir(int sym, int d) {

  if(sym & 1) {
    d ^= 1;
  }
  if(sym & 2) {
    if(d&1) d ^= 2;
  }
  if(sym & 3) {
    if(!(d&1)) d ^= 2;
  }
  return d;
}

int unsym_dir(int sym, int d) {
  if(sym & 3) {
    if(!(d&1)) d ^= 2;
  }
  if(sym & 2) {
    if(d&1) d ^= 2;
  }
  if(sym & 1) {
    d ^= 1;
  }
  return d;
}

int sym_tile(int sym, int a) {
  int b = 0;
  for(int d = 0; d < (int)(4); ++d) if(a&(1<<d)) b |= (1<<sym_dir(sym,d));
  return b;
}

tuple<int,int> sym_xy(int sym, int x, int y) {
  if(sym&1) swap(x,y);
  if(sym&2) x = n-1-x;
  if(sym&3) y = n-1-y;
  return make_tuple(x,y);
}

int layerof[NN];
int index_of[NN];
int at[N][N];

int G[NN][4];
int GH[NN], GV[NN];
int Acnt[8][16];
int Gdist[NN][NN];

vector<int> best_solution;


struct UF {
  uint8_t A[NN];
  void reset() { for(int i = 0; i < (int)(nn); ++i) { A[i]=i; }}
  inline int find(int x){
    int p = A[x];
    while(p != A[p]) p = A[p];
    while(x != p) { int y = A[x]; A[x] = p; x = y; }
    return p;
  }
  inline bool unite(int i, int j){
    i=find(i); j=find(j);
    if(i == j) return false;
    A[i] = j;
    return true;
  }
};

struct UF2 {
  uint8_t A[NN];
  uint8_t E[NN];
  void reset() { for(int i = 0; i < (int)(nn); ++i) { A[i]=i; E[i]=0; }}
  inline int find(int x) {
    int p = A[x];
    while(p != A[p]) p = A[p];
    while(x != p) { int y = A[x]; A[x] = p; x = y; }
    return p;
  }
  inline void add_edge(int i, int j){
    E[find(i)] += 1;
    E[find(j)] += 1;
  }
  inline bool unite(int i, int j){
    i=find(i); j=find(j);
    if(i == j) return false;
    A[i] = j;
    E[j] += E[i]-2;
    return true;
  }
  inline bool remove_edge(int i, int j){
    i = find(i); j = find(j);
    E[i] -= 1;
    E[j] -= 1;
    return E[i]>0 && E[j]>0;
  }
};

struct cost1_s {
  uint8_t cost[NN][NN];
  uint8_t dir[NN][NN];
};

struct cost2_s {
  uint8_t cost[NN][NN][NN];
  uint8_t dir[NN][NN][NN];
};

struct cost3_s {
  uint8_t cost[64][64][64][64];
  uint8_t dir[64][64][64][64];
};

struct cost4_s {
  uint8_t cost[25][25][25][25][25];
  uint8_t dir[25][25][25][25][25];
};

struct group {
  int sz;
  int elements[4];
  cost1_s* cost1 = 0;
  cost2_s* cost2 = 0;
  cost3_s* cost3 = 0;
  cost4_s* cost4 = 0;

  int& operator[](int ix) { return elements[ix]; }
  int const& operator[](int ix) const { return elements[ix]; }
};

vector<group> groups;
vector<int> cells;

void add_group1(int x) {
  groups.emplace_back();
  auto& g = groups.back();
  g.sz = 1;
  g[0] = x;
  cells.emplace_back(x);
};

void add_group2(int x, int y) {
  groups.emplace_back();
  auto& g = groups.back();
  g.sz = 2;
  g[0] = x;
  g[1] = y;
  cells.emplace_back(x);
  cells.emplace_back(y);
};

void add_group3(int x, int y, int z) {
  groups.emplace_back();
  auto& g = groups.back();
  g.sz = 3;
  g[0] = x;
  g[1] = y;
  g[2] = z;
  cells.emplace_back(x);
  cells.emplace_back(y);
  cells.emplace_back(z);
};

void add_group4(int x, int y, int z, int w) {
  groups.emplace_back();
  auto& g = groups.back();
  g.sz = 4;
  g[0] = x;
  g[1] = y;
  g[2] = z;
  g[3] = w;
  cells.emplace_back(x);
  cells.emplace_back(y);
  cells.emplace_back(z);
  cells.emplace_back(w);
};

bool pre_fixed[NN];

void compute_cost1(group& g) {
  cost1_s& cost = *g.cost1;
  for(int y = 0; y < (int)(nn); ++y) for(int z = 0; z < (int)(nn); ++z) cost.cost[y][z] = 255;
  for(int y = 0; y < (int)(nn); ++y) for(int z = 0; z < (int)(nn); ++z) cost.dir[y][z] = 0;

  static tuple<uint8_t,uint8_t> Q[NN*NN];
  int nq = 0;

  for(int z = 0; z < (int)(nn); ++z) if(!pre_fixed[z]) {
    cost.cost[g[0]][z] = 0;
    cost.dir[g[0]][z] = 0;
    Q[nq++] = make_tuple(g[0],z);
  }

  int iq = 0;
  while(iq < nq) {
    int y,z; tie(y,z) = Q[iq]; iq++;
    for(int d = 0; d < (int)(4); ++d) if(G[z][d] != -1 && !pre_fixed[G[z][d]]) {
      int y2 = G[z][d] == y ? z : y;
      int z2 = G[z][d];
      if(cost.cost[y2][z2] == 255) {
        cost.cost[y2][z2] = cost.cost[y][z]+1;
        Q[nq++] = make_tuple(y2,z2);
      }
      if(cost.cost[y2][z2] == cost.cost[y][z]+1) {
        cost.dir[y2][z2] |= 1<<(d^2);
      }
    }
  }
}

void compute_cost2(group& g) {
  cost2_s& cost = *g.cost2;
  for(int x = 0; x < (int)(nn); ++x) for(int y = 0; y < (int)(nn); ++y) for(int z = 0; z < (int)(nn); ++z) cost.cost[x][y][z] = 255;
  for(int x = 0; x < (int)(nn); ++x) for(int y = 0; y < (int)(nn); ++y) for(int z = 0; z < (int)(nn); ++z) cost.dir[x][y][z] = 0;

  static tuple<uint8_t,uint8_t,uint8_t> Q[NN*NN*NN];
  int nq = 0;

  for(int z = 0; z < (int)(nn); ++z) if(!pre_fixed[z]) {
    cost.cost[g[0]][g[1]][z] = 0;
    cost.dir[g[0]][g[1]][z] = 0;
    Q[nq++] = make_tuple(g[0],g[1],z);
  }

  int iq = 0;
  while(iq < nq) {
    int x,y,z; tie(x,y,z) = Q[iq]; iq++;
    for(int d = 0; d < (int)(4); ++d) if(G[z][d] != -1 && !pre_fixed[G[z][d]]) {
      int x2 = G[z][d] == x ? z : x;
      int y2 = G[z][d] == y ? z : y;
      int z2 = G[z][d];
      if(cost.cost[x2][y2][z2] == 255) {
        cost.cost[x2][y2][z2] = cost.cost[x][y][z]+1;
        Q[nq++] = make_tuple(x2,y2,z2);
      }
      if(cost.cost[x2][y2][z2] == cost.cost[x][y][z]+1) {
        cost.dir[x2][y2][z2] |= 1<<(d^2);
      }
    }
  }
}

void compute_cost3(group& g) {
  cost3_s& cost = *g.cost3;
  for(int x1 = 0; x1 < (int)(64); ++x1) for(int x2 = 0; x2 < (int)(64); ++x2) for(int x3 = 0; x3 < (int)(64); ++x3) for(int z = 0; z < (int)(64); ++z) cost.cost[x1][x2][x3][z] = 255;
  for(int x1 = 0; x1 < (int)(64); ++x1) for(int x2 = 0; x2 < (int)(64); ++x2) for(int x3 = 0; x3 < (int)(64); ++x3) for(int z = 0; z < (int)(64); ++z) cost.dir[x1][x2][x3][z] = 0;

  static tuple<uint8_t,uint8_t,uint8_t,uint8_t> Q[64*64*64*64];
  int nq = 0;

  for(int z = 0; z < (int)(nn); ++z) if(!pre_fixed[z]) {
    cost.cost[g[0]][g[1]][g[2]][z] = 0;
    cost.dir[g[0]][g[1]][g[2]][z] = 0;
    Q[nq++] = make_tuple(g[0],g[1],g[2],z);
  }

  int iq = 0;
  while(iq < nq) {
    int x1,x2,x3,z; tie(x1,x2,x3,z) = Q[iq]; iq++;
    for(int d = 0; d < (int)(4); ++d) if(G[z][d] != -1 && !pre_fixed[G[z][d]]) {
      int y1 = G[z][d] == x1 ? z : x1;
      int y2 = G[z][d] == x2 ? z : x2;
      int y3 = G[z][d] == x3 ? z : x3;
      int z2 = G[z][d];
      if(cost.cost[y1][y2][y3][z2] == 255) {
        cost.cost[y1][y2][y3][z2] = cost.cost[x1][x2][x3][z]+1;
        Q[nq++] = make_tuple(y1,y2,y3,z2);
      }
      if(cost.cost[y1][y2][y3][z2] == cost.cost[x1][x2][x3][z]+1) {
        cost.dir[y1][y2][y3][z2] |= 1<<(d^2);
      }
    }
  }
}

void compute_cost4(group& g) {
  cost4_s& cost = *g.cost4;
  for(int x1 = 0; x1 < (int)(25); ++x1) for(int x2 = 0; x2 < (int)(25); ++x2) for(int x3 = 0; x3 < (int)(25); ++x3) for(int x4 = 0; x4 < (int)(25); ++x4) for(int z = 0; z < (int)(25); ++z) cost.cost[x1][x2][x3][x4][z] = 255;
  for(int x1 = 0; x1 < (int)(25); ++x1) for(int x2 = 0; x2 < (int)(25); ++x2) for(int x3 = 0; x3 < (int)(25); ++x3) for(int x4 = 0; x4 < (int)(25); ++x4) for(int z = 0; z < (int)(25); ++z) cost.dir[x1][x2][x3][x4][z] = 0;

  static tuple<uint8_t,uint8_t,uint8_t,uint8_t,uint8_t> Q[25*25*25*25*25];
  int nq = 0;

  for(int z = 0; z < (int)(nn); ++z) if(!pre_fixed[z]) {
    cost.cost[g[0]][g[1]][g[2]][g[3]][z] = 0;
    cost.dir[g[0]][g[1]][g[2]][g[3]][z] = 0;
    Q[nq++] = make_tuple(g[0],g[1],g[2],g[3],z);
  }

  int iq = 0;
  while(iq < nq) {
    int x1,x2,x3,x4,z; tie(x1,x2,x3,x4,z) = Q[iq]; iq++;
    for(int d = 0; d < (int)(4); ++d) if(G[z][d] != -1 && !pre_fixed[G[z][d]]) {
      int y1 = G[z][d] == x1 ? z : x1;
      int y2 = G[z][d] == x2 ? z : x2;
      int y3 = G[z][d] == x3 ? z : x3;
      int y4 = G[z][d] == x4 ? z : x4;
      int z2 = G[z][d];
      if(cost.cost[y1][y2][y3][y4][z2] == 255) {
        cost.cost[y1][y2][y3][y4][z2] = cost.cost[x1][x2][x3][x4][z]+1;
        Q[nq++] = make_tuple(y1,y2,y3,y4,z2);
      }
      if(cost.cost[y1][y2][y3][y4][z2] == cost.cost[x1][x2][x3][x4][z]+1) {
        cost.dir[y1][y2][y3][y4][z2] |= 1<<(d^2);
      }
    }
  }
}

void init_groups() {
  for(int layer = (n-1); layer >= (int)(3); --layer) {
    if(layer == 3) {
      add_group4(at[3][0],at[3][1],at[3][2],at[3][3]);
      add_group3(at[2][3],at[1][3],at[0][3]);
    } else if(layer == 4) {
      add_group3(at[0][4],at[1][4],at[2][4]);
      add_group3(at[3][4],at[4][4],at[4][3]);
      add_group3(at[4][2],at[4][1],at[4][0]);
    } else if(layer == 5) {
      add_group3(at[5][0],at[5][1],at[5][2]);
      add_group3(at[5][3],at[5][4],at[5][5]);
      add_group2(at[4][5],at[3][5]);
      add_group3(at[2][5],at[1][5],at[0][5]);
    } else if(layer == 6) {
      add_group2(at[0][6],at[1][6]);
      add_group2(at[2][6],at[3][6]);
      add_group3(at[4][6],at[5][6],at[6][6]);
      add_group3(at[6][5],at[6][4],at[6][3]);
      add_group3(at[6][2],at[6][1],at[6][0]);
    } else
      if(layer&1) {
      for(int i = 0; i < layer; i += 2) add_group2(at[layer][i], at[layer][i+1]);
      add_group1(at[layer-1][layer]);
      for(int i = layer-2; i >= 0; i -= 2) add_group2(at[i][layer], at[i-1][layer]);
    }else{
      add_group1(at[0][layer]);
      for(int i = 1; i < layer; i += 2) add_group2(at[i][layer], at[i+1][layer]);
      for(int i = layer-1; i >= 0; i -= 2) add_group2(at[layer][i], at[layer][i-1]);
    }
  }
  for(int i = 0; i < (int)(groups.size()); ++i) {
    if(groups[i].sz == 1) {
      groups[i].cost1 = new cost1_s;
      compute_cost1(groups[i]);
    }else if(groups[i].sz == 2){
      groups[i].cost2 = new cost2_s;
      compute_cost2(groups[i]);
    }else if(groups[i].sz == 3){
      groups[i].cost3 = new cost3_s;
      compute_cost3(groups[i]);
    }else if(groups[i].sz == 4){
      groups[i].cost4 = new cost4_s;
      compute_cost4(groups[i]);
    }
    for(int j = 0; j < (int)(groups[i].sz); ++j) pre_fixed[groups[i][j]] = 1;
  }
  do { cerr << "main.cpp" ":" "415" "  {" << "TM.elapsed()" << "} = {"; debug_impl_seq(TM.elapsed()); cerr << endl << flush; } while(0);

  for(int i = 0; i < (int)(nn); ++i) index_of[i] = -1;
  for(int i = 0; i < (int)(cells.size()); ++i) {
    index_of[cells[i]] = i;
  }
}

uint8_t p8_cost[9][362880];
uint8_t p8_dir[9][362880];
int p8_graph[9][362880][4];

int p8_hash(array<uint8_t,9> const& a) {
  int ix = 0;
  int fact = 40320;
  for(int i = 0; i < (int)(8); ++i) {
    int x = a[i];
    for(int j = 0; j < (int)(i); ++j) if(a[j] < a[i]) x -= 1;
    ix += x * fact;
    fact /= 8-i;
  }
  return ix;
}

void init_p8(){
  vector<int> holes;
  if(n <= 6) holes = {0,1,3,4};
  else holes = {0};
  for(int hole : holes) {
    for(int i = 0; i < (int)(362880); ++i) p8_cost[hole][i] = 255;
    static array<uint8_t,9> Q[181440];
    int nq = 0;
    { array<uint8_t,9> I; for(int i = 0; i < (int)(9); ++i) I[i] = i;
      auto hI = p8_hash(I);
      p8_cost[hole][hI] = 0;
      p8_dir[hole][hI] = -1;
      Q[nq++] = I;
    }
    int iq = 0;
    while(iq < nq) {
      auto A = Q[iq]; iq++;
      auto hA = p8_hash(A);
      int i = 0; while(A[i] != hole) i++;
      for(int d = 0; d < (int)(4); ++d) {
        int j;
        if(d == 0) {
          if(i%3 == 0) continue;
          j = i-1;
        }
        if(d == 1) {
          if(i/3 == 0) continue;
          j = i-3;
        }
        if(d == 2) {
          if(i%3 == 2) continue;
          j = i+1;
        }
        if(d == 3) {
          if(i/3 == 2) continue;
          j = i+3;
        }
        auto B = A;
        swap(B[i],B[j]);
        auto hB = p8_hash(B);
        p8_graph[hole][hA][d] = hB;
        if(p8_cost[hole][hB] == 255) {
          p8_cost[hole][hB] = p8_cost[hole][hA]+1;
          p8_dir[hole][hB] = d^2;
          Q[nq++] = B;
        }
      }
    }
  }
}

void init(){
  { int cur = 0;
    for(int layer = 0; layer < (int)(N); ++layer) {
      for(int x = 0; x < (int)(layer+1); ++x) {
        layerof[cur] = layer;
        at[x][layer] = cur++;
      }
      for(int y = (layer-1); y >= (int)(0); --y) {
        layerof[cur] = layer;
        at[layer][y] = cur++;
      }
    }
  }

  for(int x = 0; x < (int)(n); ++x) for(int y = 0; y < (int)(n); ++y) {
    if(y == n-1) GH[at[x][y]] = -1;
    else GH[at[x][y]] = at[x][y+1];
  }
  for(int x = 0; x < (int)(n); ++x) for(int y = 0; y < (int)(n); ++y) {
    if(x == n-1) GV[at[x][y]] = -1;
    else GV[at[x][y]] = at[x+1][y];
  }

  for(int i = 0; i < (int)(nn); ++i) for(int j = 0; j < (int)(4); ++j) G[i][j] = -1;
  for(int i = 0; i < (int)(nn); ++i) {
    if(GH[i] != -1) {
      G[i][2] = GH[i];
      G[GH[i]][0] = i;
    }
    if(GV[i] != -1) {
      G[i][3] = GV[i];
      G[GV[i]][1] = i;
    }
  }
  for(int sym = 0; sym < (int)(8); ++sym) for(int i = 0; i < (int)(n); ++i) for(int j = 0; j < (int)(n); ++j) {
    int x,y; tie(x,y) = sym_xy(sym,i,j);
    A[sym][at[x][y]] = sym_tile(sym, A0[i][j]);
  }
  for(int sym = 0; sym < (int)(8); ++sym) for(int i = 0; i < (int)(nn); ++i) Acnt[sym][A[sym][i]] += 1;

  for(int i1 = 0; i1 < (int)(n); ++i1) for(int i2 = 0; i2 < (int)(n); ++i2) for(int j1 = 0; j1 < (int)(n); ++j1) for(int j2 = 0; j2 < (int)(n); ++j2) Gdist[at[i1][j1]][at[i2][j2]] = abs(i1-i2)+abs(j1-j2);

  init_groups();
  init_p8();
}


const int INF = (1<<20);
int ha[101][101];
int hp[101];
int hungarian(int n,int m) {
  static int u[101],v[101],way[101];
  static int minv[101],used[101];
  for(int i = 0; i < (int)(m+1); ++i) u[i]=v[i]=hp[i]=way[i] = 0;
  for(int i = (1); i <= (int)(n); ++i) {
    hp[0] = i;
    int j0 = 0;
    for(int j = 0; j < (int)(m+1); ++j) minv[j] = INF;
    for(int j = 0; j < (int)(m+1); ++j) used[j] = 0;
    do {
      used[j0] = true;
      int i0 = hp[j0], delta = INF, j1=0;
      for(int j = (1); j <= (int)(m); ++j) if(!used[j]) {
        int cur = ha[i0][j] - u[i0] - v[j];
        if(cur < minv[j]) { minv[j] = cur; way[j] = j0; }
        if(minv[j] < delta) { delta = minv[j]; j1 = j; }
      }
      for(int j = (0); j <= (int)(m); ++j) {
        if(used[j]) { u[hp[j]] += delta; v[j] -= delta; }
        else { minv[j] -= delta; }
      }
      j0 = j1;
    } while(hp[j0] != 0);
    do {
      int j1 = way[j0];
      hp[j0] = hp[j1];
      j0 = j1;
    } while(j0);
  }
  return -v[0];
}

int skel_matching(uint8_t const* a, uint8_t const* B) {
  uint8_t qa[16][NN];
  int nqa[16];
  uint8_t qb[16][NN];
  int nqb[16];
  for(int i = 0; i < (int)(16); ++i) nqa[i] = nqb[i] = 0;
  for(int i = 0; i < (int)(nn); ++i) qa[a[i]][nqa[a[i]]++] = i;
  for(int i = 0; i < (int)(nn); ++i) qb[B[i]][nqb[B[i]]++] = i;
  float r = 0;
  for(int i = (1); i <= (int)(15); ++i) {
    (static_cast<void> (0));
    if(nqb[i] == 0) continue;
    for(int ia = 0; ia < (int)(nqa[i]); ++ia) for(int ib = 0; ib < (int)(nqb[i]); ++ib) {
      ha[ib+1][ia+1] = Gdist[qa[i][ia]][qb[i][ib]];
    }
    r += hungarian(nqb[i],nqa[i]);
  }
  return (int)r;
}

int goal_matching(vector<int> const& a, vector<int> const& B) {
  uint8_t qa[16][NN];
  int nqa[16];
  uint8_t qb[16][NN];
  int nqb[16];
  for(int i = 0; i < (int)(16); ++i) nqa[i] = nqb[i] = 0;
  for(int i = 0; i < (int)(nn); ++i) qa[a[i]][nqa[a[i]]++] = i;
  for(int i = 0; i < (int)(nn); ++i) qb[B[i]][nqb[B[i]]++] = i;
  float r = 0;
  for(int i = (1); i <= (int)(15); ++i) {
    (static_cast<void> (0));
    if(nqa[i] == 0) continue;
    for(int ia = 0; ia < (int)(nqa[i]); ++ia) for(int ib = 0; ib < (int)(nqb[i]); ++ib) {
      ha[ib+1][ia+1] = sqrt(1000*layerof[qb[i][ib]]) * Gdist[qa[i][ia]][qb[i][ib]];
    }
    r += hungarian(nqb[i],nqa[i]);
  }
  return (int)r;
}

struct state {
  int sym;
  UF2 uf;
  uint8_t board[NN];
  uint8_t rem[16];

  void reset(int sym_) {
    sym = sym_;
    uf.reset();
    for(int i = 0; i < (int)(nn); ++i) {
      if(GH[i] != -1) uf.add_edge(i,GH[i]);
      if(GV[i] != -1) uf.add_edge(i,GV[i]);
    }
    for(int i = 0; i < (int)(nn); ++i) board[i] = 0;
    for(int i = 0; i < (int)(16); ++i) rem[i] = 0;
    for(int i = 0; i < (int)(nn); ++i) rem[A[sym][i]] += 1;
  }

  int score() const {
    int sc = skel_matching(A[sym], board);
    return sc;
  }
};

const int MAX_BEAM_WIDTH = 8000;
const int BEAM_BRANCH = 15;
int beam_width = 500;

state BEAM_A[MAX_BEAM_WIDTH];
state BEAM_B[MAX_BEAM_WIDTH * BEAM_BRANCH];
int BEAM_I[MAX_BEAM_WIDTH * BEAM_BRANCH];
int BEAM_H[MAX_BEAM_WIDTH * BEAM_BRANCH];

vector<tuple<int,vector<int>>> build_skeleton(){
  int na = 1;
  for(int sym = 0; sym < (int)(8); ++sym) BEAM_A[0].reset(sym);
  vector<int> cur_fixed(nn,0);
  cur_fixed[0] = 1;

  int iter=0;

  for(int i = (nn-1); i >= (int)(0); --i) {
    if(TM.elapsed() > TL2) return {};
    iter++;

    int dat[16][4];
    int ndat[16];
    for(int j = 0; j < (int)(16); ++j) ndat[j] = 0;
    for(int j = 0; j < (int)(16); ++j) for(int d = 0; d < (int)(4); ++d) if(j&(1<<d)) {
      if(G[i][d] == -1) continue;
      if(!cur_fixed[G[i][d]]) continue;
      dat[j][ndat[j]++] = d;
    }

    int idat[16][4];
    int nidat[16];
    for(int j = 0; j < (int)(16); ++j) nidat[j] = 0;
    for(int j = 0; j < (int)(16); ++j) for(int d = 0; d < (int)(4); ++d) if(!(j&(1<<d))) {
      if(G[i][d] == -1) continue;
      if(cur_fixed[G[i][d]]) continue;
      idat[j][nidat[j]++] = d;
    }

    int odat[16][4];
    int nodat[16];
    for(int j = 0; j < (int)(16); ++j) nodat[j] = 0;
    for(int j = 0; j < (int)(16); ++j) for(int d = 0; d < (int)(4); ++d) if(!(j&(1<<d))) {
      if(G[i][d] == -1) continue;
      if(!cur_fixed[G[i][d]]) continue;
      odat[j][nodat[j]++] = d;
    }

    int jat[16]; int njat = 0;
    for(int j = 0; j < (int)(16); ++j) {
      if(j==0 && (i > 3 || (i > 0 && n > 6))) continue;
      for(int d = 0; d < (int)(4); ++d) if((j&(1<<d)) && G[i][d] == -1) goto l_pass1;
      jat[njat++] = j;
      l_pass1:;
    }

    int nb = 0;
    for(int ia = 0; ia < (int)(na); ++ia) {
      state const& sa = BEAM_A[ia];
      for(int j0 = 0; j0 < (int)(njat); ++j0) if(int j = jat[j0];1) if(sa.rem[j] > 0) {
        for(int d0 = 0; d0 < (int)(ndat[j]); ++d0) if(int d = dat[j][d0];1){
          if(!(sa.board[G[i][d]]&(1<<(d^2)))) goto l_pass;
        }
        for(int d0 = 0; d0 < (int)(nodat[j]); ++d0) if(int d = odat[j][d0];1){
          if(sa.board[G[i][d]]&(1<<(d^2))) goto l_pass;
        }
        BEAM_B[nb] = sa;
        for(int d0 = 0; d0 < (int)(ndat[j]); ++d0) if(int d = dat[j][d0];1){
          if(!BEAM_B[nb].uf.unite(i,G[i][d])) goto l_pass;
        }
        for(int d0 = 0; d0 < (int)(nidat[j]); ++d0) if(int d = idat[j][d0];1){
          if(!BEAM_B[nb].uf.remove_edge(i,G[i][d])) goto l_pass;
        }
        BEAM_B[nb].board[i] = j;
        BEAM_B[nb].rem[j] -= 1;
        BEAM_I[nb] = nb;
        BEAM_H[nb] = BEAM_B[nb].score();
        nb++;
      l_pass:;
      }
    }

    na = min(nb, beam_width);

    random_shuffle(BEAM_I,BEAM_I+nb,[&](int m){ return RNG.random(m); });
    nth_element(BEAM_I,BEAM_I+na,BEAM_I+nb, [&](int i, int j){
      return BEAM_H[i] < BEAM_H[j];
    });

    for(int ia = 0; ia < (int)(na); ++ia) BEAM_A[ia] = BEAM_B[BEAM_I[ia]];

    cur_fixed[i] = 1;
  }

  sort(BEAM_A,BEAM_A+na, [&](auto const& x, auto const& y){
    return skel_matching(A[x.sym],x.board) < skel_matching(A[y.sym],y.board);
  });
  beam_width *= 1.2;
  beam_width = min(beam_width, MAX_BEAM_WIDTH);
  do { cerr << "main.cpp" ":" "735" "  {" << "\"BUILD\", na, TM.elapsed()" << "} = {"; debug_impl_seq("BUILD", na, TM.elapsed()); cerr << endl << flush; } while(0);

  vector<tuple<int,vector<int>>> R;
  for(int ia = 0; ia < (int)(na); ++ia) {
    vector<int> B(nn);
    for(int i = 0; i < (int)(nn); ++i) B[i] = BEAM_A[ia].board[i];
    R.emplace_back(BEAM_A[ia].sym,B);
  }
  return R;
}


void solution_add(vector<int>& sol, int d) {
  if(!sol.empty() && sol.back() == (d^2)) sol.pop_back();
  else sol.emplace_back(d);
}

tuple<uint8_t,uint8_t> p8_matching(int hole, array<uint8_t,9> const& a, array<uint8_t,9> const& b) {
  uint8_t qa[16][9];
  int nqa[16];
  uint8_t qb[16][9];
  int nqb[16];
  for(int i = 0; i < (int)(16); ++i) nqa[i] = nqb[i] = 0;
  for(int i = 0; i < (int)(9); ++i) qa[a[i]][nqa[a[i]]++] = i;
  for(int i = 0; i < (int)(9); ++i) qb[b[i]][nqb[b[i]]++] = i;
  array<uint8_t,9> p;
  int cost = 255;
  uint8_t dir = 0;
  auto bt = letrec([&](auto bt, int i) -> void {
    if(i == 16) {
      int h = p8_hash(p);
      if(p8_cost[hole][h] < cost) {
        cost = p8_cost[hole][h];
        dir = p8_dir[hole][h];
      }
      return;
    }
    int I[9];
    for(int j = 0; j < (int)(nqa[i]); ++j) I[j] = j;
    do {
      for(int ib = 0; ib < (int)(nqb[i]); ++ib) {
        p[qa[i][ib]] = qb[i][I[ib]];
      }
      bt(i+1);
    } while(next_permutation(I,I+nqa[i]));
  });
  bt(0);
  return make_tuple(cost,dir);
}

int solve_board(int sym, vector<int> goal, bool reversed) {
  vector<int> solution;

  vector<int> a(nn);
  if(reversed) {
    for(int i = 0; i < (int)(nn); ++i) a[i] = goal[i];
    for(int i = 0; i < (int)(nn); ++i) goal[i] = A[sym][i];
  }else{
    for(int i = 0; i < (int)(nn); ++i) a[i] = A[sym][i];
  }
  int z = -1; for(int i = 0; i < (int)(nn); ++i) if(a[i] == 0) z = i;
  { int y = -1; for(int i = 0; i < (int)(nn); ++i) if(goal[i] == 0) y = i;
    if(y >= 4) return 1<<20;
  }

  vector<uint8_t> cur_fixed(nn,false);

  auto find_perms = [&](vector<tuple<int,int,int,int, int,int,int,int>>& perms3,
                        vector<tuple<int,int,int,int,int,int, int,int,int,int>>& perms5){
    vector<int> b(nn); for(int i = 0; i < (int)(nn); ++i) b[i] = i;
    int y = z;
    for(int i = ((int)solution.size()-1); i >= (int)(0); --i) {
      int d = solution[i];
      swap(b[y],b[G[y][d^2]]);
      y = G[y][d^2];

      for(int e = 0; e < (int)(4); ++e) {
        if(G[y][e] == -1) continue;
        int y1 = G[y][e];
        for(int f0 = 0; f0 < (int)(2); ++f0) {
          int f = e^1; if(f0) f^=2;
          if(G[y1][f] == -1) continue;
          int y2 = G[y1][f];
          int y3 = G[y][f];
          (static_cast<void> (0));
          (static_cast<void> (0));
          if(cur_fixed[b[y1]] && a[b[y1]] != a[b[y2]]) continue;
          if(cur_fixed[b[y2]] && a[b[y2]] != a[b[y3]]) continue;
          if(cur_fixed[b[y3]] && a[b[y3]] != a[b[y1]]) continue;
          int moves[4] = {e,f,e^2,f^2};
          int fr = 0, to = 3;
          int cost = 4;
          int j0 = i-1, j1 = i;
          for(; j0 >= 0 && fr <= to && moves[fr] == (solution[j0]^2); --j0) { fr += 1; cost -= 2; }
          for(; j1 < (int)solution.size() && fr <= to && moves[to] == (solution[j1]^2); ++j1) { to -= 1; cost -= 2; }
          if(fr > to) {
            while(j0 >= 0 && j1 < (int)solution.size() && solution[j0] == (solution[j1]^2)) { j0--; j1++; cost -= 2; }
          }
          perms3.emplace_back(b[y1],b[y2],b[y3],cost, 0,e,f,i);
        }
      }

      for(int e = 0; e < (int)(4); ++e) {
        if(G[y][e] == -1) continue;
        int y1 = G[y][e];
        if(G[y1][e] == -1) continue;
        int y2 = G[y1][e];
        for(int f0 = 0; f0 < (int)(2); ++f0) {
          int f = e^1; if(f0) f^=2;
          if(G[y2][f] == -1) continue;
          int y3 = G[y2][f];
          int y4 = G[y3][e^2];
          int y5 = G[y4][e^2];
          (static_cast<void> (0));
          if(cur_fixed[b[y1]] && a[b[y1]] != a[b[y2]]) continue;
          if(cur_fixed[b[y2]] && a[b[y2]] != a[b[y3]]) continue;
          if(cur_fixed[b[y3]] && a[b[y3]] != a[b[y4]]) continue;
          if(cur_fixed[b[y4]] && a[b[y4]] != a[b[y5]]) continue;
          if(cur_fixed[b[y5]] && a[b[y5]] != a[b[y1]]) continue;
          int moves[6] = {e,e,f,e^2,e^2,f^2};
          int fr = 0, to = 5;
          int cost = 6;
          int j0 = i-1, j1 = i;
          for(; j0 >= 0 && fr <= to && moves[fr] == (solution[j0]^2); --j0) { fr += 1; cost -= 2; }
          for(; j1 < (int)solution.size() && fr <= to && moves[to] == (solution[j1]^2); ++j1) { to -= 1; cost -= 2; }
          if(fr > to) {
            while(j0 >= 0 && j1 < (int)solution.size() && solution[j0] == (solution[j1]^2)) { j0--; j1++; cost -= 2; }
          }
          perms5.emplace_back(b[y1],b[y2],b[y3],b[y4],b[y5],cost, 0,e,f,i);
        }
      }

      for(int e = 0; e < (int)(4); ++e) {
        if(G[y][e] == -1) continue;
        int y1 = G[y][e];
        for(int f0 = 0; f0 < (int)(2); ++f0) {
          int f = e^1; if(f0) f^=2;
          if(G[y1][f] == -1) continue;
          int y2 = G[y1][f];
          if(G[y2][f] == -1) continue;
          int y3 = G[y2][f];
          int y4 = G[y3][e^2];
          int y5 = G[y4][f^2];
          (static_cast<void> (0));
          if(cur_fixed[b[y1]] && a[b[y1]] != a[b[y2]]) continue;
          if(cur_fixed[b[y2]] && a[b[y2]] != a[b[y3]]) continue;
          if(cur_fixed[b[y3]] && a[b[y3]] != a[b[y4]]) continue;
          if(cur_fixed[b[y4]] && a[b[y4]] != a[b[y5]]) continue;
          if(cur_fixed[b[y5]] && a[b[y5]] != a[b[y1]]) continue;
          int moves[6] = {e,f,f,e^2,f^2,f^2};
          int fr = 0, to = 5;
          int cost = 6;
          int j0 = i-1, j1 = i;
          for(; j0 >= 0 && fr <= to && moves[fr] == (solution[j0]^2); --j0) { fr += 1; cost -= 2; }
          for(; j1 < (int)solution.size() && fr <= to && moves[to] == (solution[j1]^2); ++j1) { to -= 1; cost -= 2; }
          if(fr > to) {
            while(j0 >= 0 && j1 < (int)solution.size() && solution[j0] == (solution[j1]^2)) { j0--; j1++; cost -= 2; }
          }
          perms5.emplace_back(b[y1],b[y2],b[y3],b[y4],b[y5],cost, 1,e,f,i);
        }
      }

      for(int e = 0; e < (int)(4); ++e) {
        if(G[y][e] == -1) continue;
        int y1 = G[y][e];
        if(G[y1][e] == -1) continue;
        int y2 = G[y1][e];
        for(int f0 = 0; f0 < (int)(2); ++f0) {
          int f = e^1; if(f0) f^=2;
          if(G[y2][f] == -1) continue;
          int y3 = G[y2][f];
          int y4 = G[y3][e^2];
          int y5 = G[y4][f^2];
          (static_cast<void> (0));
          if(cur_fixed[b[y2]] && a[b[y2]] != a[b[y3]]) continue;
          if(cur_fixed[b[y3]] && a[b[y3]] != a[b[y4]]) continue;
          if(cur_fixed[b[y4]] && a[b[y4]] != a[b[y2]]) continue;
          int moves[6] = {e,e,f,e^2,f^2,e^2};
          int fr = 0, to = 5;
          int cost = 6;
          int j0 = i-1, j1 = i;
          for(; j0 >= 0 && fr <= to && moves[fr] == (solution[j0]^2); --j0) { fr += 1; cost -= 2; }
          for(; j1 < (int)solution.size() && fr <= to && moves[to] == (solution[j1]^2); ++j1) { to -= 1; cost -= 2; }
          if(fr > to) {
            while(j0 >= 0 && j1 < (int)solution.size() && solution[j0] == (solution[j1]^2)) { j0--; j1++; cost -= 2; }
          }
          perms3.emplace_back(b[y2],b[y3],b[y4],cost, 1,e,f,i);
        }
      }

    }
  };

  auto apply_perm3 = [&](tuple<int,int,int,int, int,int,int,int> perm3){
    int x1,x2,x3,pc, ty,e,f,ip; tie(x1,x2,x3,pc, ty,e,f,ip) = perm3;
    swap(a[z],a[x1]);
    swap(a[x1],a[x2]);
    swap(a[x2],a[x3]);
    swap(a[x3],a[z]);
    { vector<int> t;
      for(int j = 0; j < (int)(ip); ++j) solution_add(t,solution[j]);
      if(ty == 0) {
        solution_add(t,e);
        solution_add(t,f);
        solution_add(t,e^2);
        solution_add(t,f^2);
      }else{
        solution_add(t,e);
        solution_add(t,e);
        solution_add(t,f);
        solution_add(t,e^2);
        solution_add(t,f^2);
        solution_add(t,e^2);
      }
      for(int j = (ip); j <= (int)((int)solution.size()-1); ++j) solution_add(t,solution[j]);
      solution = t;
    }
  };

  auto apply_perm5 = [&](tuple<int,int,int,int,int,int, int,int,int,int> perm5){
    int x1,x2,x3,x4,x5,pc, ty,e,f,ip; tie(x1,x2,x3,x4,x5,pc, ty,e,f,ip) = perm5;
    swap(a[z],a[x1]);
    swap(a[x1],a[x2]);
    swap(a[x2],a[x3]);
    swap(a[x3],a[x4]);
    swap(a[x4],a[x5]);
    swap(a[x5],a[z]);
    { vector<int> t;
      for(int j = 0; j < (int)(ip); ++j) solution_add(t,solution[j]);
      if(ty == 0) {
        solution_add(t,e);
        solution_add(t,e);
        solution_add(t,f);
        solution_add(t,e^2);
        solution_add(t,e^2);
        solution_add(t,f^2);
      }else if(ty == 1) {
        solution_add(t,e);
        solution_add(t,f);
        solution_add(t,f);
        solution_add(t,e^2);
        solution_add(t,f^2);
        solution_add(t,f^2);
      } else {
        (static_cast<void> (0));
      }
      for(int j = (ip); j <= (int)((int)solution.size()-1); ++j) solution_add(t,solution[j]);
      solution = t;
    }
  };

  auto pick_move = [&](int md) -> int {
    int d;
    do { d = RNG.random(4); } while(!(md&(1<<d)));
    return d;
  };

  for(int ig = 0; ig < (int)((int)groups.size()); ++ig) {
    if(!best_solution.empty() && solution.size() >= best_solution.size()) return 1<<20;
    auto const& g = groups[ig];
    uint8_t unfixed[NN]; int nunfixed = 0;
    for(int i = 0; i < (int)(nn); ++i) if(!cur_fixed[i]) unfixed[nunfixed++] = i;
  l_start:;
    bitset<16> ing = 0;
    for(int i = 0; i < (int)(g.sz); ++i) ing[goal[g[i]]] = 1;
    vector<tuple<int,int,int,int, int,int,int,int>> perms3;
    vector<tuple<int,int,int,int,int,int, int,int,int,int>> perms5;
    find_perms(perms3,perms5);

    if(g.sz == 1) {
      int bpty = -1, bp = -1, by = -1, bc = 255;
      auto find_path = [&](int pty, int p, int pc){
        for(int y0 = 0; y0 < (int)(nunfixed); ++y0) if(int y = unfixed[y0];1) if(a[y] == goal[g[0]] && g.cost1->cost[y][z]+pc < bc) {
          bpty = pty; bp = p; by = y; bc = g.cost1->cost[y][z]+pc;
        }
      };
      find_path(-1,-1,0);
      for(int ip = 0; ip < (int)((int)perms3.size()); ++ip) {
        int x1,x2,x3,pc; tie(x1,x2,x3,pc, ignore,ignore,ignore,ignore) = perms3[ip];
        if(!ing[a[x1]] && !ing[a[x2]] && !ing[a[x3]]) continue;
        swap(a[z],a[x1]);
        swap(a[x1],a[x2]);
        swap(a[x2],a[x3]);
        swap(a[x3],a[z]);
        find_path(3,ip,pc);
        swap(a[x3],a[z]);
        swap(a[x2],a[x3]);
        swap(a[x1],a[x2]);
        swap(a[z],a[x1]);
      }
      for(int ip = 0; ip < (int)((int)perms5.size()); ++ip) {
        int x1,x2,x3,x4,x5,pc; tie(x1,x2,x3,x4,x5,pc, ignore,ignore,ignore,ignore) = perms5[ip];
        if(!ing[a[x1]] && !ing[a[x2]] && !ing[a[x3]] && !ing[a[x4]] && !ing[a[x5]]) continue;
        swap(a[z],a[x1]);
        swap(a[x1],a[x2]);
        swap(a[x2],a[x3]);
        swap(a[x3],a[x4]);
        swap(a[x4],a[x5]);
        swap(a[x5],a[z]);
        find_path(5,ip,pc);
        swap(a[x5],a[z]);
        swap(a[x4],a[x5]);
        swap(a[x3],a[x4]);
        swap(a[x2],a[x3]);
        swap(a[x1],a[x2]);
        swap(a[z],a[x1]);
      }
      int fr = solution.size();
      if(bp != -1) {
        if(bpty == 3) apply_perm3(perms3[bp]);
        if(bpty == 5) apply_perm5(perms5[bp]);
        goto l_start;
      };
      (static_cast<void> (0));
      while(g.cost1->dir[by][z] != 0) {
        int d = pick_move(g.cost1->dir[by][z]);
        swap(a[z],a[G[z][d]]);
        if(G[z][d] == by) by = z;
        z = G[z][d];
        solution_add(solution,d);
      }
      (static_cast<void> (0));
      (static_cast<void> (0));
    }else if(g.sz == 2){
      int bpty = -1, bp = -1, by1 = -1, by2 = -1, bc = 255;
      auto find_path = [&](int pty, int p, int pc){
        static uint8_t q1[NN]; int nq1 = 0; for(int y0 = 0; y0 < (int)(nunfixed); ++y0) if(int y = unfixed[y0];1) if(a[y] == goal[g[0]]) q1[nq1++] = y;
        static uint8_t q2[NN]; int nq2 = 0; for(int y0 = 0; y0 < (int)(nunfixed); ++y0) if(int y = unfixed[y0];1) if(a[y] == goal[g[1]]) q2[nq2++] = y;
        for(int iq1 = 0; iq1 < (int)(nq1); ++iq1) if(int y1 = q1[iq1];1) {
          for(int iq2 = 0; iq2 < (int)(nq2); ++iq2) if(int y2 = q2[iq2];1) if(y1 != y2) {
            if(g.cost2->cost[y1][y2][z]+pc < bc) {
              bpty = pty; bp = p; by1 = y1; by2 = y2; bc = g.cost2->cost[y1][y2][z]+pc;
            }
          }
        }
      };
      find_path(-1,-1,0);

      for(int ip = 0; ip < (int)((int)perms3.size()); ++ip) {
        int x1,x2,x3,pc; tie(x1,x2,x3,pc, ignore,ignore,ignore,ignore) = perms3[ip];
        if(!ing[a[x1]] && !ing[a[x2]] && !ing[a[x3]]) continue;
        swap(a[z],a[x1]);
        swap(a[x1],a[x2]);
        swap(a[x2],a[x3]);
        swap(a[x3],a[z]);
        find_path(3,ip,pc);
        swap(a[x3],a[z]);
        swap(a[x2],a[x3]);
        swap(a[x1],a[x2]);
        swap(a[z],a[x1]);
      }
      for(int ip = 0; ip < (int)((int)perms5.size()); ++ip) {
        int x1,x2,x3,x4,x5,pc; tie(x1,x2,x3,x4,x5,pc, ignore,ignore,ignore,ignore) = perms5[ip];
        if(!ing[a[x1]] && !ing[a[x2]] && !ing[a[x3]] && !ing[a[x4]] && !ing[a[x5]]) continue;
        swap(a[z],a[x1]);
        swap(a[x1],a[x2]);
        swap(a[x2],a[x3]);
        swap(a[x3],a[x4]);
        swap(a[x4],a[x5]);
        swap(a[x5],a[z]);
        find_path(5,ip,pc);
        swap(a[x5],a[z]);
        swap(a[x4],a[x5]);
        swap(a[x3],a[x4]);
        swap(a[x2],a[x3]);
        swap(a[x1],a[x2]);
        swap(a[z],a[x1]);
      }

      int fr = solution.size();
      if(bp != -1) {
        if(bpty == 3) apply_perm3(perms3[bp]);
        if(bpty == 5) apply_perm5(perms5[bp]);
        goto l_start;
      };
      (static_cast<void> (0));
      while(g.cost2->dir[by1][by2][z] != 0) {
        int d = pick_move(g.cost2->dir[by1][by2][z]);
        swap(a[z],a[G[z][d]]);
        if(G[z][d] == by1) by1 = z;
        if(G[z][d] == by2) by2 = z;
        z = G[z][d];
        solution_add(solution,d);
      }
      (static_cast<void> (0));
      (static_cast<void> (0));
      (static_cast<void> (0));
    }else if(g.sz == 3) {
      int bpty = -1, bp = -1, by1 = -1, by2 = -1, by3 = -1, bc = 255;
      auto find_path = [&](int pty, int p, int pc){
        static uint8_t q1[NN]; int nq1 = 0; for(int y0 = 0; y0 < (int)(nunfixed); ++y0) if(int y = unfixed[y0];1) if(a[y] == goal[g[0]]) q1[nq1++] = y;
        static uint8_t q2[NN]; int nq2 = 0; for(int y0 = 0; y0 < (int)(nunfixed); ++y0) if(int y = unfixed[y0];1) if(a[y] == goal[g[1]]) q2[nq2++] = y;
        static uint8_t q3[NN]; int nq3 = 0; for(int y0 = 0; y0 < (int)(nunfixed); ++y0) if(int y = unfixed[y0];1) if(a[y] == goal[g[2]]) q3[nq3++] = y;
        for(int iq1 = 0; iq1 < (int)(nq1); ++iq1) if(int y1 = q1[iq1];1) {
          for(int iq2 = 0; iq2 < (int)(nq2); ++iq2) if(int y2 = q2[iq2];1) if(y1 != y2) {
            for(int iq3 = 0; iq3 < (int)(nq3); ++iq3) if(int y3 = q3[iq3];1) if(y3 != y1 && y3 != y2) {
              if(g.cost3->cost[y1][y2][y3][z]+pc < bc) {
                bpty = pty; bp = p; by1 = y1; by2 = y2; by3 = y3; bc = g.cost3->cost[y1][y2][y3][z]+pc;
              }
            }
          }
        }
      };
      find_path(-1,-1,0);
      for(int ip = 0; ip < (int)((int)perms3.size()); ++ip) {
        int x1,x2,x3,pc; tie(x1,x2,x3,pc, ignore,ignore,ignore,ignore) = perms3[ip];
        if(!ing[a[x1]] && !ing[a[x2]] && !ing[a[x3]]) continue;
        swap(a[z],a[x1]);
        swap(a[x1],a[x2]);
        swap(a[x2],a[x3]);
        swap(a[x3],a[z]);
        find_path(3,ip,pc);
        swap(a[x3],a[z]);
        swap(a[x2],a[x3]);
        swap(a[x1],a[x2]);
        swap(a[z],a[x1]);
      }
      for(int ip = 0; ip < (int)((int)perms5.size()); ++ip) {
        int x1,x2,x3,x4,x5,pc; tie(x1,x2,x3,x4,x5,pc, ignore,ignore,ignore,ignore) = perms5[ip];
        if(!ing[a[x1]] && !ing[a[x2]] && !ing[a[x3]] && !ing[a[x4]] && !ing[a[x5]]) continue;
        swap(a[z],a[x1]);
        swap(a[x1],a[x2]);
        swap(a[x2],a[x3]);
        swap(a[x3],a[x4]);
        swap(a[x4],a[x5]);
        swap(a[x5],a[z]);
        find_path(5,ip,pc);
        swap(a[x5],a[z]);
        swap(a[x4],a[x5]);
        swap(a[x3],a[x4]);
        swap(a[x2],a[x3]);
        swap(a[x1],a[x2]);
        swap(a[z],a[x1]);
      }


      int fr = solution.size();
      if(bp != -1) {
        if(bpty == 3) apply_perm3(perms3[bp]);
        if(bpty == 5) apply_perm5(perms5[bp]);
        goto l_start;
      };
      (static_cast<void> (0));
      while(g.cost3->dir[by1][by2][by3][z] != 0) {
        int d = pick_move(g.cost3->dir[by1][by2][by3][z]);
        swap(a[z],a[G[z][d]]);
        if(G[z][d] == by1) by1 = z;
        if(G[z][d] == by2) by2 = z;
        if(G[z][d] == by3) by3 = z;
        z = G[z][d];
        solution_add(solution,d);
      }
      (static_cast<void> (0));
      (static_cast<void> (0));
      (static_cast<void> (0));
      (static_cast<void> (0));
    } else if(g.sz == 4) {
      int bpty = -1, bp = -1, by1 = -1, by2 = -1, by3 = -1, by4 = -1, bc = 255;
      auto find_path = [&](int pty, int p, int pc){
        static uint8_t q1[NN]; int nq1 = 0; for(int y0 = 0; y0 < (int)(nunfixed); ++y0) if(int y = unfixed[y0];1) if(a[y] == goal[g[0]]) q1[nq1++] = y;
        static uint8_t q2[NN]; int nq2 = 0; for(int y0 = 0; y0 < (int)(nunfixed); ++y0) if(int y = unfixed[y0];1) if(a[y] == goal[g[1]]) q2[nq2++] = y;
        static uint8_t q3[NN]; int nq3 = 0; for(int y0 = 0; y0 < (int)(nunfixed); ++y0) if(int y = unfixed[y0];1) if(a[y] == goal[g[2]]) q3[nq3++] = y;
        static uint8_t q4[NN]; int nq4 = 0; for(int y0 = 0; y0 < (int)(nunfixed); ++y0) if(int y = unfixed[y0];1) if(a[y] == goal[g[3]]) q4[nq4++] = y;
        for(int iq1 = 0; iq1 < (int)(nq1); ++iq1) if(int y1 = q1[iq1];1) {
          for(int iq2 = 0; iq2 < (int)(nq2); ++iq2) if(int y2 = q2[iq2];1) if(y1 != y2) {
            for(int iq3 = 0; iq3 < (int)(nq3); ++iq3) if(int y3 = q3[iq3];1) if(y3 != y1 && y3 != y2) {
              for(int iq4 = 0; iq4 < (int)(nq4); ++iq4) if(int y4 = q4[iq4];1) if(y4 != y1 && y4 != y2 && y4 != y3) {
                if(g.cost4->cost[y1][y2][y3][y4][z]+pc < bc) {
                  bpty = pty; bp = p; by1 = y1; by2 = y2; by3 = y3; by4 = y4; bc = g.cost4->cost[y1][y2][y3][y4][z]+pc;
                }
              }
            }
          }
        }
      };
      find_path(-1,-1,0);
      for(int ip = 0; ip < (int)((int)perms3.size()); ++ip) {
        int x1,x2,x3,pc; tie(x1,x2,x3,pc, ignore,ignore,ignore,ignore) = perms3[ip];
        if(!ing[a[x1]] && !ing[a[x2]] && !ing[a[x3]]) continue;
        swap(a[z],a[x1]);
        swap(a[x1],a[x2]);
        swap(a[x2],a[x3]);
        swap(a[x3],a[z]);
        find_path(3,ip,pc);
        swap(a[x3],a[z]);
        swap(a[x2],a[x3]);
        swap(a[x1],a[x2]);
        swap(a[z],a[x1]);
      }
      for(int ip = 0; ip < (int)((int)perms5.size()); ++ip) {
        int x1,x2,x3,x4,x5,pc; tie(x1,x2,x3,x4,x5,pc, ignore,ignore,ignore,ignore) = perms5[ip];
        if(!ing[a[x1]] && !ing[a[x2]] && !ing[a[x3]] && !ing[a[x4]] && !ing[a[x5]]) continue;
        swap(a[z],a[x1]);
        swap(a[x1],a[x2]);
        swap(a[x2],a[x3]);
        swap(a[x3],a[x4]);
        swap(a[x4],a[x5]);
        swap(a[x5],a[z]);
        find_path(5,ip,pc);
        swap(a[x5],a[z]);
        swap(a[x4],a[x5]);
        swap(a[x3],a[x4]);
        swap(a[x2],a[x3]);
        swap(a[x1],a[x2]);
        swap(a[z],a[x1]);
      }

      int fr = solution.size();
      if(bp != -1) {
        if(bpty == 3) apply_perm3(perms3[bp]);
        if(bpty == 5) apply_perm5(perms5[bp]);
        goto l_start;
      };
      (static_cast<void> (0));
      while(g.cost4->dir[by1][by2][by3][by4][z] != 0) {
        int d = pick_move(g.cost4->dir[by1][by2][by3][by4][z]);
        swap(a[z],a[G[z][d]]);
        if(G[z][d] == by1) by1 = z;
        if(G[z][d] == by2) by2 = z;
        if(G[z][d] == by3) by3 = z;
        if(G[z][d] == by4) by4 = z;
        z = G[z][d];
        solution_add(solution,d);
      }
      (static_cast<void> (0));
      (static_cast<void> (0));
      (static_cast<void> (0));
      (static_cast<void> (0));
      (static_cast<void> (0));
    }else{
      (static_cast<void> (0));
    }
    for(int i = 0; i < (int)(g.sz); ++i) cur_fixed[g[i]] = 1;

  }


  int hole;
  for(int i = 0; i < (int)(9); ++i) if(goal[at[i/3][i%3]] == 0) hole = i;
  array<uint8_t,9> g8;
  for(int j = 0; j < (int)(9); ++j) g8[j] = goal[at[j/3][j%3]];

 l_start2:;
  { vector<tuple<int,int,int,int, int,int,int,int>> perms3;
    vector<tuple<int,int,int,int,int,int, int,int,int,int>> perms5;
    find_perms(perms3,perms5);
    int bpty = -1, bp = -1, bc = 255;
    uint8_t bd = 0;

    { array<uint8_t,9> b;
      for(int i = 0; i < (int)(9); ++i) b[i] = a[at[i/3][i%3]];
      uint8_t c,d; tie(c,d) = p8_matching(hole,b,g8);
      if(c < bc) {
        bpty = -1; bp = -1; bc = c; bd = d;
      }
    }
    for(int ip = 0; ip < (int)((int)perms3.size()); ++ip) {
      int x1,x2,x3,pc; tie(x1,x2,x3,pc, ignore,ignore,ignore,ignore) = perms3[ip];
      swap(a[z],a[x1]);
      swap(a[x1],a[x2]);
      swap(a[x2],a[x3]);
      swap(a[x3],a[z]);
      { array<uint8_t,9> b;
        for(int i = 0; i < (int)(9); ++i) b[i] = a[at[i/3][i%3]];
        uint8_t c,d; tie(c,d) = p8_matching(hole,b,g8);
        if(c+pc < bc) {
          bpty = 3; bp = ip; bc = c+pc; bd = d;
        }
      }
      swap(a[x3],a[z]);
      swap(a[x2],a[x3]);
      swap(a[x1],a[x2]);
      swap(a[z],a[x1]);
    }
    for(int ip = 0; ip < (int)((int)perms5.size()); ++ip) {
      int x1,x2,x3,x4,x5,pc; tie(x1,x2,x3,x4,x5,pc, ignore,ignore,ignore,ignore) = perms5[ip];
      swap(a[z],a[x1]);
      swap(a[x1],a[x2]);
      swap(a[x2],a[x3]);
      swap(a[x3],a[x4]);
      swap(a[x4],a[x5]);
      swap(a[x5],a[z]);
      { array<uint8_t,9> b;
        for(int i = 0; i < (int)(9); ++i) b[i] = a[at[i/3][i%3]];
        uint8_t c,d; tie(c,d) = p8_matching(hole,b,g8);
        if(c+pc < bc) {
          bpty = 5; bp = ip; bc = c+pc; bd = d;
        }
      }
      swap(a[x5],a[z]);
      swap(a[x4],a[x5]);
      swap(a[x3],a[x4]);
      swap(a[x2],a[x3]);
      swap(a[x1],a[x2]);
      swap(a[z],a[x1]);
    }

    if(bc == 255) return 1<<20;

    if(bp != -1) {
      if(bpty == 3) apply_perm3(perms3[bp]);
      if(bpty == 5) apply_perm5(perms5[bp]);
      goto l_start2;
    };

    if(bc != 0) {
      int d = bd;
      solution_add(solution, d);
      swap(a[z],a[G[z][d]]);
      z = G[z][d];
      goto l_start2;
    }
  }
  (static_cast<void> (0));

  if(best_solution.empty() || solution.size() < best_solution.size()) {
    best_solution.clear();
    if(reversed) {
      for(int d : solution) best_solution.emplace_back(unsym_dir(sym,d^2));
      reverse(begin(best_solution), end(best_solution));
      do { cerr << "main.cpp" ":" "1366" "  {" << "\"REV\"" << "} = {"; debug_impl_seq("REV"); cerr << endl << flush; } while(0);
    }else{
      for(int d : solution) best_solution.emplace_back(unsym_dir(sym,d));
    }
    do { cerr << "main.cpp" ":" "1370" "  {" << "best_solution.size(), round(500000.0 * (2.0 - best_solution.size()/(2.0*n*n*n)))" << "} = {"; debug_impl_seq(best_solution.size(), round(500000.0 * (2.0 - best_solution.size()/(2.0*n*n*n)))); cerr << endl << flush; } while(0);
  }
  return solution.size();
}

bool goal_valid(vector<int>& goal){
  UF uf; uf.reset();
  for(int i = 0; i < (int)(nn); ++i) {
    if(GH[i] != -1 && (goal[i]&4)) {
      if(!uf.unite(i,GH[i])) return false;
    }
    if(GV[i] != -1 && (goal[i]&8)) {
      if(!uf.unite(i,GV[i])) return false;
    }
  }
  return true;
}

void improve_goal(int sym, vector<int>& goal) {
  vector<int> a(nn); for(int i = 0; i < (int)(nn); ++i) a[i]=A[sym][i];
  int hgoal = goal_matching(a,goal);
 l_start_imp:;
  {
    if(TM.elapsed() > TL1) return;

    vector<vector<int>> HH1(nn), HH2(nn);
    for(int i1 = 0; i1 < (int)(nn); ++i1) if(goal[i1] != 0 && GH[i1] != -1 && goal[GH[i1]] != 0) {
      for(int i2 = 0; i2 < (int)(nn); ++i2) if(goal[i2] != 0 && GH[i2] != -1 && goal[GH[i2]] != 0) {
        if(i2 == i1 || i2 == GH[i1] || GH[i2] == i1 || GH[i2] == GH[i1]) continue;
        if((goal[i1]^4) == goal[i2]) { HH1[i1].emplace_back(i2); }
        if((goal[GH[i1]]^1) == goal[GH[i2]]) { HH2[i1].emplace_back(i2); }
      }
    }

    vector<vector<int>> HV1(nn), HV2(nn);
    for(int i1 = 0; i1 < (int)(nn); ++i1) if(goal[i1] != 0 && GV[i1] != -1 && goal[GV[i1]] != 0) {
      for(int i2 = 0; i2 < (int)(nn); ++i2) if(goal[i2] != 0 && GV[i2] != -1 && goal[GV[i2]] != 0) {
        if(i2 == i1 || i2 == GV[i1] || GV[i2] == i1 || GV[i2] == GV[i1]) continue;
        if((goal[i1]^8) == goal[i2]) { HV1[i1].emplace_back(i2); }
        if((goal[GV[i1]]^2) == goal[GV[i2]]) { HV2[i1].emplace_back(i2); }
      }
    }

    vector<int> E(nn,0);
    vector<int> C;
    auto find_h = letrec([&](auto find_h, int root, int i, int depth, int limit) -> bool {
      if(i == root && depth != 0) {
        for(int j = 0; j < (int)(depth); ++j) swap(goal[C[2*j]],goal[C[2*j+1]]);
        for(int j = 0; j < (int)(depth); ++j) swap(goal[GH[C[2*j+1]]],goal[GH[C[(2*j+2)%(2*depth)]]]);
        if(goal_valid(goal)) {
          int h = goal_matching(a,goal);
          if(h < hgoal) {
            hgoal = h;
            C.clear();
            return true;
          }
        }
        for(int j = 0; j < (int)(depth); ++j) swap(goal[C[2*j]],goal[C[2*j+1]]);
        for(int j = 0; j < (int)(depth); ++j) swap(goal[GH[C[2*j+1]]],goal[GH[C[(2*j+2)%(2*depth)]]]);
        return false;
      }
      if(depth == limit) return false;
      if(E[i]||E[GH[i]]) return false;
      E[i]=E[GH[i]]=1;
      C.emplace_back(i);
      for(int j : HH1[i]) {
        if(E[j]||E[GH[j]]) continue;
        E[j]=E[GH[j]]=1;
        C.emplace_back(j);
        for(int k : HH2[j]) if(k <= root) {
          if(find_h(root,k,depth+1,limit)) return true;
        }
        C.pop_back();
        E[j]=E[GH[j]]=0;
      }
      C.pop_back();
      E[i]=E[GH[i]]=0;
      return false;
    });

    auto find_v = letrec([&](auto find_v, int root, int i, int depth, int limit) -> bool {
      if(i == root && depth != 0) {
        for(int j = 0; j < (int)(depth); ++j) swap(goal[C[2*j]],goal[C[2*j+1]]);
        for(int j = 0; j < (int)(depth); ++j) swap(goal[GV[C[2*j+1]]],goal[GV[C[(2*j+2)%(2*depth)]]]);
        if(goal_valid(goal)) {
          int h = goal_matching(a,goal);
          if(h < hgoal) {
            hgoal = h;
            C.clear();
            return true;
          }
        }
        for(int j = 0; j < (int)(depth); ++j) swap(goal[C[2*j]],goal[C[2*j+1]]);
        for(int j = 0; j < (int)(depth); ++j) swap(goal[GV[C[2*j+1]]],goal[GV[C[(2*j+2)%(2*depth)]]]);
        return false;
      }
      if(depth == limit) return false;
      if(E[i]||E[GV[i]]) return false;
      E[i]=E[GV[i]]=1;
      C.emplace_back(i);
      for(int j : HV1[i]) {
        if(E[j]||E[GV[j]]) continue;
        E[j]=E[GV[j]]=1;
        C.emplace_back(j);
        for(int k : HV2[j]) if(k <= root) {
          if(find_v(root,k,depth+1,limit)) return true;
        }
        C.pop_back();
        E[j]=E[GV[j]]=0;
      }
      C.pop_back();
      E[i]=E[GV[i]]=0;
      return false;
    });

    for(int limit = (1); limit <= (int)(3); ++limit) {
      bool improved = false;
      for(int i = 0; i < (int)(nn); ++i) if(GH[i] != -1) if(find_h(i,i,0,limit)) { improved = 1; }
      for(int i = 0; i < (int)(nn); ++i) if(GV[i] != -1) if(find_v(i,i,0,limit)) { improved = 1; }
      if(improved) goto l_start_imp;
    }
  }
}

int bestSym = -1;
vector<int> bestBoard;
bool bestReversed;

void solve(){
  int num_iter = 0;
  while(TM.elapsed() < TL1) {
    num_iter += 1;

    auto R = build_skeleton();
    if(R.empty()) continue;

    for(auto [sym,B] : R) {
      improve_goal(sym,B);
      { int r = solve_board(sym, B, 0);
        if(r == (int)best_solution.size()) {
          bestSym = sym;
          bestBoard = B;
          bestReversed = 0;
        }
      }
      if(n <= 6){
        int r = solve_board(sym,B,1);
        if(r == (int)best_solution.size()) {
          bestSym = sym;
          bestBoard = B;
          bestReversed = 1;
        }
      }
      if(TM.elapsed() > TL1) {
        break;
      }
    }
  }

  while(TM.elapsed() < TL2) {
    if(bestSym != -1) {
      solve_board(bestSym, bestBoard, bestReversed);
    }
  }

  do { cerr << "main.cpp" ":" "1535" "  {" << "num_iter, TM.elapsed()" << "} = {"; debug_impl_seq(num_iter, TM.elapsed()); cerr << endl << flush; } while(0);
  if(!best_solution.empty()) {
    double k = best_solution.size(), t = 2.0*n*n*n;
    int score = round(500000.0 * (2.0 - k/t));
    do { cerr << "main.cpp" ":" "1539" "  {" << "score" << "} = {"; debug_impl_seq(score); cerr << endl << flush; } while(0);
    do { cerr << "main.cpp" ":" "1540" "  {" << "best_solution.size()" << "} = {"; debug_impl_seq(best_solution.size()); cerr << endl << flush; } while(0);
  }
  for(int d : best_solution) cout << "LURD"[d];
  cout << endl;
}

int main() {
  TM.reset();
  read();
  init();
  solve();

  return 0;
}
