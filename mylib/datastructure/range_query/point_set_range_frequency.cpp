// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/range_query/point_set_range_frequency.cpp
// Point Set Range Frequency。
//
// Yosupo: point_set_range_frequency 向け。
//
// 使い方:
//   PointSetRangeFrequency<long long> ds(a);
//   ds.set(p, x);          // a[p] = x
//   ds.count(l, r, x);     // [l, r) にある x の個数
//
// 計算量:
//   構築 O(N log N)
//   set / count O(log N)
//
// 注意:
//   - kth や range_freq(< upper) は扱わない。
//   - DynamicWaveletMatrix より機能を絞る代わりに、この問題ではかなり速い。

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

template <class T>
class PointSetRangeFrequency {
   private:
    using OrderedSet =
        __gnu_pbds::tree<int, __gnu_pbds::null_type, less<int>, __gnu_pbds::rb_tree_tag,
                         __gnu_pbds::tree_order_statistics_node_update>;

    vector<T> data;
    map<T, OrderedSet> positions;

   public:
    PointSetRangeFrequency() = default;
    explicit PointSetRangeFrequency(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        data = v;
        positions.clear();
        for (int i = 0; i < (int)data.size(); i++) {
            positions[data[i]].insert(i);
        }
    }

    int size() const { return (int)data.size(); }
    bool empty() const { return data.empty(); }

    T access(int p) const {
        assert(0 <= p && p < size());
        return data[p];
    }

    void set(int p, const T &x) {
        assert(0 <= p && p < size());
        if (data[p] == x) return;

        auto it = positions.find(data[p]);
        assert(it != positions.end());
        it->second.erase(p);
        if (it->second.empty()) positions.erase(it);

        data[p] = x;
        positions[x].insert(p);
    }

    int count(int l, int r, const T &x) const {
        assert(0 <= l && l <= r && r <= size());
        auto it = positions.find(x);
        if (it == positions.end()) return 0;
        const OrderedSet &s = it->second;
        return (int)(s.order_of_key(r) - s.order_of_key(l));
    }
};
