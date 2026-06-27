#include <algorithm>
#include <chrono>
#include <deque>
#include <iostream>
#include <limits>
#include <queue>
#include <random>
#include <set>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <cctype>
#include <atcoder/dsu>

constexpr double kTimeLimit = 3.0;
constexpr int kGridSize = 30;
constexpr int kPaddedGridSize = kGridSize + 2;
constexpr int kNumCells = kPaddedGridSize * kPaddedGridSize;
constexpr int kNumTurns = 300;

const std::vector<std::tuple<char, int>> kDirections = {
  {'L', -1}, {'R', 1}, {'U', -kPaddedGridSize}, {'D', kPaddedGridSize},
};

constexpr int ToCellIndex(int row, int column) {
  return kPaddedGridSize * row + column;
}

auto Now() {
  return std::chrono::high_resolution_clock::now();
}

struct Timer {
  Timer() : start(Now()) {}

  double GetTime() {
    return std::chrono::duration<double>(Now() - start).count();
  }

  const decltype(Now()) start;
};

class Position {
 public:
  explicit Position(int index) : index_(index) {}
  Position(int y, int x) : index_(ToCellIndex(y, x)) {}

  constexpr int Y() const {
    return index_ / kPaddedGridSize;
  }

  constexpr int X() const {
    return index_ % kPaddedGridSize;
  }

  template <std::size_t Index>
  constexpr int get() const {
    if constexpr (Index == 0) {
      return Y();
    } else {
      return X();
    }
  }

  operator int() const {
    return index_;
  }

  Position& operator+=(int delta) {
    index_ += delta;
    return *this;
  }

  friend Position operator+(Position position, int delta) {
    return position += delta;
  }

 private:
  int index_;
};

template <>
struct std::tuple_size<Position> : std::integral_constant<std::size_t, 2> {};

template <std::size_t Index>
struct std::tuple_element<Index, Position> {
  using type = int;
};

struct Pet {
  enum class Type {
    kCow = 1,
    kPig = 2,
    kRabbit = 3,
    kDog = 4,
    kCat = 5,
  };

  Pet(int id_, Type type_, Position position_) :
      id(id_), type(type_), position(position_), captured(false) {}

  const int id;
  const Type type;
  Position position;
  bool captured;
};

struct Worker {
  enum class Phase {
    kSpreading,
    kBuilding,
    kCapturing,
  };

  Worker(int id_, Position position_) :
      id(id_), phase(Phase::kSpreading), position(position_) {}

  const int id;
  Phase phase;
  Position position;
  std::deque<Position> route;
};

struct State {
  State(const std::vector<std::tuple<int, int, int>>& pets_,
        const std::vector<std::tuple<int, int>>& workers_) :
      num_blockers(kNumCells) {
    for (std::size_t i = 0; i < pets_.size(); ++i) {
      const auto& [y, x, t] = pets_[i];
      const Position position(y, x);
      pets.emplace_back(i, static_cast<Pet::Type>(t), position);
      ++num_blockers[position];
      for (const auto& [move, delta] : kDirections) {
        ++num_blockers[position + delta];
      }
    }
    for (std::size_t i = 0; i < workers_.size(); ++i) {
      const auto& [y, x] = workers_[i];
      const Position position(y, x);
      workers.emplace_back(i, position);
      ++num_blockers[position];
    }
  }

  void InitializeCountsInRooms(int num_rooms,
                               const std::vector<int>& room_index_map) {
    room_pet_counts.resize(num_rooms);
    for (const auto& pet : pets) {
      const int room = room_index_map[pet.position];
      if (room < 0) continue;
      ++room_pet_counts[room];
    }
    room_worker_counts.resize(num_rooms);
    for (const auto& worker : workers) {
      const int room = room_index_map[worker.position];
      if (room < 0) continue;
      ++room_worker_counts[room];
    }
  }

  void MovePets(const std::vector<std::string>& moves,
                const std::vector<int>& room_index_map) {
    for (auto& pet : pets) {
      Position position = pet.position;
      for (const auto& move : moves[pet.id]) {
        for (const auto& [move2, delta] : kDirections) {
          if (move2 == move) {
            position += delta;
          }
        }
      }
      MovePet(pet, position, room_index_map);
    }
  }

  void MovePet(Pet& pet, Position destination,
               const std::vector<int>& room_index_map) {
    --num_blockers[pet.position];
    for (const auto& [move, delta] : kDirections) {
      --num_blockers[pet.position + delta];
    }
    if (const int room = room_index_map[pet.position]; room >= 0) {
      --room_pet_counts[room];
    }
    pet.position = destination;
    ++num_blockers[pet.position];
    for (const auto& [move, delta] : kDirections) {
      ++num_blockers[pet.position + delta];
    }
    if (const int room = room_index_map[destination]; room >= 0) {
      ++room_pet_counts[room];
    }
  }

  void MoveWorker(Worker& worker, Position destination,
                  const std::vector<int>& room_index_map) {
    --num_blockers[worker.position];
    ++num_blockers[destination];
    if (const int room = room_index_map[worker.position]; room >= 0) {
      --room_worker_counts[room];
    }
    if (const int room = room_index_map[destination]; room >= 0) {
      ++room_worker_counts[room];
    }
    worker.position = destination;
  }

  std::vector<Pet> pets;
  std::vector<Worker> workers;
  std::vector<int> num_blockers;
  std::vector<int> room_pet_counts;
  std::vector<int> room_worker_counts;
};

enum class Cell {
  kEmpty,
  kReserved,
  kImpassible,
};

class Solver {
 public:
  Solver(const std::vector<std::tuple<int, int, int>>& pets,
         const std::vector<std::tuple<int, int>>& workers) :
      turn_(-1), cells_(kNumCells), room_index_map_(kNumCells, -1),
      adjacent_positions_(kNumCells),
      distance_table_(kNumCells, std::vector<int>(kNumCells)),
      distance_table_dirty_(true), state_(pets, workers), last_penalty_(0.0) {
    if (workers.size() < 7 && pets.size() > 5 * workers.size() - 15) {
      room_height_ = 3;
      passages_ = {5, 12, 19, 26};
    } else if (workers.size() > 6 && workers.size() < 9 &&
               pets.size() > 3 * workers.size() - 7) {
      room_height_ = 2;
      passages_ = {5, 12, 19, 26};
    } else if ((workers.size() < 6 && pets.size() < 11) ||
               (workers.size() > 5 && workers.size() < 8 && pets.size() > 11)) {
      room_height_ = 3;
      passages_ = {4, 9, 15, 21, 27};
    } else {
      room_height_ = 3;
      passages_ = {4, 9, 15, 20, 25, 30};
    }

    num_passages_ = passages_.size();
    num_rooms_per_row_ = num_passages_ + (passages_.back() < kGridSize);
    num_rooms_ = num_rooms_per_row_ * (kGridSize / room_height_);
    room_unavailabilities_.resize(num_rooms_);
    door_positions_.resize(num_rooms_);
    closer_positions_.resize(num_rooms_);

    for (int i = 0; i < kPaddedGridSize; ++i) {
      cells_[ToCellIndex(0, i)] = Cell::kImpassible;
      cells_[ToCellIndex(kGridSize + 1, i)] = Cell::kImpassible;
      cells_[ToCellIndex(i, 0)] = Cell::kImpassible;
      cells_[ToCellIndex(i, kGridSize + 1)] = Cell::kImpassible;
    }
    if (room_height_ == 3) {
      for (const int i : {1, 2, kGridSize}) {
        for (const int j : passages_) {
          if (i == 1 && (j == 1 || j == kGridSize)) continue;
          cells_[ToCellIndex(i, j)] = Cell::kReserved;
        }
      }
      for (int i = 4; i < kGridSize; i += 3) {
        for (int j = 1; j <= kGridSize; ++j) {
          cells_[ToCellIndex(i, j)] = Cell::kReserved;
        }
        for (const int j : passages_) {
          for (int dj = -1; dj <= 1; ++dj) {
            if (j + dj < 1 || j + dj > kGridSize) continue;
            cells_[ToCellIndex(i, j + dj)] = Cell::kEmpty;
          }
        }
      }
      for (int i = 6; i < kGridSize; i += 3) {
        for (const int j : passages_) {
          for (int dj = -1; dj <= 1; dj += 2) {
            if (j + dj < 1 || j + dj > kGridSize) continue;
            cells_[ToCellIndex(i, j + dj)] = Cell::kReserved;
          }
        }
      }
      for (int k = 0; k < num_rooms_per_row_; ++k) {
        for (int i = 1; i <= kGridSize; ++i) {
          if (i != 1 && i % 3 == 1) continue;
          int left = (k == 0) ? 1 : passages_[k - 1] + 2;
          int right = (k == num_passages_) ? kGridSize : passages_[k] - 2;
          if (i <= 3 || i >= kGridSize - 1) {
            left -= k > 0;
            right += k < num_passages_;
          }
          for (int j = left; j <= right; ++j) {
            room_index_map_[ToCellIndex(i, j)] = num_rooms_per_row_ * ((i - 1) / 3) + k;
          }
        }
      }
      for (int k = 0; k < num_rooms_per_row_; ++k) {
        for (int i = 2; i <= kGridSize; i += 3) {
          const int y = std::max(3, i);
          const int room = num_rooms_per_row_ * ((i - 1) / room_height_) + k;
          if (k > 0) {
            door_positions_[room].emplace_back(y, passages_[k - 1] + 1);
            closer_positions_[room].emplace_back(y, passages_[k - 1]);
          }
          if (k < num_passages_) {
            door_positions_[room].emplace_back(y, passages_[k] - 1);
            closer_positions_[room].emplace_back(y, passages_[k]);
          }
        }
      }
    } else {
      for (const int j : passages_) {
        cells_[ToCellIndex(1, j)] = Cell::kReserved;
      }
      for (int i = 3; i < kGridSize; i += 2) {
        for (int j = 1; j <= kGridSize; ++j) {
          cells_[ToCellIndex(i, j)] = Cell::kReserved;
        }
        for (const int j : passages_) {
          for (int dj = -1; dj <= 1; ++dj) {
            if (j + dj < 1 || j + dj > kGridSize) continue;
            cells_[ToCellIndex(i, j + dj)] = Cell::kEmpty;
          }
        }
      }
      for (int k = 0; k < num_rooms_per_row_; ++k) {
        for (int i = 1; i <= kGridSize; ++i) {
          if (i != 1 && i % 2 == 1) continue;
          int left = (k == 0) ? 1 : passages_[k - 1] + 2;
          int right = (k == num_passages_) ? kGridSize : passages_[k] - 2;
          for (int j = left; j <= right; ++j) {
            room_index_map_[ToCellIndex(i, j)] = num_rooms_per_row_ * ((i - 1) / room_height_) + k;
          }
        }
      }
      for (int k = 0; k < num_rooms_per_row_; ++k) {
        for (int i = 2; i <= kGridSize; i += 2) {
          const int room = num_rooms_per_row_ * ((i - 1) / room_height_) + k;
          if (k > 0) {
            door_positions_[room].emplace_back(i, passages_[k - 1] + 1);
            closer_positions_[room].emplace_back(i, passages_[k - 1]);
          }
          if (k < num_passages_) {
            door_positions_[room].emplace_back(i, passages_[k] - 1);
            closer_positions_[room].emplace_back(i, passages_[k]);
          }
        }
      }
    }

    for (int i = 0; i < kNumCells; ++i) {
      if (cells_[i] != Cell::kReserved) continue;
      for (const auto& [move, delta] : kDirections) {
        const int room = room_index_map_[i + delta];
        if (room < 0) continue;
        ++room_unavailabilities_[room];
      }
    }

    state_.InitializeCountsInRooms(num_rooms_, room_index_map_);

    std::pair<int, int> best_cost(1000, 1000);
    std::vector<int> best_assignment;
    const int num_workers = state_.workers.size();
    std::vector<int> assignment(num_workers);
    for (int i = 0; i < num_workers; ++i) {
      assignment[i] = i;
    }
    std::vector<int> num_rows(num_workers);
    for (int i = 0; i < num_workers; ++i) {
      if (room_height_ == 3) {
        num_rows[i] = 10 * (i + 1) / num_workers - 10 * i / num_workers;
      } else if (room_height_ == 2) {
        num_rows[i] = 8 * (i + 1) / num_workers - 8 * i / num_workers;
      }
    }
    std::sort(num_rows.rbegin(), num_rows.rend());
    const int max = *std::max_element(num_rows.rbegin(), num_rows.rend());
    const int shift = std::count(num_rows.begin(), num_rows.end(), max) / 2;
    std::rotate(num_rows.begin(), num_rows.begin() + shift, num_rows.end());
    std::vector<int> ys(num_workers, 2);
    for (int i = 1; i < num_workers; ++i) {
      ys[i] = ys[i - 1] + num_rows[i - 1] * (6 - room_height_);
    }
    do {
      std::pair<int, int> cost;
      for (int i = 0; i < num_workers; ++i) {
        const auto [y, x] = state_.workers[i].position;
        const int target_y = ys[assignment[i]];
        const int d = std::abs(y - target_y) + std::min(x - 1, kGridSize - x);
        cost.first = std::max(cost.first, d);
        cost.second += d;
      }
      if (cost < best_cost) {
        best_cost = cost;
        best_assignment = assignment;
      }
    } while (std::next_permutation(assignment.begin(), assignment.end()));
    for (int i = 0; i < num_workers; ++i) {
      auto& worker = state_.workers[i];
      const int target_y = ys[best_assignment[i]];
      const int target_x = worker.position.X() <= kGridSize / 2 ? 1 : kGridSize;
      worker.route.emplace_back(target_y, target_x);
      if (best_assignment[i] == 0 && num_rows[best_assignment[i]] == 1 &&
          ((target_x == 1 && passages_.back() == kGridSize) ||
           (target_x == kGridSize && passages_.front() == 1))) {
        worker.route.emplace_back(target_y, kGridSize + 1 - target_x);
        worker.route.emplace_back(target_y + 1, kGridSize + 1 - target_x);
      } else {
        worker.route.emplace_back(target_y, kGridSize + 1 - target_x);
      }
      if (num_rows[best_assignment[i]] == 2) {
        if (room_height_ == 3) {
          worker.route.emplace_back(target_y + 3, kGridSize + 1 - target_x);
          worker.route.emplace_back(target_y + 3, target_x);
        } else {
          worker.route.emplace_back(target_y + 4, kGridSize + 1 - target_x);
          worker.route.emplace_back(target_y + 4, target_x);
        }
      }
    }
  }

  std::string DecideActions(int turn) {
    turn_ = turn;
    moves_.assign(state_.workers.size(), '.');
    last_worker_positions_.clear();
    for (const auto& worker : state_.workers) {
      last_worker_positions_.push_back(worker.position);
      ++state_.num_blockers[worker.position];
    }

    int remaining = 0;
    for (const auto& pet : state_.pets) {
      remaining += !pet.captured;
    }
    if (remaining == 0) {
      return moves_;
    }

    MoveBuilders();
    CaptureIfPossible();
    if (turn_ + 1 == kNumTurns ||
        (turn_ + 20 >= kNumTurns && last_penalty_ + 1 > kNumTurns)) {
      if (TryLastMove()) {
        return moves_;
      }
    }
    MoveCapturers();
    for (const auto& position : last_worker_positions_) {
      --state_.num_blockers[position];
    }
    return moves_;
  }

  void MovePets(const std::vector<std::string>& moves) {
    last_pet_positions_.clear();
    for (auto& pet : state_.pets) {
      Position next = pet.position;;
      for (const auto& [move, delta] : kDirections) {
        if (move != moves[pet.id][0]) continue;
        next += delta;
      }
      last_pet_positions_.emplace_back(pet.position, next);
    }
    state_.MovePets(moves, room_index_map_);
  }

 private:
  void MoveBuilders() {
    for (auto& worker : state_.workers) {
      if (worker.phase == Worker::Phase::kCapturing) continue;
      MoveWorker(worker);
    }
  }

  void CaptureIfPossible() {
    for (int room = 0; room < num_rooms_; ++room) {
      if (room_unavailabilities_[room] > 0) continue;
      if (state_.room_pet_counts[room] == 0) continue;
      if (state_.room_worker_counts[room] > 0) continue;
      std::vector<int> closers;
      for (std::size_t i = 0; i < closer_positions_[room].size(); ++i) {
        if (state_.num_blockers[door_positions_[room][i]] > 0) break;
        for (const auto& worker : state_.workers) {
          if (moves_[worker.id] != '.') continue;
          if (worker.position != closer_positions_[room][i]) continue;
          closers.push_back(worker.id);
          break;
        }
      }
      if (closers.size() != closer_positions_[room].size()) continue;
      std::vector<int> pet_indices;
      for (const auto& pet : state_.pets) {
        if (room_index_map_[pet.position] != room) continue;
        pet_indices.push_back(pet.id);
      }
      for (const auto& closer : closers) {
        const auto& worker = state_.workers[closer];
        const int k = room % num_rooms_per_row_;
        if (k < num_passages_ && worker.position.X() == passages_[k]) {
          moves_[closer] = 'l';
        } else {
          moves_[closer] = 'r';
        }
        for (const auto& [move, delta] : kDirections) {
          if (std::tolower(move) != moves_[closer]) continue;
          cells_[worker.position + delta] = Cell::kImpassible;
        }
      }
      distance_table_dirty_ = true;
      for (const auto& index : pet_indices) {
        state_.pets[index].captured = true;
      }
      ++room_unavailabilities_[room];

      if (door_positions_[room].size() == 1) continue;
      int num_closed = 0;
      const int column = room % num_rooms_per_row_;
      for (int i = column; i < num_rooms_; i += num_rooms_per_row_) {
        num_closed += cells_[door_positions_[i][0]] == Cell::kImpassible;
      }
      if (num_closed + 1 == num_rooms_ / num_rooms_per_row_) {
        for (int i = column; i < num_rooms_; i += num_rooms_per_row_) {
          ++room_unavailabilities_[i];
        }
      }
    }
  }

  void MoveCapturers() {
    std::set<int> capturers;
    std::vector<int> assigned_times(kNumCells, -1);
    for (const auto& worker : state_.workers) {
      if (worker.phase == Worker::Phase::kCapturing) {
        if (moves_[worker.id] != '.') {
          assigned_times[worker.position] = turn_;
        } else {
          capturers.insert(worker.id);
        }
      }
    }
    if (capturers.empty()) return;
    UpdateDistanceTable();

    std::vector<Position> available_positions;
    for (int i = 0; i < kNumCells; ++i) {
      if (cells_[i] == Cell::kImpassible) continue;
      available_positions.emplace_back(i);
    }
    std::vector<std::vector<int>> initial_target_candidates(state_.pets.size());
    for (const auto& pet : state_.pets) {
      const auto& [from, to] = last_pet_positions_[pet.id];
      if (pet.type == Pet::Type::kDog) {
        for (const auto& worker : state_.workers) {
          const auto& distances = distance_table_[worker.position];
          if (distances[to] < distances[from]) {
            initial_target_candidates[pet.id].push_back(worker.id);
          }
        }
      }
      if (pet.type == Pet::Type::kCat) {
        for (int i = 0; i < kNumCells; ++i) {
          if (cells_[i] == Cell::kImpassible) continue;
          if (distance_table_[i][to] < distance_table_[i][from]) {
            initial_target_candidates[pet.id].push_back(i);
          }
        }
      }
    }

    const double now = timer_.GetTime();
    const double time_limit = now + 0.95 * (0.9 * kTimeLimit - now) / (kNumTurns - turn_);
    int num_trials = 0;
    using T = std::pair<int, int>;
    std::vector<std::priority_queue<T, std::vector<T>, std::greater<T>>> queues(num_rooms_);
    std::vector<int> sums(num_rooms_);
    for (; ; ++num_trials) {
      if (num_trials >= 10 && timer_.GetTime() > time_limit) break;
      std::vector<std::vector<int>> times(num_rooms_);
      auto simulated = state_;
      for (const auto& worker : simulated.workers) {
        --simulated.num_blockers[worker.position];
        --simulated.num_blockers[last_worker_positions_[worker.id]];
      }
      std::vector<int> pet_targets(simulated.pets.size(), -1);
      for (const auto& pet : simulated.pets) {
        if (initial_target_candidates[pet.id].empty()) continue;
        pet_targets[pet.id] = Choose(initial_target_candidates[pet.id]);
      }
      const auto move_standard = [&](Position& position) {
        const auto& candidates = adjacent_positions_[position];
        const Position next = Choose(candidates);
        position = next;
      };
      const auto move_toward_target = [&](Position& position, Position target) {
        std::vector<Position> candidates;
        for (const auto& [move, delta] : kDirections) {
          const auto next = position + delta;
          if (cells_[next] == Cell::kImpassible) continue;
          if (distance_table_[target][next] < distance_table_[target][position]) {
            candidates.push_back(next);
          }
        }
        const Position next = Choose(candidates);
        position = next;
      };
      const auto move_dog = [&](const Pet& pet, Position& position) {
        auto& target = pet_targets[pet.id];
        while (target < 0 || distance_table_[simulated.workers[target].position][position] <= 0) {
          target = Choose(simulated.workers).id;
        }
        move_toward_target(position, simulated.workers[target].position);
        if (target >= 0 && position == simulated.workers[target].position) {
          target = -1;
        }
        move_standard(position);
        if (target >= 0 && position == simulated.workers[target].position) {
          target = -1;
        }
      };
      const auto move_cat = [&](const Pet& pet, Position& position) {
        auto& target = pet_targets[pet.id];
        while (target < 0 || distance_table_[target][position] <= 0) {
          target = Choose(available_positions);
        }
        move_toward_target(position, Position(target));
        if (target >= 0 && position == target) {
          target = -1;
        }
        move_standard(position);
        if (target >= 0 && position == target) {
          target = -1;
        }
      };

      for (int t = turn_; t < std::min(kNumTurns, turn_ + 20); ++t) {
        for (int room = 0; room < num_rooms_; ++room) {
          if (room_unavailabilities_[room] > 0) continue;
          if (simulated.room_pet_counts[room] == 0) continue;
          bool has_blocker = false;
          for (const auto& position : door_positions_[room]) {
            if (simulated.num_blockers[position] > 0) {
              has_blocker = true;
              break;
            }
          }
          if (has_blocker) continue;
          times[room].push_back(t);
        }

        for (auto& pet : simulated.pets) {
          if (pet.captured) continue;
          Position position = pet.position;
          if (pet.type == Pet::Type::kDog) {
            move_dog(pet, position);
          } else if (pet.type == Pet::Type::kCat) {
            move_cat(pet, position);
          } else {
            for (int k = 0; k < static_cast<int>(pet.type); ++k) {
              move_standard(position);
            }
          }
          simulated.MovePet(pet, position, room_index_map_);
        }
      }
      for (int room = 0; room < num_rooms_; ++room) {
        times[room].push_back(kNumTurns);
        sums[room] += times[room][0];
        for (std::size_t i = 1; i < times[room].size(); ++i) {
          queues[room].emplace(times[room][i - 1], times[room][i]);
        }
      }
    }

    std::vector<bool> closed(num_rooms_);
    for (int room = 0; room < num_rooms_; ++room) {
      closed[room] = room_unavailabilities_[room] > 0;
    }
    last_penalty_ = std::numeric_limits<double>::max();
    while (!capturers.empty()) {
      int best_room = -1;
      double best_penalty = std::numeric_limits<double>::infinity();
      std::vector<std::tuple<int, int, Position>> best_closers;
      for (int room = 0; room < num_rooms_; ++room) {
        if (closed[room]) continue;
        int earliest = turn_;
        std::vector<Position> targets;
        for (const auto& position : closer_positions_[room]) {
          if (assigned_times[position] >= 0) {
            earliest = std::max(earliest, assigned_times[position]);
          } else {
            targets.push_back(position);
          }
        }
        if (targets.empty()) continue;
        if (targets.size() > capturers.size()) continue;
        std::vector<std::vector<std::pair<int, int>>> candidates(targets.size());
        for (std::size_t i = 0; i < targets.size(); ++i) {
          for (const auto& capturer : capturers) {
            const auto distance = distance_table_[targets[i]][state_.workers[capturer].position];
            candidates[i].emplace_back(turn_ + distance, capturer);
          }
          std::sort(candidates[i].begin(), candidates[i].end());
        }
        if (targets.size() == 2) {
          if (candidates[0][0].second == candidates[1][0].second) {
            const int time0 = std::max(candidates[1][0].first, candidates[0][1].first);
            const int time1 = std::max(candidates[0][0].first, candidates[1][1].first);
            if (time0 < time1) {
              std::swap(candidates[0][0], candidates[0][1]);
            } else {
              std::swap(candidates[1][0], candidates[1][1]);
            }
          }
        }
        std::vector<std::tuple<int, int, Position>> closers;
        for (std::size_t i = 0; i < targets.size(); ++i) {
          const auto& [time, closer] = candidates[i][0];
          closers.emplace_back(time, closer, targets[i]);
          earliest = std::max(earliest, time);
        }
        while (!queues[room].empty() && queues[room].top().first < earliest) {
          const auto& [current, next] = queues[room].top();
          sums[room] += next - current;
          queues[room].pop();
        }

        if (sums[room] == kNumTurns * num_trials) continue;
        double penalty = 1.0 * sums[room] / num_trials;
        penalty += 0.1 * (earliest > turn_) + 0.01 * (earliest - turn_);
        penalty += 0.01 * targets.size();

        if (penalty < best_penalty) {
          best_penalty = penalty;
          best_room = room;
          best_closers = closers;
        }
      }
      last_penalty_ = std::min(last_penalty_, best_penalty);

      if (best_room < 0) break;
      closed[best_room] = true;
      for (const auto& [time, closer, target] : best_closers) {
        assigned_times[target] = time;
        state_.workers[closer].route = {target};
        MoveWorker(state_.workers[closer]);
        capturers.erase(closer);
      }
    }
  }

  bool TryLastMove() {
    UpdateDistanceTable();
    const int num_workers = state_.workers.size();
    int available_workers = 0;
    std::vector<int> candidate_directions(num_workers, -1);
    for (const auto& worker : state_.workers) {
      if (moves_[worker.id] != '.') continue;
      int best_distance = kNumCells;
      for (int i = 0; i < 4; ++i) {
        const auto& [move, delta] = kDirections[i];
        const auto position = worker.position + delta;
        if (cells_[position] == Cell::kImpassible) continue;
        if (state_.num_blockers[position] > 0) continue;
        int distance = kNumCells;
        for (const auto& pet : state_.pets) {
          if (pet.captured) continue;
          distance = std::min(distance, distance_table_[position][pet.position]);
        }
        if (distance < best_distance) {
          best_distance = distance;
          candidate_directions[worker.id] = i;
        }
      }
      if (candidate_directions[worker.id] < 0) continue;
      available_workers |= 1 << worker.id;
    }
    int best_score = ComputeScore();
    std::string best_moves = moves_;
    for (int i = 0; i < (1 << num_workers); ++i) {
      if (i & ~available_workers) continue;
      for (int j = 0; j < num_workers; ++j) {
        if (!((i >> j) & 1)) continue;
        const auto& [move, delta] = kDirections[candidate_directions[j]];
        const auto position = state_.workers[j].position + delta;
        cells_[position] = Cell::kImpassible;
      }
      const int score = ComputeScore();
      if (score > best_score) {
        best_score = score;
        best_moves = moves_;
        for (int j = 0; j < num_workers; ++j) {
          if (!((i >> j) & 1)) continue;
          const auto& [move, delta] = kDirections[candidate_directions[j]];
          best_moves[j] = std::tolower(move);
        }
      }
      for (int j = 0; j < num_workers; ++j) {
        if (!((i >> j) & 1)) continue;
        const auto& [move, delta] = kDirections[candidate_directions[j]];
        const auto position = state_.workers[j].position + delta;
        cells_[position] = Cell::kEmpty;
      }
    }
    if (best_score >= 40'000'000 || turn_ + 1 == kNumTurns) {
      moves_ = best_moves;
      for (auto& pet : state_.pets) {
        pet.captured = true;
      }
      return true;
    }
    return false;
  }

  int ComputeScore() const {
    atcoder::dsu dsu(kNumCells);
    for (int i = 0; i < kNumCells; ++i) {
      if (cells_[i] == Cell::kImpassible) continue;
      for (const auto& [move, delta] : kDirections) {
        if (cells_[i + delta] != Cell::kImpassible) {
          dsu.merge(i, i + delta);
        }
      }
    }
    std::vector<int> num_pets(kNumCells);
    for (const auto& pet : state_.pets) {
      ++num_pets[dsu.leader(pet.position)];
    }
    double sum = 0.0;
    for (const auto& worker : state_.workers) {
      double s = 1.0 * dsu.size(worker.position) / (kGridSize * kGridSize);
      s /= 1 << num_pets[dsu.leader(worker.position)];
      sum += s;
    }
    return std::round(1.0e8 * sum / state_.workers.size());
  }

  void UpdateDistanceTable() {
    if (!distance_table_dirty_) return;
    distance_table_dirty_ = false;
    for (int i = 0; i < kNumCells; ++i) {
      adjacent_positions_[i].clear();
      if (cells_[i] == Cell::kImpassible) continue;
      for (const auto& [move, delta] : kDirections) {
        const auto next = i + delta;
        if (cells_[next] == Cell::kImpassible) continue;
        adjacent_positions_[i].emplace_back(next);
      }
      auto& distances = distance_table_[i];
      std::fill(distances.begin(), distances.end(), -1);
      std::queue<int> queue;
      distances[i] = 0;
      queue.push(i);
      while (!queue.empty()) {
        const auto position = queue.front();
        queue.pop();
        for (const auto& [move, delta] : kDirections) {
          const auto next = position + delta;
          if (cells_[next] == Cell::kImpassible) continue;
          if (distances[next] >= 0) continue;
          distances[next] = distances[position] + 1;
          queue.push(next);
        }
      }
    }
  }

  void MoveWorker(Worker& worker) {
    if (worker.position == worker.route.front()) {
      if (worker.phase == Worker::Phase::kSpreading) {
        worker.phase = Worker::Phase::kBuilding;
      }
      worker.route.pop_front();
    }
    const bool can_skip = worker.phase != Worker::Phase::kBuilding;
    std::vector<int> distances(kNumCells, -2);
    if (!worker.route.empty()) {
      std::queue<Position> queue;
      distances[worker.route.front()] = 0;
      queue.push(worker.route.front());
      while (!queue.empty()) {
        const auto position = queue.front();
        queue.pop();
        if (position == worker.position) break;
        for (const auto& [move, delta] : kDirections) {
          const auto next = position + delta;
          if (cells_[next] == Cell::kImpassible) continue;
          if (distances[next] >= 0) continue;
          distances[next] = distances[position] + 1;
          queue.push(next);
        }
      }
    }
    int num_next = 0;
    for (const auto& [move, delta] : kDirections) {
      const auto next = worker.position + delta;
      num_next += distances[next] == distances[worker.position] - 1;
    }
    bool can_move_forward = true;
    for (const auto& [move, delta] : kDirections) {
      const auto next = worker.position + delta;
      if (can_skip && (next.X() == 1 || next.X() == kGridSize)) continue;
      if (can_skip && next.Y() == 2) continue;
      if (num_next != 1 || distances[next] != distances[worker.position] - 1) {
        if (cells_[next] == Cell::kReserved) {
          can_move_forward = can_skip;
          if (state_.num_blockers[next] == 0) {
            cells_[next] = Cell::kImpassible;
            moves_[worker.id] = std::tolower(move);
            distance_table_dirty_ = true;
            for (const auto& [move2, delta2] : kDirections) {
              const int room = room_index_map_[next + delta2];
              if (room < 0) continue;
              --room_unavailabilities_[room];
            }
            return;
          }
        }
      }
    }
    if (can_move_forward) {
      if (worker.route.empty()) {
        worker.phase = Worker::Phase::kCapturing;
        return;
      }
      for (const auto& [move, delta] : kDirections) {
        const auto next = worker.position + delta;
        if (distances[next] != distances[worker.position] - 1) continue;
        moves_[worker.id] = move;
        state_.MoveWorker(worker, next, room_index_map_);
        if (next == worker.route.front()) {
          if (worker.phase == Worker::Phase::kSpreading) {
            worker.phase = Worker::Phase::kBuilding;
          }
          worker.route.pop_front();
        }
        return;
      }
    }
  }

  template <typename T>
  const T& Choose(const std::vector<T>& population) {
    return population[rng_() % population.size()];
  }

  int turn_;
  int room_height_;
  int num_rooms_;
  int num_rooms_per_row_;
  int num_passages_;
  std::vector<int> passages_;
  std::vector<Cell> cells_;
  std::vector<std::vector<Position>> closer_positions_;
  std::vector<std::vector<Position>> door_positions_;
  std::vector<int> room_index_map_;
  std::vector<int> room_unavailabilities_;
  std::vector<std::vector<Position>> adjacent_positions_;
  std::vector<std::vector<int>> distance_table_;
  bool distance_table_dirty_;
  State state_;
  std::string moves_;
  std::vector<Position> last_worker_positions_;
  std::vector<std::pair<Position, Position>> last_pet_positions_;
  double last_penalty_;
  std::mt19937 rng_;
  Timer timer_;
};

int main() {
  int N, M;
  std::cin >> N;
  std::vector<std::tuple<int, int, int>> pets(N);
  for (auto& [y, x, t] : pets) {
    std::cin >> y >> x >> t;
  }
  std::cin >> M;
  std::vector<std::tuple<int, int>> workers(M);
  for (auto& [y, x] : workers) {
    std::cin >> y >> x;
  }

  Solver solver(pets, workers);
  for (int turn = 0; turn < kNumTurns; ++turn) {
    std::cout << solver.DecideActions(turn) << std::endl;
    std::vector<std::string> moves(N);
    for (auto& move : moves) {
      std::cin >> move;
    }
    solver.MovePets(moves);
  }

  return 0;
}
