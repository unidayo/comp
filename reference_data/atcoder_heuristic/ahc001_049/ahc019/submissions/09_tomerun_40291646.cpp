#include <algorithm>
#include <utility>
#include <vector>
#include <iostream>
#include <sstream>
#include <array>
#include <numeric>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <sys/time.h>

#ifdef LOCAL
#ifndef NDEBUG
// #define MEASURE_TIME
#define DEBUG
#endif
#else
#define NDEBUG
// #define DEBUG
#endif
#include <cassert>

using namespace std;
using u8=uint8_t;
using u16=uint16_t;
using u32=uint32_t;
using u64=uint64_t;
using i64=int64_t;
using ll=int64_t;
using ull=uint64_t;
using vi=vector<int>;
using vvi=vector<vi>;
using vvvi=vector<vvi>;

namespace {

#ifdef LOCAL
constexpr ll TL = 2800;
#else
constexpr ll TL = 5900;
#endif

inline ll get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  ll result =  tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;
  return result;
}

const ll start_time = get_time(); // msec

inline ll get_elapsed_msec() {
  return get_time() - start_time;
}

struct Rand {
  uint32_t x,y,z,w;
  static const double TO_DOUBLE;

  Rand() {
    x = 123456789;
    y = 362436069;
    z = 521288629;
    w = 88675123;
  }

  template<class int_type>
  int next(int_type n) {
    uint32_t t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    return w % n;
  }

  int next() {
    uint32_t t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
  }

  double next(double max) {
    return (uint32_t)next() * TO_DOUBLE * max;
  }
};
const double Rand::TO_DOUBLE = 1.0 / (1LL << 32);


struct Counter {
  vector<ull> cnt;

  void add(int i) {
    if (i >= cnt.size()) {
      cnt.resize(i+1);
    }
    ++cnt[i];
  }

  void print() {
    cerr << "counter:[";
    for (int i = 0; i < cnt.size(); ++i) {
      cerr << cnt[i] << ", ";
      if (i % 10 == 9) cerr << endl;
    }
    cerr << "]" << endl;
  }
};

struct Timer {
  vector<ull> at;
  vector<ull> sum;

  void start(int i) {
    if (i >= at.size()) {
      at.resize(i+1);
      sum.resize(i+1);
    }
    at[i] = get_time();
  }

  void stop(int i) {
    sum[i] += get_time() - at[i];
  }

  void print() {
    cerr << "timer:[";
    for (int i = 0; i < at.size(); ++i) {
      cerr << sum[i] << ", ";
      if (i % 10 == 9) cerr << endl;
    }
    cerr << "]" << endl;
  }
};

}

#ifdef MEASURE_TIME
#define START_TIMER(i) (timer.start(i))
#define STOP_TIMER(i) (timer.stop(i))
#define PRINT_TIMER() (timer.print())
#define ADD_COUNTER(i) (counter.add(i))
#define PRINT_COUNTER() (counter.print())
#else
#define START_TIMER(i)
#define STOP_TIMER(i)
#define PRINT_TIMER()
#define ADD_COUNTER(i)
#define PRINT_COUNTER()
#endif

#ifdef DEBUG
#define debug(format, ...) fprintf(stderr, format, __VA_ARGS__)
#define debugStr(str) fprintf(stderr, str)
#define debugln() fprintf(stderr, "\n")
#else
#define debug(format, ...)
#define debugStr(str)
#define debugln()
#endif

template<class T>
constexpr inline T sq(T v) { return v * v; }

void debug_vec(const vi& vec) {
  debugStr("[");
  for (int i = 0; i < vec.size(); ++i) {
    debug("%d ", vec[i]);
  }
  debugStr("]");
}

Rand rnd;
Timer timer;
Counter counter;

template<class T>
void shuffle(vector<T>& v) {
  for (int i = 0; i + 1 < v.size(); ++i) {
    int pos = rnd.next(v.size() - i) + i;
    swap(v[i], v[pos]);
  }
}

//////// end of template ////////

constexpr int INF = 1 << 30;
constexpr int DP[6] = {1 << 4, 1, 1 << 8, -(1 << 4), -1, -(1 << 8)};
constexpr int ROT_DP[24][6] = {
{16, 1, 256, -16, -1, -256},
{16, 256, -1, -16, -256, 1},
{16, -1, -256, -16, 1, 256},
{16, -256, 1, -16, 256, -1},
{1, 16, -256, -1, -16, 256},
{1, 256, 16, -1, -256, -16},
{1, -16, 256, -1, 16, -256},
{1, -256, -16, -1, 256, 16},
{256, 16, 1, -256, -16, -1},
{256, 1, -16, -256, -1, 16},
{256, -16, -1, -256, 16, 1},
{256, -1, 16, -256, 1, -16},
{-16, 1, -256, 16, -1, 256},
{-16, 256, 1, 16, -256, -1},
{-16, -1, 256, 16, 1, -256},
{-16, -256, -1, 16, 256, 1},
{-1, 16, 256, 1, -16, -256},
{-1, 256, -16, 1, -256, 16},
{-1, -16, -256, 1, 16, 256},
{-1, -256, 16, 1, 256, -16},
{-256, 16, -1, 256, -16, 1},
{-256, 1, 16, 256, -1, -16},
{-256, -16, 1, 256, 16, -1},
{-256, -1, -16, 256, 1, 16},
};
constexpr int DR[4] = {0, 1, 0, -1};
constexpr int DC[4] = {1, 0, -1, 0};
int D;

using bi_type = array<int, 16 * 16 * 16>;

inline bool inside(int v) {
  return 0 < v && v <= D;
}

int encode_pos(int z, int x, int y) {
  return (z << 8) | (x << 4) | y;
}

struct Pos {
  int v;
  Pos(int x, int y, int z) : v(encode_pos(z, x, y)) {}
  Pos(int v_) : v(v_) {}

  int x() const {
    return (v >> 4) & 0xF;
  }

  int y() const {
    return v & 0xF;
  }

  int z() const {
    return v >> 8;
  }
};

struct Block {
  vector<Pos> ps;
  int rot;

  int size() const {
    return ps.size();
  }
};

struct Silhouette {
  array<array<int, 16>, 16> f, r;
  array<array<int, 16>, 16> fc, rc;
  array<array<int, 16>, 16> gain_f, gain_r;
  vector<Pos> valid_pos;
  vector<Pos> start_empty;
  vector<Pos> start_extra;
  array<int, 16 * 16 * 16> valid_dir;

  Silhouette() {
    char buf[20];
    for (int i = 0; i < D; ++i) {
      scanf("%s", buf);
      for (int j = 0; j < D; ++j) {
        f[i + 1][j + 1] = buf[j] == '1';
      }
    }
    for (int i = 0; i < D; ++i) {
      scanf("%s", buf);
      for (int j = 0; j < D; ++j) {
        r[i + 1][j + 1] = buf[j] == '1';
      }
    }
    for (int i = 1; i <= D; ++i) {
      for (int j = 1; j <= D; ++j) {
        if (!f[i][j]) continue;
        gain_f[i][j] = 5;  // 評価値：シルエット上での次数が小さいところを埋めるのを優先したい
        for (int dir = 0; dir < 4; ++dir) {
          int ni = i + DR[dir];
          int nj = j + DC[dir];
          if (inside(ni) && inside(nj) && f[ni][nj]) gain_f[i][j]--;
        }
        gain_f[i][j] *= gain_f[i][j];
      }
    }
    for (int i = 1; i <= D; ++i) {
      for (int j = 1; j <= D; ++j) {
        if (!r[i][j]) continue;
        gain_r[i][j] = 5;
        for (int dir = 0; dir < 4; ++dir) {
          int ni = i + DR[dir];
          int nj = j + DC[dir];
          if (inside(ni) && inside(nj) && r[ni][nj]) gain_r[i][j]--;
        }
        gain_r[i][j] *= gain_r[i][j];
      }
    }
    for (int i = 1; i <= D; ++i) {
      for (int j = 1; j <= D; ++j) {
        if (!f[i][j]) continue;
        for (int k = 1; k <= D; ++k) {
          if (r[i][k]) {
            valid_pos.emplace_back(j, k, i);
          }
        }
      }
    }
  }

  bool is_done() const {
    for (int i = 1; i <= D; ++i) {
      for (int j = 1; j <= D; ++j) {
        if (f[i][j] && fc[i][j] == 0) return false;
        if (r[i][j] && rc[i][j] == 0) return false;
      }
    }
    return true;
  }

  int count_uncovered() const {
    int c = 0;
    for (int i = 1; i <= D; ++i) {
      int ft = 0;
      int rt = 0;
      for (int j = 1; j <= D; ++j) {
        if (f[i][j] && fc[i][j] == 0) ft++;
        if (r[i][j] && rc[i][j] == 0) rt++;
      }
      c += max(ft, rt);
    }
    return c;
  }

  void set_start_position(const bi_type& bi) {
    // 探索の効率化のために開始時点として選びうる位置を前計算
    start_empty.clear();
    start_extra.clear();
    for (const Pos& cp : valid_pos) {
      if (bi[cp.v]) continue;
      valid_dir[cp.v] = 0;
      for (int i = 0; i < 6; ++i) {
        int np = cp.v + DP[i];
        if (bi[np] == 0) {
          valid_dir[cp.v] |= 1 << i;
        }
      }
      if (valid_dir[cp.v] == 0) continue; // 孤立した点は除外
      if (fc[cp.z()][cp.x()] == 0 || rc[cp.z()][cp.y()] == 0) {
        start_empty.insert(start_empty.end(), gain_f[cp.z()][cp.x()] + gain_r[cp.z()][cp.y()], cp);
      } else {
        start_extra.push_back(cp);
      }
    }
  }

  void init_bi(bi_type& bi) {
    for (int z = 1; z <= D; ++z) {
      for (int x = 1; x <= D; ++x) {
        for (int y = 1; y <= D; ++y) {
          bi[encode_pos(z, x, y)] = (f[z][x] && r[z][y]) ? 0 : INF;
        }
      }
    }
  }

  void update_start_position(const Block& b) {
    for (const Pos& cp : b.ps) {
      valid_dir[cp.v] = 0;
      for (int i = 0; i < 6; ++i) {
        int np = cp.v - DP[i];
        valid_dir[np] &= ~(1 << i);
      }
    }
    for (int i = 0; i < start_extra.size(); ++i) {
      const Pos& p = start_extra[i];
      if (valid_dir[p.v] == 0) {
        swap(start_extra[i], start_extra.back());
        start_extra.pop_back();
        --i;
        continue;
      }
    }
    for (int i = 0; i < start_empty.size(); ++i) {
      const Pos& p = start_empty[i];
      if (valid_dir[p.v] == 0) {
        swap(start_empty[i], start_empty.back());
        start_empty.pop_back();
        --i;
        continue;
      } else if (fc[p.z()][p.x()] > 0 && rc[p.z()][p.y()] > 0) {
        start_extra.push_back(p);
        swap(start_empty[i], start_empty.back());
        start_empty.pop_back();
        --i;
      }
    }
  }

  vector<Pos> get_remain_pos() const {
    vector<Pos> ps;
    for (int z = 1; z <= D; ++z) {
      vi xs;
      vi ys;
      for (int i = 1; i <= D; ++i) {
        if (f[z][i] && fc[z][i] == 0) {
          xs.push_back(i);
        }
        if (r[z][i] && rc[z][i] == 0) {
          ys.push_back(i);
        }
      }
      if (xs.empty() && ys.empty()) {
        continue;
      }
      if (xs.empty()) {
        for (int i = 1; i <= D; ++i) {
          if (f[z][i]) {
            xs.push_back(i);
            break;
          }
        }
      }
      if (ys.empty()) {
        for (int i = 1; i <= D; ++i) {
          if (r[z][i]) {
            ys.push_back(i);
            break;
          }
        }
      }
      assert(!xs.empty() && !ys.empty());
      while (xs.size() < ys.size()) {
        xs.push_back(xs[0]);
      }
      while (ys.size() < xs.size()) {
        ys.push_back(ys[0]);
      }
      for (int i = 0; i < xs.size(); ++i) {
        ps.emplace_back(xs[i], ys[i], z);
      }
    }
    return ps;
  }
};

struct Result {
  vector<Block> b0, b1;
  double score;

  Result(const vector<Block>& b0_, const vector<Block>& b1_, double score_) : b0(b0_), b1(b1_), score(score_) {}
  Result(vector<Block>&& b0_, vector<Block>&& b1_, double score_) : b0(b0_), b1(b1_), score(score_) {}
};

const Result RESULT_EMPTY(vector<Block>(), vector<Block>(), 1e30);

bi_type bi0; // z, x, y
bi_type bi1;

struct Solver {

  ll timelimit;
  Silhouette s0, s1;

  Solver(ll timelimit_) : timelimit(timelimit_) {}

  void output_ans(const Result& res) {
    vvvi block_idx0(D, vvi(D, vi(D, 0)));
    vvvi block_idx1(D, vvi(D, vi(D, 0)));
    for (int i = 1; i <= D; ++i) {
      fill(s0.fc[i].begin(), s0.fc[i].end(), 0);
      fill(s0.rc[i].begin(), s0.rc[i].end(), 0);
      fill(s1.fc[i].begin(), s1.fc[i].end(), 0);
      fill(s1.rc[i].begin(), s1.rc[i].end(), 0);
    }
    debugStr("size:[ ");
    for (int i = 0; i < res.b0.size(); ++i) {
      debug("%lu ", res.b0[i].ps.size());
      for (int j = 0; j < res.b0[i].ps.size(); ++j) {
        {
          const Pos& p = res.b0[i].ps[j];
          block_idx0[p.z() - 1][p.x() - 1][p.y() - 1] = i + 1;
          s0.fc[p.z()][p.x()]++;
          s0.rc[p.z()][p.y()]++;
        }
        {
          const Pos& p = res.b1[i].ps[j];
          block_idx1[p.z() - 1][p.x() - 1][p.y() - 1] = i + 1;
          s1.fc[p.z()][p.x()]++;
          s1.rc[p.z()][p.y()]++;
        }
      }
    }
    debugStr(" ]\n");

    // fill uncovered
    int bi = 0;
    for (int z = 1; z <= D; ++z) {
      fill(s0.fc[z].begin(), s0.fc[z].end(), 0);
      fill(s0.rc[z].begin(), s0.rc[z].end(), 0);
      fill(s1.fc[z].begin(), s1.fc[z].end(), 0);
      fill(s1.rc[z].begin(), s1.rc[z].end(), 0);
      for (int x = 1; x <= D; ++x) {
        for (int y = 1; y <= D; ++y) {
          if (block_idx0[z - 1][x - 1][y - 1] != 0) {
            s0.fc[z][x]++;
            s0.rc[z][y]++;
          }
          if (block_idx1[z - 1][x - 1][y - 1] != 0) {
            s1.fc[z][x]++;
            s1.rc[z][y]++;
          }
        }
      }
    }
    vector<Pos> rp0 = s0.get_remain_pos();
    vector<Pos> rp1 = s1.get_remain_pos();
    for (int z = 1; z <= D; ++z) {
      for (int x = 1; x <= D; ++x) {
        for (int y = 1; y <= D; ++y) {
          bi = max({bi, block_idx0[z - 1][x - 1][y - 1], block_idx1[z - 1][x - 1][y - 1]});
        }
      }
    }
    bi++;
    for (int i = 0; i < min(rp0.size(), rp1.size()); ++i) {
      Pos p0 = rp0[i];
      Pos p1 = rp1[i];
      block_idx0[p0.z() - 1][p0.x() - 1][p0.y() - 1] = bi;
      block_idx1[p1.z() - 1][p1.x() - 1][p1.y() - 1] = bi;
      bi++;
    }
    for (int i = min(rp0.size(), rp1.size()); i < rp0.size(); ++i) {
      Pos p = rp0[i];
      block_idx0[p.z() - 1][p.x() - 1][p.y() - 1] = bi;
      bi++;
    }
    for (int i = min(rp0.size(), rp1.size()); i < rp1.size(); ++i) {
      Pos p = rp1[i];
      block_idx1[p.z() - 1][p.x() - 1][p.y() - 1] = bi;
      bi++;
    }

    int max_b = 0;
    for (int i = 0; i < D; ++i) {
      for (int j = 0; j < D; ++j) {
        max_b = max(max_b, *max_element(block_idx0[i][j].begin(), block_idx0[i][j].end()));
        max_b = max(max_b, *max_element(block_idx1[i][j].begin(), block_idx1[i][j].end()));
      }
    }
    printf("%d\n", max_b);
    for (int i = 0; i < D; ++i) {
      for (int j = 0; j < D; ++j) {
        for (int k = 0; k < D; ++k) {
          printf("%d ", block_idx0[k][i][j]);
        }
      }
    }
    printf("\n");
    for (int i = 0; i < D; ++i) {
      for (int j = 0; j < D; ++j) {
        for (int k = 0; k < D; ++k) {
          printf("%d ", block_idx1[k][i][j]);
        }
      }
    }
    printf("\n");
  }

  int count_connected(bi_type& bi, int s) {
    const int VISITED = 1 << 29;
    static vi q;
    q.clear();
    q.push_back(s);
    const int idx = bi[s] >> 16;
    bi[s] |= VISITED;
    for (int i = 0; i < q.size(); ++i) {
      for (int j = 0; j < 6; ++j) {
        int np = q[i] + DP[j];
        if (bi[np] & VISITED) continue;
        if ((bi[np] >> 16) == idx) {
          q.emplace_back(np);
          bi[np] |= VISITED;
        }
      }
    }
    for (int i = 0; i < q.size(); ++i) {
      bi[q[i]] &= ~VISITED;
    }
    return q.size();
  }

  Result solve() {
    const ll last_time = 500;
    Result best_res = RESULT_EMPTY;
    for (int i = 1; i <= D; ++i) {
      for (int j = 1; j <= D; ++j) {
        bi0[encode_pos(0, i, j)] = bi0[encode_pos(D + 1, i, j)] = INF;
        bi0[encode_pos(i, 0, j)] = bi0[encode_pos(i, D + 1, j)] = INF;
        bi0[encode_pos(i, j, 0)] = bi0[encode_pos(i, j, D + 1)] = INF;
        bi1[encode_pos(0, i, j)] = bi1[encode_pos(D + 1, i, j)] = INF;
        bi1[encode_pos(i, 0, j)] = bi1[encode_pos(i, D + 1, j)] = INF;
        bi1[encode_pos(i, j, 0)] = bi1[encode_pos(i, j, D + 1)] = INF;
      }
    }
    int turn = 0;
    while (true) {
      turn++;
      if (get_time() > timelimit - last_time) {
        debug("turn:%d\n", turn);
        break;
      }

      Result res = solve_single();
      if (res.score < best_res.score) {
        debug("best_score:%f turn:%d\n", res.score, turn);
        best_res = res;
      }
    }

    // 一番良かった結果を時間までさらに山登り
    for (int i = 1; ; ++i) {
      if (get_time() > timelimit) {
        debug("last_turn:%d\n", i);
        break;
      }
      double new_score = improve(best_res.b0, best_res.b1, best_res.score);
      if (new_score <= best_res.score) {
        best_res.score = new_score;
        postprocess(best_res);
        debug("improve turn=%d score=%.3f\n", i, best_res.score);
      }
    }

    return best_res;
  }

  void postprocess(Result& res) {
    // 既存の部品を拡張できるならする
    START_TIMER(15);
    vector<Block>& blocks0 = res.b0;
    vector<Block>& blocks1 = res.b1;
    s0.init_bi(bi0);
    s1.init_bi(bi1);
    vi order;
    for (int i = 1; i <= D; ++i) {
      fill(s0.fc[i].begin(), s0.fc[i].end(), 0);
      fill(s0.rc[i].begin(), s0.rc[i].end(), 0);
      fill(s1.fc[i].begin(), s1.fc[i].end(), 0);
      fill(s1.rc[i].begin(), s1.rc[i].end(), 0);
    }
    for (int i = 0; i < blocks0.size(); ++i) {
      order.push_back(i);
      for (int j = 0; j < blocks0[i].size(); ++j) {
        bi0[blocks0[i].ps[j].v] = ((i + 1) << 16) + j;
        bi1[blocks1[i].ps[j].v] = ((i + 1) << 16) + j;
        s0.fc[blocks0[i].ps[j].z()][blocks0[i].ps[j].x()]++;
        s0.rc[blocks0[i].ps[j].z()][blocks0[i].ps[j].y()]++;
        s1.fc[blocks1[i].ps[j].z()][blocks1[i].ps[j].x()]++;
        s1.rc[blocks1[i].ps[j].z()][blocks1[i].ps[j].y()]++;
      }
    }
    sort(order.begin(), order.end(), [&blocks0](int i, int j){ return blocks0[i].size() < blocks0[j].size(); });
    for (int i : order) {
      Block& b0 = blocks0[i];
      Block& b1 = blocks1[i];
      for (int j = 0; j < b0.size(); ++j) {
        for (int dir = 0; dir < 6; ++dir) {
          int np0 = b0.ps[j].v + DP[dir];
          int np1 = b1.ps[j].v + ROT_DP[b1.rot][dir];
          if (bi0[np0] == INF || bi1[np1] == INF) continue;
          if (bi0[np0] && bi1[np1]) continue;
          double diff = 1.0 / (b0.size() + 1) - 1.0 / b0.size();
          if (bi0[np0]) {
            // 他の部品とぶつかったのでへこませられるか調べる
            const int old_biv = bi0[np0];
            int obi = (old_biv >> 16) - 1;
            int obp = old_biv & 0xFFFF;
            assert(blocks0[obi].ps[obp].v == np0);
            if (obi == i) continue;
            if (blocks0[obi].size() <= b0.size()) continue;
            {
              bi0[np0] = 0;
              if (count_connected(bi0, blocks0[obi].ps[obp == 0].v) != blocks0[obi].size() - 1) {
                bi0[np0] = old_biv;
                continue;
              }
            }
            {
              Pos rmp = blocks1[obi].ps[obp];
              if (s1.fc[rmp.z()][rmp.x()] == 1 || s1.rc[rmp.z()][rmp.y()] == 1) {
                bi0[np0] = old_biv;
                continue;
              }
              assert(bi1[rmp.v] == old_biv);
              bi1[rmp.v] = 0;
              s1.fc[rmp.z()][rmp.x()]--;
              s1.rc[rmp.z()][rmp.y()]--;
            }
            Pos npp1(np1);
            s1.fc[npp1.z()][npp1.x()]++;
            s1.rc[npp1.z()][npp1.y()]++;
            for (int k = obp + 1; k < blocks0[obi].size(); ++k) {
              Pos p = blocks0[obi].ps[k];
              assert((bi0[p.v] & 0xFFFF) == k);
              bi0[p.v]--;  // インデックスをずらす
              p = blocks1[obi].ps[k];
              assert((bi1[p.v] & 0xFFFF) == k);
              bi1[p.v]--;  // インデックスをずらす
            }
            blocks0[obi].ps.erase(blocks0[obi].ps.begin() + obp);
            blocks1[obi].ps.erase(blocks1[obi].ps.begin() + obp);
            diff += 1.0 / (blocks0[obi].size()) - 1.0 / (blocks0[obi].size() + 1);
          } else if (bi1[np1]) {
            const int old_biv = bi1[np1];
            int obi = (old_biv >> 16) - 1;
            int obp = old_biv & 0xFFFF;
            assert(blocks1[obi].ps[obp].v == np1);
            if (obi == i) continue;
            if (blocks1[obi].size() <= b1.size()) continue;
            {
              bi1[np1] = 0;
              if (count_connected(bi1, blocks1[obi].ps[obp == 0].v) != blocks1[obi].size() - 1) {
                bi1[np1] = old_biv;
                continue;
              }
            }
            {
              Pos rmp = blocks0[obi].ps[obp];
              if (s0.fc[rmp.z()][rmp.x()] == 1 || s0.rc[rmp.z()][rmp.y()] == 1) {
                bi1[np1] = old_biv;
                continue;
              }
              assert(bi0[rmp.v] == old_biv);
              bi0[rmp.v] = 0;
              s0.fc[rmp.z()][rmp.x()]--;
              s0.rc[rmp.z()][rmp.y()]--;
            }
            Pos npp0(np0);
            s0.fc[npp0.z()][npp0.x()]++;
            s0.rc[npp0.z()][npp0.y()]++;
            for (int k = obp + 1; k < blocks1[obi].size(); ++k) {
              Pos p = blocks0[obi].ps[k];
              assert((bi0[p.v] & 0xFFFF) == k);
              bi0[p.v]--;  // インデックスをずらす
              p = blocks1[obi].ps[k];
              assert((bi1[p.v] & 0xFFFF) == k);
              bi1[p.v]--;  // インデックスをずらす
            }
            blocks0[obi].ps.erase(blocks0[obi].ps.begin() + obp);
            blocks1[obi].ps.erase(blocks1[obi].ps.begin() + obp);
            diff += 1.0 / (blocks0[obi].size()) - 1.0 / (blocks0[obi].size() + 1);
          } else {
            Pos p0(np0);
            s0.fc[p0.z()][p0.x()]++;
            s0.rc[p0.z()][p0.y()]++;
            Pos p1(np1);
            s1.fc[p1.z()][p1.x()]++;
            s1.rc[p1.z()][p1.y()]++;
          }

          ADD_COUNTER(20);
          bi0[np0] = ((i + 1) << 16) + b0.size();
          bi1[np1] = ((i + 1) << 16) + b1.size();
          b0.ps.emplace_back(np0);
          b1.ps.emplace_back(np1);

          // debug("diff:%.4f\n", diff);
          assert(diff <= 1e-9);
          res.score += diff;
        }
      }
    }
    STOP_TIMER(15);
  }

  Result solve_single() {
    for (int i = 1; i <= D; ++i) {
      fill(s0.fc[i].begin(), s0.fc[i].end(), 0);
      fill(s0.rc[i].begin(), s0.rc[i].end(), 0);
      fill(s1.fc[i].begin(), s1.fc[i].end(), 0);
      fill(s1.rc[i].begin(), s1.rc[i].end(), 0);
    }
    s0.init_bi(bi0);
    s1.init_bi(bi1);

    // 初期解
    START_TIMER(8);
    double score = 0.0;
    int bi = 1;
    vector<Block> blocks0;
    vector<Block> blocks1;
    pair<Block, Block> tmp_blocks;
    pair<Block, Block> best_blocks;
    while (true) {
      int best_gain = 0;
      best_blocks.first.ps.clear();
      s0.set_start_position(bi0);
      s1.set_start_position(bi1);
      for (int i = 0; i < 1; ++i) {
        tmp_blocks.first.ps.clear();
        tmp_blocks.second.ps.clear();
        int gain = select_blocks(tmp_blocks);
        if (gain == 0) {
          continue;
        }
        for (const Pos& p : tmp_blocks.first.ps) {
          bi0[p.v] = 0;
          s0.fc[p.z()][p.x()]--;
          s0.rc[p.z()][p.y()]--;
        }
        for (const Pos& p : tmp_blocks.second.ps) {
          bi1[p.v] = 0;
          s1.fc[p.z()][p.x()]--;
          s1.rc[p.z()][p.y()]--;
        }
        if (best_gain < gain) {
          best_gain = gain;
          swap(tmp_blocks, best_blocks);
          // if (bi <= 5 && best_blocks.first.size() >= 5) break;
        }
      }
      if (best_blocks.first.size() == 0) break;
      for (const Pos& p : best_blocks.first.ps) {
        bi0[p.v] = bi;
        s0.fc[p.z()][p.x()]++;
        s0.rc[p.z()][p.y()]++;
      }
      for (const Pos& p : best_blocks.second.ps) {
        bi1[p.v] = bi;
        s1.fc[p.z()][p.x()]++;
        s1.rc[p.z()][p.y()]++;
      }
      score += 1.0 / best_blocks.first.size();
      blocks0.push_back(best_blocks.first);
      blocks1.push_back(best_blocks.second);
      if (s0.is_done() && s1.is_done()) {
        break;
      }
      bi++;
    }
    int c0 = s0.count_uncovered();
    int c1 = s1.count_uncovered();
    score += max(c0, c1);
    STOP_TIMER(8);

    // 山登り
    for (int i = 1; i <= 500; ++i) {
      if (get_time() > timelimit) {
        break;
      }
      double new_score = improve(blocks0, blocks1, score);
      if (new_score < score) {
        score = new_score;
      }
    }
    Result res(blocks0, blocks1, score);
    postprocess(res);
    return res;
  }

  double improve(vector<Block>& blocks0, vector<Block>& blocks1, double old_score) {
    START_TIMER(0);
    assert(blocks0.size() == blocks1.size());
    s0.init_bi(bi0);
    s1.init_bi(bi1);
    for (int i = 1; i <= D; ++i) {
      fill(s0.fc[i].begin(), s0.fc[i].end(), 0);
      fill(s0.rc[i].begin(), s0.rc[i].end(), 0);
      fill(s1.fc[i].begin(), s1.fc[i].end(), 0);
      fill(s1.rc[i].begin(), s1.rc[i].end(), 0);
    }

    // 残す部品を選んでそれ以外を削除
    double new_score = 0.0;
    vector<bool> use_block(blocks0.size());
    int use_cnt = blocks0.size() * (rnd.next(10) + 7) / 40 + 1;
    while (use_cnt >= blocks0.size() - 1 && use_cnt > 0) use_cnt--;
    int size_sum = 0;
    for (const Block& b : blocks0) {
      size_sum += b.ps.size();
    }
    for (int i = 0; i < use_cnt; ++i) {
      int t = rnd.next(size_sum);
      int sum = 0;
      for (int j = 0; j < blocks0.size(); ++j) {
        if (use_block[j]) continue;
        sum += blocks0[j].size();
        if (t < sum) {
          use_block[j] = true;
          size_sum -= blocks0[j].size();
          new_score += 1.0 / blocks0[j].size();
          for (int k = 0; k < blocks0[j].size(); ++k) {
            {
              const Pos& p = blocks0[j].ps[k];
              bi0[p.v] = i + 1;
              s0.fc[p.z()][p.x()]++;
              s0.rc[p.z()][p.y()]++;
            }
            {
              const Pos& p = blocks1[j].ps[k];
              bi1[p.v] = i + 1;
              s1.fc[p.z()][p.x()]++;
              s1.rc[p.z()][p.y()]++;
            }
          }
          break;
        }
      }
    }
    STOP_TIMER(0);

    int bi = use_cnt + 1;
    // TODO: solve_single のコピペなのでまとめる
    vector<Block> new_blocks0;
    vector<Block> new_blocks1;
    pair<Block, Block> tmp_blocks;
    pair<Block, Block> best_blocks;
    START_TIMER(1);
    s0.set_start_position(bi0);
    s1.set_start_position(bi1);
    STOP_TIMER(1);
    const int REP = 120 + (D - 5) * 20;
    while (true) {
      int best_gain = 0;
      best_blocks.first.ps.clear();
      START_TIMER(2);
      // ランダムに部品を作って一番よかったのを選ぶ
      for (int i = 0; i < REP; ++i) {
        tmp_blocks.first.ps.clear();
        tmp_blocks.second.ps.clear();
        int gain = select_blocks(tmp_blocks);
        if (gain == 0) {
          break;
        }
        START_TIMER(13);
        for (const Pos& p : tmp_blocks.first.ps) {
          bi0[p.v] = 0;
          s0.fc[p.z()][p.x()]--;
          s0.rc[p.z()][p.y()]--;
        }
        for (const Pos& p : tmp_blocks.second.ps) {
          bi1[p.v] = 0;
          s1.fc[p.z()][p.x()]--;
          s1.rc[p.z()][p.y()]--;
        }
        // TODO:もっといい評価ができる？
        if (best_gain < gain) {
          best_gain = gain;
          swap(tmp_blocks, best_blocks);
        }
        STOP_TIMER(13);
      }
      STOP_TIMER(2);
      if (best_blocks.first.size() <= 0) break;
      for (const Pos& p : best_blocks.first.ps) {
        bi0[p.v] = bi;
        s0.fc[p.z()][p.x()]++;
        s0.rc[p.z()][p.y()]++;
      }
      for (const Pos& p : best_blocks.second.ps) {
        bi1[p.v] = bi;
        s1.fc[p.z()][p.x()]++;
        s1.rc[p.z()][p.y()]++;
      }
      new_score += 1.0 / best_blocks.first.size();
      if (old_score <= new_score) {
        return old_score;
      }
      new_blocks0.push_back(best_blocks.first);
      new_blocks1.push_back(best_blocks.second);
      if (s0.is_done() && s1.is_done()) {
        break;
      }
      bi++;
      START_TIMER(4);
      s0.update_start_position(best_blocks.first);
      s1.update_start_position(best_blocks.second);
      STOP_TIMER(4);
    }
    int c0 = s0.count_uncovered();
    int c1 = s1.count_uncovered();
    new_score += max(c0, c1);

    if (old_score <= new_score) {
      return old_score;
    } else {
      for (int i = 0; i < blocks0.size(); ++i) {
        if (use_block[i]) {
          new_blocks0.emplace_back(move(blocks0[i]));
          new_blocks1.emplace_back(move(blocks1[i]));
        }
      }
      swap(blocks0, new_blocks0);
      swap(blocks1, new_blocks1);
      return new_score;
    }
  }

  int select_blocks(pair<Block, Block>& blocks) {
    START_TIMER(10);
    // 探索の開始位置を決める
    Pos p0(0);
    Pos p1(0);
    if (s0.start_empty.empty()) {
      if (s0.start_extra.empty() || s1.start_empty.empty()) {
        return 0;
      }
      p0 = s0.start_extra[rnd.next(s0.start_extra.size())];
    } else {
      p0 = s0.start_empty[rnd.next(s0.start_empty.size())];
      if (s0.start_empty.size() > 50) {
        for (int i = 0; i < 1; ++i) {
          Pos tp = s0.start_empty[rnd.next(s0.start_empty.size())];
          if (s0.gain_f[p0.z()][p0.x()] + s0.gain_r[p0.z()][p0.y()] < s0.gain_f[tp.z()][tp.x()] + s0.gain_r[tp.z()][tp.y()]) {
            p0 = tp;
          }
        }
      }
    }
    if (s1.start_empty.empty()) {
      if (s1.start_extra.empty()) {
        return 0;
      }
      p1 = s1.start_extra[rnd.next(s1.start_extra.size())];
    } else {
      p1 = s1.start_empty[rnd.next(s1.start_empty.size())];
      if (s1.start_empty.size() > 50) {
        for (int i = 0; i < 1; ++i) {
          Pos tp = s1.start_empty[rnd.next(s1.start_empty.size())];
          if (s1.gain_f[p1.z()][p1.x()] + s1.gain_r[p1.z()][p1.y()] < s1.gain_f[tp.z()][tp.x()] + s1.gain_r[tp.z()][tp.y()]) {
            p1 = tp;
          }
        }
      }
    }
    STOP_TIMER(10);
    START_TIMER(11);
    // 回転の向きを決めて使えるだけ使う
    int rot = rnd.next(24);
    Block& block0 = blocks.first;
    Block& block1 = blocks.second;
    block0.ps.push_back(p0);
    block1.ps.push_back(p1);
    bi0[p0.v] = INF;
    bi1[p1.v] = INF;
    int gain = 0;
    // 評価：まだ埋まっていないシルエットの位置を埋めたらプラス
    if (s0.fc[p0.z()][p0.x()] == 0) gain += s0.gain_f[p0.z()][p0.x()];
    if (s0.rc[p0.z()][p0.y()] == 0) gain += s0.gain_r[p0.z()][p0.y()];
    if (s1.fc[p1.z()][p1.x()] == 0) gain += s1.gain_f[p1.z()][p1.x()];
    if (s1.rc[p1.z()][p1.y()] == 0) gain += s1.gain_r[p1.z()][p1.y()];
    s0.fc[p0.z()][p0.x()]++;
    s0.rc[p0.z()][p0.y()]++;
    s1.fc[p1.z()][p1.x()]++;
    s1.rc[p1.z()][p1.y()]++;
    const int prob_skip = 80;
    STOP_TIMER(11);
    START_TIMER(12);
    while (true) {
      for (int pi = 0; pi < block0.ps.size(); ++pi) {
        const Pos& cp0 = block0.ps[pi];
        int dirs = s0.valid_dir[cp0.v];
        while (dirs) {
          int dir = __builtin_ctz(dirs);
          dirs &= dirs - 1;
          int np1 = block1.ps[pi].v + ROT_DP[rot][dir];
          int np0 = block0.ps[pi].v + DP[dir];
          if (bi1[np1] || bi0[np0]) {
            continue;
          }
          int np1x = (np1 >> 4) & 0xF;
          int np1y = np1 & 0xF;
          int np1z = np1 >> 8;
          int np0x = (np0 >> 4) & 0xF;
          int np0y = np0 & 0xF;
          int np0z = np0 >> 8;
          if (s0.fc[np0z][np0x] > 0 && s0.rc[np0z][np0y] > 0 && s1.fc[np1z][np1x] > 0 && s1.rc[np1z][np1y] > 0 && rnd.next(prob_skip) == 0) {
            continue;
          }
          block0.ps.emplace_back(np0);
          block1.ps.emplace_back(np1);
          bi0[np0] = INF;
          bi1[np1] = INF;
          if (s0.fc[np0z][np0x] == 0) gain += s0.gain_f[np0z][np0x];
          if (s0.rc[np0z][np0y] == 0) gain += s0.gain_r[np0z][np0y];
          if (s1.fc[np1z][np1x] == 0) gain += s1.gain_f[np1z][np1x];
          if (s1.rc[np1z][np1y] == 0) gain += s1.gain_r[np1z][np1y];
          s0.fc[np0z][np0x]++;
          s0.rc[np0z][np0y]++;
          s1.fc[np1z][np1x]++;
          s1.rc[np1z][np1y]++;
        }
      }
      if (D <= 7 || block0.size() > 1) {
        block1.rot = rot;
        break;
      }
      // 最低でも複数ブロックの部品になるまでは向きを変えて再度試す
      rot++;
      if (rot == 24) rot = 0;
    }
    STOP_TIMER(12);
    return gain;
  }

};

int main() {
  ll tl = TL;
#ifdef DEBUG
  char* tl_env = getenv("TL");
  if (tl_env) {
    stringstream ss(tl_env);
    ss >> tl;
  }
#endif
  scanf("%d", &D);
  debug("D:%d\n", D);
  auto solver = unique_ptr<Solver>(new Solver(start_time + tl));
  Result res = solver->solve();
  solver->output_ans(res);
#ifdef DEBUG
  PRINT_TIMER();
  PRINT_COUNTER();
  debug("%lld\n", (int64_t)round(res.score * 1e4));
#endif
}
