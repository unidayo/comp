int DBG = 0;
#define TIME_BASED
double GM = 0.5;
double TL = 2.97;
#pragma GCC optimize("O3,Ofast,omit-frame-pointer,unroll-all-loops,tree-loop-vectorize,tree-slp-vectorize")
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include <cstring>
#include <cmath>
#include <cassert>
#include <random>
#include <iomanip>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <bitset>

int STANDARD = 1;
using namespace std;
#define F0(i,n) for (int i=0; i<n; i++)
#define FR(i,n) for (int i=n-1; i>=0; i--)
#define F1(i,n) for (int i=1; i<=n; i++)
#define CL(a,x) memset(x, a, sizeof(x));
#define SZ(x) ((int)x.size())
const int inf = 1000000000;
const double pi = acos(-1.0);
typedef pair<int, int> pii;
typedef long long ll;
typedef unsigned long long ull;
const double EPS = 1e-9;
#define PR(x) cerr << #x << "=" << x << endl
template<class A, class B>
ostream& operator<<(ostream& os, const pair<A, B>& p) { os << "(" << p.first << "," << p.second << ")"; return os; }
template<class A, class B, class C>
ostream& operator<<(ostream& os, const tuple<A, B, C>& p) { os << "(" << get<0>(p) << "," << get<1>(p) << "," << get<2>(p) << ")"; return os; }
istream& operator>>(istream& is, pii& p) { is>>p.first>>p.second; return is; }
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    os << "["; F0(i,SZ(v)) { if (i>0) os << ","; os << v[i]; } os << "]"; return os;
}
template<class T>
ostream& operator<<(ostream& os, const set<T>& v) {
    os << "{"; int f=1; for(auto i:v) { if(f)f=0;else os << ","; cerr << i; } os << "}"; return os;
}
template<class T, class R>
ostream& operator<<(ostream& os, const map<T,R>& v) {
    os << "{"; int f=1; for(auto i:v) { if(f)f=0;else os << ", "; cerr << i.first << ":" << i.second; } os << "}"; return os;
}

#ifdef ATCODERX
inline ll GetTSC() {
    ll lo, hi;
    asm volatile ("rdtsc": "=a"(lo), "=d"(hi));
    return lo + (hi << 32);
}
inline double GetSeconds() {
    return GetTSC() / 3.0e9;
}
#else
chrono::system_clock::time_point init_time = chrono::system_clock::now();
inline double GetSeconds() {
     chrono::system_clock::time_point current_time = chrono::system_clock::now();
     double ret =  chrono::duration_cast<std::chrono::nanoseconds>(current_time - init_time).count();
     return ret * 1e-9;
}
#endif

const int MAX_RAND = 1 << 30;
struct Rand {
    ll x, y, z, w, o;
    Rand() {}
    Rand(ll seed) { reseed(seed); o = 0; }
    inline void reseed(ll seed) { x = 0x498b3bc5 ^ seed; y = 0; z = 0; w = 0;  F0(i, 20) mix(); }
    inline void mix() { ll t = x ^ (x << 11); x = y; y = z; z = w; w = w ^ (w >> 19) ^ t ^ (t >> 8); }
    inline ll rand() { mix(); return x & (MAX_RAND - 1); }
    inline int nextInt(int n) { return rand() % n; }
    inline int nextInt(int L, int R) { return rand()%(R - L + 1) + L; }
    inline int nextBool() { if (o < 4) o = rand(); o >>= 2; return o & 1; }
    double nextDouble() { return rand() * 1.0 / MAX_RAND; }
};
Rand my(2023);
double saveTime;
double t_o[20];
ll c_o[20];
void Init() {
    saveTime = GetSeconds();
    F0(i, 20) t_o[i] = 0.0;
    F0(i, 20) c_o[i] = 0;
}
double Elapsed() { return GetSeconds() - saveTime; }
void Report() {
    double tmp = Elapsed();
    cerr << "-------------------------------------" << endl;
    cerr << "Elapsed time: " << tmp << " sec" << endl;
    double total = 0.0; F0(i, 20) { if (t_o[i] > 0) cerr << "t_o[" << i << "] = " << t_o[i] << endl; total += t_o[i]; } cerr << endl; //if (total > 0) cerr << "Total spent: " << total << endl;
    F0(i, 20) if (c_o[i] > 0) cerr << "c_o[" << i << "] = " << c_o[i] << endl;
    cerr << "-------------------------------------" << endl;
}
struct AutoTimer {
    int x;
    double t;
    AutoTimer(int x) : x(x) {
        t = Elapsed();
    }
    ~AutoTimer() {
        t_o[x] += Elapsed() - t;
    }
};
#define AT(i) AutoTimer a##i(i)
//#define AT(i)

// CONSTANTS
const int N = 64;
const int W = 1000;
const int MAXW = 1 << 10;
int D, n, K;
double e;
int a[N][N], b[N][N], bb[N][N];
int hb[N][N][MAXW], bx[N][N][N], xx[N][N][N], c[N][N], bc[N][N];
pii wki[N][N];
int o[N][N][N], bo[N][N][N];
int part_score[N], p2[N], pot_score[N];
vector<int> P[N][N];
ll bscore, score, rscore;
typedef pair<pii, pii> Rect;
vector<vector<Rect>> bsol, sol;
int max_spot[N], sum_h[N], sum_day[N], max_day[N], min_all_days, max_all_days, avg_all_days, max_all_a;
int gseed, lseed;
int g_score, best_g_score;
int g_e;
bool inited;
int klist[N], kn;
int k2list[N], k2n;
int current_h[N][16], current_sum_h[N];
int sh[N][N];
int new_w[N][16];
int p[N], bp[N], bK;
set<int> S[N][16], BS[N][16], R[N][16];
int current_where[N][N];
int pdiv[N][N][MAXW], pwst[N][N][MAXW];
int max_e;
bool allow_mix, allow_any;
vector<int> remain[N];
set<int> rems[N];
bitset<1001> dp[N][N];

const int LOGN = 1 << 16;
double logs[LOGN];

template<class T> void RS(vector<T>& v) { F1(i, SZ(v)-1) { int j = my.nextInt(0, i); swap(v[i], v[j]); } }
template<class T> T sqr(T x) { return x*x; }

//#define VIS
#ifdef VIS
#include "CImg.h"
using namespace cimg_library;
CImg < unsigned char > img;
CImgDisplay disp;
unsigned char white[] = {255, 255, 255};
unsigned char grey[] = {192, 192, 192};
unsigned char red[] = {255, 128, 64};
unsigned char color1[] = {123, 20, 200};
unsigned char green[] = {128, 255, 128};
unsigned char black[] = {0, 0, 0};
unsigned char rcol[] = {128, 128, 128};
int minx, maxx, miny, maxy;
int MX = 1500;
int MY = 1000;
int undox(int x) { return x * (maxx-minx+1) / MX + minx; }
int undoy(int y) { return y * (maxy-miny+1) / MY + miny; }
int scalex(int x) { return MX*(x-minx)/(maxx-minx+1); }
int scaley(int y) { return MY*(y-miny)/(maxy-miny+1); }
void draw_text(int x, int y, const char* const text) {
    img.draw_text(scalex(x), scaley(y), text, 8, white);
}
void draw_circle(int x, int y, int r, const unsigned char* const color) {
    img.draw_circle(scalex(x), scaley(y), r, color);
}
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, const unsigned char* const color) {
    img.draw_triangle(scalex(x1), scaley(y1), scalex(x2), scaley(y2), scalex(x3), scaley(y3), color, 1);
}
void draw_rectangle(int x1, int y1, int x2, int y2, const unsigned char* const color, const float opacity, const unsigned int pattern) {
    img.draw_rectangle(scalex(x1), scaley(y1), scalex(x2), scaley(y2), color, opacity, pattern);
}
void draw_rectangle(Rect r, const unsigned char* const color, const float opacity, const unsigned int pattern) {
    draw_rectangle(r.first.first, r.first.second, r.second.first, r.second.second, color, opacity, pattern);
}
void draw_rectangle(int x1, int y1, int x2, int y2, const unsigned char* const color) {
    img.draw_rectangle(scalex(x1), scaley(y1), scalex(x2), scaley(y2), color);
}
void draw_line(int x1, int y1, int x2, int y2, const unsigned char* const color, const float opa = 2) {
    img.draw_line(scalex(x1), scaley(y1), scalex(x2), scaley(y2), color, opa);
}
unsigned char* randcol() {
    rcol[0] = rand()%256;
    rcol[1] = rand()%256;
    rcol[2] = rand()%256;
    return rcol;
}

pii scale(pii p) {
    return pii(scalex(p.first), scaley(p.second));
}

void Draw(int score, bool stay) {
    img = CImg<unsigned char>(1700, 1000, 1, 3);

    minx = 0;
    //maxx = 1000 * D + K * 100;
    maxx = 1100 * (D + 1) * K;
    miny = 0;
    maxy = 1000;

    vector<pair<pii, pii>> boundaries;
    boundaries.push_back(make_pair(pii(minx, miny), pii(maxx, maxy)));

    while (1) {
        F0(k, K) {
            pot_score[k] = 0;
            F0(d, D - 1) {
                pot_score[k] += abs(max(c[d][k], 1) - max(c[d + 1][k], 1));
            }
        }

        srand(1);
        img = (unsigned char)0;
        img.draw_rectangle(0, 0, MX, MY, white);

        minx = boundaries.back().first.first;
        miny = boundaries.back().first.second;
        maxx = boundaries.back().second.first;
        maxy = boundaries.back().second.second;


        //int base_sx = 0;
        F0(k, K) {
            int w = p[k];
            F0(d, D) {
                F0(i, c[d][k]) {
                    auto rr = green;
                    //int sx = base_sx + d * w;
                    int sx = 1100 * (k * (D + 1) + d);
                    int sy = bx[d][k][i];
                    int index = o[d][k][i];
                    int used = pdiv[d][index][w];
                    int wasted = bx[d][k][i + 1] - bx[d][k][i] - pdiv[d][index][w];

                    if (used > 2)
                    //draw_rectangle(sx + w/5, bx[d][k][i] + 2, sx + 4*w/5, bx[d][k][i] + used, red);
                    draw_rectangle(sx + 200, bx[d][k][i] + 2, sx + 900, bx[d][k][i] + used, red);
                    if (wasted >= 2)
                    draw_rectangle(sx + 200, bx[d][k][i + 1] - wasted, sx + 900, bx[d][k][i + 1] - 1, green);
                    //draw_rectangle(sx + w/5, bx[d][k][i + 1] - wasted, sx + 4*w/5, bx[d][k][i + 1] - 1, green);
                }
                F0(i, c[d][k] + 1) {
                    auto rr = black;
                    int sx = 1100 * (k * (D + 1) + d);
                    //int sx = base_sx + d * w;
                    int sy = bx[d][k][i];
                    //draw_rectangle(sx + w/5, sy - 1, sx + 4*w/5, sy + 1, rr);
                    draw_rectangle(sx + 200, sy - 1, sx + 900, sy + 1, rr);
                }
            }
            //base_sx += w * D + 100;
        }

        /*
        F0(i, SZ(paths)) {
            draw_line(p1.first, p1.second, p2.first, p2.second, rn, 100.0);
        }
        */

        int hor = 0;
        hor += 50;
        img.draw_text(MX+20, hor, to_string(score).c_str(), white, black, 1, 20);
        int ps_sum = 0;
        F0(k, K) {
            ps_sum += part_score[k];
            hor += 40;
            string s = to_string(part_score[k])+"/"+to_string(pot_score[k])+" "+to_string(k)+"-"+to_string(p[k]);
            img.draw_text(MX+20, hor, s.c_str(), white, black, 1, 14);
        }
        {
            hor += 50;
            string s = "=" + to_string(ps_sum);
            img.draw_text(MX+20, hor, s.c_str(), white, black, 1, 14);
        }
        {
            hor += 50;
            string s = "D=" + to_string(D);
            img.draw_text(MX+20, hor, s.c_str(), white, black, 1, 14);
        }
        {
            hor += 50;
            string s = "N=" + to_string(n);
            img.draw_text(MX+20, hor, s.c_str(), white, black, 1, 14);
        }
        if (stay) {
            hor += 50;
            string s = "DONE!";
            img.draw_text(MX+20, hor, s.c_str(), white, black, 1, 14);
        }
        disp.display(img);
        if (!stay) return;
        int wx = -1, wy = -1;
        while (1) {
            disp.wait();
            if (disp.is_keyQ()) return;
            if (disp.is_keyP()) {
                if (SZ(boundaries) > 1) boundaries.pop_back();
                break;
            }
            if (disp.button()&1) {
                if (wx == -1) {
                    wx = disp.mouse_x();
                    wy = disp.mouse_y();
                } else {
                    int sx = disp.mouse_x();
                    int sy = disp.mouse_y();
                    img.draw_rectangle(wx, wy, sx, sy, grey, 1, 0xFF00FF00);
                    disp.display(img);
                }
            } else {
                if (wx != -1) {
                    int sx = disp.mouse_x();
                    int sy = disp.mouse_y();
                    boundaries.push_back(make_pair(pii(undox(min(wx, sx)), undoy(min(wy, sy))),
                                                   pii(undox(max(wx, sx)), undoy(max(wy, sy)))));
                    wx = -1; wy = -1;
                    break;
                }
            }
        }
    }
}
#else
void Draw(int, bool) {
}
#endif


double Pen(int d, int k, int h) {
    if (S[d][k].empty()) return 0.0;
    double ret = 0;
    for (int i : S[d][k]) ret += pwst[d][i][h];
    return ret;
}

void Prepare() {
    my.reseed(4);
    bscore = inf;
    if (!inited) {
        F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);
        inited = true;
    }

    F0(i, n) max_spot[i] = 0;
    max_all_days = 0;
    min_all_days = W * W;
    avg_all_days = 0;
    max_all_a = 0;
    F0(d, D) {
        sum_h[d] = sum_day[d] = max_day[d] = 0;
    }

    F0(d, D) {
        F0(i, n) {
            max_day[d] = max(max_day[d], a[d][i]);
            sum_day[d] += a[d][i];
            max_spot[i] = max(max_spot[i], a[d][i]);
            sum_h[d] += (a[d][i] + W - 1) / W;
        }
        avg_all_days += sum_day[d];
        max_all_days = max(max_all_days, sum_day[d]);
        min_all_days = min(min_all_days, sum_day[d]);
        max_all_a = max(max_all_a, max_day[d]);
    }
    avg_all_days /= D;
    e = 1.0 * avg_all_days / (W*W);
    max_e = sqrt(1.0 - 1.0 * avg_all_days / (W*W)) * 10000.0 + 0.5;

    //PR(D); PR(n); PR(max_e);

    F0(i, n) a[D][i] = max_spot[i];
    F0(d, D + 1) F0(i, n) F0(x, W + 1) {
        if (x == 0) {
            pdiv[d][i][x] = W + 1 + a[d][i];
            pwst[d][i][x] = 0;
        } else {
            pdiv[d][i][x] = (a[d][i] + x - 1) / x;
            pwst[d][i][x] = x * pdiv[d][i][x] - a[d][i];
        }
    }

    sol.resize(D);
    F0(i, D) sol[i].resize(n);
}

void PrintSol() {
    if (!STANDARD) return;
    F0(d, D) {
        F0(i, n) {
            Rect r = bsol[d][i];
            //if (d == 0 && i == 0) PR(r);
            cout << r.first.first << " " << r.first.second << " "
                 << r.second.first << " " << r.second.second << endl;
        }
    }
}

int PartHeight(int d, int k) {
    if (k == K) return 0;
    if (S[d][k].empty()) return 0;
    int minP = 1; // int minP = (current_sum[k] + W - 1) / W;
    int P = minP, Q = W + 1;
    while (P < Q) {
        int R = (P + Q) / 2;
        int w = 0;
        for (int i : S[d][k]) w += pdiv[d][i][R];
        if (w <= W) {
            Q = R;
        } else {
            P = R + 1;
        }
    }
    return P;
}

template<class T>
int PH(T S) {
    if (S.empty()) return 0;
    int minP = 1;
    int P = minP, Q = W + 1;
    while (P < Q) {
        int R = (P + Q) / 2;
        int w = 0;
        for (int i : S) w += (i + R - 1) / R;
        if (w <= W) {
            Q = R;
        } else {
            P = R + 1;
        }
    }
    return P;
}

// Find valid placement(partition) only one day
bool AnnealPart(int d, int tries) {
    K = 10;
    //while ((K - 1) * (K - 1) >= n) K--;
    F0(k, K) {
        S[d][k].clear();
    }
    F0(i, n) {
        int k = my.nextInt(K);
        S[d][k].insert(i);
        current_where[d][i] = k;
    }
    current_sum_h[d] = 0;
    F0(k, K) {
        current_h[d][k] = PartHeight(d, k);
        current_sum_h[d] += current_h[d][k];
    }
    if (current_sum_h[d] <= W) {
        return true;
    }

    int i, i2, k, k2;

    int itc = 128;
    F0(tid, tries) {
        int tot = 1, acc = 0;
        double T2 = 0.08, T1 = 0.04, TEMP = 0.0, r = 0.0;
        F0(iter, itc) {
            if ((iter & 7) == 0) {
                r = 1.0 * (itc - iter) / itc;
                if (r <= 0) {
                    break;
                }
                TEMP = pow(T1, 1 - r) * pow(T2, r);
            }
            double maxd = TEMP * logs[my.nextInt(LOGN)];

            i = my.nextInt(n);
            k = current_where[d][i];
            k2 = k;
            kn = k2n = 0;
            if (my.nextInt(4) && SZ(S[d][k]) != n) {
                while (k2 == k) {
                    i2 = my.nextInt(n);
                    k2 = current_where[d][i2];
                }
                klist[kn++] = i;
                k2list[k2n++] = i2;
            } else {
                while (k2 == k) {
                    k2 = my.nextInt(K);
                }
                klist[kn++] = i;
            }

            int h_delta = -current_h[d][k] - current_h[d][k2];
            double delta = -Pen(d, k, current_h[d][k]) - Pen(d, k2, current_h[d][k2]);

            F0(index, kn) {
                int i = klist[index];
                S[d][k].erase(i);
                S[d][k2].insert(i);
            }
            F0(index, k2n) {
                int i = k2list[index];
                S[d][k2].erase(i);
                S[d][k].insert(i);
            }

            int h1 = PartHeight(d, k), h2 = PartHeight(d, k2);
            delta += Pen(d, k, h1) + Pen(d, k2, h2);

            h_delta += h1 + h2;

            delta = h_delta + 0.003 * delta;

            if (delta > 0) tot++;
            if (delta <= 0 || delta <= maxd) {
                current_sum_h[d] += h_delta;
                if (delta > 0) acc++;
                F0(index, kn) {
                    current_where[d][klist[index]] = k2;
                }
                F0(index, k2n) {
                    current_where[d][k2list[index]] = k;
                }

                current_h[d][k] = h1;
                current_h[d][k2] = h2;
                if (current_sum_h[d] <= W) {
                    return true;
                }
            } else {
                F0(index, kn) {
                    int i = klist[index];
                    S[d][k2].erase(i);
                    S[d][k].insert(i);
                }
                F0(index, k2n) {
                    int i = k2list[index];
                    S[d][k].erase(i);
                    S[d][k2].insert(i);
                }
            }
        }
        //cerr << acc << "/" << tot << " " << 100.0 * acc / tot << " " << current_sum_h << endl;
        itc *= 2;
    }
    return false;
}

void LoadBestSol() {
    g_score = best_g_score;
    int w1 = 0;
    F0(k, K) {
        int w2 = w1 + p[k];
        F0(d, D) {
            F0(i, bc[d][k]) {
                sol[d][bo[d][k][i]] = Rect({xx[d][k][i], w1}, {xx[d][k][i + 1], w2});
            }
        }
        w1 = w2;
    }
}

void UpdateBest() {
    score = best_g_score + 1;
    if (score < bscore) {
        LoadBestSol();
        bscore = score;
        bsol = sol;
    }
}

void Perfect() {
    vector<int> h;
    h.push_back(0);
    F0(i, n) h.push_back(h.back() + (max_spot[i] + W - 1) / W);

    F0(d, D) {
        F0(i, n) {
            sol[d][i] = Rect({h[i], 0}, {h[i + 1], W});
        }
    }
    score = 1;
    UpdateBest();
}

int NaiveG() {
    int ret = 0;
    F0(k, K) {
        part_score[k] = 0;
        F0(d, D - 1) {
            F1(i, c[d][k] - 1) {
                part_score[k] += !hb[d + 1][k][bx[d][k][i]];
            }
        }
        F1(d, D - 1) {
            F1(i, c[d][k] - 1) {
                part_score[k] += !hb[d - 1][k][bx[d][k][i]];
            }
        }
        ret += part_score[k] * p[k];
    }
    return ret;
}

void HorizontalPacking(int d, int w_limit) {
    int x = 0;
    vector<int> ord;
    F0(i, n) ord.push_back(i);
    //RS(ord);
    sort(ord.rbegin(), ord.rend());
    int last_row = 0;
    for (int i : ord) {
        int height = (a[d][i] + W - 1) / W;
        if (x + height > w_limit) {
            sol[d][i] = Rect({W - 1, last_row}, {W, last_row+1});
            last_row++;
        } else {
            sol[d][i] = Rect({x, 0}, {x + height, W});
            x += height;
        }
    }
}

void PartitionPacking(int d) {
    int current_h = 0;
    F0(k, K) {
        int h = PartHeight(d, k);

        int current_w = 0;
        for (int i : S[d][k]) {
            int w = (a[d][i] + h - 1) / h;
            sol[d][i] = Rect({current_h, current_w}, {current_h + h, current_w + w});
            current_w += w;
        }

        current_h += h;
    }
}

void LoadBS() {
    K = bK;
    F0(k, K) {
        p[k] = bp[k];
        F0(d, D) S[d][k] = BS[d][k];
    }
}

void Grab() {
    set<int> ps;
    F0(d, D) F0(i, n) {
        ps.insert(sol[d][i].first.second);
        ps.insert(sol[d][i].second.second);
    }
    vector<int> pp(ps.begin(), ps.end());
    K = SZ(pp) - 1;
    F0(k, K) p[k] = pp[k + 1] - pp[k];

    int redis = W - pp.back();
    F0(i, redis) p[my.nextInt(K)]++;

    F0(k, K) F0(d, D) {
        S[d][k].clear();
        vector<pii> v;
        F0(i, n) if (sol[d][i].first.second == pp[k]) {
            S[d][k].insert(i);
            v.push_back({sol[d][i].first.first, i});
        }
        sort(v.begin(), v.end());
        c[d][k] = SZ(v);
        F0(i, c[d][k]) {
            o[d][k][i] = v[i].second;
            bx[d][k][i] = v[i].first;
        }
        bx[d][k][c[d][k]] = W;
    }
}

void Validate(int d, int k) {
    int cnt = 0;
    F0(ii, W + 1) if (hb[d][k][ii]) cnt++;
    if (cnt != c[d][k]) {
        cerr << cnt << " " << c[d][k] << endl;
    }
    assert(cnt == c[d][k]);
    F0(ii, c[d][k] + 1) {
        int h = bx[d][k][ii];
        assert(hb[d][k][h] == ii);
        int i = o[d][k][ii];
        if (ii != c[d][k]) {
            assert(wki[d][i] == pii(k, ii));
            assert(p[k] * (bx[d][k][ii + 1] - bx[d][k][ii]) >= a[d][i]);
        }
    }
}

pii va[N*N];
int vn;
vector<pii> vv, v;
bool allow_donations = false;

void MakeChange(int d, int k, int index, int i, int k2, int index2, int i1, int i2) {
    c[d][k2]--;
    hb[d][k2][bx[d][k2][index2 + 1]] = 0;
    for (int ii = index2 + 1; ii <= c[d][k2]; ii++) {
        if (ii != c[d][k2]) {
            o[d][k2][ii] = o[d][k2][ii + 1];
            wki[d][o[d][k2][ii]] = {k2, ii};
        }
        bx[d][k2][ii] = bx[d][k2][ii + 1];
        hb[d][k2][bx[d][k2][ii]] = ii;
    }

    o[d][k2][index2] = i;
    wki[d][i] = {k2, index2};

    c[d][k]++;
    for (int ii = c[d][k]; ii >= index + 2; ii--) {
        if (ii != c[d][k]) {
            o[d][k][ii] = o[d][k][ii - 1];
            wki[d][o[d][k][ii]] = {k, ii};
        }
        bx[d][k][ii] = bx[d][k][ii - 1];
        hb[d][k][bx[d][k][ii]] = ii;
    }
    o[d][k][index] = i1;
    wki[d][i1] = {k, index};

    int h = bx[d][k][index] + pdiv[d][i1][p[k]];

    bx[d][k][index + 1] = h;

    if (d > 0) {
        part_score[k2] -= 1;
        g_score -= p[k2];
    }
    if (d + 1 < D) {
        part_score[k2] -= 1;
        g_score -= p[k2];
    }

    if (d > 0) {
        if (hb[d - 1][k][h]) {
            part_score[k] -= 1;
            g_score -= 1 * p[k];
        } else {
            part_score[k] += 1;
            g_score += 1 * p[k];
        }
    }
    if (d + 1 < D) {
        if (hb[d + 1][k][h]) {
            part_score[k] -= 1;
            g_score -= 1 * p[k];
        } else {
            part_score[k] += 1;
            g_score += 1 * p[k];
        }
    }

    hb[d][k][h] = index + 1;

    o[d][k][index + 1] = i2;
    wki[d][i2] = {k, index + 1};
    //Validate(d, k);
    //Validate(d, k2);
}

void Donations(int k, int k2) {
    F0(d, D) {
        int f = 0;
        v.clear();
        F0(index, c[d][k]) {
            int i = o[d][k][index];
            if (i != 0) v.push_back({i, index});
        }
        sort(v.begin(), v.end());

        vv.clear();

        F0(index2, c[d][k2] - 1) {
            if ((d > 0) && hb[d - 1][k2][bx[d][k2][index2 + 1]]) continue;
            if ((d + 1 < D) && hb[d + 1][k2][bx[d][k2][index2 + 1]]) continue;
            int i1 = o[d][k2][index2];
            int i2 = o[d][k2][index2 + 1];
            vv.push_back(pii(pdiv[d][i1][p[k]] + pdiv[d][i2][p[k]], index2));
        }
        sort(vv.begin(), vv.end());

        for (auto [psum, index2] : vv) {
            if (f) break;
            int i1 = o[d][k2][index2];
            int i2 = o[d][k2][index2 + 1];
            int a2 = (bx[d][k2][index2 + 2] - bx[d][k2][index2]) * p[k2];
            for (int ii = SZ(v) - 1; ii >= 0; ii--) {
                auto [i, index] = v[ii];
                if (a2 >= a[d][i] && pdiv[d][i][p[k]] >= pdiv[d][i1][p[k]] + pdiv[d][i2][p[k]]) {
                    MakeChange(d, k, index, i, k2, index2, i1, i2);
                    f = true;
                    break;
                }
            }
        }
    }
}

void Donations2(int k2, int k) {
    F0(d, D) {
        int f = 0;
        v.clear();
        F0(index, c[d][k]) {
            int i = o[d][k][index];
            v.push_back({i, index});
        }
        sort(v.begin(), v.end());

        vv.clear();
        F0(index2, c[d][k2] - 1) {
            if (d > 0 && hb[d - 1][k2][bx[d][k2][index2 + 1]]) continue;
            if (d + 1 < D && hb[d + 1][k2][bx[d][k2][index2 + 1]]) continue;
            int i1 = o[d][k2][index2];
            int i2 = o[d][k2][index2 + 1];
            vv.push_back(pii(pdiv[d][i1][p[k2]] + pdiv[d][i2][p[k2]], index2));
        }
        sort(vv.rbegin(), vv.rend());

        for (auto [psu, index2] : vv) {
            if (f) break;
            int i1 = o[d][k2][index2];
            int i2 = o[d][k2][index2 + 1];
            int a2 = (bx[d][k2][index2 + 2] - bx[d][k2][index2]) * p[k2];
            F0(ii, SZ(v)) {
                auto [i, index] = v[ii];
                int a1 = bx[d][k][index + 1] - bx[d][k][index];
                if (a2 >= a[d][i]
                    && pdiv[d][i][p[k2]] <= pdiv[d][i1][p[k2]] + pdiv[d][i2][p[k2]]
                    && a1 >= pdiv[d][i1][p[k]] + pdiv[d][i2][p[k]]) {
                    MakeChange(d, k, index, i, k2, index2, i1, i2);
                    f = true;
                    break;
                }
            }
        }
    }
}

bool success;
set<int> candi;
int totm;

bool Relax(int d, int k, int i, int dir) {
    return false;
    assert(i >= 0 && i <= c[d][k]);
    if (i == 0 || i == c[d][k]) return false;
    int w = p[k];
    int d_left = d, d_right = d;
    int h = bx[d][k][i];
    int i1 = h - 1, i2 = h + 1;
    if (d_left > 0 && hb[d_left - 1][k][h]) return false;
    if (d_right + 1 < D && hb[d_right + 1][k][h]) return false;
    while (d_left > 0 && hb[d_left - 1][k][h]) d_left--;
    while (d_right + 1 < D && hb[d_right + 1][k][h]) d_right++;
    int new_h = 0;
    c_o[4]++;
    c_o[5] += d_right-d_left+1;

    if (dir == 0) {
        for (int d = d_left; d <= d_right; d++) {
            int gap1 = pdiv[d][o[d][k][hb[d][k][h] - 1]][w];
            if (bx[d][k][hb[d][k][h] - 1] + gap1 > i1) {
                if (!Relax(d, k, hb[d][k][h] - 1, 0)) return false;
                if (success) return false;
            }
        }
        new_h = i1;
    } else {
        for (int d = d_left; d <= d_right; d++) {
            int gap2 = pdiv[d][o[d][k][hb[d][k][h]]][w];
            if (bx[d][k][hb[d][k][h] + 1] - gap2 < i2) {
                if (!Relax(d, k, hb[d][k][h] + 1, 1)) return false;
                if (success) return false;
            }
        }
        new_h = i2;
    }

    if (d_left > 0 && hb[d_left - 1][k][new_h]) {
        part_score[k]-=2;
        g_score -= 2*p[k];
        success = true;
    }
    if (d_right + 1 < D && hb[d_right + 1][k][new_h]) {
        part_score[k]-=2;
        g_score -= 2*p[k];
        success = true;
    }
    for (int d = d_left; d <= d_right; d++) {
        totm++;
        int i = hb[d][k][h];
        assert(i > 0 && i < c[d][k]);
        hb[d][k][new_h] = i;
        hb[d][k][h] = 0;
        bx[d][k][i] = new_h;
    }
    //F0(d, D) Validate(d, k);
    if (success) return false;
    return true;
}


void Borrow(int k) {
    //AT(1);
    F0(d, D) {
        int sg = 1;
        while (sg) {
            sg = 0;
            vn = 0;
            F0(index, c[d][k]) {
                va[vn++] = {o[d][k][index], index};
            }
            if (!vn) break;
            sort(va, va + vn);
            for (int i2 = 0; i2 < va[vn - 1].first; i2++) {
                auto [k2, index2] = wki[d][i2];
                int area = (bx[d][k2][index2 + 1] - bx[d][k2][index2]) * p[k2];
                if (k2 == k || a[d][va[0].first] > area) continue;
                int P = 0, Q = vn - 1;
                while (P < Q) {
                    int R = (P + Q) / 2;
                    if (va[R].first > i2) {
                        Q = R;
                    } else {
                        P = R + 1;
                    }
                }
                auto [i, index] = va[P];

                if (area >= a[d][i]) {
                    swap(o[d][k2][index2], o[d][k][index]);
                    wki[d][o[d][k2][index2]] = {k2, index2};
                    wki[d][o[d][k][index]] = {k, index};
                    sg = 1;
                    break;
                }
            }
        }
    }
    if (allow_donations) {
        int k2 = my.nextInt(K);
        if (k2 != k) {
            Donations(k, k2);
            Donations2(k, k2);
        }
    }
}


void GridShifts(bool just_draw = false) {
    CL(0, hb);
    F0(k, K) {
        int maxsumh = 0;
        F0(d, D) {
            P[d][k].assign(S[d][k].begin(), S[d][k].end());
            c[d][k] = SZ(S[d][k]);
            if (c[d][k] == 0) continue;
            vector<int> h(c[d][k], 0);
            int sum_h = 0;
            F0(i, c[d][k]) {
                h[i] = pdiv[d][P[d][k][i]][p[k]];
                sum_h += h[i];
            }
            maxsumh = max(maxsumh, sum_h);
            while (sum_h < W) {
                int cutoff = my.nextInt(sum_h);
                F0(i, c[d][k]) {
                    cutoff -= h[i];
                    if (cutoff <= 0) {
                        h[i]++;
                        sum_h++;
                        break;
                    }
                }
            }

            F0(i, c[d][k]) {
                o[d][k][i] = P[d][k][i];
                wki[d][o[d][k][i]] = {k, i};
                bx[d][k][i + 1] = bx[d][k][i] + h[i];
                hb[d][k][bx[d][k][i + 1]] = i + 1;
            }
        }
    }
    g_score = best_g_score = NaiveG();
    F0(k, K) {
        F0(d, D) {
            bc[d][k] = c[d][k];
            F0(i, c[d][k] + 1) {
                xx[d][k][i] = bx[d][k][i];
                bo[d][k][i] = o[d][k][i];
            }
        }
    }

    Draw(g_score, true);
    if (just_draw) {
        cerr << "Exiting" << endl;
        UpdateBest();
        return;
    }

    double startTime = Elapsed();
    PR(startTime);
    int last_k = -1;
    int mult = 4;
    int tot = 1, acc = 0;
    double T2 = 0.20, T1 = 0.09, TEMP = 0.0, r = 0.0;
    int itc = 5000000 * GM;
#ifdef TIME_BASED
    double endTime = TL; itc = 1000000000;
#endif
    F0(iter, itc) {
        if ((iter & 127) == 0) {
#ifdef TIME_BASED
            r = (endTime - Elapsed()) / (endTime - startTime);
#else
            r = 1.0 * (itc - iter) / itc;
#endif
            allow_donations = r < (1.0 - max_e * 0.00015);
            //allow_donations = r < (e - 0.25);
            //allow_donations = max_e <= 3000;

            if (r <= 0) {
                //PR(iter);
                break;
            }
            TEMP = T2 - (T2 - T1) * pow(1 - r, 2.0 - 1.0 * (n - 5) / 45.0);
            //TEMP = pow(T1, 1-r) * pow(T2, r);
        }

        int k = (iter % (mult * D * K)) / (mult * D);
        int d = (iter % (mult * D * K)) % D;
        if ((iter % (mult * D * K) / (D * K)) & 1) d = D - 1 - d;

        if (k != last_k) {
            last_k = k;
            Borrow(k);

            if (0) {
                F0(k, K) p2[k] = part_score[k];
                int n_score = NaiveG();
                int x = 0;
                F0(k, K) if (p2[k] != part_score[k]) {
                    cerr << d << " " << p[k] << " " << k << " " << p2[k] << "!=" << part_score[k] << endl;
                    x = 1;
                }
                if (x) throw;
                if (g_score != n_score) {
                    PR(k);
                    F0(k, K) if (p2[k] != part_score[k]) {
                        cerr << d << " " << p[k] << " " << k << " " << p2[k] << "!=" << part_score[k] << endl;
                    }
                    PR(g_score);
                    PR(NaiveG());
                    throw;
                }
            }
        }

        if (c[d][k] <= 1) continue;
        int i = my.nextInt(1, c[d][k] - 1);
        int w = p[k];

        int h = bx[d][k][i];

        int d_left = d, d_right = d;
        if (1) {
            if (my.nextInt(4) == 0)
            while (d_left > 0 && hb[d_left - 1][k][h]) d_left--;
            if (my.nextInt(4) == 0)
            while (d_right + 1 < D && hb[d_right + 1][k][h]) d_right++;
        }

        bool do_swap = (d_left == d_right) ? (my.nextInt(4) != 0) : (0);
        //do_swap = false;
        if (do_swap) {
            swap(o[d][k][i - 1], o[d][k][i]);
            wki[d][o[d][k][i - 1]] = {k, i - 1};
            wki[d][o[d][k][i]] = {k, i};
        }

        int i1 = -1, i2 = W + 1;
        for (int d = d_left; d <= d_right; d++) {
            int gap1 = pdiv[d][o[d][k][hb[d][k][h] - 1]][w];
            if (bx[d][k][hb[d][k][h] - 1] + gap1 > i1) {
                i1 = bx[d][k][hb[d][k][h] - 1] + gap1;
            }

            int gap2 = pdiv[d][o[d][k][hb[d][k][h]]][w];
            if (bx[d][k][hb[d][k][h] + 1] - gap2 < i2) {
                i2 = bx[d][k][hb[d][k][h] + 1] - gap2;
            }
            if (i1 == i2) break;
        }

        if (!do_swap) {
            if (i1 == i2) {
                continue;
            }
        } else {
            if (i1 > i2 || (i1 == i2 && i1 == h)) {
                swap(o[d][k][i - 1], o[d][k][i]);
                wki[d][o[d][k][i - 1]] = {k, i - 1};
                wki[d][o[d][k][i]] = {k, i};
                continue;
            }
        }

        int new_h = my.nextInt(i1, i2);
        int r = my.nextInt(4);
        if (r == 0) new_h = i1;
        else if (r == 1) new_h = i2;
        else {
            int side = r & 1;
            int e_dir = my.nextInt(2);
            //int tr = (max_e < 2500) ? 1 : 2;
            F0(x, 1) {
                e_dir ^= 1;
                F0(y, 2) {
                    side ^= 1;
                    if (side == 0) {
                        if (d_left > 0 && c[d_left - 1][k] > 1) {
                            int j = (e_dir) ? 0 : c[d_left - 1][k];
                            while (bx[d_left - 1][k][j] < i1) j++;
                            while (bx[d_left - 1][k][j] > i2) j--;
                            if (bx[d_left - 1][k][j] >= i1 && bx[d_left - 1][k][j] <= i2) {
                                new_h = bx[d_left - 1][k][j];
                                x = y = 2;
                                break;
                            }
                        }
                    } else {
                        if (d_right + 1 < D && c[d_right + 1][k] > 1) {
                            int j = (e_dir) ? 0 : c[d_right + 1][k];
                            while (bx[d_right + 1][k][j] < i1) j++;
                            while (bx[d_right + 1][k][j] > i2) j--;
                            if (bx[d_right + 1][k][j] >= i1 && bx[d_right + 1][k][j] <= i2) {
                                new_h = bx[d_right + 1][k][j];
                                x = y = 2;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (!do_swap && new_h == h) continue;

        int delta = ((d_left > 0) ? ((hb[d_left - 1][k][h] != 0) - (hb[d_left - 1][k][new_h] != 0)) : 0)
                  + ((d_right + 1 < D) ? ((hb[d_right + 1][k][h] != 0) - (hb[d_right + 1][k][new_h] != 0)) : 0);
        //if (delta > 0)
        tot++;
        if (delta <= 0 || delta <= TEMP * logs[my.nextInt(LOGN)]) {
            delta *= 2;
            //if (delta > 0)
            acc++;

            part_score[k] += delta;
            g_score += delta * w;
            if (h != new_h) {
                for (int d = d_left; d <= d_right; d++) {
                    int i = hb[d][k][h];
                    hb[d][k][new_h] = i;
                    hb[d][k][h] = 0;
                    bx[d][k][i] = new_h;
                }
            }

            if (acc % 100000 == 0) {
                Draw(g_score, false);
            }
            if (0) {
                F0(k, K) p2[k] = part_score[k];
                if (g_score != NaiveG()) {
                    F0(k, K) if (p2[k] != part_score[k]) {
                        cerr << k << " " << p2[k] << "!=" << part_score[k] << endl;
                    }
                    PR(g_score - delta);
                    PR(delta);
                    PR(g_score);
                    PR(NaiveG());
                    PR(hb[d - 1][k][h]);
                    PR(hb[d - 1][k][new_h]);
                    PR(hb[d + 1][k][h]);
                    PR(hb[d + 1][k][new_h]);
                    throw;
                }
            }

            if (g_score < best_g_score) {
                best_g_score = g_score;
                F0(k, K) {
                    F0(d, D) {
                        bc[d][k] = c[d][k];
                        F0(i, c[d][k] + 1) {
                            xx[d][k][i] = bx[d][k][i];
                            bo[d][k][i] = o[d][k][i];
                        }
                    }
                }
            }
        } else {
            if (do_swap) {
                swap(o[d][k][i - 1], o[d][k][i]);
                wki[d][o[d][k][i - 1]] = {k, i - 1};
                wki[d][o[d][k][i]] = {k, i};
            }
        }
    }
    Draw(g_score, true);
    while (0) {
        F0(k, K) {
            int has_moved = 0;
            success = false;
            F0(d, D) {
                if (c[d][k] <= 1) continue;
                int i = my.nextInt(0, c[d][k] - 1);
                while (Relax(d, k, i, 0)) {
                    has_moved++;
                }
                while (Relax(d, k, i + 1, 1)) {
                    has_moved++;
                }
                if (success) {
                    cerr << "!!!!!!!!!!!!!!!!!!!!!" << endl;
                    throw;
                }
            }
            PR(has_moved);
            Draw(g_score, true);
        }
    }


    if (1) {
        cerr << acc << "/" << tot << " " << 100.0 * acc / tot << " " << score << "/" << bscore << endl;
    }

    LoadBestSol();

    Draw(g_score, true);

}

bool Fix() {
    int cnt = 0;
    F0(d, D) F0(k, K) {
        sh[d][k] = 0;
        for (int i : R[d][k]) sh[d][k] += pdiv[d][i][p[k]];
    }

    int sg = 1;
    while (sg) {
        sg = 0;
        F0(d, D) F0(k, K) if (R[d][k].empty()) {
            F0(k2, K) if (k2 != k && SZ(R[d][k2]) > 1) for (int i : R[d][k2]) if (R[d][k].empty() && pdiv[d][i][p[k]] + sh[d][k] <= W) {
                sh[d][k] += pdiv[d][i][p[k]];
                sh[d][k2] -= pdiv[d][i][p[k2]];
                R[d][k2].erase(i);
                R[d][k].insert(i);
                sg = 1;
                break;
            }
        }
        if (sg) continue;
    }
    F0(d, D) F0(k, K) if (R[d][k].empty()) cnt++;
    if (cnt != 0) return false;
    return true;
}

void Play() {
    rscore = 0;
    int its = 0;
    double bv = inf;
    while (Elapsed() < (rscore == 0 ? 2.6 : 0.4)) {
        its++;
        int w_left = W;
        F0(d, D) {
            rems[d].clear();
            F0(i, n) {
                rems[d].insert(i);
            }
        }
        F0(k, 15) {
            vector<int> hs(D);
            F0(d, D) {
                S[d][k].clear();
                remain[d].assign(rems[d].begin(), rems[d].end());
                for (int i : remain[d]) {
                    hs[d] += pdiv[d][i][w_left];
                }
                //cerr << d << " " << hs[d] << " " << rems[d] << endl;
            }
            sort(hs.rbegin(), hs.rend());

            if (hs[0] == 0) break;
            if (hs[0] <= W) {
                // CAN END ON K
                K = k + 1;

                if (1) {
                    p[k] = w_left;
                    F0(d, D) F0(k, K) R[d][k] = S[d][k];
                    F0(d, D) for (int i : remain[d]) R[d][k].insert(i);
                    if (Fix()) {
                        double av = 0.0;
                        F0(k, K) {
                            av += p[k];
                        }
                        av = sqrt(av / K) / K;
                        if (av < bv) {
                            bK = K;
                            F0(d, D) F0(k, K) BS[d][k] = R[d][k];
                            F0(k, K) bp[k] = p[k];
                            bv = av;
                            rscore = k + 1;
                        }
                    }
                }
            }

            int max_a = 0;
            F0(d, D) if (!remain[d].empty()) {
                max_a = max(max_a, a[d][remain[d].back()]);
            }
            int min_h_needed = (max_a + W - 1) / W;

            if (min_h_needed >= w_left) break;

            p[k] = min_h_needed;
            w_left -= min_h_needed;

            if (my.nextInt(2)) {
                int DIV = max((n - 10) / 2, 1);
                int add = my.nextInt(max(1, w_left / DIV) + 1);
                p[k] += add;
                w_left -= add;
            }

            int w = p[k];
            F0(d, D) {
                if (remain[d].empty()) continue;
                if (a[d][remain[d].back()] != max_a) {
                    int sz = SZ(remain[d]);
                    dp[d][0].reset();
                    dp[d][0].set(0);
                    int f = -1;
                    F0(index, sz) {
                        int i = remain[d][sz - index - 1];
                        dp[d][index + 1] = dp[d][index]
                                         | (dp[d][index] << pdiv[d][i][w]);
                        if (dp[d][index + 1][W]) {
                            f = index + 1;
                            break;
                        }
                    }
                    if (f == -1) f = sz;
                    int mx = W;
                    while (!dp[d][f][mx]) mx--;

                    while (f > 0) {
                        f--;
                        int i = remain[d][sz - f - 1];
                        int l = pdiv[d][i][w];
                        if (!dp[d][f][mx]) {
                            S[d][k].insert(i);
                            rems[d].erase(i);
                            mx -= l;
                        }
                    }
                } else {
                    // Greedy
                    int h_left = W;
                    int sz = SZ(remain[d]);
                    F0(index, sz) {
                        int i = remain[d][sz - index - 1];
                        if (pdiv[d][i][w] <= h_left) {
                            h_left -= pdiv[d][i][w];
                            S[d][k].insert(i);
                            rems[d].erase(i);
                        }
                    }
                }
            }
        }
    }
    PR(max_all_a);
    cerr << "Not found: ";
    PR(its);
}

void SolveBad() {
    F0(d, D) {
        PR(d);
        bool f = AnnealPart(d, 16);
        if (f) PartitionPacking(d);
        if (!f) {
            HorizontalPacking(d, W - 1);
        }
    }
    score = inf - 1;
    UpdateBest();

}

int num_groups;
set<int> g[N][N], bg[N][N]; // d, group_id
int w[N][N];
int gs[N][N], gt[N];
double eval_delta;
multiset<int> gmax[N];
int g1, g2;
double coeff;

void Add(int d, int j, int i) {
    g[d][j].insert(i);
    w[d][i] = j;
    gmax[j].erase(gmax[j].find(gs[d][j]));
    gs[d][j] += a[d][i];
    gt[j] += a[d][i];
    gmax[j].insert(gs[d][j]);
}

void Del(int d, int j, int i) {
    g[d][j].erase(i);
    gmax[j].erase(gmax[j].find(gs[d][j]));
    gs[d][j] -= a[d][i];
    gt[j] -= a[d][i];
    gmax[j].insert(gs[d][j]);
}

void MoveTo(int d, int i, int j1, int j2) {
    Del(d, j1, i);
    Add(d, j2, i);
}

int GroupCost(int j) {
    return *(--gmax[j].end());
}

int Eval() {
    int ret = 0;
    F0(j, num_groups) {
        ret += GroupCost(j);
    }
    return ret;
}

void InitGroups() {
    int dir = 1, at = 0;

    F0(j, num_groups) {
        gmax[j].clear();
        F0(d, D) {
            g[d][j].clear();
            gs[d][j] = 0;
            gmax[j].insert(gs[d][j]);
        }
        gt[j] = 0;
    }

    for (int i = n - 1; i >= 0; i--) {
        F0(d, D) {
            Add(d, at, i);
        }
        at += dir;
        if (at >= num_groups) {
            dir = -1;
            at = num_groups - 1;
        } else if (at < 0) {
            dir = 1;
            at = 0;
        }
    }
}

int MakeMove(int d) {
    int c1 = 0, c2 = 0;
    c1 = GroupCost(g1); c2 = GroupCost(g2);
    int ret = -c1 - c2;
    eval_delta = min(c1, c2) - max(c1, c2);

    F0(k, kn) {
        MoveTo(d, klist[k], g1, g2);
    }
    F0(k, k2n) {
        MoveTo(d, k2list[k], g2, g1);
    }

    c1 = GroupCost(g1); c2 = GroupCost(g2);
    ret += c1 + c2;
    eval_delta -= min(c1, c2) - max(c1, c2);
    eval_delta = coeff * eval_delta + ret;
    return ret;
}

void RevertMove(int d) {
    F0(k, kn) {
        MoveTo(d, klist[k], g2, g1);
    }
    F0(k, k2n) {
        MoveTo(d, k2list[k], g1, g2);
    }
}

void ImproveGroups() {
    int itc = 50000;
    int tot = 1, acc = 0;
    double T2 = 3000.00, T1 = 0.1, TEMP = 0.0, r = 0.0;

    int rel_score = 0, low_score = 0;
    F0(d, D) F0(j, num_groups) bg[d][j] = g[d][j];
    int i, d;

    //allow_mix = max_e <= 3000 ;
    allow_mix = max_e * pow(1.0 * n / D, 0.5) <= 3000;

    F0(iter, itc) {
        if ((iter & 31) == 0) {
            r = 1.0 * (itc - iter) / itc;
            coeff = r * 0.01;
            if (r <= 0) {
                break;
            }
            //TEMP = T1 + (T2 - T1) * r;
            TEMP = pow(T1, 1 - r) * pow(T2, r);
        }


        g1 = my.nextInt(num_groups);
        d = 0;
        F0(d2, D) {
            if (gs[d2][g1] > gs[d][g1]) d = d2;
        }
        i = my.nextInt(n);
        i = my.nextInt(i, n - 1);
        while (w[d][i] != g1) {
            i = my.nextInt(n);
        }

        kn = k2n = 0;

        if (allow_mix && SZ(g[d][g1]) > 1 && my.nextInt(8) == 0) {
            g2 = g1;
            while (g2 == g1) g2 = my.nextInt(num_groups);
            if (SZ(g[d][g1]) <= SZ(g[d][g2]) - 1) continue;
            klist[kn++] = i;
        } else {
            int i2 = i ? my.nextInt(i) : my.nextInt(n);
            g2 = w[d][i2];
            while (g2 == g1) {
                i2 = my.nextInt(n);
                g2 = w[d][i2];
            }
            klist[kn++] = i;
            k2list[k2n++] = i2;
        }

        int delta = MakeMove(d);
        if (eval_delta > 0) tot++;
        if (eval_delta <= 0 || eval_delta <= TEMP * logs[my.nextInt(LOGN)]) {
            rel_score += delta;
            if (rel_score < low_score) {
                low_score = rel_score;
                F0(d, D) F0(j, num_groups) bg[d][j] = g[d][j];
            }
            if (eval_delta > 0) acc++;
        } else {
            RevertMove(d);
        }
    }
    F0(d, D) F0(j, num_groups) g[d][j] = bg[d][j];
    cerr << acc << "/" << tot << " " << 100.0 * acc / tot << " " << rel_score << " " << rel_score - low_score << endl;
}

int GroupW(int cur_w, int j) {
    for (int w = 1; w <= W; w++) {
        int h = 0;
        int max_h = 0;
        F0(d, D) {
            int sum_h = 0;
            for (int i : g[d][j]) {
                int cur_h = pdiv[d][i][w];
                sol[d][i] = Rect({h + sum_h, cur_w}, {h + sum_h + cur_h, cur_w + w});
                sum_h += cur_h;
            }
            max_h = max(max_h, sum_h);
        }
        h += max_h;
        F0(d, D) {
            int i = *(--g[d][j].end());
            sol[d][i].second.first = h;
        }

        if (h <= W) return w;
    }
    return W + 1;
}

void Grouping() {
    for (num_groups = min(n / 2, 15); num_groups >= 1; num_groups--) {
        InitGroups();

        if (num_groups > 1) ImproveGroups();

        if (Eval() > W*W) continue;

        if (0) {
            int h = 0;
            F0(j, num_groups) {
                int max_h = 0;
                F0(d, D) {
                    int sum_h = 0;
                    for (int i : g[d][j]) {
                        int cur_h = pdiv[d][i][W];
                        sol[d][i] = Rect({h + sum_h, 0}, {h + sum_h + cur_h, W});
                        sum_h += pdiv[d][i][W];
                    }
                    max_h = max(max_h, sum_h);
                }
                h += max_h;
                F0(d, D) {
                    int i = *(--g[d][j].end());
                    sol[d][i].second.first = h;
                }
            }
            PR(h);
            if (h <= 1000) {
                UpdateBest();
                return;
            }
        }

        // All Separate
        int w = 0;
        F0(j, num_groups) {
            int cur_w = GroupW(w, j);
            w += cur_w;
        }
        PR(w);
        if (w <= 1000) {
            //exit(0);
            Grab();
            GridShifts(false);
            UpdateBest();
            break;
        }
    }
}

void Solve() {
    Prepare();

    int sum_of_max_h = 0;
    F0(i, n) sum_of_max_h += (max_spot[i] + W - 1) / W;
    if (sum_of_max_h <= W) {
        Perfect();
    } else if (1) {
        Grouping();
        if (bsol.empty()) {
            Play();
            if (rscore != 0) {
                PR(rscore);
                LoadBS();
                GridShifts(false);
                UpdateBest();
            } else {
                SolveBad();
            }
        }
    }

    PR(bscore);
    PrintSol();

    Report();
}


void ReadInput() {
    int w;
    cin >> w >> D >> n;
    F0(d, D) F0(i, n) cin >> a[d][i];
}

int main(int argc, char* argv[]) {
    Init();

    int seed1 = 0, seed2 = 0;
    if(argc>1) {
        if (argc == 2) {
            seed1 = seed2 = atoi(argv[1]);
        } else {
            seed1 = atoi(argv[1]);
            seed2 = atoi(argv[2]);
        }
        STANDARD=0;
    }

    if(STANDARD) {
        ReadInput();
        Solve();
        return 0;
    }

    for (int seed=seed1; seed<=seed2; seed++) {
        gseed=seed;
        if(seed>=0 && seed<1000) {
            char inp[128];
            sprintf(inp, "in/%04d.txt", seed);
            char outp[128];
            sprintf(outp, "out/%04d.txt", seed);
            ignore = freopen(inp, "r", stdin);
            ignore = freopen(outp, "w", stdout);
            ReadInput();
            //cerr << "Seed #" << seed << " ";
            Solve();
            //cerr << bscore;
            //cerr << endl;
            //cout << "Score would be " << bscore << endl;
        } else {
            // Generate
            Rand my(seed);
            D = my.nextInt(5, 50);
            n = my.nextInt(5, 50);
            int e = my.nextInt(500, 5000);
            //int e = my.nextInt(670, 870);
            D = 27; n = 27; e = 650;
            e = 2700;

            g_e = e;
            double e_ratio = e / 10000.0;
            int E = W * W * e_ratio * e_ratio + 0.5;
            //cerr << "Gen seed #" << seed << " ";
            F0(d, D) {
                int Td = my.nextInt(W*W - 3*E/2, W*W - E/2);
                set<int> S;
                S.insert(0);
                S.insert(Td);
                while (SZ(S) <= n) {
                    S.insert(my.nextInt(1, Td - 1));
                }
                vector<int> v(S.begin(), S.end());
                F0(i, n) a[d][i] = v[i + 1] - v[i];
                sort(a[d], a[d] + n);
            }
            Solve();
            cout << "Score = " << bscore << endl;
        }
    }

    return 0;
}
