#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <utility>
#include <vector>

struct Cell {
    int row;
    int col;
};

static bool IsKingAdjacent(const Cell &lhs, const Cell &rhs) {
    return std::max(std::abs(lhs.row - rhs.row), std::abs(lhs.col - rhs.col)) == 1;
}

static std::int64_t CurrentTimeMs() {
    using Clock = std::chrono::steady_clock;
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               Clock::now().time_since_epoch())
        .count();
}

class BoardTransform {
  public:
    explicit BoardTransform(int id = 0) : id_(id) {}

    Cell Apply(const Cell &cell, int n) const {
        switch (id_) {
        case 0:
            return cell;
        case 1:
            return Cell{cell.col, n - 1 - cell.row};
        case 2:
            return Cell{n - 1 - cell.row, n - 1 - cell.col};
        case 3:
            return Cell{n - 1 - cell.col, cell.row};
        case 4:
            return Cell{cell.row, n - 1 - cell.col};
        case 5:
            return Cell{cell.col, cell.row};
        case 6:
            return Cell{n - 1 - cell.row, cell.col};
        case 7:
            return Cell{n - 1 - cell.col, n - 1 - cell.row};
        default:
            assert(false);
            return cell;
        }
    }

    Cell Inverse(const Cell &cell, int n) const {
        switch (id_) {
        case 0:
            return cell;
        case 1:
            return BoardTransform(3).Apply(cell, n);
        case 2:
            return cell;
        case 3:
            return BoardTransform(1).Apply(cell, n);
        case 4:
        case 5:
        case 6:
        case 7:
            return Apply(cell, n);
        default:
            assert(false);
            return cell;
        }
    }

    const char *Name() const {
        static constexpr std::array<const char *, 8> kNames{
            "identity", "rot90", "rot180", "rot270",
            "flip_v",   "flip_d", "flip_h", "flip_a"};
        return kNames[id_];
    }

    int id() const { return id_; }

    static const std::array<BoardTransform, 8> &All() {
        static const std::array<BoardTransform, 8> kAll{
            BoardTransform(0), BoardTransform(1), BoardTransform(2), BoardTransform(3),
            BoardTransform(4), BoardTransform(5), BoardTransform(6), BoardTransform(7),
        };
        return kAll;
    }

  private:
    int id_;
};

struct Board {
    int n = 0;
    std::vector<std::vector<int>> a;

    static Board Read(std::istream &in) {
        Board board;
        in >> board.n;
        board.a.assign(board.n, std::vector<int>(board.n));
        for (int i = 0; i < board.n; ++i) {
            for (int j = 0; j < board.n; ++j) {
                in >> board.a[i][j];
            }
        }
        return board;
    }

    Board Transformed(const BoardTransform &transform) const {
        Board out;
        out.n = n;
        out.a.assign(n, std::vector<int>(n, 0));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                const Cell dst = transform.Apply(Cell{i, j}, n);
                out.a[dst.row][dst.col] = a[i][j];
            }
        }
        return out;
    }

    std::int64_t Evaluate(const std::vector<Cell> &route) const {
        assert(static_cast<int>(route.size()) == n * n);
        std::vector<int> seen(n * n, -1);
        std::int64_t score = 0;
        for (int t = 0; t < static_cast<int>(route.size()); ++t) {
            const Cell &cell = route[t];
            assert(0 <= cell.row && cell.row < n);
            assert(0 <= cell.col && cell.col < n);
            const int id = cell.row * n + cell.col;
            assert(seen[id] == -1);
            seen[id] = t;
            if (t > 0) {
                assert(IsKingAdjacent(route[t - 1], cell));
            }
            score += static_cast<std::int64_t>(t) * a[cell.row][cell.col];
        }
        return score;
    }
};

struct CandidateSolution {
    std::int64_t score = std::numeric_limits<std::int64_t>::lowest();
    std::vector<Cell> route;
    BoardTransform transform;
    bool reversed = false;
    int width = -1;
    const char *layout_name = "";
};

struct SearchResult {
    CandidateSolution baseline;
    CandidateSolution best;
};

class TwoOptImprover {
  public:
    explicit TwoOptImprover(const Board &board) : board_(board), n_(board.n) {}

    CandidateSolution Improve(const CandidateSolution &initial, int max_iterations) const {
        CandidateSolution improved = initial;
        std::vector<Cell> &route = improved.route;
        const int m = static_cast<int>(route.size());
        std::vector<int> pos(n_ * n_, -1);
        for (int i = 0; i < m; ++i) {
            pos[CellId(route[i])] = i;
        }

        int accepted = 0;
        for (int iter = 0; iter < max_iterations; ++iter) {
            std::vector<int> weights(m);
            std::vector<std::int64_t> prefix_weight(m + 1, 0);
            std::vector<std::int64_t> prefix_index_weight(m + 1, 0);
            for (int i = 0; i < m; ++i) {
                weights[i] = board_.a[route[i].row][route[i].col];
                prefix_weight[i + 1] = prefix_weight[i] + weights[i];
                prefix_index_weight[i + 1] =
                    prefix_index_weight[i] + static_cast<std::int64_t>(i) * weights[i];
            }

            std::int64_t best_delta = 0;
            int best_l = -1;
            int best_r = -1;

            for (int l = 1; l < m; ++l) {
                ForEachNeighbor(route[l - 1], [&](const Cell &neighbor) {
                    const int r = pos[CellId(neighbor)];
                    if (r < l) {
                        return;
                    }
                    if (r + 1 < m && !IsKingAdjacent(route[l], route[r + 1])) {
                        return;
                    }
                    const std::int64_t delta =
                        ReverseDelta(l, r, prefix_weight, prefix_index_weight);
                    if (delta > best_delta) {
                        best_delta = delta;
                        best_l = l;
                        best_r = r;
                    }
                });
            }

            ForEachNeighbor(route[0], [&](const Cell &neighbor) {
                const int r = pos[CellId(neighbor)] - 1;
                if (r < 0) {
                    return;
                }
                const std::int64_t delta = ReverseDelta(0, r, prefix_weight, prefix_index_weight);
                if (delta > best_delta) {
                    best_delta = delta;
                    best_l = 0;
                    best_r = r;
                }
            });

            if (best_delta <= 0) {
                break;
            }

            std::reverse(route.begin() + best_l, route.begin() + best_r + 1);
            for (int i = best_l; i <= best_r; ++i) {
                pos[CellId(route[i])] = i;
            }
            improved.score += best_delta;
            ++accepted;
        }
        std::cerr << "two_opt accepted=" << accepted
                  << " final_score=" << improved.score << '\n';
        return improved;
    }

  private:
    const Board &board_;
    int n_;

    int CellId(const Cell &cell) const { return cell.row * n_ + cell.col; }

    template <class Func>
    void ForEachNeighbor(const Cell &cell, Func &&func) const {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) {
                    continue;
                }
                const int nr = cell.row + dr;
                const int nc = cell.col + dc;
                if (0 <= nr && nr < n_ && 0 <= nc && nc < n_) {
                    func(Cell{nr, nc});
                }
            }
        }
    }

    static std::int64_t ReverseDelta(int l, int r, const std::vector<std::int64_t> &prefix_weight,
                                     const std::vector<std::int64_t> &prefix_index_weight) {
        if (l >= r) {
            return 0;
        }
        const std::int64_t sum_weight = prefix_weight[r + 1] - prefix_weight[l];
        const std::int64_t sum_index_weight = prefix_index_weight[r + 1] - prefix_index_weight[l];
        return static_cast<std::int64_t>(l + r) * sum_weight - 2 * sum_index_weight;
    }
};

class OrOptOneImprover {
  public:
    explicit OrOptOneImprover(const Board &board) : board_(board), n_(board.n) {}

    CandidateSolution Improve(const CandidateSolution &initial, int max_iterations) const {
        CandidateSolution improved = initial;
        std::vector<Cell> &route = improved.route;
        const int m = static_cast<int>(route.size());

        int accepted = 0;
        for (int iter = 0; iter < max_iterations; ++iter) {
            std::vector<int> pos(n_ * n_, -1);
            std::vector<int> weights(m);
            std::vector<std::int64_t> prefix_weight(m + 1, 0);
            for (int i = 0; i < m; ++i) {
                pos[CellId(route[i])] = i;
                weights[i] = board_.a[route[i].row][route[i].col];
                prefix_weight[i + 1] = prefix_weight[i] + weights[i];
            }

            std::int64_t best_delta = 0;
            int best_p = -1;
            int best_q = -2;

            for (int p = 0; p < m; ++p) {
                if (!RemovalValid(route, p)) {
                    continue;
                }

                const Cell moved = route[p];
                bool can_front = false;
                bool can_back = false;
                std::array<int, 8> candidate_qs{};
                int candidate_count = 0;

                ForEachNeighbor(moved, [&](const Cell &neighbor) {
                    const int idx = pos[CellId(neighbor)];
                    if (idx == -1) {
                        return;
                    }
                    if (idx == 0) {
                        can_front = true;
                    }
                    if (idx == m - 1) {
                        can_back = true;
                    }
                    if (idx + 1 >= m || idx == p || idx + 1 == p) {
                        return;
                    }
                    if (!IsKingAdjacent(moved, route[idx + 1])) {
                        return;
                    }
                    for (int i = 0; i < candidate_count; ++i) {
                        if (candidate_qs[i] == idx) {
                            return;
                        }
                    }
                    candidate_qs[candidate_count++] = idx;
                });

                if (can_front && p != 0) {
                    const std::int64_t delta = DeltaMoveToFront(p, weights[p], prefix_weight);
                    if (delta > best_delta) {
                        best_delta = delta;
                        best_p = p;
                        best_q = -1;
                    }
                }
                if (can_back && p != m - 1) {
                    const std::int64_t delta = DeltaMoveToBack(p, weights[p], prefix_weight, m);
                    if (delta > best_delta) {
                        best_delta = delta;
                        best_p = p;
                        best_q = m - 1;
                    }
                }

                for (int i = 0; i < candidate_count; ++i) {
                    const int q = candidate_qs[i];
                    const std::int64_t delta = DeltaMoveAfter(p, q, weights[p], prefix_weight);
                    if (delta > best_delta) {
                        best_delta = delta;
                        best_p = p;
                        best_q = q;
                    }
                }
            }

            if (best_delta <= 0) {
                break;
            }

            ApplyMove(route, best_p, best_q);
            improved.score += best_delta;
            ++accepted;
        }
        std::cerr << "or_opt1 accepted=" << accepted
                  << " final_score=" << improved.score << '\n';
        return improved;
    }

  private:
    const Board &board_;
    int n_;

    int CellId(const Cell &cell) const { return cell.row * n_ + cell.col; }

    static bool RemovalValid(const std::vector<Cell> &route, int p) {
        return p == 0 || p + 1 == static_cast<int>(route.size()) ||
               IsKingAdjacent(route[p - 1], route[p + 1]);
    }

    template <class Func>
    void ForEachNeighbor(const Cell &cell, Func &&func) const {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) {
                    continue;
                }
                const int nr = cell.row + dr;
                const int nc = cell.col + dc;
                if (0 <= nr && nr < n_ && 0 <= nc && nc < n_) {
                    func(Cell{nr, nc});
                }
            }
        }
    }

    static std::int64_t RangeWeight(const std::vector<std::int64_t> &prefix_weight, int l, int r) {
        if (l > r) {
            return 0;
        }
        return prefix_weight[r + 1] - prefix_weight[l];
    }

    static std::int64_t DeltaMoveToFront(int p, int weight,
                                         const std::vector<std::int64_t> &prefix_weight) {
        return -static_cast<std::int64_t>(p) * weight + RangeWeight(prefix_weight, 0, p - 1);
    }

    static std::int64_t DeltaMoveToBack(int p, int weight,
                                        const std::vector<std::int64_t> &prefix_weight, int m) {
        return static_cast<std::int64_t>(m - 1 - p) * weight -
               RangeWeight(prefix_weight, p + 1, m - 1);
    }

    static std::int64_t DeltaMoveAfter(int p, int q, int weight,
                                       const std::vector<std::int64_t> &prefix_weight) {
        if (q < p) {
            return static_cast<std::int64_t>(q + 1 - p) * weight +
                   RangeWeight(prefix_weight, q + 1, p - 1);
        }
        if (q > p) {
            return static_cast<std::int64_t>(q - p) * weight -
                   RangeWeight(prefix_weight, p + 1, q);
        }
        return 0;
    }

    static void ApplyMove(std::vector<Cell> &route, int p, int q) {
        if (q == -1) {
            std::rotate(route.begin(), route.begin() + p, route.begin() + p + 1);
            return;
        }
        if (q == static_cast<int>(route.size()) - 1) {
            std::rotate(route.begin() + p, route.begin() + p + 1, route.end());
            return;
        }
        if (q < p) {
            std::rotate(route.begin() + q + 1, route.begin() + p, route.begin() + p + 1);
            return;
        }
        if (q > p) {
            std::rotate(route.begin() + p, route.begin() + p + 1, route.begin() + q + 1);
        }
    }
};

class OrOptTwoImprover {
  public:
    explicit OrOptTwoImprover(const Board &board) : board_(board), n_(board.n) {}

    CandidateSolution Improve(const CandidateSolution &initial, int max_iterations) const {
        CandidateSolution improved = initial;
        std::vector<Cell> &route = improved.route;
        const int m = static_cast<int>(route.size());

        int accepted = 0;
        for (int iter = 0; iter < max_iterations; ++iter) {
            std::vector<int> pos(n_ * n_, -1);
            std::vector<int> weights(m);
            std::vector<std::int64_t> prefix_weight(m + 1, 0);
            for (int i = 0; i < m; ++i) {
                pos[CellId(route[i])] = i;
                weights[i] = board_.a[route[i].row][route[i].col];
                prefix_weight[i + 1] = prefix_weight[i] + weights[i];
            }

            std::int64_t best_delta = 0;
            int best_p = -1;
            int best_q = -2;

            for (int p = 0; p + 1 < m; ++p) {
                if (!RemovalValid(route, p)) {
                    continue;
                }

                const Cell first = route[p];
                const Cell last = route[p + 1];
                std::array<int, 8> candidate_qs{};
                int candidate_count = 0;

                if (p > 0 && IsKingAdjacent(last, route[0])) {
                    const std::int64_t delta =
                        DeltaMoveToFront(p, weights[p], weights[p + 1], prefix_weight);
                    if (delta > best_delta) {
                        best_delta = delta;
                        best_p = p;
                        best_q = -1;
                    }
                }
                if (p + 2 < m && IsKingAdjacent(route[m - 1], first)) {
                    const std::int64_t delta =
                        DeltaMoveToBack(p, weights[p], weights[p + 1], prefix_weight, m);
                    if (delta > best_delta) {
                        best_delta = delta;
                        best_p = p;
                        best_q = m - 1;
                    }
                }

                ForEachNeighbor(first, [&](const Cell &neighbor) {
                    const int q = pos[CellId(neighbor)];
                    if (q == -1 || q + 1 >= m) {
                        return;
                    }
                    if (q >= p && q <= p + 1) {
                        return;
                    }
                    if (q + 1 >= p && q + 1 <= p + 1) {
                        return;
                    }
                    if (!IsKingAdjacent(last, route[q + 1])) {
                        return;
                    }
                    for (int i = 0; i < candidate_count; ++i) {
                        if (candidate_qs[i] == q) {
                            return;
                        }
                    }
                    candidate_qs[candidate_count++] = q;
                });

                for (int i = 0; i < candidate_count; ++i) {
                    const int q = candidate_qs[i];
                    const std::int64_t delta =
                        DeltaMoveAfter(p, q, weights[p], weights[p + 1], prefix_weight);
                    if (delta > best_delta) {
                        best_delta = delta;
                        best_p = p;
                        best_q = q;
                    }
                }
            }

            if (best_delta <= 0) {
                break;
            }

            ApplyMove(route, best_p, best_q);
            improved.score += best_delta;
            ++accepted;
        }
        std::cerr << "or_opt2 accepted=" << accepted
                  << " final_score=" << improved.score << '\n';
        return improved;
    }

  private:
    const Board &board_;
    int n_;

    int CellId(const Cell &cell) const { return cell.row * n_ + cell.col; }

    static bool RemovalValid(const std::vector<Cell> &route, int p) {
        return p == 0 || p + 2 == static_cast<int>(route.size()) ||
               IsKingAdjacent(route[p - 1], route[p + 2]);
    }

    template <class Func>
    void ForEachNeighbor(const Cell &cell, Func &&func) const {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) {
                    continue;
                }
                const int nr = cell.row + dr;
                const int nc = cell.col + dc;
                if (0 <= nr && nr < n_ && 0 <= nc && nc < n_) {
                    func(Cell{nr, nc});
                }
            }
        }
    }

    static std::int64_t RangeWeight(const std::vector<std::int64_t> &prefix_weight, int l, int r) {
        if (l > r) {
            return 0;
        }
        return prefix_weight[r + 1] - prefix_weight[l];
    }

    static std::int64_t DeltaMoveToFront(int p, int first_weight, int second_weight,
                                         const std::vector<std::int64_t> &prefix_weight) {
        return -static_cast<std::int64_t>(p) * (first_weight + second_weight) +
               2 * RangeWeight(prefix_weight, 0, p - 1);
    }

    static std::int64_t DeltaMoveToBack(int p, int first_weight, int second_weight,
                                        const std::vector<std::int64_t> &prefix_weight, int m) {
        return static_cast<std::int64_t>(m - p - 2) * (first_weight + second_weight) -
               2 * RangeWeight(prefix_weight, p + 2, m - 1);
    }

    static std::int64_t DeltaMoveAfter(int p, int q, int first_weight, int second_weight,
                                       const std::vector<std::int64_t> &prefix_weight) {
        if (q < p) {
            return static_cast<std::int64_t>(q + 1 - p) * (first_weight + second_weight) +
                   2 * RangeWeight(prefix_weight, q + 1, p - 1);
        }
        if (q > p + 1) {
            return static_cast<std::int64_t>(q - p - 1) * (first_weight + second_weight) -
                   2 * RangeWeight(prefix_weight, p + 2, q);
        }
        return 0;
    }

    static void ApplyMove(std::vector<Cell> &route, int p, int q) {
        if (q == -1) {
            std::rotate(route.begin(), route.begin() + p, route.begin() + p + 2);
            return;
        }
        if (q == static_cast<int>(route.size()) - 1) {
            std::rotate(route.begin() + p, route.begin() + p + 2, route.end());
            return;
        }
        if (q < p) {
            std::rotate(route.begin() + q + 1, route.begin() + p, route.begin() + p + 2);
            return;
        }
        if (q > p + 1) {
            std::rotate(route.begin() + p, route.begin() + p + 2, route.begin() + q + 1);
        }
    }
};

class OrOptThreeImprover {
  public:
    explicit OrOptThreeImprover(const Board &board) : board_(board), n_(board.n) {}

    CandidateSolution Improve(const CandidateSolution &initial, int max_iterations) const {
        CandidateSolution improved = initial;
        std::vector<Cell> &route = improved.route;
        const int m = static_cast<int>(route.size());

        int accepted = 0;
        for (int iter = 0; iter < max_iterations; ++iter) {
            std::vector<int> pos(n_ * n_, -1);
            std::vector<int> weights(m);
            std::vector<std::int64_t> prefix_weight(m + 1, 0);
            for (int i = 0; i < m; ++i) {
                pos[CellId(route[i])] = i;
                weights[i] = board_.a[route[i].row][route[i].col];
                prefix_weight[i + 1] = prefix_weight[i] + weights[i];
            }

            std::int64_t best_delta = 0;
            int best_p = -1;
            int best_q = -2;

            for (int p = 0; p + 2 < m; ++p) {
                if (!RemovalValid(route, p)) {
                    continue;
                }

                const Cell first = route[p];
                const Cell last = route[p + 2];
                std::array<int, 8> candidate_qs{};
                int candidate_count = 0;

                if (p > 0 && IsKingAdjacent(last, route[0])) {
                    const std::int64_t delta =
                        DeltaMoveToFront(p, weights[p], weights[p + 1], weights[p + 2],
                                         prefix_weight);
                    if (delta > best_delta) {
                        best_delta = delta;
                        best_p = p;
                        best_q = -1;
                    }
                }
                if (p + 3 < m && IsKingAdjacent(route[m - 1], first)) {
                    const std::int64_t delta =
                        DeltaMoveToBack(p, weights[p], weights[p + 1], weights[p + 2],
                                        prefix_weight, m);
                    if (delta > best_delta) {
                        best_delta = delta;
                        best_p = p;
                        best_q = m - 1;
                    }
                }

                ForEachNeighbor(first, [&](const Cell &neighbor) {
                    const int q = pos[CellId(neighbor)];
                    if (q == -1 || q + 1 >= m) {
                        return;
                    }
                    if (q >= p && q <= p + 2) {
                        return;
                    }
                    if (q + 1 >= p && q + 1 <= p + 2) {
                        return;
                    }
                    if (!IsKingAdjacent(last, route[q + 1])) {
                        return;
                    }
                    for (int i = 0; i < candidate_count; ++i) {
                        if (candidate_qs[i] == q) {
                            return;
                        }
                    }
                    candidate_qs[candidate_count++] = q;
                });

                for (int i = 0; i < candidate_count; ++i) {
                    const int q = candidate_qs[i];
                    const std::int64_t delta =
                        DeltaMoveAfter(p, q, weights[p], weights[p + 1], weights[p + 2],
                                       prefix_weight);
                    if (delta > best_delta) {
                        best_delta = delta;
                        best_p = p;
                        best_q = q;
                    }
                }
            }

            if (best_delta <= 0) {
                break;
            }

            ApplyMove(route, best_p, best_q);
            improved.score += best_delta;
            ++accepted;
        }
        std::cerr << "or_opt3 accepted=" << accepted
                  << " final_score=" << improved.score << '\n';
        return improved;
    }

  private:
    const Board &board_;
    int n_;

    int CellId(const Cell &cell) const { return cell.row * n_ + cell.col; }

    static bool RemovalValid(const std::vector<Cell> &route, int p) {
        return p == 0 || p + 3 == static_cast<int>(route.size()) ||
               IsKingAdjacent(route[p - 1], route[p + 3]);
    }

    template <class Func>
    void ForEachNeighbor(const Cell &cell, Func &&func) const {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) {
                    continue;
                }
                const int nr = cell.row + dr;
                const int nc = cell.col + dc;
                if (0 <= nr && nr < n_ && 0 <= nc && nc < n_) {
                    func(Cell{nr, nc});
                }
            }
        }
    }

    static std::int64_t RangeWeight(const std::vector<std::int64_t> &prefix_weight, int l, int r) {
        if (l > r) {
            return 0;
        }
        return prefix_weight[r + 1] - prefix_weight[l];
    }

    static std::int64_t DeltaMoveToFront(int p, int w0, int w1, int w2,
                                         const std::vector<std::int64_t> &prefix_weight) {
        const std::int64_t sum_weight = static_cast<std::int64_t>(w0) + w1 + w2;
        return -static_cast<std::int64_t>(p) * sum_weight + 3 * RangeWeight(prefix_weight, 0, p - 1);
    }

    static std::int64_t DeltaMoveToBack(int p, int w0, int w1, int w2,
                                        const std::vector<std::int64_t> &prefix_weight, int m) {
        const std::int64_t sum_weight = static_cast<std::int64_t>(w0) + w1 + w2;
        return static_cast<std::int64_t>(m - p - 3) * sum_weight -
               3 * RangeWeight(prefix_weight, p + 3, m - 1);
    }

    static std::int64_t DeltaMoveAfter(int p, int q, int w0, int w1, int w2,
                                       const std::vector<std::int64_t> &prefix_weight) {
        const std::int64_t sum_weight = static_cast<std::int64_t>(w0) + w1 + w2;
        if (q < p) {
            return static_cast<std::int64_t>(q + 1 - p) * sum_weight +
                   3 * RangeWeight(prefix_weight, q + 1, p - 1);
        }
        if (q > p + 2) {
            return static_cast<std::int64_t>(q - p - 2) * sum_weight -
                   3 * RangeWeight(prefix_weight, p + 3, q);
        }
        return 0;
    }

    static void ApplyMove(std::vector<Cell> &route, int p, int q) {
        if (q == -1) {
            std::rotate(route.begin(), route.begin() + p, route.begin() + p + 3);
            return;
        }
        if (q == static_cast<int>(route.size()) - 1) {
            std::rotate(route.begin() + p, route.begin() + p + 3, route.end());
            return;
        }
        if (q < p) {
            std::rotate(route.begin() + q + 1, route.begin() + p, route.begin() + p + 3);
            return;
        }
        if (q > p + 2) {
            std::rotate(route.begin() + p, route.begin() + p + 3, route.begin() + q + 1);
        }
    }
};

class BlockSwapImprover {
  public:
    explicit BlockSwapImprover(const Board &board) : board_(board), n_(board.n) {}

    CandidateSolution Improve(const CandidateSolution &initial, int max_iterations) const {
        CandidateSolution improved = initial;
        std::vector<Cell> &route = improved.route;
        const int m = static_cast<int>(route.size());

        int accepted = 0;
        for (int iter = 0; iter < max_iterations; ++iter) {
            std::vector<int> pos(n_ * n_, -1);
            std::vector<int> weights(m);
            std::vector<std::int64_t> prefix_weight(m + 1, 0);
            for (int i = 0; i < m; ++i) {
                pos[CellId(route[i])] = i;
                weights[i] = board_.a[route[i].row][route[i].col];
                prefix_weight[i + 1] = prefix_weight[i] + weights[i];
            }

            std::int64_t best_delta = 0;
            int best_a = -1;
            int best_la = -1;
            int best_c = -1;
            int best_lb = -1;
            bool found = false;

            for (int a = 0; a < m; ++a) {
                if (UpdateFirst(route, pos, prefix_weight, m, a, 1, 1, best_delta, best_a, best_la,
                                best_c, best_lb) ||
                    UpdateFirst(route, pos, prefix_weight, m, a, 1, 2, best_delta, best_a, best_la,
                                best_c, best_lb) ||
                    UpdateFirst(route, pos, prefix_weight, m, a, 2, 1, best_delta, best_a, best_la,
                                best_c, best_lb)) {
                    found = true;
                    break;
                }
            }

            if (!found || best_delta <= 0) {
                break;
            }

            ApplySwap(route, best_a, best_la, best_c, best_lb);
            improved.score += best_delta;
            ++accepted;
        }
        std::cerr << "block_swap accepted=" << accepted
                  << " final_score=" << improved.score << '\n';
        return improved;
    }

  private:
    const Board &board_;
    int n_;

    int CellId(const Cell &cell) const { return cell.row * n_ + cell.col; }

    template <class Func>
    void ForEachNeighbor(const Cell &cell, Func &&func) const {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) {
                    continue;
                }
                const int nr = cell.row + dr;
                const int nc = cell.col + dc;
                if (0 <= nr && nr < n_ && 0 <= nc && nc < n_) {
                    func(Cell{nr, nc});
                }
            }
        }
    }

    static std::int64_t RangeWeight(const std::vector<std::int64_t> &prefix_weight, int l, int r) {
        if (l > r) {
            return 0;
        }
        return prefix_weight[r + 1] - prefix_weight[l];
    }

    static std::int64_t SwapDelta(const std::vector<std::int64_t> &prefix_weight, int a, int la,
                                  int c, int lb) {
        const int middle_len = c - (a + la);
        const std::int64_t sum_a = RangeWeight(prefix_weight, a, a + la - 1);
        const std::int64_t sum_middle = RangeWeight(prefix_weight, a + la, c - 1);
        const std::int64_t sum_b = RangeWeight(prefix_weight, c, c + lb - 1);
        return static_cast<std::int64_t>(middle_len + lb) * sum_a +
               static_cast<std::int64_t>(lb - la) * sum_middle -
               static_cast<std::int64_t>(middle_len + la) * sum_b;
    }

    static void ApplySwap(std::vector<Cell> &route, int a, int la, int c, int lb) {
        const int middle_len = c - (a + la);
        assert(la >= 1 && lb >= 1);
        assert(a >= 0 && a + la <= c && c + lb <= static_cast<int>(route.size()));
        assert(middle_len >= 1);

        std::vector<Cell> rebuilt;
        rebuilt.reserve(la + middle_len + lb);
        for (int i = 0; i < lb; ++i) {
            rebuilt.push_back(route[c + i]);
        }
        for (int i = 0; i < middle_len; ++i) {
            rebuilt.push_back(route[a + la + i]);
        }
        for (int i = 0; i < la; ++i) {
            rebuilt.push_back(route[a + i]);
        }
        for (int i = 0; i < static_cast<int>(rebuilt.size()); ++i) {
            route[a + i] = rebuilt[i];
        }
    }

    static bool IsValidSwap(const std::vector<Cell> &route, int a, int la, int c, int lb) {
        const Cell &a_first = route[a];
        const Cell &a_last = route[a + la - 1];
        const Cell &b_first = route[c];
        const Cell &b_last = route[c + lb - 1];
        const Cell &middle_first = route[a + la];
        const Cell &middle_last = route[c - 1];

        if (a > 0 && !IsKingAdjacent(route[a - 1], b_first)) {
            return false;
        }
        if (!IsKingAdjacent(b_last, middle_first)) {
            return false;
        }
        if (!IsKingAdjacent(middle_last, a_first)) {
            return false;
        }
        if (c + lb < static_cast<int>(route.size()) && !IsKingAdjacent(a_last, route[c + lb])) {
            return false;
        }
        return true;
    }

    bool UpdateFirst(const std::vector<Cell> &route, const std::vector<int> &pos,
                     const std::vector<std::int64_t> &prefix_weight, int m, int a, int la, int lb,
                     std::int64_t &best_delta, int &best_a, int &best_la, int &best_c,
                     int &best_lb) const {
        if (a + la >= m) {
            return false;
        }
        std::array<int, 8> candidates{};
        int candidate_count = 0;

        if (a > 0) {
            ForEachNeighbor(route[a - 1], [&](const Cell &neighbor) {
                const int c = pos[CellId(neighbor)];
                if (!CanUseCandidate(m, a, la, c, lb)) {
                    return;
                }
                for (int i = 0; i < candidate_count; ++i) {
                    if (candidates[i] == c) {
                        return;
                    }
                }
                candidates[candidate_count++] = c;
            });
        } else {
            ForEachNeighbor(route[a + la], [&](const Cell &neighbor) {
                const int end = pos[CellId(neighbor)];
                const int c = end - lb + 1;
                if (!CanUseCandidate(m, a, la, c, lb)) {
                    return;
                }
                for (int i = 0; i < candidate_count; ++i) {
                    if (candidates[i] == c) {
                        return;
                    }
                }
                candidates[candidate_count++] = c;
            });
        }

        for (int i = 0; i < candidate_count; ++i) {
            const int c = candidates[i];
            if (!IsValidSwap(route, a, la, c, lb)) {
                continue;
            }
            const std::int64_t delta = SwapDelta(prefix_weight, a, la, c, lb);
            if (delta > 0) {
                best_delta = delta;
                best_a = a;
                best_la = la;
                best_c = c;
                best_lb = lb;
                return true;
            }
        }
        return false;
    }

    static bool CanUseCandidate(int m, int a, int la, int c, int lb) {
        if (c < 0 || c + lb > m) {
            return false;
        }
        if (c <= a + la) {
            return false;
        }
        return true;
    }
};

class VariantSearch {
  public:
    template <class Generator>
    std::optional<SearchResult> Search(const Board &board, Generator &&generate) const {
        SearchResult result;
        bool has_best = false;
        bool has_baseline = false;

        for (const BoardTransform &transform : BoardTransform::All()) {
            const Board transformed = board.Transformed(transform);
            const std::optional<std::vector<Cell>> transformed_route = generate(transformed);
            if (!transformed_route.has_value()) {
                continue;
            }
            std::vector<Cell> route = RestoreRoute(*transformed_route, transform, board.n);

            Consider(board, route, transform, false, result, has_best, has_baseline);
            std::reverse(route.begin(), route.end());
            Consider(board, route, transform, true, result, has_best, has_baseline);
        }

        if (!has_best) {
            return std::nullopt;
        }
        return result;
    }

  private:
    static std::vector<Cell> RestoreRoute(const std::vector<Cell> &route,
                                          const BoardTransform &transform, int n) {
        std::vector<Cell> restored;
        restored.reserve(route.size());
        for (const Cell &cell : route) {
            restored.push_back(transform.Inverse(cell, n));
        }
        return restored;
    }

    static void Consider(const Board &board, const std::vector<Cell> &route,
                         const BoardTransform &transform, bool reversed, SearchResult &result,
                         bool &has_best, bool &has_baseline) {
        CandidateSolution candidate;
        candidate.score = board.Evaluate(route);
        candidate.route = route;
        candidate.transform = transform;
        candidate.reversed = reversed;

        if (transform.id() == 0 && !reversed) {
            result.baseline = candidate;
            has_baseline = true;
        }
        if (!has_best || candidate.score > result.best.score) {
            result.best = std::move(candidate);
            has_best = true;
        }
    }
};

enum class LayoutKind {
    Spiral,
    Zigzag,
};

static const char *LayoutName(LayoutKind kind) {
    switch (kind) {
    case LayoutKind::Spiral:
        return "spiral";
    case LayoutKind::Zigzag:
        return "zigzag";
    }
    assert(false);
    return "";
}

class StripedLayoutSolver {
  public:
    StripedLayoutSolver(LayoutKind kind, int width)
        : kind_(kind), width_(width), permutations_(BuildPermutations(width)) {}

    std::optional<std::vector<Cell>> Solve(const Board &board) const {
        Instance instance(board, kind_, width_, permutations_);
        return instance.Solve();
    }

  private:
    static std::vector<std::vector<int>> BuildPermutations(int width) {
        std::vector<std::vector<int>> perms;
        std::vector<int> perm(width);
        for (int i = 0; i < width; ++i) {
            perm[i] = i;
        }
        do {
            perms.push_back(perm);
        } while (std::next_permutation(perm.begin(), perm.end()));
        return perms;
    }

    class Instance {
      public:
        Instance(const Board &board, LayoutKind kind, int width,
                 const std::vector<std::vector<int>> &permutations)
            : board_(board),
              kind_(kind),
              n_(board.n),
              width_(width),
              layers_(board.n / 2),
              permutations_(permutations) {}

        std::optional<std::vector<Cell>> Solve() {
            assert((n_ & 1) == 0);
            assert(width_ >= 2);
            BuildLayout();
            BuildTransitionGraph();
            if (!Optimize()) {
                return std::nullopt;
            }
            return BuildAnswer();
        }

      private:
        struct CellWithOuter {
            Cell cell;
            Cell outer;
        };

        const Board &board_;
        LayoutKind kind_;
        int n_;
        int width_;
        int layers_;
        const std::vector<std::vector<int>> &permutations_;

        std::vector<std::vector<Cell>> paths_;
        std::vector<std::vector<int>> aligned_indices_;
        std::vector<std::vector<int>> index_to_slice_;
        std::vector<std::vector<Cell>> slice_cells_;
        std::vector<std::vector<std::int64_t>> stage_times_;
        std::vector<std::int64_t> stage_offsets_;
        std::vector<int> best_perm_ids_;
        std::vector<int> slice_pattern_id_;
        std::vector<std::vector<std::vector<int>>> transition_next_;
        std::vector<char> first_valid_perm_;
        std::vector<char> last_valid_perm_;

        void BuildLayout() {
            switch (kind_) {
            case LayoutKind::Spiral:
                BuildSpiralLayout();
                break;
            case LayoutKind::Zigzag:
                BuildZigzagLayout();
                break;
            }
            ValidatePaths();
            FinalizeAlignedIndices();
        }

        void ValidatePaths() const {
            std::set<std::pair<int, int>> used;
            for (int color = 0; color < width_; ++color) {
                for (int i = 0; i < static_cast<int>(paths_[color].size()); ++i) {
                    const Cell &cell = paths_[color][i];
                    assert(0 <= cell.row && cell.row < n_);
                    assert(0 <= cell.col && cell.col < n_);
                    assert(used.emplace(cell.row, cell.col).second);
                    if (i > 0) {
                        assert(IsKingAdjacent(paths_[color][i - 1], paths_[color][i]));
                    }
                }
            }
            assert(static_cast<int>(used.size()) == n_ * n_);
        }

        void BuildSpiralLayout() {
            std::vector<std::vector<Cell>> outward_cells(width_);
            paths_.assign(width_, {});
            for (int color = 0; color < width_; ++color) {
                for (int depth = color; depth < layers_; depth += width_) {
                    for (int col = std::max(0, depth - width_ + 1); col < depth; ++col) {
                        paths_[color].push_back(Cell{depth, col});
                        if (color > 0) {
                            outward_cells[color].push_back(Cell{depth - 1, col});
                        }
                    }
                    AppendSpiralRingSegment(color, depth, outward_cells[color]);
                }
            }

            for (int t = 0; t < width_; ++t) {
                const int length = width_ - 1 - 2 * t;
                if (length <= 0) {
                    break;
                }
                const int color = (layers_ + t) % width_;
                const int row = layers_ + t;
                const int left = layers_ - (width_ - 1) + t;
                const int right = layers_ - 1 - t;
                for (int col = left; col <= right; ++col) {
                    paths_[color].push_back(Cell{row, col});
                    if (color > 0) {
                        outward_cells[color].push_back(Cell{row - 1, col});
                    }
                }
            }

            const int anchor = width_ - 1;
            aligned_indices_.assign(width_, std::vector<int>(paths_[anchor].size(), -1));
            for (int i = 0; i < static_cast<int>(paths_[anchor].size()); ++i) {
                aligned_indices_[anchor][i] = i;
            }

            std::vector<std::vector<int>> match_to_prev(width_);
            for (int color = 1; color < width_; ++color) {
                std::vector<std::vector<int>> pos(n_, std::vector<int>(n_, -1));
                for (int i = 0; i < static_cast<int>(paths_[color - 1].size()); ++i) {
                    const Cell &cell = paths_[color - 1][i];
                    pos[cell.row][cell.col] = i;
                }
                match_to_prev[color].resize(paths_[color].size());
                int prev = -1;
                for (int i = 0; i < static_cast<int>(paths_[color].size()); ++i) {
                    const Cell &outer = outward_cells[color][i];
                    const int idx = pos[outer.row][outer.col];
                    assert(idx != -1);
                    assert(IsKingAdjacent(paths_[color][i], paths_[color - 1][idx]));
                    assert(prev < idx);
                    match_to_prev[color][i] = idx;
                    prev = idx;
                }
            }

            for (int color = anchor - 1; color >= 0; --color) {
                aligned_indices_[color].resize(aligned_indices_[anchor].size());
                for (int i = 0; i < static_cast<int>(aligned_indices_[anchor].size()); ++i) {
                    aligned_indices_[color][i] =
                        match_to_prev[color + 1][aligned_indices_[color + 1][i]];
                }
            }
        }

        void AppendSpiralRingSegment(int color, int depth, std::vector<Cell> &outward_cells) {
            const int last = n_ - 1 - depth;
            std::vector<CellWithOuter> border;
            border.reserve(4 * (last - depth + 1));

            for (int col = depth; col <= last; ++col) {
                border.push_back(CellWithOuter{Cell{depth, col}, Cell{depth - 1, col}});
            }
            for (int row = depth + 1; row <= last; ++row) {
                border.push_back(CellWithOuter{Cell{row, last}, Cell{row, last + 1}});
            }
            for (int col = last - 1; col >= depth; --col) {
                border.push_back(CellWithOuter{Cell{last, col}, Cell{last + 1, col}});
            }
            for (int row = last - 1; row > depth; --row) {
                border.push_back(CellWithOuter{Cell{row, depth}, Cell{row, depth - 1}});
            }

            const int omit = std::min(width_ - 1, last - depth);
            const int keep = static_cast<int>(border.size()) - omit;
            for (int i = 0; i < keep; ++i) {
                paths_[color].push_back(border[i].cell);
                if (color > 0) {
                    outward_cells.push_back(border[i].outer);
                }
            }
        }

        void BuildZigzagLayout() {
            paths_.assign(width_, {});
            for (int color = 0; color < width_; ++color) {
                const int left = width_ - 1 - color;
                const int right = n_ - 1 - color;
                int block = 0;
                bool first = true;
                while (true) {
                    const int top = 2 * block * width_ + color;
                    if (top >= n_) {
                        break;
                    }
                    const int start_col = first ? 0 : left + 1;
                    for (int col = start_col; col <= right; ++col) {
                        paths_[color].push_back(Cell{top, col});
                    }

                    const int bottom = (2 * block + 1) * width_ + (width_ - 1 - color);
                    if (bottom >= n_) {
                        break;
                    }
                    for (int row = top + 1; row <= bottom; ++row) {
                        paths_[color].push_back(Cell{row, right});
                    }
                    for (int col = right - 1; col >= left; --col) {
                        paths_[color].push_back(Cell{bottom, col});
                    }

                    const int next_top = (2 * block + 2) * width_ + color;
                    const int tail = std::min(next_top, n_ - 1);
                    for (int row = bottom + 1; row <= tail; ++row) {
                        paths_[color].push_back(Cell{row, left});
                    }
                    if (next_top >= n_) {
                        break;
                    }
                    ++block;
                    first = false;
                }
            }

            aligned_indices_.assign(width_, {});
            std::vector<std::vector<int>> pos(width_);
            for (int color = 0; color < width_; ++color) {
                pos[color].assign(n_ * n_, -1);
                for (int i = 0; i < static_cast<int>(paths_[color].size()); ++i) {
                    const Cell &cell = paths_[color][i];
                    pos[color][cell.row * n_ + cell.col] = i;
                }
            }

            const int full_blocks = n_ / (2 * width_);
            for (int block = 0; block < full_blocks; ++block) {
                const int top_left = (block == 0 ? 0 : width_);
                const int top_right = n_ - width_;
                for (int col = top_left; col <= top_right; ++col) {
                    for (int color = 0; color < width_; ++color) {
                        const Cell cell{2 * block * width_ + color, col};
                        aligned_indices_[color].push_back(pos[color][cell.row * n_ + cell.col]);
                    }
                }

                const int bottom_left = width_ - 1;
                const int bottom_right = n_ - width_ - 1;
                for (int col = bottom_right; col >= bottom_left; --col) {
                    for (int color = 0; color < width_; ++color) {
                        const Cell cell{(2 * block + 1) * width_ + (width_ - 1 - color), col};
                        aligned_indices_[color].push_back(pos[color][cell.row * n_ + cell.col]);
                    }
                }
            }

            // If `n` is divisible by `width` but not by `2*width`, one extra top half-block remains.
            if (n_ % (2 * width_) >= width_) {
                const int block = full_blocks;
                const int top_left = (block == 0 ? 0 : width_);
                const int top_right = n_ - width_;
                for (int col = top_left; col <= top_right; ++col) {
                    for (int color = 0; color < width_; ++color) {
                        const Cell cell{2 * block * width_ + color, col};
                        aligned_indices_[color].push_back(pos[color][cell.row * n_ + cell.col]);
                    }
                }
            }
        }

        void FinalizeAlignedIndices() {
            const int anchor = width_ - 1;
            const int slices = static_cast<int>(aligned_indices_[anchor].size());
            for (int color = 0; color < width_; ++color) {
                assert(static_cast<int>(aligned_indices_[color].size()) == slices);
            }

            index_to_slice_.assign(width_, {});
            slice_cells_.assign(width_, {});
            stage_times_.assign(width_, {});
            stage_offsets_.assign(width_, 0);

            std::int64_t offset = 0;
            for (int stage = 0; stage < width_; ++stage) {
                if (stage > 0) {
                    offset += paths_[stage - 1].size();
                }
                stage_offsets_[stage] = offset;
                index_to_slice_[stage].assign(paths_[stage].size(), -1);
                slice_cells_[stage].resize(slices);
                stage_times_[stage].resize(slices);
                for (int i = 0; i < slices; ++i) {
                    const int idx = aligned_indices_[stage][i];
                    assert(0 <= idx && idx < static_cast<int>(paths_[stage].size()));
                    if (i > 0) {
                        assert(aligned_indices_[stage][i - 1] < idx);
                    }
                    index_to_slice_[stage][idx] = i;
                    slice_cells_[stage][i] = paths_[stage][idx];
                    stage_times_[stage][i] =
                        IsForwardStage(stage)
                            ? stage_offsets_[stage] + idx
                            : stage_offsets_[stage] + static_cast<int>(paths_[stage].size()) - 1 - idx;
                }
            }
        }

        const Cell &ChosenCell(int slice, int stage, int perm_id) const {
            const int color = permutations_[perm_id][stage];
            return slice_cells_[color][slice];
        }

        bool IsForwardStage(int stage) const { return (stage & 1) == 0; }

        std::int64_t SliceGain(int slice, int perm_id) const {
            std::int64_t gain = 0;
            for (int stage = 0; stage < width_; ++stage) {
                const Cell &cell = ChosenCell(slice, stage, perm_id);
                gain += stage_times_[stage][slice] * board_.a[cell.row][cell.col];
            }
            return gain;
        }

        std::int64_t FixedGain() const {
            std::int64_t gain = 0;
            for (int stage = 0; stage < width_; ++stage) {
                for (int idx = 0; idx < static_cast<int>(paths_[stage].size()); ++idx) {
                    if (index_to_slice_[stage][idx] != -1) {
                        continue;
                    }
                    const Cell &cell = paths_[stage][idx];
                    const std::int64_t time =
                        IsForwardStage(stage)
                            ? stage_offsets_[stage] + idx
                            : stage_offsets_[stage] + static_cast<int>(paths_[stage].size()) - 1 - idx;
                    gain += time * board_.a[cell.row][cell.col];
                }
            }
            return gain;
        }

        bool StageStartAttachmentValid(int stage, int perm_id) const {
            if (aligned_indices_[stage].empty()) {
                return true;
            }
            if (IsForwardStage(stage)) {
                const int first_idx = aligned_indices_[stage][0];
                return first_idx == 0 ||
                       IsKingAdjacent(paths_[stage][first_idx - 1], ChosenCell(0, stage, perm_id));
            }
            const int last_slice = static_cast<int>(aligned_indices_[stage].size()) - 1;
            const int last_idx = aligned_indices_[stage][last_slice];
            return last_idx + 1 == static_cast<int>(paths_[stage].size()) ||
                   IsKingAdjacent(ChosenCell(last_slice, stage, perm_id), paths_[stage][last_idx + 1]);
        }

        bool StageEndAttachmentValid(int stage, int perm_id) const {
            if (aligned_indices_[stage].empty()) {
                return true;
            }
            if (IsForwardStage(stage)) {
                const int last_slice = static_cast<int>(aligned_indices_[stage].size()) - 1;
                const int last_idx = aligned_indices_[stage][last_slice];
                return last_idx + 1 == static_cast<int>(paths_[stage].size()) ||
                       IsKingAdjacent(ChosenCell(last_slice, stage, perm_id), paths_[stage][last_idx + 1]);
            }
            const int first_idx = aligned_indices_[stage][0];
            return first_idx == 0 ||
                   IsKingAdjacent(ChosenCell(0, stage, perm_id), paths_[stage][first_idx - 1]);
        }

        Cell StageStartNode(int stage, int perm_id) const {
            if (aligned_indices_[stage].empty()) {
                return IsForwardStage(stage) ? paths_[stage].front() : paths_[stage].back();
            }
            if (IsForwardStage(stage)) {
                return aligned_indices_[stage][0] == 0 ? ChosenCell(0, stage, perm_id) : paths_[stage].front();
            }
            const int last_slice = static_cast<int>(aligned_indices_[stage].size()) - 1;
            return aligned_indices_[stage][last_slice] + 1 == static_cast<int>(paths_[stage].size())
                       ? ChosenCell(last_slice, stage, perm_id)
                       : paths_[stage].back();
        }

        Cell StageEndNode(int stage, int perm_id) const {
            if (aligned_indices_[stage].empty()) {
                return IsForwardStage(stage) ? paths_[stage].back() : paths_[stage].front();
            }
            if (IsForwardStage(stage)) {
                const int last_slice = static_cast<int>(aligned_indices_[stage].size()) - 1;
                return aligned_indices_[stage][last_slice] + 1 == static_cast<int>(paths_[stage].size())
                           ? ChosenCell(last_slice, stage, perm_id)
                           : paths_[stage].back();
            }
            return aligned_indices_[stage][0] == 0 ? ChosenCell(0, stage, perm_id) : paths_[stage].front();
        }

        bool FirstSliceValid(int perm_id) const {
            for (int stage = 0; stage < width_; ++stage) {
                if (IsForwardStage(stage)) {
                    if (!StageStartAttachmentValid(stage, perm_id)) {
                        return false;
                    }
                } else {
                    if (!StageEndAttachmentValid(stage, perm_id)) {
                        return false;
                    }
                }
            }
            for (int stage = 1; stage + 1 < width_; stage += 2) {
                if (!IsKingAdjacent(StageEndNode(stage, perm_id), StageStartNode(stage + 1, perm_id))) {
                    return false;
                }
            }
            return true;
        }

        bool LastSliceValid(int perm_id) const {
            for (int stage = 0; stage < width_; ++stage) {
                if (IsForwardStage(stage)) {
                    if (!StageEndAttachmentValid(stage, perm_id)) {
                        return false;
                    }
                } else {
                    if (!StageStartAttachmentValid(stage, perm_id)) {
                        return false;
                    }
                }
            }
            for (int stage = 0; stage + 1 < width_; stage += 2) {
                if (!IsKingAdjacent(StageEndNode(stage, perm_id), StageStartNode(stage + 1, perm_id))) {
                    return false;
                }
            }
            return true;
        }

        bool TransitionValid(int slice, int perm_id, int next_perm_id) const {
            for (int stage = 0; stage < width_; ++stage) {
                const int cur_idx = aligned_indices_[stage][slice];
                const int next_idx = aligned_indices_[stage][slice + 1];
                const Cell &cur_cell = ChosenCell(slice, stage, perm_id);
                const Cell &next_cell = ChosenCell(slice + 1, stage, next_perm_id);

                if (IsForwardStage(stage)) {
                    if (next_idx == cur_idx + 1) {
                        if (!IsKingAdjacent(cur_cell, next_cell)) {
                            return false;
                        }
                    } else if (!(IsKingAdjacent(cur_cell, paths_[stage][cur_idx + 1]) &&
                                 IsKingAdjacent(paths_[stage][next_idx - 1], next_cell))) {
                        return false;
                    }
                } else {
                    if (next_idx == cur_idx + 1) {
                        if (!IsKingAdjacent(next_cell, cur_cell)) {
                            return false;
                        }
                    } else if (!(IsKingAdjacent(next_cell, paths_[stage][next_idx - 1]) &&
                                 IsKingAdjacent(paths_[stage][cur_idx + 1], cur_cell))) {
                        return false;
                    }
                }
            }
            return true;
        }

        void BuildTransitionGraph() {
            const int perm_count = static_cast<int>(permutations_.size());
            first_valid_perm_.assign(perm_count, 0);
            last_valid_perm_.assign(perm_count, 0);
            for (int perm_id = 0; perm_id < perm_count; ++perm_id) {
                first_valid_perm_[perm_id] = static_cast<char>(FirstSliceValid(perm_id));
                last_valid_perm_[perm_id] = static_cast<char>(LastSliceValid(perm_id));
            }

            const int slices = static_cast<int>(aligned_indices_[width_ - 1].size());
            slice_pattern_id_.clear();
            transition_next_.clear();
            if (slices <= 1) {
                return;
            }

            std::map<std::vector<int>, int> pattern_to_id;
            std::vector<int> representative_slice;
            slice_pattern_id_.assign(slices - 1, -1);
            for (int slice = 0; slice + 1 < slices; ++slice) {
                std::vector<int> gaps(width_);
                for (int stage = 0; stage < width_; ++stage) {
                    gaps[stage] = aligned_indices_[stage][slice + 1] - aligned_indices_[stage][slice];
                }
                const auto [it, inserted] =
                    pattern_to_id.emplace(gaps, static_cast<int>(transition_next_.size()));
                if (inserted) {
                    representative_slice.push_back(slice);
                    transition_next_.push_back(std::vector<std::vector<int>>(perm_count));
                }
                slice_pattern_id_[slice] = it->second;
            }

            for (int pattern_id = 0; pattern_id < static_cast<int>(transition_next_.size()); ++pattern_id) {
                const int rep_slice = representative_slice[pattern_id];
                for (int perm_id = 0; perm_id < perm_count; ++perm_id) {
                    for (int next_perm_id = 0; next_perm_id < perm_count; ++next_perm_id) {
                        if (TransitionValid(rep_slice, perm_id, next_perm_id)) {
                            transition_next_[pattern_id][perm_id].push_back(next_perm_id);
                        }
                    }
                }
            }
        }

        bool Optimize() {
            const int slices = static_cast<int>(aligned_indices_[width_ - 1].size());
            if (slices == 0) {
                return false;
            }

            const int perm_count = static_cast<int>(permutations_.size());
            const std::int64_t neg_inf = std::numeric_limits<std::int64_t>::lowest() / 4;
            const std::int64_t fixed_gain = FixedGain();

            std::vector<std::vector<std::int64_t>> slice_gain(
                slices, std::vector<std::int64_t>(perm_count, 0));
            for (int slice = 0; slice < slices; ++slice) {
                for (int perm_id = 0; perm_id < perm_count; ++perm_id) {
                    slice_gain[slice][perm_id] = SliceGain(slice, perm_id);
                }
            }

            std::vector<std::vector<std::int64_t>> dp(
                slices, std::vector<std::int64_t>(perm_count, neg_inf));
            std::vector<std::vector<int>> prev(slices, std::vector<int>(perm_count, -1));

            for (int perm_id = 0; perm_id < perm_count; ++perm_id) {
                if (first_valid_perm_[perm_id]) {
                    dp[0][perm_id] = slice_gain[0][perm_id];
                }
            }

            for (int slice = 0; slice + 1 < slices; ++slice) {
                const int pattern_id = slice_pattern_id_[slice];
                for (int perm_id = 0; perm_id < perm_count; ++perm_id) {
                    if (dp[slice][perm_id] == neg_inf) {
                        continue;
                    }
                    for (int next_perm_id : transition_next_[pattern_id][perm_id]) {
                        const std::int64_t cand = dp[slice][perm_id] + slice_gain[slice + 1][next_perm_id];
                        if (cand > dp[slice + 1][next_perm_id]) {
                            dp[slice + 1][next_perm_id] = cand;
                            prev[slice + 1][next_perm_id] = perm_id;
                        }
                    }
                }
            }

            int best_perm_id = 0;
            std::int64_t best_value = neg_inf;
            for (int perm_id = 0; perm_id < perm_count; ++perm_id) {
                if (dp[slices - 1][perm_id] == neg_inf || !last_valid_perm_[perm_id]) {
                    continue;
                }
                const std::int64_t cand = fixed_gain + dp[slices - 1][perm_id];
                if (cand > best_value) {
                    best_value = cand;
                    best_perm_id = perm_id;
                }
            }

            if (best_value == neg_inf) {
                best_perm_ids_.clear();
                return false;
            }

            best_perm_ids_.assign(slices, 0);
            int cur = best_perm_id;
            for (int slice = slices - 1; slice >= 0; --slice) {
                best_perm_ids_[slice] = cur;
                if (slice > 0) {
                    cur = prev[slice][cur];
                    assert(cur != -1);
                }
            }
            return true;
        }

        std::vector<Cell> BuildAnswer() const {
            std::vector<Cell> answer;
            answer.reserve(n_ * n_);
            for (int stage = 0; stage < width_; ++stage) {
                if (IsForwardStage(stage)) {
                    for (int idx = 0; idx < static_cast<int>(paths_[stage].size()); ++idx) {
                        const int slice = index_to_slice_[stage][idx];
                        answer.push_back(slice == -1 ? paths_[stage][idx]
                                                     : ChosenCell(slice, stage, best_perm_ids_[slice]));
                    }
                } else {
                    for (int idx = static_cast<int>(paths_[stage].size()) - 1; idx >= 0; --idx) {
                        const int slice = index_to_slice_[stage][idx];
                        answer.push_back(slice == -1 ? paths_[stage][idx]
                                                     : ChosenCell(slice, stage, best_perm_ids_[slice]));
                    }
                }
            }
            assert(static_cast<int>(answer.size()) == n_ * n_);
            return answer;
        }
    };

    LayoutKind kind_;
    int width_;
    std::vector<std::vector<int>> permutations_;
};

struct WidthSearchEntry {
    const char *layout_name = "";
    int width = -1;
    std::optional<SearchResult> result;
};

struct MultiWidthSearchResult {
    CandidateSolution best;
    std::vector<WidthSearchEntry> entries;
};

class MultiWidthSearch {
  public:
    struct Spec {
        LayoutKind layout;
        int width;
    };

    explicit MultiWidthSearch(std::vector<Spec> specs) {
        for (const Spec &spec : specs) {
            specs_.push_back(spec);
            solvers_.emplace_back(spec.layout, spec.width);
        }
    }

    MultiWidthSearchResult Search(const Board &board) const {
        MultiWidthSearchResult result;
        bool has_best = false;
        const VariantSearch variant_search;
        const std::int64_t search_start_ms = CurrentTimeMs();

        for (int i = 0; i < static_cast<int>(specs_.size()); ++i) {
            const Spec &spec = specs_[i];
            const std::int64_t spec_start_ms = CurrentTimeMs();
            const std::optional<SearchResult> variant_result =
                variant_search.Search(board, [this, i](const Board &transformed) {
                    return solvers_[i].Solve(transformed);
                });

            WidthSearchEntry entry;
            entry.layout_name = LayoutName(spec.layout);
            entry.width = spec.width;
            entry.result = variant_result;
            if (entry.result.has_value()) {
                entry.result->baseline.width = spec.width;
                entry.result->baseline.layout_name = entry.layout_name;
                entry.result->best.width = spec.width;
                entry.result->best.layout_name = entry.layout_name;
            }
            result.entries.push_back(entry);

            if (entry.result.has_value() &&
                (!has_best || entry.result->best.score > result.best.score)) {
                result.best = entry.result->best;
                has_best = true;
            }

            std::cerr << "search_time layout=" << entry.layout_name
                      << " width=" << entry.width
                      << " elapsed_ms=" << (CurrentTimeMs() - spec_start_ms) << '\n';
        }

        assert(has_best);
        std::cerr << "search_time total_elapsed_ms="
                  << (CurrentTimeMs() - search_start_ms) << '\n';
        return result;
    }

  private:
    std::vector<Spec> specs_;
    std::vector<StripedLayoutSolver> solvers_;
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const Board board = Board::Read(std::cin);
    const MultiWidthSearch search(std::vector<MultiWidthSearch::Spec>{
        // {LayoutKind::Spiral, 3},
        // {LayoutKind::Spiral, 4},
        // {LayoutKind::Spiral, 6},
        // {LayoutKind::Zigzag, 4},
        {LayoutKind::Zigzag, 5},
    });
    const MultiWidthSearchResult result = search.Search(board);

    for (const WidthSearchEntry &entry : result.entries) {
        if (!entry.result.has_value()) {
            std::cerr << "layout=" << entry.layout_name << " width=" << entry.width << " skipped=1\n";
            continue;
        }
        std::cerr << "layout=" << entry.layout_name << " width=" << entry.width
                  << " base_score=" << entry.result->baseline.score
                  << " best_score=" << entry.result->best.score
                  << " delta=" << (entry.result->best.score - entry.result->baseline.score)
                  << " symmetry=" << entry.result->best.transform.Name()
                  << " reversed=" << (entry.result->best.reversed ? 1 : 0) << '\n';
    }
    std::cerr << "selected_layout=" << result.best.layout_name
              << " selected_width=" << result.best.width
              << " selected_score=" << result.best.score
              << " symmetry=" << result.best.transform.Name()
              << " reversed=" << (result.best.reversed ? 1 : 0) << '\n';

    const TwoOptImprover two_opt_improver(board);
    const OrOptOneImprover or_opt_improver(board);
    const OrOptTwoImprover or_opt_two_improver(board);
    const OrOptThreeImprover or_opt_three_improver(board);
    const BlockSwapImprover block_swap_improver(board);
    constexpr int kPostprocessIterations = 500;
    constexpr int kPostprocessRounds = 10;

    CandidateSolution improved = result.best;
    for (int round = 1; round <= kPostprocessRounds; ++round) {
        const std::int64_t round_start_score = improved.score;
        const std::int64_t round_start_ms = CurrentTimeMs();

        std::int64_t phase_start_ms = CurrentTimeMs();
        improved = two_opt_improver.Improve(improved, kPostprocessIterations);
        std::cerr << "postprocess_time round=" << round
                  << " phase=two_opt elapsed_ms=" << (CurrentTimeMs() - phase_start_ms) << '\n';

        phase_start_ms = CurrentTimeMs();
        improved = or_opt_improver.Improve(improved, kPostprocessIterations);
        std::cerr << "postprocess_time round=" << round
                  << " phase=or_opt1 elapsed_ms=" << (CurrentTimeMs() - phase_start_ms) << '\n';

        phase_start_ms = CurrentTimeMs();
        improved = or_opt_two_improver.Improve(improved, kPostprocessIterations);
        std::cerr << "postprocess_time round=" << round
                  << " phase=or_opt2 elapsed_ms=" << (CurrentTimeMs() - phase_start_ms) << '\n';

        phase_start_ms = CurrentTimeMs();
        improved = or_opt_three_improver.Improve(improved, kPostprocessIterations);
        std::cerr << "postprocess_time round=" << round
                  << " phase=or_opt3 elapsed_ms=" << (CurrentTimeMs() - phase_start_ms) << '\n';

        phase_start_ms = CurrentTimeMs();
        improved = block_swap_improver.Improve(improved, kPostprocessIterations);
        std::cerr << "postprocess_time round=" << round
                  << " phase=block_swap elapsed_ms=" << (CurrentTimeMs() - phase_start_ms)
                  << '\n';

        std::cerr << "postprocess round=" << round
                  << " delta=" << (improved.score - round_start_score)
                  << " elapsed_ms=" << (CurrentTimeMs() - round_start_ms)
                  << " score=" << improved.score << '\n';
        if (improved.score == round_start_score) {
            break;
        }
    }
    std::cerr << "postprocess_score=" << improved.score << '\n';

    for (const Cell &cell : improved.route) {
        std::cout << cell.row << ' ' << cell.col << '\n';
    }
    return 0;
}
