#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using namespace atcoder;

/* alias */
using ull = unsigned long long;
using ll = long long;
using vi = vector<int>;
using vl = vector<long>;
using vll = vector<long long>;
using vvi = vector<vi>;
using vvl = vector<vl>;
using vvll = vector<vll>;
using vs = vector<string>;
using pii = pair<int, int>;
using pll = pair<long long, long long>;
using ppll = pair<pll, pll>;


/* define short */
#define pb push_back
#define mp make_pair
#define fi first
#define se second
#define sz(obj) int(obj.size())
#define all(obj) (obj).begin(), (obj).end()
#define yesno(bool) if(bool){cout<<"yes"<<"\n";}else{cout<<"no"<<"\n";}
#define YesNo(bool) if(bool){cout<<"Yes"<<"\n";}else{cout<<"No"<<"\n";}
#define YESNO(bool) if(bool){cout<<"YES"<<"\n";}else{cout<<"NO"<<"\n";}

/* REP macro */
#define reps(i, a, n) for (int i = (a); i < (int)(n); ++i)
#define rep(i, n) reps(i, 0, n)
#define rep1(i, n) reps(i, 1, n + 1)
#define rrep(i,n) for(int i=n-1;i>=0;i--)
#define rrep1(i,n) for(int i=n;i>=1;i--)

/* input */
#define scint(x) scanf("%d", &x)
#define scchar(x) scanf("%c", &x)
#define scfloat(x) scanf("%f", &x)
#define scdouble(x) scanf("%lf", &x)
#define sclong(x) scanf("%ld", &x)
#define sclonglong(x) scanf("%lld", &x)

/* functions */
template<typename T>
T floor_divide(T a, T b){
    // floor(a/b)を返す. Tはint or llを想定
    // a, bは共に正であることを要請
    assert(a > 0);
    assert(b > 0);
    return (a-1)/b+1;
}

// メルセンヌツイスタを初期化
mt19937 mt(0);
// [a, b] の整数値乱数を返す
int rand(int a, int b) {
    return uniform_int_distribution<int>(a, b)(mt);
}

// 便利関数

// vectorの中身を出力
template<typename T>
void print_vector(vector<T> vec){
    for(auto x : vec){
        cout << x << " ";
    }
    cout << endl;
}

// 入出力関連
class input_info{
    public:
    int N;
    vector<pll> P; // sz(P) == N
    input_info(int N, vector<pll> P) : N(N), P(P) {}
};

// 入力を読み込む
input_info read_input(){
    int N;
    cin >> N;
    vector<pll> P(N);
    rep(i, N){
        ll x, y;
        cin >> x >> y;
        P[i] = mp(x, y);
    }
    return input_info(N, P);
}

// input情報のあるテキストファイルを読み込む
input_info read_input_from_file(string filename){
    ifstream file(filename);
    int N;
    file >> N;
    vector<pll> P(N);
    rep(i, N){
        ll x, y;
        file >> x >> y;
        P[i] = mp(x, y);
    }
    return input_info(N, P);
}
// output情報を表すクラスを作成
class output_info{
    public:
    int M;
    vector<pair<pll, pll>> E; // sz(E) == M
    output_info(int M, vector<pair<pll, pll>> E) : M(M), E(E) {}
};
// 出力を行う
void output(output_info output){
    cout << output.M << endl;
    for(auto e : output.E){
        cout << e.fi.fi << " " << e.fi.se << " " << e.se.fi << " " << e.se.se << endl;
    }
    return;
}
// output情報をテキストファイルに書き込む
void output_to_textfile(output_info output, string filename){
    ofstream file(filename);
    file << output.M << endl;
    for(auto e : output.E){
        file << e.fi.fi << " " << e.fi.se << " " << e.se.fi << " " << e.se.se << endl;
    }
    file.close();
    return;
}

// output情報からスコアを計算
ll score(output_info output){
    int M = output.M;
    vector<pair<pll, pll>> E = output.E;
    ll ret = 0;
    rep(i, M){
        ll x1 = E[i].fi.fi;
        ll y1 = E[i].fi.se;
        ll x2 = E[i].se.fi;
        ll y2 = E[i].se.se;
        ret += (x2-x1) + (y2-y1);
    }
    return ret;
}

output_info reunite_output(output_info output){
    // outputの再構築を行う
    int M = output.M;
    vector<pair<pll, pll>> E = output.E;
    vector<pair<pll, pll>> new_E;
    rep(i, M){
        ppll e = E[i];
        pll p1 = e.fi;
        pll p2 = e.se;
        ll cost = abs(p1.fi - p2.fi) + abs(p1.se - p2.se);
        ll tmp_cost = cost;
        int idx = -1;
        for(int j = 0; j < sz(new_E); j++){
            ppll ne = new_E[j];
            pll np1 = ne.fi;
            pll np2 = ne.se;
            // np1->np2から分岐させる
            pll npm = mp(min(p2.fi, np2.fi), min(p2.se, np2.se));
            // npmがnp1をマジョライズしていない場合はスキップ
            if(np1.fi > npm.fi || np1.se > npm.se){
                continue;
            }
            // new_costはnpm->p2のコスト
            ll new_cost = abs(npm.fi - p2.fi) + abs(npm.se - p2.se);
            // new_costがtmp_costより小さい場合は更新
            if(new_cost < tmp_cost){
                tmp_cost = new_cost;
                idx = j;
            }
        }
        // idxが-1でない場合は更新
        if(idx == -1){
            new_E.pb(e);
        }
        else{
            // new_E[idx]を更新
            ppll ne = new_E[idx];
            pll np1 = ne.fi;
            pll np2 = ne.se;
            pll npm = mp(min(p2.fi, np2.fi), min(p2.se, np2.se));
            // case 1: npm == np2
            // この場合, 単にnew_Eの後ろにeの変形を追加するだけ
            if(npm == np2){
                ppll pe = mp(np2, p2);
                new_E.pb(pe);
            }
            // case 2: npm == p2
            // この場合, new_E[idx]を更新し, かつその位置に挿入する
            else if(npm == p2){
                new_E[idx] = mp(np1, p2);
                // idxの位置にmp(p2, np2)を挿入
                new_E.insert(new_E.begin()+idx+1, mp(p2, np2));
            }
            // case 3: それ以外
            // この場合, new_E[idx]を更新し, かつその位置に挿入する
            else{
                new_E[idx] = mp(np1, npm);
                // idxの位置にmp(npm, np2)を挿入
                new_E.insert(new_E.begin()+idx+1, mp(npm, np2));
                // idxの位置にmp(p2, np2)を挿入
                new_E.insert(new_E.begin()+idx+2, mp(npm, p2));
            }
        }
    }
    return output_info(sz(new_E), new_E);
}

output_info solve_by_greedy(input_info input){
    int N = input.N;
    vector<pll> P = input.P;
    vector<ppll> E;
    // setについて, (x+y, (x, y))を入れてx+yでソートする
    set<pair<ll, pll>> S;
    // setにPを入れる
    rep(i, N){
        S.insert(mp(P[i].fi + P[i].se, P[i]));
    }
    // ここから処理
    // sz(S) > 1の間続ける
    while(sz(S) > 1){
        // setのうち最大のものを取り出す
        auto it = S.end();
        it--;
        ppll tmp_pair = mp(mp(0, 1e10), mp(0, 1e10));
        tmp_pair.se = it->se;
        // 残りのものでマンハッタン距離が最小のものを取り出す
        S.erase(it);
        ll min_dist = 1e10;
        for(auto p : S){
            ll dist = abs(p.se.fi - tmp_pair.se.fi) + abs(p.se.se - tmp_pair.se.se);
            if(dist < min_dist){
                min_dist = dist;
                tmp_pair.fi = p.se;
            }
        }
        // 取り出したものをSからerase
        S.erase(mp(tmp_pair.fi.fi + tmp_pair.fi.se, tmp_pair.fi));

        // Eに追加
        pll p1 = tmp_pair.fi;
        ll x1 = tmp_pair.fi.fi;
        ll y1 = tmp_pair.fi.se;
        pll p2 = tmp_pair.se;
        ll x2 = tmp_pair.se.fi;
        ll y2 = tmp_pair.se.se;

        ll xm = min(x1, x2);
        ll ym = min(y1, y2);
        pll pm = mp(xm, ym);
        // case 1: pm == p1
        if(x1 < x2 && y1 < y2){
            E.pb(mp(pm, p2));
            // pmをinsert
            S.insert(mp(pm.fi + pm.se, pm));
        }
        // case 2: pm == p2
        else if(x1 > x2 && y1 > y2){
            E.pb(mp(pm, p1));
            // pmをinsert
            S.insert(mp(pm.fi + pm.se, pm));
        }
        // case 3: pm != p1, pm != p2
        else{
            E.pb(mp(pm, p1));
            E.pb(mp(pm, p2));
            // pmをinsert
            S.insert(mp(pm.fi + pm.se, pm));
        }
    }
    // 最後1つになったら, 0, 0に繋ぐ
    pll p1 = S.begin()->se;
    E.pb(mp(mp(0, 0), p1));
    // 処理ここまで

    // 出力を作成
    // Eを反対にしたvectorを作成
    reverse(all(E));
    int M = sz(E);
    output_info output(M, E);
    return output;
}

ll weight(ll a, ll b, pll p){
    return a*p.fi + b*p.se;
}

output_info solve_by_greedy2raw(input_info input, ll a, ll b){
    // a, bを使ってSのスコアから重みづけを行う
    int N = input.N;
    vector<pll> P = input.P;
    vector<ppll> E;
    // setについて, (x+y, (x, y))を入れてx+yでソートする
    set<pair<ll, pll>> S;
    // setにPを入れる
    rep(i, N){
        ll w = weight(a, b, P[i]);
        S.insert(mp(w, P[i]));
    }
    // ここから処理
    // sz(S) > 1の間続ける
    while(sz(S) > 1){
        // setのうち最大のものを取り出す
        auto it = S.end();
        it--;
        ppll tmp_pair = mp(mp(0, 1e10), mp(0, 1e10));
        tmp_pair.se = it->se;
        // 残りのものでマンハッタン距離が最小のものを取り出す
        S.erase(it);
        ll min_dist = 1e10;
        for(auto p : S){
            ll dist = abs(p.se.fi - tmp_pair.se.fi) + abs(p.se.se - tmp_pair.se.se);
            if(dist < min_dist){
                min_dist = dist;
                tmp_pair.fi = p.se;
            }
        }
        // 取り出したものをSからerase
        S.erase(mp(weight(a, b, tmp_pair.fi), tmp_pair.fi));

        // Eに追加
        pll p1 = tmp_pair.fi;
        ll x1 = tmp_pair.fi.fi;
        ll y1 = tmp_pair.fi.se;
        pll p2 = tmp_pair.se;
        ll x2 = tmp_pair.se.fi;
        ll y2 = tmp_pair.se.se;

        ll xm = min(x1, x2);
        ll ym = min(y1, y2);
        pll pm = mp(xm, ym);
        // case 1: pm == p1
        if(x1 < x2 && y1 < y2){
            E.pb(mp(pm, p2));
            // pmをinsert
            S.insert(mp(weight(a, b, pm), pm));
        }
        // case 2: pm == p2
        else if(x1 > x2 && y1 > y2){
            E.pb(mp(pm, p1));
            // pmをinsert
            S.insert(mp(weight(a, b, pm), pm));
        }
        // case 3: pm != p1, pm != p2
        else{
            E.pb(mp(pm, p1));
            E.pb(mp(pm, p2));
            // pmをinsert
            S.insert(mp(weight(a, b, pm), pm));
        }
    }
    // 最後1つになったら, 0, 0に繋ぐ
    pll p1 = S.begin()->se;
    E.pb(mp(mp(0, 0), p1));
    // 処理ここまで

    // 出力を作成
    // Eを反対にしたvectorを作成
    reverse(all(E));
    int M = sz(E);
    output_info output(M, E);

    // outputを再構築
    output_info output2 = reunite_output(output);

    return output2;
}

output_info reunite_output_backward(output_info output, ll a, ll b){
    // outputの再構築を後ろから行う
    int M = output.M;
    vector<pair<pll, pll>> E = output.E;
    // Eからpllを取り出す
    set<pll> S;
    for(auto e : E){
        S.insert(e.fi);
        S.insert(e.se);
    }
    // (0, 0)は削除
    S.erase(mp(0ll, 0ll));
    //Sの要素としてinputを作成
    vector<pll> P;
    for(auto s : S){
        P.pb(s);
    }
    int nN = sz(P);
    input_info ninput = input_info(nN, P);
    // 再構築
    output_info noutput = solve_by_greedy2raw(ninput, a, b);
    // outputを縮約
    // p1 = p2のものを削除
    vector<pair<pll, pll>> nnE;
    rep(i, noutput.M){
        if(noutput.E[i].fi != noutput.E[i].se){
            nnE.pb(noutput.E[i]);
        }
    }
    noutput = output_info(sz(nnE), nnE);
    return noutput;
}



output_info solve_by_greedy2(input_info input){
    // a, bを変えてみる
    vector<output_info> outputs;
    outputs.pb(solve_by_greedy2raw(input, (ll)0, (ll)1));
    outputs.pb(solve_by_greedy2raw(input, (ll)1, (ll)0));
    outputs.pb(solve_by_greedy2raw(input, (ll)1, (ll)1));
    // a, b <= 10でgcd(a, b) == 1なものを試す
    ll abmx = 13;
    for(ll a = 1; a <= abmx; a++){
        for(ll b = 1; b <= abmx; b++){
            if(gcd(a, b) != 1){
                continue;
            }
            // aとbが離れすぎているものはスキップ
            if(abs(a-b) * 2 > max(a, b)){
                continue;
            }
            output_info op = solve_by_greedy2raw(input, a, b);
            outputs.pb(op);
            output_info opb = reunite_output_backward(op, a, b);
            outputs.pb(opb);
        }
    }

    // スコアが最小のものを返す
    ll min_score = 1e18;
    output_info ret = outputs[0];
    for(auto output : outputs){
        // outputのうち, M > 5*Nのものはスキップ
        if(output.M > 5*input.N){
            continue;
        }
        ll sc = score(output);
        if(sc < min_score){
            min_score = sc;
            ret = output;
        }
    }

    return ret;
}




int debug(){
    // code
    // 入力をテキストファイルから受け取る
    input_info input = read_input_from_file("in/0000.txt");
    // input_infoの確認
    cout << input.N << endl;
    for(auto p : input.P){
        cout << p.fi << " " << p.se << endl;
    }

    // 処理を行う
    // 時間を計測
    auto start = chrono::system_clock::now();
    auto output = solve_by_greedy2(input);
    auto end = chrono::system_clock::now();
    // 時間を出力
    double elapsed = chrono::duration_cast<chrono::milliseconds>(end-start).count();
    cout << "elapsed: " << elapsed << "ms" << endl;
    // 出力を行う
    output_to_textfile(output, "out/code07/0000.txt");

    return 0;
}

int submit(){
    // code
    input_info input = read_input();
    auto so = solve_by_greedy2(input);
    output(so);
    return 0;
}

int main() {
    // code
    // debug();
    submit();
    return 0;
}