#pragma GCC optimize("O3")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")

// #define NDEBUG  // TODO

// #include <atcoder/mincostflow>

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
#define HHH(x) cerr << "L" << __LINE__ << ": " << #x << " : " << (x) << endl
#define LOG(x) cerr << #x << " = " << x << endl
#define GRAPH(x, y) cerr << "graph " << x << " " << y << endl;

using namespace std;

template <typename T> T &chmin(T &a, const T &b) { return a = min(a, b); }
template <typename T> T &chmax(T &a, const T &b) { return a = max(a, b); }

using ll = long long;
using ld = double;
using P = pair<int,int>;
const int INF = 1e9;

// Template
ostream& operator<<(ostream& os, const P& p) {
  return os << "(" << p.first << ", " << p.second << ")";
}

template <typename T> ostream& operator<<(ostream& os, const vector<T>& vec) {
  os << "[";
  for (const T& e : vec) os << " " << e;
  os << " ]";
  return os;
}

class Timer {
public:
  int64_t start, stop;
  static const int64_t CYCLES_PER_SEC = 2900000000;
  inline int64_t get_cycle() const {
    uint32_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return int64_t(low) | (int64_t(high) << 32);
  }
  Timer(double time_limit) :
    start(get_cycle()), stop(start + time_limit * CYCLES_PER_SEC) {}
  Timer() : Timer(0) {}
  inline bool within(int64_t end) const { return get_cycle() < end; }
  inline bool within() const { return within(stop); }
};

class XorShift {
  unsigned int x, y, z, w;
  double nL[65536];
public:
  XorShift() : x(314159265), y(358979323), z(846264338), w(327950288) {}
  inline unsigned int next() {
    unsigned int t = x ^ x << 11;
    x = y; y = z; z = w;
    return w = w ^ w >> 19 ^ t ^ t >> 8;
  }
  inline int next(int n) { return next() % n; }
  inline int next(int lo, int hi) { return lo + next(hi - lo + 1); }
  inline double next(double x) { return double(next()) / (1LL << 32) * x; }
};

double get_param(const char* env_var) {
  char* ret = getenv(env_var);
  assert (ret != nullptr);
  return stod(string(ret));
}

// double a_param = get_param("A_PARAM");
// double b_param = get_param("B_PARAM");
// double c_param = get_param("C_PARAM");
// int a_param = get_param("A_PARAM");
// int j_param = get_param("J_PARAM");

Timer timer(1.99);
XorShift rnd;

constexpr int GRID_SIZE = 40;
pair<int, int> A[10010];
constexpr int MAX_LEN = 400000;
constexpr int GRID_LEN = 100000 / GRID_SIZE;
int linspace[GRID_SIZE + 1];
vector<int> bucket[GRID_SIZE][GRID_SIZE];
int bucket_score[GRID_SIZE][GRID_SIZE];

constexpr int N = 5000;

void setup() {
  int N_;
  scanf("%d", &N_);
  REP(i,N*2) scanf("%d %d", &A[i].second, &A[i].first);

  REP(i,GRID_SIZE + 1) linspace[i] = i * 100000LL / GRID_SIZE;
  REP(i,N*2) {
    int y = A[i].first, x = A[i].second;
    int y_idx = min(y / GRID_LEN, GRID_SIZE);
    int x_idx = min(x / GRID_LEN, GRID_SIZE);
    bucket[y_idx][x_idx].push_back(i);
    bucket_score[y_idx][x_idx] += i < N ? 1 : -1;
  }
}

int Hash(int y, int x) { return y * (GRID_SIZE + 1) + x; };

void EarlyExit() {
  std::_Exit(0);
}

const int dy8[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const int dx8[8] = {1, 1, 0, -1, -1, -1, 0, 1};
const int dy4[4] = {0, 1, 0, -1};
const int dx4[4] = {1, 0, -1, 0};

struct State {
  vector<vector<bool>> used;
  int score = 1;
  int len = 0;
  State() : used(GRID_SIZE, vector<bool>(GRID_SIZE, false)) {
  }
  bool is_used(int i, int j) const {
    if (i < 0 || i >= GRID_SIZE || j < 0 || j >= GRID_SIZE) return false;
    return used[i][j];
  }
  bool CanChange(int y, int x) const {
    int dlen = 0;
    int c = 0;
    bool cur_used = used[y][x];
    REP(dir,4) {
      int ny = y + dy4[dir];
      int nx = x + dx4[dir];
      bool next_used = is_used(ny, nx);
      c += next_used;
      dlen += (next_used == cur_used ? GRID_LEN : -GRID_LEN);
    }
    if (len + dlen > MAX_LEN) return false;
    if (c == 0) return false;
    if (c == 4) return false;

    int d = 0;
    REP(dir,8) {
      int ny1 = y + dy8[dir];
      int nx1 = x + dx8[dir];
      int ny2 = y + dy8[(dir + 1) & 7];
      int nx2 = x + dx8[(dir + 1) & 7];
      d += (is_used(ny1, nx1) != is_used(ny2, nx2));
    }
    return d <= 2;
  }
  int Predict(int y, int x) const {
    // std::cerr << y << " " << x << ":" << CanChange(y, x) << std::endl;
    if (!CanChange(y, x)) return -INF;
    if (used[y][x]) return -bucket_score[y][x];
    else return bucket_score[y][x];
  }
  void Change(int y, int x) {
    used[y][x] = !used[y][x];
    if (used[y][x]) score += bucket_score[y][x];
    else score -= bucket_score[y][x];

    bool cur_used = used[y][x];
    REP(dir,4) {
      int ny = y + dy4[dir];
      int nx = x + dx4[dir];
      bool next_used = is_used(ny, nx);
      len += (next_used == cur_used ? -GRID_LEN : GRID_LEN);
    }
  }
  void Output() const {
    int sy = -1, sx = -1;
    REP(i,GRID_SIZE) {
      REP(j,GRID_SIZE) {
        if (used[i][j]) {
          sy = i;
          sx = j;
          break;
        }
      }
      if (sy != -1) break;
    }
    vector<pair<int, int>> res;
    unordered_set<int> visited(N);
    res.emplace_back(sy, sx);
    visited.insert(sy * GRID_SIZE + sx);
    for (;;) {
      // cerr << sy << " " << sx << endl;
      int next_y = -1, next_x = -1;
      REP(dir,4) {
        int ny = sy + dy4[dir];
        int nx = sx + dx4[dir];
        // if (ny < 0 || ny >= GRID_SIZE || nx < 0 || nx >= GRID_SIZE) continue;
        if (visited.count(Hash(ny, nx))) continue;
        if (dir == 0 && is_used(sy, sx) == is_used(sy - 1, sx)) continue;
        if (dir == 1 && is_used(sy, sx) == is_used(sy, sx - 1)) continue;
        if (dir == 2 && is_used(sy - 1, sx - 1) == is_used(sy, sx - 1)) continue;
        if (dir == 3 && is_used(sy - 1, sx - 1) == is_used(sy - 1, sx)) continue;
        next_y = ny;
        next_x = nx;
      }
      if (next_y == -1) break;
      if (res.size() >= 2) {
        auto [y, x] = res[res.size() - 2];
        if (y == next_y || x == next_x) res.pop_back();
      }
      res.emplace_back(next_y, next_x);
      visited.insert(Hash(sy, sx));
      sy = next_y;
      sx = next_x;
    }

    LOG(res.size());
    cout << res.size() << endl;
    for (auto [y, x] : res) {
      cout << linspace[x] << " " << linspace[y] << endl;
    }
  }
  void Print() const {
    // cerr << "score: " << score + 1 << endl;
    // cerr << "len: " << len << endl;
    REP(i,GRID_SIZE) {
      REP(j,GRID_SIZE) {
        if (used[i][j]) cerr << "X";
        else cerr << ".";
      }
      cerr << endl;
    }
  }
};

double start_temp = 20;
double end_temp = 0;

State best_state;

int main() {
  setup();
  vector<State> states(50);
  {
    vector<tuple<int, int, int>> vec;
    REP(i,GRID_SIZE) REP(j,GRID_SIZE) {
      vec.emplace_back(-bucket_score[i][j], i, j);
    }
    std::sort(ALL(vec));
    REP(i,states.size()) {
      int init_y = -1, init_x = -1;
      tie(ignore, init_y, init_x) = vec[i];
      states[i].Change(init_y, init_x);
    }
  }
  vector<int> can_changes;

  int loop_count = 0;
  int update_count = 0;
  bool once = true;
  double temp = start_temp;
  while (timer.within()) {
    if (loop_count % 100 == 0) {
      temp = start_temp + (end_temp - start_temp) * (timer.get_cycle() - timer.start) / (timer.stop - timer.start);
      if (temp > 5) temp = temp * 3 - 10;
      // LOG(temp);
      // std::cerr << "score: " << state.score + 1 << std::endl;
      if (once) {
        double elapsed = (double)(timer.get_cycle() - timer.start) / Timer::CYCLES_PER_SEC;
        if (elapsed > 1.5) {
          for (int i = 1; i < int(states.size()); ++i) {
            if (states[0].score < states[i].score) swap(states[0], states[i]);
          }
          best_state = states[0];
          once = false;
        }
      }
    }
    ++loop_count;
    int i = rnd.next(int(states.size()));
    State& state = states[i];
    int x = rnd.next(GRID_SIZE);
    int y = rnd.next(GRID_SIZE);
    int p = state.Predict(y, x);
    if (p < -1000) continue;
    if (p >= 0 || exp(double(p) / temp) > rnd.next(1e9) / 1e9) {
      state.Change(y, x);
      if (!once && state.score > best_state.score) best_state = state;
    }
    ++update_count;
  }
  best_state.Print();
  best_state.Output();
  LOG(best_state.score);
  double elapsed = (double)(timer.get_cycle() - timer.start) / Timer::CYCLES_PER_SEC;
  LOG(loop_count);
  LOG(update_count);
  LOG(elapsed);
  std::_Exit(0);
  return 0;
}
