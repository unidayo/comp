// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algebra/number_theory/extgcd.cpp
pair<ll, ll> extgcd(ll a, ll b) {
    // ax + by = gcd(a, b) を満たす係数 {x, y} を返す。
    // gcd 自体が必要な場合は std::gcd(a, b) で求める。
    if (b == 0) {
        return {a < 0 ? -1 : 1, 0};
    }
    auto [y, x] = extgcd(b, a % b);
    y -= a / b * x;
    return {x, y};
}
