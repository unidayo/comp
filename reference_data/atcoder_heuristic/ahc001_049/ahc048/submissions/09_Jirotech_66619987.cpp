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
// #define ll long long int
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define reps(i, n) for (int i = 1; i <= (int)(n); i++)
#define REP(i, n) for (int i = n - 1; i >= 0; i--)
#define REPS(i, n) for (int i = n; i > 0; i--)
// #define MOD (long long int)(1e9 + 7)
#define INF (int)(1e9)
#define LINF (long long int)(1e18)
#define ALL(v) v.begin(), v.end()
typedef pair<int, int> Pii;
typedef pair<long long int, long long int> Pll;
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
  template <typename ID>
  bool IsConnectedAfterRemoving(const vector<ID>& ids) const {
    // 自身を削除してもids[p]の連結性が維持されるか3x3を見て判定
    bitset<8> bs = 0;
    int adj_count = 0;
    int i = -1;
    const auto& p = *this;
    const int base_id = ids[p];
    for (const auto adj : p.Adj()) {
      i++;
      if (ids[adj] == base_id) {
        adj_count++;
        bs[i] = true;
      }
    }
    if (adj_count <= 1) return true;

    i = 3;
    for (const auto adjadj : adjadjs_check_remove[p]) {
      i++;
      bs[i] = ids[adjadj] == base_id;
    }
    return can_remove_bses[p][bs.to_ulong()];
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

    can_remove_bses.resize(N2);
    adjadjs_check_remove.resize(N2);
    rep(idx, N * N) {
      const Pos p(idx);
      set<Pos> S;
      for (const auto adj : p.Adj()) {
        for (const auto adjadj : adj.Adj()) {
          if (adjadj == p) continue;
          if (S.count(adjadj) > 0) {
            adjadjs_check_remove[p].push_back(adjadj);
          } else {
            S.insert(adjadj);
          }
        }
      }
      assert(adjadjs_check_remove[p].size() <= 4);
      rep(bit, 256) {
        // adj0, adj1, ..., adjadj0, adjdadj1, ... を詰めずに並べる
        bitset<8> bs(bit);
        set<Pos> adjs, adjadjs;
        rep(j, p.Adj().size()) {
          if (bs[j]) {
            adjs.insert(p.Adj()[j]);
          }
        }
        rep(j, adjadjs_check_remove[p].size()) {
          const auto adjadj = adjadjs_check_remove[p][j];
          if (bs[j + 4]) {
            int count = 0;
            for (const auto adjadjadj : adjadj.Adj()) {
              count += adjs.count(adjadjadj);
            }
            if (count >= 2) {
              adjadjs.insert(adjadj);
            }
          }
        }

        can_remove_bses[p][bs.to_ulong()] =
            ((int)adjadjs.size() >= (int)adjs.size() - 1);
      }
    }
  }
  static int N, N2, N4;
  static vector<Pos> all_poses;
  static vector<vector<Pos>> adj_poses;
  static vector<vector<Dir>> adj_dirs;
  static vector<vector<Pos>> move_to;
  static vector<int> pos_2_x, pos_2_y;
  // 2^8のbit列を受け取って削除可能かを返す
  // adj_poses[0], ..., adj_poses[3], adjadjs_check_remove[0], ...,
  // adjadjs_check_remove[3]
  // の順に詰めずに並べること 左が上位桁。
  static vector<bitset<256>> can_remove_bses;
  static vector<vector<Pos>> adjadjs_check_remove;
};

vector<Pos> Pos::all_poses;
vector<vector<Pos>> Pos::adj_poses;
vector<vector<Dir>> Pos::adj_dirs;
vector<vector<Pos>> Pos::move_to;
vector<int> Pos::pos_2_x, Pos::pos_2_y;
int Pos::N, Pos::N2, Pos::N4;
vector<bitset<256>> Pos::can_remove_bses;
vector<vector<Pos>> Pos::adjadjs_check_remove;




class Timer {
  chrono::system_clock::time_point _start, _end;
  long long int _sum = 0, _count = 0;

 public:
  void start() { _start = chrono::system_clock::now(); }

  void stop() { _end = chrono::system_clock::now(); }

  void add() {
    const chrono::system_clock::time_point now = chrono::system_clock::now();
    _sum += static_cast<double>(
        chrono::duration_cast<chrono::nanoseconds>(now - _start).count());
    _count++;
  }

  long long int sum() const { return _sum / 1000; }

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



#include <array>
#include <cmath>
#include <iostream>
#include <limits>

/* start */

struct Vec3 {
  double x, y, z;

  Vec3() = default;
  Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

  Vec3 operator-(const Vec3 &v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
  }
  Vec3 &operator-=(const Vec3 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }
  Vec3 operator+(const Vec3 &v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
  }
  Vec3 &operator+=(const Vec3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }
  Vec3 operator*(double s) const { return Vec3(x * s, y * s, z * s); }
  Vec3 operator/(double s) const {
    assert(s != 0.0); // ゼロ除算を防ぐ
    return Vec3(x / s, y / s, z / s);
  }
  Vec3 &operator/=(double s) {
    x /= s;
    y /= s;
    z /= s;
    return *this;
  }
  double dot(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }
  Vec3 cross(const Vec3 &v) const {
    return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }
  double norm2() const { return dot(*this); }
  double norm() const { return std::sqrt(norm2()); }
  double distance(const Vec3 &v) const {
    return std::sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) +
                     (z - v.z) * (z - v.z));
  }
  double get(int index) const {
    if (index == 0) {
      return x;
    } else if (index == 1) {
      return y;
    } else if (index == 2) {
      return z;
    } else {
      assert(false); // Invalid index
    }
  }

  friend std::ostream &operator<<(std::ostream &os, const Vec3 &v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
  }
};

// 三角形への直交射影
Vec3 closest_point_on_segment(const Vec3 &p, const Vec3 &a, const Vec3 &b) {
  Vec3 ab = b - a;
  double t = (p - a).dot(ab) / ab.norm2();
  t = std::max(0.0, std::min(1.0, t));
  return a + ab * t;
}

bool is_inside_triangle(const Vec3 &p, const Vec3 &a, const Vec3 &b,
                        const Vec3 &c) {
  Vec3 n = (b - a).cross(c - a);
  double total_area = n.norm();
  double area1 = (b - p).cross(c - p).norm();
  double area2 = (c - p).cross(a - p).norm();
  double area3 = (a - p).cross(b - p).norm();
  double sum = area1 + area2 + area3;
  return std::abs(sum - total_area) < 1e-6;
}

Vec3 project_point_to_triangle(const Vec3 &p, const Vec3 &a, const Vec3 &b,
                               const Vec3 &c, const double eps = 1e-6) {
  Vec3 n = (b - a).cross(c - a);
  double denom = n.norm2();
  if (denom < 1e-12)
    return a; // degenerate

  // double dist = (p - a).dot(n) / std::sqrt(denom);
  Vec3 proj = p - n * ((p - a).dot(n) / denom);

  if (is_inside_triangle(proj, a, b, c))
    return proj;

  Vec3 best = closest_point_on_segment(p, a, b);
  double min_dist2 = (p - best).norm2();

  for (auto &&[u, v] : std::array<std::pair<Vec3, Vec3>, 2>{{{b, c}, {c, a}}}) {
    Vec3 q = closest_point_on_segment(p, u, v);
    double d2 = (p - q).norm2();
    if (d2 < min_dist2) {
      min_dist2 = d2;
      best = q;
    }
  }

  Vec3 bestDiff = best - p;
  Vec3 ret = p + bestDiff * (1.0 + eps); // 少しだけ先に
  return ret;
}

// 射影本体（四面体に対する）
Vec3 project_point_to_tetrahedron(const Vec3 &v, const Vec3 &a, const Vec3 &b,
                                  const Vec3 &c, const Vec3 &d, const double eps = 1e-6) {
  Vec3 best;
  double min_dist2 = std::numeric_limits<double>::max();

  for (auto &&face : std::array<std::array<Vec3, 3>, 4>{
           {{a, b, c}, {a, b, d}, {a, c, d}, {b, c, d}}}) {
    Vec3 proj = project_point_to_triangle(v, face[0], face[1], face[2], eps);
    double d2 = (v - proj).norm2();
    if (d2 < min_dist2) {
      min_dist2 = d2;
      best = proj;
    }
  }

  return best;
}

double signed_volume(const Vec3 &a, const Vec3 &b, const Vec3 &c,
                     const Vec3 &d) {
  return ((b - a).cross(c - a)).dot(d - a) / 6.0;
}

bool is_inside_tetrahedron(const Vec3 &v, const Vec3 &a, const Vec3 &b,
                           const Vec3 &c, const Vec3 &d) {
  double v0 = signed_volume(a, b, c, d);
  double v1 = signed_volume(v, b, c, d);
  double v2 = signed_volume(a, v, c, d);
  double v3 = signed_volume(a, b, v, d);
  double v4 = signed_volume(a, b, c, v);

  // すべて同じ符号またはゼロ
  return (v0 >= 0 && v1 >= 0 && v2 >= 0 && v3 >= 0 && v4 >= 0) ||
         (v0 <= 0 && v1 <= 0 && v2 <= 0 && v3 <= 0 && v4 <= 0);
}

double tetrahedron_volume(const Vec3 &a, const Vec3 &b, const Vec3 &c,
                          const Vec3 &d) {
  Vec3 ab = b - a;
  Vec3 ac = c - a;
  Vec3 ad = d - a;
  return std::abs(ab.cross(ac).dot(ad)) / 6.0;
}

// 3次元空間上の凸包構築（点数が少ない場合の全探索法）
// 戻り値: 各面を構成する3点のインデックスのリスト
inline std::vector<std::array<int, 3>> convex_hull_3d(const std::vector<Vec3>& pts) {
    int n = pts.size();
    std::vector<std::array<int, 3>> faces;
    if (n < 4) return faces;
    for (int i = 0; i < n; ++i) for (int j = 0; j < n; ++j) for (int k = 0; k < n; ++k) {
        if (i == j || j == k || k == i) continue;
        Vec3 nrm = (pts[j] - pts[i]).cross(pts[k] - pts[i]);
        if (nrm.norm2() < 1e-12) continue;
        int sign = 0, ok = 1;
        for (int l = 0; l < n; ++l) {
            if (l == i || l == j || l == k) continue;
            double d = nrm.dot(pts[l] - pts[i]);
            if (std::abs(d) < 1e-10) continue;
            if (sign == 0) sign = (d > 0 ? 1 : -1);
            else if ((d > 0 ? 1 : -1) != sign) { ok = 0; break; }
        }
        if (ok) faces.push_back({i, j, k});
    }
    std::sort(faces.begin(), faces.end());
    faces.erase(std::unique(faces.begin(), faces.end()), faces.end());
    return faces;
}


/* start */

Timer global_timer;
int N, K, H, T, D;

struct InitialPartition {
  vector<bool> canMoveRights;
  vector<bool> canMoveDowns;
  InitialPartition()
      : canMoveRights(N * N, false), canMoveDowns(N * N, false) {}

  InitialPartition(vector<bool> canMoveRights, vector<bool> canMoveDowns)
      : canMoveRights(canMoveRights), canMoveDowns(canMoveDowns) {}

  InitialPartition(const vector<Pos> &continuousPoses)
      : canMoveRights(N * N, false), canMoveDowns(N * N, false) {
    // 連続している場合、仕切りは0
    rep(i, (int)continuousPoses.size() - 1) {
      const auto &pos = continuousPoses[i];
      const auto &nextPos = continuousPoses[i + 1];
      if (pos.X() == nextPos.X()) {
        // 横方向の仕切り
        canMoveDowns[Pos(pos.X(), min(pos.Y(), nextPos.Y()))] = true;
      } else if (pos.Y() == nextPos.Y()) {
        // 縦方向の仕切り
        canMoveRights[Pos(min(pos.X(), nextPos.X()), pos.Y())] = true;
      } else {
        assert(false); // ここに来ることはないはず
      }
    }
  }

  void Merge(const InitialPartition &other) {
    for (int i = 0; i < N * N; ++i) {
      canMoveRights[i] = canMoveRights[i] || other.canMoveRights[i];
      canMoveDowns[i] = canMoveDowns[i] || other.canMoveDowns[i];
    }
  }

  void AddPartition(const Pos pos1, const Pos pos2) {
    if (pos1.X() == pos2.X()) {
      // 横方向の仕切り
      canMoveDowns[Pos(pos1.X(), min(pos1.Y(), pos2.Y()))] = false;
    } else if (pos1.Y() == pos2.Y()) {
      // 縦方向の仕切り
      canMoveRights[Pos(min(pos1.X(), pos2.X()), pos1.Y())] = false;
    } else {
      assert(false); // Invalid partition
    }
  }
};

void Print(const InitialPartition &partition) {
  for (int y = 0; y < N; ++y) {
    for (int x = 0; x < N - 1; ++x) {
      Pos p(x, y);
      cout << (partition.canMoveRights[p] ? 0 : 1) << " ";
    }
    cout << endl;
  }
  for (int y = 0; y < N - 1; ++y) {
    for (int x = 0; x < N; ++x) {
      Pos p(x, y);
      cout << (partition.canMoveDowns[p] ? 0 : 1) << " ";
    }
    cout << endl;
  }
}

struct Color {
  double c, m, y;
  Color() : c(0.0), m(0.0), y(0.0) {}
  Color(double c, double m, double y) : c(c), m(m), y(y) {}
  Color(const Vec3 &vec) : c(vec.x), m(vec.y), y(vec.z) {}

  Color operator+(const Color &other) const {
    Color result;
    result.c = c + other.c;
    result.m = m + other.m;
    result.y = y + other.y;
    return result;
  }

  Color operator-(const Color &other) const {
    Color result;
    result.c = c - other.c;
    result.m = m - other.m;
    result.y = y - other.y;
    return result;
  }

  Color &operator+=(const Color &other) {
    c += other.c;
    m += other.m;
    y += other.y;
    return *this;
  }

  Color &operator-=(const Color &other) {
    c -= other.c;
    m -= other.m;
    y -= other.y;
    return *this;
  }

  Color operator*(double factor) const {
    Color result;
    result.c = c * factor;
    result.m = m * factor;
    result.y = y * factor;
    return result;
  }

  Color operator/(double factor) const {
    Color result;
    result.c = c / factor;
    result.m = m / factor;
    result.y = y / factor;
    return result;
  }

  double Distance(const Color &other) const {
    return sqrt(pow(c - other.c, 2) + pow(m - other.m, 2) +
                pow(y - other.y, 2));
  }

  friend ostream &operator<<(ostream &os, const Color &color) {
    os << color.c << " " << color.m << " " << color.y;
    return os;
  }

  double Get(int index) const {
    if (index == 0) {
      return c;
    } else if (index == 1) {
      return m;
    } else if (index == 2) {
      return y;
    } else {
      assert(false); // Invalid index
    }
  }

  Vec3 ToVec3() const { return Vec3(c, m, y); }
};

struct Well {
  double colorAmount;
  int cellCount;
  Color color;

  Well() : colorAmount(0.0), cellCount(1), color(Color{0.0, 0.0, 0.0}) {}

  Well(const int cellCount)
      : colorAmount(0.0), cellCount(cellCount), color(Color{0.0, 0.0, 0.0}) {}

  Well(double a, const int cellCount, const Color &c)
      : colorAmount(a), cellCount(cellCount), color(c) {}

  void Merge(const Well &other) {
    if (colorAmount + other.colorAmount > 0.0) {
      color = (color * colorAmount + other.color * other.colorAmount) /
              (colorAmount + other.colorAmount);
      colorAmount = colorAmount + other.colorAmount;
    }
    cellCount = cellCount + other.cellCount;
  }

  pair<Well, Well> Split(const int a, const int b) const {
    assert(a + b == cellCount);
    assert(a + b > 0);
    Well result_a, result_b;
    result_a.colorAmount = colorAmount * a / (a + b);
    result_b.colorAmount = colorAmount * b / (a + b);
    result_a.color = color;
    result_b.color = color;
    result_a.cellCount = a;
    result_b.cellCount = b;
    return make_pair(result_a, result_b);
  }

  bool CanFullPut() const {
    return colorAmount + 1.0 < cellCount + 1e-6; // 誤差を考慮して
  }

  void Put(const Color &color) {
    Color newColor = this->color * (colorAmount / (colorAmount + 1.0)) +
                     color * (1.0 / (colorAmount + 1.0));
    this->color = newColor;
    colorAmount += 1.0;
  }

  bool CanSubmit() const {
    return colorAmount >= 1.0 - 1e-6; // 誤差を考慮して
  }

  void Pick() {
    assert(colorAmount >= 1.0 - 1e-6);
    colorAmount -= 1.0;
    chmax(colorAmount, 0.0);
  }

  double GetError(const Color &targetColor) const {
    return color.Distance(targetColor);
  }

  double AmountPerCell() const {
    return cellCount > 0 ? colorAmount / cellCount : 0.0;
  }
};

double Amari(const double amount){
  return abs(amount - std::round(amount));
}

// 位置を蛇上に並べただけ
vector<Pos> snakePoses;

vector<Color> ownColors;
vector<Vec3> ownColorVecs;
vector<Color> targetColors;
vector<Vec3> targetColorVecs;

vector<int> bestColorIndexesFour;
vector<int> bestColorIndexesFive;
vector<int> bestColorIndexesSix;
vector<vector<int>> indexesSixAll;
double bestOutsideErrorFour = INF;
double bestOutsideErrorFive = INF;
double bestOutsideErrorSix = INF;
vector<bitset<1000>> insideBSs;

bitset<1000> &InsideBSAt(int k1, int k2, int k3, int k4) {
  return insideBSs[k1 * K * K * K + k2 * K * K + k3 * K + k4];
}

void InitInsideBSs() {
  // 四面体にターゲットが含まれているかどうかを求める
  insideBSs.resize(K * K * K * K);
  vector<int> currentColorIndexes;
  int maxInsideCount = 0;
  const auto func = [&](auto self) {
    if (currentColorIndexes.size() == 4u) {
      // 四面体に含まれてるかどうかを計算
      bitset<1000> insideBS = 0;
      vector<Vec3> points;
      rep(k, 4) {
        const auto &c = ownColors[currentColorIndexes[k]];
        points.emplace_back(c.c, c.m, c.y);
      }
      rep(i, H) {
        if (is_inside_tetrahedron(targetColorVecs[i], points[0], points[1],
                                  points[2], points[3])) {
          insideBS[i] = true;
        }
      }
      if (chmax(maxInsideCount, (int)insideBS.count())) {
        bestColorIndexesFour = currentColorIndexes;
      }

      // 4!通りのindexで格納
      vector<int> permutation = currentColorIndexes;
      do {
        int index = 0;
        rep(k, 4) { index = index * K + permutation[k]; }
        insideBSs[index] = insideBS;
      } while (next_permutation(permutation.begin(), permutation.end()));
      return;
    }

    // DFS
    for (int i = currentColorIndexes.empty() ? 0
                                             : currentColorIndexes.back() + 1;
         i < (int)ownColors.size(); ++i) {
      currentColorIndexes.push_back(i);
      self(self);
      currentColorIndexes.pop_back();
    }
  };

  func(func);

  cerr << "Max inside count(4) = " << maxInsideCount << endl;
}

// プログラムの一番最初に呼び出す。入力を受け取ったり、前計算すべき要素の初期化を行ったりする。
void Initialize(istream &is) {
  global_timer.start();
  is >> N >> K >> H >> T >> D;
  Pos::StaticInit(N);

  rep(k, K) {
    Color c;
    is >> c.c >> c.m >> c.y;
    ownColors.push_back(c);
    ownColorVecs.emplace_back(c.ToVec3());
  }
  rep(h, H) {
    Color c;
    is >> c.c >> c.m >> c.y;
    targetColors.push_back(c);
    targetColorVecs.emplace_back(c.ToVec3());
  }

  bool flip = false;
  rep(y, N) {
    rep(x, N) {
      const int xxx = flip ? N - 1 - x : x;
      snakePoses.emplace_back(xxx, y);
    }
    flip = !flip;
  }

  cerr << "K = " << K << endl;

  // 凸包を求める
  const auto faces = convex_hull_3d(ownColorVecs);
  set<int> convexHullSet;
  for (const auto &face : faces) {
    for (const auto &index : face) {
      convexHullSet.insert(index);
    }
  }
  cerr << "Convex hull size = " << convexHullSet.size() << endl;

  InitInsideBSs();

  {
    double minDistSum = 0.0;
    rep(i, H) {
      const auto &target = targetColorVecs[i];
      if (is_inside_tetrahedron(target, ownColorVecs[bestColorIndexesFour[0]],
                                ownColorVecs[bestColorIndexesFour[1]],
                                ownColorVecs[bestColorIndexesFour[2]],
                                ownColorVecs[bestColorIndexesFour[3]])) {
        // 四面体の内側なら、距離は0
        continue;
      }

      // 射影の距離
      const auto projected = project_point_to_tetrahedron(
          target, ownColorVecs[bestColorIndexesFour[0]],
          ownColorVecs[bestColorIndexesFour[1]],
          ownColorVecs[bestColorIndexesFour[2]],
          ownColorVecs[bestColorIndexesFour[3]]);
      minDistSum += projected.distance(target);
    }

    cerr << "Min dist sum(4) = " << minDistSum
         << " x 1e4 = " << (int)(minDistSum * 1e4) << endl;
    bestOutsideErrorFour = minDistSum * 1e4;
  }

  {
    // 5色使った場合の、最大内包数
    int maxInsideCount = 0;
    // k3,k4,k5を底面として、k1,k2を頂点とする
    rep(k5, K) {
      rep(k4, k5) {
        rep(k3, k4) {
          rep(k2, K) {
            rep(k1, k2) {
              const auto bs1 = InsideBSAt(k1, k3, k4, k5);
              const auto bs2 = InsideBSAt(k2, k3, k4, k5);
              const auto mergedBS = bs1 | bs2;
              int insideCount = (int)mergedBS.count();
              if (insideCount > maxInsideCount) {
                maxInsideCount = insideCount;
                bestColorIndexesFive = {k1, k2, k3, k4, k5};
              }
            }
          }
        }
      }
    }

    cerr << "Max inside count(5) = " << maxInsideCount << endl;
    const int k1 = bestColorIndexesFive[0];
    const int k2 = bestColorIndexesFive[1];
    const int k3 = bestColorIndexesFive[2];
    const int k4 = bestColorIndexesFive[3];
    const int k5 = bestColorIndexesFive[4];
    // k3, k4, k5が底面
    const auto bs1 = InsideBSAt(k1, k3, k4, k5);
    const auto bs2 = InsideBSAt(k2, k3, k4, k5);
    const auto mergedBS = bs1 | bs2;

    double minDistSum = 0.0;
    rep(i, H) {
      if (mergedBS[i]) {
        continue;
      }

      const auto &target = targetColorVecs[i];

      // 射影の最短距離
      const auto projected1 = project_point_to_tetrahedron(
          target, ownColorVecs[k1], ownColorVecs[k3], ownColorVecs[k4],
          ownColorVecs[k5]);
      const auto projected2 = project_point_to_tetrahedron(
          target, ownColorVecs[k2], ownColorVecs[k3], ownColorVecs[k4],
          ownColorVecs[k5]);
      double minDist =
          min(projected1.distance(target), projected2.distance(target));
      cerr << minDist << " ";
      minDistSum += minDist;
    }
    cerr << endl;
    cerr << "Min dist sum = " << minDistSum
         << " x 1e4 = " << (int)(minDistSum * 1e4) << endl;
    bestOutsideErrorFive = minDistSum * 1e4;
  }

  if (K >= 6) {
    // 6色使った場合の、最大内包数
    int maxInsideCount = 0;
    bitset<1000> bestBS;
    rep(k5, K) {
      rep(k4, k5) {
        rep(k3, k4) {
          rep(k2, k3) {
            const auto bs0 = InsideBSAt(k2, k3, k4, k5);
            rep(k1, k2) {
              const auto bs1 = bs0 | InsideBSAt(k1, k3, k4, k5) | InsideBSAt(k1, k2, k4, k5) |
                         InsideBSAt(k1, k2, k3, k5) |
                         InsideBSAt(k1, k2, k3, k4);
              rep(k0, k1) {
                const auto bs2 = bs1 | InsideBSAt(k0, k3, k4, k5) | InsideBSAt(k0, k2, k4, k5) |
                           InsideBSAt(k0, k2, k3, k5) | InsideBSAt(k0, k2, k3, k4) |
                           InsideBSAt(k0, k1, k4, k5) | InsideBSAt(k0, k1, k3, k5) |
                           InsideBSAt(k0, k1, k3, k4) | InsideBSAt(k0, k1, k2, k5) |
                           InsideBSAt(k0, k1, k2, k3);
                int insideCount = bs2.count();
                if (insideCount > maxInsideCount) {
                  maxInsideCount = insideCount;
                  bestColorIndexesSix = {k0, k1, k2, k3, k4, k5};
                  bestBS = bs2;
                }
                if(insideCount == H){
                  indexesSixAll.push_back(
                      {k0, k1, k2, k3, k4, k5}); // 全てのターゲットが含まれる
                }
              }
            }
          }
        }
      }
    }
    cerr << "Max inside count(6) = " << maxInsideCount << endl;

    // const int k0 = bestColorIndexesSix[0];
    // const int k1 = bestColorIndexesSix[1];
    // const int k2 = bestColorIndexesSix[2];
    // const int k3 = bestColorIndexesSix[3];
    // const int k4 = bestColorIndexesSix[4];
    // const int k5 = bestColorIndexesSix[5];
    // // k3, k4, k5が底面
    // const auto bs0 = InsideBSAt(k0, k3, k4, k5);
    // const auto bs1 = InsideBSAt(k1, k3, k4, k5);
    // const auto bs2 = InsideBSAt(k2, k3, k4, k5);
    // const auto mergedBS = bs0 | bs1 | bs2;

    // double minDistSum = 0.0;
    // rep(i, H) {
    //   if (mergedBS[i]) {
    //     continue;
    //   }

    //   const auto &target = targetColorVecs[i];

    //   // 射影の最短距離
    //   const auto projected0 = project_point_to_tetrahedron(
    //       target, ownColorVecs[k0], ownColorVecs[k3], ownColorVecs[k4],
    //       ownColorVecs[k5]);
    //   const auto projected1 = project_point_to_tetrahedron(
    //       target, ownColorVecs[k1], ownColorVecs[k3], ownColorVecs[k4],
    //       ownColorVecs[k5]);
    //   const auto projected2 = project_point_to_tetrahedron(
    //       target, ownColorVecs[k2], ownColorVecs[k3], ownColorVecs[k4],
    //       ownColorVecs[k5]);
    //   double minDist =
    //       min({projected1.distance(target), projected2.distance(target),
    //            projected0.distance(target)});
    //   cerr << minDist << " ";
    //   minDistSum += minDist;
    // }
    // cerr << endl;
    // cerr << "Min dist sum = " << minDistSum
    //      << " x 1e4 = " << (int)(minDistSum * 1e4) << endl;
    // bestOutsideErrorSix = minDistSum * 1e4;
    bestOutsideErrorSix = INF;
  }
}




/* start */

vector<double> PARAMS = {1.0, 0.1};


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

enum RealActionKind { kPut, kSubmit, kRemove, kPartition };

struct RealAction {
  RealActionKind kind;
  Pos pos;
  Dir dir; // 出し入れする仕切りの方向
  int k;

  static RealAction Put(Pos p, int k) { return RealAction{kPut, p, kD, k}; }

  static RealAction Submit(Pos p, int target) { return RealAction{kSubmit, p, kD, target}; }

  static RealAction Remove(Pos p) { return RealAction{kRemove, p, kD, -1}; }

  static RealAction Partition(Pos p, Dir d) {
    return RealAction{kPartition, p, d, -1};
  }

  static RealAction Partition(Pos p0, Pos p1) {
    if (p0.X() == p1.X()) {
      return Partition(Pos(p0.X(), min(p0.Y(), p1.Y())), kD);
    } else if (p0.Y() == p1.Y()) {
      return Partition(Pos(min(p0.X(), p1.X()), p0.Y()), kR);
    } else {
      assert(false); // Invalid partition
    }
  }

  friend ostream &operator<<(ostream &os, const RealAction &action) {
    switch (action.kind) {
    case kPut:
      os << 1 << " " << action.pos.Y() << " " << action.pos.X() << " "
         << action.k;
      break;
    case kSubmit:
      os << 2 << " " << action.pos.Y() << " " << action.pos.X();
      break;
    case kRemove:
      os << 3 << " " << action.pos.Y() << " " << action.pos.X();
      break;
    case kPartition: {
      Pos nextPos = action.pos;
      nextPos.Move(action.dir);
      os << 4 << " " << action.pos.Y() << " " << action.pos.X() << " "
         << nextPos.Y() << " " << nextPos.X();
    } break;
    default:
      assert(false); // Invalid action kind
    }
    return os;
  }
};

void Print(const vector<RealAction> &actions) {
  for (const auto &action : actions) {
    cout << action << endl;
  }
}

/* start */

struct BinaryPoint {
  vector<int> targetIndexes;
  Vec3 center;
  int prevColorIndex = -1; // この点を作成するために使用された色のindex
  vector<Vec3> movedTargets; // 動かしたターゲットのリスト

  // ダミーを生成するコンストラクタ
  BinaryPoint() { center = Vec3(0.0, 0.0, 0.0); }
  // 1要素によるBinaryPointのコンストラクタ
  explicit BinaryPoint(const int targetIndex)
      : targetIndexes({targetIndex}), prevColorIndex(-1) {
    center = targetColorVecs[targetIndex];
    movedTargets.push_back(targetColorVecs[targetIndex]);
  }

  // 複数要素によるBinaryPointのコンストラクタ
  BinaryPoint(vector<int> targetIndexes_, const int prevColorIndex_,
              vector<Vec3> movedTargets_)
      : targetIndexes(targetIndexes_), prevColorIndex(prevColorIndex_),
        movedTargets(movedTargets_) {
    center = Vec3(0.0, 0.0, 0.0);
    for (const auto &target : movedTargets) {
      center = center + target;
    }
    center /= targetIndexes.size();
  }

  int Count() const { return (int)targetIndexes.size(); }

  void InverseMove(const int k, const double moveProps) {
    prevColorIndex = k; // 動かす方向を記録
    center = center - ownColorVecs[k] * moveProps;
    for (auto &target : movedTargets) {
      target = target - ownColorVecs[k] * moveProps;
    }
  }

  // ダミーと組んだ場合、半分になる
  void ToHalf(){
    assert(movedTargets.size() == 1u);
    prevColorIndex = -1;
    center /= 2.0;
    for (auto &target : movedTargets) {
      target /= 2.0;
    }
  }

  bool IsDummy() const { return targetIndexes.empty(); }

  static BinaryPoint Merge(const BinaryPoint &a, const BinaryPoint &b) {
    // 2つのBinaryPointを結合して新しいBinaryPointを作成
    vector<int> newTargetIndexes = a.targetIndexes;
    newTargetIndexes.insert(newTargetIndexes.end(), b.targetIndexes.begin(),
                            b.targetIndexes.end());
    auto movedTargets = a.movedTargets;
    movedTargets.insert(movedTargets.end(), b.movedTargets.begin(),
                        b.movedTargets.end());
    int newPrevColorIndex = -1; // 結合時は色のindexはリセット
    return BinaryPoint(newTargetIndexes, newPrevColorIndex, movedTargets);
  }
};

struct BinaryState {
  // [i] := BinaryPointの1つあたりの要素数は2^i
  vector<vector<BinaryPoint>> binary2Points;
  int beginIndex = 0;
  int endIndex = 0;
  int totalCount = 0;
  vector<Pos> continuousPoses; // 連続したPosのリスト

  BinaryState(const int beginIndex, const int endIndex,
              vector<Pos> continuousPoses)
      : beginIndex(beginIndex), endIndex(endIndex),
        totalCount(endIndex - beginIndex), continuousPoses(continuousPoses) {
    // beginIndexからendIndexまでのtargetIndexesを持つBinaryPointを作成
    binary2Points.reserve(endIndex - beginIndex);
    // 1要素のBinaryPointを作成
    vector<BinaryPoint> initialPoints;
    for (int i = beginIndex; i < endIndex; ++i) {
      initialPoints.emplace_back(BinaryPoint(i));
    }
    binary2Points.emplace_back(initialPoints);
  }

  int MergedPointCount() const {
    // 現在のBinaryPointの数を返す
    return (int)binary2Points.back().size();
  }

  void MiddleStep() {
    // BinaryPointを2つずつ結合して新しいBinaryPointを作成
    const auto oldPoints = binary2Points.back();
    const bool isFirstStep = binary2Points.size() == 1;

    // 新しいBinaryPointを作成
    // まず、全体を(0,0,0)に近づける
    // TODO: ペアを作ることも踏まえて、動かす方向を決める
    vector<BinaryPoint> movedPoints;

    if (isFirstStep) {
      // 最初は、移動しないでペアを作るので、何もしない
      for (const auto &point : oldPoints) {
        movedPoints.emplace_back(point);
      }
      // 2の累乗個になるように、ダミーを追加する
      int size = 1;
      while (size < (int)oldPoints.size()) {
        size *= 2;
      }
      while ((int)movedPoints.size() < size) {
        movedPoints.emplace_back(BinaryPoint()); // ダミーのBinaryPoint
      }
    } else {
      const double moveProps = pow(0.5, (int)binary2Points.size() - 1);
      Vec3 zeroVec(0.0, 0.0, 0.0);

      for (int i = 0; i < (int)oldPoints.size(); i++) {
        if(oldPoints[i].Count() == 1){
          // ダミーと組んだ場合は、半分になる
          auto newPoint = oldPoints[i];
          newPoint.ToHalf();
          movedPoints.emplace_back(newPoint);
          // cerr << oldPoints[i].center.distance(zeroVec) << endl;
          continue;
        }
        double minPenalty = INF;
        int bestK = -1;
        rep(k, K) {
          // 動かす方向を全部試す
          // targetを動かすので、引き算
          Vec3 newCenter = oldPoints[i].center - ownColorVecs[k] * moveProps;
          double expectedPenalty = 0.0;
          rep(kkk, K) {
            Vec3 newNewCenter = newCenter - ownColorVecs[kkk] * moveProps;
            double penalty = newNewCenter.distance(zeroVec);
            expectedPenalty += penalty;
          }
          expectedPenalty /= K;
          if (expectedPenalty < minPenalty) {
            minPenalty = expectedPenalty;
            bestK = k;
          }
        }

        // 最も近づく方向を適用
        BinaryPoint newPoint = oldPoints[i];
        newPoint.InverseMove(bestK, moveProps);
        movedPoints.emplace_back(newPoint);
        if (i == 0) {
          auto debug = newPoint;
          cerr << "--" << endl;
          cerr << oldPoints[i].center.distance(zeroVec) << " "
               << newPoint.center.distance(zeroVec) << " " << bestK << " "
               << oldPoints[i].center.x << " " << oldPoints[i].center.y << " "
               << oldPoints[i].center.z << endl;
          cerr << minPenalty << " " << bestK << " " << debug.center.x << " "
               << debug.center.y << " " << debug.center.z << endl;
          cerr << "--" << endl;
        }
      }
    }
    binary2Points.back() = movedPoints;

    // 焼きなましでペアを作る
    // 初期ペア
    vector<pair<int, int>> bestPairs;
    rep(i, (int)movedPoints.size() / 2) {
      bestPairs.emplace_back(i * 2, i * 2 + 1);
    }

    const auto func = [&](const pair<int, int> pair) {
      const int a = pair.first;
      const int b = pair.second;
      const bool isDummyA = movedPoints[a].IsDummy();
      const bool isDummyB = movedPoints[b].IsDummy();
      if (isDummyA && isDummyB) {
        return (double)INF; // 両方ダミーなら、ペナルティはINF
      }

      if (isDummyA || isDummyB) {
        // 片方がダミーなら、ペナルティは0
        return 0.0;
      }

      const auto centerA = movedPoints[a].center;
      const auto centerB = movedPoints[b].center;
      // ペナルティを計算
      double penalty = centerA.distance(centerB);
      return penalty;
    };

    double minPenalty = 0.0;
    for (const auto &pair : bestPairs) {
      minPenalty += func(pair);
    }

    cerr << "initial Penalty = " << minPenalty / bestPairs.size() << endl;
    const double kBeginTemp = 1.0;
    const double kEndTemp = 0.0001;
    double temp = kBeginTemp;
    const int kIterCount = 1000000;
    rep(iter, kIterCount) {
      if(iter % 100 == 0){
        temp = kBeginTemp * pow(kEndTemp / kBeginTemp, (double)iter / kIterCount);
      }

      // ペアをランダムに入れ替える
      const int i = rand(0, (int)bestPairs.size());
      const int j = rand(0, (int)bestPairs.size());
      if (i == j)
        continue;
      auto oldPairA = bestPairs[i];
      auto oldPairB = bestPairs[j];
      auto newPairA = oldPairA;
      auto newPairB = oldPairB;
      if (rand(0, 2) == 0) {
        swap(newPairA.first, newPairB.first);
      } else {
        swap(newPairA.first, newPairB.second);
      }

      double diff = 0.0;
      diff -= func(oldPairA);
      diff -= func(oldPairB);
      diff += func(newPairA);
      diff += func(newPairB);

      double acceptProb = exp(-diff / temp);
      if (diff < 0.0 || rand(0, INF) < acceptProb * INF) {
        // ペナルティが減ったか、確率的に受け入れる
        minPenalty += diff;
        bestPairs[i] = newPairA;
        bestPairs[j] = newPairB;
      }
    }
    cerr << "minPenalty = " << minPenalty / bestPairs.size() << endl;
    cerr << "temp = " << temp << endl;

    // 新しいBinaryPointを作成
    vector<BinaryPoint> newPoints;
    for (const auto &pair : bestPairs) {
      const int a = pair.first;
      const int b = pair.second;

      // ペアを結合
      const BinaryPoint mergedPoint =
          BinaryPoint::Merge(movedPoints[a], movedPoints[b]);
      newPoints.emplace_back(mergedPoint);
      cerr << movedPoints[a].center.norm() << " "
           << movedPoints[b].center.norm() << " -> "
           << mergedPoint.center.norm() << endl;
    }

    binary2Points.push_back(newPoints);
  }

  void FinalStep() {
    const auto &lastPoints = binary2Points.back();
    assert((int)lastPoints.size() == 1);
    auto lastPoint = lastPoints[0];
    // 全体を2回動かす
    const double moveProps = pow(0.5, (int)binary2Points.size() - 1);
    const Vec3 zeroVec(0.0, 0.0, 0.0);

    int bestK1 = -1;
    int bestK2 = -1;
    double minPenalty = INF;
    rep(k1, K) {
      rep(k2, K) {
        auto newCenter = lastPoint.center - ownColorVecs[k1] * moveProps -
                         ownColorVecs[k2] * moveProps;
        double penalty = newCenter.distance(zeroVec);
        if (penalty < minPenalty) {
          minPenalty = penalty;
          bestK1 = k1;
          bestK2 = k2;
        }
      }
    }

    // 最も近づく方向を適用
    lastPoint.InverseMove(bestK1, moveProps);
    binary2Points.back() = {lastPoint}; // 最後のポイントを更新
    lastPoint.InverseMove(bestK2, moveProps);
    binary2Points.push_back({lastPoint});
    cerr << "Final Step: bestK1 = " << bestK1 << ", bestK2 = " << bestK2
         << ", minPenalty = " << minPenalty << endl;
  }

  vector<RealAction> ToRealActions() const {
    // 各targetIndexについて、各時点で属するMergedPointsのindexを逆順に並べたものを用いてソートする
    map<int, vector<int>> targetToMergedPoints;
    for (int i = 0; i < (int)binary2Points.size(); ++i) {
      const auto &points = binary2Points[i];
      rep(j, points.size()) {
        const auto &point = points[j];
        for (int targetIndex : point.targetIndexes) {
          targetToMergedPoints[targetIndex].push_back(j);
        }
      }
    }
    vector<int> sortedTargetIndexes;
    for (auto &[targetIndex, mergedPoints] : targetToMergedPoints) {
      // 各targetIndexについて、mergedPointsを逆順に並べる
      reverse(mergedPoints.begin(), mergedPoints.end());
      sortedTargetIndexes.push_back(targetIndex);
    }

    sort(sortedTargetIndexes.begin(), sortedTargetIndexes.end(),
         [&](int a, int b) {
           const auto &aaa = targetToMergedPoints[a];
           const auto &bbb = targetToMergedPoints[b];
           rep(i, min(aaa.size(), bbb.size())) {
             if (aaa[i] != bbb[i]) {
               return aaa[i] < bbb[i]; // 早い方を優先
             }
           }
           return aaa.size() < bbb.size(); // サイズが小さい方を優先
         });

    // ペアを見つけられなかった要素がある場合は、追加で1を足していく
    set<int> lonelyIndexes;
    for (const auto &point : binary2Points[1]) {
      if (point.targetIndexes.size() == 1u) {
        lonelyIndexes.insert(point.targetIndexes[0]);
      }
    }
    int additionalIndex = 0;
    map<int, int> targetToPosIndex;
    rep(i, sortedTargetIndexes.size()) {
      const int targetIndex = sortedTargetIndexes[i];
      assert(targetIndex >= 0);
      targetToPosIndex[targetIndex] = i + additionalIndex;
      if (lonelyIndexes.count(targetIndex)) {
        // ペアを見つけられなかった要素は、追加で1を足す
        additionalIndex++;
      }
    }

    // アクションに変換する
    vector<RealAction> actions;
    vector<RealAction> partitionActions;
    set<int> existPartitionIndexes;
    int nextPartition = 128;
    REP(t, binary2Points.size()) {
      int debug = 0;
      const auto &points = binary2Points[t];
      for (const auto &point : points) {
        // マージされた各pointについて、アクションを生成
        const auto colorIndex = point.prevColorIndex;
        if (colorIndex == -1) {
          continue;
        }

        // ダミーと組んだ場合の初手は、何もしない
        if(point.Count() == 1){
          continue;
        }

        const auto representativeTargetIndex =
            point.targetIndexes[0]; // 最初のtargetIndexを代表とする
        const auto pos =
            continuousPoses[targetToPosIndex.at(representativeTargetIndex)];
        actions.push_back(RealAction::Put(pos, colorIndex));
        debug++;
      }
      cerr << "t = " << t << ", debug = " << debug << endl;

      if (t >= 2 && binary2Points[t - 1].size() != binary2Points[t].size()) {
        // 仕切りを追加
        for (int left = nextPartition - 1; left < 256; left += nextPartition) {
          if (existPartitionIndexes.count(left)) {
            continue; // 既に存在する仕切りはスキップ
          }
          if (left + 1 >= (int)continuousPoses.size()) {
            continue; // 端の場合はスキップ
          }
          Pos leftPos = continuousPoses[left];
          Pos rightPos = continuousPoses[left + 1];
          RealAction action = RealAction::Partition(leftPos, rightPos);
          actions.push_back(action);
          partitionActions.push_back(action);
          existPartitionIndexes.insert(left);
        }

        nextPartition /= 2;
      }
    }

    // 提出する
    for (int targetIndex = beginIndex; targetIndex < endIndex; ++targetIndex) {
      const auto pos = continuousPoses[targetToPosIndex[targetIndex]];
      actions.push_back(RealAction::Submit(pos, targetIndex));
    }

    // 追加した仕切りを一通り削除
    actions.insert(actions.end(), partitionActions.begin(),
                   partitionActions.end());

    return actions;
  }
};







/* start */

namespace Solve3 {
class Solver {
public:
  Solver() {}
  void Solve() {
    auto continuousPoses = snakePoses;
    continuousPoses.resize(256);
    Print(InitialPartition(continuousPoses));
    vector<RealAction> answers;
    for (int beginIndex = 0; beginIndex < H; beginIndex += 256) {
      cerr << "beginIndex: " << beginIndex << endl;
      const int endIndex = min(beginIndex + 256, H);
      BinaryState binaryState(beginIndex, endIndex, continuousPoses);

      // 移動とマージを繰り返す
      while (binaryState.MergedPointCount() >= 2) {
        cerr << binaryState.MergedPointCount() << " points remain" << endl;
        binaryState.MiddleStep();
      }
      binaryState.FinalStep();

      Debug(binaryState);
      const auto answer = binaryState.ToRealActions();
      answers.insert(answers.end(), answer.begin(), answer.end());
    }
    cerr << "Total actions: " << answers.size() << endl;
    Print(answers);
    return;
  }

private:
  void Debug(const BinaryState &state) const {
    rep(t, state.binary2Points.size()) {
      const auto& points = state.binary2Points[t];
      double sum = 0.0;
      int count = 0;
      for(const auto& point : points) {
        for(const auto p : point.movedTargets) {
          sum += point.center.distance(p);
          count++;
        }
      }
      sum /= count;

      Vec3 allCenter(0.0, 0.0, 0.0);
      int allCount = 0;
      for (const auto &point : points) {
        for (const auto &p : point.movedTargets) {
          allCenter = allCenter + p;
          allCount++;
        }
      }
      allCenter /= allCount;
      double allSum = 0.0;
      for(const auto &point : points) {
        for (const auto &p : point.movedTargets) {
          allSum += allCenter.distance(p);
        }
      }
      allSum /= allCount;
      cerr << "t = " << t << ", points.size() = " << points.size()
           << ", avg distance = " << sum << ", all avg distance = " << allSum << endl;
    }
  }
};
} // namespace Solve3







/* start */

struct Candidate {
  int turn;
  double error;
  int putCount;
  int actionId;
  Candidate(int t, double e, int p, int a)
      : turn(t), error(e), putCount(p), actionId(a) {}
};

/*
各ターンは以下の順序とする
1. 全ボトルのCandidatesを計算
2. 全ボトルのClose()を実行 : このタイミングでは、Mainは変化しない
ボトルはコンタミする
3. 全ボトルのOpen()を実行 : このタイミングで、Mainが変化する
4. 全ボトルのUpdateMainAfterSubmit()を実行
*/
struct Bottle {
  // 目的の値と最大追加回数を受け取って、
  // 手数と誤差の候補を返す
  // 誤差の小さい順にソートされていることを期待する
  virtual vector<Candidate> Candidates(double targetValue) const = 0;

  // 現在の色を返す
  virtual Vec3 GetColor() const = 0;

  virtual int GetColorIndex() const = 0;

  // 中央ウェルを受け取って、ボトルを閉じ、その後の中央ウェルを返す
  virtual Well Close(const Well centerWell, const int actionId,
                     vector<RealAction> &realActions) = 0;

  // 中央ウェルを受け取って、アクションIDで指定された方法でボトルを開き、その後の中央ウェルを返す
  virtual Well Open(const Well centerWell, const int actionId,
                    vector<RealAction> &realActions) = 0;

  // 提出後の中央ウェルを受け取って更新する
  virtual void UpdateMainAfterSubmit(const Well centerWell) = 0;

  virtual int MainWellCellCount() const = 0;

  virtual void Save() = 0;
  virtual void Load() = 0;

  virtual InitialPartition GetInitialPartition() const = 0;

  // 全てを解放する
  virtual Well OpenAll(const Well centerWell,
                       vector<RealAction> &realActions) = 0;

  virtual double Amount() const = 0;
  virtual double RemainAmount() const = 0;
  virtual void SetLargeT() = 0;
  virtual void SetVeryLargeT() = 0;
};











/* start */

// iをa:bに分割する
// iとi+1をマージする
// iに絵の具を追加する
// iから絵の具を削除する
enum Kind {
  kBottleSplit,
  kBottleMerge,
  kBottlePut,
  kBottleRemove,
};
struct Command {
  Kind kind;
  int i;
  int a;
  int b;
  int k;
  Command(Kind kind, int i, int a = 0, int b = 0, int k = 0)
      : kind(kind), i(i), a(a), b(b), k(k) {}
  static Command Split(const int i, const int a, const int b) {
    return Command(kBottleSplit, i, a, b);
  }

  static Command Merge(const int i) { return Command(kBottleMerge, i); }

  static Command Put(const int i, const int k) {
    return Command(kBottlePut, i, 0, 0, k);
  }

  static Command Remove(const int i) { return Command(kBottleRemove, i); }
};

struct Action {
  vector<Command> closeCommands;
  vector<Command> openCommands;
  Action(vector<Command> close, vector<Command> open)
      : closeCommands(close), openCommands(open) {}
};

// 一列に並んだマス
// 0番目の要素は、共有スペース
struct Line {
  vector<Well> wells;
  // 0番目の要素は、共有スペース
  vector<Pos> *poses = nullptr;

  Line(vector<Pos>* continuousPoses) {
    // 共有スペースも含めたマス数
    wells.emplace_back(0.0, continuousPoses->size(), Color{0.0, 0.0, 0.0});
    poses = continuousPoses;
  }

  void DoCommand(const Command &command,
                 vector<RealAction> *realActions = nullptr) {
    switch (command.kind) {
    case kBottleSplit: {
      // iをa:bに分割する
      assert(command.i >= 0 && command.i < (int)wells.size());
      assert(command.a + command.b == wells[command.i].cellCount);
      if (realActions) {
        const int right = GetIndex(command.i, command.a);
        realActions->emplace_back(
            RealAction::Partition((*poses)[right - 1],
                                  (*poses)[right])); // 仕切りを追加
      }
      auto [well1, well2] = wells[command.i].Split(command.a, command.b);
      wells[command.i] = well1;
      wells.insert(wells.begin() + command.i + 1, well2);
      break;
    }
    case kBottleMerge: {
      // iとi+1をマージする
      assert(command.i >= 0 && command.i < (int)wells.size() - 1);
      if (realActions) {
        const int right = GetIndex(command.i + 1, 0);
        realActions->emplace_back(
            RealAction::Partition((*poses)[right - 1],
                                  (*poses)[right])); // 仕切りを削除
      }
      wells[command.i].Merge(wells[command.i + 1]);
      wells.erase(wells.begin() + command.i + 1);
      break;
    }
    case kBottlePut: {
      // iに絵の具を追加する
      assert(command.i >= 0 && command.i < (int)wells.size());
      if (realActions) {
        const int index = GetIndex(command.i, 0);
        realActions->emplace_back(
            RealAction::Put((*poses)[index], command.k)); // 絵の具を追加
      }
      wells[command.i].Put(ownColors[command.k]);
      break;
    }
    case kBottleRemove: {
      // iから絵の具を削除する
      assert(command.i >= 0 && command.i < (int)wells.size());
      if (realActions) {
        const int index = GetIndex(command.i, 0);
        realActions->emplace_back(
            RealAction::Remove((*poses)[index])); // 絵の具を削除
      }
      wells[command.i].Pick();
      break;
    }
    }
  }

  // i番目のwellのj番目のセルのindexを返す
  int GetIndex(const int i, const int j) const {
    int count = 0;
    for (int k = 0; k < i; ++k) {
      count += wells[k].cellCount;
    }
    return count + j;
  }

  void SetMainWell(const Color color, const double amountPerCell) {
    assert(!wells.empty());
    wells[0].color = color;
    wells[0].colorAmount = amountPerCell * wells[0].cellCount;
  }
};


#include <cstddef>

/* start */

// まっすぐ並んだ普通のボトル
struct BottleNormal : public Bottle {
  static vector<Action> actions;

  int kTankCap = 0;
  Line line;
  int colorIndex;
  Line savedLine;
  int putCount = 0;
  int savedPutCount = 0;    // 保存された補充回数
  int totalMaxPutCount = 0; // 最大補充回数
  bool isSmallT = false;    // 小さいTかどうか
  bool isFirstSmallT = false;
  bool isVeryLargeT = false; // 大きいTかどうか
  bool isFirstVeryLargeT = false; // 最初のターンでVeryLargeTかどうか
  static constexpr int kEmptyActionId = 0; // 何もしないアクションのID

  // 最初の要素は、隣接する共有スペース
  BottleNormal(vector<Pos> continuousPoses, const int colorIndex,
               const int totalMaxPutCount, bool isSmallT_, bool isVeryLargeT_)
      : line(new vector<Pos>(continuousPoses)), colorIndex(colorIndex),
        savedLine(line), totalMaxPutCount(totalMaxPutCount) {
    isSmallT = isSmallT_;
    isFirstSmallT = isSmallT_;
    isVeryLargeT = isVeryLargeT_;
    isFirstVeryLargeT = isVeryLargeT_;
    kTankCap = 30;
    assert(colorIndex >= 0 && colorIndex < (int)ownColors.size());
    if (IsSmallT()) {
      // 一番左だけ残す
      Command command = Command::Split(0, 1, line.wells[0].cellCount - 1);
      line.DoCommand(command);
    } else if (IsVeryLargeT()) {
      // 右1マスと10マス残す
      Command command1 = Command::Split(
          0, line.wells[0].cellCount - kTankCap - 1, kTankCap + 1);
      Command command2 = Command::Split(1, 1, kTankCap);
      line.DoCommand(command1);
      line.DoCommand(command2);
    } else {
      // 一旦、一番右だけ残す
      Command command = Command::Split(0, line.wells[0].cellCount - 1, 1);
      line.DoCommand(command);
    }
    Save();

    if (actions.empty()) {
      // 何もしないを追加
      actions.reserve(5e7);
      actions.push_back(Action({}, {})); // 0手のアクションを追加
    }
  }

  bool IsSmallT() const { return isSmallT && !IsVeryLargeT(); }

  void SetLargeT() override { isSmallT = false; }
  void SetVeryLargeT() override { isVeryLargeT = true; }

  bool IsVeryLargeT() const { return isVeryLargeT; }

  // ↓ 嘘コメント
  // アクションは以下のパターンがある
  // 開けっ放しで終わる場合は、次のターンの+1手を足した手数を記載している
  // 0手
  // * 何もしない
  // 2手
  // * (補充して、) 敷居を内側に追加して、開ける
  // 4手
  // * 敷居を外側に追加して開け、(補充して、)敷居を内側に追加して開ける
  // 6手
  // * 敷居1を内側に追加して、敷居2を外側に追加して、敷居0を開け、敷居3を内側に追加して、敷居2を開け、敷居3を開ける
  vector<Candidate> Candidates(double targetValue) const override {
    assert(targetValue >= 0.0);
    vector<Candidate> candidates;

    // 何もしないを追加
    candidates.emplace_back(0, targetValue, 0, kEmptyActionId);

    // 色の総量がtargetValue + eps未満なら、補充はしない
    const double colorAmountSum = ColorAmountSum();
    const int maxPutCount =
        colorAmountSum >= targetValue + 1e-2 || putCount == totalMaxPutCount
            ? 0
            : 1;
    // cerr << targetValue << " / " << colorAmountSum << ", " << putCount << " /
    // "
    //      << totalMaxPutCount << endl;

    // 2~4手を追加
    rep(putCount, maxPutCount + 1) {
      // 敷居を外側に追加して、内側を開ける
      // a1が負の場合は、内側に移動する
      for (int a1 = (line.wells.size() >= 2u && !IsVeryLargeT()
                         ? -(line.wells[1].cellCount - 1)
                         : 1);
           a1 <= line.wells[0].cellCount; a1++) {
        // for (int a1 = 1; a1 <= line.wells[0].cellCount; a1++) {
        if (a1 == 0)
          continue;
        auto currentLine1 = line;
        currentLine1.SetMainWell(Color(), 0.0);
        int b1 = a1 > 0 ? line.wells[0].cellCount - a1
                        : line.wells[1].cellCount + a1;

        // 敷居が0なら必ず追加しないといけない
        if (currentLine1.wells.size() == 1u && (b1 == 0 || a1 < 0)) {
          continue;
        }
        // Tが小さい場合、補充のタイミングで敷居を最大まで戻すようにする
        if (IsSmallT()) {
          if (a1 < 0)
            continue;
          if (putCount > 0 && a1 != 1)
            continue;
          if (putCount == 0 && b1 != 0)
            continue;
        }
        vector<Command> closeCommands;
        vector<Command> firstOpenCommands;

        // もし、veryLargeTなら、戻す
        if (IsVeryLargeT()) {
          if (currentLine1.wells.size() == 3u) {
            // なるべく10マス確保する
            if (currentLine1.wells[1].cellCount >= 2) {
              const int bbb = min(kTankCap - currentLine1.wells[2].cellCount,
                                  currentLine1.wells[1].cellCount - 1);
              if (bbb > 0) {
                const int aaa = currentLine1.wells[1].cellCount - bbb;
                const auto command = Command::Split(1, aaa, bbb);
                currentLine1.DoCommand(command);
                closeCommands.push_back(command);
                const auto command0 =
                    Command::Merge(2); // 敷居を外側に追加して開ける
                currentLine1.DoCommand(command0);
                closeCommands.push_back(command0);
              }
            }
          } else if (currentLine1.wells.size() == 2u) {
            // 敷居を外側に追加
            const int bbb = min(kTankCap, currentLine1.wells[1].cellCount - 1);
            const int aaa = currentLine1.wells[1].cellCount - bbb;
            if (bbb > 0) {
              const auto command0 = Command::Split(1, aaa, bbb);
              currentLine1.DoCommand(command0);
              closeCommands.push_back(command0);
            } else {
              const int bbbbb = 9;
              const int aaaaa = currentLine1.wells[0].cellCount - bbbbb;
              const auto command0 =
                  Command::Split(0, aaaaa, bbbbb); // 敷居を外側に追加
              currentLine1.DoCommand(command0);
              closeCommands.push_back(command0);
              b1 = currentLine1.wells[0].cellCount - a1;
            }
          } else {
            assert(currentLine1.wells.size() == 1u);
            // 1,10を追加
            const int bbb = kTankCap + 1;
            const int aaa = currentLine1.wells[0].cellCount - bbb;
            const auto command0 = Command::Split(0, aaa, bbb);
            currentLine1.DoCommand(command0);
            closeCommands.push_back(command0);

            const int aaaaa = 1;
            const int bbbbb = kTankCap;
            const auto command1 = Command::Split(1, aaaaa, bbbbb);
            currentLine1.DoCommand(command1);
            closeCommands.push_back(command1);
            b1 = currentLine1.wells[0].cellCount - a1;
          }
        }

        if (a1 > 0) {
          if (b1 > 0) {
            const auto command1 = Command::Split(0, a1, b1); // 敷居を外側に追加
            currentLine1.DoCommand(command1);
            closeCommands.push_back(command1);

            // 内側を開ける
            if (currentLine1.wells.size() >= 3u) {
              const auto command2 = Command::Merge(1);
              currentLine1.DoCommand(command2);
              closeCommands.push_back(command2);
            }
          }
        } else {
          // 内側に移動
          if (b1 == 0)
            continue;
          // 敷居を内側に追加
          const auto command1 = Command::Split(1, -a1, b1);
          currentLine1.DoCommand(command1);
          firstOpenCommands.push_back(command1);

          // 外側を開ける
          const auto command2 = Command::Merge(0);
          currentLine1.DoCommand(command2);
          firstOpenCommands.push_back(command2);
        }

        bool ng = false;
        rep(_t, putCount) {
          int index = IsVeryLargeT() ? 2 : 1;
          if (!currentLine1.wells[index].CanFullPut()) {
            if (index == 2) {
              index = 1;
            } else {
              ng = true; // 補充できないので、スキップ
              break;
            }
          }
          const auto putCommand = Command::Put(index, colorIndex);
          currentLine1.DoCommand(putCommand);
          firstOpenCommands.push_back(putCommand);
        }
        if (ng) {
          continue;
        }

        // VeryLargeなら、流し込む
        unordered_set<int> a0Set = {0};
        if (IsVeryLargeT()) {
          const auto amountPerCell = currentLine1.wells[2].AmountPerCell();
          if (amountPerCell > 0.0) {
            // 目標値のX倍になるように流す
            int a0Base = std::round(
                (targetValue * 1.2 - currentLine1.wells[1].colorAmount) /
                    amountPerCell +
                1e-12);
            for (auto a0 : {a0Base, a0Base - 1, a0Base + 1, INF}) {
              a0 = min(a0, currentLine1.wells[2].cellCount);
              a0 = max(a0, 0);
              a0Set.insert(a0);
            }

            // cerr << "a0: " << a0 << " amountPerCell: " << amountPerCell
            //      << ", targetValue: " << targetValue << " colorAmount: "
            //      << currentLine1.wells[1].colorAmount << endl;
          }
        }
        const auto currentLine1Memo = currentLine1;
        const auto firstOpenCommandsMemo = firstOpenCommands;
        for (const int a0 : a0Set) {
          currentLine1 = currentLine1Memo;
          firstOpenCommands = firstOpenCommandsMemo;
          if (a0 > 0) {
            const int b0 = currentLine1.wells[2].cellCount - a0;
            if (b0 > 0) {
              const auto command0 = Command::Split(2, a0, b0);
              currentLine1.DoCommand(command0);
              firstOpenCommands.push_back(command0);
            }

            const auto command1 = Command::Merge(1);
            currentLine1.DoCommand(command1);
            firstOpenCommands.push_back(command1);
          }

          // しきいを内側に追加して、外側を開ける
          // amountに、amountPerCell2 * いくつ を足すと、
          // targetValueに近づくか？
          const double amount = currentLine1.wells[0].colorAmount;
          const double amountPerCell2 = currentLine1.wells[1].AmountPerCell();
          const double bestCellCountDouble2 =
              (amountPerCell2 < 1e-8)
                  ? 0.0
                  : (targetValue - amount) /
                        amountPerCell2; // 目標値からの誤差を補う
          // TODO: 0や全部も試す
          const int bestCellCount2 = (int)round(bestCellCountDouble2);
          const int delta = IsVeryLargeT() ? 1 : 2; // 1マスずらす
          for (int a2 = bestCellCount2 - delta; a2 <= bestCellCount2 + delta;
               ++a2) {
            if (a2 <= 0)
              continue;
            const int b2 = currentLine1.wells[1].cellCount - a2;
            if (b2 < 0)
              continue;

            auto currentLine2 = currentLine1;
            vector<Command> openCommands = firstOpenCommands;
            // b2 == 0の場合は敷居の追加が不要
            if (b2 > 0) {
              const auto command3 =
                  Command::Split(1, a2, b2); // 敷居を内側に追加
              currentLine2.DoCommand(command3);
              openCommands.push_back(command3);
            }

            // 開ける
            const auto command4 = Command::Merge(0); // 外側を開ける
            currentLine2.DoCommand(command4);
            openCommands.push_back(command4);

            // 誤差を計算
            const double amount = currentLine2.wells[0].colorAmount;
            const double error = amount - targetValue;
            // if(abs(error) >= 0.1) continue;
            candidates.emplace_back(closeCommands.size() + openCommands.size(),
                                    error, putCount, actions.size());
            actions.push_back(Action(closeCommands, openCommands));
          }
        }
      }
    }
    sort(candidates.begin(), candidates.end(),
         [](const Candidate &a, const Candidate &b) {
           // 誤差が小さい順にソート
           return abs(a.error) < abs(b.error);
         });
    //  cerr << "Candidates: " << candidates.size() << endl;

    return candidates;
  }

  Vec3 GetColor() const override { return ownColorVecs[colorIndex]; }

  int GetColorIndex() const override {
    return colorIndex; // 色のインデックスを返す
  }

  Well Close(const Well centerWell, const int actionId,
             vector<RealAction> &realActions) override {
    const int oldMainCount = line.wells[0].cellCount;

    // アクションを実行
    for (const auto &command : actions[actionId].closeCommands) {
      if (command.kind == kBottlePut) {
        // 補充アクションの場合は、補充回数を増やす
        putCount++;
      }
      line.DoCommand(command, &realActions);
    }

    const int newMainCount = line.wells[0].cellCount;

    assert(oldMainCount >= newMainCount);
    // diff分、削った
    const int diff = oldMainCount - newMainCount;
    const auto [ret, _] = centerWell.Split(centerWell.cellCount - diff, diff);
    return ret;
  }

  Well Open(const Well centerWell, const int actionId,
            vector<RealAction> &realActions) override {
    // 一旦、Mainの一部を切り離した扱いにする
    auto [mainWell, subWell] =
        centerWell.Split(centerWell.cellCount - line.wells[0].cellCount,
                         line.wells[0].cellCount);
    line.SetMainWell(subWell.color, subWell.AmountPerCell());

    // アクションを実行
    const auto &action = actions[actionId];
    for (const auto &command : action.openCommands) {
      if (command.kind == kBottlePut) {
        // 補充アクションの場合は、補充回数を増やす
        putCount++;
      }
      line.DoCommand(command, &realActions);
    }

    // 再度マージする
    mainWell.Merge(line.wells[0]);
    return mainWell;
  }

  void UpdateMainAfterSubmit(const Well centerWell) override {
    // ボトルの色を更新
    line.SetMainWell(centerWell.color, centerWell.AmountPerCell());
    actions.clear();
    actions.push_back(Action({}, {})); // 何もしないアクションを追加
  }

  int MainWellCellCount() const override {
    return line.wells[0].cellCount; // 共有スペースのセル数
  }

  void Save() {
    savedLine = line;         // 現在の状態を保存
    savedPutCount = putCount; // 現在の補充回数を保存
  }

  void Load() {
    line = savedLine;         // 保存した状態を復元
    putCount = savedPutCount; // 保存した補充回数を復元
  }

  InitialPartition GetInitialPartition() const override {
    auto poses = *line.poses;
    if (isFirstVeryLargeT) {
      // 初期状態 = n, 1, 10としている
      auto partition = InitialPartition(poses);
      partition.AddPartition(poses[(int)poses.size() - kTankCap - 1],
                             poses[(int)poses.size() - kTankCap]);
      partition.AddPartition(poses[(int)poses.size() - kTankCap - 2],
                             poses[(int)poses.size() - kTankCap - 1]);
      return partition;
    } else if (isFirstSmallT) {
      // 初期状態 = 全マスとしているため、敷居が必要
      poses.erase(poses.begin());
      return InitialPartition(poses);
    } else {
      // 初期状態 = 最後の1マスとしているため、敷居が必要
      poses.pop_back();
      return InitialPartition(poses);
    }
  }

  double ColorAmountSum() const {
    double sum = 0.0;
    for (int i = 1; i < (int)line.wells.size(); ++i) {
      sum += line.wells[i].colorAmount; // 共有スペースを除く
    }
    return sum;
  }

  Well OpenAll(const Well centerWell,
               vector<RealAction> &realActions) override {
    // 先に、centerWellに対するマージを一通り行う
    auto ret = centerWell;
    for (int i = 1; i < (int)line.wells.size(); ++i) {
      ret.Merge(line.wells[i]);
    }
    // 全ての敷居を削除して、色を全て解放する
    // TODO: 実装を確認
    while (line.wells.size() > 1u) {
      // 敷居を削除
      const auto command = Command::Merge(0);
      line.DoCommand(command, &realActions);
    }
    return ret;
  }

  double Amount() const override {
    double sum = 0.0;
    reps(i, (int)line.wells.size() - 1) {
      // 共有スペースを除く
      sum += line.wells[i].colorAmount;
    }
    return sum;
  }

  double RemainAmount() const override {
    double sum = 0.0;
    reps(i, (int)line.wells.size() - 1) {
      // 共有スペースを除く
      sum += line.wells[i].colorAmount;
    }
    return sum + totalMaxPutCount - putCount; // 残りの補充回数を足す
  }
};

vector<Action> BottleNormal::actions;







/* start */

namespace Layout {
// 4レーンからなる配置
pair<vector<Pos>, vector<vector<Pos>>> FourLane() {
  vector<Pos> mainPoses = {Pos(9, 9), Pos(10, 9), Pos(10, 10), Pos(9, 10)};

  vector<vector<Pos>> lanePoses;
  {
    vector<Pos> baseLanePoses;
    bool flip = true;
    for (int y = 0; y <= 9; y++) {
      if (!flip) {
        for (int x = 0; x <= 9; ++x) {
          Pos p(x, y);
          baseLanePoses.push_back(p);
        }
      } else {
        for (int x = 9; x >= 0; --x) {
          Pos p(x, y);
          baseLanePoses.push_back(p);
        }
      }
      flip = !flip;
    }
    reverse(baseLanePoses.begin(), baseLanePoses.end());

    // 左右反転・上下反転で4通り作成
    for (const bool flipX : {false, true}) {
      for (const bool flipY : {false, true}) {
        vector<Pos> poses;
        for (const auto &p : baseLanePoses) {
          int x = p.X();
          int y = p.Y();
          if (flipX)
            x = 19 - x;
          if (flipY)
            y = 19 - y;
          assert(poses.empty() || poses.back().X() == x ||
                 poses.back().Y() == y);
          poses.push_back(Pos(x, y));
        }
        lanePoses.push_back(poses);
      }
    }
  }
  return {mainPoses, lanePoses};
}

// 5レーンからなる配置
pair<vector<Pos>, vector<vector<Pos>>> FiveLane() {
  vector<Pos> mainPoses;
  rep(y, 5) { mainPoses.emplace_back(0, y); }
  vector<vector<Pos>> lanePoses;

  rep(k, 5) {
    vector<Pos> poses;
    vector<Pos> endPoints = {
        Pos(0, k),           Pos(19 - k, k),     Pos(19 - k, 9 - k),
        Pos(4 - k, 9 - k),   Pos(4 - k, 10 + k), Pos(19 - k, 10 + k),
        Pos(19 - k, 19 - k), Pos(4 - k, 19 - k), Pos(4 - k, 19)};
    poses.push_back(endPoints[0]);
    rep(i, (int)endPoints.size() - 1) {
      const auto begin = endPoints[i];
      const auto end = endPoints[i + 1];
      if (begin == end)
        continue;
      Dir dir;
      if (begin.X() == end.X()) {
        dir = (begin.Y() < end.Y()) ? kD : kU;
      } else {
        dir = (begin.X() < end.X()) ? kR : kL;
      }
      Pos pos = begin;
      pos.Move(dir);

      while (true) {
        poses.push_back(pos);
        if (pos == end)
          break;
        pos.Move(dir);
      }
    }

    lanePoses.push_back(poses);
  }

  return {mainPoses, lanePoses};
}

// 6レーンからなる配置
pair<vector<Pos>, vector<vector<Pos>>> SixLane() {
  vector<Pos> mainPoses;
  rep(y, 6) { mainPoses.emplace_back(0, y); }
  vector<vector<Pos>> lanePoses;

  rep(k, 6) {
    vector<Pos> poses;
    vector<Pos> endPoints = {Pos(0, k),
                             Pos(19 - k, k),
                             Pos(19 - k, 11 - k),
                             Pos(5 - k, 11 - k),
                             Pos(5 - k, 12 + k),
                             Pos(19 - k, 12 + k),
                             Pos(19 - k, min(19, 23 - k))};
    if (k >= 4) {
      endPoints.push_back(Pos(0, 23 - k));
    }
    poses.push_back(endPoints[0]);
    rep(i, (int)endPoints.size() - 1) {
      const auto begin = endPoints[i];
      const auto end = endPoints[i + 1];
      if (begin == end)
        continue;
      Dir dir;
      if (begin.X() == end.X()) {
        dir = (begin.Y() < end.Y()) ? kD : kU;
      } else {
        dir = (begin.X() < end.X()) ? kR : kL;
      }
      Pos pos = begin;
      pos.Move(dir);

      while (true) {
        poses.push_back(pos);
        if (pos == end)
          break;
        pos.Move(dir);
      }
    }

    lanePoses.push_back(poses);
  }

  return {mainPoses, lanePoses};
}
} // namespace Layout





#include <array>
#include <cmath>
#include <iostream>
#include <limits>

/* start */

// 4つの色を受け取って、最適な比率を返す関数

struct OptimalColorRatioResult {
  double c1, c2, c3, c4;
  double error;
  bool isInside;

  OptimalColorRatioResult(double c1_, double c2_, double c3_, double c4_,
                          double error_, bool isInside_)
      : c1(c1_), c2(c2_), c3(c3_), c4(c4_), error(error_), isInside(isInside_) {
  }

  double GetProp(const int index) const {
    if (index == 0) {
      return c1;
    } else if (index == 1) {
      return c2;
    } else if (index == 2) {
      return c3;
    } else if (index == 3) {
      return c4;
    } else {
      assert(false); // Invalid index
      return 0;
    }
  }
};

// targetAmountは追加で作る量。centerAmountの分は含まないので注意。
OptimalColorRatioResult
OptimalColorRatio(const Vec3 target, const double targetAmount, const Vec3 c1,
                  const Vec3 c2, const Vec3 c3, const Vec3 c4,
                  const Vec3 centerColor, const double centerAmount,
                  const bool doProjection = true) {
  // ci' = [ci^T, 1]^T とし、
  // A = [c1', c2', c3', c4']とする。
  // [b^T, targetAmount]^T = b' = Axとすると、作られる色は
  // b * targetAmount / (targetAmount + centerAmount) + centerColor *
  // centerAmount / (targetAmount + centerAmount)
  // であり、これをtargetと等しくしたいので、
  // b = {target - centerColor * centerAmount / (targetAmount + centerAmount)} *
  // (targetAmount + centerAmount) / targetAmount

  vector<vector<double>> A(4, vector<double>(4, 0.0));
  A[0][0] = c1.x;
  A[1][0] = c1.y;
  A[2][0] = c1.z;
  A[3][0] = 1.0;
  A[0][1] = c2.x;
  A[1][1] = c2.y;
  A[2][1] = c2.z;
  A[3][1] = 1.0;
  A[0][2] = c3.x;
  A[1][2] = c3.y;
  A[2][2] = c3.z;
  A[3][2] = 1.0;
  A[0][3] = c4.x;
  A[1][3] = c4.y;
  A[2][3] = c4.z;
  A[3][3] = 1.0;

  vector<double> b(4, 0.0);
  b[0] = (target.x -
          centerColor.x * centerAmount / (targetAmount + centerAmount)) *
         (targetAmount + centerAmount) / targetAmount;
  b[1] = (target.y -
          centerColor.y * centerAmount / (targetAmount + centerAmount)) *
         (targetAmount + centerAmount) / targetAmount;
  b[2] = (target.z -
          centerColor.z * centerAmount / (targetAmount + centerAmount)) *
         (targetAmount + centerAmount) / targetAmount;
  b[3] = targetAmount;

  double error = 0.0;
  bool isInside = true;
  const Vec3 bVec3(b[0], b[1], b[2]);
  // bが四面体内部でない場合は、射影する
  if (doProjection && !is_inside_tetrahedron(bVec3, c1, c2, c3, c4)) {
    Vec3 projected = project_point_to_tetrahedron(bVec3, c1, c2, c3, c4);
    error = projected.distance(bVec3);
    b[0] = projected.x;
    b[1] = projected.y;
    b[2] = projected.z;
    isInside = false;
  }

  // Ax = b'を解く
  vector<double> x(4, 0.0);
  // Ax = bを解くための行列Aとベクトルbを作成
  // Aは4 x 4の行列
  // bは4のベクトル
  // ここでは単純なガウス消去法を使う
  for (int i = 0; i < 4; ++i) {
    // ピボット選択
    int pivotRow = i;
    for (int j = i + 1; j < 4; ++j) {
      if (abs(A[j][i]) > abs(A[pivotRow][i])) {
        pivotRow = j;
      }
    }
    swap(A[i], A[pivotRow]);
    swap(b[i], b[pivotRow]);

    // 前進消去
    for (int j = i + 1; j < 4; ++j) {
      double factor = A[j][i] / A[i][i];
      for (int k = i; k < 4; ++k) {
        A[j][k] -= factor * A[i][k];
      }
      b[j] -= factor * b[i];
    }
  }

  // 後退代入
  for (int i = 4 - 1; i >= 0; --i) {
    x[i] = b[i];
    for (int j = i + 1; j < 4; ++j) {
      x[i] -= A[i][j] * x[j];
    }
    x[i] /= A[i][i];
  }

  // 負の値が合ったら、0にする
  for (int i = 0; i < 4; ++i) {
    if (doProjection && x[i] < 0.0) {
      if (x[i] < -1e-6) {
        cerr << x[i] << endl;
      }
      x[i] = 0.0;
    }
  }

  return OptimalColorRatioResult(x[0], x[1], x[2], x[3], error, isInside);
}

// シンプルな単体法による線形計画ソルバー（最大化）
// maximize: c^T x
// subject to: A_ub x <= b_ub, A_eq x = b_eq, x >= 0
// 返り値: 解ベクトルx（最適解がなければ空）
inline std::vector<double>
LinearProgrammingSimplex(const std::vector<std::vector<double>> &A_ub,
                         const std::vector<double> &b_ub,
                         const std::vector<std::vector<double>> &A_eq,
                         const std::vector<double> &b_eq,
                         const std::vector<double> &c, double eps = 1e-9,
                         int max_iter = 10000) {
  int m_ub = A_ub.size(), m_eq = A_eq.size(), n = c.size();
  int m = m_ub + m_eq;
  // 拡張tableau: スラック変数 + 人工変数
  int slack = m_ub, artif = m_eq;
  int N = n + slack + artif;
  std::vector<std::vector<double>> tableau(m + 1, std::vector<double>(N + 1));
  // 不等式制約
  for (int i = 0; i < m_ub; ++i) {
    for (int j = 0; j < n; ++j)
      tableau[i][j] = A_ub[i][j];
    tableau[i][n + i] = 1.0; // スラック
    tableau[i].back() = b_ub[i];
  }
  // 等式制約
  for (int i = 0; i < m_eq; ++i) {
    for (int j = 0; j < n; ++j)
      tableau[m_ub + i][j] = A_eq[i][j];
    tableau[m_ub + i][n + slack + i] = 1.0; // 人工変数
    tableau[m_ub + i].back() = b_eq[i];
  }
  // 目的関数
  for (int j = 0; j < n; ++j)
    tableau[m][j] = -c[j];
  // 人工変数の目的関数（第1段階）
  std::vector<double> obj1(N + 1);
  for (int i = 0; i < artif; ++i)
    obj1[n + slack + i] = 1.0;
  // 基底
  std::vector<int> basis(m);
  for (int i = 0; i < m_ub; ++i)
    basis[i] = n + i;
  for (int i = 0; i < m_eq; ++i)
    basis[m_ub + i] = n + slack + i;
  // 第1段階: 人工変数の和を最小化
  for (int i = 0; i < m; ++i)
    if (basis[i] >= n + slack) {
      for (int j = 0; j <= N; ++j)
        tableau[m][j] += tableau[i][j];
    }
  for (int iter = 0; iter < max_iter; ++iter) {
    int pivot_col = -1;
    for (int j = 0; j < N; ++j)
      if (tableau[m][j] < -eps) {
        pivot_col = j;
        break;
      }
    if (pivot_col == -1)
      break;
    int pivot_row = -1;
    double min_ratio = 1e20;
    for (int i = 0; i < m; ++i) {
      if (tableau[i][pivot_col] > eps) {
        double ratio = tableau[i].back() / tableau[i][pivot_col];
        if (ratio < min_ratio) {
          min_ratio = ratio;
          pivot_row = i;
        }
      }
    }
    if (pivot_row == -1)
      return {};
    double piv = tableau[pivot_row][pivot_col];
    for (double &x : tableau[pivot_row])
      x /= piv;
    for (int i = 0; i <= m; ++i)
      if (i != pivot_row) {
        double fac = tableau[i][pivot_col];
        for (int j = 0; j <= N; ++j)
          tableau[i][j] -= fac * tableau[pivot_row][j];
      }
    basis[pivot_row] = pivot_col;
  }
  // 人工変数が0でなければ実行不能
  for (int i = 0; i < artif; ++i)
    if (std::abs(tableau[m][n + slack + i]) > eps)
      return {};
  // 第2段階: 本来の目的関数に切り替え
  for (int j = 0; j < N; ++j)
    tableau[m][j] = 0;
  for (int j = 0; j < n; ++j)
    tableau[m][j] = -c[j];
  for (int i = 0; i < m; ++i)
    if (basis[i] < n) {
      for (int j = 0; j <= N; ++j)
        tableau[m][j] += tableau[i][j] * c[basis[i]];
    }
  for (int iter = 0; iter < max_iter; ++iter) {
    int pivot_col = -1;
    for (int j = 0; j < N; ++j)
      if (tableau[m][j] < -eps) {
        pivot_col = j;
        break;
      }
    if (pivot_col == -1)
      break;
    int pivot_row = -1;
    double min_ratio = 1e20;
    for (int i = 0; i < m; ++i) {
      if (tableau[i][pivot_col] > eps) {
        double ratio = tableau[i].back() / tableau[i][pivot_col];
        if (ratio < min_ratio) {
          min_ratio = ratio;
          pivot_row = i;
        }
      }
    }
    if (pivot_row == -1)
      return {};
    double piv = tableau[pivot_row][pivot_col];
    for (double &x : tableau[pivot_row])
      x /= piv;
    for (int i = 0; i <= m; ++i)
      if (i != pivot_row) {
        double fac = tableau[i][pivot_col];
        for (int j = 0; j <= N; ++j)
          tableau[i][j] -= fac * tableau[pivot_row][j];
      }
    basis[pivot_row] = pivot_col;
  }
  // 解の復元
  std::vector<double> x(n);
  for (int i = 0; i < m; ++i)
    if (basis[i] < n)
      x[basis[i]] = tableau[i].back();
  return x;
}



/* start */

namespace Solve4 {

vector<pair<double, int>> debug;
double stepMsSum = 0.0;
int stepCount = 0;

struct State {
  Well mainWell;
  vector<Pos> mainPoses;
  vector<Bottle *> bottles;
  int totalPutCount = 0;

  State(vector<Pos> mainPoses, vector<Bottle *> bottles_)
      : mainPoses(mainPoses), bottles(bottles_) {
    int mainWellCellCount = 0;
    for (const auto &bottle : bottles) {
      mainWellCellCount += bottle->MainWellCellCount();
    }

    mainWell = Well(0.0, mainWellCellCount, Color{0.0, 0.0, 0.0});
  }

  InitialPartition GetInitialPartition() const {
    InitialPartition partition;
    for (const auto &bottle : bottles) {
      partition.Merge(bottle->GetInitialPartition());
    }
    partition.Merge(InitialPartition(mainPoses));
    return partition;
  }

  // Putできなかったらfalseを返す
  bool Step(vector<RealAction> &realActions, const int targetIndex) {
    if (T <= 18000 && targetIndex > (18000 - T) / 4 + 20) {
      for (auto &bottle : bottles) {
        bottle->SetLargeT();
      }
    } else if (targetIndex > (34000 - T) / 16 + 20) {
      for (auto &bottle : bottles) {
        bottle->SetVeryLargeT();
      }
    }

    const int halfCount =
        (targetIndex < 500 ||
        (stepMsSum / stepCount) * (H - targetIndex) + global_timer.ms() < 2750)
            ? 4
            : 3;

    const auto &targetColor = targetColorVecs[targetIndex];

    // ボトルが5個以上ある場合は、目的の点を含む4つのうち面積が最小となるものを選ぶ
    if (bottles.size() >= 5u) {
      vector<int> bestBottleIndices;
      vector<int> currentBottleIndices;
      double minCost = LINF;
      const auto func = [&](auto self) {
        if (currentBottleIndices.size() == 4u) {
          const auto color0 = bottles[currentBottleIndices[0]]->GetColor();
          const auto color1 = bottles[currentBottleIndices[1]]->GetColor();
          const auto color2 = bottles[currentBottleIndices[2]]->GetColor();
          const auto color3 = bottles[currentBottleIndices[3]]->GetColor();
          // 目的の点を含むか
          const bool contains = is_inside_tetrahedron(targetColor, color0,
                                                      color1, color2, color3);
          if (contains) {
            // 含むなら、体積が小さいほうが良い
            const auto volume =
                tetrahedron_volume(color0, color1, color2, color3);
            if (volume < minCost) {
              minCost = volume;
              bestBottleIndices = currentBottleIndices;
            }
          } else {
            // 含まないなら、近い方が良い
            // NOTE: どれにも含まれない場合は三角形単位で見れば十分なので、無駄
            const auto projected = project_point_to_tetrahedron(
                targetColor, color0, color1, color2, color3);
            const auto distance = projected.distance(targetColor);
            const auto cost = distance * 1e5;
            if (cost < minCost) {
              minCost = cost;
              bestBottleIndices = currentBottleIndices;
            }
          }
          return;
        }

        // DFS
        for (int i = currentBottleIndices.empty()
                         ? 0
                         : currentBottleIndices.back() + 1;
             i < (int)bottles.size(); ++i) {
          currentBottleIndices.push_back(i);
          self(self);
          currentBottleIndices.pop_back();
        }
      };

      func(func);

      sort(bestBottleIndices.begin(), bestBottleIndices.end());
      // ベストな4つのボトルが先頭にくるようにする
      rep(i, 4) { swap(bottles[i], bottles[bestBottleIndices[i]]); }
    }

    // 4つのボトルの色を取得
    vector<Vec3> colors;
    rep(k, 4) { colors.push_back(bottles[k]->GetColor()); }

    // 最適な配分を求める
    auto result = OptimalColorRatio(
        targetColor, 1.0, colors[0], colors[1], colors[2], colors[3],
        mainWell.color.ToVec3(), mainWell.colorAmount);

    // 各ボトルのアクションを求める
    vector<vector<Candidate>> candidates;
    // cerr << "target [ ";
    // rep(k, 4) { cerr << result.GetProp(k) << " "; }
    // cerr << "]" << endl;
    // cerr << "error [ ";
    rep(k, 4) {
      candidates.push_back(bottles[k]->Candidates(result.GetProp(k)));
      //   cerr << abs(candidates[k][0].error) << " ";
    }
    // cerr << "]" << endl;

    rep(k, 4) { bottles[k]->Save(); }

    // 各ボトルのアクション上位4つの組み合わせを全て試す
    vector<int> candidateIndices;
    vector<int> bestCandidateIndices;
    double minError = INF;

    const auto eval = [&](const vector<int> &indices, Well &currentMainWell,
                          vector<RealAction> &currentRealActions) {
      rep(k, 4) { bottles[k]->Load(); }
      rep(k, 4) {
        auto &bottle = bottles[k];
        currentMainWell =
            bottle->Close(currentMainWell, candidates[k][indices[k]].actionId,
                          currentRealActions);
      }

      // ボトルを開く
      rep(k, 4) {
        auto &bottle = bottles[k];
        currentMainWell =
            bottle->Open(currentMainWell, candidates[k][indices[k]].actionId,
                         currentRealActions);
      }

      // 提出する
      if (currentMainWell.CanSubmit()) {
        const int newTotalPutCount =
            totalPutCount +
            accumulate(currentRealActions.begin(), currentRealActions.end(), 0,
                       [](int sum, const RealAction &action) {
                         return sum + (action.kind == RealActionKind::kPut);
                       });
        // TODO: Dの閾値を色々試す
        // NOTE: 常にfalse
        if (newTotalPutCount > H && D >= 500) {
          return (double)INF;
        }
        currentRealActions.emplace_back(
            RealAction::Submit(mainPoses[0], targetIndex));
        const auto error =
            currentMainWell.GetError(targetColorVecs[targetIndex]);
        // cerr << "Error: " << error << endl;
        return error;
      }

      return (double)INF;
    };

    const auto func = [&](auto self, const double errorSum) {
      if (candidateIndices.size() == 4) {
        // 評価
        // ボトルを閉じる
        if (errorSum + mainWell.colorAmount < 0.0) {
          return;
        }
        vector<RealAction> currentRealActions;
        auto currentMainWell = mainWell;
        const auto error =
            eval(candidateIndices, currentMainWell, currentRealActions);
        if (error < minError) {
          minError = error;
          bestCandidateIndices = candidateIndices;
        }

        return;
      }

      int positiveErrorCount = 0;
      int negativeErrorCount = 0;
      const auto &thisCandidates = candidates[candidateIndices.size()];
      for (int i = 0; i < min((int)thisCandidates.size(), halfCount * 100);
           ++i) {
        if (positiveErrorCount + negativeErrorCount >= halfCount * 2) {
          break;
        }
        const auto error = thisCandidates[i].error;
        if (error < 0) {
          if (negativeErrorCount == halfCount) {
            continue; // ネガティブエラーは半分まで
          }
          negativeErrorCount++;
        } else {
          if (positiveErrorCount == halfCount) {
            continue; // ポジティブエラーは半分まで
          }
          positiveErrorCount++;
        }
        candidateIndices.push_back(i);
        self(self, errorSum + error);
        candidateIndices.pop_back();
      }
      if (positiveErrorCount == 0) {
        // ポジティブも1つは試す
        rep(i, thisCandidates.size()) {
          if (thisCandidates[i].error > 0) {
            candidateIndices.push_back(i);
            self(self, errorSum + thisCandidates[i].error);
            candidateIndices.pop_back();
            positiveErrorCount++;
            break;
          }
        }
      }
      if (negativeErrorCount == 0) {
        // ネガティブも1つは試す
        rep(i, thisCandidates.size()) {
          if (thisCandidates[i].error < 0) {
            candidateIndices.push_back(i);
            self(self, errorSum + thisCandidates[i].error);
            candidateIndices.pop_back();
            negativeErrorCount++;
            break;
          }
        }
      }
    };

    func(func, 0.0);

    if (minError == INF) {
      // putできないので、終わり
      rep(k, 4) { bottles[k]->Load(); }
      return false;
    }
    debug.emplace_back(minError, targetIndex);
    vector<RealAction> newRealActions;
    eval(bestCandidateIndices, mainWell, newRealActions);
    // cerr << "selected error [ ";
    // rep(k, 4){
    //   const auto& cand = candidates[k][bestCandidateIndices[k]];
    //   cerr << cand.error << " ";
    // }
    // cerr << "]" << endl;
    // cerr << "t = " << targetIndex << ", minError = " << minError << endl;
    realActions.insert(realActions.end(), newRealActions.begin(),
                       newRealActions.end());
    for (const auto action : newRealActions) {
      totalPutCount += action.kind == RealActionKind::kPut;
    }
    mainWell.Pick();
    // cerr << "mainWell colorAmount: " << mainWell.colorAmount
    //      << endl;
    rep(k, bottles.size()) {
      auto &bottle = bottles[k];
      bottle->UpdateMainAfterSubmit(mainWell);
      bottle->Save();
      // cerr << "bottle[" << k << "] colorAmount: "
      //      << bottle->Amount() << endl;
    }
    return true;
  }
};

class Solver {
public:
  Solver() {}
  State InitState() {
    const auto [mainPoses, lanePoses] =
        bestOutsideErrorFour <= 100   ? Layout::FourLane()
        : bestOutsideErrorFive <= 100 ? Layout::FiveLane()
                                      : Layout::SixLane();

    vector<Bottle *> bottles;
    const auto &colorIndicies = lanePoses.size() == 4u   ? bestColorIndexesFour
                                : lanePoses.size() == 5u ? bestColorIndexesFive
                                                         : bestColorIndexesSix;

    int colorIndex = 0;
    int largableCount = T >= 18000 ? INF : (T - 10000) / 2000;
    for (const auto &poses : lanePoses) {
      bottles.push_back(new BottleNormal(poses, colorIndicies[colorIndex++],
                                         INF, T < 18000, T >= 32000));
      largableCount--;
    }

    State state(mainPoses, bottles);
    return state;
  }

  bool Solve() {
    State state = InitState();
    vector<RealAction> realActions;
    int t = 0;
    for (; t < H; ++t) {
      double statrtMs = global_timer.ms();
      const auto success = state.Step(realActions, t);
      stepCount++;
      stepMsSum += global_timer.ms() - statrtMs;
      if (!success) {
        break;
      }
    }

    // 最後に全てのボトルを開く
    rep(k, state.bottles.size()) {
      auto &bottle = state.bottles[k];
      state.mainWell = bottle->OpenAll(state.mainWell, realActions);
    }
    cerr << "remained mainWell colorAmount: " << state.mainWell.colorAmount
         << endl;

    // 全探索
    const int remainPutCount = max(0, H - state.totalPutCount);
    const int remainSubmitCount = H - t;
    if (remainSubmitCount > 0) {
      cerr << "remainPutCount: " << remainPutCount
           << ", remainSubmitCount: " << remainSubmitCount << endl;
      // -1は提出を表す
      vector<int> actionIndices;
      vector<RealAction> bestRealActions;
      double minErrorSum = INF;
      const auto func = [&](auto self, const int putCount,
                            const int submitCount) {
        if (putCount > remainPutCount) {
          return; // Put数が多すぎるので終了
        }
        if (submitCount > remainSubmitCount) {
          return; // 提出数が多すぎるので終了
        }
        if ((int)actionIndices.size() == remainPutCount + remainSubmitCount) {
          if (submitCount != remainSubmitCount) {
            return; // 提出数が合わないので終了
          }
          // 評価
          int targetIndex = t;
          auto mainWell = state.mainWell;
          double errorSum = 0.0;
          vector<RealAction> currentRealActions;
          for (const auto actionIndex : actionIndices) {
            if (actionIndex == -1) {
              // 提出
              if (!mainWell.CanSubmit()) {
                return; // 提出できないので終了
              }
              const double error =
                  mainWell.color.Distance(targetColorVecs[targetIndex]);
              errorSum += error;
              mainWell.Pick();
              currentRealActions.emplace_back(
                  RealAction::Submit(state.mainPoses[0], targetIndex));
              targetIndex++;
            } else {
              // Put
              const int colorIndex = actionIndex;
              mainWell.Put(ownColors[colorIndex]);
              currentRealActions.emplace_back(
                  RealAction::Put(state.mainPoses[0], colorIndex));
            }
          }

          if (errorSum < minErrorSum) {
            minErrorSum = errorSum;
            bestRealActions = currentRealActions;
          }
          return;
        }

        for (int i = -1; i < K; ++i) {
          actionIndices.push_back(i);
          self(self, putCount + (i != -1 ? 1 : 0),
               submitCount + (i == -1 ? 1 : 0));
          actionIndices.pop_back();
        }
      };

      func(func, 0, 0);

      realActions.insert(realActions.end(), bestRealActions.begin(),
                         bestRealActions.end());
    }

    // 提出アクションの数がTを超えている場合は失敗
    if ((int)realActions.size() > T) {
      cerr << "Too many actions: " << realActions.size() << endl;
      Print(state.GetInitialPartition());
      Print(realActions);
      exit(0);
      // return false;
    }

    Print(state.GetInitialPartition());
    Print(realActions);

    // sort(debug.begin(), debug.end());
    // reverse(debug.begin(), debug.end());
    // cerr << "Debug info:" << endl;
    // rep(t, 10) {
    //   cerr << "t = " << debug[t].second << ", error = " << debug[t].first
    //        << endl;
    // }
    // cerr << "25%: " << debug[debug.size() * 25 / 100].first
    //      << ", 50%: " << debug[debug.size() * 50 / 100].first
    //      << ", 75%: " << debug[debug.size() * 75 / 100].first
    //      << ", 90%: " << debug[debug.size() * 90 / 100].first << endl;
    cerr << global_timer.ms() << "[ms]" << endl;
    return true;
  }
};
} // namespace Solve4
















#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

#if !defined(_ARRAY_HH)
#define _ARRAY_HH

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <set>
#include <stdexcept>

/* start */

/*
File $Id: QuadProg++.cc 232 2007-06-21 12:29:00Z digasper $

 Author: Luca Di Gaspero
 DIEGM - University of Udine, Italy
 luca.digaspero@uniud.it
 http://www.diegm.uniud.it/digaspero/

 This software may be modified and distributed under the terms
 of the MIT license.  See the LICENSE file for details.

 */

// $Id: Array.hh 249 2008-11-20 09:58:23Z schaerf $
// This file is part of EasyLocalpp: a C++ Object-Oriented framework
// aimed at easing the development of Local Search algorithms.
// Copyright (C) 2001--2008 Andrea Schaerf, Luca Di Gaspero.
//
// This software may be modified and distributed under the terms
// of the MIT license.  See the LICENSE file for details.

namespace quadprogpp {

enum MType { DIAG };

template <typename T> class Vector {
public:
  Vector();
  Vector(const unsigned int n);
  Vector(const T &a, const unsigned int n); // initialize to constant value
  Vector(const T *a, const unsigned int n); // Initialize to array
  Vector(const Vector &rhs);                // copy constructor
  ~Vector();                                // destructor

  inline void set(const T *a, const unsigned int n);
  Vector<T> extract(const std::set<unsigned int> &indexes) const;
  inline T &operator[](const unsigned int &i); // i-th element
  inline const T &operator[](const unsigned int &i) const;

  inline unsigned int size() const;
  inline void resize(const unsigned int n);
  inline void resize(const T &a, const unsigned int n);

  Vector<T> &operator=(const Vector<T> &rhs); // assignment
  Vector<T> &operator=(const T &a);           // assign a to every element
  inline Vector<T> &operator+=(const Vector<T> &rhs);
  inline Vector<T> &operator-=(const Vector<T> &rhs);
  inline Vector<T> &operator*=(const Vector<T> &rhs);
  inline Vector<T> &operator/=(const Vector<T> &rhs);
  inline Vector<T> &operator^=(const Vector<T> &rhs);
  inline Vector<T> &operator+=(const T &a);
  inline Vector<T> &operator-=(const T &a);
  inline Vector<T> &operator*=(const T &a);
  inline Vector<T> &operator/=(const T &a);
  inline Vector<T> &operator^=(const T &a);

private:
  unsigned int n; // size of array. upper index is n-1
  T *v;           // storage for data
};

template <typename T> Vector<T>::Vector() : n(0), v(0) {}

template <typename T> Vector<T>::Vector(const unsigned int n) : v(new T[n]) {
  this->n = n;
}

template <typename T>
Vector<T>::Vector(const T &a, const unsigned int n) : v(new T[n]) {
  this->n = n;
  for (unsigned int i = 0; i < n; i++)
    v[i] = a;
}

template <typename T>
Vector<T>::Vector(const T *a, const unsigned int n) : v(new T[n]) {
  this->n = n;
  for (unsigned int i = 0; i < n; i++)
    v[i] = *a++;
}

template <typename T>
Vector<T>::Vector(const Vector<T> &rhs) : v(new T[rhs.n]) {
  this->n = rhs.n;
  for (unsigned int i = 0; i < n; i++)
    v[i] = rhs[i];
}

template <typename T> Vector<T>::~Vector() {
  if (v != 0)
    delete[] (v);
}

template <typename T> void Vector<T>::resize(const unsigned int n) {
  if (n == this->n)
    return;
  if (v != 0)
    delete[] (v);
  v = new T[n];
  this->n = n;
}

template <typename T> void Vector<T>::resize(const T &a, const unsigned int n) {
  resize(n);
  for (unsigned int i = 0; i < n; i++)
    v[i] = a;
}

template <typename T>
inline Vector<T> &Vector<T>::operator=(const Vector<T> &rhs)
// postcondition: normal assignment via copying has been performed;
// if vector and rhs were different sizes, vector
// has been resized to match the size of rhs
{
  if (this != &rhs) {
    resize(rhs.n);
    for (unsigned int i = 0; i < n; i++)
      v[i] = rhs[i];
  }
  return *this;
}

template <typename T>
inline Vector<T> &Vector<T>::operator=(const T &a) // assign a to every element
{
  for (unsigned int i = 0; i < n; i++)
    v[i] = a;
  return *this;
}

template <typename T>
inline T &Vector<T>::operator[](const unsigned int &i) // subscripting
{
  return v[i];
}

template <typename T>
inline const T &
Vector<T>::operator[](const unsigned int &i) const // subscripting
{
  return v[i];
}

template <typename T> inline unsigned int Vector<T>::size() const { return n; }

template <typename T> inline void Vector<T>::set(const T *a, unsigned int n) {
  resize(n);
  for (unsigned int i = 0; i < n; i++)
    v[i] = a[i];
}

template <typename T>
inline Vector<T>
Vector<T>::extract(const std::set<unsigned int> &indexes) const {
  Vector<T> tmp(indexes.size());
  unsigned int i = 0;

  for (std::set<unsigned int>::const_iterator el = indexes.begin();
       el != indexes.end(); el++) {
    if (*el >= n)
      throw std::logic_error(
          "Error extracting subvector: the indexes are out of vector bounds");
    tmp[i++] = v[*el];
  }

  return tmp;
}

template <typename T>
inline Vector<T> &Vector<T>::operator+=(const Vector<T> &rhs) {
  if (this->size() != rhs.size())
    throw std::logic_error("Operator+=: vectors have different sizes");
  for (unsigned int i = 0; i < n; i++)
    v[i] += rhs[i];

  return *this;
}

template <typename T> inline Vector<T> &Vector<T>::operator+=(const T &a) {
  for (unsigned int i = 0; i < n; i++)
    v[i] += a;

  return *this;
}

template <typename T> inline Vector<T> operator+(const Vector<T> &rhs) {
  return rhs;
}

template <typename T>
inline Vector<T> operator+(const Vector<T> &lhs, const Vector<T> &rhs) {
  if (lhs.size() != rhs.size())
    throw std::logic_error("Operator+: vectors have different sizes");
  Vector<T> tmp(lhs.size());
  for (unsigned int i = 0; i < lhs.size(); i++)
    tmp[i] = lhs[i] + rhs[i];

  return tmp;
}

template <typename T>
inline Vector<T> operator+(const Vector<T> &lhs, const T &a) {
  Vector<T> tmp(lhs.size());
  for (unsigned int i = 0; i < lhs.size(); i++)
    tmp[i] = lhs[i] + a;

  return tmp;
}

template <typename T>
inline Vector<T> operator+(const T &a, const Vector<T> &rhs) {
  Vector<T> tmp(rhs.size());
  for (unsigned int i = 0; i < rhs.size(); i++)
    tmp[i] = a + rhs[i];

  return tmp;
}

template <typename T>
inline Vector<T> &Vector<T>::operator-=(const Vector<T> &rhs) {
  if (this->size() != rhs.size())
    throw std::logic_error("Operator-=: vectors have different sizes");
  for (unsigned int i = 0; i < n; i++)
    v[i] -= rhs[i];

  return *this;
}

template <typename T> inline Vector<T> &Vector<T>::operator-=(const T &a) {
  for (unsigned int i = 0; i < n; i++)
    v[i] -= a;

  return *this;
}

template <typename T> inline Vector<T> operator-(const Vector<T> &rhs) {
  return (T)(-1) * rhs;
}

template <typename T>
inline Vector<T> operator-(const Vector<T> &lhs, const Vector<T> &rhs) {
  if (lhs.size() != rhs.size())
    throw std::logic_error("Operator-: vectors have different sizes");
  Vector<T> tmp(lhs.size());
  for (unsigned int i = 0; i < lhs.size(); i++)
    tmp[i] = lhs[i] - rhs[i];

  return tmp;
}

template <typename T>
inline Vector<T> operator-(const Vector<T> &lhs, const T &a) {
  Vector<T> tmp(lhs.size());
  for (unsigned int i = 0; i < lhs.size(); i++)
    tmp[i] = lhs[i] - a;

  return tmp;
}

template <typename T>
inline Vector<T> operator-(const T &a, const Vector<T> &rhs) {
  Vector<T> tmp(rhs.size());
  for (unsigned int i = 0; i < rhs.size(); i++)
    tmp[i] = a - rhs[i];

  return tmp;
}

template <typename T>
inline Vector<T> &Vector<T>::operator*=(const Vector<T> &rhs) {
  if (this->size() != rhs.size())
    throw std::logic_error("Operator*=: vectors have different sizes");
  for (unsigned int i = 0; i < n; i++)
    v[i] *= rhs[i];

  return *this;
}

template <typename T> inline Vector<T> &Vector<T>::operator*=(const T &a) {
  for (unsigned int i = 0; i < n; i++)
    v[i] *= a;

  return *this;
}

template <typename T>
inline Vector<T> operator*(const Vector<T> &lhs, const Vector<T> &rhs) {
  if (lhs.size() != rhs.size())
    throw std::logic_error("Operator*: vectors have different sizes");
  Vector<T> tmp(lhs.size());
  for (unsigned int i = 0; i < lhs.size(); i++)
    tmp[i] = lhs[i] * rhs[i];

  return tmp;
}

template <typename T>
inline Vector<T> operator*(const Vector<T> &lhs, const T &a) {
  Vector<T> tmp(lhs.size());
  for (unsigned int i = 0; i < lhs.size(); i++)
    tmp[i] = lhs[i] * a;

  return tmp;
}

template <typename T>
inline Vector<T> operator*(const T &a, const Vector<T> &rhs) {
  Vector<T> tmp(rhs.size());
  for (unsigned int i = 0; i < rhs.size(); i++)
    tmp[i] = a * rhs[i];

  return tmp;
}

template <typename T>
inline Vector<T> &Vector<T>::operator/=(const Vector<T> &rhs) {
  if (this->size() != rhs.size())
    throw std::logic_error("Operator/=: vectors have different sizes");
  for (unsigned int i = 0; i < n; i++)
    v[i] /= rhs[i];

  return *this;
}

template <typename T> inline Vector<T> &Vector<T>::operator/=(const T &a) {
  for (unsigned int i = 0; i < n; i++)
    v[i] /= a;

  return *this;
}

template <typename T>
inline Vector<T> operator/(const Vector<T> &lhs, const Vector<T> &rhs) {
  if (lhs.size() != rhs.size())
    throw std::logic_error("Operator/: vectors have different sizes");
  Vector<T> tmp(lhs.size());
  for (unsigned int i = 0; i < lhs.size(); i++)
    tmp[i] = lhs[i] / rhs[i];

  return tmp;
}

template <typename T>
inline Vector<T> operator/(const Vector<T> &lhs, const T &a) {
  Vector<T> tmp(lhs.size());
  for (unsigned int i = 0; i < lhs.size(); i++)
    tmp[i] = lhs[i] / a;

  return tmp;
}

template <typename T>
inline Vector<T> operator/(const T &a, const Vector<T> &rhs) {
  Vector<T> tmp(rhs.size());
  for (unsigned int i = 0; i < rhs.size(); i++)
    tmp[i] = a / rhs[i];

  return tmp;
}

template <typename T>
inline Vector<T> operator^(const Vector<T> &lhs, const Vector<T> &rhs) {
  if (lhs.size() != rhs.size())
    throw std::logic_error("Operator^: vectors have different sizes");
  Vector<T> tmp(lhs.size());
  for (unsigned int i = 0; i < lhs.size(); i++)
    tmp[i] = pow(lhs[i], rhs[i]);

  return tmp;
}

template <typename T>
inline Vector<T> operator^(const Vector<T> &lhs, const T &a) {
  Vector<T> tmp(lhs.size());
  for (unsigned int i = 0; i < lhs.size(); i++)
    tmp[i] = pow(lhs[i], a);

  return tmp;
}

template <typename T>
inline Vector<T> operator^(const T &a, const Vector<T> &rhs) {
  Vector<T> tmp(rhs.size());
  for (unsigned int i = 0; i < rhs.size(); i++)
    tmp[i] = pow(a, rhs[i]);

  return tmp;
}

template <typename T>
inline Vector<T> &Vector<T>::operator^=(const Vector<T> &rhs) {
  if (this->size() != rhs.size())
    throw std::logic_error("Operator^=: vectors have different sizes");
  for (unsigned int i = 0; i < n; i++)
    v[i] = pow(v[i], rhs[i]);

  return *this;
}

template <typename T> inline Vector<T> &Vector<T>::operator^=(const T &a) {
  for (unsigned int i = 0; i < n; i++)
    v[i] = pow(v[i], a);

  return *this;
}

template <typename T>
inline bool operator==(const Vector<T> &v, const Vector<T> &w) {
  if (v.size() != w.size())
    throw std::logic_error("Vectors of different size are not confrontable");
  for (unsigned i = 0; i < v.size(); i++)
    if (v[i] != w[i])
      return false;
  return true;
}

template <typename T>
inline bool operator!=(const Vector<T> &v, const Vector<T> &w) {
  if (v.size() != w.size())
    throw std::logic_error("Vectors of different size are not confrontable");
  for (unsigned i = 0; i < v.size(); i++)
    if (v[i] != w[i])
      return true;
  return false;
}

template <typename T>
inline bool operator<(const Vector<T> &v, const Vector<T> &w) {
  if (v.size() != w.size())
    throw std::logic_error("Vectors of different size are not confrontable");
  for (unsigned i = 0; i < v.size(); i++)
    if (v[i] >= w[i])
      return false;
  return true;
}

template <typename T>
inline bool operator<=(const Vector<T> &v, const Vector<T> &w) {
  if (v.size() != w.size())
    throw std::logic_error("Vectors of different size are not confrontable");
  for (unsigned i = 0; i < v.size(); i++)
    if (v[i] > w[i])
      return false;
  return true;
}

template <typename T>
inline bool operator>(const Vector<T> &v, const Vector<T> &w) {
  if (v.size() != w.size())
    throw std::logic_error("Vectors of different size are not confrontable");
  for (unsigned i = 0; i < v.size(); i++)
    if (v[i] <= w[i])
      return false;
  return true;
}

template <typename T>
inline bool operator>=(const Vector<T> &v, const Vector<T> &w) {
  if (v.size() != w.size())
    throw std::logic_error("Vectors of different size are not confrontable");
  for (unsigned i = 0; i < v.size(); i++)
    if (v[i] < w[i])
      return false;
  return true;
}

/**
   Input/Output
*/
template <typename T>
inline std::ostream &operator<<(std::ostream &os, const Vector<T> &v) {
  os << std::endl << v.size() << std::endl;
  for (unsigned int i = 0; i < v.size() - 1; i++)
    os << std::setw(20) << std::setprecision(16) << v[i] << ", ";
  os << std::setw(20) << std::setprecision(16) << v[v.size() - 1] << std::endl;

  return os;
}

template <typename T> std::istream &operator>>(std::istream &is, Vector<T> &v) {
  int elements;
  char comma;
  is >> elements;
  v.resize(elements);
  for (unsigned int i = 0; i < elements; i++)
    is >> v[i] >> comma;

  return is;
}

/**
   Index utilities
*/

std::set<unsigned int> seq(unsigned int s, unsigned int e);

std::set<unsigned int> singleton(unsigned int i);

template <typename T> class CanonicalBaseVector : public Vector<T> {
public:
  CanonicalBaseVector(unsigned int i, unsigned int n);
  inline void reset(unsigned int i);

private:
  unsigned int e;
};

template <typename T>
CanonicalBaseVector<T>::CanonicalBaseVector(unsigned int i, unsigned int n)
    : Vector<T>((T)0, n), e(i) {
  (*this)[e] = (T)1;
}

template <typename T>
inline void CanonicalBaseVector<T>::reset(unsigned int i) {
  (*this)[e] = (T)0;
  e = i;
  (*this)[e] = (T)1;
}

#include <stdexcept>

template <typename T> inline T sum(const Vector<T> &v) {
  T tmp = (T)0;
  for (unsigned int i = 0; i < v.size(); i++)
    tmp += v[i];

  return tmp;
}

template <typename T> inline T prod(const Vector<T> &v) {
  T tmp = (T)1;
  for (unsigned int i = 0; i < v.size(); i++)
    tmp *= v[i];

  return tmp;
}

template <typename T> inline T mean(const Vector<T> &v) {
  T sum = (T)0;
  for (unsigned int i = 0; i < v.size(); i++)
    sum += v[i];
  return sum / v.size();
}

template <typename T> inline T median(const Vector<T> &v) {
  Vector<T> tmp = sort(v);
  if (v.size() % 2 == 1) // it is an odd-sized vector
    return tmp[v.size() / 2];
  else
    return 0.5 * (tmp[v.size() / 2 - 1] + tmp[v.size() / 2]);
}

template <typename T>
inline T stdev(const Vector<T> &v, bool sample_correction = false) {
  return sqrt(var(v, sample_correction));
}

template <typename T>
inline T var(const Vector<T> &v, bool sample_correction = false) {
  T sum = (T)0, ssum = (T)0;
  unsigned int n = v.size();
  for (unsigned int i = 0; i < n; i++) {
    sum += v[i];
    ssum += (v[i] * v[i]);
  }
  if (!sample_correction)
    return (ssum / n) - (sum / n) * (sum / n);
  else
    return n * ((ssum / n) - (sum / n) * (sum / n)) / (n - 1);
}

template <typename T> inline T max(const Vector<T> &v) {
  T value = v[0];
  for (unsigned int i = 1; i < v.size(); i++)
    value = std::max(v[i], value);

  return value;
}

template <typename T> inline T min(const Vector<T> &v) {
  T value = v[0];
  for (unsigned int i = 1; i < v.size(); i++)
    value = std::min(v[i], value);

  return value;
}

template <typename T> inline unsigned int index_max(const Vector<T> &v) {
  unsigned int max = 0;
  for (unsigned int i = 1; i < v.size(); i++)
    if (v[i] > v[max])
      max = i;

  return max;
}

template <typename T> inline unsigned int index_min(const Vector<T> &v) {
  unsigned int min = 0;
  for (unsigned int i = 1; i < v.size(); i++)
    if (v[i] < v[min])
      min = i;

  return min;
}

template <typename T>
inline T dot_prod(const Vector<T> &a, const Vector<T> &b) {
  T sum = (T)0;
  if (a.size() != b.size())
    throw std::logic_error("Dotprod error: the vectors are not the same size");
  for (unsigned int i = 0; i < a.size(); i++)
    sum += a[i] * b[i];

  return sum;
}

/**
   Single element mathematical functions
*/

template <typename T> inline Vector<T> exp(const Vector<T> &v) {
  Vector<T> tmp(v.size());
  for (unsigned int i = 0; i < v.size(); i++)
    tmp[i] = exp(v[i]);

  return tmp;
}

template <typename T> inline Vector<T> log(const Vector<T> &v) {
  Vector<T> tmp(v.size());
  for (unsigned int i = 0; i < v.size(); i++)
    tmp[i] = log(v[i]);

  return tmp;
}

template <typename T> inline Vector<T> vec_sqrt(const Vector<T> &v) {
  Vector<T> tmp(v.size());
  for (unsigned int i = 0; i < v.size(); i++)
    tmp[i] = sqrt(v[i]);

  return tmp;
}

template <typename T> inline Vector<T> pow(const Vector<T> &v, double a) {
  Vector<T> tmp(v.size());
  for (unsigned int i = 0; i < v.size(); i++)
    tmp[i] = pow(v[i], a);

  return tmp;
}

template <typename T> inline Vector<T> abs(const Vector<T> &v) {
  Vector<T> tmp(v.size());
  for (unsigned int i = 0; i < v.size(); i++)
    tmp[i] = (T)fabs(v[i]);

  return tmp;
}

template <typename T> inline Vector<T> sign(const Vector<T> &v) {
  Vector<T> tmp(v.size());
  for (unsigned int i = 0; i < v.size(); i++)
    tmp[i] = v[i] > 0 ? +1 : v[i] == 0 ? 0 : -1;

  return tmp;
}

template <typename T>
inline unsigned int partition(Vector<T> &v, unsigned int begin,
                              unsigned int end) {
  unsigned int i = begin + 1, j = begin + 1;
  T pivot = v[begin];
  while (j <= end) {
    if (v[j] < pivot) {
      std::swap(v[i], v[j]);
      i++;
    }
    j++;
  }
  v[begin] = v[i - 1];
  v[i - 1] = pivot;
  return i - 2;
}

template <typename T>
inline void quicksort(Vector<T> &v, unsigned int begin, unsigned int end) {
  if (end > begin) {
    unsigned int index = partition(v, begin, end);
    quicksort(v, begin, index);
    quicksort(v, index + 2, end);
  }
}

template <typename T> inline Vector<T> sort(const Vector<T> &v) {
  Vector<T> tmp(v);

  quicksort<T>(tmp, 0, tmp.size() - 1);

  return tmp;
}

template <typename T> inline Vector<double> rank(const Vector<T> &v) {
  Vector<T> tmp(v);
  Vector<double> tmp_rank(0.0, v.size());

  for (unsigned int i = 0; i < tmp.size(); i++) {
    unsigned int smaller = 0, equal = 0;
    for (unsigned int j = 0; j < tmp.size(); j++)
      if (i == j)
        continue;
      else if (tmp[j] < tmp[i])
        smaller++;
      else if (tmp[j] == tmp[i])
        equal++;
    tmp_rank[i] = smaller + 1;
    if (equal > 0) {
      for (unsigned int j = 1; j <= equal; j++)
        tmp_rank[i] += smaller + 1 + j;
      tmp_rank[i] /= (double)(equal + 1);
    }
  }

  return tmp_rank;
}

// enum MType { DIAG };

template <typename T> class Matrix {
public:
  Matrix(); // Default constructor
  Matrix(const unsigned int n,
         const unsigned int m); // Construct a n x m matrix
  Matrix(const T &a, const unsigned int n,
         const unsigned int m); // Initialize the content to constant a
  Matrix(MType t, const T &a, const T &o, const unsigned int n,
         const unsigned int m);
  Matrix(MType t, const Vector<T> &v, const T &o, const unsigned int n,
         const unsigned int m);
  Matrix(const T *a, const unsigned int n,
         const unsigned int m); // Initialize to array
  Matrix(const Matrix<T> &rhs); // Copy constructor
  ~Matrix();                    // destructor

  inline T *operator[](const unsigned int &i) {
    return v[i];
  } // Subscripting: row i
  inline const T *operator[](const unsigned int &i) const {
    return v[i];
  }; // const subsctipting

  inline void resize(const unsigned int n, const unsigned int m);
  inline void resize(const T &a, const unsigned int n, const unsigned int m);

  inline Vector<T> extractRow(const unsigned int i) const;
  inline Vector<T> extractColumn(const unsigned int j) const;
  inline Vector<T> extractDiag() const;
  inline Matrix<T> extractRows(const std::set<unsigned int> &indexes) const;
  inline Matrix<T> extractColumns(const std::set<unsigned int> &indexes) const;
  inline Matrix<T> extract(const std::set<unsigned int> &r_indexes,
                           const std::set<unsigned int> &c_indexes) const;

  inline void set(const T *a, unsigned int n, unsigned int m);
  inline void set(const std::set<unsigned int> &r_indexes,
                  const std::set<unsigned int> &c_indexes, const Matrix<T> &m);
  inline void setRow(const unsigned int index, const Vector<T> &v);
  inline void setRow(const unsigned int index, const Matrix<T> &v);
  inline void setRows(const std::set<unsigned int> &indexes,
                      const Matrix<T> &m);
  inline void setColumn(const unsigned int index, const Vector<T> &v);
  inline void setColumn(const unsigned int index, const Matrix<T> &v);
  inline void setColumns(const std::set<unsigned int> &indexes,
                         const Matrix<T> &m);

  inline unsigned int nrows() const { return n; } // number of rows
  inline unsigned int ncols() const { return m; } // number of columns

  inline Matrix<T> &operator=(const Matrix<T> &rhs); // Assignment operator
  inline Matrix<T> &operator=(const T &a); // Assign to every element value a
  inline Matrix<T> &operator+=(const Matrix<T> &rhs);
  inline Matrix<T> &operator-=(const Matrix<T> &rhs);
  inline Matrix<T> &operator*=(const Matrix<T> &rhs);
  inline Matrix<T> &operator/=(const Matrix<T> &rhs);
  inline Matrix<T> &operator^=(const Matrix<T> &rhs);
  inline Matrix<T> &operator+=(const T &a);
  inline Matrix<T> &operator-=(const T &a);
  inline Matrix<T> &operator*=(const T &a);
  inline Matrix<T> &operator/=(const T &a);
  inline Matrix<T> &operator^=(const T &a);
  inline operator Vector<T>();

private:
  unsigned int n; // number of rows
  unsigned int m; // number of columns
  T **v;          // storage for data
};

template <typename T> Matrix<T>::Matrix() : n(0), m(0), v(0) {}

template <typename T>
Matrix<T>::Matrix(unsigned int n, unsigned int m) : v(new T *[n]) {
  this->n = n;
  this->m = m;
  v[0] = new T[m * n];
  for (unsigned int i = 1; i < n; i++)
    v[i] = v[i - 1] + m;
}

template <typename T>
Matrix<T>::Matrix(const T &a, unsigned int n, unsigned int m) : v(new T *[n]) {
  this->n = n;
  this->m = m;
  v[0] = new T[m * n];
  for (unsigned int i = 1; i < n; i++)
    v[i] = v[i - 1] + m;
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] = a;
}

template <class T>
Matrix<T>::Matrix(const T *a, unsigned int n, unsigned int m) : v(new T *[n]) {
  this->n = n;
  this->m = m;
  v[0] = new T[m * n];
  for (unsigned int i = 1; i < n; i++)
    v[i] = v[i - 1] + m;
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] = *a++;
}

template <class T>
Matrix<T>::Matrix(MType t, const T &a, const T &o, unsigned int n,
                  unsigned int m)
    : v(new T *[n]) {
  this->n = n;
  this->m = m;
  v[0] = new T[m * n];
  for (unsigned int i = 1; i < n; i++)
    v[i] = v[i - 1] + m;
  switch (t) {
  case DIAG:
    for (unsigned int i = 0; i < n; i++)
      for (unsigned int j = 0; j < m; j++)
        if (i != j)
          v[i][j] = o;
        else
          v[i][j] = a;
    break;
  default:
    throw std::logic_error("Matrix type not supported");
  }
}

template <class T>
Matrix<T>::Matrix(MType t, const Vector<T> &a, const T &o, unsigned int n,
                  unsigned int m)
    : v(new T *[n]) {
  this->n = n;
  this->m = m;
  v[0] = new T[m * n];
  for (unsigned int i = 1; i < n; i++)
    v[i] = v[i - 1] + m;
  switch (t) {
  case DIAG:
    for (unsigned int i = 0; i < n; i++)
      for (unsigned int j = 0; j < m; j++)
        if (i != j)
          v[i][j] = o;
        else
          v[i][j] = a[i];
    break;
  default:
    throw std::logic_error("Matrix type not supported");
  }
}

template <typename T>
Matrix<T>::Matrix(const Matrix<T> &rhs) : v(new T *[rhs.n]) {
  n = rhs.n;
  m = rhs.m;
  v[0] = new T[m * n];
  for (unsigned int i = 1; i < n; i++)
    v[i] = v[i - 1] + m;
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] = rhs[i][j];
}

template <typename T> Matrix<T>::~Matrix() {
  if (v != 0) {
    delete[] (v[0]);
    delete[] (v);
  }
}

template <typename T>
inline Matrix<T> &Matrix<T>::operator=(const Matrix<T> &rhs)
// postcondition: normal assignment via copying has been performed;
// if matrix and rhs were different sizes, matrix
// has been resized to match the size of rhs
{
  if (this != &rhs) {
    resize(rhs.n, rhs.m);
    for (unsigned int i = 0; i < n; i++)
      for (unsigned int j = 0; j < m; j++)
        v[i][j] = rhs[i][j];
  }
  return *this;
}

template <typename T>
inline Matrix<T> &Matrix<T>::operator=(const T &a) // assign a to every element
{
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] = a;
  return *this;
}

template <typename T>
inline void Matrix<T>::resize(const unsigned int n, const unsigned int m) {
  if (n == this->n && m == this->m)
    return;
  if (v != 0) {
    delete[] (v[0]);
    delete[] (v);
  }
  this->n = n;
  this->m = m;
  v = new T *[n];
  v[0] = new T[m * n];
  for (unsigned int i = 1; i < n; i++)
    v[i] = v[i - 1] + m;
}

template <typename T>
inline void Matrix<T>::resize(const T &a, const unsigned int n,
                              const unsigned int m) {
  resize(n, m);
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] = a;
}

template <typename T>
inline Vector<T> Matrix<T>::extractRow(const unsigned int i) const {
  if (i >= n)
    throw std::logic_error(
        "Error in extractRow: trying to extract a row out of matrix bounds");
  Vector<T> tmp(v[i], m);

  return tmp;
}

template <typename T>
inline Vector<T> Matrix<T>::extractColumn(const unsigned int j) const {
  if (j >= m)
    throw std::logic_error(
        "Error in extractRow: trying to extract a row out of matrix bounds");
  Vector<T> tmp(n);

  for (unsigned int i = 0; i < n; i++)
    tmp[i] = v[i][j];

  return tmp;
}

template <typename T> inline Vector<T> Matrix<T>::extractDiag() const {
  unsigned int d = std::min(n, m);

  Vector<T> tmp(d);

  for (unsigned int i = 0; i < d; i++)
    tmp[i] = v[i][i];

  return tmp;
}

template <typename T>
inline Matrix<T>
Matrix<T>::extractRows(const std::set<unsigned int> &indexes) const {
  Matrix<T> tmp(indexes.size(), m);
  unsigned int i = 0;

  for (std::set<unsigned int>::const_iterator el = indexes.begin();
       el != indexes.end(); el++) {
    for (unsigned int j = 0; j < m; j++) {
      if (*el >= n)
        throw std::logic_error(
            "Error extracting rows: the indexes are out of matrix bounds");
      tmp[i][j] = v[*el][j];
    }
    i++;
  }

  return tmp;
}

template <typename T>
inline Matrix<T>
Matrix<T>::extractColumns(const std::set<unsigned int> &indexes) const {
  Matrix<T> tmp(n, indexes.size());
  unsigned int j = 0;

  for (std::set<unsigned int>::const_iterator el = indexes.begin();
       el != indexes.end(); el++) {
    for (unsigned int i = 0; i < n; i++) {
      if (*el >= m)
        throw std::logic_error(
            "Error extracting columns: the indexes are out of matrix bounds");
      tmp[i][j] = v[i][*el];
    }
    j++;
  }

  return tmp;
}

template <typename T>
inline Matrix<T>
Matrix<T>::extract(const std::set<unsigned int> &r_indexes,
                   const std::set<unsigned int> &c_indexes) const {
  Matrix<T> tmp(r_indexes.size(), c_indexes.size());
  unsigned int i = 0, j;

  for (std::set<unsigned int>::const_iterator r_el = r_indexes.begin();
       r_el != r_indexes.end(); r_el++) {
    if (*r_el >= n)
      throw std::logic_error(
          "Error extracting submatrix: the indexes are out of matrix bounds");
    j = 0;
    for (std::set<unsigned int>::const_iterator c_el = c_indexes.begin();
         c_el != c_indexes.end(); c_el++) {
      if (*c_el >= m)
        throw std::logic_error(
            "Error extracting rows: the indexes are out of matrix bounds");
      tmp[i][j] = v[*r_el][*c_el];
      j++;
    }
    i++;
  }

  return tmp;
}

template <typename T>
inline void Matrix<T>::setRow(unsigned int i, const Vector<T> &a) {
  if (i >= n)
    throw std::logic_error(
        "Error in setRow: trying to set a row out of matrix bounds");
  if (this->m != a.size())
    throw std::logic_error(
        "Error setting matrix row: ranges are not compatible");
  for (unsigned int j = 0; j < ncols(); j++)
    v[i][j] = a[j];
}

template <typename T>
inline void Matrix<T>::setRow(unsigned int i, const Matrix<T> &a) {
  if (i >= n)
    throw std::logic_error(
        "Error in setRow: trying to set a row out of matrix bounds");
  if (this->m != a.ncols())
    throw std::logic_error(
        "Error setting matrix column: ranges are not compatible");
  if (a.nrows() != 1)
    throw std::logic_error("Error setting matrix column with a non-row matrix");
  for (unsigned int j = 0; j < ncols(); j++)
    v[i][j] = a[0][j];
}

template <typename T>
inline void Matrix<T>::setRows(const std::set<unsigned int> &indexes,
                               const Matrix<T> &m) {
  unsigned int i = 0;

  if (indexes.size() != m.nrows() || this->m != m.ncols())
    throw std::logic_error(
        "Error setting matrix rows: ranges are not compatible");
  for (std::set<unsigned int>::const_iterator el = indexes.begin();
       el != indexes.end(); el++) {
    for (unsigned int j = 0; j < ncols(); j++) {
      if (*el >= n)
        throw std::logic_error(
            "Error in setRows: trying to set a row out of matrix bounds");
      v[*el][j] = m[i][j];
    }
    i++;
  }
}

template <typename T>
inline void Matrix<T>::setColumn(unsigned int j, const Vector<T> &a) {
  if (j >= m)
    throw std::logic_error(
        "Error in setColumn: trying to set a column out of matrix bounds");
  if (this->n != a.size())
    throw std::logic_error(
        "Error setting matrix column: ranges are not compatible");
  for (unsigned int i = 0; i < nrows(); i++)
    v[i][j] = a[i];
}

template <typename T>
inline void Matrix<T>::setColumn(unsigned int j, const Matrix<T> &a) {
  if (j >= m)
    throw std::logic_error(
        "Error in setColumn: trying to set a column out of matrix bounds");
  if (this->n != a.nrows())
    throw std::logic_error(
        "Error setting matrix column: ranges are not compatible");
  if (a.ncols() != 1)
    throw std::logic_error(
        "Error setting matrix column with a non-column matrix");
  for (unsigned int i = 0; i < nrows(); i++)
    v[i][j] = a[i][0];
}

template <typename T>
inline void Matrix<T>::setColumns(const std::set<unsigned int> &indexes,
                                  const Matrix<T> &a) {
  unsigned int j = 0;

  if (indexes.size() != a.ncols() || this->n != a.nrows())
    throw std::logic_error(
        "Error setting matrix columns: ranges are not compatible");
  for (std::set<unsigned int>::const_iterator el = indexes.begin();
       el != indexes.end(); el++) {
    for (unsigned int i = 0; i < nrows(); i++) {
      if (*el >= m)
        throw std::logic_error(
            "Error in setColumns: trying to set a column out of matrix bounds");
      v[i][*el] = a[i][j];
    }
    j++;
  }
}

template <typename T>
inline void Matrix<T>::set(const std::set<unsigned int> &r_indexes,
                           const std::set<unsigned int> &c_indexes,
                           const Matrix<T> &a) {
  unsigned int i = 0, j;
  if (c_indexes.size() != a.ncols() || r_indexes.size() != a.nrows())
    throw std::logic_error(
        "Error setting matrix elements: ranges are not compatible");

  for (std::set<unsigned int>::const_iterator r_el = r_indexes.begin();
       r_el != r_indexes.end(); r_el++) {
    if (*r_el >= n)
      throw std::logic_error(
          "Error in set: trying to set a row out of matrix bounds");
    j = 0;
    for (std::set<unsigned int>::const_iterator c_el = c_indexes.begin();
         c_el != c_indexes.end(); c_el++) {
      if (*c_el >= m)
        throw std::logic_error(
            "Error in set: trying to set a column out of matrix bounds");
      v[*r_el][*c_el] = a[i][j];
      j++;
    }
    i++;
  }
}

template <typename T>
inline void Matrix<T>::set(const T *a, unsigned int n, unsigned int m) {
  if (this->n != n || this->m != m)
    resize(n, m);
  unsigned int k = 0;
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] = a[k++];
}

template <typename T> Matrix<T> operator+(const Matrix<T> &rhs) { return rhs; }

template <typename T>
Matrix<T> operator+(const Matrix<T> &lhs, const Matrix<T> &rhs) {
  if (lhs.ncols() != rhs.ncols() || lhs.nrows() != rhs.nrows())
    throw std::logic_error("Operator+: matrices have different sizes");
  Matrix<T> tmp(lhs.nrows(), lhs.ncols());
  for (unsigned int i = 0; i < lhs.nrows(); i++)
    for (unsigned int j = 0; j < lhs.ncols(); j++)
      tmp[i][j] = lhs[i][j] + rhs[i][j];

  return tmp;
}

template <typename T> Matrix<T> operator+(const Matrix<T> &lhs, const T &a) {
  Matrix<T> tmp(lhs.nrows(), lhs.ncols());
  for (unsigned int i = 0; i < lhs.nrows(); i++)
    for (unsigned int j = 0; j < lhs.ncols(); j++)
      tmp[i][j] = lhs[i][j] + a;

  return tmp;
}

template <typename T> Matrix<T> operator+(const T &a, const Matrix<T> &rhs) {
  Matrix<T> tmp(rhs.nrows(), rhs.ncols());
  for (unsigned int i = 0; i < rhs.nrows(); i++)
    for (unsigned int j = 0; j < rhs.ncols(); j++)
      tmp[i][j] = a + rhs[i][j];

  return tmp;
}

template <typename T>
inline Matrix<T> &Matrix<T>::operator+=(const Matrix<T> &rhs) {
  if (m != rhs.ncols() || n != rhs.nrows())
    throw std::logic_error("Operator+=: matrices have different sizes");
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] += rhs[i][j];

  return *this;
}

template <typename T> inline Matrix<T> &Matrix<T>::operator+=(const T &a) {
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] += a;

  return *this;
}

template <typename T> Matrix<T> operator-(const Matrix<T> &rhs) {
  return (T)(-1) * rhs;
}

template <typename T>
Matrix<T> operator-(const Matrix<T> &lhs, const Matrix<T> &rhs) {
  if (lhs.ncols() != rhs.ncols() || lhs.nrows() != rhs.nrows())
    throw std::logic_error("Operator-: matrices have different sizes");
  Matrix<T> tmp(lhs.nrows(), lhs.ncols());
  for (unsigned int i = 0; i < lhs.nrows(); i++)
    for (unsigned int j = 0; j < lhs.ncols(); j++)
      tmp[i][j] = lhs[i][j] - rhs[i][j];

  return tmp;
}

template <typename T> Matrix<T> operator-(const Matrix<T> &lhs, const T &a) {
  Matrix<T> tmp(lhs.nrows(), lhs.ncols());
  for (unsigned int i = 0; i < lhs.nrows(); i++)
    for (unsigned int j = 0; j < lhs.ncols(); j++)
      tmp[i][j] = lhs[i][j] - a;

  return tmp;
}

template <typename T> Matrix<T> operator-(const T &a, const Matrix<T> &rhs) {
  Matrix<T> tmp(rhs.nrows(), rhs.ncols());
  for (unsigned int i = 0; i < rhs.nrows(); i++)
    for (unsigned int j = 0; j < rhs.ncols(); j++)
      tmp[i][j] = a - rhs[i][j];

  return tmp;
}

template <typename T>
inline Matrix<T> &Matrix<T>::operator-=(const Matrix<T> &rhs) {
  if (m != rhs.ncols() || n != rhs.nrows())
    throw std::logic_error("Operator-=: matrices have different sizes");
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] -= rhs[i][j];

  return *this;
}

template <typename T> inline Matrix<T> &Matrix<T>::operator-=(const T &a) {
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] -= a;

  return *this;
}

template <typename T>
Matrix<T> operator*(const Matrix<T> &lhs, const Matrix<T> &rhs) {
  if (lhs.ncols() != rhs.ncols() || lhs.nrows() != rhs.nrows())
    throw std::logic_error("Operator*: matrices have different sizes");
  Matrix<T> tmp(lhs.nrows(), lhs.ncols());
  for (unsigned int i = 0; i < lhs.nrows(); i++)
    for (unsigned int j = 0; j < lhs.ncols(); j++)
      tmp[i][j] = lhs[i][j] * rhs[i][j];

  return tmp;
}

template <typename T> Matrix<T> operator*(const Matrix<T> &lhs, const T &a) {
  Matrix<T> tmp(lhs.nrows(), lhs.ncols());
  for (unsigned int i = 0; i < lhs.nrows(); i++)
    for (unsigned int j = 0; j < lhs.ncols(); j++)
      tmp[i][j] = lhs[i][j] * a;

  return tmp;
}

template <typename T> Matrix<T> operator*(const T &a, const Matrix<T> &rhs) {
  Matrix<T> tmp(rhs.nrows(), rhs.ncols());
  for (unsigned int i = 0; i < rhs.nrows(); i++)
    for (unsigned int j = 0; j < rhs.ncols(); j++)
      tmp[i][j] = a * rhs[i][j];

  return tmp;
}

template <typename T>
inline Matrix<T> &Matrix<T>::operator*=(const Matrix<T> &rhs) {
  if (m != rhs.ncols() || n != rhs.nrows())
    throw std::logic_error("Operator*=: matrices have different sizes");
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] *= rhs[i][j];

  return *this;
}

template <typename T> inline Matrix<T> &Matrix<T>::operator*=(const T &a) {
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] *= a;

  return *this;
}

template <typename T>
Matrix<T> operator/(const Matrix<T> &lhs, const Matrix<T> &rhs) {
  if (lhs.ncols() != rhs.ncols() || lhs.nrows() != rhs.nrows())
    throw std::logic_error("Operator+: matrices have different sizes");
  Matrix<T> tmp(lhs.nrows(), lhs.ncols());
  for (unsigned int i = 0; i < lhs.nrows(); i++)
    for (unsigned int j = 0; j < lhs.ncols(); j++)
      tmp[i][j] = lhs[i][j] / rhs[i][j];

  return tmp;
}

template <typename T> Matrix<T> operator/(const Matrix<T> &lhs, const T &a) {
  Matrix<T> tmp(lhs.nrows(), lhs.ncols());
  for (unsigned int i = 0; i < lhs.nrows(); i++)
    for (unsigned int j = 0; j < lhs.ncols(); j++)
      tmp[i][j] = lhs[i][j] / a;

  return tmp;
}

template <typename T> Matrix<T> operator/(const T &a, const Matrix<T> &rhs) {
  Matrix<T> tmp(rhs.nrows(), rhs.ncols());
  for (unsigned int i = 0; i < rhs.nrows(); i++)
    for (unsigned int j = 0; j < rhs.ncols(); j++)
      tmp[i][j] = a / rhs[i][j];

  return tmp;
}

template <typename T>
inline Matrix<T> &Matrix<T>::operator/=(const Matrix<T> &rhs) {
  if (m != rhs.ncols() || n != rhs.nrows())
    throw std::logic_error("Operator+=: matrices have different sizes");
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] /= rhs[i][j];

  return *this;
}

template <typename T> inline Matrix<T> &Matrix<T>::operator/=(const T &a) {
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] /= a;

  return *this;
}

template <typename T> Matrix<T> operator^(const Matrix<T> &lhs, const T &a) {
  Matrix<T> tmp(lhs.nrows(), lhs.ncols());
  for (unsigned int i = 0; i < lhs.nrows(); i++)
    for (unsigned int j = 0; j < lhs.ncols(); j++)
      tmp[i][j] = pow(lhs[i][j], a);

  return tmp;
}

template <typename T>
inline Matrix<T> &Matrix<T>::operator^=(const Matrix<T> &rhs) {
  if (m != rhs.ncols() || n != rhs.nrows())
    throw std::logic_error("Operator^=: matrices have different sizes");
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] = pow(v[i][j], rhs[i][j]);

  return *this;
}

template <typename T> inline Matrix<T> &Matrix<T>::operator^=(const T &a) {
  for (unsigned int i = 0; i < n; i++)
    for (unsigned int j = 0; j < m; j++)
      v[i][j] = pow(v[i][j], a);

  return *this;
}

template <typename T> inline Matrix<T>::operator Vector<T>() {
  if (n > 1 && m > 1)
    throw std::logic_error("Error matrix cast to vector: trying to cast a "
                           "multi-dimensional matrix");
  if (n == 1)
    return extractRow(0);
  else
    return extractColumn(0);
}

template <typename T>
inline bool operator==(const Matrix<T> &a, const Matrix<T> &b) {
  if (a.nrows() != b.nrows() || a.ncols() != b.ncols())
    throw std::logic_error("Matrices of different size are not confrontable");
  for (unsigned i = 0; i < a.nrows(); i++)
    for (unsigned j = 0; j < a.ncols(); j++)
      if (a[i][j] != b[i][j])
        return false;
  return true;
}

template <typename T>
inline bool operator!=(const Matrix<T> &a, const Matrix<T> &b) {
  if (a.nrows() != b.nrows() || a.ncols() != b.ncols())
    throw std::logic_error("Matrices of different size are not confrontable");
  for (unsigned i = 0; i < a.nrows(); i++)
    for (unsigned j = 0; j < a.ncols(); j++)
      if (a[i][j] != b[i][j])
        return true;
  return false;
}

/**
   Input/Output
*/
template <typename T>
std::ostream &operator<<(std::ostream &os, const Matrix<T> &m) {
  os << std::endl << m.nrows() << " " << m.ncols() << std::endl;
  for (unsigned int i = 0; i < m.nrows(); i++) {
    for (unsigned int j = 0; j < m.ncols() - 1; j++)
      os << std::setw(20) << std::setprecision(16) << m[i][j] << ", ";
    os << std::setw(20) << std::setprecision(16) << m[i][m.ncols() - 1]
       << std::endl;
  }

  return os;
}

template <typename T> std::istream &operator>>(std::istream &is, Matrix<T> &m) {
  int rows, cols;
  char comma;
  is >> rows >> cols;
  m.resize(rows, cols);
  for (unsigned int i = 0; i < rows; i++)
    for (unsigned int j = 0; j < cols; j++)
      is >> m[i][j] >> comma;

  return is;
}

template <typename T> T sign(const T &v) {
  if (v >= (T)0.0)
    return (T)1.0;
  else
    return (T)-1.0;
}

template <typename T> T dist(const T &a, const T &b) {
  T abs_a = (T)fabs(a), abs_b = (T)fabs(b);
  if (abs_a > abs_b)
    return abs_a * sqrt((T)1.0 + (abs_b / abs_a) * (abs_b / abs_a));
  else
    return (abs_b == (T)0.0
                ? (T)0.0
                : abs_b * sqrt((T)1.0 + (abs_a / abs_b) * (abs_a / abs_b)));
}

template <typename T>
void svd(const Matrix<T> &A, Matrix<T> &U, Vector<T> &W, Matrix<T> &V) {
  int m = A.nrows(), n = A.ncols(), i, j, k, l, jj, nm;
  const unsigned int max_its = 30;
  bool flag;
  Vector<T> rv1(n);
  U = A;
  W.resize(n);
  V.resize(n, n);
  T anorm, c, f, g, h, s, scale, x, y, z;
  g = scale = anorm = (T)0.0;

  // Householder reduction to bidiagonal form
  for (i = 0; i < n; i++) {
    l = i + 1;
    rv1[i] = scale * g;
    g = s = scale = (T)0.0;
    if (i < m) {
      for (k = i; k < m; k++)
        scale += fabs(U[k][i]);
      if (scale != (T)0.0) {
        for (k = i; k < m; k++) {
          U[k][i] /= scale;
          s += U[k][i] * U[k][i];
        }
        f = U[i][i];
        g = -sign(f) * sqrt(s);
        h = f * g - s;
        U[i][i] = f - g;
        for (j = l; j < n; j++) {
          s = (T)0.0;
          for (k = i; k < m; k++)
            s += U[k][i] * U[k][j];
          f = s / h;
          for (k = i; k < m; k++)
            U[k][j] += f * U[k][i];
        }
        for (k = i; k < m; k++)
          U[k][i] *= scale;
      }
    }
    W[i] = scale * g;
    g = s = scale = (T)0.0;
    if (i < m && i != n - 1) {
      for (k = l; k < n; k++)
        scale += fabs(U[i][k]);
      if (scale != (T)0.0) {
        for (k = l; k < n; k++) {
          U[i][k] /= scale;
          s += U[i][k] * U[i][k];
        }
        f = U[i][l];
        g = -sign(f) * sqrt(s);
        h = f * g - s;
        U[i][l] = f - g;
        for (k = l; k < n; k++)
          rv1[k] = U[i][k] / h;
        for (j = l; j < m; j++) {
          s = (T)0.0;
          for (k = l; k < n; k++)
            s += U[j][k] * U[i][k];
          for (k = l; k < n; k++)
            U[j][k] += s * rv1[k];
        }
        for (k = l; k < n; k++)
          U[i][k] *= scale;
      }
    }
    anorm = std::max(anorm, fabs(W[i]) + fabs(rv1[i]));
  }
  // Accumulation of right-hand transformations
  for (i = n - 1; i >= 0; i--) {
    if (i < n - 1) {
      if (g != (T)0.0) {
        for (j = l; j < n; j++)
          V[j][i] = (U[i][j] / U[i][l]) / g;
        for (j = l; j < n; j++) {
          s = (T)0.0;
          for (k = l; k < n; k++)
            s += U[i][k] * V[k][j];
          for (k = l; k < n; k++)
            V[k][j] += s * V[k][i];
        }
      }
      for (j = l; j < n; j++)
        V[i][j] = V[j][i] = (T)0.0;
    }
    V[i][i] = (T)1.0;
    g = rv1[i];
    l = i;
  }
  // Accumulation of left-hand transformations
  for (i = std::min(m, n) - 1; i >= 0; i--) {
    l = i + 1;
    g = W[i];
    for (j = l; j < n; j++)
      U[i][j] = (T)0.0;
    if (g != (T)0.0) {
      g = (T)1.0 / g;
      for (j = l; j < n; j++) {
        s = (T)0.0;
        for (k = l; k < m; k++)
          s += U[k][i] * U[k][j];
        f = (s / U[i][i]) * g;
        for (k = i; k < m; k++)
          U[k][j] += f * U[k][i];
      }
      for (j = i; j < m; j++)
        U[j][i] *= g;
    } else
      for (j = i; j < m; j++)
        U[j][i] = (T)0.0;
    U[i][i]++;
  }
  // Diagonalization of the bidiagonal form: loop over singular values, and over
  // allowed iterations.
  for (k = n - 1; k >= 0; k--) {
    for (unsigned int its = 0; its < max_its; its++) {
      flag = true;
      for (l = k; l >= 0; l--) // FIXME: in NR it was l >= 1 but there
                               // subscripts start from one
      {                        // Test for splitting
        nm = l - 1;            // Note that rV[0] is always zero
        if ((T)(fabs(rv1[l]) + anorm) == anorm) {
          flag = false;
          break;
        }
        if ((T)(fabs(W[nm]) + anorm) == anorm)
          break;
      }
      if (flag) {
        // Cancellation of rv1[l], if l > 0 FIXME: it was l > 1 in NR
        c = (T)0.0;
        s = (T)1.0;
        for (i = l; i <= k; i++) {
          f = s * rv1[i];
          rv1[i] *= c;
          if ((T)(fabs(f) + anorm) == anorm)
            break;
          g = W[i];
          h = dist(f, g);
          W[i] = h;
          h = (T)1.0 / h;
          c = g * h;
          s = -f * h;
          for (j = 0; j < m; j++) {
            y = U[j][nm];
            z = U[j][i];
            U[j][nm] = y * c + z * s;
            U[j][i] = z * c - y * s;
          }
        }
      }
      z = W[k];
      if (l == k) {       // Convergence
        if (z < (T)0.0) { // Singular value is made nonnegative
          W[k] = -z;
          for (j = 0; j < n; j++)
            V[j][k] = -V[j][k];
        }
        break;
      }
      if (its == max_its)
        throw std::logic_error(
            "Error svd: no convergence in the maximum number of iterations");
      x = W[l];
      nm = k - 1;
      y = W[nm];
      g = rv1[nm];
      h = rv1[k];
      f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
      g = dist(f, (T)1.0);
      f = ((x - z) * (x + z) + h * ((y / (f + sign(f) * fabs(g))) - h)) / x;
      c = s = (T)1.0; // Next QR transformation
      for (j = l; j <= nm; j++) {
        i = j + 1;
        g = rv1[i];
        y = W[i];
        h = s * g;
        g *= c;
        z = dist(f, h);
        rv1[j] = z;
        c = f / z;
        s = h / z;
        f = x * c + g * s;
        g = g * c - x * s;
        h = y * s;
        y *= c;
        for (jj = 0; jj < n; jj++) {
          x = V[jj][j];
          z = V[jj][i];
          V[jj][j] = x * c + z * s;
          V[jj][i] = z * c - x * s;
        }
        z = dist(f, h);
        W[j] = z;
        if (z != 0) // Rotation can be arbitrary if z = 0
        {
          z = (T)1.0 / z;
          c = f * z;
          s = h * z;
        }
        f = c * g + s * y;
        x = c * y - s * g;
        for (jj = 0; jj < m; jj++) {
          y = U[jj][j];
          z = U[jj][i];
          U[jj][j] = y * c + z * s;
          U[jj][i] = z * c - y * s;
        }
      }
      rv1[l] = (T)0.0;
      rv1[k] = f;
      W[k] = x;
    }
  }
}

template <typename T> Matrix<T> pinv(const Matrix<T> &A) {
  Matrix<T> U, V, x, tmp(A.ncols(), A.nrows());
  Vector<T> W;
  CanonicalBaseVector<T> e(0, A.nrows());
  svd(A, U, W, V);
  for (unsigned int i = 0; i < A.nrows(); i++) {
    e.reset(i);
    tmp.setColumn(
        i, dot_prod(dot_prod(dot_prod(V, Matrix<double>(DIAG, 1.0 / W, 0.0,
                                                        W.size(), W.size())),
                             t(U)),
                    e));
  }

  return tmp;
}

template <typename T>
int lu(const Matrix<T> &A, Matrix<T> &LU, Vector<unsigned int> &index) {
  if (A.ncols() != A.nrows())
    throw std::logic_error("Error in LU decomposition: matrix must be squared");
  int i, p, j, k, n = A.ncols(), ex;
  T val, tmp;
  Vector<T> d(n);
  LU = A;
  index.resize(n);

  ex = 1;
  for (i = 0; i < n; i++) {
    index[i] = i;
    val = (T)0.0;
    for (j = 0; j < n; j++)
      val = std::max(val, (T)fabs(LU[i][j]));
    if (val == (T)0.0)
      std::logic_error("Error in LU decomposition: matrix was singular");
    d[i] = val;
  }

  for (k = 0; k < n - 1; k++) {
    p = k;
    val = fabs(LU[k][k]) / d[k];
    for (i = k + 1; i < n; i++) {
      tmp = fabs(LU[i][k]) / d[i];
      if (tmp > val) {
        val = tmp;
        p = i;
      }
    }
    if (val == (T)0.0)
      std::logic_error("Error in LU decomposition: matrix was singular");
    if (p > k) {
      ex = -ex;
      std::swap(index[k], index[p]);
      std::swap(d[k], d[p]);
      for (j = 0; j < n; j++)
        std::swap(LU[k][j], LU[p][j]);
    }

    for (i = k + 1; i < n; i++) {
      LU[i][k] /= LU[k][k];
      for (j = k + 1; j < n; j++)
        LU[i][j] -= LU[i][k] * LU[k][j];
    }
  }
  if (LU[n - 1][n - 1] == (T)0.0)
    std::logic_error("Error in LU decomposition: matrix was singular");

  return ex;
}

template <typename T>
Vector<T> lu_solve(const Matrix<T> &LU, const Vector<T> &b,
                   Vector<unsigned int> &index) {
  if (LU.ncols() != LU.nrows())
    throw std::logic_error("Error in LU solve: LU matrix should be squared");
  unsigned int n = LU.ncols();
  if (b.size() != n)
    throw std::logic_error("Error in LU solve: b vector must be of the same "
                           "dimensions of LU matrix");
  Vector<T> x((T)0.0, n);
  int i, j, p;
  T sum;

  p = index[0];
  x[0] = b[p];

  for (i = 1; i < n; i++) {
    sum = (T)0.0;
    for (j = 0; j < i; j++)
      sum += LU[i][j] * x[j];
    p = index[i];
    x[i] = b[p] - sum;
  }
  x[n - 1] /= LU[n - 1][n - 1];
  for (i = n - 2; i >= 0; i--) {
    sum = (T)0.0;
    for (j = i + 1; j < n; j++)
      sum += LU[i][j] * x[j];
    x[i] = (x[i] - sum) / LU[i][i];
  }
  return x;
}

template <typename T>
void lu_solve(const Matrix<T> &LU, Vector<T> &x, const Vector<T> &b,
              Vector<unsigned int> &index) {
  x = lu_solve(LU, b, index);
}

template <typename T> Matrix<T> lu_inverse(const Matrix<T> &A) {
  if (A.ncols() != A.nrows())
    throw std::logic_error("Error in LU invert: matrix must be squared");
  unsigned int n = A.ncols();
  Matrix<T> A1(n, n), LU;
  Vector<unsigned int> index;

  lu(A, LU, index);
  CanonicalBaseVector<T> e(0, n);
  for (unsigned i = 0; i < n; i++) {
    e.reset(i);
    A1.setColumn(i, lu_solve(LU, e, index));
  }

  return A1;
}

template <typename T> T lu_det(const Matrix<T> &A) {
  if (A.ncols() != A.nrows())
    throw std::logic_error("Error in LU determinant: matrix must be squared");
  unsigned int d;
  Matrix<T> LU;
  Vector<unsigned int> index;

  d = lu(A, LU, index);

  return d * prod(LU.extractDiag());
}

template <typename T> void cholesky(const Matrix<T> A, Matrix<T> &LL) {
  if (A.ncols() != A.nrows())
    throw std::logic_error(
        "Error in Cholesky decomposition: matrix must be squared");
  int n = A.ncols();
  double sum;
  LL = A;

  for (unsigned int i = 0; i < n; i++) {
    for (unsigned int j = i; j < n; j++) {
      sum = LL[i][j];
      for (int k = i - 1; k >= 0; k--)
        sum -= LL[i][k] * LL[j][k];
      if (i == j) {
        if (sum <= 0.0)
          throw std::logic_error("Error in Cholesky decomposition: matrix is "
                                 "not postive definite");
        LL[i][i] = sqrt(sum);
      } else
        LL[j][i] = sum / LL[i][i];
    }
    for (unsigned int k = i + 1; k < n; k++)
      LL[i][k] = LL[k][i];
  }
}

template <typename T> Matrix<T> cholesky(const Matrix<T> A) {
  Matrix<T> LL;
  cholesky(A, LL);

  return LL;
}

template <typename T>
Vector<T> cholesky_solve(const Matrix<T> &LL, const Vector<T> &b) {
  if (LL.ncols() != LL.nrows())
    throw std::logic_error("Error in Cholesky solve: matrix must be squared");
  unsigned int n = LL.ncols();
  if (b.size() != n)
    throw std::logic_error("Error in Cholesky decomposition: b vector must be "
                           "of the same dimensions of LU matrix");
  Vector<T> x, y;

  /* Solve L * y = b */
  forward_elimination(LL, y, b);
  /* Solve L^T * x = y */
  backward_elimination(LL, x, y);

  return x;
}

template <typename T>
void cholesky_solve(const Matrix<T> &LL, Vector<T> &x, const Vector<T> &b) {
  x = cholesky_solve(LL, b);
}

template <typename T>
void forward_elimination(const Matrix<T> &L, Vector<T> &y, const Vector<T> b) {
  if (L.ncols() != L.nrows())
    throw std::logic_error("Error in Forward elimination: matrix must be "
                           "squared (lower triangular)");
  if (b.size() != L.nrows())
    throw std::logic_error("Error in Forward elimination: b vector must be of "
                           "the same dimensions of L matrix");
  unsigned int n = b.size();
  y.resize(n);

  y[0] = b[0] / L[0][0];
  for (unsigned int i = 1; i < n; i++) {
    y[i] = b[i];
    for (unsigned int j = 0; j < i; j++)
      y[i] -= L[i][j] * y[j];
    y[i] = y[i] / L[i][i];
  }
}

template <typename T>
Vector<T> forward_elimination(const Matrix<T> &L, const Vector<T> b) {
  Vector<T> y;
  forward_elimination(L, y, b);

  return y;
}

template <typename T>
void backward_elimination(const Matrix<T> &U, Vector<T> &x,
                          const Vector<T> &y) {
  if (U.ncols() != U.nrows())
    throw std::logic_error("Error in Backward elimination: matrix must be "
                           "squared (upper triangular)");
  if (y.size() != U.nrows())
    throw std::logic_error("Error in Backward elimination: b vector must be of "
                           "the same dimensions of U matrix");
  int n = y.size();
  x.resize(n);

  x[n - 1] = y[n - 1] / U[n - 1][n - 1];
  for (int i = n - 2; i >= 0; i--) {
    x[i] = y[i];
    for (int j = i + 1; j < n; j++)
      x[i] -= U[i][j] * x[j];
    x[i] = x[i] / U[i][i];
  }
}

template <typename T>
Vector<T> backward_elimination(const Matrix<T> &U, const Vector<T> y) {
  Vector<T> x;
  forward_elimination(U, x, y);

  return x;
}

/* Setting default linear systems machinery */

#define det lu_det
#define inverse lu_inverse
#define solve lu_solve

/* Random */

template <typename T> void random(Matrix<T> &m) {
  for (unsigned int i = 0; i < m.nrows(); i++)
    for (unsigned int j = 0; j < m.ncols(); j++)
      m[i][j] = (T)(rand() / double(RAND_MAX));
}

/**
   Aggregate functions
*/

template <typename T> Vector<T> sum(const Matrix<T> &m) {
  Vector<T> tmp((T)0, m.ncols());
  for (unsigned int j = 0; j < m.ncols(); j++)
    for (unsigned int i = 0; i < m.nrows(); i++)
      tmp[j] += m[i][j];
  return tmp;
}

template <typename T> Vector<T> r_sum(const Matrix<T> &m) {
  Vector<T> tmp((T)0, m.nrows());
  for (unsigned int i = 0; i < m.nrows(); i++)
    for (unsigned int j = 0; j < m.ncols(); j++)
      tmp[i] += m[i][j];
  return tmp;
}

template <typename T> T all_sum(const Matrix<T> &m) {
  T tmp = (T)0;
  for (unsigned int i = 0; i < m.nrows(); i++)
    for (unsigned int j = 0; j < m.ncols(); j++)
      tmp += m[i][j];
  return tmp;
}

template <typename T> Vector<T> prod(const Matrix<T> &m) {
  Vector<T> tmp((T)1, m.ncols());
  for (unsigned int j = 0; j < m.ncols(); j++)
    for (unsigned int i = 0; i < m.nrows(); i++)
      tmp[j] *= m[i][j];
  return tmp;
}

template <typename T> Vector<T> r_prod(const Matrix<T> &m) {
  Vector<T> tmp((T)1, m.nrows());
  for (unsigned int i = 0; i < m.nrows(); i++)
    for (unsigned int j = 0; j < m.ncols(); j++)
      tmp[i] *= m[i][j];
  return tmp;
}

template <typename T> T all_prod(const Matrix<T> &m) {
  T tmp = (T)1;
  for (unsigned int i = 0; i < m.nrows(); i++)
    for (unsigned int j = 0; j < m.ncols(); j++)
      tmp *= m[i][j];
  return tmp;
}

template <typename T> Vector<T> mean(const Matrix<T> &m) {
  Vector<T> res((T)0, m.ncols());
  for (unsigned int j = 0; j < m.ncols(); j++) {
    for (unsigned int i = 0; i < m.nrows(); i++)
      res[j] += m[i][j];
    res[j] /= m.nrows();
  }

  return res;
}

template <typename T> Vector<T> r_mean(const Matrix<T> &m) {
  Vector<T> res((T)0, m.rows());
  for (unsigned int i = 0; i < m.nrows(); i++) {
    for (unsigned int j = 0; j < m.ncols(); j++)
      res[i] += m[i][j];
    res[i] /= m.nrows();
  }

  return res;
}

template <typename T> T all_mean(const Matrix<T> &m) {
  T tmp = (T)0;
  for (unsigned int i = 0; i < m.nrows(); i++)
    for (unsigned int j = 0; j < m.ncols(); j++)
      tmp += m[i][j];
  return tmp / (m.nrows() * m.ncols());
}

template <typename T>
Vector<T> var(const Matrix<T> &m, bool sample_correction = false) {
  Vector<T> res((T)0, m.ncols());
  unsigned int n = m.nrows();
  double sum, ssum;
  for (unsigned int j = 0; j < m.ncols(); j++) {
    sum = (T)0.0;
    ssum = (T)0.0;
    for (unsigned int i = 0; i < m.nrows(); i++) {
      sum += m[i][j];
      ssum += (m[i][j] * m[i][j]);
    }
    if (!sample_correction)
      res[j] = (ssum / n) - (sum / n) * (sum / n);
    else
      res[j] = n * ((ssum / n) - (sum / n) * (sum / n)) / (n - 1);
  }

  return res;
}

template <typename T>
Vector<T> stdev(const Matrix<T> &m, bool sample_correction = false) {
  return vec_sqrt(var(m, sample_correction));
}

template <typename T>
Vector<T> r_var(const Matrix<T> &m, bool sample_correction = false) {
  Vector<T> res((T)0, m.nrows());
  double sum, ssum;
  unsigned int n = m.ncols();
  for (unsigned int i = 0; i < m.nrows(); i++) {
    sum = 0.0;
    ssum = 0.0;
    for (unsigned int j = 0; j < m.ncols(); j++) {
      sum += m[i][j];
      ssum += (m[i][j] * m[i][j]);
    }
    if (!sample_correction)
      res[i] = (ssum / n) - (sum / n) * (sum / n);
    else
      res[i] = n * ((ssum / n) - (sum / n) * (sum / n)) / (n - 1);
  }

  return res;
}

template <typename T>
Vector<T> r_stdev(const Matrix<T> &m, bool sample_correction = false) {
  return vec_sqrt(r_var(m, sample_correction));
}

template <typename T> Vector<T> max(const Matrix<T> &m) {
  Vector<T> res(m.ncols());
  double value;
  for (unsigned int j = 0; j < m.ncols(); j++) {
    value = m[0][j];
    for (unsigned int i = 1; i < m.nrows(); i++)
      value = std::max(m[i][j], value);
    res[j] = value;
  }

  return res;
}

template <typename T> Vector<T> r_max(const Matrix<T> &m) {
  Vector<T> res(m.nrows());
  double value;
  for (unsigned int i = 0; i < m.nrows(); i++) {
    value = m[i][0];
    for (unsigned int j = 1; j < m.ncols(); j++)
      value = std::max(m[i][j], value);
    res[i] = value;
  }

  return res;
}

template <typename T> Vector<T> min(const Matrix<T> &m) {
  Vector<T> res(m.ncols());
  double value;
  for (unsigned int j = 0; j < m.ncols(); j++) {
    value = m[0][j];
    for (unsigned int i = 1; i < m.nrows(); i++)
      value = std::min(m[i][j], value);
    res[j] = value;
  }

  return res;
}

template <typename T> Vector<T> r_min(const Matrix<T> &m) {
  Vector<T> res(m.nrows());
  double value;
  for (unsigned int i = 0; i < m.nrows(); i++) {
    value = m[i][0];
    for (unsigned int j = 1; j < m.ncols(); j++)
      value = std::min(m[i][j], value);
    res[i] = value;
  }

  return res;
}

/**
   Single element mathematical functions
*/

template <typename T> Matrix<T> exp(const Matrix<T> &m) {
  Matrix<T> tmp(m.nrows(), m.ncols());

  for (unsigned int i = 0; i < m.nrows(); i++)
    for (unsigned int j = 0; j < m.ncols(); j++)
      tmp[i][j] = exp(m[i][j]);

  return tmp;
}

template <typename T> Matrix<T> mat_sqrt(const Matrix<T> &m) {
  Matrix<T> tmp(m.nrows(), m.ncols());

  for (unsigned int i = 0; i < m.nrows(); i++)
    for (unsigned int j = 0; j < m.ncols(); j++)
      tmp[i][j] = sqrt(m[i][j]);

  return tmp;
}

/**
   Matrix operators
*/

template <typename T> Matrix<T> kron(const Vector<T> &b, const Vector<T> &a) {
  Matrix<T> tmp(b.size(), a.size());
  for (unsigned int i = 0; i < b.size(); i++)
    for (unsigned int j = 0; j < a.size(); j++)
      tmp[i][j] = a[j] * b[i];

  return tmp;
}

template <typename T> Matrix<T> t(const Matrix<T> &a) {
  Matrix<T> tmp(a.ncols(), a.nrows());
  for (unsigned int i = 0; i < a.nrows(); i++)
    for (unsigned int j = 0; j < a.ncols(); j++)
      tmp[j][i] = a[i][j];

  return tmp;
}

template <typename T>
Matrix<T> dot_prod(const Matrix<T> &a, const Matrix<T> &b) {
  if (a.ncols() != b.nrows())
    throw std::logic_error("Error matrix dot product: dimensions of the "
                           "matrices are not compatible");
  Matrix<T> tmp(a.nrows(), b.ncols());
  for (unsigned int i = 0; i < tmp.nrows(); i++)
    for (unsigned int j = 0; j < tmp.ncols(); j++) {
      tmp[i][j] = (T)0;
      for (unsigned int k = 0; k < a.ncols(); k++)
        tmp[i][j] += a[i][k] * b[k][j];
    }

  return tmp;
}

template <typename T>
Matrix<T> dot_prod(const Matrix<T> &a, const Vector<T> &b) {
  if (a.ncols() != b.size())
    throw std::logic_error("Error matrix dot product: dimensions of the matrix "
                           "and the vector are not compatible");
  Matrix<T> tmp(a.nrows(), 1);
  for (unsigned int i = 0; i < tmp.nrows(); i++) {
    tmp[i][0] = (T)0;
    for (unsigned int k = 0; k < a.ncols(); k++)
      tmp[i][0] += a[i][k] * b[k];
  }

  return tmp;
}

template <typename T>
Matrix<T> dot_prod(const Vector<T> &a, const Matrix<T> &b) {
  if (a.size() != b.nrows())
    throw std::logic_error("Error matrix dot product: dimensions of the vector "
                           "and matrix are not compatible");
  Matrix<T> tmp(1, b.ncols());
  for (unsigned int j = 0; j < tmp.ncols(); j++) {
    tmp[0][j] = (T)0;
    for (unsigned int k = 0; k < a.size(); k++)
      tmp[0][j] += a[k] * b[k][j];
  }

  return tmp;
}

template <typename T> inline Matrix<double> rank(const Matrix<T> m) {
  Matrix<double> tmp(m.nrows(), m.ncols());
  for (unsigned int j = 0; j < m.ncols(); j++)
    tmp.setColumn(j, rank<T>(m.extractColumn(j)));

  return tmp;
}

template <typename T> inline Matrix<double> r_rank(const Matrix<T> m) {
  Matrix<double> tmp(m.nrows(), m.ncols());
  for (unsigned int i = 0; i < m.nrows(); i++)
    tmp.setRow(i, rank<T>(m.extractRow(i)));

  return tmp;
}

} // namespace quadprogpp

#endif // define _ARRAY_HH_

// #ifndef NDEBUG
// #define TRACE_SOLVER
// #endif

namespace quadprogpp {

// Utility functions for updating some data needed by the solution method
void compute_d(Vector<double> &d, const Matrix<double> &J,
               const Vector<double> &np);
void update_z(Vector<double> &z, const Matrix<double> &J,
              const Vector<double> &d, int iq);
void update_r(const Matrix<double> &R, Vector<double> &r,
              const Vector<double> &d, int iq);
bool add_constraint(Matrix<double> &R, Matrix<double> &J, Vector<double> &d,
                    unsigned int &iq, double &rnorm);
void delete_constraint(Matrix<double> &R, Matrix<double> &J, Vector<int> &A,
                       Vector<double> &u, unsigned int n, int p,
                       unsigned int &iq, int l);

// Utility functions for computing the Cholesky decomposition and solving
// linear systems
void cholesky_decomposition(Matrix<double> &A);
void cholesky_solve(const Matrix<double> &L, Vector<double> &x,
                    const Vector<double> &b);
void forward_elimination(const Matrix<double> &L, Vector<double> &y,
                         const Vector<double> &b);
void backward_elimination(const Matrix<double> &U, Vector<double> &x,
                          const Vector<double> &y);

// Utility functions for computing the scalar product and the euclidean
// distance between two numbers
double scalar_product(const Vector<double> &x, const Vector<double> &y);
double distance(double a, double b);

// Utility functions for printing vectors and matrices
void print_matrix(const char *name, const Matrix<double> &A, int n = -1,
                  int m = -1);

template <typename T>
void print_vector(const char *name, const Vector<T> &v, int n = -1);

// The Solving function, implementing the Goldfarb-Idnani method

double solve_quadprog(Matrix<double> &G, Vector<double> &g0,
                      const Matrix<double> &CE, const Vector<double> &ce0,
                      const Matrix<double> &CI, const Vector<double> &ci0,
                      Vector<double> &x) {
  std::ostringstream msg;
  unsigned int n = G.ncols(), p = CE.ncols(), m = CI.ncols();
  if (G.nrows() != n) {
    msg << "The matrix G is not a squared matrix (" << G.nrows() << " x "
        << G.ncols() << ")";
    throw std::logic_error(msg.str());
  }
  if (CE.nrows() != n) {
    msg << "The matrix CE is incompatible (incorrect number of rows "
        << CE.nrows() << " , expecting " << n << ")";
    throw std::logic_error(msg.str());
  }
  if (ce0.size() != p) {
    msg << "The vector ce0 is incompatible (incorrect dimension " << ce0.size()
        << ", expecting " << p << ")";
    throw std::logic_error(msg.str());
  }
  if (CI.nrows() != n) {
    msg << "The matrix CI is incompatible (incorrect number of rows "
        << CI.nrows() << " , expecting " << n << ")";
    throw std::logic_error(msg.str());
  }
  if (ci0.size() != m) {
    msg << "The vector ci0 is incompatible (incorrect dimension " << ci0.size()
        << ", expecting " << m << ")";
    throw std::logic_error(msg.str());
  }
  x.resize(n);
  unsigned int i, j, k, l; /* indices */
  int ip; // this is the index of the constraint to be added to the active set
  Matrix<double> R(n, n), J(n, n);
  Vector<double> s(m + p), z(n), r(m + p), d(n), np(n), u(m + p), x_old(n),
      u_old(m + p);
  double f_value, psi, c1, c2, sum, ss, R_norm;
  double inf;
  if (std::numeric_limits<double>::has_infinity)
    inf = std::numeric_limits<double>::infinity();
  else
    inf = 1.0E300;
  double t, t1, t2; /* t is the step lenght, which is the minimum of the partial
                     * step length t1 and the full step length t2 */
  Vector<int> A(m + p), A_old(m + p), iai(m + p);
  unsigned int iq, iter = 0;
  Vector<bool> iaexcl(m + p);

  /* p is the number of equality constraints */
  /* m is the number of inequality constraints */
#ifdef TRACE_SOLVER
  std::cout << std::endl << "Starting solve_quadprog" << std::endl;
  print_matrix("G", G);
  print_vector("g0", g0);
  print_matrix("CE", CE);
  print_vector("ce0", ce0);
  print_matrix("CI", CI);
  print_vector("ci0", ci0);
#endif

  /*
   * Preprocessing phase
   */

  /* compute the trace of the original matrix G */
  c1 = 0.0;
  for (i = 0; i < n; i++) {
    c1 += G[i][i];
  }
  /* decompose the matrix G in the form L^T L */
  cholesky_decomposition(G);
#ifdef TRACE_SOLVER
  print_matrix("G", G);
#endif
  /* initialize the matrix R */
  for (i = 0; i < n; i++) {
    d[i] = 0.0;
    for (j = 0; j < n; j++)
      R[i][j] = 0.0;
  }
  R_norm = 1.0; /* this variable will hold the norm of the matrix R */

  /* compute the inverse of the factorized matrix G^-1, this is the initial
   * value for H */
  c2 = 0.0;
  for (i = 0; i < n; i++) {
    d[i] = 1.0;
    forward_elimination(G, z, d);
    for (j = 0; j < n; j++)
      J[i][j] = z[j];
    c2 += z[i];
    d[i] = 0.0;
  }
#ifdef TRACE_SOLVER
  print_matrix("J", J);
#endif

  /* c1 * c2 is an estimate for cond(G) */

  /*
   * Find the unconstrained minimizer of the quadratic form 0.5 * x G x + g0 x
   * this is a feasible point in the dual space
   * x = G^-1 * g0
   */
  cholesky_solve(G, x, g0);
  for (i = 0; i < n; i++)
    x[i] = -x[i];
  /* and compute the current solution value */
  f_value = 0.5 * scalar_product(g0, x);
#ifdef TRACE_SOLVER
  std::cout << "Unconstrained solution: " << f_value << std::endl;
  print_vector("x", x);
#endif

  /* Add equality constraints to the working set A */
  iq = 0;
  for (i = 0; i < p; i++) {
    for (j = 0; j < n; j++)
      np[j] = CE[j][i];
    compute_d(d, J, np);
    update_z(z, J, d, iq);
    update_r(R, r, d, iq);
#ifdef TRACE_SOLVER
    print_matrix("R", R, n, iq);
    print_vector("z", z);
    print_vector("r", r, iq);
    print_vector("d", d);
#endif

    /* compute full step length t2: i.e., the minimum step in primal space s.t.
      the contraint becomes feasible */
    t2 = 0.0;
    if (fabs(scalar_product(z, z)) >
        std::numeric_limits<double>::epsilon()) // i.e. z != 0
      t2 = (-scalar_product(np, x) - ce0[i]) / scalar_product(z, np);

    /* set x = x + t2 * z */
    for (k = 0; k < n; k++)
      x[k] += t2 * z[k];

    /* set u = u+ */
    u[iq] = t2;
    for (k = 0; k < iq; k++)
      u[k] -= t2 * r[k];

    /* compute the new solution value */
    f_value += 0.5 * (t2 * t2) * scalar_product(z, np);
    A[i] = -i - 1;

    if (!add_constraint(R, J, d, iq, R_norm)) {
      // Equality constraints are linearly dependent
      throw std::runtime_error("Constraints are linearly dependent");
      return f_value;
    }
  }

  /* set iai = K \ A */
  for (i = 0; i < m; i++)
    iai[i] = i;

l1:
  iter++;
#ifdef TRACE_SOLVER
  print_vector("x", x);
#endif
  /* step 1: choose a violated constraint */
  for (i = p; i < iq; i++) {
    ip = A[i];
    iai[ip] = -1;
  }

  /* compute s[x] = ci^T * x + ci0 for all elements of K \ A */
  ss = 0.0;
  psi = 0.0; /* this value will contain the sum of all infeasibilities */
  ip = 0;    /* ip will be the index of the chosen violated constraint */
  for (i = 0; i < m; i++) {
    iaexcl[i] = true;
    sum = 0.0;
    for (j = 0; j < n; j++)
      sum += CI[j][i] * x[j];
    sum += ci0[i];
    s[i] = sum;
    psi += std::min(0.0, sum);
  }
#ifdef TRACE_SOLVER
  print_vector("s", s, m);
#endif

  if (fabs(psi) <=
      m * std::numeric_limits<double>::epsilon() * c1 * c2 * 100.0) {
    /* numerically there are not infeasibilities anymore */
    return f_value;
  }

  /* save old values for u and A */
  for (i = 0; i < iq; i++) {
    u_old[i] = u[i];
    A_old[i] = A[i];
  }
  /* and for x */
  for (i = 0; i < n; i++)
    x_old[i] = x[i];

l2: /* Step 2: check for feasibility and determine a new S-pair */
  for (i = 0; i < m; i++) {
    if (s[i] < ss && iai[i] != -1 && iaexcl[i]) {
      ss = s[i];
      ip = i;
    }
  }
  if (ss >= 0.0) {
    return f_value;
  }

  /* set np = n[ip] */
  for (i = 0; i < n; i++)
    np[i] = CI[i][ip];
  /* set u = [u 0]^T */
  u[iq] = 0.0;
  /* add ip to the active set A */
  A[iq] = ip;

#ifdef TRACE_SOLVER
  std::cout << "Trying with constraint " << ip << std::endl;
  print_vector("np", np);
#endif

l2a: /* Step 2a: determine step direction */
  /* compute z = H np: the step direction in the primal space (through J, see
   * the paper) */
  compute_d(d, J, np);
  update_z(z, J, d, iq);
  /* compute N* np (if q > 0): the negative of the step direction in the dual
   * space */
  update_r(R, r, d, iq);
#ifdef TRACE_SOLVER
  std::cout << "Step direction z" << std::endl;
  print_vector("z", z);
  print_vector("r", r, iq + 1);
  print_vector("u", u, iq + 1);
  print_vector("d", d);
  print_vector("A", A, iq + 1);
#endif

  /* Step 2b: compute step length */
  l = 0;
  /* Compute t1: partial step length (maximum step in dual space without
   * violating dual feasibility */
  t1 = inf; /* +inf */
  /* find the index l s.t. it reaches the minimum of u+[x] / r */
  for (k = p; k < iq; k++) {
    if (r[k] > 0.0) {
      if (u[k] / r[k] < t1) {
        t1 = u[k] / r[k];
        l = A[k];
      }
    }
  }
  /* Compute t2: full step length (minimum step in primal space such that the
   * constraint ip becomes feasible */
  if (fabs(scalar_product(z, z)) >
      std::numeric_limits<double>::epsilon()) // i.e. z != 0
  {
    t2 = -s[ip] / scalar_product(z, np);
    if (t2 < 0) // patch suggested by Takano Akio for handling numerical
                // inconsistencies
      t2 = inf;
  } else
    t2 = inf; /* +inf */

  /* the step is chosen as the minimum of t1 and t2 */
  t = std::min(t1, t2);
#ifdef TRACE_SOLVER
  std::cout << "Step sizes: " << t << " (t1 = " << t1 << ", t2 = " << t2
            << ") ";
#endif

  /* Step 2c: determine new S-pair and take step: */

  /* case (i): no step in primal or dual space */
  if (t >= inf) {
    /* QPP is infeasible */
    // FIXME: unbounded to raise
    return inf;
  }
  /* case (ii): step in dual space */
  if (t2 >= inf) {
    /* set u = u +  t * [-r 1] and drop constraint l from the active set A */
    for (k = 0; k < iq; k++)
      u[k] -= t * r[k];
    u[iq] += t;
    iai[l] = l;
    delete_constraint(R, J, A, u, n, p, iq, l);
#ifdef TRACE_SOLVER
    std::cout << " in dual space: " << f_value << std::endl;
    print_vector("x", x);
    print_vector("z", z);
    print_vector("A", A, iq + 1);
#endif
    goto l2a;
  }

  /* case (iii): step in primal and dual space */

  /* set x = x + t * z */
  for (k = 0; k < n; k++)
    x[k] += t * z[k];
  /* update the solution value */
  f_value += t * scalar_product(z, np) * (0.5 * t + u[iq]);
  /* u = u + t * [-r 1] */
  for (k = 0; k < iq; k++)
    u[k] -= t * r[k];
  u[iq] += t;
#ifdef TRACE_SOLVER
  std::cout << " in both spaces: " << f_value << std::endl;
  print_vector("x", x);
  print_vector("u", u, iq + 1);
  print_vector("r", r, iq + 1);
  print_vector("A", A, iq + 1);
#endif

  if (fabs(t - t2) < std::numeric_limits<double>::epsilon()) {
#ifdef TRACE_SOLVER
    std::cout << "Full step has taken " << t << std::endl;
    print_vector("x", x);
#endif
    /* full step has taken */
    /* add constraint ip to the active set*/
    if (!add_constraint(R, J, d, iq, R_norm)) {
      iaexcl[ip] = false;
      delete_constraint(R, J, A, u, n, p, iq, ip);
#ifdef TRACE_SOLVER
      print_matrix("R", R);
      print_vector("A", A, iq);
      print_vector("iai", iai);
#endif
      for (i = 0; i < m; i++)
        iai[i] = i;
      for (i = p; i < iq; i++) {
        A[i] = A_old[i];
        u[i] = u_old[i];
        iai[A[i]] = -1;
      }
      for (i = 0; i < n; i++)
        x[i] = x_old[i];
      goto l2; /* go to step 2 */
    } else
      iai[ip] = -1;
#ifdef TRACE_SOLVER
    print_matrix("R", R);
    print_vector("A", A, iq);
    print_vector("iai", iai);
#endif
    goto l1;
  }

  /* a patial step has taken */
#ifdef TRACE_SOLVER
  std::cout << "Partial step has taken " << t << std::endl;
  print_vector("x", x);
#endif
  /* drop constraint l */
  iai[l] = l;
  delete_constraint(R, J, A, u, n, p, iq, l);
#ifdef TRACE_SOLVER
  print_matrix("R", R);
  print_vector("A", A, iq);
#endif

  /* update s[ip] = CI * x + ci0 */
  sum = 0.0;
  for (k = 0; k < n; k++)
    sum += CI[k][ip] * x[k];
  s[ip] = sum + ci0[ip];

#ifdef TRACE_SOLVER
  print_vector("s", s, m);
#endif
  goto l2a;
}

inline void compute_d(Vector<double> &d, const Matrix<double> &J,
                      const Vector<double> &np) {
  int i, j, n = d.size();
  double sum;

  /* compute d = H^T * np */
  for (i = 0; i < n; i++) {
    sum = 0.0;
    for (j = 0; j < n; j++)
      sum += J[j][i] * np[j];
    d[i] = sum;
  }
}

inline void update_z(Vector<double> &z, const Matrix<double> &J,
                     const Vector<double> &d, int iq) {
  int i, j, n = z.size();

  /* setting of z = H * d */
  for (i = 0; i < n; i++) {
    z[i] = 0.0;
    for (j = iq; j < n; j++)
      z[i] += J[i][j] * d[j];
  }
}

inline void update_r(const Matrix<double> &R, Vector<double> &r,
                     const Vector<double> &d, int iq) {
  int i, j;
  double sum;

  /* setting of r = R^-1 d */
  for (i = iq - 1; i >= 0; i--) {
    sum = 0.0;
    for (j = i + 1; j < iq; j++)
      sum += R[i][j] * r[j];
    r[i] = (d[i] - sum) / R[i][i];
  }
}

bool add_constraint(Matrix<double> &R, Matrix<double> &J, Vector<double> &d,
                    unsigned int &iq, double &R_norm) {
  unsigned int n = d.size();
#ifdef TRACE_SOLVER
  std::cout << "Add constraint " << iq << '/';
#endif
  unsigned int i, j, k;
  double cc, ss, h, t1, t2, xny;

  /* we have to find the Givens rotation which will reduce the element
    d[j] to zero.
    if it is already zero we don't have to do anything, except of
    decreasing j */
  for (j = n - 1; j >= iq + 1; j--) {
    /* The Givens rotation is done with the matrix (cc cs, cs -cc).
    If cc is one, then element (j) of d is zero compared with element
    (j - 1). Hence we don't have to do anything.
    If cc is zero, then we just have to switch column (j) and column (j - 1)
    of J. Since we only switch columns in J, we have to be careful how we
    update d depending on the sign of gs.
    Otherwise we have to apply the Givens rotation to these columns.
    The i - 1 element of d has to be updated to h. */
    cc = d[j - 1];
    ss = d[j];
    h = distance(cc, ss);
    if (fabs(h) < std::numeric_limits<double>::epsilon()) // h == 0
      continue;
    d[j] = 0.0;
    ss = ss / h;
    cc = cc / h;
    if (cc < 0.0) {
      cc = -cc;
      ss = -ss;
      d[j - 1] = -h;
    } else
      d[j - 1] = h;
    xny = ss / (1.0 + cc);
    for (k = 0; k < n; k++) {
      t1 = J[k][j - 1];
      t2 = J[k][j];
      J[k][j - 1] = t1 * cc + t2 * ss;
      J[k][j] = xny * (t1 + J[k][j - 1]) - t2;
    }
  }
  /* update the number of constraints added*/
  iq++;
  /* To update R we have to put the iq components of the d vector
    into column iq - 1 of R
    */
  for (i = 0; i < iq; i++){
    R[i][iq - 1] = d[i];
  }
#ifdef TRACE_SOLVER
  std::cout << iq << std::endl;
  print_matrix("R", R, iq, iq);
  print_matrix("J", J);
  print_vector("d", d, iq);
#endif

  if (fabs(d[iq - 1]) <= std::numeric_limits<double>::epsilon() * R_norm) {
    // problem degenerate
    return false;
  }
  R_norm = std::max<double>(R_norm, fabs(d[iq - 1]));
  return true;
}

void delete_constraint(Matrix<double> &R, Matrix<double> &J, Vector<int> &A,
                       Vector<double> &u, unsigned int n, int p,
                       unsigned int &iq, int l) {
#ifdef TRACE_SOLVER
  std::cout << "Delete constraint " << l << ' ' << iq;
#endif
  unsigned int i, j, k, qq = 0; // just to prevent warnings from smart compilers
  double cc, ss, h, xny, t1, t2;

  bool found = false;
  /* Find the index qq for active constraint l to be removed */
  for (i = p; i < iq; i++)
    if (A[i] == l) {
      qq = i;
      found = true;
      break;
    }

  if (!found) {
    std::ostringstream os;
    os << "Attempt to delete non existing constraint, constraint: " << l;
    throw std::invalid_argument(os.str());
  }
  /* remove the constraint from the active set and the duals */
  for (i = qq; i < iq - 1; i++) {
    A[i] = A[i + 1];
    u[i] = u[i + 1];
    for (j = 0; j < n; j++)
      R[j][i] = R[j][i + 1];
  }

  A[iq - 1] = A[iq];
  u[iq - 1] = u[iq];
  A[iq] = 0;
  u[iq] = 0.0;
  for (j = 0; j < iq; j++)
    R[j][iq - 1] = 0.0;
  /* constraint has been fully removed */
  iq--;
#ifdef TRACE_SOLVER
  std::cout << '/' << iq << std::endl;
#endif

  if (iq == 0)
    return;

  for (j = qq; j < iq; j++) {
    cc = R[j][j];
    ss = R[j + 1][j];
    h = distance(cc, ss);
    if (fabs(h) < std::numeric_limits<double>::epsilon()) // h == 0
      continue;
    cc = cc / h;
    ss = ss / h;
    R[j + 1][j] = 0.0;
    if (cc < 0.0) {
      R[j][j] = -h;
      cc = -cc;
      ss = -ss;
    } else
      R[j][j] = h;

    xny = ss / (1.0 + cc);
    for (k = j + 1; k < iq; k++) {
      t1 = R[j][k];
      t2 = R[j + 1][k];
      R[j][k] = t1 * cc + t2 * ss;
      R[j + 1][k] = xny * (t1 + R[j][k]) - t2;
    }
    for (k = 0; k < n; k++) {
      t1 = J[k][j];
      t2 = J[k][j + 1];
      J[k][j] = t1 * cc + t2 * ss;
      J[k][j + 1] = xny * (J[k][j] + t1) - t2;
    }
  }
}

inline double distance(double a, double b) {
  double a1, b1, t;
  a1 = fabs(a);
  b1 = fabs(b);
  if (a1 > b1) {
    t = (b1 / a1);
    return a1 * sqrt(1.0 + t * t);
  } else if (b1 > a1) {
    t = (a1 / b1);
    return b1 * sqrt(1.0 + t * t);
  }
  return a1 * sqrt(2.0);
}

inline double scalar_product(const Vector<double> &x, const Vector<double> &y) {
  int i, n = x.size();
  double sum;

  sum = 0.0;
  for (i = 0; i < n; i++)
    sum += x[i] * y[i];
  return sum;
}

void cholesky_decomposition(Matrix<double> &A) {
  int i, j, k, n = A.nrows();
  double sum;

  for (i = 0; i < n; i++) {
    for (j = i; j < n; j++) {
      sum = A[i][j];
      for (k = i - 1; k >= 0; k--)
        sum -= A[i][k] * A[j][k];
      if (i == j) {
        if (sum <= 0.0) {
          std::ostringstream os;
          // raise error
          print_matrix("A", A);
          os << "Error in cholesky decomposition, sum: " << sum;
          throw std::logic_error(os.str());
          exit(-1);
        }
        A[i][i] = sqrt(sum);
      } else
        A[j][i] = sum / A[i][i];
    }
    for (k = i + 1; k < n; k++)
      A[i][k] = A[k][i];
  }
}

void cholesky_solve(const Matrix<double> &L, Vector<double> &x,
                    const Vector<double> &b) {
  int n = L.nrows();
  Vector<double> y(n);

  /* Solve L * y = b */
  forward_elimination(L, y, b);
  /* Solve L^T * x = y */
  backward_elimination(L, x, y);
}

inline void forward_elimination(const Matrix<double> &L, Vector<double> &y,
                                const Vector<double> &b) {
  int i, j, n = L.nrows();

  y[0] = b[0] / L[0][0];
  for (i = 1; i < n; i++) {
    y[i] = b[i];
    for (j = 0; j < i; j++)
      y[i] -= L[i][j] * y[j];
    y[i] = y[i] / L[i][i];
  }
}

inline void backward_elimination(const Matrix<double> &U, Vector<double> &x,
                                 const Vector<double> &y) {
  int i, j, n = U.nrows();

  x[n - 1] = y[n - 1] / U[n - 1][n - 1];
  for (i = n - 2; i >= 0; i--) {
    x[i] = y[i];
    for (j = i + 1; j < n; j++)
      x[i] -= U[i][j] * x[j];
    x[i] = x[i] / U[i][i];
  }
}

void print_matrix(const char *name, const Matrix<double> &A, int n, int m) {
  std::ostringstream s;
  std::string t;
  if (n == -1)
    n = A.nrows();
  if (m == -1)
    m = A.ncols();

  s << name << ": " << std::endl;
  for (int i = 0; i < n; i++) {
    s << " ";
    for (int j = 0; j < m; j++)
      s << A[i][j] << ", ";
    s << std::endl;
  }
  t = s.str();
  t = t.substr(0,
               t.size() - 3); // To remove the trailing space, comma and newline

  std::cout << t << std::endl;
}

template <typename T>
void print_vector(const char *name, const Vector<T> &v, int n) {
  std::ostringstream s;
  std::string t;
  if (n == -1)
    n = v.size();

  s << name << ": " << std::endl << " ";
  for (int i = 0; i < n; i++) {
    s << v[i] << ", ";
  }
  t = s.str();
  t = t.substr(0, t.size() - 2); // To remove the trailing space and comma

  std::cout << t << std::endl;
}

} // namespace quadprogpp


/* start */

namespace QPSolver {
// ret[t][i] = tターン目にi番目の色を使う量
pair<vector<vector<double>>, double> Solve(const vector<Vec3> &colors,
                                           const vector<double> &amounts,
                                           vector<Vec3> targets,
                                           const double mainWellAmount,
                                           const Vec3 mainWellColor) {
  double total = std::accumulate(amounts.begin(), amounts.end(), 0.0);
  assert(fabs(total - targets.size() + mainWellAmount) < 1e-8 && "sum(amounts) must equal T");

  targets[0] -= mainWellColor * mainWellAmount; // 中央ウェルの色を引く
  /*
  min 0.5 * x G x + g0 x
  s.t.
      CE^T x + ce0 = 0
      CI^T x + ci0 >= 0
  */

  // x[t * N + i] = t番目のターゲットに対して、i番目の色を使う量
  // Fx -> 出来上がるベクトル(3 * T)
  // Fは3T x NTの行列
  // 0.5 (Fx - y)^T (Fx - y) = 0.5 x^T F^T F x - (y^T F)x + 0.5 y^T y
  // G -> F^T F
  // g0 -> -y^T F

  // CE: T個 -> 合計が1.0
  //   : N-1個 -> 合計がamounts[i]
  //   (最後の1つは他の制約で決まるので、N-1個にする)
  // CI: N*T個 -> 全ての要素が0以上

  const int N = colors.size();
  const int T = targets.size();
  const int n = T * N; // 変数の数
  const int p = T + N - 1;
  const int m = n;
  quadprogpp::Matrix<double> G(0.0, n, n), CE(0.0, n, p), CI(0.0, n, m);
  quadprogpp::Vector<double> g0(0.0, n), ce0(0.0, p), ci0(0.0, m), x(0.0, n),
      y(0.0, T * 3);

  // 初期解
  // rep(t, T) {
  //   rep(i, N) {
  //     x[t * N + i] = amounts[i] / T; // 各ターゲットに均等に割り当てる
  //   }
  // }

  // y[t * 3 + i] = t番目のターゲットのi成分
  for (int t = 0; t < T; ++t) {
    y[t * 3 + 0] = targets[t].x;
    y[t * 3 + 1] = targets[t].y;
    y[t * 3 + 2] = targets[t].z;
  }

  quadprogpp::Matrix<double> F(0.0, T * 3, T * N);
  for (int t = 0; t < T; ++t) {
    for (int i = 0; i < N; ++i) {
      F[t * 3 + 0][t * N + i] = colors[i].x;
      F[t * 3 + 1][t * N + i] = colors[i].y;
      F[t * 3 + 2][t * N + i] = colors[i].z;
    }
  }

  // print_matrix("F", F);

  // G = F^T * F
  rep(i, n) {
    rep(j, n) {
      G[i][j] = 0.0;
      for (int t = 0; t < T * 3; ++t) {
        G[i][j] += F[t][i] * F[t][j];
      }
    }
  }

  // 正定値にする
  rep(i, n) {
    G[i][i] += 1e-12; // 対角成分に小さい値を加える
  }

  // g0 = -y^T * F
  for (int i = 0; i < n; ++i) {
    g0[i] = 0.0;
    for (int t = 0; t < T * 3; ++t) {
      g0[i] -= y[t] * F[t][i];
    }
  }

  // CE: T個 -> 合計が1.0
  for (int t = 0; t < T; ++t) {
    for (int i = 0; i < N; ++i) {
      CE[t * N + i][t] = 1.0;
    }
    ce0[t] = t == 0 ? -1.0 + mainWellAmount : -1.0;
  }
  // CE: N-1個 -> 合計がamounts[i]
  // 最後の1つは他の制約で決まるので、N-1個にする
  for (int i = 0; i < N - 1; ++i) {
    for (int t = 0; t < T; ++t) {
      CE[t * N + i][T + i] = 1.0;
    }
    ce0[T + i] = -amounts[i];
  }

  // CI: N*T個 -> 全ての要素が0以上
  for (int i = 0; i < m; ++i) {
    CI[i][i] = 1.0; // 対角成分
  }

  // print_matrix("G", G);
  // exit(0);

  auto error = quadprogpp::solve_quadprog(G, g0, CE, ce0, CI, ci0, x);

  // 0.5 * y^T yを足す
  rep(i, T * 3) { error += 0.5 * y[i] * y[i]; }

  // print_vector("x", x, n);

  vector<vector<double>> result(T, vector<double>(N, 0.0));
  rep(t, T) {
    rep(i, N) { result[t][i] = x[t * N + i]; }
  }

  // 0.5 (Fx - y)^T (Fx - y) = 0.5 x^T F^T F x - (y^T F)x + 0.5 y^T yを計算
  double debugError = 0.0;
  for (int t = 0; t < T * 3; ++t) {
    double fx = 0.0;
    for (int i = 0; i < N * T; ++i) {
      fx += F[t][i] * x[i];
    }
    debugError += 0.5 * (fx - y[t]) * (fx - y[t]);
  }
  cerr << "Actual error: " << error << endl;
  cerr << "Expected error: " << debugError << endl;

  return {result, error}; // retは最適値
}
} // namespace QPSolver










#include <iostream>

/* start */

// colorの1.0に相当する、(a,b,c,d)の量
vector<map<int, vector<double>>> color2FourColors;

vector<double> &Color2FourColorsAt(const int baseColor, const int k1,
                                   const int k2, const int k3, const int k4) {
  if (color2FourColors.empty()) {
    color2FourColors.resize(K);
  }
  return color2FourColors[baseColor][k1 * K * K * K + k2 * K * K + k3 * K + k4];
}

void InitColor2FourColors(vector<int> useColors) {
  if (useColors.size() <= 4u)
    return;
  color2FourColors.resize(K);
  for (const auto baseColor : useColors) {
    rep(idx1, useColors.size()) {
      const int k1 = useColors[idx1];
      if (k1 == baseColor)
        continue;
      rep(idx2, idx1) {
        const int k2 = useColors[idx2];
        if (k2 == baseColor)
          continue;
        rep(idx3, idx2) {
          const int k3 = useColors[idx3];
          if (k3 == baseColor)
            continue;
          rep(idx4, idx3) {
            const int k4 = useColors[idx4];
            if (k4 == baseColor)
              continue;

            // baseColorを,k1からk4の和で表す
            const auto &c1 = ownColorVecs[k1];
            const auto &c2 = ownColorVecs[k2];
            const auto &c3 = ownColorVecs[k3];
            const auto &c4 = ownColorVecs[k4];
            const auto &baseColorVec = ownColorVecs[baseColor];

            OptimalColorRatioResult ratioResult =
                OptimalColorRatio(baseColorVec, 1.0, c1, c2, c3, c4,
                                  Vec3(0.0, 0.0, 0.0), 0.0, false);
            // デバッグ
            const auto mergedVec = c1 * ratioResult.c1 + c2 * ratioResult.c2 +
                                   c3 * ratioResult.c3 + c4 * ratioResult.c4;
            assert(mergedVec.distance(baseColorVec) < 1e-6);

            // 登録
            vector<int> fourColors = {k1, k2, k3, k4};
            vector<double> fourAmounts = {ratioResult.c1, ratioResult.c2,
                                          ratioResult.c3, ratioResult.c4};
            vector<int> perm(4);
            iota(perm.begin(), perm.end(), 0);
            do {
              vector<int> permColors(4);
              vector<double> permAmounts(4);
              rep(i, 4) {
                permColors[i] = fourColors[perm[i]];
                permAmounts[i] = fourAmounts[perm[i]];
              }
              Color2FourColorsAt(baseColor, permColors[0], permColors[1],
                                 permColors[2], permColors[3]) = permAmounts;
            } while (next_permutation(perm.begin(), perm.end()));
          }
        }
      }
    }
  }
}

struct TurnColorAmounts {
  int targetIndex;
  map<int, double> colorAmounts; // 合計は1.0
  Vec3 mergedVec;

  TurnColorAmounts(int targetIndex_, const map<int, double> &amounts)
      : targetIndex(targetIndex_), colorAmounts(amounts) {
    assert(colorAmounts.size() == 4u);
    mergedVec = Vec3(0.0, 0.0, 0.0);
    for (const auto &pair : colorAmounts) {
      mergedVec += ownColorVecs[pair.first] * pair.second;
    }
  }

  static TurnColorAmounts CreateInitial(const int targetIndex,
                                        const vector<int> &useColors) {
    // 最適な組み合わせのうちの1つを選ぶ
    double minError = LINF;
    map<int, double> bestAmounts;
    Vec3 bestMergedVec(0.0, 0.0, 0.0);
    rep(idx4, useColors.size()) {
      rep(idx3, idx4) {
        rep(idx2, idx3) {
          rep(idx1, idx2) {
            const int k1 = useColors[idx1];
            const int k2 = useColors[idx2];
            const int k3 = useColors[idx3];
            const int k4 = useColors[idx4];

            // 4色の量を求める
            const auto &c1 = ownColorVecs[k1];
            const auto &c2 = ownColorVecs[k2];
            const auto &c3 = ownColorVecs[k3];
            const auto &c4 = ownColorVecs[k4];
            auto projectedTargetColorVec = targetColorVecs[targetIndex];

            // 外側にある場合は、射影
            if (!is_inside_tetrahedron(projectedTargetColorVec, c1, c2, c3,
                                       c4)) {
              projectedTargetColorVec = project_point_to_tetrahedron(
                  projectedTargetColorVec, c1, c2, c3, c4);
            }

            // 最適な色の比率を求める
            OptimalColorRatioResult ratioResult =
                OptimalColorRatio(projectedTargetColorVec, 1.0, c1, c2, c3, c4,
                                  Vec3(0.0, 0.0, 0.0), 0.0, false);

            // 最適なら更新
            const auto mergedVec = c1 * ratioResult.c1 + c2 * ratioResult.c2 +
                                   c3 * ratioResult.c3 + c4 * ratioResult.c4;
            const double error =
                targetColorVecs[targetIndex].distance(mergedVec);
            if (error < minError) {
              minError = error;
              bestAmounts.clear();
              bestAmounts[k1] = ratioResult.c1;
              bestAmounts[k2] = ratioResult.c2;
              bestAmounts[k3] = ratioResult.c3;
              bestAmounts[k4] = ratioResult.c4;
              bestMergedVec = mergedVec;
            }
          }
        }
      }
    }

    return TurnColorAmounts(targetIndex, bestAmounts);
  }

  void Finish() {
    for (auto itr = colorAmounts.begin(); itr != colorAmounts.end();) {
      if (itr->second < 0.0) {
        assert(abs(itr->second) < 1e-6);
        itr = colorAmounts.erase(itr);
      } else {
        itr++;
      }
    }
  }

  const Vec3 &MergedVec() const { return mergedVec; }

  const map<int, double> &ColorAmounts() const { return colorAmounts; }

  double TargetError() const {
    return targetColorVecs[targetIndex].distance(mergedVec);
  }

  // 使われている色
  vector<int> UsedColors() const {
    vector<int> usedColors;
    for (const auto &pair : colorAmounts) {
      usedColors.push_back(pair.first);
    }
    return usedColors;
  }

  bool IsUsedColor(const int color) const {
    return colorAmounts.count(color) > 0;
  }

  double Amount(const int color) const {
    if (colorAmounts.count(color) == 0) {
      return 0.0;
    }
    return colorAmounts.at(color);
  }

  void RemoveAmaris(const map<int, double> &amariAmounts) {
    // 端数を受け取って、減らす
    for (const auto &pair : amariAmounts) {
      const int k = pair.first;
      colorAmounts[k] -= pair.second;
    }

    mergedVec = Vec3(0.0, 0.0, 0.0);
    for (const auto &pair : colorAmounts) {
      mergedVec += ownColorVecs[pair.first] * pair.second;
    }
  }

  void SetAmountForce(map<int, double> newAmounts) {
    // 強制的に量を設定
    colorAmounts = newAmounts;
    mergedVec = Vec3(0.0, 0.0, 0.0);
    for (const auto &pair : colorAmounts) {
      mergedVec += ownColorVecs[pair.first] * pair.second;
    }
  }

  // 新たな色を追加し、1色削除する
  int UseNewColor(const int newColor) {
    assert(colorAmounts.count(newColor) == 0);
    assert(colorAmounts.size() == 4u);

    const auto usedColors = UsedColors();

    // 新たな色に対応する量を計算
    const auto correspondingAmounts = Color2FourColorsAt(
        newColor, usedColors[0], usedColors[1], usedColors[2], usedColors[3]);

    // newColorの量を増やせる最小値を求め、原因となっている色を特定する
    double minAmount = LINF;
    int minIndex = -1;
    for (int i = 0; i < 4; ++i) {
      const auto k = usedColors[i];
      const auto deltaAmount = correspondingAmounts[i];
      if (deltaAmount < 0.0) {
        // newColorを増やすと、kも増えるので、無限に増やせる
        continue;
      }

      const auto currentAmount = colorAmounts[k];
      // newColorを1.0増やすと、kがamountだけ減る
      // なので、newColorはcurrentAmount / deltaAmountだけ増やせる
      const auto amount = currentAmount / deltaAmount;
      if (amount < minAmount) {
        minAmount = amount;
        minIndex = i;
      }
    }

    assert(minIndex != -1);
    // minIndexの色を削除し、newColorを追加
    const auto k = usedColors[minIndex];
    colorAmounts.erase(k);
    colorAmounts[newColor] = minAmount;
    rep(idx, 4) {
      const int kkk = usedColors[idx];
      if (kkk == k)
        continue;
      // newColorを増やした分だけ、減らす
      colorAmounts[kkk] -= minAmount * correspondingAmounts[idx];
      assert(colorAmounts[kkk] >= -1e-6);
    }

#ifndef NDEBUG
    // デバッグ
    // マージ結果の色は変化しないはず
    Vec3 newMergedVec(0.0, 0.0, 0.0);
    for (const auto &pair : colorAmounts) {
      newMergedVec += ownColorVecs[pair.first] * pair.second;
    }
    // cerr << newMergedVec.distance(mergedVec) << endl;
    assert(newMergedVec.distance(mergedVec) < 1e-6);
#endif

    return k;
  }

  // 各色の最大量を指定し、その量以下になるように配分を調整する
  void SetColorAmount(map<int, double> maxColorAmounts) {
    vector<int> allColors;
    for (const auto [color, _] : maxColorAmounts) {
      allColors.push_back(color);
    }
    const int kIter = 100;
    rep(t, kIter) {
      bool updated = false;
      for (const auto [color, maxAmount] : maxColorAmounts) {
        const auto actualAmount = colorAmounts[color];
        if (actualAmount - 1e-8 <= maxAmount) {
          continue;
        }
        // cerr << color << " " << actualAmount << " " << maxAmount << endl;
        updated = true;

        // その色の量をmaxAmountで無理やり固定
        set<int> otherColors;
        while (otherColors.size() < 4u) {
          const int otherColor = rand_vec(allColors);
          if (color == otherColor)
            continue;
          otherColors.insert(otherColor);
        }
        vector<int> otherColorsVec(otherColors.begin(), otherColors.end());

        // 量を調整する
        const auto fourColors =
            Color2FourColorsAt(color, otherColorsVec[0], otherColorsVec[1],
                               otherColorsVec[2], otherColorsVec[3]);
        // 量を調整
        double deltaAmount = maxAmount - actualAmount;
        // 他の色が負にならないように
        rep(k, 4) {
          if (fourColors[k] >= 0.0) {
            // 減らすと増えるので、大丈夫
            continue;
          }
          // 負の値なので、chmax
          chmax(deltaAmount, colorAmounts[otherColorsVec[k]] / fourColors[k]);
        }
        colorAmounts[color] += deltaAmount;
        rep(k, 4) {
          colorAmounts[otherColorsVec[k]] -= deltaAmount * fourColors[k];
        }
      }
      if (!updated) {
        break;
      }

      if (kIter - 1 == t) {
        // 無理やり調整
        // TODO: 丁寧に
        for (const auto [color, maxAmount] : maxColorAmounts) {
          const auto actualAmount = colorAmounts[color];
          if (actualAmount - 1e-8 <= maxAmount) {
            continue;
          }
          double deltaAmount = actualAmount - maxAmount;
          colorAmounts[color] -= deltaAmount;
          for (const auto [otherColor, maxAmount] : maxColorAmounts) {
            if (color == otherColor)
              continue;

            const double increasableAmount =
                min(deltaAmount, maxAmount - colorAmounts[otherColor]);
            if (increasableAmount <= 0.0)
              continue;
            colorAmounts[otherColor] += increasableAmount;
            deltaAmount -= increasableAmount;
          }
        }
      }
    }
  }
};

struct SolutionDouble {
  vector<TurnColorAmounts> turn2ColorAmounts; // 各ターンで使用する色の量
  map<int, double> colorAmountSums;
  // 目標値
  map<int, double> targetColorAmountSums;
  mutable double diffSum = -1.0;
  double errorSum = 0.0;

  SolutionDouble() {}

  explicit SolutionDouble(vector<TurnColorAmounts> turn2ColorAmounts_)
      : turn2ColorAmounts(turn2ColorAmounts_) {
    // 各色の合計量を計算
    for (const auto &turnAmounts : turn2ColorAmounts) {
      for (const auto &pair : turnAmounts.colorAmounts) {
        colorAmountSums[pair.first] += pair.second;
      }
      errorSum += turnAmounts.TargetError();
    }
  }

  static SolutionDouble CreateInitial(const vector<int> &useColors) {
    vector<TurnColorAmounts> turn2ColorAmounts;
    for (int i = 0; i < H; ++i) {
      turn2ColorAmounts.push_back(
          TurnColorAmounts::CreateInitial(i, useColors));
    }
    return SolutionDouble(turn2ColorAmounts);
  }

  // 最後の処理
  // 少しだけ負の値が合ったら、0にする
  void Finish() {
    rep(turn, H) { turn2ColorAmounts[turn].Finish(); }
  }

  int PutCount(const int colorIndex) const {
    const auto amount = colorAmountSums.at(colorIndex);
    return (int)std::round(amount);
  }

  double PutCountDouble(const int colorIndex) const {
    return colorAmountSums.at(colorIndex);
  }

  void
  SetTargetColorAmountSums(const map<int, double> &targetColorAmountSums_) {
    targetColorAmountSums = targetColorAmountSums_;
    diffSum = -1.0; // 合計量の余りを再計算する
  }

  double DiffSum() const {
    if (diffSum > -1.0) {
      // 計算済みの結果
      return diffSum;
    }
    diffSum = 0.0;
    for (const auto [k, amount] : colorAmountSums) {
      diffSum += abs(amount - targetColorAmountSums.at(k));
    }
    return diffSum;
  }

  double DiffSumPartial() const {
    int count = 0;
    double sum = 0.0;
    for (const auto [k, amount] : colorAmountSums) {
      count++;
      if (count > 4) {
        // 最初の4色以外を対象とする
        sum += abs(amount - targetColorAmountSums.at(k));
      }
    }

    return sum;
  }

  double ErrorSum() const { return errorSum; }

  double AmariSum() const {
    double amariSumAbs = 0.0;
    double amariSum = 0.0;
    for (const auto [k, amount] : colorAmountSums) {
      amariSumAbs += abs(std::round(amount) - amount);
      amariSum += amount - std::round(amount);
    }
    return amariSumAbs + abs(amariSum);
  }

  int TotalPutCount() const {
    int totalCount = 0;
    for (const auto &pair : colorAmountSums) {
      totalCount += (int)std::round(pair.second);
    }
    return totalCount;
  }

  Vec3 MergedVecSum() const {
    Vec3 mergedVec(0.0, 0.0, 0.0);
    for (const auto &turn : turn2ColorAmounts) {
      mergedVec += turn.mergedVec;
    }
    return mergedVec;
  }

  const TurnColorAmounts &GetTurnAmounts(const int turn) const {
    assert(turn < H);
    return turn2ColorAmounts[turn];
  }

  TurnColorAmounts &GetTurnAmounts(const int turn) {
    assert(turn < H);
    return turn2ColorAmounts[turn];
  }

  int UseNewColor(const int turn, const int newColor) {
    assert(turn < H);
    diffSum = -1.0; // 合計量の差分を再計算する
    auto &turnAmounts = turn2ColorAmounts[turn];

    // 合計量を更新
    for (const auto &pair : turnAmounts.colorAmounts) {
      colorAmountSums[pair.first] -= pair.second;
    }

    const auto removedColor = turnAmounts.UseNewColor(newColor);

    // 合計量を更新
    for (const auto &pair : turnAmounts.colorAmounts) {
      colorAmountSums[pair.first] += pair.second;
    }

    return removedColor;
  }

  // 1.01 -> 0.01を返す 0.99 -> -0.01を返す
  map<int, double> GetAllAmaris() const {
    map<int, double> amaris;
    for (const auto &pair : colorAmountSums) {
      const int k = pair.first;
      const double amount = pair.second;
      const double amari = amount - std::round(amount);
      amaris[k] = amari;
    }
    return amaris;
  }

  // 指定のターンに、全ての余りを押し付けることを試みる
  bool TryAddAmaris(const int turn) {
    const auto allAmaris = GetAllAmaris();
    auto &turnAmounts = turn2ColorAmounts[turn];
    // 余りを処理できるか確認
    for (const auto [k, amari] : allAmaris) {
      // 増やすのは可能
      if (amari <= 0.0)
        continue;

      // 減らせるか
      if (turnAmounts.Amount(k) - amari < 0.0) {
        // 減らせないので、失敗
        return false;
      }
    }

    // 合計量を更新
    for (const auto &pair : turnAmounts.ColorAmounts()) {
      colorAmountSums[pair.first] -= pair.second;
    }

    // 余りを処理可能なので、押し付ける
    errorSum -= turnAmounts.TargetError();
    turnAmounts.RemoveAmaris(allAmaris);
    errorSum += turnAmounts.TargetError();

    // 合計量を更新
    for (const auto &pair : turnAmounts.ColorAmounts()) {
      colorAmountSums[pair.first] += pair.second;
    }

    diffSum = -1.0; // 合計量の差分を再計算する
    return true;
  }

  void SetColorAmount(const int turn, const map<int, double> &maxColorAmounts) {
    turn2ColorAmounts[turn].SetColorAmount(maxColorAmounts);
  }
};

namespace TotalSolver {
vector<double> TargetDoubleAmounts(const vector<int> useColors,
                                   Vec3 targetMergedVecSum) {
  vector<double> colorAmounts(useColors.size(), 0.0);
  // 最初の4色は、方程式を解いて決める
  colorAmounts[0] = -1;
  colorAmounts[1] = -1;
  colorAmounts[2] = -1;
  colorAmounts[3] = -1;

  auto bestColorAmounts = colorAmounts;
  double minVecDiff = LINF;

  const auto eval = [&](const int currentSumAmount) {
    // 残りの4色を決めて、余りを評価する
    auto remainTargetVec = targetMergedVecSum;
    for (int i = 4; i < (int)useColors.size(); ++i) {
      remainTargetVec -= ownColorVecs[useColors[i]] * colorAmounts[i];
    }
    OptimalColorRatioResult ratioResult = OptimalColorRatio(
        remainTargetVec, 1000 - currentSumAmount, ownColorVecs[useColors[0]],
        ownColorVecs[useColors[1]], ownColorVecs[useColors[2]],
        ownColorVecs[useColors[3]], Vec3(0.0, 0.0, 0.0), 0.0, false);
    rep(k, 4) {
      const auto prop = ratioResult.GetProp(k);
      if (prop < 50.0 || 1000.0 / useColors.size() * 1.5 <= prop) {
        // 少なすぎたり多すぎたりする場合は無効
        return;
      }
      remainTargetVec -= ownColorVecs[useColors[k]] *
                         std::round(prop); // 残りのターゲットベクトルを更新
      colorAmounts[k] = prop;
    }

    const double diff = remainTargetVec.distance(Vec3(0.0, 0.0, 0.0));
    if (diff < minVecDiff) {
      minVecDiff = diff;
      bestColorAmounts = colorAmounts;
    }
  };

  const auto func = [&](auto self, const int nextIndex,
                        const int currentSumAmount) -> void {
    if (nextIndex == (int)useColors.size()) {
      // 4色以外が決まったので、評価
      eval(currentSumAmount);
      return;
    }

    // 次の色の使用量を決める
    for (int amount = 50; amount + currentSumAmount <= 950; ++amount) {
      if (amount >= 1000.0 / useColors.size() * 1.5) {
        // 多すぎるのでスキップ
        break;
      }
      colorAmounts[nextIndex] = amount;
      self(self, nextIndex + 1, currentSumAmount + amount);
    }
  };

  func(func, 4, 0);

  cerr << "Best color amounts: [ ";
  for (const auto &amount : bestColorAmounts) {
    cerr << amount << " ";
  }
  cerr << "]" << endl;
  cerr << "Min Amari Sum: " << minVecDiff << endl;
  return bestColorAmounts;
}

SolutionDouble Solve(const vector<int> useColors) {
  const int colorCount = useColors.size();
  // 色と4色の対応関係を事前計算
  InitColor2FourColors(useColors);

  // とりあえず初期解を作る
  SolutionDouble solution = SolutionDouble::CreateInitial(useColors);

  // 合計がこのベクトルになるようにする
  // TODO: 射影をするケースがあるなら、そこで調整することを検討する
  const auto mergedVecSum = solution.MergedVecSum();

  // mergedVecSumになるような、余りの少ない各色の量を、doubleで求める
  const auto targetDoubleAmounts = TargetDoubleAmounts(useColors, mergedVecSum);

  // 目標値を設定
  map<int, double> targetColorAmountSums;
  for (int i = 0; i < colorCount; ++i) {
    targetColorAmountSums[useColors[i]] = targetDoubleAmounts[i];
  }
  solution.SetTargetColorAmountSums(targetColorAmountSums);

  double minDiffSum = solution.DiffSumPartial();
  // 上手く行かなかったときの保険
  double minAmariSum = INF;
  SolutionDouble minAmariSolution = solution;

  // targetDoubleAmountsになるように、ターンごとの色の量を調整する
  // k4以降の差分を合わせていけばOK
  rep(iter, 30000) {
    const auto colorIdx = rand(0, colorCount);
    const int k = useColors[colorIdx];
    const int turn = rand(0, H);
    if (solution.GetTurnAmounts(turn).IsUsedColor(k)) {
      // 既に使用している色はスキップ
      continue;
    }
    const auto removedColor = solution.UseNewColor(turn, k);
    const double diffSum = solution.DiffSumPartial();
    if (diffSum < minDiffSum) {
      minDiffSum = diffSum;
      // cerr << "New best diff sum: " << minDiffSum << endl;
      double amariSum = solution.AmariSum();
      if (amariSum < minAmariSum && solution.TotalPutCount() == 1000) {
        minAmariSum = amariSum;
        minAmariSolution = solution;
        cerr << "New best amari sum: " << minAmariSum << endl;
      }
    } else {
      // 戻す
      solution.UseNewColor(turn, removedColor);
    }
  }

  const auto amariSum = solution.AmariSum();
  if (solution.TotalPutCount() != 1000 || amariSum >= 1.0) {
    // 失敗
    solution = minAmariSolution;
    cerr << "Failed to reduce amari sum, using the best solution." << endl;
    cerr << "Min amari sum: " << minAmariSum << endl;
    // exit(0);
  }

  cerr << "Color amounts: ";
  for (const auto color : useColors) {
    cerr << solution.PutCountDouble(color) << " ";
  }
  cerr << endl;

  // // あまりを、なるべく後のターンに押し付ける
  // REP(turn, H) {
  //   if (solution.TryAddAmaris(turn)) {
  //     cerr << "Added amaris to turn " << turn << endl;
  //     break;
  //   }
  //   assert(turn != 0);
  // }

  cerr << "Final diff sum: " << solution.DiffSum() << endl;
  cerr << "Final amari sum: " << solution.AmariSum() << endl;
  cerr << "Final error sum: " << solution.ErrorSum() << endl;

  solution.Finish();

  // map<int, double> colorAmountSums;
  // rep(turn, H){
  //   const auto& turnAmounts = solution.GetTurnAmounts(turn);
  //   Vec3 mergedVec = turnAmounts.MergedVec();
  //   Vec3 sum(0.0, 0.0, 0.0);
  //   for(const auto pa : turnAmounts.ColorAmounts()){
  //     const int color = pa.first;
  //     const double amount = pa.second;
  //     colorAmountSums[color] += amount;
  //     sum += ownColorVecs[color] * amount;
  //   }
  //   cerr << sum.distance(mergedVec) << endl;
  // }
  // exit(0);

  return solution;
}
} // namespace TotalSolver

/* start */

namespace Solve5 {

vector<pair<double, int>> debug;
SolutionDouble solutionDouble;
// submitVecSum + 残りのtargetのSum と、
// putされるベクトルの和の距離が、
// 最終的に誤差となって降ってくる
Vec3 submitVecSum;
Vec3 remainTargetVecSum;
Vec3 putVecSum;

double stepMsSum = 0.0;
int stepCount = 0;

struct State {
  Well mainWell;
  vector<Pos> mainPoses;
  vector<Bottle *> bottles;
  int totalPutCount = 0;

  State(vector<Pos> mainPoses, vector<Bottle *> bottles_)
      : mainPoses(mainPoses), bottles(bottles_) {
    int mainWellCellCount = 0;
    for (const auto &bottle : bottles) {
      mainWellCellCount += bottle->MainWellCellCount();
    }

    mainWell = Well(0.0, mainWellCellCount, Color{0.0, 0.0, 0.0});
  }

  InitialPartition GetInitialPartition() const {
    InitialPartition partition;
    for (const auto &bottle : bottles) {
      partition.Merge(bottle->GetInitialPartition());
    }
    partition.Merge(InitialPartition(mainPoses));
    return partition;
  }

  const Bottle *GetBottle(const int index) const { return bottles[index]; }
  Bottle *GetBottle(const int index) { return bottles[index]; }

  vector<int> ColorIndexs() const {
    vector<int> colors;
    for (const auto &bottle : bottles) {
      colors.push_back(bottle->GetColorIndex());
    }
    return colors;
  }

  vector<Vec3> Colors() const {
    vector<Vec3> colors;
    for (const auto &bottle : bottles) {
      colors.push_back(bottle->GetColor());
    }
    return colors;
  }

  vector<double> RemainAmounts() const {
    vector<double> amounts;
    for (const auto &bottle : bottles) {
      amounts.push_back(bottle->RemainAmount());
    }
    return amounts;
  }

  const vector<Bottle *> &GetBottles() const { return bottles; }
};

class Solver {
public:
  Solver() {}
  State InitState() {
    submitVecSum = Vec3(0.0, 0.0, 0.0);
    remainTargetVecSum = Vec3(0.0, 0.0, 0.0);
    putVecSum = Vec3(0.0, 0.0, 0.0);
    rep(t, H) { remainTargetVecSum += targetColorVecs[t]; }

    const auto [mainPoses, lanePoses] =
        K == 4
            ? Layout::
                  FourLane() // Dが大きいときは、K==4出ない限りは5レーン以上を使う
        : K == 5 ? Layout::FiveLane()
                 : Layout::SixLane();

    vector<Bottle *> bottles;
    auto colorIndicies = lanePoses.size() == 4u   ? bestColorIndexesFour
                         : lanePoses.size() == 5u ? bestColorIndexesFive
                                                  : bestColorIndexesSix;
    solutionDouble = TotalSolver::Solve(colorIndicies);
    double ms = global_timer.ms();
    if (colorIndicies.size() == 6u && indexesSixAll.size() >= 2u) {
      double minError = solutionDouble.AmariSum();
      rep(t, 2) {
        const auto indexes = rand_vec(indexesSixAll);
        const auto newSolDouble = TotalSolver::Solve(indexes);
        const double error = newSolDouble.AmariSum();
        if (error < minError) {
          minError = error;
          solutionDouble = newSolDouble;
          colorIndicies = indexes;
          cerr << "New solution found: " << minError << endl;
        }
      }
    }
    cerr << "SolutionDouble time: " << global_timer.ms() - ms << "ms" << endl;

    int colorIndex = 0;
    int totalPutCount = 0;
    for (const auto &poses : lanePoses) {
      const auto color = colorIndicies[colorIndex++];
      const int putCount = solutionDouble.PutCount(color);
      totalPutCount += putCount;
      color2BottleIndex[color] = (int)bottles.size();
      bottles.push_back(
          new BottleNormal(poses, color, putCount, T < 18000, T >= 32000));
      putVecSum += ownColorVecs[color] * putCount;
    }
    cerr << "Total put count: " << totalPutCount << endl;
    if (totalPutCount != 1000) {
      // 緊急避難(K == 4のときに起こることを確認)
      cerr << "緊急避難" << endl;
      Solve4::Solver solve4;
      solve4.Solve();
      exit(0);
    }
    assert(totalPutCount == 1000);

    State state(mainPoses, bottles);
    return state;
  }

  // Putできなかったらfalseを返す
  bool Step(State &state, vector<RealAction> &realActions, const int turn,
            const bool useDoubleSolAmounts) {
    if (T <= 18000 && turn > (18000 - T) / 4 + 20) {
      for (auto &bottle : state.bottles) {
        bottle->SetLargeT();
      }
    } else if (turn > (34000 - T) / 16 + 20) {
      for (auto &bottle : state.bottles) {
        bottle->SetVeryLargeT();
      }
    }

    const int halfCount =
        (turn < 500 ||
         (stepMsSum / stepCount) * (H - turn) + global_timer.ms() < 2550)
            ? 4
            : 3;

    const auto &turnAmounts = solutionDouble.GetTurnAmounts(turn);
    vector<int> usedColors = turnAmounts.UsedColors();
    map<int, double> colorAmounts;
    Vec3 targetVec;

    if (usedColors.size() == 4u && !useDoubleSolAmounts) {
      // 4色の場合は、改めて計算
      targetVec = targetColorVecs[turn];
      const auto result = OptimalColorRatio(
          targetVec, 1.0, ownColorVecs[usedColors[0]],
          ownColorVecs[usedColors[1]], ownColorVecs[usedColors[2]],
          ownColorVecs[usedColors[3]], state.mainWell.color.ToVec3(),
          state.mainWell.colorAmount);
      colorAmounts[usedColors[0]] = result.GetProp(0);
      colorAmounts[usedColors[1]] = result.GetProp(1);
      colorAmounts[usedColors[2]] = result.GetProp(2);
      colorAmounts[usedColors[3]] = result.GetProp(3);
    } else {
      colorAmounts = turnAmounts.ColorAmounts();
      // 全体最適化後のターゲットカラーを目標とする
      targetVec = turnAmounts.MergedVec();
    }

    // 各ボトルの上位アクションの組み合わせを全て試す
    vector<vector<Candidate>> bottleCandidates;
    vector<int> usedBottles;

    for (const auto &pair : colorAmounts) {
      const int color = pair.first;
      const double amount = pair.second;
      const int bottleIndex = color2BottleIndex[color];
      const Bottle *bottle = state.GetBottle(bottleIndex);

      // ボトルのアクションを取得
      auto candidates = bottle->Candidates(amount);
      bottleCandidates.push_back(candidates);
      usedBottles.push_back(bottleIndex);
    }

    // 各ボトルの上位アクションの組み合わせを試す
    vector<int> bestCandidateIndices;
    double minError = INF;

    const auto eval = [&](const vector<int> &indices, Well &currentMainWell,
                          vector<RealAction> &currentRealActions) {
      for (const auto bottleIndex : usedBottles) {
        auto bottle = state.GetBottle(bottleIndex);
        bottle->Load(); // ボトルの状態をリセット
      }

      rep(i, usedBottles.size()) {
        const auto bottleIndex = usedBottles[i];
        auto bottle = state.GetBottle(bottleIndex);

        // ボトルを閉じる
        currentMainWell = bottle->Close(
            currentMainWell, bottleCandidates[i][indices[i]].actionId,
            currentRealActions);
      }

      // ボトルを開く
      rep(i, usedBottles.size()) {
        const auto bottleIndex = usedBottles[i];
        auto bottle = state.GetBottle(bottleIndex);
        currentMainWell = bottle->Open(currentMainWell,
                                       bottleCandidates[i][indices[i]].actionId,
                                       currentRealActions);
      }

      if (!currentMainWell.CanSubmit()) {
        return (double)INF; // 提出できないので終了
      }

      // 提出する
      currentRealActions.emplace_back(
          RealAction::Submit(state.mainPoses.front(), turn));

      auto error = currentMainWell.GetError(targetVec);
      // 将来的なエラー
      const auto newSubmitVecSum =
          submitVecSum + currentMainWell.color.ToVec3();
      const auto newRemainTargetVecSum = remainTargetVecSum - targetVec;
      // 丸目誤差を懸念して、差分を足している
      error += (putVecSum.distance(newRemainTargetVecSum + newSubmitVecSum) -
                putVecSum.distance(submitVecSum + remainTargetVecSum)) *
               0.5;

      return error;
    };

    const auto func = [&](auto self, vector<int> &currentCandidateIndices,
                          const double errorSum) -> void {
      if (currentCandidateIndices.size() == bottleCandidates.size()) {
        // 評価
        if (errorSum + state.mainWell.colorAmount < 0.0) {
          return;
        }
        Well currentMainWell = state.mainWell;
        vector<RealAction> currentRealActions;
        const auto error =
            eval(currentCandidateIndices, currentMainWell, currentRealActions);
        if (error < minError) {
          minError = error;
          bestCandidateIndices = currentCandidateIndices;
        }
        return;
      }

      const int nextIndex = (int)currentCandidateIndices.size();
      int positiveErrorCount = 0;
      int negativeErrorCount = 0;
      for (int i = 0; i < (int)bottleCandidates[nextIndex].size(); ++i) {
        if (positiveErrorCount + negativeErrorCount >= halfCount * 2) {
          break;
        }
        const auto &candidate = bottleCandidates[nextIndex][i];
        if (candidate.error < 0) {
          if (negativeErrorCount == halfCount) {
            continue; // ネガティブエラーは半分まで
          }
          negativeErrorCount++;
        } else {
          if (positiveErrorCount == halfCount) {
            continue; // ポジティブエラーは半分まで
          }
          positiveErrorCount++;
        }
        currentCandidateIndices.push_back(i);
        self(self, currentCandidateIndices, errorSum + candidate.error);
        currentCandidateIndices.pop_back();
      }
    };

    vector<int> currentCandidateIndices;
    func(func, currentCandidateIndices, 0.0);

    // cerr << "Turn: " << turn << ", Min Error: " << minError << endl;

    if (minError == INF) {
      for (const auto bottleIndex : usedBottles) {
        auto bottle = state.GetBottle(bottleIndex);
        bottle->Load(); // ボトルの状態をリセット
      }
      return false;
    }

    vector<RealAction> bestRealActions;
    eval(bestCandidateIndices, state.mainWell, bestRealActions);
    realActions.insert(realActions.end(), bestRealActions.begin(),
                       bestRealActions.end());
    for (const auto &action : bestRealActions) {
      if (action.kind == RealActionKind::kPut) {
        state.totalPutCount++;
      }
    }
    submitVecSum += state.mainWell.color.ToVec3();
    remainTargetVecSum -= targetVec;
    state.mainWell.Pick(); // 提出するので、メインウェルを更新
    for (auto bottle : state.bottles) {
      bottle->UpdateMainAfterSubmit(state.mainWell);
      bottle->Save(); // 状態を保存
    }

    return true;
  }

  bool Solve() {
    State state = InitState();
    vector<RealAction> realActions;
    rep(turn, H) {
      double statrtMs = global_timer.ms();
      const auto success = Step(state, realActions, turn, false);
      stepCount++;
      stepMsSum += global_timer.ms() - statrtMs;

      if (success) {
        continue;
      }

      // 計画通りに置けなかった場合は、調整が必要
      // 線形計画で解く
      FinalSolve(state, turn, realActions);
      break;
    }

    // // 最終ターンは、全開放
    // for (int k = 0; k < (int)state.bottles.size(); ++k) {
    //   auto &bottle = state.bottles[k];
    //   state.mainWell = bottle->OpenAll(state.mainWell, realActions);
    // }
    // realActions.emplace_back(
    //     RealAction::Submit(state.mainPoses.front(), H - 1));

    Print(state.GetInitialPartition());
    Print(realActions);
    cerr << global_timer.ms() << "[ms]" << endl;

    return true;
  }

  void FinalSolve(State &state, const int turn, vector<RealAction> &realActions,
                  int failedCount = 0) {
    if (failedCount >= 5) {
      cerr << "Failed too many times, exiting." << endl;
      Print(state.GetInitialPartition());
      Print(realActions);
      exit(0);
    }
    // 残りターンの分をまとめて解く
    vector<Vec3> targets;

    // 色
    const auto colorIndexes = state.ColorIndexs();
    const auto colors = state.Colors();
    auto amounts = state.RemainAmounts();
    // 和が1.0 * 残りターン - mainWell.colorAmountになるように補正
    {
      double sum = 0.0;
      for (const auto &amount : amounts) {
        sum += amount;
      }
      for (auto &amount : amounts) {
        amount *= (H - turn - state.mainWell.colorAmount) / sum;
      }
    }

    for (int t = turn; t < H; ++t) {
      targets.push_back(targetColorVecs[t]);
    }

    const auto result =
        QPSolver::Solve(colors, amounts, targets, state.mainWell.colorAmount,
                        state.mainWell.color.ToVec3());

    for (int t = turn; t < H; ++t) {
      map<int, double> colorAmounts;
      rep(i, colors.size()) {
        const auto amount = result.first[t - turn][i];
        if (amount < 1e-8) {
          continue; // ほぼ0の量は無視
        }
        colorAmounts[colorIndexes[i]] = amount;
      }
      auto &turnAmounts = solutionDouble.GetTurnAmounts(t);
      turnAmounts.SetAmountForce(colorAmounts);
      const auto success = Step(state, realActions, t, true);
      if (success) {
        failedCount = 0; // 成功したので、失敗回数をリセット
        continue;
      }

      // 計画通りに置けなかった場合は、再度調整が必要
      FinalSolve(state, t, realActions, failedCount + 1);
      return;
    }
  }

private:
  map<int, int> color2BottleIndex; // 色 -> ボトルのインデックス
};
} // namespace Solve5


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
  Initialize(cin);


  if (T >= 11000) {
    if (D <= 200) {
      Solve4::Solver solver4;
      solver4.Solve();
    } else {
      Solve5::Solver solver5;
      solver5.Solve();
    }
  } else {
    Solve3::Solver solver3;
    solver3.Solve();
  }
  return 0;
}