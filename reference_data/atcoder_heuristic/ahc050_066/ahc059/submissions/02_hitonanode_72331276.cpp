#pragma GCC optimize("O3")
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <utility>
#include <vector>

struct Point {
    int r;
    int c;
};

struct Solution {
    std::vector<Point> pos;
    std::vector<int> label;
    int cost = 0;
};

constexpr unsigned int kSeed = 1234567;

struct TimeKeeper {
    std::chrono::steady_clock::time_point start;
    explicit TimeKeeper(std::chrono::steady_clock::time_point s) : start(s) {}
    double elapsed() const {
        return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    }
};

int manhattan(const Point& a, const Point& b) {
    return std::abs(a.r - b.r) + std::abs(a.c - b.c);
}

struct RMQ {
    std::vector<std::vector<std::pair<int, int>>> st;
    std::vector<int> log2;

    void build(const std::vector<int>& vals) {
        int n = static_cast<int>(vals.size());
        if (n == 0) {
            st.clear();
            log2.clear();
            return;
        }
        log2.assign(n + 1, 0);
        for (int i = 2; i <= n; ++i) {
            log2[i] = log2[i / 2] + 1;
        }
        int k = log2[n] + 1;
        st.assign(k, std::vector<std::pair<int, int>>(n));
        for (int i = 0; i < n; ++i) {
            st[0][i] = {vals[i], i};
        }
        for (int lv = 1; lv < k; ++lv) {
            int len = 1 << lv;
            int half = len >> 1;
            for (int i = 0; i + len <= n; ++i) {
                auto a = st[lv - 1][i];
                auto b = st[lv - 1][i + half];
                st[lv][i] = (a.first <= b.first) ? a : b;
            }
        }
    }

    std::pair<int, int> query(int l, int r) const {
        if (l > r) {
            return {std::numeric_limits<int>::max(), -1};
        }
        int len = r - l + 1;
        int k = log2[len];
        auto a = st[k][l];
        auto b = st[k][r - (1 << k) + 1];
        return (a.first <= b.first) ? a : b;
    }
};

struct BestInsert {
    int delta;
    int i;
    int j;
};

BestInsert best_for_order(
    const Point& a,
    const Point& b,
    const std::vector<Point>& prev_point,
    const std::vector<Point>& next_point,
    const std::vector<int>& depth,
    const std::vector<int>& next_smaller,
    const std::vector<std::vector<int>>& positions_by_depth) {
    int m = static_cast<int>(prev_point.size()) - 1;
    std::vector<int> cost_a(m + 1, 0);
    std::vector<int> cost_b(m + 1, 0);
    for (int p = 0; p <= m; ++p) {
        const Point& prev = prev_point[p];
        if (p == m) {
            cost_a[p] = manhattan(prev, a);
            cost_b[p] = manhattan(prev, b);
        } else {
            const Point& nxt = next_point[p];
            int base = manhattan(prev, nxt);
            cost_a[p] = manhattan(prev, a) + manhattan(a, nxt) - base;
            cost_b[p] = manhattan(prev, b) + manhattan(b, nxt) - base;
        }
    }

    BestInsert best{std::numeric_limits<int>::max(), 0, 0};
    for (int p = 0; p <= m; ++p) {
        const Point& prev = prev_point[p];
        int delta = 0;
        if (p == m) {
            delta = manhattan(prev, a) + manhattan(a, b);
        } else {
            const Point& nxt = next_point[p];
            int base = manhattan(prev, nxt);
            delta = manhattan(prev, a) + manhattan(a, b) + manhattan(b, nxt) - base;
        }
        if (delta < best.delta) {
            best = {delta, p, p};
        }
    }

    for (const auto& pos_list : positions_by_depth) {
        int k = static_cast<int>(pos_list.size());
        if (k <= 1) {
            continue;
        }
        std::vector<int> vals_b(k);
        for (int idx = 0; idx < k; ++idx) {
            vals_b[idx] = cost_b[pos_list[idx]];
        }
        RMQ rmq;
        rmq.build(vals_b);
        for (int idx = 0; idx < k; ++idx) {
            int i = pos_list[idx];
            int limit_pos = next_smaller[i];
            int tmax_idx = static_cast<int>(std::upper_bound(pos_list.begin(), pos_list.end(), limit_pos) - pos_list.begin()) - 1;
            if (tmax_idx <= idx) {
                continue;
            }
            auto best_b = rmq.query(idx + 1, tmax_idx);
            int delta = cost_a[i] + best_b.first;
            if (delta < best.delta) {
                best = {delta, i, pos_list[best_b.second]};
            }
        }
    }

    return best;
}

void insert_pair(
    int i,
    int j,
    int label,
    const Point& first,
    const Point& second,
    std::vector<Point>& seq_pos,
    std::vector<int>& seq_label) {
    seq_pos.insert(seq_pos.begin() + i, first);
    seq_label.insert(seq_label.begin() + i, label);
    int j2 = j + 1;
    seq_pos.insert(seq_pos.begin() + j2, second);
    seq_label.insert(seq_label.begin() + j2, label);
}

std::vector<int> build_depth(const std::vector<int>& seq_label) {
    int m = static_cast<int>(seq_label.size());
    std::vector<int> depth(m + 1, 0);
    std::vector<int> stack;
    stack.reserve(m);
    for (int i = 0; i < m; ++i) {
        int label = seq_label[i];
        if (!stack.empty() && stack.back() == label) {
            stack.pop_back();
            depth[i + 1] = depth[i] - 1;
        } else {
            stack.push_back(label);
            depth[i + 1] = depth[i] + 1;
        }
    }
    return depth;
}

std::vector<int> build_next_smaller(const std::vector<int>& depth) {
    int m = static_cast<int>(depth.size()) - 1;
    std::vector<int> next_smaller(m + 1, m);
    std::vector<int> st_pos;
    st_pos.reserve(m + 1);
    for (int i = m; i >= 0; --i) {
        while (!st_pos.empty() && depth[st_pos.back()] >= depth[i]) {
            st_pos.pop_back();
        }
        next_smaller[i] = st_pos.empty() ? m : st_pos.back();
        st_pos.push_back(i);
    }
    return next_smaller;
}

std::vector<std::vector<int>> build_positions_by_depth(const std::vector<int>& depth) {
    int max_depth = 0;
    for (int d : depth) {
        if (d > max_depth) {
            max_depth = d;
        }
    }
    std::vector<std::vector<int>> positions_by_depth(max_depth + 1);
    for (int i = 0; i < static_cast<int>(depth.size()); ++i) {
        positions_by_depth[depth[i]].push_back(i);
    }
    return positions_by_depth;
}

int total_cost(const std::vector<Point>& seq_pos, const Point& start) {
    int cost = 0;
    Point cur = start;
    for (const auto& p : seq_pos) {
        cost += manhattan(cur, p);
        cur = p;
    }
    return cost;
}

long long score_from_cost(int n, int cost) {
    long long nn = static_cast<long long>(n);
    return nn * nn + 2 * nn * nn * nn - cost;
}

Solution build_initial_solution(
    const std::vector<std::array<Point, 2>>& positions,
    const Point& start) {
    int m_pairs = static_cast<int>(positions.size());
    std::vector<Point> seq_pos;
    std::vector<int> seq_label;
    std::vector<int> remaining(m_pairs);
    for (int i = 0; i < m_pairs; ++i) {
        remaining[i] = i;
    }

    while (!remaining.empty()) {
        int m = static_cast<int>(seq_pos.size());
        std::vector<int> depth = build_depth(seq_label);
        std::vector<int> next_smaller = build_next_smaller(depth);
        std::vector<std::vector<int>> positions_by_depth = build_positions_by_depth(depth);

        std::vector<Point> prev_point(m + 1);
        std::vector<Point> next_point(m + 1);
        for (int i = 0; i <= m; ++i) {
            prev_point[i] = (i == 0) ? start : seq_pos[i - 1];
        }
        for (int i = 0; i < m; ++i) {
            next_point[i] = seq_pos[i];
        }

        int best_id = -1;
        Point best_first{0, 0};
        Point best_second{0, 0};
        int best_i = 0;
        int best_j = 0;
        int best_delta = std::numeric_limits<int>::max();

        for (int id : remaining) {
            const Point& p0 = positions[id][0];
            const Point& p1 = positions[id][1];

            BestInsert ab = best_for_order(
                p0, p1, prev_point, next_point, depth, next_smaller, positions_by_depth);
            BestInsert ba = best_for_order(
                p1, p0, prev_point, next_point, depth, next_smaller, positions_by_depth);

            if (ab.delta <= ba.delta) {
                if (ab.delta < best_delta) {
                    best_delta = ab.delta;
                    best_id = id;
                    best_first = p0;
                    best_second = p1;
                    best_i = ab.i;
                    best_j = ab.j;
                }
            } else {
                if (ba.delta < best_delta) {
                    best_delta = ba.delta;
                    best_id = id;
                    best_first = p1;
                    best_second = p0;
                    best_i = ba.i;
                    best_j = ba.j;
                }
            }
        }

        insert_pair(best_i, best_j, best_id, best_first, best_second, seq_pos, seq_label);

        for (int i = 0; i < static_cast<int>(remaining.size()); ++i) {
            if (remaining[i] == best_id) {
                remaining[i] = remaining.back();
                remaining.pop_back();
                break;
            }
        }
    }

    Solution sol;
    sol.pos = std::move(seq_pos);
    sol.label = std::move(seq_label);
    sol.cost = total_cost(sol.pos, start);
    return sol;
}

void insert_label_greedy(
    int label,
    const std::vector<std::array<Point, 2>>& positions,
    const Point& start,
    std::vector<Point>& seq_pos,
    std::vector<int>& seq_label) {
    int m = static_cast<int>(seq_pos.size());
    std::vector<int> depth = build_depth(seq_label);
    std::vector<int> next_smaller = build_next_smaller(depth);
    std::vector<std::vector<int>> positions_by_depth = build_positions_by_depth(depth);

    std::vector<Point> prev_point(m + 1);
    std::vector<Point> next_point(m + 1);
    for (int i = 0; i <= m; ++i) {
        prev_point[i] = (i == 0) ? start : seq_pos[i - 1];
    }
    for (int i = 0; i < m; ++i) {
        next_point[i] = seq_pos[i];
    }

    const Point& p0 = positions[label][0];
    const Point& p1 = positions[label][1];
    BestInsert ab = best_for_order(
        p0, p1, prev_point, next_point, depth, next_smaller, positions_by_depth);
    BestInsert ba = best_for_order(
        p1, p0, prev_point, next_point, depth, next_smaller, positions_by_depth);

    if (ab.delta <= ba.delta) {
        insert_pair(ab.i, ab.j, label, p0, p1, seq_pos, seq_label);
    } else {
        insert_pair(ba.i, ba.j, label, p1, p0, seq_pos, seq_label);
    }
}

Solution build_initial_solution_b(
    const std::vector<std::array<Point, 2>>& positions,
    const Point& start) {
    int m_pairs = static_cast<int>(positions.size());
    std::vector<int> order(m_pairs);
    for (int i = 0; i < m_pairs; ++i) {
        order[i] = i;
    }
    std::mt19937 rng(kSeed);
    std::shuffle(order.begin(), order.end(), rng);

    std::vector<Point> seq_pos;
    std::vector<int> seq_label;
    for (int label : order) {
        insert_label_greedy(label, positions, start, seq_pos, seq_label);
    }

    Solution sol;
    sol.pos = std::move(seq_pos);
    sol.label = std::move(seq_label);
    sol.cost = total_cost(sol.pos, start);
    return sol;
}

int pick_random_index(std::mt19937& rng, int lo, int hi) {
    if (lo > hi) {
        return -1;
    }
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(rng);
}

bool is_empty_stack_sequence(const std::vector<int>& seq_label) {
    std::vector<int> stack;
    stack.reserve(seq_label.size());
    for (int label : seq_label) {
        if (!stack.empty() && stack.back() == label) {
            stack.pop_back();
        } else {
            stack.push_back(label);
        }
    }
    return stack.empty();
}

Solution local_search(
    const Solution& initial,
    const Point& start,
    int n,
    const TimeKeeper& timer,
    double time_limit_sec) {
    std::mt19937 rng(kSeed);
    Solution cur = initial;
    Solution best = initial;
    const double start_temp = 0.1;
    const double end_temp = 0.1;
    long long iterations = 0;
    long long attempts[4] = {0, 0, 0, 0};
    long long accepts[4] = {0, 0, 0, 0};
    bool depth_ready = false;
    std::vector<int> depth_cache;
    RMQ depth_rmq;

    auto build_label_positions =
        [](const std::vector<int>& labels,
           std::vector<int>& first,
           std::vector<int>& second,
           std::vector<int>& mate) {
            int m_pairs = static_cast<int>(first.size());
            std::fill(first.begin(), first.end(), -1);
            std::fill(second.begin(), second.end(), -1);
            int m = static_cast<int>(labels.size());
            mate.assign(m, -1);
            for (int i = 0; i < m; ++i) {
                int label = labels[i];
                if (first[label] == -1) {
                    first[label] = i;
                } else {
                    second[label] = i;
                }
            }
            for (int label = 0; label < m_pairs; ++label) {
                if (first[label] >= 0 && second[label] >= 0) {
                    mate[first[label]] = second[label];
                    mate[second[label]] = first[label];
                }
            }
        };

    auto best_insert_for_pair =
        [&](const Point& p0,
            const Point& p1,
            std::vector<Point>& seq_pos,
            std::vector<int>& seq_label,
            int label) {
            int m = static_cast<int>(seq_pos.size());
            std::vector<int> depth = build_depth(seq_label);
            std::vector<int> next_smaller = build_next_smaller(depth);
            std::vector<std::vector<int>> positions_by_depth = build_positions_by_depth(depth);
            std::vector<Point> prev_point(m + 1);
            std::vector<Point> next_point(m + 1);
            for (int i = 0; i <= m; ++i) {
                prev_point[i] = (i == 0) ? start : seq_pos[i - 1];
            }
            for (int i = 0; i < m; ++i) {
                next_point[i] = seq_pos[i];
            }

            BestInsert ab = best_for_order(
                p0, p1, prev_point, next_point, depth, next_smaller, positions_by_depth);
            BestInsert ba = best_for_order(
                p1, p0, prev_point, next_point, depth, next_smaller, positions_by_depth);

            if (ab.delta <= ba.delta) {
                insert_pair(ab.i, ab.j, label, p0, p1, seq_pos, seq_label);
            } else {
                insert_pair(ba.i, ba.j, label, p1, p0, seq_pos, seq_label);
            }
        };

    while (true) {
        ++iterations;
        double elapsed = timer.elapsed();
        if (elapsed >= time_limit_sec) {
            break;
        }
        double t = elapsed / time_limit_sec;
        double temp = start_temp + (end_temp - start_temp) * t;

        int m = static_cast<int>(cur.pos.size());
        int m_pairs = m / 2;
        std::vector<int> first(m_pairs, -1), second(m_pairs, -1), mate;
        build_label_positions(cur.label, first, second, mate);

        int start_idx = pick_random_index(rng, 0, m - 1);
        int count = std::min(15, m - start_idx);
        std::vector<char> remove_flag(m, 0);
        for (int i = 0; i < count; ++i) {
            int idx = start_idx + i;
            remove_flag[idx] = 1;
            int other = mate[idx];
            if (other >= 0) {
                remove_flag[other] = 1;
            }
        }
        std::vector<char> removed_label(m_pairs, 0);
        std::vector<int> removed_labels;
        for (int i = 0; i < m; ++i) {
            if (remove_flag[i]) {
                int label = cur.label[i];
                if (!removed_label[label]) {
                    removed_label[label] = 1;
                    removed_labels.push_back(label);
                }
            }
        }

        std::vector<Point> reduced_pos;
        std::vector<int> reduced_label;
        reduced_pos.reserve(m - static_cast<int>(removed_labels.size()) * 2);
        reduced_label.reserve(m - static_cast<int>(removed_labels.size()) * 2);
        for (int i = 0; i < m; ++i) {
            if (!remove_flag[i]) {
                reduced_pos.push_back(cur.pos[i]);
                reduced_label.push_back(cur.label[i]);
            }
        }

        std::shuffle(removed_labels.begin(), removed_labels.end(), rng);
        for (int label : removed_labels) {
            int a = first[label];
            int b = second[label];
            if (a < 0 || b < 0) {
                continue;
            }
            const Point& p0 = cur.pos[a];
            const Point& p1 = cur.pos[b];
            best_insert_for_pair(p0, p1, reduced_pos, reduced_label, label);
        }

        int cand_cost = total_cost(reduced_pos, start);
        int delta = cand_cost - cur.cost;
        bool accept = false;
        if (delta <= 0) {
            accept = true;
        } else {
            double prob = std::exp(-static_cast<double>(delta) / temp);
            double rnum = std::generate_canonical<double, 10>(rng);
            if (rnum < prob) {
                accept = true;
            }
        }
        if (accept) {
            cur.pos = std::move(reduced_pos);
            cur.label = std::move(reduced_label);
            cur.cost = cand_cost;
            ++accepts[2];
            depth_ready = false;
            if (cur.cost < best.cost) {
                best = cur;
            }
        }
    }

    std::cerr << "ls_iters=" << iterations
              << " attempts=" << attempts[0] << "," << attempts[1] << "," << attempts[2] << "," << attempts[3]
              << " accepts=" << accepts[0] << "," << accepts[1] << "," << accepts[2] << "," << accepts[3]
              << " best_cost=" << best.cost
              << " best_score=" << score_from_cost(n, best.cost) << "\n";
    return best;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    TimeKeeper timer(std::chrono::steady_clock::now());
    int n;
    if (!(std::cin >> n)) {
        return 0;
    }
    std::vector<std::vector<int>> grid(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cin >> grid[i][j];
        }
    }
    int m_pairs = n * n / 2;
    std::vector<std::array<Point, 2>> positions(m_pairs);
    std::vector<int> count(m_pairs, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int id = grid[i][j];
            positions[id][count[id]++] = {i, j};
        }
    }

    Point start{0, 0};
    Solution initial = build_initial_solution_b(positions, start);
    std::cerr << "init_ms=" << static_cast<long long>(timer.elapsed() * 1000.0)
              << " init_cost=" << initial.cost
              << " init_score=" << score_from_cost(n, initial.cost) << "\n";
    double time_limit_sec = 1.98;
    Solution best = local_search(initial, start, n, timer, time_limit_sec);
    std::cerr << "final_cost=" << best.cost
              << " final_score=" << score_from_cost(n, best.cost) << "\n";

    std::string ans;
    ans.reserve(20000);
    Point cur = start;
    int move_count = 0;
    for (const auto& p : best.pos) {
        while (cur.r < p.r) {
            ans.push_back('D');
            ++cur.r;
            ++move_count;
        }
        while (cur.r > p.r) {
            ans.push_back('U');
            --cur.r;
            ++move_count;
        }
        while (cur.c < p.c) {
            ans.push_back('R');
            ++cur.c;
            ++move_count;
        }
        while (cur.c > p.c) {
            ans.push_back('L');
            --cur.c;
            ++move_count;
        }
        ans.push_back('Z');
    }

    std::cerr << "moves=" << move_count << " total_ops=" << ans.size() << "\n";

    for (char ch : ans) {
        std::cout << ch << "\n";
    }
    return 0;
}
