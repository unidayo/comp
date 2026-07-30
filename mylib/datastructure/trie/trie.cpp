// 最終更新: 2026-07-12 14:27:44 JST
// Trie - 固定された連続文字集合上の文字列を扱う（多重集合）
// 計算量: 各操作 O(|s|)、空間計算量 O(挿入された文字列の長さの総和 * SIGMA)
template <int SIGMA = 26, char FIRST = 'a'>
class Trie {
    static_assert(SIGMA > 0);

    struct Node {
        array<int, SIGMA> next;
        // このノードに対応する文字列を接頭辞に持つ、登録文字列の個数（重複を含む）
        // 根の pass は Trie 全体の要素数になる
        int pass = 0;
        // このノードに対応する文字列そのものの登録個数（重複を含む）
        // 0 より大きければ、このノードが登録文字列の終端である
        int terminal = 0;

        Node() { next.fill(-1); }
    };

    vector<Node> nodes;
    int distinct_count = 0;

    static int index(char c) {
        int i = static_cast<unsigned char>(c) - static_cast<unsigned char>(FIRST);
        assert(0 <= i && i < SIGMA);
        return i;
    }

    int find_node(string_view s) const {
        int v = 0;
        for (char c : s) {
            int to = nodes[v].next[index(c)];
            if (to == -1) return -1;
            v = to;
        }
        return v;
    }

   public:
    explicit Trie(size_t reserve_nodes = 1) {
        nodes.reserve(reserve_nodes);
        nodes.emplace_back();
    }

    // s を1つ挿入し、s に対応するノード番号を返す
    int insert(string_view s) {
        int v = 0;
        ++nodes[v].pass;
        for (char c : s) {
            int i = index(c);
            if (nodes[v].next[i] == -1) {
                nodes[v].next[i] = static_cast<int>(nodes.size());
                nodes.emplace_back();
            }
            v = nodes[v].next[i];
            ++nodes[v].pass;
        }
        if (nodes[v].terminal++ == 0) ++distinct_count;
        return v;
    }

    // s を1つ削除する。存在しなければ false を返して何もしない
    bool erase(string_view s) {
        int v = find_node(s);
        if (v == -1 || nodes[v].terminal == 0) return false;

        --nodes[v].terminal;
        if (nodes[v].terminal == 0) --distinct_count;
        v = 0;
        --nodes[v].pass;
        for (char c : s) {
            v = nodes[v].next[index(c)];
            --nodes[v].pass;
        }
        return true;
    }

    // s と完全一致する文字列の個数を返す
    int count(string_view s) const {
        int v = find_node(s);
        return v == -1 ? 0 : nodes[v].terminal;
    }

    // prefix で始まる文字列の個数を返す
    int count_prefix(string_view prefix) const {
        int v = find_node(prefix);
        return v == -1 ? 0 : nodes[v].pass;
    }

    bool contains(string_view s) const { return count(s) > 0; }
    bool starts_with(string_view prefix) const { return count_prefix(prefix) > 0; }

    // s の接頭辞として Trie に登録されている最長の文字列の長さを返す
    // 該当する文字列がなければ -1（空文字列が登録済みなら 0）
    int longest_prefix(string_view s) const {
        int answer = nodes[0].terminal > 0 ? 0 : -1;
        int v = 0;
        for (int length = 1; length <= static_cast<int>(s.size()); ++length) {
            int to = nodes[v].next[index(s[length - 1])];
            if (to == -1 || nodes[to].pass == 0) break;
            v = to;
            if (nodes[v].terminal > 0) answer = length;
        }
        return answer;
    }

    int size() const { return nodes[0].pass; }
    int distinct_size() const { return distinct_count; }
    bool empty() const { return size() == 0; }
    int node_count() const { return static_cast<int>(nodes.size()); }

    void clear() {
        nodes.clear();
        nodes.emplace_back();
        distinct_count = 0;
    }
};
