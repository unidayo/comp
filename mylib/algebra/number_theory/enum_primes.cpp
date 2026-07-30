// 最終更新: 2026-07-12 14:57:34 JST
// n 以下の素数を昇順で列挙する。
//
// 小さい n では奇数だけを保持するエラトステネスの篩を使う。
// 大きい n では 2、3、5 の倍数を持たない30-wheel分割篩を使う。
// 返す素数列を除く作業領域は O(sqrt(n))。
vector<ll> enum_primes(ll n) {
    vector<ll> primes;
    if (n < 2) return primes;

    auto simple_sieve = [](ll limit) {
        // 添字 i は奇数 2*i+1 に対応する。偶数は最初から保持しない。
        vector<bool> composite((size_t)(limit / 2 + 1));
        for (ll p = 3; p <= limit / p; p += 2) {
            if (composite[(size_t)(p / 2)]) continue;
            for (ll multiple = p * p; multiple <= limit; multiple += 2 * p) {
                composite[(size_t)(multiple / 2)] = true;
            }
        }

        vector<ll> result;
        if (limit >= 2) result.push_back(2);
        for (ll value = 3; value <= limit; value += 2) {
            if (!composite[(size_t)(value / 2)]) result.push_back(value);
        }
        return result;
    };

    // この程度までは、区間を分けない方が制御コストが小さい。
    constexpr ll direct_limit = 4'000'000;
    if (n <= direct_limit) return simple_sieve(n);

    // 素数定理による上界寄りの見積もりで、結果配列の再確保を減らす。
    primes.reserve((size_t)(n / (log((long double)n) - 1.1L)));
    primes.push_back(2);
    primes.push_back(3);
    primes.push_back(5);

    ll root = sqrt((long double)n);
    while (root + 1 <= n / (root + 1)) ++root;
    while (root > n / root) --root;
    vector<ll> base_primes = simple_sieve(root);

    // 30k に足す剰余と、各剰余から次の候補へ進む差分。
    constexpr int residues[8] = {1, 7, 11, 13, 17, 19, 23, 29};
    constexpr int wheel_steps[8] = {6, 4, 2, 4, 2, 4, 6, 2};
    constexpr int residue_to_slot[30] = {
        -1,  0, -1, -1, -1, -1, -1,  1, -1, -1,
        -1,  2, -1,  3, -1, -1, -1,  4, -1,  5,
        -1, -1, -1,  6, -1, -1, -1, -1, -1,  7,
    };

    // 各グループの8候補を1バイトのビットマスクで保持する。
    // 2^16 グループ = 64 KiB。
    // 1ブロックで約196万の整数を処理する。
    constexpr size_t block_groups = 1 << 16;
    constexpr ll block_span = 30 * (ll)block_groups;
    vector<unsigned char> composite(block_groups);

    // primesieveと同様に、小さい素数の合成数パターンを事前生成して使い回す。
    // 7*11*13*17 グループで同じパターンが繰り返される。
    constexpr int pre_sieve_primes[4] = {7, 11, 13, 17};
    constexpr size_t pre_sieve_period = 7 * 11 * 13 * 17;
    array<unsigned char, pre_sieve_period> pre_sieve{};
    for (size_t group = 0; group < pre_sieve_period; ++group) {
        for (int slot = 0; slot < 8; ++slot) {
            ll value = 30 * (ll)group + residues[slot];
            for (int p : pre_sieve_primes) {
                if (value % p == 0) {
                    pre_sieve[group] |= (unsigned char)(1U << slot);
                    break;
                }
            }
        }
    }

    for (ll low = 0; low <= n;) {
        ll high = low > n - min(n, block_span - 1) ? n : low + block_span - 1;
        size_t groups = (size_t)((high - low) / 30 + 1);
        size_t phase = (size_t)(low / 30) % pre_sieve_period;
        for (size_t written = 0; written < groups;) {
            size_t count = min(groups - written, pre_sieve_period - phase);
            copy_n(pre_sieve.begin() + phase, count, composite.begin() + written);
            written += count;
            phase = 0;
        }

        // 周期パターンでは素数自身も消えるため、最初のブロックだけ戻す。
        for (int p : pre_sieve_primes) {
            if (low <= p && p <= high) {
                size_t group = (size_t)((p - low) / 30);
                composite[group] &= (unsigned char)~(1U << residue_to_slot[p % 30]);
            }
        }

        for (ll p : base_primes) {
            if (p <= 17) continue;
            if (p > high / p) break;

            // multiple = p * multiplier とし、multiplier も30-wheel上だけを巡る。
            ll multiplier = max(p, low / p + (low % p != 0));
            while (multiplier % 2 == 0 || multiplier % 3 == 0 || multiplier % 5 == 0) {
                ++multiplier;
            }
            ll multiplier_limit = high / p;
            int multiplier_slot = residue_to_slot[multiplier % 30];
            ll multiple = p * multiplier;
            size_t group = (size_t)((multiple - low) / 30);

            // p ごとに8状態の遷移を作り、マーキング中の除算・剰余をなくす。
            int mark_slot[8];
            size_t group_step[8];
            int p_residue = (int)(p % 30);
            for (int state = 0; state < 8; ++state) {
                int residue = p_residue * residues[state] % 30;
                mark_slot[state] = residue_to_slot[residue];
                group_step[state] = (size_t)((residue + p * wheel_steps[state]) / 30);
            }

            while (multiplier <= multiplier_limit) {
                composite[group] |= (unsigned char)(1U << mark_slot[multiplier_slot]);
                ll step = wheel_steps[multiplier_slot];
                multiplier += step;
                group += group_step[multiplier_slot];
                multiplier_slot = (multiplier_slot + 1) & 7;
            }
        }

        for (size_t group = 0; group < groups; ++group) {
            ll base = low + 30 * (ll)group;
            unsigned available = (unsigned)(~composite[group]) & 0xffU;
            while (available != 0) {
                int slot = countr_zero(available);
                ll value = base + residues[slot];
                if (7 <= value && value <= high) primes.push_back(value);
                available &= available - 1;
            }
        }
        if (high == n) break;
        low += block_span;
    }
    return primes;
}
