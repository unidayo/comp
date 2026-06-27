#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>

#include <chrono>
using namespace std::chrono;


struct Timer {
    system_clock::time_point start;

    Timer() { reset(); }

    void reset() { start = system_clock::now(); }

    double sec_elapsed() {
        return duration_cast<milliseconds>(system_clock::now() - start).count() / 1000.0;
    }
};


uint64_t rand64() {
    static uint64_t x = 88172645463325252ULL;
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
}


double rand_p() {
    return rand64() * (1.0 / UINT64_MAX);
}


struct UnionFind {
    std::vector<int> parent;
    int cnt;

    UnionFind(int n) : parent(n, -1), cnt(n) {}

    int root(int x) {
        if (parent[x] < 0) return x;
        else return parent[x] = root(parent[x]);
    }

    bool merge(int x, int y) {
        x = root(x);
        y = root(y);
        if (x == y) return false;
        if (parent[x] > parent[y]) std::swap(x, y);
        parent[x] += parent[y];
        parent[y] = x;
        cnt -= 1;
        return true;
    }

    int same(int x, int y) { return root(x) == root(y); }

    int size(int x) { return -parent[root(x)]; }

    int count() { return cnt; }
};


struct Edge {
    int from, to;
    long long cost;
    int index;

    Edge(int from, int to, long long cost, int index) : from(from), to(to), cost(cost), index(index) {}
};


std::vector<int> kruskal(int n, std::vector<Edge> edges, std::vector<int> P) {
    std::sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b) {
        return a.cost < b.cost;
    });
    UnionFind uf(n);

    std::vector<std::vector<Edge>> tree(n);
    std::vector<int> result(edges.size(), 0);

    for (int i = 0; i < edges.size(); i++) {
        auto e = edges[i];
        if (not uf.same(e.from, e.to)) {
            uf.merge(e.from, e.to);
            result[e.index] = 1;
            tree[e.from].push_back(Edge(e.from, e.to, -1, e.index));
            tree[e.to].push_back(Edge(e.to, e.from, -1, e.index));
        }
    }
    std::vector<int> degs(n, 0);
    std::vector<int> stack;
    for (int i = 0; i < n; i++) {
        degs[i] = (int)tree[i].size();
        if (degs[i] == 1) stack.push_back(i);
    }

    while (!stack.empty()) {
        int v = stack.back();
        stack.pop_back();
        if (P[v] == 0 && v != 0) {
            for(const auto e : tree[v]) {
                result[e.index] = 0;
                degs[e.to] -= 1;
                if (degs[e.to] == 1) stack.push_back(e.to);
            }
        }
    }
    
    return result;
}


long long N, M, K;
long long nx[100], ny[100];
long long kx[5000], ky[5000];
std::vector<Edge> edges;

long long iter_count = 0;

void input() {
    std::cin >> N >> M >> K;
    for (int i = 0; i < N; i++) {
        std::cin >> nx[i] >> ny[i];
    }
    for (int i = 0; i < M; i++) {
        int u, v;
        long long cost;
        std::cin >> u >> v >> cost;
        edges.emplace_back(u - 1, v - 1, cost, i);
    }
    for (int i = 0; i < K; i++) {
        std::cin >> kx[i] >> ky[i];
    }
}


long long get_cost1(std::vector<int> P) {
    long long result = 0;
    for (int i = 0; i < P.size(); i++) result += P[i] * P[i];
    return result;
}

long long get_cost2(std::vector<int> B) {
    long long result = 0;
    for (int i = 0; i < B.size(); i++) {
        if (B[i] == 1) result += edges[i].cost;
    }
    return result;
}

void initialize(std::vector<int> &P, std::vector<std::vector<bool>> &included, std::vector<int> &counts) {
    for (int i = 0; i < K; i++) {
        int x = kx[i];
        int y = ky[i];
        counts[i] = 0;
        for (int j = 0; j < N; j++) {
            int xx = nx[j];
            int yy = ny[j];
            included[i][j] = ((x - xx) * (x - xx) + (y - yy) * (y - yy) <= P[j] * P[j]);
            if (included[i][j]) counts[i] += 1;
        }
    }
}

std::vector<int> objects;
void optimize(int i, int j, double tm, std::vector<int> &P, std::vector<std::vector<bool>> &included, std::vector<int> &counts) {
    int prev_cost = P[i] * P[i] + P[j] * P[j];
    int prev_pi = P[i];
    int prev_pj = P[j];
    objects.clear();
    iter_count += 1;
    for (int k = 0; k < K; k++) {
        int cnt = 0;
        if (included[k][i]) cnt += 1;
        if (included[k][j]) cnt += 1;
        if (cnt == counts[k]) objects.emplace_back(k);
    }
    P[i] = 5000;
    P[j] = 5000;

    int xi = nx[i]; int yi = ny[i]; int xj = nx[j]; int yj = ny[j];
    for (int t = 0; t < 200; t++) {
        bool ok = true;
        int pi = P[i];
        int pj = P[j];
        if (rand64() % 2 == 0) { pi = rand64() % (pi + 1); }
        else { pj = rand64() % (pj + 1); }
        
        for (auto const k : objects) {
            int xx = kx[k];
            int yy = ky[k];

            if ((xi - xx) * (xi - xx) + (yi - yy) * (yi - yy) <= pi * pi) continue;
            if ((xj - xx) * (xj - xx) + (yj - yy) * (yj - yy) <= pj * pj) continue;
            ok = false;
            break;
        }
        if (ok) {
            P[i] = pi;
            P[j] = pj;
        };
    }
    int cost = P[i] * P[i] + P[j] * P[j];

    if (prev_cost <= cost) {
        P[i] = prev_pi;
        P[j] = prev_pj;
    } else {
        for (int k = 0; k < K; k++) {
            int x = kx[k];
            int y = ky[k];
            if (included[k][i]) counts[k] -= 1;
            if (included[k][j]) counts[k] -= 1;
            included[k][i] = ((x - xi) * (x - xi) + (y - yi) * (y - yi) <= P[i] * P[i]);
            included[k][j] = ((x - xj) * (x - xj) + (y - yj) * (y - yj) <= P[j] * P[j]);
            if (included[k][i]) counts[k] += 1;
            if (included[k][j]) counts[k] += 1;
        }
    }
}


int main() {
    Timer timer;
    input();

    std::vector<std::vector<int>> cand_p;
    std::vector<int> P(N, 5000);
    std::vector<std::vector<bool>> included(K, std::vector<bool>(N, false));
    std::vector<int> counts(K, 0);

    initialize(P, included, counts);
    double tm = timer.sec_elapsed();
    while (tm < 1.95) {
       if (iter_count % 1000 == 0) {
           cand_p.push_back(P);
           for (int i = 0; i < N; i++) {
               P[i] = P[i] + 1000;
               if (P[i] > 5000) P[i] = 5000;
           }
           initialize(P, included, counts);
       }
       int k = rand64() % M;
       int i = edges[k].from;
       int j = edges[k].to;
      
       optimize(i, j, tm, P, included, counts);
       tm = timer.sec_elapsed();
    }
    cand_p.push_back(P);

    std::vector<long long> costs(cand_p.size(), 0);
    long long min_cost = 1000000000000;
    for (int i = 0; i < cand_p.size(); i++) {
        auto B = kruskal(N, edges, cand_p[i]);
        costs[i] = get_cost1(cand_p[i]) + get_cost2(B);
        if (costs[i] < min_cost) min_cost = costs[i];
    }
 
    for (int k = 0; k < cand_p.size(); k++) {
        if (costs[k] != min_cost) continue;
        for (int i = 0; i < N; i++) {
            std::cout << cand_p[k][i] << " ";
        }
        std::cout << std::endl;
  
        auto B = kruskal(N, edges, cand_p[k]);
        for (int i = 0; i < M; i++) {
            std::cout << B[i] << " ";
        }
        std::cout << std::endl;
  
        std::cerr << get_cost1(cand_p[k]) << std::endl;
        std::cerr << get_cost2(B) << std::endl;
        std::cerr << iter_count << std::endl;
        break;
    }
}