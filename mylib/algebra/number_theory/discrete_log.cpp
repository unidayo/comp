// 最終更新: 2026-07-12 14:27:44 JST
ll isqrt(ll n) {
    assert(n >= 0);
    if (n == 0) return 0;
    ll c = 64 - countl_zero((ull)n);
    c = (c - 1) >> 1;
    ll c2 = 64 - countl_zero(ull(c));
    ll a = 1;
    ll d = 0;
    for (ll s = c2 - 1; s >= 0; --s) {
        ll e = d;
        d = c >> s;
        a = (a << (d - e - 1)) + (n >> (2 * c - e - d + 1)) / a;
    }
    return a > n / a ? a - 1 : a;
}

ll discrete_log(ll a, ll b, const ll mod) {
    // a^x ≡ b (mod mod) を満たす最小の非負整数 x を返す。
    // 解が存在しない場合は -1 を返す。mod は素数でなくてもよい。
    // 計算量・メモリ使用量は O(sqrt(mod))。
    assert(mod > 0);
    a = smod(a, mod);
    b = smod(b, mod);
    if (mod == 1) return 0;

    // gcd(a, mod) != 1 の部分を先に取り除く。
    // 不変条件: 元の式は a^(x + offset) ≡ coefficient * a^x に帰着される。
    ll reduced_mod = mod;
    ll reduced_b = b;
    ll coefficient = 1;
    ll offset = 0;
    while (true) {
        ll g = gcd(a, reduced_mod);
        if (g == 1) break;
        if (reduced_b == coefficient) return offset;
        if (reduced_b % g != 0) return -1;
        reduced_b /= g;
        reduced_mod /= g;
        ++offset;
        coefficient = (__int128)coefficient * (a / g) % reduced_mod;
    }
    if (reduced_mod == 1) return offset;
    ll reduced_a = a % reduced_mod;

    // coefficient の逆元を掛け、a^x ≡ target (mod reduced_mod) にする。
    // ここでは gcd(coefficient, reduced_mod) = 1 が保証されている。
    auto inverse_mod = [](ll value, ll modulus) -> ll {
        ll s = modulus, t = value;
        __int128 m0 = 0, m1 = 1;
        while (t != 0) {
            ll q = s / t;
            s -= q * t;
            swap(s, t);
            m0 -= (__int128)q * m1;
            swap(m0, m1);
        }
        m0 %= modulus;
        if (m0 < 0) m0 += modulus;
        return (ll)m0;
    };
    ll target = (__int128)reduced_b * inverse_mod(coefficient, reduced_mod) % reduced_mod;

    ll block = isqrt(reduced_mod);
    if (block * block < reduced_mod) ++block;

    // 乗算方法を最初に一度だけ分岐する。
    // reduced_mod <= floor(sqrt(LLONG_MAX)) なら long long の積で安全。
    auto bsgs = [&](auto multiply) -> ll {
        unordered_map<ll, ll> baby;
        // 低めの負荷率にして、探索時の衝突とポインタ追跡を減らす。
        baby.max_load_factor(0.5f);
        baby.reserve((size_t)block);

        ll power = 1;
        for (ll j = 0; j < block; ++j) {
            baby.emplace(power, j);  // 同じ値には最小の j だけを残す。
            power = multiply(power, reduced_a);
        }

        // factor = a^(-block)。互いに素なので逆元が存在する。
        ll factor = 1;
        ll base = inverse_mod(reduced_a, reduced_mod);
        for (ll exponent = block; exponent > 0; exponent >>= 1) {
            if (exponent & 1) factor = multiply(factor, base);
            base = multiply(base, base);
        }

        ll giant = target;
        for (ll i = 0; i <= block; ++i) {
            auto it = baby.find(giant);
            if (it != baby.end()) return offset + i * block + it->second;
            giant = multiply(giant, factor);
        }
        return -1;
    };

    if (reduced_mod <= 3037000499LL) {
        return bsgs([&](ll x, ll y) { return x * y % reduced_mod; });
    }
    return bsgs([&](ll x, ll y) { return (ll)((__int128)x * y % reduced_mod); });
}
