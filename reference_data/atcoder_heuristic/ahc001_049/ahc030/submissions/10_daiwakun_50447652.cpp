#pragma GCC target("avx2")
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include <iomanip>
#include <limits>
#include <list>
#include <queue>
#include <tuple>
#include <map>
#include <stack>
#include <set>
#include <unordered_set>
#include <bitset>
#include <functional>
#include <chrono>
#include <cassert>
#include <array>
#include <random>

uint32_t xorshift(){
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 886751234;
    uint32_t t;
    t = x ^ (x<<11);
    x = y; y = z; z = w;
    w ^= t ^ (t>>8) ^ (w>>19);
    return w;
}

static int randint(int l)
{
    return xorshift() % l;
}

static double rand01()
{
    return (xorshift() + 0.5) * (1.0 / UINT32_MAX);
}

class Timer{
    std::chrono::system_clock::time_point _start, _end;
    long long _sum = 0, _count = 0;

public:
    void start(){
        _start = std::chrono::system_clock::now();
    }

    void stop(){
        _end = std::chrono::system_clock::now();
    }

    void add(){
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        _sum += static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(now - _start).count());
        _count++;
    }

    long long sum(){
        return _sum / 1000;
    }

    std::string average(){
        if(_count == 0){
            return "NaN";
        }
        return std::to_string(_sum / 1000 / _count);
    }

    void reset(){
        _start = std::chrono::system_clock::now();
        _sum = 0;
        _count = 0;
    }

    inline int ms() const{
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        return static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(now - _start).count() / 1000);
    }

    inline int ns() const{
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        return static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(now - _start).count());
    }
};

Timer timer;


struct Problem
{
    int N, M;
    double eps;
    std::vector<std::vector<std::pair<int,int>>> poly;
    std::vector<std::vector<int>> map;
    std::vector<int> real_point;
    std::vector<std::pair<int,int>> answer;
    std::vector<double> rand_query;
    int cnt = 0;
    void Read(std::istream &input)
    {
        input >> N >> M >> eps;
        poly.resize(M);
        for(int i = 0; i < M; i++)
        {
            int d;
            input >> d;
            for(int j = 0; j < d; j++)
            {
                int y, x;
                input >> y >> x;
                poly[i].push_back({y, x});
            }
        }
        #ifdef ONLINE_JUDGE
        #else
        for(int i = 0; i < M; i++)
        {
            int y, x;
            input >> y >> x;
            real_point.push_back(y*N + x);
        }
        map.resize(N);
        for(int i = 0; i < N; i++)
        {
            map[i].resize(N);
            for(int j = 0; j < N; j++)
            {
                int a;
                input >> a;
                map[i][j] = a;
                if(a > 0) answer.push_back({i, j});
            }
        }
        for(int i = 0; i < N*N*2; i++)
        {
            double a;
            input >> a;
            rand_query.push_back(a);
        }
        #endif
    }
};



struct Interface
{
    Problem problem;
    int (*query_func)(Problem &, std::vector<std::pair<int,int>>&);
    int (*answer_func)(Problem &, std::vector<std::pair<int,int>>&);

    Interface(Problem &problem):problem(problem)
    {
        #ifdef ONLINE_JUDGE
        query_func = query_judge;
        answer_func = answer_judge;
        #else
        problem.cnt = 0;
        query_func = query_local;
        answer_func = answer_local;
        #endif
    }

    int query(std::vector<std::pair<int,int>> list)
    {
        return query_func(problem, list);
    }

    int answer(std::vector<std::pair<int,int>> list)
    {
        return answer_func(problem, list);
    }

    static int query_local(Problem &problem, std::vector<std::pair<int,int>>& list)
    {
        std::cout << "q ";
        std::cout << list.size() << " ";
        for(auto i:list) std::cout << i.first << " " << i.second << " ";
        std::cout << std::endl;
        problem.cnt++;
        if(list.size() == 1) return problem.map[list[0].first][list[0].second];
        else
        {
            int sum = 0;
            for(auto i:list) sum += problem.map[i.first][i.second];
            int k = list.size();
            double ave = (k - sum) * problem.eps + sum * (1 - problem.eps);
            double sigma = sqrt(k * problem.eps * (1 - problem.eps));
            double x = ave + sigma * problem.rand_query[problem.cnt - 1];
            int x_int = std::max(0, (int)round(x));
            return x_int;
        }
        return 0;
    }

    static int answer_local(Problem &problem, std::vector<std::pair<int,int>>& list)
    {
        std::cout << "a ";
        std::cout << list.size() << " ";
        for(auto i:list) std::cout << i.first << " " << i.second << " ";
        std::cout << std::endl;
        problem.cnt++;
        std::sort(list.begin(), list.end());
        if(problem.answer == list) return 1;
        return 0;
    }

    static int query_judge(Problem &problem, std::vector<std::pair<int,int>>& list)
    {
        std::cout << "q ";
        std::cout << list.size() << " ";
        for(auto i:list) std::cout << i.first << " " << i.second << " ";
        std::cout << std::endl;
        int a;
        std::cin >> a;
        return a;
    }

    static int answer_judge(Problem &problem, std::vector<std::pair<int,int>>& list)
    {
        std::cout << "a ";
        std::cout << list.size() << " ";
        for(auto i:list) std::cout << i.first << " " << i.second << " ";
        std::cout << std::endl;
        int a;
        std::cin >> a;
        return a;
    }
};

struct Config
{
    int N, M;
    double eps;
    std::vector<std::vector<int>> poly_list;
    std::vector<std::vector<int>> poly_flag_list;
    std::vector<std::bitset<400>> poly_bitset_list;
    std::vector<std::vector<int>> feasible_point_list;
    std::vector<int> size_y, size_x;
    std::vector<std::vector<int>> feasible_point_flag_list;
    std::vector<int> map;
    Config(Problem &problem):N(problem.N), M(problem.M), eps(problem.eps)
    {
        poly_list.resize(problem.M);
        for(int i = 0; i < problem.M; i++)
        {
            for(int j = 0; j < problem.poly[i].size(); j++)
            {
                poly_list[i].push_back(problem.poly[i][j].first * problem.N + problem.poly[i][j].second);
            }
        }

        poly_flag_list.resize(problem.M);
        poly_bitset_list.resize(problem.M);
        for(int i = 0; i < problem.M; i++)
        {
            poly_flag_list[i].resize(problem.N * problem.N);
            for(auto j:poly_list[i]) poly_flag_list[i][j] = 1, poly_bitset_list[i].set(j, 1);
        }
        
        feasible_point_list.resize(problem.M);
        for(int i = 0; i < problem.M; i++)
        {
            int maxy = 0, maxx = 0;
            for(auto j:poly_list[i])
            {
                int y = j / problem.N, x = j % problem.N;
                maxy = std::max(maxy, y);
                maxx = std::max(maxx, x);
            }
            size_y.push_back(maxy);
            size_x.push_back(maxx);
            for(int y = 0; y < problem.N - maxy; y++)for(int x = 0; x < problem.N - maxx; x++)
            {
                feasible_point_list[i].push_back(y * problem.N + x);
            }
        }
        feasible_point_flag_list.resize(problem.M);
        for(int i = 0; i < problem.M; i++)
        {
            feasible_point_flag_list[i].resize(problem.N * problem.N);
            for(auto j:feasible_point_list[i]) feasible_point_flag_list[i][j] = 1;
        }

        map.resize(problem.N * problem.N, -1);
    }

    void update(int v, int num)
    {
        map[v] = num;

        if(num == 0)
        {
            for(int i = 0; i < feasible_point_list.size(); i++)
            {
                for(int j = 0; j < feasible_point_list[i].size(); j++)
                {
                    int p = feasible_point_list[i][j];
                    int py = p / N, px = p % N;
                    int vy = v / N, vx = v % N;
                    vy -= py, vx -= px;
                    if(vy < 0 || vx < 0) continue;
                    if(poly_flag_list[i][vy * N + vx])
                    {
                        feasible_point_list[i].erase(feasible_point_list[i].begin() + j);
                        j--;
                    }
                }
            }
        }
        for(int i = 0; i < 10; i++)
        {
            std::vector<std::vector<int>> use(M, std::vector<int>(N*N, 0));
            for(int i = 0; i < M; i++)
            {
                for(auto j:feasible_point_list[i])
                {
                    for(auto k:poly_list[i]) use[i][j + k] = 1;
                }
            }
            for(int i = 0; i < N*N; i++)
            {
                if(map[i] <= 0) continue;
                int cnt = 0;
                for(int j = 0; j < M; j++) if(use[j][i]) cnt++;
                if(cnt == map[i])
                {
                    for(int j = 0; j < M; j++)
                    {
                        if(!use[j][i]) continue;
                        for(int k = 0; k < feasible_point_list[j].size(); k++)
                        {
                            int p = feasible_point_list[j][k];
                            int py = p / N, px = p % N;
                            int vy = i / N, vx = i % N;
                            vy -= py, vx -= px;
                            if(vy < 0 || vx < 0 || !poly_flag_list[j][vy * N + vx])
                            {
                                feasible_point_list[j].erase(feasible_point_list[j].begin() + k);
                                k--;
                            }
                        }
                    }
                }
            }
        }
        feasible_point_flag_list.clear();
        feasible_point_flag_list.resize(M);
        for(int i = 0; i < M; i++)
        {
            feasible_point_flag_list[i].resize(N * N);
            for(auto j:feasible_point_list[i]) feasible_point_flag_list[i][j] = 1;
        }

    }

    double calc_score(int v)
    {
        double nowsize = 1;
        for(int i = 0; i < feasible_point_list.size(); i++) nowsize *= feasible_point_list[i].size();
        double prob = 1;
        double size = 1;
        for(int i = 0; i < feasible_point_list.size(); i++)
        {
            int cnt = 0;
            for(int j = 0; j < feasible_point_list[i].size(); j++)
            {
                int p = feasible_point_list[i][j];
                int py = p / N, px = p % N;
                int vy = v / N, vx = v % N;
                vy -= py, vx -= px;
                if(vy < 0 || vx < 0) continue;
                if(poly_flag_list[i][vy * N + vx]) cnt++;
            }
            prob *= (feasible_point_list[i].size() - cnt);
            prob /= feasible_point_list[i].size();
            size *= (feasible_point_list[i].size() - cnt);
        }
        double score = size * prob + nowsize * (1 - prob);
        return score;
    }
};

struct State
{
    std::vector<int> point_list;
    long double prob;
    bool operator< (const State& state) const
    {
        return prob >= state.prob;
    }
};

struct PriorityQueue
{
    std::priority_queue<State> que;
    int capacity;

    PriorityQueue(int capacity):capacity(capacity)
    {

    }

    bool empty()
    {
        return que.empty();
    }
    
    void push(State &state)
    {
        que.push(state);
        if(que.size() > capacity) que.pop();
    }

    void pop()
    {
        que.pop();
    }

    const State& top()
    {
        return que.top();
    }

    size_t size()
    {
        return que.size();
    }
};

struct Sampler
{
    Config config;
    long double start_prob;
    std::vector<std::vector<std::vector<int>>> point_to_sum_list;
    std::vector<std::vector<long double>> sum_to_prob_list;
    std::vector<long double> sum_list;
    std::vector<int> sample_sum;
    PriorityQueue state_que;
    std::vector<State> state_list;
    std::set<std::vector<int>> state_set;
    int size;
    State sample;
    double ans_prob;
    std::vector<int> ans;
    std::vector<int> dx, dy;
    
    Sampler(Problem &problem, int size):config(problem), size(size), state_que(size)
    {
        for(int i = 0; i < size; i++)
        {
            State state;
            for(int j = 0; j < config.M; j++) state.point_list.push_back(config.feasible_point_list[j][randint(config.feasible_point_list[j].size())]);
            state.prob = 1;
            if(state_set.count(state.point_list)) continue;
            state_list.push_back(state);
            state_set.insert(state.point_list);
        }
        state_set.clear();
        for(auto &state:state_list) state.prob /= state_list.size();
        sample = state_list[0];
        start_prob = state_list[0].prob;
        sample_sum.resize(problem.N * problem.N * 2);
        dx = {0,0,1,-1}, dy = {1,-1,0,0};
    }

    void update_best()
    {
        std::map<std::vector<int>,long double> X;
        long double sum = 0;
        for(const auto &state:state_list)
        {
            std::vector<int> use(config.N * config.N, 0);
            for(int i = 0; i < config.M; i++)
            {
                int p = state.point_list[i];
                for(auto v:config.poly_list[i]) use[p + v] = 1;
            }
            X[use] += state.prob;
            sum += state.prob;
        }
        long double best = 0;
        std::vector<int> best_use;
        for(auto i:X) if(i.second > best) best_use = i.first, best = i.second;
        ans.clear();
        for(int i = 0; i < best_use.size(); i++) if(best_use[i]) ans.push_back(i);
        ans_prob = best / sum;
    }

    // 標準正規分布の累積分布関数
    long double normalCDF(long double value)
    {
        return 0.5 * (1 + std::erf(value / std::sqrt(2)));
    }

    // 任意の正規分布における区間[a, b]の確率を計算
    long double probabilityInRange(long double mean, long double stddev, long double a, long double b)
    {
        long double zA = (a - mean) / stddev;
        long double zB = (b - mean) / stddev;
        return normalCDF(zB) - normalCDF(zA);
    }

    long double calc_score(const State &state)
    {
        long double score = start_prob;
        for(int i = 0; i < sum_list.size(); i++)
        {
            int sum = 0;
            for(int j = 0; j < config.M; j++) sum += point_to_sum_list[i][j][state.point_list[j]];
            long double prob = sum_to_prob_list[i][sum];
            score *= prob;
            sample_sum[i] = sum;
        }
        return score;
    }

    long double calc_score_update(const State &state, const std::vector<int> &point_list_idx, const std::vector<int> &point_list)
    {
        long double score = start_prob;
        for(int i = 0; i < sum_list.size(); i++)
        {
            int sum = sample_sum[i];
            for(int k = 0; k < point_list_idx.size(); k++)
            {
                int j = point_list_idx[k];
                sum += point_to_sum_list[i][j][point_list[k]] - point_to_sum_list[i][j][state.point_list[j]];
            }
            long double prob = sum_to_prob_list[i][sum];
            score *= prob;
            sample_sum[i] = sum;
        }
        return score;
    }

    long double calc_score(const State &state, const std::vector<int> &point_list_idx, const std::vector<int> &point_list)
    {
        long double score = start_prob;
        for(int i = 0; i < sum_list.size(); i++)
        {
            int sum = sample_sum[i];
            for(int k = 0; k < point_list_idx.size(); k++)
            {
                int j = point_list_idx[k];
                sum += point_to_sum_list[i][j][point_list[k]] - point_to_sum_list[i][j][state.point_list[j]];
            }
            long double prob = sum_to_prob_list[i][sum];
            score *= prob;
        }
        return score;
    }
    std::vector<int> cnt_list=std::vector<int>(10, 0);
    long double acrate = 0;
    long double temp = 1;

    void sampling()
    {
        sample.prob = calc_score(sample);
        int cnt = 0;
        int loop = 100000;
        for(int i = 0; i < loop; i++)
        {
            if(rand01() < 0.1)
            {
                int idx = randint(config.M);
                int p = config.feasible_point_list[idx][randint(config.feasible_point_list[idx].size())];
                int memo = sample.point_list[idx];
                std::vector<int> point_list_idx = {idx};
                std::vector<int> point_list = {p};
                long double score = calc_score(sample, point_list_idx, point_list);
                long double prob = score / sample.prob * temp;
                if(rand01() < prob)
                {
                    cnt++;
                    sample.prob = calc_score_update(sample, point_list_idx, point_list);
                    sample.point_list[idx] = p;
                    if(state_set.count(sample.point_list) || score == 0) continue;
                    if(sample.prob < state_que.top().prob) continue;
                    auto x = state_que.top().point_list;
                    state_set.erase(x);
                    state_que.push(sample);
                    state_set.insert(sample.point_list);
                }
            }
            else if(rand01() < 0.4)
            {
                int idx0 = randint(config.M);
                int idx1 = randint(config.M);
                while(idx0 == idx1) idx1 = randint(config.M);
                int mode = randint(5);
                int p0 = sample.point_list[idx1], p1 = sample.point_list[idx0];
                if(mode == 1 || mode == 3)
                {
                    int delta = config.size_y[idx1] - config.size_y[idx0];
                    int p0y = p0 / config.N + delta;
                    int p1y = p1 / config.N - delta;
                    if(p0y < 0 || p0y >= config.N || p1y < 0 || p1y >= config.N) continue;
                    p0 += (config.size_y[idx1] - config.size_y[idx0]) * config.N;
                    p1 += (config.size_y[idx0] - config.size_y[idx1]) * config.N;
                }
                if(mode == 2 || mode == 3)
                {
                    int delta = config.size_x[idx1] - config.size_x[idx0];
                    int p0x = p0 % config.N + delta;
                    int p1x = p1 % config.N - delta;
                    if(p0x < 0 || p0x >= config.N || p1x < 0 || p1x >= config.N) continue;
                    p0 += config.size_x[idx1] - config.size_x[idx0];
                    p1 += config.size_x[idx0] - config.size_x[idx1];
                }
                if(mode == 4)
                {
                    int delta = (config.size_y[idx1] - config.size_y[idx0]) / 2;
                    int p0y = p0 / config.N + delta;
                    int p1y = p1 / config.N - delta;
                    if(p0y < 0 || p0y >= config.N || p1y < 0 || p1y >= config.N) continue;
                    p0 += delta * config.N;
                    p1 -= delta * config.N;
                    delta = (config.size_x[idx1] - config.size_x[idx0]) / 2;
                    int p0x = p0 % config.N + delta;
                    int p1x = p1 % config.N - delta;
                    if(p0x < 0 || p0x >= config.N || p1x < 0 || p1x >= config.N) continue;
                    p0 += delta;
                    p1 -= delta;
                }
                if(!config.feasible_point_flag_list[idx0][p0] || !config.feasible_point_flag_list[idx1][p1]) continue;
                std::vector<int> point_list_idx = {idx0, idx1};
                std::vector<int> point_list = {p0, p1};
                long double score = calc_score(sample, point_list_idx, point_list);
                long double prob = score / sample.prob * temp;
                if(rand01() < prob)
                {
                    cnt++;
                    sample.prob = calc_score_update(sample, point_list_idx, point_list);
                    sample.point_list[idx0] = p0, sample.point_list[idx1] = p1;
                    if(state_set.count(sample.point_list) || score == 0) continue;
                    if(sample.prob < state_que.top().prob) continue;
                    auto x = state_que.top().point_list;
                    state_set.erase(x);
                    state_que.push(sample);
                    state_set.insert(sample.point_list);
                }
            }
            else
            {
                int idx = randint(config.M);
                int p = sample.point_list[idx];
                int py = p / config.N, px = p % config.N;
                int d = randint(4);
                py += dy[d], px += dx[d];
                if(py < 0 || py >= config.N || px < 0 || px >= config.N) continue;
                p = py * config.N + px;
                if(!config.feasible_point_flag_list[idx][p]) continue;
                int memo = sample.point_list[idx];
                std::vector<int> point_list_idx = {idx};
                std::vector<int> point_list = {p};
                long double score = calc_score(sample, point_list_idx, point_list);
                long double prob = score / sample.prob * temp;
                if(rand01() < prob)
                {
                    cnt++;
                    sample.prob = calc_score_update(sample, point_list_idx, point_list);
                    sample.point_list[idx] = p;
                    if(state_set.count(sample.point_list) || score == 0) continue;
                    if(sample.prob < state_que.top().prob) continue;
                    auto x = state_que.top().point_list;
                    state_set.erase(x);
                    state_que.push(sample);
                    state_set.insert(sample.point_list);
                }
            }
        }
        acrate = (long double)cnt / loop;
        //std::cerr<<"ac rate:"<<acrate<<"\n";
        if(acrate < 0.1)
        {
            //std::cerr<<"temp:"<<temp<<"->"<<temp*2<<"\n";
            temp *= 2;
        }
    }

    void check()
    {
        long double best_prob = -1e9;
        for(auto &state:state_list)
        {
            state_que.push(state);
            state_set.insert(state.point_list);
            if(best_prob < state.prob) sample = state, best_prob = state.prob;
        }

        for(int i = 0; i < 10; i++) sampling();
        state_list.clear();
        state_set.clear();
        while(!state_que.empty())
        {
            state_list.push_back(state_que.top());
            state_que.pop();
        }
        update_best();
    }

    void update(const std::vector<int>& list, int num)
    {
        if(list.size() == 1) config.update(list[0], num);
        std::vector<std::vector<int>> point_to_sum;
        for(int i = 0; i < config.M; i++)
        {
            std::vector<int> point_to_sum_poly;
            for(int j = 0; j < config.N * config.N; j++)
            {
                int sum = 0;
                for(auto k:list)
                {
                    int vy = k / config.N, vx = k % config.N;
                    if(vy >= j / config.N && vx >= j % config.N)
                    {
                        sum += config.poly_flag_list[i][k - j];
                    }
                }
                point_to_sum_poly.push_back(sum);
            }
            point_to_sum.push_back(point_to_sum_poly);
        }
        point_to_sum_list.push_back(point_to_sum);

        std::vector<long double> sum_to_prob;
        for(int i = 0; i < config.M * list.size() + 1; i++)
        {
            int sum = i;
            if(list.size() == 1)
            {
                if(sum == num) sum_to_prob.push_back((long double)1 - 1e-5);
                else sum_to_prob.push_back(1e-5);
                continue;
            }
            int k = list.size();
            long double mean = (k - sum) * config.eps + sum * (1 - config.eps);
            long double stddev = sqrt(k * config.eps * (1 - config.eps));
            long double a = -100, b = num + 0.5;
            if(num) a = num - 0.5;
            long double prob = probabilityInRange(mean, stddev, a, b);
            sum_to_prob.push_back(prob);
        }
        sum_to_prob_list.push_back(sum_to_prob);

        long double prob_sum = 0;
        for(auto &state:state_list)
        {
            int sum = 0;
            for(int i = 0; i < config.M; i++) sum += point_to_sum[i][state.point_list[i]];
            long double prob = sum_to_prob[sum];
            state.prob *= prob;
            prob_sum += state.prob;
        }
        long double best_prob = 0;
        if(prob_sum == 0)
        {
            std::cerr<<"pien!\n";
            while(timer.ms() < 4000) ;
            assert(false);
        }
        for(auto &state:state_list)
        {
            state.prob /= prob_sum;
            state_que.push(state);
            state_set.insert(state.point_list);
            if(best_prob < state.prob) sample = state;
        }
        sum_list.push_back((long double)1 / prob_sum);
        start_prob *= sum_list.back();

        sampling();
        state_list.clear();
        state_set.clear();
        while(!state_que.empty())
        {
            state_list.push_back(state_que.top());
            state_que.pop();
        }
        update_best();
    }

    void update_ans()
    {
        long double prob_sum = 0;
        double best_score = -1e9;
        std::vector<int> use_ans(config.N * config.N, 0);
        for(auto i:ans) use_ans[i] = 1;
        for(auto &state:state_list)
        {
            std::vector<int> use(config.N * config.N, 0);
            for(int i = 0; i < config.M; i++)
            {
                int p = state.point_list[i];
                for(auto v:config.poly_list[i]) use[p + v] = 1;
            }
            if(use == use_ans) continue;
            prob_sum += state.prob;
        }
        for(auto &state:state_list)
        {
            std::vector<int> use(config.N * config.N, 0);
            for(int i = 0; i < config.M; i++)
            {
                int p = state.point_list[i];
                for(auto v:config.poly_list[i]) use[p + v] = 1;
            }
            if(use == use_ans) continue;
            state.prob /= prob_sum;
            state_que.push(state);
            state_set.insert(state.point_list);
            if(state.prob > best_score) best_score = state.prob, sample = state;
        }
        state_list.clear();
        state_set.clear();
        while(!state_que.empty())
        {
            state_list.push_back(state_que.top());
            state_que.pop();
        }
        update_best();
    }
};

struct Evaluator
{
    Config config;
    std::map<std::vector<int>,int> map_to_group;
    std::vector<int> idx_to_group;
    std::vector<long double> variance_list, exp_list, exp_square_list;
    std::vector<int> sum_memo;
    int sum_memo_idx;
    Evaluator(Problem &problem):config(problem)
    {
        variance_list.resize(config.N * config.N, 0);
        exp_list.resize(config.N * config.N, 0);
        exp_square_list.resize(config.N * config.N, 0);
    }

    // 標準正規分布の累積分布関数
    long double normalCDF(long double value)
    {
        return 0.5 * (1 + std::erf(value / std::sqrt(2)));
    }

    // 任意の正規分布における区間[a, b]の確率を計算
    long double probabilityInRange(long double mean, long double stddev, long double a, long double b)
    {
        long double zA = (a - mean) / stddev;
        long double zB = (b - mean) / stddev;
        return normalCDF(zB) - normalCDF(zA);
    }

    std::vector<long double> calc_variance(const std::vector<State> &state_list)
    {
        map_to_group.clear(), idx_to_group.clear();

        std::vector<long double> state_prob_list(state_list.size());
        long double score_sum = 0;
        for(const auto &state:state_list) score_sum += state.prob;
        for(int i = 0; i < state_list.size(); i++) state_prob_list[i] = state_list[i].prob / score_sum;

        std::fill(variance_list.begin(), variance_list.end(), 0);
        std::fill(exp_list.begin(), exp_list.end(), 0);
        std::fill(exp_square_list.begin(), exp_square_list.end(), 0);

        std::vector<int> sum(config.N * config.N, 0);
        for(int state_i = 0; state_i < state_list.size(); state_i++)
        {
            const auto &state = state_list[state_i];
            std::fill(sum.begin(), sum.end(), 0);
            for(int i = 0; i < config.M; i++)
            {
                for(auto j:config.poly_list[i]) sum[j + state.point_list[i]]++;
            }
            int size = config.N * config.N;
            for(int i = 0; i < size; i++)
            {
                if(sum[i] == 0) continue;
                long double x = sum[i] * state_prob_list[state_i];
                exp_list[i] += x;
                exp_square_list[i] += x * sum[i];
                sum[i] = 1;
            }
            if(map_to_group.count(sum)) idx_to_group.push_back(map_to_group[sum]);
            else map_to_group[sum] = map_to_group.size(), idx_to_group.push_back(map_to_group[sum]);
        }

        for(int i = 0; i < variance_list.size(); i++) variance_list[i] = exp_square_list[i] - exp_list[i] * exp_list[i];
        return variance_list;
    }

    void reset(const std::vector<State> &state_list)
    {
        sum_memo.clear();
        sum_memo.resize(state_list.size());
        sum_memo_idx = 0;
    }

    long double calc_score_point(int v, const std::vector<State> &state_list)
    {
        std::vector<std::vector<long double>> group_sum_list(config.M + 1, std::vector<long double>(idx_to_group.size(), 0));
        std::vector<long double> prob_num_list(config.M + 1, 0);
        long double all_sum = 0;
        for(int i = 0; i < state_list.size(); i++)
        {
            const auto &state = state_list[i];
            int sum = 0;
            int vy = v / config.N, vx = v % config.N;
            for(int i = 0; i < config.M; i++)
            {
                if(vy >= state.point_list[i] / config.N && vx >= state.point_list[i] % config.N)
                {
                    sum += config.poly_flag_list[i][v - state.point_list[i]];
                }
            }
            prob_num_list[sum] += state.prob;
            group_sum_list[sum][idx_to_group[i]] += state.prob;
            all_sum += state.prob;
            sum_memo[i] += sum;
        }
        sum_memo_idx = 1;

        long double score = 0;
        std::vector<long double> group_sum(idx_to_group.size(), 0);
        long double prob_sum = 0;
        for(int i = 0; i < state_list.size(); i++)
        {
            prob_sum += state_list[i].prob;
            group_sum[idx_to_group[i]] += state_list[i].prob;
        }
        for(auto i:group_sum)
        {
            long double prob = i / prob_sum;
            if(prob == 0) continue;
            score -= prob * log2(prob);
        }

        for(int num = 0; num <= config.M; num++)
        {
            long double prob = prob_num_list[num] / all_sum;
            if(prob == 0) continue;
            for(auto i:group_sum_list[num])
            {
                long double group_prob = i / prob_num_list[num];
                if(group_prob == 0) continue;
                score += prob * group_prob * log2(group_prob);
            }
        }

        return score;
    }

    long double calc_score(const std::vector<int> &list_query, const std::vector<State> &state_list)
    {
        if(list_query.size() == 1) return calc_score_point(list_query[0], state_list);
        long double min_num = 0, max_num = 0;
        for(auto i:list_query)
        {
            min_num += std::max((long double)0, exp_list[i] - sqrt(std::max((long double)0, variance_list[i])) * 3);
            max_num += std::min((long double)config.M * list_query.size(), exp_list[i] + sqrt(std::max((long double)0, variance_list[i])) * 3);
        }
        int min_num_int = std::round(min_num), max_num_int = std::round(max_num);

        std::vector<std::vector<long double>> group_sum_list(max_num_int - min_num_int + 1, std::vector<long double>(idx_to_group.size(), 0));
        std::vector<long double> prob_num_list(max_num_int - min_num_int + 1, 0);
        long double all_sum = 0;
        for(int i = 0; i < state_list.size(); i++)
        {
            const auto &state = state_list[i];
            for(int j = sum_memo_idx; j < list_query.size(); j++)
            {
                int v = list_query[j];
                int vy = v / config.N, vx = v % config.N;
                for(int j = 0; j < config.M; j++)
                {
                    if(vy >= state.point_list[j] / config.N && vx >= state.point_list[j] % config.N)
                    {
                        sum_memo[i] += config.poly_flag_list[j][v - state.point_list[j]];
                    }
                }
            }
            int sum = sum_memo[i];
            for(int num = min_num; num <= max_num; num++)
            {
                int k = list_query.size();
                long double mean = (k - sum) * config.eps + sum * (1 - config.eps);
                long double stddev = sqrt(k * config.eps * (1 - config.eps));
                long double a = -100, b = num + 0.5;
                if(num) a = num - 0.5;
                long double score = state.prob * probabilityInRange(mean, stddev, a, b);
                prob_num_list[num - min_num] += score;
                group_sum_list[num - min_num][idx_to_group[i]] += score;
                all_sum += score;
            }
        }
        sum_memo_idx = list_query.size();

        long double score = 0;
        std::vector<long double> group_sum(idx_to_group.size(), 0);
        long double prob_sum = 0;
        for(int i = 0; i < state_list.size(); i++)
        {
            prob_sum += state_list[i].prob;
            group_sum[idx_to_group[i]] += state_list[i].prob;
        }
        for(auto i:group_sum)
        {
            long double prob = i / prob_sum;
            if(prob == 0) continue;
            score -= prob * log2(prob);
        }

        for(int num = min_num; num <= max_num; num++)
        {
            long double prob = prob_num_list[num - min_num] / all_sum;
            if(prob == 0) continue;
            for(auto i:group_sum_list[num - min_num])
            {
                long double group_prob = i / prob_num_list[num - min_num];
                if(group_prob == 0) continue;
                score += prob * group_prob * log2(group_prob);
            }
        }
        score *= sqrt(list_query.size());

        return score;
    }
};


struct Solver
{
    void Solve()
    {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
        for(int i = 0; i < millis; i++) xorshift();

        Problem problem;
        problem.Read(std::cin);
        Interface interface(problem);
        Sampler sampler(problem, 50);
        Evaluator eva(problem);
        int cnt = 0;
        
        for(int i = 0; i < problem.N * problem.N * 2 && cnt < problem.N * problem.N * 2; i++)
        {
            if(sampler.ans_prob > 0.9)
            {
                if(timer.ms() > 3000) std::cerr<<"time over          "<<problem.N<<" "<<problem.M<<" "<<problem.eps<<"\n";
                if(timer.ms() < 2500) sampler.check();
                if(sampler.ans_prob > 0.9)
                {
                while(cnt < problem.N * problem.N * 2)
                {
                    std::vector<std::pair<int,int>> ans;
                    for(auto i:sampler.ans) ans.push_back({i / problem.N, i % problem.N});
                    if(ans.size() == 0)
                    {
                        std::cerr<<"wa              "<<problem.N<<" "<<problem.M<<" "<<problem.eps<<"\n";
                        while(timer.ms() < 4000);
                        return;
                    }
                    bool flag = interface.answer(ans);
                    cnt++;
                    if(flag)
                    {
                        std::cerr<<"ac "<<"time:"<<timer.ms()<<" "<<problem.N<<" "<<problem.M<<" "<<problem.eps<<"\n";
                        return;
                    }
                    sampler.update_ans();
                    //std::cerr<<sampler.ans_prob<<" ";
                }
                if(cnt == problem.N * problem.N * 2) while(timer.ms() < 4000);
                return;
                }
            }
            /*{
                std::vector<std::pair<int,int>> ans;
                std::vector<int> use(problem.N * problem.N, 0);
                for(auto i:sampler.ans) use[i] = 1;
                for(int i = 0; i < problem.N; i++)for(int j = 0; j < problem.N; j++)
                {
                    if(use[i*problem.N + j]) std::cout << "#c "<<i<<" "<<j<<" yellow\n";
                    else std::cout << "#c "<<i<<" "<<j<<" purple\n";
                }
            }*/
            auto var = eva.calc_variance(sampler.state_list);
            long double best_score = -1e9;
            std::vector<int> list;

            int idx = 0;
            std::vector<std::pair<long double,int>> idxList;
            const State &best_state = sampler.state_list.back();
            for(int i = 0; i < problem.M; i++)
            {
                std::vector<int> use(problem.N * problem.N, 0);
                std::vector<int> list_tmp;
                for(auto j:sampler.config.poly_list[i]) use[j + best_state.point_list[i]] = 1;
                for(int j = 0; j < use.size(); j++) if(use[j]) list_tmp.push_back(j);
                eva.reset(sampler.state_list);
                long double score = eva.calc_score(list_tmp, sampler.state_list);
                if(score > best_score) best_score = score, list = list_tmp, idx = i;
                idxList.push_back({-score, i});
            }

            std::vector<int> best_sum(problem.N*problem.N, 0);
            for(int i = 0; i < problem.M; i++)
            {
                for(auto j:sampler.config.poly_list[i]) best_sum[j + best_state.point_list[i]]++;
            }
            for(int i = 0; i < problem.M; i++)
            {
                std::vector<int> use(problem.N * problem.N, 0);
                std::vector<int> list_tmp;
                for(auto j:sampler.config.poly_list[i]) use[j + best_state.point_list[i]] = 1;
                for(int j = 0; j < use.size(); j++) if(use[j] && best_sum[j] == 1) list_tmp.push_back(j);
                if(list_tmp.size() < 2) continue;
                eva.reset(sampler.state_list);
                long double score = eva.calc_score(list_tmp, sampler.state_list);
                if(score > best_score) best_score = score, list = list_tmp;
            }

            std::vector<int> second_best_sum(problem.N * problem.N, 0);
            State second_best;
            for(int i = 0; i < (int)sampler.state_list.size() - 1; i++)
            {
                const State &second_best_state = sampler.state_list[sampler.state_list.size() - 2 - i];
                std::fill(second_best_sum.begin(), second_best_sum.end(), 0);
                for(int i = 0; i < problem.M; i++)
                {
                    for(auto j:sampler.config.poly_list[i]) second_best_sum[j + second_best_state.point_list[i]]++;
                }
                if(best_sum != second_best_sum)
                {
                    second_best = second_best_state;
                    break;
                }
            }
            for(int i = 0; i < problem.M; i++)
            {
                std::vector<int> use(problem.N * problem.N, 0);
                std::vector<int> list_tmp;
                for(auto j:sampler.config.poly_list[i]) use[j + second_best.point_list[i]] = 1;
                for(int j = 0; j < use.size(); j++) if(use[j]) list_tmp.push_back(j);
                eva.reset(sampler.state_list);
                long double score = eva.calc_score(list_tmp, sampler.state_list);
                if(score > best_score) best_score = score, list = list_tmp, idx = i;
                idxList.push_back({-score, i});
            }
            for(int i = 0; i < 1; i++)
            {
                std::vector<int> list_tmp;
                for(int i = 0; i < best_sum.size(); i++) if(best_sum[i] < second_best_sum[i]) list_tmp.push_back(i);
                if(list_tmp.size() < 1) continue;
                eva.reset(sampler.state_list);
                long double score = eva.calc_score(list_tmp, sampler.state_list);
                if(score > best_score) best_score = score, list = list_tmp;
            }
            for(int i = 0; i < 1; i++)
            {
                std::vector<int> list_tmp;
                for(int i = 0; i < best_sum.size(); i++) if(best_sum[i] > second_best_sum[i]) list_tmp.push_back(i);
                if(list_tmp.size() < 1) continue;
                eva.reset(sampler.state_list);
                long double score = eva.calc_score(list_tmp, sampler.state_list);
                if(score > best_score) best_score = score, list = list_tmp;
            }

            std::sort(idxList.begin(), idxList.end());
            eva.reset(sampler.state_list);
            std::vector<int> use(problem.N * problem.N, 0);
            std::vector<int> list_use;
            for(int l = 2; l <= problem.M; l++)
            {
                State state = sampler.state_list.back();
                for(int k = 0; k < l; k++)
                {
                    int i = idxList[k].second;
                    for(auto j:sampler.config.poly_list[i])
                    {
                        if(use[j + state.point_list[i]] == 0) list_use.push_back(j + state.point_list[i]);
                        use[j + state.point_list[i]] = 1;
                    }
                }
                long double score = eva.calc_score(list_use, sampler.state_list);
                if(score > best_score) best_score = score, list = list_use;
                if(timer.ms() > 2000) break;
            }

            if(timer.ms() < 2000)
            {
            eva.reset(sampler.state_list);
            for(int i = 0; i < problem.M; i++)
            {
                if(idx != i) continue;
                State state = sampler.state_list.back();
                std::vector<int> use(problem.N * problem.N, 0);
                std::vector<int> list_tmp;
                for(auto j:sampler.config.poly_list[i]) use[j + state.point_list[i]] = 1;
                for(int j = 0; j < use.size(); j++) if(!use[j]) list_tmp.push_back(j);
                long double score = eva.calc_score(list_tmp, sampler.state_list);
                if(score > best_score) best_score = score, list = list_tmp;
            }
            if(list.size() == 0)
            {
                std::cerr<<problem.N<<" "<<problem.M<<" "<<problem.eps<<"\n";
                while(timer.ms() < 4000) ;
            }
            }
            assert(list.size() != 0);
            std::vector<std::pair<int,int>> list_query;
            for(auto i:list) list_query.push_back({i / problem.N, i % problem.N});
            int num = interface.query(list_query);
            cnt++;
            sampler.update(list, num);
            //std::cerr<<sampler.state_list.size()<<" ";
            /*for(auto i:sampler.state_list) std::cerr<<i.prob<<":";std::cerr<<"\n";
            State state;
            state.point_list = problem.real_point;
            std::cerr<<sampler.calc_score(state)<<"\n";*/
            //std::cerr<<sampler.ans_prob<<":";
            //std::cerr<<sampler.cnt_list[0]<<":"<<sampler.cnt_list[2]<<" ";
        }
    }
};


int main()
{
    std::ios_base::sync_with_stdio (false);
    std::cin.tie(0);
    std::cout.tie(0);
    timer.start();
    Solver solver;
    solver.Solve();
    std::cerr<<timer.ms()<<"[ms]\n";
}