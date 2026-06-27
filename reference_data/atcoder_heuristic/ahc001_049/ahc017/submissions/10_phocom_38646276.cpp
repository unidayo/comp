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
using ll = long long;
using Pii = pair<int, int>;

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
  int split_time() { return get_cycle() * ONE_CLOCK - last_time * ONE_CLOCK; }
  void split() { last_time = get_cycle(); }
} timer;

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
} ri;

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

struct UnionFind {
  vector<int> data;
  UnionFind(int n) : data(n, -1) {}
  int root(int v) { return data[v] < 0 ? v : data[v] = root(data[v]); }
  bool unite(int u, int v) {
    if ((u = root(u)) == (v = root(v))) {
      return 1;
    } else {
      if (-data[u] < -data[v]) swap(u, v);
      data[u] += data[v];
      data[v] = u;
      return 0;
    }
  }
  int size(int v) { return -data[root(v)]; }
};

using Pll = pair<ll, ll>;

struct Edge {
  int u, v, w;
  Edge() {}
  Edge(int _u, int _v, int _w) : u(_u), v(_v), w(_w) {}
  int another(int x) { return u != x ? u : v; }
};
struct Point {
  int x, y;
  Point() {}
  Point(int _x, int _y) : x(_x), y(_y) {}
};
int N, M, D, K;
vector<int> G[1000];
int deg[1000];
Point P[1000];
int euc[1000][1000];
Edge E[6000];
int mind[1000][1000];
int inf = 1 << 29;
ll llinf = 1LL << 60;
ll sum[3000][3000], cts[3000];
int used[6000];
vector<vector<int>> loops;
vector<vector<vector<int>>> loopdist;
vector<int> lens;

void read_problem() {
  cin >> N >> M >> D >> K;
  REP(i, M) {
    int u, v, w;
    cin >> u >> v >> w;
    u--;
    v--;
    E[i] = Edge(u, v, w);
    E[i + M] = Edge(v, u, w);
    G[u].push_back(i);
    G[v].push_back(i + M);
    deg[u]++;
    deg[v]++;
  }
  REP(i, N) { cin >> P[i].x >> P[i].y; }
  REP(i, N)
  REP(j, N)
  euc[i][j] = 1000 * sqrt((P[i].x - P[j].x) * (P[i].x - P[j].x) +
                          (P[i].y - P[j].y) * (P[i].y - P[j].y));
}

int nexte[6000];

int rev(int e) { return e < M ? e + M : e - M; }

vector<int> get_loop(int e) {
  vector<int> ret;
  int x = nexte[e];
  while (x != e) {
    ret.push_back(x);
    x = nexte[x];
  }
  ret.push_back(x);
  return ret;
}

void set_next() {
  REP(i, N) {
    sort(ALL(G[i]), [&](int l, int r) {
      int v1 = E[l].v, v2 = E[r].v;
      return atan2(P[v1].y - P[i].y, P[v1].x - P[i].x) <
             atan2(P[v2].y - P[i].y, P[v2].x - P[i].x);
    });
    REP(j, G[i].size()) { nexte[rev(G[i][(j + 1) % G[i].size()])] = G[i][j]; }
  }
  REP(i, M * 2) used[i] = -1;

  REP(i, M * 2) if (used[i] == -1) {
    auto lp = get_loop(i);
    int len = 0;
    for (auto e : lp) {
      used[e] = loops.size();
      len += E[e].w;
    }
    loops.push_back(lp);
    lens.push_back(len);
  }
  REP(i, loops.size()) {
    auto lp = loops[i];
    vector<vector<int>> dist(lp.size(), vector<int>(lp.size()));
    REP(i, lp.size()) {
      int j = (i + 1) % lp.size();
      int u = E[lp[i]].v;
      int d = E[lp[i]].w;
      while (u != E[lp[i]].u) {
        dist[i][j] = d;
        u = E[lp[j]].v;
        d += E[lp[j]].w;
        j = (j + 1) % lp.size();
      }
    }
    loopdist.push_back(dist);
  }
}

using ui = unsigned int;
ll alldist = 0;
vector<Pii> in_path[6000];
int targets[6000][1000], rests[6000];
int preve[1000][1000], natpreve[1000][1000], natmind[1000][1000];
int mn[1000000], mni[1000000], sdist[1000][1000], sprev[1000][1000];
priority_queue<ui, vector<ui>, greater<ui>> q;
int m1 = ~((1 << 10) - 1), m2 = (1 << 10) - 1;

vector<Pii> penas[3000];
vector<int> pathidx[1000][1000];
vector<int> path[1000][1000];

void set_mind() {
  REP(i, N) {
    auto &dist = mind[i];
    auto &prev = preve[i];
    REP(j, N) dist[j] = prev[j] = inf;
    dist[i] = 0;
    q.push(i);
    while (!q.empty()) {
      auto t = q.top();
      q.pop();
      int d = t >> 10, u = t & m2;
      if (d > dist[u]) continue;
      int j = u;
      if (i < j) alldist += dist[j];
      while (j != i) {
        int e = prev[j];
        path[i][u].push_back(in_path[e].size());
        in_path[e].emplace_back(i, u);
        targets[e][u] = targets[e][i] = 1;
        j = E[e].u;
      }
      reverse(ALL(path[i][u]));
      for (auto e : G[u]) {
        int nd = d + E[e].w;
        if (nd < dist[E[e].v]) {
          dist[E[e].v] = nd;
          prev[E[e].v] = e;
          q.push((nd << 10) | E[e].v);
        }
      }
    }
  }
  REP(i, N) REP(j, N) {
    natmind[i][j] = mind[i][j];
    natpreve[i][j] = preve[i][j];
  }
  constexpr int c2 = 3;

  REP(e, M) {
    if (timer.get_time() > 4700) break;
    int rm = e;
    vector<vector<int>> ps(N);
    for (auto p : in_path[e]) {
      ps[p.first].push_back(p.second);
    }
    REP(s, N) {
      auto &dist = mind[s];
      auto &prev = preve[s];
      if (prev[E[e].v] == e) {
        for (int x : ps[s]) {
          dist[x] = inf;
        }
        for (int u : ps[s]) {
          for (auto e : G[u]) {
            if (e % M != rm) {
              int nd = dist[E[e].v] + E[e].w;
              if (nd < dist[u] && E[e].v != s) {
                dist[u] = nd;
                prev[u] = rev(e);
                preve[u][s] = preve[E[e].v][s];
                q.push((nd << 10) | u);
              }
            }
          }
        }
        while (!q.empty()) {
          auto t = q.top();
          q.pop();
          int d = t >> 10, u = t & m2;
          if (d > dist[u]) continue;
          for (auto e : G[u])
            if (e % M != rm) {
              int nd = d + E[e].w;
              if (nd < dist[E[e].v]) {
                dist[E[e].v] = nd;
                prev[E[e].v] = e;
                preve[E[e].v][s] = preve[u][s];
                q.push((nd << 10) | E[e].v);
              }
            }
        }
        for (int u : ps[s]) {
          mind[u][s] = dist[u];
        }
      }
    }
    REP(s, N) {
      auto &dist = mind[s];
      auto &prev = preve[s];

      for (int t : ps[s]) {
        int x = s, y = t;

        while (s != y && preve[s][y] == natpreve[s][y]) y = E[preve[s][y]].u;
        while (t != x && preve[t][x] == natpreve[t][x]) x = E[preve[t][x]].u;
        int z = y;

        int diff = (dist[t] - natmind[s][t]) / 100;
        while (z != x && z != s) {
          sum[e][natpreve[s][z] % M] -= diff;
          z = E[natpreve[s][z]].u;
        }
        z = y;
        while (z != x && z != s) {
          sum[e][preve[s][z] % M] += diff * c2;
          z = E[preve[s][z]].u;
        }
      }
    }
    REP(s, N) {
      for (int t : ps[s]) {
        mind[s][t] = mind[t][s] = natmind[s][t];
        preve[s][t] = natpreve[s][t];
        preve[t][s] = natpreve[t][s];
      }
    }
  }
  REP(i, M) FOR(j, i + 1, M) if (abs(sum[i][j] + sum[j][i]) > 30) {
    penas[i].emplace_back(sum[i][j] + sum[j][i], j);
    penas[j].emplace_back(sum[i][j] + sum[j][i], i);
  }
  debug(timer.get_time());
}

struct State {
  int ans[3000];
  int rest[30];
  int vtohist[1000][30];
  ll diffs[3000][30];
  ll score;
  State() {
    REP(i, D) rest[i] = K;
    REP(i, M) ans[i] = -1;
    REP(i, N) REP(j, D) vtohist[i][j] = 0;
    int p = 0;
    REP(i, N) {
      for (auto e : G[i])
        if (ans[e % M] == -1) {
          while (rest[p] == 0) p = (p + 1) % D;
          ans[e % M] = p;
          rest[p]--;
          p = (p + 1) % D;
        }
    }
    REP(i, M) {
      if (ans[i] == -1) {
        while (rest[p] == 0) p = (p + 1) % D;
        ans[i] = p;
        rest[p]--;
      }
    }
    REP(i, M) {
      vtohist[E[i].u][ans[i]]++;
      vtohist[E[i].v][ans[i]]++;
      REP(d, D) diffs[i][d] = 0;
      for (auto p : penas[i]) {
        diffs[i][ans[p.second]] += p.first;
      }
    }
  }
  void write_solution() {
    REP(i, M) { cout << ans[i] + 1 << " "; }
    cout << endl;
  }

  void set_score() {
    score = 0;
    REP(i, M) {
      for (auto p : penas[i]) {
        if (ans[i] == ans[p.second]) score += p.first;
      }
    }
    REP(i, N) {
      if (vtohist[i][ans[G[i][0] % M]] >= deg[i]) score += 1e14;
    }
  }
  ll check_diff(int e, int d) {
    ll ret = 0;
    int from = ans[e], u = E[e].u, v = E[e].v;
    ret = (diffs[e][d] - diffs[e][from]) * 2;
    if (vtohist[u][from] == deg[u]) ret -= 1e14;
    if (vtohist[u][d] == deg[u] - 1) ret += 1e14;
    if (vtohist[v][from] == deg[v]) ret -= 1e14;
    if (vtohist[v][d] == deg[v] - 1) ret += 1e14;
    return ret;
  }
  void modify(int e, int d) {
    int from = ans[e];
    rest[from]++;
    vtohist[E[e].u][from]--;
    vtohist[E[e].u][d]++;
    vtohist[E[e].v][from]--;
    vtohist[E[e].v][d]++;
    for (auto p : penas[e]) {
      diffs[p.second][from] -= p.first;
      diffs[p.second][d] += p.first;
    }
    ans[e] = d;
    rest[d]--;
  }
  void avoid_div() {
    while (1) {
      int ok = 1;
      REP(d, D) {
        UnionFind uf(N);
        REP(i, M) {
          if (ans[i] != d) uf.unite(E[i].u, E[i].v);
        }
        REP(i, M) {
          if (ans[i] == d) {
            if (uf.root(E[i].u) != uf.root(E[i].v)) {
              ok = 0;
              int j = ri.next(D);
              while (ans[i] == j || rest[j] == 0) j = ri.next(D);
              ll best = llinf;
              int nxt = -1;
              REP(j, D) if (ans[i] != j && rest[j] > 0) {
                ll diff = check_diff(i, j);
                if (diff < best) {
                  best = diff;
                  nxt = j;
                }
              }
              debug("AVOID");
              modify(i, nxt);
              break;
            }
          }
        }
        if (!ok) break;
      }
      if (ok) break;
    }
  }
};

double PI = 3.14159265359;

int main(int argc, char **argv) {
  read_problem();
  set_next();
  set_mind();
  State state;
  state.set_score();
  State best_state = state;
  int t = 0;
  ll smp = 0;
  int smpt = 0;
  REP(t, 500) {
    int i = ri.next(M), j = ri.next(D);
    while (state.ans[i] == j || state.rest[j] == 0) j = ri.next(D);
    ll diff = state.check_diff(i, j);
    if (abs(diff) < 1e13 && diff > 0) {
      smp += abs(diff);
      smpt++;
    }
  }
  smp /= smpt;
  smp *= 1;
  constexpr int ITR = 0;
  int lim = 5800;

  int LP = 5000000;
  int dur = lim - timer.get_time();
  debug(smp, dur);

  while (ITR || timer.get_time() < lim) {
    if (ITR && t == LP) break;
    int i = ri.next(M), j = ri.next(D);
    while (state.ans[i] == j || state.rest[j] == 0) j = ri.next(D);
    int p = state.ans[i];
    auto diff = state.check_diff(i, j);
    int acc = 1;
    if (diff > 0) {
      double T = 1.0 * (lim + 1 - timer.get_time()) / dur;
      if (ITR) T = 1.0 * (LP - t) / LP;
      if (exp(-diff / T / smp) <= ri.next(10000) / 10000.0) acc = 0;
    }
    if (acc) {
      state.modify(i, j);
      state.score += diff;
      if (state.score < best_state.score) {
        best_state = state;
      }
    }
    t++;
    if (t % 1000000 == 0) debug(t, best_state.score);
  }
  debug(t, best_state.score);
  while (timer.get_time() < 5850) {
    Pii tr;
    ll best = 0;
    REP(i, M) REP(j, D) if (best_state.ans[i] != j && best_state.rest[j] > 0) {
      auto diff = best_state.check_diff(i, j);
      if (diff < best) {
        best = diff;
        tr = {i, j};
      }
    }
    if (best == 0) break;
    best_state.modify(tr.first, tr.second);
    best_state.score += best;
  }
  state = best_state;
  state.avoid_div();
  debug(timer.get_time(), state.score);
  state.write_solution();
  return 0;
}