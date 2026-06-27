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
int xz[2001];
int yz[2001];
int dist(int a,int b){return abs(xz[a]-xz[b])+abs(yz[a]-yz[b]);}
pair<int,vector<int>> solve(double time,vector<int>zyu,int yosen=0){
	int i,j,n=1000,m=50;

	xz[n+n]=400;yz[n+n]=400;
	starttime=getTime();
	mt19937 engine(starttime);

	vector<int>ans_zyu;
	int ans_kyo=mod;
	
	vector<bool> erabu(n);
	for(i=1;i+1<zyu.size();i++){erabu[zyu[i]/2]=1;}
	std::uniform_real_distribution<> ran(0.0, 1.0);
	
	int tas_bas[2000];
	int kyo_gou[1000];
	
	//double ondo_mto=60;
	double ondo=45;
	double zi=0;
	for(int kai=0;true;kai++){
		//cerr<<"de"<<endl;
		if((kai&63)==0){
			zi=myTime()/time;
			double hi[3]={60,60,60};
			double tei[3]={0.01,0.01,0.01};
			ondo=9999;
			for(i=0;i<3;i++){chmin(ondo,(1-zi)*hi[i]+tei[i]);}
			if(zi>1){cerr<<kai<<" ";break;}
		}
		//0,00, 60
		//0.50, 35
		//0.75, 20
		//1.00,  0
		/*
		double hi[3]={80,60,50};
		double tei[3]={0,5,10};
		*/
		
		int tuika_num=10;
		if(0.90<zi){tuika_num=5;}
		if(0.95<zi){tuika_num=2;}
		if(0.98<zi){tuika_num=1;}
		if(zyu.size()<m+m+2-tuika_num/2){
			//足りないなら増やす
			//各仕事に対して、どこに増やすか検討する
			
			//足すなら、一番近いところ
			//s->tの順で追加されるならOK
			//そうでない場合、却下
			vector<double>kak;//確率
			vector<int>k_ind;
			double onwa=0;
			
			for(i=0;i<n;i++){
				pair<int,int>sbas=mp(mod,0);
				pair<int,int>tbas=mp(mod,0);
				if(erabu[i]){continue;}
				int sb=i+i;
				int tb=i+i+1;
				for(j=1;j<zyu.size();j+=6){
					chmin(sbas,mp(dist(zyu[j-1],sb)+dist(zyu[j],sb)-dist(zyu[j-1],zyu[j]),j));
					chmin(tbas,mp(dist(zyu[j-1],tb)+dist(zyu[j],tb)-dist(zyu[j-1],zyu[j]),j));
				}
				{j=zyu.size()-1;//コードブロック
					chmin(sbas,mp(dist(zyu[j-1],sb)+dist(zyu[j],sb)-dist(zyu[j-1],zyu[j]),j));
					chmin(tbas,mp(dist(zyu[j-1],tb)+dist(zyu[j],tb)-dist(zyu[j-1],zyu[j]),j));
				}
				for(j=max(1,sbas.sec-5);j<min((int)zyu.size(),sbas.sec+5);j++){
					chmin(sbas,mp(dist(zyu[j-1],sb)+dist(zyu[j],sb)-dist(zyu[j-1],zyu[j]),j));
				}
				for(j=max(1,tbas.sec-5);j<min((int)zyu.size(),tbas.sec+5);j++){
					chmin(tbas,mp(dist(zyu[j-1],tb)+dist(zyu[j],tb)-dist(zyu[j-1],zyu[j]),j));
				}
				if(sbas.sec>tbas.sec){continue;}
				double genper=exp(-(sbas.fir+tbas.fir)/ondo);
				//cerr<<genper<<endl;
				onwa+=genper;
				kak.pub(onwa);
				k_ind.pub(i);
				tas_bas[sb]=sbas.sec;
				tas_bas[tb]=tbas.sec;
			}
			
			//いちどに10つ追加する
			vector<pair<int,int>>tuika;
			
			vector<bool>zahyo(zyu.size());
			for(i=0;i<tuika_num;i++){
				int aaa=k_ind[LBI(kak,ran(engine)*onwa)];
				if(erabu[aaa]||zahyo[tas_bas[aaa+aaa]]||zahyo[tas_bas[aaa+aaa+1]]){continue;}
				erabu[aaa]=1;zahyo[tas_bas[aaa+aaa]]=1;zahyo[tas_bas[aaa+aaa+1]]=1;
				tuika.pub(mp(tas_bas[aaa+aaa],aaa+aaa));
				tuika.pub(mp(tas_bas[aaa+aaa+1],aaa+aaa+1));
			}
			SO(tuika);REV(tuika);
			for(i=0;i<tuika.size();i++){
				int stk=tuika[i].sec;
				zyu.insert(zyu.begin()+tas_bas[stk],stk);
			}
			
		}else{
			//足りるなら減らす
			
			
			//全部見る
			for(i=0;i<n;i++){kyo_gou[i]=0;}
			for(j=1;j+1<zyu.size();j++){
				int ter=zyu[j]/2;
				kyo_gou[ter]+=dist(zyu[j-1],zyu[j])+dist(zyu[j],zyu[j+1])-dist(zyu[j-1],zyu[j+1]);
				tas_bas[zyu[j]]=j;
			}
			vector<double>kak;//確率
			vector<int>k_ind;
			double onwa=0;
			for(j=1;j+1<zyu.size();j++){
				if(zyu[j]%2==1){continue;}
				int ter=zyu[j]/2;
				
				double genper=exp(kyo_gou[ter]/ondo);
				//cerr<<genper<<endl;
				onwa+=genper;
				kak.pub(onwa);
				k_ind.pub(ter);
			}
			int tuika=k_ind[LBI(kak,ran(engine)*onwa)];
			int sk=tuika+tuika;
			int tk=sk+1;
			//cerr<<"de"<<" "<<tuika<<" "<<tas_bas[sk]<<endl;
			zyu.erase(zyu.begin()+tas_bas[tk]);
			zyu.erase(zyu.begin()+tas_bas[sk]);
			erabu[tuika]=0;
		}
		
		
		if(zyu.size()==m+m+2){
			int gen_kyo=0;
			for(i=0;i+1<zyu.size();i++){gen_kyo+=dist(zyu[i],zyu[i+1]);}
			if(chmin(ans_kyo,gen_kyo)){ans_zyu=zyu;}
			if(kai%73==0){
				cout<<((int)zyu.size()-2)/2;
				for(i=0;i<n;i++){if(erabu[i]){cout<<" "<<i+1;}}
				cout<<endl;
				cout<<zyu.size();
				for(i=0;i<zyu.size();i++){cout<<" "<<xz[zyu[i]]<<" "<<yz[zyu[i]];}
				cout<<endl;
				
			}
		}
		//cerr<<ans_kyo<<endl;
		
	}
	zyu=ans_zyu;
	erabu=vector<bool>(n);
	for(i=1;i+1<zyu.size();i++){erabu[zyu[i]/2]=1;}
	
	cout<<((int)zyu.size()-2)/2;
	for(i=0;i<n;i++){if(erabu[i]){cout<<" "<<i+1;}}
	cout<<endl;
	cout<<zyu.size();
	for(i=0;i<zyu.size();i++){cout<<" "<<xz[zyu[i]]<<" "<<yz[zyu[i]];}
	cout<<endl;
	int gen_kyo=0;
	for(i=0;i+1<zyu.size();i++){gen_kyo+=dist(zyu[i],zyu[i+1]);}
	cerr<<gen_kyo<<" ";
	return mp(gen_kyo,zyu);
}
int main(void){
	cout<<fixed<<setprecision(20);cin.tie(0);ios::sync_with_stdio(false);
	int i,n=1000;
	for(i=0;i<n;i++){cin>>xz[i*2]>>yz[i*2]>>xz[i*2+1]>>yz[i*2+1];}
	
	
	int goukei=0;
	for(i=0;i<1;i++){
		vector<int>zyu={n+n,n+n};
		int kyo=solve(1950,zyu).fir;
		cerr<<kyo<<endl;
		goukei+=kyo;
	}
	cerr<<goukei<<endl;
	//多点はだめ
	//cerr<<hei_taten<<" "<<hei_one<<endl;
}
/*
基準:35700
*/