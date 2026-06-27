struct SuffixAutomaton {
    struct Node {
        unordered_map<char, int> next;
        int link, len;
    };
    vector<Node> nodes;
    int last;

    SuffixAutomaton() {
        nodes.push_back({{}, -1, 0});
        last = 0;
    }

    void push(char c) {
        int new_node = int(nodes.size());
        nodes.push_back({{}, -1, nodes[last].len + 1});
        int p = last;
        while (p != -1 && !nodes[p].next.contains(c)) {
            nodes[p].next[c] = new_node;
            p = nodes[p].link;
        }
        int q = (p == -1 ? 0 : nodes[p].next[c]);
        if (p == -1 || nodes[p].len + 1 == nodes[q].len) {
            nodes[new_node].link = q;
        } else {
            // clone node (q -> new_q)
            int new_q = int(nodes.size());
            nodes.push_back({nodes[q].next, nodes[q].link, nodes[p].len + 1});
            nodes[q].link = new_q;
            nodes[new_node].link = new_q;

            while (p != -1 && nodes[p].next[c] == q) {
                nodes[p].next[c] = new_q;
                p = nodes[p].link;
            }
        }
        last = new_node;
    }
};