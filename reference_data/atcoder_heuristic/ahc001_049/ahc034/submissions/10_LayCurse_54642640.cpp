#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("inline")
#include<bits/stdc++.h>
#include<sys/time.h>
using namespace std;
template<class T> struct cLtraits_identity{
  using type = T;
}
;
template<class T> using cLtraits_try_make_signed =
  typename conditional<
    is_integral<T>::value,
    make_signed<T>,
    cLtraits_identity<T>
    >::type;
template <class S, class T> struct cLtraits_common_type{
  using tS = typename cLtraits_try_make_signed<S>::type;
  using tT = typename cLtraits_try_make_signed<T>::type;
  using type = typename common_type<tS,tT>::type;
}
;
template<class S, class T> inline auto min_L(S a, T b)
-> typename cLtraits_common_type<S,T>::type{
  return (typename cLtraits_common_type<S,T>::type) a <= (typename cLtraits_common_type<S,T>::type) b ? a : b;
}
template<class S, class T> inline S chmin(S &a, T b){
  if(a>b){
    a=b;
  }
  return a;
}
template<class S, class T> inline S chmax(S &a, T b){
  if(a<b){
    a=b;
  }
  return a;
}
struct Timer{
  double x;
  double gettimeofday_sec(void){
    timeval t;
    gettimeofday(&t, 0);
    return t.tv_sec + t.tv_usec * 1e-6;
  }
  void set(void){
    x = gettimeofday_sec();
  }
  double get(void){
    return gettimeofday_sec() - x;
  }
}
;
struct Rand{
  unsigned x;
  unsigned y;
  unsigned z;
  unsigned w;
  Rand(void){
    x=123456789;
    y=362436069;
    z=521288629;
    w=(unsigned)time(NULL);
  }
  Rand(unsigned seed){
    x=123456789;
    y=362436069;
    z=521288629;
    w=seed;
  }
  inline unsigned get(void){
    unsigned t;
    t = (x^(x<<11));
    x=y;
    y=z;
    z=w;
    w = (w^(w>>19))^(t^(t>>8));
    return w;
  }
  inline double getUni(void){
    return get()/4294967296.0;
  }
  inline int get(int a){
    return (int)(a*getUni());
  }
  inline int get(int a, int b){
    return a+(int)((b-a+1)*getUni());
  }
  inline long long get(long long a){
    return(long long)(a*getUni());
  }
  inline long long get(long long a, long long b){
    return a+(long long)((b-a+1)*getUni());
  }
  inline double get(double a, double b){
    return a+(b-a)*getUni();
  }
  inline int getExp(int a){
    return(int)(exp(getUni()*log(a+1.0))-1.0);
  }
  inline int getExp(int a, int b){
    return a+(int)(exp(getUni()*log((b-a+1)+1.0))-1.0);
  }
}
;
inline int my_getchar_unlocked(){
  static char buf[1048576];
  static int s = 1048576;
  static int e = 1048576;
  if(s == e && e == 1048576){
    e = fread_unlocked(buf, 1, 1048576, stdin);
    s = 0;
  }
  if(s == e){
    return EOF;
  }
  return buf[s++];
}
inline void rd(int &x){
  int k;
  int m=0;
  x=0;
  for(;;){
    k = my_getchar_unlocked();
    if(k=='-'){
      m=1;
      break;
    }
    if('0'<=k&&k<='9'){
      x=k-'0';
      break;
    }
  }
  for(;;){
    k = my_getchar_unlocked();
    if(k<'0'||k>'9'){
      break;
    }
    x=x*10+k-'0';
  }
  if(m){
    x=-x;
  }
}
struct MY_WRITER{
  char buf[1048576];
  int s;
  int e;
  MY_WRITER(){
    s = 0;
    e = 1048576;
  }
  ~MY_WRITER(){
    if(s){
      fwrite_unlocked(buf, 1, s, stdout);
    }
  }
}
;
MY_WRITER MY_WRITER_VAR;
void my_putchar_unlocked(int a){
  if(MY_WRITER_VAR.s == MY_WRITER_VAR.e){
    fwrite_unlocked(MY_WRITER_VAR.buf, 1, MY_WRITER_VAR.s, stdout);
    MY_WRITER_VAR.s = 0;
  }
  MY_WRITER_VAR.buf[MY_WRITER_VAR.s++] = a;
}
template<class T> inline void wt_L(const vector<T> &x);
template<class T> inline void wt_L(const set<T> &x);
template<class T> inline void wt_L(const multiset<T> &x);
template<class T1, class T2> inline void wt_L(const pair<T1,T2> x);
inline void wt_L(const char a){
  my_putchar_unlocked(a);
}
inline void wt_L(const char c[]){
  int i=0;
  for(i=0;c[i]!='\0';i++){
    my_putchar_unlocked(c[i]);
  }
}
inline void wt_L(const string &x){
  int i=0;
  for(i=0;x[i]!='\0';i++){
    my_putchar_unlocked(x[i]);
  }
}
const int LOCAL = 0;
const int TEST_CASE = 50;
const double TL = 1.95;
Timer timer;
Rand rnd;
int N;
int H[20][20];
int base;
int transpose;
int ress;
string res_str[1000000];
int res_cost;
int res_go[20];
int res_bk[20];
int tmps;
string tmp_str[1000000];
int a[20][20];
int mn[20];
int mx[20];
int pmn[20];
int pmx[20];
int ct[20];
int pct[20];
void query2(const int i, const int j, int &has, int &cost){
  if(a[i][j] > 0){
    int k;
    k = a[i][j];
    has += k;
    a[i][j] -= k;
    cost += k;
    if(a[i][j] == 0){
      ct[j]--;
      if(ct[j]==0){
        mx[j] = -1073709056;
        mn[j] = 1073709056;
      }
      else{
        while(a[mn[j]][j] == 0){
          mn[j]++;
        }
        while(a[mx[j]][j] == 0){
          mx[j]--;
        }
      }
      pct[j]--;
      if(pct[j]==0){
        pmx[j] = -1073709056;
        pmn[j] = 1073709056;
      }
      else{
        while(a[pmn[j]][j] == 0){
          pmn[j]++;
        }
        while(a[pmx[j]][j] == 0){
          pmx[j]--;
        }
      }
    }
    tmp_str[tmps++] = "+" + to_string(k);
  }
  else if(a[i][j] < 0){
    int k;
    k =min_L(-a[i][j], has);
    if(k > 0){
      a[i][j] += k;
      has -= k;
      cost += k;
      tmp_str[tmps++] = "-" + to_string(k);
    }
    if(a[i][j] == 0){
      ct[j]--;
      if(ct[j]==0){
        mx[j] = -1073709056;
        mn[j] = 1073709056;
      }
      else{
        while(a[mn[j]][j] == 0){
          mn[j]++;
        }
        while(a[mx[j]][j] == 0){
          mx[j]--;
        }
      }
    }
  }
}
void move_ip2(int &i, int &j, int &has, int &cost){
  i++;
  if(transpose==0){
    tmp_str[tmps++] ="D";
  }
  else{
    tmp_str[tmps++] ="R";
  }
  cost += 100 + has;
  query2(i,j,has,cost);
}
void move_im2(int &i, int &j, int &has, int &cost){
  i--;
  if(transpose==0){
    tmp_str[tmps++] ="U";
  }
  else{
    tmp_str[tmps++] ="L";
  }
  cost += 100 + has;
  query2(i,j,has,cost);
}
void move_jp2(int &i, int &j, int &has, int &cost){
  j++;
  if(transpose==0){
    tmp_str[tmps++] ="R";
  }
  else{
    tmp_str[tmps++] ="D";
  }
  cost += 100 + has;
  query2(i,j,has,cost);
}
void move_jm2(int &i, int &j, int &has, int &cost){
  j--;
  if(transpose==0){
    tmp_str[tmps++] ="L";
  }
  else{
    tmp_str[tmps++] ="U";
  }
  cost += 100 + has;
  query2(i,j,has,cost);
}
int score_calc2(int go[], int bk[]){
  int i;
  int j;
  int k;
  int t;
  int fmn;
  int fmx;
  int c;
  int cost = 0;
  int has = 0;
  static int us[20];
  for(i=(0);i<(N);i++){
    for(j=(0);j<(N);j++){
      a[i][j] = H[i][j];
    }
  }
  for(i=(0);i<(N);i++){
    us[i] = 0;
  }
  for(i=(0);i<(N);i++){
    mn[i] = pmn[i] = 1073709056;
    mx[i] = pmx[i] = -1073709056;
    ct[i] = pct[i] = 0;
    for(j=(0);j<(N);j++){
      if(a[j][i] != 0){
        chmin(mn[i], j);
        chmax(mx[i], j);
        ct[i]++;
      }
      if(a[j][i] > 0){
        chmin(pmn[i], j);
        chmax(pmx[i], j);
        pct[i]++;
      }
    }
  }
  i = j = 0;
  tmps = 0;
  for(t=(0);t<(N);t++){
    int m;
    for(m=(0);m<(2);m++){
      query2(i,j,has,cost);
      if(m==0){
        c =go[t];
      }
      else{
        c =bk[t];
      }
      us[c]++;
      if(ct[c] == 0){
        continue;
      }
      if(pct[c] == 0 && has == 0){
        continue;
      }
      fmn = mn[c];
      fmx = mx[c];
      if(m==0 && i > fmn){
        move_im2(i,j,has,cost);
      }
      if(m==1 && i < fmx){
        move_ip2(i,j,has,cost);
      }
      while(j < c){
        move_jp2(i,j,has,cost);
      }
      while(j > c){
        move_jm2(i,j,has,cost);
      }
      if(m==0){
        while(i+1 <= fmx && (has > 0 || i+1 <= pmx[c])){
          move_ip2(i,j,has,cost);
        }
      }
      else{
        while(i-1 >= fmn && (has > 0 || i-1 >= pmn[c])){
          move_im2(i,j,has,cost);
        }
      }
      if(us[c]==2 && ct[c]){
        return 1073709056;
      }
    }
  }
  for(i=(0);i<(N);i++){
    for(j=(0);j<(N);j++){
      if(a[i][j]){
        return 1073709056;
      }
    }
  }
  if(res_cost > cost){
    res_cost = cost;
    ress = tmps;
    for(i=(0);i<(ress);i++){
      res_str[i] = tmp_str[i];
    }
  }
  return cost;
}
void query(const int i, const int j, int &has, int &cost){
  if(a[i][j] > 0){
    int k;
    k = a[i][j];
    has += k;
    a[i][j] -= k;
    cost += k;
    if(a[i][j] == 0){
      ct[j]--;
      if(ct[j]==0){
        mx[j] = -1073709056;
        mn[j] = 1073709056;
      }
      else{
        while(a[mn[j]][j] == 0){
          mn[j]++;
        }
        while(a[mx[j]][j] == 0){
          mx[j]--;
        }
      }
      pct[j]--;
      if(pct[j]==0){
        pmx[j] = -1073709056;
        pmn[j] = 1073709056;
      }
      else{
        while(a[pmn[j]][j] == 0){
          pmn[j]++;
        }
        while(a[pmx[j]][j] == 0){
          pmx[j]--;
        }
      }
    }
  }
  else if(a[i][j] < 0){
    int k;
    k =min_L(-a[i][j], has);
    if(k > 0){
      a[i][j] += k;
      has -= k;
      cost += k;
    }
    if(a[i][j] == 0){
      ct[j]--;
      if(ct[j]==0){
        mx[j] = -1073709056;
        mn[j] = 1073709056;
      }
      else{
        while(a[mn[j]][j] == 0){
          mn[j]++;
        }
        while(a[mx[j]][j] == 0){
          mx[j]--;
        }
      }
    }
  }
}
void move_ip(int &i, int &j, int &has, int &cost){
  i++;
  cost += 100 + has;
  query(i,j,has,cost);
}
void move_im(int &i, int &j, int &has, int &cost){
  i--;
  cost += 100 + has;
  query(i,j,has,cost);
}
void move_jp(int &i, int &j, int &has, int &cost){
  j++;
  cost += 100 + has;
  query(i,j,has,cost);
}
void move_jm(int &i, int &j, int &has, int &cost){
  j--;
  cost += 100 + has;
  query(i,j,has,cost);
}
int score_calc(int go[], int bk[]){
  int i;
  int j;
  int k;
  int t;
  int fmn;
  int fmx;
  int c;
  int cost = 0;
  int has = 0;
  static int us[20];
  for(i=(0);i<(N);i++){
    for(j=(0);j<(N);j++){
      a[i][j] = H[i][j];
    }
  }
  for(i=(0);i<(N);i++){
    us[i] = 0;
  }
  for(i=(0);i<(N);i++){
    mn[i] = pmn[i] = 1073709056;
    mx[i] = pmx[i] = -1073709056;
    ct[i] = pct[i] = 0;
    for(j=(0);j<(N);j++){
      if(a[j][i] != 0){
        chmin(mn[i], j);
        chmax(mx[i], j);
        ct[i]++;
      }
      if(a[j][i] > 0){
        chmin(pmn[i], j);
        chmax(pmx[i], j);
        pct[i]++;
      }
    }
  }
  i = j = 0;
  tmps = 0;
  for(t=(0);t<(N);t++){
    int m;
    for(m=(0);m<(2);m++){
      query(i,j,has,cost);
      if(m==0){
        c =go[t];
      }
      else{
        c =bk[t];
      }
      us[c]++;
      if(ct[c] == 0){
        continue;
      }
      if(pct[c] == 0 && has == 0){
        continue;
      }
      fmn = mn[c];
      fmx = mx[c];
      if(m==0 && i > fmn){
        move_im(i,j,has,cost);
      }
      if(m==1 && i < fmx){
        move_ip(i,j,has,cost);
      }
      while(j < c){
        move_jp(i,j,has,cost);
      }
      while(j > c){
        move_jm(i,j,has,cost);
      }
      if(m==0){
        while(i+1 <= fmx && (has > 0 || i+1 <= pmx[c])){
          move_ip(i,j,has,cost);
        }
      }
      else{
        while(i-1 >= fmn && (has > 0 || i-1 >= pmn[c])){
          move_im(i,j,has,cost);
        }
      }
      if(us[c]==2 && ct[c]){
        return 1073709056;
      }
    }
  }
  for(i=(0);i<(N);i++){
    for(j=(0);j<(N);j++){
      if(a[i][j]){
        return 1073709056;
      }
    }
  }
  if(res_cost > cost){
    res_cost = cost;
    for(i=(0);i<(N);i++){
      res_go[i] = go[i];
    }
    for(i=(0);i<(N);i++){
      res_bk[i] = bk[i];
    }
  }
  return cost;
}
double solver(){
  timer.set();
  int i;
  int j;
  int k;
  int i1;
  int i2;
  int j1;
  int j2;
  double res_score;
  double t;
  double p;
  int cost;
  int n_cost;
  static int go[20];
  static int bk[20];
  static int n_go[20];
  static int n_bk[20];
  rd(N);
  {
    int BglBXU5_;
    int P9MbiVyD;
    for(BglBXU5_=(0);BglBXU5_<(N);BglBXU5_++){
      for(P9MbiVyD=(0);P9MbiVyD<(N);P9MbiVyD++){
        rd(H[BglBXU5_][P9MbiVyD]);
      }
    }
  }
  transpose = 0;
  {
    int tate = 0;
    int yoko = 0;
    int s;
    for(i=(0);i<(N);i++){
      s = 0;
      for(j=(0);j<(N);j++){
        s += H[i][j];
      }
      yoko += abs(s);
    }
    for(j=(0);j<(N);j++){
      s = 0;
      for(i=(0);i<(N);i++){
        s += H[i][j];
      }
      tate += abs(s);
    }
    if(tate > yoko){
      transpose = 1;
      for(i=(0);i<(N);i++){
        for(j=(0);j<(N);j++){
          a[i][j] = H[i][j];
        }
      }
      for(i=(0);i<(N);i++){
        for(j=(0);j<(N);j++){
          H[i][j] = a[j][i];
        }
      }
    }
  }
  base = 0;
  for(i=(0);i<(N);i++){
    for(j=(0);j<(N);j++){
      base += abs(H[i][j]);
    }
  }
  res_cost = cost = 1073709056;
  ress = 0;
  for(i=(0);i<(N);i++){
    go[i] = bk[i] = i;
  }
  while(cost == 1073709056){
    random_shuffle(go,go+N);
    random_shuffle(bk,bk+N);
    cost = score_calc(go, bk);
  }
  for(;;){
    t = (TL - timer.get()) / TL;
    if(t <= 0){
      break;
    }
    for(i=(0);i<(N);i++){
      n_go[i] = go[i];
    }
    for(i=(0);i<(N);i++){
      n_bk[i] = bk[i];
    }
    p = rnd.getUni();
    if(p<0.8){
      i = j = rnd.get(N);
      while(i==j){
        j = rnd.get(N);
      }
      if(rnd.getUni()<0.5){
        swap(n_go[i], n_go[j]);
      }
      else{
        swap(n_bk[i], n_bk[j]);
      }
    }
    else{
      i = j = rnd.get(N);
      while(i==j){
        j = rnd.get(N);
      }
      for(i1=(0);i1<(N);i1++){
        if(n_go[i1]==i){
          break;
        }
      }
      for(i2=(0);i2<(N);i2++){
        if(n_bk[i2]==i){
          break;
        }
      }
      for(j1=(0);j1<(N);j1++){
        if(n_go[j1]==j){
          break;
        }
      }
      for(j2=(0);j2<(N);j2++){
        if(n_bk[j2]==j){
          break;
        }
      }
      swap(n_go[i1], n_go[j1]);
      swap(n_bk[i1], n_bk[j1]);
    }
    n_cost = score_calc(n_go, n_bk);
    if(cost >= n_cost || rnd.getUni() < exp((cost - n_cost) / t * 0.0005)){
      for(i=(0);i<(N);i++){
        go[i] = n_go[i];
      }
      for(i=(0);i<(N);i++){
        bk[i] = n_bk[i];
      }
      cost = n_cost;
    }
  }
  res_cost = 1073709056;
  score_calc2(res_go, res_bk);
  for(i=(0);i<(ress);i++){
    wt_L(res_str[i]);
    wt_L('\n');
  }
  res_score = 1e9 * base / res_cost;
  return res_score;
}
int main(){
  if(LOCAL){
    int i;
    double score = 0;
    double s;
    for(i=(0);i<(TEST_CASE);i++){
      s = solver();
      score += s;
      fprintf(stderr, "CASE %d: %.0f: %.0f\n", i, s, score);
      wt_L("-----");
      wt_L('\n');
    }
    fprintf(stderr, "SCORE %.0f\n", score);
    fprintf(stderr, "3*SCORE %f * 1e9\n", 3*score/1e9);
  }
  else{
    double score;
    score = solver();
    fprintf(stderr, "SCORE %.0f\n", score);
  }
  return 0;
}
template<class T> inline void wt_L(const vector<T> &x){
  int fg = 0;
  for(auto a : x){
    if(fg){
      my_putchar_unlocked(' ');
    }
    fg = 1;
    wt_L(a);
  }
}
template<class T> inline void wt_L(const set<T> &x){
  int fg = 0;
  for(auto a : x){
    if(fg){
      my_putchar_unlocked(' ');
    }
    fg = 1;
    wt_L(a);
  }
}
template<class T> inline void wt_L(const multiset<T> &x){
  int fg = 0;
  for(auto a : x){
    if(fg){
      my_putchar_unlocked(' ');
    }
    fg = 1;
    wt_L(a);
  }
}
template<class T1, class T2> inline void wt_L(const pair<T1,T2> x){
  wt_L(x.first);
  my_putchar_unlocked(' ');
  wt_L(x.second);
}
// cLay version 20240420-1

// --- original code ---
// const int LOCAL = 0;
// const int TEST_CASE = 50;
// const double TL = 1.95;
// 
// Timer timer;
// Rand rnd;
// 
// int N;
// int H[20][20];
// int base;
// int transpose;
// 
// int ress; string res_str[1d6]; int res_cost; int res_go[20], res_bk[20];
// int tmps; string tmp_str[1d6];
// 
// 
// 
// 
// 
// 
// 
// 
// int a[20][20];
// int mn[20], mx[20], pmn[20], pmx[20], ct[20], pct[20];
// 
// 
// void query2(const int i, const int j, int &has, int &cost){
//   if(a[i][j] > 0){
//     int k;
//     k = a[i][j];
//     has += k;
//     a[i][j] -= k;
//     cost += k;
//     if(a[i][j] == 0){
//       ct[j]--;
//       if(ct[j]==0){
//         mx[j] = -int_inf, mn[j] = int_inf;
//       } else {
//         while(a[mn[j]][j] == 0) mn[j]++;
//         while(a[mx[j]][j] == 0) mx[j]--;
//       }
// 
//       pct[j]--;
//       if(pct[j]==0){
//         pmx[j] = -int_inf, pmn[j] = int_inf;
//       } else {
//         while(a[pmn[j]][j] == 0) pmn[j]++;
//         while(a[pmx[j]][j] == 0) pmx[j]--;
//       }
//     }
//     tmp_str[tmps++] = "+" + to_string(k);
//   } else if(a[i][j] < 0){
//     int k;
//     k = min(-a[i][j], has);
//     if(k > 0){
//       a[i][j] += k;
//       has -= k;
//       cost += k;
//       tmp_str[tmps++] = "-" + to_string(k);
//     }
//     if(a[i][j] == 0){
//       ct[j]--;
//       if(ct[j]==0){
//         mx[j] = -int_inf, mn[j] = int_inf;
//       } else {
//         while(a[mn[j]][j] == 0) mn[j]++;
//         while(a[mx[j]][j] == 0) mx[j]--;
//       }
//     }
//   }
// }
// 
// void move_ip2(int &i, int &j, int &has, int &cost){
//   i++;
//   tmp_str[tmps++] = if[transpose==0, "D", "R"];
//   cost += 100 + has;
//   query2(i,j,has,cost);
// }
// 
// void move_im2(int &i, int &j, int &has, int &cost){
//   i--;
//   tmp_str[tmps++] = if[transpose==0, "U", "L"];
//   cost += 100 + has;
//   query2(i,j,has,cost);
// }
// 
// void move_jp2(int &i, int &j, int &has, int &cost){
//   j++;
//   tmp_str[tmps++] = if[transpose==0, "R", "D"];
//   cost += 100 + has;
//   query2(i,j,has,cost);
// }
// 
// void move_jm2(int &i, int &j, int &has, int &cost){
//   j--;
//   tmp_str[tmps++] = if[transpose==0, "L", "U"];
//   cost += 100 + has;
//   query2(i,j,has,cost);
// }
// 
// 
// 
// int score_calc2(int go[], int bk[]){
//   int i, j, k, t, fmn, fmx, c;
//   int cost = 0;
//   int has = 0;
//   static int us[20];
// 
// 
//   rep(i,N) rep(j,N) a[i][j] = H[i][j];
//   rep(i,N) us[i] = 0;
// 
//   rep(i,N){
//     mn[i] = pmn[i] = int_inf;
//     mx[i] = pmx[i] = -int_inf;
//     ct[i] = pct[i] = 0;
//     rep(j,N){
//       if(a[j][i] != 0) mn[i] <?=j, mx[i] >?= j, ct[i]++;
//       if(a[j][i] > 0) pmn[i] <?= j, pmx[i] >?=j, pct[i]++;
//     }
//   }
// 
// 
//   i = j = 0;
//   tmps = 0;
//   rep(t,N) rep(m,2){
//     query2(i,j,has,cost);
// 
//     c = if[m==0, go[t], bk[t]];
//     us[c]++;
//     if(ct[c] == 0) continue;
//     if(pct[c] == 0 && has == 0) continue;
// 
//     fmn = mn[c];
//     fmx = mx[c];
// 
//     if(m==0 && i > fmn) move_im2(i,j,has,cost);
//     if(m==1 && i < fmx) move_ip2(i,j,has,cost);
// 
//     while(j < c) move_jp2(i,j,has,cost);
//     while(j > c) move_jm2(i,j,has,cost);
//     if(m==0){
//       while(i+1 <= fmx && (has > 0 || i+1 <= pmx[c])) move_ip2(i,j,has,cost);
//     } else {
//       while(i-1 >= fmn && (has > 0 || i-1 >= pmn[c])) move_im2(i,j,has,cost);
//     }
// 
//     if(us[c]==2 && ct[c]) return int_inf;
//   }
// 
//   rep(i,N) rep(j,N) if(a[i][j]) return int_inf;
// 
//   if(res_cost > cost){
//     res_cost = cost;
//     ress = tmps;
//     rep(i,ress) res_str[i] = tmp_str[i];
//   }
// 
//   return cost;
// }
// 
// 
// 
// 
// 
// void query(const int i, const int j, int &has, int &cost){
//   if(a[i][j] > 0){
//     int k;
//     k = a[i][j];
//     has += k;
//     a[i][j] -= k;
//     cost += k;
//     if(a[i][j] == 0){
//       ct[j]--;
//       if(ct[j]==0){
//         mx[j] = -int_inf, mn[j] = int_inf;
//       } else {
//         while(a[mn[j]][j] == 0) mn[j]++;
//         while(a[mx[j]][j] == 0) mx[j]--;
//       }
// 
//       pct[j]--;
//       if(pct[j]==0){
//         pmx[j] = -int_inf, pmn[j] = int_inf;
//       } else {
//         while(a[pmn[j]][j] == 0) pmn[j]++;
//         while(a[pmx[j]][j] == 0) pmx[j]--;
//       }
//     }
//     // tmp_str[tmps++] = "+" + to_string(k);
//   } else if(a[i][j] < 0){
//     int k;
//     k = min(-a[i][j], has);
//     if(k > 0){
//       a[i][j] += k;
//       has -= k;
//       cost += k;
//       // tmp_str[tmps++] = "-" + to_string(k);
//     }
//     if(a[i][j] == 0){
//       ct[j]--;
//       if(ct[j]==0){
//         mx[j] = -int_inf, mn[j] = int_inf;
//       } else {
//         while(a[mn[j]][j] == 0) mn[j]++;
//         while(a[mx[j]][j] == 0) mx[j]--;
//       }
//     }
//   }
// }
// 
// void move_ip(int &i, int &j, int &has, int &cost){
//   i++;
//   // tmp_str[tmps++] = if[transpose==0, "D", "R"];
//   cost += 100 + has;
//   query(i,j,has,cost);
// }
// 
// void move_im(int &i, int &j, int &has, int &cost){
//   i--;
//   // tmp_str[tmps++] = if[transpose==0, "U", "L"];
//   cost += 100 + has;
//   query(i,j,has,cost);
// }
// 
// void move_jp(int &i, int &j, int &has, int &cost){
//   j++;
//   // tmp_str[tmps++] = if[transpose==0, "R", "D"];
//   cost += 100 + has;
//   query(i,j,has,cost);
// }
// 
// void move_jm(int &i, int &j, int &has, int &cost){
//   j--;
//   // tmp_str[tmps++] = if[transpose==0, "L", "U"];
//   cost += 100 + has;
//   query(i,j,has,cost);
// }
// 
// 
// 
// int score_calc(int go[], int bk[]){
//   int i, j, k, t, fmn, fmx, c;
//   int cost = 0;
//   int has = 0;
//   static int us[20];
// 
// 
//   rep(i,N) rep(j,N) a[i][j] = H[i][j];
//   rep(i,N) us[i] = 0;
// 
//   rep(i,N){
//     mn[i] = pmn[i] = int_inf;
//     mx[i] = pmx[i] = -int_inf;
//     ct[i] = pct[i] = 0;
//     rep(j,N){
//       if(a[j][i] != 0) mn[i] <?=j, mx[i] >?= j, ct[i]++;
//       if(a[j][i] > 0) pmn[i] <?= j, pmx[i] >?=j, pct[i]++;
//     }
//   }
// 
// 
//   i = j = 0;
//   tmps = 0;
//   rep(t,N) rep(m,2){
//     query(i,j,has,cost);
// 
//     c = if[m==0, go[t], bk[t]];
//     us[c]++;
//     if(ct[c] == 0) continue;
//     if(pct[c] == 0 && has == 0) continue;
// 
//     fmn = mn[c];
//     fmx = mx[c];
// 
//     if(m==0 && i > fmn) move_im(i,j,has,cost);
//     if(m==1 && i < fmx) move_ip(i,j,has,cost);
// 
//     while(j < c) move_jp(i,j,has,cost);
//     while(j > c) move_jm(i,j,has,cost);
//     if(m==0){
//       while(i+1 <= fmx && (has > 0 || i+1 <= pmx[c])) move_ip(i,j,has,cost);
//     } else {
//       while(i-1 >= fmn && (has > 0 || i-1 >= pmn[c])) move_im(i,j,has,cost);
//     }
// 
//     if(us[c]==2 && ct[c]) return int_inf;
//   }
// 
//   rep(i,N) rep(j,N) if(a[i][j]) return int_inf;
// 
//   if(res_cost > cost){
//     res_cost = cost;
//     rep(i,N) res_go[i] = go[i];
//     rep(i,N) res_bk[i] = bk[i];
//   }
// 
//   return cost;
// }
// 
// 
// double solver(){
//   timer.set();
// 
//   int i, j, k, i1, i2, j1, j2;
//   double res_score, t, p;
//   int cost, n_cost;
//   static int go[20], bk[20], n_go[20], n_bk[20];
// 
//   rd(N,H(N,N));
//   transpose = 0;
// 
//   {
//     int tate = 0, yoko = 0, s;
//     rep(i,N){
//       s = 0;
//       rep(j,N) s += H[i][j];
//       yoko += abs(s);
//     }
//     rep(j,N){
//       s = 0;
//       rep(i,N) s += H[i][j];
//       tate += abs(s);
//     }
//     //fprintf(stderr, "tate %d yoko %d\n", tate, yoko);
// 
//     if(tate > yoko){
//       transpose = 1;
//       rep(i,N) rep(j,N) a[i][j] = H[i][j];
//       rep(i,N) rep(j,N) H[i][j] = a[j][i];
//     }
//   }
// 
//   base = 0;
//   rep(i,N) rep(j,N) base += abs(H[i][j]);
//   res_cost = cost = int_inf;
//   ress = 0;
// 
//   rep(i,N) go[i] = bk[i] = i;
//   while(cost == int_inf){
//     random_shuffle(go,go+N);
//     random_shuffle(bk,bk+N);
//     cost = score_calc(go, bk);
//   }
// 
//   for(;;){
//     t = (TL - timer.get()) / TL;
//     if(t <= 0) break;
// 
//     rep(i,N) n_go[i] = go[i];
//     rep(i,N) n_bk[i] = bk[i];
// 
//     p = rnd.getUni();
//     if(p<0.8){
//       i = j = rnd.get(N);
//       while(i==j) j = rnd.get(N);
//       if(rnd.getUni()<0.5) swap(n_go[i], n_go[j]);
//       else                 swap(n_bk[i], n_bk[j]);
//     }else{
//       i = j = rnd.get(N);
//       while(i==j) j = rnd.get(N);
//       rep(i1,N) if(n_go[i1]==i) break;
//       rep(i2,N) if(n_bk[i2]==i) break;
//       rep(j1,N) if(n_go[j1]==j) break;
//       rep(j2,N) if(n_bk[j2]==j) break;
//       swap(n_go[i1], n_go[j1]);
//       swap(n_bk[i1], n_bk[j1]);
//     }
// 
//     n_cost = score_calc(n_go, n_bk);
//     if(cost >= n_cost || rnd.getUni() < exp((cost - n_cost) / t * 0.0005)){
//       rep(i,N) go[i] = n_go[i];
//       rep(i,N) bk[i] = n_bk[i];
//       cost = n_cost;
//     }
//   }
//   res_cost = int_inf;
//   score_calc2(res_go, res_bk);
// 
// 
//   rep(i,ress) wt(res_str[i]);
//   res_score = 1e9 * base / res_cost;
//   return res_score;
// }
// 
// {
//   if(LOCAL){
//     int i;
//     double score = 0, s;
//     rep(i, TEST_CASE){
//       s = solver();
//       score += s;
//       fprintf(stderr, "CASE %d: %.0f: %.0f\n", i, s, score);
//       wt("-----");
//     }
//     fprintf(stderr, "SCORE %.0f\n", score);
//     fprintf(stderr, "3*SCORE %f * 1e9\n", 3*score/1e9);
//   } else {
//     double score;
//     score = solver();
//     fprintf(stderr, "SCORE %.0f\n", score);
//   }
// }
