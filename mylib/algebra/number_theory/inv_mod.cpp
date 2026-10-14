// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algebra/number_theory/inv_mod.cpp
// modinv(a, m): a の mod m における逆元を返す
//
// 条件:
//   gcd(a, m) == 1 のときだけ逆元が存在する
//
// 返り値:
//   逆元が存在する: 0 <= x < m を満たす x
//   逆元が存在しない: -1
//
// 使い方:
//   ll x = modinv(a, mod);
//   if (x == -1) {
//       // 逆元なし
//   } else {
//       // a * x % mod == 1
//   }
//
// 注意:
//   a, m は long long の範囲
//   計算途中の u -= t * v が long long を超える可能性がある場合は __int128 を使う

ll modinv(ll a, ll m) {
    ll b = m;
    ll u = 1, v = 0;

    while (b) {
        ll t = a / b;

        a -= t * b;
        swap(a, b);

        u -= t * v;
        swap(u, v);
    }

    if (a != 1) return -1;

    u %= m;
    if (u < 0) u += m;
    return u;
}
