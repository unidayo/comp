#pragma GCC optimize("O3")
#include <bits/stdc++.h>
// #include <ext/pb_ds/assoc_container.hpp>
// using namespace __gnu_pbds;
// gp_hash_table<int, int> mapka;

using namespace std;
#define PB push_back
#define MP make_pair
#define LL long long
#define int LL
#define FOR(i,a,b) for(int i = (a); i <= (b); i++)
#define RE(i,n) FOR(i,1,n)
#define REP(i,n) FOR(i,0,(int)(n)-1)
#define R(i,n) REP(i,n)
#define VI vector<int>
#define PII pair<int,int>
#define LD /*long*/ double
#define FI first
#define SE second
#define st FI
#define nd SE
#define ALL(x) (x).begin(), (x).end()
#define SZ(x) ((int)(x).size())

template<class C> void mini(C &a4, C b4) { a4 = min(a4, b4); }
template<class C> void maxi(C &a4, C b4) { a4 = max(a4, b4); }

template<class T> ostream &operator<<(ostream&, vector<T>);
template<class L, class R> ostream &operator<<(ostream &os, pair<L,R> P) {
  return os << "(" << P.st << "," << P.nd << ")";
}
template<class T> ostream &operator<<(ostream& os, vector<T> V) {
  os << "["; for (auto vv : V) os << vv << ","; return os << "]";
}

template<class TH> void _dbg(const char *sdbg, TH h){ cerr<<sdbg<<'='<<h<<endl; }
template<class TH, class... TA> void _dbg(const char *sdbg, TH h, TA... a) {
  while(*sdbg!=',')cerr<<*sdbg++;
  cerr<<'='<<h<<','; _dbg(sdbg+1, a...);
}


#ifdef LOCAL
#define debug(...) _dbg(#__VA_ARGS__, __VA_ARGS__)
#else
#define debug(...) (__VA_ARGS__)
#define cerr if(0)cout
#endif

int dx[] = {0,0,1,-1};
int dy[] = {1,-1,0,0};
vector<vector<int>> torig;
int n, m;


mt19937 rng(57);
int losuj(int a,int b){
  assert(a <= b);
  return uniform_int_distribution<int>(a, b)(rng);
}
template<class T> void obroc(vector<vector<T>>& x){
  vector<vector<T>> pom(n,vector<T>(n));
  R(i,n)R(j,n){
    pom[i][j] = x[j][n - i - 1];
  }
  swap(x, pom);
}
const LD eps = 1e-7;
LD dokoncz(vector<PII>& p){
  // assert(SZ(p) == m);
  vector<vector<int>> t = torig;
  vector<vector<LD>> pstwa(n,vector<LD>(n));
  R(i,n)R(j,n)if(t[i][j] == 0){
    pstwa[i][j] = 1./(n * n - m);
  }
  LD res = 0;
  LD SUM = 0;
  R(kt,n * n - m){
//     LD SUMKA = 0;
//     R(i,n){
//       R(j,n){
//         SUMKA += pstwa[i][j];
//       }
//     }
//     debug(SUM + SUMKA);
//   
    vector<vector<LD>> pom(n,vector<LD>(n));
    vector<vector<LD>> posrednie(n, vector<LD>(n));
    R(_,4){
      R(i,n){
        LD ak = 0;
        R(j,n){
          ak += pstwa[i][j] / 4;
          if(t[i][j] == 0){
            posrednie[i][j] += ak;
          }
          if(j && t[i][j] == 1){
            pom[i][j-1] += ak;
            ak = 0;
          }
        }
        pom[i][n - 1] += ak;
      }
      obroc(t);
      obroc(pom);
      obroc(pstwa);
      obroc(posrednie);
    }
    swap(pom, pstwa);
    // cerr << "\n\n";
    // R(i,n){
    //   R(j,n){
    //     cerr << t[i][j] << "(" << pom[i][j] << ")" << " ";
    //     // if(t[i][j])assert(pom[i][j] < eps);
    //   }
    //   cerr << "\n";
    // }
    // debug(t,pstwa);
    if(kt == SZ(p)){
      LD naj = 1e9;
      vector<PII> kan;
      R(i,n)R(j,n)if(t[i][j] == 0){
        LD wyn = pstwa[i][j] * 1000000 + posrednie[i][j];
        if(wyn > naj + eps)continue;
        if(wyn < naj - eps)kan.clear();
        kan.PB({i,j});
        naj = min(wyn,naj);
      }
      assert(!kan.empty());
      int los = losuj(0, SZ(kan) - 1);
      p.PB(kan[los]);
      debug(naj,p.back());
    }
    // debug(p.back());
    res += kt * pstwa[p[kt].FI][p[kt].SE];
    SUM += pstwa[p[kt].FI][p[kt].SE];
    pstwa[p[kt].FI][p[kt].SE] = 0;
    t[p[kt].FI][p[kt].SE] = 1;
  }
  debug(res, SUM);
  assert(abs(SUM - 1) < 1e-6);
  return res;
};
vector<vector<int>> t;
vector<vector<int>> used;
int deg(int i, int j){
  int res = 0;
  R(k,4){
    int ni = i + dx[k];
    int nj = j + dy[k];
    if(ni < 0 || ni >= n || nj < 0 || nj >= n || t[ni][nj] == 1)
      res ++;
  }
  return res;
}
bool mozna(int i,int j){
  return i < 0 || i >= n || j < 0 || j >= n || t[i][j] == 1 || (deg(i,j) == 0 && used[i][j] == 0);
}
bool mozna2(int i,int j){
  return i < 0 || i >= n || j < 0 || j >= n || t[i][j] == 1 || (deg(i,j) <= 1 && used[i][j] == 0);
}
bool puste(int i,int j){
  return i >= 0 && i < n && j >= 0 && j < n && t[i][j] == 0;
}
void zajmij(vector<PII> &p, int i,int j){
  debug("zajmij",i,j);
  if(puste(i,j)){
    p.PB({i,j});
    t[i][j] = 1;
  }
}

int32_t main() {
  ios_base::sync_with_stdio(0);
  cin.tie(0);
  cout << fixed << setprecision(11);
  cerr << fixed << setprecision(6);
  cin >> n >> m;
  t.resize(n,vector<int>(n));
  int ilee = 0;
  R(i,n){
    string pom;
    cin >> pom;
    R(j,n)if(pom[j] == '#'){
      t[i][j] = 1;
      ilee++;
    }
  }
  torig = t;
  assert(m == ilee);
  vector<PII> najp;
  LD najres = -1;
    
  R(_,25){
    t = torig;
    used.clear();
    used.resize(n,vector<int>(n));
    vector<PII> p;
    while(1){
 
      vector<pair<PII,PII>> can;
      
      R(i,n)R(j,n)if(t[i][j] == 0 && used[i][j] == 0 && !puste(i,j-1)){
        if(mozna(i-1,j) && mozna(i+1,j) && puste(i,j + 1)){
          int len = 1;
          while(puste(i, j + len))len++;
          if(mozna(i-1, j + len -1) && mozna(i+1, j + len - 1)){
            can.PB({{i,j},{len,0ll}});
            /*
            if(len > naj){
              naj = len;
              kti = i;
              ktj = j;
            }*/
          }
        }
      }
      
      R(i,n)R(j,n)if(t[i][j] == 0 && used[i][j] == 0 && !puste(i-1,j)){
        if(mozna(i,j-1) && mozna(i,j+1) && puste(i+1,j)){
          int len = 1;
          while(puste(i + len, j))len++;
          if(mozna(i + len -1, j-1) && mozna(i + len -1, j + 1)){
            can.PB({{i, j}, {len, 1ll}});
          }
        }
      }
      
      int naj = 0;
      int kti = -1;
      int ktj = -1;
      int pion = 0;
      if(!can.empty()){
        int los = losuj(0, SZ(can) - 1);
        kti = can[los].FI.FI;
        ktj = can[los].FI.SE;
        naj = can[los].SE.FI;
        pion = can[los].SE.SE;
      }else{
        R(i,n)R(j,n)if(t[i][j] == 0 && used[i][j] == 0 && !puste(i,j-1)){
          if(mozna2(i-1,j) && mozna2(i+1,j) && puste(i,j + 1)){
            int len = 1;
            while(puste(i, j + len))len++;
            if(mozna2(i-1, j + len -1) && mozna2(i+1, j + len - 1)){
              if(len > naj){
                naj = len;
                kti = i;
                ktj = j;
              }
            }
          }
        }
        if(kti == -1)break;
      }
      
      debug(naj,kti,ktj);
      
      
      
      vector<pair<PII,int>> piony;
      vector<pair<PII,int>> poziomy;
      function<void(int,int)> dodajpoziom = [&](int ii,int jj){
        while(jj && t[ii][jj-1] == 0){
          jj--;
        }
        int len = 1;
        while(puste(ii, jj + len)){
          len++;
        }
        
        R(i,len){
          assert(t[ii][jj + i] == 0);
          used[ii][jj + i] |= 1;
        }
        poziomy.PB({{ii, jj},len});
      };
      
      function<void(int,int)> dodajpion = [&](int ii,int jj){
        while(ii && t[ii-1][jj] == 0){
          ii--;
        }
        int len = 1;
        while(puste(ii + len, jj)){
          len++;
        }
        
        R(i,len){
          assert(t[ii + i][jj] == 0);
          used[ii + i][jj] |= 2;
        }
        piony.PB({{ii, jj},len});
      };
      if(pion){
        zajmij(p, kti,           ktj-1);
        zajmij(p, kti,           ktj+1);
        zajmij(p, kti + naj - 1, ktj-1);
        zajmij(p, kti + naj - 1, ktj+1);
        dodajpion(kti, ktj);
      }else{
        zajmij(p, kti-1, ktj);
        zajmij(p, kti+1, ktj);
        zajmij(p, kti-1, ktj + naj - 1);
        zajmij(p, kti+1, ktj + naj - 1);
        dodajpoziom(kti, ktj);
      }
      while(SZ(poziomy) || SZ(piony)){
        if(SZ(poziomy)){
          assert(piony.empty());
          shuffle(ALL(poziomy), rng);
          debug(poziomy,p);
          // assert(SZ(poziomy) < 10);
          for(auto el:poziomy){
            int i = el.FI.FI;
            int j = el.FI.SE;
            int len = el.SE;
            R(l, len){
              int jj = j + l;
              int ile = 0;
              if(puste(i - 1, jj))ile++;
              if(puste(i + 1, jj))ile++;
              if(ile == 1){
                if((used[i][jj] & 2) == 0){
                  dodajpion(i,jj);
                }
              }else if(ile == 2){
                bool a = mozna(i-1,jj);
                bool b = mozna(i + 1, jj);
                int los = losuj(0,1);
                if(a && (!b || los)){
                  zajmij(p,i-1,jj);
                  dodajpion(i,jj);
                }else{
                  if(b){
                    zajmij(p, i+1, jj);
                    dodajpion(i,jj);
                  }
                }
              }
            }
          }
          poziomy.clear();
        }else{
          assert(poziomy.empty());
          shuffle(ALL(piony), rng);
          debug(piony,p);
          for(auto el:piony){
            int i = el.FI.FI;
            int j = el.FI.SE;
            debug("pion", i, j);
            int len = el.SE;
            R(l, len){
              int ii = i + l;
              int ile = 0;
              if(puste(ii, j-1))ile++;
              if(puste(ii, j+1))ile++;
              debug(ii,j, ile);
              if(ile == 1){
                if((used[ii][j] & 1) == 0){
                  dodajpoziom(ii,j);
                }
              }else if(ile == 2){
                bool a = mozna(ii,j-1);
                bool b = mozna(ii,j+1);
                int los = losuj(0,1);
                if(a && (!b || los)){
                  zajmij(p,ii,j-1);
                  dodajpoziom(ii,j);
                }else{
                  if(b){
                    zajmij(p, ii, j + 1);
                    dodajpoziom(ii, j);
                  }
                }
              }
            }
          }
          piony.clear();
        }
      }
    }
    // reverse(ALL(q));
    // for(PII el:q){
    //   p.PB(el);
    // }
    LD ak = dokoncz(p);
    // cout << ak << endl;
    if(ak > najres){
      najres = ak;
      najp = p;
    }
  }
  debug(najres / (n * n - m - 1));
#ifdef TEST
  cout << najres / (n * n - m - 1) << "\n";
#else
  R(i,SZ(najp)){
    cout << najp[i].FI << " " << najp[i].SE << "\n";
  }
#endif
}
