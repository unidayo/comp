#pragma GCC target("arch=skylake-avx512")
#pragma GCC optimize("O3","inline")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,fma,avx512f")

// #include <atcoder/dsu>
// #include <atcoder/segtree>
// #include <atcoder/mincostflow>

#include <immintrin.h>

#define FINAL_SUBMIT 1

#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <climits>
#include <complex>
#include <cstdio>
#include <cstring>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define REP(i, n) for (int i = 0; i < (int)(n); i++)
#define ALL(x) (x).begin(), (x).end()

#if FINAL_SUBMIT
#define HHH(x)
#define LOG(x)
#define FORCE_LOG(x) std::cerr << #x << " = " << x << "\n"
#define GRAPH(x, y)
#define CERR() if (false) std::cerr
#define NDEBUG
#else
#define HHH(x) std::cerr << "L" << __LINE__ << ": " << #x << " : " << (x) << "\n"
#define LOG(x) std::cerr << #x << " = " << x << "\n"
#define FORCE_LOG(x) std::cerr << #x << " = " << x << "\n"
#define GRAPH(x, y) std::cerr << "graph " << x << " " << y << "\n"
#define CERR() std::cerr
#endif

template <typename T> T &chmin(T &a, const T &b) { return a = std::min(a, b); }
template <typename T> T &chmax(T &a, const T &b) { return a = std::max(a, b); }

using ll = long long;
using ld = double;
constexpr int INF = 1e9;
constexpr double PI = acos(-1.0);

template <typename T> T sq(T x) { return x * x; }

template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& p) {
  return os << "(" << p.first << ", " << p.second << ")";
}
template <typename T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
  os << "[";
  for (const T& e : vec) os << " " << e;
  os << " ]";
  return os;
}

class Timer {
public:
  static constexpr int64_t CYCLES_PER_SEC = 2900000000;
  int64_t start, stop;
  inline int64_t get_cycle() const {
    uint32_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return int64_t(low) | (int64_t(high) << 32);
  }
  Timer(double time_limit) : start(get_cycle()), stop(start + CYCLES_PER_SEC * time_limit) {}
  inline void reset_stop(double time_limit) { stop = start + CYCLES_PER_SEC *  time_limit; }
  inline bool within(int64_t end) const { return get_cycle() < end; }
  inline bool within() const { return within(stop); }
  inline double elapsed() const { return (get_cycle() - start) / double(CYCLES_PER_SEC); }
};

class XorShift {
  unsigned int x, y, z, w;
public:
  XorShift() : x(314159265), y(358979323), z(846264338), w(327950288) {}
  inline unsigned int next() {
    unsigned int t = x ^ x << 11;
    x = y; y = z; z = w;
    return w = w ^ w >> 19 ^ t ^ t >> 8;
  }
  inline int next(int n) { return next() % n; }
  inline int next(size_t n) { return next() % n; }
  inline int next(int lo, int hi) { return lo + next(hi - lo + 1); }
  inline double next(double x) { return double(next()) / (1LL << 32) * x; }
  inline double next(double lo, double hi) { return lo + next(hi - lo); }
};

double get_param(const char* env_var) {
  char* ret = getenv(env_var);
  assert (ret != nullptr);
  return stod(std::string(ret));
}

// double a_param = get_param("A_PARAM");
// double b_param = get_param("B_PARAM");
// double c_param = get_param("C_PARAM");
// int a_param = get_param("A_PARAM");
// int j_param = get_param("J_PARAM");

constexpr double TIME_LIMIT = 2.0;

Timer timer(TIME_LIMIT * 0.99);
XorShift rnd;

using namespace std;

int N, Si, Sj, Ti, Tj;
constexpr int C = 42;
bool A[C][C];
int S, T;

alignas(64) static uint32_t GDIST_S[C * C];
static uint16_t GDIST_EPOCH = 1;

inline int DistFromSIdx(int v) {
  uint32_t x = GDIST_S[v];
  return (uint16_t)(x >> 16) == GDIST_EPOCH ? int(x & 0xFFFFu) : 0;
}

void SetUp() {
  cin >> N >> Ti >> Tj;
  Si = 0; Sj = N / 2;
  REP(i,N) {
    string line;
    cin >> line;
    REP(j,N) A[i][j] = (line[j] == 'T');
  }
  LOG(N);
  LOG(Ti);
  LOG(Tj);
  S = (Si + 1) * C + (Sj + 1);
  T = (Ti + 1) * C + (Tj + 1);
}

struct Board {
  int sol_type = 0;
  int n_plot = -1;
  vector<int> path;
  alignas(64) int8_t B[C * C]; // size C*C

  void Init() {
    std::fill(B, B + C * C, 1);
    REP(i,N) REP(j,N) B[(i+1)*C+j+1] = A[i][j] ? 1 : 0;
  }

  vector<int> ShortestPath(int s, int t) const {
    alignas(64) static uint16_t seen[C * C];
    alignas(64) static uint16_t dist[C * C];
    alignas(64) static int      prevv[C * C];
    alignas(64) static int      q[C * C];
    static uint16_t stamp = 1;
    if (++stamp == 0) {
      memset(seen, 0, sizeof(seen));
      stamp = 1;
    }

    static constexpr int dx[4] = {1, -1, C, -C};
    static constexpr uint8_t PERM[12][4] = {
      {0,1,2,3},{0,2,1,3},{0,3,1,2},{1,0,2,3},{1,2,0,3},{1,3,0,2},
      {2,0,1,3},{2,1,0,3},{2,3,0,1},{3,0,1,2},{3,1,0,2},{3,2,0,1}
    };
    const uint8_t* ord = PERM[rnd.next() % 12];

    int qh = 0, qt = 0;
    q[qt++] = t;
    seen[t] = stamp;
    dist[t] = 0;
    prevv[t] = -1;

    while (qh < qt) {
      const int v  = q[qh++];
      if (v == s) break;
      const uint16_t nd = dist[v] + 1;

      int u = v + dx[ord[0]];
      if (!B[u] && seen[u] != stamp) {
        seen[u] = stamp; dist[u] = nd; prevv[u] = v; q[qt++] = u;
      }
      u = v + dx[ord[1]];
      if (!B[u] && seen[u] != stamp) {
        seen[u] = stamp; dist[u] = nd; prevv[u] = v; q[qt++] = u;
      }
      u = v + dx[ord[2]];
      if (!B[u] && seen[u] != stamp) {
        seen[u] = stamp; dist[u] = nd; prevv[u] = v; q[qt++] = u;
      }
      u = v + dx[ord[3]];
      if (!B[u] && seen[u] != stamp) {
        seen[u] = stamp; dist[u] = nd; prevv[u] = v; q[qt++] = u;
      }
    }
    if (seen[s] != stamp) return {}; // Unreachable

    vector<int> path;
    path.reserve(dist[s] + 1);
    for (int v = s; v != t; v = prevv[v]) path.push_back(v);
    return path;
  }

  bool TryCreateCenterRoadLegacy() {
    Init();
    B[T] = B[T - 1] = B[T + 1] = B[T - C] = B[T + C] = 1;

    int center = S;
    int pat = rnd.next(15);
    path.clear();
    if (pat < 5) {
      if (pat < 1) center += 1;
      else if (pat < 2) center -= 1;
      else center += C;
      path.push_back(S);
    } else if (pat < 7) {
      int dy = rnd.next(0, 4);
      int dx = rnd.next(-3, 4);
      int d = dy + abs(dx);
      if (d <= 1 || d >= 5) return false;
      center = S + dy * C + dx;
      if (B[center] > 0) return false;
      path = ShortestPath(S, center);
      for (int v : path) {
        B[v] = B[v - 1] = B[v + 1] = B[v - C] = B[v + C] = 1;
      }
      B[center] = 0;
    }

    // TODO: Find a better center
    int xl = (N + 1) / 2 - rnd.next(10);
    // int xr = xl + rnd.next(2, 4);
    int xr = (N + 3) / 2 + rnd.next(10);
    int l1 = (N - rnd.next(2)) * C + xl;
    int r1 = (N - rnd.next(2)) * C + xr;
    int l2 = rnd.next(4, N - 6) * C + ((N - 3) / 2 - rnd.next(10));
    // int l2 = (N - rnd.next(2)) * C + rnd.next(2);
    int r2 = rnd.next(4, N - 6) * C + ((N + 7) / 2 + rnd.next(10));
    int l3 = rnd.next(4, N - 6) * C + ((N - 7) / 2 - rnd.next(10));
    // int l3 = rnd.next(4, N - 6) * C + rnd.next(2);
    int r3 = rnd.next(4, N - 6) * C + ((N + 11) / 2 + rnd.next(10));
    if (B[l1] > 0) return false;
    if (B[r1] > 0) return false;
    if (B[l2] > 0) return false;
    if (B[r2] > 0) return false;
    if (r1 - l1 == 1) return false;
    if (rnd.next(2) == 0) {
      std::swap(l1, r1);
      std::swap(l2, r2);
      std::swap(l3, r3);
    }

    // Left
    vector<int> path1 = ShortestPath(center, l1);
    if (path1.empty()) return false;
    for (int v : path1) {
      if (v == center || v == l1) continue;
      B[v] = B[v - 1] = B[v + 1] = B[v - C] = B[v + C] = 1;
    }

    B[l1] = 0;
    if (B[l2] > 0) return false;
    vector<int> path1_2 = ShortestPath(l1, l2);
    if (path1_2.empty()) return false;
    for (int v : path1_2) {
      if (v == l1 || v == l2) continue;
      B[v] = B[v - 1] = B[v + 1] = B[v - C] = B[v + C] = 1;
    }
    path1.insert(path1.end(), path1_2.begin(), path1_2.end());

    do {
      B[l2] = 0;
      if (B[l3] > 0) break;
      vector<int> path1_3 = ShortestPath(l2, l3);
      if (path1_3.empty()) break;
      for (int v : path1_3) {
        if (v == l2 || v == l3) continue;
        B[v] = B[v - 1] = B[v + 1] = B[v - C] = B[v + C] = 1;
      }
      path1.insert(path1.end(), path1_3.begin(), path1_3.end());
    } while (false);

    // Right
    B[center] = 0;
    vector<int> path2 = ShortestPath(center, r1);
    if (path2.empty()) return false;
    for (int v : path2) {
      if (v == center || v == r1) continue;
      B[v] = B[v - 1] = B[v + 1] = B[v - C] = B[v + C] = 1;
    }

    B[r1] = 0;
    if (B[r2] > 0) return false;
    vector<int> path2_2 = ShortestPath(r1, r2);
    if (path2_2.empty()) return false;
    for (int v : path2_2) {
      if (v == r1 || v == r2) continue;
      B[v] = B[v - 1] = B[v + 1] = B[v - C] = B[v + C] = 1;
    }
    path2.insert(path2.end(), path2_2.begin(), path2_2.end());

    do {
      B[r2] = 0;
      if (B[r3] > 0) break;
      vector<int> path2_3 = ShortestPath(r2, r3);
      if (path2_3.empty()) break;
      for (int v : path2_3) {
        if (v == r2 || v == r3) continue;
        B[v] = B[v - 1] = B[v + 1] = B[v - C] = B[v + C] = 1;
      }
      path2.insert(path2.end(), path2_3.begin(), path2_3.end());
    } while (false);

    std::reverse(path1.begin(), path1.end());
    path1.pop_back();
    path.insert(path.begin(), path1.begin(), path1.end());
    path.insert(path.end(), path2.begin(), path2.end());
    sol_type = 1;
    return true;
  }

  static inline bool FastDFS(const int dx[4][4], int start_v, int goal_x,
                             int8_t* B, std::vector<int>& path_buf) {
    static int            v_st[C * C];
    static unsigned char  ok_st[C * C];
    static unsigned char  dirrow_st[C * C];
    static unsigned char  child_st[C * C];
    static unsigned char  entered_st[C * C];

    int sp = 0;
    v_st[0]       = start_v;
    ok_st[0]      = 0;
    dirrow_st[0]  = rnd.next(4);
    child_st[0]   = 0;
    entered_st[0] = 0;
    sp = 1;

    path_buf.clear();

    while (sp) {
      int idx = sp - 1;
      int v = v_st[idx];
      unsigned char &ok      = ok_st[idx];
      unsigned char &dirrow  = dirrow_st[idx];
      unsigned char &child   = child_st[idx];
      unsigned char &entered = entered_st[idx];

      if (!entered) {
        if (ok >= 3 && (v % C) - 1 == goal_x) return true;
        path_buf.push_back(v);
        B[v]++; B[v - 1]++; B[v + 1]++; B[v - C]++; B[v + C]++;
        entered = 1;
        ok += (unsigned char)((v / C) - 1 >= N - 2);
      }

      bool advanced = false;
      for (; child < 4; ++child) {
        int u = v + dx[dirrow][child];
        if (B[u] > 1) continue;
        v_st[sp]       = u;
        ok_st[sp]      = ok;
        dirrow_st[sp]  = rnd.next(4);
        child_st[sp]   = 0;
        entered_st[sp] = 0;
        ++sp;
        advanced = true;
        break;
      }
      if (advanced) continue;

      // B[v]--;
      // B[v - 1]--; B[v + 1]--;
      // B[v - C]--; B[v + C]--;
      path_buf.pop_back();
      --sp;
    }
    return false;
  }

  inline bool TryCreateCenterRoad() {
    // if ( <= T % C - 1 && T % C - 1 <= N - N / 4 + 1) {
    //   return TryCreateCenterRoadLegacy();
    // }
    // if (rnd.next() % 100 < 50) return TryCreateCenterRoadLegacy();

    // TODO: Revisit center
    int center_y = rnd.next(1, 4);
    int center_x = S % C - 5 + rnd.next(9);
    int offset_x = rnd.next(6);
    int goal_x = -1;
    static vector<int> path_buf;
    path_buf.clear();

    // auto dfs = [&](auto&& self, const int dx[4][4], int v, int ok = 0) -> bool {
    //   if (ok >= 3 && v % C - 1 == goal_x) return true;
    //   path_buf.push_back(v);
    //   B[v]++; B[v - 1]++; B[v + 1]++; B[v - C]++; B[v + C]++;
    //   ok += v / C - 1 >= N - 2;
    //   // ok += v / C - 1 >= N - 1;
    //   int dir_type = rnd.next(4);
    //   REP(dir, 4) {
    //     int u = v + dx[dir_type][dir];
    //     if (B[u] > 1) continue;
    //     if (self(self, dx, u, ok)) return true;
    //   }
    //   // B[v]--; B[v - 1]--; B[v + 1]--; B[v - C]--; B[v + C]--;
    //   path_buf.pop_back();
    //   return false;
    // };

    // auto dfs2 = [&](auto&& self, const int dx[4][4], int v, int ok = 0) -> bool {
    //   if (ok == 0 && v / C - 1 == N - 1) ok = 1;
    //   if (ok == 1 && v / C - 1 == 1) ok = 2;
    //   if (ok == 2 && v % C - 1 == goal_x) return true;
    //   path_buf.push_back(v);
    //   B[v]++; B[v - 1]++; B[v + 1]++; B[v - C]++; B[v + C]++;
    //   int dir_type = rnd.next(4);
    //   REP(dir, 4) {
    //     int u = v + dx[dir_type][dir];
    //     if (B[u] > 1) continue;
    //     if (self(self, dx, u, ok)) return true;
    //   }
    //   // B[v - 1]--; B[v + 1]--; B[v - C]--; B[v + C]--;
    //   path_buf.pop_back();
    //   return false;
    // };

    auto do_left = [&](bool after) {
      Init();
      B[T] = B[T - 1] = B[T + 1] = B[T - C] = B[T + C] = 1;
      goal_x = int(N * rnd.next(0.18, 0.25));

      if (after) {
        for (int v : path) {
          auto embed = [&](int v) { if (v / C - 1 >= center_y) B[v] = 1; };
          embed(v); embed(v - 1); embed(v + 1); embed(v - C); embed(v + C);
        }
      }
      REP(i,N) REP(j,N) {
        int v = (i + 1) * C + (j + 1);
        if (j > max(center_x, S % C - 1)) B[v] = 1;
        if (!after && j >= center_x && i >= center_y) {
          if ((i + offset_x) % 6 < 3) continue;
          B[v] = 1;
        }
        if (j < goal_x) B[v] = 1;
      }
      static constexpr int DX_LEFT[4][4] = {
        {-C,  1, -1,  C},
        {-C,  1,  C, -1},
        { 1, -C, -1,  C},
        { 1, -C,  C, -1},
      };
      if (!FastDFS(DX_LEFT, S, goal_x, B, path_buf)) return false;
      return true;
    };

    auto do_right = [&](bool after) {
      Init();
      B[T] = B[T - 1] = B[T + 1] = B[T - C] = B[T + C] = 1;
      goal_x = int(N * rnd.next(0.75, 0.82));

      if (after) {
        for (int v : path) {
          auto embed = [&](int v) { if (v / C - 1 >= center_y) B[v] = 1; };
          embed(v); embed(v - 1); embed(v + 1); embed(v - C); embed(v + C);
        }
      }
      REP(i,N) REP(j,N) {
        int v = (i + 1) * C + (j + 1);
        if (j < min(center_x, S % C - 1)) B[v] = 1;
        if (!after && j <= center_x && i >= center_y) {
          if ((i + offset_x) % 6 < 3) continue;
          B[v] = 1;
        }
        if (j > goal_x) B[v] = 1;
      }
      static constexpr int DX_RIGHT[4][4] = {
        {-C, -1,  1,  C},
        {-C, -1,  C,  1},
        {-1, -C,  1,  C},
        {-1, -C,  C,  1},
      };
      if (!FastDFS(DX_RIGHT, S, goal_x, B, path_buf)) return false;
      return true;
    };

    bool left_first = rnd.next(2);
    if (left_first) {
      if (!do_left(false)) return false;
    } else {
      if (!do_right(false)) return false;
    }
    path = path_buf;
    reverse(path.begin(), path.end());
    path.pop_back();
    path_buf.clear();
    if (left_first) {
      if (!do_right(true)) return false;
    } else {
      if (!do_left(true)) return false;
    }
    path.insert(path.end(), path_buf.begin(), path_buf.end());

    if (!TryCreateCenterWall()) return false;
    // Dump();

    sol_type = 2;
    return true;
  }

  bool TryCreateCenterWall() {
    int n = path.size();
    Init();
    for (int i = 1; i + 1 < n; i++) {
      int v = path[i];
      B[v] = B[v - 1] = B[v + 1] = B[v - C] = B[v + C] = 1;
    }
    for (int i = 0; i < n; i++) {
      int v = path[i];
      B[v] = 0;
    }
    return true;
  }

  inline int ReachSize() const {
    if (++GDIST_EPOCH == 0) {
      memset(GDIST_S, 0, sizeof(GDIST_S));
      GDIST_EPOCH = 1;
    }
    const uint32_t CUR = uint32_t(GDIST_EPOCH) << 16;

    alignas(64) static int q[C * C];
    int qh = 0, qt = 0;

    q[qt++] = S;
    if (B[S]) return 0;
    GDIST_S[S] = CUR | 0u;

    int cnt = 1;
    int reach_T = 0;

    while (qh < qt) {
      const int v = q[qh++];
      if (v == T) reach_T = 1;

      const uint16_t nd = (uint16_t)((GDIST_S[v] & 0xFFFFu) + 1u);

      int u = v + 1;
      if (!B[u] && (GDIST_S[u] >> 16) != GDIST_EPOCH) {
        GDIST_S[u] = CUR | nd; q[qt++] = u; ++cnt;
      }
      u = v - 1;
      if (!B[u] && (GDIST_S[u] >> 16) != GDIST_EPOCH) {
        GDIST_S[u] = CUR | nd; q[qt++] = u; ++cnt;
      }
      u = v + C;
      if (!B[u] && (GDIST_S[u] >> 16) != GDIST_EPOCH) {
        GDIST_S[u] = CUR | nd; q[qt++] = u; ++cnt;
      }
      u = v - C;
      if (!B[u] && (GDIST_S[u] >> 16) != GDIST_EPOCH) {
        GDIST_S[u] = CUR | nd; q[qt++] = u; ++cnt;
      }
    }
    return reach_T ? cnt : 0;
  }

  double Eval1() const {
    int cnt = ReachSize();
    if (cnt == 0) return 0;
    int path_size = path.empty() ? 0 : ShortestPath(path.front(), path.back()).size();
    // int path_size = path.size();
    // return (cnt - path_size * 0.8) * pow(path_size, 0.8);
    return (cnt - path_size * 0.8) * pow(path_size, 0.9);
    // return path_size;
  }

  int Eval2() const {
    int cnt = ReachSize();
    if (cnt == 0) return 0;
    int path_size = path.empty() ? 0 : ShortestPath(path.front(), path.back()).size();
    // return (n_plot + cnt * 0.01) * pow(path_size, 1.0);
    return path_size;
  }

  double CreateCenterSnake() {
    constexpr int n_try = 200;
    int i_try = 0;
    for (i_try = 0; i_try < n_try; i_try++) {
      if (!TryCreateCenterRoad()) {
        if (i_try < 1) continue;
        if (!TryCreateCenterRoadLegacy()) continue;
      }
      if (!TryCreateCenterWall()) continue;
      double res = Eval1();
      // if (res <= 0) {
      //   if (B[T - C] + B[T - 1] + B[T + 1] + B[T + C] == 4) {
      //     if (Ti >= 1 && A[Ti-1][Tj] == 0 &&
      //         B[T - 2 * C] == 1 && B[T - C - 1] + B[T - C + 1] == 1) {
      //       B[T - C] = 0;
      //       res = Eval1();
      //     } else if (Tj >= 1 && A[Ti][Tj-1] == 0 &&
      //                B[T - 2] == 1 && B[T - 1 - C] + B[T - 1 + C] == 1) {
      //       B[T - 1] = 0;
      //       res = Eval1();
      //     } else if (Ti <= N - 2 && A[Ti+1][Tj] == 0 &&
      //                B[T + 2 * C] == 1 && B[T + C - 1] + B[T + C + 1] == 1) {
      //       B[T + C] = 0;
      //       res = Eval1();
      //     } else if (Tj <= N - 2 && A[Ti][Tj+1] == 0 &&
      //                B[T + 2] == 1 && B[T + 1 - C] + B[T + 1 + C] == 1) {
      //       B[T + 1] = 0;
      //       res = Eval1();
      //     }
      //   }
      // }
      if (res > 0) {
        // LOG(i_try);
        return res;
      }
    }
    // LOG(i_try);
    return 0;
  }

  inline void swap_with(int index, int8_t& value) {
    // if (0 <= index && index < C * C) 
    std::swap(B[index], value);
  }

  // void SidePlot() {
  //   if (sol_type != 2) return;
  //   sol_type = 3;
  //   int x = T % C - 1;
  //   if (x >= 4) {
  //     int offset = rnd.next(5);
  //     REP(i,N) {
  //       int row = (i + offset) % 5;
  //       REP(j,2) {
  //         int v = (i + 1) * C + (j + 1);
  //         if (row == 0 || row == 1 || (row == 3 && j == 0)) {
  //           B[v] = 1;
  //         }
  //       }
  //     }
  //   }
  //   if (x < N - 5) {
  //     int offset = rnd.next(5);
  //     REP(i,N) {
  //       int row = (i + offset) % 5;
  //       REP(j,2) {
  //         int v = (i + 1) * C + (N - j);
  //         if (row == 0 || row == 1 || (row == 3 && j == 0)) {
  //           B[v] = 1;
  //         }
  //       }
  //     }
  //   }
  // }

  bool AroundPlot() {
    static mt19937 rng;
    static vector<vector<int>> plots = {
      {T + 2, T - 1, T - C, T + C, T + 1 + C},
      {T + 2, T - 1, T - C, T + C, T + 1 - C},
      {T - 2, T + 1, T - C, T + C, T - 1 + C},
      {T - 2, T + 1, T - C, T + C, T - 1 - C},
      {T + 2 * C, T - C, T - 1, T + 1, T + C - 1},
      {T + 2 * C, T - C, T - 1, T + 1, T + C + 1},
      {T - 2 * C, T + C, T - 1, T + 1, T - C - 1},
      {T - 2 * C, T + C, T - 1, T + 1, T - C + 1},
    };
    shuffle(ALL(plots), rng);
    int8_t oldB[5] = {1, 1, 1, 1, 1};
    // auto bestB = B;
    int best_score = 0;
    int best_index = -1;
    REP(index, 8) {
      const vector<int>& plot = plots[index];
      if (plot[0] < 0 || plot[0] >= C * C) continue;
      REP(i,5) swap_with(plot[i], oldB[i]);
      int score = ShortestPath(S, T).size();
      if (score > best_score) {
        best_score = score;
        best_index = index;
      }
      REP(i,5) swap_with(plot[i], oldB[i]);
    }
    // B = bestB;
    if (best_index >= 0) {
      REP(i,5) swap_with(plots[best_index][i], oldB[i]);
    }
    return best_score > 0;
  }

  inline bool maybe_split(int index) const {
    static constexpr int dx[8] = {1, C + 1, C, C - 1, -1, -C - 1, -C, -C + 1};
    if (0 <= index && index < C * C && B[index] == 0) {
      int count = 0;
      REP(i,8) {
        int u1 = index + dx[i];
        int u2 = index + dx[(i + 1) & 7];
        count += (B[u1] ^ B[u2]);
      }
      return count >= 4;
    }
    return false;
  }

  int PostPlot() {
    static mt19937 rng;
    static vector<int> order;
    static vector<int8_t> ploted(C * C, 0);
    std::fill(ALL(ploted), 0);
    for (int v : path) ploted[v] = 1;
    ploted[S] = ploted[T] = 1;
    if (order.empty()) {
      REP(i,N) REP(j,N) order.push_back((i + 1) * C + (j + 1));
    }
    vector<vector<int>> dxs = {
      {2, 1 + C, C, -1, -C, 1, -C + 1},
      {2, 1 - C, -C, -1, C, 1, C + 1},
      {-2, -1 + C, C, 1, -C, -1, -C - 1},
      {-2, -1 - C, -C, 1, C, -1, C - 1},
      {2 * C, C + 1, 1, -C, -1, C, C - 1},
      {2 * C, C - 1, -1, -C, 1, C, C + 1},
      {-2 * C, -C + 1, 1, C, -1, -C, -C - 1},
      {-2 * C, -C - 1, -1, C, 1, -C, -C + 1},
    };
    n_plot = 0;

    int reach_size = ReachSize();
    REP(loop,2) {
      shuffle(ALL(order), rng);
      shuffle(ALL(dxs), rng);

      for (int v : order) {
        if (v == S || v == T || B[v] || ploted[v]) continue;
        vector<int8_t> oldB = {1, 1, 1, 1, 1};
        int best_score = 0;
        int best_index = -1;
        REP(index,dxs.size()) {
          bool is_maybe_split = false;
          const vector<int>& dx = dxs[index];
          if (v + dx[0] < 0 || v + dx[0] >= C * C ||
              B[v + dx[5]] || B[v + dx[6]]) continue;
          bool has_plotted = false;
          REP(i,7) {
            int vi = v + dx[i];
            if (ploted[vi]) has_plotted = true;
          }
          if (has_plotted) continue;

          int count_zero = 0;
          REP(i,5) {
            int vi = v + dx[i];
            is_maybe_split |= maybe_split(vi);
            swap_with(vi, oldB[i]);
            if (oldB[i] == 0) ++count_zero;
          }
          if (/* has_plotted || count_zero < 0 || */ count_zero > 2 + loop) {
            REP(i,5) swap_with(v + dx[i], oldB[i]);
            continue;
          }
          if (is_maybe_split) {
            int reach_size_new = ReachSize();
            if (reach_size_new + count_zero < reach_size) {
              REP(i,5) swap_with(v + dx[i], oldB[i]);
              continue;
            }
          }
          int score = DistFromSIdx(v);
          if (score > best_score) {
            best_score = score;
            best_index = index;
          }
          REP(i,5) swap_with(v + dx[i], oldB[i]);
        }
        if (best_index != -1) {
          ploted[v] = 1;
          n_plot++;
          const vector<int>& dx = dxs[best_index];
          REP(i,5) swap_with(v + dx[i], oldB[i]);
          reach_size = ReachSize();
        }
      }
    }
    REP(loop,2) {
      shuffle(ALL(order), rng);
      for (int v : order) {
        if (v == S || v == T || B[v] == 0 || A[v / C - 1][v % C - 1] == 1) continue;
        int suma = 0, sumb = 0;
        for (int d : {1, -1, C, -C}) {
          suma += B[v + d] > 0;
          if (B[v + d] == 0 && DistFromSIdx(v + d) == 0) sumb += 1;
        }
        if (suma == 2 && sumb == 1) {
          B[v] = 0;
          ReachSize();
        }
      }
    }
    return n_plot;
  }

  void PostPlotLegacy() {
    constexpr int dx[8] = {C + 1, C, C - 1, -1, -C - 1, -C, -C + 1, 1};
    auto count_around = [&](int v) {
      int cnt = 0;
      REP(d,8) {
        int u = v + dx[d];
        if (u / C == 0 || u / C == N + 1) continue;
        if (u % C == 0 || u % C == N + 1) continue;
        if (B[u] == 1) cnt++;
      }
      return cnt;
    };
    static mt19937 rng;
    static vector<pair<int, int>> order;
    if (order.empty()) { REP(i,N) REP(j,N) order.emplace_back(i, j); }
    shuffle(ALL(order), rng);
    for (auto [y, x] : order) {
      // if (loop < 50000 && (y + x) % 2) continue;
      int v = (y + 1) * C + (x + 1);
      if (v == S || v == T || B[v]) continue;
      if (count_around(v) == 0) B[v] = 1;
    }
    shuffle(ALL(order), rng);
    for (auto [y, x] : order) {
      int v = (y + 1) * C + (x + 1);
      // if (loop < 50000 && (y + x) % 2) continue;
      if (v == S || v == T || B[v] == 1) continue;
      int cntd = 0, cnt = 0;
      REP(d,8) {
        int u1 = v + dx[d], u2 = v + dx[(d + 1) % 8];
        cnt += B[u1];
        if (B[u1] != B[u2]) cntd++;
      }
      if (cntd <= 2 && cnt <= 4) B[v] = 1;  // TODO: Param
    }
    // REP(i,N) REP(j,N) {
    //   int v = (i + 1) * C + (j + 1);
    //   if (A[i][j] == 1 || B[v] == 0) continue;
    //   if (B[v - C] + B[v + C] + B[v - 1] + B[v + 1] >= 3) B[v] = 0;
    // }
  }

  void Dump() const {
    vector<string> grid(N + 2, string(N + 2, '#'));
    REP(i,N) REP(j,N) {
      if (A[i][j]) continue;
      if (B[(i+1)*C+j+1]) {
        grid[i+1][j+1] = '%';
      } else {
        grid[i+1][j+1] = '.';
      }
    }
    grid[S / C][S % C] = 'S';
    grid[T / C][T % C] = 'T';
    REP(i,N+2) CERR() << grid[i] << "\n";
  }
};

enum CellT {
  kEmpty,
  kBlock,
  kUnkwon,
};

struct Runner {
  vector<int> first_turn;
  void Init(const int8_t* B) {
    REP(i,N) REP(j,N) {
      int v = (i + 1) * C + (j + 1);
      assert (!(A[i][j] == 1 && B[v] == 0));
      if (A[i][j] == 0 && B[v] == 1) {
        first_turn.push_back(v);
      }
    }
  }
  void Run() {
    cout << first_turn.size();
    for (int v : first_turn) {
      cout << " " << (v / C - 1) << " " << (v % C - 1);
    }
    cout << endl;
    cout << -1 << endl;
    return;

    constexpr int MAX_TURNS = 100000;
    REP(loop, MAX_TURNS) {
      int pi, pj, n;
      cin >> pi >> pj >> n;
      REP(i,n) {
        int x, y;
        cin >> x >> y;
      }
      if (pi == Ti && pj == Tj) return;

      if (loop == 0) {
        cout << first_turn.size();
        for (int v : first_turn) {
          cout << " " << (v / C - 1) << " " << (v % C - 1);
        }
        cout << endl;
      } else {
        cout << 0 << endl;
      }
    }
  }
};

struct Simulator {
  alignas(64) int8_t B[C * C];
  alignas(64) uint8_t R[C * C];
  alignas(64) uint8_t BR[C * C];
  alignas(64) uint16_t dist[C * C];
  alignas(64) uint16_t dist_t[C * C];
  alignas(64) uint8_t visited[C * C];
  alignas(64) uint8_t revealed[C * C];
  alignas(64) uint8_t vert[C * C];
  alignas(64) uint8_t horiz[C * C];
  alignas(64) int que[C * C];
  bool dist_valid = false;

  inline void RevealFrom(int v) {
    if (revealed[v]) return;
    bool no = true;
    revealed[v] = true;
    if (!vert[v]) {
      // Up
      int u = v;
      do {
        vert[u] = 1;
        R[u] = 1;
        u -= C;
      } while (B[u] == 0);
      if (no) {
        no &= dist[v - C] >= dist[v];
        dist_valid &= no || R[u] || dist[u] + (v - u) / C > dist[v];
      }
      R[u] = 1;
      BR[u] = 1;

      // Down
      u = v;
      do {
        vert[u] = 1;
        R[u] = 1;
        u += C;
      } while (B[u] == 0);
      if (no) {
        no &= dist[v + C] >= dist[v];
        dist_valid &= no || R[u] || dist[u] + (u - v) / C > dist[v];
      }
      R[u] = 1;
      BR[u] = 1;
    }
    if (!horiz[v]) {
      // Left
      int u = v;
      do {
        horiz[u] = 1;
        R[u] = 1;
        u -= 1;
      } while (B[u] == 0);
      if (no) {
        no &= dist[v - 1] >= dist[v];
        dist_valid &= no || R[u] || dist[u] + (v - u) > dist[v];
      }
      R[u] = 1;
      BR[u] = 1;

      // Right
      u = v;
      do {
        horiz[u] = 1;
        R[u] = 1;
        u += 1;
      } while (B[u] == 0);
      if (no) {
        no &= dist[v + 1] >= dist[v];
        dist_valid &= no || R[u] || dist[u] + (u - v) > dist[v];
      }
      R[u] = 1;
      BR[u] = 1;
    }
  }

  int NextStep(int s, int t) {
    if (!dist_valid) {
      dist_valid = true;
      std::fill(dist, dist + C * C, 30000);
      dist[t] = 0;

      int qh = 0, qt = 0;
      que[qt++] = t;
      while (qh < qt) {
        int v = que[qh++];
        if (v == s) break;
        int d = dist[v] + 1;
        int u = v - C; // Up
        if (dist[u] > d && BR[u] == 0) { dist[u] = d; que[qt++] = u; }
        u = v + C; // Down
        if (dist[u] > d && BR[u] == 0) { dist[u] = d; que[qt++] = u; }
        u = v - 1; // Left
        if (dist[u] > d && BR[u] == 0) { dist[u] = d; que[qt++] = u; }
        u = v + 1; // Right
        if (dist[u] > d && BR[u] == 0) { dist[u] = d; que[qt++] = u; }
      }
    }
    if (dist[s - C] < dist[s]) return s - C; // Up
    if (dist[s + C] < dist[s]) return s + C; // Down
    if (dist[s - 1] < dist[s]) return s - 1; // Left
    if (dist[s + 1] < dist[s]) return s + 1; // Right
    return -1;
  }

  Simulator(int8_t* B_) {
    std::copy(B_, B_ + C * C, B);
    std::fill(R, R + C * C, 1);
    std::fill(BR, BR + C * C, 1);

    std::fill(dist_t, dist_t + C * C, 30000);
    dist_t[T] = 0;

    int qh = 0, qt = 0;
    que[qt++] = T;
    while (qh < qt) {
      int v = que[qh++];
      int d = dist_t[v] + 1;
      int u = v - C; // Up
      if (dist_t[u] > d && B[u] == 0) { dist_t[u] = d; que[qt++] = u; }
      u = v + C; // Down
      if (dist_t[u] > d && B[u] == 0) { dist_t[u] = d; que[qt++] = u; }
      u = v - 1; // Left
      if (dist_t[u] > d && B[u] == 0) { dist_t[u] = d; que[qt++] = u; }
      u = v + 1; // Right
      if (dist_t[u] > d && B[u] == 0) { dist_t[u] = d; que[qt++] = u; }
    }
  }

  inline int ReachCount() {
    memset(visited, 0, sizeof(visited));
    visited[S] = 1;

    int qh = 0, qt = 0;
    que[qt++] = S;
    int count = 0;
    while (qh < qt) {
      int v = que[qh++];
      int u = v - C; // Up
      if (!visited[u] && BR[u] == 0)
        { visited[u] = 1; que[qt++] = u; count += (R[u] == 0); }
      u = v + C; // Down
      if (!visited[u] && BR[u] == 0)
        { visited[u] = 1; que[qt++] = u; count += (R[u] == 0); }
      u = v - 1; // Left
      if (!visited[u] && BR[u] == 0)
        { visited[u] = 1; que[qt++] = u; count += (R[u] == 0); }
      u = v + 1; // Right
      if (!visited[u] && BR[u] == 0)
        { visited[u] = 1; que[qt++] = u; count += (R[u] == 0); }
    }
    return count;
  }

  double Run(int seed) {
    REP(i,N) REP(j,N) {
      int v = (i+1)*C+j+1;
      R[v] = 0;
      BR[v] = 0;
    }
    memset(revealed, 0, sizeof(revealed));
    memset(vert, 0, sizeof(vert));
    memset(horiz, 0, sizeof(horiz));

    mt19937 rng(seed);
    vector<int> order;
    order.reserve(N * N);
    REP(i, N) REP(j, N) {
      int v = (i + 1) * C + (j + 1);
      if (v == S || v == T) continue;
      order.emplace_back(v);
    }
    shuffle(ALL(order), rng);

    int pos = S;
    int steps = 0;
    double ratio = 1.0;
    double res = 0;
    int size = order.size();

    RevealFrom(S);

    REP(i,size) {
      int v = order[i];
      dist_valid = false;
      if (R[v]) continue;
      if (i > size * 0.85) {
        double r = 1.0 / ReachCount();
        // assert(dist_t[pos] != 30000);
        res += ratio * r * (steps + dist_t[pos]);
        ratio *= 1 - r;
      }
      while (R[v] == 0) {
        if (!timer.within()) return 0;
        // assert (B[pos] == 0);
        // assert (R[v] == 0);
        int next_pos = NextStep(pos, v);
        if (next_pos == -1) break; // Unreachable
        // assert (B[next_pos] == 0);
        if (R[T] == 1) {
          assert(dist_t[pos] != 30000);
          res += ratio * (steps + dist_t[pos]);
          return res;
          // return steps + dist_t[pos];
        }
        // if (next_pos == -1) break;
        pos = next_pos;
        RevealFrom(pos);
        ++steps;
      }
    }
    // return steps + dist_t[pos];
    assert(dist_t[pos] != 30000);
    res += (steps + dist_t[pos]) * ratio;
    return res;
  }
};

int main() {
  SetUp();

  int total_tries = 0;
  vector<int> n_tries;
  vector<double> sum_scores;
  vector<double> sum2_scores;
  vector<Simulator> simulators;
  n_tries.reserve(1000);
  sum_scores.reserve(1000);
  sum2_scores.reserve(1000);
  simulators.reserve(1000);

  {
    Board init_board;
    init_board.Init();
    // init_board.SidePlot();
    init_board.AroundPlot();
    init_board.PostPlot();
    init_board.PostPlotLegacy();

    simulators.emplace_back(init_board.B);
    Simulator& simulator = simulators.back();
    n_tries.push_back(0);
    sum_scores.push_back(0);
    sum2_scores.push_back(0);
    REP(loop,2) {
      double sim_score = simulator.Run(loop);
      n_tries.back() += 1;
      sum_scores.back() += sim_score;
      sum2_scores.back() += sim_score * sim_score;
      total_tries += 1;
    }
  }

  int64_t until = timer.start + int64_t(TIME_LIMIT * 0.65 * Timer::CYCLES_PER_SEC);
  int n_trial = 0;
  bool exists_safe = false;
  vector<Board> boards;
  auto compare = [](auto &a, auto &b) { return get<1>(a) < get<1>(b); };
  using T = tuple<int, double, int>;
  priority_queue<T, vector<T>, decltype(compare)> que(compare);

  for (;;) {
    if (timer.get_cycle() > until && (int(n_tries.size()) >= 2 || !timer.within())) break;
    constexpr int n_loop = 13;
    REP(loop,n_loop) {
      if (!timer.within()) break;
      Board board;
      double score = board.CreateCenterSnake();
      if (score == 0) continue;
      int ratio = -1;
      // if (true || rnd.next(100) < 20) board.SidePlot();
      if (!board.AroundPlot()) {
        if (exists_safe) continue;
        ratio = 1;
      } else {
        exists_safe = true;
        ratio = 10;
      }
      que.emplace(boards.size(), score * ratio, ratio);
      boards.push_back(board);
    }
    if (que.empty()) continue;
    auto [board_index, _, ratio] = que.top();
    que.pop();
    Board& board = boards[board_index];
    // REP(loop,1) {
    //   Board board2;
    //   board2.sol_type = board.sol_type;
    //   board2.path = board.path;
    //   std::copy(board.B, board.B + C * C, board2.B);

    //   board2.PostPlot();
    //   board2.PostPlotLegacy();
    //   int score = board2.Eval1() * ratio;
    //   if (score == 0) continue;
    //   n_trial++;
    //   cands.emplace_back(board2, score);
    // }
    board.PostPlot();
    board.PostPlotLegacy();
    double score = board.Eval1() * ratio;
    // double score = board.Eval2() * ratio;
    if (score == 0) continue;
    n_trial++;
    // LOG(score);

    simulators.emplace_back(board.B);
    Simulator& simulator = simulators.back();
    n_tries.push_back(0);
    sum_scores.push_back(0);
    sum2_scores.push_back(0);
    // warm up
    REP(loop,4) {
      double sim_score = simulator.Run(loop);
      n_tries.back() += 1;
      sum_scores.back() += sim_score;
      sum2_scores.back() += sim_score * sim_score;
      total_tries += 1;
    }
  }

  LOG(simulators.size());

  // UCB-V
  const int n = (int)simulators.size();
  int best_index = 0;

  while (timer.within()) {
    int best_i = -1;
    double best_ucb = -1e300;

    const double logT = std::log(std::max(2, total_tries));  // Avoid log(0)
    REP(i,n) {
      const int ni = n_tries[i];
      if (ni == 0) { best_i = i; break; }
      if (i == best_index) continue;  // Don't select the current best
      const double avg = sum_scores[i] / ni;
      double var = sum2_scores[i] / ni - avg * avg;
      if (var < 0) var = 0;  // Numerical stability

      // UCB-V index
      const double bonus =
        std::sqrt((2.0 * var * logT) / ni) + (3.0 * logT) / ni;
      const double ucb = avg + bonus;

      if (ucb > best_ucb) {
        best_ucb = ucb;
        best_i = i;
      }
    }

    assert(best_i != -1);
    double sim_score = simulators[best_i].Run(n_tries[best_i]);
    n_tries[best_i] += 1;
    sum_scores[best_i]  += sim_score;
    sum2_scores[best_i] += sim_score * sim_score;
    total_tries += 1;

    if (n_tries[best_i] > n_tries[best_index]) {
      best_index = best_i;
    }
  }

  // REP(i,n) CERR() << i << " " << n_tries[i] << " " << sum_scores[i] / max(1, n_tries[i]) << endl;
  int max_tries = *max_element(ALL(n_tries));
  LOG(max_tries);
  LOG(total_tries);

  vector<int> order(n);
  iota(ALL(order), 0);
  sort(ALL(order), [&](int i, int j) { return n_tries[i] > n_tries[j]; });

  const int n_cands = std::max(2, int(sqrt(simulators.size())));
  order.resize(n_cands);
  sort(ALL(order), [&](int i, int j) {
    return sum_scores[i] / max(1, n_tries[i]) > sum_scores[j] / max(1, n_tries[j]);
  });

  const int8_t* best_board = simulators[order[0]].B;

  // // UCB1-Tuned
  // vector<int> n_tries(cands.size(), 0);
  // vector<double> sum_scores(cands.size(), 0);
  // vector<double> sum2_scores(cands.size(), 0);
  // int total_tries = 0;
  // const int n = cands.size();
  // int best_index = 0;

  // vector<Simulator> simulators;
  // simulators.reserve(n);
  // REP(i,n) simulators.emplace_back(cands[i].first.B);

  // while (timer.within()) {
  //   int best_i = -1;
  //   double best_ucb = -1;
  //   REP(i,n) {
  //     if (n_tries[i] < 2) {
  //       best_i = i;
  //       break;
  //     }
  //     if (best_index == i) continue;
  //     double avg = sum_scores[i] / n_tries[i];
  //     double var = sum2_scores[i] / n_tries[i] - avg * avg;
  //     var = max(var, 0.0);
  //     double logT = log(total_tries);
  //     // double delta = sqrt(logT / n_tries[i] * min(0.25, var + sqrt(2 * logT / n_tries[i])));
  //     double delta = sqrt(logT / n_tries[i] * var + sqrt(1.5 * logT / n_tries[i]));
  //     double ucb = avg + delta;
  //     if (ucb > best_ucb) {
  //       best_ucb = ucb;
  //       best_i = i;
  //     }
  //   }
  //   assert(best_i != -1);
  //   // Simulator simulator(cands[best_i].first.B);
  //   double sim_score = simulators[best_i].Run(n_tries[best_i]);
  //   n_tries[best_i]++;
  //   sum_scores[best_i] += sim_score;
  //   sum2_scores[best_i] += sim_score * sim_score;
  //   total_tries++;
  //   if (n_tries[best_i] > n_tries[best_index]) best_index = best_i;
  // }

  // // REP(i,n) CERR() << i << " " << n_tries[i] << " " << sum_scores[i] / max(1, n_tries[i]) << endl;
  // int max_tries = *max_element(ALL(n_tries));
  // LOG(max_tries);
  // LOG(total_tries);

  // vector<int> order(n);
  // iota(ALL(order), 0);
  // sort(ALL(order), [&](int i, int j) { return n_tries[i] > n_tries[j]; });

  // const int n_cands = std::max(2, int(sqrt(cands.size())));
  // order.resize(n_cands);
  // sort(ALL(order), [&](int i, int j) { return sum_scores[i] / max(1, n_tries[i]) > sum_scores[j] / max(1, n_tries[j]); });

  // Board best_board = cands[order[0]].first;

  // best_board.PostPlot();
  LOG(n_trial);
  // LOG(best_board.sol_type);
  // best_board.Dump();
  Runner runner;
  runner.Init(best_board);
  runner.Run();
  double elapsed = timer.elapsed();
  LOG(elapsed);
  std::_Exit(0);
}
