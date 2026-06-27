#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#define N 30
#define K 465
#define INF 1000000000

template <class T> bool setmax(T& x, const T& y) {
  if (x < y) {
    x = y;
    return true;
  }
  return false;
}

std::vector<std::array<int, 4>> solve(std::array<std::array<int, N>, N> B, int cost, double dif_cost) {
  static std::array<int, K> X = {};
  static std::array<int, K> Y = {};
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j <= i; ++j) {
      X[B[i][j]] = i;
      Y[B[i][j]] = j;
    }
  }

  std::vector<std::array<int, 4>> op;
  const auto swap = [&](int x0, int y0, int x1, int y1) {
    auto& s = B[x0][y0];
    auto& t = B[x1][y1];
    std::swap(s, t);
    std::swap(X[s], X[t]);
    std::swap(Y[s], Y[t]);
    op.push_back({x0, y0, x1, y1});
  };

  static std::array<std::array<int, N>, N> dp = {};
  static std::array<std::array<std::pair<int, int>, N>, N> parent = {};
  static std::array<bool, N> done = {};
  static std::vector<int> order;
  const auto cell_score = [&](int x, int y) { return B[x][y]; };

  for (int target = 0; target < K; ++target) {
    for (auto& v : dp) v.fill(-INF);
    for (auto& v : parent) v.fill({-1, -1});
    for (int i = 0; i <= X[target]; ++i) {
      done.fill(false);
      order.clear();
      for (int j = 0; j <= i; ++j) {
        bool ok = true;
        if (j > 0 && B[i - 1][j - 1] > target) ok = false;
        if (j < i && B[i - 1][j] > target) ok = false;
        if (B[i][j] < target) ok = false;
        if (ok) {
          const double dif = std::abs(j - i * 0.5);
          dp[i][j] = dif_cost * std::pow(dif, 2);
        }
        if (dp[i][j] != -INF) {
          dp[i][j] += cell_score(i, j);
          order.push_back(j);
        }
      }
      std::sort(order.begin(), order.end(), [&](int a, int b) { return dp[i][a] > dp[i][b]; });
      for (const int j : order) {
        const int tmp = dp[i][j] - cost;
        done[j] = true;
        if (i < N - 1) {
          if (setmax(dp[i + 1][j], tmp)) parent[i + 1][j] = {i, j};
          if (setmax(dp[i + 1][j + 1], tmp)) parent[i + 1][j + 1] = {i, j};
        }
        if (j > 0 && !done[j - 1] && setmax(dp[i][j - 1], tmp)) parent[i][j - 1] = {i, j};
        if (j < i && !done[j + 1] && setmax(dp[i][j + 1], tmp)) parent[i][j + 1] = {i, j};
      }
    }
    auto& x = X[target];
    auto& y = Y[target];
    while (x > 0) {
      const auto& [nx, ny] = parent[x][y];
      if (nx == -1) break;
      swap(x, y, nx, ny);
    }
  }
  return op;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::array<std::array<int, N>, N> B = {};
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j <= i; ++j) {
      std::cin >> B[i][j];
    }
  }

  std::vector<std::array<int, 4>> op;
  for (int c = 35; c <= 65; ++c) {
    for (int d = 45; d <= 85; d += 2) {
      auto cand = solve(B, c * 10, d * 0.1);
      if (op.empty() || op.size() > cand.size()) {
        op = std::move(cand);
      }
    }
  }
  std::cout << op.size() << '\n';
  for (const auto& [x0, y0, x1, y1] : op) {
    std::cout << x0 << ' ' << y0 << ' ' << x1 << ' ' << y1 << '\n';
  }
  return 0;
}
