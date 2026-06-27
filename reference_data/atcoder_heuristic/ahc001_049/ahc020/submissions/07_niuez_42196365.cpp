#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <bitset>
#include <queue>
#include <fstream>
#include <random>
#include <cassert>

struct Timer {
  std::chrono::high_resolution_clock::time_point st;
  Timer() { st = now(); }
  std::chrono::high_resolution_clock::time_point now() { return std::chrono::high_resolution_clock::now(); }
  std::chrono::milliseconds::rep span() {
    auto ed = now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(ed - st).count();
  }
};

struct Xor64 {
  using state_type = std::uint64_t;
  using result_type = std::uint64_t;
  state_type a;
  static constexpr result_type min() {
    return std::numeric_limits<result_type>::min();
  }
  static constexpr result_type max() {
    return std::numeric_limits<result_type>::max();
  }
  constexpr Xor64(state_type seed = 88675123): a(seed) {}
  constexpr void seed(state_type seed = 88675123) {
    a = seed;
  }
  constexpr result_type operator()() {
    a ^= a << 7;
    a ^= a >> 9;
    return a;
  }
  void discard(unsigned long long z) {
    while(z --> 0) {
      (*this)();
    }
  }
};


template<class T>
struct discrate_probability_distribution {
  std::vector<T> acc;
  std::uniform_real_distribution<double> dist;
  discrate_probability_distribution(const std::vector<T>& prob) {
    acc.resize(prob.size(), 0);
    T now = 0;
    for(int i = 0; i < prob.size(); i++) {
      now += prob[i];
      acc[i] = now;
    }
    dist = std::uniform_real_distribution<double>(0, acc.back());
  }

  template<class URGB>
    int operator()(URGB& g) {
      int v = dist(g);
      return std::lower_bound(acc.begin(), acc.end(), v) - acc.begin();
    }
};

using i64 = long long;

const int MAX_N = 100;
const int MAX_M = 300;
const int MAX_P = 5000;

struct Point {
  int x;
  int y;
};


using Wgh = int;

struct Edge {
  int u;
  int v;
  Wgh w;
};

struct Input {
  int N, M, K;
  std::vector<Point> V;
  std::vector<Edge> es;
  std::vector<Point> C;


  void from_input(std::istream& is) {
    is >> N >> M >> K;
    V.resize(N);
    for(int i = 0; i < N; i++) {
      is >> V[i].x >> V[i].y;
    }
    for(int i = 0; i < M; i++) {
      int u, v;
      Wgh w;
      is >> u >> v >> w;
      u--;
      v--;
      es.push_back( { u, v, w } );
    }
    C.resize(K);
    for(int i = 0; i < K; i++) {
      is >> C[i].x >> C[i].y;
    }
  }

  std::vector<std::vector<std::tuple<int, Wgh, int>>> G;
  std::vector<std::vector<int>> near_c;
  std::vector<std::vector<int>> near_c_dist;
  std::vector<std::vector<int>> c_idx;

  void build() {
    G.resize(N);
    for(int i = 0; i < M; i++) {
      auto& e = es[i];
      G[e.u].push_back({ e.v, e.w, i });
      G[e.v].push_back({ e.u, e.w, i });
    }
    for(int i = 0; i < N; i++) {
      std::vector<int> idx(K);
      std::vector<int> c_dist(K + 1);
      std::vector<int> c_i(K);
      std::iota(idx.begin(), idx.end(), 0);
      auto dist = [&](int x) {
        i64 dx = C[x].x - V[i].x;
        i64 dy = C[x].y - V[i].y;
        return dx * dx + dy * dy;
      };
      std::sort(idx.begin(), idx.end(), 
          [&](int x, int y) {
            return dist(x) < dist(y);
          }
      );
      for(int i = 0; i < idx.size(); i++) {
        c_i[idx[i]] = i;
        c_dist[i + 1] = std::ceil(std::sqrt(dist(idx[i])));
      }
      near_c.push_back(std::move(idx));
      near_c_dist.push_back(std::move(c_dist));
      c_idx.push_back(std::move(c_i));
    }
  }

  i64 norm(i64 v, i64 i) const {
    i64 dx = V[v].x - C[i].x;
    i64 dy = V[v].y - C[i].y;
    return dx * dx + dy * dy;
  }
};

struct Output {
  std::vector<int> P;
  std::bitset<MAX_M> B;
  Output(const Input& in) {
    P.resize(in.N);
  }
  void to_output(const Input& in, std::ostream& os) const {
    for(int i = 0; i < in.N; i++) {
      os << P[i] << " \n"[i + 1 == P.size()];
    }
    for(int i = 0; i < in.M; i++) {
      os << (B[i] ? 1 : 0) << " \n"[i + 1 == B.size()];
    }
  }
};

struct Replica {
  std::vector<int> p_idx;
  std::bitset<MAX_N> merged_flag;
  std::vector<int> merged;
  std::bitset<MAX_M> use_edge;
  i64 score = 0;

  void init(const Input& in) {
    p_idx.resize(in.N);
    for(int i = 0; i < in.K; i++) {
      auto dist = [&](const Point& v) {
        i64 dx = v.x - in.C[i].x;
        i64 dy = v.y - in.C[i].y;
        return dx * dx + dy * dy;
      };
      int nearest = std::min_element(in.V.begin(), in.V.end(),
          [&](const Point& x, const Point& y) { return dist(x) < dist(y); }
        ) - in.V.begin();
      p_idx[nearest] = std::max(p_idx[nearest], in.c_idx[nearest][i] + 1);
    }
    score = recalc_steiner(in);
  }

  i64 recalc_steiner(const Input& in) {
    merged_flag.reset();
    merged_flag.set(0);
    merged = { 0 };
    use_edge.reset();

    i64 w_sum = 0;

    std::bitset<MAX_N> yet;
    int yet_cnt = 0;
    for(int i = 1; i < in.N; i++) {
      if(p_idx[i] > 0) {
        yet.set(i);
        yet_cnt++;
      }
    }
    while(yet_cnt) {
      using Pii = std::pair<i64, int>;
      std::priority_queue<Pii, std::vector<Pii>, std::greater<Pii>> que;
      std::vector<i64> before(in.N, -1);
      std::vector<i64> bei(in.N, -1);
      std::vector<i64> dist(in.N, 1e18);
      for(auto s: merged) {
        dist[s] = 0;
        que.push({ dist[s], s });
      }
      int now;
      while(!que.empty()) {
        auto [d, v] = que.top();
        que.pop();
        if(dist[v] < d) continue;
        if(yet[v]) {
          now = v;
          break;
        }
        for(auto [u, w, ei]: in.G[v]) {
          if(d + w < dist[u]) {
            dist[u] = d + w;
            before[u] = v;
            bei[u] = ei;
            que.push({ dist[u], u });
          }
        }
      }
      yet.reset(now);
      yet_cnt--;
      while(before[now] != -1) {
        w_sum += in.es[bei[now]].w;
        use_edge.set(bei[now]);
        if(!merged_flag.test(now)) {
          merged_flag.set(now);
          merged.push_back(now);
        }
        now = before[now];
      }
    }

    i64 p_sum = 0;
    for(int i = 0; i < in.N; i++) {
      i64 p = in.near_c_dist[i][p_idx[i]];
      p_sum += p * p;
    }
    return p_sum + w_sum;
  }

  i64 p_score(const Input& in, i64 i) const {
    i64 p = in.near_c_dist[i][p_idx[i]];
    return p * p;
  }

  Output output(const Input& in) const {
    Output out(in);
    for(int i = 0; i < in.N; i++) {
      out.P[i] = in.near_c_dist[i][p_idx[i]];
    }
    out.B = use_edge;
    return out;
  }

  using URGB = Xor64;

  void metropolis(const Input& in, const double TEMP, URGB& g) {
    int query = g() % 2;
    // delete
    if(query == 0) {
      std::vector<int> cand;
      for(int v = 0; v < in.N; v++) {
        if(p_idx[v] > 0) {
          cand.push_back(v);
        }
      }
      if(cand.empty()) return;
      auto v = cand[g() % cand.size()];
      Replica before = *this;
      i64 next_p_idx = 0;
      i64 min_delta = 1e18;
      i64 now_delta = 0;
      for(int i = p_idx[v]; i --> 0;) {
        int x = in.near_c[v][i];
        int mi = -1;
        i64 d = 1e18;
        for(int u = 0; u < in.N; u++) {
          if(v == u || p_idx[u] == 0 || in.near_c_dist[u][in.c_idx[u][x]] > MAX_P) continue;
          if(in.c_idx[u][x] < p_idx[u]) {
            mi = -2;
            break;
          }
          i64 a = in.near_c_dist[u][p_idx[u]];
          i64 b = in.near_c_dist[u][in.c_idx[u][x] + 1];
          i64 n = b * b - a * a;
          if(d > n) {
            d = n;
            mi = u;
          }
        }
        if(mi == -1) {
          *this = before;
          return;
        }
        if(mi != -2) {
          now_delta -= p_score(in, mi);
          p_idx[mi] = std::max(p_idx[mi], in.c_idx[mi][x] + 1);
          now_delta += p_score(in, mi);
        }
        now_delta -= p_score(in, v);
        p_idx[v] = i;
        now_delta += p_score(in, v);
        if(i != 0 && now_delta < min_delta) {
          min_delta = now_delta;
          next_p_idx = i;
        }
      }
      i64 next_zero_score = recalc_steiner(in);
      double delta = next_zero_score - before.score;
      if(delta <= min_delta) {
        if(delta <= 0 || std::bernoulli_distribution(std::exp(-delta / TEMP))(g)) {
          score = next_zero_score;
        }
        else {
          *this = before;
        }
      }
      else {
        delta = min_delta;
        if(delta <= 0 || std::bernoulli_distribution(std::exp(-delta / TEMP))(g)) {
          *this = before;
          for(int i = p_idx[v]; i --> next_p_idx;) {
            int x = in.near_c[v][i];
            int mi = -1;
            i64 d = 1e18;
            for(int u = 0; u < in.N; u++) {
              if(v == u || p_idx[u] == 0 || in.near_c_dist[u][in.c_idx[u][x]] > MAX_P) continue;
              if(in.c_idx[u][x] < p_idx[u]) {
                mi = -1;
                break;
              }
              i64 a = in.near_c_dist[u][p_idx[u]];
              i64 b = in.near_c_dist[u][in.c_idx[u][x] + 1];
              i64 n = b * b - a * a;
              if(d > n) {
                d = n;
                mi = u;
              }
            }
            if(mi != -1) {
              p_idx[mi] = std::max(p_idx[mi], in.c_idx[mi][x] + 1);
            }
          }
          p_idx[v] = next_p_idx;
          double before_score = before.score;
          score = before_score + min_delta;
          //score = recalc_steiner(in);
          //std::cerr << v << " " << next_p_idx << std::endl;
          //assert(score - before_score == min_delta);
        }
        else {
          *this = before;
        }
      }
    }
    else {
      std::vector<int> cand;
      for(int v = 0; v < in.N; v++) {
        if(p_idx[v] > 0) {
          cand.push_back(v);
        }
      }
      if(cand.empty()) return;
      auto v = cand[g() % cand.size()];
      Replica before = *this;
      int next_p_idx = g() % p_idx[v];
      std::vector<int> xs(in.near_c[v].begin() + next_p_idx, in.near_c[v].begin() + p_idx[v]);
      std::shuffle(xs.begin(), xs.end(), g);
      i64 now_delta = 0;
      //for(int i = next_p_idx; i < p_idx[v]; i++) {
        //int x = in.near_c[v][i];
      for(auto x: xs) {
        int mi = -1;
        i64 d = 1e18;
        for(int u = 0; u < in.N; u++) {
          if(v == u || p_idx[u] == 0 || in.near_c_dist[u][in.c_idx[u][x]] > MAX_P) continue;
          if(in.c_idx[u][x] < p_idx[u]) {
            mi = -2;
            break;
          }
          i64 a = in.near_c_dist[u][p_idx[u]];
          i64 b = in.near_c_dist[u][in.c_idx[u][x] + 1];
          i64 n = b * b - a * a;
          if(d > n) {
            d = n;
            mi = u;
          }
        }
        if(mi == -1) {
          *this = before;
          return;
        }
        if(mi != -2) {
          now_delta -= p_score(in, mi);
          p_idx[mi] = std::max(p_idx[mi], in.c_idx[mi][x] + 1);
          now_delta += p_score(in, mi);
        }
      }
      now_delta -= p_score(in, v);
      p_idx[v] = next_p_idx;
      now_delta += p_score(in, v);
      i64 next_score;
      if(next_p_idx == 0) {
        next_score = recalc_steiner(in);
      }
      else {
        next_score = before.score + now_delta;
      }
      double delta = before.score - next_score;
      if(delta >= 0 || std::bernoulli_distribution(std::exp(delta / TEMP))(g)) {
        score = next_score;
      }
      else {
        *this = before;
      }
    }
  }
};

const int TL = 1980;

i64 ofv(double x) {
  return std::round(1e6 * (1 + 1e8 / (x + 1e7)));
}

void multi_time(const Input& in) {
  for(int q = 0; q < 10; q++) {
    Timer timer;
    Replica replica;
    replica.init(in);
    Xor64 mt(786);
    int iter = 0;
    while(timer.span() < TL) {
      iter++;
      replica.metropolis(in, 0, mt);
      if(iter % 100 == 0) {
      }
    }
    std::cerr << ofv(replica.score) << std::endl;
  }
}

int main() {
  Timer timer;
  Input in;
  in.from_input(std::cin);
  in.build();
  Replica replica;
  replica.init(in);
  Xor64 mt(768);
  int iter = 0;
  const double T0 = 300000;
  const double T1 = 200000;
  double time = 0;
  Output out = replica.output(in);
  i64 score = replica.score;
  while(time = timer.span(), time < TL) {
    iter++;
    replica.metropolis(in, T0 + (T1 - T0) * (time / TL), mt);
    if(score > replica.score) {
      score = replica.score;
      out = replica.output(in);
    }
#ifndef ONLINE_JUDGE
    if(iter % 100 == 0) {
      std::cerr << ofv(replica.score) << std::endl;
    }
#endif
  }

  out.to_output(in, std::cout);
}
