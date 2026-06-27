#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2")
#define REP(i, N) for (int i = 0; i < (int)N; i++)
#define FOR(i, a, b) for (int i = a; i < (int)b; i++)
#define ALL(v) v.begin(), v.end()

using namespace std;
constexpr int inf = 1 << 27;
using ll = long long;
using Pii = pair<int, int>;
constexpr int dy[4] = {-1, 0, 1, 0}, dx[4] = {0, -1, 0, 1};

namespace Lib {
struct Timer {
  static constexpr double ONE_CLOCK = 0.000000333;
  unsigned long long get_cycle() {
    unsigned int low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
  }
  unsigned long long start_time, last_time;
  Timer() { start_time = last_time = get_cycle(); }
  int get_time() { return get_cycle() * ONE_CLOCK - start_time * ONE_CLOCK; }
  int get_us() {
    return (get_cycle() * ONE_CLOCK - start_time * ONE_CLOCK) * 1000;
  }
  int split_us() {
    int ret = (get_cycle() * ONE_CLOCK - last_time * ONE_CLOCK) * 1000;
    last_time = get_cycle();
    return ret;
  }
};

class RandInt {
 public:
  int x = 8753, y = 239017, z = 1000000123;
  int next(int d) {
    int t = x ^ (x << 11);
    x = y;
    y = z;
    z ^= (z >> 19) ^ t ^ (t >> 8);
    return z % d;
  }
};

template <class T>
bool chmax(T &a, const T &b) {
  if (a < b) {
    a = b;
    return 1;
  }
  return 0;
}
template <class T>
bool chmin(T &a, const T &b) {
  if (b < a) {
    a = b;
    return 1;
  }
  return 0;
}

template <class TP, size_t SZ = 40000>
struct FQueue {
  TP arr[SZ];
  int head = 0, tail = 0;
  int size() { return head < tail ? tail - head : (tail + SZ - head); }
  void push(TP p) {
    arr[tail++] = p;
    if (tail == SZ) tail = 0;
  }
  void clear() { head = tail = 0; }
  TP front() { return arr[head]; }
  bool empty() { return head == tail; }
  TP pop() {
    TP ret = arr[head++];
    if (head == SZ) head = 0;
    return ret;
  }
};

vector<double> solve(vector<vector<double>> A, vector<double> b) {
  int n = A.size();
  vector<double> ret(n);
  for (int t = 0; t < 100; ++t) {
    REP(i, n) {
      ret[i] = b[i];
      REP(j, n) if (i != j) ret[i] -= A[i][j] * ret[j];
      if (A[i][i] == 0) {
        exit(1);
        return vector<double>();
      }
      ret[i] /= A[i][i];
    }
  }
  return ret;
}
vector<double> regression(vector<vector<double>> X, vector<double> Y) {
  for (auto &v : X) v.push_back(1);
  vector<vector<double>> A(X[0].size(), vector<double>(X[0].size()));
  REP(i, X[0].size())
  REP(j, X[0].size()) REP(k, X.size()) A[i][j] += X[k][i] * X[k][j];
  vector<double> B(X[0].size());
  REP(i, X[0].size()) REP(j, X.size()) B[i] += X[j][i] * Y[j];
  for (int i = X[0].size() - 1; i >= 0; --i) {
    if (A[i][i] == 0) {
      A.erase(A.begin() + i);
      for (auto &v : A) v.erase(v.begin() + i);
      B.erase(B.begin() + i);
    }
  }
  return solve(A, B);
}
}  // namespace Lib
Lib::RandInt ri;
Lib::Timer timer;
using Lib::chmax;
using Lib::chmin;

template <class T>
string to_string(T s);
template <class S, class T>
string to_string(pair<S, T> p);
string to_string(char c) { return string(1, c); }
string to_string(string s) { return s; }
string to_string(const char s[]) { return string(s); }

template <class T>
string to_string(T v) {
  if (v.empty()) return "{}";
  string ret = "{";
  for (auto x : v) ret += to_string(x) + ",";
  ret.back() = '}';
  return ret;
}

template <class S, class T>
string to_string(pair<S, T> p) {
  return "{" + to_string(p.first) + ":" + to_string(p.second) + "}";
}
void debug() { cerr << endl; }

template <class Head, class... Tail>
void debug(Head head, Tail... tail) {
  cerr << to_string(head) << " ";
  debug(tail...);
}

constexpr int N = 200, B = 25, BL = 1, U = 8, MX = 1 << 16;
constexpr int LMX = 1 << 10;

struct Point {
  int id;
  Point() {}
  Point(int _y, int _x) : id(((_y + B) << 8) | (_x + B)) {}
  Point(int _id) : id(_id) {}
  operator int() const { return id; }
  int y() const { return (id >> 8) - B; }
  int x() const { return (id & ((1 << 8) - 1)) - B; }
  Point mid() { return Point(y() / U * U + U / 2, x() / U * U + U / 2); }
  Point shr() { return Point(y() / U, x() / U); }
  Point wid() { return Point(y() * U + U / 2, x() * U + U / 2); }
  double euclid(Point p) {
    return sqrt((y() - p.y()) * (y() - p.y()) + (x() - p.x()) * (x() - p.x()));
  }
  int manhattan(Point p) { return abs(y() - p.y()) + abs(x() - p.x()); }
  bool valid() {
    int _y = y(), _x = x();
    return 0 <= _y && _y < N && 0 <= _x && _x < N;
  }
  Point operator+(Point &p) { return Point(y() + p.y(), x() + p.x()); }
  bool operator!=(Point &p) const { return id != p.id; }
};

struct PointL {
  int id;
  PointL() {}
  PointL(int _y, int _x) : id(((_y + BL) << 5) | (_x + BL)) {}
  PointL(int _id) : id(_id) {}
  PointL(Point p) { *this = PointL(p.y() / U, p.x() / U); }
  operator int() const { return id; }
  int y() const { return (id >> 5) - BL; }
  int x() const { return (id & ((1 << 5) - 1)) - BL; }
  double euclid(PointL p) {
    return sqrt((y() - p.y()) * (y() - p.y()) + (x() - p.x()) * (x() - p.x()));
  }
  Point wid() { return Point(y() * U + U / 2, x() * U + U / 2); }
  bool valid() {
    int _y = y(), _x = x();
    return 0 <= _y && _y < N / U && 0 <= _x && _x < N / U;
  }
  Point operator+(Point &p) { return Point(y() + p.y(), x() + p.x()); }
  bool operator!=(Point &p) const { return id != p.id; }
};

vector<Point> near[25];
int W, K, M, C, X;
int S[MX], SL[LMX], lower[MX], upper[MX], damage[MX], min_info[MX];

Point water[4], house[10], prep[14];
PointL waterl[4], housel[10], prepl[14];

int broken[MX], dist[14][MX], from[14][MX], distl[14][LMX];
bool inq[MX];
Point nexts[MX][4];
pair<int, PointL> edges[LMX][4];
vector<Point> allp;
vector<PointL> allpl;
Point widen[MX];

int local = 0;

struct Judge {
  int total = 0;
  int S[MX];
  int ex(int y, int x, int P) {
    total += P + C;
    cout << y << " " << x << " " << P << endl;
    int r;
    if (local) {
      S[Point(y, x)] -= P;
      if (S[Point(y, x)] <= 0) {
        r = 1;
      } else {
        r = -1;
      }
    } else {
      cin >> r;
    }
    return r;
  }
} judge;

#include <cstring>

void set_dist(int i) {
  Lib::FQueue<Point> q;
  auto &ds = dist[i];
  auto &fs = from[i];
  Point s = prep[i];
  fill(ds, ds + (MX), inf);
  fill(fs, fs + (MX), inf);
  fill(inq, inq + (MX), 0);
  ds[s] = 0;
  inq[s] = 1;
  q.push(s);
  while (!q.empty()) {
    auto u = q.pop();
    int d = ds[u];
    inq[u] = 0;
    REP(dir, 4) {
      auto v = nexts[u][dir];
      int nd = v ? d + (broken[v] ? 0 : S[v] + min(150, C * 3)) : inf;
      if (nd < ds[v]) {
        ds[v] = nd;
        fs[v] = u;
        if (!inq[v]) {
          inq[v] = 1;
          q.push(v);
        }
      }
    }
  }
}

void set_dist_large() {
  Lib::FQueue<PointL> q;
  for (auto u : allpl) {
    REP(dir, 4) {
      auto v = edges[u][dir].second;
      edges[u][dir].first = v ? ((SL[u] + SL[v]) / 2 + min(150, C * 3)) *
                                    (widen[u].manhattan(widen[v]) -
                                     broken[widen[u]] - broken[widen[v]])
                              : inf;
    }
  }
  REP(i, M) {
    auto &ds = distl[i];
    PointL s = prepl[i];
    fill(ds, ds + (LMX), inf);
    fill(inq, inq + (LMX), 0);
    ds[s] = 0;
    inq[s] = 1;
    q.push(s);
    while (!q.empty()) {
      auto u = q.pop();
      int d = ds[u];
      inq[u] = 0;
      REP(dir, 4) {
        auto v = edges[u][dir].second;
        int nd = d + edges[u][dir].first;
        if (nd < ds[v]) {
          ds[v] = nd;
          if (!inq[v]) {
            inq[v] = 1;
            q.push(v);
          }
        }
      }
    }
  }
}

int query(Point p, int P) {
  if (broken[p]) return 1;
  int r = judge.ex(p.y(), p.x(), P);
  if (damage[P] == 0) {
    for (auto ne : near[24]) {
      auto q = p + ne;
      if (q) chmin(min_info[q], (int)p.euclid(q));
    }
  }
  damage[p] += P;
  if (r == 2) exit(0);
  if (r == 1) {
    broken[p] = 1;
    lower[p] = max(10, damage[p] - P);
    upper[p] = damage[p];
    cout << "#" << lower[p] << " " << upper[p] << endl;
    return 1;
  } else {
    lower[p] = damage[p] + P;
    return 0;
  }
}

void invs(Point p) {
  int P = damage[p] < 100 ? min(50, C * 3 + 10) : min(120, C * 3 + 10);
  query(p, P);
}

void exc(Point p) {
  if (broken[p]) return;
  vector<vector<double>> X;
  vector<double> Y;

  for (auto ne : near[8]) {
    auto n = p + ne;
    if (n.valid() && broken[n]) {
      double y = n.y() - p.y(), x = n.x() - p.x();
      X.push_back({y, x, y * y, x * x, y * x});
      Y.push_back((lower[n] + upper[n]) / 2);
    }
  }
  if (Y.size() == 0) {
    S[p] = 30;
  } else if (Y.size() < X[0].size()) {
    S[p] = Y[0];
  } else {
    auto b = Lib::regression(X, Y);
    if (!b.empty()) {
      S[p] = min(5000.0, max(10.0, b.back()));
      cout << "#predict " << p.y() << " " << p.x() << endl;
      cout << "#predict " << S[p] << endl;
      cout << "# " << to_string(b) << endl;
      cout << "# " << to_string(X) << endl;
      cout << "# " << to_string(Y) << endl;
    }
  }
  while (!broken[p]) {
    int P = damage[p] ? min(40, C + 10) : max(min(40, C + 10), S[p] - 10);
    query(p, P);
  }
}

void exc(int fr, Point to) {
  exc(to);
  while (prep[fr] != to) exc(to = from[fr][to]);
}

struct State {
  Point hub[9];
  PointL hubl[9];
  int score;
  Point &get_point(int i) {
    if (i < M)
      return prep[i];
    else
      return hub[i - M];
  }
  PointL &get_pointl(int i) {
    if (i < M)
      return prepl[i];
    else
      return hubl[i - M];
  }
  State() {
    REP(i, K - 1) {
      hub[i] = Point(ri.next(N), ri.next(N)).mid();
      hubl[i] = PointL(ri.next(N / U), ri.next(N / U));
    }
    score = 0;
  }
  void set_score(int lim = inf) {
    int used[23] = {};
    int cost[23];
    score = 0;
    REP(i, X) cost[i] = i < W ? 0 : inf;
    while (1) {
      int v = -1;
      int minc = inf;
      REP(u, X) {
        if (!used[u] && cost[u] + used[u] * inf < minc) {
          v = u;
          minc = cost[u];
        }
      }
      if (v == -1) break;
      used[v] = 1;
      score += minc;
      if (score > lim) break;
      REP(u, X) {
        if (u < M) {
          cost[u] = min(cost[u], dist[u][v < M ? prep[v] : hub[v - M]]);
        } else if (v < M) {
          cost[u] = min(cost[u], dist[v][hub[u - M]]);
        }
      }
    }
  }
  void set_score_large(int lim = inf) {
    int used[23] = {};
    int cost[23];
    score = 0;
    REP(i, X) cost[i] = i < W ? 0 : inf;
    while (1) {
      int v = -1, minc = inf;
      REP(u, X) {
        if (!used[u] && cost[u] + used[u] * inf < minc) {
          v = u;
          minc = cost[u];
        }
      }
      if (v == -1) break;
      used[v] = 1;
      score += minc;
      if (score > lim) break;
      REP(u, X) {
        if (u < M) {
          cost[u] = min(cost[u], distl[u][v < M ? prepl[v] : hubl[v - M]]);
        } else if (v < M) {
          cost[u] = min(cost[u], distl[v][hubl[u - M]]);
        }
      }
    }
  }
  vector<Point> enum_route_btw(int s, Point t) {
    vector<Point> ret;
    ret.push_back(t);
    while (get_point(s) != t) {
      t = from[s][t];
      ret.push_back(t);
    }
    return ret;
  }
  vector<pair<int, Point>> enum_route() {
    int used[23] = {};
    int cost[23];
    int fr[23];
    score = 0;
    REP(i, X) {
      cost[i] = i < W ? 0 : inf;
      fr[i] = i;
    }
    vector<pair<int, Point>> rett;
    while (1) {
      int v = -1;
      REP(u, X) {
        if (!used[u] && (v == -1 || cost[u] < cost[v])) v = u;
      }
      if (v == -1) break;
      used[v] = 1;
      score += cost[v];
      if (get_point(v) != get_point(fr[v])) {
        int s = v < M ? v : fr[v];
        Point t = get_point(v < M ? fr[v] : v);
        rett.push_back(make_pair(s, t));
      }
      REP(u, X) {
        if (u < M) {
          if (chmin(cost[u], dist[u][get_point(v)])) {
            fr[u] = v;
          }
        } else if (v < M) {
          if (chmin(cost[u], dist[v][get_point(u)])) {
            fr[u] = v;
          }
        }
      }
    }
    return rett;
  }
  vector<PointL> enum_route_large() {
    int used[23] = {};
    int cost[23];
    int fr[23];
    score = 0;
    REP(i, X) {
      cost[i] = i < W ? 0 : inf;
      fr[i] = i;
    }
    vector<PointL> ret;
    while (1) {
      int v = -1, minc = inf;
      REP(u, X) {
        if (!used[u] && cost[u] + used[u] * inf < minc) {
          v = u;
          minc = cost[u];
        }
      }
      if (v == -1) break;
      used[v] = 1;
      score += minc;
      if (get_pointl(v) != get_pointl(fr[v])) {
        int s = v < M ? v : fr[v];
        PointL t = get_pointl(v < M ? fr[v] : v);
        ret.push_back(t);
        while (get_pointl(s) != t) {
          int best = inf;
          PointL nxt;
          REP(dir, 4) {
            auto n = edges[t][dir].second;
            if (distl[s][n] + edges[t][dir].first == distl[s][t]) {
              int d = get_pointl(s).euclid(n);
              if (d < best) {
                best = d;
                nxt = n;
              }
            }
          }
          t = nxt;
          ret.push_back(nxt);
        }
      }
      REP(u, X) {
        if (u < M) {
          if (chmin(cost[u], distl[u][get_pointl(v)])) {
            fr[u] = v;
          }
        } else if (v < M) {
          if (chmin(cost[u], distl[v][get_pointl(u)])) {
            fr[u] = v;
          }
        }
      }
    }
    return ret;
  }
  void hill_climb() {
    set_score();
    int LP = 100000;
    if (K > 1) REP(t, LP) {
        int tp = ri.next(4);
        if (tp == -1) {
          int i = ri.next(K - 1);
          int j = ri.next(M);
          int prev = score;
          Point pos = hub[i];
          Point tar = prep[j];
          hub[i] = tar + near[5][ri.next(near[5].size())];
          while (!hub[i].valid()) {
            hub[i] = tar + near[5][ri.next(near[5].size())];
          }
          set_score(prev);
          if (score > prev) {
            hub[i] = pos;
            score = prev;
          }
        } else {
          int i = ri.next(K - 1);

          int prev = score;
          Point pos = hub[i];
          hub[i] = pos + near[5][ri.next(near[5].size())];
          while (!hub[i].valid()) {
            hub[i] = pos + near[5][ri.next(near[5].size())];
          }
          set_score(prev);
          if (score > prev) {
            hub[i] = pos;
            score = prev;
          }
        }
        // if (t % 10000 == 0) debug(t, score);
      }
  }
  void hill_climb_large() {
    set_score_large();
    if (K > 1)
      while (1) {
        int prev = score;
        int best = prev;
        pair<int, PointL> tr;
        REP(i, K - 1) {
          PointL pos = hubl[i];
          REP(dir, 4) {
            auto p = edges[pos][dir].second;
            if (!p) continue;
            hubl[i] = p;
            set_score_large(best);
            if (score < best) {
              best = score;
              tr = pair<int, PointL>(i, p);
            }
          }
          hubl[i] = pos;
        }
        if (1) {
          REP(i, K - 1) {
            PointL pos = hubl[i];
            REP(j, M) {
              auto p = prepl[j];
              hubl[i] = p;
              set_score_large(best);
              if (score < best) {
                best = score;
                tr = pair<int, PointL>(i, p);
              }
            }
            hubl[i] = pos;
          }
        }
        if (best == prev) {
          score = prev;
          break;
        }
        hubl[tr.first] = tr.second;
        score = best;
      }
  }
};
int base = 150;

void read_problem() {
  REP(i, N) REP(j, N) { allp.push_back(Point(i, j)); }
  REP(i, N / U) REP(j, N / U) allpl.push_back(PointL(i, j));
  int _;
  cin >> _ >> W >> K >> C;
  M = W + K;
  if (local)
    for (auto i : allp) cin >> judge.S[i];
  REP(i, W) {
    int y, x;
    cin >> y >> x;
    prep[i] = water[i] = Point(y, x);
    prepl[i] = waterl[i] = PointL(y / U, x / U);
  }
  REP(i, K) {
    int y, x;
    cin >> y >> x;
    prep[W + i] = house[i] = Point(y, x);
    prepl[W + i] = housel[i] = PointL(y / U, x / U);
  }
  X = M + K - 1;
  for (auto i : allp) {
    lower[i] = 10;
    upper[i] = 5001;
    S[i] = base;
    min_info[i] = inf;
  }
  for (int y = -24; y <= 24; ++y) {
    for (int x = -24; x <= 24; ++x) {
      Point p(y, x);
      int d = Point(0, 0).euclid(p);
      for (int l = d; l <= 24; ++l) {
        near[l].push_back(p);
      }
    }
  }
  REP(l, 25) {
    sort(ALL(near[l]), [](Point l, Point r) {
      return l.euclid(Point(0, 0)) < r.euclid(Point(0, 0));
    });
  }
  for (auto p : allp) {
    REP(dir, 4) {
      int y = p.y() + dy[dir], x = p.x() + dx[dir];
      if (0 <= y && y < N && 0 <= x && x < N) {
        nexts[p][dir] = Point(y, x);
      } else {
        nexts[p][dir] = Point(0);
      }
    }
  }
  REP(i, N / U) {
    REP(j, N / U) {
      PointL p(i, j);
      widen[p] = Point(i * U + U / 2, j * U + U / 2);
      REP(dir, 4) {
        int y = i + dy[dir], x = j + dx[dir];
        if (0 <= y && y < N / U && 0 <= x && x < N / U) {
          edges[p][dir].second = PointL(y, x);
        } else {
          edges[p][dir].second = PointL(0);
        }
      }
    }
  }
  REP(i, M) { widen[prepl[i]] = prep[i]; }
}
vector<int> nears[MX];

int inters = 0;

void interpolate_large() {
  int sm[LMX];
  double ct[LMX];
  for (auto p : allpl) {
    sm[p] = ct[p] = 0;
    SL[p] = base;
  }
  for (auto p : allp) {
    auto mid = p.mid();
    if (damage[p] > 0) {
      int l = 2;
      int prd = broken[p] ? (lower[p] + upper[p]) / 2 : lower[p] * 2 + 150;

      for (int y = max(mid.y() - U * l, U / 2);
           y <= min(mid.y() + U * l, N - 1); y += U) {
        for (int x = max(mid.x() - U * l, U / 2);
             x <= min(mid.x() + U * l, N - 1); x += U) {
          int d = mid.euclid(Point(y, x));
          if (d < U * 1.5) {
            double fact = d == 0 ? 100.0 : 1.0 / d * U;
            PointL q(Point(y, x));
            sm[q] += (prd - base) * fact * (d == 0 ? 1.0 : fact);
            ct[q] += fact;
          }
        }
      }
    }
  }
  for (auto p : allpl)
    if (ct[p] > 0) {
      SL[p] = base + sm[p] / ct[p];
    }

  for (auto p : allp) {
    S[p] = SL[PointL(p)];
  }
}

int main(int argc, char **argv) {
  read_problem();
  REP(i, W) exc(water[i]);
  REP(i, K) exc(house[i]);
  int dists = 0, hill = 0, other = 0;
  State state;
  timer.split_us();
  interpolate_large();
  while (timer.get_time() < 4000) {
    other += timer.split_us();
    set_dist_large();
    dists += timer.split_us();
    state.hill_climb_large();
    hill += timer.split_us();
    auto route = state.enum_route_large();
    int upd = 0;
    int ct = 0;
    while (1) {
      double mn = inf, mx = 0;
      Point nxt(0);
      for (auto p : route) {
        if (!broken[widen[p]] && damage[widen[p]]) {
          if (SL[p] < mn) {
            mn = SL[p];
            nxt = widen[p];
          }
        }
      }
      if (!nxt.valid()) break;
      upd = 1;
      ct++;
      invs(nxt);
      if (!broken[nxt]) {
        break;
      }
    }
    if (!upd)
      while (1) {
        double mx = 0;
        double mn2 = inf;
        Point nxt(0);
        for (auto p : route)
          if (!broken[widen[p]]) {
            double mn = min_info[widen[p]];
            if (mn > mx) {
              mx = mn;
              nxt = widen[p];
            }
          }
        if (!nxt.valid()) {
          break;
        }
        upd = 1;
        ct++;
        invs(nxt);
        if (!broken[nxt]) {
          break;
        }
      }
    if (!upd) {
      break;
    }
    other += timer.split_us();
    interpolate_large();
    inters += timer.split_us();
  }
  debug(timer.get_time());
  int in_route[MX] = {};
  for (auto p : allp) {
    if (broken[p]) in_route[p.mid()] = 1;
  }
  if (1)
    for (int y = U / 2; y < N; y += U) {
      for (int x = U / 2; x < N; x += U) {
        auto p = Point(y, x);
        REP(d1, 4) {
          int d2 = (d1 + 1) % 4;
          auto q1 = Point(y + U * dy[d1], x + U * dx[d1]);
          auto q2 = Point(y + U * dy[d2], x + U * dx[d2]);
          if (q1.valid() && q2.valid()) {
            REP(i, U + 1) {
              REP(j, U + 1) {
                Point u(y + dy[d1] * i + dy[d2] * j,
                        x + dx[d1] * i + dx[d2] * j);
                if (p.manhattan(u) > U) continue;
                S[u] = S[p] + (S[q1] - S[p]) * i / U + (S[q2] - S[p]) * j / U;
              }
            }
          }
        }
      }
    }

  for (auto p : allp) {
    if (!in_route[p.mid()]) S[p] = 50000;
  }
  REP(i, M) set_dist(i);
  state.hill_climb();
  auto route = state.enum_route();
  debug(state.score);

  for (auto &v : route) {
    if (timer.get_time() < 4400) set_dist(v.first);
    auto path = state.enum_route_btw(v.first, v.second);
    for (auto p : path) exc(p);
  }
  debug(judge.total);
  debug(timer.get_time(), dists, hill, inters, other);

  return 0;
}