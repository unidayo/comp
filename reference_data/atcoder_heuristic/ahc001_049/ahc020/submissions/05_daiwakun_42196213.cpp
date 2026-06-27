#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <string>
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
    int N, M, K;
    std::vector<int> x, y, u, v, w, a, b;
    void Read()
    {
        std::cin >> N >> M >> K;
        x.resize(N);
        y.resize(N);
        u.resize(M);
        v.resize(M);
        w.resize(M);
        a.resize(K);
        b.resize(K);
        for(int i = 0; i < N; i++) std::cin >> x[i] >> y[i];
        for(int i = 0; i < M; i++) std::cin >> u[i] >> v[i] >> w[i], u[i]--, v[i]--;
        for(int i = 0; i < K; i++) std::cin >> a[i] >> b[i];
    }
};

struct Evalutor
{
    double score;
    std::vector<int> on, off;
    std::vector<std::vector<int>> d;
    int N;
    std::vector<int> a, b, x, y;
    std::vector<int> X;

    void Init(Problem& problem)
    {
        for(int i = 0; i < problem.K; i++) X.push_back(i);
        std::mt19937 get_rand_mt(0);
        std::shuffle( X.begin(), X.end(), get_rand_mt );
        for(int i = 0; i < problem.N; i++) on.push_back(i);
        a = problem.a;
        b = problem.b;
        x = problem.x;
        y = problem.y;
        N = problem.N;
        d.resize(problem.N, std::vector<int>(problem.N, 1e9));
        for(int i = 0; i < problem.N; i++) d[i][i] = 0;
        for(int i = 0; i < problem.M; i++) d[problem.u[i]][problem.v[i]] = problem.w[i], d[problem.v[i]][problem.u[i]] = problem.w[i];
        for (int i = 0; i < problem.N; i++)for (int j = 0; j < problem.N; j++)for (int k = 0; k < problem.N; k++)
        {
            d[j][k] = std::min(d[j][k], d[j][i] + d[i][k]);
        }
        CalcScore();
    }

    void CalcScore()
    {
        double s = 0;
        std::vector<int> list = on;
        std::vector<int> tmp;
        tmp.push_back(list[0]);
        list.erase(list.begin());
        while(list.size() > 0)
        {
            int mind = 1e9;
            int idx = -1;
            for(int idx_ = 0; idx_ < list.size(); idx_++)for(auto j:tmp)
            {
                int i = list[idx_];
                if(mind > d[i][j]) mind = d[i][j], idx = idx_;
            }
            tmp.push_back(list[idx]);
            list.erase(list.begin() + idx);
            s += mind;
        }
        std::vector<int> P(N, 0);
        int pena = 0;
        for(int i_ = 0; i_ < a.size(); i_++)
        {
            int i = X[i_];
            bool flag = false;
            for(auto j:on)
            {
                int xlen = x[j] - a[i];
                int ylen = y[j] - b[i];
                if(xlen * xlen + ylen * ylen <= P[j] * P[j])
                {
                    flag = true;
                    break;
                }
            }
            if(flag) continue;
            int minscore = 1e9, idx = -1;
            for(auto j:on)
            {
                int xlen = x[j] - a[i];
                int ylen = y[j] - b[i];
                if(std::ceil(sqrt(xlen * xlen + ylen * ylen)) > 5000) continue;
                int score = std::ceil(sqrt(xlen * xlen + ylen * ylen)) - P[j];
                score = score * score;
                if(score < minscore) minscore = score, idx = j;
            }
            if(idx == -1)
            {
                pena++;
                continue;
            }
            assert(idx != -1);
            int xlen = x[idx] - a[i];
            int ylen = y[idx] - b[i];
            P[idx] = std::ceil(sqrt(xlen * xlen + ylen * ylen));
        }
        for(int i = 0; i < N; i++) s += P[i] * P[i];
        score = 1e6 + std::round(1e6 * (1 + 1e8) / (s + 1e7)) - pena * 1e6;
    }

    void Write(Problem& problem)
    {
        double s = 0;
        std::vector<int> P(N, 0);
        for(int i_ = 0; i_ < a.size(); i_++)
        {
            int i = X[i_];
            bool flag = false;
            for(auto j:on)
            {
                int xlen = x[j] - a[i];
                int ylen = y[j] - b[i];
                if(xlen * xlen + ylen * ylen <= P[j] * P[j]) flag = true;
            }
            if(flag) continue;
            int minscore = 1e9, idx = -1;
            for(auto j:on)
            {
                int xlen = x[j] - a[i];
                int ylen = y[j] - b[i];
                if(std::ceil(sqrt(xlen * xlen + ylen * ylen)) > 5000) continue;
                int score = std::ceil(sqrt(xlen * xlen + ylen * ylen)) - P[j];
                score = score * score;
                if(score < minscore) minscore = score, idx = j;
            }
            if(idx == -1) continue;
            int xlen = x[idx] - a[i];
            int ylen = y[idx] - b[i];
            P[idx] = std::ceil(sqrt(xlen * xlen + ylen * ylen));
        }
        for(int i = 0; i < N; i++) s += P[i] * P[i];
        for(int i = 0; i < N; i++) std::cout<<std::min(5000, P[i])<<" ";
        std::cout<<"\n";
        std::vector<std::vector<int>> flag(problem.N, std::vector<int>(problem.N, 0)), use;
        use = flag;
        for(int i = 0; i < problem.M; i++) flag[problem.u[i]][problem.v[i]] = 1, flag[problem.v[i]][problem.u[i]] = 1;
        std::vector<int> B(problem.M, 0);
        std::vector<int> list = on;
        std::vector<int> tmp;
        tmp.push_back(list[0]);
        list.erase(list.begin());
        while(list.size() > 0)
        {
            int mind = 1e9;
            int idx = -1;
            int idxmemo = -1;
            int to = -1;
            for(int idx_ = 0; idx_ < list.size(); idx_++)for(auto j:tmp)
            {
                int i = list[idx_];
                if(mind > d[i][j]) mind = d[i][j], idx = i, to = j, idxmemo = idx_;
            }
            tmp.push_back(list[idxmemo]);
            list.erase(list.begin() + idxmemo);
            while(!flag[idx][to])
            {
                for(int i = 0; i < N; i++)
                {
                    if(i == idx || i == to) continue;
                    if(d[idx][to] == d[idx][i] + d[i][to] && flag[idx][i])
                    {
                        use[idx][i] = 1;
                        idx = i;
                        break;
                    }
                }
            }
            use[idx][to] = 1;
            s += mind;
        }
        for(int i = 0; i < problem.M; i++) if(use[problem.u[i]][problem.v[i]] || use[problem.v[i]][problem.u[i]]) B[i] = 1;
        for(int i = 0; i < problem.M; i++) std::cout << B[i]<<" ";
        std::cout << "\n";
    }
};

struct Backup
{
    std::vector<int> on, off, X;
    double score;
    
    void Regist(Evalutor &eva_)
    {
        on = eva_.on;
        off = eva_.off;
        X = eva_.X;
        score = eva_.score;
    }

    void Get(Evalutor &eva_)
    {
        eva_.on = on;
        eva_.off = off;
        eva_.X = X;
        eva_.score = score;
    }
};

struct State
{
    Evalutor eva, bestEva;
    Backup backup;

    State(Problem &problem)
    {
        eva.Init(problem);
        bestEva = eva;
    }

    void Neighbor()
    {
        backup.Regist(eva);
        double prob = rand01();
        int idx0 = xorshift() % eva.X.size();
        int idx1 = xorshift() % eva.X.size();
        while(idx0 == idx1) idx1 = xorshift() % eva.X.size();
        std::swap(eva.X[idx0], eva.X[idx1]);
        if(rand01() < 0.005)
        {
            std::mt19937 get_rand_mt(xorshift());
            std::shuffle( eva.X.begin(), eva.X.end(), get_rand_mt );
        }
        if(rand01() < timer.ms() / 2000 * 0.5) ;
        else if(prob < 0.33)
        {
            if(eva.on.size() == 0) return;
            int idx = xorshift() % eva.on.size();
            while(idx == 0) idx = xorshift() % eva.on.size();
            eva.off.push_back(eva.on[idx]);
            eva.on.erase(eva.on.begin() + idx);
        }
        else if(prob < 0.66)
        {
            if(eva.off.size() == 0) return;
            int idx = xorshift() % eva.off.size();
            eva.on.push_back(eva.off[idx]);
            eva.off.erase(eva.off.begin() + idx);
        }
        else
        {
            if(eva.on.size() == 0) return;
            if(eva.off.size() == 0) return;
            int idx0 = xorshift() % eva.on.size();
            while(idx0 == 0) idx0 = xorshift() % eva.on.size();
            int idx1 = xorshift() % eva.off.size();
            std::swap(eva.on[idx0], eva.off[idx1]);
        }
    }

    bool Accept(double temp)
    {
        for(auto i:eva.off) assert(0 <= i && i < 100);
        eva.CalcScore();
        for(auto i:eva.off) assert(0 <= i && i < 100);
        double deltaScore = eva.score - backup.score;
        double probability = exp(deltaScore / temp);
        return rand01() < probability;
    }

    void Update()
    {
        if(eva.score > bestEva.score)
        {
            bestEva = eva;
        }
    }

    void Rollback()
    {
        backup.Get(eva);
    }

    void Write(Problem &problem)
    {
        bestEva.Write(problem);
    }
};

struct Solver
{
    void SimulatedAnnealing(State &state)
    {
        double START_TEMP = 10000;
        double END_TEMP = 800;
        int END_TIME = 1980;
        int START_TIME = timer.ms();
        while(timer.ms() < END_TIME)
        {
            const double progressRatio = (double) (timer.ms() - START_TIME) / (END_TIME - START_TIME);
            const double temp = START_TEMP + (END_TEMP - START_TEMP) * progressRatio;
            //const double temp = exp(log(START_TEMP) + (log(END_TEMP) - log(START_TEMP)) * progressRatio);
            state.Neighbor();
            if(state.Accept(temp)) state.Update();
            else state.Rollback();
        }
    }

    void ParallelTempering(State &state)
    {
        int replicaSize = 7;
        double TEMP_MAX = 10000;
        double TEMP_MIN = 1e-0;
        int END_TIME = 1900;
        std::vector<State> states;
        std::vector<double> temp;
        for(int i = 0; i < replicaSize; i++)
        {
            states.push_back(state);
            temp.push_back(TEMP_MAX - (TEMP_MAX - TEMP_MIN) * i / (replicaSize - 1));
            //temp.push_back(exp(log(TEMP_MAX) - (log(TEMP_MAX) - log(TEMP_MIN)) * i / (replicaSize - 1)));
        }
        while(timer.ms() < END_TIME)
        {
            for(int i = 0; i < replicaSize; i++)
            {
                states[i].Neighbor();
                if(states[i].Accept(temp[i])) states[i].Update();
                else states[i].Rollback();
            }
            for(int i = 0; i < replicaSize - 1; i++)
			{
				double deltaS = (1.0 / temp[i + 1] - 1.0 / temp[i]) * (states[i].eva.score - states[i + 1].eva.score);
				const double probability = exp(deltaS);
				if(probability > rand01())
				{
					std::swap(states[i], states[i + 1]);
				}
			}
        }
        for(int i = 0; i < replicaSize; i++)
        {
            if(states[i].bestEva.score > state.bestEva.score)
            {
                state.bestEva = states[i].bestEva;
            }
        }
    }

    void Solve(Problem &problem)
    {
        State state(problem);
        SimulatedAnnealing(state);
        //ParallelTempering(state);
        state.Write(problem);
        std::cerr<<std::setprecision(10)<<state.bestEva.score<<"\n";
    }
};

void solve()
{
    Problem problem;
    problem.Read();
    Solver solver;
    solver.Solve(problem);
}

int main()
{
    std::ios_base::sync_with_stdio (false);
    std::cin.tie(0);
    std::cout.tie(0);
    timer.start();
    solve();
    //std::cerr<<"time:"<<timer.ms()<<"[ms]\n";
}