#define NDEBUG

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <cmath>

#include <iostream>
#include <sstream>
#include <array>
#include <valarray>
#include <bitset>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <random>
#include <chrono>
#include <ranges>

using namespace std;


constexpr ranges::iota_view<int, int> range(int count)
{
    return ranges::iota_view<int, int>(0, count);
}

constexpr ranges::iota_view<int, int> range(size_t count)
{
    return ranges::iota_view<int, int>(0, (int)count);
}

template <typename T, size_t C>
constexpr ranges::iota_view<int, int> range(const array<T, C> &v)
{
    return ranges::iota_view<int, int>(0, (int)v.size());
}

template <typename T>
constexpr ranges::iota_view<int, int> range(const vector<T> &v)
{
    return ranges::iota_view<int, int>(0, (int)v.size());
}

constexpr int N = 500;
constexpr int M = 50;
constexpr long long Range = 4 * 1000000000000LL;
constexpr long long L = 1000000000000000LL - Range / 2;
constexpr long long U = L + Range;

struct World {
	array<long long, M> B;

	void create() {
		for (long long i : range(M)) {
			cin >> B[i];
		}
	}
};

inline string ActionString(const array<long long, N>& action) {
	stringstream ss;
	for (auto& v : action) {
		ss << v << " ";
	}
	ss << endl;
	return ss.str();
}
inline string ActionString2(const array<int, N>& action) {
	stringstream ss;
	for (auto& v : action) {
		ss << v << " ";
	}
	ss << endl;
	return ss.str();
}

struct Agent {
  World w;

  array<long long, N> a = {};
  array<int, N> x = {};
  array<long long, M> diff = {};

  long long tc = 0;

  void prepare() {
    constexpr int b = 8;
    constexpr long long ma = Range / 10;
    for (int n = M - 1; n >= M - b; n--) {
      a[n] = L;
    }
    for (int n = M - b - 1; n >= 0; n--) {
      double r = (double)(M - b - n) / (M - b);
      double r2 = r;
      if (r > 0.5) {
        r2 = (1.0 - r);
      }
      r2 = 1.0 - r2 * 0.59;
      a[n] = L + (long long)((Range - ma) * r * r2);
    }

    double current = (Range / M) * 32; 
    for (int n = M; n < N; n++) {
      a[n] = (long long)current;
      current *= 0.9523; 
    }
  }

  void set(int n, int over) {
    int minIndex = -1;
    long long minValue = 1000000000000000000LL;
    long long val = a[n];
    for (int m = 0; m < M; m++) {
      if (diff[m] <= 0) {
        continue;
      }
      long long d = diff[m] - val;
      if (d < over) {
        continue;
      }
      long long v = abs(d);
      if (minValue > v) {
        minValue = v;
        minIndex = m;
      }
    }
    if (minIndex >= 0) {
      diff[minIndex] -= val;
    }
    x[n] = minIndex + 1;
  }

  void plan() {
    array<int, N> minX = {};
    long long minSum = 1000000000000000000LL;
    for (int o = 1; o <= 800; o += 1) {
      for (int n = 0; n < M; n++) {
        diff[n] = w.B[n];
      }

      for (int n = 0; n < M; n++) {
        set(n, -o);
        if (x[n] == 0) {
          for (int i = 0; i < M; i++) {
            diff[i] = w.B[i] - a[M - 1 - i];
            x[M - 1 - i] = i + 1;
          }
          break;
        }
      }
      for (int n = M; n < N; n++) {
        set(n, -o);
      }
      long long dSum = 1;
      for (long long v : diff) {
        dSum += abs(v);
      }
      if (minSum > dSum) {
        minSum = dSum;
        minX = x;
      }
    }
    x = minX;
  }

};


int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int n, m;
	long long l, u;
	cin >> n >> m >> l >> u;

	Agent* agent = new Agent();

	agent->prepare();
	cout << ActionString(agent->a);
	cout << std::flush;

	World& w = agent->w;
	w.create();

	agent->plan();
	cout << ActionString2(agent->x);
	cout << std::flush;

	quick_exit(0);
}
