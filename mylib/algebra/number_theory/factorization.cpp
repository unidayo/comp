// 最終更新: 2026-07-12 15:10:05 JST
// 正の整数 n を素因数へ分解し、重複を含めて昇順で返す。
//
// 2、3、5を先に取り除き、それ以降は30-wheelにより
// 30k + {1, 7, 11, 13, 17, 19, 23, 29} の候補だけを試し割りする。
// 素因数を取り除くたびに探索上限も小さくなる。
// 最悪計算量は O(sqrt(n))、返却配列以外の追加領域は O(1)。
vector<ll> factorization(ll n) {
    assert(n > 0);
    vector<ll> factors;
    if (n == 1) return factors;

    auto divide_by = [&](ll prime) {
        while (n % prime == 0) {
            factors.push_back(prime);
            n /= prime;
        }
    };

    divide_by(2);
    divide_by(3);
    divide_by(5);

    constexpr int wheel_steps[8] = {4, 2, 4, 2, 4, 6, 2, 6};
    constexpr ll sqrt_ll_max = 3037000499LL;
    ll candidate = 7;
    int wheel_index = 0;
    while (candidate <= sqrt_ll_max && candidate * candidate <= n) {
        divide_by(candidate);
        candidate += wheel_steps[wheel_index];
        wheel_index = (wheel_index + 1) & 7;
    }
    if (n > 1) factors.push_back(n);
    return factors;
}
