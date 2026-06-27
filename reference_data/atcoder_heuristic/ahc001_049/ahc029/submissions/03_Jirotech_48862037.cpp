

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


#include <fstream>
#include <random>








#include <fstream>
#include <random>





/* start */

constexpr int kTimeLimit_1 = 1600;
constexpr int kTimeLimit_2 = 1950;

// ケースごとの調整用
// 1,2,6,7,8,9,10,11,14,15,16,17,18,(22,23,)24,25,27

// K=2 log_mean: 328607.8757927714
vector<double> PARAMS_K2 = {1786,
                            0.7200767265063516,
                            4.197614521052023,
                            0.6484227916022324,
                            0.5597836398559154,
                            38.76668642345467,
                            0.9211907851445684,
                            0.646160248959329,
                            21.41321543497137,
                            26.51789343049032,
                            98.71623820100884,
                            429.6527479273086,
                            559.9995854739834,
                            0.6117856796820025,
                            0.6335914790571896,
                            3,
                            2,
                            0.021800534741293162};

// N=2 8178108.735781805
vector<double> PARAMS_N2 = {1845,
                            0.6817696379645874,
                            5.441558716098849,
                            0.5628107539571935,
                            0.6105232691193987,
                            7.363529797148727,
                            0.5918491663811098,
                            0.8027637496187451,
                            2.9103081074796657,
                            13.704473685347274,
                            81.41971481116407,
                            321.650460565697,
                            124.16707893716296,
                            0.5328499080811967,
                            0.5304322038197765,
                            6,
                            0,
                            0.10465582998022167};

// M=2,3 log_mean: 4346419.359669287
vector<double> PARAMS_M2M3 = {1785,
                              0.6645333921967271,
                              4.7804881593479625,
                              0.5706336351453692,
                              0.5800682977793808,
                              44.45299109495714,
                              0.815402870022921,
                              0.6907599042309976,
                              315.9558826892462,
                              20.477487331796144,
                              76.60746101428238,
                              355.4628050596887,
                              103.66337977360881,
                              0.5511691793733402,
                              0.5303638997224329,
                              0,
                              3,
                              0.05565633467962761};

// vector<double> PARAMS2 = {1845,
//                           0.6817696379645874,
//                           5.441558716098849,
//                           0.5628107539571935,
//                           0.6105232691193987,
//                           7.363529797148727,
//                           0.5918491663811098,
//                           0.8027637496187451,
//                           2.9103081074796657,
//                           13.704473685347274,
//                           81.41971481116407,
//                           321.650460565697,
//                           124.16707893716296,
//                           0.5328499080811967,
//                           0.5304322038197765,
//                           6,
//                           0,
//                           0.10465582998022167};

#if 0
vector<double> PARAMS = {1.0,  1800, 0.7, 10,  1.0, 1.5, 3.0, 0.9, 0.5,  // 8
                         10.0, 0.9,  0.8, 5.0, 50,  100, 20,  50,  400,  // 17
                         150,  0.9,  0.9, 0.9, 0.9, 0.8, 3.0, 3.0, 1,    // 26
                         0.05, 3};                                       // 28
#elif 0
// log_mean: 22021925.91226769
vector<double> PARAMS = {0.9966404669857151,
                         1812,
                         0.6493386234618418,
                         35.054325686997345,
                         1.480539515964294,
                         0.5755786242741348,
                         3.714672300601199,
                         0.5833764807987553,
                         0.49977334376284394,
                         22.45292369697056,
                         0.8448926954546205,
                         0.7961005029981594,
                         5,
                         0.16483334318011333,
                         174.6996873533079,
                         14.922023138049042,
                         85.73164754834795,
                         366.9324856402369,
                         322.10620000771274,
                         0.8129087770754753,
                         0.791879908032711,
                         0.8578697818825277,
                         0.7782213650974138,
                         0.5637917515221639,
                         3,
                         1,
                         1,
                         0.03041388107629327,
                         4};
#elif 1
// log_mean: 22290611.29752665
vector<double> PARAMS = {0.9926753316445034,
                         1815,
                         0.6773430948303393,
                         35.69555841855746,
                         1.4406940276313003,
                         0.5982754236914968,
                         2.7737285987679137,
                         0.6454779028035175,
                         0.4968290266401932,
                         19.6704057537192,
                         0.8075214993909438,
                         0.7779378116135154,
                         4,
                         0.10024253060659438,
                         236.16443855796717,
                         13.643375156875631,
                         82.7934265139224,
                         419.1573039912249,
                         302.07461028672594,
                         0.8251221047273809,
                         0.7320867633931263,
                         0.8209193175198605,
                         0.8256311452093579,
                         0.5000745383764912,
                         3,
                         2,
                         1,
                         0.033873953969991155,
                         4};
#elif 0
// log_mean: 17798770.325490277
vector<double> PARAMS = {0.9792777937461341,
                         1808,
                         0.7,
                         56.420984436094535,
                         1.07544959962232,
                         1.2269421009321855,  // 5
                         2.0100962956712016,
                         0.5407228093589489,
                         0.6368265226874256,  // 8
                         8.423818567851797,
                         0.9832101924206184,
                         0.8372653004357767,  // 11
                         3,
                         0.26123111291375845,
                         129.39026566197649,  // 14
                         29.917520047473896,
                         60.32652176785787,
                         320.11006744821054,  // 17
                         104.11899808230818};
#else
// log_mean: 17346296.16609269
vector<double> PARAMS = {0.9976420193348848,
                         1793.0,
                         0.7,
                         11.347311384301523,  // 3
                         1.097423776631111,
                         1.2743234463845607,
                         1.2052079940808351,  // 6
                         0.5412460374263302,
                         0.6005452407940396,
                         17.679292130645326,  // 9
                         0.9896474571483871,
                         0.8673127959745919,
                         3.0,  // 12
                         0.14539952692028058,
                         172.61433652721215,
                         24.68077793299261,  // 15
                         58.87334172502878,
                         330.76920603917875,
                         62.91438917632652};
#endif

/* start */

std::random_device seed_gen;
std::default_random_engine engine(0);
int N, M, K, T;
vector<int> input_card_kinds;
vector<double> generated_probs;
double generated_sum = 1e-5;

struct Card {
  int t;
  ll w;
  void operator*=(const ll mul) { w *= mul; }
  bool IsWork() const { return t <= 1; }
  bool IsSingleWork() const { return t == 0; }
  bool IsMultiWork() const { return t == 1; }
  void SetUsed() { t = -1; }
  bool IsUsed() const { return t < 0; }
  bool IsNoTarget() const { return t == 1 || t == 3 || t == 4; }
  bool IsRankUp() const { return t == 4; }
  bool IsSingleBomb() const { return t == 2; }
  bool IsMultiBomb() const { return t == 3; }
  bool IsBomb() const { return t == 2 || t == 3; }
  bool IsMinimumWork(const int L2) const { return t == 0 && w <= L2; }
};

struct Item {
  Card card;
  ll p;
  void operator*=(const ll mul) {
    card *= mul;
    p *= mul;
  }
  static Item Random() {
    Card card;
    card.t = rand_vec(input_card_kinds);
    card.w = 0;
    if (card.t <= 1) {
      card.w = rand(1, 51);
    }

    Item item;
    item.card = card;

    switch (card.t) {
      case 0: {
        const double www = card.w;
        std::normal_distribution<> dist(www, www / 3.0 * PARAMS[19]);
        item.p = std::clamp((int)std::round(dist(engine)), 1, 10000);
        return item;
      }
      case 1: {
        const double www = card.w * M;
        std::normal_distribution<> dist(www, www / 3.0 * PARAMS[20]);
        item.p = std::clamp((int)std::round(dist(engine)), 1, 10000);
        return item;
      }
      case 2:
      case 3:
        item.p = rand(0, 11);
        return item;
      case 4:
        item.p = rand(200, 1001);
        return item;
    }
    assert(false);
    return Item();
  }
};

struct Project {
  ll h;
  ll v;
  ll max_h;
  static Project Random() {
    Project ret;
    constexpr int kInterval = 100000;
    const double b = rand(2 * kInterval, 8 * kInterval) / (double)(kInterval);
    ret.h = ret.max_h = std::round(pow(2, b));
    // 平均b、標準偏差0.5で分布させる
    std::normal_distribution<> dist(b, 0.5 * PARAMS[21]);
    ret.v = std::round(pow(2, std::clamp(dist(engine), 0.0, 10.0)));

    return ret;
  }
  void operator*=(const ll mul) {
    h *= mul;
    v *= mul;
    max_h *= mul;
  }
  bool Decrese(const ll damage) {
    h -= damage;
    return h <= 0;
  }
};

struct Supply : vector<Item> {
  void Input(istream& is) {
    this->clear();
    rep(i, K) {
      int t, w, p;
      is >> t >> w >> p;
      this->push_back({Card{t, w}, p});
      if (i >= 1) {
        input_card_kinds.push_back(t);
        generated_probs[t] += 1.0;
        generated_sum += 1.0;
      }
    }
  }
  void operator*=(const ll mul) {
    rep(k, K) { (*this)[k] *= mul; }
  }

  static Supply Random() {
    Supply ret;
    ret.push_back(Item{Card{0, 1}, 0});
    rep(i, K - 1) { ret.push_back(Item::Random()); }
    return ret;
  }
};

struct Hand : vector<Card> {
  int rankup_count = 0;
  int used_card = 0;
  void Init(istream& is) {
    this->clear();
    rep(i, N) {
      ll t, w;
      is >> t >> w;
      this->push_back({(int)t, w});
      rankup_count += t == 4;
    }
  }
};

struct Board : vector<Project> {
  void Input(istream& is) {
    if ((int)this->size() == 0) {
      this->resize(M, {-1, -1, -1});
    }
    rep(i, M) {
      ll h, v;
      is >> h >> v;
      if (this->at(i).h != h) {
        this->at(i) = {h, v, h};
      }
    }
  }

  void operator*=(const ll mul) {
    rep(j, M) { (*this)[j] *= mul; }
  }

  ll UseCard(const Card& card, const int target) {
    ll money_sum = 0;
    if (card.t == 0) {
      if (this->at(target).Decrese(card.w)) {
        money_sum += this->at(target).v;
      }
    } else if (card.t == 1) {
      rep(j, M) {
        if (this->at(j).Decrese(card.w)) {
          money_sum += this->at(j).v;
        }
      }
    } else if (card.t == 2) {
      this->at(target).Decrese(LINF);
    } else if (card.t == 3) {
      rep(j, M) { this->at(j).Decrese(LINF); }
    }

    return money_sum;
  }
};


/* start */

struct Judge {
  static constexpr int kMaxNum = 80;
  int id;
  int project_j;
  bool is_valid = false;

  Judge() {}
  Judge(const int _id) : id(_id), project_j(0), is_valid(true) {}

  Project NextProject() { return projects[id][project_j++]; }
  Supply NextSupply(const int turn) { return supplies[id][turn / 2]; }
  bool IsValid() const { return is_valid; }

  static void Init() {
    projects.resize(kMaxNum);
    rep(i, kMaxNum) {
      projects[i].reserve(T * 2);
      rep(t, T * 2) { projects[i].push_back(Project::Random()); }
    }

    supplies.resize(kMaxNum);
    rep(x, kMaxNum) {
      supplies[x].resize(T + 1);
      for (int i = 0; i < (int)supplies[x].size(); ++i) {
        supplies[x][i] = Supply::Random();
      }
    }
  }

  static void Update(const int current_turn) {
    supplies.resize(kMaxNum);
    rep(x, kMaxNum) {
      assert((int)supplies[x].size() > 0);
      for (int i = current_turn / 2; i < (int)supplies[x].size(); ++i) {
        supplies[x][i] = Supply::Random();
      }
    }
  }

  static vector<vector<Project>> projects;
  static vector<vector<Supply>> supplies;
};

vector<vector<Project>> Judge::projects;
vector<vector<Supply>> Judge::supplies;



/* start */

struct State {
  Supply supply;
  Hand hand;
  Board board;
  ll money = 0;
  int L = 0;
  ll L2 = 1;
  int turn = 0;
  Judge judge;
  ll power_sum = 0;
  // 最初にあったとしても使わないほうがいいらしい
  // 初期手札が資産なので
  int last_rankup_turn = -100;
  // Simulatorで使う
  bool prohibit_use_rankup = false;

  ll TotalMoney() const {
    ll ret = money;
    rep(i, N) {
      const auto& card = hand[i];
      if (card.IsSingleWork()) {
        ret += card.w * PARAMS[22];
      } else if (card.IsMultiWork()) {
        ret += card.w * M * PARAMS[23];
      }
    }

    rep(j, M) {
      const auto& project = board[j];
      ret += project.max_h - project.h;
    }
    return ret;
  }

  void Init(istream& is) {
    hand.Init(is);
    board.Input(is);
    supply.resize(K);
    UpdatePowerSum();
    ll sum = 0;
    rep(i, N) {
      const auto& card = hand[i];
      if (card.IsWork()) {
        if (card.IsSingleWork()) {
          sum += card.w;
        } else {
          sum += card.w * M;
        }
      }
    }
    // 初期手札が微妙ならさっさと使う
    if (sum < 100) {
      last_rankup_turn = -2;
    }
  }

  void UpdatePowerSum() {
    power_sum = 0;
    rep(i, N) {
      const auto& card = hand[i];
      if (card.IsUsed()) continue;
      if (!card.IsWork()) continue;
      if (card.IsMinimumWork(L2)) continue;
      power_sum += card.w;
    }
  }

  void UseCard(const int i, const int j, Judge& judge) {
    if (!judge.IsValid()) {
      UseCard(i, j);
      return;
    }
    if (hand[i].IsRankUp()) {
      L++;
      L2 *= 2;
      hand.rankup_count--;
      last_rankup_turn = turn;
      prohibit_use_rankup = false;
    }
    money += board.UseCard(hand[i], j);
    hand[i].SetUsed();
    hand.used_card = i;
    turn++;

    rep(i, M) {
      if (board[i].h <= 0) {
        board[i] = judge.NextProject();
        board[i] *= L2;
      }
    }
    supply = judge.NextSupply(turn);
    supply *= L2;
    UpdatePowerSum();
  }

  void BuyCard(const int i, Judge& judge) {
    if (!judge.IsValid()) {
      BuyCard(i);
      return;
    }
    money -= supply[i].p;
    hand[hand.used_card] = supply[i].card;
    if (supply[i].card.IsRankUp()) {
      prohibit_use_rankup = false;
    }
    turn++;
    hand.rankup_count += supply[i].card.IsRankUp();
    UpdatePowerSum();
  }

 private:
  void UseCard(const int i, const int j) {
    cout << i << " " << j << endl;
    if (hand[i].IsRankUp()) {
      L++;
      L2 *= 2;
      hand.rankup_count--;
      last_rankup_turn = turn;
      prohibit_use_rankup = false;
    }
    money += board.UseCard(hand[i], j);
    hand[i].SetUsed();
    hand.used_card = i;
    turn++;

    board.Input(cin);
    ll debug_money;
    cin >> debug_money;
    CHECK(debug_money, ==, money);
    supply.Input(cin);
    UpdatePowerSum();
  }

  void BuyCard(const int i) {
    cout << i << endl;
    money -= supply[i].p;
    hand[hand.used_card] = supply[i].card;
    turn++;
    hand.rankup_count += supply[i].card.IsRankUp();
    if (supply[i].card.IsRankUp()) {
      prohibit_use_rankup = false;
    }
    UpdatePowerSum();
  }
};

State input_state;

/* start */

Timer global_timer;

ll lpow(ll a, ll b) {
  if (b == 0)
    return 1;
  else if (b % 2 == 0) {
    ll memo = lpow(a, b / 2);
    return memo * memo;
  } else
    return lpow(a, b - 1) * a;
}

struct Output {
  static void StaticInit(istream& is) {
    global_timer.start();

    // TODO: 要検討
    vector<int> init_counts = {1, 1, 1, 1, 1};
    rep(t, 5) {
      rep(_t, init_counts[t]) { input_card_kinds.push_back(t); }
    }
    generated_probs.assign(5, 0.0);

    input_state.Init(is);
  }
  friend ostream& operator<<(ostream& os, const Output& output) { return os; }
};








/* start */
class Simulator {
 public:
  State state;
  bool CanBuy(const int idx) const {
    const auto& item = state.supply[idx];
    const auto& card = item.card;
    if (item.p > state.money) return false;
    if (card.IsRankUp() && (state.turn >= PARAMS[1] * T / 1000 ||
                            state.L + state.hand.rankup_count >= 20))
      return false;
    return true;
  }
  bool CheckUseRankUp() {
    if (state.prohibit_use_rankup) return false;
    if ((state.money >= state.L2 * PARAMS[18] ||
         N - state.hand.rankup_count >= 2) &&
        state.last_rankup_turn != state.turn - 2 &&
        state.L + state.hand.rankup_count <= 19 &&
        state.turn < PARAMS[1] * T / 1000)
      return false;

    return true;
  }
  bool TryUseRankUp(const bool force = false) {
    // 持ってたら即使う
    rep(i, N) {
      if (state.hand[i].IsRankUp()) {
        if (force || CheckUseRankUp()) {
          state.UseCard(i, 0, judge);
          return true;
        } else {
          return false;
        }
      }
    }
    return false;
  }
  bool TryUseMultiBomb(const bool force = false) {
    int bomb_idx = -1;
    rep(i, N) {
      if (state.hand[i].IsMultiBomb()) {
        bomb_idx = i;
        break;
      }
    }
    if (bomb_idx == -1) return false;

    double diff_sum = 0;
    rep(i, M) {
      const auto& project = state.board[i];
      diff_sum += project.v - project.h;
    }
    if (!force && diff_sum >= 0) return false;
    state.UseCard(bomb_idx, 0, judge);
    return true;
  }
  bool TryUseSingleBomb(const bool force = false) {
    int bomb_idx = -1;
    rep(i, N) {
      if (state.hand[i].IsSingleBomb()) {
        bomb_idx = i;
        break;
      }
    }
    if (bomb_idx == -1) return false;
    // 爆弾を使うことでアドバンテージが得られるなら使う
    int best_i = -1;
    double min_eval = LINF;
    rep(i, M) {
      const auto& project = state.board[i];
      if (!force && project.v >= project.h) continue;
      // 1HPあたりの負の利益が最大のものをターゲットとしたい
      const double eval = (double)(project.v - project.h) / project.h;
      if (eval < min_eval) {
        min_eval = eval;
        best_i = i;
      }
    }

    if (best_i == -1) return false;

    state.UseCard(bomb_idx, best_i, judge);
    return true;
  }
  void UseWork() {
    int min_h_j = 0;
    {
      // MinimumWorkerは最も小さいのを対象とする
      pair<double, double> max_eval = {-LINF, 0};
      rep(j, M) {
        const ll h = state.board[j].h;
        const pair<double, double> eval = {-h, state.board[j].v};
        if (eval > max_eval) {
          max_eval = eval;
          min_h_j = j;
        }
      }
    }
    // 火力あたりの利益を最大化
    int best_card = -1;
    int best_target = -1;
    double max_eval = -LINF;
    ll min_h = INF;
    rep(j, M) { chmin(min_h, state.board[j].h); }
    rep(i, N) {
      const auto& card = state.hand[i];
      if (!card.IsWork()) continue;
      if (card.IsSingleWork()) {
        rep(j, M) {
          double eval = 0.0;
          // MinimumWorkerは最も小さいのを対象とする
          if (card.IsMinimumWork(state.L2) && j != min_h_j) continue;

          const auto& project = state.board[j];
          eval = ((double)project.v /
                  (project.h +
                   (state.money <= state.L2 * PARAMS[3] ? 0.05 : 0.0))) *
                     min((ll)(project.h * 1.05), card.w) -
                 card.w;
          eval -= max(0.0, project.h - state.power_sum * PARAMS[4] -
                               state.money * PARAMS[5] - state.L2 * PARAMS[6]) *
                  1e5;
          if (!card.IsMinimumWork(state.L2)) {
            eval += min(0.0, project.h - card.w * PARAMS[7]);
            eval += min(0.0, project.h - card.w * PARAMS[8]) * PARAMS[9];
          }

          if (eval > max_eval) {
            max_eval = eval;
            best_card = i;
            best_target = j;
          }
        }
      } else {
        double eval = 0.0;
        rep(j, M) {
          const auto& project = state.board[j];
          eval += (double)project.v / project.h *
                      min(project.h * 1.05, (double)card.w) -
                  card.w;
        }
        if (eval > max_eval) {
          max_eval = eval;
          best_card = i;
          best_target = 0;
        }
      }
    }
    // WorkAround: 手札にWorkがない場合は仕方がないのでBombを打つ
    if (best_card == -1) {
      if (TryUseSingleBomb(true)) return;
      if (TryUseRankUp(true)) return;
      if (TryUseMultiBomb(true)) return;
    }

    assert(best_card != -1);
    // SingleWorkの場合、被ダメ済みかつh <=
    // vであるような他のプロジェクトについて、 w <=
    // hなものがあれば、hが最小のものにターゲットを変更する
    // あんまり聞いてない
    if (state.hand[best_card].IsSingleWork() &&
        !state.hand[best_card].IsMinimumWork(state.L2)) {
      const ll w = state.hand[best_card].w;
      ll min_h = state.board[best_target].h;
      rep(j, M) {
        const auto& project = state.board[j];
        if (project.h != project.max_h && project.h * 10.0 <= project.v &&
            w <= project.h && project.h < min_h) {
          min_h = project.h;
          best_target = j;
        }
      }
    }

    state.UseCard(best_card,
                  (state.hand[best_card].IsSingleWork() ? best_target : 0),
                  judge);
  }
  void TurnUseCard() {
    if (TryUseRankUp()) return;
    if (TryUseMultiBomb()) return;
    if (TryUseSingleBomb()) return;
    UseWork();
  }
  int ChooseBuyCard(const bool can_buy_rankup = true) const {
    double max_eval = -INF;
    int best_k = -1;
    int single_bomb_count = 0;
    int buffer_count = 0;
    bool multi_bomb_exist = false;
    rep(i, N) {
      const auto& card = state.hand[i];
      if (card.IsUsed()) continue;
      single_bomb_count += card.IsSingleBomb();
      if (card.IsMinimumWork(state.L2)) {
        buffer_count++;
      } else if (card.IsSingleWork()) {
        rep(j, M) {
          if (card.w <= state.board[j].h) {
            buffer_count++;
            break;
          }
        }
      } else if (card.IsSingleBomb()) {
        buffer_count++;
      } else if (card.IsMultiBomb()) {
        multi_bomb_exist = true;
      }
    }
    rep(k, K) {
      const auto& item = state.supply[k];
      const auto& card = item.card;
      if (!CanBuy(k)) continue;
      if (!can_buy_rankup && card.IsRankUp()) continue;
      double eval = 0.0;
      if (card.IsSingleWork()) {
        eval = -INF;
        rep(j, M) {
          const auto& project = state.board[j];
          eval = max(
              eval, (double)max(0ll, project.v - max(0ll, project.h - card.w)) -
                        max(0ll, project.v - project.h) - item.p);
          if (project.h - state.power_sum >= state.L2 * (ll)PARAMS[24] &&
              project.h - card.w <= state.L2 * (ll)PARAMS[25]) {
            // 他に倒せるSingleWorkがない &&
            // ほぼ一撃で倒せるなら、プロジェクト起因のアドバンテージを回収しに行く
            const auto value = (double)project.v / project.max_h *
                                   min((ll)(project.h * 1.00), card.w) *
                                   PARAMS[10] -
                               item.p;
            eval = max(eval, value);
          }
        }
        if (buffer_count >= 1) {
          eval = max(eval, card.w * PARAMS[11] - item.p);
        }
      } else if (card.IsMultiWork()) {
        rep(j, M) {
          const auto& project = state.board[j];
          eval += max(0ll, project.v - max(0ll, project.h - card.w)) -
                  max(0ll, project.v - project.h);
        }
        eval -= item.p;
        eval = max(eval, card.w * PARAMS[2] * M - item.p);
      } else if (card.IsSingleBomb()) {
        // 良い要素が十分ある && お金に余裕がない場合は買わない
        int good_count = 0;
        rep(j, M) {
          good_count += (double)state.board[j].v / state.board[j].h >= 1.0;
        }

        const auto prob = generated_probs[2] / generated_sum * (K - 1);
        if (good_count >= 3 && prob >= 0.5 && item.p >= 5 * state.L2) continue;

        eval = -INF;
        if (single_bomb_count <= PARAMS[26] &&
            (good_count < 2 || state.money - item.p >= state.L2 * PARAMS[13])) {
          rep(j, M) {
            const auto& project = state.board[j];
            // 1ターンあたりに換算したほうが良いかも
            eval = max(eval, (double)project.h - project.v - item.p);
          }
          if (N - state.hand.rankup_count >= PARAMS[28]) {
            eval = max(eval, (double)state.L2 * PARAMS[12] - item.p);
          }
        }
      } else if (card.IsMultiBomb()) {
        // 既に持ってるなら買わない
        if (multi_bomb_exist) {
          eval = -INF;
        } else {
          eval = 0.0;
          rep(j, M) {
            const auto& project = state.board[j];
            // 1ターンあたりに換算したほうが良いかも
            eval += (double)project.h - project.v;
          }
          eval -= item.p;
        }
      } else {
        const auto prob = generated_probs[4] / generated_sum * (K - 1);
        if (state.money - item.p >= state.L2 * 2 *
                                        std::clamp((int)(prob * PARAMS[14]),
                                                   (int)PARAMS[15],
                                                   (int)PARAMS[16])) {
          // 見送ってもすぐ次が来るなら、安いときに買う
          // TODO: DP
          if (prob < PARAMS[27] || item.p < PARAMS[17] * state.L2) {
            eval = INF - (double)item.p / state.L2;
          }
        }
      }

      if (eval > max_eval) {
        max_eval = eval;
        best_k = k;
      }
    }
    if (!judge.IsValid()) {
      cout << "#";
      rep(t, 5) { cout << " " << generated_probs[t] / generated_sum; }
      cout << endl;
    }

    assert(best_k != -1);
    return best_k;
  }
  void TurnBuyCard(const bool can_buy_rankup = true) {
    const int buy_card = ChooseBuyCard(can_buy_rankup);
    state.BuyCard(buy_card, judge);
  }
  void Turn() {
    TurnUseCard();
    TurnBuyCard();
  }

  void Run(int max_turn = INF) {
    chmin(max_turn, T * 2);
    while (state.turn < max_turn) {
      if (state.turn % 2 == 0) {
        TurnUseCard();
      } else {
        TurnBuyCard();
      }
    }
  }

  void SetJudge(const Judge _judge) { judge = _judge; }

  Judge judge;

 private:
  int target = -1;
};

/* start */
class Solver {
 public:
  Solver() {}
  void Solve() {
    Judge::Init();
    Judge dummy_judge;
    Simulator sim;
    sim.state = input_state;
    const int kLastTurn = 50;
    rep(_t, 1000 - kLastTurn) {
      bool done_use_card = false;
      if (_t != 0 && _t % 200 == 0) {
        Judge::Update(_t * 2);
      }

      if (sim.state.hand.rankup_count > 0 && !sim.state.prohibit_use_rankup &&
          global_timer.ms() < kTimeLimit_1) {
        // 昨日買ったなら、RankUpすることを検討する
        int rank_up_i = -1;
        rep(i, N) {
          const auto& card = sim.state.hand[i];
          if (!card.IsRankUp()) continue;
          rank_up_i = i;
          break;
        }
        assert(rank_up_i != -1);

        // ランクアップを使うかモンテカルロによって判断
        const int kTrialNum = Judge::kMaxNum;
        ll original_money = 0;
        {
          rep(trial, kTrialNum) {
            auto sim2 = sim;
            // 次購入するまでは使わない
            sim2.state.prohibit_use_rankup = true;
            Judge judge(trial);
            sim2.SetJudge(judge);
            sim2.Run();
            original_money += sim2.state.TotalMoney();
          }
        }

        // 次に、使った場合のシミュレーション
        ll rank_up_money = 0;
        {
          rep(trial, kTrialNum) {
            auto sim2 = sim;
            Judge judge(trial);
            sim2.SetJudge(judge);
            sim2.state.UseCard(rank_up_i, 0, sim2.judge);
            sim2.Run();
            rank_up_money += sim2.state.TotalMoney();
          }
        }

        if (rank_up_money > original_money) {
          // 使う判断
          sim.state.UseCard(rank_up_i, 0, dummy_judge);
          done_use_card = true;
          rep(i, N) {
            if (sim.state.hand[i].IsUsed()) continue;
            if (sim.state.hand[i].IsRankUp()) {
              break;
            }
          }
        } else {
          // 使わない判断
          sim.state.prohibit_use_rankup = true;
        }
      }

      if (!done_use_card) {
        sim.TurnUseCard();
        done_use_card = true;
      }
      int rank_up_k = -1;
      ll min_rank_up_cost = LINF;
      rep(k, K) {
        const auto& item = sim.state.supply[k];
        if (!item.card.IsRankUp()) continue;
        if (!sim.CanBuy(k)) continue;
        if (item.p < min_rank_up_cost) {
          min_rank_up_cost = item.p;
          rank_up_k = k;
        }
      }
      if (rank_up_k == -1 || global_timer.ms() >= kTimeLimit_1) {
        sim.TurnBuyCard();
        continue;
      }

      // まず、普通にシミュレーションする場合にRankUpを買うか確認
      {
        const int k = sim.ChooseBuyCard();
        if (sim.state.supply[k].card.IsRankUp()) {
          // 買うなら、買う
          // 「貪欲上買うものを買わない」は、以降のターンで結局買ってしまい、一貫性がなくなるので、利益が出ない
          // 「貪欲上買わないものを買う」のみをモンテカルロによって検討する
          sim.TurnBuyCard();
          continue;
        }
      }

      // ランクアップを買うかモンテカルロによって判断
      const int kTrialNum = Judge::kMaxNum;
      ll original_money = 0;
      {
        rep(trial, kTrialNum) {
          auto sim2 = sim;
          Judge judge(trial);
          sim2.SetJudge(judge);
          sim2.Run();
          original_money += sim2.state.money;
        }
      }

      // 次に、買った場合のシミュレーション
      ll rank_up_money = 0;
      {
        rep(trial, kTrialNum) {
          auto sim2 = sim;
          Judge judge(trial);
          sim2.SetJudge(judge);
          sim2.state.BuyCard(rank_up_k, sim2.judge);
          sim2.Run();
          rank_up_money += sim2.state.money;
        }
      }

      if (rank_up_money > original_money * PARAMS[0]) {
        sim.state.BuyCard(rank_up_k, dummy_judge);
      } else {
        sim.TurnBuyCard();
      }
    }
    Judge::Update(sim.state.turn);
    rep(_t, kLastTurn) {
      if (global_timer.ms() >= kTimeLimit_2) {
        sim.TurnUseCard();
        sim.TurnBuyCard();
        continue;
      }
      sim.TurnUseCard();
      int best_buy_card = 0;
      ll max_money_sum = -LINF;
      rep(k, K) {
        if (!sim.CanBuy(k)) continue;
        const int kTrialNum = Judge::kMaxNum;
        double sum = 0;
        double log_sum = 0;
        rep(trial, kTrialNum) {
          auto sim2 = sim;
          Judge judge(trial);
          sim2.SetJudge(judge);
          sim2.state.BuyCard(k, sim2.judge);
          sim2.Run();
          sum += sim2.state.money;
          log_sum += log(sim2.state.money + 1);
        }
        // cerr << "mean:" << sum / kTrialNum << endl;
        // cerr << "log_mean:" << exp(log_sum / kTrialNum) << endl;
        if (sum > max_money_sum) {
          best_buy_card = k;
          max_money_sum = sum;
        }
      }
      cout << "# " << global_timer.ms() << endl;

      sim.state.BuyCard(best_buy_card, dummy_judge);
    }

    std::quick_exit(0);
    return;
  }

 private:
  int target = -1;
  ll power_sum = 0;
};

int main(int argc, char* argv[]) {
  fast_io;

  vector<int> idxes{1,  2,  6,  7,  8,  9,  10, 11, 14,
                    15, 16, 17, 18, 22, 23, 24, 25, 27};
  // if (argc >= 2) {
  //   int idx = 0;
  //   for (int i = 1; i < argc; ++i) {
  //     PARAMS2[idx++] = std::stod(argv[i]);
  //   }
  // }

  Timer timer;
  timer.start();
  cin >> N >> M >> K >> T;
  if (K == 2) {
    rep(i, PARAMS_K2.size()) { PARAMS[idxes[i]] = PARAMS_K2[i]; }
  } else if (N == 2) {
    rep(i, PARAMS_N2.size()) { PARAMS[idxes[i]] = PARAMS_N2[i]; }
  } else if (M == 2 || M == 3) {
    rep(i, PARAMS_M2M3.size()) { PARAMS[idxes[i]] = PARAMS_M2M3[i]; }
  }

  Output::StaticInit(cin);

  // if (K == 2 || N == 2) {
  //   Simulator sim;
  //   sim.state = input_state;
  //   sim.Run();
  //   return 0;
  // }
  Solver solver;
  solver.Solve();
  return 0;
}