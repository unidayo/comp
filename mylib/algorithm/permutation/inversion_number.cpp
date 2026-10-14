// 最終更新: 2026-10-14 18:14:44 JST
// 依存: mylib/acl/fenwicktree.cpp
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algorithm/permutation/inversion_number.cpp
ll inversion_number(const vector<ll> &v) {
    vector<ll> xs = v;
    ranges::sort(xs);
    xs.erase(unique(xs.begin(), xs.end()), xs.end());

    fenwick_tree<ll> ft(ssize(xs));
    ll ret = 0;
    ll seen = 0;
    for (ll x : v) {
        int rank = lower_bound(xs.begin(), xs.end(), x) - xs.begin();
        ret += seen - ft.sum(0, rank + 1);
        ft.add(rank, 1);
        ++seen;
    }
    return ret;
}
