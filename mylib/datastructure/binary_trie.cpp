// Binary Trie (XOR Trie) - [0, 2^B) の整数を扱う（多重集合）
// 計算量: 各操作 O(B)、xor_all は O(1)
template <int B = 30>
class BinaryTrie {
    struct Node {
        int ch[2] = {-1, -1};
        int cnt = 0;
    };
    vector<Node> pool;
    int root;
    long long lazy = 0;  // 全要素に XOR されている値

    int new_node() {
        pool.push_back({});
        return (int)pool.size() - 1;
    }

    void add(long long x, int d) {
        x ^= lazy;  // 実際に格納するキーに変換
        int v = root;
        pool[v].cnt += d;
        for (int i = B - 1; i >= 0; i--) {
            int b = (x >> i) & 1;
            if (pool[v].ch[b] == -1) {
                int c = new_node();
                pool[v].ch[b] = c;
            }
            v = pool[v].ch[b];
            pool[v].cnt += d;
        }
    }

   public:
    BinaryTrie() {
        pool.reserve(1 << 20);
        root = new_node();
    }

    // x を1つ挿入する
    void insert(long long x) { add(x, 1); }
    // x を1つ削除する（存在しなければ未定義）
    void erase(long long x) { add(x, -1); }
    // 全要素に v を XOR する（O(1)）
    void xor_all(long long v) { lazy ^= v; }
    // 要素の総数を返す
    int size() const { return pool[root].cnt; }

    // x の個数を返す
    int count(long long x) const {
        int v = root;
        x ^= lazy;
        for (int i = B - 1; i >= 0; i--) {
            int b = (x >> i) & 1;
            if (pool[v].ch[b] == -1) return 0;
            v = pool[v].ch[b];
        }
        return pool[v].cnt;
    }
    // x が存在するか判定する
    bool find(long long x) const { return count(x) > 0; }

    // x との XOR が最小になる要素を返す（空のとき未定義）
    long long min_xor(long long x) const {
        int v = root;
        long long res = 0;
        long long q = x ^ lazy;  // 格納キーに対して最小化する対象
        for (int i = B - 1; i >= 0; i--) {
            int b = (q >> i) & 1;
            int prefer = b;
            if (pool[v].ch[prefer] == -1 || pool[pool[v].ch[prefer]].cnt == 0)
                prefer ^= 1;
            res |= ((long long)prefer << i);
            v = pool[v].ch[prefer];
        }
        return res ^ lazy;  // 格納キー → 論理値に変換
    }
    // x との XOR が最大になる要素を返す（空のとき未定義）
    long long max_xor(long long x) const {
        int v = root;
        long long res = 0;
        long long q = x ^ lazy;
        for (int i = B - 1; i >= 0; i--) {
            int b = (q >> i) & 1;
            int prefer = b ^ 1;
            if (pool[v].ch[prefer] == -1 || pool[pool[v].ch[prefer]].cnt == 0)
                prefer ^= 1;
            res |= ((long long)prefer << i);
            v = pool[v].ch[prefer];
        }
        return res ^ lazy;
    }

    // {a XOR x | a in trie} の昇順 k 番目（0-indexed）を返す（k >= size() のとき未定義）
    // x = 0 のとき要素をそのまま昇順で返す
    long long kth(long long k, long long x = 0) const {
        // a = 格納キー ^ lazy なので {a XOR x} = {格納キー XOR (lazy ^ x)}
        long long xp = lazy ^ x;
        int v = root;
        long long res = 0;
        for (int i = B - 1; i >= 0; i--) {
            int xb = (xp >> i) & 1;
            // ch[xb]   → XOR 結果ビット 0
            // ch[xb^1] → XOR 結果ビット 1
            int cnt0 = (pool[v].ch[xb] != -1) ? pool[pool[v].ch[xb]].cnt : 0;
            if (k < cnt0) {
                v = pool[v].ch[xb];
            } else {
                k -= cnt0;
                res |= ((long long)1 << i);
                v = pool[v].ch[xb ^ 1];
            }
        }
        return res;  // = 格納キー XOR xp = 論理値 XOR x
    }
    // {a XOR x | a in trie} のうち q 未満の個数を返す
    // x = 0 のとき要素をそのまま比較する
    long long lower_bound(long long q, long long x = 0) const {
        if (q <= 0) return 0;
        if (q >= (1LL << B)) return size();
        long long xp = lazy ^ x;
        int v = root;
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
    long long upper_bound(long long q, long long x = 0) const { return lower_bound(q + 1, x); }
};
