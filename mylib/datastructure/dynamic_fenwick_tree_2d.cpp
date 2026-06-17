/**
 * @brief 先読み座標圧縮つき疎な 2 次元 Fenwick Tree
 *
 * 0-indexed の点加算・矩形和。
 * sum(x1, y1, x2, y2) は半開区間 [x1, x2) x [y1, y2) の和を返す。
 *
 * build() の前に、add() する可能性がある全ての点 (x, y) を reserve_point(x, y) で登録する。
 * build() の後は、登録済みの点にだけ add() できる。
 *
 * 使い方:
 *   dynamic_fenwick_tree_2d<long long> bit(H, W);
 *   for (auto [x, y] : 更新されうる点) bit.reserve_point(x, y);
 *   bit.build();
 *
 *   bit.add(x, y, v);
 *   long long ans = bit.sum(x1, y1, x2, y2);
 *
 * 計算量:
 *   M を reserve_point() した点数とすると、
 *   メモリ: O(M log H)
 *   add / sum: O(log H log M)
 */
template <class T>
class dynamic_fenwick_tree_2d {
   public:
    dynamic_fenwick_tree_2d(int h) : dynamic_fenwick_tree_2d(h, -1) {}

    dynamic_fenwick_tree_2d(int h, int w) : h(h), w(w), built(false), ys(h + 1), data(h + 1) {
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
