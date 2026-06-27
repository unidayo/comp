#include <bits/stdc++.h>
#define fi first
#define se second
#define rep(i,n) for(int i = 0; i < (n); ++i)
#define rrep(i,n) for(int i = 1; i <= (n); ++i)
#define drep(i,n) for(int i = (n)-1; i >= 0; --i)
#define srep(i,s,t) for (int i = s; i < (t); ++i)
#define dsrep(i,t,s) for(int i = (t)-1; i >= (s); --i)
#define rng(a) a.begin(),a.end()
#define rrng(a) a.rbegin(),a.rend()
#define isin(x,l,r) ((l) <= (x) && (x) < (r))
#define pb push_back
#define eb emplace_back
#define sz(x) (int)(x).size()
#define pcnt __builtin_popcountll
#define snuke srand((unsigned)clock()+(unsigned)time(NULL));
#define show(x) cerr<<#x<<" = "<<x<<endl;
#define bn(x) ((1<<(x))-1)
#define newline puts("")
using namespace std;
template<typename T> using vc = vector<T>;
template<typename T> using vv = vc<vc<T>>;
template<typename T> using PQ = priority_queue<T,vc<T>,greater<T> >;
using ll = long long;
using uint = unsigned;
using ull = unsigned long long;
using P = pair<int,int>;
using T3 = tuple<int,int,int>;
using vi = vc<int>;
using vvi = vv<int>;
using vl = vc<ll>;
using vp = vc<P>;
using vt = vc<T3>;
int getInt(){int x;scanf("%d",&x);return x;}
vi pm(int n, int s=0) { vi a(n); iota(rng(a),s); return a;}
template<typename T>istream& operator>>(istream&i,vc<T>&v){rep(j,sz(v))i>>v[j];return i;}
template<typename T>string join(const vc<T>&v,const string& d=""){stringstream s;rep(i,sz(v))(i?s<<d:s)<<v[i];return s.str();}
template<typename T>ostream& operator<<(ostream&o,const vc<T>&v){if(sz(v))o<<join(v," ");return o;}
template<typename T1,typename T2>istream& operator>>(istream&i,pair<T1,T2>&v){return i>>v.fi>>v.se;}
template<typename T1,typename T2>ostream& operator<<(ostream&o,const pair<T1,T2>&v){return o<<v.fi<<","<<v.se;}
vc<string> split(const string& s,char d=' '){vc<string> r(1);for(char c:s)if(c==d)r.pb("");else r.back()+=c;return r;}
string operator*(const string& s,int t){return join(vc<string>(t,s));}
template<typename T>bool mins(T& x,const T&y){if(x>y){x=y;return true;}else return false;}
template<typename T>bool maxs(T& x,const T&y){if(x<y){x=y;return true;}else return false;}
template<typename T>T dup(T x, T y){return (x+y-1)/y;}
template<typename T>ll suma(const vc<T>&a){ll res(0);for(auto&&x:a)res+=x;return res;}
template<typename T>void uni(vc<T>& a){sort(rng(a));a.erase(unique(rng(a)),a.end());}
const double eps = 1e-10;
const ll LINF = 1001002003004005006ll;
const int INF = 1001001001;
#define dame { puts("-1"); return;}
#define yn {puts("Yes");}else{puts("No");}
const int MX = 200005;
random_device _rd;
struct xrand {
  static const uint64_t _x = 88172645463325252ull;
  uint64_t x;
  xrand(): x(_x ^ (_rd()+time(0))) {}
  xrand(uint64_t seed): x(_x ^ seed) {}
  uint64_t get() {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }
  int operator()(int n) { return get()%n;}
  int operator()(int l, int r) { return get()%(r-l+1) + l;}
} rnd;
template<typename T>
void shuffle(vc<T>& a) {
  drep(i,sz(a)) swap(a[rnd(i+1)], a[i]);
}

const int di[] = {-1,0,1,0}, dj[] = {0,-1,0,1}; //^<v>
const string ds = "ULDR";
vi ctov;

// Time
#include <sys/time.h>
ll getTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  ll result =  tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;
  return result;
}
ll start_time, time_limit = 1000;
void setTime(){ start_time = getTime();}
ll nowTime(){ return getTime() - start_time;}
bool ended;
bool timeup(){ return ended = (nowTime() > time_limit);}
//

const int n = 50;
int si, sj;
vvi tl, val;
vi used;
vi edgeScore;
vi tlsz;
vi vlist;

int score = 0;
int best = 0;
string str, ans;

int cnt = 0;

void dfs(int i, int j) {
  ++cnt;
  if (cnt%10000 == 0 && timeup()) return;

  int ti = tl[i][j];
  used[ti] = 1;
  score += val[i][j];
  if (best < score) {
    best = score;
    ans = str;
    // cerr<<best<<": "<<nowTime()<<endl;
  }

  // vp cand;
  for (int v : vlist) {
    int ni = i+di[v], nj = j+dj[v];
    if (ni<0||nj<0||ni>=n||nj>=n) continue;
    int k = tl[ni][nj];
    if (used[k]) continue;
    // int x = edgeScore[k];
    // x *= 50;
    // x += val[ni][nj];
    // cand.eb(x, v);

    str += ds[v];
    dfs(ni,nj);
    str.pop_back();
    if (ended) return;
  }

  // sort(rng(cand));
  /*
  for (auto [_, v] : cand) {
    int ni = i+di[v], nj = j+dj[v];
    str += ds[v];
    dfs(ni,nj);
    str.pop_back();
    if (ended) return;
  }
  */

  used[ti] = 0;
  score -= val[i][j];
}

const int L = 10000;
vvi vlists;
int li, step;

bool tail;
int ei, ej;
int mis;
int MIS = 1200;
int cBest;
string memo;
void efs(int i, int j) {
  if (i == ei && j == ej) {
    score += val[i][j];
    if (cBest < score) {
      cBest = score;
      memo = str;
    }
    score -= val[i][j];
    return;
  }

  li += step; li %= L;
  int ri = li;

  int ti = tl[i][j];
  used[ti] = 1;
  score += val[i][j];

  if (tail && cBest < score) {
    cBest = score;
    memo = str;
  }

  for (int v : vlists[ri]) {
    int ni = i+di[v], nj = j+dj[v];
    if (ni<0||nj<0||ni>=n||nj>=n) continue;
    int k = tl[ni][nj];
    if (used[k]) continue;

    mis++;
    str += ds[v];
    efs(ni,nj);
    str.pop_back();
    if (mis > MIS) break;
  }

  used[ti] = 0;
  score -= val[i][j];
}

struct Solver {
  void solve() {
    setTime();

    cin>>si>>sj;
    tl = val = vvi(n,vi(n));
    cin>>tl>>val;
    int m = 0;
    rep(i,n)rep(j,n) maxs(m,tl[i][j]+1);
    used = vi(m);
    edgeScore = tlsz = vi(m);
    rep(i,n)rep(j,n) {
      int x = min(i,j);
      mins(x, n-i-1);
      mins(x, n-j-1);
      int ni = tl[i][j];
      edgeScore[ni] += x;
      tlsz[ni]++;
    }
    // rep(i,m) if (tlsz[i]) edgeScore[i] *= 2;

    {
      string s;
      if (si < sj) {
        if (si < n-sj) s = "ULRD";
        else s = "RUDL";
      } else {
        if (si < n-sj) s = "LUDR";
        else s = "DLRU";
      }
      if (s[0] == 'U' || s[0] == 'D') {
        if (sj > n-sj-1) swap(s[1],s[2]);
      } else {
        if (si > n-si-1) swap(s[1],s[2]);
      }
      for (char c : s) rep(i,4) if (ds[i] == c) vlist.pb(i);
    }
    ctov = vi(256);
    rep(i,4) ctov[ds[i]] = i;

    // rrep(ti,2) {
    //   time_limit = 200+400*ti;
    //   dfs(si,sj);
    //   swap(vlist[1], vlist[2]);
    // }

    time_limit = 750;
    dfs(si,sj);


    ended = false;
    time_limit = 1980;

    used = vi(m);
    str = "";
    vp ps;
    {
      int i = si, j = sj;
      used[tl[i][j]] = 1;
      ps.eb(i,j);
      score = val[i][j];
      for (char c : ans) {
        int v = ctov[c];
        i += di[v]; j += dj[v];
        used[tl[i][j]] = 1;
        score += val[i][j];
        ps.eb(i,j);
      }
    }

    rep(i,L) {
      vlists.pb(vlist);
      shuffle(vlist);
    }

    cnt = 0;
    while (cnt%200 != 0 || !timeup()) {
      ++cnt;
      int w = rnd(47,53);
      if (rnd(12) == 0) w = 100, MIS = 3000;
      else MIS = 1000;
      // if (cnt < 10000) shuffle(vlists[0]);
      step = rnd(10);
      int pi = rnd(sz(ans)-w);
      tail = false;
      if (rnd(30) == 0) tail = true, pi = sz(ans)-w;

      auto [bi,bj] = ps[pi];
      rep(wi,w+1) {
        auto [i,j] = ps[pi+wi];
        used[tl[i][j]] = 0;
        score -= val[i][j];
      }

      ei = ps[pi+w].fi;
      ej = ps[pi+w].se;
      cBest = 0;
      memo = "";
      mis = 0;
      efs(bi,bj);

      const int X = 20000;
      int diff = best-cBest;
      if (best <= cBest || (cBest && X > diff && rnd(X/diff) > cnt)) {
        ans.erase(ans.begin()+pi, ans.begin()+(pi+w));
        ps.erase(ps.begin()+pi, ps.begin()+(pi+w+1));
        vp nps;
        int i = bi, j = bj;
        used[tl[i][j]] = 1;
        score += val[i][j];
        nps.eb(i,j);
        for (char c : memo) {
          int v = ctov[c];
          i += di[v]; j += dj[v];
          used[tl[i][j]] = 1;
          score += val[i][j];
          nps.eb(i,j);
        }
        ans.insert(ans.begin()+pi, rng(memo));
        ps.insert(ps.begin()+pi, rng(nps));
        best = score;
      } else {
        rep(wi,w+1) {
          auto [i,j] = ps[pi+wi];
          used[tl[i][j]] = 1;
          score += val[i][j];
        }
      }
    }

    cerr<<best<<endl;
    cout<<ans<<endl;
  }
};

int main(int args, char** argv) {
  int ts = 1;
  // scanf("%d",&ts);
  rrep(ti,ts) {
    Solver solver;
    solver.solve();
  }
  return 0;
}
