/*
TODO:

*/
//#define NDEBUG

//#define ONLINE_JUDGE

#ifndef ONLINE_JUDGE
//#define OPTUNA
#endif

#ifdef ONLINE_JUDGE
#define NDEBUG
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#endif

#include<bits/stdc++.h>
using namespace std;
using ll=long long int;
//using Int=__int128;
#define mask(x) ((1LL<<x)-1)
#define ALL(A) A.begin(),A.end()
template<typename T1,typename T2> bool chmin(T1 &a,T2 b){if(a<=b)return 0; a=b; return 1;}
template<typename T1,typename T2> bool chmax(T1 &a,T2 b){if(a>=b)return 0; a=b; return 1;}
template<typename T> int bitUP(T x,int a){return (x>>a)&1;}
enum Dir{
    Right,
    Down,
    Left,
    Up
};
//→　↓ ← ↑
int dh[4]={0,1,0,-1}, dw[4]={1,0,-1,0};
//上から時計回り
//int dx[8]={0,1,1,1,0,-1,-1,-1}, dy[8]={1,1,0,-1,-1,-1,0,1};
long double EPS = 1e-6;
const ll INF=(1LL<<62);
const int MAX=(1<<30);
using pii=pair<int,int>;
using pil=pair<int,ll>;
using pli=pair<ll,int>;
using pll=pair<ll,ll>;
using psi=pair<string,int>;
using pis=pair<int,string>;
using psl=pair<string,ll>;
using pls=pair<ll,string>;
using pss=pair<string,string>;
template<class T>
using minimum_queue=priority_queue<T,vector<T>,greater<T>>;

using Graph=vector<vector<int>>;

using i8=int8_t;
using i16=int16_t;
using i32=int32_t;
using i64=int64_t;

using u8=uint8_t;
using u16=uint16_t;
using u32=uint32_t;
using u64=uint64_t; 

void FastIO(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout << fixed << setprecision(20);
}
//0-indexed vector cin
template<typename T>
inline istream &operator>>(istream &is,vector<T> &v) {
    for(size_t i=0;i<v.size();i++) is>>v[i];
	return is;
}
 
//0-indexed vector cin
template<typename T>
inline istream &operator>>(istream &is,vector<vector<T>> &v) {
    for(size_t i=0;i<v.size();i++){
        is>>v[i];
    }
    return is;
}


struct Xor32{
    using u32=uint32_t;
 
    u32 x=1234567;
    inline u32 rnd_make(){
        x^=(x<<13);
        x^=(x>>17);
        x^=(x<<5);
        return x;
    }
    inline u32 operator()(){
        return rnd_make();
    }
    //[a,b)
    inline int operator()(int a,int b){
        int dis=b-a;
        int add=rnd_make()%dis;
        return a+add;
    }
    //[0,b)
    inline int operator()(int b){
        return rnd_make()%b;
    }
    //http://web.archive.org/web/20200105011004/https://topcoder.g.hatena.ne.jp/tomerun/20171216/
    //[0,b)の中から異なる2つを選ぶ [0]の値<[1]の値
    inline array<int,2> two(int b){
        assert(b>=2);
        int v1=rnd_make()%b;
        int v2=rnd_make()%(b-1);
        if (v2>=v1) return {v1,v2+1};
        else return {v2,v1};
    }
    inline float random01(){
        return float(rnd_make())/mask(32);
    }
    inline double random_double(double a,double b){
        double sa=b-a;
        a+=random01()*sa;
        return a;
    }
    //確率pでtrueを返す
    inline bool gen_bool(float p){
        return p>random01();
    }
};

struct Xor64{
    u64 x=1234567;
    inline u64 rnd_make(){
    	x ^= x << 13;
    	x ^= x >> 7;
    	x ^= x << 17;
    	return x;
    }
    inline u64 operator()(){
        return rnd_make();
    }
};

struct Timer{
    chrono::high_resolution_clock::time_point st;
    float local;
    Timer(){
#ifndef ONLINE_JUDGE
        local=1.0;
#endif
        start();
    }
    void start(){
        st=chrono::high_resolution_clock::now();
    }
    int span()const{
        auto now=chrono::high_resolution_clock::now();
        return chrono::duration_cast<chrono::milliseconds>(now-st).count();
    }
};
struct TestTimer{
    chrono::high_resolution_clock::time_point st;
    unordered_map<string,ll> sum_time;
    unordered_map<string,chrono::high_resolution_clock::time_point> start_time;

    TestTimer(){}
    void start(const string &s){
#ifndef ONLINE_JUDGE
        start_time[s]=chrono::high_resolution_clock::now();
#endif
    }
    void end(const string &s){
#ifndef ONLINE_JUDGE
        auto now=chrono::high_resolution_clock::now();
        sum_time[s]+=chrono::duration_cast<chrono::nanoseconds>(now-start_time[s]).count();
#endif
    }
    void output()const{
#ifndef ONLINE_JUDGE
        for(auto m:sum_time){
            cerr<<m.first<<": "<<m.second/1e6<<"ms"<<endl;
        }
#endif
    }
};
struct TestCounter{
    unordered_map<string,ll> cnt;

    TestCounter(){}

    void count(const string &s){
#ifndef ONLINE_JUDGE
        cnt[s]++;
#endif
    }
    void output()const{
#ifndef ONLINE_JUDGE
        for(auto m:cnt){
            cerr<<m.first<<": "<<m.second<<endl;
        }
#endif
    }    
};
Timer TIME;
Xor32 Rand32;
Xor64 Rand64;
TestTimer testTimer;
TestCounter testCounter;

//https://atcoder.jp/contests/asprocon9/submissions/34659956
template<class T,int CAPACITY>
class DynamicArray{
public:
    array<T,CAPACITY> array_={};
    int size_=0;

    DynamicArray(){}

    DynamicArray(int n){
        resize(n);
    }

    void push_back(const T &e){
        array_[size_++]=e;
    }
    //ソートされた状態を保つように挿入
    void insert(const T &e){
        int ng=-1,ok=size_;
        while(ok-ng!=1){
            int mid=(ok+ng)/2;
            if(array_[mid]>e) ok=mid;
            else ng=mid;
        }
        for(int i=size_;i>ok;i--){
            array_[i]=array_[i-1];
        }
        array_[ok]=e;
        size_++;
    }
    //eをこえる一番左の添字
    int find_binary_search(const T &e)const{
        int ng=-1,ok=size_;
        while(ok-ng!=1){
            int mid=(ok+ng)/2;
            if(array_[mid]>=e) ok=mid;
            else ng=mid;
        }
        return ok;
    }
    void pop_back(){
        size_--;
    }

    inline T& operator[](int index){
        return array_[index];
    }
	inline const T& operator[](int index) const {
		return array_[index];
	}
    inline int size()const{
        return size_;
    }
    inline T& back(){
        return array_[size_-1];
    }
    const inline T& back()const{
        return array_[size_-1];
    }
	inline auto begin() -> decltype(array_.begin()) {
		return array_.begin();
	}

	inline auto end() -> decltype(array_.begin()) {

		return array_.begin() + size_;
	}

	inline auto begin() const -> decltype(array_.begin()) {
		return array_.begin();
	}

	inline auto end() const -> decltype(array_.begin()) {

		return array_.begin() + size_;
	}
    inline void resize(int new_size){
        size_=new_size;
    }
    void operator=(const DynamicArray &e){
        for(int i=0;i<e.size_;i++){
            array_[i]=e[i];
        }
        size_=e.size_;
    }
    bool operator==(const DynamicArray &v){
        if(size_!=v.size_) return false;
        for(int i=0;i<size_;i++){
            if(array_[i]!=v[i]){
                return false;
            }
        }
        return true;
    }
    bool operator==(const vector<T> &v){
        if(size_!=v.size()) return false;
        for(int i=0;i<size_;i++){
            if(array_[i]!=v[i]){
                return false;
            }
        }
        return true;
    }
    void clear(){
        size_=0;
    }
    //O(1)
    //末尾と入れ替える。順序が保持されないが高速
    void swap_remove(int idx){
        array_[idx]=array_[size_-1];
        size_--;
    }
    //最初から見ていき、一致したものを削除(remove)
    bool erase(T value){
        for(int i=0;i<size_;i++){
            if(array_[i]==value){
                remove(i);
                return true;
            }
        }
        return false;
    }
    void reverse(){
        for(int i=0;i<size_/2;i++){
            swap(array_[i],array_[size_-1-i]);
        }
    }
    //O(size)
    //順序を気にしない場合、swap_removeの方がいい
    void remove(int idx){
        for(int i=idx;i<size_-1;i++){
            array_[i]=array_[i+1];
        }
        size_--;
    }
    void fill(T x){
        for(int i=0;i<size_;i++){
            array_[i]=x;
        }
    }
};

//ソート
template<typename T>
inline void vsort(vector<T> &v){
    sort(v.begin(),v.end());
}
//逆順ソート
template<typename T>
inline void rvsort(vector<T> &v){
	sort(v.rbegin(),v.rend());
}
//1ビットの数を返す
inline int popcount(int x){
	return __builtin_popcount(x);
}
//1ビットの数を返す
inline int popcount(ll x){
	return __builtin_popcountll(x);
}
template<typename T>
inline void Compress(vector<T> &C){
    sort(C.begin(),C.end());
    C.erase(unique(C.begin(),C.end()),C.end());
}
//要素数n 初期値x
template<typename T>
inline vector<T> vmake(size_t n,T x){
	return vector<T>(n,x);
}

//a,b,c,x data[a][b][c] 初期値x
template<typename... Args>
auto vmake(size_t n,Args... args){
	auto v=vmake(args...);
	return vector<decltype(v)>(n,move(v));
}

//vは昇順
bool is_in(const vector<int> &v,int x){
    int n=v.size();
    if(n==0) return false;
    int ng=-1,ok=n-1;
    while(ok-ng!=1){
        int mid=(ok+ng)/2;
        if(v[mid]<x) ng=mid;
        else ok=mid;
    }
    if(v[ok]==x) return true;
    return false;
}

template<typename T >
struct edge {
	int to;
	T cost;
    int id;
	edge()=default;
	edge(int to, T cost,int id) : to(to), cost(cost), id(id) {}
};
template<class T >
struct Edge {
    int from, to,id;
    T cost;
    Edge(int from,int to,T cost,int id):from(from),to(to),cost(cost),id(id){}
    Edge()=default;

    bool operator<(const Edge<T> &e){
        return cost<e.cost;
    }
    bool operator<=(const Edge<T> &e){
        return cost<=e.cost;
    }
};
template<typename T>
using WeightGraph=vector<vector<edge<T>>>;
//vector cout
template<typename T>
inline ostream &operator<<(ostream &os,const vector<T> &v) {
    bool sp=true;
    if(string(typeid(T).name())=="c") sp=false;
    for(size_t i=0;i<v.size();i++){
        if(i and sp) os<<" ";
        os<<v[i];
    }
    return os;
}
//vector<vector> cout
template<typename T>
inline ostream &operator<<(ostream &os,const vector<vector<T>> &v) {
    for(size_t i=0;i<v.size();i++){
        os<<v[i];
        if(i+1!=v.size()) os<<"\n";
    }
    return os;
}
//pair cout
template<typename T, typename U>
inline ostream &operator<<(ostream &os,const pair<T,U> &p) {
	os<<p.first<<" "<<p.second;
	return os;
}
//map cout
template<typename F, typename S>
inline ostream &operator<<(ostream &os,const map<F,S> &M) {
	bool first=false;
    for(auto x:M){
        if(first) os<<endl;
        first=true;
        os<<x;
    }
	return os;
}
//set cout
template<typename T>
inline ostream &operator<<(ostream &os,const set<T> &S) {
	bool first=false;
    for(auto x:S){
        if(first) os<<endl;
        first=true;
        os<<x;
    }
	return os;
}
 
//pair cin
template<typename T, typename U>
inline istream &operator>>(istream &is,pair<T,U> &p) {
	is>>p.first>>p.second;
	return is;
}
template<typename T>
void append(vector<T> &v,const vector<T> &vp){
    for(auto p:vp){
        v.push_back(p);
    }
}
//Fisher–Yatesアルゴリズム
template<typename T>
void shuffle(vector<T> &v){
    int sz=v.size();
    for(int i=sz-1;i>0;i--){
        swap(v[Rand32()%(i+1)],v[i]);
    }
}

template<class T,class U>
T linear_function(U x,U start_x,U end_x,T start_value,T end_value){
    if(x>=end_x) return end_value;
    if(x<=start_x) return start_value;
    return start_value+(end_value-start_value)*(x-start_x)/(end_x-start_x);
}

///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//https://atcoder.jp/contests/asprocon9/submissions/34659956
#ifndef OPTUNA 
#define REGIST_PARAM(name, type, defaultValue) constexpr type name = defaultValue
#else
#define REGIST_PARAM(name, type, defaultValue) type name = defaultValue
#endif

namespace OP{
    REGIST_PARAM(yama,bool,false);
    REGIST_PARAM(startTemp,double,500000);
    REGIST_PARAM(endTemp,float,0);
    REGIST_PARAM(TIME_END,int,1900);
};

constexpr int Height=6,Width=6;
constexpr int num_dim=15;
constexpr int num_tane=60;
constexpr int num_turn=10;
constexpr int max_value=100;

auto max_input=vmake(num_dim,0);

//変わるやつ
auto now_max=vmake(num_dim,0);
int now_turn=0;

namespace SIM{
    double per=0.9;
    double half=0.5;
    double max_per=0.9;
    double max_half=0.5;
    vector<double> weight_dim(15,1);
    vector<double> max_weight_dim(15,1);
    void reset(){
        per=max_per;
        half=max_half;
        weight_dim=max_weight_dim;
    }
    void maxset(){
        max_per=per;
        max_half=half;
        max_weight_dim=weight_dim;
    }
};

struct Tane{
    array<int,num_dim> value;
    int id;
    Tane(){}
    int same_value(){
        int res=0;
        for(int i=0;i<num_dim;i++){
            res+=value[i];
        }
        return res;
    }
    int is_90(int dim){
        return value[dim]>=now_max[dim]*SIM::per;
    }
    int is_max(int dim){
        return value[dim]>=now_max[dim];
    }
    int near_score(Tane &tane){
        double max_cnt=0;
        int cnt90=0;
        int sum_value=same_value()+tane.same_value();
        for(int i=0;i<num_dim;i++){
            double d0=double(value[i])/now_max[i];
            double d1=double(tane.value[i])/now_max[i];
            max_cnt+=max(d0,d1)*SIM::weight_dim[i];
            if(tane.is_max(i) or is_max(i)) max_cnt+=SIM::weight_dim[i];
            if(tane.is_max(i) and is_max(i)) max_cnt+=SIM::half*SIM::weight_dim[i];
            if(tane.is_90(i) or is_90(i)) cnt90+=SIM::weight_dim[i];
        }
        return max_cnt*1e5+sum_value+cnt90*1e3;
    }
    int num_heuristic_score(){
        int num_max=0;
        for(int i=0;i<num_dim;i++){
            if(is_max(i)) num_max++;
        }
        int sum_score=same_value();
        return num_max*100+sum_score;
    }
};
//変わるやつ2
auto tane_input=vmake(num_tane,Tane());

#ifndef ONLINE_JUDGE
auto yoko_input=vmake(num_turn,Height,Width-1,num_dim,false);
auto tate_input=vmake(num_turn,Height-1,Width,num_dim,false);
#endif


array<array<int,6>,6> grid_order;
vector<pii> vec_grid_order;

ll calc_score(){
    ll sum_max=0;
    for(int i=0;i<num_dim;i++){
        sum_max+=max_input[i];
    }
    ll value_max=0;
    for(int i=0;i<num_tane;i++){
        chmax(value_max,tane_input[i].same_value());
    }
    return 1e6*value_max/sum_max;
}

void input_first(){
    queue<pii> que;
    grid_order[2][2]=0;
    que.push({2,2});
    int id=1;
    while(que.size()){
        auto [h,w]=que.front();
        que.pop();
        vec_grid_order.push_back({h,w});
        for(int i=0;i<4;i++){
            int nh=h+dh[i];
            int nw=w+dw[i];
            if(nh<0 or nh>=Height or nw<0 or nw>=Width) continue;
            if(grid_order[nh][nw]!=0 or (nh==2 and nw==2)) continue;
            grid_order[nh][nw]=id++;
            que.push({nh,nw});
        }
    }
    //grid_orderを出力させる
    for(int h=0;h<Height;h++){
        for(int w=0;w<Width;w++){
            cerr<<grid_order[h][w];
            if(w+1!=Width) cerr<<" ";
        }
        cerr<<endl;
    }

    int dummy;
    for(int i=0;i<3;i++){
        cin>>dummy;
    }

    for(int i=0;i<num_tane;i++){
        tane_input[i].id=i;
        for(int j=0;j<num_dim;j++){
            cin>>tane_input[i].value[j];
            chmax(max_input[j],tane_input[i].value[j]);
        }
    }
    now_max=max_input;
    cout<<"#"<<calc_score()<<endl;
#ifndef ONLINE_JUDGE
    for(int i=0;i<num_turn;i++){
        for(int h=0;h<Height;h++){
            for(int w=0;w<Width-1;w++){
                string s;
                cin>>s;
                for(int d=0;d<num_dim;d++){
                    if(s[d]=='1'){
                        yoko_input[i][h][w][d]=true;
                    }
                }
            }
        }
        for(int h=0;h<Height-1;h++){
            for(int w=0;w<Width;w++){
                string s;
                cin>>s;
                for(int d=0;d<num_dim;d++){
                    if(s[d]=='1'){
                        tate_input[i][h][w][d]=true;
                    }
                }
            }
        }
    }
#endif
}
void output_interactive(array<array<int,6>,6> &grid_output){
    for(int h=0;h<Height;h++){
        for(int w=0;w<Width;w++){
            cout<<grid_output[h][w];
            if(w+1!=Width) cout<<" ";
        }
        cout<<endl;
    }
#ifdef ONLINE_JUDGE
    for(int i=0;i<num_tane;i++){
        for(int j=0;j<num_dim;j++){
            cin>>tane_input[i].value[j];
        }
    }
#else
    int id=0;
    array<int,15> value;
    auto tane2=tane_input;
    for(int h=0;h<Height;h++){
        for(int w=0;w<Width-1;w++,id++){
            int id1=grid_output[h][w];
            int id2=grid_output[h][w+1];
            for(int d=0;d<num_dim;d++){
                if(yoko_input[now_turn][h][w][d]==false){
                    value[d]=tane_input[id1].value[d];
                }else{
                    value[d]=tane_input[id2].value[d];
                }
            }
            tane2[id].value=value;
        }
    }
    for(int h=0;h<Height-1;h++){
        for(int w=0;w<Width;w++,id++){
            int id1=grid_output[h][w];
            int id2=grid_output[h+1][w];
            for(int d=0;d<num_dim;d++){
                if(tate_input[now_turn][h][w][d]==false){
                    value[d]=tane_input[id1].value[d];
                }else{
                    value[d]=tane_input[id2].value[d];
                }
            }
            tane2[id].value=value;
        }
    }
    tane_input=tane2;
#endif
    cout<<"#"<<calc_score()<<endl;
    now_turn++;
    for(int i=0;i<num_dim;i++){
        now_max[i]=-1;
    }
    for(int i=0;i<num_tane;i++){
        for(int j=0;j<num_dim;j++){
            chmax(now_max[j],tane_input[i].value[j]);
        }
    }
}

void simulate_interactive(array<array<int,6>,6> &grid_output){
    array<int,15> value;
    auto tane2=tane_input;
    int id=0;
    for(int h=0;h<Height;h++){
        for(int w=0;w<Width-1;w++,id++){
            int id1=grid_output[h][w];
            int id2=grid_output[h][w+1];
            for(int d=0;d<num_dim;d++){
                if(Rand32(2)){
                    value[d]=tane_input[id1].value[d];
                }else{
                    value[d]=tane_input[id2].value[d];
                }
            }
            tane2[id].value=value;
        }
    }
    for(int h=0;h<Height-1;h++){
        for(int w=0;w<Width;w++,id++){
            int id1=grid_output[h][w];
            int id2=grid_output[h+1][w];
            for(int d=0;d<num_dim;d++){
                if(Rand32(2)){
                    value[d]=tane_input[id1].value[d];
                }else{
                    value[d]=tane_input[id2].value[d];
                }
            }
            tane2[id].value=value;
        }
    }
    tane_input=tane2;
    now_turn++;
    for(int i=0;i<num_dim;i++){
        now_max[i]=-1;
    }
    for(int i=0;i<num_tane;i++){
        for(int j=0;j<num_dim;j++){
            chmax(now_max[j],tane_input[i].value[j]);
        }
    }
}

array<array<int,6>,6> make_grid_output(){
    int id=0;
    array<array<int,6>,6> grid_output;
    for(int h=0;h<Height;h++){
        for(int w=0;w<Width;w++,id++){
            grid_output[h][w]=-1;
        }
    }
    vector<bool> used(num_tane,false);

    for(auto place:vec_grid_order){
        int h=place.first;
        int w=place.second;
        int max_score=0;
        int max_t=-1;
        for(int t=0;t<num_tane;t++){
            if(used[t]) continue;
            int sum_score=0;
            for(int dir=0;dir<4;dir++){
                int nh=h+dh[dir];
                int nw=w+dw[dir];
                if(nh<0 or nh>=Height or nw<0 or nw>=Width) continue;
                if(grid_output[nh][nw]==-1) continue;
                sum_score+=tane_input[t].near_score(tane_input[grid_output[nh][nw]]);
            }
            if(sum_score==0){
                sum_score=tane_input[t].num_heuristic_score();
            }
            if(chmax(max_score,sum_score)){
                max_t=t;
            }
        }
        for(int i=0;i<15;i++){
            if(tane_input[max_t].is_max(i)){
                SIM::weight_dim[i]-=0.05; //0.1,0.1: 89,401,920 0.05,0.1: 89,915,268 0.05(0.2): 89,444
                chmax(SIM::weight_dim[i],0.01);
            }
        }
        grid_output[h][w]=max_t;
        used[max_t]=true;
    }
    fill(SIM::weight_dim.begin(),SIM::weight_dim.end(),1);
    return grid_output;
}

ll simulate(array<array<int,6>,6> &grid_output){
    simulate_interactive(grid_output);
    //cerr<<now_turn<<" "<<num_turn<<endl;
    for(int turn=now_turn;turn<num_turn;turn++){
        auto grid_output=make_grid_output();
        simulate_interactive(grid_output);
    }
    return calc_score();
}

double simulate(array<array<int,6>,6> &grid,int num_average){
    ll sum_score=0;
    //auto now_max=vmake(num_dim,0);
    auto save_now_max=now_max;
    auto save_now_turn=now_turn;
    auto save_tane_input=tane_input;
    for(int T=0;T<num_average;T++){
        sum_score+=simulate(grid);
        now_max=save_now_max;
        now_turn=save_now_turn;
        tane_input=save_tane_input;
    }
    return double(sum_score)/num_average;
}

void solve(){

    input_first();
    
    for(int T=0;T<num_turn;T++){
        auto grid_output=make_grid_output();
        double max_score=simulate(grid_output,10);

        int end_time=1900/10*(now_turn+1);
        int loop=0;
        int update=0;
        while(TIME.span()<end_time){
            //if(T!=num_turn-1) break;
            SIM::per=Rand32(70,100)/100.0;
            SIM::half=Rand32(20,80)/100.0;
            loop++;
            auto next_grid_output=make_grid_output();
            double score=simulate(next_grid_output,1);
            if(score<max_score) continue;
            score=simulate(next_grid_output,10);
            if(chmax(max_score,score)){
                SIM::reset();
                update++;
                grid_output=next_grid_output;
            }
        }
        SIM::reset();
        cerr<<"update/loop: "<<update<<"/"<<loop<<endl;
        output_interactive(grid_output);
    }
    
    
#ifndef ONLINE_JUDGE
    testTimer.output();
    testCounter.output();
    cerr<<TIME.span()<<"ms"<<endl;
    cerr<<"score: "<<calc_score()<<endl;
    //cerr<<"score: "<<simulate(best_grid,true)<<endl;
#endif
}
 
int main(const int argc,const char** argv){
#ifndef OPTUNA
    if(argc!=1){

    }
#endif
    FastIO();
    int T=1;
    //cin>>T;
    while(T--) solve();
}