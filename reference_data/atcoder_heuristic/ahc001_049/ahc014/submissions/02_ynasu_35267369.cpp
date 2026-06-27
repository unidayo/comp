#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>
#include <cmath>
#include <cstdint>

constexpr double kTimeLimit = 4.95;
constexpr int kGridSizeBits = 6;
constexpr int kGridSize = 1 << kGridSizeBits;
constexpr int kNumPoints = kGridSize * kGridSize;

namespace Global {

int N;
int initial_range_min;
int initial_range_max;
int score_table[3][kNumPoints];

}  // namespace Global

auto Now() {
  return std::chrono::high_resolution_clock::now();
}

struct Timer {
  Timer() : start(Now()) {}

  double GetTime() const {
    return std::chrono::duration<double>(Now() - start).count();
  }

  const decltype(Now()) start;
};

class Random {
 public:
  constexpr explicit Random(std::int64_t seed) {
    state_[0] = seed;
    state_[1] = RotateLeft(seed, 11);
    for (int i = 0; i < 16; ++i) {
      Next();
    }
  }

  constexpr double Canonical() {
    return 0x1.p-64 * Next();
  }

  constexpr int Range(int size) {
    return Next() % size;
  }

  constexpr int MinMax(int min, int max) {
    return Range(max - min + 1) + min;
  }

  template <typename V>
  void Shuffle(V& vector) {
    const int N = vector.size();
    for (int i = N - 1; i > 0; --i) {
      std::swap(vector[i], vector[Range(i + 1)]);
    }
  }

 private:
  constexpr std::uint64_t Next() {
    auto [s0, s1] = state_;
    const auto result = RotateLeft(s0 * 5, 7) * 9;
    s1 ^= s0;
    state_[0] = RotateLeft(s0, 24) ^ s1 ^ (s1 << 16);
    state_[1] = RotateLeft(s1, 37);
    return result;
  }

  static constexpr std::uint64_t RotateLeft(std::uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
  }

  std::uint64_t state_[2] = {};
};

class Position {
 public:
  explicit Position(std::uint16_t index = 0) : index_(index) {}
  Position(std::uint8_t x, std::uint8_t y) : index_((x << kGridSizeBits) ^ y) {}

  constexpr std::uint8_t X() const {
    return index_ >> kGridSizeBits;
  }

  constexpr std::uint8_t Y() const {
    return index_ & ((1 << kGridSizeBits) - 1);
  }

  template <std::size_t Index>
  constexpr std::uint8_t get() const {
    if constexpr (Index == 0) {
      return X();
    } else {
      return Y();
    }
  }

  operator std::uint16_t() const {
    return index_;
  }

 private:
  std::uint16_t index_;
};

template <>
struct std::tuple_size<Position> : std::integral_constant<std::size_t, 2> {};

template <std::size_t Index>
struct std::tuple_element<Index, Position> {
  using type = std::uint8_t;
};

std::tuple<bool, Position> ToDiagonal(Position pos) {
  const auto [x, y] = pos;
  const bool odd = (x ^ y) & 1;
  const int u = (x + y) >> 1;
  const int v = (x - y + kGridSize) >> 1;
  return {odd, Position(u, v)};
}

Position FromDiagonal(bool odd, Position pos) {
  const auto [u, v] = pos;
  const int x = (2 * (u + v + odd) - kGridSize) >> 1;
  const int y = (2 * (u - v) + kGridSize) >> 1;
  return Position(x, y);
}

Position NormalizePosition(int plane_index, Position pos) {
  if (plane_index != 2) {
    pos = FromDiagonal(plane_index, pos);
  }
  return pos;
}

using Rectangle = std::pair<Position, Position>;
using Move = std::pair<int, Rectangle>;

std::array<Position, 4> ConvertMoveToVertices(const Move& move) {
  const auto& [plane_index, rectangle] = move;
  const auto& [a, b] = rectangle;
  const auto [ax, ay] = a;
  const auto [bx, by] = b;
  std::array<Position, 4> vertices;
  vertices[0] = Position(ax, ay);
  vertices[1] = Position(ax, by);
  vertices[2] = Position(bx, by);
  vertices[3] = Position(bx, ay);
  if (plane_index != 2) {
    for (int i = 0; i < 4; ++i) {
      vertices[i] = FromDiagonal(plane_index, vertices[i]);
    }
  }
  return vertices;
}

int ComputeDistance(Position a, Position b) {
  const auto [ax, ay] = a;
  const auto [bx, by] = b;
  return std::abs(ax - bx) + std::abs(ay - by);
}

struct Plane {
  Plane() : nodes_x{}, nodes_y{}, edges_x{}, edges_y{} {}

  bool HasNode(Position pos) const {
    const auto& [x, y] = pos;
    return (nodes_x[y] >> x) & 1;
  }

  void AddNode(Position pos) {
    const auto& [x, y] = pos;
    nodes_x[y] ^= 1ULL << x;
    nodes_y[x] ^= 1ULL << y;
  }

  void AddRectangle(const Rectangle& rectangle) {
    const auto& [a, b] = rectangle;
    const auto [ax, ay] = a;
    const auto [bx, by] = b;
    const auto mask_x = ((1ULL << ax) - 1) ^ ((1ULL << bx) - 1);
    const auto mask_y = ((1ULL << ay) - 1) ^ ((1ULL << by) - 1);
    nodes_x[ay] ^= 1ULL << ax;
    nodes_y[ax] ^= 1ULL << ay;
    edges_x[ay] ^= mask_x;
    edges_x[by] ^= mask_x;
    edges_y[ax] ^= mask_y;
    edges_y[bx] ^= mask_y;
  }

  bool CanAddRectangle(const Rectangle& rectangle) const {
    const auto& [a, b] = rectangle;
    if (Global::score_table[plane_index][a] == 0) return false;
    const auto [ax, ay] = a;
    const auto [bx, by] = b;
    const auto mask_x = ((1ULL << ax) - 1) ^ ((1ULL << bx) - 1);
    const auto mask_y = ((1ULL << ay) - 1) ^ ((1ULL << by) - 1);
    if ((edges_x[ay] | edges_x[by] | ((nodes_x[ay] | nodes_x[by]) & ~(1ULL << std::min(ax, bx)))) & mask_x) return false;
    if ((edges_y[ax] | edges_y[bx] | ((nodes_y[ax] | nodes_y[bx]) & ~(1ULL << std::min(ay, by)))) & mask_y) return false;
    return ((~nodes_x[ay] & nodes_x[by]) >> ax) & ((nodes_x[ay] & nodes_x[by]) >> bx) & 1;
  }

  Position FindRight(Position pos) const {
    const auto [x, y] = pos;
    if ((edges_x[y] >> x) & 1) return Position(0);
    const auto masked = nodes_x[y] & ~((1ULL << (x + 1)) - 1);
    if (masked == 0) return Position(0);
    return Position(__builtin_ctzll(masked), y);
  }

  Position FindLeft(Position pos) const {
    const auto [x, y] = pos;
    if ((edges_x[y] >> (x - 1)) & 1) return Position(0);
    const auto masked = nodes_x[y] & ((1ULL << x) - 1);
    if (masked == 0) return Position(0);
    return Position(63 - __builtin_clzll(masked), y);
  }

  Position FindUp(Position pos) const {
    const auto [x, y] = pos;
    if ((edges_y[x] >> y) & 1) return Position(0);
    const auto masked = nodes_y[x] & ~((1ULL << (y + 1)) - 1);
    if (masked == 0) return Position(0);
    return Position(x, __builtin_ctzll(masked));
  }

  Position FindDown(Position pos) const {
    const auto [x, y] = pos;
    if ((edges_y[x] >> (y - 1)) & 1) return Position(0);
    const auto masked = nodes_y[x] & ((1ULL << y) - 1);
    if (masked == 0) return Position(0);
    return Position(x, 63 - __builtin_clzll(masked));
  }

  template <int Direction>
  Position FindNext(Position pos) const {
    if constexpr ((Direction & 3) == 0) return FindLeft(pos);
    if constexpr ((Direction & 3) == 1) return FindUp(pos);
    if constexpr ((Direction & 3) == 2) return FindRight(pos);
    if constexpr ((Direction & 3) == 3) return FindDown(pos);
  }

  void StoreMoveCandidates(Position pos, int pattern, std::vector<std::vector<Move>>& candidates) const {
    const auto add_move = [&, parity = (pattern >> plane_index) & 1](Rectangle rectangle) {
      const auto& [a, b] = rectangle;
      if (ComputeDistance(a, b) == 2) {
        const auto x = (a ^ b) >> 1;
        const auto index = (x ^ (x >> kGridSizeBits) ^ parity) & 1;
        candidates[index].emplace_back(plane_index, rectangle);
      } else {
        candidates[2].emplace_back(plane_index, rectangle);
      }
    };
    const std::array<Position, 4> nexts = {
      FindNext<0>(pos),
      FindNext<1>(pos),
      FindNext<2>(pos),
      FindNext<3>(pos),
    };
    AddMoves<0>(pos, nexts, add_move);
    AddMoves<1>(pos, nexts, add_move);
    AddMoves<2>(pos, nexts, add_move);
    AddMoves<3>(pos, nexts, add_move);
  }

  template <int Direction, typename F>
  void AddMoves(Position pos, const std::array<Position, 4>& nexts, const F& add_move) const {
    const auto& a = nexts[Direction];
    const auto& b = nexts[(Direction + 1) & 3];
    if (a && b) {
      const Position opposite(a ^ b ^ pos);
      if (!HasNode(opposite) && FindNext<Direction + 3>(opposite) == a && FindNext<Direction + 2>(opposite) == b) {
        add_move({opposite, pos});
        return;
      }
    }
    if (a) {
      if (const Position opposite = FindNext<Direction + 1>(a)) {
        const Position c(pos ^ a ^ opposite);
        if (!HasNode(c) && FindNext<Direction + 3>(c) == pos && FindNext<Direction>(c) == opposite) {
          add_move({c, a});
        }
      }
    }
    if (b) {
      if (const Position opposite = FindNext<Direction>(b)) {
        const Position c(pos ^ b ^ opposite);
        if (!HasNode(c) && FindNext<Direction + 2>(c) == pos && FindNext<Direction + 1>(c) == opposite) {
          add_move({c, b});
        }
      }
    }
  }

  int plane_index;
  std::array<std::uint64_t, kGridSize> nodes_x;
  std::array<std::uint64_t, kGridSize> nodes_y;
  std::array<std::uint64_t, kGridSize> edges_x;
  std::array<std::uint64_t, kGridSize> edges_y;
};

struct Board {
  bool IsValidMove(int plane_index, const Rectangle& rectangle) const {
    return planes[plane_index].CanAddRectangle(rectangle);
  }

  void MakeMove(int plane_index, const Rectangle& rectangle) {
    if (plane_index == 2) {
      planes[2].AddRectangle(rectangle);
      planes[2].StoreMoveCandidates(rectangle.first, pattern, candidates);
      const auto [odd, diagonal] = ToDiagonal(rectangle.first);
      planes[odd].AddNode(diagonal);
      planes[odd].StoreMoveCandidates(diagonal, pattern, candidates);
    } else {
      planes[plane_index].AddRectangle(rectangle);
      planes[plane_index].StoreMoveCandidates(rectangle.first, pattern, candidates);
      const auto pos = FromDiagonal(plane_index, rectangle.first);
      planes[2].AddNode(pos);
      planes[2].StoreMoveCandidates(pos, pattern, candidates);
    }
  }

  void PlayOut(std::vector<Move>& moves, Random& rng) {
    for (bool updated = true; updated; ) {
      updated = false;
      for (auto& stack : candidates) {
        if (updated) break;
        while (!stack.empty()) {
          const int index = rng.Range(stack.size());
          std::swap(stack.back(), stack[index]);
          const auto [plane_index, rectangle] = stack.back();
          stack.pop_back();
          if (!IsValidMove(plane_index, rectangle)) continue;
          updated = true;
          MakeMove(plane_index, rectangle);
          moves.emplace_back(plane_index, rectangle);
          break;
        }
      }
    }
  }

  static void Precompute(const std::vector<Position>& points) {
    for (int i = 0; i < 3; ++i) {
      initial_planes[i].plane_index = i;
    }
    for (const auto& point : points) {
      initial_planes[2].AddNode(point);
      const auto [odd, diagonal] = ToDiagonal(point);
      initial_planes[odd].AddNode(diagonal);
    }
    initial_candidates.resize(8);
    for (int pattern = 0; pattern < 8; ++pattern) {
      initial_candidates[pattern].resize(3);
      for (const auto& pos : points) {
        const auto [odd, diagonal] = ToDiagonal(pos);
        initial_planes[2].StoreMoveCandidates(pos, pattern, initial_candidates[pattern]);
        initial_planes[odd].StoreMoveCandidates(diagonal, pattern, initial_candidates[pattern]);
      }
      for (auto& stack : initial_candidates[pattern]) {
        std::sort(stack.begin(), stack.end());
        stack.erase(std::unique(stack.begin(), stack.end()), stack.end());
      }
    }
  }

  static void Initialize(Board& board, int pattern) {
    board.pattern = pattern;
    board.planes = initial_planes;
    board.candidates.resize(3);
    for (int i = 0; i < 3; ++i) {
      board.candidates[i].reserve(kNumPoints);
      board.candidates[i] = initial_candidates[pattern][i];
    }
  }

  int pattern;
  std::array<Plane, 3> planes;
  std::vector<std::vector<Move>> candidates;

  static std::array<Plane, 3> initial_planes;
  static std::vector<std::vector<std::vector<Move>>> initial_candidates;
};
std::array<Plane, 3> Board::initial_planes;
std::vector<std::vector<std::vector<Move>>> Board::initial_candidates;

struct State {
  State(int initial_pattern, Random& rng) : pattern(initial_pattern) {
    Board::Initialize(board, pattern);
    board.PlayOut(moves, rng);
    next_moves = moves;
    best_score = score = ComputeScore();
  }

  double SuggestChange(Random& rng) {
    next_moves.clear();
    if (rng.Canonical() < 0.5) {
      Board::Initialize(board, pattern);
    } else {
      Board::Initialize(board, rng.Range(8));
    }
    bool prune = false;
    if (rng.Canonical() < 0.5) {
      const int min = Global::initial_range_min + 3;
      const int max = Global::initial_range_max - 3;
      keep.assign(kNumPoints, 0);
      for (const auto& [plane_index, rectangle] : moves) {
        const auto& [a, b] = rectangle;
        if (ComputeDistance(a, b) != 2) continue;
        const auto [x, y] = NormalizePosition(plane_index, a);
        if (x > min && x < max && y > min && y < max) continue;
        if (rng.Canonical() < 0.95) continue;
        keep[Position(x, y)] = 1;
        prune = true;
      }
      if (prune) {
        for (int i = moves.size() - 1; i >= 0; --i) {
          const auto vertices = ConvertMoveToVertices(moves[i]);
          if (!keep[vertices[0]]) continue;
          next_moves.push_back(moves[i]);
          for (int j = 1; j < 4; ++j) {
            keep[vertices[j]] = 1;
          }
        }
        std::reverse(next_moves.begin(), next_moves.end());
        for (const auto& [plane_index, rectangle] : next_moves) {
          board.MakeMove(plane_index, rectangle);
        }
      }
    }
    std::size_t move_index = moves.size();
    if (!prune) {
      move_index = rng.MinMax(0, move_index);
      if (rng.Canonical() < 0.8) {
        move_index = rng.MinMax(0, move_index);
      }
      for (std::size_t j = 0; j < move_index; ++j) {
        const auto& [plane_index, rectangle] = moves[j];
        board.MakeMove(plane_index, rectangle);
        next_moves.emplace_back(plane_index, rectangle);
      }
    }
    if (rng.Canonical() < 0.5) {
      candidates = board.candidates[2];
      while (!candidates.empty()) {
        const int index = rng.Range(candidates.size());
        std::swap(candidates.back(), candidates[index]);
        const auto [plane_index, rectangle] = candidates.back();
        candidates.pop_back();
        if (!board.IsValidMove(plane_index, rectangle)) continue;
        board.MakeMove(plane_index, rectangle);
        next_moves.emplace_back(plane_index, rectangle);
        break;
      }
    }
    if (rng.Canonical() < 0.8) {
      for (std::size_t j = move_index + 1; j < moves.size(); ++j) {
        const auto& [plane_index, rectangle] = moves[j];
        if (!board.IsValidMove(plane_index, rectangle)) continue;
        board.MakeMove(plane_index, rectangle);
        next_moves.emplace_back(plane_index, rectangle);
      }
    }
    board.PlayOut(next_moves, rng);
    next_score = ComputeScore();
    return next_score - score;
  }

  void AcceptChange() {
    pattern = board.pattern;
    moves.swap(next_moves);
    score = next_score;
    best_score = std::max(best_score, score);
  }

  double ComputeScore() const {
    double sum = 0;
    for (const auto& [plane_index, rectangle] : next_moves) {
      sum += Global::score_table[plane_index][rectangle.first];
    }
    return sum;
  }

  bool operator<(const State& other) const {
    return best_score < other.best_score;
  }

  double score;
  double next_score;
  double best_score;
  std::vector<Move> moves;
  std::vector<Move> next_moves;
  Board board;
  int pattern;
  std::vector<int> keep;
  std::vector<Move> candidates;
};

template <typename State>
auto Anneal(std::vector<State> states, Random& rng, double T0, double T1,
            Timer& timer, double time_limit) {
  const int num_levels = 32 - __builtin_clz(states.size());
  auto best_score = states[0].score;
  auto best_moves = states[0].moves;
  auto T = T0;
  const auto start = timer.GetTime();
  for (int level = 1; level <= num_levels; ++level) {
    for (int i = 0; ; ++i) {
      if ((i & ((1 << 4) - 1)) == 0) {
        const auto progress = (timer.GetTime() - start) / (time_limit - start);
        if (progress >= 1.0 * level / num_levels) break;
        T = std::pow(T0, 1.0 - progress) * std::pow(T1, progress);
      }
      for (auto& state : states) {
        const auto gain = state.SuggestChange(rng) / T;
        if (rng.Canonical() < std::exp(gain)) {
          state.AcceptChange();
          if (state.score > best_score) {
            best_score = state.score;
            best_moves = state.moves;
          }
        }
      }
    }
    const auto size = states.size() / 2;
    std::nth_element(states.rbegin(), states.rend() - size, states.rend());
    states.erase(states.begin() + size, states.end());
  }
  return best_moves;
}

std::vector<std::array<Position, 4>> Solve(int N, const std::vector<Position>& points) {
  Timer timer;

  Global::N = N;
  Global::initial_range_min = N / 4 + 1;
  Global::initial_range_max = 3 * N / 4 + 1;
  const int center = N / 2 + 1;
  for (int x = 1; x <= N; ++x) {
    for (int y = 1; y <= N; ++y) {
      const Position pos(x, y);
      const int dx = x - center;
      const int dy = y - center;
      Global::score_table[2][pos] = dx * dx + dy * dy + 1;
      const auto [odd, diagonal] = ToDiagonal(pos);
      Global::score_table[odd][diagonal] = Global::score_table[2][pos];
    }
  }
  Board::Precompute(points);

  Random rng(299792458);
  const double T0 = N * N * 1.0e0;
  const double T1 = T0 * 1.0e-2;
  std::vector<State> states;
  for (int i = 0; i < 32; ++i) {
    states.emplace_back(i % 8, rng);
  }
  const auto best_moves = Anneal(states, rng, T0, T1, timer, kTimeLimit);

  std::vector<std::array<Position, 4>> rectangles;
  for (const auto& [plane_index, rectangle] : best_moves) {
    rectangles.push_back(ConvertMoveToVertices({plane_index, rectangle}));
  }
  return rectangles;
}

int main() {
  int N, M;
  std::cin >> N >> M;
  std::vector<Position> points;
  for (int i = 0; i < M; ++i) {
    int x, y;
    std::cin >> x >> y;
    points.emplace_back(x + 1, y + 1);
  }

  const auto rectangles = Solve(N, points);
  std::cout << rectangles.size() << std::endl;
  for (const auto& [a, b, c, d] : rectangles) {
    std::cout << a.X() - 1 << ' ' << a.Y() - 1 << ' '
              << b.X() - 1 << ' ' << b.Y() - 1 << ' '
              << c.X() - 1 << ' ' << c.Y() - 1 << ' '
              << d.X() - 1 << ' ' << d.Y() - 1 << std::endl;
  }

  return 0;
}
