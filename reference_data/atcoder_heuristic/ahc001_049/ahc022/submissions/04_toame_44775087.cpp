#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <stack>
#include <functional>
#include <algorithm>
#include <climits>
#include <cmath>
#include <iomanip>
#include <cassert>
using namespace std;
#define ll long long long long int
#define rep(i,n) for( long long int i = 0; i < n; i++ )
#define rrep(i,n) for( long long int i = n; i >= 0; i-- )
#define REP(i,s,t) for( long long int i = s; i <= t; i++ )
#define RREP(i,s,t) for( long long int i = s; i >= t; i-- )
#define dump(x)  cerr << #x << " = " << (x) << endl;
#define INF 2000000000
#define mod 1000000007
#define INF2 1000000000000000000
const int64_t CYCLES_PER_SEC=3000000000;
#define TIMES 1.0
double timeLimit=3.30*TIMES;
double triangleWave(double x, double T) {
    double phase = std::fmod(x, T) / T;
    return 4.0 * std::abs(phase - 0.5) - 1.0;
}
template<typename T> class Hungarian
{
private:
    const long long int U, V;
    vector<vector<long long int> > graph;
    vector<T> dual;
    vector<long long int> alloc, rev_alloc, prev;
    const vector<vector<T> >& cost;
    long long int matching_size;
    T diff(const long long int i, const long long int j){
        return cost[i][j] - dual[i] - dual[U + j];
    }
    void init_feasible_dual(){
        for(long long int i = 0; i < U; ++i){
            dual[i] = 0;
            for(long long int j = 0; j < V; ++j){
                dual[U + j] = min(dual[U + j], cost[i][j]);
            }
        }
    }
    void construct_graph(){
        for(long long int i = 0; i < U; ++i){
            for(long long int j = 0; j < V; ++j){
                graph[i][j] = (diff(i, j) == 0 && rev_alloc[j] != i);
            }
        }
    }
    bool find_augmenting_path(const long long int cur, const long long int prv, long long int& pos){
        prev[cur] = prv;
        if(cur >= U){
            if(rev_alloc[cur - U] < 0) return true;
            if(find_augmenting_path(rev_alloc[cur - U], cur, pos)){
                graph[rev_alloc[cur - U]][cur - U] = 1;
                return true;
            }
        }else{
            const long long int MX = (alloc[cur] < 0 && pos == U) ? U : V;
            for(long long int i = 0; i < MX; ++i){
                if(graph[cur][i] && prev[U + i] < 0 && find_augmenting_path(U + i, cur, pos)){
                    graph[cur][i] = 0, alloc[cur] = i, rev_alloc[i] = cur;
                    return true;
                }
            }
            if(alloc[cur] < 0 && pos < U){
                graph[cur][pos] = 0, alloc[cur] = pos, rev_alloc[pos] = cur, prev[U + pos] = cur;
                return ++pos, true;
            }
        }
        return false;
    }
    void update_dual(const T delta){
        for(long long int i = 0; i < U; ++i) if(prev[i] >= 0) dual[i] += delta;
        for(long long int i = U; i < U + V; ++i) if(prev[i] >= 0) dual[i] -= delta;
    }
    void maximum_matching(bool initial=false){
        long long int pos = initial ? V : U;
        for(bool update = false;; update = false){
            fill(prev.begin(), prev.end(), -1);
            for(long long int i = 0; i < U; ++i){
                if(alloc[i] < 0 && find_augmenting_path(i, 2 * U, pos)){
                    update = true, ++matching_size;
                    break;
                }
            }
            if(!update) break;
        }
    }
    long long int dfs(const long long int cur, const long long int prv, vector<long long int>& new_ver){
        prev[cur] = prv;
        if(cur >= U){
            if(rev_alloc[cur - U] < 0) return cur;
            else return dfs(rev_alloc[cur - U], cur, new_ver);
        }else{
            new_ver.push_back(cur);
            for(long long int i = 0; i < V; ++i){
                if(graph[cur][i] && prev[U + i] < 0){
                    const long long int res = dfs(U + i, cur, new_ver);
                    if(res >= U) return res;
                }
            }
        }
        return -1;
    }
    long long int increase_matching(const vector<pair<long long int, long long int> >& vec, vector<long long int>& new_ver){
        for(const auto& e : vec){
            if(prev[e.first] < 0){
                const long long int res = dfs(e.first, e.second, new_ver);
                if(res >= U) return res;
            }
        }
        return -1;
    }
    void hint_increment(long long int cur){
        while(prev[cur] != 2 * U){
            if(cur >= U){
                graph[prev[cur]][cur - U] = 0, alloc[prev[cur]] = cur - U, rev_alloc[cur - U] = prev[cur];
            }else{
                graph[cur][prev[cur] - U] = 1;
            }
            cur = prev[cur];
        }
    }
public:
    Hungarian(const vector<vector<T> >& _cost)
         : U((long long int)_cost.size()), V((long long int)_cost[0].size()), graph(U, vector<long long int>(U, 1)), dual(U + V, numeric_limits<T>::max()),
            alloc(U, -1), rev_alloc(U, -1), prev(2 * U), cost{_cost}, matching_size(0){
        assert(U >= V);
    }
    pair<T, vector<long long int> > solve(){
        init_feasible_dual(), construct_graph();
        bool end = false;
        maximum_matching(true);
        while(matching_size < U){
            vector<pair<T, long long int> > cand(V, {numeric_limits<T>::max(), numeric_limits<long long int>::max()});
            for(long long int i = 0; i < U; ++i){
                if(prev[i] < 0) continue;
                for(long long int j = 0; j < V; ++j){
                    if(prev[U + j] >= 0) continue;
                    cand[j] = min(cand[j], {diff(i, j), i});
                }
            }
            while(true){
                T delta = numeric_limits<T>::max();
                for(long long int i = 0; i < V; ++i){
                    if(prev[U + i] >= 0) continue;
                    delta = min(delta, cand[i].first);
                }
                update_dual(delta);
                vector<pair<long long int, long long int> > vec;
                vector<long long int> new_ver;
                for(long long int i = 0; i < V; ++i){
                    if(prev[U + i] >= 0) continue;
                    if((cand[i].first -= delta) == 0) vec.emplace_back(U + i, cand[i].second);
                }
                long long int res = increase_matching(vec, new_ver);
                if(res >= U){
                    hint_increment(res);
                    if(++matching_size == U) end = true;
                    else construct_graph();
                    break;
                }else{
                    for(const long long int v : new_ver){
                        for(long long int i = 0; i < V; ++i){
                            if(prev[U + i] >= 0) continue;
                            cand[i] = min(cand[i], {diff(v, i), v});
                        }
                    }
                }
            }
            if(!end) maximum_matching();
        }
        T total_cost = 0;
        for(long long int i = 0; i < U; ++i){
            if(alloc[i] < V) total_cost += cost[i][alloc[i]];
            else alloc[i] = -1;
        }
        return make_pair(total_cost, alloc);
    }
};

struct Timer {
    int64_t start;
	Timer() { reset(); }
	void reset() { start = getCycle(); }
	inline double get() { return (double)(getCycle() - start) / CYCLES_PER_SEC; }
	inline int64_t getCycle() {
		uint32_t low, high;
		__asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
		return ((int64_t)low) | ((int64_t)high << 32);
	}
} timer;
unsigned int randInt() {
	static unsigned int tx = 123456791, ty = 362436069, tz = 521288629, tw = 88675123;
	unsigned int tt = (tx ^ (tx << 11));
	tx = ty; ty = tz; tz = tw;
	return (tw = (tw ^ (tw >> 19)) ^ (tt ^ (tt >> 8)));
}
double randDouble() {
    return (double)(randInt())/UINT_MAX;
}
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
using namespace std;

class Measure_Log{
public:
    long long int num;
    long long int y;
    long long int x; 
    long long int t;
    Measure_Log(long long int num, long long int y, long long int x, long long int t) {
        this->num = num;
        this->y = y;
        this->x = x;
        this->t = t;
    }
};
#include <iostream>
#include <vector>
#include <cmath>


long long int L, N, S;
long long int X[110], Y[110];
long long int point_num[110][110];
long long int predict[110][110];
long long int temp_distribution[110][110];
vector<Measure_Log> measure_log[110];
vector<long long int> dy_v{0, 1, 0, -1};
vector<long long int> dx_v{1, 0, -1, 0};
long long int placement_cost(int y, int x, const std::vector<std::vector<double>>& f, int K = 2) {
    long long int ret = 0;
    for(int k = 0; k < K; k++) {
        int yy = (y + dy_v[k] + L) % L;
        int xx = (x + dx_v[k] + L) % L;
        int diff = (f[yy][xx] - f[y][x]);
        ret += diff * diff;
    }
    return ret;
}
long long int placement_cost_all(const std::vector<std::vector<double>>& f) {
    long long int ret = 0;
    for(int i = 0; i < L; i++) {
        for(int j = 0; j < L; j++) {
            ret += placement_cost(i, j, f);
        }
    }
    return ret;
}
const int SPLIT = 13;
double fast_exp[100010];
void get_temp(double lower, double upper, double weight, std::vector<std::vector<double>>& f) {
    if(S >= 3 * 3) {
        f[0][0] = lower;
        f[L/2][L/2] = upper;
    }
    if (S >= SPLIT * SPLIT) {
        for(int i = 0; i < dx_v.size(); i++) {
            f[L/2 + dy_v[i]][L/2 + dx_v[i]] = upper;
        }
    }
    double alpha = 0.015; // Learning rate
    if(S <= 5 * 5) alpha = 0.01;
    int prev_time = timer.get() * 5.0;
    std::vector<std::vector<double>> gradient(L, std::vector<double>(L, 0));
    const double target_cost = S * 25000.0;
    while(timer.get() < timeLimit * 0.40) {
        for (int x = 0; x < L; ++x) {
            for (int y = 0; y < L; ++y) {
                gradient[x][y] = 4 * f[x][y] - f[(x + 1) % L][y] - f[(x - 1 + L) % L][y] - f[x][(y + 1) % L] - f[x][(y - 1 + L) % L];
            }
        }
        double w1 = 2.0;
        if (S >= 20 * 20) w1 = 3.0;
        if (S >= 25 * 25) w1 = 4.0;
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < N; j++) {
                if (i == j) continue;
                const double diff = abs(f[Y[i]][X[i]] - f[Y[j]][X[j]])/S;
                if (f[Y[i]][X[i]] < f[Y[j]][X[j]])
                    gradient[Y[i]][X[i]] += weight/(diff + 1);
                else
                    gradient[Y[i]][X[i]] -= weight/(diff + 1) * w1;
                if (f[Y[i]][X[i]] <= 10.0) gradient[Y[i]][X[i]] -= weight;
                else if (f[Y[i]][X[i]] >= 990.0) gradient[Y[i]][X[i]] += weight;
            }
        }
        for (int x = 0; x < L; ++x) {
            for (int y = 0; y < L; ++y) {
                f[x][y] -= alpha * gradient[x][y];
            }
        }
        if(S >= 3 * 3) {
            f[0][0] = lower;
            f[L/2][L/2] = upper;
        }
        if (S >= SPLIT * SPLIT) {
            for(int i = 0; i < dx_v.size(); i++) {
                f[L/2 + dy_v[i]][L/2 + dx_v[i]] = upper;
            }
        }
        for (int x = 0; x < L; ++x) {
            for (int y = 0; y < L; ++y) {
                f[x][y] = max<double>(0.0, min<double>(1000.0, f[x][y]));
            }
        }
        int next_time = timer.get() * 5.0;
        if(prev_time != next_time) {
            alpha *= 0.70;
            prev_time = next_time;
            if (S < 3 * 3) {
                double target = (target_cost * 1.5)/(target_cost*0.5 + placement_cost_all(f));
                weight *= target;
            }
        }
    }
}
void get_temp_ave(int middleY, int middleX, double lower, double upper, int r, std::vector<std::vector<double>>& f) {
    f[middleY][middleX] = lower;
    double alpha = 0.010; // Learning rate
    int loop = 300;
    std::vector<std::vector<double>> gradient(L, std::vector<double>(L, 0));
    while(loop--) {
        for (int x = middleX - r; x < middleX + r; ++x) {
            for (int y = middleY - r; y < middleY + r; ++y) {
                gradient[(x + L) % L][(y + L) % L] = 
                    4 * f[(x + L) % L][(y + L) % L] 
                    - f[(x + 1 + L) % L][(y + L) % L] 
                    - f[(x - 1 + L) % L][(y + L) % L] 
                    - f[(x + L) % L][(y + 1 + L) % L] 
                    - f[(x + L) % L][(y - 1 + L) % L];
            }
        }
        for (int x = middleX - r; x < middleX + r; ++x) {
            for (int y = middleY - r; y < middleY + r; ++y) {
                f[(x + L) % L][(y + L) % L] -= alpha * gradient[(x + L) % L][(y + L) % L];
            }
        }
        f[middleY][middleX] = lower;
        for (int x = middleX - r; x < middleX + r; ++x) {
            for (int y = middleY - r; y < middleY + r; ++y) {
                f[(x + L) % L][(y + L) % L] = max<double>(0.0, min<double>(1000.0, f[(x + L) % L][(y + L) % L]));
            }
        }
    }
}
void input() {
    cin >> L >> N >> S;
    for(int i = 0; i < L; i++) {
        for(int j = 0; j < L; j++) {
            point_num[i][j] = -1;
        }
    }
    for(long long int i = 0; i < N; i++) {
        cin >> Y[i] >> X[i];
        point_num[Y[i]][X[i]] = i;
    }
    
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            predict[i][j] = (i + j) % N;
        }
    }
}

long long int average_temperature(long long int y, long long int x) {
    double t = temp_distribution[y][x] + 2.0;
    for(long long int i = 0; i < dx_v.size(); i++) {
        t += temp_distribution[(y + dy_v[i] + L) % L][(x + dx_v[i] + L) % L];
    }
    return t / (dx_v.size() + 1);
}
const int SPLIT2 = 12;
void temperature_setting() {
#ifdef DEBUG
    timeLimit *= 2;
#endif 
    int D = 0;
    for(int i = 0; i < 100010; i++) fast_exp[i] = exp(i * 0.0001);
    D = min<long long int>(1000/L, S * 2);
    const double D2[12] = {100, 300, 400, 500, 600, 700, 750, 800, 900, 1000, 1000, 1000};
    if (S <= SPLIT2 * SPLIT2) {
        D = min<long long int>(D2[int(sqrt(S)) - 1]/L, S * 2);
    }
    std::vector<std::vector<double>> f(L, std::vector<double>(L, 0));
    int maximam = 0, minimum = 1000;
    for (long long int i = 0; i < L; i++) {
        for(long long int j = 0; j < L; j++) {
            f[i][j] = max<long long int>(0, min<long long int>(1000, min(i * D, (L - i) * D) + min(j * D, (L - j) * D)));
            maximam=max<int>(maximam,f[i][j]);
            minimum=min<int>(minimum,f[i][j]);
        }
    }
    for (long long int i = 0; i < L; i++) {
        for(long long int j = 0; j < L; j++) {
            f[i][j] = f[i][j] + (1000-maximam+minimum)*0.5;
        }
    }
    double weight[30] = {0.40, 0.30, 0.2, 0.07, 0.03, 
                         0, 0, 0, 0, 0,
                         0.0, 0.0, 0.0, 0.50, 0.75,
                         0.75, 0.75, 1.2, 1.2, 1.5,
                         4.0, 4.5, 5.5, 6.5, 8.0,
                         14.0, 20.0, 35.0, 55.0, 85.0,
                         };
    get_temp(f[0][0], f[L/2][L/2],weight[int(sqrt(S)) - 1], f);
    for (long long int i = 0; i < L; i++) {
        for(long long int j = 0; j < L; j++) {
            maximam=max<int>(maximam,f[i][j]);
            minimum=min<int>(minimum,f[i][j]);
        }
    }
    for (long long int i = 0; i < L; i++) {
        for(long long int j = 0; j < L; j++) {
            f[i][j] = f[i][j] + (1000-maximam+minimum)*0.5;
        }
    }
    int prev_cost = 0;
    int count = 0;
    if (S >= 23 * 23) {
        for (long long int i = 0; i < L; i++) {
            for(long long int j = 0; j < L; j++) {
                temp_distribution[i][j] = f[i][j];
            }
        }
        long long int middleY = L/2;
        long long int middleX = L/2;
        temp_distribution[middleY][middleX] = 0;
        for(long long int i = 0; i < dx_v.size(); i++) {
            temp_distribution[middleY + dy_v[i]][middleX  + dx_v[i]] = average_temperature(middleY + dy_v[i], middleX  + dx_v[i]);
        }
    } else if (S > SPLIT2 * SPLIT2) {
        int middleY = L/2;
        int middleX = L/2;
        double lower = max<double>(0.0, f[middleY - 1][middleX - 1] - S * 2.5);
        double upper = f[middleY][middleX - 1];
        if (S < 20 * 20)
            get_temp_ave(middleY, middleX, lower, upper, 3, f);
        else 
            get_temp_ave(middleY, middleX, lower, upper, 2, f);
        for (long long int i = 0; i < L; i++) {
            for(long long int j = 0; j < L; j++) {
                temp_distribution[i][j] = f[i][j];
            }
        }
    } else {
        for (long long int i = 0; i < L; i++) {
            for(long long int j = 0; j < L; j++) {
                temp_distribution[i][j] = f[i][j];
            }
        }
    }
    maximam = 0, minimum = 0;
    for (long long int i = 0; i < L; i++) {
        for(long long int j = 0; j < L; j++) {
            maximam=max<int>(maximam,temp_distribution[i][j]);
            minimum=min<int>(minimum,temp_distribution[i][j]);
        }
    }
    for (long long int i = 0; i < L; i++) {
        for(long long int j = 0; j < L; j++) {
            temp_distribution[i][j] = temp_distribution[i][j] + (1000-maximam+minimum)*0.5;
        }
    }
    for (long long int i = 0; i < L; i++) {
        for(long long int j = 0; j < L; j++) {
            cout << temp_distribution[i][j] << " ";
        }
        cout << endl;
    }
}
double loss_table[110][110];
double loss_table_origin[110][110];
double loss_table2[110][110];
double near_loss_sum(long long int i) {
    long long int count = measure_log[i].size();
    if(count == 0) return -1.0;
    double loss1 = sqrt(loss_table[i][predict[i][1]]/count) / S;
    double loss2 = sqrt(loss_table[i][predict[i][0]]/count) / S;

    double x = 3.8;
    if(S >= 10 * 10) x = 4.0;
    if(S >= 15 * 15) x = 4.3;
    return (loss1 - loss2) - x/count;
}
void search(long long int i) {
    vector<pair<double, int>> v;
    for(long long int j = 0; j < N; j++) {
        v.push_back({loss_table[i][j], j});
    }
    sort(v.begin(), v.end());
    for(long long int j = 0; j < N; j++) {
        predict[i][j] = v[j].second;
    }
}
long long int measure_count = 0;
void state_debug(int i, int move_Y, int move_X) {
    cout << "# turn = " << measure_count << std::endl;
    cout << "# measure_count:" << measure_log[i].size() << std::endl;
    double loss = 0.0;
    for(int j = 0; j < measure_log[i].size(); j++) {
        int measure_t = measure_log[i][j].t;
        int yy = (measure_log[i][j].y + Y[predict[i][0]] + L) % L;
        int xx = (measure_log[i][j].x + X[predict[i][0]] + L) % L;
        int estimate_t = temp_distribution[yy][xx];

        double diff = (temp_distribution[yy][xx] - measure_log[i][j].t);
        loss += diff * diff;
        cout << "# estimate_t:" << temp_distribution[yy][xx] << ", measure:" << measure_log[i][j].t << ", loss:" << loss << std::endl;
    }
    for(int k = 0; k < 8; k++) {
        long long int y = (Y[predict[i][k]] + move_Y + L) % L;
        long long int x = (X[predict[i][k]] + move_X + L) % L;
        long long int estimate_t = temp_distribution[y][x];
        double minimum = 1e+9;
        for(int j = 0; j < N; j++) minimum = min(minimum, loss_table[j][predict[i][k]]);
        cout << "#" << i << " " << predict[i][k] << ", estimate: " << estimate_t;
        cout << ", loss = " << sqrt(loss_table[i][predict[i][k]])/S/measure_log[i].size();
        cout << ", " << loss_table[i][predict[i][k]];
        cout << ", " << minimum  << std::endl;
    }
    cout << "# loss_sum:" << near_loss_sum(i) << endl;
}

Measure_Log measure(long long int i, long long int move_Y, long long int move_X) {
    long long int t;
#ifdef DEBUG
    state_debug(i, move_Y, move_X);
#endif
    cout << i << " " << move_Y << " " << move_X << endl;
    cin >> t;
    measure_count++;
    Measure_Log tmp = Measure_Log(i, move_Y, move_X, t);
    for(long long int j = 0; j < N; j++) {
        const int temp = temp_distribution[(Y[j] + move_Y + L) % L][(X[j] + move_X + L) % L];
        double diff = temp - t;
        double add_loss = 0.0;
        double dist = min((1000.0 - temp)/S, (double)temp/S);
        loss_table[i][j] += diff * diff;
        loss_table2[i][j] += diff;
        loss_table_origin[i][j] += diff * diff;
        if(abs(diff/S) > 3.3) {
            double add_cost =  diff * diff * (fast_exp[int(min(10.0, abs(diff/S) - 3.3) * 10000)] - 1);
            loss_table[i][j] += add_cost;
            loss_table_origin[i][j] += add_cost;
        }
        
    }
    search(i);
    return tmp;
}

Timer time_;
int prev_predict[110][110];
pair<int, int> prev_search[110];
pair<int, int> next_measure_search(int i) {
    long long int max_diff = -1;
    pair<int, int> ret;
    double rS= 1.0/S;
    int range = 6;
    if(S >= 20 * 20) range = 3;
    const int r = L/2;
    if(prev_predict[i][0] == 0 && prev_predict[i][1] == 0) {
        for(int j = 0; j < range; j++) {
            prev_predict[i][j] = predict[i][j];
        }
    } else {
        bool ok = true;
        for(int k = 0; k < range; k++) {
            if(prev_predict[i][k] != predict[i][k]) {
                ok = false;
                break;
            }
        }
        if(ok) {
            return prev_search[i];
        }
        for(int j = 0; j < range; j++) {
            prev_predict[i][j] = predict[i][j];
        }
    }
    double diff[51][51] = {};
    double first_diff[51][51] = {};
    bool nearly = false;
    if (sqrt(loss_table[i][predict[i][1]]) * rS - sqrt(loss_table[i][predict[i][0]]) * rS <= 0.5) {
        nearly = true;
    }
    for(int k = 1; k < range; k++) {
        if (k >= 2 && sqrt(loss_table[i][predict[i][k]]) * rS - sqrt(loss_table[i][predict[i][0]]) * rS >= 4.0) {
            break;
        }
        double weight1 = 0.5;
        if (k == 1) weight1 = 1.0;
        else if (loss_table[i][predict[i][k]] > 0)
            weight1 = (double)(loss_table[i][predict[i][1]])/loss_table[i][predict[i][k]];
        for(int y = -r; y <= r; y++) {
            for(int x = -r; x <= r; x++) {
                int y11 = (Y[predict[i][0]] + y + L) % L;
                int x11 = (X[predict[i][0]] + x + L) % L;
                int y21 = (Y[predict[i][1]] + y + L) % L;
                int x21 = (X[predict[i][1]] + x + L) % L;
                int y2 = (Y[predict[i][k]] + y + L) % L;
                int x2 = (X[predict[i][k]] + x + L) % L;
                int diff1 = temp_distribution[y11][x11] - temp_distribution[y2][x2];
                int diff2 = temp_distribution[y21][x21] - temp_distribution[y2][x2];
                double weight = weight1;
                diff1 = min<double>(max(5.5, S * 4.5), abs(diff1));
                if(abs(diff1)*rS > 4.0) {
                    weight *= (fast_exp[int(min(10.0, abs(diff1/S) - 4.0) * 10000)]);
                }
                if (k == 1) first_diff[y+r][x+r] = diff[y+r][x+r];
                diff[y+r][x+r] +=  diff1 * diff1 * weight * 6.0 / (k + 5.0);
                if (k >= 2 && nearly) {
                    diff[y+r][x+r] += min<double>(first_diff[y+r][x+r] * 0.50, diff2 * diff2 * weight * 6.0 / (k + 5.0));
                }
            }
        }
    }
    if (S <= 25 * 25) {
        for(int y = -r; y <= r; y++) {
            for(int x = -r; x <= r; x++) {
                diff[y+r][x+r] /= 10 + abs(x) + abs(y);
            }
        }
    }
    for(int y = -r; y <= r; y++) {
        for(int x = -r; x <= r; x++) {
            if (diff[y+r][x+r] > max_diff) {
                max_diff = diff[y+r][x+r];
                ret = {y, x};
            }
        }
    }
    prev_search[i] = ret;
    return ret;
}
bool is_check[110];
bool is_ok() {
    long long int num = 0;
    for(long long int i = 0; i < N; i++) {
        num += is_check[i];
    }
    return num == N;
}
double rate;
void fixed(long long int i) {
    if(!is_check[i]) {
#ifdef DEBUG
        state_debug(i, 0, 0);
#endif
        is_check[i] = true;
        double weight;
        if (S <= 6 * 6) weight = 1000;
        else if (S <= 12 * 12) weight = 15.0;
        else if (S <= 20 * 20) weight = 10.0;
        else weight = 5.0;
        // if (S >= 25 * 25 && measure_log[i].size() <= 25) weight *= 1.5;
        // if (S >= 25 * 25 && measure_log[i].size() >= 250) weight *= 0.6;
        for(int j = 0; j < N; j++) loss_table[j][predict[i][0]] += S * S * weight;
        //std::cerr << i << " " <<  measure_log[i].size() << std::endl;

    }
}
void output() {
    for(long long int i = 0; i < N; i++) {
        cout << "#" << i << " " << predict[i][0] << " " << loss_table[i][predict[i][0]] << ", is_check:" << is_check[i] << endl;
        std::vector<pair<long long int, long long int>> v;
        for(long long int j = 0; j < N; j++) {
            long long int next_loss = -loss_table[i][predict[i][0]] + loss_table[i][j];
            v.push_back({next_loss, j});
        }
        sort(v.begin(), v.end());
        cout << "#" << i << " " << predict[i][0] << ", loss_sum:" << near_loss_sum(i) << ": ";
        for(long long int j = 0; j < N; j++) cout << v[j].first << "," << v[j].second << " ";
        cout << endl;
    }
    
    cout << "-1 -1 -1" << endl;
    for(long long int i = 0; i < N; i++) {
        cout << predict[i][0] << endl;
    }
    
}
vector<int> more_search;
void all_search() {
    vector<vector<long long int>> cost(N, vector<long long int>(N)); // コスト行列

    // コスト行列の入力
    for (long long int i = 0; i < N; ++i) {
        for (long long int j = 0; j < N; ++j) {
            cost[i][j] = sqrt(loss_table[i][j] / sqrt(measure_log[i].size()));
        }
    }
    Hungarian<long long int> solver(cost);
    pair<long long int, vector<long long int>> result = solver.solve();
    
    cout << "# Minimum cost: " << result.first << endl;
    cout << "# Allocation:";
    int count_[110] = {};
    for (long long int i = 0; i < result.second.size(); i++) {
        cout << " " << result.second[i];
        if (predict[i][0] != result.second[i]) {
            std::cerr << "more search!" << i << " " << predict[i][0] << " " << result.second[i] << std::endl;
            is_check[i] = false;
        }
        predict[i][0] = result.second[i];
    }
    cout << endl;
}


void best_search() {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            loss_table[i][j] = loss_table_origin[i][j];
        }
    }
    int count_[110] = {};
    for(int i = 0; i < N; i++) count_[predict[i][0]]++;
    for(int i = 0; i < N; i++) if (count_[predict[i][0]] >= 2) {
        more_search.push_back(i);
        is_check[i] = false;
    }
    all_search();
    double weight = 0.10;
    if (S <= 15 * 15) weight = 0.15;
    for(int i = 0; i < N; i++) {
        for(int j = i + 1; j < N; j++) {
            double loss = 0.0;
            loss -= loss_table[i][predict[i][0]] * sqrt(measure_log[i].size());
            loss += loss_table[i][predict[j][0]] * sqrt(measure_log[i].size());
            loss -= loss_table[j][predict[j][0]] * sqrt(measure_log[j].size());
            loss += loss_table[j][predict[i][0]] * sqrt(measure_log[j].size());
            if(loss < weight * S * S) {
                is_check[i] = false;
                is_check[j] = false;
            }
        }
    }
    for(int i = 0; i < N; i++) {
        if(is_check[i]) {
            is_check[i] = false;
            fixed(i);
            is_check[i] = false;
            fixed(i);
        } else {
            more_search.push_back(i);
        }
    }
    if(more_search.size() > 0) {
        while(measure_count < 10000 && timer.get() < timeLimit + 0.40) {
            vector<int> next_more_search;
            for(int j = 0; j < more_search.size(); j++) {
                const int i = more_search[j];
                if (is_check[i]) continue;
                auto tmp = next_measure_search(i);
                measure_log[i].push_back(measure(i, tmp.first, tmp.second));
                double loss = near_loss_sum(i) - 0.1/measure_log[i].size();
                if (loss < rate) next_more_search.push_back(i);
                else {
                    search(i);
                    fixed(i);
                }
                if (measure_count >= 10000) break;
            }
            if (next_more_search.size() == 0) break;
            more_search = next_more_search;
            int count2_[110] = {};
            for(int i = 0; i < N; i++) count2_[predict[i][0]]++;
            for(int i = 0; i < N; i++) if (count2_[predict[i][0]] >= 2) {
                more_search.push_back(i);
                is_check[i] = false;
            }
        }
    }
}
signed main(void)
{
    cin.tie(0);
    ios::sync_with_stdio(false);
    
    input();
    temperature_setting();
    
    rate = 0.0;
    for(int i = 0; i < N; i++) {
        measure_log[i].push_back(measure(i, 0, 0));
        double prev_loss = near_loss_sum(i);
            if (prev_loss >= rate + 0.1/measure_log[i].size()) {
                fixed(i);
                continue;
            }
    }
    do{
        if (timer.get() > timeLimit)
            break;
        vector<pair<double, int>> v;
        for(int i = 0; i < N; i++) {
            if (is_check[i]) continue;
            search(i);
            v.push_back({near_loss_sum(i), i});
        }
        sort(v.begin(), v.end(),greater<pair<double, int>>());
        int range = N/35.0 + 2.0;
        if (S >= 20 * 20) range = N/20.0 + 3.5;
        for(long long int j = 0; j < min<int>(v.size(), range); j++) {
            int i = v[j].second;
            if(measure_count >= 10000)
                break;
            if (timer.get() > timeLimit)
                break;
            if (is_check[i]) continue;
            search(i);
            double prev_loss = near_loss_sum(i);
            pair<long long int, long long int> tmp;
            tmp = next_measure_search(i);
            int y = tmp.first;
            int x = tmp.second;
            measure_log[i].push_back(measure(i, y, x));
            double next_loss = near_loss_sum(i);
            if (next_loss >= rate) {
                fixed(i);
                break;
                continue;
            }
            if(measure_count >= 10000)
                break;
        }
        for(int i = 0; i < N; i++) {
            if (is_check[i]) continue;
            search(i);
            double prev_loss = near_loss_sum(i);
            if (prev_loss >= rate) {
                fixed(i);
                continue;
            }
        }
    }while(measure_count < 10000 && !is_ok() && timer.get() < timeLimit);
    best_search();
    output();
    
    
    return 0;
}
