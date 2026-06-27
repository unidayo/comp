#pragma GCC optimize("O3,inline")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#pragma GCC optimize("unroll-loops")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
#pragma GCC target("movbe")
#pragma GCC target("aes,pclmul,rdrnd")
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<tuple>
#include<chrono>
#include<map>
#include<set>
#include<algorithm>
#include<queue>
#include<random>
#include<numeric>
#include<functional>
#include<stack>
#include<cassert>
#include<cmath>
#include<cstring>
#include<unordered_set>
#include<unordered_map>
#include<ranges>
#include<bitset>
#include<bit>

#define rep(i, n) for (int i = 0; i < (int)n; i++)
#define reps(i, s, n) for (int i = s; i < (int)n; i++)
#define repv(x, v) for (const auto& x : v)
#define rep2(i,j,n) for (int i = 0; i < (int)n; i++) for (int j = 0; j < (int)n; j++)
#define rep3(i,j,k,n) for (int i = 0; i < (int)n; i++) for (int j = 0; j < (int)n; j++) for (int k= 0; k < (int)n; k++)
#define all(v) v.begin(),v.end()

template<typename T>
using vec = std::vector<T>;
template<typename T>
using vec2 = vec<vec<T>>;
template<typename T>
using vec3 = vec<vec2<T>>;
using ull = unsigned long long;
using ll = long long;
template<typename T>
using t2 = std::tuple<T, T>;
using i2 = std::pair<int,int>;
template<typename T>
using t3 = std::tuple<T, T, T>;
using i3 = t3<int>;
using clk = std::chrono::system_clock;

template<int k>
const double p10_k = std::pow(10, k);

// global

constexpr int N = 20;
constexpr int N2 = N*N;
vec2<int> W;
vec2<int> D;
vec<int> W1;
vec<int> D1;
vec2<int> graph;
vec2<int> mdist;

constexpr bool local = false;
std::map<std::string, std::string> LOG;

std::random_device rnd;
std::mt19937 mt(rnd());
std::uniform_real_distribution<> rand01(0.0, 1.0);

clk::time_point start_time;
std::ofstream ofstr; // result file

// utils

template<typename T> void print(const T& v){ std::cerr << v; }
template<typename... T> void print(const std::tuple<T...>& tp);
template<typename T>
void print(const std::pair<T,T>& p){
    std::cerr << "(" << p.first << " " << p.second << ")";
}
template<typename T>
void print(const std::vector<T>& vs){
    std::cerr << "[";
    rep(i, vs.size()){
        if(i > 0) print(" ");
        print(vs[i]);
    }
    std::cerr << "]";
}
template<typename TupType, size_t... I>
void print(const TupType& tp, std::index_sequence<I...>){
    std::cerr << "(";
    (..., (print(I==0? "" : " "), print(std::get<I>(tp))));
    std::cerr << ")";
}
template<typename... T>
void print(const std::tuple<T...>& tp){
    print(tp, std::make_index_sequence<sizeof...(T)>());
}
template<class T, class... A> void print(const T& first, const A&... rest) { print(first); print(" "); print(rest...); }
template<class... T>
void dprint(const T&... rest){
    if(!local) return;
    std::cerr << "# ";
    print(rest...);
    std::cerr << "\n";
}

double getElapsed(const clk::time_point& start, const clk::time_point& end){
    return (std::chrono::duration<double, std::milli>(end-start)).count() / 1000;
}

template<typename T>
T sum(const vec<T>& v){
    T res = 0;
    for(const auto x: v) res += x;
    return res;
}

template<typename T>
std::tuple<size_t,T> argmax(const vec<T>& v){
    T mx = std::numeric_limits<T>::lowest();
    size_t mi = 0;
    const int vsize = v.size();
    rep(i,vsize){
        if(mx < v[i]){
            mx = v[i]; mi = i;
        }
    }
    return {mi, mx};
}

template<typename T>
std::tuple<size_t,T> argmin(const vec<T>& v){
    T mx = std::numeric_limits<T>::max();
    size_t mi = 0;
    const int vsize = v.size();
    rep(i,vsize){
        if(mx > v[i]){
            mx = v[i]; mi = i;
        }
    }
    return {mi, mx};
}

vec<int> range(const int n){
    vec<int> v(n);
    std::iota(all(v), 0);
    return v;
}

uint32_t xorShift() {
  static uint32_t y = 2463534242;
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return y = y ^ (y << 5);
}

uint32_t randint(const uint32_t size){
    uint32_t a = xorShift();
    uint64_t m = (uint64_t)a * (uint64_t) size;
    return m >> 32;
}

vec<int> sample(int mx, int num){
    vec<int> res;
    while(res.size() < num){
        res.emplace_back(randint(mx));
        std::sort(all(res));
        auto unique_end = std::unique(all(res));
        res.erase(unique_end, res.end());
    }
    return res;
}

int randbool(){
    return mt() & 1;
}

template<typename T>
vec<int> getOrderOf(vec<T>& v){
    auto res = range(v.size());
    std::sort(all(res),
        [&](int x, int y){return v[x] < v[y];}
    );
    return res;
}

// functions
const int LOG_TABLE_SIZE = 1 << 14;
double log_table[LOG_TABLE_SIZE];
inline double log_rand() {
    static int idx = 0;
    if (idx == LOG_TABLE_SIZE) {
        idx = 0;
    }
    return log_table[idx++];
}

i2 pos(int v){
    return {v/N, v%N};
}

vec<int> getMove(int v, int nv){
    vec<int> moves;
    const auto [x,y] = pos(v);
    const auto [nx,ny] = pos(nv);
    if(x < nx){
        moves.insert(moves.end(), nx-x, 4);
    } else if (nx < x){
        moves.insert(moves.end(), x-nx, 3);
    }
    if(y < ny){
        moves.insert(moves.end(), ny-y, 6);
    } else if (ny < y){
        moves.insert(moves.end(), y-ny, 5);
    }
    return moves;
}

int getDist(int v, int nv){
    const auto [x,y] = pos(v);
    const auto [nx,ny] = pos(nv);
    return std::abs(x-nx) + std::abs(y-ny);
}

void initialize(){
    if(local){
        ofstr = std::ofstream("./result.txt");
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(0);
        auto original_buf = std::cout.rdbuf();
        std::cout.rdbuf(ofstr.rdbuf());
    }
    start_time = clk::now();
    for (int i = 0; i < LOG_TABLE_SIZE; i++) {
        log_table[i] = log(rand01(mt));
    }
    int n;
    std::cin >> n;
    rep(i,N){
        int w;
        vec<int> row;
        rep(j,N){
            std::cin >> w;
            W1.push_back(w);
            row.push_back(w);
        }
        W.push_back(row);
    }
    rep(i,N){
        int d;
        vec<int> row;
        rep(j,N){
            std::cin >> d;
            D1.push_back(d);
            row.push_back(d);
        }
        D.push_back(row);
    }

    graph = vec2<int>(N2);
    vec<i2> dirs{{1,0},{0,1},{-1,0},{0,-1}};
    rep(x,N){
        rep(y,N){
            int v = N*x + y;
            for(const auto [dx,dy] : dirs){
                int nx = x+dx;
                int ny = y+dy;
                if(nx<0 || N <= nx || ny < 0 || N <= ny){
                    continue;
                }
                int nv = nx*N+ny;
                graph[v].push_back(nv);
            }
        }
    }
    mdist = vec2<int>(N2, vec<int>(N2, 0));
    rep(v,N2){
        rep(nv, N2){
            int d = getDist(v,nv);
            mdist[v][nv] = d;
        }
    }
}
void deconstruct(){
}


bool isValid(const vec<int>& path){
    int dist = 0;
    int damage = 0;
    for(int k = path.size()-3; k >= 0; k--){
        const int v = path[k];
        const int nv = path[k+1];
        const int nnv = path[k+2];
        const int w = W1[nv];
        const int d = D1[v];
        //dprint(w,d);
        dist += getDist(nv,nnv);
        damage += w * dist;
        //dprint(d,damage);
        if(d <= damage){
            return false;
        }
    }
    return true;
}

vec<int> solve(){

    //vec<int> path{13,53,59,56,0};
    //dprint(isValid(path));
    //exit(0);
    vec2<int> paths;
    reps(v,1,N2){
        vec<int> path;
        path.push_back(v);
        path.push_back(0);
        paths.emplace_back(path);
    }

    auto bpaths = paths;
    int bscore = 15200;

    auto npaths = bpaths;
    int nscore = bscore;

    const double stemp = 1.7;
    const double etemp = 0.0;
    auto now_time = clk::now();
    double elapsed = getElapsed(start_time, now_time);
    constexpr double end_time = 1.98;

    int subCnt = 0;
    int loopCnt = 0;
    int naccept = 0;
    while(elapsed < end_time){
        subCnt++;
        if(subCnt == 100){
            subCnt = 0;
            now_time = clk::now();
            elapsed = getElapsed(start_time, now_time);
        }

        const int nnroute = npaths.size();
        const int i = randint(nnroute);
        int j = randint(nnroute-1);
        if(i==j) j++;

        int ti,tj,v;
        const int s = randint(4);
        if(s==0){
            // insert
            ti = randint(npaths[i].size()-1);
            tj = randint(npaths[j].size());
            v = npaths[i][ti];
            npaths[j].insert(npaths[j].begin() + tj, v);
            // erase path[i] after validation
        } else {
            // swap
            //if(npaths[i].size() == 2 && npaths[j].size() == 2) continue;
            ti = randint(npaths[i].size()-1);
            tj = randint(npaths[j].size()-1);
            std::swap(npaths[i][ti], npaths[j][tj]);
        }
        //dprint(tpaths[i], tpaths[j]);

        if(s==0){
            if(!isValid(npaths[j])){
                const int v = npaths[j][tj];
                //npaths[i].insert(npaths[i].begin()+ti, v);
                npaths[j].erase(npaths[j].begin() + tj);
                continue;
            }
            npaths[i].erase(npaths[i].begin() + ti);
        } else {
            if(!(isValid(npaths[i]) && isValid(npaths[j]))){
                std::swap(npaths[i][ti], npaths[j][tj]);
                continue;
            }
        }

        int tscore = nscore;

        {
            int diff3 = 0;
            if(s==0){
                {
                    const int pv = ti > 0 ? npaths[i][ti-1] : 0;
                    const int nv = npaths[i][ti];
                    diff3 -= mdist[pv][v] + mdist[v][nv] - mdist[pv][nv];
                }
                {
                    const int pv = tj > 0 ? npaths[j][tj-1] : 0;
                    const int v = npaths[j][tj];
                    const int nv = npaths[j][tj+1];
                    diff3 += mdist[pv][v] + mdist[v][nv] - mdist[pv][nv];
                }
            } else {
                {
                    const int pv = ti > 0 ? npaths[i][ti-1] : 0;
                    const int u = npaths[j][tj];
                    const int v = npaths[i][ti];
                    const int nv = npaths[i][ti+1];
                    diff3 += -mdist[pv][u]-mdist[u][nv] + mdist[pv][v] + mdist[v][nv];
                }
                {
                    const int pv = tj > 0 ? npaths[j][tj-1] : 0;
                    const int u = npaths[i][ti];
                    const int v = npaths[j][tj];
                    const int nv = npaths[j][tj+1];
                    diff3 += -mdist[pv][u]-mdist[u][nv] + mdist[pv][v] + mdist[v][nv];
                }
            }
            tscore += diff3;
        }
        //dprint(tscore, nscore);
        //dprint(npaths[i], npaths[j]);
        //dprint(tpaths[i], tpaths[j]);
        //exit(0);

        const int diff = nscore - tscore;

        bool accept = false;
        if(diff > 0){
            accept = true;
        } else if(diff <= 0){
            const double temp = stemp + elapsed * (etemp - stemp) / end_time;
            if(log_rand() * temp < diff){
                accept = true;
            }
        }
        if(accept){
            nscore = tscore;
            //npaths = tpaths;
            //npaths[i] = pathi;
            //npaths[j] = pathj;
            npaths.erase(
                std::remove_if(npaths.begin(), npaths.end(),
                   [](const vec<int>& v) { return v.size() <= 1; }),
                npaths.end()
            );

            if(nscore < bscore){
                //dprint(loopCnt, "bscore", bscore, elapsed);
                bscore = nscore;
                bpaths = npaths;
            }
        } else {
            if(s==0){
                int v = npaths[j][tj];
                npaths[i].insert(npaths[i].begin()+ti, v);
                npaths[j].erase(npaths[j].begin() + tj);
            } else {
                std::swap(npaths[i][ti], npaths[j][tj]);
            }
        }
        loopCnt++;
    }
    dprint("loopCnt:", loopCnt);

    vec<int> result;
    result.reserve(20000);
    for(const auto& path : bpaths){
        int v = 0 ;
        for(const auto nv : path){
            auto moves = getMove(v,nv);
            result.insert(result.end(), all(moves));
            if(nv != 0) result.push_back(1);
            v = nv;
        }
    }

    vec<char> dirs{'U','D','L','R'};
    int score = 0;
    for(const auto act : result){
        if(act <= 2){
            std::cout << act << std::endl;
        } else if (act <= 6){
            std::cout << dirs[act-3] << std::endl;
            score++;
        }
    }
    dprint("score:", N2 + 2*N*N*N - score);
    return result;
}

// main

int main(){
    initialize();
    solve();
    if(local){
        auto end_time = clk::now();
        double elapsed = getElapsed(start_time, end_time);
        dprint("time:", elapsed);
    }
    deconstruct();
    return 0;
}
