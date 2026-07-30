// 最終更新: 2026-07-12 14:27:44 JST
// 正の整数 n の約数を昇順で列挙する。
//
// まず n を素因数分解し、各素因数の指数から約数を展開する。
// 単純に 1 から sqrt(n) まで調べる方法と比べ、合成数では素因数を
// 取り除くたびに試し割りの上限が下がる。残った試し割りでは
// 2、3、5 の倍数を除外する 30-wheel を使う。
//
// 計算量:
// - 試し割りは最悪 O(sqrt(n))
// - 約数の個数を d として、展開 O(d)、整列 O(d log d)
vector<ll> enum_divisors(ll n) {
    assert(n > 0);

    vector<pair<ll, int>> factors;
    auto divide_by = [&](ll prime) {
        if (n % prime != 0) return;
        int exponent = 0;
        do {
            n /= prime;
            ++exponent;
        } while (n % prime == 0);
        factors.emplace_back(prime, exponent);
    };

    divide_by(2);
    divide_by(3);
    divide_by(5);

    // 7 以上で 2、3、5 のいずれでも割れない数だけを試す。
    // 差分を繰り返すと、30k + {1,7,11,13,17,19,23,29} を巡回する。
    constexpr int wheel[] = {4, 2, 4, 2, 4, 6, 2, 6};
    ll candidate = 7;
    int wheel_index = 0;
    while (candidate <= n / candidate) {
        divide_by(candidate);
        candidate += wheel[wheel_index];
        wheel_index = (wheel_index + 1) & 7;
    }
    if (n > 1) factors.emplace_back(n, 1);

    vector<ll> divisors = {1};
    for (auto [prime, exponent] : factors) {
        const size_t base_size = divisors.size();
        ll prime_power = 1;
        for (int e = 1; e <= exponent; ++e) {
            prime_power *= prime;
            for (size_t i = 0; i < base_size; ++i) {
                divisors.push_back(divisors[i] * prime_power);
            }
        }
    }
    ranges::sort(divisors);
    return divisors;
}
