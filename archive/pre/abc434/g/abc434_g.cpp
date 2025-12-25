#include <bits/stdc++.h>
using namespace std;

static const int MOD = 998244353;
static const int MAXN = 8000000 + 5;

long long pow10v[MAXN];

struct Node {
    char c;
    int pr;
    Node *l, *r;
    int sz;

    int len; 
    int del;
    int val;

    Node(char _c) : c(_c), pr((rand()<<16) ^ rand()), l(NULL), r(NULL) {
        sz = 1;
        if ('1' <= c && c <= '9') {
            len = 1;
            del = 0;
            val = c - '0';
        } else {
            len = 0;
            del = 1;
            val = 0;
        }
    }
};

inline int getsz(Node *t) { return t ? t->sz : 0; }

void upd(Node *t) {
    if (!t) return;

    t->sz = 1 + getsz(t->l) + getsz(t->r);

    int llen=0, ldel=0, lval=0;
    int rlen=0, rdel=0, rval=0;

    if (t->l) {
        llen = t->l->len;
        ldel = t->l->del;
        lval = t->l->val;
    }
    if (t->r) {
        rlen = t->r->len;
        rdel = t->r->del;
        rval = t->r->val;
    }

    int mlen, mdel, mval;
    if ('1' <= t->c && t->c <= '9') {
        mlen = 1;
        mdel = 0;
        mval = t->c - '0';
    } else {
        mlen = 0;
        mdel = 1;
        mval = 0;
    }

    int t1 = min(llen, mdel);
    int lenLS = llen + mlen - t1;
    int delLS = ldel + max(0, mdel - llen);
    long long valLS = ((long long)lval * pow10v[mlen] + mval) % MOD;

    int t2 = min(lenLS, rdel);
    t->len = lenLS + rlen - t2;
    t->del = delLS + max(0, rdel - lenLS);

    t->val = ( (valLS * pow10v[rlen]) % MOD + rval ) % MOD;
}

void split(Node *t, int k, Node *&l, Node *&r) {
    if (!t) { l = r = NULL; return; }
    int szl = getsz(t->l);
    if (k <= szl) {
        split(t->l, k, l, t->l);
        r = t;
        upd(r);
    } else {
        split(t->r, k - szl - 1, t->r, r);
        l = t;
        upd(l);
    }
}

Node* merge(Node *l, Node *r) {
    if (!l || !r) return l ? l : r;
    if (l->pr > r->pr) {
        l->r = merge(l->r, r);
        upd(l);
        return l;
    } else {
        r->l = merge(l, r->l);
        upd(r);
        return r;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    srand(123456);

    int N, Q;
    cin >> N >> Q;

    pow10v[0] = 1;
    for (int i = 1; i <= N; i++) pow10v[i] = (pow10v[i-1] * 10) % MOD;

    string S;
    cin >> S;

    Node *root = NULL;
    for (int i = 0; i < N; i++) {
        root = merge(root, new Node(S[i]));
    }

    while (Q--) {
        int typ;
        cin >> typ;
        if (typ == 1) {
            int x; char ch;
            cin >> x >> ch;
            x--;

            Node *a, *b, *c;
            split(root, x, a, b);
            split(b, 1, b, c);
            delete b;
            b = new Node(ch);
            root = merge(a, merge(b, c));

        } else {
            int l, r;
            cin >> l >> r;
            l--, r--;

            Node *a, *b, *c;
            split(root, l, a, b);
            split(b, r - l + 1, b, c);

            if (b->len == 0) cout << -1 << "\n";
            else cout << b->val << "\n";

            root = merge(a, merge(b, c));
        }
    }
}
