#include <bits/stdc++.h>
using namespace std;

constexpr int R = 10;
constexpr int LAST_DIM = R + 1;
constexpr int CARS_PER_LINE = 10;
constexpr int TOTAL_CARS = R * CARS_PER_LINE;
constexpr int MAIN_CAP = 15;
constexpr int SIDING_CAP = 20;
constexpr int TMAX = 4000;

constexpr int A_SCORE = 1000;
constexpr int B_SCORE = 100;
constexpr int C_SCORE = 900;
constexpr int LINE_DISTANCE_WEIGHT = 1;
constexpr array<int, 4> EDGE_STRUCTURE_SCORE = {0, B_SCORE, C_SCORE, A_SCORE};
constexpr array<int, 4> EDGE_READY_SCORE = {0, 0, 1, 0};
constexpr array<int, 4> EDGE_OPEN_SCORE = {0, 1, 0, 0};

constexpr int BEAM_WIDTH = 150;
constexpr int TIME_LIMIT_BEAM_WIDTH = 15;
constexpr int PAIR_BUCKET_WIDTH = 5;
constexpr int TIME_LIMIT_PAIR_BUCKET_WIDTH = 1;
constexpr int SAME_TURN_TOP_K = 3;
constexpr int NEW_TURN_TOP_K = 2;
constexpr long long SHRINK_BEAM_AFTER_MS = 1800;
constexpr int MAX_STEPS = 500;
constexpr int MACRO_BUCKET_LIMIT = 4;
constexpr int RESCHEDULE_BEAM_WIDTH = 1200;
constexpr int RESCHEDULE_SUBSET_LIMIT = 96;

struct Move {
  int type;
  int i;
  int j;
  int k;
};

struct Track {
  array<int, SIDING_CAP> car{};
  int len = 0;
};

struct Key {
  int structure = 0;
  int line_proximity = 0;
  int ready_edges = 0;
  int open_edges = 0;

  auto quality_tuple() const {
    return tuple<int, int, int, int>(
        structure,
        line_proximity,
        ready_edges,
        open_edges);
  }
};

struct State {
  array<Track, R> dep{};
  array<Track, R> sid{};
  array<array<unsigned long long, R>, 2> track_hash{};
  array<array<int, R>, 2> head{};
  array<array<int, R>, 2> tail{};
  array<int, TOTAL_CARS> prev{};
  array<int, TOTAL_CARS> next{};
  array<int, TOTAL_CARS> kind{};
  array<int, TOTAL_CARS> line{};
  array<int, TOTAL_CARS> pos{};
  array<unsigned char, TOTAL_CARS> macro_edge_state{};
  Key key;
  unsigned long long hash = 0;
  unsigned long long macro_hash = 0;
  int turn_count = 0;
  int last_i = -1;
  int last_j = -1;
};

struct Node {
  int parent = -1;
  Move move{-1, -1, -1, -1};
  Key key;
  unsigned long long hash = 0;
  unsigned long long macro_hash = 0;
  int turn_count = 0;
  int last_i = -1;
  int last_j = -1;
  int score = 0;
};

struct BeamState {
  State state;
  int node_id = 0;
};

struct Candidate {
  Key key;
  int parent_bi = 0;
  int parent_bj = 0;
  int parent_beam = -1;
  int parent_node = -1;
  Move move{};
  unsigned long long hash = 0;
  unsigned long long macro_hash = 0;
  int turn_count = 0;
  int last_i = -1;
  int last_j = -1;
  int score = 0;
  bool non_decreasing = false;
  bool same_turn = false;
  unsigned long long primary_rank = 0;
  unsigned long long tie_rank = 0;
};

#ifdef PERF_STATS
struct PerfStats {
  long long expanded_states = 0;
  long long tried_moves = 0;
  long long tried_same_turn = 0;
  long long tried_new_turn = 0;
  long long eval_calls = 0;
  long long seen_eval_hit = 0;
  long long selector_push_calls = 0;
  long long selector_dup_replace = 0;
  long long selector_dup_reject = 0;
  long long selector_macro_replace = 0;
  long long selector_macro_reject = 0;
  long long selector_fill = 0;
  long long selector_full_replace = 0;
  long long selector_full_reject = 0;
  long long sort_calls = 0;
  long long sorted_items = 0;
  long long materialize_attempts = 0;
  long long materialize_seen_reject = 0;
  long long materialized = 0;
  long long apply_calls = 0;
  long long undo_calls = 0;

  long long ns_layer = 0;
  long long ns_expand_states = 0;
  long long ns_apply_eval = 0;
  long long ns_apply_board = 0;
  long long ns_apply_collect = 0;
  long long ns_apply_remove_score = 0;
  long long ns_apply_raw_move = 0;
  long long ns_apply_refresh = 0;
  long long ns_apply_add_score = 0;
  long long ns_undo = 0;
  long long ns_selector_push = 0;
  long long ns_sort = 0;
  long long ns_materialize = 0;
  long long ns_materialize_copy = 0;

  static long long now_ns() {
    return chrono::duration_cast<chrono::nanoseconds>(
               chrono::steady_clock::now().time_since_epoch())
        .count();
  }

  static double ms(long long ns) {
    return (double)ns / 1000000.0;
  }

  void reset() {
    *this = PerfStats{};
  }
};

PerfStats perf;

struct PerfTimer {
  long long &dst;
  long long start;

  explicit PerfTimer(long long &dst) : dst(dst), start(PerfStats::now_ns()) {}

  ~PerfTimer() {
    dst += PerfStats::now_ns() - start;
  }
};
#endif

bool better_candidate(const Candidate &lhs, const Candidate &rhs);

struct Selector {
  vector<Candidate> best;
  vector<pair<unsigned long long, int>> macro_counts;
  int width = BEAM_WIDTH;

  Selector() {
    best.reserve(BEAM_WIDTH);
    macro_counts.reserve(BEAM_WIDTH);
  }

  explicit Selector(int width) {
    this->width = width;
    best.reserve(width);
    macro_counts.reserve(width);
  }

  void reset(int next_width) {
    width = next_width;
    best.clear();
    macro_counts.clear();
  }

  int macro_count(unsigned long long macro_hash) const {
    for (const auto &[h, cnt] : macro_counts) {
      if (h == macro_hash) return cnt;
    }
    return 0;
  }

  void add_macro(unsigned long long macro_hash, int delta) {
    for (auto &[h, cnt] : macro_counts) {
      if (h == macro_hash) {
        cnt += delta;
        return;
      }
    }
    macro_counts.push_back({macro_hash, delta});
  }

  int worst_index() const {
    int worst = 0;
    for (int i = 1; i < (int)best.size(); i++) {
      if (better_candidate(best[worst], best[i])) worst = i;
    }
    return worst;
  }

  void erase_candidate(int idx) {
    add_macro(best[idx].macro_hash, -1);
    best[idx] = best.back();
    best.pop_back();
  }

  void set_width(int next_width) {
    if (next_width >= width) return;
    width = next_width;
    while ((int)best.size() > width) {
      erase_candidate(worst_index());
    }
  }

  void push(const Candidate &cand) {
#ifdef PERF_STATS
    PerfTimer timer(perf.ns_selector_push);
    perf.selector_push_calls++;
#endif
    for (int idx = 0; idx < (int)best.size(); idx++) {
      Candidate &cur = best[idx];
      if (cur.hash == cand.hash) {
        if (better_candidate(cand, cur)) {
          add_macro(cur.macro_hash, -1);
          cur = cand;
          add_macro(cur.macro_hash, 1);
#ifdef PERF_STATS
          perf.selector_dup_replace++;
#endif
        } else {
#ifdef PERF_STATS
          perf.selector_dup_reject++;
#endif
        }
        return;
      }
    }

    if (macro_count(cand.macro_hash) >= MACRO_BUCKET_LIMIT) {
      int worst_same = -1;
      for (int i = 0; i < (int)best.size(); i++) {
        if (best[i].macro_hash != cand.macro_hash) continue;
        if (worst_same == -1 || better_candidate(best[worst_same], best[i])) {
          worst_same = i;
        }
      }
      if (worst_same != -1 && better_candidate(cand, best[worst_same])) {
        best[worst_same] = cand;
#ifdef PERF_STATS
        perf.selector_macro_replace++;
#endif
      } else {
#ifdef PERF_STATS
        perf.selector_macro_reject++;
#endif
      }
      return;
    }

    if ((int)best.size() < width) {
      best.push_back(cand);
      add_macro(cand.macro_hash, 1);
#ifdef PERF_STATS
      perf.selector_fill++;
#endif
      return;
    }

    int worst = worst_index();
    if (better_candidate(cand, best[worst])) {
      add_macro(best[worst].macro_hash, -1);
      best[worst] = cand;
      add_macro(cand.macro_hash, 1);
#ifdef PERF_STATS
      perf.selector_full_replace++;
#endif
    } else {
#ifdef PERF_STATS
      perf.selector_full_reject++;
#endif
    }
  }

  vector<Candidate> sorted() {
#ifdef PERF_STATS
    PerfTimer timer(perf.ns_sort);
    perf.sort_calls++;
    perf.sorted_items += best.size();
#endif
    sort(best.begin(), best.end(), better_candidate);
    return best;
  }
};

using BeamBuckets = array<array<vector<BeamState>, LAST_DIM>, LAST_DIM>;
using SelectorBuckets = array<array<Selector, LAST_DIM>, LAST_DIM>;

void reset_selector_buckets(SelectorBuckets &buckets, int width) {
  for (int i = 0; i < LAST_DIM; i++) {
    for (int j = 0; j < LAST_DIM; j++) {
      buckets[i][j].reset(width);
    }
  }
}

void set_selector_buckets_width(SelectorBuckets &buckets, int width) {
  for (int i = 0; i < LAST_DIM; i++) {
    for (int j = 0; j < LAST_DIM; j++) {
      buckets[i][j].set_width(width);
    }
  }
}

bool has_bucket_states(const BeamBuckets &buckets) {
  for (int i = 0; i < LAST_DIM; i++) {
    for (int j = 0; j < LAST_DIM; j++) {
      if (!buckets[i][j].empty()) return true;
    }
  }
  return false;
}

int count_bucket_states(const BeamBuckets &buckets) {
  int cnt = 0;
  for (int i = 0; i < LAST_DIM; i++) {
    for (int j = 0; j < LAST_DIM; j++) {
      cnt += (int)buckets[i][j].size();
    }
  }
  return cnt;
}

unsigned long long zobrist[2][R][SIDING_CAP][TOTAL_CARS];
unsigned long long macro_zobrist[TOTAL_CARS][4];
unsigned long long head_zobrist[2][R][TOTAL_CARS + 1];
unsigned long long link_zobrist[TOTAL_CARS][TOTAL_CARS];
int line_proximity_table[R][R];

unsigned long long splitmix64(unsigned long long x) {
  x += 0x9e3779b97f4a7c15ULL;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
  x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
  return x ^ (x >> 31);
}

void init_zobrist() {
  for (int row = 0; row < R; row++) {
    for (int line = 0; line < R; line++) {
      line_proximity_table[row][line] =
          LINE_DISTANCE_WEIGHT * (R - 1 - abs(line - row));
    }
  }

  for (int k = 0; k < 2; k++) {
    for (int l = 0; l < R; l++) {
      for (int x = 0; x <= TOTAL_CARS; x++) {
        unsigned long long seed = 0x9e3779b97f4a7c15ULL;
        seed = seed * 131 + k;
        seed = seed * 131 + l;
        seed = seed * 131 + x;
        head_zobrist[k][l][x] = splitmix64(seed);
      }
    }
  }
  for (int x = 0; x < TOTAL_CARS; x++) {
    for (int y = 0; y < TOTAL_CARS; y++) {
      unsigned long long seed = 0xbf58476d1ce4e5b9ULL;
      seed = seed * 131 + x;
      seed = seed * 131 + y;
      link_zobrist[x][y] = splitmix64(seed);
    }
  }

  for (int x = 0; x < TOTAL_CARS; x++) {
    for (int state = 0; state < 4; state++) {
      macro_zobrist[x][state] =
          splitmix64(0x243f6a8885a308d3ULL ^
                     (unsigned long long)(x * 4 + state));
    }
  }

  for (int k = 0; k < 2; k++) {
    for (int l = 0; l < R; l++) {
      for (int p = 0; p < SIDING_CAP; p++) {
        for (int x = 0; x < TOTAL_CARS; x++) {
          unsigned long long seed = 1;
          seed = seed * 131 + k;
          seed = seed * 131 + l;
          seed = seed * 131 + p;
          seed = seed * 131 + x;
          zobrist[k][l][p][x] = splitmix64(seed);
        }
      }
    }
  }
}

struct FastHashSet {
  vector<unsigned long long> keys;
  vector<unsigned char> used;
  int mask = 0;

  explicit FastHashSet(int expected = 1) {
    int n = 1;
    while (n < expected * 4) n <<= 1;
    keys.assign(n, 0);
    used.assign(n, 0);
    mask = n - 1;
  }

  bool contains(unsigned long long x) const {
    int p = (int)(splitmix64(x) & mask);
    while (used[p]) {
      if (keys[p] == x) return true;
      p = (p + 1) & mask;
    }
    return false;
  }

  void insert(unsigned long long x) {
    int p = (int)(splitmix64(x) & mask);
    while (used[p]) {
      if (keys[p] == x) return;
      p = (p + 1) & mask;
    }
    used[p] = 1;
    keys[p] = x;
  }
};

bool better_key(const Key &lhs, const Key &rhs) {
  if (lhs.structure != rhs.structure) return lhs.structure > rhs.structure;
  if (lhs.line_proximity != rhs.line_proximity) return lhs.line_proximity > rhs.line_proximity;
  if (lhs.ready_edges != rhs.ready_edges) return lhs.ready_edges > rhs.ready_edges;
  return lhs.open_edges > rhs.open_edges;
}

bool same_key_tuple(const Key &lhs, const Key &rhs) {
  return lhs.structure == rhs.structure &&
         lhs.line_proximity == rhs.line_proximity &&
         lhs.ready_edges == rhs.ready_edges &&
         lhs.open_edges == rhs.open_edges;
}

unsigned long long candidate_primary_rank(const Candidate &cand) {
  constexpr unsigned long long STRUCT_BASE = 1ULL << 20;
  constexpr unsigned long long LINE_BASE = 1ULL << 12;
  constexpr unsigned long long EDGE_BASE = 1ULL << 8;
  constexpr unsigned long long SCORE_BASE = 1ULL << 12;
  unsigned long long rank = cand.non_decreasing ? 1ULL : 0ULL;
  rank = rank * 2ULL + (cand.same_turn ? 1ULL : 0ULL);
  rank = rank * STRUCT_BASE + (unsigned long long)cand.key.structure;
  rank = rank * LINE_BASE + (unsigned long long)cand.key.line_proximity;
  rank = rank * EDGE_BASE + (unsigned long long)cand.key.ready_edges;
  rank = rank * EDGE_BASE + (unsigned long long)cand.key.open_edges;
  rank = rank * SCORE_BASE + (unsigned long long)cand.score;
  return rank;
}

unsigned long long candidate_tie_rank(const Candidate &cand) {
  unsigned long long rank = (unsigned long long)cand.parent_bi;
  rank = rank * LAST_DIM + (unsigned long long)cand.parent_bj;
  rank = rank * 4096ULL + (unsigned long long)(cand.parent_beam + 1);
  rank = rank * 2ULL + (unsigned long long)cand.move.type;
  rank = rank * R + (unsigned long long)cand.move.i;
  rank = rank * R + (unsigned long long)cand.move.j;
  rank = rank * (SIDING_CAP + 1ULL) + (unsigned long long)cand.move.k;
  return rank;
}

void set_candidate_rank(Candidate &cand) {
  cand.primary_rank = candidate_primary_rank(cand);
  cand.tie_rank = candidate_tie_rank(cand);
}

bool better_candidate(const Candidate &lhs, const Candidate &rhs) {
  if (lhs.primary_rank != rhs.primary_rank) {
    return lhs.primary_rank > rhs.primary_rank;
  }
  if (lhs.tie_rank != rhs.tie_rank) {
    return lhs.tie_rank < rhs.tie_rank;
  }
#ifdef LOCAL_DEBUG
  if (lhs.non_decreasing != rhs.non_decreasing) {
    return lhs.non_decreasing > rhs.non_decreasing;
  }
  if (lhs.same_turn != rhs.same_turn) return lhs.same_turn > rhs.same_turn;
  if (!same_key_tuple(lhs.key, rhs.key)) {
    return better_key(lhs.key, rhs.key);
  }
  if (lhs.score != rhs.score) return lhs.score > rhs.score;
  if (lhs.parent_bi != rhs.parent_bi) return lhs.parent_bi < rhs.parent_bi;
  if (lhs.parent_bj != rhs.parent_bj) return lhs.parent_bj < rhs.parent_bj;
  if (lhs.parent_beam != rhs.parent_beam) return lhs.parent_beam < rhs.parent_beam;
  if (lhs.move.type != rhs.move.type) return lhs.move.type < rhs.move.type;
  if (lhs.move.i != rhs.move.i) return lhs.move.i < rhs.move.i;
  if (lhs.move.j != rhs.move.j) return lhs.move.j < rhs.move.j;
  return lhs.move.k < rhs.move.k;
#else
  return false;
#endif
}

bool key_quality_ge(const Key &lhs, const Key &rhs) {
  if (lhs.structure != rhs.structure) return lhs.structure > rhs.structure;
  if (lhs.line_proximity != rhs.line_proximity) {
    return lhs.line_proximity > rhs.line_proximity;
  }
  if (lhs.ready_edges != rhs.ready_edges) return lhs.ready_edges > rhs.ready_edges;
  return lhs.open_edges >= rhs.open_edges;
}

int edge_state_for_macro(const State &st, int x) {
  bool connected = st.next[x] == x + 1;
  if (connected) return 3;

  bool lower_open = st.kind[x] == 0 && st.tail[0][st.line[x]] == x;
  bool upper_open = st.kind[x + 1] == 1 && st.head[1][st.line[x + 1]] == x + 1;
  if (lower_open && upper_open) return 2;
  if (lower_open || upper_open) return 1;
  return 0;
}

unsigned long long macro_edge_hash(int x, int edge_state) {
  return macro_zobrist[x][edge_state];
}

unsigned long long compute_macro_hash(const State &st) {
  unsigned long long h = 0x6a09e667f3bcc909ULL;
  for (int x = 0; x + 1 < TOTAL_CARS; x++) {
    if (x % 10 == 9) continue;
    h ^= macro_edge_hash(x, edge_state_for_macro(st, x));
  }
  return h;
}

Track &get_track(State &st, int k, int l) {
  return k == 0 ? st.dep[l] : st.sid[l];
}

const Track &get_track(const State &st, int k, int l) {
  return k == 0 ? st.dep[l] : st.sid[l];
}

void sync_turn_key(State &) {
}

bool can_append_to_current_turn(const State &st, const Move &mv) {
  if (st.turn_count == 0) return false;
  return st.last_i < mv.i && st.last_j < mv.j;
}

int turns_after_move(const State &st, const Move &mv) {
  return st.turn_count + (can_append_to_current_turn(st, mv) ? 0 : 1);
}

Move inverse_move(const Move &mv) {
  return Move{1 - mv.type, mv.i, mv.j, mv.k};
}

unsigned long long compute_track_hash(const State &st, int k, int l) {
  const Track &tr = get_track(st, k, l);
  unsigned long long h = 0;
  for (int p = 0; p < tr.len; p++) {
    h ^= zobrist[k][l][p][tr.car[p]];
  }
  return h;
}

unsigned long long head_hash_value(int k, int l, int x) {
  return head_zobrist[k][l][x < 0 ? TOTAL_CARS : x];
}

unsigned long long link_hash_value(int x, int y) {
  return (x < 0 || y < 0) ? 0ULL : link_zobrist[x][y];
}

void refresh_track_locs(State &st, int k, int l) {
  Track &tr = get_track(st, k, l);
  st.head[k][l] = tr.len > 0 ? tr.car[0] : -1;
  st.tail[k][l] = tr.len > 0 ? tr.car[tr.len - 1] : -1;
  for (int p = 0; p < tr.len; p++) {
    int x = tr.car[p];
    st.kind[x] = k;
    st.line[x] = l;
    st.pos[x] = p;
    st.prev[x] = p > 0 ? tr.car[p - 1] : -1;
    st.next[x] = p + 1 < tr.len ? tr.car[p + 1] : -1;
  }
}

void refresh_track_locs_and_hash(State &st, int k, int l) {
  Track &tr = get_track(st, k, l);
  unsigned long long h = 0;
  st.head[k][l] = tr.len > 0 ? tr.car[0] : -1;
  st.tail[k][l] = tr.len > 0 ? tr.car[tr.len - 1] : -1;
  for (int p = 0; p < tr.len; p++) {
    int x = tr.car[p];
    st.kind[x] = k;
    st.line[x] = l;
    st.pos[x] = p;
    st.prev[x] = p > 0 ? tr.car[p - 1] : -1;
    st.next[x] = p + 1 < tr.len ? tr.car[p + 1] : -1;
    h ^= zobrist[k][l][p][x];
  }
  st.track_hash[k][l] = h;
}

int line_proximity_value(int row, int line) {
  return line_proximity_table[row][line];
}

void add_line_proximity_score(State &st, int x, int sign) {
  st.key.line_proximity +=
      sign * line_proximity_value(x / 10, st.line[x]);
}

int partial_score_fast(const Key &key) {
  return key.line_proximity;
}

bool target_edge_index(int x) {
  return 0 <= x && x + 1 < TOTAL_CARS && x % 10 != 9;
}

int connected_pair_score(int left, int right) {
  return target_edge_index(left) && left + 1 == right ? A_SCORE : 0;
}

int head_score(const State &st, int r) {
  const Track &tr = st.dep[r];
  return tr.len > 0 && tr.car[0] == 10 * r ? A_SCORE : 0;
}

struct EdgeScore {
  int structure = 0;
  int ready = 0;
  int open = 0;
};

EdgeScore edge_score(const State &st, int x) {
  EdgeScore res;
  if (x < 0 || x + 1 >= TOTAL_CARS || x % 10 == 9) return res;

  bool connected = st.next[x] == x + 1;
  if (connected) {
    res.structure = A_SCORE;
    return res;
  }

  bool lower_open = st.kind[x] == 0 && st.tail[0][st.line[x]] == x;
  bool upper_open = st.kind[x + 1] == 1 && st.head[1][st.line[x + 1]] == x + 1;

  if (lower_open && upper_open) {
    res.structure = C_SCORE;
    res.ready = 1;
  } else if (lower_open || upper_open) {
    res.structure = B_SCORE;
    res.open = 1;
  }
  return res;
}

void add_edge_score(State &st, int x, int sign) {
  EdgeScore e = edge_score(st, x);
  st.key.structure += sign * e.structure;
  st.key.ready_edges += sign * e.ready;
  st.key.open_edges += sign * e.open;
}

void add_edge_state_score(State &st, int edge_state, int sign) {
  st.key.structure += sign * EDGE_STRUCTURE_SCORE[edge_state];
  st.key.ready_edges += sign * EDGE_READY_SCORE[edge_state];
  st.key.open_edges += sign * EDGE_OPEN_SCORE[edge_state];
}

int contest_score(const State &st);

int partial_score_fast(const State &st) {
  return st.key.line_proximity;
}

struct TurnSnapshot {
  int dep_len = 0;
  int sid_len = 0;
  Key key;
  unsigned long long hash = 0;
  unsigned long long macro_hash = 0;
  unsigned long long dep_hash = 0;
  unsigned long long sid_hash = 0;
  int turn_count = 0;
  int last_i = -1;
  int last_j = -1;
  array<unsigned char, 80> edges;
  array<unsigned char, 80> edge_states;
  int edge_cnt = 0;
};

void collect_affected_cars(const State &st, const Move &mv, array<int, 32> &cars, int &cnt) {
  if (mv.type == 0) {
    const Track &from = st.dep[mv.i];
    const Track &to = st.sid[mv.j];
    int start = from.len - mv.k;
    for (int p = start; p < from.len; p++) cars[cnt++] = from.car[p];
    if (start > 0) cars[cnt++] = from.car[start - 1];  // New departure tail.
    if (to.len > 0) cars[cnt++] = to.car[0];           // Old siding head.
  } else {
    const Track &from = st.sid[mv.j];
    const Track &to = st.dep[mv.i];
    for (int p = 0; p < mv.k; p++) cars[cnt++] = from.car[p];
    if (from.len > mv.k) cars[cnt++] = from.car[mv.k]; // New siding head.
    if (to.len > 0) cars[cnt++] = to.car[to.len - 1];  // Old departure tail.
  }
}

void collect_affected_edges(const array<int, 32> &cars, int car_cnt,
                            array<unsigned char, 80> &edges, int &edge_cnt) {
  edge_cnt = 0;
  unsigned long long edge_mask0 = 0;
  unsigned long long edge_mask1 = 0;
  auto add_edge = [&](int e) {
    if (e < 0 || e >= TOTAL_CARS || e % 10 == 9) return;
    if (e < 64) {
      unsigned long long bit = 1ULL << e;
      if (edge_mask0 & bit) return;
      edge_mask0 |= bit;
    } else {
      unsigned long long bit = 1ULL << (e - 64);
      if (edge_mask1 & bit) return;
      edge_mask1 |= bit;
    }
    edges[edge_cnt++] = (unsigned char)e;
  };

  for (int idx = 0; idx < car_cnt; idx++) {
    int x = cars[idx];
    add_edge(x);
    add_edge(x - 1);
  }
}

struct CarLoc {
  int kind = 0;
  int line = 0;
  int pos = 0;
};

CarLoc loc_after_move(const State &st, const Move &mv, int x) {
  CarLoc loc{st.kind[x], st.line[x], st.pos[x]};
  if (mv.type == 0) {
    if (loc.kind == 0 && loc.line == mv.i) {
      int start = st.dep[mv.i].len - mv.k;
      if (loc.pos >= start) {
        return CarLoc{1, mv.j, loc.pos - start};
      }
    } else if (loc.kind == 1 && loc.line == mv.j) {
      return CarLoc{1, mv.j, loc.pos + mv.k};
    }
  } else {
    if (loc.kind == 1 && loc.line == mv.j) {
      if (loc.pos < mv.k) {
        return CarLoc{0, mv.i, st.dep[mv.i].len + loc.pos};
      }
      return CarLoc{1, mv.j, loc.pos - mv.k};
    }
  }
  return loc;
}

int dep_len_after_move(const State &st, const Move &mv, int line) {
  int len = st.dep[line].len;
  if (line != mv.i) return len;
  return mv.type == 0 ? len - mv.k : len + mv.k;
}

int sid_len_after_move(const State &st, const Move &mv, int line) {
  int len = st.sid[line].len;
  if (line != mv.j) return len;
  return mv.type == 0 ? len + mv.k : len - mv.k;
}

int dep_head_score_after_move(const State &st, const Move &mv, int line) {
  int len = dep_len_after_move(st, mv, line);
  if (len == 0) return 0;
  int head = st.dep[line].car[0];
  if (line == mv.i && mv.type == 1 && st.dep[line].len == 0) {
    head = st.sid[mv.j].car[0];
  }
  return head == 10 * line ? A_SCORE : 0;
}

int edge_state_after_move(const State &st, const Move &mv, int x) {
  CarLoc left = loc_after_move(st, mv, x);
  CarLoc right = loc_after_move(st, mv, x + 1);
  if (left.kind == right.kind && left.line == right.line &&
      left.pos + 1 == right.pos) {
    return 3;
  }

  bool lower_open = false;
  if (left.kind == 0) {
    lower_open = left.pos + 1 == dep_len_after_move(st, mv, left.line);
  }
  bool upper_open = right.kind == 1 && right.pos == 0;

  if (lower_open && upper_open) return 2;
  if (lower_open || upper_open) return 1;
  return 0;
}

unsigned long long dep_hash_after_move(const State &st, const Move &mv) {
  const Track &dep = st.dep[mv.i];
  const Track &sid = st.sid[mv.j];
  unsigned long long h = 0;
  if (mv.type == 0) {
    int new_len = dep.len - mv.k;
    for (int p = 0; p < new_len; p++) {
      h ^= zobrist[0][mv.i][p][dep.car[p]];
    }
  } else {
    for (int p = 0; p < dep.len; p++) {
      h ^= zobrist[0][mv.i][p][dep.car[p]];
    }
    for (int p = 0; p < mv.k; p++) {
      h ^= zobrist[0][mv.i][dep.len + p][sid.car[p]];
    }
  }
  return h;
}

unsigned long long sid_hash_after_move(const State &st, const Move &mv) {
  const Track &dep = st.dep[mv.i];
  const Track &sid = st.sid[mv.j];
  unsigned long long h = 0;
  if (mv.type == 0) {
    int start = dep.len - mv.k;
    for (int p = 0; p < mv.k; p++) {
      h ^= zobrist[1][mv.j][p][dep.car[start + p]];
    }
    for (int p = 0; p < sid.len; p++) {
      h ^= zobrist[1][mv.j][mv.k + p][sid.car[p]];
    }
  } else {
    int new_len = sid.len - mv.k;
    for (int p = 0; p < new_len; p++) {
      h ^= zobrist[1][mv.j][p][sid.car[mv.k + p]];
    }
  }
  return h;
}

struct EvalResult {
  Key key;
  unsigned long long hash = 0;
  unsigned long long macro_hash = 0;
  int score = 0;
};

unsigned long long hash_after_move(const State &st, const Move &mv) {
  const Track &dep = st.dep[mv.i];
  const Track &sid = st.sid[mv.j];
  unsigned long long h = st.hash;
  if (mv.type == 0) {
    int dep_start = dep.len - mv.k;
    int a = dep.car[dep_start];
    int b = dep.car[dep.len - 1];
    int p = dep_start > 0 ? dep.car[dep_start - 1] : -1;
    int old_sid_head = st.head[1][mv.j];
    int new_dep_head = dep_start == 0 ? -1 : st.head[0][mv.i];

    h ^= head_hash_value(0, mv.i, st.head[0][mv.i]);
    h ^= head_hash_value(0, mv.i, new_dep_head);
    h ^= head_hash_value(1, mv.j, old_sid_head);
    h ^= head_hash_value(1, mv.j, a);
    h ^= link_hash_value(p, a);
    h ^= link_hash_value(b, old_sid_head);
  } else {
    int a = sid.car[0];
    int b = sid.car[mv.k - 1];
    int new_sid_head = mv.k < sid.len ? sid.car[mv.k] : -1;
    int old_dep_head = st.head[0][mv.i];
    int new_dep_head = dep.len == 0 ? a : old_dep_head;
    int old_dep_tail = st.tail[0][mv.i];

    h ^= head_hash_value(1, mv.j, st.head[1][mv.j]);
    h ^= head_hash_value(1, mv.j, new_sid_head);
    h ^= head_hash_value(0, mv.i, old_dep_head);
    h ^= head_hash_value(0, mv.i, new_dep_head);
    h ^= link_hash_value(b, new_sid_head);
    h ^= link_hash_value(old_dep_tail, a);
  }
  return h;
}

template <int TYPE>
EvalResult evaluate_move_fast_typed(const State &st, int i, int j, int k,
                                    int line_delta) {
#ifdef PERF_STATS
  PerfTimer timer(perf.ns_apply_eval);
  perf.eval_calls++;
#endif
  const Track &dep = st.dep[i];
  const Track &sid = st.sid[j];
  const int dep_len = dep.len;
  const int sid_len = sid.len;
  const int dep_start = TYPE == 0 ? dep_len - k : 0;
  const int next_dep_len = TYPE == 0 ? dep_len - k : dep_len + k;
  const int a = TYPE == 0 ? dep.car[dep_start] : sid.car[0];
  const int b = TYPE == 0 ? dep.car[dep_len - 1] : sid.car[k - 1];
  const int p = TYPE == 0 && dep_start > 0 ? dep.car[dep_start - 1] : -1;
  const int h = TYPE == 0 ? (sid_len > 0 ? sid.car[0] : -1)
                          : (k < sid_len ? sid.car[k] : -1);
  const int t = TYPE == 1 && dep_len > 0 ? dep.car[dep_len - 1] : -1;

  unsigned long long next_hash = st.hash;
  if constexpr (TYPE == 0) {
    const int old_dep_head = st.head[0][i];
    const int new_dep_head = dep_start == 0 ? -1 : old_dep_head;
    next_hash ^= head_hash_value(0, i, old_dep_head);
    next_hash ^= head_hash_value(0, i, new_dep_head);
    next_hash ^= head_hash_value(1, j, h);
    next_hash ^= head_hash_value(1, j, a);
    next_hash ^= link_hash_value(p, a);
    next_hash ^= link_hash_value(b, h);
  } else {
    const int old_dep_head = st.head[0][i];
    const int new_dep_head = dep_len == 0 ? a : old_dep_head;
    next_hash ^= head_hash_value(1, j, a);
    next_hash ^= head_hash_value(1, j, h);
    next_hash ^= head_hash_value(0, i, old_dep_head);
    next_hash ^= head_hash_value(0, i, new_dep_head);
    next_hash ^= link_hash_value(b, h);
    next_hash ^= link_hash_value(t, a);
  }

  EvalResult res;
  res.key = st.key;
  res.hash = next_hash;
  res.macro_hash = st.macro_hash;
  res.key.line_proximity += line_delta;

  res.key.structure -=
      dep_len > 0 && dep.car[0] == 10 * i ? A_SCORE : 0;
  if (next_dep_len > 0) {
    int head = dep.car[0];
    if constexpr (TYPE == 1) {
      if (dep_len == 0) head = sid.car[0];
    }
    if (head == 10 * i) res.key.structure += A_SCORE;
  }

  auto next_after = [&](int x) {
    if constexpr (TYPE == 0) {
      if (x == p) return -1;
      if (x == b) return h;
    } else {
      if (x == b) return -1;
      if (x == t) return a;
    }
    return st.next[x];
  };

  auto is_dep_tail_after = [&](int x) {
    if constexpr (TYPE == 0) {
      if (x == p) return true;
      if (st.kind[x] != 0) return false;
      const int line = st.line[x];
      return line != i && st.tail[0][line] == x;
    }
    if (x == b) return true;
    if (st.kind[x] != 0) return false;
    const int line = st.line[x];
    return line != i && st.tail[0][line] == x;
  };

  auto is_sid_head_after = [&](int x) {
    if constexpr (TYPE == 0) {
      if (x == a) return true;
      if (st.kind[x] != 1) return false;
      const int line = st.line[x];
      return line != j && st.head[1][line] == x;
    }
    if (x == h) return true;
    if (st.kind[x] != 1) return false;
    const int line = st.line[x];
    return line != j && st.head[1][line] == x;
  };

  auto edge_after = [&](int e) {
    if (next_after(e) == e + 1) return 3;
    bool lower_open = is_dep_tail_after(e);
    bool upper_open = is_sid_head_after(e + 1);
    if (lower_open && upper_open) return 2;
    if (lower_open || upper_open) return 1;
    return 0;
  };

  unsigned long long edge_mask0 = 0;
  unsigned long long edge_mask1 = 0;
  auto add_edge_delta = [&](int e) {
    if (e < 0 || e >= TOTAL_CARS || e % 10 == 9) return;
    if (e < 64) {
      unsigned long long bit = 1ULL << e;
      if (edge_mask0 & bit) return;
      edge_mask0 |= bit;
    } else {
      unsigned long long bit = 1ULL << (e - 64);
      if (edge_mask1 & bit) return;
      edge_mask1 |= bit;
    }
    int old_state = st.macro_edge_state[e];
    int new_state = edge_after(e);
    res.macro_hash ^= macro_edge_hash(e, old_state);
    res.macro_hash ^= macro_edge_hash(e, new_state);
    res.key.structure -= EDGE_STRUCTURE_SCORE[old_state];
    res.key.ready_edges -= EDGE_READY_SCORE[old_state];
    res.key.open_edges -= EDGE_OPEN_SCORE[old_state];
    res.key.structure += EDGE_STRUCTURE_SCORE[new_state];
    res.key.ready_edges += EDGE_READY_SCORE[new_state];
    res.key.open_edges += EDGE_OPEN_SCORE[new_state];
  };

  if constexpr (TYPE == 0) {
    add_edge_delta(p);
    add_edge_delta(b);
    add_edge_delta(a - 1);
    add_edge_delta(h - 1);
  } else {
    add_edge_delta(t);
    add_edge_delta(b);
    add_edge_delta(a - 1);
    add_edge_delta(h - 1);
  }

  res.score = partial_score_fast(res.key);
  return res;
}

EvalResult evaluate_move_fast(const State &st, const Move &mv, int line_delta) {
  if (mv.type == 0) {
    return evaluate_move_fast_typed<0>(st, mv.i, mv.j, mv.k, line_delta);
  }
  return evaluate_move_fast_typed<1>(st, mv.i, mv.j, mv.k, line_delta);
}

void apply_tracks_raw(State &st, const Move &mv) {
  if (mv.type == 0) {
    Track &from = st.dep[mv.i];
    Track &to = st.sid[mv.j];
    int block[SIDING_CAP];
    memcpy(block, &from.car[from.len - mv.k], mv.k * sizeof(int));
    from.len -= mv.k;

    memmove(&to.car[mv.k], &to.car[0], to.len * sizeof(int));
    memcpy(&to.car[0], block, mv.k * sizeof(int));
    to.len += mv.k;
  } else {
    Track &from = st.sid[mv.j];
    Track &to = st.dep[mv.i];
    int block[SIDING_CAP];
    memcpy(block, &from.car[0], mv.k * sizeof(int));
    memmove(&from.car[0], &from.car[mv.k], (from.len - mv.k) * sizeof(int));
    from.len -= mv.k;

    memcpy(&to.car[to.len], block, mv.k * sizeof(int));
    to.len += mv.k;
  }
}

void undo_tracks_raw(State &st, const Move &mv, int old_dep_len, int old_sid_len) {
  Track &dep = st.dep[mv.i];
  Track &sid = st.sid[mv.j];
  if (mv.type == 0) {
    int new_dep_len = old_dep_len - mv.k;
    memcpy(&dep.car[new_dep_len], &sid.car[0], mv.k * sizeof(int));
    memmove(&sid.car[0], &sid.car[mv.k], old_sid_len * sizeof(int));
    dep.len = old_dep_len;
    sid.len = old_sid_len;
  } else {
    int new_sid_len = old_sid_len - mv.k;
    memmove(&sid.car[mv.k], &sid.car[0], new_sid_len * sizeof(int));
    memcpy(&sid.car[0], &dep.car[old_dep_len], mv.k * sizeof(int));
    dep.len = old_dep_len;
    sid.len = old_sid_len;
  }
}

void apply_move_to_board(State &st, const Move &mv, TurnSnapshot *snap = nullptr) {
#ifdef PERF_STATS
  perf.apply_calls++;
  long long t_apply = PerfStats::now_ns();
  long long t_stage = t_apply;
#endif
  unsigned long long next_hash = hash_after_move(st, mv);
  array<int, 32> cars;
  int car_cnt = 0;
  collect_affected_cars(st, mv, cars, car_cnt);

  array<unsigned char, 80> edges;
  int edge_cnt = 0;
  unsigned long long edge_mask0 = 0;
  unsigned long long edge_mask1 = 0;
  auto add_edge = [&](int e) {
    if (e < 0 || e >= TOTAL_CARS || e % 10 == 9) return;
    if (e < 64) {
      unsigned long long bit = 1ULL << e;
      if (edge_mask0 & bit) return;
      edge_mask0 |= bit;
    } else {
      unsigned long long bit = 1ULL << (e - 64);
      if (edge_mask1 & bit) return;
      edge_mask1 |= bit;
    }
    edges[edge_cnt++] = (unsigned char)e;
  };

  for (int idx = 0; idx < car_cnt; idx++) {
    int x = cars[idx];
    add_edge(x);
    add_edge(x - 1);
  }

  if (snap != nullptr) snap->edge_cnt = edge_cnt;
#ifdef PERF_STATS
  long long t_now = PerfStats::now_ns();
  perf.ns_apply_collect += t_now - t_stage;
  t_stage = t_now;
#endif

  st.key.structure -= head_score(st, mv.i);
  for (int idx = 0; idx < edge_cnt; idx++) {
    int e = (int)edges[idx];
    int old_state = st.macro_edge_state[e];
    if (snap != nullptr) {
      snap->edges[idx] = (unsigned char)e;
      snap->edge_states[idx] = (unsigned char)old_state;
    }
    st.macro_hash ^= macro_edge_hash(e, old_state);
    add_edge_state_score(st, old_state, -1);
  }
#ifdef PERF_STATS
  t_now = PerfStats::now_ns();
  perf.ns_apply_remove_score += t_now - t_stage;
  t_stage = t_now;
#endif
  for (int idx = 0; idx < car_cnt; idx++) {
    add_line_proximity_score(st, cars[idx], -1);
  }

  apply_tracks_raw(st, mv);
#ifdef PERF_STATS
  t_now = PerfStats::now_ns();
  perf.ns_apply_raw_move += t_now - t_stage;
  t_stage = t_now;
#endif
  refresh_track_locs_and_hash(st, 0, mv.i);
  refresh_track_locs_and_hash(st, 1, mv.j);
  st.hash = next_hash;
#ifdef PERF_STATS
  t_now = PerfStats::now_ns();
  perf.ns_apply_refresh += t_now - t_stage;
  t_stage = t_now;
#endif

  st.key.structure += head_score(st, mv.i);
  for (int idx = 0; idx < edge_cnt; idx++) {
    int e = (int)edges[idx];
    int new_state = edge_state_for_macro(st, e);
    st.macro_edge_state[e] = (unsigned char)new_state;
    st.macro_hash ^= macro_edge_hash(e, new_state);
    add_edge_state_score(st, new_state, 1);
  }
  for (int idx = 0; idx < car_cnt; idx++) {
    add_line_proximity_score(st, cars[idx], 1);
  }
#ifdef PERF_STATS
  t_now = PerfStats::now_ns();
  perf.ns_apply_add_score += t_now - t_stage;
  perf.ns_apply_board += t_now - t_apply;
#endif
}

void rebuild_state_cache(State &st) {
  st.head.fill({});
  st.tail.fill({});
  for (int k = 0; k < 2; k++) {
    st.head[k].fill(-1);
    st.tail[k].fill(-1);
  }
  st.prev.fill(-1);
  st.next.fill(-1);
  st.kind.fill(-1);
  st.line.fill(-1);
  st.pos.fill(-1);
  st.macro_edge_state.fill(0);
  st.key = Key{};
  st.hash = 0x510e527fade682d1ULL;
  st.macro_hash = 0x6a09e667f3bcc909ULL;

  for (int l = 0; l < R; l++) {
    refresh_track_locs_and_hash(st, 0, l);
    refresh_track_locs_and_hash(st, 1, l);
    st.hash ^= head_hash_value(0, l, st.head[0][l]);
    st.hash ^= head_hash_value(1, l, st.head[1][l]);
    const Track &dep = st.dep[l];
    for (int p = 0; p + 1 < dep.len; p++) {
      st.hash ^= link_hash_value(dep.car[p], dep.car[p + 1]);
    }
    const Track &sid = st.sid[l];
    for (int p = 0; p + 1 < sid.len; p++) {
      st.hash ^= link_hash_value(sid.car[p], sid.car[p + 1]);
    }
  }

  for (int r = 0; r < R; r++) {
    st.key.structure += head_score(st, r);
  }
  for (int x = 0; x < TOTAL_CARS; x++) {
    add_line_proximity_score(st, x, 1);
  }
  for (int x = 0; x + 1 < TOTAL_CARS; x++) {
    if (x % 10 == 9) continue;
    int edge_state = edge_state_for_macro(st, x);
    st.macro_edge_state[x] = (unsigned char)edge_state;
    st.macro_hash ^= macro_edge_hash(x, edge_state);
    add_edge_state_score(st, edge_state, 1);
  }
  sync_turn_key(st);
}

void apply_transition(State &st, const Move &mv, int next_turn_count,
                      TurnSnapshot *snap = nullptr) {
  apply_move_to_board(st, mv, snap);
  st.turn_count = next_turn_count;
  st.last_i = mv.i;
  st.last_j = mv.j;
  sync_turn_key(st);
}

void apply_for_eval(State &st, const Move &mv, int next_turn_count,
                    TurnSnapshot &snap) {
#ifdef PERF_STATS
  PerfTimer timer(perf.ns_apply_eval);
  perf.eval_calls++;
#endif
  snap.dep_len = st.dep[mv.i].len;
  snap.sid_len = st.sid[mv.j].len;
  snap.key = st.key;
  snap.hash = st.hash;
  snap.macro_hash = st.macro_hash;
  snap.dep_hash = st.track_hash[0][mv.i];
  snap.sid_hash = st.track_hash[1][mv.j];
  snap.turn_count = st.turn_count;
  snap.last_i = st.last_i;
  snap.last_j = st.last_j;
  apply_transition(st, mv, next_turn_count, &snap);
}

void undo_eval(State &st, const Move &mv, const TurnSnapshot &snap) {
#ifdef PERF_STATS
  PerfTimer timer(perf.ns_undo);
  perf.undo_calls++;
#endif
  undo_tracks_raw(st, mv, snap.dep_len, snap.sid_len);
  st.key = snap.key;
  st.hash = snap.hash;
  st.macro_hash = snap.macro_hash;
  st.track_hash[0][mv.i] = snap.dep_hash;
  st.track_hash[1][mv.j] = snap.sid_hash;
  st.turn_count = snap.turn_count;
  st.last_i = snap.last_i;
  st.last_j = snap.last_j;
  refresh_track_locs(st, 0, mv.i);
  refresh_track_locs(st, 1, mv.j);
  for (int idx = 0; idx < snap.edge_cnt; idx++) {
    st.macro_edge_state[(int)snap.edges[idx]] = snap.edge_states[idx];
  }

  sync_turn_key(st);
}

#ifdef LOCAL_DEBUG
bool same_key(const Key &a, const Key &b) {
  return a.structure == b.structure &&
         a.line_proximity == b.line_proximity &&
         a.ready_edges == b.ready_edges &&
         a.open_edges == b.open_edges;
}

State recompute_state_for_check(const State &st) {
  State tmp = st;
  rebuild_state_cache(tmp);
  tmp.turn_count = st.turn_count;
  tmp.last_i = st.last_i;
  tmp.last_j = st.last_j;
  sync_turn_key(tmp);
  return tmp;
}
#endif

bool is_goal(const State &st) {
  for (int r = 0; r < R; r++) {
    if (st.dep[r].len != CARS_PER_LINE) return false;
    for (int c = 0; c < CARS_PER_LINE; c++) {
      if (st.dep[r].car[c] != 10 * r + c) return false;
    }
  }
  return true;
}

int contest_score(const State &st) {
  if (is_goal(st)) return 5000 - st.turn_count;
  return st.key.line_proximity;
}

bool legal_move(const State &st, const Move &mv) {
  if (mv.type == 0) {
    if (st.dep[mv.i].len < mv.k) return false;
    if (st.sid[mv.j].len + mv.k > SIDING_CAP) return false;
  } else {
    if (st.sid[mv.j].len < mv.k) return false;
    if (st.dep[mv.i].len + mv.k > MAIN_CAP) return false;
  }
  return turns_after_move(st, mv) <= TMAX;
}

vector<Move> build_move_path(const vector<Node> &nodes, int node_id) {
  vector<Move> path;
  while (node_id > 0) {
    path.push_back(nodes[node_id].move);
    node_id = nodes[node_id].parent;
  }
  reverse(path.begin(), path.end());
  return path;
}

vector<vector<Move>> build_turns_from_path(const vector<Move> &path) {
  vector<vector<Move>> turns;
  int last_i = -1;
  int last_j = -1;
  for (const Move &mv : path) {
    if (!turns.empty() && last_i < mv.i && last_j < mv.j) {
      turns.back().push_back(mv);
    } else {
      turns.push_back(vector<Move>{mv});
    }
    last_i = mv.i;
    last_j = mv.j;
  }
  return turns;
}

bool noncrossing_pair(const Move &a, const Move &b) {
  if (a.i == b.i || a.j == b.j) return false;
  return (a.i < b.i) == (a.j < b.j);
}

bool can_add_to_turn(const vector<Move> &turn, const Move &mv) {
  for (const Move &other : turn) {
    if (!noncrossing_pair(other, mv)) return false;
  }
  return true;
}

void add_move_to_turn_sorted(vector<Move> &turn, const Move &mv) {
  auto it = lower_bound(turn.begin(), turn.end(), mv,
                        [](const Move &lhs, const Move &rhs) {
                          if (lhs.i != rhs.i) return lhs.i < rhs.i;
                          return lhs.j < rhs.j;
                        });
  turn.insert(it, mv);
}

vector<vector<Move>> first_fit_reschedule_turns_from_path(
    const vector<Move> &path) {
  int n = (int)path.size();
  if (n == 0) return {};

  vector<vector<Move>> turns;
  vector<int> move_turn(n, -1);
  array<int, R> last_dep;
  array<int, R> last_sid;
  last_dep.fill(-1);
  last_sid.fill(-1);

  for (int id = 0; id < n; id++) {
    const Move &mv = path[id];
    int earliest = 0;
    if (last_dep[mv.i] != -1) {
      earliest = max(earliest, move_turn[last_dep[mv.i]] + 1);
    }
    if (last_sid[mv.j] != -1) {
      earliest = max(earliest, move_turn[last_sid[mv.j]] + 1);
    }

    int assigned = -1;
    for (int t = earliest; t < (int)turns.size(); t++) {
      if (can_add_to_turn(turns[t], mv)) {
        assigned = t;
        break;
      }
    }
    if (assigned == -1) {
      assigned = (int)turns.size();
      turns.push_back({});
    }
    add_move_to_turn_sorted(turns[assigned], mv);
    move_turn[id] = assigned;
    last_dep[mv.i] = id;
    last_sid[mv.j] = id;
  }

  return turns;
}

vector<int> choose_reschedule_turn(const vector<int> &ready,
                                   const vector<Move> &path,
                                   const vector<vector<int>> &children) {
  int m = (int)ready.size();
  vector<int> best;
  long long best_score = numeric_limits<long long>::min();
  for (int mask = 1; mask < (1 << m); mask++) {
    bool ok = true;
    int count = 0;
    int unlock = 0;
    int index_sum = 0;
    for (int a = 0; a < m && ok; a++) {
      if (((mask >> a) & 1) == 0) continue;
      int id_a = ready[a];
      count++;
      unlock += (int)children[id_a].size();
      index_sum += id_a;
      for (int b = a + 1; b < m; b++) {
        if (((mask >> b) & 1) == 0) continue;
        if (!noncrossing_pair(path[id_a], path[ready[b]])) {
          ok = false;
          break;
        }
      }
    }
    if (!ok) continue;

    long long score =
        (long long)count * 1000000000000LL +
        (long long)unlock * 1000000LL -
        index_sum;
    if (score > best_score) {
      best_score = score;
      best.clear();
      for (int a = 0; a < m; a++) {
        if ((mask >> a) & 1) best.push_back(ready[a]);
      }
    }
  }

  sort(best.begin(), best.end(), [&](int lhs, int rhs) {
    if (path[lhs].i != path[rhs].i) return path[lhs].i < path[rhs].i;
    return path[lhs].j < path[rhs].j;
  });
  return best;
}

vector<vector<Move>> reschedule_turns_from_path(const vector<Move> &path) {
  int n = (int)path.size();
  if (n == 0) return {};

  vector<vector<int>> children(n);
  vector<unsigned char> indeg(n, 0);
  array<int, R> last_dep;
  array<int, R> last_sid;
  last_dep.fill(-1);
  last_sid.fill(-1);

  for (int id = 0; id < n; id++) {
    const Move &mv = path[id];
    int dep_parent = last_dep[mv.i];
    int sid_parent = last_sid[mv.j];
    if (dep_parent != -1) {
      children[dep_parent].push_back(id);
      indeg[id]++;
    }
    if (sid_parent != -1 && sid_parent != dep_parent) {
      children[sid_parent].push_back(id);
      indeg[id]++;
    }
    last_dep[mv.i] = id;
    last_sid[mv.j] = id;
  }

  vector<int> ready;
  ready.reserve(R);
  for (int id = 0; id < n; id++) {
    if (indeg[id] == 0) ready.push_back(id);
  }

  vector<vector<Move>> turns;
  vector<unsigned char> selected(n, 0);
  int remaining = n;
  while (remaining > 0) {
    if (ready.empty()) return build_turns_from_path(path);

    vector<int> turn_ids = choose_reschedule_turn(ready, path, children);
    if (turn_ids.empty()) {
      turn_ids.push_back(ready[0]);
    }

    vector<Move> turn;
    turn.reserve(turn_ids.size());
    for (int id : turn_ids) {
      selected[id] = 1;
      turn.push_back(path[id]);
    }
    turns.push_back(move(turn));

    vector<int> next_ready;
    next_ready.reserve(R);
    for (int id : ready) {
      if (!selected[id]) next_ready.push_back(id);
    }
    for (int id : turn_ids) {
      selected[id] = 0;
      remaining--;
      for (int child : children[id]) {
        indeg[child]--;
        if (indeg[child] == 0) next_ready.push_back(child);
      }
    }
    ready.swap(next_ready);
  }

  return turns;
}

struct ScheduleSearchNode {
  int parent = -1;
  array<unsigned short, R> ids{};
  unsigned char cnt = 0;
};

struct ScheduleState {
  array<unsigned short, 2 * R> progress{};
  int node = 0;
  int done = 0;
  unsigned long long hash = 0;
  long long rank = 0;
};

struct ScheduleCandidate {
  array<unsigned short, 2 * R> progress{};
  int parent_node = 0;
  array<unsigned short, R> ids{};
  unsigned char cnt = 0;
  int done = 0;
  unsigned long long hash = 0;
  long long rank = 0;
};

vector<vector<Move>> reconstruct_rescheduled_turns(
    const vector<ScheduleSearchNode> &nodes, int node_id,
    const vector<Move> &path) {
  vector<vector<Move>> turns;
  while (node_id > 0) {
    const ScheduleSearchNode &node = nodes[node_id];
    vector<Move> turn;
    turn.reserve(node.cnt);
    for (int i = 0; i < node.cnt; i++) {
      turn.push_back(path[node.ids[i]]);
    }
    turns.push_back(move(turn));
    node_id = node.parent;
  }
  reverse(turns.begin(), turns.end());
  return turns;
}

vector<int> ready_moves_for_progress(
    const array<unsigned short, 2 * R> &progress,
    const array<vector<int>, R> &dep_seq,
    const array<vector<int>, R> &sid_seq,
    const vector<Move> &path) {
  vector<int> ready;
  ready.reserve(R);
  for (int i = 0; i < R; i++) {
    int p = progress[i];
    if (p >= (int)dep_seq[i].size()) continue;
    int id = dep_seq[i][p];
    int j = path[id].j;
    int q = progress[R + j];
    if (q < (int)sid_seq[j].size() && sid_seq[j][q] == id) {
      ready.push_back(id);
    }
  }
  return ready;
}

long long schedule_state_rank(const ScheduleState &st,
                              const array<vector<int>, R> &dep_seq,
                              const array<vector<int>, R> &sid_seq,
                              const vector<Move> &path) {
  int max_rem = 0;
  int active_lines = 0;
  int rem_sum_sq = 0;
  for (int i = 0; i < R; i++) {
    int rem = (int)dep_seq[i].size() - st.progress[i];
    max_rem = max(max_rem, rem);
    active_lines += rem > 0;
    rem_sum_sq += rem * rem;
  }
  for (int j = 0; j < R; j++) {
    int rem = (int)sid_seq[j].size() - st.progress[R + j];
    max_rem = max(max_rem, rem);
    active_lines += rem > 0;
    rem_sum_sq += rem * rem;
  }
  int ready_count =
      (int)ready_moves_for_progress(st.progress, dep_seq, sid_seq, path).size();
  return (long long)st.done * 1000000000000LL -
         (long long)max_rem * 1000000000LL -
         (long long)rem_sum_sq * 1000000LL +
         (long long)ready_count * 10000LL -
         active_lines;
}

vector<pair<long long, int>> feasible_subset_masks(
    const vector<int> &ready,
    const vector<Move> &path,
    const vector<int> &child_count) {
  int m = (int)ready.size();
  vector<pair<long long, int>> masks;
  masks.reserve(1 << m);
  for (int mask = 1; mask < (1 << m); mask++) {
    bool ok = true;
    int count = 0;
    int unlock = 0;
    int index_sum = 0;
    for (int a = 0; a < m && ok; a++) {
      if (((mask >> a) & 1) == 0) continue;
      int id_a = ready[a];
      count++;
      unlock += child_count[id_a];
      index_sum += id_a;
      for (int b = a + 1; b < m; b++) {
        if (((mask >> b) & 1) == 0) continue;
        if (!noncrossing_pair(path[id_a], path[ready[b]])) {
          ok = false;
          break;
        }
      }
    }
    if (!ok) continue;
    long long score =
        (long long)count * 1000000000LL +
        (long long)unlock * 100000LL -
        index_sum;
    masks.push_back({score, mask});
  }
  sort(masks.begin(), masks.end(), greater<pair<long long, int>>());
  if ((int)masks.size() > RESCHEDULE_SUBSET_LIMIT) {
    masks.resize(RESCHEDULE_SUBSET_LIMIT);
  }
  return masks;
}

vector<vector<Move>> beam_reschedule_turns_from_path(
    const vector<Move> &path, int current_best_turns) {
  int n = (int)path.size();
  if (n == 0 || current_best_turns <= 1) return {};

  array<vector<int>, R> dep_seq;
  array<vector<int>, R> sid_seq;
  vector<int> child_count(n, 0);
  array<int, R> last_dep;
  array<int, R> last_sid;
  last_dep.fill(-1);
  last_sid.fill(-1);

  for (int id = 0; id < n; id++) {
    const Move &mv = path[id];
    if (last_dep[mv.i] != -1) child_count[last_dep[mv.i]]++;
    if (last_sid[mv.j] != -1 && last_sid[mv.j] != last_dep[mv.i]) {
      child_count[last_sid[mv.j]]++;
    }
    dep_seq[mv.i].push_back(id);
    sid_seq[mv.j].push_back(id);
    last_dep[mv.i] = id;
    last_sid[mv.j] = id;
  }

  vector<vector<unsigned long long>> progress_zobrist(2 * R);
  for (int l = 0; l < 2 * R; l++) {
    int limit = l < R ? dep_seq[l].size() : sid_seq[l - R].size();
    progress_zobrist[l].resize(limit + 1);
    for (int p = 0; p <= limit; p++) {
      unsigned long long seed = 0x517cc1b727220a95ULL;
      seed = seed * 131 + l;
      seed = seed * 131 + p;
      progress_zobrist[l][p] = splitmix64(seed);
    }
  }

  vector<ScheduleSearchNode> nodes;
  nodes.reserve(RESCHEDULE_BEAM_WIDTH * current_best_turns + 1);
  nodes.push_back(ScheduleSearchNode{});

  ScheduleState initial;
  for (int l = 0; l < 2 * R; l++) {
    initial.hash ^= progress_zobrist[l][0];
  }
  initial.rank = schedule_state_rank(initial, dep_seq, sid_seq, path);
  vector<ScheduleState> beam{initial};

  auto increment_line = [&](ScheduleCandidate &cand, int line) {
    int old_p = cand.progress[line];
    int new_p = old_p + 1;
    cand.hash ^= progress_zobrist[line][old_p];
    cand.hash ^= progress_zobrist[line][new_p];
    cand.progress[line] = (unsigned short)new_p;
  };

  for (int turn = 1; turn < current_best_turns; turn++) {
    vector<ScheduleCandidate> next;
    next.reserve(RESCHEDULE_BEAM_WIDTH * 8);
    unordered_map<unsigned long long, int> seen;
    seen.reserve(RESCHEDULE_BEAM_WIDTH * RESCHEDULE_SUBSET_LIMIT * 2);

    for (const ScheduleState &st : beam) {
      vector<int> ready = ready_moves_for_progress(
          st.progress, dep_seq, sid_seq, path);
      if (ready.empty()) continue;

      vector<pair<long long, int>> masks =
          feasible_subset_masks(ready, path, child_count);
      for (auto [mask_score, mask] : masks) {
        ScheduleCandidate cand;
        cand.progress = st.progress;
        cand.parent_node = st.node;
        cand.done = st.done;
        cand.hash = st.hash;
        cand.cnt = 0;
        for (int b = 0; b < (int)ready.size(); b++) {
          if (((mask >> b) & 1) == 0) continue;
          int id = ready[b];
          const Move &mv = path[id];
          cand.ids[cand.cnt++] = (unsigned short)id;
          cand.done++;
          increment_line(cand, mv.i);
          increment_line(cand, R + mv.j);
        }
        sort(cand.ids.begin(), cand.ids.begin() + cand.cnt,
             [&](unsigned short lhs, unsigned short rhs) {
               const Move &a = path[lhs];
               const Move &b = path[rhs];
               if (a.i != b.i) return a.i < b.i;
               return a.j < b.j;
             });

        ScheduleState rank_state;
        rank_state.progress = cand.progress;
        rank_state.done = cand.done;
        cand.rank = schedule_state_rank(rank_state, dep_seq, sid_seq, path) +
                    mask_score;

        if (cand.done == n) {
          int node_id = (int)nodes.size();
          nodes.push_back(ScheduleSearchNode{cand.parent_node, cand.ids,
                                             cand.cnt});
          return reconstruct_rescheduled_turns(nodes, node_id, path);
        }

        auto it = seen.find(cand.hash);
        if (it == seen.end()) {
          seen[cand.hash] = (int)next.size();
          next.push_back(cand);
        } else if (cand.rank > next[it->second].rank) {
          next[it->second] = cand;
        }
      }
    }

    if (next.empty()) break;
    if ((int)next.size() > RESCHEDULE_BEAM_WIDTH) {
      nth_element(next.begin(), next.begin() + RESCHEDULE_BEAM_WIDTH,
                  next.end(), [](const ScheduleCandidate &lhs,
                                 const ScheduleCandidate &rhs) {
                    return lhs.rank > rhs.rank;
                  });
      next.resize(RESCHEDULE_BEAM_WIDTH);
    }
    sort(next.begin(), next.end(), [](const ScheduleCandidate &lhs,
                                      const ScheduleCandidate &rhs) {
      return lhs.rank > rhs.rank;
    });

    beam.clear();
    beam.reserve(next.size());
    for (const ScheduleCandidate &cand : next) {
      int node_id = (int)nodes.size();
      nodes.push_back(ScheduleSearchNode{cand.parent_node, cand.ids,
                                         cand.cnt});
      beam.push_back(ScheduleState{cand.progress, node_id, cand.done,
                                   cand.hash, cand.rank});
    }
  }

  return {};
}

int output_answer(const vector<Node> &nodes, int node_id) {
  vector<Move> path = build_move_path(nodes, node_id);
  vector<vector<Move>> turns = build_turns_from_path(path);
  vector<vector<Move>> first_fit = first_fit_reschedule_turns_from_path(path);
  if (!first_fit.empty() && first_fit.size() < turns.size()) {
    turns = move(first_fit);
  }
  vector<vector<Move>> rescheduled = reschedule_turns_from_path(path);
  if (!rescheduled.empty() && rescheduled.size() < turns.size()) {
    turns = move(rescheduled);
  }
  vector<vector<Move>> beam_rescheduled =
      beam_reschedule_turns_from_path(path, (int)turns.size());
  if (!beam_rescheduled.empty() && beam_rescheduled.size() < turns.size()) {
    turns = move(beam_rescheduled);
  }
  cout << turns.size() << '\n';
  for (const auto &turn : turns) {
    cout << turn.size() << '\n';
    for (const auto &mv : turn) {
      cout << mv.type << ' ' << mv.i << ' ' << mv.j << ' ' << mv.k << '\n';
    }
  }
  return (int)turns.size();
}

bool materialize_candidate(const Candidate &cand,
                           const BeamBuckets &source_buckets,
                           BeamBuckets &dest_buckets,
                           vector<Node> &nodes,
                           FastHashSet &seen,
                           int &best_node,
                           int &goal_node) {
#ifdef PERF_STATS
  PerfTimer timer(perf.ns_materialize);
  perf.materialize_attempts++;
#endif
  if (seen.contains(cand.hash)) {
#ifdef PERF_STATS
    perf.materialize_seen_reject++;
#endif
    return false;
  }

  const vector<BeamState> &parent_bucket =
      source_buckets[cand.parent_bi][cand.parent_bj];
  if (cand.parent_beam < 0 || cand.parent_beam >= (int)parent_bucket.size()) {
    return false;
  }

  int node_id = (int)nodes.size();
  nodes.push_back(Node{cand.parent_node,
                       cand.move,
                       cand.key,
                       cand.hash,
                       cand.macro_hash,
                       cand.turn_count,
                       cand.last_i,
                       cand.last_j,
                       cand.score});
  seen.insert(cand.hash);

  if (cand.score > nodes[best_node].score ||
      (cand.score == nodes[best_node].score &&
       better_key(cand.key, nodes[best_node].key))) {
    best_node = node_id;
  }

#ifdef PERF_STATS
  perf.materialized++;
#endif

#ifdef PERF_STATS
  long long t_copy = PerfStats::now_ns();
#endif
  State child = parent_bucket[cand.parent_beam].state;
#ifdef PERF_STATS
  perf.ns_materialize_copy += PerfStats::now_ns() - t_copy;
#endif
  apply_transition(child, cand.move, cand.turn_count);

  if (is_goal(child)) {
    goal_node = node_id;
    return true;
  }

  dest_buckets[cand.last_i + 1][cand.last_j + 1].push_back(
      BeamState{move(child), node_id});
  return false;
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  auto start_time = chrono::steady_clock::now();
  auto elapsed_ms = [&]() -> long long {
    return chrono::duration_cast<chrono::milliseconds>(
               chrono::steady_clock::now() - start_time)
        .count();
  };
  auto beam_width_for_time = [&]() -> int {
    return elapsed_ms() >= SHRINK_BEAM_AFTER_MS ? TIME_LIMIT_BEAM_WIDTH
                                                : BEAM_WIDTH;
  };
  auto pair_bucket_width_for_time = [&]() -> int {
    return elapsed_ms() >= SHRINK_BEAM_AFTER_MS ? TIME_LIMIT_PAIR_BUCKET_WIDTH
                                                : PAIR_BUCKET_WIDTH;
  };

  init_zobrist();

  int input_r;
  cin >> input_r;

  State initial;
  for (int r = 0; r < R; r++) {
    initial.dep[r].len = CARS_PER_LINE;
    for (int c = 0; c < CARS_PER_LINE; c++) {
      cin >> initial.dep[r].car[c];
    }
  }
  rebuild_state_cache(initial);

  vector<Node> nodes;
  nodes.reserve(BEAM_WIDTH * MAX_STEPS * 8 + 1);
  nodes.push_back(Node{-1, Move{-1, -1, -1, -1}, initial.key, initial.hash,
                       initial.macro_hash, initial.turn_count, initial.last_i,
                       initial.last_j, contest_score(initial)});

  BeamBuckets current_buckets{};
  current_buckets[0][0].push_back(BeamState{initial, 0});

  int best_node = 0;

  FastHashSet seen(BEAM_WIDTH * MAX_STEPS * 8 + 16);
  seen.insert(initial.hash);

  SelectorBuckets same_turn_selectors;
  SelectorBuckets next_turn_selectors;

#ifdef PERF_STATS
  auto log_perf = [&](int turn_layer, const char *phase, int start_nodes,
                      int bucket_states) {
    cerr << fixed << setprecision(3);
    cerr << "[perf] layer=" << turn_layer
         << " phase=" << phase
         << " ms=" << PerfStats::ms(perf.ns_layer)
         << " nodes+=" << ((int)nodes.size() - start_nodes)
         << " buckets=" << bucket_states
         << " expanded=" << perf.expanded_states
         << " tried=" << perf.tried_moves
         << " same=" << perf.tried_same_turn
         << " new=" << perf.tried_new_turn
         << " eval=" << perf.eval_calls
         << " seen_eval_hit=" << perf.seen_eval_hit
         << '\n';
    cerr << "[perf]   selector push=" << perf.selector_push_calls
         << " fill=" << perf.selector_fill
         << " full_replace=" << perf.selector_full_replace
         << " full_reject=" << perf.selector_full_reject
         << " macro_replace=" << perf.selector_macro_replace
         << " macro_reject=" << perf.selector_macro_reject
         << " dup_replace=" << perf.selector_dup_replace
         << " dup_reject=" << perf.selector_dup_reject
         << " sort_calls=" << perf.sort_calls
         << " sorted_items=" << perf.sorted_items
         << '\n';
    cerr << "[perf]   materialize attempts=" << perf.materialize_attempts
         << " seen_reject=" << perf.materialize_seen_reject
         << " accepted=" << perf.materialized
         << " apply_calls=" << perf.apply_calls
         << " undo_calls=" << perf.undo_calls
         << '\n';
    cerr << "[perf]   time_ms expand=" << PerfStats::ms(perf.ns_expand_states)
         << " apply_eval=" << PerfStats::ms(perf.ns_apply_eval)
         << " apply_board=" << PerfStats::ms(perf.ns_apply_board)
         << " undo=" << PerfStats::ms(perf.ns_undo)
         << " selector=" << PerfStats::ms(perf.ns_selector_push)
         << " sort=" << PerfStats::ms(perf.ns_sort)
         << " materialize=" << PerfStats::ms(perf.ns_materialize)
         << " copy=" << PerfStats::ms(perf.ns_materialize_copy)
         << '\n';
    cerr << "[perf]   apply_breakdown_ms collect="
         << PerfStats::ms(perf.ns_apply_collect)
         << " remove=" << PerfStats::ms(perf.ns_apply_remove_score)
         << " raw=" << PerfStats::ms(perf.ns_apply_raw_move)
         << " refresh=" << PerfStats::ms(perf.ns_apply_refresh)
         << " add=" << PerfStats::ms(perf.ns_apply_add_score)
         << '\n';
  };
#endif

  for (int turn_layer = 0;
       turn_layer < MAX_STEPS && has_bucket_states(current_buckets);
       turn_layer++) {
#ifdef PERF_STATS
    perf.reset();
    int perf_start_nodes = (int)nodes.size();
    long long perf_layer_start = PerfStats::now_ns();
#endif
    int pair_width = pair_bucket_width_for_time();
    reset_selector_buckets(same_turn_selectors, pair_width);
    reset_selector_buckets(next_turn_selectors, pair_width);

    int goal_node = -1;

    auto shrink_pair_buckets_if_needed = [&]() {
      int next_width = pair_bucket_width_for_time();
      if (next_width < pair_width) {
        pair_width = next_width;
        set_selector_buckets_width(same_turn_selectors, pair_width);
        set_selector_buckets_width(next_turn_selectors, pair_width);
      }
    };

    for (int bi = 0; bi < LAST_DIM; bi++) {
      for (int bj = 0; bj < LAST_DIM; bj++) {
        shrink_pair_buckets_if_needed();

        vector<Candidate> pending_same =
            same_turn_selectors[bi][bj].sorted();
        for (const Candidate &cand : pending_same) {
          if (materialize_candidate(cand,
                                    current_buckets,
                                    current_buckets,
                                    nodes,
                                    seen,
                                    best_node,
                                    goal_node)) {
#ifdef PERF_STATS
            perf.ns_layer = PerfStats::now_ns() - perf_layer_start;
            log_perf(turn_layer, "goal_same", perf_start_nodes,
                     count_bucket_states(current_buckets));
#endif
	            int output_turns = output_answer(nodes, goal_node);
	            cerr << 5000 - output_turns << '\n';
            return 0;
          }
        }

        vector<BeamState> &bucket = current_buckets[bi][bj];
        for (int b = 0; b < (int)bucket.size(); b++) {
          shrink_pair_buckets_if_needed();

#ifdef PERF_STATS
          perf.expanded_states++;
          PerfTimer expand_timer(perf.ns_expand_states);
#endif
          State &cur = bucket[b].state;
          if (is_goal(cur)) {
#ifdef PERF_STATS
            perf.ns_layer = PerfStats::now_ns() - perf_layer_start;
            log_perf(turn_layer, "goal_existing", perf_start_nodes,
                     count_bucket_states(current_buckets));
#endif
	            int output_turns = output_answer(nodes, bucket[b].node_id);
	            cerr << 5000 - output_turns << '\n';
            return 0;
          }

          const Key parent_key = cur.key;

	          auto try_move = [&](const Move &mv, bool same_turn,
	                              int next_turn_count,
	                              const EvalResult &ev) {
#ifdef PERF_STATS
	            perf.tried_moves++;
	            if (same_turn) {
	              perf.tried_same_turn++;
	            } else {
	              perf.tried_new_turn++;
	            }
#endif
#ifdef LOCAL_DEBUG
	            TurnSnapshot snap;
	            apply_for_eval(cur, mv, next_turn_count, snap);
            State chk = recompute_state_for_check(cur);
            if (!same_key(chk.key, cur.key) || chk.hash != cur.hash ||
                chk.macro_hash != cur.macro_hash ||
                chk.macro_edge_state != cur.macro_edge_state) {
              cerr << "cache mismatch after eval move "
                   << mv.type << ' ' << mv.i << ' ' << mv.j << ' ' << mv.k << '\n';
              exit(1);
            }
            if (!same_key(ev.key, cur.key) || ev.hash != cur.hash ||
                ev.macro_hash != cur.macro_hash ||
                ev.score != partial_score_fast(cur)) {
              cerr << "fast eval mismatch "
                   << mv.type << ' ' << mv.i << ' ' << mv.j << ' ' << mv.k << '\n';
              exit(1);
            }
            undo_eval(cur, mv, snap);
            State back = recompute_state_for_check(cur);
            if (!same_key(back.key, cur.key) || back.hash != cur.hash ||
                back.macro_hash != cur.macro_hash ||
                back.macro_edge_state != cur.macro_edge_state) {
              cerr << "cache mismatch after undo\n";
              exit(1);
            }
#endif

            unsigned long long h = ev.hash;
            bool already_seen = seen.contains(h);
            if (!already_seen) {
              Candidate cand{ev.key,
                             bi,
                             bj,
                             b,
                             bucket[b].node_id,
                             mv,
                             h,
	                             ev.macro_hash,
	                             next_turn_count,
	                             mv.i,
	                             mv.j,
	                             ev.score,
	                             key_quality_ge(ev.key, parent_key),
	                             same_turn};
              set_candidate_rank(cand);
              Selector &target_selector =
                  same_turn ? same_turn_selectors[mv.i + 1][mv.j + 1]
                            : next_turn_selectors[mv.i + 1][mv.j + 1];
              target_selector.push(cand);
            } else {
#ifdef PERF_STATS
              perf.seen_eval_hit++;
#endif
            }
          };

          for (int i = 0; i < R; i++) {
            int len = cur.dep[i].len;
            for (int j = 0; j < R; j++) {
              int max_k = min(len, SIDING_CAP - cur.sid[j].len);
              bool same_turn =
                  cur.turn_count > 0 && cur.last_i < i && cur.last_j < j;
              int next_turn_count = cur.turn_count + (same_turn ? 0 : 1);
              int line_delta = 0;
              for (int k = 1; k <= max_k; k++) {
	                int row = cur.dep[i].car[len - k] / 10;
	                line_delta +=
	                    line_proximity_value(row, j) - line_proximity_value(row, i);
	                EvalResult ev =
	                    evaluate_move_fast_typed<0>(cur, i, j, k, line_delta);
	                try_move(Move{0, i, j, k}, same_turn, next_turn_count, ev);
	              }
	            }
	          }

          for (int j = 0; j < R; j++) {
            int len = cur.sid[j].len;
            for (int i = 0; i < R; i++) {
              int max_k = min(len, MAIN_CAP - cur.dep[i].len);
              bool same_turn =
                  cur.turn_count > 0 && cur.last_i < i && cur.last_j < j;
              int next_turn_count = cur.turn_count + (same_turn ? 0 : 1);
              int line_delta = 0;
              for (int k = 1; k <= max_k; k++) {
	                int row = cur.sid[j].car[k - 1] / 10;
	                line_delta +=
	                    line_proximity_value(row, i) - line_proximity_value(row, j);
	                EvalResult ev =
	                    evaluate_move_fast_typed<1>(cur, i, j, k, line_delta);
	                try_move(Move{1, i, j, k}, same_turn, next_turn_count, ev);
	              }
	            }
	          }
        }
      }
    }

    Selector next_turn_selector(beam_width_for_time());
    for (int bi = 0; bi < LAST_DIM; bi++) {
      for (int bj = 0; bj < LAST_DIM; bj++) {
        vector<Candidate> bucket_candidates =
            next_turn_selectors[bi][bj].sorted();
        for (const Candidate &cand : bucket_candidates) {
          next_turn_selector.push(cand);
        }
      }
    }

    vector<Candidate> next_candidates = next_turn_selector.sorted();
    if (next_candidates.empty()) {
#ifdef PERF_STATS
      perf.ns_layer = PerfStats::now_ns() - perf_layer_start;
      log_perf(turn_layer, "empty_next", perf_start_nodes,
               count_bucket_states(current_buckets));
#endif
      break;
    }

    BeamBuckets next_buckets{};
    for (const Candidate &cand : next_candidates) {
      if (materialize_candidate(cand,
                                current_buckets,
                                next_buckets,
                                nodes,
                                seen,
                                best_node,
                                goal_node)) {
#ifdef PERF_STATS
        perf.ns_layer = PerfStats::now_ns() - perf_layer_start;
        log_perf(turn_layer, "goal_next", perf_start_nodes,
                 count_bucket_states(next_buckets));
#endif
	        int output_turns = output_answer(nodes, goal_node);
	        cerr << 5000 - output_turns << '\n';
        return 0;
      }
    }

    current_buckets = move(next_buckets);
#ifdef PERF_STATS
    perf.ns_layer = PerfStats::now_ns() - perf_layer_start;
    log_perf(turn_layer, "continue", perf_start_nodes,
             count_bucket_states(current_buckets));
#endif

#ifdef LOCAL_DEBUG
    if ((turn_layer + 1) % 10 == 0) {
      const State *dbg = nullptr;
      for (int bi = 0; bi < LAST_DIM && dbg == nullptr; bi++) {
        for (int bj = 0; bj < LAST_DIM && dbg == nullptr; bj++) {
          if (!current_buckets[bi][bj].empty()) {
            dbg = &current_buckets[bi][bj][0].state;
          }
        }
      }
      if (dbg != nullptr) {
        cerr << "turn_layer=" << (turn_layer + 1)
             << " T=" << dbg->turn_count
             << " structure=" << dbg->key.structure
             << " ready=" << dbg->key.ready_edges
             << " open=" << dbg->key.open_edges
             << " score=" << contest_score(*dbg)
             << '\n';
      }
    }
#endif
  }

	  output_answer(nodes, best_node);
  cerr << nodes[best_node].score << '\n';
  return 0;
}
