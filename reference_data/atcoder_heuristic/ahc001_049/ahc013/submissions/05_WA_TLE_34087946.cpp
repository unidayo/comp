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

#include <atcoder/mincostflow>
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






using namespace atcoder;


class Servers{
	public:


	int K,n;
	array<array<int,48>,48>grid; //内部番号のglid
	array<array<int,48>,48>plan;
	//0->空白 1~5->縦線 9~13->横線 +32->一時的に避難する
	//cp系統の番号と同じ-> そこにものがある
	array<array<int,4>,600>connect;//各頂点のconnect先 ulrd
	
	array<pair<int,int>,600>cp_mae;//もともといた場所
	array<pair<int,int>,600>cp_ato;//設計された移動先、 (0,0)は未決定を表す
	
	array<array<int,48>,48>grid_act;
	array<pair<int,int>,600>cp_act;
	vector<tuple<int,int,int,int>> movingX,movingY;
	
	vector<tuple<int,int,int,int>>cp_road;
	int Cu,Cv;//色2つ。
	
	array<array<array<int,9>,9>,600> sycost;
	//各コンピューターについて、 周辺にマスがずれたときのコスト
	array<int,6>Ckaz;
	mcf_graph<int,int> flow_gra;

	int maxsco=0;
	vector<tuple<int,int,int,int>> MmovingX,MmovingY; 

	int dr[4]={-1,0,1,0};
	int dc[4]={0,-1,0,1};
	//ULDR
	mt19937 engine;
	uniform_real_distribution<double> dist;

	double Xcost=0;//予想外の追加コストの推定

	bool outcheck(int a,int b){return (a==-1||b==-1||a==n||b==n);}
	
	
	bool do_move(tuple<int,int,int,int>zyo){
		int tr,tc,ar,ac;
		tie(tr,tc,ar,ac)=zyo;

		if(tr==ar&&tc==ac){return true;}
		if(abs(tr-ar)+abs(tc-ac)>1){
			//cerr<<"bug"<<__LINE__<<"tr="<<tr<<"tc="<<tc<<"ar="<<ar<<"ac="<<ac<<"\n";
			return false;

		}
		if(grid_act[tr][tc]==0 || grid_act[ar][ac]!=0){
			return false;
		}
		
		movingX.push_back(mt(tr,tc,ar,ac));
		swap(grid_act[tr][tc],grid_act[ar][ac]);
		cp_act[grid_act[ar][ac]]=mp(ar,ac);
		return true;
	}
	array<array<int,48>,48> kyori;

	bool roadmake(int ter,int tr,int tc){
		//ter番を自動で[tr][tc]に動かす。
		//邪魔な奴をうまくどかせるかがポイント
		
		
		if(cp_act[ter]==mp(tr,tc)){return true;}//移動は終わっている
		
		//とりあえず、縦->横 の上にあるコンピューターに避難指示を出す

		//経路探索を実行する 既に存在するコンピューターについては、 2回目以降のみ通れないものとして扱う
		//目的地とのユーグリッド距離で、 補正を変える
		int i,j;
		int Ca=(ter/100);

		int mr=cp_act[ter].fir,mc=cp_act[ter].sec;
		int siz=max(abs(mr-tr),abs(mc-tc))+3;
		//cerr<<"mr="<<mr<<"mc="<<mc<<"tr="<<tr<<"tc="<<tc<<"\n";
		for(i=max(0,mr-siz);i<=min(n-1,mr+siz);i++){
			for(j=max(0,mc-siz);j<=min(n-1,mc+siz);j++){
				kyori[i][j]=30000;
			}
		}
		queue<tuple<int,int,int>> que[34];
		que[0].push(mt(mr,mc,0));
		kyori[mr][mc]=0;
		

		int nk=0;
		while(-1){
			while(nk<30&&que[nk].size()==0){nk++;}
			if(nk==30){
				//cerr<<"de"<<__LINE__<<endl;
				return false;
			}
			int gr,gc,zyu;
			tie(gr,gc,zyu)=que[nk].front();
			zyu++;
			que[nk].pop();
			if((kyori[gr][gc]>>6)!=nk){continue;}
			//cerr<<nk<<" "<<gr<<" "<<gc<<"\n";
			if(gr==tr&&gc==tc){break;}
			
			if(gr>0&&mr-gr+1<siz){
				int ccos=nk;
				if(gr<=tr){ccos+=2;}
				if(cp_ato[grid_act[gr-1][gc]].fir==-1||(grid_act[gr-1][gc]/100)==Ca){
					if(grid_act[gr-1][gc]>0){ccos++;}
					if(chmin(kyori[gr-1][gc],ccos*64+zyu)){que[ccos].push(mt(gr-1,gc,zyu));}
				}
			}
			if(gr+1<n&&gr-mr+1<siz){
				int ccos=nk;
				if(gr>=tr){ccos+=2;}
				if(cp_ato[grid_act[gr+1][gc]].fir==-1||(grid_act[gr+1][gc]/100)==Ca){
					if(grid_act[gr+1][gc]>0){ccos++;}
					if(chmin(kyori[gr+1][gc],ccos*64+zyu)){que[ccos].push(mt(gr+1,gc,zyu));}
				}
				
			}
			if(gc>0&&mc-gc+1<siz){
				int ccos=nk;
				if(gc<=tc){ccos+=2;}
				if(cp_ato[grid_act[gr][gc-1]].fir==-1||(grid_act[gr][gc-1]/100)==Ca){
					if(grid_act[gr][gc-1]>0){ccos++;}
					if(chmin(kyori[gr][gc-1],ccos*64+zyu)){que[ccos].push(mt(gr,gc-1,zyu));}
				}
			}
			if(gc+1<n&&gc-mc+1<siz){
				int ccos=nk;
				if(gc>=tc){ccos+=2;}
				if(cp_ato[grid_act[gr][gc+1]].fir==-1||(grid_act[gr][gc+1]/100)==Ca){
					if(grid_act[gr][gc+1]>0){ccos++;}
					if(chmin(kyori[gr][gc+1],ccos*64+zyu)){que[ccos].push(mt(gr,gc+1,zyu));}
				}
			}
		}
		if(kyori[tr][tc]==30000){return false;}
		//ここから経路復元
		int ar=tr,ac=tc;
		plan[ar][ac]|=1024;
		
		vector<tuple<int,int,int,int>>pppp;
		vector<tuple<int,int,int,int>>qqqq;

		while(ar!=mr||ac!=mc){
			//周囲4つの中でkyoriが一番近いものを探す
			tuple<int,int,int>sore=mt(9999,0,0);
			if(ar>0)  {chmin(sore,mt(kyori[ar-1][ac],ar-1,ac));}
			if(ar+1<n){chmin(sore,mt(kyori[ar+1][ac],ar+1,ac));}
			if(ac>0)  {chmin(sore,mt(kyori[ar][ac-1],ar,ac-1));}
			if(ac+1<n){chmin(sore,mt(kyori[ar][ac+1],ar,ac+1));}

			qqqq.pub(mt(get<1>(sore),get<2>(sore),ar,ac));
			ar=get<1>(sore);
			ac=get<2>(sore);
			if((grid_act[ar][ac]/100)!=Ca||(mr==ar&&mc==ac)){
				REV(qqqq);
				for(auto it:qqqq){pppp.pub(it);}
				qqqq.clear();
			}
			plan[ar][ac]|=1024;
		}
		REV(pppp);
		for(auto it:pppp){cp_road.pub(it);}
		return true;
	}
	
	bool actplan_do(void){
		grid_act=grid;
		cp_act=cp_mae;
		movingX.clear();
		movingY.clear();
		//現在のplanにしたがってmovingを更新
		
		int h,i,j;
		cp_road.clear();
		flow_gra=mcf_graph<int,int>(n*n+2);
		
		
		//まずは必要な動かすコンピューターの導線を確保
		for(int kai=0;kai<=10;kai++){
			bool nokori=false;
			for(int hs=1;hs<=5;hs++){
				if(hs!=Cu&&hs!=Cv){continue;}
				for(h=hs*100;h<(hs+1)*100;h++){
					if(cp_ato[h].fir==-1){continue;}
					if(cp_act[h]==cp_ato[h]){continue;}
					if(cp_ato[grid_act[cp_ato[h].fir][cp_ato[h].sec]].fir==-1&&roadmake(h,cp_ato[h].fir,cp_ato[h].sec)){
						grid_act[cp_ato[h].fir][cp_ato[h].sec]=h;
						grid_act[cp_act[h].fir][cp_act[h].sec]=0;
						cp_act[h]=cp_ato[h];
					}else{nokori=true;}
				}
			}
			if(!nokori){break;}
			if(kai%3==2){
				for(int hs=1;hs<=5;hs++){
					if(hs!=Cu&&hs!=Cv){continue;}
					for(h=hs*100;h<(hs+1)*100;h++){
						if(cp_ato[h].fir==-1){continue;}
						if(cp_ato[h]!=cp_act[h]&&cp_ato[grid_act[cp_ato[h].fir][cp_ato[h].sec]].fir!=-1){
							//強制的に、周囲4方向のうち、空いている方に移動させる
							tuple<int,int,int> sai=mt(9999,-1,-1);
							if(cp_act[h].fir>0&&cp_ato[grid_act[cp_act[h].fir-1][cp_act[h].sec]].fir==-1){
								int uuuu=(cp_act[h].fir-1-cp_ato[h].fir)*(cp_act[h].fir-1-cp_ato[h].fir)+(cp_act[h].sec-cp_ato[h].sec)*(cp_act[h].sec-cp_ato[h].sec);
								chmin(sai,mt(uuuu,cp_act[h].fir-1,cp_act[h].sec));
							}
							if(cp_act[h].fir+1<n&&cp_ato[grid_act[cp_act[h].fir+1][cp_act[h].sec]].fir==-1){
								int uuuu=(cp_act[h].fir+1-cp_ato[h].fir)*(cp_act[h].fir+1-cp_ato[h].fir)+(cp_act[h].sec-cp_ato[h].sec)*(cp_act[h].sec-cp_ato[h].sec);
								chmin(sai,mt(uuuu,cp_act[h].fir+1,cp_act[h].sec));
							}
							if(cp_act[h].sec>0&&cp_ato[grid_act[cp_act[h].fir][cp_act[h].sec-1]].fir==-1){
								int uuuu=(cp_act[h].fir-cp_ato[h].fir)*(cp_act[h].fir-cp_ato[h].fir)+(cp_act[h].sec-1-cp_ato[h].sec)*(cp_act[h].sec-1-cp_ato[h].sec);
								chmin(sai,mt(uuuu,cp_act[h].fir,cp_act[h].sec-1));
							}
							if(cp_act[h].sec+1<n&&cp_ato[grid_act[cp_act[h].fir][cp_act[h].sec+1]].fir==-1){
								int uuuu=(cp_act[h].fir-cp_ato[h].fir)*(cp_act[h].fir-cp_ato[h].fir)+(cp_act[h].sec+1-cp_ato[h].sec)*(cp_act[h].sec+1-cp_ato[h].sec);
								chmin(sai,mt(uuuu,cp_act[h].fir,cp_act[h].sec+1));
							}
							if(get<1>(sai)!=-1){
								roadmake(h,get<1>(sai),get<2>(sai));
								grid_act[get<1>(sai)][get<2>(sai)]=h;
								grid_act[cp_act[h].fir][cp_act[h].sec]=0;
								cp_act[h]=mp(get<1>(sai),get<2>(sai));
							}
						}
					}
				}

			}
		}
		//お互いに交換する配置で、移動ができずに詰んでいる
		int damenum=0;
		for(h=0;h<600;h++){
			if(cp_ato[h].fir==-1){continue;}
			if(cp_ato[h]!=cp_act[h]){damenum++;}
		}
		if(damenum>0){
			//cerr<<"de"<<__LINE__<<endl;
			return false;
		}


		grid_act=grid;
		cp_act=cp_mae;
		//次に邪魔なコンピューターをかたづける
		//フローアルゴリズムで割り当てを決める
		//「何を何に割り当てるか」ではなく、「その道をゴミが何回通るか」で記録。
		int flow_ter=0;
		for(i=0;i<n;i++){
			for(j=0;j<n;j++){
				//価値のあるものは通れない
				if(i+1<n&&cp_ato[grid[i][j]].fir==-1&&cp_ato[grid[i+1][j]].fir==-1){
					flow_gra.add_edge(i*n+j+n,i*n+j,100,1);
					flow_gra.add_edge(i*n+j,i*n+j+n,100,1);
				}
				if(j+1<n&&cp_ato[grid[i][j]].fir==-1&&cp_ato[grid[i][j+1]].fir==-1){
					flow_gra.add_edge(i*n+j+1,i*n+j,100,1);
					flow_gra.add_edge(i*n+j,i*n+j+1,100,1);
				}
				
				if(grid[i][j]!=0&&cp_ato[grid[i][j]].fir==-1){
					//そこにゴミがある
					flow_ter++;
					flow_gra.add_edge(n*n,i*n+j,1,0);
				}
				
				if(plan[i][j]==0){flow_gra.add_edge(i*n+j,n*n+1,1,0);}//そこは移動可能
			}
		}
		
		pair<int,int> flow_data=flow_gra.flow(n*n,n*n+1);
		//cerr<<"flow_ryo="<<flow_data.fir<<" flow_cost="<<flow_data.sec<<"\n";
		if(flow_data.fir+6<flow_ter){return false;}
		//局地的に流れないものを対処しなきゃいけない

		//n*n,i*n+j,1,0の辺に無理なのがあるれば、対処する
		
		auto hens=flow_gra.edges();
		vector<array<int,4>>ryo(n*n);
		stack<tuple<int,int,int,int>>que;
		for(auto it:hens){
			if(it.from==n*n||it.to==n*n+1){continue;}
			if(it.to==it.from-n){ryo[it.from][0]=it.flow;}
			if(it.to==it.from-1){ryo[it.from][1]=it.flow;}
			if(it.to==it.from+1){ryo[it.from][3]=it.flow;}
			if(it.to==it.from+n){ryo[it.from][2]=it.flow;}
			if(it.flow!=0&&grid_act[it.from/n][it.from%n]!=0&&grid_act[it.to/n][it.to%n]==0)
			{que.push(mt(it.from/n,it.from%n,it.to/n,it.to%n));}
		}
		//移動量を表す
		//記録された通りに貪欲法で移動する
		
		int yokeru=0;
		while(que.size()){
			int tr,tc,ar,ac;
			tie(tr,tc,ar,ac)=que.top();
			que.pop();
			if(outcheck(tr,tc)||outcheck(ar,ac)){continue;}
			if(grid_act[tr][tc]==0||grid_act[ar][ac]!=0){continue;}
			int muki;
			if(ar<tr){muki=0;}
			else if(ac<tc){muki=1;}
			else if(ac>tc){muki=3;}
			else{muki=2;}
			if(ryo[tr*n+tc][muki]==0){continue;}
			ryo[tr*n+tc][muki]--;
			if(!do_move(mt(tr,tc,ar,ac))){cerr<<"bug"<<__LINE__<<"\n";}
			yokeru++;
			
			que.push(mt(tr-1,tc,tr,tc));
			que.push(mt(tr,tc-1,tr,tc));
			que.push(mt(tr+1,tc,tr,tc));
			que.push(mt(tr,tc+1,tr,tc));
			que.push(mt(ar,ac,ar-1,ac));
			que.push(mt(ar,ac,ar,ac-1));
			que.push(mt(ar,ac,ar+1,ac));
			que.push(mt(ar,ac,ar,ac+1));
		}
		
		for(i=0;i<n;i++){
			for(j=0;j<n;j++){
				//どうしても移動できなかったやつを無理やり動かす
				if(grid_act[i][j]!=0&&cp_ato[grid_act[i][j]].fir==-1&&plan[i][j]!=0){
					bool dekita=false;
					for(int d=0;d<4;d++){
						for(int e=0;e<4;e++){
							for(int f=0;f<4;f++){
								if(e==f||(d^2)==e||(d^2)==f){continue;}
								//A,B,C,D
								int Ar=i,Ac=j,Br=i+dr[d],Bc=j+dc[d];
								int Cr=Br+dr[e],Cc=Bc+dc[e],Dr=Br+dr[f],Dc=Bc+dc[f];
								if(outcheck(Cr,Cc)||outcheck(Dr,Dc)){continue;}
								if(grid_act[Cr][Cc]!=0||grid_act[Dr][Dc]!=0||plan[Cr][Cc]!=0){continue;}
								do_move(mt(Br,Bc,Dr,Dc));
								do_move(mt(Ar,Ac,Br,Bc));
								do_move(mt(Br,Bc,Cr,Cc));
								do_move(mt(Dr,Dc,Br,Bc));
								dekita=true;
								goto YATTA;
							}
						}
					}
					YATTA:
					if(!dekita){return false;}
				}
			}
		}



		//cerr<<"de"<<__LINE__<<endl;
		//cerr<<"yokeru="<<yokeru<<"\n";
		//動かすものを動かす

		for(auto it:cp_road){
			if(!do_move(it)){
				//cerr<<"de"<<__LINE__<<endl;
				return false;
			}
		}
		//cerr<<"de"<<__LINE__<<endl;
		//最後に配線を張る
		for(i=0;i<600;i++){
			if(connect[i][0]!=-1){
				j=connect[i][0];
				movingY.push_back(mt(cp_ato[i].fir,cp_ato[i].sec,cp_ato[j].fir,cp_ato[j].sec));
			}
			if(connect[i][1]!=-1){
				j=connect[i][1];
				movingY.push_back(mt(cp_ato[i].fir,cp_ato[i].sec,cp_ato[j].fir,cp_ato[j].sec));
			}
			
		}
		return true;
		
		
	}

	bool actplan(void){
		bool ret=actplan_do();
		for(int i=0;i<n;i++){
			for(int j=0;j<n;j++){
				plan[i][j]&=1023;
			}
		}
		//if(!ret){cerr<<"de"<<__LINE__<<endl;}
		return ret;
	}
	
	
	void make_sycost(void){
		//gridの情報からsycostを算出する
		int h,i,j;
		for(h=0;h<600;h++){
			if(cp_mae[h].fir==-1){continue;}
			int sr,sc;
			tie(sr,sc)=cp_mae[h];
			sycost[h][4][4]=0;
			for(i=1;i<=4;i++){
				if(outcheck(sr+i,sc)){sycost[h][i+4][4]=100;}
				else{sycost[h][i+4][4]=sycost[h][i+3][4]+1;}
				
				if(grid[sr+i][sc]>0){sycost[h][i+4][4]++;}
			}
			for(i=-1;i>=-4;i--){
				if(outcheck(sr+i,sc)){sycost[h][i+4][4]=100;}
				else{sycost[h][i+4][4]=sycost[h][i+5][4]+1;}
				
				if(grid[sr+i][sc]>0){sycost[h][i+4][4]++;}
			}
			for(i=1;i<=4;i++){
				if(outcheck(sr,sc+i)){sycost[h][4][i+4]=100;}
				else{sycost[h][4][i+4]=sycost[h][4][i+3]+1;}
				
				if(grid[sr][sc+i]>0){sycost[h][4][i+4]++;}
			}
			for(i=-1;i>=-4;i--){
				if(outcheck(sr,sc+i)){sycost[h][4][i+4]=100;}
				else{sycost[h][4][i+4]=sycost[h][4][i+5]+1;}
				
				if(grid[sr][sc+i]>0){sycost[h][4][i+4]++;}
			}
			
			for(i=1;i<=4;i++){
				for(j=1;j<=4;j++){
					if(outcheck(sr+i,sc+j)){sycost[h][i+4][j+4]=100;}
					else{sycost[h][i+4][j+4]=min(sycost[h][i+3][j+4],sycost[h][i+4][j+3])+1;}
					
					if(grid[sr+i][sc+j]>0){sycost[h][i+4][j+4]++;}
				}
				for(j=-1;j>=-4;j--){
					if(outcheck(sr+i,sc+j)){sycost[h][i+4][j+4]=100;}
					else{sycost[h][i+4][j+4]=min(sycost[h][i+3][j+4],sycost[h][i+4][j+5])+1;}
					
					if(grid[sr+i][sc+j]>0){sycost[h][i+4][j+4]++;}
				}
			}
			for(i=-1;i>=-4;i--){
				for(j=1;j<=4;j++){
					if(outcheck(sr+i,sc+j)){sycost[h][i+4][j+4]=100;}
					else{sycost[h][i+4][j+4]=min(sycost[h][i+5][j+4],sycost[h][i+4][j+3])+1;}
					
					if(grid[sr+i][sc+j]>0){sycost[h][i+4][j+4]++;}
				}
				for(j=-1;j>=-4;j--){
					if(outcheck(sr+i,sc+j)){sycost[h][i+4][j+4]=100;}
					else{sycost[h][i+4][j+4]=min(sycost[h][i+5][j+4],sycost[h][i+4][j+5])+1;}
					
					if(grid[sr+i][sc+j]>0){sycost[h][i+4][j+4]++;}
				}
			}
			
		}
	}

	//次にやること: Zcostに色と同じ線のコストを含まないようにする

	void solve(double soltime=500.0,bool singlecolor=true,int divnum=8){
		int i,j,h,k,Cskyo=9999,Csr=0,Csc=0;
		double funstarttime=myTime(); 

		for(i=0;i<600;i++){
			connect[i][0]=-1;
			connect[i][1]=-1;
			connect[i][2]=-1;
			connect[i][3]=-1;
			cp_ato[i]=mp(-1,-1);
		}
		for(i=0;i<n;i++){
			for(j=0;j<n;j++){
				plan[i][j]=0;
			}
		}

		make_sycost();
		for(i=0;i<6;i++){Ckaz[i]=0;}//各クラスタの数

		int Zcost=0;//暫定行動回数

		if(singlecolor){
			for(i=0;i<n;i++){
				for(j=0;j<n;j++){
					if((grid[i][j]/100)!=Cu){continue;}
					if(chmin(Cskyo,max(i,n-i)+max(j,n-j))){Csr=i;Csc=j;}
				}
			}
			//planをいい感じに変更していく
			
			plan[Csr][Csc]=grid[Csr][Csc];
			cp_ato[grid[Csr][Csc]]=mp(Csr,Csc);
			Ckaz[Cu]=1;
		}else{
			for(int uv=0;uv<2;uv++){
				array<int,4> Usen;//横線優先順位
				int Ca;
				if(uv==0){Usen={1,0,2,3};Ca=Cu;}
				else{Usen={n-2,n-1,n-3,n-4};Ca=Cv;}
				
				int maet=-1,mj=-1;
				for(j=0;j<n;j++){
					for(auto it:Usen){
						if((grid[it][j]/100)!=Ca){continue;}
						int gt=grid[it][j];
						plan[Usen[0]][j]=gt;
						cp_ato[gt]=mp(Usen[0],j);
						Ckaz[Ca]++;
						if(maet!=-1){
							connect[maet][3]=gt;
							connect[gt][1]=maet;
							for(k=mj+1;k<j;k++){plan[Usen[0]][k]=Ca+8;}
						}
						maet=gt;
						mj=j;
						break;
					}
				}
			}
			//そこに線があるもののうち、順にやる
			int maeS=-1;

			for(i=0;i<divnum;i++){
				int Ca,La,Lb;
				if(i%2==0){Ca=Cu;La=1;Lb=n-2;}
				else{Ca=Cv;La=n-2;Lb=1;}
				double Cpot=n*(i+0.5)/divnum;
				tuple<double,int,int> sai=mt(9999.9,-1,-1);
				for(j=maeS+2;j<min(n-1,(int)Cpot+10);j++){
					//線の元がないなら、持ってくる必要がある
					pair<int,int>senM=mp(9999,-1);
					for(int senA=j;senA>maeS;senA--){
						if((plan[La][senA]/100)!=Ca){continue;}//そこに線がないのはダメ
						chmin(senM,mp(abs(senA-j),senA));
						break;
					}
					for(int senA=j;senA<n;senA++){
						if((plan[La][senA]/100)!=Ca){continue;}//そこに線がないのはダメ
						chmin(senM,mp(abs(senA-j),senA));
						break;
					}
					int Tmoto=senM.sec;//jにする場合、Tmotoから持ってくる
					if(Tmoto==-1){continue;}

					double Dcost=abs(Cpot-j)*1.8+abs(Tmoto-j);
					for(k=2;k<=n-3;k++){//LaとLbより算出
						if((grid[k][j]/100)==Ca){Dcost-=1.5;}
						else if(grid[k][j]!=0){Dcost+=0.6;}
					}
					chmin(sai,mt(Dcost,j,Tmoto));
				}

				double Dcost;int Sc,Tmoto;
				
				tie(Dcost,Sc,Tmoto)=sai;
				if(Sc<=0||n-1<=Sc){continue;}//端に集めたくはない
				int Tt=plan[La][Tmoto];
				plan[La][Tmoto]=Ca+8;
				if(connect[Tt][1]==-1){
					if(Tmoto<Sc){
						for(j=Tmoto;j<Sc;j++){plan[La][j]=0;}
					}
					if(Sc<Tmoto){
						for(j=Sc+1;j<=Tmoto;j++){plan[La][j]=Ca+8;}
					}
				}
				if(connect[Tt][3]==-1){
					if(Tmoto<Sc){
						for(j=Tmoto;j<Sc;j++){plan[La][j]=Ca+8;}
					}
					if(Sc<Tmoto){
						for(j=Sc+1;j<=Tmoto;j++){plan[La][j]=0;}
					}
				}
				plan[La][Sc]=Tt;
				cp_ato[Tt]=mp(La,Sc);

				//そこに線を集める Ttが前の上のやつ
				if(i%2==0){
					for(k=La+3;k<Lb;k++){
						int At;
						if((grid[k][Sc]/100)==Ca){At=grid[k][Sc];}
						else if((grid[k][Sc+1]/100)==Ca){At=grid[k][Sc+1];}
						else if((grid[k][Sc-1]/100)==Ca){At=grid[k][Sc-1];}
						else{continue;}
						plan[k][Sc]=At;
						cp_ato[At]=mp(k,Sc);
						connect[Tt][2]=At;
						connect[At][0]=Tt;
						Ckaz[Ca]++;
						for(int y=cp_ato[Tt].fir+1;y<k;y++){plan[y][Sc]=Ca;}
						Tt=At;
					}
				}else{
					for(k=La-3;k>Lb;k--){
						int At;
						if((grid[k][Sc]/100)==Ca){At=grid[k][Sc];}
						else if((grid[k][Sc+1]/100)==Ca){At=grid[k][Sc+1];}
						else if((grid[k][Sc-1]/100)==Ca){At=grid[k][Sc-1];}
						else{continue;}
						plan[k][Sc]=At;
						cp_ato[At]=mp(k,Sc);
						connect[Tt][0]=At;
						connect[At][2]=Tt;
						Ckaz[Ca]++;
						for(int y=k+1;y<cp_ato[Tt].fir;y++){plan[y][Sc]=Ca;}
						Tt=At;
					}
				}
				maeS=Sc;
			}

			for(i=0;i<600;i++){
				if(cp_ato[i].fir!=-1){
					Zcost+=sycost[i][cp_ato[i].fir-cp_mae[i].fir+4][cp_ato[i].sec-cp_mae[i].sec+4];
				}
			}
			for(i=0;i<n;i++){
				for(j=0;j<n;j++){
					if(0<plan[i][j]&&plan[i][j]<=16){
						if(grid[i][j]!=0&&grid[i][j]/100!=plan[i][j]%8){Zcost++;}
					}
				}
			}
		}
		//初期状態完了

		//for debug

		double Mretu[6]={0,0,0.9,0.85,0.8,0.75};
		double Mrat=Mretu[K];//基準にする スコア/行動 の比    Mratは最初だけ高い
		
		
		
		double ondo=1.0;
		int solsco=0;
		double timeP=0.0;
		bool actstart=false;
		int actkai=0;
		for(int kai=0;true;kai++){
			if(kai%128==0){
				if(kai%512==0){timeP=(myTime()-funstarttime)/soltime;}
				if(timeP>=1.0){
					//cerr<<"Pkai="<<kai<<"time per="<<(actkai/1.5)/(kai-actkai)<<"\n";
					break;
				}
				
				bool perfect=false;
				if(singlecolor&&Ckaz[Cu]==100){perfect=true;}
				if((!singlecolor)&&Ckaz[Cu]+Ckaz[Cv]==200){perfect=true;}
				if(!actstart&&timeP>0.6){
					//cerr<<"Pkai="<<kai<<"\n";
					actkai=kai;
					actstart=true;
				}
				if(kai%(128*3*K)==0&&timeP>0.3&&(perfect || timeP>0.6)){
					if(actplan()){
						Xcost*=0.70;
						Xcost+=(((int)movingX.size())-Zcost)*0.30;
						int nowsco;
						if(singlecolor){nowsco=(Ckaz[Cu]-1)*Ckaz[Cu]/2;}
						else{nowsco=(Ckaz[Cu]-1)*Ckaz[Cu]/2+(Ckaz[Cv]-1)*Ckaz[Cv]/2;}
						//if(solsco==0&&movingX.size()+movingY.size()>100*K){cerr<<"de"<<__LINE__<<endl;}
						if(movingX.size()+movingY.size()<=100*K){
							if(chmax(maxsco,nowsco)){
								MmovingX=movingX;
								MmovingY=movingY;
							}
							chmax(solsco,nowsco);
							//cerr<<"nowsco="<<nowsco<<endl;
						}
						
					}else{
						Xcost*=0.95;
						Xcost+=80*K*0.05;
					}
				}
				
				ondo=1.0*(1-timeP)*(1-timeP);
				
				//Mrat変更
				double sa;
				double hiku=0;
				
				if(singlecolor){sa=100*K-(Zcost+Xcost+Ckaz[Cu]);}
				else{sa=100*K-(Zcost+Xcost+Ckaz[Cu]+Ckaz[Cv]);}

				if(sa<50.0||timeP>0.5){
					if(sa>50.0){sa=50.0;}
					if(sa<-50.0){sa=-50.0;}
					if(timeP>0.7&&solsco==0){sa-=70*(timeP-0.6);}
					Mrat*=1-sa*0.002;
				}

				if(kai%(4096*32)==0){
					
					//cerr<<"kai="<<kai<<"Mrat="<<Mrat<<"\tZc="<<Zcost<<"\tCk="<<Ckaz[Cu]+Ckaz[Cv]<<"\tsa="<<sa<<"\n";
					//cerr<<"timeP="<<timeP<<"\n";
					
					//if(singlecolor){cerr<<"Ckaz="<<Ckaz[Cu]<<"\n";}
					//else{cerr<<"Ckaz="<<Ckaz[Cu]<<" "<<Ckaz[Cv]<<"\n";}
					/*
					for(i=0;i<n;i++){
						for(j=0;j<n;j++){
							if(plan[i][j]==0){cout<<" ";}
							else if(plan[i][j]<8){cout<<"|";}
							else if(plan[i][j]<16){cout<<"-";}
							else{cout<<plan[i][j]/100;}
						}
						cout<<"\n";
					}
					
					int tZ=0;
					for(i=0;i<600;i++){
						if(cp_ato[i].fir==-1){continue;}
						tZ+=sycost[i][cp_ato[i].fir-cp_mae[i].fir+4][cp_ato[i].sec-cp_mae[i].sec+4];
					}
					for(i=0;i<n;i++){
						for(j=0;j<n;j++){
							if(plan[i][j]>0&&plan[i][j]<16&&grid[i][j]!=0&&plan[i][j]%8!=grid[i][j]/100){
								tZ++;
							}
						}
					}
					cerr<<Zcost<<" "<<tZ<<"\n";
					*/
					
				}

				//一定の基準で、actplanを実行。スコア次第で、更新をする
				
				
				
			}

			
				
			int Ca;
			if(singlecolor||engine()%2==0){Ca=Cu;}
			else{Ca=Cv;}
			int t=engine()%100+Ca*100;
			//ランダムに対象を選ぶ、
			int patan=engine()%9;
			if(4<=patan){
				int zisu=0;
				if(connect[t][0]!=-1){zisu++;}
				if(connect[t][1]!=-1){zisu++;}
				if(connect[t][2]!=-1){zisu++;}
				if(connect[t][3]!=-1){zisu++;}


				if(zisu==1&&Ckaz[Ca]>1){
					//それが葉ならそれを取り除く(強制効果)
					Zcost-=sycost[t][4+cp_ato[t].fir-cp_mae[t].fir][4+cp_ato[t].sec-cp_mae[t].sec];
					for(int d=0;d<4;d++){
						if(connect[t][d]==-1){continue;}
						int u=connect[t][d];
						connect[t][d]=-1;
						connect[u][d^2]=-1;
						int sr=cp_ato[t].fir,sc=cp_ato[t].sec;
						while(-1){
							sr+=dr[d];sc+=dc[d];
							if(outcheck(sr,sc)){/*cerr<<"bug"<<__LINE__<<"\n";*/break;}
							if(cp_ato[u]==mp(sr,sc)){break;}
							if(grid[sr][sc]!=0&&grid[sr][sc]/100!=Ca){Zcost--;}
							plan[sr][sc]=0;
						}
						
					}
					plan[cp_ato[t].fir][cp_ato[t].sec]=0;
					cp_ato[t]=mp(-1,-1);
					Ckaz[Ca]--;
				
				}
				else if(connect[t][0]!=-1&&connect[t][2]!=-1&&zisu==2&&Ckaz[Ca]>1){
					Zcost-=sycost[t][4+cp_ato[t].fir-cp_mae[t].fir][4+cp_ato[t].sec-cp_mae[t].sec];
					if(grid[cp_ato[t].fir][cp_ato[t].sec]>0&&grid[cp_ato[t].fir][cp_ato[t].sec]/100!=Ca){Zcost++;}
					connect[connect[t][0]][2]=connect[t][2];
					connect[connect[t][2]][0]=connect[t][0];
					connect[t][0]=-1;
					connect[t][2]=-1;
					plan[cp_ato[t].fir][cp_ato[t].sec]=Ca;
					cp_ato[t]=mp(-1,-1);
					Ckaz[Ca]--;
				}else if(connect[t][1]!=-1&&connect[t][3]!=-1&&zisu==2&&Ckaz[Ca]>1){
					Zcost-=sycost[t][4+cp_ato[t].fir-cp_mae[t].fir][4+cp_ato[t].sec-cp_mae[t].sec];
					if(grid[cp_ato[t].fir][cp_ato[t].sec]>0&&grid[cp_ato[t].fir][cp_ato[t].sec]/100!=Ca){Zcost++;}
					connect[connect[t][1]][3]=connect[t][3];
					connect[connect[t][3]][1]=connect[t][1];
					connect[t][1]=-1;
					connect[t][3]=-1;
					plan[cp_ato[t].fir][cp_ato[t].sec]=Ca+8;
					cp_ato[t]=mp(-1,-1);
					Ckaz[Ca]--;
				}
				else if(zisu>0){continue;}
				

				//その後、 周りの中から1つを選択して、接続する
				tuple<double,int,int,int,int,int,int>kouho;
				//増減するスコア,コスト,つながるターゲットA,ターゲットB、 新r,c,d
				//縦ダブル=d=4 横ダブル=d=5
				kouho=mt(-log(dist(engine))*ondo,0,0,0,0,0,0);

				//左右に動いて上につなぐ
				for(int gc=max(0,cp_mae[t].sec-4);gc<=min(n-1,cp_mae[t].sec+4);gc++){
					int gr=cp_mae[t].fir-1;
					int ncost=sycost[t][4][gc-cp_mae[t].sec+4];
					if(plan[cp_mae[t].fir][gc]>0){continue;}//先客がいる
					while(0<=gr){
						if((plan[gr][gc]/100)==Ca){
							//完成
							double itsco=1/Mrat-ncost*Mrat-log(dist(engine))*ondo;
							chmax(kouho,mt(itsco,ncost,plan[gr][gc],-1,cp_mae[t].fir,gc,0));
							break;
						}else if(plan[gr][gc]!=0){break;}//不成立

						if(grid[gr][gc]!=0&&grid[gr][gc]/100!=Ca){ncost++;}
						gr--;
					}
				}

				//左右に動いて下につなぐ
				for(int gc=max(0,cp_mae[t].sec-4);gc<=min(n-1,cp_mae[t].sec+4);gc++){
					int gr=cp_mae[t].fir+1;
					int ncost=sycost[t][4][gc-cp_mae[t].sec+4];
					if(plan[cp_mae[t].fir][gc]>0){continue;}//先客がいる
					while(gr<n){
						if((plan[gr][gc]/100)==Ca){
							//完成
							double itsco=1/Mrat-ncost*Mrat-log(dist(engine))*ondo;
							chmax(kouho,mt(itsco,ncost,plan[gr][gc],-1,cp_mae[t].fir,gc,2));
							break;
						}else if(plan[gr][gc]!=0){break;}//不成立
						if(grid[gr][gc]!=0&&grid[gr][gc]/100!=Ca){ncost++;}
						gr++;
					}
				}

				//左右に動いて上下の線に入る
				
				for(int gc=max(0,cp_mae[t].sec-4);gc<=min(n-1,cp_mae[t].sec+4);gc++){
					if(plan[cp_mae[t].fir][gc]==Ca){//対象の縦線
						int gr=cp_mae[t].fir-1;
						int Ut=-1,Dt=-1;
						while(0<=gr){
							if((plan[gr][gc]/100)==Ca){Ut=plan[gr][gc];break;}//完成
							gr--;
						}

						gr=cp_mae[t].fir+1;
						while(gr<n){
							if((plan[gr][gc]/100)==Ca){Dt=plan[gr][gc];break;}//完成
							gr++;
						}
						if(Ut!=-1&&Dt!=-1){
							int ncost=sycost[t][4][gc-cp_mae[t].sec+4];
							if(grid[cp_mae[t].fir][gc]>0&&grid[cp_mae[t].fir][gc]/100!=Ca){ncost--;}
							double itsco=1/Mrat-ncost*Mrat-log(dist(engine))*ondo;
							chmax(kouho,mt(itsco,ncost,Ut,Dt,cp_mae[t].fir,gc,4));

						}else{
							//cerr<<"bug"<<__LINE__<<Ut<<" "<<cp_mae[t].fir<<" "<<gc<<"\n";
						}
					}
				}

				//上下に動いて左につなぐ
				for(int gr=max(0,cp_mae[t].fir-4);gr<=min(n-1,cp_mae[t].fir+4);gr++){
					int gc=cp_mae[t].sec-1;
					int ncost=sycost[t][gr-cp_mae[t].fir+4][4];
					if(plan[gr][cp_mae[t].sec]>0){continue;}//先客がいる
					while(0<=gc){
						if((plan[gr][gc]/100)==Ca){
							//完成
							double itsco=1/Mrat-ncost*Mrat-log(dist(engine))*ondo;
							chmax(kouho,mt(itsco,ncost,plan[gr][gc],-1,gr,cp_mae[t].sec,1));
							break;
						}else if(plan[gr][gc]!=0){break;}//不成立
						if(grid[gr][gc]!=0&&grid[gr][gc]/100!=Ca){ncost++;}
						gc--;
					}
				}
				//上下に動いて右につなぐ
				for(int gr=max(0,cp_mae[t].fir-4);gr<=min(n-1,cp_mae[t].fir+4);gr++){
					int gc=cp_mae[t].sec+1;
					int ncost=sycost[t][gr-cp_mae[t].fir+4][4];
					if(plan[gr][cp_mae[t].sec]>0){continue;}//先客がいる
					while(gc<n){
						if((plan[gr][gc]/100)==Ca){
							//完成
							double itsco=1/Mrat-ncost*Mrat-log(dist(engine))*ondo;
							chmax(kouho,mt(itsco,ncost,plan[gr][gc],-1,gr,cp_mae[t].sec,3));
							break;
						}else if(plan[gr][gc]!=0){break;}//不成立
						if(grid[gr][gc]!=0&&grid[gr][gc]/100!=Ca){ncost++;}
						gc++;
					}
				}
				
				//上下に動いて左右の線に入る
				
				for(int gr=max(0,cp_mae[t].fir-4);gr<=min(n-1,cp_mae[t].fir+4);gr++){
					if(plan[gr][cp_mae[t].sec]==Ca+8){//対象の縦線
						int gc=cp_mae[t].sec-1;
						int Lt=-1,Rt=-1;
						while(0<=gc){
							if((plan[gr][gc]/100)==Ca){Lt=plan[gr][gc];break;}//完成
							gc--;
						}

						gc=cp_mae[t].sec+1;
						while(gc<n){
							if((plan[gr][gc]/100)==Ca){Rt=plan[gr][gc];break;}//完成
							gc++;
						}
						if(Lt!=-1&&Rt!=-1){
							int ncost=sycost[t][gr-cp_mae[t].fir+4][4];
							if(grid[gr][cp_mae[t].sec]>0&&grid[gr][cp_mae[t].sec]/100!=Ca){ncost--;}
							double itsco=1/Mrat-ncost*Mrat-log(dist(engine))*ondo;
							chmax(kouho,mt(itsco,ncost,Lt,Rt,gr,cp_mae[t].sec,5));

						}
					}
				}
				

				if(get<2>(kouho)==0){continue;}
				
				{
					Zcost+=get<1>(kouho);
					int u=get<2>(kouho),v=get<3>(kouho),gr=get<4>(kouho),gc=get<5>(kouho),gd=get<6>(kouho);

					plan[gr][gc]=t;
					cp_ato[t]=mp(gr,gc);
					Ckaz[Ca]++;

					if(gd<4){
						connect[t][gd]=u;
						connect[u][gd^2]=t;
						gr+=dr[gd];gc+=dc[gd];
						while(-1){
							if(outcheck(gr,gc)){cerr<<"bug"<<__LINE__<<"\n";break;}
							if(plan[gr][gc]==u){break;}
							if(plan[gr][gc]!=0){cerr<<"bug"<<__LINE__<<"plan="<<plan[gr][gc]<<"u="<<u<<"t="<<t<<"\n";break;}
							if(gd==0||gd==2){plan[gr][gc]|=Ca;}
							else{plan[gr][gc]|=Ca+8;}
							gr+=dr[gd];gc+=dc[gd];
						}
					}else if(gd==4){//割り込んだだけなのでplanの張替えはなし
						connect[t][0]=u;
						connect[u][2]=t;
						connect[t][2]=v;
						connect[v][0]=t;
					}else if(gd==5){//割り込んだだけなのでplanの張替えはなし
						connect[t][1]=u;
						connect[u][3]=t;
						connect[t][3]=v;
						connect[v][1]=t;
					}
					
				}
				
			}else if(patan==0||patan==1){
				//横線の位置をずらす
				if(connect[t][1]!=-1||connect[t][3]==-1){continue;}
				//対象が左端であることを確認
				int L=cp_ato[t].sec;
				vector<int>team;
				team.pub(t);
				int st=t;
				while(connect[st][3]!=-1){
					st=connect[st][3];
					team.pub(st);
				}
				int R=cp_ato[st].sec;
				int ar=cp_ato[t].fir;
				if(patan==0&&ar>0){//上ずらし
					bool can=true;
					for(j=L;j<=R;j++){
						if(plan[ar-1][j]!=0&&plan[ar-1][j]!=Ca){can=false;break;}
					}
					if(!can){continue;}
					int costpla=0;
					for(auto it:team){
						if(cp_ato[it].fir-cp_mae[it].fir+4-1<0){can=false;break;}
						costpla-=sycost[it][cp_ato[it].fir-cp_mae[it].fir+4  ][cp_ato[it].sec-cp_mae[it].sec+4];
						costpla+=sycost[it][cp_ato[it].fir-cp_mae[it].fir+4-1][cp_ato[it].sec-cp_mae[it].sec+4];
					}
					if(!can){continue;}
					for(j=L;j<=R;j++){
						if(plan[ar][j]==Ca+8){//ない
							if(grid[ar][j]!=0&&grid[ar][j]/100!=Ca){costpla--;}
							if(grid[ar-1][j]!=0&&grid[ar-1][j]/100!=Ca){costpla++;}
						}else{
							if(connect[plan[ar][j]][2]!=-1 && grid[ar][j]!=0&&grid[ar][j]/100!=Ca){costpla++;}
							if(connect[plan[ar][j]][0]!=-1 && grid[ar-1][j]!=0&&grid[ar-1][j]/100!=Ca){costpla--;}
						}
					}
					if(costpla*Mrat<-log(dist(engine))*ondo){
						//移動させます
						for(j=L;j<=R;j++){
							if(plan[ar][j]==Ca+8){
								plan[ar][j]=0;
								plan[ar-1][j]=Ca+8;
							}else{
								plan[ar-1][j]=plan[ar][j];
								if(connect[plan[ar][j]][2]!=-1){plan[ar][j]=Ca;}
								else{plan[ar][j]=0;}
							}
						}
						for(auto it:team){cp_ato[it].fir--;}
						Zcost+=costpla;
						
					}
				}else if(patan==1&&ar+1<n){
					bool can=true;
					for(j=L;j<=R;j++){
						if(plan[ar+1][j]!=0&&plan[ar+1][j]!=Ca){can=false;break;}
					}
					if(!can){continue;}
					int costpla=0;
					for(auto it:team){
						if(cp_ato[it].fir-cp_mae[it].fir+4+1>=8){can=false;break;}
						costpla-=sycost[it][cp_ato[it].fir-cp_mae[it].fir+4  ][cp_ato[it].sec-cp_mae[it].sec+4];
						costpla+=sycost[it][cp_ato[it].fir-cp_mae[it].fir+4+1][cp_ato[it].sec-cp_mae[it].sec+4];
					}
					if(!can){continue;}
					for(j=L;j<=R;j++){
						if(plan[ar][j]==Ca+8){//ない
							if(grid[ar][j]!=0&&grid[ar][j]/100!=Ca){costpla--;}
							if(grid[ar+1][j]!=0&&grid[ar+1][j]/100!=Ca){costpla++;}
						}else{
							if(connect[plan[ar][j]][0]!=-1 && grid[ar][j]!=0&&grid[ar][j]/100!=Ca){costpla++;}
							if(connect[plan[ar][j]][2]!=-1 && grid[ar+1][j]!=0&&grid[ar+1][j]/100!=Ca){costpla--;}
						}
					}
					if(costpla*Mrat<-log(dist(engine))*ondo){
						//移動させます
						for(j=L;j<=R;j++){
							if(plan[ar][j]==Ca+8){
								plan[ar][j]=0;
								plan[ar+1][j]=Ca+8;
							}else{
								plan[ar+1][j]=plan[ar][j];
								if(connect[plan[ar][j]][0]!=-1){plan[ar][j]=Ca;}
								else{plan[ar][j]=0;}
							}
						}
						for(auto it:team){cp_ato[it].fir++;}
						Zcost+=costpla;
						
					}

				}
			}else{
				if(connect[t][0]!=-1||connect[t][2]==-1){continue;}
				//対象が左端であることを確認
				int U=cp_ato[t].fir;
				vector<int>team;
				team.pub(t);
				int st=t;
				while(connect[st][2]!=-1){
					st=connect[st][2];
					team.pub(st);
				}
				int D=cp_ato[st].fir;
				int ac=cp_ato[t].sec;
				if(patan==2&&ac>0){//上ずらし
					bool can=true;
					for(j=U;j<=D;j++){
						if(plan[j][ac-1]!=0&&plan[j][ac-1]!=Ca+8){can=false;break;}
					}
					if(!can){continue;}
					int costpla=0;
					for(auto it:team){
						if(cp_ato[it].sec-cp_mae[it].sec+4-1<0){can=false;break;}
						costpla-=sycost[it][cp_ato[it].fir-cp_mae[it].fir+4][cp_ato[it].sec-cp_mae[it].sec+4  ];
						costpla+=sycost[it][cp_ato[it].fir-cp_mae[it].fir+4][cp_ato[it].sec-cp_mae[it].sec+4-1];
					}
					if(!can){continue;}
					for(j=U;j<=D;j++){
						if(plan[j][ac]==Ca){//ない
							if(grid[j][ac]!=0&&grid[j][ac]/100!=Ca){costpla--;}
							if(grid[j][ac-1]!=0&&grid[j][ac-1]/100!=Ca){costpla++;}
						}else{
							if(connect[plan[j][ac]][3]!=-1 && grid[j][ac]!=0&&grid[j][ac]/100!=Ca){costpla++;}
							if(connect[plan[j][ac]][1]!=-1 && grid[j][ac-1]!=0&&grid[j][ac-1]/100!=Ca){costpla--;}
						}
					}
					if(costpla*Mrat<-log(dist(engine))*ondo){
						//移動させます
						for(j=U;j<=D;j++){
							if(plan[j][ac]==Ca){
								plan[j][ac]=0;
								plan[j][ac-1]=Ca;
							}else{
								plan[j][ac-1]=plan[j][ac];
								if(connect[plan[j][ac]][3]!=-1){plan[j][ac]=Ca+8;}
								else{plan[j][ac]=0;}
							}
						}
						for(auto it:team){cp_ato[it].sec--;}
						Zcost+=costpla;
					}
				}else if(patan==3&&ac+1<n){
					bool can=true;
					for(j=U;j<=D;j++){
						if(plan[j][ac+1]!=0&&plan[j][ac+1]!=Ca+8){can=false;break;}
					}
					if(!can){continue;}
					int costpla=0;
					for(auto it:team){
						if(cp_ato[it].sec-cp_mae[it].sec+4+1>=8){can=false;break;}
						costpla-=sycost[it][cp_ato[it].fir-cp_mae[it].fir+4][cp_ato[it].sec-cp_mae[it].sec+4  ];
						costpla+=sycost[it][cp_ato[it].fir-cp_mae[it].fir+4][cp_ato[it].sec-cp_mae[it].sec+4+1];
					}
					if(!can){continue;}
					for(j=U;j<=D;j++){
						if(plan[j][ac]==Ca){//ない
							if(grid[j][ac]!=0&&grid[j][ac]/100!=Ca){costpla--;}
							if(grid[j][ac+1]!=0&&grid[j][ac+1]/100!=Ca){costpla++;}
						}else{
							if(connect[plan[j][ac]][1]!=-1 && grid[j][ac]!=0&&grid[j][ac]/100!=Ca){costpla++;}
							if(connect[plan[j][ac]][3]!=-1 && grid[j][ac+1]!=0&&grid[j][ac+1]/100!=Ca){costpla--;}
						}
					}
					if(costpla*Mrat<-log(dist(engine))*ondo){
						//移動させます
						for(j=U;j<=D;j++){
							if(plan[j][ac]==Ca){
								plan[j][ac]=0;
								plan[j][ac+1]=Ca;
							}else{
								plan[j][ac+1]=plan[j][ac];
								if(connect[plan[j][ac]][1]!=-1){plan[j][ac]=Ca+8;}
								else{plan[j][ac]=0;}
							}
						}
						for(auto it:team){cp_ato[it].sec++;}
						Zcost+=costpla;
					}

				}

			}

			
		}
		//cerr<<"single"<<singlecolor<<" solsco="<<solsco<<"\n";

	}

	void input(void){
		dist=uniform_real_distribution<double>(0.0,1.0);
		cin>>n>>K;
		int i,j;
		for(i=0;i<600;i++){
			cp_mae[i]=mp(-1,-1);
		}
		for(i=0;i<6;i++){Ckaz[i]=0;}
		for(i=0;i<n;i++){
			string s;cin>>s;
			for(j=0;j<n;j++){
				plan[i][j]=0;
				int c=s[j]-'0';
				if(c>0){
					cp_mae[(c*100)+Ckaz[c]]=mp(i,j);
					grid[i][j]=(c*100)+Ckaz[c];
					Ckaz[c]++;
				}else{
					grid[i][j]=0;
				}
			}
		}
		for(i=0;i<6;i++){Ckaz[i]=0;}
		Xcost=min(10.0*K*n*n/(n*n-100*K)-K*5.0,K*60.0);
	
	}

	void output_ans(void){
		cout<<MmovingX.size()<<"\n";
		//cerr<<"Xsiz="<<MmovingX.size()<<"\n";
		for(auto it:MmovingX){
			int a,b,c,d;tie(a,b,c,d)=it;
			cout<<a<<" "<<b<<" "<<c<<" "<<d<<"\n";
		}
		cout<<MmovingY.size()<<"\n";
		//cerr<<"Ysiz="<<MmovingY.size()<<"\n";
		for(auto it:MmovingY){
			int a,b,c,d;tie(a,b,c,d)=it;
			if(a!=c&&b!=d){cerr<<"bug"<<"\n";}
			cout<<a<<" "<<b<<" "<<c<<" "<<d<<"\n";
		}
	}
};
int main(void){
	
	cin.tie(0);ios::sync_with_stdio(false);
	Servers sav;
	sav.input();
	starttime=getTime();
	//cerr<<"de"<<__LINE__<<"\n";
	
	int diva=max(min(6,sav.n/5-1),2);
	
	
	/*
	for(int i=0;i<3;i++){
		if(i==2&&sav.maxsco<4950){break;}
		sav.Cu=1+i%sav.K;sav.Cv=1+(i+2+(i/sav.K))%sav.K;
		if(sav.Cu==sav.Cv){sav.Cv=1+(sav.Cu)%sav.K;}
		double Stime=200;
		if(sav.maxsco<9900){sav.solve(Stime,false,diva+i);}
	}
	if(sav.maxsco<4950){
		for(int i=0;i<4;i++){
			double Stime=50;
			sav.Cu=1+i%sav.K;sav.Cv=-1;
			if(sav.maxsco<4950){sav.solve(Stime,true,0);}
		}
	}
	*/
	
	for(int i=0;i<6;i++){
		if(i==4&&sav.maxsco<4950){break;}
		sav.Cu=1+i%sav.K;sav.Cv=1+(i+2+(i/sav.K))%sav.K;
		if(sav.Cu==sav.Cv){sav.Cv=1+(sav.Cu)%sav.K;}
		double Stime=480;
		if(sav.maxsco<9900){sav.solve(Stime,false,diva+i/2);}
	}
	if(sav.maxsco<4950){
		for(int i=0;i<6;i++){
			double Stime=160;
			sav.Cu=1+i%sav.K;sav.Cv=-1;
			if(sav.maxsco<4950){sav.solve(Stime,true,0);}
		}
	}
	
	

	sav.output_ans();
	
	cerr<<"K="<<sav.K<<"n="<<sav.n<<"score= "<<sav.maxsco<<"\n";

	
	
	return 0;
}
/*
225
324
441
576

	
	
K=2だと何も考えなくても結構そろう

K=2,3では1色が限界だと思う

1色にする場合、 木をうまいことやって作る

2色そろえる場合
xBBBBBBBBBBBBBBBBx
xAxxBxxAxxBBBAxxBx
xAxxBAAAAABxxAAABx
xAAABxxAxxBAAAxxBx
xAxxBBBAAABBBABBBx
xABBBxxAxxBxxAxxBx
xAABBxxAxxBxxAAABx
xAxxBBBABBBAAAxxBx
xAAAAAAAAAAAAAAxxx
のようにしたい、  だから骨だけ手動生成する

(メインの数-メイン以外の数)^2
でスコアが近似できる

最後に焼きなませるようにした方が、よいだろ

焼きなまし: 遷移は、
「葉の接続を切る」
「一つ新たに接続する」
(ここまで実装済)
「間にある頂点を外す」 (a-b-c <-> a-c の遷移)
「なんかの間に挟まる」

「横や縦の1列を取って、 ずらす」




コスト:
移動した頂点のユーグリッド距離
+移動する頂点の道の上にあるコンピューターの数
+ 配線上にあるコンピューターの数

コストが100K-使用頂点数 以内である必要があり、
グループの数^2 がスコアになる

1色なら初期状態から焼きなませばOK?

安定する、1色を先に実装する

残りやること
微妙に詰まるケースを直す
2色の実装
*/
