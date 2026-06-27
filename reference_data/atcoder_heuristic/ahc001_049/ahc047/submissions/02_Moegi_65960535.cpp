// #define _GLIBCXX_DEBUG
// #pragma GCC optimize("O3")
#pragma GCC optimize("O2")
// #pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using namespace atcoder;
using ll = long long;
#define rep(i,n) for (ll i = 0; i < (n); ++i)
using vl = vector<ll>;
using vvl = vector<vl>;
using P = pair<ll,ll>;
#define pb push_back
// #define int long long
// #define double long double
#define INF (ll) 3e18
// Ctrl + Shift + B コンパイル
// Ctrl + C 中断
// ./m 実行


int N=36; // 文字列の数
int M=12; // モデルの状態数 = 12
int L=1000000; // 文字列の長さ = 1000000

vector<string> s(36);
vl p(36);

string c = "aabbccddeeff";
vvl a(12, vl(12, 0));

void input(){
  cin >> N >> M >> L;
  rep(i,N) cin >> s[i] >> p[i];
  rep(i,12) rep(j,12) a[i][j] = 8;
  rep(i,12) a[i][0] = 12;
}

// 焼きなまし法のテンプレート
constexpr int TIME_LIMIT = 1960;
auto start_time = std::chrono::system_clock::now();
auto now_time = std::chrono::system_clock::now();
double progress = std::chrono::duration_cast<std::chrono::milliseconds>(now_time-start_time).count();
float start_temp = 0.99;
float end_temp = 0.0;

unsigned int randxor(){
    static unsigned int x=123456789,y=362436069,z=521288629,w=88675123;
    unsigned int t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

/*============================================================
  汎用バックアップ
============================================================*/
vvl pre_a;
string pre_c;

inline void Backup() {
  pre_a = a;
  pre_c = c;
}

inline void Rollback() {
  a = pre_a;
  c = pre_c;
  // InvalidateStatesByChar();  // 文字を書き換えた場合だけ呼ぶ
}

inline int current_lb() {
  // return 0;
  // 最初は下限を設けて、急速に下限を撤廃していくイメージ
  return std::max(0, 8 - int(32.0 * progress / TIME_LIMIT));
}

void ToNeighbor1() {
  Backup();

  const int lb = current_lb();      // ★ 今時点の最小値 [0–8]

  int i    = randxor() % M;         // 行
  int from = randxor() % M;
  int to   = randxor() % M;
  if (from == to) {
    ToNeighbor1();
    return;
  }

  /* ― from セルを lb 未満にしない範囲で移動 ― */
  int max_take = a[i][from] - lb;   // 取れる上限
  if (max_take <= 0) {
    ToNeighbor1();
    return;
  }

  int delta = 1 + randxor() % max_take;
  a[i][from] -= delta;
  a[i][to]   += delta;
}

// void ToNeighbor2() {
//   Backup();
//   int i = randxor() % 12;
//   char x = 'a' + randxor() % 6;
//   c[i] = x;
// }

bool ValidCheck(){
  // 現在参照中の解がValidであるかを確かめ、結果をboolで返す
  return true;
}

// ------------------------------------------------------------
//  speedy exact score()         置き換え用
// ------------------------------------------------------------
double score() {
  /* ---------- 0) 行列 P_flat を更新 ---------- */
  static std::vector<double> P_flat;               // 長さ M*M
  P_flat.resize((size_t)M * M);

  for (int i = 0; i < M; ++i) {
    const vl& row = a[i];
    double* dst  = &P_flat[(size_t)i * M];
    for (int j = 0; j < M; ++j) dst[j] = row[j] * 0.01;   // [%]→[0-1]
  }

  /* ---------- 1) stationay π (収束判定付き) ---------- */
  static std::vector<double> pi(M, 1.0 / M), nxt(M);
  int iter = 0;
  while (true) {
    std::fill(nxt.begin(), nxt.end(), 0.0);
    for (int i = 0; i < M; ++i) {
      double v = pi[i];
      if (v == 0.0) continue;
      const double* row = &P_flat[(size_t)i * M];
      for (int j = 0; j < M; ++j) nxt[j] += v * row[j];
    }
    double diff = 0.0;
    for (int j = 0; j < M; ++j) diff = std::max(diff, std::abs(nxt[j] - pi[j]));
    pi.swap(nxt);
    if (diff < 1e-12 || ++iter == 10) break;
  }

  /* ---------- 2) 文字→状態の逆引きテーブル ---------- */
  static std::vector<int> states_by_char[26];
  if (states_by_char[0].empty()) {
    for (int v = 0; v < M; ++v)
      states_by_char[c[v] - 'a'].push_back(v);
  }

  /* ---------- 3) DP バッファ ---------- */
  static std::vector<double> cur(M), nxt_dp(M);
  static std::vector<int>    active;

  /* ---------- 4) 文字列ごとの計算 ---------- */
  long long THRESH_P = 10;         // p[i] < 2000 は無視
  double t = (double)progress / TIME_LIMIT;
  if(t > 0.5) THRESH_P = 0; // 半分以上経ったら生スコア
  // double GAMMA = 0.001 + 0.999 * t * t * t;   // 3次曲線
  // double GAMMA = 0.001 + 0.999 * t * t;   // 2次曲線
  double GAMMA = 0.001 + 0.999 * pow(t, 1.5);   // X次曲線
  // double GAMMA = 0.001 + 0.999 * t;   // 0.3 → 1.0 を線形補間

  double total = 0.0;

  for (int idx = 0; idx < N; ++idx) {
    if (p[idx] < THRESH_P) continue;           // ★ 低スコア文字列をスキップ

    const std::string& str = s[idx];
    const int len = (int)str.size();

    /* 初期化 – 先頭文字一致のみ π を入れる */
    std::fill(cur.begin(), cur.end(), 0.0);
    active.clear();
    for (int u : states_by_char[str[0] - 'a']) {
      cur[u] = pi[u];
      if (cur[u] != 0.0) active.push_back(u);
    }

    /* 文字列の残りを走査 */
    for (int pos = 1; pos < len; ++pos) {
      const auto& dests = states_by_char[str[pos] - 'a'];
      std::fill(nxt_dp.begin(), nxt_dp.end(), 0.0);

      for (int u : active) {
        double cu = cur[u];
        const double* row = &P_flat[(size_t)u * M];
        for (int v : dests) nxt_dp[v] += cu * row[v];
      }

      active.clear();
      for (int v : dests)
        if (nxt_dp[v] != 0.0) active.push_back(v);
      cur.swap(nxt_dp);
    }

    /* p_i = Σ cur */
    double p_i = 0.0;
    for (int u : active) p_i += cur[u];

    /* ポアソン近似で Q_i */
    double E = p_i * (L - len + 1);
    double Q = (E < 1e-9) ? E : (1.0 - std::exp(-E));

    /* ★ 緩やかな勾配係数を適用 */
    double W = std::pow(Q, GAMMA);             // Q=0.1→0.5, 0.01→0.25 など
    total += p[idx] * W;
  }
  return total + 1.0;                          // 0 で割り切れ防止
}

void output(){
  // 回答を出力する
  rep(i, 12){
    cout << c[i] << " ";
    for(auto x : a[i]){
      cout << x << " ";
    }
    cout << endl;
  }
  cerr << "評価 : " << score() << endl;
  // スコアは遷移の最終状態であって、最高時のスコアではない
  now_time = std::chrono::system_clock::now();
  progress = std::chrono::duration_cast<std::chrono::milliseconds>(now_time-start_time).count();
  cerr << "経過ms : " << progress << endl;
}

void Annealing(){
  start_time = std::chrono::system_clock::now();
  now_time = std::chrono::system_clock::now();
  long long loop = 0, move = 0;
  double pre_score = score();

  /* 100 ms 単位で出力するしきい値                   */
  long long next_dump_ms = 100;                 // 0.10 s, 0.20 s, …

  int rollback_row = 0;
  while (true) {
    /*--- 100 ループごとに時計を進める --------------*/
    if ((loop & 127) == 0) {                    // 128 ループごとでも十分に細かい
      now_time = std::chrono::system_clock::now();
      progress = std::chrono::duration_cast<
                   std::chrono::milliseconds>(now_time - start_time).count();
      pre_score = score();
      /* ★ 100 ms 経過するたびにデバッグ出力 --------*/
      if (progress >= next_dump_ms) {
        output();                               // 現在解を標準出力へ
        next_dump_ms += 100;                    // 次のしきい値へ
      }

      /* 制限時間チェック ---------------------------*/
      if (progress > TIME_LIMIT) break;
    }
    ++loop;
    double old_score = pre_score;                 // (高速なら best_score を使う手も)
    ToNeighbor1();                              // いまは 1 種だけ
    // 遷移を増やすのは微妙な気がする…有効ではない気がする…
    // むしろ評価関数を弄って、初期の可能性が極端に低い文字列を高く評価するほうが重要そう
    double new_score = score();
    double co = 10000.0;
    double ratio = new_score / old_score;
    double temp = start_temp +
                  (end_temp - start_temp) * progress / TIME_LIMIT;
    double prob = std::exp((ratio - 1) * co * std::pow(0.1, temp));

    if (ratio >= 1.0 ||
        prob >= (randxor() / static_cast<double>(UINT_MAX))) {
      ++move;                                   // 遷移を受理
      pre_score = new_score;
      rollback_row = 0;
    } else {
      Rollback();
      rollback_row++;
      // 50回連続でロールバックしたら、時間が経ってるので再計算
      // if(rollback_row > 50) pre_score = score();
      // else pre_score = old_score;
    }
  }
  output();
  std::cerr << "ループ回数   : " << loop  << '\n';
  std::cerr << "有効操作回数 : " << move  << '\n';
}

signed main(){
  input();
  Annealing();
  output();
}