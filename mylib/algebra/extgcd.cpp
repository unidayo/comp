pair<ll, ll> extgcd(ll a, ll b) {
    /*一次不定方程式ax+by=1を解く*/
    if (b == 0) {
        return {1, 0};
    }
    auto [y, x] = extgcd(b, a % b);
    y -= a / b * x;
    return {x, y};
}