#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>

using namespace std;

const int N = 9;
const int M = 20;
const int K = 81;
const int MOD = 998244353;
const int N2 = N * N;
const int MAX_M = 2000;

struct stamp {
    int m;
    int p;
    int q;
    
    stamp(int m, int p, int q) : m(m), p(p), q(q) {}
};

int a[N2];
int s[MAX_M][9];
int pqp[N2];
int pqq[N2];
int pos_index[N2];
int M2, M3;
int nums[MAX_M];
int mm1[MAX_M];
int mm2[MAX_M];
int mm3[MAX_M];
int num;
int b[N2];
int pqs[K];
int cnts[K];
int remains[K];
vector<int> targets[N2];

void copy(int dest[N2], int src[N2]) {
    for (int i = 0; i < N2; i++) dest[i] = src[i];
}

void add(int now[N2], int m, int pq) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int ij = i * N + j;
            now[pq + ij] += s[m][pos_index[ij]];
            if (now[pq + ij] >= MOD) now[pq + ij] -= MOD;
        }
    }
}

void add(int now[N2], const stamp& st) {
    add(now, st.m, st.p * N + st.q);
}

void sub(int now[N2], int m, int pq) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int ij = i * N + j;
            now[pq + ij] -= s[m][pos_index[ij]];
            if (now[pq + ij] < 0) now[pq + ij] += MOD;
        }
    }
}

void sub(int now[N2], const stamp& st) {
    sub(now, st.m, st.p * N + st.q);
}

namespace beam_search {
    using node_type = int;
    using operation_type = unsigned;
    using score_type = unsigned long long;
    constexpr static bool FINISH_FIRST = false;
    constexpr static int MAX_TURN = 49;
    constexpr static int BEAM_WIDTH = 750;
    constexpr static int MAX_CANDIDATE = 2000 * BEAM_WIDTH;
    constexpr static int POOL_SIZE = MAX_TURN * BEAM_WIDTH + MAX_CANDIDATE;
    constexpr static score_type TURN_SCORE = 1e15;
    constexpr static score_type FINISH_SCORE = 1e18;
    
    namespace operation {
        inline void try_apply(operation_type& operation, score_type& score) {
            int m = (operation >> 8), pq = (operation & 0xFF);
            for (int pos : targets[pq]) {
                int x = b[pq + pos] + s[m][pos_index[pos]];
                if (x >= MOD) x -= MOD;
                score += x;
            }
        }
        
        inline void apply(operation_type operation) {
            int m = (operation >> 8), pq = (operation & 0xFF);
            add(b, m, pq);
            num += nums[m];
        }
        
        inline void revert(operation_type operation) {
            int m = (operation >> 8), pq = (operation & 0xFF);
            sub(b, m, pq);
            num -= nums[m];
        }
    }
    
    namespace node {
        constexpr static node_type EMPTY = -1;
        node_type next_node;
        node_type parent[POOL_SIZE];
        node_type child[POOL_SIZE];
        node_type prev[POOL_SIZE];
        node_type next[POOL_SIZE];
        operation_type operation[POOL_SIZE];
        score_type score[POOL_SIZE];
        
        inline void init() {
            next_node = 0;
            for (int i = 1; i < POOL_SIZE; i++) parent[i - 1] = i;
        }
        
        inline void init_node(node_type n) {
            parent[n] = child[n] = prev[n] = next[n] = EMPTY;
            score[n] = 0;
            num = 0;
            copy(b, a);
        }
        
        inline node_type get_node() {
            node_type n = next_node;
            next_node = parent[n];
            return n;
        }
        
        inline void return_node(node_type n) {
            parent[n] = next_node;
            next_node = n;
        }
        
        inline void add_node(node_type n) {
            child[n] = prev[n] = EMPTY;
            next[n] = child[parent[n]];
            child[parent[n]] = n;
            if (next[n] != EMPTY) prev[next[n]] = n;
        }
        
        inline void delete_node(node_type n) {
            while (prev[n] == EMPTY && next[n] == EMPTY) {
                node_type next = parent[n];
                return_node(n);
                n = next;
            }
            if (prev[n] == EMPTY) {
                child[parent[n]] = next[n];
            } else {
                next[prev[n]] = next[n];
            }
            if (next[n] != EMPTY) prev[next[n]] = prev[n];
            return_node(n);
        }
        
        inline void add_candidates(node_type*& candidates, node_type now, int turn) {
            int pq = pqs[turn], cnt = cnts[turn];
            if (cnt == 1 && num + remains[turn] < K) cnt = 2;
            int mm = M;
            if (cnt == 2) mm = M2;
            if (cnt == 3) mm = M3;
            for (int m = 0; m < mm; m++) {
                operation_type op = (m << 8) | pq;
                score_type next_score = score[now];
                operation::try_apply(op, next_score);
                node_type candidate = get_node();
                parent[candidate] = now;
                operation[candidate] = op;
                score[candidate] = next_score;
                *candidates++ = candidate;
            }
        }
    }
    
    vector<operation_type> construct_results(node_type top) {
        vector<operation_type> ans;
        while (node::parent[top] != node::EMPTY) {
            ans.push_back(node::operation[top]);
            top = node::parent[top];
        }
        reverse(ans.begin(), ans.end());
        return ans;
    }
    
    vector<operation_type> search() {
        node::init();
        node_type top = node::get_node();
        node::init_node(top);
        
        node_type* candidates = (node_type*)malloc(sizeof(node_type) * (MAX_CANDIDATE * 2));
        node_type* leaves = candidates + MAX_CANDIDATE;
        node_type* leaves_end = leaves;
        score_type best_score = 0;
        vector<operation_type> best_results;
        for (int turn = 0; turn < MAX_TURN; turn++) {
            while (node::child[top] != node::EMPTY && node::next[node::child[top]] == node::EMPTY) {
                top = node::child[top];
                operation::apply(node::operation[top]);
            }
            
            node_type* candidates_end = candidates;
            node_type now = top;
            do {
                while (node::child[now] != node::EMPTY) {
                    now = node::child[now];
                    operation::apply(node::operation[now]);
                }
                
                node::add_candidates(candidates_end, now, turn);
                
                if (candidates_end - candidates > BEAM_WIDTH * 2) {
                    nth_element(candidates, candidates + BEAM_WIDTH, candidates_end, [&](const node_type n1, const node_type n2) {
                        return node::score[n1] > node::score[n2];
                    });
                    for (node_type* n = candidates + BEAM_WIDTH; n != candidates_end; n++) node::return_node(*n);
                    candidates_end = candidates + BEAM_WIDTH;
                }
                
                while (now != top) {
                    operation::revert(node::operation[now]);
                    if (node::next[now] == node::EMPTY) {
                        now = node::parent[now];
                    } else {
                        now = node::next[now];
                        operation::apply(node::operation[now]);
                        break;
                    }
                }
            } while (now != top);
            
            if (candidates_end - candidates > BEAM_WIDTH) {
                nth_element(candidates, candidates + BEAM_WIDTH, candidates_end, [&](const node_type n1, const node_type n2) {
                    return node::score[n1] > node::score[n2];
                });
                for (node_type* n = candidates + BEAM_WIDTH; n != candidates_end; n++) node::return_node(*n);
                candidates_end = candidates + BEAM_WIDTH;
            }
            
            bool added = false;
            for (node_type* n = candidates; n != candidates_end; n++) {
                if (node::score[*n] >= FINISH_SCORE) {
                    if (FINISH_FIRST) {
                        return construct_results(*n);
                    } else {
                        score_type score = node::score[*n];
                        if (score > best_score) {
                            best_score = score;
                            best_results = construct_results(*n);
                        }
                        node::return_node(*n);
                        *n-- = *--candidates_end;
                    }
                } else {
                    added = true;
                    node::add_node(*n);
                }
            }
            for (node_type* n = leaves; n != leaves_end; n++) {
                if (node::child[*n] == node::EMPTY) node::delete_node(*n);
            }
            swap(candidates, leaves);
            leaves_end = candidates_end;
            if (!added) break;
        }
        
        if (!best_results.empty()) return best_results;
        return construct_results(*max_element(leaves, leaves_end, [&](const node_type n1, const node_type n2) {
            return node::score[n1] < node::score[n2];
        }));
    }
}

void input() {
    scanf("%*d %*d %*d");
    for (int i = 0; i < N2; i++) scanf("%d", &a[i]);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < 9; j++) scanf("%d", &s[i][j]);
    }
}

void output(const vector<stamp>& v) {
    printf("%d\n", v.size());
    for (const stamp& st : v) printf("%d %d %d\n", st.m, st.p, st.q);
}

long long score(int now[N2]) {
    long long sum = 0;
    for (int i = 0; i < N2; i++) sum += now[i];
    return sum;
}

long long score(const vector<stamp>& v) {
    int now[N2];
    copy(now, a);
    for (const stamp& st : v) add(now, st);
    return score(now);
}

void init() {
    for (int p = 0; p < N; p++) {
        for (int q = 0; q < N; q++) {
            pqp[p * N + q] = p;
            pqq[p * N + q] = q;
        }
    }
    for (int i = 0, c = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            pos_index[i * N + j] = c++;
        }
    }
    for (int m = 0; m < M; m++) {
        nums[m] = 1;
        mm1[m] = m;
        mm2[m] = mm3[m] = -1;
    }
    M2 = M;
    for (int m1 = 0; m1 < M; m1++) {
        for (int m2 = m1; m2 < M; m2++) {
            for (int i = 0; i < 9; i++) {
                s[M2][i] = s[m1][i] + s[m2][i];
                if (s[M2][i] >= MOD) s[M2][i] -= MOD;
            }
            nums[M2] = 2;
            mm1[M2] = m1;
            mm2[M2] = m2;
            mm3[M2] = -1;
            M2++;
        }
    }
    M3 = M2;
    for (int m1 = 0; m1 < M; m1++) {
        for (int m2 = m1; m2 < M; m2++) {
            for (int m3 = m2; m3 < M; m3++) {
                for (int i = 0; i < 9; i++) {
                    s[M3][i] = s[m1][i] + s[m2][i];
                    if (s[M3][i] >= MOD) s[M3][i] -= MOD;
                    s[M3][i] += s[m3][i];
                    if (s[M3][i] >= MOD) s[M3][i] -= MOD;
                }
                nums[M3] = 3;
                mm1[M3] = m1;
                mm2[M3] = m2;
                mm3[M3] = m3;
                M3++;
            }
        }
    }
}

int main() {
    input();
    
    init();
    
    long long best_score = 0;
    vector<stamp> best_ans;
    for (int dir = 0; dir < 8; dir++) {
        for (int p = 0; p <= N - 3; p++) {
            for (int q = 0; q <= N - 3; q++) {
                targets[p * N + q].clear();
            }
        }
        int index = 0;
        for (int i = 0; i <= N - 3; i++) {
            for (int j = 0; j <= N - 3; j++) {
                int p, q, sp, sq, dp, dq, cp = 1, cq = 1;
                if (dir == 0) {
                    p = i;
                    sp = i;
                    dp = 1;
                    q = j;
                    sq = j;
                    dq = 1;
                } else if (dir == 1) {
                    q = i;
                    sq = i;
                    dq = 1;
                    p = j;
                    sp = j;
                    dp = 1;
                } else if (dir == 2) {
                    p = N - i - 3;
                    sp = N - i - 1;
                    dp = -1;
                    q = j;
                    sq = j;
                    dq = 1;
                } else if (dir == 3) {
                    q = N - i - 3;
                    sq = N - i - 1;
                    dq = -1;
                    p = j;
                    sp = j;
                    dp = 1;
                } else if (dir == 4) {
                    p = i;
                    sp = i;
                    dp = 1;
                    q = N - j - 3;
                    sq = N - j - 1;
                    dq = -1;
                } else if (dir == 5) {
                    q = i;
                    sq = i;
                    dq = 1;
                    p = N - j - 3;
                    sp = N - j - 1;
                    dp = -1;
                } else if (dir == 6) {
                    p = N - i - 3;
                    sp = N - i - 1;
                    dp = -1;
                    q = N - j - 3;
                    sq = N - j - 1;
                    dq = -1;
                } else if (dir == 7) {
                    q = N - i - 3;
                    sq = N - i - 1;
                    dq = -1;
                    p = N - j - 3;
                    sp = N - j - 1;
                    dp = -1;
                }
                if (i == N - 3) {
                    if (dir % 2 == 0) {
                        cp = 3;
                    } else {
                        cq = 3;
                    }
                }
                if (j == N - 3) {
                    if (dir % 2 == 0) {
                        cq = 3;
                    } else {
                        cp = 3;
                    }
                }
                int pq = p * N + q;
                pqs[index] = pq;
                for (int k = 0; k < cp; k++) {
                    for (int l = 0; l < cq; l++) {
                        targets[pq].emplace_back((sp + dp * k - p) * N + sq + dq * l - q);
                    }
                }
                cnts[index] = ((targets[pq].size() == 1) ? 1 : 3);
                index++;
            }
        }
        for (int i = index; i >= 0; i--) remains[i] = remains[i + 1] + cnts[i];
        
        vector<beam_search::operation_type> v = beam_search::search();
        vector<stamp> ans;
        for (beam_search::operation_type op : v) {
            int m = (op >> 8), pq = (op & 0xFF);
            int m1 = mm1[m], m2 = mm2[m], m3 = mm3[m];
            int p = pqp[pq], q = pqq[pq];
            if (m1 >= 0) ans.emplace_back(m1, p, q);
            if (m2 >= 0) ans.emplace_back(m2, p, q);
            if (m3 >= 0) ans.emplace_back(m3, p, q);
        }
        long long s = score(ans);
        if (s > best_score) {
            best_score = s;
            best_ans = ans;
        }
    }
    
    output(best_ans);
    
    fprintf(stderr, "%lld\n", best_score);
    
    return 0;
}
