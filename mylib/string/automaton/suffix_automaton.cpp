// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/string/automaton/suffix_automaton.cpp
// ALPHABET_SIZE: 文字種類数、MIN_CHAR: 使用する文字の最小値
// 例: 英小文字なら既定値、英大文字なら SuffixAutomaton<26, 'A'>、
//     任意の 1 byte 文字列なら SuffixAutomaton<256, 0> を使う。
template <int ALPHABET_SIZE = 26, unsigned char MIN_CHAR = 'a'>
struct SuffixAutomaton {
    static_assert(1 <= ALPHABET_SIZE && ALPHABET_SIZE <= 256);
    static_assert(int(MIN_CHAR) + ALPHABET_SIZE <= 256);

    struct Node {
        // unordered_map よりメモリの局所性がよく、遷移を最悪 O(1) で引ける。
        // 遷移が存在しないときは -1。
        array<int, ALPHABET_SIZE> next;
        int link;  // この状態が表す文字列から先頭を削った最長の状態
        int len;   // この状態が表す文字列の最大長

        explicit Node(int len = 0) : link(-1), len(len) { next.fill(-1); }
    };

    vector<Node> nodes;
    int last;  // 現在の文字列全体に対応する状態

    // max_length が分かるなら渡す。状態数は高々 2 * max_length - 1 なので、
    // push 中の vector の再確保と Node のコピーを避けられる。
    explicit SuffixAutomaton(size_t max_length = 0) : last(0) {
        if (max_length > 0) nodes.reserve(2 * max_length);
        nodes.emplace_back();  // root: 空文字列に対応する状態
    }

    // 末尾に c を追加し、新しい文字列全体に対応する状態番号を返す。
    // 文字列全体の構築は O(|S|)、状態数・遷移数も O(|S|)。
    int push(char c) {
        const int x = static_cast<unsigned char>(c) - int(MIN_CHAR);
        assert(0 <= x && x < ALPHABET_SIZE);

        const int cur = int(nodes.size());
        nodes.emplace_back(nodes[last].len + 1);

        // c の遷移をまだ持たない suffix に cur への遷移を追加する。
        int p = last;
        while (p != -1 && nodes[p].next[x] == -1) {
            nodes[p].next[x] = cur;
            p = nodes[p].link;
        }

        if (p == -1) {
            // root まで辿っても既存遷移がなければ、最長 suffix は空文字列。
            nodes[cur].link = 0;
        } else {
            const int q = nodes[p].next[x];
            if (nodes[p].len + 1 == nodes[q].len) {
                // p --c--> q が長さも含めてそのまま suffix link に使える。
                nodes[cur].link = q;
            } else {
                // q は異なる長さの文字列を同じ状態で表しているため clone する。
                // clone は q と遷移・link が同じだが、最大長だけを p に合わせる。
                const int clone = int(nodes.size());
                nodes.push_back(nodes[q]);
                nodes[clone].len = nodes[p].len + 1;

                nodes[q].link = nodes[cur].link = clone;
                // q を指す c 遷移だけを clone へ付け替える。
                while (p != -1 && nodes[p].next[x] == q) {
                    nodes[p].next[x] = clone;
                    p = nodes[p].link;
                }
            }
        }

        return last = cur;
    }
};
