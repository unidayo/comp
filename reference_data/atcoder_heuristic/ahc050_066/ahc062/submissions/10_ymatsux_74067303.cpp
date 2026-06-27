#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

namespace {

using std::array;
using std::cin;
using std::cout;
using std::pair;
using std::vector;

constexpr int kN = 200;
constexpr int kRegularBlocks = 49;  // rows 0..195
constexpr int kPhases = 4;
constexpr int A_PHASE = 0;
constexpr int B_PHASE = 1;
constexpr int C_PHASE = 2;
constexpr int D_PHASE = 3;

struct Solver {
  using Clock = std::chrono::steady_clock;

  int n;
  vector<vector<int>> a;

  vector<array<int, 4>> permutations;
  bool ok_transition[24][24]{};

  long long time_index[kPhases][kRegularBlocks][kN]{};
  int row_of_phase[kRegularBlocks][kPhases][kN]{};

  vector<pair<int, int>> path;
  vector<vector<bool>> used;
  vector<vector<int>> pos_of;

  std::mt19937 rng;
  Clock::time_point program_start_time;

  explicit Solver(int n_in, Clock::time_point start_time)
      : n(n_in),
        a(n, vector<int>(n)),
        used(n, vector<bool>(n, false)),
        pos_of(n, vector<int>(n, -1)),
        rng(123456789),
        program_start_time(start_time) {
    for (int p = 0; p < kPhases; ++p) {
      for (int b = 0; b < kRegularBlocks; ++b) {
        for (int c = 0; c < kN; ++c) {
          time_index[p][b][c] = -1;
          row_of_phase[b][p][c] = -1;
        }
      }
    }
  }

  void Read() {
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        cin >> a[i][j];
      }
    }
  }

  double ElapsedSecFromProgramStart() const {
    return std::chrono::duration<double>(Clock::now() - program_start_time).count();
  }

  static bool IsAdjacent(const pair<int, int>& u, const pair<int, int>& v) {
    return std::max(std::abs(u.first - v.first), std::abs(u.second - v.second)) == 1;
  }

  static bool AdjCell(const pair<int, int>& x, const pair<int, int>& y) {
    if (x.first == y.first && x.second == y.second) {
      return false;
    }
    return std::max(std::abs(x.first - y.first), std::abs(x.second - y.second)) <= 1;
  }

  void AddCell(int r, int c) {
    if (r < 0 || r >= n || c < 0 || c >= n) {
      std::cerr << "Out of range: (" << r << ", " << c << ")\n";
      std::exit(1);
    }
    if (used[r][c]) {
      std::cerr << "Duplicated cell: (" << r << ", " << c << ")\n";
      std::exit(1);
    }
    if (!path.empty() && !IsAdjacent(path.back(), {r, c})) {
      std::cerr << "Non-adjacent move: (" << path.back().first << ", " << path.back().second
                << ") -> (" << r << ", " << c << ")\n";
      std::exit(1);
    }
    used[r][c] = true;
    path.emplace_back(r, c);
  }

  void BuildPermutations() {
    array<int, 4> p = {0, 1, 2, 3};
    do {
      permutations.push_back(p);
    } while (std::next_permutation(p.begin(), p.end()));
    assert(static_cast<int>(permutations.size()) == 24);

    for (int i = 0; i < 24; ++i) {
      for (int j = 0; j < 24; ++j) {
        bool ok = true;
        for (int ph = 0; ph < 4; ++ph) {
          if (std::abs(permutations[i][ph] - permutations[j][ph]) > 1) {
            ok = false;
            break;
          }
        }
        ok_transition[i][j] = ok;
      }
    }
  }

  array<int, 4> FixedPermAtCol1(int block) const {
    if ((block & 1) == 0) {
      return {1, 2, 3, 0};
    }
    return {2, 1, 0, 3};
  }

  array<int, 4> FixedPermAtCol198(int block) const {
    if ((block & 1) == 0) {
      return {2, 1, 0, 3};
    }
    return {1, 2, 3, 0};
  }

  static bool Compatible(const array<int, 4>& lhs, const array<int, 4>& rhs) {
    for (int ph = 0; ph < 4; ++ph) {
      if (std::abs(lhs[ph] - rhs[ph]) > 1) {
        return false;
      }
    }
    return true;
  }

  void PrecomputeTimes() {
    int day = 0;

    // Phase A on regular blocks.
    for (int b = 0; b < kRegularBlocks; ++b) {
      day += 2;
      if ((b & 1) == 0) {
        for (int c = 1; c <= 198; ++c) {
          time_index[A_PHASE][b][c] = day++;
        }
      } else {
        for (int c = 198; c >= 1; --c) {
          time_index[A_PHASE][b][c] = day++;
        }
      }
      day += 2;
    }

    // Special Phase A on rows 196..199.
    day += 4;
    day += 199;
    day += 3;
    assert(day == 10104);

    // Phase B on regular blocks.
    for (int b = kRegularBlocks - 1; b >= 0; --b) {
      day += 2;
      if ((b & 1) == 0) {
        for (int c = 1; c <= 198; ++c) {
          time_index[B_PHASE][b][c] = day++;
        }
      } else {
        for (int c = 198; c >= 1; --c) {
          time_index[B_PHASE][b][c] = day++;
        }
      }
      day += 2;
    }
    assert(day == 20002);

    // Phase C on regular blocks.
    for (int b = 0; b < kRegularBlocks; ++b) {
      if ((b & 1) == 0) {
        for (int c = 198; c >= 1; --c) {
          time_index[C_PHASE][b][c] = day++;
        }
      } else {
        for (int c = 1; c <= 198; ++c) {
          time_index[C_PHASE][b][c] = day++;
        }
      }
    }

    // Special Phase C on rows 196..198, cols 1..198.
    day += 3;
    day += 3 * (198 - 1);
    assert(day == 30298);

    // Phase D on regular blocks.
    for (int b = kRegularBlocks - 1; b >= 0; --b) {
      if ((b & 1) == 0) {
        for (int c = 198; c >= 1; --c) {
          time_index[D_PHASE][b][c] = day++;
        }
      } else {
        for (int c = 1; c <= 198; ++c) {
          time_index[D_PHASE][b][c] = day++;
        }
      }
    }
    assert(day == 40000);
  }

  long long ColumnScore(int block, int col, int perm_id) const {
    const int base_r = 4 * block;
    const auto& perm = permutations[perm_id];
    long long score = 0;
    for (int ph = 0; ph < 4; ++ph) {
      const int rr = base_r + perm[ph];
      score += time_index[ph][block][col] * static_cast<long long>(a[rr][col]);
    }
    return score;
  }

  void SolveRegularBlockDP(int block) {
    constexpr long long kNegInf = std::numeric_limits<long long>::min() / 4;
    constexpr int kCols = 196;  // 2..197

    const array<int, 4> left_fixed = FixedPermAtCol1(block);
    const array<int, 4> right_fixed = FixedPermAtCol198(block);

    array<long long, 24> dp_prev;
    array<long long, 24> dp_cur;
    for (int i = 0; i < 24; ++i) {
      dp_prev[i] = kNegInf;
      dp_cur[i] = kNegInf;
    }

    vector<array<int, 24>> parent(kCols);
    for (int idx = 0; idx < kCols; ++idx) {
      for (int s = 0; s < 24; ++s) {
        parent[idx][s] = -1;
      }
    }

    {
      const int col = 2;
      for (int cur = 0; cur < 24; ++cur) {
        if (!Compatible(left_fixed, permutations[cur])) {
          continue;
        }
        dp_prev[cur] = ColumnScore(block, col, cur);
      }
    }

    for (int idx = 1; idx < kCols; ++idx) {
      const int col = 2 + idx;
      for (int cur = 0; cur < 24; ++cur) {
        dp_cur[cur] = kNegInf;
      }
      for (int prev = 0; prev < 24; ++prev) {
        if (dp_prev[prev] == kNegInf) {
          continue;
        }
        for (int cur = 0; cur < 24; ++cur) {
          if (!ok_transition[prev][cur]) {
            continue;
          }
          const long long cand = dp_prev[prev] + ColumnScore(block, col, cur);
          if (cand > dp_cur[cur]) {
            dp_cur[cur] = cand;
            parent[idx][cur] = prev;
          }
        }
      }
      dp_prev = dp_cur;
    }

    long long best_score = kNegInf;
    int best_last = -1;
    for (int s = 0; s < 24; ++s) {
      if (dp_prev[s] == kNegInf) {
        continue;
      }
      if (!Compatible(permutations[s], right_fixed)) {
        continue;
      }
      if (dp_prev[s] > best_score) {
        best_score = dp_prev[s];
        best_last = s;
      }
    }

    if (best_last == -1) {
      std::cerr << "DP failed on block " << block << "\n";
      std::exit(1);
    }

    for (int ph = 0; ph < 4; ++ph) {
      row_of_phase[block][ph][1] = left_fixed[ph];
      row_of_phase[block][ph][198] = right_fixed[ph];
    }

    int cur = best_last;
    for (int idx = kCols - 1; idx >= 0; --idx) {
      const int col = 2 + idx;
      for (int ph = 0; ph < 4; ++ph) {
        row_of_phase[block][ph][col] = permutations[cur][ph];
      }
      if (idx > 0) {
        cur = parent[idx][cur];
        if (cur == -1) {
          std::cerr << "Parent recovery failed on block " << block << "\n";
          std::exit(1);
        }
      }
    }
  }

  void SolveAllBlocks() {
    BuildPermutations();
    PrecomputeTimes();
    for (int b = 0; b < kRegularBlocks; ++b) {
      SolveRegularBlockDP(b);
    }
  }

  void EmitPhaseARegularBlock(int block) {
    const int r = 4 * block;
    if ((block & 1) == 0) {
      AddCell(r + 0, 0);
      AddCell(r + 1, 0);
      for (int c = 1; c <= 198; ++c) {
        AddCell(r + row_of_phase[block][A_PHASE][c], c);
      }
      AddCell(r + 2, 199);
      AddCell(r + 3, 199);
    } else {
      AddCell(r + 0, 199);
      AddCell(r + 1, 199);
      for (int c = 198; c >= 1; --c) {
        AddCell(r + row_of_phase[block][A_PHASE][c], c);
      }
      AddCell(r + 2, 0);
      AddCell(r + 3, 0);
    }
  }

  void EmitPhaseASpecial() {
    AddCell(196, 199);
    AddCell(197, 199);
    AddCell(198, 199);
    AddCell(199, 199);
    for (int c = 198; c >= 0; --c) {
      AddCell(199, c);
    }
    AddCell(198, 0);
    AddCell(197, 0);
    AddCell(196, 0);
  }

  void EmitPhaseBRegularBlock(int block) {
    const int r = 4 * block;
    if ((block & 1) == 0) {
      AddCell(r + 3, 0);
      AddCell(r + 2, 0);
      for (int c = 1; c <= 198; ++c) {
        AddCell(r + row_of_phase[block][B_PHASE][c], c);
      }
      AddCell(r + 1, 199);
      AddCell(r + 0, 199);
    } else {
      AddCell(r + 3, 199);
      AddCell(r + 2, 199);
      for (int c = 198; c >= 1; --c) {
        AddCell(r + row_of_phase[block][B_PHASE][c], c);
      }
      AddCell(r + 1, 0);
      AddCell(r + 0, 0);
    }
  }

  void EmitPhaseCRegularBlock(int block) {
    const int r = 4 * block;
    if ((block & 1) == 0) {
      for (int c = 198; c >= 1; --c) {
        AddCell(r + row_of_phase[block][C_PHASE][c], c);
      }
    } else {
      for (int c = 1; c <= 198; ++c) {
        AddCell(r + row_of_phase[block][C_PHASE][c], c);
      }
    }
  }

  void EmitPhaseCSpecial() {
    AddCell(196, 1);
    AddCell(197, 1);
    AddCell(198, 1);
    for (int c = 2; c <= 198; ++c) {
      if ((c & 1) == 0) {
        AddCell(198, c);
        AddCell(197, c);
        AddCell(196, c);
      } else {
        AddCell(196, c);
        AddCell(197, c);
        AddCell(198, c);
      }
    }
  }

  void EmitPhaseDRegularBlock(int block) {
    const int r = 4 * block;
    if ((block & 1) == 0) {
      for (int c = 198; c >= 1; --c) {
        AddCell(r + row_of_phase[block][D_PHASE][c], c);
      }
    } else {
      for (int c = 1; c <= 198; ++c) {
        AddCell(r + row_of_phase[block][D_PHASE][c], c);
      }
    }
  }

  void BuildPath() {
    for (int b = 0; b < kRegularBlocks; ++b) {
      EmitPhaseARegularBlock(b);
    }
    EmitPhaseASpecial();

    for (int b = kRegularBlocks - 1; b >= 0; --b) {
      EmitPhaseBRegularBlock(b);
    }

    for (int b = 0; b < kRegularBlocks; ++b) {
      EmitPhaseCRegularBlock(b);
    }
    EmitPhaseCSpecial();

    for (int b = kRegularBlocks - 1; b >= 0; --b) {
      EmitPhaseDRegularBlock(b);
    }

    if (static_cast<int>(path.size()) != n * n) {
      std::cerr << "Path size mismatch: " << path.size() << " vs " << (n * n) << "\n";
      std::exit(1);
    }

    for (int i = 0; i < n * n; ++i) {
      const auto [r, c] = path[i];
      pos_of[r][c] = i;
    }
  }

  long long SwapDelta(int p, int q) const {
    const auto [br, bc] = path[p];
    const auto [er, ec] = path[q];
    return 1LL * (p - q) * (a[er][ec] - a[br][bc]);
  }

  long long RotateDeltaLeft(int p, int q, int r) const {
    const auto [br, bc] = path[p];
    const auto [er, ec] = path[q];
    const auto [hr, hc] = path[r];
    const long long old_score =
        1LL * p * a[br][bc] +
        1LL * q * a[er][ec] +
        1LL * r * a[hr][hc];
    const long long new_score =
        1LL * p * a[er][ec] +
        1LL * q * a[hr][hc] +
        1LL * r * a[br][bc];
    return new_score - old_score;
  }

  long long RotateDeltaRight(int p, int q, int r) const {
    const auto [br, bc] = path[p];
    const auto [er, ec] = path[q];
    const auto [hr, hc] = path[r];
    const long long old_score =
        1LL * p * a[br][bc] +
        1LL * q * a[er][ec] +
        1LL * r * a[hr][hc];
    const long long new_score =
        1LL * p * a[hr][hc] +
        1LL * q * a[br][bc] +
        1LL * r * a[er][ec];
    return new_score - old_score;
  }

  bool TrySwapPositions(int p, int q) {
    if (p > q) {
      std::swap(p, q);
    }
    if (p <= 0 || q <= 0 || p + 1 >= static_cast<int>(path.size()) ||
        q + 1 >= static_cast<int>(path.size())) {
      return false;
    }
    if (q - p <= 2) {
      return false;
    }

    const auto A = path[p - 1];
    const auto B = path[p];
    const auto C = path[p + 1];
    const auto D = path[q - 1];
    const auto E = path[q];
    const auto F = path[q + 1];

    if (std::abs(B.first - E.first) != 1 || B.second != E.second) {
      return false;
    }

    if (!AdjCell(A, E) || !AdjCell(E, C)) {
      return false;
    }
    if (!AdjCell(D, B) || !AdjCell(B, F)) {
      return false;
    }

    const long long delta = SwapDelta(p, q);
    if (delta <= 0) {
      return false;
    }

    std::swap(path[p], path[q]);
    pos_of[B.first][B.second] = q;
    pos_of[E.first][E.second] = p;
    return true;
  }

  bool TryRotateTriplePositions(int p, int q, int r) {
    std::array<int, 3> ord = {p, q, r};
    std::sort(ord.begin(), ord.end());
    p = ord[0];
    q = ord[1];
    r = ord[2];

    if (p <= 0 || q <= 0 || r <= 0) {
      return false;
    }
    if (p + 1 >= static_cast<int>(path.size()) ||
        q + 1 >= static_cast<int>(path.size()) ||
        r + 1 >= static_cast<int>(path.size())) {
      return false;
    }
    if (q - p <= 2 || r - q <= 2) {
      return false;
    }

    const auto A = path[p - 1];
    const auto B = path[p];
    const auto C = path[p + 1];

    const auto D = path[q - 1];
    const auto E = path[q];
    const auto F = path[q + 1];

    const auto G = path[r - 1];
    const auto H = path[r];
    const auto I = path[r + 1];

    if (!(B.second == E.second && E.second == H.second)) {
      return false;
    }
    std::array<int, 3> rows = {B.first, E.first, H.first};
    std::sort(rows.begin(), rows.end());
    if (!(rows[0] + 1 == rows[1] && rows[1] + 1 == rows[2])) {
      return false;
    }

    // (B, E, H) -> (E, H, B)
    if (AdjCell(A, E) && AdjCell(E, C) &&
        AdjCell(D, H) && AdjCell(H, F) &&
        AdjCell(G, B) && AdjCell(B, I)) {
      const long long delta = RotateDeltaLeft(p, q, r);
      if (delta > 0) {
        path[p] = E;
        path[q] = H;
        path[r] = B;
        pos_of[B.first][B.second] = r;
        pos_of[E.first][E.second] = p;
        pos_of[H.first][H.second] = q;
        return true;
      }
    }

    // (B, E, H) -> (H, B, E)
    if (AdjCell(A, H) && AdjCell(H, C) &&
        AdjCell(D, B) && AdjCell(B, F) &&
        AdjCell(G, E) && AdjCell(E, I)) {
      const long long delta = RotateDeltaRight(p, q, r);
      if (delta > 0) {
        path[p] = H;
        path[q] = B;
        path[r] = E;
        pos_of[B.first][B.second] = q;
        pos_of[E.first][E.second] = r;
        pos_of[H.first][H.second] = p;
        return true;
      }
    }

    return false;
  }

  void LocalImproveUntilProgramTime(double absolute_time_limit_sec) {
    vector<pair<int, int>> vertical_pairs;
    vertical_pairs.reserve((n - 1) * n);
    for (int r = 0; r + 1 < n; ++r) {
      for (int c = 0; c < n; ++c) {
        vertical_pairs.emplace_back(r, c);
      }
    }

    vector<pair<int, int>> vertical_triples;
    vertical_triples.reserve((n - 2) * n);
    for (int r = 0; r + 2 < n; ++r) {
      for (int c = 0; c < n; ++c) {
        vertical_triples.emplace_back(r, c);
      }
    }

    int iter = 0;
    int accepted_swap = 0;
    int accepted_rotate = 0;

    while (ElapsedSecFromProgramStart() < absolute_time_limit_sec) {
      ++iter;
      const uint32_t mode = rng() % 10;

      if (mode < 7) {
        const int idx = static_cast<int>(rng() % vertical_pairs.size());
        const int r = vertical_pairs[idx].first;
        const int c = vertical_pairs[idx].second;

        const int p1 = pos_of[r][c];
        const int p2 = pos_of[r + 1][c];
        if (p1 == -1 || p2 == -1) {
          continue;
        }
        if (TrySwapPositions(p1, p2)) {
          ++accepted_swap;
        }
      } else {
        const int idx = static_cast<int>(rng() % vertical_triples.size());
        const int r = vertical_triples[idx].first;
        const int c = vertical_triples[idx].second;

        const int p1 = pos_of[r][c];
        const int p2 = pos_of[r + 1][c];
        const int p3 = pos_of[r + 2][c];
        if (p1 == -1 || p2 == -1 || p3 == -1) {
          continue;
        }
        if (TryRotateTriplePositions(p1, p2, p3)) {
          ++accepted_rotate;
        }
      }
    }

    std::cerr << "elapsed=" << ElapsedSecFromProgramStart()
              << " iterations=" << iter
              << " accepted_swap=" << accepted_swap
              << " accepted_rotate=" << accepted_rotate << "\n";
  }

  void Output() const {
    for (const auto& [r, c] : path) {
      cout << r << ' ' << c << '\n';
    }
  }

  void Solve() {
    SolveAllBlocks();
    BuildPath();
    LocalImproveUntilProgramTime(2.70);
    Output();
  }
};

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  cin.tie(nullptr);

  const auto program_start_time = std::chrono::steady_clock::now();

  int n;
  cin >> n;

  Solver solver(n, program_start_time);
  solver.Read();
  solver.Solve();
  return 0;
}