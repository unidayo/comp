#pragma GCC optimize "O3,omit-frame-pointer,inline,unroll-loops"
// #pragma GCC target "sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2"
// #include <algorithm>
// #include <cstdlib>
// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <cmath>
#include "bits/stdc++.h"
// #include <sys/time.h>
// #include <emmintrin.h>
// #include <string>
// #include <bitset>
// #include <unistd.h>

using namespace std;

#ifdef LOCAL
inline double GetSeconds() {
  return static_cast<double>(chrono::duration_cast<chrono::nanoseconds>(
        chrono::steady_clock::now().time_since_epoch()).count())/1000000000;
}
#else
inline long long GetTSC() {
  long long lo, hi;
  asm volatile ("rdtsc": "=a"(lo), "=d"(hi));
  return lo + (hi << 32);
}
inline double GetSeconds() {
  return GetTSC() / 3.0e9;
}
#endif

struct XorShift {
  uint64_t x = 88172645463325252ULL;
  double nl[65536];
  XorShift() {}

  void init(){
    double n2 = 1.0 / 65536*2;
    for(int i=0; i<65536; i++) nl[i] = log(i / 65536.0 + n2);
  }

  void setSeed(uint64_t seed) { x = seed; init(); }

  uint64_t next() {
    x ^= x << 13; x ^= x >> 7; x ^= x << 17;
    return x;
  }

  int nextInt(int n) {
    uint64_t upper = 0xFFFFFFFFFFFFFFFFULL / n * n;
    uint64_t v = next();
    while (v >= upper) v = next();
    return v % n;
  }

  double nextDouble() {
    uint64_t v = next() >> 11; // 53bit
    return (double)v / (1ULL << 53);
  }

  double nextLog() {
    return nl[nextInt(65536)];
  }
};

struct dsu {
  public:
    dsu() : _n(0) {}
    explicit dsu(int n) : _n(n), parent_or_size(n, -1) {}

    int merge(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        int x = leader(a), y = leader(b);
        if (x == y) return x;
        if (-parent_or_size[x] < -parent_or_size[y]) std::swap(x, y);
        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;
        return x;
    }

    bool same(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        return leader(a) == leader(b);
    }

    int leader(int a) {
        assert(0 <= a && a < _n);
        if (parent_or_size[a] < 0) return a;
        return parent_or_size[a] = leader(parent_or_size[a]);
    }

    int size(int a) {
        assert(0 <= a && a < _n);
        return -parent_or_size[leader(a)];
    }

    std::vector<std::vector<int>> groups() {
        std::vector<int> leader_buf(_n), group_size(_n);
        for (int i = 0; i < _n; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<int>> result(_n);
        for (int i = 0; i < _n; i++) {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < _n; i++) {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [&](const std::vector<int>& v) { return v.empty(); }),
            result.end());
        return result;
    }

  private:
    int _n;
    // root node: -1 * component size
    // otherwise: parent
    std::vector<int> parent_or_size;
};


XorShift xs;
#ifdef LOCAL
const double TO = 1.95 * 1e0;
#else
const double TO = 1.95 * 1e0;
#endif

double starttime, gt, gg;
int att,btt,ctt,dtt,ett,ftt,gtt,htt;

const int N = 100;
const int MAX_M = 300;
const int MAX_K = 5000;

int M, K;
int X[N], Y[N];
int U[MAX_M], V[MAX_M], W[MAX_M];
int A[MAX_K], B[MAX_K];

tuple<int,int,int,int> WS[MAX_M];
tuple<int,int> AD[MAX_K][N];
tuple<int,int> AA[MAX_K];
const int ADUI = 2;

bitset<N> use;
bitset<MAX_M> ur, mur;
double rr[N], mrr[N];

const int KMM = 200;
int KM[KMM];
int KMS[KMM][MAX_K];
int KMSI[KMM];
tuple<int,int> KMA[MAX_K];

void kmeans_(){
  for(int i=0; i<KMM; i++) KMSI[i] = 0;
  for(int i=0; i<K; i++){
    int mi = 0, mm = 1e9;
    for(int j=0; j<KMM; j++){
      int k = KM[j];
      int tt = (A[k] - A[i]) * (A[k] - A[i]) + (B[k] - B[i]) * (B[k] * B[i]);
      if(tt < mm){
        mm = tt;
        mi = j;
      }
    }
    KMS[mi][KMSI[mi]++] = i;
  }

  for(int i=0; i<KMM; i++){
    if(KMSI[i] < 2) continue;
    double ak = 0, bk = 0;
    for(int j=0; j<KMSI[i]; j++){
      ak += A[KMS[i][j]];
      bk += B[KMS[i][j]];
    }
    ak /= KMSI[i];
    bk /= KMSI[i];
    int mi = 0;
    double mm = 1e9;
    for(int j=0; j<KMSI[i]; j++){
      int k = KMS[i][j];
      double tt = (A[k] - ak) * (A[k] - ak) + (B[k] - bk) * (B[k] * bk);
      if(tt < mm){
        mm = tt;
        mi = j;
      }
    }
    KM[i] = KMS[i][mi];
  }
}

void kmeans(){
  bitset<MAX_K> ku;
  KM[0] = xs.nextInt(K);
  ku[KM[0]] = 1;
  for(int i=1; i<KMM; i++){
    int km = 0;
    double mm = 0;
    for(int t=0; t<100; t++){
      int k = xs.nextInt(K);
      while(ku[k]) k = xs.nextInt(K);

      double tt = 0;
      for(int j=0; j<i; j++) tt += (A[KM[j] - A[k]]) * (A[KM[j]] - A[k]) + (B[KM[j]] - B[k]) * (B[KM[j]] * B[k]);
      if(tt > mm){
        mm = tt;
        km = k;
      }
    }
    KM[i] = km;
  }

  for(int i=0; i<10; i++) kmeans_();

  for(int ki=0; ki<KMM; ki++){
    int i = KM[ki];
    KMA[i] = make_tuple(-get<0>(AD[i][ADUI]), i);
  }
  sort(&KMA[0], &KMA[KMM]);

  for(int i=0; i<KMM; i++){
    KM[i] = get<1>(KMA[i]);
  }
}

void init(){
  int n;
  cin >> n >> M >> K;

  for(int i=0; i<N; i++) cin >> X[i] >> Y[i];
  for(int i=0; i<M; i++){
    cin >> U[i] >> V[i] >> W[i];
    U[i]--; V[i]--;
  }
  for(int i=0; i<K; i++) cin >> A[i] >> B[i];

  for(int i=0; i<M; i++){
    WS[i] = make_tuple(W[i], U[i], V[i], i);
  }
  sort(&WS[0], &WS[M]);

  for(int i=0; i<K; i++){
    for(int j=0; j<N; j++){
      AD[i][j] = make_tuple((A[i]-X[j]) * (A[i]-X[j]) + (B[i]-Y[j]) * (B[i]-Y[j]), j);
    }
    sort(&AD[i][0], &AD[i][N]);
    AA[i] = make_tuple(get<0>(AD[i][ADUI]), i);
  }
  sort(&AA[0], &AA[K]);

  kmeans();

  cerr << "int_time: " << (GetSeconds() - starttime) << endl;
}

double calc_k(){
  double r = 0;
  dsu uf = dsu(N);
  ur.reset();
  for(int i=0; i<M; i++){
    int w = get<0>(WS[i]);
    int u = get<1>(WS[i]);
    int v = get<2>(WS[i]);
    if(!use[u] || !use[v] || uf.same(u, v)) continue;
    if(0 && att == 55 && get<3>(WS[i]) == 85){
      cerr << "wuv: " << w << " " << u << " " << v;
      cerr << " use: " << use[u] << " " << use[v] << " " << uf.same(u, v) << endl;
    }
    r += w;
    uf.merge(u, v);
    int wi = get<3>(WS[i]);
    ur[wi] = 1;
  }

  for(int i=1; i<N; i++){
    if(use[i] && !uf.same(0, i)){
      // cerr << "not: " << i << endl;
      r += 1e8;
    }
  }

  for(int i=0; i<N; i++) rr[i] = 0;
  for(int ai=0; ai<KMM; ai++){
    int i = KM[0];
    int mi = 0, mm = 1e9;;
    for(int j=0; j<N; j++){
      if(!use[get<1>(AD[i][j])]) continue;
      int tt = get<0>(AD[i][j]) - rr[get<1>(AD[i][j])];
      if(tt < mm){
        mm = tt;
        mi = j;
        if(tt <= 0) break;
      }
    }
    if(mm <= 0) continue;
    rr[get<1>(AD[i][mi])] = get<0>(AD[i][mi]);
  }

  for(int i=0; i<N; i++){
    r += rr[i];
    if(rr[i] > 5000 * 5000) r += 1e8;
  }
  return r;
}

double calc(){
  double r = 0;
  dsu uf = dsu(N);
  ur.reset();
  for(int i=0; i<M; i++){
    int w = get<0>(WS[i]);
    int u = get<1>(WS[i]);
    int v = get<2>(WS[i]);
    if(!use[u] || !use[v] || uf.same(u, v)) continue;
    if(0 && att == 55 && get<3>(WS[i]) == 85){
      cerr << "wuv: " << w << " " << u << " " << v;
      cerr << " use: " << use[u] << " " << use[v] << " " << uf.same(u, v) << endl;
    }
    r += w;
    uf.merge(u, v);
    int wi = get<3>(WS[i]);
    ur[wi] = 1;
  }

  for(int i=1; i<N; i++){
    if(use[i] && !uf.same(0, i)){
      // cerr << "not: " << i << endl;
      r += 1e8;
    }
  }

  for(int i=0; i<N; i++) rr[i] = 0;
  for(int ai=0; ai<K; ai++){
    int i = get<1>(AA[K-ai-1]);
    int mi = 0, mm = 1e9;;
    for(int j=0; j<N; j++){
      if(!use[get<1>(AD[i][j])]) continue;
      int tt = get<0>(AD[i][j]) - rr[get<1>(AD[i][j])];
      if(tt < mm){
        mm = tt;
        mi = j;
        if(tt <= 0) break;
      }
    }
    if(mm <= 0) continue;
    rr[get<1>(AD[i][mi])] = get<0>(AD[i][mi]);
  }

  for(int i=0; i<N; i++){
    r += rr[i];
    if(rr[i] > 5000 * 5000) r += 1e8;
  }
  return r;
}

void solve(){
  use.set();
  double kms = calc_k();
  double ms = calc();
  mur = ur;
  memcpy(mrr, rr, sizeof(rr));

  double ss = 0, kss = 0;
  while(1){
    gt = GetSeconds() - starttime;
    if(gt >= TO) break;

    gg = 1 - gt / TO;
    double gg = (TO - gt) / TO;

    att++;
    int a = att % (N-1) + 1;//xs.nextInt(N-1) + 1;
    use[a] = !use[a];
    double kts = calc_k();
    kss += abs(kms - kts);
    double lg = xs.nextLog();
    if(kms >= kts || (kms - kts) > kss / att / 2 * lg * gg){
      ctt++;
      double ts = calc();
      ss += abs(ms - ts);
      if(ms >= ts || (ms - ts) > ss / ctt / 2 * lg * gg){
        ms = ts;
        mur = ur;
        memcpy(mrr, rr, sizeof(rr));
        btt++;
      }else{
        use[a] = !use[a];
      }
    }else{
      use[a] = !use[a];
    }
  }


  cerr << "time: " << (GetSeconds() - starttime) << " ms: " << ms << endl;
#ifdef DEBUG
  cerr << " att: " << att << " btt: " << btt;
  cerr << " ctt: " << ctt << " dtt: " << dtt << " ett: " << ett;
  cerr << " ftt: " << ftt << " gtt: " << gtt << " htt: " << htt << endl;
#endif
}

void output(){
  for(int i=0; i<N; i++){
    int s = pow(mrr[i], 0.5);
    if(s*s < mrr[i]) s += 1;
    cout << s << ' ';
  }
  cout << '\n';
  for(int i=0; i<M; i++) cout << mur[i] << ' ';
  cout << '\n';
  cout << flush;
}

int main() {
  starttime = GetSeconds();
  srand((unsigned) time(NULL));
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  xs.setSeed(rand() % 65536 + 65537);
  cerr << "x: " << xs.x << endl;
  xs.next();

  init();
  solve();
  output();

  return 0;
}

