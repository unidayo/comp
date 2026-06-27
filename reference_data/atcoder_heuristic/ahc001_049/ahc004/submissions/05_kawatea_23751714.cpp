#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

class BipartiteMatching {
    public:
    BipartiteMatching(int n, int m);
    ~BipartiteMatching();
    void add_edge(int left, int right);
    int bipartite_matching();
    int get_matching(int left);
    
    private:
    int n;
    int m;
    int *level;
    int *que;
    bool *used;
    int *match;
    vector <vector <int> > graph;
    void bfs();
    bool dfs(int from);
};

BipartiteMatching::BipartiteMatching(int n, int m) : n(n), m(m), graph(n + m) {
    level = (int *)malloc(sizeof(int) * n);
    que = (int *)malloc(sizeof(int) * n);
    used = (bool *)malloc(sizeof(bool) * n);
    match = (int *)malloc(sizeof(int) * (n + m));
}

BipartiteMatching::~BipartiteMatching() {
    free(level);
    free(que);
    free(used);
    free(match);
}

void BipartiteMatching::add_edge(int left, int right) {
    graph[left].push_back(n + right);
}

int BipartiteMatching::bipartite_matching() {
    int matching = 0;
    for (int i = 0; i < n + m; i++) match[i] = -1;
    while (1) {
        int current = 0;
        bfs();
        for (int i = 0; i < n; i++) used[i] = false;
        for (int i = 0; i < n; i++) {
            if (match[i] < 0 && dfs(i)) current++;
        }
        if (current == 0) return matching;
        matching += current;
    }
}

int BipartiteMatching::get_matching(int left) {
    return match[left] - n;
}

void BipartiteMatching::bfs() {
    int qs = 0, qt = 0;
    for (int i = 0; i < n; i++) level[i] = -1;
    for (int i = 0; i < n; i++) {
        if (match[i] < 0) {
            level[i] = 0;
            que[qt++] = i;
        }
    }
    
    while (qs < qt) {
        int now = que[qs++];
        for (int i = 0; i < graph[now].size(); i++) {
            int to = graph[now][i], next = match[to];
            if (next >= 0 && level[next] < 0) {
                level[next] = level[now] + 1;
                que[qt++] = next;
            }
        }
    }
}

bool BipartiteMatching::dfs(int from) {
    used[from] = true;
    for (int i = 0; i < graph[from].size(); i++) {
        int to = graph[from][i], next = match[to];
        if (next < 0 || (!used[next] && level[from] < level[next] && dfs(next))) {
            match[from] = to;
            match[to] = from;
            return true;
        }
    }
    return false;
}

const int N = 20;
const int MAX_M = 800;
const int MAX_L = 12;
int M;
int m;
int length[MAX_M];
int len[MAX_M];
int cnt[MAX_M];
bool used[MAX_M];
char s[MAX_M][MAX_L + 1];
char t[MAX_M][N + 1];
char ans[MAX_M][N + 1];
char tmp[N][N];

bool include(int x) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            bool f = true;
            for (int k = 0; k < length[x]; k++) {
                if (ans[i][(j + k) % N] != s[x][k]) {
                    f = false;
                    break;
                }
            }
            if (f) return true;
            
            f = true;
            for (int k = 0; k < length[x]; k++) {
                if (ans[(i + k) % N][j] != s[x][k]) {
                    f = false;
                    break;
                }
            }
            if (f) return true;
        }
    }
    return false;
}

int score() {
    int c = 0;
    for (int i = 0; i < M; i++) {
        if (include(i)) c++;
    }
    if (c < M) return round(1e8 * c / M);
    
    int d = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (ans[i][j] == '.') d++;
        }
    }
    return round(1e8 * 2 * N * N / (2 * N * N - d));
}

bool contain() {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            if (i == j || len[i] < len[j]) continue;
            for (int k = 0; k < len[i]; k++) {
                if (len[i] < N && k + len[j] > len[i]) break;
                bool f = true;
                for (int l = 0; l < len[j]; l++) {
                    char cc;
                    if (k + l >= N) {
                        cc = t[i][k + l - N];
                    } else {
                        cc = t[i][k + l];
                    }
                    if (cc != t[j][l]) {
                        f = false;
                        break;
                    }
                }
                if (f) {
                    cnt[i] += cnt[j];
                    for (int l = 0; l < N; l++) t[j][l] = t[m - 1][l];
                    len[j] = len[m - 1];
                    cnt[j] = cnt[m - 1];
                    m--;
                    return true;
                }
            }
        }
    }
    return false;
}

void combine() {
    while (true) {
        if (contain()) continue;
        
        int best = 0, x = -1, y = -1, z = -1, w = -1;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < m; j++) {
                if (i == j) continue;
                for (int k = 0; k < len[i]; k++) {
                    bool f = true;
                    int c = 0;
                    for (int l = 0; l < len[j]; l++) {
                        char cc;
                        if (k + l >= N) {
                            cc = t[i][k + l - N];
                        } else {
                            cc = t[i][k + l];
                        }
                        if (cc != '.' && cc != t[j][l]) {
                            f = false;
                            break;
                        }
                        if (cc != '.') c++;
                    }
                    if (f && (c > best || (c == best && w < len[i] + len[j]))) {
                        best = c;
                        x = i;
                        y = j;
                        z = k;
                        w = len[i] + len[j];
                    }
                }
            }
        }
        
        if (best == 0) break;
        for (int i = 0; i < len[y]; i++) {
            t[x][(z + i) % N] = t[y][i];
        }
        len[x] = len[x] + len[y] - best;
        cnt[x] += cnt[y];
        for (int i = 0; i < N; i++) t[y][i] = t[m - 1][i];
        len[y] = len[m - 1];
        cnt[y] = cnt[m - 1];
        m--;
    }
}

bool equal(int x1, int y1, int x2, int y2) {
    if (y1 >= N) y1 -= N;
    if (y2 >= N) y2 -= N;
    return ans[x1][y1] == '.' || ans[x2][y2] == '.' || ans[x1][y1] == ans[x2][y2];
}

int main() {
    scanf("%*d %d", &M);
    m = M;
    for (int i = 0; i < m; i++) {
        scanf("%s", s[i]);
        length[i] = len[i] = strlen(s[i]);
        cnt[i] = 1;
        for (int j = 0; j < len[i]; j++) t[i][j] = s[i][j];
        for (int j = len[i]; j < N; j++) t[i][j] = '.';
    }
    
    combine();
    
    for (int i = 0; i < m; i++) {
        int best = 0;
        for (int j = 0; j < m; j++) {
            if (cnt[j] == -1) continue;
            cnt[j] = 0;
            for (int k = 0; k < M; k++) {
                if (used[k]) continue;
                for (int l = 0; l < len[j]; l++) {
                    bool f = true;
                    for (int r = 0; r < length[k]; r++) {
                        char cc;
                        if (l + r >= N) {
                            cc = t[j][l + r - N];
                        } else {
                            cc = t[j][l + r];
                        }
                        if (cc != s[k][r]) {
                            f = false;
                            break;
                        }
                    }
                    if (f) {
                        cnt[j]++;
                        break;
                    }
                }
            }
            if (cnt[j] > cnt[best]) best = j;
        }
        for (int k = 0; k < M; k++) {
            if (used[k]) continue;
            for (int l = 0; l < len[best]; l++) {
                bool f = true;
                for (int r = 0; r < length[k]; r++) {
                    char cc;
                    if (l + r >= N) {
                        cc = t[best][l + r - N];
                    } else {
                        cc = t[best][l + r];
                    }
                    if (cc != s[k][r]) {
                        f = false;
                        break;
                    }
                }
                if (f) {
                    used[k] = true;
                    break;
                }
            }
        }
        for (int j = 0; j < N; j++) ans[i][j] = t[best][j];
        cnt[best] = -1;
    }
    
    bool update = false;
    for (int dx1 = 0; dx1 < N; dx1++) {
        for (int dy1 = 0; dy1 < N; dy1++) {
            BipartiteMatching bm(N, N);
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    for (int k = 0; k < N; k++) {
                        if (equal(i, k, N, j) && equal(i, k + dy1, N + 1, j + dx1)) {
                            bm.add_edge(i, j);
                            break;
                        }
                    }
                }
            }
            if (bm.bipartite_matching() != N) continue;
            
            for (int dx2 = 0; dx2 < N; dx2++) {
                for (int dy2 = 0; dy2 < 15; dy2++) {
                    if (dy1 == dy2) continue;
                    if (update) break;
                    BipartiteMatching bm(N, N);
                    for (int i = 0; i < N; i++) {
                        for (int j = 0; j < N; j++) {
                            for (int k = 0; k < N; k++) {
                                if (equal(i, k, N, j) && equal(i, k + dy1, N + 1, j + dx1) && equal(i, k + dy2, N + 2, j + dx2)) {
                                    bm.add_edge(i, j);
                                    break;
                                }
                            }
                        }
                    }
                    if (bm.bipartite_matching() == N) {
                        update = true;
                        for (int i = 0; i < N; i++) {
                            int x = bm.get_matching(i), y = 0;
                            for (int j = 0; j < N; j++) {
                                if (equal(i, j, N, x) && equal(i, j + dy1, N + 1, x + dx1) && equal(i, j + dy2, N + 2, x + dx2)) {
                                    y = j;
                                    break;
                                }
                            }
                            for (int j = 0; j < N; j++) {
                                tmp[x][j] = ans[i][(y + j) % N];
                            }
                        }
                        for (int i = 0; i < N; i++) {
                            if (ans[N][i] != '.') tmp[i][0] = ans[N][i];
                            if (ans[N + 1][i] != '.') tmp[(i - dx1 + N) % N][dy1] = ans[N + 1][i];
                            if (ans[N + 2][i] != '.') tmp[(i - dx2 + N) % N][dy2] = ans[N + 2][i];
                        }
                        for (int i = 0; i < N; i++) {
                            for (int j = 0; j < N; j++) {
                                ans[i][j] = tmp[i][j];
                            }
                        }
                    }
                }
            }
        }
    }
    
    for (int dx = 0; dx < N; dx++) {
        for (int dy = 0; dy < N; dy++) {
            if (update) break;
            BipartiteMatching bm(N, N);
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    for (int k = 0; k < N; k++) {
                        if (equal(i, k, N, j) && equal(i, k + dy, N + 1, j + dx)) {
                            bm.add_edge(i, j);
                            break;
                        }
                    }
                }
            }
            if (bm.bipartite_matching() == N) {
                update = true;
                for (int i = 0; i < N; i++) {
                    int x = bm.get_matching(i), y = 0;
                    for (int j = 0; j < N; j++) {
                        if (equal(i, j, N, x) && equal(i, j + dy, N + 1, x + dx)) {
                            y = j;
                            break;
                        }
                    }
                    for (int j = 0; j < N; j++) {
                        tmp[x][j] = ans[i][(y + j) % N];
                    }
                }
                for (int i = 0; i < N; i++) {
                    if (ans[N][i] != '.') tmp[i][0] = ans[N][i];
                    if (ans[N + 1][i] != '.') tmp[(i - dx + N) % N][dy] = ans[N + 1][i];
                }
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        ans[i][j] = tmp[i][j];
                    }
                }
            }
        }
    }
    
    if (!update) {
        BipartiteMatching bm(N, N);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                for (int k = 0; k < N; k++) {
                    if (equal(i, k, N, j)) {
                        bm.add_edge(i, j);
                        break;
                    }
                }
            }
        }
        if (bm.bipartite_matching() == N) {
            for (int i = 0; i < N; i++) {
                int x = bm.get_matching(i), y = 0;
                for (int j = 0; j < N; j++) {
                    if (equal(i, j, N, x)) {
                        y = j;
                        break;
                    }
                }
                for (int j = 0; j < N; j++) {
                    tmp[x][j] = ans[i][(y + j) % N];
                }
            }
            for (int i = 0; i < N; i++) {
                if (ans[N][i] != '.') tmp[i][0] = ans[N][i];
            }
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    ans[i][j] = tmp[i][j];
                }
            }
        }
    }
    
    fprintf(stderr, "%d\n", score());
    
    for (int i = 0; i < N; i++) printf("%s\n", ans[i]);
    
    return 0;
}
