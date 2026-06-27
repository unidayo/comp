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
#include <numeric>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "sys/time.h"
using namespace std;
#define HERE (cerr << "LINE: " << __LINE__ << " " << __FUNCTION__ << endl)

#define DBG(x) cerr << #x << ": " << (x) << endl
template<typename... Args>
string format(const string& fmt, Args... args) {
  int sz = snprintf(nullptr, 0, fmt.c_str(), args ...);
  char buf[sz+1];
  snprintf(buf, sz+1, fmt.c_str(), args ...);
  return buf;
}


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

  string seed_string() const {
    return format("%08x%08x%08x%08x", x, y, z, w);
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

const double TIME_LIMIT = 5.0;

/* insert here */
bool arg_detail = false;
bool opt_detail = false;

double tm_mc_dij = 0;
double tm_mc = 0;
double tm_ax = 0;
double tm_somewhere = 0;
double tm_score = 0;

int cnt_score = 0;

const int inf = 1234567;
const int CABLE = 7;
const int OUTER = 99;
const int G = 64;
int CP(int x,int y) { return (y+1)*G+(x+1); }
int CX(int p) { return p%G - 1; }
int CY(int p) { return p/G - 1; }
string CS(int p) { return format("(%2d,%2d)", CX(p), CY(p)); }
string CSS(const vector<int>& v) {
  string s;
  for (int p : v) { s += CS(p); };
  return s;
}
const vector<int> DPS = { 1, -G, -1, G };
int mhd(int p,int q) {
  int dx = CX(p) - CX(q);
  int dy = CY(p) - CY(q);
  return abs(dx) + abs(dy);
}
int from_border(int p,int N) {
  int x = CX(p);
  int y = CY(p);
  return min({x, N-1-x, y, N-1-y});
};

int sign(int x) { return (x > 0) ? 1 : (x < 0) ? -1 : 0; }
template<typename T> T pow2(const T& x) { return x*x; }
template<typename T>
bool vcount(const vector<T>& v, const T& x) {
  return find(v.begin(), v.end(), x) != v.end();
}

struct grid_t {
  int N, K;
  int empty_cells;
  
  vector<int> g;
  struct cinfo_t {
    int s, t, color, dir;
  };
  vector<cinfo_t> cinfo;
  vector<pair<int,int>> moves;
  vector<cinfo_t> connections;
  set<int> fixed;
  vector<int> roots;
  uf_t uf;

  grid_t() : N(0), K(0), empty_cells(0) {}

  void read(istream& is) {
    is >> N >> K;
    g.resize((N+2)*G, OUTER);
    cinfo.resize((N+2)*G, {-1, -1, -1});
    DBG(N);
    DBG(K);
    for (int y=0; y<N; y++) {
      string s;
      is >> s;
      for (int x=0; x<N; x++) {
        int c = s[x] - '0';
        g.at(CP(x, y)) = c;
        empty_cells += (c == 0);
      }
    }
    DBG(empty_cells);
  }

  bool is_connectable(int s,int t,int dir=0) {
    if (dir == 0) {
      int dx = CX(t) - CX(s);
      int dy = CY(t) - CY(s);
      assert(dx == 0 || dy == 0);
      if (dx != 0) {
        dir = sign(dx)*1;
      } else {
        dir = sign(dy)*G;
      }
    }

    //cerr << "is_connectable: " << CS(s) << " " << CS(t) << " " << d << endl;
    for (int p=s+dir; p!=t; p+=dir) {
      if (g[p] == CABLE) {
        //cerr << "  fail: " << CS(p) << endl;
        return false;
      }
    }

    return true;
  }

  void make_reconnect(int s,int mid,int t) {
    for (int i=0; i<connections.size(); i++) {
      if (connections[i].s == s && connections[i].t == t) {
        auto ci(connections[i]);
        int dir = connections[i].dir;
        assert(dir != 0);

        connections[i].t = mid;
        connections.push_back({mid, t, g[s], dir});
        ci.t = mid;
        fixed.insert(mid);
        uf.merge(s, mid);

        for (int p=s+dir; p!=mid; p+=dir) {
          assert(p > 0 && p < G*G);
          cinfo[p] = ci;
        }
        ci.s = mid;
        ci.t = t;
        for (int p=mid+dir; p!=t; p+=dir) {
          assert(p > 0 && p < G*G);          
          cinfo[p] = ci;
        }

        empty_cells ++;

        return;
      }
    }
    assert(false);
  }

  void make_connection(int s,int t,int dir=0) {
    if (dir == 0) {
      int dx = CX(t) - CX(s);
      int dy = CY(t) - CY(s);
      assert(dx == 0 || dy == 0);
      if (dx != 0) {
        dir = sign(dx)*1;
      } else {
        dir = sign(dy)*G;
      }
    }
    //cerr << "make_connection: " << CS(s) << " " << CS(t) << " " << d << endl;
    
    connections.push_back({s, t, g[s], dir});
    //assert(!fixed.count(s) || !fixed.count(t));
    fixed.insert(s);
    fixed.insert(t);
    uf.merge(s, t);
    for (int p=s+dir; p!=t; p+=dir) {
      assert(g[p] == 0);
      g[p] = CABLE;
      cinfo[p].s = s;
      cinfo[p].t = t;
      cinfo[p].color = g[s];
      cinfo[p].dir = dir;
      empty_cells --;
    }
  }

  void make_move(int p,int q) {
    assert(mhd(p, q) == 1);
    if (g[p] == 0 || g[p] == CABLE) {
      HERE;
      DBG(g[p]);
      DBG(CS(p));
      DBG(CS(q));
      output_and_exit();
    }
    
    assert(g[p] >= 1 && g[p] <= K);
    assert(g[q] == 0);

    moves.push_back({p, q});
    swap(g[p], g[q]);
  }

  int count_operations() const {
    return moves.size() + connections.size();
  }

  int calc_unsafe_score() const {
    tm_score -= timer_now();
    cnt_score ++;
    auto uff(uf);
    
    map<int,vector<int>> freqs;
    set<int> roots;
    for (int y=0; y<N; y++)
      for (int x=0; x<N; x++) {
        int p = CP(x, y);
        if (g[p] >= 1 && g[p] <= K) {
          if (!freqs.count(uff.root(p))) {
            freqs[uff.root(p)] = vector<int>(K+1);
          }
          roots.insert(uff.root(p));
          freqs[uff.root(p)][g[p]] ++;
        }
      }

    int score = 0;
    for (int root : roots) {
      const auto& freq(freqs[root]);
      int local_sc = 0;
      for (int i=1; i<=K; i++) {
        local_sc += freq[i] * (freq[i]-1) / 2;
        for (int j=i+1; j<=K; j++) {
          local_sc -= freq[i] * freq[j];
        }
      }
      score += local_sc;
    }

    tm_score += timer_now();    
    return score;
  }

  int eval_connectivity(int p) const {
    int sum = 0;
    for (int dp : DPS) {
      for (int i=1; i<N; i++) {
        int q = p + i*dp;
        if (g[q] == 0) {

        } else {
          if (g[q] == g[p] && !fixed.count(q)) {
            sum ++;
          }
          break;
        }
      }
    }

    return sum;
  }

  int obs_cost(int p,int col) const {
    const int cable_cost = 1;    
    return ((fixed.count(p)) ? 2 :
            (g[p] == CABLE) ? (cinfo[p].color == col ? 0 : cable_cost) :
            (g[p] == 0) ? 0 : 1);
  }

  int count_obs_naive(int p,int t) const {
    int px = CX(p);
    int py = CY(p);

    int tx = CX(t);
    int ty = CY(t);

    int dx = sign(CX(t) - CX(p));
    int dy = sign(CY(t) - CY(p));

    if (min(abs(tx-px), abs(ty-py)) > 10) { return inf; }

    int tcol = g[t];
    // xy
    int cost_xy = 0;
    for (int x=px+dx; x!=tx; x+=dx) {
      cost_xy += obs_cost(CP(x, py), tcol);
    }
    if (px != tx) { cost_xy += obs_cost(CP(tx, py), tcol); }
    for (int y=py+dy; y!=ty; y+=dy) {
      cost_xy += obs_cost(CP(tx, y), tcol);
    }

    // yx
    int cost_yx = 0;
    for (int y=py+dy; y!=ty; y+=dy) {
      cost_yx += obs_cost(CP(px, y), tcol);
    }
    if (py != ty) { cost_yx += obs_cost(CP(px, ty), tcol); }
    for (int x=px+dx; x!=tx; x+=dx) {
      cost_yx += obs_cost(CP(x, ty), tcol);
    }

    return min(cost_xy, cost_yx);
  }

  bool move_to_somewhere(int s,int taboo,const vector<int>& nondests) {
    //cerr << "move_to_somewhere: " << CS(s) << endl;
    static vector<int> dmp(g.size());
    fill(dmp.begin(), dmp.end(), inf);
    assert(g[s] >= 1 && g[s] <= K);
    int t = -1;

    vector<int> que, que2;
    que = {s};
    const int max_distance = 100*K - count_operations();
    for (int d=0; d<max_distance; d++) {
      que2.clear();
      for (int p : que) {
        if (dmp[p] <= d) { continue; }
        dmp[p] = d;
        
        for (int dp : DPS) {
          int q = p + dp;
          if (dmp[q] <= d) { continue; }
          if (g[q] == OUTER) { continue; }
          if (q == taboo) { continue; }
          if (fixed.count(q)) { continue; }
          if (g[q] == 0 && !vcount(nondests, q)) { t = q; dmp[q] = d+1; goto found; }
          que2.push_back(q);
        }
      }
      que.swap(que2);
    }
  found:

    if (t < 0) { return false; }
    assert(!vcount(nondests, t));
    
    int p = t;
    vector<pair<int,int>> st;
    while (p != s) {
      bool found = false;
      for (int dp : DPS) {
        int q = p + dp;
        if (dmp[q] < dmp[p]) {
          found = true;
          st.push_back({q, p});
          p = q;
          break;
        }
      }
      if (g[st.back().first] != 0 && g[st.back().first] != CABLE) {
        vector<int> tmp_cable;
        for (auto [p, q] : st) {
          if (g[p] == CABLE) { tmp_cable.push_back(p); g[p] = 0; }
          if (g[q] == CABLE) { tmp_cable.push_back(q); g[q] = 0; }
        }
        while (!st.empty()) {
          make_move(st.back().first, st.back().second);
          st.pop_back();
        }
        for (auto p : tmp_cable) {
          assert(g[p] == 0);
          g[p] = CABLE;
        }
      }
      assert(found);
    }

    return true;
  }

  int move_and_connect(int s,int t,int max_operations) {
    bool verbose = false;

    double progress = max(0.0, (count_operations()-100.0*(K-1))/100.0);
    int max_cable_length = clamp(empty_cells-N*(1.0-progress), 1.0, (double)N);

    map<int,pair<double,int>> mids;
    // {mid} connectable positions.
    // s -(move)-> mid -(connect)-> t
    // value: {cost, dir}
    // cost: (obstacles between q and t) and connectivity
    for (int dp : DPS) {
      double ob = 0;
      if (g[t+dp] == CABLE && (cinfo[t+dp].s == t || cinfo[t+dp].t == t)) {
        for (int i=1; i<max_cable_length; i++) {
          int q = t + i*dp;
          if (fixed.count(q)) { break; }
          mids[q] = {0.0, dp};
        }
      } else {
        for (int i=1; i<max_cable_length; i++) {
          int q = t + i*dp;
          if (fixed.count(q) || g[q] == CABLE || g[q] == OUTER) { break; }
          if (q != s && g[q] > 0) { ob ++; }
          //mids[q] = {ob, dp};
          
          mids[q] = {ob, dp}; // ??
        }
      }
    }
    if (mids.empty()) {
      // no connectable positions.
      return -1;
    }

    // dijkstra
    static vector<int> dmp(g.size());
    static vector<int> fr(g.size());
    fill(dmp.begin(), dmp.end(), inf);
    less_priority_queue<tuple<int,int,int>> que;
    que.push({0, s, s});

    tm_mc_dij -= timer_now();
    double best_cost = inf;
    int mid = -1;
    while (!que.empty()) {
      auto [cost, p, from] = que.top();
      cost /= G;
      if (cost > 8) { break; }
      que.pop();
      if (dmp[p] <= cost) { continue; }
      dmp[p] = cost;
      fr[p] = from;
      if (mids.count(p)) {
        double total_cost = cost + mids.at(p).first;
        if (total_cost < best_cost) {
          best_cost = total_cost;
          mid = p;
        }
      }

      if (cost >= best_cost) { break; }
      for (int dp : DPS) {
        int q = p + dp;
        if (g[q] == OUTER) { continue; }
        if (fixed.count(q)) { continue; }
        int cost2 = cost;
        if (g[q] != 0) {
          // some other node. should be moved away. (more cost?)
          cost2 += 1;
        }
        if (mids.count(p) && mids.count(q)) {
          // connect. need not to move
        } else {
          cost2 += 1;
        }
        if (dmp[q] > cost2) {
          cost2 = cost2*G + mhd(q, CP(N/2, N/2));
          que.push({cost2, q, p});
        }
      }
    }
    tm_mc_dij += timer_now();    
    if (mid < 0) { return -1; }
    //assert(g[mid] != CABLE);
    bool reconnecting = (g[mid] == CABLE);

    vector<int> path;
    int p = t;
    vector<int> tmp_removed_cables;
    for (int lp=0; p != mid; lp++) {
      assert(lp < 1000);
      path.push_back(p);
      p += mids.at(mid).second; // dir
    }
    for (int lp=0; p != s; lp++) {
      assert(lp < 1000);
      path.push_back(p);
      if (g[p] == CABLE) {
        tmp_removed_cables.push_back(p);
        g[p] = 0;
      }
      p = fr[p];
    }
    path.push_back(s);

    reverse(path.begin(), path.end());
    vector<int> dummies;
    bool connecting = false;
    bool true_connecting = false;
    for (int i=1; i<path.size(); i++) {
      if (count_operations() > max_operations) { return -1; }
      
      int p = path[i-1];
      int q = path[i];
      if (verbose) { 
        cerr << CS(p) << "  " << CS(q) << endl;
      }
      if (p == mid) {
        if (reconnecting) {
          for (int p : tmp_removed_cables) {
            if (p == mid) {
            } else {
              assert(g[p] == 0);
              g[p] = CABLE;
            }
          }
          make_reconnect(cinfo[mid].s, mid, cinfo[mid].t);
          break;
        }
        connecting = true;
      }
      if (q == t) {
        for (int c : dummies) { g[c] = 0; }
        for (int p : tmp_removed_cables) {
          assert(g[p] == 0);
          g[p] = CABLE;
        }
        make_connection(mid, t);
      } else {
        if (g[q] == CABLE) {
          if (connecting) {
            HERE;
            DBG(g[t]);
            DBG(CS(s));
            DBG(CS(t));
            DBG(CSS(path));
            DBG(CS(p));
            DBG(CS(q));
            output_and_exit();
          }
          assert(!connecting);
        } else if (g[q] > 0) {
          if (fixed.count(q)) { return -1; }
          tm_somewhere -= timer_now();
          auto ok = move_to_somewhere(q, p, path);
          tm_somewhere += timer_now();
          if (!ok) { return -1; }
        }

        if (true_connecting) {
          dummies.push_back(q);
          g[q] = CABLE;
          
        } else if (connecting) {

          int cur = eval_connectivity(p) - (double)from_border(p, N)/N;
          swap(g[p], g[q]);
          int nxt = eval_connectivity(q) - (double)from_border(q, N)/N;
          swap(g[p], g[q]);
          if (g[q] == 0 && nxt > cur) {
            make_move(p, q);
            mid = q;
          } else {
            dummies.push_back(q);
            g[q] = CABLE;
            true_connecting = true;
          }
        } else {
          make_move(p, q);
        }
      }

      p = fr[p];
    }

    return mid;
  }

  void output_and_exit() const {
    HERE;
    output(cout);
    exit(0);
  }

  void output(ostream& os) const {
    auto m(moves);
    auto c(connections);

    if (m.size() > 100*K) {
      m.resize(100*K);
      c.clear();
    } else if (m.size() + c.size() > 100*K) {
      c.resize(100*K - m.size());
    }

    os << m.size() << endl;
    for (auto [p, q] : m) {
      os << CY(p) << " " << CX(p) << " " << CY(q) << " " << CX(q) << endl;
    }
    os << c.size() << endl;
    //for (auto [p, q] : c) {
    for (auto ci : c) {
      os << CY(ci.s) << " " << CX(ci.s) << " " << CY(ci.t) << " " << CX(ci.t) << endl;
    }
  }
};

vector<grid_t> history;

void set_adjs(const grid_t& sol,vector<array<int,4>>& adjs) {
  for (int y=0; y<sol.N; y++) {
    int q = -1;
    for (int x=0; x<sol.N; x++) {
      int p = CP(x, y);
      if (sol.g[p] == 0) { continue; }
      //nodes[sol.g[p]].push_back(p);
      adjs[p] = {-1, -1, -1, -1};
      if (q >= 0) {
        adjs[q][0] = p;
        adjs[p][2] = q;
      }
      q = p;        
    }
  }
      
  for (int x=0; x<sol.N; x++) {
    int q = -1;
    for (int y=0; y<sol.N; y++) {
      int p = CP(x, y);
      if (sol.g[p] == 0) { continue; }
      if (q >= 0) {
        adjs[q][3] = p;
        adjs[p][1] = q;
      }
      q = p;
    }
  }
}

struct solver_t {
  grid_t best_sol;
  const grid_t ini;
  int N, K;
  vector<int> obs_x, obs_y;

  solver_t(const grid_t& ini) : ini(ini) {
    N = ini.N;
    K = ini.K;
  }

  void build_obstacles_acc(const grid_t& grid,int col) {
    if (obs_x.empty()) {
      obs_x.resize(grid.g.size());
      obs_y.resize(grid.g.size());
    }

    const int cable_cost = 1;
    for (int y=0; y<N; y++) {
      int acc = 0;
      obs_x[CP(0, y)] = acc;
      for (int x=0; x<N; x++) {
        int p = CP(x, y);
        int v = grid.g[p];
        acc += (grid.fixed.count(p)) ? 2 : (v == CABLE) ? (grid.cinfo[p].color == col ? 0 : cable_cost) : (v == 0) ? 0 : 1;
        obs_x[CP(x+1, y)] = acc;
      }
    }

    for (int x=0; x<N; x++) {
      int acc = 0;
      obs_y[CP(x, 0)] = acc;
      for (int y=0; y<N; y++) {
        int p = CP(x, y);
        int v = grid.g[p];
        acc += (grid.fixed.count(p)) ? 2 : (v == CABLE) ? (grid.cinfo[p].color == col ? 0 : cable_cost) : (v == 0) ? 0 : 1;
        obs_y[CP(x, y+1)] = acc;
      }
    }
  }

  int count_obs0(int x1,int y1,int x2,int y2) {
    if (y1 == y2) {
      if (x1 > x2) { swap(x1, x2); }
      int y = y1;

      return obs_x[CP(x2+1, y)] - obs_x[CP(x1, y)];
    } else if (x1 == x2) {
      if (y1 > y2) { swap(y1, y2); }
      int x = x1;

      return obs_y[CP(x, y2+1)] - obs_y[CP(x, y1)];
    } else {
      assert(false);
    }
  }

  int count_obs(int x1,int y1,int x2,int y2) {
    if (y1 == y2) {
      int y = y1;
      if (x1 < x2) {
        return obs_x[CP(x2+1, y)] - obs_x[CP(x1+1, y)];
      } else {
        return obs_x[CP(x1, y)] - obs_x[CP(x2, y)];
      }
    } else if (x1 == x2) {
      int x = x1;
      if (y1 < y2) {
        return obs_y[CP(x, y2+1)] - obs_y[CP(x, y1+1)];
      } else {
        return obs_y[CP(x, y1)] - obs_y[CP(x, y2)];
      }
    } else {
      assert(false);
    }
  }

  grid_t solve_axis_align(grid_t sol,int root,bool flag_outer) {
    const int target_color = sol.g[root];
    assert(target_color >= 1 && target_color <= K);
    uf_t uf;
    sol.roots.push_back(root);
    vector<int> que;
    for (int y=0; y<N; y++)
      for (int x=0; x<N; x++) {
        int p = CP(x, y);
        if (p != root && sol.g[p] == sol.g[root] && !sol.fixed.count(p)) {
          que.push_back(p);
        }
      }

    vector<int> nodes = {root};
    sol.fixed.insert(root);
    bool verbose = false;
    //bool verbose = true;
    verbose |= opt_detail;
    
    if (verbose) { DBG(CS(root)); }

    const int center = CP(N/2, N/2);
    const int max_cost = 10;
    for (int lp=0; /**/; lp++) {
      if (verbose) { cerr << "===[lp:" << lp << ":" << sol.connections.size() << "]==========" << endl; }
      tm_ax -= timer_now();
      build_obstacles_acc(sol, sol.g[root]);
      que.clear();
      for (int y=0; y<N; y++) {
        for (int x=0; x<N; x++) {
          int p = CP(x, y);
          if (sol.g[p] == target_color && !sol.fixed.count(p)) {
            que.push_back(p);
          }
        }
      }
      tm_ax += timer_now();

      struct item_t {
        double cost;
        int mhd;
        int idx;
        int leaf;
        int q;
        int actual;

        bool operator<(const item_t& item) const {
          return make_tuple(cost, mhd, idx, leaf, !actual) >
                      make_tuple(item.cost, item.mhd, item.idx, item.leaf, !item.actual);
        }
      };

      map<pair<int,int>,pair<grid_t,item_t>> memo;
      priority_queue<item_t> qc;
      double min_qcost = inf;
      for (int i=0; i<que.size(); i++) {
        int p = que[i];
        int px = CX(p);
        int py = CY(p);
        for (int j=0; j<nodes.size(); j++) {
          int t = nodes[j];
          if (sol.uf.root(p) == sol.uf.root(t)) { continue; }
          int tx = CX(t);
          int ty = CY(t);

          double local_best_cost = inf;
          {
            int cost = abs(tx - px) + count_obs(px, py, tx, py) + count_obs(tx, py, tx, ty);
            
            if (cost < local_best_cost) {
              local_best_cost = cost;
            }
          }
          {
            int cost = abs(ty - py) + count_obs(px, py, px, ty) + count_obs(px, ty, tx, ty);
            if (cost < local_best_cost) {
              local_best_cost = cost;
            }
          }
          item_t item({local_best_cost, mhd(p, t), i, t, 0});
          if (local_best_cost < min_qcost+10 && local_best_cost + sol.count_operations() <= 100*K) {
            qc.push(item);
          }
          min_qcost = min(min_qcost, local_best_cost);
        }
      }

      bool found = false;
      item_t item;
      int actual_count = 0;
      int max_operations = 100*K + 1;
      //double known_best = max_operations;
      while (!qc.empty()) {
        item = qc.top();
        qc.pop();
        if (verbose) {
          cerr << "item: " << item.actual << " co:" << item.cost << "," << item.mhd << " " << CS(que[item.idx]) << "->" << CS(item.leaf) << endl;
        }
        if (!item.actual) {
          auto sol2(sol);
          tm_mc -= timer_now();
          auto q = sol2.move_and_connect(que[item.idx], item.leaf, max_operations);
          tm_mc += timer_now();
          
          actual_count ++;          
          if (actual_count >= 30) { break; }
          if (q < 0) {
            // can't find connection
            if (verbose) { cerr << " -> XXX" << endl; }
            continue;
          }
          if (sol2.count_operations() > 100*K) { continue; }
          max_operations = min(sol2.count_operations()+3, max_operations);
          
          double actual_cost = sol2.count_operations() - sol.count_operations();
          //known_best = min(known_best, actual_cost);
          //actual_cost += 1.0 - sol2.eval_connectivity(item.q) / (2.0*N*N);
          actual_cost += 1.0 - (double)sol2.empty_cells / (N*N);
          if (flag_outer) {
            actual_cost += (pow2(from_border(item.leaf, N)) + pow2(from_border(item.q, N)))/(3.0*N);
          }
          
          if (verbose) {
            cerr << " ->  " << actual_cost << endl;
          }
          
          item.actual = true;
          item.cost = actual_cost;
          item.q = q;
          memo[make_pair(item.idx,item.leaf)] = {sol2, item};
          qc.push(item);
        } else {
          found = true;
          sol = memo[make_pair(item.idx,item.leaf)].first;
          break;
        }
      }
      if (!found && !memo.empty()) {
        for (const auto& pa : memo) {
          const auto& [sol2, item2] = pa.second;
          if (!item.actual || item2.cost < item.cost) {
            item = item2;
            sol = sol2;
            found = true;
          }
        }
      }
      if (!found) { break; }

      assert(found);
      //cerr << item.cost << " " << actual_count << " " << sol.count_operations() << endl;

      if (verbose) {
        cerr << "con: " << CS(item.q) << " " << sol.eval_connectivity(item.q) << endl;
        if (!opt_detail) { sol.output(cout); }
      }
      if (opt_detail) { history.push_back(sol); }
      nodes.push_back(item.q);
    }
    //best_sol = sol;
    return sol;
  }

  grid_t greedy_connection(grid_t sol) {
    for (int c=1; c<=K; c++) {
      for (int y=0; y<N; y++) {
        for (int x=0; x<N; x++) {
          int p = CP(x, y);
          if (sol.g[p] == 0 || sol.g[p] == CABLE) { continue; }
          for (int dp : {1, G}) {
            int q = p;
            while (true) {
              q += dp;
              if (sol.g[q] == sol.g[p]) {
                if (sol.uf.root(q) == sol.uf.root(p)) { break; }
                sol.make_connection(p, q);
                if (opt_detail) { history.push_back(sol); }

                if (sol.count_operations() >= 100*K) { goto prune; }
                break;
              } else if (sol.g[q] != 0) {
                break;
              }
            }
          }
        }
      }
    }
  prune:

    return sol;
  }

  grid_t greedy_connection2(grid_t sol) {
    if (sol.count_operations() >= 100*K) { return sol; }
    
    uf_t uf;
    vector<vector<int>> adjs(sol.g.size());
    
    for (int y=0; y<N; y++) {
      for (int x=0; x<N; x++) {
        int p = CP(x, y);
        if (sol.g[p] == 0 || sol.g[p] == CABLE) { continue; }
        for (int dp : {1, G}) {
          int q = p;
          while (true) {
            q += dp;
            if (sol.g[q] == sol.g[p]) {
              if (sol.uf.root(q) == sol.uf.root(p)) { break; }
              uf.merge(p, q);
              //cerr << "edge: " << CS(p) << " " << CS(q) << endl;
              adjs[p].push_back(q);
              adjs[q].push_back(p);
              break;
            } else if (sol.g[q] != 0) {
              break;
            }
          }
        }
      }
    }

    vector<pair<int,int>> vals;
    for (int y=0; y<N; y++) {
      for (int x=0; x<N; x++) {
        int p = CP(x, y);
        if (uf.root(p) == p) {
          if (uf.size(p) > 1) {
            vals.push_back({-(int)uf.size(p), p});
          }
        }
      }
    }
    sort(vals.begin(), vals.end());

    for (const auto [n,root] : vals) {
      vector<int> que = {root};
      while (!que.empty()) {
        int p = que.back();
        que.pop_back();
        for (int q : adjs[p]) {
          if (sol.uf.root(q) == sol.uf.root(root)) { continue; }
          if (!sol.is_connectable(p, q)) { continue; }
          //cerr << CS(p) << " " << CS(q) << endl;
          sol.make_connection(p, q);
          if (sol.count_operations() >= 100*K) { goto prune; }
          que.push_back(q);
        }
      }
    }
  prune:

    return sol;
  }

  vector<int> find_good_roots(grid_t sol) {
    uf_t uf;
    vector<vector<int>> freq(sol.g.size(), vector<int>(K+1, 0));
    for (int y=0; y<N; y++) {
      for (int x=0; x<N; x++) {
        int p = CP(x, y);
        if (!sol.fixed.count(p) && sol.g[p] <= K) {
          freq[p][sol.g[p]] = 1;
        }
      }
    }

    for (int y=0; y<N; y++) {
      for (int x=0; x<N; x++) {
        int p = CP(x, y);
        if (!sol.fixed.count(p) && sol.g[p] != CABLE) {
          for (int dp : {1, G}) {
            int q = p + dp;
            if (!sol.fixed.count(q) && sol.g[q] != CABLE) {              
              if (uf.root(p) != uf.root(q)) {
                int rp = uf.root(p);
                int rq = uf.root(q);
                uf.merge(p, q);
                for (int i=0; i<=K; i++) {
                  freq[uf.root(p)][i] = freq[rp][i] + freq[rq][i];
                }
              }
            }
          }
        }
      }
    }

    //int root = CP(0, 0);
    //int ties = 0;
    vector<int> roots;
    int best_cnt = 0;
    for (int y=0; y<N; y++) {
      for (int x=0; x<N; x++) {
        int p = CP(x, y);
        if (!sol.fixed.count(p) && sol.g[p] >= 1 && sol.g[p] <= K) {
          const auto& v(freq[uf.root(p)]);
          int col = sol.g[p];
          
          int cnt = v[col] * 10000 + v[0];
          if (cnt > best_cnt) {
            best_cnt = cnt;
            roots = {p};
          } else if (cnt == best_cnt) {
            roots.push_back({p});
          }
        }
      }
    }
    //cerr << " " << CS(root) << " " << freq[uf.root(root)] << endl;

    return roots;
  }

  void solve1() {

    xor128_t rng;
    int best_score = 0;

    vector<int> roots0 = find_good_roots(ini);
    grid_t best_sol1;
    vector<int> best_roots1;

    const double time_limit_first = 1.8;
    const double time_limit_second = 2.5;

    double tm0 = timer_now();
    for (int lp=0; /**/; lp++) {
      if (lp >= 2) {
        double tm = timer_now();
        double est = (tm - tm0)*(lp+1)/lp + tm0;
        if (est > time_limit_first && !best_roots1.empty()) {
          cerr << "timeout: " << lp << endl;
          break;
        }
        if (est > time_limit_second) {
          cerr << "timeout: " << lp << endl;
          break;
        }
      }
      auto sol(ini);
      auto local_best_sol(ini);
      int local_best_score = 0;

      grid_t sol1;
      vector<int> roots1;

      for (int c=0; c<3; c++) {
        auto roots = (c == 0) ? roots0 : find_good_roots(sol);
        if (c == 1) {
          roots1 = roots;
          sol1 = sol;
        }
        int root = rng.sample(roots);
        auto sol2 = solve_axis_align(sol, root, c==0);
        
        if (sol2.count_operations() == sol.count_operations()) {
          break;
        }
        sol = sol2;
        int sc2 = sol.calc_unsafe_score();
        if (sc2 > local_best_score) {
          local_best_sol = sol;
          local_best_score = sc2;
        }
        auto sol3 = greedy_connection2(sol2);
        int sc3 = sol3.calc_unsafe_score();
        if (sc3 > local_best_score) {
          local_best_sol = sol3;
          local_best_score = sc3;
        }
        cerr << " ax: " << sc2 << "/" << sol2.count_operations() << "  gr: " << sc3 << "/" << sol3.count_operations() << endl;
        if (c == 1 && local_best_score < best_score*9/10) {
          break;
        }
      }
      cerr << lp << " " << local_best_score << " " << CSS(local_best_sol.roots) << endl;

      if (local_best_score > best_score) {
        best_score = local_best_score;
        best_sol = local_best_sol;

        if (!roots1.empty()) {
          best_roots1 = roots1;
          best_sol1 = sol1;
        }
      }
    }
    DBG(best_sol.calc_unsafe_score());

    if (!best_roots1.empty()) {
      rng.shuffle(best_roots1);
    }
    
    double tm1 = timer_now();
    for (int lp=0; /**/; lp++) {
      if (best_roots1.empty()) { break; }
      if (lp >= 2) {
        double tm = timer_now();
        double est = (tm - tm1)*(lp+1)/lp + tm1;
        if (est > time_limit_second) {
          cerr << "timeout: " << lp << endl;
          break;
        }
      }
      auto sol(best_sol1);
      auto local_best_sol(best_sol1);
      int local_best_score = 0;

      for (int c=1; c<3; c++) {
        int root;
        if (c == 1) {
          root = best_roots1[lp%best_roots1.size()];
        } else {
          auto roots = find_good_roots(sol);
          root = rng.sample(roots);
        }
        auto sol2 = solve_axis_align(sol, root, c==0);
        
        if (sol2.count_operations() == sol.count_operations()) {
          break;
        }
        sol = sol2;
        int sc2 = sol.calc_unsafe_score();
        if (sc2 > local_best_score) {
          local_best_sol = sol;
          local_best_score = sc2;
        }
        auto sol3 = greedy_connection2(sol2);
        int sc3 = sol3.calc_unsafe_score();
        if (sc3 > local_best_score) {
          local_best_sol = sol3;
          local_best_score = sc3;
        }
        cerr << " ax: " << sc2 << "/" << sol2.count_operations() << "  gr: " << sc3 << "/" << sol3.count_operations() << endl;
        if (c == 1 && local_best_score < best_score*9/10) {
          break;
        }
      }
      cerr << lp << " " << local_best_score << " " << CSS(local_best_sol.roots) << endl;

      if (local_best_score > best_score) {
        best_score = local_best_score;
        best_sol = local_best_sol;
      }
    }
    DBG(best_sol.calc_unsafe_score());
    
  }
  
  void output_solution() const {
    best_sol.output(cout);
  }
};

int main(int argc,const char *argv[]) {
  timer_start();

  for (int i=0; argv[i]; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      arg_detail = true;
    }
  }
  
  grid_t grid;
  grid.read(cin);

  solver_t slv(grid);
  slv.solve1();

  double tm_total = timer_now();
  DBG(tm_total);

  DBG(tm_ax);
  DBG(tm_mc);
  DBG(tm_mc_dij);
  DBG(tm_somewhere);
  DBG(tm_score);
  DBG(cnt_score);
  
  slv.output_solution();

  if (arg_detail) {
    for (const auto& g : history) {
      g.output(cout);
    }
  }
}
