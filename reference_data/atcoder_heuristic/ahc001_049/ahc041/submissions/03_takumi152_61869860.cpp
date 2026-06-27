#ifdef __GNUC__
#pragma GCC optimize ("O3")
#pragma GCC optimize ("unroll-loops")
#pragma GCC target ("avx2")
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <array>
#include <algorithm>
#include <utility>
#include <string>
#include <queue>
#include <stack>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <random>
#include <functional>
#include <memory>
#include <cmath>
#include <cassert>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

struct xorshift64 {
  unsigned long long int x = 88172645463325252ULL;
  inline unsigned short nextUShort() {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }
  inline unsigned int nextUShortMod(unsigned long long int mod) {
    x = x ^ (x << 7);
    x = x ^ (x >> 9);
    return ((x & 0x0000ffffffffffff) * mod) >> 48;
  }
  inline unsigned int nextUInt() {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }
  inline unsigned int nextUIntMod(unsigned long long int mod) {
    x = x ^ (x << 7);
    x = x ^ (x >> 9);
    return ((x & 0x00000000ffffffff) * mod) >> 32;
  }
  inline unsigned long long int nextULL() {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }
  inline double nextDouble() {
    x = x ^ (x << 7);
    x = x ^ (x >> 9);
    return (double)x * 5.42101086242752217e-20;
  }
};

struct timer {
  double t = 0.0;
  double lastStop = 0.0;
  bool stopped = false;
  timer() {
    restart();
  }
  inline void restart() {
    t = now();
    stopped = false;
  }
  inline void start() {
    if (stopped) {
      t += now() - lastStop;
      stopped = false;
    }
  }
  inline void stop() {
    if (!stopped) {
      lastStop = now();
      stopped = true;
    }
  }
  inline double time() {
    if (stopped) return lastStop - t;
    else return now() - t;
  }
  inline double now() {
    #ifdef _MSC_VER
      #ifdef LOCAL
        return __rdtsc() * 2.857142857142857e-10; // 1 / 3.5e9, for local (Ryzen 9 3950X)
      #else
        //return __rdtsc() * 3.5714285714285715e-10; // 1 / 2.8e9, for AWS EC2 C3 (Xeon E5-2680 v2)
        //return __rdtsc() * 3.4482758620689656e-10; // 1 / 2.9e9, for AWS EC2 C4 (Xeon E5-2666 v3)
        //return __rdtsc() * 3.333333333333333e-10; // 1 / 3.0e9, for AWS EC2 C5 (Xeon Platinum 8124M / Xeon Platinum 8275CL)
        return __rdtsc() * 3.4482758620689656e-10; // 1 / 2.9e9, for AWS EC2 C6i (Xeon Platinum 8375C)
      #endif
    #else
      unsigned long long l, h;
        __asm__ ("rdtsc" : "=a"(l), "=d"(h));
      #ifdef LOCAL
        return (double)(l | h << 32) * 2.857142857142857e-10; // 1 / 3.5e9, for local (Ryzen 9 3950X)
      #else
        //return (double)(l | h << 32) * 3.5714285714285715e-10; // 1 / 2.8e9, for AWS EC2 C3 (Xeon E5-2680 v2)
        //return (double)(l | h << 32) * 3.4482758620689656e-10; // 1 / 2.9e9, for AWS EC2 C4 (Xeon E5-2666 v3)
        //return (double)(l | h << 32) * 3.333333333333333e-10; // 1 / 3.0e9, for AWS EC2 C5 (Xeon Platinum 8124M / Xeon Platinum 8275CL)
        return (double)(l | h << 32) * 3.4482758620689656e-10; // 1 / 2.9e9, for AWS EC2 C6i (Xeon Platinum 8375C)
      #endif
    #endif
  }
};

using namespace std;

typedef long long int ll;
typedef unsigned long long int ull;
typedef pair<int, int> Pii;
typedef unsigned char uchar;

const ll mod = 998244353;

timer theTimer;
xorshift64 theRandom;
mt19937_64 theMersenne(1);

// libraries
namespace Lib {
}

// hyper parameters
namespace HyperParameter {
  int beam_beam_width = 512;
  double sa_over_height_penalty_factor_begin = 81.928237562532277;
  double sa_over_height_penalty_factor_end = 20811.73935434013;
  double sa_base_temperature = 98.68904431694484;
  double sa_target_temperature = 9.936146699783144;

  void load_hyper_parameter(int argc, char *argv[]) {
    const char* c_beam_beam_width = std::getenv("AHC_BEAM_BEAM_WIDTH");
    if (c_beam_beam_width) beam_beam_width = std::stoi(c_beam_beam_width);

    const char* c_sa_over_height_penalty_factor_begin = std::getenv("AHC_SA_OVER_HEIGHT_PENALTY_FACTOR_BEGIN");
    if (c_sa_over_height_penalty_factor_begin) sa_over_height_penalty_factor_begin = std::stoi(c_sa_over_height_penalty_factor_begin);

    const char* c_sa_over_height_penalty_factor_end = std::getenv("AHC_SA_OVER_HEIGHT_PENALTY_FACTOR_END");
    if (c_sa_over_height_penalty_factor_end) sa_over_height_penalty_factor_end = std::stoi(c_sa_over_height_penalty_factor_end);

    const char* c_sa_base_temperature = std::getenv("AHC_SA_BASE_TEMPERATURE");
    if (c_sa_base_temperature) sa_base_temperature = std::stod(c_sa_base_temperature);

    const char* c_sa_target_temperature = std::getenv("AHC_SA_TARGET_TEMPERATURE");
    if (c_sa_target_temperature) sa_target_temperature = std::stod(c_sa_target_temperature);
  }
}

// enums

// structs
struct Position {
  int x;
  int y;

  Position(): x(0), y(0) {}
  Position(int x, int y): x(x), y(y) {}
};

// constants
struct Consts {
  static constexpr int max_vertex_weight = 100;
};

// inputs
struct Input {
  int vertex_num;
  int edge_num;
  int height_limit;

  vector<int> vertex_weight;

  vector<pair<int, int>> edges;

  vector<Position> vertex_pos;
};

// outputs
struct Output {
  vector<int> parent_ids;
};

// internals
struct Environment {
  int vertex_num;
  int edge_num;
  int height_limit;

  vector<int> vertex_weight;

  vector<vector<int>> graph;
};

Input get_input() {
  Input input;

  cin >> input.vertex_num >> input.edge_num >> input.height_limit;

  input.vertex_weight.resize(input.vertex_num);
  for (int i = 0; i < input.vertex_num; i++) {
    cin >> input.vertex_weight[i];
  }

  input.edges.resize(input.edge_num);
  for (int i = 0; i < input.edge_num; i++) {
    cin >> input.edges[i].first >> input.edges[i].second;
  }

  input.vertex_pos.resize(input.vertex_num);
  for (int i = 0; i < input.vertex_num; i++) {
    cin >> input.vertex_pos[i].x >> input.vertex_pos[i].y;
  }

  return input;
}

Environment create_environment(const Input &input) {
  Environment env;

  env.vertex_num = input.vertex_num;
  env.edge_num = input.edge_num;
  env.height_limit = input.height_limit;

  env.vertex_weight = input.vertex_weight;

  env.graph.resize(env.vertex_num);
  for (auto &e: input.edges) {
    env.graph[e.first].push_back(e.second);
    env.graph[e.second].push_back(e.first);
  }

  return env;
}

Output solve_greedy(const Environment &env) {
  vector<int> parent_ids(env.vertex_num, -1);
  vector<bool> used(env.vertex_num, false);
  while (true) {
    int best_score = 0;
    int best_vertex = -1;
    for (int i = 0; i < env.vertex_num; i++) {
      if (used[i]) continue;
      {
        vector<bool> visited = used;
        queue<pair<int, int>> que; // next_vertex, depth
        que.emplace(i, 0);
        int score = 0;
        while (!que.empty()) {
          auto [v, d] = que.front();
          que.pop();
          if (visited[v]) continue;
          visited[v] = true;
          score += (d + 1) * env.vertex_weight[v];
          if (d == env.height_limit) continue;
          for (auto &nv: env.graph[v]) {
            if (!visited[nv]) {
              que.emplace(nv, d + 1);
            }
          }
        }
        if (score > best_score) {
          best_score = score;
          best_vertex = i;
        }
      }
    }
    if (best_vertex == -1) break;
    {
      queue<tuple<int, int, int>> que; // prev_vertex, next_vertex, depth
      que.emplace(-1, best_vertex, 0);
      while (!que.empty()) {
        auto [pv, v, d] = que.front();
        que.pop();
        if (used[v]) continue;
        used[v] = true;
        if (pv != -1) parent_ids[v] = pv;
        if (d == env.height_limit) continue;
        for (auto &nv: env.graph[v]) {
          if (!used[nv]) {
            que.emplace(v, nv, d + 1);
          }
        }
      }
    }
  }

  Output output;
  output.parent_ids = parent_ids;

  return output;
}

Output solve_beam(const Environment &env) {
  struct BeamState {
    int id;
    int prev_id;
    int score;
    int current_vertex;
    int vertex_weight_sum;
    int depth;

    BeamState(int id, int prev_id, int score, int current_vertex, int vertex_weight_sum, int depth): id(id), prev_id(prev_id), score(score), current_vertex(current_vertex), vertex_weight_sum(vertex_weight_sum), depth(depth) {}
  };

  vector<int> parent_ids(env.vertex_num, -1);
  vector<bool> used(env.vertex_num, false);
  vector<int> vertex_penalty(env.vertex_num, 0);
  vector<int> vertex_height(env.vertex_num, 0);
  
  vector<int> vertex_check_order(env.vertex_num);
  iota(vertex_check_order.begin(), vertex_check_order.end(), 0);
  sort(vertex_check_order.begin(), vertex_check_order.end(), [&](int a, int b) {
    return env.vertex_weight[a] > env.vertex_weight[b];
  });

  const int beam_width = HyperParameter::beam_beam_width;
  for (const auto base_vertex: vertex_check_order) {
    if (used[base_vertex]) continue;
    vector<BeamState> history;
    vector<BeamState> current_state_list;
    vector<BeamState> completed_state_list;

    auto init_state = BeamState(history.size(), -1, 0, base_vertex, env.vertex_weight[base_vertex], 0);
    history.push_back(init_state);
    current_state_list.push_back(init_state);

    while (!current_state_list.empty()) {
      vector<BeamState> next_state_list;
      sort(current_state_list.begin(), current_state_list.end(), [&](const BeamState &a, const BeamState &b) {
        return a.score < b.score;
      });
      for (int i = 0; i < min(beam_width, (int) current_state_list.size()); i++) {
        const auto &state = current_state_list[i];
        if (state.depth == env.height_limit) {
          completed_state_list.push_back(state);
          continue;
        }
        vector<int> local_used;
        {
          int state_id = state.prev_id;
          while (state_id != -1) {
            const auto &prev_state = history[state_id];
            local_used.push_back(prev_state.current_vertex);
            state_id = prev_state.prev_id;
          }
        }
        for (auto &nv: env.graph[state.current_vertex]) {
          if (find(local_used.begin(), local_used.end(), nv) != local_used.end()) continue;
          if (used[nv]) {
            int base_height = state.depth + vertex_height[nv] + 1;
            if (base_height > env.height_limit) continue;
            auto final_state = BeamState(-1, state.id, state.score + Consts::max_vertex_weight * (env.height_limit - base_height) * (env.height_limit - base_height + 1) / 2 + state.vertex_weight_sum * (env.height_limit - base_height) + vertex_penalty[nv], nv, state.vertex_weight_sum, env.height_limit - vertex_height[nv]);
            completed_state_list.push_back(final_state);
            continue;
          }
          auto next_state = BeamState(history.size(), state.id, state.score + (state.depth + 1) * env.vertex_weight[nv], nv, state.vertex_weight_sum + env.vertex_weight[nv], state.depth + 1);
          history.push_back(next_state);
          next_state_list.push_back(next_state);
        }
        {
          auto final_state = BeamState(-1, state.prev_id, state.score + Consts::max_vertex_weight * (env.height_limit - state.depth) * (env.height_limit - state.depth + 1) / 2 + state.vertex_weight_sum * (env.height_limit - state.depth), state.current_vertex, state.vertex_weight_sum, state.depth);
          completed_state_list.push_back(final_state);
        }
      }
      current_state_list = move(next_state_list);
    }

    sort(completed_state_list.begin(), completed_state_list.end(), [&](const BeamState &a, const BeamState &b) {
      return a.score < b.score;
    });
    const auto &best_state = completed_state_list[0];
    {
      auto state = best_state;
      int current_vertex = best_state.current_vertex;
      int current_height = env.height_limit - best_state.depth;
      int current_penalty = used[current_vertex] ? env.vertex_weight[current_vertex] * (env.height_limit - current_height) : vertex_penalty[current_vertex];
      while (true) {
        vertex_height[current_vertex] = current_height;
        used[current_vertex] = true;
        vertex_penalty[current_vertex] = current_penalty;
        if (state.prev_id == -1) break;
        const auto &prev_state = history[state.prev_id];
        parent_ids[prev_state.current_vertex] = current_vertex;
        current_vertex = prev_state.current_vertex;
        current_height++;
        current_penalty += env.vertex_weight[current_vertex] * (env.height_limit - current_height);
        state = prev_state;
      }
    }
  }

  Output output;
  output.parent_ids = parent_ids;

  return output;
}

Output solve_sa(const Environment &env, const Output &init_ans) {
  struct SAState {
    struct Score {
      int score;
      int over_height_penalty;

      Score(): score(0), over_height_penalty(0) {}
      Score(int score, int over_height_penalty): score(score), over_height_penalty(over_height_penalty) {}

      double get_score(double progress) const {
        const double penalty_factor = exp(log(HyperParameter::sa_over_height_penalty_factor_begin) - ((log(HyperParameter::sa_over_height_penalty_factor_begin) - log(HyperParameter::sa_over_height_penalty_factor_end)) * progress));
        return (double) score - penalty_factor * (double) over_height_penalty;
      }
    };

    const Environment &env;

    vector<int> parent_ids;

    vector<vector<int>> child_ids;

    vector<int> vertex_height;
    Score score;

    SAState(const Environment &env, const Output& init_ans): env(env) {

      parent_ids = init_ans.parent_ids;

      child_ids.resize(env.vertex_num);
      for (int i = 0; i < env.vertex_num; i++) {
        if (parent_ids[i] != -1) {
          child_ids[parent_ids[i]].push_back(i);
        }
      }

      vertex_height.assign(env.vertex_num, 0);
      score = Score(0, 0);

      update_score_full();
    }

    void update_score_full() {
      vector<bool> visited(env.vertex_num, false);
      for (int i = 0; i < env.vertex_num; i++) {
        if (visited[i]) continue;
        stack<int> st;
        int current_vertex = i;
        int base_height = -1;
        while (true) {
          if (visited[current_vertex]) {
            base_height = vertex_height[current_vertex];
            break;
          }
          visited[current_vertex] = true;
          st.push(current_vertex);
          if (parent_ids[current_vertex] == -1) break;
          current_vertex = parent_ids[current_vertex];
        }
        int next_height = base_height + 1;
        while (!st.empty()) {
          current_vertex = st.top();
          st.pop();
          vertex_height[current_vertex] = next_height;
          next_height++;
        }
      }

      int score = 0;
      int over_height_penalty = 0;
      for (int i = 0; i < env.vertex_num; i++) {
        score += (vertex_height[i] + 1) * env.vertex_weight[i];
        if (vertex_height[i] > env.height_limit) {
          over_height_penalty += vertex_height[i] - env.height_limit;
        }
      }

      this->score = Score(score, over_height_penalty);
    }

    void update_score_partial(const int base_vertex) {
      int base_height;
      if (parent_ids[base_vertex] == -1) {
        base_height = 0;
      }
      else {
        base_height = vertex_height[parent_ids[base_vertex]] + 1;
      }

      stack<pair<int, int>> st; // vertex, height
      st.emplace(base_vertex, base_height);
      while (!st.empty()) {
        auto [current_vertex, current_height] = st.top();
        st.pop();
        score.score -= (vertex_height[current_vertex] + 1) * env.vertex_weight[current_vertex];
        if (vertex_height[current_vertex] > env.height_limit) {
          score.over_height_penalty -= vertex_height[current_vertex] - env.height_limit;
        }
        vertex_height[current_vertex] = current_height;
        score.score += (vertex_height[current_vertex] + 1) * env.vertex_weight[current_vertex];
        if (vertex_height[current_vertex] > env.height_limit) {
          score.over_height_penalty += vertex_height[current_vertex] - env.height_limit;
        }
        for (auto &child: child_ids[current_vertex]) {
          st.emplace(child, current_height + 1);
        }
      }
    }

    void update_child_ids(const int base_vertex, const int orig_parent) {
      if (orig_parent != -1) {
        auto it = find(child_ids[orig_parent].begin(), child_ids[orig_parent].end(), base_vertex);
        if (it != child_ids[orig_parent].end()) {
          child_ids[orig_parent].erase(it);
        }
      }
      if (parent_ids[base_vertex] != -1) {
        child_ids[parent_ids[base_vertex]].push_back(base_vertex);
      }
    }

    int get_score(double progress) {
      return score.get_score(progress);
    }

    Output get_output() {
      Output output;
      output.parent_ids = parent_ids;
      return output;
    }
  };

  SAState state(env, init_ans);
  {
    auto score = state.score;
    auto last_score = score;
    auto best_score = score;

    const double base_temperature = HyperParameter::sa_base_temperature;
    const double target_temperature = HyperParameter::sa_target_temperature;
    // const double decay_rate = 4e-5;
    double temperature = base_temperature;

    int iter_count = 0;
    double time_start = theTimer.time();
    double current_time = time_start;
    const double time_limit = 1.980;

    double progress = 0.0;

    while ((current_time = theTimer.time()) < time_limit) {
      progress = (current_time - time_start) / (time_limit - time_start);
      double roll = theRandom.nextDouble();
      if (roll < 1.00) {
        int v = theRandom.nextUIntMod(env.vertex_num);
        int p = theRandom.nextUIntMod(env.graph[v].size() + 1);

        int orig_parent = state.parent_ids[v];

        if (p == (int) env.graph[v].size()) {
          state.parent_ids[v] = -1;
        }
        else {
          state.parent_ids[v] = env.graph[v][p];
        }

        // loop check
        if (state.parent_ids[v] != -1) {
          int current_vertex = v;
          while (true) {
            if (state.parent_ids[current_vertex] == -1) break;
            if (state.parent_ids[current_vertex] == v) break;
            current_vertex = state.parent_ids[current_vertex];
          }
          if (state.parent_ids[current_vertex] == v) {
            state.parent_ids[v] = orig_parent;
            continue;
          }
        }

        state.update_score_partial(v);

        score = state.score;

        #ifdef DEBUG
        if (iter_count % 1000 == 0) cerr << iter_count << " " << score.get_score(progress) << " " << last_score.get_score(progress) << " " << best_score.get_score(progress) << " " << temperature << " " << theTimer.time() << endl;
        #endif

        if (score.get_score(progress) >= last_score.get_score(progress)) {
          state.update_child_ids(v, orig_parent);

          last_score = score;
          if (score.get_score(progress) > best_score.get_score(progress)) {
            best_score = score;
          }
        }
        else if (theRandom.nextDouble() < exp(double(score.get_score(progress) - last_score.get_score(progress)) / temperature)) { // accept
          state.update_child_ids(v, orig_parent);

          last_score = score;
        }
        else { // rollback
          state.parent_ids[v] = orig_parent;
          state.update_score_partial(v);

          score = last_score;
        }
      }

      // temperature *= 1.0 - decay_rate;
      temperature = exp(log(base_temperature) - ((log(base_temperature) - log(target_temperature)) * progress));
      iter_count++;
    }

    #ifdef DEBUG
    cerr << "iter_count   = " << iter_count << endl;
    cerr << "score        = " << score.get_score(progress) << endl;
    cerr << "best_score   = " << best_score.get_score(progress) << endl;
    cerr << "temperature  = " << temperature << endl;
    #endif
  }

  Output output = state.get_output();

  return output;
}

Output solve(const Environment &env) {
  // auto output1 = solve_greedy(env);
  auto output1 = solve_beam(env);
  auto output2 = solve_sa(env, output1);
  return output2;
}

void print_output(const Output &output) {
  for (auto &p: output.parent_ids) cout << p << " ";
  cout << endl;
}

int main(int argc, char *argv[]) {
  cin.tie(0);
  ios::sync_with_stdio(false);

  HyperParameter::load_hyper_parameter(argc, argv);

  const auto input = get_input();
  const auto env = create_environment(input);
  const auto output = solve(env);
  print_output(output);

  return 0;
}
