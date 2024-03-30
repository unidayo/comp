#include <bits/stdc++.h>
#include <atcoder/all>

/* namespace */
using namespace std;
using namespace atcoder;


/* type */
using ll = long long;
using ull = unsigned long long;
using vll = __int128_t;
using vull = __uint128_t;

#define BEGIN(x) ranges::begin((x))
#define END(x) ranges::end((x))


/* algorithms */
template <typename T>
concept RAR = ranges::random_access_range<T>;

template <ranges::range Range>
using rvt = ranges::range_value_t<Range>;

template <RAR Range>
ll lower_count(const Range &r, const rvt<Range> &x) {
    return ranges::distance(BEGIN(r), ranges::lower_bound(r, x));
}

template <RAR Range>
ll upper_count(const Range &r, const rvt<Range> &x) {
    return ranges::distance(BEGIN(r), ranges::upper_bound(r, x));
}

template <class T>
bool chmin(T &a, const T &b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}
template <class T>
bool chmax(T &a, const T &b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}

template <RAR Range>
map<rvt<Range>, ll> compression(Range r) {
    ranges::sort(r);
    auto res = ranges::unique(r);
    r.erase(BEGIN(res), END(res));
    map<rvt<Range>, ll> ret;
    for (ll i = 0; auto v : r) {
        ret[v] = i;
        ++i;
    }
    return ret;
}

template <ranges::range Range>
vector<rvt<Range>> accumulate_sum(const Range &r) {
    vector<rvt<Range>> res(ssize(r));
    partial_sum(BEGIN(r), END(r), BEGIN(res));
    return res;
}

template <ranges::range Range>
vector<rvt<Range>> accumulate_sum(const Range &r, const rvt<Range> &init) {
    vector<rvt<Range>> res(ssize(r) + 1);
    res[0] = init;
    inclusive_scan(BEGIN(r), END(r), BEGIN(res) + 1, plus<>(), init);
    return res;
}

template <ranges::range Range>
vector<vector<rvt<Range>>> lpartition(const Range &r, int w) {
    int n = ssize(r);
    int h = (n + w - 1) / w;
    vector<vector<rvt<Range>>> ret(h, vector<rvt<Range>>(w));
    for (int i = 0; i < n; ++i) {
        ret[i / w][i % w] = r[i];
    }
    return ret;
}

template <RAR Range>
auto runlength(const Range &R) {
    vector<pair<rvt<Range>, ll>> ret;
    for (ll l = 0; l < ssize(R);) {
        ll r = l + 1;
        for (; r < ssize(R) && R[l] == R[r]; ++r) {
        }
        ret.emplace_back(R[l], r - l);
        l = r;
    }
    return ret;
}

template <RAR Range>
void remove_duplicates(Range &v) {
    ranges::sort(v);
    auto res = ranges::unique(v);
    v.erase(BEGIN(res), END(res));
}

template <ranges::range Range>
auto sum_of(Range R) {
    return reduce(BEGIN(R), END(R));
}
template <ranges::range Range, class T>
T sum_of(Range R) {
    return reduce(BEGIN(R), END(R), T());
}

template <class T>
vector<T> gen_sequence(T init, int n, auto op) {
    vector<T> res(n + 1);
    res[0] = init;
    for (int i = 0; i < n; ++i) {
        res[i + 1] = op(res[i]);
    }
    return res;
}

int main() {
}