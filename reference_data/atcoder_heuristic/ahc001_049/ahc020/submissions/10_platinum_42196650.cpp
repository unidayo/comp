#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <set>
#include <cassert>
#define rep(i,n) for(int i = 0; i < (int)(n); i++)

using namespace std;
using LL = long long;
using pii = pair<int,int>;
using vvi = vector<vector<int>>;
const int INF = (int)1e9;
const LL LINF = (LL)1e18;

const double TL = 1.95;

const int testcase = 0;

enum class file_status{
    local,
    debug,
    score,
    submit,
};
file_status now_file_status = file_status::submit;

void read_input(){
    std::stringstream ss;
    std::string num = std::to_string(testcase);
    int siz = num.size();
    for(int i = 0; i < 4 - siz; i++) num = '0' + num;
    ss << "tools/in/" << num << ".txt";
    FILE *in = freopen(ss.str().c_str(), "r", stdin);
}
void file_output(){
    std::stringstream ss;
    std::string num = std::to_string(testcase);
    int siz = num.size();
    for(int i = 0; i < 4 - siz; i++) num = '0' + num;
    ss << "tools/out/" << num << ".txt";
    FILE *out = freopen(ss.str().c_str(), "w", stdout);
}

class UnionFind {
public:
    vector <long long> par;
    vector <long long> siz;

    UnionFind() {}

    UnionFind(long long sz_): par(sz_), siz(sz_, (long long)1) {
        for (long long i = 0; i < sz_; ++i) par[i] = i;
    }
    void init(long long sz_) {
        par.resize(sz_);
        siz.assign(sz_, (long long)1);
        for (long long i = 0; i < sz_; ++i) par[i] = i;
    }

    long long root(long long x) {
        while (par[x] != x) {
            x = par[x] = par[par[x]];
        }
        return x;
    }

    bool unite(long long x, long long y) {
        x = root(x);
        y = root(y);
        if (x == y) return false;
        if (siz[x] < siz[y]) swap(x, y);
        siz[x] += siz[y];
        par[y] = x;
        return true;
    }

    bool same(long long x, long long y) {
        return root(x) == root(y);
    }

    long long size(long long x) {
        return siz[root(x)];
    }
};

struct edge {
   int from, to;
   LL cost;
   int num;
   edge(int f, int t, LL c, int num) : from(f), to(t), cost(c), num(num) {}

   bool operator< (const edge& e) const {
       return cost < e.cost;
   }
};

vector<int> connected;
vector<int> P;

struct Kruskal {
    LL sum;
    vector<int> selected_edge;
    int connection;

    Kruskal(const vector<edge>& edge_cost, int N) {
        sum = 0;
        connection = 0;
        UnionFind uf(N);

        for(auto e : edge_cost) {
            if(!connected[e.from] || !connected[e.to]) continue;
            if(uf.same(e.from, e.to)) continue;
            uf.unite(e.from, e.to);
            sum += e.cost;
            selected_edge.emplace_back(e.num);
        }
        rep(i,N) {
            if(uf.same(0, i)) connection++;
        }
    }
};

random_device rnd;
mt19937 engine(rnd());

int N, M, K;
vector<pii> bases;
vector<vector<edge>> G;
vector<pii> H;

vector<edge> edges;
vector<edge> edge_cost;

vvi bases_dist;
vvi all_dist;
vector<vector<pii>> near_bases;

LL score_check(vector<int>& P, vector<int>& selected_edge) {
    LL S = 0;
    rep(i,N) S += P[i] * P[i];

    UnionFind uf(N);
    for(auto num : selected_edge) {
        edge e = edges[num];
        uf.unite(e.from, e.to);
        S += e.cost;
    }

    vector<int> reached(K);
    int cnt = 0;
    rep(i,N) {
        if(!uf.same(0, i)) continue;
        rep(j,K) {
            if(reached[j]) continue;
            int dx = bases[i].first - H[j].first;
            int dy = bases[i].second - H[j].second;
            int dist = dx * dx + dy * dy;
            if(dist <= P[i] * P[i]) {
                reached[j] = 1;
                cnt++;
            }
        }
    }

    if(cnt == K) {
        return round(1e6 * (1.0 + 1e8 / (1e7 + S)));
    }
    else {
        return round(1e6 * (double)(cnt + 1) / K);
    }
}

LL calc_score(int reached, LL total_cost) {
    if(reached == K) {
        return round(1e6 * (1.0 + 1e8 / (1e7 + total_cost)));
    }
    else {
        return round(1e6 * (double)(reached + 1) / K);
    }   
}

pair<int,LL> decide_power() {
    int reached = 0;
    LL total_cost = 0;

    rep(i,N) P[i] = 0;
    rep(j,K) {
        int min_cost = INF;
        int base_number = -1;
        int now_power = 0;

        for(auto [dist, i] : near_bases[j]) {
            if(!connected[i]) continue;
            int power = sqrt((double)dist) + 1;
            if(power > 5000) break;

            int cost_dif = power * power - P[i] * P[i];
            if(cost_dif <= 0) {
                base_number = i;
                now_power = P[i];
                min_cost = 0;
                break;
            }
            if(cost_dif < min_cost) {
                min_cost = cost_dif;
                base_number = i;
                now_power = power;
            }
        }

        if(base_number != -1) {
            reached++;
            total_cost += min_cost;
            P[base_number] = now_power;
        }
    }
    return make_pair(reached, total_cost);
}

std::uniform_real_distribution<> randP(0.0, 1.0);
std::uniform_int_distribution<> cent(1, 100);

Kruskal edge_select() {
    Kruskal K(edge_cost, N);
    return K;
}

vector<int> selected_edge;
LL final_score;
int SimulatedAnnealing(double T0, double T1) {
    double time = 0.0, temp = T0;
    int update_number = 0;
    uniform_int_distribution<> select_base(1, N - 1);
    decide_power();
    selected_edge = edge_select().selected_edge;
    LL now_score = score_check(P, selected_edge);
    int total_base = N;

    for(int turn = 0; ; turn++){
        if(turn % 100 == 0){
            time = 1.0 * clock() / CLOCKS_PER_SEC;
            temp = pow(T0, 1.0 - time / TL) * pow(T1, time / TL);
        }
        if(time > TL){
            if(now_file_status != file_status::submit) {
                cerr << "turn = " << turn << endl;
            }
            break;
        }
        turn++;

        int change_size = 1;
        vector<int> base_num;
        rep(i,change_size) {
            int num = select_base(engine);
            total_base += 1 - connected[num] * 2;
            connected[num] ^= 1;
            base_num.emplace_back(num);
        }

        auto new_selected_edge = edge_select();
        if(new_selected_edge.connection != total_base) {
            for(auto num : base_num) {
                connected[num] ^= 1;
                total_base -= 1 - connected[num] * 2;
            }
            continue;
        }
        LL total_cost = new_selected_edge.sum;

        auto res = decide_power();
        int reached = res.first;
        total_cost += res.second;
        LL new_score = calc_score(reached, total_cost);

        double poss = 1.0;
        long long diff = new_score - now_score;
        if(diff < 0) poss = exp(diff / temp);
        // 更新しない場合
        if(poss < randP(engine)){
            for(auto num : base_num) {
                connected[num] ^= 1;
                total_base -= 1 - connected[num] * 2;
            }
        }
        // 更新する場合
        else {
            now_score = new_score;
            selected_edge = new_selected_edge.selected_edge;
            update_number++;

            final_score = now_score;
        }
    }

    return update_number;
}

void output(vector<int>& P, vector<int>& selected_edge) {
    rep(i,N-1) cout << P[i] << " ";
    cout << P[N-1] << endl;
    vector<int> B(M);
    for(auto num : selected_edge) B[num] = 1;
    rep(i,M-1) cout << B[i] << " ";
    cout << B[M-1] << endl;
}

int main(){
    if(now_file_status != file_status::submit){
        read_input();
        file_output();
    }

    cin >> N >> M >> K;
    rep(i,N) {
        int x, y;
        cin >> x >> y;
        bases.emplace_back(x, y);
    }
    G.resize(N);
    P.resize(N);
    connected.resize(N, 1);

    bases_dist.resize(N);
    all_dist.resize(N, vector<int>(K));
    near_bases.resize(K);
    rep(i,M) {
        int u, v;
        LL w;
        cin >> u >> v >> w;
        u--; v--;
        G[u].emplace_back(u, v, w, i);
        G[v].emplace_back(v, u, w, i);
        edges.emplace_back(u, v, w, i);
        edge_cost.emplace_back(u, v, w, i);
    }
    sort(edge_cost.begin(), edge_cost.end());

    rep(i,K) {
        int a, b;
        cin >> a >> b;
        H.emplace_back(a, b);
    }

    //全頂点始点ダイクストラ
    rep(st,N) {
        vector<int> dist(N, INF);
        dist[st] = 0;
        priority_queue<pii,vector<pii>,greater<pii>> pq;
        pq.emplace(0, st);

        while(!pq.empty()) {
            pii node = pq.top(); pq.pop();
            int d = node.first, u = node.second;
            if(d != dist[u]) continue;

            for(auto e : G[u]) {
                int v = e.to;
                if(dist[v] <= dist[u] + e.cost) continue;
                dist[v] = dist[u] + e.cost;
                pq.emplace(dist[v], v);
            }
        }
        bases_dist[st] = dist;
    }

    // 基地と家の距離の二乗
    rep(i,N) {
        rep(j,K) {
            int dx = bases[i].first - H[j].first;
            int dy = bases[i].second - H[j].second;
            int dist = dx * dx + dy * dy;
            all_dist[i][j] = dist;

            near_bases[j].emplace_back(dist, i);
        }
    }
    rep(j,K) sort(near_bases[j].begin(), near_bases[j].end());

    int update_number = SimulatedAnnealing(1e7, 1e2);
    if(now_file_status != file_status::submit) {
        cerr << "update = " << update_number << endl;
    }
    decide_power();
    output(P, selected_edge);
    if(now_file_status != file_status::submit) {
        cerr << "score = " << final_score << endl;
    }

	return 0;
}
