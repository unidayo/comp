// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/heuristic/differential_beam_search.cpp
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

/**
 * @brief Differential update beam search for AHC-style solvers.
 *
 * State is owned by the caller. This class only stores the search tree
 * (parent/action/score/hash), restores a node by apply/undo, and evaluates each
 * child as apply(action) -> evaluate/hash -> undo(action).
 *
 * Required callbacks for run():
 *   - expand(depth, node, try_action)
 *       Current external state is restored to node.
 *       Call try_action(action) for each candidate action.
 *   - apply(action)
 *       Applies action to the external state. It may return bool; false means
 *       the action was rejected and undo() is not called.
 *   - undo(action)
 *       Reverts an action for which apply() succeeded.
 *   - evaluate(depth)
 *       Returns Score for the current external state.
 *   - hash()
 *       Returns Hash for the current external state. Used for deduplication.
 *   - is_goal(depth, node)
 *       Optional early stop condition after each layer. Return false if unused.
 *
 * Score ordering is controlled by Config::larger_score_better.
 */
template <class Action, class Score = long long, class Hash = std::uint64_t>
struct DifferentialBeamSearch {
    struct Config {
        int beam_width = 1;
        int max_depth = 0;
        bool larger_score_better = true;
        bool deduplicate_by_hash = true;
        int reserve_nodes = 0;
    };

    struct Node {
        int parent = -1;
        int depth = 0;
        Action action{};
        Score score{};
        Hash hash{};
    };

    struct Candidate {
        int parent = -1;
        Action action{};
        Score score{};
        Hash hash{};
    };

    explicit DifferentialBeamSearch(Config config) : cfg(config) {
        assert(cfg.beam_width > 0);
        assert(cfg.max_depth >= 0);
        if (cfg.reserve_nodes > 0) nodes.reserve(cfg.reserve_nodes);
    }

    void reset(Score initial_score = Score{}, Hash initial_hash = Hash{}) {
        nodes.clear();
        beam.clear();
        current_node = 0;
        nodes.push_back(Node{-1, 0, Action{}, initial_score, initial_hash});
        beam.push_back(0);
    }

    template <class Expand, class Apply, class Undo, class Evaluate, class GetHash, class IsGoal>
    int run(Score initial_score,
            Hash initial_hash,
            Expand&& expand,
            Apply&& apply,
            Undo&& undo,
            Evaluate&& evaluate,
            GetHash&& get_hash,
            IsGoal&& is_goal) {
        reset(initial_score, initial_hash);
        int best = 0;

        for (int depth = 0; depth < cfg.max_depth && !beam.empty(); ++depth) {
            std::vector<Candidate> candidates;
            candidates.reserve(static_cast<std::size_t>(beam.size()) * 8);

            for (int node_id : beam) {
                restore(node_id, apply, undo);
                auto try_action = [&](const Action& action) {
                    if (!call_apply(apply, action)) return false;
                    Candidate cand;
                    cand.parent = node_id;
                    cand.action = action;
                    cand.score = evaluate(depth + 1);
                    cand.hash = get_hash();
                    candidates.push_back(std::move(cand));
                    undo(action);
                    return true;
                };
                expand(depth, nodes[node_id], try_action);
            }

            if (candidates.empty()) break;
            sort_candidates(candidates);
            if (cfg.deduplicate_by_hash) deduplicate_candidates(candidates);
            if (static_cast<int>(candidates.size()) > cfg.beam_width) {
                candidates.resize(cfg.beam_width);
            }

            beam.clear();
            beam.reserve(candidates.size());
            for (const Candidate& cand : candidates) {
                nodes.push_back(Node{cand.parent, nodes[cand.parent].depth + 1, cand.action, cand.score, cand.hash});
                int id = static_cast<int>(nodes.size()) - 1;
                beam.push_back(id);
                if (better(nodes[id], nodes[best])) best = id;
            }

            for (int node_id : beam) {
                if (is_goal(depth + 1, nodes[node_id])) {
                    best = node_id;
                    restore(best, apply, undo);
                    return best;
                }
            }
        }

        restore(best, apply, undo);
        return best;
    }

    template <class Expand, class Apply, class Undo, class Evaluate, class GetHash>
    int run(Score initial_score,
            Hash initial_hash,
            Expand&& expand,
            Apply&& apply,
            Undo&& undo,
            Evaluate&& evaluate,
            GetHash&& get_hash) {
        auto never_goal = [](int, const Node&) { return false; };
        return run(initial_score, initial_hash, std::forward<Expand>(expand), std::forward<Apply>(apply),
                   std::forward<Undo>(undo), std::forward<Evaluate>(evaluate), std::forward<GetHash>(get_hash),
                   never_goal);
    }

    std::vector<Action> actions_to(int node_id) const {
        assert(0 <= node_id && node_id < static_cast<int>(nodes.size()));
        std::vector<Action> actions;
        for (int v = node_id; nodes[v].parent != -1; v = nodes[v].parent) {
            actions.push_back(nodes[v].action);
        }
        std::reverse(actions.begin(), actions.end());
        return actions;
    }

    template <class Apply, class Undo>
    void restore(int target, Apply&& apply, Undo&& undo) {
        assert(0 <= target && target < static_cast<int>(nodes.size()));
        if (target == current_node) return;

        int a = current_node;
        int b = target;
        std::vector<int> down;

        while (nodes[a].depth > nodes[b].depth) {
            undo(nodes[a].action);
            a = nodes[a].parent;
        }
        while (nodes[b].depth > nodes[a].depth) {
            down.push_back(b);
            b = nodes[b].parent;
        }
        while (a != b) {
            undo(nodes[a].action);
            a = nodes[a].parent;
            down.push_back(b);
            b = nodes[b].parent;
        }
        for (auto it = down.rbegin(); it != down.rend(); ++it) {
            bool ok = call_apply(apply, nodes[*it].action);
            assert(ok);
        }
        current_node = target;
    }

    const Node& node(int node_id) const {
        assert(0 <= node_id && node_id < static_cast<int>(nodes.size()));
        return nodes[node_id];
    }

    const std::vector<int>& current_beam() const {
        return beam;
    }

    std::vector<Node> nodes;

private:
    Config cfg;
    std::vector<int> beam;
    int current_node = 0;

    bool better(const Node& lhs, const Node& rhs) const {
        if (lhs.score != rhs.score) {
            return cfg.larger_score_better ? lhs.score > rhs.score : lhs.score < rhs.score;
        }
        if (lhs.depth != rhs.depth) return lhs.depth < rhs.depth;
        return lhs.hash < rhs.hash;
    }

    bool better(const Candidate& lhs, const Candidate& rhs) const {
        if (lhs.score != rhs.score) {
            return cfg.larger_score_better ? lhs.score > rhs.score : lhs.score < rhs.score;
        }
        if (lhs.parent != rhs.parent) return lhs.parent < rhs.parent;
        return lhs.hash < rhs.hash;
    }

    void sort_candidates(std::vector<Candidate>& candidates) const {
        std::sort(candidates.begin(), candidates.end(), [&](const Candidate& lhs, const Candidate& rhs) {
            return better(lhs, rhs);
        });
    }

    void deduplicate_candidates(std::vector<Candidate>& candidates) const {
        std::unordered_set<Hash> seen;
        seen.reserve(candidates.size() * 2);
        std::vector<Candidate> deduped;
        deduped.reserve(candidates.size());
        for (const Candidate& cand : candidates) {
            if (seen.insert(cand.hash).second) deduped.push_back(cand);
        }
        candidates.swap(deduped);
    }

    template <class Apply>
    static bool call_apply(Apply& apply, const Action& action) {
        if constexpr (std::is_same_v<std::invoke_result_t<Apply&, const Action&>, bool>) {
            return apply(action);
        } else {
            apply(action);
            return true;
        }
    }
};
