# pragma GCC optimize("O3")
# pragma GCC optimize("unroll-loops")
# pragma GCC target("avx2")
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

#define chmin(a,b) a=min(a,b)
#define chmax(a,b) a=max(a,b)
#define rep_(_1,_2,_3,NAME,...) NAME
#define rep(...) rep_(__VA_ARGS__, rep2, rep1)(__VA_ARGS__)
#define rep1(i,j) for(int i=0;i<(j);i++)
#define rep2(i,j,k) for(int i=(j);i<(k);i++)

static inline int RAND(){
    static uint32_t x = 57u;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    return (int)(x>>1); // 非負
}

string UDLRS = "UDLRS";

int N, M, K; // for output

// ======== 問題インスタンス（nxt前計算＋初期位置フラット） ========
struct Instance {
    int N, M, K, TOT;       // TOT = N*N
    int init_idx[10];       // 各ロボの初期位置 id (0..N*N-1)
    int nxt[900][5];        // nxt[id][act], act: 0U,1D,2L,3R,4S
};

static inline int cell_id(int i, int j, int N){ return i*N + j; }

Instance read_instance_from_stdin(){
    Instance I;
    vector<pair<int,int>> init;
    vector<string> V, H;

    cin >> I.N >> I.M >> I.K; // N=30, M=10, K=10 固定
    N=I.N; M=I.M; K=I.K;
    I.TOT = I.N * I.N;

    init.resize(I.M);
    rep(k, I.M){
        int i,j; cin>>i>>j;
        init[k] = {i,j};
        I.init_idx[k] = cell_id(i,j,I.N);
    }

    V.assign(I.N, "");
    rep(i, I.N){
        string s; cin >> s; // len N-1
        V[i] = std::move(s);
    }
    H.assign(I.N-1, "");
    rep(i, I.N-1){
        string s; cin >> s; // len N
        H[i] = std::move(s);
    }

    // nxt 前計算
    rep(i, I.N) rep(j, I.N){
        int id = cell_id(i,j,I.N);
        // S
        I.nxt[id][4] = id;
        // U
        if(i>0 && H[i-1][j]=='0') I.nxt[id][0] = cell_id(i-1,j,I.N);
        else                      I.nxt[id][0] = id;
        // D
        if(i+1<I.N && H[i][j]=='0') I.nxt[id][1] = cell_id(i+1,j,I.N);
        else                        I.nxt[id][1] = id;
        // L
        if(j>0 && V[i][j-1]=='0') I.nxt[id][2] = cell_id(i,j-1,I.N);
        else                      I.nxt[id][2] = id;
        // R
        if(j+1<I.N && V[i][j]=='0') I.nxt[id][3] = cell_id(i,j+1,I.N);
        else                        I.nxt[id][3] = id;
    }
    return I;
}

// ======== 解のパラメタB（評価・探索はコレだけで回す） ========
struct B{
    int c[10][10];   // 0..9 ボタンの各ロボ動作 in {0:U,1:D,2:L,3:R,4:S}
    int T0;          // 先頭の自由区間の長さ（例：20）
    int a[100];      // 先頭T0で押すボタン番号（0..9）
    int W;           // 周期の幅（8をW回→7→9をW回→7→…）
    int d[10];       // 7/8/9 の派生動作パラメタ。d[r]: x=(d>>1)∈{0..3}, s=(d&1)
};

// 7/8/9 の実動作を b.c[7..9][] に前計算（以降は c[btn][r] を直接参照）
static inline void build_derived_acts(B& b){
    rep(r,10){
        int d = b.d[r];
        int x = (d >> 1);        // 0..3
        int s = (d & 1);         // 0/1
        b.c[7][r] = x;
        b.c[8][r] = (x ^ 2) ^ s; // 180°反転 ^ 反転フラグ
        b.c[9][r] = (x ^ 3) ^ s; // 90/270°相当 ^ 反転フラグ
    }
}

// ======== 評価（B→直接シミュレート：分岐なし） ========
struct EvalResult {
    long long score; // 問題の定義どおり
    int R;           // 未踏マス数
    int usedT;       // 実際に踏破完了したステップ数（R=0時のみ有効）
    bool valid;
};

// 1800手までシミュレーション（R=0で早期return）
static inline EvalResult calc_score_fast_B(const B &b, const Instance &I){
    EvalResult res{0, 0, 0, true};

    // 状態
    uint8_t vis[900]; memset(vis, 0, sizeof(vis));
    int pos[10]; rep(r, I.M) pos[r] = I.init_idx[r];

    int covered = 0;
    auto mark = [&](int id){
        if(!vis[id]){ vis[id] = 1; ++covered; }
    };
    rep(r, I.M) mark(pos[r]);

    const int LIM = 2*I.N*I.N; // 1800
    int t = 0;

    // 先頭 T0 区間
    for(; t<b.T0 && t<LIM; ++t){
        int btn = b.a[t]; // 0..9 を想定
        rep(r, I.M) pos[r] = I.nxt[pos[r]][ b.c[btn][r] ];
        rep(r, I.M) mark(pos[r]);
        if(covered == I.TOT){
            res.R = 0;
            res.usedT = t+1;
            res.score = 3LL*I.TOT - res.usedT;
            return res;
        }
    }

    // 以降は 周期: [8]*W -> [7]*1 -> [9]*W -> [7]*1 -> …
    int phase = 0;                // 0:8ラン, 1:7(1回), 2:9ラン, 3:7(1回)
    int run = 0;
    int runlen = (b.W>=0? b.W:0);
    int btn = (runlen>0 ? 8 : 7);

    for(; t<LIM; ++t){
        if((phase==0 || phase==2) && run==runlen){
            // 8/9 の連打が終わった → 7を1回
            phase ^= 1;
            btn = 7; run = 0; runlen = 1;
        }else if((phase==1 || phase==3) && run==runlen){
            // 7の1回が終わった → 8or9 の連打へ
            phase = (phase==1? 2: 0);
            btn = (phase==0? 8: 9);
            run = 0; runlen = (b.W>=0? b.W:0);
        }

        rep(r, I.M) pos[r] = I.nxt[pos[r]][ b.c[btn][r] ];
        ++run;

        rep(r, I.M) mark(pos[r]);
        if(covered == I.TOT){
            res.R = 0;
            res.usedT = t+1;
            res.score = 3LL*I.TOT - res.usedT;
            return res;
        }
    }

    // 1800手到達までに埋まらなかった
    res.R = I.TOT - covered;
    res.usedT = LIM;
    if(res.R==0) res.score = 3LL*I.TOT - LIM; // ここには通常来ない
    else         res.score = 1LL*I.TOT - res.R;
    return res;
}

// ======== 仕上げ用：最後に新規を踏んだ時点に短縮し、0..6のみで最寄りから埋める ========

struct SimState {
    int pos[10];
    uint8_t vis[900];
    int covered;
};

// btn_of[r][act]（act=0..3）: ロボ r に対し その方向に動かすボタン番号（0..6）を1つ返す。無い場合 -1。
static void build_btn_of(const B& b, const Instance& I, int btn_of[10][4]){
    for(int r=0;r<I.M;r++) for(int a=0;a<4;a++) btn_of[r][a] = -1;
    for(int btn=0; btn<7; ++btn){
        for(int r=0; r<I.M; ++r){
            int act = b.c[btn][r];
            if(0<=act && act<=3 && btn_of[r][act]==-1) btn_of[r][act] = btn;
        }
    }
}

// 最初のスケジュールを 1800 手まで回し、
// - new が起きた最後の時刻の状態（スナップショット st）
// - その時刻までのボタン列 prefix（長さ = t_last+1）
// - 途中で全埋めなら usedT で返す
static void build_prefix_until_last_new(const B& b, const Instance& I,
                                        vector<int>& prefix, SimState& st,
                                        bool& finished, int& usedT)
{
    prefix.clear();
    // 現在状態
    SimState cur;
    memset(cur.vis, 0, sizeof(cur.vis));
    rep(r,I.M) cur.pos[r] = I.init_idx[r];
    cur.covered = 0;
    auto mark = [&](int id){
        if(!cur.vis[id]){ cur.vis[id]=1; ++cur.covered; }
    };
    rep(r,I.M) mark(cur.pos[r]);

    const int LIM = 2*I.N*I.N;
    int phase=0, run=0, runlen=max(0,b.W), btn=(runlen>0?8:7);
    int last_new_t = -1;

    // スナップショット（最後に新規が出た直後の状態）
    SimState snap = cur;

    // 先頭 T0
    int t=0;
    for(; t<b.T0 && t<LIM; ++t){
        int btt = b.a[t];
        prefix.push_back(btt);
        int before = cur.covered;

        rep(r,I.M) cur.pos[r] = I.nxt[cur.pos[r]][ b.c[btt][r] ];
        rep(r,I.M) mark(cur.pos[r]);

        if(cur.covered > before){
            last_new_t = t;
            snap = cur; // 状態を保存
        }
        if(cur.covered == I.TOT){
            finished = true; usedT = t+1;
            st = cur;
            prefix.resize(usedT);
            return;
        }
    }

    // 周期
    for(; t<LIM; ++t){
        if((phase==0||phase==2) && run==runlen){ phase^=1; btn=7; run=0; runlen=1; }
        else if((phase==1||phase==3) && run==runlen){ phase=(phase==1?2:0); btn=(phase==0?8:9); run=0; runlen=max(0,b.W); }

        prefix.push_back(btn);
        int before = cur.covered;

        rep(r,I.M) cur.pos[r] = I.nxt[cur.pos[r]][ b.c[btn][r] ];
        ++run;

        rep(r,I.M) mark(cur.pos[r]);

        if(cur.covered > before){
            last_new_t = t;
            snap = cur;
        }
        if(cur.covered == I.TOT){
            finished = true; usedT = t+1;
            st = cur;
            prefix.resize(usedT);
            return;
        }
    }

    // ここまでで埋まらない → 最後に新規が出た時刻 after を採用
    finished = false;
    if(last_new_t >= 0){
        prefix.resize(last_new_t+1);
        st = snap;
    }else{
        // 1手も新規が出てないなら 0 手から開始
        prefix.clear();
        st = SimState();
        memset(st.vis, 0, sizeof(st.vis));
        rep(r,I.M) st.pos[r] = I.init_idx[r];
        st.covered = 0;
        auto mark0 = [&](int id){ if(!st.vis[id]){ st.vis[id]=1; ++st.covered; } };
        rep(r,I.M) mark0(st.pos[r]);
    }
    usedT = (int)prefix.size();
}

// st から残りを埋める。0..6 ボタンのみ使用。seq に追記。
// budget_rem は「残りで押せる手数」。
static void finish_greedy_nearest(const B& b, const Instance& I,
                                  vector<int>& seq, SimState& st, int budget_rem)
{
    if(budget_rem<=0) return;

    int btn_of[10][4]; build_btn_of(b, I, btn_of);

    auto mark = [&](int id){
        if(!st.vis[id]){ st.vis[id]=1; ++st.covered; }
    };

    // BFS 用配列（使い回し）
    static int dist[10][900];
    static int par_prev[10][900];
    static int par_act[10][900];

    while(budget_rem>0 && st.covered < I.TOT){
        int best_r = -1, best_tgt = -1, best_d = INT_MAX;

        // 各ロボットごとに BFS（0..6で到達できる範囲のみ）
        for(int r=0; r<I.M; ++r){
            if(btn_of[r][0]==-1 && btn_of[r][1]==-1 && btn_of[r][2]==-1 && btn_of[r][3]==-1) continue;

            for(int i=0;i<I.TOT;i++){
                dist[r][i] = INT_MAX;
                par_prev[r][i] = -1;
                par_act[r][i]  = -1;
            }
            deque<int> dq;
            int s = st.pos[r];
            dist[r][s] = 0;
            dq.push_back(s);

            while(!dq.empty()){
                int v = dq.front(); dq.pop_front();
                int dv = dist[r][v];
                for(int a=0;a<4;a++){
                    int btn = btn_of[r][a];
                    if(btn==-1) continue;                 // その方向へ動かすボタンが無い
                    int to = I.nxt[v][a];
                    if(to == v) continue;                 // 壁で動けない
                    if(dist[r][to] != INT_MAX) continue;  // 既訪
                    dist[r][to] = dv + 1;
                    par_prev[r][to] = v;
                    par_act[r][to]  = a;
                    dq.push_back(to);
                }
            }

            for(int id=0; id<I.TOT; ++id){
                if(st.vis[id]) continue;
                int d = dist[r][id];
                if(d < best_d){
                    best_d = d;
                    best_r = r;
                    best_tgt = id;
                }
            }
        }

        if(best_r == -1 || best_d==INT_MAX) break;

        // 経路復元
        vector<int> acts; acts.reserve(best_d);
        for(int cur=best_tgt; cur!=st.pos[best_r]; ){
            int a = par_act[best_r][cur];
            int p = par_prev[best_r][cur];
            if(a==-1 || p==-1){ acts.clear(); break; }
            acts.push_back(a);
            cur = p;
        }
        reverse(acts.begin(), acts.end());
        if(acts.empty()) break;

        bool any_new = false;
        for(int a: acts){
            if(budget_rem<=0) break;
            int btn = btn_of[best_r][a]; // 0..6
            seq.push_back(btn);
            --budget_rem;

            // 全ロボ移動
            for(int rr=0; rr<I.M; ++rr){
                int act = b.c[btn][rr];         // 0..4
                st.pos[rr] = I.nxt[st.pos[rr]][act];
            }
            // 踏破更新
            int before = st.covered;
            for(int rr=0; rr<I.M; ++rr) mark(st.pos[rr]);
            if(st.covered > before) any_new = true;

            if(st.covered>=I.TOT) break;
        }
        if(!any_new) break;
    }
}

// ======== 出力用（最後の一回だけ：仕上げも含む完全列を生成） ========
struct ans{
    int c[10][10];   // 0..9
    int T;           // 操作列長
    int a[1800];     // 操作列（0..K-1）
};

static inline void output(const ans& A){
    rep(i,K){
        rep(j,M){
            cout<<UDLRS[A.c[i][j]]<<(j+1==M?'\n':' ');
        }
    }
    rep(i,A.T) cout<<A.a[i]<<'\n';
}

// 1) 通常スケジュールを 1800 手まで回して「最後に新規が出た時点」で短縮
// 2) 残りは 0..6 のみで最寄りから貪欲に埋める（1800 手まで）
// ※ b.c[7..9] は build_derived_acts 済みであることが前提
static inline ans convert_with_finish(const B& b_in, const Instance& I){
    B b = b_in; // 念のためローカルで派生を再構築
    build_derived_acts(b);

    ans A;
    // 行動表は b.c をそのままコピー（0..9）
    rep(i,10) rep(j,I.M) A.c[i][j] = b.c[i][j];

    vector<int> seq;
    SimState st;
    bool finished=false; int usedT=0;

    build_prefix_until_last_new(b, I, seq, st, finished, usedT);
    if(!finished){
        int total_budget = 2*I.N*I.N;              // 1800
        int remain = max(0, total_budget - (int)seq.size());
        finish_greedy_nearest(b, I, seq, st, remain);
    }
    A.T = min((int)seq.size(), 2*I.N*I.N);
    for(int i=0;i<A.T;i++) A.a[i]=seq[i];
    return A;
}

// ======== ペナルティ（各ロボが U/D/L/R を最低1つずつ持つ） ========
static inline int count_missing_dirs(const B& b, const Instance& I){
    int miss=0;
    for(int r=0;r<I.M;r++){
        bool has[4]={false,false,false,false};
        for(int btn=0; btn<7; ++btn){
            int act=b.c[btn][r];
            if(0<=act && act<=3) has[act]=true;
        }
        for(int a=0;a<4;a++) if(!has[a]) ++miss;
    }
    return miss;
}

struct AugScore {
    long long base;
    long long aug;
};
static inline AugScore eval_aug(const B& b, const Instance& I, int PEN_W){
    auto er = calc_score_fast_B(b, I);
    int miss = count_missing_dirs(b, I);
    long long aug = er.score - 1LL*PEN_W*miss;
    return {er.score, aug};
}

// ======== 初期解の構築（各ロボに最低限の4方向を入れてから残りをランダム） ========
static inline void init_B(B& b, const Instance& I){
    // まず全て S に
    rep(btn,10) rep(r,I.M) b.c[btn][r]=4;
    // 各ロボに U/D/L/R を1つずつ割当（0..6の中で）
    for(int r=0;r<I.M;r++){
        int perm[7]={0,1,2,3,4,5,6};
        for(int i=6;i>0;i--){ int j=RAND()%(i+1); swap(perm[i],perm[j]); }
        b.c[perm[0]][r]=0; // U
        b.c[perm[1]][r]=1; // D
        b.c[perm[2]][r]=2; // L
        b.c[perm[3]][r]=3; // R
        for(int k=4;k<7;k++) b.c[perm[k]][r]=RAND()%5; // 残りランダム
    }
    b.T0 = 20;
    rep(i,b.T0) b.a[i] = RAND()%I.K; // 0..9
    b.W = 30;
    rep(i,I.M) b.d[i] = RAND()%8;    // (d>>1)∈0..3, (d&1)∈0..1
    build_derived_acts(b);           // 7..9 を反映
}

// ======== メイン（局所探索はBのみで回す） ========
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Instance I = read_instance_from_stdin();

    const int PEN_W = 120;          // 1方向欠落あたりのペナルティ
    const int UNS_MAX = 1000;

    B b; init_B(b, I);

    auto sc = eval_aug(b, I, PEN_W);
    long long cur_base = sc.base;
    long long cur_aug  = sc.aug;

    B best_b = b; long long best_base = cur_base;

    int uns = 0, cnt_loop = 0;

    while(true){
        ++cnt_loop;
        if((cnt_loop & 1023) == 0 && clock() >= 1950000) break; // ~1.8s

        B nb = b;
        int rmut = RAND()%5;
        if(rmut==0){
            // ランダム変更
            nb.c[RAND()%7][RAND()%I.M] = RAND()%5;
        }else if(rmut==1){
            if(nb.T0>0) nb.a[RAND()%nb.T0] = RAND()%I.K;
        }else if(rmut==2){
            nb.d[RAND()%I.M] = RAND()%8;
        }else if(rmut==3){
            int w = nb.W + (RAND()%3 - 1); // -1,0,+1
            if(w < 0) w = 0;
            if(w > 60) w = 60;             // 任意上限
            nb.W = w;
        }else{
            // ★ 足りない向きを埋める“指向的”突然変異
            int rr = RAND()%I.M;
            bool has[4]={false,false,false,false};
            for(int btn=0; btn<7; ++btn){
                int act = nb.c[btn][rr];
                if(0<=act && act<=3) has[act]=true;
            }
            int missing[4], mc=0;
            for(int a=0;a<4;a++) if(!has[a]) missing[mc++]=a;
            if(mc>0){
                int want = missing[RAND()%mc];
                int btn = RAND()%7;
                nb.c[btn][rr] = want;        // 1つ強制付与
            }else{
                int btn = RAND()%7;
                nb.c[btn][rr] = RAND()%5;
            }
        }
        build_derived_acts(nb); // 7..9 を更新（d や c を変えた後は必ず）

        auto nsc = eval_aug(nb, I, PEN_W);

        if(nsc.aug > cur_aug || uns >= UNS_MAX){
            b = nb; cur_aug = nsc.aug; cur_base = nsc.base; uns = 0;
        }else{
            ++uns;
        }
        if(nsc.base > best_base){
            best_b = nb; best_base = nsc.base;
        }
    }

    // 出力：仕上げ込みの列を生成（内部で build_derived_acts 再適用）
    ans out = convert_with_finish(best_b, I);

    // 出力
    for(int i=0;i<K;i++){
        for(int j=0;j<M;j++){
            cout<<UDLRS[out.c[i][j]]<<(j+1==M?'\n':' ');
        }
    }
    for(int i=0;i<out.T;i++) cout<<out.a[i]<<'\n';

    cerr << "score=" << best_base << " T=" << out.T << "\n";
    cerr << cnt_loop << endl;
    return 0;
}
