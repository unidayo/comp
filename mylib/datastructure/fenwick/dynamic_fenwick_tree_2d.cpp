// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/fenwick/dynamic_fenwick_tree_2d.cpp
/**
 * @brief 先読み座標圧縮つき 2 次元 Fenwick Tree
 *
 * 0-indexed の点加算・矩形和。
 * sum(x1, y1, x2, y2) は半開区間 [x1, x2) x [y1, y2) の和を返す。
 *
 * build() の前に、add() する可能性がある全ての点 (x, y) を reserve_point(x, y) で登録する。
 * build() の後は、登録済みの点にだけ add() できる。
 * sum() で頻繁に使う x 境界は build() の前に reserve_prefix_x(x) で登録しておくと速い。
 *
 * 使い方:
 *   dynamic_fenwick_tree_2d<long long> bit(H, W);        // H が大きくても使える疎外側版
 *   dense_fenwick_tree_2d<long long> bit(H, W);          // H が小さいときはこちらが速い
 *   for (auto [x, y] : 更新されうる点) bit.reserve_point(x, y);
 *   for (auto [x1, x2] : クエリの x 境界) {
 *       bit.reserve_prefix_x(x1);
 *       bit.reserve_prefix_x(x2);
 *   }
 *   bit.build();
 *
 *   bit.add(x, y, v);
 *   long long ans = bit.sum(x1, y1, x2, y2);
 *
 * 計算量:
 *   M を reserve_point() した点数とすると、
 *   メモリ: O(M log H)
 *   add / sum: O(log H log M)
 *
 * dynamic_fenwick_tree_2d:
 *   H が 1e9 以上でも、外側 Fenwick のうち実際に触る index だけを持つ。
 *   reserve_prefix_x(x) で sum の prefix 境界 x を登録しておくと、外側 index 列も前計算する。
 *   未登録の x 境界で sum() しても動くが、その場合は外側 index を二分探索で引くため少し遅い。
 *
 * dense_fenwick_tree_2d:
 *   外側 Fenwick を vector で持つ。H が小さいとき高速。
 */

template <class T>
class dense_fenwick_tree_2d {
   public:
    dense_fenwick_tree_2d(int h) : dense_fenwick_tree_2d(h, -1) {}

    dense_fenwick_tree_2d(int h, int w) : h(h), w(w), built(false), ys(h + 1), data(h + 1) {
        assert(0 <= h);
        assert(-1 <= w);
    }

    void reserve_point(int x, int y) {
        assert(!built);
        assert(0 <= x && x < h);
        if (w != -1) assert(0 <= y && y < w);
        for (++x; x <= h; x += x & -x) ys[x].push_back(y);
    }

    void build() {
        assert(!built);
        for (int i = 1; i <= h; ++i) {
            sort(ys[i].begin(), ys[i].end());
            ys[i].erase(unique(ys[i].begin(), ys[i].end()), ys[i].end());
            data[i].assign((int)ys[i].size() + 1, T());
        }
        built = true;
    }

    void add(int x, int y, T v) {
        assert(built);
        assert(0 <= x && x < h);
        if (w != -1) assert(0 <= y && y < w);
        for (++x; x <= h; x += x & -x) {
            auto it = lower_bound(ys[x].begin(), ys[x].end(), y);
            assert(it != ys[x].end() && *it == y);
            int j = (int)(it - ys[x].begin()) + 1;
            for (; j < (int)data[x].size(); j += j & -j) data[x][j] += v;
        }
    }

    T sum(int x1, int y1, int x2, int y2) const {
        assert(built);
        assert(0 <= x1 && x1 <= x2 && x2 <= h);
        if (w != -1) assert(0 <= y1 && y1 <= y2 && y2 <= w);
        return prefix_sum(x2, y2) - prefix_sum(x1, y2) - prefix_sum(x2, y1) + prefix_sum(x1, y1);
    }

    T get(int x, int y) const {
        assert(built);
        assert(0 <= x && x < h);
        if (w != -1) assert(0 <= y && y < w);
        return sum(x, y, x + 1, y + 1);
    }

   private:
    int h, w;
    bool built;
    vector<vector<int>> ys;
    vector<vector<T>> data;

    T prefix_sum(int x, int y) const {
        T ret = T();
        for (; x > 0; x -= x & -x) {
            int j = (int)(lower_bound(ys[x].begin(), ys[x].end(), y) - ys[x].begin());
            for (; j > 0; j -= j & -j) ret += data[x][j];
        }
        return ret;
    }
};

/**
 * @brief H が大きくても使える疎外側版
 */
template <class T>
class dynamic_fenwick_tree_2d {
   public:
    dynamic_fenwick_tree_2d(long long h) : dynamic_fenwick_tree_2d(h, -1) {}

    dynamic_fenwick_tree_2d(long long h, long long w) : h(h), w(w), built(false) {
        assert(0 <= h);
        assert(-1 <= w);
    }

    void reserve_point(long long x, long long y) {
        assert(!built);
        assert(0 <= x && x < h);
        if (w != -1) assert(0 <= y && y < w);
        add_xs.push_back(x);
        for (++x; x <= h; x += x & -x) pending.emplace_back(x, y);
    }

    void reserve_prefix_x(long long x) {
        assert(!built);
        assert(0 <= x && x <= h);
        prefix_xs.push_back(x);
    }

    void build() {
        assert(!built);
        sort(pending.begin(), pending.end());

        xs.clear();
        ys.clear();
        for (int i = 0; i < (int)pending.size();) {
            long long x = pending[i].first;
            xs.push_back(x);
            ys.push_back({});
            while (i < (int)pending.size() && pending[i].first == x) {
                ys.back().push_back(pending[i].second);
                i++;
            }
            auto &vec = ys.back();
            vec.erase(unique(vec.begin(), vec.end()), vec.end());
        }
        data.assign(ys.size(), {});
        for (int i = 0; i < (int)ys.size(); i++) data[i].assign((int)ys[i].size() + 1, T());

        sort(add_xs.begin(), add_xs.end());
        add_xs.erase(unique(add_xs.begin(), add_xs.end()), add_xs.end());
        add_paths.resize(add_xs.size());
        for (int i = 0; i < (int)add_xs.size(); i++) {
            for (long long x = add_xs[i] + 1; x <= h; x += x & -x) {
                int id = xid(x);
                assert(id != -1);
                add_paths[i].push_back(id);
            }
        }

        prefix_xs.push_back(0);
        prefix_xs.push_back(h);
        sort(prefix_xs.begin(), prefix_xs.end());
        prefix_xs.erase(unique(prefix_xs.begin(), prefix_xs.end()), prefix_xs.end());
        prefix_paths.resize(prefix_xs.size());
        for (int i = 0; i < (int)prefix_xs.size(); i++) {
            for (long long x = prefix_xs[i]; x > 0; x -= x & -x) {
                int id = xid(x);
                if (id != -1) prefix_paths[i].push_back(id);
            }
        }

        pending.clear();
        pending.shrink_to_fit();
        built = true;
    }

    void add(long long x, long long y, T v) {
        assert(built);
        assert(0 <= x && x < h);
        if (w != -1) assert(0 <= y && y < w);
        int path_id = add_path_id(x);
        assert(path_id != -1);
        for (int i : add_paths[path_id]) {
            auto &vec = ys[i];
            auto it = lower_bound(vec.begin(), vec.end(), y);
            assert(it != vec.end() && *it == y);
            auto &bit = data[i];
            int j = (int)(it - vec.begin()) + 1;
            for (; j < (int)bit.size(); j += j & -j) bit[j] += v;
        }
    }

    T sum(long long x1, long long y1, long long x2, long long y2) const {
        assert(built);
        assert(0 <= x1 && x1 <= x2 && x2 <= h);
        if (w != -1) assert(0 <= y1 && y1 <= y2 && y2 <= w);
        return prefix_sum(x2, y2) - prefix_sum(x1, y2) - prefix_sum(x2, y1) + prefix_sum(x1, y1);
    }

    T get(long long x, long long y) const {
        assert(built);
        assert(0 <= x && x < h);
        if (w != -1) assert(0 <= y && y < w);
        return sum(x, y, x + 1, y + 1);
    }

   private:
    long long h, w;
    bool built;
    vector<pair<long long, long long>> pending;
    vector<long long> xs;
    vector<vector<long long>> ys;
    vector<vector<T>> data;
    vector<long long> add_xs;
    vector<vector<int>> add_paths;
    vector<long long> prefix_xs;
    vector<vector<int>> prefix_paths;

    int xid(long long x) const {
        auto it = lower_bound(xs.begin(), xs.end(), x);
        if (it == xs.end() || *it != x) return -1;
        return (int)(it - xs.begin());
    }

    int add_path_id(long long x) const {
        auto it = lower_bound(add_xs.begin(), add_xs.end(), x);
        if (it == add_xs.end() || *it != x) return -1;
        return (int)(it - add_xs.begin());
    }

    int prefix_path_id(long long x) const {
        auto it = lower_bound(prefix_xs.begin(), prefix_xs.end(), x);
        if (it == prefix_xs.end() || *it != x) return -1;
        return (int)(it - prefix_xs.begin());
    }

    T prefix_sum(long long x, long long y) const {
        T ret = T();
        int path_id = prefix_path_id(x);
        if (path_id != -1) {
            for (int i : prefix_paths[path_id]) {
                const auto &vec = ys[i];
                const auto &bit = data[i];
                int j = (int)(lower_bound(vec.begin(), vec.end(), y) - vec.begin());
                for (; j > 0; j -= j & -j) ret += bit[j];
            }
            return ret;
        }
        for (; x > 0; x -= x & -x) {
            int i = xid(x);
            if (i == -1) continue;
            const auto &vec = ys[i];
            const auto &bit = data[i];
            int j = (int)(lower_bound(vec.begin(), vec.end(), y) - vec.begin());
            for (; j > 0; j -= j & -j) ret += bit[j];
        }
        return ret;
    }
};
