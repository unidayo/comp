vector<ll> enum_divisors(ll n) {
    vector<ll> divs_s, divs_l;
    ll i = 1;
    for (; i * i < n; ++i) {
        if (n % i == 0) {
            divs_s.push_back(i);
            divs_l.push_back(n / i);
        }
    }
    if (i * i == n) divs_s.push_back(i);
    reverse(divs_l.begin(), divs_l.end());
    divs_s.insert(divs_s.end(), divs_l.begin(), divs_l.end());
    return divs_s;
}