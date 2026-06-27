
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <chrono>
#include <algorithm>
#include <random>
#include <map>
#include <set>
#include <queue>
#include <random>
#include <chrono>
#include <cmath>
#include <climits>
#include <bitset>
#include <time.h>
#include <cassert>

using namespace std;
using Ps = pair<short, short>;
using vec_int = vector<int>;
using P = pair<int, int>;
using P2 = pair<P, P>;
using P3 = pair<float, int>;
using Tu = tuple<int, int, int>;
using T2 = tuple<float, int, int>;
using T3 = tuple<float, int, int, int, int>;
using T4 = tuple<int, int, int, int>;
using T5 = tuple<int, int, int, int, int>;
using TT = tuple<int, int, int>;
using ll = long long;
using uc = unsigned char;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)

int N, M, T, LA, LB;
vec_int u, v;
vec_int t;
vec_int x, y;

struct dsu
{
public:
    dsu() : _n(0) {}
    explicit dsu(int n) : _n(n), parent_or_size(n, -1) {}

    int merge(int a, int b)
    {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        int x = leader(a), y = leader(b);
        if (x == y)
            return x;
        if (-parent_or_size[x] < -parent_or_size[y])
            std::swap(x, y);
        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;
        return x;
    }

    bool same(int a, int b)
    {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        return leader(a) == leader(b);
    }

    int leader(int a)
    {
        assert(0 <= a && a < _n);
        if (parent_or_size[a] < 0)
            return a;
        return parent_or_size[a] = leader(parent_or_size[a]);
    }

    int size(int a)
    {
        assert(0 <= a && a < _n);
        return -parent_or_size[leader(a)];
    }

    vector<int> belonging_group(int a)
    {
        vec_int result;
        int ld = leader(a);
        for (int i = 0; i < _n; i++)
        {
            if (leader(i) == ld)
            {
                result.push_back(i);
            }
        }
        return result;
    }

    std::vector<std::vector<int>> groups()
    {
        std::vector<int> leader_buf(_n), group_size(_n);
        for (int i = 0; i < _n; i++)
        {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<int>> result(_n);
        for (int i = 0; i < _n; i++)
        {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < _n; i++)
        {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [&](const std::vector<int> &v)
                           { return v.empty(); }),
            result.end());
        return result;
    }

private:
    int _n;
    // root node: -1 * component size
    // otherwise: parent
    std::vector<int> parent_or_size;
};

// #pragma GCC optimize("Ofast")


std::mt19937 mt{12345};
int RAND_ARR_LEN = 100000;
int RAND_RANGE = 1000000000;
float TIME_LIMIT = 5000;
float INV_TIME_LIMIT = 1. / 1000.;
const int JOB_MAX_LEN = 1003;
std::uniform_int_distribution<int> dist(1, RAND_RANGE);

int WAITING_TASK = 1001;

void remove_val_and_pop_from_last(vec_int &A, int val)
{
    for (int i = 0; i < A.size(); i++)
    {
        if (A.at(i) == val)
        {
            A.at(i) = A.at(A.size() - 1);
            A.pop_back();
            return;
        }
    }
    throw runtime_error("trying to remove non-existing value");
}

class Rand
{
private:
    int count = 0;
    vec_int rand_arr;

public:
    Rand(){
        rep(i, RAND_ARR_LEN){
            rand_arr.push_back(dist(mt));
}
}
;
int get();
int get_rand(int from, int to);
float get_float();
}
;

int Rand::get()
{
    int num = rand_arr.at(count);
    count += 1;
    count %= RAND_ARR_LEN;
    return num;
}

int Rand::get_rand(int from, int to)
{
    int diff = to - from;
    int num = get() % diff;
    return num + from;
}

float Rand::get_float()
{
    // 0~1の間の一様乱数
    return (float)get() / (float)RAND_RANGE;
}

Rand ro;

class PseudoSet
{
private:
    vec_int index_arr;

public:
    vec_int data;
    PseudoSet() {};
    PseudoSet(int value_range)
    {
        index_arr = vec_int(value_range, -1);
    }

    bool count(int value);
    void insert(int value);
    void erase(int value);
    vec_int get_data() { return data; };
    int size() { return data.size(); };
    int get_random() { return data.at(ro.get_rand(0, size())); };
};

bool PseudoSet::count(int value)
{
    return index_arr[value] != -1;
}

void PseudoSet::insert(int value)
{
    if (count(value))
        return;
    index_arr[value] = data.size();
    data.push_back(value);
}

void PseudoSet::erase(int value)
{
    if (!count(value))
    {
        // throw runtime_error("no existing value:"+to_string(value));
        return;
    }
    int tail_value = data[data.size() - 1];
    if (value == tail_value)
    {
        data.pop_back();
        index_arr[value] = -1;
    }
    else
    {
        index_arr[tail_value] = index_arr[value];
        index_arr[value] = -1;
        data[index_arr[tail_value]] = tail_value;
        data.pop_back();
    }
}

float get_time(bool init)
{
    static std::chrono::system_clock::time_point start;
    if (init)
    {
        start = std::chrono::system_clock::now();
    }
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); // 処理に要した時間をミリ秒に変換
}

double current_tmp(double current_time, double T0, double T1, double TIME_LIMIT)
{
    // return T1 + (T0 - T1) * (TIME_LIMIT - current_time) / (TIME_LIMIT);
    // double start_time = TIME_LIMIT * 0.6;
    double x = (current_time) / (TIME_LIMIT);
    return pow(T0, 1 - x) * pow(T1, x);
}

bool is_valid_transition(int diff, double current_time, double T0, float T1, double TIME_LIMIT)
{
    float t = current_tmp(current_time, T0, T1, TIME_LIMIT);
    float rand = ro.get_float();
    // diffが正だったら常にアクセぷと
    return rand < exp(((float)diff) / t);
}




bool is_connected(vec_int &path, vector<vec_int> &G)
{
    set<int> path_set = set<int>(path.begin(), path.end());
    queue<int> que;
    que.push(path[0]);
    set<int> visited;
    while (que.size() > 0)
    {
        int pos = que.front();
        que.pop();
        if (visited.count(pos))
            continue;
        visited.insert(pos);
        for (auto next_pos : G[pos])
        {
            if (path_set.count(next_pos) && !visited.count(next_pos))
            {
                que.push(next_pos);
            }
        }
    }
    return visited.size() == path_set.size();
}

int find_leaf_node(vector<vec_int> &G, vec_int &path)
{
    set<int> path_set = set<int>(path.begin(), path.end());
    int root = path[0];
    queue<int> que;
    que.push(root);
    set<int> visited;
    while (que.size() > 0)
    {
        int pos = que.front();
        que.pop();
        if (visited.count(pos))
            continue;
        visited.insert(pos);

        bool is_leaf = true;
        for (auto next_pos : G[pos])
        {
            if (path_set.count(next_pos) && !visited.count(next_pos))
            {
                is_leaf = false;
                que.push(next_pos);
            }
        }
        if (is_leaf)
        {
            return pos;
        }
    }
    throw "no leaf";
}

vec_int find_shortest_path(vector<vec_int> &G, int from, int to)
{
    vec_int result;
    vec_int prev(N, -1);
    queue<P> que;
    que.push(P(from, -2));

    while (que.size() > 0)
    {
        auto [pos, prev_pos] = que.front();
        que.pop();
        if (prev[pos] != -1)
            continue;
        prev[pos] = prev_pos;
        if (pos == to)
        {
            int tmp = pos;
            while (tmp != -2)
            {
                result.push_back(tmp);
                tmp = prev[tmp];
            }
            reverse(result.begin(), result.end());
            return result;
        }
        for (auto next_pos : G[pos])
        {
            if (prev[next_pos] == -1)
            {
                que.push(P(next_pos, pos));
            }
        }
    }
    throw "no path";
}

vec_int find_shortest_path2(vector<vec_int> &G, int from, int to, set<int> already_used)
{
    set<int> targets;
    rep(i, T)
    {
        targets.insert(t[i]);
    }
    vec_int result;
    vec_int prev(N, -1);
    priority_queue<Tu, vector<Tu>, greater<Tu>> que;
    que.push(Tu(0, from, -2));

    while (que.size() > 0)
    {
        auto [cost, pos, prev_pos] = que.top();
        que.pop();
        if (prev[pos] != -1)
            continue;
        prev[pos] = prev_pos;
        if (pos == to)
        {
            int tmp = pos;
            while (tmp != -2)
            {
                result.push_back(tmp);
                tmp = prev[tmp];
            }
            reverse(result.begin(), result.end());
            return result;
        }
        for (auto next_pos : G[pos])
        {
            int next_cost = cost + 1;
            if (already_used.count(next_pos))
                next_cost += 2;
            if (!targets.count(next_pos))
                next_cost += 2;
            if (prev[next_pos] == -1)
            {
                que.push(Tu(next_cost, next_pos, pos));
            }
        }
    }
    throw "no path";
}

int find_nearest_point(int x_pos, int y_pos)
{
    int nearest_dist = INT_MAX;
    int result = -1;
    rep(i, N)
    {
        int dist = abs(x[i] - x_pos) + abs(y[i] - y_pos);
        if (dist < nearest_dist)
        {
            nearest_dist = dist;
            result = i;
        }
    }
    return result;
}

P find_edge_center(vector<vec_int> &dist_pair)
{
    P min_max_dist = P(INT_MAX, INT_MAX);
    int best_pos = -1;
    for (int i = 0; i < N; i++)
    {
        int max_dist = 0;
        int tot_dist = 0;
        for (int j = 0; j < N; j++)
        {
            max_dist = max(max_dist, dist_pair[i][j]);
            tot_dist += dist_pair[i][j];
        }

        if (P(max_dist, tot_dist) < min_max_dist)
        {
            min_max_dist = P(max_dist, tot_dist);
            best_pos = i;
        }
    }
    return P(best_pos, min_max_dist.first);
}

int matched_node_size(vector<vec_int> &paths)
{

    vector<vec_int> common_node_count(paths.size(), vec_int(paths.size(), 0));
    for (int i = 0; i < paths.size(); i++)
    {
        set<int> path_set(paths[i].begin(), paths[i].end());
        for (int j = i + 1; j < paths.size(); j++)
        {
            if (paths[i].size() > LB && paths[j].size() > LB)
                continue;
            for (auto node : paths[j])
            {
                if (path_set.count(node))
                {
                    common_node_count[i][j]++;
                    common_node_count[j][i]++;
                }
            }
        }
    }

    // 上位から順にマージしていく
    set<int> merged;
    int tot_size = 0;
    vector<P> path_pairs;

    while (merged.size() + 1 < paths.size())
    {
        int largest_in_common = 0;
        P best_pair = P(-1, -1);
        rep(i, paths.size())
        {
            if (merged.count(i))
                continue;
            for (int j = i + 1; j < paths.size(); j++)
            {
                if (merged.count(j))
                    continue;

                if (common_node_count[i][j] > largest_in_common)
                {
                    largest_in_common = common_node_count[i][j];
                    best_pair = P(i, j);
                }
            }
        }
        auto [i, j] = best_pair;
        merged.insert(i);
        merged.insert(j);
        path_pairs.push_back(best_pair);
        tot_size += paths[i].size() + paths[j].size() - largest_in_common;
    }
    rep(i, paths.size())
    {
        if (!merged.count(i))
        {
            tot_size += paths[i].size();
        }
    }
    return tot_size;
}

vector<vec_int> erase_bad_cospa(vector<vec_int> &paths)
{

    vector<vec_int> tmp_paths = paths;

    while (true)
    {
        map<int, int> node_count_map;
        for (auto path : tmp_paths)
        {
            for (auto node : path)
            {
                node_count_map[node]++;
            }
        }
        int erase_index = -1;
        for (int i = 0; i < tmp_paths.size(); i++)
        {
            int one_count = 0;
            for (auto node : tmp_paths[i])
            {
                if (node_count_map[node] = 1)
                {
                    one_count++;
                }
            }
            if (one_count <= 1)
            {
                erase_index = i;
                break;
            }
        }
        if (erase_index == -1)
            break;
        tmp_paths.erase(tmp_paths.begin() + erase_index);
    }
    return tmp_paths;
}

int estimate_covering_size()
{
    double coeff1 = -1.64585218e+01;
    double coeff2 = -1.31474852e+00;
    double coeff3 = 1.91443422e-03;
    double coeff4 = -9.47548633e-07;
    double intercept = 1171.5305881564427;
    double x = M - 800;

    return intercept + (double)LB * coeff1 + x * coeff2 + x * x * coeff3 + x * x * x * coeff4 + 50;
}




struct Edge
{
    int start;
    int end;
    vector<int> path;
    vec_int get_route(int from, int to, vector<vec_int> &G)
    {
        set<int> nodes;
        rep(i, path.size())
        {
            nodes.insert(path[i]);
        }
        vec_int reuslt;
        queue<P> que;
        map<int, int> prev;
        que.push(P(from, -1));
        while (que.size() > 0)
        {
            auto [pos, prev_pos] = que.front();
            que.pop();
            if (prev.count(pos) > 0)
                continue;
            prev[pos] = prev_pos;
            if (pos == to)
            {
                int tmp = pos;
                while (tmp != -1)
                {
                    reuslt.push_back(tmp);
                    tmp = prev[tmp];
                }
                reverse(reuslt.begin(), reuslt.end());
                return reuslt;
            }
            for (auto next_pos : G[pos])
            {
                if (nodes.count(next_pos) > 0 && prev.count(next_pos) == 0)
                {
                    que.push(P(next_pos, pos));
                }
            }
        }
        cerr << from << " " << to << endl;
        cerr << path.size() << endl;
        for (auto node : path)
        {
            cerr << node << " ";
        }
        cerr << endl;
        cerr << x[from] << " " << y[from] << " " << x[to] << " " << y[to] << endl;
        throw runtime_error("no route is found");
        /*
        int from_index = find(path.begin(), path.end(), from) - path.begin();
        int to_index = find(path.begin(), path.end(), to) - path.begin();

        int min_index = min(from_index, to_index);
        int max_index = max(from_index, to_index);

        vec_int result = vec_int(path.begin() + min_index, path.begin() + max_index + 1);
        if (from_index > to_index)
        {
            reverse(result.begin(), result.end());
        }
        return result;
        */
    }
    bool is_connected(vector<vec_int> &G)
    {
        set<int> node_set = set<int>(path.begin(), path.end());
        queue<int> que;
        set<int> visited;
        que.push(start);

        while (que.size() > 0)
        {
            int pos = que.front();
            que.pop();
            if (visited.count(pos) > 0)
                continue;
            visited.insert(pos);
            for (auto next_pos : G[pos])
            {
                if (visited.count(next_pos) == 0 && node_set.count(next_pos) > 0)
                {
                    que.push(next_pos);
                }
            }
        }
        return visited.size() == node_set.size();
    }
};



struct Line
{
    vector<int> path;
    set<int> path_set;
    int start_on_A_vec;
    bool has_node(int node) { return path_set.count(node) > 0; };
};

struct TranslatedAvecAndEdges
{
    vector<Edge> edges;
    vec_int A_vec;
    vec_int start_on_A_vec;
};

void merge_paths(vec_int path1, vec_int path2, vector<vec_int> &G, vec_int &A_vec, vector<Line> &lines)
{
    if (path1.size() < path2.size())
    {
        swap(path1, path2);
    }
    int center_node = path1[0];
    int leaf_node1 = find_leaf_node(G, path1);
    int leaf_node2 = find_leaf_node(G, path2);

    set<int> path1_set = set<int>(path1.begin(), path1.end());
    set<int> path2_set = set<int>(path2.begin(), path2.end());
    set<int> intersection = set<int>();
    for (auto node : path2)
    {
        if (path1_set.count(node) && node != leaf_node1 && node != leaf_node2)
        {
            intersection.insert(node);
        }
    }

    vec_int new_path1 = {center_node};
    vec_int new_path2 = {center_node};
    if (path1[0] != path2[0])
    {
        new_path1 = {};
        new_path2 = {};
    }
    for (auto node : intersection)
    {
        if (node != center_node || path1[0] != path2[0])
        {
            new_path1.push_back(node);
            new_path2.push_back(node);
        }
    }

    // 共通部分でないかつleafでないノードをそれぞれ追加
    for (auto node : path1)
    {
        if (!intersection.count(node) && node != leaf_node1)
        {
            new_path1.push_back(node);
        }
    }
    for (auto node : path2)
    {
        if (!intersection.count(node) && node != leaf_node2)
        {
            new_path2.push_back(node);
        }
    }
    new_path1.push_back(leaf_node1);
    new_path2.push_back(leaf_node2);

    int common_length = intersection.size();

    int pos1 = A_vec.size();
    int pos2 = A_vec.size() + new_path1.size() - common_length;

    reverse(new_path1.begin(), new_path1.end());

    for (int i = 0; i < new_path1.size(); i++)
    {
        A_vec.push_back(new_path1[i]);
    }
    for (int i = common_length; i < new_path2.size(); i++)
    {
        A_vec.push_back(new_path2[i]);
    }

    if (new_path1.size() > LB)
    {
        vec_int vec1;
        vec_int vec2;
        rep(i, LB)
        {
            vec1.push_back(new_path1[i]);
            vec2.push_back(new_path1[i + 1]);
        }

        set<int> path1_set = set<int>(vec1.begin(), vec1.end());
        set<int> path2_set = set<int>(vec2.begin(), vec2.end());
        lines.push_back(Line{vec1, path1_set, pos1});
        lines.push_back(Line{vec2, path2_set, pos1 + 1});
    }
    else
    {
        set<int> path1_set = set<int>(new_path1.begin(), new_path1.end());
        lines.push_back(Line{new_path1, path1_set, pos1});
    }

    if (new_path2.size() > LB)
    {
        vec_int vec1;
        vec_int vec2;
        rep(i, LB)
        {
            vec1.push_back(A_vec[pos2 + i]);
            vec2.push_back(A_vec[pos2 + i + 1]);
        }

        set<int> path1_set = set<int>(vec1.begin(), vec1.end());
        set<int> path2_set = set<int>(vec2.begin(), vec2.end());
        lines.push_back(Line{vec1, path1_set, pos2});
        lines.push_back(Line{vec2, path2_set, pos2 + 1});
    }
    else
    {
        set<int> path2_set = set<int>(new_path2.begin(), new_path2.end());
        lines.push_back(Line{new_path2, path2_set, pos2});
    }
}

pair<vector<P>, vec_int> match_paths(vector<vec_int> &paths)
{

    vector<vec_int> common_node_count(paths.size(), vec_int(paths.size(), 0));
    for (int i = 0; i < paths.size(); i++)
    {
        set<int> path_set(paths[i].begin(), paths[i].end());
        for (int j = i + 1; j < paths.size(); j++)
        {
            if (paths[i].size() > LB && paths[j].size() > LB)
                continue;

            if (paths[i][0] != paths[j][0])
                continue;
            for (auto node : paths[j])
            {
                if (path_set.count(node))
                {
                    common_node_count[i][j]++;
                    common_node_count[j][i]++;
                }
            }
        }
    }

    // 上位から順にマージしていく
    set<int> merged;
    int tot_size = 0;
    vector<P> path_pairs;

    while (merged.size() + 1 < paths.size())
    {
        int largest_in_common = 0;
        P best_pair = P(-1, -1);
        rep(i, paths.size())
        {
            if (merged.count(i))
                continue;
            for (int j = i + 1; j < paths.size(); j++)
            {
                if (merged.count(j))
                    continue;

                if (common_node_count[i][j] > largest_in_common)
                {
                    largest_in_common = common_node_count[i][j];
                    best_pair = P(i, j);
                }
            }
        }
        if (largest_in_common == 0)
            break;
        auto [i, j] = best_pair;
        merged.insert(i);
        merged.insert(j);
        path_pairs.push_back(best_pair);
    }
    int last = -1;
    vec_int ret_vec;
    rep(i, paths.size())
    {
        if (merged.count(i))
            continue;
        ret_vec.push_back(i);
    }
    return {path_pairs, ret_vec};
}

tuple<vector<Edge>, vec_int, vec_int> make_edges(vector<vec_int> &G, vector<vec_int> &dist_pair, vector<vec_int> paths)
{
    auto [path_pairs, single_vec] = match_paths(paths);
    vec_int A_vec;
    vector<Line> lines;
    for (auto path_pair : path_pairs)
    {
        auto [i, j] = path_pair;
        merge_paths(paths[i], paths[j], G, A_vec, lines);
    }
    for (int last : single_vec)
    {
        auto leaf_node = find_leaf_node(G, paths[last]);
        auto tmp_path = paths[last];
        tmp_path.erase(find(tmp_path.begin(), tmp_path.end(), leaf_node));
        tmp_path.push_back(leaf_node);

        if (tmp_path.size() > LB)
        {
            vec_int vec1;
            vec_int vec2;
            rep(i, LB)
            {
                vec1.push_back(tmp_path[i]);
                vec2.push_back(tmp_path[i + 1]);
            }

            set<int> path1_set = set<int>(vec1.begin(), vec1.end());
            set<int> path2_set = set<int>(vec2.begin(), vec2.end());
            lines.push_back(Line{vec1, path1_set, (int)A_vec.size()});
            lines.push_back(Line{vec2, path2_set, (int)A_vec.size() + 1});
        }
        else
        {
            set<int> tmp_path_set = set<int>(tmp_path.begin(), tmp_path.end());
            lines.push_back(Line{tmp_path, tmp_path_set, (int)A_vec.size()});
        }

        for (auto node : tmp_path)
        {
            A_vec.push_back(node);
        }
    }

    vector<Edge> edges;
    vec_int edge_starts;
    for (auto line : lines)
    {
        vec_int path = line.path;
        edges.push_back(Edge{path[0], path[path.size() - 1], path});
        edge_starts.push_back(line.start_on_A_vec);
    }
    return make_tuple(edges, A_vec, edge_starts);
}

TranslatedAvecAndEdges match_edges_and_make_A_vec(vector<vec_int> &paths, vector<vec_int> &G, vector<vec_int> &dist_pair, vector<Edge> &current_edges, vec_int &current_A_vec, vec_int &current_edge_starts)
{
    auto edges = current_edges;
    auto A_vec = current_A_vec;
    auto edge_starts = current_edge_starts;
    auto [additional_edges, new_A_vec, new_edge_starts] = make_edges(G, dist_pair, paths);
    cerr << "additional_edges.size():" << additional_edges.size() << endl;
    rep(i, new_edge_starts.size()) new_edge_starts[i] += A_vec.size();

    edges.insert(edges.end(), additional_edges.begin(), additional_edges.end());
    A_vec.insert(A_vec.end(), new_A_vec.begin(), new_A_vec.end());
    edge_starts.insert(edge_starts.end(), new_edge_starts.begin(), new_edge_starts.end());
    cerr << "ok" << endl;

    return TranslatedAvecAndEdges{edges, A_vec, edge_starts};
}

vector<pair<PseudoSet, int>> find_tree_centers(vector<vec_int> &G, vector<vec_int> &dist_pairs, int LB)
{
    vector<pair<PseudoSet, int>> ret;

    PseudoSet all_used_nodes(N);

    while (all_used_nodes.size() < N)
    {
        int best_center = -1;
        int best_count = 0;
        rep(i, N)
        {
            if (all_used_nodes.count(i))
                continue;

            int count = 0;

            rep(j, N)
            {
                if (all_used_nodes.count(j))
                    continue;
                if (dist_pairs[i][j] < LB)
                {
                    count++;
                }
            }
            if (count > best_count)
            {
                best_count = count;
                best_center = i;
            }
        }

        PseudoSet node_groups(N);
        rep(j, N)
        {
            if (dist_pairs[best_center][j] < LB)
            {
                node_groups.insert(j);
            }
        }

        dsu uf(N);
        for (auto node : node_groups.data)
        {
            for (auto next_node : G[node])
            {
                if (node_groups.count(next_node))
                {
                    uf.merge(node, next_node);
                }
            }
        }

        while (true)
        {
            bool flag = true;
            for (auto node : node_groups.data)
            {
                // 連結でないとき
                if (!uf.same(node, best_center))
                {
                    flag = false;

                    int nearest_length = INT_MAX;
                    int nearest_node = -1;
                    for (auto nd : node_groups.data)
                    {
                        if (nd == node)
                            continue;
                        if (!uf.same(nd, best_center))
                            continue;
                        if (dist_pairs[node][nd] < nearest_length)
                        {
                            nearest_length = dist_pairs[node][nd];
                            nearest_node = nd;
                        }
                    }

                    // nearest nodeまで行く
                    auto path = find_shortest_path(G, node, nearest_node);
                    for (auto p : path)
                    {
                        node_groups.insert(p);
                        for (auto next_node : G[p])
                        {
                            uf.merge(p, next_node);
                        }
                    }
                    break;
                }
            }
            if (flag)
                break;
        }
        for (auto node : node_groups.data)
        {
            all_used_nodes.insert(node);
        }

        ret.push_back({node_groups, best_center});
    }
    return ret;
}




PseudoSet find_nodes_to_use(vector<vec_int> &G, vector<vec_int> &dist_pairs)
{
    // oset<int> used_node_set(t.begin(), t.end());
    PseudoSet used_node_set(N);
    for (auto node : t)
    {
        used_node_set.insert(node);
    }
    used_node_set.insert(0);
    vector<int> target_list = used_node_set.data;
    dsu uf_tree(N);
    rep(i, M)
    {
        if (used_node_set.count(u[i]) > 0 && used_node_set.count(v[i]) > 0)
        {
            uf_tree.merge(u[i], v[i]);
        }
    }

    int first_target = t[0];

    vec_int added_nodes;
    set<int> added_node_set;

    // これで使用するノードの組みを定義した
    /**/
    while (true)
    {
        bool ok = true;
        for (auto node : target_list)
        {
            if (uf_tree.same(first_target, node) == false)
            {
                ok = false;
                break;
            }
        }
        if (ok)
        {
            break;
        }

        // 幅優先探索でfirst_targetの所属しているグループから最も短い経路で別のグループに行けるノードの組みを作る
        auto group = uf_tree.belonging_group(first_target);
        vec_int prev(N, -1);
        queue<P> que;

        for (auto node : group)
        {
            auto neighbors = G[node];
            shuffle(neighbors.begin(), neighbors.end(), mt);
            for (auto neighbor : neighbors)
            {
                if (!used_node_set.count(neighbor))
                {
                    que.push(P(neighbor, node));
                }
            }
        }

        vec_int new_nodes;

        while (!que.empty())
        {

            auto [node, prev_node] = que.front();
            que.pop();
            if (prev[node] != -1)
                continue;
            prev[node] = prev_node;

            // こいつの隣接するノードがused_node_setに含まれていてかつuf_treeがfirst_targetとsameのでない場合にok
            bool flag = false;
            for (auto neighbor : G[node])
            {
                if (!uf_tree.same(first_target, neighbor) && used_node_set.count(neighbor))
                {
                    flag = true;
                    break;
                }
            }
            if (flag)
            {
                int nd = node;
                while (nd != -1)
                {
                    new_nodes.push_back(nd);
                    used_node_set.insert(nd);
                    nd = prev[nd];
                }
                break;
            }
            auto neighbors = G[node];
            shuffle(neighbors.begin(), neighbors.end(), mt);

            for (auto neighbor : neighbors)
            {
                if (prev[neighbor] == -1 && !used_node_set.count(neighbor))
                {
                    que.push(P(neighbor, node));
                }
            }
        }
        // 新しくくわったノードについてuftreeを更新する
        for (auto node : new_nodes)
        {
            for (auto neighbor : G[node])
            {
                if (used_node_set.count(neighbor))
                {
                    uf_tree.merge(node, neighbor);
                }
            }
        }
    }
    /*
    rep(i, N)
    {
        if (300 <= x[i] && x[i] <= 700 && 300 <= y[i] && y[i] <= 700)
        {
            used_node_set.insert(i);
        }
    }
    */
    return used_node_set;
}

vector<P> prev_for_a_good_path = vector<P>(600, P(-1, -1));
int path_counter = 0;

vec_int find_a_good_path(vector<vec_int> &G, PseudoSet &used_node_set, set<int> added_node_set, int start, int end, vector<vec_int> &dist_pairs)
{
    path_counter++;
    // 極力既存で使用している箇所を避けながらパスを繋ぐ
    // 追加した場所 or targetでないノードは+0.2する

    priority_queue<tuple<double, double, int, int>, vector<tuple<double, double, int, int>>, greater<tuple<double, double, int, int>>> pq;
    pq.push({(double)dist_pairs[start][end] * 0.99, 0, start, -2});

    // A*探索をする

    while (pq.size() > 0)
    {
        auto [estimate, cost, pos, prev_pos] = pq.top();
        pq.pop();
        if (prev_for_a_good_path[pos].first == path_counter)
            continue;

        prev_for_a_good_path[pos] = {path_counter, prev_pos};
        if (pos == end)
        {
            vec_int path;
            while (pos != -2)
            {
                path.push_back(pos);
                pos = prev_for_a_good_path[pos].second;
            }
            reverse(path.begin(), path.end());
            return path;
        }
        for (auto next : G[pos])
        {
            double next_cost = cost + 1;
            if (!used_node_set.count(next))
            {
                next_cost += 0.1;
            }
            if (added_node_set.count(next))
            {
                next_cost += 0.1;
            }
            double estimate = (double)dist_pairs[next][end] * 0.99 + next_cost;
            pq.push({estimate, next_cost, next, pos});
        }
    }
    throw "not found";
}

tuple<vector<Edge>, vec_int, vec_int> additional_edges_long_pathes(vector<vec_int> &G, vector<vec_int> &dist_pairs)
{
    PseudoSet used_node_set = find_nodes_to_use(G, dist_pairs);

    // 考え方としては
    // 長い道に関して最短経路を見つけてedgeに入れる
    // 使い切るとまずいので、残りの回数がまだ道に入っていないused_node_setの数より少なくならないようにやる

    vector<Edge> edges;

    int added_node_count = 0;
    set<int> added_used_node_set;
    bool end_flag = false;
    vector<vec_int> paths;

    vector<int> last_selected(N, -100);
    vector<vec_int> path_caches(N, vec_int(N, -1));
    vec_int last_calc_turn(N, -100);

    set<int> used_turn;
    rep(ii, T)
    {
        int turn = -1;

        int best_score = -100;
        vec_int vec;
        rep(i, T - 1) vec.push_back(i);
        // shuffle(vec.begin(), vec.end(), mt);
        vector<P> records;
        rep(ttt, T - 1)
        {
            // 新規パスを通った数が多いやつを選ぶ
            int tn = vec[ttt];
            if (used_turn.count(tn))
                continue;
            int from = tn == 0 ? 0 : t[tn - 1];
            int to = t[tn];
            vec_int tmp_path;
            if (last_calc_turn[tn] > ii - 4)
            {
                tmp_path = path_caches[tn];
            }
            else
            {
                tmp_path = find_a_good_path(G, used_node_set, added_used_node_set, from, to, dist_pairs);
                path_caches[tn] = tmp_path;
                last_calc_turn[tn] = ii;
            }
            int score = 0;

            if (!added_used_node_set.count(from) && !added_used_node_set.count(to))
            {
                score += 2;
            }
            if (added_used_node_set.count(from) && added_used_node_set.count(to))
            {
                score -= 3;
            }

            for (auto p : tmp_path)
            {
                if (used_node_set.count(p) && !added_used_node_set.count(p))
                {
                    score++;
                }
            }
            records.push_back({score, tn});
            if (score > best_score)
            {
                best_score = score;
                turn = tn;
            }
            last_selected[tn] = ii;
        }
        vec_int candidates;
        for (auto record : records)
        {
            if (record.first == best_score)
            {
                candidates.push_back(record.second);
            }
        }
        turn = candidates[ro.get_rand(0, candidates.size()) % candidates.size()];

        // int turn = dist_t[ii].second;
        int from = turn == 0 ? 0 : t[turn - 1];
        int to = t[turn];
        /// vec_int path = find_shortest_path(G, from, to);

        vec_int path = find_a_good_path(G, used_node_set, added_used_node_set, from, to, dist_pairs);

        vec_int path2;
        for (auto p : path)
        {
            added_node_count++;
            path2.push_back(p);
            if (used_node_set.count(p) > 0)
            {
                added_used_node_set.insert(p);
            }
            int nokori = used_node_set.size() - added_used_node_set.size();
            if (LA - added_node_count == nokori)
            {
                end_flag = true;
                break;
            }
        }
        int sz = path2.size() / LB;
        if (path2.size() % LB != 0)
            sz++;

        /*
        rep(j, sz)
        {
            int start = path2[j * LB];
            int to_pos = min((j + 1) * LB, (int)path2.size());
            int end = path2[to_pos];
            edges.push_back({start, end, vec_int(path2.begin() + j * LB, path2.begin() + to_pos)});
        }
        */
        paths.push_back(path2);

        if (end_flag)
        {
            break;
        }
    }
    // cerr << "added_node_count:" << added_node_count << " " << used_node_set.size() - added_used_node_set.size() << " " << LA - added_node_count << endl;
    // あとは残りを適当に追加していく

    vector<int> visited(N, 0);
    for (auto path : paths)
    {
        for (auto p : path)
        {
            visited[p] = 1;
        }
    }
    cout << endl;
    PseudoSet ps(N);
    rep(i, N)
    {
        if (used_node_set.count(i) > 0 && visited[i] == 0)
        {
            ps.insert(i);
        }
    }

    while (true)
    {
        int start;
        while (true)
        {
            int len = ps.size();
            if (len == 0)
                break;
            start = ps.data[ro.get_rand(0, len) % len];
            if (visited[start] == 0)
                break;

            ps.erase(start);
            if (ps.size() == 0)
                break;
        }
        if (ps.size() == 0)
            break;

        vec_int path;
        path.push_back(start);
        visited[start] = 1;
        int pos = start;
        while (path.size() < LB)
        {
            // startから生やしていく
            vector<P> nexts;
            for (auto tmp_pos : path)
            {
                for (auto pos2 : G[tmp_pos])
                {
                    if (used_node_set.count(pos2) == 0)
                        continue;
                    if (visited[pos2] == 0)
                    {
                        int tot_cost = 0;
                        for (auto p : path)
                        {
                            tot_cost += dist_pairs[p][pos2];
                        }

                        nexts.push_back(P(tot_cost, pos2));
                    }
                }
            }
            if (nexts.size() == 0)
                break;

            sort(nexts.begin(), nexts.end());
            reverse(nexts.begin(), nexts.end());
            auto next = nexts[0].second;

            path.push_back(next);
            pos = next;
            visited[pos] = 1;
        }
        int end = pos;

        // 次に始める候補点を追加する
        for (auto p : path)
        {
            for (auto p2 : G[p])
            {
                if (visited[p2] == 0 && used_node_set.count(p2) > 0)
                {
                    ps.insert(p2);
                }
            }
        }
        edges.push_back({start, end, path});
    }

    vector<int> edge_starts;
    vec_int A_vec(LA, 0);
    int index = 0;
    rep(i, edges.size())
    {
        edge_starts.push_back(index);
        for (auto p : edges[i].path)
        {
            A_vec[index] = p;
            index++;
        }
    }

    // 長いパスをまとめて登録する

    for (auto path : paths)
    {
        int tmp_index = index;
        for (auto p : path)
        {
            A_vec[index] = p;
            index++;
        }
        // 一個ずつずらしてedgeに登録する
        for (int start_pos = 0; start_pos < max((int)path.size() - LB + 1, 1); start_pos++)
        {
            vec_int edge_path;
            for (int j = start_pos; j < min(start_pos + LB, (int)path.size()); j++)
            {
                edge_path.push_back(path[j]);
            }
            edges.push_back({edge_path[0], edge_path.back(), edge_path});
            edge_starts.push_back(tmp_index + start_pos);
        }
    }
    cerr << index << " LA:" << LA << endl;

    return make_tuple(edges, edge_starts, A_vec);

    // return edges;
}

/**/
class EdgeStrategyAnnealingState
{
private:
    vector<vec_int> G;
    vector<vec_int> dist_pairs;
    void prepare_initial_state();
    // ここは最短経路を探す際の配列のキャッシュ
    vector<P> prev_pos_cache = vector<P>(N, P(-1, -1));
    int counter = 0;

public:
    PseudoSet used_node_set;
    PseudoSet edge_ids;                    // 管理対象
    PseudoSet added_used_node_set;         // 管理対象
    vector<vec_int> paths;                 // 管理対象
    vector<int> node_belonging_path_count; // 管理対象
    int added_node_count = 0;              // 管理対象
    EdgeStrategyAnnealingState(vector<vec_int> &G, vector<vec_int> &dist_pairs, PseudoSet &used_node_set) : G(G), dist_pairs(dist_pairs), used_node_set(used_node_set)
    {
        edge_ids = PseudoSet(T);
        paths = vector<vec_int>(T);
        node_belonging_path_count = vector<int>(N, 0);
        added_used_node_set = PseudoSet(N);
        prepare_initial_state();
    };
    vec_int find_a_good_path(int from, int to);
    vector<vec_int> make_paths()
    {
        vector<vec_int> res;
        for (auto edge_id : edge_ids.data)
        {
            res.push_back(paths[edge_id]);
        }
        return res;
    }
    void erase_edge(int edge_id)
    {
        edge_ids.erase(edge_id);
        added_node_count -= paths[edge_id].size();
        for (auto p : paths[edge_id])
        {
            node_belonging_path_count[p]--;
            if (used_node_set.count(p) && node_belonging_path_count[p] == 0)
            {
                added_used_node_set.erase(p);
            }
        }
    }
    void append_edge(int edge_id)
    {
        edge_ids.insert(edge_id);
        auto path = find_a_good_path(edge_id == 0 ? 0 : t[edge_id - 1], t[edge_id]);
        paths[edge_id] = path;
        added_node_count += path.size();
        for (auto p : path)
        {
            if (used_node_set.count(p))
            {
                added_used_node_set.insert(p);
            }
            node_belonging_path_count[p]++;
        }
    }
    void append_edge_with_path(int edge_id, vec_int &path)
    {
        edge_ids.insert(edge_id);
        paths[edge_id] = path;
        added_node_count += path.size();
        for (auto p : path)
        {
            if (used_node_set.count(p))
            {
                added_used_node_set.insert(p);
            }
            node_belonging_path_count[p]++;
        }
    }
};

void EdgeStrategyAnnealingState::prepare_initial_state()
{
    // 初期に使用するedgeの組みを決める
    // この際に、使用したエッジのノード数の合計 + 使う予定で未使用のノード数がLAを超えないようにする

    bool end_flag = false;

    vector<int> last_selected(N, -100);
    vector<vec_int> path_caches(N, vec_int(N, -1));
    vec_int last_calc_turn(N, -100);

    rep(ii, T)
    {
        int turn = -1;

        int best_score = -1;
        vec_int vec;
        rep(i, T - 1) vec.push_back(i);
        // shuffle(vec.begin(), vec.end(), mt);
        vector<P> records;
        rep(ttt, T - 1)
        {
            if (edge_ids.count(ttt))
                continue;
            // 新規パスを通った数が多いやつを選ぶ
            int tn = vec[ttt];
            int from = tn == 0 ? 0 : t[tn - 1];
            int to = t[tn];
            vec_int tmp_path;
            if (last_calc_turn[tn] > ii - 2)
            {
                tmp_path = path_caches[tn];
            }
            else
            {
                tmp_path = find_a_good_path(from, to);
                path_caches[tn] = tmp_path;
                last_calc_turn[tn] = ii;
            }
            int score = 0;
            for (auto p : tmp_path)
            {
                if (used_node_set.count(p) && !added_used_node_set.count(p))
                {
                    score++;
                }
            }
            records.push_back({score, tn});
            if (score > best_score)
            {
                best_score = score;
                turn = tn;
            }
            last_selected[tn] = ii;
        }
        vec_int candidates;
        for (auto record : records)
        {
            if (record.first == best_score)
            {
                candidates.push_back(record.second);
            }
        }

        // この中でpath.length -2 をLBで割った余りが最も大きいものを選ぶ
        int max_mod = -1;
        for (auto c : candidates)
        {
            int mod = (path_caches[c].size() - 2) % LB;
            if (mod > max_mod)
            {
                max_mod = mod;
                turn = c;
            }
        }

        int from = turn == 0 ? 0 : t[turn - 1];
        int to = t[turn];

        vec_int path = path_caches[turn];

        int nokori = used_node_set.size() - added_used_node_set.size();
        int res_operation = LA - added_node_count - path.size();
        if (nokori >= res_operation)
        {
            cerr << "nokori:" << nokori << " res_operation:" << res_operation << " added_node_count" << added_node_count << endl;
            cerr << path.size() << endl;
            break;
        }

        edge_ids.insert(turn);
        added_node_count += path.size();
        for (auto p : path)
        {
            if (used_node_set.count(p))
            {
                added_used_node_set.insert(p);
            }
            node_belonging_path_count[p]++;
        }
        paths[turn] = path;
    }
}

vec_int EdgeStrategyAnnealingState::find_a_good_path(int from, int to)
{
    counter++;
    priority_queue<tuple<double, double, int, int>, vector<tuple<double, double, int, int>>, greater<tuple<double, double, int, int>>> pq;
    pq.push({(double)dist_pairs[from][to] * 0.99, 0, from, -2});

    while (pq.size() > 0)
    {
        auto [estimate, cost, pos, prev_pos] = pq.top();
        pq.pop();
        if (prev_pos_cache[pos].first == counter)
            continue;

        prev_pos_cache[pos] = {counter, prev_pos};
        if (pos == to)
        {
            vec_int path;
            while (pos != -2)
            {
                path.push_back(pos);
                pos = prev_pos_cache[pos].second;
            }
            reverse(path.begin(), path.end());
            return path;
        }
        for (auto next : G[pos])
        {
            double next_cost = cost + 1;
            if (used_node_set.count(next) == 0)
            {
                next_cost += 0.4;
            }
            if (node_belonging_path_count[next] > 0)
            {
                next_cost += 0.3;
            }
            double estimate = (double)dist_pairs[next][to] * 0.99 + next_cost;
            pq.push({estimate, next_cost, next, pos});
        }
    }
    throw "not found";
}

class EdgeStrategyAnnealing
{
public:
    PseudoSet used_node_set;
    EdgeStrategyAnnealing() {}
    EdgeStrategyAnnealingState prepare_initial_state(vector<vec_int> &G, vector<vec_int> &dist_pairs)
    {
        used_node_set = find_nodes_to_use(G, dist_pairs);
        return EdgeStrategyAnnealingState(G, dist_pairs, used_node_set);
    }
    tuple<vector<Edge>, vec_int, vec_int> annealing(vector<vec_int> &G, vector<vec_int> &dist_pairs);
};

tuple<vector<Edge>, vec_int, vec_int> EdgeStrategyAnnealing::annealing(vector<vec_int> &G, vector<vec_int> &dist_pairs)
{
    auto state = prepare_initial_state(G, dist_pairs);

    int score = state.added_used_node_set.size();
    cerr << "initial:" << " " << score << " " << state.added_node_count << endl;

    while (true)
    {
        if (get_time(false) > 2000)
        {
            break;
        }
        int rnd = ro.get_rand(0, 2);
        if (rnd == 0)
        {
            int erase_edge_id = state.edge_ids.data[ro.get_rand(0, state.edge_ids.size())];
            int add_edge_id;
            int cnt = 0;
            while (true)
            {

                add_edge_id = ro.get_rand(0, T);
                if (state.edge_ids.count(add_edge_id))
                {
                    cnt++;
                    continue;
                }
                break;
            }

            int old_score = state.added_used_node_set.size();
            auto old_path = state.paths[erase_edge_id];
            state.erase_edge(erase_edge_id);
            state.append_edge(add_edge_id);

            int new_score = state.added_used_node_set.size();
            int nokori = state.used_node_set.size() - state.added_used_node_set.size();

            if (new_score >= old_score && LA - state.added_node_count >= nokori)
            {
                score = new_score;
            }
            else
            {
                state.erase_edge(add_edge_id);
                state.append_edge_with_path(erase_edge_id, old_path);
            }
        }
        else
        {
            int erase_edge_id = state.edge_ids.data[ro.get_rand(0, state.edge_ids.size())];

            int old_score = state.added_used_node_set.size();
            auto old_path = state.paths[erase_edge_id];
            state.erase_edge(erase_edge_id);
            state.append_edge(erase_edge_id);

            int new_score = state.added_used_node_set.size();

            int nokori = state.used_node_set.size() - state.added_used_node_set.size();
            if (new_score >= old_score && LA - state.added_node_count >= nokori)
            {
                score = new_score;
            }
            else
            {
                state.erase_edge(erase_edge_id);
                state.append_edge_with_path(erase_edge_id, old_path);
            }
        }
    }

    cerr << "end:" << " " << score << " " << state.added_node_count << endl;
    auto paths = state.make_paths();
    auto added_used_node_set = set<int>(state.added_used_node_set.data.begin(), state.added_used_node_set.data.end());
    int added_node_count = state.added_node_count;

    bool end_flag = false;

    rep(ii, T)
    {
        int turn = -1;

        int best_score = -1;
        vec_int vec;
        rep(i, T - 1) vec.push_back(i);
        // shuffle(vec.begin(), vec.end(), mt);
        vector<P> records;
        rep(ttt, T - 1)
        {
            // 新規パスを通った数が多いやつを選ぶ
            int tn = vec[ttt];
            int from = tn == 0 ? 0 : t[tn - 1];
            int to = t[tn];
            vec_int tmp_path = find_a_good_path(G, used_node_set, added_used_node_set, from, to, dist_pairs);
            int score = 0;
            for (auto p : tmp_path)
            {
                if (used_node_set.count(p) && !added_used_node_set.count(p))
                {
                    score++;
                }
            }
            records.push_back({score, tn});
            if (score > best_score)
            {
                best_score = score;
                turn = tn;
            }
        }
        vec_int candidates;
        for (auto record : records)
        {
            if (record.first == best_score)
            {
                candidates.push_back(record.second);
            }
        }
        turn = candidates[ro.get_rand(0, candidates.size()) % candidates.size()];

        // int turn = dist_t[ii].second;
        int from = turn == 0 ? 0 : t[turn - 1];
        int to = t[turn];
        /// vec_int path = find_shortest_path(G, from, to);

        vec_int path = find_a_good_path(G, used_node_set, added_used_node_set, from, to, dist_pairs);

        vec_int path2;
        for (auto p : path)
        {
            added_node_count++;
            path2.push_back(p);
            if (used_node_set.count(p) > 0)
            {
                added_used_node_set.insert(p);
            }
            int nokori = used_node_set.size() - added_used_node_set.size();
            if (LA - added_node_count <= nokori)
            {
                end_flag = true;
                break;
            }
        }
        int sz = path2.size() / LB;
        if (path2.size() % LB != 0)
            sz++;

        /*
        rep(j, sz)
        {
            int start = path2[j * LB];
            int to_pos = min((j + 1) * LB, (int)path2.size());
            int end = path2[to_pos];
            edges.push_back({start, end, vec_int(path2.begin() + j * LB, path2.begin() + to_pos)});
        }
        */
        paths.push_back(path2);

        if (end_flag)
        {
            break;
        }
    }

    vector<Edge> edges;

    vector<int> visited(N, 0);
    for (auto path : paths)
    {
        for (auto p : path)
        {
            visited[p] = 1;
        }
    }
    cout << endl;
    PseudoSet ps(N);
    rep(i, N)
    {
        if (used_node_set.count(i) > 0 && visited[i] == 0)
        {
            ps.insert(i);
        }
    }

    while (true)
    {
        int start;
        while (true)
        {
            int len = ps.size();
            if (len == 0)
                break;
            start = ps.data[ro.get_rand(0, len) % len];
            if (visited[start] == 0)
                break;

            ps.erase(start);
            if (ps.size() == 0)
                break;
        }
        if (ps.size() == 0)
            break;

        vec_int path;
        path.push_back(start);
        visited[start] = 1;
        int pos = start;
        while (path.size() < LB)
        {
            // startから生やしていく
            vector<P> nexts;
            for (auto tmp_pos : path)
            {
                for (auto pos2 : G[tmp_pos])
                {
                    if (used_node_set.count(pos2) == 0)
                        continue;
                    if (visited[pos2] == 0)
                    {
                        int tot_cost = 0;
                        for (auto p : path)
                        {
                            tot_cost += dist_pairs[p][pos2];
                        }

                        nexts.push_back(P(tot_cost, pos2));
                    }
                }
            }
            if (nexts.size() == 0)
                break;

            sort(nexts.begin(), nexts.end());
            reverse(nexts.begin(), nexts.end());
            auto next = nexts[0].second;

            path.push_back(next);
            pos = next;
            visited[pos] = 1;
        }
        int end = pos;

        // 次に始める候補点を追加する
        for (auto p : path)
        {
            for (auto p2 : G[p])
            {
                if (visited[p2] == 0 && used_node_set.count(p2))
                {
                    ps.insert(p2);
                }
            }
        }
        edges.push_back({start, end, path});
    }

    vector<int> edge_starts;
    vec_int A_vec(LA, 0);
    int index = 0;
    rep(i, edges.size())
    {
        edge_starts.push_back(index);
        for (auto p : edges[i].path)
        {
            A_vec[index] = p;
            index++;
        }
    }

    // 長いパスをまとめて登録する

    for (auto path : paths)
    {
        int tmp_index = index;
        for (auto p : path)
        {
            A_vec[index] = p;
            index++;
        }
        // 一個ずつずらしてedgeに登録する
        for (int start_pos = 0; start_pos < max((int)path.size() - LB + 1, 1); start_pos++)
        {
            vec_int edge_path;
            for (int j = start_pos; j < min(start_pos + LB, (int)path.size()); j++)
            {
                edge_path.push_back(path[j]);
            }
            edges.push_back({edge_path[0], edge_path.back(), edge_path});
            edge_starts.push_back(tmp_index + start_pos);
        }
    }

    return make_tuple(edges, edge_starts, A_vec);
}




struct Result
{
    int score;
    vector<vec_int> manipulations;
    vector<vector<vec_int>> moves;
    vector<vec_int> first_moves;
    vec_int A_vec;
    vector<int> edge_starts;
};

pair<vector<int>, vec_int> normal_A_vec(vector<Edge> &edges)
{
    // 各ノードに所属しているエッジを記録する
    vector<vector<int>> node_edges(N);
    rep(i, edges.size())
    {
        for (auto p : edges[i].path)
        {
            node_edges[p].push_back(i);
        }
    }

    // 各エッジをLAに転記して、スタート位置を記録する
    vec_int A_vec(LA, 0);
    int edge_count = edges.size();
    vec_int edge_starts(edge_count, -1);
    int start_pos = 0;
    rep(i, edge_count)
    {
        edge_starts[i] = start_pos;
        for (auto p : edges[i].path)
        {
            A_vec[start_pos] = p;
            start_pos++;
        }
    }
    return {edge_starts, A_vec};
}

Result find_operations(vector<vec_int> &G, vector<Edge> &edges, vector<int> &edge_starts, vec_int &A_vec)
{
    // 各ノードに所属しているエッジを記録する
    vector<vector<int>> node_edges(N);
    rep(i, edges.size())
    {
        for (auto p : edges[i].path)
        {
            node_edges[p].push_back(i);
        }
    }

    // 各エッジをLAに転記して、スタート位置を記録する
    /*
    vec_int A_vec(LA, 0);
    int edge_count = edges.size();
    vec_int edge_starts(edge_count, -1);
    int start_pos = 0;
    rep(i, edge_count)
    {
        edge_starts[i] = start_pos;
        for (auto p : edges[i].path)
        {
            A_vec[start_pos] = p;
            start_pos++;
        }
    }
    */

    // あとは適当に最短経路問題を毎回解いていく
    // 解は一旦解きやすい形式で持っておく

    vector<vec_int> manipulations(T); // 各ターンにおいて行う信号操作のリスト
    vector<vector<vec_int>> moves(T); // 各ターンの各信号操作の後に行う移動のリスト
    vector<vec_int> first_moves(T);

    int turn_start = 0;
    int tmp_score = 0;

    int last_edge = -1;

    int shortcut_found_turns = 0;
    vector<T5> cache(N, T5(-1, INT_MAX, -1, -1, -1));

    for (int turn = 0; turn < T; turn++)
    {
        int destination = t[turn];

        // destinationに行く最短経路を求める
        queue<T5> que;
        que.push(T5(0, turn_start, -1, -1, -1));
        if (last_edge != -1)
        {
            for (auto pos : edges[last_edge].path)
            {
                if (pos == turn_start)
                    continue;
                que.push(T5(0, pos, -1, -1, -1));
            }
        }
        set<int> visited_pos;

        vector<T4> last_candidates; // 最後の1ターンでは複数の可能性を残す, 具体的には次のターンの目的地を持っているエッジを優先させる
        int asikiri_cost = INT_MAX;

        while (que.size() > 0)
        {
            auto [cost, pos, from_pos, from_prev_pos, used_edge] = que.front();
            que.pop();
            if ((cost > asikiri_cost && pos == destination) || (cost >= asikiri_cost && pos != destination))
                continue;

            if ((pos != destination && get<0>(cache[pos]) == turn) || (pos == destination && get<0>(cache[pos]) == turn && get<1>(cache[pos]) < cost))
                continue;

            cache[pos] = T5(turn, cost, from_pos, from_prev_pos, used_edge);
            visited_pos.insert(pos);

            if (pos == destination)
            {
                asikiri_cost = cost;
                last_candidates.push_back(T4(cost, from_pos, from_prev_pos, used_edge));
                continue;
            }

            // この次の信号操作で行ける場所を列挙する
            // 一個隣のノードのエッジのうち、used_edgeでないもので行けるところを列挙する

            int next_cost = cost + 1;
            for (auto neighbor_node : G[pos])
            {
                // if (get<0>(cache[neighbor_node]) < INT_MAX)
                //    continue;
                for (auto edge_index : node_edges[neighbor_node])
                {
                    if (edge_index == used_edge)
                        continue;

                    for (auto to_pos : edges[edge_index].path)
                    {
                        que.push(T5(next_cost, to_pos, neighbor_node, pos, edge_index));
                    }
                }
            }
        }
        int next_destination = turn < T - 1 ? t[turn + 1] : -1;
        // cerr << "turn:" << turn << " next:" << next_destination << " last_candidates:" << last_candidates.size() << endl;
        // cerr << node_edges[destination].size() << endl;
        int best_candidate = -1;
        rep(i, last_candidates.size())
        {
            auto [cost, from_pos, from_prev_pos, used_edge] = last_candidates[i];
            if (used_edge == -1)
                continue;
            set<int> path_set = set<int>(edges[used_edge].path.begin(), edges[used_edge].path.end());
            if (path_set.count(next_destination))
            {
                best_candidate = i;
                // cerr << "found!" << best_candidate << " turn:" << turn << " next:" << next_destination << " edge:" << used_edge << endl;
                shortcut_found_turns++;
                break;
            }
        }
        // cerr << best_candidate << " " << last_candidates.size() << endl;
        if (best_candidate == -1)
        {
            auto [cost, from_pos, from_prev_pos, used_edge] = last_candidates[0];
            cache[destination] = T5(turn, cost, from_pos, from_prev_pos, used_edge);
        }
        else
        {
            auto [cost, from_pos, from_prev_pos, used_edge] = last_candidates[best_candidate];
            cache[destination] = T5(turn, cost, from_pos, from_prev_pos, used_edge);
        }

        // 再構築をする
        int pos2 = destination;
        // cerr << "pos2:" << pos2 << endl;

        while (true)
        {
            auto [turn, cost, from_pos, from_prev_pos, used_edge] = cache[pos2];
            if (used_edge == -1)
                break;
            if (used_edge != -1)
            {
                // cerr << "edges size:" << edges.size() << endl;
                manipulations[turn].push_back(used_edge);
                moves[turn].push_back(edges[used_edge].get_route(from_pos, pos2, G));
                // cerr << "get route finished" << endl;
            }
            pos2 = from_prev_pos;
            tmp_score += 1;
        }
        if (last_edge != -1)
        {
            auto tmp_moves = edges[last_edge].get_route(pos2, turn_start, G);
            tmp_moves.pop_back();
            reverse(tmp_moves.begin(), tmp_moves.end());
            first_moves[turn] = tmp_moves;
        }

        reverse(manipulations[turn].begin(), manipulations[turn].end());
        reverse(moves[turn].begin(), moves[turn].end());
        turn_start = destination;
        last_edge = manipulations[turn].size() > 0 ? manipulations[turn].back() : last_edge;
    }
    // cerr << "shortcut_found_turns:" << shortcut_found_turns << endl;

    return {
        tmp_score,
        manipulations,
        moves,
        first_moves,
        A_vec,
        edge_starts};
}

Result find_operations_thunder(vector<vec_int> &G, vector<Edge> &edges, vector<int> &edge_starts, vec_int &A_vec)
{
    // 各ノードに所属しているエッジを記録する
    vector<vector<int>> node_edges(N);
    rep(i, edges.size())
    {
        for (auto p : edges[i].path)
        {
            node_edges[p].push_back(i);
        }
    }
    /*

    // 各エッジをLAに転記して、スタート位置を記録する
    vec_int A_vec(LA, 0);
    int edge_count = edges.size();
    vec_int edge_starts(edge_count, -1);
    int start_pos = 0;
    rep(i, edge_count)
    {
        edge_starts[i] = start_pos;
        for (auto p : edges[i].path)
        {
            A_vec[start_pos] = p;
            start_pos++;
        }
    }
    */

    // あとは適当に最短経路問題を毎回解いていく
    // 解は一旦解きやすい形式で持っておく

    vector<set<int>> path_set(edges.size());
    for (int i = 0; i < edges.size(); i++)
    {
        for (auto p : edges[i].path)
        {
            path_set[i].insert(p);
        }
    }

    vector<vec_int> manipulations(T); // 各ターンにおいて行う信号操作のリスト
    vector<vector<vec_int>> moves(T); // 各ターンの各信号操作の後に行う移動のリスト
    vector<vec_int> first_moves(T);

    int turn_start = 0;
    int tmp_score = 0;

    int last_edge = -1;

    int center_node = edges[0].path[0];

    // turn0は特殊なので別に対処する
    int destination_0 = t[0];
    int selected_edge_0 = node_edges[0][0];
    for (auto edge_index : node_edges[0])
    {
        if (path_set[edge_index].count(destination_0) > 0)
        {
            selected_edge_0 = edge_index;
            break;
        }
    }

    manipulations[0].push_back(selected_edge_0);
    if (path_set[selected_edge_0].count(destination_0))
    {
        last_edge = selected_edge_0;
        vec_int tmp = edges[selected_edge_0].get_route(destination_0, 0, G);
        tmp.pop_back();
        reverse(tmp.begin(), tmp.end());

        moves[0].push_back(tmp);
        tmp_score++;
    }
    else
    {
        last_edge = selected_edge_0;
        tmp_score += 2;

        vec_int tmp = edges[selected_edge_0].get_route(center_node, 0, G);
        tmp.pop_back();
        reverse(tmp.begin(), tmp.end());
        moves[0].push_back(tmp);

        int next_destination = t[1];
        int selected_edge_1 = node_edges[destination_0][0];
        for (auto edge_index : node_edges[destination_0])
        {
            if (path_set[edge_index].count(next_destination) > 0)
            {
                selected_edge_1 = edge_index;
                break;
            }
        }
        manipulations[0].push_back(selected_edge_1);
        last_edge = selected_edge_1;

        tmp = edges[selected_edge_1].get_route(destination_0, center_node, G);
        tmp.pop_back();
        reverse(tmp.begin(), tmp.end());
        moves[0].push_back(tmp);
    }

    for (int turn = 1; turn < T; turn++)
    {
        int from = t[turn - 1];
        int destination = t[turn];
        if (path_set[last_edge].count(destination))
        {
            vec_int tmp = edges[last_edge].get_route(destination, from, G);
            tmp.pop_back();
            reverse(tmp.begin(), tmp.end());
            first_moves[turn] = tmp;
            continue;
        }

        tmp_score++;
        // 中央に行く
        vec_int tmp = edges[last_edge].get_route(center_node, from, G);
        tmp.pop_back();
        reverse(tmp.begin(), tmp.end());
        first_moves[turn] = tmp;

        // 次のエッジを選ぶ
        int selected_edge = node_edges[destination][0];
        if (turn < T - 1)
        {
            int next_destination = t[1 + turn];
            for (auto edge_index : node_edges[destination])
            {
                if (path_set[edge_index].count(next_destination) > 0)
                {
                    selected_edge = edge_index;
                    break;
                }
            }
        }

        manipulations[turn].push_back(selected_edge);
        last_edge = selected_edge;
        tmp = edges[selected_edge].get_route(destination, center_node, G);
        tmp.pop_back();
        reverse(tmp.begin(), tmp.end());
        moves[turn].push_back(tmp);

        /*
        int destination = t[turn];

        // destinationに行く最短経路を求める
        vector<T4> cache(N, T4(INT_MAX, -1, -1, -1));
        priority_queue<T5, vector<T5>, greater<T5>> pq;
        pq.push(T5(0, turn_start, -1, -1, -1));
        if (last_edge != -1)
        {
            for (auto pos : edges[last_edge].path)
            {
                if (pos == turn_start)
                    continue;
                pq.push(T5(0, pos, -1, -1, -1));
            }
        }
        set<int> visited_pos;

        while (pq.size() > 0)
        {
            auto [cost, pos, from_pos, from_prev_pos, used_edge] = pq.top();
            pq.pop();
            if (get<0>(cache[pos]) <= cost)
                continue;

            cache[pos] = T4(cost, from_pos, from_prev_pos, used_edge);
            visited_pos.insert(pos);

            if (pos == destination)
            {
                // ここで終了
                break;
            }

            // この次の信号操作で行ける場所を列挙する
            // 一個隣のノードのエッジのうち、used_edgeでないもので行けるところを列挙する

            int next_cost = cost + 1;
            for (auto neighbor_node : G[pos])
            {
                if (get<0>(cache[neighbor_node]) < INT_MAX)
                    continue;
                for (auto edge_index : node_edges[neighbor_node])
                {
                    if (edge_index == used_edge)
                        continue;

                    for (auto to_pos : edges[edge_index].path)
                    {
                        pq.push(T5(next_cost, to_pos, neighbor_node, pos, edge_index));
                    }
                }
            }
        }

        // 再構築をする
        int pos2 = destination;

        while (true)
        {
            auto [cost, from_pos, from_prev_pos, used_edge] = cache[pos2];
            if (used_edge == -1)
                break;
            if (used_edge != -1)
            {
                manipulations[turn].push_back(used_edge);
                moves[turn].push_back(edges[used_edge].get_route(from_pos, pos2, G));
            }
            pos2 = from_prev_pos;
            tmp_score += 1;
        }
        if (last_edge != -1)
        {
            auto tmp_moves = edges[last_edge].get_route(pos2, turn_start, G);
            tmp_moves.pop_back();
            reverse(tmp_moves.begin(), tmp_moves.end());
            first_moves[turn] = tmp_moves;
        }

        reverse(manipulations[turn].begin(), manipulations[turn].end());
        reverse(moves[turn].begin(), moves[turn].end());
        turn_start = destination;
        last_edge = manipulations[turn].size() > 0 ? manipulations[turn].back() : -1;
        */
    }

    return {
        tmp_score,
        manipulations,
        moves,
        first_moves,
        A_vec,
        edge_starts};
}




class Thunder
{
public:
    vec_int path;
    Thunder(vec_int path)
    {
        this->path = path;
    }
};

class Thunders
{
private:
    vector<PseudoSet> belongings;
    vector<vec_int> next_targets = vector<vec_int>(N);

public:
    int score()
    {
        if (not_visited_neccesary_node_set.size() > 0)
        {
            return not_visited_neccesary_node_set.size();
        }
        else
        {
            set<P> used_set;
            int cnt = 0;
            for (int i = 0; i < thunders.size(); i++)
            {
                for (auto node : thunders[i].path)
                {
                    for (auto next : next_targets[node])
                    {
                        if (used_set.count(P(node, next)) > 0)
                        {
                            continue;
                        }
                        if (belongings[i].count(next))
                        {
                            used_set.insert(P(node, next));
                            cnt++;
                            break;
                        }
                    }
                }
            }
            return -cnt;
        }
    }
    vector<Thunder> thunders;
    vector<int> node_count;
    set<int> used_node_set;
    PseudoSet not_visited_neccesary_node_set;
    Thunders(vector<Thunder> thunders, set<int> used_node_set)
    {
        for (int i = 0; i < T - 1; i++)
        {
            next_targets[t[i]].push_back(t[i + 1]);
        }
        belongings = vector<PseudoSet>(thunders.size(), PseudoSet(N));
        for (int i = 0; i < thunders.size(); i++)
        {
            for (auto node : thunders[i].path)
            {
                belongings[i].insert(node);
            }
        }

        not_visited_neccesary_node_set = PseudoSet(N);
        this->thunders = thunders;
        this->used_node_set = used_node_set;
        this->node_count = vec_int(N, 0);
        for (auto thunder : thunders)
        {
            for (auto node : thunder.path)
            {
                node_count[node] += 1;
            }
        }
        rep(i, N)
        {
            if (used_node_set.count(i) && node_count[i] == 0)
            {
                not_visited_neccesary_node_set.insert(i);
            }
        }
    }
    void replace_thunder(Thunder &thunder, int id)
    {
        for (auto node : thunders[id].path)
        {
            node_count[node] -= 1;
            if (node_count[node] == 0 && used_node_set.count(node))
            {
                not_visited_neccesary_node_set.insert(node);
            }
            belongings[id].erase(node);
        }

        for (auto node : thunder.path)
        {
            node_count[node] += 1;
            if (node_count[node] == 1 && used_node_set.count(node))
            {
                not_visited_neccesary_node_set.erase(node);
            }
            belongings[id].insert(node);
        }
        thunders[id] = thunder;
    }
};

class TreeAnnealer
{
    vector<vec_int> G;
    set<int> used_node_set;
    vec_int construct_nodes(int root_node, vector<vec_int> &dist_pair, vec_int &initial_path, PseudoSet &not_visited_neccesary_node_set);

public:
    TreeAnnealer(vector<vec_int> &G, set<int> &used_node_set)
    {
        this->G = G;
        this->used_node_set = used_node_set;
    }

    pair<vector<Edge>, bool> annealing(int root_node, vector<vec_int> &dist_pair, int diff_length);
};

vec_int TreeAnnealer::construct_nodes(int root_node, vector<vec_int> &dist_pair, vec_int &initial_path, PseudoSet &not_visited_neccesary_node_set)
{
    vec_int path = {root_node};
    if (initial_path.size() > 0)
    {
        path = initial_path;
    }

    set<int> visited = {root_node};
    int deepest = 0;
    for (auto p : path)
    {
        deepest = max(deepest, dist_pair[root_node][p]);
    }

    while (path.size() < LB)
    {
        vec_int candidates;
        vec_int second_candidates;
        for (auto p : path)
        {
            for (auto next : G[p])
            {
                if (visited.count(next) == 0 && dist_pair[root_node][next] > deepest || not_visited_neccesary_node_set.count(next) > 0)
                {
                    candidates.push_back(next);
                }
                else if (visited.count(next) == 0 && dist_pair[root_node][next] >= deepest - 2)
                {
                    second_candidates.push_back(next);
                }
            }
        }
        if (candidates.size() == 0)
        {
            candidates = second_candidates;
        }
        if (candidates.size() == 0)
        {
            break;
        }
        int new_node = candidates.at(ro.get_rand(0, candidates.size()));
        path.push_back(new_node);
        visited.insert(new_node);
        deepest = max(deepest, dist_pair[root_node][new_node]);
    }
    return path;
}

pair<vector<Edge>, bool> TreeAnnealer::annealing(int root_node, vector<vec_int> &dist_pair, int diff_length)
{
    int count = LA / LB;
    if (diff_length <= 0)
    {
        count += diff_length * 4;
    }
    vector<Thunder> thunder_vec;
    PseudoSet tmp_set(N);
    for (auto node : used_node_set)
    {
        tmp_set.insert(node);
    }

    rep(i, count)
    {
        vec_int aa;
        vec_int path = construct_nodes(root_node, dist_pair, aa, tmp_set);
        Thunder thunder = Thunder(path);
        thunder_vec.push_back(thunder);
        for (auto node : path)
        {
            tmp_set.erase(node);
        }
    }

    Thunders thunders = Thunders(thunder_vec, used_node_set);
    cerr << "thunders size:" << thunders.thunders.size() << endl;
    cerr << "not_visited_neccesary_node_set size:" << thunders.not_visited_neccesary_node_set.size() << endl;

    int score = thunders.score();
    double T0 = 1.2;
    double T1 = 0.0;

    int best_score = score;
    Thunders best_state = thunders;

    while (true)
    {
        if (get_time(false) > 1900)
            break;

        int id = ro.get_rand(0, thunders.thunders.size());
        auto old_thunder = thunders.thunders[id];

        // old_thunderのパスの一部だけを取得する。この際に長さをランダムに選択して、先頭からその分だけ取得する
        /*
        vec_int initial_path = old_thunder.path;
        initial_path.resize(ro.get_rand(0, initial_path.size()));
        */
        vec_int initial_path;
        if (ro.get_rand(0, 4) == 0 && thunders.not_visited_neccesary_node_set.size() > 0)
        {
            int node = thunders.not_visited_neccesary_node_set.data.at(ro.get_rand(0, thunders.not_visited_neccesary_node_set.size()));
            initial_path.push_back(node);
            int depth = dist_pair[root_node][node];
            if (depth >= LB)
            {
                continue;
            }
            while (node != root_node)
            {
                vec_int candidates;
                for (auto next : G[node])
                {
                    if (dist_pair[root_node][next] == depth - 1)
                    {
                        candidates.push_back(next);
                    }
                }
                int next_node = candidates.at(ro.get_rand(0, candidates.size()));
                initial_path.push_back(next_node);
                node = next_node;
                depth--;
            }
            reverse(initial_path.begin(), initial_path.end());
        }
        else
        {
            initial_path = old_thunder.path;
            initial_path.resize(ro.get_rand(0, initial_path.size()));
        }

        auto new_thunder = Thunder(construct_nodes(root_node, dist_pair, initial_path, thunders.not_visited_neccesary_node_set));
        thunders.replace_thunder(new_thunder, id);
        int score_new = thunders.score();
        double diff = score_new - score;
        if (is_valid_transition(-diff, get_time(false), T0, T1, 1900))
        {
            score = score_new;
            if (score < best_score)
            {
                best_score = score;
                best_state = thunders;
                cerr << "new:" << score_new << endl;
            }
        }
        else
        {
            thunders.replace_thunder(old_thunder, id);
        }
    }
    // cerr << "end_score:" << best_score << endl;

    vector<Edge> edges;
    for (auto thunder : best_state.thunders)
    {
        Edge edge = Edge{thunder.path[0], thunder.path[thunder.path.size() - 1], thunder.path};
        edges.push_back(edge);
    }

    vector<set<int>> path_set(best_state.thunders.size());
    rep(i, best_state.thunders.size())
    {
        for (auto node : best_state.thunders[i].path)
        {
            path_set[i].insert(node);
        }
    }

    /*
    int continuous_count = 0;
    for (int i = 0; i < T - 1; i++)
    {
        int from = t[i];
        int to = t[i + 1];
        rep(j, thunders.thunders.size())
        {
            if (path_set[j].count(from) > 0 && path_set[j].count(to) > 0)
            {
                continuous_count++;
                break;
            }
        }
    }
    cerr << "continuous_count:" << continuous_count << endl;
    */
    cerr << "annealing end:" << best_score << endl;

    return {edges, best_score <= 0};
}

tuple<vector<Edge>, vec_int, vec_int> compress_edges(vector<Edge> &edges, vector<vec_int> dist_pair, int center_node, vector<vec_int> &G)
{
    int edge_count = edges.size();
    vector<vec_int> edge_common = vector<vec_int>(edge_count, vec_int(edge_count, 0));

    for (int i = 0; i < edge_count; i++)
    {
        for (int j = i + 1; j < edge_count; j++)
        {
            if (i == j)
            {
                continue;
            }
            for (int k = 0; k < edges[i].path.size() && k < edges[j].path.size(); k++)
            {
                if (edges[i].path[k] != edges[j].path[k])
                {
                    edge_common[i][j] = k;
                    edge_common[j][i] = k;
                    break;
                }
            }
        }
    }
    cerr << "hogee" << endl;

    vector<P> edge_pairs;
    vector<bool> used = vector<bool>(edge_count, false);
    // 共通部分が多いペアから順に使っていく
    int count = 0;
    int tot_common = 0;
    while (edge_count - count > 1)
    {
        int max_common = 0;
        P max_pair;
        for (int i = 0; i < edge_count; i++)
        {
            if (used[i])
            {
                continue;
            }
            for (int j = i + 1; j < edge_count; j++)
            {
                if (used[j])
                {
                    continue;
                }
                if (edge_common[i][j] > max_common)
                {
                    max_common = edge_common[i][j];
                    max_pair = P(i, j);
                }
            }
        }
        edge_pairs.push_back(max_pair);
        used[max_pair.first] = true;
        used[max_pair.second] = true;
        tot_common += max_common;
        count += 2;
    }
    cerr << "tot_common:" << tot_common << endl;

    vec_int A_vec = vec_int(LA, 0);
    vec_int start_edges = vec_int(edge_count, -1);

    int index = 0;
    for (auto edge_pair : edge_pairs)
    {
        auto [edge_1, edge_2] = edge_pair;
        int common = edge_common[edge_1][edge_2];

        vec_int path_1 = edges[edge_1].path;
        reverse(path_1.begin(), path_1.end());
        start_edges[edge_1] = index;
        for (auto node : path_1)
        {
            A_vec[index] = node;
            index++;
        }

        start_edges[edge_2] = index - common;
        for (int i = common; i < edges[edge_2].path.size(); i++)
        {
            A_vec[index] = edges[edge_2].path[i];
            index++;
        }
    }
    int katahou = -1;
    rep(i, edge_count)
    {
        if (start_edges[i] == -1)
        {
            katahou = i;
            break;
        }
    }
    if (katahou != -1)
    {
        start_edges[katahou] = index;
        for (auto node : edges[katahou].path)
        {
            A_vec[index] = node;
            index++;
        }
    }

    // 残りの辺の数
    int res = LA - index;

    set<int> target_nodes;
    rep(i, T)
    {
        target_nodes.insert(t[i]);
    }
    set<int> used_nodes;
    for (auto edge : edges)
    {
        for (auto node : edge.path)
        {
            used_nodes.insert(node);
        }
    }

    set<int> not_included_nodes;
    for (auto target_node : target_nodes)
    {
        if (used_nodes.count(target_node) == 0)
        {
            not_included_nodes.insert(target_node);
        }
    }

    // not_included_nodesの各ノードを含む辺を作って、used_nodesに隣接するようにする
    int check = 4;
    while (!not_included_nodes.empty())
    {
        vec_int nodes = vec_int(not_included_nodes.begin(), not_included_nodes.end());
        cerr << "not_included_nodes.size()" << not_included_nodes.size() << endl;
        int max_distance = 0;
        int farest_node = -1;
        for (auto node : nodes)
        {
            int distance = dist_pair[center_node][nodes[0]];
            if (distance > max_distance)
            {
                max_distance = distance;
                farest_node = node;
            }
        }

        // farest_nodeから始めて、既存のノードにたどり着くまでのedgeを追加する
        vec_int path = {farest_node};
        set<int> belonging_nodes = {farest_node};

        while (true)
        {
            vec_int neighbors;
            bool flag = false;
            for (auto node : path)
            {
                for (auto next : G[node])
                {
                    if (used_nodes.count(next) > 0)
                    {
                        flag = true;
                        break;
                    }
                    if (belonging_nodes.count(next) == 0)
                    {
                        neighbors.push_back(next);
                    }
                }
            }
            if (flag)
            {
                break;
            }

            if (LB - path.size() > check)
            {
                // neighborの中でnot_included_nodesがいたらそいつを追加する
                int nd = -1;
                for (auto node : neighbors)
                {
                    if (not_included_nodes.count(node) > 0)
                    {
                        nd = node;
                        break;
                    }
                }
                if (nd != -1)
                {
                    path.push_back(nd);
                    belonging_nodes.insert(nd);
                    continue;
                }
            }
            int min_distance = INT_MAX;
            int min_distance_node = -1;
            for (auto node : neighbors)
            {
                int distance = dist_pair[center_node][node];
                if (distance < min_distance)
                {
                    min_distance = distance;
                    min_distance_node = node;
                }
            }
            path.push_back(min_distance_node);
            belonging_nodes.insert(min_distance_node);
        }
        if (path.size() > LB)
        {
            check++;
            continue;
        }

        auto edge = Edge{farest_node, path[path.size() - 1], path};
        edges.push_back(edge);
        start_edges.push_back(index);
        for (auto node : path)
        {
            if (index >= LA)
            {
                start_edges.push_back(22222);
                return {edges, start_edges, A_vec};
            }
            not_included_nodes.erase(node);
            A_vec[index] = node;
            index++;
        }
        cerr << "path_size:" << path.size() << " " << not_included_nodes.count(path[0]) << endl;
    }
    cerr << "last_index:" << index << " " << LA << endl;

    return {edges, start_edges, A_vec};
}




struct Move
{
    int turn;
    int line_id;
    int from;
    int to;
};

tuple<vector<Edge>, vec_int, vec_int> solve(vector<vec_int> &G, vector<vec_int> &dist_pair, vector<vec_int> paths, int center_node)
{
    auto [path_pairs, single_vec] = match_paths(paths);
    vec_int A_vec;
    vector<Line> lines;
    for (auto path_pair : path_pairs)
    {
        auto [i, j] = path_pair;
        merge_paths(paths[i], paths[j], G, A_vec, lines);
    }
    /*
    if (last != -1)
    {
    */
    for (int last : single_vec)
    {
        auto leaf_node = find_leaf_node(G, paths[last]);
        auto tmp_path = paths[last];
        tmp_path.erase(find(tmp_path.begin(), tmp_path.end(), leaf_node));
        tmp_path.push_back(leaf_node);

        if (tmp_path.size() > LB)
        {
            vec_int vec1;
            vec_int vec2;
            rep(i, LB)
            {
                vec1.push_back(tmp_path[i]);
                vec2.push_back(tmp_path[i + 1]);
            }
            set<int> path1_set = set<int>(vec1.begin(), vec1.end());
            set<int> path2_set = set<int>(vec2.begin(), vec2.end());
            lines.push_back(Line{vec1, path1_set, (int)A_vec.size()});
            lines.push_back(Line{vec2, path2_set, (int)A_vec.size() + 1});
        }
        else
        {
            set<int> tmp_path_set = set<int>(tmp_path.begin(), tmp_path.end());
            lines.push_back(Line{tmp_path, tmp_path_set, (int)A_vec.size()});
        }

        for (auto node : tmp_path)
        {
            A_vec.push_back(node);
        }
    }

    vector<Edge> edges;
    vec_int edge_starts;
    for (auto line : lines)
    {
        vec_int path = line.path;
        edges.push_back(Edge{path[0], path[path.size() - 1], path});
        edge_starts.push_back(line.start_on_A_vec);
    }

    // return make_pair(find_operations(G, edges, edge_starts, A_vec), edges);
    return make_tuple(edges, edge_starts, A_vec);
}

void cout_answer(Result &result, vector<Edge> &edges)
{
    vec_int tmp_a_vec = vec_int(LA, 0);
    rep(i, result.A_vec.size()) tmp_a_vec[i] = result.A_vec[i];

    rep(i, LA)
    {
        cout << tmp_a_vec[i] << " ";
    }
    cout << endl;

    int turn_start = 0;
    int last_manipulation = -1;
    int score2 = 0;
    rep(turn, T)
    {
        for (auto p : result.first_moves[turn])
        {
            cout << "m " << p << endl;
        }
        turn_start = t[turn];
        score2 += result.manipulations[turn].size();
        rep(i, result.manipulations[turn].size())
        {
            int edge_index = result.manipulations[turn][i];
            int left = result.edge_starts[edge_index];
            int right = min(left + LB - 1, LA - 1);
            int l = right - left + 1;
            if (last_manipulation != edge_index)
            {
                cout << "# edge_index:" << edge_index << endl;
                cout << "# ";
                for (auto node : edges[edge_index].path)
                {
                    cout << node << " ";
                }
                cout << endl;

                cout << "s " << l << " " << left << " " << 0 << endl;
            }
            last_manipulation = edge_index;
            for (auto p : result.moves[turn][i])
            {
                cout << "# edge_index:" << edge_index << endl;
                cout << "m " << p << endl;
            }
        }
    }
    cerr << "score:" << score2 << endl;
}




class TreeState
{
    vec_int parent = vec_int(N, -1);
    vector<PseudoSet> children_set;
    PseudoSet target_set;
    int count_downstream_nodes(vec_int &downstream_node_counts, vector<vec_int> &children, int pos);
    tuple<int, int, vec_int> reconstruct_tree(int node, vector<vec_int> &path_group, int depth);
    P rec_calc_score(int node, int depth);
    vector<P> score_cache = vector<P>(N, P(-1, -1));
    vector<bool> is_changed;

public:
    vector<vec_int> make_additional_paths(int res_nodes, int root_node, vector<vec_int> &current_paths, float time_limit);
    int center_node;
    PseudoSet node_groups;
    vector<vec_int> G;
    vector<vec_int> dist_pair;
    TreeState(int center_node, PseudoSet &node_groups, vector<vec_int> &G) : center_node(center_node), node_groups(node_groups), G(G)
    {
        target_set = PseudoSet(N);
        rep(i, T) target_set.insert(t[i]);
        target_set.insert(0);
        dist_pair = vector<vec_int>(N, vec_int(N, INT_MAX));
        children_set = vector<PseudoSet>(N, PseudoSet(N));

        queue<P> que;
        for (auto node : node_groups.data)
        {
            que.emplace(node, 0);
            while (que.size() > 0)
            {
                auto [pos, d] = que.front();
                que.pop();
                if (dist_pair[node][pos] != INT_MAX)
                    continue;
                dist_pair[node][pos] = d;
                for (auto next_pos : G[pos])
                {
                    if (!node_groups.count(next_pos))
                        continue;
                    if (dist_pair[node][next_pos] == INT_MAX)
                    {
                        que.emplace(next_pos, d + 1);
                    }
                }
            }
        }
        is_changed = vector<bool>(N, true);
    };
    int score();
    pair<vector<vec_int>, int> annealing(int time_ms, vector<int> pre_used_nodes);
    vector<vec_int> append_edges(vector<vec_int> ret);
    int calc_score(bool verbose);
};

int count_downstream_nodes(vec_int &downstream_node_counts, vector<vec_int> &children, int pos)
{
    int result = 1;
    for (auto next_pos : children[pos])
    {
        result += count_downstream_nodes(downstream_node_counts, children, next_pos);
    }
    downstream_node_counts[pos] = result;
    return result;
}

pair<int, int> TreeState::rec_calc_score(int node, int depth)
{
    if (!is_changed[node])
    {
        return score_cache[node];
    }

    if (depth > LB && target_set.count(node))
    {
        score_cache[node] = P(1000000, 0);
        is_changed[node] = false;
        return P(1000000, 0);
    }
    // cerr << "node:" << node << " depth:" << depth << endl;
    int result = 0;
    int downstream_node_count = 0;
    // 自身も含めたノードの数:
    int upstream_node_count = 1 + depth;

    if (children_set[node].size() == 0 && !target_set.count(node))
    {
        score_cache[node] = P(0, 0);
        is_changed[node] = false;
        return {0, 0};
    }

    vec_int downstream_node_counts;
    int invalid_leaf_count = 0;
    vec_int children = children_set[node].data;
    sort(children.begin(), children.end());
    for (auto next_pos : children)
    {
        auto [next_result, next_downstream_node_count] = rec_calc_score(next_pos, depth + 1);
        if (next_result == 0 && next_downstream_node_count == 0)
        {
            invalid_leaf_count++;
        }
        result += next_result;
        downstream_node_count += next_downstream_node_count;
        if (next_downstream_node_count > 0)
        {
            if (downstream_node_counts.size() > 0)
            {
                int idx = -1;
                for (int i = 0; i < downstream_node_counts.size(); i++)
                {
                    if (downstream_node_counts[i] + next_downstream_node_count + upstream_node_count <= LB)
                    {
                        idx = i;
                        break;
                    }
                }
                if (idx != -1)
                {
                    downstream_node_counts[idx] += next_downstream_node_count;
                }
                else
                {
                    downstream_node_counts.push_back(next_downstream_node_count);
                }
            }
            else
            {
                downstream_node_counts.push_back(next_downstream_node_count);
            }
        }
    }
    if (children_set[node].size() == invalid_leaf_count && !target_set.count(node) && result == 0 && downstream_node_count == 0)
    {
        score_cache[node] = P(0, 0);
        is_changed[node] = false;

        return {0, 0};
    }

    if (children_set[node].size() - invalid_leaf_count > 0 && downstream_node_count == 0)
    {
        score_cache[node] = P(result, 0);
        is_changed[node] = false;

        return make_pair(result, 0);
    }

    if (downstream_node_count + upstream_node_count > LB && downstream_node_count > 0)
    {
        // このときには分割を行う
        int cnt = downstream_node_counts.size() / 2;
        sort(downstream_node_counts.begin(), downstream_node_counts.end(), greater<int>());

        result += cnt * upstream_node_count;
        rep(i, cnt * 2)
        {
            result += downstream_node_counts[i];
            if (downstream_node_counts[i * 2] + upstream_node_count > LB && downstream_node_counts[i * 2 + 1] + upstream_node_count > LB)
            {
                result += upstream_node_count;
            }
        }

        if (downstream_node_counts.size() % 2 == 0)
        {
            score_cache[node] = P(result, 0);
            is_changed[node] = false;
            return {result, 0};
        }
        else
        {
            score_cache[node] = P(result, downstream_node_counts[cnt * 2] + 1);
            is_changed[node] = false;
            return {result, downstream_node_counts[cnt * 2] + 1};
        }
    }
    score_cache[node] = P(result, downstream_node_count + 1);
    is_changed[node] = false;
    return {result, downstream_node_count + 1};
}

tuple<int, int, vec_int> TreeState::reconstruct_tree(int node, vector<vec_int> &path_group, int depth)
{
    if (depth > LB && target_set.count(node))
    {
        vec_int downstream_path;
        return make_tuple(1000000, 0, downstream_path);
    }
    // cerr << "node:" << node << " depth:" << depth << endl;
    int result = 0;
    int downstream_node_count = 0;
    // 自身も含めたノードの数:
    int upstream_node_count = 1 + depth;

    if (children_set[node].size() == 0 && !target_set.count(node))
    {
        return {0, 0, {}};
    }

    vec_int downstream_node_counts;
    vector<vec_int> down_paths;

    int invalid_leaf_count = 0;
    for (auto next_pos : children_set[node].data)
    {
        auto [next_result, next_downstream_node_count, down_path] = reconstruct_tree(next_pos, path_group, depth + 1);
        result += next_result;
        downstream_node_count += next_downstream_node_count;
        if (next_downstream_node_count == 0 && next_result == 0)
        {
            invalid_leaf_count++;
        }
        if (next_downstream_node_count > 0)
        {

            if (downstream_node_counts.size() > 0)
            {
                int idx = -1;
                for (int i = 0; i < downstream_node_counts.size(); i++)
                {
                    if (downstream_node_counts[i] + next_downstream_node_count + upstream_node_count <= LB)
                    {
                        idx = i;
                        break;
                    }
                }
                if (idx != -1)
                {
                    downstream_node_counts[idx] += next_downstream_node_count;
                    down_paths[idx].insert(down_paths[idx].end(), down_path.begin(), down_path.end());
                }
                else
                {
                    downstream_node_counts.push_back(next_downstream_node_count);
                    down_paths.push_back(down_path);
                }
            }
            else
            {
                downstream_node_counts.push_back(next_downstream_node_count);
                down_paths.push_back(down_path);
            }
        }
    }
    if (children_set[node].size() == invalid_leaf_count && !target_set.count(node) && result == 0 && downstream_node_count == 0)
    {

        return {0, 0, {}};
    }

    if (children_set[node].size() - invalid_leaf_count > 0 && downstream_node_count == 0)
    {
        vec_int downstream_path;
        return make_tuple(result, 0, downstream_path);
    }

    if (downstream_node_count + upstream_node_count > LB && downstream_node_count > 0)
    {
        vec_int upstream_path = {};
        int tmp = node;
        while (tmp != -1)
        {
            upstream_path.push_back(tmp);
            tmp = parent[tmp];
        }

        // このときには分割を行う
        int cnt = downstream_node_counts.size() / 2;
        vector<pair<int, vec_int>> downstream_node_count_pairs;
        rep(i, downstream_node_counts.size())
        {
            downstream_node_count_pairs.push_back({downstream_node_counts[i], down_paths[i]});
        }
        sort(downstream_node_count_pairs.begin(), downstream_node_count_pairs.end(), greater<pair<int, vec_int>>());

        result += cnt * upstream_node_count;
        rep(i, cnt * 2)
        {
            result += downstream_node_count_pairs[i].first;
            vec_int tmp_path = upstream_path;
            tmp_path.insert(tmp_path.end(), downstream_node_count_pairs[i].second.begin(), downstream_node_count_pairs[i].second.end());
            path_group.push_back(tmp_path);
        }

        if (downstream_node_counts.size() % 2 == 0)
        {
            vec_int downstream_path;
            return {result, 0, downstream_path};
        }
        else
        {
            vec_int tmp = downstream_node_count_pairs[cnt * 2].second;
            tmp.push_back(node);
            return {result, downstream_node_count_pairs[cnt * 2].first + 1, tmp};
        }
    }
    vec_int path;
    rep(i, down_paths.size())
    {
        if (down_paths[i].size() == 0)
            continue;
        path.insert(path.end(), down_paths[i].begin(), down_paths[i].end());
    }

    path.push_back(node);
    return {
        result,
        downstream_node_count + 1,
        path};
}

int TreeState::calc_score(bool verbose = false)
{

    // スコアのルールは
    // 上流から下流に向かってみて行った時、
    // 1. 深さがLBを超えたらアウト
    // 2. あるノードで、以下の数を数える:
    // そのノードよりも上流のノードの数
    // そのノードよりも下流のノードの数
    // これがLBを超えたらその地点でスコアの計算が起こる
    // 2つをセットにして、サイズを共通部分 + 残りの部分、というような方法で計算ができる

    auto [result, res] = rec_calc_score(center_node, 0);
    // cerr << result + res << " " << result2 + res2 << endl;
    /*
    if (result + res != result2 + res2)
        throw "";
        */

    return result + res;
}

pair<vector<vec_int>, int> TreeState::annealing(int time_ms, vector<int> pre_used_nodes)
{
    for (auto pos : pre_used_nodes)
    {
        target_set.erase(pos);
    }
    // 初期状態を作る
    // 各有効なノードについてcenterから最短経路を結ぶ

    // 中心のノードから各ノードへの最短経路を求める
    // 基本的にはdepthが大きくなる方向に木を伸ばしていく

    parent = vec_int(N, -1);

    queue<int> que;
    // que.push(center_node);
    vector<P> node_dist_pair;
    for (auto node : node_groups.data)
    {
        node_dist_pair.push_back(P(dist_pair[center_node][node], node));
    }
    sort(node_dist_pair.begin(), node_dist_pair.end());
    reverse(node_dist_pair.begin(), node_dist_pair.end());
    vector<int> children_count(N, 0);
    set<int> node_set(node_groups.data.begin(), node_groups.data.end());

    cerr << "aa" << endl;
    for (auto [dist, node] : node_dist_pair)
    {
        if (dist == 0)
            break;

        vector<P> parent_candidates;
        for (auto next : G[node])
        {
            if (node_groups.count(next) && dist_pair[center_node][next] < dist)
            {
                parent_candidates.push_back(P(children_count[next], next));
            }
        }
        sort(parent_candidates.begin(), parent_candidates.end());
        if (parent_candidates.size() == 0)
            continue;
        // cerr << parent_candidates.size() << endl;

        int parent_node = parent_candidates[ro.get_rand(0, parent_candidates.size())].second;
        parent[node] = parent_node;
        children_count[parent_node]++;
        children_set[parent_node].insert(node);
    }

    // 点数を計算する
    int score = calc_score(false);

    double T0 = time_ms > 500 ? 0.2 : 0.2;
    double T1 = 0.01;
    int iter = 0;
    int start_time = get_time(false);
    int time_limit = start_time + time_ms;

    while (true)
    {
        float ct = get_time(false);
        if (ct >= time_limit)
        {
            break;
        }

        int node, candidate, original_parent;
        int while_count = 0;
        while (while_count < 100)
        {
            while_count++;
            node = node_groups.data[ro.get_rand(0, node_groups.size())];

            set<int> descendants;
            queue<int> que;
            que.push(node);
            while (que.size() > 0)
            {
                int pos = que.front();
                que.pop();
                descendants.insert(pos);
                for (auto next_pos : children_set[pos].data)
                {
                    que.push(next_pos);
                }
            }

            vec_int candidate_vec;
            original_parent = parent[node];
            // TODO 子孫でないかを判定するのにこれが必要って本当?
            for (auto next : G[node])
            {
                if (descendants.count(next) == 0 && node_groups.count(next) && next != parent[node] && (children_set[next].size() > 0 || parent[next] != -1))
                {
                    candidate_vec.push_back(next);
                }
            }
            if (candidate_vec.size() == 0)
                continue;

            candidate = candidate_vec[ro.get_rand(0, candidate_vec.size())];
            break;
        }
        if (while_count == 100)
            continue;
        parent[node] = candidate;
        children_set[candidate].insert(node);
        children_set[original_parent].erase(node);

        vec_int changed_nodes;
        queue<int> que;
        que.emplace(candidate);
        que.emplace(original_parent);
        while (que.size() > 0)
        {
            int pos = que.front();
            que.pop();
            changed_nodes.push_back(pos);
            if (parent[pos] >= 0)
                que.push(parent[pos]);
        }
        que.emplace(node);
        while (que.size() > 0)
        {
            int pos = que.front();
            que.pop();
            changed_nodes.push_back(pos);
            for (auto next : children_set[pos].data)
            {
                que.push(next);
            }
        }

        vector<P> original_cach;
        for (auto nd : changed_nodes)
        {
            original_cach.push_back(score_cache[nd]);
            is_changed[nd] = true;
        }

        int after_score = calc_score(false);
        int diff_score = after_score - score;
        if (is_valid_transition(-diff_score, ct, T0, T1, time_limit))
        {
            score = after_score;
        }
        else
        {
            parent[node] = original_parent;
            children_set[candidate].erase(node);
            children_set[original_parent].insert(node);
            rep(i, changed_nodes.size())
            {
                score_cache[changed_nodes[i]] = original_cach[i];
            }
        }
        iter++;
    }
    cerr << "iter:" << iter << endl;

    vector<vec_int> path_group;

    auto [result, _, path] = reconstruct_tree(center_node, path_group, 0);
    if (path.size() > 0)
        path_group.push_back(path);

    vector<vec_int> ret;
    for (auto path : path_group)
    {
        vec_int tmp = {center_node};
        path.erase(find(path.begin(), path.end(), center_node));
        tmp.insert(tmp.end(), path.begin(), path.end());
        ret.push_back(tmp);
    }
    cerr << "size:" << score << endl;
    return make_pair(ret, score);
}

vector<vec_int> TreeState::append_edges(vector<vec_int> ret)
{
    // 500msくらい使って、連続するノードをまとめられるかを試してみる
    int current_size0 = matched_node_size(ret);
    int res_nodes = LA - current_size0;

    vector<vec_int> additional_paths = make_additional_paths(res_nodes, center_node, ret, 200);
    for (auto tmp : additional_paths)
    {
        ret.push_back(tmp);
    }

    int current_size = matched_node_size(ret);
    vector<vec_int> additional_paths2 = make_additional_paths(LA - current_size, center_node, ret, 80);
    for (auto tmp : additional_paths2)
    {
        ret.push_back(tmp);
    }

    return ret;
}

vec_int construct_tree(vec_int &nodes, vector<vec_int> &dist_pair, vector<vec_int> &G)
{
    int center_node = nodes[0];
    vec_int nodes2 = nodes;
    shuffle(nodes2.begin(), nodes2.end(), mt);

    // nodesを使ってmstを作る
    // プリム法でいいかな
    vec_int ret = {nodes2[0]};

    for (int i = 1; i < nodes2.size(); i++)
    {
        // 最も近いノードに行く
        int min_dist = 1000000;
        int nearest_node = -1;
        for (auto nd : ret)
        {
            int distance = dist_pair[nd][nodes2[i]];
            if (distance < min_dist)
            {
                min_dist = distance;
                nearest_node = nd;
            }
        }
        if (min_dist == 0)
            continue;

        // 最も近いノードに向かって繋ぐ
        ret.push_back(nodes2[i]);
        int current = nodes2[i];
        for (int i = 0; i < min_dist - 1; i++)
        {
            int next;
            for (auto nd : G[current])
            {
                if (dist_pair[nearest_node][nd] == min_dist - i - 1)
                {
                    next = nd;
                    break;
                }
            }
            ret.push_back(next);
            current = next;
        }
    }
    rep(i, ret.size())
    {
        if (ret[i] == center_node)
        {
            swap(ret[0], ret[i]);
            break;
        }
    }
    return ret;
}

vector<vec_int> TreeState::make_additional_paths(int res_nodes, int root_node, vector<vec_int> &current_paths, float time_limit)
{
    int cnt = res_nodes / LB;
    if (cnt <= 0)
        return {};

    vector<set<int>> path_sets(current_paths.size());
    vector<vec_int> belonging_paths(N, vec_int());
    for (int i = 0; i < current_paths.size(); i++)
    {
        for (auto node : current_paths[i])
        {
            path_sets[i].insert(node);
            belonging_paths[node].push_back(i);
        }
    }

    vector<bool> used(T, false);
    int default_shortcut = 0;
    for (int turn = 0; turn < T - 1; turn++)
    {
        int node = t[turn];
        int next_node = t[turn + 1];
        for (auto path : belonging_paths[node])
        {
            if (path_sets[path].count(next_node))
            {
                used[turn] = true;
                used[turn + 1] = true;
                default_shortcut++;
                break;
            }
        }
    }
    cerr << "default_shortcut:" << default_shortcut << endl;

    // vector<vec_int> containing_turns(cnt);
    vector<vec_int> containing_nodes(cnt, {root_node});
    vector<vec_int> trees(cnt, {root_node});
    vector<int> node_counts(cnt, 0);
    vector<int> tree_sizes(cnt, 1);
    int score = 0;
    int tot_size = 0;

    // スコアは受け入れることができたノードの数で決めることにする

    float start_time = get_time(false);
    int threshold = LB / 4;
    map<P, int> target_map;
    rep(i, T - 1)
    {
        target_map[P(t[i], t[i + 1])] = i;
    }

    while (true)
    {
        if (get_time(false) > time_limit + start_time)
            break;

        int rng = ro.get_rand(0, 4);
        if (rng == 0)
        {
            int idx = ro.get_rand(0, cnt);

            int turn = -1;
            int fail_count = 0;
            while (fail_count < 50)
            {
                turn = ro.get_rand(0, T - 1);
                int node1 = t[turn];
                int node2 = t[turn + 1];
                if (dist_pair[node1][node2] >= threshold)
                {
                    fail_count++;
                    continue;
                }
                if (used[turn] || used[turn + 1])
                    continue;
                break;
            }
            if (fail_count == 50)
            {
                threshold++;
                continue;
            }

            vec_int tmp_nodes = containing_nodes[idx];
            tmp_nodes.push_back(t[turn]);
            tmp_nodes.push_back(t[turn + 1]);

            vec_int tree = construct_tree(tmp_nodes, dist_pair, G);
            if (tree.size() <= LB)
            {
                score++;
                containing_nodes[idx] = tmp_nodes;
                trees[idx] = tree;
                used[turn] = true;
                used[turn + 1] = true;
                node_counts[idx] += 1;
                tot_size -= tree_sizes[idx];
                tot_size += tree.size();
                tree_sizes[idx] = tree.size();
            }
            else
            {
                continue;
            }
        }
        else if (rng == 1)
        {
            // 移動させる
            if (cnt < 2)
                continue;

            int from = ro.get_rand(0, cnt);
            int to = ro.get_rand(0, cnt - 1);
            if (to >= from)
                to++;

            vec_int from_nodes = containing_nodes[from];
            vec_int to_nodes = containing_nodes[to];
            if (from_nodes.size() == 1 || to_nodes.size() == 1)
                continue;

            // cerr << "from_nodes:" << from_nodes.size() << " to_nodes:" << to_nodes.size() << endl;

            int from_idx = ro.get_rand(0, from_nodes.size() / 2);
            int move_node1 = from_nodes[from_idx * 2 + 1];
            int move_node2 = from_nodes[from_idx * 2 + 2];

            // from_nodes.erase(from_nodes.begin() + from_idx * 2 + 1, from_nodes.begin() + from_idx * 2 + 3);
            vec_int new_from;
            for (int i = 0; i < from_nodes.size(); i++)
            {
                if (i == from_idx * 2 + 1 || i == from_idx * 2 + 2)
                    continue;
                new_from.push_back(from_nodes[i]);
            }
            from_nodes = new_from;
            to_nodes.push_back(move_node1);
            to_nodes.push_back(move_node2);

            vec_int tree_from = construct_tree(from_nodes, dist_pair, G);
            vec_int tree_to = construct_tree(to_nodes, dist_pair, G);

            int before_size = tree_sizes[from] + tree_sizes[to];
            int after_size = tree_from.size() + tree_to.size();
            if (after_size <= before_size && tree_from.size() <= LB && tree_to.size() <= LB)
            {
                containing_nodes[from] = from_nodes;

                containing_nodes[to] = to_nodes;
                trees[from] = tree_from;
                trees[to] = tree_to;
                tot_size -= before_size;
                tot_size += after_size;
                tree_sizes[from] = tree_from.size();
                tree_sizes[to] = tree_to.size();
                node_counts[from] -= 1;
                node_counts[to] += 1;
            }
        }
        else if (rng == 2)
        {

            int from = ro.get_rand(0, cnt);
            vec_int from_nodes = containing_nodes[from];
            if (from_nodes.size() == 1)
                continue;

            int from_idx = ro.get_rand(0, from_nodes.size() / 2);
            int move_node1 = from_nodes[from_idx * 2 + 1];
            int move_node2 = from_nodes[from_idx * 2 + 2];

            vec_int new_from;
            for (int i = 0; i < from_nodes.size(); i++)
            {
                if (i == from_idx * 2 + 1 || i == from_idx * 2 + 2)
                    continue;
                new_from.push_back(from_nodes[i]);
            }

            int idx = ro.get_rand(0, cnt);
            int turn = -1;
            int fail_count = 0;
            while (fail_count < 50)
            {
                turn = ro.get_rand(0, T - 1);
                int node1 = t[turn];
                int node2 = t[turn + 1];
                if (dist_pair[node1][node2] >= threshold)
                {
                    fail_count++;
                    continue;
                }
                if (used[turn] || used[turn + 1])
                    continue;
                break;
            }
            if (fail_count == 50)
            {
                threshold++;
                continue;
            }

            new_from.push_back(t[turn]);
            new_from.push_back(t[turn + 1]);

            vec_int tree = construct_tree(new_from, dist_pair, G);
            int original_size = trees[idx].size();
            int after_size = tree.size();
            if (after_size <= original_size)
            {
                containing_nodes[from] = new_from;
                trees[from] = tree;
                used[turn] = true;
                used[turn + 1] = true;
                tot_size -= tree_sizes[from];
                tot_size += tree.size();
                tree_sizes[from] = tree.size();

                int tn = target_map[P(move_node1, move_node2)];
                used[tn] = false;
                used[tn + 1] = false;
            }
        }
        else if (rng == 3)
        {
            // 移動させる
            if (cnt < 2)
                continue;

            int from = ro.get_rand(0, cnt);
            int to = ro.get_rand(0, cnt - 1);
            if (to >= from)
                to++;

            vec_int from_nodes = containing_nodes[from];
            vec_int to_nodes = containing_nodes[to];
            if (from_nodes.size() == 1 || to_nodes.size() == 1)
                continue;

            // cerr << "from_nodes:" << from_nodes.size() << " to_nodes:" << to_nodes.size() << endl;

            int from_idx = ro.get_rand(0, from_nodes.size() / 2);
            int from_move_node1 = from_nodes[from_idx * 2 + 1];
            int from_move_node2 = from_nodes[from_idx * 2 + 2];

            int to_idx = ro.get_rand(0, to_nodes.size() / 2);
            int to_move_node1 = to_nodes[to_idx * 2 + 1];
            int to_move_node2 = to_nodes[to_idx * 2 + 2];

            // from_nodes.erase(from_nodes.begin() + from_idx * 2 + 1, from_nodes.begin() + from_idx * 2 + 3);
            vec_int new_from;
            for (int i = 0; i < from_nodes.size(); i++)
            {
                if (i == from_idx * 2 + 1 || i == from_idx * 2 + 2)
                    continue;
                new_from.push_back(from_nodes[i]);
            }
            vec_int new_to;
            for (int i = 0; i < to_nodes.size(); i++)
            {
                if (i == to_idx * 2 + 1 || i == to_idx * 2 + 2)
                    continue;
                new_to.push_back(to_nodes[i]);
            }
            new_from.push_back(to_move_node1);
            new_from.push_back(to_move_node2);
            new_to.push_back(from_move_node1);
            new_to.push_back(from_move_node2);

            vec_int tree_from = construct_tree(new_from, dist_pair, G);
            vec_int tree_to = construct_tree(new_to, dist_pair, G);

            int before_size = tree_sizes[from] + tree_sizes[to];
            int after_size = tree_from.size() + tree_to.size();
            if (after_size <= before_size && tree_from.size() <= LB && tree_to.size() <= LB)
            {
                containing_nodes[from] = new_from;
                containing_nodes[to] = new_to;
                trees[from] = tree_from;
                trees[to] = tree_to;
                tot_size -= before_size;
                tot_size += after_size;
                tree_sizes[from] = tree_from.size();
                tree_sizes[to] = tree_to.size();
            }
        }
    }

    // 最後にもう一つを付け加えられないかをチェックしていく

    rep(i, cnt)
    {
        auto nodes = containing_nodes[i];

        int cnt2 = nodes.size() / 2;
        rep(j, cnt2)
        {
            int node1 = nodes[j * 2 + 1];
            int node2 = nodes[j * 2 + 2];
            int turn = target_map[P(node1, node2)];
            int new_node = turn >= T - 2 ? -1 : t[turn + 2];
            if (new_node == -1)
                continue;
            if (!node_groups.count(new_node))
                continue;

            auto new_nodes = nodes;
            new_nodes.push_back(new_node);
            auto new_tree = construct_tree(new_nodes, dist_pair, G);
            if (new_tree.size() <= LB)
            {
                cerr << "found!!!!" << endl;
                containing_nodes[i] = new_nodes;
                node_counts[i] += 1;
                tot_size -= tree_sizes[i];
                tot_size += new_tree.size();
                tree_sizes[i] = new_tree.size();
                trees[i] = new_tree;
                score++;
                nodes = new_nodes;
            }
        }
    }

    int reduced = 0;
    for (auto node : containing_nodes)
    {
        reduced += node.size() / 2;
    }
    cerr << "sc:" << score << "reduced:" << reduced << " res_nodes:" << res_nodes << endl;

    return trees;
}

pair<vector<vec_int>, bool> add_unconnected_nodes(vector<vec_int> &paths, vector<vec_int> &G, vector<vec_int> &dist_pair, int center_node)
{
    float start_time = get_time(false);
    set<int> used_nodes;
    set<int> original_nodes;
    for (auto path : paths)
    {
        for (auto node : path)
        {
            used_nodes.insert(node);
        }
    }
    set<int> not_included_nodes;
    rep(i, T)
    {
        if (used_nodes.count(t[i]) == 0)
        {
            not_included_nodes.insert(t[i]);
        }
    }
    if (!used_nodes.count(0))
    {
        not_included_nodes.insert(0);
    }
    int check = 4;

    vector<vec_int> ret;
    while (!not_included_nodes.empty())
    {
        if (get_time(false) > start_time + 100)
        {
            return pair(ret, false);
        }
        vec_int nodes = vec_int(not_included_nodes.begin(), not_included_nodes.end());
        int max_distance = 0;
        int farest_node = -1;
        for (auto node : nodes)
        {
            int distance = dist_pair[center_node][nodes[0]];
            if (distance > max_distance || distance == max_distance && ro.get_rand(0, 2) == 0)
            {
                max_distance = distance;
                farest_node = node;
            }
        }

        // farest_nodeから始めて、既存のノードにたどり着くまでのedgeを追加する
        vec_int path = {farest_node};
        set<int> belonging_nodes = {farest_node};

        while (true)
        {
            vec_int neighbors;
            vec_int not_used_neighbors;
            bool flag = false;
            for (auto node : path)
            {
                for (auto next : G[node])
                {
                    if (used_nodes.count(next) > 0)
                    {
                        flag = true;
                        break;
                    }
                    if (belonging_nodes.count(next) == 0)
                    {
                        neighbors.push_back(next);
                        if (!used_nodes.count(next))
                            not_used_neighbors.push_back(next);
                    }
                }
            }

            if (flag && path.size() == LB)
                break;
            /*
            if (flag || not_used_neighbors.size() == 0)
            {
                break;
            }
            */

            if (flag)
            {
                // neighborの中でnot_included_nodesがいたらそいつを追加する
                int nd = -1;
                for (auto node : neighbors)
                {
                    if (not_included_nodes.count(node) > 0)
                    {
                        nd = node;
                        break;
                    }
                }
                if (nd != -1)
                {
                    path.push_back(nd);
                    belonging_nodes.insert(nd);
                    continue;
                }
                if (nd == -1 && flag)
                {
                    break;
                }
            }
            int min_distance = INT_MAX;
            int min_distance_node = -1;
            for (auto node : neighbors)
            {
                int distance = dist_pair[center_node][node];
                if (distance < min_distance || distance == min_distance && ro.get_rand(0, 2) == 0)
                {
                    min_distance = distance;
                    min_distance_node = node;
                }
            }
            path.push_back(min_distance_node);
            belonging_nodes.insert(min_distance_node);
        }
        if (path.size() > LB)
        {
            check++;
            continue;
        }

        for (auto node : path)
        {
            not_included_nodes.erase(node);
        }
        ret.push_back(path);
    }

    return make_pair(ret, true);
}

vector<vec_int> make_additional_paths2(int res_nodes, vector<vec_int> &current_paths, float time_limit, vector<vec_int> &dist_pairs, vector<vec_int> &G)
{
    int cnt = res_nodes / LB;
    if (cnt <= 0)
        return {};

    vector<set<int>> path_sets(current_paths.size());
    vector<vec_int> belonging_paths(N, vec_int());
    for (int i = 0; i < current_paths.size(); i++)
    {
        for (auto node : current_paths[i])
        {
            path_sets[i].insert(node);
            belonging_paths[node].push_back(i);
        }
    }

    vector<bool> used(T, false);
    int default_shortcut = 0;
    for (int turn = 0; turn < T - 1; turn++)
    {
        int node = t[turn];
        int next_node = t[turn + 1];
        for (auto path : belonging_paths[node])
        {
            if (path_sets[path].count(next_node))
            {
                used[turn] = true;
                used[turn + 1] = true;
                default_shortcut++;
                break;
            }
        }
    }
    cerr << "default_shortcut:" << default_shortcut << endl;

    // vector<vec_int> containing_turns(cnt);
    vector<vec_int> containing_nodes(cnt);
    vector<vec_int> trees(cnt);
    vector<int> node_counts(cnt, 0);
    vector<int> tree_sizes(cnt, 0);
    int score = 0;
    int tot_size = 0;

    // スコアは受け入れることができたノードの数で決めることにする

    float start_time = get_time(false);
    int threshold = LB / 4;

    while (true)
    {
        if (get_time(false) > time_limit + start_time)
            break;

        if (ro.get_rand(0, 2) == 0)
        {
            int idx = ro.get_rand(0, cnt);

            int turn = -1;
            int fail_count = 0;
            while (fail_count < 50)
            {
                turn = ro.get_rand(0, T - 1);
                int node1 = t[turn];
                int node2 = t[turn + 1];
                if (dist_pairs[node1][node2] >= threshold)
                {
                    fail_count++;
                    continue;
                }
                if (used[turn] || used[turn + 1])
                    continue;
                break;
            }
            if (fail_count == 50)
            {
                threshold++;
                continue;
            }

            vec_int tmp_nodes = containing_nodes[idx];
            tmp_nodes.push_back(t[turn]);
            tmp_nodes.push_back(t[turn + 1]);
            cerr << "tmp_nodes:" << tmp_nodes.size() << endl;

            vec_int tree = construct_tree(tmp_nodes, dist_pairs, G);
            cerr << "tree_size:" << tree.size() << endl;
            if (tree.size() <= LB)
            {
                score++;
                containing_nodes[idx] = tmp_nodes;
                trees[idx] = tree;
                used[turn] = true;
                used[turn + 1] = true;
                node_counts[idx] += 1;
                tot_size -= tree_sizes[idx];
                tot_size += tree.size();
                tree_sizes[idx] = tree.size();
            }
            else
            {
                continue;
            }
        }
        else
        {
            if (cnt < 2)
                continue;

            int from = ro.get_rand(0, cnt);
            int to = ro.get_rand(0, cnt - 1);
            if (to >= from)
                to++;

            vec_int from_nodes = containing_nodes[from];
            vec_int to_nodes = containing_nodes[to];
            if (from_nodes.size() == 1 || to_nodes.size() == 1)
                continue;

            // cerr << "from_nodes:" << from_nodes.size() << " to_nodes:" << to_nodes.size() << endl;

            int from_idx = ro.get_rand(0, from_nodes.size() / 2);
            int move_node1 = from_nodes[from_idx * 2 + 0];
            int move_node2 = from_nodes[from_idx * 2 + 1];

            // from_nodes.erase(from_nodes.begin() + from_idx * 2 + 1, from_nodes.begin() + from_idx * 2 + 3);
            vec_int new_from;
            for (int i = 0; i < from_nodes.size(); i++)
            {
                if (i == from_idx * 2 + 1 || i == from_idx * 2 + 2)
                    continue;
                new_from.push_back(from_nodes[i]);
            }
            from_nodes = new_from;
            to_nodes.push_back(move_node1);
            to_nodes.push_back(move_node2);

            vec_int tree_from = construct_tree(from_nodes, dist_pairs, G);
            vec_int tree_to = construct_tree(to_nodes, dist_pairs, G);

            int before_size = tree_sizes[from] + tree_sizes[to];
            int after_size = tree_from.size() + tree_to.size();
            if (after_size <= before_size && tree_from.size() <= LB && tree_to.size() <= LB)
            {
                containing_nodes[from] = from_nodes;

                containing_nodes[to] = to_nodes;
                trees[from] = tree_from;
                trees[to] = tree_to;
                tot_size -= before_size;
                tot_size += after_size;
                tree_sizes[from] = tree_from.size();
                tree_sizes[to] = tree_to.size();
                node_counts[from] -= 1;
                node_counts[to] += 1;
            }
            // cerr << "end" << endl;
        }
    }
    int reduced = 0;
    for (auto node : containing_nodes)
    {
        reduced += node.size() / 2;
    }
    cerr << "sc:" << score << "reduced:" << reduced << " res_nodes:" << res_nodes << endl;

    return trees;
}




vec_int list_up_farest_points(int start, vector<vec_int> &dist_pair, vector<vec_int> &G)
{

    vec_int result;
    queue<P> que;
    que.push(P(start, 0));
    set<int> visited;
    while (!que.empty())
    {
        auto [pos, dist] = que.front();
        que.pop();
        if (visited.count(pos) > 0)
            continue;
        visited.insert(pos);
        if (dist == LB - 1)
        {
            result.push_back(pos);
            continue;
        }

        for (auto next_pos : G[pos])
        {
            if (visited.count(next_pos) == 0 && dist_pair[start][next_pos] == dist + 1)
            {
                que.push(P(next_pos, dist + 1));
            }
        }
    }
    return result;
}

int matched_size(vector<vec_int> paths)
{
    int paths_size = paths.size();
    vector<vec_int> common_nodes(paths_size, vec_int(paths_size, 0));

    rep(i, paths_size)
    {
        set<int> path_set(paths[i].begin(), paths[i].end());
        for (int j = i + 1; j < paths_size; j++)
        {
            for (auto node : paths[j])
            {
                if (path_set.count(node))
                {
                    common_nodes[i][j]++;
                    common_nodes[j][i]++;
                }
            }
        }
    }

    int result = 0;
    int count = 0;
    vector<bool> used(paths_size, false);

    while (2 * count < paths_size - 1)
    {
        int max_common = -1;
        int node1 = -1;
        int node2 = -1;
        for (int i = 0; i < paths_size; i++)
        {
            if (used[i])
                continue;
            for (int j = i + 1; j < paths_size; j++)
            {
                if (used[j])
                    continue;
                if (common_nodes[i][j] > max_common)
                {
                    max_common = common_nodes[i][j];
                    node1 = i;
                    node2 = j;
                }
            }
        }
        result += paths[node1].size() + paths[node2].size() - max_common;
        used[node1] = true;
        used[node2] = true;
        count++;
    }

    rep(i, paths_size)
    {
        if (!used[i])
        {
            result += paths[i].size();
            break;
        }
    }
    return result;
}

void small_lb_strategy(vector<vec_int> &G, vector<vec_int> &dist_pair)
{
    // やるべきことは基本的に一個ずつ辺を追加していくこと
    // 辺はある点から最も遠いところに伸びるようにする
    // 始点と終点を共有するのは避ける
    // できるだけ橙色の点を通るようにする
    // 始点は橙色の点に限定する
    // 途中まで共有していて、最後のターンで二股に分かれるのはOK

    // 各点について、最も遠い点とそこまでの最短パスを全て列挙する
    vector<vector<vec_int>> farest_paths(N);

    PseudoSet target_nodes(N);
    rep(i, T)
    {
        target_nodes.insert(t[i]);
    }

    int count = 0;
    for (auto start : target_nodes.data)
    {
        auto farest_points = list_up_farest_points(start, dist_pair, G);
        for (auto end : farest_points)
        {
            farest_paths[start].push_back(find_shortest_path(G, start, end));
            count++;
        }
    }

    // 各farest_pathsのうち、最もtarget_nodeのうちvisitedに含まれないpathを選ぶ
    PseudoSet visited(N);
    PseudoSet start_and_end_points(N);
    // また、start地点が始点・終点になっているものは選択しない

    int cnt = 0;
    vector<vec_int> paths;
    int determined = 0;
    int estimated = 0;
    while (determined + estimated < LA - LB)
    {
        int best_start_point = -1;
        int best_index = -1;
        float best_score = 0;
        cerr << "start:" << cnt << endl;

        rep(start_node, N)
        {
            if (start_and_end_points.count(start_node))
                continue;

            for (int i = 0; i < farest_paths[start_node].size(); i++)
            {
                auto path = farest_paths[start_node][i];

                float tmp_score = 0;
                for (auto node : path)
                {
                    if (target_nodes.count(node) && visited.count(node) == 0)
                    {
                        tmp_score += 1;
                    }
                    else if (visited.count(node) == 0)
                    {
                        tmp_score += 0.3;
                    }
                }
                if (tmp_score > best_score)
                {
                    best_score = tmp_score;
                    best_start_point = start_node;
                    best_index = i;
                }
            }
        }
        cerr << "best_score:" << best_score << " best_index:" << best_index << " " << best_start_point << endl;
        if (best_score == 0)
        {
            break;
        }
        cerr << "aaa" << endl;
        paths.push_back(farest_paths[best_start_point][best_index]);
        for (auto node : farest_paths[best_start_point][best_index])
        {
            visited.insert(node);
        }
        start_and_end_points.insert(farest_paths[best_start_point][best_index][0]);
        start_and_end_points.insert(farest_paths[best_start_point][best_index].back());
        cnt++;
        cerr << "cnt" << cnt << endl;

        estimated += farest_paths[best_start_point][best_index].size();
        cerr << estimated << endl;
        if (cnt % 10 == 0)
        {
            cerr << "cnt" << endl;
            determined = matched_size(paths);
            estimated = 0;
        }
    }

    /*
    while (true)
    {
        int start_point = ro.get_rand(0, N);
        int best_index = ro.get_rand(0, farest_paths[start_point].size());
        paths.push_back(farest_paths[start_point][best_index]);

        int sz = matched_size(paths);
        cerr << "sz:" << sz << endl;

        if (sz > LA)
        {
            paths.pop_back();
            break;
        }
    }
    */

    cerr << "node_count:" << matched_size(paths) << endl;

    cerr << "count:" << count << endl;
    cerr << "path count:" << cnt << endl;

    cout << "# graph ";
    for (auto path : paths)
    {
        cout << path.size() << " ";
        for (auto node : path)
        {
            cout << node << " ";
        }
    }
    cout << endl;
    for (auto path : paths)
    {
        cout << "# edge ";
        for (auto node : path)
        {
            cout << node << " ";
        }
        cout << endl;
    }
    auto [edges, A_vec, edge_starts] = make_edges(G, dist_pair, paths);
    auto result = find_operations(G, edges, edge_starts, A_vec);
    cerr << "score:" << result.score << endl;
    rep(i, LA)
    {
        cout << result.A_vec[i] << " ";
    }
    cout << endl;

    int turn_start = 0;
    int last_manipulation = -1;
    int score2 = 0;
    rep(turn, T)
    {
        for (auto p : result.first_moves[turn])
        {
            cout << "m " << p << endl;
        }
        cout << "# turn:" << turn << " min_length:" << dist_pair[turn_start][t[turn]] << endl;
        turn_start = t[turn];
        score2 += result.manipulations[turn].size();
        rep(i, result.manipulations[turn].size())
        {
            int edge_index = result.manipulations[turn][i];
            int left = result.edge_starts[edge_index];
            int right = min(left + LB - 1, LA - 1);
            int l = right - left + 1;
            if (last_manipulation != edge_index)
                cout << "s " << l << " " << left << " " << 0 << endl;
            last_manipulation = edge_index;
            for (auto p : result.moves[turn][i])
            {
                cout << "m " << p << endl;
            }
        }
    }

    return;
}

void multi_tree_solution(vector<vec_int> &G, vector<vec_int> &dist_pairs)
{
    int center_node = find_nearest_point(500, 500);
    vec_int center_nodes = {center_node, find_nearest_point(200, 200), find_nearest_point(800, 200),
                            find_nearest_point(200, 800), find_nearest_point(800, 800)};
    center_nodes.push_back(find_nearest_point(500, 200));
    center_nodes.push_back(find_nearest_point(200, 500));
    center_nodes.push_back(find_nearest_point(500, 800));
    center_nodes.push_back(find_nearest_point(800, 500));
    /*
    center_nodes.push_back(find_nearest_point(100, 100));
    center_nodes.push_back(find_nearest_point(100, 900));
    center_nodes.push_back(find_nearest_point(900, 100));
    center_nodes.push_back(find_nearest_point(900, 900));
     */
    /*
    center_nodes.push_back(find_nearest_point(800, 1000));
    center_nodes.push_back(find_nearest_point(1000, 800));
    rep(i, 30)
    {
        center_nodes.push_back(find_nearest_point(ro.get_rand(0, 1000), ro.get_rand(0, 1000)));
    }
    */
    vector<vec_int> paths;
    vec_int A_vec;
    vec_int edge_starts;
    vector<Edge> edges;
    set<int> used;

    for (auto center : center_nodes)
    {

        set<int> target_node_set(t.begin(), t.end());
        TreeAnnealer ta(G, target_node_set);

        PseudoSet node_groups(N);
        rep(i, N)
        {
            if (dist_pairs[center][i] <= LB)
            {
                if (used.count(i) && dist_pairs[center][i] >= LB)
                    continue;
                if (dist_pairs[center][i] == LB && ro.get_rand(0, 200) == 0)
                {
                    continue;
                }
                node_groups.insert(i);
            }
        }
        // centerから到達可能なものだけを選ぶ
        PseudoSet tmp_set(N);
        queue<int> que;
        que.push(center);
        while (que.size() > 0)
        {
            auto node = que.front();
            que.pop();
            if (tmp_set.count(node))
            {
                continue;
            }

            tmp_set.insert(node);
            for (auto next : G[node])
            {
                if (node_groups.count(next))
                {
                    que.push(next);
                }
            }
        }
        node_groups = tmp_set;

        TreeState tree_state = TreeState(center, node_groups, G);
        auto [tmp_paths, _] = tree_state.annealing(100, {});
        tmp_paths = erase_bad_cospa(tmp_paths);
        for (auto path : tmp_paths)
        {
            paths.push_back(path);
            for (auto node : path)
            {
                used.insert(node);
            }
        }
        cout << "# edge ";
        for (auto path : tmp_paths)
        {
            for (auto node : path)
            {
                cout << node << " ";
            }
        }
        cout << endl;

        auto tt = match_edges_and_make_A_vec(tmp_paths, G, dist_pairs, edges, A_vec, edge_starts);
        edges = tt.edges;
        A_vec = tt.A_vec;
        edge_starts = tt.start_on_A_vec;
    }
    set<int> used_nodes;
    for (auto edge : edges)
    {
        for (auto node : edge.path)
        {
            used_nodes.insert(node);
        }
    }

    vector<vec_int> additional_paths;
    for (auto target_node : t)
    {
        if (used_nodes.count(target_node) == 0)
        {
            // target_nodeから出発して、既存のノードの隣のノードに到達するまでのパスを追加する

            vec_int prev = vec_int(N, -1);
            queue<P> que;
            que.push(P(target_node, -1));
            vec_int current_path;
            while (que.size() > 0)
            {
                auto [node, p] = que.front();
                cerr << "aaa" << " " << node << " " << p << endl;
                que.pop();
                if (prev[node] != -1)
                    continue;
                prev[node] = p;
                if (used_nodes.count(node))
                {
                    while (node != -1)
                    {
                        current_path.push_back(node);
                        node = prev[node];
                        // cerr << "node:" << node << endl;
                    }
                    cerr << "end" << endl;
                    break;
                }
                for (auto neighbor : G[node])
                {
                    if (prev[neighbor] == -1 && neighbor != target_node)
                    {
                        que.push(P(neighbor, node));
                    }
                }
            }
            // current_path.pop_back();

            additional_paths.push_back(current_path);
        }
    }

    if (additional_paths.size() > 0)
    {
        auto tt = match_edges_and_make_A_vec(additional_paths, G, dist_pairs, edges, A_vec, edge_starts);

        edges = tt.edges;
        A_vec = tt.A_vec;
        edge_starts = tt.start_on_A_vec;
    }

    /*
    for (auto path : paths)
    {
        cout << "# edge ";
        for (auto node : path)
        {
            cout << node << " ";
        }
        cout << endl;
    }
    */

    /*
     rep(i, paths.size())
     {
         edge_starts.push_back(A_vec.size());
         for (auto node : paths[i])
         {
             A_vec.push_back(node);
         }
         edges.push_back(Edge{paths[i][0], paths[i].back(), paths[i]});
     }
     */
    cerr << "find operations" << endl;
    Result result = find_operations(G, edges, edge_starts, A_vec);
    cerr << "score:" << result.score << endl;

    rep(i, max((int)result.A_vec.size(), LA))
    {
        if (i < result.A_vec.size())
            cout << result.A_vec[i] << " ";
        else
            cout << 0 << " ";
    }
    cout << endl;
    cerr << result.A_vec.size() << endl;

    int turn_start = 0;
    int last_manipulation = -1;
    int score2 = 0;
    rep(turn, T)
    {
        for (auto p : result.first_moves[turn])
        {
            cout << "m " << p << endl;
        }
        cout << "# turn:" << turn << " min_length:" << dist_pairs[turn_start][t[turn]] << endl;
        turn_start = t[turn];
        score2 += result.manipulations[turn].size();
        rep(i, result.manipulations[turn].size())
        {
            int edge_index = result.manipulations[turn][i];
            int left = result.edge_starts[edge_index];
            int right = min(left + LB - 1, (int)result.A_vec.size() - 1);
            int l = right - left + 1;
            if (last_manipulation != edge_index)
            {
                cout << "# edge_index:" << edge_index << endl;
                cout << "# ";
                for (auto node : edges[edge_index].path)
                {
                    cout << node << " ";
                }
                cout << endl;

                cout << "s " << l << " " << left << " " << 0 << endl;
            }
            last_manipulation = edge_index;
            for (auto p : result.moves[turn][i])
            {
                cout << "m " << p << endl;
            }
        }
    }
}



double T0 = 500.;
double T1 = 0;

vector<vec_int> get_dist_pairs(vector<vec_int> &G)
{
    vector<vec_int> dist_pairs = vector<vec_int>(N, vec_int(N, INT_MAX));
    rep(i, N)
    {
        priority_queue<P, vector<P>, greater<P>> pq;
        pq.push(P(0, i));
        while (pq.size() > 0)
        {
            auto [cost, pos] = pq.top();
            pq.pop();
            if (dist_pairs[i][pos] <= cost)
                continue;
            dist_pairs[i][pos] = cost;
            for (auto next_pos : G[pos])
            {
                if (dist_pairs[i][next_pos] == INT_MAX)
                    pq.push(P(cost + 1, next_pos));
            }
        }
    }
    return dist_pairs;
}

struct Path
{
    int from_pos;
    int to_pos;
    vec_int nodes;
};

int main(int argc, char *argv[])
{
#ifdef OPTUNA_STUDY
    cerr << "optuna study, override parameters" << endl;
    T0 = stod(argv[1]);
    T1 = stod(argv[2]);
#endif

    get_time(true);

    cin >> N >> M >> T >> LA >> LB;
    u.resize(M);
    v.resize(M);
    rep(i, M) cin >> u[i] >> v[i];
    t.resize(T);
    rep(i, T) cin >> t[i];
    x.resize(N);
    y.resize(N);
    rep(i, N) cin >> x[i] >> y[i];
    // if (LB < 22)
    //    return 0;

    vector<vector<int>> G(N);
    rep(i, M)
    {
        G[u[i]].push_back(v[i]);
        G[v[i]].push_back(u[i]);
    }

    auto dist_pairs = get_dist_pairs(G);
    int from_pos = 0;
    int tot_distance = 0;
    rep(i, T)
    {
        tot_distance += dist_pairs[from_pos][t[i]];
        from_pos = t[i];
    }
    /*
    cerr << "total distance:" << tot_distance << endl;
    cerr << "lower_bound:" << tot_distance / LB << endl;
    set<int> t_set(t.begin(), t.end());
    cerr << "target_node size:" << t_set.size() << endl;
    */

    auto [center_node, max_distance] = find_edge_center(dist_pairs);
    cerr << "center_node:" << center_node << endl;
    cerr << "max_distance:" << max_distance << endl;

    if (max_distance <= LB)
    {

        PseudoSet node_groups(N);
        rep(i, N)
        {
            if (dist_pairs[center_node][i] <= LB)
            {
                node_groups.insert(i);
            }
        }

        TreeState tree_state0 = TreeState(center_node, node_groups, G);
        auto [path0, estimated_size] = tree_state0.annealing(200, {});

        if (estimated_size > LA)
            goto OUT;

        int best_score = INT_MAX;
        TreeState tree_state = TreeState(center_node, node_groups, G);
        vector<int> pre_used_nodes;
        vector<vec_int> pre_paths;
        if (LA > estimated_size)
        {
            double ratio = LA < 800 ? 1.3 : 1.6;
            pre_paths = tree_state.make_additional_paths((LA - estimated_size) * ratio, center_node, pre_paths, 550);
            for (auto path : pre_paths)
            {
                for (auto node : path)
                {
                    pre_used_nodes.push_back(node);
                }
            }
        }

        auto [paths, sc] = tree_state.annealing(500, pre_used_nodes);
        for (auto path : pre_paths)
        {
            paths.push_back(path);
            /*
            cout << "# edge ";
            for (auto nd : path)
            {
                cout << nd << " ";
            }
            cout << endl;
            */
        }
        paths = tree_state.append_edges(paths);
        cerr << "start_solve:" << get_time(false) << endl;
        // auto [result, edges] = solve(G, dist_pairs, paths, center_node);
        auto [edges, edge_starts, A_vec] = solve(G, dist_pairs, paths, center_node);
        if (A_vec.size() > LA)
        {
            paths = tree_state0.append_edges(path0);
            auto [edges2, edge_starts2, A_vec2] = solve(G, dist_pairs, paths, center_node);
            edges = edges2;
            edge_starts = edge_starts2;
            A_vec = A_vec2;
        }
        cerr << "find operation" << endl;

        auto result = find_operations(G, edges, edge_starts, A_vec);
        cerr << "end_solve:" << get_time(false) << endl;

        cout_answer(result, edges);
        cerr << "time:" << get_time(false) << endl;

        return 0;
    }
OUT:

    // small_lb_strategy(G, dist_pairs);

    vector<Edge> edges;
    Result result;

    if ((LB >= max_distance - 5 && max_distance < LB * 2 - 3) || LB >= 10)
    {
        for (int reduce = 0; reduce < 6; reduce++)
        {
            cerr << "reduce:" << reduce << endl;
            if (get_time(false) > 2000)
                break;
            PseudoSet node_groups(N);
            rep(i, N)
            {
                if (dist_pairs[center_node][i] <= LB - reduce / 2)
                {
                    if (reduce % 2 == 1 && dist_pairs[center_node][i] == LB - reduce / 2 && ro.get_rand(0, 2) == 0)
                        continue;
                    node_groups.insert(i);
                }
            }

            vector<int> pre_used_nodes;
            vector<vec_int> pre_paths;
            TreeState tree_state = TreeState(center_node, node_groups, G);
            if (LA > 800 && LB >= 12)
            {
                pre_paths = tree_state.make_additional_paths(LA - 750, center_node, pre_paths, 250);
                for (auto path : pre_paths)
                {
                    for (auto node : path)
                    {
                        pre_used_nodes.push_back(node);
                    }
                }
            }
            cerr << "reduce:" << reduce << " " << get_time(false) << endl;

            auto [paths, _] = tree_state.annealing(200, pre_used_nodes);
            for (auto path : pre_paths)
            {
                paths.push_back(path);
            }

            // 残り部分を追加する

            vector<Edge> edges;
            vec_int A_vec;
            vec_int edge_starts;
            auto tmp = match_edges_and_make_A_vec(paths, G, dist_pairs, edges, A_vec, edge_starts);
            vec_int center_nodes = {center_node};

            auto [new_paths, success] = add_unconnected_nodes(paths, G, dist_pairs, center_node);
            cerr << "new_paths:" << new_paths.size() << " " << get_time(false) << endl;
            if (!success)
                break;
            auto tmp2 = match_edges_and_make_A_vec(new_paths, G, dist_pairs, tmp.edges, tmp.A_vec, tmp.start_on_A_vec);

            int current_A_vec_length = tmp2.A_vec.size();
            cerr << "current_A_vec_length:" << current_A_vec_length << " LA:" << LA << endl;
            if (current_A_vec_length > LA)
                continue;
            int diff = LA - current_A_vec_length;

            vector<vec_int> additional_paths = tree_state.make_additional_paths(diff, center_node, paths, 150);
            tmp2 = match_edges_and_make_A_vec(additional_paths, G, dist_pairs, tmp2.edges, tmp2.A_vec, tmp2.start_on_A_vec);
            diff = LA - tmp2.A_vec.size();
            additional_paths = tree_state.make_additional_paths(diff, center_node, paths, 50);
            tmp2 = match_edges_and_make_A_vec(additional_paths, G, dist_pairs, tmp2.edges, tmp2.A_vec, tmp2.start_on_A_vec);
            cerr << "current_A_vec_length2:" << tmp2.A_vec.size() << " LA:" << LA << endl;

            if (tmp2.A_vec.size() <= LA)
            {
                cout << "# graph ";
                for (auto path : tmp2.edges)
                {
                    for (auto node : path.path)
                    {
                        cout << node << " ";
                    }
                }
                cout << endl;

                auto result = find_operations(G, tmp2.edges, tmp2.start_on_A_vec, tmp2.A_vec);
                cerr << "result found" << endl;
                vec_int tmp_a_vec = vec_int(LA, 0);
                rep(i, result.A_vec.size()) tmp_a_vec[i] = result.A_vec[i];

                rep(i, LA)
                {
                    cout << tmp_a_vec[i] << " ";
                }
                cout << endl;

                int turn_start = 0;
                int last_manipulation = -1;
                int score2 = 0;
                rep(turn, T)
                {
                    for (auto p : result.first_moves[turn])
                    {
                        cout << "m " << p << endl;
                    }
                    turn_start = t[turn];
                    score2 += result.manipulations[turn].size();
                    rep(i, result.manipulations[turn].size())
                    {
                        int edge_index = result.manipulations[turn][i];
                        int left = result.edge_starts[edge_index];
                        int right = min(left + LB - 1, LA - 1);
                        int l = right - left + 1;
                        if (last_manipulation != edge_index)
                        {
                            cout << "# edge_index:" << edge_index << endl;
                            cout << "# ";
                            for (auto node : tmp2.edges[edge_index].path)
                            {
                                cout << node << " ";
                            }
                            cout << endl;

                            cout << "s " << l << " " << left << " " << 0 << endl;
                        }
                        last_manipulation = edge_index;
                        for (auto p : result.moves[turn][i])
                        {
                            cout << "# edge_index:" << edge_index << endl;
                            cout << "m " << p << endl;
                        }
                    }
                }
                cerr << "score:" << score2 << endl;
                cerr << "time:" << get_time(false) << endl;

                return 0;
            }
        }
    }

    int best_score = INT_MAX;
    Result best_result;
    int count = 0;
    while (true)
    {
        // edges = additional_edges_long_pathes(G, dist_pairs);
        // auto [edge_starts, A_vec] = normal_A_vec(edges);
        // auto annealer = EdgeStrategyAnnealing();
        // auto [tmp_edges, edge_starts, A_vec] = annealer.annealing(G, dist_pairs);

        auto [tmp_edges, edge_starts, A_vec] = additional_edges_long_pathes(G, dist_pairs);
        // cerr << edges.size() << " " << edge_starts.back() << " " << A_vec.size() << endl;
        result = find_operations(G, tmp_edges, edge_starts, A_vec);
        if (result.score < best_score)
        {
            best_score = result.score;
            best_result = result;
            edges = tmp_edges;
        }
        count++;
        double average_time = get_time(false) / (double)count;

        if (get_time(false) > 2150 || get_time(false) + average_time > 2300)
            break;
    }
    result = best_result;

    cerr << "score:" << result.score << endl;
    cerr << "time:" << get_time(false) << endl;
    rep(i, LA)
    {
        cout << result.A_vec[i] << " ";
    }
    cout << endl;

    int turn_start = 0;
    int last_manipulation = -1;
    int score2 = 0;
    rep(turn, T)
    {
        for (auto p : result.first_moves[turn])
        {
            cout << "m " << p << endl;
        }
        cout << "# turn:" << turn << " min_length:" << dist_pairs[turn_start][t[turn]] << endl;
        turn_start = t[turn];
        score2 += result.manipulations[turn].size();
        rep(i, result.manipulations[turn].size())
        {
            int edge_index = result.manipulations[turn][i];
            int left = result.edge_starts[edge_index];
            int right = min(left + LB - 1, LA - 1);
            int l = right - left + 1;
            if (last_manipulation != edge_index)
            {
                cout << "# expected nodes:";
                for (auto p : edges[edge_index].path)
                {
                    cout << p << " ";
                }
                cout << endl;
                cout << "s " << l << " " << left << " " << 0 << endl;
            }
            last_manipulation = edge_index;
            for (auto p : result.moves[turn][i])
            {
                cout << "m " << p << endl;
            }
        }
    }

    return 0;
}
