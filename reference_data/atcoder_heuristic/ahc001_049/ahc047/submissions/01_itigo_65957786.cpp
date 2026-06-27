#include <bits/stdc++.h>
using namespace std;
#define rep(i,n) for(int (i)=0;(i)<(n);(i)++)
#define ll long long
#define pp pair<ll,ll>
#define ld long double
#define all(a) (a).begin(),(a).end()
#define mk make_pair
#define difine define
#define Yes(q) cout << ((q) ? "Yes" : "No") << endl;
int inf=1000000000;
constexpr ll INF=2e18;
constexpr ll mod=1000000007;
constexpr ll MOD=998244353;
int dx[4]={1,0,-1,0},dy[4]={0,-1,0,1};
int dx1[8]={0,0,-1,1,1,1,-1,-1},dy1[8]={-1,1,0,0,1,-1,1,-1};

const int N = 12;
const double EPS = 1e-9;


// #include <atcoder/all>
// typedef atcoder::modint998244353 mint;
// typedef atcoder::modint1000000007 mint;

// ガウス消去法による連立一次方程式の解法
vector<double> gaussElimination(vector<vector<double>>& A, vector<double>& b) {
    for (int i = 0; i < N; ++i) {
        // ピボット選択
        int pivot = i;
        for (int j = i + 1; j < N; ++j) {
            if (fabs(A[j][i]) > fabs(A[pivot][i])) {
                pivot = j;
            }
        }
        if (fabs(A[pivot][i]) < EPS) continue; // 0であればスキップ
        
        // 行を入れ替え
        swap(A[i], A[pivot]);
        swap(b[i], b[pivot]);
        
        // 前進消去
        for (int j = i + 1; j < N; ++j) {
            double factor = A[j][i] / A[i][i];
            for (int k = i; k < N; ++k) {
                A[j][k] -= factor * A[i][k];
            }
            b[j] -= factor * b[i];
        }
    }
    
    // 後退代入
    vector<double> x(N, 0.0);
    for (int i = N - 1; i >= 0; --i) {
        if (fabs(A[i][i]) < EPS) continue; // 解が不定の場合
        x[i] = b[i];
        for (int j = i + 1; j < N; ++j) {
            x[i] -= A[i][j] * x[j];
        }
        x[i] /= A[i][i];
    }
    return x;
}

// 定常状態を求める関数
vector<double> findStationaryState(const vector<vector<double>>& P) {
    vector<vector<double>> A(N, vector<double>(N, 0.0));
    vector<double> b(N, 0.0);
    
    // (P^T - I) を設定
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[j][i] = P[i][j] - (i == j ? 1.0 : 0.0);
        }
    }
    
    // 確率条件 Σπ_i = 1 を追加
    for (int i = 0; i < N; ++i) {
        A[0][i] = 1.0;
    }
    b[0] = 1.0;
    
    // ガウス消去法で解く
    vector<double> pi = gaussElimination(A, b);
    
    // 正規化（確率ベクトルにする）
    double sum = 0.0;
    for (double val : pi) sum += val;
    for (double& val : pi) val /= sum;
    
    return pi;
}

struct sol{
    int n,m,l;
    vector<string> s;
    vector<int> p;

    vector<vector<ld>> prob;
    vector<char> ch;
    
    void input(){
        cin >> n >> m >> l;
        s.resize(n);
        p.resize(n);
        rep(i,n){
            cin >> s[i] >> p[i];
        }
        prob.resize(12);
        rep(i,12) prob[i].resize(12);
        ch.resize(12);
        rep(i,12) ch[i]='a'+(i%6);
    }
    
    
    ld cal_score(vector<vector<int>> &ansi,bool hevy){
        vector<vector<double>> ans(m,vector<double>(m));
        rep(i,m){
            rep(j,m) ans[i][j]=ansi[i][j]/(ld)100;
        }
        vector<double> stationary_state=findStationaryState(ans);
        ld score=0;
        rep(i,n){
            ld sc=0;
            rep(j,12){
                rep(k,12) prob[j][k]=0;
            }
            rep(j,12){
                if (s[i][0]==ch[j]) prob[0][j]=stationary_state[j];
            }
            for(int j=1;j<s[i].size();j++){
                rep(k,m){
                    if (s[i][j-1]!=ch[k]) continue;
                    rep(kk,m){
                        if (s[i][j]!=ch[kk]) continue;
                        prob[j][kk]+=prob[j-1][k]*ans[k][kk];
                    }
                }
                if (j==s[i].size()-1){
                    rep(k,m) sc+=prob[j][k];
                }
            }
            if (hevy) score+=pow(p[i],1.8)*pow((1.0-pow(1.0-sc,1000000-s[i].size())),0.4);
            else score+=p[i]*(1.0-pow(1.0-sc,1000000-s[i].size()));
        }
        return score;
    }

    void output(vector<vector<int>> ans){
        rep(i,m){
            cout << ch[i] << " ";
            rep(j,m) {
                cout << ans[i][j] << " ";
            }
            cout << endl;
        }
    }

    vector<vector<int>> make_initial(){
        vector<vector<int>> ans(m,vector<int>(m));
        vector<vector<int>> anss(m,vector<int>(m));
        rep(i,m){
            rep(j,m) ans[i][j]=rand()%100;
            int S=0;
            rep(j,m) S+=ans[i][j];
            int sum=0,sum2=0;
            rep(j,m){
                anss[i][j]=((100*(sum2+ans[i][j])/S)-sum);
                sum+=anss[i][j];
                sum2+=ans[i][j];
            }
        }
        return anss;
    }

    void annealing(){
        vector<vector<int>> best_ans;
        ld best_score=-inf;

        rep(iii,8){
            vector<vector<int>> ans(m,vector<int>(m));
            ld sc=-inf;
            rep(i,200){
                vector<vector<int>> temp=make_initial();
                ld score=cal_score(temp,true);
                if (score>sc){
                    sc=score;
                    ans=temp;
                }
            }
            int co=0;
            ld start_temp=1, end_temp=0.0001;
            ld start_time=clock();
            bool hevy=true;
            ld an_time=0.235;
            while(clock()-start_time < CLOCKS_PER_SEC*an_time){
                int r=rand()%m;
                int ra=rand()%m,ri=rand()%m;
                if (ra==ri) continue;
                int diff=1+rand()%10;
                if (ans[r][ra]-diff<0) continue;
                if (clock()-start_time > CLOCKS_PER_SEC*an_time*2/3 && hevy){
                    hevy=false;
                    sc=cal_score(ans,hevy);
                    if (sc>best_score) {
                        best_score=sc;
                        best_ans=ans;
                    }
                }
                ans[r][ra]-=diff;
                ans[r][ri]+=diff;
                ld score=cal_score(ans,hevy);
                double temp = start_temp + (end_temp - start_temp) * (clock()-start_time) / (CLOCKS_PER_SEC*an_time);
                ld prob = exp((score-sc)/temp);
                if (prob > (rand())/(double)RAND_MAX){
                    sc=score;
                    if (best_score<score && !hevy){
                        best_score=score;
                        best_ans=ans;
                    }
                } else {
                    ans[r][ra]+=diff;
                    ans[r][ri]-=diff;
                }
            }
        }
        output(best_ans);
        cerr << best_score << endl;
    }
};

int main() {
    sol S;
    S.input();
    S.annealing();
}