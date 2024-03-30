/* math*/
ll powll(ll x, ll n) {
    ll ret = 1;
    while (n) {
        if (n & 1) ret = x * ret;
        x *= x;
        n >>= 1;
    }
    return ret;
}
ll pow_mod64(vll x, vll n, ll m) {
    if (m == 1) return 0;
    vll ret = 1;
    x %= m;
    while (n) {
        if (n & 1) ret = x * ret % m;
        x = x * x % m;
        n >>= 1;
    }
    return ret;
}
template <class T>
T divceil(T x, T y) {
    return signbit(x) ^ signbit(y) ? x / y : (x + y - 1) / y;
}
template <class T>
T divfloor(T x, T y) {
    return signbit(x) ^ signbit(y) ? -divceil(abs(x), abs(y)) : x / y;
}
template <class T>
T smod(T x, T m) {
    if (x < 0) return (x % m + m) % m;
    return x % m;
}