#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
using namespace std;
using lint = long long int;
using P = pair<int, int>;
using PL = pair<lint, lint>;
#define FOR(i, begin, end) for(int i=(begin),i##_end_=(end);i<i##_end_;i++)
#define IFOR(i, begin, end) for(int i=(end)-1,i##_begin_=(begin);i>=i##_begin_;i--)
#define REP(i, n) FOR(i,0,n)
#define IREP(i, n) IFOR(i,0,n)
#define ALL(a)  (a).begin(),(a).end()
constexpr int MOD = 1000000007;
constexpr lint B1 = 1532834020;
constexpr lint M1 = 2147482409;
constexpr lint B2 = 1388622299;
constexpr lint M2 = 2147478017;
constexpr int INF = 2147483647;
void yes(bool expr) {cout << (expr ? "Yes" : "No") << "\n";}
template<class T>void chmax(T &a, const T &b) { if (a<b) a=b; }
template<class T>void chmin(T &a, const T &b) { if (b<a) a=b; }
#include <atcoder/all>
using namespace atcoder;
using mint = modint998244353;
vector<vector<vector<int>>> s;
int BEAM_WIDTH = 32;

struct State;
vector<State> states;
struct State {
  int id;
  int last = -1;
  int rest = 75;
  lint score = 0;
  mint a[81];
  int sk = -1, sx = -1, sy = -1;
  bool operator < (const State &other) const { return score > other.score; }
  State() {}
  State(int last, mint a[81], int sk, int sx, int sy) : last(last), sk(sk), sx(sx), sy(sy) {
    REP(i, 81) this->a[i] = a[i];
    if(last != -1) rest = states[last].rest;
    if(sk != -1) {
      REP(ii, 3) REP(jj, 3) this->a[(sx+ii)*9+sy+jj] += s[sk][ii][jj];
      rest--;
    }
    id = states.size();
    states.push_back(*this);
  }
  void calc_score(vector<P> &score_masu) {
    score = 0;
    for(auto p : score_masu) score += a[p.first*9+p.second].val();
    score += min(76-(int)score_masu.size(), rest) * 40000000;
    if(rest < 0) score = -1e9;
  }
};

constexpr int TIME_LIMIT = 1900;

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  int N, M, K;
  cin >> N >> M >> K;
  vector<vector<int>> aa(N, vector<int>(N));
  REP(i, N) REP(j, N) cin >> aa[i][j];
  mint a[81];
  REP(i, N) REP(j, N) a[i*9+j] = aa[i][j];
  s.resize(M, vector<vector<int>>(3, vector<int>(3)));
  REP(i, M) REP(j, 3) REP(k, 3) cin >> s[i][j][k];

  auto start_time = chrono::system_clock::now();

  mt19937 mt(0);

  vector<P> order;
  REP(i, N-2) REP(j, N-2) order.push_back(P(i, j));

  //端以外はビームサーチで決め、端は全探索or焼きなましをする
  vector<vector<State>> beam(order.size()+1);
  State init(-1, a, -1, -1, -1);
  beam[0].push_back(init);

  int ptr = 0;
  for(auto p : order) {
    cerr << ptr << endl;
    int i = p.first;
    int j = p.second;

    sort(ALL(beam[ptr]));
    if(beam[ptr].size() > BEAM_WIDTH) beam[ptr].resize(BEAM_WIDTH);

    vector<P> score_masu;
    int ii = i;
    int jj = j;
    if(j == N-3) jj += 2;
    while(ii >= 0) {
      score_masu.push_back(P(ii, jj));
      jj--;
      if(jj < 0) {
        jj = N-1;
        ii--;
      }
    }
    if(i == N-3) {
      FOR(ii, i+1, i+3) REP(jj, j+1) score_masu.push_back(P(ii, jj));
    }

    for(auto &state : beam[ptr]) {
      State n0(state.id, state.a, -1, -1, -1);
      n0.calc_score(score_masu);
      beam[ptr+1].push_back(n0);
      REP(k1, M) {
        State n1(state.id, state.a, k1, i, j);
        n1.calc_score(score_masu);
        beam[ptr+1].push_back(n1);
        FOR(k2, k1, M) {
          State n2(n1.id, n1.a, k2, i, j);
          n2.calc_score(score_masu);
          beam[ptr+1].push_back(n2);
          if(i == N-3 || j == N-3) {
            FOR(k3, k2, M) {
              State n3(n2.id, n2.a, k3, i, j);
              n3.calc_score(score_masu);
              beam[ptr+1].push_back(n3);
            }
          }
        }
      }
    }

    ptr++;
  }
  sort(ALL(beam[ptr]));

  //最終状態を復元する
  lint best_score = 0;
  vector<int> best_sk;
  vector<int> best_x;
  vector<int> best_y;
  REP(z, 200) {
    State state = beam[ptr][z];
    int sn = 0;
    vector<int> sk;
    vector<int> x, y;
    while(state.last != -1) {
      if(state.sk != -1) {
        sk.push_back(state.sk);
        x.push_back(state.sx);
        y.push_back(state.sy);
        sn++;
      }
      state = states[state.last];
    }
    REP(i, 81) a[i] = beam[ptr][z].a[i];
    reverse(ALL(sk));
    reverse(ALL(x));
    reverse(ALL(y));

    //端を貪欲で決めていく
    REP(i, N-2) REP(j, N-2) {
      if(i != N-3 || j != N-3) continue;
      vector<P> score_masu;
      int li = i;
      int lj = j;
      int ri = i;
      int rj = j;
      if(i == N-3) ri += 2;
      if(j == N-3) rj += 2;
      FOR(ii, li, ri+1) FOR(jj, lj, rj+1) score_masu.push_back(P(ii, jj));
      lint best_score_sub = 0;
      for(auto p : score_masu) best_score_sub += a[p.first*9+p.second].val();
      int nk1 = -1;
      int nk2 = -1;
      int nk3 = -1;
      int nk4 = -1;
      int nk5 = -1;
      int nk6 = -1;
      REP(k1, M) {
        REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[k1][ii][jj];
        lint score = 0;
        for(auto p : score_masu) score += a[p.first*9+p.second].val();
        if(score > best_score_sub) {
          best_score_sub = score;
          nk1 = k1;
          nk2 = -1;
          nk3 = -1;
          nk4 = -1;
          nk5 = -1;
          nk6 = -1;
        }
        if(i == N-3 || j == N-3) {
          FOR(k2, k1, M) {
            REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[k2][ii][jj];
            lint score = 0;
            for(auto p : score_masu) score += a[p.first*9+p.second].val();
            if(score > best_score_sub) {
              best_score_sub = score;
              nk1 = k1;
              nk2 = k2;
              nk3 = -1;
              nk4 = -1;
              nk5 = -1;
              nk6 = -1;
            }
            FOR(k3, k2, M) {
              REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[k3][ii][jj];
              lint score = 0;
              for(auto p : score_masu) score += a[p.first*9+p.second].val();
              if(score > best_score_sub) {
                best_score_sub = score;
                nk1 = k1;
                nk2 = k2;
                nk3 = k3;
                nk4 = -1;
                nk5 = -1;
                nk6 = -1;
              }
              if(i == N-3 && j == N-3) {
                FOR(k4, k3, M) {
                  REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[k4][ii][jj];
                  lint score = 0;
                  for(auto p : score_masu) score += a[p.first*9+p.second].val();
                  if(score > best_score_sub) {
                    best_score_sub = score;
                    nk1 = k1;
                    nk2 = k2;
                    nk3 = k3;
                    nk4 = k4;
                    nk5 = -1;
                    nk6 = -1;
                  }
                  FOR(k5, k4, M) {
                    REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[k5][ii][jj];
                    lint score = 0;
                    for(auto p : score_masu) score += a[p.first*9+p.second].val();
                    if(score > best_score_sub) {
                      best_score_sub = score;
                      nk1 = k1;
                      nk2 = k2;
                      nk3 = k3;
                      nk4 = k4;
                      nk5 = k5;
                      nk6 = -1;
                    }
                    FOR(k6, k5, M) {
                      REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[k6][ii][jj];
                      lint score = 0;
                      for(auto p : score_masu) score += a[p.first*9+p.second].val();
                      if(score > best_score_sub) {
                        best_score_sub = score;
                        nk1 = k1;
                        nk2 = k2;
                        nk3 = k3;
                        nk4 = k4;
                        nk5 = k5;
                        nk6 = k6;
                      }
                      REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] -= s[k6][ii][jj];
                    }
                    REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] -= s[k5][ii][jj];
                  }
                  REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] -= s[k4][ii][jj];
                }
              }
              REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] -= s[k3][ii][jj];
            }
            REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] -= s[k2][ii][jj];
          }
        }
        REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] -= s[k1][ii][jj];
      }
      if(nk1 != -1 && sn < K) {
        REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[nk1][ii][jj];
        sn++;
        sk.push_back(nk1);
        x.push_back(i);
        y.push_back(j);
        if(nk2 != -1 && sn < K) {
          REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[nk2][ii][jj];
          sn++;
          sk.push_back(nk2);
          x.push_back(i);
          y.push_back(j);
          if(nk3 != -1 && sn < K) {
            REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[nk3][ii][jj];
            sn++;
            sk.push_back(nk3);
            x.push_back(i);
            y.push_back(j);
            if(nk4 != -1 && sn < K) {
              REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[nk4][ii][jj];
              sn++;
              sk.push_back(nk4);
              x.push_back(i);
              y.push_back(j);
              if(nk5 != -1 && sn < K) {
                REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[nk5][ii][jj];
                sn++;
                sk.push_back(nk5);
                x.push_back(i);
                y.push_back(j);
                if(nk6 != -1 && sn < K) {
                  REP(ii, 3) REP(jj, 3) a[(i+ii)*9+j+jj] += s[nk6][ii][jj];
                  sn++;
                  sk.push_back(nk6);
                  x.push_back(i);
                  y.push_back(j);
                }
              }
            }
          }
        }
      }
    }
    lint score = 0;
    REP(i, N) REP(j, N) score += a[i*9+j].val();
    //cerr << z << " " <<  score << endl;
    if(score > best_score) {
      best_score = score;
      best_sk = sk;
      best_x = x;
      best_y = y;
    }
  }

  int sn = best_sk.size();
  cout << sn << endl;
  REP(i, sn) cout << best_sk[i] << " " << best_x[i] << " " << best_y[i] << endl;

  cerr << best_score << endl;
}