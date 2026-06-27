#include <iostream>
#include <cstdlib>

#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "sys/time.h"
using namespace std;
#define HERE (cerr << "LINE: " << __LINE__ << " " << __FUNCTION__ << endl)

#define DBG(x) cerr << #x << ": " << (x) << endl

struct timeval timer_begin, timer_end;
uint64_t tsc_begin;
//double sec_per_count = 1.0 / 2.7e9;
double sec_per_count = -1;
int timer_called;
inline uint64_t get_time_64() {
  uint32_t lo, hi;
  asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
  return (((uint64_t)hi << 32) | lo);
}

inline void timer_start() 
{
  timer_called++;
  gettimeofday(&timer_begin, NULL);
  tsc_begin = get_time_64();
}     
inline double timer_now() 
{
  if (sec_per_count < 0) {
    timer_called++;
    gettimeofday(&timer_end, NULL);         
    double duration = timer_end.tv_sec - timer_begin.tv_sec +
      (timer_end.tv_usec - timer_begin.tv_usec)/1000000.0;
    if (duration < 1e-3) {
      return duration;
    }
    uint64_t tsc_duration = get_time_64() - tsc_begin;
    sec_per_count = duration / tsc_duration;
    DBG(1/sec_per_count);
  }
  return (get_time_64() - tsc_begin) * sec_per_count;
}
void timer_stat() {
  DBG(1/sec_per_count);
  {
    timer_called++;
    gettimeofday(&timer_end, NULL);         
    double duration = timer_end.tv_sec - timer_begin.tv_sec +
      (timer_end.tv_usec - timer_begin.tv_usec)/1000000.0;
    uint64_t tsc_duration = get_time_64() - tsc_begin;
    DBG(duration);
    DBG(tsc_duration);
    DBG(tsc_duration / duration);
  }
}

template<class T>
using less_priority_queue = priority_queue<T,vector<T>,greater<T>>;

template<typename T>
void assign_min_max(T& min,T& max,const T& x) {
  if (x < min)
    min = x;
  if (x > max)
    max = x;
}
unsigned int hash_function(unsigned long p) {
  // xor32()
  p ^= p<<7;
  p ^= p>>1;
  p ^= p<<25;
  unsigned int c = __builtin_popcount(p);
  return p<<c | p>>(32-c);
}
uint64_t hash_function64(uint64_t p) {
  // xor64()
  p ^= p<<16;
  p ^= p>>7;
  p ^= p<<39;
  uint64_t c = __builtin_popcount(p);
  return p<<c | p>>(64-c);
}  

struct xor128_t {
  uint64_t x, y, z, w;

  xor128_t(int seed=0) : x(123456789^seed) , y(362436069), z(521288629), w(88675123) {
    for (int i=0; i<48; i++)
      get();
  }

  void init(int seed) {
    x = 123456789^seed;
    y = 362436069;
    z = 521288629;
    w = 88675123;

    for (int i=0; i<48; i++)
      get();
  }

  inline uint64_t get() {
    uint64_t t=(x^(x<<11)); x=y; y=z; z=w;
    return (w=(w^(w>>19))^(t^(t>>8)));
  }

  inline uint64_t get(unsigned int sz) {
    if (sz <= 1)
      return 0;

    uint64_t x;
    const uint64_t mask = (1<<(ilogb(sz-1)+1)) - 1;
    //cout << sz << " " << mask << endl;
    assert(mask >= (sz-1) && mask < 2*sz-1);
    do {
      x = get() & mask;
    } while (x >= sz);

    return x;
  }

  inline int64_t get(int beg,int end) {
    return get(end-beg) + beg;
  }

  double get_double() {
    static const double double_factor = 1.0 / (1.0 + 0xffffffffffffffffuLL);
    return get() * double_factor;
  }

  double get_norm() {
    double a = get_double();
    double b = get_double();

    return sqrt(-2*log(a)) * cos(2*M_PI*b);
  }

  double get_gamma(double a) {
    if (a < 1.0) {
      return get_gamma(1+a) * pow(get_double(), 1/a);
    }
    
    double d = a - 1/3.0;
    double c = 1/sqrt(9.0 * d);

    while (true) {
      double x = get_norm();
      double uni = 1 - get_double();
      double v = (1.0+c*x)*(1.0+c*x)*(1.0+c*x);
      if (v > 0 && log(uni) < 0.5 * x*x + d - d*v + d*log(v))
	return d*v;
    }
  }

  template<typename T> void shuffle(vector<T>& v,int partial=-1) {
    int sz = v.size();
    if (partial < 0 || partial > sz)
      partial = sz;

    for (int i=0; i<partial; i++) {
      swap(v[i], v[i + get(sz-i)]);
    }
  }
  
  template<typename T> T sample(const vector<T>& v) {
    assert(!v.empty());
    return v[get(v.size())];
  }
};

// Van der Corput sequence
double vdc(int i,int base) {
  double result = 0;
  double f = 1;

  while (i > 0) {
    f /= base;
    result += f * (i%base);
    i = i/base;
  }

  return result;
}

struct uf_t {
  vector <int16_t> parent;

  void clear() {
    parent.clear();
  }

  void add(int elm) {
    if (elm >= parent.size()) 
      parent.resize(elm+1, -1);
  }

  void merge(int a,int b) {
    add(a);
    add(b);

    int ia = a;
    int ib = b;

    if ((ia^ib)&1) swap(ia, ib);  // ok?
      
    int ra = root(ia);
    int rb = root(ib);

    if (ra != rb) {
      parent[ra] += parent[rb];
      parent[rb] = ra;
    }
  }

  int size(int elm) {
    return -parent[root(elm)];
  }

  int id(int elm) {
    return root(elm);
  }

  int root(int ia) {
    add(ia);
    return (parent[ia] < 0) ? ia : (parent[ia] = root(parent[ia]));
  }
};

struct reusable_set_t {
  typedef uint8_t mark_t;
  mark_t mark;
  vector<mark_t> v;
  reusable_set_t() {}
  reusable_set_t(int n) : v(n), mark(1) {}

  void insert(int x) { v[x] = mark; }
  void erase(int x) { v[x] = mark-1; }
  int count(int x) const { return (v[x] == mark); }
  void clear() {
    mark++;
    if (mark == 0) {
      fill(v.begin(), v.end(), 0);
      mark ++;
    }
  }
};

struct sa_t {
  double ini, fin, inv_T, progress;
  int num_call, num_accept, num_better;
  xor128_t rng;
  sa_t() {}
  sa_t(double ini, double fin) : ini(ini), fin(fin), num_accept(0), num_call(0), num_better(0) {
    set_progress(0);
  }

  void set_progress(double x) {
    progress = clamp(x, 0.0, 1.0);
    inv_T = 1 / (ini * pow(fin/ini, progress));
  }

  bool acceptable(double x) {
    //bool result = (x > 0) || rng.get_double() < exp(x*inv_T);
    //bool result = (x > 0) || log(1-rng.get_double()) < x*inv_T;
    double z = x*inv_T;
    bool result = (x > 0) || (z > -12 && log(1-rng.get_double()) < z);

    //num_call ++;
    //num_accept += result;
    //num_better += (x > 0);

    return result;
  }

  void show_log() const {
    cerr << "better/accept/call: "
	 << num_better
	 << "/" << num_accept
	 << "/" << num_call 
	 << " " << (double)num_accept/num_call << endl;
  }
};

template<typename T> ostream& operator<<(ostream& os, const vector<T>& v) {
  os << "[ ";
  for(typename vector<T>::const_iterator it=v.begin(); it!=v.end(); ++it)
    os << '\"' << *it << '\"' << (it+1==v.end() ? "" : ", "); os << " ]"; 
  return os; 
}
template<typename T> ostream& operator<<(ostream& os, const set<T>& v) {
  os << "{ ";
  for(typename set<T>::const_iterator it=v.begin(); it!=v.end(); ++it) {
    if (it != v.begin())
      os << ", ";
    os << *it; 
  }
  os << " }"; 
  return os; 
}
template<typename T1,typename T2> ostream& operator<<(ostream& os, const pair<T1,T2>& v) {
  os << "[ " << v.first << ", " << v.second << "]";
  return os; 
}
template<typename... Args>
string format(const string& fmt, Args... args) {
  int sz = snprintf(nullptr, 0, fmt.c_str(), args ...);
  char buf[sz+1];
  snprintf(buf, sz+1, fmt.c_str(), args ...);
  return buf;
}

template<typename T>
void make_vector_unique(vector<T>& v) {
  sort(v.begin(), v.end());
  v.erase(unique(v.begin(), v.end()), v.end());
}
template<typename T>
bool include_v(const vector<T>& v,const T& item) {
  return find(v.begin(), v.end(), item) != v.end();
}

const int G = 64;
int CP(int x,int y) { return (y+1)*G+(x+1); }
int CX(int p) { return p%G - 1; }
int CY(int p) { return p/G - 1; }
pair<int,int> CXY(int p) { return {CX(p), CY(p)}; }
string CS(int p) { return format("(%2d,%2d)", CX(p), CY(p)); }
string CSS(const vector<int>& ps) {
  string s;
  s += "{";
  for (int p : ps) {
    s += format("(%2d,%2d), ", CX(p), CY(p));
  }
  s += "}";
  return s;
}
const vector<int> DPS4 = { 1, -G, -1, G };
const vector<int> DPS8 = { 1, -G+1, -G, -G-1, -1, G-1, G, G+1 };
int MOD(int x,int m) {
  int y = x % m;
  if (y < 0) { y += m; }
  return y;
}
template<typename T> T pow2(const T& x) { return x*x; }
char OX(bool a) { return "xo"[a]; }

const double TIME_LIMIT = 5.0;

/* insert here */
#ifdef LOCAL
#include "./svg.hpp"
#endif
bool opt_cheat = false;
bool opt_self_check = false;

const int U = (int)1e5;
const int L_MIN = (int)1e5 / 10;
const int L_MAX = (int)1e5 / 2;

double gaussian(double x) {
  const double inv_sqrt_2 = 1.0/sqrt(2);
  return 0.5 + 0.5*erf(inv_sqrt_2 * x);
}

double inv_gaussian(double y) {
  double x0 = -10.0;
  double x2 = 10.0;
  double x1;

  for (int i=0; i<60; i++) {
    x1 = (x0+x2)/2.0;
    double y1 = gaussian(x1);
    if (y1 < y) {
      x0 = x1;
    } else {
      x2 = x1;
    }
  }

  return x1;
}

double truncated_gaussian_median(double x,double lo,double hi,double sigma) {
  double zlo = (lo - x) / sigma;
  double zhi = (hi - x) / sigma;

  double qmed = (gaussian(zlo) + gaussian(zhi)) / 2.0;

  double zmed = inv_gaussian(qmed);
  
  return x + zmed*sigma;
}

template <typename FUNC>
double golden_section_min(double x0,double x3,const FUNC& func) {
  const double inv_phi = 2.0 / (1 + sqrt(5));

  double y0 = func(x0);
  double y3 = func(x3);

  for (int i=0; i<10; i++) {
    if (isinf(y0)) {
      x0 = x0 + 0.5*(x3-x0);
      y0 = func(x0);
    } else if (isinf(y3)) {
      x3 = x3 - 0.5*(x3-x0);
      y3 = func(x3);
    } else {
      break;
    }
  }

  double x1 = x3 - (x3 - x0)*inv_phi;
  double x2 = x0 + (x3 - x0)*inv_phi;

  double y1 = func(x1);
  double y2 = func(x2);

  for (int i=0; i<16; i++) {
    if (y1 < y2) {
      x3 = x2; y3 = y2;
      x2 = x1; y2 = y1;

      x1 = x3 - (x3 - x0)*inv_phi;
      y1 = func(x1);
    } else {
      x0 = x1; y0 = y1;
      x1 = x2; y1 = y2;

      x2 = x0 + (x3 - x0)*inv_phi;
      y2 = func(x2);
    }
  }

  if (y0 < y1 && y0 < y3) {
    return x0;
  } else if (y3 < y0 && y3 < y1) {
    return x3;
  } else {
    return x1;
  }
};

struct box_t {
  int id;
  int w, h;

  int width(bool rot) const { return (rot) ? h : w; }
  int height(bool rot) const { return (rot) ? w : h; }

  int longer_side() const { return max(w, h); }
  int shorter_side() const { return min(w, h); }

  bool is_landscape() const { return (w > h); }
};

struct w_loss_table_t {
  static const int M = 1000;
  static const int SZ = 1000;
  
  vector<double> tbl;

  void build(int L) {
    tbl.clear();
    
    double tm = -timer_now();
    for (int i=0; i<SZ; i++) {
      int64_t g = i*M;

      double sum = 0;
      int n = 0;
      for (int a=L; a<=U; a+=M) {
        sum += (a > g) ? g : tbl[(g-a)/M];
        n += 1;
      }
      sum /= n;

      tbl.push_back(sum);
    }
    tm += timer_now();
    DBG(tm);
  }

  double get(int w) const {
    if (w/M+1 >= SZ) { return tbl.back(); }

    int xlo = w/M;
    int xhi = xlo + 1;

    double ylo = tbl[xlo];
    double yhi = tbl[xhi];


    return ylo + (yhi - ylo) * (double)(w - xlo*M)/M;
  }

};

struct testcase_t {
  int N, T, sigma;
  int64_t area;
  int lb_score;
  mutable int L;
  vector<box_t> bs, bs0;
  w_loss_table_t w_loss;  

  testcase_t() : L(-1) {}

  double integrate_gaussian_pdf(double a,double b) const {
    const double inv_sqrt_2 = 1.0 / sqrt(2);
    if (b < 0) {
      swap(a, b);
      a *= -1;
      b *= -1;
    }

    return 0.5 * (erf(inv_sqrt_2 * b) -erf(inv_sqrt_2 * a));
  }

  double likely(int x,int l) const {
    double lo = (double)(l - x) / sigma;
    double hi = (double)(U - x) / sigma;

    return integrate_gaussian_pdf(lo, hi) / (U - l);
  }

  int estimate_L() const {
    double best_val = 1e234;
    int best_l;
    best_l = golden_section_min(L_MIN, L_MAX, [&](double x) {
      double val = 0;
      for (const auto& box : bs0) {
        val += -log(likely(box.w, x));
        val += -log(likely(box.h, x));
      }
      //cerr << x << " " << val << endl;
      return val;
    });

    return best_l;
  }

  void read(istream& is) {
    is >> N >> T >> sigma;
    DBG(N);
    DBG(T);
    {
      int S = sigma;
      DBG(S);
    }
    bs0.resize(N);
    bs.resize(N);

    for (int i=0; i<N; i++) {
      bs0[i].id = i;
      is >> bs0[i].w >> bs0[i].h;

      bs[i] = bs0[i];
    }

    calc_area();
    L = estimate_L();
    DBG(L);

    for (int i=0; i<N; i++) {
      //bs[i].w = clamp(bs0[i].w, L, U);
      //bs[i].h = clamp(bs0[i].h, L, U);
      bs[i].w = truncated_gaussian_median(bs0[i].w, L, U, sigma);
      bs[i].h = truncated_gaussian_median(bs0[i].h, L, U, sigma);
    }      
  }

  void read_cheat(const string& fname) {
    ifstream ifs(fname);
    
    ifs >> N >> T >> sigma;
    DBG(N);
    DBG(T);

    sigma = 10;
    bs.resize(N);

    for (int i=0; i<N; i++) {
      ifs >> bs[i].w >> bs[i].h;      
    }
    for (int i=0; i<N; i++) {
      bs[i].id = i;
      ifs >> bs[i].w >> bs[i].h;
    }

    calc_area();

    L = estimate_L();
    cerr << "cheat" << bs.size() << endl;
  }

  void read_truth(const string& fname) {
    ifstream ifs(fname);

    ifs >> N >> T >> sigma;
    bs.resize(N);

    for (int i=0; i<N; i++) {
      bs[i].id = i;
      ifs >> bs[i].w >> bs[i].h;      
    }
    for (int i=0; i<N; i++) {
      bs[i].id = i;
      ifs >> bs[i].w >> bs[i].h;            
    }
    calc_area();
    
    cerr << "read: ground truth" << endl;
  }

  void calc_area() {
    area = 0;
    for (int i=0; i<N; i++) {
      area += (int64_t)bs[i].w * bs[i].h;
    }      
    lb_score = ceil(sqrt(area)) * 2;
  }

  void build_w_loss_table() {
    w_loss.build(L);
  }

  double relative_score(int sc) const {
    return (double)(sc)/lb_score;
  }    

  double fill_score(int sc) const {
    return (double)lb_score/sc;
  }

  double get_sample(xor128_t& rng,int x,int L,int U) const {
    double y = x;
    for (int i=0; i<10; i++) {
      y = x + sigma * rng.get_norm();
      if (y >= L && y <= U) {
        return y;
      }
    }

    // fallback for a outlier.
    return get_sample2(rng, x, L, U);
  }

  double get_sample2(xor128_t& rng,int x,int L,int U) const {
    double qL = gaussian((double)(L-x)/sigma);
    double qU = gaussian((double)(U-x)/sigma);

    double q = qL + (qU-qL)*rng.get_double();

    int y = x + sigma*inv_gaussian(q);

    assert(y >= L);
    assert(y <= U);
    return y;
  }

  testcase_t generate(xor128_t& rng) const {
    testcase_t tc(*this);
    const int U = 1e5;

    int trials = 0;
    for (int i=0; i<N; i++) {
      tc.bs[i].w = get_sample(rng, bs0[i].w, L, U);
      tc.bs[i].h = get_sample(rng, bs0[i].h, L, U);
    }

    tc.calc_area();        
    return tc;
  }
};

struct rect_t {
  int l, t, r, b;
  rect_t() : l(-1), t(-1), r(-1), b(-1) {}

  bool is_valid() const {
    return l >= 0;
  }

  bool operator==(const rect_t& rect) const {
    return l == rect.l && r == rect.r && t == rect.t && b == rect.b;
  }
};

struct command_t {
  int id;
  bool rot;
  char dir;
  int ref;

  string to_output() const {
    return format("%d %d %c %d", id, rot, dir, ref);
  }
};

struct range_val_t {
  map<int,int> m;

  int plus(int x) const { return 2*x+1; }
  int minus(int x) const { return 2*x; }

  range_val_t() {
    m[minus(-100)] = 0;
    m[plus(100*100000)] = 0;
  }

  void update(int l,int r,int y) {

    m[plus(l)] = y;
    m[minus(r)] = y;

    auto lo = m.find(plus(l));
    lo --;
    m[minus(l)] = lo->second;
    
    auto hi = m.find(minus(r));
    hi ++;
    m[plus(r)] = hi->second;

    auto beg = m.find(plus(l));
    auto end = m.find(minus(r));
    beg ++;

    m.erase(beg, end);
  }

  int get(int x) { return get_max(x, x); }
  
  int get_max(int l,int r) {
    auto beg = m.lower_bound(plus(l));
    auto end = m.upper_bound(minus(r));
    if (beg->first != plus(l)) {
      beg --;
    }
    int y = 0;

    for (auto it=beg; it!=end; it++) {
      y = max(y, it->second);
    }

    return y;
  }

  int64_t integrate() const {
    int64_t sum = 0;
    auto it = m.begin();
    while (it != m.end()) {
      int x1 = it->first;
      int y1 = it->second;
      it ++;
      assert(it != m.end());
      int x2 = it->first;
      int y2 = it->second;
      it ++;

      //cerr << "xy1: " << x1 << " " << y1 << endl;
      //cerr << "xy2: " << x2 << " " << y2 << endl;
      assert(y1 == y2);

      sum += (int64_t)(x2/2 - x1/2) * y1;
    }

    return sum;
  }
};


struct canvas_t {
  int N;
  vector<rect_t> rect_history;
  vector<int> id2index;
  int width, height;
  int64_t val;
  range_val_t y_envelope;
  uint64_t tail_hash;
  string memo;
  vector<command_t> history;
  canvas_t(int N) : N(N), val(0), tail_hash(0) {
    id2index.resize(N, -1);
    width = height = 0;
  }

  bool is_solved() const {
    return history.size() == N;
  }

  const rect_t& rect_by_id(int id) const {
    assert(id2index[id] >= 0);
    return rect_history[id2index[id]];
  }

  void put_box(const box_t& box,const command_t& cmd) {
    assert(cmd.ref == -1 || cmd.ref >= 0 && cmd.ref < N && id2index[cmd.ref] >= 0);
    assert(cmd.dir == 'U');
    int x = (cmd.ref == -1) ? 0 : rect_history[id2index[cmd.ref]].r;

    int w = box.w;
    int h = box.h;
    if (cmd.rot) { swap(w, h); }

    int t = 0;
    for (const auto& rect : rect_history)  {
      if (x < rect.r && x+w > rect.l) {
        t = max(t, rect.b);
      }
    }

    rect_t rect;

    rect.t = t;
    rect.b = t + h;

    rect.l = x;
    rect.r = x + w;

    //y_envelope.update(x, x+w, t+h);

    id2index[box.id] = rect_history.size();
    rect_history.push_back(rect);

    width = max(width, rect.r);
    height = max(height, rect.b);

    history.push_back(cmd);
  }

  pair<int,int> size_with_new_box(const box_t& box,const command_t& cmd) const {
    assert(cmd.ref == -1 || cmd.ref >= 0 && cmd.ref < N && id2index[cmd.ref] >= 0);
    assert(cmd.dir == 'U');
    int x = (cmd.ref == -1) ? 0 : rect_history[id2index[cmd.ref]].r;

    int w = box.w;
    int h = box.h;
    if (cmd.rot) { swap(w, h); }

    int t = 0;
    //for (const auto& rect : rect_history)  {
    for (auto it=rect_history.rbegin(); it!=rect_history.rend(); it++) {
      const auto& rect(*it);
      if (t < rect.b && x < rect.r && x+w > rect.l) {
        t = max(t, rect.b);
      }
    }

    int b = t + h;
    int r = x + w;

    return {max(width, r), max(height, b)};
  }

  int b_with_new_box(const box_t& box,const command_t& cmd) const {
    assert(cmd.ref == -1 || cmd.ref >= 0 && cmd.ref < N && id2index[cmd.ref] >= 0);
    assert(cmd.dir == 'U');
    int x = (cmd.ref == -1) ? 0 : rect_history[id2index[cmd.ref]].r;

    int w = box.w;
    int h = box.h;
    if (cmd.rot) { swap(w, h); }

    int t = 0;
    for (auto it=rect_history.rbegin(); it!=rect_history.rend(); it++) {
      const auto& rect(*it);
      if (t < rect.b && x < rect.r && x+w > rect.l) {
        t = max(t, rect.b);
      }
    }

    int b = t + h;

    return b;
  }

  void put_box_U(const box_t& box,bool rot,int ref) {
    command_t cmd({box.id, rot, 'U', ref});
    put_box(box, cmd);
  }

  pair<int,int> size_with_new_box(const box_t& box,bool rot,int ref) const {
    command_t cmd({box.id, rot, 'U', ref});
    return size_with_new_box(box, cmd);
  }

  int b_with_new_box(const box_t& box,bool rot,int ref) const {
    command_t cmd({box.id, rot, 'U', ref});
    return b_with_new_box(box, cmd);
  }

  bool operator==(const canvas_t& canvas) const {
    return rect_history == canvas.rect_history;
  }
  bool operator!=(const canvas_t& canvas) const {
    return !(*this == canvas);
  }

  int score() const {
    if (history.size() == N) {
      return width + height;
    } else {
      return width + height + (N - history.size()) *2*100000;
    }
  }

  void output(ostream& os) const {
    if (!memo.empty()) {
      os << "# " << memo << endl;
    }
    os << history.size() << endl;
    for (const auto& cmd : history) {
      os << cmd.to_output() << endl;
    }
  }

#ifdef LOCAL
  void draw(const testcase_t& tc) const {
    static int called = 0;
    called ++;

    int w = 1e5 * sqrt(tc.N);
    svg_t svg(w, w);
    svg.rect(0, 0, w, w, "fill='#fff'");
    //for (const auto& rect : rect_history) {
    for (int id=0; id<N; id++) {
      if (id2index[id] < 0) { continue; }
      const auto& rect(rect_by_id(id));
      svg.rect(rect.l, rect.t, rect.r-rect.l, rect.b-rect.t,
               "fill='#808080'");
      svg.centered_text((rect.l+rect.r)/2, (rect.t+rect.b)/2,
                        format("r%02d", id),
                        "font-size='10000'");
    }
    svg.centered_text(w/2, w-30000, memo,
                      "font-size='10000'");                      

    {
      ofstream ofs(format("images/out-%04d.svg", called));
      ofs << svg.to_string() << endl;
    }
  }
#endif
};

struct layer_t {
  vector<pair<int,bool>> ids;
  int w;
  int mh;
  int64_t loss;
  size_t size() const { return ids.size(); }

  layer_t() : w(0), mh(0), loss(0) {}

  void put_box(const box_t& box,bool rot) {
    //cerr << "put_box(" << box.id << "," << rot << ")" << endl;
    //cerr << " box: " << box.width(rot) << " " << box.height(rot) << endl;
    ids.push_back({box.id, rot});

    int h = box.height(rot);
    if (h > mh) {
      loss += (int64_t)w*(h - mh);
      mh = h;
    }
    loss += (int64_t)box.width(rot)*(mh - h);
    w += box.width(rot);
  }

  int64_t increased_loss_with_new_box(const box_t& box,bool rot) const {
    int h = box.height(rot);
    int new_mh = mh;
    int64_t new_loss = loss;
    if (h > new_mh) {
      new_loss += (int64_t)w*(h - new_mh);
      new_mh = h;
    }
    new_loss += (int64_t)box.width(rot)*(new_mh - h);

    return new_loss - loss;
  }

  int last_index() const {
    return (ids.empty()) ? -1 : ids.back().first;
  }
};

struct solver_t {
  const testcase_t tc, tc_truth;
  solver_t(const testcase_t& tc,const testcase_t& tc_truth) : tc(tc), tc_truth(tc_truth) {}

  vector<canvas_t> generate_sequential(const testcase_t& tc) {
    canvas_t canvas(tc.N);
    auto best_canvas(canvas);
    int best_score = best_canvas.score();

    int th = (tc.lb_score/2) * 0.8;

    vector<canvas_t> cands;
    for (int lp=0; /**/; lp++) {
      layer_t layer;
      int cnt = 0;

      int min_exceed = th + 1e5;
      int num_layer = 1;
      int max_w = 0;
      canvas = canvas_t(tc.N);      
      for (int i=0; i<tc.N; i++) {
        if (layer.w + tc.bs[i].shorter_side() > th) {
          min_exceed = min(min_exceed, layer.w + tc.bs[i].shorter_side());
          max_w = max(max_w, layer.w);
          layer = layer_t();
          num_layer ++;
        }
        
        int ref = layer.last_index();
        bool rot = tc.bs[i].is_landscape();
        {
          int ex_w = layer.w;
          for (int j=i; j<tc.N; j++) {
            ex_w += tc.bs[i].longer_side();
          }
          if (ex_w < max_w) {
            rot = !rot;
          } else {
            int d_short = abs(tc.bs[i].shorter_side() - layer.mh);
            int d_long = abs(tc.bs[i].longer_side() - layer.mh);
            if (layer.size() > 0 && d_short < d_long) {
              rot = !rot;
            }
          }
        }
        
        layer.put_box(tc.bs[i], rot);
        canvas.put_box_U(tc.bs[i], rot, ref);
      }
      canvas.memo = format("th: %d", th);
      cands.push_back(canvas);

      int sc = canvas.score();
      if (sc < best_score) {
        best_score = sc;
        best_canvas = canvas;
      }
      if (sc > best_score*1.3) { break; }
      if (num_layer == 1) {
        break;
      }

      th = min_exceed;
    }

    return cands;
  }

  vector<canvas_t> choose_candidates_simple(const vector<canvas_t>& cands) {
    vector<pair<int,int>> ord;
    for (int i=0; i<cands.size(); i++) {
      ord.push_back({cands[i].score(), i});
    }
#if 0    
    sort(ord.begin(), ord.end());
    ord.erase(unique(ord.begin(), ord.end(), [&](const auto& a,const auto& b) {
      return a.first == b.first && cands[a.second].memo == cands[b.second].memo;
    }), ord.end());
#endif
    sort(ord.begin(), ord.end());    
    ord.erase(unique(ord.begin(), ord.end(), [&](const auto& a,const auto& b) {
      return a.first == b.first && cands[a.second].width == cands[b.second].width;
    }), ord.end());
    
    DBG(ord.size());
    
    vector<canvas_t> cands2;
    for (int i=0; i<tc.T; i++) {
      const auto& canvas((i < ord.size()) ? cands[ord[i].second] : cands[ord[0].second]);
      cands2.push_back(canvas);
    }

    return cands2;
  }
  
  vector<canvas_t> choose_candidates_env(const vector<canvas_t>& cands) {
    vector<tuple<int,int,int64_t,int>> ord;
    for (int i=0; i<cands.size(); i++) {
      range_val_t yenv;
      for (const auto& rect : cands[i].rect_history) {
        yenv.update(rect.l, rect.r, rect.b);
      }
      auto a = yenv.integrate();
      ord.push_back({cands[i].score(), cands[i].width, a, i});
    }
    sort(ord.begin(), ord.end());
    
    ord.erase(unique(ord.begin(), ord.end(), [&](const auto& a,const auto& b) {
      return get<0>(a) == get<0>(b) && get<1>(a) == get<1>(b);
    }), ord.end());
    
    DBG(ord.size());
    
    vector<canvas_t> cands2;
    for (int i=0; i<tc.T; i++) {
      int idx = (i < ord.size()) ? i : 0;
      const auto& canvas(cands[get<3>(ord[idx])]);
      cands2.push_back(canvas);
    }

    return cands2;
  }
  
  vector<canvas_t> generate_layers(double lim_w,double lim_sum,xor128_t& rng) {
    struct state_t {
      int64_t val;
      int c, j;
      bool rot;

      bool operator<(const state_t& st) const {
        return make_tuple(val, c, j, rot) < make_tuple(st.val, st.c, st.j, st.rot);
      }
    };

    vector<pair<canvas_t,vector<layer_t>>> que, que2;
    canvas_t canvas(tc.N);
    vector<layer_t> layers(tc.N);
    que.push_back({canvas, layers});

    range_val_t yenv;
    
    vector<state_t> cands, local_cands, cands2;

    int beam_width = 4000 * pow2(80.0 + 10) / pow2(tc.N + 10);
    DBG(beam_width);

    for (int i=0; i<tc.N; i++) {

      cands.clear();
      for (int c=0; c<que.size(); c++) {
        local_cands.clear();
        const auto& [canvas, layers] = que[c];

        bool first = true;
        for (int j=0; j<tc.N; j++) {
          if (j > 0 && layers[j-1].w == 0) { break; }

          bool ok_layer = false;
          for (int r=0; r<2; r++) {
            bool rot = tc.bs[i].is_landscape();
            int w = tc.bs[i].shorter_side();
            int h = tc.bs[i].longer_side();

            if (r == 1) {
              rot = !rot;
              swap(w, h);
            }

            // out of width limit
            if (layers[j].w + w > lim_w) {
              continue;
            }

            // W(current layer) must not exceed W(back layer).
            if (!first) {
              int w_cur = layers[j].w + w;
              int w_back = layers[j-1].w;
              int num_vars = layers[j].size() + 1 + layers[j-1].size();
              //double margin = sqrt(num_vars) * 2*tc.sigma;
              double margin = sqrt(num_vars) * 2*tc.sigma;
            
              if (w_back - w_cur < margin) {
                continue;
              }
            }

            auto b = canvas.b_with_new_box(tc.bs[i], rot, layers[j].last_index());

            int wi = max(canvas.width, layers[j].w + w);
            int he = max(canvas.height, b);
            if (wi > lim_w || wi+he > lim_sum) {
              continue;
            }

            //int64_t outer_w = min(U, (int)lim_w - wi);
            //int64_t outer_h = min(U, (int)(lim_sum-lim_w) - he);
            
            int64_t gap = (layers[j].size() == 0) ? abs((U+tc.L)/2 - h) : abs(canvas.rect_by_id(layers[j].last_index()).b - b);
            //yenv = canvas.y_envelope;
            //yenv.update(layers[j].w, layers[j].w + w, b);
            //auto a = yenv.integrate();

            local_cands.push_back({canvas.val + gap*w, c, j, rot});
            ok_layer = true;
          }
          if (first && !ok_layer) {
            local_cands.clear();
          } else {
            first = false;
          }
        }
        
        cands.insert(cands.end(), local_cands.begin(), local_cands.end());
      }

      //cerr << i << " " << cands.size() << "/" << width << endl;

      if (i == tc.N-1) {
        // don't shrink
      } else if (cands.size() > beam_width) {
        cands2.clear();
        sort(cands.begin(), cands.end());
        unordered_map<uint64_t,int> used;
        vector<pair<int,int>> pri(cands.size());
        for (int q=0; q<pri.size(); q++) {
          pri[q] = {0, q};
        }
        for (int q=0; q<cands.size(); q++) {
          auto tail_hash = que[cands[q].c].first.tail_hash;
          pri[q].first = used[tail_hash];
          used[tail_hash] ++;
        }
        sort(pri.begin(), pri.end());
        cands2.clear();
        for (int q=0; q<beam_width; q++) {
          cands2.push_back(cands[pri[q].second]);
        }
        cands.swap(cands2);
        //cerr << i << " " << used.size() << endl;
      }

      que2.clear();
      for (const auto st : cands) {
        tie(canvas, layers) = que[st.c];
        
        canvas.put_box_U(tc.bs[i], st.rot, layers[st.j].last_index());
        canvas.val = st.val;
        canvas.memo += format("%d%c", st.j, "|-"[st.rot]);
        canvas.tail_hash = (canvas.tail_hash << 6) | (st.j << 1) | (st.rot);
        canvas.tail_hash &= (1uLL << 42)-1;

        layers[st.j].put_box(tc.bs[i], st.rot);

        que2.push_back({canvas, layers});

        //canvas.draw(tc);
      }

      que.swap(que2);
      //cerr << i << " " << que.size() << endl;
    }

    vector<canvas_t> result;
    for (const auto& [canvas, layers] : que) {
      result.push_back(canvas);
    }
    
    return result;
  }    
  
  void run() {
    DBG(tc.lb_score);

    vector<int> ws;
    int lo = tc.lb_score/2 * 0.95;
    for (int i=0; i<tc.N; i++) {
      for (int j=i; j<tc.N; j++) {
        int w = 0;
        for (int k=i; k<=j; k++) {
          w += tc.bs[i].shorter_side();
        }
        if (w > lo) {
          ws.push_back(w);
        }
      }
    }

    double best_score = 1e234;
    int lim_w, lim_sum;
    canvas_t canvas(tc.N), best_canvas(tc.N);
    xor128_t rng;

    auto cands0 = generate_sequential(tc);
    //cands0.erase(cands0.begin()+1, cands0.end());
    {
      const auto& canvas = *min_element(cands0.begin(), cands0.end(), [&](const auto& a,const auto& b) {
        return a.score() < b.score();
      });
      auto w = canvas.width;
      auto h = canvas.height;
      auto lb = tc.lb_score;
      
      cerr << "seq_rel_w: " << w / (lb/2.0) << endl;
      cerr << "seq_rel_h: " << h / (lb/2.0) << endl;
      cerr << "seq_rel_sum: " << (w+h) / (lb/1.0) << endl;
      cerr << "seq_rel_a: " << (double)w*h / tc.area << endl;
    }
    

    lim_w = (tc.lb_score/2) * 1.1;
    lim_sum = tc.lb_score * 1.06;
    auto cands1 = generate_layers(lim_w, lim_sum, rng);

    DBG(cands0.size());
    DBG(cands1.size());
    auto cands(cands0);
    cands.insert(cands.end(), cands1.begin(), cands1.end());
    DBG(cands.size());
    
    //cands = choose_candidates_sum(cands);
    //cands = choose_candidates_top(cands);
    //cands = choose_candidates_max(cands);
    //cands = choose_candidates_simple(cands);
    cands = choose_candidates_env(cands);
    assert(cands.size() == tc.T);

    cerr << "local_rel_w: " << cands[0].width / (tc.lb_score/2.0) << endl;
    cerr << "local_rel_h: " << cands[0].height / (tc.lb_score/2.0) << endl;
    cerr << "local_rel_sum: " << (cands[0].width + cands[0].height) / (tc.lb_score/1.0) << endl;
    cerr << "local_rel_a: " << (double)cands[0].width*cands[0].height / tc.area << endl;
    
    for (int t=0; t<tc.T; t++) {
      const auto& canvas(cands[t]);
      
      cout << "# " << canvas.width << " " << canvas.height << endl;
      canvas.output(cout);

      int w, h;
      cin >> w >> h;
    }
  }
};

testcase_t rebuild_testcase_with_query(const testcase_t& tc0) {
  testcase_t tc(tc0);
  
  int w, h;
  for (int i=0; i<tc0.N; i++) {
    canvas_t canvas(tc0.N);
    canvas.put_box_U(tc0.bs[i], 0, -1);
    canvas.output(cout);
    cin >> w >> h;
    tc.bs0[i].w = (tc0.bs0[i].w + w)/2;
    tc.bs0[i].h = (tc0.bs0[i].h + h)/2;
  }

  tc.T -= tc.N;
  tc.sigma /= sqrt(2);
  tc.calc_area();
  tc.L = tc.estimate_L();

  for (int i=0; i<tc.N; i++) {
    //tc.bs[i].w = clamp(tc.bs0[i].w, tc.L, U);
    //tc.bs[i].h = clamp(tc.bs0[i].h, tc.L, U);

    tc.bs[i].w = truncated_gaussian_median(tc.bs0[i].w, tc.L, U, tc.sigma);
    tc.bs[i].h = truncated_gaussian_median(tc.bs0[i].h, tc.L, U, tc.sigma);
  }      

  return tc;
}

testcase_t partial_rebuild_testcase_with_query(const testcase_t& tc0) {
  int c = tc0.T - tc0.N;
  if (c <= 0) { return tc0; }

  testcase_t tc(tc0);
    
  vector<pair<int,int>> ord;
  {
    int med = (U + tc.L)/2;
    for (int i=0; i<tc0.N; i++) {
      int w = tc0.bs0[i].w;
      int h = tc0.bs0[i].h;
      ord.push_back({min({w-tc0.L, U-w, h-tc0.L, U-h}), i});
    }
    if (ord.size() > c) {
      ord.resize(c);
    }
  }
  
  int w, h;
  //for (int i=0; i<tc0.N; i++) {
  for (auto [val, i] : ord) {
    canvas_t canvas(tc0.N);
    canvas.put_box_U(tc0.bs[i], 0, -1);
    canvas.output(cout);
    cin >> w >> h;
    tc.bs0[i].w = (tc0.bs0[i].w + w)/2;
    tc.bs0[i].h = (tc0.bs0[i].h + h)/2;
  }

  tc.T -= ord.size();
  //tc.sigma /= sqrt(2);
  tc.sigma *= sqrt(((tc.N-c) + 0.5*c)/tc.N);
  tc.calc_area();
  tc.L = tc.estimate_L();

  for (int i=0; i<tc.N; i++) {
    //tc.bs[i].w = clamp(tc.bs0[i].w, tc.L, U);
    //tc.bs[i].h = clamp(tc.bs0[i].h, tc.L, U);

    tc.bs[i].w = truncated_gaussian_median(tc.bs0[i].w, tc.L, U, tc.sigma);
    tc.bs[i].h = truncated_gaussian_median(tc.bs0[i].h, tc.L, U, tc.sigma);
  }      

  return tc;
}

testcase_t refine_testcase(const testcase_t& tc0) {
  int c = tc0.T - tc0.N;
  //c = min(tc0.N, c);
  if (c <= 0) { return tc0; }

  testcase_t tc(tc0);
    
  vector<vector<int>> ws(tc0.N);
  vector<vector<int>> hs(tc0.N);
  vector<pair<int,int>> ord;
  {
    int med = (U + tc.L)/2;
    for (int i=0; i<tc0.N; i++) {
      int w = tc0.bs0[i].w;
      int h = tc0.bs0[i].h;

      ws[i].push_back(w);
      hs[i].push_back(h);
      ord.push_back({min({w-tc0.L, U-w, h-tc0.L, U-h}), i});
    }
  }
  sort(ord.begin(), ord.end());
  
  int w, h;
  for (int lp=0; lp<c; lp++) {
    int i = (ord[lp].second % ord.size());
    canvas_t canvas(tc0.N);
    canvas.put_box_U(tc0.bs[i], 0, -1);
    canvas.output(cout);
    cin >> w >> h;
    ws[i].push_back(w);
    hs[i].push_back(h);
  }

  double sum_weight = 0;
  for (int i=0; i<tc.N; i++) {
    double sum = 0;
    for (auto w : ws[i]) { sum += w; }
    tc.bs0[i].w = sum / ws[i].size();

    sum = 0;
    for (auto h : hs[i]) { sum += h; }
    tc.bs0[i].h = sum / hs[i].size();

    sum_weight += 1.0/ws[i].size();
  }

  tc.T -= c;
  tc.sigma *= sqrt(sum_weight/tc.N);
  tc.calc_area();
  tc.L = tc.estimate_L();

  for (int i=0; i<tc.N; i++) {
    //tc.bs[i].w = clamp(tc.bs0[i].w, tc.L, U);
    //tc.bs[i].h = clamp(tc.bs0[i].h, tc.L, U);

    tc.bs[i].w = truncated_gaussian_median(tc.bs0[i].w, tc.L, U, tc.sigma);
    tc.bs[i].h = truncated_gaussian_median(tc.bs0[i].h, tc.L, U, tc.sigma);
  }      

  return tc;
}

int main(int argc,const char *argv[]) {
  timer_start();

  string fname;
  for (int i=1; argv[i]; i++) {
    if (strcmp(argv[i], "--cheat") == 0) {
      opt_cheat = true;
      i++;
      fname = argv[i];
    } else if (strcmp(argv[i], "--self-check") == 0) {
      opt_self_check = true;
      i++;
      fname = argv[i];
    }
  }
       
  testcase_t tc, tc_truth;
  tc.read(cin);
  if (opt_cheat) {
    tc.read_cheat(fname);
  } else if (opt_self_check) {
    tc_truth.read_truth(fname);
  }

  auto tc0(tc);
#if 0  
  if (tc.T > 2*tc.N) {
    tc = rebuild_testcase_with_query(tc);
  } else {
    tc = partial_rebuild_testcase_with_query(tc);
  }
#endif
  tc = refine_testcase(tc);
  tc.build_w_loss_table();
  DBG(tc.T);
  DBG(tc.sigma);
  
  solver_t slv(tc, tc_truth);
  slv.run();

  double tm_total = timer_now();
  DBG(tm_total);
}
