#include <bits/stdc++.h>
using namespace std;

struct Op {
    int m;
    int d;
};

struct Connector {
    int belt_id;
    int submit_cell;
    int outside_cell;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    constexpr int N = 20;
    constexpr int V = N * N;
    constexpr int EXIT = 0 * N + 10;
    constexpr int T_LIMIT = 100000;
    constexpr int SUBMIT = 0;
    constexpr int SUBMIT_DIR = 1;

    int inputN;
    cin >> inputN;

    auto cell_id = [&](int i, int j) {
        return i * N + j;
    };

    vector<int> board(V), pos(V);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int a;
            cin >> a;
            int c = cell_id(i, j);
            board[c] = a;
            pos[a] = c;
        }
    }

    vector<vector<int>> belts;

    // 0: submit belt.  This is the same 200-cell belt as the base code.
    {
        vector<int> b;

        for (int j = 1; j <= 18; j++) b.push_back(cell_id(0, j));

        for (int i = 1; i <= 19; i++) b.push_back(cell_id(i, 18));
        b.push_back(cell_id(19, 17));
        for (int i = 18; i >= 3; i--) b.push_back(cell_id(i, 17));
        for (int j = 16; j >= 14; j--) b.push_back(cell_id(3, j));

        for (int i = 4; i <= 19; i++) b.push_back(cell_id(i, 14));
        b.push_back(cell_id(19, 13));
        for (int i = 18; i >= 3; i--) b.push_back(cell_id(i, 13));
        for (int j = 12; j >= 10; j--) b.push_back(cell_id(3, j));

        for (int i = 4; i <= 19; i++) b.push_back(cell_id(i, 10));
        b.push_back(cell_id(19, 9));
        for (int i = 18; i >= 3; i--) b.push_back(cell_id(i, 9));
        for (int j = 8; j >= 6; j--) b.push_back(cell_id(3, j));

        for (int i = 4; i <= 19; i++) b.push_back(cell_id(i, 6));
        b.push_back(cell_id(19, 5));
        for (int i = 18; i >= 3; i--) b.push_back(cell_id(i, 5));
        for (int j = 4; j >= 2; j--) b.push_back(cell_id(3, j));

        for (int i = 4; i <= 19; i++) b.push_back(cell_id(i, 2));
        b.push_back(cell_id(19, 1));
        for (int i = 18; i >= 1; i--) b.push_back(cell_id(i, 1));

        belts.push_back(b);
    }

    const int L = (int)belts[SUBMIT].size();

    vector<int> submit_idx(V, -1);
    for (int k = 0; k < L; k++) {
        submit_idx[belts[SUBMIT][k]] = k;
    }

    vector<Connector> connectors;
    vector<int> connector_by_submit_cell(V, -1);
    vector<int> connector_by_outside_cell(V, -1);
    vector<int> connector_by_submit_idx(L, -1);

    auto add_connector = [&](int outside, int submit) {
        if (outside < 0 || outside >= V) return;
        if (submit < 0 || submit >= V) return;
        if (submit_idx[submit] < 0) return;
        if (submit_idx[outside] >= 0) return;
        if (connector_by_submit_cell[submit] != -1) return;
        if (connector_by_outside_cell[outside] != -1) return;

        int belt_id = (int)belts.size();
        belts.push_back({outside, submit});

        int cid = (int)connectors.size();
        connectors.push_back({belt_id, submit, outside});

        connector_by_submit_cell[submit] = cid;
        connector_by_outside_cell[outside] = cid;
        connector_by_submit_idx[submit_idx[submit]] = cid;
    };

    // No 2x2 side belts are used.  Every non-submit cell is connected directly
    // to exactly one submit-belt cell by a 2-cell connector.

    // Extreme columns.
    for (int i = 0; i < N; i++) {
        add_connector(cell_id(i, 0), cell_id(i, 1));
        add_connector(cell_id(i, 19), cell_id(i, 18));
    }

    // Upper opened area.
    for (int j = 2; j <= 17; j++) {
        add_connector(cell_id(1, j), cell_id(0, j));
        add_connector(cell_id(2, j), cell_id(3, j));
    }

    // Lower width-2 gaps.
    for (int i = 4; i < N; i++) {
        add_connector(cell_id(i, 3), cell_id(i, 2));
        add_connector(cell_id(i, 4), cell_id(i, 5));

        add_connector(cell_id(i, 7), cell_id(i, 6));
        add_connector(cell_id(i, 8), cell_id(i, 9));

        add_connector(cell_id(i, 11), cell_id(i, 10));
        add_connector(cell_id(i, 12), cell_id(i, 13));

        add_connector(cell_id(i, 15), cell_id(i, 14));
        add_connector(cell_id(i, 16), cell_id(i, 17));
    }

    const int M = (int)belts.size();

    vector<Op> ops;

    auto is_submit_cell = [&](int c) {
        return c >= 0 && submit_idx[c] >= 0;
    };

    auto submit_dist_idx = [&](int idx) {
        return (submit_idx[EXIT] - idx + L) % L;
    };

    auto idx_from_dist = [&](int dist) {
        dist %= L;
        if (dist < 0) dist += L;
        return (submit_idx[EXIT] - dist + L) % L;
    };

    auto apply_op_state = [&](int m, int d) {
        const auto &b = belts[m];
        int len = (int)b.size();
        vector<int> oldv(len);

        for (int i = 0; i < len; i++) {
            oldv[i] = board[b[i]];
        }

        for (int i = 0; i < len; i++) {
            int ni = (i + d + len) % len;
            int c = b[ni];
            int v = oldv[i];
            board[c] = v;
            if (v >= 0) pos[v] = c;
        }
    };

    int cur = 0;

    auto remove_if_possible = [&]() {
        if (cur < V && board[EXIT] == cur) {
            board[EXIT] = -1;
            pos[cur] = -1;
            cur++;
        }
    };

    auto do_op = [&](int m, int d) -> bool {
        if ((int)ops.size() >= T_LIMIT) return false;
        if (m == SUBMIT && d != SUBMIT_DIR) return false;

        ops.push_back({m, d});
        apply_op_state(m, d);
        remove_if_possible();
        return true;
    };

    auto rotate_submit = [&]() -> bool {
        return do_op(SUBMIT, SUBMIT_DIR);
    };

    auto swap_connector = [&](int cid) -> bool {
        return do_op(connectors[cid].belt_id, 1);
    };

    remove_if_possible();

    auto cargo_on_submit = [&](int x) {
        return 0 <= x && x < V && pos[x] >= 0 && is_submit_cell(pos[x]);
    };

    auto cargo_outside_connector = [&](int x) -> int {
        if (x < 0 || x >= V) return -1;
        if (pos[x] < 0) return -1;
        if (is_submit_cell(pos[x])) return -1;
        return connector_by_outside_cell[pos[x]];
    };

    auto select_leave_for_zero_load = [&]() -> int {
        if (cur != 0 || pos[0] < 0 || is_submit_cell(pos[0])) return -1;

        if (cargo_on_submit(200)) return 200;
        for (int x = 1; x <= 199; x++) {
            if (cargo_on_submit(x)) return x;
        }
        return -1;
    };

    auto evacuate_first_half_from_submit = [&](int leave) -> bool {
        auto need_evac = [&](int x) {
            if (x < 0) return false;
            if (x == leave) return false;
            int lo = max(cur, 1);
            return lo <= x && x <= 200;
        };

        auto any_need_on_submit = [&]() {
            for (int x = max(cur, 1); x <= 200; x++) {
                if (x == leave) continue;
                if (cargo_on_submit(x)) return true;
            }
            return false;
        };

        int guard = 0;
        while (any_need_on_submit() && guard <= L) {
            for (int cid = 0; cid < (int)connectors.size(); cid++) {
                const auto &co = connectors[cid];
                int outside = board[co.outside_cell];
                int on_submit = board[co.submit_cell];
                if (outside >= 201 && need_evac(on_submit)) {
                    if (!swap_connector(cid)) return false;
                }
            }

            if (!any_need_on_submit()) break;
            if (!rotate_submit()) return false;
            guard++;
        }

        return !any_need_on_submit();
    };

    auto load_zero_if_needed = [&](int leave) -> bool {
        if (cur != 0) return true;
        if (pos[0] < 0) return true;
        if (is_submit_cell(pos[0])) return true;
        if (leave < 0 || pos[leave] < 0 || !is_submit_cell(pos[leave])) return false;

        int cid = cargo_outside_connector(0);
        if (cid < 0) return false;

        int submit_cell = connectors[cid].submit_cell;

        for (int step = 0; step <= L; step++) {
            if (board[submit_cell] == leave) {
                return swap_connector(cid);
            }
            if (step == L) break;
            if (!rotate_submit()) return false;
        }

        return false;
    };

    auto place_ordered_range = [&](int end_label) -> bool {
        if (cur > end_label) return true;

        vector<vector<int>> at_time(L);
        int max_t = -1;

        bool has_anchor = (cur <= end_label && pos[cur] >= 0 && is_submit_cell(pos[cur]));
        int anchor_label = cur;
        int anchor_idx = has_anchor ? submit_idx[pos[cur]] : -1;

        int first = has_anchor ? cur + 1 : cur;

        for (int x = first; x <= end_label; x++) {
            if (pos[x] < 0) continue;
            if (is_submit_cell(pos[x])) {
                continue;
            }

            int cid = cargo_outside_connector(x);
            if (cid < 0) return false;

            int target_idx;
            if (has_anchor) {
                target_idx = (anchor_idx - (x - anchor_label)) % L;
                if (target_idx < 0) target_idx += L;
            } else {
                int dist = x - cur + 1;
                target_idx = idx_from_dist(dist);
            }

            int conn_idx = submit_idx[connectors[cid].submit_cell];
            int t = (conn_idx - target_idx + L) % L;
            at_time[t].push_back(x);
            max_t = max(max_t, t);
        }

        if (max_t < 0) return true;

        for (int t = 0; t <= max_t; t++) {
            for (int x : at_time[t]) {
                if (pos[x] < 0) continue;
                if (is_submit_cell(pos[x])) continue;

                int cid = cargo_outside_connector(x);
                if (cid < 0) return false;

                if (board[connectors[cid].outside_cell] != x) return false;
                if (!swap_connector(cid)) return false;
            }

            if (t < max_t) {
                if (!rotate_submit()) return false;
            }
        }

        return true;
    };

    auto flush_until = [&](int next_cur) -> bool {
        while (cur < next_cur) {
            if (pos[cur] < 0) {
                cur++;
                continue;
            }
            if (!is_submit_cell(pos[cur])) {
                return false;
            }
            if (!rotate_submit()) return false;
        }
        return true;
    };


    auto evacuate_range_to_empty_from_submit = [&](int lo, int hi) -> bool {
        auto need_evac = [&](int x) {
            return lo <= x && x <= hi;
        };

        auto any_need_on_submit = [&]() {
            for (int x = lo; x <= hi; x++) {
                if (cargo_on_submit(x)) return true;
            }
            return false;
        };

        int guard = 0;
        while (any_need_on_submit() && guard <= L) {
            for (int cid = 0; cid < (int)connectors.size(); cid++) {
                const auto &co = connectors[cid];
                int outside = board[co.outside_cell];
                int on_submit = board[co.submit_cell];
                if (outside == -1 && need_evac(on_submit)) {
                    if (!swap_connector(cid)) return false;
                }
            }

            if (!any_need_on_submit()) break;
            if (!rotate_submit()) return false;
            guard++;
        }

        return !any_need_on_submit();
    };

    auto load_current_if_outside = [&]() -> bool {
        if (cur >= V) return true;
        if (pos[cur] < 0) return true;
        if (is_submit_cell(pos[cur])) return true;

        int cid = cargo_outside_connector(cur);
        if (cid < 0) return false;
        return swap_connector(cid);
    };

    // Phase 1: clear 1..200 from the submit belt.  If 0 is outside, keep one
    // low cargo on the belt and use it as the swap target for loading 0.
    int leave = select_leave_for_zero_load();
    if (!evacuate_first_half_from_submit(leave)) {
        // The construction should make this reachable.  If not, output the
        // partial operation sequence rather than producing invalid output.
    } else if (!load_zero_if_needed(leave)) {
        // Partial output fallback.
    } else {
        // Place and unload the first half.
        if (place_ordered_range(199)) {
            flush_until(200);
        }
    }

    // Phase 2: make the submit belt empty for 201..399, then load 200,
    // place 201..399, and unload everything.
    if (cur == 200) {
        if (evacuate_range_to_empty_from_submit(201, 399)) {
            if (load_current_if_outside()) {
                if (place_ordered_range(399)) {
                    flush_until(400);
                }
            }
        }
    }

    cout << M << '\n';
    for (int m = 0; m < M; m++) {
        cout << belts[m].size();
        for (int c : belts[m]) {
            cout << ' ' << c / N << ' ' << c % N;
        }
        cout << '\n';
    }

    cout << ops.size() << '\n';
    for (auto [m, d] : ops) {
        cout << m << ' ' << d << '\n';
    }

    return 0;
}
