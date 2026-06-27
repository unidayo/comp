#pragma GCC optimize("Ofast")
#define _USE_MATH_DEFINES
#include "bits/stdc++.h"

using namespace std;

using ll = int64_t;
using db = double;
using ld = long double;
using vi = vector<int>;
using vl = vector<ll>;
using vvi = vector<vector<int>>;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
using vpii = vector<pii>;
using vpll = vector<pll>;

constexpr char newl = '\n';
constexpr double eps = 1e-10;

#define FOR(i,a,b) for (int i = (a); i < (b); i++)
#define F0R(i,b) FOR(i,0,b)
#define RFO(i,a,b) for (int i = ((b)-1); i >=(a); i--)
#define RF0(i,b) RFO(i,0,b)
#define show(x) cout << #x << " = " << x << '\n';
#define rng(a) a.begin(),a.end()
#define rrng(a) a.rbegin(),a.rend()
#define sz(x) (int)(x).size()
#define YesNo {cout<<"Yes";}else{cout<<"No";}
#define YESNO {cout<<"YES";}else{cout<<"NO";}
#define v(T) vector<T>
#define vv(T) vector<vector<T>>

template<typename T1, typename T2> inline void chmin(T1& a, T2 b) { if (a > b) a = b; }
template<typename T1, typename T2> inline void chmax(T1& a, T2 b) { if (a < b) a = b; }
template<class T> bool lcmp(const pair<T, T>& l, const pair<T, T>& r) {
  return l.first < r.first;
}
template<class T> istream& operator>>(istream& i, v(T)& v) {
  F0R(j, sz(v)) i >> v[j];
  return i;
}
template<class A, class B> istream& operator>>(istream& i, pair<A, B>& p) {
  return i >> p.first >> p.second;
}
template<class A, class B, class C> istream& operator>>(istream& i, tuple<A, B, C>& t) {
  return i >> get<0>(t) >> get<1>(t) >> get<2>(t);
}
template<class T> ostream& operator<<(ostream& o, const pair<T, T>& v) {
  o << "(" << v.first << "," << v.second << ")";
  return o;
}
template<class T> ostream& operator<<(ostream& o, const vector<T>& v) {
  F0R(i, v.size()) {
    o << v[i] << ' ';
  }
  o << newl;
  return o;
}
template<class T> ostream& operator<<(ostream& o, const set<T>& v) {
  for (auto e : v) {
    o << e << ' ';
  }
  o << newl;
  return o;
}
template<class T1, class T2> ostream& operator<<(ostream& o, const map<T1, T2>& m) {
  for (auto& p : m) {
    o << p.first << ": " << p.second << newl;
  }
  o << newl;
  return o;
}

#if 1

/* -------------------------------------------
   汎用
*/

struct Random {
  Random() : distReal01(0.0, 1.0) {
    mt = mt19937_64(rd());
  }
  void setSeed(ll seed) {
    mt.seed(seed);
  }
  double GetDouble() {
    return distReal01(mt);
  }
  int GetInt(int n) {
    uniform_int_distribution<> dist(0, n - 1);
    return dist(mt);
  }
  template<class T>
  auto Pick(T cont) {
    uniform_int_distribution<> dist(0, cont.size() - 1);
    return cont[dist(mt)];
  }
protected:
  uniform_real_distribution<double> distReal01;
  random_device rd;
  mt19937_64 mt;
};
Random rnd;
enum d4s : int { LEFT = 0, UP, RIGHT, DOWN };

uint32_t xor128()
{
  static uint32_t x = 123456789, y = 362436069, z = 521288629, w = 88675123;
  uint32_t t;
  t = (x ^ (x << 11)); x = y; y = z; z = w; return(w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}
double xor128d() {
  return xor128() / 4294967296.0;
}

using namespace std::chrono;
class Timer {
public:
  Timer() {
  }
  Timer(ostream& os) : Timer() {
    this->os = &os;
  }
  void start() {
    st = high_resolution_clock::now();
  }
  ll elapsed() {
    return dur2ms(now() - st);
  }
  void setLimit(int ms) {
    et = st + milliseconds(ms);
  }
  bool within() {
    return high_resolution_clock::now() < et;
  }
  void print() {
    if (os) {
      (*os) << elapsed() <<"ms / " <<dur2ms(et-st)<<"ms"<< newl;
    }
  }
  high_resolution_clock::time_point now() {
    return high_resolution_clock::now();
  }
  high_resolution_clock::time_point startTime() {
    return st;
  }
  high_resolution_clock::time_point endTime() {
    return et;
  }

protected:
  int dur2ms(chrono::nanoseconds duration) {
    return duration_cast<milliseconds>(duration).count();
  }
  ostream* os = nullptr;
  high_resolution_clock::time_point st, et;
};


/* -------------------------------------------
   ローカル
*/

struct Rect;

struct Dem {
public:
  int x, y, r;

  constexpr db eval(int s) const {
    db a = 1 - (db)min(s, this->r) / max(s, this->r);
    return 1 - a * a;
  }
  constexpr db eval(int h, int w) const {
    int s = h * w;
    db a = 1 - (db)min(s, this->r) / max(s, this->r);
    return 1 - a * a;
  }
};

struct Rect {
  const db infForce = 1000;
public:
  Rect() {}
  Rect(int x, int y, int x2, int y2) :x(x), y(y), x2(x2), y2(y2) {
  }
  Rect(const Rect& s) : x(s.x),y(s.y),x2(s.x2),y2(s.y2) {
  }
  Rect& operator = (const Rect& s) {
    x = s.x; y = s.y; x2 = s.x2; y2 = s.y2;
    return *this;
  }

  int x = 0, y = 0, x2 = 0, y2 = 0;
  constexpr int w() const { return x2 - x; }
  constexpr int h() const { return y2 - y; }

  constexpr bool hit(const Rect& d) const {
    return x < d.x2&& y < d.y2&& d.x < x2&& d.y < y2;
  }
  constexpr db eval(const Dem& d) const {
    if (d.x < x || x2 <= d.x || d.y < y || y2 <= d.y) return 0;
    return d.eval(h(), w());
  }
};


// 4方向の逆
constexpr int invDir(int d) {
  return (d + 2) & 0x3;
}
constexpr bool dirHrz(int d) {
  return d == d4s::LEFT || d == d4s::RIGHT;
}

const int XMIN = 0, XMAX = 10000, YMIN = 0, YMAX = 10000, NMAX = 200;
const int ms1 = 0, ms2 = 4900, ms3 = 4900;
const int AREA_DIV_CNT = 5, AREA_DIV_SIZE = 2000;
int N;
v(Dem) ds;
vi dsSorted;
v(Rect) rs, finals;
vv(unordered_set<int>) touches; // 箱が辺で接している他の箱
v(unordered_set<int>) xs, ys, x2s, y2s; // 座標ごとに、その座標に辺がある箱を記録する
db bestScore = 0;
Timer timer(cerr);
vi prRects;
// debug
int goodCnt = 0, badCnt = 0;

vi rcPrmValids;
vv(ld) rcPrms;
#define invRp(i) rcPrmValids[i]=0
void rpInit() {
  fill(rng(rcPrmValids), 0);
}
// 垂直に伸びる力
db vforce(int i) {
  const int X = 0;
  if (!(rcPrmValids[i] & (1<<X))) {
    rcPrmValids[i] |= 1 << X;
    int s = rs[i].h() * rs[i].w();
    rcPrms[i][X] = ds[i].eval(s + rs[i].w()) - ds[i].eval(s);
  }
  return rcPrms[i][X];
}
// 水平に伸びる力
db hforce(int i) {
  const int X = 1;
  if (!(rcPrmValids[i] & (1 << X))) {
    rcPrmValids[i] |= 1 << X;
    int s = rs[i].h() * rs[i].w();
    rcPrms[i][X] = ds[i].eval(s + rs[i].h()) - ds[i].eval(s);
  }
  return rcPrms[i][X];
}
// 垂直に押される力への反発
db vrforce(int i) {
  const int X = 2;
  if (!(rcPrmValids[i] & (1 << X))) {
    rcPrmValids[i] |= 1 << X;
    int s = rs[i].h() * rs[i].w();
    rcPrms[i][X] = ds[i].eval(s) - ds[i].eval(s - rs[i].w());
  }
  return rcPrms[i][X];
}
// 水平に押される力への反発
db hrforce(int i) {
  const int X = 3;
  if (!(rcPrmValids[i] & (1 << X))) {
    rcPrmValids[i] |= 1 << X;
    int s = rs[i].h() * rs[i].w();
    rcPrms[i][X] = ds[i].eval(s) - ds[i].eval(s - rs[i].h());
  }
  return rcPrms[i][X];
}
db reForce(int i, int dir) {
  const db infForce = 1000;
  switch (dir) {
  case d4s::LEFT:
    if (rs[i].x == ds[i].x) return infForce;
    return hrforce(i);
  case d4s::UP:
    if (rs[i].y == ds[i].y)return infForce;
    return vrforce(i);
  case d4s::RIGHT:
    if (rs[i].x2 - 1 == ds[i].x)return infForce;
    return hrforce(i);
  default:
    if (rs[i].y2 - 1 == ds[i].y)return infForce;
    return vrforce(i);
  }
}

void output(const v(Rect)& rs) {
  for(auto &r:rs) {
    cout << r.x << ' ' << r.y << ' ' << r.x2 << ' ' << r.y2 << newl;
  }
}

bool hitTest(int idx) {
  F0R(i, N) {
    if (i == idx) continue;
    if (rs[idx].hit(rs[i])) return true;
  }
  return false;
}

// 四角の4方向にどれだけスペースがあるか. O(N)
int sideSpace(int ri, int dr) {
  int re = 0;
  auto& rc = rs[ri];
  switch (dr) {
  case d4s::LEFT:
    re = rc.x-XMIN;
    F0R(i, N) {
      if (rs[i].x2 <= rc.x && rc.y < rs[i].y2 && rs[i].y < rc.y2) {
        chmin(re, rc.x - rs[i].x2);
      }
    }
    break;
  case d4s::UP:
    re = rc.y - YMIN;
    F0R(i, N) {
      if (rs[i].y2 <= rc.y && rc.x < rs[i].x2 && rs[i].x < rc.x2) {
        chmin(re, rc.y - rs[i].y2);
      }
    }
    break;
  case d4s::RIGHT:
    re = XMAX-rc.x2;
    F0R(i, N) {
      if (rc.x2 <= rs[i].x && rc.y < rs[i].y2 && rs[i].y < rc.y2) {
        chmin(re, rs[i].x - rc.x2);
      }
    }
    break;
  case d4s::DOWN:
    re = YMAX - rc.y2;
    F0R(i, N) {
      if (rc.y2 <= rs[i].y && rc.x < rs[i].x2 && rs[i].x < rc.x2) {
        chmin(re, rs[i].y - rc.y2);
      }
    }
    break;
  }
  return re;
}

// 点だけ含む1x1の形にする
void init(int i) {
  rs[i].x = ds[i].x;
  rs[i].y = ds[i].y;
  rs[i].x2 = ds[i].x + 1;
  rs[i].y2 = ds[i].y + 1;
}
void init(v(Rect)& rs) {
  rs.resize(N);
  F0R(i, N) {
    init(i);
  }
}

// clearなし
void initTouchesE(int i, int dir) {
  switch (dir) {
  case d4s::LEFT:
    for (auto j : x2s[rs[i].x]) {
      if (rs[i].y < rs[j].y2 && rs[j].y < rs[i].y2) {
        touches[i][dir].insert(j);
      }
    }
    break;
  case d4s::UP:
    for (auto j : y2s[rs[i].y]) {
      if (rs[i].x < rs[j].x2 && rs[j].x < rs[i].x2) {
        touches[i][dir].insert(j);
      }
    }
    break;
  case d4s::RIGHT:
    for (auto j : xs[rs[i].x2]) {
      if (rs[i].y < rs[j].y2 && rs[j].y < rs[i].y2) {
        touches[i][dir].insert(j);
      }
    }
    break;
  case d4s::DOWN:
    for (auto j : ys[rs[i].y2]) {
      if (rs[i].x < rs[j].x2 && rs[j].x < rs[i].x2) {
        touches[i][dir].insert(j);
      }
    }
    break;
  }
}
void initTouches(int i, int dir) {
  touches[i][dir].clear();
  initTouchesE(i, dir);
}
// eraseなし、2方向
void initTouches2(int i, int dir) {
  switch (dir) {
  case d4s::LEFT:
    for (auto j : x2s[rs[i].x]) {
      if (rs[i].y < rs[j].y2 && rs[j].y < rs[i].y2) {
        touches[i][dir].insert(j);
        touches[j][d4s::RIGHT].insert(i);
      }
    }
    break;
  case d4s::UP:
    for (auto j : y2s[rs[i].y]) {
      if (rs[i].x < rs[j].x2 && rs[j].x < rs[i].x2) {
        touches[i][dir].insert(j);
        touches[j][d4s::DOWN].insert(i);
      }
    }
    break;
  }
}
void initTouches() {
  FOR(x, XMIN, XMAX + 1) {
    xs[x].clear(); x2s[x].clear();
  }
  FOR(y, YMIN, YMAX + 1) {
    ys[y].clear(); y2s[y].clear();
  }
  F0R(i, N) {
    xs[rs[i].x].insert(i);
    x2s[rs[i].x2].insert(i);
    ys[rs[i].y].insert(i);
    y2s[rs[i].y2].insert(i);
  }
#if 0
  F0R(i, N) {
    F0R(d, 4) {
      initTouches(i, d);
    }
  }
#else
  F0R(i, N) {
    F0R(d, 4) {
      touches[i][d].clear();
    }
  }
  F0R(i, N) {
    F0R(d, 2) {
      initTouches2(i, d);
    }
  }
#endif
}

void increLeft(int ri) {
  invRp(ri);
  for (auto j : x2s[rs[ri].x]) {
    if (rs[j].y2 == rs[ri].y) {
      touches[ri][d4s::UP].insert(j);
      touches[j][d4s::DOWN].insert(ri);
    }
    else if (rs[ri].y2 == rs[j].y) {
      touches[ri][d4s::DOWN].insert(j);
      touches[j][d4s::UP].insert(ri);
    }
  }
  xs[rs[ri].x].erase(ri);
  rs[ri].x--;
  xs[rs[ri].x].insert(ri);
  initTouchesE(ri, d4s::LEFT);
  for (auto j : touches[ri][d4s::LEFT]) {
    touches[j][d4s::RIGHT].insert(ri);
  }
}
void increUp(int ri) {
  invRp(ri);
  for (auto j : y2s[rs[ri].y]) {
    if (rs[j].x2 == rs[ri].x) {
      touches[ri][d4s::LEFT].insert(j);
      touches[j][d4s::RIGHT].insert(ri);
    }
    else if (rs[ri].x2 == rs[j].x) {
      touches[ri][d4s::RIGHT].insert(j);
      touches[j][d4s::LEFT].insert(ri);
    }
  }
  ys[rs[ri].y].erase(ri);
  rs[ri].y--;
  ys[rs[ri].y].insert(ri);
  initTouchesE(ri, d4s::UP);
  for (auto j : touches[ri][d4s::UP]) {
    touches[j][d4s::DOWN].insert(ri);
  }
}
void increRight(int ri) {
  invRp(ri);
  for (auto j : xs[rs[ri].x2]) {
    if (rs[j].y2 == rs[ri].y) {
      touches[ri][d4s::UP].insert(j);
      touches[j][d4s::DOWN].insert(ri);
    }
    else if (rs[ri].y2 == rs[j].y) {
      touches[ri][d4s::DOWN].insert(j);
      touches[j][d4s::UP].insert(ri);
    }
  }
  x2s[rs[ri].x2].erase(ri);
  rs[ri].x2++;
  x2s[rs[ri].x2].insert(ri);
  initTouchesE(ri, d4s::RIGHT);
  for (auto j : touches[ri][d4s::RIGHT]) {
    touches[j][d4s::LEFT].insert(ri);
  }
}
void increDown(int ri) {
  invRp(ri);
  for (auto j : ys[rs[ri].y2]) {
    if (rs[j].x2 == rs[ri].x) {
      touches[ri][d4s::LEFT].insert(j);
      touches[j][d4s::RIGHT].insert(ri);
    }
    else if (rs[ri].x2 == rs[j].x) {
      touches[ri][d4s::RIGHT].insert(j);
      touches[j][d4s::LEFT].insert(ri);
    }
  }
  y2s[rs[ri].y2].erase(ri);
  rs[ri].y2++;
  y2s[rs[ri].y2].insert(ri);
  initTouchesE(ri, d4s::DOWN);
  for (auto j : touches[ri][d4s::DOWN]) {
    touches[j][d4s::UP].insert(ri);
  }
}
void decreLeft(int ri, int si) {
  invRp(ri);
  xs[rs[ri].x].erase(ri);
  rs[ri].x++;
  xs[rs[ri].x].insert(ri);
  for (auto j : touches[ri][d4s::LEFT]) {
    if (j == si)continue;
    touches[j][d4s::RIGHT].erase(ri);
  }
  touches[ri][d4s::LEFT].clear();
  for (auto j : x2s[rs[ri].x]) {
    if (rs[j].y2 == rs[ri].y) {
      touches[ri][d4s::UP].erase(j);
      touches[j][d4s::DOWN].erase(ri);
    }
    else if (rs[ri].y2 == rs[j].y) {
      touches[ri][d4s::DOWN].erase(j);
      touches[j][d4s::UP].erase(ri);
    }
  }
}
void decreUp(int ri, int si) {
  invRp(ri);
  ys[rs[ri].y].erase(ri);
  rs[ri].y++;
  ys[rs[ri].y].insert(ri);
  for (auto j : touches[ri][d4s::UP]) {
    if (j == si)continue;
    touches[j][d4s::DOWN].erase(ri);
  }
  touches[ri][d4s::UP].clear();
  for (auto j : y2s[rs[ri].y]) {
    if (rs[j].x2 == rs[ri].x) {
      touches[ri][d4s::LEFT].erase(j);
      touches[j][d4s::RIGHT].erase(ri);
    }
    else if (rs[ri].x2 == rs[j].x) {
      touches[ri][d4s::RIGHT].erase(j);
      touches[j][d4s::LEFT].erase(ri);
    }
  }
}
void decreRight(int ri, int si) {
  invRp(ri);
  x2s[rs[ri].x2].erase(ri);
  rs[ri].x2--;
  x2s[rs[ri].x2].insert(ri);
  for (auto j : touches[ri][d4s::RIGHT]) {
    if (j == si)continue;
    touches[j][d4s::LEFT].erase(ri);
  }
  touches[ri][d4s::RIGHT].clear();
  for (auto j : xs[rs[ri].x2]) {
    if (rs[j].y2 == rs[ri].y) {
      touches[ri][d4s::UP].erase(j);
      touches[j][d4s::DOWN].erase(ri);
    }
    else if (rs[ri].y2 == rs[j].y) {
      touches[ri][d4s::DOWN].erase(j);
      touches[j][d4s::UP].erase(ri);
    }
  }
}
void decreDown(int ri, int si) {
  invRp(ri);
  y2s[rs[ri].y2].erase(ri);
  rs[ri].y2--;
  y2s[rs[ri].y2].insert(ri);
  for (auto j : touches[ri][d4s::DOWN]) {
    if (j == si)continue;
    touches[j][d4s::UP].erase(ri);
  }
  touches[ri][d4s::DOWN].clear();
  for (auto j : ys[rs[ri].y2]) {
    if (rs[j].x2 == rs[ri].x) {
      touches[ri][d4s::LEFT].erase(j);
      touches[j][d4s::RIGHT].erase(ri);
    }
    else if (rs[ri].x2 == rs[j].x) {
      touches[ri][d4s::RIGHT].erase(j);
      touches[j][d4s::LEFT].erase(ri);
    }
  }
}

// 四角を指定方向に伸縮する
constexpr void inflate(Rect& rc, int dr, int am) {
  switch (dr) {
  case d4s::LEFT:
    rc.x -= am;
    break;
  case d4s::UP:
    rc.y -= am;
    break;
  case d4s::RIGHT:
    rc.x2 += am;
    break;
  case d4s::DOWN:
    rc.y2 += am;
    break;
  }
}

// 一つの四角を、要求サイズを超えず他とぶつからない範囲で可能な限り広げる
void inflateMax(int idx) {
  auto& dm = ds[idx];
  auto& rc = rs[idx];
  vi ds(4); iota(rng(ds), 0);
  const int am = 4;
  while(sz(ds)) {
    int i = xor128()%sz(ds);
    int d = ds[i];
    bool keep = true;

    switch (d) {
    case d4s::LEFT:
    case d4s::RIGHT:
      if ((rc.w() + am) * rc.h() > dm.r) keep = false;
      break;
    case d4s::UP:
    case d4s::DOWN:
      if ((rc.h() + am) * rc.w() > dm.r) keep = false;
      break;
      break;
    }
    if (keep) {
      int s = sideSpace(idx, d);
      if (keep = s > 0) {
        inflate(rc, d, min(s, am));
      }
    }

    if (!keep) {
      ds.erase(ds.begin() + i);
    }
  }
}
// ランダム性をかなり強くしたinflateMax. 速いが良い解でない可能性が高まる
void inflateMaxR(int idx) {
  auto& dm = ds[idx];
  auto& rc = rs[idx];
  vi ds(4); iota(rng(ds), 0);
  while (sz(ds)) {
    int i = xor128() % sz(ds);
    int d = ds[i];
    int am=0;

    switch (d) {
    case d4s::LEFT:
    case d4s::RIGHT:
      am = dm.r / rc.h() - rc.w();
      break;
    case d4s::UP:
    case d4s::DOWN:
      am = dm.r / rc.w() - rc.h();
      break;
      break;
    }
    bool keep = am > 0;
    if (keep) {
      int s = sideSpace(idx, d);
      if (keep = s > 0) {
        chmin(am, s);
        inflate(rc, d, 1 + xor128() % am);
      }
    }

    if (!keep) {
      ds.erase(ds.begin() + i);
    }
  }
}
void inflateMaxSR(int idx) {
  auto& dm = ds[idx];
  auto& rc = rs[idx];
  vi ds(4); iota(rng(ds), 0);
  while (sz(ds)) {
    int i = xor128() % sz(ds);
    int d = ds[i];
    int am = 0;

    switch (d) {
    case d4s::LEFT:
    case d4s::RIGHT:
      am = dm.r / rc.h() - rc.w();
      break;
    case d4s::UP:
    case d4s::DOWN:
      am = dm.r / rc.w() - rc.h();
      break;
      break;
    }
    bool keep = am > 0;
    if (keep) {
      int s = sideSpace(idx, d);
      if (keep = s > 0) {
        chmin(am, s);
        inflate(rc, d, am);
      }
    }

    ds.erase(ds.begin() + i);
  }
}
// ver2. 予めinitTouches()が必要。増分1のときは無印より速いが、無印を増分2以上にすると負ける
void inflateMax2(int idx) {
  auto& dm = ds[idx];
  auto& rc = rs[idx];
  vi ds(4); iota(rng(ds), 0);
  while (sz(ds)) {
    F0R(di, sz(ds)) {

      int d = ds[di];
      bool ok = !sz(touches[idx][d]);
      if (ok) {
        switch (d) {
        case d4s::LEFT:
          if ((rc.w() + 1) * rc.h() <= dm.r && rc.x > XMIN) {
            increLeft(idx);
          }
          else {
            ok = false;
          }
          break;
        case d4s::UP:
          if (rc.w() * (rc.h() + 1) <= dm.r && rc.y > YMIN) {
            increUp(idx);
          }
          else {
            ok = false;
          }
          break;
        case d4s::RIGHT:
          if ((rc.w() + 1) * rc.h() <= dm.r && rc.x2 < XMAX) {
            increRight(idx);
          }
          else {
            ok = false;
          }
          break;
        case d4s::DOWN:
          if (rc.w() * (rc.h() + 1) <= dm.r && rc.y2 < YMAX) {
            increDown(idx);
          }
          else {
            ok = false;
          }
          break;
        }
      }
      if (!ok) {
        ds.erase(ds.begin() + di);
        di--;
      }
    }
  }
}
// 要求点を中心とし、要求サイズを超えない範囲で最大の正方形にする
void initSquare(int idx) {
  auto& dm = ds[idx];
  auto& rc = rs[idx];
  int size = min(min(dm.x - XMIN, dm.y - YMIN), min(XMAX - dm.x - 1, YMAX - dm.y - 1));
  chmin(size, (int)(sqrt(dm.r)-1)/2);
  F0R(i, N) {
    if (i == idx) continue;
    int s = 0;
    if (rs[i].x > dm.x) chmax(s, rs[i].x - dm.x - 1);
    if (rs[i].y > dm.y) chmax(s, rs[i].y - dm.y - 1);
    if (rs[i].x2 <= dm.x) chmax(s, dm.x - rs[i].x2);
    if (rs[i].y2 <= dm.y) chmax(s, dm.y - rs[i].y2);
    chmin(size, s);
  }
  rc = { dm.x - size,dm.y - size,dm.x + 1 + size,dm.y + 1 + size };
}

void inflateAll(int loop) {
  if (loop == 0) {
    F0R(i, N) {
      initSquare(dsSorted[i]);
      inflateMax(dsSorted[i]);
    }
  }
  else if (loop == 1) {
    RF0(i, N) {
      initSquare(dsSorted[i]);
      inflateMax(dsSorted[i]);
    }
  }
  else {
    //initTouches();
    vi as(N); iota(rng(as), 0);
    while (sz(as)) {
      int i = xor128() % sz(as);
      initSquare(as[i]);
      inflateMax(as[i]);
      as.erase(as.begin() + i);
    }
  }
}

// 評価
db eval(const v(Rect)& rs) {
  // 点を含まない場合0点だが、現在は必ず点を含むデータを渡されると仮定し、その判定はしない
  db score = 0;
  F0R(i, N) {
    score += rs[i].eval(ds[i]);
  }
  return score;
}

bool forcePushHappened = false;
void simForce(int idx) {
  auto& dm = ds[idx];
  auto& rc = rs[idx];
  
  F0R(d, 4) {
    auto force = dirHrz(d) ? hforce(idx):vforce(idx);
    db reforce = 0;
    for (auto j : touches[idx][d]) {
      reforce += reForce(j, invDir(d));
    }
    if (force > reforce) {
      // 押し出す
      switch (d) {
      case d4s::LEFT:
        if (rc.x > XMIN) {
          forcePushHappened = true;
          for (auto j : touches[idx][d]) {
            decreRight(j,idx);
          }
          increLeft(idx);
          d--; continue;
        }
        break;
      case d4s::UP:
        if (rc.y > YMIN) {
          forcePushHappened = true;
          for (auto j : touches[idx][d]) {
            decreDown(j, idx);
          }
          increUp(idx);
          d--; continue;
        }
        break;
      case d4s::RIGHT:
        if (rc.x2 < XMAX) {
          forcePushHappened = true;
          for (auto j : touches[idx][d]) {
            decreLeft(j, idx);
          }
          increRight(idx);
          d--; continue;
        }
        break;
      case d4s::DOWN:
        if (rc.y2 < YMAX) {
          forcePushHappened = true;
          for (auto j : touches[idx][d]) {
            decreUp(j, idx);
          }
          increDown(idx);
          d--; continue;
        }
        break;
      }
    }
  }
}

// 内部保持用のスコアから、実際にコンテストで扱われるスコアに変換する
ll cnvScore(db score) {
  return (ll)(score / N * 1'000'000'000 + 0.5);
}

// 押し合いっこが落ち着くまで待つ
db waitForPushing() {
  //int loopCount = 0;
  //while (timer.within()) {
  rpInit();
  while(true) {
    //loopCount++;

    forcePushHappened = false;
    F0R(i, N){
      simForce(i);
    }
    if (!forcePushHappened) {
      break;
    }
  }
  //cerr << "push loop:" << loopCount << newl;
  return eval(rs);
}

db generateNew(int loopCount) {
  //Timer tm;

  //cerr << "Generating new... ------------------" << newl;
  init(rs);

  //tm.start();
  inflateAll(loopCount);
  //cerr << "Inflated. score:"<<cnvScore(eval(rs))<<" " << tm.elapsed() << "ms" << newl;
  //return eval(rs);
  
  //tm.start();
  initTouches();
  //cerr << tm.elapsed() << "ms" << newl;

  //tm.start();
  auto highScore = waitForPushing();
  //cerr << "Pushed. score:"<<cnvScore(highScore)<<" " << tm.elapsed() << "ms" << newl;
  
  return highScore;
}

void input() {
  cin >> N;
  ds.resize(N);
  F0R(i, N) {
    cin >> ds[i].x >> ds[i].y >> ds[i].r;
  }
}

void loadBest() {
  rs.assign(rng(finals));
}

// 次のサンプルを採用するか？
bool adoptNext(db currentScore, db nextScore, int d) {
  if (nextScore >= currentScore) return true;

  // 山登り法
  //return false;

  const db endTemp = 0.0;
  db startTemp = 0.015 * d;
  db temp = startTemp + (endTemp - startTemp) * timer.elapsed() / 3200.0;
  if (temp <= 0) return false;
  db prob = exp((nextScore - currentScore) / temp);
  //cout << cnvScore(currentScore) << "->" << cnvScore(nextScore) << " " << (prob * 100) << "%" << newl;
  return prob > xor128d();
}

// 特定の箱と交差する箱をすべて取得
vi rectsHitRect(const Rect& cr) {
  vi is;
  F0R(i, N) {
    if (cr.hit(rs[i])) {
      is.push_back(i);
    }
  }
  return is;
}
// ランダムにN個の箱を取得
vi rectsRandom(int cnt) {
  vi as(N); iota(rng(as), 0);
  vi is(cnt);
  F0R(i, cnt) {
    int a = xor128() % sz(as);
    is[i] = as[a];
    as.erase(as.begin()+a);
  }
  return is;
}
// 特定の箱と、それに接触している全ての箱を取得
vi rectsTouch(int ri) {
  vi is = { ri };
  F0R(d, 4) {
    for (auto j: touches[ri][d]) {
      is.push_back(j);
    }
  }
  return is;
}
set<int> rectsTouchS(int ri) {
  set<int> is; is.insert(ri);
  F0R(d, 4) {
    for (auto j : touches[ri][d]) {
      is.insert(j);
    }
  }
  return is;
}
vvi touches2V() {
  vvi ts;
  F0R(i, N) {
    ts.resize(sz(ts) + 1);
    ts.back().push_back(i);
    F0R(d, 4) {
      for (auto j : touches[i][d]) {
        ts.back().push_back(j);
      }
    }
    if (sz(ts.back()) <= 1) {
      ts.resize(sz(ts) - 1);
    }
  }
  return ts;
}

// 適当に箱のサイズをリセットして、再度計算し直すことにより点数を増やせないか試す
void replaceRects() {
  timer.setLimit(ms2);

  //int good = 0, bad = 0;

  int loop = 0;
  loadBest();
  initTouches();
  //auto ts = touches2V();
  while (timer.within()) {
    loop++;

    vi is = rectsTouch(loop % N);
    if (sz(is) <= 1)continue;
    int szis = sz(is);
    
    //is.assign(rng(ts[loop % sz(ts)]));

    F0R(i, sz(is)) {
      init(is[i]);
    }

    while(sz(is)) {
      int a = xor128() % (sz(is));
      int i = is[a];
      //initSquare(i);
      inflateMaxR(i);
      is.erase(is.begin() + a);
    }
    initTouches();
    auto score = waitForPushing();
    //score > bestScore ? good++ : bad++;
    score > bestScore ? goodCnt++ : badCnt++;
    if (adoptNext(bestScore, score, szis)) {
    //if (score>bestScore) {
      bestScore = score;
      finals.assign(rng(rs));
      //ts = touches2V();
    }
    else {
    //else if(score<bestScore) {
      loadBest();
      // 本当はここでinitTouches()しないと次loopでの接触箱の取得が正しくできないが
      // しない方が速いし割となんとかなるからしない
      //initTouches();
    }
  }
  //cerr << "replaceRects() finished. score:" << cnvScore(bestScore) << newl;
  //cerr << "Good:" << good << " Bad:" << bad << " "<<((db)good/(good+bad)*100)<<"%" << newl;
}

void solve() {
  timer.start();
  //timer.setLimit(4800);

  bestScore = 0;

  touches.resize(N);
  dsSorted.resize(N); iota(rng(dsSorted), 0);
  sort(rng(dsSorted), [](int a, int b){
    return ds[a].r < ds[b].r;
  });
  F0R(i, N) { touches[i].resize(4); }
  xs.resize(XMAX-XMIN+1);
  x2s.resize(XMAX-XMIN+1);
  ys.resize(YMAX-YMIN+1);
  y2s.resize(YMAX-YMIN+1);
  rcPrmValids.resize(N);
  rcPrms.resize(N);
  F0R(i, N) rcPrms[i].resize(8);

  int loopCount = 0;
  //do {  // 最低1回は行う
  F0R(i, 3) {
    loopCount++;
    auto score = generateNew(loopCount);

    //cerr << "---loop" << loopCount << " score:" << cnvScore(score);
    if (bestScore < score) {
      bestScore = score;
      swap(rs, finals);
      //cerr << " (new high score!)";
    }
    //cerr << newl;
    //break;

  }
  //} while (timer.now() < timer.startTime() + milliseconds(ms1));

  //cerr << "final score:" << cnvScore(bestScore) << newl;
  //cerr << newl;

  replaceRects();
}

v(Dem) genInput() {
  int N = 50 * pow(4, rnd.GetDouble()) + 0.5;
  v(Dem) ds(N);
  {
    int di = 0;
    set<int> ps;
    while (di < N) {
      int x = rnd.GetInt(10000), y = rnd.GetInt(10000);
      int n = y * 10000 + x;
      if (ps.find(n) == ps.end()) {
        ps.insert(n);
        ds[di].x = x;
        ds[di].y = y;
        di++;
      }
    }
    set<int> qs;
    while (sz(qs) < N - 1) {
      int q = 1 + rnd.GetInt(99999999);
      qs.insert(q);
    }
    qs.insert(100'000'000);

    int ri = 0, p = 0;
    for (auto q : qs) {
      ds[ri++].r = q - p;
      p = q;
    }
  }
  return ds;
}
vv(Dem) genInput(int cnt) {
  vv(Dem) vvd(cnt);
  F0R(i, cnt) {
    vvd[i] = genInput();
  }
  return vvd;
}
void printInput(v(Dem)& ds) {
  int N = sz(ds);
  cout << N << newl;
  F0R(i, N) {
    auto& dm = ds[i];
    cout << dm.x << ' ' << dm.y << ' ' << dm.r << newl;
  }
}

// INSERT ABOVE HERE
signed main() {
  cin.tie(0);
  ios_base::sync_with_stdio(false);

#if 0
  // Random test
  int T = 50;
  vl scores(T);
  rnd.setSeed(0);
  auto vvd = genInput(T);
  F0R(i, 3) {

    F0R(i, T) {
      N = sz(vvd[i]);
      ds = vvd[i];
      cerr << "Case " << (i+1) <<"/"<<T<< " ..." << newl;
      solve();
      scores[i] = (cnvScore(eval(finals)));
    }
    cerr << newl;
    cerr << "Test cases:" << T << newl;
    //cerr << "Scores:" << newl;
    ll sum = 0;
    F0R(i, T) {
      //cout << scores[i] << newl;
      sum += scores[i];
    }
    //cerr << newl;
    cerr << "Total: " << sum;

    // some debug outputs
    cerr << " RR count:" << (goodCnt + badCnt);
    //show(goodCnt);
    //show(badCnt);
    goodCnt = badCnt = 0;
    cerr << newl;
  }
#else
  // Normal
  input();
  solve();
  output(finals);
#endif
}
#endif
