// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/set/interval_set.cpp
//
// IntervalSet
//
// 「互いに重ならない半開区間 [l, r) の集合」を set で管理するライブラリ。
// 各区間には値 `val` を持たせることもできる。
//
// 何ができるか:
// - 点 p を含む区間を探す
// - [l, r) が 1 本の区間で丸ごと覆われているか調べる
// - 2 点 p, q が同じ区間に属するか調べる
// - 区間 [l, r) を挿入する
// - 区間 [l, r) を削除する
// - 区間 [l, r) を値 val で塗り替える
// - mex を求める
//
// 基本イメージ:
//   例: {[1, 3), [7, 11)} を持っているとする。
//
//   insert(3, 7) をすると
//     -> {[1, 11)} にマージされる
//
//   erase(2, 8) をすると
//     -> {[1, 2), [8, 11)} に分割される
//
//   assign(2, 8, x) をすると
//     -> [2, 8) を値 x の 1 区間として上書きする
//
// `VAL` を使う用途:
// - 同じ値を持つ連続区間をまとめて管理したい
// - 区間を消す/追加するたびに、別の集計値も更新したい
//
// たとえば
// - 「色ごとの長さ」
// - 「区間ごとの寄与の総和」
// - 「隣接区間の境界数」
// などを add/del で同期できる。
//
// 重要:
//   `assign / insert / erase` に渡す `add` と `del` は
//   「互いに逆操作」である必要がある。
//
// なぜか:
//   たとえば現在の集合が {[1, 3), [7, 11)} で、erase(5, 9) すると、
//   実際に起きる内部処理は
//     del(7, 11)
//     add(9, 11)
//   のような「いったん元区間を消して、残る部分を再追加する」形になる。
//
//   つまり「消えた部分だけ del する」のではなく、
//   「触った既存区間を一度まるごと del して、残る区間を add し直す」
//   実装になっている。
//
//   そのため add / del が逆操作でないと、外部で持っている集計値が壊れる。
//
// 計算量の感覚:
// - 1 回の操作は、触った区間数を k としてだいたい O(k log N)
// - 連続する同値区間をまとめて持つので、愚直に全要素を触るよりかなり軽い
//
// 典型用途:
// - 値が同じ連続区間の管理
// - 塗りつぶし / 削除 / mex 管理
// - 「区間をまとめて持って、必要な境界だけ更新する」問題
//
// 検証済み
//   第六回 アルゴリズム実技検定 M - 等しい数（assign の検証）
//     https://atcoder.jp/contests/past202104-open/tasks/past202104_m
//
//   RUPC 2018 G - Elevator（insert、same の検証）
//     https://onlinejudge.u-aizu.ac.jp/problems/2880
//
//   AtCoder ABC 255 Ex - Range Harvest Query（assign の検証）
//     https://atcoder.jp/contests/abc255/tasks/abc255_h
//
//   yukicoder No.674 n連勤（insert の検証）
//     https://yukicoder.me/problems/no/674
//
//   AtCoder ABC 330 E - Mex and Update（insert、erase、mex の検証）
//     https://atcoder.jp/contests/abc330/tasks/abc330_e
//
//   第五回 アルゴリズム実技検定 N - 旅行会社（insert、erase、lower_bound の検証）
//     https://atcoder.jp/contests/past202012-open/tasks/past202012_n
//
//   Code Festival 2015 予選 B D - マスと駒と色塗り（insert、lower_bound の検証）
//     https://atcoder.jp/contests/code-festival-2015-qualb/tasks/codefestival_2015_qualB_d
//
//   CPSCO 2019 Session 1 E - Exclusive OR Queries（del 付き insert、lower_bound の検証）
//     https://atcoder.jp/contests/cpsco2019-s1/tasks/cpsco2019_s1_e
//
//   yukicoder No.3017 交互浴（assign の検証）
//     https://yukicoder.me/problems/no/3017
//
//   AtCoder ABC 256 Ex - I like Query Problem
//     https://atcoder.jp/contests/abc256/tasks/abc256_h
//
//   KUPC 2018 I - League of Kyoto（add、del 付き insert、erase の検証。erase の強いテストと思われる）
//     https://atcoder.jp/contests/kupc2018/tasks/kupc2018_i
//

#include <bits/stdc++.h>
using namespace std;

// 区間集合
// T: 座標の型、VAL: 区間に持たせる値の型
//
// 常に次の不変条件を保つ。
// - 各 Node は空でない半開区間 [l, r) を表す。
// - 区間同士は交わらない。
// - 隣接して値も等しい区間は存在しない（1 区間に併合される）。
//
// 値が不要な単なる区間集合として使う場合は IntervalSet<T> とし、
// insert / erase / contains / mex を使えばよい。
template <class T, class VAL = long long>
struct IntervalSet {
    struct Node {
        T l, r;
        VAL val;
        Node(const T &l, const T &r, const VAL &val) : l(l), r(r), val(val) {}
        constexpr bool operator<(const Node &rhs) const {
            if (l != rhs.l)
                return l < rhs.l;
            else
                return r < rhs.r;
        }
        friend ostream &operator<<(ostream &s, const Node &e) { return s << "([" << e.l << ", " << e.r << "): " << e.val << ")"; }
    };

    using iterator = typename set<Node>::iterator;
    using const_iterator = typename set<Node>::const_iterator;

    // 区間外で value_at を呼んだときに返す値。
    const VAL identity;
    set<Node> S;

  private:
    // 左端が x の Node のうち最初のものを探すための検索キー。
    // r = 0 とすると、負の座標を扱う場合に同じ左端の Node を
    // 飛び越すことがあるため lowest() を使う。
    Node lower_key(const T &x) const { return Node(x, numeric_limits<T>::lowest(), identity); }

  public:

    // コンストラクタ。配列から構築すると、同じ値が連続する部分をまとめる。
    IntervalSet(const VAL &identity = VAL()) : identity(identity) {}
    IntervalSet(const vector<VAL> &v, const VAL &identity = VAL()) : identity(identity) {
        vector<Node> vec;
        for (int l = 0; l < (int)v.size();) {
            int r = l;
            while (r < (int)v.size() && v[r] == v[l]) r++;
            vec.emplace_back(l, r, v[l]);
            l = r;
        }
        S = set<Node>(vec.begin(), vec.end());
    }

    // std::set と同様に左端の昇順で走査する。
    iterator begin() { return S.begin(); }
    iterator end() { return S.end(); }
    const_iterator begin() const { return S.begin(); }
    const_iterator end() const { return S.end(); }
    const_iterator cbegin() const { return S.cbegin(); }
    const_iterator cend() const { return S.cend(); }
    bool empty() const { return S.empty(); }
    size_t size() const { return S.size(); }  // 点の個数ではなく、保持中の区間数
    void clear() { S.clear(); }

    // 点 p を含む区間を返す。存在しなければ end() を返す。
    const_iterator get(const T &p) const {
        auto it = S.upper_bound(Node(p, numeric_limits<T>::max(), identity));
        if (it == S.begin()) return S.end();
        it = prev(it);
        if (it->l <= p && p < it->r)
            return it;
        else
            return S.end();
    }

    // 点 p を含む区間、なければ p より右にある最初の区間を返す。
    const_iterator lower_bound(const T &p) const {
        auto it = get(p);
        if (it != S.end()) return it;
        return S.upper_bound(Node(p, numeric_limits<T>::max(), identity));
    }

    // 点 p がいずれかの区間に含まれるかを返す。
    constexpr bool contains(const T &p) const {
        auto it = get(p);
        if (it != S.end())
            return true;
        else
            return false;
    }

    // 半開区間 [l, r) 全体が1本の区間に含まれるかを返す。
    // 空区間は常に true とする。
    constexpr bool contains(const T &l, const T &r) const {
        assert(l <= r);
        if (l == r) return true;
        auto it = get(l);
        if (it != S.end() && r <= it->r)
            return true;
        else
            return false;
    }

    // 2点 p, q が同じ区間に含まれるかを返す。
    constexpr bool same(const T &p, const T &q) const {
        if (!contains(p) || !contains(q)) return false;
        return get(p) == get(q);
    }

    // 点 p を含む区間の値を返す。区間外なら identity を返す。
    constexpr VAL value_at(const T &p) const {
        auto it = get(p);
        if (it != S.end())
            return it->val;
        else
            return identity;
    }
    VAL operator[](const T &p) const { return value_at(p); }

    // p 以上で、どの区間にも含まれない最小の座標を返す。
    constexpr T mex(const T &p = 0) const {
        auto it = get(p);
        if (it == S.end()) return p;
        T res = it->r;
        for (++it; it != S.end() && it->l == res; ++it) res = it->r;
        return res;
    }

    // 半開区間 [l, r) を値 val で上書きする。
    // del は区間削除、add は区間追加の影響を外部集計へ反映する。
    // add と del は互いに逆の操作でなければならない。
    template <class ADDFUNC, class DELFUNC>
    void assign(T l, T r, const VAL &val, const ADDFUNC &add, const DELFUNC &del) {
        assert(l <= r);
        if (l == r) return;
        auto it = S.lower_bound(lower_key(l));
        while (it != S.end() && it->l <= r) {
            if (it->l == r) {
                if (it->val == val) {
                    r = it->r;
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                }
                break;
            }
            if (it->r <= r) {
                del(it->l, it->r, it->val);
                it = S.erase(it);
            } else {
                if (it->val == val) {
                    r = it->r;
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                } else {
                    Node node = *it;
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                    it = S.emplace_hint(it, r, node.r, node.val);
                    add(it->l, it->r, it->val);
                }
            }
        }
        if (it != S.begin()) {
            it = prev(it);
            if (it->r == l) {
                if (it->val == val) {
                    l = it->l;
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                }
            } else if (l < it->r) {
                if (it->val == val) {
                    l = min(l, it->l);
                    r = max(r, it->r);
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                } else {
                    if (r < it->r) {
                        it = S.emplace_hint(next(it), r, it->r, it->val);
                        add(it->l, it->r, it->val);
                        it = prev(it);
                    }
                    Node node = *it;
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                    it = S.emplace_hint(it, node.l, l, node.val);
                    add(it->l, it->r, it->val);
                }
            }
        }
        if (it != S.end()) it = next(it);
        it = S.emplace_hint(it, l, r, val);
        add(it->l, it->r, it->val);
    }
    void assign(const T &l, const T &r, const VAL &val) {
        assign(l, r, val, [](T, T, VAL) {}, [](T, T, VAL) {});
    }
    template <class ADDFUNC, class DELFUNC>
    void insert(T l, T r, const ADDFUNC &add, const DELFUNC &del) {
        assign(l, r, identity, add, del);
    }
    void insert(const T &l, const T &r) {
        assign(l, r, identity, [](T, T, VAL) {}, [](T, T, VAL) {});
    }

    // 半開区間 [l, r) を削除する。
    // del は区間削除、add は残った区間の再追加を外部集計へ反映する。
    // add と del は互いに逆の操作でなければならない。
    template <class ADDFUNC, class DELFUNC>
    void erase(T l, T r, const ADDFUNC &add, const DELFUNC &del) {
        assert(l <= r);
        if (l == r) return;
        auto it = S.lower_bound(lower_key(l));
        while (it != S.end() && it->l <= r) {
            if (it->l == r) break;
            if (it->r <= r) {
                del(it->l, it->r, it->val);
                it = S.erase(it);
            } else {
                Node node = *it;
                del(it->l, it->r, it->val);
                it = S.erase(it);
                it = S.emplace_hint(it, r, node.r, node.val);
                add(it->l, it->r, it->val);
            }
        }
        if (it != S.begin()) {
            it = prev(it);
            if (l < it->r) {
                if (r < it->r) {
                    it = S.emplace_hint(next(it), r, it->r, it->val);
                    add(it->l, it->r, it->val);
                    it = prev(it);
                }
                Node node = *it;
                del(it->l, it->r, it->val);
                it = S.erase(it);
                it = S.emplace_hint(it, node.l, l, node.val);
                add(it->l, it->r, it->val);
            }
        }
    }
    void erase(const T &l, const T &r) {
        erase(l, r, [](T, T, VAL) {}, [](T, T, VAL) {});
    }

    // デバッグ出力
    friend ostream &operator<<(ostream &s, const IntervalSet &ins) {
        for (auto e : ins.S) {
            s << "([" << e.l << ", " << e.r << "): " << e.val << ") ";
        }
        return s;
    }
};

#if 0
// 検証用プログラム。再利用するライブラリ本体には含めない。

// 第六回 アルゴリズム実技検定 M - 等しい数
void PAST_6_M() {
    long long N, Q;
    cin >> N;
    vector<long long> A(N);
    map<long long, long long> cnt;
    for (int i = 0; i < N; i++) {
        cin >> A[i];
        cnt[A[i]]++;
    }
    long long res = 0;
    for (auto [val, num] : cnt) res += num * (num - 1) / 2;

    auto add = [&](int l, int r, long long val) -> void {
        long long before = cnt[val] * (cnt[val] - 1) / 2;
        cnt[val] += r - l;
        long long after = cnt[val] * (cnt[val] - 1) / 2;
        res += after - before;
    };
    auto del = [&](int l, int r, long long val) -> void {
        long long before = cnt[val] * (cnt[val] - 1) / 2;
        cnt[val] -= r - l;
        long long after = cnt[val] * (cnt[val] - 1) / 2;
        res += after - before;
    };
    IntervalSet<int, long long> ins(A);

    cin >> Q;
    while (Q--) {
        int l, r;
        long long val;
        cin >> l >> r >> val;
        l--;
        ins.assign(l, r, val, add, del);
        cout << res << '\n';
    }
}

// RUPC 2018 G - Elevator
void RUPC_2018_G() {
    using fll = array<long long, 4>;
    int N, M, Q, t, l, r;
    cin >> N >> M >> Q;
    vector<fll> qs(M + Q);
    for (int i = 0; i < M; i++) {
        cin >> t >> l >> r, l--, r--;
        qs[i] = fll({-1, t * 2 + 1, l * 2, r * 2 + 1});
    }
    for (int i = 0; i < Q; i++) {
        cin >> t >> l >> r, l--, r--;
        qs[i + M] = fll({i, t * 2, l * 2, r * 2});
    }
    sort(qs.begin(), qs.end(), [&](const fll &p, const fll &q) { return p[1] < q[1]; });

    IntervalSet<long long> ins;
    vector<bool> res(Q, false);
    for (auto q : qs) {
        if (q[0] == -1) {
            ins.insert(q[2], q[3]);
        } else {
            if (q[2] >= q[3] || ins.same(q[2], q[3]))
                res[q[0]] = true;
            else
                res[q[0]] = false;
        }
    }
    for (int q = 0; q < Q; ++q) cout << (res[q] ? "Yes" : "No") << endl;
}

// AtCoder ABC 255 Ex - Range Harvest Query
// modint
template <int MOD>
struct Fp {
    // 内部の値
    long long val;

    // コンストラクタ
    constexpr Fp() : val(0) {}
    constexpr Fp(long long v) : val(v % MOD) {
        if (val < 0) val += MOD;
    }
    constexpr long long get() const { return val; }
    constexpr int get_mod() const { return MOD; }

    // 算術演算子
    constexpr Fp operator+() const { return Fp(*this); }
    constexpr Fp operator-() const { return Fp(0) - Fp(*this); }
    constexpr Fp operator+(const Fp &r) const { return Fp(*this) += r; }
    constexpr Fp operator-(const Fp &r) const { return Fp(*this) -= r; }
    constexpr Fp operator*(const Fp &r) const { return Fp(*this) *= r; }
    constexpr Fp operator/(const Fp &r) const { return Fp(*this) /= r; }
    constexpr Fp &operator+=(const Fp &r) {
        val += r.val;
        if (val >= MOD) val -= MOD;
        return *this;
    }
    constexpr Fp &operator-=(const Fp &r) {
        val -= r.val;
        if (val < 0) val += MOD;
        return *this;
    }
    constexpr Fp &operator*=(const Fp &r) {
        val = val * r.val % MOD;
        return *this;
    }
    constexpr Fp &operator/=(const Fp &r) {
        long long a = r.val, b = MOD, u = 1, v = 0;
        while (b) {
            long long t = a / b;
            a -= t * b, swap(a, b);
            u -= t * v, swap(u, v);
        }
        val = val * u % MOD;
        if (val < 0) val += MOD;
        return *this;
    }
    constexpr Fp pow(long long n) const {
        Fp res(1), mul(*this);
        while (n > 0) {
            if (n & 1) res *= mul;
            mul *= mul;
            n >>= 1;
        }
        return res;
    }
    constexpr Fp inv() const {
        Fp res(1), div(*this);
        return res / div;
    }

    // その他の演算子
    constexpr bool operator==(const Fp &r) const { return this->val == r.val; }
    constexpr bool operator!=(const Fp &r) const { return this->val != r.val; }
    constexpr Fp &operator++() {
        ++val;
        if (val >= MOD) val -= MOD;
        return *this;
    }
    constexpr Fp &operator--() {
        if (val == 0) val += MOD;
        --val;
        return *this;
    }
    constexpr Fp operator++(int) const {
        Fp res = *this;
        ++*this;
        return res;
    }
    constexpr Fp operator--(int) const {
        Fp res = *this;
        --*this;
        return res;
    }
    friend constexpr istream &operator>>(istream &is, Fp<MOD> &x) {
        is >> x.val;
        x.val %= MOD;
        if (x.val < 0) x.val += MOD;
        return is;
    }
    friend constexpr ostream &operator<<(ostream &os, const Fp<MOD> &x) { return os << x.val; }
    friend constexpr Fp<MOD> pow(const Fp<MOD> &r, long long n) { return r.pow(n); }
    friend constexpr Fp<MOD> inv(const Fp<MOD> &r) { return r.inv(); }
};
void ABC_255_Ex() {
    const int MOD = 998244353;
    const long long INF = 1LL << 60;
    using mint = Fp<MOD>;

    long long N, Q, D, L, R;
    cin >> N >> Q;
    mint res = 0;
    auto add = [&](long long l, long long r, long long val) -> void { res -= mint(D - val) * (l + r - 1) * (r - l) / 2; };
    auto del = [&](long long l, long long r, long long val) -> void { res += mint(D - val) * (l + r - 1) * (r - l) / 2; };
    IntervalSet<long long, long long> ins;
    ins.assign(0, INF, 0);
    while (Q--) {
        cin >> D >> L >> R;
        res = 0;
        ins.assign(L, R + 1, D, add, del);
        cout << res << '\n';
    }
}

// yukicoder No.674 n連勤
void yukicoder_674() {
    long long D, Q, A, B;
    cin >> D >> Q;
    IntervalSet<long long> ins;
    long long res = 0;
    while (Q--) {
        cin >> A >> B;
        ins.insert(A, B + 1);
        auto it = ins.get(A);
        long long l = it->l, r = it->r;
        res = max(res, r - l);
        cout << res << '\n';
    }
}

// AtCoder ABC 330 E - Mex and Update
void ABC_330_E() {
    int N, Q, i, x;
    cin >> N >> Q;
    vector<int> A(N);
    map<int, int> mp;
    IntervalSet<int> ins;
    for (int i = 0; i < N; i++) {
        cin >> A[i];
        mp[A[i]]++;
        ins.insert(A[i], A[i] + 1);
    }
    while (Q--) {
        cin >> i >> x;
        i--;
        mp[A[i]]--;
        if (mp[A[i]] == 0) ins.erase(A[i], A[i] + 1);
        A[i] = x;
        mp[A[i]]++;
        ins.insert(A[i], A[i] + 1);
        cout << ins.mex() << '\n';
    }
}

// 第五回 アルゴリズム実技検定 N - 旅行会社
void PAST_5_N() {
    const int IN = -2;
    const int OUT = -1;
    using tint = array<int, 3>;  // (time, type(0, 1, 2), city)
    int N, Q;
    cin >> N >> Q;
    vector<tint> events;
    for (int i = 0; i < N - 1; i++) {
        int L, R;
        cin >> L >> R;
        L--;
        events.push_back({L, IN, i});
        events.push_back({R, OUT, i});
    }
    for (int i = 0; i < Q; i++) {
        int A, B;
        cin >> A >> B;
        A--, B--;
        events.push_back({A, i, B});
    }
    sort(events.begin(), events.end());

    IntervalSet<int> ins;
    vector<int> res(Q);
    for (auto [time, id, city] : events) {
        if (id == IN) {
            ins.insert(city, city + 1);
        } else if (id == OUT) {
            ins.erase(city, city + 1);
        } else {
            int ma = city, mi = city;
            auto it = ins.lower_bound(city);
            if (it != ins.end()) {
                if (city >= it->l) ma = max(ma, it->r), mi = min(mi, it->l);
            }
            if (it != ins.begin()) {
                it = prev(it);
                if (city <= it->r) mi = min(mi, it->l);
            }
            res[id] = ma - mi + 1;
        }
    }
    for (auto val : res) cout << val << '\n';
}

// Code Festival 2015 予選 B D - マスと駒と色塗り（insert、lower_bound の検証）
void code_festival_2015_B_D() {
    long long N, S, C;
    cin >> N;

    const long long INF = 1LL << 60;
    IntervalSet<long long> ins;
    ins.insert(INF, INF * 2);
    while (N--) {
        cin >> S >> C;
        S--;
        long long cur = S;
        while (C > 0) {
            auto it = ins.lower_bound(cur);
            if (cur >= it->l)
                cur = it->r;
            else {
                long long diff = it->l - cur;
                cur += min(diff, C);
                C -= min(diff, C);
            }
        }
        ins.insert(S, cur);
        cout << cur << '\n';
    }
}

// CPSCO 2019 Session 1 E - Exclusive OR Queries
void cpsco_2019_session_1_E() {
    long long N, Q, A, L, R, X;
    cin >> N >> Q;
    set<long long> se;
    for (int i = 0; i < N; i++) {
        cin >> A;
        if (se.count(A))
            se.erase(A);
        else
            se.insert(A);
    }
    long long res = 0, num = 0;
    auto add = [&](long long l, long long r, long long val) -> void {
        for (long long k = l; k < r; k++) res ^= k, num--;
    };
    auto del = [&](long long l, long long r, long long val) -> void {
        for (long long k = l; k < r; k++) res ^= k, num++;
    };
    IntervalSet<long long, long long> ins;
    for (auto val : se) ins.assign(val, val + 1, 1);
    while (Q--) {
        cin >> L >> R >> X;
        R++;
        res = 0, num = 0;
        ins.erase(L, R, add, del);
        cout << res << '\n';
        int exist = ins.contains(X);
        if ((num + exist) % 2 == 1)
            ins.assign(X, X + 1, 1);
        else
            ins.erase(X, X + 1);
    }
}

// yukicoder No.3017 交互浴
void yukicoder_3017() {
    long long N, H;
    cin >> N;

    long long res = 0;
    auto add = [&](long long l, long long r, long long val) -> void { res += (r - l) * val; };
    auto del = [&](long long l, long long r, long long val) -> void { res -= (r - l) * val; };
    IntervalSet<long long, long long> ins;
    for (int iter = 0; iter < N; iter++) {
        cin >> H;
        if (iter % 2 == 0)
            ins.assign(0, H, 1, add, del);
        else
            ins.assign(0, H, 0, add, del);
        cout << res << '\n';
    }
}

// AtCoder ABC 256 Ex - I like Query Problem
#include "atcoder/lazysegtree.hpp"
using ll = long long;
using pl = pair<ll, ll>;
pl ti() { return {0, 0}; }
ll ei() { return -1; }
pl f(pl a, pl b) { return pl{a.first + b.first, a.second + b.second}; }
pl g(ll b, pl a) { return b == ei() ? a : pl{a.second * b, a.second}; }
ll h(ll b, ll a) { return b == ei() ? a : b; }
using segtree = atcoder::lazy_segtree<pl, f, ti, ll, g, h, ei>;
void ABC_256_Ex() {
    int N, Q, type, L, R, x, y;
    cin >> N >> Q;
    vector<int> A(N);
    vector<pl> PA(N);
    for (int i = 0; i < N; i++) scanf("%d", &A[i]), PA[i] = {A[i], 1};
    segtree seg(PA);
    IntervalSet<int, int> ins(A);
    while (Q--) {
        cin >> type;
        if (type == 1) {
            scanf("%d %d %d", &L, &R, &x);
            L--;
            for (int v = L; v < R;) {
                auto it = ins.lower_bound(v);
                if (it == ins.end()) break;
                v = max(v, it->l);
                if (v >= R) break;
                int r = min(it->r, R);
                int before = it->val;
                int after = before / x;
                if (after == 0)
                    ins.erase(v, r);
                else
                    ins.assign(v, r, after);
                seg.apply(v, r, after);
                v = r;
            }
        } else if (type == 2) {
            scanf("%d %d %d", &L, &R, &y);
            L--;
            ins.assign(L, R, y);
            seg.apply(L, R, y);
        } else {
            scanf("%d %d", &L, &R);
            L--;
            ll res = seg.prod(L, R).first;
            printf("%lld\n", res);
        }
    }
}

// KUPC 2018 I - League of Kyoto
template <class Monoid, class Action>
struct LazySegmentTree {
    // 各種関数の型
    using FuncMonoid = function<Monoid(Monoid, Monoid)>;
    using FuncAction = function<Monoid(Action, Monoid)>;
    using FuncComposition = function<Action(Action, Action)>;

    // 基本設定
    int N;
    FuncMonoid OP;
    FuncAction ACT;
    FuncComposition COMP;
    Monoid IDENTITY_MONOID;
    Action IDENTITY_ACTION;

    // 内部データ
    int log, offset;
    vector<Monoid> dat;
    vector<Action> lazy;

    // コンストラクタ
    LazySegmentTree() {}
    LazySegmentTree(int n, const FuncMonoid op, const FuncAction act, const FuncComposition comp, const Monoid &identity_monoid,
                    const Action &identity_action) {
        init(n, op, act, comp, identity_monoid, identity_action);
    }
    LazySegmentTree(const vector<Monoid> &v, const FuncMonoid op, const FuncAction act, const FuncComposition comp, const Monoid &identity_monoid,
                    const Action &identity_action) {
        init(v, op, act, comp, identity_monoid, identity_action);
    }
    void init(int n, const FuncMonoid op, const FuncAction act, const FuncComposition comp, const Monoid &identity_monoid, const Action &identity_action) {
        N = n, OP = op, ACT = act, COMP = comp;
        IDENTITY_MONOID = identity_monoid, IDENTITY_ACTION = identity_action;
        log = 0, offset = 1;
        while (offset < N) ++log, offset <<= 1;
        dat.assign(offset * 2, IDENTITY_MONOID);
        lazy.assign(offset * 2, IDENTITY_ACTION);
    }
    void init(const vector<Monoid> &v, const FuncMonoid op, const FuncAction act, const FuncComposition comp, const Monoid &identity_monoid,
              const Action &identity_action) {
        init((int)v.size(), op, act, comp, identity_monoid, identity_action);
        build(v);
    }
    void build(const vector<Monoid> &v) {
        assert(N == (int)v.size());
        for (int i = 0; i < N; ++i) dat[i + offset] = v[i];
        for (int k = offset - 1; k > 0; --k) pull_dat(k);
    }
    int size() const { return N; }

    // 遅延セグメント木の基本操作
    void pull_dat(int k) { dat[k] = OP(dat[k * 2], dat[k * 2 + 1]); }
    void apply_lazy(int k, const Action &f) {
        dat[k] = ACT(f, dat[k]);
        if (k < offset) lazy[k] = COMP(f, lazy[k]);
    }
    void push_lazy(int k) {
        apply_lazy(k * 2, lazy[k]);
        apply_lazy(k * 2 + 1, lazy[k]);
        lazy[k] = IDENTITY_ACTION;
    }
    void pull_dat_deep(int k) {
        for (int h = 1; h <= log; ++h) pull_dat(k >> h);
    }
    void push_lazy_deep(int k) {
        for (int h = log; h >= 1; --h) push_lazy(k >> h);
    }

    // 0-indexed の A[i] を設定・取得する。計算量は O(log N)。
    void set(int i, const Monoid &v) {
        assert(0 <= i && i < N);
        int k = i + offset;
        push_lazy_deep(k);
        dat[k] = v;
        pull_dat_deep(k);
    }
    Monoid get(int i) {
        assert(0 <= i && i < N);
        int k = i + offset;
        push_lazy_deep(k);
        return dat[k];
    }
    Monoid operator[](int i) { return get(i); }

    // 添字 i の要素に作用 f を適用する。
    void apply(int i, const Action &f) {
        assert(0 <= i && i < N);
        int k = i + offset;
        push_lazy_deep(k);
        dat[k] = ACT(f, dat[k]);
        pull_dat_deep(k);
    }
    // 半開区間 [l, r) に作用 f を適用する。
    void apply(int l, int r, const Action &f) {
        assert(0 <= l && l <= r && r <= N);
        if (l == r) return;
        l += offset, r += offset;
        for (int h = log; h >= 1; --h) {
            if (((l >> h) << h) != l) push_lazy(l >> h);
            if (((r >> h) << h) != r) push_lazy((r - 1) >> h);
        }
        int original_l = l, original_r = r;
        for (; l < r; l >>= 1, r >>= 1) {
            if (l & 1) apply_lazy(l++, f);
            if (r & 1) apply_lazy(--r, f);
        }
        l = original_l, r = original_r;
        for (int h = 1; h <= log; ++h) {
            if (((l >> h) << h) != l) pull_dat(l >> h);
            if (((r >> h) << h) != r) pull_dat((r - 1) >> h);
        }
    }

    // 半開区間 [l, r) の総積を返す。
    Monoid prod(int l, int r) {
        assert(0 <= l && l <= r && r <= N);
        if (l == r) return IDENTITY_MONOID;
        l += offset, r += offset;
        for (int h = log; h >= 1; --h) {
            if (((l >> h) << h) != l) push_lazy(l >> h);
            if (((r >> h) << h) != r) push_lazy(r >> h);
        }
        Monoid val_left = IDENTITY_MONOID, val_right = IDENTITY_MONOID;
        for (; l < r; l >>= 1, r >>= 1) {
            if (l & 1) val_left = OP(val_left, dat[l++]);
            if (r & 1) val_right = OP(dat[--r], val_right);
        }
        return OP(val_left, val_right);
    }
    Monoid all_prod() { return dat[1]; }

    // f(get(l, r)) が true となる最大の r を返す。0-indexed、O(log N)。
    // f(IDENTITY) は true でなければならない。
    int max_right(const function<bool(Monoid)> f, int l = 0) {
        if (l == N) return N;
        l += offset;
        push_lazy_deep(l);
        Monoid sum = IDENTITY_MONOID;
        do {
            while (l % 2 == 0) l >>= 1;
            if (!f(OP(sum, dat[l]))) {
                while (l < offset) {
                    push_lazy(l);
                    l = l * 2;
                    if (f(OP(sum, dat[l]))) {
                        sum = OP(sum, dat[l]);
                        ++l;
                    }
                }
                return l - offset;
            }
            sum = OP(sum, dat[l]);
            ++l;
        } while ((l & -l) != l);  // stop if l = 2^e
        return N;
    }

    // f(get(l, r)) が true となる最小の l を返す。0-indexed、O(log N)。
    // f(IDENTITY) は true でなければならない。
    int min_left(const function<bool(Monoid)> f, int r = -1) {
        if (r == 0) return 0;
        if (r == -1) r = N;
        r += offset;
        push_lazy_deep(r - 1);
        Monoid sum = IDENTITY_MONOID;
        do {
            --r;
            while (r > 1 && (r % 2)) r >>= 1;
            if (!f(OP(dat[r], sum))) {
                while (r < offset) {
                    push_lazy(r);
                    r = r * 2 + 1;
                    if (f(OP(dat[r], sum))) {
                        sum = OP(dat[r], sum);
                        --r;
                    }
                }
                return r + 1 - offset;
            }
            sum = OP(dat[r], sum);
        } while ((r & -r) != r);
        return 0;
    }

    // デバッグ出力
    friend ostream &operator<<(ostream &s, LazySegmentTree seg) {
        for (int i = 0; i < (int)seg.size(); ++i) {
            s << seg[i];
            if (i != (int)seg.size() - 1) s << " ";
        }
        return s;
    }

    // 内部状態の表示
    void dump() {
        for (int i = 0; i <= log; ++i) {
            for (int j = (1 << i); j < (1 << (i + 1)); ++j) {
                cout << "{" << dat[j] << "," << lazy[j] << "} ";
            }
            cout << endl;
        }
    }
};
void KUPC_2018_I() {
    int N, M, Q;
    cin >> N >> M;
    vector<int> L(M), R(M), S(M);
    for (int i = 0; i < M; i++) cin >> L[i] >> R[i] >> S[i], L[i]--;
    cin >> Q;
    vector<int> typ(Q), L2(Q), R2(Q);
    for (int q = 0; q < Q; q++) cin >> typ[q] >> L2[q] >> R2[q], L2[q]--;

    IntervalSet<int> ins;
    ins.insert(0, N);
    vector<vector<pair<int, int>>> adds(Q), dels(Q);
    int q = 0;
    auto add = [&](int l, int r, int v) -> void { dels[q].emplace_back(l, r); };
    auto del = [&](int l, int r, int v) -> void { adds[q].emplace_back(l, r); };
    for (; q < Q; q++) {
        if (typ[q] == 0)
            ins.insert(L2[q], R2[q], add, del);
        else
            ins.erase(L2[q], R2[q], add, del);
    }

    auto op = [&](long long p, long long q) -> long long { return max(p, q); };
    auto mapping = [&](long long f, long long v) -> long long { return v + f; };
    auto composition = [&](long long g, long long f) -> long long { return f + g; };
    LazySegmentTree<long long, long long> seg(N, op, mapping, composition, 0, 0);
    vector<vector<pair<int, int>>> enemy(N);
    for (int i = 0; i < M; i++) {
        enemy[L[i]].emplace_back(R[i], S[i]);
        seg.apply(R[i] - 1, N, S[i]);
    }
    map<pair<int, int>, long long> score;

    vector<vector<int>> inters(N);
    for (int q = 0; q < Q; q++) {
        for (auto [l, r] : adds[q]) inters[l].emplace_back(r);
        for (auto [l, r] : dels[q]) inters[l].emplace_back(r);
    }
    for (int l = 0; l < N; l++) {
        for (auto r : inters[l]) {
            score[make_pair(l, r)] = seg.prod(l, r);
        }
        for (auto [r, s] : enemy[l]) {
            seg.apply(r - 1, N, -s);
        }
    }

    long long res = 0;
    for (int q = 0; q < Q; q++) {
        for (auto [l, r] : adds[q]) res += score[make_pair(l, r)];
        for (auto [l, r] : dels[q]) res -= score[make_pair(l, r)];
        cout << res << '\n';
    }
}

int main() {
    // PAST_6_M();
    // RUPC_2018_G();
    // ABC_255_Ex();
    // yukicoder_674();
    // ABC_330_E();
    // PAST_5_N();
    // code_festival_2015_B_D();
    // cpsco_2019_session_1_E();
    // yukicoder_3017();
    ABC_256_Ex();
    // KUPC_2018_I();
}
#endif
