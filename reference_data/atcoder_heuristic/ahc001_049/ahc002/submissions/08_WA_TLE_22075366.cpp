#include<deque>
#include<queue>
#include<vector>
#include<algorithm>
#include<iostream>
#include<set>
#include<cmath>
#include<tuple>
#include<string>
#include<chrono>
#include<functional>
#include<iterator>
#include<random>
#include<unordered_set>
#include<array>
#include<map>
#include<iomanip>
#include<assert.h>
#include<list>
#include<bitset>
#include<stack>
#include<memory>
#include<numeric>
#include <utility>


using namespace std;
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
const llint mod=1000000007;
const llint inf=2.19e15+1;
const long double pai=3.141592653589793238462643383279502884197;
const long double eps=1e-10;
template <class T,class U>bool chmin(T& a,U b){if(a>b){a=b;return true;}return false;}
template <class T,class U>bool chmax(T& a,U b){if(a<b){a=b;return true;}return false;}
llint gcd(llint a,llint b){if(a%b==0){return b;}else return gcd(b,a%b);}
llint lcm(llint a,llint b){if(a==0){return b;}return a/gcd(a,b)*b;}
template<class T> void SO(T& ve){sort(ve.begin(),ve.end());}
template<class T> void REV(T& ve){reverse(ve.begin(),ve.end());}
template<class T>llint LBI(const vector<T>&ar,T in){return lower_bound(ar.begin(),ar.end(),in)-ar.begin();}
template<class T>llint UBI(const vector<T>&ar,T in){return upper_bound(ar.begin(),ar.end(),in)-ar.begin();}
inline double getTime() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
double starttime;
inline double myTime(){return getTime()-starttime;}//ミリ秒単位

int main(void){
	cout<<fixed<<setprecision(20);
	cin.tie(0);ios::sync_with_stdio(false);
	mt19937 engine(46);
	//焼きなまし法
	int i,j,n=50,sy,sx;cin>>sy>>sx;
	sy++;sx++;
	int t[52][52]={};
	int p[52][52]={};
	for(i=1;i<=n;i++){
		for(j=1;j<=n;j++){
			cin>>t[i][j];
			t[i][j]++;
		}
	}
	for(i=1;i<=n;i++){
		for(j=1;j<=n;j++){
			cin>>p[i][j];
		}
	}
	starttime=getTime();
	int tx[52][52]={};//相方のタイルのx
	int ty[52][52]={};//相方のタイルのx
	//0,1,2,3,4
	//U,D,L,R,x
	int dy[5]={-1,1,0,0,0};
	int dx[5]={0,0,-1,1,0};
	for(i=1;i<=n;i++){
		for(j=1;j<=n;j++){
			ty[i][j]=i;tx[i][j]=j;
			if(t[i][j]==t[i-1][j]){ty[i][j]=i-1;}
			if(t[i][j]==t[i+1][j]){ty[i][j]=i+1;}
			if(t[i][j]==t[i][j-1]){tx[i][j]=j-1;}
			if(t[i][j]==t[i][j+1]){tx[i][j]=j+1;}
		}
	}
	int use[52][52]={};
	use[sy][sx]=1;
	int ren[52][52]={};//どっち側とつながってるか情報
	for(i=0;i<=n+1;i++){
		use[0][i]=1;
		use[i][0]=1;
		use[n+1][i]=1;
		use[i][n+1]=1;
	}
	int gy=sy,gx=sx;
	int pena=30;
	int asiz=0;
	int ondo=800;
	int Eval=12,Aval=11,Bval=11;
	uniform_real_distribution<> dist(0,1);
	for(int kai=0;kai<31000;kai++){
		if((kai&127)==0){
			//gx終点を貪欲で伸ばせる場合、伸ばす
			//これは適当でよくない
			double nowT=myTime();
			if(myTime()>1800){break;}
			
			if((kai&511)==0){
				
				if(kai>1500&&nowT<1600){
					for(int kaz=0;kaz<Aval;kaz++){
						if(ren[gy][gx]==1){use[gy][gx]=0;ren[gy][gx]=0;gy--;ren[gy][gx]-=2;}
						else if(ren[gy][gx]==2){use[gy][gx]=0;ren[gy][gx]=0;gy++;ren[gy][gx]-=1;}
						else if(ren[gy][gx]==4){use[gy][gx]=0;ren[gy][gx]=0;gx--;ren[gy][gx]-=8;}
						else if(ren[gy][gx]==8){use[gy][gx]=0;ren[gy][gx]=0;gx++;ren[gy][gx]-=4;}
					}
				}
				
				use[gy][gx]=0;
				vector<vector<pair<int,int>>>goa(1);
				goa[0].pub(mp(gy,gx));
				for(int mi=0;mi<goa.size()&&goa[mi].size()<Aval+5;mi++){
					//上下左右に伸ばす
					//if(goa.size()>10000){break;}
					for(int d=0;d<4;d++){
						//cerr<<"de"<<__LINE__<<" ";
						int ay=goa[mi].back().fir + dy[d];
						int ax=goa[mi].back().sec + dx[d];
						if(use[ay][ax]){continue;}
						if(ay<=0||ay>n||ax<=0||ax>n){cerr<<"bug";continue;}
						if(ty[ay][ax]<=0||ty[ay][ax]>n||tx[ay][ax]<=0||tx[ay][ax]>n){cerr<<"bug";continue;}
						if(use[ty[ay][ax]][tx[ay][ax]]){continue;}
						//cerr<<"de"<<__LINE__<<" ";
						int dame=false;
						for(auto it:goa[mi]){
							if(ay==it.fir&&ax==it.sec){dame=true;break;}
							if(ty[ay][ax]==it.fir&&tx[ay][ax]==it.sec){dame=true;break;}
						}
						if(!dame){
							goa.pub(goa[mi]);
							goa.back().pub(mp(ay,ax));
						}
					}
				}
				pair<int,int>ssai=mp(0,0);
				for(int mi=0;mi<goa.size();mi++){
					int sco=0;
					for(auto it:goa[mi]){sco+=p[it.fir][it.sec];}
					chmax(ssai,mp(sco,mi));
				}
				auto aaa=goa[ssai.sec];
				//for(auto it:aaa){cerr<<it.fir<<" "<<it.sec<<endl;}
				for(int s=0;s+1<aaa.size();s++){
					use[aaa[s].fir][aaa[s].sec]=1;
					if(aaa[s].fir-1==aaa[s+1].fir){
						ren[aaa[s].fir][aaa[s].sec]+=1;
						ren[aaa[s+1].fir][aaa[s+1].sec]+=2;
					}else if(aaa[s].fir+1==aaa[s+1].fir){
						ren[aaa[s].fir][aaa[s].sec]+=2;
						ren[aaa[s+1].fir][aaa[s+1].sec]+=1;
					}else if(aaa[s].sec-1==aaa[s+1].sec){
						ren[aaa[s].fir][aaa[s].sec]+=4;
						ren[aaa[s+1].fir][aaa[s+1].sec]+=8;
					}else if(aaa[s].sec+1==aaa[s+1].sec){
						ren[aaa[s].fir][aaa[s].sec]+=8;
						ren[aaa[s+1].fir][aaa[s+1].sec]+=4;
					}
				}
				use[aaa.back().fir][aaa.back().sec]=1;
				gy=aaa.back().fir;
				gx=aaa.back().sec;
			}
			
			cerr<<"asiz="<<asiz<<endl;
			asiz=0;
			int pot=0;
			for(i=1;i<=n;i++){
				for(j=1;j<=n;j++){
					if(use[i][j]){pot+=p[i][j];}
				}
			}
			cerr<<"pot="<<pot<<endl;
			Eval=12+kai/1500;
			Aval=11+(kai+800)/1500;
			Bval=11+(kai+800)/1500;
			if(Eval%2!=(Aval+Bval)%2){Bval++;}
			pena= max(0.0,70-(nowT*80/1800));
			ondo=3500*(1-nowT/1600)*(1-nowT/1600);
			if(nowT>1600){ondo=0;}
			cerr<<"kai="<<kai<<" "<<Eval<<" "<<Aval<<" "<<Bval<<endl;
			cerr<<"pena="<<pena<<endl;
		}
		
		//まず、ランダムにタイルを見つける
		int uy=engine()%n +1,ux=engine()%n +1;
		while(!use[uy][ux]){
			uy=engine()%n +1,ux=engine()%n +1;
		}
		//その後、片側10タイルを消去 gy,gxが上限
		int kd=4,kkk=engine()%4;
		if(ren[uy][ux]&(1<<kkk)){kd=kkk;}
		else if(ren[uy][ux]&(1<<(kkk^1))){kd=(kkk^1);}
		else if(ren[uy][ux]&(1<<(kkk^2))){kd=(kkk^2);}
		
		if(kd==4){cerr<<"bug"<<endl;continue;}
		
		int eay=uy,eax=ux;
		ren[uy][ux]-=(1<<kd);
		uy+=dy[kd];ux+=dx[kd];
		ren[uy][ux]-=(1<<(kd^1));
		for(int kaz=0;kaz<Eval;kaz++){
			if(ren[uy][ux]==1){use[uy][ux]=0;ren[uy][ux]=0;uy--;ren[uy][ux]-=2;}
			else if(ren[uy][ux]==2){use[uy][ux]=0;ren[uy][ux]=0;uy++;ren[uy][ux]-=1;}
			else if(ren[uy][ux]==4){use[uy][ux]=0;ren[uy][ux]=0;ux--;ren[uy][ux]-=8;}
			else if(ren[uy][ux]==8){use[uy][ux]=0;ren[uy][ux]=0;ux++;ren[uy][ux]-=4;}
		}
		
		/*for(i=1;i<=n;i++){
			for(j=1;j<=n;j++){
				cerr<<use[i][j];
			}cerr<<endl;
		}*/
		//cerr<<"de"<<__LINE__<<endl;
		int eby=uy,ebx=ux;
		//その後、両端から22マスを上限に探索.
		
		//goa,gobから11マスずつを探索する
		
		use[eay][eax]=0;use[eby][ebx]=0;
		vector<vector<pair<int,int>>>goa(1);
		goa[0].pub(mp(eay,eax));
		//cerr<<"de"<<__LINE__<<endl;
		//cerr<<"ebx="<<ebx<<"eby="<<eby<<endl;
		for(int mi=0;mi<goa.size()&&goa[mi].size()<Aval;mi++){
			//上下左右に伸ばす
			//if(goa.size()>10000){break;}
			for(int d=0;d<4;d++){
				//cerr<<"de"<<__LINE__<<" ";
				int ay=goa[mi].back().fir + dy[d];
				int ax=goa[mi].back().sec + dx[d];
				if(use[ay][ax]){continue;}
				if(ay<=0||ay>n||ax<=0||ax>n){cerr<<"bug";continue;}
				if(ty[ay][ax]<=0||ty[ay][ax]>n||tx[ay][ax]<=0||tx[ay][ax]>n){cerr<<"bug";continue;}
				if(use[ty[ay][ax]][tx[ay][ax]]){continue;}
				//cerr<<"de"<<__LINE__<<" ";
				int dame=false;
				for(auto it:goa[mi]){
					if(ay==it.fir&&ax==it.sec){dame=true;break;}
					if(ty[ay][ax]==it.fir&&tx[ay][ax]==it.sec){dame=true;break;}
				}
				if(!dame){
					goa.pub(goa[mi]);
					goa.back().pub(mp(ay,ax));
				}
			}
		}
		
		vector<vector<pair<int,int>>>gob(1);
		gob[0].pub(mp(eby,ebx));
		for(int mi=0;mi<gob.size()&&gob[mi].size()<Bval;mi++){
			//上下左右に伸ばす
			//if(gob.size()>10000){break;}
			for(int d=0;d<4;d++){
				//cerr<<"de"<<__LINE__<<" ";
				int by=gob[mi].back().fir + dy[d];
				int bx=gob[mi].back().sec + dx[d];
				if(use[by][bx]){continue;}
				if(by<=0||by>n||bx<=0||bx>n){cerr<<"bug";continue;}
				if(ty[by][bx]<=0||ty[by][bx]>n||tx[by][bx]<=0||tx[by][bx]>n){cerr<<"bug";continue;}
				if(use[ty[by][bx]][tx[by][bx]]){continue;}
				//cerr<<"de"<<__LINE__<<" ";
				int dame=false;
				for(auto it:gob[mi]){
					if(by==it.fir&&bx==it.sec){dame=true;break;}
					if(ty[by][bx]==it.fir&&tx[by][bx]==it.sec){dame=true;break;}
				}
				if(!dame){
					gob.pub(gob[mi]);
					gob.back().pub(mp(by,bx));
				}
			}
		}
		//cerr<<"size="<<goa.size()<<endl;
		asiz+=goa.size()+gob.size();
		//cerr<<"de"<<__LINE__<<endl;
		
		vector<tuple<int,int,int,int>>soa(goa.size());
		vector<tuple<int,int,int,int>>sob(gob.size());
		
		for(int mi=0;mi<goa.size();mi++){
			int pot=0;
			for(auto it:goa[mi]){
				pot+=p[it.fir][it.sec];
				pot-=p[ty[it.fir][it.sec]][tx[it.fir][it.sec]]*pena/100;
			}
			soa[mi]=mt(goa[mi].back().fir,goa[mi].back().sec,pot,mi);
		}
		for(int mi=0;mi<gob.size();mi++){
			int pot=0;
			for(auto it:gob[mi]){
				pot+=p[it.fir][it.sec];
				pot-=p[ty[it.fir][it.sec]][tx[it.fir][it.sec]]*pena/100;
			}
			sob[mi]=mt(gob[mi].back().fir,gob[mi].back().sec,pot,mi);
		}
		SO(soa);
		SO(sob);
		tuple<int,int,int>sai=mt(-mod,0,0);
		int bL=-1,bR=-1;
		
		for(int mi=soa.size()-1;mi>=0;mi--){
			if(mi==soa.size()-1||get<0>(soa[mi])!=get<0>(soa[mi+1])||get<1>(soa[mi])!=get<1>(soa[mi+1])){
				tuple<int,int,int,int> ttt=mt(get<0>(soa[mi]),get<1>(soa[mi]),-mod,0);
				bL=LBI(sob,ttt);
				ttt=mt(get<0>(soa[mi]),get<1>(soa[mi]),mod,0);
				bR=LBI(sob,ttt)-1;
			}
			for(int mj=bR;mj>=bL;mj--){
				int sco=0;
				sco=get<2>(soa[mi])+get<2>(sob[mj])-p[get<0>(soa[mi])][get<1>(soa[mi])];
				sco+=ondo*log(dist(engine));
				if(sco<get<0>(sai)){continue;}
				
				//両者のがぶつかってないことを確認する
				bool dame=false;
				for(auto it:goa[get<3>(soa[mi])]){
					if(it==goa[get<3>(soa[mi])].back()){break;}
					for(auto jt:gob[get<3>(sob[mj])]){
						if(it==jt){dame=true;break;}
						if(ty[it.fir][it.sec]==jt.fir&&tx[it.fir][it.sec]==jt.sec){dame=true;break;}
					}
					if(dame){break;}
				}
				if(goa[get<3>(soa[mi])].back()!=gob[get<3>(sob[mj])].back()){cerr<<"bug"<<__LINE__<<endl;}
				if(!dame){chmax(sai,mt(sco,get<3>(soa[mi]),get<3>(sob[mj])));}
			}
		}
		if(goa[get<1>(sai)].back()!=gob[get<2>(sai)].back()){cerr<<"bug"<<__LINE__<<endl;break;}
		if(get<0>(sai)==-mod){cerr<<"bug"<<__LINE__<<endl;break;}
		//cerr<<"de"<<__LINE__<<endl;
		//焼きなまし温度に応じたランダム加算の後、最終的に、多いものを選ぶ
		
		auto aaa=goa[get<1>(sai)];
		//for(auto it:aaa){cerr<<it.fir<<" "<<it.sec<<endl;}
		for(int s=0;s+1<aaa.size();s++){
			use[aaa[s].fir][aaa[s].sec]=1;
			if(aaa[s].fir-1==aaa[s+1].fir){
				ren[aaa[s].fir][aaa[s].sec]+=1;
				ren[aaa[s+1].fir][aaa[s+1].sec]+=2;
			}else if(aaa[s].fir+1==aaa[s+1].fir){
				ren[aaa[s].fir][aaa[s].sec]+=2;
				ren[aaa[s+1].fir][aaa[s+1].sec]+=1;
			}else if(aaa[s].sec-1==aaa[s+1].sec){
				ren[aaa[s].fir][aaa[s].sec]+=4;
				ren[aaa[s+1].fir][aaa[s+1].sec]+=8;
			}else if(aaa[s].sec+1==aaa[s+1].sec){
				ren[aaa[s].fir][aaa[s].sec]+=8;
				ren[aaa[s+1].fir][aaa[s+1].sec]+=4;
			}
		}
		use[aaa.back().fir][aaa.back().sec]=1;
		
		aaa=gob[get<2>(sai)];
		if(aaa[0].fir!=eby){cerr<<"bug"<<__LINE__<<endl;break;}
		
		for(int s=0;s+1<aaa.size();s++){
			use[aaa[s].fir][aaa[s].sec]=1;
			if(aaa[s].fir-1==aaa[s+1].fir){
				ren[aaa[s].fir][aaa[s].sec]+=1;
				ren[aaa[s+1].fir][aaa[s+1].sec]+=2;
			}else if(aaa[s].fir+1==aaa[s+1].fir){
				ren[aaa[s].fir][aaa[s].sec]+=2;
				ren[aaa[s+1].fir][aaa[s+1].sec]+=1;
			}else if(aaa[s].sec-1==aaa[s+1].sec){
				ren[aaa[s].fir][aaa[s].sec]+=4;
				ren[aaa[s+1].fir][aaa[s+1].sec]+=8;
			}else if(aaa[s].sec+1==aaa[s+1].sec){
				ren[aaa[s].fir][aaa[s].sec]+=8;
				ren[aaa[s+1].fir][aaa[s+1].sec]+=4;
			}
		}
		//cerr<<"de"<<__LINE__<<endl;
		/*for(i=1;i<=n;i++){
			for(j=1;j<=n;j++){
				cerr<<use[i][j];
			}cerr<<endl;
		}*/
	}
	while(sy!=gy||sx!=gx){
		if(ren[sy][sx]==1){cout<<"U";sy--;ren[sy][sx]-=2;}
		if(ren[sy][sx]==2){cout<<"D";sy++;ren[sy][sx]-=1;}
		if(ren[sy][sx]==4){cout<<"L";sx--;ren[sy][sx]-=8;}
		if(ren[sy][sx]==8){cout<<"R";sx++;ren[sy][sx]-=4;}
		//cerr<<ren[sy][sx]<<endl;
		if(ren[sy][sx]<0){cerr<<"bug"<<endl;break;}
	}
	cout<<endl;
	return 0;
}

