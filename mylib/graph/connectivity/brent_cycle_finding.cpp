// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/graph/connectivity/brent_cycle_finding.cpp
// Brent のアルゴリズムによるループ検出
// 数列 A_{i+1} = f(A_i) に対して
//   mu  : ループに入るまでの長さ
//   lam : ループの周期
// を O(mu + lam), メモリ O(1) で求める
//
// 使い方:
//   // 0 -> 1 -> 2 -> 3 -> 4 -> 2 -> 3 -> 4 -> ...
//   vector<int> next = {1, 2, 3, 4, 2};
//   auto f = [&](int x) { return next[x]; };
//
//   auto [mu, lambda] = brent_cycle_finding(f, 0);
//   // mu == 2: 0, 1 の2項を過ぎるとループに入る
//   // lambda == 3: 2, 3, 4 の3項周期
//
//   auto [before, cycle] = brent_cycle_lists(f, 0);
//   // before == {0, 1}
//   // cycle  == {2, 3, 4}
//
//   auto add = [](long long a, long long b) { return a + b; };
//   long long sum = fold_eventually_periodic_sequence(
//       [&](long long x) { return (long long)next[x]; }, 0LL, 10, add, 0LL);
//   // 先頭10項 0,1,2,3,4,2,3,4,2,3 の和なので sum == 24
//
//   // 状態と集約値の型が異なる場合は、最後に射影 value を渡す。
//   // op が列の連結を表すモノイドなら、最大 prefix 和なども集約できる。
//   struct S { long long sum, max_prefix; };
//   constexpr long long NEG_INF = -(1LL << 60);
//   auto concat = [](S a, S b) {
//       return S{a.sum + b.sum, std::max(a.max_prefix, a.sum + b.max_prefix)};
//   };
//   auto value = [](int x) { return S{x, x}; };
//   S result = fold_eventually_periodic_sequence(
//       [&](int x) { return next[x]; }, 0, 10, concat,
//       S{0, NEG_INF}, value);
template <class F, class T>
std::pair<long long, long long> brent_cycle_finding(F f, T x0) {
    long long power = 1;  // 走査ブロックのサイズ
    long long lam = 1;    // 現在検出している周期長

    T first = x0;      // ブロックの先頭
    T second = f(x0);  // 走査ポインタ

    // まず周期 lam を求める
    while (!(first == second)) {
        // 現在のブロックを走査し切ったら
        // ブロックサイズを倍にして先頭を更新
        if (power == lam) {
            first = second;
            power <<= 1;  // power *= 2
            lam = 0;
        }
        second = f(second);
        ++lam;
    }

    // 次にループ開始位置 mu を求める
    // second を lam だけ先に進めてから
    // first と同時に進めると，合流点が mu
    first = x0;
    second = x0;
    for (long long i = 0; i < lam; ++i) {
        second = f(second);
    }

    long long mu = 0;
    while (!(first == second)) {
        first = f(first);
        second = f(second);
        ++mu;
    }

    return {mu, lam};
}

// brent_cycle_finding を用いて
//   before_cycle : A_0 ... A_{mu-1}
//   cycle        : A_mu ... A_{mu+lam-1}
// を vector として返す
template <class F, class T>
std::pair<std::vector<T>, std::vector<T>> brent_cycle_lists(F f, T x0) {
    // ループの開始位置 mu と周期 lam を取得
    auto [mu, lam] = brent_cycle_finding(f, x0);

    // ===== ループ前の部分 =====
    std::vector<T> before_cycle;
    before_cycle.reserve(mu);  // 無駄な再確保を防ぐ

    if (mu > 0) {
        // A_0 = x0
        before_cycle.push_back(x0);
        // A_1 ... A_{mu-1}
        for (long long i = 1; i < mu; ++i) {
            before_cycle.push_back(f(before_cycle.back()));
        }
    }

    // ===== ループ本体 =====
    std::vector<T> cycle;
    cycle.reserve(lam);

    // ループ開始点 A_mu
    // mu == 0 のときは x0 自体がループに含まれる
    T curr = (mu == 0 ? x0 : f(before_cycle.back()));
    cycle.push_back(curr);

    // 残り A_{mu+1} ... A_{mu+lam-1}
    for (long long i = 1; i < lam; ++i) {
        cycle.push_back(f(cycle.back()));
    }

    return {before_cycle, cycle};
}

// f     : 次の状態を返す関数
// x0    : 初期状態 A_0
// n     : A_0 から A_{n-1} までを使う (n >= 0)
// op    : 結合的な二項演算（モノイド）。引数の順序は列の順序と同じ
// e     : op の単位元
// value : 状態 A_i を集約値へ写す関数
//
// 戻り値:
//   op(value(A_0), value(A_1), ..., value(A_{n-1}))
//
// 計算量: O(mu + lambda + log n), 追加メモリ O(1)
template <class F, class State, class Op, class Value, class Projection>
Value fold_eventually_periodic_sequence(
    F f, State x0, long long n, Op op, Value e, Projection value) {
    // ループ開始位置 mu と周期 lam を取得
    auto [mu, lam] = brent_cycle_finding(f, x0);

    Value ret = e;    // 最終的な答え
    State curr = x0;  // 現在の A_i

    // ===== ループに入る前 (A_0 ... A_{mu-1}) =====
    // n が小さい場合はここで終わる
    long long take = std::min(n, mu);
    for (long long i = 0; i < take; ++i) {
        ret = op(ret, value(curr));
        curr = f(curr);
        --n;
    }

    if (n == 0) return ret;

    // ===== ループ部分 =====
    // n 個を周期 lam で分解
    long long loop_count = n / lam;  // 完全な周回数
    long long remainder = n % lam;   // 余り

    // per_cycles:
    //   「1 周分」または「2^k 周分」の集約値を持つ
    Value per_cycles = e;

    // 1 周分と、その先頭 remainder 個分を計算する
    Value remainder_total = e;
    for (long long i = 1; i <= lam; ++i) {
        Value x = value(curr);
        per_cycles = op(per_cycles, x);
        if (i <= remainder) remainder_total = op(remainder_total, x);

        curr = f(curr);
    }

    // ===== 周回数を二乗法でまとめる =====
    // loop_total = op(per_cycles を loop_count 回)
    Value loop_total = e;
    while (loop_count > 0) {
        if (loop_count & 1) {
            loop_total = op(loop_total, per_cycles);
        }
        per_cycles = op(per_cycles, per_cycles);
        loop_count >>= 1;
    }

    // 完全な周回の後に余りを連結する
    ret = op(ret, loop_total);
    ret = op(ret, remainder_total);
    return ret;
}

// 状態そのものを集約する従来形式
template <class F, class T, class Op>
T fold_eventually_periodic_sequence(F f, T x0, long long n, Op op, T e) {
    return fold_eventually_periodic_sequence(
        f, x0, n, op, e, [](const T &x) { return x; });
}
