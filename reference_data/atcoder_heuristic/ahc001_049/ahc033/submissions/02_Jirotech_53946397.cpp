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

// 0: InversionCountの重み ; 0.01
// 1: suitが一緒の場合 ; 5
// 2: suitが異なる場合 ; 1
// 3: maxOutTurnを採用する場合のmanhattanSumの重み ; 0.001
// 4: 暇の重み ; 1
// 5: maxOutTurnを計算し始めるターン ; 250
// 6: パリティを使うなら1 ; 0
// 7: minMaxBoardCountの最大許容値 ; 8
// 8: kBoardGamma ; 1.0
// 9: kBoardCoefBegin ; 0.2

// 10: minBoardCountPenaltyの引く数 ; 5
// 11: 累乗数 ; 2
// 12: 係数 ; 0.05
// vector<double> PARAMS = {0.01, 5, 1, 0.001, 1, 250, 0, 8, 1.0, 0.2, 5, 2,
// 0.05};
vector<double> PARAMS = {0.03273674739290095,
                         13.406466763234965,
                         1,
                         0.14746467674010605,
                         0.8227064160896068,
                         250,
                         0,
                         11,
                         0.9964172783658546,
                         0.2532947113754917,
                         5.929859308878837,
                         1.4963378285367226,
                         0.02419188139259471};

/* start */

Timer global_timer;
int N;
vector<vector<int>> input_containers;

bool IsInPos(const Pos p) { return p.X() == 0; }
bool IsOutPos(const Pos p) { return p.X() == N - 1; }

vector<int> _bsPosGoalDists;
int GetGoalDist(const bitset<15> bs, const Pos start, const int goalY) {
  return _bsPosGoalDists[bs.to_ulong() * N * N * N + start.Idx() * N + goalY];
}
void SetGoalDist(const bitset<15> bs, const Pos start, const int goalY,
                 const int dist) {
  _bsPosGoalDists[bs.to_ulong() * N * N * N + start.Idx() * N + goalY] = dist;
}
void GoalDistInit() { _bsPosGoalDists.resize(N * N * N * (1 << 15)); }
void ChangeBS(bitset<15>& bs, const Pos p, const bool exist) {
  if (p.X() == 0 || p.X() == N - 1) return;
  bs[p.Y() * 3 + p.X() - 1] = exist;
}
bool IsExistBS(const bitset<15>& bs, const Pos p) {
  // X==0をfalseにするのは要件等
  if (p.X() == 0 || p.X() == N - 1) return false;
  return bs[p.Y() * 3 + p.X() - 1];
}

vector<double> _bs2Penalties;
double GetBSPenalty(const bitset<15> bs) {
  return _bs2Penalties[bs.to_ulong()];
}
void SetBSPenalty(const bitset<15> bs, double value) {
  _bs2Penalties[bs.to_ulong()] = value;
}
void AddBSPenalty(const bitset<15> bs, double value) {
  _bs2Penalties[bs.to_ulong()] += value;
}
void BSPenaltyInit() { _bs2Penalties.resize(1 << 15); }

// 盤面評価値の減衰率
double kBoardGamma = 0.0;
// 初期の係数
double kBoardCoefBegin = 0.0;
// 各ターンのボード評価値にかける係数を事前計算
// kBoardGamma ^ turn * kBoardCoefBegin
vector<double> turn2BoardEvalCoefs;
void InitTurnBoardEvalCoefs() {
  turn2BoardEvalCoefs.push_back(kBoardCoefBegin);
  rep(t, 1000) {
    turn2BoardEvalCoefs.push_back(turn2BoardEvalCoefs.back() * kBoardGamma);
  }
}

struct Card {
  int suit;
  int number;

  bool IsDummy() const { return suit == -1; }
  static Card Dummy() { return Card{-1, -1}; }
  bool operator==(const Card& other) const {
    return suit == other.suit && number == other.number;
  }
  bool operator!=(const Card& other) const { return !(*this == other); }
  int ToContainerId() const {
    assert(suit >= 0 && suit < N && number >= 0 && number < N);
    return suit * N + number;
  }
  operator int() const { return ToContainerId(); }
  operator size_t() const { return ToContainerId(); }
  Pos Goal() const { return Pos(N - 1, suit); }
  int GoalManhattan(Pos p) const { return Goal().Manhattan(p); }
};
using Deck = vector<vector<Card>>;

Deck CreateInitDeck() {
  Deck deck(N);
  rep(i, N) {
    REP(j, N) {
      deck[i].push_back(
          Card{input_containers[i][j] / N, input_containers[i][j] % N});
    }
  }
  return deck;
}

vector<vector<Card>> input_cards;
vector<pair<int, int>> input_card2ij;

struct Output {
  static void StaticInit(istream& is) {
    global_timer.start();
    kBoardGamma = PARAMS[8];
    kBoardCoefBegin = PARAMS[9];

    is >> N;
    input_containers.resize(N);
    input_cards.resize(N);
    input_card2ij.resize(N * N);
    rep(i, N) {
      rep(j, N) {
        int a;
        is >> a;
        input_containers[i].push_back(a);
        const Card card{a / N, a % N};
        input_cards[i].push_back(card);
        input_card2ij[card] = {i, j};
      }
    }

    Pos::StaticInit(N);

    // 障害物込みでの搬出口までの最短距離を事前計算
    GoalDistInit();
    rep(bit, 1 << 15) {
      bitset<15> bs(bit);
      rep(goal_y, N) {
        Pos goal(N - 1, goal_y);
        vector<int> minDists(N * N, INF);
        minDists[goal] = 0;
        queue<Pos> q;
        q.push(goal);
        while (q.size() > 0) {
          Pos p = q.front();
          q.pop();
          for (const auto adj : p.Adj()) {
            if (IsExistBS(bs, adj)) {
              continue;
            }
            if (chmin(minDists[adj], minDists[p] + 1)) {
              q.push(adj);
            }
          }
        }
        rep(y, N) {
          rep(x, N) {
            Pos start(x, y);
            int minDist = minDists[start];
            // スタート地点に置かれていた場合はINFになるが、
            // 実際には移動できるので、周囲のmin + 1をする
            for (const auto adj : start.Adj()) {
              if (IsExistBS(bs, adj)) continue;
              chmin(minDist, minDists[adj] + 1);
            }
            SetGoalDist(bs, start, goal_y, minDist);
          }
        }
      }
    }

    // 盤面配置による潜在コストを事前に評価
    BSPenaltyInit();
    rep(bit, 1 << 15) {
      bitset<15> bs(bit);
      // 25通りの運び方のうち、遠回りしなければいけない運び方の個数をカウント
      int ukaiCount = 0;
      rep(start_y, N) {
        Pos start(0, start_y);
        rep(goal_y, N) {
          Pos goal(N - 1, goal_y);
          if (start.Manhattan(goal) < GetGoalDist(bs, start, goal_y)) {
            ukaiCount++;
          }
        }
      }

      AddBSPenalty(bs, ukaiCount);

      // 四方を囲まれている個数
      // int kakomareCount = 0;
      // rep(y, N) {
      //   rep(x, N) {
      //     Pos p(x, y);
      //     bool ok = false;
      //     for (const auto adj : p.Adj()) {
      //       // X==0も存在しているとみなす
      //       if (adj.X() == 0) continue;
      //       if (IsExistBS(bs, adj)) continue;
      //       ok = true;
      //       break;
      //     }
      //     if (!ok) {
      //       kakomareCount++;
      //     }
      //   }
      // }

      // AddBSPenalty(bs, kakomareCount * 0.2);
    }
    InitTurnBoardEvalCoefs();
  }
};








/* start */
struct Agent {
  int id;
  Pos p;
  Card card;
  Pos prevPos;

  bool CanMove(Dir dir) const {
    Pos ppp = p;
    if (!ppp.Move(dir)) return false;
    return true;
  }
  void MoveTo(const Pos ppp) {
    prevPos = p;
    p = ppp;
  }
  bool IsCatch() const { return !card.IsDummy(); }
  void Catch(const Card _card) { card = _card; }
  void Put() { card = Card::Dummy(); }
  const Pos& P() const { return p; }
  bool IsBig() const { return id == 0; }
};

struct AgentBoard {
  vector<Agent> agents;
  vector<vector<int>> pos2AgentIds;

  AgentBoard() {}
  AgentBoard(vector<Agent>& agents) : agents(agents), pos2AgentIds(N * N) {
    for (const auto a : agents) {
      pos2AgentIds[a.P()].push_back(a.id);
    }
  }

  const Agent& Get(int ai) const { return agents[ai]; }

  bool CanMove(int ai, Dir dir) const {
    const auto& agent = agents[ai];
    // Agent都合で動けない
    if (!agent.CanMove(dir)) return false;

    Pos ppp = agent.P();
    ppp.Move(dir);

    // すれ違い禁止
    for (const auto otherAi : pos2AgentIds[agent.P()]) {
      if (ai == otherAi) continue;
      assert(agents[otherAi].P() == agent.P());
      if (agents[otherAi].prevPos == ppp) return false;
    }

    if (pos2AgentIds[ppp].size()) {
      if (pos2AgentIds[ppp].size() >= 2) return false;
      for (const auto otherAi : pos2AgentIds[ppp]) {
        // 移動先に既に行動を確定したエージェント
        if (otherAi < ai) return false;
      }
    }

    return true;
  }

  void Move(int ai, Dir dir) {
    auto& agent = agents[ai];
    Pos p = agent.P();
    auto ppp = p;
    ppp.Move(dir);
    agent.MoveTo(ppp);
    if ((int)pos2AgentIds[p].front() == ai) {
      // swapしてからpop_back
      swap(pos2AgentIds[p].front(), pos2AgentIds[p].back());
    }
    assert(pos2AgentIds[p].back() == ai);
    pos2AgentIds[p].pop_back();
    pos2AgentIds[ppp].push_back(ai);
    assert(pos2AgentIds[ppp].size() <= 2);
  }

  void BackwardMove(int ai, Dir dir) {
    Move(ai, InvDir(dir));
    agents[ai].prevPos = Pos::Dummy();
  }

  bool MustMove(int ai) const {
    return (int)pos2AgentIds[agents[ai].P()].size() >= 2;
  }

  void Catch(const int ai, const Card card) { agents[ai].Catch(card); }

  bool CanCatch(const int ai) const {
    return !agents[ai].IsCatch() && !MustMove(ai);
  }

  Card BackwardCatch(const int ai) {
    const auto card = agents[ai].card;
    agents[ai].Put();
    return card;
  }

  void Put(const int ai) { agents[ai].Put(); }

  bool CanPut(const int ai) const {
    return agents[ai].IsCatch() && !MustMove(ai);
  }

  void BackwardPut(const int ai, const Card card) { agents[ai].Catch(card); }

  bool CanWait(const int ai) const { return !MustMove(ai); }

  void Wait() const { return; }

  void BackwardWait() const { return; }
};

struct CardBoard {
  vector<Card> pos2Cards;
  vector<Pos> card2Poses;
  vector<int> row2NextInIndexes;
  vector<int> row2NextOutNums;
  vector<int> col2Counts;
  // 各Inの出現ターン
  vector<vector<int>> row2AppearedTurns;
  bitset<15> existBS;
  int turn = 0;

  int minX = 0;

  CardBoard()
      : pos2Cards(N * N, Card::Dummy()),
        card2Poses(N * N, Pos::Dummy()),
        row2NextInIndexes(N, 0),
        row2NextOutNums(N, 0),
        col2Counts(N),
        row2AppearedTurns(N) {
    col2Counts[0] = N * N;
    existBS = 0;
  }

  // 入口ならその位置を返す・出口またはエージェントならDummyを返す
  Pos FindCard(const Card& card) const {
    assert(!card.IsDummy());
    assert(card.suit < N);
    assert(card.number < N);
    assert((int)card >= 0);
    assert((int)card < (int)card2Poses.size());
    // 置かれている
    const auto& p = card2Poses[card];
    if (!p.IsDummy()) return p;

    // 入口か判定
    const auto [i, j] = input_card2ij[card];
    if (row2NextInIndexes[i] <= j) {
      // 入口
      return Pos(0, i);
    }

    // 出口またはエージェント
    return Pos::Dummy();
  }

  // 各入口からドローを行った回数
  // 出現 + 持ち上げまで行ったらドローをしたとみなす
  vector<int> DrawCounts() const {
    vector<int> drawCounts(N);
    rep(i, N) {
      int nextInIdx = row2NextInIndexes[i];
      if (nextInIdx - 1 >= 0) {
        // 直前のやつがまだ残ってるなら、ドローできてないので-1
        const auto& prevCard = input_cards[i][nextInIdx - 1];
        if (pos2Cards[Pos(0, i)] == prevCard) {
          nextInIdx--;
        }
      }
      drawCounts[i] = nextInIdx;
    }
    return drawCounts;
  }

  bool Exist(Pos p) const { return !pos2Cards[p].IsDummy(); }
  bool CanMoveToWithCard(Pos p) const {
    // 箱が出現予定の場所への移動は本来可能だが、評価値がかなり良くなるので不可能としている
    // おそらくハッシュの衝突によるものだが、コンテスト終了までにハッシュの見直しをしている時間が無い
    return pos2Cards[p].IsDummy();
  }

  void BackwardTurnStart() {
    assert(turn % 5 == 0);
    rep(y, N) {
      if (row2AppearedTurns[y].size() > 0 &&
          row2AppearedTurns[y].back() == turn) {
        const Pos p(0, y);
        assert(!pos2Cards[p].IsDummy());
        row2NextInIndexes[y]--;
        const auto card = pos2Cards[p];
        pos2Cards[p] = Card::Dummy();
        card2Poses[card] = Pos::Dummy();
        row2AppearedTurns[y].pop_back();
      }
    }
  }

  void IncCol(const int x) {
    col2Counts[x]++;
    chmin(minX, x);
  }

  void DecCol(const int x) {
    col2Counts[x]--;
    if (x != minX) return;
    while (minX < N && col2Counts[minX] == 0) {
      minX++;
    }
    assert(minX < N);
  }

  Card TryCatchCard(const Pos p) {
    if (pos2Cards[p].IsDummy()) {
      return Card::Dummy();
    }
    const auto card = pos2Cards[p];
    card2Poses[card] = Pos::Dummy();
    pos2Cards[p] = Card::Dummy();
    DecCol(p.X());
    ChangeBS(existBS, p, false);
    return card;
  }

  bool TryPutCard(const Pos p, const Card& card) {
    if (!pos2Cards[p].IsDummy()) return false;
    if (p.X() == N - 1 &&
        (p.Y() != card.suit || row2NextOutNums[p.Y()] != card.number))
      return false;
    // 評価値への反映を早くするために、荷物の出荷は即時行っている
    // ターン開始時までにそれで不都合は生じないので、問題ない
    if (p.X() == N - 1) {
      assert(row2NextOutNums[card.suit] == card.number);
      card2Poses[card] = Pos::Dummy();
      row2NextOutNums[card.suit]++;
    } else {
      pos2Cards[p] = card;
      card2Poses[card] = p;
    }
    IncCol(p.X());
    ChangeBS(existBS, p, true);

    return true;
  }

  void BackwardCatchCard(const Pos p, const Card card) {
    const auto prevCard = pos2Cards[p];
    assert(prevCard.IsDummy());
    pos2Cards[p] = card;
    card2Poses[card] = p;
    IncCol(p.X());
    ChangeBS(existBS, p, true);
  }

  Card BackwardPutCard(const Pos p) {
    auto card = pos2Cards[p];
    if (card.IsDummy()) {
      assert(p.X() == N - 1);
      card = Card{p.Y(), --row2NextOutNums[p.Y()]};
    }
    card2Poses[card] = Pos::Dummy();
    pos2Cards[p] = Card::Dummy();
    // 念のためもう一度更新
    if (p.X() == N - 1) {
      row2NextOutNums[p.Y()] = card.number;
    }
    DecCol(p.X());
    ChangeBS(existBS, p, false);
    return card;
  }

  void TurnStart(const AgentBoard& agentBoard) {
    assert(turn % 5 == 0);
    // 荷物の出現
    rep(y, N) {
      const Pos p(0, y);
      if (pos2Cards[p].IsDummy() && row2NextInIndexes[y] < N) {
        // 荷物持ちの小さなエージェントが居たら湧かない
        const auto ais = agentBoard.pos2AgentIds[p];
        if (ais.size()) {
          assert((int)ais.size() == 1);
          const auto ai = ais.front();
          const auto& agent = agentBoard.Get(ai);
          if (agent.IsCatch()) {
            continue;
          }
        }
        pos2Cards[p] = input_cards[y][row2NextInIndexes[y]++];
        card2Poses[pos2Cards[p]] = p;
        row2AppearedTurns[y].push_back(turn);
      }
    }

    // 荷物の出荷は置いた瞬間に行っているのでskip
  }

  bool Debug() const {
    rep(y, N) {
      rep(x, N) {
        Pos p(x, y);
        auto card = pos2Cards[p];
        assert(card2Poses[card] == p);
      }
    }

    rep(suit, N) {
      rep(num, N) {
        Card card{suit, num};
        auto p = card2Poses[card.ToContainerId()];
        assert(pos2Cards[p] == card);
      }
    }
  }
};

struct Board {
  CardBoard cardBoard;
  AgentBoard agentBoard;
  vector<int> card2Agents;

  Board() {
    vector<Agent> agents;
    rep(y, N) {
      agents.push_back(Agent{y, Pos(0, y), Card::Dummy(), Pos(0, y)});
    }
    agentBoard = AgentBoard(agents);
    card2Agents.resize(N * N, -1);
    TurnStart();
  }

  bool TryMove(const int ai, const Dir dir) {
    auto& agent = agentBoard.agents[ai];
    // エージェント都合で動けない
    if (!agentBoard.CanMove(ai, dir)) return false;
    auto ppp = agent.p;
    ppp.Move(dir);
    // コンテナ都合で動けない
    if (agent.IsCatch() && !agent.IsBig() &&
        !cardBoard.CanMoveToWithCard(ppp)) {
      return false;
    }
    agentBoard.Move(ai, dir);
    return true;
  }

  bool TryCatch(const int ai) {
    if (!agentBoard.CanCatch(ai)) return false;
    const auto card = cardBoard.TryCatchCard(agentBoard.agents[ai].P());
    if (card.IsDummy()) return false;
    agentBoard.Catch(ai, card);
    card2Agents[card] = ai;

    return true;
  }

  bool TryPut(const int ai) {
    if (!agentBoard.CanPut(ai)) return false;
    const auto& agent = agentBoard.agents[ai];
    if (!cardBoard.TryPutCard(agent.P(), agent.card)) return false;
    card2Agents[agent.card] = -1;
    agentBoard.Put(ai);

    return true;
  }

  bool TryWait(const int ai) {
    if (!agentBoard.CanWait(ai)) return false;
    agentBoard.Wait();
    return true;
  }

  // BackwardTurnStartは以下の理由により実装していない
  // * 復元が難しい
  // * ビームサーチにおいて、BackwardTurnStartをした後にTurnStartをせずにexpandすることはない
  // * Put/Catchの合わない辻褄はBackwardPut/BackwardCatchのタイミングで修正
  void TurnStart() { cardBoard.TurnStart(agentBoard); }

  // 順方向の処理を打ち消す
  void BackwardMove(const int ai, const Dir dir) {
    agentBoard.BackwardMove(ai, dir);
  }

  void BackwardCatch(const int ai) {
    const auto card = agentBoard.BackwardCatch(ai);
    cardBoard.BackwardCatchCard(agentBoard.Get(ai).P(), card);
    card2Agents[card] = -1;
  }

  void BackwardPut(const int ai) {
    const auto card = cardBoard.BackwardPutCard(agentBoard.Get(ai).P());
    agentBoard.BackwardPut(ai, card);
    card2Agents[card] = ai;
  }

  bool TryForward(const int ai, const char c) {
    bool success = false;
    if (c == 'U') {
      success = TryMove(ai, kU);
    } else if (c == 'D') {
      success = TryMove(ai, kD);
    } else if (c == 'L') {
      success = TryMove(ai, kL);
    } else if (c == 'R') {
      success = TryMove(ai, kR);
    } else if (c == '.') {
      success = TryWait(ai);
    } else if (c == 'P') {
      success = TryCatch(ai);
    } else if (c == 'Q') {
      success = TryPut(ai);
    }
    if (success) {
      cardBoard.turn++;
    }
    return success;
  }

  void Backward(const int ai, const char c) {
    cardBoard.turn--;
    if (c == 'U') {
      BackwardMove(ai, kU);
    } else if (c == 'D') {
      BackwardMove(ai, kD);
    } else if (c == 'L') {
      BackwardMove(ai, kL);
    } else if (c == 'R') {
      BackwardMove(ai, kR);
    } else if (c == '.') {
    } else if (c == 'P') {
      BackwardCatch(ai);
    } else if (c == 'Q') {
      BackwardPut(ai);
    } else {
      assert(false);
    }
  }

  void BackwardTurnStart() { cardBoard.BackwardTurnStart(); }

  // 各行のOutの排出にかかる最大手数
  int MaxOutTurn(const int nextAi) const {
    int ret = 0;
    rep(y, N) {
      for (int number = cardBoard.row2NextOutNums[y]; number < N; number++) {
        // y行目のnumberの排出にかかるターン数
        // 自身のマンハッタン距離 + CatchPut + 後続の個数 * 2
        int minTurn = INF;
        const Card card{y, number};
        const auto& pos = cardBoard.card2Poses[card];
        if (!pos.IsDummy()) {
          // ボードに置いてある
          rep(ai, N) {
            const auto& agent = agentBoard.Get(ai);
            // そのエージェントが最速で取りに行った場合の、置くまでのターン数
            // 行動済みなら+1
            int turn = (agent.id < nextAi ? 1 : 0);
            if (agent.IsCatch()) {
              // 届けてから取りに行く
              // その際の排出待ちは考慮しない
              // 移動 + 置く + 取りに行く + 拾う + 移動 + 置く
              turn += agent.card.GoalManhattan(agent.P()) + 1 +
                      pos.Manhattan(agent.card.Goal()) + 1 +
                      card.GoalManhattan(pos) + 1;
            } else {
              // 取りに行く + 拾う + 移動 + 置く
              turn +=
                  pos.Manhattan(agent.P()) + 1 + card.GoalManhattan(pos) + 1;
            }
            chmin(minTurn, turn);
          }
        } else {
          // 入口か、エージェント
          const auto [i, j] = input_card2ij[card];
          // 入口にある場合
          if (cardBoard.row2NextInIndexes[i] <= j) {
            Pos pos(0, i);
            rep(ai, N) {
              const auto& agent = agentBoard.Get(ai);
              // そのエージェントが最速で取りに行った場合の、置くまでのターン数
              // まずは回収までのターン数を求める
              // 行動済みなら+1
              int takeTurn = (agent.id < nextAi ? 1 : 0);
              if (agent.IsCatch()) {
                // 届けてから取りに行く
                // その際の排出待ちは考慮しない
                // 移動 + 置く + 取りに行く + 拾う (+ 移動 + 置く)
                takeTurn += agent.card.GoalManhattan(agent.P()) + 1 +
                            pos.Manhattan(agent.card.Goal()) + 1;
              } else {
                // 取りに行く + 拾う (+ 移動 + 置く)
                takeTurn += pos.Manhattan(agent.P()) + 1;
              }

              // 搬入待ち
              // 自分の前に搬出されるべき個数
              // 未回収分も考慮している
              const int prevCount =
                  (j - cardBoard.row2NextInIndexes[i]) +
                  (cardBoard.pos2Cards[Pos(0, i)].IsDummy() ? 0 : 2);
              // 1個当たり2回(回収 + 退く)
              chmax(takeTurn, prevCount * 2);

              // 回収後、移動 + 置く
              int turn = takeTurn + card.GoalManhattan(pos) + 1;
              chmin(minTurn, turn);
            }
          } else {
            // エージェントの場合は後で評価
            continue;
          }
        }

        // 後続処理分
        minTurn += (N - 1 - number) * 2;
        chmax(ret, minTurn);
      }
    }

    // エージェントの分
    rep(ai, N) {
      const auto& agent = agentBoard.Get(ai);
      const auto& card = agent.card;
      if (card.IsDummy()) continue;
      // 行動済みなら+1
      int turn = (agent.id < nextAi ? 1 : 0);
      turn += card.GoalManhattan(agent.P()) + 1;
      // 後続処理分
      turn += (N - 1 - card.number) * 2;
      chmax(ret, turn);
    }
    return ret;
  }
};




/* start */

// この問題をフリーセルに簡易化して分析を行うソルバー
// 元の問題におけるコンテナのことをカードと呼ぶことにする
// i行目の搬出口にj番目に搬出するコンテナのことを、スートi, 数字jのカードと呼ぶ
// i行目の搬入口をi番目のデッキと呼ぶ
// 各デッキにおいてドロー済みのカードの個数を状態とする(6^5通り)
// ドロー済みカードのうち、搬出できるカードは即座に搬出するものとし、
// 残ったカードはフリーセルに退避することになる
// 必要なフリーセルの最大枚数などについて、DPで解析を行う
namespace FreeCell {

struct StateInfo {
  // デッキに残っている枚数
  int remainInputCount = 0;
  // ボード上に避難している枚数
  int boardCount = 0;
  // 搬出済みの枚数
  int doneOutputCount = 0;
  // 最適な状態遷移を選択し続けた場合の、board上の個数の最大値の最小値
  int minMaxBoardCount = -1;
  // 現在の状態から行うべき最小の退避回数
  // 選択する遷移はminMaxBoardCountとは異なってもよいものとしている
  // 以降の変数についても同様
  int minSpillCount = 0;
  double expectedSpillCount = 0;
  // 必要な退避または提出できる、ドローの個数(0~5)
  int taskCount = 0;
  // 各時点でのボードの個数をペナルティ換算し累積
  double minBoardCountPenalty = 0.0;
  // ドロー済みカードをすべて即座に反映した場合の、各スートの次に搬出する数字
  vector<int> row2NextOutNums;
};

// 各デッキのドロー済み枚数に対する様々なパラメータ
vector<StateInfo> stateInfos;

uint64_t Index(const int y, const int count) {
  constexpr int kCoefs[] = {1, 6, 6 * 6, 6 * 6 * 6, 6 * 6 * 6 * 6};
  return kCoefs[y] * count;
}

// 各デッキのドロー済み枚数を、stateInfosのIndexに変換する
// 0~5が5個で6^5通り
uint64_t Index(const vector<int>& drawCounts) {
  uint64_t index = 0;
  rep(y, 5) { index += Index(y, drawCounts[y]); }
  return index;
}

struct Solver {
  Solver() {}

  // 各状態の基本的なパラメータを求める
  // デッキの残り枚数、ボード上の避難枚数、提出済み枚数など
  void DFS(vector<int>& drawCounts, vector<vector<bool>>& isDrawed) {
    if ((int)drawCounts.size() == N) {
      // 各デッキのドロー済み枚数に対して、
      // デッキの残り枚数、ボード上の避難枚数、提出済み枚数をカウント
      const int idx = Index(drawCounts);
      StateInfo info;
      rep(i, N) {
        bool ok = true;
        rep(j, N) {
          if (isDrawed[i][j]) {
            if (ok) {
              info.doneOutputCount++;
            } else {
              info.boardCount++;
            }
          } else {
            ok = false;
            info.remainInputCount++;
          }
        }
      }
      assert(info.doneOutputCount + info.boardCount + info.remainInputCount ==
             N * N);
      info.row2NextOutNums.resize(N, N);
      rep(i, N) {
        rep(j, N) {
          if (!isDrawed[i][j]) {
            info.row2NextOutNums[i] = j;
            break;
          }
        }
      }
      stateInfos[idx] = info;
      return;
    }

    // 各デッキのドロー済み枚数を再帰的に生成している
    const int i = drawCounts.size();
    rep(k, 6) {
      if (k >= 1) {
        // ドロー済みフラグを立てる
        const auto& card = input_cards[i][k - 1];
        isDrawed[card.suit][card.number] = true;
      }
      drawCounts.push_back(k);
      DFS(drawCounts, isDrawed);
      drawCounts.pop_back();
    }

    // ドロー済みフラグを元に戻す
    rep(k, 6) {
      if (k >= 1) {
        const auto& card = input_cards[i][k - 1];
        isDrawed[card.suit][card.number] = false;
      }
    }
  }

  // 各状態のパラメータのうち、遷移先状態を参照して求めるパラメータをDPで計算する
  // board上の個数の最大値の最小値、退避回数の最小値など
  void DP(vector<int>& drawCounts) {
    if ((int)drawCounts.size() == N) {
      // 遷移先の各パラメータは既に計算済みなので、
      // 現在の状態のパラメータを計算する
      const int idx = Index(drawCounts);
      int minMaxBoardCount = INF;
      int minSpillCount = INF;
      vector<double> y2MinExpSpillEval(N, INF);
      double minMinBoardCountPenalty = INF;
      rep(i, 5) {
        if (drawCounts[i] == 5) continue;
        // i番目のデッキをドローすることで遷移した場合の各パラメータを求めて、
        // 最小値をとる
        drawCounts[i]++;
        const int nextIdx = Index(drawCounts);
        const auto& nextInfo = stateInfos[nextIdx];
        auto nextMinSpillCount = nextInfo.minSpillCount;
        auto nextExpectedSpillCount = nextInfo.expectedSpillCount;
        // 遷移のために退避をしたら+1
        if (stateInfos[idx].boardCount < nextInfo.boardCount) {
          assert(stateInfos[idx].boardCount + 1 == nextInfo.boardCount);
          nextMinSpillCount++;
          nextExpectedSpillCount += 2;
        }
        chmin(minMaxBoardCount, nextInfo.minMaxBoardCount);
        chmin(minSpillCount, nextMinSpillCount);
        chmin(minMinBoardCountPenalty, nextInfo.minBoardCountPenalty);
        rep(y, 5) {
          // yから出現して、iまで来て、その行動を選択
          chmin(y2MinExpSpillEval[y], nextExpectedSpillCount + abs(y - i));
        }
        drawCounts[i]--;
      }
      // 状態自身のパラメータを代入
      stateInfos[idx].minMaxBoardCount = stateInfos[idx].boardCount;
      stateInfos[idx].minBoardCountPenalty =
          pow(max(0.0, stateInfos[idx].boardCount - PARAMS[10]), PARAMS[11]) *
          PARAMS[12];
      stateInfos[idx].expectedSpillCount = 0.0;

      // 最終状態は遷移先が無い
      if (minMaxBoardCount == INF) return;

      // 各yは等確率で発生するが、
      // 一手確率で選ぶことができる
      constexpr double kBestChooseProb = 0.8;
      double minExpSpillEval = INF;
      rep(y, N) {
        stateInfos[idx].expectedSpillCount +=
            y2MinExpSpillEval[y] / N * (1.0 - kBestChooseProb);
        chmin(minExpSpillEval, y2MinExpSpillEval[y]);
      }
      stateInfos[idx].expectedSpillCount += minExpSpillEval * kBestChooseProb;

      // 遷移先のminとのmaxをとる
      chmax(stateInfos[idx].minMaxBoardCount, minMaxBoardCount);
      stateInfos[idx].minSpillCount = minSpillCount;

      // 意味のあるドロー(それをドローすることによって、次の状態の退避回数が1減る)の個数をカウントする
      auto memoDrawCounts = drawCounts;
      rep(i, 5) {
        if (drawCounts[i] == 5) continue;
        drawCounts[i]++;
        const int nextIdx = Index(drawCounts);
        const auto& nextInfo = stateInfos[nextIdx];
        if (stateInfos[idx].minSpillCount > nextInfo.minSpillCount) {
          // 退避回数が減るので、意味のあるドロー
          stateInfos[idx].taskCount++;
        } else if (stateInfos[idx].doneOutputCount < nextInfo.doneOutputCount) {
          // 提出可能なので、意味のあるドロー
          stateInfos[idx].taskCount++;
        }
        drawCounts[i]--;
      }

      stateInfos[idx].minBoardCountPenalty += minMinBoardCountPenalty;
      return;
    }

    // 各デッキのドロー済み枚数を再帰的に生成している
    // 各デッキについてドロー枚数が多い順にDFSすることで、
    // 各状態を評価する際には遷移先の状態が評価済みになっている
    REP(k, 6) {
      drawCounts.push_back(k);
      DP(drawCounts);
      drawCounts.pop_back();
    }
  }

  void Solve() {
    stateInfos.resize(pow(6, 5));
    vector<int> drawCounts;
    vector<vector<bool>> isDrawed(5, vector<bool>(6));
    DFS(drawCounts, isDrawed);
    DP(drawCounts);
    cerr << "minMaxBoardCount: " << stateInfos[0].minMaxBoardCount << endl;
    return;
  }
};

}  // namespace FreeCell

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


/* start */

/*
実装手順(自分用メモ)
1. Stateのコンストラクタ: 初期状態を定義
2. Action: 行動を表すデータstructを定義
3. State.move_forward, move_backward: Actionを適用したときのStateの変化を実装
4. Evaluator: 盤面の評価値を定義
5: State.expand: 行動を適用した後の評価値やハッシュ値などを計算
6: 外側: configを適切に設定し、適当な初期Nodeを用いて、beam_search()を呼び出す
*/

namespace Beam {
int turn = 0;
using Hash = uint32_t;
using Cost = double;

/* ここから先はハッシュを求める処理 */

/*
状態のハッシュ化にはZobristHashを使用しています。
その際、厳密には違う状態であっても、
似た状態はあえて区別しないことで、
状態の多様性を確保しています。

各状態について、以下の要素が一致する状態には同一のハッシュが割り当てられます
* 各行ごとの(大クレーンの個数, 小クレーンの個数)
* 各搬入口ごとのCatchした個数
* 各搬出口ごとの搬出した個数
* 中央列(0列目と4列目以外)の置かれた荷物の有無

以下は考慮・区別していないことに注意してください。
* IDの異なる2つの小クレーン
* クレーンのX座標
* 0列目に置かれた荷物の有無
  * ただし、「各搬入口ごとのCatchした個数」で概ねのケースは考慮している
*/

uint32_t Pow6(const int y) {
  constexpr int kVals[] = {1, 6, 6 * 6, 6 * 6 * 6, 6 * 6 * 6 * 6};
  return kVals[y];
}

// (y座標, 小クレーンの個数)に対してランダムなハッシュ値を割り当てる
vector<Hash> _smallCranePosHash;
Hash SmallCranePosHash(const uint32_t smallCraneIdx) {
  if ((int)_smallCranePosHash.size() == 0) {
    _smallCranePosHash.resize(6 * 6 * 6 * 6 * 6);
    rep(i, _smallCranePosHash.size()) { _smallCranePosHash[i] = xorshift(); }
  }
  return _smallCranePosHash[smallCraneIdx];
}

// 大クレーンのy座標に対してランダムなハッシュ値を割り当てる
vector<Hash> _bigCranePosHash;
Hash BigCranePosHash(const Pos p) {
  if ((int)_bigCranePosHash.size() == 0) {
    _bigCranePosHash.resize(N);
    rep(i, _bigCranePosHash.size()) { _bigCranePosHash[i] = xorshift(); }
  }
  return _bigCranePosHash[p.Y()];
}

// 搬入の進捗状況(6^5通り)に対してランダムなハッシュ値を割り当てる
vector<Hash> _drawCountHash;
Hash DrawCountHash(const int drawCountIdx) {
  if ((int)_drawCountHash.size() == 0) {
    _drawCountHash.resize(FreeCell::stateInfos.size());
    rep(i, FreeCell::stateInfos.size()) { _drawCountHash[i] = xorshift(); }
  }
  return _drawCountHash[drawCountIdx];
}
Hash DrawCountHash(const vector<int>& drawCounts) {
  return DrawCountHash(FreeCell::Index(drawCounts));
}

// 搬出の進捗状況(6^5通り)に対してランダムなハッシュ値を割り当てる
vector<Hash> _completeHash;
Hash CompleteHash(const int y, const int nextNum) {
  if ((int)_completeHash.size() == 0) {
    _completeHash.resize(N * 6);
    rep(i, N * 6) { _completeHash[i] = xorshift(); }
  }
  return _completeHash[y * 6 + nextNum];
}
Hash CompleteHash(const vector<int>& row2NextOutIdxes) {
  Hash hash = 0;
  rep(i, N) { hash ^= CompleteHash(i, row2NextOutIdxes[i]); }
  return hash;
}

// 0,4列目以外の各位置(15箇所)に対してランダムなハッシュ値を割り当てる
// 置かれているならそのハッシュ値を使用する
vector<Hash> _putPosHash;
Hash PutPosHashNotInOut(Pos p) {
  if ((int)_putPosHash.size() == 0) {
    // 25個確保しているが、実際に使用されるのは15個
    _putPosHash.resize(N * N);
    rep(i, N * N) { _putPosHash[i] = xorshift(); }
  }
  // 0,4列目は区別しないため、0とする
  if (p.X() == 0 || p.X() == N - 1) return 0;
  return _putPosHash[p];
}

// 状態遷移を行うために必要な情報
// メモリ使用量をできるだけ小さくしてください
struct Action {
  int ai;
  char c;
  Action(int ai, char c) : ai(ai), c(c) {}
};

/*

評価関数 1次元+独立
* 箱のマンハッタンの総和
* 帰る距離(置かれてる箱のX距離の総和 + 持ってるエージェント*4 +
持ってないエージェントの左までの距離)
* 掴む離すの必要数(置かれている箱*2 + 現在の状態から行うべき退避の回数*2 +
持っている出口準備済みの箱*1 + 持っている出口未準備の箱*3)

2次元
* 無駄な縦移動の回数
  * 出口->入口のずれ
  * 箱が邪魔で運べない
* パスした回数
* 終了タイミングが揃わず暇した回数

*/

// 入口・未出現も含む
int EvaluatePuttedCard(const Pos p, const Card& card) {
  if (p.X() == N - 1) return 0;
  int ret = 0;
  // 荷物の評価: ゴールまでのマンハッタン距離
  ret += card.GoalManhattan(p);
  // 荷物の評価: エージェントの将来の帰宅距離
  ret += -p.X();
  // 荷物の評価: 掴む離すの必要数
  ret += 2;
  return ret;
}

int GetGoalDistConsideringX0(const Board& board, const bitset<15> existBS,
                             const Pos& agentPos, const int goalY) {
  return GetGoalDist(existBS, agentPos, goalY);

  if (agentPos.X() != 0) {
    return GetGoalDist(existBS, agentPos, goalY);
  }

  // 右、下、上
  int minDist = N * N * N;
  const int y = agentPos.Y();
  if (board.cardBoard.pos2Cards[Pos(1, y)].IsDummy()) {
    chmin(minDist, GetGoalDist(existBS, Pos(1, y), goalY) + 1);
  }
  if (y - 1 >= 0 &&
      (board.cardBoard.pos2Cards[Pos(0, y - 1)].IsDummy() ||
       (board.agentBoard.pos2AgentIds[Pos(0, y - 1)].size() > 0))) {
    chmin(minDist, GetGoalDist(existBS, Pos(0, y - 1), goalY) + 1);
  }
  if (y + 1 < N &&
      (board.cardBoard.pos2Cards[Pos(0, y + 1)].IsDummy() ||
       (board.agentBoard.pos2AgentIds[Pos(0, y + 1)].size() > 0))) {
    chmin(minDist, GetGoalDist(existBS, Pos(0, y + 1), goalY) + 1);
  }
  return minDist;
}

int EvaluateAgent(const Agent& agent, const vector<int>& row2NextOutNumbers,
                  const int minX, const vector<int>& card2Agents,
                  const Board& board, int nextAi) {
  int ret = 0;
  // エージェントの評価: エージェントの帰宅距離
  ret += max(0, agent.P().X() - minX);
  if (agent.IsCatch()) {
    const auto& card = agent.card;
    assert(!card.IsDummy());
    // 荷物の評価: ゴールまでのマンハッタン距離
    ret += card.GoalManhattan(agent.P());
    // 荷物の評価: エージェントの将来の帰宅距離
    ret += -agent.P().X();
    if (row2NextOutNumbers[card.suit] == card.number) {
      // 搬出済みの場合
      // 荷物の評価: 掴む離すの必要回数
      // 持っている出口準備済みの箱: Put
      ret += 1;
      if (!agent.IsBig() &&
          GetGoalDistConsideringX0(board, board.cardBoard.existBS, agent.P(),
                                   card.suit) > card.GoalManhattan(agent.P())) {
        // 荷物の評価: 掴む離すの必要回数
        // 遠回りする場合は、一度退避する or 迂回する必要がある
        ret += 2;
      }
    } else if (row2NextOutNumbers[card.suit] == card.number - 1 &&
               card2Agents[Card{card.suit, card.number - 1}] != -1) {
      // もうすぐ準備済みになりそうな場合
      // 前のエージェント
      const auto otherCard = Card{card.suit, card.number - 1};
      const int otherAi = card2Agents[otherCard];
      const auto& otherAgent = board.agentBoard.Get(otherAi);
      // 荷物の評価: 掴む離すの必要回数
      ret += 1;
      if (!agent.IsBig() &&
          GetGoalDistConsideringX0(board, board.cardBoard.existBS, agent.P(),
                                   card.suit) > card.GoalManhattan(agent.P())) {
        // 荷物の評価: 掴む離すの必要回数
        // 遠回りする場合は、一度退避する or 迂回する必要がある
        ret += 2;
      } else {
        // 前のエージェントを待つ必要がある
        // 今を0ターンとしたときに、前のエージェントが搬出して、どいて、自分が搬出
        // 前を考慮しつつ自身は瞬間移動できるとした場合にかかるターン数
        // 行動済みなら+1 前が移動 前が搬出 前が退く(同時に自分が入る)
        // 自分が搬出
        const int otherTurn = (otherAi < nextAi ? 1 : 0) +
                              otherCard.GoalManhattan(otherAgent.P()) + 3;
        // 行動済みなら+1 移動 搬出
        const int myTurn =
            (agent.id < nextAi ? 1 : 0) + card.GoalManhattan(agent.P()) + 1;
        // 待つ分を加算
        if (agent.P().X() != N - 1) {
          // その場に置くが可能な可能性があるので、minは2
          ret += max(0, min(2, otherTurn - myTurn));
        } else {
          // その場に置けないので、余計な移動もしないといけない
          ret += max(0, min(4, otherTurn - myTurn));
        }
      }
    } else {
      // 荷物の評価: 掴む離すの必要回数
      // 持っている出口未準備の箱: Put+Catch+Put
      ret += 3;
      // 何らかの出口の上に立っている場合は退く必要がある
      if (agent.P().X() == N - 1) {
        // 荷物の評価: ゴールまでのマンハッタン距離
        ret += 2;
      }
    }
  }
  return ret;
}

int EvaluateInState(const int drawCountIdx) {
  return 0;
  // 現在の状態から行うべき退避の回数*2
  const auto& info = FreeCell::stateInfos[drawCountIdx];
  return info.minSpillCount * 2;
}

int EvaluateCompleteCount(const int completeCount) {
  // 将来出現予定のクレーンのX座標
  return (N * N - completeCount) * (N - 1);
}

double EvaluateRowInversion(const int y, const vector<Card>& pos2cards) {
  double count = 0;
  for (int x = 1; x <= 2; x++) {
    for (int x2 = x + 1; x2 <= 3; x2++) {
      const auto& card1 = pos2cards[Pos(x, y)];
      const auto& card2 = pos2cards[Pos(x2, y)];
      if (card1.IsDummy()) continue;
      if (card2.IsDummy()) continue;
      count += max(card2.number - card1.number + 1, 0) *
               (card1.suit == card2.suit ? PARAMS[1] : PARAMS[2]);
    }
  }
  return count;
}

double EvaluateExpectedSpill(const int drawCountIdx) {
  const auto& info = FreeCell::stateInfos[drawCountIdx];
  return info.expectedSpillCount;
}

// 状態のコストを評価するための構造体
// メモリ使用量をできるだけ小さくしてください
struct Evaluator {
  int nextAi = 0;
  int manhattanSum = 0;
  Hash randomHash = 0;
  // 一時的なペナルティ
  double penalty = 0.0;
  int drawCountIdx = 0;
  int completeCount = 0;
  double inversionCount = 0;
  int maxOutTurn = 0;
  int outTaskCount = 0;
  // 小クレーンのy座標の個数を6進数表記したもの
  uint32_t smallCraneIdx = 0;
  Evaluator(const Board& board, int nextAi) : nextAi(nextAi) {
    rep(y, N) {
      completeCount += board.cardBoard.row2NextOutNums[y];
      if (!board.cardBoard.pos2Cards[Pos(N - 1, y)].IsDummy()) {
        completeCount++;
      }
    }
    rep(ai, N) {
      const auto& agent = board.agentBoard.Get(ai);
      manhattanSum +=
          EvaluateAgent(agent, board.cardBoard.row2NextOutNums,
                        board.cardBoard.minX, board.card2Agents, board, nextAi);

      if (!agent.IsBig()) {
        smallCraneIdx += Pow6(agent.P().Y());
      }
    }
    rep(suit, N) {
      rep(number, N) {
        const Card card{suit, number};
        const auto p = board.cardBoard.FindCard(card);
        // 出口またはエージェント
        if (p.IsDummy()) continue;

        // 置かれているまたは入口
        manhattanSum += EvaluatePuttedCard(p, card);
      }
    }

    manhattanSum +=
        EvaluateInState(FreeCell::Index(board.cardBoard.DrawCounts()));

    drawCountIdx = FreeCell::Index(board.cardBoard.DrawCounts());

    rep(y, N) {
      inversionCount += EvaluateRowInversion(y, board.cardBoard.pos2Cards);
    }

    rep(i, N) {
      // 搬出できるか
      if (board.cardBoard.row2NextOutNums[i] <
          FreeCell::stateInfos[drawCountIdx].row2NextOutNums[i]) {
        outTaskCount++;
        continue;
      }
    }

    manhattanSum += EvaluateCompleteCount(completeCount);
  }
  Evaluator(int nextAi, int manhattanSum, Hash randomHash, double penalty,
            int drawCountIdx, int completeCount, double inversionCount,
            int maxOutTurn, int outTaskCount, uint32_t smallCraneIdx)
      : nextAi(nextAi),
        manhattanSum(manhattanSum),
        randomHash(randomHash),
        penalty(penalty),
        drawCountIdx(drawCountIdx),
        completeCount(completeCount),
        inversionCount(inversionCount),
        maxOutTurn(maxOutTurn),
        outTaskCount(outTaskCount),
        smallCraneIdx(smallCraneIdx) {}

  Cost Evaluate() const {
    return max((double)manhattanSum,
               maxOutTurn * 5 - nextAi + manhattanSum * PARAMS[3]) +
           penalty + inversionCount * PARAMS[0];
  }
};

// 展開するノードの候補を表す構造体
struct Candidate {
  Action action;
  Evaluator evaluator;
  Hash hash;
  int parent;
  Cost cost;

  Candidate(Action action, Evaluator evaluator, Hash hash, int parent,
            Cost cost)
      : action(action),
        evaluator(evaluator),
        hash(hash),
        parent(parent),
        cost(cost) {}
};

// ビームサーチの設定
struct Config {
  int max_turn;
  size_t beam_width;
  size_t nodes_capacity;
  uint32_t hash_map_capacity;
};

pair<Cost, int> F(pair<Cost, int> a, pair<Cost, int> b) {
  if (a.first >= b.first) {
    return a;
  } else {
    return b;
  }
}

pair<Cost, int> G() { return make_pair(numeric_limits<Cost>::min(), -1); }

// 削除可能な優先度付きキュー
using MaxSegtree = atcoder::segtree<pair<Cost, int>, F, G>;

// ノードの候補から実際に追加するものを選ぶクラス
// ビーム幅の個数だけ、評価がよいものを選ぶ
// ハッシュ値が一致したものについては、評価がよいほうのみを残す
class Selector {
 public:
  explicit Selector(const Config& config)
      : hash_to_index_(config.hash_map_capacity) {
    beam_width = config.beam_width;
    candidates_.reserve(beam_width * 10);
    full_ = false;
    st_original_.resize(beam_width);
    finished_candidates_.reserve(beam_width * 10);
  }

  // 候補を追加する
  // ターン数最小化型の問題で、candidateによって実行可能解が得られる場合にのみ
  // finished = true とする ビーム幅分の候補をCandidateを追加したときにsegment
  // treeを構築する
  void push(Action action, const Evaluator& evaluator, Hash hash, int parent,
            bool finished) {
    Cost cost = evaluator.Evaluate();
    if (finished) {
      finished_candidates_.emplace_back(
          Candidate(action, evaluator, hash, parent, cost));
      return;
    }
    if (full_ && cost >= st_.all_prod().first) {
      // 保持しているどの候補よりもコストが小さくないとき
      return;
    }
    auto [valid, i] = hash_to_index_.GetIndex(hash);

    if (valid) {
      int j = hash_to_index_.Get(i);
      if (hash == candidates_[j].hash) {
        // ハッシュ値が等しいものが存在しているとき
        if (cost < candidates_[j].cost) {
          // 更新する場合
          candidates_[j] = Candidate(action, evaluator, hash, parent, cost);
          if (full_) {
            st_.set(j, {cost, j});
          }
        }
        return;
      }
    }
    if (full_) {
      // segment treeが構築されている場合
      int j = st_.all_prod().second;
      hash_to_index_.Set(i, hash, j);
      candidates_[j] = Candidate(action, evaluator, hash, parent, cost);
      st_.set(j, {cost, j});
    } else {
      // segment treeが構築されていない場合
      hash_to_index_.Set(i, hash, candidates_.size());
      candidates_.emplace_back(
          Candidate(action, evaluator, hash, parent, cost));

      if (candidates_.size() == beam_width) {
        // 保持している候補がビーム幅分になったとき
        construct_segment_tree();
      }
    }
  }

  // 選んだ候補を返す
  const vector<Candidate>& select() const { return candidates_; }

  // 実行可能解が見つかったか
  bool have_finished() const { return !finished_candidates_.empty(); }

  // 実行可能解に到達する「候補」を返す
  vector<Candidate> get_finished_candidates() const {
    return finished_candidates_;
  }

  void clear() {
    candidates_.clear();
    hash_to_index_.Clear();
    full_ = false;
  }

 private:
  size_t beam_width;
  vector<Candidate> candidates_;
  HashMap<Hash, int> hash_to_index_;
  bool full_;
  vector<pair<Cost, int>> st_original_;
  MaxSegtree st_;
  vector<Candidate> finished_candidates_;

  void construct_segment_tree() {
    full_ = true;
    for (size_t i = 0; i < beam_width; ++i) {
      st_original_[i] = {candidates_[i].cost, i};
    }
    st_ = MaxSegtree(st_original_);
  }
};

// パリティ調整に使うターン
// そのターン中は、全ての状態を区別する
constexpr int kInitTurn = 15;

// 深さ優先探索に沿って更新する情報をまとめたクラス
class State {
 public:
  Board board;
  explicit State(/* const Input& input */) {}

  bool ShouldEdakariBeforeAction(const int ai, const char c) {
    // 行動前に、枝狩りする
    const auto& agent = board.agentBoard.Get(ai);
    if (!agent.card.IsDummy() &&
        board.cardBoard.row2NextOutNums[agent.card.suit] == agent.card.number &&
        agent.card.GoalManhattan(agent.P()) == 0 && c != 'Q')
      return true;

    // 搬入口の荷物を持っているエージェントは右に動けるなら上下に動かない
    if(turn <= 50 && (c == 'U' || c == 'D') && agent.IsCatch() && agent.P().X() == 0 && board.agentBoard.CanMove(ai, kR) && board.cardBoard.CanMoveToWithCard(Pos(1, agent.P().Y()))){
      return true;
    }
    return false;
  }

  bool ShouldEdakariAfterAction(const int ai, const char c,
                                const int nextDrawCount) {
    // 行動後に、枝狩りする
    const auto& stateInfo = FreeCell::stateInfos[nextDrawCount];
    if (stateInfo.minMaxBoardCount >
        max(FreeCell::stateInfos[0].minMaxBoardCount, (int)PARAMS[7])) {
      return true;
    }

    if (c == 'Q') {
      // 最短距離で搬出できる荷物が無い場合は枝狩り
      bool ok = false;
      rep(y, N) {
        const int number = board.cardBoard.row2NextOutNums[y];
        if (number == N) {
          if (board.agentBoard.Get(ai).P().X() == N - 1) {
            ok = true;
            break;
          }
          continue;
        }
        const Card card{y, number};
        auto pos = board.cardBoard.FindCard(card);
        if (pos.IsDummy()) {
          const int ai = board.card2Agents[card];
          if (ai == 0) {
            ok = true;
            break;
          }
          if (ai == -1) {
            continue;
          }
          pos = board.agentBoard.Get(ai).P();
        }

        if (GetGoalDistConsideringX0(board, board.cardBoard.existBS, pos, y) <=
            card.GoalManhattan(pos)) {
          ok = true;
          break;
        }
      }
      if (!ok) {
        return true;
      }
    }

    return false;
  }

  bool ApproveForParity(const int ai, const char c) {
    if (PARAMS[6] == 0.0) return true;
    // 持ってるかどうかでパリティを合わせるために、最初のkInitTurnで調整し、それ以降はパスを禁止とする
    if (turn >= kInitTurn) {
      // パリティ調整済みなので、待機は禁止
      return c != '.';
    } else if (turn >= kInitTurn - 4) {
      // 荷物を持っている場合はパリティがずれていることが期待されるので、
      // 「荷物持ちかどうかがリーダーと一致しているか否か」と「パリティがリーダーと一致しているか否か」が一致するようにする
      const auto& agent = board.agentBoard.Get(ai);
      const auto& agentPos = agent.P();
      const auto& isAgentCatch = agent.IsCatch();
      const auto& leader = board.agentBoard.Get(0);
      const auto& leaderPos = leader.P();
      const bool isLeaderCatch = leader.IsCatch();
      const bool shouldStop = (isLeaderCatch == isAgentCatch) ==
                              (((agentPos.X() + agentPos.Y()) % 2 == 0) ==
                               ((leaderPos.X() + leaderPos.Y()) % 2 == 0));
      return shouldStop == (c == '.');
    } else {
      return true;
    }
  }

  // 次の状態候補を全てselectorに追加する
  // 引数
  //   evaluator : 今の評価器
  //   hash      : 今のハッシュ値
  //   parent    : 今のノードID（次のノードにとって親となる）
  void expand(const Evaluator& evaluator, const Hash hash, int parent,
              Selector& selector) {
    auto baseManhattanSum = evaluator.manhattanSum;
    const int ai = evaluator.nextAi;

#if 0
    rep(i, 5) {
      const auto& aa = board.agentBoard.Get(i);
      cerr << aa.P().Idx() << (aa.IsCatch() ? "+ " : " ");
    }
    cerr << endl;
    cerr << evaluator.manhattanSum << endl << endl;
#endif

    const auto prevPos = board.agentBoard.Get(ai).P();
    for (auto c : vector<char>{'U', 'D', 'L', 'R', '.', 'X'}) {
      if (c == 'X') {
        if (board.agentBoard.Get(ai).IsCatch()) {
          c = 'Q';
        } else {
          c = 'P';
        }
      }

      if (!ApproveForParity(ai, c)) continue;
      if (ShouldEdakariBeforeAction(ai, c)) continue;

      // constexpr bitset<25> puttableCells =
      // 0b11100'10000'11100'10000'11100ull; if (c == 'Q' &&
      // !puttableCells[prevPos]) {
      //   continue;
      // }
      double penalty = 0.0;

      int manhattanSum = baseManhattanSum;
      Hash nextHash = hash ^ evaluator.randomHash;
      int drawCountIdx = evaluator.drawCountIdx;
      int completeCount = evaluator.completeCount;
      int inversionCount = evaluator.inversionCount;
      auto smallCraneIdx = evaluator.smallCraneIdx;

      const bool shouldAllAgentEval = c == 'P' || c == 'Q';
      int shouldOtherAgentId = -1;
      // 変更前のスコア・ハッシュを反映
      {
        // エージェント
        const auto& agent = board.agentBoard.Get(ai);
        manhattanSum -=
            EvaluateAgent(agent, board.cardBoard.row2NextOutNums,
                          board.cardBoard.minX, board.card2Agents, board, ai);

        // Inの状態
        manhattanSum -= EvaluateInState(drawCountIdx);

        // 置かれているコンテナ
        const auto card = board.cardBoard.pos2Cards[prevPos];
        if (!card.IsDummy()) {
          manhattanSum -= EvaluatePuttedCard(prevPos, card);
          nextHash ^= PutPosHashNotInOut(prevPos);
        }
      }

      if (c == 'P' || c == 'Q') {
        // drawCountが更新されるか確認して更新
        const auto& agent = board.agentBoard.Get(ai);
        const int x = agent.P().X();
        const int y = agent.P().Y();
        if (x == 0) {
          auto nextInIdx = board.cardBoard.row2NextInIndexes[y];
          const auto& card = board.cardBoard.pos2Cards[agent.P()];
          // 直前のやつが残っているならまだドロー出来ていない
          if (nextInIdx - 1 >= 0 && card == input_cards[y][nextInIdx - 1]) {
            nextInIdx--;
          }
          nextHash ^= DrawCountHash(drawCountIdx);
          drawCountIdx -= FreeCell::Index(y, nextInIdx);
        }
        inversionCount -= EvaluateRowInversion(y, board.cardBoard.pos2Cards);
      }

      if (c == 'U' || c == 'D' || c == 'L' || c == 'R' || c == '.') {
        const auto& agent = board.agentBoard.Get(ai);
        if (!agent.card.IsDummy()) {
          if (agent.card.number - 1 ==
              board.cardBoard.row2NextOutNums[agent.card.suit]) {
            const auto otherAi =
                board.card2Agents[Card{agent.card.suit, agent.card.number - 1}];
            if (otherAi != -1) {
              shouldOtherAgentId = otherAi;
            }
          } else if (agent.card.number ==
                         board.cardBoard.row2NextOutNums[agent.card.suit] &&
                     agent.card.number + 1 < N) {
            const auto otherAi =
                board.card2Agents[Card{agent.card.suit, agent.card.number + 1}];
            if (otherAi != -1) {
              shouldOtherAgentId = otherAi;
            }
          }
        }
      }

      if (shouldAllAgentEval) {
        rep(i, N) {
          // 既に評価しているので二重評価になってしまうのでcontinue
          if (ai == i) continue;
          const auto& agent = board.agentBoard.Get(i);
          manhattanSum -=
              EvaluateAgent(agent, board.cardBoard.row2NextOutNums,
                            board.cardBoard.minX, board.card2Agents, board, ai);
        }
      } else if (shouldOtherAgentId != -1) {
        const auto& agent = board.agentBoard.Get(shouldOtherAgentId);
        manhattanSum -=
            EvaluateAgent(agent, board.cardBoard.row2NextOutNums,
                          board.cardBoard.minX, board.card2Agents, board, ai);
      }

      if (c == 'U' || c == 'D') {
        const auto& agent = board.agentBoard.Get(ai);
        if (agent.IsBig()) {
          nextHash ^= BigCranePosHash(agent.P());
        } else {
          nextHash ^= SmallCranePosHash(smallCraneIdx);
          smallCraneIdx -= Pow6(agent.P().Y());
        }
      }

      if (!board.TryForward(ai, c)) {
        continue;
      }

      // 置かれているコンテナ
      // nextPosではなくprevPosを参照する
      // ターンスタートで搬入される前に更新
      {
        const auto card = board.cardBoard.pos2Cards[prevPos];
        if (!card.IsDummy()) {
          manhattanSum += EvaluatePuttedCard(prevPos, card);
          nextHash ^= PutPosHashNotInOut(prevPos);
        }
      }

      if (ai == N - 1) {
        board.TurnStart();
      }

      if (c == 'U' || c == 'D') {
        const auto& agent = board.agentBoard.Get(ai);
        if (agent.IsBig()) {
          nextHash ^= BigCranePosHash(agent.P());
        } else {
          smallCraneIdx += Pow6(agent.P().Y());
          nextHash ^= SmallCranePosHash(smallCraneIdx);
        }
      }

      if (c == 'Q' && board.agentBoard.Get(ai).P().X() == N - 1) {
        const int y = board.agentBoard.Get(ai).P().Y();
        nextHash ^= CompleteHash(y, board.cardBoard.row2NextOutNums[y]);
      }

      if (c == 'P' || c == 'Q') {
        // drawCountが更新されるか確認して更新
        const auto& agent = board.agentBoard.Get(ai);
        const int x = agent.P().X();
        const int y = agent.P().Y();
        if (x == 0) {
          auto nextInIdx = board.cardBoard.row2NextInIndexes[y];
          const auto& card = board.cardBoard.pos2Cards[agent.P()];
          // 直前のやつが残っているならまだドロー出来ていない
          if (nextInIdx - 1 >= 0 && card == input_cards[y][nextInIdx - 1]) {
            nextInIdx--;
          }
          drawCountIdx += FreeCell::Index(y, nextInIdx);
          nextHash ^= DrawCountHash(drawCountIdx);
        }

        inversionCount += EvaluateRowInversion(y, board.cardBoard.pos2Cards);
      }

      if (ShouldEdakariAfterAction(ai, c, drawCountIdx)) {
        if (ai == N - 1) {
          board.BackwardTurnStart();
        }
        board.Backward(ai, c);
        continue;
      }

      if (c == 'Q') {
        // completeCountの更新
        const auto& agent = board.agentBoard.Get(ai);
        const auto& p = agent.P();
        if (p.X() == N - 1) {
          manhattanSum -= EvaluateCompleteCount(completeCount);
          completeCount++;
          manhattanSum += EvaluateCompleteCount(completeCount);
        }
      }

      if (c == 'Q' && board.agentBoard.Get(ai).P().X() == N - 1) {
        const int y = board.agentBoard.Get(ai).P().Y();
        nextHash ^= CompleteHash(y, board.cardBoard.row2NextOutNums[y]);
      }

#if 0
      const auto debug = FreeCell::Index(board.cardBoard.DrawCounts());
      CHECK((uint64_t)drawCountIdx, ==, debug);
#endif

      // 変更後のスコア・ハッシュを反映
      {
        const auto& agent = board.agentBoard.Get(ai);

        // エージェント
        manhattanSum += EvaluateAgent(agent, board.cardBoard.row2NextOutNums,
                                      board.cardBoard.minX, board.card2Agents,
                                      board, (ai + 1) % N);

        // Inの状態
        manhattanSum += EvaluateInState(drawCountIdx);

        // 出口準備済みコンテナが変化するので、評価値を更新
        if (shouldAllAgentEval) {
          rep(i, N) {
            // 既に評価しているので二重評価になってしまう
            if (ai == i) continue;
            const auto& agent = board.agentBoard.Get(i);
            manhattanSum += EvaluateAgent(
                agent, board.cardBoard.row2NextOutNums, board.cardBoard.minX,
                board.card2Agents, board, (ai + 1) % N);
          }
        } else if (shouldOtherAgentId != -1) {
          const auto& agent = board.agentBoard.Get(shouldOtherAgentId);
          manhattanSum += EvaluateAgent(agent, board.cardBoard.row2NextOutNums,
                                        board.cardBoard.minX, board.card2Agents,
                                        board, (ai + 1) % N);
        }
      }

      int maxOutTurn = evaluator.maxOutTurn;
      if (turn >= PARAMS[5]) {
        maxOutTurn = board.MaxOutTurn((ai + 1) % N);
      }

      // 意味のあるタスクの個数を反映
      int outTaskCount = evaluator.outTaskCount;
      if ((c == 'P' && board.agentBoard.Get(ai).P().X() == 0) ||
          (c == 'Q' && board.agentBoard.Get(ai).P().X() == N - 1)) {
        outTaskCount = 0;
        rep(i, N) {
          // 既にドロー済みのコンテナのうち、搬出可能なスートの個数をカウント
          if (board.cardBoard.row2NextOutNums[i] <
              FreeCell::stateInfos[drawCountIdx].row2NextOutNums[i]) {
            outTaskCount++;
            continue;
          }
        }
      }

      // 意味のあるタスクの数は、搬出可能なドロー済みコンテナ +
      // 搬入口にある退避が必要なコンテナの個数
      // 割り当てないといけないエージェントの個数は、min(N*N-completeCount, N)
      // それらの差が暇なエージェントの個数
      const int himaCount =
          max(0, min(N * N - completeCount, N) - outTaskCount -
                     FreeCell::stateInfos[drawCountIdx].taskCount);
      penalty += himaCount * PARAMS[4];

      penalty +=
          GetBSPenalty(board.cardBoard.existBS) * turn2BoardEvalCoefs[turn];
      penalty += FreeCell::stateInfos[drawCountIdx].minBoardCountPenalty;
      penalty += EvaluateExpectedSpill(drawCountIdx);

      // 初期化時の多様性確保のために乱数をハッシュに加えている
      const auto randomHash =
          (PARAMS[6] == 1.0 && turn < kInitTurn ? xorshift() : 0);

      const bool finished = ai == N - 1 && manhattanSum == 0;
      CHECK((manhattanSum == 0), ==, (completeCount == N * N));
      selector.push(Action(ai, c),
                    Evaluator((ai + 1) % N, manhattanSum, randomHash, penalty,
                              drawCountIdx, completeCount, inversionCount,
                              maxOutTurn, outTaskCount, smallCraneIdx),
                    nextHash ^ randomHash, parent, finished);

#if 0
      Evaluator debug(board);
      if (debug.manhattanSum != manhattanSum) {
        Evaluator hoge(board);
      }
      CHECK(debug.manhattanSum, ==, manhattanSum);
#endif
      if (ai == N - 1) {
        board.BackwardTurnStart();
      }

      board.Backward(ai, c);
    }
  }

  // actionを実行して次の状態に遷移する
  void move_forward(Action action) {
    auto success = board.TryForward(action.ai, action.c);
    assert(success);
    if (action.ai == N - 1) {
      board.TurnStart();
    }
  }

  // actionを実行する前の状態に遷移する
  // 今の状態は、親からactionを実行して遷移した状態である
  void move_backward(Action action) {
    if (action.ai == N - 1) {
      board.BackwardTurnStart();
    }
    board.Backward(action.ai, action.c);
  }

 private:
  // TODO
};

// 探索木（二重連鎖木）のノード
struct Node {
  Action action;
  Evaluator evaluator;
  Hash hash;
  int parent, child, left, right;

  // 根のコンストラクタ
  Node(Action action, const Evaluator& evaluator, Hash hash)
      : action(action),
        evaluator(evaluator),
        hash(hash),
        parent(-1),
        child(-1),
        left(-1),
        right(-1) {}

  // 通常のコンストラクタ
  Node(const Candidate& candidate, int right)
      : action(candidate.action),
        evaluator(candidate.evaluator),
        hash(candidate.hash),
        parent(candidate.parent),
        child(-1),
        left(-1),
        right(right) {}
};

// 二重連鎖木に対する操作をまとめたクラス
class Tree {
 public:
  explicit Tree(const State& state, size_t nodes_capacity, const Node& root)
      : state_(state) {
    nodes_.Reserve(nodes_capacity);
    root_ = nodes_.Push(root);
  }

  // 状態を更新しながら深さ優先探索を行い、次のノードの候補を全てselectorに追加する
  void dfs(Selector& selector) {
    update_root();

    int v = root_;
    while (true) {
      v = move_to_leaf(v);
      state_.expand(nodes_[v].evaluator, nodes_[v].hash, v, selector);
      v = move_to_ancestor(v);
      if (v == root_) {
        break;
      }
      v = move_to_right(v);
    }
  }

  // 根からノードvまでのパスを取得する
  vector<Action> get_path(int v) {
    // cerr << nodes_.size() << endl;

    vector<Action> path;
    while (nodes_[v].parent != -1) {
      path.push_back(nodes_[v].action);
      v = nodes_[v].parent;
    }
    reverse(path.begin(), path.end());
    return path;
  }

  // 新しいノードを追加する
  int add_leaf(const Candidate& candidate) {
    int parent = candidate.parent;
    int sibling = nodes_[parent].child;
    int v = nodes_.Push(Node(candidate, sibling));

    nodes_[parent].child = v;

    if (sibling != -1) {
      nodes_[sibling].left = v;
    }
    return v;
  }

  // ノードvに子がいなかった場合、vと不要な先祖を削除する
  void remove_if_leaf(int v) {
    if (nodes_[v].child == -1) {
      remove_leaf(v);
    }
  }

  // 最も評価がよいノードを返す
  int get_best_leaf(const vector<int>& last_nodes) {
    assert(!last_nodes.empty());
    int ret = last_nodes[0];
    for (int v : last_nodes) {
      if (nodes_[v].evaluator.Evaluate() < nodes_[ret].evaluator.Evaluate()) {
        ret = v;
      }
    }
    return ret;
  }

  // デバッグ用
  vector<vector<Action>> get_all_pathes(const vector<int>& last_nodes) {
    assert(!last_nodes.empty());
    auto vec = last_nodes;
    sort(all(vec), [&](const auto& l, const auto& r) {
      return nodes_[l].evaluator.Evaluate() < nodes_[r].evaluator.Evaluate();
    });

    vector<vector<Action>> ret;
    for (const auto node : vec) {
      ret.push_back(get_path(node));
    }
    return ret;
  }

  // 最も評価が悪いノードを返す
  int get_worst_leaf(const vector<int>& last_nodes) {
    assert(!last_nodes.empty());
    int ret = last_nodes[0];
    for (int v : last_nodes) {
      if (nodes_[v].evaluator.Evaluate() > nodes_[ret].evaluator.Evaluate()) {
        ret = v;
      }
    }
    return ret;
  }

  State state_;
  ObjectPool<Node> nodes_;
  int root_;

 private:
  // 根から一本道の部分は往復しないようにする
  void update_root() {
    int child = nodes_[root_].child;
    while (child != -1 && nodes_[child].right == -1) {
      root_ = child;
      state_.move_forward(nodes_[child].action);
      child = nodes_[child].child;
    }
  }

  // ノードvの子孫で、最も左にある葉に移動する
  int move_to_leaf(int v) {
    int child = nodes_[v].child;
    while (child != -1) {
      v = child;
      state_.move_forward(nodes_[child].action);
      child = nodes_[child].child;
    }
    return v;
  }

  // ノードvの先祖で、右への分岐があるところまで移動する
  int move_to_ancestor(int v) {
    while (v != root_ && nodes_[v].right == -1) {
      state_.move_backward(nodes_[v].action);
      v = nodes_[v].parent;
    }
    return v;
  }

  // ノードvの右のノードに移動する
  int move_to_right(int v) {
    state_.move_backward(nodes_[v].action);
    v = nodes_[v].right;
    state_.move_forward(nodes_[v].action);
    return v;
  }

  // 不要になった葉を再帰的に削除する
  void remove_leaf(int v) {
    while (true) {
      int left = nodes_[v].left;
      int right = nodes_[v].right;
      if (left == -1) {
        int parent = nodes_[v].parent;

        if (parent == -1) {
          cerr << "ERROR: root is removed" << endl;
          exit(-1);
        }
        nodes_.Erase(v);
        nodes_[parent].child = right;
        if (right != -1) {
          nodes_[right].left = -1;
          return;
        }
        v = parent;
      } else {
        nodes_.Erase(v);
        nodes_[left].right = right;
        if (right != -1) {
          nodes_[right].left = left;
        }
        return;
      }
    }
  }
};

// ビームサーチを行う関数
vector<Action> beam_search(const Config& config, State state, Node root) {
  Tree tree(state, config.nodes_capacity, root);

  cerr << root.evaluator.manhattanSum << endl;

  // 探索中のノード集合
  vector<int> curr_nodes;
  curr_nodes.reserve(config.beam_width * 2);
  // 本来は curr_nodes = {state.root_} とすべきだが, 省略しても問題ない

  // 新しいノードの集合
  vector<int> next_nodes;
  next_nodes.reserve(config.beam_width * 2);

  // 新しいノード候補の集合
  Selector selector(config);

  for (turn = 0; turn < config.max_turn; ++turn) {
    // デバッグのために現在の状態に対応する行動をすべて出力
#if 0
    if (turn == 250) {
      const auto ret = tree.get_all_pathes(curr_nodes);
      for (const auto& actions : ret) {
        vector<string> ans(5);
        for (const auto action : actions) {
          ans[action.ai] += action.c;
        }
        rep(y, N) { cout << ans[y] << endl; }
      }
      exit(0);
    }
#endif
#ifdef LOCAL
    if (turn % 5 == 0 && turn > 0) {
      int best_leaf = tree.get_best_leaf(curr_nodes);
      int worst_leaf = tree.get_worst_leaf(curr_nodes);
      cerr << "turn: " << turn
           << " manhattanSum: " << tree.nodes_[best_leaf].evaluator.manhattanSum
           << " < " << tree.nodes_[worst_leaf].evaluator.manhattanSum << endl;
      // cerr << tree.nodes_[best_leaf].evaluator.inversionCount << endl;
      // cerr << tree.nodes_[best_leaf].evaluator.maxOutTurn << endl;
    }
#endif
    // cerr << "turn: " << turn << endl;
    // Euler Tour で selector に候補を追加する
    tree.dfs(selector);

    if (selector.have_finished()) {
      // ターン数最小化型の問題で実行可能解が見つかったとき
      Candidate candidate = selector.get_finished_candidates()[0];
      vector<Action> ret = tree.get_path(candidate.parent);
      ret.push_back(candidate.action);
      cerr << global_timer.ms() << " [ms]" << endl;
#ifndef LOCAL
      vector<string> ans(5);
      for (const auto action : ret) {
        ans[action.ai] += action.c;
      }
      rep(ai, N) {
        while (ans[ai].size() < ans[0].size()) {
          ans[ai] += '.';
        }
      }

      // 末尾の不要なコマンドを削除する
      while (true) {
        bool existQ = false;
        rep(i, N) {
          if (ans[i].back() == 'Q') {
            existQ = true;
            break;
          }
        }
        if (existQ) break;

        rep(i, N) { ans[i].pop_back(); }
      }

      rep(ai, N) { cout << ans[ai] << endl; }
      cerr << "score: " << ans[0].size() << endl;
      std::quick_exit(0);
#endif
      return ret;
    }
    // 新しいノードを追加する
    for (const Candidate& candidate : selector.select()) {
      next_nodes.push_back(tree.add_leaf(candidate));
    }
    if (next_nodes.empty()) {
      // 新しいノードがないとき
      cerr << "ERROR: Failed to find any valid solution" << endl;
      return {};
    }
    // 不要なノードを再帰的に削除する
    for (int v : curr_nodes) {
      tree.remove_if_leaf(v);
    }
    // ダブルバッファリングで配列を使い回す
    swap(curr_nodes, next_nodes);
    next_nodes.clear();

    selector.clear();
  }
  // ターン数固定型の問題で全ターンが終了したとき
  int best_leaf = tree.get_best_leaf(curr_nodes);
  cerr << tree.nodes_[best_leaf].evaluator.manhattanSum << endl;
  return tree.get_path(best_leaf);
}

}  // namespace Beam





/* start */

class Solver {
 public:
  Solver() {}

  void Solve() {
    {
      FreeCell::Solver solver;
      solver.Solve();
    }
    Beam::Config config;
    config.beam_width = 7000;
    config.hash_map_capacity = (1 << 22);
    config.max_turn = 1000;
    config.nodes_capacity = (1 << 20);

    Beam::State state;

    Beam::Node root(Beam::Action(0, '.'), Beam::Evaluator(state.board, 0),
                    Beam::Hash(0));

    const auto ret = beam_search(config, state, root);

    vector<string> ans(5);
    for (const auto action : ret) {
      ans[action.ai] += action.c;
    }
    rep(ai, N) {
      while (ans[ai].size() < ans[0].size()) {
        ans[ai] += '.';
      }
    }

    // 末尾の不要なコマンドを削除する
    while (true) {
      bool existQ = false;
      rep(i, N) {
        if (ans[i].back() == 'Q') {
          existQ = true;
          break;
        }
      }
      if (existQ) break;

      rep(i, N) { ans[i].pop_back(); }
    }

    rep(ai, N) { cout << ans[ai] << endl; }
    cerr << "score: " << ans[0].size() << endl;
#ifdef LOCAL
    // デバッグのために以下を求める
    // * CatchPutの回数
    // * コンテナをゴールから遠ざけた距離 * 2
    // * コンテナを持たずに左以外に移動して回避した回数 (右なら*2)
    // * コンテナを拾うために上下に移動した回数
    // * 最後何もせず暇したターン
    Board board;
    int catchPutCount = 0;
    int awayGoalCount = 0;
    int avoidCount = 0;
    int forCatchCount = 0;
    vector<Pos> prevPutPoses(5);
    rep(y, 5) { prevPutPoses[y] = Pos(0, y); }
    int turn = -1;
    for (const auto action : ret) {
      turn++;
      if (action.ai == 0) {
        board.TurnStart();
        Beam::Evaluator eval(board, action.ai);
        cerr << turn << " " << eval.manhattanSum << endl;
      }
      if (action.c == 'P' || action.c == 'Q') {
        catchPutCount++;
      }

      const auto prevAgent = board.agentBoard.Get(action.ai);
      const auto prevPos = prevAgent.P();
      board.TryForward(action.ai, action.c);
      const auto nextAgent = board.agentBoard.Get(action.ai);
      const auto nextPos = nextAgent.P();

      if (action.c == 'Q') {
        prevPutPoses[action.ai] = prevPos;
      }

      if (action.c == 'P') {
        if (!prevPutPoses[action.ai].IsDummy()) {
          const int dy = abs(prevPutPoses[action.ai].Y() - prevPos.Y());
          // dy分は回収のために動いた分
          forCatchCount += dy;
          avoidCount -= dy;

          // 回収のために右に動いた分
          const int dx = prevPos.X() - prevPutPoses[action.ai].X();
          if (dx > 0) {
            forCatchCount += dx * 2;
            avoidCount -= dx * 2;
          }
        }
      }

      if (prevPos != nextPos) {
        if (prevAgent.IsCatch()) {
          const auto& card = prevAgent.card;
          if (card.GoalManhattan(prevAgent.P()) <
              card.GoalManhattan(nextAgent.P())) {
            awayGoalCount += 2;
          }
        } else {
          if (action.c == 'U' || action.c == 'D' || action.c == '.' || action.c == 'B') {
            avoidCount += 1;
          } else if (action.c == 'R') {
            avoidCount += 2;
          }
        }
      }
    }

    int neatTurn = 0;
    rep(ai, N) {
      REP(j, ans[ai].size()) {
        if (ans[ai][j] == 'Q') break;
        neatTurn++;
        if (ans[ai][j] == 'U' || ans[ai][j] == 'D' || ans[ai][j] == '.' || ans[ai][j] == 'B') {
          avoidCount -= 1;
        } else if (ans[ai][j] == 'R') {
          avoidCount -= 2;
        }
      }
    }

    cerr << "score: " << ans[0].size() << " "
         << catchPutCount - N * N * 2 -
                FreeCell::stateInfos[0].minSpillCount * 2
         << " " << awayGoalCount << " " << avoidCount << " " << forCatchCount
         << " " << neatTurn << endl;

#endif
    std::quick_exit(0);
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