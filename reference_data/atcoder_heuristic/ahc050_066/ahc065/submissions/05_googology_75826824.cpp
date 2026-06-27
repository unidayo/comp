#include <bits/stdc++.h>

#ifndef KAMMYU_CONSTANTS
#define KAMMYU_CONSTANTS

#include <limits>

namespace kammyu
{
  namespace infinities
  {
    constexpr int inf = std::numeric_limits<int>::max();
    constexpr int _inf = std::numeric_limits<int>::min();
    constexpr long long INF = std::numeric_limits<long long>::max();
    constexpr long long _INF = std::numeric_limits<long long>::min();
  }; // namespace infinities
  namespace modulos
  {
    constexpr int Smod = 998244353;
    constexpr int Bmod = 1000000007;
  }; // namespace modulos
}; // namespace kammyu

#endif // KAMMYU_CONSTANTS

#ifndef KAMMYU_INPUT
#define KAMMYU_INPUT

#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

namespace kammyu
{
  namespace input
  {
    template <typename T1, typename T2>
    std::istream& operator>>(std::istream& is, std::pair<T1, T2>& p)
    {
      return is >> p.first >> p.second;
    }

    template <typename T>
    std::istream& operator>>(std::istream& is, std::vector<T>& v)
    {
      for (T& x : v)
        is >> x;
      return is;
    }

    template <typename T>
    void inp_arr(std::vector<T>& v)
    {
      for (T& i : v)
        std::cin >> i;
    }
    template <typename T1, typename T2>
    void inp_arr(std::vector<T1>& v1, std::vector<T2>& v2)
    {
      for (int i = 0; i < (int)v1.size(); i++)
        std::cin >> v1[i] >> v2[i];
    }
    template <typename T1, typename T2, typename T3>
    void inp_arr(std::vector<T1>& v1, std::vector<T2>& v2, std::vector<T3>& v3)
    {
      for (int i = 0; i < (int)v1.size(); i++)
        std::cin >> v1[i] >> v2[i] >> v3[i];
    }
    template <typename T1, typename T2, typename T3, typename T4>
    void inp_arr(std::vector<T1>& v1, std::vector<T2>& v2, std::vector<T3>& v3, std::vector<T4>& v4)
    {
      for (int i = 0; i < (int)v1.size(); i++)
        std::cin >> v1[i] >> v2[i] >> v3[i] >> v4[i];
    }
    template <typename T>
    void inp_arr(std::vector<std::vector<T>>& v)
    {
      for (std::vector<T>& i : v)
        for (T& j : i)
          std::cin >> j;
    }
  }; // namespace input
}; // namespace kammyu

#endif // KAMMYU_INPUT
#ifndef KAMMYU_OUTPUT
#define KAMMYU_OUTPUT

#include <iostream>
#include <ostream>
#include <vector>

namespace kammyu
{
  namespace output
  {
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<T>>& v)
    {
      for (const std::vector<T>& x : v)
        os << x << "\n";
      return os;
    }
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
    {
      for (const T& x : v)
        os << x << " ";
      return os;
    }

    void yes(bool o = false)
    {
      if (o)
        std::cout << "Yes";
      else
        std::cout << "Yes" << std::endl;
    }
    void no(bool o = false)
    {
      if (o)
        std::cout << "No";
      else
        std::cout << "No" << std::endl;
    }
    void yn(bool b, bool o = false)
    {
      if (b)
        yes(o);
      else
        no(o);
    }
  }; // namespace output
}; // namespace kammyu

#endif // KAMMYU_OUTPUT

#ifndef KAMMYU_POINT
#define KAMMYU_POINT

#include <iostream>
#include <tuple>
#include <vector>

namespace kammyu
{
  namespace point
  {
    struct P
    {
      P() { P(0, 0); }
      P(int _i, int _j) : i(_i), j(_j) {}
      int i, j;
      bool operator==(const P& p) const { return i == p.i && j == p.j; }
      bool operator!=(const P& p) const { return !(*this == p); }
      bool operator<(const P& other) const { return std::tie(i, j) < std::tie(other.i, other.j); }
      P operator+(const P& other) const { return P(i + other.i, j + other.j); }
      P operator-(const P& other) const { return P(i - other.i, j - other.j); }
      P operator*(const int& other) const { return P(i * other, j * other); }
      P operator*(const P& other) const { return P(i * other.i, j * other.j); }
      P operator*(const double& other) const { return P(i * other, j * other); }
      P operator/(const int& scaler) const { return P(i / scaler, j / scaler); }
      P operator/(const double& scaler) const { return P(i / scaler, j / scaler); }
      P operator+=(const P& p) { return *this = *this + p; }
      P operator-=(const P& p) { return *this = *this - p; }
      friend std::ostream& operator<<(std::ostream& os, const P& p) { return os << p.i << " " << p.j; }
      friend std::istream& operator>>(std::istream& is, P& p) { return is >> p.i >> p.j; }
      bool out_of_bounds(int size) const { return i < 0 || j < 0 || i >= size || j >= size; }
      bool out_of_bounds(int H, int W) const { return i < 0 || j < 0 || i >= H || j >= W; }
      P& operator--()
      {
        --i, --j;
        return *this;
      }
      P operator--(int)
      {
        P p = *this;
        --(*this);
        return p;
      }
      void swap()
      {
        std::swap(i, j);
      }
    };

    using piP = std::pair<int, P>;
    using pPP = std::pair<P, P>;
    using vP = std::vector<P>;
    using vpiP = std::vector<piP>;
    using vpPP = std::vector<pPP>;
    using vvP = std::vector<std::vector<P>>;
    using vvpiP = std::vector<std::vector<piP>>;
    const vP around4({P(0, 1), P(1, 0), P(0, -1), P(-1, 0)});
    const vP around8({P(0, 1), P(1, 1), P(1, 0), P(1, -1), P(0, -1), P(-1, -1), P(-1, 0), P(-1, 1)});
  }; // namespace point
}; // namespace kammyu

#endif // KAMMYU_POINT

#ifndef KAMMYU_UTILS
#define KAMMYU_UTILS

#include <algorithm>
#include <functional>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

#define all(v) v.begin(), v.end()
#define rall(v) v.rbegin(), v.rend()
#define rep(i, n) for (int i = 0; i < (n); ++i)
#define sign(f) (f == 0 ? 0 : (f > 0) * 2 - 1)

#define pqueue priority_queue

namespace kammyu
{

  namespace utils
  {
    template <typename T1, typename T2>
    bool chmax(T1& m, const T2& val)
    {
      if (m < val)
      {
        m = val;
        return true;
      }
      return false;
    }
    template <typename T1, typename T2>
    bool chmin(T1& m, const T2& val)
    {
      if (m > val)
      {
        m = val;
        return true;
      }
      return false;
    }

    using ll = long long;
    using pii = std::pair<int, int>;
    using piii = std::pair<int, pii>;
    using si = std::set<int>;
    using vi = std::vector<int>;
    using vpii = std::vector<pii>;
    using vpiii = std::vector<piii>;
    using vvi = std::vector<std::vector<int>>;
    using vvpii = std::vector<std::vector<pii>>;

    using vb = std::vector<bool>;
    using vvb = std::vector<vb>;
    using pli = std::pair<ll, int>;
    using plii = std::pair<pli, int>;
    using vpli = std::vector<pli>;
    using vvpli = std::vector<std::vector<pli>>;

    using pll = std::pair<ll, ll>;
    using plll = std::pair<pll, ll>;
    using vl = std::vector<ll>;
    using vpll = std::vector<pll>;
    using vvl = std::vector<std::vector<ll>>;
    using vvpll = std::vector<std::vector<pll>>;

    using vsi = std::vector<std::set<int>>;

    using vs = std::vector<std::string>;
    template <typename T>
    using vv = std::vector<std::vector<T>>;

    template <typename T>
    using min_pqueue = std::priority_queue<T, std::vector<T>, std::greater<T>>;
    template <typename T>
    using max_pqueue = std::priority_queue<T>;

    template <typename T1, typename T2>
    std::pair<T1, T2> operator+(const std::pair<T1, T2>& a, const std::pair<T1, T2>& b)
    {
      return std::make_pair(a.first + b.first, a.second + b.second);
    }
    template <typename T1, typename T2>
    std::pair<T1, T2> operator-(const std::pair<T1, T2>& a, const std::pair<T1, T2>& b)
    {
      return std::make_pair(a.first - b.first, a.second - b.second);
    }

    template <typename T>
    void sort(std::vector<T>& v)
    {
      std::sort(all(v));
    }

    vi str2vi(const std::string& s, char first_char = 'a')
    {
      vi res(s.size());
      rep(i, s.size()) res[i] = s[i] - first_char;
      return res;
    }

    template <typename T>
    T sum(const std::vector<T>& vec)
    {
      T res = T();
      for (const T& val : vec)
        res = res + val;
      return res;
    }
    template <typename T>
    T min(const std::vector<T>& vec, T init)
    {
      T res = init;
      for (const T& val : vec)
        if (val < res)
          res = val;
      return res;
    }
    template <typename T>
    T max(const std::vector<T>& vec, T init)
    {
      T res = init;
      for (const T& val : vec)
        if (val > res)
          res = val;
      return res;
    }

  }; // namespace utils
}; // namespace kammyu

#endif // KAMMYU_UTILS

using namespace kammyu::utils;
using namespace kammyu::input;
using namespace kammyu::output;
using namespace kammyu::point;

void MAIN();
int main()
{
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  MAIN();

  return 0;
}

using namespace std;
void solve();
void precalc();
void MAIN()
{
  precalc();
  int T = 1;
  // cin >> T;
  while (T--)
    solve();

  return;
}

// using namespace kammyu::infinities;
// using namespace kammyu::modulos;
void precalc()
{
  return;
}

int N;
vvi a(20, vi(20));
void input()
{
  cin >> N;
  cin >> a;
}

struct Board
{
  vvi _board;
  vvi board;
  vvP conveyor;
  vpii ans;

  vP pos;

  int nxt = 0;

  Board() : board(a), _board(a), conveyor(), pos(N * N)
  {
    rep(i, N) rep(j, N) pos[board[i][j]] = {i, j};
    remove();
  }

  bool remove()
  {
    if (board[0][N / 2] == nxt)
    {
      nxt++;
      return true;
    }
    return false;
  }
  void answer()
  {
    cout << conveyor.size() << endl;
    for (auto& con : conveyor)
      cout << con.size() << " " << con << endl;
    cout << ans.size() << endl;
    for (auto& op : ans)
      cout << op.first << " " << op.second << endl;
  }

  void move(int conv_id, int n)
  {
    if (n == 0)
      return;

    auto& con = conveyor[conv_id];

    rep(i, con.size())
    {
      P& p = con[i];
      int ni = (i + n + con.size()) % con.size();
      board[con[ni].i][con[ni].j] = _board[p.i][p.j];
    }
    for (P p : con)
      pos[board[p.i][p.j]] = p, _board[p.i][p.j] = board[p.i][p.j];

    rep(i, abs(n)) ans.push_back({conv_id, sign(n)});

    remove();
  }
};
namespace solve1
{
  void conv(Board& b)
  {
    {
      vP con;
      rep(j, N) con.push_back({0, j});
      rep(j, N) con.push_back({1, N - j - 1});
      b.conveyor.push_back(con);
    }
    for (int j = 0; j < N; j += 2)
    {
      vP con;
      for (int i = 0; i < N; i++)
        con.push_back({i, j});
      for (int i = N - 1; i >= 0; i--)
        con.push_back({i, j + 1});
      b.conveyor.push_back(con);
    }
  }

  void slv(Board& b)
  {
    while (b.nxt < N * N)
    {
      // 最上段迄持ってくる
      const P& p = b.pos[b.nxt];
      int conv = p.j / 2 + 1;
      int dir = 1;
      if (p.j % 2 == 0)
        dir = -1;
      int dist = p.i;
      b.move(conv, dir * dist);

      // ゴールに持っていく
      b.move(0, N / 2 - p.j);
      break;
    }
  }
} // namespace solve1
namespace solve1_2
{
  void conv(Board& b)
  {
    {
      vP con;
      rep(j, N) con.push_back({0, j});
      rep(j, N) con.push_back({1, N - j - 1});
      b.conveyor.push_back(con);
    }
    for (int j = 0; j < N; j += 2)
    {
      vP con;
      for (int i = 0; i < N; i++)
        con.push_back({i, j});
      for (int i = N - 1; i >= 0; i--)
        con.push_back({i, j + 1});
      b.conveyor.push_back(con);
    }
  }

  void slv(Board& b)
  {
    while (b.nxt < N * N)
    {
      // 上二列迄持ってくる
      const P& p = b.pos[b.nxt];
      int conv = p.j / 2 + 1;
      int dir = 1;
      if (p.j % 2 == 0)
        dir = -1;
      int dist = p.i;
      b.move(conv, dir * dist);

      // ゴールに持っていく
      b.move(0, N / 2 - p.j);
      break;
    }
  }
} // namespace solve1_2
namespace solve2
{
  void conv(Board& b)
  {
    for (int i = 0; i < N; i += 2)
    {
      vP con;
      for (int j = 0; j < N; j++)
        con.push_back({i, j});
      for (int j = N - 1; j >= 0; j--)
        con.push_back({i + 1, j});
      b.conveyor.push_back(con);
    }
    for (int j = 0; j < N; j += 2)
    {
      vP con;
      for (int i = 0; i < N; i++)
        con.push_back({i, j});
      for (int i = N - 1; i >= 0; i--)
        con.push_back({i, j + 1});
      b.conveyor.push_back(con);
    }
  }

  void slv(Board& b)
  {
    int nxt2 = 0; // 次に中列に持っていくべきもの
    int bot = 0;  // 最下点
    while (b.nxt < N * N)
    {
      // 中列に持ってくる
      while (nxt2 < N * N)
      {
        const P& p = b.pos[nxt2];
        int conv = p.i / 2 + 1;
        if (b.pos[nxt2].j == N / 2)
        {
          nxt2++;
          continue;
        }
        else if (b.board[p.i][N / 2] == b.nxt)
        {
          break;
        }
      }

      // ゴールに持っていく
      // b.move(0, N / 2 - p.j);
      break;
    }
  }
} // namespace solve2
namespace solve3
{
  vvi convs;
  vvb onmain;
  vvP pair_pos;
  int NN2 = 200;

  void conv(Board& b)
  {
    convs = vvi(N, vi(N, -1));
    onmain = vvb(N, vb(N, false));
    pair_pos = vvP(N, vP(N));
    {
      vP con;
      auto add = [&](P p, P e, int cnt)
      {
        rep(i, cnt)
        {
          onmain[p.i][p.j] = true;
          con.push_back(p);
          p += e;
        }
      };
      add(P(0, 1), P(0, 1), N - 2);
      add(P(1, N - 2), P(1, 0), 2);
      for (int j = N - 2; j >= 1; j -= 4)
      {
        add(P(3, j), P(1, 0), N - 3);
        add(P(N - 1, j - 1), P(-1, 0), N - 3);
        if (j - 2 >= 1)
          add(P(3, j - 2), P(0, -1), 2);
      }
      add(P(2, 1), P(-1, 0), 2);

      b.conveyor.push_back(con);
    }
    const P dps[4] = {P(0, 1), P(0, -1), P(-1, 0), P(1, 0)};
    rep(i, NN2)
    {
      P p = b.conveyor[0][i];
      for (P dp : dps)
      {
        P np = p + dp;
        if (np.out_of_bounds(N) || onmain[np.i][np.j] || convs[np.i][np.j] != -1)
          continue;
        convs[p.i][p.j] = convs[np.i][np.j] = i + 1;
        pair_pos[p.i][p.j] = np;
        pair_pos[np.i][np.j] = p;
        b.conveyor.push_back({p, np});
        break;
      }
    }
  }

  void slv(Board& b)
  {
    {
      bool f = false;
      do
      {
        f = false;
        rep(v, NN2)
        {
          P p = b.pos[v];
          if (onmain[p.i][p.j])
          {
            f = true;

            P pp = pair_pos[p.i][p.j];
            if (b.board[pp.i][pp.j] >= NN2)
              b.move(convs[p.i][p.j], 1);
          }
        }
        if (f)
          b.move(0, 1);
      } while (f);
    }
    int pre = convs[0][N / 2];
    while (b.nxt < N * N)
    {
      rep(v, NN2)
      {
        P p = b.pos[v];
        if (!onmain[p.i][p.j] && (pre - convs[p.i][p.j] + N * N) % NN2 == v)
          b.move(convs[p.i][p.j], 1);
      }
      rep(_v, NN2)
      {
        int v = NN2 + _v;
        P p = b.pos[v];
        if (!onmain[p.i][p.j] && (pre - convs[p.i][p.j] + N * N) % NN2 == v - NN2)
        {
          P pp = pair_pos[p.i][p.j];
          if (b.board[pp.i][pp.j] < b.nxt)
            b.move(convs[p.i][p.j], 1);
        }
      }
      pre++;
      b.move(0, 1);
    }
  }
} // namespace solve3
namespace solve3_2
{
  vvi convs;
  vvb onmain;
  vvP pair_pos;
  int NN2 = 200;

  void conv(Board& b)
  {
    convs = vvi(N, vi(N, -1));
    onmain = vvb(N, vb(N, false));
    pair_pos = vvP(N, vP(N));
    {
      vP con;
      auto add = [&](P p, P e, int cnt)
      {
        p.j = N - p.j - 1;
        e.j = -e.j;
        rep(i, cnt)
        {
          onmain[p.i][p.j] = true;
          con.push_back(p);
          p += e;
        }
      };
      add(P(0, 1), P(0, 1), N - 2);
      add(P(1, N - 2), P(1, 0), 2);
      for (int j = N - 2; j >= 1; j -= 4)
      {
        add(P(3, j), P(1, 0), N - 3);
        add(P(N - 1, j - 1), P(-1, 0), N - 3);
        if (j - 2 >= 1)
          add(P(3, j - 2), P(0, -1), 2);
      }
      add(P(2, 1), P(-1, 0), 2);

      b.conveyor.push_back(con);
    }
    const P dps[4] = {P(0, -1), P(0, 1), P(-1, 0), P(1, 0)};
    rep(i, NN2)
    {
      P p = b.conveyor[0][i];
      for (P dp : dps)
      {
        P np = p + dp;
        if (np.out_of_bounds(N) || onmain[np.i][np.j] || convs[np.i][np.j] != -1)
          continue;
        convs[p.i][p.j] = convs[np.i][np.j] = i + 1;
        pair_pos[p.i][p.j] = np;
        pair_pos[np.i][np.j] = p;
        b.conveyor.push_back({p, np});
        break;
      }
    }
  }

  void slv(Board& b)
  {
    {
      bool f = false;
      do
      {
        f = false;
        rep(v, NN2)
        {
          P p = b.pos[v];
          if (onmain[p.i][p.j])
          {
            f = true;

            P pp = pair_pos[p.i][p.j];
            if (b.board[pp.i][pp.j] >= NN2)
              b.move(convs[p.i][p.j], 1);
          }
        }
        if (f)
          b.move(0, 1);
      } while (f);
    }
    int pre = convs[0][N / 2];
    while (b.nxt < N * N)
    {
      rep(v, NN2)
      {
        P p = b.pos[v];
        if (!onmain[p.i][p.j] && (pre - convs[p.i][p.j] + N * N) % NN2 == v)
          b.move(convs[p.i][p.j], 1);
      }
      rep(_v, NN2)
      {
        int v = NN2 + _v;
        P p = b.pos[v];
        if (!onmain[p.i][p.j] && (pre - convs[p.i][p.j] + N * N) % NN2 == v - NN2)
        {
          P pp = pair_pos[p.i][p.j];
          if (b.board[pp.i][pp.j] < b.nxt)
            b.move(convs[p.i][p.j], 1);
        }
      }
      pre++;
      b.move(0, 1);
    }
  }
} // namespace solve3_2
void solve()
{
  input();
  Board b;
  {
    using namespace solve3;
    conv(b);
    slv(b);
  }
  {
    using namespace solve3_2;
    Board b2;
    conv(b2);
    slv(b2);

    if (b.ans.size() > b2.ans.size())
      b = b2;
  }
  b.answer();

  return;
}

