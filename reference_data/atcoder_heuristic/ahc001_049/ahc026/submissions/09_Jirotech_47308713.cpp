

namespace atcoder {}

#ifdef LOCAL
#define dbg(x) cerr << __LINE__ << " : " << #x << " = " << (x) << endl;
#else
#define NDEBUG
#define dbg(x) true;
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#endif

#ifdef GTEST
#include <gtest/gtest.h>
#endif

#include <math.h>

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#ifdef PERF
#include <gperftools/profiler.h>
#endif

using namespace std;
using namespace atcoder;
#define fast_io                     \
  ios_base::sync_with_stdio(false); \
  cin.tie(0);                       \
  cout.tie(0);
#define ll long long int
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define reps(i, n) for (int i = 1; i <= (int)(n); i++)
#define REP(i, n) for (int i = n - 1; i >= 0; i--)
#define REPS(i, n) for (int i = n; i > 0; i--)
#define MOD (long long int)(1e9 + 7)
#define INF (int)(1e9)
#define LINF (long long int)(1e18)
#define chmax(a, b) a = (((a) < (b)) ? (b) : (a))
#define chmin(a, b) a = (((a) > (b)) ? (b) : (a))
#define ALL(v) v.begin(), v.end()
typedef pair<int, int> Pii;
typedef pair<ll, ll> Pll;
constexpr double PI = acos(-1);

#ifdef NDEBUG
#define CHECK(v1, op, v2)
#else
#define CHECK(v1, op, v2)                            \
  if (!((v1)op(v2))) {                               \
    cerr << "ERROR:" << (v1) << " " << (v2) << endl; \
    assert((v1)op(v2));                              \
  }
#endif

long double nCr(const int n, const int r) {
  long double ret = 1;
  rep(t, r) {
    ret *= (n - t);
    ret /= (r - t);
  }
  return ret;
}

template <typename T>
string to_string(const vector<T>& vec) {
  string ret = "";
  rep(i, vec.size()) {
    ret += vec[i].to_string();
    if (i + 1 != vec.size()) {
      ret += ",";
    }
  }
  return ret;
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& vec) {
  os << to_string(vec);
  return os;
}

uint32_t xorshift() {
  static uint32_t x = 12345789;
  static uint32_t y = 362436069;
  static uint32_t z = 521288629;
  static uint32_t w = 88675123;
  uint32_t t;
  t = x ^ (x << 11);
  x = y;
  y = z;
  z = w;
  w ^= t ^ (t >> 8) ^ (w >> 19);

  return w;
}

int rand(const uint32_t l, const uint32_t r) {
  return xorshift() % (r - l) + l;
}

uint32_t rand_other_than(const uint32_t l, const uint32_t r,
                         const uint32_t other) {
  const uint32_t num = rand(l, r - 1);
  return num + (num >= other);
}

template <typename T>
const T& rand_vec(const vector<T>& vec) {
  assert(vec.size() > 0);
  return vec[rand(0, vec.size())];
}

template <typename T>
void shuffle(vector<T>& vec) {
  rep(l, (int)vec.size() - 1) {
    const int idx = rand(l, vec.size());
    swap(vec[idx], vec[l]);
  }
}


class Timer {
  chrono::system_clock::time_point _start, _end;
  ll _sum = 0, _count = 0;

 public:
  void start() { _start = chrono::system_clock::now(); }

  void stop() { _end = chrono::system_clock::now(); }

  void add() {
    const chrono::system_clock::time_point now = chrono::system_clock::now();
    _sum += static_cast<double>(
        chrono::duration_cast<chrono::nanoseconds>(now - _start).count());
    _count++;
  }

  ll sum() const { return _sum / 1000; }

  int count() const { return _count; }

  string average() const {
    if (_count == 0) {
      return "NaN";
    }
    return to_string(_sum / 1000 / _count);
  }

  void reset() {
    _start = chrono::system_clock::now();
    _sum = 0;
    _count = 0;
  }

  inline int ms() const {
    const chrono::system_clock::time_point now = chrono::system_clock::now();
    return static_cast<double>(
        chrono::duration_cast<chrono::microseconds>(now - _start).count() /
        1000);
  }

  inline int ns() const {
    const chrono::system_clock::time_point now = chrono::system_clock::now();
    return static_cast<double>(
        chrono::duration_cast<chrono::microseconds>(now - _start).count());
  }
};

#ifdef LOCAL
struct Timers : unordered_map<string, Timer> {
  friend ostream& operator<<(ostream& os, const Timers& timers) {
    for (const auto& pa : timers) {
      os << pa.first << " time: " << pa.second.sum() / 1000
         << " count: " << pa.second.count() << endl;
    }
    return os;
  }
};
#else
struct Timers {
  struct Dummy {
    void start() const {}
    void add() const {}
  };
  Dummy dummy;
  const Dummy& operator[](const std::string& str) { return dummy; }
  friend ostream& operator<<(ostream& os, const Timers& timers) { return os; }
};
#endif

Timers global_timers;





/* start */

Timer global_timer;
const int N = 200;
const int M = 10;
vector<vector<int>> B;

struct Output {
  static void StaticInit(istream& is) {
    global_timer.start();
    int _;
    is >> _ >> _;
    B.resize(M);
    rep(i, M) {
      B[i].resize(N / M);
      // 0が下
      // pop_back()
      rep(j, N / M) {
        is >> B[i][j];
        B[i][j]--;
      }
    }
  }
  friend ostream& operator<<(ostream& os, const Output& output) { return os; }
};



/* start */

vector<double> PARAMS = {1.0, 0.1};




/* start */

struct Action {
  int v;
  int to;
  int health;

  friend ostream& operator<<(ostream& os, const Action& action) {
    os << action.v + 1 << " " << action.to + 1;
    return os;
  }
};

int TotalHealth(const vector<Action>& actions) {
  int ret = 0;
  for (const auto& action : actions) {
    ret += action.health;
  }
  return ret;
}

vector<int> ball2order;
bool renzoku_matomete = true;

struct Yama {
  vector<int> balls;
  int id;
  int sorted_count = 0;

  Yama(const vector<int>& _balls, int _id) : balls(_balls), id(_id) {}

  vector<Action> Sort(int& next_ball, vector<Yama>& other_yamas) {
    // 降順ソートされているボール群を取り出して、降順にならべる
    vector<Action> ret;
    // 降順の最大数
    vector<int> max_order_count(M);
    rep(i, M) {
      if (other_yamas[i].sorted_count > 0) {
        max_order_count[i] = -INF;
        continue;
      }
      REP(j, other_yamas[i].balls.size()) {
        if (j - 1 >= 0 && other_yamas[i].balls[j - 1] > other_yamas[i].balls[j])
          break;

        max_order_count[i]++;
      }
    }
    // その山の中での各ballの順位
    if (ball2order.size() == 0) {
      ball2order.resize(N);
    }
    {
      vector<int> sorted_balls = balls;
      sort(ALL(sorted_balls));
      rep(i, sorted_balls.size()) { ball2order[sorted_balls[i]] = i; }
    }

    REP(i, balls.size()) {
      if (balls[i] == next_ball) {
        ret.push_back(Action{balls[i], -1, 0});
        balls.pop_back();
        next_ball++;
        continue;
      }

      // 昇順に連続している場合もまとめて取り出す
      if (renzoku_matomete) {
        if (i - 1 >= 0 &&
            ball2order[balls[i - 1]] == ball2order[balls[i]] + 1 &&
            balls[i - 1] != next_ball)
          continue;
      }
      // TODO: 最後の1手をさぼれるならサボる

      // 転倒境目なのでi以降をうごかす
      int max_ball = -INF;
      int best_j = -1;
      rep(jjj, M) {
        if (jjj == id) continue;
        if (other_yamas[jjj].sorted_count > 0) continue;
        const int other_ball = (int)other_yamas[jjj].balls.size() == 0
                                   ? -1
                                   : other_yamas[jjj].balls.back();
        if (balls[i] > other_ball && max_ball < other_ball) {
          max_ball = other_ball;
          best_j = jjj;
        }
        if (renzoku_matomete) {
          if (ball2order[balls[i]] + 1 == ball2order[other_ball]) {
            best_j = jjj;
            max_ball = INF;
          }
        }
      }

      // 降順なのでまとめて取り出したほうが嬉しい場合はまとめて取り出す
      if (i - 1 >= 0 && balls[i - 1] < balls[i] && balls[i - 1] > max_ball &&
          balls[i - 1] != next_ball)
        continue;

      // TODO: 大丈夫？
      if (best_j == -1) {
        // 降順が最も少ないところに諦めてassign
        int min_count = INF;
        rep(jjj, M) {
          if (jjj == id) continue;
          const int order_count = max_order_count[jjj];
          if (order_count < min_count) {
            min_count = order_count;
            best_j = jjj;
          }
        }
        max_order_count[best_j] = 1000;
        if (other_yamas[best_j].sorted_count > 0) {
          max_order_count[best_j] = INF;
        }
      }

      if (best_j == -1) {
        // TODO: テストケースに含まれるなら対応
        cerr << "score: 1" << endl;
        std::quick_exit(0);
      }

      ret.push_back(Action{balls[i], best_j, 1});

      for (int iii = i; iii < balls.size(); iii++) {
        other_yamas[best_j].balls.push_back(balls[iii]);
        ret.back().health++;
      }
      max_order_count[best_j] += ret.back().health - 1;
      assert(ret.back().health >= 2);
      balls.resize(i);
      continue;
    }

    // 他の山に移したやつを戻す
    while (true) {
      const int current_top = (int)balls.size() == 0 ? INF : balls.back();
      int best_j = -1;
      int max_ball = -INF;
      rep(j, other_yamas.size()) {
        if (j == id) continue;
        if (other_yamas[j].sorted_count == other_yamas[j].balls.size())
          continue;
        // 他の山からも持ってくる
        int other_ball = -1;
        if ((int)other_yamas[j].balls.size() == 0) {
          continue;
        }
        other_ball = other_yamas[j].balls.back();
        if (other_ball == next_ball) {
          continue;
        }

        if (other_ball < current_top && max_ball < other_ball) {
          max_ball = other_ball;
          best_j = j;
        }
      }

      if (best_j == -1) break;
      int best_i = (int)other_yamas[best_j].balls.size() - 1;
      if (renzoku_matomete) {
        for (;; best_i--) {
          if (best_i - 1 >= 0 &&
              ball2order[other_yamas[best_j].balls[best_i - 1]] ==
                  ball2order[other_yamas[best_j].balls[best_i]] + 1) {
            continue;
          }
          break;
        }
      }
      ret.push_back(Action{other_yamas[best_j].balls[best_i], id,
                           (int)other_yamas[best_j].balls.size() - best_i + 1});
      balls.insert(balls.end(), other_yamas[best_j].balls.begin() + best_i,
                   other_yamas[best_j].balls.end());
      other_yamas[best_j].balls.resize(best_i);
    }
    sorted_count = balls.size();
    return ret;
  }
};

struct Solution {
  vector<Yama> yamas;
  int next_ball = 0;

  Solution() {
    if (ball2order.size() == 0) {
      ball2order.resize(N);
    }
    rep(i, M) {
      yamas.emplace_back(Yama(B[i], i));
      //   vector<int> sorted_balls = B[i];
      //   sort(ALL(sorted_balls));
      //   rep(j, sorted_balls.size()) { ball2order[sorted_balls[j]] = j; }
    }
  }

  vector<Action> MoveToZero() {
    vector<Action> ret;
    while (true) {
      bool ok = false;

      rep(i, M) {
        if (yamas[i].balls.size() > 0 && yamas[i].balls.back() == next_ball) {
          ret.push_back(Action{next_ball, -1, 0});
          yamas[i].balls.pop_back();
          if (yamas[i].sorted_count == (int)yamas[i].balls.size() + 1) {
            yamas[i].sorted_count--;
          }
          next_ball++;
          ok = true;
          break;
        }
      }
      if (!ok) break;
    }
    return ret;
  }
};

class Solver {
 public:
  Solver() {}
  vector<Action> Run() {
    Solution sol;

    vector<Action> actions;
    vector<int> ball2yamas(N);
    rep(i, M) {
      for (const auto ball : sol.yamas[i].balls) {
        ball2yamas[ball] = i;
      }
    }
    while (true) {
      bool finish = true;
      rep(i, M) {
        if (sol.yamas[i].balls.size() > 0) {
          finish = false;
          break;
        }
      }
      if (finish) break;
      const auto ret1 = sol.MoveToZero();
      actions.insert(actions.end(), ALL(ret1));
      if (sol.next_ball == N) break;
      int i = ball2yamas[sol.next_ball];

      const auto ret2 = sol.yamas[i].Sort(sol.next_ball, sol.yamas);
      actions.insert(actions.end(), ALL(ret2));
      if (global_timer.ms() > 1000) {
        for (const auto action : actions) {
          cout << action << endl;
        }
        std::quick_exit(0);
      }
    }
    return actions;
  }
  void Solve() {
    vector<Action> best_actions;
    int min_turn = INF;
    // {
    //   renzoku_matomete = true;
    //   const auto actions = Run();
    //   const int turn = TotalHealth(actions);
    //   if (min_turn > turn) {
    //     min_turn = turn;
    //     best_actions = actions;
    //   }
    // }
    {
      renzoku_matomete = false;
      const auto actions = Run();
      const int turn = TotalHealth(actions);
      if (min_turn > turn) {
        min_turn = turn;
        best_actions = actions;
      }
    }

    for (const auto action : best_actions) {
      cout << action << endl;
    }

    cerr << "score: " << max(1, 10000 - TotalHealth(best_actions)) << endl;
    return;
  }

 private:
};

int main(int argc, char* argv[]) {
  fast_io;

  if (argc >= 2) {
    int idx = 0;
    for (int i = 1; i < argc; ++i) {
      PARAMS[idx++] = std::stod(argv[i]);
    }
  }

  Timer timer;
  timer.start();
  Output::StaticInit(cin);
  Solver solver;
  solver.Solve();
  return 0;
}