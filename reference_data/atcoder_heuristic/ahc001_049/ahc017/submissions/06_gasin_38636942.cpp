#include <array>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <numeric>
#include <iostream>
#include <random>

using P = std::pair<int, int>;

constexpr int MAX_N = 1000;
constexpr int MAX_M = 3000;
constexpr int MAX_D = 30;
constexpr int UNREACH = 1000000000;
constexpr int INF = 100000000;

constexpr double TL = 5.8;
constexpr int SAMPLE_NUM = 150;

constexpr unsigned long long int cycle_per_sec = 3000000000;

unsigned long long int getCycle() {
    unsigned int low, high;
    __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
    return ((unsigned long long int)low) | ((unsigned long long int)high << 32);
}

double getTime (unsigned long long int begin_cycle) {
    return (double)(getCycle() - begin_cycle) / cycle_per_sec;
}

class XorShift {
public:
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int w;
    unsigned int t;
    
    XorShift(int tmp) {
        std::mt19937 rnd(tmp);
        x = rnd();
        y = rnd();
        z = rnd();
        w = rnd();
        t = 1;
    }
    
    int rand() {
        t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        return w & 0x7fffffff;
    }
} engine(1);

struct Node {
    int x;
    int y;
};

struct Edge {
    int id;
    int from;
    int to;
    int weight;
};

namespace answer {
    std::array<int, MAX_M> colors;

    // for preprocess
    std::array<int, MAX_D> counts;

    std::array<std::array<std::array<int, MAX_N>, SAMPLE_NUM>, MAX_D> cached_dists;
    std::array<std::array<long long, SAMPLE_NUM>, MAX_D> scores_per_color;

    void init() {
        colors.fill(-1);
        counts.fill(0);
    }

    long long score_per_color(int c, int sample_num) {
        return std::accumulate(scores_per_color[c].begin(), scores_per_color[c].begin()+sample_num, 0LL);
    }

    long long calc_estimated_score(int N, int D, int sample_num, long long base_score) {
        long long acc_score = 0;
        for (int i = 0; i < sample_num; i++) {
            for (int j = 0; j < D; j++) {
                acc_score += scores_per_color[j][i];
            }
        }
        acc_score -= base_score*D;

        return std::round(1000 * (acc_score * N / (double)sample_num) / (N * (N-1) * D));
    }
};

std::priority_queue<long long, std::vector<long long>, std::greater<long long>> global_pque;
constexpr int GPQ_MASK = (1<<10)-1;
constexpr int GPQ_SHIFT = 10;

long long BASE_SCORE;
long long BASE_SAMPLE_SCORE;
std::array<long long, MAX_N> BASE_SCORE_PER_NODE;

int N, M, D, K;
std::array<Node, MAX_N> nodes;
std::vector<std::vector<Edge>> edges;
std::array<Edge, MAX_M> edge_arr;

std::array<int, SAMPLE_NUM> samples;
std::array<std::array<int, MAX_N>, MAX_N> ideal_matrix;

std::array<std::array<int, MAX_N>, SAMPLE_NUM> dists_buffer, dists_buffer2;

// -1 .. negative, 0 .. continue, 1 .. positive
inline int judge_diff(std::array<long long, SAMPLE_NUM>& diffs, int num) {
    double diff_ave = std::accumulate(diffs.begin(), diffs.begin() + num, 0LL) / num;

    double diff_var = 0;
    for (int i = 0; i < num; i++) {
        diff_var += (diffs[i] - diff_ave) * (diffs[i] - diff_ave);
    }
    double diff_sd = std::sqrt(diff_var / num);
    if (diff_ave - diff_sd > 0) {
        return 1;
    }
    if (diff_ave + diff_sd < 0) {
        return -1;
    }
    return 0;
}

inline int distance2(const Node& x, const Node& y) {
    return (x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y);
}

inline double distance(const Node& x, const Node& y) {
    return std::sqrt(distance2(x, y));
}

int find_closest_node(const Node& pos) {
    int closest = INF;
    int index = -1;
    for (int i = 0; i < N; i++) {
        int dist = distance2(nodes[i], pos);
        if (dist < closest) {
            closest = dist;
            index = i;
        }
    }
    return index;
}

inline long long make_gpq_value(int dist, int node) {
    return (dist << (long long)GPQ_SHIFT) + node;
}

void input() {
    std::cin >> N >> M >> D >> K;
    edges.resize(N);
    for (int i = 0; i < M; i++) {
        int u, v, w;
        std::cin >> u >> v >> w;
        u--;
        v--;
        edges[u].push_back(Edge{i, u, v, w});
        edges[v].push_back(Edge{i, v, u, w});
        edge_arr[i] = Edge{i, u, v, w};
    }
    for (int i = 0; i < N; i++) {
        int x, y;
        std::cin >> x >> y;
        nodes[i] = Node{x, y};
    }
}

long long calc_dijkstra_sum_core(int color, std::array<int, MAX_N>& dists) {
    while (!global_pque.empty()) {
        int dist_w = global_pque.top();
        int dist = (dist_w >> GPQ_SHIFT), w = dist_w & GPQ_MASK;
        global_pque.pop();
        if (dist > dists[w]) {
            continue;
        }
        for (const Edge& e : edges[w]) {
            if (answer::colors[e.id] == color) {
                continue;
            }
            if (dist + e.weight < dists[e.to]) {
                dists[e.to] = dist + e.weight;
                global_pque.push(make_gpq_value(dists[e.to], e.to));
            }
        }
    }

    return std::accumulate(dists.begin(), dists.begin() + N, 0LL);
}

long long calc_score_color_add_edge(int color, const Edge& e, int i, std::array<int, MAX_N>& dists) {
    dists = answer::cached_dists[color][i];
    if (std::abs(dists[e.from] - dists[e.to]) <= e.weight) {
        return std::accumulate(dists.begin(), dists.begin()+N, 0LL);
    }
    if (dists[e.from] > dists[e.to]) {
        global_pque.push(make_gpq_value(dists[e.to], e.to));
        return calc_dijkstra_sum_core(color, dists);
    } else {
        global_pque.push(make_gpq_value(dists[e.from], e.from));
        return calc_dijkstra_sum_core(color, dists);
    }
}

long long calc_score_color_remove_edge(int color, const Edge& e, int i, std::array<int, MAX_N>& dists) {
    dists = answer::cached_dists[color][i];
    if (std::abs(dists[e.from] - dists[e.to]) < e.weight) {
        return std::accumulate(dists.begin(), dists.begin()+N, 0LL);
    }

    std::array<int, MAX_N> buf2;
    std::array<long long, MAX_N> buf;
    int buf_size = 0, buf_size2 = 0;

    int fixed_dist = std::max(dists[e.from], dists[e.to]);
    int base_node = dists[e.from] > dists[e.to] ? e.from : e.to;
    for (int j = 0; j < N; j++) {
        if (dists[j] >= fixed_dist + ideal_matrix[base_node][j]) {
            dists[j] = UNREACH;
            buf2[buf_size2++] = j;
        }
    }
    for (int _j = 0; _j < buf_size2; _j++) {
        int j = buf2[_j];
        for (const Edge& edge: edges[j]) {
            if (answer::colors[edge.id] != color) {
                dists[j] = std::min(dists[j], dists[edge.to] + edge.weight);
            }
        }
        if (dists[j] != UNREACH) {
            buf[buf_size++] = make_gpq_value(dists[j], j);
        }
    }
    global_pque = std::priority_queue<long long, std::vector<long long>, std::greater<long long>>(buf.begin(), buf.begin() + buf_size);

    return calc_dijkstra_sum_core(color, dists);
}

long long calc_score_color(int color, int i, std::array<int, MAX_N>& dbuf) {
    dbuf.fill(UNREACH);
    dbuf[i] = 0;
    global_pque.push(make_gpq_value(dbuf[i], i));
    return calc_dijkstra_sum_core(color, dbuf);
}

void set_color(const Edge& edge, int color) {
    answer::colors[edge.id] = color;
    answer::counts[color]++;
    for (int i = 0; i < SAMPLE_NUM; i++) {
        answer::scores_per_color[color][i] = calc_score_color_remove_edge(color, edge, i, answer::cached_dists[color][i]);
    }
}

void reset_color(const Edge& edge) {
    int color = answer::colors[edge.id];
    answer::colors[edge.id] = -1;
    answer::counts[color]--;
    for (int i = 0; i < SAMPLE_NUM; i++) {
        answer::scores_per_color[color][i] = calc_score_color_add_edge(color, edge, i, answer::cached_dists[color][i]);
    }
}


void assign_color(const Edge& edge) {

    std::array<long long, MAX_D> diffs{};
    for (int c = 0; c < D; c++) {
        if (answer::counts[c] == K) {
            diffs[c] = -1;
        }
    }
    std::array<int, SAMPLE_NUM> iotas;
    std::iota(iotas.begin(), iotas.begin()+SAMPLE_NUM, 0);
    /* std::sort(iotas.begin(), iotas.begin() + SAMPLE_NUM, [&](const int l, const int r) {
        return ideal_matrix[edge.from][l] < ideal_matrix[edge.from][r];
    });
    */
    for (int i = 1; i < SAMPLE_NUM; i++) {
        int x = engine.rand()%(i+1);
        std::swap(iotas[x], iotas[i]);
    }
    for (int i_ = 0; i_ < SAMPLE_NUM; i_++) {
        int i = iotas[i_];
        for (int color = 0; color < D; color++) {
            if (diffs[color] == -1) {
                continue;
            }
            answer::colors[edge.id] = color;
            diffs[color] += calc_score_color_remove_edge(color, edge, i, dists_buffer[i]) - answer::scores_per_color[color][i];
            answer::colors[edge.id] = -1;
        }

        int remain_num = 0;
        for (int c = 0; c < D; c++) {
            if (diffs[c] != -1) {
                remain_num++;
            }
        }

        while (remain_num > 1 && remain_num * i_ >= (SAMPLE_NUM-1)*2) {
            int pos = std::max_element(diffs.begin(), diffs.begin() + D) - diffs.begin();
            diffs[pos] = -1;
            remain_num--;
        }
        if (remain_num == 1) {
            break;
        }
    }
    int next_color = std::max_element(diffs.begin(), diffs.begin() + D) - diffs.begin();

    set_color(edge, next_color);
}

std::array<int, MAX_M> arrange_edges() {
    std::array<std::pair<long long, int>, MAX_M> scores;
    for (int i = 0; i < M; i++) {
        scores[i] = std::make_pair(0, i);
        answer::colors[i] = 0;
        for (int j = 0; j < SAMPLE_NUM/2; j++) {
            scores[i].first += calc_score_color_remove_edge(0, edge_arr[i], j, dists_buffer[j]);
        }
        answer::colors[i] = -1;
    }
    std::sort(scores.begin(), scores.begin()+M, std::greater<std::pair<long long, int>>());
    std::array<int, MAX_M> edge_order;
    for (int i = 0; i < M; i++) {
        edge_order[i] = scores[i].second;
    }
    return edge_order;
}

void initialize_answer(unsigned long long start_time) {
    answer::init();
    for (int i = 0; i < SAMPLE_NUM; i++) {
        answer::scores_per_color[0][i] = calc_score_color(0, samples[i], dists_buffer[i]);
    }
    answer::cached_dists[0] = dists_buffer;
    for (int c = 1; c < D; c++) {
        answer::scores_per_color[c] = answer::scores_per_color[0];
        answer::cached_dists[c] = answer::cached_dists[0];
    }

    std::array<int, MAX_M> edge_order = arrange_edges();
    std::array<int, MAX_M> edge_rorder;
    for (int i = 0; i < M; i++) {
        edge_rorder[edge_order[i]] = i;
    }

    double avg_time = 0.0;
    int time_cnt = 0;
    for (int i = 0; i < M; i++) {
        double st_time = getTime(start_time);
        assign_color(edge_arr[edge_order[i]]);
        double en_time = getTime(start_time);
        double rem_time = TL-0.2-en_time;
        avg_time += en_time - st_time;
        time_cnt++;

        if (avg_time / time_cnt * (M-i) > rem_time) {
            continue;
        }

        std::array<int, 2> vs{edge_arr[edge_order[i]].from, edge_arr[edge_order[i]].to};
        for (int v : vs) {
            for (const Edge& e : edges[v]) {
                if (edge_rorder[e.id] >= i) {
                    continue;
                }
                reset_color(edge_arr[e.id]);
                assign_color(edge_arr[e.id]);
            }
        }
    }
}

void update_color(double current_time, int sample_num) {
    int x = engine.rand() % M;
    int prev_color = answer::colors[x];
    int new_color = engine.rand() % D;

    for (int i = 0; prev_color == new_color || answer::counts[new_color] == K; i++) {
        new_color = engine.rand() % D;

        if (i == 100) {
            return;
        }
    }

    answer::colors[x] = new_color;

    long long diff = 0;
    std::array<long long, SAMPLE_NUM> diffs{};
    std::array<long long, SAMPLE_NUM> new_score_prev_colors;
    std::array<long long, SAMPLE_NUM> new_score_new_colors;
    int better_count = 0;
    std::array<int, SAMPLE_NUM> iotas;
    std::iota(iotas.begin(), iotas.begin()+sample_num, 0);
    /*
    std::sort(iotas.begin(), iotas.begin() + SAMPLE_NUM, [&](const int l, const int r) {
        return ideal_matrix[edge_arr[x].from][l] < ideal_matrix[edge_arr[x].from][r];
    });
    */
    for (int i = 1; i < SAMPLE_NUM; i++) {
        int x = engine.rand()%(i+1);
        std::swap(iotas[x], iotas[i]);
    }
    for (int i_ = 0; i_ < sample_num; i_++) {
        int i = iotas[i_];
        diffs[i] = answer::scores_per_color[prev_color][i] + answer::scores_per_color[new_color][i];
        new_score_prev_colors[i] = calc_score_color_add_edge(prev_color, edge_arr[x], i, dists_buffer2[i]);
        new_score_new_colors[i] = calc_score_color_remove_edge(new_color, edge_arr[x], i, dists_buffer[i]);
        diffs[i] -= new_score_prev_colors[i] + new_score_new_colors[i];
        diff += diffs[i];

        if (diffs[i] > 0) {
            better_count++;
        }
        if (diffs[i] < 0) {
            better_count--;
        }
        if (better_count < -3 && diff < 0) {
            answer::colors[x] = prev_color;
            return;
        }
    }

    if (diff <= 0) {
        answer::colors[x] = prev_color;
    } else {
        answer::counts[prev_color]--;
        answer::counts[new_color]++;
        for (int i = 0; i < sample_num; i++) {
            answer::scores_per_color[prev_color][i] = new_score_prev_colors[i];
            answer::scores_per_color[new_color][i] = new_score_new_colors[i];
        }
        answer::cached_dists[prev_color] = dists_buffer2;
        answer::cached_dists[new_color] = dists_buffer;
    }
}

#ifdef LOG
std::array<long long, 1000000> logs;
#endif

void refine_answer(unsigned long long start_time, int sample_num, double time_limit) {
    /*
    for (int color = 0; color < D; color++) {
        for (int i = 0; i < sample_num; i++) {
            answer::scores_per_color[color][i] = calc_score_color(color, samples[i], dists_buffer[i]);
        }
        answer::cached_dists[color] = dists_buffer;
    }
    */

    int loop = 0;
    while (getTime(start_time) < time_limit) {
        loop++;
        update_color(getTime(start_time), sample_num);
#ifdef LOG
        logs[loop-1] = answer::calc_estimated_score(N, D, sample_num, BASE_SAMPLE_SCORE);
#endif
    }
#ifdef LOG
    std::ofstream log_text(std::to_string(sample_num) + "log.txt");
    for (int i = 0; i < loop; i++) {
        log_text << logs[i] << "\n";
    }
    log_text.close();
#endif
    std::cerr << "while loop: " << loop << std::endl;
    std::cerr << "estimated_score: " << answer::calc_estimated_score(N, D, sample_num, BASE_SAMPLE_SCORE) << std::endl;
}

int check_reachability(const std::array<int, MAX_M>& color_states, int color) {
    std::array<bool, MAX_N> reached;
    reached.fill(false);
    reached[0] = true;
    std::queue<int> que;
    que.push(0);

    while (!que.empty()) {
        const auto w = que.front();
        que.pop();
        for (const Edge& e : edges[w]) {
            if (color_states[e.id] == color || reached[e.to]) {
                continue;
            }
            reached[e.to] = true;
            que.push(e.to);
        }
    }

    for (int i = 0; i < M; i++) {
        if (edge_arr[i].id != color) {
            continue;
        }
        if (reached[edge_arr[i].from] != reached[edge_arr[i].to]) {
            return i;
        }
    }

    return -1;
}

int find_neck_edge() {
    for (int color = 0; color < D; color++) {
        int e_id = check_reachability(answer::colors, color);
        if (e_id != -1) {
            return e_id;
        }
    }
    return -1;
}

void make_nodes_reachable(unsigned long long start_time) {
    while (getTime(start_time) < TL) {
        int neck_id = find_neck_edge();
        if (neck_id == -1) {
            break;
        }
        int y = engine.rand() % M;
        while (answer::colors[neck_id] == answer::colors[y]) {
            y = engine.rand() % M;
        }
        std::swap(answer::colors[neck_id], answer::colors[y]);
    }
}

void solve() {
    auto start_time = getCycle();

    initialize_answer(start_time);
#ifdef LOCAL
    std::cerr << "initialize_answer ends: " << std::endl;
    std::cerr << "time: " << getTime(start_time) << std::endl;
    std::cerr << answer::calc_estimated_score(N, D, SAMPLE_NUM, BASE_SAMPLE_SCORE) << std::endl;
#endif

    // make_nodes_reachable(start_time);
#ifdef LOCAL
    std::cerr << "make_nodes_reachable ends: " << std::endl;
    std::cerr << "time: " << getTime(start_time) << std::endl;
    std::cerr << answer::calc_estimated_score(N, D, SAMPLE_NUM, BASE_SAMPLE_SCORE) << std::endl;
    assert(getTime(start_time) < TL);
#endif

    refine_answer(start_time, SAMPLE_NUM, TL);
#ifdef LOCAL
    std::cerr << answer::calc_estimated_score(N, D, SAMPLE_NUM, BASE_SAMPLE_SCORE) << std::endl;
#endif
}

long long calc_accurate_score() {
    long long acc_score = 0;
    std::array<int, MAX_N> buffer;
    for (int color = 0; color < D; color++) {
        for (int i = 0; i < N; i++) {
            acc_score += calc_score_color(color, i, buffer);
        }
    }
    return acc_score - BASE_SCORE * D;
}

long long calc_normalized_score(long long score) {
    return std::round(1000 * score / (double)(N * (N-1) * D));
}

void output() {
    for (int i = 0; i < M; i++) {
        if (i == M-1) {
            std::cout << answer::colors[i] + 1 << std::endl;
        } else {
            std::cout << answer::colors[i] + 1 << " ";
        }
    }
#ifdef LOCAL
    long long score = calc_accurate_score();
    std::cerr << "save_csv: " << score << ", " << calc_normalized_score(score) << std::endl;
#endif
}


void init() {

    std::array<int, MAX_N> buffer;
    for (int i = 0; i < N; i++) {
        BASE_SCORE_PER_NODE[i] = calc_score_color(-2, i, buffer);
    }
    BASE_SCORE = std::accumulate(BASE_SCORE_PER_NODE.begin(), BASE_SCORE_PER_NODE.begin() + N, 0LL);
    samples[0] = find_closest_node(Node{500, 500});
    std::array<long long, MAX_N> sample_dists{};
    sample_dists[samples[0]] = -1;
    for (int i = 1; i < SAMPLE_NUM; i++) {
        for (int j = 0; j < N; j++) {
            if (sample_dists[j] < 0) {
                continue;
            }
            sample_dists[j] += ideal_matrix[j][samples[i-1]];
        }
        int next = std::max_element(sample_dists.begin(), sample_dists.begin()+N) - sample_dists.begin();
        sample_dists[next] = -1;
        samples[i] = next;
    }
    /*
    for (int i = 0; i < SAMPLE_NUM; i++) {
        int x = engine.rand()%N;
        while (std::find(samples.begin(), samples.begin()+i, x) != samples.begin()+i) {
            x = engine.rand()%N;
        }
        samples[i] = x;
    }
    */
    for (int i = 0; i < SAMPLE_NUM; i++) {
        BASE_SAMPLE_SCORE += BASE_SCORE_PER_NODE[samples[i]];
    }

    for (int i = 0; i < N; i++) {
        buffer.fill(UNREACH);
        buffer[i] = 0;
        global_pque.push(make_gpq_value(buffer[i], i));
        calc_dijkstra_sum_core(-2, buffer);
        for (int j = 0; j < N; j++) {
            ideal_matrix[i][j] = buffer[j];
        }
    }
}

int main() {
    input();

    init();

    solve();

    output();
}