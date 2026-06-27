#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
#pragma GCC target("avx2")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3")
#include <bits/stdc++.h>
using namespace std;

const int INF = 1e9;
const int MOD = 998244353;
const long long LINF = 4e18;

using ll = long long;
using vi = vector<int>;
using vl = vector<long long>;
using vs = vector<string>;
using vc = vector<char>;
using vb = vector<bool>;
using vvi = vector<vector<int>>;
using vvvi = vector<vector<vector<int>>>;
using vvvvi = vector<vector<vector<vector<int>>>>;
using vvl = vector<vector<long long>>;
using vvvl = vector<vector<vector<long long>>>;
using vvvvl = vector<vector<vector<vector<long long>>>>;
using vvc = vector<vector<char>>;
using vvb = vector<vector<bool>>;
using vvvb = vector<vector<vector<bool>>>;
using vvvvb = vector<vector<vector<vector<bool>>>>;

#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define dump(x)  cout << #x << " = " << (x) << endl;
#define Yes(n) cout << ((n) ? "Yes" : "No"  ) << endl
#define ALL(obj) (obj).begin(),(obj).end()

int n, m;
int pos_x[40], pos_y[40];

int dx[4] = {1, 0, -1, 0};
int dy[4] = {0, 1, 0, -1};
char dc[4] = {'D', 'R', 'U', 'L'};
vector<pair<char, char>> calc_ans(vvb will_stone){
    vvb use_block(n, vb(n, false));
    vector<pair<char, char>> ret;
    vvb stone(n, vb(n, false));
    for(int i = 1;i < m;i++){
        int sx = pos_x[i - 1], sy = pos_y[i - 1];
        int tx = pos_x[i], ty = pos_y[i];
        vvi dist(n, vi(n, INF));
        vector<vector<pair<int,int>>> prev(n, vector<pair<int,int>>(n, {-1, -1}));
        queue<pair<int,int>> que;
        dist[sx][sy] = 0;
        que.push({sx, sy});
        while(!que.empty()){
            auto [x, y] = que.front();
            que.pop();
            rep(j,4){
                int nx = x + dx[j];
                int ny = y + dy[j];
                if(0 <= nx && nx < n && 0 <= ny && ny < n && stone[nx][ny] == false && dist[nx][ny] == INF){
                    dist[nx][ny] = dist[x][y] + 1;
                    prev[nx][ny] = {x, y};
                    que.push({nx, ny});
                }
            }
            rep(j,4){
                int cur = 1;
                while(0 <= x + dx[j]*cur && x + dx[j]*cur < n && 0 <= y + dy[j]*cur && y + dy[j]*cur < n && stone[x + dx[j]*cur][y + dy[j]*cur] == false) cur++;
                cur--;
                if(dist[x + dx[j]*cur][y + dy[j]*cur] == INF){
                    dist[x + dx[j]*cur][y + dy[j]*cur] = dist[x][y] + 1;
                    prev[x + dx[j]*cur][y + dy[j]*cur] = {x, y};
                    que.push({x + dx[j]*cur, y + dy[j]*cur});
                }
            }
            if(dist[tx][ty] != INF) break;
        }
        vector<pair<char, char>> cur_ans;
        int x = tx, y = ty;
        vvb dame(n, vb(n, false));
        while(1){
            rep(j,4){
                int nx = x + dx[j], ny = y + dy[j];
                if(0 <= nx && nx < n && 0 <= ny && ny < n && !stone[nx][ny] && will_stone[nx][ny] && !dame[nx][ny]){
                    cur_ans.push_back({'A', dc[j]});
                    stone[nx][ny] = true;
                }
            }
            if(x == sx && y == sy) break;
            if(prev[x][y] == make_pair(-1, -1)){
                return {};
            }
            dame[x][y] = true;
            if(abs(prev[x][y].first - x) + abs(prev[x][y].second - y) == 1){
                if(prev[x][y].first + 1 == x) cur_ans.push_back({'M', 'D'});
                if(prev[x][y].first - 1 == x) cur_ans.push_back({'M', 'U'});
                if(prev[x][y].second + 1 == y) cur_ans.push_back({'M', 'R'});
                if(prev[x][y].second - 1 == y) cur_ans.push_back({'M', 'L'});
            }else{
                if(prev[x][y].first < x){
                    cur_ans.push_back({'S', 'D'});
                    if(x != n - 1) use_block[x + 1][y] = true;
                    for(int j = prev[x][y].first;j < x;j++) dame[j][y] = true;
                }
                if(prev[x][y].first > x){
                    cur_ans.push_back({'S', 'U'});
                    if(x != 0) use_block[x - 1][y] = true;
                    for(int j = prev[x][y].first;j > x;j--) dame[j][y] = true;
                }
                if(prev[x][y].second < y){
                    cur_ans.push_back({'S', 'R'});
                    if(y != n - 1) use_block[x][y + 1] = true;
                    for(int j = prev[x][y].second;j < y;j++) dame[x][j] = true;
                }
                if(prev[x][y].second > y){
                    cur_ans.push_back({'S', 'L'});
                    if(y != 0) use_block[x][y - 1] = true;
                    for(int j = prev[x][y].second;j > y;j--) dame[x][j] = true;
                }
            }
            
            auto [nx, ny] = prev[x][y];
            x = nx, y = ny;
        }
        reverse(ALL(cur_ans));
        for(auto tmp : cur_ans) ret.push_back(tmp);
    }
    int not_use = 0;
    rep(i,n)rep(j,n)if(will_stone[i][j] && !use_block[i][j]) will_stone[i][j] = false;
    if(not_use != 0) return calc_ans(will_stone);
    return ret;
}

int calc_score(vvb& will_stone){
    int ret = 0;
    vvb stone(n, vb(n, false));
    for(int i = 1;i < m;i++){
        int sx = pos_x[i - 1], sy = pos_y[i - 1];
        int tx = pos_x[i], ty = pos_y[i];
        vvi dist(n, vi(n, INF));
        vector<vector<pair<int,int>>> prev(n, vector<pair<int,int>>(n, {-1, -1}));
        queue<pair<int,int>> que;
        dist[sx][sy] = 0;
        que.push({sx, sy});
        while(!que.empty()){
            auto [x, y] = que.front();
            que.pop();
            rep(j,4){
                int nx = x + dx[j];
                int ny = y + dy[j];
                if(0 <= nx && nx < n && 0 <= ny && ny < n && stone[nx][ny] == false && dist[nx][ny] == INF){
                    dist[nx][ny] = dist[x][y] + 1;
                    prev[nx][ny] = {x, y};
                    que.push({nx, ny});
                }
            }
            rep(j,4){
                int cur = 1;
                while(0 <= x + dx[j]*cur && x + dx[j]*cur < n && 0 <= y + dy[j]*cur && y + dy[j]*cur < n && stone[x + dx[j]*cur][y + dy[j]*cur] == false) cur++;
                cur--;
                if(dist[x + dx[j]*cur][y + dy[j]*cur] == INF){
                    dist[x + dx[j]*cur][y + dy[j]*cur] = dist[x][y] + 1;
                    prev[x + dx[j]*cur][y + dy[j]*cur] = {x, y};
                    que.push({x + dx[j]*cur, y + dy[j]*cur});
                }
            }
            if(dist[tx][ty] != INF) break;
        }
        int x = tx, y = ty;
        vvb dame(n, vb(n, false));
        while(1){
            rep(j,4){
                int nx = x + dx[j], ny = y + dy[j];
                if(0 <= nx && nx < n && 0 <= ny && ny < n && !stone[nx][ny] && will_stone[nx][ny] && !dame[nx][ny]){
                    stone[nx][ny] = true;
                    ret += 1;
                }
            }
            if(x == sx && y == sy) break;
            if(prev[x][y] == make_pair(-1, -1)){
                return {};
            }
            dame[x][y] = true;
            if(abs(prev[x][y].first - x) + abs(prev[x][y].second - y) == 1){
                ret++;
            }else{
                ret++;
                if(prev[x][y].first < x){
                    for(int j = prev[x][y].first;j < x;j++) dame[j][y] = true;
                }
                if(prev[x][y].first > x){
                    for(int j = prev[x][y].first;j > x;j--) dame[j][y] = true;
                }
                if(prev[x][y].second < y){
                    for(int j = prev[x][y].second;j < y;j++) dame[x][j] = true;
                }
                if(prev[x][y].second > y){
                    for(int j = prev[x][y].second;j > y;j--) dame[x][j] = true;
                }
            }
            
            auto [nx, ny] = prev[x][y];
            x = nx, y = ny;
        }
    }
    return ret;
}

unsigned long xor128(void){
	static unsigned long x = 123456789,y = 362436069,z = 541288629,w = 88675123;
	unsigned long t;
	t = (x^(x<<11));
	x = y;y = z;z = w;
	return(w = (w^(w>>19))^(t^(t>>8)));
}

bool shift(double start_temp,double end_temp,double time_limit,double start_time,double scoredist,double now_time){
    long long INF = 1e18;
    double temp = start_temp + (end_temp - start_temp) * (now_time-start_time) / time_limit;  //線形でstart_tempからend_tempに変化する。
    double prob = exp(((double)scoredist)/temp); //scoredistが正のときは1負のときは1未満
    return (prob > (xor128()%INF)/(double)INF);
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    rep(i,1) xor128();
    cin >> n >> m;
    rep(i,m) cin >> pos_x[i] >> pos_y[i];
    vvb will_stone(n, vb(n, false));
    vvb on_dir(n, vb(n, false));
    rep(i,m) on_dir[pos_x[i]][pos_y[i]] = true;
    vector<pair<int,int>> cand;
    rep(i,m - 1){
        int sx = pos_x[i], sy = pos_y[i];
        int tx = pos_x[i + 1], ty = pos_y[i + 1];
        if(sx < tx){
            if(tx + 1 < n && !on_dir[tx + 1][sy]) cand.push_back({tx + 1, sy});
            if(tx + 1 < n && !on_dir[tx + 1][ty]) cand.push_back({tx + 1, ty});
        }
        if(sx > tx){
            if(0 <= tx - 1 && !on_dir[tx - 1][sy]) cand.push_back({tx - 1, sy});
            if(0 <= tx - 1 && !on_dir[tx - 1][ty]) cand.push_back({tx - 1, ty});
        }
        if(sy < ty){
            if(ty + 1 < n && !on_dir[sx][ty + 1]) cand.push_back({sx, ty + 1});
            if(ty + 1 < n && !on_dir[tx][ty + 1]) cand.push_back({tx, ty + 1});
        }
        if(ty < sy){
            if(0 <= ty - 1 && !on_dir[sx][ty - 1]) cand.push_back({sx, ty - 1});
            if(0 <= ty - 1 && !on_dir[tx][ty - 1]) cand.push_back({tx, ty - 1});
        }
    }
    sort(ALL(cand));
    cand.erase(unique(ALL(cand)), cand.end());
    int score = -1;
    while(1){
        rep(i,n)rep(j,n) will_stone[i][j] = false;
        rep(i,cand.size()){
            if(xor128()%3 == 0) will_stone[cand[i].first][cand[i].second] = true;
        }
        score = calc_score(will_stone);
        if(score != 0) break;
    }

    
    double start_time = (double)clock()/CLOCKS_PER_SEC;
    double cur_time = start_time;
    double end_time = 1.95;
    double temp = 1;

    for(int yaki_cnt = 0; ;yaki_cnt++){
        if(yaki_cnt%10 == 0){
            cur_time = (double)clock()/CLOCKS_PER_SEC;
            if(cur_time > end_time){
                cerr << yaki_cnt << endl;
                break;
            }
        }
        int op = xor128()%4;
        if(op == 0){
            vector<pair<int,int>> era_cand;
            rep(i,n)rep(j,n)if(will_stone[i][j]) era_cand.push_back({i, j});
            auto [i, j] = era_cand[xor128()%era_cand.size()];
            will_stone[i][j] = !will_stone[i][j];
            int nscore = calc_score(will_stone);
            if(nscore != 0 && shift(temp, 0, end_time, start_time, score - nscore, cur_time)){
                score = nscore;
            }else{
                will_stone[i][j] = !will_stone[i][j];
            }
        }else if(op == 1){
            vector<pair<int,int>> shift_cand;
            rep(i,n)rep(j,n)if(will_stone[i][j]) shift_cand.push_back({i, j});
            auto [i, j] = shift_cand[xor128()%shift_cand.size()];
            int dir = xor128()%4;
            if(0 <= i + dx[dir] && i + dx[dir] < n && 0 <= j + dy[dir] && j + dy[dir] < n && !will_stone[i + dx[dir]][j + dy[dir]] && !on_dir[i + dx[dir]][j + dy[dir]]){
                will_stone[i][j] = false;
                will_stone[i + dx[dir]][j + dy[dir]] = true;
                int nscore = calc_score(will_stone);
                if(nscore != 0 && shift(temp, 0, end_time, start_time, score - nscore, cur_time)){
                    score = nscore;
                }else{
                    will_stone[i][j] = true;
                    will_stone[i + dx[dir]][j + dy[dir]] = false;
                }
            }
        }else if(op == 2){
            int i = xor128()%n;
            int j = xor128()%n;
            if(on_dir[i][j]) continue;
            will_stone[i][j] = !will_stone[i][j];
            int nscore = calc_score(will_stone);
            if(nscore != 0 && shift(temp, 0, end_time, start_time, score - nscore, cur_time)){
                score = nscore;
            }else{
                will_stone[i][j] = !will_stone[i][j];
            }
        }else if(op == 3){
            vector<pair<int,int>> ins_cand;
            rep(i,cand.size()) if(!will_stone[cand[i].first][cand[i].second]){
                ins_cand.push_back(cand[i]);
            }
            auto [i, j] = ins_cand[xor128()%ins_cand.size()];
            if(on_dir[i][j]) continue;
            will_stone[i][j] = !will_stone[i][j];
            int nscore = calc_score(will_stone);
            if(nscore != 0 && shift(temp, 0, end_time, start_time, score - nscore, cur_time)){
                score = nscore;
            }else{
                will_stone[i][j] = !will_stone[i][j];
            }
        }
    }
    vector<pair<char, char>> ans = calc_ans(will_stone);
    for(auto[i, j] : ans) cout << i << ' ' << j << '\n';
    return 0;
}

//1,455.96
//1,457.77
//1,464.71
//1,464.81