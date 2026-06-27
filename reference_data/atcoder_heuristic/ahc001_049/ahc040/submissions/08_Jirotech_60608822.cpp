
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

enum Dir{
  kU, kL
};

struct Pos{
  int x,y;
  Pos() : x(0), y(0) {}
  Pos(int x, int y) : x(x), y(y) {}
};

/* start */

Timer global_timer;
int N, T, sigma;

struct Action {
  int id;
  bool isRotate;
  Dir dir;
  int otherId;
  Action() {}
  Action(int id, bool isRotate, Dir dir, int otherId)
      : id(id), isRotate(isRotate), dir(dir), otherId(otherId) {}
};

struct Rect {
  int id, w, h;
  bool isRotate = false;

  Rect() {}
  Rect(int id, int w, int h) : id(id), w(w), h(h) {}
  void Rotate(){
    swap(w, h);
    isRotate = !isRotate;
  }
  int Idx() const { return id; }
  ll Area() const { return (ll)w * h; }
  int Min() const { return min(w, h); }
  int Max() const { return max(w, h); }
};

struct Response {
  int w, h;
  Response(int w, int h) : w(w), h(h) {}
  int Sum() const { return w + h; }
};

vector<Rect> inputRects;
int q = 0;

Response Query(const vector<Action> &actions) {
  if (q == T) {
    cerr << "Time is up" << endl;
    exit(1);
  }

  cout << actions.size() << endl;
  for (const auto &action : actions) {
    cout << action.id << " " << (action.isRotate ? 1 : 0) << " "
         << (action.dir == kU ? "U" : "L") << " " << action.otherId << endl;
  }
  q++;
  int w, h;
  cin >> w >> h;
  return Response(w, h);
}

struct Output {
  static void StaticInit(istream &is) {
    global_timer.start();
    is >> N >> T >> sigma;
    rep(i, N) {
      int w, h;
      is >> w >> h;
      inputRects.emplace_back(i, w, h);
    }
  }
  friend ostream &operator<<(ostream &os, const Output &output) { return os; }
};



/* start */

vector<double> PARAMS = {1944.6682036214052, 1059.0779202724052,
                         9545.564969083158,  23.01913581860427,
                         1.0553637452690036, 1.2507727225191285,
                         1440.9174908170182, 257.09523107109555,
                         7.99192785490137,   0.6608675327627342};



/* start */





/* start */

struct Line {
  int id;
  int y;
  int l, r;
  Line(int id, int y, int l, int r) : id(id), y(y), l(l), r(r) {}
  bool Intersect(int lll, int rrr) { return lll < r && l < rrr; }
};

struct FastBoard {
  // 左から詰めていくのみ
  int maxWidth = 0;
  int maxHeight = 0;
  int orderReverseCount = 0;
  vector<Line> lastLines;

  void AddLine(const vector<Rect> &rects, const bool doEdakari = true) {
    if (rects.empty())
      return;

    int l = 0;
    int maxR = 0;
    vector<Line> nextLastLines;
    rep(i, rects.size()) {
      const auto &rect = rects[i];
      int r = l + rect.w;
      int maxY = 0;
      // 交差する場合はmaxYを更新
      rep(nextLi, lastLines.size()) {
        auto lastLine = lastLines[nextLi];

        if (lastLine.Intersect(l, r)) {
          chmax(maxY, lastLine.y);
          // 自分より大きいRectと交差する場合、達成不可能
          if (lastLine.id > rect.id) {
            orderReverseCount++;
            // 枝狩り
            if(doEdakari) return;
          }
        }
      }

      // 次のLineとして追加
      // 測定誤差を考慮して幅を大きめに取る
      const double kAlpha = PARAMS[9];
      nextLastLines.emplace_back(rect.id, maxY + rect.h,
                                 l - kAlpha * sqrt(i) * sigma,
                                 r + kAlpha * sqrt(i + 1.0) * sigma);

      chmax(maxWidth, r);
      chmax(maxHeight, maxY + rect.h);
      l = r;
      chmax(maxR, nextLastLines.back().r);
    }
    // 覆い切れていない部分を反映
    rep(i, lastLines.size()) {
      const auto &line = lastLines[i];
      if (maxR < line.r) {
        nextLastLines.emplace_back(line.id, line.y, line.l, line.r);
        chmax(maxR, line.r);
      }
    }
    lastLines = nextLastLines;
  }
};





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
  Timer timer_;

  void UpdateTime() { cur_time_ = timer_.ns(); }
  void UpdateTemp() {
    cur_temp_ = start_temp_ * pow(diff_temp_, cur_time_ * inv_time_);
    // cur_temp_ = max(0.0, start_temp_ - cur_time_ * diff_temp_ * inv_time_);
  }
};




#include <algorithm>

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

// 左上から詰めていく解法
namespace NBD4 {

struct Evaluation {
  int width = 0;
  int height = 0;
  int orderReverseCount = 0;
  double Sum() const { return width + height + orderReverseCount * 1e9; }
};

/*
 * どこで折り返すか
 * 向きをどうするか
 */
struct Solution {
  Evaluation eval;
  vector<vector<Rect>> lines;

  Solution(vector<vector<Rect>> lines) : lines(lines) { InitEval(); }
  void InitEval() {
    FastBoard board;

    for (const auto &line : lines) {
      board.AddLine(line);
      // 枝狩り
      if (board.orderReverseCount > 0)
        break;
    }
    eval.width = board.maxWidth;
    eval.height = board.maxHeight;
    eval.orderReverseCount = board.orderReverseCount;
  }
  vector<Action> ToActions() const {
    vector<Action> actions;
    for (const auto &line : lines) {
      int prevId = -1;
      for (const auto &rect : line) {
        actions.emplace_back(rect.id, rect.isRotate, kU, prevId);
        prevId = rect.id;
      }
    }
    sort(all(actions),
         [](const Action &a, const Action &b) { return a.id < b.id; });
    return actions;
  }
  static Solution CreateInitialSolGreedy(const vector<Rect> estimatedRects,
                                         const double sumAreaFactor) {
    vector<vector<Rect>> lines;
    vector<ll> widthes;
    // 面積の合計
    ll sumArea = 0;
    rep(i, N) { sumArea += estimatedRects[i].Area(); }
    // 幅の目標値
    const ll targetWidth = sqrt(sumArea * sumAreaFactor);
    rep(i, N) {
      auto rect = estimatedRects[i];

      // 幅に収まるなら追加
      bool added = false;
      rep(j, lines.size()) {
        assert(!lines[j].empty());
        const auto &lastRect = lines[j].back();
        // 直前の高さとなるべくそろえる
        if (abs(lastRect.h - rect.h) > abs(lastRect.h - rect.w)) {
          rect.Rotate();
        }
        rep(t, 2) {
          // 下の方が長い場合は追加できない
          // TODO: 分散考慮
          if (j + 1 < (int)widthes.size() &&
              widthes[j + 1] + sigma * (int)lines[j + 1].size() * 2.0 >
                  widthes[j])
            break;
          if (widthes[j] + rect.w <= targetWidth) {
            lines[j].emplace_back(rect);
            widthes[j] += rect.w;
            added = true;
            break;
          }
          // 追加出来ない場合は、向きを変えて試す
          rect.Rotate();
        }
        if (added)
          break;
      }
      if (!added) {
        // 最初の長方形は、長い方を縦にする
        if (rect.w > rect.h)
          rect.Rotate();
        lines.emplace_back();
        lines.back().emplace_back(rect);
        widthes.emplace_back(rect.w);
      }
    }
    Solution sol(lines);
    return sol;
  }
  double Eval() const { return eval.Sum(); }
  double Score() const { return -Eval(); }
  friend ostream &operator<<(ostream &os, const Solution &sol) { return os; }
};

struct NBD {
  double Diff() const { return diff; }
  void Update(Solution *) const {};
  virtual void Restore(Solution *) const = 0;
  double diff;
};

// 向きの変更
struct NBD1 : public NBD {
  int i, j;
  Evaluation memoEval;
  NBD1() {}
  NBD1(Solution *const sol) {
    diff = -sol->Eval();
    memoEval = sol->eval;
    while (true) {
      i = rand(0, sol->lines.size());
      if ((int)sol->lines[i].size() == 0)
        continue;
      j = rand(0, sol->lines[i].size());
      sol->lines[i][j].Rotate();
      sol->InitEval();
      diff += sol->Eval();
      return;
    }
  }
  void Restore(Solution *sol) const override {
    sol->lines[i][j].Rotate();
    sol->eval = memoEval;
  }
};

// 移動
struct NBD2 : public NBD {
  int from_i, from_j, to_i, to_j;
  bool rot;
  Evaluation memoEval;
  NBD2() {}
  NBD2(Solution *const sol) {
    diff = -sol->Eval();
    memoEval = sol->eval;
    while (true) {
      from_i = rand(0, sol->lines.size());
      if ((int)sol->lines[from_i].size() == 0)
        continue;
      from_j = rand(0, sol->lines[from_i].size());
      auto rect = sol->lines[from_i][from_j];

      to_i = rand_other_than(0, sol->lines.size(), from_i);
      if (abs(from_i - to_i) >= 2)
        continue;
      to_j = 0;
      // 昇順に並ぶように挿入
      while (to_j < (int)sol->lines[to_i].size() &&
             sol->lines[to_i][to_j].id < rect.id)
        to_j++;
      rot = rand(0, 2) == 0;

      sol->lines[from_i].erase(sol->lines[from_i].begin() + from_j);
      if (rot)
        rect.Rotate();
      sol->lines[to_i].insert(sol->lines[to_i].begin() + to_j, rect);

      sol->InitEval();
      diff += sol->Eval();
      return;
    }
  }
  void Restore(Solution *sol) const override {
    auto rect = sol->lines[to_i][to_j];
    sol->lines[to_i].erase(sol->lines[to_i].begin() + to_j);
    if (rot)
      rect.Rotate();
    sol->lines[from_i].insert(sol->lines[from_i].begin() + from_j, rect);
    sol->eval = memoEval;
  }
};

// スワップ
struct NBD3 : public NBD {
  int i0, j0, i1, j1;
  vector<Rect> line0, line1;
  bool rot0, rot1;
  Evaluation memoEval;
  NBD3() {}
  NBD3(Solution *const sol) {
    diff = -sol->Eval();
    memoEval = sol->eval;
    rep(_t, 100) {
      i0 = rand(0, sol->lines.size());
      if ((int)sol->lines[i0].size() == 0)
        continue;
      j0 = rand(0, sol->lines[i0].size());
      rot0 = rand(0, 2) == 0;

      i1 = rand_other_than(0, sol->lines.size(), i0);
      if ((int)sol->lines[i1].size() == 0)
        continue;
      j1 = rand(0, sol->lines[i1].size());
      rot1 = rand(0, 2) == 0;

      if (abs(i0 - i1) >= 4)
        continue;
      auto rect0 = sol->lines[i0][j0];
      auto rect1 = sol->lines[i1][j1];
      if (abs(rect0.id - rect1.id) > sqrt(N))
        continue;

      if (rot0)
        rect0.Rotate();
      if (rot1)
        rect1.Rotate();
      line0 = sol->lines[i0];
      line1 = sol->lines[i1];
      sol->lines[i0][j0] = rect1;
      sol->lines[i1][j1] = rect0;
      // TODO: 高速化
      sort(all(sol->lines[i0]),
           [](const Rect &a, const Rect &b) { return a.id < b.id; });
      sort(all(sol->lines[i1]),
           [](const Rect &a, const Rect &b) { return a.id < b.id; });

      sol->InitEval();
      diff += sol->Eval();
      return;
    }
    diff = LINF;
  }
  void Restore(Solution *sol) const override {
    if (diff == LINF)
      return;
    // TODO: 高速化
    sol->lines[i0] = line0;
    sol->lines[i1] = line1;

    sol->eval = memoEval;
  }
};

vector<double> nbdProbSums = {0, 0, 0};

struct NBDGenerator {
  NBD *Generate(Solution *const sol, const int g, const double progress) {
    const int random = rand(0, 10000);
    if (random < nbdProbSums[0]) {
      nbd1_ = NBD1(sol);
      return &nbd1_;
    } else if (random < nbdProbSums[1]) {
      nbd2_ = NBD2(sol);
      return &nbd2_;
    } else if (random < 10000) {
      nbd3_ = NBD3(sol);
      return &nbd3_;
    } else {
      assert(false);
    }
  }

private:
  NBD1 nbd1_;
  NBD2 nbd2_;
  NBD3 nbd3_;
};

void Run() {
  rep(i, 3) { nbdProbSums[i] = PARAMS[i]; }
  rep(i, 2) { nbdProbSums[i + 1] += nbdProbSums[i]; }
  rep(i, 3) { nbdProbSums[i] = nbdProbSums[i] * 10000 / nbdProbSums[2]; }

  {
    // 雑に平均値を取って、推定精度を向上させる
    vector<vector<Rect>> responses(N);
    rep(i, N) {
      responses[i].emplace_back(i, inputRects[i].w, inputRects[i].h);
    }
    rep(_t, T) {
      if (T - q <= PARAMS[3])
        break;
      // 分散の合計値が最大のものを選択(あまり意味なさそう)
      int maxId = -1;
      pair<int, double> maxCountVar = {-INF, 0};
      rep(i, N) {
        double sumW = 0;
        double sumH = 0;
        double sum2W = 0;
        double sum2H = 0;
        for (const auto &res : responses[i]) {
          sumW += res.w;
          sumH += res.h;
          sum2W += (double)res.w * res.w;
          sum2H += (double)res.h * res.h;
        }
        const double varW =
            sum2W / responses[i].size() -
            (sumW / responses[i].size()) * (sumW / responses[i].size());
        const double varH =
            sum2H / responses[i].size() -
            (sumH / responses[i].size()) * (sumH / responses[i].size());
        const pair<int, double> countVar = {-(int)responses[i].size(),
                                            varW + varH};
        if (chmax(maxCountVar, countVar)) {
          maxId = i;
        }
      }
      vector<Action> actions;
      actions.emplace_back(maxId, false, kU, -1);
      const auto res = Query(actions);
      responses[maxId].emplace_back(maxId, res.w, res.h);
    }

    // 平均値をとる
    double mean = 0;
    int count = 0;
    rep(i, N) {
      for (const auto &res : responses[i]) {
        mean += res.w;
        mean += res.h;
        count += 2;
      }
    }
    mean /= count;
    // 平均値から最低値を決め打つ
    int minValue = 1e5 - (1e5 - mean) * 2;
    // minValueをクリッピング
    minValue = std::clamp(minValue, (int)1e4, (int)5e4);

    // 最大値と最小値でクリッピング
    rep(i, N) {
      for (auto &res : responses[i]) {
        res.w = std::clamp(res.w, minValue, (int)1e5);
        res.h = std::clamp(res.h, minValue, (int)1e5);
      }
    }

    // 平均値で置き換える
    rep(i, N) {
      const auto &reses = responses[i];
      if ((int)reses.size() == 0)
        continue;

      int sumW = 0;
      int sumH = 0;
      for (const auto &res : reses) {
        sumW += res.w;
        sumH += res.h;
      }
      inputRects[i].w = sumW / (int)reses.size();
      inputRects[i].h = sumH / (int)reses.size();
      // 上でクリッピング済みなので多分不要だけど一応
      chmin(inputRects[i].w, 100000);
      chmin(inputRects[i].h, 100000);
      chmax(inputRects[i].w, 10000);
      chmax(inputRects[i].h, 10000);
    }
  }
  const int remainT = T - q;
  const int searchCount = T - q - PARAMS[8];
  int ttt = 0;
  vector<pair<int, Solution>> allSolutions;
  for (; q < T;) {
    cerr << "turn: " << q + 1 << endl;
    auto estimatedRects = inputRects;
    const double kSumAreaFactorBegin = PARAMS[4];
    const double kSumAreaFactorEnd = PARAMS[5];
    auto first_sol = Solution::CreateInitialSolGreedy(
        estimatedRects,
        kSumAreaFactorBegin + (kSumAreaFactorEnd - kSumAreaFactorBegin) * ttt /
                                  (double)max(1, searchCount - 1));
    if (allSolutions.size() >= PARAMS[8]) {
      sort(all(allSolutions),
           [](const auto &a, const auto &b) { return a.first < b.first; });
      first_sol = allSolutions.front().second;
    }
    NBDGenerator gen;
    SimulatedAnnealing<Solution, NBDGenerator> SA(2900 / remainT, PARAMS[6],
                                                  PARAMS[7]);
    const auto final_sol = SA.run(first_sol, gen);
    cerr << "estimated cost: " << final_sol.Eval() << " "
         << "(w,h) = " << final_sol.eval.width << " " << final_sol.eval.height
         << endl;
    cout << "# estimated cost: " << final_sol.Eval() << " "
         << "(w,h) = " << final_sol.eval.width << " " << final_sol.eval.height
         << endl;
    const auto actions = final_sol.ToActions();
    const auto res = Query(actions);
    allSolutions.emplace_back(res.Sum() * 1e-1 + final_sol.eval.Sum(), final_sol);
    cerr << "return cost: " << res.Sum() << " " << "(w,h) = " << res.w << " "
         << res.h << endl;
  }
  cerr << "Time: " << global_timer.ms() << " [ms]" << endl;
  std::quick_exit(0);
}

} // namespace NBD4

/* start */

class Solver {
public:
  Solver() {}
  void Solve() {
    NBD4::Run();
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