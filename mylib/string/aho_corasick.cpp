struct AhoCorasick {
    vector<map<char, int>> next;
    vector<int> cnt, fail;

    AhoCorasick() : cnt(1), next(1) {}

    int add(string s) {
        int cur = 0;
        for (auto c : s) {
            if (!next[cur].contains(c)) {
                next[cur][c] = ssize(next);
                next.push_back({});
                cnt.push_back(0);
            }
            cur = next[cur][c];
        }
        cnt[cur]++;
        return cur;
    }

    void build() {
        fail = vector<int>(ssize(next), -1);
        queue<int> que;
        que.push(0);
        while (!que.empty()) {
            int cur = que.front();
            que.pop();
            for (auto [c, v] : next[cur]) {
                que.push(v);
                fail[v] = (*this)(fail[cur], c);
                cnt[v]+=cnt[fail[v]];
            }
        }
    }

    int find(string s) {
        int cur = 0;
        for (auto c : s) {
            if (!next[cur].contains(c)) {
                return -1;
            }
            cur = next[cur][c];
        }
        return cur;
    }

    int operator()(int v, char c) {
        while (v != -1) {
            if (next[v].contains(c)) {
                return next[v][c];
            }
            v = fail[v];
        }
        return 0;
    }
};