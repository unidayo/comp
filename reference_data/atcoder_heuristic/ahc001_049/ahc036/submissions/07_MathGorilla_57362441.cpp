#include <bits/stdc++.h>
#include <atcoder/all>
#include <cstdlib>
#include <cmath>
#include <random>
#include <fstream>

#define rep(i, begin, end) for (int i = begin; i < end; i++)
#define repint(i, begin, end) for (int i = begin; i < end; i++)
#define drep(i, begin, end) for (int i = begin; i >= end; i--)
#define rep1toN(i, n) for (int i = 1; i <= (int)(n); i++)
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
using ll = long long;
// #define rep(i, begin, end) for (ll i = (ll)begin; i < (ll)(end); i++)
using si = __int16_t;
#define repsi(i, begin, end) for (si i = (si)begin; i < (si)end; i++)
// using ld = double;
using namespace std;
using namespace atcoder;
template<class T> using v = vector<T>;
template<class T> using vv = v<v<T>>;
template<class T> using priority_queue_rev = priority_queue<T, vector<T>, greater<T>>;
template<class T> inline bool chmin(T &a, const T b) { if (a > b) { a = b; return true; } return false; }
template<class T> inline bool chmax(T &a, const T b) { if (a < b) { a = b; return true; } return false; }
template<class T> istream &operator>>(istream &is, vector<T> &v) { for (auto &e : v) is >> e; return is; }
template<class T> ostream &operator<<(ostream &os, const vector<T> &v) { for (auto &e : v) os << e << ' '; return os; }
template<class T> inline void to_unique(vector<T> &a) { sort(all(a)); a.erase(unique(all(a)), a.end()); }
struct fast_ios { fast_ios(){ cin.tie(nullptr); ios::sync_with_stdio(false); }; } fast_ios_;

mt19937 rnd(0);
float normal_dist(float mu, float sigma){
    float r1 = rnd() / (rnd.max() + 1.0), r2 = rnd() / (rnd.max() + 1.0);
    float z = sqrt(-2 * log(r1)) * cos(2 * M_PI * r2);
    return mu + sigma * z;
}
// mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());
auto system_now = std::chrono::system_clock::now();
int check_time() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - system_now).count();
}
auto lap_time = std::chrono::system_clock::now();
void log_check_lap_time(string s) {
    cerr << s << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lap_time).count() << "ms" << endl;
    lap_time = std::chrono::system_clock::now();
}

#if DEBUG
ll time_limit = 4000;
#elif LOCAL
ll time_limit = 2200;
#else
ll time_limit = 3000-100;
#endif

const int MAX = 100000;
const int N = 600, T = 600;
int Ts[T];
int M, La, Lb;
int X[N], Y[N];
vv<int> G(N);
vv<int> GDist(N, v<int>(N, MAX));
int GoalCnt[N]; // GoalCnt[i] := 頂点iを目的地に設定している数
v<bool> unused_vert(N);
vv<int> Routes(T);

v<int> LaArray;
// [i] := i番目のキャッシュラインが含む頂点のリスト
vv<int> CLVerts;
// [i] := i番目のキャッシュラインのLa上での開始位置
v<int> CLBeginPosInLa;
// [i] := i番目のキャッシュラインの長さ
v<int> CLLen;
// [i][j] := 辺i->j を結ぶキャッシュラインの区間set. [1, 16), [34, 50) のような持ち方。
v<array<int,2>> EdgeCLSet[N][N];
// [i][j] := 頂点iから頂点jまでのキャッシュラインを使った最短距離
int DistMap[N][N];
// [i][j] := 頂点iから隣接する頂点jに移動する際の、読み込みキャッシュラインの長さの最小値
int MinLoadCLLength[N][N];
// [i][j] := MinLoadCLLengthを実現するキャッシュラインの代表
int MinLoadCL[N][N];

int LissajousCurveVisitedCost = 200;
float VisitedVertWeightCostPow = 1;
float LissajousSamplingRate = 2.3;

struct CacheArray{
    int VERT[N];
    int VERT2[N];
    int VERT3[N];
    int EDGE[N][N];
    int vert_idx, edge_idx;
    CacheArray(){
        rep(i, 0, N) VERT[i] = -1, VERT2[i] = -1, VERT3[i] = -1;
        rep(i, 0, N) rep(j, 0, N) EDGE[i][j] = -1;
        vert_idx = 0;
        edge_idx = 0;
    }

    void use(){ vert_idx++, edge_idx++; }

    // Lbと現在頂点を渡して、辿れる頂点リストを返す
    v<int> connected_verts(int cur, v<int>& cache){
        use();
        for(auto e : cache) if(e > 0) VERT[e] = vert_idx;
        v<int> res = {cur};
        queue<int> q; q.push(cur);
        VERT2[cur] = vert_idx;
        while(!q.empty()){
            int c = q.front(); q.pop();
            for(int nxt : G[c]){
                if(VERT2[nxt] == vert_idx) continue;
                if(VERT[nxt] < vert_idx) continue;
                res.push_back(nxt);
                q.push(nxt);
                VERT2[nxt] = vert_idx;
            }
        }
        return res;
    }
}cache_array;

struct Dijkstra{
    v<int> dist_map(vv<int>& g, v<int>& start){
        priority_queue_rev<array<int,2>> que;
        v<int> dist(g.size(), INT_MAX);
        for(auto s : start){
            dist[s] = 0;
            que.push({dist[s], s});
        }
        while(que.size()){
            auto [d, idx] = que.top(); que.pop();
            if(d > dist[idx]) continue;
            for(auto e : g[idx]){
                if(chmin(dist[e], d+1)) que.push({dist[e], e});
            }
        }
        return dist;
    }
    v<int> dist_map(vv<int>& g, int start){
        v<int> starts = {start};
        return dist_map(g, starts);
    }

    // 最短経路のパスを返す
    pair<bool, v<int>> shortest_path(vv<int>& g, v<int> starts, v<int> goals){
        v<int> dist = dist_map(g, starts);
        int goal = -1, min_dist = INT_MAX;
        for(auto g : goals) if(chmin(min_dist, dist[g])) goal = g;
        if(goal == -1) return {false, {}};

        v<int> res = {goal};
        cache_array.use();
        for(auto s : starts) cache_array.VERT[s] = cache_array.vert_idx;
        int cur = goal;
        while(cache_array.VERT[cur] < cache_array.vert_idx){
            for(auto nxt : g[cur]){
                if(dist[nxt] == dist[cur] - 1){
                    res.push_back(nxt);
                    cur = nxt;
                    break;
                }
            }
        }
        reverse(all(res));
        return {true, res};
    }

    pair<bool, v<int>> shortest_path(vv<int>& g, int start, int goal, v<int>& dist){
        if(dist[goal] == INT_MAX) return {false, {}};

        v<int> res = {goal};
        int cur = goal;
        while(cur != start){
            for(auto nxt : g[cur]){
                if(dist[nxt] == dist[cur] - 1){
                    res.push_back(nxt);
                    cur = nxt;
                    break;
                }
            }
        }
        reverse(all(res));
        return {true, res};
    }

    // 重み付きグラフに対する最短経路
    v<int> dist_map(vv<array<int,2>>& wg, v<int>& start){
        priority_queue_rev<array<int,2>> que;
        v<int> dist(wg.size(), INT_MAX);
        for(auto s : start){
            dist[s] = 0;
            que.push({dist[s], s});
        }
        while(que.size()){
            auto [d, idx] = que.top(); que.pop();
            if(d > dist[idx]) continue;
            for(auto [e, w] : wg[idx]){
                if(chmin(dist[e], d+w)) que.push({dist[e], e});
            }
        }
        return dist;
    }

    pair<bool, v<int>> shortest_path(vv<array<int,2>>& wg, int start, int goal){
        v<int> starts = {start};
        v<int> dist = dist_map(wg, starts);

        v<int> res = {goal};
        int cur = goal;
        while(cur != start){
            for(auto [nxt, cost] : wg[cur]){
                if(dist[nxt] + cost == dist[cur]){
                    res.push_back(nxt);
                    cur = nxt;
                    break;
                }
            }
        }
        reverse(all(res));
        return {true, res};
    }

}dijkstra;

struct VertexReducer{
    /**
     * 各目的地間の経路を求め、利用回数の少ない頂点から順に削除を試す
     * 削除した際の全経路長の総和が閾値以下なら削除 & Gを再構築
     * ↓
     * 最終的な各目的地間の経路を持っておく。
     */
    vv<int> G;
    vv<int> Routes = vv<int>(T);

    v<pair<int,int>> edges = v<pair<int,int>>();
    vv<int> vert_pair;
    v<int> use_cnt = v<int>(N);
    v<bool> removed = v<bool>(N);
    v<bool> use_fix = v<bool>(N); // この頂点は削除しない
    bool use_map[T][N];
    int accept_path_length_diff_threshold = 5;

    VertexReducer(vv<int> G): G(G){
        int cur = 0;
        for(auto nxt : Ts){
            vert_pair.push_back({cur, nxt});
            cur = nxt;
        }
        rep(i,0,N) for(auto e : G[i]) if(i < e) edges.push_back({i, e});
        rep(i,0,T) rep(j,0,N) use_map[i][j] = false;
    }

    void reduce(){
        rep(i,0,T) Routes[i] = dijkstra.shortest_path(G, {vert_pair[i][0]}, {vert_pair[i][1]}).second;
        rep(i,0,T) for(auto e : Routes[i]) use_map[i][e] = true;
        for(auto route : Routes) for(auto e : route) use_cnt[e]++;
        // use_fixの初期設定
        for(auto p : vert_pair) use_fix[p[0]] = use_fix[p[1]] = true;
        use_fix[0] = true;
        
        v<int> edge_len2(N);
        rep(i,0,N) for(auto e : G[i]) edge_len2[i] += pow((X[i] - X[e]) * (X[i] - X[e]) + (Y[i] - Y[e]) * (Y[i] - Y[e]), 1.0);

        rep(_,0,N){
            int reduce_vert = -1;
            // 利用回数が最も少ない頂点を削除しても全経路長が閾値以下なら削除
            // int min_use_cnt = INT_MAX;
            // rep(i,0,N) if(!removed[i] && !use_fix[i] && chmin(min_use_cnt, use_cnt[i])) reduce_vert = i;
            
            // 次数が小さい順
            // int min_deg = INT_MAX;
            // rep(i,0,N) if(!removed[i] && !use_fix[i] && chmin(min_deg, (int)G[i].size())) reduce_vert = i;

            // 合計の辺の長さが短い順
            int min_edge_length = INT_MAX;
            rep(i,0,N) if(!removed[i] && !use_fix[i]) if(chmin(min_edge_length, edge_len2[i])) reduce_vert = i;

            // 外側順
            // int max_xpy = 0;
            // rep(i,0,N) if(!removed[i] && !use_fix[i] && chmax(max_xpy, abs(X[i] - 500) + abs(Y[i] - 500))) reduce_vert = i;

            if(reduce_vert == -1) break;
            // reduce_vertを削除しても全経路長が閾値以下なら削除
            v<pair<int,int>> reduce_edges;
            for(auto e : G[reduce_vert]) reduce_edges.push_back({reduce_vert, e});
            G[reduce_vert].clear();
            for(auto [a, b] : reduce_edges) G[b].erase(remove(all(G[b]), reduce_vert), G[b].end());
            
            v<int> change_routes;
            vv<int> new_routes;
            int total_path_length_diff = 0;
            rep(i,0,T) if(use_map[i][reduce_vert]){
                change_routes.push_back(i);
                auto [can_travel, path] = dijkstra.shortest_path(G, {vert_pair[i][0]}, {vert_pair[i][1]});
                new_routes.push_back(path);
                total_path_length_diff += new_routes.back().size() - Routes[i].size() + (can_travel ? 0 : 10000);
                if(total_path_length_diff > accept_path_length_diff_threshold) break;
            }
            
            if(total_path_length_diff <= accept_path_length_diff_threshold){
                // 削除を採用. Routes, (edges), use_cnt, use_mapを更新
                rep(i,0,change_routes.size()){
                    int route_idx = change_routes[i];
                    for(auto e : Routes[route_idx]) use_map[route_idx][e] = false, use_cnt[e]--;
                    for(auto e : new_routes[i]) use_map[route_idx][e] = true, use_cnt[e]++;
                    Routes[route_idx] = new_routes[i];
                }
                removed[reduce_vert] = true;
            }else{
                // 削除を採用しない
                for(auto [a, b] : reduce_edges) G[a].push_back(b), G[b].push_back(a);
                use_fix[reduce_vert] = true;
            }
        }
        // 最終経路で未使用頂点が新たに出る場合があるので、それらを削除
        {
            v<pair<int,int>> new_edges;
            for(auto [a, b] : edges) if(use_cnt[a] > 0 && use_cnt[b] > 0) new_edges.push_back({a, b});
            edges = new_edges;
            rep(i,0,N) G[i].clear();
            for(auto [a, b] : new_edges) G[a].push_back(b), G[b].push_back(a);
            rep(i,0,N) removed[i] = use_cnt[i] == 0;
        }
        rep(i,0,N) if(!removed[i]) GDist[i] = dijkstra.dist_map(G, i);
    }
};

// リサージュ曲線に沿った経路でキャッシュラインを作成
struct LissajousCurveCacheLineCreator{
    vv<int> lines;
    vv<array<int,2>> weighted_g;
    v<array<int,2>> edges;
    int edge_used[N][N];
    int vert_used[N];
    bool fill_rest_LaArray = false;

    LissajousCurveCacheLineCreator(){
        lines = vv<int>();
        weighted_g = vv<array<int,2>>(N);
        edges = v<array<int,2>>();
        rep(i,0,N) for(auto e : G[i]) if(i < e) edges.push_back({i, e});
        for(auto [a, b] : edges){
            weighted_g[a].push_back({b, 1});
            weighted_g[b].push_back({a, 1});
        }
        rep(i,0,N) for(auto e : weighted_g[i]) edge_used[i][e[0]] = 0;
        rep(i,0,N) vert_used[i] = 0;
    }

    v<array<int,2>> lissajous_curve_points(float a, float b){
        v<array<int,2>> res;
        // 解像度
        float p = a * LissajousSamplingRate;
        rep(t,0,p){
            float tt = (float)t / p;
            float x = sin(2*M_PI*a*tt);
            float y = cos(2*M_PI*b*tt);
            // 0~1000に変換
            res.push_back({(int)(500 + 500*x), (int)(500 + 500*y)});
        }
        return res;
    }

    // v<array<int,2>> lissajous_curve_points(float a, float b){
    //     v<float> t;
    //     // 0,1/2a,2/2a,...,2a/2a,  0,1/2b,2/2b,...,2b/2b の値を求めて、sort
    //     rep(i,0,2*a) t.push_back((float)i / (2*a));
    //     rep(i,1,2*b-1) if(i != b) t.push_back((float)i / (2*b));
    //     sort(all(t));
        
    //     // TODO: 間が大きく開くときは、中間点を追加する
    //     //
    //     //

    //     v<array<int,2>> res;
    //     for(auto tt : t){
    //         float x = sin(2*M_PI*a*tt);
    //         float y = cos(2*M_PI*b*tt);
    //         // 0~1000に変換
    //         res.push_back({(int)(500 + 500*x), (int)(500 + 500*y)});
    //     }
    //     return res;
    // }

    void random_fill_rest_LaArray(v<int>& route, int rest){
        while(rest > 0){
            v<int> candidates;
            rep(i,0,N) if(!unused_vert[i]){
                int dx = X[i] - X[route.back()], dy = Y[i] - Y[route.back()];
                if(sqrtl(dx * dx + dy * dy) > 300) candidates.push_back(i);
            }
            if(candidates.size() == 0) return;
            int nxt = candidates[rand() % candidates.size()];
            auto path = dijkstra.shortest_path(G, {route.back()}, {nxt}).second;
            for(auto e : path){
                route.push_back(e);
                if(--rest == 0) return;
            }
        }
    }

    int get_min_dist_vert(int posX, int posY){
        int min_dist = INT_MAX, min_idx = -1;
        rep(i,0,N) if(!unused_vert[i]){
            // int dist = abs(X[i] - posX) + abs(Y[i] - posY) + vert_used[i] * LissajousCurveVisitedCost;
            int dx = X[i] - posX, dy = Y[i] - posY;
            int dist = sqrtl(dx * dx + dy * dy) + vert_used[i] * LissajousCurveVisitedCost;
            if(chmin(min_dist, dist)) min_idx = i;
        }
        return min_idx;
    }

    void update_weighted_g_with_vert_usage(v<int> path){
        v<int> last_popped_path(path.begin(), path.end()); last_popped_path.pop_back();
        for(auto e : last_popped_path) vert_used[e]++;
        set<int> update_verts;
        for(auto e : path) for(auto nxt : G[e]) update_verts.insert(nxt);
        for(auto e : update_verts){
            for(auto &nxt : weighted_g[e]){
                // TODO: 重み0.5の方が良いケースも割とあった
                nxt[1] = pow(1 + vert_used[nxt[0]] + vert_used[e], VisitedVertWeightCostPow);
            }
        }
    }

    void update_weighted_g_with_edge_usage(v<int> path){
        rep(i,0,path.size()-1){
            int a = path[i], b = path[i+1];
            edge_used[a][b]++;
            edge_used[b][a]++;
        }
        // 辺の重みを 1 + edge_used にする
        set<int> used_verts; for(auto e : path) used_verts.insert(e);
        for(auto e : used_verts){
            for(auto &nxt : weighted_g[e]){
                nxt[1] = (1 + edge_used[e][nxt[0]]) * (1 + edge_used[nxt[0]][e]);
            }
        }
    }

    v<int> create_lissajous_curve_route(float a, float b){
        auto points = lissajous_curve_points(a, b);
        auto cur = get_min_dist_vert(points[0][0], points[0][1]);
        v<int> res = {cur};
        rep(i,1,points.size()){
            res.pop_back();
            auto nxt = get_min_dist_vert(points[i][0], points[i][1]);
            auto [can_travel, path] = dijkstra.shortest_path(weighted_g, cur, nxt);
            for(auto e : path) res.push_back(e);
            cur = nxt;

            update_weighted_g_with_vert_usage(path);
        }
        return res;
    }

    vv<int> unvisited_verts_cachelines(v<bool> &visited){
        dsu uf(N);
        rep(i,0,N) for(auto e : G[i]) if(!visited[i] && !visited[e] && (uf.size(i) + uf.size(e) <= Lb)) uf.merge(i, e);
        vv<int> res;
        for(auto group : uf.groups()) if(!visited[group[0]] && !unused_vert[group[0]]){
            res.push_back(group);
        }
        return res;
    }

    v<int> drop_in_to_unvisited(v<int>& route){
        v<int> visited_cnt(N);
        for(auto e : route) visited_cnt[e]++;
        int necessary_verts = 0;
        rep(i,0,N) necessary_verts += visited_cnt[i] == 0 && !unused_vert[i];

        v<array<int,2>> drop_ins; // drop_ins[i] := route[i]からroute[i+1]に行く途中で、未到達のゴールに寄り道する
        v<int> perm(route.size());
        iota(all(perm), 0);
        shuffle(all(perm), rnd);

        for(auto i : perm){
            int a = route[i];
            // for(auto goal : G[a]) if(GoalCnt[goal] > 0 && visited_cnt[goal] == 0){
            for(auto goal : G[a]) if(visited_cnt[goal] == 0 && !unused_vert[goal]){
                drop_ins.push_back({i, goal});
                visited_cnt[goal]++;
                // visited_cnt[a]++;
                necessary_verts--;
                break;
            }
        }
        sort(all(drop_ins));

        v<int> res;
        int drop_in_idx = 0;
        rep(i,0,route.size()){
            res.push_back(route[i]);
            while(drop_in_idx < drop_ins.size() && i == drop_ins[drop_in_idx][0]){
                res.push_back(drop_ins[drop_in_idx][1]);
                // res.push_back(route[i]);
                drop_in_idx++;
            }
        }
        // cerr << "added goal: " << res.size() - route.size() << endl;

        while(res.size() + necessary_verts > La){
            auto last = res.back();
            res.pop_back();
            visited_cnt[last]--;
            if(visited_cnt[last] == 0) necessary_verts++;
        }
        // cerr << "after drop in, necessary_verts: " << necessary_verts << endl;
        return res;
    }

    // routeの途中で、未到達のゴールに寄り道する
    v<int> drop_in_to_goal(v<int>& route){
        v<int> visited_cnt(N);
        for(auto e : route) visited_cnt[e]++;
        int necessary_verts = 0;
        rep(i,0,N) necessary_verts += visited_cnt[i] == 0 && !unused_vert[i];

        v<array<int,2>> drop_ins; // drop_ins[i] := route[i]からroute[i+1]に行く途中で、未到達のゴールに寄り道する
        v<int> perm(route.size());
        iota(all(perm), 0);
        shuffle(all(perm), rnd);

        for(auto i : perm){
            int a = route[i];
            for(auto goal : G[a]) if(GoalCnt[goal] > 0 && visited_cnt[goal] == 0){
            // for(auto goal : G[a]) if(visited_cnt[goal] == 0 && !unused_vert[goal]){
                drop_ins.push_back({i, goal});
                visited_cnt[goal]++;
                // visited_cnt[a]++;
                necessary_verts--;
                break;
            }
        }
        sort(all(drop_ins));

        v<int> res;
        int drop_in_idx = 0;
        rep(i,0,route.size()){
            res.push_back(route[i]);
            while(drop_in_idx < drop_ins.size() && i == drop_ins[drop_in_idx][0]){
                res.push_back(drop_ins[drop_in_idx][1]);
                // res.push_back(route[i]);
                drop_in_idx++;
            }
        }
        // cerr << "added goal: " << res.size() - route.size() << endl;

        while(res.size() + necessary_verts > La){
            auto last = res.back();
            res.pop_back();
            visited_cnt[last]--;
            if(visited_cnt[last] == 0) necessary_verts++;
        }
        // cerr << "after drop in, necessary_verts: " << necessary_verts << endl;
        return res;
    }

    // 孤立のゴール側から、ルート内の隣接頂点にconnectしていく方針
    v<int> drop_in_to_goal_from_isolated_goal(v<int>& route){
        v<int> visited_cnt(N);
        for(auto e : route) visited_cnt[e]++;
        int necessary_verts = 0;
        rep(i,0,N) necessary_verts += visited_cnt[i] == 0 && !unused_vert[i];
        
        
        vv<int> vert_idxs_in_route(N);
        rep(i,0,route.size()) vert_idxs_in_route[route[i]].push_back(i);
        v<int> unvisited_goals;
        for(auto goal : Ts) if(visited_cnt[goal] == 0) unvisited_goals.push_back(goal);
        to_unique(unvisited_goals);

        v<array<int,2>> drop_ins; // drop_ins[i] := route[i]からroute[i+1]に行く途中で、未到達のゴールに寄り道する
        for(auto goal : unvisited_goals){
            v<int> connected_verts_in_route;
            for(auto e : G[goal]) for(auto idx : vert_idxs_in_route[e]) connected_verts_in_route.push_back(idx);
            if(connected_verts_in_route.size() == 0) continue;
            to_unique(connected_verts_in_route);
            drop_ins.push_back({connected_verts_in_route[rand() % connected_verts_in_route.size()], goal});
            visited_cnt[goal]++;
            necessary_verts--;
        }
        sort(all(drop_ins));

        v<int> res;
        int drop_in_idx = 0;
        rep(i,0,route.size()){
            res.push_back(route[i]);
            while(drop_in_idx < drop_ins.size() && i == drop_ins[drop_in_idx][0]){
                res.push_back(drop_ins[drop_in_idx][1]);
                drop_in_idx++;
            }
        }

        while(res.size() + necessary_verts > La){
            auto last = res.back();
            res.pop_back();
            visited_cnt[last]--;
            if(visited_cnt[last] == 0) necessary_verts++;
        }
        return res;
    }

    float a = 2, b = 3;
    void run(){
        int min_nessesary_verts = MAX, min_a = -1, min_b = -1, min_unvisited_goals = MAX;
        v<int> best_route;
        rep(loop,0,1){
            v<int> route = create_lissajous_curve_route(a, b);
            rep(t,0,5){
                // 平滑化
                v<int> smoothed_route = {route[0]};
                rep(i,1,route.size()-1){
                    int a = route[i-1], b = route[i], c = route[i+1];
                    if(a == c){
                        i++;
                        continue;
                    }else{
                        smoothed_route.push_back(b);
                    }
                }
                route = smoothed_route;
            }
            // rep(t,0,6) route = drop_in_to_goal(route);
            rep(t,0,6) route = drop_in_to_unvisited(route);            

            v<int> visited_cnt(N);
            for(auto e : route) visited_cnt[e]++;
            int necessary_verts = 0;
            rep(i,0,N) necessary_verts += visited_cnt[i] == 0 && !unused_vert[i];

            bool over_len = route.size() + necessary_verts > La;
            while(route.size() + necessary_verts > La){
                auto last = route.back();
                route.pop_back();
                visited_cnt[last]--;
                if(visited_cnt[last] == 0) necessary_verts++;
            }

            int unvisited_goals = 0;
            for(auto goal : Ts) if(visited_cnt[goal] == 0) unvisited_goals++;

            // cerr << "リサージュ" << a << " " << b << "での非到達点数: " << necessary_verts << " 未到達ゴール数: " << unvisited_goals << " over_len: " << over_len << endl;
            if(chmin(min_nessesary_verts, necessary_verts)){
                best_route = route, min_a = a, min_b = b, min_unvisited_goals = unvisited_goals;
            }
        }
        cerr << "min_a: " << min_a << " min_b: " << min_b << " 非到達点数: " << min_nessesary_verts << " 未到達ゴール数: " << min_unvisited_goals;
        if(min_nessesary_verts + best_route.size() < La) cerr << "     La余り: " << La - min_nessesary_verts - best_route.size();
        cerr << endl;
        if(fill_rest_LaArray){
            random_fill_rest_LaArray(best_route, La - min_nessesary_verts - best_route.size());
        }
        v<int> visited_cnt(N);
        for(auto e : best_route) visited_cnt[e]++;
        map<int,int> visited_cnt_map;
        for(auto e : visited_cnt) visited_cnt_map[e]++;

        // best_route = drop_in_to_goal(best_route);
        lines.push_back(best_route);
        v<bool> visited(N);
        for(auto e : best_route) visited[e] = true;
        for(auto e : unvisited_verts_cachelines(visited)) lines.push_back(e);
    }
};

struct CacheLineSplitter{
    void register_edge_cl_set(v<int> &clverts, int cl_idx, int not_merge_edge_cl_set){
        // clvertsに隣接する頂点集合を取得
        set<int> adj_v;
        for(auto e : clverts) adj_v.insert(e);
        for(auto e : clverts) for(auto nxt : G[e]) adj_v.insert(nxt);

        
        // 隣接頂点から、clverts内の各点に対して、辺を構築
        for(auto from : adj_v){
            for(auto to : clverts){
                if(EdgeCLSet[from][to].size() == 0) EdgeCLSet[from][to].push_back({cl_idx, cl_idx+1});
                else{
                    int last_idx = EdgeCLSet[from][to].back()[1];
                    if(last_idx == not_merge_edge_cl_set || last_idx != cl_idx) EdgeCLSet[from][to].push_back({cl_idx, cl_idx+1});
                    else EdgeCLSet[from][to].back()[1]++;
                }
            }
        }
    }

    void register_short_edge_cl_set(v<int> &clverts, int cl_idx){
        // clvertsに隣接する頂点集合を取得
        set<int> adj_v;
        for(auto e : clverts) adj_v.insert(e);
        for(auto e : clverts) for(auto nxt : G[e]) adj_v.insert(nxt);

        // 隣接頂点から、clverts内の各点に対して、辺を構築
        for(auto from : adj_v){
            for(auto to : clverts){
                MinLoadCLLength[from][to] = clverts.size();
                MinLoadCL[from][to] = cl_idx;
            }
        }
    }

    void update_dist_map(){
        rep(i,0,N) rep(j,0,N) DistMap[i][j] = MAX;
        rep(i,0,N)rep(j,0,N) if(EdgeCLSet[i][j].size() > 0) DistMap[i][j] = 1;
        rep(i,0,N) DistMap[i][i] = 0;
        v<int> used_verts; rep(i,0,N) if(!unused_vert[i]) used_verts.push_back(i);
        int usedN = used_verts.size();
        rep(k,0,usedN) rep(i,0,usedN) rep(j,0,usedN) chmin(DistMap[used_verts[i]][used_verts[j]], DistMap[used_verts[i]][used_verts[k]] + DistMap[used_verts[k]][used_verts[j]]);
    }

    bool is_connect(v<int>& line){
        cache_array.use();
        for(auto e : line) cache_array.VERT[e] = cache_array.vert_idx;
        cache_array.VERT2[line[0]] = cache_array.vert_idx;
        int cnt = 1;
        queue<int> q; q.push(line[0]);
        while(!q.empty()){
            int cur = q.front(); q.pop();
            for(auto nxt : G[cur]){
                if(cache_array.VERT[nxt] != cache_array.vert_idx) continue;
                if(cache_array.VERT2[nxt] == cache_array.vert_idx) continue;
                cnt++;
                cache_array.VERT2[nxt] = cache_array.vert_idx;
                q.push(nxt);
            }
        }

        for(auto e : line) if(cache_array.VERT3[e] != cache_array.vert_idx) cnt--, cache_array.VERT3[e] = cache_array.vert_idx;
        return cnt == 0;
    }

    void run(vv<int> &lines){
        rep(i,0,N) rep(j,0,N) MinLoadCLLength[i][j] = MAX, MinLoadCL[i][j] = -1;

        int cur_cl_idx = 0;
        int la_offset = 0;
        int not_merge_edge_cl_set = -1;
        for(auto line : lines){
            // rep(begin,0,max(1, (int)line.size()-Lb+1)){
            rep(begin,0,(int)line.size()){
                v<int> cut;
                rep(i,0,min(Lb, (int)line.size()-begin)) cut.push_back(line[begin+i]);
                if(!is_connect(cut)) continue;

                CLVerts.push_back(cut);
                CLBeginPosInLa.push_back(la_offset + begin);
                CLLen.push_back(cut.size());
                register_edge_cl_set(cut, cur_cl_idx, not_merge_edge_cl_set);
                cur_cl_idx++;
            }
            la_offset += line.size();
            not_merge_edge_cl_set = cur_cl_idx;
        }
        
        // lines[0]を各長さで登録する
        auto long_line = lines[0];
        drep(len,Lb-1,1){
            rep(begin,0,(int)long_line.size()-len+1){
                v<int> cut;
                rep(i,0,len) cut.push_back(long_line[begin+i]);
                if(!is_connect(cut)) continue;

                CLVerts.push_back(cut);
                CLBeginPosInLa.push_back(begin);
                CLLen.push_back(len);
                register_short_edge_cl_set(cut, cur_cl_idx);
                cur_cl_idx++;
            }
        }

        rep(i,0,N)rep(j,0,N) if(MinLoadCLLength[i][j] < MAX) EdgeCLSet[i][j].push_back({MinLoadCL[i][j], MinLoadCL[i][j]+1});
        
        update_dist_map();
        return;
    }
};

struct LoadOperation{
    int cl_idx, load_begin_pos, load_len, lb_pos;
    LoadOperation(int cl_idx, int load_begin_pos, int load_len, int lb_pos): cl_idx(cl_idx), load_begin_pos(load_begin_pos), load_len(load_len), lb_pos(lb_pos){}
};

struct SimpleRouteSolver{
    struct ReservedOperation{
        int prevprev_vert; // 2手前に到達すべき頂点
        int prev_vert; // 1手前に到達すべき頂点
        int prev_cl_idx; // 1手前にロードすべきキャッシュライン
        LoadOperation dest_op; // 目的地に到達する際のLoadOperation

        ReservedOperation(int prevprev_vert, int prev_vert, int prev_cl_idx, LoadOperation dest_op): prevprev_vert(prevprev_vert), prev_vert(prev_vert), prev_cl_idx(prev_cl_idx), dest_op(dest_op){}
        ReservedOperation(): prevprev_vert(-1), prev_vert(-1), prev_cl_idx(-1), dest_op(LoadOperation(-1, -1, -1, -1)){}
    };

    v<int> memory;
    int cur_pos;
    v<pair<LoadOperation, int>> ans;
    int score = 0;

    v<int> get_connect_verts(v<int>& verts, int cur){
        v<int> res;
        cache_array.use();
        for(auto e : verts) if(e >= 0) cache_array.VERT[e] = cache_array.vert_idx;
        cache_array.VERT2[cur] = cache_array.vert_idx;
        res.push_back(cur);
        queue<int> q; q.push(cur);
        while(!q.empty()){
            int cur = q.front(); q.pop();
            for(auto nxt : G[cur]){
                if(cache_array.VERT[nxt] != cache_array.vert_idx) continue;
                if(cache_array.VERT2[nxt] == cache_array.vert_idx) continue;
                res.push_back(nxt);
                cache_array.VERT2[nxt] = cache_array.vert_idx;
                q.push(nxt);
            }
        }
        return res;
    }
    v<int> current_available_verts(){
        // cur_posから辿れる頂点を返す
        v<int> verts;
        cache_array.use();
        for(auto e : memory) if(e >= 0) cache_array.VERT[e] = cache_array.vert_idx;

        function<void(int)> dfs = [&](int cur){
            verts.push_back(cur);
            cache_array.VERT2[cur] = cache_array.vert_idx;
            for(auto nxt : G[cur]){
                if(cache_array.VERT[nxt] != cache_array.vert_idx) continue;
                if(cache_array.VERT2[nxt] == cache_array.vert_idx) continue;
                dfs(nxt);
            }
        };
        dfs(cur_pos);
        return verts;
    }

    v<array<int,2>> merge_cl_sections(v<array<int,2>> &cl_sections){
        v<array<int,2>> res;
        sort(all(cl_sections));
        for(auto [l, r] : cl_sections){
            if(res.size() == 0 || res.back()[1] < l) res.push_back({l, r});
            else res.back()[1] = r;
        }
        return res;
    }

    ReservedOperation search_best_arrival_on_dist1(int ts_idx, int dest, v<int> start_candidates){
        ReservedOperation best_res;
        // <次に0手で到達可能な目的地の数, -初めて0手で到達不可の際のdist, -初めて０手で到達不可の際の"元のG上での距離"> 。大きいほど良い
        array<int,3> best_score = {0, -100, -100};
        
        // 目的地に行くCLをLbの長さでLoadするケース
        for(auto prev : start_candidates){
            auto& edge_sections = EdgeCLSet[prev][dest];
            for(auto [l, r] : edge_sections){
                rep(i,l,r){
                    // キャッシュラインiをロードした時の次以降のts_idxに到達するための最短経路を考える
                    auto& last_verts = CLVerts[i];
                    array<int,3> score = {0, -100, -100};
                    for(auto last : last_verts){
                        int zero_arrival_cnt = 0, last_dist = -1, last_gdist = -1;
                        int nxt_ts_idx = ts_idx + 1;
                        while(nxt_ts_idx < T){
                            if(DistMap[last][Ts[nxt_ts_idx]] == 0) zero_arrival_cnt++, nxt_ts_idx++;
                            else{
                                last_dist = DistMap[last][Ts[nxt_ts_idx]];
                                last_gdist = GDist[last][Ts[nxt_ts_idx]];
                                break;
                            }
                        }
                        score = {zero_arrival_cnt, -last_dist, -last_gdist};
                        if(chmax(best_score, score)) best_res = ReservedOperation(prev, prev, -1, LoadOperation(i, 0, CLLen[i], 0));
                    }
                }
            }
        }

        // 目的地に行くCLを短い長さでLoadして、prevloadを残すケース
        for(auto prev : start_candidates){
            // prevからdestに行くロードの中で最短のものを取得
            auto cl_len = MinLoadCLLength[prev][dest];
            if(cl_len == MAX) continue;
            // 現在のロード状況において、先頭 or 末尾にロードして、prevに戻ってこられるか
            int prev_vert_idx = -1;
            rep(i,0,memory.size()) if(memory[i] == prev) {prev_vert_idx = i; break;}
            if(prev_vert_idx >= cl_len){
                // 先頭にロードして、後ろに残ったmemory分で、prevとconnectしてるものを取得
                v<int> rest_memory(memory.begin() + cl_len, memory.end());
                auto connect_verts = get_connect_verts(rest_memory, prev);

                array<int,3> score = {0, -100, -100};
                for(auto last : connect_verts){
                    int zero_arrival_cnt = 0, last_dist = -1, last_gdist = -1;
                    int nxt_ts_idx = ts_idx + 1;
                    while(nxt_ts_idx < T){
                        if(DistMap[last][Ts[nxt_ts_idx]] == 0) zero_arrival_cnt++, nxt_ts_idx++;
                        else{
                            last_dist = DistMap[last][Ts[nxt_ts_idx]];
                            last_gdist = GDist[last][Ts[nxt_ts_idx]];
                            break;
                        }
                    }
                    score = {zero_arrival_cnt, -last_dist, -last_gdist};
                    if(chmax(best_score, score)) best_res = ReservedOperation(prev, prev, -1, LoadOperation(MinLoadCL[prev][dest], 0, cl_len, 0));
                }
            }
            if(prev_vert_idx < Lb - cl_len){
                // 末尾にロード
                v<int> rest_memory(memory.begin(), memory.begin() + prev_vert_idx+1);
                auto connect_verts = get_connect_verts(rest_memory, prev);

                array<int,3> score = {0, -100, -100};
                for(auto last : connect_verts){
                    int zero_arrival_cnt = 0, last_dist = -1, last_gdist = -1;
                    int nxt_ts_idx = ts_idx + 1;
                    while(nxt_ts_idx < T){
                        if(DistMap[last][Ts[nxt_ts_idx]] == 0) zero_arrival_cnt++, nxt_ts_idx++;
                        else{
                            last_dist = DistMap[last][Ts[nxt_ts_idx]];
                            last_gdist = GDist[last][Ts[nxt_ts_idx]];
                            break;
                        }
                    }
                    score = {zero_arrival_cnt, -last_dist, -last_gdist};
                    if(chmax(best_score, score)){
                        best_res = ReservedOperation(prev, prev, -1, LoadOperation(MinLoadCL[prev][dest], 0, cl_len, Lb - cl_len));
                    }
                }
            }
        }
        return best_res;
    }

    ReservedOperation search_best_arrival2(int ts_idx){
        auto dest = Ts[ts_idx];
        auto verts = current_available_verts();
        int min_dist = INT_MAX;
        v<int> start_candidates;
        for(auto e : verts){
            if(chmin(min_dist, DistMap[e][dest])) start_candidates = {e};
            else if(min_dist == DistMap[e][dest]) start_candidates.push_back(e);
        }
        // 0手移動なら、何もOperationを行わずに到達
        if(min_dist == 0) return ReservedOperation(start_candidates[0], start_candidates[0], -1, LoadOperation(-1, -1, -1, -1));

        if(min_dist == 1) return search_best_arrival_on_dist1(ts_idx, dest, start_candidates);

        vv<int> dist_map(3); // dist_map[i] := destへの距離がiの頂点集合
        rep(i,0,N){
            int dist = DistMap[i][dest];
            if(dist > 2) continue;
            auto distViaWaypoint = MAX;
            for(auto start : start_candidates) chmin(distViaWaypoint, DistMap[start][i] + DistMap[i][dest]);
            if(distViaWaypoint == min_dist) dist_map[dist].push_back(i);
        }

        ReservedOperation best_res;
        // <次に0手で到達可能な目的地の数, -初めて0手で到達不可の際のdist, -初めて０手で到達不可の際の"元のG上での距離"> 。大きいほど良い
        array<int,3> best_score = {0, -100, -100};
        
        // 目的地に行くCLをLbの長さでLoadするケース
        for(auto prev : dist_map[1]){
            auto& edge_sections = EdgeCLSet[prev][dest];
            for(auto [l, r] : edge_sections){
                rep(i,l,r){
                    // キャッシュラインiをロードした時の次以降のts_idxに到達するための最短経路を考える
                    auto& last_verts = CLVerts[i];
                    array<int,3> score = {0, -100, -100};
                    for(auto last : last_verts){
                        int zero_arrival_cnt = 0, last_dist = -1, last_gdist = -1;
                        int nxt_ts_idx = ts_idx + 1;
                        while(nxt_ts_idx < T){
                            if(DistMap[last][Ts[nxt_ts_idx]] == 0) zero_arrival_cnt++, nxt_ts_idx++;
                            else{
                                last_dist = DistMap[last][Ts[nxt_ts_idx]];
                                last_gdist = GDist[last][Ts[nxt_ts_idx]];
                                break;
                            }
                        }
                        score = {zero_arrival_cnt, -last_dist, -last_gdist};
                        if(chmax(best_score, score)) best_res = ReservedOperation(-1, prev, -1, LoadOperation(i, 0, CLLen[i], 0));
                    }
                }
            }
        }

        // 目的地に行くCLを短い長さでLoadして、prevloadを残すケース
        for(auto prev : dist_map[1]){
            v<array<int,2>> cl_sections; // prevに移動する際に、ロード可能なキャッシュラインの範囲
            for(auto prevprev : dist_map[2]) for(auto [l, r] : EdgeCLSet[prevprev][prev]) cl_sections.push_back({l, r});
            cl_sections = merge_cl_sections(cl_sections);
            int last_load_len = MinLoadCLLength[prev][dest];
            for(auto [l, r] : cl_sections){
                rep(cl_idx, l, r){
                    // cl_idxをロードして、prevに移動した状態。ここからlast_load_lenのキャッシュラインをロードしてdestに移動する。余った部分から次以降の目的地への移動コストを考える
                    auto& prev_verts = CLVerts[cl_idx];
                    int prev_vert_idx = -1;

                    rep(i,0,prev_verts.size()) if(prev_verts[i] == prev) {prev_vert_idx = i; break;}
                    // 前半部分にlastをロードしてprevが上書きされなければ、そこからscore計算
                    if(prev_vert_idx >= last_load_len){
                        v<int> rest_memory(prev_verts.begin() + last_load_len, prev_verts.end());
                        auto connect_verts = get_connect_verts(rest_memory, prev);
                        
                        for(auto last_vert : connect_verts){
                            int zero_arrival_cnt = 0, last_dist = -1, last_gdist = -1;
                            int nxt_ts_idx = ts_idx + 1;
                            while(nxt_ts_idx < T){
                                if(DistMap[last_vert][Ts[nxt_ts_idx]] == 0) zero_arrival_cnt++, nxt_ts_idx++;
                                else{
                                    last_dist = DistMap[last_vert][Ts[nxt_ts_idx]];
                                    last_gdist = GDist[last_vert][Ts[nxt_ts_idx]];
                                    break;
                                }
                            }
                            array<int,3> score = {zero_arrival_cnt, -last_dist, -last_gdist};
                            if(chmax(best_score, score)){
                                auto load_cl_idx = MinLoadCL[prev][dest];
                                best_res = ReservedOperation(-1, prev, cl_idx, LoadOperation(load_cl_idx, 0, last_load_len, 0));
                            }
                        }
                    }
                    if(prev_vert_idx < int(prev_verts.size()) - last_load_len){
                        // 後半部分にlastをロードしてprevが上書きされなければ、そこからscore計算
                        v<int> rest_memory(prev_verts.begin(), prev_verts.begin() + prev_vert_idx+1);
                        auto connect_verts = get_connect_verts(rest_memory, prev);
                        for(auto last_vert : connect_verts){
                            int zero_arrival_cnt = 0, last_dist = -1, last_gdist = -1;
                            int nxt_ts_idx = ts_idx + 1;
                            while(nxt_ts_idx < T){
                                if(DistMap[last_vert][Ts[nxt_ts_idx]] == 0) zero_arrival_cnt++, nxt_ts_idx++;
                                else{
                                    last_dist = DistMap[last_vert][Ts[nxt_ts_idx]];
                                    last_gdist = GDist[last_vert][Ts[nxt_ts_idx]];
                                    break;
                                }
                            }
                            array<int,3> score = {zero_arrival_cnt, -last_dist, -last_gdist};
                            if(chmax(best_score, score)){
                                auto load_cl_idx = MinLoadCL[prev][dest];
                                best_res = ReservedOperation(-1, prev, cl_idx, LoadOperation(load_cl_idx, 0, last_load_len, Lb - last_load_len));
                            }
                        }
                    }
                }
            }
        }

        // cerr << best_res.prevprev_vert << " " << best_res.prev_vert << " " << best_res.prev_cl_idx << " " << best_res.dest_op.cl_idx << " " << best_res.dest_op.load_begin_pos << " " << best_res.dest_op.load_len << " " << best_res.dest_op.lb_pos << endl;
        // prevprevを求める
        for(auto prevprev : dist_map[2]){
            bool has_cl = false;
            for(auto [l, r] : EdgeCLSet[prevprev][best_res.prev_vert]){
                // [l, r) にbest_res.prev_cl_idxが含まれるか
                if(best_res.prev_cl_idx == -1 || (l <= best_res.prev_cl_idx && best_res.prev_cl_idx < r)){
                    has_cl = true;
                    break;
                }
            }
            if(has_cl){
                best_res.prevprev_vert = prevprev;
                break;
            }
        }
        if(best_res.prevprev_vert == -1){
            cerr << "prevprev not found" << endl;
            exit(1);
        }
        return best_res;
    }

    // returns: <到達1手前の頂点idx, 目的地に到達する際にロードするキャッシュラインidx>
    array<int,2> search_best_arrival(int ts_idx){
        auto dest = Ts[ts_idx];
        // 現在のロード状況でdestに一番近い頂点に移動
        auto verts = current_available_verts();
        int min_dist = INT_MAX;
        v<int> min_idxs;
        for(auto e : verts){
            if(chmin(min_dist, DistMap[e][dest])) min_idxs = {e};
            else if(min_dist == DistMap[e][dest]) min_idxs.push_back(e);
        }
        if(min_dist == 0) return {dest, -1};

        v<int> available_prevs;
        rep(i,0,N) if(DistMap[i][dest] == 1){
            for(auto start : min_idxs) if(DistMap[start][i] == min_dist - 1){
                available_prevs.push_back(i);
                break;
            }
        }

        // min_idxsの中から、次以降のts_idxに到達するための最短経路を考えて、その経路を追加
        // <次に0手で到達可能な目的地の数, -初めて0手で到達不可の際のdist, -初めて０手で到達不可の際の"元のG上での距離"> 。大きいほど良い
        array<int,3> best_score = {0, -100, -100};
        int best_prev_idx = -1, best_cl_idx = -1;
        for(auto prev : available_prevs){
            auto& edge_sections = EdgeCLSet[prev][dest];
            for(auto [l, r] : edge_sections){
                rep(i,l,r){
                    // キャッシュラインiをロードした時の次以降のts_idxに到達するための最短経路を考える
                    auto& last_verts = CLVerts[i];
                    array<int,3> score = {0, -100, -100};
                    for(auto last : last_verts){
                        int zero_arrival_cnt = 0, last_dist = -1, last_gdist = -1;
                        int nxt_ts_idx = ts_idx + 1;
                        while(nxt_ts_idx < T){
                            if(DistMap[last][Ts[nxt_ts_idx]] == 0) zero_arrival_cnt++, nxt_ts_idx++;
                            else{
                                last_dist = DistMap[last][Ts[nxt_ts_idx]];
                                last_gdist = GDist[last][Ts[nxt_ts_idx]];
                                break;
                            }
                        }
                        score = {zero_arrival_cnt, -last_dist, -last_gdist};
                        if(chmax(best_score, score)) best_prev_idx = prev, best_cl_idx = i;
                    }
                }
            }
        }
        assert(best_prev_idx != -1);
        return {best_prev_idx, best_cl_idx};
    }

    void apply_load_operation(LoadOperation &op){
        auto [cl_idx, load_begin_pos, load_len, lb_pos] = op;
        rep(i,0,load_len) memory[lb_pos + i] = CLVerts[cl_idx][load_begin_pos + i];
    }

    void goto_dest2(int ts_idx){
        auto dest = Ts[ts_idx];
        auto verts = current_available_verts();
        auto reserved_op = search_best_arrival2(ts_idx);

        int start_vert = -1, min_dist_to_prevprev = INT_MAX;
        for(auto e : verts) if(chmin(min_dist_to_prevprev, DistMap[e][reserved_op.prevprev_vert])) start_vert = e;
        ans.push_back({LoadOperation(-1,-1,-1,-1), start_vert});
        if(start_vert == dest) return;

        cur_pos = start_vert;
        while(cur_pos != reserved_op.prevprev_vert){
            int next_vert = -1;
            rep(i,0,N) if(EdgeCLSet[cur_pos][i].size() > 0 && DistMap[i][reserved_op.prevprev_vert] == DistMap[cur_pos][reserved_op.prevprev_vert] - 1) next_vert = i;
            // cur_posからnext_vertに移動
            int cl_idx = EdgeCLSet[cur_pos][next_vert][0][0];

            auto load_op = LoadOperation(cl_idx, 0, CLLen[cl_idx], 0);
            apply_load_operation(load_op);
            ans.push_back({load_op, next_vert});
            cur_pos = next_vert;
        }

        // prevprev_vertに到達したので、prev_vertに移動
        if(cur_pos != reserved_op.prev_vert){
            auto cl_idx = reserved_op.prev_cl_idx;
            if(cl_idx == -1) cl_idx = EdgeCLSet[reserved_op.prevprev_vert][reserved_op.prev_vert][0][0];

            auto load_op = LoadOperation(cl_idx, 0, CLLen[cl_idx], 0);
            apply_load_operation(load_op);
            ans.push_back({load_op, reserved_op.prev_vert});
            cur_pos = reserved_op.prev_vert;
        }

        // prev_vertに到達したので、destに移動
        {
            auto load_op = reserved_op.dest_op;
            apply_load_operation(load_op);
            ans.push_back({load_op, dest});
            cur_pos = dest;
        }
    }

    void goto_dest(int ts_idx){
        auto dest = Ts[ts_idx];
        auto verts = current_available_verts();
        auto [arrival_prev, arrival_cl] = search_best_arrival(ts_idx);
        // vertsの中で、arrival_prevに一番近い頂点に移動
        int min_dist = INT_MAX, min_idx = -1;
        for(auto e : verts) if(chmin(min_dist, DistMap[e][arrival_prev])) min_idx = e;

        ans.push_back({LoadOperation(-1, -1, -1, -1), min_idx});
        if(min_idx == dest) return;

        cur_pos = min_idx;
        while(cur_pos != arrival_prev){
            int next_vert = -1;
            rep(i,0,N) if(EdgeCLSet[cur_pos][i].size() > 0 && DistMap[i][arrival_prev] == DistMap[cur_pos][arrival_prev] - 1) next_vert = i;
            // cur_posからnext_vertに移動
            int cl_idx = EdgeCLSet[cur_pos][next_vert][0][0];

            auto load_op = LoadOperation(cl_idx, 0, CLLen[cl_idx], 0);
            apply_load_operation(load_op);
            ans.push_back({load_op, next_vert});
            cur_pos = next_vert;
        }
        // arrival_prevに到達したので、arrival_clをロードしてdestに移動
        {
            int load_lb_pos = 0;
            if(CLLen[arrival_cl] < Lb){
                auto contains_cur_at_front = false;
                rep(i,0,Lb/2) if(memory[i] == cur_pos) contains_cur_at_front = true;
                if(contains_cur_at_front) load_lb_pos = Lb-CLLen[arrival_cl];
                else load_lb_pos = 0;
            }
            auto load_op = LoadOperation(arrival_cl, 0, CLLen[arrival_cl], load_lb_pos);
            apply_load_operation(load_op);
            ans.push_back({load_op, dest});
            cur_pos = dest;
        }
    }

    // <ロード操作, move先の頂点idx>の配列を返す
    void solve(){
        memory = v<int>(Lb, -1);
        cur_pos = 0;
        rep(i,0,T){
            goto_dest2(i);
        }
        for(auto [op, dest] : ans) score += op.cl_idx >= 0;
    }
};

void CopyToLaArray(vv<int> &lines){
    v<int> res;
    for(auto line : lines) for(int v : line) res.push_back(v);
    while(res.size() < La) res.push_back(0);
    LaArray = res;
}

struct DebugLogger{
    void debug_reduced_input(){
        v<array<int,2>> edges;
        rep(i,0,N) for(auto e : G[i]) if(i < e) edges.push_back({i, e});
        cout << N << " " << edges.size() << " " << T << " " << La << " " << Lb << endl;
        for(auto [a, b] : edges) cout << a << " " << b << endl;
        rep(i,0,T) cout << Ts[i] << endl;
        rep(i,0,N){
            if(unused_vert[i]) cout << "0 0" << endl;
            else cout << X[i] << " " << Y[i] << endl;
        }
        exit(0);
    }

    void debug_vertex_usage_count_map(v<int> line){
        v<int> use_cnt(N);
        for(auto e : line) use_cnt[e]++;
        map<int,int> use_cnt_map;
        for(auto e : use_cnt) use_cnt_map[e]++;
        cerr << "Vertex Usage Count: ";
        for(auto [k, v] : use_cnt_map) cerr << "(" << k << ": " << v << ") ";
        cerr << endl;

        map<pair<int,int>,int> edge_use_cnt;
        rep(i,0,line.size()-1){
            int a = line[i], b = line[i+1];
            edge_use_cnt[{min(a,b), max(a,b)}]++;
        }
        use_cnt_map.clear();
        for(auto e : edge_use_cnt) use_cnt_map[e.second]++;
        cerr << "Edge Usage Count: ";
        for(auto [k, v] : use_cnt_map) cerr << "(" << k << ": " << v << ") ";
        cerr << endl;
    }

    void debug_route_draw_operation(vv<int> &lines){
        // splitted lines を順番に描画
        CopyToLaArray(lines);

        CacheLineSplitter cl_splitter;
        cl_splitter.run(lines);
        // 0から LaArray[0]に移動
        cout << LaArray << endl;
        int cur = 0;
        auto [can_travel, path] = dijkstra.shortest_path(G, {cur}, {LaArray[0]});
        rep(i,1,path.size()){
            int la_idx = -1; rep(j,0, LaArray.size()) if(LaArray[j] == path[i]) la_idx = j;
            cout << "s " << 1 << " " << la_idx << " " << 0 << endl;
            cout << "m " << path[i] << endl;
            cur = path[i];
        }
        v<int> memory(Lb, -1);

        int la_pos = 0;
        rep(i,0,min(La + 50, (int)CLVerts.size())){
            cout << "s " << CLLen[i] << " " << CLBeginPosInLa[i] << " 0" << endl;
            for(int j = 0; j < CLLen[i]; j++) memory[j] = LaArray[CLBeginPosInLa[i] + j];
            // CLVertsの最後尾まで最短経路で移動
            cache_array.use();
            for(auto e : memory) if(e >= 0) cache_array.VERT[e] = cache_array.vert_idx;
            vv<int> localG(N);
            for(auto e : CLVerts[i]) for(auto nxt : G[e]) if(cache_array.VERT[nxt] == cache_array.vert_idx) localG[e].push_back(nxt);
            int dest_idx = CLBeginPosInLa[i] + CLLen[i] - 1;
            while(la_pos < dest_idx){
                la_pos++;
                auto [can_travel, path] = dijkstra.shortest_path(localG, {cur}, {LaArray[la_pos]});
                if(!can_travel) exit(0);

                rep(j,1,path.size()){
                    cout << "m " << path[j] << endl;
                    cur = path[j];
                }
            }
            // auto [can_travel, path] = dijkstra.shortest_path(localG, {cur}, {CLVerts[i].back()});
            // if(!can_travel) break;
            // rep(j,1,path.size()){
            //     cout << "m " << path[j] << endl;
            //     cur = path[j];
            // }
        }
        exit(0);
    }

    bool debug_lower_limit_score(){
        int score = 0;
        int cur = 0;
        for(auto nxt : Ts){
            score += DistMap[cur][nxt];
            cur = nxt;
        }
        // cerr << "最低保証スコア: " << score << endl;
        return score < 10000;
    }
}debug;

void input(){
#if DEBUG
    ifstream cin("tools/in/0027.txt");
#endif
    int _;
    cin >> _ >> M >> _ >> La >> Lb;
    rep(i, 0, M) {
        int a,b; cin >> a >> b;
        G[a].push_back(b);
        G[b].push_back(a);
    }
    for(auto& g : G) sort(all(g));

    rep(i,0,T) cin >> Ts[i];
    rep(i,0,N) cin >> X[i] >> Y[i];
    rep(i,0,T) GoalCnt[Ts[i]]++;
    cerr << "M: " << M << " La: " << La << " Lb: " << Lb << endl;

}

v<string> answer_strs(v<pair<LoadOperation, int>>& ops){
    v<string> res;
    // LaArray
    string la_str = "";
    for(auto e : LaArray) la_str += to_string(e) + " ";
    res.push_back(la_str);
    // ops
    v<int> memory(Lb, -1);
    int cur = 0;
    for(auto [op, dest] : ops){
        if(op.cl_idx >= 0){
            for(int i = 0; i < op.load_len; i++) memory[op.lb_pos + i] = LaArray[CLBeginPosInLa[op.cl_idx] + op.load_begin_pos + i];
            string s = "s " + to_string(op.load_len) + " " + to_string(CLBeginPosInLa[op.cl_idx] + op.load_begin_pos) + " " + to_string(op.lb_pos);
            res.push_back(s);
        }
        cache_array.use();
        for(auto e : memory) if(e >= 0) cache_array.VERT[e] = cache_array.vert_idx;
        cache_array.VERT2[cur] = cache_array.vert_idx;
        
        v<int> vert_path;
        function<bool(int)> dfs = [&](int cur){
            vert_path.push_back(cur);
            if(cur == dest) return true;
            cache_array.VERT2[cur] = cache_array.vert_idx;
            for(auto nxt : G[cur]){
                if(cache_array.VERT[nxt] != cache_array.vert_idx) continue;
                if(cache_array.VERT2[nxt] == cache_array.vert_idx) continue;
                if(dfs(nxt)) return true;
            }
            vert_path.pop_back();
            return false;
        };
        dfs(cur);
        if(vert_path.size() == 0){
            cerr << "error: from " << cur << " to " << dest << endl;
            return res;
        }
        vert_path.erase(vert_path.begin());
        for(auto e : vert_path) res.push_back("m " + to_string(e));
        cur = dest;
    }
    return res;
}

// <ロード操作, move先の頂点idx>, linesは分割後のキャッシュライン
void output_res(v<pair<LoadOperation, int>> ops){
    int score = 0;
    cout << LaArray << endl;
    // TODO: opsを出力
    v<int> memory(Lb, -1);
    int cur = 0;
    for(auto [op, dest] : ops){
        if(op.cl_idx >= 0){
            score++;
            // ロード操作
            for(int i = 0; i < op.load_len; i++) memory[op.lb_pos + i] = LaArray[CLBeginPosInLa[op.cl_idx] + op.load_begin_pos + i];
            
            cout << "s " << op.load_len << " " << CLBeginPosInLa[op.cl_idx] + op.load_begin_pos << " " << op.lb_pos << endl;
        }
        // dfsで移動経路を求める
        cache_array.use();
        for(auto e : memory) if(e >= 0) cache_array.VERT[e] = cache_array.vert_idx;
        cache_array.VERT2[cur] = cache_array.vert_idx;
        
        v<int> vert_path;
        function<bool(int)> dfs = [&](int cur){
            vert_path.push_back(cur);
            if(cur == dest) return true;
            cache_array.VERT2[cur] = cache_array.vert_idx;
            for(auto nxt : G[cur]){
                if(cache_array.VERT[nxt] != cache_array.vert_idx) continue;
                if(cache_array.VERT2[nxt] == cache_array.vert_idx) continue;
                if(dfs(nxt)) return true;
            }
            vert_path.pop_back();
            return false;
        };
        dfs(cur);
        if(vert_path.size() == 0){
            cout << "error: from " << cur << " to " << dest << endl;
            exit(1);
        }
        vert_path.erase(vert_path.begin());
        for(auto e : vert_path) cout << "m " << e << endl;
        cur = dest;
    }
    cerr << "Score = " << score << endl;
}

int main(int argc, char *argv[]){
    input();

    VertexReducer vr(G);
    vr.reduce();
    G = vr.G;
    Routes = vr.Routes;
    rep(i,0,N) unused_vert[i] = vr.use_cnt[i] == 0;
    cerr << "Unused Verts: " << accumulate(unused_vert.begin(), unused_vert.end(), 0) << endl;

    // debug.debug_reduced_input();

    v<string> best_ans;
    vv<int> best_lines;
    v<pair<LoadOperation, int>> best_ops;
    int best_score = INT_MAX;
    float best_a = 5;

    int ng_streak = -10000;
    bool fix_a = false;
    rep(loop,0,300){
        rnd.seed(1);
#if !DEBUG
        if(check_time() > time_limit - 300) break;
#endif
        LissajousCurveCacheLineCreator cl_creator;
        cl_creator.a = 5 + loop;
        cl_creator.b = cl_creator.a + 1;
        if(fix_a){
            cl_creator.fill_rest_LaArray = true;
            // cl_creator.a = best_a + (float)((rand() % 100) - 50) / 500;
            rnd.seed(loop+1);
            cl_creator.a = best_a + normal_dist(0, 0.1);
            cerr << "a: " << cl_creator.a << endl;
            cl_creator.b = cl_creator.a + 1;
        }
        
        cl_creator.run();
        int line_vert_cnt = 0; for(auto line : cl_creator.lines) line_vert_cnt += line.size();
        if(line_vert_cnt < La * 0.9) continue;

        vv<int> lines = cl_creator.lines;
        CopyToLaArray(lines);
        
        CacheLineSplitter cl_splitter;
        cl_splitter.run(lines);

        if(!debug.debug_lower_limit_score()){
            LaArray.clear();
            CLVerts.clear();
            CLBeginPosInLa.clear();
            CLLen.clear();
            rep(i,0,N) rep(j,0,N) EdgeCLSet[i][j].clear();
            rep(i,0,N) rep(j,0,N) DistMap[i][j] = MAX;

            ng_streak++;
            if(ng_streak >= 5) fix_a = true;
            continue;
        }
        // debug.debug_vertex_usage_count_map(lines[0]);


        SimpleRouteSolver rs;
        rs.solve();
        cerr << "TmpScore: " << rs.score << endl;
        if(chmin(best_score, rs.score)){
            cerr << "Update Best Score: " << best_score << endl;
            best_ans = answer_strs(rs.ans);
            best_lines = lines;
            best_ops = rs.ans;
            best_a = cl_creator.a;
            ng_streak = 0;
        }else{
            ng_streak++;
            if(ng_streak >= 5) fix_a = true;
        }

        LaArray.clear();
        CLVerts.clear();
        CLBeginPosInLa.clear();
        CLLen.clear();
        rep(i,0,N) rep(j,0,N) EdgeCLSet[i][j].clear();
        rep(i,0,N) rep(j,0,N) DistMap[i][j] = MAX;
 
        // if(ng_streak > 5) break;
    }
    // debug.debug_route_draw_operation(best_lines);

    cerr << "Score = " << best_score << endl;
    for(auto s : best_ans) cout << s << endl;
}