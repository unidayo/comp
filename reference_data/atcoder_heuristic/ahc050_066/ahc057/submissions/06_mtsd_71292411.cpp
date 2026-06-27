#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;
#define MP make_pair
#define MT make_tuple
#define PB push_back
#define fi first
#define se second
#define rep(i,n) for(int i = 0; i < (int)(n); ++i)
#define rrep(i,n) for(int i = (int)(n)-1; i >= 0; --i)
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

#define inf 1e20
static const double none=-inf/3;
template<class D>struct MWPM{
    #define rng(i,a,b) for(int i=int(a);i<int(b);i++)
    #define rep(i,b) rng(i,0,b)
    #define gnr(i,a,b) for(int i=int(b)-1;i>=int(a);i--)
    #define per(i,b) gnr(i,0,b)
    #define pb push_back
    #define eb emplace_back
    #define a first
    #define b second
    #define bg begin()
    #define ed end()
    #define all(x) x.bg,x.ed
    #define si(x) int(x.size())
    template<class t> using vc=vector<t>;
    template<class t> using vvc=vc<vc<t>>;

    using pi=pair<int,int>;
    using vi=vc<int>;
	struct E{
		int a,b;
		bool operator==(const E&e)const{
			return a==e.a&&b==e.b;
		}
		E rev()const{
			return E{b,a};
		}
	};
	const vvc<D>&g;
	const int n;
	vi idx,mt,bl,bs,tp;
	vc<D> u,z;
	vc<E> par,sf,ss;
	vvc<E> me,ce;
	vvc<int> cb;
	D p(const E&e){
		if(e.a==e.b)return inf;
		return u[e.a]+u[e.b]-g[e.a][e.b]*2;
	}
	void chme(E&x,const E&y){
		if(p(x)>p(y))
			x=y;
	}
	void match(E e){
		int a=e.a,b=e.b;
		mt[a]=b;
		mt[b]=a;
		bs[bl[a]]=a;
		bs[bl[b]]=b;
	}
	void setb(int v,int b){
		if(v<n)
			bl[v]=b;
		else
			for(auto c:cb[v])
				setb(c,b);
	}
	void make(const vc<E>&es){
		int v=idx.back();
		idx.pop_back();
		ce[v]=es;
		cb[v].clear();
		for(auto e:es)
			cb[v].pb(bl[e.a]);
		rep(i,n)
			me[v][i]=E{-1,-1};
		for(auto c:cb[v]){
			tp[c]=-2;
			setb(c,v);
			rep(i,n)
				chme(me[v][i],me[c][i]);
		}
		bs[v]=bs[cb[v][0]];
		z[v]=0;
		labelS(v,par[cb[v][0]]);
	}
	vc<E> path(int v){
		vc<E> es;
		while(par[bl[v]].a!=-1){
			es.pb(par[bl[v]]);
			v=par[bl[v]].a;
		}
		return es;
	}
	bool SS(E e){
		vc<E> x=path(e.a),y=path(e.b);
		while(x.size()&&y.size()&&x.back()==y.back()){
			x.pop_back();
			y.pop_back();
		}
		vc<E> es=x;
		reverse(all(es));
		es.pb(e);
		for(auto f:y)
			es.pb(f.rev());
		if(bl[es.front().a]==bl[es.back().b]){
			make(es);
			return false;
		}else{
			for(int i=0;i<int(es.size());i+=2){
				match(es[i]);
				for(auto j:{bl[es[i].a],bl[es[i].b]}){
					expand(j,true);
					setb(j,j);
				}
			}
			return true;
		}
	}
	void expand(int v,bool r){
		if(v<n)return;
		for(auto c:cb[v])
			setb(c,c);
		rep(i,cb[v].size())
			if(bl[bs[v]]==cb[v][i]){
				rotate(cb[v].begin(),cb[v].begin()+i,cb[v].end());
				rotate(ce[v].begin(),ce[v].begin()+i,ce[v].end());
				break;
			}
		bs[bl[bs[v]]]=bs[v];
		for(int i=1;i<int(ce[v].size());i+=2)
			match(ce[v][i]);
		if(r){
			for(auto c:cb[v])
				expand(c,true);
		}else{
			int w=bl[par[v].b];
			labelT(w,par[v]);
			rep(i,cb[v].size())
				if(cb[v][i]==w){
					if(i%2==0){
						rep(j,i)
							if(j%2==0)
								labelT(bl[ce[v][j].a],ce[v][j].rev());
							else
								labelS(bl[ce[v][j].a],ce[v][j].rev());
						rng(j,i+1,cb[v].size())
							tp[cb[v][j]]=-1;
					}else{
						rng(j,i,ce[v].size())
							if(j%2==0)
								labelT(bl[ce[v][j].b],ce[v][j]);
							else
								labelS(bl[ce[v][j].b],ce[v][j]);
						rng(j,1,i)
							tp[cb[v][j]]=-1;
					}
				}
			tp[v]=-2;
			idx.pb(v);
		}
	}
	void labelS(int v,const E&p){
		tp[v]=0;
		par[v]=p;
		ss[v]=E{-1,-1};
		rep(i,n)if(bl[i]!=v){
			if(tp[bl[i]]==0){
				chme(ss[v],me[v][i]);
				chme(ss[bl[i]],me[v][i].rev());
			}else{
				chme(sf[i],me[v][i]);
			}
		}
	}
	void labelT(int v,const E&p){
		tp[v]=1;
		par[v]=p;
	}
	vc<D> ans;
	vvc<int> sol;
	//adjacent matrix
	MWPM(const vvc<D>&gg):g(gg),n(g.size()),
		idx(n),mt(n,-1),bl(n),bs(n*2),tp(n*2,-1),u(n),z(n*2,0),
		par(n*2),sf(n),ss(n*2),me(n*2,vc<E>(n)),
		ce(n*2),cb(n*2){
		iota(all(idx),n);
		iota(all(bl),0);
		iota(all(bs),0);
		rng(i,n,n*2)tp[i]=-2;
		rep(i,n)rep(j,n)
			me[i][j]=E{i,j};
		{
			D mx=-inf;
			rep(i,n)rep(j,i)chmax(mx,g[i][j]);
			fill(all(u),mx);
		}
		ans.pb(0);
		sol.pb(vi(n,-1));
		rep(_,n/2){
			rep(i,n*2)if(tp[i]!=-2)
				tp[i]=-1;
			fill(all(sf),E{-1,-1});
			rep(i,n)if(mt[i]==-1)
				labelS(bl[i],E{-1,-1});
			while(true){
				D d=inf;
				E e;
				rep(i,n){
					int b=bl[i];
					D w=inf;
					E f;
					if(tp[b]==-1){
						w=p(sf[i]);
						f=sf[i];
					}else if(tp[b]==0){
						w=p(ss[b])/2;
						f=ss[b];
					}else if(n<=b){
						w=z[b]/2;
						f=E{b,-1};
					}
					if(d>w){
						d=w;
						e=f;
					}
				}
				rep(i,n){
					int t=tp[bl[i]];
					if(t==0)
						u[i]-=d;
					else if(t==1)
						u[i]+=d;
				}
				rng(i,n,n*2)
					if(tp[i]==0)
						z[i]+=d*2;
					else if(tp[i]==1)
						z[i]-=d*2;
				if(e.b==-1){
					expand(e.a,false);
				}else if(tp[bl[e.b]]==0){
					if(SS(e))
						break;
				}else{
					int t=bl[e.b],m=mt[bs[t]];
					labelT(t,e);
					labelS(bl[m],E{bs[t],m});
				}
			}
			bool bad=false;
			D sum=0;
			rep(i,n)if(i<mt[i]){
				if(g[i][mt[i]]==none){
					bad=true;
					break;
				}
				sum+=g[i][mt[i]];
			}
			if(bad)break;
			ans.pb(sum);
			sol.pb(mt);
		}
	}
};
// https://judge.yosupo.jp/submission/110742


// 固定パラメータ
const int N = 300;
const int T = 1000;
const int M = 10;
const int K = 30;
const double L = 1e5;

struct MovingPoint {
    double x, y;
    double vx, vy;
    int sz = 1;
    int born = 0;
    vector<int> members;
};

// トーラス上の差分を [-L/2, L/2) に正規化
double wrap_delta(double d) {
    d = fmod(d, L);
    if (d >= L / 2) d -= L;
    if (d < -L / 2) d += L;
    return d;
}

// r0 + v*t が (k+0.5)L をまたぐ時刻
vector<double> crossing_points(double r0, double v, double Tmax) {
    vector<double> res;
    if (fabs(v) < 1e-12) return res;
    double lo = min(r0, r0 + v * Tmax);
    double hi = max(r0, r0 + v * Tmax);
    long long k0 = (long long)ceil(lo / L - 0.5);
    long long k1 = (long long)floor(hi / L - 0.5);
    for (long long k = k0; k <= k1; ++k) {
        double t = ((k + 0.5) * L - r0) / v;
        if (0 < t && t < Tmax) res.push_back(t);
    }
    return res;
}

// [0, Tmax] 内の整数時刻で最も近づく時刻と距離二乗
pair<int, double> closest_time_within(const MovingPoint &p1, const MovingPoint &p2, int Tmax) {
    double dx0 = wrap_delta(p2.x - p1.x);
    double dy0 = wrap_delta(p2.y - p1.y);
    double dvx = p2.vx - p1.vx;
    double dvy = p2.vy - p1.vy;

    vector<double> ts = {0.0, (double)Tmax};
    auto xs = crossing_points(dx0, dvx, Tmax);
    auto ys = crossing_points(dy0, dvy, Tmax);
    ts.insert(ts.end(), xs.begin(), xs.end());
    ts.insert(ts.end(), ys.begin(), ys.end());
    sort(ts.begin(), ts.end());
    ts.erase(unique(ts.begin(), ts.end()), ts.end());

    auto eval = [&](int t, double rx0, double ry0) {
        double rx = rx0 + dvx * t;
        double ry = ry0 + dvy * t;
        return rx * rx + ry * ry;
    };

    double best_d2 = 1e100;
    int best_t = 0;
    for (int idx = 0; idx + 1 < (int)ts.size(); ++idx) {
        double a = ts[idx];
        double b = ts[idx + 1];
        int Lb = (int)ceil(a - 1e-9);
        int Rb = (int)floor(b + 1e-9);
        if (Lb < 0) Lb = 0;
        if (Rb > Tmax) Rb = Tmax;
        if (Lb > Rb) continue;
        double mid = (a + b) * 0.5;
        long long kx = llround((dx0 + dvx * mid) / L);
        long long ky = llround((dy0 + dvy * mid) / L);
        double rx0 = dx0 - kx * L;
        double ry0 = dy0 - ky * L;
        double denom = dvx * dvx + dvy * dvy;
        vector<int> cand = {Lb, Rb};
        if (denom > 0) {
            double tstar = - (rx0 * dvx + ry0 * dvy) / denom;
            int tf = (int)floor(tstar);
            int tc = (int)ceil(tstar);
            if (Lb <= tf && tf <= Rb) cand.push_back(tf);
            if (Lb <= tc && tc <= Rb) cand.push_back(tc);
        }
        for (int t : cand) {
            double d2 = eval(t, rx0, ry0);
            if (d2 < best_d2 - 1e-12) {
                best_d2 = d2;
                best_t = t;
            }
        }
    }
    return {best_t, best_d2};
}

MovingPoint advance_point(const MovingPoint &p, int dt) {
    MovingPoint np = p;
    np.x = fmod(p.x + p.vx * dt, L);
    np.y = fmod(p.y + p.vy * dt, L);
    if (np.x < 0) np.x += L;
    if (np.y < 0) np.y += L;
    np.born += (int)llround(dt);
    return np;
}

struct MergeCandidate {
    double cost;
    int best_t;
    int base_time;
    int merge_time;
    double dist2;
};

optional<MergeCandidate> merge_candidate(const MovingPoint &a, const MovingPoint &b, int deadline) {
    int base = max(a.born, b.born);
    if (base >= deadline) return nullopt;
    int Tmax = deadline - base;
    MovingPoint pa = advance_point(a, base - a.born);
    MovingPoint pb = advance_point(b, base - b.born);
    auto res = closest_time_within(pa, pb, Tmax);
    double d = sqrt(res.second);
    MergeCandidate mc;
    mc.cost = round(d);
    mc.best_t = res.first;
    mc.base_time = base;
    mc.merge_time = base + res.first;
    mc.dist2 = res.second;
    return mc;
}

class UnionFind {
private:
    int sz;
    vector<int> par, size_;
    vector<vector<int>> member_;
public:
    UnionFind(){}
    UnionFind(int node_size) : sz(node_size), par(sz), size_(sz, 1), member_(sz){
        iota(par.begin(), par.end(), 0);
        rep(i,node_size)member_[i].push_back(i);
    }
    int find(int x){
        if(par[x] == x) return x;
        else return par[x] = find(par[x]);
    }
    void unite(int x,int y){
        x = find(x), y = find(y);
        if(x == y) return;
        if(size_[x] < size_[y]) swap(x,y);
        par[y] = x;
        size_[x] += size_[y];
        for(auto yy:member_[y])member_[x].push_back(yy);
    }
    int size(int x){
        x = find(x);
        return size_[x];
    }
    vector<int> member(int x){
        x = find(x);
        return member_[x];
    }
    bool same(int x,int y){
        return find(x) == find(y);
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n_in, t_in, m_in, k_in;
    double L_in;
    if (!(cin >> n_in >> t_in >> m_in >> k_in >> L_in)) return 0;
    vector<MovingPoint> ps(N);
    for (int i = 0; i < N; ++i) {
        double x, y, vx, vy;
        cin >> x >> y >> vx >> vy;
        MovingPoint p;
        p.x = x; p.y = y; p.vx = vx; p.vy = vy;
        p.sz = 1; p.born = 0.0; p.members = {i};
        ps[i] = p;
    }
    auto PS = ps;

    vector<vector<int>> deadline_cand = {
        {199,399,599},
        {299,499,699},
        {199,499,799},
        {399,499,799},
        {499,599,799},
        {599,699,799},
        {299,499,899},
    };



    // vector<StageConfig> stages = {
    //     {3, 140, deadlines[0]},  // T1: 2*140,1*20
    //     {7, 60, deadlines[1]},   // T2: 4*60,3*20
    //     {15, 20, deadlines[2]},  // T3: 8*20,7*20
    //     {15, 20, deadlines[3]},  // T4: 15*20
    //     {30, 20, deadlines[4]},  // T5: 30*20
    // };
    auto merge = [](int i,int j,UnionFind &uf, vector<MovingPoint>&ps, int time)-> void {
        int si = uf.size(i);
        int sj = uf.size(j);
        // cerr << i << " " << j << endl;
        assert(!uf.same(i,j));
        double nxt_vx = (ps[i].vx*si+ps[j].vx*sj)/(si+sj);
        double nxt_vy = (ps[i].vy*si+ps[j].vy*sj)/(si+sj);
        uf.unite(i,j);
        for(auto x:uf.member(i)){
            ps[x] = advance_point(ps[x],time - ps[x].born);
            ps[x].vx = nxt_vx;
            ps[x].vy = nxt_vy;
            ps[x].born = time;
        }
    };

    using pii = pair<int,int>;
    double best_score = 1e10;
    vector<tuple<int,int,int>> best_ops;
    // for(auto deadlines:deadline_cand){
    for(int deadlines0=199;deadlines0<=499;deadlines0+=100){
        ps = PS;
        UnionFind uf(N);

        vector<tuple<int,int,int>> ops;
        double score = 0.0;
        // 2*140, 1*20
        int n = ps.size();
        vv(double,g,n,n,none);
        vv(optional<MergeCandidate>,cand,n,n);
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                auto m = merge_candidate(ps[i], ps[j], deadlines0);
                if (!m) continue;
                cand[i][j] = m;
                cand[j][i] = m;
                // if(i==182&&j==201)cerr << i << " " << j << " " << m->cost << " " << m->best_t << endl;
                g[i][j] = 1e7-m->cost;
                g[j][i] = 1e7-m->cost;
            }
        }

        MWPM wm(g);
        // cerr << "score:" << 140 << " " << 1e7 * 140 - wm.ans[140] << " " << (1e7 * 140 - wm.ans[140])/140 << endl;
        score += 1e7 * 140 - wm.ans[140];
        vector<int> sol = wm.sol[140];
        vector<pair<int,int>> p;
        rep(i,n){
            if(sol[i]!=-1&&sol[i]>i){
                p.push_back({i,sol[i]});
            }
        }
        for(auto [x,y]:p){
            merge(x,y,uf,ps,cand[x][y]->merge_time);
            ops.push_back({cand[x][y]->merge_time,x,y});
        }
        auto cur_ops = ops;
        auto cur_uf = uf;
        auto cur_ps = ps;
        auto cur_score = score;

        for(int deadlines1=deadlines0+100;deadlines1<=799;deadlines1+=100){
            auto ops = cur_ops;
            auto uf = cur_uf;
            auto ps = cur_ps;
            auto score = cur_score;
            // 4*60, 3*20
            int n = 160;
            vv(double,g,n,n,none);
            vv(optional<MergeCandidate>,cand,n,n);
            vv(pii,best_pair,n,n);
            map<int,int> mp;
            vector<int> resp;
            int num = 0;
            rep(i,N){
                if(uf.find(i)==i){
                    mp[i] = num++;
                    resp.push_back(i);
                }
            }
            for (int i = 0; i < N; ++i) {
                for (int j = i + 1; j < N; ++j) {
                    if(uf.same(i,j))continue;
                    if(uf.size(i)+uf.size(j)<3)continue;
                    auto m = merge_candidate(ps[i], ps[j], deadlines1);
                    if (!m) continue;
                    int ci = mp[uf.find(i)];
                    int cj = mp[uf.find(j)];
                    if(chmax(g[ci][cj],1e7-m->cost)){
                        cand[ci][cj] = m;
                        cand[cj][ci] = m;
                        best_pair[ci][cj] = {i,j}; 
                        best_pair[cj][ci] = {i,j}; 
                        g[ci][cj] = 1e7-m->cost;
                        g[cj][ci] = 1e7-m->cost;
                    }
                }
            }
            MWPM wm(g);
            // cerr << "score:" << 80 << " " << 1e7 * 80 - wm.ans[80] << " " << (1e7 * 80 - wm.ans[80])/80 << endl;
            score += 1e7 * 80 - wm.ans[80];
            vector<int> sol = wm.sol[80];
            vector<pair<int,int>> p;
            rep(i,n){
                if(sol[i]!=-1&&sol[i]>i){
                    p.push_back({i,sol[i]});
                }
            }
            for(auto [xx,yy]:p){
                auto [x,y] = best_pair[xx][yy];
                merge(x,y,uf,ps,cand[xx][yy]->merge_time);
                ops.push_back({cand[xx][yy]->merge_time,x,y});
            }
            auto cur_ops = ops;
            auto cur_uf = uf;
            auto cur_ps = ps;
            auto cur_score = score;
            for(int deadlines2=deadlines1+100;deadlines2<=899;deadlines2+=200){
                auto ops = cur_ops;
                auto uf = cur_uf;
                auto ps = cur_ps;
                auto score = cur_score;
                // 8*20, 7*20
                int n = 80;
                vv(double,g,n,n,none);
                vv(optional<MergeCandidate>,cand,n,n);
                vv(pii,best_pair,n,n);
                map<int,int> mp;
                vector<int> resp;
                int num = 0;
                rep(i,N){
                    if(uf.find(i)==i){
                        mp[i] = num++;
                        resp.push_back(i);
                    }
                }
                for (int i = 0; i < N; ++i) {
                    for (int j = i + 1; j < N; ++j) {
                        if(uf.same(i,j))continue;
                        if(uf.size(i)+uf.size(j)<7)continue;
                        auto m = merge_candidate(ps[i], ps[j], deadlines2);
                        if (!m) continue;
                        int ci = mp[uf.find(i)];
                        int cj = mp[uf.find(j)];
                        if(chmax(g[ci][cj],1e7-m->cost)){
                            cand[ci][cj] = m;
                            cand[cj][ci] = m;
                            best_pair[ci][cj] = {i,j}; 
                            best_pair[cj][ci] = {i,j}; 
                            g[ci][cj] = 1e7-m->cost;
                            g[cj][ci] = 1e7-m->cost;
                        }
                    }
                }
                MWPM wm(g);
                // cerr << "score:" << 40 << " " << 1e7 * 40 - wm.ans[40] << " " << (1e7 * 40 - wm.ans[40])/40 << endl;
                score += 1e7 * 40 - wm.ans[40] ;
                vector<int> sol = wm.sol[40];
                vector<pair<int,int>> p;
                rep(i,n){
                    if(sol[i]!=-1&&sol[i]>i){
                        p.push_back({i,sol[i]});
                    }
                }
                for(auto [xx,yy]:p){
                    auto [x,y] = best_pair[xx][yy];
                    merge(x,y,uf,ps,cand[xx][yy]->merge_time);
                    ops.push_back({cand[xx][yy]->merge_time,x,y});
                }
                auto cur_ops = ops;
                auto cur_uf = uf;
                auto cur_ps = ps;
                auto cur_score = score;
                for(int deadline3 = deadlines2+50;deadline3<999;deadline3 += 50){   
                    auto ops = cur_ops;
                    auto uf = cur_uf;
                    auto ps = cur_ps;
                    auto score = cur_score;
                    {
                        // 15*20
                        int n = 40;
                        vv(double,g,n,n,none);
                        vv(optional<MergeCandidate>,cand,n,n);
                        vv(pii,best_pair,n,n);
                        map<int,int> mp;
                        vector<int> resp;
                        int num = 0;
                        rep(i,N){
                            if(uf.find(i)==i){
                                mp[i] = num++;
                                resp.push_back(i);
                            }
                        }
                        dbg(num);
                        set<pair<int,int>> st;
                        for (int i = 0; i < N; ++i) {
                            for (int j = i + 1; j < N; ++j) {
                                if(uf.same(i,j))continue;
                                if(uf.size(i)+uf.size(j)!=15)continue;
                                auto m = merge_candidate(ps[i], ps[j], deadline3);
                                if (!m) continue;
                                int ci = mp[uf.find(i)];
                                int cj = mp[uf.find(j)];
                                if(chmax(g[ci][cj],1e7-m->cost)){
                                    cand[ci][cj] = m;
                                    cand[cj][ci] = m;
                                    best_pair[ci][cj] = {i,j}; 
                                    best_pair[cj][ci] = {i,j}; 
                                    g[ci][cj] = 1e7-m->cost;
                                    g[cj][ci] = 1e7-m->cost;
                                }
                            }
                        }
                        MWPM wm(g);
                        score += 1e7 * 20 - wm.ans[20];
                        vector<int> sol = wm.sol[20];
                        vector<pair<int,int>> p;
                        rep(i,n){
                            if(sol[i]!=-1&&sol[i]>i){
                                p.push_back({i,sol[i]});
                            }
                        }
                        for(auto [xx,yy]:p){
                            auto [x,y] = best_pair[xx][yy];
                            merge(x,y,uf,ps,cand[xx][yy]->merge_time);
                            ops.push_back({cand[xx][yy]->merge_time,x,y});
                        }
                    }
                    
                    {
                        // 30*10
                        int n = 20;
                        vv(double,g,n,n,none);
                        vv(optional<MergeCandidate>,cand,n,n);
                        vv(pii,best_pair,n,n);
                        map<int,int> mp;
                        vector<int> resp;
                        int num = 0;
                        rep(i,N){
                            if(uf.find(i)==i){
                                mp[i] = num++;
                                resp.push_back(i);
                            }
                        }
                        dbg(num);
                        set<pair<int,int>> st;
                        for (int i = 0; i < N; ++i) {
                            for (int j = i + 1; j < N; ++j) {
                                if(uf.same(i,j))continue;
                                auto m = merge_candidate(ps[i], ps[j], 999);
                                if (!m) continue;
                                int ci = mp[uf.find(i)];
                                int cj = mp[uf.find(j)];
                                if(chmax(g[ci][cj],1e7-m->cost)){
                                    cand[ci][cj] = m;
                                    cand[cj][ci] = m;
                                    best_pair[ci][cj] = {i,j}; 
                                    best_pair[cj][ci] = {i,j}; 
                                    g[ci][cj] = 1e7-m->cost;
                                    g[cj][ci] = 1e7-m->cost;
                                }
                            }
                        }
                        MWPM wm(g);
                        // cerr << "score:" << 10 << " " << 1e7 * 10 - wm.ans[10] << " " << (1e7 * 10 - wm.ans[10])/10 << endl;
                        score += 1e7 * 10 - wm.ans[10];
                        vector<int> sol = wm.sol[10];
                        vector<pair<int,int>> p;
                        rep(i,n){
                            if(sol[i]!=-1&&sol[i]>i){
                                p.push_back({i,sol[i]});
                            }
                        }
                        for(auto [xx,yy]:p){
                            auto [x,y] = best_pair[xx][yy];
                            merge(x,y,uf,ps,cand[xx][yy]->merge_time);
                            ops.push_back({cand[xx][yy]->merge_time,x,y});
                        }
                    }
                    if(chmin(best_score,score)){
                        best_ops = ops;
                    }
                    // cerr << "all:" << deadlines0 << " " << deadlines1 << " " << deadlines2 << " " << deadline3 << " " << score << endl;

                }
               
            }
        }
        
    }
        
       
    // }
    cerr << "best_score: " << best_score << endl;
    for (auto &op : best_ops) {
        int t, i, j;
        tie(t, i, j) = op;
        cout << t << " " << i << " " << j << "\n";
    }
    return 0;
}
