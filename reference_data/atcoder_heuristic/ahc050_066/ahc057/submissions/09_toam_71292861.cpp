#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define elif else if
#define vi vector<int>
#define vll vector<ll>
#define vvi vector<vi>
#define pii pair<int, int>

#define repname(a, b, c, d, e, ...) e
#define rep(...) repname(__VA_ARGS__, rep3, rep2, rep1, rep0)(__VA_ARGS__)
#define rep0(x) for (int rep_counter = 0; rep_counter < (x); ++rep_counter)
#define rep1(i, x) for (int i = 0; i < (x); ++i)
#define rep2(i, l, r) for (int i = (l); i < (r); ++i)
#define rep3(i, l, r, c) for (int i = (l); i < (r); i += (c))

struct ScalarInput
{
  template <class T>
  operator T()
  {
    T ret;
    cin >> ret;
    return ret;
  }
};

struct VectorInput
{
  size_t n;
  VectorInput(size_t n) : n(n) {}
  template <class T>
  operator vector<T>()
  {
    vector<T> ret(n);
    for (T &x : ret)
      cin >> x;
    return ret;
  }
};
ScalarInput input() { return ScalarInput(); }
VectorInput input(size_t n) { return VectorInput(n); }

template <typename T>
ostream &operator<<(ostream &os, const set<T> s)
{
  os << "{";
  bool first = true;
  for (const auto &x : s)
  {
    if (!first)
      os << ", ";
    os << x;
    first = false;
  }
  os << "}";
  return os;
}

template <typename T>
ostream &operator<<(ostream &os, const unordered_set<T> s)
{
  os << "{";
  bool first = true;
  for (const auto &x : s)
  {
    if (!first)
      os << ", ";
    os << x;
    first = false;
  }
  os << "}";
  return os;
}

template <typename K, typename V>
ostream &operator<<(ostream &os, const map<K, V> m)
{
  os << "{";
  bool first = true;
  for (const auto &[key, value] : m)
  {
    if (!first)
      os << ", ";
    os << key << ": " << value;
    first = false;
  }
  os << "}";
  return os;
}

template <typename K, typename V>
ostream &operator<<(ostream &os, const unordered_map<K, V> m)
{
  os << "{";
  bool first = true;
  for (const auto &[key, value] : m)
  {
    if (!first)
      os << ", ";
    os << key << ": " << value;
    first = false;
  }
  os << "}";
  return os;
}

template <typename T, size_t N>
ostream &operator<<(ostream &os, const array<T, N> a)
{
  os << "(";
  for (size_t i = 0; i < N; ++i)
  {
    if (i > 0)
      os << ", ";
    os << a[i];
  }
  os << ")";
  return os;
}

template <typename T1, typename T2>
ostream &operator<<(ostream &os, const pair<T1, T2> p)
{
  os << "(" << p.first << ", " << p.second << ")";
  return os;
}

template <typename T>
ostream &operator<<(ostream &os, const vector<T> v)
{
  os << "[";
  for (int i = 0; i < v.size(); ++i)
  {
    if (i > 0)
      os << ", ";
    os << v[i];
  }
  os << "]";
  return os;
}

template <class T>
void print(T x)
{
  cout << x << '\n';
}

template <class Head, class... Tail>
void print(Head &&head, Tail &&...tail)
{
  cout << head << ' ';
  print(forward<Tail>(tail)...);
}

double stop_watch(struct timespec start_time)
{
  // 経過時間を秒単位で返す.
  struct timespec end_time;
  clock_gettime(CLOCK_REALTIME, &end_time);
  long long int sec = end_time.tv_sec - start_time.tv_sec;
  long long int nsec = end_time.tv_nsec - start_time.tv_nsec;
  return (double)sec + (double)nsec / (1000 * 1000 * 1000);
}

/*
初期化
  struct timespec start_time;
  clock_gettime(CLOCK_REALTIME, &start_time);

時間取得
stop_watch(start_time);

*/

// -----------------------------------------------

// -----------------------------------------------------------------------------
// 高速マッチングライブラリ (Blossom Algorithm)
// -----------------------------------------------------------------------------
// ヒューリスティックコンテスト等で N が 500 を超える場合は，
// 下記の MAX_N を適切な値 (例: 1005) に変更してください．
constexpr int MAX_N = 1005;

namespace MatchingSolver
{

#define all(x) x.begin(), x.end()
  constexpr int INF = 1e9 + 100;

  struct edge
  {
    int u, v, w;
    edge() {}
    edge(int _u, int _v, int _w) : u(_u), v(_v), w(_w) {}
  };

  int n, n_x;
  edge g[MAX_N * 2][MAX_N * 2];
  int lab[MAX_N * 2];
  int match[MAX_N * 2], slack[MAX_N * 2], st[MAX_N * 2], pa[MAX_N * 2];
  int flo_from[MAX_N * 2][MAX_N + 1], S[MAX_N * 2], vis[MAX_N * 2];
  vector<int> flo[MAX_N * 2];
  queue<int> q;

  int e_delta(const edge &e) { return lab[e.u] + lab[e.v] - g[e.u][e.v].w * 2; }

  void update_slack(int u, int x)
  {
    if (!slack[x] || e_delta(g[u][x]) < e_delta(g[slack[x]][x]))
      slack[x] = u;
  }

  void set_slack(int x)
  {
    slack[x] = 0;
    for (int u = 1; u <= n; u++)
      if (g[u][x].w > 0 && st[u] != x && S[st[u]] == 0)
        update_slack(u, x);
  }

  void q_push(int x)
  {
    if (x <= n)
      q.push(x);
    else
      for (int i = 0; i < (int)flo[x].size(); i++)
        q_push(flo[x][i]);
  }

  void set_st(int x, int b)
  {
    st[x] = b;
    if (x > n)
      for (int i = 0; i < (int)flo[x].size(); i++)
        set_st(flo[x][i], b);
  }

  int get_pr(int b, int xr)
  {
    int pr = find(all(flo[b]), xr) - flo[b].begin();
    if (pr & 1)
    {
      reverse(1 + all(flo[b]));
      return (int)flo[b].size() - pr;
    }
    else
      return pr;
  }

  void set_match(int u, int v)
  {
    edge e = g[u][v];
    match[u] = g[u][v].v;
    if (u <= n)
      return;
    int xr = flo_from[u][e.u], pr = get_pr(u, xr);
    for (int i = 0; i < pr; i++)
      set_match(flo[u][i], flo[u][i ^ 1]);
    set_match(xr, v);
    rotate(flo[u].begin(), pr + all(flo[u]));
  }

  void augment(int u, int v)
  {
    while (true)
    {
      int xnv = st[match[u]];
      set_match(u, v);
      if (!xnv)
        return;
      set_match(xnv, st[pa[xnv]]);
      u = st[pa[xnv]];
      v = xnv;
    }
  }

  int get_lca(int u, int v)
  {
    static int t = 0;
    for (++t; u || v; swap(u, v))
    {
      if (u == 0)
        continue;
      if (vis[u] == t)
        return u;
      vis[u] = t;
      u = st[match[u]];
      if (u)
        u = st[pa[u]];
    }
    return 0;
  }

  void add_blossom(int u, int lca, int v)
  {
    int b = n + 1;
    while (b <= n_x && st[b])
      ++b;
    if (b > n_x)
      ++n_x;
    lab[b] = 0;
    S[b] = 0;
    match[b] = match[lca];
    flo[b] = vector<int>{lca};
    for (int x = u, y; x != lca; x = st[pa[y]])
      flo[b].push_back(x), flo[b].push_back(y = st[match[x]]), q_push(y);
    reverse(1 + all(flo[b]));
    for (int x = v, y; x != lca; x = st[pa[y]])
      flo[b].push_back(x), flo[b].push_back(y = st[match[x]]), q_push(y);
    set_st(b, b);
    for (int x = 1; x <= n_x; x++)
      g[b][x].w = g[x][b].w = 0;
    for (int x = 1; x <= n; x++)
      flo_from[b][x] = 0;
    for (int i = 0; i < (int)flo[b].size(); i++)
    {
      int xs = flo[b][i];
      for (int x = 1; x <= n_x; x++)
        if (g[b][x].w == 0 || e_delta(g[xs][x]) < e_delta(g[b][x]))
          g[b][x] = g[xs][x], g[x][b] = g[x][xs];
      for (int x = 1; x <= n; x++)
        if (flo_from[xs][x])
          flo_from[b][x] = xs;
    }
    set_slack(b);
  }

  void expand_blossom(int b)
  {
    for (int i = 0; i < (int)flo[b].size(); i++)
      set_st(flo[b][i], flo[b][i]);
    int xr = flo_from[b][g[b][pa[b]].u], pr = get_pr(b, xr);
    for (int i = 0; i < pr; i += 2)
    {
      int xs = flo[b][i], xns = flo[b][i + 1];
      pa[xs] = g[xns][xs].u;
      S[xs] = 1;
      S[xns] = 0;
      slack[xs] = 0;
      set_slack(xns);
      q_push(xns);
    }
    S[xr] = 1;
    pa[xr] = pa[b];
    for (int i = pr + 1; i < (int)flo[b].size(); i++)
      S[flo[b][i]] = -1, set_slack(flo[b][i]);
    st[b] = 0;
  }

  bool on_found_edge(const edge &e)
  {
    int u = st[e.u], v = st[e.v];
    if (S[v] == -1)
    {
      pa[v] = e.u;
      S[v] = 1;
      int nu = st[match[v]];
      slack[v] = slack[nu] = S[nu] = 0;
      S[nu] = 0;
      q_push(nu);
    }
    else if (S[v] == 0)
    {
      int lca = get_lca(u, v);
      if (!lca)
        return augment(u, v), augment(v, u), true;
      else
        add_blossom(u, lca, v);
    }
    return false;
  }

  bool matching()
  {
    memset(S + 1, -1, sizeof(int) * n_x);
    memset(slack + 1, 0, sizeof(int) * n_x);
    q = queue<int>();
    for (int x = 1; x <= n_x; x++)
      if (st[x] == x && !match[x])
        pa[x] = 0, S[x] = 0, q_push(x);
    if (q.empty())
      return false;
    while (true)
    {
      while (q.size())
      {
        int u = q.front();
        q.pop();
        if (S[st[u]] == 1)
          continue;
        for (int v = 1; v <= n; v++)
          if (g[u][v].w > 0 && st[u] != st[v])
          {
            if (e_delta(g[u][v]) == 0)
            {
              if (on_found_edge(g[u][v]))
                return true;
            }
            else
              update_slack(u, st[v]);
          }
      }
      int d = INF;
      for (int b = n + 1; b <= n_x; b++)
        if (st[b] == b && S[b] == 1)
          d = min(d, lab[b] / 2);
      for (int x = 1; x <= n_x; x++)
        if (st[x] == x && slack[x])
        {
          if (S[x] == -1)
            d = min(d, e_delta(g[slack[x]][x]));
          else if (S[x] == 0)
            d = min(d, e_delta(g[slack[x]][x]) / 2);
        }
      for (int u = 1; u <= n; u++)
      {
        if (S[st[u]] == 0)
        {
          if (lab[u] <= d)
            return 0;
          lab[u] -= d;
        }
        else if (S[st[u]] == 1)
          lab[u] += d;
      }
      for (int b = n + 1; b <= n_x; b++)
        if (st[b] == b)
        {
          if (S[st[b]] == 0)
            lab[b] += d * 2;
          else if (S[st[b]] == 1)
            lab[b] -= d * 2;
        }
      q = queue<int>();
      for (int x = 1; x <= n_x; x++)
        if (st[x] == x && slack[x] && st[slack[x]] != x && e_delta(g[slack[x]][x]) == 0)
          if (on_found_edge(g[slack[x]][x]))
            return true;
      for (int b = n + 1; b <= n_x; b++)
        if (st[b] == b && S[b] == 1 && lab[b] == 0)
          expand_blossom(b);
    }
    return false;
  }

  void solve()
  {
    memset(match + 1, 0, sizeof(int) * n);
    n_x = n;
    int w_max = 0;
    for (int u = 0; u <= n; u++)
      st[u] = u, flo[u].clear();
    for (int u = 1; u <= n; u++)
      for (int v = 1; v <= n; v++)
        flo_from[u][v] = u == v ? u : 0, w_max = max(w_max, g[u][v].w);
    for (int u = 1; u <= n; u++)
      lab[u] = w_max;
    while (matching())
      ;
  }

  void init(int _n)
  {
    n = _n;
    for (int u = 0; u <= n * 2; u++)
    {
      match[u] = 0;
      st[u] = 0;
      flo[u].clear();
    }
    for (int u = 1; u <= n; u++)
      for (int v = 1; v <= n; v++)
        g[u][v] = edge(u, v, 0);
  }

  void add_edge(int u, int v, int w)
  {
    if (w <= g[u][v].w)
      return;
    g[u][v].w = g[v][u].w = w;
  }
}

// -----------------------------------------------------------------------------
// ラッパー関数
// -----------------------------------------------------------------------------

/**
 * N頂点の最小重み完全マッチングを計算する (int版)
 * * 最小化問題を最大化問題に変換して解きます。
 * 変換式: weight = OFFSET - A[i][j]
 * * @param n 頂点数 (偶数)
 * @param A コスト行列 (NxN, 対称行列, 各要素は 10^6 以下)
 * @return マッチングされた頂点のペアのリスト (0-indexed)
 */
vector<pair<int, int>> calc_matching(int n, vector<vector<int>> A)
{
  if (n == 0)
    return {};

  // 1. ソルバの初期化
  MatchingSolver::init(n);

  // 2. 重みの変換パラメータ設定
  // コストの最大値が10^6なので、OFFSETはそれより大きい 2*10^6 程度で十分です。
  // 内部計算で加算が行われても int (約2*10^9) に十分収まります。
  const int OFFSET = 2000000;

  // 3. 辺の追加
  // 完全グラフとして全ての辺を追加します
  for (int i = 0; i < n; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      int cost = A[i][j];

      // コストが小さいほど重みが大きくなるように変換
      // weight が正であることを保証します (OFFSET > 10^6)
      int weight = OFFSET - cost;

      // Solverは1-indexed
      MatchingSolver::add_edge(i + 1, j + 1, weight);
    }
  }

  // 4. マッチング計算実行
  MatchingSolver::solve();

  // 5. 結果の取り出し
  vector<pair<int, int>> result;
  for (int i = 1; i <= n; i++)
  {
    int m = MatchingSolver::match[i];
    // 重複防止のため i < m の組み合わせのみ採用
    if (m > i)
    {
      result.push_back({i - 1, m - 1});
    }
  }

  return result;
}

// ---------------------------------------------------------

const int N = 300;
const int T = 1000;
const int M = 10;
const int K = 30;
const double L = 100000;

struct Molecule
{
  vector<pair<int, pair<double, double>>> pos;
  double vx, vy;
  int last_merge_time;
};

vector<Molecule> init_data;

// A, B を時刻 [t_start, t_end-1] のどこかで結合したときの
// 1. 最小距離コスト best_cost
// 2. そのときの時刻 best_t
// 3. どの頂点同士か (best_u: A側, best_v: B側)
// 4. 距離 + 速度ペナルティを加味したコスト combined_cost
array<int, 5> calc_best_cost_over_time(const Molecule &A, const Molecule &B,
                                       int t_end, double lambda)
{
  // 探索開始時刻（両方の last_merge_time 以降）
  int t_start = max(A.last_merge_time, B.last_merge_time);

  // 探索区間が存在しない場合の保険
  if (t_start >= t_end)
  {
    return {INT_MAX, -1, -1, -1, INT_MAX};
  }

  // 距離二乗の最小値で管理
  double best_d2 = 1e100;
  int best_t = -1;
  int best_u = -1, best_v = -1;

  const int na = (int)A.pos.size();
  const int nb = (int)B.pos.size();

  // 時刻ごとの位置を一時的に保持するバッファ
  static vector<double> ax, ay, bx, by;
  ax.resize(na);
  ay.resize(na);
  bx.resize(nb);
  by.resize(nb);

  for (int t = t_start; t < t_end; ++t)
  {
    int dtA = t - A.last_merge_time;
    int dtB = t - B.last_merge_time;

    // 時刻 t における A 側の全点の位置
    for (int i = 0; i < na; ++i)
    {
      double x = A.pos[i].second.first + A.vx * dtA;
      double y = A.pos[i].second.second + A.vy * dtA;

      // 速度・T の制約上、±L 一回で十分正規化できる
      if (x >= L)
        x -= L;
      else if (x < 0)
        x += L;
      if (y >= L)
        y -= L;
      else if (y < 0)
        y += L;

      ax[i] = x;
      ay[i] = y;
    }

    // 時刻 t における B 側の全点の位置
    for (int j = 0; j < nb; ++j)
    {
      double x = B.pos[j].second.first + B.vx * dtB;
      double y = B.pos[j].second.second + B.vy * dtB;

      if (x >= L)
        x -= L;
      else if (x < 0)
        x += L;
      if (y >= L)
        y -= L;
      else if (y < 0)
        y += L;

      bx[j] = x;
      by[j] = y;
    }

    // 全ペアのトーラス距離²を計算し、最小を更新
    for (int i = 0; i < na; ++i)
    {
      for (int j = 0; j < nb; ++j)
      {
        double dx = fabs(ax[i] - bx[j]);
        double dy = fabs(ay[i] - by[j]);
        dx = min(dx, L - dx);
        dy = min(dy, L - dy);

        double d2 = dx * dx + dy * dy;

        if (d2 < best_d2)
        {
          best_d2 = d2;
          best_t = t;
          best_u = A.pos[i].first;
          best_v = B.pos[j].first;
        }
      }
    }
  }

  int best_cost;
  if (best_t == -1)
  {
    // 探索区間が空などで見つからなかった場合
    best_cost = INT_MAX;
    return {best_cost, best_t, best_u, best_v, INT_MAX};
  }
  else
  {
    double dist = sqrt(best_d2);
    best_cost = static_cast<int>(llround(dist));
  }

  // ここから速度ロスペナルティを計算
  double vxA = A.vx, vyA = A.vy;
  double vxB = B.vx, vyB = B.vy;

  double spA = hypot(vxA, vyA);
  double spB = hypot(vxB, vyB);

  int sizeA = (int)A.pos.size();
  int sizeB = (int)B.pos.size();
  int sizeAll = sizeA + sizeB;

  double vx_new = (sizeA * vxA + sizeB * vxB) / sizeAll;
  double vy_new = (sizeA * vyA + sizeB * vyB) / sizeAll;
  double spNew = hypot(vx_new, vy_new);

  double loss = (spA + spB) - spNew;
  if (loss < 0.0)
    loss = 0.0; // 数値誤差対策

  int vel_penalty = (int)llround(lambda * loss);

  // ここから時間ペナルティを計算（遅いほどペナルティ大）
  // 例: best_t が t_start からどれだけ遅れているかを線形で足す
  double time_weight = 0.001 * lambda; // ←ここもチューニング用
  int time_penalty = (int)llround(time_weight * (best_t));
  if (time_penalty < 0) time_penalty = 0;

  int combined_cost = (best_cost + vel_penalty + time_penalty);

  // [距離コスト, 時刻, u, v, 距離+速度+時間ペナルティ]
  return {best_cost, best_t, best_u, best_v, combined_cost};
}


Molecule merge_molecule(const Molecule &A, const Molecule &B, int t)
{
  // A, B のサイズ（質量は頂点数に比例とみなす）
  int sizeA = (int)A.pos.size();
  int sizeB = (int)B.pos.size();
  int sizeAll = sizeA + sizeB;

  // 運動量保存則による新しい速度
  double vx_new = (sizeA * A.vx + sizeB * B.vx) / sizeAll;
  double vy_new = (sizeA * A.vy + sizeB * B.vy) / sizeAll;

  // A/B の基準時刻から t までの経過時間
  int dtA = t - A.last_merge_time;
  int dtB = t - B.last_merge_time;

  Molecule C;
  C.vx = vx_new;
  C.vy = vy_new;
  C.last_merge_time = t;
  C.pos.reserve(sizeAll);

  auto advance = [&](double x0, double y0, double vx_old, double vy_old, int dt)
  {
    double x = x0 + vx_old * dt;
    double y = y0 + vy_old * dt;

    // トーラスに正規化（この問題の制約なら ±L 一回で十分）
    if (x >= L)
      x -= L;
    else if (x < 0)
      x += L;
    if (y >= L)
      y -= L;
    else if (y < 0)
      y += L;

    return make_pair(x, y);
  };

  // A 側の頂点を追加（時刻 t に進める）
  for (const auto &pa : A.pos)
  {
    int id = pa.first;
    double x0 = pa.second.first;
    double y0 = pa.second.second;

    auto [x, y] = advance(x0, y0, A.vx, A.vy, dtA);
    C.pos.push_back({id, {x, y}});
  }

  // B 側の頂点を追加（時刻 t に進める）
  for (const auto &pb : B.pos)
  {
    int id = pb.first;
    double x0 = pb.second.first;
    double y0 = pb.second.second;

    auto [x, y] = advance(x0, y0, B.vx, B.vy, dtB);
    C.pos.push_back({id, {x, y}});
  }

  return C;
}

void setup()
{
  int n, t, m, k, l;
  cin >> n >> t >> m >> k >> l;
  rep(i, N)
  {
    int x, y, vx, vy;
    cin >> x >> y >> vx >> vy;
    Molecule mol;
    mol.pos.push_back({i, {x, y}});
    mol.vx = vx;
    mol.vy = vy;
    mol.last_merge_time = 0;
    init_data.push_back(mol);
  }
}

int score(int d_sum)
{
  // L は double、N, M は int なので long double に揃える
  long double numerator = (long double)L * (long double)(N - M);
  long double denominator = (long double)d_sum + 1.0L;
  long double ratio = numerator / denominator;

  // 問題文の式: round( 1e6 * log2( L*(N-M) / (D_sum+1) ) )
  long double val = 1'000'000.0L * log2l(ratio);

  // 四捨五入して int に
  long long s = llroundl(val);
  return (int)s;
}

/*
1. 1 x 30 -> (1 x 2 + 2 x 14)
2. (1 x 2 + 2 x 14) -> (4 x 6 + 3 x 2)
3. (4 x 6 + 3 x 2) -> (7 x 2 + 8 x 2)
4. (7 x 2 + 8 x 2) -> 15 x 2
5. 15 x 2 -> 30 x 1
*/

int inf = 1000000;

vector<double> lambdas;
struct timespec start_time;

pair<int, vector<array<int, 3>>> calc(vector<int> time_stamp)
{
  ll d_sum = 0;
  vector<array<int, 3>> ans;

  vector<Molecule> prev_res = init_data;
  rep(turn, 5)
  {
    if (stop_watch(start_time) > 1.93)
    {
      return {inf, {}};
    }
    vector<Molecule> next_res;
    int sz = prev_res.size();
    int dummy = 0;
    vector<pair<int, int>> ng_size;

    if (turn == 0)
    {
      dummy = 20;
    }
    if (turn == 1)
    {
      ng_size.push_back({1, 1});
    }
    if (turn == 2)
    {
      ng_size.push_back({3, 3});
    }
    if (turn == 3)
    {
      ng_size.push_back({7, 7});
      ng_size.push_back({8, 8});
    }
    int n = sz + dummy;

    vector<vector<array<int, 5>>> memo(sz, vector<array<int, 5>>(sz));
    vvi A(n, vi(n));
    rep(i, sz)
    {
      rep(j, i + 1, sz)
      {
        bool flag = true;
        for (auto [s1, s2] : ng_size)
        {
          if (s1 == prev_res[i].pos.size() && s2 == prev_res[j].pos.size())
          {
            flag = false;
          }
          if (s1 == prev_res[j].pos.size() && s2 == prev_res[i].pos.size())
          {
            flag = false;
          }
        }
        if (flag)
        {
          array<int, 5> cost = calc_best_cost_over_time(prev_res[i], prev_res[j], time_stamp[turn + 1], lambdas[turn]);
          memo[i][j] = cost;
          A[i][j] = cost[4];
          A[j][i] = cost[4];
        }
        else
        {
          A[i][j] = inf;
          A[j][i] = inf;
        }
      }
    }
    rep(i, n)
    {
      A[i][i] = inf;
    }
    rep(i, n)
    {
      rep(j, sz, n)
      {
        A[i][j] = 0;
        A[j][i] = 0;
      }
    }
    rep(i, sz, n)
    {
      rep(j, sz, n)
      {
        A[i][j] = inf;
      }
    }

    vector<pair<int, int>> match = calc_matching(n, A);
    for (auto [x, y] : match)
    {
      assert(A[x][y] != inf);
      if (x < sz && y < sz)
      {
        d_sum += memo[x][y][0];
        ans.push_back({memo[x][y][1], memo[x][y][2], memo[x][y][3]});
        next_res.push_back(merge_molecule(prev_res[x], prev_res[y], memo[x][y][1]));
      }
      else
      {
        next_res.push_back(prev_res[x]);
      }
    }
    prev_res = next_res;
  }
  cerr << score(d_sum) << ":" << time_stamp << endl;
  return {d_sum, ans};
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout << fixed << setprecision(20);

  setup();
  clock_gettime(CLOCK_REALTIME, &start_time);
  vector<array<int, 3>> ans;

  vector<int> init_time_stamp = {0, 400, 700, 800, 900, 1000};
  lambdas = {80, 40, 0, 0, 0};
  int best;
  {
    auto res = calc(init_time_stamp);
    best = res.first;
    ans = res.second;
  }

  vector<int> bits;
  rep(bit, 16)
  {
    bits.push_back(bit);
  }
  mt19937 rng(0);
  shuffle(bits.begin(), bits.end(), rng);

  for (auto bit : bits)
  {
    if (stop_watch(start_time) > 1.78)
      break;

    vi time_stamp = init_time_stamp;
    for (int i = 0; i < 4; i++)
    {
      if ((bit >> i) & 1)
      {
        time_stamp[4 - i] *= 1.05;
      }
      else
      {
        time_stamp[4 - i] *= 0.95;
      }
    }
    auto res = calc(time_stamp);
    if (res.first < best)
    {
      best = res.first;
      ans = res.second;
    }
  }

  for (auto [t, u, v] : ans)
  {
    print(t, u, v);
  }
}