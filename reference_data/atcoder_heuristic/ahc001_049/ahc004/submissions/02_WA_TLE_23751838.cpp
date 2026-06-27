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

template< int char_size >
struct TrieNode {
  int nxt[char_size];

  int exist;
  vector< int > accept;
	vector< int > accept_child;

  TrieNode() : exist(0) {
  	for(int i=0;i<char_size;i++){nxt[i]=-1;}
  }
};

template< int char_size, int margin >
struct Trie {
	using Node = TrieNode< char_size >;

	vector< Node > nodes;
	int root;

	Trie() : root(0) {
		nodes.push_back(Node());
	}

	void update_direct(int node, int id) {
		++nodes[node].exist;
		nodes[node].accept.push_back(id);
		nodes[node].accept_child.push_back(id);
	}

	void update_child(int node, int child, int id) {
		++nodes[node].exist;
		nodes[node].accept_child.push_back(id);
	}

	void add(const string &str, int str_index, int node_index, int id) {
		if(str_index == str.size()) {
			update_direct(node_index, id);
		} else {
			const int c = str[str_index] - margin;
			if(nodes[node_index].nxt[c] == -1) {
				nodes[node_index].nxt[c] = (int) nodes.size();
				nodes.push_back(Node());
			}
			add(str, str_index + 1, nodes[node_index].nxt[c], id);
			update_child(node_index, nodes[node_index].nxt[c], id);
		}
	}

	void add(const string &str, int id) {
		add(str, 0, 0, id);
	}

	void add(const string &str) {
		add(str, nodes[0].exist);
	}

	int get_nodeidx(const string &str, int str_index, int node_index) const{
		//strで始まるnodeのindexを返す。
		//それがないなら、-1を返す
		if(str_index == str.size()) {
			return node_index;
		} else {
			const int c = str[str_index] - margin;
			if(nodes[node_index].nxt[c] == -1){ return -1;}
			return get_nodeidx(str, str_index + 1, nodes[node_index].nxt[c]);
		}
	}

	int get_nodeidx(const string &str) const {
		return get_nodeidx(str, 0, 0);
	}

	vector<int>exist;
	int count() const {
		return (nodes[0].exist);
	}
	int count(int in) const {
		return (nodes[in].exist);
	}
	int count(const string &str)const{
		int id=get_nodeidx(str);
		if(id==-1){return 0;}
		return count(id);
	}
	int size() const {
		return ((int) nodes.size());
	}
};

string lastr(const string&s,int a){return s.substr(s.size()-a);}
mt19937 engine(1333);
int decnt=0;
vector<pair<int,int>> ofset[8][512];
pair<vector<tuple<int,int,int>>,vector<tuple<int,int,int>>> toras_ans;
int max_t=0;
bool solve_toras(const vector<string>&ring,const vector<tuple<int,int,int>>&gyo,const vector<tuple<int,int,int>>&rtu,int nex_t,int pass=0){
	//ringの順に、全探索をする
	//つぎのgyoに当てはまるのを考える
	
	int n=20,s=ring.size(),nex=nex_t/2,i,j,k;
	if(nex_t/8<pass){return false;}
	if(chmax(max_t,nex_t)){toras_ans=mp(gyo,rtu);}
	decnt++;
	//if(decnt%102400==0){cerr<<"decnt="<<decnt<<"nex_t="<<nex_t<<endl;}
	if(n<=nex){toras_ans=mp(gyo,rtu);return true;}
	vector<int>use(s);
	for(auto it:gyo){use[get<0>(it)]=1;}
	for(auto it:rtu){use[get<0>(it)]=1;}
	//cerr<<"de"<<__LINE__<<" "<<nex_t<<endl;
	int bts=0;
	int ofbit=0;
	for(k=0;k<rtu.size();k++){
		int a,x,y;
		tie(a,x,y)=rtu[k];
		if(x==0){bts+=(ring[a][nex+y]-'A')*64;ofbit+=4;}
		if(x==1){bts+=(ring[a][nex+y]-'A')*8;ofbit+=2;}
		if(x==2){bts+=(ring[a][nex+y]-'A');ofbit++;}
	}
	bool aru=false;
	for(auto miru:ofset[ofbit][bts]){
		i=miru.fir;j=miru.sec;
		if(use[i]){continue;}
		int dame=0;
		int out_k=-1;
		for(k=0;k<rtu.size();k++){
			int a,x,y;
			tie(a,x,y)=rtu[k];
			
			if(ring[i][x+j]!=ring[a][nex+y]){dame++;out_k=k;}
			//許容範囲= 5つあるなら1個、10つで2個
			if(dame>min(2,((int)rtu.size())/5)){break;}
		}
		if(dame>min(2,((int)rtu.size())/5)){continue;}
		if(out_k!=-1){
			dame=0;
			int oka=get<0>(rtu[out_k]);
			int okx=get<1>(rtu[out_k]);
			int oky=get<2>(rtu[out_k]);
			for(k=0;k<gyo.size();k++){
				int a,x,y;
				tie(a,x,y)=gyo[k];
				if(ring[oka][x+oky]!=ring[a][okx+y]){dame++;}
				//許容範囲= 5つあるなら1個、10つで2個
				if(dame+1>min(2,((int)rtu.size())/5)){break;}
			}
			if(dame+1<=min(2,((int)rtu.size())/5)){out_k=-1;}
		}
		if(out_k==-1){
			aru=true;
			auto aaa=gyo;
			aaa.pub(mt(i,nex,j));
			if(solve_toras(ring,rtu,aaa,nex_t+1,pass)){return true;}
		}else{
			auto aaa=gyo;
			aaa.pub(mt(i,nex,j));
			auto bbb=rtu;
			bbb.erase(bbb.begin()+out_k);
			if(solve_toras(ring,bbb,aaa,nex_t+1,pass+1)){return true;}
		}
	}
	if((!aru)&&(nex_t+1)/8 >= pass+1){
		if(solve_toras(ring,rtu,gyo,nex_t+1,pass+1)){return true;}
	}
	return false;
}
int main(void){
	cout<<fixed<<setprecision(20);
	cin.tie(0);ios::sync_with_stdio(false);
	int i,j,k,c,n,m;cin>>n>>m;
	vector<string>genom(m);
	int Lsize=0;
	for(i=0;i<m;i++){cin>>genom[i];chmax(Lsize,genom[i].size()-2);}
	starttime=getTime();
	/*
	空白はない。
	他のゲノムに完全に含まれるゲノムは、無視。
	トーラスを40個作成する。
	1行目 対称性により固定
	1列目 全探索(1文字ヒント)
	2行目 全探索(1文字ヒント)
	2列目 全探索(2文字ヒント)
	3行目 全探索(2文字ヒント)
	行けそうな雰囲気がある。
	*/
	
	//genom重複除去
	//自分を含まない接尾辞をsetに入れ、自分があれば削除
	
	Trie<8,'A'> s_genom;
	for(i=0;i<m;i++){
		int s=genom[i].size();
		s_genom.add(genom[i].substr(0,s-1));
		for(j=1;j<s;j++){s_genom.add(genom[i].substr(j));}
	}
	vector<string>t_genom;
	for(i=0;i<m;i++){
		if(s_genom.count(genom[i])!=0){continue;}
		t_genom.pub(genom[i]);
		s_genom.add(genom[i]);
	}
	genom=move(t_genom);
	
	
	
	
	vector<string>ring;
	vector<string>b_ring;//break ring. 完全につながっていない、切れ端。
	for(int kai=0;kai<6;kai++){
		m=genom.size();
		cerr<<"m="<<m<<endl;
		vector<int>use(m);
		vector<string>moneg(m);
		for(i=0;i<m;i++){moneg[i]=genom[i];REV(moneg[i]);}
		vector<vector<int>>prev_str(m);
		vector<vector<int>>next_str(m);
		//次の文字の候補を推定しておく
		//一致文字が最長のもののリスト
		//常に、一致文字が最長のもののみを取ると仮定する
		vector<int>ps_it(m);
		vector<int>ns_it(m);
		//一致文字数
		//cerr<<"de"<<__LINE__<<endl;
		Trie<8,'A'> ps_genom;
		for(i=0;i<m;i++){ps_genom.add(moneg[i]);}
		for(i=0;i<m;i++){
			int s=moneg[i].size();
			for(j=s-1;j>0;j--){
				int idx=ps_genom.get_nodeidx(moneg[i].substr(s-j));
				if(idx==-1){continue;}//無い
				prev_str[i]=ps_genom.nodes[idx].accept_child;
				ps_it[i]=j;
				break;
			}
		}
		Trie<8,'A'> ns_genom;
		for(i=0;i<m;i++){ns_genom.add(genom[i]);}
		for(i=0;i<m;i++){
			int s=genom[i].size();
			for(j=s-1;j>0;j--){
				int idx=ns_genom.get_nodeidx(genom[i].substr(s-j));
				if(idx==-1){continue;}//無い
				next_str[i]=ns_genom.nodes[idx].accept_child;
				ns_it[i]=j;
				break;
			}
		}
		//cerr<<"de"<<__LINE__<<endl;
		//prev_str,next_strをみて、相方がそれよりも相性の良いパーツがある場合、それを除く
		for(i=0;i<m;i++){
			for(j=0;j<prev_str[i].size();j++){
				if(ns_it[prev_str[i][j]]!=ps_it[i]){prev_str[i][j]=99999;}
			}
			SO(prev_str[i]);
			while(prev_str[i].size()>0&&prev_str[i].back()==99999){prev_str[i].pob();}
		}
		for(i=0;i<m;i++){
			for(j=0;j<next_str[i].size();j++){
				if(ps_it[next_str[i][j]]!=ns_it[i]){next_str[i][j]=99999;}
			}
			SO(next_str[i]);
			while(next_str[i].size()>0&&next_str[i].back()==99999){next_str[i].pob();}
		}
		//cerr<<"de"<<__LINE__<<endl;
		/*
		リング推定
		リングを40個作る。
		アルゴリズム:連結が大きいものを優先して連結させる
		候補が複数残ってる場合、後回しにして、逆側から連結させるのを狙う
		リングがうまく作れない場合、後で修正できるようにしたい
		
		リングの「正確さ」という値を導入する。 連結が強固なほど、高い。
		*/
		
		//next_itrのsizeが1の、正しい組み合わせ同士を連結させる
		
		b_ring.clear();
		vector<vector<int>>b_tuka;
		for(i=0;i<m;i++){
			if(use[i]){continue;}
			string rs=genom[i];
			int L=i,R=i;
			vector<int>tuka={i};
			while(-1){
				if(rs.size()>n+10){break;}
				if(next_str[R].size()==1){
					int ata=next_str[R][0];
					if((!use[ata])&&prev_str[ata].size()==1&&prev_str[ata][0]==R){
						rs+=genom[ata].substr(ns_it[R]);
						R=ata;
						tuka.pub(R);
						continue;
					}
				}
				if(prev_str[L].size()==1){
					int ata=prev_str[L][0];
					if((!use[ata])&&next_str[ata].size()==1&&next_str[ata][0]==L){
						rs=genom[ata].substr(0,genom[ata].size()-ps_it[L])+rs;
						L=ata;
						tuka.pub(L);
						continue;
					}
				}
				break;
			}
			//しっかりトーラスになっているか見る
			if(rs.size()>n){
				bool ok=1;
				for(i=n;i<rs.size();i++){
					if(rs[i]!=rs[i%n]){ok=0;continue;}
				}
				if(ok){
					for(auto it:tuka){use[it]=1;}
					ring.pub(rs.substr(0,n));
				}else{
					for(auto it:tuka){use[it]=2;}
				}
			}else{
				for(auto it:tuka){use[it]=2;}
				b_ring.pub(rs);
				b_tuka.pub(tuka);
			}
		}
		//cerr<<"de"<<__LINE__<<endl;
		//b_ringは輪の切れ端 2つを組み合わせることができるのならば、つなげる
		vector<tuple<int,int,int,int>>kumi;//合う部分、ijの共通長さ、i,j
		for(i=0;i<b_ring.size();i++){
			for(j=i+1;j<b_ring.size();j++){
				int kyo=((int)b_ring[i].size())+b_ring[j].size()-n;
				if(kyo<=3- kai/3){continue;}
				for(k=0;k<=kyo;k++){
					if(lastr(b_ring[i],k)==b_ring[j].substr(0,k)&&lastr(b_ring[j],kyo-k)==b_ring[i].substr(0,kyo-k)){
						kumi.pub(mt(kyo,k,i,j));
					}
				}
			}
		}
		cerr<<"ringnum="<<ring.size()<<endl;
		cerr<<"bringnum="<<b_ring.size()<<endl;
		SO(kumi);
		REV(kumi);
		vector<int>b_use(b_ring.size());
		for(auto it:kumi){
			int kyo,sa,x,y;
			tie(kyo,sa,x,y)=it;
			if(b_use[x]+b_use[y]){continue;}
			b_use[x]=1;b_use[y]=1;
			ring.pub(b_ring[x].substr(kyo-sa)+b_ring[y].substr(sa));
			for(auto jt:b_tuka[x]){use[jt]=1;}
			for(auto jt:b_tuka[y]){use[jt]=1;}
		}
		vector<string>tt_genom;
		for(i=0;i<m;i++){
			if(use[i]!=1){tt_genom.pub(genom[i]);}
		}
		genom=move(tt_genom);
	}
	cerr<<"ringnum="<<ring.size()<<endl;
	//for(auto it:ring){cerr<<it<<endl;}
	//とりあえず半分の点を提出する
	
	cerr<<"L="<<Lsize<<endl;
	/*
	トーラス推定(最初の部分)
	トーラスを作る。
	縦,横,縦,横...でうまくはまるのを狙う。
	正しくないリングが紛れ込むこともある...
	ある程度はまり始めたら、少しあってないものも許容する
	あってないリングが多い列は、外す
	ある程度はまったら、次のステージに進む
	うまくいかないまま全探索が終わったら、もう一度やり直す
	ringにはまるなら、それは正しい文字列と考えてもよさそう
	*/
	for(i=0;i<ring.size();i++){ring[i]+=ring[i];}
	bool wakata=false;
	for(int bit=0;bit<=7;bit++){
		for(i=0;i<ring.size();i++){
			for(j=0;j<n;j++){
				int ban=0;
				for(k=0;k<3;k++){
					ban*=8;
					if(bit&(1<<(2-k))){
						ban+=ring[i][j+k]-'A';
					}
					
				}
				ofset[bit][ban].pub(mp(i,j));
			}
		}
	}
	if(ring.size()>32){
		vector<tuple<int,int,int>>kra,kkra;
		kkra.pub(mt(engine()%n,0,engine()%n));
		if(solve_toras(ring,kra,kkra,1)){wakata=true;}
	}
	cerr<<"L="<<Lsize<<endl;
	cerr<<"max_t="<<max_t<<endl;
	vector<pair<int,string>>rrr(b_ring.size());
	for(i=0;i<b_ring.size();i++){rrr[i]=mp((int)b_ring[i].size(),b_ring[i]);}
	SO(rrr);REV(rrr);
	if(wakata){
		cerr<<"wakatta!"<<endl;
		vector<string>ans(n);
		for(i=0;i<n;i++){ans[i]=string(n,'A');}
		for(auto it:toras_ans.fir){
			int a,x,y;
			tie(a,x,y)=it;
			for(i=0;i<n;i++){ans[x][i]=ring[a][i+y];}
		}
		for(auto it:toras_ans.sec){
			int a,x,y;
			tie(a,x,y)=it;
			for(i=0;i<n;i++){ans[i][x]=ring[a][i+y];}
		}
		for(i=0;i<n;i++){cout<<ans[i]<<endl;}
	}else{
		for(i=0;i<min((int)ring.size(),20);i++){cout<<ring[i].substr(0,20)<<endl;}
		if(ring.size()<20){
			for(i=0;i<20-ring.size();i++){
				cout<<rrr[i].sec;
				for(j=rrr[i].sec.size();j<20;j++){cout<<"A";}
				cout<<endl;
			}
		}
	}
	/*
	トーラス推定(終盤)
	大体はまったら、おかしなリングがわかっている。
	とりあえず、完全にはまるリングをはめる。
	その後、未回収の文字を、トーラスに直接当てはめていく。
	焼きなまし法を用いる。
	*/
	return 0;
}

