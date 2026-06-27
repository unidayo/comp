#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include <map>
constexpr int N = 5000;
constexpr int W = 1e5;
constexpr int LogW = 17;
#include <cstdint>
#include <numeric>
#include <chrono>

struct Timer {
  std::chrono::high_resolution_clock::time_point st;
  Timer() { st = now(); }
  std::chrono::high_resolution_clock::time_point now() { return std::chrono::high_resolution_clock::now(); }
  std::chrono::milliseconds::rep span() {
    auto ed = now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(ed - st).count();
  }
};

struct Xor128 {
  using state_type = std::uint32_t;
  using result_type = std::uint32_t;
  state_type x = 123456789, y = 362436039, z = 521288629, w = 88675123;
  constexpr static double INV_MAX = 1.0 / 0xFFFFFFFF;

  static constexpr result_type min() {
    return std::numeric_limits<result_type>::min();
  }
  static constexpr result_type max() {
    return std::numeric_limits<result_type>::max();
  }

  constexpr Xor128(state_type seed = 88675123): w(seed) {}
  constexpr void seed(state_type seed = 88675123) {
    w = seed;
  }
  constexpr result_type operator()() {
    state_type t = x ^ (x << 11);
    x = y, y = z, z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
  }
  void discard(unsigned long long z) {
    while(z --> 0) {
      (*this)();
    }
  }

  // [0, a)
  constexpr result_type rand_int(state_type a) {
    return ((std::uint64_t) (*this)() * a) >> 32;
  }
  
  // [a, b]
  constexpr result_type rand_int(state_type a, state_type b) {
    return rand_int(b - a + 1) + a;
  }
  
  // [0, 1]
  constexpr double prob() {
    return (*this)() * INV_MAX;
  }
  
  // [a, b]
  constexpr double rand_double(double a, double b) {
    return prob() * (b - a) + a;
  }
};

#include <set>
#include <vector>
#include <algorithm>

template<class T>
struct Compression {
  using size_type = std::size_t;

  std::vector<T> v;

  Compression(){}
  void add(const T& t) { v.push_back(t); }
  void build() {
    std::sort(begin(v), end(v));
    v.erase(unique(begin(v), end(v)), end(v));
  }
  size_type comp(const T& x) const {
    return lower_bound(begin(v), end(v), x) - begin(v);
  }
  size_type size() const { return v.size(); }
};

template<class T>
struct plane_to_line {
  using size_type = std::size_t;
  using coord_type = std::pair<T, T>;
  using value_type = T;

  std::vector<std::pair<coord_type, size_type>> elems;
  std::vector<size_type> yarray;
  std::vector<size_type> xstart;
  Compression<value_type> X, Y;

  plane_to_line(const std::vector<coord_type>& a): elems(a.size()) {
    for(size_type i = 0;i < a.size();i++) {
      elems[i] = { a[i], i };
      X.add(a[i].first);
      Y.add(a[i].second);
    }
    sort(std::begin(elems), std::end(elems));
    X.build();
    Y.build();

    xstart.resize(X.size() + 1);
    yarray.resize(elems.size());
    size_type x = 0;
    for(size_type i = 0;i < elems.size();i++) {
      if(!i || elems[i - 1].first.first < elems[i].first.first) {
        xstart[x++] = i;
      }
      yarray[i] = Y.comp(elems[i].first.second);
    }
    xstart[X.size()] = elems.size();
  }

  struct rangefreq_arg {
    size_type left;
    size_type right;
    size_type x;
    size_type y;
  };

  rangefreq_arg to_rangefreq(value_type xl, value_type xr, value_type yl, value_type yr) const {
    size_type cxl = xstart[X.comp(xl)];
    size_type cxr = xstart[X.comp(xr)];
    size_type cyl = Y.comp(yl);
    size_type cyr = Y.comp(yr);
    return rangefreq_arg { cxl, cxr, cyl, cyr };
  }
};

#include <cstdint>
#include <set>
#include <vector>
#include <iostream>

using i64 = long long;

class bitvector {
  using bit_type = std::uint_least64_t;
  using size_type = std::size_t;
  static constexpr size_type wordsize = 64;
  
  std::vector<bit_type> bit;
  std::vector<size_type> sum;
  
public:

  bitvector() : bit(), sum() {} 
  bitvector(const size_type size)
    : bit(size / wordsize + 1, 0), sum(size / wordsize + 1, 0) {}

  void set(const size_type i) {
    bit[i / wordsize] |= static_cast<bit_type>(1) << (i % wordsize);
  }
  void build() {
    for (size_type i = 1; i < bit.size(); i++) {
      sum[i] = sum[i - 1] + __builtin_popcountll(bit[i - 1]);
    }
  }

  size_type at(const size_type i) const {
    return bit[i / wordsize] >> (i % wordsize);
  }

  // count of ones in [0, i)
  size_type rank(const size_type i) const {
    return sum[i / wordsize]
      + __builtin_popcountll(bit[i / wordsize] & (static_cast<bit_type>(1) << (i % wordsize)) - 1);
  }

  // count of ones in [0, i)
  size_type rank(const size_type i, const size_type b) const {
    size_type ans = sum[i / wordsize]
      + __builtin_popcountll(bit[i / wordsize] & (static_cast<bit_type>(1) << (i % wordsize)) - 1);
    if(b) return ans;
    else return i - ans;
  }
};

class wavelet_matrix {
  using Integer = std::size_t;
  using integer_type = Integer;
  using size_type = std::size_t;


  const size_type depth;
  const size_type len;
  std::vector<bitvector> mat;
  std::vector<size_type> spl;

public:

  wavelet_matrix(const std::vector<integer_type>& arr, size_type de)
    : depth(de),
      len(arr.size()),
      mat(std::vector<bitvector>(depth, bitvector(arr.size()))),
      spl(std::vector<size_type>(depth, 0)) {
        std::vector<size_type> idx(len);
        std::vector<size_type> left(len), right(len);
        for(size_type i = 0;i < len;i++) idx[i] = i;
        for(size_type d = depth; d-- > 0;) {
          size_type l = 0, r = 0;
          
          for(size_type i = 0; i < len; i++) {
            size_type k = (arr[idx[i]] >> d) & 1;
            if(k) right[r++] = idx[i], mat[d].set(i);
            else left[l++] = idx[i];
          }
          mat[d].build();
          spl[d] = l;
          swap(idx, left);
          for(size_type i = 0; i < r; i++) idx[i + l] = right[i];
        }
      }

  integer_type at(size_type i) const {
    integer_type x = static_cast<integer_type>(0);
    for(size_type d = depth; d-- > 0;) {
      size_type k = mat[d].at(i);
      x |= (static_cast<integer_type>(k) << d);
      i = mat[d].rank(i, k) + spl[d] * k;
    }
    return x;
  }

  // counting elements that equal to x in range [left, right)
  size_type rank_x(size_type left, size_type right, integer_type x) const {
    for(size_type d = depth; d-- > 0;) {
      size_type k = ((x >> d) & 1);
      left = mat[d].rank(left, k) + spl[d] * k;
      right = mat[d].rank(right, k) + spl[d] * k;
    }
    return right - left;
  }

  // sorted(arr[left..right])[i]
  integer_type quantile(size_type left, size_type right, size_type i) const {
    integer_type x = static_cast<integer_type>(0);
    for(size_type d = depth; d-- > 0;) {
      size_type cnt = mat[d].rank(right, 0) - mat[d].rank(left, 0);
      size_type k = (i < cnt) ? 0 : 1;
      if(k == 1) {
        i -= cnt;
        x |= (1 << d);
      }
      left = mat[d].rank(left, k) + spl[d] * k;
      right = mat[d].rank(right, k) + spl[d] * k;
    }
    return x;
  }

  struct rank_result {
    size_type le;
    size_type eq;
    size_type mo;
  };

  // couting elements that less than x, equal to x, and more than x in range [left, right)
  rank_result rank_less_eq_more(size_type left, size_type right, integer_type x) const {
    size_type le = 0, mo = 0;
    for(size_type d = depth; d --> 0;) {
      size_type k = (x >> d) & 1;
      size_type l = mat[d].rank(left, 1);
      size_type r = mat[d].rank(right, 1);
      if(k == 0) {
        mo += r - l;
        left -= l;
        right -= r;
      }
      else {
        le += (right - left) - (r - l);
        left = l + spl[d];
        right = r + spl[d];
      }
    }
    return rank_result { le, right - left, mo };
  }

  size_type rangefreq(size_type left, size_type right, integer_type x, integer_type y, integer_type l, size_type d) const {
    integer_type r = l + (1 << d);
    if(x <= l && r <= y) {
      return right - left;
    }
    else if(y <= l || r <= x) {
      return 0;
    }
    else {
      d--;
      size_type lr = mat[d].rank(left, 1);
      size_type rr = mat[d].rank(right, 1);
      return
        rangefreq(left - lr, right - rr, x, y, l, d) +
        rangefreq(lr + spl[d], rr + spl[d], x, y, l + (1 << d), d);
    }
  }

  size_type rangefreq(size_type left, size_type right, integer_type x, integer_type y) const {
    return rangefreq(left, right, x, y, 0, depth);
  }

  size_type rangefreq(plane_to_line<int>::rangefreq_arg arg) const {
    return rangefreq(arg.left, arg.right, arg.x, arg.y);
  }
};

#include <vector>
#include <tuple>

struct union_find {
  std::vector<int> par;
  int V;
  union_find(int N): par(N, -1), V(N) {}
  void reset() {
    par.assign(par.size(), -1);
    V = par.size();
  }
  int root(int x) {
    return par[x] < 0 ? x : par[x] = root(par[x]);
  }
  std::tuple<int, int> unite(int x, int y) {
    x = root(x);
    y = root(y);
    if(x == y) return { -1, -1 };
    if(par[x] > par[y]) std::swap(x, y);
    V -= 1;
    par[x] += par[y];
    par[y] = x;
    return { x, y };
  }
  int size(int x) {
    return -par[root(x)];
  }
};

using P = std::pair<int, int>;

int main() {
  Timer timer;
  {
    int n_ = 0;
    std::cin >> n_;
  }
  std::vector<P> A(N);
  std::vector<P> B(N);
  for(int i = 0; i < N; i++) {
    std::cin >> A[i].first >> A[i].second;
  }
  for(int i = 0; i < N; i++) {
    std::cin >> B[i].first >> B[i].second;
  }

  plane_to_line<int> Apl(A);
  plane_to_line<int> Bpl(B);
  wavelet_matrix Awm(Apl.yarray, LogW);
  wavelet_matrix Bwm(Bpl.yarray, LogW);

  auto range_count = [&](int xl, int xr, int yl, int yr) {
    return Awm.rangefreq(Apl.to_rangefreq(xl, xr, yl, yr)) - Bwm.rangefreq(Bpl.to_rangefreq(xl, xr, yl, yr));
  };

  int step = 10000;
  int H = W / step;
  std::vector<int> rect_cnt(H * H);
  for(int i = 0; i < W; i += step) {
    for(int j = 0; j < W; j += step) {
      rect_cnt[(i / step) * H + j / step] = range_count(i, i + step, j, j + step);
    }
  }

  std::vector<bool> use(H * H, false);
  std::vector<int> vs;

  int first_root = std::max_element(rect_cnt.begin(), rect_cnt.end()) - rect_cnt.begin();
  use[first_root] = true;
  vs.push_back(first_root);

  Xor128 mt(768);

  auto acc = [&](int x, int y) {
    if(0 <= x && x < H && 0 <= y && y < H) {
      return (bool)use[x * H + y];
    }
    else {
      return false;
    }
  };
  const std::vector<std::pair<int, int>> rounds = {
    {-1, -1},
    {0, -1},
    {1, -1},
    {1, 0},
    {1, 1},
    {0, 1},
    {-1, 1},
    {-1, 0},
  };
  const std::vector<std::pair<int, int>> cands = {
    { -1, 0 },
    { 1, 0 },
    { 0, -1},
    {0, 1 },
  };
  auto check_ok = [&](bool next_use, int x, int y) {
    static union_find uf(9);
    uf.reset();
    for(int i = 0; i < 8; i++) {
      int j = (i + 1) % 8;
      if(acc(x + rounds[i].first, y + rounds[i].second) == acc(x + rounds[j].first, y + rounds[j].second)) {
        uf.unite(i, j);
      }
    }
    if(uf.V == 2 && acc(x - 1, y - 1) == true) {
      return next_use;
    }
    for(int i = 1; i < 8; i += 2) {
      if(acc(x + rounds[i].first, y + rounds[i].second) == next_use) {
        uf.unite(i, 8);
      }
    }
    return uf.V == 2;
  };

  auto length_delta_cnt = [&](bool next_use, int x, int y) {
    int cnt = 0;
    for(auto& c: cands) {
      int i = x + c.first;
      int j = y + c.second;
      if(acc(i, j) == next_use) {
        cnt -= 1;
      }
      else {
        cnt += 1;
      }
    }
    return cnt;
  };


  int length = step * 4;
  int weight = rect_cnt[first_root];
  const double ratio = 1.5;
  const double T0 = 35 * ratio;
  const double T1 = 0.05 * ratio;
  double T = T0;
  int iter = 0;

  std::vector<int> scales = { 10000, 5000, 2500, 625 };
  //std::vector<int> scales = { 10000, 5000};
  int scale_idx = 0;
  const int YET = -1e9;
  auto get_weight = [&](int x, int y) {
    if(rect_cnt[x * H + y] == YET) {
      rect_cnt[x * H + y] = range_count(x * step, x * step + step, y * step, y * step + step);
    }
    return rect_cnt[x * H + y];
  };
  while(true) {
    iter += 1;
    int t = timer.span();
    if(t >= 1980) {
      break;
    }
    double p = double(t) / double(1980);
    if(iter % 100000 == 0) {
      std::cerr << T << " " << weight << " " << length << std::endl;
      T = std::pow(T0, 1 - p) * std::pow(T1, p);
    }
    if(p >= double(1) / double(scales.size()) * (scale_idx + 1)) {
      /*
      for(int i = 0; i < H; i++) {
        for(int j = 0; j < H; j++) {
          std::cerr << (use[i * H + j] ? "#" : " ");
        }
        std::cerr << std::endl;
      }
      */
      std::cerr << "up scale!" << std::endl;
      // up scale
      // rect_cnt
      // use
      // vs
      // step
      // H
      int new_step = scales[scale_idx + 1];
      int new_H = W / new_step;
      int R = new_H / H;
      std::cerr << H << " -> " << new_H << std::endl;
      std::vector<int> new_rect_cnt(new_H * new_H, YET);
      std::vector<bool> new_use(new_H * new_H, false);
      std::vector<int> new_vs;
      for(auto& v: vs) {
        // v = i * H + j
        // nv = i * H * R + j * R
        int x = v / H;
        int y = v % H;
        for(int i = 0; i < R; i++) {
          for(int j = 0; j < R; j++) {
            int nv = (x * R + i) * new_H + (y * R + j);
            //std::cerr << i << " " << j << " = " << x << " " << y << " -> " << nv / new_H << " " << nv % new_H << std::endl;
            new_vs.push_back(nv);
            new_use[nv] = true;
          }
        }
      }
      scale_idx += 1;

      new_rect_cnt.swap(rect_cnt);
      new_use.swap(use);
      new_vs.swap(vs);
      step = new_step;
      H = new_H;
      /*
      for(int i = 0; i < H; i++) {
        for(int j = 0; j < H; j++) {
          std::cerr << (use[i * H + j] ? "#" : " ");
        }
        std::cerr << std::endl;
      }
      */
    }

    if(mt() % 3 == 0 || vs.size() == 1) {
      // insert
      int idx = mt.rand_int(vs.size());
      int x = vs[idx] / H;
      int y = vs[idx] % H;
      std::vector<std::pair<int, int>> cs;
      for(auto& c: cands) {
        int i = x + c.first;
        int j = y + c.second;
        if(0 <= i && i < H && 0 <= j && j < H && !use[i * H + j] && check_ok(true, i, j)) {
          cs.push_back({ i, j });
        }
      }
      if(cs.size() == 0) continue;
      auto [i, j] = cs[mt.rand_int(cs.size())];
      int next_length = length + step * length_delta_cnt(true, i, j);
      int weight_delta = get_weight(i, j);
      if(next_length > 400000) {
        continue;
      }
      if(weight_delta >= 0 || mt.prob() < std::exp(weight_delta / T)) {
        vs.push_back(i * H + j);
        use[i * H + j] = true;
        length = next_length;
        weight += weight_delta;
      }
    }
    else {
      // erase
      int idx = mt.rand_int(vs.size());
      int x = vs[idx] / H;
      int y = vs[idx] % H;
      if(!check_ok(false, x, y)) continue;
      int next_length = length + step * length_delta_cnt(false, x, y);
      int weight_delta = - get_weight(x, y);
      if(next_length > 400000) {
        continue;
      }
      if(weight_delta >= 0 || mt.prob() < std::exp(weight_delta / T)) {
        if(idx != vs.size() - 1) {
          std::swap(vs[idx], vs.back());
        }
        vs.pop_back();
        use[x * H + y] = false;
        length = next_length;
        weight += weight_delta;
      }
    }
  }

  std::map<P, std::vector<P>> edges;

  P p, q;
  int result = weight;
  for(int i = 0; i < H; i++) {
    for(int j = 0; j < H; j++) {
      if(!use[i * H + j]) {
        continue;
      }
      if(i == 0 || !use[(i - 1) * H + j]) {
        p = P(i * step, j * step);
        q = P(i * step, (j + 1) * step);
        edges[p].push_back(q);
        edges[q].push_back(p);
      }
      if(i == H - 1 || !use[(i + 1) * H + j]) {
        p = P((i + 1) * step, j * step);
        q = P((i + 1) * step, (j + 1) * step);
        edges[p].push_back(q);
        edges[q].push_back(p);
      }
      if(j == 0 || !use[i * H + j - 1]) {
        p = P(i * step, j * step);
        q = P((i + 1) * step, j * step);
        edges[p].push_back(q);
        edges[q].push_back(p);
      }
      if(j == H - 1 || !use[i * H + j + 1]) {
        p = P(i * step, (j + 1) * step);
        q = P((i + 1) * step, (j + 1) * step);
        edges[p].push_back(q);
        edges[q].push_back(p);
      }
    }
  }

  {
    std::vector<P> ans = { p, q };
    while(ans.back() != p) {
      int i = ans.size() - 1;
      for(auto& next: edges[ans[i]]) {
        if(next != ans[i - 1]) {
          ans.push_back(next);
          break;
        }
      }
    }
    ans.pop_back();
    std::cout << ans.size() << std::endl;
    for(auto v: ans) {
      std::cout << v.first << " " << v.second << std::endl;
    }
  }

  std::cerr << result << std::endl;
}
