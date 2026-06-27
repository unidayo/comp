vector<ll> enum_primes(ll n) {
    vector<ll> ret;
    if (n < 2) return ret;
    ll array_length = (n - 1) / 2;
    vector<bool> prime_flag(array_length, true);
    for (ll i = 3; i * i <= n; i += 2) {
        ll j = (i - 1) / 2 - 1;
        if (prime_flag[j]) {
            for (ll k = i + j; k < array_length; k += i) {
                prime_flag[k] = false;
            }
        }
    }
    vector<ll> primes = {2};
    for (ll i = 0; i < array_length; ++i) {
        if (prime_flag[i]) primes.push_back((i + 1) * 2 + 1);
    }
    return primes;
}