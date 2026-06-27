

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





#include <cstdint>



/* start */

enum MoveDir {
  kMoveU = 0,
  kMoveR = 1,
  kMoveD = 2,
  kMoveL = 3,
  kMoveNone = 4,
};

vector<MoveDir> kMoveDirs4 = {kMoveU, kMoveR, kMoveD, kMoveL};
vector<MoveDir> kMoveDirs5 = {kMoveU, kMoveR, kMoveD, kMoveL, kMoveNone};
vector<MoveDir> kInvMoveDirs = {kMoveD, kMoveL, kMoveU, kMoveR, kMoveNone};

ostream& operator<<(ostream& os, MoveDir dir) {
  switch (dir) {
    case kMoveU:
      os << "U";
      break;
    case kMoveR:
      os << "R";
      break;
    case kMoveD:
      os << "D";
      break;
    case kMoveL:
      os << "L";
      break;
    case kMoveNone:
      os << ".";
      break;
  }
  return os;
}

enum RotateDir { kRotateU = 0, kRotateR = 1, kRotateD = 2, kRotateL = 3 };
vector<RotateDir> kRotateDirs = {kRotateU, kRotateR, kRotateD, kRotateL};
// Rotateの場合、Uの反対はU
vector<RotateDir> kInvRotateDirs = {kRotateU, kRotateL, kRotateD, kRotateR};

ostream& operator<<(ostream& os, RotateDir dir) {
  switch (dir) {
    case kRotateU:
      os << ".";
      break;
    case kRotateR:
      os << "R";
      break;
    case kRotateL:
      os << "L";
      break;
    case kRotateD:
      os << "D";
      break;
  }
  return os;
}

MoveDir AddDir(MoveDir dir1, RotateDir dir2) {
  return static_cast<MoveDir>((dir1 + dir2) % 4);
}

RotateDir AddDir(RotateDir dir1, RotateDir dir2) {
  return static_cast<RotateDir>((dir1 + dir2) % 4);
}

int AbsSubDir(RotateDir dir1, RotateDir dir2) {
  return min((dir1 - dir2 + 4) % 4, (dir2 - dir1 + 4) % 4);
}

vector<RotateDir> SubDirs(RotateDir prev, RotateDir next) {
  if (prev == next) {
    return {};
  }
  if (prev == kRotateU) {
    if (next == kRotateR) {
      return {kRotateR};
    } else if (next == kRotateD) {
      return {kRotateR, kRotateR};
    } else if (next == kRotateL) {
      return {kRotateL};
    }
  } else if (prev == kRotateR) {
    if (next == kRotateU) {
      return {kRotateL};
    } else if (next == kRotateD) {
      return {kRotateR};
    } else if (next == kRotateL) {
      return {kRotateR, kRotateR};
    }
  } else if (prev == kRotateD) {
    if (next == kRotateU) {
      return {kRotateR, kRotateR};
    } else if (next == kRotateR) {
      return {kRotateL};
    } else if (next == kRotateL) {
      return {kRotateR};
    }
  } else if (prev == kRotateL) {
    if (next == kRotateU) {
      return {kRotateR};
    } else if (next == kRotateR) {
      return {kRotateR, kRotateR};
    } else if (next == kRotateD) {
      return {kRotateL};
    }
  }
  assert(false);
}

string MoveDirToStr(MoveDir dir) {
  switch (dir) {
    case kMoveU:
      return "U";
    case kMoveR:
      return "R";
    case kMoveD:
      return "D";
    case kMoveL:
      return "L";
    case kMoveNone:
      return "None";
  }
  assert(false);
}

string RotateDirToStr(RotateDir dir) {
  switch (dir) {
    case kRotateU:
      return ".";
    case kRotateR:
      return "R";
    case kRotateL:
      return "L";
    default:
      assert(false);
  }
  assert(false);
}

struct Pos {
  int x, y;

  Pos() {}
  Pos(int x, int y) : x(x), y(y) {}

  bool IsDummy() const { return x == INF; }
  static Pos Dummy() { return Pos(INF, INF); }

  static Pos Create(int len, MoveDir dir) {
    switch (dir) {
      case kMoveU:
        return Pos(0, -len);
      case kMoveD:
        return Pos(0, len);
      case kMoveL:
        return Pos(-len, 0);
      case kMoveR:
        return Pos(len, 0);
      case kMoveNone:
        return Pos(0, 0);
    }
    assert(false);
  }

  static void Init(const int _N) {
    N = _N;
    p2Hashes.resize(N * N);
    xorshift();
    xorshift();
    xorshift();
    xorshift();
    xorshift();
    rep(i, N * N) { p2Hashes[i] = xorshift(); }

    vector<Pos> allPoses;
    rep(i, N) {
      rep(j, N) {
        Pos pos(i, j);
        allPoses.push_back(pos);
      }
    }

    nearPoses.resize(N * N, allPoses);
    rep(idx, N*N){
      Pos p(idx % N, idx / N);
      // 近い順(自身も含む)
      sort(all(nearPoses[idx]), [&](const Pos& a, const Pos& b){
        return a.Dist(p) < b.Dist(p);
      });
    }
  }

  vector<MoveDir> PathTo(const Pos& toPos) const {
    vector<MoveDir> ret;
    auto currentPos = *this;
    while (currentPos != toPos) {
      if (currentPos.x < toPos.x) {
        ret.push_back(kMoveR);
        currentPos.x++;
      } else if (currentPos.x > toPos.x) {
        ret.push_back(kMoveL);
        currentPos.x--;
      } else if (currentPos.y < toPos.y) {
        ret.push_back(kMoveD);
        currentPos.y++;
      } else if (currentPos.y > toPos.y) {
        ret.push_back(kMoveU);
        currentPos.y--;
      } else {
        assert(false);
      }
    }
    return ret;
  }

  // 自身から見た時のotherの方向
  RotateDir Dir(const Pos other) const {
    assert((x == other.x) ^ (y == other.y));
    if (x == other.x) {
      if (y < other.y) {
        return kRotateD;
      } else {
        return kRotateU;
      }
    } else {
      if (x < other.x) {
        return kRotateR;
      } else {
        return kRotateL;
      }
    }
  }

  uint32_t Hash() const { return p2Hashes[NormalIdx()]; }

  int NormalIdx() const { return y * N + x; }

  int Dist(const Pos& other) const {
    return abs(x - other.x) + abs(y - other.y);
  }

  bool InBoard() const { return 0 <= x && x < N && 0 <= y && y < N; }

  Pos operator+(const Pos& other) const {
    return Pos(x + other.x, y + other.y);
  }
  Pos operator-(const Pos& other) const {
    return Pos(x - other.x, y - other.y);
  }
  Pos operator+=(const Pos& other) {
    x += other.x;
    y += other.y;
    return *this;
  }
  bool operator==(const Pos& other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Pos& other) const {
    return x != other.x || y != other.y;
  }

  bool operator<(const Pos& other) const {
    return y == other.y ? x < other.x : y < other.y;
  }

  operator int() const { return NormalIdx(); }

  static int N;
  static vector<uint32_t> p2Hashes;
  static vector<vector<Pos>> nearPoses;
};

int Pos::N;
vector<uint32_t> Pos::p2Hashes;
vector<vector<Pos>> Pos::nearPoses;

/* start */

Timer global_timer;

int N, M, V;
vector<bool> input_takoyaki;
vector<bool> input_goal;
vector<Pos> input_all_targets;
Pos centerPos;
double halfN;

bool NeedCatchOrRelease(Pos pos) {
  return input_takoyaki[pos.NormalIdx()] != input_goal[pos.NormalIdx()];
}

enum CatchRelease { kCatch, kRelease, kHold };

ostream& operator<<(ostream& os, CatchRelease cr) {
  switch (cr) {
    case kCatch:
      os << "P";
      break;
    case kRelease:
      os << "P";
      break;
    case kHold:
      os << ".";
      break;
    default:
      assert(false);
  }
  return os;
}

struct AnswerAction {
  MoveDir moveDir;
  vector<RotateDir> rotateDirs;
  vector<bool> catchReleases;

  AnswerAction(MoveDir moveDir, vector<RotateDir> rotateDirs,
               vector<bool> catchReleases)
      : moveDir(moveDir),
        rotateDirs(rotateDirs),
        catchReleases(catchReleases) {}

  friend ostream& operator<<(ostream& os, const AnswerAction& action) {
    os << action.moveDir;
    for (auto dir : action.rotateDirs) {
      os << dir;
    }
    for (auto cr : action.catchReleases) {
      os << (cr ? "P" : ".");
    }
    return os;
  }
};

struct Output {
  static void StaticInit(istream& is) {
    global_timer.start();
    is >> N >> M >> V;
    Pos::Init(N);
    input_takoyaki.resize(N * N);
    input_goal.resize(N * N);
    rep(y, N) {
      string s;
      is >> s;
      rep(x, N) {
        input_takoyaki[Pos(x, y).NormalIdx()] = s[x] == '1';
        assert(s[x] == '0' || s[x] == '1');
      }
    }
    rep(y, N) {
      string s;
      is >> s;
      rep(x, N) {
        input_goal[Pos(x, y).NormalIdx()] = s[x] == '1';
        assert(s[x] == '0' || s[x] == '1');
      }
    }
    centerPos = Pos(N / 2, N / 2);
    rep(y, N) {
      rep(x, N) {
        Pos pos(x, y);
        if (NeedCatchOrRelease(pos)) {
          input_all_targets.push_back(pos);
        }
      }
    }
    halfN = (N-1) / 2.0;
    cerr << N << " " << M << " " << V << endl;
    cerr << "input_all_targets.size() = " << input_all_targets.size() << endl;
  }
  friend ostream& operator<<(ostream& os, const Output& output) { return os; }
};



/* start */

// vector<double> PARAMS = {2.0, 1.0, 0.3, 0.5, 1.0, 1.0, 1.0, 0.666, 1.0, 1.0,
// 0.0};

vector<double> PARAMS = {
    2.32218600571897,   0.71089040670497,   0.30576527572335127,
    0.5516182847221739, 1.2278918402013803, 1.0850882237182267,
    1.4389381122947054, 0.3599224287611271, 0.8577851096693518,
    1.7752639151199088, 1.921885017447648};








/* start */

struct RawBoard {
  vector<bool> takoyakis;
  // goalsはimmutable
  vector<bool> goals;
  // goal以外に置かれたたこ焼きの個数
  int wrongTakoyakiCount = 0;
  // たこ焼きの置かれていないgoalの個数
  int emptyGoalCount = 0;
  vector<double> pos2MakikomareProbs;
  vector<double> pos2Costs;
  double takoyakiGoalPenalty = 0.0;
  vector<int> x2Counts;
  vector<int> y2Counts;
  vector<double> lineCount2Costs;
  double linePenalty = 0;
  vector<double> x2Weights;
  vector<double> y2Weights;
  uint32_t hash = 0;

  RawBoard() {}
  RawBoard(vector<bool> takoyakis, vector<bool> goals)
      : takoyakis(takoyakis),
        goals(goals),
        pos2MakikomareProbs(N * N, 0.0),
        pos2Costs(N * N, 0.0),
        x2Counts(N, 0),
        y2Counts(N, 0),
        x2Weights(N, 0.0),
        y2Weights(N, 0.0) {
    assert(takoyakis.size() == N * N);
    assert(goals.size() == N * N);
    lineCount2Costs.push_back(0.0);
    rep(i, N) {
      const double val = lineCount2Costs.back() + PARAMS[2] * pow(PARAMS[3], i);
      lineCount2Costs.push_back(val);
    }

    rep(i, N) {
      x2Weights[i] = pow(abs(i - halfN), PARAMS[4]) / halfN * PARAMS[5] + PARAMS[6];
      y2Weights[i] = pow(abs(i - halfN), PARAMS[4]) / halfN * PARAMS[5] + PARAMS[6];
    }

    rep(i, N * N) {
      Pos p(i % N, i / N);
      if (takoyakis[i] && !goals[i]) {
        wrongTakoyakiCount++;
        IncPos(p);
      }
      if (!takoyakis[i] && goals[i]) {
        emptyGoalCount++;
        IncPos(p);
      }
    }
  }

  void SetPosCosts(vector<double> posCosts) {
    pos2Costs.assign(N * N, 0.0);
    rep(y, N) {
      rep(x, N) {
        Pos p(x, y);
        pos2Costs[p.NormalIdx()] = posCosts[p.NormalIdx()];
      }
    }

    InitTakoyakiGoalPenalty();
  }

  void SetMakikomareProbs(vector<double> makikomareProbs) {
    pos2MakikomareProbs.assign(N * N, 0.0);
    rep(y, N) {
      rep(x, N) {
        Pos p(x, y);
        // 巻き込まれない確率 = weight
        pos2MakikomareProbs[p.NormalIdx()] =
            1.0 - makikomareProbs[p.NormalIdx()];
      }
    }

    InitTakoyakiGoalPenalty();
  }

  void InitTakoyakiGoalPenalty() {
    takoyakiGoalPenalty = 0.0;
    rep(i, N * N) {
      Pos pos(i % N, i / N);
      if (takoyakis[i] == goals[i]) continue;
      takoyakiGoalPenalty += pos2MakikomareProbs[i] * Pos2Costs(pos);
    }
  }

  bool Complete() const {
    return wrongTakoyakiCount == 0 && emptyGoalCount == 0;
  }

  bool ShouldPut(Pos pos) const {
    return CanPut(pos) && goals[pos.NormalIdx()];
  }

  bool ShouldCatch(Pos pos) const {
    return CanCatch(pos) && !goals[pos.NormalIdx()];
  }

  bool CanPut(Pos pos) const {
    return pos.InBoard() && !takoyakis[pos.NormalIdx()];
  }

  bool CanCatch(Pos pos) const {
    return pos.InBoard() && takoyakis[pos.NormalIdx()];
  }

  double Pos2Costs(Pos pos) const { return pos2Costs[pos.NormalIdx()]; }

  void IncPos(Pos pos) {
    linePenalty -= lineCount2Costs[x2Counts[pos.x]] * x2Weights[pos.x];
    linePenalty -= lineCount2Costs[y2Counts[pos.y]] * y2Weights[pos.y];
    x2Counts[pos.x]++;
    y2Counts[pos.y]++;
    linePenalty += lineCount2Costs[x2Counts[pos.x]] * x2Weights[pos.x];
    linePenalty += lineCount2Costs[y2Counts[pos.y]] * y2Weights[pos.y];
    hash ^= pos.Hash();
  }

  void DecPos(Pos pos) {
    linePenalty -= lineCount2Costs[x2Counts[pos.x]] * x2Weights[pos.x];
    linePenalty -= lineCount2Costs[y2Counts[pos.y]] * y2Weights[pos.y];
    x2Counts[pos.x]--;
    y2Counts[pos.y]--;
    linePenalty += lineCount2Costs[x2Counts[pos.x]] * x2Weights[pos.x];
    linePenalty += lineCount2Costs[y2Counts[pos.y]] * y2Weights[pos.y];
    hash ^= pos.Hash();
  }

  void Put(Pos pos) {
    assert(CanPut(pos));
    takoyakis[pos.NormalIdx()] = true;
    if (goals[pos.NormalIdx()]) {
      emptyGoalCount--;
      takoyakiGoalPenalty -=
          pos2MakikomareProbs[pos.NormalIdx()] * Pos2Costs(pos);
      DecPos(pos);
    } else {
      wrongTakoyakiCount++;
      takoyakiGoalPenalty +=
          pos2MakikomareProbs[pos.NormalIdx()] * Pos2Costs(pos);
      IncPos(pos);
    }
  }

  void Catch(Pos pos) {
    assert(CanCatch(pos));
    takoyakis[pos.NormalIdx()] = false;
    if (goals[pos.NormalIdx()]) {
      emptyGoalCount++;
      takoyakiGoalPenalty +=
          pos2MakikomareProbs[pos.NormalIdx()] * Pos2Costs(pos);
      IncPos(pos);
    } else {
      wrongTakoyakiCount--;
      takoyakiGoalPenalty -=
          pos2MakikomareProbs[pos.NormalIdx()] * Pos2Costs(pos);
      DecPos(pos);
    }
  }

  double Evaluate() const {
    return takoyakiGoalPenalty + 1000.0 + linePenalty * PARAMS[7];
  }
};






#include <cstdint>





/* start */

struct RawRobot {
  // dirs[i] = i番目の頂点の方向
  // dirs[0,1,...,jointCount-1]が関節
  // dirs[jointCount,...,V-1]が指
  vector<MoveDir> moveDirs;
  vector<RotateDir> rotateDirs;
  vector<Pos> poses;
  vector<bool> isHolds;
  vector<int> lens;
  vector<int> parents;
  vector<vector<int>> childrens;
  int jointCount = 0;
  int fingerCount = 0;
  Pos firstPos;
  int turn = 0;
  vector<vector<int>> fingerNopTurns;
  vector<int> allNopTurns;
  vector<int> turn2NopCounts;
  vector<int> turn2FingerNopCounts;
  uint32_t holdHash = 0;
  uint32_t jointHash = 0;
  // turnがnopではない場合に、turn終了時のpos
  vector<pair<int, Pos>> lastNotNopPoses;
  vector<int> wristIdxes;

  RawRobot() {}
  RawRobot(Pos firstPos, vector<int> lens, vector<int> parents)
      : lens(lens), parents(parents), firstPos(firstPos), fingerNopTurns(V) {
    assert(V == (int)lens.size());
    assert(V == (int)parents.size());
    moveDirs.resize(V, kMoveR);
    rotateDirs.resize(V, kRotateU);
    poses.resize(V, firstPos);
    isHolds.resize(V, false);
    turn2NopCounts.push_back(0);
    turn2FingerNopCounts.push_back(0);

    rep(i, V) { fingerNopTurns[i].push_back(-INF); }
    allNopTurns.push_back(-INF);

    childrens.resize(V);
    rep(i, V) {
      if (parents[i] != -1) {
        childrens[parents[i]].push_back(i);
      }
    }

    rep(i, V) {
      if (childrens[i].size() == 0) {
        fingerCount++;
      } else {
        jointCount++;
      }
    }

    rep(i, V) { jointHash ^= CalcJointHash(i); }

    wristIdxes.push_back(jointCount - 1);
    rep(i, V){
      if(!IsFinger(i)){
        continue;
      }
      const auto parent = parents[i];
      if(parent != wristIdxes.back()){
        assert(parent == wristIdxes.back() - 1);
        wristIdxes.push_back(i);
        swap(wristIdxes.front(), wristIdxes.back());
        break;
      }
    }

    UpdateAll();
  }

  uint32_t CalcJointHash(int idx) const {
    if (idx == 0 || IsFinger(idx)) return 0;
    return static_cast<uint32_t>(rotateDirs[idx]) << (idx * 2);
  }

  int Size() const { return lens.size(); }

  int Parent(int idx) const { return parents[idx]; }

  int Len(int idx) const { return lens[idx]; }

  Pos FirstPos() const { return firstPos; }

  MoveDir GetMoveDir(int idx) const { return AddDir(moveDirs[Parent(idx)], rotateDirs[idx]); }

  const Pos GetPos(int idx) const { return poses[idx]; }

  void UpdateAll() {
    reps(i, V - 1) { UpdatePartial(i); }
  }

  void UpdatePartial(int i) {
    const int parent = parents[i];
    moveDirs[i] = AddDir(moveDirs[parent], rotateDirs[i]);
    const auto& parentPos = poses[parents[i]];
    poses[i] = parentPos + Pos::Create(lens[i], moveDirs[i]);
  }

  bool CanMove(MoveDir dir) const {
    const auto movedPos = poses[0] + Pos::Create(1, dir);
    return movedPos.InBoard();
  }

  void ForwardMove(MoveDir dir) {
    poses[0] = poses[0] + Pos::Create(1, dir);
  }

  void BackwardMove(MoveDir dir) {
    poses[0] = poses[0] + Pos::Create(1, kInvMoveDirs[dir]);
  }

  bool CanChangeRotateDir(int idx, RotateDir dir) const {
    const int dist = AbsSubDir(rotateDirs[idx], dir);
    if (dist <= 1) return true;
    if (allNopTurns.back() == turn - 1) return true;
    if (IsFinger(idx) && fingerNopTurns[idx].back() == turn - 1) return true;
    return false;
  }

  void ChangeRotateDir(int idx, RotateDir dir) {
    assert(idx >= 0);
    assert(idx < V);
    jointHash ^= CalcJointHash(idx);
    rotateDirs[idx] = dir;
    // idxが若い順のDFSで使用されることを想定している
    UpdatePartial(idx);
    jointHash ^= CalcJointHash(idx);
  }

  bool IsFinger(int idx) const { return idx >= jointCount; }

  bool CanPut(int idx) const { return IsFinger(idx) && isHolds[idx]; }

  bool CanCatch(int idx) const { return IsFinger(idx) && !isHolds[idx]; }

  void Put(int idx) {
    assert(isHolds[idx]);
    isHolds[idx] = false;
    holdHash ^= (1 << (idx - jointCount));
  }

  void Catch(int idx) {
    assert(!isHolds[idx]);
    isHolds[idx] = true;
    holdHash ^= (1 << (idx - jointCount));
  }

  void ForwardNop(int idx) {
    assert(idx >= 1);
    turn2NopCounts.back()++;
    if (IsFinger(idx)) {
      fingerNopTurns[idx].push_back(turn);
      turn2FingerNopCounts.back()++;
    }
  }

  void BackwardNop(int idx) {
    assert(idx >= 1);
    assert(turn2NopCounts.back() > 0);
    if (IsFinger(idx)) {
      assert(fingerNopTurns[idx].back() == turn);
      fingerNopTurns[idx].pop_back();
      turn2FingerNopCounts.back()--;
    }
    turn2NopCounts.back()--;
  }

  void ForwardTurnEnd() {
    if (turn2NopCounts.back() == V - 1) {
      allNopTurns.push_back(turn);
    }else{
      lastNotNopPoses.push_back({turn, poses[0]});
    }
    turn++;
    turn2NopCounts.push_back(0);
    turn2FingerNopCounts.push_back(0);
  }

  void BackwardTurnEnd(const bool updateAll) {
    turn2NopCounts.pop_back();
    turn2FingerNopCounts.pop_back();
    turn--;
    if (allNopTurns.back() == turn) {
      allNopTurns.pop_back();
    }else{
      if(!lastNotNopPoses.empty() && lastNotNopPoses.back().first == turn){
        lastNotNopPoses.pop_back();
      }
    }
    // ビームサーチ木上でターンをまたいで遡る場合は、UpdateAllを呼ばないと辻褄が合わなくなる
    if (updateAll) UpdateAll();
  }

  // 1ターンの途中の評価の場合はafterEnd=falseとする
  double Evaluate(const bool afterEnd) const {
    // fingerがNopなのにjointが動いている場合はペナルティINF
    if (turn >= 1 && turn2NopCounts[turn - 1] != V - 1 &&
        turn2FingerNopCounts[turn - 1] == fingerCount) {
      return INF;
    }
    // Nopをする場合は、最後に行動した位置から遠ざかる方向に歩き続けないといけない
    // NOTE: jointの都合で1ターンその場待機したい場合も考えられるが、
    // そのようなケースは「Nopで1歩移動 → 戻ってアクション」として表現できるので問題ない
    if(afterEnd && turn >= 1 && allNopTurns.back() == turn - 1 && !lastNotNopPoses.empty()){
      const auto& [lastTurn, lastPos] = lastNotNopPoses.back();
      assert(lastTurn < turn - 1);
      const int dist = poses[0].Dist(lastPos);
      assert(dist + 1 <= turn - lastTurn);
      if(dist + 1 != turn - lastTurn){
        return INF;
      }
    }
    const double nopBonus = allNopTurns.back() == turn - 1 ? -1e-5 : 0.0;
    return nopBonus;
  }
};


/* start */

struct RawState {
  struct Action {
    int idx;
    MoveDir moveDir;
    RotateDir rotateDir;
    bool doesFingerAction;

    Action(MoveDir moveDir)
        : idx(0),
          moveDir(moveDir),
          rotateDir(kRotateU),
          doesFingerAction(false) {}
    Action(int idx, RotateDir rotateDir, bool doesFingerAction)
        : idx(idx),
          moveDir(kMoveNone),
          rotateDir(rotateDir),
          doesFingerAction(doesFingerAction) {}

    bool IsNop() const { return rotateDir == kRotateU && !doesFingerAction; }
  };

  RawRobot robot;
  RawBoard board;

  RawState() {}

  RawState(RawRobot robot, RawBoard board) : robot(robot), board(board) {
    {
      vector<double> posCosts;
      rep(i, N * N) {
        const int dist = centerPos.Dist(Pos(i % N, i / N));
        // posCosts.push_back((double)max(1.0, pow(minDists[i], 1.0)));
        posCosts.push_back(pow((double)dist / N, PARAMS[0]) + PARAMS[1]);
      }
      this->board.SetPosCosts(posCosts);
    }

    InitMakikomare(10);
  }

  void InitMakikomare(const int kIterCount) {
    // 以下を貪欲に繰り返すとしたときに、各地点に対してアクションを行うことになる確率を求める。
    // 1. たこ焼きとゴールが不一致の地点Xをランダムに一つ選ぶ。
    // 2.
    // その地点に指が届くような関節を、exp(その関節における最大アクション数)に比例する確率でランダムに選ぶ。
    // 3. 他の各指について、届く不一致地点の中から一様ランダムに選ぶ。
    // ↑ は
    // 「Xに対してアクションを行った」と呼び、他の地点については「巻き込まれた」と呼ぶ。

    // 各地点について、その地点に対するアクションを行うことになる確率を求める。
    // まず、各地点に対応する順列をランダムに生成し、その順列の先頭から、もし残っていればアクションを行うとする。
    // その場合、ある地点Yに対するアクションを行うことになる確率は、
    // その地点Yを巻き込みうる && 地点Yより順列上で先に来る任意の地点において、
    // Yを巻き込まない確率を掛け合わせたものである。
    // ※
    // 簡単のため、「Yを巻き込みうる地点」が「他の地点に巻き込まれる」可能性は無視している。
    // 追記: Vが大きいときに無視できなかったので複数iter回すことで考慮している

    // 「地点Xに対してアクションを行った場合」に「地点Xが地点Yを巻き込む確率」を求めたい。
    // これは、任意の手首の位置に対して、よしなにやれば求まる

    // basePosの確率の総和 後で正規化に使用
    vector<double> basePos2ProbSum(N * N, 0.0);
    // basePosがtargetPosを巻き込む確率
    vector<vector<double>> basePos2TargetPos2Prob(N * N,
                                                  vector<double>(N * N, 0.0));

    // 1つ目の手首のindex
    int firstWristIdx = robot.jointCount - 1;
    int secondWristIdx = -1;
    rep(i, V) {
      if (!robot.IsFinger(i)) continue;
      const auto parent = robot.Parent(i);
      if (parent != firstWristIdx) {
        secondWristIdx = firstWristIdx;
        firstWristIdx = parent;
        break;
      }
    }
    rep(y, N) {
      rep(x, N) {
        // 手首の位置
        // TODO: ボードの外も考慮するべきか？
        Pos firstWrist(x, y);
        vector<Pos> secondWrists;
        if (secondWristIdx == -1) {
          secondWrists.push_back(firstWrist);
        } else {
          const int secondLen = robot.Len(secondWristIdx);
          for (const auto moveDir : kMoveDirs4) {
            const Pos secondWrist =
                firstWrist + Pos::Create(secondLen, moveDir);
            // second手首はボードの外もヨシとしている
            // if(!secondWrist.InBoard()) continue;
            secondWrists.push_back(secondWrist);
          }
        }

        for (const auto secondWrist : secondWrists) {
          // これらの手首の位置において、各指のtargetを列挙する
          vector<vector<Pos>> finger2Targets(robot.fingerCount);
          int totalTargetCount = 0;
          for (int idx = robot.jointCount; idx < V; idx++) {
            const int len = robot.Len(idx);
            int targetCount = 0;
            for (const auto moveDir : kMoveDirs4) {
              const Pos target =
                  (robot.Parent(idx) == firstWristIdx ? firstWrist
                                                      : secondWrist) +
                  Pos::Create(len, moveDir);
              if (!target.InBoard()) continue;
              if (board.takoyakis[target.NormalIdx()] !=
                  board.goals[target.NormalIdx()]) {
                targetCount++;
                finger2Targets[idx - robot.jointCount].push_back(target);
              }
            }

            if (targetCount == 0) continue;
            totalTargetCount++;
          }

          if (totalTargetCount == 0) continue;

          // この手首が選ばれる確率(に比例する値)
          // if(totalTargetCount == 1) continue;
          const double wristProb = pow(totalTargetCount, PARAMS[8]) * PARAMS[9] + PARAMS[10];
          // 地点Xに対してアクションを行った時に地点Yが巻き込まれる確率 = Σ_手首
          // 手首が選ばれる確率 * 手首が地点Yを巻き込む確率
          rep(fingerX, finger2Targets.size()) {
            for (const auto targetX : finger2Targets[fingerX]) {
              // TODO: targetYが1つも存在しない場合は除外したほうが良いかも
              basePos2ProbSum[targetX.NormalIdx()] += wristProb;
              // 巻き込まれない確率(2つの指から参照される場合があるので、巻き込まれる確率
              // = 1 - ∏巻き込まれない確率 をする必要がある)
              map<Pos, double> targetY2NoProb;
              rep(fingerY, finger2Targets.size()) {
                if (fingerX == fingerY) continue;
                for (const auto targetY : finger2Targets[fingerY]) {
                  if (targetX == targetY) continue;
                  if (targetY2NoProb.count(targetY) == 0) {
                    targetY2NoProb[targetY] = 1.0;
                  }
                  targetY2NoProb[targetY] *=
                      1.0 - (1.0 / finger2Targets[fingerY].size());
                }
              }

              for (const auto [targetY, noProb] : targetY2NoProb) {
                basePos2TargetPos2Prob[targetX.NormalIdx()]
                                      [targetY.NormalIdx()] +=
                    wristProb * (1.0 - noProb);
              }
            }
          }
        }
      }
    }

    // 各地点が巻き込まれる確率を求める
    vector<double> pos2MakikomareProbs(N * N, 0.0);
    rep(iter, kIterCount) {
      vector<double> pos2MakikomareProbsNext(N * N, 1.0);
      rep(y, N) {
        rep(x, N) {
          const Pos targetPos(x, y);
          if (board.takoyakis[targetPos.NormalIdx()] ==
              board.goals[targetPos.NormalIdx()])
            continue;

          // 自身を巻き込みうる地点について、巻き込まれない確率を列挙する
          vector<double> noProbs;
          rep(yyy, N) {
            rep(xxx, N) {
              const Pos basePos(xxx, yyy);
              if (basePos == targetPos) continue;
              const auto rawProb =
                  basePos2TargetPos2Prob[basePos.NormalIdx()]
                                        [targetPos.NormalIdx()];
              if (rawProb == 0.0) continue;

              assert(basePos2ProbSum[basePos.NormalIdx()] > 0.0);
              // 巻き込まれる確率 =
              // basePosに対してアクションを行った時に自身が巻き込まれる確率 *
              // (1.0 - basePosが巻き込まれる確率)
              const auto prob =
                  rawProb / basePos2ProbSum[basePos.NormalIdx()] *
                  (1.0 - pos2MakikomareProbs[basePos.NormalIdx()]);
              assert(prob <= 1.0);
              noProbs.push_back(1.0 - prob);
            }
          }

          // 巻き込まれない確率 = (1 + p1x)(1 + p2x)...(1 + p{K-1}x) の
          // 多項式を計算し、
          // x^0の項は1/K * 0!, x^1の項は1/K * 1/(K-1) * 1!,
          // x^2の項は1/K * 1/(K-1) * 1/(K-2) * 2!
          // x^aの項は1/K * 1/(K-1) * ... * 1/(K-a) に置き換えて和を取る
          const int K = noProbs.size() + 1;
          vector<double> poly(K, 0.0);
          poly[0] = 1.0;
          rep(i, noProbs.size()) {
            vector<double> nextPoly(K, 0.0);
            rep(j, i + 1) {
              nextPoly[j] += poly[j];
              nextPoly[j + 1] += poly[j] * noProbs[i];
            }
            swap(poly, nextPoly);
            // if (poly.size() >= 2) cerr << poly[0] / K << " " << poly[1] /
            // (K*K) << endl;
          }

          // probSum = 巻き込まれない確率
          double noProbSum = 0.0;
          double keisu = 1.0;
          rep(i, poly.size()) {
            keisu *= 1.0 / (K - i) * (i == 0 ? 1 : i);
            noProbSum += poly[i] * keisu;
          }
          assert(noProbSum <= 1.0);
          assert(noProbSum >= 0.0);
          // cerr << x << " " << y << " " << noProbSum << endl;
          pos2MakikomareProbsNext[targetPos.NormalIdx()] = 1.0 - noProbSum;
        }
      }
      swap(pos2MakikomareProbs, pos2MakikomareProbsNext);
      // cerr << endl << endl;
    }

    this->board.SetMakikomareProbs(pos2MakikomareProbs);
  }

  void ForwardAction(Action action) {
    if (action.idx == 0) {
      ForwardMove(action.moveDir);
    } else {
      Rotate(action.idx, action.rotateDir);
    }
    if (action.doesFingerAction) {
      DoFingerAction(action.idx);
    }
    if (action.IsNop()) {
      ForwardNop(action.idx);
    }
  }

  void BackwardAction(Action action) {
    if (action.IsNop()) {
      BackwardNop(action.idx);
    }
    if (action.doesFingerAction) {
      DoFingerAction(action.idx);
    }
    if (action.idx == 0) {
      BackwardMove(action.moveDir);
    } else {
      Rotate(action.idx, kInvRotateDirs[action.rotateDir]);
    }
  }

  bool CanMove(MoveDir dir) const { return robot.CanMove(dir); }

  void ForwardMove(MoveDir dir) { robot.ForwardMove(dir); }

  void BackwardMove(MoveDir dir) { robot.BackwardMove(dir); }

  void ChangeRotateDir(int idx, RotateDir dir) {
    robot.ChangeRotateDir(idx, dir);
  }

  void Rotate(int idx, RotateDir dir) {
    const auto oldRotateDir = robot.rotateDirs[idx];
    const auto newRotateDir = AddDir(oldRotateDir, dir);
    robot.ChangeRotateDir(idx, newRotateDir);
  }

  bool CanRotate(int idx, RotateDir dir) const {
    const auto oldRotateDir = robot.rotateDirs[idx];
    const auto newRotateDir = AddDir(oldRotateDir, dir);
    return robot.CanChangeRotateDir(idx, newRotateDir);
  }

  bool ShouldFingerAction(int idx) const {
    const auto pos = robot.GetPos(idx);
    return (robot.CanPut(idx) && board.ShouldPut(pos)) ||
           (robot.CanCatch(idx) && board.ShouldCatch(pos));
  }

  void DoFingerAction(int idx) {
    const auto pos = robot.GetPos(idx);
    if (robot.CanPut(idx)) {
      robot.Put(idx);
      board.Put(pos);
    } else if (robot.CanCatch(idx)) {
      robot.Catch(idx);
      board.Catch(pos);
    } else {
      assert(false);
    }
  }

  void ForwardNop(int idx) {
    if (idx == 0) return;
    robot.ForwardNop(idx);
  }

  void BackwardNop(int idx) {
    if (idx == 0) return;
    robot.BackwardNop(idx);
  }

  void ForwardTurnEnd() { robot.ForwardTurnEnd(); }

  void BackwardTurnEnd(const bool updateAll) {
    robot.BackwardTurnEnd(updateAll);
  }

  double Evaluate(const bool afterEnd) const {
    return board.Evaluate() + robot.Evaluate(afterEnd);
  }
};

void Print(ostream& os, const RawRobot& robot,
           const vector<vector<RawState::Action>>& rawActions) {
  os << robot.Size() << endl;
  rep(i, robot.Size()) {
    if (i == 0) continue;
    os << robot.Parent(i) << " " << robot.Len(i) << endl;
  }
  os << robot.FirstPos().y << " " << robot.FirstPos().x << endl;

  // kRotateDを2ターンに分割する
  auto allActions = rawActions;
  rep(i, allActions.size()) {
    if (i == 0) continue;
    rep(j, allActions[i].size()) {
      if (j == 0) continue;
      if (allActions[i][j].rotateDir == kRotateD) {
        assert(allActions[i - 1][j].rotateDir == kRotateU);
        allActions[i][j].rotateDir = kRotateL;
        allActions[i - 1][j].rotateDir = kRotateL;
      }
    }
  }

  for (const auto& actions : allActions) {
    os << actions[0].moveDir;
    rep(i, robot.Size()) {
      if (i == 0) continue;
      os << actions[i].rotateDir;
    }
    rep(i, robot.Size()) {
      if (actions[i].doesFingerAction) {
        os << "P";
      } else {
        os << ".";
      }
    }
    os << endl;
  }
}


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
実装手順
1. Stateのコンストラクタ: 初期状態を定義
2. Action: 行動を表すデータstructを定義
3. State.move_forward, move_backward: Actionを適用したときのStateの変化を実装
4. Evaluator: 盤面の評価値を定義
5: State.expand: 行動を適用した後の評価値やハッシュ値などを計算
6: 外側: configを適切に設定し、適当な初期Nodeを用いて、beam_search()を呼び出す
*/

namespace Beam2 {
using Hash = uint32_t;
using Cost = double;

// 状態遷移を行うために必要な情報
// メモリ使用量をできるだけ小さくしてください

// 状態のコストを評価するための構造体
// メモリ使用量をできるだけ小さくしてください
// TODO
struct Evaluator {
  double cost = INF;
  Evaluator() {}
  Evaluator(double cost) : cost(cost) {}
  Cost Evaluate() const { return cost; }
};

struct Action {
  vector<RawState::Action> actions;
  // 木上のビームサーチ上で複数の初期位置を実現するために、
  // 最初の行動でのみ使用する変数
  Pos startPos = Pos::Dummy();
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

// 削除可能な優先度付きキュー
pair<Cost, int> max_comparator(pair<Cost, int> a, pair<Cost, int> b) {
  if (a.first >= b.first) {
    return a;
  } else {
    return b;
  }
}

pair<Cost, int> min_pair() {
  return make_pair(numeric_limits<Cost>::min(), -1);
}

using MaxSegtree = atcoder::segtree<pair<Cost, int>, max_comparator, min_pair>;

// ノードの候補から実際に追加するものを選ぶクラス
// ビーム幅の個数だけ、評価がよいものを選ぶ
// ハッシュ値が一致したものについては、評価がよいほうのみを残す
class Selector {
 public:
  explicit Selector(const Config& config)
      : hash_to_index_(config.hash_map_capacity) {
    beam_width = config.beam_width;
    candidates_.reserve(beam_width);
    full_ = false;
    st_original_.resize(beam_width);
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

// 深さ優先探索に沿って更新する情報をまとめたクラス
class State {
 public:
  RawState state;
  State(RawState state) : state(state) {}

  uint32_t GetHash() const {
    // 前のターンFullNoopだった場合はjointハッシュを1とする
    // cerr << (state.robot.allNopTurns.back() == state.robot.turn - 1 ? 1 : 0)
    // << endl;
    return ((((state.robot.poses[0].NormalIdx() << state.robot.fingerCount) ^
              state.robot.holdHash)
             << (state.robot.jointCount * 2)) ^
            (state.robot.allNopTurns.back() == state.robot.turn - 1
                 ? 1
                 : state.robot.jointHash)) ^
           state.board.hash;
  }

  void saiki(int currentOrder, Action& action, const Evaluator& evaluator,
             Hash hash, int parent, Selector& selector,
             const vector<int>& indexOrders) {
    if (currentOrder == V) {
      state.ForwardTurnEnd();
      const auto score = state.Evaluate(true);
      selector.push(action, Evaluator(score), GetHash(), parent,
                    state.board.Complete());
      state.BackwardTurnEnd(false);
      return;
    }
    const int currentIdx = indexOrders[currentOrder];

    // 根のアクションは移動
    if (currentIdx == 0) {
      vector<int> randomIndexes = {0, 1, 2, 3, 4};
      rep(i, 4) { swap(randomIndexes[i], randomIndexes[rand() % 5]); }
      for (const int mi : randomIndexes) {
        const auto moveDir = kMoveDirs5[mi];
        if (!state.CanMove(moveDir)) continue;
        state.ForwardMove(moveDir);
        action.actions[currentIdx] = RawState::Action(moveDir);
        saiki(currentOrder + 1, action, evaluator, hash, parent, selector,
              indexOrders);
        state.BackwardMove(moveDir);
      }
      return;
    }

    if (state.robot.IsFinger(currentIdx)) {
      // 手首が盤面外に出る場合は枝狩り
      // if(!state.robot.GetPos(state.robot.Parent(currentIdx)).InBoard())
      // return;
      if (state.board.emptyGoalCount + state.board.wrongTakoyakiCount <= 30) {
        // 最終盤は全探索
        bool existFingerAction = false;
        for (const auto rotateDir : {kRotateL, kRotateR, kRotateD, kRotateU}) {
          if (!state.CanRotate(currentIdx, rotateDir)) continue;
          state.Rotate(currentIdx, rotateDir);
          const bool shouldFingerAction = state.ShouldFingerAction(currentIdx);
          state.Rotate(currentIdx, kInvRotateDirs[rotateDir]);

          RawState::Action tryAction(currentIdx, rotateDir, shouldFingerAction);
          if (!shouldFingerAction) {
            // 指がアクションをしない場合は、Nopのみを許容とする
            if (rotateDir == kRotateU && !existFingerAction) {
              state.ForwardAction(tryAction);
              action.actions[tryAction.idx] = tryAction;
              saiki(currentOrder + 1, action, evaluator, hash, parent, selector,
                    indexOrders);
              state.BackwardAction(tryAction);
            }
          } else {
            existFingerAction = true;
            state.ForwardAction(tryAction);
            action.actions[tryAction.idx] = tryAction;
            saiki(currentOrder + 1, action, evaluator, hash, parent, selector,
                  indexOrders);
            state.BackwardAction(tryAction);
          }
        }
      } else {
        // 指のアクションは、最も評価値が良いものを選ぶ(高速化のため)
        // ダミーで初期化
        RawState::Action bestAction(kMoveD);
        double minScore = INF;
        const auto wrist = state.robot.GetPos(state.robot.Parent(currentIdx));
        if (wrist.x < -2 * N || wrist.x > 2 * N || wrist.y < -2 * N ||
            wrist.y > 2 * N)
          return;
        const auto curDir = state.robot.GetMoveDir(currentIdx);
        const auto isNop = state.robot.fingerNopTurns[currentIdx].back() ==
                           state.robot.turn - 1;
        const auto isHold = state.robot.isHolds[currentIdx];
        const auto& rotateDirCandidates =
            finger2ShouldRotateDirs[currentIdx]
                                   [ToIdx(wrist, curDir, isNop, isHold)];
        for (const auto rotateDir : rotateDirCandidates) {
          if (!state.CanRotate(currentIdx, rotateDir)) continue;
          state.Rotate(currentIdx, rotateDir);
          const bool shouldFingerAction = state.ShouldFingerAction(currentIdx);
          if (shouldFingerAction) {
            state.DoFingerAction(currentIdx);
          }
          RawState::Action action(currentIdx, rotateDir, shouldFingerAction);
          if (!shouldFingerAction) {
            // 指がアクションをしない場合は、Nopのみを許容とする
            if (rotateDir == kRotateU) {
              state.ForwardNop(currentIdx);
              const auto score = state.Evaluate(false);
              if (score < minScore) {
                minScore = score;
                bestAction = action;
              }
              state.BackwardNop(currentIdx);
            }
          } else {
            const auto score = state.Evaluate(false);
            if (score < minScore) {
              minScore = score;
              bestAction = action;
            }
          }
          if (shouldFingerAction) {
            state.DoFingerAction(currentIdx);
          }
          state.Rotate(currentIdx, kInvRotateDirs[rotateDir]);
        }

        if (minScore == INF) {
          // Nop
          const auto rotateDir = kRotateU;
          state.Rotate(currentIdx, rotateDir);
          const bool shouldFingerAction = state.ShouldFingerAction(currentIdx);
          if (shouldFingerAction) {
            state.DoFingerAction(currentIdx);
          }
          RawState::Action action(currentIdx, rotateDir, shouldFingerAction);
          if (!shouldFingerAction) {
            // 指がアクションをしない場合は、Nopのみを許容とする
            if (rotateDir == kRotateU) {
              state.ForwardNop(currentIdx);
              const auto score = state.Evaluate(false);
              if (score < minScore) {
                minScore = score;
                bestAction = action;
              }
              state.BackwardNop(currentIdx);
            }
          } else {
            const auto score = state.Evaluate(false);
            if (score < minScore) {
              minScore = score;
              bestAction = action;
            }
          }
          if (shouldFingerAction) {
            state.DoFingerAction(currentIdx);
          }
          state.Rotate(currentIdx, kInvRotateDirs[rotateDir]);
        }

        // 最良のアクションを適用
        state.ForwardAction(bestAction);
        action.actions[bestAction.idx] = bestAction;
        saiki(currentOrder + 1, action, evaluator, hash, parent, selector,
              indexOrders);
        state.BackwardAction(bestAction);
      }
    } else {
      // 関節のアクションは全探索
      for (const auto rotateDir : {kRotateL, kRotateR, kRotateD, kRotateU}) {
        if (!state.CanRotate(currentIdx, rotateDir)) continue;
        state.Rotate(currentIdx, rotateDir);
        action.actions[currentIdx] =
            RawState::Action(currentIdx, rotateDir, false);
        if (rotateDir == kRotateU) {
          state.ForwardNop(currentIdx);
        }
        saiki(currentOrder + 1, action, evaluator, hash, parent, selector,
              indexOrders);
        if (rotateDir == kRotateU) {
          state.BackwardNop(currentIdx);
        }
        state.Rotate(currentIdx, kInvRotateDirs[rotateDir]);
      }
    }
  }

  int ToIdx(const Pos& wrist, const MoveDir curDir, const bool isNop,
            const bool isHold) const {
    return ((wrist.y + 2 * N) * (4 * N + 1) + wrist.x + 2 * N) * 16 +
           curDir * 4 + (isNop ? 2 : 0) + (isHold ? 1 : 0);
  }

  // 次の状態候補を全てselectorに追加する
  // 引数
  //   evaluator : 今の評価器
  //   hash      : 今のハッシュ値
  //   parent    : 今のノードID（次のノードにとって親となる）
  void expand(const Evaluator& evaluator, Hash hash, int parent,
              Selector& selector) {
    // 初期位置を複数試す
    if (parent == 0) {
      rep(y, N) {
        rep(x, N) {
          Action action;
          action.startPos = Pos(x, y);
          state.robot.poses[0] = Pos(x, y);
          selector.push(action, evaluator, GetHash(), parent, false);
        }
      }
      return;
    }
    if (finger2ShouldRotateDirs.empty()) {
      // (手首の位置, 現在の向き, nopをしたか,
      // 保持しているか)を受けとって、試すべき方向を返す
      finger2ShouldRotateDirs.resize(V);
      rep(fingerIdx, V) {
        if (!state.robot.IsFinger(fingerIdx)) continue;
        auto& shouldRotateDirs = finger2ShouldRotateDirs[fingerIdx];
        shouldRotateDirs.resize((4 * N + 1) * (4 * N + 1) * 16);
        const int len = state.robot.Len(fingerIdx);
        for (int y = -2 * N; y <= 2 * N; y++) {
          for (int x = -2 * N; x <= 2 * N; x++) {
            // 手首の位置
            Pos wrist(x, y);
            for (const auto curDir : kMoveDirs4) {
              // 現在の向き
              for (const bool isNop : {true, false}) {
                // nopしたか
                for (const bool isHold : {true, false}) {
                  const int idx = ToIdx(wrist, curDir, isNop, isHold);
                  vector<RotateDir> rotateDirs;
                  for (const auto rotateDir :
                       {kRotateL, kRotateR, kRotateD, kRotateU}) {
                    if (rotateDir == kRotateD && !isNop) continue;
                    const auto nextDir = AddDir(curDir, rotateDir);
                    const auto targetPos = wrist + Pos::Create(len, nextDir);
                    if (!targetPos.InBoard()) continue;
                    if (input_takoyaki[targetPos.NormalIdx()] ==
                        input_goal[targetPos.NormalIdx()])
                      continue;
                    if (input_takoyaki[targetPos.NormalIdx()] && isHold)
                      continue;
                    if (input_goal[targetPos.NormalIdx()] && !isHold) continue;
                    rotateDirs.push_back(rotateDir);
                  }
                  shouldRotateDirs[idx] = rotateDirs;
                }
              }
            }
          }
        }
      }
    }

    // global_timers["expand"].start();
    {
      Action action;
      action.actions.resize(V, RawState::Action(kMoveU));
      vector<int> indexOrders(V);
      iota(indexOrders.begin(), indexOrders.end(), 0);
      saiki(0, action, evaluator, hash, parent, selector, indexOrders);
    }
    // {
    //   Action action;
    //   action.actions.resize(V, RawState::Action(kMoveU));
    //   vector<int> indexOrders(V);
    //   iota(indexOrders.begin(), indexOrders.end(), 0);
    //   reverse(indexOrders.begin() + state.robot.jointCount,
    //   indexOrders.end()); saiki(0, action, evaluator, hash, parent, selector,
    //   indexOrders);
    // }
    // global_timers["expand"].add();
  }

  // actionを実行して次の状態に遷移する
  void move_forward(Action action) {
    // 初期位置
    if (!action.startPos.IsDummy()) {
      state.robot.poses[0] = action.startPos;
      return;
    }
    for (const auto& a : action.actions) {
      state.ForwardAction(a);
      // if (a.IsNop()) state.ForwardNop(a.idx);
    }
    state.ForwardTurnEnd();
  }

  // actionを実行する前の状態に遷移する
  // 今の状態は、親からactionを実行して遷移した状態である
  void move_backward(Action action) {
    // 初期位置
    if (!action.startPos.IsDummy()) {
      return;
    }
    state.BackwardTurnEnd(true);
    for (int i = (int)action.actions.size() - 1; i >= 0; --i) {
      // if (action.actions[i].IsNop())
      // state.BackwardNop(action.actions[i].idx);
      state.BackwardAction(action.actions[i]);
    }
  }

 private:
  // 指番号, (手首の位置, 現在の向き, nopをしたか,
  // 保持しているか)を受けとって、試すべき方向を返す
  vector<vector<vector<RotateDir>>> finger2ShouldRotateDirs;
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
  ObjectPool<Node> nodes_;

 private:
  State state_;
  int root_;

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
  // デバッグ用
  // auto memoState = state;
  Tree tree(state, config.nodes_capacity, root);

  // 探索中のノード集合
  vector<int> curr_nodes;
  curr_nodes.reserve(config.beam_width);
  // 本来は curr_nodes = {state.root_} とすべきだが, 省略しても問題ない

  // 新しいノードの集合
  vector<int> next_nodes;
  next_nodes.reserve(config.beam_width);

  // 新しいノード候補の集合
  Selector normalSelector(config);

  auto firstConfig = config;
  firstConfig.beam_width = N * N;
  Selector firstSelector(firstConfig);

  bool kinkyuKaihi = false;
  for (int turn = 0; turn < config.max_turn; ++turn) {
    if(!kinkyuKaihi && global_timer.ms() >= 2800){
      kinkyuKaihi = true;
      auto newConfig = config;
      newConfig.beam_width = max(1, (int)config.beam_width / 10);
      normalSelector = Selector(newConfig);
    }
    // Euler Tour で selector に候補を追加する
    auto& selector = (turn == 0) ? firstSelector : normalSelector;
    tree.dfs(selector);

    // if (turn >= 1) {
    //   const int best_leaf = tree.get_best_leaf(curr_nodes);
    //   const int worst_leaf = tree.get_worst_leaf(curr_nodes);
    //   cerr << "turn: " << turn
    //        << " best_eval: " << tree.nodes_[best_leaf].evaluator.cost
    //        << " worst_eval: " << tree.nodes_[worst_leaf].evaluator.cost << endl;
    // }

    if (selector.have_finished()) {
      // ターン数最小化型の問題で実行可能解が見つかったとき
      Candidate candidate = selector.get_finished_candidates()[0];
      vector<Action> ret = tree.get_path(candidate.parent);
      ret.push_back(candidate.action);
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

  return {};

  // cout << "score: " << 10000 << endl;
  // exit(0);
  // ターン数固定型の問題で全ターンが終了したとき
  // int best_leaf = tree.get_best_leaf(curr_nodes);
  // const auto actions = tree.get_path(best_leaf);

  // rep(i, actions.size()) {
  //   const auto action = actions[i];
  //   memoState.move_forward(action);
  // }
  // return actions;
}

}  // namespace Beam2





/* start */

namespace Solver2 {

// 貪欲法
class Solver {
 public:
  Solver(const int firstLen, const int minFingerCount) {
    vector<int> lens;
    vector<int> parents;
    lens.push_back(0);
    parents.push_back(-1);
    lens.push_back(firstLen);
    parents.push_back(0);

    while ((int)lens.size() + minFingerCount < V) {
      const int parent = (int)lens.size() - 1;
      const int nextLen = lens.back() * 2;
      if (nextLen >= N / 2.0) break;
      lens.push_back(nextLen);
      parents.push_back(parent);
    }

    int wrist = (int)lens.size() - 1;
    int remainV = V - (int)lens.size();
    if (remainV == 2) {
      lens.push_back(1);
      parents.push_back(wrist);
      lens.push_back(3);
      parents.push_back(wrist);
    } else if (remainV == 3) {
      lens.push_back(1);
      parents.push_back(wrist);
      lens.push_back(3);
      parents.push_back(wrist);
      lens.push_back(5);
      parents.push_back(wrist);
    } else if (remainV >= 9) {
      lens.push_back(remainV / 2);
      parents.push_back(wrist);
      const int remainV = V - (int)lens.size();
      rep(t, (remainV + 1) / 2) {
        lens.push_back(t + 1);
        parents.push_back(wrist + 1);
      }
      rep(t, (remainV) / 2) {
        lens.push_back(t + 1);
        parents.push_back(wrist);
      }
    } else {
      rep(t, 100) {
        if ((int)lens.size() == V) break;
        lens.push_back(t + 1);
        parents.push_back(wrist);
      }
    }

    RawRobot robot(Pos(N / 2, N / 2), lens, parents);
    RawBoard board(input_takoyaki, input_goal);
    state = RawState(robot, board);

    cerr << "jointCount = " << state.robot.jointCount << endl;
    cerr << "fingerCount = " << state.robot.fingerCount << endl;
  }
  int Solve(ostream& os, const int timeLimit, const int maxTurn) {
    Beam2::Config config;
    const int fingerCount = state.robot.fingerCount;
    const double minTurn =
        (double)input_all_targets.size() / pow(fingerCount, 0.9);
    const double actionCount =
        5.0 * pow(3, state.robot.jointCount) * fingerCount;
    config.beam_width =
        (int)max(1.0, timeLimit / 3e3 * 4e7 / (actionCount * minTurn));
    cerr << "beam_width = " << config.beam_width << endl;
    config.max_turn = maxTurn;
    config.hash_map_capacity = 1 << 20;
    config.nodes_capacity = 1 << 20;

    Beam2::State initialState(state);
    Beam2::Node root(Beam2::Action{},
                     Beam2::Evaluator(initialState.state.Evaluate(true)),
                     initialState.GetHash());

    const auto actions = Beam2::beam_search(config, initialState, root);
    if (actions.empty()) return INF;
    vector<vector<RawState::Action>> answers;
    for (const auto& action : actions) {
      // 初期位置
      if (!action.startPos.IsDummy()) {
        state.robot.firstPos = action.startPos;
        continue;
      }
      answers.push_back(action.actions);
    }

    Print(os, state.robot, answers);

    cerr << global_timers << endl;
    cerr << "[ms]: " << global_timer.ms() << endl;
    cerr << "score = " << answers.size() << endl;
    return answers.size();
  }

 private:
  RawState state;
};

}  // namespace Solver2

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

  vector<pair<int, int>> firstLenAndMinFingerCounts;
  if (V == 5) {
    firstLenAndMinFingerCounts = {{N / 5.0, 2}, {N / 2.5, 2}};
  } else if (V == 6) {
    firstLenAndMinFingerCounts = {{N / 5.0, 2}};
  } else if (V == 7) {
    firstLenAndMinFingerCounts = {{N / 5.0, 2}};
  } else if (V <= 9) {
    firstLenAndMinFingerCounts = {{N / 5.0, 2}};
  } else {
    firstLenAndMinFingerCounts = {{2, 2}};
  }
  int minScore = 3000;
  string bestAnswer = "";
  for (const auto [firstLen, minFingerCount] : firstLenAndMinFingerCounts) {
    stringstream ss;
    Solver2::Solver solver(firstLen, minFingerCount);
    const int score = solver.Solve(
        ss, 2800.0 / firstLenAndMinFingerCounts.size(), minScore + 1);
    if (chmin(minScore, score)) {
      bestAnswer = ss.str();
    }
  }
  cout << bestAnswer;
  cout.flush();
  cerr << "total[ms]= " << global_timer.ms() << endl;
  cerr << "bestScore= " << minScore << endl;
  std::quick_exit(0);
  return 0;
}