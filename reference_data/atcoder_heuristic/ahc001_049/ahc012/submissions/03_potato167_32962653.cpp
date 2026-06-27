#include <bits/stdc++.h>
#pragma GCC optimize("Ofast")
#define _GLIBCXX_DEBUG
using namespace std;
using std::cout;
using std::cin;
using std::endl;
using ll=long long;
using ld=long double;
ll ILL=2167167167167167167;
const int INF=100000000;
const ll mod=998244353;
#define rep(i,a) for (int i=0;i<a;i++)
#define all(p) p.begin(),p.end()
template<class T> using _pq = priority_queue<T, vector<T>, greater<T>>;
template<class T> ll LB(vector<T> &v,T a){return lower_bound(v.begin(),v.end(),a)-v.begin();}
template<class T> ll UB(vector<T> &v,T a){return upper_bound(v.begin(),v.end(),a)-v.begin();}
template<class T> bool chmin(T &a,const T &b){if(a>b){a=b;return 1;}else return 0;}
template<class T> bool chmax(T &a,const T &b){if(a<b){a=b;return 1;}else return 0;}
template<class T> void So(vector<T> &v) {sort(v.begin(),v.end());}
template<class T> void Sore(vector<T> &v) {sort(v.begin(),v.end(),[](T x,T y){return x>y;});}
void yneos(bool a){if(a) cout<<"Yes\n"; else cout<<"No\n";}
template<class T> void vec_out(vector<T> &p){for(int i=0;i<(int)(p.size());i++){if(i) cout<<" ";cout<<p[i];}cout<<"\n";}

int N,K;
vector<int> A(11);
vector<int> X,Y;
vector<pair<int,int>> pos;
void solve(int div,int type);
void in();
ll score(vector<int> &B);
ll out_score(vector<int> &B);
vector<vector<int>> out_ans;
ll last_score=0;
void out();
// oddloop
int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	//srand(time(0));
	in();
	for(int d=5;d<17;d++){
		solve(d,0);
		solve(d,1);
		solve(d,2);
		solve(d,3);
		solve(d,4);
	}
	out();
}

void out(){
	cout<<out_ans.size()<<"\n";
	for(auto x:out_ans) vec_out(x);
}

void in(){
	cin>>N>>K;
	rep(i,10) cin>>A[i+1];
	X.resize(N),Y.resize(N);
	vector<pair<int,int>> p(N);
	rep(i,N) cin>>p[i].first>>p[i].second;
	So(p);
	pos=p;
	rep(i,N) tie(X[i],Y[i])=p[i];
}

ll score(vector<int> &B){
	if(B[0]>K) return ILL;
	ll ans=0;
	for(ll i=1;i<=10;i++){
		if(A[i]<=B[i]) continue;
		ll tmp=(A[i]-B[i])*(A[i]-B[i]+1);
		tmp*=(ll)(INF);
		ans+=((i*4-1)*tmp)/A[i];
		ans+=((ll)(INF)*(ll)(A[i]-B[i]))*i;
	}
	return ans;
}
ll out_score(vector<int> &B){
	if(B[0]>K) return ILL;
	ll ans=0,M=0;
	for(int i=1;i<=10;i++){
		ans+=min(A[i],B[i]);
		M+=A[i];
	}
	return -1*((ans*1000000ll)/M);
}
void solve(int div,int type){
	vector<vector<int>> ans;
	vector<int> S(div);
	if(type==0) rep(i,div) S[i]=((N*(i+1)))/div;
	else if(type==4){
		int L=1e4;
		for(int i=1;i<=div;i++){
			S[i-1]=LB(pos,{(L*2*i)/div-L,0});
		}
	}
	else{
		rep(i,div) S[i]=(type*100-(type*400*abs(div/2-i)*abs(div/2-i))/(div*div));
		int sum=0;
		rep(i,div) S[i]+=1000,sum+=S[i];
		int tmp=0;
		rep(i,div) tmp+=S[i],S[i]=(N*tmp)/sum;
	}
	rep(i,div-1){
		int a=pos[S[i]-1].first+pos[S[i]].first;
		int b=pos[S[i]-1].second+pos[S[i]].second;
		if(pos[S[i]-1].first==pos[S[i]].first){
			ans.push_back({pos[S[i]].first+1,pos[S[i]-1].second-INF,pos[S[i]].first-1,pos[S[i]-1].second+1+INF});
		}else{
			ans.push_back({pos[S[i]-1].first,INF,pos[S[i]].first,-INF});
		}
	}
	vector<tuple<int,int,int>> strawberries(N);
	rep(i,N){
		int k=0;
		rep(j,div) if(i>=S[j]) k++;
		strawberries[i]={pos[i].second,pos[i].first,k};
	}
	So(strawberries);
	vector<vector<int>> sum(div,vector<int>(N+1));
	rep(i,N){
		rep(j,div) sum[j][i+1]=sum[j][i];
		sum[get<2>(strawberries[i])][i+1]++;
	}
	vector<int> front(N+1);
	vector<vector<int>> p(N+1,vector<int>(11));
	rep(i,N+1) p[i][0]=div-2;
	vector<ll> val(N+1,ILL);
	for(int i=1;i<=N;i++){
		for(int j=max(0,(i-div*10));j<i;j++){
		//cout<<i<<" "<<j<<endl;
		auto tmp=p[j];
		for(int k=0;k<div;k++){
			if(sum[k][i]>sum[k][j]&&sum[k][i]-sum[k][j]<=10){
				tmp[sum[k][i]-sum[k][j]]++;
			}
		}
		tmp[0]++;
		if(i<(N*19)/20&&chmin(val[i],score(tmp))) p[i]=tmp,front[i]=j;
		if(i>=(N*19)/20&&chmin(val[i],out_score(tmp))) p[i]=tmp,front[i]=j;
		}
		if(p[i][0]<K&&chmin(val[N],out_score(p[i]))) front[N]=i,p[N]=p[i];
	}
	int ind=N;
	while(true){
		ind=front[ind];
		if(ind==0) break;
		int a1,b1,c1,a2,b2,c2;
		tie(a1,b1,c1)=strawberries[ind];
		tie(a2,b2,c2)=strawberries[ind-1];
		if(a1==a2){
			ans.push_back({-INF+b2,a1+1,INF+b2+1,a1-1});
		}else{
			ans.push_back({INF,a1,-INF,a2});
		}
	}
	if(chmin(last_score,out_score(p[N]))){
		out_ans=ans;
	}
	//out_ans=ans;
	//out();
}

