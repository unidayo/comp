#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("inline")

#include <cmath>
#include <string>
#include <array>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <limits>
#include <queue>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <bitset>
#include <chrono>
using namespace std;

template<class F, class S> string in_v_to_str (const pair<F, S> v);
template<class F, class S> string v_to_str (const pair<F, S> v);
string in_v_to_str (const char v) { return "'" + string{v} + "'"; }
string in_v_to_str (const char* v) { return "\"" + string(v) + "\""; }
string in_v_to_str (const string v) { return "\"" + v + "\""; }
template<class T> string in_v_to_str (const T v) { stringstream ss; ss << v; return ss.str(); }
template<class T> string v_to_str (const T v) { stringstream ss; ss << v; return ss.str(); }
template<class T, size_t N> string v_to_str (const array<T, N>& v) { stringstream ss; if (v.size() > 0) { ss << "["; for (size_t i = 0; i < v.size() - 1; ++i) { ss << in_v_to_str(v[i]) << ", "; } ss << in_v_to_str(v[v.size() - 1]) << "]"; } else { ss << "[]"; } return ss.str(); }
template<class T, size_t N> string v_to_str (const array< array<T, N>, N >& v) { stringstream ss; if (v.size() > 0) { ss << "["; for (size_t i = 0; i < v.size() - 1; ++i) { ss << v_to_str(v[i]) << ", "; } ss << v_to_str(v[v.size() - 1]) << "]"; } else { ss << "[-]"; } return ss.str(); }
template<class T> string v_to_str (const vector<T>& v) { stringstream ss; if (v.size() > 0) { ss << "["; for (size_t i = 0; i < v.size() - 1; ++i) { ss << in_v_to_str(v[i]) << ", "; } ss << in_v_to_str(v[v.size() - 1]) << "]"; } else { ss << "[]"; } return ss.str(); }
template<class T> string v_to_str (const vector< vector<T> >& v) { stringstream ss; if (v.size() > 0) { ss << "["; for (size_t i = 0; i < v.size() - 1; ++i) { ss << v_to_str(v[i]) << ", "; } ss << v_to_str(v[v.size() - 1]) << "]"; } else { ss << "[-]"; } return ss.str(); }
template<class T> string v_to_str (const set<T>& v) { stringstream ss; int len = v.size(); ss << (v.size() > 0 ? "{" : "{}"); for (auto& i : v) { ss << in_v_to_str(i) << (len-- > 1 ? ", " : "}"); } return ss.str(); }
template<class K, class V> string v_to_str (const map<K, V>& v) { stringstream ss; int len = v.size(); ss << (v.size() > 0 ? "{" : "{}"); for (auto& i : v) { ss << in_v_to_str(i.first) << " : " << in_v_to_str(i.second) << (len-- > 1 ? ", " : "}"); } return ss.str(); }
template<class T> string v_to_str (const unordered_set<T>& v) { stringstream ss; int len = v.size(); ss << (v.size() > 0 ? "{" : "{}"); for (auto& i : v) { ss << in_v_to_str(i) << (len-- > 1 ? ", " : "}"); } return ss.str(); }
template<class K, class V> string v_to_str (const unordered_map<K, V>& v) { stringstream ss; int len = v.size(); ss << (v.size() > 0 ? "{" : "{}"); for (auto& i : v) { ss << in_v_to_str(i.first) << " : " << in_v_to_str(i.second) << (len-- > 1 ? ", " : "}"); } return ss.str(); }
template<class F, class S> string in_v_to_str (const pair<F, S> v) { stringstream ss; ss << "<" << v_to_str(v.first) << ", " << v_to_str(v.second) << ">"; return ss.str(); }
template<class F, class S> string v_to_str (const pair<F, S> v) { stringstream ss; ss << "<" << v_to_str(v.first) << ", " << v_to_str(v.second) << ">"; return ss.str(); }
string print () { return ""; }
template<typename F, typename... R> string print (const F& f, const R& ...r) { stringstream ss; ss << v_to_str(f); if (sizeof...(r) > 0) { ss << " " << print(r...); } return ss.str(); }

template<typename F, typename... R> void pdebug (const F& f, const R& ...r) {
    stringstream ss;
    ss << v_to_str(f); if (sizeof...(r) > 0) { ss << " " << print(r...); }
    cerr << "<cerr>" << ss.str() << "</cerr>" << endl;
}

template<typename F, typename... R> void fdebug (const F& f, const R& ...r) {
    stringstream ss;
    ss << v_to_str(f); if (sizeof...(r) > 0) { ss << " " << print(r...); }
    cerr << "<cerrfile>" << ss.str() << "</cerrfile>" << endl;
}

template<typename F, typename... R> void tdebug (const F& f, const R& ...r) {
    stringstream ss;
    ss << v_to_str(f); if (sizeof...(r) > 0) { ss << " " << print(r...); }
    cerr << "<cerr>[time]" << ss.str() << "</cerr>" << endl;
}



struct e512pos {
public:
    int x;
    int y;
    e512pos () { this->x = 0; this->y = 0; }
    e512pos (int x, int y) {
        this->x = x;
        this->y = y;
    }
    e512pos operator + (const e512pos& t) { return e512pos(this->x + t.x, this->y + t.y); }
    e512pos operator - (const e512pos& t) { return e512pos(this->x - t.x, this->y - t.y); }
    bool operator == (const e512pos& t) const { return this->x == t.x && this->y == t.y; }
};
namespace std {
    template <> class hash<e512pos> {
    public:
        size_t operator()(const e512pos& t) const{ return t.x<<16 | t.y; }
    };
}
ostream& operator << (ostream& os, const e512pos& p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
};

class StopWatch {
public:
    std::chrono::system_clock::time_point start, tstart, end;
    StopWatch () {
        this->start = std::chrono::system_clock::now();
    }
    inline void stop () {
        this->tstart = std::chrono::system_clock::now();
    }
    inline void resume () {
        this->start += std::chrono::system_clock::now() - this->tstart;
    }
    inline uint64_t get_milli_time () {
        this->end = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    }
};


inline uint32_t xrnd () {
    static uint32_t y = 2463534242;
    y = y ^ (y << 13);
    y = y ^ (y >> 17);
    return y = y ^ (y << 5);
}

int dx[] = {0, 1, 0, -1};
int dy[] = {-1, 0, 1, 0};
inline bool inside (const int x, const int y) {
    return (0 <= x && x < 10 && 0 <= y && y < 10);
}

class MM {
public:
    vector<int> va;
    
    array<array<int, 10>, 10> grid;
    
    
    StopWatch sw;
    
    int bestd = 0;
    array<int, 4> D;
    double DD;
    
    MM () { this->sw = StopWatch(); }
    void input () {
        this->D[0] = 0;
        this->D[1] = 0;
        this->D[2] = 0;
        this->D[3] = 0;
        
        for (int i = 0; i < 100; ++i) {
            int t;
            cin >> t;
            this->va.emplace_back(t);
            this->D[t] += 1;
            
        }
        
        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 10; ++x) {
                this->grid[y][x] = 0;
            }
        }
        // pdebug(D[0], D[1], D[2], D[3]);
        
        
        this->DD = ((double)(this->D[1] * this->D[1]+this->D[2] * this->D[2]+this->D[3] * this->D[3]));
    }
    void update (int turn) {
        int t;
        cin >> t;
        int cnt = 0;
        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 10; ++x) {
                if (this->grid[y][x] == 0) {
                    cnt += 1;
                    if (cnt == t) {
                        this->grid[y][x] = this->va[turn];
                        return;
                    }
                }
            }
        }
    }
    
    void output (int d) {
        cout << "FRBL"[d] << endl;
        cout.flush();
    }
    
    void loop () {
        for (int turn = 0; turn < 100; ++turn) {
            this->update(turn);
            this->solve(turn);
            this->output(this->bestd);
            this->grid = this->collect(this->bestd, this->grid);
            if (turn==99) { pdebug(this->gscore(this->grid)); }
            if (turn==99) { pdebug(this->sw.get_milli_time()); }
            // if (turn==99) { this->vis(turn); }
        }
    }
    
    int gscore (array<array<int, 10>, 10>& g) {
        static array<bitset<10>, 10> use;
        static vector<e512pos> q;
        for (int i = 0; i < 10; ++i) { use[i].reset(); }
        double score = 0;
        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 10; ++x) {
                if (use[y][x]) { continue; }
                if (g[y][x] == 0) { continue; }
                int c = g[y][x];
                int cnt = 0;
                
                q.clear();
                q.emplace_back(x, y);
                while(q.size()>0) {
                    e512pos p = q[q.size()-1];
                    q.pop_back();
                    if (use[p.y][p.x]) { continue; }
                    use[p.y][p.x] = true;
                    cnt += 1;
                    for (int d = 0; d < 4; ++d) {
                        int px = p.x + dx[d];
                        int py = p.y + dy[d];
                        if (!inside(px, py)) { continue; }
                        if (use[py][px]) { continue; }
                        if (g[py][px] != c) { continue; }
                        q.emplace_back(px, py);
                    }
                }
                score += ((double)(cnt*cnt));
            }
        }
        
        
        return score / this->DD * 1000000;
    }
    
    int gscore2 (array<array<int, 10>, 10>& g) {
        static array<bitset<10>, 10> use;
        static vector<e512pos> q;
        for (int i = 0; i < 10; ++i) { use[i].reset(); }
        int ccnt[4] = {0, 0, 0, 0};
        int tcnt = 0;
        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 10; ++x) {
                if (use[y][x]) { continue; }
                if (g[y][x] == 0) { continue; }
                int c = g[y][x];
                int cnt = 0;
                
                q.clear();
                q.emplace_back(x, y);
                while(q.size()>0) {
                    e512pos p = q[q.size()-1];
                    q.pop_back();
                    if (use[p.y][p.x]) { continue; }
                    use[p.y][p.x] = true;
                    cnt += 1;
                    for (int d = 0; d < 4; ++d) {
                        int px = p.x + dx[d];
                        int py = p.y + dy[d];
                        if (!inside(px, py)) { continue; }
                        if (use[py][px]) { continue; }
                        if (g[py][px] != c) { continue; }
                        q.emplace_back(px, py);
                    }
                }
                ccnt[c] = max(ccnt[c], cnt);
                
                if (cnt <= 5) { tcnt += 1; }
            }
        }
        
        return 100 - tcnt;
        
        // return ccnt[1] + ccnt[2] + ccnt[3];
    }
    
    array<array<int, 10>, 10> collect (int d, array<array<int, 10>, 10> g) {
        if (d == 0) {
            for (int x = 0; x < 10; ++x) {
                int p = 0;
                for (int y = 0; y < 10; ++y) {
                    if (g[y][x] > 0) {
                        swap(g[y][x], g[p][x]);
                        p += 1;
                    }
                }
            }
        }
        if (d == 1) {
            for (int y = 0; y < 10; ++y) {
                int p = 9;
                for (int x = 9; x >= 0; --x) {
                    if (g[y][x] > 0) {
                        swap(g[y][x], g[y][p]);
                        p -= 1;
                    }
                }
            }
        }
        
        if (d == 2) {
            for (int x = 0; x < 10; ++x) {
                int p = 9;
                for (int y = 9; y >= 0; --y) {
                    if (g[y][x] > 0) {
                        swap(g[y][x], g[p][x]);
                        p -= 1;
                    }
                }
            }
        }
        if (d == 3) {
            for (int y = 0; y < 10; ++y) {
                int p = 0;
                for (int x = 0; x < 10; ++x) {
                    if (g[y][x] > 0) {
                        swap(g[y][x], g[y][p]);
                        p += 1;
                    }
                }
            }
        }
        return g;
    }
    
    void vis (int turn) {
        fdebug(turn);
        for (int y = 0; y < 10; ++y) {
            string s = "";
            for (int x = 0; x < 10; ++x) {
                if (this->grid[y][x] == 0) {
                    s += " ";
                } else {
                    s += to_string(this->grid[y][x]);
                }
            }
            fdebug(s);
        }
    }
    
    void randomput (int turn, array<array<int, 10>, 10>& g) {
        int cnt = 0;
        int t = 1 + xrnd() % (100-turn);
        for (int y = 0; y < 10; ++y) {
            for (int x = 0; x < 10; ++x) {
                if (g[y][x] == 0) {
                    cnt += 1;
                    if (cnt == t) {
                        g[y][x] = this->va[turn];
                        return;
                    }
                }
            }
        }
    }
    
    
    void solve (int turn) {
        int bestscore = 0;
        int dsum[] = {0, 0, 0, 0};
        
        
        {
            int tt = 1600;
            if (this->sw.get_milli_time() > 1600) { tt = 1000; }
            if (this->sw.get_milli_time() > 1800) { tt = 100; }
            if (this->sw.get_milli_time() > 1900) { tt = 50; }
            // if (turn < 50) { tt = 100; }
            for (int z = 0; z < tt; ++z) {
                for (int i = 0; i < 4; ++i) {
                    array<array<int, 10>, 10> g = this->grid;
                    g = this->collect(i, g);
                    this->randomput(min(turn+1, 99), g);
                    this->randomput(min(turn+1, 99), g);
                    this->randomput(min(turn+1, 99), g);
                    this->randomput(min(turn+1, 99), g);
                    this->randomput(min(turn+1, 99), g);
                    int bscore = 0;
                    for (int j = 0; j < 4; ++j) {
                        array<array<int, 10>, 10> g2 = g;
                        g2 = this->collect(j, g2);
                        int score = this->gscore(g2);
                        if (score > bscore) {
                            bscore = score;
                        }
                    }
                    dsum[i] += bscore;
                }
            }
        }
        
        for (int i = 0; i < 4; ++i) {
            int score = dsum[i];
            if (score > bestscore) {
                this->bestd = i;
                bestscore = score;
            }
        }
        
    }
};

int main () {
    cin.tie(0);
    ios::sync_with_stdio(false);
    
    MM mm;
    mm.input();
    mm.loop();
    return 0;
}

