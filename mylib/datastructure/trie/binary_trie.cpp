// 最終更新: 2026-07-13 01:23:42 JST
// Binary Trie (XOR Trie) - [0, 2^B) の整数を扱う（多重集合）
//
// 使い方:
//   BinaryTrie<30> trie;
//   BinaryTrie<30> reserved(2'000'000); // 予想ノード数を予約
//
// 計算量:
//   insert / erase / count / min_xor / max_xor / kth / lower_bound: O(B)
//   xor_all / size / empty: O(1)
//   メモリ: O(作成されたノード数)。最大で O(挿入種類数 * B)
//
// 注意:
//   - erase(x) はxが存在する場合にだけ呼ぶ。
//   - コンストラクタの引数は要素数ではなく、予想ノード数。
template <int B = 30>
class BinaryTrie {
    static_assert(1 <= B && B <= 62);

    struct Node {
        int ch[2] = {-1, -1};
        int cnt = 0;
    };
    using U = unsigned long long;
    vector<Node> pool;
    U lazy = 0;  // 全要素に XOR されている値

    int new_node() {
        pool.push_back({});
        return (int)pool.size() - 1;
    }

   public:
    explicit BinaryTrie(size_t reserve_nodes = 1 << 20) {
        pool.reserve(reserve_nodes);
        new_node();
    }

    // x を1つ挿入する
    void insert(long long x) {
        U key = static_cast<U>(x) ^ lazy;
        int v = 0;
        ++pool[v].cnt;
        for (int i = B - 1; i >= 0; --i) {
            int bit = (key >> i) & 1;
            int next = pool[v].ch[bit];
            if (next == -1) {
                next = new_node();
                pool[v].ch[bit] = next;
            }
            v = next;
            ++pool[v].cnt;
        }
    }
    // x を1つ削除する（存在しなければ未定義）
    void erase(long long x) {
        U key = static_cast<U>(x) ^ lazy;
        int v = 0;
        --pool[v].cnt;
        for (int i = B - 1; i >= 0; --i) {
            v = pool[v].ch[(key >> i) & 1];
            --pool[v].cnt;
        }
    }
    // 全要素に v を XOR する（O(1)）
    void xor_all(long long v) { lazy ^= static_cast<U>(v); }
    // 要素の総数を返す
    int size() const { return pool[0].cnt; }
    bool empty() const { return size() == 0; }
    int node_count() const { return static_cast<int>(pool.size()); }
    void reserve_nodes(size_t n) { pool.reserve(n); }

    // x の個数を返す
    int count(long long x) const {
        int v = 0;
        U key = static_cast<U>(x) ^ lazy;
        for (int i = B - 1; i >= 0; i--) {
            int b = (key >> i) & 1;
            if (pool[v].ch[b] == -1) return 0;
            v = pool[v].ch[b];
        }
        return pool[v].cnt;
    }
    // x が存在するか判定する
    bool find(long long x) const { return count(x) > 0; }

    // x との XOR が最小になる要素を返す（空のとき未定義）
    long long min_xor(long long x) const {
        int v = 0;
        U res = 0;
        U q = static_cast<U>(x) ^ lazy;  // 格納キーに対して最小化する対象
        for (int i = B - 1; i >= 0; i--) {
            int b = (q >> i) & 1;
            int prefer = b;
            if (pool[v].ch[prefer] == -1 || pool[pool[v].ch[prefer]].cnt == 0)
                prefer ^= 1;
            res |= U(prefer) << i;
            v = pool[v].ch[prefer];
        }
        return static_cast<long long>(res ^ lazy);  // 格納キー → 論理値に変換
    }
    // x との XOR が最大になる要素を返す（空のとき未定義）
    long long max_xor(long long x) const {
        int v = 0;
        U res = 0;
        U q = static_cast<U>(x) ^ lazy;
        for (int i = B - 1; i >= 0; i--) {
            int b = (q >> i) & 1;
            int prefer = b ^ 1;
            if (pool[v].ch[prefer] == -1 || pool[pool[v].ch[prefer]].cnt == 0)
                prefer ^= 1;
            res |= U(prefer) << i;
            v = pool[v].ch[prefer];
        }
        return static_cast<long long>(res ^ lazy);
    }

    // {a XOR x | a in trie} の昇順 k 番目（0-indexed）を返す（k >= size() のとき未定義）
    // x = 0 のとき要素をそのまま昇順で返す
    long long kth(long long k, long long x = 0) const {
        // a = 格納キー ^ lazy なので {a XOR x} = {格納キー XOR (lazy ^ x)}
        U xp = lazy ^ static_cast<U>(x);
        int v = 0;
        U res = 0;
        for (int i = B - 1; i >= 0; i--) {
            int xb = (xp >> i) & 1;
            // ch[xb]   → XOR 結果ビット 0
            // ch[xb^1] → XOR 結果ビット 1
            int cnt0 = (pool[v].ch[xb] != -1) ? pool[pool[v].ch[xb]].cnt : 0;
            if (k < cnt0) {
                v = pool[v].ch[xb];
            } else {
                k -= cnt0;
                res |= U(1) << i;
                v = pool[v].ch[xb ^ 1];
            }
        }
        return static_cast<long long>(res);  // = 格納キー XOR xp = 論理値 XOR x
    }
    // {a XOR x | a in trie} のうち q 未満の個数を返す
    // x = 0 のとき要素をそのまま比較する
    long long lower_bound(long long q, long long x = 0) const {
        if (q <= 0) return 0;
        if (q >= (1LL << B)) return size();
        U xp = lazy ^ static_cast<U>(x);
        int v = 0;
        long long res = 0;
        for (int i = B - 1; i >= 0; i--) {
            int qb = (q >> i) & 1;
            int xb = (xp >> i) & 1;
            // ch[xb]   → XOR 結果ビット 0
            // ch[xb^1] → XOR 結果ビット 1
            if (qb == 1) {
                if (pool[v].ch[xb] != -1)
                    res += pool[pool[v].ch[xb]].cnt;
                if (pool[v].ch[xb ^ 1] == -1) break;
                v = pool[v].ch[xb ^ 1];
            } else {
                if (pool[v].ch[xb] == -1) break;
                v = pool[v].ch[xb];
            }
        }
        return res;
    }
    // {a XOR x | a in trie} のうち q 以下の個数を返す
    long long upper_bound(long long q, long long x = 0) const {
        if (q == numeric_limits<long long>::max()) return size();
        return lower_bound(q + 1, x);
    }
};
