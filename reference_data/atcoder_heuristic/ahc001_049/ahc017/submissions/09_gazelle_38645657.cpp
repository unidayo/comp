#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <climits>
#include <iomanip>
#include <iostream>
#include <queue>
#include <random>
using namespace std;
#define ALL(v) v.begin(), v.end()

constexpr long long INF = 1000000000;

static uint32_t xorshift(int N) {
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;

    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8))) % N;
}

constexpr int MAX_NUM_VERTICES = 1000, MAX_NUM_EDGES = 3000, MAX_NUM_DAYS = 30;
constexpr int MAX_NUM_SAMPLES = 55, NUM_OPS = 5;
constexpr double EXECUTION_DURATION_TIME = 5.9;
constexpr bool EXECUTE_LOCAL = false;
constexpr int MAX_NUM_ITERATION = 107399;
mt19937 mt;

using pi = pair<int, int>;
using Weight = int;

struct Timer {
    double total_time = 0;
    chrono::system_clock::time_point start_clock;

    void start() { start_clock = chrono::system_clock::now(); }
    void stop() {
        total_time += (chrono::duration_cast<chrono::microseconds>(
                           chrono::system_clock::now() - (*this).start_clock)
                           .count() *
                       1e-6);
    }
};
Timer timer;

struct Coordinate {
    int x;
    int y;

    Coordinate(int x, int y) : x(x), y(y) {}
    Coordinate() : x(0), y(0) {}

    static int square_distance(const Coordinate& p, const Coordinate& q) {
        return (p.x - q.x) * (p.x - q.x) + (p.y - q.y) * (p.y - q.y);
    }
};

struct Edge {
    int index;
    int from;
    int to;
    Weight weight;

    Edge(int index, int from, int to, Weight weight)
        : index(index), from(from), to(to), weight(weight) {}
    Edge() : index(-1), from(-1), to(-1), weight(0) {}
};

namespace Graph {
int num_vertices;
int num_edges;
Edge edges[MAX_NUM_EDGES];
Coordinate vertex_coordinates[MAX_NUM_VERTICES];
Edge adjacency_list[MAX_NUM_VERTICES][MAX_NUM_VERTICES];
int num_neighbors[MAX_NUM_VERTICES];

void compose_graph(
    int num_vertices, int num_edges, const array<Edge, MAX_NUM_EDGES>& edges,
    const array<Coordinate, MAX_NUM_VERTICES>& vertex_coordinates) {
    Graph::num_vertices = num_vertices;
    Graph::num_edges = num_edges;
    for (int i = 0; i < Graph::num_edges; i++) {
        Graph::edges[i] = edges[i];
    }
    for (int i = 0; i < Graph::num_vertices; i++) {
        Graph::vertex_coordinates[i] = vertex_coordinates[i];
    }
    for (int edge_i = 0; edge_i < Graph::num_edges; edge_i++) {
        const Edge& edge = Graph::edges[edge_i];
        Graph::adjacency_list[edge.from][Graph::num_neighbors[edge.from]] =
            Edge(edge.index, edge.from, edge.to, edge.weight);
        Graph::num_neighbors[edge.from]++;

        Graph::adjacency_list[edge.to][Graph::num_neighbors[edge.to]] =
            Edge(edge.index, edge.to, edge.from, edge.weight);
        Graph::num_neighbors[edge.to]++;
    }
}
}  // namespace Graph

namespace Problem {
int num_vertices;
int num_edges;
int num_days;
int max_num_planned_edges_per_day;

void read_input() {
    cin >> Problem::num_vertices >> Problem::num_edges >> Problem::num_days >>
        Problem::max_num_planned_edges_per_day;

    int u, v, w, x, y;
    array<Edge, MAX_NUM_EDGES> edges;
    array<Coordinate, MAX_NUM_VERTICES> vertex_coordinates;
    for (int edge_i = 0; edge_i < Problem::num_edges; edge_i++) {
        cin >> u >> v >> w;
        u--;
        v--;
        edges[edge_i] = Edge(edge_i, u, v, w);
    }
    for (int vertex_i = 0; vertex_i < Problem::num_vertices; vertex_i++) {
        cin >> x >> y;
        vertex_coordinates[vertex_i] = Coordinate(x, y);
    }
    Graph::compose_graph(Problem::num_vertices, Problem::num_edges, edges,
                         vertex_coordinates);
}
}  // namespace Problem

namespace Plan {
array<int, MAX_NUM_EDGES> planned_days_of_edges;
array<array<array<bool, MAX_NUM_VERTICES>, MAX_NUM_VERTICES>, MAX_NUM_DAYS>
    planned_adjacency_matrix;
array<int, MAX_NUM_DAYS> num_planned_edges_at_days;

void plan(int edge_i, int day) {
    int from = Graph::edges[edge_i].from, to = Graph::edges[edge_i].to;

    assert(Plan::planned_days_of_edges[edge_i] != day);
    Plan::planned_days_of_edges[edge_i] = day;
    Plan::num_planned_edges_at_days[day]++;

    Plan::planned_adjacency_matrix[day][from][to] =
        Plan::planned_adjacency_matrix[day][to][from] = true;
}

void unplan(int edge_i, int day) {
    int from = Graph::edges[edge_i].from, to = Graph::edges[edge_i].to;

    assert(Plan::planned_days_of_edges[edge_i] == day);
    Plan::planned_days_of_edges[edge_i] = -1;
    Plan::num_planned_edges_at_days[day]--;

    Plan::planned_adjacency_matrix[day][from][to] =
        Plan::planned_adjacency_matrix[day][to][from] = false;
}

void transfer_edge_between_days(int edge_i, int day_from, int day_to) {
    // 辺 edge_i の工事日を day_from から day_to に変更する
    Plan::unplan(edge_i, day_from);
    Plan::plan(edge_i, day_to);
}

void init_plan() {
    for (int edge_i = 0; edge_i < Problem::num_edges; edge_i++) {
        Plan::planned_days_of_edges[edge_i] = -1;
        int plan_day = xorshift(Problem::num_days);

        while (Plan::num_planned_edges_at_days[plan_day] >=
               Problem::max_num_planned_edges_per_day) {
            plan_day = xorshift(Problem::num_days);
        }

        Plan::plan(edge_i, plan_day);
    }
}

void output_to_stdout() {
    for (int edge_i = 0; edge_i < Problem::num_edges; edge_i++) {
        if (edge_i > 0) cout << " ";
        cout << Plan::planned_days_of_edges[edge_i] + 1;
    }
    cout << endl;
}
}  // namespace Plan

namespace ComputationCache {
array<array<array<int, MAX_NUM_VERTICES>, MAX_NUM_SAMPLES>, MAX_NUM_DAYS>
    distance_matrices;
array<array<array<int, MAX_NUM_VERTICES>, MAX_NUM_SAMPLES>, MAX_NUM_DAYS>
    distance_matrices_buffer;
array<array<array<int, MAX_NUM_VERTICES>, MAX_NUM_SAMPLES>, MAX_NUM_DAYS>
    distance_matrices_memo;
array<long long, MAX_NUM_DAYS> scores_at_days;
array<long long, MAX_NUM_DAYS> scores_at_days_buffer;

array<int, MAX_NUM_DAYS> previous_num_samples;

array<int, MAX_NUM_DAYS> buffer_counters;

void add_at(int day, int num_samples) {
    assert(ComputationCache::buffer_counters[day] == 0);
    ComputationCache::buffer_counters[day] = 1;
    ComputationCache::previous_num_samples[day] = num_samples;
}

void remove_at(int day) {
    assert(ComputationCache::buffer_counters[day] == 1);
    ComputationCache::buffer_counters[day] = 0;
}

void commit_at(int day, int sample_only = -1) {
    assert(ComputationCache::buffer_counters[day] == 1);
    ComputationCache::buffer_counters[day] = 0;

    for (int sample_i = 0;
         sample_i < ComputationCache::previous_num_samples[day]; sample_i++) {
        if (sample_only != -1 and sample_i != sample_only) continue;
        for (int vertex_i = 0; vertex_i < Graph::num_vertices; vertex_i++) {
            ComputationCache::distance_matrices[day][sample_i][vertex_i] =
                ComputationCache::distance_matrices_buffer[day][sample_i]
                                                          [vertex_i];
        }
    }
    ComputationCache::scores_at_days[day] =
        ComputationCache::scores_at_days_buffer[day];
}

bool buffer_empty() {
    bool empty = true;
    for (int i = 0; i < Problem::num_days; i++) {
        empty &= (ComputationCache::buffer_counters[i] == 0);
    }
    return empty;
}
}  // namespace ComputationCache

struct SampleArray {
    int num_samples;
    array<int, MAX_NUM_SAMPLES> samples;

    SampleArray() {}

    void sample_corners() {
        (*this).clear();

        vector<pair<Coordinate, int>> coordinates(Graph::num_vertices);

        for (int i = 0; i < Graph::num_vertices; i++) {
            coordinates[i].first = Graph::vertex_coordinates[i];
            coordinates[i].second = i;
        }

        sort(ALL(coordinates), [](const pair<Coordinate, int>& a,
                                  const pair<Coordinate, int>& b) {
            return tie(a.first.x, a.first.y) < tie(b.first.x, b.first.y);
        });
        (*this).add(coordinates.front().second);
        (*this).add(coordinates.back().second);

        sort(ALL(coordinates), [](const pair<Coordinate, int>& a,
                                  const pair<Coordinate, int>& b) {
            return tie(a.first.y, a.first.x) < tie(b.first.y, b.first.x);
        });
        (*this).add(coordinates.front().second);
        (*this).add(coordinates.back().second);

        Coordinate mean = Coordinate(500, 500);
        int min_square_distance_from_mean = INF;
        int near_mean = -1;
        for (const auto& [coordinate, index] : coordinates) {
            int square_distance_from_mean =
                Coordinate::square_distance(coordinate, mean);
            if (square_distance_from_mean < min_square_distance_from_mean) {
                min_square_distance_from_mean = square_distance_from_mean;
                near_mean = index;
            }
        }
        (*this).add(near_mean);

        // for (int i = 0; i < MAX_NUM_SAMPLES - 5; i++) {
        //     (*this).add(xorshift(Graph::num_vertices));
        // }
    }

    void update() {}

    void clear() { SampleArray::num_samples = 0; }

    void add(int vertex_i) {
        SampleArray::samples[SampleArray::num_samples] = vertex_i;
        SampleArray::num_samples++;
    }
};

struct ExecutionStats {
    int num_iteration;
    chrono::system_clock::time_point start_clock;
    array<long long, NUM_OPS> op_scores;

    void reset() {
        (*this).num_iteration = 0;
        (*this).start_clock = chrono::system_clock::now();
        for (int op_i = 0; op_i < NUM_OPS; op_i++) (*this).op_scores[op_i] = 0;
    }

    void touch() { (*this).num_iteration++; }

    double elapsed_time() {
        return (chrono::duration_cast<chrono::microseconds>(
                    chrono::system_clock::now() - (*this).start_clock)
                    .count() *
                1e-6);
    }

    bool its_time() {
        return ((*this).elapsed_time() > EXECUTION_DURATION_TIME);
    }

    bool its_time_local(const SampleArray& samples) {
        return (*this).num_iteration >= MAX_NUM_ITERATION;
    }

    void add_score(int op_i, long long score) {
        (*this).op_scores[op_i] += score;
    }

    bool accept(long long current_score, long long new_score, int num_sample) {
        // return current_score > new_score;
        static double startTemp = 100;
        static double endTemp = 1;
        static int R = 1000000;
        double elapsed_time =
            (chrono::duration_cast<chrono::microseconds>(
                 chrono::system_clock::now() - (*this).start_clock)
                 .count() *
             1e-6);
        double normal_t = (*this).elapsed_time() / EXECUTION_DURATION_TIME;
        double temp = pow(startTemp, 1 - normal_t) * pow(endTemp, normal_t);
        // if (EXECUTE_LOCAL) {
        //     // temp = max(startTemp + (endTemp - startTemp) *
        //     num_iteration /
        //     //                            MAX_NUM_ITERATION,
        //     //            1.0);
        // } else {
        //     temp =
        //         max(startTemp + (endTemp - startTemp) *
        //         (*this).elapsed_time() /
        //                             EXECUTION_DURATION_TIME,
        //             1.0);
        // }
        double probability =
            exp((current_score - new_score) / (double)num_sample /
                (double)Graph::num_vertices / temp);
        return (probability > xorshift(R) / (double)R);
    }

    void output_to_stderr() {
        for (int op_i = 0; op_i < NUM_OPS; op_i++) {
            cerr << "op " << op_i << "op score : " << (*this).op_scores[op_i]
                 << endl;
        }
        cerr << "num iteration: " << (*this).num_iteration << endl;
    }
};

void dijkstra(int s, int day, array<int, MAX_NUM_VERTICES>& distance_vector) {
    priority_queue<pi, vector<pi>, greater<pi>> pq;
    pq.push({distance_vector[s], s});

    while (not pq.empty()) {
        auto [distance, v] = pq.top();
        pq.pop();
        if (distance_vector[v] != distance) continue;
        int k = 0;
        for (int i = 0; i < Graph::num_neighbors[v]; i++) {
            int adjacency_v = Graph::adjacency_list[v][i].to,
                weight = Graph::adjacency_list[v][i].weight;
            if (Plan::planned_adjacency_matrix[day][v][adjacency_v]) continue;
            if (distance + weight >= distance_vector[adjacency_v]) continue;
            k = max(k, distance + weight);
            distance_vector[adjacency_v] = distance + weight;
            pq.push({distance_vector[adjacency_v], adjacency_v});
        }
    }
}

array<int, MAX_NUM_VERTICES> start_vs;
array<bool, MAX_NUM_VERTICES> selected_as_vs;
array<bool, MAX_NUM_VERTICES> selected_as_vs_2;
void dijkstra_multi_start(int start_nums, int day,
                          array<int, MAX_NUM_VERTICES>& distance_vector) {
    priority_queue<pi, vector<pi>, greater<pi>> pq;
    for (int i = 0; i < start_nums; i++) {
        pq.push({distance_vector[start_vs[i]], start_vs[i]});
        selected_as_vs[start_vs[i]] = false;
        selected_as_vs_2[start_vs[i]] = false;
    }

    while (not pq.empty()) {
        auto [distance, v] = pq.top();
        pq.pop();
        if (distance_vector[v] != distance) continue;
        int k = 0;
        for (int i = 0; i < Graph::num_neighbors[v]; i++) {
            int adjacency_v = Graph::adjacency_list[v][i].to,
                weight = Graph::adjacency_list[v][i].weight;
            if (Plan::planned_adjacency_matrix[day][v][adjacency_v]) continue;
            if (distance + weight >= distance_vector[adjacency_v]) continue;
            k = max(k, distance + weight);
            distance_vector[adjacency_v] = distance + weight;
            pq.push({distance_vector[adjacency_v], adjacency_v});
        }
    }
}

long long compute_score_at(int day, const SampleArray& samples,
                           int sample_only = -1) {
    ComputationCache::add_at(day, samples.num_samples);

    array<array<int, MAX_NUM_VERTICES>, MAX_NUM_SAMPLES>& distance_matrix =
        ComputationCache::distance_matrices_buffer[day];

    long long score = 0;

    for (int sample_i = 0; sample_i < samples.num_samples; sample_i++) {
        if (sample_only != -1 and sample_i != sample_only) continue;
        int sample = samples.samples[sample_i];
        array<int, MAX_NUM_VERTICES>& distance_vector =
            distance_matrix[sample_i];

        for (int i = 0; i < Graph::num_vertices; i++) {
            distance_vector[i] = INF;
        }
        distance_vector[sample] = 0;

        dijkstra(sample, day, distance_vector);

        long long sum_distance = 0;
        for (int i = 0; i < Graph::num_vertices; i++) {
            sum_distance += distance_vector[i];
        }

        score += sum_distance;
    }
    if (sample_only != -1) score += ComputationCache::scores_at_days[day];
    return ComputationCache::scores_at_days_buffer[day] = score;
}

long long compute_score_with_added_edge_at(int day, const SampleArray& samples,
                                           const Edge& added_edge) {
    ComputationCache::add_at(day, samples.num_samples);

    array<array<int, MAX_NUM_VERTICES>, MAX_NUM_SAMPLES>& distance_matrix =
        ComputationCache::distance_matrices_buffer[day];

    long long score = 0;

    for (int sample_i = 0; sample_i < samples.num_samples; sample_i++) {
        int sample = samples.samples[sample_i];
        array<int, MAX_NUM_VERTICES>& distance_vector =
            distance_matrix[sample_i];

        for (int i = 0; i < Graph::num_vertices; i++) {
            distance_vector[i] =
                ComputationCache::distance_matrices[day][sample_i][i];
        }

        int u = added_edge.from, v = added_edge.to, weight = added_edge.weight;
        if (distance_vector[u] > distance_vector[v]) swap(u, v);
        distance_vector[v] =
            min(distance_vector[v], distance_vector[u] + weight);

        dijkstra(v, day, distance_vector);

        long long sum_distance = 0;
        for (int i = 0; i < Graph::num_vertices; i++) {
            sum_distance += distance_vector[i];
        }

        score += sum_distance;
    }

    return ComputationCache::scores_at_days_buffer[day] = score;
}

long long compute_score_with_deleted_edge_at(int day,
                                             const SampleArray& samples,
                                             const Edge& deleted_edge) {
    ComputationCache::add_at(day, samples.num_samples);

    array<array<int, MAX_NUM_VERTICES>, MAX_NUM_SAMPLES>& distance_matrix =
        ComputationCache::distance_matrices_buffer[day];

    long long score = 0;

    for (int sample_i = 0; sample_i < samples.num_samples; sample_i++) {
        int sample = samples.samples[sample_i];

        array<int, MAX_NUM_VERTICES>& distance_vector =
            distance_matrix[sample_i];

        for (int i = 0; i < Graph::num_vertices; i++) {
            distance_vector[i] =
                ComputationCache::distance_matrices[day][sample_i][i];
        }

        int u = deleted_edge.from, v = deleted_edge.to,
            weight = deleted_edge.weight;
        if (distance_vector[u] > distance_vector[v]) swap(u, v);

        if (distance_vector[v] - distance_vector[u] >= weight) {
            queue<int> q;

            selected_as_vs[v] = true;
            q.push(v);
            int start_nums = 0;

            while (not q.empty()) {
                int p = q.front();
                q.pop();
                for (int i = 0; i < Graph::num_neighbors[p]; i++) {
                    const Edge& edge = Graph::adjacency_list[p][i];
                    if (Plan::planned_adjacency_matrix[day][p][edge.to])
                        continue;
                    if (distance_vector[edge.to] ==
                            distance_vector[p] + edge.weight and
                        not selected_as_vs[edge.to]) {
                        selected_as_vs[edge.to] = true;
                        q.push(edge.to);
                    }
                }
            }
            // for (int i = 0; i < Graph::num_vertices; i++) {
            //     if (not selected_as_vs[i]) continue;
            //     int new_dist = INF;
            //     for (int j = 0; j < Graph::num_neighbors[i]; j++) {
            //         const Edge& edge = Graph::adjacency_list[i][j];
            //         if (Plan::planned_adjacency_matrix[day][i][edge.to])
            //             continue;
            //         if (i == v and edge.to == u) continue;
            //         if (selected_as_vs[edge.to]) continue;
            //         new_dist =
            //             min(new_dist, distance_vector[edge.to] +
            //             edge.weight);
            //     }
            //     distance_vector[i] = new_dist;
            // }
            queue<pair<int, int>> pq;
            selected_as_vs_2[v] = true;
            pq.push({v, -1});
            while (not pq.empty()) {
                auto [p, prev] = pq.front();
                start_vs[start_nums] = p;
                start_nums++;
                pq.pop();
                int new_dist = INF;
                for (int i = 0; i < Graph::num_neighbors[p]; i++) {
                    const Edge& edge = Graph::adjacency_list[p][i];
                    if (Plan::planned_adjacency_matrix[day][p][edge.to]) {
                        continue;
                    }
                    if (p == v and edge.to == u) continue;
                    if (edge.to == prev) {
                        new_dist = min(new_dist,
                                       distance_vector[edge.to] + edge.weight);
                    } else if (distance_vector[edge.to] ==
                                   distance_vector[p] + edge.weight and
                               not selected_as_vs_2[edge.to]) {
                        selected_as_vs_2[edge.to] = true;
                        pq.push({edge.to, p});
                    } else if (not selected_as_vs[edge.to]) {
                        new_dist = min(new_dist,
                                       distance_vector[edge.to] + edge.weight);
                    }
                }
                distance_vector[p] = new_dist;
            }
            dijkstra_multi_start(start_nums, day, distance_vector);
        }

        long long sum_distance = 0;
        for (int i = 0; i < Graph::num_vertices; i++) {
            sum_distance += distance_vector[i];
        }

        score += sum_distance;
    }

    return ComputationCache::scores_at_days_buffer[day] = score;
}

long long compute_score_with_swap_edge_at(int day, const SampleArray& samples,
                                          const Edge& added_edge,
                                          const Edge& deleted_edge) {
    ComputationCache::add_at(day, samples.num_samples);

    array<array<int, MAX_NUM_VERTICES>, MAX_NUM_SAMPLES>& distance_matrix =
        ComputationCache::distance_matrices_buffer[day];

    long long score = 0;

    for (int sample_i = 0; sample_i < samples.num_samples; sample_i++) {
        int sample = samples.samples[sample_i];
        array<int, MAX_NUM_VERTICES>& distance_vector =
            distance_matrix[sample_i];

        int deleted_u = deleted_edge.from, deleted_v = deleted_edge.to,
            deleted_weight = deleted_edge.weight;

        for (int i = 0; i < Graph::num_vertices; i++) {
            distance_vector[i] =
                ComputationCache::distance_matrices[day][sample_i][i];
        }

        if (distance_vector[deleted_u] > distance_vector[deleted_v]) {
            swap(deleted_u, deleted_v);
        }

        if (distance_vector[deleted_v] - distance_vector[deleted_u] <
            deleted_weight) {
            int added_u = added_edge.from, added_v = added_edge.to,
                added_weight = added_edge.weight;
            if (distance_vector[added_u] > distance_vector[added_v]) {
                swap(added_u, added_v);
            }
            distance_vector[added_v] =
                min(distance_vector[added_v],
                    distance_vector[added_u] + added_weight);
            dijkstra(added_v, day, distance_vector);
        } else {
            queue<int> q;

            selected_as_vs[deleted_v] = true;
            q.push(deleted_v);
            int start_nums = 0;

            while (not q.empty()) {
                int p = q.front();
                q.pop();
                for (int i = 0; i < Graph::num_neighbors[p]; i++) {
                    const Edge& edge = Graph::adjacency_list[p][i];
                    if (Plan::planned_adjacency_matrix[day][p][edge.to])
                        continue;
                    if (distance_vector[edge.to] ==
                            distance_vector[p] + edge.weight and
                        not selected_as_vs[edge.to]) {
                        selected_as_vs[edge.to] = true;
                        q.push(edge.to);
                    }
                }
                start_vs[start_nums] = p;
                start_nums++;
            }
            queue<pair<int, int>> pq;
            selected_as_vs_2[deleted_v] = true;
            pq.push({deleted_v, -1});
            while (not pq.empty()) {
                auto [p, prev] = pq.front();
                pq.pop();
                int new_dist = INF;
                for (int i = 0; i < Graph::num_neighbors[p]; i++) {
                    const Edge& edge = Graph::adjacency_list[p][i];
                    if (Plan::planned_adjacency_matrix[day][p][edge.to]) {
                        continue;
                    }
                    if (p == deleted_v and edge.to == deleted_u) continue;
                    if (edge.to == prev) {
                        new_dist = min(new_dist,
                                       distance_vector[edge.to] + edge.weight);
                    } else if (distance_vector[edge.to] ==
                                   distance_vector[p] + edge.weight and
                               not selected_as_vs_2[edge.to]) {
                        selected_as_vs_2[edge.to] = true;
                        pq.push({edge.to, p});
                    } else if (not selected_as_vs[edge.to]) {
                        new_dist = min(new_dist,
                                       distance_vector[edge.to] + edge.weight);
                    }
                }
                distance_vector[p] = new_dist;
            }
            dijkstra_multi_start(start_nums, day, distance_vector);
            int added_u = added_edge.from, added_v = added_edge.to,
                added_weight = added_edge.weight;
            if (distance_vector[added_u] > distance_vector[added_v]) {
                swap(added_u, added_v);
            }
            distance_vector[added_v] =
                min(distance_vector[added_v],
                    distance_vector[added_u] + added_weight);
            dijkstra(added_v, day, distance_vector);
        }

        long long sum_distance = 0;
        for (int i = 0; i < Graph::num_vertices; i++) {
            sum_distance += distance_vector[i];
        }

        score += sum_distance;
    }

    return ComputationCache::scores_at_days_buffer[day] = score;
}

int edge_want(int edge_i, const SampleArray& samples) {
    assert(ComputationCache::buffer_empty());

    const Edge& edge = Graph::edges[edge_i];
    int plan_day = Plan::planned_days_of_edges[edge_i];

    int want = 0;
    for (int i = 0; i < samples.num_samples; i++) {
        int current_distance =
            abs(ComputationCache::distance_matrices[plan_day][i][edge.to] -
                ComputationCache::distance_matrices[plan_day][i][edge.from]);

        if (current_distance > edge.weight) want++;
    }
    return want;
}

pair<int, long long> edge_waste(int edge_i, int day,
                                const SampleArray& samples) {
    assert(ComputationCache::buffer_empty());

    const Edge& edge = Graph::edges[edge_i];

    int waste = 0;
    long long waste_distance = 0;

    for (int i = 0; i < samples.num_samples; i++) {
        int current_distance =
            abs(ComputationCache::distance_matrices[day][i][edge.to] -
                ComputationCache::distance_matrices[day][i][edge.from]);
        if (current_distance < edge.weight) {
            waste++;
            waste_distance += edge.weight - current_distance;
        }
        // else {
        //     int min_dis = INF;
        //     REP(i, adj_num[v]) {
        //         auto [a, w] = adj[v][i];
        //         if (planned_edge[day][v][a]) continue;
        //         min_dis = min(min_dis, dist[j][day][a] + w);
        //     }
        //     if (min_dis - dist[j][day][u] < 10000)
        // unnecessarity++;
        // }
    }
    return make_pair(waste, waste_distance);
}

pair<int, int> select_max_waste_day_for(int edge_i, const SampleArray& samples,
                                        bool use_rnd = true) {
    int plan_day = Plan::planned_days_of_edges[edge_i];
    int max_waste_day = -1, max_waste = -1;
    long long max_waste_distance = -1;

    vector<int> candidate_days(Problem::num_days);
    iota(ALL(candidate_days), 0);
    if (use_rnd) shuffle(ALL(candidate_days), mt);

    for (auto day : candidate_days) {
        if (day == plan_day) continue;
        if (Plan::num_planned_edges_at_days[day] ==
            Problem::max_num_planned_edges_per_day) {
            continue;
        }
        auto [waste, waste_distance] = edge_waste(edge_i, day, samples);
        if (waste > max_waste or
            (waste == max_waste and waste_distance > max_waste_distance)) {
            max_waste = waste;
            max_waste_distance = waste_distance;
            max_waste_day = day;
        }
    }

    return make_pair(max_waste_day, max_waste);
}

// void op_swap_based_on_waste(const SampleArray& samples, ExecutionStats&
// stats,
//                             int op_i = 0) {
//     int edge_1 = xorshift(Problem::num_edges);

//     if (edge_want(edge_1, samples) == 0) return;

//     int day_1 = Plan::planned_days_of_edges[edge_1];

//     auto [day_2, waste] = select_max_waste_day_for(edge_1, samples, true);

//     // // waste が最大なら定期的に移されるはずなので枝切り
//     // if (waste == samples.num_samples) return;

//     int edge_2 = xorshift(Problem::num_edges);
//     while (Plan::planned_days_of_edges[edge_2] != day_2) {
//         edge_2 = xorshift(Problem::num_edges);
//     }

//     long long current_score = ComputationCache::scores_at_days[day_1] +
//                               ComputationCache::scores_at_days[day_2];

//     Plan::transfer_edge_between_days(edge_1, day_1, day_2);
//     Plan::transfer_edge_between_days(edge_2, day_2, day_1);

//     long long new_score =
//         compute_score_with_swap_edge_at(day_1, samples, Graph::edges[edge_1],
//                                         Graph::edges[edge_2]) +
//         compute_score_with_swap_edge_at(day_2, samples, Graph::edges[edge_2],
//                                         Graph::edges[edge_1]);

//     if (not stats.accept(current_score, new_score, samples.num_samples)) {
//         ComputationCache::remove_at(day_1);
//         ComputationCache::remove_at(day_2);
//         Plan::transfer_edge_between_days(edge_1, day_2, day_1);
//         Plan::transfer_edge_between_days(edge_2, day_1, day_2);
//     } else {
//         ComputationCache::commit_at(day_1);
//         ComputationCache::commit_at(day_2);
//         stats.add_score(op_i, current_score - new_score);
//     }
// }

// void op_transfer_based_on_waste(const SampleArray& samples,
//                                 ExecutionStats& stats, bool rnd_day_2,
//                                 int op_i = 2) {
//     static int edge_for_not_rnd = 0;
//     // static int edge_1 = 0;
//     // edge_1 = (edge_1 + 1) % Graph::num_edges;
//     // int edge_1 = xorshift(Graph::num_edges);
//     int edge_1;
//     if (not rnd_day_2) {
//         edge_1 = edge_for_not_rnd;
//         edge_for_not_rnd = (edge_for_not_rnd + 1) % Graph::num_edges;
//     } else {
//         edge_1 = xorshift(Graph::num_edges);
//     }

//     if (EXECUTE_LOCAL) {
//         for (int i = 0; i < Problem::num_edges; i++) {
//             if (edge_want(edge_1, samples) == 0) {
//                 edge_1 = xorshift(Problem::num_edges);
//             } else {
//                 break;
//             }
//         }
//     } else {
//         if (edge_want(edge_1, samples) == 0) return;
//     }

//     int day_1 = Plan::planned_days_of_edges[edge_1];
//     int day_2;
//     if (rnd_day_2) {
//         day_2 = xorshift(Problem::num_days);
//         while (day_1 == day_2 or Plan::num_planned_edges_at_days[day_2] ==
//                                      Problem::max_num_planned_edges_per_day)
//             day_2 = (day_2 + 1) % Problem::num_days;
//     } else {
//         int waste;
//         tie(day_2, waste) = select_max_waste_day_for(edge_1, samples, true);
//         // // waste が最大なら定期的に移されるはずなので枝切り
//         // if (waste == samples.num_samples) return;
//     }

//     long long current_score = ComputationCache::scores_at_days[day_1] +
//                               ComputationCache::scores_at_days[day_2];

//     Plan::transfer_edge_between_days(edge_1, day_1, day_2);

//     long long new_score =
//         compute_score_with_added_edge_at(day_1, samples,
//         Graph::edges[edge_1]) + compute_score_with_deleted_edge_at(day_2,
//         samples,
//                                            Graph::edges[edge_1]);

//     if (not stats.accept(current_score, new_score, samples.num_samples)) {
//         ComputationCache::remove_at(day_1);
//         ComputationCache::remove_at(day_2);
//         Plan::transfer_edge_between_days(edge_1, day_2, day_1);
//     } else {
//         ComputationCache::commit_at(day_1);
//         ComputationCache::commit_at(day_2);
//         stats.add_score(op_i, current_score - new_score);
//     }
// }

// void op_swap_based_on_waste_optimized(const SampleArray& samples,
//                                       ExecutionStats& stats, int op_i =
//                                       0) {
//     int edge_1 = xorshift(Problem::num_edges);

//     if (EXECUTE_LOCAL) {
//         for (int i = 0; i < Problem::num_edges; i++) {
//             if (edge_want(edge_1, samples) == 0) {
//                 edge_1 = xorshift(Problem::num_edges);
//             } else {
//                 break;
//             }
//         }
//     } else {
//         if (edge_want(edge_1, samples) == samples.num_samples) return;
//     }

//     int day_1 = Plan::planned_days_of_edges[edge_1];

//     auto [day_2, waste] = select_max_waste_day_for(edge_1, samples,
//     true);

//     // // waste が最大なら定期的に移されるはずなので枝切り
//     // if (waste == samples.num_samples) return;

//     // できるだけ day_1 で不要な辺を工事する案
//     int max_waste = -1;
//     array<int, MAX_NUM_EDGES> candidate_edges;
//     int num_candidate_edges = 0;
//     for (int i = 0; i < Problem::num_edges; i++) {
//         if (Plan::planned_days_of_edges[i] != day_2) continue;
//         auto [waste, _] = edge_waste(i, day_1, samples);
//         if (waste > max_waste) {
//             max_waste = waste;
//             num_candidate_edges = 0;
//         }
//         if (waste == max_waste) {
//             candidate_edges[num_candidate_edges] = i;
//             num_candidate_edges++;
//         }
//     }
//     int edge_2 = candidate_edges[xorshift(num_candidate_edges)];

//     long long current_score = ComputationCache::scores_at_days[day_1] +
//                               ComputationCache::scores_at_days[day_2];

//     Plan::transfer_edge_between_days(edge_1, day_1, day_2);
//     Plan::transfer_edge_between_days(edge_2, day_2, day_1);

//     long long new_score =
//         compute_score_at(day_1, samples) + compute_score_at(day_2,
//         samples);

//     if (new_score > current_score) {
//         ComputationCache::remove_at(day_1);
//         ComputationCache::remove_at(day_2);
//         Plan::transfer_edge_between_days(edge_1, day_2, day_1);
//         Plan::transfer_edge_between_days(edge_2, day_1, day_2);
//     } else {
//         ComputationCache::commit_at(day_1);
//         ComputationCache::commit_at(day_2);
//         stats.add_score(op_i, current_score - new_score);
//     }
// }

// void op_swap(const SampleArray& samples, ExecutionStats& stats, int op_i
// = 3)
// {
//     int edge_1 = xorshift(Problem::num_edges),
//         edge_2 = xorshift(Problem::num_edges);
//     while (Plan::planned_days_of_edges[edge_2] ==
//            Plan::planned_days_of_edges[edge_1]) {
//         edge_2 = xorshift(Problem::num_edges);
//     }

//     int day_1 = Plan::planned_days_of_edges[edge_1],
//         day_2 = Plan::planned_days_of_edges[edge_2];

//     long long current_score = ComputationCache::scores_at_days[day_1] +
//                               ComputationCache::scores_at_days[day_2];

//     Plan::transfer_edge_between_days(edge_1, day_1, day_2);
//     Plan::transfer_edge_between_days(edge_2, day_2, day_1);

//     long long new_score =
//         compute_score_at(day_1, samples) + compute_score_at(day_2,
//         samples);

//     if (new_score > current_score) {
//         ComputationCache::remove_at(day_1);
//         ComputationCache::remove_at(day_2);
//         Plan::transfer_edge_between_days(edge_1, day_2, day_1);
//         Plan::transfer_edge_between_days(edge_2, day_1, day_2);
//     } else {
//         ComputationCache::commit_at(day_1);
//         ComputationCache::commit_at(day_2);
//         stats.add_score(op_i, current_score - new_score);
//     }
// }

// void op_clean_up_obvious_transfer(const SampleArray& samples,
//                                   ExecutionStats& stats, int op_i = 1) {
//     vector<int> edges(Problem::num_edges);
//     iota(ALL(edges), 0);
//     shuffle(ALL(edges), mt);

//     for (auto& edge_i : edges) {
//         int plan_day = Plan::planned_days_of_edges[edge_i];
//         auto [waste_day, waste] =
//             select_max_waste_day_for(edge_i, samples, true);

//         if (waste != samples.num_samples) continue;
//         long long current_score = ComputationCache::scores_at_days[plan_day]
//         +
//                                   ComputationCache::scores_at_days[waste_day];

//         Plan::transfer_edge_between_days(edge_i, plan_day, waste_day);

//         long long new_score = compute_score_with_added_edge_at(
//                                   plan_day, samples, Graph::edges[edge_i]) +
//                               compute_score_with_deleted_edge_at(
//                                   waste_day, samples, Graph::edges[edge_i]);
//         ComputationCache::commit_at(plan_day);
//         ComputationCache::commit_at(waste_day);
//         stats.add_score(op_i, current_score - new_score);
//     }

//     // for (int day = 0; day < Problem::num_days; day++) {
//     //     long long current_score = ComputationCache::scores_at_days[day];
//     //     long long new_score = compute_score_at(day, samples);
//     //     ComputationCache::commit_at(day);
//     //     stats.add_score(op_i, current_score - new_score);
//     // }
// }

void test(const SampleArray& samples, string str) {
    long long sum_1 = 0, sum_2 = 0;
    for (int day = 0; day < Problem::num_days; day++) {
        sum_1 += ComputationCache::scores_at_days[day];
        compute_score_at(day, samples);
        ComputationCache::commit_at(day);
        sum_2 += ComputationCache::scores_at_days[day];
    }
    cerr << str << endl;
    assert(sum_1 == sum_2);
}

array<int, MAX_NUM_EDGES> original_days;
array<bool, MAX_NUM_VERTICES> used_v;
int vertex_1 = 0;
void op_distribute_around_vertex(const SampleArray& samples,
                                 ExecutionStats& stats, int op_i = 4) {
    // static int vertex_1 = 0;
    vertex_1 = (vertex_1 + 1) % (Graph::num_vertices);
    vector<int> target_edges;

    // for(int day = 0; day < Problem::num_days; day++) {
    //     for(int si = 0; si < samples.num_samples; si++) {
    //         for(int vi = 0; vi < samples.num_samples; vi++) {

    //         }
    //     }
    // }

    long long current_score = 0;
    for (int day = 0; day < Problem::num_days; day++) {
        current_score += ComputationCache::scores_at_days[day];
    }

    for (int i = 0; i < Graph::num_neighbors[vertex_1]; i++) {
        const Edge& edge = Graph::adjacency_list[vertex_1][i];
        target_edges.push_back(edge.index);
        int day = Plan::planned_days_of_edges[edge.index];
        original_days[edge.index] = day;

        Plan::unplan(edge.index, day);
        compute_score_with_added_edge_at(day, samples,
                                         Graph::edges[edge.index]);
        ComputationCache::commit_at(day);
    }
    shuffle(ALL(target_edges), mt);
    for (auto edge_i : target_edges) {
        int best_day = -1;
        long long best_score = -INF * INF;
        for (int day = 0; day < Problem::num_days; day++) {
            if (Plan::num_planned_edges_at_days[day] ==
                Problem::max_num_planned_edges_per_day) {
                continue;
            }
            Plan::plan(edge_i, day);
            long long cur_sco = ComputationCache::scores_at_days[day];
            long long new_sco = compute_score_with_deleted_edge_at(
                day, samples, Graph::edges[edge_i]);
            if (cur_sco - new_sco > best_score) {
                best_score = cur_sco - new_sco;
                best_day = day;
            }
            Plan::unplan(edge_i, day);
            ComputationCache::remove_at(day);

            if (best_score == 0) break;
        }
        // cerr << best_day << endl;
        Plan::plan(edge_i, best_day);
        // cerr << best_day << endl;

        compute_score_with_deleted_edge_at(best_day, samples,
                                           Graph::edges[edge_i]);
        ComputationCache::commit_at(best_day);
    }

    long long new_score = 0;

    for (int day = 0; day < Problem::num_days; day++) {
        new_score += ComputationCache::scores_at_days[day];
    }

    if (not stats.accept(current_score, new_score, samples.num_samples)) {
        for (auto edge_i : target_edges) {
            int cur_day = original_days[edge_i],
                new_day = Plan::planned_days_of_edges[edge_i];
            Plan::unplan(edge_i, new_day);
            compute_score_with_added_edge_at(new_day, samples,
                                             Graph::edges[edge_i]);
            ComputationCache::commit_at(new_day);
            Plan::plan(edge_i, cur_day);
            compute_score_with_deleted_edge_at(cur_day, samples,
                                               Graph::edges[edge_i]);
            ComputationCache::commit_at(cur_day);
        }
    } else {
        // cerr << current_score << " " << new_score << endl;
        stats.add_score(op_i, current_score - new_score);
    }
}

array<bool, MAX_NUM_VERTICES> used_as_samples;
array<int, MAX_NUM_VERTICES> sample_candidates;
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    ExecutionStats stats = ExecutionStats();

    stats.reset();
    Problem::read_input();

    SampleArray samples = SampleArray();
    samples.sample_corners();

    for (int si = 0; si < samples.num_samples; si++) {
        used_as_samples[samples.samples[si]] = true;
        sample_candidates[si] = samples.samples[si];
    }

    for (int si = samples.num_samples; si < Graph::num_vertices; si++) {
        int max_v = -1;
        int max_dist = -1;
        for (int vi = 0; vi < Graph::num_vertices; vi++) {
            if (used_as_samples[vi]) continue;
            int dist = INF;
            for (int ssi = 0; ssi < si; ssi++) {
                int sample = sample_candidates[ssi];
                dist = min(dist, Coordinate::square_distance(
                                     Graph::vertex_coordinates[vi],
                                     Graph::vertex_coordinates[sample]));
            }
            if (dist > max_dist) {
                max_dist = dist;
                max_v = vi;
            }
        }
        used_as_samples[max_v] = true;
        sample_candidates[si] = max_v;
    }

    // long long min_score = 1e18;
    // array<int, MAX_NUM_EDGES> min_planned_days_of_edges;

    // for (int i = 0; i < 100; i++) {
    //     Plan::init_plan();
    //     long long score = 0;
    //     for (int day = 0; day < Problem::num_days; day++) {
    //         score += compute_score_at(day, samples);
    //         ComputationCache::remove_at(day);
    //     }
    //     if (score < min_score) {
    //         min_score = score;
    //         for (int edge_i = 0; edge_i < Problem::num_edges; edge_i++) {
    //             min_planned_days_of_edges[edge_i] =
    //                 Plan::planned_days_of_edges[edge_i];
    //         }
    //     }
    //     for (int edge_i = 0; edge_i < Problem::num_edges; edge_i++) {
    //         Plan::unplan(edge_i, Plan::planned_days_of_edges[edge_i]);
    //     }
    // }
    // for (int edge_i = 0; edge_i < Problem::num_edges; edge_i++) {
    //     Plan::plan(edge_i, min_planned_days_of_edges[edge_i]);
    // }

    timer.start();
    for (int day = 0; day < Problem::num_days; day++) {
        compute_score_at(day, samples);
        ComputationCache::commit_at(day);
    }
    timer.stop();

    int start_sample_num = 5;
    double ppp = (Graph::num_vertices * Problem::num_days - 1500) / 28500;
    int end_sample_num = 20 * ppp + 40 * (1 - ppp);

    for (int i = 0; i < start_sample_num - 5; i++) {
        // long long max_dist = -1;
        // int max_v = -1;
        // for (int vi = 0; vi < Graph::num_vertices; vi++) {
        //     long long dist = 0;
        //     for (int day = 0; day < Problem::num_days; day++) {
        //         for (int si = 0; si < samples.num_samples; si++) {
        //             dist += ComputationCache::distance_matrices[day][si][vi];
        //         }
        //     }
        //     if (dist > max_dist) {
        //         max_dist = dist;
        //         max_v = vi;
        //     }
        // }
        samples.add(sample_candidates[samples.num_samples]);
        for (int day = 0; day < Problem::num_days; day++) {
            compute_score_at(day, samples, samples.num_samples - 1);
            ComputationCache::commit_at(day, samples.num_samples - 1);
        }
    }

    // for (int v = 0; v < Graph::num_vertices; v++) {
    //     op_distribute_around_vertex(samples, stats);
    // }

    for (int ei = 0; ei < Graph::num_edges; ei++) {
        Plan::planned_days_of_edges[ei] = -1;
        int plan_day = -1;
        long long min_cost = 1e18;
        vector<int> days(Problem::num_days);
        iota(ALL(days), 0);
        shuffle(ALL(days), mt);
        for (auto day : days) {
            if (Plan::num_planned_edges_at_days[day] ==
                Problem::max_num_planned_edges_per_day) {
                continue;
            }
            Plan::plan(ei, day);
            long long cost = compute_score_with_deleted_edge_at(
                day, samples, Graph::edges[ei]);
            if (cost < min_cost) {
                min_cost = cost;
                plan_day = day;
            }
            Plan::unplan(ei, day);
            ComputationCache::remove_at(day);
        }
        Plan::plan(ei, plan_day);
        compute_score_with_deleted_edge_at(plan_day, samples, Graph::edges[ei]);
        ComputationCache::commit_at(plan_day);
    }

    while (true) {
        if (EXECUTE_LOCAL) {
            // if (stats.its_time_local(samples)) break;
        } else {
            if (stats.its_time()) break;
        }

        if (EXECUTE_LOCAL) {
            // int max_samples = stats.num_iteration / MAX_NUM_ITERATION * 50;
            // int original_samples = samples.num_samples;
            // if (samples.num_samples < max_samples) {
            //     samples.add(min(Graph::num_vertices - 1,
            //     samples.num_samples));
            // }
            // if (original_samples < samples.num_samples) {
            //     for (int day = 0; day < Problem::num_days; day++) {
            //         compute_score_at(day, samples, samples.num_samples - 1);
            //         ComputationCache::commit_at(day, samples.num_samples -
            //         1);
            //     }
            // }

            // if (stats.num_iteration > MAX_NUM_ITERATION * 0.5 and
            //     stats.num_iteration % 10000 == 0) {
            //     op_clean_up_obvious_transfer(samples, stats);
            // }
        } else {
            int max_samples =
                stats.elapsed_time() / EXECUTION_DURATION_TIME * end_sample_num;
            int original_samples = samples.num_samples;
            if (samples.num_samples < max_samples) {
                samples.add(sample_candidates[samples.num_samples]);
            }
            if (original_samples < samples.num_samples) {
                for (int day = 0; day < Problem::num_days; day++) {
                    compute_score_at(day, samples, samples.num_samples - 1);
                    ComputationCache::commit_at(day, samples.num_samples - 1);
                }
            }
        }

        // int select = xorshift(2);
        // stats.num_iteration *= 10;
        // if (stats.its_time_local(samples)) {
        //     if (stats.op_scores[0] >= stats.op_scores[1]) {
        //         select = 0;
        //     } else {
        //         select = 1;
        //     }
        // }
        // stats.num_iteration /= 10;
        // if (stats.num_iteration % 1000 == 0) {
        //     op_clean_up_obvious_transfer(samples, stats);
        // }
        if (true) {
            op_distribute_around_vertex(samples, stats);
        } else {
            // op_transfer_based_on_waste(samples, stats, false);
        }
        stats.touch();
    }

    // for (int v = 0; v < Graph::num_vertices; v++) {
    //     op_distribute_around_vertex(samples, stats);
    // }

    // for (int i = 0; i < 30; i++) {
    //     samples.add(xorshift(Graph::num_vertices));
    // }
    // for (int day = 0; day < Problem::num_days; day++) {
    //     compute_score_at(day, samples);
    //     ComputationCache::commit_at(day);
    // }
    // op_clean_up_obvious_transfer(samples, stats);
    cerr << timer.total_time << endl;
    stats.output_to_stderr();
    Plan::output_to_stdout();

    return 0;
}
