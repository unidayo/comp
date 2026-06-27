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
#include <cassert>
#include <iomanip>
using namespace std;
#define ll long long int
#define rep(i,n) for( int i = 0; i < n; i++ )
#define rrep(i,n) for( int i = n; i >= 0; i-- )
#define REP(i,s,t) for( int i = s; i <= t; i++ )
#define RREP(i,s,t) for( int i = s; i >= t; i-- )
#define dump(x)  cerr << #x << " = " << (x) << endl;
#define INF 200000000
#define mod 1000000007
#define INF2 1000000000000000000
typedef pair<int, int> P;
typedef pair<int, P> PP;
const int Q = 1000;
const int N = 30;
const int dx[4] = {0, 1, 0, -1};
const int dy[4] = {1, 0, -1, 0};
int len_predict[31][31][2];
int d[31][31];
int visited_cnt[31][31][2];
P pre_v[31][31];


int correction_len_predict[31][31][2];
unsigned int randInt() {
    static unsigned int tx = 113456789, ty=362436069, tz=521288629, tw=88675123;
    unsigned int tt = (tx^(tx<<11));
    tx = ty; ty = tz; tz = tw;
    return ( tw=(tw^(tw>>19))^(tt^(tt>>8)) );
}

void dijkstra(int sx, int sy, int noise = 0, int visited_p = 0) {
    rep(i, N + 1) rep(j, N + 1) d[i][j] = INF;
    d[sx][sy] = 0;
    rep(i, N + 1) rep(j, N + 1) pre_v[i][j] = {-1, -1};
    priority_queue<PP, vector<PP>, greater<PP>> que;
    que.push({d[sx][sy], {sx, sy}});
    rep(i, N) {
        rep(j, N) {
            rep(k, 2) {
                correction_len_predict[i][j][k] = min<int>(9000, len_predict[i][j][k] + sqrt(visited_cnt[i][j][k]) * visited_p);
            }
        }
    }
    while(que.size()) {
        PP p = que.top(); que.pop();
        int x = p.second.first;
        int y = p.second.second;
        if(d[x][y] < p.first) continue;
        rep(k, 4) {
            int xx = x + dx[k];
            int yy = y + dy[k];
            if(xx < 0 || xx >= N || yy < 0 || yy >= N) continue;
            int add = 0;
            if(k == 0) add = correction_len_predict[x][y][1];
            else if(k == 1) add = correction_len_predict[x][y][0];
            else if(k == 2) add = correction_len_predict[x][y - 1][1];
            else add = correction_len_predict[x - 1][y][0];
            if(d[xx][yy] > d[x][y] + add) {
                d[xx][yy] = d[x][y] + add;
                que.push({d[xx][yy], {xx, yy}});
                pre_v[xx][yy] = {x, y};
            }
        }
    }
}

string get_path(int gx, int gy) {
    string path;
    while(true) {
        //cerr << gx << " " << gy << endl;
        P tmp = pre_v[gx][gy];
        if(tmp.first == -1) break;
        if(tmp.first + 1 == gx) {
            path += 'R';
        } else if(tmp.first - 1 == gx) {
            path += 'L';
        } else if(tmp.second + 1 == gy) {
            path += 'D';
        } else {
            path += 'U';
        }
        gx = tmp.first;
        gy = tmp.second;
        
    }
    reverse(path.begin(), path.end());
    return path;
}
vector<PP> get_path2(int gx, int gy) {
    vector<PP> path;
    while(true) {
        P tmp = pre_v[gx][gy];
        if(tmp.first == -1) break;
        rep(k, 4) {
            if(tmp.first + dx[k] == gx && tmp.second + dy[k] == gy) {
                if(k == 0) path.push_back({1, tmp});
                if(k == 1) path.push_back({0, tmp});
                if(k == 2) path.push_back({1, {tmp.first, tmp.second - 1}});
                if(k == 3) path.push_back({0, {tmp.first - 1, tmp.second}});
            }
        }
        gx = tmp.first;
        gy = tmp.second;
        
    }
    reverse(path.begin(), path.end());
    return path;
}
int true_len1[31][31];
int sx[1000], sy[1000], gx[1000], gy[1000];
int true_len[1000];
string path[1000];
vector<vector<PP>> p(1000);
int D;
int calc_path_cost(int i) {
    int ret = 0;
    for(auto e: p[i]) {
        ret += len_predict[e.second.first][e.second.second][e.first];
    }
    return ret;
}

int path_error(int last) {
    int ret = 0;
    for(int i = 0; i < last; i++) {
        ret += abs(true_len[i] - calc_path_cost(i));
    }
    return ret;
}

int prev_len_predict[31][31][2];
void save() {
    rep(i, N) rep(j, N) rep(k, 2) prev_len_predict[i][j][k] = len_predict[i][j][k];
}
void recall() {
    rep(i, N) rep(j, N) rep(k, 2) len_predict[i][j][k] = prev_len_predict[i][j][k];
}
void update_predict_path(int i, double r) {
    const int diff = true_len[i]/path[i].size() - calc_path_cost(i)/path[i].size();
    for(auto e: p[i]) {
        int x = e.second.first;
        int y = e.second.second;
        len_predict[x][y][e.first] += (int)(diff * r);
        visited_cnt[x][y][e.first]++;
        len_predict[x][y][e.first] = min(9000 - D, max(1000 + D, len_predict[x][y][e.first]));
    }
}

void update_predict_path_after(int last, double r) {
    for(int i = 0; i <= last; i++) {
        const int diff = double(true_len[i]- calc_path_cost(i))/path[i].size();
        bool is_skip = i >= 250 && (abs(diff) <= randInt() % 230 + 20);
        //if(i == last - 1) cerr << i << " " << diff << " " << is_skip << endl;
        if(is_skip) continue; 
        
        for(auto e: p[i]) {
            const int x = e.second.first;
            const int y = e.second.second;
            len_predict[x][y][e.first] += (int)(diff * r * 1);
            len_predict[x][y][e.first] = min(9000 - D, max(1000 + D, len_predict[x][y][e.first]));
        }
    }
}
bool is_confirm;
bool is_separate;
bool is_not_separate;
void update_averate_path0(int j, double r) {
    double c = 0.7;
    int range = 900;
    int d = 4;
    if(is_not_separate) {range = 1500; d = 1;}
    if(is_separate) {range = 600; d = 4;}
    for(int i = 0; i < N - 2; i++) {
        const int a = len_predict[i][j][0];
        const int b = len_predict[i + 1][j][0];
        rep(k, d) if(abs(a - b) >= range * (k + 1)) r = r * c;
        if(is_separate && abs(a - b) > 3000) r = 0.05;
        len_predict[i][j][0] = min<int>(9000 - D, max<int>(1000 + D, a * (1 - r) + b * r));
        len_predict[i + 1][j][0] = min<int>(9000 - D, max<int>(1000 + D, b * (1 - r) + a * r));
    }
    for(int i = 0; i < N - 2; i++) {
        const int a = len_predict[j][i][1];
        const int b = len_predict[j][i + 1][1];
        rep(k, d) if(abs(a - b) >= range * (k + 1)) r = r * c;
        if(is_separate && abs(a - b) > 3000) r = 0.05;
        len_predict[j][i][1] = min<int>(9000 - D, max<int>(1000 + D, a * (1 - r) + b * r));
        len_predict[j][i + 1][1] = min<int>(9000 - D, max<int>(1000 + D, b * (1 - r) + a * r));
    }
    for(int i = N - 2; i >= 0; i--) {
        const int a = len_predict[i][j][0];
        const int b = len_predict[i + 1][j][0];
        rep(k, d) if(abs(a - b) >= range * (k + 1)) r = r * c;
        if(is_separate && abs(a - b) > 3000) r = 0.05;
        len_predict[i][j][0] = min<int>(9000 - D, max<int>(1000 + D, a * (1 - r) + b * r));
        len_predict[i + 1][j][0] = min<int>(9000 - D, max<int>(1000 + D, b * (1 - r) + a * r));
    }
    for(int i = N - 2; i >= 0; i--) {
        const int a = len_predict[j][i][1];
        const int b = len_predict[j][i + 1][1];
        rep(k, d) if(abs(a - b) >= range * (k + 1)) r = r * c;
        if(is_separate && abs(a - b) > 3000) r = 0.05;
        len_predict[j][i][1] = min<int>(9000 - D, max<int>(1000 + D, a * (1 - r) + b * r));
        len_predict[j][i + 1][1] = min<int>(9000 - D, max<int>(1000 + D, b * (1 - r) + a * r));
    }
}
int separate_a[2][31];

void update_average_path(int process, int dir, int separate, int j, double r) {
    int sum[31] = {};
    int weight[31] = {};
    if(dir == 0) rep(i, N - 1) sum[i + 1] += len_predict[i][j][0] + sum[i];
    else rep(i, N - 1) sum[i + 1] += len_predict[j][i][1] + sum[i];
    
    if(!is_separate && !separate && (is_not_separate || process <= 200)) {
        if(is_not_separate) r *= 8;
        if(dir == 0) rep(i, N - 1) len_predict[i][j][0] = min<int>(9000 - D, max<int>(1000 + D, len_predict[i][j][0] * (1.0 - r) + sum[N - 1]/(N - 1) * r));
        else rep(i, N - 1) len_predict[j][i][1] = min<int>(9000 - D, max<int>(1000 + D, len_predict[j][i][1] * (1.0 - r) + sum[N - 1]/(N - 1) * r));
        return;
    }  
    if(is_separate && process > 600 && randInt() % 2 == 0 && separate_a[dir][j] != -1) {
        int max_idx = separate_a[dir][j];
        r *= 0.3;
        if(dir == 0) {
            if(max_idx > 18) rep(i, max_idx - 3) len_predict[i][j][0] = min<int>(9000 - D, max<int>(1000 + D, len_predict[i][j][0] * (1.0 - r)  + sum[max_idx - 2]/(max_idx - 2) * r));
            else REP(i, max_idx + 3, N - 2) len_predict[i][j][0] = min<int>(9000 - D, max<int>(1000 + D, len_predict[i][j][0] * (1.0 - r)  + (sum[N - 1] - sum[max_idx + 1]) / (N - 1 - (max_idx + 1)) * r));
        } else {
            if(max_idx > 18) rep(i, max_idx - 3) len_predict[j][i][1] = min<int>(9000 - D, max<int>(1000 + D, len_predict[j][i][1] * (1.0 - r)  + sum[max_idx + 4]/(max_idx + 4) * r));
            else REP(i, max_idx + 3, N - 2) len_predict[j][i][1] = min<int>(9000 - D, max<int>(1000 + D, len_predict[j][i][1] * (1.0 - r)  + (sum[N - 1] - sum[max_idx + 4]) / (N - 1 - (max_idx + 4)) * r));
        }
    } 
    double dist[15];
    //rep(i, 15) dist[i] =  max(0.01, 1.0 - pow(i, 1.5)/16.0);
    rep(i, 15) dist[i] =  max(0.01, 1.0 - i/8.0);
    int range = 5;
    if(dir == 0) {
        rep(i, N - 1) {
            for(int c = max(0, i - range); c < min(N - 1, i + range); c++) {
                double w = min<double>(1.0, visited_cnt[i][j][0]/4.0) * dist[abs(i - c)];
                len_predict[c][j][0] = len_predict[c][j][0] * (1.0 - r * w) + len_predict[i][j][0] * r * w;
            }
        }
    } else {
        rep(i, N - 1) {
            for(int c = max(0, i - range); c < min(N - 1, i + range); c++) {
                double w = min<double>(1.0, visited_cnt[j][i][1]/4.0) * dist[abs(i - c)];
                len_predict[j][c][1] = len_predict[j][c][1] * (1.0 - r * w) + len_predict[j][i][1] * r * w;
            }
        }
    }
    
}
const int64_t CYCLES_PER_SEC=3000000000;
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
int separate_num() {
    int ret = 0;
    rep(dir, 2) {
        rep(j, N) {
            int sum[31] = {};
            if(dir == 0) rep(i, N - 1) sum[i + 1] += len_predict[i][j][0] + sum[i];
            else rep(i, N - 1) sum[i + 1] += len_predict[j][i][1] + sum[i];
            int max_idx = 0;
            int max_diff = 0;
            int diff[31]; 
            
            rep(i, N - 2) {
                int L = sum[i + 1] / (i + 1);
                int R = (sum[N - 1] - sum[i + 1]) / (N - 1 - (i + 1));
                diff[i] = abs(R - L);
            }
            rep(i, N - 2) {
                if(max_diff <= diff[i]) {
                    max_idx = i;
                    max_diff = diff[i];
                }
            }
            if(max_diff > 2500) separate_a[dir][j] = max_idx;
            else separate_a[dir][j] = -1;
            ret += max_diff;
        }
    }
    return ret;
}
bool is_separate2 = true;
int main(void)
{
    cin.tie(0);
    ios::sync_with_stdio(false);

    rep(i, N) rep(j, N) rep(k, 2) len_predict[i][j][k] = 3000 + (min(i, 29 - i) * min(j, 29 - j)) * 15;
    rep(i, Q) {
        D = max(100, 1000 - i * 5);
        cin >> sy[i] >> sx[i] >> gy[i] >> gx[i];
        dijkstra(sx[i], sy[i], 0, max((i <= (150 + is_separate * 50)) * 20, (100 - i)*3));
        if(i == 100) {const int num = separate_num(); cerr <<"1:" << num << endl; if(num <= 22000) is_not_separate = true; if(num >= 37000) is_separate = true;}
        if(i == 150) {const int num = separate_num(); cerr <<"1:" << num << endl; if(num <= 30000) is_not_separate = true; if(num >= 40000) is_separate = true;}
        if(i == 200) {const int num = separate_num(); cerr <<"2:" << num << endl; if(num <= 35000) is_not_separate = true; if(num >= 45000) is_separate = true;}
        if(i >= 600 && i % 100 == 0 && is_separate) separate_num();
        //if(i == 400) {const int num = separate_num(); cerr <<"2:" << num << endl;  if(num >= 45000) is_separate = true;}
        path[i] = get_path(gx[i], gy[i]);
        cout << path[i] << endl;
        cin >> true_len[i];
        p[i] = get_path2(gx[i], gy[i]);
        //cerr << i << " " << double(true_len[i] - calc_path_cost(i))/path[i].size() << " " << path[i].size() << endl;
        update_predict_path(i, 0.8);
        update_predict_path_after(i, 0.1);
        // for(int j = 0; j < N; j++) {
        //     update_averate_path0(j, 0.05 + (is_not_separate) * 0.05 - (is_separate) * 0.01);
        // }
        double r = 1.5;
        for(int l = 0; l < 20; l++) {
            if(i % 3 != 0) break;
            if(timer.get() > 1.8 && l >= 10) break;
            update_predict_path_after(i, 0.1 * r);
            for(int j = 0; j < N; j++) {
                update_averate_path0(j, 0.03 + (is_not_separate) * 0.06);
                update_average_path(i, 0, l % 2 != 0, j, r * (0.04 + (is_not_separate) * 0.02 - (is_separate) * 0.01));
            }
            update_predict_path_after(i, 0.1 * r);
            for(int j = 0; j < N; j++) {
                update_averate_path0(j, 0.03 + (is_not_separate) * 0.06);
                update_average_path(i, 1, l % 2 != 0, j, r * (0.04 + (is_not_separate) * 0.02 - (is_separate) * 0.01));
                 
            }
            update_predict_path_after(i, 0.1 * r);
            for(int j = 0; j < N; j++) {
                update_averate_path0(j, r * (0.03 + (is_not_separate) * 0.05 - (is_separate) * 0.01));
            }
            r *= 0.95;
        }
    }
    cerr << timer.get() << endl;
    int sum = 0;
    // rep(j, Q) {
    //     sum += abs(calc_path_cost(j) - true_len[j]);
    //     cerr << calc_path_cost(j) << " " << true_len[j] << " " << sum << endl;
    // }
    // rep(j, N) {
    //  //   cerr << i << ": ";
    //     rep(i, N - 1) {
    //         cerr << len_predict[i][j][0] << " ";
    //     }
    //     cerr << endl;
    // }
    // rep(j, N - 1) {
    //     rep(i, N) {
    //         cerr << len_predict[i][j][1] << " ";
    //     }
    //     cerr << endl;
    // }

    return 0;
}
