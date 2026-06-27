#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <complex>
#include <cstdio>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2")
#define REP(i, N) for (int i = 0; i < (int)N; i++)
#define FOR(i, a, b) for (int i = a; i < (int)b; i++)
#define ALL(x) (x).begin(), (x).end()

using namespace std;

constexpr int inf = 1 << 30;
constexpr int dy[4] = {-1, 0, 1, 0}, dx[4] = {0, -1, 0, 1};

using ll = long long;
using Pii = pair<int, int>;

inline int popcount(ll x) { return __builtin_popcountll(x); }
inline int div2num(ll x) { return __builtin_ctzll(x); }
inline bool bit(ll x, int b) { return (x >> b) & 1; }

struct Point {
  int y, x;
  Point() {}
  Point(int _y, int _x) : y(_y), x(_x) {}
};

struct Line {
  Point s, g;
  Line() {}
  Line(Point _s, Point _g) : s(_s), g(_g) {}
};

template <class T>
bool chmax(T& a, const T& b) {
  if (a < b) {
    a = b;
    return 1;
  }
  return 0;
}
template <class T>
bool chmin(T& a, const T& b) {
  if (b < a) {
    a = b;
    return 1;
  }
  return 0;
}

template <class T>
string to_string(T s);
string to_string(Point p) {
  return "{" + to_string(p.y) + "," + to_string(p.x) + "}";
}
string to_string(Line l) {
  return "{" + to_string(l.s) + "," + to_string(l.g) + "}";
}
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
  int next(int l, int r) { return next(r - l) + l; }
} ri;

class Timer {
  static constexpr double ONE_CLOCK = 0.000000333;
  unsigned long long get_cycle() {
    unsigned int low, high;
    // __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
  }

 public:
  unsigned long long start_time;
  Timer() { start_time = get_cycle(); }
  int get_time() { return get_cycle() * ONE_CLOCK - start_time * ONE_CLOCK; }
} timer;

string dirs = "ULDR";

int N, si, sj, M, L;
int mp[70][70], cross[70][70];

vector<Point> crosses;
vector<Line> lines;

vector<Pii> p_to_l[400];
vector<int> include_lines[400];

bool valid(int i, int j) { return 0 <= i && i < N && 0 <= j && j < N; }

struct State {
  vector<int> order;
  int score;
  struct Transition {
    int type, i, j;
    Transition(int _type, int _i, int _j) : type(_type), i(_i), j(_j) {}
  };
  State() {
    order = vector<int>(L);
    iota(ALL(order), 0);
  }
  void set_score() {
    score = 0;
    int now = 0;
    for (int i : order) {
      score += p_to_l[now][i].first;
      now = p_to_l[now][i].second;
    }
    score += p_to_l[now][0].first;
  }
  void greedy() {
    score = 0;
    order.clear();
    set<int> rest;
    REP(i, L) rest.insert(i);
    int now = 0;
    REP(i, L) {
      Pii best(inf, inf);
      for (auto j : rest) {
        chmin(best, Pii(p_to_l[now][j].first, j));
      }
      score += best.first;
      order.push_back(best.second);
      now = p_to_l[now][best.second].second;
      rest.erase(best.second);
    }
    score += p_to_l[now][0].first;
  }
  void print_answer() {
    int now = 0;
    string ans;
    order.push_back(0);
    REP(i, L + 1) {
      Point s = crosses[now], g = crosses[p_to_l[now][order[i]].second];
      if (i == L) g = crosses[0];
      priority_queue<Pii, vector<Pii>, greater<Pii>> q;
      vector<int> dist(N * N, inf), prev(N * N);
      q.push(Pii(0, s.y * N + s.x));
      dist[s.y * N + s.x] = 0;
      while (!q.empty()) {
        auto tp = q.top();
        q.pop();
        int d = tp.first, u = tp.second;
        if (dist[u] < d) continue;
        int y = u / N, x = u % N;
        if (y == g.y && x == g.x) break;
        REP(dir, 4) {
          int ny = y + dy[dir], nx = x + dx[dir];
          if (valid(ny, nx) && mp[ny][nx] != 0) {
            int nd = d + mp[ny][nx];
            if (dist[ny * N + nx] > nd) {
              dist[ny * N + nx] = nd;
              prev[ny * N + nx] = dir;
              q.push(Pii(nd, ny * N + nx));
            }
          }
        }
      }
      string str;
      int y = g.y, x = g.x;
      while (y != s.y || x != s.x) {
        int dir = prev[y * N + x];
        str += dirs[dir];
        y -= dy[dir];
        x -= dx[dir];
      }
      reverse(ALL(str));
      ans += str;
      now = p_to_l[now][order[i]].second;
    }
    cout << ans << endl;
  }
  Transition get_transition() {
    int type = ri.next(3);
    int i = ri.next(L - 1) + 1, j = ri.next(L - 1) + 1;
    while (i == j) j = ri.next(L - 1) + 1;
    if (i > j) swap(i, j);
    return Transition(type, i, j);
  }
  void accept_transition(Transition transition) {
    if (transition.type == 0)
      reverse(order.begin() + transition.i, order.begin() + transition.j + 1);
    else if (transition.type == 1) {
      int t = order[transition.i];

      if (transition.i > transition.j) {
        for (int i = transition.i - 1; i >= transition.j; --i)
          order[i + 1] = order[i];
      } else {
        for (int i = transition.i + 1; i <= transition.j; ++i)
          order[i - 1] = order[i];
      }
      order[transition.j] = t;
    } else {
      swap(order[transition.i], order[transition.j]);
    }
  }
  void cancel_transition(Transition transition) {
    if (transition.type == 0)
      reverse(order.begin() + transition.i, order.begin() + transition.j + 1);
    else if (transition.type == 1) {
      swap(transition.i, transition.j);
      int t = order[transition.i];

      if (transition.i > transition.j) {
        for (int i = transition.i - 1; i >= transition.j; --i)
          order[i + 1] = order[i];
      } else {
        for (int i = transition.i + 1; i <= transition.j; ++i)
          order[i - 1] = order[i];
      }
      order[transition.j] = t;
    } else {
      swap(order[transition.i], order[transition.j]);
    }
  }
};

void read_problem() {
  cin >> N >> si >> sj;
  REP(i, N) {
    string s;
    cin >> s;
    REP(j, N) {
      if (s[j] == '#')
        mp[i][j] = 0;
      else
        mp[i][j] = s[j] - '0';
    }
  }
}

void make_crosses() {
  REP(i, N) REP(j, N) cross[i][j] = -1;
  cross[si][sj] = 0;
  crosses.emplace_back(si, sj);
  REP(i, N) REP(j, N) if (mp[i][j] > 0 && (si != i || sj != j)) {
    int v = 0, h = 0;
    for (int dir : {0, 2}) {
      int i2 = i + dy[dir], j2 = j + dx[dir];
      if (valid(i2, j2) && mp[i2][j2] > 0) v = 1;
    }
    for (int dir : {1, 3}) {
      int i2 = i + dy[dir], j2 = j + dx[dir];
      if (valid(i2, j2) && mp[i2][j2] > 0) h = 1;
    }
    if (v && h) {
      cross[i][j] = crosses.size();
      crosses.emplace_back(i, j);
    }
  }
  M = crosses.size();
}

void make_lines() {
  lines.emplace_back(Point(si, sj), Point(si, sj));
  REP(j, N) {
    vector<Point> points;
    REP(i, N) {
      if (cross[i][j] != -1) points.emplace_back(i, j);
      if (mp[i][j] == 0 && !points.empty()) {
        lines.emplace_back(points.front(), points.back());
        points.clear();
      }
    }
    if (!points.empty()) {
      lines.emplace_back(points.front(), points.back());
      points.clear();
    }
  }
  REP(i, N) {
    vector<Point> points;
    REP(j, N) {
      if (cross[i][j] != -1) points.emplace_back(i, j);
      if (mp[i][j] == 0 && !points.empty()) {
        lines.emplace_back(points.front(), points.back());
        points.clear();
      }
    }
    if (!points.empty()) {
      lines.emplace_back(points.front(), points.back());
      points.clear();
    }
  }
  L = lines.size();
}

void make_include_lines() {
  REP(i, crosses.size()) {
    Point p = crosses[i];
    REP(j, lines.size()) {
      Line l = lines[j];
      int ok = 0;
      if (l.s.x == l.g.x) {
        if (l.s.x == p.x && l.s.y <= p.y && p.y <= l.g.y) {
          ok = 1;
        }
      } else {
        if (l.s.y == p.y && l.s.x <= p.x && p.x <= l.g.x) {
          ok = 1;
        }
      }
      if (ok) {
        include_lines[i].push_back(j);
      }
    }
  }
}

void make_p_to_l() {
  REP(i, M) {
    p_to_l[i] = vector<Pii>(L, Pii(inf, inf));
    priority_queue<Pii, vector<Pii>, greater<Pii>> q;
    vector<int> dist(M, inf);
    q.push(Pii(0, i));
    dist[i] = 0;
    while (!q.empty()) {
      auto tp = q.top();
      q.pop();
      int d = tp.first, u = tp.second;
      if (dist[u] < d) continue;
      int y = crosses[u].y, x = crosses[u].x;
      for (int j : include_lines[u]) {
        chmin(p_to_l[i][j], Pii(d, u));
      }
      REP(dir, 4) {
        int ny = y + dy[dir], nx = x + dx[dir];
        if (!valid(ny, nx) || mp[ny][nx] == 0) continue;
        int nd = d + mp[ny][nx];
        while (valid(ny, nx) && mp[ny][nx] != 0 && cross[ny][nx] == -1) {
          ny += dy[dir];
          nx += dx[dir];
          nd += mp[ny][nx];
        }
        if (valid(ny, nx) && cross[ny][nx] != -1 && dist[cross[ny][nx]] > nd) {
          dist[cross[ny][nx]] = nd;
          q.push(Pii(nd, cross[ny][nx]));
        }
      }
    }
  }
}

int main(int argc, char** argv) {
  read_problem();
  make_crosses();
  make_lines();
  make_include_lines();
  make_p_to_l();
  State state;
  state.greedy();
  state.set_score();

  debug(state.score, 10000 + 10000000 * N / state.score);

  State best_state;
  best_state.score = inf;
  array<int, 3> acc;
  REP(i, 3) acc[i] = 0;
  constexpr int count = 3600000;
  for (int t = 0; t < count; ++t) {
    int prev = state.score;
    auto neigh = state.get_transition();
    state.accept_transition(neigh);
    state.set_score();
    double diff = state.score - prev;
    bool accept = 1;
    if (diff > 0) {
      if (exp(-diff / (1.0 * (count - t) / count) / 2) * 10000 <=
          ri.next(10000))
        accept = 0;
    }
    if (!accept) {
      state.score = prev;
      state.cancel_transition(neigh);
    }
    if (best_state.score > state.score) {
      // acc[neigh.type]++;
      best_state = state;
    }
    // if (t % 10000 == 0) debug(t, best_state.score);
  }
  state = best_state;
  debug(state.score, 10000 + 10000000 * N / state.score);
  state.print_answer();

  return 0;
}