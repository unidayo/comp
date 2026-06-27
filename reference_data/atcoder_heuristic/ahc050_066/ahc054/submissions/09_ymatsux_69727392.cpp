#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <queue>
#include <ranges>
#include <string>
#include <vector>

using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
using int32 = int32_t;
using uint32 = uint32_t;
using int64 = int64_t;
using uint64 = uint64_t;
using int128 = __int128_t;
using uint128 = __uint128_t;

// Policy: Don't overuse using. Limit its use to extremely common types and
// functions.
using std::array;
using std::optional;
using std::nullopt;
using std::pair;
using std::string;
using std::tuple;
using std::vector;
using std::views::iota;

#define UNEXPECTED()                                                 \
  {                                                                  \
    std::cerr << "Unexpected@" << __FILE__ << ":" << __LINE__ << "!" \
              << std::endl;                                          \
    exit(1);                                                         \
  }

// Override operator<< for ostream to handle vector.
template <typename T>
std::ostream &operator<<(std::ostream &os, const vector<T> &v) {
  os << "[";
  for (const int i : iota(0, int(v.size()))) {
    if (i > 0)
      os << ", ";
    os << v[i];
  }
  os << "]";
  return os;
}

template <std::integral T>
constexpr auto closed_iota(T first, T last) {
  return std::views::iota(first, last + 1);
}

class Timer {
public:
  Timer() : start_(std::chrono::steady_clock::now()) {}

  int64 CurrentMs() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_)
        .count();
  }

private:
  std::chrono::time_point<std::chrono::steady_clock> start_;
};
Timer g_timer;

class Random {
 public:
  Random() : state_(kInitialState) {}

  double RandomDouble(double a, double b) {
    Update();
    const double r = state_ * kScale_;
    return a + (b - a) * r;
  }

  int RandomInt(int a, int b) {
    return static_cast<int>(RandomDouble(a, b));
  }

  int RandomIntInclusive(int a, int b) {
    return RandomInt(a, b + 1);
  }

  void Reset() {
    state_ = kInitialState;
  }

 private:
  static constexpr uint64 kInitialState = 3141592653589793238ULL;
  uint64 state_;

  // 2^(-64)
  static constexpr double kScale_ = 1.0 / 18446744073709551616.0;

  void Update() {
    // xorshift
    state_ ^= state_ >> 13;
    state_ ^= state_ << 7;
    state_ ^= state_ >> 17;
  }
};
Random g_random;

double ExpInterpolation(const double x0, const double y0, const double x1,
                        const double y1, const double x) {
  const double log_y0 = std::log(y0);
  const double log_y1 = std::log(y1);
  const double log_y = log_y0 + (x - x0) * (log_y1 - log_y0) / (x1 - x0);
  return std::exp(log_y);
}

double LinearInterpolation(const double x0, const double y0, const double x1,
                           const double y1, const double x) {
  return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

#pragma once



template <typename T>
struct Matrix {
 private:
  int r_size_;
  int c_size_;
  std::vector<T> data_;

 public:
  int r_size() const { return r_size_; }
  int c_size() const { return c_size_; }
  Matrix(const int r_size, const int c_size)
      : r_size_(r_size), c_size_(c_size), data_(r_size * c_size) {}
  Matrix(const int r_size, const int c_size, const T& value)
      : r_size_(r_size), c_size_(c_size), data_(r_size * c_size, value) {}
  Matrix() : r_size_(0), c_size_(0), data_() {}
  T& operator[](const int r, const int c) { return data_[r * c_size_ + c]; }
  const T& operator[](const int r, const int c) const {
    return data_[r * c_size_ + c];
  }
  void Fill(const T& value) {
    std::fill(data_.begin(), data_.end(), value);
  }
};

#pragma once

#include <algorithm>
#include <deque>
#include <vector>


#pragma once




int g_grid_size = 20;

bool g_is_deterministic_mode = false;

enum class Direction { kUp = 0, kDown = 1, kLeft = 2, kRight = 3 };

std::ostream& operator<<(std::ostream& os, const Direction& direction) {
  os << "UDLR"[static_cast<int>(direction)];
  return os;
}

constexpr array<Direction, 4> kDirections = {
    Direction::kUp, Direction::kDown, Direction::kLeft, Direction::kRight};

struct Cell {
  int r;
  int c;
  Cell(const int r, const int c) : r(r), c(c) {}
  Cell() : r(-1), c(-1) {}

  Cell AdjacentCell(const Direction& direction) const {
    constexpr array<int, 4> drs = {-1, 1, 0, 0};
    constexpr array<int, 4> dcs = {0, 0, -1, 1};
    return Cell(r + drs[static_cast<int>(direction)],
                c + dcs[static_cast<int>(direction)]);
  }

  bool InGrid() const {
    return 0 <= r && r < g_grid_size && 0 <= c && c < g_grid_size;
  }

  bool operator==(const Cell& other) const {
    return r == other.r && c == other.c;
  }

  int Distance(const Cell& other) const {
    return std::abs(r - other.r) + std::abs(c - other.c);
  }
};

// Override operator<< for ostream to handle vector.
std::ostream& operator<<(std::ostream& os, const Cell& cell) {
  os << "(" << cell.r << ", " << cell.c << ")";
  return os;
}

enum class CellState { kOpen = 0, kTree = 1, kTreant = 2, kUndecided = 3 };

int CountAdjacentOpenCells(const Matrix<CellState>& map, const Cell& cell) {
  int count = 0;
  for (const Direction& direction : kDirections) {
    const Cell adjacent_cell = cell.AdjacentCell(direction);
    if (adjacent_cell.InGrid() &&
        map[adjacent_cell.r, adjacent_cell.c] == CellState::kOpen) {
      count++;
    }
  }
  return count;
}

struct Rectangle {
  // [r_begin, r_end) x [c_begin, c_end)
  int r_begin, c_begin, r_end, c_end;
  Rectangle(const int r_begin, const int c_begin, const int r_end,
            const int c_end)
      : r_begin(r_begin), c_begin(c_begin), r_end(r_end), c_end(c_end) {}
  Rectangle() : r_begin(0), c_begin(0), r_end(0), c_end(0) {}
  bool Intersects(const Rectangle& other) const {
    return r_begin < other.r_end && r_end > other.r_begin &&
           c_begin < other.c_end && c_end > other.c_begin;
  }
};

struct LayoutParams {
  int left_street_column = -1;
  int right_street_column = -1;
  int main_path_c_min = -1;
  int main_path_c_max = -1;
};

#pragma once




bool VerifyCompactGoalCorridor(Matrix<CellState>& map,
                               const vector<Cell>& goal_corridor,
                              const LayoutParams& layout_params) {
  auto maybe_make_open_cell = [&]() -> void {
    const int dr = goal_corridor[2].r - goal_corridor[0].r;
    const int dc = goal_corridor[2].c - goal_corridor[0].c;
    const Cell cell_0 = Cell(goal_corridor[0].r - dr, goal_corridor[0].c - dc);
    if (layout_params.left_street_column < cell_0.c &&
        cell_0.c < layout_params.main_path_c_min) {
      for (const int c :
           closed_iota(layout_params.left_street_column + 1, cell_0.c)) {
        if (map[cell_0.r, c] == CellState::kUndecided) {
          map[cell_0.r, c] = CellState::kOpen;
        }
      }
    } else if (layout_params.main_path_c_max < cell_0.c &&
      cell_0.c < layout_params.right_street_column) {
      for (const int c :
           closed_iota(cell_0.c, layout_params.right_street_column - 1)) {
        if (map[cell_0.r, c] == CellState::kUndecided) {
          map[cell_0.r, c] = CellState::kOpen;
        }
      }
    } else if (cell_0.c < layout_params.left_street_column - 1) {
      for (const int c :
           closed_iota(cell_0.c, layout_params.left_street_column - 1)) {
        if (map[cell_0.r, c] == CellState::kUndecided) {
          map[cell_0.r, c] = CellState::kOpen;
        }
      }
    } else if (cell_0.c > layout_params.right_street_column + 1) {
      for (const int c :
           closed_iota(layout_params.right_street_column + 1, cell_0.c)) {
        if (map[cell_0.r, c] == CellState::kUndecided) {
          map[cell_0.r, c] = CellState::kOpen;
        }
      }
    }
  };
  maybe_make_open_cell();

  Matrix<CellState> tmp_map = map;

  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(0, g_grid_size)) {
      if (tmp_map[r, c] == CellState::kUndecided) {
        tmp_map[r, c] = CellState::kTreant;
      }
    }
  }

  const Cell s(0, g_grid_size / 2);
  const Cell t = goal_corridor[2];
  vector<Cell> path;

  // Lambda: find shortest path from s to t by BFS over kOpen cells
  auto find_st_path = [&](const Cell& s_cell, const Cell& t_cell) -> vector<Cell> {
    if (!s_cell.InGrid() || !t_cell.InGrid()) return {};
    if (tmp_map[s_cell.r, s_cell.c] != CellState::kOpen) return {};
    if (tmp_map[t_cell.r, t_cell.c] != CellState::kOpen) return {};

    Matrix<int> dist(g_grid_size, g_grid_size, -1);
    Matrix<Cell> prev(g_grid_size, g_grid_size, Cell(-1, -1));
    std::queue<Cell> q;

    dist[s_cell.r, s_cell.c] = 0;
    q.push(s_cell);

    while (!q.empty()) {
      const Cell cur = q.front();
      q.pop();
      if (cur == t_cell) break;
      for (const Direction dir : kDirections) {
        const Cell nxt = cur.AdjacentCell(dir);
        if (!nxt.InGrid()) continue;
        if (tmp_map[nxt.r, nxt.c] != CellState::kOpen) continue;
        if (dist[nxt.r, nxt.c] != -1) continue;
        dist[nxt.r, nxt.c] = dist[cur.r, cur.c] + 1;
        prev[nxt.r, nxt.c] = cur;
        q.push(nxt);
      }
    }

    if (dist[t_cell.r, t_cell.c] == -1) {
      return {};
    }

    vector<Cell> result;
    for (Cell cur = t_cell; !(cur == s_cell); cur = prev[cur.r, cur.c]) {
      result.push_back(cur);
    }
    result.push_back(s_cell);
    std::reverse(result.begin(), result.end());
    return result;
  };

  path = find_st_path(s, t);
  if (path.empty()) {
    return false;
  }

  Matrix<int8> seen_table(g_grid_size, g_grid_size, 0);
  auto mark_visible_from = [&](const Cell& p) {
    // Current cell is visible
    seen_table[p.r, p.c] = 1;
    // Cast rays in 4 directions until hitting Tree/Treant or going out of grid
    for (const Direction dir : kDirections) {
      Cell cur = p;
      while (true) {
        cur = cur.AdjacentCell(dir);
        if (!cur.InGrid()) break;
        seen_table[cur.r, cur.c] = 1;
        const CellState cs = tmp_map[cur.r, cur.c];
        if (cs == CellState::kTree || cs == CellState::kTreant) break;
      }
    }
  };

  for (const Cell& p : path) {
    mark_visible_from(p);
  }

  // int seen_count = 0;
  // for (const int r : iota(0, g_grid_size)) {
  //   for (const int c : iota(0, g_grid_size)) {
  //     if (seen_table[r, c] == 1) {
  //       seen_count++;
  //     }
  //   }
  // }
  // std::cerr << "seen_count: " << seen_count << std::endl;

  bool valid = true;
  for (const Cell& cell : {goal_corridor[0], goal_corridor[1]}) {
    for (const Direction& direction : kDirections) {
      const Cell adj_cell = cell.AdjacentCell(direction);
      if (!adj_cell.InGrid()) continue;
      if (tmp_map[adj_cell.r, adj_cell.c] == CellState::kOpen) continue;
      if (seen_table[adj_cell.r, adj_cell.c] == 0) {
        valid = false;
        break;
      }
    }
  }

  // The goal was visible somehow.
  if (seen_table[goal_corridor[0].r, goal_corridor[0].c]) return false;

  return valid;
}

#pragma once




vector<Cell> FindIsolatedPathToTargetSetInternal(
    const Matrix<CellState>& map, const Cell& goal_cell,
    const Cell& source_cell, const vector<Cell>& target_cells) {
  if (!source_cell.InGrid()) return {};
  if (map[source_cell.r, source_cell.c] == CellState::kTree) return {};

  Matrix<int8> is_target(g_grid_size, g_grid_size, 0);
  for (const Cell& t : target_cells) {
    if (!t.InGrid()) continue;
    if (map[t.r, t.c] == CellState::kTree) continue;
    is_target[t.r, t.c] = 1;
  }

  if (is_target[source_cell.r, source_cell.c]) {
    return {source_cell};
  }

  Matrix<int8> visited(g_grid_size, g_grid_size, 0);
  Matrix<int> distance_table(g_grid_size, g_grid_size, -1);
  Matrix<Cell> parent(g_grid_size, g_grid_size, Cell(-1, -1));
  std::deque<Cell> q;
  q.push_back(source_cell);
  visited[source_cell.r, source_cell.c] = 1;
  distance_table[source_cell.r, source_cell.c] = 0;

  const Cell start_cell(0, g_grid_size / 2);

  Cell target(-1, -1);
  int best_target_distance = -1;
  while (!q.empty()) {
    const Cell cur = q.front();
    q.pop_front();
    if (best_target_distance != -1 &&
        distance_table[cur.r, cur.c] >= best_target_distance) {
      break;
    }
    for (const Direction& d : {Direction::kDown, Direction::kLeft,
                               Direction::kRight, Direction::kUp}) {
      const Cell nxt = cur.AdjacentCell(d);
      if (nxt.Distance(start_cell) <= 2) continue;

      if (!nxt.InGrid()) continue;
      if (map[nxt.r, nxt.c] != CellState::kUndecided) continue;
      bool invalid = false;
      if (nxt != source_cell) {
        for (const Direction& direction : kDirections) {
          const Cell adjacent_cell = nxt.AdjacentCell(direction);
          if (!adjacent_cell.InGrid()) continue;
          if (map[adjacent_cell.r, adjacent_cell.c] == CellState::kOpen &&
              adjacent_cell != source_cell) {
            invalid = true;
            break;
          }
        }
      }
      if (invalid) continue;
      const int candidate_distance = distance_table[cur.r, cur.c] + 1;
      if (distance_table[nxt.r, nxt.c] == -1) {
        visited[nxt.r, nxt.c] = 1;
        parent[nxt.r, nxt.c] = cur;
        distance_table[nxt.r, nxt.c] = candidate_distance;
        if (is_target[nxt.r, nxt.c]) {
          target = nxt;
          if (best_target_distance == -1)
            best_target_distance = candidate_distance;
        }
        q.push_back(nxt);
      } else if (distance_table[nxt.r, nxt.c] == candidate_distance) {
        // Tie on shortest distance: choose the parent closer to goal_cell.
        const Cell prev_parent = parent[nxt.r, nxt.c];
        if (prev_parent.r == -1 ||
            cur.Distance(goal_cell) < prev_parent.Distance(goal_cell)) {
          parent[nxt.r, nxt.c] = cur;
        }
      }
    }
  }

  if (target.r == -1) return {};

  vector<Cell> path;
  for (Cell cur = target; cur.r != -1; cur = parent[cur.r, cur.c]) {
    path.push_back(cur);
  }
  std::reverse(path.begin(), path.end());
  return path;
}

optional<vector<Cell>> MakeGoalCorridor(
    const Matrix<CellState>& map, const Cell& goal_cell,
    [[maybe_unused]] const LayoutParams& layout_params) {
  vector<vector<Direction>> patterns = {
      {Direction::kRight, Direction::kUp},
      {Direction::kLeft, Direction::kUp},
      {Direction::kDown, Direction::kRight},
      {Direction::kDown, Direction::kLeft},
      {Direction::kUp, Direction::kRight},
      {Direction::kUp, Direction::kLeft},
      {Direction::kRight, Direction::kDown},
      {Direction::kLeft, Direction::kDown},
  };

  auto get_remaining_visibility_target_cells =
      [&](const vector<Cell>& visibility_target_cells, const vector<Cell>& path,
          const Matrix<CellState>& current_map) -> vector<Cell> {
    vector<Cell> remaining_visibility_target_cells;
    for (const Cell& visibility_target_cell : visibility_target_cells) {
      bool visible = false;
      for (const Cell& path_cell : path) {
        for (const Direction& direction : kDirections) {
          Cell current_cell = path_cell;
          while (true) {
            current_cell = current_cell.AdjacentCell(direction);
            if (!current_cell.InGrid()) break;
            if (current_cell == visibility_target_cell) {
              visible = true;
            }
            if (current_map[current_cell.r, current_cell.c] !=
                CellState::kOpen) {
              break;
            }
          }
        }
      }

      if (!visible) {
        remaining_visibility_target_cells.push_back(visibility_target_cell);
      }
    }
    return remaining_visibility_target_cells;
  };

  std::vector<Cell> best_path;
  for (const auto& pattern : patterns) {
    Matrix<CellState> tmp_map = map;
    tmp_map[goal_cell.r, goal_cell.c] = CellState::kOpen;

    Cell curr_cell = goal_cell;
    bool invalid = false;
    vector<Cell> path = {goal_cell};
    for (const Direction& direction : pattern) {
      curr_cell = curr_cell.AdjacentCell(direction);
      if (!curr_cell.InGrid()) {
        invalid = true;
        break;
      }
      if (tmp_map[curr_cell.r, curr_cell.c] != CellState::kUndecided) {
        invalid = true;
        break;
      }
      tmp_map[curr_cell.r, curr_cell.c] = CellState::kOpen;
      path.push_back(curr_cell);
    }
    if (invalid) continue;

    vector<Cell> target_visibility_cells = [&]() -> vector<Cell> {
      std::deque<Cell> q;

      Matrix<int8> q_pushed_table(g_grid_size, g_grid_size, 0);
      Matrix<int8> is_passable_table(g_grid_size, g_grid_size, 0);
      for (const int r : iota(0, g_grid_size)) {
        for (const int c : iota(0, g_grid_size)) {
          if (tmp_map[r, c] == CellState::kUndecided) {
            is_passable_table[r, c] = 1;
          }
        }
      }

      for (const int i : iota(0, std::ssize(pattern))) {
        for (const Direction& direction : kDirections) {
          const Cell adj_cell = path[i].AdjacentCell(direction);
          if (!adj_cell.InGrid()) continue;
          if (tmp_map[adj_cell.r, adj_cell.c] == CellState::kOpen) continue;
          if (q_pushed_table[adj_cell.r, adj_cell.c]) continue;
          q.push_back(adj_cell);
          q_pushed_table[adj_cell.r, adj_cell.c] = 1;
          // target visibility cells are not passable.
          is_passable_table[adj_cell.r, adj_cell.c] = 0;
        }
      }

      vector<Cell> finalized_target_visibility_cells;
      while (!q.empty()) {
        const Cell cell = q.front();
        q.pop_front();
        bool valid = false;
        for (const Direction& direction : kDirections) {
          const Cell adj_cell = cell.AdjacentCell(direction);
          if (!adj_cell.InGrid()) continue;
          if (is_passable_table[adj_cell.r, adj_cell.c]) {
            valid = true;
            break;
          }
        }
        if (valid) {
          finalized_target_visibility_cells.push_back(cell);
          is_passable_table[cell.r, cell.c] = 0;
        } else {
          for (const Direction& direction : kDirections) {
            const Cell adj_cell = cell.AdjacentCell(direction);
            if (!adj_cell.InGrid()) continue;
            if (q_pushed_table[adj_cell.r, adj_cell.c]) continue;
            if (tmp_map[adj_cell.r, adj_cell.c] == CellState::kTree) {
              q.push_back(adj_cell);
              q_pushed_table[adj_cell.r, adj_cell.c] = 1;
            }
          }
        }
      }
      return finalized_target_visibility_cells;
    }();

    // for (const int i : iota(0, 2)) {
    //   for (const Direction& direction : kDirections) {
    //     const Cell adj_cell = path[i].AdjacentCell(direction);
    //     if (!adj_cell.InGrid()) continue;
    //     if (tmp_map[adj_cell.r, adj_cell.c] == CellState::kOpen) continue;
    //     target_visibility_cells.push_back(adj_cell);
    //   }
    // }

    vector<Cell> visibility_path = {path.back()};
    target_visibility_cells = get_remaining_visibility_target_cells(
        target_visibility_cells, visibility_path, tmp_map);

    while (!target_visibility_cells.empty()) {
      vector<Cell> target_cells;
      for (const Cell& cell : target_visibility_cells) {
        for (const Direction& direction : kDirections) {
          const Cell adj_cell = cell.AdjacentCell(direction);
          if (!adj_cell.InGrid()) continue;
          if (tmp_map[adj_cell.r, adj_cell.c] != CellState::kUndecided)
            continue;
          target_cells.push_back(adj_cell);
        }
      }
      vector<Cell> path_segment = FindIsolatedPathToTargetSetInternal(
          tmp_map, goal_cell, path.back(), target_cells);
      if (path_segment.empty()) {
        invalid = true;
        break;
      }
      // Remove the first element of path_segment
      path_segment.erase(path_segment.begin());

      path.insert(path.end(), path_segment.begin(), path_segment.end());
      visibility_path.insert(visibility_path.end(), path_segment.begin(),
                             path_segment.end());
      for (const Cell& cell : path_segment) {
        tmp_map[cell.r, cell.c] = CellState::kOpen;
      }
      target_visibility_cells = get_remaining_visibility_target_cells(
          target_visibility_cells, visibility_path, tmp_map);
    }
    if (invalid) continue;

    if (path.size() <= 16) {
      return path;
    }
    if (best_path.empty() || path.size() < best_path.size()) {
      best_path = path;
    }
  }

  // TODO: Replace "24" with a proper condition.
  if (!best_path.empty() && best_path.size() <= 24) {
    return best_path;
  }
  return nullopt;
}

optional<vector<Cell>> MakeCompactGoalCorridor(
    const Matrix<CellState>& map, const Cell& goal_cell,
    [[maybe_unused]] const LayoutParams& layout_params) {
  vector<vector<Direction>> patterns;
  // {
  //     {Direction::kRight, Direction::kUp},
  //     {Direction::kLeft, Direction::kUp},
  //     {Direction::kDown, Direction::kRight},
  //     {Direction::kDown, Direction::kLeft},
  //     {Direction::kUp, Direction::kRight},
  //     {Direction::kUp, Direction::kLeft},
  //     {Direction::kRight, Direction::kDown},
  //     {Direction::kLeft, Direction::kDown},
  // };
  {
    constexpr Direction U = Direction::kUp;
    constexpr Direction D = Direction::kDown;
    constexpr Direction L = Direction::kLeft;
    constexpr Direction R = Direction::kRight;
    if (goal_cell.c <= g_grid_size / 2) {
      if (goal_cell.c >= layout_params.main_path_c_min - 1) {
        patterns = {{L, D, L}, {D, L, D}, {D, R, D}};
      } else if (goal_cell.c > layout_params.left_street_column) {
        patterns = {{U, R, U}, {R, U, R}};
      } else {
        patterns = {{U, L, U}, {L, U, L}};
      }
    } else {
      if (goal_cell.c <= layout_params.main_path_c_max + 1) {
        patterns = {{R, D, R}, {D, R, D}, {D, L, D}};
      } else if (goal_cell.c < layout_params.right_street_column) {
        patterns = {{U, L, U}, {L, U, L}};
      } else {
        patterns = {{U, R, U}, {R, U, R}};
      }
    }
  }

  // std::vector<Cell> best_path;
  for (const auto& pattern : patterns) {
    Matrix<CellState> tmp_map = map;
    tmp_map[goal_cell.r, goal_cell.c] = CellState::kOpen;

    Cell curr_cell = goal_cell;
    bool invalid = false;
    vector<Cell> path = {goal_cell};
    for (const Direction& direction : pattern) {
      curr_cell = curr_cell.AdjacentCell(direction);
      if (!curr_cell.InGrid()) {
        invalid = true;
        break;
      }
      if (tmp_map[curr_cell.r, curr_cell.c] != CellState::kUndecided) {
        invalid = true;
        break;
      }
      tmp_map[curr_cell.r, curr_cell.c] = CellState::kOpen;
      path.push_back(curr_cell);
    }
    if (invalid) continue;

    if (layout_params.left_street_column + 2 < path.back().c &&
           path.back().c < layout_params.main_path_c_min) {
      vector<Cell> target_cells;
      for (const int r : iota(0, g_grid_size)) {
        const Cell cell(r, layout_params.left_street_column + 2);
        if (tmp_map[cell.r, cell.c] != CellState::kUndecided) continue;
        if (CountAdjacentOpenCells(tmp_map, cell) > 0) continue;
        target_cells.push_back(cell);
      }
      vector<Cell> path_segment = FindIsolatedPathToTargetSetInternal(
          tmp_map, goal_cell, path.back(), target_cells);
      if (std::ssize(path_segment) <=
          std::abs(path.back().c - (layout_params.left_street_column + 2)) +
              3) {
        path.insert(path.end(), path_segment.begin(), path_segment.end());
      }
    }

    if (layout_params.right_street_column - 2 > path.back().c &&
        path.back().c > layout_params.main_path_c_max) {
      vector<Cell> target_cells;
      for (const int r : iota(0, g_grid_size)) {
        const Cell cell(r, layout_params.right_street_column - 2);
        if (tmp_map[cell.r, cell.c] != CellState::kUndecided) continue;
        if (CountAdjacentOpenCells(tmp_map, cell) > 0) continue;
        target_cells.push_back(cell);
      }
      vector<Cell> path_segment = FindIsolatedPathToTargetSetInternal(
          tmp_map, goal_cell, path.back(), target_cells);
      if (std::ssize(path_segment) <=
          std::abs(path.back().c - (layout_params.right_street_column - 2)) +
              3) {
        path.insert(path.end(), path_segment.begin(), path_segment.end());
      }
    }

    return path;
  }

  return nullopt;
}

#pragma once





namespace main_path_internal {

vector<Cell> FindIsolatedPathToTargetSet(const Matrix<CellState>& map,
                                         const Cell& source_cell,
                                         const vector<Cell>& target_cells) {
  if (!source_cell.InGrid()) return {};
  if (map[source_cell.r, source_cell.c] == CellState::kTree) return {};

  Matrix<int8> is_target(g_grid_size, g_grid_size, 0);
  for (const Cell& t : target_cells) {
    if (!t.InGrid()) continue;
    if (map[t.r, t.c] == CellState::kTree) continue;
    is_target[t.r, t.c] = 1;
  }

  if (is_target[source_cell.r, source_cell.c]) {
    return {source_cell};
  }

  Matrix<int8> visited(g_grid_size, g_grid_size, 0);
  Matrix<Cell> parent(g_grid_size, g_grid_size, Cell(-1, -1));
  std::deque<Cell> q;
  q.push_back(source_cell);
  visited[source_cell.r, source_cell.c] = 1;

  Cell target(-1, -1);
  while (!q.empty()) {
    const Cell cur = q.front();
    q.pop_front();
    for (const Direction& d : {Direction::kDown, Direction::kLeft,
                               Direction::kRight, Direction::kUp}) {
      const Cell nxt = cur.AdjacentCell(d);
      if (!nxt.InGrid()) continue;
      if (is_target[nxt.r, nxt.c]) {
        visited[nxt.r, nxt.c] = 1;
        parent[nxt.r, nxt.c] = cur;
        if (is_target[nxt.r, nxt.c]) {
          target = nxt;
          q.clear();
          break;
        }
      }
      if (map[nxt.r, nxt.c] != CellState::kUndecided) continue;
      if (visited[nxt.r, nxt.c]) continue;
      bool invalid = false;
      if (nxt != source_cell) {
        for (const Direction& direction : kDirections) {
          const Cell adjacent_cell = nxt.AdjacentCell(direction);
          if (!adjacent_cell.InGrid()) continue;
          if (map[adjacent_cell.r, adjacent_cell.c] == CellState::kOpen &&
              adjacent_cell != source_cell &&
              !is_target[adjacent_cell.r, adjacent_cell.c]) {
            invalid = true;
            break;
          }
        }
      }
      if (invalid) continue;
      visited[nxt.r, nxt.c] = 1;
      parent[nxt.r, nxt.c] = cur;
      q.push_back(nxt);
    }
  }

  if (target.r == -1) return {};

  vector<Cell> path;
  for (Cell cur = target; cur.r != -1; cur = parent[cur.r, cur.c]) {
    path.push_back(cur);
  }
  std::reverse(path.begin(), path.end());
  return path;
}

}  // namespace main_path_internal

double AnnealingThreshold(const double temperature) {
  return std::max(-3.0 * temperature,
                  std::log(g_random.RandomDouble(0.0, 1.0)) * temperature);
}

int FillDistanceTable(const Matrix<int8>& block_table, const Cell& s,
                      const Cell& t, Matrix<int>& dist) {
  dist.Fill(-1);

  dist[s.r, s.c] = 0;

  // Use vector with head index as a queue to avoid extra includes.
  static vector<Cell> q;
  q.clear();
  q.reserve(g_grid_size * g_grid_size);
  q.push_back(s);
  int head = 0;

  while (head < std::ssize(q)) {
    const Cell curr = q[head++];
    for (const Direction& d : kDirections) {
      const Cell next = curr.AdjacentCell(d);
      if (!next.InGrid()) continue;
      if (block_table[next.r, next.c] > 0) continue;
      if (dist[next.r, next.c] != -1) continue;
      dist[next.r, next.c] = dist[curr.r, curr.c] + 1;
      // if (next == t) return dist[next.r, next.c];
      q.push_back(next);
    }
  }

  return dist[t.r, t.c];
}

Matrix<int> DistanceTable(const Matrix<int8>& block_table, const Cell& s) {
  Matrix<int> distance_table(g_grid_size, g_grid_size, -1);
  distance_table[s.r, s.c] = 0;

  // Use vector with head index as a queue to avoid extra includes.
  vector<Cell> q;
  q.reserve(g_grid_size * g_grid_size);
  q.push_back(s);
  size_t head = 0;

  while (head < q.size()) {
    const Cell curr = q[head++];
    for (const Direction& d : kDirections) {
      const Cell next = curr.AdjacentCell(d);
      if (!next.InGrid()) continue;
      if (block_table[next.r, next.c] > 0) continue;
      if (distance_table[next.r, next.c] != -1) continue;
      distance_table[next.r, next.c] = distance_table[curr.r, curr.c] + 1;
      q.push_back(next);
    }
  }

  return distance_table;
}

void RunOptimization(const Cell& start_cell, const int c_min, const int c_max,
                     const Matrix<int8> fixed_block_table, const Cell& s,
                     const Cell& t, const int time_limit_ms,
                     Matrix<int8>& block_table) {
  Matrix<int> distance_table(g_grid_size, g_grid_size, -1);
  Matrix<int> new_distance_table(g_grid_size, g_grid_size, -1);
  vector<int> distance_count_table((c_max - c_min + 1) * g_grid_size + 1, 0);
  auto update_distance_count_table = [&]() -> void {
    std::fill(distance_count_table.begin(), distance_count_table.end(), 0);
    for (const int r : iota(0, g_grid_size)) {
      for (const int c : closed_iota(c_min, c_max)) {
        if (distance_table[r, c] >= 0) {
          distance_count_table[distance_table[r, c]]++;
        }
      }
    }
  };

  int score = FillDistanceTable(fixed_block_table, s, t, distance_table);
  update_distance_count_table();
  int max_score = score;
  Matrix<int8> max_block_table = block_table;

  int try_count = 0;
  int pass_count = 0;
  int pass_full_count = 0;
  int reject_count = 0;
  int reject_full_count = 0;
  int rejected_by_fixed_count = 0;
  int rejected_by_distance_dup_heuristic_count = 0;
  int immediate_pass_add_not_reachable_count = 0;
  int immediate_pass_remove_single_adj_count = 0;
  int immediate_pass_add_single_adj_count = 0;
  int immediate_pass_remove_count = 0;
  int immediate_pass_no_contraction_count = 0;
  int immediate_pass_no_distance_increase_count = 0;
  int rejected_by_distance_contraction_heuristic_count = 0;
  auto run_annealing_step = [&](const double temperature) -> void {
    const Cell cell = Cell(g_random.RandomInt(0, g_grid_size),
                           g_random.RandomIntInclusive(c_min, c_max));
    try_count++;
    if (fixed_block_table[cell.r, cell.c] != 0) {
      reject_count++;
      rejected_by_fixed_count++;
      return;
    }
    if (cell == start_cell || cell == s || cell == t) {
      reject_count++;
      rejected_by_fixed_count++;
      return;
    }
    const double threshold = AnnealingThreshold(temperature);

    bool should_reject = false;
    if (block_table[cell.r, cell.c] == 0) {
      // In this case, placing a block on the cell will disconnect s and t.
      if (distance_table[cell.r, cell.c] >= 0 &&
        distance_table[cell.r, cell.c] < score &&
          distance_count_table[distance_table[cell.r, cell.c]] == 1) {
        reject_count++;
        rejected_by_distance_dup_heuristic_count++;
        return;
      }

      // In this case, placing a block doesn't affect distance_table.
      // Pass immediately without updating distance_table.
      if (distance_table[cell.r, cell.c] < 0) {
        // HACK! Seems positive?
        if (g_random.RandomDouble(0.0, 1.0) < 0.1) {
          block_table[cell.r, cell.c] = 1;
          pass_count++;
          immediate_pass_add_not_reachable_count++;
          return;
        } else {
          reject_count++;
          // TODO: Add a counter here.
          return;
        }
      }
      
      {
        int adj_count = 0;
        for (const Direction& direction : kDirections) {
          const Cell adj_cell = cell.AdjacentCell(direction);
          if (!adj_cell.InGrid()) continue;
          if (distance_table[adj_cell.r, adj_cell.c] >= 0) {
            adj_count++;
          }
        }

        if (adj_count == 1) {
          if (g_random.RandomDouble(0.0, 1.0) < 0.1) {
          block_table[cell.r, cell.c] = 1;
          const int d = distance_table[cell.r, cell.c];
          distance_table[cell.r, cell.c] = -1;
          distance_count_table[d]--;
          pass_count++;
          immediate_pass_add_single_adj_count++;
          return;
          } else {
            reject_count++;
            // TODO: Add a counter here.
            return;
          }
        }

        int min_adj_distance = 1E9;
        for (const Direction& direction : kDirections) {
          const Cell adj_cell = cell.AdjacentCell(direction);
          if (!adj_cell.InGrid()) continue;
          if (distance_table[adj_cell.r, adj_cell.c] < 0) continue;
          min_adj_distance = std::min(min_adj_distance,
                                      distance_table[adj_cell.r, adj_cell.c]);
        }

        bool should_pass_no_distance_increase = [&]() -> bool {
          // Clockwise order.
          const array<Direction, 4> directions = {
              Direction::kRight, Direction::kDown, Direction::kLeft,
              Direction::kUp};
          for (const int i : iota(0, 4)) {
            const Direction direction_i = directions[i];
            const Cell cell_i = cell.AdjacentCell(direction_i);
            if (!cell_i.InGrid()) continue;
            const bool should_inspect_i =
                distance_table[cell_i.r, cell_i.c] > min_adj_distance;
            if (!should_inspect_i) continue;

            bool ok = false;
            for (const int j : iota(0, 4)) {
              const Direction direction_j = directions[j];
              const Cell cell_ij = cell_i.AdjacentCell(direction_j);
              if (!cell_ij.InGrid()) continue;
              if (cell_ij == cell) continue;
              if (distance_table[cell_i.r, cell_i.c] ==
                  distance_table[cell_ij.r, cell_ij.c] + 1) {
                ok = true;
              }
            }
            if (!ok) return false;
          }
          return true;
        }();
        if (should_pass_no_distance_increase) {
          block_table[cell.r, cell.c] = 1;
          const int d = distance_table[cell.r, cell.c];
          distance_table[cell.r, cell.c] = -1;
          distance_count_table[d]--;
          pass_count++;
          immediate_pass_no_distance_increase_count++;
          return;
        }
      }
    } else {
      int max_adj_distance = -1E9;
      int min_adj_distance = 1E9;
      int adj_non_block_count = 0;
      int adj_non_block_non_connected_count = 0;
      for (const Direction& direction : kDirections) {
        const Cell adj_cell = cell.AdjacentCell(direction);
        if (!adj_cell.InGrid()) continue;
        if (!block_table[adj_cell.r, adj_cell.c]) {
          adj_non_block_count++;
          if (distance_table[adj_cell.r, adj_cell.c] < 0) {
            adj_non_block_non_connected_count++;
          }
        }
        if (distance_table[adj_cell.r, adj_cell.c] < 0) continue;
        max_adj_distance =
            std::max(max_adj_distance, distance_table[adj_cell.r, adj_cell.c]);
        min_adj_distance =
            std::min(min_adj_distance, distance_table[adj_cell.r, adj_cell.c]);
      }
      // No reachable adjacent cell -> pass immediately without updating
      // distance_table.
      if (max_adj_distance < min_adj_distance) {
        block_table[cell.r, cell.c] = 0;
        pass_count++;
        immediate_pass_remove_count++;
        return;
      }

      if (adj_non_block_count == 1) {
        block_table[cell.r, cell.c] = 0;
        distance_table[cell.r, cell.c] = min_adj_distance + 1;
        distance_count_table[distance_table[cell.r, cell.c]]++;
        // immediate_pass_remove_count++;
        pass_count++;
        immediate_pass_remove_single_adj_count++;
        return;
      }

      if (adj_non_block_non_connected_count == 0 &&
          max_adj_distance - min_adj_distance <= 2) {
        block_table[cell.r, cell.c] = 0;
        distance_table[cell.r, cell.c] = min_adj_distance + 1;
        distance_count_table[distance_table[cell.r, cell.c]]++;
        pass_count++;
        immediate_pass_no_contraction_count++;
        return;
      }

      if (max_adj_distance - min_adj_distance >= 2 - threshold &&
          max_adj_distance < score &&
          distance_count_table[max_adj_distance] == 1) {
        // std::cerr << "max_adj_distance: " << max_adj_distance << std::endl;
        // std::cerr << "min_adj_distance: " << min_adj_distance << std::endl;
        // std::cerr << "threshold: " << threshold << std::endl;
        // std::cerr << "distance_count_table[max_adj_distance]: " << distance_count_table[max_adj_distance] << std::endl;
        // should_reject = true;
        reject_count++;
        rejected_by_distance_contraction_heuristic_count++;
        return;
      }
    }


    block_table[cell.r, cell.c] ^= 1;
    const int new_score =
        FillDistanceTable(block_table, s, t, new_distance_table);
    const bool pass = new_score >= 0 && new_score - score >= threshold;
    if (pass) {
      if (should_reject) {
        std::cerr << "score: " << score << std::endl;
        std::cerr << "new_score: " << new_score << std::endl;
        UNEXPECTED();
      }
      pass_count++;
      pass_full_count++;
      score = new_score;
      std::swap(distance_table, new_distance_table);
      update_distance_count_table();
      if (score > max_score) {
        max_score = score;
        max_block_table = block_table;
      }
    } else {
      reject_count++;
      reject_full_count++;
      // Undo the transition.
      block_table[cell.r, cell.c] ^= 1;
    }
  };

  const double kTemperatureBegin = 2.0;
  const double kTemperatureEnd = 0.2;
  if (g_is_deterministic_mode) {
    const int iter_count = 2E8 / (g_grid_size * g_grid_size);
    for (const int iter : iota(0, iter_count)) {
      const double temperature = ExpInterpolation(
          0, kTemperatureBegin, iter_count, kTemperatureEnd, iter);
      run_annealing_step(temperature);
    }
  } else {
    const int time_begin_ms = g_timer.CurrentMs();
    // const int time_limit_ms = 1600;
    while (true) {
      const int current_time_ms = g_timer.CurrentMs();
      if (current_time_ms > time_limit_ms) break;

      const double temperature =
          ExpInterpolation(time_begin_ms, kTemperatureBegin, time_limit_ms,
                           kTemperatureEnd, current_time_ms);
      run_annealing_step(temperature);
    }
  }

  std::cerr << "try_count: " << try_count << std::endl;
  std::cerr << "pass_count: " << pass_count << std::endl;
  std::cerr << "pass_full_count: " << pass_full_count << std::endl;
  std::cerr << "reject_count: " << reject_count << std::endl;
  std::cerr << "reject_full_count: " << reject_full_count << std::endl;
  std::cerr << "rejected_by_fixed_count: " << rejected_by_fixed_count
            << std::endl;
  std::cerr << "rejected_by_distance_dup_heuristic_count: "
            << rejected_by_distance_dup_heuristic_count << std::endl;
  std::cerr << "rejected_by_distance_contraction_heuristic_count: "
            << rejected_by_distance_contraction_heuristic_count << std::endl;
  std::cerr << "immediate_pass_add_not_reachable_count: "
            << immediate_pass_add_not_reachable_count << std::endl;
  std::cerr << "immediate_pass_remove_single_adj_count: "
            << immediate_pass_remove_single_adj_count << std::endl;
  std::cerr << "immediate_pass_add_single_adj_count: "
            << immediate_pass_add_single_adj_count << std::endl;
  std::cerr << "immediate_pass_no_contraction_count: "
            << immediate_pass_no_contraction_count << std::endl;
  std::cerr << "immediate_pass_remove_count: " << immediate_pass_remove_count
            << std::endl;
  std::cerr << "immediate_pass_no_distance_increase_count: "
            << immediate_pass_no_distance_increase_count << std::endl;
  std::cerr << "main_path_score: " << max_score << std::endl;
}

void PlaceMainPath(const Cell& start_cell, const int c_min, const int c_max,
                   const vector<Cell>& target_corridor,
                   const int time_limit_ms,
                   Matrix<CellState>& output_map) {
  Matrix<int8> fixed_block_table(g_grid_size, g_grid_size, 0);

  // Block cells outside the range [c_min, c_max].
  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(0, c_min)) {
      fixed_block_table[r, c] = 1;
    }
    for (const int c : iota(c_max + 1, g_grid_size)) {
      fixed_block_table[r, c] = 1;
    }
  }

  // Block cells around the target corridor.
  for (const Cell& cell : target_corridor) {
    fixed_block_table[cell.r, cell.c] = 1;
    for (const Direction& direction : kDirections) {
      const Cell adj_cell = cell.AdjacentCell(direction);
      if (adj_cell.InGrid()) {
        fixed_block_table[adj_cell.r, adj_cell.c] = 1;
      }
    }
  }

  // Block tree cells.
  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(0, g_grid_size)) {
      if (output_map[r, c] == CellState::kTree) {
        fixed_block_table[r, c] = 1;
      }
    }
  }

  Matrix<int> distance_table = DistanceTable(fixed_block_table, start_cell);

  const Cell s = [&]() -> Cell {
    for (const int c : closed_iota(c_min, c_max)) {
      for (const int r : std::views::reverse(iota(0, g_grid_size))) {
        if (distance_table[r, c] < 0) continue;
        if (output_map[r, c] == CellState::kUndecided) {
          return Cell(r, c);
        }
      }
    }
    UNEXPECTED();
  }();

  const Cell t = [&]() -> Cell {
    for (const int c : std::views::reverse(closed_iota(c_min, c_max))) {
      for (const int r : std::views::reverse(iota(0, g_grid_size))) {
        if (distance_table[r, c] < 0) continue;
        if (output_map[r, c] == CellState::kUndecided) {
          return Cell(r, c);
        }
      }
    }
    UNEXPECTED();
  }();

  Matrix<int8> block_table(g_grid_size, g_grid_size, 0);
  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(0, g_grid_size)) {
      if (fixed_block_table[r, c]) {
        block_table[r, c] = 1;
      }
    }
  }

  RunOptimization(start_cell, c_min, c_max, fixed_block_table, s, t,
                  time_limit_ms,block_table);

  Matrix<int> result_distance_table = DistanceTable(block_table, s);
  vector<Cell> path({t});
  Cell current_cell = t;
  while (current_cell != s) {
    const int distance = result_distance_table[current_cell.r, current_cell.c];
    Cell next_cell(-1, -1);
    for (const Direction& direction : kDirections) {
      Cell adj_cell = current_cell.AdjacentCell(direction);
      if (!adj_cell.InGrid()) continue;
      if (result_distance_table[adj_cell.r, adj_cell.c] == distance - 1) {
        next_cell = adj_cell;
        break;
      }
    }
    if (!next_cell.InGrid()) {
      UNEXPECTED();
    }
    path.push_back(next_cell);
    current_cell = next_cell;
  }

  for (const Cell& cell : path) {
    output_map[cell.r, cell.c] = CellState::kOpen;
  }

  if (result_distance_table[start_cell.r, start_cell.c] < 0) {
    vector<Cell> start_connection_path =
        main_path_internal::FindIsolatedPathToTargetSet(output_map, start_cell,
                                                        path);
    for (const Cell& cell : start_connection_path) {
      output_map[cell.r, cell.c] = CellState::kOpen;
    }
  }

  output_map[start_cell.r, start_cell.c] = CellState::kOpen;
}


#pragma once

#include <queue>





void PlaceStreet(const int column, Matrix<CellState>& map) {
  vector<Cell> street_cells;

  for (const int r : iota(0, g_grid_size)) {
    if (map[r, column] != CellState::kUndecided) continue;
    if (CountAdjacentOpenCells(map, Cell(r, column)) != 0) continue;
    street_cells.push_back(Cell(r, column));
  }

  for (const Cell& cell : street_cells) {
    map[cell.r, cell.c] = CellState::kOpen;
  }
}


namespace maze_generation_v1 {

enum class StreetType { kLeft, kRight };

vector<Cell> FindIsolatedPathToTargetSet(const Matrix<CellState>& map,
                                         const Cell& source_cell,
                                         const vector<Cell>& target_cells) {
  if (!source_cell.InGrid()) return {};
  if (map[source_cell.r, source_cell.c] == CellState::kTree) return {};

  Matrix<int8> is_target(g_grid_size, g_grid_size, 0);
  for (const Cell& t : target_cells) {
    if (!t.InGrid()) continue;
    if (map[t.r, t.c] == CellState::kTree ||
        map[t.r, t.c] == CellState::kTreant) {
      continue;
    }
    is_target[t.r, t.c] = 1;
  }

  if (is_target[source_cell.r, source_cell.c]) {
    return {source_cell};
  }

  Matrix<int8> visited(g_grid_size, g_grid_size, 0);
  Matrix<Cell> parent(g_grid_size, g_grid_size, Cell(-1, -1));
  std::deque<Cell> q;
  q.push_back(source_cell);
  visited[source_cell.r, source_cell.c] = 1;

  Cell target(-1, -1);
  while (!q.empty()) {
    const Cell cur = q.front();
    q.pop_front();
    for (const Direction& d : {Direction::kDown, Direction::kLeft,
                               Direction::kRight, Direction::kUp}) {
      const Cell nxt = cur.AdjacentCell(d);
      if (!nxt.InGrid()) continue;
      if (map[nxt.r, nxt.c] == CellState::kTree ||
          map[nxt.r, nxt.c] == CellState::kTreant) {
        continue;
      }
      if (visited[nxt.r, nxt.c]) continue;
      bool invalid = false;
      if (nxt != source_cell && !is_target[nxt.r, nxt.c]) {
        for (const Direction& direction : kDirections) {
          const Cell adjacent_cell = nxt.AdjacentCell(direction);
          if (!adjacent_cell.InGrid()) continue;
          if (map[adjacent_cell.r, adjacent_cell.c] == CellState::kOpen &&
              adjacent_cell != source_cell &&
              !is_target[adjacent_cell.r, adjacent_cell.c]) {
            invalid = true;
            break;
          }
        }
      }
      if (invalid) continue;
      visited[nxt.r, nxt.c] = 1;
      parent[nxt.r, nxt.c] = cur;
      if (is_target[nxt.r, nxt.c]) {
        target = nxt;
        q.clear();
        break;
      }
      q.push_back(nxt);
    }
  }

  if (target.r == -1) return {};

  vector<Cell> path;
  for (Cell cur = target; cur.r != -1; cur = parent[cur.r, cur.c]) {
    path.push_back(cur);
  }
  std::reverse(path.begin(), path.end());
  return path;
}

vector<Cell> MakeTargetCorridorLegacy(const Matrix<CellState>& map,
                                      const Cell& start_cell,
                                      const Cell& final_target_cell) {
  constexpr Direction U = Direction::kUp;
  constexpr Direction D = Direction::kDown;
  constexpr Direction L = Direction::kLeft;
  constexpr Direction R = Direction::kRight;
  vector<vector<Direction>> patterns = {
      {D, R, U, L, D}, {D, L, U, R, D}, {R, D, L, U, R}, {R, U, L, D, R},
      {L, D, R, U, L}, {L, U, R, D, L}, {U, R, D, L, U}, {U, L, D, R, U},
  };

  vector<Cell> best_path;
  int best_score = 0;
  for (const vector<Direction>& directions : patterns) {
    Matrix<CellState> current_map = map;
    vector<Cell> path;
    Cell segment_start_cell = final_target_cell;
    int score = 0;
    for (const Direction& direction : directions) {
      vector<Cell> target_cells;

      Cell cell = final_target_cell;
      while (true) {
        cell = cell.AdjacentCell(direction);
        if (!cell.InGrid()) break;
        if (current_map[cell.r, cell.c] != CellState::kUndecided) continue;

        bool invalid = false;
        for (const Direction& adjacent_direction : kDirections) {
          const Cell adjacent_cell = cell.AdjacentCell(adjacent_direction);
          if (!adjacent_cell.InGrid()) continue;
          if (current_map[adjacent_cell.r, adjacent_cell.c] ==
              CellState::kOpen) {
            invalid = true;
            break;
          }
        }

        if (std::abs(cell.r - start_cell.r) + std::abs(cell.c - start_cell.c) <=
            4) {
          invalid = true;
          break;
        }

        if (invalid) continue;

        target_cells.push_back(cell);
      }

      vector<Cell> path_segment = FindIsolatedPathToTargetSet(
          current_map, segment_start_cell, target_cells);
      if (!path_segment.empty() && path_segment.size() <= 10) {
        for (const Cell& path_segment_cell : path_segment) {
          current_map[path_segment_cell.r, path_segment_cell.c] =
              CellState::kOpen;
        }
        path.insert(path.end(), path_segment.begin(), path_segment.end());
        score++;
      }
      if (!path.empty()) {
        segment_start_cell = path.back();
      }
    }
    if (score > best_score) {
      best_score = score;
      best_path = path;
    }
  }
  return best_path;
}

void MakeExtraSpaces(Matrix<CellState>& output_table, int phase) {
  while (true) {
    vector<Cell> candidates;
    for (const int r : iota(0, g_grid_size)) {
      for (const int c : iota(0, g_grid_size)) {
        if (output_table[r, c] != CellState::kUndecided) continue;
        int adjacent_count = 0;
        for (const Direction& direction : kDirections) {
          const Cell adjacent_cell = Cell(r, c).AdjacentCell(direction);
          if (!adjacent_cell.InGrid()) continue;
          if (output_table[adjacent_cell.r, adjacent_cell.c] ==
              CellState::kOpen) {
            adjacent_count++;
          }
        }

        // Run in only phase 1.
        if (phase > 0 && adjacent_count == 2) {
          if (r + 1 < g_grid_size && c + 1 < g_grid_size &&
              output_table[r, c + 1] == CellState::kOpen &&
              output_table[r + 1, c] == CellState::kOpen &&
              output_table[r + 1, c + 1] == CellState::kOpen) {
            candidates.push_back(Cell(r, c));
          } else if (r + 1 < g_grid_size && c - 1 >= 0 &&
                     output_table[r, c - 1] == CellState::kOpen &&
                     output_table[r + 1, c] == CellState::kOpen &&
                     output_table[r + 1, c - 1] == CellState::kOpen) {
            candidates.push_back(Cell(r, c));
          } else if (r - 1 >= 0 && c + 1 < g_grid_size &&
                     output_table[r, c + 1] == CellState::kOpen &&
                     output_table[r - 1, c] == CellState::kOpen &&
                     output_table[r - 1, c + 1] == CellState::kOpen) {
            candidates.push_back(Cell(r, c));
          } else if (r - 1 >= 0 && c - 1 >= 0 &&
                     output_table[r, c - 1] == CellState::kOpen &&
                     output_table[r - 1, c] == CellState::kOpen &&
                     output_table[r - 1, c - 1] == CellState::kOpen) {
            candidates.push_back(Cell(r, c));
          }
        }

        if (adjacent_count != 1) continue;

        // const int target_cell_distance =
        //     std::abs(r - target_cell.r) + std::abs(c - target_cell.c);
        // if (target_cell_distance <= 2) continue;
        candidates.push_back(Cell(r, c));
      }
    }

    if (candidates.empty()) break;

    const Cell cell = candidates[g_random.RandomInt(0, candidates.size())];
    output_table[cell.r, cell.c] = CellState::kOpen;
  }
}

void PlaceLShapes(Matrix<CellState>& map) {
  struct Delta {
    int dr;
    int dc;
    Delta(int dr, int dc) : dr(dr), dc(dc) {}
  };
  vector<Delta> left_y_shape = {
      Delta(0, -1),  Delta(0, -2), Delta(-1, -2),
      Delta(-1, -3), Delta(1, -2), Delta(1, -3),
  };
  vector<Delta> left_l_shape_0 = {
      Delta(0, -1),
      Delta(0, -2),
      Delta(-1, -2),
  };
  vector<Delta> left_l_shape_1 = {
      Delta(0, -1),
      Delta(0, -2),
      Delta(1, -2),
  };
  vector<Delta> right_y_shape = {
      Delta(0, 1),  Delta(0, 2), Delta(-1, 2),
      Delta(-1, 3), Delta(1, 2), Delta(1, 3),
  };
  vector<Delta> right_l_shape_0 = {
      Delta(0, 1),
      Delta(0, 2),
      Delta(-1, 2),
  };
  vector<Delta> right_l_shape_1 = {
      Delta(0, 1),
      Delta(0, 2),
      Delta(1, 2),
  };
  vector<Delta> up_l_shape_0 = {
      Delta(-1, 0),
      Delta(-2, 0),
      Delta(-2, -1),
  };
  vector<Delta> up_l_shape_1 = {
      Delta(-1, 0),
      Delta(-2, 0),
      Delta(-2, 1),
  };

  auto is_applicable = [&](const Cell& cell,
                           const vector<Delta>& pattern) -> bool {
    if (!cell.InGrid()) return false;
    if (map[cell.r, cell.c] != CellState::kOpen) return false;
    for (const int i : iota(0, std::ssize(pattern))) {
      const Cell pattern_cell =
          Cell(cell.r + pattern[i].dr, cell.c + pattern[i].dc);
      if (!pattern_cell.InGrid()) return false;
      if (map[pattern_cell.r, pattern_cell.c] != CellState::kUndecided) {
        return false;
      }
      if (i == 0) {
        if (CountAdjacentOpenCells(map, pattern_cell) != 1) {
          return false;
        }
      } else {
        if (CountAdjacentOpenCells(map, pattern_cell) != 0) {
          return false;
        }
      }
    }
    return true;
  };
  auto apply = [&](const Cell& cell, const vector<Delta>& pattern) -> void {
    for (const int i : iota(0, std::ssize(pattern))) {
      const Cell pattern_cell =
          Cell(cell.r + pattern[i].dr, cell.c + pattern[i].dc);
      map[pattern_cell.r, pattern_cell.c] = CellState::kOpen;
    }
  };

  for (const int r : iota(0, g_grid_size)) {
    for (const int c : closed_iota(0, g_grid_size / 2)) {
      if (map[r, c] != CellState::kOpen) continue;
      for (const vector<Delta>& pattern : {right_y_shape}) {
        if (is_applicable(Cell(r, c), pattern)) {
          apply(Cell(r, c), pattern);
        }
      }
    }
  }
  for (const int r : iota(0, g_grid_size)) {
    for (const int c : closed_iota(0, g_grid_size / 2)) {
      if (map[r, c] != CellState::kOpen) continue;
      for (const vector<Delta>& pattern : {left_y_shape}) {
        if (is_applicable(Cell(r, c), pattern)) {
          apply(Cell(r, c), pattern);
        }
      }
    }
  }

  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(g_grid_size / 2 + 1, g_grid_size)) {
      if (map[r, c] != CellState::kOpen) continue;
      for (const vector<Delta>& pattern : {left_y_shape}) {
        if (is_applicable(Cell(r, c), pattern)) {
          apply(Cell(r, c), pattern);
        }
      }
    }
  }
  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(g_grid_size / 2 + 1, g_grid_size)) {
      if (map[r, c] != CellState::kOpen) continue;
      for (const vector<Delta>& pattern : {right_y_shape}) {
        if (is_applicable(Cell(r, c), pattern)) {
          apply(Cell(r, c), pattern);
        }
      }
    }
  }

  for (const int r : iota(0, g_grid_size)) {
    for (const int c : closed_iota(0, g_grid_size / 2)) {
      if (map[r, c] != CellState::kOpen) continue;
      for (const vector<Delta>& pattern : {right_l_shape_0, right_l_shape_1}) {
        if (is_applicable(Cell(r, c), pattern)) {
          apply(Cell(r, c), pattern);
        }
      }
    }
  }
  for (const int r : iota(0, g_grid_size)) {
    for (const int c : closed_iota(0, g_grid_size / 2)) {
      if (map[r, c] != CellState::kOpen) continue;
      for (const vector<Delta>& pattern : {left_l_shape_0, left_l_shape_1}) {
        if (is_applicable(Cell(r, c), pattern)) {
          apply(Cell(r, c), pattern);
        }
      }
    }
  }

  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(g_grid_size / 2 + 1, g_grid_size)) {
      if (map[r, c] != CellState::kOpen) continue;
      for (const vector<Delta>& pattern : {left_l_shape_0, left_l_shape_1}) {
        if (is_applicable(Cell(r, c), pattern)) {
          apply(Cell(r, c), pattern);
        }
      }
    }
  }
  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(g_grid_size / 2 + 1, g_grid_size)) {
      if (map[r, c] != CellState::kOpen) continue;
      for (const vector<Delta>& pattern : {right_l_shape_0, right_l_shape_1}) {
        if (is_applicable(Cell(r, c), pattern)) {
          apply(Cell(r, c), pattern);
        }
      }
    }
  }

  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(0, g_grid_size)) {
      if (map[r, c] != CellState::kOpen) continue;
      for (const vector<Delta>& pattern : {up_l_shape_0, up_l_shape_1}) {
        if (is_applicable(Cell(r, c), pattern)) {
          apply(Cell(r, c), pattern);
        }
      }
    }
  }
}

void PrintOutputMap(const Matrix<CellState>& output_map,
                    const Cell& target_cell) {
  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(0, g_grid_size)) {
      if (r == target_cell.r && c == target_cell.c) {
        std::cerr << "!";
      } else if (output_map[r, c] == CellState::kOpen) {
        std::cerr << ".";
      } else if (output_map[r, c] == CellState::kTree) {
        std::cerr << "#";
      } else if (output_map[r, c] == CellState::kTreant) {
        std::cerr << "+";
      } else if (output_map[r, c] == CellState::kUndecided) {
        std::cerr << "?";
      } else {
        UNEXPECTED();
      }
    }
    std::cerr << "\n";
  }
}

vector<Cell> MakeTargetCorridorConnection(const Matrix<CellState>& map,
                                          const vector<Cell>& target_corridor) {
  Matrix<int8> is_target_corridor_table(g_grid_size, g_grid_size, 0);
  for (const Cell& cell : target_corridor) {
    is_target_corridor_table[cell.r, cell.c] = 1;
  }

  vector<Cell> target_cells;
  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(0, g_grid_size)) {
      if (map[r, c] == CellState::kOpen && !is_target_corridor_table[r, c]) {
        target_cells.push_back(Cell(r, c));
      }
    }
  }

  for (const Cell& cell : std::views::reverse(target_corridor)) {
    const vector<Cell> path =
        FindIsolatedPathToTargetSet(map, cell, target_cells);
    if (!path.empty()) {
      return path;
    }
  }

  return {};
}

void PlaceConnectionPaths(const Cell& start_cell,
                          const vector<Cell>& target_corridor,
                          Matrix<CellState>& map) {
  Matrix<int8> is_target_corridor_table(g_grid_size, g_grid_size, 0);
  for (const Cell& cell : target_corridor) {
    is_target_corridor_table[cell.r, cell.c] = 1;
  }

  while (true) {
    Matrix<int> distance_table(g_grid_size, g_grid_size, -1);

    std::deque<Cell> q;
    distance_table[start_cell.r, start_cell.c] = 0;
    q.push_back(start_cell);

    while (!q.empty()) {
      const Cell cur = q.front();
      // if (cur == target_corridor[0] || cur == target_corridor[1]) continue;
      q.pop_front();
      for (const Direction& d : kDirections) {
        const Cell nxt = cur.AdjacentCell(d);
        if (!nxt.InGrid()) continue;
        if (map[nxt.r, nxt.c] != CellState::kOpen) continue;
        if (distance_table[nxt.r, nxt.c] != -1) continue;
        distance_table[nxt.r, nxt.c] = distance_table[cur.r, cur.c] + 1;
        q.push_back(nxt);
      }
    }

    vector<Cell> target_cells;
    for (const int r : iota(0, g_grid_size)) {
      for (const int c : iota(0, g_grid_size)) {
        if (map[r, c] == CellState::kOpen && !is_target_corridor_table[r, c] &&
            distance_table[r, c] == -1) {
          target_cells.push_back(Cell(r, c));
        }
      }
    }
    const Cell& target_corridor_entry = target_corridor.back();
    if (distance_table[target_corridor_entry.r, target_corridor_entry.c] == -1) {
      target_cells.push_back(target_corridor_entry);
    }
    // target_cells.push_back(target_corridor.back());

    if (target_cells.empty()) break;

    // If the only remaining cell is the target corridor entry, we can stop.
    if (target_cells.size() == 1 && target_cells[0] == target_corridor_entry) {
      break;
    }

    vector<Cell> path_start_cells;
    for (const int r : iota(0, g_grid_size)) {
      for (const int c : iota(0, g_grid_size)) {
        if (distance_table[r, c] >= 0) {
          path_start_cells.push_back(Cell(r, c));
        }
      }
    }
    std::sort(path_start_cells.begin(), path_start_cells.end(),
              [&](const Cell& a, const Cell& b) {
                return distance_table[a.r, a.c] > distance_table[b.r, b.c];
              });

    bool found = false;
    for (const Cell& path_start_cell : path_start_cells) {
      vector<Cell> path =
          FindIsolatedPathToTargetSet(map, path_start_cell, target_cells);
      // std::cerr << "path: " << path << std::endl;
      if (!path.empty()) {
        if (path.back() == target_corridor_entry && path.size() > 5) {
          vector<Cell> anothter_target_cells = target_cells;
          anothter_target_cells.erase(std::find(anothter_target_cells.begin(),
                                                anothter_target_cells.end(),
                                                target_corridor_entry));
          vector<Cell> another_path = FindIsolatedPathToTargetSet(
              map, path_start_cell, anothter_target_cells);
          if (another_path.empty()) {
            continue;
          }
          path = another_path;
        }
        for (const Cell& path_cell : path) {
          map[path_cell.r, path_cell.c] = CellState::kOpen;
        }
        found = true;
        break;
      }
    }

    if (!found) {
      for (const Cell& cell : target_cells) {
        if (distance_table[cell.r, cell.c] == -1) {
          map[cell.r, cell.c] = CellState::kUndecided;
        }
      }
      break;
    }
  }
}

optional<Matrix<CellState>> GenerateMazeInternal(
    const int phase, const int size, const Matrix<int8>& blocked_table,
    const int start_r, const int start_c, const int target_r,
    const int target_c) {
  std::cerr << "grid_size: " << g_grid_size << std::endl;

  Matrix<int8> reserved_table(size, size, 0);
  Matrix<CellState> output_map(size, size, CellState::kUndecided);
  for (const int r : iota(0, size)) {
    for (const int c : iota(0, size)) {
      if (blocked_table[r, c]) {
        output_map[r, c] = CellState::kTree;
      }
    }
  }

  LayoutParams layout_params = [&]() -> LayoutParams {
    LayoutParams layout_params;
    if (g_grid_size < 22) {
      layout_params.left_street_column = 3;
      layout_params.right_street_column = g_grid_size - 4;
      layout_params.main_path_c_min = 5;
      layout_params.main_path_c_max = g_grid_size - 6;
    } else {
      layout_params.left_street_column = 3;
      layout_params.right_street_column = g_grid_size - 4;
      layout_params.main_path_c_min = 8;
      layout_params.main_path_c_max = g_grid_size - 9;
    }
    return layout_params;
  }();

  optional<vector<Cell>> opt_target_corridor;
  if (phase == 0) {
    opt_target_corridor = MakeCompactGoalCorridor(
        output_map, Cell(target_r, target_c), layout_params);
  } else if (phase == 1) {
    opt_target_corridor =
        MakeGoalCorridor(output_map, Cell(target_r, target_c), layout_params);
  } else {
    UNEXPECTED();
  }

  if (phase == 1 && !opt_target_corridor) {
    std::cerr << "MakeGoalCorridor failed" << std::endl;
  }

  if (phase == 0 && !opt_target_corridor) {
    return nullopt;
  }

  if (phase== 1 && !opt_target_corridor) {
    opt_target_corridor = MakeTargetCorridorLegacy(
        output_map, Cell(start_r, start_c), Cell(target_r, target_c));
  }
  if (!opt_target_corridor) {
    UNEXPECTED();
  }
  vector<Cell> target_corridor = *opt_target_corridor;
  for (const Cell& cell : target_corridor) {
    output_map[cell.r, cell.c] = CellState::kOpen;
  }

  // Place treants around the first two cells of the target corridor.
  // TODO: Make this number dynamic.
  for (const int i : iota(0, 2)) {
    if (i >= std::ssize(target_corridor)) {
      break;
    }
    for (const Direction& direction : kDirections) {
      const Cell adj_cell = target_corridor[i].AdjacentCell(direction);
      if (adj_cell.InGrid() &&
          output_map[adj_cell.r, adj_cell.c] == CellState::kUndecided) {
        output_map[adj_cell.r, adj_cell.c] = CellState::kTreant;
      }
    }
  }

  PlaceStreet(layout_params.left_street_column, output_map);

  PlaceStreet(layout_params.right_street_column, output_map);

  const int time_limit_ms = phase == 0 ? 600 : 1800;
  PlaceMainPath(Cell(start_r, start_c), layout_params.main_path_c_min,
                layout_params.main_path_c_max, target_corridor, time_limit_ms,
                output_map);

  PlaceConnectionPaths(Cell(start_r, start_c), target_corridor, output_map);

  vector<Cell> target_corridor_connection_path =
      MakeTargetCorridorConnection(output_map, target_corridor);
  for (const Cell& cell : target_corridor_connection_path) {
    output_map[cell.r, cell.c] = CellState::kOpen;
  }

  if (phase == 0) {
    if (!VerifyCompactGoalCorridor(output_map, target_corridor,
                                   layout_params)) {
      std::cerr << "phase0-invalid" << std::endl;
      return nullopt;
    } else {
      std::cerr << "phase0-valid" << std::endl;
    }
  }

  PlaceLShapes(output_map);

  MakeExtraSpaces(output_map, 0);
  MakeExtraSpaces(output_map, 1);

  for (const int r : iota(0, g_grid_size)) {
    for (const int c : iota(0, g_grid_size)) {
      if (output_map[r, c] == CellState::kUndecided) {
        output_map[r, c] = CellState::kTreant;
      }
    }
  }

  PrintOutputMap(output_map, Cell(target_r, target_c));

  return output_map;
}

Matrix<CellState> GenerateMaze(const int size, const Matrix<int8>& blocked_table,
const int start_r, const int start_c, const int target_r,
const int target_c) {
  g_random.Reset();
  optional<Matrix<CellState>> opt_phase_0_result =
      GenerateMazeInternal(0, size, blocked_table, start_r, start_c, target_r,
                           target_c);
  if (opt_phase_0_result) {
    return *opt_phase_0_result;
  }

  g_random.Reset();
  optional<Matrix<CellState>> opt_phase_1_result =
      GenerateMazeInternal(1, size, blocked_table, start_r, start_c, target_r,
                           target_c);
  if (!opt_phase_1_result) {
    UNEXPECTED();
  }
  return *opt_phase_1_result;
}

}  // namespace maze_generation_v1


int main() {
  int size;
  std::cin >> size;
  g_grid_size = size;
  int target_r, target_c;
  std::cin >> target_r >> target_c;
  Cell target(target_r, target_c);
  Matrix<int8> blocked_table(size, size);
  vector<string> rows;
  for (const int r : iota(0, size)) {
    string row;
    std::cin >> row;
    for (const int c : iota(0, size)) {
      if (row[c] == 'T') {
        blocked_table[r, c] = 1;
      }
    }
  }
  const int start_r = 0;
  const int start_c = size / 2;

  Matrix<CellState> map = maze_generation_v1::GenerateMaze(
      size, blocked_table, start_r, start_c, target_r, target_c);

  int turn = 0;
  while (true) {
    int current_r, current_c;
    std::cin >> current_r >> current_c;
    Cell current_cell(current_r, current_c);
    int new_visible_count;
    std::cin >> new_visible_count;
    for (const int _ : iota(0, new_visible_count)) {
      int visible_r, visible_c;
      std::cin >> visible_r >> visible_c;
    }

    if (current_r == target_r && current_c == target_c) {
      break;
    }

    if (turn == 0) {
      vector<Cell> treant_cells;
      for (const int r : iota(0, size)) {
        for (const int c : iota(0, size)) {
          if (map[r, c] == CellState::kTreant) {
            treant_cells.push_back(Cell(r, c));
          }
        }
      }
      std::cout << treant_cells.size() << " ";
      for (const Cell& cell : treant_cells) {
        std::cout << cell.r << " " << cell.c << " ";
      }
      std::cout << "\n";
      std::cout.flush();
    } else {
      std::cout << "-1\n";
      std::cout.flush();
      break;
    }

    turn++;
  }

  return 0;
}
