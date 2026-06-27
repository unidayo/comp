#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef unsigned long long ull;

#define mp make_pair
#define fr first
#define sc second

template<class T> T T_INF(){ return 1000000000000000000; }
template<> int T_INF<int>(){ return 1000000000; }

struct UF{
	vector<int> par,r;
	UF(int n){ init(n); }
	UF(){}
	void init(int n=-1){
		if(n==-1)n=par.size();
		par.resize(n);
		r.resize(n);
		iota(par.begin(),par.end(),0);
		fill(r.begin(),r.end(),0);
	}
	int find(int x){
		if(par[x]==x)return x;
		return par[x]=find(par[x]);
	}
 	bool same(int x,int y){ return find(x)==find(y); }
	void unit(int x,int y){
		if(same(x,y))return;
		x=find(x);
		y=find(y);
		if(r[x]>r[y])swap(x,y);
		par[x]=y;
		if(r[x]==r[y])r[y]++;
	}
};

uint32_t XOR(void) {
  static uint32_t y = 2463534242;
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return y = y ^ (y << 5);
}

const int N=400,M=1995;
int x[N],y[N];
int u[M],v[M];
int d[M];
int l[M];

const int X=1000,Y=100;
int sz[X];
int ind[Y][X];

int main(){
	for(int i=0;i<N;i++)scanf("%d%d",&x[i],&y[i]);
	for(int i=0;i<M;i++)scanf("%d%d",&u[i],&v[i]);
	
	for(int i=0;i<M;i++){
		d[i]=sqrt((float)(x[u[i]]-x[v[i]])*(x[u[i]]-x[v[i]])+(y[u[i]]-y[v[i]])*(y[u[i]]-y[v[i]]))+0.5;
	}
	
	UF uf_(N);
	
	for(int i=0;i<M;i++){
		scanf("%d",&l[i]);
		const int Q=84;
		int cnt=0;
		int sum=0;
		for(int q=0;q<Q;q++){
			for(int j=i+1;j<M;j++){
				l[j]=d[j]*(512+(XOR()&1023u))/512;
			}
			UF uf=uf_;
			for(int j=0;j<X;j++)sz[j]=0;
			for(int j=i+1;j<M;j++){
				ind[sz[l[j]]++][l[j]]=j;
			}
			for(int j=0;j<X;j++){
				for(int k=0;k<sz[j];k++){
					uf.unit(u[ind[k][j]],v[ind[k][j]]);
				}
				if(uf.same(u[i],v[i])){
					sum+=j;
					break;
				}
			}
			if(!uf.same(u[i],v[i]))sum+=X;
		}
		if(sum>=Q*l[i]*0.95)
		{
			cout<<1<<endl;
			fflush(stdout);
			l[i]=0;
			uf_.unit(u[i],v[i]);
		}
		else {
			cout<<0<<endl;
			fflush(stdout);
			l[i]=X-1;
		}
	}
}