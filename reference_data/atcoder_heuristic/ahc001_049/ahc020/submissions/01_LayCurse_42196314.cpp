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
template<class T> using cLtraits_try_make_unsigned =
  typename conditional<
    is_integral<T>::value,
    make_unsigned<T>,
    cLtraits_identity<T>
    >::type;
template <class S, class T> struct cLtraits_common_type{
  using tS = typename cLtraits_try_make_signed<S>::type;
  using tT = typename cLtraits_try_make_signed<T>::type;
  using type = typename common_type<tS,tT>::type;
}
;
void*wmem;
char memarr[96000000];
template<class S, class T> inline auto max_L(S a, T b)
-> typename cLtraits_common_type<S,T>::type{
  return (typename cLtraits_common_type<S,T>::type) a >= (typename cLtraits_common_type<S,T>::type) b ? a : b;
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
template<class S, class T> inline S divup_L(S a, T b){
  return (a+b-1)/b;
}
template<class T> inline void walloc1d(T **arr, int x, void **mem = &wmem){
  static int skip[16] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  (*mem) = (void*)( ((char*)(*mem)) + skip[((unsigned long long)(*mem)) & 15] );
  (*arr)=(T*)(*mem);
  (*mem)=((*arr)+x);
}
template<class T> inline void walloc1d(T **arr, int x1, int x2, void **mem = &wmem){
  walloc1d(arr, x2-x1, mem);
  (*arr) -= x1;
}
inline int Ilog2_f_L(const int n){
  int res;
  if(n <= 0){
    return -1;
  }
  res = sizeof(int) * 8 - __builtin_clz(n) - 1;
  return res;
}
inline int Ilog2_f_L(const long long n){
  int res;
  if(n <= 0){
    return -1;
  }
  res = sizeof(long long) * 8 - __builtin_clzll(n) - 1;
  return res;
}
template<class T1> void sortI(int N, T1 a[], void *mem = wmem){
  sort(a, a+N);
}
template<class T1, class T2> void sortI(int N, T1 a[], T2 b[], void *mem = wmem){
  int i;
  pair<T1, T2>*arr;
  walloc1d(&arr, N, &mem);
  for(i=(0);i<(N);i++){
    arr[i].first = a[i];
    arr[i].second = b[i];
  }
  sort(arr, arr+N);
  for(i=(0);i<(N);i++){
    a[i] = arr[i].first;
    b[i] = arr[i].second;
  }
}
template<class T1, class T2, class T3> void sortI(int N, T1 a[], T2 b[], T3 c[], void *mem = wmem){
  int i;
  pair<T1, pair<T2, T3> >*arr;
  walloc1d(&arr, N, &mem);
  for(i=(0);i<(N);i++){
    arr[i].first = a[i];
    arr[i].second.first = b[i];
    arr[i].second.second = c[i];
  }
  sort(arr, arr+N);
  for(i=(0);i<(N);i++){
    a[i] = arr[i].first;
    b[i] = arr[i].second.first;
    c[i] = arr[i].second.second;
  }
}
template<class T1, class T2, class T3, class T4> void sortI(int N, T1 a[], T2 b[], T3 c[], T4 d[], void *mem = wmem){
  int i;
  pair<pair<T1, T2>, pair<T3, T4> >*arr;
  walloc1d(&arr, N, &mem);
  for(i=(0);i<(N);i++){
    arr[i].first.first = a[i];
    arr[i].first.second = b[i];
    arr[i].second.first = c[i];
    arr[i].second.second = d[i];
  }
  sort(arr, arr+N);
  for(i=(0);i<(N);i++){
    a[i] = arr[i].first.first;
    b[i] = arr[i].first.second;
    c[i] = arr[i].second.first;
    d[i] = arr[i].second.second;
  }
}
template<class T> inline int sort_helper_getbit(T A[]){
  return -1;
}
template<> inline int sort_helper_getbit(int A[]){
  return sizeof(int)*8;
}
template<> inline int sort_helper_getbit(unsigned A[]){
  return sizeof(unsigned)*8;
}
template<> inline int sort_helper_getbit(long long A[]){
  return sizeof(long long)*8;
}
template<> inline int sort_helper_getbit(unsigned long long A[]){
  return sizeof(unsigned long long)*8;
}
template<> inline int sort_helper_getbit(char A[]){
  return sizeof(char)*8;
}
template<class T> void sortA_1_int_L(int N, T A[], void *mem = wmem){
  int i;
  int j;
  int k;
  int b;
  int s;
  int ok;
  ok = 1;
  for(i=(1);i<(N);i++){
    if(A[i-1] > A[i]){
      ok = 0;
      break;
    }
  }
  if(ok){
    return;
  }
  if(N < 128){
    sort(A,A+N);
    return;
  }
  b = sort_helper_getbit(A);
  if(b==-1){
    sort(A,A+N);
    return;
  }
  T mn;
  T mx;
  mn = mx = A[0];
  for(i=(1);i<(N);i++){
    chmin(mn, A[i]);
  }
  for(i=(1);i<(N);i++){
    chmax(mx, A[i]);
  }
  ok = 1;
  if(mn < 0 && mx > 0 && (mn < -N || mx > N)){
    ok = 0;
  }
  if(ok && mx - mn > N){
    ok = 0;
  }
  if(ok){
    int*tmp;
    walloc1d(&tmp, mx-mn+1, &mem);
    for(i=(0);i<(mx-mn+1);i++){
      tmp[i] = 0;
    }
    for(i=(0);i<(N);i++){
      tmp[A[i]-mn]++;
    }
    k = 0;
    for(i=(0);i<(mx-mn+1);i++){
      while(tmp[i] > 0){
        tmp[i]--;
        A[k++] = i+mn;
      }
    }
    return;
  }
  {
    typename make_unsigned<T>::type *t[2];
    typename make_unsigned<T>::type  mask;
    typename make_unsigned<T>::type  cur;
    typename make_unsigned<T>::type  one = 1;
    T tone = 1;
    int*pos;
    int nn = 0;
    int ss;
    s =Ilog2_f_L(N);
    if(s > 8){
      s = (8 + (s-7)/2);
    }
    ss = 1;
    for(;;){
      if(ss >= b){
        break;
      }
      if( mx >= 0 && (tone << (ss-1)) < mx ){
        ss++;
        continue;
      }
      if( mn < 0 && -(tone << (ss-1)) >= mn ){
        ss++;
        continue;
      }
      break;
    }
    k =(divup_L(ss,s));
    s =(divup_L(ss,k));
    mask = 0;
    for(i=(0);i<(b);i++){
      if(i < s*k){
        mask |= one << i;
      }
    }
    t[0] = (typename make_unsigned<T>::type *) A;
    walloc1d(&t[1], N, &mem);
    walloc1d(&pos, (1<<s)+1, &mem);
    for(j=(0);j<(k);j++){
      cur = 0;
      for(i=(0);i<(b);i++){
        if(s*j <= i && i < s*(j+1) && i < b){
          cur |= one << i;
        }
      }
      for(i=(0);i<((1<<s)+1);i++){
        pos[i] = 0;
      }
      for(i=(0);i<(N);i++){
        pos[((t[nn][i]&cur)>>(s*j))+1]++;
      }
      for(i=(0);i<((1<<s));i++){
        pos[i+1] += pos[i];
      }
      for(i=(0);i<(N);i++){
        t[nn^1][pos[(t[nn][i]&cur)>>(s*j)]++] = t[nn][i];
      }
      nn ^= 1;
      mask ^= cur;
    }
    if(mn < 0 && mx >= 0){
      k = 0;
      for(i=(0);i<(N);i++){
        if(A[i] < 0){
          k++;
        }
      }
      for(i=(0);i<(k);i++){
        t[nn^1][i] = t[nn][N-k+i];
      }
      for(i=(k);i<(N);i++){
        t[nn^1][i] = t[nn][i-k];
      }
      nn ^= 1;
    }
    if(nn==1){
      for(i=(0);i<(N);i++){
        t[0][i] = t[1][i];
      }
    }
    return;
  }
  sort(A, A+N);
}
template<class T> void sortA_1_nonint_L(int N, T A[], void *mem = wmem){
  sort(A,A+N);
}
template<class T> void sortA_1_call_L(int N, T A[], void *mem = wmem){
  sortA_1_nonint_L(N, A, mem);
}
template<> void sortA_1_call_L(int N, int A[], void *mem){
  sortA_1_int_L(N, A, mem);
}
template<> void sortA_1_call_L(int N, unsigned A[], void *mem){
  sortA_1_int_L(N, A, mem);
}
template<> void sortA_1_call_L(int N, long long A[], void *mem){
  sortA_1_int_L(N, A, mem);
}
template<> void sortA_1_call_L(int N, unsigned long long A[], void *mem){
  sortA_1_int_L(N, A, mem);
}
template<> void sortA_1_call_L(int N, char A[], void *mem){
  sortA_1_int_L(N, A, mem);
}
template<class T1> void sortA(int N, T1 a[], void *mem = wmem){
  sortA_1_call_L(N, a, mem);
}
template<class T1, class T2> void sortA_2_int_L(int N, T1 A[], T2 B[], void *mem = wmem){
  int i;
  int b_a;
  int b_b;
  int s1;
  int s2;
  int so2;
  T1 mn1;
  T1 mx1;
  T2 mn2;
  T2 mx2;
  typename cLtraits_try_make_unsigned<T1>::type r1;
  typename cLtraits_try_make_unsigned<T2>::type r2;
  so2 = 1;
  for(i=(1);i<(N);i++){
    if(A[i-1] > A[i] || (A[i-1]==A[i] && B[i-1] > B[i])){
      so2 = 0;
      break;
    }
  }
  if(so2){
    return;
  }
  so2 = 1;
  for(i=(1);i<(N);i++){
    if(A[i-1] > A[i]){
      so2 = 0;
      break;
    }
  }
  if(so2==1){
    int k = 0;
    for(i=(1);i<(N);i++){
      if(A[i] != A[i-1]){
        sortA_1_call_L(i-k, B+k, mem);
        k = i;
      }
    }
    sortA_1_call_L(N-k, B+k, mem);
    return;
  }
  if(N < 128){
    sortI(N,A,B,mem);
    return;
  }
  b_a = sort_helper_getbit(A);
  b_b = sort_helper_getbit(B);
  if(b_a == -1 || b_b == -1){
    sortI(N,A,B,mem);
    return;
  }
  mn1 = mx1 = A[0];
  for(i=(1);i<(N);i++){
    chmin(mn1, A[i]);
  }
  for(i=(1);i<(N);i++){
    chmax(mx1, A[i]);
  }
  mn2 = mx2 = B[0];
  for(i=(1);i<(N);i++){
    chmin(mn2, B[i]);
  }
  for(i=(1);i<(N);i++){
    chmax(mx2, B[i]);
  }
  if(mn1 < -4611686016279904256LL || mn2 < -4611686016279904256LL || mx1 > 4611686016279904256LL || mx2 > 4611686016279904256LL || mx1-mn1 > 4611686016279904256LL || mx2-mn2 > 4611686016279904256LL){
    sortI(N,A,B,mem);
    return;
  }
  r1 = (typename cLtraits_try_make_unsigned<T1>::type)(mx1) - (typename cLtraits_try_make_unsigned<T1>::type)(mn1);
  r2 = (typename cLtraits_try_make_unsigned<T2>::type)(mx2) - (typename cLtraits_try_make_unsigned<T2>::type)(mn2);
  if(r1 == 0){
    sortA_1_call_L(N, B, mem);
    return;
  }
  if(r2 == 0){
    sortA_1_call_L(N, A, mem);
    return;
  }
  if(r1 <= N){
    so2 = 1;
    for(i=(1);i<(N);i++){
      if(B[i-1] > B[i]){
        so2 = 0;
        break;
      }
    }
    if(so2 == 1){
      T1*aa;
      T2*bb;
      int*pos;
      int k;
      walloc1d(&aa,N,&mem);
      walloc1d(&bb,N,&mem);
      walloc1d(&pos,r1+2,&mem);
      for(i=(0);i<(r1+2);i++){
        pos[i] = 0;
      }
      for(i=(0);i<(N);i++){
        aa[i] = A[i];
      }
      for(i=(0);i<(N);i++){
        bb[i] = B[i];
      }
      for(i=(0);i<(N);i++){
        pos[(typename cLtraits_try_make_unsigned<T1>::type)((typename cLtraits_try_make_unsigned<T1>::type)aa[i]-(typename cLtraits_try_make_unsigned<T1>::type)mn1)+1]++;
      }
      for(i=(1);i<(r1+2);i++){
        pos[i] += pos[i-1];
      }
      for(i=(0);i<(N);i++){
        k = pos[(typename cLtraits_try_make_unsigned<T1>::type)((typename cLtraits_try_make_unsigned<T1>::type)aa[i]-(typename cLtraits_try_make_unsigned<T1>::type)mn1)+0]++;
        A[k] = aa[i];
        B[k] = bb[i];
      }
      return;
    }
  }
  s1 = s2 = 1;
  while( s1 < 64 && r1 >= (1ULL<<s1) ){
    s1++;
  }
  while( s2 < 64 && r2 >= (1ULL<<s2) ){
    s2++;
  }
  if(s1 + s2 <= 32){
    unsigned*tmp;
    walloc1d(&tmp,N,&mem);
    for(i=(0);i<(N);i++){
      tmp[i] = (((unsigned)((int)A[i]-(int)mn1)) << s2) | ((unsigned)((int)B[i]-(int)mn2));
    }
    sortA_1_call_L(N, tmp, mem);
    for(i=(0);i<(N);i++){
      A[i] = ((int)(tmp[i] >> s2)) + ((int)mn1);
      B[i] = ((int)(tmp[i] & ((1U<<s2)-1))) + ((int)mn2);
    }
    return;
  }
  if(s1 + s2 <= 64){
    unsigned long long*tmp;
    walloc1d(&tmp,N,&mem);
    for(i=(0);i<(N);i++){
      tmp[i] = (((unsigned long long)((long long)A[i]-(long long)mn1)) << s2) | ((unsigned long long)((long long)B[i]-(long long)mn2));
    }
    sortA_1_call_L(N, tmp, mem);
    for(i=(0);i<(N);i++){
      A[i] = ((long long)(tmp[i] >> s2)) + ((long long)mn1);
      B[i] = ((long long)(tmp[i] & ((1ULL<<s2)-1))) + ((long long)mn2);
    }
    return;
  }
  sortI(N,A,B,mem);
}
template<class T1, class T2> void sortA_2_nonint_L(int N, T1 A[], T2 B[], void *mem = wmem){
  sortI(N,A,B,mem);
}
template<class T1, class T2> void sortA_2_call_L(int N, T1 A[], T2 B[], void *mem = wmem){
  sortA_2_nonint_L(N, A, B, mem);
}
template<class T2> void sortA_2_call_L(int N, int A[], T2 B[], void *mem){
  sortA_2_int_L(N, A, B, mem);
}
template<class T2> void sortA_2_call_L(int N, unsigned A[], T2 B[], void *mem){
  sortA_2_int_L(N, A, B, mem);
}
template<class T2> void sortA_2_call_L(int N, long long A[], T2 B[], void *mem){
  sortA_2_int_L(N, A, B, mem);
}
template<class T2> void sortA_2_call_L(int N, unsigned long long A[], T2 B[], void *mem){
  sortA_2_int_L(N, A, B, mem);
}
template<class T2> void sortA_2_call_L(int N, char A[], T2 B[], void *mem){
  sortA_2_int_L(N, A, B, mem);
}
template<class T1, class T2> void sortA(int N, T1 a[], T2 b[], void *mem = wmem){
  sortA_2_call_L(N, a, b, mem);
}
template<class T1, class T2, class T3> void sortA(int N, T1 a[], T2 b[], T3 c[], void *mem = wmem){
  int i;
  pair<T1, pair<T2, T3> >*arr;
  walloc1d(&arr, N, &mem);
  for(i=(0);i<(N);i++){
    arr[i].first = a[i];
    arr[i].second.first = b[i];
    arr[i].second.second = c[i];
  }
  sort(arr, arr+N);
  for(i=(0);i<(N);i++){
    a[i] = arr[i].first;
    b[i] = arr[i].second.first;
    c[i] = arr[i].second.second;
  }
}
template<class T1, class T2, class T3, class T4> void sortA(int N, T1 a[], T2 b[], T3 c[], T4 d[], void *mem = wmem){
  int i;
  pair<pair<T1, T2>, pair<T3, T4> >*arr;
  walloc1d(&arr, N, &mem);
  for(i=(0);i<(N);i++){
    arr[i].first.first = a[i];
    arr[i].first.second = b[i];
    arr[i].second.first = c[i];
    arr[i].second.second = d[i];
  }
  sort(arr, arr+N);
  for(i=(0);i<(N);i++){
    a[i] = arr[i].first.first;
    b[i] = arr[i].first.second;
    c[i] = arr[i].second.first;
    d[i] = arr[i].second.second;
  }
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
inline void rd(long long &x){
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
template<class T> inline void wt_L(vector<T> x);
template<class T> inline void wt_L(set<T> x);
template<class T> inline void wt_L(multiset<T> x);
template<class T1, class T2> inline void wt_L(pair<T1,T2> x);
inline void wt_L(char a){
  my_putchar_unlocked(a);
}
inline void wt_L(int x){
  int s=0;
  int m=0;
  char f[10];
  if(x<0){
    m=1;
    x=-x;
  }
  while(x){
    f[s++]=x%10;
    x/=10;
  }
  if(!s){
    f[s++]=0;
  }
  if(m){
    my_putchar_unlocked('-');
  }
  while(s--){
    my_putchar_unlocked(f[s]+'0');
  }
}
inline void wt_L(long long x){
  int s=0;
  int m=0;
  char f[20];
  if(x<0){
    m=1;
    x=-x;
  }
  while(x){
    f[s++]=x%10;
    x/=10;
  }
  if(!s){
    f[s++]=0;
  }
  if(m){
    my_putchar_unlocked('-');
  }
  while(s--){
    my_putchar_unlocked(f[s]+'0');
  }
}
int WRITER_DOUBLE_DIGIT = 15;
inline int writerDigit_double(){
  return WRITER_DOUBLE_DIGIT;
}
inline void writerDigit_double(int d){
  WRITER_DOUBLE_DIGIT = d;
}
inline void wt_L(double x){
  const int d = WRITER_DOUBLE_DIGIT;
  int k;
  int r;
  double v;
  if(x!=x || (x==x+1 && x==2*x)){
    my_putchar_unlocked('E');
    my_putchar_unlocked('r');
    my_putchar_unlocked('r');
    return;
  }
  if(x < 0){
    my_putchar_unlocked('-');
    x = -x;
  }
  x += 0.5 * pow(0.1, d);
  r = 0;
  v = 1;
  while(x >= 10*v){
    v *= 10;
    r++;
  }
  while(r >= 0){
    r--;
    k = floor(x / v);
    if(k >= 10){
      k = 9;
    }
    if(k <= -1){
      k = 0;
    }
    x -= k * v;
    v *= 0.1;
    my_putchar_unlocked(k + '0');
  }
  if(d > 0){
    my_putchar_unlocked('.');
    v = 1;
    for(r=(0);r<(d);r++){
      v *= 0.1;
      k = floor(x / v);
      if(k >= 10){
        k = 9;
      }
      if(k <= -1){
        k = 0;
      }
      x -= k * v;
      my_putchar_unlocked(k + '0');
    }
  }
}
template<class T> inline T pow2_L(T a){
  return a*a;
}
struct unionFind{
  int*d;
  int N;
  int M;
  unionFind(){
  }
  unionFind(const char mode, const int n, void **mem = &wmem){
    if(mode == 'm'){
      malloc(n);
    }
    if(mode == 'w'){
      walloc(n, mem);
    }
  }
  unionFind(const char mode, const int n, const int fg, void **mem = &wmem){
    if(mode == 'm'){
      malloc(n, fg);
    }
    if(mode == 'w'){
      walloc(n, fg, mem);
    }
  }
  inline void malloc(const int n){
    d = (int*)std::malloc(n*sizeof(int));
    M = n;
  }
  inline void malloc(const int n, const int fg){
    d = (int*)std::malloc(n*sizeof(int));
    M = n;
    if(fg){
      init(n);
    }
  }
  inline void free(void){
    std::free(d);
  }
  inline void walloc(const int n, void **mem=&wmem){
    walloc1d(&d, n, mem);
    M = n;
  }
  inline void walloc(const int n, const int fg, void **mem=&wmem){
    walloc1d(&d, n, mem);
    M = n;
    if(fg){
      init(n);
    }
  }
  inline void init(const int n){
    int i;
    N = n;
    for(i=(0);i<(n);i++){
      d[i] = -1;
    }
  }
  inline void init(void){
    init(M);
  }
  inline int get(int a){
    int t = a;
    int k;
    while(d[t]>=0){
      t=d[t];
    }
    while(d[a]>=0){
      k=d[a];
      d[a]=t;
      a=k;
    }
    return a;
  }
  inline int connect(int a, int b){
    if(d[a]>=0){
      a=get(a);
    }
    if(d[b]>=0){
      b=get(b);
    }
    if(a==b){
      return 0;
    }
    if(d[a] < d[b]){
      d[a] += d[b];
      d[b] = a;
    }
    else{
      d[b] += d[a];
      d[a] = b;
    }
    return 1;
  }
  inline int operator()(int a){
    return get(a);
  }
  inline int operator()(int a, int b){
    return connect(a,b);
  }
  inline int& operator[](const int a){
    return d[a];
  }
  inline int size(int a){
    a = get(a);
    return -d[a];
  }
  inline int sizeList(int res[]){
    int i;
    int sz=0;
    for(i=(0);i<(N);i++){
      if(d[i]<0){
        res[sz++] = -d[i];
      }
    }
    return sz;
  }
  inline int comp(int res[], void *mem = wmem){
    int i;
    int sz=0;
    int*cnt;
    walloc1d(&cnt, N, &mem);
    for(i=(0);i<(N);i++){
      cnt[i] = 0;
    }
    for(i=(0);i<(N);i++){
      cnt[get(i)] = 1;
    }
    for(i=(0);i<(N);i++){
      if(cnt[i]){
        cnt[i] = sz++;
      }
    }
    for(i=(0);i<(N);i++){
      res[i] = cnt[get(i)];
    }
    return sz;
  }
}
;
inline long long Isqrt_c_L(const long long n){
  long long r = sqrt(n);
  r =max_L(r-2, 0);
  while((pow2_L(r))< n ){
    r++;
  }
  return r;
}
#define TL 1.96
Timer timer;
Rand rnd;
int N;
int M;
int K;
long long X[100];
long long Y[100];
int U[300];
int V[300];
long long W[300];
long long A[5000];
long long B[5000];
int home2node[5000][100];
long long dist[5000][100];
int r_K;
int r_use[5000];
int r_lis[5000];
int r_home2node[5000][100];
long long r_dist[5000][100];
int edge[100][100][300];
int es[300][300];
long long edge_cost[100][100];
unionFind uf;
int res_on[300];
long long res_R[100];
int res_node[100];
double res_score;
int tmp_on[300];
long long tmp_R[100];
int tmp_node[100];
long long ttt_R[100];
double calc_score(){
  int i;
  double cost = 0;
  double score;
  for(i=(0);i<(N);i++){
    cost += tmp_R[i] * tmp_R[i];
  }
  for(i=(0);i<(M);i++){
    if(tmp_on[i]){
      cost += W[i];
    }
  }
  score = 1e6 * (1 + 1e8 / (cost + 1e7));
  if(res_score < score){
    res_score = score;
    for(i=(0);i<(M);i++){
      res_on[i] = tmp_on[i];
    }
    for(i=(0);i<(N);i++){
      res_R[i] = tmp_R[i];
    }
    for(i=(0);i<(N);i++){
      res_node[i] = tmp_node[i];
    }
  }
  return score;
}
void ans_print(){
  {
    int pyHJGNQc;
    if(N==0){
      wt_L('\n');
    }
    else{
      for(pyHJGNQc=(0);pyHJGNQc<(N-1);pyHJGNQc++){
        wt_L(res_R[pyHJGNQc]);
        wt_L(' ');
      }
      wt_L(res_R[pyHJGNQc]);
      wt_L('\n');
    }
  }
  {
    int lmouyaID;
    if(M==0){
      wt_L('\n');
    }
    else{
      for(lmouyaID=(0);lmouyaID<(M-1);lmouyaID++){
        wt_L(res_on[lmouyaID]);
        wt_L(' ');
      }
      wt_L(res_on[lmouyaID]);
      wt_L('\n');
    }
  }
}
int vis[100];
int added[100];
long long cur_cost[100];
priority_queue<pair<long long,pair<int,int>>> q;
void solve_node2edge_add(int i){
  int j;
  int k;
  long long c;
  if(added[i]){
    return;
  }
  added[i] = 1;
  for(j=(0);j<(N);j++){
    if(tmp_node[j] && !vis[j] && edge_cost[i][j] < cur_cost[j]){
      cur_cost[j] = edge_cost[i][j];
      q.push( {-cur_cost[j], {i,j}} );
    }
  }
}
double solve_node2edge(){
  int i;
  int j;
  int k;
  int m;
  long long c;
  double res = 0;
  for(i=(0);i<(N);i++){
    vis[i] = added[i] = 0;
  }
  for(i=(0);i<(M);i++){
    tmp_on[i] = 0;
  }
  for(i=(1);i<(N);i++){
    cur_cost[i] = 4611686016279904256LL;
  }
  vis[0] = 1;
  solve_node2edge_add(0);
  while(q.size()){
    c = -q.top().first;
    i = q.top().second.first;
    j = q.top().second.second;
    q.pop();
    if(vis[j]){
      continue;
    }
    vis[j] = 1;
    for(m=(0);m<(es[i][j]);m++){
      k = edge[i][j][m];
      if(tmp_on[k]){
        continue;
      }
      tmp_on[k] = 1;
      res += W[k];
      solve_node2edge_add(U[k]);
      solve_node2edge_add(V[k]);
    }
  }
  return res;
}
int ind[5000];
int chked[5000];
double solve_node2weight(int loop=2, int rough = 0, int fin = 0){
  int i;
  int j;
  int k;
  int ok;
  double res;
  double tmp;
  long long mn;
  long long c;
  int dj;
  int dk;
  res = 1e150;
  if(rough){
    int EEPmwYOn;
    for(i=(0);i<(r_K);i++){
      ind[i] = i;
    }
    for(EEPmwYOn=(0);EEPmwYOn<(loop);EEPmwYOn++){
      int Lbzl0EzV;
      for(i=(0);i<(N);i++){
        ttt_R[i] = 0;
      }
      random_shuffle(ind,ind+r_K);
      for(Lbzl0EzV=(0);Lbzl0EzV<(r_K);Lbzl0EzV++){
        auto&i = ind[Lbzl0EzV];
        mn = 4611686016279904256LL;
        for(j=(0);j<(N);j++){
          if(r_dist[i][j] > 5000){
            break;
          }
          k = r_home2node[i][j];
          if(tmp_node[k]==0){
            continue;
          }
          c = r_dist[i][j] * r_dist[i][j] - ttt_R[k] * ttt_R[k];
          chmax(c, 0);
          if(mn > c){
            mn = c;
            dj = j;
            dk = k;
          }
          if(mn==0){
            break;
          }
        }
        if(mn == 4611686016279904256LL){
          return res;
        }
        chmax(ttt_R[dk], r_dist[i][dj]);
      }
      tmp = 0;
      for(i=(0);i<(N);i++){
        tmp += ttt_R[i] * ttt_R[i];
      }
      if(res > tmp){
        res = tmp;
        for(i=(0);i<(N);i++){
          tmp_R[i] = ttt_R[i];
        }
      }
    }
  }
  else{
    int y3GbSApi;
    for(i=(0);i<(K);i++){
      ind[i] = i;
    }
    for(y3GbSApi=(0);y3GbSApi<(loop);y3GbSApi++){
      int uJ4m8Ld4;
      for(i=(0);i<(N);i++){
        ttt_R[i] = 0;
      }
      for(i=(0);i<(K);i++){
        chked[i] = 0;
      }
      random_shuffle(ind,ind+K);
      if(fin){
        int bU36gMow;
        for(bU36gMow=(0);bU36gMow<(2);bU36gMow++){
          int gx9SJKIt;
          for(gx9SJKIt=(0);gx9SJKIt<(K);gx9SJKIt++){
            auto&i = ind[gx9SJKIt];
            if(!chked[i]){
              mn = 4611686016279904256LL;
              ok = 0;
              for(j=(0);j<(N);j++){
                if(dist[i][j] > 5000){
                  break;
                }
                k = home2node[i][j];
                if(tmp_node[k]==0){
                  continue;
                }
                ok++;
                c = dist[i][j] * dist[i][j] - ttt_R[k] * ttt_R[k];
                chmax(c, 0);
                if(mn > c){
                  mn = c;
                  dj = j;
                  dk = k;
                }
                if(mn==0){
                  break;
                }
              }
              if(mn == 4611686016279904256LL){
                return res;
              }
              if(ok==1 || mn==0){
                chmax(ttt_R[dk], dist[i][dj]);
                chked[i] = 1;
              }
            }
          }
        }
      }
      for(uJ4m8Ld4=(0);uJ4m8Ld4<(K);uJ4m8Ld4++){
        auto&i = ind[uJ4m8Ld4];
        if(!chked[i]){
          mn = 4611686016279904256LL;
          for(j=(0);j<(N);j++){
            if(dist[i][j] > 5000){
              break;
            }
            k = home2node[i][j];
            if(tmp_node[k]==0){
              continue;
            }
            c = dist[i][j] * dist[i][j] - ttt_R[k] * ttt_R[k];
            chmax(c, 0);
            if(mn > c){
              mn = c;
              dj = j;
              dk = k;
            }
            if(mn==0){
              break;
            }
          }
          if(mn == 4611686016279904256LL){
            return res;
          }
          chmax(ttt_R[dk], dist[i][dj]);
        }
      }
      tmp = 0;
      for(i=(0);i<(N);i++){
        tmp += ttt_R[i] * ttt_R[i];
      }
      if(res > tmp){
        res = tmp;
        for(i=(0);i<(N);i++){
          tmp_R[i] = ttt_R[i];
        }
      }
    }
  }
  return res;
}
int main(){
  wmem = memarr;
  int i;
  int j;
  int k;
  int x;
  int y;
  double t_edge = 0;
  double t_weight = 0;
  double t_k;
  timer.set();
  writerDigit_double(2);
  rd(N);
  rd(M);
  rd(K);
  {
    int gt9Icayh;
    for(gt9Icayh=(0);gt9Icayh<(N);gt9Icayh++){
      rd(X[gt9Icayh]);
      rd(Y[gt9Icayh]);
    }
  }
  {
    int rnB8a3xI;
    for(rnB8a3xI=(0);rnB8a3xI<(M);rnB8a3xI++){
      rd(U[rnB8a3xI]);U[rnB8a3xI] += (-1);
      rd(V[rnB8a3xI]);V[rnB8a3xI] += (-1);
      rd(W[rnB8a3xI]);
    }
  }
  {
    int TnpbgAiR;
    for(TnpbgAiR=(0);TnpbgAiR<(K);TnpbgAiR++){
      rd(A[TnpbgAiR]);
      rd(B[TnpbgAiR]);
    }
  }
  uf.walloc(N);
  for(i=(0);i<(K);i++){
    for(j=(0);j<(N);j++){
      home2node[i][j] = j;
      dist[i][j] =Isqrt_c_L((pow2_L((A[i]-X[j])))+(pow2_L((B[i]-Y[j]))));
    }
  }
  for(;;){
    y = 0;
    for(i=(0);i<(K);i++){
      if(r_use[i]==0){
        r_lis[y++] = i;
      }
    }
    if(y==0){
      break;
    }
    k = r_lis[rnd.get(y)];
    for(j=(0);j<(N);j++){
      r_home2node[r_K][j] = j;
    }
    for(i=(0);i<(K);i++){
      if(r_use[i]==0 && (((pow2_L((A[k]-A[i])))) + ((pow2_L((B[k]-B[i]))))) <= 20000){
        r_use[i] = 1;
        for(j=(0);j<(N);j++){
          chmax(r_dist[r_K][j], dist[i][j]);
        }
      }
    }
    r_K++;
  }
  for(i=(0);i<(K);i++){
    sortA(N,dist[i],home2node[i]);
  }
  for(i=(0);i<(r_K);i++){
    sortA(N,r_dist[i],r_home2node[i]);
  }
  for(i=(0);i<(N);i++){
    for(j=(0);j<(N);j++){
      edge_cost[i][j] = 4611686016279904256LL;
    }
  }
  for(i=(0);i<(N);i++){
    edge_cost[i][i] = 0;
    es[i][i] = 0;
  }
  for(i=(0);i<(M);i++){
    for(k=(0);k<(2);k++){
      if(k==0){
        auto TEmw1hE5 = ((U[i]));
        auto bQ1DX57i = (( V[i]));
        x=TEmw1hE5;
        y=bQ1DX57i;
      }
      if(k==1){
        auto ISBunWUX = ((V[i]));
        auto ip2Ud5hL = (( U[i]));
        x=ISBunWUX;
        y=ip2Ud5hL;
      }
      edge[x][y][0] = i;
      es[x][y] = 1;
      edge_cost[x][y] = W[i];
    }
  }
  for(k=(0);k<(N);k++){
    for(i=(0);i<(N);i++){
      for(j=(0);j<(N);j++){
        if(edge_cost[i][j] > edge_cost[i][k] + edge_cost[k][j]){
          edge_cost[i][j] = edge_cost[i][k] + edge_cost[k][j];
          es[i][j] = 0;
          for(x=(0);x<(es[i][k]);x++){
            edge[i][j][es[i][j]++] = edge[i][k][x];
          }
          for(x=(0);x<(es[k][j]);x++){
            edge[i][j][es[i][j]++] = edge[k][j][x];
          }
        }
      }
    }
  }
  int pre_tmp[100];
  double cur;
  int fst = 1;
  for(i=(0);i<(N);i++){
    tmp_node[i] = 1;
  }
  solve_node2edge();
  solve_node2weight();
  cur = calc_score();
  while(timer.get() < TL){
    int i;
    int j;
    int f;
    int loop;
    int rough;
    double r;
    double r1;
    double r2;
    double ttm;
    ttm = 1 - timer.get() / TL;
    if(ttm > 0.5){
      loop = 1;
    }
    else if(ttm > 0.3){
      loop = 2;
    }
    else if(ttm > 0.1){
      loop = 3;
    }
    else{
      loop = 10;
    }
    if(ttm > 0.4){
      rough = 1;
    }
    else{
      rough = 0;
    }
    for(i=(0);i<(N);i++){
      pre_tmp[i] = tmp_node[i];
    }
    if(fst){
      fst = 0;
    }
    else{
      f = rnd.get(N-1) + 1;
      tmp_node[f] ^= 1;
    }
    r2 = solve_node2weight(loop, rough);
    if(r2 > 1e10){
      for(i=(0);i<(N);i++){
        tmp_node[i] = pre_tmp[i];
      }
      continue;
    }
    for(i=(0);i<(N);i++){
      if(tmp_R[i]==0){
        tmp_node[i] = 0;
      }
    }
    r1 = solve_node2edge();
    r = calc_score();
    if(cur <= r || rnd.getUni() < exp((r-cur) / ttm / 2500)){
      cur = r;
    }
    else{
      for(i=(0);i<(N);i++){
        tmp_node[i] = pre_tmp[i];
      }
    }
  }
  ans_print();
  return 0;
}
template<class T> inline void wt_L(vector<T> x){
  int fg = 0;
  for(auto a : x){
    if(fg){
      my_putchar_unlocked(' ');
    }
    fg = 1;
    wt_L(a);
  }
}
template<class T> inline void wt_L(set<T> x){
  int fg = 0;
  for(auto a : x){
    if(fg){
      my_putchar_unlocked(' ');
    }
    fg = 1;
    wt_L(a);
  }
}
template<class T> inline void wt_L(multiset<T> x){
  int fg = 0;
  for(auto a : x){
    if(fg){
      my_putchar_unlocked(' ');
    }
    fg = 1;
    wt_L(a);
  }
}
template<class T1, class T2> inline void wt_L(pair<T1,T2> x){
  wt_L(x.first);
  my_putchar_unlocked(' ');
  wt_L(x.second);
}
// cLay version 20221230-1

// --- original code ---
// #define TL 1.96
// 
// Timer timer;
// Rand rnd;
// 
// int N, M, K;
// ll X[100], Y[100];
// int U[300], V[300]; ll W[300];
// ll A[5000], B[5000];
// 
// int home2node[5000][100]; ll dist[5000][100];
// 
// int r_K, r_use[5000], r_lis[5000];
// int r_home2node[5000][100]; ll r_dist[5000][100];
// 
// int edge[100][100][300]; int es[300][300]; ll edge_cost[100][100];
// 
// unionFind uf;
// 
// int res_on[300]; ll res_R[100]; int res_node[100]; double res_score;
// int tmp_on[300]; ll tmp_R[100]; int tmp_node[100];
// ll ttt_R[100];
// 
// double calc_score(){
//   double cost = 0, score;
//   rep(i,N) cost += tmp_R[i] * tmp_R[i];
//   rep(i,M) if(tmp_on[i]) cost += W[i];
//   score = 1e6 * (1 + 1e8 / (cost + 1e7));
// 
//   if(res_score < score){
//     res_score = score;
//     rep(i,M) res_on[i] = tmp_on[i];
//     rep(i,N) res_R[i] = tmp_R[i];
//     rep(i,N) res_node[i] = tmp_node[i];
//   }
// 
//   return score;
// }
// 
// 
// void ans_print(){
//   wt(res_R(N));
//   wt(res_on(M));
// }
// 
// 
// 
// int vis[100], added[100];
// ll cur_cost[100];
// priority_queue<pair<ll,pair<int,int>>> q;
// 
// void solve_node2edge_add(int i){
//   int j, k;
//   ll c;
//   if(added[i]) return;
//   added[i] = 1;
//   rep(j,N) if(tmp_node[j] && !vis[j] && edge_cost[i][j] < cur_cost[j]){
//     cur_cost[j] = edge_cost[i][j];
//     q.push( {-cur_cost[j], {i,j}} );
//   }
// }
// 
// double solve_node2edge(){
//   int i, j, k, m;
//   ll c;
//   double res = 0;
// 
//   rep(i,N) vis[i] = added[i] = 0;
//   rep(i,M) tmp_on[i] = 0;
// 
//   rep(i,1,N) cur_cost[i] = ll_inf;
// 
//   vis[0] = 1;
//   solve_node2edge_add(0);
// 
//   while(q.size()){
//     c = -q.top().first;
//     i = q.top().second.first;
//     j = q.top().second.second;
//     q.pop();
//     if(vis[j]) continue;
//     vis[j] = 1;
// 
//     rep(m,es[i][j]){
//       k = edge[i][j][m];
//       if(tmp_on[k]) continue;
//       tmp_on[k] = 1;
//       res += W[k];
//       solve_node2edge_add(U[k]);
//       solve_node2edge_add(V[k]);
//     }
//   }
// 
//   return res;
// }
// 
// 
// int ind[5000], chked[5000];
// double solve_node2weight(int loop=2, int rough = 0, int fin = 0){
//   int i, j, k, ok;
//   double res, tmp;
//   ll mn, c; int dj, dk;
// 
//   res = double_inf;
// 
//   if(rough){
//     rep(i,r_K) ind[i] = i;
//     rep(loop){
//       rep(i,N) ttt_R[i] = 0;
//       random_shuffle(ind,ind+r_K);
// 
//       rep[ind](i,r_K){
//         mn = ll_inf;
//         rep(j,N){
//           if(r_dist[i][j] > 5000) break;
//           k = r_home2node[i][j];
//           if(tmp_node[k]==0) continue;
//           c = r_dist[i][j] * r_dist[i][j] - ttt_R[k] * ttt_R[k];
//           c >?= 0;
//           if(mn > c) mn = c, dj = j, dk = k;
//           if(mn==0) break;
//         }
//         if(mn == ll_inf) return res;
//         ttt_R[dk] >?= r_dist[i][dj];
//       }
// 
//       tmp = 0;
//       rep(i,N) tmp += ttt_R[i] * ttt_R[i];
//       if(res > tmp){
//         res = tmp;
//         rep(i,N) tmp_R[i] = ttt_R[i];
//       }
//     }
//   } else {
//     rep(i,K) ind[i] = i;
//     rep(loop){
//       rep(i,N) ttt_R[i] = 0;
//       rep(i,K) chked[i] = 0;
//       random_shuffle(ind,ind+K);
// 
//       if(fin) rep(2){
//         rep[ind](i,K) if(!chked[i]){
//           mn = ll_inf;
//           ok = 0;
//           rep(j,N){
//             if(dist[i][j] > 5000) break;
//             k = home2node[i][j];
//             if(tmp_node[k]==0) continue;
//             ok++;
//             c = dist[i][j] * dist[i][j] - ttt_R[k] * ttt_R[k];
//             c >?= 0;
//             if(mn > c) mn = c, dj = j, dk = k;
//             if(mn==0) break;
//           }
//           if(mn == ll_inf) return res;
//           if(ok==1 || mn==0) ttt_R[dk] >?= dist[i][dj], chked[i] = 1;
//         }
//       }
// 
//       rep[ind](i,K) if(!chked[i]){
//         mn = ll_inf;
//         rep(j,N){
//           if(dist[i][j] > 5000) break;
//           k = home2node[i][j];
//           if(tmp_node[k]==0) continue;
//           c = dist[i][j] * dist[i][j] - ttt_R[k] * ttt_R[k];
//           c >?= 0;
//           if(mn > c) mn = c, dj = j, dk = k;
//           if(mn==0) break;
//         }
//         if(mn == ll_inf) return res;
//         ttt_R[dk] >?= dist[i][dj];
//       }
// 
//       tmp = 0;
//       rep(i,N) tmp += ttt_R[i] * ttt_R[i];
//       if(res > tmp){
//         res = tmp;
//         rep(i,N) tmp_R[i] = ttt_R[i];
//       }
//     }
//   }
// 
//   return res;
// }
// 
// 
// {
//   int i, j, k, x, y;
//   double t_edge = 0, t_weight = 0, t_k;
//   timer.set();
//   writerDigit_double(2);
// 
//   rd(N,M,K,(X,Y)(N),(U--,V--,W)(M),(A,B)(K));
//   uf.walloc(N);
// 
//   rep(i,K) rep(j,N){
//     home2node[i][j] = j;
//     dist[i][j] = Isqrt_c( (A[i]-X[j]) ** 2 + (B[i]-Y[j]) ** 2 );
//   }
// 
//   for(;;){
//     y = 0;
//     rep(i,K) if(r_use[i]==0) r_lis[y++] = i;
//     if(y==0) break;
// 
//     k = r_lis[rnd.get(y)];
//     rep(j,N) r_home2node[r_K][j] = j;
//     rep(i,K) if(r_use[i]==0 && (((A[k]-A[i]) ** 2) + ((B[k]-B[i]) ** 2)) <= 20000){
//       r_use[i] = 1;
//       rep(j,N) r_dist[r_K][j] >?= dist[i][j];
//     }
//     r_K++;
//   }
// 
//   rep(i,K) sortA(N,dist[i],home2node[i]);
//   rep(i,r_K) sortA(N,r_dist[i],r_home2node[i]);
// 
//   rep(i,N) rep(j,N) edge_cost[i][j] = ll_inf;
//   rep(i,N) edge_cost[i][i] = 0, es[i][i] = 0;
//   rep(i,M) rep(k,2){
//     if(k==0) (x, y) = (U[i], V[i]);
//     if(k==1) (x, y) = (V[i], U[i]);
//     edge[x][y][0] = i;
//     es[x][y] = 1;
//     edge_cost[x][y] = W[i];
//   }
//   rep(k,N) rep(i,N) rep(j,N) if(edge_cost[i][j] > edge_cost[i][k] + edge_cost[k][j]){
//     edge_cost[i][j] = edge_cost[i][k] + edge_cost[k][j];
//     es[i][j] = 0;
//     rep(x,es[i][k]) edge[i][j][es[i][j]++] = edge[i][k][x];
//     rep(x,es[k][j]) edge[i][j][es[i][j]++] = edge[k][j][x];
//   }
// 
//   int pre_tmp[100];
//   double cur;
//   int fst = 1;
// 
//   rep(i,N) tmp_node[i] = 1;
//   solve_node2edge();
//   solve_node2weight();
//   cur = calc_score();
// 
//   while(timer.get() < TL){
//     int i, j, f, loop, rough;
//     double r, r1, r2;
//     double ttm;
// 
//     ttm = 1 - timer.get() / TL;
// 
//     if(ttm > 0.5)      loop = 1;
//     else if(ttm > 0.3) loop = 2;
//     else if(ttm > 0.1) loop = 3;
//     else               loop = 10;
// 
//     if(ttm > 0.4) rough = 1;
//     else          rough = 0;
// 
//     rep(i,N) pre_tmp[i] = tmp_node[i];
//     if(fst){
//       fst = 0;
//     } else {
//       f = rnd.get(N-1) + 1;
//       tmp_node[f] ^= 1;
//     }
// 
//     // t_k = timer.get();
//     r2 = solve_node2weight(loop, rough);
//     // t_weight += timer.get() - t_k;
//     if(r2 > 1e10){
//       rep(i,N) tmp_node[i] = pre_tmp[i];
//       continue;
//     }
//     rep(i,N) if(tmp_R[i]==0) tmp_node[i] = 0;
// 
//     // t_k = timer.get();
//     r1 = solve_node2edge();
//     // t_edge += timer.get() - t_k;
//     r = calc_score();
// 
//     if(cur <= r || rnd.getUni() < exp((r-cur) / ttm / 2500)){
//       cur = r;
//     } else {
//       rep(i,N) tmp_node[i] = pre_tmp[i];
//     }
//   }
// 
//   // fprintf(stderr, "%f\n", res_score);
// 
//   // rep(i,N) tmp_node[i] = res_node[i];
//   // solve_node2edge();
//   // uf.init(N);
//   // rep(i,M) if(tmp_on[i]) uf(U[i],V[i]);
//   // rep(i,N) if(uf(0)==uf(i)) tmp_node[i] = 1;
//   // solve_node2weight(10, 0, 1);
//   // calc_score();
// 
//   ans_print();
//   // fprintf(stderr, "%f\n", res_score);
//   // puts("");
// 
//   // fprintf(stderr, "%f %f\n", t_edge, t_weight);
//   // wt(K,r_K);
// }
