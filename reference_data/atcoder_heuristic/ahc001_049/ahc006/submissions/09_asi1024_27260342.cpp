#include <bits/stdc++.h>

#define REP(i, n) for (int i = 0; i < (int)(n); i++)
#define ALL(x) (x).begin(), (x).end()
#define HHH(x) cerr << "L" << __LINE__ << ": " << #x << " = " << (x) << endl

using namespace std;

template <typename T> T &chmin(T &a, const T &b) { return a = min(a, b); }
template <typename T> T &chmax(T &a, const T &b) { return a = max(a, b); }

using ll = long long;
using ld = long double;

// Template
class Timer {
public:
  static const int64_t CYCLES_PER_SEC = 3000000000;
  int64_t start, stop;
  Timer(double time_limit) :
    start(get_cycle()), stop(start + time_limit * CYCLES_PER_SEC) {}
  Timer() : Timer(0) {}
  inline int64_t get_cycle() const {
    uint32_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return int64_t(low) | (int64_t(high) << 32);
  }
  inline bool within(int64_t end) const { return get_cycle() < end; }
  inline bool within() const { return within(stop); }
};

Timer timer(1.95);

double get_param(const char* env_var) {
  char* ret = getenv(env_var);
  assert (ret != nullptr);
  return stod(string(ret));
}

class XorShift {
  unsigned int x, y, z, w;
  double nL[65536];
public:
  XorShift() : x(314159265), y(358979323), z(846264338), w(327950288) {}
  inline unsigned int next() {
    unsigned int t = x ^ x << 11;
    x = y; y = z; z = w;
    return w = w ^ w >> 19 ^ t ^ t >> 8;
  }
  inline int next(int n) { return next() % n; }
  inline int next(int lo, int hi) { return lo + next(hi - lo + 1); }
  inline double next(double x) { return double(next()) / (1LL << 32) * x; }
};

XorShift rnd;

struct P {
  int x, y;
  P() : x(0), y(0) {;}
  P(int x, int y) : x(x), y(y) {;}
};

int dist(const P &a, const P &b) { return abs(a.x - b.x) + abs(a.y - b.y); };

using Route = vector<P>;

const int N = 1000;
P from[1024], to[1024];

struct State {
  vector<int> set;
  vector<int> path;
  int dist;
  State() : dist(0) {;}
  void output() const {
    Route r = route();
    cerr << "dist: " << dist << endl;
    cerr << "score: " << int(100000000. / (1000 + dist) + 0.5) << endl;

    cout << set.size();
    for (int x: set) cout << " " << x + 1;
    cout << endl << r.size();
    for (const P &p: r) cout << " " << p.x << " " << p.y;
    cout << endl;
  };
  bool operator<(const State &rhs) const { return dist < rhs.dist; }
  P at(int i) const {
    if (i < 0 || i >= int(path.size())) return P(400, 400);
    int x = path[i];
    if (x % 2 == 0) return from[x / 2];
    else return to[x / 2];
  }
  Route route() const {
    Route res;
    res.reserve(path.size() + 2);
    res.emplace_back(400, 400);
    for (int x: path) {
      if (x % 2 == 0) res.push_back(from[x / 2]);
      else res.push_back(to[x / 2]);
    }
    res.emplace_back(400, 400);
    return res;
  }
};

const int max_block = 1;

State best_add(State s, int np) {
  for (int x: s.set) { if (x == np) assert(false); }
  int pos1 = 0, pos2 = 0, cost = 1e9;
  const Route route = s.route();
  const int M = route.size();

  // ... -> X -> Y -> ...
  REP(i,M-1) {
    int c = dist(route[i], from[np])
      + dist(route[i+1], to[np])
      - dist(route[i], route[i+1]);
    if (c < cost) { pos1 = i; pos2 = i + 1; cost = c; }
  }
  cost += dist(from[np], to[np]);

  // ... -> X -> ... -> Y -> ...
  vector<int> diff1(M-1), diff2(M-1);
  vector<int> best1(M-1, 0), best2(M-1, M-2);
  REP(i,M-1) {
    int old = dist(route[i], route[i+1]);
    diff1[i] = dist(route[i], from[np]) + dist(route[i+1], from[np]) - old;
    diff2[i] = dist(route[i], to[np]) + dist(route[i+1], to[np]) - old;
  }
  for (int i = 1; i < M-1; ++i) {
    best1[i] = diff1[i] < diff1[best1[i-1]] ? i : best1[i-1];
  }
  for (int i = M-3; i >= 0; --i) {
    best2[i] = diff2[i] < diff2[best2[i+1]] ? i : best2[i+1];
  }
  REP(i,M-2) {
    int new_cost = diff1[best1[i]] + diff2[best2[i+1]];
    if (new_cost < cost) {
      cost = new_cost;
      pos1 = best1[i];
      pos2 = best2[i+1] + 1;
    }
  }

  // Update
  s.set.push_back(np);
  s.dist += cost;
  s.path.insert(s.path.begin() + pos1, np * 2 + 0);
  s.path.insert(s.path.begin() + pos2, np * 2 + 1);
  return s;
}

State solve() {
  vector<State> states;
  states.push_back(State());

  REP(loop,1) {
    vector<State> new_states;
    for (State s: states) {
      REP(np, N) {
        bool exists = false;
        for (int x: s.set) {
          if (x == np) { exists = true; break; }
        }
        if (exists) continue;
        new_states.push_back(best_add(s, np));
      }
    }
    sort(ALL(new_states));
    new_states.resize(25);
    // new_states.resize(50);
    states = move(new_states);
  }

  REP(loop,12) {
    vector<State> new_states;
    REP(i,states.size()) {
      const State &s = states[i];
      REP(np, N) {
        bool exists = false;
        for (int x: s.set) {
          if (x == np) { exists = true; break; }
        }
        if (exists) continue;
        State new_s = best_add(s, np);
        if (i >= int(new_states.size()))
          new_states.push_back(new_s);
        else if (new_s.dist < new_states[i].dist)
          new_states[i] = new_s;
      }
    }
    new_states.resize(25);
    states = move(new_states);
  }

  sort(ALL(states));
  states.resize(5);

  REP(loop,29) {
    vector<State> new_states;
    REP(i,states.size()) {
      const State &s = states[i];
      REP(np, N) {
        bool exists = false;
        for (int x: s.set) {
          if (x == np) { exists = true; break; }
        }
        if (exists) continue;
        State new_s = best_add(s, np);
        if (i >= int(new_states.size()))
          new_states.push_back(new_s);
        else if (new_s.dist < new_states[i].dist)
          new_states[i] = new_s;
      }
    }
    new_states.resize(5);
    states = move(new_states);
  }

  REP(loop,8) {
    vector<State> new_states;
    for (State s: states) {
      REP(np, N) {
        bool exists = false;
        for (int x: s.set) {
          if (x == np) { exists = true; break; }
        }
        if (exists) continue;
        new_states.push_back(best_add(s, np));
      }
    }
    sort(ALL(new_states));
    new_states.resize(1);
    // new_states.resize(50);
    states = move(new_states);
  }
  return states[0];
};

const double start_temp = 30;

void optimize(State &s) {
  int loop_count = 0;
  const int MAX_LOOPS = 23000;
  while (timer.within()) {
    const double temp = start_temp * (MAX_LOOPS - loop_count) / MAX_LOOPS;
    State best_s = s;

    // Erase
    int r = rnd.next(50);
    int delete_pos = s.set[r];
    s.set.erase(s.set.begin() + r);
    int del1 = 0, del2 = 0;
    REP(i,int(s.path.size())) {
      if (s.path[i] == delete_pos * 2 + 0) del1 = i;
      else if (s.path[i] == delete_pos * 2 + 1) del2 = i;
    }
    s.dist += dist(s.at(del2-1), s.at(del2+1))
      - dist(s.at(del2), s.at(del2-1))
      - dist(s.at(del2), s.at(del2+1));
    s.path.erase(s.path.begin() + del2);
    s.dist += dist(s.at(del1-1), s.at(del1+1))
      - dist(s.at(del1), s.at(del1-1))
      - dist(s.at(del1), s.at(del1+1));
    s.path.erase(s.path.begin() + del1);

    // Best add
    REP(i,10) {
      int np = rnd.next(N);
      bool exists = false;
      for (int x: s.set) {
        if (x == np) { exists = true; break; }
      }
      if (exists) continue;
      State new_state = best_add(s, np);

      bool ok = false;
      if (new_state.dist < best_s.dist) ok = true;
      else if (temp > 0) {
        double prob = exp((best_s.dist - new_state.dist) / temp);
        if (prob * 65536 > rnd.next(65536)) ok = true;
      }
      if (ok) { // if (new_state.dist < best_s.dist) {
        best_s = new_state;
      }
    }
    s = best_s;
    ++loop_count;
  }
  cerr << "LOOPS: " << loop_count << endl;
}

int main() {
  REP(i,N) cin >> from[i].x >> from[i].y >> to[i].x >> to[i].y;
  State state = solve();
  optimize(state);
  state.output();
  return 0;
}
