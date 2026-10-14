// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/union_find/weighted_union_find.cpp
// 重み付き（ポテンシャル付き）Union-Find。
//
// 各頂点 v に未知の値 potential(v) があると考え、
//   potential(b) - potential(a) = difference
// という差分制約を merge(a, b, difference) で追加する。
//
// 使用例:
//   WeightedUnionFind<long long> uf(n);
//   bool ok = uf.merge(a, b, d);  // potential(b) - potential(a) = d
//   optional<long long> d = uf.difference(a, b);
//
// T は可換加法群として T{}（零元）、+、-、単項 -、== を使える型。
// long long、modint、座標を表す独自型などを利用できる。
// 全操作の償却計算量は O(alpha(N))、メモリは O(N)。
template <class T>
class WeightedUnionFind {
   public:
    explicit WeightedUnionFind(int n = 0)
        : parent_or_size_(n, -1), potential_to_parent_(n, T{}), component_count_(n) {
        assert(n >= 0);
    }

    // potential(b) - potential(a) = difference という制約を追加する。
    // 異なる成分なら併合して true、既に同じ成分なら制約と整合するときだけ true。
    // false は既存の制約との矛盾を表し、連結関係は変更しない（経路圧縮は行われ得る）。
    bool merge(int a, int b, const T& difference) {
        assert_vertex(a);
        assert_vertex(b);

        const int root_a = leader(a);
        const int root_b = leader(b);

        // path compression 後は potential_to_parent_[v]
        // = potential(v) - potential(root_v)。よって次は
        // potential(root_b) - potential(root_a) に必要な値となる。
        T root_difference = difference + potential_to_parent_[a] - potential_to_parent_[b];
        if (root_a == root_b) return root_difference == T{};

        int parent = root_a;
        int child = root_b;
        if (-parent_or_size_[parent] < -parent_or_size_[child]) {
            swap(parent, child);
            root_difference = -root_difference;
        }

        parent_or_size_[parent] += parent_or_size_[child];
        parent_or_size_[child] = parent;
        // child が root_b なら pot(root_b)-pot(root_a)、
        // swap 済みならその逆を保存する。
        potential_to_parent_[child] = root_difference;
        --component_count_;
        return true;
    }

    // v の属する成分の代表元を返す。
    // 経路圧縮と同時に、v から新しい親までのポテンシャル差も累積する。
    int leader(int v) {
        assert_vertex(v);
        if (parent_or_size_[v] < 0) return v;
        const int parent = parent_or_size_[v];
        const int root = leader(parent);
        potential_to_parent_[v] = potential_to_parent_[v] + potential_to_parent_[parent];
        return parent_or_size_[v] = root;
    }

    bool same(int a, int b) {
        assert_vertex(a);
        assert_vertex(b);
        return leader(a) == leader(b);
    }

    // potential(v) - potential(leader(v)) を返す。
    // 成分全体へ同じ定数を足しても差分制約は変わらないため、代表元を 0 としている。
    T potential(int v) {
        assert_vertex(v);
        leader(v);
        return potential_to_parent_[v];
    }

    // 同じ成分に属する a, b について potential(b) - potential(a) を返す。
    // 未接続なら nullopt。
    optional<T> difference(int a, int b) {
        assert_vertex(a);
        assert_vertex(b);
        if (leader(a) != leader(b)) return nullopt;
        return potential_to_parent_[b] - potential_to_parent_[a];
    }

    int size(int v) {
        assert_vertex(v);
        return -parent_or_size_[leader(v)];
    }

    int component_count() const { return component_count_; }

    // 各連結成分の頂点一覧を返す。順序は保証しない。計算量 O(N)。
    vector<vector<int>> groups() {
        const int n = parent_or_size_.size();
        vector<int> root(n), group_size(n);
        for (int v = 0; v < n; ++v) {
            root[v] = leader(v);
            ++group_size[root[v]];
        }

        vector<vector<int>> result(n);
        for (int v = 0; v < n; ++v) result[v].reserve(group_size[v]);
        for (int v = 0; v < n; ++v) result[root[v]].push_back(v);
        result.erase(remove_if(result.begin(), result.end(), [](const auto& group) { return group.empty(); }),
                     result.end());
        return result;
    }

   private:
    // root: -成分サイズ、非 root: 親頂点。
    vector<int> parent_or_size_;
    // 非 root では potential(v) - potential(parent(v))。
    // leader() 後は potential(v) - potential(root(v))。
    vector<T> potential_to_parent_;
    int component_count_;

    void assert_vertex(int v) const { assert(0 <= v && v < (int)parent_or_size_.size()); }
};
