// Brent のアルゴリズムによるループ検出
// 数列 A_{i+1} = f(A_i) に対して
//   mu  : ループに入るまでの長さ
//   lam : ループの周期
// を O(mu + lam), メモリ O(1) で求める
template <class F, class T>
std::pair<long long, long long> cycle_detection(F f, T x0) {
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

// cycle_detection を用いて
//   before_cycle : A_0 ... A_{mu-1}
//   cycle        : A_mu ... A_{mu+lam-1}
// を vector として返す
template <class F, class T>
std::pair<std::vector<T>, std::vector<T>> cycle_detection_lists(F f, T x0) {
    // ループの開始位置 mu と周期 lam を取得
    auto [mu, lam] = cycle_detection(f, x0);

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

// f    : 次の要素を返す関数
// x0   : 初期値 A_0
// n    : A_0 から A_{n-1} までを使う
// op   : 結合的な二項演算（モノイド）
// e    : op の単位元
//
// 戻り値:
//   op(A_0, A_1, ..., A_{n-1})
template <class F, class T, class Op>
T calc_cycle_problem_answer(F f, T x0, long long n, Op op, T e) {
    // ループ開始位置 mu と周期 lam を取得
    auto [mu, lam] = cycle_detection(f, x0);

    T ret = e;    // 最終的な答え
    T curr = x0;  // 現在の A_i

    // ===== ループに入る前 (A_0 ... A_{mu-1}) =====
    // n が小さい場合はここで終わる
    long long take = std::min(n, mu);
    for (long long i = 0; i < take; ++i) {
        ret = op(ret, curr);
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
    T per_cycles = e;

    // 1 周分を計算しつつ、余り分を ret に足す
    for (long long i = 1; i <= lam; ++i) {
        per_cycles = op(per_cycles, curr);

        // remainder 個分だけ先に ret に反映
        if (i == remainder) {
            ret = op(ret, per_cycles);
        }

        curr = f(curr);
    }

    // ===== 周回数を二乗法でまとめる =====
    // loop_total = op(per_cycles を loop_count 回)
    T loop_total = e;
    while (loop_count > 0) {
        if (loop_count & 1) {
            loop_total = op(loop_total, per_cycles);
        }
        per_cycles = op(per_cycles, per_cycles);
        loop_count >>= 1;
    }

    // 最後に全体へ反映
    ret = op(ret, loop_total);
    return ret;
}
