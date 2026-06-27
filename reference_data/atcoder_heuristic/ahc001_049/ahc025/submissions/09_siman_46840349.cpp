#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <climits>
#include <random>
#include <map>
#include <queue>
#include <set>
#include <chrono>
#include <cstring>
#include <vector>

using namespace std;
using namespace std::chrono;
typedef long long ll;

const ll CYCLE_PER_SEC = 2700000000;

std::random_device seed_gen;
std::default_random_engine engine(seed_gen());

unsigned long long xor128() {
  static unsigned long long rx = 123456789, ry = 362436069, rz = 521288629, rw = 88675123;
  unsigned long long rt = (rx ^ (rx << 11));
  rx = ry;
  ry = rz;
  rz = rw;
  return (rw = (rw ^ (rw >> 19)) ^ (rt ^ (rt >> 8)));
}

unsigned long long int get_cycle() {
  unsigned int low, high;
  __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
  return ((unsigned long long int) low) | ((unsigned long long int) high << 32);
}

double get_time(unsigned long long int begin_cycle) {
  return (double) (get_cycle() - begin_cycle) / CYCLE_PER_SEC;
}

double get_time(high_resolution_clock::time_point &begin) {
  high_resolution_clock::time_point end = high_resolution_clock::now();
  ll count = duration_cast<microseconds>(end - begin).count();

  return count / 1000000.0;
}

const int MAX_N = 110;
const int MAX_D = 30;
const int UNDEFINED = -1;
const int BIG = 0;
const int SMALL = 1;
const int EQUAL = 2;
const int MERGE_CNT[71] = {
  119, 123, 128, 135, 140, 147, 152, 158, 163, 169, 175, 180, 186, 192, 199, 205, 210, 216, 222, 227, 233, 237, 244,
  252, 257, 263, 269, 274, 282, 287, 292, 299, 306, 310, 316, 324, 331, 337, 343, 351, 356, 363, 373, 378, 384, 390,
  397, 403, 414, 416, 423, 431, 438, 444, 452, 460, 465, 471, 478, 483, 490, 502, 506, 511, 518, 523, 531, 538, 548,
  553, 559
};

int g_comp_memo[MAX_N][MAX_N];
vector<int> g_above_graph[MAX_N];
int g_checked[MAX_N][MAX_N];
int g_check_counter;
int max_L[MAX_N];
bool g_failed[MAX_D][MAX_N][MAX_N];

int N, D, Q;
int credits;
bool g_sort_skip;

struct Item {
  int id;
  int w;

  Item(int id = -1, int w = -1) {
    this->id = id;
    this->w = w;
  }

  bool operator<(const Item &i) const {
    return w > i.w;
  }
};

vector <Item> g_items;

struct Box {
  int id;
  vector<int> item_ids;

  Box(int id = -1) {
    this->id = id;
  }
};

struct Result {
  int result;
  vector<int> L;
  vector<int> R;

  Result(int result = -1, vector<int> L = vector<int>(), vector<int> R = vector<int>()) {
    this->result = result;
    this->L = L;
    this->R = R;
  }
};

vector <Result> g_results;

void update_comp_table() {
  for (int root = 0; root < N; ++root) {
    queue<int> que;
    que.push(root);
    vector<bool> visited(N, false);

    while (not que.empty()) {
      int v = que.front();
      que.pop();

      if (visited[v]) continue;
      visited[v] = true;

      if (v != root && g_comp_memo[root][v] != EQUAL) {
        if (g_comp_memo[root][v] != UNDEFINED) {
          assert(SMALL == g_comp_memo[root][v]);
        }
        g_comp_memo[root][v] = SMALL;
        g_comp_memo[v][root] = BIG;
      }

      for (int u : g_above_graph[v]) {
        que.push(u);
      }
    }
  }
}

class BalancingByBalance {
public:

  void load_data() {
    memset(g_comp_memo, UNDEFINED, sizeof(g_comp_memo));

    cin >> N >> D >> Q;
    credits = Q;

    fprintf(stderr, "N = %d\n", N);
    fprintf(stderr, "D = %d\n", D);
    fprintf(stderr, "Q = %d\n", Q);

    for (int i = 0; i < N; ++i) {
      g_items.push_back(Item(i, 0));
      g_comp_memo[i][i] = EQUAL;
    }
  }

  void merge(vector <Item> &items, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    vector <Item> L(n1);
    vector <Item> R(n2);
    for (int i = 0; i < n1; ++i) {
      L[i] = items[l + i];
    }
    for (int j = 0; j < n2; ++j) {
      R[j] = items[m + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = l;

    while (i < n1 && j < n2) {
      if (comp(L[i].id, R[j].id) != BIG) {
        items[k] = L[i];
        ++i;
      } else {
        items[k] = R[j];
        ++j;
      }
      ++k;
    }

    while (i < n1) {
      items[k] = L[i];
      ++i;
      ++k;
    }

    while (j < n2) {
      items[k] = R[j];
      ++j;
      ++k;
    }
  }

  void merge_sort(vector <Item> &items, int l, int r) {
    if (l >= r) {
      return;
    }

    int m = l + (r - l) / 2;
    merge_sort(items, l, m);
    merge_sort(items, m + 1, r);
    merge(items, l, m, r);
  }

  int comp(Box &box1, Box &box2) {
    if (credits <= 0) {
      return BIG;
    }
    vector<int> L, R;
    vector<int> item_counter(N, 0);
    for (int id : box1.item_ids) {
      ++item_counter[id];
    }
    for (int id : box2.item_ids) {
      ++item_counter[id];
    }

    for (int id : box1.item_ids) {
      if (item_counter[id] >= 2) continue;
      L.push_back(id);
    }
    for (int id : box2.item_ids) {
      if (item_counter[id] >= 2) continue;
      R.push_back(id);
    }

    if (L.empty() && not R.empty()) {
      return SMALL;
    }
    if (not L.empty() && R.empty()) {
      return BIG;
    }
    if (L.empty() && R.empty()) {
      return EQUAL;
    }

    assert(L.size() > 0);
    assert(R.size() > 0);

    int res = send_query(L, R);
    g_results.push_back(Result(res, L, R));

    return res;
  }

  int comp(int id1, int id2) {
    if (g_comp_memo[id1][id2] != UNDEFINED) {
      return g_comp_memo[id1][id2];
    }

    vector<int> L, R;
    L.push_back(id1);
    R.push_back(id2);

    int ret = send_query(L, R);
    g_comp_memo[id1][id2] = ret;
    switch (ret) {
      case BIG:
        g_comp_memo[id2][id1] = SMALL;
        g_above_graph[id2].push_back(id1);
        break;
      case SMALL:
        g_comp_memo[id2][id1] = BIG;
        g_above_graph[id1].push_back(id2);
        break;
      case EQUAL:
        g_comp_memo[id2][id1] = EQUAL;
        break;
      default:
        assert(false);
    }
    update_comp_table();

    return ret;
  }

  int send_query(vector<int> &L, vector<int> &R) {
    if (credits <= 0) {
      return xor128() % 3;
    }
    assert(credits >= 0);
    --credits;
    string query = "";

    query += to_string(L.size()) + " ";
    query += to_string(R.size()) + " ";

    for (int l : L) {
      query += to_string(l) + " ";
    }
    for (int r : R) {
      query += to_string(r) + " ";
    }

    // fprintf(stderr, "credits: %d, query = %s\n", credits, query.c_str());
    cout << query << endl;

    string ret;
    cin >> ret;

    if (ret == ">") {
      return BIG;
    } else if (ret == "<") {
      return SMALL;
    } else if (ret == "=") {
      return EQUAL;
    } else {
      fprintf(stderr, "Unknown response: %s\n", ret.c_str());
      assert(false);
    }
  }

  bool re_sort_box_list(vector <Box> &box_list, int bid1, int bid2) {
    assert(bid1 < bid2);
    Box box1 = box_list[bid1];
    Box box2 = box_list[bid2];
    vector <Box> copy_box = box_list;

    box_list.erase(box_list.begin() + bid1);
    box_list.erase(box_list.begin() + bid2 - 1);

    int pos1 = search_insert_box_pos(box_list, box1);
    if (pos1 == -1) {
      box_list = copy_box;
      return false;
    }
    box_list.insert(box_list.begin() + pos1, box1);
    int first_pos = max(2, D - pos1 + 1);
    int pos2 = search_insert_box_pos(box_list, box2, first_pos);
    if (pos2 == -1) {
      box_list = copy_box;
      return false;
    }
    box_list.insert(box_list.begin() + pos2, box2);

    return true;
  }

  pair<int, int> select_swap_item_ids(Box &box1, Box &box2, map<int, int> &item_order, bool sort_completed) {
    int min_diff = INT_MAX;
    int from = -1;
    int to = -1;

    for (int i = 0; i < box1.item_ids.size(); ++i) {
      int id1 = box1.item_ids[i];

      for (int j = 0; j < box2.item_ids.size(); ++j) {
        int id2 = box2.item_ids[j];
        if (sort_completed && item_order[id1] < item_order[id2]) continue;
        if (g_checked[id1][id2] == g_check_counter) continue;

        bool ok = true;

        if (sort_completed) {
          if (max_L[id1] > item_order[id2]) continue;
          for (int l = 0; l < N && ok; ++l) {
            if (item_order[id1] < item_order[l]) continue;
            if (max_L[l] >= item_order[id2]) ok = false;
          }
        } else if (g_comp_memo[id1][id2] == SMALL) {
          for (int bid = 1; bid <= D; ++bid) {
            for (int l = 0; l < N; ++l) {
              for (int r = l + 1; r < N; ++r) {
                if (not g_failed[bid][l][r]) continue;
                if (g_comp_memo[l][id1] == BIG && g_comp_memo[r][id2] == SMALL) ok = false;
              }
            }
          }
        }

        int diff = 10 * abs(item_order[id1] - item_order[id2]);
        if (not sort_completed) diff += item_order[id1] + item_order[id2];
        if (ok && min_diff > diff) {
          min_diff = diff;
          from = i;
          to = j;
        }
      }
    }

    return make_pair(from, to);
  }

  void update_item_order(map<int, int> &item_order) {
    vector<int> win_counter(N, 0);
    vector<int> lose_counter(N, 0);
    for (int i = 0; i < N; ++i) {
      int v = 50;
      int small_cnt = 0;
      int big_cnt = 0;

      for (int j = 0; j < N; ++j) {
        if (g_comp_memo[i][j] == SMALL) small_cnt++;
        if (g_comp_memo[i][j] == BIG) big_cnt++;
      }

      win_counter[i] = big_cnt;
      lose_counter[i] = small_cnt;
      item_order[i] = v - small_cnt + big_cnt;
      if (small_cnt <= big_cnt) {
        item_order[i] += pow(big_cnt - small_cnt, 1.5);
      }
    }

    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        if (g_comp_memo[i][j] == UNDEFINED) continue;

        if (win_counter[i] > N - lose_counter[j]) {
          g_comp_memo[i][j] = BIG;
          g_comp_memo[j][i] = SMALL;
          g_above_graph[j].push_back(i);
        }
      }
    }

    update_comp_table();
  }

  vector <Box> swap_items(vector <Box> box_list, vector <Item> items, bool sort_completed,
                          high_resolution_clock::time_point &begin) {
    map<int, int> item_order;
    memset(g_failed, false, sizeof(g_failed));

    for (int i = 0; i < N; ++i) {
      Item &item = items[i];
      item_order[item.id] = i;
    }

    int limit = min(1000, 2 * N * D);
    if (not sort_completed) limit = 100000;
    g_check_counter = 0;
    memset(g_checked, -1, sizeof(g_checked));
    int max_order = 0;
    bool cant_move[MAX_N];
    memset(cant_move, false, sizeof(cant_move));
    double cur_time = get_time(begin);
    memset(max_L, 0, sizeof(max_L));
    int last_bid = box_list[D].id;
    int no_update_cnt[N];
    memset(no_update_cnt, 0, sizeof(no_update_cnt));
    int ll = limit;
    int type_list[5] = {0, 0, 1, 2, 2};
    int last_credits = credits;

    while (credits > 0 && limit > 0 && cur_time < 1.9) {
      if (not sort_completed && credits != last_credits) {
        last_credits = credits;
        update_item_order(item_order);
      }

      cur_time = get_time(begin);
      int type = type_list[xor128() % 5];
      if (sort_completed) {
        if (ll - limit < 2 * N) type = 0;
        if (ll - limit < N) type = 2;
      } else {
        type = 2;
        if (no_update_cnt[last_bid] > 100) type = 0;
        if (no_update_cnt[last_bid] > 300 && D >= 3) type = 1;
      }
      --limit;
      no_update_cnt[last_bid]++;

      if (type == 0) {
        Box box1 = box_list[1];
        Box box2 = box_list[D];

        pair<int, int> p = select_swap_item_ids(box1, box2, item_order, sort_completed);

        if (p.first == -1) continue;
        int id1 = box1.item_ids[p.first];
        int id2 = box2.item_ids[p.second];
        g_checked[id1][id2] = g_check_counter;
        swap(box1.item_ids[p.first], box2.item_ids[p.second]);
        if (not sort_completed && comp(id1, id2) != SMALL) continue;

        if (comp(box1, box_list[D]) != BIG) {
          memset(no_update_cnt, 0, sizeof(no_update_cnt));
          if (xor128() % 5 == 0) {
            g_check_counter++;
          }
          for (int i = 0; i < N; ++i) {
            if (not g_failed[1][id1][i]) continue;
            g_comp_memo[i][id2] = BIG;
            g_comp_memo[id2][i] = SMALL;
            g_above_graph[id2].push_back(i);
          }
          for (int i = 0; i < N; ++i) {
            if (not g_failed[1][i][id2]) continue;
            g_comp_memo[id1][i] = BIG;
            g_comp_memo[i][id1] = SMALL;
            g_above_graph[i].push_back(id1);
          }

          box_list[1] = box1;
          box_list[D] = box2;
          if (not re_sort_box_list(box_list, 1, D)) {
            return box_list;
          }

          last_bid = box_list[D].id;
        } else {
          max_L[id1] = max(max_L[id1], item_order[id2]);
          g_failed[1][id1][id2] = true;
        }
      } else if (type == 1) {
        int bid1 = (xor128() % (D / 2)) + 1;
        int bid2 = D - (xor128() % (D / 2));
        if (xor128() % 2 == 0) {
          bid1 = 1;
        } else {
          bid2 = D;
        }
        if (not sort_completed) {
          if (no_update_cnt[last_bid] / 500 >= D) {
            bid1 = 1;
            bid2 = max(2, 2 * D - no_update_cnt[last_bid] / 500 - 1);
          } else {
            bid1 = min(D - 1, 2 + no_update_cnt[last_bid] / 500);
            bid2 = D;
          }
        }

        if (bid1 >= bid2) continue;
        if (abs(bid1 - bid2) <= min(2, D - 3)) continue;

        Box box1 = box_list[bid1];
        Box box2 = box_list[bid2];

        pair<int, int> p = select_swap_item_ids(box1, box2, item_order, sort_completed);
        int from = p.first;
        int to = p.second;

        if (from == -1) continue;
        int id1 = box1.item_ids[from];
        int id2 = box2.item_ids[to];
        g_checked[id1][id2] = g_check_counter;
        swap(box1.item_ids[from], box2.item_ids[to]);
        if (not sort_completed && comp(id1, id2) != SMALL) continue;

        int ret1 = UNDEFINED;
        int ret2 = UNDEFINED;

        if (bid1 == 1) {
          if (credits <= 0) {
            ret2 = SMALL;
          } else {
            ret2 = comp(box2, box_list[1]);
          }

          if (ret2 == SMALL) {
            ret1 = BIG;
          }
        } else {
          ret1 = comp(box1, box_list[D]);

          if (ret1 != SMALL) {
            ret2 = UNDEFINED;
          } else if (bid2 == D) {
            ret2 = BIG;
          } else {
            ret2 = comp(box2, box_list[1]);
          }
        }

        if (ret1 != BIG && ret2 != SMALL) {
          memset(no_update_cnt, 0, sizeof(no_update_cnt));
          if (xor128() % 5 == 0) {
            g_check_counter++;
          }
          for (int i = 0; i < N; ++i) {
            if (not g_failed[bid1][id1][i]) continue;
            g_comp_memo[i][id2] = BIG;
            g_comp_memo[id2][i] = SMALL;
            g_above_graph[id2].push_back(i);
          }
          for (int i = 0; i < N; ++i) {
            if (not g_failed[bid1][i][id2]) continue;
            g_comp_memo[id1][i] = BIG;
            g_comp_memo[i][id1] = SMALL;
            g_above_graph[i].push_back(id1);
          }
          box_list[bid1] = box1;
          box_list[bid2] = box2;
          if (not re_sort_box_list(box_list, bid1, bid2)) {
            return box_list;
          }

          last_bid = box_list[D].id;
        } else {
          max_L[id1] = max(max_L[id1], item_order[id2]);
          g_failed[bid1][id1][id2] = true;
        }
      } else if (type == 2) {
        Box box1 = box_list[1];
        Box box2 = box_list[D];

        int from = xor128() % box2.item_ids.size();
        int id1 = box2.item_ids[from];

        if (not sort_completed) {
          int max_v = 0;
          for (int i = 0; i < box2.item_ids.size(); ++i) {
            int id = box2.item_ids[i];
            if (cant_move[id]) continue;
            int v = 0;
            if (max_order < item_order[id]) v -= 10000;
            v += item_order[id];

            if (max_v < v) {
              max_v = v;
              from = i;
              id1 = id;
            }
          }
        }
        box2.item_ids.erase(box2.item_ids.begin() + from);
        box1.item_ids.push_back(id1);
        if (cant_move[id1]) continue;
        if (sort_completed && max_order > item_order[id1]) continue;

        bool ok = true;
        for (int i = 0; i < N && ok; ++i) {
          if (cant_move[i] && g_comp_memo[i][id1] == SMALL) ok = false;
        }

        if (ok && comp(box1, box_list[D]) != BIG) {
          memset(no_update_cnt, 0, sizeof(no_update_cnt));
          box_list[1] = box1;
          box_list[D] = box2;
          if (not re_sort_box_list(box_list, 1, D)) {
            return box_list;
          }

          for (int i = 0; i < N; ++i) {
            if (cant_move[i]) {
              g_comp_memo[i][id1] = BIG;
              g_comp_memo[id1][i] = SMALL;
              g_above_graph[id1].push_back(i);
            }
          }

          last_bid = box_list[D].id;
        } else {
          cant_move[id1] = true;
          max_order = max(max_order, item_order[id1]);
        }
      }
    }

    return box_list;
  }

  int search_insert_box_pos(vector <Box> &box_list, Box &box, int first_pos = -1) {
    int ok = 1;
    int ng = box_list.size();
    while (abs(ok - ng) >= 2) {
      int idx = (ok + ng) / 2;
      assert(2 <= idx && idx <= D);

      if (credits <= 0) {
        return -1;
      }

      if (abs(ok - ng) == box_list.size() - 1 && first_pos != -1) {
        idx = max(2, first_pos);
      }

      if (comp(box, box_list[idx - 1]) != SMALL) {
        ok = idx;
      } else {
        ng = idx;
      }
    }

    return ok;
  }

  vector <Box> build_box_list() {
    vector <Box> box_list;
    box_list.push_back(Box(0));
    for (int i = 1; i <= D; ++i) {
      box_list.push_back(Box(i));
    }
    box_list.push_back(Box(D + 1));

    return box_list;
  }

  vector <Box> random_packing(vector <Item> items) {
    vector <Box> init_box_list = build_box_list();

    int mid = N - D;

    for (int i = 0; i < mid; ++i) {
      int bid = i % D + 1;
      Item &item = items[i];

      init_box_list[bid].item_ids.push_back(item.id);
    }

    vector <Box> box_list;
    box_list.push_back(Box(0));
    box_list.push_back(Box(D + 1));

    for (int i = mid; i < N; ++i) {
      Item item = items[i];
      assert(not init_box_list.empty());
      Box box = init_box_list[1];

      box.item_ids.push_back(item.id);
      init_box_list.erase(init_box_list.begin() + 1);

      int ok = 1;
      int ng = box_list.size();

      while (abs(ok - ng) >= 2) {
        int idx = (ok + ng) / 2;
        assert(2 <= idx && idx <= box_list.size());

        if (comp(box, box_list[idx - 1]) != SMALL) {
          ok = idx;
        } else {
          ng = idx;
        }
      }

      box_list.insert(box_list.begin() + ok, box);
    }

    return box_list;
  }

  vector <Box> packing(vector <Item> items, int n_mid = -1) {
    vector <Box> box_list = build_box_list();
    int mid = D;
    if (n_mid != -1) {
      mid = n_mid;
    }

    for (int i = 0; i < mid; ++i) {
      int bid = i % D + 1;
      Item &item = items[i];

      box_list[bid].item_ids.push_back(item.id);
    }
    reverse(box_list.begin(), box_list.end());

    int last_pos = D;

    for (int i = mid; i < N; ++i) {
      Item item = items[i];
      Box smallest_box = box_list[1];

      smallest_box.item_ids.push_back(item.id);
      box_list.erase(box_list.begin() + 1);

      int ok = 1;
      int ng = D + 1;
      while (abs(ok - ng) >= 2) {
        int idx = (ok + ng) / 2;
        assert(2 <= idx && idx <= D);

        if (abs(ok - ng) == D) {
          idx = max(2, last_pos);
        }

        if (comp(smallest_box, box_list[idx - 1]) != SMALL) {
          ok = idx;
        } else {
          ng = idx;
        }
      }

      last_pos = ok;
      box_list.insert(box_list.begin() + ok, smallest_box);
    }

    return box_list;
  }

  void run() {
    load_data();
    high_resolution_clock::time_point begin = high_resolution_clock::now();
    double rate = Q * 1.0 / N;
    int cnt = MERGE_CNT[N - 30] + (N - D) * log2(D);
    g_sort_skip = cnt > Q;

    if (not g_sort_skip) {
      merge_sort(g_items, 0, N - 1);
      reverse(g_items.begin(), g_items.end());
    }

    bool sort_completed = (not g_sort_skip && credits > 0);

    vector <Box> box_list = (g_sort_skip) ? random_packing(g_items) : packing(g_items);
    fprintf(stderr, "remain_credits = %d, rate: %.4f\n", credits, Q * 1.0 / N);

    box_list = swap_items(box_list, g_items, sort_completed, begin);

    while (credits >= 4) {
      map<int, int> item_order;
      for (int i = 0; i < N; ++i) {
        Item &item = g_items[i];
        item_order[item.id] = i;
      }

      credits -= 4;
      vector <Box> cur_list = box_list;
      for (int t = 0; t < 1; ++t) {
        int i, j, from, to;
        int type_list[5] = {0, 1, 1, 1, 1};
        do {
          int type = type_list[xor128() % 5];
          if (type == 0) {
            i = 1;
            j = xor128() % D + 1;
          } else if (type == 1) {
            i = xor128() % max(1, D / 5) + 1;
            j = D;
          }

          from = xor128() % cur_list[i].item_ids.size();
          to = xor128() % cur_list[j].item_ids.size();
        } while (i >= j);

        Box box1 = cur_list[i];
        Box box2 = cur_list[j];
        swap(box1.item_ids[from], box2.item_ids[to]);

        cur_list.erase(cur_list.begin() + i);
        cur_list.erase(cur_list.begin() + j - 1);

        int pos1 = max(1, search_insert_box_pos(cur_list, box1));
        cur_list.insert(cur_list.begin() + pos1, box1);
        int first_pos = max(2, D - pos1 + 1);
        int pos2 = max(1, search_insert_box_pos(cur_list, box2, first_pos));
        cur_list.insert(cur_list.begin() + pos2, box2);
      }
      bool ok = credits > 0;
      vector <Box> second_list = swap_items(cur_list, g_items, sort_completed, begin);

      credits += 4;
      if (not ok) break;

      if (credits > 4) {
        if (comp(box_list[1], second_list[1]) != BIG &&
            comp(second_list[D], box_list[D]) != BIG) {
          box_list = second_list;
        }
      } else {
        if (comp(box_list[1], second_list[1]) != BIG &&
            comp(second_list[D], box_list[D]) != BIG &&
            comp(second_list[1], second_list[2]) != BIG &&
            comp(second_list[D - 1], second_list[D]) != BIG) {
          box_list = second_list;
        }
      }
    }

    vector<int> ans(N, 0);
    for (int i = 1; i <= D; ++i) {
      Box &box = box_list[i];
      box.id = i;
      fprintf(stderr, "box.id = %d, box.item_ids.size() = %d\n", box.id, (int) box.item_ids.size());

      for (int id : box.item_ids) {
        ans[id] = box.id - 1;
      }
    }

    double cur_time = get_time(begin);
    fprintf(stderr, "elapsed_time: %.4f, not_use_credits: %d\n", cur_time, credits);

    while (credits > 0) {
      vector<int> L = {0};
      vector<int> R = {1};
      send_query(L, R);
    }

    for (int id : ans) {
      cout << id << " ";
    }
    cout << endl;
  }
};

int main() {
  BalancingByBalance bbb;
  bbb.run();

  return 0;
}