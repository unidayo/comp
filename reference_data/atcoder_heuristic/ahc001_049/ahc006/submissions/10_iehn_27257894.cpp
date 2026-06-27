#pragma GCC optimize "O3,omit-frame-pointer,inline,unroll-loops"
#pragma GCC target "sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "bits/stdc++.h"
#include <sys/time.h>
#include <emmintrin.h>
#include <string>
#include <bitset>
#include <unistd.h>

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


XorShift xs;
#ifdef LOCAL
const double TO = 1.97 * 1.2e-0 * 2;
#else
const double TO = 1.97 * 1e0;
#endif

double starttime, gt;
int att,btt,ctt,dtt,ett,ftt,gtt,htt;

const int N = 1000;
const int N2 = 2000;

int A[N], B[N], C[N+1], D[N+1];
int DD[N2+1][N2+1];

tuple<int, int> TV[N];

bitset<N> U, NU;

int R[102];
int P[N2];

inline int dist(int a, int b, int c, int d){
  return abs(a - c) + abs(b - d);
}

void init(){
  for(int i=0; i<N; i++) cin >> A[i] >> B[i] >> C[i] >> D[i];
  C[N] = D[N] = 400;

  for(int i=0; i<N; i++){
    DD[i][i+N] = DD[i+N][i] = dist(A[i], B[i], C[i], D[i]);
    TV[i] = make_tuple(DD[i][i+N], i);
    DD[N2][i] = DD[i][N2] = dist(400, 400, A[i], B[i]);
    DD[N2][i+N] = DD[i+N][N2] = dist(400, 400, C[i], D[i]);
    for(int j=i+1; j<N; j++){
      DD[i][j] = DD[j][i] = dist(A[i], B[i], A[j], B[j]);
      DD[i+N][j] = DD[j][i+N] = dist(C[i], D[i], A[j], B[j]);
      DD[i][j+N] = DD[j+N][i] = dist(A[i], B[i], C[j], D[j]);
      DD[i+N][j+N] = DD[j+N][i+N] = dist(C[i], D[i], C[j], D[j]);
    }
  }

  sort(&TV[0], &TV[N]);
  NU.reset();
  for(int i=700; i<N; i++) NU[get<1>(TV[i])] = 1;
}


const int QM = 128;
tuple<int, int, int, int, int> MV[QM * 700];
int MVI;
int TS[102], TT[102];

struct State{
  int sc, rs;
  int r[102];
  bitset<N> u;

  State() { u.reset(); }

  void nextMoves(int _bi){
    int mx = max(200, 2000 / rs);
    for(int i=0; i<N; i++) if(!u[i]){
      auto ds = DD[i];
      auto dt = DD[i+N];
      int d0 = ds[i+N];
      int mm = 1e5, ms, mt;
      int mms = 1e5;
      int mmt = 1e5;
      for(int j=0; j<rs-1; j++){
        int dj = DD[r[j]][r[j+1]];
        TS[j] = ds[r[j]] + ds[r[j+1]] - dj;
        TT[j] = dt[r[j]] + dt[r[j+1]] - dj;
        mms = min(mms, TS[j]);
        mmt = min(mmt, TT[j]);
        int d = ds[r[j]] + dt[r[j+1]] + d0 - dj;
        if(mm > d){
          mm = d;
          ms = mt = j;
        }
      }
      if(mm > mx && mms+mmt > mx) continue;
      int md = 1e5;
      for(int j=0; j<rs-1; j++){
        int d = TS[j];
        if(d >= md) continue;
        md = d;
        for(int k=j+1; k<rs-1; k++){
          if(d + TT[k] < mm){
            mm = d + TT[k];
            ms = j; mt = k;
          }
        }
      }

      MV[MVI++] = make_tuple(sc + mm, _bi, i, ms, mt);
    }
  }

  State move(tuple<int, int, int, int, int> mv){
    int i = get<2>(mv);
    int j = get<3>(mv);
    int k = get<4>(mv);
    State s;
    s.sc = get<0>(mv);
    s.rs = rs + 2;
    s.u = u;
    s.u[i] = 1;
    int ri = 0;
    for(int l=0; l<rs; l++){
      s.r[ri++] = r[l];
      if(l == j) s.r[ri++] = i;
      if(l == k) s.r[ri++] = i + N;
    }
    return s;
  }
};

State Q[2][QM];
int QI[2];

void solve(){
  State ss = State();
  ss.sc = 0;
  ss.rs = 2;
  ss.r[0] = ss.r[1] = N2;
  ss.u = NU;
  Q[0][0] = ss;
  QI[0] = 1;

  for(int i=0; i<50; i++){
    int qi = i % 2;
    int qs = QI[qi];
    // cerr << i << ": " << qs << " " << Q[qi][0].sc << endl;
    int qj = 1 - qi;
    MVI = 0;
    for(int j=0; j<qs; j++) Q[qi][j].nextMoves(j);
    sort(&MV[0], &MV[MVI]);

    int ns = 0;
    for(int j=0; j<MVI; j++){
      Q[qj][ns] = Q[qi][get<1>(MV[j])].move(MV[j]);
      const auto u = Q[qj][ns].u;
      bool ng = 0;
      for(int k=0; k<ns; k++) if(Q[qj][k].u == u){ ng = 1; break; }
      if(ng) continue;
      ns++;
      if(ns >= QM) break;
    }
    QI[qj] = ns;
  }

  U = Q[0][0].u ^ NU;
  memcpy(R, Q[0][0].r, sizeof(R));
  int r = Q[0][0].sc;

  cerr << "sc: " << r << " time: " << GetSeconds() - starttime << endl;

  for(int i=1; i<101; i++) P[R[i]] = i;
  while(1){
    gt = GetSeconds() - starttime;
    if(gt >= TO) break;
    dtt++;

    int a = xs.nextInt(100) + 1;
    int t = R[a];
    int b;
    if(t < N){
      int p = P[t+N];
      b = xs.nextInt(p - 1) + 1;
    }else{
      int p = P[t-N];
      b = xs.nextInt(100 - p) + p + 1;
    }
    if(a == b) continue;

    int s = R[b];
    if(s < N){
      if(P[s+N] <= a) continue;
    }else{
      if(P[s-N] >= a) continue;
    }

    if(a > b){
      swap(a, b);
      swap(t, s);
    }

    att++;

    int cs = DD[R[a-1]][t] + DD[s][R[b+1]];
    int ns = DD[R[a-1]][s] + DD[t][R[b+1]];
    if(a+1 < b){
      cs += DD[t][R[a+1]] + DD[R[b-1]][s];
      ns += DD[s][R[a+1]] + DD[R[b-1]][t];
    }

    if(ns <= cs){
      btt++;
      if(ns < cs) ctt++;

      r += ns - cs;
      swap(R[a], R[b]);
      swap(P[t], P[s]);
    }
  }

  int ms = 1e8 / (1000 + r);

#ifdef DEBUG
  int md = 0;
  for(int i=0; i<N; i++) if(U[i]){
    cerr << DD[i][i+N] << " ";
    md = max(md, DD[i][i+N]);
  }
  cerr << endl;
  int c = 0;
  for(int i=0; i<N; i++) if(DD[i][i+N] > md) c++;
  cerr << md << " c: " << c << endl;
  cerr << "ms: " << ms << " * 100 = " << (ms)*100;
  cerr << " sc: " << r;
  cerr << " time: " << (GetSeconds() - starttime);
  cerr << " att: " << att << " btt: " << btt;
  cerr << " ctt: " << ctt << " dtt: " << dtt << " ett: " << ett;
  cerr << " ftt: " << ftt << " gtt: " << gtt << " htt: " << htt << endl;
#endif
}

void output(){
  cout << 50;
  for(int i=0; i<N; i++) if(U[i]) cout << " " << i+1;
  cout << "\n102";
  for(int i=0; i<102; i++){
    int t = R[i];
    if(t < N) cout << " " << A[t] << " " << B[t];
    else cout << " " << C[t-N] << " " << D[t-N];
  }
  cout << endl;
}

int main() {
  starttime = GetSeconds();
  srand((unsigned) time(NULL));
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  xs.setSeed(rand() % 65536 + 65537);
  cerr << "x: " << xs.x << endl;

  init();
  solve();
  output();

  return 0;
}

