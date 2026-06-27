#include <bits/stdc++.h>

using namespace std;
using ll = long long;

constexpr int INF = 1e9;
constexpr int N_MAX = 600;
constexpr int HG_MAX = 400;
int N,M,T,LA,LB;
vector<vector<int>> G;
vector<bitset<N_MAX>> Gmatrix;
vector<int> t;
bitset<N_MAX> set_t;
int HG_size;

vector<int> X,Y;

unsigned int randxor()
{
    static unsigned int x=123456789,y=362436069,z=521288629,w=88675123;
    unsigned int t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}


void input() {
    cin >> N >> M >> T >> LA >> LB;
    G.resize(N);
    Gmatrix.resize(N);
    for (int i = 0; i < M; ++i) {
        int u,v;
        cin >> u >> v;
        G[u].emplace_back(v);
        G[v].emplace_back(u);
        Gmatrix[u].set(v);
        Gmatrix[v].set(u);
    }
    t.resize(N);
    for (int i = 0; i < N; ++i) cin >> t[i];
    X.resize(N);
    Y.resize(N);
    for (int i = 0; i < N; ++i) cin >> X[i] >> Y[i];

    for (int i = 0; i < N; ++i) set_t.set(t[i]);
    set_t.set(0);
    HG_size = (LA+LB-1)/LB;
}

double calcEuclid(int i, int j) {
    int dx = abs(X[i]-X[j]);
    int dy = abs(Y[i]-Y[j]);
    return sqrt(dx*dx + dy*dy);
}

int calcEuclid2(int i, int j) {
    int dx = abs(X[i]-X[j]);
    int dy = abs(Y[i]-Y[j]);
    return dx*dx + dy*dy;
}


// 時間を管理するクラス
class TimeKeeper {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    int64_t time_threshold;

public:
    // 時間制限をミリ単位で指定してインスタンスを作る
    TimeKeeper(const int64_t &time_threshold_) 
     : start_time(std::chrono::high_resolution_clock::now()),
       time_threshold(time_threshold_) {}
    
    // インスタンス生成した時から指定した時間制限を超過したか判定する
    bool isTimeOver() const {
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;
        auto diff = std::chrono::high_resolution_clock::now() - this->start_time;
        return duration_cast<milliseconds>(diff).count() >= time_threshold;
    }

    // 経過時間を返す
    int64_t elapsedTime() const {
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;
        auto diff = std::chrono::high_resolution_clock::now() - this->start_time;
        return duration_cast<milliseconds>(diff).count();
    }

    int64_t getLimit() const { return time_threshold; }
};


// 焼きなまし法で配列Aの初期解を求める
namespace InitA {


/**
 * 関節点と橋をO(E + V)で求めるアルゴリズム.
 */
class LowLink {
    /**
    * 訪問したことがある頂点かどうか.
    */
    bitset<N_MAX> visited;
    /**
    * 順序.
    */
    vector<int> order;
    /**
    * 配列low. low[親] < low[子]なら、ブリッジとなる。
    */
    vector<int> low;

public:
    /**
    * 与えられる木。隣接リスト形式でコストは含まれない.親は-1とする。
    */
    bitset<N_MAX> set_A; 
    /**
    * 関節点　その点を取り除くと全体が不連結になる点.
    */
    bitset<N_MAX> not_articulation_points;

private:

    void dfs(int now, int parent, int& count) {
        visited.set(now);
        order[now] = count;
        low[now] = order[now];
        count++;
        bool is_art = false;/* 関節点であるかどうか */
        int son_count = 0;
        for (int next : G[now]) {
            if (!set_A[next]) continue;
            if (!visited[next]) {
                son_count++;
                dfs(next, now, count);
                if (next != parent) low[now] = min(low[now], low[next]); // ループに行きついたときに、戻る際の伝播
                //関節点か イコールがつくのは、ひとまとまりのグループ内のlow[]は関節点のlow[]と同じ値だから。端はもうこれ以上伸びないので、関節点にカウントしない
                if (parent != -1 && order[now] <= low[next]) is_art = true;
            }
            else {
                //すでに行ったことある頂点=後退辺
                if (next != parent) low[now] = min(low[now], order[next]);// ループに行きついた時にorder[]を代入
            }

        }
        //根は2つ以上の子があるなら、関節点
        if (parent == -1 && son_count >= 2) is_art = true;
        if (!is_art) not_articulation_points.set(now);
    }

public:
    /**
    * 渡すのは隣接リスト形式のグラフ。辺の長さは受け付けない.
    *
    * \param e
    */
    LowLink(const bitset<N_MAX>& set_A) : set_A(set_A) {
        visited.reset();
        order.resize(N_MAX);
        low.resize(N_MAX);
    }

    LowLink() {
        visited.reset();
        order.resize(N_MAX);
        low.resize(N_MAX);
    }

    void init(const bitset<N_MAX>& set_A_) {
        set_A = set_A_;
        visited.reset();
        not_articulation_points.reset();
    }

    /**
    * 計算の際に呼び出す。各連結成分ごとにDFSしている。.
    *
    */
    void build(int i) {
        int count = 0;
        // for (int i = 0; i < N_MAX; ++i) {
        //     if (!set_A[i]) continue;
        //     if (!visited[i]) dfs(i, -1, count);
        // }
        dfs(i,-1,count);
    }
};

// mt19937 mt_random;
class State {
public:
    int score;
    vector<int> A;
    vector<int> cnt;
    vector<bitset<N_MAX>> set_A;
    vector<bitset<N_MAX>> nart_A;
    LowLink lowlink;

    State() {
        score = 0;
        A.resize(LA);
        cnt.resize(N);
        set_A.resize(HG_size);
        nart_A.resize(HG_size);
    }

    State(const State& state) {
        score = state.score;
        A = state.A;
        cnt = state.cnt;
        set_A = state.set_A;
        nart_A = state.nart_A;
        lowlink = state.lowlink;
    }

    // 状態を初期化する
    void init() {
        // t[i]からt[i+1]の最短距離とか適当に入れておく
        vector<vector<int>> X;
        vector<unordered_set<int>> X_Adj;
        vector<bool> used(N);
        queue<int> q;
        q.push(t[0]);
        vector<int> path(N,-1);
        while (!q.empty()) {
            int x = q.front();
            q.pop();

            if (x == 0) break;
            
            for (int y : G[x]) {
                if (path[y] != -1 || y == t[0]) continue;
                path[y] = x;
                q.push(y);
            }
        }
        vector<int> p;
        int start = 0;
        while (path[start] != -1) {
            start = path[start];
            p.emplace_back(start);
        }
        reverse(p.begin(),p.end());
        X.resize((p.size()+LB-1)/LB);
        X_Adj.resize((p.size()+LB-1)/LB);
        for (int i = 0; i < p.size(); ++i) {
            X[i/LB].emplace_back(p[i]);
            for (int j : G[p[i]]) {
                X_Adj[i/LB].insert(j);
            }
            used[p[i]] = true;
        }

        for (int i = 1; i < N; ++i) {
            if (used[t[i]]) continue;

            while (!q.empty()) q.pop();
            q.push(t[i]);
            for (int i = 0; i < N; ++i) path[i] = -1;
            start = -1;
            while (!q.empty()) {
                int x = q.front();
                q.pop();

                if (used[x]) {
                    start = x;
                    break;
                }
                
                for (int y : G[x]) {
                    if (path[y] != -1 || y == t[i]) continue;
                    path[y] = x;
                    q.push(y);
                }
            }

            vector<int> pp;
            while (path[start] != -1) {
                start = path[start];
                pp.emplace_back(start);
            }
            reverse(pp.begin(),pp.end());

            vector<vector<int>> tmp((pp.size()+LB-1)/LB);
            vector<unordered_set<int>> tmp_Adj((pp.size()+LB-1)/LB);
            for (int i = 0; i < pp.size(); ++i) {
                tmp[i/LB].emplace_back(pp[i]);
                for (int j : G[pp[i]]) {
                    tmp_Adj[i/LB].insert(j);
                }
                used[pp[i]] = true;
            }

            for (int i = 0; i < tmp.size(); ++i) {
                if (tmp[i].size() == LB) {
                    X.emplace_back(tmp[i]);
                    X_Adj.emplace_back(tmp_Adj[i]);
                }
                else {
                    bool flag = false;
                    for (int j = 0; j < X.size(); ++j) {
                        if (X[j].size() == LB) continue;
                        bool ok = false;
                        for (int k : tmp[i]) {
                            if (X_Adj[j].count(k)) ok = true;
                        }
                        if (!ok) continue;

                        if (X[j].size() + tmp[i].size() > LB) continue;

                        flag = true;
                        for (int k : tmp[i]) X[j].emplace_back(k);
                        for (int k : tmp_Adj[i]) X_Adj[j].insert(k);
                        break;
                    }

                    if (!flag) {
                        vector<int> dis(N,1e5);
                        vector<int> path(N,-1);
                        queue<int> q;
                        for (int j : tmp[i]) {
                            q.push(j);
                            dis[j] = 0;
                        }
                        while (!q.empty()) {
                            int x = q.front();
                            q.pop();
                            
                            for (int y : G[x]) {
                                if (dis[y] <= dis[x]+1) continue;
                                dis[y] = dis[x]+1;
                                path[y] = x;
                                q.push(y);
                            }
                        }

                        for (int j = 0; j < X.size(); ++j) {
                            if (X[j].size() == LB) continue;

                            int select = -1;
                            int score = 1e5;

                            for (int k : X[j]) {
                                if (dis[k] < score) {
                                    select = k;
                                    score = dis[k];
                                }
                            }

                            if (score-1 + X[j].size() + tmp[i].size() > LB) continue;

                            vector<int> p_tmp;
                            int start = select;
                            while (!tmp_Adj[i].count(start)) {
                                start = path[start];
                                p_tmp.emplace_back(start);
                            }

                            flag = true;
                            for (int k : tmp[i]) X[j].emplace_back(k);
                            for (int k : p_tmp) X[j].emplace_back(k);

                            for (int k : tmp_Adj[i]) X_Adj[j].insert(k);
                            for (int k : p_tmp) {
                                for (int l : G[k]) X_Adj[j].insert(l);
                            }
                             
                            break;
                        }
                    }

                    if (!flag) {
                        X.emplace_back(tmp[i]);
                        X_Adj.emplace_back(tmp_Adj[i]);
                    }
                }
            }
        }

        int z = 0;
        for (int i = 0; i < X.size(); ++i) {
            unordered_set<int> st;
            queue<int> q;
            for (int j : X[i]) {
                q.push(j);
                st.insert(j);
            }
            while (!q.empty()) {
                int x = q.front();
                q.pop();

                if (st.size() == LB) break;
                
                for (int y : G[x]) {
                    if (st.count(y)) continue;
                    if (st.size() == LB) break;
                    st.insert(y);
                    q.push(y);
                }
            }
            X[i].clear();
            while (!q.empty()) q.pop();
            unordered_set<int> st2;
            q.push(*st.begin());
            X[i].emplace_back(*st.begin());
            st2.insert(*st.begin());
            while (!q.empty()) {
                int x = q.front();
                q.pop();
                
                for (int y : G[x]) {
                    if (st2.count(y)) continue;
                    if (!st.count(y)) continue;
                    X[i].emplace_back(y);
                    st2.insert(y);
                    q.push(y);
                }
            }

            for (int j : X[i]) {
                if (z >= LA) break;
                A[z] = j;
                ++z;
            }
        }

        int zz = N-1;
        while (z < LA) {
            queue<int> q;
            q.push(t[zz]);
            vector<int> path(N,-1);
            while (!q.empty()) {
                int x = q.front();
                q.pop();

                if (x == t[zz-1]) break;
                
                for (int y : G[x]) {
                    if (path[y] != -1 || y == t[zz]) continue;
                    path[y] = x;
                    q.push(y);
                }
            }

            vector<int> p;
            int start = t[zz-1];
            p.emplace_back(start);
            while (start != t[zz]) {
                start = path[start];
                p.emplace_back(start);
            }
            reverse(p.begin(),p.end());
            vector<vector<int>> tmp((p.size()+LB-1)/LB);
            for (int i = 0; i < p.size(); ++i) {
                tmp[i/LB].emplace_back(p[i]);
            }

            for (int i = 0; i < tmp.size(); ++i) {
                unordered_set<int> st;
                queue<int> q;
                for (int j : tmp[i]) {
                    q.push(j);
                    st.insert(j);
                }
                while (!q.empty()) {
                    int x = q.front();
                    q.pop();

                    if (st.size() == LB) break;
                    
                    for (int y : G[x]) {
                        if (st.count(y)) continue;
                        if (st.size() == LB) break;
                        st.insert(y);
                        tmp[i].emplace_back(y);
                        q.push(y);
                    }
                }

                for (int j : tmp[i]) {
                    if (z >= LA) break;
                    A[z] = j;
                    ++z;
                }
            }

            --zz;
            if (zz == 0) zz = N-1;
        }

        for (int i = 0; i < LA; ++i) ++cnt[A[i]];

        for (int i = 0; i < LA; ++i) {
            set_A[i/LB].set(A[i]);
        }

        for (int i = 0; i < HG_size; ++i) {
            lowlink.init(set_A[i]);
            lowlink.build(A[i*LB]);
            nart_A[i] = lowlink.not_articulation_points;
        }

        for (int i = 0; i < N; ++i) {
            if (cnt[i] > 0) {
                if (set_t[i]) score += 1000;
                else score += 1;
            }
        }
    }

    // 遷移する(+score計算)
    int r1,r2;
    int b_v;
    int b_score;
    bitset<N_MAX> nart;
    int cand[N_MAX];
    void transition() {
        b_score = score;
        // r1 = mt_random()%HG_size;
        r1 = randxor()%HG_size;
        int l = (r1 == HG_size-1 && LA%LB) ? LA%LB : LB;

        if (nart_A[r1].none()) {
            lowlink.init(set_A[r1]);
            lowlink.build(A[r1*LB]);
            nart_A[r1] = lowlink.not_articulation_points;
        }

        int cand_num = 0;
        for (int i = 0; i < l; ++i) {
            if (nart_A[r1][A[r1*LB+i]]) {
                cand[cand_num] = i;
                ++cand_num;
            }
        }

        r2 = cand[randxor()%cand_num];
        b_v = A[r1*LB+r2];

        // 選べる頂点を列挙する
        bitset<N_MAX> set_cand;
        for (int i = 0; i < l; ++i) {
            if (i == r2) continue;
            set_cand |= Gmatrix[A[r1*LB+i]];
        }
        set_cand &= ~set_A[r1];
        
        if (set_cand.none()) {
            r2 = 0;
            b_v = A[r1*LB];
            return;
        }

        nart = nart_A[r1];
        nart_A[r1].reset();

        cand_num = 0;
        for (int i = 0; i < N_MAX; ++i) {
            if (!set_cand[i]) continue;
            cand[cand_num] = i;
            ++cand_num;
        }
        int select = cand[randxor()%cand_num];
        A[r1*LB+r2] = select;

        set_A[r1].reset(b_v);
        set_A[r1].set(A[r1*LB+r2]);
        
        --cnt[b_v];
        if (cnt[b_v] == 0) {
            if (set_t[b_v]) score -= 1000;
            else score -= 1;
        }

        if (cnt[A[r1*LB+r2]] == 0) {
            if (set_t[A[r1*LB+r2]]) score += 1000;
            else score += 1;
        }
        ++cnt[A[r1*LB+r2]];
    }

    // 遷移前に戻す(+scoreも元に戻す)
    void back() {
        score = b_score;

        int l = (r1 == HG_size-1 && LA%LB) ? LA%LB : LB;
        int pos = r1*LB + r2;

        if (b_v != A[pos]) nart_A[r1] = nart;

        --cnt[A[pos]];
        set_A[r1].reset(A[pos]);
        A[pos] = b_v;
        set_A[r1].set(A[pos]);
        ++cnt[A[pos]];
    }
};

State simulatedAnnealing(double start_temp, double end_temp, const TimeKeeper& timekeeper) {
    State now_state;
    now_state.init();
    int now_score = now_state.score;
    int best_score = now_score;
    auto best_state = now_state;
    // mt19937 mt_for_action(0);
    int INF = INT_MAX;
    while (!timekeeper.isTimeOver()) {
        if (best_score >= set_t.count()*1000) break;
        // 近傍に遷移する
        now_state.transition();

        // スコア取得
        int next_score = now_state.score;

        double temp = start_temp + (end_temp - start_temp) * ((double)timekeeper.elapsedTime()/(double)timekeeper.getLimit());
        // 温度を非線形にする
		// double temp = end_temp + start_temp*exp(-(double)timekeeper.elapsedTime()/10.0);
        double probability = exp((next_score-now_score)/temp);
        bool is_force_next = probability > (randxor() % INF)/(double)(INF);

        if (next_score > now_score || is_force_next) {
            now_score = next_score;
        }
        else {
            now_state.back();
        }

        if (next_score > best_score) {
            best_score = next_score;
            best_state = now_state;
        }
    }
    
    // cout << "# t_num:  " << set_t.count() << endl;
    // cout << "# t_score:" << best_score << endl;
    return best_state;
}

};


// 焼きなましを用いて今回の問題のスコアを減らす
namespace SA {

/**
 * 関節点と橋をO(E + V)で求めるアルゴリズム.
 */
class LowLink {
    /**
    * 訪問したことがある頂点かどうか.
    */
    bitset<N_MAX> visited;
    /**
    * 順序.
    */
    vector<int> order;
    /**
    * 配列low. low[親] < low[子]なら、ブリッジとなる。
    */
    vector<int> low;

public:
    /**
    * 与えられる木。隣接リスト形式でコストは含まれない.親は-1とする。
    */
    bitset<N_MAX> set_A; 
    /**
    * 関節点　その点を取り除くと全体が不連結になる点.
    */
    bitset<N_MAX> not_articulation_points;

private:

    void dfs(int now, int parent, int& count) {
        visited.set(now);
        order[now] = count;
        low[now] = order[now];
        count++;
        bool is_art = false;/* 関節点であるかどうか */
        int son_count = 0;
        for (int next : G[now]) {
            if (!set_A[next]) continue;
            if (!visited[next]) {
                son_count++;
                dfs(next, now, count);
                if (next != parent) low[now] = min(low[now], low[next]); // ループに行きついたときに、戻る際の伝播
                //関節点か イコールがつくのは、ひとまとまりのグループ内のlow[]は関節点のlow[]と同じ値だから。端はもうこれ以上伸びないので、関節点にカウントしない
                if (parent != -1 && order[now] <= low[next]) is_art = true;
            }
            else {
                //すでに行ったことある頂点=後退辺
                if (next != parent) low[now] = min(low[now], order[next]);// ループに行きついた時にorder[]を代入
            }

        }
        //根は2つ以上の子があるなら、関節点
        if (parent == -1 && son_count >= 2) is_art = true;
        if (!is_art) not_articulation_points.set(now);
    }

public:
    /**
    * 渡すのは隣接リスト形式のグラフ。辺の長さは受け付けない.
    *
    * \param e
    */
    LowLink(const bitset<N_MAX>& set_A) : set_A(set_A) {
        visited.reset();
        order.resize(N_MAX);
        low.resize(N_MAX);
    }

    LowLink() {
        visited.reset();
        order.resize(N_MAX);
        low.resize(N_MAX);
    }

    void init(const bitset<N_MAX>& set_A_) {
        set_A = set_A_;
        visited.reset();
        not_articulation_points.reset();
    }

    /**
    * 計算の際に呼び出す。各連結成分ごとにDFSしている。.
    *
    */
    void build(int i) {
        int count = 0;
        // for (int i = 0; i < N_MAX; ++i) {
        //     if (!set_A[i]) continue;
        //     if (!visited[i]) dfs(i, -1, count);
        // }
        dfs(i,-1,count);
    }
};


// mt19937 mt_random;
class State {
public:
    int score;
    vector<int> A;
    vector<int> cnt;
    vector<bitset<N_MAX>> set_A;

    // スコア差分計算用
    vector<unordered_set<int>> HG;
    vector<vector<int>> HGmatrix;
    vector<int> dist;
    vector<int> path;
    vector<unordered_set<int>> invV;

    LowLink lowlink;
    vector<bitset<N_MAX>> nart_A;
    int HG2_size;
    int tmp_score;

    State() {
        score = -1e9;
        cnt.resize(N_MAX);
        invV.resize(N_MAX);
        tmp_score = 0;
    }

    State(const State& state) {
        score = state.score;
        A = state.A;
        cnt = state.cnt;
        set_A = state.set_A;

        HG = state.HG;
        HGmatrix = state.HGmatrix;
        dist = state.dist;
        path = state.path;
        invV = state.invV;
        nart_A = state.nart_A;
        HG2_size = state.HG2_size;
    }

    // 配列Aを圧縮した後、どのくらい余白が出るか計算する
    int calcCompression(const vector<int>& A_) {
        int HG2_size = (A_.size()+LB-1)/LB;
        vector<bitset<N_MAX>> set_A_(HG2_size);
        for (int i = 0; i < A_.size(); ++i) {
            set_A_[i/LB].set(A_[i]);
        }

        // それぞれの組み合わせで、良い組合せを貪欲に取っていく
        vector<tuple<int,int,int>> tmp;
        vector<int> cnt(HG2_size);
        for (int i = 0; i < HG2_size; ++i) {
            for (int j = i+1; j < HG2_size; ++j) {
                int score = (set_A_[i]&set_A_[j]).count();
                tmp.emplace_back(score,i,j);
                cnt[i] += score;
                cnt[j] += score;
            }
        }
        sort(tmp.begin(),tmp.end(),[&](tuple<int,int,int>& a, tuple<int,int,int>& b)->bool{
            auto [a_score,a_i,a_j] = a;
            auto [b_score,b_i,b_j] = b;
            if (a_score == b_score) return cnt[a_i]+cnt[a_j] < cnt[b_i]+cnt[b_j];
            else return a_score > b_score;
        });

        int z = 0;
        vector<bool> used(HG2_size);
        for (auto [score,i,j] : tmp) {
            if (used[i] || used[j]) continue;
            used[i] = true;
            used[j] = true;

            z += (set_A_[i]|set_A_[j]).count();
        }
        for (int i = 0; i < HG2_size; ++i) {
            if (used[i]) continue;
            z += set_A_[i].count();
        }

        return A_.size()-z;
    }

    // 状態を初期化する
    void init(const vector<int>& A_) {
        // 余白が出る量を予測して、配列Aを少し大きく取る
        int d = calcCompression(A_);
        // d = (d+LB-1)/LB;
        d /= LB;
        d += d/LB;
        int zz = N_MAX/2;
        while (d > 0) {
            queue<int> q;
            q.push(t[zz]);
            vector<int> path(N_MAX,-1);
            while (!q.empty()) {
                int x = q.front();
                q.pop();

                if (x == t[zz-1]) break;
                
                for (int y : G[x]) {
                    if (path[y] != -1 || y == t[zz]) continue;
                    path[y] = x;
                    q.push(y);
                }
            }

            vector<int> p;
            int start = t[zz-1];
            p.emplace_back(start);
            while (start != t[zz]) {
                start = path[start];
                p.emplace_back(start);
            }
            reverse(p.begin(),p.end());
            vector<vector<int>> tmp((p.size()+LB-1)/LB);
            for (int i = 0; i < p.size(); ++i) {
                tmp[i/LB].emplace_back(p[i]);
            }

            for (int i = 0; i < tmp.size(); ++i) {
                unordered_set<int> st;
                queue<int> q;
                for (int j : tmp[i]) {
                    q.push(j);
                    st.insert(j);
                }
                while (!q.empty()) {
                    int x = q.front();
                    q.pop();

                    if (st.size() == LB) break;
                    
                    for (int y : G[x]) {
                        if (st.count(y)) continue;
                        if (st.size() == LB) break;
                        st.insert(y);
                        tmp[i].emplace_back(y);
                        q.push(y);
                    }
                }

                for (int j : tmp[i]) {
                    A.emplace_back(j);
                }
                --d;
                if (d == 0) break;
            }

            --zz;
            if (zz == 0) zz = N_MAX-1;
        }

        for (int i = 0; i < A_.size(); ++i) {
            A.emplace_back(A_[i]);
        }

        for (int i = 0; i < A.size(); ++i) ++cnt[A[i]];
        // for (int i = 0; i < N_MAX; ++i) {
        //     if (set_t[i]) tmp_score += cnt[i];
        // }

        // resize
        HG2_size = (A.size()+LB-1)/LB;
        set_A.resize(HG2_size);
        HG.resize(HG2_size);
        HGmatrix.resize(HG2_size, vector<int>(HG2_size));
        dist.resize(HG2_size*HG2_size,1e5);
        path.resize(HG2_size*HG2_size,-1);
        nart_A.resize(HG2_size);
        

        int z = 0;
        for (int i = 0; i < HG2_size; ++i) {
            int cnt = 0;
            while (cnt < LB && z < A.size()) {
                set_A[i].set(A[z]);
                ++z;
                ++cnt;
            }
        }


        // スコア差分計算用
        for (int i = 0; i < A.size(); ++i) invV[A[i]].insert(i/LB);

        vector<vector<int>> V(HG2_size);
        for (int i = 0; i < HG2_size; ++i) {
            for (int j = 0; j < LB; ++j) {
                if (i*LB+j >= A.size()) break;
                V[i].emplace_back(A[i*LB+j]);
            }
        }

        for (int i = 0; i < HG2_size; ++i) {
            unordered_set<int> Adj;
            for (int j : V[i]) {
                for (int k : G[j]) Adj.insert(k);
            }

            for (int j = i+1; j < HG2_size; ++j) {
                bool ok = false;

                for (int k : V[j]) {
                    if (Adj.count(k)) ok = true;
                }

                if (ok) {
                    HG[i].insert(j);
                    HG[j].insert(i);
                }
            }
        }

        for (int i = 0; i < A.size(); ++i) {
            for (int j : G[A[i]]) {
                for (int k : invV[j]) {
                    ++HGmatrix[i/LB][k];
                }
            }
        }

        for (int i = 0; i < HG2_size; ++i) {
            queue<int> q;
            q.push(i);
            int ii = i*HG2_size;
            dist[ii+i] = 0;
            while (!q.empty()) {
                int x = q.front();
                q.pop();
                
                for (int y : HG[x]) {
                    if (dist[ii+y] != 1e5) continue;
                    dist[ii+y] = dist[ii+x] + 1;
                    path[ii+y] = x;
                    q.push(y);
                }
            }
        }

        for (int i = 0; i < HG2_size; ++i) {
            LowLink lowlink(set_A[i]);
            lowlink.build(A[i*LB]);
            nart_A[i] = lowlink.not_articulation_points;
        }
 
        score = -calcScore();// + tmp_score;      
    }

    // 遷移する(+score計算)
    int r1,r2;
    int b_v;
    int b_score;
    bitset<N_MAX> nart;
    int cand[N_MAX];
    void transition() {
        b_score = score;
        r1 = randxor()%HG2_size;
        int l = (r1 == HG2_size-1 && A.size()%LB) ? A.size()%LB : LB;

        if (nart_A[r1].none()) {
            lowlink.init(set_A[r1]);
            lowlink.build(A[r1*LB]);
            nart_A[r1] = lowlink.not_articulation_points;
        }
        
        int cand_num = 0;
        for (int i = 0; i < l; ++i) {
            if (!nart_A[r1][A[r1*LB+i]]) continue;
            if (cnt[A[r1*LB+i]] == 1 && set_t[A[r1*LB+i]]) continue;
            cand[cand_num] = i;
            ++cand_num;
        }
        if (cand_num == 0) {
            r2 = 0;
            b_v = A[r1*LB];
            return;
        }

        r2 = cand[randxor()%cand_num];
        int pos = r1*LB+r2;
        b_v = A[pos];

        // 選べる頂点を列挙する
        bitset<N_MAX> set_cand;
        for (int i = 0; i < l; ++i) {
            if (i == r2) continue;
            
            set_cand |= Gmatrix[A[r1*LB+i]];
        }
        set_cand &= ~set_A[r1];

        if (set_cand.none()) return;

        cand_num = 0;
        for (int i = 0; i < N_MAX; ++i) {
            if (!set_cand[i]) continue;
            cand[cand_num] = i;
            ++cand_num;
            if (set_t[i]) {
                cand[cand_num] = i;
                ++cand_num;
            }
        }

        // if (set_t[A[pos]]) --tmp_score;
        int select = cand[randxor()%cand_num];
        A[pos] = select;

        set_A[r1].reset(b_v);
        set_A[r1].set(A[pos]);
        
        --cnt[b_v];
        ++cnt[A[pos]];

        nart = nart_A[r1];
        nart_A[r1].reset();
        // if (set_t[A[pos]]) ++tmp_score;

        // グラフの最短距離等を変更する
        if (!recalcDist(r1,b_v,A[pos], false)) {
            score = -1e9;
            return;
        }

        score = -calcScore(); // + tmp_score;
    }

    // 遷移前に戻す(+scoreも元に戻す)
    void back() {
        score = b_score;
        if (b_v == A[r1*LB+r2]) return;

        int l = (r1 == HG2_size-1 && A.size()%LB) ? A.size()%LB : LB;
        int pos = r1*LB + r2;
        
        recalcDist(r1,A[pos],b_v, true);

        nart_A[r1] = nart;
        
        // if (set_t[A[pos]]) --tmp_score;
        --cnt[A[pos]];
        set_A[r1].reset(A[pos]);
        A[pos] = b_v;
        set_A[r1].set(A[pos]);
        ++cnt[A[pos]];
        // if (set_t[A[pos]]) ++tmp_score;
    }

    // スコア計算
    int dp[2*N_MAX];
    int calcScore() {
        unordered_set<int> start_pos;
        for (int i : G[0]) {
            for (int j : invV[i]) {
                start_pos.insert(j);
            }
        }

        int dp_num = 0;
        int ndp_num = 0;
        for (int i : start_pos) {
            dp[dp_num] = i;
            ++dp_num;
        }
        
        int ret = 1;
        for (int i = 0; i < N_MAX; ++i) {
            int tmp = (i&1)*N_MAX;
            int n_best_cost = INF;
            for (int v : invV[t[i]]) {
                int best_cost = INF;
                for (int j = 0; j < dp_num; ++j) {
                    int u = dp[tmp + j];
                    best_cost = min(best_cost, dist[v*HG2_size+u]);
                }

                if (n_best_cost > best_cost) {
                    ndp_num = 0;
                    dp[N_MAX-tmp + ndp_num] = v;
                    ++ndp_num;
                    n_best_cost = best_cost;
                }
                else if (n_best_cost == best_cost) {
                    dp[N_MAX-tmp + ndp_num] = v;
                    ++ndp_num;
                }
            }
            ret += n_best_cost;
            swap(dp_num,ndp_num);
            // if (ret > 2-score) return 1e9;
        }

        return ret;
    }

    // 配列Aから変更する頂点を渡すと、グラフの構造と距離を変更する
    int add[HG_MAX];
    int erase[HG_MAX];
    bool recalcDist(int r1, int bv, int nv, bool isBack) {
        // 1. invVを更新
        // 2. r1から繋がる辺を更新
        // 3. 追加された辺をよって、距離が変わるか計算する
        // 4. 削除された辺によって、距離が変わるか計算する

        // 1. invVを更新
        invV[bv].erase(r1);
        invV[nv].insert(r1);

        // 2. r1から繋がる辺を更新
        // 増える辺集合
        int add_num = 0;
        for (int j : G[nv]) {
            for (int k : invV[j]) {
                if (r1 == k) continue;
                if (HGmatrix[r1][k] == 0) {
                    add[add_num] = k;
                    ++add_num;
                }
                ++HGmatrix[r1][k];
                ++HGmatrix[k][r1];
            }
        }
        // 減る辺集合
        int erase_num = 0;
        for (int j : G[bv]) {
            for (int k : invV[j]) {
                if (r1 == k) continue;
                if (HGmatrix[r1][k] == 1) {
                    erase[erase_num] = k;
                    ++erase_num;
                }
                --HGmatrix[r1][k];
                --HGmatrix[k][r1];
            }
        }

        if (add_num == 0 && erase_num == 0) return true;
        if (add_num == 0 && erase_num > 1 && !isBack) return false;
        if (erase_num == 0 && add_num > 1 && isBack) return false;
        // if (add_num*4+1 < erase_num && !isBack) return false;
        // if (erase_num*4+1 < add_num && isBack) return false;

        for (int j = 0; j < add_num; ++j) HG[r1].insert(add[j]), HG[add[j]].insert(r1);
        // 3. 追加された辺をよって、距離が変わるか計算する
        if (add_num > 0) recalcDistAdd(r1,add_num);
        
        for (int j = 0; j < erase_num; ++j) HG[r1].erase(erase[j]), HG[erase[j]].erase(r1);
        // 4. 削除された辺によって、距離が変わるか計算する
        if (erase_num > 0) recalcDistErase(r1,erase_num);

        return true;
    }


    // 追加された辺による距離の変化を求める
    int que[N_MAX];
    int ql,qr;
    void recalcDistAdd(int r1, int add_num) {
        ql = 0; qr = 0;
        for (int i = 0; i < HG2_size; ++i) {
            int ii = i*HG2_size;
            bool flag = false;
            int score = dist[ii+r1];
            int tmp = r1;
            for (int j = 0; j < add_num; ++j) {
                if (abs(dist[ii+add[j]]-dist[ii+r1]) <= 1) continue;
                if (dist[ii+add[j]] < score) {
                    score = dist[ii+add[j]];
                    tmp = add[j];
                }
                flag = true;
            }

            if (flag) {
                if (tmp != r1) {
                    dist[ii+r1] = score+1;
                    path[ii+r1] = tmp;
                }
                que[qr] = r1; ++qr;
                if (qr >= N_MAX) qr -= N_MAX;
            }
            else continue;

            while (ql != qr) {
                int x = que[ql]; ++ql;
                if (ql >= N_MAX) ql -= N_MAX;
                
                for (int y : HG[x]) {
                    if (dist[ii+y] <= dist[ii+x]+1) continue;
                    dist[ii+y] = dist[ii+x]+1;
                    path[ii+y] = x;
                    que[qr] = y; ++qr;
                    if (qr >= N_MAX) qr -= N_MAX;
                }
            }
        }
    }

    // 削除された辺による距離の変化を求める
    int INF_vertex[HG_MAX];
    void recalcDistErase(int r1, int erase_num) {
        int inf_num = 0;
        ql = 0; qr = 0;
        for (int i = 0; i < HG2_size; ++i) {
            int ii = i*HG2_size;
            inf_num = 0;
            for (int j = 0; j < erase_num; ++j) {
                // その辺が部分木に使用されている(r1が親)
                if (path[ii+erase[j]] == r1) { que[qr] = erase[j]; ++qr; if (qr >= N_MAX) qr -= N_MAX; }
                // その辺が部分木に使用されている(r1が子)
                else if (path[ii+r1] == erase[j]) { que[qr] = r1; ++qr; if (qr >= N_MAX) qr -= N_MAX; }
                else continue;

                while (ql != qr) {
                    int x = que[ql]; ++ql;
                    if (ql >= N_MAX) ql -= N_MAX;

                    // 更新不要か判定する
                    int select = -1;
                    for (int y : HG[x]) {
                        if (dist[ii+x] == dist[ii+y]+1) {
                            select = y;
                            break;
                        }
                    }
                    if (select != -1) {
                        path[ii+x] = select;
                        continue;
                    }

                    // この頂点をINFに変える
                    dist[ii+x] = 1e5;
                    path[ii+x] = -1;
                    INF_vertex[inf_num] = x;
                    ++inf_num;

                    // 部分木をたどる
                    for (int y : HG[x]) {
                        if (path[ii+y] == x) {
                            que[qr] = y; ++qr;
                            if (qr >= N_MAX) qr -= N_MAX;
                        }
                    }
                }
            }

            if (inf_num == 0) continue;
            for (int j = 0; j < inf_num; ++j) {

                int score = 1e5;
                int tmp = -1;
                for (int k : HG[INF_vertex[j]]) {
                    if (score > dist[ii+k]) {
                        score = dist[ii+k];
                        tmp = k;
                    }
                }

                if (tmp != -1) {
                    dist[ii+INF_vertex[j]] = score+1;
                    path[ii+INF_vertex[j]] = tmp;
                    que[qr] = INF_vertex[j]; ++qr;
                    if (qr >= N_MAX) qr -= N_MAX;
                }
            }

            while (ql != qr) {
                int x = que[ql]; ++ql;
                if (ql >= N_MAX) ql -= N_MAX;
                
                for (int y : HG[x]) {
                    if (dist[ii+y] <= dist[ii+x]+1) continue;
                    dist[ii+y] = dist[ii+x]+1;
                    path[ii+y] = x;
                    que[qr] = y; ++qr;
                    if (qr >= N_MAX) qr -= N_MAX;
                }
            }
        }
    }
};

vector<int> simulatedAnnealing(const vector<int>& A, double start_temp, double end_temp, const TimeKeeper& timekeeper) {
    State now_state;
    now_state.init(A);
    int now_score = now_state.score;
    int best_score = now_score;
    vector<int> best_state = now_state.A;
    // mt19937 mt_for_action(0);
    int INF = INT_MAX;

    int loop_num = 0;
    while (!timekeeper.isTimeOver()) {
        ++loop_num;
        // 近傍に遷移する
        now_state.transition();

        // スコア取得
        int next_score = now_state.score;

        double temp = start_temp + (end_temp - start_temp) * ((double)timekeeper.elapsedTime()/(double)timekeeper.getLimit());
        // 温度を非線形にする
		// double temp = end_temp + start_temp*exp(-(double)timekeeper.elapsedTime()/10.0);
        double probability = exp((next_score-now_score)/temp);
        bool is_force_next = probability > (randxor() % INF)/(double)(INF);

        if (next_score > now_score || is_force_next) {
            now_score = next_score;
        }
        else {
            now_state.back();
        }

        if (next_score > best_score) {
            best_score = next_score;
            best_state = now_state.A;
        }
    }
    cout << "# loop_num:" << loop_num << endl;

    return best_state;
}

};




// 貪欲
// ・幅優先をして次の経路を決める
// ・LBの更新は全ての組み合わせを見て、最も次へ進むものを選ぶ
vector<tuple<char,int,int,int>> solveGreedy(vector<int>& A) {
    vector<int> B(LB,-1);
    vector<tuple<char,int,int,int>> history; 
    int now = 0;
    for (int i = 0; i < T; ++i) {
        // 経路を見つける
        queue<int> q;
        vector<bool> used(N);
        vector<int> path(N,-1);
        q.push(now);
        used[now] = true;
        while (!q.empty()) {
            int x = q.front();
            q.pop();
            
            bool flag = false;
            for (int y : G[x]) {
                if (used[y]) continue;
                used[y] = true;
                path[y] = x;
                q.push(y);
                if (y == t[i]) flag = true;
            }
            if (flag) break;
        }

        vector<int> p;
        int tmp = t[i];
        while (path[tmp] != -1) {
            p.emplace_back(tmp);
            tmp = path[tmp];
        }
        reverse(p.begin(),p.end());


        // 次の目的地に到着するまで繰り返す
        int step = 0;
        while (now != t[i]) {
            // 移動できるまで繰り返す
            while (step < p.size()) {
                bool flag = true;

                for (int j = 0; j < LB; ++j) {
                    if (B[j] == p[step]) {
                        history.emplace_back('m',p[step],-1,-1);
                        now = p[step];
                        ++step;
                        flag = false;
                        break;
                    }
                }

                if (flag) break;
            }
            if (now == t[i]) break;

            // LBの値を更新する(LBの値を全て更新にする)
            vector<int> tmp_B = B;
            int best = -1;
            int score = -1;
            for (int a = 0; a <= LA-LB; ++a) {
                for (int b = 0; b < LB; ++b) {
                    tmp_B[b] = A[a+b];
                }

                int tmp_step = step;
                while (tmp_step < p.size()) {
                    bool flag = true;

                    for (int j = 0; j < LB; ++j) {
                        if (tmp_B[j] == p[tmp_step]) {
                            ++tmp_step;
                            flag = false;
                            break;
                        }
                    }

                    if (flag) break;
                }

                if (tmp_step > score) {
                    score = tmp_step;
                    best = a;
                }
            }

            history.emplace_back('s',LB,best,0);
            for (int b = 0; b < LB; ++b) {
                B[b] = A[best+b];
            }
        }


    }

    return history;
}

// 貪欲
// 配列Aを初期化する
vector<int> greedyInitA() {
    vector<int> A(LA);
    vector<vector<int>> v;

    int now = 0;
    for (int i = 0; i < T; ++i) {
        // 経路を見つける
        queue<int> q;
        vector<bool> used(N);
        vector<int> path(N,-1);
        q.push(now);
        used[now] = true;
        while (!q.empty()) {
            int x = q.front();
            q.pop();
            
            bool flag = false;
            for (int y : G[x]) {
                if (used[y]) continue;
                used[y] = true;
                path[y] = x;
                q.push(y);
                if (y == t[i]) flag = true;
            }
            if (flag) break;
        }

        vector<int> p;
        int tmp = t[i];
        while (path[tmp] != -1) {
            p.emplace_back(tmp);
            tmp = path[tmp];
        }
        reverse(p.begin(),p.end());
        v.emplace_back(p);
        now = t[i];
    }

    int x = 0;
    vector<bool> used(N);
    vector<bool> st(N);
    for (int i = 0; i < N; ++i) {
        if (x == LA) break;

        int score = -1;
        int select = -1;
        for (int j = 0; j < N; ++j) {
            if (st[j]) continue;
            int tmp_score = 0;
            for (int k = 0; k < v[j].size(); ++k) {
                if (!used[v[j][k]]) ++tmp_score;
            }

            if (score < tmp_score) {
                score = tmp_score;
                select = j;
            }
        }

        st[select] = true;

        if (score > 0) {
            for (int k = 0; k < v[select].size(); ++k) {
                if (!used[v[select][k]]) {
                    used[v[select][k]] = true;
                    A[x] = v[select][k];
                    ++x;
                }
            }
        }
        else {
            for (int k = 0; k < v[select].size(); ++k) {
                A[x] = v[select][k];
                ++x;
                if (x == LA) break;
            }
        }
        
    }

    return A;
}

// 貪欲
pair<vector<int>,vector<tuple<char,int,int,int>>> greedy() {
    auto A = greedyInitA();
    auto ans = solveGreedy(A);
    return {A,ans};
}


// 焼きなまし法で求めた配列Aから、解を生成する
pair<vector<int>,vector<tuple<char,int,int,int>>> createAnswer(const vector<int>& A, const vector<pair<int,int>>& section) {
    vector<tuple<char,int,int,int>> ans;

    // 1. 配列Aから新しいグラフHGを作る
    // 2. HGの全ての頂点間の距離を前計算する
    // 3. dpで経路を計算する(最悪O(N^3))
    // 4. 復元する

    // 1. 配列Aから新しいグラフHGを作る
    int HG2_size = section.size();
    vector<vector<int>> HG(HG2_size);
    vector<vector<int>> V(HG2_size);
    for (int i = 0; i < HG2_size; ++i) {
        int j = section[i].first;
        for (int k = 0; k < section[i].second; ++k) {
            V[i].emplace_back(A[j+k]);
        }
    }

    for (int i = 0; i < HG2_size; ++i) {
        unordered_set<int> Adj;
        for (int j : V[i]) {
            for (int k : G[j]) Adj.insert(k);
        }

        for (int j = i+1; j < HG2_size; ++j) {
            bool ok = false;

            for (int k : V[j]) {
                if (Adj.count(k)) ok = true;
            }

            if (ok) {
                HG[i].emplace_back(j);
                HG[j].emplace_back(i);
            }
        }
    }

    // cerr << "1 done" << endl;
    vector<vector<int>> invV(N);
    for (int i = 0; i < HG2_size; ++i) {
        int j = section[i].first;
        for (int k = 0; k < section[i].second; ++k) {
            invV[A[j+k]].emplace_back(i);
        }
    }

    // 2. HGの全ての頂点間の距離を前計算する
    vector<vector<int>> dist(HG2_size, vector<int>(HG2_size,INF));
    vector<vector<int>> path(HG2_size, vector<int>(HG2_size,-1));
    for (int i = 0; i < HG2_size; ++i) {
        queue<int> q;
        q.push(i);
        dist[i][i] = 0;
        while (!q.empty()) {
            int x = q.front();
            q.pop();
            
            for (int y : HG[x]) {
                if (dist[i][y] != INF) continue;
                dist[i][y] = dist[i][x] + 1;
                path[i][y] = x;
                q.push(y);
            }
        }
    }

    // cerr << "2 done" << endl;

    // 3. dpで経路を計算する(最悪O(N^3))
    // {HG上の頂点、移動コスト、親}
    vector<vector<tuple<int,int,int>>> dp(N+1);
    // 初期化
    set<int> start_pos;
    for (int i : G[0]) {
        for (int j : invV[i]) start_pos.insert(j);
    }
    for (int v : start_pos) {
        dp[0].emplace_back(v,1,-1);
    }

    // dp
    for (int i = 0; i < N; ++i) {
        for (int v : invV[t[i]]) {
            int best_cost = INF;
            int best_idx = -1;
            for (int j = 0; j < dp[i].size(); ++j) {
                auto [u,cost,_] = dp[i][j];

                if (best_cost > cost + dist[u][v]) {
                    best_cost = cost + dist[u][v];
                    best_idx = j;
                }
            }
            if (best_idx == -1) return greedy();
            dp[i+1].emplace_back(v,best_cost,best_idx);
        } 
    }

    // cerr << "3 done" << endl;

    // 4. 復元する
    vector<int> p;
    int best_cost = INF;
    int best_idx = -1;
    for (int j = 0; j < dp[N].size(); ++j) {
        auto [u,cost,_] = dp[N][j];

        if (best_cost > cost) {
            best_cost = cost;
            best_idx = j;
        }
    }
    
    for (int i = N; i >= 0; --i) {
        auto [u,cost,j] = dp[i][best_idx];
        if (p.empty() || p.back() != u) p.emplace_back(u);
        best_idx = j;
    }
    vector<int> HG_path;
    HG_path.emplace_back(p[0]);
    for (int i = 0; i < p.size()-1; ++i) {
        int start = p[i+1];
        vector<int> tmp;
        while (start != p[i]) {
            tmp.emplace_back(start);
            start = path[p[i]][start];
        }
        reverse(tmp.begin(),tmp.end());
        for (int j : tmp) HG_path.emplace_back(j);
    }
    reverse(HG_path.begin(),HG_path.end());

    // HG上でのパスを、元のグラフでのパスに変換する
    int now = 0;

    // 遷移可能な位置へ移動する
    // 信号を切り替える
    // 青色の頂点へ移動する
    int z = 0;
    for (int i = 0; i < HG_path.size(); ++i) {
        // 遷移可能な位置へ移動する
        if (i > 0) {
            // 目的の頂点を求める
            set<int> tmp;
            for (int j : V[HG_path[i]]) {
                for (int k : G[j]) tmp.insert(k);
            }

            int next = -1;
            for (int j : V[HG_path[i-1]]) {
                if (tmp.count(j)) next = j;
            }
            tmp.clear();
            for (int j : V[HG_path[i-1]]) tmp.insert(j);

            // 経路を求める
            map<int,int> p;
            p[next] = -1;
            queue<int> q;
            q.push(next);
            while (!q.empty()) {
                int x = q.front();
                q.pop();
                
                for (int y : G[x]) {
                    if (p.find(y) != p.end()) continue;
                    if (!tmp.count(y)) continue;

                    p[y] = x;
                    q.push(y);
                }
            }

            while (now != next) {
                now = p[now];
                ans.emplace_back('m',now,-1,-1);
            }
        }

        // 信号を切り替える
        // int l = (HG_path[i] == HG_size-1 && LA%LB) ? LA%LB : LB;
        int l = section[HG_path[i]].second;
        int a = section[HG_path[i]].first;
        ans.emplace_back('s',l,a,0);

        // 青色の頂点へ移動する
        set<int> tmp;
        for (int j : V[HG_path[i]]) tmp.insert(j);
        for (int j : G[now]) {
            if (tmp.count(j)) {
                now = j;
                ans.emplace_back('m',now,-1,-1);
                break;
            }
        }

        // 内部に目的地が存在する場合は、目的地へ移動する
        while (z < N && tmp.count(t[z])) {
            map<int,int> p;
            p[t[z]] = -1;
            queue<int> q;
            q.push(t[z]);
            while (!q.empty()) {
                int x = q.front();
                q.pop();
                
                for (int y : G[x]) {
                    if (p.find(y) != p.end()) continue;
                    if (!tmp.count(y)) continue;

                    p[y] = x;
                    q.push(y);
                }
            }

            while (now != t[z]) {
                now = p[now];
                ans.emplace_back('m',now,-1,-1);
            }

            ++z;
        }
    }

    return {A,ans};
}

// 配列Aを圧縮して余白をつくる
pair<vector<int>,vector<pair<int,int>>> compression(const vector<int>& A) {
    int HG2_size = (A.size()+LB-1)/LB;
    vector<bitset<N_MAX>> set_A(HG2_size);
    for (int i = 0; i < A.size(); ++i) {
        set_A[i/LB].set(A[i]);
    }

    // 良い組合せを貪欲に取っていく
    vector<tuple<int,int,int>> tmp;
    vector<int> cnt(HG2_size);
    for (int i = 0; i < HG2_size; ++i) {
        for (int j = i+1; j < HG2_size; ++j) {
            int score = (set_A[i]&set_A[j]).count();
            tmp.emplace_back(score,i,j);
            cnt[i] += score;
            cnt[j] += score;
        }
    }
    sort(tmp.begin(),tmp.end(),[&](tuple<int,int,int>& a, tuple<int,int,int>& b)->bool{
        auto [a_score,a_i,a_j] = a;
        auto [b_score,b_i,b_j] = b;
        if (a_score == b_score) return cnt[a_i]+cnt[a_j] < cnt[b_i]+cnt[b_j];
        else return a_score > b_score;
    });

    int z = 0;
    vector<int> ret_A(LA);
    vector<pair<int,int>> ret_section;
    vector<bool> used(HG2_size);
    for (auto [score,i,j] : tmp) {
        if (used[i] || used[j]) continue;
        used[i] = true;
        used[j] = true;

        ret_section.emplace_back(z,set_A[i].count());
        auto i_and_j = set_A[i]&set_A[j];
        auto i_only = set_A[i]^(i_and_j);
        auto j_only = set_A[j]^(i_and_j);
        for (int k= 0; k < N_MAX; ++k) {
            if (!i_only[k]) continue;
            if (z >= LA) {
                ret_section.pop_back();
                return {ret_A,ret_section};
            }
            ret_A[z] = k;
            ++z;
        }

        ret_section.emplace_back(z,set_A[j].count());
        for (int k= 0; k < N_MAX; ++k) {
            if (!i_and_j[k]) continue;
            if (z >= LA) {
                ret_section.pop_back();
                ret_section.pop_back();
                return {ret_A,ret_section};
            }
            ret_A[z] = k;
            ++z;
        }
        for (int k= 0; k < N_MAX; ++k) {
            if (!j_only[k]) continue;
            if (z >= LA) {
                ret_section.pop_back();
                return {ret_A,ret_section};
            }
            ret_A[z] = k;
            ++z;
        }
    }
    for (int i = 0; i < HG2_size; ++i) {
        if (used[i]) continue;

        ret_section.emplace_back(z,set_A[i].count());
        for (int k= 0; k < N_MAX; ++k) {
            if (!set_A[i][k]) continue;
            if (z >= LA) {
                ret_section.pop_back();
                return {ret_A,ret_section};
            }
            ret_A[z] = k;
            ++z;
        }
    }

    if (z >= LA) return {ret_A,ret_section};

    // 余った空白を適当に貪欲で埋めておく
    // 今の状態でdpをする
    // ある区間t[i]からt[i+1]の最短コストも同時に求めておく
    // 最短コストと比べて差が大きい場所を埋める感じにしておく

    // 今の状態でdpをする
    HG2_size = ret_section.size();
    vector<vector<int>> HG(HG2_size);
    vector<vector<int>> V(HG2_size);
    for (int i = 0; i < HG2_size; ++i) {
        int j = ret_section[i].first;
        for (int k = 0; k < ret_section[i].second; ++k) {
            V[i].emplace_back(ret_A[j+k]);
        }
    }

    for (int i = 0; i < HG2_size; ++i) {
        
        unordered_set<int> Adj;
        for (int j : V[i]) {
            for (int k : G[j]) Adj.insert(k);
        }

        for (int j = i+1; j < HG2_size; ++j) {
            bool ok = false;

            for (int k : V[j]) {
                if (Adj.count(k)) ok = true;
            }

            if (ok) {
                HG[i].emplace_back(j);
                HG[j].emplace_back(i);
            }
        }
    }

    // cerr << "1 done" << endl;
    vector<vector<int>> invV(N);
    for (int i = 0; i < HG2_size; ++i) {
        int j = ret_section[i].first;
        for (int k = 0; k < ret_section[i].second; ++k) {
            invV[ret_A[j+k]].emplace_back(i);
        }
    }

    // 2. HGの全ての頂点間の距離を前計算する
    vector<vector<int>> dist(HG2_size, vector<int>(HG2_size,INF));
    vector<vector<int>> path(HG2_size, vector<int>(HG2_size,-1));
    for (int i = 0; i < HG2_size; ++i) {
        queue<int> q;
        q.push(i);
        dist[i][i] = 0;
        while (!q.empty()) {
            int x = q.front();
            q.pop();
            
            for (int y : HG[x]) {
                if (dist[i][y] != INF) continue;
                dist[i][y] = dist[i][x] + 1;
                path[i][y] = x;
                q.push(y);
            }
        }
    }

    // cerr << "2 done" << endl;

    // 3. dpで経路を計算する(最悪O(N^3))
    // {HG上の頂点、移動コスト、親}
    vector<vector<tuple<int,int,int>>> dp(N+1);
    vector<int> cost(N_MAX);
    // 初期化
    set<int> start_pos;
    for (int i : G[0]) {
        for (int j : invV[i]) start_pos.insert(j);
    }
    for (int v : start_pos) {
        dp[0].emplace_back(v,1,-1);
    }

    // dp
    for (int i = 0; i < N_MAX; ++i) {
        int n_best_cost = INF;
        for (int v : invV[t[i]]) {
            int best_cost = INF;
            int best_idx = -1;
            for (int j = 0; j < dp[i].size(); ++j) {
                auto [u,cost,_] = dp[i][j];

                if (best_cost > cost + dist[u][v]) {
                    n_best_cost = min(n_best_cost,dist[u][v]);
                    best_cost = cost + dist[u][v];
                    best_idx = j;
                }
            }
            dp[i+1].emplace_back(v,best_cost,best_idx);
            
        } 
        cost[i] = n_best_cost;
    }

    // ある区間t[i]からt[i+1]の最短コストも同時に求めておく
    vector<vector<int>> G_dist(N_MAX, vector<int>(N_MAX,INF));
    vector<vector<int>> G_path(N_MAX, vector<int>(N_MAX,-1));
    for (int i = 0; i < N_MAX; ++i) {
        queue<int> q;
        q.push(i);
        G_dist[i][i] = 0;
        while (!q.empty()) {
            int x = q.front();
            q.pop();
            
            for (int y : G[x]) {
                if (G_dist[i][y] <= G_dist[i][x]+1) continue;
                G_dist[i][y] = G_dist[i][x]+1;
                G_path[i][y] = x;
                q.push(y);
            }
        }
    }

    // 最短コストと比べて差が大きい場所を埋める感じにしておく
    vector<pair<int,int>> tmp_s;
    for (int i = 0; i < N_MAX; ++i) {
        int now = 0;
        if (i > 0) now = t[i-1];
        int next = t[i];
        int score = cost[i] - (G_dist[now][next]+LB-1)/LB;
        tmp_s.emplace_back(score,i);
    }
    sort(tmp_s.rbegin(), tmp_s.rend());

    for (auto [score,i] : tmp_s) {
        if (z >= LA) break;

        int now = 0;
        if (i > 0) now = t[i-1];
        int next = t[i];

        vector<int> p;
        while (now != -1) {
            p.emplace_back(now);
            now = G_path[next][now];
        }

        int b_z = z;
        for (int j : p) {
            if (z >= LA) break;
            ret_A[z] = j;
            ++z;
        }

        for (int j = b_z; j <= z-LB; ++j) {
            ret_section.emplace_back(j,LB);
        }
    }

    return {ret_A, ret_section};
}

// 出力
void output(vector<int>& A, vector<tuple<char,int,int,int>>& ans) {
    for (int i = 0; i < LA; ++i) {
        cout << A[i] << ' ';
    }
    cout << endl;
    int score = 0;
    for (auto [c,l,a,b] : ans) {
        if (c == 's') cout << c << ' ' << l << ' ' << a << ' ' << b << endl, ++score;
        else cout << c << ' ' << l << endl;
    }
    cerr << "score:" << score << endl;
}


int main(void) {
    input();

    TimeKeeper timekeeper(1000);
    TimeKeeper timekeeper2(2850);
    auto state = InitA::simulatedAnnealing(10.0,1.0,timekeeper);
    auto A = state.A;
    
    auto A1 = SA::simulatedAnnealing(A,0.8*N_MAX/LA,0.0001,timekeeper2);
    auto [A2,section] = compression(A1);
    auto [A3,ans] = createAnswer(A2,section);

    output(A3,ans);
    
    return 0;
}