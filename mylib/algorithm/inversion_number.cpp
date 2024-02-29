ll inversion_number(vector<ll> &v) {
    ll n = ssize(v);
    fenwick_tree<ll> ft(n + 1);
    ll ret = 0;
    for (ll i : views::iota(0, n)) {
        ret += i - ft.sum(0, v[i]);
        ft.add(v[i], 1);
    }
    return ret;
}