// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

// clang-format off
#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <charconv>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numbers>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
using namespace std;

#ifdef ATCODER
#include <atcoder/all>
#include <ankerl/unordered_dense.h>
using namespace atcoder;
#endif

#ifdef ATCODER
template <typename K, typename V> using hash_map = ankerl::unordered_dense::map<K, V>;
template <typename K> using hash_set = ankerl::unordered_dense::set<K>;
#else
template <typename K, typename V> using hash_map = unordered_map<K, V>;
template <typename K> using hash_set = unordered_set<K>;
#endif

namespace cp_internal {
char char_buff[64];
template <ranges::range Range, class T>
auto sorted_lower_bound(const Range &r, const T &x) {
    if constexpr (requires { r.lower_bound(x); }) return r.lower_bound(x);
    else return ranges::lower_bound(r, x);
}
template <ranges::range Range, class T>
auto sorted_upper_bound(const Range &r, const T &x) {
    if constexpr (requires { r.upper_bound(x); }) return r.upper_bound(x);
    else return ranges::upper_bound(r, x);
}
};

/* type */
using uint = unsigned int;
using ll = long long;
using ull = unsigned long long;
using vll = __int128_t;
using uvll = __uint128_t;
template <typename T> concept RAR = ranges::random_access_range<T>;
template <ranges::range Range> using rvt = ranges::range_value_t<Range>;
