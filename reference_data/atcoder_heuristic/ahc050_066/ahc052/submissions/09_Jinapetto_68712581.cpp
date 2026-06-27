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
#define ALL(obj) (obj).begin(),(obj).end()

int n,m,k;
int dist[30][30][30][30];
vi s_x(10), s_y(10);
vvb v(30, vb(29, false));
vvb h(29, vb(30, false));
int dx[4] = {1, 0, -1, 0};
int dy[4] = {0, 1, 0, -1};

unsigned long xor128(void){
	static unsigned long x = 123456789,y = 362436069,z = 541288629,w = 88675123;
	unsigned long t;
	t = (x^(x<<11));
	x = y;y = z;z = w;
	return(w = (w^(w>>19))^(t^(t>>8)));
}

void input(){
    cin >> n >> m >> k;
    rep(i,m) cin >> s_x[i] >> s_y[i];
    rep(i,n){
        string s;
        cin >> s;
        rep(j,n - 1) if(s[j] == '1') v[i][j] = true;
    }
    rep(i,n - 1){
        string s;
        cin >> s;
        rep(j,n) if(s[j] == '1') h[i][j] = true;
    }
}

void calc_dist(){
    rep(sx,n)rep(sy,n){
        rep(i,n)rep(j,n) dist[sx][sy][i][j] = INF;
        queue<pair<int,int>> que;
        que.push({sx, sy});
        dist[sx][sy][sx][sy] = 0;
        while(!que.empty()){
            auto [x, y] = que.front();
            que.pop();
            rep(i,4){
                int nx = x + dx[i];
                int ny = y + dy[i];
                if(nx < 0 || n <= nx || ny < 0 || n <= ny) continue;
                if(dist[sx][sy][nx][ny] != INF) continue;
                if(x < nx && h[x][y]) continue;
                if(nx < x && h[nx][y]) continue;
                if(y < ny && v[x][y]) continue;
                if(ny < y && v[x][ny]) continue;
                
                dist[sx][sy][nx][ny] = dist[sx][sy][x][y] + 1;
                que.push({nx, ny});
            }
        }
    }
}

vector<int> single_robot(vvb need_vis, int x, int y){
    need_vis[x][y] = false;
    int need_vis_cnt = 0;
    rep(i,n)rep(j,n) if(need_vis[i][j]) need_vis_cnt++;
    vector<int> ret;
    rep(_, need_vis_cnt){
        bool ok = false;
        rep(i,4){
            int nx = x + dx[i];
            int ny = y + dy[i];
            if(nx < 0 || n <= nx || ny < 0 || n <= ny) continue;
            if(!need_vis[nx][ny]) continue;
            if(x < nx && h[x][y]) continue;
            if(nx < x && h[nx][y]) continue;
            if(y < ny && v[x][y]) continue;
            if(ny < y && v[x][ny]) continue;
            ret.push_back(i);
            x = nx, y = ny;
            need_vis[nx][ny] = false;
            ok = true;
            break;
        }
        if(ok) continue;

        int mn = INF;
        int mn_x = -1, mn_y = -1;
        rep(i,n)rep(j,n) if(need_vis[i][j] && mn > dist[x][y][i][j]) mn = dist[x][y][i][j], mn_x = i, mn_y = j;
        int cur_x = mn_x, cur_y = mn_y;
        vector<int> pb;
        while(cur_x != x || cur_y != y){
            rep(i,4){
                int nx = cur_x + dx[(i + 2)%4];
                int ny = cur_y + dy[(i + 2)%4];
                if(nx < 0 || n <= nx || ny < 0 || n <= ny) continue;
                if(dist[x][y][nx][ny] + 1 != dist[x][y][cur_x][cur_y]) continue;
                if(cur_x < nx && h[cur_x][cur_y]) continue;
                if(nx < cur_x && h[nx][cur_y]) continue;
                if(cur_y < ny && v[cur_x][cur_y]) continue;
                if(ny < cur_y && v[cur_x][ny]) continue;
                cur_x = nx, cur_y = ny;
                pb.push_back(i);
                break;
            }
        }
        for(int i = (int)pb.size() - 1;i >= 0;i--) ret.push_back(pb[i]);
        x = mn_x, y = mn_y;
        need_vis[x][y] = false;
    }
    return ret;
}

void greedy_ver1(){
    vvb need_vis(n, vb(n, true));
    rep(i,m) need_vis[s_x[i]][s_y[i]] = false;
    int single_idx = 0;
    vi x = s_x, y = s_y;
    int turn = 0;
    vector<int> ans;
    while(1){
        bool fn = false;
        rep(_,n - 1){
            turn++;
            rep(i,m)if(i != single_idx){
                if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                x[i]++;
                need_vis[x[i]][y[i]] = false;
            }
            if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                fn = true;
                break;
            }
        }
        if(fn) break;

        turn++;
        rep(i,m)if(i != single_idx){
            if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
            y[i]++;
            need_vis[x[i]][y[i]] = false;
        }
        if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
            ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
            fn = true;
            break;
        }

        rep(_,n - 1){
            turn++;
            rep(i,m)if(i != single_idx){
                if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                x[i]--;
                need_vis[x[i]][y[i]] = false;
            }
            if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                fn = true;
                break;
            }
        }
        if(fn) break;
        
        turn++;
        rep(i,m)if(i != single_idx){
            if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
            y[i]++;
            need_vis[x[i]][y[i]] = false;
        }
        if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
            ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
            fn = true;
            break;
        }
    }
    rep(i,k){
        rep(j,m){
            if(j == single_idx){
                if(i == 0 || i == 4) cout << "D ";
                else if(i == 1 || i == 5) cout << "R ";
                else if(i == 2 || i == 6) cout << "U ";
                else if(i == 3 || i == 7) cout << "L ";
                else cout << "S ";
            }else{
                if(i < 4) cout << "D ";
                else if(i < 8) cout << "U ";
                else if(i == 8) cout << "R ";
                else if(i == 9) cout << "L ";
            }
        }
    }
    int cur_idx = 0;
    for(int cur_turn = 0;cur_turn < turn || cur_idx != ans.size();cur_turn++){
        if(cur_turn%60 == 29 || cur_turn%60 == 59){
            cout << 8 << '\n';
        }else if(cur_turn%60 < 30){
            if(cur_idx == ans.size()) cout << 0 << '\n';
            else cout << ans[cur_idx++] << '\n';
        }else{
            if(cur_idx == ans.size()) cout << 4 << '\n';
            else cout << ans[cur_idx++] + 4 << '\n';
        }
    }
}

void yama_ver2(){
    vi mn_ans;
    int mn_turn = INF;
    int base_turn = INF;
    vi mn_dir(m, 0);
    vi base_dir(m, 0);
    int base_single_idx = 0;
    int mn_single_idx = 0;
    int yaki_cnt;
    vi mn_other_ans;
    bool pre_is_imi = false;
    for(yaki_cnt = 0;;yaki_cnt++){
        double cur_time = clock()/(double)CLOCKS_PER_SEC;
        if(cur_time > 1.9) break;
        bool is_imi = (cur_time > 1.0);
        if(is_imi && !pre_is_imi){
            base_single_idx = 0;
            base_dir = vector<int>(m, 0);
            pre_is_imi = true;
            base_turn = INF;
        }
        vi dir = base_dir;
        int single_idx = base_single_idx;

        int op = xor128()%10;
        if(op < 9){
            int pos = xor128()%m;
            if(pos == single_idx) continue;
            int nval = xor128()%8;
            if(dir[pos] == nval) continue;
            dir[pos] = nval;
        }else if(op == 1){
            int pos = xor128()%m;
            if(single_idx == pos) continue;
            single_idx = pos;
        }
        
        vvb need_vis(n, vb(n, true));
        rep(i,m) need_vis[s_x[i]][s_y[i]] = false;
        vi x = s_x, y = s_y;
        int turn = 0;
        vector<int> ans;
        vector<int> other_ans;
        while(1){
            bool fn = false;
            rep(_,n - 1){
                if(is_imi){
                    bool imi = false;
                    rep(i,m)if(i != single_idx){
                        int cur_x = x[i], cur_y = y[i];
                        while(1){
                            if(dir[i] == 0 || dir[i] == 1){
                                if(cur_x == n - 1 || h[cur_x][cur_y]) break;
                                cur_x++;
                            }else if(dir[i] == 2 || dir[i] == 3){
                                if(cur_x == 0 || h[cur_x - 1][cur_y]) break;
                                cur_x--;
                            }else if(dir[i] == 4 || dir[i] == 5){
                                if(cur_y == n - 1 || v[cur_x][cur_y]) break;
                                cur_y++;
                            }else{
                                if(cur_y == 0 || v[cur_x][cur_y - 1]) break;
                                cur_y--;
                            }
                            if(need_vis[cur_x][cur_y]) imi = true;
                        }
                    }
                    if(!imi) break;
                }
                rep(i,m)if(i != single_idx){
                    if(dir[i] == 0 || dir[i] == 1){
                        if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                        x[i]++;
                    }else if(dir[i] == 2 || dir[i] == 3){
                        if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                        x[i]--;
                    }else if(dir[i] == 4 || dir[i] == 5){
                        if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                        y[i]++;
                    }else{
                        if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                        y[i]--;
                    }
                    need_vis[x[i]][y[i]] = false;
                }
                turn++;
                other_ans.push_back(0);
                if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                    ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                    fn = true;
                    break;
                }
            }
            if(fn) break;

            turn++;
            other_ans.push_back(1);
            rep(i,m)if(i != single_idx){
                if(dir[i] == 4 || dir[i] == 6){
                    if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                    x[i]++;
                }else if(dir[i] == 5 || dir[i] == 7){
                    if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                    x[i]--;
                }else if(dir[i] == 0 || dir[i] == 2){
                    if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                    y[i]++;
                }else{
                    if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                    y[i]--;
                }
                need_vis[x[i]][y[i]] = false;
            }
            if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                fn = true;
                break;
            }

            rep(_,n - 1){
                if(is_imi){
                    bool imi = false;
                    rep(i,m)if(i != single_idx){
                        int cur_x = x[i], cur_y = y[i];
                        while(1){
                            if(dir[i] == 2 || dir[i] == 3){
                                if(cur_x == n - 1 || h[cur_x][cur_y]) break;
                                cur_x++;
                            }else if(dir[i] == 0 || dir[i] == 1){
                                if(cur_x == 0 || h[cur_x - 1][cur_y]) break;
                                cur_x--;
                            }else if(dir[i] == 6 || dir[i] == 7){
                                if(cur_y == n - 1 || v[cur_x][cur_y]) break;
                                cur_y++;
                            }else{
                                if(cur_y == 0 || v[cur_x][cur_y - 1]) break;
                                cur_y--;
                            }
                            if(need_vis[cur_x][cur_y]) imi = true;
                        }
                    }
                    if(!imi) break;
                }
                rep(i,m)if(i != single_idx){
                    if(dir[i] == 2 || dir[i] == 3){
                        if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                        x[i]++;
                    }else if(dir[i] == 0 || dir[i] == 1){
                        if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                        x[i]--;
                    }else if(dir[i] == 6 || dir[i] == 7){
                        if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                        y[i]++;
                    }else{
                        if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                        y[i]--;
                    }
                    need_vis[x[i]][y[i]] = false;
                }
                other_ans.push_back(2);
                turn++;
                if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                    ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                    fn = true;
                    break;
                }
            }
            if(fn) break;
            
            turn++;
            other_ans.push_back(1);
            rep(i,m)if(i != single_idx){
                if(dir[i] == 4 || dir[i] == 6){
                    if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                    x[i]++;
                }else if(dir[i] == 5 || dir[i] == 7){
                    if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                    x[i]--;
                }else if(dir[i] == 0 || dir[i] == 2){
                    if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                    y[i]++;
                }else{
                    if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                    y[i]--;
                }
                need_vis[x[i]][y[i]] = false;
            }
            if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                fn = true;
                break;
            }
        }
        if(mn_turn > turn){
            mn_turn = turn;
            mn_ans = ans;
            mn_dir = dir;
            mn_single_idx = single_idx;
            mn_other_ans = other_ans;
        }
        if(base_turn > turn){
            base_dir = dir;
            base_single_idx = single_idx;
        }
    }
    cerr << yaki_cnt << '\n';
    int single_idx = mn_single_idx;
    vi dir = mn_dir;
    vi ans = mn_ans;
    vi other_ans = mn_other_ans;
    int turn = mn_turn;
    rep(i,k){
        rep(j,m){
            if(j == single_idx){
                if(i == 0 || i == 4) cout << "D ";
                else if(i == 1 || i == 5) cout << "R ";
                else if(i == 2 || i == 6) cout << "U ";
                else if(i == 3 || i == 7) cout << "L ";
                else cout << "S ";
            }else{
                if(i < 4){
                    if(dir[j] == 0 || dir[j] == 1){
                        cout << "D ";
                    }else if(dir[j] == 2 || dir[j] == 3){
                        cout << "U ";
                    }else if(dir[j] == 4 || dir[j] == 5){
                        cout << "R ";
                    }else{
                        cout << "L ";
                    }
                }
                else if(i < 8){
                    if(dir[j] == 0 || dir[j] == 1){
                        cout << "U ";
                    }else if(dir[j] == 2 || dir[j] == 3){
                        cout << "D ";
                    }else if(dir[j] == 4 || dir[j] == 5){
                        cout << "L ";
                    }else{
                        cout << "R ";
                    }
                }
                else if(i == 8){
                    if(dir[j] == 4 || dir[j] == 6){
                        cout << "D ";
                    }else if(dir[j] == 5 || dir[j] == 7){
                        cout << "U ";
                    }else if(dir[j] == 0 || dir[j] == 2){
                        cout << "R ";
                    }else{
                        cout << "L ";
                    }
                }
                else if(i == 9) cout << "S ";
            }
        }
    }
    int cur_idx = 0;
    int other_idx = 0;
    while(other_idx != other_ans.size() || cur_idx != ans.size()){
        if(other_idx == other_ans.size()){
            cout << ans[cur_idx++] << '\n';
        }else if(other_ans[other_idx] == 1){
            cout << 8 << '\n';
            other_idx++;
        }else if(other_ans[other_idx] == 0){
            if(cur_idx == ans.size()) cout << 0 << '\n';
            else cout << ans[cur_idx++] << '\n';
            other_idx++;
        }else{
            if(cur_idx == ans.size()) cout << 4 << '\n';
            else cout << ans[cur_idx++] + 4 << '\n';
            other_idx++;
        }
    }
}

tuple<vi, int, vi, vi> yama_ver1(){
    vi mn_ans;
    int mn_turn = INF;
    vi mn_dir(m, 0);
    int mn_single_idx = 0;
    int yaki_cnt;
    vi mn_other_ans;
    for(yaki_cnt = 0;;yaki_cnt++){
        if(clock()/(double)CLOCKS_PER_SEC > 1.0) break;
        vi dir = mn_dir;
        int single_idx = mn_single_idx;

        int op = xor128()%10;
        if(op < 9){
            int pos = xor128()%m;
            if(pos == single_idx) continue;
            int nval = xor128()%8;
            if(dir[pos] == nval) continue;
            dir[pos] = nval;
        }else if(op == 1){
            int pos = xor128()%m;
            if(single_idx == pos) continue;
            single_idx = pos;
        }
        
        vvb need_vis(n, vb(n, true));
        rep(i,m) need_vis[s_x[i]][s_y[i]] = false;
        vi x = s_x, y = s_y;
        int turn = 0;
        vector<int> ans;
        vector<int> other_ans;
        while(1){
            bool fn = false;
            rep(_,n - 1){
                bool imi = false;
                vi prex = x;
                vi prey = y;
                rep(i,m)if(i != single_idx){
                    if(dir[i] == 0 || dir[i] == 1){
                        if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                        x[i]++;
                    }else if(dir[i] == 2 || dir[i] == 3){
                        if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                        x[i]--;
                    }else if(dir[i] == 4 || dir[i] == 5){
                        if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                        y[i]++;
                    }else{
                        if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                        y[i]--;
                    }
                    if(need_vis[x[i]][y[i]]) imi = true;
                    need_vis[x[i]][y[i]] = false;
                }
                if(!imi){
                    x = prex, y = prey;
                    break;
                }
                turn++;
                other_ans.push_back(0);
                if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                    ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                    fn = true;
                    break;
                }
            }
            if(fn) break;

            turn++;
            other_ans.push_back(1);
            rep(i,m)if(i != single_idx){
                if(dir[i] == 4 || dir[i] == 6){
                    if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                    x[i]++;
                }else if(dir[i] == 5 || dir[i] == 7){
                    if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                    x[i]--;
                }else if(dir[i] == 0 || dir[i] == 2){
                    if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                    y[i]++;
                }else{
                    if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                    y[i]--;
                }
                need_vis[x[i]][y[i]] = false;
            }
            if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                fn = true;
                break;
            }

            rep(_,n - 1){
                bool imi = false;
                vi prex = x;
                vi prey = y;
                rep(i,m)if(i != single_idx){
                    if(dir[i] == 2 || dir[i] == 3){
                        if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                        x[i]++;
                    }else if(dir[i] == 0 || dir[i] == 1){
                        if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                        x[i]--;
                    }else if(dir[i] == 6 || dir[i] == 7){
                        if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                        y[i]++;
                    }else{
                        if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                        y[i]--;
                    }
                    if(need_vis[x[i]][y[i]]) imi = true;
                    need_vis[x[i]][y[i]] = false;
                }
                if(!imi){
                    x = prex, y = prey;
                    break;
                }
                other_ans.push_back(2);
                turn++;
                if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                    ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                    fn = true;
                    break;
                }
            }
            if(fn) break;
            
            turn++;
            other_ans.push_back(1);
            rep(i,m)if(i != single_idx){
                if(dir[i] == 4 || dir[i] == 6){
                    if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                    x[i]++;
                }else if(dir[i] == 5 || dir[i] == 7){
                    if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                    x[i]--;
                }else if(dir[i] == 0 || dir[i] == 2){
                    if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                    y[i]++;
                }else{
                    if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                    y[i]--;
                }
                need_vis[x[i]][y[i]] = false;
            }
            if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                fn = true;
                break;
            }
        }
        if(mn_turn > turn){
            mn_turn = turn;
            mn_ans = ans;
            mn_dir = dir;
            mn_single_idx = single_idx;
            mn_other_ans = other_ans;
        }
    }
    return {mn_dir, mn_single_idx, mn_other_ans, mn_ans};
    cerr << yaki_cnt << '\n';
    int single_idx = mn_single_idx;
    vi dir = mn_dir;
    vi ans = mn_ans;
    vi other_ans = mn_other_ans;
    rep(i,k){
        rep(j,m){
            if(j == single_idx){
                if(i == 0 || i == 4) cout << "D ";
                else if(i == 1 || i == 5) cout << "R ";
                else if(i == 2 || i == 6) cout << "U ";
                else if(i == 3 || i == 7) cout << "L ";
                else cout << "S ";
            }else{
                if(i < 4){
                    if(dir[j] == 0 || dir[j] == 1){
                        cout << "D ";
                    }else if(dir[j] == 2 || dir[j] == 3){
                        cout << "U ";
                    }else if(dir[j] == 4 || dir[j] == 5){
                        cout << "R ";
                    }else{
                        cout << "L ";
                    }
                }
                else if(i < 8){
                    if(dir[j] == 0 || dir[j] == 1){
                        cout << "U ";
                    }else if(dir[j] == 2 || dir[j] == 3){
                        cout << "D ";
                    }else if(dir[j] == 4 || dir[j] == 5){
                        cout << "L ";
                    }else{
                        cout << "R ";
                    }
                }
                else if(i == 8){
                    if(dir[j] == 4 || dir[j] == 6){
                        cout << "D ";
                    }else if(dir[j] == 5 || dir[j] == 7){
                        cout << "U ";
                    }else if(dir[j] == 0 || dir[j] == 2){
                        cout << "R ";
                    }else{
                        cout << "L ";
                    }
                }
                else if(i == 9) cout << "S ";
            }
        }
    }
    int cur_idx = 0;
    int other_idx = 0;
    while(other_idx != other_ans.size() || cur_idx != ans.size()){
        if(other_idx == other_ans.size()){
            cout << ans[cur_idx++] << '\n';
        }else if(other_ans[other_idx] == 1){
            cout << 8 << '\n';
            other_idx++;
        }else if(other_ans[other_idx] == 0){
            if(cur_idx == ans.size()) cout << 0 << '\n';
            else cout << ans[cur_idx++] << '\n';
            other_idx++;
        }else{
            if(cur_idx == ans.size()) cout << 4 << '\n';
            else cout << ans[cur_idx++] + 4 << '\n';
            other_idx++;
        }
    }
}

tuple<vi, int, vi, vi> yama_ver3(){
    vi mn_ans;
    int mn_turn = INF;
    vi mn_dir(m, 2);
    int mn_single_idx = 0;
    int yaki_cnt;
    vi mn_other_ans;
    for(yaki_cnt = 0;;yaki_cnt++){
        if(clock()/(double)CLOCKS_PER_SEC > 1.9) break;
        vi dir = mn_dir;
        int single_idx = mn_single_idx;

        int op = xor128()%10;
        if(op < 9){
            int pos = xor128()%m;
            if(pos == single_idx) continue;
            int nval = xor128()%8;
            if(dir[pos] == nval) continue;
            dir[pos] = nval;
        }else if(op == 1){
            int pos = xor128()%m;
            if(single_idx == pos) continue;
            single_idx = pos;
        }
        
        vvb need_vis(n, vb(n, true));
        rep(i,m) need_vis[s_x[i]][s_y[i]] = false;
        vi x = s_x, y = s_y;
        int turn = 0;
        vector<int> ans;
        vector<int> other_ans;
        while(1){
            bool fn = false;
            rep(_,n - 1){
                bool imi = false;
                vi prex = x;
                vi prey = y;
                rep(i,m)if(i != single_idx){
                    if(dir[i] == 0 || dir[i] == 1){
                        if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                        x[i]++;
                    }else if(dir[i] == 2 || dir[i] == 3){
                        if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                        x[i]--;
                    }else if(dir[i] == 4 || dir[i] == 5){
                        if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                        y[i]++;
                    }else{
                        if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                        y[i]--;
                    }
                    if(need_vis[x[i]][y[i]]) imi = true;
                    need_vis[x[i]][y[i]] = false;
                }
                if(!imi){
                    x = prex, y = prey;
                    break;
                }
                turn++;
                other_ans.push_back(0);
                if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                    ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                    fn = true;
                    break;
                }
            }
            if(fn) break;

            turn++;
            other_ans.push_back(1);
            rep(i,m)if(i != single_idx){
                if(dir[i] == 4 || dir[i] == 6){
                    if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                    x[i]++;
                }else if(dir[i] == 5 || dir[i] == 7){
                    if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                    x[i]--;
                }else if(dir[i] == 0 || dir[i] == 2){
                    if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                    y[i]++;
                }else{
                    if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                    y[i]--;
                }
                need_vis[x[i]][y[i]] = false;
            }
            if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                fn = true;
                break;
            }

            rep(_,n - 1){
                bool imi = false;
                vi prex = x;
                vi prey = y;
                rep(i,m)if(i != single_idx){
                    if(dir[i] == 2 || dir[i] == 3){
                        if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                        x[i]++;
                    }else if(dir[i] == 0 || dir[i] == 1){
                        if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                        x[i]--;
                    }else if(dir[i] == 6 || dir[i] == 7){
                        if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                        y[i]++;
                    }else{
                        if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                        y[i]--;
                    }
                    if(need_vis[x[i]][y[i]]) imi = true;
                    need_vis[x[i]][y[i]] = false;
                }
                if(!imi){
                    x = prex, y = prey;
                    break;
                }
                other_ans.push_back(2);
                turn++;
                if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                    ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                    fn = true;
                    break;
                }
            }
            if(fn) break;
            
            turn++;
            other_ans.push_back(1);
            rep(i,m)if(i != single_idx){
                if(dir[i] == 4 || dir[i] == 6){
                    if(x[i] == n - 1 || h[x[i]][y[i]]) continue;
                    x[i]++;
                }else if(dir[i] == 5 || dir[i] == 7){
                    if(x[i] == 0 || h[x[i] - 1][y[i]]) continue;
                    x[i]--;
                }else if(dir[i] == 0 || dir[i] == 2){
                    if(y[i] == n - 1 || v[x[i]][y[i]]) continue;
                    y[i]++;
                }else{
                    if(y[i] == 0 || v[x[i]][y[i] - 1]) continue;
                    y[i]--;
                }
                need_vis[x[i]][y[i]] = false;
            }
            if(single_robot(need_vis, s_x[single_idx], s_y[single_idx]).size() < turn){
                ans = single_robot(need_vis, s_x[single_idx], s_y[single_idx]);
                fn = true;
                break;
            }
        }
        if(mn_turn > turn){
            mn_turn = turn;
            mn_ans = ans;
            mn_dir = dir;
            mn_single_idx = single_idx;
            mn_other_ans = other_ans;
        }
    }
    return {mn_dir, mn_single_idx, mn_other_ans, mn_ans};
    cerr << yaki_cnt << '\n';
    int single_idx = mn_single_idx;
    vi dir = mn_dir;
    vi ans = mn_ans;
    vi other_ans = mn_other_ans;
    rep(i,k){
        rep(j,m){
            if(j == single_idx){
                if(i == 0 || i == 4) cout << "D ";
                else if(i == 1 || i == 5) cout << "R ";
                else if(i == 2 || i == 6) cout << "U ";
                else if(i == 3 || i == 7) cout << "L ";
                else cout << "S ";
            }else{
                if(i < 4){
                    if(dir[j] == 0 || dir[j] == 1){
                        cout << "D ";
                    }else if(dir[j] == 2 || dir[j] == 3){
                        cout << "U ";
                    }else if(dir[j] == 4 || dir[j] == 5){
                        cout << "R ";
                    }else{
                        cout << "L ";
                    }
                }
                else if(i < 8){
                    if(dir[j] == 0 || dir[j] == 1){
                        cout << "U ";
                    }else if(dir[j] == 2 || dir[j] == 3){
                        cout << "D ";
                    }else if(dir[j] == 4 || dir[j] == 5){
                        cout << "L ";
                    }else{
                        cout << "R ";
                    }
                }
                else if(i == 8){
                    if(dir[j] == 4 || dir[j] == 6){
                        cout << "D ";
                    }else if(dir[j] == 5 || dir[j] == 7){
                        cout << "U ";
                    }else if(dir[j] == 0 || dir[j] == 2){
                        cout << "R ";
                    }else{
                        cout << "L ";
                    }
                }
                else if(i == 9) cout << "S ";
            }
        }
    }
    int cur_idx = 0;
    int other_idx = 0;
    while(other_idx != other_ans.size() || cur_idx != ans.size()){
        if(other_idx == other_ans.size()){
            cout << ans[cur_idx++] << '\n';
        }else if(other_ans[other_idx] == 1){
            cout << 8 << '\n';
            other_idx++;
        }else if(other_ans[other_idx] == 0){
            if(cur_idx == ans.size()) cout << 0 << '\n';
            else cout << ans[cur_idx++] << '\n';
            other_idx++;
        }else{
            if(cur_idx == ans.size()) cout << 4 << '\n';
            else cout << ans[cur_idx++] + 4 << '\n';
            other_idx++;
        }
    }
}

void out(vi mn_dir,int mn_single_idx, vi mn_other_ans, vi mn_ans){
    int single_idx = mn_single_idx;
    vi dir = mn_dir;
    vi ans = mn_ans;
    vi other_ans = mn_other_ans;
    rep(i,k){
        rep(j,m){
            if(j == single_idx){
                if(i == 0 || i == 4) cout << "D ";
                else if(i == 1 || i == 5) cout << "R ";
                else if(i == 2 || i == 6) cout << "U ";
                else if(i == 3 || i == 7) cout << "L ";
                else cout << "S ";
            }else{
                if(i < 4){
                    if(dir[j] == 0 || dir[j] == 1){
                        cout << "D ";
                    }else if(dir[j] == 2 || dir[j] == 3){
                        cout << "U ";
                    }else if(dir[j] == 4 || dir[j] == 5){
                        cout << "R ";
                    }else{
                        cout << "L ";
                    }
                }
                else if(i < 8){
                    if(dir[j] == 0 || dir[j] == 1){
                        cout << "U ";
                    }else if(dir[j] == 2 || dir[j] == 3){
                        cout << "D ";
                    }else if(dir[j] == 4 || dir[j] == 5){
                        cout << "L ";
                    }else{
                        cout << "R ";
                    }
                }
                else if(i == 8){
                    if(dir[j] == 4 || dir[j] == 6){
                        cout << "D ";
                    }else if(dir[j] == 5 || dir[j] == 7){
                        cout << "U ";
                    }else if(dir[j] == 0 || dir[j] == 2){
                        cout << "R ";
                    }else{
                        cout << "L ";
                    }
                }
                else if(i == 9) cout << "S ";
            }
        }
    }
    int cur_idx = 0;
    int other_idx = 0;
    while(other_idx != other_ans.size() || cur_idx != ans.size()){
        if(other_idx == other_ans.size()){
            cout << ans[cur_idx++] << '\n';
        }else if(other_ans[other_idx] == 1){
            cout << 8 << '\n';
            other_idx++;
        }else if(other_ans[other_idx] == 0){
            if(cur_idx == ans.size()) cout << 0 << '\n';
            else cout << ans[cur_idx++] << '\n';
            other_idx++;
        }else{
            if(cur_idx == ans.size()) cout << 4 << '\n';
            else cout << ans[cur_idx++] + 4 << '\n';
            other_idx++;
        }
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    input();
    calc_dist();
    // yama_ver2();
    auto [i1, i2, i3, i4] = yama_ver1();
    auto [j1, j2, j3, j4] = yama_ver3();
    if(max(i3.size(), i4.size()) < max(j3.size(), j4.size())){
        out(i1, i2, i3, i4);
    }else{
        out(j1, j2, j3, j4);
    }
    return 0;
}