#if __INCLUDE_LEVEL__ == 0

#include <bits/stdc++.h>

using namespace std;

#include __BASE_FILE__

namespace {

constexpr int n = 30;
constexpr int m = n * (n + 1) / 2;

template <class T>
using Array2D = array<array<T, n>, n>;

Array2D<int> b_input;

void read_input() {
  for (int x : rep(n)) {
    for (int y : rep1(0, x)) {
      cin >> b_input[x][y];
    }
  }
}

template <class F>
void for_each_adj(int x, int y, F f) {
  if (y) {
    f(x, y - 1);
  }
  if (y < x) {
    f(x, y + 1);
  }
  if (x + 1 < n) {
    f(x + 1, y);
  }
  if (x + 1 < n) {
    f(x + 1, y + 1);
  }
}

pair<int, vector<array<int, 4>>> eval(double A, double B,
                                      bool make_ans = false) {
  Array2D<int> indeg{};
  for (int x : rep(n - 1)) {
    for (int y : rep1(0, x)) {
      ++indeg[x + 1][y];
      ++indeg[x + 1][y + 1];
    }
  }

  auto b = b_input;
  int k = 0;
  vector<array<int, 4>> ans;
  auto go = [&](int x, int y, int x2, int y2) {
    swap(b[x][y], b[x2][y2]);
    ++k;
    if (make_ans) {
      ans.push_back({x, y, x2, y2});
    }
  };
  for (int v : rep(m)) {
    Array2D<double> d;
    Array2D<array<int, 2>> prv;
    priority_queue q{greater{}, vector<tuple<double, int, int>>{}};
    for (int x : rep(n)) {
      d[x].fill(numeric_limits<double>::max());
      prv[x].fill({-1, -1});
      for (int y : rep1(0, x)) {
        if (v <= b[x][y] && indeg[x][y] == 0) {
          q.emplace(d[x][y] = 0, x, y);
        }
      }
    }
    for (; !q.empty(); q.pop()) {
      auto [dxy, x, y] = q.top();
      if (dxy != d[x][y]) {
        continue;
      }
      if (b[x][y] == v) {
        int i = x, j = y;
        while (true) {
          auto [pi, pj] = prv[i][j];
          if (pi == -1) {
            break;
          }
          go(i, j, pi, pj);
          i = pi, j = pj;
        }
        assert(b[i][j] == v);
        assert(indeg[i][j] == 0);
        if (i + 1 < n) {
          --indeg[i + 1][j];
          --indeg[i + 1][j + 1];
        }
        break;
      }
      for_each_adj(x, y, [&](int nx, int ny) {
        if (v <= b[nx][ny]) {
          double cost;
          if (x < nx) {
            cost = (m - b[x][y]) + 1e-6;
          } else {
            cost = (m - b[x][y]) * A + B;
          }
          if (chmin(d[nx][ny], dxy + cost)) {
            q.emplace(d[nx][ny], nx, ny);
            prv[nx][ny] = {x, y};
          }
        }
      });
    }
  }
  return {k, ans};
}

HYPERPARAMETER(double, A, 0.01);
HYPERPARAMETER(double, B, 490);

void solve() {
  read_input();

  double curA = A;
  double curB = B;
  int k = eval(curA, curB).first;

  int min_k = k;
  double minA = curA;
  double minB = curB;
  for (auto sa : SA(1.9, 1, 0.1)) {
    auto nA = curA, nB = curB;
    (bernoulli(0.5) ? nA : nB) *= uniform(0.5, 2.0);
    int nk = eval(nA, nB).first;
    if (nk <= k + sa.thold) {
      k = nk;
      curA = nA;
      curB = nB;
      if (chmin(min_k, k)) {
        minA = curA;
        minB = curB;
      }
    }
  }

  auto ans = eval(minA, minB, true).second;
  print(ans.size());
  for (auto e : ans) {
    print(e);
  }
}

}  // namespace

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  solve();
}

#else  // __INCLUDE_LEVEL__

#undef assert
#define assert(expr) (expr) || (__builtin_unreachable(), 0)

#define HYPERPARAMETER(T, name, value) \
  const T name = getenv(#name) ? stod(getenv(#name)) : value

inline mt19937_64 mt_engine(
    chrono::steady_clock::now().time_since_epoch().count());

template <bool Log = false>
double uniform(double a, double b) {
  assert(a <= b);
  if constexpr (Log) {
    assert(0 < a);
    return exp(uniform(log(a), log(b)));
  } else {
    return uniform_real_distribution(a, b)(mt_engine);
  }
}

template <bool Log = false>
int randint(int a, int b) {
  assert(a <= b);
  if constexpr (Log) {
    assert(0 < a);
    return round(uniform<true>(a - 0.5, b + 0.5));
  } else {
    return uniform_int_distribution(a, b)(mt_engine);
  }
}

inline bool bernoulli(double p) {
  assert(0 <= p && p <= 1);
  return bernoulli_distribution(p)(mt_engine);
}

__attribute__((constructor)) inline double get_time() {
  using namespace chrono;
  static const auto s = steady_clock::now();
  return duration<double>(steady_clock::now() - s).count();
}

template <int UpdateInterval = 1>
class SA {
 public:
  explicit SA(double end_time, double start_temp = 0, double end_temp = 0)
      : start_time_(get_time()),
        time_limit_(end_time - start_time_),
        start_temp_(start_temp),
        cooling_rate_(start_temp ? end_temp / start_temp : 1),
        iter_(0),
        elapsed_(0),
        temp_(start_temp) {
    assert(start_time_ < end_time);
    assert(0 <= end_temp && end_temp <= start_temp);
  }

  SA begin() const { return *this; }
  int end() const { return 0; }

  bool operator!=(int) const {
    return elapsed_ < 1 || (cerr << "SA: " << iter_ << " iterations\n", false);
  }

  void operator++() {
    if (++iter_ % UpdateInterval == 0) {
      elapsed_ = (get_time() - start_time_) / time_limit_;
      temp_ = start_temp_ * pow(cooling_rate_, elapsed_);
    }
  }

  auto operator*() const {
    struct {
      int64_t iter;
      double elapsed;
      double temp;
      double thold;
    } sa{iter_, elapsed_, temp_, temp_ * -log(uniform(0, 1))};
    return sa;
  }

 private:
  double start_time_;
  double time_limit_;
  double start_temp_;
  double cooling_rate_;
  int64_t iter_;
  double elapsed_;
  double temp_;
};

template <class T, class U = T>
bool chmin(T& x, U&& y) {
  return y < x && (x = forward<U>(y), true);
}

template <class T, class U = T>
bool chmax(T& x, U&& y) {
  return x < y && (x = forward<U>(y), true);
}

namespace std {

template <class T1, class T2>
istream& operator>>(istream& is, pair<T1, T2>& p) {
  return is >> p.first >> p.second;
}

template <class... Ts>
istream& operator>>(istream& is, tuple<Ts...>& t) {
  return apply([&is](auto&... xs) -> istream& { return (is >> ... >> xs); }, t);
}

template <class... Ts>
istream& operator>>(istream& is, tuple<Ts&...>&& t) {
  return is >> t;
}

template <class R, enable_if_t<!is_convertible_v<R, string>>* = nullptr>
auto operator>>(istream& is, R&& r) -> decltype(is >> *begin(r)) {
  for (auto&& e : r) {
    is >> e;
  }
  return is;
}

template <class T1, class T2>
ostream& operator<<(ostream& os, const pair<T1, T2>& p) {
  return os << p.first << ' ' << p.second;
}

template <class... Ts>
ostream& operator<<(ostream& os, const tuple<Ts...>& t) {
  auto f = [&os](const auto&... xs) -> ostream& {
    [[maybe_unused]] auto sep = "";
    ((os << exchange(sep, " ") << xs), ...);
    return os;
  };
  return apply(f, t);
}

template <class R, enable_if_t<!is_convertible_v<R, string_view>>* = nullptr>
auto operator<<(ostream& os, R&& r) -> decltype(os << *begin(r)) {
  auto sep = "";
  for (auto&& e : r) {
    os << exchange(sep, " ") << e;
  }
  return os;
}

}  // namespace std

template <class... Ts>
void print(Ts&&... xs) {
  cout << tie(xs...) << '\n';
}

struct rep1 {
  int l, r;
  rep1(int l, int r) : l(l), r(r) {}
  explicit rep1(int n) : rep1(1, n) {}
  rep1 begin() const { return *this; }
  int end() const { return 0; }
  bool operator!=(int) const { return l <= r; }
  void operator++() { ++l; }
  int operator*() const { return l; }
};

inline rep1 rep(int l, int r) { return {l, r - 1}; }
inline rep1 rep(int n) { return {0, n - 1}; }

struct per1 {
  int l, r;
  per1(int l, int r) : l(l), r(r) {}
  explicit per1(int n) : per1(1, n) {}
  per1 begin() const { return *this; }
  int end() const { return 0; }
  bool operator!=(int) const { return l <= r; }
  void operator++() { --r; }
  int operator*() const { return r; }
};

inline per1 per(int l, int r) { return {l, r - 1}; }
inline per1 per(int n) { return {0, n - 1}; }

#endif  // __INCLUDE_LEVEL__
