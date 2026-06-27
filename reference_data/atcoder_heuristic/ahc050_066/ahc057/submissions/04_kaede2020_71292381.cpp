#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <random>
#include <chrono>
#include <tuple>
#include <cstring>

using namespace std;

// --- Constants ---
constexpr int N = 300;
constexpr int T_MAX = 1000;
constexpr int M = 10;
constexpr int K = 30;
constexpr int L = 100000;
constexpr int L_HALF = 50000;

// --- Structs ---
struct Point {
    double x, y;
};

struct MergeEvent {
    int t;
    int u, v; // Global indices for output
    double cost;
    // For neighborhood search information
    vector<int> small_side; 
    vector<int> large_side; 
};

struct GroupResult {
    double total_cost;
    vector<MergeEvent> events;
};

// --- Globals ---
Point initial_pos[N];
pair<int,int> initial_vel[N];

// State
int assignment[N];
vector<int> group_members[M];
GroupResult group_results[M];
double global_total_cost = 0;

// Trajectory history: [Time][AtomIndex]
// Updated only when a move is accepted to speed up
Point pos_history[T_MAX][N];

// --- Helper Functions ---

// Fast distance squared on torus
inline double get_dist_sq(const Point& p1, const Point& p2) {
    double dx = std::abs(p1.x - p2.x);
    if (dx > L_HALF) dx = L - dx;
    double dy = std::abs(p1.y - p2.y);
    if (dy > L_HALF) dy = L - dy;
    return dx*dx + dy*dy;
}

// Coordinate normalization
inline double norm_coord(double c) {
    if (c >= L) c -= L;
    else if (c < 0) c += L;
    return c;
}

// Predict position (optimized)
inline Point predict(const Point& p, double vx, double vy, int dt) {
    double nx = p.x + vx * dt;
    double ny = p.y + vy * dt;
    // Handle wrapping (loop is faster than fmod for small overshoots)
    while(nx >= L) nx -= L; while(nx < 0) nx += L;
    while(ny >= L) ny -= L; while(ny < 0) ny += L;
    return {nx, ny};
}

// Initial solution generation helpers
double unwrap(double ref, double target) {
    double d = target - ref;
    if (d > L / 2.0) target -= L;
    else if (d < -L / 2.0) target += L;
    return target;
}

Point get_isolated_pos(int i, int t) {
    return predict(initial_pos[i], initial_vel[i].first, initial_vel[i].second, t);
}

// --- Core Simulation Logic ---

// Simulation state for a component within a group
struct LocalComp {
    double vx, vy;
    double weight;
    vector<int> mems; // Stores local indices (0..member_count-1)
};

// Solve for a single group
// members: list of global atom indices
// result: output cost and events
// update_history: if true, writes to global pos_history
void solve_group(int gid, const vector<int>& members, GroupResult& result, bool update_history) {
    result.total_cost = 0;
    result.events.clear();
    int m_size = (int)members.size();
    if (m_size == 0) return;

    // Static buffers to avoid allocation
    static LocalComp comps[N]; 
    static Point curr_pos[N]; 
    static int atom_comp_idx[N]; // local atom index -> local comp index

    // Initialize local state
    vector<int> active_comps;
    active_comps.reserve(m_size);

    for(int i=0; i<m_size; i++) {
        int u = members[i];
        comps[i].vx = (double)initial_vel[u].first;
        comps[i].vy = (double)initial_vel[u].second;
        comps[i].weight = 1.0;
        comps[i].mems.clear();
        comps[i].mems.push_back(i);
        
        curr_pos[i] = initial_pos[u];
        atom_comp_idx[i] = i;
        active_comps.push_back(i);
    }

    for(int t=0; t<T_MAX; t++) {
        // Update history if requested
        if(update_history) {
            for(int i=0; i<m_size; i++) {
                pos_history[t][members[i]] = curr_pos[i];
            }
        }

        // Merge Phase
        if(active_comps.size() > 1) {
            int needed = (int)active_comps.size() - 1;
            int time_left = T_MAX - t;
            bool panic = (time_left < needed * 2 + 10);

            double min_d2 = 1e18;
            int best_ci = -1, best_cj = -1; // indices in comps array
            int best_ui = -1, best_vi = -1; // local atom indices
            int best_ac_idx_i = -1, best_ac_idx_j = -1; // indices in active_comps

            // Find closest pair of components
            for(int i=0; i<(int)active_comps.size(); i++) {
                int ci = active_comps[i];
                for(int j=i+1; j<(int)active_comps.size(); j++) {
                    int cj = active_comps[j];
                    
                    // Iterate over all atom pairs between components
                    for(int u : comps[ci].mems) {
                        for(int v : comps[cj].mems) {
                            double d2 = get_dist_sq(curr_pos[u], curr_pos[v]);
                            if(d2 < min_d2) {
                                min_d2 = d2;
                                best_ci = ci; best_cj = cj;
                                best_ui = u;  best_vi = v;
                                best_ac_idx_i = i; best_ac_idx_j = j;
                            }
                        }
                    }
                }
            }

            if(best_ci != -1) {
                double dist = std::sqrt(min_d2);
                bool do_merge = false;

                if(panic) {
                    do_merge = true;
                } else {
                    // Prediction logic
                    Point pu_next = predict(curr_pos[best_ui], comps[best_ci].vx, comps[best_ci].vy, 10);
                    Point pv_next = predict(curr_pos[best_vi], comps[best_cj].vx, comps[best_cj].vy, 10);
                    double next_d2 = get_dist_sq(pu_next, pv_next);
                    
                    // If distance is not decreasing significantly
                    if(next_d2 >= min_d2 - 1e-5) {
                        double th = 35000.0 + t * 10.0;
                        if(dist < th) do_merge = true;
                    }
                }

                if(do_merge) {
                    double cost_val = std::round(dist);
                    result.total_cost += cost_val;

                    // Create event
                    MergeEvent ev;
                    ev.t = t;
                    ev.u = members[best_ui];
                    ev.v = members[best_vi];
                    ev.cost = cost_val;

                    // Determine parent/child for physics and history
                    LocalComp *parent, *child;
                    int parent_idx, child_idx_in_active;

                    if(comps[best_ci].mems.size() < comps[best_cj].mems.size()) {
                        parent = &comps[best_cj];
                        child = &comps[best_ci];
                        parent_idx = best_cj;
                        child_idx_in_active = best_ac_idx_i;
                    } else {
                        parent = &comps[best_ci];
                        child = &comps[best_cj];
                        parent_idx = best_ci;
                        child_idx_in_active = best_ac_idx_j;
                    }

                    // Save members for neighborhood search
                    ev.small_side.reserve(child->mems.size());
                    for(int m : child->mems) ev.small_side.push_back(members[m]);
                    ev.large_side.reserve(parent->mems.size());
                    for(int m : parent->mems) ev.large_side.push_back(members[m]);
                    
                    result.events.push_back(ev);

                    // Update physics
                    double nvx = (parent->vx * parent->weight + child->vx * child->weight) / (parent->weight + child->weight);
                    double nvy = (parent->vy * parent->weight + child->vy * child->weight) / (parent->weight + child->weight);
                    parent->vx = nvx;
                    parent->vy = nvy;
                    parent->weight += child->weight;

                    // Merge members
                    for(int m : child->mems) {
                        parent->mems.push_back(m);
                        atom_comp_idx[m] = parent_idx;
                    }

                    // Remove child from active list
                    active_comps[child_idx_in_active] = active_comps.back();
                    active_comps.pop_back();
                }
            }
        }

        // Move Phase
        // Only update active components
        for(int idx : active_comps) {
            double vx = comps[idx].vx;
            double vy = comps[idx].vy;
            for(int m : comps[idx].mems) {
                double nx = curr_pos[m].x + vx;
                double ny = curr_pos[m].y + vy;
                if(nx >= L) nx -= L; else if(nx < 0) nx += L;
                if(ny >= L) ny -= L; else if(ny < 0) ny += L;
                curr_pos[m].x = nx;
                curr_pos[m].y = ny;
            }
        }
    }
}

long long calc_atcoder_score(double total_D) {
    if (total_D < 0) return 0;
    double val = (double)L * (N - M) / (total_D + 1.0);
    if (val <= 0) return 0;
    return (long long)std::round(1e6 * std::log2(val));
}

vector<int> generate_initial_assignment() {
    mt19937 rng(42);
    int PREDICT_T = 350;
    vector<Point> feature_pos(N);
    for(int i=0; i<N; i++) feature_pos[i] = get_isolated_pos(i, PREDICT_T);

    vector<Point> centroids(M);
    vector<int> p(N);
    for(int i=0; i<N; i++) p[i] = i;
    shuffle(p.begin(), p.end(), rng);
    for(int i=0; i<M; i++) centroids[i] = feature_pos[p[i]];

    vector<int> asgn(N);

    for(int iter=0; iter<15; iter++) {
        vector<vector<int>> clusters(M);
        for(int i=0; i<N; i++) {
            int best_c = 0;
            double min_d = 1e18;
            for(int c=0; c<M; c++) {
                double d = get_dist_sq(feature_pos[i], centroids[c]);
                if (d < min_d) { min_d = d; best_c = c; }
            }
            clusters[best_c].push_back(i);
        }
        for(int c=0; c<M; c++) {
            if (clusters[c].empty()) continue;
            double base_x = feature_pos[clusters[c][0]].x;
            double base_y = feature_pos[clusters[c][0]].y;
            double sum_x = 0, sum_y = 0;
            for(int idx : clusters[c]) {
                sum_x += unwrap(base_x, feature_pos[idx].x);
                sum_y += unwrap(base_y, feature_pos[idx].y);
            }
            double avg_x = sum_x / clusters[c].size();
            double avg_y = sum_y / clusters[c].size();
            avg_x = std::fmod(avg_x, (double)L); if(avg_x < 0) avg_x += L;
            avg_y = std::fmod(avg_y, (double)L); if(avg_y < 0) avg_y += L;
            centroids[c] = {avg_x, avg_y};
        }
    }

    struct Cand { double d; int u; int c; bool operator<(const Cand& o)const{return d < o.d;} };
    vector<Cand> cands;
    for(int i=0; i<N; i++) {
        for(int c=0; c<M; c++) {
            double d = get_dist_sq(feature_pos[i], centroids[c]);
            cands.push_back({d, i, c});
        }
    }
    sort(cands.begin(), cands.end());
    vector<int> cnt(M, 0);
    vector<bool> used(N, false);
    for(auto& x : cands) {
        if(!used[x.u] && cnt[x.c] < K) {
            asgn[x.u] = x.c;
            cnt[x.c]++;
            used[x.u] = true;
        }
    }
    return asgn;
}

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    int n_in, t_in, m_in, k_in, l_in;
    if(!(cin >> n_in >> t_in >> m_in >> k_in >> l_in)) return 0;
    for(int i=0; i<N; i++) {
        int x, y, vx, vy;
        cin >> x >> y >> vx >> vy;
        initial_pos[i] = {(double)x, (double)y};
        initial_vel[i] = {vx, vy};
    }

    // Initial Assignment
    vector<int> init_assignment = generate_initial_assignment();
    for(int i=0; i<N; i++) {
        assignment[i] = init_assignment[i];
        group_members[assignment[i]].push_back(i);
    }

    // Initial Evaluation
    global_total_cost = 0;
    for(int g=0; g<M; g++) {
        solve_group(g, group_members[g], group_results[g], true); // true to init history
        global_total_cost += group_results[g].total_cost;
    }

    long long current_score = calc_atcoder_score(global_total_cost);
    long long best_score = current_score;
    vector<int> best_assignment(assignment, assignment + N);

    // SA
    mt19937 rng(1234);
    auto start_clock = chrono::steady_clock::now();
    double time_limit = 1800.0;
    const double START_TEMP = 3000.0;
    const double END_TEMP = 10.0;

    int iter = 0;

    while(true) {
        iter++;
        if((iter & 31) == 0) {
            auto now = chrono::steady_clock::now();
            if(chrono::duration_cast<chrono::milliseconds>(now - start_clock).count() > time_limit) break;
        }

        // 1. Pick a group with bad merges
        vector<pair<double, int>> group_max_cost; 
        for(int g=0; g<M; g++) {
            double max_c = 0;
            for(auto& ev : group_results[g].events) {
                if(!ev.small_side.empty()) max_c = max(max_c, ev.cost);
            }
            if(max_c > 0) group_max_cost.push_back({max_c, g});
        }
        if(group_max_cost.empty()) break; 

        sort(group_max_cost.rbegin(), group_max_cost.rend());
        int pick = rng() % min((int)group_max_cost.size(), 5);
        int g_src = group_max_cost[pick].second;

        const auto& evs = group_results[g_src].events;
        vector<int> cand_events;
        for(int i=0; i<(int)evs.size(); i++) {
            if(!evs[i].small_side.empty()) cand_events.push_back(i);
        }
        if(cand_events.empty()) continue;
        
        int ev_idx = cand_events[rng() % cand_events.size()];
        for(int k=0; k<5; k++) {
            int idx = cand_events[rng() % cand_events.size()];
            if(evs[idx].cost > evs[ev_idx].cost) ev_idx = idx;
        }

        const auto& target_event = evs[ev_idx];
        int target_u = target_event.small_side[rng() % target_event.small_side.size()];

        // 2. Find best destination group
        int best_g_dst = -1;
        double min_affinity_dist = 1e18;

        for(int g=0; g<M; g++) {
            if(g == g_src) continue;
            double current_min = 1e18;
            
            for(int t=0; t<T_MAX; t+=100) {
                Point p_u = get_isolated_pos(target_u, t);
                for(int member : group_members[g]) {
                    double d2 = get_dist_sq(p_u, pos_history[t][member]);
                    if(d2 < current_min) current_min = d2;
                }
            }
            if(current_min < min_affinity_dist) {
                min_affinity_dist = current_min;
                best_g_dst = g;
            }
        }

        if(best_g_dst == -1) best_g_dst = (g_src + 1) % M;

        // 3. Pick victim from dst
        int target_v = -1;
        double max_cost_dst = -1;
        for(const auto& ev : group_results[best_g_dst].events) {
            if(ev.cost > max_cost_dst && !ev.small_side.empty()) {
                max_cost_dst = ev.cost;
                target_v = ev.small_side[rng() % ev.small_side.size()];
            }
        }
        if(target_v == -1) {
            target_v = group_members[best_g_dst][rng() % group_members[best_g_dst].size()];
        }

        // --- Execute Swap ---
        GroupResult old_res_src = group_results[g_src];
        GroupResult old_res_dst = group_results[best_g_dst];
        double old_global_cost = global_total_cost;

        auto it_u = find(group_members[g_src].begin(), group_members[g_src].end(), target_u);
        *it_u = target_v; 
        auto it_v = find(group_members[best_g_dst].begin(), group_members[best_g_dst].end(), target_v);
        *it_v = target_u;

        swap(assignment[target_u], assignment[target_v]);

        // Re-simulate (no history update yet)
        solve_group(g_src, group_members[g_src], group_results[g_src], false);
        solve_group(best_g_dst, group_members[best_g_dst], group_results[best_g_dst], false);

        double new_global_cost = old_global_cost 
                               - old_res_src.total_cost - old_res_dst.total_cost
                               + group_results[g_src].total_cost + group_results[best_g_dst].total_cost;

        double delta = new_global_cost - old_global_cost;
        
        bool accept = false;
        if(delta <= 0) accept = true;
        else {
            auto now = chrono::steady_clock::now();
            double elapsed = chrono::duration_cast<chrono::milliseconds>(now - start_clock).count();
            double progress = elapsed / time_limit;
            double temp = START_TEMP + (END_TEMP - START_TEMP) * progress;
            if(bernoulli_distribution(exp(-delta/temp))(rng)) accept = true;
        }

        if(accept) {
            global_total_cost = new_global_cost;
            if(calc_atcoder_score(global_total_cost) > best_score) {
                best_score = calc_atcoder_score(global_total_cost);
                for(int i=0; i<N; i++) best_assignment[i] = assignment[i];
            }
            // Update history on acceptance
            solve_group(g_src, group_members[g_src], group_results[g_src], true);
            solve_group(best_g_dst, group_members[best_g_dst], group_results[best_g_dst], true);
        } else {
            // Rollback
            group_results[g_src] = old_res_src;
            group_results[best_g_dst] = old_res_dst;
            swap(assignment[target_u], assignment[target_v]);
            *it_u = target_u;
            *it_v = target_v;
        }
    }

    // Final Output
    for(int i=0; i<N; i++) assignment[i] = best_assignment[i];
    for(int g=0; g<M; g++) group_members[g].clear();
    for(int i=0; i<N; i++) group_members[assignment[i]].push_back(i);
    
    vector<MergeEvent> all_events;
    for(int g=0; g<M; g++) {
        GroupResult res;
        solve_group(g, group_members[g], res, false);
        all_events.insert(all_events.end(), res.events.begin(), res.events.end());
    }
    
    sort(all_events.begin(), all_events.end(), [](const MergeEvent& a, const MergeEvent& b){
        return a.t < b.t;
    });

    for(const auto& ev : all_events) {
        cout << ev.t << " " << ev.u << " " << ev.v << "\n";
    }

    return 0;
}