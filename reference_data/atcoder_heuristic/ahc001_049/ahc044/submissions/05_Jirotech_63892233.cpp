

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
// #define MOD (long long int)(1e9 + 7)
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
  static uint32_t w = 38675123;
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


#ifndef ATCODER_DSU_HPP
#define ATCODER_DSU_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>

namespace atcoder {

// Implement (union by size) + (path compression)
// Reference:
// Zvi Galil and Giuseppe F. Italiano,
// Data structures and algorithms for disjoint set union problems
struct dsu {
  public:
    dsu() : _n(0) {}
    dsu(int n) : _n(n), parent_or_size(n, -1) {}

    int merge(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        int x = leader(a), y = leader(b);
        if (x == y) return x;
        if (-parent_or_size[x] < -parent_or_size[y]) std::swap(x, y);
        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;
        return x;
    }

    bool same(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        return leader(a) == leader(b);
    }

    int leader(int a) {
        assert(0 <= a && a < _n);
        if (parent_or_size[a] < 0) return a;
        return parent_or_size[a] = leader(parent_or_size[a]);
    }

    int size(int a) {
        assert(0 <= a && a < _n);
        return -parent_or_size[leader(a)];
    }

    std::vector<std::vector<int>> groups() {
        std::vector<int> leader_buf(_n), group_size(_n);
        for (int i = 0; i < _n; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<int>> result(_n);
        for (int i = 0; i < _n; i++) {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < _n; i++) {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [&](const std::vector<int>& v) { return v.empty(); }),
            result.end());
        return result;
    }

  private:
    int _n;
    // root node: -1 * component size
    // otherwise: parent
    std::vector<int> parent_or_size;
};

}  // namespace atcoder

#endif  // ATCODER_DSU_HPP




/* start */

Timer global_timer;
int N, L;
vector<int> Ts;
vector<float> TProps;
vector<float> currentTProps;
vector<int> removedIndices;
vector<int> removedTs;

const float kAlpha = 0.5;
int kUpdateCount = 1;

struct State {
  vector<vector<int>> ABs;
  vector<float> props;
  float cost = 0.0;

  State() {
    ABs.resize(N);
    rep(i, N) {
      ABs[i].resize(2);
      ABs[i][0] = (i + 1) % N;
      ABs[i][1] = (i + 1) % N;
    }

    props.resize(N);
    AllUpdate();
  }

  int RealScore() const {
    vector<int> visitCounts(N, 0);
    int current = 0;
    rep(_t, L) {
      visitCounts[current]++;
      current = ABs[current][(visitCounts[current] + 1) % 2];
    }

    int realCost = 0;
    rep(i, N) { realCost += abs(visitCounts[i] - Ts[i]); }
    for(const auto t : removedTs){
      realCost += t;
    }
    return (int)1e6 - realCost;
  }

  int GetTo(const int i, const int ab) const { return ABs[i][ab]; }

  void Change(const int i, const int ab, const int newTo) {
    const int oldTo = ABs[i][ab];
    cost -= CostAt(oldTo);
    props[oldTo] -= currentTProps[i] / 2.0;
    cost += CostAt(oldTo);
    cost -= CostAt(newTo);
    props[newTo] += currentTProps[i] / 2.0;
    cost += CostAt(newTo);
    ABs[i][ab] = newTo;
  }

  void Swap(const int i, const int ab_i, const int j, const int ab_j) {
    if(ABs[i][ab_i] == ABs[j][ab_j]){
      return;
    }
    assert(ABs[i][ab_i] != ABs[j][ab_j]);
    const int old_i = ABs[i][ab_i];
    const int old_j = ABs[j][ab_j];
    Change(i, ab_i, old_j);
    Change(j, ab_j, old_i);
  }

  float CostAt(const int idx) const {
    return abs(TProps[idx] - props[idx]) * L;
  }

  bool IsValid() const {
    // 全てのノードが強連結かを確認する
    // 0から全てのノードに到達でき、0に戻ってくるルートが存在すればOK
    vector<bool> visited(N, false);
    int visitedCount = 0;
    queue<int> q;
    q.push(0);
    while ((int)q.size() > 0) {
      int current = q.front();
      q.pop();
      rep(i, 2) {
        int next = ABs[current][i];
        if (!visited[next]) {
          visited[next] = true;
          visitedCount++;
          if (next != 0) {
            q.push(next);
          }
        }
      }
    }
    return visitedCount == N;
  }

  void AllUpdate() {
    cost = 0.0;
    rep(i, N){
      props[i] = 0.0;
    }
    rep(i, N){
      int a = ABs[i][0];
      int b = ABs[i][1];
      props[a] += currentTProps[i] / 2.0;
      props[b] += currentTProps[i] / 2.0;
    }
    rep(i, N){
      cost += abs(TProps[i] - props[i]) * L;
    }
    // float sum = 0.0;
    // rep(i, N){
    //   sum += props[i];
    // }
    // rep(i, N){
    //   props[i] /= sum;
    // }

    // vector<float> nextProps(N);
    // rep(i, N) { nextProps[i] = kAlpha * props[i]; }

    // rep(i, N) {
    //   int a = ABs[i][0];
    //   int b = ABs[i][1];
    //   nextProps[a] += (1 - kAlpha) * props[i] / 2.0;
    //   nextProps[b] += (1 - kAlpha) * props[i] / 2.0;
    // }

    // float sum = 0.0;
    // rep(i, N) { sum += nextProps[i]; }

    // rep(i, N) { nextProps[i] /= sum; }

    // props = nextProps;

    // TODO: 連結判定
  }

  float Cost() const {
    return cost;
  }

  int Score() const { return (int)(1e6 - Cost()); }

  void Print() const {
    // 除外したノードを戻してから出力
    auto newABs = ABs;
    rep(_i, removedIndices.size()){
      const int removedIdx = removedIndices[_i];
      newABs.insert(newABs.begin() + removedIdx, {removedIdx, removedIdx});
      N++;
      rep(j, N){
        if(removedIdx == j) continue;
        if(newABs[j][0] >= removedIdx) newABs[j][0]++;
        if(newABs[j][1] >= removedIdx) newABs[j][1]++;
      }
    }
    rep(i, N) { cout << newABs[i][0] << " " << newABs[i][1] << endl; }
  }
};

struct Output {
  static void StaticInit(istream &is) {
    global_timer.start();
    is >> N >> L;
    Ts.resize(N);
    TProps.resize(N);
    rep(i, N) {
      is >> Ts[i];
      TProps[i] = Ts[i] / (float)L;
    }

    // Tsが小さすぎるノードは除外
    reps(i, N-1){
      if(Ts[i] < 50){
        removedIndices.push_back(i);
        removedTs.push_back(Ts[i]);
      }
    }

    N -= removedIndices.size();
    REP(i, removedIndices.size()){
      Ts.erase(Ts.begin() + removedIndices[i]);
      TProps.erase(TProps.begin() + removedIndices[i]);
    }
    currentTProps = TProps;
  }
  friend ostream &operator<<(ostream &os, const Output &output) { return os; }
};



/* start */

vector<double> PARAMS = {5e2,3e0};



/* start */





// NBD = neighborhood
template <class Solution, class NBDGenerator> struct SimulatedAnnealing {
public:
  // kは0以下 (k = 0のときは線形, k = -∞のときは指数)
  SimulatedAnnealing(double end_time, double start_temp, double end_temp,
                     double k = 0.0)
      : end_time_(end_time * 1000), // ns
        inv_time_(1.0 / end_time_), cur_time_(0.0), start_temp_(start_temp),
        end_temp_(end_temp), diff_temp_(end_temp_ / start_temp_),
        cur_temp_(start_temp), k_(k) {
    assert(start_temp >= end_temp);
    timer_.start();
  }

  Solution run(const Solution &initial_sol, NBDGenerator &nbd_generator) {
    Solution sol(initial_sol);

    int acc = 0;
    int g = 0;
    for (;; ++g) {
      if ((g & 0xff) == 0) {
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
  double k_;
  Timer timer_;

  void UpdateTime() { cur_time_ = timer_.ns(); }
  void UpdateTemp() {
    // 指数
    cur_temp_ = start_temp_ * pow(diff_temp_, cur_time_ * inv_time_);
    // 線形
    // cur_temp_ = max(0.0, start_temp_ - cur_time_ * diff_temp_ * inv_time_);
    // 一般化
    // cur_temp_ = pow(start_temp_ - k_, 1 - cur_time_ * inv_time_) *
    //                 pow(end_temp_ - k_, cur_time_ * inv_time_) +
    //             k_;
  }
};





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
  // 最大化問題の場合は符号を反転すること
  float cost = 0;
  double Sum() const { return cost; }
};

struct Solution {
  State state;
  Evaluation eval;

  Solution() { InitEval(); }
  void InitEval() { eval.cost = state.Cost(); }
  static Solution CreateInitialSol() {
    Solution sol;
    return sol;
  }
  void UpdateCost() { eval.cost = state.Cost(); }
  double Eval() const { return eval.Sum(); }
  double Score() const { return 1e6 - Eval(); }
  friend ostream &operator<<(ostream &os, const Solution &sol) { return os; }
};

struct NBD {
  double Diff() const { return diff; }
  virtual void Update(Solution *) const = 0;
  virtual void Restore(Solution *) const = 0;
  double diff;
};

// 1点変更
struct NBD1 : public NBD {
  NBD1() {}
  NBD1(Solution *const sol) {
    diff = -sol->eval.Sum();
    oldEval = sol->eval;
    while (true) {
      i = rand(0, N);
      ab = rand(0, 2);
      oldTo = sol->state.GetTo(i, ab);
      newTo = rand(0, N);
      if (oldTo == newTo)
        continue;

      sol->state.Change(i, ab, newTo);
      sol->UpdateCost();
      diff += sol->eval.Sum();
      return;
    }
  }
  void Restore(Solution *sol) const override {
    sol->state.Change(i, ab, oldTo);
    sol->eval = oldEval;
  }

  void Update(Solution *sol) const override {
    // invalidなら戻す
    if(!sol->state.IsValid()){
      Restore(sol);
      return;
    }
  }

  int i;
  int ab;
  int oldTo;
  int newTo;
  Evaluation oldEval;
};

// スワップ
struct NBD2 : public NBD {
  NBD2() {}
  NBD2(Solution *const sol) {
    diff = -sol->eval.Sum();
    oldEval = sol->eval;
    while (true) {
      i = rand(0, N);
      ab_i = rand(0, 2);
      j = rand(0, N);
      ab_j = rand(0, 2);
      if (i == j)
        continue;

      sol->state.Swap(i, ab_i, j, ab_j);
      sol->UpdateCost();
      diff += sol->eval.Sum();
      return;
    }
  }
  void Restore(Solution *sol) const override {
    sol->state.Swap(i, ab_i, j, ab_j);
    sol->eval = oldEval;
  }

  void Update(Solution *sol) const override {
    // invalidなら戻す
    if(!sol->state.IsValid()){
      Restore(sol);
      return;
    }
  }

  int i;
  int ab_i;
  int j;
  int ab_j;
  Evaluation oldEval;
};

// 3点ローテーション
struct NBD3 : public NBD {
  NBD3() {}
  NBD3(Solution *const sol) {
    diff = -sol->eval.Sum();
    oldEval = sol->eval;
    while (true) {
      i = rand(0, N);
      ab_i = rand(0, 2);
      j = rand(0, N);
      ab_j = rand(0, 2);
      if (i == j)
        continue;
      k = rand(0, N);
      ab_k = rand(0, 2);
      if (i == k || j == k)
        continue;

      sol->state.Swap(i, ab_i, j, ab_j);
      sol->state.Swap(j, ab_j, k, ab_k);
      sol->UpdateCost();
      diff += sol->eval.Sum();
      return;
    }
  }
  void Restore(Solution *sol) const override {
    sol->state.Swap(j, ab_j, k, ab_k);
    sol->state.Swap(i, ab_i, j, ab_j);
    sol->eval = oldEval;
  }

  void Update(Solution *sol) const override {
    // invalidなら戻す
    if(!sol->state.IsValid()){
      Restore(sol);
      return;
    }
  }

  int i;
  int ab_i;
  int j;
  int ab_j;
  int k;
  int ab_k;
  Evaluation oldEval;
};

struct NBDGenerator {
  NBD *Generate(Solution *const sol, const int g, const double progress) {
    if(g % 100000 == 0){
      currentTProps = sol->state.props;
      sol->state.AllUpdate();
      sol->UpdateCost();
    }
    // if (g == 0 || sol->Score() > best_sol.Score()) {
    //   best_sol = *sol;
    // }
    // constexpr int kBeginUpdateCount = 10;
    // constexpr int kEndUpdateCount = 10;
    // kUpdateCount = kBeginUpdateCount + progress * (kEndUpdateCount - kBeginUpdateCount);
    const int random = rand(0, 1000);
    if (random < 100) {
      nbd1_ = NBD1(sol);
      return &nbd1_;
    } else if (random < 1000) {
      nbd2_ = NBD2(sol);
      return &nbd2_;
    } else if (random < -1) {
      nbd3_ = NBD3(sol);
      return &nbd3_;
    } else {
      assert(false);
    }
  }
  // const Solution& GetBestSol() const { return best_sol; }

private:
  NBD1 nbd1_;
  NBD2 nbd2_;
  NBD3 nbd3_;
  // Solution best_sol;
};

void Run() {
  Solution first_sol = Solution::CreateInitialSol();
  NBDGenerator gen;
  SimulatedAnnealing<Solution, NBDGenerator> SA(1950 - global_timer.ms(),
                                                PARAMS[0], PARAMS[1]);
  auto sa_final_sol = SA.run(first_sol, gen);
  cerr << "SA score: " << sa_final_sol.Score() << endl;
  auto bestState = sa_final_sol.state;
  int bestRealScore = bestState.RealScore();
  cerr << "SA real score: " << bestRealScore << endl;

  while (global_timer.ms() < 1990) {
    int i = rand(0, N);
    bestState.Swap(i, 0, i, 1);
    auto realScore = bestState.RealScore();
    if (realScore > bestRealScore) {
      bestRealScore = realScore;
    } else {
      bestState.Swap(i, 0, i, 1);
    }
  }

  cerr << "final real score: " << bestRealScore << endl;
  bestState.Print();
  return;
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