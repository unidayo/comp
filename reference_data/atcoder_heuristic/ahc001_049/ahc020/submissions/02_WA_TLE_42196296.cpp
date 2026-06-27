#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cwctype>
#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>
#include <array>
#include <atomic>
#include <chrono>
#include <codecvt>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

using namespace std;
using namespace std::chrono;
typedef long long int llint;
typedef long double lldo;
#define mp make_pair
#define mt make_tuple
#define pub push_back
#define puf push_front
#define pob pop_back
#define pof pop_front
#define fir first
#define sec second
#define res resize
#define ins insert
#define era erase
#define REP(i, n) for(int i = 0;i < (n);i++)
/*cout<<fixed<<setprecision(20);cin.tie(0);ios::sync_with_stdio(false);*/
const int mod = 998244353;
const llint inf = 2.19e18 + 1;
const long double PI = 3.141592653589793238462643383279502884197;
const long double eps = 1e-10;
template <class T, class U>bool chmin(T& a, U b) { if (a > b) { a = b; return true; }return false; }
template <class T, class U>bool chmax(T& a, U b) { if (a < b) { a = b; return true; }return false; }
llint gcd(llint a, llint b) { if (a % b == 0) { return b; } else return gcd(b, a % b); }
llint lcm(llint a, llint b) { if (a == 0) { return b; }return a / gcd(a, b) * b; }
template<class T> void SO(T& ve) { sort(ve.begin(), ve.end()); }
template<class T> void REV(T& ve) { reverse(ve.begin(), ve.end()); }
template<class T>int LBI(const vector<T>& ar, T in) { return lower_bound(ar.begin(), ar.end(), in) - ar.begin(); }
template<class T>int UBI(const vector<T>& ar, T in) { return upper_bound(ar.begin(), ar.end(), in) - ar.begin(); }
inline double getTime() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
double starttime;
inline double myTime(){return getTime()-starttime;}//ミリ秒単位
mt19937 engine(1333);
int dist(pair<int,int>a,pair<int,int>b){
	int norm=(a.first-b.first)*(a.first-b.first)+(a.second-b.second)*(a.second-b.second);
	return int(sqrt(norm))+1;
}

void output(vector<int>hen_on,array<llint,100>kyori,llint sen_teisu){
	
	for(int i=0;i<100;i++){
		if(kyori[i]==0){cout<<0<<" ";}
		else{
			cout<<int(sqrt(kyori[i]-sen_teisu+0.5))<<" ";
		}
	}
	cout<<endl;
	for(auto it:hen_on){cout<<it<<" ";}cout<<endl;
}
int main(void) {
	int n,m,K,i,j;cin>>n>>m>>K;
	vector<pair<int,int>>tow(n);
	for(i=0;i<n;i++){cin>>tow[i].fir>>tow[i].sec;}
	vector<vector<tuple<int,llint,int>>>road(n);
	vector<tuple<llint,int,int,int>>hen(m);
	
	for(i=0;i<m;i++){
		int u,v,w;cin>>u>>v>>w;
		u--;v--;
		road[u].pub(mt(v,w,i));
		road[v].pub(mt(u,w,i));
		hen[i]=mt(w,u,v,i);
	}
	uniform_real_distribution<> Rdistribution(0.0, 1.0);
	vector<pair<int,int>>home(K);
	for(i=0;i<K;i++){cin>>home[i].first>>home[i].second;}
	

	llint zen_kaz=4;
	tuple<llint,array<llint,100>,vector<int>>true_ans;
	get<0>(true_ans)=inf;
	llint sen_teisu=400000;
	for(int zen_kai=0;zen_kai<zen_kaz;zen_kai++){
		starttime=getTime();
		vector<int>hen_on(m);
		llint cost=0;

		llint D=500;
		vector<pair<int,int>>Shome;
		vector<vector<pair<llint,int>>>HTd;//コスト、塔名
		vector<vector<pair<llint,int>>>THd(n);//コスト、家名
	
		for(i=0;i<K;i++){
			bool ok=true;
			for(auto it:Shome){
				if(dist(it,home[i])<D){ok=false;break;}
			}
			if(!ok){continue;}
			int Si=Shome.size();
			Shome.pub(home[i]);
		
			vector<pair<llint,int>>aaa;
			for(j=0;j<n;j++){
				llint C=(dist(tow[j],home[i])+D)*(dist(tow[j],home[i])+D)+sen_teisu;
				if(C<=25000000+sen_teisu){
					aaa.pub(mp(C,j));
					THd[j].pub(mp(C,Si));
				}
			}
			HTd.pub(aaa);
		}
		for(i=0;i<n;i++){
			THd[i].pub(mp(0,-1));
			THd[i].pub(mp(mod,-1));
			SO(THd[i]);
		}
	
		array<llint,100>kyori;
		array<int,100>kyoran;
		for(i=0;i<n;i++){
			kyoran[i]=((int)THd[i].size()-2);
			cost+=THd[i][kyoran[i]].first;
			kyori[i]=THd[i][kyoran[i]].first;
		}
		pair<llint,array<llint,100>>ans;
		tuple<llint,llint,array<int,100>>gen;
	

		double zi=0;
		double ondo_bai=100000*(3+zen_kai*5.0/zen_kaz);
		double ondo=ondo_bai;
		int SK=Shome.size();
		for(int st=0;st<n;st++){
			if(kyoran[st]==0){continue;}//もともとoff
			//if(THd[st][kyoran[st]].second!=ie){continue;}
			//stの塔
			cost-=kyori[st];
			while(kyoran[st]>0){
				int sie=THd[st][kyoran[st]].second;
				bool kesu=false;
				for(auto saaa:HTd[sie]){
					if(saaa.second==st){continue;}
					if(saaa.first <= kyori[saaa.sec]){kesu=true;}
				}
				if(!kesu){break;}
				kyoran[st]--;
				kyori[st]=THd[st][kyoran[st]].fir;
			}
			cost+=kyori[st];
		
		}
		ans=mp(cost,kyori);
		gen=mt(cost,cost,kyoran);

		bool changed=false;
	
		for(int kai=0;true;kai++){
			if((kai&127)==0){
				zi = myTime() / (1600.0/zen_kaz);
				if(zi>=0.5&&(!changed)){
					//output(hen_on,kyori);
					changed=true;
					Shome=home;
					HTd.clear();
					THd.clear();
					THd.resize(n);

					for(int i=0;i<K;i++){
						vector<pair<llint,int>>aaa;
						for(j=0;j<n;j++){
							llint C=(dist(tow[j],home[i]))*(dist(tow[j],home[i]))+sen_teisu;
							if(C<=25000000+sen_teisu){
								aaa.pub(mp(C,j));
								THd[j].pub(mp(C,i));
							}
						}
						HTd.pub(aaa);
					}
					for(i=0;i<n;i++){
						cost-=kyori[i];
						THd[i].pub(mp(0,-1));
						THd[i].pub(mp(mod,-1));
						SO(THd[i]);
						kyoran[i]=UBI(THd[i],mp(kyori[i]+5,0))-1;
						cost+=THd[i][kyoran[i]].first;
						kyori[i]=THd[i][kyoran[i]].first;
					}
					//output(hen_on,kyori);
					for(int st=0;st<n;st++){
						if(kyoran[st]==0){continue;}//もともとoff
						//if(THd[st][kyoran[st]].second!=ie){continue;}
						//stの塔
						cost-=kyori[st];
						while(kyoran[st]>0){
							int sie=THd[st][kyoran[st]].second;
							bool kesu=false;
							for(auto saaa:HTd[sie]){
								if(saaa.second==st){continue;}
								if(saaa.first <= kyori[saaa.sec]){kesu=true;}
							}
							if(!kesu){break;}
							kyoran[st]--;
							kyori[st]=THd[st][kyoran[st]].fir;
						}
						cost+=kyori[st];
					}
					//output(hen_on,kyori);
					ans=mp(cost,kyori);
					gen=mt(cost,cost,kyoran);

				}
				
				ondo=(1-zi)*(1-zi)*ondo_bai;
				if(zi>=0.9){
					llint true_cost=ans.first;
					for(i=0;i<m;i++){hen_on[i]=0;}
					vector<int>kita(n);
					kita[0]=2;
					while(-1){
						priority_queue<tuple<int,int,int,int>,vector<tuple<int,int,int,int>>,greater<tuple<int,int,int,int>>>que;
						//距離、場所、前の地点、その道
						vector<int>mae(n,-1);
						vector<int>maroad(n,-1);
						for(i=0;i<n;i++){
							if(kita[i]==1){kita[i]=0;}
							if(kita[i]){que.push(mt(0,i,-1,-1));}
						}
						int terbas=-1;
						while(que.size()){
							int d,p,mab,mar;tie(d,p,mab,mar)=que.top();
							que.pop();
							if(kita[p]==1){continue;}

							if(kita[p]==0){kita[p]=1;}
							//cerr<<mab<<" "<<mar<<endl;
							mae[p]=mab;
							maroad[p]=mar;
							//cerr<<"p="<<p<<endl;
							if(kita[p]==1&&ans.sec[p]>0){terbas=p;break;}
							for(auto it:road[p]){
								int u;llint ds;int rname;
								tie(u,ds,rname)=it;
								if(kita[u]){continue;}
								que.push(mt(d+ds,u,p,rname));
							}
						}
						if(terbas==-1){break;}
						while(kita[terbas]!=2){
							//cerr<<terbas<<" "<<maroad[terbas]<<endl;
							kita[terbas]=2;
							hen_on[maroad[terbas]]=1;
							true_cost+=get<0>(hen[maroad[terbas]]);
							terbas=mae[terbas];
						}

					}
					cerr<<"tc="<<true_cost<<endl;
					chmin(true_ans,mt(true_cost,ans.second,hen_on));
				}
				if(zi>=1){break;}
			
			}

			get<0>(gen)=get<1>(gen);
			cost=get<1>(gen);
			kyoran=get<2>(gen);
			for(i=0;i<n;i++){
				kyori[i]=THd[i][kyoran[i]].first;
			}
		


			int ter=engine()%n;
			//terの円を一段づつ大きくする
			while(kyoran[ter]+2<THd[ter].size()){
				cost-=kyori[ter];
				kyoran[ter]++;
				kyori[ter]=THd[ter][kyoran[ter]].fir;
				cost+=kyori[ter];
				//その他の円が小さくなる
				int ie=THd[ter][kyoran[ter]].sec;
				bool heru=false;
				for(auto it:HTd[ie]){
					int st=it.second;//円を小さくする塔
					if(st==ter){continue;}
					if(kyoran[st]==0){continue;}//もともとoff
					if(THd[st][kyoran[st]].second!=ie){continue;}
					//stの塔
					cost-=kyori[st];
					while(kyoran[st]>0){
						int sie=THd[st][kyoran[st]].second;
						bool kesu=false;
						for(auto saaa:HTd[sie]){
							if(saaa.second==st){continue;}
							if(saaa.first <= kyori[saaa.sec]){kesu=true;}
						}
						if(!kesu){break;}
						kyoran[st]--;
						kyori[st]=THd[st][kyoran[st]].fir;
						heru=true;
					}
					cost+=kyori[st];
				}

				//このcostでgenに更新をかけたい
				if(heru){
					chmin(ans,mp(cost,kyori));
					chmin(gen,mt(cost+log(Rdistribution(engine))*ondo,cost,kyoran));
				}
			}

		}
		cost=ans.first;
		kyori=ans.second;
		//シュタイナー木問題を解く
		//プリム砲みたいなやつ
		

	}
	
	
	
	output(get<2>(true_ans),get<1>(true_ans),sen_teisu);
	cerr<<"cost="<<get<0>(true_ans)<<endl;
	cerr<<"score="<<((llint)(1e6*(1+1e8/(get<0>(true_ans)+1e7))))<<endl;


	return 0;
}
/*
点を選び、　その点の周囲200を削除
辺コスト：最小全域木
常に条件を満たすようにする
動作：放送局の強度を大きくする

kai=55808
*/