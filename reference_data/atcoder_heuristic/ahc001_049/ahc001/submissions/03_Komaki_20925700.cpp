#include <bits/stdc++.h>
#include <sys/time.h>
#include <unistd.h>
using namespace std;
#define rep(i, n)   for(int i = 0; (i) < ((int)(n)); ++(i))
#define bit(n)      (((int)1)<<((int)(n)))
#define sz(v)       ((int)((v).size()))

std::string dbgDelim(int &i){ return (i++ == 0 ? "" : ", "); }
#define dbgEmbrace(exp) { int i = 0; os << "{"; { exp; } os << "}"; return os; }
template <class T> std::ostream& operator<<(std::ostream &os, std::vector<T> v);
template <class T> std::ostream& operator<<(std::ostream &os, std::set<T> v);
template <class T> std::ostream& operator<<(std::ostream &os, std::queue<T> q);
template <class T> std::ostream& operator<<(std::ostream &os, std::priority_queue<T> q);
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::pair<T, K> p);
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::map<T, K> mp);
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::unordered_map<T, K> mp);
template <int INDEX, class TUPLE> void dbgDeploy(std::ostream &os, TUPLE _){}
template <int INDEX, class TUPLE, class H, class ...Ts> void dbgDeploy(std::ostream &os, TUPLE t){ os << (INDEX == 0 ? "" : ", ") << get<INDEX>(t); dbgDeploy<INDEX + 1, TUPLE, Ts...>(os, t); }
template <class T, class K> void dbgDeploy(std::ostream &os, std::pair<T, K> p, std::string delim){ os << "(" << p.first << delim << p.second << ")"; }
template <class ...Ts> std::ostream& operator<<(std::ostream &os, std::tuple<Ts...> t){ os << "("; dbgDeploy<0, std::tuple<Ts...>, Ts...>(os, t); os << ")"; return os; }
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::pair<T, K> p){ dbgDeploy(os, p, ", "); return os; }
template <class T> std::ostream& operator<<(std::ostream &os, std::vector<T> v){ dbgEmbrace( for(T t: v){ os << dbgDelim(i) << t; }); }
template <class T> std::ostream& operator<<(std::ostream &os, std::set<T> s){ dbgEmbrace( for(T t: s){ os << dbgDelim(i) << t; }); }
template <class T> std::ostream& operator<<(std::ostream &os, std::queue<T> q){ dbgEmbrace( for(; q.size(); q.pop()){ os << dbgDelim(i) << q.front(); }); }
template <class T> std::ostream& operator<<(std::ostream &os, std::priority_queue<T> q){ dbgEmbrace( for(; q.size(); q.pop()){ os << dbgDelim(i) << (T) q.top();   }); }
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::map<T, K> mp){ dbgEmbrace( for(auto p: mp){ os << dbgDelim(i); dbgDeploy(os, p, "->"); }); }
template <class T, class K> std::ostream& operator<<(std::ostream &os, std::unordered_map<T, K> mp){ dbgEmbrace( for(auto p: mp){ os << dbgDelim(i); dbgDeploy(os, p, "->"); }); }
#define DBG_OUT std::cerr
#define DBG_OVERLOAD(_1, _2, _3, _4, _5, _6, macro_name, ...) macro_name
#define DBG_LINE() { char s[99]; sprintf(s, "line:%3d | ", __LINE__); DBG_OUT << s; }
#define DBG_OUTPUT(v) { DBG_OUT << (#v) << "=" << (v); }
#define DBG1(v, ...) { DBG_OUTPUT(v); }
#define DBG2(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG1(__VA_ARGS__); }
#define DBG3(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG2(__VA_ARGS__); }
#define DBG4(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG3(__VA_ARGS__); }
#define DBG5(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG4(__VA_ARGS__); }
#define DBG6(v, ...) { DBG_OUTPUT(v); DBG_OUT << ", "; DBG5(__VA_ARGS__); }

#define DEBUG0() { DBG_LINE(); DBG_OUT << std::endl; }
#define DEBUG(...)                                                      \
  {                                                                     \
    DBG_LINE();                                                         \
    DBG_OVERLOAD(__VA_ARGS__, DBG6, DBG5, DBG4, DBG3, DBG2, DBG1)(__VA_ARGS__); \
    DBG_OUT << std::endl;                                               \
  }

#ifdef LOCAL
#define my_assert(b) assert(b)
#else
#define my_assert(b) assert(true)
#endif

void shuffle_ints(vector<int> &v)
{
  random_shuffle(v.begin(), v.end());
}

class Timer
{
public:
    void restart();
    double getElapsed();

    Timer();
private:
    static double rdtsc_per_sec_inv;

    double getTimeOfDay();
    unsigned long long int getCycle();

    double start_time;
    unsigned long long int start_clock;
};
double Timer::rdtsc_per_sec_inv = -1;


inline double Timer::getElapsed()
{
  if(rdtsc_per_sec_inv != -1) return (double)(getCycle() - start_clock) * rdtsc_per_sec_inv;

  const double RDTSC_MEASUREMENT_INTERVAL = 0.1;
  double res = getTimeOfDay() - start_time;
  if(res <= RDTSC_MEASUREMENT_INTERVAL) return res;

  rdtsc_per_sec_inv = 1.0 / (getCycle() - start_clock);
  rdtsc_per_sec_inv *= getTimeOfDay() - start_time;
  return getElapsed();
}

inline void Timer::restart()
{
  start_time = getTimeOfDay();
  start_clock = getCycle();
}

Timer::Timer()
{
  restart();
}

inline double Timer::getTimeOfDay()
{
  timeval tv;
  gettimeofday(&tv,0);
  return tv.tv_sec + tv.tv_usec * 0.000001;
}

class XorShift{
public:
    static int rand();
    static double rand_double();
private:
    static unsigned int x;
    static unsigned int y;
    static unsigned int z;
    static unsigned int w;
    static unsigned int t;
};
unsigned int XorShift::x = 123456789;
unsigned int XorShift::y = 362436069;
unsigned int XorShift::z = 521288629;
unsigned int XorShift::w = 88675123;
unsigned int XorShift::t = 1;

int XorShift::rand()
{
  t = x ^ (x << 11);
  x = y;
  y = z;
  z = w;
  w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
  return w & 0x7fffffff;
}

double XorShift::rand_double()
{
  return (double) rand() / 0x7fffffff;
}

class Params
{
public:
    constexpr static const double annealing_coeff = 25;
    constexpr static const int extend_abs_limit = 500;
    constexpr static const double extend_rel_limit = 1.0;

    constexpr static const int extend_id = 1;
    constexpr static const int optimal_id = 3;
    constexpr static const int stab_id = 4;
    constexpr static const int exchange_id = 5;
    constexpr static const int offer_id = 6;
    constexpr static const int destroy_id = 7;


    static vector<int> get_ops()
    {
      vector<int> ops({});
      int _extend_id = extend_id;
      int _optimal_id = optimal_id;
      int _stab_id = stab_id;
      int _exchange_id = exchange_id;
      int _offer_id = offer_id;
      int _destroy_id = destroy_id;
      rep(i, 100) ops.push_back(_extend_id);
      rep(i, 20) ops.push_back(_optimal_id);
      rep(i, 10) ops.push_back(_stab_id);
      rep(i, 10) ops.push_back(_exchange_id);
      rep(i, 20) ops.push_back(_offer_id);
      rep(i, 2) ops.push_back(_destroy_id);
      return ops;
    }
};



class Score
{
public:
    Score(Timer &timer, double time_limit) : timer(timer), time_limit(time_limit) {};

    double time_limit;
    Timer &timer;
    double score = 0.0;

    bool should_update(double diff)
    {
      if(0 <= diff) return true;
      double temp = get_temperature();
      if(temp <= 0) return false;
      double coeff = Params::annealing_coeff;
      return XorShift::rand_double() < exp(coeff * diff / temp);
    }

    double get_temperature()
    {
      return (time_limit - timer.getElapsed()) / time_limit;;
    }

    void add_score(double diff)
    {
      score += diff;
    }
};

inline unsigned long long int Timer::getCycle()
{
  unsigned int low, high;
  __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
  return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
}

class Stats
{
public:
    long long int score = -1;
    long long int extend_failure = 0;
    long long int extend_success = 0;
    long long int optimal_failure = 0;
    long long int optimal_success = 0;
    long long int stab_failure = 0;
    long long int stab_success = 0;
    long long int exchange_failure = 0;
    long long int exchange_success = 0;
    long long int offer_failure = 0;
    long long int offer_success = 0;
    long long int destroy_failure = 0;
    long long int destroy_success = 0;
    long long int composite_failure = 0;
    long long int composite_success = 0;
    double expand_all_improvement = 0;
    vector<double> scores;
} global_stats;

void local_post_process(int score)
{
  ofstream score_ofs(getenv("SCORE_PATH"));
  ofstream stats_ofs(getenv("STATS_PATH"));

  score_ofs << score << endl;
  stats_ofs << "total: " << (global_stats.extend_failure +
                             global_stats.extend_success +
                             global_stats.optimal_failure +
                             global_stats.optimal_success +
                             global_stats.stab_failure +
                             global_stats.stab_success +
                             global_stats.offer_failure +
                             global_stats.offer_success +
                             global_stats.destroy_failure +
                             global_stats.destroy_success +
                             global_stats.composite_failure +
                             global_stats.composite_success +
                             global_stats.exchange_failure +
                             global_stats.exchange_success
  ) << endl;

  double destroy_rate = (double) global_stats.destroy_success / (double) (global_stats.destroy_success + global_stats.destroy_failure);
  stats_ofs << "destroy  : " << (global_stats.destroy_success + global_stats.destroy_failure)
            << " (" << destroy_rate << ")" << endl;

  double extend_rate = (double) global_stats.extend_success / (double) (global_stats.extend_success + global_stats.extend_failure);
  stats_ofs << "extend  : " << (global_stats.extend_success + global_stats.extend_failure)
            << " (" << extend_rate << ")" << endl;

  double optimal_rate = (double) global_stats.optimal_success / (double) (global_stats.optimal_success + global_stats.optimal_failure);
  stats_ofs << "optimal : " << (global_stats.optimal_success + global_stats.optimal_failure)
            << " (" << optimal_rate << ")" << endl;

  double stab_rate = (double) global_stats.stab_success / (double) (global_stats.stab_success + global_stats.stab_failure);
  stats_ofs << "stab    : " << (global_stats.stab_success + global_stats.stab_failure)
            << " (" << stab_rate << ")" << endl;

  double offer_rate = (double) global_stats.offer_success / (double) (global_stats.offer_success + global_stats.offer_failure);
  stats_ofs << "offer   : " << (global_stats.offer_success + global_stats.offer_failure)
            << " (" << offer_rate << ")" << endl;

  double composite_rate = (double) global_stats.composite_success / (double) (global_stats.composite_success + global_stats.composite_failure);
  stats_ofs << "composite: " << (global_stats.composite_success + global_stats.composite_failure)
            << " (" << composite_rate << ")" << endl;

  double exchange_rate = (double) global_stats.exchange_success / (double) (global_stats.exchange_success + global_stats.exchange_failure);
  stats_ofs << "exchange: " << (global_stats.exchange_success + global_stats.exchange_failure)
            << " (" << exchange_rate << ")" << endl;


  stats_ofs << "expand all : " << global_stats.expand_all_improvement << endl;

  stats_ofs << "scores: ";
  for(double s: global_stats.scores) stats_ofs << int(round(s * 1e9)) << ", ";
  stats_ofs << endl;

  score_ofs.close();
  stats_ofs.close();
}

const int MAX = 10000;
const int MAX_N = 512;

class Rec
{
public:
    Rec() : x0(0), x1(0), y0(0), y1(0) {}
    Rec(int x0, int x1, int y0, int y1) : x0(x0), x1(x1), y0(y0), y1(y1) {}
    int x0, x1, y0, y1;
    int area() const
    {
      return (x1 - x0) * (y1 - y0);
    }
};

class Ad
{
public:
    Ad() : spon_x(0), spon_y(0), spon_r(0) {}
    Ad(int spon_x, int spon_y, int spon_r) :
            spon_x(spon_x), spon_y(spon_y), spon_r(spon_r),
            x0(spon_x), x1(spon_x + 1), y0(spon_y), y1(spon_y + 1) {}

    bool validate() const
    {
      if(spon_x < x0 || x1 <= spon_x) return false;
      if(spon_y < y0 || y1 <= spon_y) return false;
      if(x0 < 0 || MAX < x1) return false;
      if(y0 < 0 || MAX < y1) return false;
      return true;
    }

    void set(Rec &rec)
    {
      x0 = rec.x0;
      x1 = rec.x1;
      y0 = rec.y0;
      y1 = rec.y1;
    }

    Rec get_rec() const
    {
      return Rec{x0, x1, y0, y1};
    }

    double compute_score() const
    {
      int area = (x1 - x0) * (y1 - y0);
      double ratio = (spon_r < area) ? ((double) spon_r / area) : ((double) area / spon_r);
      return 1.0 - (1 - ratio) * (1 - ratio);
    }

    int x0 = 0;
    int x1 = 1;
    int y0 = 0;
    int y1 = 1;
    int spon_x;
    int spon_y;
    int spon_r;
};


class AdWrapper
{
public:
    virtual int get_index() = 0;
    virtual int get_spon_x() = 0;
    virtual int get_spon_y() = 0;
    virtual int get_spon_r() = 0;
    virtual int get_x0() = 0;
    virtual int get_x1() = 0;
    virtual int get_y0() = 0;
    virtual int get_y1() = 0;
    virtual void set_x0(int x0) = 0;
    virtual void set_x1(int x1) = 0;
    virtual void set_y0(int y0) = 0;
    virtual void set_y1(int y1) = 0;
    virtual bool validate() = 0;
    virtual double compute_score() = 0;
    virtual Rec get_rec() = 0;
    virtual void set_rec(Rec &rec) = 0;
    virtual ~AdWrapper()= default;;
};


class AdWrapper000 : public AdWrapper
{
public:
    explicit AdWrapper000(Ad &ad, int index) : ad(ad), index(index) {}

    Rec get_rec() override
    {
      return Rec{get_x0(), get_x1(), get_y0(), get_y1()};
    }

    void set_rec(Rec &rec) override
    {
      set_x0(rec.x0);
      set_x1(rec.x1);
      set_y0(rec.y0);
      set_y1(rec.y1);
    }

    int get_index() override
    {
      return index;
    }

    double compute_score() override
    {
      return ad.compute_score();
    }

    int get_x0() override
    {
      return ad.x0;
    }

    int get_x1() override
    {
      return ad.x1;
    }

    int get_y0() override
    {
      return ad.y0;
    }

    int get_y1() override
    {
      return ad.y1;
    }

    int get_spon_x() override
    {
      return ad.spon_x;
    }

    int get_spon_y() override
    {
      return ad.spon_y;
    }

    int get_spon_r() override
    {
      return ad.spon_r;
    }

    void set_x0(int next_x0) override
    {
      ad.x0 = next_x0;
    }

    void set_x1(int next_x1) override
    {
      ad.x1 = next_x1;
    }

    void set_y0(int next_y0) override
    {
      ad.y0 = next_y0;
    }

    void set_y1(int next_y1) override
    {
      ad.y1 = next_y1;
    }

    bool validate() override
    {
      return ad.validate();
    }

private:
    Ad &ad;
    int index;
};

class AdWrapper090 : public AdWrapper
{
public:
    explicit AdWrapper090(Ad &ad, int index) : ad(ad), index(index) {}

    Rec get_rec() override
    {
      return Rec{get_x0(), get_x1(), get_y0(), get_y1()};
    }

    void set_rec(Rec &rec) override
    {
      set_x0(rec.x0);
      set_x1(rec.x1);
      set_y0(rec.y0);
      set_y1(rec.y1);
    }

    int get_index() override
    {
      return index;
    }

    double compute_score() override
    {
      return ad.compute_score();
    }

    int get_x0() override
    {
      return MAX - ad.y1;
    }

    int get_x1() override
    {
      return MAX - ad.y0;
    }

    int get_y0() override
    {
      return ad.x0;
    }

    int get_y1() override
    {
      return ad.x1;
    }

    int get_spon_x() override
    {
      return MAX - ad.spon_y - 1;
    }

    int get_spon_y() override
    {
      return ad.spon_x;
    }

    int get_spon_r() override
    {
      return ad.spon_r;
    }

    void set_x0(int next_x0) override
    {
      ad.y1 = MAX - next_x0;
    }

    void set_x1(int next_x1) override
    {
      ad.y0 = MAX - next_x1;
    }

    void set_y0(int next_y0) override
    {
      ad.x0 = next_y0;
    }

    void set_y1(int next_y1) override
    {
      ad.x1 = next_y1;
    }

    bool validate() override
    {
      return ad.validate();
    }

private:
    Ad &ad;
    int index;
};

class AdWrapper180 : public AdWrapper
{
public:
    explicit AdWrapper180(Ad &ad, int index) : ad(ad), index(index) {}

    Rec get_rec() override
    {
      return Rec{get_x0(), get_x1(), get_y0(), get_y1()};
    }

    void set_rec(Rec &rec) override
    {
      set_x0(rec.x0);
      set_x1(rec.x1);
      set_y0(rec.y0);
      set_y1(rec.y1);
    }

    int get_index() override
    {
      return index;
    }

    double compute_score() override
    {
      return ad.compute_score();
    }

    int get_x0() override
    {
      return MAX - ad.x1;
    }

    int get_x1() override
    {
      return MAX - ad.x0;
    }

    int get_y0() override
    {
      return MAX - ad.y1;
    }

    int get_y1() override
    {
      return MAX - ad.y0;
    }

    int get_spon_x() override
    {
      return MAX - ad.spon_x - 1;
    }

    int get_spon_y() override
    {
      return MAX - ad.spon_y - 1;
    }

    int get_spon_r() override
    {
      return ad.spon_r;
    }

    void set_x0(int next_x0) override
    {
      ad.x1 = MAX - next_x0;
    }

    void set_x1(int next_x1) override
    {
      ad.x0 = MAX - next_x1;
    }

    void set_y0(int next_y0) override
    {
      ad.y1 = MAX - next_y0;
    }

    void set_y1(int next_y1) override
    {
      ad.y0 = MAX - next_y1;
    }

    bool validate() override
    {
      return ad.validate();
    }

private:
    Ad &ad;
    int index;
};

class AdWrapper270 : public AdWrapper
{
public:
    explicit AdWrapper270(Ad &ad, int index) : ad(ad), index(index) {}

    Rec get_rec() override
    {
      return Rec{get_x0(), get_x1(), get_y0(), get_y1()};
    }

    void set_rec(Rec &rec) override
    {
      set_x0(rec.x0);
      set_x1(rec.x1);
      set_y0(rec.y0);
      set_y1(rec.y1);
    }

    int get_index() override
    {
      return index;
    }

    double compute_score() override
    {
      return ad.compute_score();
    }

    int get_x0() override
    {
      return ad.y0;
    }

    int get_x1() override
    {
      return ad.y1;
    }

    int get_y0() override
    {
      return MAX - ad.x1;
    }

    int get_y1() override
    {
      return MAX - ad.x0;
    }

    int get_spon_x() override
    {
      return ad.spon_y;
    }

    int get_spon_y() override
    {
      return MAX - ad.spon_x - 1;
    }

    int get_spon_r() override
    {
      return ad.spon_r;
    }

    void set_x0(int next_x0) override
    {
      ad.y0 = next_x0;
    }

    void set_x1(int next_x1) override
    {
      ad.y1 = next_x1;
    }

    void set_y0(int next_y0) override
    {
      ad.x1 = MAX - next_y0;
    }

    void set_y1(int next_y1) override
    {
      ad.x0 = MAX - next_y1;
    }

    bool validate() override
    {
      return ad.validate();
    }

private:
    Ad &ad;
    int index;
};


class AdCollection
{
public:

    AdCollection(vector<int> &xs, vector<int> &ys, vector<int> &rs)
    {
      n = sz(xs);
      ads = vector<Ad>(n);
      rep(i, sz(ads)) ads[i] = Ad(xs[i], ys[i], rs[i]);

      {
        vector<AdWrapper*> v(n);
        rep(i, sz(ads)) v[i] = new AdWrapper000(ads[i], i);
        wrappers_list.push_back(v);
      }
      {
        vector<AdWrapper*> v(n);
        rep(i, sz(ads)) v[i] = new AdWrapper090(ads[i], i);
        wrappers_list.push_back(v);
      }
      {
        vector<AdWrapper*> v(n);
        rep(i, sz(ads)) v[i] = new AdWrapper180(ads[i], i);
        wrappers_list.push_back(v);
      }
      {
        vector<AdWrapper*> v(n);
        rep(i, sz(ads)) v[i] = new AdWrapper270(ads[i], i);
        wrappers_list.push_back(v);
      }

      orig_id_to_wrapper_indexes = vector<vector<int>>(sz(wrappers_list));
      rep(i, sz(wrappers_list)){
        sort(wrappers_list[i].begin(), wrappers_list[i].end(), [](AdWrapper *a, AdWrapper *b){
            return a->get_spon_x() < b->get_spon_x();
        });
        vector<int> v(n);
        rep(j, sz(wrappers_list[i])) v[wrappers_list[i][j]->get_index()] = j;
        orig_id_to_wrapper_indexes.push_back(v);
      }

      rep(i, n){
        vector<int> v;
        rep(j, n)if(i != j) v.push_back(j);
        sort(v.begin(), v.end(), [&](int a, int b){
            int dxa = xs[a] - xs[i];
            int dya = ys[a] - ys[i];
            int dxb = xs[b] - xs[i];
            int dyb = ys[b] - ys[i];
            return dxa * dxa + dya * dya < dxb * dxb + dyb * dyb;
        });
        close_ads_list.push_back(v);
      }
    }

    vector<AdWrapper*>& get_random_wrappers()
    {
      return wrappers_list[XorShift::rand() & 3];
    }

    int n;
    vector<Ad> ads;
    vector<vector<AdWrapper*>> wrappers_list;
    vector<vector<int>> orig_id_to_wrapper_indexes;
    vector<vector<int>> close_ads_list;

};

class MyPair
{
public:
    int x, y;

};
class MyTriple
{
public:
    int x, y, is_lower;

    bool operator<(const MyTriple& triple) const {
      return x < triple.x;
    }
};
class MyBound
{
public:
    int upper, lower, left, right;
};
int fb_upper_lefts[MAX_N];
int fb_upper_rights[MAX_N];
int fb_lower_lefts[MAX_N];
int fb_lower_rights[MAX_N];
MyPair fb_upper_left_cands[MAX_N];
MyPair fb_upper_right_cands[MAX_N];
MyPair fb_lower_left_cands[MAX_N];
MyPair fb_lower_right_cands[MAX_N];
MyTriple fb_left_buf[MAX_N * 2];
MyTriple fb_right_buf[MAX_N * 2];
MyBound fb_left_bounds[MAX_N * 2];
MyBound fb_right_bounds[MAX_N * 2];
vector<Rec> fb_recs;
int fb_stack[2 * 256];
const int N16 = 1 << 4;
const int fb_mask = N16 - 1;
const int N8 = 8;
int fb_counts[N16];
int* fb_ptrs[N16];
void fb_sort(int *arg_start, const int *arg_end)
{
  const int l = (int) (arg_end - arg_start);

  int turn = 0;
  {
    int next_turn = 1 - turn;

    memset(fb_counts, 0, sizeof(fb_counts));
    for(int *ptr = arg_start; ptr < arg_end; ++ptr){
      int v = *ptr;
      int dst = v & fb_mask;
      ++fb_counts[dst];
    }

    int* next_stack = fb_stack + (next_turn << N8);
    int count_total = 0;
    rep(i, N16){
      fb_ptrs[i] = next_stack + count_total;
      count_total += fb_counts[i];
    }

    for(int *ptr = arg_start; ptr < arg_end; ++ptr){
      int v = *ptr;
      int dst = v & fb_mask;
      *(fb_ptrs[dst]++) = v;
    }

    turn = next_turn;
  }

  for(int shift = 4; shift < 16; shift += 4){
    int next_turn = 1 - turn;

    int* curr_stack = fb_stack + (turn << N8);
    memset(fb_counts, 0, sizeof(fb_counts));
    for(int *ptr = curr_stack, *end = curr_stack + l; ptr < end; ++ptr){
      int v = *ptr;
      int dst = (v >> shift) & fb_mask;
      ++fb_counts[dst];
    }

    int* next_stack = fb_stack + (next_turn << N8);
    int count_total = 0;
    rep(i, N16){
      fb_ptrs[i] = next_stack + count_total;
      count_total += fb_counts[i];
    }

    for(int *ptr = curr_stack, *end = curr_stack + l; ptr < end; ++ptr){
      int v = *ptr;
      int dst = (v >> shift) & fb_mask;
      *(fb_ptrs[dst]++) = v;
    }

    turn = next_turn;
  }

  memcpy(arg_start, fb_stack + (turn << N8), l * sizeof(int));

}

void random_kill()
{
  assert(XorShift::rand() % 1000 != 0);
}

Rec find_best_rec(vector<Ad> &ads, int tar_id, double p = 0.0)
{
  Ad &tar_ad = ads[tar_id];
  int *upper_left_i = fb_upper_lefts;
  int *upper_right_i = fb_upper_rights;
  int *lower_left_i = fb_lower_lefts;
  int *lower_right_i = fb_lower_rights;
  int offset = 16;
  int mask = (1 << offset) - 1;
  int tar_spon_x = tar_ad.spon_x;
  int tar_spon_y = tar_ad.spon_y;
  int tar_spon_r = tar_ad.spon_r;
  rep(i, sz(ads)) {
    if (i == tar_id) continue;
    Ad &ad = ads[i];
    bool is_upper = ad.y0 <= tar_spon_y;
    bool is_lower = tar_spon_y < ad.y1;
    bool is_left = ad.x0 <= tar_spon_x;
    bool is_right = tar_spon_x < ad.x1;
    int l_t = (i << offset) | (MAX - ad.x1);
    int r_t = (i << offset) | ad.x0;
    *upper_left_i = l_t;
    *upper_right_i = r_t;
    *lower_left_i = l_t;
    *lower_right_i = r_t;
    upper_left_i += (is_upper && is_left);
    upper_right_i += (is_upper && is_right);
    lower_left_i += (is_lower && is_left);
    lower_right_i += (is_lower && is_right);
  }

  fb_sort(fb_upper_lefts, upper_left_i);
  fb_sort(fb_lower_lefts, lower_left_i);
  fb_sort(fb_upper_rights, upper_right_i);
  fb_sort(fb_lower_rights, lower_right_i);

  MyPair *upper_left_cand_i = fb_upper_left_cands;
  MyPair *upper_right_cand_i = fb_upper_right_cands;
  MyPair *lower_left_cand_i = fb_lower_left_cands;
  MyPair *lower_right_cand_i = fb_lower_right_cands;

  upper_left_cand_i->x = MAX;
  upper_left_cand_i->y = 0;
  upper_left_cand_i++;
  lower_left_cand_i->x = MAX;
  lower_left_cand_i->y = MAX;
  lower_left_cand_i++;
  upper_right_cand_i->x = 0;
  upper_right_cand_i->y = 0;
  upper_right_cand_i++;
  lower_right_cand_i->x = 0;
  lower_right_cand_i->y = MAX;
  lower_right_cand_i++;

  for(int *i = fb_upper_lefts, prev_y = fb_upper_left_cands[0].y; i < upper_left_i; ++i){
    int id = *i >> offset;
    if(ads[id].y1 <= prev_y) continue;
    if(ads[id].x1 == (upper_left_cand_i - 1)->x) --upper_left_cand_i;
    upper_left_cand_i->x = ads[id].x1;
    upper_left_cand_i->y = ads[id].y1;
    upper_left_cand_i++;
    prev_y = ads[id].y1;
  }
  for(int *i = fb_upper_rights, prev_y = fb_upper_right_cands[0].y; i < upper_right_i; ++i){
    int id = *i >> offset;
    if(ads[id].y1 <= prev_y) continue;
    if(ads[id].x0 == (upper_right_cand_i - 1)->x) --upper_right_cand_i;
    upper_right_cand_i->x = ads[id].x0;
    upper_right_cand_i->y = ads[id].y1;
    upper_right_cand_i++;
    prev_y = ads[id].y1;
  }
  for(int *i = fb_lower_lefts, prev_y = fb_lower_left_cands[0].y; i < lower_left_i; ++i){
    int id = *i >> offset;
    if(prev_y <= ads[id].y0) continue;
    if(ads[id].x1 == (lower_left_cand_i - 1)->x) --lower_left_cand_i;
    lower_left_cand_i->x = ads[id].x1;
    lower_left_cand_i->y = ads[id].y0;
    lower_left_cand_i++;
    prev_y = ads[id].y0;
  }
  for(int *i = fb_lower_rights, prev_y = fb_lower_right_cands[0].y; i < lower_right_i; ++i){
    int id = *i >> offset;
    if(prev_y <= ads[id].y0) continue;
    if(ads[id].x0 == (lower_right_cand_i - 1)->x) --lower_right_cand_i;
    lower_right_cand_i->x = ads[id].x0;
    lower_right_cand_i->y = ads[id].y0;
    lower_right_cand_i++;
    prev_y = ads[id].y0;
  }

  upper_left_cand_i->x = 0;
  upper_left_cand_i->y = MAX;
  upper_left_cand_i++;
  lower_left_cand_i->x = 0;
  lower_left_cand_i->y = 0;
  lower_left_cand_i++;
  upper_right_cand_i->x = MAX;
  upper_right_cand_i->y = MAX;
  upper_right_cand_i++;
  lower_right_cand_i->x = MAX;
  lower_right_cand_i->y = 0;
  lower_right_cand_i++;

  MyTriple *triple_left_end = fb_left_buf;
  MyTriple *triple_right_end = fb_right_buf;
  {
    MyPair *upper_i = fb_upper_left_cands;
    MyPair *lower_i = fb_lower_left_cands;
    while(upper_i < upper_left_cand_i || lower_i < lower_left_cand_i){
      bool insert_lower = false;
      if(upper_i == upper_left_cand_i){
        insert_lower = true;
      }else if(lower_i == lower_left_cand_i){
        insert_lower = false;
      }else{
        insert_lower = lower_i->x > upper_i->x;
      }
      if(insert_lower){
        triple_left_end->x = lower_i->x;
        triple_left_end->y = lower_i->y;
        triple_left_end->is_lower = 1;
        ++lower_i;
      }else{
        triple_left_end->x = upper_i->x;
        triple_left_end->y = upper_i->y;
        triple_left_end->is_lower = 0;
        ++upper_i;
      }
      ++triple_left_end;
    }
  }
  {
    MyPair *upper_i = fb_upper_right_cands;
    MyPair *lower_i = fb_lower_right_cands;
    while(upper_i < upper_right_cand_i || lower_i < lower_right_cand_i){
      bool insert_lower = false;
      if(upper_i == upper_right_cand_i){
        insert_lower = true;
      }else if(lower_i == lower_right_cand_i){
        insert_lower = false;
      }else{
        insert_lower = lower_i->x < upper_i->x;
      }
      if(insert_lower){
        triple_right_end->x = lower_i->x;
        triple_right_end->y = lower_i->y;
        triple_right_end->is_lower = 1;
        ++lower_i;
      }else{
        triple_right_end->x = upper_i->x;
        triple_right_end->y = upper_i->y;
        triple_right_end->is_lower = 0;
        ++upper_i;
      }
      ++triple_right_end;
    }
  }

  fb_recs.clear();
  {
    int left_bound_i = 0;
    int left_upper = 0;
    int left_lower = MAX;
    for(MyTriple *left_ptr = fb_left_buf; left_ptr < triple_left_end; ++left_ptr) {
      int left = (left_ptr + 1)->x;
      if (left_ptr->is_lower == 0) {
        left_upper = left_ptr->y;
        if (tar_spon_y < left_upper) break;
      } else {
        left_lower = left_ptr->y;
        if (left_lower <= tar_spon_y) break;
      }
      if (tar_spon_x < left) continue;
      fb_left_bounds[left_bound_i].left = left;
      fb_left_bounds[left_bound_i].upper = left_upper;
      fb_left_bounds[left_bound_i].lower = left_lower;
      ++left_bound_i;
    }

    int right_bound_i = 0;
    int right_upper = 0;
    int right_lower = MAX;
    for(MyTriple *right_ptr = fb_right_buf; right_ptr < triple_right_end; ++right_ptr){
      int right = (right_ptr + 1)->x;
      if(right_ptr->is_lower == 0){
        right_upper = right_ptr->y;
        if(tar_spon_y < right_upper) break;
      }else{
        right_lower = right_ptr->y;
        if(right_lower <= tar_spon_y) break;
      }
      if(right <= tar_spon_x) continue;
      fb_right_bounds[right_bound_i].right = right;
      fb_right_bounds[right_bound_i].upper = right_upper;
      fb_right_bounds[right_bound_i].lower = right_lower;
      ++right_bound_i;
    }
    rep(l_i, left_bound_i)rep(r_i, right_bound_i){
        MyBound &l = fb_left_bounds[l_i];
        MyBound &r = fb_right_bounds[r_i];
        fb_recs.push_back({l.left, r.right, max(l.upper, r.upper), min(l.lower, r.lower)});
      }
  }
  my_assert(!fb_recs.empty());


  {
    sort(fb_recs.begin(), fb_recs.end(), [](const Rec &a, const Rec &b){
        return a.area() > b.area();
    });

    int prev = 0;
    rep(i, sz(fb_recs)){
      if(fb_recs[prev].area() != fb_recs[i].area()) prev = i;
      int s = XorShift::rand() % (i - prev + 1) + prev;
      swap(fb_recs[i], fb_recs[s]);
    }
  }
  Rec &rec = fb_recs[0];
  for(int i = 0; true; ++i){
    rec = fb_recs[i % sz(fb_recs)];
    if(p <= XorShift::rand_double()) break;
  }

  if(tar_spon_r < rec.area()){
    double r = (double) tar_spon_r / rec.area();
    double min_x_r = max(r, 1.0 / (rec.x1 - rec.x0));
    double max_x_r = min(1.0, r * (rec.y1 - rec.y0));
    double d = XorShift::rand_double();
    double x_r = pow(min_x_r, d) * pow(max_x_r, 1 - d);
    my_assert(min_x_r - 1e-8 <= x_r && x_r <= max_x_r + 1e-8);
    double y_r = r / x_r;
    my_assert(r - 1e-8 <= x_r && x_r <= 1 + 1e-8);
    my_assert(r - 1e-8 <= y_r && y_r <= 1 + 1e-8);
    int x_len = rec.x1 - rec.x0;
    int y_len = rec.y1 - rec.y0;
    int best_x_len = max(1, int(x_r * x_len));
    int best_y_len = max(1, int(y_r * y_len));
    int x_offset_max = x_len - best_x_len;
    int y_offset_max = y_len - best_y_len;
    int x_offset = XorShift::rand() % (x_offset_max + 1);
    int y_offset = XorShift::rand() % (y_offset_max + 1);
    int prob = 5;
    if(XorShift::rand() % prob == 0) x_offset = (XorShift::rand() % 2 == 0) ? 0 : x_offset_max;
    if(XorShift::rand() % prob == 0) y_offset = (XorShift::rand() % 2 == 0) ? 0 : y_offset_max;
    my_assert(0 < best_x_len && best_x_len <= x_len);
    my_assert(0 < best_y_len && best_y_len <= y_len);
    rec.x0 += x_offset;
    rec.y0 += y_offset;
    rec.x0 = min(tar_spon_x, max(tar_spon_x - best_x_len + 1, rec.x0));
    rec.y0 = min(tar_spon_y, max(tar_spon_y - best_y_len + 1, rec.y0));
    my_assert(rec.x0 + best_x_len <= rec.x1);
    my_assert(rec.y0 + best_y_len <= rec.y1);
    rec.x1 = rec.x0 + best_x_len;
    rec.y1 = rec.y0 + best_y_len;
  }

  return rec;
}


void shrink_to_best(AdWrapper *ad, bool shrink_x0, bool shrink_x1, bool shrink_y0, bool shrink_y1)
{
  int op_x0 = 0;
  int op_x1 = 1;
  int op_y0 = 2;
  int op_y1 = 3;
  vector<int> ops({op_x0, op_x1, op_y0, op_y1});
  shuffle_ints(ops);

  int spon_x = ad->get_spon_x();
  int spon_y = ad->get_spon_y();
  int spon_r = ad->get_spon_r();

  int x0 = ad->get_x0();
  int x1 = ad->get_x1();
  int y0 = ad->get_y0();
  int y1 = ad->get_y1();

  for(auto op: ops){
    int dx = x1 - x0;
    int dy = y1 - y0;
    int area = (x1 - x0) * (y1 - y0);
    int best_x_len = spon_r / dy;
    int best_y_len = spon_r / dx;

    if(area <= spon_r) break;

    if(op == op_x0){
      if(!shrink_x0) continue;
      int best_x0 = x1 - best_x_len;
      int next_x0 = min(best_x0, spon_x);
      ad->set_x0(next_x0);
      x0 = next_x0;
    } else if(op == op_x1){
      if(!shrink_x1) continue;
      int best_x1 = x0 + best_x_len;
      int next_x1 = max(best_x1, spon_x + 1);
      ad->set_x1(next_x1);
      x1 = next_x1;
    } else if(op == op_y0){
      if(!shrink_y0) continue;
      int best_y0 = y1 - best_y_len;
      int next_y0 = min(best_y0, spon_y);
      ad->set_y0(next_y0);
      y0 = next_y0;
    } else if(op == op_y1){
      if(!shrink_y1) continue;
      int best_y1 = y0 + best_y_len;
      int next_y1 = max(best_y1, spon_y + 1);
      ad->set_y1(next_y1);
      y1 = next_y1;
    }
  }
}

bool exchange_sub(AdCollection &ad_collection, vector<AdWrapper*> &wrappers, Score &score, int tar_id, int right_id, double score_diff = 0.0)
{
  AdWrapper *tar_ad = wrappers[tar_id];
  AdWrapper *right_ad = wrappers[right_id];

  if(tar_ad->get_spon_y() == right_ad->get_spon_y()){
    ++global_stats.exchange_failure;
    return false;
  }
  my_assert(tar_ad->get_y0() <= right_ad->get_spon_y() && right_ad->get_spon_y() < tar_ad->get_y1());
  my_assert(right_ad->get_y0() <= tar_ad->get_spon_y() && tar_ad->get_spon_y() < right_ad->get_y1());

  int x0 = tar_ad->get_x0();
  int x1 = right_ad->get_x1();
  int y0 = max(tar_ad->get_y0(), right_ad->get_y0());
  int y1 = min(tar_ad->get_y1(), right_ad->get_y1());
  Rec prev_tar = tar_ad->get_rec();
  Rec prev_right = right_ad->get_rec();

  int mid_y;
  {
    int min_y = min(tar_ad->get_spon_y(), right_ad->get_spon_y());
    int max_y = max(tar_ad->get_spon_y(), right_ad->get_spon_y());
    mid_y = XorShift::rand() % (max_y - min_y) + 1 + min_y;
  }
  bool optimize_mid_y = !true;

  score_diff -= tar_ad->compute_score();
  score_diff -= right_ad->compute_score();
  tar_ad->set_x0(x0);
  tar_ad->set_x1(x1);
  right_ad->set_x0(x0);
  right_ad->set_x1(x1);
  if(right_ad->get_spon_y() < tar_ad->get_spon_y()){
    if(optimize_mid_y){
      double r = ((double) right_ad->get_spon_r()) / (tar_ad->get_spon_r() + right_ad->get_spon_r());
      mid_y = int(round((y1 - y0) * r)) + y0;
      mid_y = max(right_ad->get_spon_y() + 1, min(tar_ad->get_spon_y(), mid_y));
    }
    right_ad->set_y0(y0);
    right_ad->set_y1(mid_y);
    tar_ad->set_y0(mid_y);
    tar_ad->set_y1(y1);
  }else{
    if(optimize_mid_y){
      double r = ((double) tar_ad->get_spon_r()) / (tar_ad->get_spon_r() + right_ad->get_spon_r());
      mid_y = int(round((y1 - y0) * r)) + y0;
      mid_y = max(tar_ad->get_spon_y() + 1, min(right_ad->get_spon_y(), mid_y));
    }
    tar_ad->set_y0(y0);
    tar_ad->set_y1(mid_y);
    right_ad->set_y0(mid_y);
    right_ad->set_y1(y1);
  }
  my_assert(tar_ad->validate());
  my_assert(right_ad->validate());

  bool tar_ad_updated = false;
  if (tar_ad->get_spon_r() <= tar_ad->get_rec().area()) {
    tar_ad_updated = true;
    shrink_to_best(tar_ad, true, true, true, true);
  }

  bool right_ad_updated = false;
  if(right_ad->get_spon_r() <= right_ad->get_rec().area()) {
    right_ad_updated = true;
    shrink_to_best(right_ad, true, true, true, true);
  }

  int turn = XorShift::rand() % 2;
  rep(step, 2){
    if (step == turn) {
      if(tar_ad_updated) continue;
      Rec tar_best_rec = find_best_rec(ad_collection.ads, tar_ad->get_index());
      ad_collection.ads[tar_ad->get_index()].set(tar_best_rec);
    }else{
      if(right_ad_updated) continue;
      Rec right_best_rec = find_best_rec(ad_collection.ads, right_ad->get_index());
      ad_collection.ads[right_ad->get_index()].set(right_best_rec);
    }
  }

  score_diff += tar_ad->compute_score();
  score_diff += right_ad->compute_score();

  if(!score.should_update(score_diff)){
    ++global_stats.exchange_failure;
    right_ad->set_rec(prev_right);
    tar_ad->set_rec(prev_tar);
    return false;
  }
  ++global_stats.exchange_success;
  score.add_score(score_diff);
  return true;
}

void exchange(AdCollection &ad_collection, Score &score)
{
  vector<AdWrapper*> &wrappers = ad_collection.get_random_wrappers();

  int target = XorShift::rand() % sz(wrappers);
  AdWrapper *tar_ad = wrappers[target];

  int right_ad_i = -1;
  {
    int tar_x1 = tar_ad->get_x1();
    int tar_y0 = tar_ad->get_y0();
    int tar_y1 = tar_ad->get_y1();
    int tar_spon_y = tar_ad->get_spon_y();
    for(int i = sz(wrappers) - 1; target < i; --i){
      AdWrapper *ad = wrappers[i];
      if(tar_x1 != ad->get_x0()) continue;
      int spon_y = ad->get_spon_y();
      if(spon_y < tar_y0 || tar_y1 <= spon_y) continue;
      if(tar_spon_y < ad->get_y0() || ad->get_y1() <= tar_spon_y) continue;
      right_ad_i = i;
      break;
    }
  }

  if(right_ad_i == -1){
    ++global_stats.exchange_failure;
    return;
  }

  exchange_sub(ad_collection, wrappers, score, target, right_ad_i);
}

void stab(AdCollection &ad_collection, Score &score)
{
  vector<AdWrapper*> &wrappers = ad_collection.get_random_wrappers();

  int target = XorShift::rand() % sz(wrappers);
  AdWrapper *tar_ad = wrappers[target];

  int right_ad_i = -1;
  int tar_x1 = tar_ad->get_x1();
  int tar_y0 = tar_ad->get_y0();
  int tar_y1 = tar_ad->get_y1();
  for(int i = sz(wrappers) - 1; target < i; --i){
    AdWrapper *ad = wrappers[i];
    if(tar_x1 != ad->get_x0()) continue;
    if(tar_y0 < ad->get_y0()) continue;
    if(ad->get_y1() < tar_y1) continue;
    if(tar_y0 <= ad->get_spon_y() && ad->get_spon_y() < tar_y1) continue;
    right_ad_i = i;
    break;
  }

  if(right_ad_i == -1){
    ++global_stats.stab_failure;
    return;
  }
  AdWrapper *right_ad = wrappers[right_ad_i];

  Rec tar_rec = tar_ad->get_rec();
  Rec right_rec = right_ad->get_rec();

  double score_diff = 0;
  score_diff -= tar_ad->compute_score();
  score_diff -= right_ad->compute_score();

  tar_ad->set_x1(right_ad->get_x1());
  if(right_ad->get_spon_y() < tar_ad->get_y0()){
    right_ad->set_y1(tar_ad->get_y0());
  }else{
    right_ad->set_y0(tar_ad->get_y1());
  }

  if(false){
    Rec tar_best_rec = find_best_rec(ad_collection.ads, tar_ad->get_index());
    ad_collection.ads[tar_ad->get_index()].set(tar_best_rec);

    Rec right_best_rec = find_best_rec(ad_collection.ads, right_ad->get_index());
    ad_collection.ads[right_ad->get_index()].set(right_best_rec);

  }else{
    shrink_to_best(tar_ad, true, true, true, true);

    Rec right_best_rec = find_best_rec(ad_collection.ads, right_ad->get_index());
    ad_collection.ads[right_ad->get_index()].set(right_best_rec);
  }


  score_diff += tar_ad->compute_score();
  score_diff += right_ad->compute_score();

  if(!score.should_update(score_diff)){
    ++global_stats.stab_failure;
    tar_ad->set_rec(tar_rec);
    right_ad->set_rec(right_rec);
    return;
  }
  ++global_stats.stab_success;
  score.add_score(score_diff);
}

double extend_sub(AdCollection &ad_collection, vector<AdWrapper*> &wrappers, int tar_id, int next_x0, Score &score)
{
  AdWrapper *tar_ad = wrappers[tar_id];

  vector<int> ids;
  {
    int tar_x0 = tar_ad->get_x0();
    int tar_y0 = tar_ad->get_y0();
    int tar_y1 = tar_ad->get_y1();
    vector<int> cands;
    for(int i = tar_id - 1; 0 <= i; --i){
      AdWrapper *ad = wrappers[i];
      int x1 = ad->get_x1();
      if (x1 <= next_x0) continue;
      if (ad->get_y1() <= tar_y0) continue;
      if (tar_y1 <= ad->get_y0()) continue;
      cands.push_back(i);
      next_x0 = max(next_x0, ad->get_spon_x() + 1);
      if(tar_x0 <= next_x0) return 0;
    }
    if(tar_x0 <= next_x0) return 0;
    for(int i: cands){
      AdWrapper *ad = wrappers[i];
      if(ad->get_x1() <= next_x0) continue;
      ids.push_back(i);
    }
  }

  double score_diff = 0;
  Rec tar_rec = tar_ad->get_rec();
  {
    int x0 = tar_ad->get_x0();
    score_diff -= tar_ad->compute_score();
    tar_ad->set_x0(next_x0);
    shrink_to_best(tar_ad, false, true, true, true);
    score_diff += tar_ad->compute_score();
  }

  for(int i : ids){
    AdWrapper* ad = wrappers[i];
    score_diff -= ad->compute_score();
    my_assert(ad->get_spon_x() < next_x0);
    int x1 = ad->get_x1();
    ad->set_x1(next_x0);
    score_diff += ad->compute_score();
    ad->set_x1(x1);
  }

  if(!score.should_update(score_diff)){
    ++global_stats.extend_failure;
    tar_ad->set_rec(tar_rec);
    return 0;
  }
  ++global_stats.extend_success;
  for(int i : ids){
    AdWrapper* ad = wrappers[i];
    my_assert(ad->get_spon_x() < next_x0);
    ad->set_x1(next_x0);
  }
  score.add_score(score_diff);
  return score_diff;
}

void extend(AdCollection &ad_collection, Score &score)
{
  vector<AdWrapper*> &wrappers = ad_collection.get_random_wrappers();

  int target = XorShift::rand() % sz(wrappers);

  AdWrapper *tar_ad = wrappers[target];
  if(tar_ad->get_x0() == 0) return;

  int abs_limit = Params::extend_abs_limit;
  int maxi = min(tar_ad->get_x0(), abs_limit);
  int stride = XorShift::rand() % maxi + 1;
  int next_x0 = tar_ad->get_x0() - stride;

  extend_sub(ad_collection, wrappers, target, next_x0, score);
}


vector<int> offer_ops({0, 1, 2});
void offer(AdCollection &ad_collection, Score &score)
{
  vector<AdWrapper*> &wrappers = ad_collection.get_random_wrappers();
  vector<Ad> &ads = ad_collection.ads;

  int target = XorShift::rand() % sz(wrappers);
  AdWrapper *tar_ad = wrappers[target];

  int tar_x0 = tar_ad->get_x0();
  int tar_x1 = tar_ad->get_x1();
  int tar_y0 = tar_ad->get_y0();
  int tar_y1 = tar_ad->get_y1();
  vector<int> uppers({});
  vector<int> lowers({});
  vector<int> middles({});
  bool accept_bigger = XorShift::rand() % 2 == 0;
  for(int i = sz(wrappers) - 1; target < i; --i){
    AdWrapper *ad = wrappers[i];
    if(tar_x1 != ad->get_x0()) continue;
    if(ad->get_spon_y() < tar_y0 || tar_y1 <= ad->get_spon_y()) continue;
    if(ad->get_y0() <= tar_ad->get_spon_y() && tar_ad->get_spon_y() < ad->get_y1()){
      middles.push_back(i);
    }else {
      if (!accept_bigger && (ad->get_y0() < tar_y0 || tar_x1 < ad->get_y1())) continue;
      if(ad->get_y1() <= tar_ad->get_spon_y()) uppers.push_back(i);
      else lowers.push_back(i);
    }
  }
  my_assert(sz(middles) <= 1);

  sort(uppers.begin(), uppers.end(), [&](int a, int b){ return wrappers[a]->get_y0() < wrappers[b]->get_y0(); });
  sort(lowers.begin(), lowers.end(), [&](int a, int b){ return wrappers[a]->get_y0() > wrappers[b]->get_y0(); });

  shuffle_ints(offer_ops);
  double cut_p = 0.25;
  bool use_optimize = XorShift::rand_double() < 0.2;
  for(int op: offer_ops){
    double score_diff = 0;

    vector<pair<int, Rec>> dat;
    dat.push_back(make_pair(tar_ad->get_index(), ads[tar_ad->get_index()].get_rec()));

    score_diff -= tar_ad->compute_score();

    if(op == 0){
      if(sz(uppers) == 0) continue;

      rep(i, sz(uppers)){
        if(i != 0 && XorShift::rand_double() <= cut_p) break;
        AdWrapper *ad = wrappers[uppers[i]];
        dat.push_back(make_pair(ad->get_index(), ads[ad->get_index()].get_rec()));
        score_diff -= ad->compute_score();
        ad->set_y0(max(ad->get_y0(), tar_ad->get_y0()));
        tar_ad->set_y0(ad->get_y1());
        if(use_optimize) {
          Rec rec = find_best_rec(ads, ad->get_index());
          ads[ad->get_index()].set(rec);
        }else{
          ad->set_x0(tar_x0);
          shrink_to_best(ad, true, true, true, true);
        }
        score_diff += ad->compute_score();
      }
      my_assert(!dat.empty());

      Rec new_tar_rec = find_best_rec(ads, tar_ad->get_index());
      ads[tar_ad->get_index()].set(new_tar_rec);
      score_diff += tar_ad->compute_score();
      my_assert(tar_ad->validate());

      if(!score.should_update(score_diff)){
        ++global_stats.offer_failure;
        rep(i, sz(dat)) ads[dat[i].first].set(dat[i].second);
      }else {
        ++global_stats.offer_success;
        score.add_score(score_diff);
        return;
      }
    }else if(op == 1) {
      if (sz(lowers) == 0) continue;

      rep(i, sz(lowers)) {
        if (i != 0 && XorShift::rand_double() <= cut_p) break;
        AdWrapper *ad = wrappers[lowers[i]];
        dat.push_back(make_pair(ad->get_index(), ads[ad->get_index()].get_rec()));
        score_diff -= ad->compute_score();
        ad->set_y1(min(ad->get_y1(), tar_ad->get_y1()));
        tar_ad->set_y1(ad->get_y0());
        if(use_optimize) {
          Rec rec = find_best_rec(ad_collection.ads, ad->get_index());
          ad_collection.ads[ad->get_index()].set(rec);
        }else{
          ad->set_x0(tar_x0);
          shrink_to_best(ad, true, true, true, true);
        }
        score_diff += ad->compute_score();
      }
      my_assert(!dat.empty());

      Rec new_tar_rec = find_best_rec(ad_collection.ads, tar_ad->get_index());
      ad_collection.ads[tar_ad->get_index()].set(new_tar_rec);
      score_diff += tar_ad->compute_score();
      my_assert(tar_ad->validate());

      if (!score.should_update(score_diff)) {
        ++global_stats.offer_failure;
        rep(i, sz(dat)) ads[dat[i].first].set(dat[i].second);
      } else {
        ++global_stats.offer_success;
        score.add_score(score_diff);
        return;
      }
    }else if(op == 2){
      if(middles.empty()) continue;
      my_assert(sz(middles) == 1);
      score_diff += tar_ad->compute_score();
      if(exchange_sub(ad_collection, wrappers, score, target, middles[0], score_diff)){
        ++global_stats.composite_success;
        return;
      }else{
        ++global_stats.composite_failure;
        rep(i, sz(dat)) ads[dat[i].first].set(dat[i].second);
      }
    }else{
      my_assert(false);
    }
  }
}

bool optimize(AdCollection &ad_collection, Score &score, double p = 0.75, int target = -1)
{
  vector<Ad> &ads = ad_collection.ads;
  target = (target != -1) ? target : XorShift::rand() % sz(ads);
  Ad &tar_ad = ads[target];
  Rec rec = find_best_rec(ad_collection.ads, target, p);

  double score_diff = 0;
  Rec orig_rec = tar_ad.get_rec();
  score_diff -= tar_ad.compute_score();
  tar_ad.set(rec);
  score_diff += tar_ad.compute_score();
  if(!score.should_update(score_diff)){
    tar_ad.set(orig_rec);
    ++global_stats.optimal_failure;
    return false;
  }
  score.add_score(score_diff);
  ++global_stats.optimal_success;
  return true;
}

void destroy(AdCollection &ad_collection, Score &score)
{
  vector<Ad> &ads = ad_collection.ads;

  int center = XorShift::rand() % ad_collection.n;
  double center_score = ads[center].compute_score();
  rep(i, int(sqrt(ad_collection.n))){
    int t = XorShift::rand() % ad_collection.n;
    double d = ads[t].compute_score();
    if(d < center_score){
      center = t;
      center_score = d;
    }
  }

  vector<int> v({center});
  rep(i, 4) v.push_back(ad_collection.close_ads_list[center][i]);
  sort(v.begin(), v.end(), [&](int a, int b){
      return ads[a].compute_score() < ads[b].compute_score();
  });

  vector<Rec> dat({});
  rep(i, sz(v)) dat.push_back(ads[v[i]].get_rec());

  double score_diff = 0.0;
  rep(i, sz(v)){
    Ad &ad = ads[v[i]];
    score_diff -= ad.compute_score();
    ad.x0 = ad.spon_x;
    ad.x1 = ad.spon_x + 1;
    ad.y0 = ad.spon_y;
    ad.y1 = ad.spon_y + 1;
  }

  rep(i, sz(v)){
    Rec rec = find_best_rec(ads, v[i]);
    Ad &ad = ads[v[i]];
    ad.set(rec);
    score_diff += ad.compute_score();
  }

  if(score.should_update(score_diff)){
    ++global_stats.destroy_success;
    score.add_score(score_diff);
    return;
  }

  ++global_stats.destroy_failure;
  rep(i, sz(v)) ads[v[i]].set(dat[i]);
}

void run(AdCollection &ad_collection, Score &score)
{
  vector<int> ops = Params::get_ops();
  while(0.0 < score.get_temperature()){
    int id = ops[XorShift::rand() % sz(ops)];
    if(id == Params::extend_id) {
      extend(ad_collection, score);
      continue;
    }
    if(id == Params::optimal_id){
      optimize(ad_collection, score);
      continue;
    }
    if(id == Params::stab_id){
      stab(ad_collection, score);
      continue;
    }
    if(id == Params::exchange_id){
      exchange(ad_collection, score);
      continue;
    }
    if(id == Params::offer_id){
      offer(ad_collection, score);
      continue;
    }
    if(id == Params::destroy_id){
      destroy(ad_collection, score);
      continue;
    }
    my_assert(false);
  }
}


double run(AdCollection &ad_collection, double time_limit, vector<double> &prev_scores, int try_count)
{
  Timer timer;
  Score score(timer, time_limit);
  rep(i, sz(ad_collection.ads)) score.add_score(ad_collection.ads[i].compute_score());

  {
    vector<int> ids({});
    rep(i, ad_collection.n) ids.push_back(i);
    shuffle_ints(ids);
    if (try_count == 0) {
    } else if (try_count == 1) {
      sort(ids.begin(), ids.end(), [&](int a, int b) {
          return ad_collection.ads[a].get_rec().area() > ad_collection.ads[b].get_rec().area();
      });
    } else {
      sort(ids.begin(), ids.end(), [&](int a, int b) {
          return prev_scores[a] < prev_scores[b];
      });
    }
    for (int id: ids) optimize(ad_collection, score, 0.0, id);
  }

  run(ad_collection, score);
  rep(step, 4) {
    vector<int> ids({});
    rep(i, ad_collection.n) ids.push_back(i);
    sort(ids.begin(), ids.end(), [&](int a, int b) {
        return ad_collection.ads[a].get_rec().area() < ad_collection.ads[b].get_rec().area();
    });
    for(int id: ids){
      optimize(ad_collection, score, 0.0, id);
      for(int diff: {10, 1}){
        for (vector<AdWrapper*> &wrappers: ad_collection.wrappers_list){
          AdWrapper *tar_ad = wrappers[id];
          while (diff <= tar_ad->get_x0()) {
            int next_x0 = tar_ad->get_x0() - diff;
            double score_diff = extend_sub(ad_collection, wrappers, id, next_x0, score);
            if(score_diff <= 0) break;
          }
        }
      }
    }
  }

  return score.score;
}

int run(Timer &timer)
{

  int n;
  cin >> n;
  vector<int> xs(n), ys(n), rs(n);
  rep(i, n) cin >> xs[i] >> ys[i] >> rs[i];

  const int num_try = 3 * 200 / n;
  const double time_limit = 4.5;

  double best_score = 0;
  vector<Rec> best_recs(n);
  vector<double> prev_scores(n, 0);

  rep(step, num_try){
    double next_time_limit = (time_limit - timer.getElapsed()) / (num_try - step);
    if(next_time_limit <= 0) break;

    AdCollection ad_collection(xs, ys, rs);

    double score = run(ad_collection, next_time_limit, prev_scores, step);
    if(best_score < score){
      best_score = score;
      rep(i, n) best_recs[i] = ad_collection.ads[i].get_rec();
    }
    global_stats.scores.push_back(score / n);
    for(vector<AdWrapper*> &v: ad_collection.wrappers_list)for(AdWrapper *wrapper: v) delete wrapper;
    rep(i, n) prev_scores[i] = ad_collection.ads[i].compute_score();
  }

  for(Rec &rec: best_recs){
    cout << rec.x0 << " " << rec.y0 << " " << rec.x1 << " " << rec.y1 << endl;
  }

  return int(round(1e9 * best_score / n));
}

int main()
{
  Timer timer;
  int score = run(timer);
#ifdef LOCAL
  local_post_process(score);
#endif
}
