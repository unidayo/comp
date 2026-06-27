#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC target "sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2"
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


#include <sys/time.h>
inline long long GetTSC() {
  long long lo, hi;
  asm volatile ("rdtsc": "=a"(lo), "=d"(hi));
  return lo + (hi << 32);
}
inline double GetSeconds() {
  return GetTSC() / 3.0e9;
}
double starttime;


template<typename T> class Hungarian
{
private:
    const int U, V;
    vector<vector<int> > graph;
    vector<T> dual;
    vector<int> alloc, rev_alloc, prev;
    const vector<vector<T> >& cost;
    int matching_size;
    T diff(const int i, const int j){
        return cost[i][j] - dual[i] - dual[U + j];
    }
    void init_feasible_dual(){
        for(int i = 0; i < U; ++i){
            dual[i] = 0;
            for(int j = 0; j < V; ++j){
                dual[U + j] = min(dual[U + j], cost[i][j]);
            }
        }
    }
    void construct_graph(){
        for(int i = 0; i < U; ++i){
            for(int j = 0; j < V; ++j){
                graph[i][j] = (diff(i, j) == 0 && rev_alloc[j] != i);
            }
        }
    }
    bool find_augmenting_path(const int cur, const int prv, int& pos){
        prev[cur] = prv;
        if(cur >= U){
            if(rev_alloc[cur - U] < 0) return true;
            if(find_augmenting_path(rev_alloc[cur - U], cur, pos)){
                graph[rev_alloc[cur - U]][cur - U] = 1;
                return true;
            }
        }else{
            const int MX = (alloc[cur] < 0 && pos == U) ? U : V;
            for(int i = 0; i < MX; ++i){
                if(graph[cur][i] && prev[U + i] < 0 && find_augmenting_path(U + i, cur, pos)){
                    graph[cur][i] = 0, alloc[cur] = i, rev_alloc[i] = cur;
                    return true;
                }
            }
            if(alloc[cur] < 0 && pos < U){
                graph[cur][pos] = 0, alloc[cur] = pos, rev_alloc[pos] = cur, prev[U + pos] = cur;
                return ++pos, true;
            }
        }
        return false;
    }
    void update_dual(const T delta){
        for(int i = 0; i < U; ++i) if(prev[i] >= 0) dual[i] += delta;
        for(int i = U; i < U + V; ++i) if(prev[i] >= 0) dual[i] -= delta;
    }
    void maximum_matching(bool initial=false){
        int pos = initial ? V : U;
        for(bool update = false;; update = false){
            fill(prev.begin(), prev.end(), -1);
            for(int i = 0; i < U; ++i){
                if(alloc[i] < 0 && find_augmenting_path(i, 2 * U, pos)){
                    update = true, ++matching_size;
                    break;
                }
            }
            if(!update) break;
        }
    }
    int dfs(const int cur, const int prv, vector<int>& new_ver){
        prev[cur] = prv;
        if(cur >= U){
            if(rev_alloc[cur - U] < 0) return cur;
            else return dfs(rev_alloc[cur - U], cur, new_ver);
        }else{
            new_ver.push_back(cur);
            for(int i = 0; i < V; ++i){
                if(graph[cur][i] && prev[U + i] < 0){
                    const int res = dfs(U + i, cur, new_ver);
                    if(res >= U) return res;
                }
            }
        }
        return -1;
    }
    int increase_matching(const vector<pair<int, int> >& vec, vector<int>& new_ver){
        for(const auto& e : vec){
            if(prev[e.first] < 0){
                const int res = dfs(e.first, e.second, new_ver);
                if(res >= U) return res;
            }
        }
        return -1;
    }
    void hint_increment(int cur){
        while(prev[cur] != 2 * U){
            if(cur >= U){
                graph[prev[cur]][cur - U] = 0, alloc[prev[cur]] = cur - U, rev_alloc[cur - U] = prev[cur];
            }else{
                graph[cur][prev[cur] - U] = 1;
            }
            cur = prev[cur];
        }
    }
public:
    Hungarian(const vector<vector<T> >& _cost)
         : U((int)_cost.size()), V((int)_cost[0].size()), graph(U, vector<int>(U, 1)), dual(U + V, numeric_limits<T>::max()),
            alloc(U, -1), rev_alloc(U, -1), prev(2 * U), cost{_cost}, matching_size(0){
        assert(U >= V);
    }
    pair<T, vector<int> > solve(){
        init_feasible_dual(), construct_graph();
        bool end = false;
        maximum_matching(true);
        while(matching_size < U){
            vector<pair<T, int> > cand(V, {numeric_limits<T>::max(), numeric_limits<int>::max()});
            for(int i = 0; i < U; ++i){
                if(prev[i] < 0) continue;
                for(int j = 0; j < V; ++j){
                    if(prev[U + j] >= 0) continue;
                    cand[j] = min(cand[j], {diff(i, j), i});
                }
            }
            while(true){
                T delta = numeric_limits<T>::max();
                for(int i = 0; i < V; ++i){
                    if(prev[U + i] >= 0) continue;
                    delta = min(delta, cand[i].first);
                }
                update_dual(delta);
                vector<pair<int, int> > vec;
                vector<int> new_ver;
                for(int i = 0; i < V; ++i){
                    if(prev[U + i] >= 0) continue;
                    if((cand[i].first -= delta) == 0) vec.emplace_back(U + i, cand[i].second);
                }
                int res = increase_matching(vec, new_ver);
                if(res >= U){
                    hint_increment(res);
                    if(++matching_size == U) end = true;
                    else construct_graph();
                    break;
                }else{
                    for(const int v : new_ver){
                        for(int i = 0; i < V; ++i){
                            if(prev[U + i] >= 0) continue;
                            cand[i] = min(cand[i], {diff(v, i), v});
                        }
                    }
                }
            }
            if(!end) maximum_matching();
        }
        T total_cost = 0;
        for(int i = 0; i < U; ++i){
            if(alloc[i] < V) total_cost += cost[i][alloc[i]];
            else alloc[i] = -1;
        }
        return make_pair(total_cost, alloc);
    }
};
const int64_t CYCLES_PER_SEC = 3000000000;
struct Timer {
	int64_t start;
	Timer() { reset(); }
	void reset() { start = getCycle(); }
	void plus(double a) { start -= (a * CYCLES_PER_SEC); }
	inline double get() { return (double)(getCycle() - start) / CYCLES_PER_SEC; }
	inline int64_t getCycle() {
		uint32_t low, high;
		__asm__ volatile("rdtsc"
						 : "=a"(low), "=d"(high));
		return ((int64_t)low) | ((int64_t)high << 32);
	}
};



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

struct FastSet {
	vector<char> list;
	vector<char> pos;
	void init(int N) {
		pos.resize(N, -1);
		list.reserve(N);
	}
	void insert_all() {
		list.clear();
		list.resize(pos.size());
		for (int i = 0; i < list.size(); i++) {
			pos[i] = list[i] = i;
		}
	}
	void insert(int a) {
		if (pos[a] == -1) {
			pos[a] = list.size();
			list.push_back(a);
		}
	}
	void erase(int a) {
		if (pos[a] >= 0) {
			swap(list[pos[a]], list.back());
			pos[list[pos[a]]] = pos[a];
			pos[a] = -1;
			list.pop_back();
		}
	}
	void flip(int a) {
		if (pos[a] == -1) {
			pos[a] = list.size();
			list.push_back(a);
		} else {
			swap(list[pos[a]], list.back());
			pos[list[pos[a]]] = pos[a];
			pos[a] = -1;
			list.pop_back();
		}
	}
	inline int size() {
		return list.size();
	}
	inline int rand() {
		return list[rnd.nextInt(list.size())];
	}
	inline void erase_all() {
		for (int i : list) {
			pos[i] = -1;
		}
		list.clear();
	}
};

class UF_pop {
public:
    int sz;
    vector<char> par,nrank; vector<vector<char> > hist;
    UF_pop(){}
    UF_pop(int node_size) : sz(node_size), par(sz), nrank(sz, 0){
        for(int i = 0; i < sz; i++){
            par[i] = i;
        }
    }
    int find(char x){
        return (par[x] == x)?(x):find(par[x]);
    }
    void pop(){
        if(!hist.empty()){
            auto& a = hist.back();
            par[a[0]] = a[0], nrank[a[0]] = a[1], par[a[2]] = a[2];
            hist.pop_back();
        }
    }
    void unite(char x,char y) {
        x = find(x); y = find(y);
        hist.push_back({x,nrank[x],y});
        if(x == y) return;
        if(nrank[x] < nrank[y]){
            par[x] = y;
        }else{
            nrank[x] += (nrank[x] == nrank[y]);
            par[y] = x;
        }
    }
    bool same(char x,char y){
        return find(x) == find(y);
    }
};

int n,T;

int dx[] = {0,-1,0,1};
int dy[] = {-1,0,1,0};
int idir[] = {2,3,0,1};
char chara[] = {'L','U','R','D'};

bool placeflag[100][100];

char MoveID[100][4][11];

char move_id(int id,int dir,int D = n){
    int x = id/D;
    int y = id%D;
    if(x+dx[dir]<0||x+dx[dir]>=D||y+dy[dir]<0||y+dy[dir]>=D)return -1;
    return (x+dx[dir]) * D + y+ dy[dir];
}
char calc_id(int x,int y,int N=n){
    return x*N + y;
}


short table_two_dp[32][101][101][101];
char table_two_pre[32][100][100][100];
short table_three_dp[11][7*7][7*7][7*7][7*7];
char table_three_pre[11][7*7][7*7][7*7][7*7];
bool table_four_dp_second[4][4*4][4*4][4*4][4*4][4*4];
char table_four_pre_second[4][4*4][4*4][4*4][4*4][4*4];
char table_n_N[101];
vector<pair<char,char>> pop_id[101];
vector<vector<char> > PIECES;
vector<char> tile_list;


map<int,pair<int,char> > three_times_three_transiton;

vector<char> clean_up_three_table(vector<char>&b){
    auto calc_field_num = [&](vector<char> &v)->int{
        int z = 1;
        int ss = 0;
        rep(i,9){
            ss += z*v[i];
            z *= 10;
        }
        return ss;
    };
    vector<char> vvvvv = {1,2,3,4,5,6,7,8,0};
    int tar_num = calc_field_num(vvvvv);
    int cur_num = calc_field_num(b);
    vector<char> res;
    while(cur_num!=tar_num){
        auto [num,dir] = three_times_three_transiton[cur_num];
        res.push_back(dir);
        cur_num = num;
    }
    return res;
}

vector<char> move_two_pieces_table(int id,vector<char> &a,int xid,int yid,int sid,int N = n){
    int x[3] = {xid,yid,sid};
    int sx = sid;
    vector<char> res;
    while(table_two_dp[id][x[0]][x[1]][x[2]]!=1){
        char dir = table_two_pre[id][x[0]][x[1]][x[2]];
        int nxt2 = MoveID[x[2]][dir][N];
        rep(z,2){
            if(nxt2==x[z]){
                x[z] = x[2];
            }
        }
        x[2] = nxt2;
        res.push_back(dir);
    }
    for(auto dir:res){
        int nx = MoveID[sx][dir][n];
        swap(a[sx],a[nx]);
        sx = nx;
    }
    return res;
}

vector<char> move_three_pieces_table(int id,vector<char> &a,int xid,int yid,int zid,int sid,int N = n){
    int x[4] = {xid,yid,zid,sid};
    int sx = sid;
    vector<char> res;
    while(table_three_dp[id][x[0]][x[1]][x[2]][x[3]]!=1){
        char dir = table_three_pre[id][x[0]][x[1]][x[2]][x[3]];
        int nxt3 = MoveID[x[3]][dir][N];
        rep(z,3){
            if(nxt3==x[z]){
                x[z] = x[3];
            }
        }
        x[3] = nxt3;
        res.push_back(dir);
    }
    for(auto dir:res){
        int nx = MoveID[sx][dir][N];
        swap(a[sx],a[nx]);
        sx = nx;
    }
    return res;
}

vector<char> create_number(vector<char> &a,vector<char>&b,int N,int M){
    vv(int,g,N*M,N*M);
    int cc = 0;
    vector<char>num(N*M);
    int dd = 1;
    rep(x,N){
        rep(y,M){
            if(b[x*M + y]!=0){
                num[x*M + y] = dd;
                dd++;
            }
        }
    }
    rep(i,N){
        rep(j,M){
            rep(x,N){
                rep(y,M){
                    if(a[i*M+j]==b[x*M+y]){
                        if(a[i*M+j]==0){
                            cc = (abs(i-x) + abs(j-y))%2;
                        }
                        g[i*M+j][x*M+y] = abs(i-x) + abs(j-y);
                    }else{
                        g[i*M+j][x*M+y] = 50000;
                    }
                }
            }
        }
    }
    Hungarian<int> hg(g);// min-cost flow のほうが速い？
    auto [ff,alloc] = hg.solve();
    vector<char>res(N*M);
    rep(i,N){
        rep(j,M){
            if(a[i*M+j]==0){

            }else{
                res[i*M+j] = num[alloc[i*M+j]];
            }
        }
    }
    // 互換 check
    vector<char> flag(N*M);
    int c = 0;
    rep(i,N*M){
        if(flag[i])continue;
        int id = i;
        while(!flag[id]){
            flag[id] = 1;
            c++;
            id = alloc[id];
        }
        c--;
    }
    if(c%2!=cc%2){
        // cerr << "parity!" << endl;
        int ii=-1,jj=-1,xx=-1,yy=-1;
        int mi = inf;
        rep(i,N){
            rep(j,M){        
                rep(x,N){
                    rep(y,M){
                        if(i==x&&j==y)continue;
                        if(a[i*M+j]==a[x*M+y]){
                            if(chmin(mi,abs(i-x)+abs(j-y))){
                                ii = i;
                                jj = j;
                                xx = x;
                                yy = y;
                            }
                        }
                    }
                }
            }
        }
        if(ii==-1){
            return {};
        }
        swap(res[ii*M + jj],res[xx* M + yy]);
    }
    return res;
}

vector<vector<char> > make_pieces(int N = n){
    vector<vector<char> > pieces;
    if(N%2==0){
        if(N==6){
            return {{5,4,3},{2,1,0},{6,7,8},{9,10,11},{17,16,15},{14,13,12},{18,24,30},{19,25,31},{20,26,32}};
        }else{
            for(int i=0;i<N-6;i+=2){
                for(int j=i;j<N;j+=2){
                    pieces.push_back({calc_id(i,j,N),calc_id(i,j+1,N)});
                }
                if(i!=n-2){
                    for(int j=N-1;j>=i+1;j-=2){
                        pieces.push_back({calc_id(i+1,j,N),calc_id(i+1,j-1,N)});
                    }
                }
                for(int j=i+2;j<N;j+=2){
                    pieces.push_back({calc_id(j,i,N),calc_id(j+1,i,N)});
                }
                for(int j=N-1;j>=i+3;j-=2){
                    pieces.push_back({calc_id(j,i+1,N),calc_id(j-1,i+1,N)});
                }
            }
        }
    }else{
        if(N==7){
            return {{6,13,20},{5,4,3},{2,1,0},{7,8,9},{10,11,12},{19,18,17},{16,15,14},{21,22,23},{28,35,42},{29,36,43},{30,37,44}};
        }else{
            pieces.push_back({calc_id(0,0,N),calc_id(1,0,N)});
            for(int j=1;j<N;j+=2){
                pieces.push_back({calc_id(0,j,N),calc_id(0,j+1,N)});
            }
            for(int j=N-1;j>=2;j-=2){
                pieces.push_back({calc_id(1,j,N),calc_id(1,j-1,N)});
            }
            pieces.push_back({calc_id(2,0,N),calc_id(2,1,N)});
            for(int i=3;i<N;i+=2){
                pieces.push_back({calc_id(i,0),calc_id(i+1,0)});
            }
            for(int i=N-1;i>=4;i-=2){
                pieces.push_back({calc_id(i,1),calc_id(i-1,1)});
            }
        }
    }
    return pieces;
}
void make_tables(){
    rep(i,n*n){
        rep(dir,4){
            for(int z=4;z<=n;z++)
            MoveID[i][dir][z] = move_id(i,dir,z);
        }
    }
    {// three 用 table
        int N = 6 + n%2;
        // auto pieces = make_pieces(N);
        vector<vector<char> > pieces;
        if(N==6){
            pieces={{5,4,3},{2,1,0},{6,7,8},{9,10,11},{17,16,15},{14,13,12},{18,24,30},{19,25,31},{20,26,32}};
        
        }else{
            pieces= {{6,13,20},{5,4,3},{2,1,0},{7,8,9},{10,11,12},{19,18,17},{16,15,14},{21,22,23},{28,35,42},{29,36,43},{30,37,44}};
        }
        vec(char,flag,N*N);
        rep(id,pieces.size()){
            int tar[3] = {pieces[id][0],pieces[id][1],pieces[id][2]};
            vector<int> neighbor;
            rep(z,3){
                rep(dir,4){
                    int xx =  MoveID[tar[z]][dir][N];
                    if(xx==-1)continue;
                    if(xx==tar[0]||xx==tar[1]||xx==tar[2])continue;
                    if(flag[xx])continue;
                    neighbor.push_back(xx);
                }
            }
            queue<tuple<int,int,int,int> > q;
            // fill(table_three_dp,inf);
            for(auto xx:neighbor){
                table_three_dp[id][tar[0]][tar[1]][tar[2]][xx] = 1;
                q.push(MT(tar[0],tar[1],tar[2],xx));
            }
            while(!q.empty()){
                auto [d,f,h,g] = q.front();
                int cost = table_three_dp[id][d][f][h][g]; 
                q.pop();
                rep(dir,4){
                    int nxtg = MoveID[g][dir][N];
                    if(nxtg==-1)continue;
                    if(flag[nxtg])continue;
                    int nxtd = d;
                    int nxtf = f;
                    int nxth = h;
                    if(nxtg==d){
                        nxtd = g;
                    }
                    if(nxtg==f){
                        nxtf = g;
                    }
                    if(nxtg==h){
                        nxth = g;
                    }
                    if(!table_three_dp[id][nxtd][nxtf][nxth][nxtg]){
                        table_three_dp[id][nxtd][nxtf][nxth][nxtg] = cost + 1;
                        table_three_pre[id][nxtd][nxtf][nxth][nxtg] = idir[dir];
                        q.push(MT(nxtd,nxtf,nxth,nxtg));
                    }
                }
            }
            flag[tar[0]] = 1;
            flag[tar[1]] = 1;
            flag[tar[2]] = 1;
        }
    }
    if(n>=8){// two 用 table
        auto pieces = make_pieces();
        vec(char,flag,n*n);
        rep(id,pieces.size()){
            int tar[2] = {pieces[id][0],pieces[id][1]};
            vector<int> neighbor;
            rep(z,2){
                rep(dir,4){
                    int xx = MoveID[tar[z]][dir][n];
                    if(xx==-1)continue;
                    if(xx==tar[0]||xx==tar[1])continue;
                    if(flag[xx])continue;
                    neighbor.push_back(xx);
                }
            }
            queue<tuple<int,int,int> > q;
            for(auto xx:neighbor){
                table_two_dp[id][tar[0]][tar[1]][xx] = 1;
                q.push(MT(tar[0],tar[1],xx));
            }
            while(!q.empty()){
                auto [d,f,g] = q.front();
                int cost = table_two_dp[id][d][f][g];
                q.pop();
                rep(dir,4){
                    int nxtg = MoveID[g][dir][n];
                    if(nxtg==-1)continue;
                    if(flag[nxtg])continue;
                    int nxtd = d;
                    int nxtf = f;
                    if(nxtg==d){
                        nxtd = g;
                    }
                    if(nxtg==f){
                        nxtf = g;
                    }
                    if(!table_two_dp[id][nxtd][nxtf][nxtg]){
                        table_two_dp[id][nxtd][nxtf][nxtg] = cost+1;
                        table_two_pre[id][nxtd][nxtf][nxtg] = idir[dir];
                        q.push(MT(nxtd,nxtf,nxtg));
                    }
                }
            }
            flag[tar[0]] = 1;
            flag[tar[1]] = 1;
        }
    }
    if(n%2==1){
        {
            vector<vector<int> > pieces={{0,1,2,3},{4,5,6,7},{8,9,12,13},{10,11,14,0}};
            vec(char,flag,4*4);
            for(int i=0;i<4;i++){
                int tar[4]={};
                rep(z,4){
                    tar[z] = pieces[i][z];
                }
                vector<int> neighbor;
                rep(z,4){
                    if(i==3&&z==3)continue;
                    rep(dir,4){
                        int xx = MoveID[tar[z]][dir][4];
                        if(xx==-1)continue;
                        if(xx==tar[0]||xx==tar[1]||xx==tar[2]||xx==tar[3])continue;
                        if(flag[xx])continue;
                        neighbor.push_back(xx);
                    }
                }
                queue<tuple<int,int,int,int,int> > q;
                for(auto xx:neighbor){
                    table_four_dp_second[i][tar[0]][tar[1]][tar[2]][tar[3]][xx] = true;
                    q.push(MT(tar[0],tar[1],tar[2],tar[3],xx));
                }
                while(!q.empty()){
                    auto [d,f,h,j,g] = q.front();
                    q.pop();
                    rep(dir,4){
                        int nxtg = MoveID[g][dir][4];
                        if(nxtg==-1)continue;
                        if(flag[nxtg])continue;
                        int nxtd = d;
                        int nxtf = f;
                        int nxth = h;
                        int nxtj = j;
                        if(nxtg==d){
                            nxtd = g;
                        }
                        if(nxtg==f){
                            nxtf = g;
                        }
                        if(nxtg==h){
                            nxth = g;
                        }
                        if(nxtg==j){
                            nxtj = g;
                        }
                        if(!table_four_dp_second[i][nxtd][nxtf][nxth][nxtj][nxtg]){
                            table_four_dp_second[i][nxtd][nxtf][nxth][nxtj][nxtg] = true;
                            table_four_pre_second[i][nxtd][nxtf][nxth][nxtj][nxtg] = idir[dir];
                            q.push(MT(nxtd,nxtf,nxth,nxtj,nxtg));
                        }
                    }
                }
                rep(z,4){
                    flag[tar[z]] = true;
                }
            }
        }
    }
    if(n%2==0){
        auto calc_field_num = [&](vector<int> &v)->int{
            int z = 1;
            int ss = 0;
            rep(i,9){
                ss += z*v[i];
                z *= 10;
            }
            return ss;
        };
        set<int> st;
        map<int,int> pre_dir;
        map<int,int> pre;
        queue<vector<int>> q;
        vector<int> vvvvv = {1,2,3,4,5,6,7,8,0};
        st.insert(calc_field_num(vvvvv));
        q.push(vvvvv);
        while(!q.empty()){
            auto v = q.front();
            q.pop();
            int xid;
            int v_num = calc_field_num(v);
            rep(i,9){
                if(v[i]==0){
                    xid = i;
                    break;
                }
            }
            int x = xid/3;
            int y = xid%3;
            rep(dir,4){
                if(x+dx[dir]<0||x+dx[dir]>=3||y+dy[dir]<0||y+dy[dir]>=3)continue;
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                auto A = v;
                swap(A[nx*3+ny],A[x*3+y]);
                int num = calc_field_num(A);
                if(st.count(num)==0){
                    q.push(A);
                    three_times_three_transiton[num] = MP(v_num,idir[dir]);
                    st.insert(num);
                }
            }
        }
    }    
    PIECES = make_pieces(n);
    if(n>=8&&n%2==1){
        auto ppp = make_pieces(7);
        for(auto&x:ppp){
            for(auto& y:x){    
                y += (n-7)*n + (n-7)*(y/7 + 1);
            }
            PIECES.push_back(x);
        }
        rep(i,7*7){
            table_n_N[i + (n-7)*n + (n-7)*(i/7 + 1)] = i;
        }
    }else if(n>=8&&n%2==0){
        auto ppp = make_pieces(6);
        for(auto&x:ppp){
            for(auto& y:x){
                int py = y;
                y += (n-6)*n + (n-6)*(y/6 + 1);
                table_n_N[y] = py;
            }
            PIECES.push_back(x);
        }
        
        rep(i,6*6){
            table_n_N[i + (n-6)*n + (n-6)*(i/6 + 1)] = i;
        }
        
    }
    if(n%2==0){
        srep(i,n-3,n){
            srep(j,n-3,n){
                int id = i*n + j;
                rep(x,n){
                    rep(y,n){
                        if(x==i){
                            if(y==j)break;
                        }else if(x>=i+1){
                            if(y==n-3)break;
                        }
                        placeflag[id][x*n+y] = true;
                    }
                }
            }
        }   
    }else{
        srep(i,n-4,n){
            srep(j,n-4,n){
                int id = i*n + j;
                rep(x,n){
                    rep(y,n){
                        if(x==i){
                            if(y==j)break;
                        }else if(x>=i+1){
                            if(y==n-4)break;
                        }
                        placeflag[id][x*n+y] = true;
                    }
                }
            }
        }
    }
    for(int j=0;j<PIECES.size();j++){
        for(auto x:PIECES[j]){
            for(int k=j+1;k<PIECES.size();k++){
                for(auto y:PIECES[k]){
                    placeflag[y][x] = true;
                }
            }
            for(auto y:PIECES[j]){
                if(x==y)break;
                placeflag[x][y] = true;
            }
        }
    }
    rep(i,n*n){
        rep(dir,4){
            int nxt = MoveID[i][dir][n];
            if(nxt==-1)continue;
            if(placeflag[i][nxt]){
                pop_id[i].push_back({nxt,dir});
            }
        }
    }
}

void show_field(vector<char>& field){
    rep(i,n){
        rep(j,n){
            if(field[i*n+j]<10){
                cout << (int)field[i*n+j];
            }else{
                cout << (char)('a' + field[i*n+j]-10);
            }
        }
        cout << "\n";
    }
}

double time_check[4];
struct state{
    char pieceid;
    short penalty;
    char cnt[16];
    vector<char> ans;
    vector<char> cur;
    map<char,char> frontier_info;
    state(){}
    void init(vector<char>&a){
        cur = a;
        rep(i,16)cnt[i] = 0;
        rep(i,n*n){
            if(a[i]!=0)cnt[a[i]]++;
        }
        pieceid = 0;
        penalty = 0;
    }
    // bool penalty_calc(const int i,const int j,const int dir){
    //     if(j==0&&dir==0) return (target[i*n + j]>>dir)&1;
    //     if(j==n-1&&dir==2) return (target[i*n + j]>>dir)&1;
    //     if(i==0&&dir==1) return (target[i*n + j]>>dir)&1;
    //     if(i==n-1&&dir==3) return (target[i*n + j]>>dir)&1;
    //     if((target[i*n + j]>>dir)&1){
    //         if((target[(i + dx[dir])*n + j + dy[dir]]>>idir[dir])&1){
    //             return 0;
    //         }else{
    //             return 1;
    //         }
    //     }else{
    //         return (target[(i + dx[dir])*n + j + dy[dir]]>>idir[dir])&1;
    //     }
    // }
    bool is_open(const int id,const int x,const int y,const int dir){
        int P = MoveID[x*n+y][dir][n];
        if(P!=-1){
            return !placeflag[id][P];
        }else{
            return false;
        }
    }
    bool put_pieces_randomly(int D = 4){
        int m = PIECES[pieceid].size();
        vector<int> putpieces;
        auto piece_type = [&](int id,int dir)->char{
            int nxt = MoveID[id][dir][n];
            if(nxt==-1 || !placeflag[id][nxt])return -1;
            if(pieceid==0){
                return putpieces.back();          
            }else{
                if(!placeflag[PIECES[pieceid][0]][nxt]){
                    return putpieces.back();   
                }else{
                    return cur[nxt];
                }
            }
        };
        auto dfs = [&](auto&&f,int M)->bool{
            if(M==m)return true;
            int id = PIECES[pieceid][M];
            int x = id/n;
            int y = id%n;
            vector<pair<char,char> >pop_element;
            auto pre_info = frontier_info;
            // double SSS = GetSeconds();
            {// TODO target -> cur
                for(auto [nxt,dir]:pop_id[id]){
                    bool ff = 0;
                    if(!placeflag[id][nxt])continue;
                    if(frontier_info.count(nxt)==0)continue;
                    int checkdir = (dir+1)%4;
                    
                    if((piece_type(id,dir)>>checkdir)&1){
                        int P = MoveID[nxt][checkdir][n];
                        if(P!=-1){
                            if(!placeflag[id][P])ff =1;
                        }
                    }
                     
                    if(ff){
                        continue;
                    }
                    checkdir = (dir+3)%4;
                    if((piece_type(id,dir)>>checkdir)&1){
                        int P = MoveID[nxt][checkdir][n];
                        if(P!=-1){
                            if(!placeflag[id][P])ff =1;
                        }
                    }
                    if(!ff){
                        pop_element.push_back({nxt,frontier_info[nxt]});
                    }
                }
            }
            // time_check[2] += GetSeconds()-SSS;
            int need_bits = 0;
            vector<int> p;
            rep(dir,4){
                char nxt = MoveID[id][dir][n];
                if(nxt==-1){
                    continue;
                }
                if(!placeflag[id][nxt]){
                    p.push_back(dir);
                    continue;
                }
                if((piece_type(id,dir)>>idir[dir])&1){
                    need_bits ^= (1<<dir);
                }
            }
            if(p.size()>2||p.size()==0)assert(0);
            vector<int> candidate;
            int S = 0;
            if(p.size()==1){
                if(cnt[need_bits])candidate.push_back(need_bits);
                if(cnt[need_bits^(1<<p[0])])candidate.push_back(need_bits^(1<<p[0]));
            }else{
                if(cnt[need_bits])candidate.push_back(need_bits);
                if(cnt[need_bits^(1<<p[0])])candidate.push_back(need_bits^(1<<p[0]));
                if(cnt[need_bits^(1<<p[1])])candidate.push_back(need_bits^(1<<p[1]));
                if(cnt[need_bits^(1<<p[0])^(1<<p[1])])candidate.push_back(need_bits^(1<<p[0])^(1<<p[1]));
            }
            if(id!=n*n-1 && candidate.empty())return false;
            for(auto x:candidate){
                S += cnt[x];
            }
            if(id==n*n-1) D = 1;
            chmin(D,(int)candidate.size());
            vector<char>used(candidate.size());
            rep(i,D){
                int z;
                int piece;
                if(id==n*n-1)piece = 0;
                else{
                    z = rnd.nextInt(S);
                    rep(j,candidate.size()){
                        if(used[j])continue;
                        if(z < cnt[candidate[j]]){
                            used[j] = 1;
                            piece = candidate[j];
                            S -= cnt[candidate[j]];
                            break;
                        }
                        z -= cnt[candidate[j]];
                    }
                }
                putpieces.push_back(piece);
                bool ng = 0;
                if(y==0&&((piece>>0)&1))ng = true;
                if(x==0&&((piece>>1)&1))ng = true;
                if(y==n-1&&((piece>>2)&1))ng = true;
                if(x==n-1&&((piece>>3)&1))ng = true;
                if(ng){
                    putpieces.pop_back();
                    continue;
                }
                int now_mate = id;
                map<char,char> change_mate;
                rep(dir,4){
                    if((piece>>dir)&1){
                        int nxt = MoveID[id][dir][n];
                        if(placeflag[id][nxt]){
                            if(now_mate == id){
                                now_mate = frontier_info[nxt];
                            }else{
                                if(now_mate==frontier_info[nxt]){
                                    ng = true;
                                    break;    
                                }
                                change_mate[frontier_info[nxt]] = now_mate ;
                            }
                        }
                    }
                }
                if(ng){
                    putpieces.pop_back();
                    frontier_info = pre_info;
                    continue;
                }
                for(auto& [x,y]:pop_element){
                    frontier_info.erase(x);
                }
                for(auto& [x,y]:frontier_info){
                    if(change_mate.count(y)){
                        y = change_mate[y];
                    }
                }
                bool openflag = false;
                rep(dir,4){
                    if(((piece>>dir)&1)&&is_open(id,x,y,dir))openflag = true;
                }
                if(openflag){

                }else{
                    // check 
                    bool okflag = false;
                    for(auto& [x,y]:frontier_info){
                        if(now_mate == y){
                            okflag = true;
                            break;
                        }
                    }
                    if(!okflag){
                        // cerr << "NG" << endl;
                        putpieces.pop_back();
                        frontier_info = pre_info;
                        continue;
                    }
                }
                if(openflag){
                    // cerr << "open: " << now_mate << endl;
                    frontier_info[id] = now_mate;
                }
                // pop_element のチェック
                set<char> frontier;
                for(auto [x,y]:frontier_info){
                    frontier.insert(y);
                }
                
                for(auto [x,y]:pop_element){
                    if(change_mate.count(y)){
                        y = change_mate[y];
                    }
                    if(frontier.count(y) == 0){
                        // cerr << "NG! : " << (int)x << " " << (int)y << " " << now_mate << endl;
                        ng = 1;
                        break;
                    }
                }
                if(ng){

                    putpieces.pop_back();
                    frontier_info = pre_info;
                    continue;
                }
                if(id!=n*n-1){
                    cnt[piece]--;
                }
                if(f(f,M+1))return true;
                if(id!=n*n-1){
                    cnt[piece]++;
                }
                putpieces.pop_back();
                frontier_info = pre_info;
            }
            return false;
        };
        // double AA = GetSeconds();
        bool ok = dfs(dfs,0);
        // time_check[0] += GetSeconds() - AA;
        
        if(ok){
            // AA = GetSeconds();
            if(m==2){
                auto &v = PIECES[pieceid];
                int sid = 0;
                int tar[2] = {putpieces[0],putpieces[1]};
                vector<int> cand[2];
                rep(i,n*n){
                    if(cur[i]==0){
                        sid = i;
                        break;
                    }
                };
                rep(z,2){
                    rep(i,n*n){
                        if(!placeflag[PIECES[pieceid][0]][i]&&cur[i]==tar[z]){
                            cand[z].push_back(i);
                        }        
                    }
                }
                short mi = 30000;
                int X,Y;
                for(auto x:cand[0]){
                    for(auto y:cand[1]){
                        if(x==y)continue;
                        if(chmin(mi,table_two_dp[pieceid][x][y][sid])){
                            X = x;
                            Y = y;
                        }
                    }
                }
                auto tmp = move_two_pieces_table(pieceid,cur,X,Y,sid);
                for(auto x:tmp)ans.push_back(x);
                // flag[v[0]] = 1;
                // flag[v[1]] = 1;
            }else{
                auto &v = PIECES[pieceid];
                int sid = 0;
                int tar[3] = {putpieces[0],putpieces[1],putpieces[2]};
                
                vector<int> cand[3];
                rep(i,n*n){
                    if(cur[i]==0){
                        sid = i;
                        break;
                    }
                };
                rep(z,3){
                    rep(i,n*n){
                        if(!placeflag[PIECES[pieceid][0]][i]&&cur[i]==tar[z]){
                            cand[z].push_back(i);
                        }        
                    }
                }
                short mi = 30000;
                // n >= 8 以上の時編集が必要！！！
                if(n>=8){
                    int X=-1,Y=-1,Z=-1;
                    int N;
                    if(n%2==0){
                        N = 6;
                    }else{
                        N = 7;
                    }
                    int idDD = 0;
                    for(int i=0;i<PIECES.size();i++){
                        if(PIECES[i].size()==3){
                            idDD = i;
                            break;
                        }
                    }
                    int ss = table_n_N[sid];
                    
                    for(auto x:cand[0]){
                        int xx = table_n_N[x];
                        for(auto y:cand[1]){
                            int yy = table_n_N[y];
                            for(auto z:cand[2]){
                                if(x==y||y==z||z==x)continue;
                                int zz = table_n_N[z];
                                if(chmin(mi,table_three_dp[pieceid-idDD][xx][yy][zz][ss])){
                                    // cerr << pieceid-idDD <<" "  << xx << " " << yy << " " <<zz << " " << ss << " " << mi << endl;
                                    X = xx;
                                    Y = yy;
                                    Z = zz;
                                }
                            }
                        }
                    }
                    // cerr << mi << endl;
                    int x[4] = {X,Y,Z,ss};
                    int sx = sid;
                    vector<char> res;
                    while(table_three_dp[pieceid-idDD][x[0]][x[1]][x[2]][x[3]]!=1){
                        int dir = table_three_pre[pieceid-idDD][x[0]][x[1]][x[2]][x[3]];
                        int nxt3 = MoveID[x[3]][dir][N];
                        rep(z,3){
                            if(nxt3==x[z]){
                                x[z] = x[3];
                            }
                        }
                        x[3] = nxt3;
                        res.push_back(dir);
                    }
                    for(auto dir:res){
                        int nx = MoveID[sx][dir][n];
                        swap(cur[sx],cur[nx]);
                        sx = nx;
                        ans.push_back(dir);
                    }
                    // flag[v[0]] = 1;
                    // flag[v[1]] = 1;
                    // flag[v[2]] = 1;
                }else{
                    int X=-1,Y=-1,Z=-1;
                    for(auto x:cand[0]){
                        for(auto y:cand[1]){
                            for(auto z:cand[2]){
                                if(x==y||y==z||z==x)continue;
                                if(chmin(mi,table_three_dp[pieceid][x][y][z][sid])){
                                    X = x;
                                    Y = y;
                                    Z = z;
                                }
                            }
                        }
                    }
                    auto tmp = move_three_pieces_table(pieceid,cur,X,Y,Z,sid);
                    for(auto x:tmp)ans.push_back(x);
                    // flag[v[0]] = 1;
                    // flag[v[1]] = 1;
                    // flag[v[2]] = 1;
                }
            }
            // time_check[1] += GetSeconds() - AA;
            pieceid++;
            penalty = ans.size();
            return true;
        }else{
            return false;
        }
    }
    vector<char> make_feasible_solution(int D = 4){
        int firstID;
        int mm = 3;
        if(n%2==0){
            firstID = (n-3) * n + n-3;
        }else{
            firstID = (n-4) * n + n-4;    
            mm = 4;
        }
        vector<char> putpieces;
        auto piece_type = [&](int id,int dir)->char{
            int nxt = MoveID[id][dir][n];
            if(nxt==-1 || !placeflag[id][nxt])return -1;
            if(!placeflag[firstID][nxt]){
                if(dir==0){
                    return putpieces.back();
                }else if(dir==1){
                    return putpieces[putpieces.size()-mm];
                }
            }else{
                return cur[nxt];
            }
        };
        auto dfs = [&](auto&&f,int id)->bool{
            if(id==n*n)return true;
            if(placeflag[firstID][id])return f(f,id+1);
            if(id==n*n-1){
                if((putpieces.back()>>2)&1)return false;
                if((putpieces[putpieces.size()-mm]>>3)&1)return false;
                putpieces.push_back(0);
                return true;
            }
            int x = id/n;
            int y = id%n;
            vector<pair<char,char> >pop_element;
            auto pre_info = frontier_info;
            // double SSS = GetSeconds();
            {// TODO target -> cur
                for(auto [nxt,dir]:pop_id[id]){
                    bool ff = 0;
                    if(!placeflag[id][nxt])continue;
                    if(frontier_info.count(nxt)==0)continue;
                    int checkdir = (dir+1)%4;
                    
                    if((piece_type(id,dir)>>checkdir)&1){
                        int P = MoveID[nxt][checkdir][n];
                        if(P!=-1){
                            if(!placeflag[id][P])ff =1;
                        }
                    }
                     
                    if(ff){
                        continue;
                    }
                    checkdir = (dir+3)%4;
                    if((piece_type(id,dir)>>checkdir)&1){
                        int P = MoveID[nxt][checkdir][n];
                        if(P!=-1){
                            if(!placeflag[id][P])ff =1;
                        }
                    }
                    if(!ff){
                        pop_element.push_back({nxt,frontier_info[nxt]});
                    }
                }
            }
            // time_check[2] += GetSeconds()-SSS;
            int need_bits = 0;
            vector<int> p;
            rep(dir,4){
                char nxt = MoveID[id][dir][n];
                if(nxt==-1){
                    continue;
                }
                if(!placeflag[id][nxt]){
                    p.push_back(dir);
                    continue;
                }
                if((piece_type(id,dir)>>idir[dir])&1){
                    need_bits ^= (1<<dir);
                }
            }
            if(p.size()>2||p.size()==0)assert(0);
            vector<int> candidate;
            int S = 0;
            if(p.size()==1){
                if(cnt[need_bits])candidate.push_back(need_bits);
                if(cnt[need_bits^(1<<p[0])])candidate.push_back(need_bits^(1<<p[0]));
            }else{
                if(cnt[need_bits])candidate.push_back(need_bits);
                if(cnt[need_bits^(1<<p[0])])candidate.push_back(need_bits^(1<<p[0]));
                if(cnt[need_bits^(1<<p[1])])candidate.push_back(need_bits^(1<<p[1]));
                if(cnt[need_bits^(1<<p[0])^(1<<p[1])])candidate.push_back(need_bits^(1<<p[0])^(1<<p[1]));
            }
            if(id!=n*n-1 && candidate.empty())return false;
            for(auto x:candidate){
                S += cnt[x];
            }
            if(id==n*n-1) D = 1;
            chmin(D,(int)candidate.size());
            vector<char>used(candidate.size());
            rep(i,D){
                int z;
                int piece;
                if(id==n*n-1)piece = 0;
                else{
                    z = rnd.nextInt(S);
                    rep(j,candidate.size()){
                        if(used[j])continue;
                        if(z < cnt[candidate[j]]){
                            used[j] = 1;
                            piece = candidate[j];
                            S -= cnt[candidate[j]];
                            break;
                        }
                        z -= cnt[candidate[j]];
                    }
                }
                putpieces.push_back(piece);
                bool ng = 0;
                if(y==0&&((piece>>0)&1))ng = true;
                if(x==0&&((piece>>1)&1))ng = true;
                if(y==n-1&&((piece>>2)&1))ng = true;
                if(x==n-1&&((piece>>3)&1))ng = true;
                if(ng){
                    putpieces.pop_back();
                    continue;
                }
                int now_mate = id;
                map<int,int> change_mate;
                rep(dir,4){
                    if((piece>>dir)&1){
                        int nxt = MoveID[id][dir][n];
                        if(placeflag[id][nxt]){
                            if(now_mate == id){
                                now_mate = frontier_info[nxt];
                            }else{
                                if(now_mate==frontier_info[nxt]){
                                    ng = true;
                                    break;    
                                }
                                change_mate[frontier_info[nxt]] = now_mate ;
                            }
                        }
                    }
                }
                if(ng){
                    putpieces.pop_back();
                    frontier_info = pre_info;
                    continue;
                }
                for(auto& [x,y]:pop_element){
                    frontier_info.erase(x);
                }
                for(auto& [x,y]:frontier_info){
                    if(change_mate.count(y)){
                        y = change_mate[y];
                    }
                }
                bool openflag = false;
                rep(dir,4){
                    if(((piece>>dir)&1)&&is_open(id,x,y,dir))openflag = true;
                }
                if(openflag){

                }else if(id!=n*n-2){
                    // check 
                    bool okflag = false;
                    for(auto& [x,y]:frontier_info){
                        if(now_mate == y){
                            okflag = true;
                            break;
                        }
                    }
                    if(!okflag){
                        // cerr << "NG" << endl;
                        putpieces.pop_back();
                        frontier_info = pre_info;
                        continue;
                    }
                }
                if(openflag){
                    // cerr << "open: " << now_mate << endl;
                    frontier_info[id] = now_mate;
                }
                // pop_element のチェック
                if(id!=n*n-2){
                    set<char> frontier;
                    for(auto [x,y]:frontier_info){
                        frontier.insert(y);
                    }
                    
                    for(auto [x,y]:pop_element){
                        if(change_mate.count(y)){
                            y = change_mate[y];
                        }
                        if(frontier.count(y) == 0){
                            ng = 1;
                            break;
                        }
                    }
                    if(ng){
                        putpieces.pop_back();
                        frontier_info = pre_info;
                        continue;
                    }
                }
                if(ng){

                    putpieces.pop_back();
                    frontier_info = pre_info;
                    continue;
                }
                cnt[piece]--;
                if(f(f,id+1))return true;
                cnt[piece]++;
                putpieces.pop_back();
                frontier_info = pre_info;
            }
            return false;
        };    
        // show_field(cur);
        
        dfs(dfs,firstID);
        // cerr << putpieces << endl;
        return putpieces;
    }
    bool three_answer(vector<char>&B){
        vector<char> A;
        for(int i=n-3;i<n;i++){
            for(int j=n-3;j<n;j++){
                A.push_back(cur[i*n+j]);
            }
        }
        auto cc = create_number(A,B,3,3);
        if(cc.empty())return false;
        auto tmp = clean_up_three_table(cc);
        for(auto dir:tmp)ans.push_back(dir);
        penalty = ans.size();
        
        return true;
    }
    bool four_answer(vector<char>&B){
        vector<char> A;
        for(int i=n-4;i<n;i++){
            for(int j=n-4;j<n;j++){
                A.push_back(cur[i*n+j]);
            }
        }
        auto cc = create_number(A,B,4,4);
        if(cc.empty())return false;
        
        {
            vector<vector<int> > pieces={{0,1,2,3},{4,5,6,7},{8,9,12,13},{10,11,14,0}};
            rep(i,pieces.size()){
                int X[5]={};
                rep(z,4)X[z] = pieces[i][z] + 1;
                int x[5]={};
                rep(j,4*4){
                    rep(z,5){
                        if(cc[j]==X[z])x[z] = j;
                    }
                }
                if(i==3){
                    x[3] = 0;
                }
                vector<int> tmp;
                int S = x[4];
                while(x[0]!=X[0]-1||x[1]!=X[1]-1||x[2]!=X[2]-1||(x[3]!=X[3]-1&&i!=3)){
                    int dir = table_four_pre_second[i][x[0]][x[1]][x[2]][x[3]][x[4]];
                    int nxt4 = MoveID[x[4]][dir][4];
                    rep(z,4){
                        if(nxt4==x[z]){
                            x[z] = x[4];
                        }
                    }
                    x[4] = nxt4;
                    tmp.push_back(dir);
                }
                for(auto dir:tmp){
                    ans.push_back(dir);
                    int T = MoveID[S][dir][4];
                    swap(cc[S],cc[T]);
                    S = T;
                }
            }
        }
        penalty = ans.size();
        
        return true;
    }
    bool operator<(const state& rhs) const{
        return penalty < rhs.penalty;
    }
    bool operator>(const state& rhs) const{
        return penalty > rhs.penalty;
    }
    void debug_(){
        ans.clear();
    }
};
double time_sum[10];

// priority_queue<state,vector<state>,greater<state>>beam[42];
#define STEPLIMIT 500
const int beam_max = 500;
short min_cost[42];
// int sz[42][STEPLIMIT];
// int id[42][STEPLIMIT];
// state beam[42][STEPLIMIT][beam_max];
vector<state> beam[42][STEPLIMIT];
// 42
constexpr double TIME_LIMIT = 2.8;
int main(){
    cin.tie(0);
    ios::sync_with_stdio(false);
    starttime = GetSeconds();

    // Timer tm;
    cin >> n >> T;
    // if(n>=8)return 0;
    VEC(string,s,n);
    vector<char> pp;
    rep(i,n){
        rep(j,n){
            int c = 0;
            if('0'<=s[i][j]&&s[i][j]<='9'){
                c = s[i][j] -'0';
            }else{
                c = 10 + s[i][j] - 'a';
            }
            pp.push_back(c);
            if(c!=0)tile_list.push_back(c);
        }
    }
    make_tables();
    // time_sum[4] = GetSeconds() - starttime ;
    // cerr << tm.get() << endl;
    sort(all(tile_list));
    // auto res = beam_search2(pp);
    // cerr << GetSeconds() - starttime << endl;
    // vector<priority_queue<state,vector<state>,greater<state>>> beam(PIECES.size());
    // vector<map<vector<char>,int > > field_map(PIECES.size());
    state initialstate;
    initialstate.init(pp);
    int SEARCH_DEPTH = PIECES.size();
    state best_state;
    int best_penalty = inf;
    bool feasiblefind = false;
    int TRANSITION_COUNT = 10;
    if(n==6)TRANSITION_COUNT = 6;
    // if(n<=8)beam_max = 3000;
    // if(n==6){
    //     TRANSITION_COUNT = 6;
    // }
    // rep(i,n*n){
    //     cerr << i << endl;
    //     rep(x,n){
    //         rep(y,n){
    //             cerr << placeflag[i][x*n+y];
    //         }
    //         cerr << endl;
    //     }
    // }
    fill(min_cost,30000);
    int counter = 0;
    // cerr << GetSeconds() - starttime << endl;
    while(GetSeconds() - starttime<=TIME_LIMIT){
        counter++;
        for(int round = 0;round < SEARCH_DEPTH;round++){
            state cur_state;
            // double A = GetSeconds();
            // double B = 0;
            if(round==0){
                // double AA = GetSeconds();
                cur_state = initialstate;   
                // time_sum[5] +=  GetSeconds() - AA;
            }else{
                bool EXIST = 0;
                for(int j=min_cost[round];j<STEPLIMIT;j++){
                    // if(id[round][j]!=sz[round][j]){
                    //     double AA = GetSeconds();
                    //     cur_state = beam[round];        
                    //     B = GetSeconds()-AA;
                    //     time_sum[5] +=  B;
                    //     // id[round][j]++;
                    //     // beam[round][j].pop_back();
                    //     EXIST = true;
                    //     break;
                    // }
                    if(!beam[round][j].empty()){
                        // double AA = GetSeconds();
                        cur_state = beam[round][j].back();        
                        // B = GetSeconds()-AA;
                        // time_sum[5] +=  B;
                        beam[round][j].pop_back();
                        EXIST = true;
                        break;
                    }
                }
                if(!EXIST)continue;
            }
            // time_sum[6] += GetSeconds() -A - B;
            // cerr << round << "/" << PIECES.size() << " " << cur_state.penalty << endl;
            // if(round==SEARCH_DEPTH-1){
            //     for(auto x:cur_state.ans){
            //         cerr << chara[x];
            //     }
            //     cerr << endl;
            //     for(auto [x,y]:cur_state.frontier_info){
            //         cerr << (int) x << " : " << (int) y << endl;
            //     }
            // }
            rep(i,TRANSITION_COUNT){
                if(GetSeconds() - starttime >TIME_LIMIT)break;
                // double AA = GetSeconds();
                auto nxt_state = cur_state;
                // time_sum[5] +=  GetSeconds() - AA;
                // AA = GetSeconds();
                bool putflag = nxt_state.put_pieces_randomly();
                // nxt_state.debug_();
                // time_sum[0] +=GetSeconds() - AA;
                // set<vector<char> > st;
                if(putflag){
                    if(round!=SEARCH_DEPTH-1){
                        if(nxt_state.penalty >= STEPLIMIT)continue;
                        if(best_penalty < nxt_state.penalty)continue;
                        if(!beam[round+1][nxt_state.penalty].empty()&&beam[round+1][nxt_state.penalty].back().cur==nxt_state.cur)continue;
                        // if(field_map[round].count(nxt_state.cur)!=0&&field_map[round][nxt_state.cur] < nxt_state.penalty)continue;
                        // if(st.count(nxt_state.cur))continue;
                        chmin(min_cost[round+1],nxt_state.penalty);
                        // AA = GetSeconds();
                        // {
                        //     beam[round+1][nxt_state.penalty][sz[round+1][nxt_state.penalty]%beam_max] = nxt_state;
                        //     sz[round+1][nxt_state.penalty]++;
                        // }
                        beam[round+1][nxt_state.penalty].push_back(nxt_state);
                        // time_sum[5] +=  GetSeconds() - AA;
                        
                        // st.insert(nxt_state.cur);
                        // field_map[round][nxt_state.cur] = nxt_state.penalty;
                    }else{
                        vector<char> ffff;
                        // AA = GetSeconds();
                        rep(zz,4){
                            ffff = nxt_state.make_feasible_solution();
                            if(!ffff.empty())break;
                        }
                        // for(auto x:nxt_state.ans){
                        //     cerr << chara[x];
                        // }
                        // cerr << endl;
                        // time_sum[1] += GetSeconds()-AA;
                        if(!ffff.empty()){
                            if(n%2==0){
                                // AA = GetSeconds();
                                bool feasible = nxt_state.three_answer(ffff);
                                // time_sum[2] += GetSeconds()-AA;
                                if(feasible && best_penalty> nxt_state.penalty){
                                    // double AA = GetSeconds();
                                    best_state = nxt_state;  
                                    // time_sum[5] +=  GetSeconds() - AA; 
                                    best_penalty = nxt_state.penalty;
                                }
                            }
                            if(n%2==1){
                                // AA = GetSeconds();
                                bool feasible = nxt_state.four_answer(ffff);
                                // time_sum[3] += GetSeconds()-AA;
                                if(feasible && best_penalty> nxt_state.penalty){
                                    // double AA = GetSeconds();
                                    best_state = nxt_state;   
                                    // time_sum[5] +=  GetSeconds() - AA;
                                    best_penalty = nxt_state.penalty;
                                }
                            }
                        }
                    }
                }
            }
            int ccc = 0;
            for(int i=min_cost[round];i<STEPLIMIT;i++){
                if(ccc==beam_max){
                    // while(!beam[round][i].empty())beam[round][i].pop_back();
                    beam[round][i].clear();
                    continue;
                }
                // random_shuffle(all(beam[round][i]));
                ccc += beam[round][i].size();
                while(ccc > beam_max){
                    beam[round][i].pop_back();
                    ccc--;
                }
            }
            if(GetSeconds() - starttime >TIME_LIMIT)break;
        }
    }
    // cerr << counter << endl;
    // cerr << "copy time: " << time_sum[5] << endl;
    // cerr << "precalc time: " << time_sum[4] << endl;
    // cerr << "min search time: " << time_sum[6] << endl;
    // cerr << "put time: " << time_sum[0] << endl;
    // cerr << "make feasible time: " << time_sum[1] << endl; 
    // cerr << "make answer time: " << time_sum[2] + time_sum[3] << endl;
    // cerr << time_sum[0] + time_sum[1] + time_sum[2] + time_sum[4] + time_sum[4] + time_sum[5] + time_sum[6] << endl; 
    // // cerr << time_sum[0] << " " << time_sum[1] <<" " << time_sum[2] << " " << time_sum[3] << endl;
    // cerr << "put_dfs time :" << time_check[0] << endl;
    // cerr << "move time :" << time_check[1] << endl;
    // cerr << "pop_check time:" << time_check[2]  << endl;
    // cerr << GetSeconds() - starttime << endl;
    if(best_penalty>T){
        // cerr << "ERROR" << endl;
        return 0;
    }
    rep(i,best_state.ans.size()){
        cout << chara[best_state.ans[i]];
    }
    cout << "\n";
    
    
    // if(best_state.penalty > T)assert(0);
    // beam.clear();
    // rep(i,PIECES.size()){
    //     rep(j,STEPLIMIT){
    //         beam[i][j].clear();
    //         // while(!beam[i][j].empty())beam[i][j].pop_back();
    //     }
    // }
    // cerr << n << " " <<  500000 * (2- (double)best_state.ans.size()/T) << " " <<GetSeconds() - starttime << endl;
    // cerr << tm.get() << endl;
    return 0;
}