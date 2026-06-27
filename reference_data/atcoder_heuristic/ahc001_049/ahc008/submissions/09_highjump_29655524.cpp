#pragma GCC optimize("Ofast")
#include <iostream>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <cctype>
#include <sstream>
#include <vector>
#include <stack>
#include <deque>
#include <queue>
#include <list>
#include <set>
#include <map>
#include <unordered_map>  
#include <chrono>
#include <random>

using namespace std;

using ll = long long;
using P = pair<int, int>;
template <class T>using V = vector<T>;
template <class T>using VV = V<V<T>>;
template <class T, class U>bool chmin(T& t, const U& u) { if (t > u) { t = u; return 1; } else return 0; }
template <class T, class U>bool chmax(T& t, const U& u) { if (t < u) { t = u; return 1; } else return 0; }

#define REP(i,n) for(int i=0;i<int(n);i++)
#define FOR(i,a,b) for(int i=int(a);i<=int(b);i++)
#define FORD(i,a,b) for(int i=int(a);i>=int(b);i--)
#define MP make_pair
#define SZ(x) int(x.size())
#define ALL(x) x.begin(),x.end()
#define INF 10001000
#define endl "\n"

chrono::system_clock::time_point t_start;
const double TIME_LIMIT1 = 500, TIME_LIMIT = 2930;
double last_update = 0, t_diff;
double start_temp, end_temp;

mt19937 rng;
using uni_int = uniform_int_distribution<>;
using uni_real = uniform_real_distribution<>;

uni_int rnd_int(0, 60);
V<int> vec = { -30,1,30,-1 ,0 };
V<int> y_vec = { 0, 1,0, -1,0 };
V<int> x_vec = { -1, 0,1, 0,0 };
V<char> dir = { 'U','R','D','L','.' };
V<char> dep = { 'u','r','d','l' };
unordered_map<char, int> dtoi = { {'U',0},{'R',1},{'D',2},{'L',3},{'.',4} };
unordered_map<char, int> btoi = { {'u',0},{'r',1},{'d',2},{'l',3} };
V<int> PARAM = { 160,50,50,50,50,50 };
V<int> width = {20,20,20,20,20,20};

queue<int> q;
V<int> color;
V<bool> visited;
V<int> blueprint;

void get_time() {
    auto t_now = chrono::system_clock::now();
    t_diff = chrono::duration_cast<chrono::milliseconds>(t_now - t_start).count();
}

const int H = 30;
int N, M, Turn, score, dog, cat;
string Action = "";

int Pos(int x, int y) {
    return x * H + y;
}

int Dir(int from, int to) {
    if (from == to)return 4;
    if (from < to) {
        if (to - from == 1)return 1;
        else return 2;
    }
    else {
        if (from - to == 1)return 3;
        else return 0;
    }
}

class Cell {
public:
    int state = 0;
    bool full = false;

    V<int> around4;
    V<int> newAround4;
};

V<Cell> grid(H* H);

class Human {
public:
    int pos = 0;

    Human() {};
    Human(int p) :pos(p) {};

    void execute(char c) {
        if (c == '.')return;
        else if (c <= 'Z') {
            this->move(c);
        }
    }

    void move(int d) {
        if (d == -1 || grid[pos + vec[d]].state == 1) {
            this->move(4);
            //this->walk();
            return;
        }
        grid[pos + vec[d]].full = true;
        Action += dir[d];
    }

    void move(char c) {
        grid[pos].full = false;
        pos += vec[dtoi[c]];
    }

    void block(int d) {
        int target = pos + vec[d];
        if (grid[target].full) {
            this->move(4);
            return;
        }
        grid[target].state = 1;
        grid[target].full = true;
        for (auto x : grid[target].around4) {
            for (auto itr = grid[x].around4.begin(); itr != grid[x].around4.end(); itr++) {
                if (*itr == target) {
                    grid[x].around4.erase(itr);
                    break;
                }
            }
        }
        Action += dep[d];
    }

    void set_full() {
        grid[pos].full = true;
    }

    int bfs(int target);
};

class Animal {
public:
    int pos = 0;

    Animal() {};
    Animal(int p) :pos(p) {};

    void move(int d) {
        grid[pos].full = false;
        for (auto x : grid[pos].around4)grid[x].full = false;
        pos += vec[d];
    }

    void move(string s) {
        grid[pos].full = false;
        for (auto x : grid[pos].around4)grid[x].full = false;
        for (auto c : s) {
            pos += vec[dtoi[c]];
        }
    }

    void set_full() {
        grid[pos].full = true;
        for (auto x : grid[pos].around4)grid[x].full = true;
    }
};

class Area {
public:
    int region = 0;
    int num_h = 0;
    int num_a = 0;
    int num_d = 0;
    int num_c = 0;
    int num_r = 0;
    V<int> doors;
};

class Target {
public:
    int dis = 0;
    int val = 0;
    int usr = 0;
    int pos = 0;

    Target() {};
    Target(int d, int v, int u, int p) :dis(d), val(v), usr(u), pos(p) {};
};


V<Human> humans;
V<Animal> animals;
V<Area> areas;
V<int> kind, order,aim,work,task;
V<bool> border(H* H);

int Human::bfs(int target) {
    q = queue<int>();
    visited = V<bool>(H * H);
    q.push(pos);
    q.push(-1);
    int res = -1;
    while (!q.empty()) {
        int x = q.front(); q.pop();
        int d = q.front(); q.pop();
        if (visited[x])continue;
        visited[x] = true;
        for (auto n : grid[x].around4) {
            if (visited[n] || grid[n].state == 1)continue;
            //if (color[n] != -1 && areas[color[n]].num_a > 1)continue;
            if (n == target) {
                res = (d != -1 ? d : Dir(x, n));
                return res;
            }
            q.push(n);
            q.push((d != -1 ? d : Dir(x, n)));
        }
    }
    return -1;
}

int get_score() {
    color = V<int>(H * H, -1);
    areas = V<Area>();
    int area = -1;
    REP(i, M) {
        if (color[humans[i].pos] != -1) {
            areas[color[humans[i].pos]].num_h++;
            continue;
        }
        area++;
        areas.emplace_back(Area());
        areas.back().num_h++;
        q = queue<int>();
        q.push(humans[i].pos);
        while (!q.empty()) {
            int p = q.front(); q.pop();
            if (color[p] == area)continue;
            color[p] = area;
            areas[area].region++;
            for (int n : grid[p].around4) {
                if (grid[n].state != 0 || color[n] != -1)continue;
                q.push(n);
            }
        }
    }
    REP(i, N) {
        if (color[animals[i].pos] != -1) {
            areas[color[animals[i].pos]].num_a++;
        }
    }
    double res = 0.0;
    for (auto a : areas) {
        res += double(a.region) / 900.0 / pow(2, a.num_a) * a.num_h;
    }
    return int(round(100000000.0 * res / M));
}

void init() {
    V<int> muki = { 1,3,0,2 };
    REP(i, H) {
        REP(j, H) {
            grid[Pos(i, j)].around4 = V<int>();
            grid[Pos(i, j)].newAround4 = V<int>();
            for (auto k : muki) {
                int nx = i + x_vec[k];
                int ny = j + y_vec[k];
                if (nx < 0 || nx >= H || ny < 0 || ny >= H)continue;
                grid[Pos(i, j)].around4.emplace_back(Pos(nx, ny));
                grid[Pos(i, j)].newAround4.emplace_back(Pos(nx, ny));
            }
        }
    }

    cin >> N;
    kind = V<int>(N);
    animals = V<Animal>(N);
    dog = 0;
    cat = 0;
    REP(i, N) {
        int x, y, z; cin >> x >> y >> z;
        x--; y--; z--;
        animals[i].pos = Pos(x, y);
        kind[i] = z;
        if (z == 3)dog++;
        else if (z == 4)cat++;
    }
    cin >> M;
    humans = V<Human>(M);
    order = V<int>(M);
    REP(i, M)order[i] = i;
    REP(i, M) {
        int x, y; cin >> x >> y;
        x--; y--;
        humans[i].pos = Pos(x, y);
    }
    blueprint = V<int>(H * H);
}

bool operator < (const Target& l, const Target& r) {
    // if (l.val != r.val)return l.val < r.val;
    // if (l.dis != r.dis)return l.dis > r.dis;
    if(l.dis*(5-l.val)!=r.dis*(5-r.val)){
        return l.dis*(5-l.val)>r.dis*(5-r.val);
    }
    return l.usr > r.usr;
}

void set_target() {
    aim = V<int>(M, -1);
    priority_queue<Target> pq;
    shuffle(ALL(order), rng);
    int avail=0;
    REP(i, M) {
        avail++;
        q = queue<int>();
        visited = V<bool>(H * H, false);
        q.push(humans[i].pos);
        q.push(0);
        while (!q.empty()) {
            int x = q.front(); q.pop();
            int d = q.front(); q.pop();
            if (visited[x])continue;
            visited[x] = true;
            if (blueprint[x] > 0) {
                pq.push(Target(d, blueprint[x], i, x));
            }
            for (auto n : grid[x].around4) {
                if (visited[n] || grid[n].state == 1)continue;
                //if (color[n] != -1 && areas[color[n]].num_a > 0)continue;
                q.push(n);
                q.push(d + 1);
            }
        }
    }

    int dec = 0;
    visited = V<bool>(H * H, false);
    while (dec < avail && !pq.empty()) {
        Target t = pq.top(); pq.pop();
        if (t.dis == 0) {
            visited[t.pos] = true;
            continue;
        }

        if (aim[t.usr] != -1)continue;
        if (visited[t.pos])continue;
        if (M == 5 && t.dis > 40)break;
        if(abs(t.pos/H-humans[t.usr].pos/H)>15)continue;

        int bro = -1;
        if (t.pos / H == 10 && (t.pos % H == 29 || t.pos % H % 3 == 0)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(19, t.pos % H);
            else bro = Pos(19, t.pos % H - 1);
        }
        else if (t.pos / H == 19 && (t.pos % H == 0 || t.pos % H % 3 == 2)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(10, t.pos % H);
            else bro = Pos(10, t.pos % H + 1);
        }
        if(bro!=-1){
            bool ok = false;
            V<Target> pool;
            Target nt;
            while (!pq.empty()) {
                nt = pq.top(); pq.pop();
                if (aim[nt.usr] != -1)continue;
                if (visited[nt.pos])continue;
                if (nt.dis - t.dis > width[M-5])break;
                if (nt.pos == bro && nt.usr != t.usr) {
                    ok = true;
                    break;
                }
                pool.push_back(nt);
            }
            while (!pool.empty()) {
                pq.push(pool.back());
                pool.pop_back();
            }

            if (ok) {
                dec++;
                aim[nt.usr] = nt.pos;
                visited[nt.pos] = true;
            }
            else {
                continue;
            }
        }
        dec++;
        aim[t.usr] = t.pos;
        visited[t.pos] = true;
    }
}

void set_target2() {
    aim = V<int>(M, -1);
    priority_queue<Target> pq;
    shuffle(ALL(order), rng);
    REP(i, M) {
        q = queue<int>();
        visited = V<bool>(H * H, false);
        q.push(humans[i].pos);
        q.push(0);
        while (!q.empty()) {
            int x = q.front(); q.pop();
            int d = q.front(); q.pop();
            if (visited[x])continue;
            visited[x] = true;
            if (blueprint[x] > 0) {
                pq.push(Target(d, blueprint[x], i, x));
            }
            for (auto n : grid[x].around4) {
                if (visited[n] || grid[n].state == 1)continue;
                //if (color[n] != -1 && areas[color[n]].num_a > 0)continue;
                q.push(n);
                q.push(d + 1);
            }
        }
    }

    int dec = 0;
    visited = V<bool>(H * H, false);
    while (dec < M && !pq.empty()) {
        Target t = pq.top(); pq.pop();
        if (t.dis == 0) {
            visited[t.pos] = true;
            continue;
        }
        
        if (aim[t.usr] != -1)continue;
        if (visited[t.pos])continue;
        if (M == 5 && t.dis > 40)break;
        if(abs(t.pos/H-humans[t.usr].pos/H)>12)continue;
        int bro = -1;
        if (t.pos / H == 7 && (t.pos % 3 == 0 || t.pos % H == 29)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(13, t.pos % H);
            else bro = Pos(13, t.pos % H - 1);
        }
        else if (t.pos / H == 13 && (t.pos % 3 == 2 || t.pos % H == 0)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(7, t.pos % H);
            else bro = Pos(7, t.pos % H + 1);
        }
        else if (t.pos / H == 15 && (t.pos % 3 == 0 || t.pos % H == 29)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(22, t.pos % H);
            else bro = Pos(22, t.pos % H - 1);
        }
        else if (t.pos / H == 22 && (t.pos % 3 == 2 || t.pos % H == 0)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(15, t.pos % H);
            else bro = Pos(15, t.pos % H + 1);
        }
        if (bro != -1) {
            bool ok = false;
            V<Target> pool;
            Target nt;
            while (!pq.empty()) {
                nt = pq.top(); pq.pop();
                if (aim[nt.usr] != -1)continue;
                if (visited[nt.pos])continue;
                if (nt.dis - t.dis > width[M-5])break;
                if (nt.pos == bro && nt.usr != t.usr) {
                    ok = true;
                    break;
                }
                pool.push_back(nt);
            }
            while (!pool.empty()) {
                pq.push(pool.back());
                pool.pop_back();
            }

            if (ok) {
                dec++;
                aim[nt.usr] = nt.pos;
                visited[nt.pos] = true;
            }
            else {
                continue;
            }
        }
        dec++;
        aim[t.usr] = t.pos;
        visited[t.pos] = true;
    }
}

void set_target3() {
    aim = V<int>(M, -1);
    priority_queue<Target> pq;
    shuffle(ALL(order), rng);
    REP(i, M) {
        q = queue<int>();
        visited = V<bool>(H * H, false);
        q.push(humans[i].pos);
        q.push(0);
        while (!q.empty()) {
            int x = q.front(); q.pop();
            int d = q.front(); q.pop();
            if (visited[x])continue;
            visited[x] = true;
            if (blueprint[x] > 0) {
                pq.push(Target(d, blueprint[x], i, x));
            }
            for (auto n : grid[x].around4) {
                if (visited[n] || grid[n].state == 1)continue;
                //if (color[n] != -1 && areas[color[n]].num_a > 0)continue;
                q.push(n);
                q.push(d + 1);
            }
        }
    }

    int dec = 0;
    visited = V<bool>(H * H, false);
    while (dec < M && !pq.empty()) {
        Target t = pq.top(); pq.pop();
        if (t.dis == 0) {
            visited[t.pos] = true;
            continue;
        }
        
        if (aim[t.usr] != -1)continue;
        if (visited[t.pos])continue;
        if (M == 5 && t.dis > 40)break;
        if(abs(t.pos/H-humans[t.usr].pos/H)>9)continue;
        int bro = -1;
        if (t.pos / H == 5 && (t.pos % 3 == 0 || t.pos % H == 29)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(10, t.pos % H);
            else bro = Pos(10, t.pos % H - 1);
        }
        else if (t.pos / H == 10 && (t.pos % 3 == 2 || t.pos % H == 0)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(5, t.pos % H);
            else bro = Pos(5, t.pos % H + 1);
        }
        else if (t.pos / H == 12 && (t.pos % 3 == 0 || t.pos % H == 29)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(17, t.pos % H);
            else bro = Pos(17, t.pos % H - 1);
        }
        else if (t.pos / H == 17 && (t.pos % 3 == 2 || t.pos % H == 0)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(12, t.pos % H);
            else bro = Pos(12, t.pos % H + 1);
        }
        else if (t.pos / H == 19 && (t.pos % 3 == 0 || t.pos % H == 29)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(24, t.pos % H);
            else bro = Pos(24, t.pos % H - 1);
        }
        else if (t.pos / H == 24 && (t.pos % 3 == 2 || t.pos % H == 0)) {
            if (t.pos % H == 0 || t.pos % H == 29)bro = Pos(19, t.pos % H);
            else bro = Pos(19, t.pos % H + 1);
        }
        if (bro != -1) {
            bool ok = false;
            V<Target> pool;
            Target nt;
            while (!pq.empty()) {
                nt = pq.top(); pq.pop();
                if (aim[nt.usr] != -1)continue;
                if (visited[nt.pos])continue;
                if (nt.dis - t.dis > width[M-5])break;
                if (nt.pos == bro && nt.usr != t.usr) {
                    ok = true;
                    break;
                }
                pool.push_back(nt);
            }
            while (!pool.empty()) {
                pq.push(pool.back());
                pool.pop_back();
            }

            if (ok) {
                dec++;
                aim[nt.usr] = nt.pos;
                visited[nt.pos] = true;
            }
            else {
                continue;
            }
        }
        dec++;
        aim[t.usr] = t.pos;
        visited[t.pos] = true;
    }
}

void getArea() {
    color = V<int>(H * H, -1);
    areas = V<Area>();
    int area = -1;
    REP(i, H * H) {
        if (color[i] != -1 || border[i] || grid[i].state == 1)continue;
        area++;
        areas.emplace_back(Area());
        q = queue<int>();
        q.push(i);
        while (!q.empty()) {
            int p = q.front(); q.pop();
            if (color[p] == area)continue;
            color[p] = area;
            areas[area].region++;
            for (int n : grid[p].around4) {
                if (border[n] && grid[n].state == 0) {
                    areas[area].doors.emplace_back(n);
                }
                if (border[n] || color[n] != -1 || grid[n].state == 1)continue;
                q.push(n);
            }
        }
    }
    REP(i, N) {
        if (color[animals[i].pos] != -1) {
            areas[color[animals[i].pos]].num_a++;
            if (kind[i] == 3)areas[color[animals[i].pos]].num_d++;
            else if (kind[i] == 4)areas[color[animals[i].pos]].num_c++;
            else if (kind[i] == 2)areas[color[animals[i].pos]].num_r++;
        }
    }
    REP(i, M) {
        if (color[humans[i].pos] != -1) {
            areas[color[humans[i].pos]].num_h++;
        }
    }
}

void set_comment() {
    REP(i, M) {
        int npos = humans[i].pos;
        cout << "#human" << i + 1 << " pos(" << npos / H + 1 << "," << npos % H + 1 << "),";
        if (aim[i] != -1)cout << "target(" << (aim[i] / H) + 1 << ", " << (aim[i] % H) + 1 << "),";
        else cout << "target(" << -1 << ", " << -1 << "),";
        cout << "work(" << work[i] << ")" << endl;
    }
}

void final_log() {
    score = get_score();
    get_time();

    //cerr << "time:  " << int(t_diff) << " ms" << endl;
    cerr << "N   :  " << N << endl;
    cerr << "M   :  " << M << endl;
    cerr << "dog :  " << dog << endl;
    cerr << "cat :  " << cat << endl;
}

void setTask() {
    task = V<int>(M, 14);
    int left = H, right = -1;
    V<int> highline,lowline;
    REP(i, N) {
        int npos = animals[i].pos;
        if (color[npos] != -1 && SZ(areas[color[npos]].doors) == 0)continue;
        chmin(left, npos % H);
        chmax(right, npos % H);
        if(0<=npos/H&&npos/H<=29)highline.push_back(npos % H);
        if(0<=npos/H&&npos/H<=29)lowline.push_back(npos % H);
    }
    sort(ALL(highline));
    sort(ALL(lowline));
    highline.erase(unique(ALL(highline)), highline.end());
    lowline.erase(unique(ALL(lowline)), lowline.end());

    V<P> low, high;
    REP(i, M) {
        int npos = humans[i].pos;
        
        if (npos / H < 15)high.push_back(MP(npos % H, i));
        else low.push_back(MP(npos % H, i));
    }
    sort(ALL(high));
    sort(ALL(low));

    if (SZ(highline) > 0) {
        if (SZ(high) >= SZ(highline)) {
            REP(i, SZ(high))task[high[i].second] = highline[i % SZ(highline)];
        }
        else if (SZ(high) == 1) {
            task[high[0].second] = highline[0];
        }
        else if (SZ(high) == 2) {
            task[high[0].second] = highline[0];
            task[high[1].second] = highline[SZ(highline) - 1];
        }
        else if (SZ(high) > 2) {
            task[high[0].second] = highline[0];
            task[high[SZ(high) - 1].second] = highline[SZ(highline) - 1];
            FOR(i, 1, SZ(high) - 2) {
                int t = SZ(highline) * i / (SZ(high) - 1);
                task[high[i].second] = highline[t];
            }
        }
    }
    if (SZ(lowline) > 0) {
        if (SZ(low) >= SZ(lowline)) {
            REP(i, SZ(low))task[low[i].second] = lowline[i % SZ(lowline)];
        }
        else if (SZ(low) == 1) {
            task[low[0].second] = lowline[0];
        }
        else if (SZ(low) == 2) {
            task[low[0].second] = lowline[0];
            task[low[1].second] = lowline[SZ(lowline) - 1];
        }
        else if (SZ(low) > 2) {
            task[low[0].second] = lowline[0];
            task[low[SZ(low) - 1].second] = lowline[SZ(lowline) - 1];
            FOR(i, 1, SZ(low) - 2) {
                int t = SZ(lowline) * i / (SZ(low) - 1);
                task[low[i].second] = lowline[t];
            }
        }
    }
}

void setTask2() {
    task = V<int>(M, 14);
    V<int> highline, lowline, midline;
    REP(i, N) {
        int npos = animals[i].pos;
        if (color[npos] != -1 && SZ(areas[color[npos]].doors) == 0)continue;
        if (npos / H <= 15)highline.push_back(npos % H);
        if (5 <= npos / H && npos / H <= 24)midline.push_back(npos % H);
        if (13 <= npos / H)lowline.push_back(npos % H);
    }
    sort(ALL(highline));
    sort(ALL(midline));
    sort(ALL(lowline));
    highline.erase(unique(ALL(highline)), highline.end());
    midline.erase(unique(ALL(midline)), midline.end());
    lowline.erase(unique(ALL(lowline)), lowline.end());

    V<P> low, high, mid;
    REP(i, M) {
        int npos = humans[i].pos;
        if (npos / H <= 9)high.push_back(MP(npos % H, i));
        else if (npos / H <= 19)mid.push_back(MP(npos % H, i));
        else low.push_back(MP(npos % H, i));
    }
    sort(ALL(high));
    sort(ALL(mid));
    sort(ALL(low));

    if (SZ(highline) > 0) {
        if (SZ(high) >= SZ(highline)) {
            REP(i, SZ(high))task[high[i].second] = highline[i % SZ(highline)];
        }
        else if (SZ(high) == 1) {
            task[high[0].second] = highline[0];
        }
        else if (SZ(high) == 2) {
            task[high[0].second] = highline[0];
            task[high[1].second] = highline[SZ(highline) - 1];
        }
        else if (SZ(high) > 2) {
            task[high[0].second] = highline[0];
            task[high[SZ(high) - 1].second] = highline[SZ(highline) - 1];
            FOR(i, 1, SZ(high) - 2) {
                int t = SZ(highline) * i / (SZ(high) - 1);
                task[high[i].second] = highline[t];
            }
        }
    }

    if (SZ(midline) > 0) {
        if (SZ(mid) >= SZ(midline)) {
            REP(i, SZ(mid))task[mid[i].second] = midline[i % SZ(midline)];
        }
        else if (SZ(mid) == 1) {
            task[mid[0].second] = midline[0];
        }
        else if (SZ(mid) == 2) {
            task[mid[0].second] = midline[0];
            task[mid[1].second] = midline[SZ(midline) - 1];
        }
        else if (SZ(mid) > 2) {
            task[mid[0].second] = midline[0];
            task[mid[SZ(mid) - 1].second] = midline[SZ(midline) - 1];
            FOR(i, 1, SZ(mid) - 2) {
                int t = SZ(midline) * i / (SZ(mid) - 1);
                task[mid[i].second] = midline[t];
            }
        }
    }

    if (SZ(lowline) > 0) {
        if (SZ(low) >= SZ(lowline)) {
            REP(i, SZ(low))task[low[i].second] = lowline[i % SZ(lowline)];
        }
        else if (SZ(low) == 1) {
            task[low[0].second] = lowline[0];
        }
        else if (SZ(low) == 2) {
            task[low[0].second] = lowline[0];
            task[low[1].second] = lowline[SZ(lowline) - 1];
        }
        else if (SZ(low) > 2) {
            task[low[0].second] = lowline[0];
            task[low[SZ(low) - 1].second] = lowline[SZ(lowline) - 1];
            FOR(i, 1, SZ(low) - 2) {
                int t = SZ(lowline) * i / (SZ(low) - 1);
                task[low[i].second] = lowline[t];
            }
        }
    }
}

void setTask3() {
    task = V<int>(M, 14);
    V<int> oneline, twoline, threeline, fourline;
    REP(i, N) {
        int npos = animals[i].pos;
        if (color[npos] != -1 && SZ(areas[color[npos]].doors) == 0)continue;
        if (npos / H <= 12)oneline.push_back(npos % H);
        if (3 <= npos / H && npos / H <= 19)twoline.push_back(npos % H);
        if (10 <= npos / H && npos / H <= 26)threeline.push_back(npos % H);
        if (17 <= npos / H)fourline.push_back(npos % H);
    }
    sort(ALL(oneline));
    sort(ALL(twoline));
    sort(ALL(threeline));
    sort(ALL(fourline));

    oneline.erase(unique(ALL(oneline)), oneline.end());
    twoline.erase(unique(ALL(twoline)), twoline.end());
    threeline.erase(unique(ALL(threeline)), threeline.end());
    fourline.erase(unique(ALL(fourline)), fourline.end());

    V<P> one, two, three, four;
    REP(i, M) {
        int npos = humans[i].pos;
        if (npos / H <= 7)one.push_back(MP(npos % H, i));
        else if (npos / H <= 14)two.push_back(MP(npos % H, i));
        else if (npos / H <= 21)three.push_back(MP(npos % H, i));
        else four.push_back(MP(npos % H, i));
    }
    sort(ALL(one));
    sort(ALL(two));
    sort(ALL(three));
    sort(ALL(four));

    if (SZ(oneline) > 0) {
        if (SZ(one) >= SZ(oneline)) {
            REP(i, SZ(one))task[one[i].second] = oneline[i % SZ(oneline)];
        }
        else if (SZ(one) == 1) {
            task[one[0].second] = oneline[0];
        }
        else if (SZ(one) == 2) {
            task[one[0].second] = oneline[0];
            task[one[1].second] = oneline[SZ(oneline) - 1];
        }
        else if (SZ(one) > 2) {
            task[one[0].second] = oneline[0];
            task[one[SZ(one) - 1].second] = oneline[SZ(oneline) - 1];
            FOR(i, 1, SZ(one) - 2) {
                int t = SZ(oneline) * i / (SZ(one) - 1);
                task[one[i].second] = oneline[t];
            }
        }
    }

    if (SZ(twoline) > 0) {
        if (SZ(two) >= SZ(twoline)) {
            REP(i, SZ(two))task[two[i].second] = twoline[i % SZ(twoline)];
        }
        else if (SZ(two) == 1) {
            task[two[0].second] = twoline[0];
        }
        else if (SZ(two) == 2) {
            task[two[0].second] = twoline[0];
            task[two[1].second] = twoline[SZ(twoline) - 1];
        }
        else if (SZ(two) > 2) {
            task[two[0].second] = twoline[0];
            task[two[SZ(two) - 1].second] = twoline[SZ(twoline) - 1];
            FOR(i, 1, SZ(two) - 2) {
                int t = SZ(twoline) * i / (SZ(two) - 1);
                task[two[i].second] = twoline[t];
            }
        }
    }

    if (SZ(threeline) > 0) {
        if (SZ(three) >= SZ(threeline)) {
            REP(i, SZ(three))task[three[i].second] = threeline[i % SZ(threeline)];
        }
        else if (SZ(three) == 1) {
            task[three[0].second] = threeline[0];
        }
        else if (SZ(three) == 2) {
            task[three[0].second] = threeline[0];
            task[three[1].second] = threeline[SZ(threeline) - 1];
        }
        else if (SZ(three) > 2) {
            task[three[0].second] = threeline[0];
            task[three[SZ(three) - 1].second] = threeline[SZ(threeline) - 1];
            FOR(i, 1, SZ(three) - 2) {
                int t = SZ(threeline) * i / (SZ(three) - 1);
                task[three[i].second] = threeline[t];
            }
        }
    }

    if (SZ(fourline) > 0) {
        if (SZ(four) >= SZ(fourline)) {
            REP(i, SZ(four))task[four[i].second] = fourline[i % SZ(fourline)];
        }
        else if (SZ(four) == 1) {
            task[four[0].second] = fourline[0];
        }
        else if (SZ(four) == 2) {
            task[four[0].second] = fourline[0];
            task[four[1].second] = fourline[SZ(fourline) - 1];
        }
        else if (SZ(four) > 2) {
            task[four[0].second] = fourline[0];
            task[four[SZ(four) - 1].second] = fourline[SZ(fourline) - 1];
            FOR(i, 1, SZ(four) - 2) {
                int t = SZ(fourline) * i / (SZ(four) - 1);
                task[four[i].second] = fourline[t];
            }
        }
    }
}

void threeStep() {
    int upperNum = 0;
    int lowerNum = 0;
    int alive = 0;
    int hole = 0;
    REP(i, M) {
        if (humans[i].pos / H < 15)upperNum++;
        else lowerNum++;
    }
    REP(j, H) {
        REP(i, 3) {
            border[Pos(8 + i, j)] = true;
            border[Pos(19 + i, j)] = true;
        }
        if (2 <= j && j <= 27) {
            if (j % 3 == 2) {
                if(grid[Pos(11,j-1)].state==1&& grid[Pos(18, j - 1)].state == 1)blueprint[Pos(10, j)] = true;
            }
            else if (j % 3 == 0) {
                if(grid[Pos(11, j + 1)].state == 1 && grid[Pos(18, j + 1)].state == 1)blueprint[Pos(19, j)] = true;
            }
        }

        if (j % 3 != 1)continue;
        REP(i, H)border[Pos(i, j)] = true;
        blueprint[Pos(1, j)] = (grid[Pos(0, j)].state == 0 ? 1 : 0);
        blueprint[Pos(28, j)] = (grid[Pos(29, j)].state == 0 ? 1 : 0);
        if (j % 3 != 1 && grid[Pos(10, j)].state == 1)hole++;
        if (j % 6 == 1)blueprint[Pos(12, j)] = (grid[Pos(11, j)].state == 0 ? 1 : 0);
        else if (j % 6 == 4)blueprint[Pos(17, j)] = (grid[Pos(18, j)].state == 0 ? 1 : 0);
    }

    Action = "";
    getArea();

    V<bool> active(SZ(areas));
    REP(i, N) {
        int npos = animals[i].pos;
        if (color[npos] != -1 && SZ(areas[color[npos]].doors) == 0)continue;
        alive++;
        if (color[npos] != -1)active[color[npos]] = true;
        for (auto x : grid[npos].newAround4) {
            if (color[x] != -1)active[color[x]] = true;
            for (auto y : grid[x].newAround4) {
                if (y == npos)continue;
                if (color[y] != -1)active[color[y]] = true;
                for (auto z : grid[y].newAround4) {
                    if (x == z)continue;
                    if (color[z] != -1)active[color[z]] = true;
                    for (auto v : grid[z].newAround4) {
                        if (y == v)continue;
                        if (color[v] != -1)active[color[v]] = true;
                        // for (auto u : grid[v].newAround4) {
                        //     if (u == z)continue;
                        //     if (color[u] != -1)active[color[u]] = true;
                        // }
                    }
                }
            }
        }
    }

    REP(i, SZ(areas)) {
        auto a = areas[i];
        if (!active[i] && a.num_a == 0 && M > 4) {
            for (auto x : a.doors)blueprint[x] = 0;
        }
        else if (SZ(a.doors) == 0 || SZ(a.doors) > 2) {
            continue;
        }
        else if (hole >= 19 && (a.doors[0] / H == 10 || a.doors[0] / H == 19)) {
            for (auto x : a.doors)blueprint[x] = 0;
        }
        else if (a.num_a > 0 && a.num_h == 0) {
            if (Turn < PARAM[0]) {
                if (a.num_a > 1) {
                    for (auto x : a.doors) {
                        if (a.num_d > 0 || a.num_c > 0||a.num_a>1)blueprint[x] = 2;
                        else blueprint[x] = 1;
                    }
                }
                else for (auto x : a.doors)blueprint[x] = 0;
            }
            else {
                for (auto x : a.doors) {
                    if (a.num_d > 0 || a.num_c > 0||a.num_a>1)blueprint[x] = 2;
                    else blueprint[x] = 1;
                }
            }
        }
        else {
            for (auto x : a.doors)blueprint[x] = 0;
        }
    }

    set_target();
    setTask();
    set_comment();

    REP(i, M) {
        int npos = humans[i].pos;
        if (alive == 0) {
            humans[i].move(4);
            continue;
        }
        if (work[i] == 0 && ((npos % H) % 3 == 1) && (npos / H == 1 || npos / H == 28
            || (npos / H == 12 && (npos % H) % 6 == 1) || (npos / H == 17 && (npos % H) % 6 == 4))) {
            if (npos / H == 1) {
                if (grid[npos + vec[0]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 1;
                    humans[i].block(0);
                }
            }
            else if (npos / H == 12) {
                if (grid[npos + vec[0]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 2;
                    humans[i].block(0);
                }
            }
            else if (npos / H == 28) {
                if (grid[npos + vec[2]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 3;
                    humans[i].block(2);
                }
            }
            else if (npos / H == 17) {
                if (grid[npos + vec[2]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 4;
                    humans[i].block(2);
                }
            }
        }
        else if (work[i] == 0) {
            if (aim[i] / H == 1 || aim[i] / H == 28 || aim[i] / H == 12 || aim[i] / H == 17) {
                humans[i].move(humans[i].bfs(aim[i]));
            }
            else {
                bool ok = false;
                for (auto x : grid[npos].around4) {
                    if (x == aim[i]) {
                        if (aim[i] / H == 10 && (aim[i] % 3 == 0 || aim[i] % H == 29)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(19, aim[i] % H);
                            else bro = Pos(19, aim[i] % H - 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 0) {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else if (aim[i] / H == 19 && (aim[i] % 3 == 2 || aim[i] % H == 0)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(10, aim[i] % H);
                            else bro = Pos(10, aim[i] % H + 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 29) {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else {
                            humans[i].block(Dir(npos, aim[i]));
                        }
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    if (aim[i] != -1)humans[i].move(humans[i].bfs(aim[i]));
                    else {
                        if (npos / H != 9 && npos / H != 20) {
                            if (npos / H < 9)humans[i].move(humans[i].bfs(Pos(9, npos % H)));
                            else if (npos / H > 20)humans[i].move(humans[i].bfs(Pos(20, npos % H)));
                            else {
                                if (abs(upperNum - lowerNum) < 3) {
                                    if (npos / H >= 15)humans[i].move(humans[i].bfs(Pos(20, npos % H)));
                                    else humans[i].move(humans[i].bfs(Pos(9, npos % H)));
                                }
                                else {
                                    if (upperNum > lowerNum)humans[i].move(humans[i].bfs(Pos(20, npos % H)));
                                    else humans[i].move(humans[i].bfs(Pos(9, npos % H)));
                                }
                            }
                        }
                        else {
                            if (task[i] > npos% H)humans[i].move(1);
                            else if (task[i] == npos % H)humans[i].move(4);
                            else if (task[i] < npos % H)humans[i].move(3);
                        }
                    }
                }
            }
        }
        else {
            if (work[i] == 1) {
                if (npos / H == 8) {
                    if (grid[npos + vec[0]].state == 0) {
                        humans[i].block(0);
                        if (grid[npos + vec[0]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[0]].state == 1)humans[i].move(2);
                else humans[i].block(0);
            }
            else if (work[i] == 2) {
                if (npos / H == 19) {
                    if (grid[npos + vec[0]].state == 0) {
                        humans[i].block(0);
                        if (grid[npos + vec[0]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[0]].state == 1)humans[i].move(2);
                else humans[i].block(0);
            }
            else if (work[i] == 3) {
                if (npos / H == 21) {
                    if (grid[npos + vec[2]].state == 0) {
                        humans[i].block(2);
                        if (grid[npos + vec[2]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[2]].state == 1)humans[i].move(0);
                else humans[i].block(2);
            }
            else if (work[i] == 4) {
                if (npos / H == 10) {
                    if (grid[npos + vec[2]].state == 0) {
                        humans[i].block(2);
                        if (grid[npos + vec[2]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[2]].state == 1)humans[i].move(0);
                else humans[i].block(2);
            }
        }
    }
}

void fourStep() {
    int highN = 0, midN = 0, lowN = 0;
    REP(i, M) {
        if (humans[i].pos / H <=9)highN++;
        else if (humans[i].pos / H <= 19)midN++;
        else lowN++;
    }
    REP(j, H) {
        REP(i, 3) {
            border[Pos(5 + i, j)] = true;
            border[Pos(13 + i, j)] = true;
            border[Pos(22 + i, j)] = true;
        }
        if (2 <= j && j <= 27) {
            if (j % 3 == 2) {
                if(grid[Pos(8,j-1)].state==1&& grid[Pos(12, j - 1)].state == 1)blueprint[Pos(7, j)] = true;
                if(grid[Pos(16,j-1)].state==1&& grid[Pos(21, j - 1)].state == 1)blueprint[Pos(15, j)] = true;
            }
            else if (j % 3 == 0) {
                if(grid[Pos(8, j + 1)].state == 1 && grid[Pos(12, j + 1)].state == 1)blueprint[Pos(13, j)] = true;
                if(grid[Pos(16, j + 1)].state == 1 && grid[Pos(21, j + 1)].state == 1)blueprint[Pos(22, j)] = true;
            }
        }

        if (j % 3 != 1)continue;
        REP(i, H)border[Pos(i, j)] = true;
        blueprint[Pos(1, j)] = (grid[Pos(0, j)].state == 0 ? 1 : 0);
        blueprint[Pos(28, j)] = (grid[Pos(29, j)].state == 0 ? 1 : 0);
        blueprint[Pos(9, j)] = (grid[Pos(8, j)].state == 0 ? 1 : 0);
        blueprint[Pos(20, j)] = (grid[Pos(21, j)].state == 0 ? 1 : 0);
    }
    int alive = 0;
    Action = "";
    getArea();

    V<bool> active(SZ(areas));
    REP(i, N) {
        int npos = animals[i].pos;
        if (color[npos] != -1 && SZ(areas[color[npos]].doors) == 0)continue;
        alive++;
        if (color[npos] != -1)active[color[npos]] = true;
        for (auto x : grid[npos].newAround4) {
            if (color[x] != -1)active[color[x]] = true;
            for (auto y : grid[x].newAround4) {
                if (y == npos)continue;
                if (color[y] != -1)active[color[y]] = true;
                for (auto z : grid[y].newAround4) {
                    if (x == z)continue;
                    if (color[z] != -1)active[color[z]] = true;
                    for (auto v : grid[z].newAround4) {
                        if (y == v)continue;
                        if (color[v] != -1)active[color[v]] = true;
                        /*for (auto u : grid[v].newAround4) {
                            if (u == z)continue;
                            if (color[u] != -1)active[color[u]] = true;
                        }*/
                    }
                }
            }
        }
    }

    REP(i, SZ(areas)) {
        auto a = areas[i];
        if (!active[i] && a.num_a == 0 && M > 4) {
            for (auto x : a.doors)blueprint[x] = 0;
        }
        else if (SZ(a.doors) == 0 || SZ(a.doors) > 2) {
            continue;
        }
        else if (a.num_a > 0 && a.num_h == 0) {
            if (Turn < PARAM[M - 5]) {
                if (a.num_a > 1) {
                    for (auto x : a.doors) {
                        if (a.num_d > 0 || a.num_c > 0||a.num_a>1)blueprint[x] = 2;
                        else blueprint[x] = 1;
                    }
                }
                else for (auto x : a.doors)blueprint[x] = 0;
            }
            else {
                for (auto x : a.doors) {
                    if (a.num_d > 0 || a.num_c > 0||a.num_a>1)blueprint[x] = 2;
                    else blueprint[x] = 1;
                }
            }
        }
        else {
            for (auto x : a.doors)blueprint[x] = 0;
        }
    }

    set_target2();
    setTask2();
    set_comment();

    REP(i, M) {
        int npos = humans[i].pos;
        if (alive == 0) {
            humans[i].move(4);
            continue;
        }
        if (work[i] == 0 && (npos % H) % 3 == 1 &&
            (npos / H == 1 || npos / H == 28 || npos / H == 9 || npos / H == 20)) {
            if (npos / H == 1) {
                if (grid[npos + vec[0]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 1;
                    humans[i].block(0);
                }
            }
            else if (npos / H == 9) {
                if (grid[npos + vec[0]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 2;
                    humans[i].block(0);
                }
            }
            else if (npos / H == 20) {
                if (grid[npos + vec[2]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 4;
                    humans[i].block(2);
                }
            }
            else {
                if (grid[npos + vec[2]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 3;
                    humans[i].block(2);
                }
            }
        }
        else if (work[i] == 0) {
            if (aim[i] / H == 1 || aim[i] / H == 28 || aim[i] / H == 9 || aim[i] / H == 20) {
                humans[i].move(humans[i].bfs(aim[i]));
            }
            else {
                bool ok = false;
                for (auto x : grid[npos].around4) {
                    if (x == aim[i]) {
                        if (aim[i] / H == 7 && (aim[i] % 3 == 0 || aim[i] % H == 29)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(13, aim[i] % H);
                            else bro = Pos(13, aim[i] % H - 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 0) {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else if (aim[i] / H == 13 && (aim[i] % 3 == 2 || aim[i] % H == 0)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(7, aim[i] % H);
                            else bro = Pos(7, aim[i] % H + 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 29) {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else if (aim[i] / H == 15 && (aim[i] % 3 == 0 || aim[i] % H == 29)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(22, aim[i] % H);
                            else bro = Pos(22, aim[i] % H - 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 0) {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else if (aim[i] / H == 22 && (aim[i] % 3 == 2 || aim[i] % H == 0)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(15, aim[i] % H);
                            else bro = Pos(15, aim[i] % H + 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 29) {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else {
                            humans[i].block(Dir(npos, aim[i]));
                        }
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    if (aim[i] != -1) {
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                    else {
                        if (npos / H != 6 && npos / H != 14 && npos / H != 23) {
                            if (npos / H <=9) {
                                if (highN >= midN + 2) {
                                    humans[i].move(humans[i].bfs(Pos(14, npos % H)));
                                }
                                else {
                                    humans[i].move(humans[i].bfs(Pos(6, npos % H)));
                                }
                            }
                            else if (npos / H <=19) {
                                if (midN >= highN + 2) {
                                    humans[i].move(humans[i].bfs(Pos(6, npos % H)));
                                }
                                else if (midN >= lowN + 2) {
                                    humans[i].move(humans[i].bfs(Pos(23, npos % H)));
                                }
                                else {
                                    humans[i].move(humans[i].bfs(Pos(14, npos % H)));
                                }
                            }
                            else {
                                if (lowN >= midN + 2) {
                                    humans[i].move(humans[i].bfs(Pos(14, npos % H)));
                                }
                                else {
                                    humans[i].move(humans[i].bfs(Pos(23, npos % H)));
                                }
                            }
                        }
                        else {
                            if (task[i] > npos% H)humans[i].move(1);
                            else if (task[i] == npos % H)humans[i].move(4);
                            else if (task[i] < npos % H)humans[i].move(3);
                        }
                    }
                }
            }
        }
        else {
            if (work[i] == 1) {
                if (npos / H == 5) {
                    if (grid[npos + vec[0]].state == 0) {
                        humans[i].block(0);
                        if (grid[npos + vec[0]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[0]].state == 1)humans[i].move(2);
                else humans[i].block(0);
            }
            else if (work[i] == 2) {
                if (npos / H == 13) {
                    if (grid[npos + vec[0]].state == 0) {
                        humans[i].block(0);
                        if (grid[npos + vec[0]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[0]].state == 1)humans[i].move(2);
                else humans[i].block(0);
            }
            else if (work[i] == 4) {
                if (npos / H == 15) {
                    if (grid[npos + vec[2]].state == 0) {
                        humans[i].block(2);
                        if (grid[npos + vec[2]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[2]].state == 1)humans[i].move(0);
                else humans[i].block(2);
            }
            else if (work[i] == 3) {
                if (npos / H == 24) {
                    if (grid[npos + vec[2]].state == 0) {
                        humans[i].block(2);
                        if (grid[npos + vec[2]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[2]].state == 1)humans[i].move(0);
                else humans[i].block(2);
            }
        }
    }
}

void fiveStep() {
    int oneN = 0, twoN = 0, threeN = 0, fourN = 0;
    REP(i, M) {
        if (humans[i].pos / H <= 7)oneN++;
        else if (humans[i].pos / H <= 14)twoN++;
        else if (humans[i].pos / H <= 21)threeN++;
        else fourN++;
    }
    REP(j, H) {
        REP(i, 3) {
            border[Pos(3 + i, j)] = true;
            border[Pos(10 + i, j)] = true;
            border[Pos(17 + i, j)] = true;
            border[Pos(24 + i, j)] = true;
        }
        if (2 <= j && j <= 27) {
            if (j % 3 == 2) {
                if(grid[Pos(6,j-1)].state==1&& grid[Pos(9, j - 1)].state == 1)blueprint[Pos(5, j)] = true;
                if(grid[Pos(13,j-1)].state==1&& grid[Pos(16, j - 1)].state == 1)blueprint[Pos(12, j)] = true;
                if(grid[Pos(20,j-1)].state==1&& grid[Pos(23, j - 1)].state == 1)blueprint[Pos(19, j)] = true;
            }
            else if (j % 3 == 0) {
                if(grid[Pos(6, j + 1)].state == 1 && grid[Pos(9, j + 1)].state == 1)blueprint[Pos(10, j)] = true;
                if(grid[Pos(13, j + 1)].state == 1 && grid[Pos(16, j + 1)].state == 1)blueprint[Pos(17, j)] = true;
                if(grid[Pos(20, j + 1)].state == 1 && grid[Pos(23, j + 1)].state == 1)blueprint[Pos(24, j)] = true;
            }
        }

        if (j % 3 != 1)continue;
        REP(i, H)border[Pos(i, j)] = true;
        blueprint[Pos(1, j)] = (grid[Pos(0, j)].state == 0 ? 1 : 0);
        blueprint[Pos(28, j)] = (grid[Pos(29, j)].state == 0 ? 1 : 0);
        blueprint[Pos(7, j)] = (grid[Pos(6, j)].state == 0 ? 1 : 0);
        blueprint[Pos(22, j)] = (grid[Pos(23, j)].state == 0 ? 1 : 0);

        if (j % 6 == 1)blueprint[Pos(14, j)] = (grid[Pos(13, j)].state == 0 ? 1 : 0);
        if (j % 6 == 4)blueprint[Pos(15, j)] = (grid[Pos(16, j)].state == 0 ? 1 : 0);
    }
    int alive = 0;
    Action = "";
    getArea();

    V<bool> active(SZ(areas));
    REP(i, N) {
        int npos = animals[i].pos;
        if (color[npos] != -1 && SZ(areas[color[npos]].doors) == 0)continue;
        alive++;
        if (color[npos] != -1)active[color[npos]] = true;
        for (auto x : grid[npos].newAround4) {
            if (color[x] != -1)active[color[x]] = true;
            for (auto y : grid[x].newAround4) {
                if (y == npos)continue;
                if (color[y] != -1)active[color[y]] = true;
                for (auto z : grid[y].newAround4) {
                    if (x == z)continue;
                    if (color[z] != -1)active[color[z]] = true;
                    //for (auto v : grid[z].newAround4) {
                    //    if (y == v)continue;
                    //    if (color[v] != -1)active[color[v]] = true;
                    //    /*for (auto u : grid[v].newAround4) {
                    //        if (u == z)continue;
                    //        if (color[u] != -1)active[color[u]] = true;
                    //    }*/
                    //}
                }
            }
        }
    }

    REP(i, SZ(areas)) {
        auto a = areas[i];
        if (!active[i] && a.num_a == 0 && M > 4) {
            for (auto x : a.doors)blueprint[x] = 0;
        }
        else if (SZ(a.doors) == 0 || SZ(a.doors) > 2) {
            continue;
        }
        else if (a.num_a > 0 && a.num_h == 0) {
            if (Turn < PARAM[M - 5]) {
                if (a.num_a > 1) {
                    for (auto x : a.doors) {
                        if (a.num_d > 0 || a.num_c > 0||a.num_a>1)blueprint[x] = 2;
                        else blueprint[x] = 1;
                    }
                }
                else for (auto x : a.doors)blueprint[x] = 0;
            }
            else {
                for (auto x : a.doors) {
                    if (a.num_d > 0 || a.num_c > 0||a.num_a>1)blueprint[x] = 2;
                    else blueprint[x] = 1;
                }
            }
        }
        else {
            for (auto x : a.doors)blueprint[x] = 0;
        }
    }

    set_target3();
    setTask3();
    set_comment();

    REP(i, M) {
        int npos = humans[i].pos;
        if (alive == 0) {
            humans[i].move(4);
            continue;
        }
        if (work[i] == 0 && (npos % H) % 3 == 1 &&
            (npos / H == 1 || npos / H == 28 || npos / H == 7 || npos / H == 22 ||
            (npos / H == 14 && npos % H % 6 == 1) || (npos / H == 15 && npos % H % 6 == 4))) {
            if (npos / H == 1) {
                if (grid[npos + vec[0]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 1;
                    humans[i].block(0);
                }
            }
            else if (npos / H == 7) {
                if (grid[npos + vec[0]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 2;
                    humans[i].block(0);
                }
            }
            else if (npos / H == 14) {
                if (grid[npos + vec[0]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 3;
                    humans[i].block(0);
                }
            }
            else if (npos / H == 15) {
                if (grid[npos + vec[2]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 4;
                    humans[i].block(2);
                }
            }
            else if (npos / H == 22) {
                if (grid[npos + vec[2]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 5;
                    humans[i].block(2);
                }
            }
            else if (npos / H == 28) {
                if (grid[npos + vec[2]].state == 1) {
                    humans[i].move(humans[i].bfs(aim[i]));
                }
                else {
                    work[i] = 6;
                    humans[i].block(2);
                }
            }
        }
        else if (work[i] == 0) {
            if (aim[i] / H == 1 || aim[i] / H == 28 || aim[i] / H == 7
                || aim[i] / H == 22 || aim[i] / H == 14 || aim[i] / H == 15) {
                humans[i].move(humans[i].bfs(aim[i]));
            }
            else {
                bool ok = false;
                for (auto x : grid[npos].around4) {
                    if (x == aim[i]) {
                        if (aim[i] / H == 5 && (aim[i] % 3 == 0 || aim[i] % H == 29)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(10, aim[i] % H);
                            else bro = Pos(10, aim[i] % H - 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 0) {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else if (aim[i] / H == 10 && (aim[i] % 3 == 2 || aim[i] % H == 0)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(5, aim[i] % H);
                            else bro = Pos(5, aim[i] % H + 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 29) {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else if (aim[i] / H == 12 && (aim[i] % 3 == 0 || aim[i] % H == 29)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(17, aim[i] % H);
                            else bro = Pos(17, aim[i] % H - 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 0) {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else if (aim[i] / H == 17 && (aim[i] % 3 == 2 || aim[i] % H == 0)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(12, aim[i] % H);
                            else bro = Pos(12, aim[i] % H + 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 29) {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else if (aim[i] / H == 19 && (aim[i] % 3 == 0 || aim[i] % H == 29)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(24, aim[i] % H);
                            else bro = Pos(24, aim[i] % H - 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 0) {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro + H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else if (aim[i] / H == 24 && (aim[i] % 3 == 2 || aim[i] % H == 0)) {
                            int bro;
                            if (aim[i] % H == 0 || aim[i] % H == 29)bro = Pos(19, aim[i] % H);
                            else bro = Pos(19, aim[i] % H + 1);
                            if (grid[bro].state == 1)humans[i].block(Dir(npos, aim[i]));
                            else if (grid[aim[i]].full || grid[bro].full)humans[i].move(4);
                            else {
                                bool on = false;
                                REP(j, M) {
                                    if (aim[i] % H == 29) {
                                        if ((humans[j].pos == (bro - 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                    else {
                                        if ((humans[j].pos == (bro + 1)) || (humans[j].pos == (bro - H))) {
                                            if (aim[j] == bro) {
                                                on = true;
                                                break;
                                            }
                                            else continue;
                                        }
                                    }
                                }
                                if (on) {
                                    humans[i].block(Dir(npos, aim[i]));
                                }
                                else {
                                    humans[i].move(4);
                                }
                            }
                        }
                        else {
                            humans[i].block(Dir(npos, aim[i]));
                        }
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    if (aim[i] != -1)humans[i].move(humans[i].bfs(aim[i]));
                    else {
                        if (npos / H != 4 && npos / H != 11 && npos / H != 18 && npos / H != 25) {
                            if (npos / H <= 7) {
                                if (oneN >= twoN + 2) {
                                    humans[i].move(humans[i].bfs(Pos(11, npos % H)));
                                }
                                else {
                                    humans[i].move(humans[i].bfs(Pos(4, npos % H)));
                                }
                            }
                            else if (npos / H <= 14) {
                                if (twoN >= oneN + 2) {
                                    humans[i].move(humans[i].bfs(Pos(4, npos % H)));
                                }
                                else if (twoN >= threeN + 2) {
                                    humans[i].move(humans[i].bfs(Pos(18, npos % H)));
                                }
                                else {
                                    humans[i].move(humans[i].bfs(Pos(11, npos % H)));
                                }
                            }
                            else if (npos / H <= 21) {
                                if (threeN >= twoN + 2) {
                                    humans[i].move(humans[i].bfs(Pos(11, npos % H)));
                                }
                                else if (threeN >= fourN + 2) {
                                    humans[i].move(humans[i].bfs(Pos(25, npos % H)));
                                }
                                else {
                                    humans[i].move(humans[i].bfs(Pos(18, npos % H)));
                                }
                            }
                            else {
                                if (fourN >= threeN + 2) {
                                    humans[i].move(humans[i].bfs(Pos(18, npos % H)));
                                }
                                else {
                                    humans[i].move(humans[i].bfs(Pos(25, npos % H)));
                                }
                            }
                        }
                        else {
                            if (task[i] > npos% H)humans[i].move(1);
                            else if (task[i] == npos % H)humans[i].move(4);
                            else if (task[i] < npos % H)humans[i].move(3);
                        }
                    }
                }
            }
        }
        else {
            if (work[i] == 1) {
                if (npos / H == 3) {
                    if (grid[npos + vec[0]].state == 0) {
                        humans[i].block(0);
                        if (grid[npos + vec[0]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[0]].state == 1)humans[i].move(2);
                else humans[i].block(0);
            }
            else if (work[i] == 2) {
                if (npos / H == 10) {
                    if (grid[npos + vec[0]].state == 0) {
                        humans[i].block(0);
                        if (grid[npos + vec[0]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[0]].state == 1)humans[i].move(2);
                else humans[i].block(0);
            }
            else if (work[i] == 3) {
                if (npos / H == 17) {
                    if (grid[npos + vec[0]].state == 0) {
                        humans[i].block(0);
                        if (grid[npos + vec[0]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[0]].state == 1)humans[i].move(2);
                else humans[i].block(0);
            }
            else if (work[i] == 4) {
                if (npos / H == 12) {
                    if (grid[npos + vec[2]].state == 0) {
                        humans[i].block(2);
                        if (grid[npos + vec[2]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[2]].state == 1)humans[i].move(0);
                else humans[i].block(2);
            }
            else if (work[i] == 5) {
                if (npos / H == 19) {
                    if (grid[npos + vec[2]].state == 0) {
                        humans[i].block(2);
                        if (grid[npos + vec[2]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[2]].state == 1)humans[i].move(0);
                else humans[i].block(2);
            }
            else if (work[i] == 6) {
                if (npos / H == 26) {
                    if (grid[npos + vec[2]].state == 0) {
                        humans[i].block(2);
                        if (grid[npos + vec[2]].state == 1)work[i] = 0;
                    }
                    else {
                        work[i] = 0;
                        humans[i].move(humans[i].bfs(aim[i]));
                    }
                }
                else if (grid[npos + vec[2]].state == 1)humans[i].move(0);
                else humans[i].block(2);
            }
        }
    }
}

signed main() {
    cin.tie(0);
    ios::sync_with_stdio(false);
    cout << fixed << setprecision(15);
    cerr << fixed << setprecision(15);
    t_start = chrono::system_clock::now();
    get_time();

    init();

    work = V<int>(M);
    Turn = 1;

    FOR(turn, 1, 300) {
        REP(i, N)animals[i].set_full();
        REP(i, M)humans[i].set_full();

        if (M == 5)threeStep();
        else if (M<7)fourStep();
        else fiveStep();

        REP(i, M)humans[i].execute(Action[i]);
        if (Turn == 299)final_log();
        cout << Action << endl;
        cout.flush();

        REP(i, N) {
            string a; cin >> a;
            animals[i].move(a);
        }

        Turn++;
    }

    return 0;
}
