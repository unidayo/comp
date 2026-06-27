#include <bits/stdc++.h>
constexpr int DEBUG = 0;
using namespace std;
using int64 = long long;

constexpr int MAX_N = 100;
using BitSetN = bitset<MAX_N>;

template<typename T> ostream& operator<<(ostream& s, const vector<T>& v) {
  s << "[";
  for (int i = 0; i < ssize(v); i++) { if (i > 0) { s << ", "; } s << v[i]; }
  s << "]";
  return s;
}

template<typename T> vector<T> VectorWithRemoved(const vector<T>& v, T x) {
  vector<T> r;
  for (const auto& element : v) {
    if (element == x) continue;
    r.push_back(element);
  }
  return r;
}

template<typename T> vector<T> ConcatenateVectors(
    const vector<T>& v0, const vector<T>& v1) {
  vector<T> r;
  for (const auto& element : v0) {
    r.push_back(element);
  }
  for (const auto& element : v1) {
    r.push_back(element);
  }
  return r;
}

class Random {
 public:
  Random(int seed) : random_engine_(seed) {}

  int RandomInt(int a, int b, bool inclusive = false) {
    if (inclusive) b++;
    return uniform_int_distribution<int>(a, b - 1)(random_engine_);
  }

  double RandomDouble(double a, double b) {
    return uniform_real_distribution<double>(a, b)(random_engine_);
  }

  template <class T>
  void Shuffle(vector<T>& v) {
    shuffle(v.begin(), v.end(), random_engine_);
  }

 private:
  default_random_engine random_engine_;
};

int Log2Ceil(int x) {
  int v = 1;
  int r = 0;
  while (v < x) {
    v *= 2;
    r++;
  }
  return r;
}

class Main {
 private:
  Random random_;

  int n_;
  int group_count_;
  int q_count_;

  vector<int> assignment_;
  vector<int> item_rank_;
  // item_order_matrix_[i][j] is set to true when
  // item_weight(i) < item_weight(j).
  vector<BitSetN> item_order_matrix_;
  int q_index_;

  map<string, string> query_cache_;

  // If invalid_move_table_[i] is true, a move operation for the item i always
  // fails.
  // invalid_move_table_[i] can be set to true when
  // group_weight(0) + item_weight(i) > group_weight(group_count_ - 1).
  // This is because group_weight(group_count_ - 1) - group_weight(0) is
  // non-increasing throughout the process.
  vector<int> invalid_move_table_;

  vector<vector<int>> invalid_swap_table_;

  int pairwise_shuffle_failure_count_;

  bool multi_ldm_done_ = false;

  string QueryByGroupIds(int group_0, int group_1) {
    vector<int> items_0;
    vector<int> items_1;
    for (int id = 0; id < n_; id++) {
      if (assignment_[id] == group_0) {
        items_0.push_back(id);
      }
      if (assignment_[id] == group_1) {
        items_1.push_back(id);
      }
    }

    return QueryByItemVectors(items_0, items_1);
  }

  string QueryByItemVectors(
      const vector<int>& items_0, const vector<int>& items_1) {
    if (items_0.size() == 1 && items_1.size() == 1) {
      int item_0 = items_0[0];
      int item_1 = items_1[0];
      if (!item_rank_.empty()) {
        if (item_rank_[item_0] > item_rank_[item_1]) {
          return ">";
        } else if (item_rank_[item_0] < item_rank_[item_1]) {
          return "<";
        } else {
          // exit(1);
        }
      } else if (!item_order_matrix_.empty()) {
        if (item_order_matrix_[item_0][item_1]) {
          return "<";
        } else if (item_order_matrix_[item_1][item_0]) {
          return ">";
        }
      }
    }

    vector<int> key_0 = items_0;
    sort(key_0.begin(), key_0.end());
    vector<int> key_1 = items_1;
    sort(key_1.begin(), key_1.end());
    stringstream key_stream, reverse_key_stream;
    key_stream << key_0 << "," << key_1;
    reverse_key_stream << key_1 << "," << key_0;
    const string key = key_stream.str();
    const string reverse_key = reverse_key_stream.str();

    if (query_cache_.contains(key)) {
      return query_cache_[key];
    }

    if (q_index_ >= q_count_) {
      return "*";
    }

    cout << items_0.size() << " ";
    cout << items_1.size() << " ";
    for (int id : items_0) {
      cout << id << " ";
    }
    for (int id : items_1) {
      cout << id << " ";
    }
    cout << endl;

    q_index_++;

    string result; cin >> result;
  
    if (result == "<") {
      query_cache_[key] = "<";
      query_cache_[reverse_key] = ">";
    } else if (result == ">") {
      query_cache_[key] = ">";
      query_cache_[reverse_key] = "<";
    }

    if (items_0.size() == 1 && items_1.size() == 1 &&
        !item_order_matrix_.empty()) {
      int item_0 = items_0[0];
      int item_1 = items_1[0];
      if (result == "<") {
        item_order_matrix_[item_0][item_1] = true;
      } else if (result == ">") {
        item_order_matrix_[item_1][item_0] = true;
      }
      for (int i = 0; i < n_; i++) {
        for (int j = 0; j < n_; j++) {
          if (item_order_matrix_[i][j]) {
            item_order_matrix_[i] |= item_order_matrix_[j];
          }
        }
      }
    }

    return result;
  }

  string QueryByItemVectorsWithDuplication(
      const vector<int>& items_0, const vector<int>& items_1) {
    vector<int> in_items_0(n_), in_items_1(n_);
    for (int item_0 : items_0) {
      in_items_0[item_0] = 1;
    }
    for (int item_1 : items_1) {
      in_items_1[item_1] = 1;
    }
    vector<int> new_items_0, new_items_1;
    for (int item : views::iota(0, n_)) {
      if (in_items_0[item] && !in_items_1[item]) {
        new_items_0.push_back(item);
      }
      if (!in_items_0[item] && in_items_1[item]) {
        new_items_1.push_back(item);
      }
    }

    if (new_items_0.empty() && new_items_1.empty()) {
      return "=";
    } else if (new_items_0.empty()) {
      return "<";
    } else if (new_items_1.empty()) {
      return ">";
    }

    return QueryByItemVectors(new_items_0, new_items_1);
  }

  vector<int> GroupMergeSort(const vector<int>& xs) {
    // cerr << "GroupMergeSort(): " << xs << endl;
    if (xs.size() == 1) {
      return xs;
    } else if (xs.size() == 2) {
      const string result = QueryByGroupIds(xs[0], xs[1]);
      if (result == "<") {
        return {xs[0], xs[1]};
      } else {
        return {xs[1], xs[0]};
      }
    }

    vector<int> children_0;
    vector<int> children_1;
    for (int i = 0; i < ssize(xs); i++) {
      if (i % 2 == 0) {
        children_0.push_back(xs[i]);
      } else {
        children_1.push_back(xs[i]);
      }
    }

    children_0 = GroupMergeSort(children_0);
    children_1 = GroupMergeSort(children_1);

    vector<int> rs;
    int c_index_0 = 0;
    int c_index_1 = 0;
    while (c_index_0 < ssize(children_0) || c_index_1 < ssize(children_1)) {
      if (c_index_0 == ssize(children_0)) {
        rs.push_back(children_1[c_index_1]);
        c_index_1++;
      } else if (c_index_1 == ssize(children_1)) {
        rs.push_back(children_0[c_index_0]);
        c_index_0++;
      } else {
        const string result = QueryByGroupIds(
            children_0[c_index_0], children_1[c_index_1]);
        if (result == "<") {
          rs.push_back(children_0[c_index_0]);
          c_index_0++;
        } else {
          rs.push_back(children_1[c_index_1]);
          c_index_1++;
        }
      }
    }

    return rs;
  }

  vector<int> ItemMergeSort(const vector<int>& xs) {
    // cerr << "BlockMergeSort(): " << xs << endl;
    if (xs.size() == 1) {
      return xs;
    } else if (xs.size() == 2) {
      const string result = QueryByItemVectors({xs[0]}, {xs[1]});
      if (result == "<") {
        return {xs[0], xs[1]};
      } else {
        return {xs[1], xs[0]};
      }
    }

    vector<int> children_0;
    vector<int> children_1;
    for (int i = 0; i < ssize(xs); i++) {
      if (i % 2 == 0) {
        children_0.push_back(xs[i]);
      } else {
        children_1.push_back(xs[i]);
      }
    }

    children_0 = ItemMergeSort(children_0);
    children_1 = ItemMergeSort(children_1);

    vector<int> rs;
    int c_index_0 = 0;
    int c_index_1 = 0;
    while (c_index_0 < ssize(children_0) || c_index_1 < ssize(children_1)) {
      if (c_index_0 == ssize(children_0)) {
        rs.push_back(children_1[c_index_1]);
        c_index_1++;
      } else if (c_index_1 == ssize(children_1)) {
        rs.push_back(children_0[c_index_0]);
        c_index_0++;
      } else {
        const string result =
            QueryByItemVectors({children_0[c_index_0]}, {children_1[c_index_1]});
        if (result == "<") {
          rs.push_back(children_0[c_index_0]);
          c_index_0++;
        } else {
          rs.push_back(children_1[c_index_1]);
          c_index_1++;
        }
      }
    }

    return rs;
  }

  int InitializationWithItemRankCost() {
    const int margin = n_;
    return (n_ * Log2Ceil(n_) - (1 << Log2Ceil(n_)) + 1) +
        (group_count_ * Log2Ceil(group_count_) -
             (1 << Log2Ceil(group_count_)) + 1) +
        margin;
  }

  vector<int> RunItemMergeSort() {
    vector<int> sorted_items(n_);
    iota(sorted_items.begin(), sorted_items.end(), 0);
    sorted_items = ItemMergeSort(sorted_items);
    item_rank_ = vector<int>(n_, -1);
    for (int i : views::iota(0, n_)) {
      item_rank_[sorted_items[i]] = i;
    }
    cerr << "item_rank_: " << item_rank_ << endl;
    return sorted_items;
  }

  bool MaybeRunInitialLargestDifferencingMethod() {
    int group_sort_cost = 0;
    for (int i = group_count_; i >= 3; i--) {
      group_sort_cost += Log2Ceil(i);
    }
    int ldm_cost = 0;
    for (int i = n_ - 1; i >= 2; i--) {
      ldm_cost += Log2Ceil(i)+ group_sort_cost;
    }
    const int kMargin = n_;
    if (q_index_ + ldm_cost + kMargin > q_count_) {
      return false;
    }

    cerr << "InitialLDM" << endl;

    vector<int> items;
    for (int item : views::iota(0, n_)) {
      items.push_back(item);
    }
    sort(items.begin(), items.end(), [&](int i0, int i1) -> bool {
      return item_rank_[i0] < item_rank_[i1];
    });

    vector<vector<vector<int>>> tuples(n_);
    for (int i : views::iota(0, n_)) {
      vector<vector<int>> tuple(group_count_);
      tuples[i] = tuple;
      tuples[i][0] = {items[i]};
    }

    auto tuple_comparator =
        [&](const vector<vector<int>>& t0,
            const vector<vector<int>>& t1) -> bool {
      const auto v0 = t0.front();
      const auto v1 = t0.back();
      const auto v2 = t1.front();
      const auto v3 = t1.back();
      const auto v4 = ConcatenateVectors(v0, v3);
      const auto v5 = ConcatenateVectors(v1, v2);
      string result = QueryByItemVectors(v4, v5);
      return result == "<";
    };

    auto vector_comparator =
        [&](const vector<int>& v0, const vector<int>& v1) -> bool {
      string result = QueryByItemVectorsWithDuplication(v0, v1);
      return result == ">";
    };

    while (tuples.size() >= 2) {
      const auto t0 = tuples.back();
      tuples.pop_back();
      const auto t1 = tuples.back();
      tuples.pop_back();

      vector<vector<int>> t2 = {
        ConcatenateVectors(t0[0], t1[group_count_ - 1]),
        ConcatenateVectors(t0[group_count_ - 1], t1[0])
      };
      for (int i = 1; i < group_count_ - 1; i++) {
        const auto v = ConcatenateVectors(t0[i], t1[group_count_ - 1 - i]);
        t2.insert(
            lower_bound(t2.begin(), t2.end(), v, vector_comparator),
            v);
      }

      tuples.insert(
          lower_bound(tuples.begin(), tuples.end(), t2, tuple_comparator),
          t2);
    }

    assignment_ = vector<int>(n_, -1);
    for (int i = 0; i < group_count_; i++) {
      for (int item : tuples[0][i]) {
        assignment_[item] = group_count_ - 1 - i;
      }
    }

    multi_ldm_done_ = true;

    return true;
  }

  void RunInitializationWithItemRank() {
    cerr << "RunInitializationWithItemRank" << endl;
    vector<int> sorted_items = RunItemMergeSort();

    if (MaybeRunInitialLargestDifferencingMethod()) {
      return;
    }

    vector<double> item_weights(n_);
    double p_max = 1.0 - exp(-n_ / group_count_);
    for (int i = 0; i < n_; i++) {
      int rank = item_rank_[i];
      double p = p_max * (2 * rank + 1) / (2 * n_);
      double x = -1.0E5 * log(1.0 - p);
      item_weights[i] = x;
    }

    // vector<double> sorted_item_weights = item_weights;
    // sort(sorted_item_weights.begin(), sorted_item_weights.end());
    // for (double w : sorted_item_weights) {
    //   cerr << w << " ";
    // }
    // cerr << endl;

    vector<int> items;
    for (int item : views::iota(0, n_)) {
      items.push_back(item);
    }
    sort(items.begin(), items.end(), [&](int i0, int i1) -> bool {
      return item_rank_[i0] < item_rank_[i1];
    });

    vector<vector<vector<int>>> tuples(n_);
    for (int i : views::iota(0, n_)) {
      vector<vector<int>> tuple(group_count_);
      tuples[i] = tuple;
      tuples[i][0] = {items[i]};
    }

    auto tuple_comparator =
        [&](const vector<vector<int>>& t0,
            const vector<vector<int>>& t1) -> bool {
      double w0 = 0.0, w1 = 0.0;
      const auto v0 = t0.front();
      const auto v1 = t0.back();
      const auto v2 = t1.front();
      const auto v3 = t1.back();
      for (int item : v0) {
        w0 += item_weights[item];
      }
      for (int item : v1) {
        w0 -= item_weights[item];
      }
      for (int item : v2) {
        w1 += item_weights[item];
      }
      for (int item : v3) {
        w1 -= item_weights[item];
      }
      return w0 < w1;
    };

    auto vector_comparator =
        [&](const vector<int>& v0, const vector<int>& v1) -> bool {
      double w0 = 0.0, w1 = 0.0;
      for (int item : v0) {
        w0 += item_weights[item];
      }
      for (int item : v1) {
        w1 += item_weights[item];
      }
      return w0 > w1;
    };

    while (tuples.size() >= 2) {
      const auto t0 = tuples.back();
      tuples.pop_back();
      const auto t1 = tuples.back();
      tuples.pop_back();

      vector<vector<int>> t2 = {
        ConcatenateVectors(t0[0], t1[group_count_ - 1]),
        ConcatenateVectors(t0[group_count_ - 1], t1[0])
      };
      for (int i = 1; i < group_count_ - 1; i++) {
        const auto v = ConcatenateVectors(t0[i], t1[group_count_ - 1 - i]);
        t2.insert(
            lower_bound(t2.begin(), t2.end(), v, vector_comparator),
            v);
      }

      tuples.insert(
          lower_bound(tuples.begin(), tuples.end(), t2, tuple_comparator),
          t2);
    }

    assignment_ = vector<int>(n_, -1);
    for (int i = 0; i < group_count_; i++) {
      for (int item : tuples[0][i]) {
        assignment_[item] = group_count_ - 1 - i;
      }
    }

    RunGroupMergeSortPhase();

    WriteTemporarySolution();
  }

  vector<int> GetGroupItems(int group_id) {
    vector<int> item_ids;
    for (int id = 0; id < n_; id++) {
      if (assignment_[id] == group_id) {
        item_ids.push_back(id);
      }
    }
    return item_ids;
  }

  void RunGroupMergeSortPhase() {
    vector<int> sorted_group_ids(group_count_);
    iota(sorted_group_ids.begin(), sorted_group_ids.end(), 0);
    sorted_group_ids = GroupMergeSort(sorted_group_ids);
    vector<int> group_id_map(group_count_);
    for (int i = 0; i < group_count_; i++) {
      group_id_map[sorted_group_ids[i]] = i;
    }
    for (int id = 0; id < n_; id++) {
      assignment_[id] = group_id_map[assignment_[id]];
    }
  }

  void WriteTemporarySolution() {
    cout << "#c ";
    for (int id = 0; id < n_; id++) {
      cout << assignment_[id] << " ";
    }
    cout << endl;
  }

  bool RunSingleUpdatePhase() {
    cerr << "RunSingleUpdatePhase(): " << q_index_ << endl;

    // if (item_rank_.empty()) {
    //   int order_count = 0;
    //   for (int i : views::iota(0, n_)) {
    //     for (int j : views::iota(0, n_)) {
    //       if (item_order_matrix_[i][j]) {
    //         order_count++;
    //       }
    //     }
    //   }
    //   cerr << "order_count: " << order_count << " / "
    //        << n_ * (n_ - 1) / 2 << endl;
    // }

    auto run_update_post_process =
          [&](int group_0, int group_1, bool ranked) -> void {
      WriteTemporarySolution();

      vector<int> sorted_group_ids;
      for (int group_id : views::iota(0, group_count_)) {
        if (group_id != group_0 && group_id != group_1) {
          sorted_group_ids.push_back(group_id);
        }
      }

      auto comparator = [&](int g0, int g1) -> bool {
        string result = QueryByGroupIds(g0, g1);
        return result == "<";
      };

      auto it_0 = lower_bound(
          sorted_group_ids.begin(), sorted_group_ids.end(), group_0,
          comparator);
      it_0 = sorted_group_ids.insert(it_0, group_0);

      if (ranked) {
        auto it_1 = lower_bound(
            it_0, sorted_group_ids.end(), group_1, comparator);
        sorted_group_ids.insert(it_1, group_1);
      } else {
        auto it_1 = lower_bound(
            sorted_group_ids.begin(), sorted_group_ids.end(), group_1,
            comparator);
        sorted_group_ids.insert(it_1, group_1);
      }

      vector<int> group_id_map(group_count_);
      for (int i = 0; i < group_count_; i++) {
        group_id_map[sorted_group_ids[i]] = i;
      }

      for (int id = 0; id < n_; id++) {
        assignment_[id] = group_id_map[assignment_[id]];
      }

      WriteTemporarySolution();
    };

    vector<tuple<int, int>> group_pairs;
    for (int d = group_count_ - 1; d >= 1; d--) {
      for (int group_1 = group_count_ - 1; group_1 - d >= 0; group_1--) {
        group_pairs.push_back({group_1 - d, group_1});
      }
    }
    auto pair_score = [&](int g0, int g1) -> int {
      int m = group_count_ - 1;
      int d0 = (m - 2 * g0) * (m - 2 * g0) + (m - 2 * g1) * (m - 2 * g1);
      int g = g0 + g1;
      int d1 = (m - g) * (m - g) * 2;
      return d0 - d1;
    };
    sort(group_pairs.begin(), group_pairs.end(),
         [&](const tuple<int, int>& t0, const tuple<int, int>& t1) -> bool {
           const auto [g00, g01] = t0;
           const auto [g10, g11] = t1;
           int score_0 = pair_score(g00, g01);
           int score_1 = pair_score(g10, g11);
           if (score_0 != score_1) {
             return score_0 > score_1;
           }
           return g01 > g11;
         });


    for (const auto& [group_0, group_1] : group_pairs) {
      const vector<int> group_0_items = GetGroupItems(group_0);
      const vector<int> group_1_items = GetGroupItems(group_1);

      if (group_1_items.size() == 1) {
        continue;
      }

      // Pairwise shuffle.
      if (item_rank_.empty()) {
        constexpr int kPairwiseShuffleFilureMaxCount = 1;
        while (pairwise_shuffle_failure_count_ < kPairwiseShuffleFilureMaxCount) {
          vector<int> items = ConcatenateVectors(group_0_items, group_1_items);
          random_.Shuffle(items);
          vector<int> group_0_items_new;
          vector<int> group_1_items_new;
          for (int i : views::iota(0, ssize(items))) {
            if (i % 2 == 0) {
              group_0_items_new.push_back(items[i]);
            } else {
              group_1_items_new.push_back(items[i]);
            }
          }

          const string check_result_0 = QueryByItemVectorsWithDuplication(
              group_0_items_new, group_1_items_new);
          if (check_result_0 == ">") {
            swap(group_0_items_new, group_1_items_new);
          }

          const string check_result_1 = QueryByItemVectorsWithDuplication(
              group_1_items_new, group_1_items);
          if (check_result_1 == "<") {
            for (int item : group_0_items_new) {
              assignment_[item] = group_0;
            }
            for (int item : group_1_items_new) {
              assignment_[item] = group_1;
            }
            cerr << "[P-SHUFFLE] q_index: " << q_index_
                  << " g0: " << group_0 << " g1: " << group_1 << endl;
            run_update_post_process(group_0, group_1, true);
            return true;
          } else {
            pairwise_shuffle_failure_count_++;
            continue;
          }
        }
      }

      // Try a move operation.
      // A move operation can happen only when group_0 is 0.
      if (group_0 == 0) {
        if (!item_rank_.empty()) {
          // When item rank is enabled.
          vector<int> sorted_group_1_items = group_1_items;
          sort(sorted_group_1_items.begin(), sorted_group_1_items.end(),
               [&](int item_0, int item_1) -> bool {
                 return item_rank_[item_0] < item_rank_[item_1];
               });

          bool changed = false;
          vector<int> group_0_items_new = group_0_items;
          vector<int> group_1_items_new = group_1_items;
          for (int move_item_candidate : sorted_group_1_items) {
            if (invalid_move_table_[move_item_candidate]) break;

            bool skip_by_extended_invalid_move_table = false;
            for (int i : views::iota(0, n_)) {
              if (item_rank_[i] < item_rank_[move_item_candidate] &&
                  invalid_move_table_[i]) {
                skip_by_extended_invalid_move_table = true;
              }
            }
            if (skip_by_extended_invalid_move_table) {
              invalid_move_table_[move_item_candidate] = true;
              break;
            }

            vector<int> group_0_items_candidate = group_0_items;
            group_0_items_candidate.push_back(move_item_candidate);
            vector<int> group_1_items_candidate =
                VectorWithRemoved(group_1_items, move_item_candidate);

            const string check_result_0 = QueryByItemVectorsWithDuplication(
                group_0_items_candidate, group_1_items_candidate);
            if (check_result_0 == "<") {
            } else if (check_result_0 == ">") {
              swap(group_0_items_candidate, group_1_items_candidate);
            } else {
              break;
            }

            const string check_result_1 = QueryByItemVectorsWithDuplication(
                group_1_items_candidate, group_1_items_new);
            if (check_result_1 == "<") {
              group_0_items_new = group_0_items_candidate;
              group_1_items_new = group_1_items_candidate;
              changed = true;
            } else {
              if (group_0 == 0 && group_1 == group_count_ - 1 &&
                  !changed && check_result_1 == ">") {
                invalid_move_table_[move_item_candidate] = 1;
              }
              break;
            }
          }

          if (changed) {
            for (int item : group_0_items_new) {
              assignment_[item] = group_0;
            }
            for (int item : group_1_items_new) {
              assignment_[item] = group_1;
            }
            cerr << "[MOVE] q_index: " << q_index_
                 << " g0: " << group_0 << " g1: " << group_1 << endl;
            run_update_post_process(group_0, group_1, true);
            return true;
          }
        } else {
          // When item rank is not enabled.
          for (int move_item : group_1_items) {
            if (invalid_move_table_[move_item]) continue;

            bool skip_by_extended_invalid_move_table = false;
            for (int i : views::iota(0, n_)) {
              if ((i == move_item || item_order_matrix_[i][move_item]) &&
                  invalid_move_table_[i]) {
                skip_by_extended_invalid_move_table = true;
              }
            }
            if (skip_by_extended_invalid_move_table) {
              invalid_move_table_[move_item] = true;
              continue;
            }

            vector<int> new_group_0_items = group_0_items;
            new_group_0_items.push_back(move_item);
            string check_result = QueryByItemVectorsWithDuplication(
                new_group_0_items, group_1_items);
            
            if (group_0 == 0 && group_1 == group_count_ - 1 &&
                check_result == ">") {
              invalid_move_table_[move_item] = 1;
            }

            if (check_result != "<") continue;

            assignment_[move_item] = group_0;

            // Update item_order_matrix_.
            for (int i : views::iota(0, n_)) {
              if (invalid_move_table_[i]) {
                item_order_matrix_[move_item][i] = true;
              }
            }
            for (int i = 0; i < n_; i++) {
              for (int j = 0; j < n_; j++) {
                if (item_order_matrix_[i][j]) {
                  item_order_matrix_[i] |= item_order_matrix_[j];
                }
              }
            }

            cerr << "[MOVE] q_index: " << q_index_
                 << " g0: " << group_0 << " g1: " << group_1 << endl;
            run_update_post_process(group_0, group_1, false);
            return true;
          } 
        }
      }

      // Try a swap opearation.
      if (!item_rank_.empty()) {
        // When item rank is enabled.
        vector<int> sorted_group_0_items = group_0_items;
        sort(sorted_group_0_items.begin(), sorted_group_0_items.end(),
              [&](int item_0, int item_1) -> bool {
                return item_rank_[item_0] < item_rank_[item_1];
              });
        vector<int> sorted_group_1_items = group_1_items;
        sort(sorted_group_1_items.begin(), sorted_group_1_items.end(),
              [&](int item_0, int item_1) -> bool {
                return item_rank_[item_0] < item_rank_[item_1];
              });

        bool changed = false;
        vector<int> group_0_items_new = group_0_items;
        vector<int> group_1_items_new = group_1_items;

        for (int item_1 : sorted_group_1_items) {
          for (int item_0 : sorted_group_0_items | views::reverse) {
            if (item_rank_[item_0] > item_rank_[item_1]) {
              continue;
            }

            if (invalid_swap_table_[item_0][item_1]) {
              break;
            }

            bool skip_by_extended_invalid_swap_table = false;
            for (int i0 : views::iota(0, n_)) {
              for (int i1 : views::iota(0, n_)) {
                if (item_rank_[item_0] <= item_rank_[i0] &&
                    item_rank_[i1] <= item_rank_[item_1] &&
                    invalid_swap_table_[i0][i1]) {
                  skip_by_extended_invalid_swap_table = true;
                }
              }
            }

            if (skip_by_extended_invalid_swap_table) {
              invalid_swap_table_[item_0][item_1] = 1;
              break;
            }

            vector<int> group_0_items_candidate =
                VectorWithRemoved(group_0_items, item_0);
            group_0_items_candidate.push_back(item_1);
            vector<int> group_1_items_candidate =
                VectorWithRemoved(group_1_items, item_1);
            group_1_items_candidate.push_back(item_0);

            const string check_result_0 = QueryByItemVectorsWithDuplication(
                group_0_items_candidate, group_1_items_candidate);
            if (check_result_0 == "<") {
            } else if (check_result_0 == ">") {
              swap(group_0_items_candidate, group_1_items_candidate);
            } else {
              break;
            }

            const string check_result_1 = QueryByItemVectorsWithDuplication(
                group_1_items_candidate, group_1_items_new);
            if (check_result_1 == "<") {
              group_0_items_new = group_0_items_candidate;
              group_1_items_new = group_1_items_candidate;
              changed = true;
            } else {
              if (group_0 == 0 && group_1 == group_count_ - 1 &&
                  !changed && check_result_1 == ">") {
                invalid_swap_table_[item_0][item_1] = 1;
              }
              break;
            }
          }
          if (changed) {
            break;
          }
        }

        if (changed) {
          for (int item : group_0_items_new) {
            assignment_[item] = group_0;
          }
          for (int item : group_1_items_new) {
            assignment_[item] = group_1;
          }
          cerr << "[SWAP] q_index: " << q_index_
               << " g0: " << group_0 << " g1: " << group_1 << endl;
          run_update_post_process(group_0, group_1, true);
          return true;
        }
      } else {
        vector<tuple<int, int>> item_tuples;
        for (int i = 0; i < max(ssize(group_0_items), ssize(group_1_items));
             i++) {
          if (i < ssize(group_0_items)) {
            item_tuples.push_back({group_0_items[i], 0});
          }
          if (i < ssize(group_1_items)) {
            item_tuples.push_back({group_1_items[i], 1});
          }
        }

        bool invalid = false;
        auto comparator =
            [&](const tuple<int, int>& t0,
                const tuple<int, int>& t1) -> bool {
          const auto [i0, g0] = t0;
          const auto [i1, g1] = t1;
          string result = QueryByItemVectors({i0}, {i1});
          if (result == "*") {
            invalid = true;
          }
          return result == "<";
        };

        vector<tuple<int, int>> sorted_item_tuples;
        for (const auto item_tuple : item_tuples) {
          auto it = lower_bound(
              sorted_item_tuples.begin(), sorted_item_tuples.end(), item_tuple,
              comparator);
          if (invalid) {
            return false;
          }
          it = sorted_item_tuples.insert(it, item_tuple);
          int index = distance(sorted_item_tuples.begin(), it);

          int swap_item_0 = -1;
          int swap_item_1 = -1;
          if (get<1>(item_tuple) == 0 &&
              index + 1 < ssize(sorted_item_tuples) &&
              get<1>(sorted_item_tuples[index + 1]) == 1) {
            swap_item_0 = get<0>(sorted_item_tuples[index]);
            swap_item_1 = get<0>(sorted_item_tuples[index + 1]);
          } else if (get<1>(item_tuple) == 1 && 1 <= index &&
                     get<1>(sorted_item_tuples[index - 1]) == 0) {
            swap_item_0 = get<0>(sorted_item_tuples[index - 1]);
            swap_item_1 = get<0>(sorted_item_tuples[index]);
          }

          if (swap_item_0 >= 0 && swap_item_1 >= 0) {
            if (invalid_swap_table_[swap_item_0][swap_item_1]) {
              continue;
            }

            bool skip_by_extended_invalid_swap_table = false;
            for (int i0 : views::iota(0, n_)) {
              for (int i1 : views::iota(0, n_)) {
                if ((swap_item_0 == i0 || item_order_matrix_[swap_item_0][i0]) &&
                    (i1 == swap_item_1 || item_order_matrix_[i1][swap_item_1]) &&
                    invalid_swap_table_[i0][i1]) {
                  skip_by_extended_invalid_swap_table = true;
                }
              }
            }

            if (skip_by_extended_invalid_swap_table) {
              invalid_swap_table_[swap_item_0][swap_item_1] = 1;
              continue;
            }

            vector<int> group_0_items_new =
                VectorWithRemoved(group_0_items, swap_item_0);
            group_0_items_new.push_back(swap_item_1);

            const string check_result_1 = QueryByItemVectorsWithDuplication(
                group_0_items_new, group_1_items);

            if (group_0 == 0 && group_1 == group_count_ - 1 &&
                check_result_1 == ">") {
              invalid_swap_table_[swap_item_0][swap_item_1] = 1;
            }

            if (check_result_1 != "<") continue;

            assignment_[swap_item_0] = group_1;
            assignment_[swap_item_1] = group_0;

            // Update item_order_matrix_.
            for (int i0 : views::iota(0, n_)) {
              for (int i1 : views::iota(0, n_)) {
                if (invalid_swap_table_[i0][i1]) {
                  if (item_order_matrix_[swap_item_0][i0]) {
                    item_order_matrix_[swap_item_1][i1] = true;
                  }
                  if (item_order_matrix_[i1][swap_item_1]) {
                    item_order_matrix_[i0][swap_item_0] = true;
                  }
                }
              }
            }
            for (int i = 0; i < n_; i++) {
              for (int j = 0; j < n_; j++) {
                if (item_order_matrix_[i][j]) {
                  item_order_matrix_[i] |= item_order_matrix_[j];
                }
              }
            }

            cerr << "[SWAP] q_index: " << q_index_
                << " g0: " << group_0 << " g1: " << group_1
                << " item_0: " << swap_item_0 << " item_1: " << swap_item_1
                << endl;
            run_update_post_process(group_0, group_1, false);
            return true;
          }
        }
      }

      // Run the largest differencing method.
      if (!item_rank_.empty()) {
        int size = group_0_items.size() + group_1_items.size();
        int ldm_cost = 0;
        for (int i = size - 1; i >= 2; i--) {
          ldm_cost += Log2Ceil(i);
        }
        ldm_cost += 1;  // The last comparison with the current solution.
        if (q_index_ + ldm_cost > q_count_) {
          continue;
        }
        // cerr << "LargestDifferencingMethod" << endl;
        // cerr << "ldm_cost: " << ldm_cost << endl;
        // cerr << "q_index_: " << q_index_ << endl;
        vector<int> items;
        for (int item : group_0_items) {
          items.push_back(item);
        }
        for (int item : group_1_items) {
          items.push_back(item);
        }
        // cerr << "size: " << items.size() << endl;

        sort(items.begin(), items.end(), [&](int i0, int i1) -> bool {
          return item_rank_[i0] < item_rank_[i1];
        });

        vector<tuple<vector<int>, vector<int>>> tuples;
        for (int item : items) {
          tuples.push_back({{item}, {}});
        }

        bool invalid = false;
        auto comparator =
            [&](const tuple<vector<int>, vector<int>>& t0,
                const tuple<vector<int>, vector<int>>& t1) -> bool {
          const auto [v0, v1] = t0;
          const auto [v2, v3] = t1;
          const auto v4 = ConcatenateVectors(v0, v3);
          const auto v5 = ConcatenateVectors(v1, v2);
          string result = QueryByItemVectors(v4, v5);
          if (result == "*") {
            invalid = true;
          }
          return result == "<";
        };

        while (tuples.size() >= 2) {
          const auto [v0, v1] = tuples.back();
          tuples.pop_back();
          const auto [v2, v3] = tuples.back();
          tuples.pop_back();

          const auto v4 = ConcatenateVectors(v0, v3);
          const auto v5 = ConcatenateVectors(v1, v2);
          const auto t = make_tuple(v4, v5);

          auto it_0 = lower_bound(
              tuples.begin(), tuples.end(), t,
              comparator);
          it_0 = tuples.insert(it_0, t);
        }

        if (invalid) {
          return false;
        }

        // cerr << "q_index_: " << q_index_ << endl;

        const auto [group_1_items_new, group_0_items_new] = tuples[0];

        string result = QueryByItemVectorsWithDuplication(
            group_1_items_new, group_1_items);
        if (result == "<") {
          for (int item : group_0_items_new) {
            assignment_[item] = group_0;
          }
          for (int item : group_1_items_new) {
            assignment_[item] = group_1;
          }
          cerr << "[LDM] q_index: " << q_index_
               << " g0: " << group_0 << " g1: " << group_1 << endl;
          run_update_post_process(group_0, group_1, true);
          return true;
        }
      } else {
        // Assumption: All items are sorted in SWAP phase and order information
        // is available.

        int size = group_0_items.size() + group_1_items.size();
        int ldm_cost = 0;
        for (int i = size - 1; i >= 2; i--) {
          ldm_cost += Log2Ceil(i);
        }
        ldm_cost += 1;  // The last comparison with the current solution.
        if (q_index_ + ldm_cost > q_count_) {
          continue;
        }

        vector<int> items;
        for (int item : group_0_items) {
          items.push_back(item);
        }
        for (int item : group_1_items) {
          items.push_back(item);
        }
        // cerr << "size: " << items.size() << endl;

        sort(items.begin(), items.end(), [&](int i0, int i1) -> bool {
          return item_order_matrix_[i0][i1];
        });

        vector<tuple<vector<int>, vector<int>>> tuples;
        for (int item : items) {
          tuples.push_back({{item}, {}});
        }

        bool invalid = false;
        auto comparator =
            [&](const tuple<vector<int>, vector<int>>& t0,
                const tuple<vector<int>, vector<int>>& t1) -> bool {
          const auto [v0, v1] = t0;
          const auto [v2, v3] = t1;
          const auto v4 = ConcatenateVectors(v0, v3);
          const auto v5 = ConcatenateVectors(v1, v2);
          string result = QueryByItemVectors(v4, v5);
          if (result == "*") {
            invalid = true;
          }
          return result == "<";
        };

        while (tuples.size() >= 2) {
          const auto [v0, v1] = tuples.back();
          tuples.pop_back();
          const auto [v2, v3] = tuples.back();
          tuples.pop_back();

          const auto v4 = ConcatenateVectors(v0, v3);
          const auto v5 = ConcatenateVectors(v1, v2);
          const auto t = make_tuple(v4, v5);

          auto it_0 = lower_bound(
              tuples.begin(), tuples.end(), t,
              comparator);
          it_0 = tuples.insert(it_0, t);
        }

        if (invalid) {
          return false;
        }

        // cerr << "q_index_: " << q_index_ << endl;

        const auto [group_1_items_new, group_0_items_new] = tuples[0];

        string result = QueryByItemVectorsWithDuplication(
            group_1_items_new, group_1_items);
        if (result == "<") {
          for (int item : group_0_items_new) {
            assignment_[item] = group_0;
          }
          for (int item : group_1_items_new) {
            assignment_[item] = group_1;
          }
          cerr << "[LDM] q_index: " << q_index_
               << " g0: " << group_0 << " g1: " << group_1 << endl;
          run_update_post_process(group_0, group_1, true);
          return true;
        }
      }
    }

    return false;
  }

 public:
  Main() : random_(0) {}

  void Run() {
    cin >> n_ >> group_count_ >> q_count_;
    cerr << "n: "<<  n_
         << " group_count: " << group_count_
         << " q_count: " << q_count_ << endl;

    q_index_ = 0;

    invalid_move_table_ = vector<int>(n_);
    invalid_swap_table_ = vector<vector<int>>(n_, vector<int>(n_));

    pairwise_shuffle_failure_count_ = 0;

    if (q_count_ >= InitializationWithItemRankCost())  {
      RunInitializationWithItemRank();
    } else {
      item_order_matrix_ = vector<BitSetN>(n_);

      assignment_ = vector<int>(n_);
      for (int id = 0; id < n_; id++) {
        assignment_[id] = id % group_count_;
      }
      RunGroupMergeSortPhase();
    }

    WriteTemporarySolution();

    while (q_index_ < q_count_) {
      if (!RunSingleUpdatePhase()) {
        break;
      }
    }

    if (q_index_ < q_count_) {
      cerr << "Completed RunSingleUpdatePhase loop: " << q_index_
           << " / "<< q_count_ << endl;
      while (q_index_ < q_count_) {
        cout << "1 1 0 1" << endl;
        string result;
        cin >> result;
        q_index_++;
      }
    }

    for (int id = 0; id < n_; id++) {
      cout << assignment_[id] << " ";
    }
    cout << endl;
  }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(0);

  Main main;
  main.Run();
}
