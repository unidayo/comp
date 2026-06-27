#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;
#define inf 1000000007
#define MP make_pair
#define MT make_tuple
#define PB push_back
#define fi first
#define se second
#define rep(i,n) for(int i = 0; i < (int)(n); ++i)
#define rrep(i,n) for(int i = (int)n-1; i >= 0; --i)
#define srep(i,a,b) for(int i = (int)a; i < (int)(b); ++i)
#define all(x) (x).begin(),(x).end()
#define SUM(v) accumulate(all(v), 0LL)
#define MIN(v) *min_element(all(v))
#define MAX(v) *max_element(all(v))
#define lb(c, x) distance((c).begin(), lower_bound(all(c), (x)))
#define ub(c, x) distance((c).begin(), upper_bound(all(c), (x)))
#define UNIQUE(x) sort(all(x)), x.erase(unique(all(x)), x.end())
#define SZ(c) (int)(c).size()
template<typename T>
ostream& operator << (ostream& os, vector<T>& vec) {
    os << "{";
    for (int i = 0; i<(int)vec.size(); i++) {
        os << vec[i] << (i + 1 == (int)vec.size() ? "" : ", ");
    }
    os << "}";
    return os;
}
// pair出力
template<typename T, typename U>
ostream& operator << (ostream& os, pair<T, U> pair_var) {
    os << "(" << pair_var.first << ", " << pair_var.second << ")";
    return os;
}
// map出力
template<typename T, typename U>
ostream& operator << (ostream& os, map<T, U>& map_var) {
    os << "{";
    for(auto itr = map_var.begin(); itr != map_var.end(); itr++){
        os << "(" << itr->first << ", " << itr->second << ")";
        itr++;
        if(itr != map_var.end()) os << ", ";
        itr--;
    }
    os << "}";
    return os;
}
// set 出力
template<typename T>
ostream& operator << (ostream& os, set<T>& set_var) {
    os << "{";
    for(auto itr = set_var.begin(); itr != set_var.end(); itr++){
        os << (*itr);
        ++itr;
        if(itr != set_var.end()) os << ", ";
        itr--;
    }
    os << "}";
    return os;
}
// tuple 出力
template<int N,class Tuple>
void out(ostream &os,const Tuple &t){}
template<int N,class Tuple,class H,class ...Ts>
void out(ostream &os,const Tuple &t){
    if(N)os<<", ";
    os<<get<N>(t);
    out<N+1,Tuple,Ts...>(os,t);
}
template<class ...Ts>
ostream& operator<<(ostream &os, const tuple<Ts...> &t){
    os<<"(";
    out<0,tuple<Ts...>,Ts...>(os,t);
    os<<")";
    return os;
}
#define overload2(_1, _2, name, ...) name
#define vec(type, name, ...) vector<type> name(__VA_ARGS__)
#define VEC(type, name, size)                                                                                                                                  \
    vector<type> name(size);                                                                                                                                   \
    IN(name)
#define vv(type, name, h, ...) vector<vector<type>> name(h, vector<type>(__VA_ARGS__))
#define VV(type, name, h, w)                                                                                                                                   \
    vector<vector<type>> name(h, vector<type>(w));                                                                                                             \
    IN(name)
#define vvv(type, name, h, w, ...) vector<vector<vector<type>>> name(h, vector<vector<type>>(w, vector<type>(__VA_ARGS__)))
#define vvvv(type, name, a, b, c, ...)                                                                                                                         \
    vector<vector<vector<vector<type>>>> name(a, vector<vector<vector<type>>>(b, vector<vector<type>>(c, vector<type>(__VA_ARGS__))))
#define INT(...)                                                                                                                                               \
    int __VA_ARGS__;                                                                                                                                           \
    IN(__VA_ARGS__)
#define LL(...)                                                                                                                                                \
    ll __VA_ARGS__;                                                                                                                                            \
    IN(__VA_ARGS__)
#define STR(...)                                                                                                                                               \
    string __VA_ARGS__;                                                                                                                                        \
    IN(__VA_ARGS__)
#define CHR(...)                                                                                                                                               \
    char __VA_ARGS__;                                                                                                                                          \
    IN(__VA_ARGS__)
#define DBL(...)                                                                                                                                               \
    double __VA_ARGS__;                                                                                                                                        \
    IN(__VA_ARGS__)
int scan() { return getchar(); }
void scan(int &a) { cin >> a; }
void scan(long long &a) { cin >> a; }
void scan(char &a) { cin >> a; }
void scan(double &a) { cin >> a; }
void scan(string &a) { cin >> a; }
template <class T, class S> void scan(pair<T, S> &p) { scan(p.first), scan(p.second); }
template <class T> void scan(vector<T> &);
template <class T> void scan(vector<T> &a) {
    for(auto &i : a) scan(i);
}
template <class T> void scan(T &a) { cin >> a; }
void IN() {}
template <class Head, class... Tail> void IN(Head &head, Tail &...tail) {
    scan(head);
    IN(tail...);
}
const string YESNO[2] = {"NO", "YES"};
const string YesNo[2] = {"No", "Yes"};
const string yesno[2] = {"no", "yes"};
void YES(bool t = 1) { cout << YESNO[t] << endl; }
void NO(bool t = 1) { YES(!t); }
void Yes(bool t = 1) { cout << YesNo[t] << endl; }
void No(bool t = 1) { Yes(!t); }
void yes(bool t = 1) { cout << yesno[t] << endl; }
void no(bool t = 1) { yes(!t); }
#ifdef LOCAL
void debug_out() { cerr << endl; }
template <typename Head, typename... Tail>
void debug_out(Head H, Tail... T) {
    cerr << " " << H;
    debug_out(T...);
}
#define dbg(...) \
    cerr << __LINE__ << " [" << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)
#define dump(x) cerr << __LINE__ << " " << #x << " = " << (x) << endl
#else
#define dbg(...) (void(0))
#define dump(x) (void(0))
#endif
template<typename A, typename T>
std::enable_if_t<std::is_convertible<T, A>::value> fill(A& array, const T& val)
{
    array = val;
}
template<typename A, typename T>
std::enable_if_t<!std::is_convertible<T, A>::value> fill(A& array, const T& val)
{
    for (auto& a : array) {
        fill(a, val);
    }
}
template <typename T, typename S> T ceil(T x, S y) {
    assert(y);
    return (y < 0 ? ceil(-x, -y) : (x > 0 ? (x + y - 1) / y : x / y));
}
template <typename T, typename S> T floor(T x, S y) {
    assert(y);
    return (y < 0 ? floor(-x, -y) : (x > 0 ? x / y : x / y - (x % y == 0 ? 0 : 1)));
}
vector<int> iota(int n) {vector<int> a(n);iota(all(a), 0);return a;}
template <class T> T POW(T x, int n) {T res = 1;for(; n; n >>= 1, x *= x){if(n & 1) res *= x;}return res;}
ll pow2(int i) { return 1LL << i; }
int topbit(signed t) { return t == 0 ? -1 : 31 - __builtin_clz(t); }
int topbit(ll t) { return t == 0 ? -1 : 63 - __builtin_clzll(t); }
int lowbit(signed a) { return a == 0 ? 32 : __builtin_ctz(a); }
int lowbit(ll a) { return a == 0 ? 64 : __builtin_ctzll(a); }
// int allbit(int n) { return (1 << n) - 1; }
ll allbit(ll n) { return (1LL << n) - 1; }
int popcount(signed t) { return __builtin_popcount(t); }
int popcount(ll t) { return __builtin_popcountll(t); }
bool ispow2(int i) { return i && (i & -i) == i; }


template <class S> void fold_in(vector<S> &v) {}
template <typename Head, typename... Tail, class S> void fold_in(vector<S> &v, Head &&a, Tail &&...tail) {
    for(auto e : a) v.emplace_back(e);
    fold_in(v, tail...);
}
template <class S> void renumber(vector<S> &v) {}
template <typename Head, typename... Tail, class S> void renumber(vector<S> &v, Head &&a, Tail &&...tail) {
    for(auto &&e : a) e = lb(v, e);
    renumber(v, tail...);
}
template <class S, class... Args> void zip(vector<S> &head, Args &&...args) {
    vector<S> v;
    fold_in(v, head, args...);
    sort(all(v)), v.erase(unique(all(v)), v.end());
    renumber(v, head, args...);
}
template<class T> inline bool chmax(T &a, T b){
    if(a<b){
        a = b;
        return true;
    }
    return false;
}
template<class T> inline bool chmin(T &a, T b){
    if(a>b){
        a = b;
        return true;
    }
    return false;
}

class XorShift {
   public:
	unsigned int x, y, z, w;
	double nL[65536];
 
	XorShift() {
		init();
	}
 
	void init() {
		x = 314159265;
		y = 358979323;
		z = 846264338;
		w = 327950288;
		double n = 1 / (double)(2 * 65536);
		for (int i = 0; i < 65536; i++) {
			nL[i] = log(((double)i / 65536) + n);
		}
	}
 
	inline unsigned int next() {
		unsigned int t = x ^ x << 11;
		x = y;
		y = z;
		z = w;
		return w = w ^ w >> 19 ^ t ^ t >> 8;
	}
 
	inline double nextLog() {
		return nL[next() & 0xFFFF];
	}
 
	inline int nextInt(int m) {
		return (int)(next() % m);
	}
 
	int nextInt(int min, int max) {
		return min + nextInt(max - min + 1);
	}
 
	inline double nextDouble() {
		return (double)next() / ((long long)1 << 32);
	}
};
XorShift rnd;

using namespace std;

using ull=unsigned long long;


int n = 30;
int BALL = 465;
int a[31][31];
int pyramid[465];
int initial[465];
int id[31][31];
int X[465];
int Y[465];
int pos[465];
int h[465];
int hs[465*465];
bool INSIDE[465*2];
int NEXT[465*2];

int opX[465*465];
int opY[465*465];

vector<vector<int>> g;
ull INF = (1ull<<60);
bool inside(int x,int y){
        if(x<0||x>=n||y<0||y>x)return false;
        return true;
};
bool check(int ID,int b){
    if(INSIDE[ID*2]&&pyramid[NEXT[ID*2]] > b){
        return false;
    }
    if(INSIDE[ID*2+1]&&pyramid[NEXT[ID*2+1]] > b){
        return false;
    }
    return true;
};

ull check_score = 100000;
bool is_finished = false;

struct State{
    int current_id = 0;
    // 初期状態の生成
    State(){
        rep(i,465){
            if(check(pos[i],i))current_id++;
            else break;
        }
        // // cerr << current_id << endl;
        // dbg(current_id);
    }

    ull score(){
        ull z = 0;
        rep(i,465){
            z += h[i] * pyramid[i];
        }
        return z + check_score * current_id;
        // TODO
    }

    ull hash(){
        ull X = 0;
        rep(i,465){
            X ^= hs[465*i + pyramid[i]];
        }
        return X;
        // TODO
    }

    // スコアとハッシュの差分計算
    // 状態は更新しない
    pair<ull,ull> try_apply(ull op,ull score,ull hash){
        // cerr << "old: " << score << " " << hash << endl;
        int x = opX[op];
        int y = opY[op];
        int bx = pyramid[x];
        int by = pyramid[y];
        hash ^= hs[465*x + by];
        hash ^= hs[465*y + bx];
        score += h[x] * by;
        score += h[y] * bx;
        score -= h[x] * bx;
        score -= h[y] * by;
        
        
       
        swap(pos[bx],pos[by]);
        swap(pyramid[x],pyramid[y]);
        int t = pyramid[y] ;
        while(t!=465&&check(pos[t],t))t++;
        score += check_score * (t-bx);
        if(t==465)score += INF;
        // cerr << t << " " << bx << endl;
        swap(pos[bx],pos[by]);
        swap(pyramid[x],pyramid[y]);
        // cerr << "new: " << score << " " << hash << endl;
        return {score,hash};
        // TODO
    }

    // 状態を更新する
    // 元の状態に戻すための情報を返す
    ull apply(ull op){
        int x = opX[op];
        int y = opY[op];
        int bx = pyramid[x];
        int by = pyramid[y];
        swap(pos[bx],pos[by]);
        swap(pyramid[x],pyramid[y]);
        ull backup = op + (ull)current_id * 465*465;
        while(current_id!=465&&check(pos[current_id],current_id))current_id++;
        return backup;
        // TODO
    }
    
    // applyから返された情報をもとに状態を元に戻す
    void back(ull backup){
        current_id = backup/(465*465);
        int x = opX[(backup%(465*465))];
        int y = backup%465;
        int bx = pyramid[x];
        int by = pyramid[y];
        swap(pos[bx],pos[by]);
        swap(pyramid[x],pyramid[y]);
        // TODO
    }
};

struct Node;

struct Kouho{
    ull op;
    shared_ptr<Node> parent;
    ull score;
    ull hash;
    ull p; // 優先度(複数もたせたほうがいい場合があるかもしれない。)
};

using Parent=optional<pair<ull,shared_ptr<Node>>>;
using Children=vector<pair<ull,weak_ptr<Node>>>;

struct Node{
    Parent parent; // 操作、親への参照
    Children child; // 操作、子への参照
    ull score;
    ull hash;
    
    Node(Parent parent,Children child,ull score,ull hash):
        parent(parent),child(child),score(score),hash(hash){}
};


// 多スタート用に構造体にまとめておくと楽
struct Tree{
    State state;
    shared_ptr<Node> node;
    ull rank;
    
    // 注意: depthは深くなっていくごとに-1されていく
    void dfs(vector<Kouho>& next_states,bool one,ull& p,ull depth,int turn){
        if(depth==0){
            ull score=node->score;
            ull hash=node->hash;

            // 検算
            // assert(score==state.score());
            // assert(hash==state.hash());

            // 次の操作を列挙
            int X = state.current_id;
            // cerr << turn << " " << X << endl;
            if(X<465){
                for(auto y:g[pos[X]]){
                    if(pyramid[y]<X)continue;
                    auto [next_score,next_hash]=state.try_apply(pos[X]*465 + y,score,hash);
                    next_states.emplace_back(Kouho{pos[X]*465+y,node,next_score,next_hash,p});
                    p+=1;
                }
            }
        }
        else{
            auto node_backup=node;
            auto child=&node_backup->child;
            // 有効な子だけにする
            child->erase(remove_if(child->begin(),child->end(),[](pair<ull,weak_ptr<Node>>& x){return x.second.expired();}),child->end());

            bool next_one=one&child->size()==1;
            
            // 定数調整の必要あり
            if(depth==50){
                p=0;
            }
            ++rank;

            for(const auto& [op,ptr]:*child){
                node=ptr.lock();
                ull backup=state.apply(op);
                dfs(next_states,next_one,p,depth-1,turn);
                
                if(!next_one){
                    state.back(backup);
                }
            }
            
            if(!next_one){
                node=node_backup;
                --rank;
            }
        }
    }
};

vector<ull> beam(){
    constexpr ull TURN=2500;
    constexpr ull M=900; // ビーム幅

    State state;
    ull score=state.score();
    ull hash=state.hash();

    Tree tree{move(state),shared_ptr<Node>(new Node(Parent(),Children(),score,hash)),0};

    vector<shared_ptr<Node>> cur{tree.node};
    vector<Kouho> next_states;

    unordered_set<ull> set;
    
    for(ull i=0;i<TURN;++i){
        if(is_finished)break;
        next_states.clear();
        ull tmp=0;
        tree.dfs(next_states,true,tmp,i-tree.rank,i);

        if(i+1!=TURN){
            // 上位M個を残す
            if(next_states.size()>M){
                nth_element(next_states.begin(),next_states.begin()+M,next_states.end(),[](Kouho& a,Kouho& b){
                    if(a.score==b.score){
                        return a.p>b.p;
                    }
                    else{
                        return a.score>b.score;
                    }
                });
                next_states.erase(next_states.begin()+M,next_states.end());
            }

            cur.clear();
            set.clear();
            for(const auto&[op,parent,next_score,next_hash,p]:next_states){
                // 重複除去
                if(set.count(next_hash)==0){
                    set.insert(next_hash);
                    auto child_ptr=shared_ptr<Node>(new Node(Parent({op,parent}),Children(),next_score,next_hash));
                    parent->child.emplace_back(op,weak_ptr<Node>(child_ptr));
                    cur.emplace_back(child_ptr);
                }
                if(next_score >= INF)is_finished = true;
            }
        }
    }

    // 最良の状態を選択
    int arg_max=-1;
    int max=0;
    for(ull i=0;i<next_states.size();++i){
        if(next_states[i].score>=max){
            max=next_states[i].score;
            arg_max=i;
        }
    }
    dbg(arg_max);
    auto [op,ptr,best_score,_,__]=next_states[arg_max];

    vector<ull> ret{op};
    cerr<<"score: "<<best_score<<endl;
    cerr<<"rank: "<<TURN-tree.rank<<endl;

    // 操作の復元
    while(ptr->parent.has_value()){
        auto [op,parent]=ptr->parent.value();
        ret.emplace_back(op);
        ptr=parent;
    }

    reverse(ret.begin(),ret.end());
    return ret;
}
// https://qiita.com/rhoo/items/f2be256cde5ad2e62dde

// if(inside(x-1,y-1)&&pyramid[id[x-1][y-1]] > b){
//         return false;
//     }
//     if(inside(x-1,y)&&pyramid[id[x-1][y]] > b){
//         return false;
//     }

int main(){
    rep(i,465*465){
        hs[i] = rnd.next();
        opX[i] = i/465;
        opY[i] = i%465;
    }
    int CCCCC = 0;
    rep(i,n){
        rep(j,i+1){
            cin >> a[i][j];
            id[i][j] = CCCCC++;
            pyramid[id[i][j]] = a[i][j];
            pos[a[i][j]] = id[i][j];
            h[id[i][j]] = i;
            X[id[i][j]] = i;
            Y[id[i][j]] = j;
            initial[id[i][j]] = a[i][j];
        }
    }
    rep(i,n){
        rep(j,n){
            INSIDE[id[i][j]*2] = inside(i-1,j-1);
            INSIDE[id[i][j]*2+1] = inside(i-1,j);
            if(INSIDE[id[i][j]*2]){
                NEXT[id[i][j]*2] = id[i-1][j-1];
            }
            if(INSIDE[id[i][j]*2+1]){
                NEXT[id[i][j]*2+1] = id[i-1][j];
            }
        }
    }
    g.resize(BALL);
    rep(i,n){
        if(i!=n-1){
            rep(j,i+1){
                g[id[i+1][j]].push_back(id[i][j]);    
                g[id[i+1][j+1]].push_back(id[i][j]);
            }
        }
        rep(j,i){
            g[id[i][j]].push_back(id[i][j+1]);
            g[id[i][j+1]].push_back(id[i][j]);
        }
    }
    auto ret = beam();
    cout << ret.size() << "\n";
    for(auto op:ret){
        int x = op/465;
        int y = op%465;
        cout << X[x] << " " << Y[x] << " " << X[y] << " " << Y[y] << "\n";
    }
    return 0;
}