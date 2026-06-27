#include <bits/stdc++.h>

#include <optional>

#define ll long long
#define rep(i, n) for (ll i = 0; i < (ll)(n); i++)
using namespace std;

void print() { cout << endl; }
template <class Head, class... Tail>
void print(Head&& head, Tail&&... tail) {
    if (sizeof...(tail) == 0)
        cout << head << endl;  // oj用
    else {
        cout << head << " ";
        print(std::forward<Tail>(tail)...);
    }
}
template <typename T>
void vprint(const vector<T>& v) {
    rep(i, (ll)v.size()) cout << v[i] << (i != (ll)v.size() - 1 ? " " : "");
    cout << endl;
}

random_device rd;
mt19937 gen(rd());

struct Path {
    vector<int> nodes;  // move sequence from start shop, including destination shop
    int dest_shop;
    int length() const { return (int)nodes.size(); }
};

struct Problem {
    int N, M, K, T;
    vector<vector<int>> g;

    void read() {
        cin >> N >> M >> K >> T;
        g.assign(N, {});
        rep(i, M) {
            int a, b;
            cin >> a >> b;
            g[a].push_back(b);
            g[b].push_back(a);
        }
        // coordinates are not needed
        rep(i, N) {
            int x, y;
            cin >> x >> y;
        }
    }

    bool is_shop(int v) const { return v < K; }
};

struct PathGenerator {
    const Problem& prob;
    int trials = 10000;
    int max_steps = 25;
    bool built = false;
    vector<vector<Path>> cached_paths;

    explicit PathGenerator(const Problem& p) : prob(p) {}

    const vector<vector<Path>>& build() {
        if (built) return cached_paths;
        vector<vector<Path>> paths(prob.K);
        rep(s, prob.K) {
            vector<Path> raw;
            rep(t, trials) {
                auto path = random_walk_path(s);
                if (!path.nodes.empty()) raw.push_back(path);
            }
            // unique by node sequence
            set<vector<int>> seen;
            for (const auto& p : raw) {
                if (seen.insert(p.nodes).second) paths[s].push_back(p);
            }
            sort(paths[s].begin(), paths[s].end(),
                 [](const Path& a, const Path& b) { return a.length() < b.length(); });
        }
        cached_paths = std::move(paths);
        built = true;
        return cached_paths;
    }

    vector<vector<Path>> get_paths(const vector<int>& store_max_steps) const {
        vector<vector<Path>> filtered(prob.K);
        if (!built) return filtered;
        rep(s, prob.K) {
            int max_step = store_max_steps[s];
            map<int, vector<Path>> by_len;
            for (const auto& p : cached_paths[s]) {
                if (p.length() <= max_step) by_len[p.length()].push_back(p);
            }
            for (auto& [len, group] : by_len) {
                shuffle(group.begin(), group.end(), gen);
                filtered[s].insert(filtered[s].end(), group.begin(), group.end());
            }
        }
        return filtered;
    }

    bool has_path(int shop, int max_step) const {
        if (!built) return false;
        for (const auto& p : cached_paths[shop]) {
            if (p.length() <= max_step) return true;
        }
        return false;
    }

    Path random_walk_path(int start) {
        int cur = start;
        int prev = -1;
        vector<int> nodes;
        rep(step, max_steps) {
            const auto& adj = prob.g[cur];
            vector<int> cand;
            cand.reserve(adj.size());
            for (int v : adj) {
                if (v == prev) continue;
                cand.push_back(v);
            }
            if (cand.empty()) {
                // fallback if all neighbors are prev (should not happen in 2-edge-connected graph)
                cand = adj;
            }
            uniform_int_distribution<int> dist(0, (int)cand.size() - 1);
            int nxt = cand[dist(gen)];
            prev = cur;
            cur = nxt;
            nodes.push_back(cur);
            if (prob.is_shop(cur)) {
                Path p;
                p.nodes = std::move(nodes);
                p.dest_shop = cur;
                return p;
            }
        }
        return Path{};
    }
};

struct Simulator {
    const Problem& prob;
    const vector<vector<Path>>& paths;

    vector<bool> tree_is_red;
    vector<bool> visited_tree;
    struct ConeKey {
        bitset<31> bits;
        uint8_t len = 0;

        bool operator==(const ConeKey& other) const {
            return len == other.len && bits == other.bits;
        }
    };
    struct ConeKeyHash {
        size_t operator()(const ConeKey& key) const {
            uint64_t val = key.bits.to_ullong();
            size_t h1 = std::hash<uint64_t>{}(val);
            size_t h2 = std::hash<uint8_t>{}(key.len);
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };
    vector<unordered_set<ConeKey, ConeKeyHash>> inventory;
    vector<int> outputs;
    vector<int> last_visit_idx;

    int cur = 0;
    int prev = -1;
    ConeKey cone;

    Simulator(const Problem& p, const vector<vector<Path>>& paths_)
        : prob(p),
          paths(paths_),
          tree_is_red(p.N, false),
          visited_tree(p.N, false),
          inventory(p.K),
          last_visit_idx(p.N, -1) {}

    void run() {
        while ((int)outputs.size() < prob.T) {
            if (prob.is_shop(cur)) {
                auto next = choose_path(cur);
                if (!next.has_value()) break;
                follow_path(next.value().path, next.value().flip_node);
            } else {
                // Should not happen in this strategy, but keep it safe.
                step_random();
            }
        }
    }

    int score() const {
        int sum = 0;
        for (const auto& s : inventory) sum += (int)s.size();
        return sum;
    }

    struct Choice {
        Path path;
        int flip_node;
    };

    static constexpr int kFlipAlreadyDone = -2;

    optional<Choice> choose_path(int shop) {
        const auto& ps = paths[shop];
        for (const auto& p : ps) {
            if (!p.nodes.empty() && prev != -1 && p.nodes[0] == prev) continue;
            ConeKey s = build_cone_key(p);
            if (!inventory[p.dest_shop].count(s)) return Choice{p, -1};
        }
        for (const auto& p : ps) {
            if (!p.nodes.empty() && prev != -1 && p.nodes[0] == prev) continue;
            int flip_node = find_flip_node(p);
            if (flip_node == kFlipAlreadyDone) return Choice{p, -1};
            if (flip_node != -1) return Choice{p, flip_node};
        }
        if (ps.empty()) return nullopt;
        vector<int> idx;
        idx.reserve(ps.size());
        rep(i, (ll)ps.size()) {
            if (!ps[i].nodes.empty() && prev != -1 && ps[i].nodes[0] == prev) continue;
            idx.push_back((int)i);
        }
        if (idx.empty()) return nullopt;
        shuffle(idx.begin(), idx.end(), gen);
        int pick = min(5, (int)idx.size());
        int best = idx[0];
        for (int i = 1; i < pick; i++) {
            int id = idx[i];
            if (ps[id].length() < ps[best].length()) best = id;
        }
        return Choice{ps[best], -1};
    }

    ConeKey build_cone_key(const Path& p) const {
        ConeKey s;
        for (int v : p.nodes) {
            if (!prob.is_shop(v)) {
                if (tree_is_red[v]) s.bits.set(s.len);
                s.len++;
            }
        }
        return s;
    }

    int find_flip_node(const Path& p) {
        ConeKey base;
        vector<vector<int>> pos_by_node(prob.N);
        for (int v : p.nodes) {
            if (!prob.is_shop(v)) {
                int idx = (int)base.len;
                if (tree_is_red[v]) base.bits.set(base.len);
                base.len++;
                pos_by_node[v].push_back(idx);
            }
        }
        // Only try flipping when the current color sequence already exists for dest_shop.
        if (!inventory[p.dest_shop].count(base)) return -1;
        vector<int> candidates;
        for (int v = prob.K; v < prob.N; v++) {
            if (tree_is_red[v]) continue;
            if (!visited_tree[v]) continue;
            if (pos_by_node[v].empty()) continue;
            ConeKey s = base;
            // NOTE: This assumes flipping v changes all occurrences in the path.
            // If the path visits v multiple times, positions before the flip should remain 'W'.
            // This simplification can produce an incorrect "new" string (likely the bug).
            for (int idx : pos_by_node[v]) s.bits.set(idx);
            if (!inventory[p.dest_shop].count(s)) {
                candidates.push_back(v);
            }
        }
        if (!candidates.empty()) {
            uniform_int_distribution<int> dist(0, (int)candidates.size() - 1);
            int v = candidates[dist(gen)];
            if (!insert_flip_after_last_visit(v)) return -1;
            tree_is_red[v] = true;
            return kFlipAlreadyDone;
        }
        return -1;
    }

    bool insert_flip_after_last_visit(int v) {
        int idx = last_visit_idx[v];
        if (idx < 0) return false;
        if ((int)outputs.size() + 1 > prob.T) return false;
        int insert_pos = idx + 1;
        if (insert_pos > (int)outputs.size()) insert_pos = (int)outputs.size();
        outputs.insert(outputs.begin() + insert_pos, -1);
        // Update cached indices for all vertices after the insertion point.
        for (int u = 0; u < prob.N; u++) {
            if (last_visit_idx[u] >= insert_pos) last_visit_idx[u]++;
        }
        return true;
    }

    void follow_path(const Path& p, int flip_node) {
        bool flipped = false;
        for (int v : p.nodes) {
            if ((int)outputs.size() >= prob.T) break;
            outputs.push_back(v);
            last_visit_idx[v] = (int)outputs.size() - 1;
            prev = cur;
            cur = v;
            if (prob.is_shop(cur)) {
                inventory[cur].insert(cone);
                cone.bits.reset();
                cone.len = 0;
            } else {
                visited_tree[cur] = true;
                if (tree_is_red[cur]) cone.bits.set(cone.len);
                cone.len++;
            }
            if (!flipped && cur == flip_node && !tree_is_red[cur] && (int)outputs.size() < prob.T) {
                outputs.push_back(-1);
                tree_is_red[cur] = true;
                flipped = true;
            }
        }
    }

    void step_random() {
        const auto& adj = prob.g[cur];
        vector<int> cand;
        cand.reserve(adj.size());
        for (int v : adj) {
            if (v == prev) continue;
            cand.push_back(v);
        }
        if (cand.empty()) cand = adj;
        uniform_int_distribution<int> dist(0, (int)cand.size() - 1);
        int nxt = cand[dist(gen)];
        outputs.push_back(nxt);
        last_visit_idx[nxt] = (int)outputs.size() - 1;
        prev = cur;
        cur = nxt;
        if (prob.is_shop(cur)) {
            inventory[cur].insert(cone);
            cone.bits.reset();
            cone.len = 0;
        } else {
            visited_tree[cur] = true;
            if (tree_is_red[cur]) cone.bits.set(cone.len);
            cone.len++;
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Problem prob;
    prob.read();

    const double time_limit_sec = 1.98;
    auto start = chrono::steady_clock::now();

    int best_score = -1;
    vector<int> best_outputs;

    PathGenerator generator(prob);
    const int min_steps = 10;
    const int max_steps = 15;
    uniform_int_distribution<int> step_dist(min_steps, max_steps);
    uniform_int_distribution<int> shop_dist(0, prob.K - 1);
    uniform_int_distribution<int> step_delta(0, 1);
    uniform_real_distribution<double> accept_dist(0.0, 1.0);
    generator.build();

    const double phase1_sec = 0.2;
    vector<int> best_store_max_steps;

    while (true) {
        auto now = chrono::steady_clock::now();
        double elapsed = chrono::duration_cast<chrono::duration<double>>(now - start).count();
        if (elapsed >= min(phase1_sec, time_limit_sec)) break;

        int max_step = step_dist(gen);
        vector<int> store_max_steps(prob.K, max_step);
        auto paths = generator.get_paths(store_max_steps);
        Simulator sim(prob, paths);
        sim.run();
        int score = sim.score();
        if (score > best_score) {
            best_score = score;
            best_outputs = sim.outputs;
            best_store_max_steps = store_max_steps;
        }
    }

    auto now_phase1 = chrono::steady_clock::now();
    double elapsed_phase1 =
        chrono::duration_cast<chrono::duration<double>>(now_phase1 - start).count();
    if (elapsed_phase1 >= time_limit_sec) {
        for (int v : best_outputs) cout << v << "\n";
        return 0;
    }

    vector<int> store_max_steps = best_store_max_steps;
    if (store_max_steps.empty()) {
        store_max_steps.assign(prob.K, step_dist(gen));
    }
    auto paths = generator.get_paths(store_max_steps);
    Simulator sim(prob, paths);
    sim.run();
    int cur_score = sim.score();
    vector<int> cur_outputs = sim.outputs;
    if (cur_score > best_score) {
        best_score = cur_score;
        best_outputs = cur_outputs;
    }

    const double start_temp = 1.0;
    const double end_temp = 0.01;
    const double sa_time_limit = max(0.0, time_limit_sec - elapsed_phase1);
    auto sa_start = chrono::steady_clock::now();

    while (true) {
        auto now = chrono::steady_clock::now();
        double elapsed = chrono::duration_cast<chrono::duration<double>>(now - start).count();
        if (elapsed >= time_limit_sec) break;
        double sa_elapsed = chrono::duration_cast<chrono::duration<double>>(now - sa_start).count();
        if (sa_elapsed > sa_time_limit) break;

        double ratio = sa_time_limit > 0.0 ? sa_elapsed / sa_time_limit : 1.0;
        double temp = start_temp * pow(end_temp / start_temp, ratio);

        int shop = shop_dist(gen);
        int delta = step_delta(gen) == 0 ? -1 : 1;
        int next_step = store_max_steps[shop] + delta;
        // if (next_step < min_steps || next_step > max_steps) continue;
        if (!generator.has_path(shop, next_step)) continue;

        vector<int> next_store_max_steps = store_max_steps;
        next_store_max_steps[shop] = next_step;
        auto next_paths = generator.get_paths(next_store_max_steps);
        Simulator next_sim(prob, next_paths);
        next_sim.run();
        int next_score = next_sim.score();

        int diff = next_score - cur_score;
        float accept_ratio = exp((double)diff / temp);
        // print("diff: ", diff, " temp: ", temp, " accept_ratio: ", accept_ratio);
        // vprint(store_max_steps);
        if (diff >= 0 || accept_ratio > accept_dist(gen)) {
            store_max_steps = std::move(next_store_max_steps);
            cur_score = next_score;
            cur_outputs = std::move(next_sim.outputs);
            if (cur_score > best_score) {
                best_score = cur_score;
                best_outputs = cur_outputs;
            }
        }
    }

    // for (int x : store_max_steps) cout << x << " ";
    // cout << endl;
    for (int v : best_outputs) cout << v << "\n";
    return 0;
}
