

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




enum Dir {
  // y-1, y+1, x-1, x+1
  kU,
  kD,
  kL,
  kR
};

Dir RightDir(const Dir dir) {
  constexpr Dir kRightDirs[4] = {kR, kL, kU, kD};
  return kRightDirs[dir];
}

Dir InvDir(const Dir dir) {
  constexpr Dir kInvDirs[4] = {kD, kU, kR, kL};
  return kInvDirs[dir];
}

struct Pos {
  int idx_;
  Pos() {}
  explicit Pos(const int _idx) : idx_(_idx) {}
  Pos(int _x, int _y) : idx_(_y * N + _x) { assert(N > 0); }
  int X() const { return pos_2_x[*this]; }
  int Y() const { return pos_2_y[*this]; }
  int Idx() const { return idx_; }
  operator int() const { return Idx(); }
  operator size_t() const { return Idx(); }
  const vector<Pos>& Adj() const { return adj_poses[*this]; }
  const vector<Dir>& AdjDirs() const { return adj_dirs[*this]; }
  int Manhattan(const Pos& other) const {
    return abs(X() - other.X()) + abs(Y() - other.Y());
  }
  int Euclid2(const Pos& other) const {
    const int dx = X() - other.X();
    const int dy = Y() - other.Y();
    return dx * dx + dy * dy;
  }
  bool Move(const Dir dir) {
    if (move_to[dir][*this].IsDummy()) {
      return false;
    } else {
      *this = move_to[dir][*this];
      return true;
    }
  }
  bool operator<(const Pos& other) const { return this->Idx() < other.Idx(); }
  bool operator==(const Pos& other) const { return this->Idx() == other.Idx(); }
  bool operator!=(const Pos& other) const { return this->Idx() != other.Idx(); }
  friend ostream& operator<<(ostream& os, const Pos& pos) {
    os << pos.X() << " " << pos.Y();
    return os;
  }
  bool IsDummy() const { return this->idx_ < 0; }
  static Pos Dummy() {
    Pos p;
    p.idx_ = -1;
    return p;
  }

  static Pos TryCreate(const int x, const int y) {
    if (y < 0 || y >= N || x < 0 || x >= N) {
      return Pos::Dummy();
    } else {
      return Pos(x, y);
    }
  }

  static void StaticInit(const int n) {
    N = n;
    N2 = N * N;
    N4 = N2 * N2;
    adj_poses = vector<vector<Pos>>(N2);
    adj_dirs = vector<vector<Dir>>(N2);
    move_to = vector<vector<Pos>>(4, vector<Pos>(N2, Pos::Dummy()));
    pos_2_y.resize(N2);
    pos_2_x.resize(N2);

    rep(y, N) {
      rep(x, N) {
        const Pos p{x, y};
        all_poses.push_back(p);
        pos_2_y[p] = y;
        pos_2_x[p] = x;
        for (int dy = -1; dy <= 1; ++dy) {
          for (int dx = -1; dx <= 1; ++dx) {
            if (abs(dy) + abs(dx) != 1) continue;
            const int adj_y = y + dy;
            const int adj_x = x + dx;
            if (adj_y < 0 || adj_y >= N) continue;
            if (adj_x < 0 || adj_x >= N) continue;
            adj_poses[p].emplace_back(adj_x, adj_y);

            Dir dir;
            if (dy == -1) {
              dir = kU;
            } else if (dy == 1) {
              dir = kD;
            } else if (dx == -1) {
              dir = kL;
            } else if (dx == 1) {
              dir = kR;
            } else {
              assert(false);
            }

            move_to[dir][p] = {adj_x, adj_y};
            adj_dirs[p].emplace_back(dir);
          }
        }
      }
    }
    all_poses.shrink_to_fit();
  }
  static int N, N2, N4;
  static vector<Pos> all_poses;
  static vector<vector<Pos>> adj_poses;
  static vector<vector<Dir>> adj_dirs;
  static vector<vector<Pos>> move_to;
  static vector<int> pos_2_x, pos_2_y;
};

vector<Pos> Pos::all_poses;
vector<vector<Pos>> Pos::adj_poses;
vector<vector<Dir>> Pos::adj_dirs;
vector<vector<Pos>> Pos::move_to;
vector<int> Pos::pos_2_x, Pos::pos_2_y;
int Pos::N, Pos::N2, Pos::N4;




/* start */

Timer global_timer;
int N, M, K, T;
const int kStationCost = 5000;
const int kRailCost = 100;

struct Line {
  Pos p0, p1;
  Line(Pos p0, Pos p1) : p0(p0), p1(p1) {}

  pair<Line, Line> Split(const Pos pos) const {
    return {Line(p0, pos), Line(pos, p1)};
  }

  // p0からp1に最短経路で行く際に、通行可能なかどうか
  bool CanBetween(const Pos pos) const {
    return p0.Manhattan(pos) + pos.Manhattan(p1) == p0.Manhattan(p1);
  }
};

struct Human {
  Pos from;
  Pos to;

  // 通勤できた場合の収入
  int Income() const { return from.Manhattan(to); }
};

// マンハッタン距離が2以下の座標を列挙
vector<vector<Pos>> adj2Poses;

vector<Human> inputHumans;
// 人iの(from, to)に対応するハッシュ値
vector<pair<uint32_t, uint32_t>> humanHashes;

// 駅からFromに到達できる人のリスト
vector<vector<int>> station2Froms;
// 駅からToに到達できる人のリスト
vector<vector<int>> station2Tos;

// 各位置からその位置にいる人のリスト
vector<vector<int>> pos2Humans;

// 人を通勤させることが出来る駅の組を全列挙
vector<pair<Pos, Pos>> sortedValidStationPairs;

// ↑の収入も一緒に持ったもの
vector<pair<pair<Pos, Pos>, int>> sortedValidStationPairToIncome;

// 場所のハッシュ値
vector<uint32_t> posHashes;

// fromから見たtoの方向
Dir CalcDir(Pos from, Pos to) {
  assert((from.X() == to.X()) ^ (from.Y() == to.Y()));
  if (from.X() == to.X()) {
    if (from.Y() < to.Y()) {
      return kD;
    } else {
      return kU;
    }
  } else {
    if (from.X() < to.X()) {
      return kR;
    } else {
      return kL;
    }
  }
}

Pos MovePos(Pos pos, Dir dir) {
  if (dir == kU) {
    return Pos(pos.X(), pos.Y() - 1);
  }
  if (dir == kD) {
    return Pos(pos.X(), pos.Y() + 1);
  }
  if (dir == kL) {
    return Pos(pos.X() - 1, pos.Y());
  }
  if (dir == kR) {
    return Pos(pos.X() + 1, pos.Y());
  }
  assert(false);
}

enum CommandKind { kRail, kStation, kPass };
// 出力する行動
struct Command {
  CommandKind kind;
  Pos p;
  Dir dir0;
  Dir dir1;

  Command() {}
  Command(CommandKind kind, Pos p, Dir dir0, Dir dir1)
      : kind(kind), p(p), dir0(dir0), dir1(dir1) {}

  int Cost() const {
    if (kind == kRail) {
      return kRailCost;
    } else if (kind == kStation) {
      return kStationCost;
    } else {
      return 0;
    }
  }

  static Command Station(Pos p) { return Command(kStation, p, kU, kU); }

  static Command Rail(Pos prevPos, Pos currentPos, Pos nextPos) {
    const Dir dir0 = CalcDir(currentPos, prevPos);
    const Dir dir1 = CalcDir(currentPos, nextPos);
    return Command(kRail, currentPos, dir0, dir1);
  }

  static Command Pass() { return Command(kPass, Pos::Dummy(), kU, kU); }

  string ToString() const {
    if (kind == kRail) {
      const bool up = dir0 == kU || dir1 == kU;
      const bool down = dir0 == kD || dir1 == kD;
      const bool left = dir0 == kL || dir1 == kL;
      const bool right = dir0 == kR || dir1 == kR;
      int railNumber = -1;
      if (left && right) {
        railNumber = 1;
      } else if (up && down) {
        railNumber = 2;
      } else if (left && down) {
        railNumber = 3;
      } else if (up && left) {
        railNumber = 4;
      } else if (up && right) {
        railNumber = 5;
      } else if (right && down) {
        railNumber = 6;
      } else {
        assert(false);
      }
      return to_string(railNumber) + " " + to_string(p.Y()) + " " +
             to_string(p.X());
    } else if (kind == kStation) {
      return "0 " + to_string(p.Y()) + " " + to_string(p.X());
    } else {
      return "-1";
    }
  }
};

bool HasEnoughMoney(CommandKind commandKind, int money) {
  if (commandKind == kRail) {
    return money >= kRailCost;
  } else if (commandKind == kStation) {
    return money >= kStationCost;
  } else {
    return true;
  }
}

struct Output {
  static void StaticInit(istream &is) {
    global_timer.start();
    is >> N >> M >> K >> T;
    Pos::StaticInit(N);
    inputHumans.resize(M);
    station2Froms.resize(N * N);
    station2Tos.resize(N * N);
    pos2Humans.resize(N * N);

    map<pair<Pos, Pos>, int> validStationPairToIncomes;
    for (int i = 0; i < M; i++) {
      int sy, sx, ty, tx;
      is >> sy >> sx >> ty >> tx;
      const auto from = Pos(sx, sy);
      const auto to = Pos(tx, ty);
      inputHumans[i].from = from;
      inputHumans[i].to = to;
      pos2Humans[from].push_back(i);
      pos2Humans[to].push_back(i);

      rep(y, N) {
        rep(x, N) {
          const auto pos = Pos(x, y);
          if (from.Manhattan(pos) <= 2) {
            // 到達可能
            station2Froms[pos].push_back(i);
          }
          if (to.Manhattan(pos) <= 2) {
            // 到達可能
            station2Tos[pos].push_back(i);
          }
        }
      }
    }

    // 人を通勤させることが出来る駅の組を全列挙
    rep(y, N) {
      rep(x, N) {
        const auto from = Pos(x, y);
        for (const auto human : station2Froms[from]) {
          const auto to = inputHumans[human].to;
          for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
              const int yyy = to.Y() + dy;
              const int xxx = to.X() + dx;
              if (yyy < 0 || yyy >= N || xxx < 0 || xxx >= N)
                continue;
              const auto to2 = Pos(xxx, yyy);
              if (to2.Manhattan(to) <= 2) {
                auto p0 = from;
                auto p1 = to2;
                if (p0.Idx() > p1.Idx()) {
                  swap(p0, p1);
                }
                validStationPairToIncomes[{p0, p1}] +=
                    inputHumans[human].Income();
              }
            }
          }
        }
      }
    }

    rep(i, M) { humanHashes.emplace_back(xorshift(), xorshift()); }

    rep(i, N * N) { posHashes.push_back(xorshift()); }

    // Incomeが大きい順にソート
    for (const auto &[pair, income] : validStationPairToIncomes) {
      sortedValidStationPairToIncome.emplace_back(pair, income);
    }
    sort(all(sortedValidStationPairToIncome),
         [&](const auto &a, const auto &b) { return a.second > b.second; });
    for (int i = 0; i < (int)sortedValidStationPairToIncome.size(); i++) {
      sortedValidStationPairs.push_back(
          sortedValidStationPairToIncome[i].first);
    }
    cerr << "validStationPairs: " << sortedValidStationPairs.size() << endl;

    // マンハッタン距離が2以下の座標を列挙
    adj2Poses.resize(N * N);
    rep(i, N * N) {
      const Pos p(i);
      for (int dy = -2; dy <= 2; dy++) {
        for (int dx = -2; dx <= 2; dx++) {
          const int yyy = p.Y() + dy;
          const int xxx = p.X() + dx;
          if (yyy < 0 || yyy >= N || xxx < 0 || xxx >= N)
            continue;
          const auto to = Pos(xxx, yyy);
          if (p.Manhattan(to) <= 2) {
            adj2Poses[i].push_back(to);
          }
        }
      }
    }
    return;
  }
  friend ostream &operator<<(ostream &os, const Output &output) { return os; }
};



/* start */

// vector<double> PARAMS = {300.0, 5.0, 60,    20,   5,    15,    10,
//                          10,    0.3, 0.999, 2000, 10.0, 200.0, 50};
vector<double> PARAMS = {443.37229912860846, 1.601271868072543,
                         98.38284812651281,  85.39622163898689,
                         91.54579304786638,  91.86675267429274,
                         33.70296292033233,  0.0,
                         0.3124461068545914, 0.9967328054545139,
                         1387.0056324817542, 2.059663858693012,
                         673.4397379337325,  65.85629943345863,
                        1100.0};



/* start */




/* start */

struct MoneyStateBSAction {
  int passTurn;
  Pos stationPos;
  int railCount;
  bool isPaidPass;

  MoneyStateBSAction() {}
  MoneyStateBSAction(int passTurn, Pos stationPos, int railCount, bool isPaidPass)
      : passTurn(passTurn), stationPos(stationPos), railCount(railCount), isPaidPass(isPaidPass) {}
};

struct StateInfoAction {
  int passTurn;
  int incIncome;
  int railCount;
  int usedPaidPassCount;
  bool isPaidPass;
  StateInfoAction() {}
  StateInfoAction(int passTurn, int incIncome, int railCount, int usePaidPassCount, bool isPaidPass)
      : passTurn(passTurn), incIncome(incIncome), railCount(railCount),
        usedPaidPassCount(usePaidPassCount), isPaidPass(isPaidPass) {}
  bool Invalid() const { return passTurn > T; }
};

// 数値的な情報
struct StateInfo {
  // 1ターン当たりの収入
  int income = 0;
  // 半分達成している収入候補
  int halfIncome = 0;
  // 現在保有しているお金
  int money = K;
  // 現在のターン
  int turn = 0;
  // 100円払ってパスした回数
  // 後で線路に置き換えることができる
  int paidPassCount = 0;

  int FinalMoney() const { return money + income * (T - turn) + paidPassCount * kRailCost; }

  // 駅を建てるために必要なパスの回数
  int NeedPassTurnToBuildStation(const int railCount, const bool isPaidPass) const {
    assert(!isPaidPass || income > kRailCost);
    // 線路数 + 駅
    const int buildTurnCount = max(0, railCount - paidPassCount) + 1;
    // 建設コスト
    const int costSum = max(0, railCount - paidPassCount) * kRailCost + kStationCost;
    if (income == 0 && costSum > money) {
      // パスしてもお金がたまらない
      return INF;
    }
    // 建設コストがたまるには何ターン待つか
    // income == 0の場合は、お金が足りていることを上で確認している
    // paidPassの場合は、moneyが5000になるまで、(income - 100)ずつ増えていく
    const int moneyWaitTurnCount =
        income == 0 ? 0 : isPaidPass ? Ceil(max(0, kStationCost - money), income - kRailCost) : Ceil(max(0, costSum - money), income);

    // 建築完了までに必要なターン数
    const int additionalTurn = max(buildTurnCount, moneyWaitTurnCount + 1);

    // パスすべきターン数
    const int passTurn = max(0, additionalTurn - buildTurnCount);
    return passTurn;
  }

  void Pass(const int passTurn, const bool isPaidPass) {
    // 集金フェーズ
    money += income * passTurn;
    // ターン経過
    turn += passTurn;
    if(isPaidPass){
      paidPassCount += passTurn;
      money -= passTurn * kRailCost;
    }
  }

  void RollbackPass(const int passTurn, const bool isPaidPass) {
    // ターン経過
    turn -= passTurn;
    // 集金フェーズ
    money -= income * passTurn;
    if(isPaidPass){
      paidPassCount -= passTurn;
      money += passTurn * kRailCost;
    }
  }

  int GetUsePaidPassCount(const int railCount) const{
    return min(railCount, paidPassCount);
  }

  // 使ったpaidPassの個数を返す
  int BuildStation(const int incIncome, const int railCount, const int usePaidPassCount) {
    assert(railCount >= usePaidPassCount);
    // 建設完了までにかかるターン
    const int additionalTurn = railCount - usePaidPassCount + 1;

    // それまでに得られる収入
    money += income * (additionalTurn - 1);
    // 支払うコスト
    money -= kStationCost + kRailCost * (railCount - usePaidPassCount);

    // 収入増加
    income += incIncome;

    // 建設完了後の集金フェーズ
    money += income;

    // ターン経過
    turn += additionalTurn;

    // 使ったpaidPassの分だけ減らす
    paidPassCount -= usePaidPassCount;

    return usePaidPassCount;
  }

  void RollbackBuildStation(const int incIncome, const int railCount, const int usedPaidPassCount) {
    assert(railCount >= usedPaidPassCount);
    // 使ったpaidPassを戻す
    paidPassCount += usedPaidPassCount;

    // 建設完了までにかかるターン
    const int additionalTurn = (railCount - usedPaidPassCount) + 1;

    // ターン経過
    turn -= additionalTurn;

    // 建設完了後の集金フェーズ
    money -= income;

    // 収入減少
    income -= incIncome;

    // 支払うコスト
    money += kStationCost + kRailCost * (railCount - usedPaidPassCount);
    // それまでに得られる収入
    money -= income * (additionalTurn - 1);
  }

  void Forward(const StateInfoAction &action) {
    Pass(action.passTurn, action.isPaidPass);
    BuildStation(action.incIncome, action.railCount, action.usedPaidPassCount);
  }

  void Rollback(const StateInfoAction &action) {
    RollbackBuildStation(action.incIncome, action.railCount, action.usedPaidPassCount);
    RollbackPass(action.passTurn, action.isPaidPass);
  }
};

// 駅にカバーされているかどうかをbitsetで管理することで、お金を管理する状態
// MoneyStateよりもコピーが高速となる
struct MoneyStateBS {
  // 駅にカバーされているか
  bitset<50 * 50> isCovered;

  StateInfo info;

  MoneyStateBS() : isCovered(0) {}

  // 指定の本数の線路と指定の位置に駅を建てるためのアクションを生成する
  MoneyStateBSAction CreateAction(const Pos stationPos,
                                  const int railCount,
                                 const bool isPaidPass) const {
    const int passTurn = NeedPassTurnToBuildStation(railCount, isPaidPass);
    return MoneyStateBSAction(passTurn, stationPos, railCount, isPaidPass);
  }

  // 駅を建てるために必要なパスの回数
  int NeedPassTurnToBuildStation(const int railCount, const bool isPaidPass) const {
    return info.NeedPassTurnToBuildStation(railCount, isPaidPass);
  }

  // 駅を建設する 内部状態の更新も行う
  // シミュレーション結果のStateInfoを返す
  StateInfo BuildStation(const Pos stationPos, const int railCount, const bool usePaidPass) {
    int incIncome = 0;
    int incHalfIncome = 0;

    // そこに駅を建てることでカバーされる位置を更新
    for (const auto adj : adj2Poses[stationPos]) {
      // 既にカバー済みなら何もしない
      if (isCovered[adj])
        continue;

      // カバーした位置に存在する人に対して処理
      isCovered[adj] = true;
      for (const auto humanId : pos2Humans[adj]) {
        const auto &human = inputHumans[humanId];
        const int coveredCount = isCovered[human.from] + isCovered[human.to];
        if (coveredCount == 1) {
          incHalfIncome += human.Income();
        } else if (coveredCount == 2) {
          incHalfIncome -= human.Income();
          incIncome += human.Income();
        } else {
          assert(false);
        }
      }
    }


    auto nextInfo = info;
    const int usePaidPassCount = usePaidPass ? nextInfo.GetUsePaidPassCount(railCount) : 0;
    nextInfo.BuildStation(incIncome, railCount, usePaidPassCount);
    nextInfo.halfIncome += incHalfIncome;
    assert(nextInfo.money >= 0);

    info = nextInfo;

    return nextInfo;
  }

  // 駅を建設するシミュレーション結果のStateInfoを返す
  // 内部状態の更新は行わない
  StateInfo BuildStationNoUpdate(const Pos stationPos, const int railCount,
                                 StateInfo baseInfo, const bool usePaidPass) const {
    auto nextInfo = baseInfo;
    int incIncome = 0;
    int incHalfIncome = 0;

    // そこに駅を建てることでカバーされる位置を更新
    for (const auto adj : adj2Poses[stationPos]) {
      // 既にカバー済みなら何もしない
      if (isCovered[adj])
        continue;

      for (const auto humanId : pos2Humans[adj]) {
        const auto &human = inputHumans[humanId];
        // NOTE: 1手でfromとtoをカバーすることはないので、+1で良い
        const int coveredCount =
            isCovered[human.from] + isCovered[human.to] + 1;
        if (coveredCount == 1) {
          incHalfIncome += human.Income();
        } else if (coveredCount == 2) {
          incHalfIncome -= human.Income();
          incIncome += human.Income();
        } else {
          assert(false);
        }
      }
    }

    const int usePaidPassCount = usePaidPass ? nextInfo.GetUsePaidPassCount(railCount) : 0;
    nextInfo.BuildStation(incIncome, railCount, usePaidPassCount);
    nextInfo.halfIncome += incHalfIncome;
    assert(nextInfo.money >= 0);

    return nextInfo;
  }

  void Pass(const int passTurn, const bool isPaidPass) { info.Pass(passTurn, isPaidPass); }

  int RemainTurn() const { return T - info.turn; }

  ll FinalMoney() const { return info.money + (ll)RemainTurn() * info.income; }

  void Forward(const MoneyStateBSAction &action) {
    Pass(action.passTurn, action.isPaidPass);
    // パスしたらなら、paidPassは使わない
    BuildStation(action.stationPos, action.railCount, action.passTurn == 0);
  }

  StateInfo Simulate(const MoneyStateBSAction &action) const {
    auto nextInfo = info;
    // パス
    nextInfo.Pass(action.passTurn, action.isPaidPass);
    // 駅の建設
    // パスしたらなら、paidPassは使わない
    nextInfo =
        BuildStationNoUpdate(action.stationPos, action.railCount, nextInfo, action.passTurn == 0);
    return nextInfo;
  }
};



/* start */

// Keyはintへの変換を持つ
template<typename Key, typename Value>
struct IndexDictionary {
  vector<Key> keys;
  vector<Value> values;
  vector<int> pos;
  Value defaultValue;

  IndexDictionary(int n) : pos(n, -1) {}

  void add(Key key, Value value) {
    pos[(int)key] = keys.size();
    keys.push_back(key);
    values.push_back(value);
  }

  void remove(Key v) {
    int p = pos[(int)v];
    Key b = keys.back();
    keys[p] = b;
    keys.pop_back();
    Value value = values.back();
    values[p] = value;
    values.pop_back();
    pos[b] = p;
    pos[(int)v] = -1;
  }

  Value get(Key key) const{
    return values[pos[(int)key]];
  }

  void set(Key key, Value value){
    if(contains(key)){
      int p = pos[(int)key];
      values[p] = value;
    }else{
      add(key, value);
    }
  }

  bool contains(Key v) const {
    return pos[(int)v] != -1;
  }

  int size() const {
    return keys.size();
  }

  void clear(){
    for(const auto key : keys){
      pos[(int)key] = -1;
    }
    keys.clear();
    values.clear();
  }
};




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

namespace NBD {

// dp用の入れ物
// 使いまわす用
IndexDictionary<Pos, float> dpGlobal(50 * 50);
IndexDictionary<Pos, Pos> prevsGlobal(50 * 50);

struct Station {
  int id;
  int order;
  Pos pos;
  int parentId;
  vector<Pos> rails;
  vector<int> children;

  Station() {}
  Station(int id, int order, Pos pos, int parentId)
      : id(id), order(order), pos(pos), parentId(parentId) {}

  void SetRails(const vector<Pos> &rails) { this->rails = rails; }

  void AddChild(const int childId) { children.push_back(childId); }

  void RemoveChild(const int childId) {
    const auto itr = find(all(children), childId);
    assert(itr != children.end());
    children.erase(itr);
  }

  void RemoveChildIfExists(const int childId) {
    const auto itr = find(all(children), childId);
    if (itr == children.end()) {
      return;
    }
    children.erase(itr);
  }
};

struct Board {
  vector<int> pos2StationIds;
  vector<int> pos2RailIds;

  Board() {}
  explicit Board(const vector<Station> &stations)
      : pos2StationIds(N * N, -1), pos2RailIds(N * N, -1) {
    for (const auto &station : stations) {
      pos2StationIds[station.pos] = station.id;
      for (const auto &rail : station.rails) {
        pos2RailIds[rail] = station.id;
      }
    }
  }
};

struct State {
  // id順(order順ではない)
  vector<Station> stations;
  Board board;
  // idを立てる順に並べたもの
  vector<int> stationOrders;
  int paidPassIncomeThreshold = INF;

  State() {}
  State(const vector<Station> &stations, const vector<int> &stationOrders,
        const int paidPassIncomeThreshold)
      : stations(stations), board(stations), stationOrders(stationOrders),
        paidPassIncomeThreshold(paidPassIncomeThreshold) {}

  // 指定の位置に駅を立てる
  // 順序は最後とする
  // 後で線路の構築・順序swapを呼ぶこと
  // 割り当てたIDを返す
  int PushStation(const Pos pos) {
    assert(board.pos2StationIds[pos] == -1);
    const int nextStationId = stations.size();
    const int nextStationOrder = stationOrders.size();
    const int parentId = -1;
    auto station = Station(nextStationId, nextStationOrder, pos, parentId);
    stations.push_back(station);
    stationOrders.push_back(nextStationId);
    board.pos2StationIds[pos] = nextStationId;
    return nextStationId;
  }

  // 指定の駅を削除する
  void EraseStation(const int stationId) {
    auto &station = stations[stationId];
    // 子を空にしてから呼ぶこと
    assert(station.children.empty());

    // 線路を削除してから呼ぶこと
    assert(station.rails.empty());
    assert(station.parentId == -1);

    // 順序列から削除
    stationOrders.erase(stationOrders.begin() + station.order);
    for (int i = station.order; i < (int)stationOrders.size(); i++) {
      stations[stationOrders[i]].order = i;
    }

    board.pos2StationIds[station.pos] = -1;
    station.pos = Pos::Dummy();
  }

  // 削除された駅に割り振られたIDを開放し、stationIDを連番にする
  // 定期的に呼び出す
  void GC() {
    vector<Station> newStations;
    vector<int> newStationOrders;
    unordered_map<int, int> oldId2NewIds;
    for (const auto oldStationId : stationOrders) {
      auto station = stations[oldStationId];
      const int newStationId = newStations.size();
      oldId2NewIds[oldStationId] = newStationId;
      station.id = newStationId;
      newStations.push_back(station);
      newStationOrders.push_back(newStationId);
    }

    // 親と子のIDを書き換える
    for (auto &station : newStations) {
      station.parentId = oldId2NewIds[station.parentId];
      for (auto &childId : station.children) {
        childId = oldId2NewIds[childId];
      }
    }

    // 更新
    stations = newStations;
    stationOrders = newStationOrders;

    // ボードも更新
    board = Board(stations);
  }

  vector<Command> ConvertToCommands() const {
    vector<Command> commands;
    for (const auto stationId : stationOrders) {
      const auto &station = stations[stationId];
      for (const auto rail : station.rails) {
        // 向きは適当
        commands.push_back(Command(kRail, rail, kU, kU));
      }
      commands.push_back(Command::Station(station.pos));
    }
    return commands;
  }

  // パス未挿入のコマンド列からStateを生成
  // コマンドは、線路の後に駅が来ることを前提とする
  // 線路は、既にある駅から伸ばす順番になっていることを前提としている
  static State CreateByCommands(const vector<Command> &commands) {
    vector<Station> stations;
    // 初期状態のstationIdとorderは一致
    vector<int> stationOrders;
    int nextStationId = 0;
    vector<Pos> railPoses;
    vector<int> pos2StationIds(N * N, -1);
    vector<int> pos2RailIds(N * N, -1);
    for (const auto &command : commands) {
      if (command.kind == kRail) {
        railPoses.push_back(command.p);
        continue;
      }

      assert(command.kind == kStation);
      const auto stationPos = command.p;
      int parentId = -1;
      if (nextStationId == 0) {
        // 最初の駅は親無し
      } else if ((int)railPoses.size() == 0) {
        // 線路を立てていない場合は、線路上に立てているか、駅に隣接して立てている

        // 線路
        int parentRailId = pos2RailIds[stationPos];
        if (parentRailId == -1) {
          parentRailId = INF;
        }

        // 駅
        int parentStationId = INF;
        // 周囲にある駅の位置を取得し、それを親とする
        for (const auto adj : stationPos.Adj()) {
          if (pos2StationIds[adj] != -1) {
            // 順番が早い親にしたほうが得
            chmin(parentStationId, pos2StationIds[adj]);
          }
        }

        // 若い方を親にする
        assert(parentRailId != INF || parentStationId != INF);
        parentId = min(parentRailId, parentStationId);
      } else {
        // 最初の線路に隣接している駅が親
        parentId = INF;
        // 若いほうが良い
        for (const auto adj : railPoses[0].Adj()) {
          if (pos2StationIds[adj] != -1) {
            chmin(parentId, pos2StationIds[adj]);
          }
        }
        assert(parentId != INF);
      }

      auto station =
          Station(nextStationId, nextStationId, stationPos, parentId);
      nextStationId++;
      station.SetRails(railPoses);
      pos2StationIds[stationPos] = station.id;
      for (const auto rail : railPoses) {
        pos2RailIds[rail] = station.id;
      }
      railPoses.clear();
      stations.push_back(station);
      stationOrders.push_back(station.id);
      if (parentId != -1) {
        stations[parentId].AddChild(station.id);
      }
    }

    assert((int)railPoses.size() == 0);
    return State(stations, stationOrders, PARAMS[14]);
  }

  bool TryAdjSwapStationOrder(const int prevOrder) {
    const int nextOrder = prevOrder + 1;
    if (nextOrder == (int)stationOrders.size()) {
      return false;
    }
    auto &prevStation = stations[stationOrders[prevOrder]];
    auto &nextStation = stations[stationOrders[nextOrder]];
    if (prevStation.id == nextStation.parentId) {
      return false;
    }
    swap(prevStation.order, nextStation.order);
    swap(stationOrders[prevOrder], stationOrders[nextOrder]);
    return true;
  }

  // 指定の駅の線路を削除する
  // 削除後に、駅を移動するなら駅を移動した後で、線路再構築メソッドを呼び出す想定
  // 削除前の古い駅を返す
  Station ResetStationRails(const int stationId) {
    const auto oldStation = stations[stationId];

    for (const auto rail : oldStation.rails) {
      assert(board.pos2RailIds[rail] == stationId);
      board.pos2RailIds[rail] = -1;
    }

    stations[stationId].rails.clear();
    stations[stationId].parentId = -1;
    if (oldStation.parentId != -1) {
      stations[oldStation.parentId].RemoveChild(stationId);
    }

    return oldStation;
  }

  void MoveStation(const int stationId, const Pos newPos) {
    auto &station = stations[stationId];
    // 子がいる駅を動かすなら、先に子をリセットしておく
    assert(station.children.empty());
    const auto oldPos = station.pos;
    station.pos = newPos;
    assert(board.pos2StationIds[oldPos] == stationId);
    board.pos2StationIds[oldPos] = -1;
    assert(board.pos2StationIds[newPos] == -1);
    board.pos2StationIds[newPos] = stationId;
  }

  bool TryBuildStationRails(const int stationId) {
    // 根は非想定
    assert(stationId != 0);
    auto &station = stations[stationId];
    assert(station.rails.empty());
    assert(station.parentId == -1);

    // 子が自身の線路上に立っているなら、線路の再構築は諦める
    // TODO: 子を通るようにしたうえで、線路を再構築する
    // TODO: 子の線路も再構築する
    for (const auto childId : station.children) {
      const auto &childStation = stations[childId];
      // 線路が1本以上あるなら、線路で繋がっているので問題ない
      if ((int)childStation.rails.size() >= 1)
        continue;

      // 線路が0本で、自身と隣接していない場合は、線路上に存在する
      // TODO: 駅の位置を移動した場合に対応する
      if (station.pos.Manhattan(childStation.pos) != 1) {
        return false;
      }
    }

    // もし、自身が自分より若い線路上に存在するなら、その線路を親とする
    const auto railId = board.pos2RailIds[station.pos];
    if (railId != -1 && stations[railId].order < station.order) {
      station.parentId = railId;
      stations[station.parentId].AddChild(station.id);
      return true;
    }

    // BFSで、自分より若い駅に到達するまでの線路を構築
    // 距離 * 1e5 - 通った駅の数 が小さいほうが良いとする
    dpGlobal.clear();
    prevsGlobal.clear();
    queue<Pos> que;
    que.push(station.pos);
    dpGlobal.add(station.pos, 0.0);
    constexpr int kDistCoef = (int)1e5;
    while (!que.empty()) {
      const auto pos = que.front();
      que.pop();
      // 自分より若い駅に到着したらおしまい
      if (board.pos2StationIds[pos] != -1 &&
          stations[board.pos2StationIds[pos]].order < station.order) {
        station.parentId = board.pos2StationIds[pos];
        break;
      }
      for (const auto adj : pos.Adj()) {
        // 線路があり、まだ駅になっていないなら、通れない
        if (board.pos2RailIds[adj] != -1 &&
            (board.pos2StationIds[adj] == -1 ||
             stations[board.pos2StationIds[adj]].order > station.order)) {
          continue;
        }
        // 距離 * 係数 - 通った駅の数
        const auto newCost = dpGlobal.get(pos) + kDistCoef
                             + xorshift() % 100 / 100.0;
        if (!dpGlobal.contains(adj) || dpGlobal.get(adj) > newCost) {
          if (!dpGlobal.contains(adj)) {
            // 初訪問ならqueueに追加
            que.push(adj);
          }
          dpGlobal.set(adj, newCost);
          prevsGlobal.set(adj, pos);
        }
      }
    }

    // 経路が存在しなかったらfalse
    if (station.parentId == -1) {
      return false;
    }

    // 復元
    vector<Pos> rails;
    auto pos = stations[station.parentId].pos;
    pos = prevsGlobal.get(pos);
    while (pos != station.pos) {
      rails.push_back(pos);
      pos = prevsGlobal.get(pos);
      assert((int)rails.size() <= N * N * 2);
    }

    // 反映
    for (const auto rail : rails) {
      assert(board.pos2RailIds[rail] == -1);
      board.pos2RailIds[rail] = station.id;
    }
    stations[station.id].SetRails(rails);
    stations[station.parentId].AddChild(station.id);

    return true;
  }

  // 駅,線路の状態を復元する
  // 複数回呼ぶ場合は親から順に呼ぶこと
  void RollbackStation(const Station &newStation) {
    const auto oldStation = stations[newStation.id];
    assert(oldStation.order == newStation.order);
    stations[newStation.id] = newStation;
    if (oldStation.parentId != -1) {
      // 連続指定復元すると、親側に自身が子として存在しない場合があるので、存在しない場合は削除しない
      stations[oldStation.parentId].RemoveChildIfExists(oldStation.id);
    }
    if (newStation.parentId != -1) {
      stations[newStation.parentId].AddChild(newStation.id);
    }
    for (const auto rail : oldStation.rails) {
      // 複数の駅に対して呼び出す場合に、勝手に書き換えないようにしている
      if (board.pos2RailIds[rail] == oldStation.id) {
        board.pos2RailIds[rail] = -1;
      }
    }
    for (const auto rail : newStation.rails) {
      board.pos2RailIds[rail] = newStation.id;
    }
    if (oldStation.pos != newStation.pos) {
      if (!oldStation.pos.IsDummy()) {
        board.pos2StationIds[oldStation.pos] = -1;
      }
      board.pos2StationIds[newStation.pos] = newStation.id;
    }

    // 順序列の復元
    if (newStation.order >= (int)stationOrders.size() ||
        stationOrders[newStation.order] != newStation.id) {
      stationOrders.insert(stationOrders.begin() + newStation.order,
                           newStation.id);
      for (int i = newStation.order + 1; i < (int)stationOrders.size(); i++) {
        stations[stationOrders[i]].order = i;
      }
    }
  }
};

struct Simulator {
  int Simulate(const State &state, const int paidPassIncomeThreshold) const {
    MoneyStateBS moneyStateBS;
    int maxMoney = K;
    rep(i, state.stationOrders.size()) {
      const int stationId = state.stationOrders[i];
      const auto &station = state.stations[stationId];
      const auto action = moneyStateBS.CreateAction(
          station.pos, station.rails.size(),
          moneyStateBS.info.income >= paidPassIncomeThreshold);
      if (action.passTurn + moneyStateBS.info.turn > T) {
        break;
      }
      moneyStateBS.Forward(action);
      if (moneyStateBS.info.turn > T)
        break;

      chmax(maxMoney, moneyStateBS.FinalMoney());
    }

    return maxMoney;
  }
};

struct Evaluation {
  int maxMoney = 0;
  // 最大化問題の場合は符号を反転すること
  double Sum() const { return -maxMoney; }
};

struct Solution {
  State state;
  Simulator simulator;
  Evaluation eval;

  Solution() { InitEval(); }
  void InitEval() {
    eval = Evaluation();
    const int maxMoney =
        simulator.Simulate(state, state.paidPassIncomeThreshold);
    eval.maxMoney = maxMoney;
  }
  static Solution CreateInitialSol(const vector<Command> &commands) {
    Solution sol;
    sol.state = State::CreateByCommands(commands);
    sol.InitEval();
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

// 順序の隣接スワップ
struct NBD1 : public NBD {
  NBD1() {}
  NBD1(Solution *const sol) {
    if((int)sol->state.stationOrders.size() < 3){
      diff = INF;
      return;
    }
    oldEval = sol->eval;
    diff = -sol->Eval();
    int count = 0;
    while (true) {
      count++;
      if (count > 100) {
        diff = INF;
        return;
      }
      order = rand(0, (int)sol->state.stationOrders.size() - 1);
      if (sol->state.TryAdjSwapStationOrder(order)) {
        break;
      }
    }
    sol->InitEval();
    diff += sol->Eval();
  }
  void Restore(Solution *sol) const override {
    if (diff == INF)
      return;
    const auto success = sol->state.TryAdjSwapStationOrder(order);
    sol->eval = oldEval;
    assert(success);
  }

  int order;
  Evaluation oldEval;
};

// 順序の1点挿入
struct NBD2 : public NBD {
  NBD2() {}
  NBD2(Solution *const sol) {
    if((int)sol->state.stationOrders.size() < 3){
      diff = INF;
      return;
    }
    oldEval = sol->eval;
    diff = -sol->Eval();
    int count = 0;
    while (true) {
      count++;
      if (count > 100) {
        diff = INF;
        return;
      }
      oldOrder = rand(1, (int)sol->state.stationOrders.size());
      stationId = sol->state.stationOrders[oldOrder];
      const int parentId = sol->state.stations[stationId].parentId;
      // 根は変えない
      if (parentId == -1) {
        continue;
      }
      newOrder =
          rand_other_than(1, (int)sol->state.stationOrders.size(), oldOrder);
      bool ok = true;
      if (oldOrder < newOrder) {
        for (int prevOrder = oldOrder; prevOrder <= newOrder - 1; prevOrder++) {
          const auto success = sol->state.TryAdjSwapStationOrder(prevOrder);
          if (!success) {
            // 復元
            for (int i = prevOrder - 1; i >= oldOrder; i--) {
              sol->state.TryAdjSwapStationOrder(i);
            }
            ok = false;
            break;
          }
        }
      } else {
        for (int prevOrder = oldOrder - 1; prevOrder >= newOrder; prevOrder--) {
          const auto success = sol->state.TryAdjSwapStationOrder(prevOrder);
          if (!success) {
            // 復元
            for (int i = prevOrder + 1; i <= oldOrder - 1; i++) {
              sol->state.TryAdjSwapStationOrder(i);
            }
            ok = false;
            break;
          }
        }
      }
      if (ok) {
        break;
      }
    }
    sol->InitEval();
    diff += sol->Eval();
  }
  void Restore(Solution *sol) const override {
    if (diff == INF)
      return;
    sol->eval = oldEval;
    if (newOrder < oldOrder) {
      for (int prevOrder = newOrder; prevOrder <= oldOrder - 1; prevOrder++) {
        const auto success = sol->state.TryAdjSwapStationOrder(prevOrder);
        assert(success);
      }
    } else {
      for (int prevOrder = newOrder - 1; prevOrder >= oldOrder; prevOrder--) {
        const auto success = sol->state.TryAdjSwapStationOrder(prevOrder);
        assert(success);
      }
    }
  }

  int stationId;
  int oldOrder;
  int newOrder;
  Evaluation oldEval;
};

// 線路の再構築
struct NBD3 : public NBD {
  NBD3() {}
  NBD3(Solution *const sol) {
    if((int)sol->state.stationOrders.size() < 3){
      diff = INF;
      return;
    }
    oldEval = sol->eval;
    diff = -sol->Eval();
    int count = 0;
    while (true) {
      count++;
      if (count > 100) {
        diff = INF;
        return;
      }
      order = rand(1, (int)sol->state.stationOrders.size());
      stationId = sol->state.stationOrders[order];
      oldStation = sol->state.ResetStationRails(stationId);
      const bool success = sol->state.TryBuildStationRails(stationId);
      if (!success) {
        sol->state.RollbackStation(oldStation);
        continue;
      }
      break;
    }
    sol->InitEval();
    diff += sol->Eval();
  }
  void Restore(Solution *sol) const override {
    if (diff == INF)
      return;
    sol->state.RollbackStation(oldStation);
    sol->eval = oldEval;
  }

  int order;
  int stationId;
  Station oldStation;
  Evaluation oldEval;
};

// 葉の駅を1マス移動
struct NBD4 : public NBD {
  NBD4() {}
  NBD4(Solution *const sol) {
    oldEval = sol->eval;
    diff = -sol->Eval();
    int count = 0;
    while (true) {
      count++;
      if (count > 100) {
        diff = INF;
        return;
      }
      order = rand(1, (int)sol->state.stationOrders.size());
      stationId = sol->state.stationOrders[order];
      // 子がいるなら対象外
      if (!sol->state.stations[stationId].children.empty()) {
        continue;
      }
      newPos = rand_vec(sol->state.stations[stationId].pos.Adj());
      // 既に他の駅があるならcontinue
      if (sol->state.board.pos2StationIds[newPos] != -1) {
        continue;
      }
      oldStation = sol->state.ResetStationRails(stationId);
      sol->state.MoveStation(stationId, newPos);
      const bool success = sol->state.TryBuildStationRails(stationId);
      if (!success) {
        sol->state.RollbackStation(oldStation);
        continue;
      }
      break;
    }
    sol->InitEval();
    diff += sol->Eval();
  }
  void Restore(Solution *sol) const override {
    if (diff == INF)
      return;
    sol->state.RollbackStation(oldStation);
    sol->eval = oldEval;
  }

  int order;
  int stationId;
  Station oldStation;
  Pos newPos;
  Evaluation oldEval;
};

// 駅を1マス移動 葉以外も可
struct NBD5 : public NBD {
  NBD5() {}
  NBD5(Solution *const sol) {
    if((int)sol->state.stationOrders.size() < 3){
      diff = INF;
      return;
    }
    oldEval = sol->eval;
    diff = -sol->Eval();
    int count = 0;
    while (true) {
      count++;
      if (count > 100) {
        diff = INF;
        return;
      }
      order = rand(1, (int)sol->state.stationOrders.size());
      stationId = sol->state.stationOrders[order];
      newPos = rand_vec(sol->state.stations[stationId].pos.Adj());
      // 既に他の駅があるならcontinue
      if (sol->state.board.pos2StationIds[newPos] != -1) {
        continue;
      }

      // 子がいる場合は、子の線路を先にリセットする
      auto children = sol->state.stations[stationId].children;

      oldChildren.clear();
      // orderが小さい順に並び替え
      if ((int)children.size() > 0) {
        sort(all(children), [&](const int a, const int b) {
          return sol->state.stations[a].order < sol->state.stations[b].order;
        });
      }

      // リセットは、orderが大きい順
      REP(i, children.size()) {
        const auto childId = children[i];
        const auto oldChild = sol->state.ResetStationRails(childId);
        oldChildren.push_back(oldChild);
      }
      if (!oldChildren.empty()) {
        reverse(all(oldChildren));
        assert(order < oldChildren[0].order);
      }

      oldStation = sol->state.ResetStationRails(stationId);
      sol->state.MoveStation(stationId, newPos);

      // 対象の駅を経路を再構築
      // 再構築は、orderが小さい順
      if (!sol->state.TryBuildStationRails(stationId)) {
        // 失敗したら復元
        sol->state.RollbackStation(oldStation);
        // 子も復元
        for (const auto &oldChild : oldChildren) {
          sol->state.RollbackStation(oldChild);
        }
        continue;
      }

      // 子の再構築
      bool ok = true;
      for (const auto childId : children) {
        if (!sol->state.TryBuildStationRails(childId)) {
          ok = false;
          break;
        }
      }
      if (ok) {
        break;
      }

      // 失敗したら復元
      // 復元も、orderが小さい順
      sol->state.RollbackStation(oldStation);
      for (const auto &oldChild : oldChildren) {
        sol->state.RollbackStation(oldChild);
      }
    }
    sol->InitEval();
    diff += sol->Eval();
  }
  void Restore(Solution *sol) const override {
    if (diff == INF)
      return;
    sol->state.RollbackStation(oldStation);
    // 子も復元
    for (const auto oldChild : oldChildren) {
      sol->state.RollbackStation(oldChild);
    }
    sol->eval = oldEval;
  }

  int order;
  int stationId;
  Station oldStation;
  Pos newPos;
  vector<Station> oldChildren;
  Evaluation oldEval;
};

// 駅の追加
struct NBD6 : public NBD {
  NBD6() {}
  NBD6(Solution *const sol) {
    oldEval = sol->eval;
    diff = -sol->Eval();
    int count = 0;
    while (true) {
      count++;
      if (count > 100) {
        diff = INF;
        return;
      }
      pos = rand_vec(Pos::all_poses);
      // 既にある
      if (sol->state.board.pos2StationIds[pos] != -1) {
        continue;
      }

      // 駅を追加
      stationId = sol->state.PushStation(pos);
      // 線路を作成
      const auto success = sol->state.TryBuildStationRails(stationId);
      if (!success) {
        // 復元
        sol->state.EraseStation(stationId);
        continue;
      }

      // なるべく前に追加する
      while (true) {
        const int order = sol->state.stations[stationId].order;
        const int prevOrder = order - 1;
        if (prevOrder <= 0)
          break;
        if (!sol->state.TryAdjSwapStationOrder(prevOrder)) {
          // スワップに失敗したら終わり
          break;
        }
      }
      break;
    }
    sol->InitEval();
    diff += sol->Eval();
  }
  void Restore(Solution *sol) const override {
    if (diff == INF)
      return;
    sol->state.ResetStationRails(stationId);
    sol->state.EraseStation(stationId);
    sol->eval = oldEval;
  }

  Pos pos;
  int stationId;
  Evaluation oldEval;
};

// 駅の削除
struct NBD7 : public NBD {
  NBD7() {}
  NBD7(Solution *const sol) {
    if((int)sol->state.stationOrders.size() < 3){
      diff = INF;
      return;
    }
    oldEval = sol->eval;
    diff = -sol->Eval();
    int count = 0;
    while (true) {
      count++;
      if (count > 100) {
        diff = INF;
        return;
      }

      order = rand(1, (int)sol->state.stationOrders.size());
      stationId = sol->state.stationOrders[order];

      // 子がいる場合は、子の線路を先にリセットする
      auto children = sol->state.stations[stationId].children;

      oldChildren.clear();
      // orderが小さい順に並び替え
      if ((int)children.size() > 0) {
        sort(all(children), [&](const int a, const int b) {
          return sol->state.stations[a].order < sol->state.stations[b].order;
        });
      }

      // リセットは、orderが大きい順
      REP(i, children.size()) {
        const auto childId = children[i];
        const auto oldChild = sol->state.ResetStationRails(childId);
        oldChildren.push_back(oldChild);
      }
      if (!oldChildren.empty()) {
        reverse(all(oldChildren));
        assert(order < oldChildren[0].order);
      }

      // 駅を削除
      oldStation = sol->state.ResetStationRails(stationId);
      sol->state.EraseStation(stationId);

      // 子の経路を再構築
      // 再構築は、orderが小さい順
      bool ok = true;
      for (const auto childId : children) {
        if (!sol->state.TryBuildStationRails(childId)) {
          ok = false;
          break;
        }
      }
      if (ok) {
        break;
      }

      // 失敗したら復元
      sol->state.RollbackStation(oldStation);
      // 復元も、orderが小さい順
      for (const auto &oldChild : oldChildren) {
        sol->state.RollbackStation(oldChild);
      }
    }
    sol->InitEval();
    diff += sol->Eval();
  }
  void Restore(Solution *sol) const override {
    if (diff == INF)
      return;
    sol->eval = oldEval;
    sol->state.RollbackStation(oldStation);
    // 子も復元
    for (const auto oldChild : oldChildren) {
      sol->state.RollbackStation(oldChild);
    }

  }

  int order;
  int stationId;
  Station oldStation;
  Evaluation oldEval;
  vector<Station> oldChildren;
};

vector<int> NBDProbs;
const int kProbSum = 10000;
struct NBDGenerator {
  NBD *Generate(Solution *const sol, const int g, const double progress) {
    if (g % 5000 == 0) {
      sol->state.GC();
    }

    if (g % 1000 == 0) {
      // paidPassの基準incomeを山登り
      const auto oldIncomeThreshold = sol->state.paidPassIncomeThreshold;
      const auto oldEval = sol->eval;

      int bestIncomeThreshold = oldIncomeThreshold;
      auto minEval = oldEval;
      for (int delta = 100; delta >= -100; delta -= 100) {
        if (delta == 0) {
          continue;
        }
        if (oldIncomeThreshold + delta <= kRailCost) {
          continue;
        }
        sol->state.paidPassIncomeThreshold = oldIncomeThreshold + delta;
        sol->InitEval();
        if (sol->eval.Sum() <= minEval.Sum()) {
          bestIncomeThreshold = sol->state.paidPassIncomeThreshold;
          minEval = sol->eval;
        }
      }
      // cerr << bestIncomeThreshold << endl;

      // 一番良い閾値に変更
      sol->state.paidPassIncomeThreshold = bestIncomeThreshold;
      sol->eval = minEval;
    }

    const int random = rand(0, kProbSum);
    if (random < NBDProbs[0]) {
      nbd1_ = NBD1(sol);
      return &nbd1_;
    } else if (random < NBDProbs[1]) {
      nbd2_ = NBD2(sol);
      return &nbd2_;
    } else if (random < NBDProbs[2]) {
      nbd3_ = NBD3(sol);
      return &nbd3_;
    } else if (random < NBDProbs[3]) {
      nbd5_ = NBD5(sol);
      return &nbd5_;
    } else if (random < NBDProbs[4]) {
      nbd6_ = NBD6(sol);
      return &nbd6_;
    } else if (random < NBDProbs[5]) {
      nbd7_ = NBD7(sol);
      return &nbd7_;
    } else {
      assert(false);
    }
  }
  const Solution &GetBestSol() const { return best_sol; }

private:
  NBD1 nbd1_;
  NBD2 nbd2_;
  NBD3 nbd3_;
  NBD5 nbd5_;
  NBD6 nbd6_;
  NBD7 nbd7_;
  Solution best_sol;
};

// 駅・線路からなるコマンド列を返す
// 線路の向きは考慮しないので、呼び出しもとで向きを直す
vector<Command> Run(const vector<Command> &commands, const int timeLimit) {
  // optunaの パラメータを確率に反映
  NBDProbs.resize(6);
  // AddとRemoveはどちらもPARAMS[6]を参照するようにしている
  vector<double> props = {PARAMS[2], PARAMS[3], PARAMS[4],
                          PARAMS[5], PARAMS[6], PARAMS[6]};
  double propSum = 0.0;
  for (const auto prop : props) {
    propSum += prop;
  }
  rep(t, 6) {
    const int prevSum = (t == 0 ? 0 : NBDProbs[t - 1]);
    NBDProbs[t] = prevSum + props[t] / propSum * kProbSum;
  }
  NBDProbs.back() = kProbSum;

  Solution first_sol = Solution::CreateInitialSol(commands);
  if((int)first_sol.state.stationOrders.size() <= 2)
  {
    return first_sol.state.ConvertToCommands();
  }
  NBDGenerator gen;
  cerr << "SA_first_score: " << first_sol.Score() << endl;
  SimulatedAnnealing<Solution, NBDGenerator> SA(timeLimit, PARAMS[0],
                                                PARAMS[1]);
  const auto final_sol = SA.run(first_sol, gen);
  cerr << "SA_final_score: " << final_sol.Score() << endl;

  vector<Command> ret = first_sol.Score() > final_sol.Score() ? first_sol.state.ConvertToCommands() : final_sol.state.ConvertToCommands();
  return ret;
}
} // namespace NBD




/* start */

struct MoneyStateAction {
  int passTurn;
  Pos stationPos;
  int railCount;

  MoneyStateAction() : passTurn(-1), stationPos(Pos::Dummy()), railCount(-1) {}
  MoneyStateAction(int passTurn, Pos stationPos, int railCount)
      : passTurn(passTurn), stationPos(stationPos), railCount(railCount) {}
  bool IsDummy() const { return passTurn == -1; }
};

// お金を管理する状態
struct MoneyState {
  // 人iのfromから到達できる駅の個数
  vector<int> fromCounts;
  // 人iのtoから到達できる駅の個数
  vector<int> toCounts;
  // 1ターン当たりの収入
  int incomePerTurn = 0;
  // 半分達成している収入候補
  int halfIncome = 0;
  // 現在保有しているお金
  int money = K;
  // 現在のターン
  int turn = 0;

  MoneyState() : fromCounts(M), toCounts(M) {}

  // 駅を建てるために必要なパスの回数
  int NeedPassTurnToBuildStation(const int railCount) const {
    // NOTE: この構造体は最初しか使わないので、paidPassは考慮しなくてよい
    // 線路数 + 駅
    int buildTurnCount = railCount + 1;
    // 建設コスト
    const int costSum = railCount * kRailCost + kStationCost;
    if (incomePerTurn == 0 && costSum > money) {
      // パスしてもお金がたまらない
      return INF;
    }
    // 建設コストがたまるには何ターン待つか
    // incomePerTurn == 0の場合は、お金が足りていることを上で確認している
    const int moneyWaitTurnCount =
        incomePerTurn == 0 ? 0 : Ceil(max(0, costSum - money), incomePerTurn);

    // 建築完了までに必要なターン数
    const int additionalTurn = max(buildTurnCount, moneyWaitTurnCount + 1);

    // パスすべきターン数
    const int passTurn = max(0, additionalTurn - buildTurnCount);
    return passTurn;
  }

  // 駅を建設する
  void BuildStation(const Pos stationPos, const int railCount) {
    // 建設完了までにかかるターン
    const int additionalTurn = railCount + 1;

    // それまでに得られる収入
    money += incomePerTurn * (additionalTurn - 1);
    // 支払うコスト
    money -= kStationCost + kRailCost * railCount;

    // その駅から到達できる人に対して処理
    for (int i : station2Froms[stationPos]) {
      fromCounts[i]++;
      if (fromCounts[i] == 1) {
        if (toCounts[i] > 0) {
          // その人が初めて到達できる駅になったとき
          incomePerTurn += inputHumans[i].Income();
          halfIncome -= inputHumans[i].Income();
        } else {
          halfIncome += inputHumans[i].Income();
        }
      }
    }
    for (int i : station2Tos[stationPos]) {
      toCounts[i]++;
      if (toCounts[i] == 1) {
        if (fromCounts[i] > 0) {
          // その人が初めて到達できる駅になったとき
          incomePerTurn += inputHumans[i].Income();
          halfIncome -= inputHumans[i].Income();
        } else {
          halfIncome += inputHumans[i].Income();
        }
      }
    }

    assert(money >= 0);

    // 建設完了後の集金フェーズ
    money += incomePerTurn;

    // ターン経過
    turn += additionalTurn;
  }

  // 駅を建設する前の状態に遷移する
  void RollbackBuildStation(const Pos stationPos, const int railCount) {
    const int additionalTurn = railCount + 1;
    // ターン経過
    turn -= additionalTurn;
    // 集金フェーズ
    money -= incomePerTurn;

    // 建設フェーズ
    // その駅から到達できる人に対して処理
    for (int i : station2Tos[stationPos]) {
      toCounts[i]--;
      if (toCounts[i] == 0) {
        if (fromCounts[i] > 0) {
          // その人が到達できる駅がなくなったとき
          incomePerTurn -= inputHumans[i].Income();
          halfIncome += inputHumans[i].Income();
        } else {
          halfIncome -= inputHumans[i].Income();
        }
      }
    }

    for (int i : station2Froms[stationPos]) {
      fromCounts[i]--;
      if (fromCounts[i] == 0) {
        if (toCounts[i] > 0) {
          // その人が到達できる駅がなくなったとき
          incomePerTurn -= inputHumans[i].Income();
          halfIncome += inputHumans[i].Income();
        } else {
          halfIncome -= inputHumans[i].Income();
        }
      }
    }

    money += kStationCost + kRailCost * railCount;
    money -= incomePerTurn * (additionalTurn - 1);
  }

  void Pass(const int passTurn) {
    // 集金フェーズ
    money += incomePerTurn * passTurn;
    // ターン経過
    turn += passTurn;
  }

  void RollbackPass(const int passTurn) {
    // ターン経過
    turn -= passTurn;
    // 集金フェーズ
    money -= incomePerTurn * passTurn;
  }

  int RemainTurn() const { return T - turn; }

  int FinalMoney() const { return money + RemainTurn() * incomePerTurn; }

  void Forward(const MoneyStateAction &action) {
    Pass(action.passTurn);
    BuildStation(action.stationPos, action.railCount);
  }

  void Backward(const MoneyStateAction &action) {
    RollbackBuildStation(action.stationPos, action.railCount);
    RollbackPass(action.passTurn);
  }
};


/* start */

// 序盤(3手？)をめっちゃ頑張って最適化する
// ビームサーチの最初の3手分を担う
struct FirstSolver {
  // 上流のビームサーチにおいて、いくら以上のお金が溜まるターンをビームの添え字とするか
  int baseCost;
  FirstSolver(int baseCost) : baseCost(baseCost) {}
  // [t] := baseCostが溜まるターンがtとなるなかで最良の、1,2,3手目を返す
  vector<tuple<MoneyStateAction, MoneyStateAction, MoneyStateAction>> Solve() const{
    const int kBW12 = 1000;

    // 1手目と2手目を全列挙
    vector<pair<pair<Pos, Pos>, int>> firstSecondPosesAndIncomes;
    for (const auto [pair, income] : sortedValidStationPairToIncome) {
      const auto [p0, p1] = pair;
      // 必要な線路数
      const int railCount = p0.Manhattan(p1) - 1;

      // お金が足りない
      if (railCount * kRailCost + 2 * kStationCost > K)
        continue;

      firstSecondPosesAndIncomes.emplace_back(pair, income);
    }

    cerr << "firstSecondPosesAndIncomes.size() = "
         << firstSecondPosesAndIncomes.size() << endl;
    cerr << "maxIncome = " << firstSecondPosesAndIncomes[0].second << endl;

    if ((int)firstSecondPosesAndIncomes.size() > kBW12) {
      firstSecondPosesAndIncomes.resize(kBW12);
    }

    MoneyState moneyState;
    vector<int> turn2MaxIncome(T + 1, 0);
    vector<tuple<MoneyStateAction, MoneyStateAction, MoneyStateAction>> turn2BestActions;
    turn2BestActions.resize(T + 1);

    // 3手目を全列挙
    for (const auto [pair, _] : firstSecondPosesAndIncomes) {
      const auto [p0, p1] = pair;
      MoneyStateAction action0(0, p0, 0);
      MoneyStateAction action1(0, p1, p0.Manhattan(p1) - 1);
      moneyState.Forward(action0);
      moneyState.Forward(action1);

      rep(idx, N * N) {
        const Pos p2(idx);
        if (p2 == p0 || p2 == p1) {
          continue;
        }

        // 必要な線路数
        int railCount = min(p1.Manhattan(p2), p0.Manhattan(p2)) - 1;
        // 間にある場合は0
        if (p0.Manhattan(p1) == p0.Manhattan(p2) + p1.Manhattan(p2)) {
          railCount = 0;
        }
        // 必要なパス
        const int passTurn = moneyState.NeedPassTurnToBuildStation(railCount);
        MoneyStateAction action2(passTurn, p2, railCount);
        moneyState.Forward(action2);

        // 評価
        assert(moneyState.incomePerTurn > 0);
        const int moneyWaitTurnCount = Ceil(max(0, baseCost - moneyState.money), moneyState.incomePerTurn);
        const int nextTurn = moneyState.turn + moneyWaitTurnCount;
        if(nextTurn <= T){
          const int income = moneyState.incomePerTurn;
          if(chmax(turn2MaxIncome[nextTurn], income)){
            turn2BestActions[nextTurn] = {action0, action1, action2};
          }
        }

        moneyState.Backward(action2);
      }

      moneyState.Backward(action1);
      moneyState.Backward(action0);
    }

    vector<tuple<MoneyStateAction, MoneyStateAction, MoneyStateAction>> ret;
    rep(t, T + 1) {
      if (turn2MaxIncome[t] == 0) {
        continue;
      }

      // cerr << "t = " << t << " maxIncome = " << turn2MaxIncome[t] << endl;
      ret.push_back(turn2BestActions[t]);
    }

    return ret;
  }
};





/* start */

// コマンド列を渡すと、パスのタイミングを最適な位置に挿入する
// ただし、設置した全ての駅が連結であることを仮定している
// また、ある駅を建てる金が不足している時に、
// 駅の後に予定していた線路を先に建てても、駅を建てるターンが変わらないのであれば、
// 先に線路を建てる
struct GreedyPassInserter {
  vector<Command> InsertPass(vector<Command> commands) {
    return InsertPassAndReturnMoney(commands).first;
  }

  pair<vector<Command>, int>
  InsertPassAndReturnMoney(vector<Command> commands) {

    // 線路を駅に変えることでスコアが改善するなら、変える
    {
      int maxMoney = InsertPassGreedy(commands).first;
      rep(_t, 10) {
        rep(i, commands.size()) {
          if (commands[i].kind != kRail) {
            continue;
          }

          assert(commands[i].kind == kRail);
          const auto oldCommand = commands[i];
          commands[i] = Command::Station(oldCommand.p);
          // もし、未来でその場所に駅を建てようとするコマンドがあるなら、それは削除
          int duplicateCommandIdx = -1;
          Command duplicateCommand;
          for (int j = i + 1; j < (int)commands.size(); j++) {
            if (commands[j].kind == kStation && commands[j].p == oldCommand.p) {
              duplicateCommandIdx = j;
              break;
            }
          }
          if (duplicateCommandIdx != -1) {
            duplicateCommand = commands[duplicateCommandIdx];
            commands.erase(commands.begin() + duplicateCommandIdx);
          }
          const auto [money, _] = InsertPassGreedy(commands);
          if (chmax(maxMoney, money)) {
            continue;
          } else {
            // 戻す
            commands[i] = oldCommand;
            if (duplicateCommandIdx != -1) {
              commands.insert(commands.begin() + duplicateCommandIdx,
                              duplicateCommand);
            }
          }
        }
      }
    }

    // パスの挿入
    // 高速化できるがボトルネックではないので愚直にやっている
    {
      vector<Command> bestPassInsertedCommands;
      int maxMoney = -INF;
      const auto [money, passInsertedCommands] = InsertPassGreedy(commands);
      maxMoney = money;
      bestPassInsertedCommands = passInsertedCommands;

      // ある線路を先に建てることで、スコアが改善するなら、先に建てる
      rep(i, commands.size()) {
        if (i == 0)
          continue;

        const auto [money, passInsertedCommands] =
            InsertPassGreedy(commands, i);
        if (chmax(maxMoney, money)) {
          bestPassInsertedCommands = passInsertedCommands;
        }
      }
      return {bestPassInsertedCommands, maxMoney};
    }
  }

  // railPriorityIdxのコマンドに到達したら、それ以降はパスせずに線路を立てる
  pair<int, vector<Command>> InsertPassGreedy(const vector<Command> &commands,
                                              const int railPriorityIdx = INF) {
    // 両方が正になったらincomeを得られるようになる
    vector<int> human2FromCounts(M, 0);
    vector<int> human2ToCounts(M, 0);

    vector<Command> passInsertedCommands;
    // 格納したいくつのコマンドを採用し、残りをPassにするのがベストか
    int bestCommandSize = 0;
    // その時に得られる最終的な金額
    int maxMoney = K;

    int currentIncome = 0;
    int currentMoney = K;
    int nextCommandIdx = 0;
    int nextRailIdx = 0;
    rep(t, T) {
      // tターン目のコマンドが実行できるなら実行する、実行できないならパスする

      // 既に立てた線路はスキップ
      while (nextCommandIdx < (int)commands.size() &&
             commands[nextCommandIdx].kind == kRail &&
             nextCommandIdx < nextRailIdx) {
        nextCommandIdx++;
      }

      // もうコマンドが残っていない
      if (nextCommandIdx == (int)commands.size()) {
        break;
      }

      const auto &command = commands[nextCommandIdx];
      const bool canCommand = command.Cost() <= currentMoney;
      if (canCommand) {
        // コマンドを実行
        currentMoney -= command.Cost();
        if (command.kind == kStation) {
          // 駅を設置したなら、incomeが増えるかどうかを確認
          const auto &froms = station2Froms[command.p];
          const auto &tos = station2Tos[command.p];
          for (const auto i : froms) {
            human2FromCounts[i]++;
            if (human2FromCounts[i] == 1 && human2ToCounts[i] > 0) {
              currentIncome += inputHumans[i].Income();
            }
          }
          for (const auto i : tos) {
            human2ToCounts[i]++;
            if (human2FromCounts[i] > 0 && human2ToCounts[i] == 1) {
              currentIncome += inputHumans[i].Income();
            }
          }
        }
        passInsertedCommands.push_back(command);
        nextCommandIdx++;
      } else {
        if (nextCommandIdx >= railPriorityIdx && command.kind == kStation &&
            currentMoney >= kRailCost) {
          // パスせずに線路を建てる
          chmax(nextRailIdx, nextCommandIdx);
          while (nextRailIdx < (int)commands.size() &&
                 commands[nextRailIdx].kind != kRail) {
            nextRailIdx++;
          }
          if (nextRailIdx < (int)commands.size()) {
            const auto &command = commands[nextRailIdx];
            assert(command.kind == kRail);
            currentMoney -= command.Cost();
            passInsertedCommands.push_back(command);
            nextRailIdx++;
          } else {
            // パスを実行
            passInsertedCommands.push_back(Command::Pass());
          }
        } else {
          // パスを実行
          passInsertedCommands.push_back(Command::Pass());
        }
      }

      // 収入を得る
      // cerr << "currentMoney: " << currentMoney << endl;
      // cerr << "currentIncome: " << currentIncome << endl;
      currentMoney += currentIncome;

      // 残りの収入を得られる回数
      const int remainTurn = T - t - 1;
      const int finalMoney = currentMoney + currentIncome * remainTurn;

      if (chmax(maxMoney, finalMoney)) {
        bestCommandSize = t + 1;
      }
    }

    assert(bestCommandSize <= T);
    assert(bestCommandSize <= (int)passInsertedCommands.size());
    passInsertedCommands.resize(bestCommandSize);
    while ((int)passInsertedCommands.size() < T) {
      passInsertedCommands.push_back(Command::Pass());
    }
    return {maxMoney, passInsertedCommands};
  }
};



/* start */

template<typename T>
struct IndexSet {
  vector<T> que;
  vector<int> pos;

  IndexSet(int n) : pos(n, -1) {}

  void add(T v) {
    pos[(int)v] = que.size();
    que.push_back(v);
  }

  void remove(T v) {
    int p = pos[(int)v];
    T b = que.back();
    que[p] = b;
    que.pop_back();
    pos[b] = p;
    pos[(int)v] = -1;
  }

  bool contains(T v) const {
    return pos[(int)v] != -1;
  }

  int size() const {
    return que.size();
  }
};
#ifndef ATCODER_SEGTREE_HPP
#define ATCODER_SEGTREE_HPP 1

#include <algorithm>
// #include <atcoder/internal_bit>
#include <cassert>
#include <vector>

// combine.pyの仕様に合わせるためにinclude先をhppにした
#ifndef ATCODER_INTERNAL_BITOP_HPP
#define ATCODER_INTERNAL_BITOP_HPP 1

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace atcoder {

namespace internal {

// @param n `0 <= n`
// @return minimum non-negative `x` s.t. `n <= 2**x`
int ceil_pow2(int n) {
    int x = 0;
    while ((1U << x) < (unsigned int)(n)) x++;
    return x;
}

// @param n `1 <= n`
// @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
int bsf(unsigned int n) {
#ifdef _MSC_VER
    unsigned long index;
    _BitScanForward(&index, n);
    return index;
#else
    return __builtin_ctz(n);
#endif
}

}  // namespace internal

}  // namespace atcoder

#endif  // ATCODER_INTERNAL_BITOP_HPP


namespace atcoder {

template <class S, S (*op)(S, S), S (*e)()>
struct segtree {
 public:
  segtree() : segtree(0) {}
  segtree(int n) : segtree(std::vector<S>(n, e())) {}
  segtree(const std::vector<S>& v) : _n(int(v.size())) {
    log = internal::ceil_pow2(_n);
    size = 1 << log;
    d = std::vector<S>(2 * size, e());
    for (int i = 0; i < _n; i++) d[size + i] = v[i];
    for (int i = size - 1; i >= 1; i--) {
      update(i);
    }
  }

  void set(int p, S x) {
    assert(0 <= p && p < _n);
    p += size;
    d[p] = x;
    for (int i = 1; i <= log; i++) update(p >> i);
  }

  S get(int p) {
    assert(0 <= p && p < _n);
    return d[p + size];
  }

  S prod(int l, int r) {
    assert(0 <= l && l <= r && r <= _n);
    S sml = e(), smr = e();
    l += size;
    r += size;

    while (l < r) {
      if (l & 1) sml = op(sml, d[l++]);
      if (r & 1) smr = op(d[--r], smr);
      l >>= 1;
      r >>= 1;
    }
    return op(sml, smr);
  }

  S all_prod() { return d[1]; }

  template <bool (*f)(S)>
  int max_right(int l) {
    return max_right(l, [](S x) { return f(x); });
  }
  template <class F>
  int max_right(int l, F f) {
    assert(0 <= l && l <= _n);
    assert(f(e()));
    if (l == _n) return _n;
    l += size;
    S sm = e();
    do {
      while (l % 2 == 0) l >>= 1;
      if (!f(op(sm, d[l]))) {
        while (l < size) {
          l = (2 * l);
          if (f(op(sm, d[l]))) {
            sm = op(sm, d[l]);
            l++;
          }
        }
        return l - size;
      }
      sm = op(sm, d[l]);
      l++;
    } while ((l & -l) != l);
    return _n;
  }

  template <bool (*f)(S)>
  int min_left(int r) {
    return min_left(r, [](S x) { return f(x); });
  }
  template <class F>
  int min_left(int r, F f) {
    assert(0 <= r && r <= _n);
    assert(f(e()));
    if (r == 0) return 0;
    r += size;
    S sm = e();
    do {
      r--;
      while (r > 1 && (r % 2)) r >>= 1;
      if (!f(op(d[r], sm))) {
        while (r < size) {
          r = (2 * r + 1);
          if (f(op(d[r], sm))) {
            sm = op(d[r], sm);
            r--;
          }
        }
        return r + 1 - size;
      }
      sm = op(d[r], sm);
    } while ((r & -r) != r);
    return 0;
  }

 private:
  int _n, size, log;
  std::vector<S> d;

  void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
};

}  // namespace atcoder

#endif  // ATCODER_SEGTREE_HPP




/* start */

// 参考: https://eijirou-kyopro.hatenablog.com/entry/2024/02/01/115639
// std::unordered_mapよりも早いHashMap
// Keyは、
// * uint32_tやuint64_tなどの符号無し整数
// * 一様ランダムに分布している
template <typename Key, class T>
struct HashMap {
 public:
  struct HashMapIndex {
    uint32_t idx;

    // GetIndex内のreturn時にHashMapIndexの一時オブジェクトを作成しないために、
    // 暗黙の型変換を許容している
    // (もしかしたらより良い書き方があるかも)
    HashMapIndex(uint32_t _idx) : idx(_idx){}
  };

  // n: HashMap用に確保する領域の大きさ
  //    2の累乗数かつ1e9未満とする
  //    nはKeyの最大値よりも小さくて良いことに注意
  explicit HashMap(uint32_t n) : n_(n), valid_(n, false), data_(n) {
    assert(n_ < 1e9);
    bitMask_ = n_ - 1;
    // 2の累乗数であることを確認
    uint32_t log2n = 0;
    while (pow(2, log2n) < n_) {
      log2n++;
    }
    assert(pow(2, log2n) == n_);
  }

  // (発見できたか, idx)を返す
  // 発見できなかった場合には新しい要素をそのidxに追加する想定
  pair<bool, HashMapIndex> GetIndex(const Key key) const {
    uint32_t idx = (key & bitMask_);
#ifndef NDEBUG
    total_count_++;
    collision_count_++;
#endif
    while (IsValid(idx)) {
      if (data_[idx].first == key) {
        return {true, idx};
      }

#ifndef NDEBUG
      collision_count_++;
#endif
      if (++idx == n_) {
        idx = 0;
      }
    }
    return {false, idx};
  }

  void Set(const HashMapIndex idx, const Key key, const T& value) {
    valid_[idx.idx] = true;
    data_[idx.idx].first = key;
    data_[idx.idx].second = value;
  }

  void Set(const HashMapIndex idx, const Key key, T&& value) {
    valid_[idx.idx] = true;
    data_[idx.idx].first = key;
    data_[idx.idx].second = std::forward<T>(value);
  }

  const T& Get(const HashMapIndex idx) const {
    assert(IsValid(idx.idx));
    return data_[idx.idx].second;
  }

  void Clear() { std::fill(valid_.begin(), valid_.end(), false); }

#ifndef NDEBUG
  // 1回 GetIndexあたりの要素へのアクセス回数の平均値
  // 一度もハッシュが衝突しなかったなら1となり、
  // 衝突すればするほど大きくなる
  double MeanAccessCount() const {
    assert(total_count_ > 0);
    return (double)collision_count_ / total_count_;
  }
#endif

 private:
  bool IsValid(const int idx) const { return valid_[idx]; }

  uint32_t n_ = 0;
  vector<bool> valid_;
  vector<pair<Key, T>> data_;
  uint32_t bitMask_ = 0;
#ifndef NDEBUG
  // ハッシュ衝突具合をデバッグする用
  // GetIndexを読んだ回数
  mutable uint32_t total_count_ = 0;
  // 要素にアクセスした回数
  mutable uint32_t collision_count_ = 0;
#endif
};



/* start */

// 参考: https://eijirou-kyopro.hatenablog.com/entry/2024/02/01/115639
template <typename T>
struct ObjectPool {
  T& operator[](const int idx) {
#ifndef NDEBUG
    assert(is_used_[idx]);
#endif
    return data_[idx];
  }
  const T& operator[](const int idx) const {
#ifndef NDEBUG
    assert(is_used_[idx]);
#endif
    return data_[idx];
  }

  void Reserve(size_t capacity) {
    data_.reserve(capacity);
    unused_idxes_.reserve(capacity);
  }

  // 要素を追加し、追加したindexを返す
  int Push(const T& obj) {
    if (unused_idxes_.size()) {
      const int idx = unused_idxes_.back();
      unused_idxes_.pop_back();
#ifndef NDEBUG
      assert(!is_used_[idx]);
      is_used_[idx] = true;
#endif
      data_[idx] = obj;
      return idx;
    } else {
      data_.emplace_back(obj);
#ifndef NDEBUG
      is_used_.emplace_back(true);
#endif
      return (int)data_.size() - 1;
    }
  }

  // move代入版
  int Push(T&& obj) {
    if (unused_idxes_.size()) {
      const int idx = unused_idxes_.back();
      unused_idxes_.pop_back();
#ifndef NDEBUG
      assert(!is_used_[idx]);
      is_used_[idx] = true;
#endif
      data_[idx] = std::forward<T>(obj);
      return idx;
    } else {
#ifndef NDEBUG
      is_used_.emplace_back(true);
#endif
      data_.emplace_back(std::forward<T>(obj));
      return (int)data_.size() - 1;
    }
  }

  void Erase(const int idx) {
#ifndef NDEBUG
    assert(is_used_[idx]);
    is_used_[idx] = false;
#endif
    unused_idxes_.push_back(idx);
  }

  // 現在のサイズではなく、これまでの最大サイズ
  // Reserveの目安を知るためのデバッグ用
  int MaxSize() const { return data_.size(); }

 private:
  vector<T> data_;
  vector<int> unused_idxes_;
#ifndef NDEBUG
  // 破棄されたindexにアクセスしていないか確認するデバッグ用
  vector<bool> is_used_;
#endif
};



#include <unordered_map>

/* start */

namespace Chokudai {
using Hash = uint32_t;
using Cost = double;

// // 各ターンのIncomeの期待値を
// // f(t) = exp(b / a * (t - t0)) * f(t0) とする
// // Aは1つ駅を建てる時のコストの期待値
// // Bは1手あたりのIncomeの増加量の期待値
// // t0, f(t0) は初期解のターンとincome
// double scoreCoefA = 5600;
// double scoreCoefB = 20;
// double scoreT0 = 20;
// double scoreF0 = 20;
#ifdef DEBUG_BUILD
const int kTimeLimitBeam = 25000;
const int kTimeLimitSA = 10000;
#else
const int kTimeLimitBeam = 2400;
const int kTimeLimitSA = 2800;
#endif

// 状態遷移を行うために必要な情報
struct Action {
  MoneyStateBSAction moneyStateBSAction;
  Action() {}
  Action(MoneyStateBSAction moneyStateBSAction)
      : moneyStateBSAction(moneyStateBSAction) {}
};

// 状態のコストを評価するための構造体
struct Evaluator {
  double score = 0;
  int turn = 0;
  Evaluator() {}
  Evaluator(double score, int turn) : score(score), turn(turn) {}

  Cost Evaluate() const { return 1'000'000'000 - score; }
};

// 展開するノードの候補を表す構造体
struct Candidate {
  Action action;
  Evaluator evaluator;
  Hash hash;
  int parent;
  double score;

  Candidate() {}
  Candidate(Action action, Evaluator evaluator, Hash hash, int parent,
            double score)
      : action(action), evaluator(evaluator), hash(hash), parent(parent),
        score(score) {}

  bool operator<(const Candidate &other) const { return score < other.score; }
};

struct Selector {
  // ターンごとにキューを持つ
  vector<set<Candidate>> turn2Selectors;
  vector<unordered_map<Hash, float>> turn2Hash2MaxScores;
  int beamWidth;

  Selector(const int beamWidth)
      : turn2Selectors(801), turn2Hash2MaxScores(801), beamWidth(beamWidth) {}

  void Push(Action action, const Evaluator evaluator, Hash hash, int parent) {
    assert(evaluator.turn <= T);
    const auto score = evaluator.score;
    // すでに同じハッシュのノードがある場合は、スコアが高いものを採用する
    if (turn2Hash2MaxScores[evaluator.turn].count(hash) &&
        turn2Hash2MaxScores[evaluator.turn][hash] >= score) {
      return;
    }
    auto &selector = turn2Selectors[evaluator.turn];
    selector.emplace(Candidate(action, evaluator, hash, parent, score));
    if ((int)selector.size() > beamWidth) {
      selector.erase(selector.begin());
    }
  }

  int BestTurn(int beginTurn, int endTurn) {
    double maxScore = -INF;
    int bestTurn = -1;
    for (int turn = beginTurn; turn < endTurn; ++turn) {
      const auto &selector = turn2Selectors[turn];
      if (selector.empty()) {
        continue;
      }
      const auto &bestCandidate = *selector.rbegin();
      if (chmax(maxScore, bestCandidate.score)) {
        bestTurn = turn;
      }
    }

    return bestTurn;
  }

  Candidate Pop(int turn) {
    const auto itr = prev(turn2Selectors[turn].end());
    const auto candidate = *itr;
    chmax(turn2Hash2MaxScores[candidate.evaluator.turn][candidate.hash],
          candidate.score);
    turn2Selectors[turn].erase(itr);
    return candidate;
  }
};

// expand()に渡してcandidateを受け取るための構造体
struct OneCandidateSelector {
  Candidate candidate;
  void Push(Action action, const Evaluator evaluator, Hash hash, int parent) {
    candidate = Candidate(action, evaluator, hash, parent, evaluator.score);
  }
};

// 深さ優先探索に沿って更新する情報をまとめたクラス
class State {
public:
  bitset<2500> isStations;
  bitset<2500> isRails;
  vector<Pos> stations;
  MoneyStateBS moneyState;

  explicit State() : isStations(0), isRails(0), stations(), moneyState() {}

  explicit State(Pos firstStation)
      : isStations(0), isRails(0), stations(), moneyState() {
    isStations[firstStation] = true;
    stations.push_back(firstStation);
    moneyState.BuildStation(firstStation, 0, false);
  }

  // actionを実行して次の状態に遷移する
  void Forward(Action action, vector<Pos> rails) {
    moneyState.Forward(action.moneyStateBSAction);
    const auto stationPos = action.moneyStateBSAction.stationPos;
    assert(!isStations[stationPos]);
    isStations[stationPos] = true;
    stations.push_back(stationPos);
    for (const auto rail : rails) {
      assert(!isStations[rail]);
      assert(!isRails[rail]);
      isRails[rail] = true;
    }
  }
};

// DPの1マス分の結果
struct DPData {
  int dist;
  float score;
  Pos prevPos;

  DPData(int dist, float score, Pos prevPos)
      : dist(dist), score(score), prevPos(prevPos) {}

  // 小さいほうが良いとする
  bool operator<(const DPData &other) const {
    if (dist != other.dist) {
      // 距離は小さいほうが良い
      return dist < other.dist;
    }
    // スコアは大きいほうが良い
    return score > other.score;
  }
};

// 線路の経路のDPの結果
struct DPResult {
  vector<DPData> dpData;
  DPResult() {}
  DPResult(vector<DPData> dpData) : dpData(dpData) {}

  vector<Pos> GetRailRoute(const Pos goal) const {
    vector<Pos> rails;
    Pos pos = dpData[goal].prevPos;
    while (!pos.IsDummy()) {
      rails.push_back(pos);
      pos = dpData[pos].prevPos;
    }
    if (!rails.empty()) {
      rails.pop_back();
    }
    return rails;
  }
};

// ビムサのノード Candidateを具現化したもの
struct Node {
  Action action;
  Evaluator evaluator;
  Hash hash;
  int parent;
  State state;
  DPResult dpResult;

  // 根のコンストラクタ
  Node(Action action, const Evaluator &evaluator, Hash hash, State state)
      : action(action), evaluator(evaluator), hash(hash), parent(-1),
        state(state) {}

  // 通常のコンストラクタ
  Node(const Candidate &candidate, const State &state, const DPResult &dpResult)
      : action(candidate.action), evaluator(candidate.evaluator),
        hash(candidate.hash), parent(candidate.parent), state(state),
        dpResult(dpResult) {}

  // 次の状態候補を生成し、selectorに追加する
  template <typename Selector>
  void expand(const Evaluator &evaluator, Hash hash, int parent,
              Selector &selector, const vector<Pos> &posCandidates) const {
    for (const auto pos : posCandidates) {
      if (state.isStations[pos]) {
        // 駅がある場合はスキップ
        continue;
      }

      if (station2Froms[pos].empty() && station2Tos[pos].empty()) {
        // 利用者がいない場合はスキップ
        continue;
      }

      // 駅を立てるために必要な線路の本数を取得
      const int railCount = max(0, dpResult.dpData[pos].dist - 1);

      // 駅を建てる経路が存在しない場合はスキップ
      if (railCount > N * N)
        continue;

      // 駅を建てるアクションを生成
      const Action action(state.moneyState.CreateAction(pos, railCount, state.moneyState.info.income >= PARAMS[14]));
      if (action.moneyStateBSAction.passTurn + state.moneyState.info.turn > T) {
        continue;
      }

      // 行動後の状態をシミュレーションする 更新は行わない
      const auto stateInfo =
          state.moneyState.Simulate(action.moneyStateBSAction);

      // 最大ターンを超過した場合はスキップ
      if (stateInfo.turn > T)
        continue;

      // 次の状態の評価値を計算
      // const double expectedIncome =
      //     exp(scoreCoefB / scoreCoefA * (stateInfo.turn - scoreT0)) *
      //     scoreF0;
      const double score = stateInfo.money + (stateInfo.income + stateInfo.halfIncome * PARAMS[8] *
          pow(PARAMS[9], max(0.0, PARAMS[10] - stateInfo.income))) * (T - stateInfo.turn);
      const Evaluator nextEvaluator(score, stateInfo.turn);

      // 次の状態のハッシュを計算
      const auto nextHash = hash ^ posHashes[pos];

      // 次の状態をselectorに追加
      selector.Push(action, nextEvaluator, nextHash, parent);
    }
  }
};

// 各マスに駅を立てるときの評価値をDPで計算
DPResult CalcDP(const State &state, const vector<float> &pos2Scores) {
  global_timers["CalcDP"].start();
  vector<DPData> dpData(N * N, DPData(INF, -INF, Pos::Dummy()));
  queue<Pos> que;
  rep(idx, N * N) {
    const Pos pos(idx);
    if (state.isStations[pos]) {
      // 駅がある場合は、距離0でスコアは0
      dpData[pos] = DPData(0, 0, Pos::Dummy());
      que.push(pos);
    } else if (state.isRails[pos]) {
      // 駅が無く、線路がある場合も、距離0でスコアは0
      dpData[pos] = DPData(0, 0, Pos::Dummy());
      // ただし、その位置から線路を繋ぐことはできないため、queueには追加しない
    }
  }

  // BFSでDP
  while (!que.empty()) {
    const auto pos = que.front();
    que.pop();
    for (const auto nextPos : pos.Adj()) {
      if (state.isRails[nextPos]) {
        // 既に線路がある場合はスキップ
        continue;
      }

      const auto dist = dpData[pos].dist + 1;
      const auto prevPos = dpData[pos].prevPos;
      const bool isCurve = !prevPos.IsDummy() &&
                           (prevPos.X() == pos.X()) != (pos.X() == nextPos.X());
      // 曲がるのは線路効率が悪いので、ペナルティを与える
      const auto score =
          dpData[pos].score + pos2Scores[nextPos] +
          isCurve * PARAMS[11];
      const auto newDpData = DPData(dist, score, pos);
      // 改善していれば更新
      if (newDpData < dpData[nextPos]) {
        // まだqueueに追加していなければqueueに追加
        if (dpData[nextPos].dist == INF) {
          que.push(nextPos);
        }
        dpData[nextPos] = newDpData;
      }
    }
  }

  global_timers["CalcDP"].add();
  return DPResult(dpData);
}

Node Candidate2Node(const Candidate &candidate, const Node &parentNode,
                    const vector<float> &pos2Scores) {
  global_timers["Candidate2Node"].start();
  State state = parentNode.state;

  // 駅を建てるための線路の経路を取得
  const auto rails = parentNode.dpResult.GetRailRoute(
      candidate.action.moneyStateBSAction.stationPos);

  // 状態を更新
  state.Forward(candidate.action, rails);

  // 次に、各駅からDPを行い、距離と評価値を求める
  const auto dpDatas = CalcDP(state, pos2Scores);
  global_timers["Candidate2Node"].add();
  // ノードを返す
  return Node(candidate, state, DPResult(dpDatas));
}

void InitFirstSolutionsByFirstSolver(Selector &selector,
                                     ObjectPool<Node> &allNodes,
                                     const vector<float> &originalPos2Scores) {
  const FirstSolver firstSolver(kStationCost + kRailCost);
  const auto firstActions = firstSolver.Solve();
  unordered_map<Hash, int> hash2NodeId;

  auto pos2Scores = originalPos2Scores;
  for (const auto [action0, action1, action2] : firstActions) {
    const auto p0 = action0.stationPos;
    const auto p1 = action1.stationPos;
    const auto p2 = action2.stationPos;
    const auto firstPos = action0.stationPos;
    State firstState(firstPos);
    // 各駅からDPを行い、距離と評価値を求める
    const auto firstDpDatas = CalcDP(firstState, pos2Scores);
    // Candidateを生成
    const Candidate firstCandidate(Action(MoneyStateBSAction(0, firstPos, 0, false)),
                                   Evaluator(0, 0), posHashes[firstPos], -1, 0);
    // ノードに変換
    const Node firstNode(firstCandidate, firstState, DPResult(firstDpDatas));
    // 格納
    const auto firstNodeId = allNodes.Push(firstNode);

    // 2手目の状態を生成
    OneCandidateSelector secondCandidateSelector;
    vector<Pos> memos;
    // 2手目start～3手目goalを阻害する方向には進まない
    Pos start, goal, other;
    if(p0.Manhattan(p2) < p1.Manhattan(p2)){
      // p0~p2を最短で進めるように
      start = p0;
      goal = p2;
      other = p1;
    }else{
      start = p1;
      goal = p2;
      other = p0;
    }

    // start~goalとstart~otherをそれぞれどの方向にすべきか
    int minManhattanSum = INF;
    Pos bestGoalAdj = Pos::Dummy();
    Pos bestOtherAdj = Pos::Dummy();
    for(const auto goalAdj : start.Adj()){
      for(const auto otherAdj : start.Adj()){
        const int manhattanSum = goalAdj.Manhattan(goal) + otherAdj.Manhattan(other);
        if(chmin(minManhattanSum, manhattanSum)){
          bestGoalAdj = goalAdj;
          bestOtherAdj = otherAdj;
        }
      }
    }

    // bestOtherAdjを通るように引く
    memos.push_back(bestOtherAdj);
    pos2Scores[bestOtherAdj] += 1e5;

    // 3手目を通れるなら通るようにする
    memos.push_back(action2.stationPos);
    pos2Scores[action2.stationPos] = INF;
    firstNode.expand(firstNode.evaluator, firstNode.hash, firstNodeId,
                     secondCandidateSelector, {action1.stationPos});
    const auto secondCandidate = secondCandidateSelector.candidate;
    const auto secondNode =
        Candidate2Node(secondCandidate, firstNode, pos2Scores);
    const auto secondNodeId = allNodes.Push(secondNode);

    // 3手目の状態をselectorに追加
    secondNode.expand(secondNode.evaluator, secondNode.hash, secondNodeId,
                      selector, {action2.stationPos});
    for(const auto pos : memos){
      pos2Scores[pos] = originalPos2Scores[pos];
    }
  }

  cerr << "初期解を構築しました。" << endl;
}

// ビームサーチの結果からコマンド列を復元する
// 最終スコアも返す
pair<vector<Command>, int> GenerateAnswer(const ObjectPool<Node> &allNodes,
                                          int bestNodeId) {
  // パス以外のコマンドをまずは復元する
  vector<Command> commandsNoPass;
  // 逆順で復元
  for (int v = bestNodeId; v != -1; v = allNodes[v].parent) {
    const auto &node = allNodes[v];
    const auto action = node.action;

    // 駅を建てる
    commandsNoPass.push_back(
        Command::Station(action.moneyStateBSAction.stationPos));

    // 線路を建てる
    // ただし、最初のノードは親が存在しないし、線路を建てない
    if (node.parent != -1) {
      const auto &parentNode = allNodes[node.parent];
      const auto rails = parentNode.dpResult.GetRailRoute(
          node.action.moneyStateBSAction.stationPos);
      for (const auto rail : rails) {
        // 線路の方向は分からないので、一旦仮の値とする
        commandsNoPass.push_back(Command(kRail, rail, kU, kU));
      }
    }
  }

  // 逆順なので、反転する
  reverse(all(commandsNoPass));

  // 最初と最後は駅である
  assert(commandsNoPass.front().kind == kStation);
  assert(commandsNoPass.back().kind == kStation);

  // SA
  commandsNoPass = NBD::Run(commandsNoPass, kTimeLimitSA - global_timer.ms());

  // 線路の向きを正しくする
  // 既に建てた駅をメモしておく
  bitset<50 * 50> isStation = 0;
  for (int i = 0; i < (int)commandsNoPass.size(); i++) {
    const auto command = commandsNoPass[i];
    if (command.kind == kStation) {
      isStation[command.p] = true;
      continue;
    }

    const auto prevCommand = commandsNoPass[i - 1];
    const auto nextCommand = commandsNoPass[i + 1];

    if (commandsNoPass[i].kind == kRail) {
      const auto currentPos = commandsNoPass[i].p;

      Pos prevPos = Pos::Dummy();
      if (prevCommand.kind == kRail) {
        // 一つ前が線路の場合は、その線路に繋ぐ
        prevPos = prevCommand.p;
      } else {
        // そうでない場合は、隣接する駅に繋ぐ
        for (const auto adj : currentPos.Adj()) {
          if (isStation[adj]) {
            prevPos = adj;
            break;
          }
        }
        assert(!prevPos.IsDummy());
      }

      // 次が駅でも線路でも、それに繋ぐ
      Pos nextPos = nextCommand.p;

      // 線路の向きを更新する
      commandsNoPass[i] = Command::Rail(prevPos, currentPos, nextPos);
    }
  }

  // SAの結果、同じマスに建てる線路より駅の方が先に建てられるようになる場合があるため、
  // そのような線路は削除する
  // NOTE:
  // 実装が楽なので、上段で方向の復元をしてから削除処理を行うようにしている
  {
    vector<bool> existStation(N * N, false);
    rep(i, commandsNoPass.size()) {
      const auto pos = commandsNoPass[i].p;
      const auto kind = commandsNoPass[i].kind;
      if (kind == kStation) {
        existStation[pos] = true;
        continue;
      }

      assert(kind == kRail);
      if (existStation[pos]) {
        commandsNoPass.erase(commandsNoPass.begin() + i);
        i--;
      }
    }
  }

  // パスを復元する
  GreedyPassInserter inserter;
  const auto [bestCommands, finalMoney] =
      inserter.InsertPassAndReturnMoney(commandsNoPass);
  return {bestCommands, finalMoney};
}

// 最終的に建てた駅を頂点とした最小全域木を構築し、最小の線路数と実際の線路数をデバッグ出力する
void DebugMST(const vector<Command> &command) {
  vector<Pos> stations;
  int actualRailCount = 0;
  for (const auto &c : command) {
    if (c.kind == kStation) {
      stations.push_back(c.p);
    } else if (c.kind == kRail) {
      actualRailCount++;
    }
  }

  // 最小全域木をプリム法で構築
  vector<Pos> addedStations;
  vector<Pos> remainStations = stations;
  addedStations.push_back(remainStations.back());
  remainStations.pop_back();
  int mstRailCount = 0;
  while (!remainStations.empty()) {
    int minDist = INF;
    Pos minPos = Pos::Dummy();
    for (const auto &added : addedStations) {
      for (const auto &remain : remainStations) {
        const int dist = added.Manhattan(remain);
        if (chmin(minDist, dist)) {
          minPos = remain;
        }
      }
    }
    mstRailCount += max(0, minDist - 1);
    addedStations.push_back(minPos);
    remainStations.erase(find(all(remainStations), minPos));
  }

  cerr << "mst_rail_count: " << mstRailCount
       << " actual_rail_count: " << actualRailCount << endl;
}

// chokudaiサーチを行う関数
void chokudai_search() {
  vector<float> pos2Scores(N * N);
  rep(idx, N * N) {
    const Pos pos(idx);
    // そこに置いたときに得られるincomeの和
    float score = 0;
    for (const auto humanFrom : station2Froms[pos]) {
      score += inputHumans[humanFrom].Income();
    }
    for (const auto humanTo : station2Tos[pos]) {
      score += inputHumans[humanTo].Income();
    }
    pos2Scores[pos] = score;
  }

  const int nodeCapacity = 1 << 20;
  // ノードを格納する
  ObjectPool<Node> allNodes;
  allNodes.Reserve(nodeCapacity);

  // ノード候補の集合
  const int kBW = 5;
  Selector selector(kBW);
  InitFirstSolutionsByFirstSolver(selector, allNodes, pos2Scores);

  cerr << "init_ms: " << global_timer.ms() << endl;

  int bestNode = -1;
  ll maxFinalMoney = -INF;

  const int targetIncome = PARAMS[12];
  int targetIncomeAchiveTurn = INF;

#ifdef LOCAL
  const int debugTargetIncome = 1000;
  int debugTargetIncomeAchiveTurn = INF;
#endif

  while (global_timer.ms() < kTimeLimitBeam) {
    for (int beginTurn = 0; beginTurn < 800;
         beginTurn += (beginTurn < targetIncomeAchiveTurn ? 1 : 1)) {
      if (global_timer.ms() >= kTimeLimitBeam)
        break;
      const int endTurn =
          min(800, beginTurn + (beginTurn < targetIncomeAchiveTurn ? (int)PARAMS[13] : 1));
      // 展開するCandidateを選択する
      const int bestTurn = selector.BestTurn(beginTurn, endTurn);
      if (bestTurn < 0)
        continue;
      const auto baseCandidate = selector.Pop(bestTurn);

      const auto &parentNode = allNodes[baseCandidate.parent];

      // CandidateをNodeとして具現化する
      const auto node = Candidate2Node(baseCandidate, parentNode, pos2Scores);

      if (node.state.moneyState.info.income >= targetIncome) {
        chmin(targetIncomeAchiveTurn, node.evaluator.turn);
      }

#ifdef LOCAL
      if (node.state.moneyState.info.income >= debugTargetIncome) {
        chmin(debugTargetIncomeAchiveTurn, node.evaluator.turn);
      }
#endif

      // Nodeを格納する
      const auto nodeId = allNodes.Push(node);

      // そのままパスをし続けた時の最終スコアを更新し、それが最大なら更新する
      const ll finalMoney = node.state.moneyState.FinalMoney();
      if (chmax(maxFinalMoney, finalMoney)) {
        bestNode = nodeId;
      }

      // Nodeからの遷移をSelectorに追加する
      node.expand(node.evaluator, node.hash, nodeId, selector, Pos::all_poses);
    }
#ifdef DEBUG_BUILD
    break;
#endif
  }

  cerr << "beam_score: " << maxFinalMoney << endl;
  if(maxFinalMoney < K){
    // 何もしないほうが得な場合は、何もしない
    rep(i, T){
      cout << "-1" << endl;
    }
    exit(0);
  }
  if(maxFinalMoney < 0){
    cerr << "invalid: 0.000000001" << endl;
    exit(0);
  }
  cerr << "pool_size: " << allNodes.MaxSize() << endl;

  // 復元
  const auto [commands, finalMoney] = GenerateAnswer(allNodes, bestNode);
  for (const auto &command : commands) {
    cout << command.ToString() << endl;
  }

  cerr << "final_ms: " << global_timer.ms() << endl;
  cerr << "final_score: " << finalMoney << endl;

#ifdef LOCAL
  // cerr << "target_income: " << debugTargetIncome << endl;
  // cerr << "achive_turn: " << debugTargetIncomeAchiveTurn << endl;
  // cerr << global_timers << endl;
#endif

  // DebugMST(commands);

  quick_exit(0);
}

} // namespace Chokudai

int main(int argc, char *argv[]) {
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
  Chokudai::chokudai_search();
  return 0;
}