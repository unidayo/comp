#include <iostream>
#include <cstdlib>

#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <algorithm>
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
    //return duration;
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

const double TIME_LIMIT = 5.0;

/* insert here */
bool opt_verbose = false;
bool opt_draw = false;
int opt_lp = -1;

vector<int> cut_count;

const int SZ = 10000;
double pow2(double x) { return x*x; }

struct rect_t {
  int l, t, r, b;

  rect_t() : l(0), t(0), r(0), b(0) {}
  rect_t(int l,int t,int r,int b) : l(l), t(t), r(r), b(b) {}

  void enlarge(int margin) {
    l -= margin;
    t -= margin;
    r += margin;
    b += margin;
  }

  void enlargeD(int margin,int dir) {
    switch (dir) {
    case 0: l -= margin; break;
    case 1: t -= margin; break;
    case 2: r += margin; break;
    case 3: b += margin; break;
    }
  }

  void transpose() {
    swap(l, t);
    swap(r, b);
  }

  int area() const {
    return (l >= r || t >= b) ? 0 : (r-l)*(b-t);
  }

  int distance(const rect_t& rect) const {
    int dx = (l > rect.r) ? l - rect.r : (r < rect.l) ? rect.l - r : 0;
    int dy = (t > rect.b) ? t - rect.b : (b < rect.t) ? rect.b - t : 0;

    return max(dx, dy);
  }

  int w() const { return r - l; }
  int h() const { return b - t; }

  bool is_overlap(const rect_t& rect) const {
    return !((area() > 0) &&
             (rect.area() > 0) &&
             (((r <= rect.l) || (l >= rect.r)) ||
              ((b <= rect.t) || (t >= rect.b))));
  }

  bool is_included(const rect_t& rect) const {
    return (l >= rect.l && r <= rect.r && t >= rect.t && b <= rect.b);
  }

  bool operator==(const rect_t& rect) const {
    return (l == rect.l && t == rect.t && r == rect.r && b == rect.b);
  }
  bool operator!=(const rect_t& rect) const {
    return !(*this == rect);
  }

  bool operator<(const rect_t& rect) const {
    return make_tuple(area(), l,t,r,b) < make_tuple(rect.area(), rect.l, rect.t, rect.r, rect.b);
  }
};
ostream& operator<<(ostream& os,const rect_t& rect) {
  os << format("(%4d,%4d)-(%4d,%4d)", rect.l, rect.t, rect.r, rect.b);
  return os;
}

struct ad_t {
  int x, y, r;
  vector<int> ord;

  int distance(const ad_t& ad) const {
    int dx = abs(x - ad.x);
    int dy = abs(y - ad.y);

    return max(dx, dy);
  }
  
  bool is_included(const rect_t& rect) const {
    return (x >= rect.l && x < rect.r && y >= rect.t && y < rect.b);
  }

  void transpose() {
    swap(x, y);
  }

  double calc_score(const rect_t& rect) const {
    if (is_included(rect)) {
      int s = rect.area();
      return 1 - pow2(1 - (double)min(s, r)/max(s, r));
    } else {
      return 0.0;
    }
  }

  double calc_scoreQ(const rect_t& rect) const {
    if (is_included(rect)) {
      int s = rect.area();
      if (s > r) {
        //return 1.0 + 1e-9 * (double)s/r;
        return 1.0;
      } else {
        return 1 - pow2(1 - (double)s/r);
      }
    } else {
      return 0.0;
    }
  }
};
ostream& operator<<(ostream& os,const ad_t& ad) {
  os << format("(%4d,%4d:%8d)", ad.x, ad.y, ad.r);
  return os;
}

double tm_bo = 0;
// bb: bounding_box
vector<rect_t> possible_bbs(const vector<rect_t>& rects,const ad_t& ad,int idx) {
  static vector<rect_t> que, que2;
  //tm_bo -= timer_now();

  que.clear();
  que.push_back(rect_t(0, 0, SZ, SZ));

  auto smart_push = [](vector<rect_t>& que2,const rect_t& rect) {
    for (int i=0; i<que2.size(); i++) {
      if (rect.is_included(que2[i])) {
        return;
      }
      if (que2[i].is_included(rect)) {
        que2[i] = rect;
        return;
      }
    }

    que2.push_back(rect);
  };

  const int N = rects.size();
  //for (int i=0; i<rects.size(); i++) {
  for (int ii=0; ii<ad.ord.size(); ii++) {
    if (que.size() == 1 && que[0].area() == rects[idx].area()) {
      //tm_bo += timer_now();      
      return que;
    }
    
    int i = ad.ord[ii];
    assert(que.size() < 10000);
    const auto& rect(rects[i]);
    if (opt_verbose) {
      cerr << i << " " << que.size() << endl;
    }
    
    if (i == idx)
      continue;

    if (all_of(que.begin(), que.end(), [&](const rect_t& b) {
          return !b.is_overlap(rect);
        }))
      continue;

    que2.clear();
    for (const auto& b : que) {
      if (!b.is_overlap(rect)) {
        que2.push_back(b);
        continue;
      }

      if (b.l < rect.r) {
        auto b2(b);
        b2.l = rect.r;
        if (b2.area() > 0 && ad.is_included(b2)) {
          smart_push(que2, b2);
          //cut_count[idx*N+i] ++;
        }
      }
      
      if (b.r > rect.l) {
        auto b2(b);
        b2.r = rect.l;
        if (b2.area() > 0 && ad.is_included(b2)) {
          smart_push(que2, b2);
          //cut_count[idx*N+i] ++;
        }
      }
      
      if (b.t < rect.b) {
        auto b2(b);
        b2.t = rect.b;
        if (b2.area() > 0 && ad.is_included(b2)) {
          smart_push(que2, b2);
          //cut_count[idx*N+i] ++;
        }
      }
      
      if (b.b > rect.t) {
        auto b2(b);
        b2.b = rect.t;
        if (b2.area() > 0 && ad.is_included(b2)) {
          smart_push(que2, b2);
          //cut_count[idx*N+i] ++;
        }
      }
    }

    que.clear();
    for (const auto& b : que2) {
      if (all_of(que2.begin(), que2.end(), [&](const rect_t& b2) {
            return b == b2 || !b.is_included(b2);
          })) {
        que.push_back(b);
      }
    }
  }
  //tm_bo += timer_now();

  sort(que.begin(), que.end());
  reverse(que.begin(), que.end());
  return que;
}

rect_t subrect(const rect_t& bb, int w,int h, const ad_t& ad) {
  auto rect(bb);

  if (bb.l == 0) {
    rect.l = 0;
    rect.r = w;
  } else if (bb.r == SZ) {
    rect.l = SZ-w;
    rect.r = SZ;
  } else {
    rect.l = bb.l + (bb.w() - w)/2;
    rect.r = rect.l + w;
  }
  
  if (bb.t == 0) {
    rect.t = 0;
    rect.b = h;
  } else if (bb.b == SZ) {
    rect.t = SZ-h;
    rect.b = SZ;
  } else {
    rect.t = bb.t + (bb.h() - h)/2;
    rect.b = rect.t + h;
  }

  assert(rect.w() == w);
  assert(rect.h() == h);
  assert(rect.is_included(bb));
  
  if (rect.l > ad.x) {
    int shift = rect.l - ad.x;
    rect.l -= shift;
    rect.r -= shift;
  }
  if (rect.r <= ad.x) {
    int shift = rect.r - (ad.x+1);
    rect.l -= shift;
    rect.r -= shift;
  }
  if (rect.t > ad.y) {
    int shift = rect.t - ad.y;
    rect.t -= shift;
    rect.b -= shift;
  }
  if (rect.b <= ad.y) {
    int shift = rect.b - (ad.y+1);
    rect.t -= shift;
    rect.b -= shift;
  }

  assert(rect.w() == w);
  assert(rect.h() == h);
  assert(rect.is_included(bb));
  
  return rect;
}  

rect_t random_subrect(const rect_t& bb, int w,int h, const ad_t& ad,xor128_t& rng) {
  auto rect(bb);

  if (bb.l == 0) {
    rect.l = 0;
    rect.r = w;
  } else if (bb.r == SZ) {
    rect.l = SZ-w;
    rect.r = SZ;
  } else {
    int q = rng.get(3);
    rect.l =
      (q == 0) ? bb.l :
      (q == 1) ? bb.r - w : bb.l + (bb.w() - w)/2;
    rect.r = rect.l + w;
  }
  
  if (bb.t == 0) {
    rect.t = 0;
    rect.b = h;
  } else if (bb.b == SZ) {
    rect.t = SZ-h;
    rect.b = SZ;
  } else {
    int q = rng.get(3);
    rect.t =
      (q == 0) ? bb.t :
      (q == 1) ? bb.b - h : bb.t + (bb.h() - h)/2;
    rect.b = rect.t + h;
  }

  assert(rect.w() == w);
  assert(rect.h() == h);
  assert(rect.is_included(bb));
  
  if (rect.l > ad.x) {
    int shift = rect.l - ad.x;
    rect.l -= shift;
    rect.r -= shift;
  }
  if (rect.r <= ad.x) {
    int shift = rect.r - (ad.x+1);
    rect.l -= shift;
    rect.r -= shift;
  }
  if (rect.t > ad.y) {
    int shift = rect.t - ad.y;
    rect.t -= shift;
    rect.b -= shift;
  }
  if (rect.b <= ad.y) {
    int shift = rect.b - (ad.y+1);
    rect.t -= shift;
    rect.b -= shift;
  }

  assert(rect.w() == w);
  assert(rect.h() == h);
  assert(rect.is_included(bb));
  
  return rect;
}  

int cnt_unsafe = 0;
double unsafe_calc_score(const vector<ad_t>& ads,const vector<rect_t>& rects) {
  //cnt_unsafe ++;
  double sum = 0.0;
  for (int i=0; i<ads.size(); i++) {
    sum += ads[i].calc_score(rects[i]);
  }

  return sum/ads.size();
}

double unsafe_calc_scoreQ(const vector<ad_t>& ads,const vector<rect_t>& rects) {
  //cnt_unsafe ++;
  double sum = 0.0;
  for (int i=0; i<ads.size(); i++) {
    if (rects[i].area() > ads[i].r) {
      sum += 1.0;
    } else
      sum += ads[i].calc_score(rects[i]); {
    }
  }

  return sum/ads.size();
}

bool is_valid_solution(const vector<rect_t>& rects,bool verbose=false) {
  const int N = rects.size();  
  for (int i=0; i<N; i++) {
    if (rects[i].l < 0 || rects[i].r > SZ || rects[i].t < 0 || rects[i].b > SZ) {
      if (verbose) { cerr << "Illegal position: #" << i << " " << rects[i] << endl; }
      return false;
    }
    for (int j=i+1; j<N; j++)
      if (rects[i].is_overlap(rects[j])) {
        if (verbose) { cerr << "Overlap: "
                            << " #" << i << rects[i]
                            << " #" << j << rects[j] << endl; }
        return false;
      }
  }
  return true;
}

bool is_valid_solutionI(const vector<rect_t>& rects,int i,bool verbose=false) {
  const int N = rects.size();  
  if (rects[i].l < 0 || rects[i].r > SZ || rects[i].t < 0 || rects[i].b > SZ) {
    if (verbose) { cerr << "Illegal position: #" << i << " " << rects[i] << endl; }
    return false;
  }

  for (int j=0; j<N; j++) {
    if (i == j)
      continue;
    if (rects[i].is_overlap(rects[j])) {
      if (verbose) { cerr << "Overlap: "
                          << " #" << i << rects[i]
                          << " #" << j << rects[j] << endl; }
      return false;
    }
  }
  return true;
}

rect_t smallest_rectangle(const ad_t& ad) {
  rect_t rect;
  rect.l = ad.x;
  rect.t = ad.y;
  rect.r = ad.x+1;
  rect.b = ad.y+1;

  return rect;
}

string color_string(int target,int actual) {
  double ratio = (double)min(target, actual)/max(target, actual);
  int err = 255-floor(ratio*255.99);
  int g = 255 - err;
  int r = 0;
  int b = 0;
  if (actual < target) {
    r = err;
  } else {
    g = b = 255;
  }

  return format("#%02x%02x%02x", r, g, b);
}

void draw(const vector<ad_t>& ads,const vector<rect_t>& rects,string msg="") {
  static int called = 0;
  called ++;
  ofstream ofs(format("images/out%04d.svg", called));
  assert(ofs);
  ofs << "<?xml version='1.0' standalone='no'?>" << endl;
  ofs << "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' viewBox='-300 -500 10500 10500'>" << endl;
  ofs << format("  <rect x='0' y='0' width='10000' height='10000' stroke='black' stroke-width='1' fill='#cccccc' />") << endl;
  for (int i=0; i<ads.size(); i++) {
    const auto& rect(rects[i]);
    const auto& ad(ads[i]);

    string col = color_string(ads[i].r, rect.area());
    
    ofs << format("  <rect x='%d' y='%d' width='%d' height='%d' stroke='black' stroke-width='1' fill='%s' />",
                  rect.l, rect.t,
                  (rect.r-rect.l), (rect.b-rect.t),
                  col.c_str()) << endl;
  }
  for (int i=0; i<ads.size(); i++) {
    const auto& rect(rects[i]);
    const auto& ad(ads[i]);
    double sc = ad.calc_score(rect);
    bool almost_perfect = (rect.area() > ad.r || round(sc * 1000) == 1000);
    
    //string col = format("#%06x", (((i&4)<<14)+((i&2)<<7)+(i%1))*128);
    string col = "#000000";
    
    ofs << format("  <rect x='%d' y='%d' width='%d' height='%d' stroke='black' stroke-width='1' fill='%s' />", ad.x-30, ad.y-30, 60, 60, col.c_str()) << endl;
    ofs << format("  <text font-size='200' text-anchor='middle' x='%d' y='%d'>%d</text>", (rect.l+rect.r)/2, (rect.t+rect.b)/2, i) << endl;
    ofs << format("  <line x1='%d' y1='%d' x2='%d' y2='%d' stroke='black' stroke-width='1' />", ad.x, ad.y, (rect.l+rect.r)/2, (rect.t+rect.b)/2) << endl;
    if (!almost_perfect) {
      ofs << format("  <text font-size='100' text-anchor='middle' x='%d' y='%d'>%.1f</text>", (rect.l+rect.r)/2, (rect.t+rect.b)/2+200, (1 - sc)*100) << endl;
    }
  }
  ofs << format("  <text font-size='200' text-anchor='middle' x='5000' y='-250'>N: %d   Score: %.6f(%.6f)</text>", ads.size(), 100*unsafe_calc_score(ads, rects), 100*unsafe_calc_scoreQ(ads, rects)) << endl;
  ofs << format("  <text font-size='170' text-anchor='middle' x='5000' y='-50'>%s</text>", msg.c_str()) << endl;
  ofs << "</svg>" << endl;
}

vector<int> find_neighbors(rect_t b,const vector<rect_t>& rects) {
  b.enlarge(1);
  
  vector<int> v;
  for (int i=0; i<rects.size(); i++)
    if (rects[i].is_overlap(b))
      v.push_back(i);

  return v;
}

vector<int> find_neighbors1D(rect_t b,const vector<rect_t>& rects,int dir) {
  switch (dir) {
  case 0: b.l --; break;
  case 1: b.t --; break;
  case 2: b.r ++; break;
  case 3: b.b ++; break;
  }
  
  vector<int> v;
  for (int i=0; i<rects.size(); i++)
    if (rects[i].is_overlap(b))
      v.push_back(i);

  return v;
}

rect_t find_some_rectangle(const vector<rect_t>& sol,const ad_t& ad, int i,
                           double alpha, xor128_t& rng, const rect_t& bb) {

  double target_area = alpha * ad.r;  
  
  int W = bb.w();
  int H = bb.h();

  rect_t best_rect(sol[i]);
  int sum_wei = 0;

  int best_type = -1;
  for (int type=0; type<4; type++) {
    int w, h;

    if (type == 0) {
      w = W;
      h = min(H, (int)round(target_area / w));
    } else if (type == 1) {
      h = H;
      w = min(W, (int)round(target_area / h));
    } else if (type == 2) {
      double ratio = min(1.0, sqrt((target_area)/(H*W)));
      double best_err = 1e10;
      for (int dh=0; dh<2; dh++)
        for (int dw=0; dw<2; dw++) {
          double err = abs(target_area - floor(W * ratio + dw)*floor(H * ratio + dh));
          if (err < best_err) {
            best_err = err;
            w = round(W * ratio + dw);
            h = round(H * ratio + dh);
          }
        }
      w = clamp(w, 1, W);
      h = clamp(h, 1, H);
    } else if (type == 3) {
      double margin = 0.5 * ((W+H) - sqrt((double)(W-H)*(W-H) + 4*target_area));
      margin = max(0.0, margin);

      w = W - margin;
      h = H - margin;
    }

    auto rect = random_subrect(bb, w, h, ad, rng);
    if (rect.area() == 0)
      continue;

    double sc = ad.calc_score(rect);
    int wei = 1;
    sum_wei += wei;
    if (sum_wei*rng.get_double() < wei) {
      best_rect = rect;
      best_type = type;
    }

    if (W*H < target_area)
      break;
  }

  //cerr << best_type << " " << best_rect.area()/target_area << endl;

  return best_rect;
}

rect_t find_some_rectangle(const vector<rect_t>& sol,const ad_t& ad, int i,
                           double alpha, xor128_t& rng) {

  auto bbs = possible_bbs(sol, ad, i);

  double target_area = alpha * ad.r;
      
  assert(!bbs.empty());
  for (int j=bbs.size()-1; j>=1; j--) {
    if (bbs[j].area() < target_area) {
      bbs.pop_back();
    } else {
      break;
    }
  }

  auto bb = rng.sample(bbs);
  
  return find_some_rectangle(sol, ad, i, alpha, rng, bb);
}

rect_t find_some_rectangleA(const vector<rect_t>& sol,const ad_t& ad, int i) {

  int target_area = ad.r;
  auto calc_score = [&](int a) {
    double ratio = (double)min(a, target_area) / max(a, target_area);
    return 1 - (1 - ratio)*(1 - ratio);
  };

  auto bbs = possible_bbs(sol, ad, i);
  assert(!bbs.empty());
  if (bbs[0].area() <= ad.r) {
    return bbs[0];
  }

  rect_t best_rect(sol[i]);
  rect_t rect(sol[i]);
  double best_score = ad.calc_score(sol[i]);
  for (const auto& bb : bbs) {
  
    int W = bb.w();
    int H = bb.h();

    int w, h;
    if (W < H) {
      for (int w=1; w<=W; w++) {
        int h = clamp(target_area/w, 1, H);
        for (int lp=0; lp<2; lp++) {
          double sc = calc_score(w*h);
          if (sc > best_score) {
            best_score = sc;
            best_rect = subrect(bb, w, h, ad);
            //cerr << best_score << " " << w << "x" << h << " " << best_rect << endl;
            if (w*h == target_area) {
              //return best_rect;
            }
          }
          h = min(h+1, H);
        }
      }
    } else {
      for (int h=1; h<=H; h++) {
        int w = clamp(target_area/h, 1, W);
        for (int lp=0; lp<2; lp++) {
          double sc = calc_score(w*h);
          if (sc > best_score) {
            best_score = sc;
            best_rect = subrect(bb, w, h, ad);
            //cerr << best_score << " " << w << "x" << h << " " << best_rect << endl;
            if (w*h == target_area) {
              //return best_rect;
            }
          }
          w = min(w+1, W);
        }
      }
    }
  }

  return best_rect;
}

vector<rect_t> improve_by_shifting_borders_subr(const vector<ad_t>& ads, vector<rect_t> rects) {
  map<int,vector<int>> ls;
  map<int,vector<int>> rs;
  set<int> xss;

  for (int i=0; i<ads.size(); i++) {
    ls[rects[i].l].push_back(i);
    rs[rects[i].r].push_back(i);
    xss.insert(rects[i].l);
    xss.insert(rects[i].r);
  }

  xss.insert(0);
  xss.insert(SZ);
  vector<int> xs(xss.begin(), xss.end());
  rect_t rect;

  auto rects0(rects);

  vector<int> best_xs;
  for (int i=1; i+1<xs.size(); i++) {
    int x1 = xs[i];
    if (!ls.count(x1) || !rs.count(x1))
      continue;
    
    int x0 = xs[i-1];
    int x2 = xs[i+1];

    for (int i : rs[x1]) {
      x0 = max(x0, ads[i].x+1);
    }
    for (int i : ls[x1]) {
      x2 = min(x2, ads[i].x);
    }

    double best_score = 0.0;
    best_xs.clear();
    best_xs.push_back(x1);
    for (int x=x0; x<=x2; x++) {
      double sc = 0;
      for (int i : rs[x1]) {
        rect = rects[i];
        rect.r = x;
        sc += ads[i].calc_scoreQ(rect);
      }
      for (int i : ls[x1]) {
        rect = rects[i];
        rect.l = x;
        sc += ads[i].calc_scoreQ(rect);
      }

      if (sc > best_score) {
        best_score = sc;
        best_xs.clear();
        best_xs.push_back(x);
      } else if (sc == best_score) {
        best_xs.push_back(x);
      }
    }
    int best_x = best_xs[best_xs.size()/2];

    for (int i : rs[x1]) {
      rects[i].r = best_x;
    }
    for (int i : ls[x1]) {
      rects[i].l = best_x;
    }

    if (best_x == xs[i-1] || best_x == xs[i+1]) {
      rs[best_x].insert(rs[best_x].end(), rs[x1].begin(), rs[x1].end());
      ls[best_x].insert(ls[best_x].end(), ls[x1].begin(), ls[x1].end());
    } else {
      rs[best_x] = rs[xs[i]];
      rs.erase(xs[i]);
      ls[best_x] = ls[xs[i]];
      ls.erase(xs[i]);
      xs[i] = best_x;
    }

#if 0    
    if (!is_valid_solution(rects)) {
      DBG(x0);
      DBG(x1);
      DBG(x2);
      DBG(best_x);
      for (int i : rs[x1])
        cerr << "R: " << i << " " << rects[i] << endl;
      for (int i : ls[x1])
        cerr << "L: " << i << " " << rects[i] << endl;

      for (int i=0; i<ads.size(); i++)
        for (int j=i+1; j<ads.size(); j++)
          if (rects[i].is_overlap(rects[j])) {
            cerr << "X: " << i << "," << j << " " << rects[i] << " " << rects[j] << endl;
            cerr << "   " << i << "," << j << " " << rects0[i] << " " << rects0[j] << endl;
          }
      
      assert(is_valid_solution(rects));
    }
#endif
    
  }

  return rects;
}
  
vector<rect_t> improve_by_shifting_borders(vector<ad_t> ads, vector<rect_t> rects) {
  rects = improve_by_shifting_borders_subr(ads, rects);
  for (auto& a : ads)
    a.transpose();
  for (auto& r : rects)
    r.transpose();
  rects = improve_by_shifting_borders_subr(ads, rects);
  for (auto& r : rects)
    r.transpose();
  return rects;
}

struct group_shift_t {
  struct group_t {
    vector<int> rs, ls;
    int border;
    int t, b;

    void clear() { rs.clear(); ls.clear(); }
  };

  vector<ad_t> ads;
  vector<rect_t> rects;
  vector<group_t> gs;
  map<int,vector<int>> rs, ls;
  set<int> xs;

  group_shift_t(const vector<ad_t>& ads, const vector<rect_t>& rects) :
    ads(ads), rects(rects) {
    xs.insert(0);
    xs.insert(SZ);
    for (int i=0; i<rects.size(); i++) {
      const auto& rect(rects[i]);
      rs[rect.r].push_back(i);
      ls[rect.l].push_back(i);
      xs.insert(rect.r);
      xs.insert(rect.l);
    }
    for (auto& pa : rs) { sort_by_y(pa.second); }
    for (auto& pa : ls) { sort_by_y(pa.second); }

    group_t g;
    for (int x : xs) {
      if (!rs.count(x) || !ls.count(x)) {
        //continue;
      }

      const auto& R = rs[x];
      const auto& L = ls[x];

      g.clear();
      g.border = x;
      int ri = 0;
      int li = 0;
#if 0      
      cerr << "=== " << x << endl;
      DBG(R);
      DBG(L);
#endif
      rect_t empty;
      while (ri < R.size() && li < L.size()) {
        int t = min(rects[R[ri]].t, rects[L[li]].t);
        int b = min(rects[R[ri]].b, rects[L[li]].b);
        while (true) {
#if 0          
          cerr << b
               << " " << ri << ":" << (ri < R.size() ? rects[R[ri]] : empty)
               << " " <<  li  << ":" << (li < L.size() ? rects[L[li]] : empty) << endl;
#endif
          
          if (ri < R.size() && rects[R[ri]].t < b) {
            b = max(b, rects[R[ri]].b);
            g.rs.push_back(R[ri]);
            ri ++;
          } else if (li < L.size() && rects[L[li]].t < b) {
            b = max(b, rects[L[li]].b);
            g.ls.push_back(L[li]);
            li ++;
          } else {
            break;
          }
        }

        if (!g.rs.empty() || !g.ls.empty()) {
          g.border = x;
          g.t = t;
          g.b = b;
          gs.push_back(g);
#if 0
          cerr << g.rs << " " << g.ls << endl;
#endif
          g.clear();
        }
      }
    }
  }

  void improve() {
    int lp = 0;
    for (auto& g : gs) {
      lp++;
      int x0, x1, x2;
      tie(x0, x2) = find_border_range(g);
      x1 = g.border;

#if 0
      cerr << "=== " << endl;
      cerr << g.border << " " << g.rs << " " << g.ls << endl;
      cerr << "x_range: " << x0 << " " << x2 << endl;
#endif
      double best_score = calc_scoreQ(g);
      vector<int> best_xs = {x1};
      for (int x=x0; x<=x2; x++) {
        set_border_tmp(g, x);        
        double sc = calc_scoreQ(g);
        if (sc >= best_score) {
          if (sc > best_score) {
            best_score = sc;
            best_xs.clear();
          }
          best_xs.push_back(x);
        }
      }

      int best_x = best_xs[best_xs.size()/2];
      //DBG(best_xs.size());
      //DBG(best_x);

      xs.insert(best_x);
      set_border(g, best_x);

#if 0      
      if (!is_valid_solution(rects, true)) {
        assert(false);
      }
#endif
    }
  }

  void set_border_tmp(const group_t& g,int x) {
    for (int i : g.rs) { rects[i].r = x; }
    for (int i : g.ls) { rects[i].l = x; }
  }

  void set_border(group_t& g,int x) {
    auto vfind = [](const vector<int>& v,int x) {
      auto it = find(v.begin(), v.end(), x);
      if (it == v.end()) {
        DBG(v);
        DBG(x);
      }
      assert(it != v.end());
      return it;
    };

    for (int i : g.rs) { rs[g.border].erase(vfind(rs[g.border], i)); }
    for (int i : g.ls) { ls[g.border].erase(vfind(ls[g.border], i)); }
    g.border = x;
    for (int i : g.rs) { rects[i].r = x; rs[x].push_back(i); }
    for (int i : g.ls) { rects[i].l = x; ls[x].push_back(i); }
  }

  double calc_scoreQ(const group_t& g) {
    double sum = 0;
    for (int i : g.rs) { sum += ads[i].calc_scoreQ(rects[i]); }
    for (int i : g.ls) { sum += ads[i].calc_scoreQ(rects[i]); }
    return sum;
  }

  pair<int,int> find_border_range(const group_t& g) {
    int x0 = 0;
    int x2 = SZ;

    for (int i : g.rs) { x0 = max(x0, ads[i].x+1); }
    for (int i : g.ls) { x2 = min(x2, ads[i].x); }

    [&](){
      for (auto it = rs.find(g.border); it!=rs.end() && it->first > x0; it--) {
        for (auto i : it->second) {
          if (rects[i].b > g.t && rects[i].t < g.b && find(g.rs.begin(), g.rs.end(), i) == g.rs.end()) {
            x0 = it->first;
            return;
          }
        }
      }
    }();

    [&](){
      for (auto it = ls.find(g.border); it!=ls.end() && it->first < x2; it++) {
        for (auto i : it->second) {
          if (rects[i].b > g.t && rects[i].t < g.b && find(g.ls.begin(), g.ls.end(), i) == g.ls.end()) {
            x2 = it->first;
            return;
          }
        }
      }
    }();

    return {x0, x2};
  }

  void sort_by_y(vector<int>& v) {
    sort(v.begin(), v.end(), [&](int a,int b) {
        return rects[a].t < rects[b].t;
      });
  }

};

vector<rect_t> group_shift(vector<ad_t> ads, vector<rect_t> rects) {

  for (int it=0; it<2; it++) {
    group_shift_t gs(ads, rects);
    
    gs.improve();
    rects = gs.rects;

    for (auto& a : ads)
      a.transpose();
    for (auto& r : rects)
      r.transpose();
  }
  
  return rects;
}

void push_u(const vector<ad_t>& ads, vector<rect_t>& rects,int a,int dir) {
  vector<int> neighbors;
  for (int i=0; i<4; i++) {
    neighbors = find_neighbors1D(rects[a], rects, (dir+i)%4);
    if (neighbors.size() > 1) {
      dir = (dir+i) % 4;
      break;
    }
  }

  if (neighbors.size() <= 1)
    return;

  vector<int> A, B;
  B = neighbors;
  B.erase(find(B.begin(), B.end(), a));
  for (auto b : B) {
    auto ret = find_neighbors1D(rects[b], rects, dir^2);
    A.insert(A.end(), ret.begin(), ret.end());
    A.erase(find(A.begin(), A.end(), b));
  }
  sort(A.begin(), A.end());
  A.erase(unique(A.begin(), A.end()), A.end());
  vector<int> C(A);
  C.insert(C.end(), B.begin(), B.end());

  double best_score = 0.0;
  for (auto c : C) { best_score += ads[c].calc_scoreQ(rects[c]); }

  //while (true) {
  int width = 100;
  for (int a : A) {
    auto rect(rects[a]);    
    rect.enlargeD(width, dir);

    for (int i=0; i<rects.size(); i++) {
      if (find(C.begin(), C.end(), i) != C.end())
        continue;
      if (rect.is_overlap(rects[i])) {
        width --;
        rect.enlargeD(-1, dir);
        i --;
        if (width == 0)
          return;
      }
    }
  }
  //cerr << "a: " << a << " " << A << " " << B << " " << width << ":width" << endl;

  for (int lp=0; lp<width; lp++) {
    for (auto a : A) { rects[a].enlargeD(1, dir); }
    for (auto b : B) { rects[b].enlargeD(-1, dir^2); }

    if (false && !is_valid_solution(rects, true)) {
      draw(ads, rects);
      cerr << "a: " << a << " " << A << " " << B << " " << width << ":width" << endl;
      assert(false);
    }

    double score = 0.0;
    for (auto c : C) {
      auto sc = ads[c].calc_scoreQ(rects[c]);
      if (sc <= 0.0) {
        score = -1e234;
        break;
      }
      score += sc;
    }

    if (score >= best_score) {
      best_score = score;

    } else {
      for (auto a : A) { rects[a].enlargeD(-1, dir); }
      for (auto b : B) { rects[b].enlargeD(1, dir^2); }

      if (lp >= 1) {
        //cerr << A << " " << B << endl;
      }

      if (false && !is_valid_solution(rects)) {
        DBG(a);
        DBG(dir);
        DBG(A);
        DBG(B);
        DBG(lp);
        draw(ads, rects, "after");        

        for (auto a : A) { cerr << "a: " << a << " " << rects[a] << endl; }
        for (auto b : B) { cerr << "b: " << b << " " << rects[b] << endl; }

        
        assert(is_valid_solution(rects, true));
      }

      return;
    }
  }

  //assert(is_valid_solution(rects, true));
}

vector<rect_t> solve(vector<ad_t> ads) {
  const int N = ads.size();
  DBG(N);
  
  vector<rect_t> sol(ads.size());

  for (int i=0; i<N; i++) {
    sol[i] = smallest_rectangle(ads[i]);
  }

  for (int i=0; i<N; i++) {
    vector<tuple<int,int>> ord;
    for (int j=0; j<N; j++) {
      if (i != j) {
        ord.push_back({ads[i].distance(ads[j]), j});
      }
    }
    sort(ord.begin(), ord.end());
    for (const auto& tu : ord)
      ads[i].ord.push_back(get<1>(tu));
  }
  cut_count.resize(N*N);

  double cur_score = unsafe_calc_score(ads, sol);
  auto best_sol(sol);
  auto best_score = cur_score;

  auto sol2(sol);

  double inv_N = 1.0/N;
  int lp;
  double checkpoint = 0.0;
  
  xor128_t rng(123456);
  int hill_climb = 0;
  double tm0 = timer_now();
  DBG(tm0);

  sa_t sa(1e-3, 1e-6);
  int qcc = 0;

  for (lp=0; hill_climb<5; lp++) {
    double tm = timer_now();
    double progress = (tm - tm0)/(0.9*TIME_LIMIT - tm0);
    if (opt_lp > 5) {
      progress = (double) lp / (opt_lp - 5);
    }
    sa.set_progress(progress);

    hill_climb += (progress >= 1);
    if (hill_climb) {
      cur_score = best_score;
      sol = best_sol;
    }
    
    //double alpha = min(1.0, 0.8+0.21*progress);
    double alpha = min(1.0, 0.5+0.6*progress);
    if (progress > checkpoint) {
      double eff = best_score/(1-(1-alpha)*(1-alpha));
      cerr << format("%.3f:p  %.2f:a  %d:lp\t%.5f:cur\t%.5f:best\t(%.5f)",
                     progress, alpha, lp, cur_score, best_score, eff) << endl;
      if (opt_draw) { draw(ads, best_sol, format("progress:%.3f  eff:%.6f", progress, 100*eff)); }
      checkpoint += 0.1;
    }

    vector<int> neighbors;
    for (int i=0; i<N; i++) {
      
      double target_area = alpha * ads[i].r;
      
      auto bbs = possible_bbs(sol, ads[i], i);

      assert(!bbs.empty());
      for (int j=bbs.size()-1; j>=1; j--) {
        if (bbs[j].area() < target_area) {
          bbs.pop_back();
        } else {
          break;
        }
      }
      
      auto bb = rng.sample(bbs);
      int W = bb.w();
      int H = bb.h();

      if (!hill_climb && bb.area() == sol[i].area()) {
        qcc ++;
        sol2 = sol;
      
        auto b2(bb);
        neighbors = find_neighbors(bb, sol);
        rng.shuffle(neighbors);

        double sc_diff = 0;
        for (int j : neighbors) {
          sc_diff -= ads[j].calc_score(sol[j]);
          sol[j] = smallest_rectangle(ads[j]);
        }

        sol[i] = find_some_rectangle(sol, ads[i], i, alpha, rng);

        for (int j : neighbors) {
          if (j != i) {
            sol[j] = find_some_rectangle(sol, ads[j], j, alpha, rng);
          }
          sc_diff += ads[j].calc_score(sol[j]);            
        }

        auto sc = cur_score + sc_diff * inv_N;
        if ((hill_climb && sc > cur_score) ||
            //(!hill_climb && sa.acceptable(sc_diff))) {
            (!hill_climb && sa.acceptable(sc - cur_score))) {
          cur_score = sc;
        } else {
          sol = sol2;
        }
      } else {
        double sc_diff = 0;
        sc_diff -= ads[i].calc_score(sol[i]);
        sol[i] = find_some_rectangle(sol, ads[i], i, alpha, rng, bb);
        sc_diff += ads[i].calc_score(sol[i]);
        cur_score += sc_diff * inv_N;
      }

      //auto sc2 = unsafe_calc_score(ads, sol);
      //assert(abs(cur_score - sc2) < 1e-9);
      if (cur_score > best_score) {
        best_score = cur_score;
        best_sol = sol;
      }
    }

    if (alpha == 1.0) {
      sol = improve_by_shifting_borders(ads, sol);
      auto sc = unsafe_calc_score(ads, sol);
      cur_score = sc;
      if (sc > best_score) {
        best_score = sc;
        best_sol = sol;
      }
    }
  }
  sol = best_sol;
  DBG(lp);
  DBG(timer_now());
  DBG(tm_bo);

  DBG(qcc);
  DBG(lp*N);

  {
    auto sc_before_shift = unsafe_calc_score(ads, sol);
    DBG(sc_before_shift);
    
    if (opt_draw) {
      draw(ads, sol, "before border shifting");
    }

    // micchiri
    for (int i=0; i<N; i++) {
      auto bbs = possible_bbs(sol, ads[i], i);
      assert(!bbs.empty());
      sol[i] = bbs[0];
    }

    auto scQ_before_shift = unsafe_calc_scoreQ(ads, sol);
    DBG(scQ_before_shift);
    
    double tm_shift = -timer_now();
    int lp_shift;
    for (lp_shift=0; lp_shift<100; lp_shift++) {
      //unsafe_calc_scoreQ(ads, sol);
      double tm = timer_now();
      if (opt_lp < 0 && tm > 0.95*TIME_LIMIT) {
        break;
      }
      sol = improve_by_shifting_borders(ads, sol);
    }
    tm_shift += timer_now();
    DBG(lp_shift);
    DBG(tm_shift);

    if (opt_draw) {
      draw(ads, sol, "after border shifting");
    }

    auto scQ_after_shift = unsafe_calc_scoreQ(ads, sol);
    DBG(scQ_after_shift);
  }

  {
    for (int i=0; i<N; i++) {
      auto bbs = possible_bbs(sol, ads[i], i);
      assert(!bbs.empty());
      sol[i] = bbs[0];
    }

    if (opt_draw) {
      draw(ads, sol, "before G shifting");
    }
    
    double tm_Gshift = -timer_now();
    int lp_Gshift;
    for (lp_Gshift=0; lp_Gshift<100; lp_Gshift++) {
      double tm = timer_now();
      if (opt_lp < 0 && tm > 0.95*TIME_LIMIT) {
        break;
      }
      sol = group_shift(ads, sol);
    }
    tm_Gshift += timer_now();
    DBG(lp_Gshift);
    DBG(tm_Gshift);

    auto scQ_after_Gshift = unsafe_calc_scoreQ(ads, sol);
    DBG(scQ_after_Gshift);
    if (opt_draw) {
      draw(ads, sol, "after G shifting");
    }
  }

  {
    for (int i=0; i<N; i++) {
      sol[i] = find_some_rectangleA(sol, ads[i], i);
    }
  }
  //DBG(cnt_unsafe);
  
  double sc = unsafe_calc_score(ads, sol);
  DBG(sc);

  return sol;
}

int main(int argc,const char *argv[]) {
  timer_start();
  
  for (int i=1; argv[i]; i++) {
    if (strcmp(argv[i], "--verbose") == 0) {
      opt_verbose = true;
    } else if (strcmp(argv[i], "--draw") == 0) {
      opt_draw = true;
    } else if (strcmp(argv[i], "--lp") == 0) {
      i++;
      assert(argv[i]);
      opt_lp = atoi(argv[i]);
    }
  }
  
  int N;
  cin >> N;
  vector<ad_t> ads(N);
  for (int i=0; i<N; i++)
    cin >> ads[i].x >> ads[i].y >> ads[i].r;

  auto ret = solve(ads);
  for (const auto o : ret) {
    cout << o.l << " " << o.t << " " << o.r << " " << o.b << endl;
  }

  timer_stat();

  if (opt_verbose) {
    for (int i=0; i<N; i++) {
      cerr << format("%3d %.6f", i, ads[i].calc_score(ret[i])) << " " << ads[i] << " " << ret[i] << endl;
    }
  }
  if (opt_draw) {
    draw(ads, ret);
  }
}
