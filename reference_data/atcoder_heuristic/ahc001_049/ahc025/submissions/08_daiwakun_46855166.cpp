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
    int N, D, Q;
    std::vector<int> weightave;
    void Read()
    {
        std::cin >> N >> D >> Q;
        int loop = 2000;
        std::default_random_engine engine(xorshift());
        std::exponential_distribution<> dist(1e-5);
        weightave.resize(N);
        for(int i = 0; i < loop; i++)
        {
            std::vector<int> weight;
            for(int i = 0; i < N; i++)
            {
                double w = dist(engine);
                if(w * D > N * 1e5) i--;
                else weight.push_back((std::max(1, (int)round(w))));
            }
            std::sort(weight.begin(), weight.end());
            for(int i = 0; i < N; i++) weightave[i] += weight[i];
        }
        for(auto &i:weightave) i /= loop;
    }
};

struct Interface
{
    Problem problem;
    std::vector<int> weight;
    bool sim = true;

    void init(Problem problem_)
    {
        problem = problem_;
        reset();
    }

    void reset()
    {
        weight.clear();
        std::default_random_engine engine(xorshift());
        std::exponential_distribution<> dist(1e-5);
        for(int i = 0; i < problem.N; i++)
        {
            double w = dist(engine);
            if(w * problem.D > problem.N * 1e5) i--;
            else weight.push_back((std::max(1, (int)round(w))));
        }
    }

    char simask(std::vector<int> lList, std::vector<int> rList)
    {
        int suml = 0, sumr = 0;
        for(auto i:lList) suml += weight[i];
        for(auto i:rList) sumr += weight[i];
        if(suml < sumr) return '<';
        else if(suml == sumr) return '=';
        else return '>';
    }

    int score(std::vector<std::vector<int>> list)
    {
        std::vector<int> group(list.size(), 0);
        for(int i = 0; i < list.size(); i++)
        {
            for(auto j:list[i]) group[i] += weight[j];
        }
        double ave = 0;
        for(auto i:weight) ave += i;
        ave /= problem.D;
        double sum = 0;
        for(auto i:group) sum += (double)i * (double)i;
        sum /= problem.D;
        return std::sqrt(sum - ave*ave) * 100 + 1;
    }

    char ask(std::vector<int> lList, std::vector<int> rList)
    {
        if(sim) return simask(lList, rList);
        std::cout << lList.size() << " " << rList.size() <<" ";
        for(auto i:lList) std::cout << i << " ";
        for(auto i:rList) std::cout << i << " ";
        std::cout << std::endl;
        char x;
        std::cin >> x;
        return x;
    }

    void answer(std::vector<std::vector<int>> list)
    {
        std::vector<int> answer;
        int sum = 0;
        for(auto i:list) sum += i.size();
        answer.resize(sum);
        for(int i = 0; i < list.size(); i++)
        {
            for(auto j:list[i]) answer[j] = i;
        }
        for(auto i:answer) std::cout << i << " ";
        std::cout << std::endl;
    }

    void semianswer(std::vector<std::vector<int>> list)
    {
        if(sim) return;
        std::cout << "#c ";
        answer(list);
    }
};

struct State2
{
    Problem problem;
    Interface &interface;
    std::vector<std::vector<int>> list;
    bool sortFlag = false;
    std::vector<int> weightrank;
    int count = 0;
    int ngrank;
    int movengrank;

    std::map<std::vector<int>,int> toIdx;
    std::vector<std::vector<int>> graph;
    std::vector<std::pair<int,int>> ngswap;
    std::vector<std::pair<int,int>> goodswap;

    std::vector<std::vector<int>> realIdx;
    int size;

    State2(Problem problem, Interface &interface):problem(problem), interface(interface)
    {
        size = problem.N / 2;
        realIdx.resize(size);
        for(int i = 0; i < problem.N; i++) realIdx[i%size].push_back(i);
        list.resize(problem.D);
        for(int i = 0; i < size; i++) list[i%problem.D].push_back(i);
        ngrank = size;
        movengrank = size;
    }

    std::vector<int> tolist(std::vector<int> A)
    {
        std::vector<int> B;
        for(auto i:A)for(auto j:realIdx[i]) B.push_back(j);
        return B;
    }

    std::vector<std::vector<int>> tolist(std::vector<std::vector<int>> list)
    {
        for(int i = 0; i < list.size(); i++) list[i] = tolist(list[i]);
        return list;
    }

    int bfs(int s, int g)
    {
        std::vector<int> r(graph.size(), 0);
        std::queue<int> que;
        que.push(s);
        while(!que.empty())
        {
            int v = que.front();
            que.pop();
            if(r[v]) continue;
            r[v] = 1;
            if(v == g) break;
            for(auto x:graph[v]) if(!r[x]) que.push(x);
        }
        return r[g];
    }

    char ask(std::vector<int> lList, std::vector<int> rList)
    {
        lList = tolist(lList);
        rList = tolist(rList);
        if(lList.size() == 0 && rList.size() == 0) return '=';
        else if(lList.size() == 0) return '<';
        else if(rList.size() == 0) return '>';
        std::sort(lList.begin(), lList.end());
        std::sort(rList.begin(), rList.end());
        bool lr = 0, rl = 0;
        if(toIdx.count(lList) && toIdx.count(rList))
        {
            lr = bfs(toIdx[lList], toIdx[rList]);
            rl = bfs(toIdx[rList], toIdx[lList]);
        }
        char x;
        if(lr && rl) x = '=';
        else if(lr) x = '<';
        else if(rl) x = '>';
        else
        {
            if(!toIdx.count(lList)) toIdx[lList] = toIdx.size();
            if(!toIdx.count(rList)) toIdx[rList] = toIdx.size();
            graph.resize(toIdx.size());
            x = interface.ask(lList, rList), count++;
            if(x != '>') graph[toIdx[lList]].push_back(toIdx[rList]);
            if(x != '<') graph[toIdx[rList]].push_back(toIdx[lList]);
        }
        return x;
    }

    char preask(std::vector<int> lList, std::vector<int> rList)
    {
        lList = tolist(lList);
        rList = tolist(rList);
        if(lList.size() == 0 && rList.size() == 0) return '=';
        else if(lList.size() == 0) return '<';
        else if(rList.size() == 0) return '>';
        std::sort(lList.begin(), lList.end());
        std::sort(rList.begin(), rList.end());
        bool lr = 0, rl = 0;
        if(toIdx.count(lList) && toIdx.count(rList))
        {
            lr = bfs(toIdx[lList], toIdx[rList]);
            rl = bfs(toIdx[rList], toIdx[lList]);
        }
        char x = '0';
        if(lr && rl) x = '=';
        else if(lr) x = '<';
        else if(rl) x = '>';
        return x;
    }

    void listSort()
    {
        std::vector<std::vector<int>> sortList;
        for(int i = 0; i < list.size(); i++)
        {
            int ok = -1, ng = sortList.size();
            while(count < problem.Q && abs(ok - ng) > 1)
            {
                int mid = (ok + ng) / 2;
                char x = ask(list[i], sortList[mid]);
                if(x == '>') ok = mid;
                else ng = mid;
            }
            sortList.insert(sortList.begin() + ok + 1, list[i]);
            if(count == problem.Q) break;
        }
        if(sortList.size() == list.size()) list = sortList;
    }

    void weightSort()
    {
        std::vector<int> weightSortList;
        for(int i = 0; i < size; i++)
        {
            int ok = -1, ng = weightSortList.size();
            while(count < problem.Q && abs(ok - ng) > 1)
            {
                int mid = (ok + ng) / 2;
                char x = ask({i}, {weightSortList[mid]});
                if(x == '>') ok = mid;
                else ng = mid;
            }
            weightSortList.insert(weightSortList.begin() + ok + 1, i);
            if(count == problem.Q) break;
        }
        if(weightSortList.size() == size)
        {
            sortFlag = true;
            weightrank.resize(size);
            for(int i = 0; i < size; i++) weightrank[weightSortList[i]] = i;
        }
    }

    void weightSortInit()
    {
        list.clear();
        list.resize(problem.D);
        std::vector<int> weightIdx;
        for(int i = 0; i < size; i++) weightIdx.push_back(i);
        sort(weightIdx);
        for(int i = 0; i < size; i++)
        {
            list[(problem.D * 1000 - 1 - i) % problem.D].push_back(weightIdx[size - 1 - i]);
        }
    }

    void sort(std::vector<int> &list)
    {
        assert(sortFlag == true);
        std::vector<std::pair<int,int>> idxList;
        for(int i = 0; i < list.size(); i++) idxList.push_back({weightrank[list[i]], i});
        std::sort(idxList.begin(), idxList.end());
        std::vector<int> newList;
        for(int i = 0; i < list.size(); i++) newList.push_back(list[idxList[i].second]);
        list = newList;
    }

    void sortInsert(std::vector<int> A)
    {
        int ok = -1, ng = list.size();
        for(int i = 0; i < list.size(); i++)
        {
            char x = preask(A, list[i]);
            if(x == '>') ok = i;
            if(x == '<') ng = i;
        }
        while(count < problem.Q && abs(ok - ng) > 1)
        {
            int mid = (ok + ng) / 2;
            char x = ask(A, list[mid]);
            if(x == '>') ok = mid;
            else ng = mid;
        }
        list.insert(list.begin() + ok + 1, A);
    }

    bool randomflat(int lIdx, int rIdx, int breakcount = 1)
    {
        std::vector<int> w0 = list[lIdx], w1 = list[rIdx];
        std::vector<int> lswapList, rswapList;
        std::vector<int> lList = w0, rList = w1;
        int l = 0, r = 1;
        int trycount = 0;
        bool flag = false;
        while(count < problem.Q)
        {
            if(trycount > breakcount)
            {
                break;
            }
            if(lswapList.size() < l)
            {
                int idx = xorshift() % lList.size();
                lswapList.push_back(lList[idx]);
                lList.erase(lList.begin() + idx);
            }
            if(rswapList.size() < r)
            {
                int idx = xorshift() % rList.size();
                rswapList.push_back(rList[idx]);
                rList.erase(rList.begin() + idx);
            }
            {
                char x = ask(lswapList, rswapList);
                if(x != '<')
                {
                    if(r < w1.size() - 1)
                    {
                        r++;
                        continue;
                    }
                    break;
                }
            }
            {
                if(count == problem.Q)
                {
                    break;
                }
                int memo = count;
                char x = ask(lList, rList);
                if(memo < count) trycount++;
                if(x != '<')
                {
                    if(l < w0.size())
                    {
                        l++;
                        continue;
                    }
                    break;
                }
            }
            flag = true;
            for(auto i:rswapList) lList.push_back(i);
            for(auto i:lswapList) rList.push_back(i);
            w0 = lList, w1 = rList;
            if(count < problem.Q)
            {
                char x = ask(w0, w1);
                if(x == '<')
                {
                    lswapList.clear(), rswapList.clear();
                    lList = w0, rList = w1;
                    l = 0, r = 1;
                    trycount = breakcount;
                    continue;
                }
            }
            break;
        }
        if(flag)
        {
            list.erase(list.begin() + rIdx);
            list.erase(list.begin() + lIdx);
            sortInsert(w0);
            sortInsert(w1);
            return 1;
        }
        return 0;
    }

    bool greedyswap(int lIdx, int rIdx)
    {
        assert(sortFlag == true);
        auto w0 = list[lIdx], w1 = list[rIdx];
        sort(w0), sort(w1);
        std::vector<std::pair<int,int>> pairList, goodpairList;
        for(int i = w1.size() - 1; i >= 0 && count < problem.Q; i--)
        {
            int idx = -1;
            for(int j = 0; j < w0.size(); j++)
            {
                if(weightrank[w0[j]] < weightrank[w1[i]])
                {
                    idx = j;
                }
            }
            if(idx == -1) continue;
            while(i - 1 >= 0 && weightrank[w0[idx]] < weightrank[w1[i - 1]]) i--;
            bool flag = false;
            for(auto j:ngswap) if(weightrank[w0[idx]] <= j.first && j.second <= weightrank[w1[i]]) flag = true;
            if(flag) continue;
            flag = false;
            for(auto j:goodswap) if(j.first <= weightrank[w0[idx]] && weightrank[w1[i]] <= j.second) flag = true;
            if(flag) goodpairList.push_back({idx, i});
            else pairList.push_back({idx, i});
        }
        for(auto i:pairList) goodpairList.push_back(i);
        for(int j = 0; j < goodpairList.size() && count < problem.Q; j++)
        {
            int idx = goodpairList[j].first, i = goodpairList[j].second;
            auto copyw0 = w0, copyw1 = w1;
            copyw0.erase(copyw0.begin() + idx);
            copyw1.erase(copyw1.begin() + i);
            char x = ask(copyw0, copyw1);
            if(x == '<')
            {
                goodswap.push_back({w0[idx], w1[i]});
                std::swap(w0[idx], w1[i]);
                list.erase(list.begin() + rIdx);
                list.erase(list.begin() + lIdx);
                sortInsert(w0);
                sortInsert(w1);
                return 1;
            }
            else if(lIdx == 0 && rIdx == problem.D - 1) ngswap.push_back({weightrank[w0[idx]], weightrank[w1[i]]});
        }
        return 0;
    }

    bool greedymove(int lIdx, int rIdx)
    {
        assert(sortFlag == true);
        auto w0 = list[lIdx], w1 = list[rIdx];
        sort(w0), sort(w1);
        int w1min = w1[0];
        if(weightrank[w1[0]] >= movengrank) return 0;
        w1.erase(w1.begin());
        char x = ask(w0, w1);
        if(x == '<')
        {
            w0.push_back(w1min);
            list.erase(list.begin() + rIdx);
            list.erase(list.begin() + lIdx);
            sortInsert(w0);
            sortInsert(w1);
            return 1;
        }
        if(lIdx == 0 && rIdx == problem.D - 1) movengrank = weightrank[w1[0]];
        return 0;
    }

    bool greedy2swap(int lIdx, int rIdx)
    {
        assert(sortFlag == true);
        auto w0 = list[lIdx], w1 = list[rIdx];
        sort(w0), sort(w1);
        std::vector<std::pair<int,int>> pair0, pair1;
        std::vector<std::pair<int,int>> goodpair0List, goodpair1List;
        for(int i = w1.size() - 1; i >= 0 && count < problem.Q; i--)
        {
            int idx = -1;
            for(int j = 0; j < w0.size(); j++)
            {
                if(weightrank[w0[j]] < weightrank[w1[i]])
                {
                    idx = j;
                }
            }
            if(idx == -1) continue;
            while(i - 1 >= 0 && weightrank[w0[idx]] < weightrank[w1[i - 1]]) i--;
            bool flag = false;
            for(auto j:goodswap) if(j.first <= weightrank[w0[idx]] && weightrank[w1[i]] <= j.second) flag = true;
            if(flag) goodpair0List.push_back({idx, i});
            else pair0.push_back({idx, i});
        }
        for(int i = w0.size() - 1; i >= 0 && count < problem.Q; i--)
        {
            int idx = -1;
            for(int j = 0; j < w1.size(); j++)
            {
                if(weightrank[w1[j]] < weightrank[w0[i]])
                {
                    idx = j;
                }
            }
            if(idx == -1) continue;
            while(i - 1 >= 0 && weightrank[w1[idx]] < weightrank[w0[i - 1]]) i--;
            bool flag = false;
            for(auto j:goodswap) if(j.first <= weightrank[w1[idx]] && weightrank[w0[i]] <= j.second) flag = true;
            if(flag) goodpair1List.push_back({i, idx});
            else pair1.push_back({i, idx});
        }
        for(auto i:pair0) goodpair0List.push_back(i);
        for(auto i:pair1) goodpair1List.push_back(i);
        for(auto i:goodpair0List)for(auto j:goodpair1List)
        {
            if(count >= problem.Q - 1) continue;
            if(i.first == j.first || i.second == j.second) continue;
            auto copyw0 = w0, copyw1 = w1;
            std::vector<int> swapList0 = {w0[i.first], w0[j.first]}, swapList1 = {w1[i.second], w1[j.second]};
            char x = ask(swapList0, swapList1);
            if(x != '<') continue;
            copyw0.erase(copyw0.begin() + std::max(i.first, j.first));
            copyw0.erase(copyw0.begin() + std::min(i.first, j.first));
            copyw1.erase(copyw1.begin() + std::max(i.second, j.second));
            copyw1.erase(copyw1.begin() + std::min(i.second, j.second));
            x = ask(copyw0, copyw1);
            if(x == '<')
            {
                //std::cout<<"#!\n";
                std::swap(w0[i.first], w1[i.second]);
                std::swap(w0[j.first], w1[j.second]);
                list.erase(list.begin() + rIdx);
                list.erase(list.begin() + lIdx);
                sortInsert(w0);
                sortInsert(w1);
                return 1;
            }
        }
        return 0;
    }

    bool kick(int lIdx, int rIdx, int x = 0)
    {
        assert(sortFlag == true);
        auto listMemo = list;
        auto w0 = list[lIdx], w1 = list[rIdx];
        auto memo = w0;
        sort(w0), sort(w1);
        if(x == 1)
        {
            int w0min = w0[0];
            w0.erase(w0.begin());
            w1.push_back(w0min);
            list[lIdx] = w0, list[rIdx] = w1;
        }
        else
        {
            int w1min = w1[0];
            w1.erase(w1.begin());
            w0.push_back(w1min);
            list[lIdx] = w1, list[rIdx] = w0;
        }
        while(count < problem.Q)
        {
            auto w0 = list[lIdx], w1 = list[rIdx];
            sort(w0), sort(w1);
            bool flag = true;
            for(int i = w1.size() - 1; i >= 0 && count < problem.Q; i--)
            {
                int idx = -1;
                for(int j = 0; j < w0.size(); j++)
                {
                    if(weightrank[w0[j]] < weightrank[w1[i]])
                    {
                        idx = j;
                    }
                }
                if(idx == -1) continue;
                while(i - 1 >= 0 && weightrank[w0[idx]] < weightrank[w1[i - 1]]) i--;
                auto copyw0 = w0, copyw1 = w1;
                copyw0.erase(copyw0.begin() + idx);
                copyw1.erase(copyw1.begin() + i);
                char x = ask(copyw0, copyw1);
                if(x == '<')
                {
                    flag = false;
                    std::swap(w0[idx], w1[i]);
                    if(count == problem.Q) break;
                    char x = ask(w0, w1);
                    if(x == '>') std::swap(w0, w1);
                    list[lIdx] = w0, list[rIdx] = w1;
                    if(count == problem.Q) break;
                    auto memocp = memo, w0cp = w0;
                    for(int i = 0; i < memocp.size(); i++)
                    {
                        for(int j = 0; j < w0cp.size(); j++)
                        {
                            if(memocp[i] == w0cp[j])
                            {
                                memocp.erase(memocp.begin() + i);
                                w0cp.erase(w0cp.begin() + j);
                                i--;
                                break;
                            }
                        }
                    }
                    x = ask(memocp, w0cp);
                    if(x == '<')
                    {
                        //std::cout<<"# kick!\n";
                        list.erase(list.begin() + rIdx);
                        list.erase(list.begin() + lIdx);
                        sortInsert(w0);
                        sortInsert(w1);
                        return 1;
                    }
                    break;
                }
                else ngswap.push_back({weightrank[w0[idx]], weightrank[w1[i]]});
            }
            if(flag) break;
        }
        list = listMemo;
        return 0;
    }

    bool greedyokmove(int lIdx, int rIdx)
    {
        assert(sortFlag == true);
        auto w0 = list[lIdx], w1 = list[rIdx];
        sort(w0), sort(w1);
        int ok = -1;
        int ng = w1.size();
        for(int i = 0; i < w1.size(); i++)
        {
            if(weightrank[w1[i]] >= ngrank)
            {
                ng = i;
                break;
            }
        }
        while(abs(ok - ng) > 1 && count < problem.Q)
        {
            int mid = (ok + ng) / 2;
            auto memo0 = w0, memo1 = w1;
            memo0.push_back(w1[mid]);
            memo1.erase(memo1.begin() + mid);
            char x = ask(memo0, memo1);
            if(x == '<') ok = mid;
            else if(x == '=')
            {
                ok = mid;
                break;
            }
            else ng = mid, ngrank = weightrank[w1[mid]];
        }
        if(ok != -1)
        {
            if(ok < w1.size() - 1 && count < problem.Q && weightrank[w1[ok + 1]] < movengrank)
            {
                int w1min = w1[ok + 1];
                auto memo = w1;
                memo.erase(memo.begin() + ok + 1);
                char x = ask(w0, memo);
                if(x == '<') ok++;
            }
            w0.push_back(w1[ok]);
            w1.erase(w1.begin() + ok);
            list.erase(list.begin() + rIdx);
            list.erase(list.begin() + lIdx);
            sortInsert(w0);
            sortInsert(w1);
            return 1;
        }
        return 0;
    }
};

struct State
{
    Problem problem;
    Interface &interface;
    std::vector<std::vector<int>> list;
    bool sortFlag = false;
    std::vector<int> weightrank;
    int count = 0;
    int ngrank;
    int movengrank;

    std::map<std::vector<int>,int> toIdx;
    std::vector<std::vector<int>> graph;
    std::vector<std::pair<int,int>> ngswap;
    std::vector<std::pair<int,int>> goodswap;

    State(Problem problem, Interface &interface):problem(problem), interface(interface)
    {
        list.resize(problem.D);
        for(int i = 0; i < problem.N; i++) list[i%problem.D].push_back(i);
        ngrank = problem.N;
        movengrank = problem.N;
    }

    int bfs(int s, int g)
    {
        std::vector<int> r(graph.size(), 0);
        std::queue<int> que;
        que.push(s);
        while(!que.empty())
        {
            int v = que.front();
            que.pop();
            if(r[v]) continue;
            r[v] = 1;
            if(v == g) break;
            for(auto x:graph[v]) if(!r[x]) que.push(x);
        }
        return r[g];
    }

    char ask(std::vector<int> lList, std::vector<int> rList)
    {
        if(lList.size() == 0 && rList.size() == 0) return '=';
        else if(lList.size() == 0) return '<';
        else if(rList.size() == 0) return '>';
        std::sort(lList.begin(), lList.end());
        std::sort(rList.begin(), rList.end());
        bool lr = 0, rl = 0;
        if(toIdx.count(lList) && toIdx.count(rList))
        {
            lr = bfs(toIdx[lList], toIdx[rList]);
            rl = bfs(toIdx[rList], toIdx[lList]);
        }
        char x;
        if(lr && rl) x = '=';
        else if(lr) x = '<';
        else if(rl) x = '>';
        else
        {
            if(!toIdx.count(lList)) toIdx[lList] = toIdx.size();
            if(!toIdx.count(rList)) toIdx[rList] = toIdx.size();
            graph.resize(toIdx.size());
            x = interface.ask(lList, rList), count++;
            if(x != '>') graph[toIdx[lList]].push_back(toIdx[rList]);
            if(x != '<') graph[toIdx[rList]].push_back(toIdx[lList]);
        }
        return x;
    }

    char preask(std::vector<int> lList, std::vector<int> rList)
    {
        if(lList.size() == 0 && rList.size() == 0) return '=';
        else if(lList.size() == 0) return '<';
        else if(rList.size() == 0) return '>';
        std::sort(lList.begin(), lList.end());
        std::sort(rList.begin(), rList.end());
        bool lr = 0, rl = 0;
        if(toIdx.count(lList) && toIdx.count(rList))
        {
            lr = bfs(toIdx[lList], toIdx[rList]);
            rl = bfs(toIdx[rList], toIdx[lList]);
        }
        char x = '0';
        if(lr && rl) x = '=';
        else if(lr) x = '<';
        else if(rl) x = '>';
        return x;
    }

    void listSort()
    {
        std::vector<std::vector<int>> sortList;
        for(int i = 0; i < list.size(); i++)
        {
            int ok = -1, ng = sortList.size();
            while(count < problem.Q && abs(ok - ng) > 1)
            {
                int mid = (ok + ng) / 2;
                char x = ask(list[i], sortList[mid]);
                if(x == '>') ok = mid;
                else ng = mid;
            }
            sortList.insert(sortList.begin() + ok + 1, list[i]);
            if(count == problem.Q) break;
        }
        if(sortList.size() == list.size()) list = sortList;
    }

    void weightSort()
    {
        std::vector<int> weightSortList;
        for(int i = 0; i < problem.N; i++)
        {
            int ok = -1, ng = weightSortList.size();
            while(count < problem.Q && abs(ok - ng) > 1)
            {
                int mid = (ok + ng) / 2;
                char x = ask({i}, {weightSortList[mid]});
                if(x == '>') ok = mid;
                else ng = mid;
            }
            weightSortList.insert(weightSortList.begin() + ok + 1, i);
            if(count == problem.Q) break;
        }
        if(weightSortList.size() == problem.N)
        {
            sortFlag = true;
            weightrank.resize(problem.N);
            for(int i = 0; i < problem.N; i++) weightrank[weightSortList[i]] = i;
        }
    }

    void weightSortInit()
    {
        list.clear();
        list.resize(problem.D);
        std::vector<int> weightIdx;
        for(int i = 0; i < problem.N; i++) weightIdx.push_back(i);
        sort(weightIdx);
        for(int i = 0; i < problem.N; i++)
        {
            list[(problem.D * 1000 - 1 - i) % problem.D].push_back(weightIdx[problem.N - 1 - i]);
        }
    }

    void sort(std::vector<int> &list)
    {
        assert(sortFlag == true);
        std::vector<std::pair<int,int>> idxList;
        for(int i = 0; i < list.size(); i++) idxList.push_back({weightrank[list[i]], i});
        std::sort(idxList.begin(), idxList.end());
        std::vector<int> newList;
        for(int i = 0; i < list.size(); i++) newList.push_back(list[idxList[i].second]);
        list = newList;
    }

    void sortInsert(std::vector<int> A)
    {
        int ok = -1, ng = list.size();
        for(int i = 0; i < list.size(); i++)
        {
            char x = preask(A, list[i]);
            if(x == '>') ok = i;
            if(x == '<') ng = i;
        }
        while(count < problem.Q && abs(ok - ng) > 1)
        {
            int mid = (ok + ng) / 2;
            char x = ask(A, list[mid]);
            if(x == '>') ok = mid;
            else ng = mid;
        }
        list.insert(list.begin() + ok + 1, A);
    }

    bool randomflat(int lIdx, int rIdx, int breakcount = 1)
    {
        std::vector<int> w0 = list[lIdx], w1 = list[rIdx];
        std::vector<int> lswapList, rswapList;
        std::vector<int> lList = w0, rList = w1;
        int l = 0, r = 1;
        int trycount = 0;
        bool flag = false;
        while(count < problem.Q)
        {
            if(trycount > breakcount)
            {
                break;
            }
            if(lswapList.size() < l)
            {
                int idx = xorshift() % lList.size();
                lswapList.push_back(lList[idx]);
                lList.erase(lList.begin() + idx);
            }
            if(rswapList.size() < r)
            {
                int idx = xorshift() % rList.size();
                rswapList.push_back(rList[idx]);
                rList.erase(rList.begin() + idx);
            }
            {
                char x = ask(lswapList, rswapList);
                if(x != '<')
                {
                    if(r < w1.size() - 1)
                    {
                        r++;
                        continue;
                    }
                    break;
                }
            }
            {
                if(count == problem.Q)
                {
                    break;
                }
                int memo = count;
                char x = ask(lList, rList);
                if(memo < count) trycount++;
                if(x != '<')
                {
                    if(l < w0.size())
                    {
                        l++;
                        continue;
                    }
                    break;
                }
            }
            flag = true;
            for(auto i:rswapList) lList.push_back(i);
            for(auto i:lswapList) rList.push_back(i);
            w0 = lList, w1 = rList;
            if(count < problem.Q)
            {
                char x = ask(w0, w1);
                if(x == '<')
                {
                    lswapList.clear(), rswapList.clear();
                    lList = w0, rList = w1;
                    l = 0, r = 1;
                    trycount = breakcount;
                    continue;
                }
            }
            break;
        }
        if(flag)
        {
            list.erase(list.begin() + rIdx);
            list.erase(list.begin() + lIdx);
            sortInsert(w0);
            sortInsert(w1);
            return 1;
        }
        return 0;
    }

    bool greedyswap(int lIdx, int rIdx)
    {
        assert(sortFlag == true);
        auto w0 = list[lIdx], w1 = list[rIdx];
        sort(w0), sort(w1);
        std::vector<std::pair<int,int>> pairList, goodpairList;
        for(int i = w1.size() - 1; i >= 0 && count < problem.Q; i--)
        {
            int idx = -1;
            for(int j = 0; j < w0.size(); j++)
            {
                if(weightrank[w0[j]] < weightrank[w1[i]])
                {
                    idx = j;
                }
            }
            if(idx == -1) continue;
            while(i - 1 >= 0 && weightrank[w0[idx]] < weightrank[w1[i - 1]]) i--;
            bool flag = false;
            for(auto j:ngswap) if(weightrank[w0[idx]] <= j.first && j.second <= weightrank[w1[i]]) flag = true;
            if(flag) continue;
            flag = false;
            for(auto j:goodswap) if(j.first <= weightrank[w0[idx]] && weightrank[w1[i]] <= j.second) flag = true;
            if(flag) goodpairList.push_back({idx, i});
            else pairList.push_back({idx, i});
        }
        for(auto i:pairList) goodpairList.push_back(i);
        for(int j = 0; j < goodpairList.size() && count < problem.Q; j++)
        {
            int idx = goodpairList[j].first, i = goodpairList[j].second;
            auto copyw0 = w0, copyw1 = w1;
            copyw0.erase(copyw0.begin() + idx);
            copyw1.erase(copyw1.begin() + i);
            char x = ask(copyw0, copyw1);
            if(x == '<')
            {
                goodswap.push_back({w0[idx], w1[i]});
                std::swap(w0[idx], w1[i]);
                list.erase(list.begin() + rIdx);
                list.erase(list.begin() + lIdx);
                sortInsert(w0);
                sortInsert(w1);
                return 1;
            }
            else if(lIdx == 0 && rIdx == problem.D - 1) ngswap.push_back({weightrank[w0[idx]], weightrank[w1[i]]});
        }
        return 0;
    }

    bool greedymove(int lIdx, int rIdx)
    {
        assert(sortFlag == true);
        auto w0 = list[lIdx], w1 = list[rIdx];
        sort(w0), sort(w1);
        int w1min = w1[0];
        if(weightrank[w1[0]] >= movengrank) return 0;
        w1.erase(w1.begin());
        char x = ask(w0, w1);
        if(x == '<')
        {
            w0.push_back(w1min);
            list.erase(list.begin() + rIdx);
            list.erase(list.begin() + lIdx);
            sortInsert(w0);
            sortInsert(w1);
            return 1;
        }
        if(lIdx == 0 && rIdx == problem.D - 1) movengrank = weightrank[w1[0]];
        return 0;
    }

    bool greedy2swap(int lIdx, int rIdx)
    {
        assert(sortFlag == true);
        auto w0 = list[lIdx], w1 = list[rIdx];
        sort(w0), sort(w1);
        std::vector<std::pair<int,int>> pair0, pair1;
        std::vector<std::pair<int,int>> goodpair0List, goodpair1List;
        for(int i = w1.size() - 1; i >= 0 && count < problem.Q; i--)
        {
            int idx = -1;
            for(int j = 0; j < w0.size(); j++)
            {
                if(weightrank[w0[j]] < weightrank[w1[i]])
                {
                    idx = j;
                }
            }
            if(idx == -1) continue;
            while(i - 1 >= 0 && weightrank[w0[idx]] < weightrank[w1[i - 1]]) i--;
            bool flag = false;
            for(auto j:goodswap) if(j.first <= weightrank[w0[idx]] && weightrank[w1[i]] <= j.second) flag = true;
            if(flag) goodpair0List.push_back({idx, i});
            else pair0.push_back({idx, i});
        }
        for(int i = w0.size() - 1; i >= 0 && count < problem.Q; i--)
        {
            int idx = -1;
            for(int j = 0; j < w1.size(); j++)
            {
                if(weightrank[w1[j]] < weightrank[w0[i]])
                {
                    idx = j;
                }
            }
            if(idx == -1) continue;
            while(i - 1 >= 0 && weightrank[w1[idx]] < weightrank[w0[i - 1]]) i--;
            bool flag = false;
            for(auto j:goodswap) if(j.first <= weightrank[w1[idx]] && weightrank[w0[i]] <= j.second) flag = true;
            if(flag) goodpair1List.push_back({i, idx});
            else pair1.push_back({i, idx});
        }
        for(auto i:pair0) goodpair0List.push_back(i);
        for(auto i:pair1) goodpair1List.push_back(i);
        for(auto i:goodpair0List)for(auto j:goodpair1List)
        {
            if(count >= problem.Q - 1) continue;
            if(i.first == j.first || i.second == j.second) continue;
            auto copyw0 = w0, copyw1 = w1;
            std::vector<int> swapList0 = {w0[i.first], w0[j.first]}, swapList1 = {w1[i.second], w1[j.second]};
            char x = ask(swapList0, swapList1);
            if(x != '<') continue;
            copyw0.erase(copyw0.begin() + std::max(i.first, j.first));
            copyw0.erase(copyw0.begin() + std::min(i.first, j.first));
            copyw1.erase(copyw1.begin() + std::max(i.second, j.second));
            copyw1.erase(copyw1.begin() + std::min(i.second, j.second));
            x = ask(copyw0, copyw1);
            if(x == '<')
            {
                //std::cout<<"#!\n";
                std::swap(w0[i.first], w1[i.second]);
                std::swap(w0[j.first], w1[j.second]);
                list.erase(list.begin() + rIdx);
                list.erase(list.begin() + lIdx);
                sortInsert(w0);
                sortInsert(w1);
                return 1;
            }
        }
        return 0;
    }

    bool kick(int lIdx, int rIdx, int x = 0)
    {
        assert(sortFlag == true);
        auto listMemo = list;
        auto w0 = list[lIdx], w1 = list[rIdx];
        auto memo = w0;
        sort(w0), sort(w1);
        if(x == 1)
        {
            int w0min = w0[0];
            w0.erase(w0.begin());
            w1.push_back(w0min);
            list[lIdx] = w0, list[rIdx] = w1;
        }
        else
        {
            int w1min = w1[0];
            w1.erase(w1.begin());
            w0.push_back(w1min);
            list[lIdx] = w1, list[rIdx] = w0;
        }
        while(count < problem.Q)
        {
            auto w0 = list[lIdx], w1 = list[rIdx];
            sort(w0), sort(w1);
            bool flag = true;
            for(int i = w1.size() - 1; i >= 0 && count < problem.Q; i--)
            {
                int idx = -1;
                for(int j = 0; j < w0.size(); j++)
                {
                    if(weightrank[w0[j]] < weightrank[w1[i]])
                    {
                        idx = j;
                    }
                }
                if(idx == -1) continue;
                while(i - 1 >= 0 && weightrank[w0[idx]] < weightrank[w1[i - 1]]) i--;
                auto copyw0 = w0, copyw1 = w1;
                copyw0.erase(copyw0.begin() + idx);
                copyw1.erase(copyw1.begin() + i);
                char x = ask(copyw0, copyw1);
                if(x == '<')
                {
                    flag = false;
                    std::swap(w0[idx], w1[i]);
                    if(count == problem.Q) break;
                    char x = ask(w0, w1);
                    if(x == '>') std::swap(w0, w1);
                    list[lIdx] = w0, list[rIdx] = w1;
                    if(count == problem.Q) break;
                    auto memocp = memo, w0cp = w0;
                    for(int i = 0; i < memocp.size(); i++)
                    {
                        for(int j = 0; j < w0cp.size(); j++)
                        {
                            if(memocp[i] == w0cp[j])
                            {
                                memocp.erase(memocp.begin() + i);
                                w0cp.erase(w0cp.begin() + j);
                                i--;
                                break;
                            }
                        }
                    }
                    x = ask(memocp, w0cp);
                    if(x == '<')
                    {
                        //std::cout<<"# kick!\n";
                        list.erase(list.begin() + rIdx);
                        list.erase(list.begin() + lIdx);
                        sortInsert(w0);
                        sortInsert(w1);
                        return 1;
                    }
                    break;
                }
                else ngswap.push_back({weightrank[w0[idx]], weightrank[w1[i]]});
            }
            if(flag) break;
        }
        list = listMemo;
        return 0;
    }

    bool greedyokmove(int lIdx, int rIdx)
    {
        assert(sortFlag == true);
        auto w0 = list[lIdx], w1 = list[rIdx];
        sort(w0), sort(w1);
        int ok = -1;
        int ng = w1.size();
        for(int i = 0; i < w1.size(); i++)
        {
            if(weightrank[w1[i]] >= ngrank)
            {
                ng = i;
                break;
            }
        }
        while(abs(ok - ng) > 1 && count < problem.Q)
        {
            int mid = (ok + ng) / 2;
            auto memo0 = w0, memo1 = w1;
            memo0.push_back(w1[mid]);
            memo1.erase(memo1.begin() + mid);
            char x = ask(memo0, memo1);
            if(x == '<') ok = mid;
            else if(x == '=')
            {
                ok = mid;
                break;
            }
            else ng = mid, ngrank = weightrank[w1[mid]];
        }
        if(ok != -1)
        {
            if(ok < w1.size() - 1 && count < problem.Q && weightrank[w1[ok + 1]] < movengrank)
            {
                int w1min = w1[ok + 1];
                auto memo = w1;
                memo.erase(memo.begin() + ok + 1);
                char x = ask(w0, memo);
                if(x == '<') ok++;
            }
            w0.push_back(w1[ok]);
            w1.erase(w1.begin() + ok);
            list.erase(list.begin() + rIdx);
            list.erase(list.begin() + lIdx);
            sortInsert(w0);
            sortInsert(w1);
            return 1;
        }
        return 0;
    }
};

struct Solver
{
    Problem problem;
    Interface interface;

    Solver()
    {
        problem.Read();
        interface.init(problem);
    }

    int Solve1(bool sim = false)
    {
        State state(problem, interface);
        state.listSort();
        if(!sim) interface.semianswer(state.list);
        int l = 0, r = problem.D - 1;
        int ncnt = 0;
        int nimpcnt = 0;
        while(state.count < problem.Q)
        {
            nimpcnt++;
            int memo = state.count;
            l = 0, r = problem.D - 1;
            while(state.list[r].size() == 1) r--;
            if(ncnt > 10)
            {
                if(rand01() < 0.5)
                {
                    r = xorshift() % problem.D;
                    while(state.list[r].size() == 1 || r <= l) r = xorshift() % problem.D;
                }
                else
                {
                    l = xorshift() % problem.D;
                    while(r <= l) l = xorshift() % problem.D;
                }
                if(ncnt > 20)
                {
                    r = xorshift() % problem.D;
                    l = xorshift() % problem.D;
                    while(state.list[r].size() == 1 || r <= l)
                    {
                        r = xorshift() % problem.D;
                        l = xorshift() % problem.D;
                    }
                }
            }
            int x = nimpcnt / 10 + 1;
            bool flag = state.randomflat(l, r, x);
            if(memo == state.count) ncnt++;
            if(flag) ncnt = 0, nimpcnt = 0;
            if(!sim) interface.semianswer(state.list);
        }
        if(sim) return interface.score(state.list);
        interface.answer(state.list);
        return 0;
    }

    int Solve2(bool sim = false)
    {
        State state(problem, interface);
        state.weightSort();
        if(state.sortFlag) state.weightSortInit();
        if(!sim) interface.semianswer(state.list);
        int l = 0, r = problem.D - 1;
        bool pien = false;
        int ncnt = 0;
        while(state.count < problem.Q)
        {
            bool flag = false;
            if(pien)
            {
                int memo = state.count;
                l = 0, r = problem.D - 1;
                while(state.list[r].size() == 1) r--;
                if(ncnt > 10)
                {
                    if(rand01() < 0.5)
                    {
                        r = xorshift() % problem.D;
                        while(state.list[r].size() == 1 || r <= l) r = xorshift() % problem.D;
                    }
                    else
                    {
                        l = xorshift() % problem.D;
                        while(r <= l) l = xorshift() % problem.D;
                    }
                    if(ncnt > 20)
                    {
                        r = xorshift() % problem.D;
                        l = xorshift() % problem.D;
                        while(state.list[r].size() == 1 || r <= l)
                        {
                            r = xorshift() % problem.D;
                            l = xorshift() % problem.D;
                        }
                    }
                }
                bool flag = state.randomflat(l, r, 4);
                if(memo == state.count) ncnt++;
                if(flag)
                {
                    ncnt = 0;l = 0, r = problem.D - 1, pien = false;
                }
                if(!sim) interface.semianswer(state.list);
            }
            else if(state.list[r].size() != 1)
            {
                flag = state.greedyokmove(l, r);
                if(!flag && state.count < problem.Q) flag = state.greedymove(l, r);
                if(!flag && state.count < problem.Q) flag = state.greedyswap(l, r);
                if(!flag && state.count < problem.Q) flag = state.greedy2swap(l, r);
                if(!flag && state.count < problem.Q) flag = state.kick(l, r);
                if(!flag && state.count < problem.Q && state.list[l].size() != 1) flag = state.kick(l, r, 1);
            }
            if(!sim) interface.semianswer(state.list);
            if(flag) l = 0, r = problem.D - 1, pien = false;
            if(!flag)
            {
                if(r - l == 1 && r == problem.D - 1)
                {
                    l = 0, r = problem.D - 1;
                    pien = true;
                }
                else if(r == problem.D - 1) r = r - l - 1, l = 0;
                else l = problem.D - 1 - r, r = problem.D - 1;
            }
        }
        if(sim) return interface.score(state.list);
        interface.answer(state.list);
        return 0;
    }

    int Solve3(bool sim = false)
    {
        State2 state(problem, interface);
        state.weightSort();
        if(state.sortFlag) state.weightSortInit();
        if(!sim) interface.semianswer(state.tolist(state.list));
        int l = 0, r = problem.D - 1;
        bool pien = false;
        int nimpcnt = 0;
        while(state.count < problem.Q)
        {
            nimpcnt++;
            bool flag = false;
            if(pien)
            {
                state.size = problem.N;
                state.list = state.tolist(state.list);
                state.realIdx.resize(problem.N);
                for(int i = 0; i < problem.N; i++) state.realIdx[i] = {i};
                int ncnt = 0;
                while(state.count < problem.Q)
                {
                    int memo = state.count;
                    l = 0, r = problem.D - 1;
                    while(state.list[r].size() == 1) r--;
                    if(ncnt > 10)
                    {
                        if(rand01() < 0.5)
                        {
                            r = xorshift() % problem.D;
                            while(state.list[r].size() == 1 || r <= l) r = xorshift() % problem.D;
                        }
                        else
                        {
                            l = xorshift() % problem.D;
                            while(r <= l) l = xorshift() % problem.D;
                        }
                        if(ncnt > 20)
                        {
                            r = xorshift() % problem.D;
                            l = xorshift() % problem.D;
                            while(state.list[r].size() == 1 || r <= l)
                            {
                                r = xorshift() % problem.D;
                                l = xorshift() % problem.D;
                            }
                        }
                    }
                    int x = nimpcnt / 6 + 1;
                    bool flag = state.randomflat(l, r, x);
                    if(memo == state.count) ncnt++;
                    if(flag) ncnt = 0, nimpcnt = 0;
                    if(!sim) interface.semianswer(state.tolist(state.list));
                }
                break;
            }
            else if(state.list[r].size() != 1)
            {
                flag = state.greedyokmove(l, r);
                if(!flag && state.count < problem.Q) flag = state.greedymove(l, r);
                if(!flag && state.count < problem.Q) flag = state.greedyswap(l, r);
                if(!flag && state.count < problem.Q) flag = state.greedy2swap(l, r);
            }
            if(!sim) interface.semianswer(state.tolist(state.list));
            if(flag) l = 0, r = problem.D - 1, pien = false;
            if(!flag)
            {
                l = 0, r = problem.D - 1;
                pien = true;
            }
        }
        if(sim) return interface.score(state.tolist(state.list));
        interface.answer(state.tolist(state.list));
        return 0;
    }

    void Solve()
    {
        //interface.sim = false;
        //Solve1();
        //return;
        std::vector<int> count(3, 0);
        for(int i = 0; i < 10000; i++)
        {
            if(timer.ms() > 1500) break;
            int score1 = Solve1(1);
            int score2 = Solve2(1);
            int score3 = Solve3(1);
            std::vector<int> scores = {score1, score2, score3};
            int minscore = *std::min_element(scores.begin(), scores.end());
            for(int i = 0; i < 3; i++) if(scores[i] == minscore) count[i]++;
            interface.reset();
        }
        int idx = std::max_element(count.begin(), count.end()) - count.begin();
        std::cout<<"# "<<idx<<"\n";
        std::cout<<"# "<<count[0]<<" "<<count[1]<<" "<<count[2]<<"\n";
        interface.sim = false;
        if(idx == 0) Solve1();
        else if(idx == 1) Solve2();
        else Solve3();
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
}