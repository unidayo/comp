struct RollingHash {
    const static ll M = (1LL << 61) - 1;
    static vll base;
    static vector<vll> basepow;
    static vector<vll> basepowinv;
    vector<vll> hash;
    string S;
    int N;
    // 文字列 s のローリングハッシュを構築する
    RollingHash(string s) {
        if (base == 0) {
            set_base();
        }
        S = s;
        N = ssize(S);
        if (ssize(basepow) < N + 1) {
            extend_pow();
        }
        hash.resize(N + 1);
        for (int i = 0; i < N; ++i) {
            hash[i + 1] = ((hash[i] * base) + S[i]) % M;
        }
    }

    // 文字列 t のハッシュ値を返す（RollingHash オブジェクトなしで使える）
    static vll get_hash(string t) {
        vll ret = 0;
        int n = ssize(t);
        for (int i = 0; i < n; ++i) {
            ret *= base;
            ret += t[i];
            ret %= M;
        }
        return ret;
    }
    // a + b のハッシュを返す
    // 引数 : a, b, b の長さ
    static vll unite(vll a, vll b, ll bsize) { return (a * basepow[bsize] + b) % M; }

    // 文字列の長さを返す
    int size() const { return N; }

    // S[l, r)のハッシュを返す
    vll get(int l, int r) const { return smod((hash[r] - hash[l] * basepow[r - l]), M); }

    // S[l, N)で最初に文字列tが出現する位置を返す
    int find(string t, int l = 0) {
        vll h = get_hash(t);
        int n = ssize(t);
        for (int i = l; i < N - n + 1; ++i) {
            if (get(i, i + n) == h) {
                return i;
            }
        }
        return -1;
    }

    // S[l, N)で最初に長さlenの文字列ハッシュhが出現する位置を返す
    int find(vll h, int len, int l = 0) {
        for (int i = l; i < N - len + 1; ++i) {
            if (get(i, i + len) == h) {
                return i;
            }
        }
        return -1;
    }

    // S[l, N)で文字列tが出現する位置をすべて返す
    vector<int> find_all(string t, int l = 0) {
        vll h = get_hash(t);
        int n = ssize(t);
        vector<int> ret;
        for (int i = l; i < N - n + 1; ++i) {
            if (get(i, i + n) == h) {
                ret.emplace_back(i);
            }
        }
        return ret;
    }

    // S[l, N)で長さlenの文字列ハッシュhが出現する位置をすべて返す
    vector<int> find_all(vll h, int len, int l = 0) {
        vector<int> ret;
        for (int i = l; i < N - len + 1; ++i) {
            if (get(i, i + len) == h) {
                ret.emplace_back(i);
            }
        }
        return ret;
    }

    // a[al..] と b[bl..] の最長共通接頭辞（LCP）の長さを返す
    // 二分探索 + ローリングハッシュで O(log N)
    static int lcp(const RollingHash &a, const RollingHash &b, int al, int bl) {
        int ok = 0, ng = min(a.size() - al, b.size() - bl) + 1;
        while (abs(ng - ok) > 1) {
            int mid = (ok + ng) / 2;
            (a.get(al, mid + al) == b.get(bl, mid + bl) ? ok : ng) = mid;
        }
        return ok;
    }

    // a[al, ar) と b[bl, br) を辞書順比較する（ar/br 省略時は文字列末尾まで）
    // a < b なら -1, a == b なら 0, a > b なら 1 を返す
    static int strcmp(const RollingHash &a, const RollingHash &b, int al, int bl, int ar = -1, int br = -1) {
        if (ar == -1) ar = a.size();
        if (br == -1) br = b.size();
        int n = min<int>({lcp(a, b, al, bl), ar - al, br - bl});
        if (al + n == ar) {
            return bl + n == br ? 0 : -1;
        } else if (bl + n == br) {
            return 1;
        } else {
            return a.S[al + n] < b.S[bl + n] ? -1 : 1;
        }
    }

   private:
    // ランダムなハッシュの基数を設定する（初回構築時に一度だけ呼ばれる）
    void set_base() {
        std::random_device seed_gen;
        std::uint32_t seed = seed_gen();
        std::mt19937 engine(seed);
        uniform_int_distribution<> dist(1 << 24, INT32_MAX);
        base = dist(engine);
    }

    // basepow / basepowinv を現在の N に合わせて拡張する
    void extend_pow() {
        int preN = ssize(basepow);
        basepow.resize(2 * N + 1);
        basepowinv.resize(2 * N + 1);
        for (int i = preN; i <= 2 * N; ++i) {
            basepow[i] = basepow[i - 1] * base % M;
        }
        basepowinv[2 * N] = pow_mod64(basepow[2 * N], M - 2, M);
        for (int i = 2 * N; i > preN; --i) {
            basepowinv[i - 1] = basepowinv[i] * base % M;
        }
    }
};
vll RollingHash::base = 0;
vector<vll> RollingHash::basepow = {1};
vector<vll> RollingHash::basepowinv = {1};