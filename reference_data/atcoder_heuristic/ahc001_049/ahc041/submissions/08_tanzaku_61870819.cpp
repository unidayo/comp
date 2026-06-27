// ---- ORIGINAL SOURCE BEGIN ----
// #include <algorithm>
// #include <cstdint>
// #include <cstring>
// #include <iostream>
// #include <string>
// #include <tuple>
// #include <vector>
// 
// #include "basic.h"
// 
// constexpr double TL = 1980;
// constexpr int N = 1000;
// constexpr int MAX_M = 3000;
// constexpr int MAX_A = 100;
// constexpr int H = 10;
// int M;
// int A[N];
// int edges[MAX_M][2];
// int to_e_len[N];
// int to_e[N][N];
// 
// int depth[N];
// int children_cnt[N];
// int children[N][N];
// // int sum_A[N];
// // int sum_weighted_A[N];
// int parent[N];
// int best_parent[N];
// 
// int new_root_len;
// int new_root[N];
// 
// int best_score = 0;
// int cur_score = 0;
// int iter = 0;
// 
// // int traverse_depth = 0;
// int traverse(int d, int v, int v0)
// {
//   // traverse_depth++;
//   // DBG(traverse_depth);
//   // DBG(d);
//   // DBG(v);
//   // DBG(v0);
//   if (d == H + 1) {
//     d = 0;
//     new_root[new_root_len++] = v;
//   }
// 
//   int s = (d + 1) * A[v];
//   FOR (i, children_cnt[v]) {
//     int c = children[v][i];
//     if (v0 == c) continue;
//     s += traverse(d + 1, c, v0);
//   }
//   // traverse_depth--;
// 
//   return s;
// }
// 
// void update_depth(int d, int v)
// {
//   // traverse_depth++;
//   // DBG(traverse_depth);
//   // DBG(d);
//   // DBG(v);
//   // DBG(v0);
// 
//   // if (d > H) return;
// 
//   depth[v] = d;
//   FOR (i, children_cnt[v]) {
//     int c = children[v][i];
//     update_depth(d + 1, c);
//   }
//   // traverse_depth--;
// }
// 
// void remove_parent(int v)
// {
//   int p = parent[v];
//   if (p == -1) return;
//   int cnt = children_cnt[p];
//   FOR (i, cnt) {
//     if (children[p][i] == v) {
//       children[p][i] = children[p][--children_cnt[p]];
//       return;
//     }
//   }
// 
//   STACKTRACE();
// };
// 
// void add_child(int v, int c)
// {
//   // DBG(v);
//   // DBG(children_cnt[v]);
//   children[v][children_cnt[v]++] = c;
// };
// 
// void validate()
// {
//   // int in_child[N]{};
//   // FOR (ii, N) {
//   //   FOR (jj, children_cnt[ii]) {
//   //     in_child[children[ii][jj]]++;
//   //   }
//   // }
//   // FOR (i, N) {
//   //   if (in_child[i] > 1) {
//   //     DBG(in_child[i]);
//   //     STACKTRACE();
//   //   }
//   //   if (parent[i] == -1) {
//   //     if (in_child[i] != 0) {
//   //       FOR (ii, N) {
//   //         FOR (jj, children_cnt[ii]) {
//   //           if (children[ii][jj] == i) {
//   //             DBG(i);
//   //             DBG(in_child[i]);
//   //             DBG(ii);
//   //             DBG(jj);
//   //             DBG(children_cnt[ii]);
//   //             STACKTRACE();
//   //           }
//   //         }
//   //       }
//   //       DBG(i);
//   //       DBG(in_child[i]);
//   //       STACKTRACE();
//   //     }
//   //     continue;
//   //   }
//   //   if (in_child[i] == 0) {
//   //     DBG(i);
//   //     DBG(parent[i]);
//   //     DBG(depth[i]);
//   //     DBG(depth[parent[i]]);
//   //     DBG(in_child[i]);
//   //     STACKTRACE();
//   //   }
//   //   int p = parent[i];
//   //   int found = 0;
//   //   FOR (j, children_cnt[p]) {
//   //     if (children[p][j] == i) {
//   //       found++;
//   //       break;
//   //     }
//   //   }
//   //   if (found != 1) {
//   //     DBG(found);
//   //     STACKTRACE();
//   //   }
//   // }
//   // FOR (i, N) {
//   //   int j = parent[i];
//   //   int d = 0;
//   //   while (j != -1) {
//   //     d++;
//   //     j = parent[j];
//   //   }
//   //   if (depth[i] != d) {
//   //     DBG(i);
//   //     DBG(depth[i]);
//   //     DBG(d);
//   //     STACKTRACE();
//   //     throw;
//   //   }
//   // }
// }
// 
// // void check_calc_depth(int v)
// // {
// //   int j = parent[v];
// //   int d = 0;
// //   while (j != -1) {
// //     d++;
// //     j = parent[j];
// //   }
// //   if (depth[v] != d) {
// //     DBG(v);
// //     DBG(depth[v]);
// //     DBG(d);
// //     STACKTRACE();
// //     throw;
// //   }
// // }
// 
// // int broken_cnt[MAX_A + 1];
// // int broken[MAX_A + 1][N];
// 
// constexpr int LIMIT_CNT = 200;
// int broken_flat_cnt;
// int broken_flat[N];
// int old_parent[N];
// int old_depth[N];
// int is_need_update_depth[N];
// 
// void collect_children(int v)
// {
//   if (broken_flat_cnt > LIMIT_CNT) {
//     return;
//   }
// 
//   // DBG(v);
//   // DBG(parent[338]);
// 
//   broken_flat[broken_flat_cnt++] = v;
//   old_parent[v] = parent[v];
//   old_depth[v] = depth[v];
//   parent[v] = -1;
//   depth[v] = 0;
//   is_need_update_depth[v] = 1;
// 
//   FOR (i, children_cnt[v]) {
//     int c = children[v][i];
//     collect_children(c);
//   }
// }
// 
// // i64 stat_ok[N + 1];
// // i64 stat_ng[N + 1];
// 
// void reconstruct(int v, double temp)
// {
//   validate();
//   broken_flat_cnt = 0;
//   collect_children(v);
// 
//   if (broken_flat_cnt > LIMIT_CNT) {
//     FOR (i, broken_flat_cnt) {
//       int c = broken_flat[i];
//       parent[c] = old_parent[c];
//       depth[c] = old_depth[c];
//       is_need_update_depth[c] = 0;
//     }
// 
//     // DBG(parent[338]);
//     validate();
//     return;
//   }
// 
//   int new_score = 0;
//   int old_score = 0;
// 
//   // TODO shuffleしない？
//   // rng.shuffle(broken_flat, broken_flat_cnt);
//   // VIS();
// 
//   int que_len = 0;
//   int que[N];
//   FOR (i, broken_flat_cnt) {
//     int c = broken_flat[i];
// 
//     FOR (j, to_e_len[c]) {
//       int t = to_e[c][j];
//       if (is_need_update_depth[t] == 0) {
//         is_need_update_depth[c] = 2;
//         que[que_len++] = c;
//         break;
//       }
//     }
//   }
// 
//   // DBG(broken_flat_cnt);
//   // DBG(que_len);
//   FOR (i, que_len) {
//     int c = que[i];
// 
//     int cand_len = 0;
//     int cand[N];
//     FOR (j, to_e_len[c]) {
//       int t = to_e[c][j];
//       if (depth[t] == H || is_need_update_depth[t] != 0) continue;
//       cand[cand_len++] = t;
//     }
// 
//     // DBG(cand_len);
//     if (cand_len == 0) {
//       is_need_update_depth[c] = 1;
//     } else {
//       int p = cand[rng.nextInt(cand_len)];
//       parent[c] = p;
//       depth[c] = depth[p] + 1;
//       is_need_update_depth[c] = 0;
//       // check_calc_depth(c);
// 
//       FOR (j, to_e_len[c]) {
//         int t = to_e[c][j];
//         if (is_need_update_depth[t] == 1) {
//           is_need_update_depth[t] = 2;
//           que[que_len++] = t;
//         }
//       }
//     }
//     // old_score += (old_depth[c] + 1) * A[c];
//     // new_score += (depth[c] + 1) * A[c];
//   }
// 
//   FOR (i, broken_flat_cnt) {
//     int c = broken_flat[i];
//     // check_calc_depth(c);
//     old_score += (old_depth[c] + 1) * A[c];
//     new_score += (depth[c] + 1) * A[c];
//   }
// 
//   // FOR (i, broken_flat_cnt) {
//   //   int c = broken_flat[i];
// 
//   //   int st[N];
//   //   int st_len = 0;
//   //   int cc = c;
//   //   for (int cc = c; cc != -1 && is_need_update_depth[cc]; cc = parent[cc]) {
//   //     is_need_update_depth[cc] = false;
//   //     st[st_len++] = cc;
//   //   }
//   //   while (st_len > 0) {
//   //     int cc = st[--st_len];
//   //     depth[cc] = parent[cc] >= 0 ? depth[parent[cc]] + 1 : 0;
//   //   }
// 
//   //   old_score += (old_depth[c] + 1) * A[c];
//   //   new_score += (depth[c] + 1) * A[c];
//   // }
// 
//   if (!sa_accept_maximize(old_score, new_score, temp)) {
//     FOR (i, broken_flat_cnt) {
//       int c = broken_flat[i];
//       parent[c] = old_parent[c];
//       depth[c] = old_depth[c];
//       is_need_update_depth[c] = 0;
//     }
//     validate();
//     // stat_ng[broken_flat_cnt]++;
// 
//     return;
//   }
//   // stat_ok[broken_flat_cnt]++;
// 
//   // FOR (i, broken_flat_cnt) {
//   //   int c = broken_flat[i];
//   //   check_calc_depth(c);
//   // }
// 
//   FOR (i, broken_flat_cnt) {
//     int c = broken_flat[i];
//     is_need_update_depth[c] = 0;
//   }
// 
//   // FOR (i, broken_flat_cnt) {
//   //   int c = broken_flat[i];
//   //   check_calc_depth(c);
//   // }
// 
//   cur_score += new_score - old_score;
//   // DBG(old_score);
//   // DBG(new_score);
//   if (best_score < cur_score) {
//     best_score = cur_score;
//     memcpy(best_parent, parent, sizeof(parent));
//   }
// 
//   // FOR (i, broken_flat_cnt) {
//   //   int c = broken_flat[i];
//   //   DBG(c);
//   //   DBG(parent[c]);
//   //   DBG(depth[c]);
//   //   check_calc_depth(c);
//   // }
// 
//   FOR (i, broken_flat_cnt) {
//     int c = broken_flat[i];
//     int pp = parent[c];
//     parent[c] = old_parent[c];
//     remove_parent(c);
//     parent[c] = pp;
//   }
// 
//   // FOR (i, broken_flat_cnt) {
//   //   int c = broken_flat[i];
//   //   DBG(c);
//   //   DBG(parent[c]);
//   //   DBG(depth[c]);
//   //   check_calc_depth(c);
//   // }
// 
//   FOR (i, broken_flat_cnt) {
//     int c = broken_flat[i];
//     if (parent[c] >= 0)
//       add_child(parent[c], c);
//   }
//   // FOR (i, broken_flat_cnt) {
//   //   int c = broken_flat[i];
//   //   check_calc_depth(c);
//   // }
// 
//   validate();
// }
// 
// int main(int argc, char **argv)
// {
//   timer.reset();
// 
//   int xxxx;
//   cin >> xxxx >> M >> xxxx;
// 
//   FOR (i, N) {
//     cin >> A[i];
//   }
//   FOR (i, M) {
//     FOR (j, 2) cin >> edges[i][j];
//     to_e[edges[i][0]][to_e_len[edges[i][0]]++] = edges[i][1];
//     to_e[edges[i][1]][to_e_len[edges[i][1]]++] = edges[i][0];
//   }
//   FOR (i, N)
//     FOR (j, 2) cin >> xxxx;
// 
//   FOR (i, N) {
//     parent[i] = -1;
//     depth[i] = 0;
//     // sum_A[i] = A[i];
//     // sum_weighted_A[N] = 0;
//     children_cnt[i] = 0;
//     cur_score += A[i];
//   }
// 
//   auto is_connected = [&](int i, int j) {
//     // DBG(i);
//     // DBG(j);
//     // DBG(depth[i]);
//     // DBG(depth[j]);
//     while (depth[j] > depth[i]) {
//       j = parent[j];
//       // DBG(i);
//       // DBG(j);
//       // DBG(depth[i]);
//       // DBG(depth[j]);
//     }
// 
//     return i == j;
//   };
// 
//   sa_until(TL, 250.0, 1.0, [&](double t, double temp) {
//     int i = rng.nextInt(N);
// 
//     int j = to_e[i][rng.nextInt(to_e_len[i])];
//     if (is_connected(i, j)) {
//       return;
//     }
// 
//     if (rng.nextDouble() <= 0.95) {
//       validate();
//       // cut
//       new_root_len = 0;
//       int old_score = traverse(depth[i], i, i);
//       // diff -= sum_weighted_A[i];
// 
//       // connect
//       // int diff_depth = depth[j] - depth[i];
//       // diff += diff_depth * sum_A[i] + sum_weighted_A[j];
//       new_root_len = 0;
//       int new_score = traverse(depth[j] + 1, i, i);
// 
//       if (!sa_accept_maximize(old_score, new_score, temp)) {
//         validate();
//         return;
//       }
// 
//       cur_score += new_score - old_score;
//       if (best_score < cur_score) {
//         best_score = cur_score;
//         memcpy(best_parent, parent, sizeof(parent));
//       }
// 
//       // validate();
//       // DBG(i);
//       // DBG(j);
//       // DBG(parent[i]);
//       remove_parent(i);
//       parent[i] = j;
//       children[j][children_cnt[j]++] = i;
//       update_depth(depth[j] + 1, i);
//       // validate();
//       FOR (i, new_root_len) {
//         // DBG(i);
//         // DBG(new_root[i]);
//         // DBG(depth[new_root[i]]);
//         int &p = parent[new_root[i]];
//         if (p >= 0) {
//           children_cnt[p] = 0;
//           p = -1;
//         }
//         update_depth(0, new_root[i]);
//       }
//       validate();
//     } else {
//       int v = rng.nextInt(N);
//       reconstruct(v, temp);
//     }
// 
//     // validate();
//     iter++;
//   });
// 
//   // int score = 0;
//   FOR (i, N) {
//     // score += depth[i] * A[i];
//     if (i) cout << " ";
//     cout << best_parent[i];
//   }
// 
//   // FOR (i, N + 1) {
//   //   if (stat_ok[i] == 0 && stat_ng[i] == 0) continue;
//   //   cerr << i << " " << stat_ok[i] << " " << stat_ng[i] << endl;
//   // }
//   cout << endl;
//   // cerr << iter << endl;
//   // cerr << cur_score << endl;
//   cerr << best_score << endl;
//   // DBG(iter);
//   // DBG(cur_score);
//   // DBG(best_score);
// }
// ---- ORIGINAL SOURCE END ----
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#ifndef BASIC_H
#define BASIC_H
#ifndef LIB_H
#define LIB_H
#include <algorithm>
#include <bitset>
#include <cstring>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
using i64 = std::int64_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i8 = std::int8_t;
using u8 = std::uint8_t;
using f64 = float;
using f32 = double;
#define FOR(i, n) for (int i = 0; i < int(n); i++)
#define FOR1(i, n) for (int i = 1; i <= int(n); i++)
#define FORA(i, s, n) for (int i = int(s); i < int(n); i++)
#define RFOR(i, n) for (int i = int(n) - 1; i >= 0; i--)
#define RFOR1(i, n) for (int i = int(n); i > 0; i--)
#define RFORA(i, s, n) for (int i = int(n); i >= int(s); i--)
#define INIT_SEQ(ary) iota(ary.begin(), ary.end(), 0)
#define CLAMP(v, a, b) std::min(std::max(a, v), b)
#define CLEAR(ary) memset(&ary[0], 0, sizeof(ary[0]) * ary.size())
#define FILL(ary, v) memset(&ary[0], v, sizeof(ary[0]) * ary.size())
#define COPY(dst, src) memcpy(&dst[0], &src[0], sizeof(src[0]) * src.size())
#define CLEAR_N(ary, n) memset(&ary[0], 0, sizeof(ary[0]) * n)
#define FILL_N(ary, v, n) memset(&ary[0], 0, sizeof(ary[0]) * n)
#define COPY_N(dst, src, n) memcpy(&dst[0], &src[0], sizeof(src[0]) * n)
#ifdef LOCAL
#define VIS() cerr << "VIS: line=" << __LINE__ << " (" << timer.elapsed() << "[ms])" << endl;
#define DBG(x) cerr << "DBG: " << __FILE__ << ":" << __LINE__ << " " << #x << "=" << x << " (" << timer.elapsed() << "[ms])" << endl;
#define DBG_V(x) cerr << x << endl;
#define STACKTRACE()     \
  {                      \
    volatile int *x = 0; \
    *x = 0;              \
  }
#else
#define VIS()
#define DBG(x)
#define STACKTRACE()
#endif
template <typename T>
inline void fill(T &x, typename T::value_type v)
{
  std::fill(x.begin(), x.end(), v);
}
inline int bit(int x, int b)
{
  return x >> b & 1;
}
inline void set_bit(int x, int b)
{
  x |= 1 << b;
}
inline void clear_bit(int x, int b)
{
  x &= ~(1 << b);
}
using u8 = uint8_t;
using i32 = int32_t;
using i64 = int64_t;
constexpr i64 inf_i64 = i64(1) << 60;
constexpr i32 inf_i32 = i32(1) << 29;
#include <chrono>
class Timer
{
  std::chrono::system_clock::time_point start;
  std::chrono::system_clock::time_point end;
  int throttle;
public:
  Timer() { reset(); }
  void reset()
  {
    throttle = 0;
    start = std::chrono::system_clock::now();
    end = start;
  }
  inline double force_elapsed()
  {
    end = std::chrono::system_clock::now();
    throttle = 0;
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  }
  inline double elapsed()
  {
    end = std::chrono::system_clock::now();
    throttle = 0;
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  }
};
#include <cmath>
#include <cstdint>
#include <tuple>
class XorShiftL
{
  static const int LOG_CACHE_SIZE = 0x10000;
  int logIndex = 0;
  double logCache[LOG_CACHE_SIZE];
  std::uint64_t x = 88172645463325252LL;
public:
  XorShiftL()
  {
    for (int i = 0; i < LOG_CACHE_SIZE; i++) {
      logCache[i] = log((i + 0.5) / 0x10000);
    }
    shuffle(logCache);
  }
  std::uint64_t xor64()
  {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }
  int nextInt(int n) { return xor64() % n; }
  int nextInt(int low, int high) { return xor64() % (high - low) + low; }
  double nextDouble() { return xor64() / double(std::numeric_limits<std::uint64_t>::max()); }
  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) が保証されている
   */
  std::tuple<int, int> nextIntPair(int low, int high)
  {
    int i = nextInt(low, high);
    int j = nextInt(low, high - 1);
    if (j >= i)
      j++;
    else
      std::swap(i, j);
    return std::make_tuple(i, j);
  }
  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) が保証されている
   */
  std::tuple<int, int> nextIntPair(int n)
  {
    int i = nextInt(n);
    int j = nextInt(n - 1);
    if (j >= i)
      j++;
    else
      std::swap(i, j);
    return std::make_tuple(i, j);
  }
  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) は保証されない
   */
  std::tuple<int, int> nextIntUnorderedPair(int low, int high)
  {
    int i = nextInt(low, high);
    int j = nextInt(low, high - 1);
    if (j >= i)
      j++;
    return std::make_tuple(i, j);
  }
  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) は保証されない
   */
  std::tuple<int, int> nextIntUnorderedPair(int n)
  {
    int i = nextInt(n);
    int j = nextInt(n - 1);
    if (j >= i)
      j++;
    return std::make_tuple(i, j);
  }
  /**
   * ログを取得する
   */
  double nextLog()
  {
    const double res = logCache[logIndex++];
    if (logIndex == LOG_CACHE_SIZE) {
      logIndex = 0;
    }
    return res;
  }
  template <typename T>
  void shuffle(T &s)
  {
    for (int i = 1; i < (int)s.size(); i++) {
      int j = nextInt(0, i + 1);
      if (i != j) {
        std::swap(s[i], s[j]);
      }
    }
  }
  template <typename T, int n>
  void shuffle(T (&ary)[n])
  {
    for (int i = 1; i < n; i++) {
      int j = nextInt(0, i + 1);
      if (i != j) {
        std::swap(ary[i], ary[j]);
      }
    }
  }
  template <typename T, int n>
  void shuffle(T (&ary)[n], int m)
  {
    for (int i = 1; i < m; i++) {
      int j = nextInt(0, i + 1);
      if (i != j) {
        std::swap(ary[i], ary[j]);
      }
    }
  }
  template <typename T>
  auto get(T ary) -> decltype(ary[0])
  {
    return ary[nextInt(ary.size())];
  }
};
XorShiftL rng;
Timer timer;
#if 1
#include <algorithm>
#include <array>
#include <queue>
#include <tuple>
#include <vector>
using namespace std;
struct union_find {
  vector<int> data;
  void init(int size)
  {
    data.resize(size);
    clear();
  }
  void clear()
  {
    fill(data.begin(), data.end(), -1);
  }
  int root(int x) { return data[x] < 0 ? x : (data[x] = root(data[x])); }
  void unite(int x, int y)
  {
    if ((x = root(x)) != (y = root(y))) {
      if (data[y] < data[x]) {
        swap(x, y);
      }
      data[x] += data[y];
      data[y] = x;
    }
  }
  bool same(int x, int y) { return root(x) == root(y); }
  int size(int x) { return -data[root(x)]; }
};
struct union_find_clear_fast {
  vector<int> data;
  uint64_t gid;
  vector<uint64_t> id;
  void init(int size)
  {
    data.resize(size);
    id.resize(size);
    gid = 1;
  }
  void clear()
  {
    gid++;
  }
  inline int get(int x)
  {
    if (id[x] != gid) {
      data[x] = -1;
      id[x] = gid;
    }
    return data[x];
  }
  int root(int x) { return get(x) < 0 ? x : (data[x] = root(data[x])); }
  void unite(int x, int y)
  {
    if ((x = root(x)) != (y = root(y))) {
      if (data[y] < data[x]) {
        swap(x, y);
      }
      data[x] += data[y];
      data[y] = x;
    }
  }
  bool same(int x, int y) { return root(x) == root(y); }
  int size(int x) { return -data[root(x)]; }
};
#endif
#include <vector>
class Cookie
{
  int gid;
  std::vector<int> id;
public:
  void init(int cookieSize)
  {
    if ((int)id.size() < cookieSize) {
      gid = 0;
      id.resize(cookieSize);
      fill(id.begin(), id.end(), 0);
    }
  }
  bool contains(int i) const { return id[i] == gid; }
  void clear() { gid++; }
  void set(int i)
  {
    id[i] = gid;
  }
  void remove(int i)
  {
    id[i] = gid - 1;
  }
};
class DenseSet
{
public:
  vector<int> i2v;
  vector<int> v2i;
  void init(int capacity)
  {
    i2v.reserve(capacity);
    v2i.resize(capacity);
    fill(::begin(v2i), ::end(v2i), capacity);
  }
  void clear()
  {
    i2v.clear();
  }
  bool insert(int v)
  {
    if (contains(v)) {
      return false;
    }
    v2i[v] = i2v.size();
    i2v.emplace_back(v);
    return true;
  }
  void remove(int v)
  {
    int i = v2i[v];
    if (i >= int(i2v.size())) {
      return;
    }
    int last_v = i2v.back();
    i2v[i] = last_v;
    v2i[last_v] = i;
    i2v.pop_back();
    v2i[v] = int(v2i.size());
  }
  int size() const
  {
    return i2v.size();
  }
  vector<int>::iterator begin()
  {
    return i2v.begin();
  }
  vector<int>::iterator end()
  {
    return i2v.end();
  }
  bool contains(int v) const
  {
    return v2i[v] < int(i2v.size());
  }
};
template <typename T>
using PQ = priority_queue<T, vector<T>, greater<T>>;
template <typename T, int n>
struct arr {
  using value_type = T;
  array<T, n> ary;
  int sz = n;
  arr() : sz(n)
  {
  }
  arr(const arr<T, n> &other) : sz(other.sz)
  {
    FOR (i, sz) ary[i] = other.ary[i];
  }
  arr<T, n> &operator=(arr<T, n> &rhs)
  {
    sz = rhs.sz;
    FOR (i, sz) ary[i] = rhs.ary[i];
    return *this;
  }
  void resize(int m)
  {
    sz = m;
  }
  int size() const
  {
    return sz;
  }
  int capacity() const
  {
    return int(ary.size());
  }
  T *begin()
  {
    return ary.begin();
  }
  T *end()
  {
    return ary.begin() + sz;
  }
  T &operator[](int i)
  {
#if LOCAL
    if (i < 0 || i >= sz) {
      STACKTRACE();
    }
#endif
    return ary[i];
  }
  const T &operator[](int i) const
  {
#if LOCAL
    if (i < 0 || i >= sz) {
      STACKTRACE();
    }
#endif
    return ary[i];
  }
  void sort()
  {
    ::sort(begin(), end());
  }
  void reverse_range(int i, int j)
  {
    ::reverse(begin() + i, begin() + j);
  }
};
template <typename T, int n>
struct vec {
  using value_type = T;
  array<T, n> ary;
  int sz;
  vec() : sz(0)
  {
  }
  vec(const vec<T, n> &other) : sz(other.sz)
  {
    FOR (i, sz) ary[i] = other.ary[i];
  }
  vec<T, n> &operator=(vec<T, n> &rhs)
  {
    sz = rhs.sz;
#if LOCAL
    if (sz < 0 || sz > n) {
      STACKTRACE();
    }
#endif
    FOR (i, sz) ary[i] = rhs.ary[i];
    return *this;
  }
  vec<T, n> &operator=(const vec<T, n> &rhs)
  {
    sz = rhs.sz;
#if LOCAL
    if (sz < 0 || sz > n) {
      STACKTRACE();
    }
#endif
    FOR (i, sz) ary[i] = rhs.ary[i];
    return *this;
  }
  void push(T x)
  {
#if LOCAL
    if (sz >= n) {
      STACKTRACE();
    }
#endif
    ary[sz++] = x;
  }
  template <class... Args>
  void emplace(Args &&...args)
  {
    push(T(std::forward<Args>(args)...));
  }
  T &back()
  {
#if LOCAL
    if (sz <= 0) {
      STACKTRACE();
    }
#endif
    return ary[sz - 1];
  }
  void clear()
  {
    sz = 0;
  }
  T pop()
  {
#if LOCAL
    if (sz <= 0) {
      STACKTRACE();
    }
#endif
    return ary[--sz];
  }
  T pop_swap(int i)
  {
#if LOCAL
    if (i < 0 || i >= sz || sz <= 0) {
      STACKTRACE();
    }
#endif
    auto e = ary[i];
    ary[i] = ary[--sz];
    return e;
  }
  bool is_empty() const
  {
    return sz == 0;
  }
  T peek()
  {
#if LOCAL
    if (sz <= 0) {
      STACKTRACE();
    }
#endif
    return ary[sz - 1];
  }
  void resize(int m)
  {
    sz = m;
  }
  int size() const
  {
    return sz;
  }
  T remove(int i)
  {
#if LOCAL
    if (i < 0 || i >= sz) {
      STACKTRACE();
    }
#endif
    auto x = ary[i];
    FORA (j, i, sz - 1)
      ary[j] = ary[j + 1];
    sz--;
    return x;
  }
  void insert(int i, T x)
  {
#if LOCAL
    if (i < 0 || i > sz) {
      STACKTRACE();
    }
#endif
    RFORA (j, i, sz - 1)
      ary[j + 1] = ary[j];
    ary[i] = x;
    sz++;
  }
  void remove_element(T &v)
  {
    RFOR (i, sz) {
      if (ary[i] == v) {
        pop_swap(i);
        break;
      }
    }
  }
  void remove_insert(int i, int j)
  {
    auto v = this->remove(i);
    this->insert(j, v);
  }
  T *max() const
  {
    return max_element(begin(), end());
  }
  T *begin()
  {
    return ary.begin();
  }
  T *end()
  {
    return ary.begin() + sz;
  }
  T &operator[](int i)
  {
#if LOCAL
    if (i < 0 || i >= sz) {
      DBG(i);
      DBG(sz);
      STACKTRACE();
    }
#endif
    return ary[i];
  }
  const T &operator[](int i) const
  {
#if LOCAL
    if (i < 0 || i >= sz) {
      DBG(i);
      DBG(sz);
      STACKTRACE();
    }
#endif
    return ary[i];
  }
  template <typename Cmp>
  void sort_by(Cmp cmp)
  {
    ::sort(begin(), end(), cmp);
  }
  void sort()
  {
    ::sort(begin(), end());
  }
  void reverse()
  {
    ::reverse(begin(), end());
  }
  void reverse_range(int i, int j)
  {
    reverse(begin() + i, begin() + j);
  }
  void rsort()
  {
    sort();
    reverse();
  }
};
template <int n>
struct cookie {
  array<uint64_t, n> ary;
  uint64_t id;
  cookie()
  {
    CLEAR(ary);
    id = 1;
  }
  void clear()
  {
    if (++id == 0) {
      CLEAR(ary);
    }
  }
  bool add(int x)
  {
    if (ary[x] == id) {
      return false;
    } else {
      ary[x] = id;
      return true;
    }
  }
  bool contains(int x) const
  {
    return ary[x] == id;
  }
  bool operator[](int x) const
  {
    return ary[x] == id;
  }
};
int grid_n;
int dirs[4];
vector<tuple<int, int>> all_adj_pairs;
vector<arr<uint8_t, 4>> valid_dir;
vector<arr<int, 4>> next_dir;
int to_1d(tuple<int, int> tpl)
{
  return get<0>(tpl) * grid_n + get<1>(tpl);
}
int to_1d(int a, int b)
{
  return a * grid_n + b;
}
tuple<int, int> to_2d(int g)
{
  return make_tuple(g / grid_n, g % grid_n);
}
void grid_init(int n)
{
  grid_n = n;
  all_adj_pairs.reserve(n * 4);
  valid_dir.resize(n * n);
  next_dir.resize(n * n);
  FOR (i, n * n) fill(next_dir[i].begin(), next_dir[i].end(), -1);
  dirs[0] = 1;
  dirs[1] = -n;
  dirs[2] = -1;
  dirs[3] = n;
  FOR (i, n) {
    FOR (j, n) {
      int ij = to_1d(i, j);
      if (j + 1 < n) {
        next_dir[ij][0] = ij + 1;
        next_dir[ij + 1][2] = ij;
        valid_dir[ij][0] = 1;
        valid_dir[ij + 1][2] = 1;
      }
      if (i + 1 < n) {
        next_dir[ij + n][1] = ij;
        next_dir[ij][3] = ij + n;
        valid_dir[ij][3] = 1;
        valid_dir[ij + n][1] = 1;
      }
      if (i + 1 < n) all_adj_pairs.emplace_back(ij, ij + n);
      if (j + 1 < n) all_adj_pairs.emplace_back(ij, ij + 1);
    }
  }
}
struct P {
  int x, y;
};
template <typename T>
T sqr(T x)
{
  return x * x;
}
double euclid_dist(const P &a, const P &b)
{
  return sqrt(sqr(a.x - b.x) + sqr(a.y - b.y));
}
i64 euclid_dist_ceil(const P &a, const P &b)
{
  return i64(ceil(euclid_dist(a, b)));
}
istream &operator>>(istream &is, P &p)
{
  is >> p.x >> p.y;
  return is;
}
ostream &operator<<(ostream &os, P &p)
{
  os << "(" << p.x << "," << p.y << ")";
  return os;
}
template <typename T1, typename T2>
ostream &operator<<(ostream &os, tuple<T1, T2> v)
{
  os << "(" << get<0>(v) << "," << get<1>(v) << ")";
  return os;
}
template <typename T1, typename T2, typename T3>
ostream &operator<<(ostream &os, tuple<T1, T2, T3> v)
{
  os << "(" << get<0>(v) << "," << get<1>(v) << "," << get<2>(v) << ")";
  return os;
}
template <typename T, int n>
ostream &operator<<(ostream &os, vec<T, n> &v)
{
  FOR (i, v.size()) {
    if (i) os << " ";
    os << v[i];
  }
  return os;
}
ostream &operator<<(ostream &os, uint8_t &v)
{
  os << int64_t(v);
  return os;
}
template <int n>
ostream &operator<<(ostream &os, vec<uint8_t, n> &v)
{
  FOR (i, v.size()) {
    if (i) os << " ";
    os << int64_t(v[i]);
  }
  return os;
}
template <typename T>
ostream &operator<<(ostream &os, const vector<T> &v)
{
  FOR (i, v.size()) {
    if (i) os << " ";
    os << v[i];
  }
  return os;
}
template <typename T, int n>
ostream &operator<<(ostream &os, arr<T, n> &v)
{
  FOR (i, v.size()) {
    if (i) os << " ";
    os << v[i];
  }
  return os;
}
template <int n>
ostream &operator<<(ostream &os, arr<uint8_t, n> &v)
{
  FOR (i, v.size()) {
    if (i) os << " ";
    os << int64_t(v[i]);
  }
  return os;
}
bool sa_accept_minimize(double old_cost, double new_cost, double temp)
{
  return new_cost <= old_cost - temp * rng.nextLog();
}
bool sa_accept_maximize(double old_score, double new_score, double temp)
{
  return new_score >= old_score + temp * rng.nextLog();
}
template <typename F>
void sa_until(double end_time, double start_temp, double end_temp, F f)
{
  double start_time = timer.elapsed();
  while (1) {
    double cur_time = timer.elapsed();
    if (cur_time >= end_time) {
      break;
    }
    double t = (cur_time - start_time) / (end_time - start_time);
    double cur_temp = start_temp * (1 - t) + end_temp * t;
    f(t, cur_temp);
  }
}
inline int leading_zeros(uint64_t x)
{
  return __builtin_clzll(x);
}
inline int trailing_zeros(uint64_t x)
{
  return __builtin_ctzll(x);
}
inline int popcount(uint64_t x)
{
  return __builtin_popcountll(x);
}
#endif // LIB_H
template <class T>
std::tuple<typename T::value_type, typename T::value_type> fl_minmax(std::initializer_list<T> t)
{
  using value_type = typename T::value_type;
  auto min = std::numeric_limits<value_type>::max();
  auto max = std::numeric_limits<value_type>::min();
  for (auto &&c : t) {
    auto [min_val, max_val] = std::minmax_element(c.begin(), c.end());
    min = std::min(min, *min_val);
    max = std::max(max, *max_val);
  }
  return {min, max};
}
template <class... T>
auto fl_minmax(T... a)
{
  return fl_minmax(std::initializer_list{a...});
}
template <class T>
typename T::value_type fl_max(std::initializer_list<T> t)
{
  using value_type = typename T::value_type;
  auto max = std::numeric_limits<value_type>::min();
  for (auto &&c : t) {
    auto max_val = std::max_element(c.begin(), c.end());
    max = std::max(max, *max_val);
  }
  return max;
}
template <class... T>
auto fl_max(T... a)
{
  return fl_max(std::initializer_list{a...});
}
template <class T>
typename T::value_type fl_min(std::initializer_list<T> t)
{
  using value_type = typename T::value_type;
  auto max = std::numeric_limits<value_type>::min();
  for (auto &&c : t) {
    auto max_val = std::min_element(c.begin(), c.end());
    max = std::min(max, *max_val);
  }
  return max;
}
template <class... T>
auto fl_min(T... a)
{
  return fl_min(std::initializer_list{a...});
}

#endif

constexpr double TL = 1980;
constexpr int N = 1000;
constexpr int MAX_M = 3000;
constexpr int MAX_A = 100;
constexpr int H = 10;
int M;
int A[N];
int edges[MAX_M][2];
int to_e_len[N];
int to_e[N][N];
int depth[N];
int children_cnt[N];
int children[N][N];
int parent[N];
int best_parent[N];
int new_root_len;
int new_root[N];
int best_score = 0;
int cur_score = 0;
int iter = 0;
int traverse(int d, int v, int v0)
{
  if (d == H + 1) {
    d = 0;
    new_root[new_root_len++] = v;
  }
  int s = (d + 1) * A[v];
  FOR (i, children_cnt[v]) {
    int c = children[v][i];
    if (v0 == c) continue;
    s += traverse(d + 1, c, v0);
  }
  return s;
}
void update_depth(int d, int v)
{
  depth[v] = d;
  FOR (i, children_cnt[v]) {
    int c = children[v][i];
    update_depth(d + 1, c);
  }
}
void remove_parent(int v)
{
  int p = parent[v];
  if (p == -1) return;
  int cnt = children_cnt[p];
  FOR (i, cnt) {
    if (children[p][i] == v) {
      children[p][i] = children[p][--children_cnt[p]];
      return;
    }
  }
  STACKTRACE();
};
void add_child(int v, int c)
{
  children[v][children_cnt[v]++] = c;
};
void validate()
{
}
constexpr int LIMIT_CNT = 200;
int broken_flat_cnt;
int broken_flat[N];
int old_parent[N];
int old_depth[N];
int is_need_update_depth[N];
void collect_children(int v)
{
  if (broken_flat_cnt > LIMIT_CNT) {
    return;
  }
  broken_flat[broken_flat_cnt++] = v;
  old_parent[v] = parent[v];
  old_depth[v] = depth[v];
  parent[v] = -1;
  depth[v] = 0;
  is_need_update_depth[v] = 1;
  FOR (i, children_cnt[v]) {
    int c = children[v][i];
    collect_children(c);
  }
}
void reconstruct(int v, double temp)
{
  validate();
  broken_flat_cnt = 0;
  collect_children(v);
  if (broken_flat_cnt > LIMIT_CNT) {
    FOR (i, broken_flat_cnt) {
      int c = broken_flat[i];
      parent[c] = old_parent[c];
      depth[c] = old_depth[c];
      is_need_update_depth[c] = 0;
    }
    validate();
    return;
  }
  int new_score = 0;
  int old_score = 0;
  int que_len = 0;
  int que[N];
  FOR (i, broken_flat_cnt) {
    int c = broken_flat[i];
    FOR (j, to_e_len[c]) {
      int t = to_e[c][j];
      if (is_need_update_depth[t] == 0) {
        is_need_update_depth[c] = 2;
        que[que_len++] = c;
        break;
      }
    }
  }
  FOR (i, que_len) {
    int c = que[i];
    int cand_len = 0;
    int cand[N];
    FOR (j, to_e_len[c]) {
      int t = to_e[c][j];
      if (depth[t] == H || is_need_update_depth[t] != 0) continue;
      cand[cand_len++] = t;
    }
    if (cand_len == 0) {
      is_need_update_depth[c] = 1;
    } else {
      int p = cand[rng.nextInt(cand_len)];
      parent[c] = p;
      depth[c] = depth[p] + 1;
      is_need_update_depth[c] = 0;
      FOR (j, to_e_len[c]) {
        int t = to_e[c][j];
        if (is_need_update_depth[t] == 1) {
          is_need_update_depth[t] = 2;
          que[que_len++] = t;
        }
      }
    }
  }
  FOR (i, broken_flat_cnt) {
    int c = broken_flat[i];
    old_score += (old_depth[c] + 1) * A[c];
    new_score += (depth[c] + 1) * A[c];
  }
  if (!sa_accept_maximize(old_score, new_score, temp)) {
    FOR (i, broken_flat_cnt) {
      int c = broken_flat[i];
      parent[c] = old_parent[c];
      depth[c] = old_depth[c];
      is_need_update_depth[c] = 0;
    }
    validate();
    return;
  }
  FOR (i, broken_flat_cnt) {
    int c = broken_flat[i];
    is_need_update_depth[c] = 0;
  }
  cur_score += new_score - old_score;
  if (best_score < cur_score) {
    best_score = cur_score;
    memcpy(best_parent, parent, sizeof(parent));
  }
  FOR (i, broken_flat_cnt) {
    int c = broken_flat[i];
    int pp = parent[c];
    parent[c] = old_parent[c];
    remove_parent(c);
    parent[c] = pp;
  }
  FOR (i, broken_flat_cnt) {
    int c = broken_flat[i];
    if (parent[c] >= 0)
      add_child(parent[c], c);
  }
  validate();
}
int main(int argc, char **argv)
{
  timer.reset();
  int xxxx;
  cin >> xxxx >> M >> xxxx;
  FOR (i, N) {
    cin >> A[i];
  }
  FOR (i, M) {
    FOR (j, 2) cin >> edges[i][j];
    to_e[edges[i][0]][to_e_len[edges[i][0]]++] = edges[i][1];
    to_e[edges[i][1]][to_e_len[edges[i][1]]++] = edges[i][0];
  }
  FOR (i, N)
    FOR (j, 2) cin >> xxxx;
  FOR (i, N) {
    parent[i] = -1;
    depth[i] = 0;
    children_cnt[i] = 0;
    cur_score += A[i];
  }
  auto is_connected = [&](int i, int j) {
    while (depth[j] > depth[i]) {
      j = parent[j];
    }
    return i == j;
  };
  sa_until(TL, 250.0, 1.0, [&](double t, double temp) {
    int i = rng.nextInt(N);
    int j = to_e[i][rng.nextInt(to_e_len[i])];
    if (is_connected(i, j)) {
      return;
    }
    if (rng.nextDouble() <= 0.95) {
      validate();
      new_root_len = 0;
      int old_score = traverse(depth[i], i, i);
      new_root_len = 0;
      int new_score = traverse(depth[j] + 1, i, i);
      if (!sa_accept_maximize(old_score, new_score, temp)) {
        validate();
        return;
      }
      cur_score += new_score - old_score;
      if (best_score < cur_score) {
        best_score = cur_score;
        memcpy(best_parent, parent, sizeof(parent));
      }
      remove_parent(i);
      parent[i] = j;
      children[j][children_cnt[j]++] = i;
      update_depth(depth[j] + 1, i);
      FOR (i, new_root_len) {
        int &p = parent[new_root[i]];
        if (p >= 0) {
          children_cnt[p] = 0;
          p = -1;
        }
        update_depth(0, new_root[i]);
      }
      validate();
    } else {
      int v = rng.nextInt(N);
      reconstruct(v, temp);
    }
    iter++;
  });
  FOR (i, N) {
    if (i) cout << " ";
    cout << best_parent[i];
  }
  cout << endl;
  cerr << best_score << endl;
}

