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
        x=x^(x>>17);
        return x=x^(x<<5);
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
    //[0,b)の中から異なる2つを選ぶ first<second
    inline pair<int,int> two(int b){
        assert(b>=2);
        int v1=rnd_make()%b;
        int v2=rnd_make()%(b-1);
        if (v2>=v1) return {v1,v2+1};
        else return {v2,v1};
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
constexpr int height_grid=9;
constexpr int width_grid=9;
constexpr int num_stamps=20;
constexpr int max_cnt_try=81;
array<array<array<ll,3>,3>,21*21*21*21> stamps;
array<array<ll,width_grid>,height_grid> grid_input;
constexpr ll mod=998244353;

constexpr int Height=9;
constexpr int Width=9;


array<array<array<bool,width_grid>,height_grid>,50> is_score;
array<array<array<bool,width_grid>,height_grid>,50> is_hash;
array<array<int,9>,9> base_cnt;


void prepare_grid(){
    for(int h=0;h<height_grid;h++){
        for(int w=0;w<width_grid;w++){
            for(int i=0;i<50;i++){
                is_score[i][h][w]=false;
            }
        }
    }
    for(int depth=0;depth<49;depth++){
        for(int i=0;i<depth;i++){
            int h=i/7;
            int w=i%7;
            is_score[depth][h][w]=true;
            if(w==6){
                is_score[depth][h][7]=true;
                is_score[depth][h][8]=true;
            }
        }   
        //行
        if(depth>=43){
            for(int w=0;w<depth%7;w++){
                is_score[depth][7][w]=true;
                is_score[depth][8][w]=true;
            }
        }    
    }
    for(int h=0;h<height_grid;h++){
        for(int w=0;w<width_grid;w++){
            is_score[49][h][w]=true;
        }
    }
    for(int h=0;h<height_grid;h++){
        for(int w=0;w<width_grid;w++){
            for(int i=0;i<50;i++){
                is_hash[i][h][w]=(not is_score[i][h][w]);
            }
        }
    }

    for(int h=0;h<height_grid-2;h++){
        for(int w=0;w<width_grid-2;w++){
            base_cnt[h][w]=1;
            if(h==6){
                base_cnt[h][w]=3;
            }else if(h==0 or w==0 or w==6){
                base_cnt[h][w]=2;
            }
        }
    }
}


struct Place{
    int h,w;
    Place(){}
    Place(int h,int w):h(h),w(w){
 
    }
    Place(int idx):h(idx/Width),w(idx%Width){
 
    }
    int dist(const Place &np){
        return sqrt(double(h-np.h)*(h-np.h) + double(w-np.w)*(w-np.w))+1;
    }

    Place pre_place(int dir){
        return Place(h-dh[dir],w-dw[dir]);
    }    
    Place next_place(int dir){
        return Place(h+dh[dir],w+dw[dir]);
    }
    void move(int dir){
        h+=dh[dir];
        w+=dw[dir];
    }
    void remove(int dir){
        h-=dh[dir];
        w-=dw[dir];        
    }
    bool ok_move(int dir){
        return h+dh[dir]<Height and w+dw[dir]<Width and h+dh[dir]>=0 and w+dw[dir]>=0;
    }
    bool out_grid(){
        return h>=Height or w>=Width or h<0 or w<0;
    }
    bool operator==(const Place &p){
        return h==p.h and w==p.w;
    }
    bool operator!=(const Place &p){
        return h!=p.h or w!=p.w;
    }
    int idx(){
        return h*Width+w;
    }
    //上下 左右
    bool operator<(Place &p){
        if(p.h!=h) return h<p.h;
        return w<p.w;
    }
    friend istream& operator>>(istream& is, Place& p){
        is>>p.h>>p.w;
        return is;
    }
    friend ostream& operator<<(ostream& os, Place& p){
        os<<p.h<<" "<<p.w;
        return os;
    }
};

struct Action{
    Place p;
    int stamp;//21進数でやるか
};

array<array<DynamicArray<Place,9>,9>,9> place_scored;

struct State{
    array<array<ll,width_grid>,height_grid> grid;
    //(Place,stamp)
    vector<Action> history_actions;
    int cnt_rest=1;
    ll score=0;
    ll hash=0;

    State(){
        grid=grid_input;
    }

    void apply(Action action){
        //cerr<<action.p<<endl;
        int cnt=0;
        int s=action.stamp;
        while(s){
            cnt++;
            s/=21;
        }
        cnt_rest+=base_cnt[action.p.h][action.p.w]-cnt;
        //cerr<<cnt_rest<<endl; 
        history_actions.push_back(action);
        for(int h=0;h<3;h++){
            for(int w=0;w<3;w++){
                grid[action.p.h+h][action.p.w+w]+=stamps[action.stamp][h][w];
                if(grid[action.p.h+h][action.p.w+w]>=mod){
                    grid[action.p.h+h][action.p.w+w]-=mod;
                }
                //grid[action.p.h+h][action.p.w+w]%=mod;
            }
        }
        // while(action.stamp){
        //     cnt_actions++;
        //     int stamp=action.stamp%21;
        //     stamp--;
        //     for(int h=0;h<3;h++){
        //         for(int w=0;w<3;w++){
        //             grid[action.p.h+h][action.p.w+w]+=stamps[stamp][h][w];
        //             grid[action.p.h+h][action.p.w+w]%=mod;
        //         }
        //     }
        //     action.stamp/=21;
        // }
        score+=grid[action.p.h][action.p.w];
        if(action.p.h==6){
            score+=grid[action.p.h+1][action.p.w];
            score+=grid[action.p.h+2][action.p.w];
        }
        if(action.p.w==6){
            score+=grid[action.p.h][action.p.w+1];
            score+=grid[action.p.h][action.p.w+2];
        }
        if(action.p.h==6 and action.p.w==6){
            for(int h=7;h<9;h++){
                for(int w=7;w<9;w++){
                    score+=grid[h][w];
                }
            }
        }

        //cerr<<calc_score()<<endl;
        //cerr<<action.p<<endl;
        //assert(score==calc_score());
        //assert(hash==calc_hash());
    }

    vector<Action> find_next_actions(){
        auto p=Place(0,0);
        if(history_actions.size()){
            p=history_actions.back().p;
            p.w++;
            if(p.w==7){
                p.h++;
                p.w=0;
            }
        }
        vector<Action> res;
        int cnt=base_cnt[p.h][p.w];
        if(cnt_rest>=1){
            cnt++;
        }
        //chmin(cnt,3);
        ll X=0;
        auto dfs=[&](auto self,int depth,int x){
            if(depth==0){
                if((grid[p.h][p.w]+stamps[0][0][0])%mod>=9e8){
                    res.push_back({p,0});  
                } 
                //return;
            }
            if(depth==cnt){
                return;
            }
            for(int i=max(1,x%21);i<=num_stamps;i++){
                bool ok=true;
                ll base=9e8;
                int cnt=0;
                for(auto np:place_scored[p.h][p.w]){
                    if((grid[np.h][np.w]+stamps[x*21+i][np.h-p.h][np.w-p.w])%mod<base){
                        ok=false;
                    }
                    //break;
                    if(base==9e8) base=8e8;
                    cnt++;
                    if(cnt>=3) break;
                }
                if(ok) res.push_back({p,x*21+i});
                //     if((grid[p.h][p.w]+stamps[x*21+i][0][0])%mod>=9e8)
                //     if(p.w!=6 or (grid[p.h][p.w+1]+stamps[x*21+i][0][1])%mod>=8e8){
                //         res.push_back({p,x*21+i});
                    
                // } 
                self(self,depth+1,x*21+i);
            }
        };
        dfs(dfs,0,0);
        // for(int i=0;i<=num_stamps;i++){
        //     res.push_back({p,i});
        // }
        // if(p.w==6 or p.h==6 or p.h==0 or p.w==0){
        //     for(int i=1;i<=num_stamps;i++){
        //         for(int j=i;j<=num_stamps;j++){
        //             res.push_back({p,i*21+j});
        //         }
        //     }
        // }
        // if(p.h==6){
        //     for(int i=1;i<=num_stamps;i++){
        //         for(int j=i;j<=num_stamps;j++){
        //             for(int k=j;k<=num_stamps;k++){
        //                 res.push_back({p,i*21*21+j*21+k});
        //             }
        //         }
        //     }
        // }
        return res;
    }

    ll calc_score()const{
        ll res=0;
        for(int h=0;h<height_grid;h++){
            for(int w=0;w<width_grid;w++){
                if(is_score[history_actions.size()][h][w]){
                    res+=grid[h][w];
                    //res%=mod;
                }
            }
        }
        return res;
    }

    u64 calc_hash()const{
        u64 res=0;
        for(int h=0;h<height_grid;h++){
            for(int w=0;w<width_grid;w++){
                if(is_hash[history_actions.size()][h][w]){
                    res^=grid[h][w];
                }
            }
        }
        return res;
    }

    bool operator<(const State &state)const{
        return score>state.score;
    }

};

template<class Action,class Score,class State,class Hash>
vector<Action> beam_search(State init_state,int width_beam,int max_depth){
    vector<State> states={init_state};
    vector<State> next_states;

    for(int depth=0;depth<max_depth;depth++){
        next_states.clear();
#ifdef ONLINE_JUDGE
        if(TIME.span()>1700){
            width_beam=1;
        }
#endif

        for(int i=0;i<states.size();i++){
            vector<Action> next_actions=states[i].find_next_actions();
            for(Action action:next_actions){
                State state=states[i];
                state.apply(action);
                next_states.push_back(state);
            }
        }
        sort(next_states.begin(),next_states.end());
        states.clear();
        unordered_set<u64> set;
        vector<int> cnt(81,0);
        for(int i=0;i<next_states.size() and states.size()<width_beam;i++){
            auto &state=next_states[i];
            if(i<width_beam or cnt[state.cnt_rest]==0){
                assert(i<width_beam);
                states.push_back(state);
                cnt[state.cnt_rest]++;
            }
            //cout<<cnt<<endl;
            //cerr<<-1<<endl;
            // u64 hash=state.calc_hash();
            // if(set.count(hash)){
            //     continue;
            // }
            // set.insert(hash);
            //states.push_back(state);
        }
    }

    sort(states.begin(),states.end());
    cerr<<"score: "<<states[0].calc_score()<<endl;
    return states[0].history_actions;
}


void input(){
    int dummy;
    cin>>dummy>>dummy>>dummy;

    for(int h=0;h<height_grid;h++){
        for(int w=0;w<width_grid;w++){
            cin>>grid_input[h][w];
        }
    }

    for(int i=0;i<stamps.size();i++){
        for(int h=0;h<3;h++){
            for(int w=0;w<3;w++){
                stamps[i][h][w]=0;
            }
        }
    }
    for(int i=0;i<num_stamps;i++){
        for(int h=0;h<3;h++){
            for(int w=0;w<3;w++){
                cin>>stamps[i+1][h][w];
            }
        }
    }

    for(int i=21;i<stamps.size();i++){
        int stamp=i;
        while(stamp){
            for(int h=0;h<3;h++){
                for(int w=0;w<3;w++){
                    stamps[i][h][w]+=stamps[stamp%21][h][w];
                    if(stamps[i][h][w]>=mod){
                        stamps[i][h][w]-=mod;
                    }
                    //stamps[i][h][w]%=mod;
                }
            }
            stamp/=21;
        }
    }
}

void solve(){
    input();
    prepare_grid();

    for(int h=0;h<=6;h++){
        for(int w=0;w<=6;w++){
            Place p(h,w);
            place_scored[h][w].push_back(p);

            if(p.h==6){
                auto np=p;
                np.h++;
                place_scored[h][w].push_back(np);
                np.h++;
                place_scored[h][w].push_back(np);
            }
            if(p.w==6){
                auto np=p;
                np.w++;
                place_scored[h][w].push_back(np);
                np.w++;
                place_scored[h][w].push_back(np);
            }
            if(p.h==6 and p.w==6){
                for(int h=7;h<9;h++){
                    for(int w=7;w<9;w++){
                        place_scored[h][w].push_back(Place(h,w));
                    }
                }
            }
        }
    }


    State init_state;
    auto actions=beam_search<Action,ll,State,u64>(init_state,1700,49);

    vector<Action> ans;
    for(int i=0;i<actions.size();i++){
        while(actions[i].stamp){
            ans.push_back({actions[i].p,actions[i].stamp%21-1});
            actions[i].stamp/=21;
        }
    }
    cout<<ans.size()<<endl;
    for(auto act:ans){
        cout<<act.stamp<<" "<<act.p<<endl;
    }
#ifndef ONLINE_JUDGE
    testTimer.output();
    testCounter.output();
    cerr<<TIME.span()<<"ms"<<endl;
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