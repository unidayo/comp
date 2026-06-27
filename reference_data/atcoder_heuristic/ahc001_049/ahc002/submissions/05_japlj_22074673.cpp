#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <utility>
#include <numeric>
#include <algorithm>
#include <bitset>
#include <complex>
#include <array>
#include <list>
#include <stack>
#include <valarray>

using namespace std;

typedef unsigned uint;
typedef long long Int;
typedef unsigned long long UInt;

const int INF = 1001001001;
const Int INFLL = 1001001001001001001LL;

template<typename T> void pv(T a, T b) { for (T i = a; i != b; ++i) cout << *i << " "; cout << endl; }
template<typename T> void chmin(T& a, T b) { if (a > b) a = b; }
template<typename T> void chmax(T& a, T b) { if (a < b) a = b; }
int in() { int x; scanf("%d", &x); return x; }
double fin() { double x; scanf("%lf", &x); return x; }
Int lin() { Int x; scanf("%lld", &x); return x; }

clock_t start_time;

double elapsed() {
  return (clock() - start_time) / (double)CLOCKS_PER_SEC;
}

namespace ran {
typedef unsigned long long UInt;
static unsigned x = 123456789, y = 362436069, z = 521288629, w = 88675123;

unsigned xor128() {
  unsigned t = x ^ (x << 11);
  x = y; y = z; z = w;
  return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

void init(unsigned seed) {
  x = 123456789; y = 362436069; z = 521288629;
  w = seed;
  for (int i = 0; i < 100; ++i) {
    xor128();
  }
}

void init(const char* seed) {
  unsigned iseed = 0;
  for (int i = 0; seed[i]; ++i) {
    iseed = iseed * 1000000007 + seed[i];
  }
  init(iseed);
}

int ran(int lo, int hi) {
  UInt v = ((UInt)xor128() << 32) | xor128();
  return v % (hi - lo + 1) + lo;
}

double dran() {
  return xor128() / (double)numeric_limits<unsigned>::max();
}

template<typename T> void shuffle(T a, T b) {
  const int n = b - a;
  for (int i = 1; i < n; ++i) {
    swap(a[i], a[ran::ran(0, i)]);
  }
}
}

const int SZ = 50;
bool inside(int r, int c) {
  return 0 <= r && r < SZ && 0 <= c && c < SZ;
}

const int DR[] = {1, 0, -1, 0}, DC[] = {0, 1, 0, -1};
const char* DIR = "DRUL";
struct pos {
  int r, c;
  pos() { }
  pos(int r, int c) : r(r), c(c) { }
  bool inside() const { return ::inside(r, c); }
  pair<int, int> pr() const { return make_pair(r, c); }
  bool operator==(const pos& p) const {
    return pr() == p.pr();
  }
  bool operator<(const pos& p) const {
    return pr() < p.pr();
  }
  pos go(int d) const {
    return pos(r + DR[d], c + DC[d]);
  }
};

int S[64][64], U[64][64], tileId[64][64], NX[64][64];
pos P[64][64];

int IU[64][64], INX[64][64], ISCORE;
int BESTU[64][64], BESTNX[64][64], BESTSCORE;

int random_walk(int sr, int sc) {
  int score = 0;
  for (int i = 0; i < SZ; ++i) {
    fill(U[i], U[i] + SZ, 0);
    fill(NX[i], NX[i] + SZ, -1);
  }
  U[sr][sc] = 1;
  score += S[sr][sc];
  pos p(sr, sc);
  while (true) {
    bool moved = false;
    int ds[4] = {0, 1, 2, 3};
    ran::shuffle(ds, ds + 4);
    for (const int d : ds) {
      pos pp = p.go(d);
      if (!pp.inside()) {
        continue;
      }
      const pos pz = P[pp.r][pp.c];
      if (U[pp.r][pp.c] || U[pz.r][pz.c]) {
        continue;
      }
      U[pp.r][pp.c] = 1;
      score += S[pp.r][pp.c];
      NX[p.r][p.c] = d;
      p = pp;
      moved = true;
      break;
    }
    if (!moved) {
      break;
    }
  }
  return score;
}

int greedy_walk(int sr, int sc) {
  int score = 0;
  for (int i = 0; i < SZ; ++i) {
    fill(U[i], U[i] + SZ, 0);
    fill(NX[i], NX[i] + SZ, -1);
  }
  U[sr][sc] = 1;
  score += S[sr][sc];
  pos p(sr, sc);
  while (true) {
    int ds[4] = {0, 1, 2, 3};

    int bestd = -1;
    double bests = -1e9;
    for (const int d : ds) {
      pos pp = p.go(d);
      if (!pp.inside()) {
        continue;
      }
      const pos pz = P[pp.r][pp.c];
      if (U[pp.r][pp.c] || U[pz.r][pz.c]) {
        continue;
      }
      double s = S[pp.r][pp.c] + (abs(pp.r - sr) + abs(pp.c - sc)) * 20;
      if (bests < s) {
        bests = s;
        bestd = d;
      }
    }

    if (bestd == -1) {
      break;
    }
    NX[p.r][p.c] = bestd;
    p = p.go(bestd);
    U[p.r][p.c] = 1;
    score += S[p.r][p.c];
  }
  return score;
}

int best_score, search_lim;
vector<int> cur_ds, best_ds;

void search(const pos& p, const pos& goal, int score, bool no_goal = false) {
  score += S[p.r][p.c];
  if (no_goal || p == goal) {
    if (score > best_score) {
      best_score = score;
      best_ds = cur_ds;
      search_lim = -1;
    }
    if (!no_goal) {
      return;
    }
  }

  if (--search_lim <= 0) {
    return;
  }
  int ds[] = {0, 1, 2, 3};
  ran::shuffle(ds, ds + 4);
  for (const int d : ds) {
    const pos pp = p.go(d);
    if (!pp.inside()) {
      continue;
    }
    const bool goal_special = !no_goal && pp == goal;
    if (!goal_special) {
      const pos pz = P[pp.r][pp.c];
      if (U[pp.r][pp.c] || U[pz.r][pz.c]) {
        continue;
      }
    }
    cur_ds.push_back(d);
    U[pp.r][pp.c] = 1;
    search(pp, goal, score, no_goal);
    if (!goal_special) {
      U[pp.r][pp.c] = 0;
    }
    cur_ds.pop_back();
  }

  score -= S[p.r][p.c];
}

int improve(int sr, int sc, int len, int lim) {
  vector<int> org_ds;
  int org_score = S[sr][sc];
  pos p(sr, sc);
  bool no_goal = false;
  for (int i = 0; i < len; ++i) {
    const int d = NX[p.r][p.c];
    U[p.r][p.c] = 0;
    if (d == -1) {
      no_goal = true;
      break;
    }
    NX[p.r][p.c] = -1;
    org_ds.push_back(d);
    p = p.go(d);
    org_score += S[p.r][p.c];
  }

  best_score = org_score;
  best_ds = org_ds;
  search_lim = lim;
  U[sr][sc] = 1;
  if (!no_goal) assert(U[p.r][p.c] == 1);
  search(pos(sr, sc), p, 0, no_goal);

  int new_score = S[sr][sc];
  p = pos(sr, sc);
  for (const int d : best_ds) {
    U[p.r][p.c] = 1;
    NX[p.r][p.c] = d;
    p = p.go(d);
    new_score += S[p.r][p.c];
  }
  U[p.r][p.c] = 1;

  return new_score - org_score;
}

int improve_random(int len, int lim) {
  while (true) {
    const int r = ran::ran(0, SZ - 1);
    const int c = ran::ran(0, SZ - 1);
    if (U[r][c]) {
      return improve(r, c, len, lim);
    }
  }
}

int steps(int sr, int sc) {
  pos p(sr, sc);
  int s = 1;
  while (true) {
    if (NX[p.r][p.c] == -1) {
      break;
    }
    ++s;
    p = p.go(NX[p.r][p.c]);
  }
  return s;
}

void output(int sr, int sc) {
  pos p(sr, sc);
  while (true) {
    if (BESTNX[p.r][p.c] == -1) {
      break;
    }
    putchar(DIR[BESTNX[p.r][p.c]]);
    p = p.go(BESTNX[p.r][p.c]);
  }
  puts("");
}

int main() {
  start_time = clock();
  int sr = in();
  int sc = in();
  for (int i = 0; i < SZ; ++i) {
    for (int j = 0; j < SZ; ++j) {
      tileId[i][j] = in();
    }
  }
  for (int i = 0; i < SZ; ++i) {
    for (int j = 0; j < SZ; ++j) {
      S[i][j] = in();
    }
  }
  for (int r = 0; r < SZ; ++r) {
    for (int c = 0; c < SZ; ++c) {
      P[r][c] = pos(r, c);
      for (int d = 0; d < 4; ++d) {
        const int rr = r + DR[d], cc = c + DC[d];
        if (!inside(rr, cc)) {
          continue;
        }
        if (tileId[r][c] == tileId[rr][cc]) {
          P[r][c] = pos(rr, cc);
          break;
        }
      }
    }
  }

  ISCORE = random_walk(sr, sc);
  ISCORE += improve(sr, sc, 2000, 1000000);
  memcpy(INX, NX, sizeof(NX));
  memcpy(IU, U, sizeof(U));
  BESTSCORE = -1;
  int first_loops = 0;
//   for (int outer_loop = 0; outer_loop < 20; ++outer_loop) {
  while (elapsed() < 0.9) {
    ++first_loops;
    memcpy(NX, INX, sizeof(NX));
    memcpy(U, IU, sizeof(U));
    int score = ISCORE;
    for (int i = 0; i < 5000; ++i) {
      const int len = ran::ran(45, 55);
      int v = improve_random(len, 300 + len);
      if (v > 0) {
        score += v;
      }
    }
    if (BESTSCORE < score) {
      BESTSCORE = score;
      memcpy(BESTNX, NX, sizeof(NX));
      memcpy(BESTU, U, sizeof(U));
    }
  }

  // BESTSCORE = -1;
  // for (int outer_loop = 0; outer_loop < 5; ++outer_loop) {
  //   int score = random_walk(sr, sc);
  //   score += improve(sr, sc, 2000, 1000000);
  //   for (int i = 0; i < 3000; ++i) {
  //     int v = improve_random(ran::ran(45, 55), ran::ran(300, 300));
  //     if (v > 0) {
  //       score += v;
  //     }
  //   }
  //   if (BESTSCORE < score) {
  //     BESTSCORE = score;
  //     memcpy(BESTNX, NX, sizeof(NX));
  //     memcpy(BESTU, U, sizeof(U));
  //   }
  // }

  memcpy(NX, BESTNX, sizeof(NX));
  memcpy(U, BESTU, sizeof(U));
  int score = BESTSCORE;

  int second_loops = 0;
  for (int i = 0; ; ++i) {
    if ((i % 500) == 0 && elapsed() > 1.9) break;
    ++second_loops;
    const int len = ran::ran(45, 55);
    int v = improve_random(len, len + 300);
    if (v > 0) {
      score += v;
    }
  }

  BESTSCORE = score;
  memcpy(BESTNX, NX, sizeof(NX));
  memcpy(BESTU, U, sizeof(U));

  // cerr<<first_loops<<' '<<second_loops<<endl;
  // cerr<<BESTSCORE<<endl;
  output(sr, sc);

  return 0;
}
