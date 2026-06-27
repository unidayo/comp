#include <bits/stdc++.h>
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")
using namespace std;
using std::cout;
using std::cin;
using std::endl;
using ll=long long;
using ld=long double;
ll ILL=2167167167167167167;
const int INF=2100000000;
const int mod=998244353;
#define rep(i,a,b) for (ll i=a;i<b;i++)
#define all(p) p.begin(),p.end()
template<class T> using _pq = priority_queue<T, vector<T>, greater<T>>;
template<class T> ll LB(vector<T> &v,T a){return lower_bound(v.begin(),v.end(),a)-v.begin();}
template<class T> ll UB(vector<T> &v,T a){return upper_bound(v.begin(),v.end(),a)-v.begin();}
template<class T> bool chmin(T &a,T b){if(a>b){a=b;return 1;}else return 0;}
template<class T> bool chmax(T &a,T b){if(a<b){a=b;return 1;}else return 0;}
template<class T> void So(vector<T> &v) {sort(v.begin(),v.end());}
template<class T> void Sore(vector<T> &v) {sort(v.begin(),v.end(),[](T x,T y){return x>y;});}
void yneos(bool a){if(a) cout<<"Yes\n"; else cout<<"No\n";}
template<class T> void vec_out(vector<T> &p){for(int i=0;i<(int)(p.size());i++){if(i) cout<<" ";cout<<p[i];}cout<<"\n";}
template<class T> T vec_min(vector<T> &a){assert(!a.empty());T ans=a[0];for(auto &x:a) chmin(ans,x);return ans;}
template<class T> T vec_max(vector<T> &a){assert(!a.empty());T ans=a[0];for(auto &x:a) chmax(ans,x);return ans;}
template<class T> T vec_sum(vector<T> &a){assert(!a.empty());T ans=a[0]-a[0];for(auto &x:a) ans+=x;return ans;}
int pop_count(long long a){int res=0;while(a){res+=(a&1),a>>=1;}return res;}
//https://ei1333.github.io/library/other/xor-shift.cpp.html
struct XorShift {
private:
  constexpr static double R = 1.0 / 0xffffffff;
  uint64_t x;

public:
  explicit XorShift(uint64_t seed = 88172645463325252ull) : x(seed) {}

  template< typename T = uint64_t >
  inline T get() { // [0, 2^64)
    x ^= x << 7ull;
    x ^= x >> 9ull;
    return x;
  }

  inline uint32_t get(uint32_t r) { // [0, r)
    return ((uint64_t) get< uint32_t >() * r) >> 32ull;
  }

  inline uint32_t get(uint32_t l, uint32_t r) { // [l, r)
    return l + get(r - l);
  }

  inline double probability() { // [0.0, 1.0]
    return get< uint32_t >() * R;
  }
};


template<class T>
void shuffle_vector(vector<T> &p,XorShift &table){
	int L=p.size();
	for(int i=0;i<L;i++){
		int ind=table.get(i,L);
		swap(p[ind],p[i]);
	}
	return ;
}

XorShift rand_table;
namespace po167{
struct UFtree
{
	using _F=int;
	int _n;
	std::vector<_F> wei;
	std::vector<int> q;
	int component;
	UFtree(int n):_n(n),wei(n),component(n),par(n){
		for(int i=0;i<n;i++){
			wei[i]=1,par[i]=i;
		}
	}
	void intialize(){
		for(auto x:q){
			wei[root(x)]=1;
			par[x]=x;
		}
		component=(int)par.size();
		q={};
	}
	//根っこを返す
	int root(int a){
		assert(0<=a&&a<_n);
		if(a==par[a]) return a;
		return par[a]=root(par[a]);
	}
	//trueなら1,falseなら0
	int same(int a,int b){
		assert(0<=a&&a<_n);
		assert(0<=b&&b<_n);
		if(root(a)==root(b)) return 1;
		else return 0;
	}
	_F size(int a){
		return wei[root(a)];
	}
	//a,bが違う根っこの元なら結合する,結合したらtrueを返す
	bool unite(int a,int b){
		a=root(a),b=root(b);
		if(a==b) return false;
		if(wei[a]<wei[b]) std::swap(a,b);
		par[b]=a;
		q.push_back(b);
		wei[a]+=wei[b];
		component--;
		return true;
	}
	private:
	std::vector<int> par;
};
}
using po167::UFtree;

struct point{
	int x;
	int y;
};

int dist(point a,point b){
	int res=(a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y);
	int z=sqrt(res);
	while(z*z<res) z++;
	return z;
}

int N,M,K;
vector<int> U,V,order,var_order;
vector<ll> W;
vector<point> broad,house;
vector<vector<int>> BtoS;


void inp(){
	cin>>N>>M>>K;
	broad.resize(N),U.resize(M),V.resize(M),W.resize(M),house.resize(K);
	rep(i,0,N) cin>>broad[i].x>>broad[i].y;
	rep(i,0,M) cin>>U[i]>>V[i]>>W[i],U[i]--,V[i]--;
	rep(i,0,K) cin>>house[i].x>>house[i].y;
	BtoS.resize(N);
	rep(i,0,N){
		BtoS[i].resize(K);
		rep(j,0,K) BtoS[i][j]=dist(broad[i],house[j]);
	}
	var_order.resize(N);
	rep(i,0,N) var_order[i]=i;
}

struct STA{
	vector<int> P,B;
	vector<vector<int>> near;
	vector<int> unuse;
	STA(){
		P.resize(N,0),B.resize(M,0);
		near.resize(N,{});
		unuse.resize(N,0);
	}
	ll score;
	bool wrong(){
		rep(i,0,N) if(P[i]>5000) return true;
		return false;
	}
	ll calc(){
		vector<int> ok(K);
		UFtree T(N);
		score=0;
		rep(i,0,M){
			if(B[i]) T.unite(U[i],V[i]),score+=W[i];
		}
		rep(i,0,N){
			if(P[i]>5000){
				score=ILL;
				return score;
			}
			score+=P[i]*P[i];
			if(T.same(i,0)){
				rep(j,0,M){
					if(BtoS[i][j]<=P[i]){
						ok[j]=1;
					}
				}
			}
		}
		if(vec_min(ok)==0) score=ILL;
		return score;
	}
	ll calc_simp(){
		score=0;
		rep(i,0,M){
			if(B[i]) score+=W[i];
		}
		rep(i,0,N){
			if(P[i]>5000){
				score=ILL;
				return score;
			}
			score+=P[i]*P[i];
		}
		return score;
	}
	void small_ch(int ind,int sign){
		score+=sign*P[ind]*P[ind];
	}
	void erase_edge(){
		vector<int> var(N);
		vector<vector<int>> G(N);
		rep(i,0,M){
			if(B[i]) var[U[i]]++,var[V[i]]++,G[U[i]].push_back(i),G[V[i]].push_back(i);
		}
		vector<int> p;
		rep(i,1,N){
			if(var[i]==1&&P[i]==0){
				p.push_back(i);
			}
		}
		rep(i,0,(int)p.size()){
			int a=p[i];
			unuse[a]=1;
			for(auto x:G[a]){
				int y=(a^U[x]^V[x]);
				if(unuse[y]) continue;
				B[x]=0;
				var[y]--;
				if(y!=0&&var[y]==1&&P[y]==0) p.push_back(y);
			}
		}
	}
	void out(){
		vec_out(P);
		vec_out(B);
	}
};

void re_edge(STA &state,int v){
	if(state.P[v]) return;
	if(v==0) return;
	auto tmp=state;
	rep(i,0,M) tmp.B[i]=0;
	tmp.unuse[v]=1;
	int X=0;
	rep(i,0,N) X+=tmp.unuse[i];
	UFtree T(N);
	rep(i,0,M){
		int a=order[i];
		if(tmp.unuse[U[a]]+tmp.unuse[V[a]]) continue;
		if(T.unite(U[a],V[a])) tmp.B[a]=1;
	}
	/*
	if(v==72){
		cout<<state.calc_simp()<<" "<<tmp.calc_simp()<<endl;
		cout<<X+1<<" "<<T.component<<endl;
	}*/
	if(T.component==X+1&&state.calc_simp()>tmp.calc_simp()){
		state=tmp;
	}
}

STA init(){
	order.resize(M);
	rep(i,0,M) order[i]=i;
	sort(all(order),[&](int l,int r){return W[l]<W[r];});
	STA res;
	UFtree T(N);
	rep(i,0,M){
		int a=order[i];
		if(T.unite(U[a],V[a])) res.B[a]=1;
	}
	rep(i,0,K){
		int a=0,d=dist(broad[0],house[i]);
		rep(j,0,N){
			if(chmin(d,dist(broad[j],house[i]))) a=j;
		}
		res.near[a].push_back(i);
		chmax(res.P[a],d);
	}
	res.calc_simp();
	rep(i,1,N){
		re_edge(res,i);
	}
	res.erase_edge();
	return res;
}

//https://gasin.hatenadiary.jp/entry/2019/09/03/162613
double TL =(double)(1.8)*(double)(CLOCKS_PER_SEC);
double TL2=(double)(0.5)*(double)(CLOCKS_PER_SEC);

int diff(int a,int b){
	return abs(a*a-b*b);
}

void move_house(STA &state,int a){
	bool ok=0;
	shuffle_vector(var_order,rand_table);
	rep(i,0,N){
		int b=var_order[i];
		if(state.unuse[b]==0&&BtoS[b][a]<=state.P[b]){
			state.near[b].push_back(a);
			ok=1;
			break;
		}
	}
	if(!ok){
		int n_ind=-1;
		rep(i,0,N){
			int b=var_order[i];
			if(state.unuse[b]==0){
				if(n_ind==-1) n_ind=b;
				else if(diff(BtoS[n_ind][a],state.P[n_ind])
						>diff(BtoS[b][a],state.P[b])) n_ind=b;
			}
		}
		state.small_ch(n_ind,-1);
		chmax(state.P[n_ind],BtoS[n_ind][a]);
		state.small_ch(n_ind,1);
		state.near[n_ind].push_back(a);
	}
}

void modify (STA& state,int all=0) {
	vector<int> p;
	vector<ll> q;
	int ind;
	rep(i,0,N){
		if(state.P[i]==0) continue;
		p.push_back(i);
		q.push_back(INF/((int)state.near[i].size()*(int)state.near[i].size()));
	}
	if(all==0){
		ind=p[rand_table.get()%(int)(p.size())];
		int s=state.near[ind].size();
		if(s!=1){
			rep(rp,0,2) rep(i,0,s-1){
				if(BtoS[ind][state.near[ind][i]]>BtoS[ind][state.near[ind][i+1]]) swap(state.near[ind][i],state.near[ind][i+1]);
			}
			state.unuse[ind]=1;
			state.small_ch(ind,-1);
			state.P[ind]=BtoS[ind][state.near[ind][s-2]];
			move_house(state,(*state.near[ind].rbegin()));
			state.near[ind].resize(s-1);
			state.small_ch(ind,1);
			state.unuse[ind]=0;
			return;
		}
	}
	rep(i,1,(int)q.size()) q[i]+=q[i-1];
	ind=p[LB(q,(ll)(rand_table.get()%(*q.rbegin())))];
	shuffle_vector(state.near[ind],rand_table);
	state.unuse[ind]=1;
	state.P[ind]=0;
	for(auto a:state.near[ind]){
		move_house(state,a);
	}
	state.unuse[ind]=0;
	state.near[ind].clear();
	re_edge(state,ind);
	//state.erase_edge();
	state.calc_simp();
}

int local=0;

STA mountain() {
    STA state;
    state=init();
	state.out();
    double start_time=clock(); // 開始時刻
    while (true) { // 時間の許す限り回す
        double now_time=clock(); // 現在時刻
        if (now_time - start_time > TL) break;
        STA new_state = state;
		/*
        if (now_time-start_time >TL2) modify(new_state);
		else modify(new_state,1);
		*/
		modify(new_state);
		if(new_state.wrong()) continue;
        if (state.score> new_state.score) {
		//if(state.calc()>new_state.calc()){
            state = new_state;
			if(local) state.out();
        }
    }
	state.erase_edge();
	state.out();
	rep(i,1,N) re_edge(state,i);
	state.out();
	return state;
}


double start_temp = 1000000;
double end_temp = 1000;
double temp = 100000;

STA sa() {
    STA state;
    state=init();
	state.out();
    double start_time=clock(); // 開始時刻
    while (true) { // 時間の許す限り回す
        double now_time=clock(); // 現在時刻
        if (now_time - start_time > TL) break;
        STA new_state = state;
		/*
        if (now_time-start_time >TL2) modify(new_state);
		else modify(new_state,1);
		*/
		modify(new_state);
		if(new_state.wrong()) continue;
        // 温度関数
        double temp = start_temp + (end_temp - start_temp) * (now_time-start_time) / TL;
        // 遷移確率関数(最大化の場合)
        double prob = exp((-new_state.score+state.score)/temp);
        if (prob > (rand_table.get()%INF)/(double)INF) { // 確率probで遷移する
            state = new_state;
			if(local) state.out();
        }
    }
	rep(i,0,N) re_edge(state,i);
	state.erase_edge();
	state.out();
	return state;
}

void solve();
// oddloop
int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	
	int t=1;
	//cin>>t;
	rep(i,0,t) solve();
}

void solve(){
	inp();
	//mountain();
	sa();
}
