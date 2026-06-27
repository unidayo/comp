#include <bits/stdc++.h>
#include <atcoder/all>
#include <cstdlib>
#include <cmath>
#include <random>
#include <fstream>

#define rep(i, begin, end) for (int i = begin; i < end; i++)
#define drep(i, begin, end) for (int i = begin; i >= end; i--)
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
using namespace std;
using namespace atcoder;
#ifdef LOCAL
void debug_out() {
    cerr << endl;
}
template <class Head, class... Tail> void debug_out(Head H, Tail... T) {
    cerr << ' ' << H;
    debug_out(T...);
}
#define debug(...) cerr << 'L' << __LINE__ << " [" << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)
#define dump(x) cerr << 'L' << __LINE__ << " " << #x << " = " << (x) << endl
#else
#define debug(...) (void(0))
#define dump(x) (void(0))
#endif
using ll = long long;
using pi = pair<int, int>;
template<class T> using V = vector<T>;
template<class T> using VV = vector<vector<T>>;
template<class T> using VVV = vector<vector<vector<T>>>;
template<class T> using priority_queue_rev = priority_queue<T, vector<T>, greater<T>>;
template<class T> inline bool chmin(T &a, const T b) { if (a > b) { a = b; return true; } return false; }
template<class T> inline bool chmax(T &a, const T b) { if (a < b) { a = b; return true; } return false; }
template<class T> istream &operator>>(istream &is, vector<T> &v) { for (auto &e : v) is >> e; return is; }
template<class T> ostream &operator<<(ostream &os, const vector<T> &v) { for (auto &e : v) os << e << ' '; return os; }
template<class T> inline void to_unique(vector<T> &a) { sort(all(a)); a.erase(unique(all(a)), a.end()); }
struct fast_ios { fast_ios(){ cin.tie(nullptr); ios::sync_with_stdio(false); }; } fast_ios_;

#if DEBUG
constexpr float machine_spec = 0.5;
#elif LOCAL
constexpr float machine_spec = 1.4;
#else
constexpr float machine_spec = 1;
#endif
constexpr ll time_limit = (2000-50);

struct Timer{
    private:
        chrono::system_clock::time_point start = chrono::system_clock::now();
        chrono::system_clock::time_point lap = chrono::system_clock::now();
    public:
        // 実行開始時からの経過時間(ms)を返す
        int elapsed_ms(){
            auto now = chrono::system_clock::now();
            #if DEBUG or LOCAL
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() * machine_spec;
            #else
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            #endif
        }
        // ラップタイム(ms)を返す
        int lap_ms(){
            auto now = chrono::system_clock::now();
            #if DEBUG or LOCAL
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - lap).count() * machine_spec;
            #else
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - lap).count();
            #endif
        }
        // ラップを刻む
        void mark_lap(){
            lap = chrono::system_clock::now();
        }
} timer;

class xrand {
    uint64_t x;

   public:
    using result_type = uint32_t;
    static constexpr result_type min() {
        return std::numeric_limits<result_type>::min();
    }
    static constexpr result_type max() {
        return std::numeric_limits<result_type>::max();
    }
    xrand(uint64_t k) : x(k) {}
    xrand() : xrand(998244353) {}
    result_type operator()() {
        x ^= x << 9;
        return x ^= x >> 7;
        // return (x * 0x123456789abcdef) >> 32;
    }
    int64_t operator()(int m) { return int64_t((*this)()) * m >> 32; }
    int64_t operator()(int l_include, int r_exlude) { return l_include + (*this)(r_exlude - l_include); }
    double rand() { return double((*this)()) / (int64_t(1) << 32); }
// }rnd;
} rnd(clock() * 1e9 + time(nullptr));

/**
 * @brief Gumbel分布からのサンプリング
 */
struct GumbelSampler{
    const static int sample_size_exp = 15;
    float samples[1<<sample_size_exp];
    int mask = (1<<sample_size_exp) - 1;
    
    GumbelSampler(float scale){
        rep(i,0,1<<sample_size_exp)
            samples[i] = -log(-log(rnd.rand())) * scale;
    }
    
    float sample(){
        return samples[rnd() & mask];
    }

} gumbel_sampler(1);

constexpr int MAX_XY = 10000;
constexpr int N = 800; // 頂点数
int M; // グループの数
constexpr int Q = 400; // 投げられるクエリ数
int L; // 1つのクエリで投げられる頂点の数
int W; // 頂点の座標の範囲の縦横最大値
int G[400];
int LX[N], RX[N], LY[N], RY[N]; // 頂点iの座標の範囲(境界含む)
int WX[N], WY[N]; // 頂点iのブレ幅
bool ignore_verts[N]; // ignore_verts[i] = trueなら、頂点iの座標推定はせずに、size=1のグループにつっこむ
bool local_judge = false;


namespace mst{
    struct Result{
        V<pair<int, int>> edges;
        int total_len;
    };

    struct MstRootedTree{
        int root;
        int child_size[N]; // 部分木のサイズ. 自分を含む
        set<int> childs[N]; // 子供
        int parent[N]; // 親
        int parent_dist[N]; // 親との距離
        int depth[N]; // 木の深さ
    };

    struct Solver{
        public:
        Result solve(V<int>& verts, int X[N], int Y[N]){
            int sz = verts.size();
            priority_queue_rev<tuple<int, int, int>> edges;
            rep(i,0,sz)rep(j,i+1,sz){
                int u = verts[i], v = verts[j];
                int len = sqrt((X[u] - X[v]) * (X[u] - X[v]) + (Y[u] - Y[v]) * (Y[u] - Y[v]));
                edges.emplace(len, i, j);
            }
            return kraskal(edges, verts);
        }

        Result solve(V<int>& verts, int Dist[N][N]){
            int sz = verts.size();
            priority_queue_rev<tuple<int, int, int>> edges;
            rep(i,0,sz)rep(j,i+1,sz){
                int u = verts[i], v = verts[j];
                int len = Dist[u][v];
                edges.emplace(len, i, j);
            }
            return kraskal(edges, verts);
        }

        MstRootedTree build_mst_rooted_tree(Result& res, int Dist[N][N]){
            MstRootedTree tree;
            VV<int> g(N);
            for(auto [u, v] : res.edges){
                g[u].push_back(v);
                g[v].push_back(u);
            }
            // 適当な頂点を根とした根付き木を仮で作り、depthを求める
            int tmp_root = res.edges[0].first;
            queue<int> q;
            q.push(tmp_root);
            int tmp_depth[N]; memset(tmp_depth, -1, sizeof(tmp_depth));
            tmp_depth[tmp_root] = 0;
            while(!q.empty()){
                int v = q.front(); q.pop();
                for(auto nxt : g[v]){
                    if(tmp_depth[nxt] != -1) continue;
                    tmp_depth[nxt] = tmp_depth[v] + 1;
                    q.push(nxt);
                }
            }
            int root = -1, max_depth = -1;
            rep(i,0,N) if(chmax(max_depth, tmp_depth[i])) root = i;
            // 根をrootとして、根付き木を作る
            tree.root = root;
            q.push(root);
            memset(tree.depth, -1, sizeof(tree.depth));
            memset(tree.parent, -1, sizeof(tree.parent));
            memset(tree.parent_dist, 0, sizeof(tree.parent_dist));
            tree.depth[root] = 0;
            while(!q.empty()){
                int v = q.front(); q.pop();
                tree.childs[v].clear();
                for(auto nxt : g[v]){
                    if(tree.depth[nxt] != -1) continue;
                    tree.depth[nxt] = tree.depth[v] + 1;
                    tree.parent[nxt] = v;
                    tree.parent_dist[nxt] = Dist[v][nxt];
                    tree.childs[v].insert(nxt);
                    q.push(nxt);
                }
            }
            // 部分木のサイズをdfsで求める
            function<int(int)> dfs = [&](int v){
                int res = 1;
                for(auto nxt : tree.childs[v]) res += dfs(nxt);
                return tree.child_size[v] = res;
            };
            dfs(root);
            return tree;
        }

        private:
        Result kraskal(priority_queue_rev<tuple<int, int, int>>& edges, V<int>& verts){
            int sz = verts.size();
            dsu uf(sz);
            V<pair<int, int>> res;
            int total_len = 0;
            while(!edges.empty()){
                auto [len, u, v] = edges.top(); edges.pop();
                if(uf.same(u, v)) continue;
                uf.merge(u, v);
                res.emplace_back(verts[u], verts[v]);
                total_len += len;
                if(res.size() == sz-1) break;
            }
            return {res, total_len};
        }        
    };
}

struct judge{
    int X[N], Y[N]; // local_judgeの場合のみ使用
    void init(){
        #if DEBUG
        local_judge = true;
        #endif
        if(local_judge) rep(i,0,N) cin >> X[i] >> Y[i];
    }

    V<pair<int, int>> local_kruskal(V<int> verts){
        // クエリの頂点を含む最小全域木を求める
        int sz = verts.size();
        sort(all(verts));
        map<int, int> mp; // 頂点番号 -> 圧縮後のidx
        rep(i,0,sz) mp[verts[i]] = i;
        priority_queue_rev<tuple<int, int, int>> edges;
        rep(i,0,sz)rep(j,i+1,sz){
            int u = verts[i], v = verts[j];
            int len = (X[u] - X[v]) * (X[u] - X[v]) + (Y[u] - Y[v]) * (Y[u] - Y[v]);
            edges.emplace(len, u, v);
        }
        dsu uf(sz);
        V<pair<int, int>> res;
        while(!edges.empty()){
            auto [len, u, v] = edges.top(); edges.pop();
            if(uf.same(mp[u], mp[v])) continue;
            uf.merge(mp[u], mp[v]);
            res.emplace_back(u, v);
        }
        return res;
    }

    V<pair<int, int>> send_query(V<int> verts){
        cout << "? " << verts.size();
        for(auto v : verts) cout << " " << v;
        cout << endl;
        if(local_judge){
            return local_kruskal(verts);
        }else{
            V<pair<int, int>> res;
            rep(i,0,verts.size()-1){
                int u, v; cin >> u >> v;
                res.emplace_back(u, v);
            }
            return res;
        }
    }
} judge;

namespace Geometry{
    struct Vec{
        float x, y;
        Vec operator+(Vec& v){ return {x + v.x, y + v.y}; }
        Vec operator-(Vec& v){ return {x - v.x, y - v.y}; }
        Vec operator*(float k){ return {x * k, y * k}; }

        friend Vec operator+(const Vec& vec1, const Vec& vec2){
            return Vec{vec1.x + vec2.x, vec1.y + vec2.y};
        }
    };

    struct Pos{
        int x, y;
        Pos operator+(Vec& v){ return {x + (int)v.x, y + (int)v.y}; }
        Pos operator-(Vec& v){ return {x - (int)v.x, y - (int)v.y}; }
        Pos operator*(float k){ return {(int)(x * k), (int)(y * k)}; }

        friend Pos operator+(const Pos& pos, const Pos& pos2){
            return Pos{pos.x + pos2.x, pos.y + pos2.y};
        }
    };

    int dist(Pos& a, Pos& b){ return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)); }
    int dist2(Pos& a, Pos& b){ return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y); }
}

namespace ignore_verts_preprocess{

    void run(){
        // return;
        if(L > 4) return;
        if(L == 4 && W < 1200) return;
        if(L == 3 && W < 1200) return;


        int g1_cnt = 0;
        rep(i,0,M) if(G[i] == 1) g1_cnt++;
        if(g1_cnt == 0) return;

        // TODO: L, Wに応じて、そもそもignoreするべきか決める
        // Lが大きいなら、推定精度が高くてMSTも最適に近くなるので、ignoreしない方が良さそう
        // Wが小さいなら、同様にMSTが最適に近くなるのでignoreしない方が良い
        
        V<pair<int, int>> var_idx_pairs;
        rep(i,0,N) var_idx_pairs.push_back({WX[i] * WX[i] + WY[i] * WY[i], i});
        sort(rall(var_idx_pairs));
        rep(i,0,g1_cnt){
            auto [_, idx] = var_idx_pairs[i];
            ignore_verts[idx] = true;
        }
    }
};

namespace vert3_sa{
    struct SA{
        struct State{
            // クエリに投げる3頂点の組み合わせ達
            int tuples[400][3];
            int tuple_size;
            int score; // 全クエリの三角形度合いの合計

            int calc_score(int tuple_idx, Geometry::Pos pos[N]){
                static int len[3];
                int tuple[3] = {tuples[tuple_idx][0], tuples[tuple_idx][1], tuples[tuple_idx][2]};
                len[0] = Geometry::dist(pos[tuple[0]], pos[tuple[1]]);
                len[1] = Geometry::dist(pos[tuple[1]], pos[tuple[2]]);
                len[2] = Geometry::dist(pos[tuple[2]], pos[tuple[0]]);

                // score = 最短辺の長さ - 最長辺の長さ
                int max_len = 0, min_len = INT_MAX;
                rep(i,0,3){
                    chmax(max_len, len[i]);
                    chmin(min_len, len[i]);
                }
                return min_len - max_len;
            }
        };
        
        struct StaticData{
            Geometry::Pos pos[N];
        };
        StaticData data;
        
        struct Transition{
            int types[2000];
            int type_size = 0;
            
            long long try_cnt[100];
            long long accept_cnt[100];
            const int TransitionTypeSize = 1;
    
            Transition(){
                rep(i,0,1) types[type_size++] = 0;
            }
    
            bool execute(State& state, StaticData& data, float current_temp){
                int type = types[rnd() % type_size];
                bool res;
                switch(type){
                    case 0: res = swap_random(state, data, current_temp); break;
                }
                try_cnt[type]++;
                if(res) accept_cnt[type]++;
                return res;
            }
    
            bool should_accept_transition(float score_diff, float current_temp){
                return score_diff > 0 || rnd.rand() < exp(score_diff / current_temp);
            }

            // 1頂点をランダムに選び、その座標をランダムに変更する
            bool swap_random(State& state, StaticData& data, float current_temp){
                int tidx0 = rnd(state.tuple_size);
                int tidx1 = rnd(state.tuple_size);
                if(tidx0 == tidx1) return false;

                int t0_tuple_idx = rnd(3);
                int t1_tuple_idx = rnd(3);
                if(state.tuples[tidx0][t0_tuple_idx] == state.tuples[tidx1][t1_tuple_idx]) return false;

                // swapした時に、tuple内に同じ頂点が入ったらNG
                rep(i,0,3) if(state.tuples[tidx0][i] == state.tuples[tidx1][t1_tuple_idx]) return false;
                rep(i,0,3) if(state.tuples[tidx1][i] == state.tuples[tidx0][t0_tuple_idx]) return false;

                int prev_score = state.calc_score(tidx0, data.pos) + state.calc_score(tidx1, data.pos);
                // swap
                swap(state.tuples[tidx0][t0_tuple_idx], state.tuples[tidx1][t1_tuple_idx]);
                int next_score = state.calc_score(tidx0, data.pos) + state.calc_score(tidx1, data.pos);
                int score_diff = next_score - prev_score;
                if(should_accept_transition(score_diff, current_temp)){
                    // accept
                    state.score = next_score;
                    return true;
                }else{
                    // reject
                    swap(state.tuples[tidx0][t0_tuple_idx], state.tuples[tidx1][t1_tuple_idx]);
                    return false;
                }
            }
        } transition;
    
        struct Temp{
            float start_temp = 1000;
            float mid_temp = 2000;
            float end_temp = 0.1;
            
            float get_temp(float progress){
                // float threshold = 0.3;
                // if(progress < threshold)
                //     return pow(start_temp, 1.0 - progress / threshold) * pow(mid_temp, progress / threshold);
                // else
                //     return mid_temp + (end_temp - mid_temp) * (progress - threshold) / (1.0 - threshold);
                return pow(start_temp, 1.0 - progress) * pow(end_temp, progress);
                // return start_temp + (end_temp - start_temp) * progress;
            }
        } temp;

        SA(Geometry::Pos pred_pos[N]){
            rep(i,0,N) data.pos[i] = pred_pos[i];
        }
    
    
        State run(int time_limit, State state){
            timer.mark_lap();
    
            State best_state = state;
            float current_temp = temp.start_temp;
            int loop_cnt = 0;
            int accept_cnt = 0;
            while(timer.lap_ms() < time_limit){
                float progress = (float)timer.lap_ms() / time_limit;
                current_temp = temp.get_temp(progress);
                rep(_,0,1024){
                    loop_cnt++;
                    bool accept = transition.execute(state, data, current_temp);
                    accept_cnt += accept;
    
                    if(accept && state.score > best_state.score){
                        debug(loop_cnt, current_temp, state.score);
                        best_state = state;
                    }
                }
            }
            debug(loop_cnt, state.score, best_state.score);
            cerr << "loop_cnt: " << loop_cnt << ", accept_rate: " << (float)accept_cnt / loop_cnt << endl;
            // return best_state;
            return state;
        }
    };

    struct Solver{
        SA::State init_state(V<int> verts, Geometry::Pos pred_pos[N]){
            SA::State state;
            state.tuple_size = verts.size() / 3;
            sort(all(verts));
            int idx = 0;
            rep(j,0,3)rep(i,0,state.tuple_size){
                state.tuples[i][j] = verts[idx++];
            }
            state.score = 0;
            rep(i,0,state.tuple_size)
                state.score += state.calc_score(i, pred_pos);

            return state;
        }

        SA::State run(V<int> verts, Geometry::Pos pred_pos[N]){
            auto state = init_state(verts, pred_pos);
            SA sa(pred_pos);
            state = sa.run(150, state);

            return state;
        }
    };
}

namespace position_estimation{
    // 2辺の長さの比較結果
    struct EdgeCompareInfo{
        int shorter_edge[2];
        int longer_edge[2];

        int CalcError(Geometry::Pos pos[N]){
            int d0 = Geometry::dist(pos[shorter_edge[0]], pos[shorter_edge[1]]);
            int d1 = Geometry::dist(pos[longer_edge[0]], pos[longer_edge[1]]);
            return max(0, d0 - d1);
        }
    };

    struct QueryBuilder{
        Geometry::Pos pred_pos[N];

        VV<int> query_verts_log;
        VV<pair<int, int>> query_results_log;

        QueryBuilder(Geometry::Pos pred_pos[N]){
            rep(i,0,N) this->pred_pos[i] = pred_pos[i];
        }        

        V<EdgeCompareInfo> convert_query_results_to_shortest_edge_info(V<pair<int, int>> edges, V<int> verts){
            int n = verts.size();
            int m = edges.size();
            sort(all(verts));

            V<EdgeCompareInfo> res;
            // edgesを1つ除いた時の２グループを繋ぐ辺リストの中で、今取り除いた辺が最小であるように
            // 間違えて短く評価された辺が複数のShortestInfoに入ると過剰に評価されるので、edgesを長い順に見ていき、同一のミス辺の使用回数がthresholdを超えたら、その辺は以降のShortestInfoに入れない
            //（現状、とても長いと評価されているやつで補正かけられるから、その頃には、より短いedgeとの評価は楽々クリアしているだろう。という思考）

            // {辺の長さ, 頂点1, 頂点2}
            V<array<int, 3>> ascend_edge_lens;
            rep(i,0,n)rep(j,i+1,n) ascend_edge_lens.push_back({Geometry::dist(pred_pos[verts[i]], pred_pos[verts[j]]), verts[i], verts[j]});
            sort(all(ascend_edge_lens));

            // edgesは降順ソートする
            sort(rall(edges), [&](auto& a, auto& b){
                return Geometry::dist(pred_pos[a.first], pred_pos[a.second]) < Geometry::dist(pred_pos[b.first], pred_pos[b.second]);
            });

            // ミス辺として使う回数上限
            int threshold = (L <= 10) ? 10 : 5;
            if(L > 10) threshold = 5 + (15 - L);
            // クラスカル辺よりどれくらい長い辺まで見るか
            float upper_threshold = 1 * W;
            map<pair<int, int>, int> edge_use_cnt;
            rep(remove_edge_idx,0,edges.size()){
                auto [v0, v1] = edges[remove_edge_idx];
                auto pred_len = Geometry::dist(pred_pos[v0], pred_pos[v1]);

                if(v1 < v0) swap(v0, v1);
                // NOTE: 座標圧縮すべきだけど、高々Q*(L-1)回しかやらない処理なので
                dsu uf(N);
                rep(i,0,m) if(i != remove_edge_idx) uf.merge(edges[i].first, edges[i].second);
                
                int eci_cnt = 0;
                for(auto& [len, u, v] : ascend_edge_lens){
                    if(u == v0 && v == v1) continue;
                    if(uf.same(u, v)) continue;
                    if(edge_use_cnt[{u,v}] >= threshold) continue;
                    if(eci_cnt >= 2 && len > pred_len + upper_threshold) break;
                    edge_use_cnt[{u,v}]++;
                    EdgeCompareInfo eci;
                    eci.shorter_edge[0] = v0;
                    eci.shorter_edge[1] = v1;
                    eci.longer_edge[0] = u;
                    eci.longer_edge[1] = v;
                    res.push_back(eci);
                    eci_cnt++;
                }
                // cerr << "remove_edge_idx: " << remove_edge_idx << " eci_cnt: " << eci_cnt << endl;
            }
            return res;
        }

        V<EdgeCompareInfo> get_unique_edge_compare_info(V<EdgeCompareInfo>& edge_compare_infos){
            V<EdgeCompareInfo> res;
            int hash[N];
            rep(i,0,N) hash[i] = rnd();
            unordered_set<int> used;
            int skip_cnt = 0;
            // for(auto& eci : edge_compare_infos){
            rep(i,0,edge_compare_infos.size()){
                auto& eci = edge_compare_infos[i];
                auto h = hash[eci.shorter_edge[0]] ^ (hash[eci.shorter_edge[1]] << 1) ^ (hash[eci.longer_edge[0]] << 2) ^ (hash[eci.longer_edge[1]] << 3);
                if(used.count(h)){
                    // debug("skip", i, eci.shorter_edge[0], eci.shorter_edge[1], eci.longer_edge[0], eci.longer_edge[1]);
                    skip_cnt++;
                    continue;
                }
                used.insert(h);
                res.push_back(eci);
            }
            debug(skip_cnt, edge_compare_infos.size(), skip_cnt * 100.0 / edge_compare_infos.size());
            // exit(0);
            return res;
        }

        // 重心から近い頂点を選んでいくクエリを投げる
        V<EdgeCompareInfo> create_around_pos_query(int query_num){
            int X[N], Y[N];
            rep(i,0,N) X[i] = (LX[i] + RX[i]) / 2, Y[i] = (LY[i] + RY[i]) / 2;

            float STD[N];
            rep(i,0,N) STD[i] = sqrt(WX[i] * WX[i] + WY[i] * WY[i]) / 2;

            int selected_pair_cnt[N][N];
            memset(selected_pair_cnt, 0, sizeof(selected_pair_cnt));

            V<EdgeCompareInfo> res;
            int vert_hash[N];
            rep(i,0,N) vert_hash[i] = rnd();
            
            V<int> candidates;
            rep(i,0,N) if(max(WX[i], WY[i]) > W / 3 && !ignore_verts[i]) candidates.push_back(i);
            V<int> selected_cnts(N, 0);

            rep(qi,0,query_num){
                // int v0 = candidates[rnd(candidates.size())];
                int v0 = -1, min_selected_cnt = INT_MAX;
                for(auto c : candidates) if(chmin(min_selected_cnt, selected_cnts[c])) v0 = c;

                Geometry::Pos center = {X[v0], Y[v0]};
                V<int> verts = {v0};
                set<int> used;
                used.insert(v0);
                while(verts.size() < L){
                    int best_idx = -1;
                    float best_score = 1e9;
                    // for(auto i : candidates){
                    rep(i,0,N){
                        if(ignore_verts[i]) continue;
                        if(used.count(i)) continue;
                        Geometry::Pos p = {X[i], Y[i]};
                        auto dist = Geometry::dist(center, p);
                        // gumbelのスケールが、L14,15くらいでWが小さい時、1000にしてた時の方が強かった
                        // L4, W2000では、 STD*1, selected*100 で三角クエリより強かった
                        static float std_coef = 400.0 / W;
                        float score = (float)(dist - STD[i] * std_coef) + gumbel_sampler.sample() * 0.1 * W;
                        // score -= STD[i] * 0.1;
                        // float score = (float)(dist - STD[i] * 0.1) + gumbel_sampler.sample() * max<float>(100, 0.3 * W) + selected_cnts[i] * 00;
                        int total_selected_pair_cnt = 0;
                        for(auto u : verts) total_selected_pair_cnt += selected_pair_cnt[i][u];
                        // score += total_selected_pair_cnt / verts.size() * 1000;
                        
                        static int selected_pair_error_coef = (20 - L) * (L <= 13 ? 60 : 200);
                        score += total_selected_pair_cnt * selected_pair_error_coef / verts.size();
                        // score += (total_selected_pair_cnt * total_selected_pair_cnt) * 100 / verts.size();
                        if(chmin(best_score, score)) best_idx = i;
                    }
                    int total_selected_pair_cnt = 0;
                    for(auto u : verts) total_selected_pair_cnt += selected_pair_cnt[best_idx][u];
                    // debug(qi, verts.size(), total_selected_pair_cnt);

                    // centerを更新
                    center.x = (center.x * verts.size() + X[best_idx]) / (verts.size() + 1);
                    center.y = (center.y * verts.size() + Y[best_idx]) / (verts.size() + 1);

                    verts.push_back(best_idx);
                    used.insert(best_idx);
                    // STD[best_idx] *= 0.85;
                }
                
                auto judge_res = judge.send_query(verts);
                auto edge_compare_infos = convert_query_results_to_shortest_edge_info(judge_res, verts);
                res.insert(res.end(), all(edge_compare_infos));
                
                for(auto v : verts) selected_cnts[v]++;
                for(auto v : verts) for(auto u : verts) if(v != u) selected_pair_cnt[v][u]++;
                
                // 現在のX,Yでのクラスカル結果とクエリ結果を見比べて、使用の可否が間違えている辺を元に、X,Yを更新する
                static int dx_sum[N], dy_sum[N];
                rep(t,0,10){
                    for(auto v : verts) dx_sum[v] = 0, dy_sum[v] = 0;

                    auto local_res = mst::Solver().solve(verts, X, Y);
                    // 正の辺は近づくべき、負の辺は遠ざかるべき
                    map<pair<int, int>, int> edge_use_cnt;
                    for(auto& [u, v] : local_res.edges) edge_use_cnt[{u, v}]--;
                    for(auto& [u, v] : judge_res) edge_use_cnt[{u, v}]++;

                    for(auto& [edge, cnt] : edge_use_cnt){
                        if(cnt == 0) continue;
                        auto [u, v] = edge;
                        // X,Yを更新する
                        Geometry::Vec uv = {(float)X[v] - X[u], (float)Y[v] - Y[u]};
                        int len = sqrt(uv.x * uv.x + uv.y * uv.y);
                        if(len == 0) continue;
                        int cor_len = max<int>(30, min<int>(len * 0.1, 500));
                        int dx = uv.x * cor_len / len;
                        int dy = uv.y * cor_len / len;
                        dx_sum[u] += dx * cnt, dy_sum[u] += dy * cnt;
                        dx_sum[v] -= dx * cnt, dy_sum[v] -= dy * cnt;
                    }
                    float move_sum = 0;
                    for(auto v : verts){
                        move_sum += sqrt(dx_sum[v] * dx_sum[v] + dy_sum[v] * dy_sum[v]);
                        X[v] += dx_sum[v], Y[v] += dy_sum[v];
                        // LX, RXでclamp
                        X[v] = max(LX[v], min(X[v], RX[v])), Y[v] = max(LY[v], min(Y[v], RY[v]));
                    }
                    if(move_sum == 0) break;
                }
                
                query_verts_log.push_back(verts);
                query_results_log.push_back(judge_res);
                // これまでのクエリについても、クエリ結果を見比べてX,Yを更新する
                int loop_cnt = max(0, 10 - L);
                if(L >= 10) loop_cnt = qi % 10 == 0 ? 1 : 0;
                rep(t,0,loop_cnt){
                    rep(i,0,query_verts_log.size()){
                        auto& verts = query_verts_log[i];
                        auto& judge_res = query_results_log[i];
                        for(auto v : verts) dx_sum[v] = 0, dy_sum[v] = 0;

                        auto local_res = mst::Solver().solve(verts, X, Y);
                        // 正の辺は近づくべき、負の辺は遠ざかるべき
                        map<pair<int, int>, int> edge_use_cnt;
                        for(auto& [u, v] : local_res.edges) edge_use_cnt[{u, v}]--;
                        for(auto& [u, v] : judge_res) edge_use_cnt[{u, v}]++;

                        for(auto& [edge, cnt] : edge_use_cnt){
                            if(cnt == 0) continue;
                            auto [u, v] = edge;
                            // X,Yを更新する
                            Geometry::Vec uv = {(float)X[v] - X[u], (float)Y[v] - Y[u]};
                            int len = sqrt(uv.x * uv.x + uv.y * uv.y);
                            if(len == 0) continue;
                            int cor_len = max<int>(30, min<int>(len * 0.1, 500));
                            int dx = uv.x * cor_len / len;
                            int dy = uv.y * cor_len / len;
                            dx_sum[u] += dx * cnt, dy_sum[u] += dy * cnt;
                            dx_sum[v] -= dx * cnt, dy_sum[v] -= dy * cnt;
                        }
                        for(auto v : verts){
                            X[v] += dx_sum[v], Y[v] += dy_sum[v];
                            // LX, RXでclamp
                            X[v] = max(LX[v], min(X[v], RX[v])), Y[v] = max(LY[v], min(Y[v], RY[v]));
                        }
                    }
                }
            }

            V<int> perm(N); rep(i,0,N) perm[i] = i;
            sort(all(perm), [&](int a, int b){
                return STD[a] < STD[b];
            });
            // rep(i,0,N) debug(i, selected_cnts[i], STD[i]);
            rep(i,0,N) debug(i, selected_cnts[perm[i]], STD[perm[i]]);
            cerr << "query elapsed time: " << timer.elapsed_ms() << endl;
            // exit(0);
            rep(i,0,N) pred_pos[i].x = X[i], pred_pos[i].y = Y[i];
            return get_unique_edge_compare_info(res);
        }

        
        // 正三角形のタイルを並べた形状の頂点を選んでクエリを投げる
        V<EdgeCompareInfo> create_triangle_tile_query(){
            int X[N], Y[N];
            rep(i,0,N) X[i] = (LX[i] + RX[i]) / 2, Y[i] = (LY[i] + RY[i]) / 2;

            float STD[N];
            rep(i,0,N) STD[i] = sqrt(WX[i] * WX[i] + WY[i] * WY[i]) / 2;

            bool init_triangle_used[N][N];
            rep(i,0,N) memset(init_triangle_used[i], 0, sizeof(init_triangle_used[i]));

            int min_edge_len = 500, max_edge_len = 4000;
            if(L >= 4) max_edge_len /= sqrt(L);
            V<int> selected_cnts(N, 0);
            // TODO: 推定回数が限られているので、分散小さいやつを推定対象から外しているが、パラメータ調整が全く。元々は0
            V<bool> enough_std(N, false);
            rep(i,0,N) enough_std[i] = STD[i] > W * 0.7 / 4;
            
            V<EdgeCompareInfo> res;
            rep(qi,0,Q){
                int root_idx;
                float max_std = 0;
                rep(i,0,N) if(!ignore_verts[i] && chmax(max_std, STD[i])) root_idx = i;

                Geometry::Pos v0 = {X[root_idx], Y[root_idx]};
                
                int len = rnd(min_edge_len, max_edge_len);
                Geometry::Vec vec0, vec1;
                while(true){
                    // vec0を360度ランダムに選ぶ
                    float theta0 = rnd(360) * M_PI / 180;
                    float theta1 = theta0 + M_PI / 3;
                    vec0 = Geometry::Vec{cos(theta0), sin(theta0)} * len;
                    vec1 = Geometry::Vec{cos(theta1), sin(theta1)} * len;
                    auto v1 = v0 + vec0;
                    auto v2 = v0 + vec1;
                    if(v1.x < 0 || v1.x >= MAX_XY || v1.y < 0 || v1.y >= MAX_XY) continue;
                    if(v2.x < 0 || v2.x >= MAX_XY || v2.y < 0 || v2.y >= MAX_XY) continue;
                    break;
                }

                // 三角座標系で表した目的座標をL個選ぶ
                V<pair<int, int>> tri_axis = {{0,0}, {1,0}, {0,1}};
                set<pair<int, int>> used = {{0,0}, {1,0}, {0,1}};
                set<pair<int, int>> candidate;
                map<pair<int, int>, int> nxt_cnt_map;
                rep(i,0,3){
                    auto [sx, sy] = tri_axis[i];
                    // 隣接座標は (sx+1,sy), (sx,sy+1), (sx-1,sy), (sx,sy-1), (sx+1,sy-1), (sx-1,sy+1)
                    for(auto [dx, dy] : V<pair<int, int>>{{1,0}, {0,1}, {-1,0}, {0,-1}, {1,-1}, {-1,1}}){
                        int nx = sx + dx, ny = sy + dy;
                        nxt_cnt_map[{nx,ny}]++;
                        if(!used.count({nx,ny}) && nxt_cnt_map[{nx,ny}] == 2) candidate.insert({nx,ny});
                    }
                }
                rep(li,3,L){
                    // candidatesからランダムに選ぶ
                    auto it = candidate.begin();
                    advance(it, rnd(candidate.size()));
                    auto [sx, sy] = *it;
                    tri_axis.push_back({sx, sy});
                    used.insert({sx, sy});
                    candidate.erase({sx, sy});
                    // 隣接座標は (sx+1,sy), (sx,sy+1), (sx-1,sy), (sx,sy-1), (sx+1,sy-1), (sx-1,sy+1)
                    for(auto [dx, dy] : V<pair<int, int>>{{1,0}, {0,1}, {-1,0}, {0,-1}, {1,-1}, {-1,1}}){
                        int nx = sx + dx, ny = sy + dy;
                        nxt_cnt_map[{nx,ny}]++;
                        if(!used.count({nx,ny}) && nxt_cnt_map[{nx,ny}] == 2) candidate.insert({nx,ny});
                    }
                }
                // 三角座標系で目的座標が全て選べたので、実際の座標で近い頂点を選んでいく。(0,0)に当てはめる頂点はroot_idxで固定
                V<int> verts = {root_idx};
                set<int> used_verts = {root_idx};
                rep(i,1,L){
                    auto vec = (vec0 * tri_axis[i].first) + (vec1 * tri_axis[i].second);
                    Geometry::Pos target = v0 + vec;
                    
                    int best_idx = -1;
                    float best_score = 1e9;
                    rep(j,0,N){
                        if(ignore_verts[j]) continue;
                        if(!enough_std[j]) continue;
                        if(i < 3 && init_triangle_used[root_idx][j]) continue;
                        if(used_verts.count(j)) continue;

                        Geometry::Pos p = {X[j], Y[j]};
                        auto dist = Geometry::dist(target, p);
                        // float score = dist * min<float>(1.0, (float)dist / (STD[j]+1)) / W + rnd.rand() * 0.1; // TODO : add gumbel noise
                        // float score = dist * min<float>(1.0, (float)dist / (STD[j]+1) ) / W + gumbel_sampler.sample() * 0.01;
                        float score = dist * min<float>(1.0, (float)dist / pow((STD[j]+1), 1.0) ) / W + gumbel_sampler.sample() * 0.01;
                        // float score = dist;
                        if(chmin(best_score, score)) best_idx = j;
                    }
                    // debug(target.x, target.y, best_idx, X[best_idx], Y[best_idx]);
                    verts.push_back(best_idx);
                    used_verts.insert(best_idx);
                }
                // クエリを投げる
                auto judge_res = judge.send_query(verts);
                auto edge_compare_infos = convert_query_results_to_shortest_edge_info(judge_res, verts);
                res.insert(res.end(), all(edge_compare_infos));

                // 現在のX,Yでのクラスカル結果とクエリ結果を見比べて、使用の可否が間違えている辺を元に、X,Yを更新する
                static int dx_sum[N], dy_sum[N];
                rep(t,0,10){
                    for(auto v : verts) dx_sum[v] = 0, dy_sum[v] = 0;

                    auto local_res = mst::Solver().solve(verts, X, Y);
                    // 正の辺は近づくべき、負の辺は遠ざかるべき
                    map<pair<int, int>, int> edge_use_cnt;
                    for(auto& [u, v] : local_res.edges) edge_use_cnt[{u, v}]--;
                    for(auto& [u, v] : judge_res) edge_use_cnt[{u, v}]++;

                    for(auto& [edge, cnt] : edge_use_cnt){
                        if(cnt == 0) continue;
                        auto [u, v] = edge;
                        // X,Yを更新する
                        Geometry::Vec uv = {(float)X[v] - X[u], (float)Y[v] - Y[u]};
                        int len = sqrt(uv.x * uv.x + uv.y * uv.y);
                        if(len == 0) continue;
                        // int cor_len = max<int>(30, min<int>(len * 0.1, 100));
                        int cor_len = 100;
                        int dx = uv.x * cor_len / len;
                        int dy = uv.y * cor_len / len;
                        dx_sum[u] += dx * cnt, dy_sum[u] += dy * cnt;
                        dx_sum[v] -= dx * cnt, dy_sum[v] -= dy * cnt;
                    }
                    float move_sum = 0;
                    for(auto v : verts){
                        move_sum += sqrt(dx_sum[v] * dx_sum[v] + dy_sum[v] * dy_sum[v]);
                        X[v] += dx_sum[v], Y[v] += dy_sum[v];
                        // LX, RXでclamp
                        X[v] = max(LX[v], min(X[v], RX[v])), Y[v] = max(LY[v], min(Y[v], RY[v]));
                    }
                    // debug(t, move_sum);
                    // if(t == 0) debug(t, move_sum);
                    if(move_sum < 0.1) break;
                }
                
                query_verts_log.push_back(verts);
                query_results_log.push_back(judge_res);
                // これまでのクエリについても、クエリ結果を見比べてX,Yを更新する
                int loop_cnt = 10;
                rep(t,0,loop_cnt){
                    rep(i,0,query_verts_log.size()){
                        auto& verts = query_verts_log[i];
                        auto& judge_res = query_results_log[i];
                        for(auto v : verts) dx_sum[v] = 0, dy_sum[v] = 0;

                        auto local_res = mst::Solver().solve(verts, X, Y);
                        // 正の辺は近づくべき、負の辺は遠ざかるべき
                        map<pair<int, int>, int> edge_use_cnt;
                        for(auto& [u, v] : local_res.edges) edge_use_cnt[{u, v}]--;
                        for(auto& [u, v] : judge_res) edge_use_cnt[{u, v}]++;

                        for(auto& [edge, cnt] : edge_use_cnt){
                            if(cnt == 0) continue;
                            auto [u, v] = edge;
                            // X,Yを更新する
                            Geometry::Vec uv = {(float)X[v] - X[u], (float)Y[v] - Y[u]};
                            int len = sqrt(uv.x * uv.x + uv.y * uv.y);
                            if(len == 0) continue;
                            int cor_len = max<int>(30, min<int>(len * 0.1, 100));
                            int dx = uv.x * cor_len / len;
                            int dy = uv.y * cor_len / len;
                            dx_sum[u] += dx * cnt, dy_sum[u] += dy * cnt;
                            dx_sum[v] -= dx * cnt, dy_sum[v] -= dy * cnt;
                        }
                        float move_sum = 0;
                        for(auto v : verts){
                            move_sum += sqrt(dx_sum[v] * dx_sum[v] + dy_sum[v] * dy_sum[v]);
                            X[v] += dx_sum[v], Y[v] += dy_sum[v];
                            // LX, RXでclamp
                            X[v] = max(LX[v], min(X[v], RX[v])), Y[v] = max(LY[v], min(Y[v], RY[v]));
                        }
                    }
                }

                // 使用した頂点のSTDを下げる
                rep(i,0,L) STD[verts[i]] /= 1.4;
                rep(i,0,L) selected_cnts[verts[i]]++;
            }

            V<int> perm; rep(i,0,N) perm.push_back(i);
            rep(i,0,N) STD[i] = sqrt(WX[i] * WX[i] + WY[i] * WY[i]) / 2;
            sort(all(perm), [&](int a, int b){ return STD[a] < STD[b]; });
            
            // rep(i,0,8){
            //     rep(j,0,100) cerr << selected_cnts[perm[i * 100 + j]] << " ";
            //     cerr << endl;
            // }
            int one_g_cnt = 0;
            rep(i,0,M) if(G[i] == 1) one_g_cnt++;
            debug(one_g_cnt);
            // exit(0);
            rep(i,0,N) pred_pos[i].x = X[i], pred_pos[i].y = Y[i];
            return get_unique_edge_compare_info(res);
        }

        V<EdgeCompareInfo> vert3_process(){
            V<EdgeCompareInfo> res;

            int X[N], Y[N];
            rep(i,0,N) X[i] = (LX[i] + RX[i]) / 2, Y[i] = (LY[i] + RY[i]) / 2;
            float STD[N];
            rep(i,0,N) STD[i] = sqrt(WX[i] * WX[i] + WY[i] * WY[i]) / 2;
            V<int> target_query_cnt(N, 0);
            int total_select_cnt = L * Q;
            priority_queue<pair<int, int>> pq;
            rep(i,0,N) if(!ignore_verts[i]) pq.push({WX[i] * WX[i] + WY[i] * WY[i], i});
            rep(i,0,total_select_cnt){
                auto [var, idx] = pq.top(); pq.pop();
                target_query_cnt[idx]++;
                pq.push({var * 0.5, idx});
            }

            V<int> all_verts;
            rep(i,0,N)rep(_,0,target_query_cnt[i]) all_verts.push_back(i);
            
            
            int sa_vert_idx = 0;
            int loop_cnt = 4;
            rep(t,0,loop_cnt){
                V<int> sa_verts;
                rep(i,0,all_verts.size()/loop_cnt){
                    swap(all_verts[sa_vert_idx], all_verts[rnd(all_verts.size() - sa_vert_idx) + sa_vert_idx]);
                    sa_verts.push_back(all_verts[sa_vert_idx]);
                    sa_vert_idx++;
                }

                Geometry::Pos pred_pos[N];
                rep(i,0,N) pred_pos[i] = {X[i], Y[i]};

                auto sa_solver = vert3_sa::Solver();
                auto state = sa_solver.run(sa_verts, pred_pos);

                rep(i,0,state.tuple_size){
                    V<int> query_verts;
                    rep(j,0,3) query_verts.push_back(state.tuples[i][j]);
                    auto judge_res = judge.send_query(query_verts);
                    auto edge_compare_infos = convert_query_results_to_shortest_edge_info(judge_res, query_verts);
                    res.insert(res.end(), all(edge_compare_infos));

                    // 現在のX,Yでのクラスカル結果とクエリ結果を見比べて、使用の可否が間違えている辺を元に、X,Yを更新する
                    static int dx_sum[N], dy_sum[N];
                    rep(_,0,10){
                        for(auto v : query_verts) dx_sum[v] = 0, dy_sum[v] = 0;

                        auto local_res = mst::Solver().solve(query_verts, X, Y);
                        // 正の辺は近づくべき、負の辺は遠ざかるべき
                        map<pair<int, int>, int> edge_use_cnt;
                        for(auto& [u, v] : local_res.edges) edge_use_cnt[{u, v}]--;
                        for(auto& [u, v] : judge_res) edge_use_cnt[{u, v}]++;

                        for(auto& [edge, cnt] : edge_use_cnt){
                            if(cnt == 0) continue;
                            auto [u, v] = edge;
                            // X,Yを更新する
                            Geometry::Vec uv = {(float)X[v] - X[u], (float)Y[v] - Y[u]};
                            int len = sqrt(uv.x * uv.x + uv.y * uv.y);
                            if(len == 0) continue;
                            // int cor_len = max<int>(30, min<int>(len * 0.1, 100));
                            int cor_len = 100;
                            int dx = uv.x * cor_len / len;
                            int dy = uv.y * cor_len / len;
                            dx_sum[u] += dx * cnt, dy_sum[u] += dy * cnt;
                            dx_sum[v] -= dx * cnt, dy_sum[v] -= dy * cnt;
                        }
                        float move_sum = 0;
                        for(auto v : query_verts){
                            move_sum += sqrt(dx_sum[v] * dx_sum[v] + dy_sum[v] * dy_sum[v]);
                            X[v] += dx_sum[v], Y[v] += dy_sum[v];
                            // LX, RXでclamp
                            X[v] = max(LX[v], min(X[v], RX[v])), Y[v] = max(LY[v], min(Y[v], RY[v]));
                        }
                        // debug(t, move_sum);
                        // if(t == 0) debug(t, move_sum);
                        if(move_sum < 0.1) break;
                    }
                    
                    query_verts_log.push_back(query_verts);
                    query_results_log.push_back(judge_res);
                    // これまでのクエリについても、クエリ結果を見比べてX,Yを更新する
                    int loop_cnt = 10;
                    rep(_,0,loop_cnt){
                        rep(qi,0,query_verts_log.size()){
                            auto& verts = query_verts_log[qi];
                            auto& judge_res = query_results_log[qi];
                            for(auto v : verts) dx_sum[v] = 0, dy_sum[v] = 0;

                            auto local_res = mst::Solver().solve(verts, X, Y);
                            // 正の辺は近づくべき、負の辺は遠ざかるべき
                            map<pair<int, int>, int> edge_use_cnt;
                            for(auto& [u, v] : local_res.edges) edge_use_cnt[{u, v}]--;
                            for(auto& [u, v] : judge_res) edge_use_cnt[{u, v}]++;

                            for(auto& [edge, cnt] : edge_use_cnt){
                                if(cnt == 0) continue;
                                auto [u, v] = edge;
                                // X,Yを更新する
                                Geometry::Vec uv = {(float)X[v] - X[u], (float)Y[v] - Y[u]};
                                int len = sqrt(uv.x * uv.x + uv.y * uv.y);
                                if(len == 0) continue;
                                int cor_len = max<int>(30, min<int>(len * 0.1, 100));
                                int dx = uv.x * cor_len / len;
                                int dy = uv.y * cor_len / len;
                                dx_sum[u] += dx * cnt, dy_sum[u] += dy * cnt;
                                dx_sum[v] -= dx * cnt, dy_sum[v] -= dy * cnt;
                            }
                            float move_sum = 0;
                            for(auto v : verts){
                                move_sum += sqrt(dx_sum[v] * dx_sum[v] + dy_sum[v] * dy_sum[v]);
                                X[v] += dx_sum[v], Y[v] += dy_sum[v];
                                // LX, RXでclamp
                                X[v] = max(LX[v], min(X[v], RX[v])), Y[v] = max(LY[v], min(Y[v], RY[v]));
                            }
                        }
                    }
                }
            }

            return get_unique_edge_compare_info(res);
        }

        V<EdgeCompareInfo> run(){
            // return create_triangle_tile_query();
            // return create_around_pos_query(Q);
            if(L == 3 && W > 900) return vert3_process();
            if(L == 4 && W > 2000) return create_triangle_tile_query();
            return create_around_pos_query(Q);
        }
    };

    struct SA{
        struct State{
            Geometry::Pos pos[N];
            // 移動平均Pos(Exponential Moving Average)
            Geometry::Pos ema_pos[N];
            static constexpr float ema_alpha = 0.03;
            int score; // クエリ結果との誤差の総和
        };
        
        struct StaticData{
            // 頂点iを含むクエリのリスト
            V<EdgeCompareInfo> contain_edge_compare_infos[N];
        };
        StaticData data;

        
        struct Transition{
            int types[2000];
            int type_size = 0;
            
            long long try_cnt[100];
            long long accept_cnt[100];
            const int TransitionTypeSize = 2;
    
            Transition(){
                rep(i,0,1) types[type_size++] = 0;
                rep(i,0,10) types[type_size++] = 1;
            }
    
            void set_transition_fix(int type){
                type_size = 0;
                types[type_size++] = type;
            }
    
            void log_accept_rate(){
                cerr << "accept rate " << endl;
                rep(i,0,TransitionTypeSize){
                    cerr << i << ": " << accept_cnt[i] << " / " << try_cnt[i] << " (" << accept_cnt[i] * 100 / max(1ll,try_cnt[i]) << "%)" << endl;
                }
                cerr << endl;
            }
    
            void reset_accept_log(){
                rep(i,0,TransitionTypeSize){
                    try_cnt[i] = 0;
                    accept_cnt[i] = 0;
                }
            }
    
            bool execute(State& state, StaticData& data, float current_temp){
                int type = types[rnd() % type_size];
                bool res;
                switch(type){
                    case 0: res = move_one_random(state, data, current_temp); break;
                    case 1: res = move_one_near(state, data, current_temp); break;
                }
                try_cnt[type]++;
                if(res) accept_cnt[type]++;
                return res;
            }
    
            bool should_accept_transition(float score_diff, float current_temp){
                return score_diff > 0 || rnd.rand() < exp(score_diff / current_temp);
            }

            bool move_vert_pos(State& state, StaticData& data, int idx, Geometry::Pos new_pos, float current_temp){
                if(data.contain_edge_compare_infos[idx].size() == 0) return false;

                Geometry::Pos prev = state.pos[idx];
                int score_diff = 0;
                for(auto& eci : data.contain_edge_compare_infos[idx]){
                    score_diff += eci.CalcError(state.pos);
                }
                
                state.pos[idx] = new_pos;

                for(auto& eci : data.contain_edge_compare_infos[idx]){
                    score_diff -= eci.CalcError(state.pos);
                }
                
                if(should_accept_transition(score_diff, current_temp)){
                    state.score += score_diff;
                    state.ema_pos[idx] = state.pos[idx] * State::ema_alpha + (state.ema_pos[idx] * (1 - State::ema_alpha));
                    return true;
                }else{
                    state.pos[idx] = prev;
                    return false;
                }
            }

            // 1頂点をランダムに選び、その座標をランダムに変更する
            bool move_one_random(State& state, StaticData& data, float current_temp){
                int idx = rnd(N);
                Geometry::Pos new_pos = {(int)rnd(LX[idx], RX[idx]+1), (int)rnd(LY[idx], RY[idx]+1)};
                return move_vert_pos(state, data, idx, new_pos, current_temp);
            }

            // 1頂点をランダムに選び、その座標を近くに変更する(+-W/5くらい。端っこClampしないよう、抽選区域をずらす)
            int NearWidth;
            bool move_one_near(State& state, StaticData& data, float current_temp){
                int idx = rnd(N);
                
                Geometry::Pos cur_pos = state.pos[idx];
                int lx = max(LX[idx], cur_pos.x - NearWidth), rx = min(RX[idx], cur_pos.x + NearWidth);
                int ly = max(LY[idx], cur_pos.y - NearWidth), ry = min(RY[idx], cur_pos.y + NearWidth);
                Geometry::Pos new_pos = {(int)rnd(lx, rx+1), (int)rnd(ly, ry+1)};
                return move_vert_pos(state, data, idx, new_pos, current_temp);
            }

            void set_near_width(int l, int w){
                float frac = 1;
                if(l <= 8) frac = l-2;
                else if(l <= 13) frac = 6 + 2 * (l-8);
                else if(l == 14) frac = 17;
                else if(l == 15) frac = 18;

                if(l >= 5){
                    frac *= 1 + 0.3 * (float)(w - 1250) / 2500;
                }
                NearWidth = max<int>(1, w / frac);
            }
        } transition;
    
        struct Temp{
            float start_temp = 500;
            float mid_temp = 2000;
            float end_temp = 0.1;
            
            float get_temp(float progress){
                // float threshold = 0.3;
                // if(progress < threshold)
                //     return pow(start_temp, 1.0 - progress / threshold) * pow(mid_temp, progress / threshold);
                // else
                //     return mid_temp + (end_temp - mid_temp) * (progress - threshold) / (1.0 - threshold);
                return pow(start_temp, 1.0 - progress) * pow(end_temp, progress);
                // return start_temp + (end_temp - start_temp) * progress;
            }
        } temp;

        SA(V<EdgeCompareInfo>& edge_compare_infos){
            rep(i,0,N) data.contain_edge_compare_infos[i].clear();
            for(auto& eci : edge_compare_infos){
                set<int> verts;
                rep(i,0,2) verts.insert(eci.shorter_edge[i]), verts.insert(eci.longer_edge[i]);
                for(auto v : verts) data.contain_edge_compare_infos[v].push_back(eci);
            }
            transition.set_near_width(L, W);
        }
    
    
        State run(int time_limit, State state){
            timer.mark_lap();
    
            State best_state = state;
            float current_temp = temp.start_temp;
            int loop_cnt = 0;
            int accept_cnt = 0;
            while(timer.lap_ms() < time_limit){
                float progress = (float)timer.lap_ms() / time_limit;
                current_temp = temp.get_temp(progress);
                rep(_,0,1024){
                    loop_cnt++;
                    bool accept = transition.execute(state, data, current_temp);
                    accept_cnt += accept;
    
                    if(accept && state.score > best_state.score){
                        // debug(loop_cnt, current_temp, state.score);
                        best_state = state;
                    }
                }
            }
            debug(loop_cnt, state.score, best_state.score);
            cerr << "loop_cnt: " << loop_cnt << ", accept_rate: " << (float)accept_cnt / loop_cnt << endl;
            // return best_state;
            return state;
        }
    };

    struct Solver{
        SA::State init_state(V<EdgeCompareInfo>& edge_compare_infos, Geometry::Pos pred_pos[N]){
            SA::State state;
            // rep(i,0,N) state.pos[i] = {(LX[i] + RX[i]) / 2, (LY[i] + RY[i]) / 2}, state.ema_pos[i] = state.pos[i];
            rep(i,0,N) state.pos[i] = pred_pos[i], state.ema_pos[i] = pred_pos[i];
            state.score = 0;
            for(auto& eci : edge_compare_infos) state.score -= eci.CalcError(state.pos);
            return state;
        }

        SA::State run(){
            Geometry::Pos pred_pos[N];
            rep(i,0,N) pred_pos[i] = {(LX[i] + RX[i]) / 2, (LY[i] + RY[i]) / 2};
            QueryBuilder query_builder(pred_pos);
            auto query_result = query_builder.run();
            
            auto state = init_state(query_result, query_builder.pred_pos);
            SA sa(query_result);
            state = sa.run(time_limit - timer.elapsed_ms() - 300, state);

            return state;
        }
    };
}

namespace group_division{
    // 推定したX,Y座標を真として扱う
    int X[N], Y[N];
    int Dist[N][N];

    int quota_size_cnts[N+1]; // quota_sizes[i] := グループのサイズがiのクエリの数
    VV<int> res;

    /**
     * @brief 全頂点を使ったクラスカル木を構築し、適当な深いleafを根とした根付き木として見て、quotaの大きさにマッチした部分木を切り離していく
     */
    struct SubTreeCutSolver{
        mst::MstRootedTree tree;
        bool unused[N]; // 木に残っているか

        SubTreeCutSolver(){
            // ignore_vertsを除いた頂点を使ったクラスカル木を構築
            rep(i,0,N) unused[i] = !ignore_verts[i];
            V<int> verts;
            rep(i,0,N) if(unused[i]) verts.push_back(i);
            auto mst_res = mst::Solver().solve(verts, Dist);
            tree = mst::Solver().build_mst_rooted_tree(mst_res, Dist);
        }

        int get_threshold_edge_len(int rest_cut_cnt){
            // 残っている辺の中で、rest_group_cnt番目に長い辺を取得する
            priority_queue_rev<pair<int, pair<int, int>>> pq;
            rep(i,0,N) if(unused[i] && tree.parent[i] != -1){
                pq.push({tree.parent_dist[i], {tree.parent[i], i}});
                if(pq.size() > rest_cut_cnt) pq.pop();
            }
            if(pq.size() == 0) return INT_MAX;
            return pq.top().first;
        }
        
        VV<int> run(){
            // VV<int> res;
            while(res.size() < M-1){
                int threshold_edge_len = get_threshold_edge_len(M - res.size() - 1);
                int min_group_size = INT_MAX;
                drep(i,N,1) if(quota_size_cnts[i] > 0) min_group_size = i;

                // dfsで切り取る部分木を決める。
                int cut_v = -1, max_score = INT_MIN;
                
                function<int(int)> dfs = [&](int v){
                    if(tree.child_size[v] < min_group_size-1) return 0;

                    if(!unused[v]) return 0;
                    int child_score = 0;
                    for(auto nxt : tree.childs[v]){
                        if(!unused[nxt]) continue;
                        child_score += dfs(nxt);
                    }
                    
                    // best_score比較
                    int score = child_score + tree.parent_dist[v] - threshold_edge_len;
                    if(quota_size_cnts[tree.child_size[v]] > 0 && chmax(max_score, score)) cut_v = v;

                    // 親に部分木のscoreを渡す
                    int subtree_score = child_score - max<int>(0, tree.parent_dist[v] - threshold_edge_len);
                    return subtree_score;
                };
                dfs(tree.root);

                if(cut_v == -1) break;

                
                // cut_vを切り取る
                quota_size_cnts[tree.child_size[cut_v]]--;
                V<int> verts;
                queue<int> q;
                q.push(cut_v);
                while(!q.empty()){
                    int v = q.front(); q.pop();
                    verts.push_back(v);
                    unused[v] = false;
                    for(auto nxt : tree.childs[v]) q.push(nxt);
                }
                res.push_back(verts);
                // cut_vの親の子供からcut_vを削除
                tree.childs[tree.parent[cut_v]].erase(cut_v);
                // cut_vの親の子供のサイズを更新
                int p = tree.parent[cut_v];
                while(p != -1){
                    tree.child_size[p] -= tree.child_size[cut_v];
                    p = tree.parent[p];
                }
            }
            if(res.size() == M-1){
                // 1つの部分木にまとめる
                V<int> verts;
                rep(i,0,N) if(unused[i]) verts.push_back(i), unused[i] = false;
                res.push_back(verts);
                quota_size_cnts[verts.size()]--;
            }

            return res;
        }
    };

    /**
     * @brief ある頂点の塊を初期値として、目的のquota_sizeになるまで貪欲に頂点を追加していくSolver
     * 初期値の頂点の塊は、単純に部分木 or 部分木2つを最短距離で繋げたもの
     */
    struct FallBackSolver{
        V<int> quota_sizes;
        V<int> g[N];
        set<int> tree_verts;
        int depth[N];

        FallBackSolver(V<int> quota_sizes, bool unused[N]){
            this->quota_sizes = quota_sizes;
            
            rep(i,0,N) if(unused[i]) tree_verts.insert(i);
            V<int> verts(all(tree_verts));
            auto mst_res = mst::Solver().solve(verts, Dist);
            for(auto [u, v] : mst_res.edges){
                g[u].push_back(v);
                g[v].push_back(u);
            }
            // depthをbfsで求める
            memset(depth, -1, sizeof(depth));
            if(tree_verts.size() == 0) return;

            queue<int> q;
            q.push(verts[0]);
            depth[0] = 0;
            while(!q.empty()){
                int v = q.front(); q.pop();
                for(auto nxt : g[v]){
                    if(depth[nxt] != -1) continue;
                    depth[nxt] = depth[v] + 1;
                    q.push(nxt);
                }
            }
        }

        /**
         * @brief 貪欲法の初期値の頂点リストを求める
         * @return pair<int, V<int>> {目的のquota_size, 初期の頂点リスト}
         */
        pair<int, V<int>> find_best_init_verts(V<int> verts, int quota_size_cnts[N]){
            auto mst_res = mst::Solver().solve(verts, Dist);
            auto tree = mst::Solver().build_mst_rooted_tree(mst_res, Dist);
            
            // {cost, subTree0-root, subTree1-root}
            array<int, 3> best_costs[N+1];
            rep(i,0,N+1) best_costs[i] = {INT_MAX, -1, -1};
            
            for(auto v : verts)if(v != tree.root){
                best_costs[tree.child_size[v]] = {-tree.parent_dist[v], v, -1};
            }



            // best_costsの中で最も良いものを選ぶ
            int target_quota_size = -1, best_target_quota_size = -1, best_score = INT_MIN;
            array<int, 3> best_set = {INT_MAX, -1, -1};
            drep(i,N,0){
                if(quota_size_cnts[i] > 0) target_quota_size = i;
                if(target_quota_size == -1) continue;
                if(best_costs[i][0] == INT_MAX) continue;

                int score = -best_costs[i][0] - (target_quota_size - i) * 100;
                if(chmax(best_score, score)) best_target_quota_size = target_quota_size, best_set = best_costs[i];
            }

            V<int> init_verts;
            rep(i,1,3) if(best_set[i] != -1){
                queue<int> q;
                q.push(best_set[i]);
                while(!q.empty()){
                    int v = q.front(); q.pop();
                    init_verts.push_back(v);
                    for(auto nxt : tree.childs[v]) q.push(nxt);
                }
            }
            return {best_target_quota_size, init_verts};
        }
        
        
        VV<int> run(){
            // VV<int> res;
            rep(t,0,(int)quota_sizes.size()-1){
                debug(t, quota_sizes.size());
                V<int> rest_verts(all(tree_verts));
                auto [sz, verts] = find_best_init_verts(rest_verts, quota_size_cnts);
                for(auto v : verts) tree_verts.erase(v);
                // vertsの隣接点の接続辺を削除
                for(auto v : verts) for(auto nxt : g[v]) g[nxt].erase(find(all(g[nxt]), v));
                V<pair<int, int>> nearests(N, {INT_MAX, -1}); // nearests[i] := {最短の距離, iに最も近いverts内頂点}
                for(auto v : verts) for(auto tv : tree_verts) chmin(nearests[tv], {Dist[v][tv], v});
                
                while(verts.size() < sz){
                    int min_cost = 1e9, min_v = -1;
                    mst::Result min_mst_res;
                    for(auto v : tree_verts){
                        int u = nearests[v].second;
                        // コスト = uとvの距離 - (vとvの隣接点の距離の総和) + (vの隣接点をクラスカルで繋ぎ直した場合の距離の総和)
                        int cost = Dist[u][v];
                        for(auto vnxts : g[v]) cost -= Dist[v][vnxts];
                        auto mst_res = mst::Solver().solve(g[v], Dist);
                        for(auto& [u, v] : mst_res.edges) cost += Dist[u][v];
                        if(chmin(min_cost, cost)) min_v = v, min_mst_res = mst_res;
                    }
                    verts.push_back(min_v);
                    tree_verts.erase(min_v);
                    for(auto v : tree_verts) chmin(nearests[v], {Dist[v][min_v], min_v});
                    // min_vの隣接点の接続辺を削除して、mst_resの辺を追加
                    for(auto nxt : g[min_v]) g[nxt].erase(find(all(g[nxt]), min_v));
                    for(auto [u, v] : min_mst_res.edges) g[u].push_back(v), g[v].push_back(u);
                }
                res.push_back(verts);
                quota_size_cnts[sz]--;
            }
            // 残ったvertsを１グループとして追加
            V<int> verts(all(tree_verts));
            res.push_back(verts);
            return res;
        }
    };

    void output(VV<int>& groups){
        // Gに従ってvertsの出力順を決める
        V<queue<int>> group_idxs(N+1);
        rep(i,0,groups.size()) group_idxs[groups[i].size()].push(i);
        V<int> perm;
        rep(i,0,M) perm.push_back(group_idxs[G[i]].front()), group_idxs[G[i]].pop();
        
        int local_score = 0;
        cout << "!" << endl;
        for(auto gi : perm){
            V<int> verts = groups[gi];
            auto mst_res = mst::Solver().solve(verts, Dist);
            for(auto v : verts) cout << v << " \n"[v == verts.back()];
            for(auto [u, v] : mst_res.edges) cout << u << " " << v << endl;
            local_score += mst_res.total_len;
        }
        cerr << "local_score: " << local_score << endl;
    }

    struct Solver{
        Solver(int predictX[N], int predictY[N]){
            rep(i,0,N) X[i] = predictX[i], Y[i] = predictY[i];

            bool dist_correction = L <= 3;
            if(L==4 && W > 1500) dist_correction = true;
            if(L==5 && W > 2000) dist_correction = true;

            rep(i,0,N)rep(j,0,N){
                Dist[i][j] = (int) sqrt((X[i] - X[j]) * (X[i] - X[j]) + (Y[i] - Y[j]) * (Y[i] - Y[j]));
                // TODO: 移動平均分散を導入したらそれに置き換える
                if(dist_correction) Dist[i][j] += (max(WX[i], WY[i]) + max(WX[j], WY[j])) * 0.1;
            }
            memset(quota_size_cnts, 0, sizeof(quota_size_cnts));
            rep(i,0,M) quota_size_cnts[G[i]]++;
        }

        void assign_ignore_verts_to_size_1(){
            rep(i,0,N) if(ignore_verts[i]){
                assert(quota_size_cnts[1] > 0);
                quota_size_cnts[1]--;
                res.push_back({i});
            }
        }

        void run(){
            res.clear();
            assign_ignore_verts_to_size_1();

            SubTreeCutSolver subtree_cut_solver;
            subtree_cut_solver.run();

            // fallback
            V<int> rest_quota_sizes;
            rep(i,0,N+1) rep(_,0,quota_size_cnts[i]) rest_quota_sizes.push_back(i);
            FallBackSolver fallback_solver(rest_quota_sizes, subtree_cut_solver.unused);
            fallback_solver.run();
            
            output(res);
        }
    };
}

struct Solver{
    void run(){
        // 分散の大きい頂点をsize=1のグループに詰める前提で除外する（L,Wによっては実行しない）
        ignore_verts_preprocess::run();

        // 頂点の位置を推定
        auto position_solver = position_estimation::Solver();
        auto state = position_solver.run();
        // rep(i,0,N){
        //     debug(abs(state.pos[i].x - state.ema_pos[i].x), abs(state.pos[i].y - state.ema_pos[i].y), WX[i], WY[i]);
        // }
        int predictX[N], predictY[N];
        rep(i,0,N) predictX[i] = state.ema_pos[i].x, predictY[i] = state.ema_pos[i].y;

        // グループ分割
        auto group_solver = group_division::Solver(predictX, predictY);
        group_solver.run();
    }
};

void input(){
    int _; cin >> _ >> M >> _ >> L >> W; // N,M,Q,L,W
    rep(i,0,M) cin >> G[i];
    rep(i,0,N) cin >> LX[i] >> RX[i] >> LY[i] >> RY[i];
    rep(i,0,N) WX[i] = RX[i] - LX[i], WY[i] = RY[i] - LY[i];
    // NOTE: [l, r)の方が扱いやすいので
    rep(i,0,N) RX[i]++, RY[i]++;
    judge.init(); // LocalJudge用の初期化
}

int main(int argc, char *argv[])
{
#if DEBUG
    freopen("tools/in/0045.txt", "r", stdin);
#endif
    input();
    
    Solver solver;
    solver.run();
    cerr << "elapsed time: " << timer.elapsed_ms() << "ms" << endl;
}