// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/string/automaton/aho_corasick.cpp
// Aho-Corasick: 複数のパターンをテキストからまとめて検出する。
//
// 使い方:
//   AhoCorasick aho;                 // デフォルトは 'a'～'z'
//   aho.add("he");
//   aho.add("she");
//   aho.build();                     // 全パターンの add() 後に1回呼ぶ
//
//   int state = 0;
//   for (char c : text) {
//       state = aho(state, c);        // c を読んだ後の状態へ O(1) で遷移
//       if (aho.cnt[state] > 0) {
//           // この位置を右端とする登録パターンが1個以上ある。
//       }
//   }
//
// add(s) の返り値は s の終端状態。
// terminal[v]: v で直接終わる登録パターン数（同じ文字列の重複登録も数える）。
// cnt[v]:      v または failure link の先で終わる登録パターン数。
//              よってテキストがいずれかのパターンを含むかは cnt[state] > 0 で判定できる。
// fail[v]:     failure link。根の fail[0] は -1。
//
// 特定パターンの終端を target = add(pattern) で保存しても、state == target だけでは
// suffix としての一致を検出できない。例えば state が "she" の状態なら、failure link
// の先の "he" にも一致している。特定パターンだけを O(1)/文字で判定したい場合は、
// build() 後に fail[v] -> v の failure tree を作り、target の部分木に印を付ける。
// 走査中は、その印が state に付いているかを調べればよい。
//
// find(s) は s が Trie 上に存在すれば状態を返す。s 自体が登録済みか調べる場合は、
// int v = aho.find(s); v != -1 && aho.terminal[v] > 0 を確認する。
//
// アルファベットは先頭文字から文字コード上で連続している必要がある:
//   AhoCorasick<> lower;          // 'a'～'z'
//   AhoCorasick<26, 'A'> upper;   // 'A'～'Z'
//   AhoCorasick<10, '0'> digits;  // '0'～'9'
// 範囲外の文字を add(), find(), operator() に渡してはならない。
//
// 状態数 V、アルファベット数 Sigma、パターン合計長 L、テキスト長 T に対して、
// add: O(L), build: O(V * Sigma), テキスト走査: O(T), メモリ: O(V * Sigma)。
template <int Sigma = 26, char First = 'a'>
struct AhoCorasick {
    vector<array<int, Sigma>> next;
    vector<int> cnt, terminal, fail;

  private:
    vector<int> parent;
    vector<int> parent_symbol;
    bool built = false;

    static int symbol(char c) {
        int res = static_cast<unsigned char>(c) - static_cast<unsigned char>(First);
        assert(0 <= res && res < Sigma);
        return res;
    }

    void add_node(int p, int c) {
        next.emplace_back();
        next.back().fill(-1);
        cnt.push_back(0);
        terminal.push_back(0);
        parent.push_back(p);
        parent_symbol.push_back(c);
    }

  public:
    AhoCorasick() { add_node(-1, -1); }

    // パターンを登録し、その終端状態を返す。build() より前に呼ぶ。
    int add(string_view s) {
        assert(!built);
        int cur = 0;
        for (char c : s) {
            int x = symbol(c);
            if (next[cur][x] == -1) {
                int v = ssize(next);
                add_node(cur, x);
                next[cur][x] = v;
            }
            cur = next[cur][x];
        }
        terminal[cur]++;
        return cur;
    }

    // failure link、一致数、全遷移を構築する。
    // パターンを追加していなければ、複数回呼んでもよい。
    void build() {
        const int n = ssize(next);

        // build() を複数回呼んでもよいように、補完済みの遷移を除去する。
        if (built) {
            for (int v = 0; v < n; v++) {
                for (int c = 0; c < Sigma; c++) {
                    int u = next[v][c];
                    if (u != -1 && (parent[u] != v || parent_symbol[u] != c)) next[v][c] = -1;
                }
            }
        }

        cnt = terminal;
        fail.assign(n, 0);
        fail[0] = -1;
        queue<int> que;

        for (int c = 0; c < Sigma; c++) {
            int u = next[0][c];
            if (u == -1) {
                next[0][c] = 0;
            } else {
                fail[u] = 0;
                que.push(u);
            }
        }

        while (!que.empty()) {
            int v = que.front();
            que.pop();
            cnt[v] += cnt[fail[v]];

            for (int c = 0; c < Sigma; c++) {
                int u = next[v][c];
                if (u != -1 && parent[u] == v && parent_symbol[u] == c) {
                    fail[u] = next[fail[v]][c];
                    que.push(u);
                } else {
                    next[v][c] = next[fail[v]][c];
                }
            }
        }
        built = true;
    }

    // failure 遷移を使わず、Trie 上で s に対応する状態を探す。
    // s が登録パターンの接頭辞としても存在しなければ -1 を返す。
    int find(string_view s) const {
        int cur = 0;
        for (char c : s) {
            int x = symbol(c);
            int v = next[cur][x];
            // build() 後の next には failure 遷移も入るため、Trie の辺か確認する。
            if (v == -1 || parent[v] != cur || parent_symbol[v] != x) return -1;
            cur = v;
        }
        return cur;
    }

    // build() 後、状態 v から文字 c を1文字読んだ次状態を返す。
    int operator()(int v, char c) const {
        assert(built);
        if (v == -1) return 0;
        return next[v][symbol(c)];
    }
};
