

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
#define all(v) v.begin(), v.end()
typedef pair<int, int> Pii;
typedef pair<ll, ll> Pll;
const double PI = acos(-1);

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

int rand(const int l, const int r) {
  return xorshift() % (r - l) + l;
}

int rand_other_than(const int l, const int r,
                         const int other) {
  const int num = rand(l, r - 1);
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

template <class T, class U = T>
bool chmin(T& x, U&& y) {
  return y < x && (x = std::forward<U>(y), true);
}

template <class T, class U = T>
bool chmax(T& x, U&& y) {
  return x < y && (x = std::forward<U>(y), true);
}

template <typename Ret, typename T>
Ret Sum(const vector<T>& vec) {
  return std::accumulate(all(vec), (Ret)0);
}

template <typename Ret, typename T>
Ret Mean(const vector<T>& vec) {
  assert((int)vec.size() > 0);
  return Sum<T, Ret>(vec) / vec.size();
}

template <typename Ret, typename T>
Ret Std(const vector<T>& vec) {
  assert((int)vec.size() > 0);
  const auto mean =  Mean<Ret>(vec);
  const auto sum2 = std::accumulate(
      all(vec), (Ret)0,
      [](const Ret acc, const T val) { return acc + (Ret)val * val; });
  return (Ret)sum2 / vec.size() - mean * mean;
}

template<typename T, typename U>
T Ceil(const T a, const U b){
  assert(a >= 0);
  assert(b > 0);
  return (a + b - 1) / b;
}

template<typename T, typename U>
T Mod(const T a, const U b){
  return (a + b) % b;
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

struct Pos {
  int x, y;
};

int N, M, H;
vector<int> A;
// 大きい順
vector<int> sortdAIdxes;
vector<vector<int>> G;
vector<Pos> Ps;
vector<vector<int>> triangleNodes;

struct Output {
  static void StaticInit(istream &is) {
    global_timer.start();
    is >> N >> M >> H;
    rep(i, N) {
      int a;
      is >> a;
      A.push_back(a);
    }
    sortdAIdxes.resize(N);
    iota(all(sortdAIdxes), 0);
    sort(all(sortdAIdxes), [&](int i, int j) { return A[i] > A[j]; });
    G.resize(N);
    rep(i, M) {
      int u, v;
      is >> u >> v;
      G[u].push_back(v);
      G[v].push_back(u);
    }

    rep(i, N) {
      int x, y;
      is >> x >> y;
      Ps.push_back({x, y});
    }

    // 三角形の頂点
    rep(i, N){
      for(const auto adj : G[i]){
        for(const auto adj2 : G[adj]){
          if(adj2 == i) continue;
          bool isTriangle = false;
          for(const auto adj3 : G[adj2]){
            if(adj3 == i){
              isTriangle = true;
              break;
            }
          }
          if(!isTriangle) continue;
          if(min({i, adj, adj2}) != i) continue;
          vector<int> sortedNodes = {i, adj, adj2};
          sort(all(sortedNodes));
          triangleNodes.push_back(sortedNodes);
        }
      }
    }
    cerr << triangleNodes.size() << endl;
  }
  friend ostream &operator<<(ostream &os, const Output &output) { return os; }
};



/* start */

vector<double> PARAMS = {4e2, 2e1};





/* start */





// NBD = neighborhood
template <class Solution, class NBDGenerator>
struct SimulatedAnnealing {
 public:
  SimulatedAnnealing(double end_time, double start_temp, double end_temp)
      : end_time_(end_time * 1000),  // ns
        inv_time_(1.0 / end_time_),
        cur_time_(0.0),
        start_temp_(start_temp),
        end_temp_(end_temp),
        diff_temp_(end_temp_ / start_temp_),
        cur_temp_(start_temp) {
    assert(start_temp >= end_temp);
    timer_.start();
  }

  Solution run(const Solution& initial_sol, NBDGenerator& nbd_generator) {
    Solution sol(initial_sol);

    int acc = 0;
    int g = 0;
    for (;; ++g) {
      if ((g & 0xfff) == 0) {
#ifdef SA_MAX_G
        if (g >= SA_MAX_G) {
          break;
        }
#endif
        UpdateTime();
        UpdateTemp();
        if (cur_time_ > end_time_) {
          break;
        }
      }

      const auto nbd = nbd_generator.Generate(&sol, g, cur_time_ * inv_time_);
      constexpr int mod = 1'000'000'000;
      const int r = rand(0, mod);
      if (static_cast<double>(r) / mod < exp(-nbd->Diff() / cur_temp_)) {
        acc++;
        nbd->Update(&sol);
      } else {
        nbd->Restore(&sol);
      }
    }
    cerr << "g,acc: " << g << " " << acc << endl;
    return sol;
  }

 private:
  double end_time_, inv_time_, cur_time_;
  double start_temp_, end_temp_, diff_temp_, cur_temp_;
  Timer timer_;

  void UpdateTime() { cur_time_ = timer_.ns(); }
  void UpdateTemp() {
    cur_temp_ = start_temp_ * pow(diff_temp_, cur_time_ * inv_time_);
    // cur_temp_ = max(0.0, start_temp_ - cur_time_ * diff_temp_ * inv_time_);
  }
};




#include <iostream>

/*

実装手順
0. Output::StaticInit(istream&)を実装
1. Solutionの初期解生成を書く
2. Solutionに対する操作とEvalの更新を書く
3. Evalの初期値計算を書く
4. Eval::Sum()とSolution::Score()を書く
5. 近傍操作と復元操作を書く
6. 近傍の各確率を調整

*/

struct Evaluation {
  int realScore = 1;
  int invalidCount = 0;
  // 最大化問題の場合は符号を反転すること
  double Sum() const { return -realScore + invalidCount * 1e9; }
};

struct Solution {
  Evaluation eval;
  vector<int> hs;
  vector<int> parentAdjCounts;

  Solution() : hs(N, 0), parentAdjCounts(N, 0) { InitEval(); }
  void InitEval() {
    eval = Evaluation();
    parentAdjCounts = vector<int>(N, 0);
    rep(i, N) {
      eval.realScore += (hs[i] + 1) * A[i];

      // 隣接する、自分より1小さい数の個数
      for (const auto adj : G[i]) {
        if (hs[i] == hs[adj] + 1) {
          parentAdjCounts[i]++;
        }
      }

      if (hs[i] != 0 && parentAdjCounts[i] == 0) {
        eval.invalidCount++;
      }
    }
  }
  static Solution CreateInitialSol() {
    Solution sol;
    // cerr << sol.Eval() << endl;
    // Aが大きい順に貪欲に大きくしていく
    // ほぼ意味ない
    rep(t, H) {
      rep(_i, N) {
        const int idx = sortdAIdxes[_i];
        if (sol.hs[idx] != t)
          continue;

        auto diff = -sol.Eval();
        sol.ChangeHeight(idx, t + 1);
        diff += sol.Eval();
        if (diff > 0.0) {
          // 復元
          sol.ChangeHeight(idx, t);
        }
      }
    }
    // cerr << sol.Eval() << endl;
    sol.InitEval();
    return sol;
  }
  void ChangeHeight(const int idx, const int newH) {
    if(hs[idx] == newH) return;
    // 自分
    if (hs[idx] != 0 && parentAdjCounts[idx] == 0) {
      // invalidを解消
      eval.invalidCount--;
    }
    parentAdjCounts[idx] = 0;

    // 隣
    for (const auto adj : G[idx]) {
      if (hs[adj] == hs[idx] + 1) {
        // 1つ減らす
        parentAdjCounts[adj]--;
        // invalidになった
        if (hs[adj] != 0 && parentAdjCounts[adj] == 0)
          eval.invalidCount++;
      }
      if (hs[adj] == newH + 1) {
        // 1つ増やす
        parentAdjCounts[adj]++;
        // invalidではなくなった
        if (hs[adj] != 0 && parentAdjCounts[adj] == 1)
          eval.invalidCount--;
      }
      if (newH == hs[adj] + 1) {
        // 自分
        parentAdjCounts[idx]++;
      }
    }
    eval.realScore += (newH - hs[idx]) * A[idx];
    hs[idx] = newH;

    // invalidになった
    if (hs[idx] != 0 && parentAdjCounts[idx] == 0) {
      eval.invalidCount++;
    }
  }
  double Eval() const { return eval.Sum(); }
  double Score() const { return -Eval(); }
  friend ostream &operator<<(ostream &os, const Solution &sol) {
    rep(i, N) {
      // cerr << sol.hs[i] << " ";
      if (i != 0)
        os << " ";

      if (sol.hs[i] == 0) {
        os << "-1";
        continue;
      }

      // cerr << i << " " << sol.hs[i];
      for (const auto adj : G[i]) {
        // cerr << " " << sol.hs[adj];
        if (sol.hs[i] == sol.hs[adj] + 1) {
          os << adj;
          break;
        }
      }
      // cerr << endl;
    }
    return os;
  }
};

struct NBD {
  double Diff() const { return diff; }
  void Update(Solution *) const { return; };
  virtual void Restore(Solution *) const = 0;
  double diff;
};

// 隣+1にする
struct NBD1 : public NBD {
  int idx;
  int oldH;
  int newH;
  NBD1() {}
  NBD1(Solution *const sol) {
    diff = -sol->Eval();
    while (true) {
      idx = rand(0, N);
      oldH = sol->hs[idx];
      newH = rand(0, 100) < 90 ? sol->hs[rand_vec(G[idx])] + 1 : rand(0, 2) == 0 ? 0 : H;
      if (oldH == newH)
        continue;
      if (newH > H)
        continue;
      sol->ChangeHeight(idx, newH);
      diff += sol->Eval();
      break;
    }
  }
  void Restore(Solution *sol) const override {
    sol->ChangeHeight(idx, oldH);
    return;
  }
};

// 1増減
struct NBD2 : public NBD {
  int idx;
  int oldH;
  int newH;
  NBD2() {}
  NBD2(Solution *const sol) {
    diff = -sol->Eval();
    while (true) {
      idx = rand(0, N);
      oldH = sol->hs[idx];
      newH = oldH == 0         ? 1
             : oldH == H       ? H - 1
             : rand(0, 2) == 0 ? oldH + 1
                               : oldH - 1;
      sol->ChangeHeight(idx, newH);
      diff += sol->Eval();
      break;
    }
  }
  void Restore(Solution *sol) const override { sol->ChangeHeight(idx, oldH); }
};

// 隣接スワップ
struct NBD3 : public NBD {
  int idx0;
  int idx1;
  int oldH0;
  int oldH1;
  NBD3() {}
  NBD3(Solution *const sol) {
    diff = -sol->Eval();
    while (true) {
      idx0 = rand(0, N);
      oldH0 = sol->hs[idx0];
      idx1 = rand_vec(G[idx0]);
      oldH1 = sol->hs[idx1];
      sol->ChangeHeight(idx0, oldH1);
      sol->ChangeHeight(idx1, oldH0);
      diff += sol->Eval();
      break;
    }
  }
  void Restore(Solution *sol) const override {
    sol->ChangeHeight(idx0, oldH0);
    sol->ChangeHeight(idx1, oldH1);
  }
};

// 隣接増減
struct NBD4 : public NBD {
  int idx0;
  int idx1;
  int oldH0;
  int oldH1;
  NBD4() {}
  NBD4(Solution *const sol) {
    diff = -sol->Eval();
    while (true) {
      idx0 = rand(0, N);
      oldH0 = sol->hs[idx0];
      idx1 = rand_vec(G[idx0]);
      oldH1 = sol->hs[idx1];
      if (oldH0 == oldH1)
        continue;
      if (abs(oldH0 - oldH1) >= 2)
        continue;
      const int hDiff = rand(0, 100) < 90 ? 1 : -1;
      if (oldH0 + hDiff > H || oldH1 + hDiff > H)
        continue;
      if (oldH0 + hDiff < 0 || oldH1 + hDiff < 0)
        continue;
      sol->ChangeHeight(idx0, oldH0 + hDiff);
      sol->ChangeHeight(idx1, oldH1 + hDiff);
      diff += sol->Eval();
      break;
    }
  }
  void Restore(Solution *sol) const override {
    sol->ChangeHeight(idx0, oldH0);
    sol->ChangeHeight(idx1, oldH1);
  }
};

// 隣接する自分+1を全て増加
struct NBD5 : public NBD {
  vector<int> idxes;
  vector<int> oldHs;
  NBD5() {}
  NBD5(Solution *const sol) {
    diff = -sol->Eval();
    while (true) {
      idxes.clear();
      oldHs.clear();
      const int baseIdx = rand(0, N);
      const int baseOldH = sol->hs[baseIdx];
      if (baseOldH + 2 > H)
        continue;
      idxes.push_back(baseIdx);
      oldHs.push_back(baseOldH);

      for (const auto adj : G[baseIdx]) {
        const int adjH = sol->hs[adj];
        if (adjH == baseOldH + 1) {
          idxes.push_back(adj);
          oldHs.push_back(adjH);
        }
      }

      rep(i, idxes.size()) { sol->ChangeHeight(idxes[i], oldHs[i] + 1); }
      diff += sol->Eval();
      break;
    }
  }
  void Restore(Solution *sol) const override {
    rep(i, idxes.size()) { sol->ChangeHeight(idxes[i], oldHs[i]); }
  }
};

// 隣接ABについて、 A = B と　B = rand()
struct NBD6 : public NBD {
  int idx0;
  int idx1;
  int oldH0;
  int oldH1;
  NBD6() {}
  NBD6(Solution *const sol) {
    diff = -sol->Eval();
    while (true) {
      idx0 = rand(0, N);
      oldH0 = sol->hs[idx0];
      idx1 = rand_vec(G[idx0]);
      oldH1 = sol->hs[idx1];
      // A = B
      const int newH0 = oldH1;

      // B = rand()
      const auto adjAdj = rand_vec(G[idx1]);
      if (adjAdj == idx0)
        continue;
      const int newH1 = sol->hs[adjAdj] + 1;
      if (newH1 > H)
        continue;

      sol->ChangeHeight(idx0, newH0);
      sol->ChangeHeight(idx1, newH1);
      diff += sol->Eval();
      break;
    }
  }
  void Restore(Solution *sol) const override {
    sol->ChangeHeight(idx0, oldH0);
    sol->ChangeHeight(idx1, oldH1);
  }
};

// 三角形のローテーション
struct NBD7 : public NBD {
  int idx0;
  int idx1;
  int idx2;
  int oldH0;
  int oldH1;
  int oldH2;
  NBD7() {}
  NBD7(Solution *const sol) {
    diff = -sol->Eval();
    while (true) {
      const int triangleIdx = rand(0, triangleNodes.size());
      const auto &nodes = triangleNodes[triangleIdx];
      idx0 = nodes[0];
      idx1 = nodes[1];
      idx2 = nodes[2];
      oldH0 = sol->hs[idx0];
      oldH1 = sol->hs[idx1];
      oldH2 = sol->hs[idx2];
      if (rand(0, 2) == 0) {
        // 0 -> 1 -> 2
        sol->ChangeHeight(idx0, oldH1);
        sol->ChangeHeight(idx1, oldH2);
        sol->ChangeHeight(idx2, oldH0);
      } else {
        // 0 -> 2 -> 1
        sol->ChangeHeight(idx0, oldH2);
        sol->ChangeHeight(idx1, oldH0);
        sol->ChangeHeight(idx2, oldH1);
      }
      diff += sol->Eval();
      break;
    }
  }
  void Restore(Solution *sol) const override {
    sol->ChangeHeight(idx0, oldH0);
    sol->ChangeHeight(idx1, oldH1);
    sol->ChangeHeight(idx2, oldH2);
  }
};

// 三角形のうち2つをスワップ

struct NBD8 : public NBD {
  int idx0;
  int idx1;
  int oldH0;
  int oldH1;
  NBD8() {}
  NBD8(Solution *const sol) {
    diff = -sol->Eval();
    while (true) {
      const int triangleIdx = rand(0, triangleNodes.size());
      const auto &nodes = triangleNodes[triangleIdx];
      const int _i0 = rand(0, 3);
      const int _i1 = rand_other_than(0, 3, _i0);
      idx0 = nodes[_i0];
      idx1 = nodes[_i1];
      oldH0 = sol->hs[idx0];
      oldH1 = sol->hs[idx1];
      sol->ChangeHeight(idx0, oldH1);
      sol->ChangeHeight(idx1, oldH0);

      diff += sol->Eval();
      break;
    }
  }
  void Restore(Solution *sol) const override {
    sol->ChangeHeight(idx0, oldH0);
    sol->ChangeHeight(idx1, oldH1);
  }
};

struct NBDGenerator {
  NBD *Generate(Solution *const sol, const int g, const double progress) {
    if (g == 0 || sol->Score() > best_sol.Score()) {
      best_sol = *sol;
    }
#ifdef LOCAL
    if (g % 100000 == 0) {
      // cout << *sol << endl;
    }
#endif

    const int random = rand(0, 105);
    if (random < 75) {
      nbd1_ = NBD1(sol);
      return &nbd1_;
    // } else if (random < -1) {
    //   nbd2_ = NBD2(sol);
    //   return &nbd2_;
    } else if (random < 95) {
      nbd3_ = NBD3(sol);
      return &nbd3_;
    } else if (random < 100) {
      nbd4_ = NBD4(sol);
      return &nbd4_;
    // } else if (random < -1) {
    //   nbd5_ = NBD5(sol);
    //   return &nbd5_;
    } else if (random < 105) {
      nbd6_ = NBD6(sol);
      return &nbd6_;
    // } else if (random < -1) {
    //   nbd7_ = NBD7(sol);
    //   return &nbd7_;
    // } else if (random < -1) {
    //   nbd8_ = NBD8(sol);
    //   return &nbd8_;
    } else {
      assert(false);
    }
  }
  const Solution &GetBestSol() const { return best_sol; }

private:
  NBD1 nbd1_;
  NBD2 nbd2_;
  NBD3 nbd3_;
  NBD4 nbd4_;
  NBD5 nbd5_;
  NBD6 nbd6_;
  NBD7 nbd7_;
  NBD8 nbd8_;
  Solution best_sol;
};

void Run() {
  Solution first_sol = Solution::CreateInitialSol();
  NBDGenerator gen;
  SimulatedAnnealing<Solution, NBDGenerator> SA(1990 - global_timer.ms(),
                                                PARAMS[0], PARAMS[1]);
  const auto final_sol = SA.run(first_sol, gen);
  const auto best_sol = gen.GetBestSol();
  cerr << "score: " << best_sol.Score() << endl;
  cout << best_sol << endl;
}

/* start */

class Solver {
public:
  Solver() {}

  void Solve() {
    Run();
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