#include <bits/stdc++.h>
using namespace std;

static const int DR[] = {-1, 1, 0, 0};
static const int DC[] = {0, 0, -1, 1};
static const int MAXN = 20;

static char dir_of(int dr, int dc)
{
    if (dr == -1) return 'U';
    if (dr == 1) return 'D';
    if (dc == -1) return 'L';
    return 'R';
}

static vector<pair<int,int>> rebuild_path(int N, uint32_t seed,
                                          const bool walls[][MAXN],
                                          const int rfood[][MAXN],
                                          const int* tgt, int tgt_off, int M,
                                          int relax,
                                          pair<int,int> start);

static int compute_E(const vector<pair<int,int>>& path,
                     const int food[][MAXN],
                     const int* tgt, int M)
{
    int sc[MAXN * MAXN];
    for (int i = 0; i < 5; i++) sc[i] = 1;

    int fi = 0;
    for (int idx = 5; idx < (int)path.size() && 5 + fi < M; idx++) {
        auto [r, c] = path[idx];
        if (food[r][c] > 0) sc[5 + fi++] = food[r][c];
    }
    if (5 + fi != M) return INT_MAX / 4;

    int E = 0;
    for (int i = 0; i < M; i++) if (sc[i] != tgt[i]) E++;
    return E;
}

static int estimate_bite_potential(int N, int M, int C,
                                   const pair<int,int>* stp,
                                   const int* stc,
                                   const int* tgt)
{
    int bidx[MAXN][MAXN];
    memset(bidx, -1, sizeof(bidx));
    for (int i = 0; i < M; i++) bidx[stp[i].first][stp[i].second] = i;

    auto [hr, hc] = stp[0];
    int best = INT_MAX / 4;
    for (int d = 0; d < 4; d++) {
        int ar = hr + DR[d], ac = hc + DC[d];
        if (ar < 0 || ar >= N || ac < 0 || ac >= N) continue;
        if (ar == stp[1].first && ac == stp[1].second) continue;
        int idx = bidx[ar][ac];
        if (idx < 2 || idx >= M - 2) continue;

        int fixed_E = 0;
        for (int i = 0; i <= idx + 1; i++) if (stc[i] != tgt[i]) fixed_E++;

        int freq_rel[8] = {}, freq_tgt[8] = {};
        for (int i = idx + 1; i <= M - 2; i++) freq_rel[stc[i + 1]]++;
        for (int i = idx + 2; i < M; i++) freq_tgt[tgt[i]]++;

        int max_match = 0;
        for (int col = 1; col <= C; col++) max_match += min(freq_rel[col], freq_tgt[col]);
        int suffix_len = M - (idx + 2);
        int optimistic_E = fixed_E + (suffix_len - max_match);
        int est_score = optimistic_E * 1024 - suffix_len;
        best = min(best, est_score);
    }
    return best;
}

static vector<pair<int,int>> build_path(int N, const int food[][MAXN],
                                        const int* tgt, int M,
                                        long long time_limit_ms,
                                        const function<long long()>& elapsed_ms)
{
    vector<pair<int,int>> fixed_prefix = {{0,0}, {1,0}, {2,0}, {3,0}, {4,0}};

    vector<pair<int,int>> best_path;
    int best_E = INT_MAX;
    uint32_t seed = 1;

    while (elapsed_ms() < time_limit_ms) {
        vector<pair<int,int>> prefix = fixed_prefix;
        bool walls[MAXN][MAXN];
        memset(walls, 0, sizeof(walls));
        for (auto [r, c] : prefix) walls[r][c] = true;

        uint32_t cur_seed = seed * 2654435761u;
        int relax_opts[] = {0, 1, 1, 1, 2, 2, 3};
        int relax = relax_opts[seed % 7];
        auto suffix = rebuild_path(N, cur_seed, walls, food, tgt, 5, M, relax, {4, 0});
        seed++;
        if (suffix.empty()) continue;

        vector<pair<int,int>> cand;
        cand.reserve(N * N);
        for (int i = 0; i < 4; i++) cand.push_back(prefix[i]);
        cand.insert(cand.end(), suffix.begin(), suffix.end());

        int E = compute_E(cand, food, tgt, M);
        if (E < best_E) {
            best_E = E;
            best_path.swap(cand);
            if (E == 0) break;
        }
    }

    if (!best_path.empty()) return best_path;

    best_path.reserve(N * N);
    for (int c = 0; c < N; c++) {
        if (c % 2 == 0) for (int r = 0; r < N; r++) best_path.push_back({r, c});
        else for (int r = N-1; r >= 0; r--) best_path.push_back({r, c});
    }
    return best_path;
}

// Warnsdorff + relaxed color priority on subgrid with walls
static vector<pair<int,int>> rebuild_path(int N, uint32_t seed,
                                          const bool walls[][MAXN],
                                          const int rfood[][MAXN],
                                          const int* tgt, int tgt_off, int M,
                                          int relax,
                                          pair<int,int> start)
{
    const int NN = N * N;
    bool vis[MAXN * MAXN];
    memset(vis, 0, sizeof(vis));
    int nwalls = 0;
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            if (walls[r][c]) { vis[r*N+c] = true; nwalls++; }

    vector<pair<int,int>> path;
    path.reserve(NN - nwalls + 1);
    path.push_back(start);

    auto rng = [&]() -> uint32_t { seed ^= seed<<13; seed ^= seed>>17; seed ^= seed<<5; return seed; };

    int w[MAXN * MAXN] = {};
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++) {
            if (vis[r*N+c]) continue;
            int cnt = 0;
            for (int di = 0; di < 4; di++) {
                int nr = r+DR[di], nc = c+DC[di];
                if (nr>=0 && nr<N && nc>=0 && nc<N && !vis[nr*N+nc]) cnt++;
            }
            w[r*N+c] = cnt;
        }

    int food_count = 0;
    int target_size = NN - nwalls + 1;

    while ((int)path.size() < target_size) {
        auto [r, c] = path.back();
        int adj[4], adj_w[4], nadj = 0;
        for (int di = 0; di < 4; di++) {
            int nr = r + DR[di], nc = c + DC[di];
            if (nr < 0 || nr >= N || nc < 0 || nc >= N || vis[nr*N+nc]) continue;
            adj[nadj] = di;
            adj_w[nadj] = w[nr*N+nc];
            nadj++;
        }
        if (nadj == 0) return {};

        int min_w = *min_element(adj_w, adj_w + nadj);
        int color_target = (tgt_off + food_count < M) ? tgt[tgt_off + food_count] : 0;
        int chosen = -1;
        if (relax > 0 && color_target > 0) {
            int best_w = INT_MAX;
            int color_cands[4], ncol = 0;
            for (int i = 0; i < nadj; i++) {
                if (adj_w[i] > min_w + relax) continue;
                int nr = r + DR[adj[i]], nc = c + DC[adj[i]];
                if (rfood[nr][nc] != color_target) continue;
                if (adj_w[i] > min_w && min_w == 1) {
                    bool safe = true;
                    for (int m = 0; m < nadj && safe; m++) {
                        if (adj_w[m] != 1) continue;
                        int rm = r + DR[adj[m]], cm = c + DC[adj[m]];
                        for (int dj = 0; dj < 4; dj++)
                            if (rm+DR[dj]==nr && cm+DC[dj]==nc) { safe = false; break; }
                    }
                    if (!safe) continue;
                }
                if (adj_w[i] < best_w) { best_w = adj_w[i]; ncol = 0; }
                if (adj_w[i] == best_w) color_cands[ncol++] = adj[i];
            }
            if (ncol > 0) chosen = color_cands[rng() % ncol];
        }

        if (chosen == -1) {
            int cands[4], ncands = 0;
            for (int i = 0; i < nadj; i++) if (adj_w[i] == min_w) cands[ncands++] = adj[i];
            chosen = cands[rng() % ncands];
        }

        int nr = r + DR[chosen], nc = c + DC[chosen];
        vis[nr*N+nc] = true;
        for (int di = 0; di < 4; di++) {
            int xr = nr + DR[di], xc = nc + DC[di];
            if (xr >= 0 && xr < N && xc >= 0 && xc < N && !vis[xr*N+xc]) w[xr*N+xc]--;
        }
        if (rfood[nr][nc] > 0) food_count++;
        path.push_back({nr, nc});
    }
    return path;
}

// Beam search version (kept for reference, not used in main loop due to speed tradeoff)
static const int MAX_BW = 16;

struct BeamState {
    bool vis[MAXN * MAXN];
    int w[MAXN * MAXN];
    int r, c;
    int food_count, matches;
    int tree_idx;
};
struct TreeNode { int parent; short r, c; };

static BeamState g_bs[2][MAX_BW];
static TreeNode g_tree[MAXN * MAXN * MAX_BW + 1];

[[maybe_unused]]
static vector<pair<int,int>> beam_rebuild_path(
    int N, uint32_t seed,
    const bool walls[][MAXN],
    const int rfood[][MAXN],
    const int* tgt, int tgt_off, int M,
    int beam_width,
    pair<int,int> start)
{
    const int NN = N * N;
    int nwalls = 0;
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            if (walls[r][c]) nwalls++;
    const int target_size = NN - nwalls + 1;

    auto rng = [&]() -> uint32_t {
        seed ^= seed<<13; seed ^= seed>>17; seed ^= seed<<5;
        return seed;
    };

    // Init base vis & w from walls
    bool base_vis[MAXN * MAXN];
    int base_w[MAXN * MAXN];
    memset(base_vis, 0, NN);
    memset(base_w, 0, NN * sizeof(int));
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            if (walls[r][c]) base_vis[r*N+c] = true;
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++) {
            if (base_vis[r*N+c]) continue;
            int cnt = 0;
            for (int di = 0; di < 4; di++) {
                int nr = r+DR[di], nc = c+DC[di];
                if (nr>=0 && nr<N && nc>=0 && nc<N && !base_vis[nr*N+nc]) cnt++;
            }
            base_w[r*N+c] = cnt;
        }

    // First beam: start position (already visited as wall)
    int tree_sz = 0;
    {
        auto& b = g_bs[0][0];
        memcpy(b.vis, base_vis, NN);
        memcpy(b.w, base_w, NN * sizeof(int));
        b.r = start.first; b.c = start.second;
        b.food_count = 0; b.matches = 0;
        g_tree[0] = {-1, (short)start.first, (short)start.second};
        b.tree_idx = 0;
        tree_sz = 1;
    }

    int cur = 0, nbeams = 1;
    if (beam_width > MAX_BW) beam_width = MAX_BW;

    for (int step = 1; step < target_size; step++) {
        struct Cand { int bi, di, score; };
        Cand cands[MAX_BW * 4];
        int ncands = 0;
        bool last_step = (step == target_size - 1);

        for (int bi = 0; bi < nbeams; bi++) {
            auto& b = g_bs[cur][bi];
            // Compute per-beam min_w for Warnsdorff constraint
            int min_w = INT_MAX;
            for (int di = 0; di < 4; di++) {
                int nr = b.r + DR[di], nc = b.c + DC[di];
                if (nr < 0 || nr >= N || nc < 0 || nc >= N || b.vis[nr*N+nc]) continue;
                min_w = min(min_w, b.w[nr*N+nc]);
            }
            for (int di = 0; di < 4; di++) {
                int nr = b.r + DR[di], nc = b.c + DC[di];
                if (nr < 0 || nr >= N || nc < 0 || nc >= N || b.vis[nr*N+nc]) continue;

                int wdeg = b.w[nr*N+nc];
                if (wdeg == 0 && !last_step) continue;
                // Strict Warnsdorff: only allow within +1 of min degree
                if (wdeg > min_w + 1 && !last_step) continue;

                int mb = 0;
                if (rfood[nr][nc] > 0) {
                    int ct = (tgt_off + b.food_count < M) ? tgt[tgt_off + b.food_count] : 0;
                    if (rfood[nr][nc] == ct) mb = 1;
                }

                // Score: Warnsdorff safety first, color match bonus, accumulated matches
                int score = -wdeg * 1000 + mb * 100 + b.matches * 10 + (int)(rng() % 8);
                cands[ncands++] = {bi, di, score};
            }
        }

        if (ncands == 0) return {};

        // Sort all candidates by score
        for (int i = 0; i < ncands-1; i++)
            for (int j = i+1; j < ncands; j++)
                if (cands[j].score > cands[i].score) swap(cands[i], cands[j]);

        // Select with diversity: max 2 candidates per parent beam
        Cand selected[MAX_BW];
        int nsel = 0;
        int per_beam[MAX_BW] = {};
        for (int i = 0; i < ncands && nsel < beam_width; i++) {
            if (per_beam[cands[i].bi] < 2) {
                selected[nsel++] = cands[i];
                per_beam[cands[i].bi]++;
            }
        }

        int nxt = 1 - cur;
        int new_nbeams = 0;

        for (int i = 0; i < nsel; i++) {
            auto& cd = selected[i];
            auto& src = g_bs[cur][cd.bi];
            auto& dst = g_bs[nxt][new_nbeams];

            int nr = src.r + DR[cd.di], nc = src.c + DC[cd.di];

            memcpy(dst.vis, src.vis, NN);
            memcpy(dst.w, src.w, NN * sizeof(int));

            dst.vis[nr*N+nc] = true;
            for (int di = 0; di < 4; di++) {
                int xr = nr + DR[di], xc = nc + DC[di];
                if (xr >= 0 && xr < N && xc >= 0 && xc < N && !dst.vis[xr*N+xc])
                    dst.w[xr*N+xc]--;
            }

            dst.r = nr; dst.c = nc;
            dst.food_count = src.food_count;
            dst.matches = src.matches;

            if (rfood[nr][nc] > 0) {
                int ct = (tgt_off + dst.food_count < M) ? tgt[tgt_off + dst.food_count] : 0;
                if (rfood[nr][nc] == ct) dst.matches++;
                dst.food_count++;
            }

            g_tree[tree_sz] = {src.tree_idx, (short)nr, (short)nc};
            dst.tree_idx = tree_sz;
            tree_sz++;
            new_nbeams++;
        }

        cur = nxt;
        nbeams = new_nbeams;
    }

    // Pick beam with most matches
    int best_bi = 0;
    for (int bi = 1; bi < nbeams; bi++)
        if (g_bs[cur][bi].matches > g_bs[cur][best_bi].matches)
            best_bi = bi;

    // Reconstruct path from tree
    vector<pair<int,int>> path(target_size);
    int idx = g_bs[cur][best_bi].tree_idx;
    for (int i = target_size - 1; i >= 0; i--) {
        path[i] = {g_tree[idx].r, g_tree[idx].c};
        idx = g_tree[idx].parent;
    }
    return path;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    struct FertileProfile {
        int prefix_lim;
        int e_w;
        int prefix_w;
        int head_w;
        int bite_w;
        int depth_w;
        int t_div;
    };
    static const FertileProfile FPROF[] = {
        {16, 64, 320, 64, 128, 4, 8},
        {16, 64, 352, 72, 144, 4, 16},
        {16, 64, 320, 64, 112, 5, 8},
        {16, 64, 288, 64, 160, 5, 8},
        {18, 64, 320, 64, 128, 4, 8},
        {14, 64, 352, 80, 144, 5, 8},
    };
    int fprof_idx = 2;
    if (const char* env = getenv("AHC063_PROFILE")) {
        int v = atoi(env);
        if (0 <= v && v < (int)(sizeof(FPROF) / sizeof(FPROF[0]))) fprof_idx = v;
    }
    const auto& fprof = FPROF[fprof_idx];

    int N, M, C;
    cin >> N >> M >> C;

    static int tgt[MAXN * MAXN];
    for (int i = 0; i < M; i++) cin >> tgt[i];

    static int food[MAXN][MAXN], food_orig[MAXN][MAXN];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) { cin >> food[i][j]; food_orig[i][j] = food[i][j]; }

    auto t0 = chrono::steady_clock::now();
    auto elapsed_ms = [&]() {
        return chrono::duration_cast<chrono::milliseconds>(
            chrono::steady_clock::now() - t0).count();
    };

    const int NN = N * N;

    // Phase 1: build Hamiltonian path guided by target colors
    vector<pair<int,int>> best_path = build_path(N, food_orig, tgt, M, 900, elapsed_ms);

    // Follow path to eat all food
    int ph1_head = 4;
    {
        int eaten = 0;
        const int need = M - 5;
        while (eaten < need && ph1_head + 1 < NN) {
            auto [pr, pc] = best_path[ph1_head];
            auto [nr, nc] = best_path[ph1_head + 1];
            cout << dir_of(nr - pr, nc - pc) << '\n';
            if (food[nr][nc] > 0) { food[nr][nc] = 0; ++eaten; }
            ++ph1_head;
        }
    }

    // Build snake color sequence
    static int sc2[MAXN * MAXN];
    for (int i = 0; i < 5; i++) sc2[i] = 1;
    {
        int fi = 0;
        for (int idx = 5; idx <= ph1_head && 5 + fi < M; idx++) {
            auto [r, c] = best_path[idx];
            if (food_orig[r][c] > 0) sc2[5 + fi++] = food_orig[r][c];
        }
    }
    int phase1_E = 0;
    for (int i = 0; i < M; i++) if (sc2[i] != tgt[i]) phase1_E++;
    if (phase1_E == 0) return 0;

    // State A: Phase 1 end position
    static pair<int,int> sp2a[MAXN * MAXN];
    for (int i = 0; i < M; i++) sp2a[i] = best_path[ph1_head - i];

    // State B: walked to end of Hamiltonian path
    static pair<int,int> sp2b[MAXN * MAXN];
    static int sc2b[MAXN * MAXN];
    int walk_T = 0;
    string walk_str;
    {
        int end_head = NN - 1;
        walk_T = end_head - ph1_head;
        for (int i = 0; i < M; i++) sp2b[i] = best_path[end_head - i];
        for (int i = 0; i < M; i++) sc2b[i] = sc2[i];
        walk_str.reserve(walk_T * 2);
        for (int i = ph1_head; i < end_head; i++) {
            auto [pr, pc] = best_path[i];
            auto [nr, nc] = best_path[i + 1];
            walk_str += dir_of(nr - pr, nc - pc);
            walk_str += '\n';
        }
    }

    // Phase 2: iterative bite + rebuild
    int best_p2_score = phase1_E * 10000;
    string best_p2_output;
    bool has_fertile = false;
    static pair<int,int> fertile_pos[MAXN * MAXN];
    static int fertile_col[MAXN * MAXN];
    int fertile_T = 0;
    string fertile_output;
    long long fertile_key = (long long)4e18;
    int fertile_E = INT_MAX;

    auto eval_state_E = [&](const int* col) -> int {
        int E = 0;
        for (int i = 0; i < M; i++) if (col[i] != tgt[i]) E++;
        return E;
    };
    auto state_metrics = [&](const pair<int,int>* pos, const int* col, int T) -> tuple<long long,int,int> {
        int E = eval_state_E(col);
        int potential = estimate_bite_potential(N, M, C, pos, col, tgt);

        int prefix_lim = min(M, fprof.prefix_lim);
        int prefix_E = 0;
        for (int i = 0; i < prefix_lim; i++) if (col[i] != tgt[i]) prefix_E++;

        int bidx[MAXN][MAXN];
        memset(bidx, -1, sizeof(bidx));
        for (int i = 0; i < M; i++) bidx[pos[i].first][pos[i].second] = i;

        auto [hr, hc] = pos[0];
        int bite_cnt = 0;
        int head_free = 0;
        int best_bh = -1;
        for (int d = 0; d < 4; d++) {
            int ar = hr + DR[d], ac = hc + DC[d];
            if (ar < 0 || ar >= N || ac < 0 || ac >= N) continue;
            if (ar == pos[1].first && ac == pos[1].second) continue;
            int idx = bidx[ar][ac];
            if (idx == -1 || idx == M - 1) head_free++;
            if (idx >= 2 && idx < M - 2) {
                bite_cnt++;
                best_bh = max(best_bh, idx);
            }
        }

        long long key = (long long)potential
                      + (long long)E * fprof.e_w
                      + (long long)prefix_E * fprof.prefix_w
                      + (long long)(4 - head_free) * fprof.head_w
                      - (long long)bite_cnt * fprof.bite_w
                      - (long long)max(best_bh, 0) * fprof.depth_w
                      + T / fprof.t_div;
        return {key, E, potential};
    };
    auto consider_fertile = [&](const pair<int,int>* pos, const int* col, int T, const string& out) {
        auto [key, E, potential] = state_metrics(pos, col, T);
        (void)potential;
        if (!has_fertile || key < fertile_key || (key == fertile_key && E < fertile_E)) {
            has_fertile = true;
            fertile_key = key;
            fertile_E = E;
            fertile_T = T;
            copy(pos, pos + M, fertile_pos);
            memcpy(fertile_col, col, M * sizeof(int));
            fertile_output = out;
        }
    };
    auto consider_best = [&](const int* col, int T, const string& out) {
        int E = eval_state_E(col);
        int score = T + 10000 * E;
        if (score < best_p2_score) {
            best_p2_score = score;
            best_p2_output = out;
        }
    };
    consider_fertile(sp2a, sc2, 0, "");
    consider_fertile(sp2b, sc2b, walk_T, walk_str);
    auto try_initial_stir = [&](pair<int,int>* base_pos, int* base_col, int base_T,
                                const string& base_out, uint32_t seed0) {
        pair<int,int> pos[MAXN * MAXN];
        int col[MAXN * MAXN];
        copy(base_pos, base_pos + M, pos);
        memcpy(col, base_col, M * sizeof(int));
        string out = base_out;
        int T = base_T;

        auto rng = [&]() -> uint32_t {
            seed0 ^= seed0 << 13;
            seed0 ^= seed0 >> 17;
            seed0 ^= seed0 << 5;
            return seed0;
        };

        int walk_len = 24 + (rng() % 40);
        for (int step = 0; step < walk_len; step++) {
            auto [hr, hc] = pos[0];
            int cand_d[4], ncand = 0;
            for (int d = 0; d < 4; d++) {
                int nr = hr + DR[d], nc = hc + DC[d];
                if (nr < 0 || nr >= N || nc < 0 || nc >= N) continue;
                if (nr == pos[1].first && nc == pos[1].second) continue;

                bool bite = false;
                for (int i = 0; i < M - 1; i++) {
                    if (pos[i].first == nr && pos[i].second == nc) {
                        bite = true;
                        break;
                    }
                }
                if (bite) continue;
                cand_d[ncand++] = d;
            }
            if (ncand == 0) break;

            int d = cand_d[rng() % ncand];
            int nr = hr + DR[d], nc = hc + DC[d];
            out += dir_of(DR[d], DC[d]);
            out += '\n';
            for (int i = M - 1; i >= 1; i--) pos[i] = pos[i - 1];
            pos[0] = {nr, nc};
            T++;
        }

        int bidx[MAXN][MAXN];
        memset(bidx, -1, sizeof(bidx));
        for (int i = 0; i < M; i++) bidx[pos[i].first][pos[i].second] = i;
        auto [hr, hc] = pos[0];
        int cand_bh[4], cand_dr[4], cand_dc[4], cand_score[4], nbc = 0;
        for (int d = 0; d < 4; d++) {
            int ar = hr + DR[d], ac = hc + DC[d];
            if (ar < 0 || ar >= N || ac < 0 || ac >= N) continue;
            if (ar == pos[1].first && ac == pos[1].second) continue;
            int idx = bidx[ar][ac];
            if (idx >= 2 && idx < M - 2) {
                cand_bh[nbc] = idx;
                cand_dr[nbc] = DR[d];
                cand_dc[nbc] = DC[d];
                int fixed_E = 0;
                int prefix_lim = min(idx + 2, min(M, fprof.prefix_lim));
                for (int i = 0; i < prefix_lim; i++) if (col[i] != tgt[i]) fixed_E++;
                cand_score[nbc] = fixed_E * 512 - idx * 8;
                nbc++;
            }
        }
        if (nbc == 0) return;

        int bi = 0;
        for (int i = 1; i < nbc; i++) {
            if (cand_score[i] < cand_score[bi] ||
                (cand_score[i] == cand_score[bi] && cand_bh[i] > cand_bh[bi])) bi = i;
        }
        int bh = cand_bh[bi];

        bool walls[MAXN][MAXN];
        memset(walls, 0, sizeof(walls));
        for (int i = 0; i <= bh; i++) walls[pos[i].first][pos[i].second] = true;

        int rfood[MAXN][MAXN];
        memset(rfood, 0, sizeof(rfood));
        for (int i = bh + 1; i <= M - 2; i++) rfood[pos[i].first][pos[i].second] = col[i + 1];

        int rlx_opts[] = {0, 1, 1, 1, 2, 2};
        int rlx = rlx_opts[rng() % 6];
        auto rp = rebuild_path(N, rng(), walls, rfood, tgt, bh + 2, M, rlx, pos[bh]);
        if (rp.empty()) return;

        out += dir_of(cand_dr[bi], cand_dc[bi]);
        out += '\n';

        int new_col[MAXN * MAXN];
        for (int i = 0; i <= bh + 1; i++) new_col[i] = col[i];
        int ci = bh + 2;

        deque<pair<int,int>> dq;
        dq.push_back(pos[bh]);
        for (int i = 0; i <= bh; i++) dq.push_back(pos[i]);
        for (int i = 1; i < (int)rp.size(); i++) {
            auto [pr, pc] = rp[i - 1];
            auto [nr, nc] = rp[i];
            out += dir_of(nr - pr, nc - pc);
            out += '\n';
            if (rfood[nr][nc] > 0) {
                dq.push_front({nr, nc});
                new_col[ci++] = rfood[nr][nc];
            } else {
                dq.push_front({nr, nc});
                dq.pop_back();
            }
        }
        if (ci != M) return;

        pair<int,int> new_pos[MAXN * MAXN];
        for (int i = 0; i < M; i++) new_pos[i] = dq[i];
        T += 1 + (int)rp.size();
        consider_fertile(new_pos, new_col, T, out);
        consider_best(new_col, T, out);
    };

    if (elapsed_ms() < 1150) {
        for (int t = 0; t < 3 && elapsed_ms() < 1200; t++) {
            try_initial_stir(sp2a, sc2, 0, "", 123456789u + t * 977u);
            try_initial_stir(sp2b, sc2b, walk_T, walk_str, 362436069u + t * 1237u);
        }
    }

    uint32_t mseed = 1;
#ifdef LOCAL
    int total_trials = 0, total_cycles = 0;
#endif
    while (elapsed_ms() < 1950) {
        struct InitState {
            pair<int,int>* pos;
            int* col;
            int prefix_T;
            const string* out;
            long long key;
        };
        InitState cand_init[3];
        int n_init = 0;
        auto [key_a, _ea, _pa] = state_metrics(sp2a, sc2, 0);
        cand_init[n_init++] = {sp2a, sc2, 0, nullptr, key_a};
        auto [key_b, _eb, _pb] = state_metrics(sp2b, sc2b, walk_T);
        cand_init[n_init++] = {sp2b, sc2b, walk_T, &walk_str, key_b};
        if (has_fertile) cand_init[n_init++] = {fertile_pos, fertile_col, fertile_T, &fertile_output, fertile_key};

        for (int i = 0; i < n_init - 1; i++)
            for (int j = i + 1; j < n_init; j++)
                if (cand_init[j].key < cand_init[i].key)
                    swap(cand_init[i], cand_init[j]);

        int pick = 0;
        if (n_init >= 2) {
            int weights[3] = {};
            long long base_key = cand_init[0].key;
            for (int i = 0; i < n_init; i++) {
                long long gap = cand_init[i].key - base_key;
                int w = 1;
                if (gap <= 0) w = 16;
                else if (gap <= 128) w = 12;
                else if (gap <= 512) w = 8;
                else if (gap <= 2048) w = 4;
                else w = 2;
                if (i == 0) w += 8;
                weights[i] = w;
            }
            int total_w = 0;
            for (int i = 0; i < n_init; i++) total_w += weights[i];
            uint32_t rv = mseed % total_w;
            for (int i = 0; i < n_init; i++) {
                if ((int)rv < weights[i]) {
                    pick = i;
                    break;
                }
                rv -= weights[i];
            }
        }
        pair<int,int> stp[MAXN * MAXN];
        int stc[MAXN * MAXN];
        int cur_k = M;
        int board_food[MAXN][MAXN];
        int board_food_cnt = 0;
        int bidx[MAXN][MAXN]; memset(bidx, -1, sizeof(bidx));

        string tbuf;
        tbuf.reserve(200000);
        copy(cand_init[pick].pos, cand_init[pick].pos + M, stp);
        memcpy(stc, cand_init[pick].col, M * sizeof(int));
        memset(board_food, 0, sizeof(board_food));
        for (int i = 0; i < cur_k; i++) bidx[stp[i].first][stp[i].second] = i;
        if (cand_init[pick].out != nullptr) tbuf = *cand_init[pick].out;
        int tT = cand_init[pick].prefix_T;
        uint32_t tseed = mseed * 2654435761u;
        auto rng = [&]() -> uint32_t {
            tseed ^= tseed << 13; tseed ^= tseed >> 17; tseed ^= tseed << 5;
            return tseed;
        };
        struct PartialProfile {
            int miss_w;
            int max_stall;
            int collect_dist;
            int collect_food_cnt;
        };
        static const PartialProfile pprof_tbl[] = {
            {1, 4, 4, 2}, // p0 current
            {1, 5, 4, 2}, // p1 longer stall
            {2, 4, 4, 2}, // p2 heavier missing penalty
            {1, 6, 5, 3}, // p3 looser collect and stall
            {2, 5, 5, 3}, // p4 heavier + looser
            {1, 3, 3, 1}, // p5 tighter
        };
        int partial_profile_id = 0;
        if (const char* p = getenv("AHC063_PARTIAL_PROFILE")) {
            partial_profile_id = max(0, min((int)(size(pprof_tbl) - 1), atoi(p)));
        }
        const auto& pprof = pprof_tbl[partial_profile_id];
        auto eval_partial_state = [&]() -> int {
            int E = 0;
            for (int i = 0; i < cur_k; i++) if (stc[i] != tgt[i]) E++;
            return tT + 10000 * (E + pprof.miss_w * (M - cur_k));
        };
        auto good_prefix_len = [&]() -> int {
            int len = 0;
            while (len < cur_k && stc[len] == tgt[len]) len++;
            return len;
        };
        auto rebuild_bidx = [&]() {
            memset(bidx, -1, sizeof(bidx));
            for (int i = 0; i < cur_k; i++) bidx[stp[i].first][stp[i].second] = i;
        };
        auto nearest_food_dist = [&](int sr, int sc) -> int {
            if (board_food_cnt == 0) return 1000;
            int best = 1000;
            for (int r = 0; r < N; r++) {
                for (int c = 0; c < N; c++) {
                    if (board_food[r][c] == 0) continue;
                    best = min(best, abs(sr - r) + abs(sc - c));
                }
            }
            return best;
        };
        auto move_metrics = [&](int nr, int nc) -> pair<int,int> {
            pair<int,int> next_pos[MAXN * MAXN];
            next_pos[0] = {nr, nc};
            for (int i = 1; i < cur_k; i++) next_pos[i] = stp[i - 1];

            int next_k = cur_k;
            int bite_h = -1;
            for (int h = 1; h < cur_k - 1; h++) {
                if (next_pos[h] == next_pos[0]) {
                    bite_h = h;
                    break;
                }
            }
            if (bite_h != -1) next_k = bite_h + 1;

            int head_free = 0;
            int bite_cnt = 0;
            auto [hr3, hc3] = next_pos[0];
            for (int d2 = 0; d2 < 4; d2++) {
                int ar = hr3 + DR[d2], ac = hc3 + DC[d2];
                if (ar < 0 || ar >= N || ac < 0 || ac >= N) continue;
                if (next_k >= 2 && ar == next_pos[1].first && ac == next_pos[1].second) continue;
                int idx = -1;
                for (int i = 0; i < next_k; i++) {
                    if (next_pos[i].first == ar && next_pos[i].second == ac) {
                        idx = i;
                        break;
                    }
                }
                if (idx == -1 || idx == next_k - 1) head_free++;
                if (idx >= 2 && idx < next_k - 2) bite_cnt++;
            }
            return {head_free, bite_cnt};
        };
        auto try_walk_continue = [&](bool collect_mode) -> bool {
            auto [hr2, hc2] = stp[0];
            int cand_d[4], cand_pri[4], cand_aux[4], ncand = 0;
            for (int d = 0; d < 4; d++) {
                int nr = hr2 + DR[d], nc = hc2 + DC[d];
                if (nr < 0 || nr >= N || nc < 0 || nc >= N) continue;
                if (cur_k >= 2 && nr == stp[1].first && nc == stp[1].second) continue;
                int idx = bidx[nr][nc];
                int pri = 10;
                int aux = 0;
                if (board_food[nr][nc] > 0 && idx == -1) {
                    pri = 0;
                    aux = 0;
                } else if (cur_k < M && board_food_cnt > 0) {
                    auto [head_free, bite_cnt] = move_metrics(nr, nc);
                    int nfd = nearest_food_dist(nr, nc);
                    if (collect_mode) {
                        if (idx == -1 || idx == cur_k - 1) {
                            pri = 16 + nfd * 2 - head_free * 3 - bite_cnt * 2;
                            aux = (idx == cur_k - 1 ? 4 : 0) - head_free * 8 - bite_cnt * 4;
                        } else if (idx >= 2 && idx < cur_k - 1) {
                            pri = 24 + nfd * 2 - head_free * 2 - bite_cnt;
                            aux = -idx * 4 - head_free * 6 - bite_cnt * 3;
                        } else continue;
                    } else if (idx >= 2 && idx < cur_k - 1) {
                        pri = 8 + nfd - head_free * 3 - bite_cnt * 5;
                        aux = -idx * 6 - head_free * 4 - bite_cnt * 6;
                    } else if (idx == -1 || idx == cur_k - 1) {
                        pri = 14 + nfd * 2 - head_free * 2 - bite_cnt * 3;
                        aux = (idx == cur_k - 1 ? 4 : 0) - head_free * 4 - bite_cnt * 4;
                    } else continue;
                } else if (idx >= 2 && idx < cur_k - 1) {
                    pri = 1;
                    aux = -idx;
                } else if (idx == -1 || idx == cur_k - 1) {
                    pri = 2;
                    aux = (idx == cur_k - 1 ? 1 : 0);
                }
                else continue;
                cand_d[ncand] = d;
                cand_pri[ncand] = pri;
                cand_aux[ncand] = aux;
                ncand++;
            }
            if (ncand == 0) return false;
            for (int i = 0; i < ncand - 1; i++) {
                for (int j = i + 1; j < ncand; j++) {
                    if (cand_pri[j] < cand_pri[i] ||
                        (cand_pri[j] == cand_pri[i] && cand_aux[j] < cand_aux[i])) {
                        swap(cand_d[i], cand_d[j]);
                        swap(cand_pri[i], cand_pri[j]);
                        swap(cand_aux[i], cand_aux[j]);
                    }
                }
            }
            int lim = 1;
            if (cur_k < M && board_food_cnt > 0) lim = min(ncand, 2);
            int d = cand_d[rng() % lim];

            int nr = hr2 + DR[d], nc = hc2 + DC[d];
            tbuf += dir_of(DR[d], DC[d]);
            tbuf += '\n';
            tT++;

            if (board_food[nr][nc] > 0) {
                int food_col = board_food[nr][nc];
                board_food[nr][nc] = 0;
                board_food_cnt--;
                for (int i = cur_k; i >= 1; i--) stp[i] = stp[i - 1];
                stp[0] = {nr, nc};
                stc[cur_k] = food_col;
                cur_k++;
                rebuild_bidx();
                return true;
            }

            pair<int,int> next_pos[MAXN * MAXN];
            next_pos[0] = {nr, nc};
            for (int i = 1; i < cur_k; i++) next_pos[i] = stp[i - 1];

            int bite_h = -1;
            for (int h = 1; h < cur_k - 1; h++) {
                if (next_pos[h] == next_pos[0]) {
                    bite_h = h;
                    break;
                }
            }
            if (bite_h != -1) {
                for (int p = bite_h + 1; p < cur_k; p++) {
                    if (board_food[next_pos[p].first][next_pos[p].second] == 0) board_food_cnt++;
                    board_food[next_pos[p].first][next_pos[p].second] = stc[p];
                }
                cur_k = bite_h + 1;
            }
            for (int i = 0; i < cur_k; i++) stp[i] = next_pos[i];
            rebuild_bidx();
            return true;
        };
        int local_best_eval = eval_partial_state();
        int stall_count = 0;
        const int max_stall = pprof.max_stall;

        for (;;) {
            if (tT + NN + 10 > 99900) break;
            if (elapsed_ms() > 1950) break;
            if (stall_count >= max_stall) break;

            auto [hr, hc] = stp[0];
            int preserve_pref = (cur_k < M) ? good_prefix_len() : 0;
            int best_adj_bh = -1;
            for (int d = 0; d < 4; d++) {
                int ar = hr + DR[d], ac = hc + DC[d];
                if (ar < 0 || ar >= N || ac < 0 || ac >= N) continue;
                if (ar == stp[1].first && ac == stp[1].second) continue;
                int idx = bidx[ar][ac];
                if (idx >= 2 && idx < cur_k - 2 && (preserve_pref == 0 || idx + 2 >= preserve_pref))
                    best_adj_bh = max(best_adj_bh, idx);
            }
            bool prefer_collect = (cur_k < M && board_food_cnt > 0 &&
                (nearest_food_dist(hr, hc) <= pprof.collect_dist || board_food_cnt <= pprof.collect_food_cnt));
            if (prefer_collect && best_adj_bh < max(2, cur_k - 6)) {
                if (try_walk_continue(true)) {
                    int cur_eval = eval_partial_state();
                    if (cur_eval < local_best_eval) {
                        local_best_eval = cur_eval;
                        stall_count = 0;
                    } else {
                        stall_count++;
                    }
                    continue;
                }
            }
            int bcbh[4], bcdr[4], bcdc[4], bcscore[4], nbc = 0;
            for (int d = 0; d < 4; d++) {
                int ar = hr+DR[d], ac = hc+DC[d];
                if (ar<0||ar>=N||ac<0||ac>=N) continue;
                if (ar==stp[1].first && ac==stp[1].second) continue;
                int idx = bidx[ar][ac];
                if (idx >= 2 && idx < cur_k - 2) {
                    if (preserve_pref > 0 && idx + 2 < preserve_pref) continue;
                    bcbh[nbc]=idx; bcdr[nbc]=DR[d]; bcdc[nbc]=DC[d];
                    int fixed_E = 0;
                    int prefix_lim = min(idx + 2, min(cur_k, fprof.prefix_lim));
                    for (int i = 0; i < prefix_lim; i++) if (stc[i] != tgt[i]) fixed_E++;
                    int food_dist_pen = 0;
                    if (cur_k < M && board_food_cnt > 0) {
                        food_dist_pen = min(12, nearest_food_dist(stp[idx].first, stp[idx].second));
                    }
                    // Keep deep bites preferred, but lightly penalize dirty fixed prefixes.
                    bcscore[nbc] = fixed_E * 512 + food_dist_pen * 24 - idx * 8;
                    nbc++;
                }
            }
            if (nbc == 0) {
                bool collect_mode = (cur_k < M && board_food_cnt > 0 &&
                    (nearest_food_dist(hr, hc) <= pprof.collect_dist || board_food_cnt <= pprof.collect_food_cnt));
                if (!try_walk_continue(collect_mode)) break;
                int cur_eval = eval_partial_state();
                if (cur_eval < local_best_eval) {
                    local_best_eval = cur_eval;
                    stall_count = 0;
                } else {
                    stall_count++;
                }
                continue;
            }

            // Sort by light prefix-aware bite score.
            for (int i = 0; i < nbc-1; i++)
                for (int j = i+1; j < nbc; j++)
                    if (bcscore[j] < bcscore[i] || (bcscore[j] == bcscore[i] && bcbh[j] > bcbh[i])) {
                        swap(bcscore[i],bcscore[j]);
                        swap(bcbh[i],bcbh[j]);
                        swap(bcdr[i],bcdr[j]);
                        swap(bcdc[i],bcdc[j]);
                    }

            bool ok = false;
            for (int attempt = 0; attempt < 20 && !ok; attempt++) {
                int bi;
                if (attempt < min(3, nbc)) bi = attempt;
                else if (rng() % 10 < 8) bi = rng() % min(2, nbc);
                else bi = rng() % nbc;
                int bh = bcbh[bi];

                bool walls[MAXN][MAXN]; memset(walls, 0, sizeof(walls));
                for (int i = 0; i <= bh; i++) walls[stp[i].first][stp[i].second] = true;

                int rfood[MAXN][MAXN];
                memcpy(rfood, board_food, sizeof(rfood));
                for (int i = bh+1; i <= cur_k-2; i++)
                    rfood[stp[i].first][stp[i].second] = stc[i+1];

                int rlx;
                if (cur_k < M || board_food_cnt > 0) {
                    int rlx_opts[] = {1, 1, 1, 2, 2, 3};
                    rlx = rlx_opts[rng() % 6];
                } else {
                    int rlx_opts[] = {0, 1, 1, 1, 2, 2};
                    rlx = rlx_opts[rng() % 6];
                }
                auto rp = rebuild_path(N, rng(), walls, rfood,
                                       tgt, bh+2, M, rlx, stp[bh]);
                if (rp.empty()) continue;

                ok = true;
                int cycle_T = (int)rp.size();

                tbuf += dir_of(bcdr[bi], bcdc[bi]);
                tbuf += '\n';
                int new_col[MAXN * MAXN];
                for (int i = 0; i <= bh+1; i++) new_col[i] = stc[i];
                int ci = bh + 2;

                deque<pair<int,int>> dq;
                dq.push_back(stp[bh]);
                for (int i = 0; i <= bh; i++) dq.push_back(stp[i]);

                for (int i = 1; i < (int)rp.size(); i++) {
                    auto [pr, pc] = rp[i-1];
                    auto [nr, nc] = rp[i];
                    tbuf += dir_of(nr-pr, nc-pc);
                    tbuf += '\n';
                    if (rfood[nr][nc] > 0) {
                        dq.push_front({nr, nc});
                        new_col[ci++] = rfood[nr][nc];
                    } else {
                        dq.push_front({nr, nc});
                        dq.pop_back();
                    }
                }

                memset(bidx, -1, sizeof(bidx));
                cur_k = M;
                memset(board_food, 0, sizeof(board_food));
                board_food_cnt = 0;
                for (int i = 0; i < cur_k; i++) {
                    stp[i] = dq[i];
                    stc[i] = new_col[i];
                    bidx[stp[i].first][stp[i].second] = i;
                }
                tT += cycle_T;
#ifdef LOCAL
                total_cycles++;
#endif

                int E = 0;
                for (int i = 0; i < M; i++) if (stc[i] != tgt[i]) E++;
                int score = tT + 10000 * E;
                if (score < best_p2_score) {
                    best_p2_score = score;
                    best_p2_output = tbuf;
                }
                consider_fertile(stp, stc, tT, tbuf);

                int cur_eval = eval_partial_state();
                if (cur_eval < local_best_eval) {
                    local_best_eval = cur_eval;
                    stall_count = 0;
                } else {
                    stall_count++;
                }
            }
            if (!ok) {
                int bi = 0;
                int bh = bcbh[bi];
                tbuf += dir_of(bcdr[bi], bcdc[bi]);
                tbuf += '\n';
                tT += 1;

                for (int i = bh + 1; i <= cur_k - 2; i++) {
                    if (board_food[stp[i].first][stp[i].second] == 0) board_food_cnt++;
                    board_food[stp[i].first][stp[i].second] = stc[i + 1];
                }

                deque<pair<int,int>> dq;
                dq.push_back(stp[bh]);
                for (int i = 0; i <= bh; i++) dq.push_back(stp[i]);
                cur_k = bh + 2;

                memset(bidx, -1, sizeof(bidx));
                for (int i = 0; i < cur_k; i++) {
                    stp[i] = dq[i];
                    bidx[stp[i].first][stp[i].second] = i;
                }

                int cur_eval = eval_partial_state();
                if (cur_eval < local_best_eval) {
                    local_best_eval = cur_eval;
                    stall_count = 0;
                } else {
                    stall_count++;
                }
                continue;
            }
        }
        mseed++;
#ifdef LOCAL
        total_trials++;
#endif
    }

    if (!best_p2_output.empty()) {
        cout << best_p2_output;
#ifdef LOCAL
        fprintf(stderr, "P2: ph1_E=%d -> p2_score=%d (saved %d) trials=%d cycles=%d\n",
                phase1_E, best_p2_score, phase1_E*10000 - best_p2_score, total_trials, total_cycles);
#endif
    }
#ifdef LOCAL
    else {
        fprintf(stderr, "P2: no improvement (ph1_E=%d, trials=%d, cycles=%d)\n",
                phase1_E, total_trials, total_cycles);
    }
#endif

    return 0;
}
