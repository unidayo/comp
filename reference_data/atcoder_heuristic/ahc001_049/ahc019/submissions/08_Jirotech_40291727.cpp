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
#define all(v) v.begin(), v.end()
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




/* start */

// 4~10がprobs[0~6]
vector<double> PARAMS = {0.201948712490333,
                         8.74490876214633,
                         1.018737307607095,
                         1.0119157230424247,
                         4.348988225095003,
                         43.947920139233666,
                         5.954229975611572,
                         65.8239040427743,
                         63.64846000713827,
                         8.992373078714241,
                         93.2929118925929,
                         39,
                         74,
                         13,
                         0.07630368078968593,
                         74,
                         0.2908334106615962,
                         0.12607085541493837,
                         0.3,
                         0.1};





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


// NBD = neighborhood
template <class Solution, class NBDGenerator>
struct SimulatedAnnealing {
 public:
  SimulatedAnnealing(double end_time, double start_temp, double end_temp)
      : end_time_(end_time),
        inv_time_(1.0 / end_time_),
        cur_time_(0.0),
        start_temp_(start_temp),
        end_temp_(end_temp),
        diff_temp_(end_temp - start_temp),
        cur_temp_(start_temp) {
    timer_.start();
  }

  Solution run(const Solution& initial_sol, NBDGenerator& nbd_generator) {
    Solution best_sol(initial_sol);
    Solution sol(initial_sol);

    int acc = 0;
    int g = 0;
    for (;; ++g) {
      // if (g % 10000 == 0) {
      //   cout << sol.Eval() << " " << sol.Score() << endl;
      //   cout << sol.Convert() << endl << endl;
      // }
      if ((g & 0xf) == 0) {
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

      // const auto prev_sol = sol;
      const auto nbd = nbd_generator.Generate(&sol, g);
      constexpr int mod = 1'000'000'000;
      const int r = rand(0, mod);
      if (static_cast<double>(r) / mod < exp(-nbd->Diff() / cur_temp_)) {
        acc++;
        nbd->Update(&sol);
      } else {
        // const auto mid_sol = sol;
        nbd->Restore(&sol);
        // if (prev_sol != sol) {
        //   for (const auto& obj : prev_sol.pos_pairs) {
        //     cerr << obj.Idx() << " ";
        //   }
        //   for (const auto& obj : mid_sol.pos_pairs) {
        //     cerr << obj.Idx() << " ";
        //   }
        //   for (const auto& obj : sol.pos_pairs) {
        //     cerr << obj.Idx() << " ";
        //   }
        //   abort();
        // }
      }
    }
    cerr << g << " " << acc << " " << nbd_generator.BestSol().Score() << endl;
    return nbd_generator.BestSol();
  }

 private:
  double end_time_, inv_time_, cur_time_;
  double start_temp_, end_temp_, diff_temp_, cur_temp_;
  Timer timer_;

  void UpdateTime() { cur_time_ = timer_.ms(); }
  void UpdateTemp() {
    cur_temp_ = max(0.0, start_temp_ - cur_time_ * diff_temp_ * inv_time_);
  }
};



enum Dir {
  // z-1, z+1, y-1, y+1, x-1, x+1
  kU,
  kD,
  kB,
  kF,
  kL,
  kR
};

vector<vector<Dir>> kind_dir_2_rotated_dir;

inline Dir RotateDir(const int kind, const Dir dir) {
  return kind_dir_2_rotated_dir[kind][dir];
}

Dir RotateDirForCreate(const int kind, const Dir dir) {
  int x = 3;
  int y = 2;
  int z = 1;
  bool plus = true;
  if (kind % 6 == 0) {
  } else if (kind % 6 == 1) {
    swap(x, y);
    plus = !plus;
  } else if (kind % 6 == 2) {
    swap(x, z);
    plus = !plus;
  } else if (kind % 6 == 3) {
    swap(y, z);
    plus = !plus;
  } else if (kind % 6 == 4) {
    swap(x, y);
    swap(y, z);
  } else {
    swap(x, z);
    swap(y, z);
  }

  if (kind / 6 % 2 == 1) {
    x = -x;
    plus = !plus;
  }
  if (kind >= 12) {
    y = -y;
    plus = !plus;
  }
  if (!plus) {
    z = -z;
  }

  const int val = (int)(dir) / 2 + 1;
  int base_val;
  bool base_plus;
  if (abs(x) == val) {
    base_val = 4;
    base_plus = x > 0;
  } else if (abs(y) == val) {
    base_val = 2;
    base_plus = y > 0;
  } else {
    base_val = 0;
    base_plus = z > 0;
  }

  return Dir(base_val + (base_plus ? (int)(dir) % 2 : 1 - (int)(dir) % 2));
}

struct Pos {
  int idx_;
  Pos() {}
  explicit Pos(const int _idx) : idx_(_idx) {}
  Pos(int _x, int _y, int _z) : idx_(_z * N * N + _y * N + _x) {
    assert(N > 0);
  }
  int X() const { return pos_2_x[*this]; }
  int Y() const { return pos_2_y[*this]; }
  int Z() const { return pos_2_z[*this]; }
  int Idx() const { return idx_; }
  operator int() const { return Idx(); }
  operator size_t() const { return Idx(); }
  const vector<Pos>& Adj() const { return adj_poses[*this]; }
  const vector<Dir>& AdjDirs() const { return adj_dirs[*this]; }
  int Manhattan(const Pos& other) const {
    return abs(X() - other.X()) + abs(Y() - other.Y()) + abs(Z() - other.Z());
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
    os << pos.X() << " " << pos.Y() << " " << pos.Z();
    return os;
  }
  bool IsDummy() const { return this->idx_ < 0; }
  static Pos Dummy() {
    Pos p;
    p.idx_ = -1;
    return p;
  }

  // kind: 0から23
  Pos Rotate(const int kind) const {
    int x = X();
    int y = Y();
    int z = Z();
    bool plus = true;
    if (kind % 6 == 0) {
    } else if (kind % 6 == 1) {
      swap(x, y);
      plus = !plus;
    } else if (kind % 6 == 2) {
      swap(x, z);
      plus = !plus;
    } else if (kind % 6 == 3) {
      swap(y, z);
      plus = !plus;
    } else if (kind % 6 == 4) {
      swap(x, y);
      swap(y, z);
    } else {
      swap(x, z);
      swap(y, z);
    }

    if (kind / 6 % 2 == 1) {
      x = N - x - 1;
      plus = !plus;
    }
    if (kind >= 12) {
      y = N - y - 1;
      plus = !plus;
    }
    if (!plus) {
      z = N - z - 1;
    }

    return Pos(x, y, z);
  }

  Pos InvRotate(const int kind) const {
    int x = X();
    int y = Y();
    int z = Z();
    bool plus = true;
    if (kind % 6 == 1 || kind % 6 == 2 || kind % 6 == 3) {
      plus = !plus;
    }

    if (kind >= 12) {
      y = N - y - 1;
      plus = !plus;
    }

    if (kind / 6 % 2 == 1) {
      x = N - x - 1;
      plus = !plus;
    }

    if (!plus) {
      z = N - z - 1;
    }

    if (kind % 6 == 0) {
    } else if (kind % 6 == 1) {
      swap(x, y);
    } else if (kind % 6 == 2) {
      swap(x, z);
    } else if (kind % 6 == 3) {
      swap(y, z);
    } else if (kind % 6 == 4) {
      swap(y, z);
      swap(x, y);
    } else {
      swap(y, z);
      swap(x, z);
    }

    return Pos(x, y, z);
  }

  static Pos TryCreate(const int x, const int y, const int z) {
    if (z < 0 || z >= N || y < 0 || y >= N || x < 0 || x >= N) {
      return Pos::Dummy();
    } else {
      return Pos(x, y, z);
    }
  }

  static void StaticInit(const int n) {
    N = n;
    N3 = N * N * N;
    N6 = N3 * N3;
    adj_poses = vector<vector<Pos>>(N * N * N);
    adj_dirs = vector<vector<Dir>>(N * N * N);
    move_to = vector<vector<Pos>>(6, vector<Pos>(N * N * N, Pos::Dummy()));
    pos_2_z.resize(N * N * N);
    pos_2_y.resize(N * N * N);
    pos_2_x.resize(N * N * N);

    rep(z, N) {
      rep(y, N) {
        rep(x, N) {
          const Pos p{x, y, z};
          pos_2_z[p] = z;
          pos_2_y[p] = y;
          pos_2_x[p] = x;
          for (int dz = -1; dz <= 1; ++dz) {
            for (int dy = -1; dy <= 1; ++dy) {
              for (int dx = -1; dx <= 1; ++dx) {
                if (abs(dz) + abs(dy) + abs(dx) != 1) continue;
                const int adj_z = z + dz;
                const int adj_y = y + dy;
                const int adj_x = x + dx;
                if (adj_z < 0 || adj_z >= N) continue;
                if (adj_y < 0 || adj_y >= N) continue;
                if (adj_x < 0 || adj_x >= N) continue;
                adj_poses[p].emplace_back(adj_x, adj_y, adj_z);

                Dir dir;
                if (dz == -1) {
                  dir = kU;
                } else if (dz == 1) {
                  dir = kD;
                } else if (dy == -1) {
                  dir = kB;
                } else if (dy == 1) {
                  dir = kF;
                } else if (dx == -1) {
                  dir = kL;
                } else if (dx == 1) {
                  dir = kR;
                } else {
                  assert(false);
                }

                move_to[dir][p] = {adj_x, adj_y, adj_z};
                adj_dirs[p].emplace_back(dir);
              }
            }
          }
        }
      }
    }

    rep(kind, 24) {
      kind_dir_2_rotated_dir.emplace_back();
      rep(dir_int, 6) {
        const auto rotated_dir = RotateDirForCreate(kind, Dir(dir_int));
        kind_dir_2_rotated_dir.back().emplace_back(rotated_dir);
      }
    }
  }
  static int N, N3, N6;
  static vector<vector<Pos>> adj_poses;
  static vector<vector<Dir>> adj_dirs;
  static vector<vector<Pos>> move_to;
  static vector<int> pos_2_x, pos_2_y, pos_2_z;
};

vector<vector<Pos>> Pos::adj_poses;
vector<vector<Dir>> Pos::adj_dirs;
vector<vector<Pos>> Pos::move_to;
vector<int> Pos::pos_2_x, Pos::pos_2_y, Pos::pos_2_z;
int Pos::N, Pos::N3, Pos::N6;












class Constraints {
 public:
  Constraints() {}
  Constraints(istream& is)
      : Fs(2),
        Rs(2),
        t2z2must_xs(2),
        t2z2must_ys(2),
        adj_valid_poses(2),
        valid_poses(2) {
    is >> N_;
    Pos::StaticInit(N_);
    rep(t, 2) {
      t2z2must_xs[t].resize(N_);
      t2z2must_ys[t].resize(N_);

      auto& F = Fs[t];
      rep(row, N_) {
        string s;
        is >> s;
        rep(i, s.size()) {
          const bool is_one = s[i] == '1';
          F.emplace_back(is_one);
          must_fr_count_ += is_one;
          if (is_one) {
            t2z2must_xs[t][row].emplace_back(i);
          }
        }
      }

      auto& R = Rs[t];
      rep(row, N_) {
        string s;
        is >> s;
        rep(i, s.size()) {
          const bool is_one = s[i] == '1';
          R.emplace_back(is_one);
          must_fr_count_ += is_one;
          if (is_one) {
            t2z2must_ys[t][row].emplace_back(i);
          }
        }
      }
    }

    rep(t, 2) {
      adj_valid_poses[t].resize(N_ * N_ * N_);
      rep(idx, N_ * N_ * N_) {
        const Pos p(idx);
        if (!CanBlock(t, p)) continue;

        valid_poses[t].emplace_back(p);
        for (const auto& adj_p : p.Adj()) {
          if (!CanBlock(t, adj_p)) continue;
          adj_valid_poses[t][p].emplace_back(adj_p);
        }
      }
    }
  }

  int N() { return N_; }

  bool CanBlock(const int id, const Pos& p) const {
    assert(id == 0 || id == 1);
    return Fs[id][p.Z() * N_ + p.X()] && Rs[id][p.Z() * N_ + p.Y()];
  }

  bool MustXZ(const int id, const int x, const int z) const {
    return Fs[id][z * N_ + x];
  }

  bool MustYZ(const int id, const int y, const int z) const {
    return Rs[id][z * N_ + y];
  }

  const vector<int>& MustXs(const int id, const int z) const {
    return t2z2must_xs[id][z];
  }

  const vector<int>& MustYs(const int id, const int z) const {
    return t2z2must_ys[id][z];
  }

  const vector<Pos>& ValidPoses(const int id) const { return valid_poses[id]; }

  const vector<Pos>& AdjValidPoses(const int id, const Pos& p) const {
    return adj_valid_poses[id][p];
  }

  int MustFRCount() const { return must_fr_count_; }

  int N_;
  vector<vector<bool>> Fs, Rs;
  vector<vector<vector<int>>> t2z2must_xs, t2z2must_ys;
  vector<vector<vector<Pos>>> adj_valid_poses;
  vector<vector<Pos>> valid_poses;
  int must_fr_count_ = 0;
};







/* start */

constexpr int kUseBSMaxN = 13;
using bitset_FR = bitset<kUseBSMaxN * kUseBSMaxN * 2>;

struct PosPair {
  int idx_;

  PosPair() {}

  explicit PosPair(const int idx) : idx_(idx) {}

  struct P0P1Kind {
    Pos p0;
    Pos p1;
    int kind;

    P0P1Kind(const Pos& _p0, const Pos& _p1, const int _kind)
        : p0(_p0), p1(_p1), kind(_kind) {}

    inline bool Move(const Dir dir, const Constraints& constraints) {
      const int N = Pos::N;
      const int N3 = N * N * N;
      const int N6 = N3 * N3;
      if (!p0.Move(dir) || !constraints.CanBlock(0, p0)) {
        return false;
      }

      const auto rotated_dir = RotateDir(kind, dir);
      if (!p1.Move(rotated_dir) || !constraints.CanBlock(1, p1)) {
        return false;
      }

      return true;
    }
  };

  int Idx() const { return idx_; }

  bool operator==(const PosPair& other) const { return idx_ == other.idx_; }

  static PosPair Create(const Pos& p0, const Pos& p1, const int kind) {
    return PosPair(
        kindp0p12idx[kind * Pos::N6 + p0.Idx() * Pos::N3 + p1.Idx()]);
  }

  static PosPair Dummy() { return PosPair{-1}; }
  bool IsDummy() const { return idx_ == -1; }

  static void StaticInit(const Constraints& _constraints) {
    constraints = _constraints;
    global_timers["PosPair::StaticInit"].start();
    idx2p0p1kind.clear();
    adjs.clear();

    const int N = Pos::N;
    const int N3 = N * N * N;
    const int N6 = N3 * N3;
    //[kind*N6 + p0*N3 + p1] = idx
    vector<Pos> p0s, p1s;
    rep(z0, N) {
      for (const int x0 : constraints.MustXs(0, z0)) {
        for (const int y0 : constraints.MustYs(0, z0)) {
          p0s.emplace_back(x0, y0, z0);
        }
      }
    }
    p0s.shrink_to_fit();

    rep(z1, N) {
      for (const int x1 : constraints.MustXs(1, z1)) {
        for (const int y1 : constraints.MustYs(1, z1)) {
          p1s.emplace_back(x1, y1, z1);
        }
      }
    }
    p1s.shrink_to_fit();

    idx2p0p1kind.reserve(24 * p0s.size() * p1s.size());
    int kind_n6 = 0;
    rep(kind, 24) {
      for (const auto& p0 : p0s) {
        for (const auto& p1 : p1s) {
          kindp0p12idx[kind_n6 + p0.Idx() * N3 + p1.Idx()] =
              idx2p0p1kind.size();
          idx2p0p1kind.emplace_back(p0, p1, kind);
        }
      }
      kind_n6 += N6;
    }
    idx2p0p1kind.shrink_to_fit();

    cerr << "PosPair count: " << idx2p0p1kind.size() << endl;

    adjs.resize(idx2p0p1kind.size());
    rep(idx, idx2p0p1kind.size()) {
      auto p0p1kind_base = idx2p0p1kind[idx];
      for (const auto dir : p0p1kind_base.p0.AdjDirs()) {
        auto p0p1kind = p0p1kind_base;
        const bool success = p0p1kind.Move(dir, constraints);
        if (!success) continue;

        const auto& [p0, p1, kind] = p0p1kind;
        const int moved_idx =
            kindp0p12idx[kind * N6 + p0.Idx() * N3 + p1.Idx()];
        adjs[idx].emplace_back(PosPair(moved_idx));
      }
    }

    global_timers["PosPair::StaticInit"].add();
  }

  const vector<PosPair>& Adj() const { return adjs[this->idx_]; }
  const Pos& P0() const { return idx2p0p1kind[idx_].p0; }
  const Pos& P1() const { return idx2p0p1kind[idx_].p1; }
  int RotateKind() const { return idx2p0p1kind[idx_].kind; }
  bitset_FR Bitset() const {
    const auto itr = idx2bs.find(this->Idx());
    if (itr != idx2bs.end()) {
      return itr->second;
    }

    const int N2 = Pos::N * Pos::N;

    if (tXYzxy2bsidx.size() == 0) {
      tXYzxy2bsidx.resize(N2 * 4);
      int idx = 0;
      rep(t, 2) {
        const int t_idx = t * N2 * 2;
        rep(z, Pos::N) {
          for (const auto x : constraints.MustXs(t, z)) {
            // t, (ZX:0, ZY:1), z, xy
            tXYzxy2bsidx[t_idx + z * Pos::N + x] = idx++;
          }
          for (const auto y : constraints.MustYs(t, z)) {
            tXYzxy2bsidx[t_idx + N2 + z * Pos::N + y] = idx++;
          }
        }
      }
    }

    global_timers["Bitset"].start();
    queue<PosPair> q;
    unordered_set<int> S;
    bitset_FR bs(0);

    q.push(*this);
    S.insert(q.front().Idx());
    const int t0zx_idx = 0;
    const int t0zy_idx = N2 * 1;
    const int t1zx_idx = N2 * 2;
    const int t1zy_idx = N2 * 3;
    while (q.size() > 0) {
      const auto pos_pair = q.front();
      q.pop();
      // t, (ZX:0, ZY:1), z, xy
      bs[tXYzxy2bsidx[t0zx_idx + pos_pair.P0().Z() * Pos::N +
                      pos_pair.P0().X()]] = true;
      bs[tXYzxy2bsidx[t0zy_idx + pos_pair.P0().Z() * Pos::N +
                      pos_pair.P0().Y()]] = true;
      bs[tXYzxy2bsidx[t1zx_idx + pos_pair.P1().Z() * Pos::N +
                      pos_pair.P1().X()]] = true;
      bs[tXYzxy2bsidx[t1zy_idx + pos_pair.P1().Z() * Pos::N +
                      pos_pair.P1().Y()]] = true;

      for (const auto& adj : pos_pair.Adj()) {
        if (S.count(adj.Idx()) > 0) continue;
        q.push(adj);
        S.insert(adj.Idx());
      }
    }

    for (const auto pp_idx : S) {
      idx2bs[pp_idx] = bs;
    }

    global_timers["Bitset"].add();

    return bs;
  }

  // [idx] = (pos0, pos1, rotate_kind)
  static vector<P0P1Kind> idx2p0p1kind;
  static vector<vector<PosPair>> adjs;
  static int kindp0p12idx[14 * 14 * 14 * 14 * 14 * 14 * 24];
  static unordered_map<int, bitset_FR> idx2bs;
  static Constraints constraints;
  static vector<int> tXYzxy2bsidx;
};

int PosPair::kindp0p12idx[14 * 14 * 14 * 14 * 14 * 14 * 24];
vector<PosPair::P0P1Kind> PosPair::idx2p0p1kind;
vector<vector<PosPair>> PosPair::adjs;
unordered_map<int, bitset_FR> PosPair::idx2bs;
Constraints PosPair::constraints;
vector<int> PosPair::tXYzxy2bsidx;

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

struct Solution {
  vector<vector<int>> blocks;
  set<int> kind_S;
  int N;
  Solution(const int N_) : blocks(2, vector<int>(N_ * N_ * N_, -1)), N(N_) {}
  void AddBlock(const int id, const int kind, const vector<Pos>& poses) {
    kind_S.insert(kind);
    for (const auto& pos : poses) {
      blocks[id][pos] = kind;
    }
  }

  Solution& FillInTheEmpty(const Constraints& constraints) {
    const int useable_kind = kind_S.size() > 0 ? (*kind_S.rbegin()) + 1 : 0;
    vector<int> next_kinds(2, useable_kind);
    rep(t, 2) {
      int& next_kind = next_kinds[t];
      rep(z, N) {
        vector<int> Xs, Ys;
        int can_x = 0, can_y = 0;
        rep(x, N) {
          if (!constraints.MustXZ(t, x, z)) {
            continue;
          }

          can_x = x;

          bool exists = false;
          rep(y, N) {
            const Pos p(x, y, z);
            if (blocks[t][p] != -1) {
              exists = true;
              break;
            }
          }
          if (exists) continue;

          Xs.emplace_back(x);
        }

        rep(y, N) {
          if (!constraints.MustYZ(t, y, z)) {
            continue;
          }

          can_y = y;

          bool exists = false;
          rep(x, N) {
            const Pos p(x, y, z);
            if (blocks[t][p] != -1) {
              exists = true;
              break;
            }
          }
          if (exists) continue;

          Ys.emplace_back(y);
        }

        rep(i, max(Xs.size(), Ys.size())) {
          const int x = (i < Xs.size() ? Xs[i] : can_x);
          const int y = (i < Ys.size() ? Ys[i] : can_y);

          kind_S.insert(next_kind);
          blocks[t][Pos(x, y, z)] = next_kind++;
        }
      }
    }

    const int t = (next_kinds[0] < next_kinds[1] ? 0 : 1);
    [&]() {
      rep(x, N) {
        rep(y, N) {
          rep(z, N) {
            const Pos p(x, y, z);
            if (blocks[t][p] != -1) continue;
            if (!constraints.CanBlock(t, p)) continue;
            if (next_kinds[0] == next_kinds[1]) {
              return;
            }
            blocks[t][p] = next_kinds[t]++;
          }
        }
      }
    }();

    return *this;
  }

  friend ostream& operator<<(ostream& os, const Solution& src) {
    const int n = src.kind_S.size();
    os << n;
    for (const auto& block : src.blocks) {
      os << "\n";
      rep(x, src.N) {
        rep(y, src.N) {
          rep(z, src.N) {
            Pos p{x, y, z};
            os << block[p] + 1 << " ";
          }
        }
      }
    }
#ifdef LOCAL
    rep(t, 2) {
      dsu d(Pos::N3);
      rep(idx, Pos::N3) {
        Pos p(idx);
        if (src.blocks[t][p] == -1) continue;
        for (const auto& adj : p.Adj()) {
          if (src.blocks[t][p] == src.blocks[t][adj]) {
            d.merge(p, adj);
          }
        }
      }

      set<int> S;
      auto groups = d.groups();
      for (const auto& group : groups) {
        const int kind = src.blocks[t][group[0]];
        if (kind == -1) continue;

        assert(S.count(kind) == 0);
        S.insert(kind);
        for (const auto idx : group) {
          assert(kind == src.blocks[t][idx]);
        }
      }
    }
#endif
    return os;
  }
};

struct BoardLight {
  BoardLight(const Constraints& constraints) {
    initial_fr_count = constraints.MustFRCount();
  };

  struct KindPosPair {
    int kind;
    PosPair pos_pair;

    KindPosPair(const int _kind, const PosPair& _pos_pair)
        : kind(_kind), pos_pair(_pos_pair) {}
  };

  struct Data {
    struct EvalScore {
      double eval = 0.0;
      double score = 0.0;

      void Reset() {
        eval = 0.0;
        score = 0.0;
      }

      void ChangeZXYCount(const int prev_count, const int next_count) {
        if (prev_count == 0) {
          score -= 1.0;
          eval -= PARAMS[0];
        }
        if (next_count == 0) {
          score += 1.0;
          eval += PARAMS[0];
        }
      }

      double Eval() const { return eval; }

      double Score() const { return score; }
    };

    vector<int> kinds;
    vector<int> zx_counts;
    vector<int> zy_counts;
    EvalScore eval_score;

    void Put(const int kind, const Pos& p) {
      kinds[p.Idx()] = kind;
      const int old_zx_count = zx_counts[p.Z() * Pos::N + p.X()]++;
      eval_score.ChangeZXYCount(old_zx_count, old_zx_count + 1);
      const int old_zy_count = zy_counts[p.Z() * Pos::N + p.Y()]++;
      eval_score.ChangeZXYCount(old_zy_count, old_zy_count + 1);
    }

    void Erase(const Pos& p) {
      kinds[p.Idx()] = -1;
      const int old_zx_count = zx_counts[p.Z() * Pos::N + p.X()]--;
      eval_score.ChangeZXYCount(old_zx_count, old_zx_count - 1);
      const int old_zy_count = zy_counts[p.Z() * Pos::N + p.Y()]--;
      eval_score.ChangeZXYCount(old_zy_count, old_zy_count - 1);
    }

    bool IsOnlyPosAboutZXY(const Pos& p) const {
      return zx_counts[p.Z() * Pos::N + p.X()] == 1 ||
             zy_counts[p.Z() * Pos::N + p.Y()] == 1;
    }

    void Reset() {
      eval_score.Reset();
      kinds.clear();
      kinds.resize(Pos::N3, -1);
      zx_counts.clear();
      zx_counts.resize(Pos::N * Pos::N);
      zy_counts.clear();
      zy_counts.resize(Pos::N * Pos::N);
    }

    bool Empty(const Pos& p) const { return kinds[p] == -1; }
    bool Exists(const Pos& p) const { return kinds[p] != -1; }

    double Eval() { return eval_score.Eval(); }
    double Score() { return eval_score.Score(); }

    pair<int, int> ZXYCounts(const Pos& p) const {
      return std::make_pair(zx_counts[p.Z() * Pos::N + p.X()],
                            zy_counts[p.Z() * Pos::N + p.Y()]);
    }
  };

  static int initial_fr_count;

  unordered_map<int, vector<KindPosPair>> kind2kpps;
  vector<PosPair> pos_pairs;
  Data datas0, datas1;
  bool should_update_board = false;
  double memo_eval = INF;
  double memo_score = INF;
  int exist_fr_count = -1;

  bool operator==(const BoardLight& other) const {
    return pos_pairs == other.pos_pairs;
  }

  bool operator!=(const BoardLight& other) const {
    return pos_pairs != other.pos_pairs;
  }

  const vector<PosPair>& PosPairs() const { return pos_pairs; }

  int GetIdxOfPP(const PosPair& pp) const {
    rep(i, pos_pairs.size()) {
      if (pp == pos_pairs[i]) {
        return i;
      }
    }
    assert(false);
    return -1;
  }

  void SwapOrder(const int idx0, const int idx1) {
    should_update_board = true;
    memo_eval = INF;
    memo_score = INF;
    std::swap(pos_pairs[idx0], pos_pairs[idx1]);
  }

  void EmplaceBack(const PosPair& pp) {
    should_update_board = true;
    memo_eval = INF;
    memo_score = INF;
    exist_fr_count = -1;
    pos_pairs.emplace_back(pp);
  }

  void Insert(const int idx, const PosPair& pp) {
    should_update_board = true;
    memo_eval = INF;
    memo_score = INF;
    exist_fr_count = -1;
    pos_pairs.insert(pos_pairs.begin() + idx, pp);
  }

  int Erase(const PosPair& pp) {
    should_update_board = true;
    memo_eval = INF;
    memo_score = INF;
    exist_fr_count = -1;
    const auto itr = find(all(pos_pairs), pp);
    assert(itr != pos_pairs.end());
    const int idx = itr - pos_pairs.begin();
    pos_pairs.erase(itr);
    return idx;
  }

  bool CanAdd(const PosPair& pp) {
    for (const auto& other_pp : pos_pairs) {
      if (pp.P0() == other_pp.P0() || pp.P1() == other_pp.P1()) return false;
    }
    return true;
  }

  Solution Convert() {
    GenBoard();
    const int N = Pos::N;
    Solution sol(N);

    int id = 0;
    for (const auto& [kind, kpps] : kind2kpps) {
      vector<Pos> poses0, poses1;
      for (const auto& kpp : kpps) {
        poses0.emplace_back(kpp.pos_pair.P0());
        poses1.emplace_back(kpp.pos_pair.P1());
      }
      sol.AddBlock(0, id, poses0);
      sol.AddBlock(1, id, poses1);
      id++;
    }
    return sol;
  }

  int ExistFRCount() {
    if (exist_fr_count != -1) {
      return exist_fr_count;
    }
    global_timers["ExistFRCount"].start();
    bitset_FR bs(0);
    for (const auto& pp : pos_pairs) {
      bs |= pp.Bitset();
    }
    exist_fr_count = bs.count();
    global_timers["ExistFRCount"].add();
    return exist_fr_count;
  }

  void SetMemoEval(const double val) { memo_eval = val; }
  void SetMemoScore(const double val) { memo_score = val; }

  double Eval() {
    if (memo_eval != INF) {
      return memo_eval;
    }
    if (Pos::N <= kUseBSMaxN) {
      const int exist_fr_count = ExistFRCount();
      if (exist_fr_count != initial_fr_count) {
        assert(exist_fr_count < initial_fr_count);
        return (initial_fr_count - exist_fr_count) * 100000;
      }
    }
    GenBoard();
    double ret = 0.0;
    ret += datas0.Eval();
    ret += datas1.Eval();
    for (const auto& [kind, kpps] : kind2kpps) {
      ret += PARAMS[1] / kpps.size() + PARAMS[2];
    }
    ret += pos_pairs.size() * PARAMS[3];
    const double eval = ret + initial_fr_count;
    memo_eval = eval_weight * eval + (1.0 - eval_weight) * Score();
    return memo_eval;
  }

  double Score() {
    if (memo_score != INF) {
      return memo_score;
    }
    GenBoard();
    double ret = 0.0;
    ret += datas0.Score();
    ret += datas1.Score();
    for (const auto& [kind, kpps] : kind2kpps) {
      ret += 1.0 / kpps.size();
    }
    memo_score = ret + initial_fr_count;
    return memo_score;
  }

  void ExpandBlockNecessary(const int kind, vector<PosPair>& removed_pps) {
    const auto& pos_pair = pos_pairs[kind];
    if (datas0.Exists(pos_pair.P0())) return;
    if (datas1.Exists(pos_pair.P1())) return;

    global_timers["First"].start();
    queue<int> q;
    vector<int> parents;
    vector<int> child_counts;
    parents.reserve(Pos::N3 / 2 + 1);
    child_counts.reserve(Pos::N3 / 2 + 1);

    auto& kpps = kind2kpps[kind];
    parents.clear();
    child_counts.clear();

    kpps.emplace_back(kind, pos_pair);
    q.push(0);
    parents.emplace_back(-1);
    child_counts.emplace_back(0);
    datas0.Put(kind, pos_pair.P0());
    datas1.Put(kind, pos_pair.P1());

    while (q.size() > 0) {
      const int parent_idx = q.front();
      const auto kpp = kpps[parent_idx];
      q.pop();

      for (const auto& adj : kpp.pos_pair.Adj()) {
        if (datas0.Exists(adj.P0())) continue;
        if (datas1.Exists(adj.P1())) continue;

        datas0.Put(kpp.kind, adj.P0());
        datas1.Put(kpp.kind, adj.P1());
        parents.emplace_back(parent_idx);
        child_counts.emplace_back(0);
        q.push(kpps.size());
        kpps.emplace_back(kpp.kind, adj);

        child_counts[parent_idx]++;
      }
    }

    global_timers["First"].add();
    global_timers["Second"].start();

    int should_remove_count = 0;
    REP(i, (int)kpps.size()) {
      // ZX_count, ZY_countに寄与せず、葉であるなら削除する
      const auto& pp = kpps[i].pos_pair;
      if (child_counts[i] > 0) continue;
      if (datas0.IsOnlyPosAboutZXY(pp.P0()) ||
          datas1.IsOnlyPosAboutZXY(pp.P1())) {
        continue;
      }

      // 破壊したという目印
      child_counts[i] = -1;
      datas0.Erase(pp.P0());
      datas1.Erase(pp.P1());
      const int parent_idx = parents[i];
      if (parent_idx != -1) {
        child_counts[parent_idx]--;
        should_remove_count++;
      }
    }

    global_timers["Second"].add();
    global_timers["Third"].start();

    {
      std::vector<KindPosPair> next_kpps;
      next_kpps.reserve((int)kpps.size() - should_remove_count);
      removed_pps.reserve(should_remove_count);
      rep(i, kpps.size()) {
        if (child_counts[i] != -1) {
          next_kpps.emplace_back(kpps[i]);
        } else if (parents[i] != -1 && child_counts[parents[i]] != -1) {
          // 根ではなく、親が破壊されていないnodeだけ追加すれば十分
          // この追加されたnodeは、隣接nodeが存在(=親)することが保証される
          removed_pps.emplace_back(kpps[i].pos_pair);
        }
      }

      kpps = std::move(next_kpps);
    }

    if (kpps.size() == 0) {
      kind2kpps.erase(kind);
    }
    global_timers["Third"].add();
  }

  void ExpandMinimumBlocks(
      unordered_map<int, vector<PosPair>>& kind2removed_pps) {
    // 最後に全ブロックを少しずつ大きくしていく
    vector<int> next_idxes(kind2removed_pps.size(), 0);
    // 小さいblockから少しずつ拡大していく
    // -block_size, kind
    priority_queue<pair<int, int>> pq;
    rep(kind, pos_pairs.size()) {
      const auto itr = kind2kpps.find(kind);
      if (itr == kind2kpps.end()) continue;

      const auto& kpps = itr->second;
      pq.emplace(-(int)kpps.size(), kind);
    }

    while (pq.size() > 0) {
      const auto [block_size_minus, kind] = pq.top();
      pq.pop();

      auto& removed_pps = kind2removed_pps[kind];
      int& next_idx = next_idxes[kind];
      if (next_idx >= removed_pps.size()) continue;

      // removed_ppを追加したい
      const auto& removed_pp = removed_pps[next_idx++];
      if (datas0.Exists(removed_pp.P0()) || datas1.Exists(removed_pp.P1())) {
        // 被ってしまったのでそのままpqに戻す
        pq.emplace(block_size_minus, kind);
        continue;
      }

      // 追加
      auto& kpps = kind2kpps[kind];
      kpps.emplace_back(kind, removed_pp);
      datas0.Put(kind, removed_pp.P0());
      datas1.Put(kind, removed_pp.P1());

      for (const auto& adj : removed_pp.Adj()) {
        // 周辺を候補として追加
        if (datas0.Exists(adj.P0())) continue;
        if (datas1.Exists(adj.P1())) continue;

        removed_pps.emplace_back(adj);
      }

      // sizeが1大きくなった分を反映して、pqに戻す
      pq.emplace(block_size_minus - 1, kind);
    }
  }

  void GenBoard() {
    if (!should_update_board) {
      return;
    }
    global_timers["GenBoard"].start();
    should_update_board = false;
    kind2kpps.clear();
    datas0.Reset();
    datas1.Reset();

    // 1周目は、ZX_count,ZY_countに寄与しない葉は削除して、pqに入れておく
    // 最後に少しずつ伸ばしていく
    unordered_map<int, vector<PosPair>> kind2removed_pps;

    rep(kind, pos_pairs.size()) {
      ExpandBlockNecessary(kind, kind2removed_pps[kind]);
    }

    ExpandMinimumBlocks(kind2removed_pps);
    global_timers["GenBoard"].add();
  }

  static double eval_weight;
};
int BoardLight::initial_fr_count = 0;
double BoardLight::eval_weight = 1.0;





/* start */

namespace for_debug {

vector<int> gs;
vector<string> nbd_msgs;
vector<BoardLight> prev_boards;
vector<BoardLight> next_boards;
string prev_nbd_for_debug = "";
inline void SetPrevNBDForDebug(string&& msg) {
#ifdef PRINT_DEBUG
  prev_nbd_for_debug = std::move(msg);
#endif
}

}  // namespace for_debug

struct NBD {
  double Diff() const { return diff_; }
  void Update(BoardLight*) const {}
  virtual void Restore(BoardLight*) const = 0;

  double diff_;
  double minus_prev_eval_;
  double prev_score_;
};

struct NBD_insert : public NBD {
  NBD_insert() {}
  NBD_insert(BoardLight* const board, const int idx, const PosPair pp) {
    diff_ = minus_prev_eval_ = -board->Eval();
    prev_score_ = board->Score();
    board->Insert(idx, pp);
    added_pp_ = pp;
    diff_ += board->Eval();
    for_debug::SetPrevNBDForDebug("NBD_insert");
  }
  void Restore(BoardLight* board) const override {
    board->Erase(added_pp_);
    board->SetMemoEval(-minus_prev_eval_);
    board->SetMemoScore(prev_score_);
  }

  PosPair added_pp_;
};

struct NBD_erase : public NBD {
  NBD_erase() {}
  NBD_erase(BoardLight* const board, const PosPair pp) {
    diff_ = minus_prev_eval_ = -board->Eval();
    prev_score_ = board->Score();
    idx_ = board->Erase(pp);
    erased_pp_ = pp;
    diff_ += board->Eval();
    for_debug::SetPrevNBDForDebug("NBD_erase");
  }
  void Restore(BoardLight* board) const override {
    board->Insert(idx_, erased_pp_);
    board->SetMemoEval(-minus_prev_eval_);
    board->SetMemoScore(prev_score_);
  }

  int idx_;
  PosPair erased_pp_;
};

struct NBD_erase_insert : public NBD {
  NBD_erase_insert() {}
  NBD_erase_insert(BoardLight* const board, const PosPair erase_pp,
                   const PosPair add_pp) {
    diff_ = minus_prev_eval_ = -board->Eval();
    prev_score_ = board->Score();
    idx_ = board->Erase(erase_pp);
    erased_pp_ = erase_pp;
    board->Insert(idx_, add_pp);
    added_pp_ = add_pp;
    diff_ += board->Eval();
    for_debug::SetPrevNBDForDebug("NBD_erase_insert");
  }
  void Restore(BoardLight* board) const override {
    board->Erase(added_pp_);
    board->Insert(idx_, erased_pp_);
    board->SetMemoEval(-minus_prev_eval_);
    board->SetMemoScore(prev_score_);
  }

  int idx_;
  PosPair erased_pp_;
  PosPair added_pp_;
};

struct NBD_order_swap : public NBD {
  NBD_order_swap() {}
  NBD_order_swap(BoardLight* const board, const int idx0, const int idx1) {
    diff_ = minus_prev_eval_ = -board->Eval();
    prev_score_ = board->Score();
    board->SwapOrder(idx0, idx1);
    idx0_ = idx0;
    idx1_ = idx1;
    diff_ += board->Eval();
    for_debug::SetPrevNBDForDebug("NBD_order_swap");
  }
  void Restore(BoardLight* board) const override {
    board->SwapOrder(idx0_, idx1_);
    board->SetMemoEval(-minus_prev_eval_);
    board->SetMemoScore(prev_score_);
  }

  int idx0_, idx1_;
};

struct NBD_change_p0_or_p1 : public NBD {
  NBD_change_p0_or_p1() {}
  NBD_change_p0_or_p1(BoardLight* const board, const PosPair& old_pp,
                      const PosPair& new_pp) {
    diff_ = minus_prev_eval_ = -board->Eval();
    prev_score_ = board->Score();
    erased_pp_ = old_pp;
    added_pp_ = new_pp;
    idx_ = board->Erase(old_pp);
    board->Insert(idx_, new_pp);
    diff_ += board->Eval();
    for_debug::SetPrevNBDForDebug("NBD_change_p0_or_p1");
  }
  void Restore(BoardLight* board) const override {
    board->Erase(added_pp_);
    board->Insert(idx_, erased_pp_);
    board->SetMemoEval(-minus_prev_eval_);
    board->SetMemoScore(prev_score_);
  }

  int idx_;
  PosPair erased_pp_;
  PosPair added_pp_;
};

struct NBD_p0p1_swap : public NBD {
  NBD_p0p1_swap() {}
  NBD_p0p1_swap(BoardLight* const board, const PosPair pp0, const PosPair pp1,
                const int rotate0, const int rotate1) {
    diff_ = minus_prev_eval_ = -board->Eval();
    prev_score_ = board->Score();
    erased_pp0_ = pp0;
    erased_pp1_ = pp1;
    idx0_ = board->Erase(pp0);
    idx1_ = board->Erase(pp1);
    added_pp1_ = PosPair::Create(pp0.P0(), pp1.P1(), rotate1);
    board->Insert(idx1_, added_pp1_);
    added_pp0_ = PosPair::Create(pp1.P0(), pp0.P1(), rotate0);
    board->Insert(idx0_, added_pp0_);
    diff_ += board->Eval();
    for_debug::SetPrevNBDForDebug("NBD_p0p1_swap");
  }
  void Restore(BoardLight* board) const override {
    board->Erase(added_pp0_);
    board->Erase(added_pp1_);
    board->Insert(idx1_, erased_pp1_);
    board->Insert(idx0_, erased_pp0_);
    board->SetMemoEval(-minus_prev_eval_);
    board->SetMemoScore(prev_score_);
  }

  int idx0_, idx1_;
  PosPair erased_pp0_, erased_pp1_;
  PosPair added_pp0_, added_pp1_;
};

struct NBD_rotate_and_translate : public NBD {
  NBD_rotate_and_translate() {}
  NBD_rotate_and_translate(BoardLight* const board, const PosPair& old_pp,
                           const PosPair& next_pp) {
    diff_ = minus_prev_eval_ = -board->Eval();
    prev_score_ = board->Score();
    erased_pp_ = old_pp;
    added_pp_ = next_pp;
    idx_ = board->Erase(old_pp);
    board->Insert(idx_, next_pp);
    diff_ += board->Eval();
    for_debug::SetPrevNBDForDebug("NBD_rotate_and_translate");
  }
  void Restore(BoardLight* board) const override {
    board->Erase(added_pp_);
    board->Insert(idx_, erased_pp_);
    board->SetMemoEval(-minus_prev_eval_);
    board->SetMemoScore(prev_score_);
  }

  int idx_;
  PosPair erased_pp_;
  PosPair added_pp_;
};

class NBDGenerator {
 public:
  static vector<int> probs;
  NBDGenerator(Constraints* constraints, const int time_limit)
      : constraints_(constraints),
        best_board_(*constraints),
        time_limit_(time_limit) {
    timer_.start();
    double sum = 0.0;
    for (int i = 4; i <= 10; ++i) {
      sum += PARAMS[i];
    }
    int sum_int = 0;
    for (int i = 4; i <= 10; ++i) {
      probs.emplace_back(PARAMS[i] / sum * 100);
      sum_int += probs.back();
    }

    while (sum_int > 100) {
      sum_int--;
      probs.back()--;
    }
    while (sum_int < 100) {
      sum_int++;
      probs[0]++;
    }

    rep(i, probs.size()) {
      if (i == 0) continue;
      probs[i] += probs[i - 1];
    }
  }
  NBD* Generate(BoardLight* const board, const int g) {
    if (g == 0) {
      best_board_ = *board;
    }

    constexpr double kPercent = 0.9;
    constexpr double kEps = 1e-6;
    if (board->Score() + kEps < best_board_.Score()) {
#ifdef PRINT_DEBUG
      cerr << g << " " << best_board_.Score() << " " << board->Score() << " "
           << for_debug::prev_nbd_for_debug << endl;
      if (board->Score() / best_board_.Score() < kPercent) {
        for_debug::gs.emplace_back(g);
        for_debug::nbd_msgs.emplace_back(for_debug::prev_nbd_for_debug);
        for_debug::prev_boards.emplace_back(best_board_);
        for_debug::next_boards.emplace_back(*board);
      }
#endif
      best_board_ = *board;
    }

    if ((g & 0xff) == 0) {
      const double kStartPercent = PARAMS[16];
      const double kEndPercent = PARAMS[16] + PARAMS[17];
      BoardLight::eval_weight =
          1.0 -
          std::clamp(((double)timer_.ms() - (time_limit_ * kStartPercent)) /
                         (time_limit_ * (kEndPercent - kStartPercent)),
                     0.0, 1.0);
      board->SetMemoEval(INF);
      board->Eval();
    }

    int debug = 0;
    while (true) {
      if (debug >= 10000) {
        cerr << "eval: 10000000" << endl;
        quick_exit(0);
      }
      debug++;
      const int r = rand(0, 100);
      if (r < probs[0]) {
        // Insert
        const Pos p0 = rand_vec(constraints_->ValidPoses(0));
        const Pos p1 = rand_vec(constraints_->ValidPoses(1));
        const int rotate = rand(0, 24);
        const auto pp = PosPair::Create(p0, p1, rotate);
        if (!board->CanAdd(pp)) continue;
        if (Pos::N <= kUseBSMaxN &&
            pp.Bitset().count() <=
                max(PARAMS[13], constraints_->MustFRCount() * PARAMS[14]))
          continue;
        const int idx = rand(0, board->PosPairs().size() + 1);
        nbd_insert0 = NBD_insert(board, idx, pp);
        return &nbd_insert0;
      } else if (r < probs[1]) {
        // Erase
        const auto& pps = board->PosPairs();
        if (pps.size() == 0) continue;
        const auto& pp = rand_vec(pps);
        nbd_erase0 = NBD_erase(board, pp);
        return &nbd_erase0;
      } else if (r < probs[2]) {
        // Erase and Add
        const auto& pps = board->PosPairs();
        if (pps.size() == 0) continue;

        const auto& erase_pp = rand_vec(pps);

        const Pos p0 = rand_vec(constraints_->ValidPoses(0));
        const Pos p1 = rand_vec(constraints_->ValidPoses(1));
        const int rotate = rand(0, 24);
        const auto add_pp = PosPair::Create(p0, p1, rotate);
        if (!board->CanAdd(add_pp)) continue;
        if (Pos::N <= kUseBSMaxN &&
            add_pp.Bitset().count() <=
                max(PARAMS[13], constraints_->MustFRCount() * PARAMS[14]))
          continue;

        nbd_erase_insert0 = NBD_erase_insert(board, erase_pp, add_pp);
        return &nbd_erase_insert0;
      } else if (r < probs[3]) {
        // Change p0 or p1
        const auto& pps = board->PosPairs();
        if (pps.size() == 0) continue;

        const auto& erase_pp = rand_vec(pps);
        PosPair add_pp;
        if (rand(0, 100) < PARAMS[15]) {
          add_pp = PosPair::Create(erase_pp.P0(),
                                   rand_vec(constraints_->ValidPoses(1)),
                                   rand(0, 24));
        } else {
          add_pp = PosPair::Create(rand_vec(constraints_->ValidPoses(0)),
                                   erase_pp.P1(), rand(0, 24));
        }
        if (Pos::N <= kUseBSMaxN &&
            add_pp.Bitset().count() <=
                max(PARAMS[13], constraints_->MustFRCount() * PARAMS[14]))
          continue;

        nbd_change_p0_or_p1 = NBD_change_p0_or_p1(board, erase_pp, add_pp);
        return &nbd_change_p0_or_p1;
      } else if (r < probs[4]) {
        // P0P1 swap
        const auto& pps = board->PosPairs();
        if (pps.size() <= 1) continue;

        const int idx0 = rand(0, (int)pps.size() - 1);
        const int idx1 = idx0 + 1;
        const auto& pp0 = pps[idx0];
        const auto& pp1 = pps[idx1];

        nbd_p0p1_swap0 =
            NBD_p0p1_swap(board, pp0, pp1, rand(0, 24), rand(0, 24));
        return &nbd_p0p1_swap0;
      } else if (r < probs[5]) {
        // Order swap
        const auto& pps = board->PosPairs();
        if (pps.size() <= 1) continue;

        const int idx0 = rand(0, (int)pps.size() - 1);
        const int idx1 = idx0 + 1;
        nbd_order_swap0 = NBD_order_swap(board, idx0, idx1);
        return &nbd_order_swap0;
      } else {
        // rotate and translate
        const auto& pps = board->PosPairs();
        if (pps.size() == 0) continue;
        const auto& old_pp = rand_vec(pps);

        bool change_rotate = false;
        bool change_translate = false;
        if (rand(0, 100) < PARAMS[11]) {
          change_rotate = change_translate = true;
        } else {
          if (rand(0, 100) < PARAMS[12]) {
            change_rotate = true;
          } else {
            change_translate = true;
          }
        }

        auto next_pp = old_pp;
        if (change_rotate) {
          // まずは回転を変える
          next_pp = PosPair::Create(next_pp.P0(), next_pp.P1(), rand(0, 24));
        }

        if (change_translate) {
          // 次に、平行移動してずらす
          const auto& adjs = next_pp.Adj();
          if (adjs.size() == 0) {
            // サイズ1のブロックを作ってもしょうがないので、continue
            continue;
          }
          const auto& adj_pp = rand_vec(adjs);
          next_pp =
              PosPair::Create(adj_pp.P0(), adj_pp.P1(), next_pp.RotateKind());
        }
        if (Pos::N <= kUseBSMaxN &&
            next_pp.Bitset().count() <=
                max(PARAMS[13], constraints_->MustFRCount() * PARAMS[14]))
          continue;

        nbd_rotate_and_translate0 =
            NBD_rotate_and_translate(board, old_pp, next_pp);
        return &nbd_rotate_and_translate0;
      }
    }
  }

  BoardLight BestSol() { return best_board_; }

 private:
  Constraints* constraints_;
  BoardLight best_board_;
  NBD_insert nbd_insert0;
  NBD_erase nbd_erase0;
  NBD_erase_insert nbd_erase_insert0;
  NBD_order_swap nbd_order_swap0;
  NBD_p0p1_swap nbd_p0p1_swap0;
  NBD_rotate_and_translate nbd_rotate_and_translate0;
  NBD_change_p0_or_p1 nbd_change_p0_or_p1;
  int time_limit_;
  Timer timer_;
};

vector<int> NBDGenerator::probs;

class Solver {
 public:
  explicit Solver(istream& is) : Solver(Constraints(is)) {}
  explicit Solver(const Constraints& constraints) : constraints_(constraints) {
    PosPair::StaticInit(constraints);
  }
  Solution Solve(int time_limit) {
    Timer timer;
    timer.start();

    const int N = constraints_.N();
    NBDGenerator nbd_generator(&constraints_, time_limit);
    BoardLight initial_board(constraints_);
    time_limit -= timer.ms();

    SimulatedAnnealing<BoardLight, NBDGenerator> sa(time_limit, PARAMS[18],
                                                    PARAMS[19]);
    auto best_board = sa.run(initial_board, nbd_generator);

    auto best_sol = best_board.Convert();
    best_sol.FillInTheEmpty(constraints_);
    cerr << global_timers << endl;
    cerr << timer.ms() << endl;
    cerr << "score: " << best_board.Score() << endl;

#ifdef PRINT_DEBUG
    rep(i, for_debug::prev_boards.size()) {
      const int g = for_debug::gs[i];
      auto& prev_board = for_debug::prev_boards[i];
      auto& next_board = for_debug::next_boards[i];
      const auto& nbd = for_debug::nbd_msgs[i];
      cout << endl << endl << endl;
      cout << g << " " << prev_board.Score() << " " << next_board.Score()
           << endl;
      cout << nbd << endl;
      cout << endl << prev_board.Convert().FillInTheEmpty(constraints_) << endl;
      cout << endl << next_board.Convert().FillInTheEmpty(constraints_) << endl;
    }
#endif

    cout << best_sol << endl;
    quick_exit(0);
    return best_sol;
  }

 private:
  Constraints constraints_;
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
  Solver solver(cin);
  auto sol = solver.Solve(5850 - timer.ms());
  cout << sol << endl;
  return 0;
}