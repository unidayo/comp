#pragma GCC optimize("Ofast,unroll-all-loops")
#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <limits>
#include <numbers>
#include <tuple>
#include <vector>
#include <cmath>
#include <cstdint>

constexpr double kTimeLimit = 1.95;
constexpr int kNumTurns = 1000;
constexpr int kMaxInvestmentLevel = 20;
constexpr int kNumCardTypes = 5;
constexpr int kGenerationParameterLimits[kNumCardTypes] = {20, 10, 10, 5, 3};

namespace Global {

int num_cards_in_hand;
int num_projects;
int num_candidate_cards;

}

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
  constexpr explicit Random(std::uint64_t seed = 0) :
      state_(seed ^ 0x07a8ded236b339c8ull) {}

  constexpr double Canonical() {
    return (Next() >> 1) * 0x1p-63;
  }

  constexpr double Normal() {
    const auto x = Canonical();
    const auto y = Canonical();
    return std::sqrt(-2.0 * std::log(x)) * std::cos(2.0 * std::numbers::pi * y);
  }

  constexpr std::uint32_t Range(std::uint32_t size) {
    return __uint128_t(Next()) * size >> 64;
  }

  constexpr int MinMax(int min, int max) {
    return Range(max - min + 1) + min;
  }

 private:
  constexpr std::uint64_t Next() {
    const auto product = __uint128_t(state_ ^ 0xdbd2abd4066651c5ull) * state_;
    state_ += 0xdbd2abd4066651c5ull;
    return (product >> 64) ^ product;
  }

  std::uint64_t state_;
};

struct Card {
  enum class Type {
    kRegularWork,
    kHardWork,
    kCancel,
    kRestructuring,
    kInvestment,
  };

  Type type;
  std::int64_t labor_power;
  std::int64_t cost;
};

struct Project {
  std::int64_t remaining_work;
  std::int64_t value;
};

class Judge {
 public:
  void Initialize() {
    projects_.resize(Global::num_projects);
    for (int i = 0; i < Global::num_cards_in_hand; ++i) {
      std::int64_t t, w;
      std::cin >> t >> w;
      cards_.emplace_back(static_cast<Card::Type>(t), w, 0);
    }
    for (auto& [remaining_work, value] : projects_) {
      std::cin >> remaining_work >> value;
    }
  }

  auto GetState() {
    return std::make_tuple(cards_, projects_);
  }

  auto PlayCard(int card_index, int project_index) {
    std::cout << card_index << ' ' << project_index << std::endl;

    played_card_index_ = card_index;
    for (auto& [remaining_work, value] : projects_) {
      std::cin >> remaining_work >> value;
    }
    std::cin >> money_;
    candidate_cards_.clear();
    for (int i = 0; i < Global::num_candidate_cards; ++i) {
      std::int64_t t, w, p;
      std::cin >> t >> w >> p;
      candidate_cards_.emplace_back(static_cast<Card::Type>(t), w, p);
    }
    return std::make_tuple(money_, projects_, candidate_cards_);
  }

  void SelectCandidateCard(int candidate_index) {
    std::cout << candidate_index << std::endl;

    money_ -= candidate_cards_[candidate_index].cost;
    cards_[played_card_index_] = candidate_cards_[candidate_index];
  }

 private:
  int played_card_index_;
  std::int64_t money_;
  std::vector<Card> cards_;
  std::vector<Card> candidate_cards_;
  std::vector<Project> projects_;
};

namespace Global {

Judge judge;

}

class Generator {
 public:
  Generator(Random& rng) : rng_(rng), future_candidate_cards_(kNumTurns, std::vector<Card>(Global::num_candidate_cards)) {}

  void Initialize(const std::vector<int>& frequencies) {
    types_.clear();
    for (int type = 0; type < 2; ++type) {
      for (int i = 0; i < frequencies[type]; ++i) {
        types_.push_back(static_cast<Card::Type>(type));
      }
    }
    future_projects_.clear();
    future_candidate_cards_end_ = 0;
    Reset();
  }

  void Reset() {
    future_project_index_ = 0;
    future_candidate_cards_index_ = 0;
  }

  Project GetNextProject(int investment_level) {
    if (future_projects_.size() == future_project_index_) {
      const auto b = rng_.Canonical() * 6.0 + 2.0;
      const auto h = std::round(std::exp2(b));
      const auto v = std::round(std::exp2(std::clamp(rng_.Normal() * 0.5 + b, 0.0, 10.0)));
      future_projects_.emplace_back(h, v);
    }
    auto project = future_projects_[future_project_index_++];
    project.remaining_work *= 1 << investment_level;
    project.value *= 1 << investment_level;
    return project;
  }

  const std::vector<Card>& GetNextCandidateCards(int investment_level) {
    if (future_candidate_cards_index_ == future_candidate_cards_end_) {
      auto& cards = future_candidate_cards_[future_candidate_cards_end_++];
      cards[0] = {Card::Type::kRegularWork, 1, 0};
      for (int k = 1; k < Global::num_candidate_cards; ++k) {
        const auto type = types_[rng_.Range(types_.size())];
        const std::int64_t labor_power = rng_.MinMax(1, 50);
        const double x = rng_.Normal() * labor_power / 3.0 + labor_power;
        std::int64_t cost = 0;
        if (type == Card::Type::kRegularWork) {
          cost = std::clamp(std::round(x), 1.0, 10000.0);
        } else {
          cost = std::clamp(std::round(x * Global::num_projects), 1.0, 10000.0);
        }
        cards[k] = {type, labor_power, cost};
      }
    }
    candidate_cards_ = future_candidate_cards_[future_candidate_cards_index_++];
    for (auto& card : candidate_cards_) {
      card.labor_power *= 1 << investment_level;
      card.cost *= 1 << investment_level;
    }
    return candidate_cards_;
  }

 private:
  Random& rng_;
  std::vector<Card::Type> types_;
  std::size_t future_project_index_;
  std::size_t future_candidate_cards_index_;
  std::size_t future_candidate_cards_end_;
  std::vector<Project> future_projects_;
  std::vector<std::vector<Card>> future_candidate_cards_;
  std::vector<Card> candidate_cards_;
};

constexpr auto CreateDecayTable() {
  std::array<double, 1025> table = {};
  table[0] = 0.98;
  for (int i = 1; i <= 1024; ++i) {
    table[i] = table[i - 1] * 0.995;
  }
  return table;
}

constexpr auto kDecayTable = CreateDecayTable();

struct State {
  void PlayCard(int candidate_card_index, int card_index, int project_index,
                Generator* generator = nullptr) {
    if (candidate_card_index >= 0) {
      cards[played_card_index] = candidate_cards[candidate_card_index];
      money -= candidate_cards[candidate_card_index].cost;
      if (!generator) {
        Global::judge.SelectCandidateCard(candidate_card_index);
      }
    }

    ++turn;
    played_card_index = card_index;
    const auto& played_card = cards[card_index];
    if (played_card.type == Card::Type::kInvestment) {
      ++investment_level;
    }

    if (generator) {
      if (played_card.type == Card::Type::kRegularWork) {
        projects[project_index].remaining_work -= played_card.labor_power;
        if (projects[project_index].remaining_work <= 0) {
          money += projects[project_index].value;
          projects[project_index] = generator->GetNextProject(investment_level);
        }
      }
      if (played_card.type == Card::Type::kHardWork) {
        for (int j = 0; j < Global::num_projects; ++j) {
          projects[j].remaining_work -= played_card.labor_power;
          if (projects[j].remaining_work <= 0) {
            money += projects[j].value;
            projects[j] = generator->GetNextProject(investment_level);
          }
        }
      }
      if (played_card.type == Card::Type::kCancel) {
        projects[project_index] = generator->GetNextProject(investment_level);
      }
      if (played_card.type == Card::Type::kRestructuring) {
        for (int j = 0; j < Global::num_projects; ++j) {
          projects[j] = generator->GetNextProject(investment_level);
        }
      }
      candidate_cards = generator->GetNextCandidateCards(investment_level);
    } else {
      const auto& [updated_money, updated_projects, next_candidate_cards] =
          Global::judge.PlayCard(card_index, project_index);
      money = updated_money;
      projects = updated_projects;
      candidate_cards = next_candidate_cards;
    }
  }

  double Evaluate(int candidate_card_index, int card_index, int project_index) const {
    const auto& refilled_card = (candidate_card_index < 0) ?
                                Card{} : candidate_cards[candidate_card_index];

    double sum = money;
    if (candidate_card_index >= 0) {
      sum -= refilled_card.cost;
    }

    const auto& played_card = (card_index == played_card_index) ?
                              refilled_card : cards[card_index];

    int next_investment_level = investment_level;
    if (played_card.type == Card::Type::kInvestment) {
      ++next_investment_level;
    }
    const double speed = 1 << next_investment_level;
    sum += std::min(speed * 50.0, sum) * 0.5;
    sum += std::max(0, kNumTurns - turn - 20) * speed * 3.0;

    for (int i = 0; i < Global::num_cards_in_hand; ++i) {
      if (i == card_index) continue;
      const auto& card = (i == played_card_index) ? refilled_card : cards[i];
      if (card.type == Card::Type::kRegularWork) {
        sum += card.labor_power * 0.95;
      }
      if (card.type == Card::Type::kHardWork) {
        sum += card.labor_power * 1.9;
      }
      if (card.type == Card::Type::kCancel) {
        sum += speed * 5.0;
      }
      if (card.type == Card::Type::kRestructuring) {
        sum += speed * 5.0;
      }
    }

    const double new_project_bonus = speed * 5.0;
    if (played_card.type == Card::Type::kRestructuring) {
      sum += new_project_bonus * Global::num_projects;
    } else {
      for (int j = 0; j < Global::num_projects; ++j) {
        if (played_card.type == Card::Type::kCancel && j == project_index) {
          sum += new_project_bonus;
          continue;
        }
        auto remaining_work = projects[j].remaining_work;
        if (played_card.type == Card::Type::kHardWork ||
            (played_card.type == Card::Type::kRegularWork && j == project_index)) {
          remaining_work -= played_card.labor_power;
          if (remaining_work <= 0) {
            sum += projects[j].value;
            sum += new_project_bonus;
            continue;
          }
        }
        const auto value = projects[j].value - remaining_work;
        if (value <= 0) {
          sum += value * 0.1;
          continue;
        }
        sum += value * kDecayTable[remaining_work >> next_investment_level];
      }
    }

    return sum;
  }

  template <bool Initial>
  const std::vector<std::tuple<int, int, int>>& EnumerateMoves() {
    moves.clear();
    if constexpr (Initial) {
      for (int i = 0; i < Global::num_cards_in_hand; ++i) {
        if (cards[i].type == Card::Type::kRegularWork || cards[i].type == Card::Type::kCancel) {
          for (int j = 0; j < Global::num_projects; ++j) {
            moves.emplace_back(-1, i, j);
          }
        } else {
          moves.emplace_back(-1, i, 0);
        }
      }
    } else {
      for (int k = 0; k < Global::num_candidate_cards; ++k) {
        if (candidate_cards[k].cost > money) continue;
        for (int i = 0; i < Global::num_cards_in_hand; ++i) {
          const auto& card = (i == played_card_index) ? candidate_cards[k] : cards[i];
          if (card.type == Card::Type::kRegularWork || card.type == Card::Type::kCancel) {
            for (int j = 0; j < Global::num_projects; ++j) {
              moves.emplace_back(k, i, j);
            }
          } else {
            if (card.type == Card::Type::kInvestment && investment_level == kMaxInvestmentLevel) continue;
            moves.emplace_back(k, i, 0);
          }
        }
      }
    }
    return moves;
  }

  double Simulate(int candidate_card_index, int card_index, int project_index,
                  Generator& generator, int max_turns, bool use_score) const {
    generator.Reset();
    static State state;
    state = *this;
    state.PlayCard(candidate_card_index, card_index, project_index, &generator);
    std::int64_t max_money = state.money;
    const int end = std::min(turn + max_turns, kNumTurns);
    while (state.turn < end) {
      std::tuple<double, int, int, int> best_move;
      for (const auto& [k, i, j] : state.EnumerateMoves<false>()) {
        std::tuple<double, int, int, int> move(state.Evaluate(k, i, j), k, i, j);
        best_move = std::max(best_move, move);
      }
      const auto& [score, k, i, j] = best_move;
      if (use_score && state.turn + 1 == end) {
        return score;
      }
      state.PlayCard(k, i, j, &generator);
      max_money = std::max(max_money, state.money);
    }
    return max_money;
  }

  int turn;
  int investment_level;
  int played_card_index;
  std::int64_t money;
  std::vector<Card> cards;
  std::vector<Card> candidate_cards;
  std::vector<Project> projects;
  std::vector<std::tuple<int, int, int>> moves;
};

class Solver {
 public:
  Solver() : type_counts_(kNumCardTypes), estimated_parameters_(kNumCardTypes),
             log_ratios_(kGenerationParameterLimits[0] + 1), generator_(rng_) {
    const auto& [initial_cards, initial_projects] = Global::judge.GetState();
    state_.turn = 0;
    state_.investment_level = 0;
    state_.played_card_index = -1;
    state_.money = 0;
    state_.cards = initial_cards;
    state_.projects = initial_projects;

    for (const auto& card : initial_cards) {
      ++type_counts_[static_cast<int>(card.type)];
    }
    for (int i = 0; i < kNumCardTypes; ++i) {
      estimated_parameters_[i] = rng_.MinMax(1, kGenerationParameterLimits[i]);
    }
    for (int i = 0; i < 10; ++i) {
      UpdateEstimates();
    }
  }

  void DetermineMove(int turn) {
    const double time = timer_.GetTime();
    const double time_limit = time + (kTimeLimit - time) / (kNumTurns - turn) * 0.95;

    moves_.clear();
    if (turn == 0) {
      for (auto& [k, i, j] : state_.EnumerateMoves<true>()) {
        moves_.emplace_back(state_.Evaluate(k, i, j), k, i, j);
      }
    } else {
      for (auto& [k, i, j] : state_.EnumerateMoves<false>()) {
        moves_.emplace_back(state_.Evaluate(k, i, j), k, i, j);
      }
    }

    std::ranges::sort(moves_, std::greater{});
    moves_.resize(std::min<std::size_t>(moves_.size(), 8));
    for (auto& [score, k, i, j] : moves_) {
      score = 0.0;
    }

    while (timer_.GetTime() < time_limit) {
      UpdateEstimates();
      generator_.Initialize(estimated_parameters_);
      for (auto& [sum, k, i, j] : moves_) {
        sum += state_.Simulate(k, i, j, generator_, 4, turn + 10 <= kNumTurns);
      }
    }

    const auto& [score, k, i, j] = *std::ranges::max_element(moves_);
    state_.PlayCard(k, i, j);
    for (int index = 1; index < Global::num_candidate_cards; ++index) {
      ++type_counts_[static_cast<int>(state_.candidate_cards[index].type)];
    }
  }

 private:
  void UpdateEstimates() {
    for (int i = 0; i < kNumCardTypes; ++i) {
      double max = std::numeric_limits<double>::lowest();
      for (int j = 1; j <= kGenerationParameterLimits[i]; ++j) {
        estimated_parameters_[i] = j;
        double total = 0.0;
        for (int k = 0; k < kNumCardTypes; ++k) {
          total += estimated_parameters_[k];
        }
        double sum = 0.0;
        for (int k = 0; k < kNumCardTypes; ++k) {
          sum += std::log(estimated_parameters_[k] / total) * type_counts_[k];
        }
        log_ratios_[j] = sum;
        max = std::max(max, sum);
      }
      double sum = 0.0;
      for (int j = 1; j <= kGenerationParameterLimits[i]; ++j) {
        const auto p = std::exp(log_ratios_[j] - max);
        if (p == 0.0) continue;
        sum += p;
        if (rng_.Canonical() < p / sum) {
          estimated_parameters_[i] = j;
        }
      }
    }
  }

  std::vector<int> type_counts_;
  std::vector<int> estimated_parameters_;
  std::vector<double> log_ratios_;
  std::vector<std::tuple<double, int, int, int>> moves_;
  Timer timer_;
  Random rng_;
  State state_;
  Generator generator_;
};

int main() {
  int N, M, K, T;
  std::cin >> N >> M >> K >> T;

  Global::num_cards_in_hand = N;
  Global::num_projects = M;
  Global::num_candidate_cards = K;
  Global::judge.Initialize();

  Solver solver;
  for (int turn = 0; turn < kNumTurns; ++turn) {
    solver.DetermineMove(turn);
  }
  Global::judge.SelectCandidateCard(0);

  return 0;
}
