#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <cassert>
#include <cmath>

// #define LOCAL

using namespace std;

constexpr int SIZE = 30;
constexpr int INF  = 100000000;

const int dx[4] = {1, 0, -1, 0};
const int dy[4] = {0, 1, 0, -1};
const int di[4] = {1, SIZE, -1, -SIZE};
const string DIR = "RDLU";

#ifdef LOCAL
class Checker {
private:
    int h[SIZE][SIZE-1];
    int v[SIZE-1][SIZE];
    double score = 0.0;

    int si, sj, ti, tj, a;
    double e;
public:
    void read_input() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE-1; j++) {
                cin >> h[i][j];
            }
        }
        for (int i = 0; i < SIZE-1; i++) {
            for (int j = 0; j < SIZE; j++) {
                cin >> v[i][j];
            }
        }
    }

    void read_query(int& _si, int& _sj, int& _ti, int& _tj) {
        cin >> si >> sj >> ti >> tj >> a >> e;
        _si = si; _sj = sj; _ti = ti; _tj = tj;
    }

    int calc_len(string ans) {
        double dist = 0.0;
        bool saw[SIZE][SIZE] = {0};
        for (char d : ans) {
            if (saw[si][sj]) {
                cerr << "saw" << endl;
                return 0;
            }
            saw[si][sj] = true;
            if (d == 'R') {
                dist += h[si][sj];
                sj += 1;
            } else if (d == 'L') {
                dist += h[si][sj-1];
                sj -= 1;
            } else if (d == 'U') {
                dist += v[si-1][sj];
                si -= 1;
            } else if (d == 'D') {
                dist += v[si][sj];
                si += 1;
            } else {
                cerr << "fail" << endl;
                return 0;
            }
            if (si < 0 || si >= SIZE || sj < 0 || sj >= SIZE) {
                cerr << "fail" << endl;
                return 0;
            }
        }
        if (si != ti || sj != tj) {
            cerr << "unreach" << endl;
            return 0;
        }
        score = score * 0.998 + a / dist;
        return (int)round(dist * e);
    }

    int final_score() {
        return (int)round(2312311 * score);
    }
} checker;
#endif

const int INITIAL_WEIGHT = 2500;
const int DIVIDE_M_START = 200;
const int M_THRES = 2500;
const int LOOP_CNT = 40;

int edge_count[2*SIZE*SIZE];
int weight[SIZE*SIZE*2];
int barriers[2*SIZE];

int M_state = 0; // 0 .. unknown

vector<pair<int,int>> special_edges[SIZE*SIZE];

struct PATH {
    vector<pair<int,int>> route;
    string commands;
    int score;

    vector<int> cnts;

    PATH (vector<pair<int,int>>& _route, string& _commands, int _score) : route(_route), commands(_commands), score(_score){
        for (int i = 0; i < (int)commands.size(); i++) {
            const pair<int,int>& pos = route[i];
            const char command = commands[i];
            if (command == 'L' || command == 'R') {
                int x = (SIZE+pos.second)*SIZE+pos.first-(command=='L');
                cnts.push_back(x);
                edge_count[x]++;
            } else {
                int x = pos.first*SIZE+pos.second-(command=='U');
                cnts.push_back(x);
                edge_count[x]++;
            }
        }
    }
};

vector<PATH> paths;

void init() {
    for (int i = 0; i < 2*SIZE*SIZE; i++) {
        weight[i] = INITIAL_WEIGHT;
    }
#ifdef LOCAL
    checker.read_input();
#endif
}

void set_weight(int ite) {
    vector<int> shifts(SIZE*SIZE*2, 0);
    vector<int> cnts(SIZE*SIZE*2, 0);
    int ave_weight[2*SIZE];
    int ave_weight2[2*SIZE][2];
    for (int _ = 0; _ < LOOP_CNT; _++) {
        std::fill(shifts.begin(), shifts.end(), 0);
        std::fill(cnts.begin(), cnts.end(), 0);

        for (PATH& path : paths) {
            int dist = 0;
            int len = path.commands.size();
            for (int p : path.cnts) {
                dist += weight[p];
            }

            int shift = (path.score - dist) / len;

            for (int p : path.cnts) {
                shifts[p] += shift;
                cnts[p]++;
            }
        }
        for (int i = 0; i < SIZE*SIZE*2; i++) {
            if (cnts[i] <= 0) continue;
            weight[i] += shifts[i] / cnts[i];

            weight[i] = min(weight[i], 9000);
            weight[i] = max(weight[i], 1000);
        }

        if (_ == LOOP_CNT-1) break;

        if (M_state == 1) {
            for (int i = 0; i < SIZE*2; i++) {
                ave_weight[i] = 0;
                for (int j = 0; j < SIZE-1; j++) {
                    ave_weight[i] += weight[i*SIZE+j];
                }
                ave_weight[i] /= (SIZE-1);
            }
        }

        if (M_state == 2) {
            for (int i = 0; i < SIZE*2; i++) {
                ave_weight2[i][0] = 0;
                ave_weight2[i][1] = 0;
                for (int j = 0; j < SIZE-1; j++) {
                    ave_weight2[i][(j>=barriers[i])] += weight[i*SIZE+j];
                }
                ave_weight2[i][0] /= barriers[i];
                ave_weight2[i][1] /= (SIZE-1-barriers[i]);
            }
        }

        int new_weight[2*SIZE*SIZE];
        for (int i = 0; i < SIZE*2; i++) {
            for (int j = 0; j < SIZE-1; j++) {
                int cnt = edge_count[i*SIZE+j]*2;
                new_weight[i*SIZE+j] = weight[i*SIZE+j]*cnt;
                if (M_state == 1) {
                    new_weight[i*SIZE+j] += ave_weight[i]*4;
                    cnt += 4;
                } else if (M_state == 2) {
                    if (abs(j-barriers[i]) > 5) {
                        if (j) {
                            new_weight[i*SIZE+j] += weight[i*SIZE+j-1];
                            cnt++;
                        }
                        if (j > 1) {
                            new_weight[i*SIZE+j] += weight[i*SIZE+j-2];
                            cnt++;
                        }
                        if (j > 2) {
                            new_weight[i*SIZE+j] += weight[i*SIZE+j-3];
                            cnt++;
                        }
                        if (j < SIZE-2) {
                            new_weight[i*SIZE+j] += weight[i*SIZE+j+1];
                            cnt++;
                        }
                        if (j < SIZE-3) {
                            new_weight[i*SIZE+j] += weight[i*SIZE+j+2];
                            cnt++;
                        }
                        if (j < SIZE-4) {
                            new_weight[i*SIZE+j] += weight[i*SIZE+j+3];
                            cnt++;
                        }
                    } else {
                        if (j) {
                            new_weight[i*SIZE+j] += weight[i*SIZE+j-1];
                            cnt++;
                        }
                        if (j > 1) {
                            new_weight[i*SIZE+j] += weight[i*SIZE+j-2];
                            cnt++;
                        }
                        if (j < SIZE-2) {
                            new_weight[i*SIZE+j] += weight[i*SIZE+j+1];
                            cnt++;
                        }
                        if (j < SIZE-3) {
                            new_weight[i*SIZE+j] += weight[i*SIZE+j+2];
                            cnt++;
                        }
                    }
                } else {
                    if (j) {
                        new_weight[i*SIZE+j] += weight[i*SIZE+j-1];
                        cnt++;
                    }
                    if (j > 1) {
                        new_weight[i*SIZE+j] += weight[i*SIZE+j-2];
                        cnt++;
                    }
                    if (j < SIZE-2) {
                        new_weight[i*SIZE+j] += weight[i*SIZE+j+1];
                        cnt++;
                    }
                    if (j < SIZE-3) {
                        new_weight[i*SIZE+j] += weight[i*SIZE+j+2];
                        cnt++;
                    }
                }
                new_weight[i*SIZE+j] /= cnt;
            }
        }
        for (int i = 0; i < SIZE*SIZE*2; i++) {
            weight[i] = new_weight[i];
        }
        /*
        for (int i = 0; i < SIZE*2; i++) {
            for (int j = 0; j < SIZE-2; j++) {
                weight[i*SIZE+j] = (new_weight[i*SIZE+j]+new_weight[i*SIZE+j+1]) / 2;
            }
        }
        */
    }
}

void solve_dp(int st, int en) {

    for (int i = st; i < en; i++) {
        int sx, sy, tx, ty;
#ifndef LOCAL
        std::cin >> sy >> sx >> ty >> tx;
#else
        checker.read_query(sy, sx, ty, tx);
#endif

        vector<int> dp(SIZE*SIZE, INF);
        vector<int> pre(SIZE*SIZE);

        dp[sx+sy*SIZE] = 0;

        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> que;

        que.push(make_pair(0, sx+sy*SIZE));

        while (que.size()) {
            int v = que.top().second;
            int x = v % SIZE, y = v / SIZE;
            int dist = que.top().first;
            que.pop();

            if (dp[v] < dist) continue;

            for (pair<int,int> p : special_edges[v]) {
                if (dp[p.first] > dp[v] + paths[p.second].score*1.1) {
                    dp[p.first] = dp[v] + paths[p.second].score*1.1;
                    pre[p.first] = SIZE*SIZE+p.second;
                    que.push(make_pair(dp[p.first], p.first));
                }
            }

            for (int k = 0; k < 4; k++) {
                int nx = x + dx[k], ny = y + dy[k];
                if (nx < 0 || nx >= SIZE || ny < 0 || ny >= SIZE) {
                    continue;
                }
                int nv = nx+ny*SIZE;
                int add_dist = 0;
                if (k == 0) {
                    add_dist = weight[(SIZE+y)*SIZE+x];
                } else if (k == 1) {
                    add_dist = weight[x*SIZE+y];
                } else if (k == 2) {
                    add_dist = weight[(SIZE+y)*SIZE+x-1];
                } else {
                    add_dist = weight[x*SIZE+y-1];
                }

                if (dp[nv] > dp[v] + add_dist) {
                    dp[nv] = dp[v] + add_dist;
                    pre[nv] = v;
                    que.push(make_pair(dp[nv], nv));
                }
            }
        }

        int goal = tx+ty*SIZE;

        string ans;
        vector<pair<int,int>> vec;
        vec.push_back(make_pair(tx,ty));

        while (goal != sx+sy*SIZE) {
            int to = pre[goal];
            if (to >= SIZE*SIZE) {
                to -= SIZE*SIZE;
                const PATH& path = paths[to];
                string tmp_string = path.commands;
                if (path.route[0] == pair<int,int>(goal%SIZE, goal/SIZE)) {
                    for (char& s : tmp_string) {
                        if (s == 'R') s = 'L';
                        else if (s == 'L') s = 'R';
                        else if (s == 'U') s = 'D';
                        else s = 'U';
                    }
                    ans += tmp_string;
                    for (int i = 1; i < path.route.size(); i++) {
                        vec.push_back(path.route[i]);
                    }
                    goal = path.route.back().first+path.route.back().second*SIZE;
                } else {
                    reverse(tmp_string.begin(), tmp_string.end());
                    ans += tmp_string;
                    for (int i = (int)path.route.size()-2; i >= 0; i--) {
                        vec.push_back(path.route[i]);
                    }
                    goal = path.route[0].first+path.route[0].second*SIZE;
                }
                continue;
            }
            for (int k = 0; k < 4; k++) {
                if (to+di[k] == goal) {
                    int x = to%SIZE, y = to/SIZE;
                    ans.push_back(DIR[k]);
                    vec.push_back(make_pair(x, y));
                }
            }
            goal = to;
        }
        std::reverse(ans.begin(), ans.end());
        std::reverse(vec.begin(), vec.end());

        while (true) {
            int hoge = -1, huga;
            for (int j = 0; j < vec.size(); j++) {
                for (int k = j+1; k < vec.size(); k++) {
                    if (vec[j] == vec[k]) {
                        hoge = j;
                        huga = k;
                        break;
                    }
                }
                if (hoge != -1) break;
            }
            if (hoge == -1) break;
            vec.erase(vec.begin()+hoge, vec.begin()+huga);
            ans.erase(ans.begin()+hoge, ans.begin()+huga);
        }
        
        int score;
#ifndef LOCAL
        std::cout << ans << std::endl;
        std::cin >> score;
#else
        score = checker.calc_len(ans);
#endif
        paths.push_back(PATH(vec, ans, score));

        const PATH& path = paths.back();
        int st = path.route[0].first+path.route[0].second*SIZE;
        int en = path.route.back().first+path.route.back().second*SIZE;
        special_edges[st].push_back(make_pair(en, (int)paths.size()-1));
        special_edges[en].push_back(make_pair(st, (int)paths.size()-1));
    }
}

int main() {
    std::cin.tie(nullptr);
    ios::sync_with_stdio(false);

    init();


    for (int i = 0; i < 1000; i += 1) {
        set_weight(i);
        solve_dp(i, i+1);

        if (i >= DIVIDE_M_START) {
            int hoge = 0;
            for (int j = 0; j < 2*SIZE; j++) {
                int value = 0;
                barriers[j] = 5;
                for (int k = 5; k < SIZE-1-5; k++) {
                    int sum1 = 0, sum2 = 0;
                    for (int l = 0; l < k; l++) {
                        sum1 += weight[j*SIZE+l];
                    }
                    sum1 /= k;
                    for (int l = k; l < SIZE-1; l++) {
                        sum2 += weight[j*SIZE+l];
                    }
                    sum2 /= (SIZE-1-k);

                    if (value < abs(sum1-sum2)) {
                        value = abs(sum1-sum2);
                        barriers[j] = k;
                    }
                }
                hoge = max(hoge,value);
            }

            if (i == DIVIDE_M_START) {
                if (hoge <= M_THRES) {
                    M_state = 1;
                } else {
                    M_state = 2;
                }
            } else {
                if (hoge <= M_THRES) {
                    M_state = 1;
                }
            }
        }
    }
#ifdef LOCAL
    cout << checker.final_score() << endl;
#endif
}