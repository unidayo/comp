int STANDARD=1;
#include <bits/stdc++.h>
using namespace std;
#define F0(i,n) for (int i=0; i<n; i++)
#define FR(i,n) for (int i=n-1; i>=0; i--)
#define F1(i,n) for (int i=1; i<=n; i++)
#define CL(a,x) memset(x, a, sizeof(x));
#define SZ(x) ((int)x.size())
const int inf = 1000009;
const double pi = acos(-1.0);
typedef pair<int, int> pii;
typedef long long ll;
typedef unsigned long long ull;
const double EPS = 1e-9;
#define PR(x) cerr << #x << "=" << x << endl
ostream& operator<<(ostream& os, const pii& p) { os << "(" << p.first << "," << p.second << ")"; return os; }
istream& operator>>(istream& is, pii& p) { is>>p.first>>p.second; return is; }
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    os << "["; F0(i,SZ(v)) { if (i>0) os << ","; os << v[i]; } os << "]" << endl; return os;
}
void Print(set<int> v) {
    cerr << "{"; int f=1; for(int i:v) { if(f)f=0;else cerr << ","; cerr << i; }
    cerr << "}" << endl;
}

inline ll GetTSC() {
    ll lo, hi;
    asm volatile ("rdtsc": "=a"(lo), "=d"(hi));
    return lo + (hi << 32);
}
inline double GetSeconds() {
    return GetTSC() / 2.75e9;
}

const int MAX_RAND = 1 << 30;
struct Rand {
    ll x, y, z, w, o;
    Rand(ll seed) { reseed(seed); o = 0; }
    inline void reseed(ll seed) { x = 0x498b3bc5 ^ seed; y = 0; z = 0; w = 0;  F0(i, 20) mix(); }
    inline void mix() { ll t = x ^ (x << 11); x = y; y = z; z = w; w = w ^ (w >> 19) ^ t ^ (t >> 8); }
    inline ll rand() { mix(); return x & (MAX_RAND - 1); }
    inline int nextInt(int n) { return rand() % n; }
    inline int nextInt(int L, int R) { return rand()%(R - L + 1) + L; }
    inline int nextBool() { if (o < 4) o = rand(); o >>= 2; return o & 1; }
    double nextDouble() { return rand() * 1.0 / MAX_RAND; }
};
Rand my(2020);
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
    double tmp = Elapsed(); while (Elapsed() < tmp+0.1);
    cerr << "-------------------------------------" << endl;
    cerr << "Elapsed time: " << Elapsed() << " sec" << endl;
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
const int N = 32;
const double DECAY = 0.998;
const int LOGN = 1 << 16;
double logs[LOGN];
enum { UP, RIGHT, DOWN, LEFT };
const int DX[]={-1,0,1,0};
const int DY[]={0,1,0,-1};
const string DS="URDL";
int n=20, m, om, act_score;
double score, bscore;
char ans[N][N];
string s[1024];
int d[1024][1024], maxl;
char secret[N][N];
string p[1024];
int sz[1024];
int a[2*N][1024], cnt[2*N], l, wx[1024], wy[1024], len[1024];
int d1(int x, int y) { return abs(x-y); }
int d1(pii p1, pii p2) { return d1(p1.first,p2.first)+d1(p1.second,p2.second); }
pii Move(pii p, int k) { return pii(p.first + DX[k], p.second + DY[k]); }
bool In(pii p) { return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n; }
void ReversePath(vector<int>& p) { reverse(p.begin(),p.end()); for(int&x:p) x^=2; }
vector<pii> bnext[1024], bprev[1024];
vector<string> ps;
int cnt2[8][8], cnt3[8][8][8], cnt4[8][8][8][8];
int sqr(int x) { return x*x; }
void Prepare() {
    bscore=-1;
    F0(i, LOGN) logs[i] = -log((i+0.5)/LOGN);
}

int addr(int x, int y) {
    return sz[y]-d[x][y];
}

double coeff;
double lenp(int x) {
    int lim=n+maxl-2;
    double ret=0.0;
    if(x>lim) ret+= (x-lim)*1.5;
    ret+=x*x*0.01;
    return ret;
}

int used[1024];
vector<pair<string,int>> g;

string findopt(int len) {
    int best=-1;
    string ret="";
    vector<int> bused;
    F0(i,2*n) if(cnt[i]>0) {
        for (int st=0; st <= cnt[i]; st++) {
            string build;
            int val=0;
            int pr=-1;
            vector<int> cused;
            for(int j1=st; j1<cnt[i]; j1++) {
                int k=a[i][j1];
                if(pr==-1) build=p[k]; else build += p[k].substr(d[pr][k]);
                pr=k;
                if(!used[k]) { val += g[k].second; cused.push_back(k); }
                if(val>best) {
                    if(SZ(build) > len) {
                        if(len!=n) continue;
                        int bad=0;
                        for(int x=n; x<SZ(build); x++) if(build[x%n]!=build[x]) { bad=1; break; }
                        if(bad) continue;
                    }
                    best=val;
                    bused=cused;
                    ret=build;
                    if(SZ(ret)>len) ret.resize(len);
                }
            }
        }
    }
    for(int k : bused) used[k]=1;
    return ret;
}

int stat[8][8][8], stat4[8][8][8][8];
int b[64][64];
bool taken[64];
vector<int> inds[128][8];
int oi[64], oj[64];
int rowi[64];
int t[64][64];
bool solved;

void go2(int at) {
    //PR(at);
    if(at==n) {
        solved=true;
        return;
    }
    F0(i,l) if (!taken[i]) {
        F0(j,n) {
            int bad=0;
            F0(x,n) if (t[x][at] != b[i][j+x]) { bad=1; break; }

            if(!bad) {
                taken[i]=1;
                go2(at+1);
                if(solved) return;
                taken[i]=0;
            }
        }
    }
}

void go(int at) {
    if(at==n) {
        go2(4);
        return;
    }
    F0(i,l) if (!taken[i]) {
        F0(j,n) if(b[i][j]==b[oi[1]][oj[1]+at] && b[i][j+1]==b[oi[2]][oj[2]+at] && b[i][j+2]==b[oi[3]][oj[3]+at] && b[i][j+3]==b[oi[4]][oj[4]+at]) {
            taken[i]=1;

            F0(j2,n) t[at][j2]=b[i][j+j2];

            go(at+1);
            if(solved) return;

            taken[i]=0;
        }
    }
}

void SolvePerfect() {
    F0(i,l) F0(j,2*n) b[i][j]=ps[i][j%n]-'A';

    taken[0] = true;
    go(0);

    F0(i,l) F0(j,n) {
        inds[i][b[i][j]].push_back(j);
        stat[b[i][j]][b[i][j+1]][b[i][j+2]]++;
        stat4[b[i][j]][b[i][j+1]][b[i][j+2]][b[i][j+3]]++;
    }
    F0(j,n) t[0][j]=b[0][j];

    F1(i1,l-1) for (int j1 : inds[i1][b[0][0]]) {
        taken[i1]=1;
        F1(i2,l-1) if(i2!=i1) for (int j2:inds[i2][b[0][1]]) {
            taken[i2]=1;
            F1(i3,l-1) if(!taken[i3]) for(int j3:inds[i3][b[0][2]]) {
                int bad=0;
                F0(i,n) if(!stat[b[i1][j1+i]][b[i2][j2+i]][b[i3][j3+i]]) {
                    bad=1;
                    break;
                }
                if(bad) continue;
                taken[i3]=1;
                F1(i4,l-1) if(!taken[i4]) for (int j4:inds[i4][b[0][3]]) {
                    int bad=0;
                    F0(i,n) if(!stat4[b[i1][j1+i]][b[i2][j2+i]][b[i3][j3+i]][b[i4][j4+i]]) {
                        bad=1;
                        break;
                    }
                    if(bad) continue;
                    taken[i4]=1;

                    oi[1]=i1; oi[2]=i2; oi[3]=i3; oi[4]=i4;
                    oj[1]=j1; oj[2]=j2; oj[3]=j3; oj[4]=j4;
                    go(1);

                    if(solved) return;

                    taken[i4]=0;
                }
                taken[i3]=0;
            }
            taken[i2]=0;
        }
        taken[i1]=0;
    }
}

void Solve() {
    Init();

    om=m;
    Prepare();
    my.reseed(2020);

    F0(i,n)F0(j,n) ans[i][j]='.';

    g.clear();

    PR(m);
    sort(s,s+m,[](string a, string b) {return SZ(a)>SZ(b);});
    F0(i,m) {
        string p=s[i];
        bool ins=false;
        F0(i,SZ(g)) if(g[i].first.find(p)!=-1) {
            //PR(p);
            g[i].second++;
            ins=true;
            break;
        }
        if(!ins) g.push_back(make_pair(p,1));
    }
    m=SZ(g);
    PR(m);
    F0(i,m) {
        p[i]=g[i].first;
        sz[i]=SZ(p[i]);
        maxl=max(maxl, sz[i]);
        F0(j,sz[i]) {
            if(j+1<sz[i]) cnt2[p[i][j]-'A'][p[i][j+1]-'A']++;
            if(j+2<sz[i]) cnt3[p[i][j]-'A'][p[i][j+1]-'A'][p[i][j+2]-'A']++;
        }
    }

    F0(i,8) F0(j,8) cerr << cnt2[i][j]<<" "; cerr<<endl;
    F0(i,8) F0(j,8) F0(k,8) cerr << cnt3[i][j][k]<<" "; cerr<<endl;

    F0(i,m) F0(j,m) if(i!=j) {
        string p1=p[i], p2=p[j];
        for (int x=SZ(p1)-1; x>=0; x--) if(p1.substr(SZ(p1)-x)==p2.substr(0,x)) {
            d[i][j]=x;
            if(x>0) {
                bnext[i].push_back(pii(x,j));
                bprev[j].push_back(pii(x,i));
            }
            break;
        }
    }

    F0(i,m) {
        sort(bnext[i].rbegin(), bnext[i].rend());
        sort(bprev[i].rbegin(), bprev[i].rend());
    }

    l=2*n;
    F0(i,l) {
        cnt[i]=0;
        len[i]=0;
    }
    F0(i,m) {
        int x=i%l;
        wx[i]=x;
        wy[i]=cnt[x];
        if(cnt[x]==0) {
            len[x]+=sz[i];
        } else len[x]+=addr(a[x][cnt[x]-1], i);
        a[x][cnt[x]++]=i;
    }

    int iter=10000000;
    int tot=1,succ=0;
    F0(its,iter) {
        double maxd = 1.5 * (iter-its) / iter * logs[my.nextInt(LOGN)];

        int k=my.nextInt(m);
        int i=wx[k], j=wy[k], i2 = -1, j2 = -1;

        if(my.nextInt(4)) {
            i2=my.nextInt(l);
            j2=my.nextInt(cnt[i2]+1);
        } else {
            int side=my.nextInt(2);
            int targ=my.nextInt(3,4);
            int CH=10000;
            if(side) {
                int sz=min(SZ(bprev[k]), CH);
                while(sz>1 && bprev[k][sz-1].first<=targ) sz--;
                int k2=bprev[k][my.nextInt(sz)].second;
                i2=wx[k2];
                j2=wy[k2]+1;
            } else {
                int sz=min(SZ(bnext[k]), CH);
                while(sz>1 && bnext[k][sz-1].first<=targ) sz--;
                int k2=bnext[k][my.nextInt(sz)].second;
                i2=wx[k2];
                j2=wy[k2];
            }
        }

        if(i==i2) {
            continue;
        }

        int delta1=0, delta2=0;
        if(j>0 && j+1<cnt[i]) {
            delta1-=addr(a[i][j-1],k);
            delta1-=addr(k,a[i][j+1]);
            delta1+=addr(a[i][j-1],a[i][j+1]);
        }
        else if(j>0) delta1-=addr(a[i][j-1],k);
        else if(j+1<cnt[i]) {
            delta1-=sz[k]+addr(k,a[i][j+1]);
            delta1+=sz[a[i][j+1]];
        } else delta1-=sz[k];

        if(j2>0 && j2<cnt[i2]) {
            delta2-=addr(a[i2][j2-1],a[i2][j2]);
            delta2+=addr(a[i2][j2-1], k);
            delta2+=addr(k,a[i2][j2]);
        }
        else if(j2>0) delta2+=addr(a[i2][j2-1], k);
        else if(j2<cnt[i2]) {
            delta2-=sz[a[i2][j2]];
            delta2+=sz[k]+addr(k,a[i2][j2]);
        } else delta2+=sz[k];

        double ev=delta1+delta2;
        ev-=lenp(len[i])+lenp(len[i2]);
        ev+=lenp(len[i]+delta1)+lenp(len[i2]+delta2);
        if(ev>0) tot++;
        if(ev<=0 || ev<=maxd) {
            if(ev>0) succ++;
            for(int jj=j;jj<cnt[i]-1;jj++) {
                a[i][jj]=a[i][jj+1];
                wy[a[i][jj]]=jj;
            }
            cnt[i]--;
            for(int jj=cnt[i2]; jj>j2;jj--) {
                a[i2][jj]=a[i2][jj-1];
                wy[a[i2][jj]]=jj;
            }
            a[i2][j2]=k;
            cnt[i2]++;
            wx[k]=i2;
            wy[k]=j2;
            len[i]+=delta1;
            len[i2]+=delta2;
        }
    }
    cerr<<succ<<"/"<<tot-1<<" " << 100.0*succ/tot<<endl;

    vector<string> hors;
    F0(i,n) {
        string p=findopt(n);
        if(p=="") break;
        hors.push_back(p);
    }
    int i=0;
    sort(hors.begin(), hors.end(), [](string s1, string s2) { return SZ(s1)<SZ(s2);});
    for(string s:hors) {
        F0(j,SZ(s)) ans[i][j]=s[j];
        i++;
    }

    int perfect=1;
    ps.clear();
    F0(i,l)  {
        string b=p[a[i][0]];
        F1(j,cnt[i]-1) b += p[a[i][j]].substr(d[a[i][j-1]][a[i][j]]);
        F0(x, SZ(b)) {
            if(x%n==0) cerr<<" ";
            cerr << b[x];
        }
        cerr<<endl;

        if (len[i] < n) {
            PR(len[i]);
            perfect=0;
            break;
        }
        F0(q,SZ(b)) if(b[q]!=b[q%n]) {
            cerr <<b[q] << " " << b[q%n] << endl;
            perfect=0;
            break;
        }
        if(!perfect) break;
        ps.push_back(b.substr(0, n));
    }
    PR(perfect);

    if(perfect) {
        SolvePerfect();
        if(solved) {
            F0(i,n) F0(j,n) ans[i][j]=t[i][j]+'A';
        }
    } else {
        for(int j=n-1; j>=0; j--) {
            int len=0;
            while(len<n && ans[len][j]=='.') len++;
            string q = findopt(len);
            if(q!="") {
                F0(i,SZ(q)) ans[i][j]=q[i];
            }
        }
        F0(i,n) F0(j,n) if(ans[i][j]=='.') ans[i][j]=my.nextInt(8)+'A';
    }

    //if(Elapsed() > 1.5) throw;

    if(STANDARD) {
        F0(i,n) {
            puts(ans[i]);
        }
    }
}

void ReadInput() {
    cin>>n>>m;
    F0(i,m) {
        cin>>s[i];
    }
}

void Eval() {
    int act_score=0, f=0;
    F0(i,n)F0(j,n)if(ans[i][j]=='.') f++;
    int matches=0;
    F0(k,om) {
        int matched=0;
        F0(i,n) F0(j,n) {
            int fail0=0, fail1=0;
            F0(l,SZ(s[k])) if(s[k][l] != ans[i][(j+l)%n]) { fail0=1; break;}
            F0(l,SZ(s[k])) if(s[k][l] != ans[(i+l)%n][j]) { fail1=1; break;}
            if(!fail0 || !fail1) {
                matched=1;
                i=n;
                j=n;
                break;
            }
        }
        if(matched) matches++;
    }
    double sc=matches==om ? 2.0*n*n/(2*n*n-f) : 1.0*matches/om;
    act_score = 1e8*sc+0.5;
    cout << matches << "/" << om << " " << sc << " " << act_score << endl;
}

int main(int argc, char* argv[]) {
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
        if(seed>=0 && seed<100) {
            char inp[128];
            sprintf(inp, "in/%04d.txt", seed);
            ignore = freopen(inp, "r", stdin);
            ReadInput();
        } else {
            // Generate
            Rand rnd(seed);
            F0(i, n) F0(j, n) secret[i][j]=rnd.nextInt(8)+'A';
            int L=rnd.nextInt(4,10);
            m=rnd.nextInt(400,800);
            F0(k,m) {
                int i=rnd.nextInt(n), j=rnd.nextInt(n);
                int l=rnd.nextInt(L-2,L+2);
                int d=rnd.nextInt(2);
                s[k]="";
                F0(len,l) {
                    s[k]+=secret[i][j];
                    if(d==0)j=(j+1)%n; else i=(i+1)%n;
                }
            }

            act_score=0;
        }
        Solve();
        Eval();
    }

    return 0;
}
