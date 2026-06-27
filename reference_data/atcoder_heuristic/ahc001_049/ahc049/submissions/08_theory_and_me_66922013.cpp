#include <bits/stdc++.h>
using namespace std;
// #include <atcoder/all>
// using namespace atcoder;

#define rep_(i, a_, b_, a, b, ...) for (int i = (a), lim##i = (b); i < lim##i; ++i)
#define rep(i, ...) rep_(i, __VA_ARGS__, __VA_ARGS__, 0, __VA_ARGS__)
#define drep_(i, a_, b_, a, b, ...) for (int i = (a) - 1, lim##i = (b); i >= lim##i; --i)
#define drep(i, ...) drep_(i, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, 0)
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#ifdef LOCAL
void debug_out() {
    cerr << endl;
}
template <class Head, class... Tail> void debug_out(Head H, Tail... T) {
    cerr << ' ' << H;
    debug_out(T...);
}
#define debug(...) cerr << 'L' << __LINE__ << " [" << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)
#define dump(x) cerr << 'L' << __LINE__ << " " << #x << " = " << (x) << endl
#else
#define debug(...) (void(0))
#define dump(x) (void(0))
#endif
template <class T> using V = vector<T>;
template <class T> using VV = V<V<T>>;
using ll = long long;
using ld = long double;
using Vi = V<int>;
using VVi = VV<int>;
using Vl = V<ll>;
using VVl = VV<ll>;
using Vd = V<ld>;
using VVd = VV<ld>;
using Vb = V<bool>;
using VVb = VV<bool>;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
template <class T> using priority_queue_rev = priority_queue<T, vector<T>, greater<T>>;
template <class T> vector<T> make_vec(size_t n, T a) {
    return vector<T>(n, a);
}
template <class... Ts> auto make_vec(size_t n, Ts... ts) {
    return vector<decltype(make_vec(ts...))>(n, make_vec(ts...));
}
template <class T> inline void fin(const T x) {
    cout << x << '\n';
    exit(0);
}
template <class T> inline void deduplicate(vector<T> &a) {
    sort(all(a));
    a.erase(unique(all(a)), a.end());
}
template <class T> inline bool chmin(T &a, const T b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}
template <class T> inline bool chmax(T &a, const T b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}
template <class T> inline int sz(const T &x) {
    return x.size();
}
template <class T> inline int count_between(const vector<T> &a, T l, T r) {
    return lower_bound(all(a), r) - lower_bound(all(a), l);
}
template <class T1, class T2> istream &operator>>(istream &is, pair<T1, T2> &p) {
    is >> p.first >> p.second;
    return is;
}
template <class T1, class T2> ostream &operator<<(ostream &os, const pair<T1, T2> &p) {
    os << '(' << p.first << ", " << p.second << ')';
    return os;
}
template <class T, size_t n> istream &operator>>(istream &is, array<T, n> &v) {
    for (auto &e : v) is >> e;
    return is;
}
template <class T, size_t n> ostream &operator<<(ostream &os, const array<T, n> &v) {
    for (auto &e : v) os << e << ' ';
    return os;
}
template <class T> istream &operator>>(istream &is, vector<T> &v) {
    for (auto &e : v) is >> e;
    return is;
}
template <class T> ostream &operator<<(ostream &os, const vector<T> &v) {
    for (auto &e : v) os << e << ' ';
    return os;
}
template <class T> istream &operator>>(istream &is, deque<T> &v) {
    for (auto &e : v) is >> e;
    return is;
}
template <class T> ostream &operator<<(ostream &os, const deque<T> &v) {
    for (auto &e : v) os << e << ' ';
    return os;
}
inline ll floor_div(ll x, ll y) {
    if (y < 0) x = -x, y = -y;
    return x >= 0 ? x / y : (x - y + 1) / y;
}
inline ll ceil_div(ll x, ll y) {
    if (y < 0) x = -x, y = -y;
    return x >= 0 ? (x + y - 1) / y : x / y;
}
inline int floor_log2(const ll x) {
    assert(x > 0);
    return 63 - __builtin_clzll(x);
}
inline int ceil_log2(const ll x) {
    assert(x > 0);
    return (x == 1) ? 0 : 64 - __builtin_clzll(x - 1);
}
inline int popcount(const ll x) {
    return __builtin_popcountll(x);
}
struct fast_ios {
    fast_ios() {
        cin.tie(nullptr);
        ios::sync_with_stdio(false);
        cout << fixed << setprecision(20);
    }
} fast_ios_;
constexpr int INF = numeric_limits<int>::max() >> 1;
// using mint = modint998244353;
// using mint = modint1000000007;
// using mint = modint;
// using Vm = V<mint>; using VVm = VV<mint>;
auto system_now = std::chrono::system_clock::now();
int check_time() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - system_now).count();
}
#if DEBUG
constexpr float machine_spec = 1.0;
#elif LOCAL
constexpr float machine_spec = 1.0;
#else
constexpr float machine_spec = 1.0;
#endif
constexpr ll time_limit = (2000 - 50);
struct Timer {
    chrono::system_clock::time_point start = chrono::system_clock::now();
    chrono::system_clock::time_point lap = chrono::system_clock::now();
    int elapsed_ms() {
        auto now = chrono::system_clock::now();
#if DEBUG or LOCAL
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() * machine_spec;
#else
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
#endif
    }
    int lap_ms() {
        auto now = chrono::system_clock::now();
#if DEBUG or LOCAL
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - lap).count() * machine_spec;
#else
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - lap).count();
#endif
    }
    void mark_lap() {
        lap = chrono::system_clock::now();
    }
} timer;
class xrand {
    uint64_t x;

   public:
    using result_type = uint32_t;
    static constexpr result_type min() {
        return std::numeric_limits<result_type>::min();
    }
    static constexpr result_type max() {
        return std::numeric_limits<result_type>::max();
    }
    xrand(uint64_t k)
        : x(k) {
    }
    xrand()
        : xrand(1) {
    }
    result_type operator()() {
        x ^= x << 9;
        return x ^= x >> 7;
    }
    int64_t operator()(int m) {
        return int64_t((*this)()) * m >> 32;
    }
    int64_t operator()(int l_include, int r_exlude) {
        return l_include + (*this)(r_exlude - l_include);
    }
    double rand() {
        return double((*this)()) / (int64_t(1) << 32);
    }
} rnd(998244353);

void print_2darray(const array<array<int, 20>, 20> &a) {
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            cerr << a[i][j] << ' ';
        }
        cerr << "\n";
    }
    cerr << "\n";
}

// 入力
constexpr int N = 20;
array<array<int, N>, N> weight, duration;

// 出力
void output_task_list(vector<vector<pii>> &tasks_list) {
    // 出力
    vector<char> res;
    rep(i, tasks_list.size()) {
        reverse(tasks_list[i].begin(), tasks_list[i].end());
        tasks_list[i].push_back({0, 0});  // 最後に(0, 0)に戻る
        int cx = 0, cy = 0;
        rep(j, tasks_list[i].size()) {
            int tx = tasks_list[i][j].first, ty = tasks_list[i][j].second;

            while (cx > tx) {
                res.push_back('U');
                cx--;
            }

            while (cx < tx) {
                res.push_back('D');
                cx++;
            }

            while (cy < ty) {
                res.push_back('R');
                cy++;
            }

            while (cy > ty) {
                res.push_back('L');
                cy--;
            }

            if (j < tasks_list[i].size() - 1) {
                res.push_back('1');  // 段ボールを追加
            }
        }
    }

    for (auto c : res) {
        cout << c << '\n';
    }
}

// 事前にマンハッタン距離をテーブル化しておく
static int dist_tbl[N * N][N * N];
inline void build_dist() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int u = i * N + j;
            for (int x = 0; x < N; x++)
                for (int y = 0; y < N; y++) {
                    int v = x * N + y;
                    dist_tbl[u][v] = abs(i - x) + abs(j - y);
                }
        }
    }
}

// inline int manhattan_distance(int x1, int y1, int x2, int y2) {
//     // u, vは0-indexedの座標
//     int u = x1 * N + y1;
//     int v = x2 * N + y2;
//     return dist_tbl[u][v];
// }

inline int manhattan_distance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// inline double get_temp(double start_temp, double end_temp, int elapsed_time, int total_time) {
//     return start_temp + (end_temp - start_temp) * elapsed_time / total_time;
// };

inline double get_temp(double start_temp, double end_temp, int elapsed_time, int total_time) {
    double t = double(elapsed_time) / total_time;  // 0.0 〜 1.0
    return start_temp * std::pow(end_temp / start_temp, t);
}

// 解く
void solve() {
    // 後ろから貪欲に決める
    vector<vector<pii>> tasks_list;
    array<array<int, N>, N> used;
    rep(i, N) {
        rep(j, N) {
            used[i][j] = 0;  // 使用済みフラグ
        }
    }
    int cnt = 0;
    while (true) {
        int cur_tl = 0, cur_damage = 0, cx = 0, cy = 0;
        vector<pii> tasks;
        while (true) {
            // 追加可能な段ボールを列挙
            vector<pii> available;
            rep(i, N) {
                rep(j, N) {
                    if (used[i][j]) continue;
                    if (cur_damage < duration[i][j]) {
                        available.emplace_back(i, j);
                    }
                }
            }

            // 追加する段ボールを選ぶ
            if (available.empty()) {
                break;
            }

            // 選び方は色々ある
            sort(all(available), [&](const pii &a, const pii &b) {
                // 距離が近いものを優先
                int da = abs(cx - a.first) + abs(cy - a.second);
                int db = abs(cx - b.first) + abs(cy - b.second);

                // 重みが大きいものを優先
                return da + 0.0001 * weight[a.first][a.second] < db + 0.0001 * weight[b.first][b.second];
            });

            pii target_pos = available[0];
            cur_tl += abs(cx - target_pos.first) + abs(cy - target_pos.second);
            cur_damage += weight[target_pos.first][target_pos.second] * cur_tl;
            cx = target_pos.first;
            cy = target_pos.second;
            used[target_pos.first][target_pos.second] = 1;  // 使用済みにする
            tasks.push_back({target_pos});
            cnt++;
        }
        dump(tasks);
        tasks_list.push_back(tasks);
        if (cnt == N * N - 1) {
            break;  // 全ての段ボールを追加したら終了
        }
    }

    // 焼きなましでtask_listを改善する
    vector<vector<pii>> cur_sol = tasks_list;  // 現在の解を保持
    int cur_score = 0;
    vector<int> tasks_score(tasks_list.size(), 0);
    rep(i, cur_sol.size()) {
        auto &tasks = cur_sol[i];
        int score = 0;
        score += manhattan_distance(0, 0, tasks[0].first, tasks[0].second);
        for (int j = 1; j < tasks.size(); j++) {
            score += manhattan_distance(tasks[j - 1].first, tasks[j - 1].second, tasks[j].first, tasks[j].second);
        }
        score += manhattan_distance(tasks.back().first, tasks.back().second, 0, 0);
        tasks_score[i] = score;
        cur_score += score;
    }
    dump(cur_score);

    auto calc_score_single_tasks = [&](const vector<pii> &tasks) {
        int score = 0;
        if (tasks.empty()) return 0;  // 空のタスクはスコア0
        score += manhattan_distance(0, 0, tasks[0].first, tasks[0].second);
        for (int j = 1; j < tasks.size(); j++) {
            score += manhattan_distance(tasks[j - 1].first, tasks[j - 1].second, tasks[j].first, tasks[j].second);
        }
        score += manhattan_distance(tasks.back().first, tasks.back().second, 0, 0);
        return score;
    };

    auto check_feasibility_single_tasks = [&](const vector<pii> &tasks) {
        int cur_tl = 0, cur_damage = 0, cx = 0, cy = 0;
        for (const auto &task : tasks) {
            int tx = task.first, ty = task.second;
            if (cur_damage >= duration[tx][ty]) {
                return false;  // 不可能なタスク
            }
            cur_tl += abs(cx - tx) + abs(cy - ty);
            cur_damage += weight[tx][ty] * cur_tl;
            cx = tx;
            cy = ty;
        }
        return true;  // 全てのタスクが可能
    };

    int itr_cnt = 0;
    double start_temp = 3;
    double end_temp = 0.1;
    vector<int> rand_range = {40, 60, 0};  // 遷移の確率
    vector<int> rand_range_cum(rand_range.size() + 1, 0);
    rep(i, rand_range.size()) {
        rand_range_cum[i + 1] = rand_range_cum[i] + rand_range[i];
    }
    int start_time = timer.elapsed_ms();
    int usable_time = time_limit - start_time;

    int best_score = cur_score;
    vector<vector<pii>> best_sol = cur_sol;  // 最良解を保持

    while (true) {
        int elapsed_time = timer.elapsed_ms() - start_time;
        if (elapsed_time > usable_time) {
            break;  // 時間切れ
        }
        rep(_, 1024) {
            int random_dice = rnd(100);
            if (random_dice < rand_range_cum[1]) {
                // ランダムスワップ
                int task1_idx = rnd(cur_sol.size());
                int task2_idx = rnd(cur_sol.size());
                if (cur_sol[task1_idx].empty() || cur_sol[task2_idx].empty()) {
                    continue;  // 空のタスクはスキップ
                }
                int task1_pos = rnd(cur_sol[task1_idx].size());
                int task2_pos = rnd(cur_sol[task2_idx].size());
                swap(cur_sol[task1_idx][task1_pos], cur_sol[task2_idx][task2_pos]);

                if (!check_feasibility_single_tasks(cur_sol[task1_idx]) || !check_feasibility_single_tasks(cur_sol[task2_idx])) {
                    // 不可能なタスクができた場合は元に戻す
                    swap(cur_sol[task1_idx][task1_pos], cur_sol[task2_idx][task2_pos]);
                    continue;
                }

                // スコアを計算
                int new_score1 = calc_score_single_tasks(cur_sol[task1_idx]);
                int new_score2 = calc_score_single_tasks(cur_sol[task2_idx]);
                int old_score1 = tasks_score[task1_idx];
                int old_score2 = tasks_score[task2_idx];
                double score_delta = (new_score1 - old_score1) + (new_score2 - old_score2);
                double temp = get_temp(start_temp, end_temp, elapsed_time, usable_time);
                double prob = exp(-score_delta / temp);
                if ((score_delta < 0) || rnd.rand() < prob) {
                    // スコアが改善された場合のみ更新
                    tasks_score[task1_idx] = new_score1;
                    tasks_score[task2_idx] = new_score2;
                    cur_score += score_delta;

                    if (cur_score < best_score) {
                        best_score = cur_score;
                        best_sol = cur_sol;  // 最良解を更新
                    }
                } else {
                    // スコアが悪化した場合は元に戻す
                    swap(cur_sol[task1_idx][task1_pos], cur_sol[task2_idx][task2_pos]);
                }
            } else if (random_dice < rand_range_cum[2]) {
                // タスクを削除し，別の場所に挿入
                int task_idx = rnd(cur_sol.size());
                int task_pos = rnd(cur_sol[task_idx].size());
                int new_task_idx = rnd(cur_sol.size());
                int new_task_pos = rnd(cur_sol[new_task_idx].size());
                // if (task_idx == new_task_idx) {
                //     continue;  // 同じタスクはスキップ
                // }
                if (cur_sol[task_idx].empty()) {
                    continue;  // 空のタスクはスキップ
                }

                auto back_up_task = cur_sol[task_idx];
                auto back_up_task_new = cur_sol[new_task_idx];

                pii removed_task = cur_sol[task_idx][task_pos];
                cur_sol[task_idx].erase(cur_sol[task_idx].begin() + task_pos);  // タスクを削除
                cur_sol[new_task_idx].insert(cur_sol[new_task_idx].begin() + new_task_pos, removed_task);

                // 挿入後のタスクが可能かチェック
                if (!check_feasibility_single_tasks(cur_sol[new_task_idx]) or !check_feasibility_single_tasks(cur_sol[task_idx])) {
                    // 不可能なタスクができた場合は元に戻す
                    if (task_idx == new_task_idx) {
                        cur_sol[task_idx].erase(cur_sol[task_idx].begin() + new_task_pos);
                        cur_sol[task_idx].insert(cur_sol[task_idx].begin() + task_pos, removed_task);
                    } else {
                        cur_sol[new_task_idx].erase(cur_sol[new_task_idx].begin() + new_task_pos);
                        cur_sol[task_idx].insert(cur_sol[task_idx].begin() + task_pos, removed_task);
                    }
                    continue;
                }
                // スコアを計算
                int new_score1 = calc_score_single_tasks(cur_sol[task_idx]);
                int new_score2 = calc_score_single_tasks(cur_sol[new_task_idx]);
                int old_score1 = tasks_score[task_idx];
                int old_score2 = tasks_score[new_task_idx];
                double score_delta = (new_score1 - old_score1) + (new_score2 - old_score2);
                double temp = get_temp(start_temp, end_temp, elapsed_time, usable_time);
                double prob = exp(-score_delta / temp);
                if ((score_delta < 0) || rnd.rand() < prob) {
                    // スコアが改善された場合のみ更新
                    tasks_score[task_idx] = new_score1;
                    tasks_score[new_task_idx] = new_score2;
                    cur_score += score_delta;

                    // もしもtask_idxが空になったら，そのタスクを削除
                    if (cur_sol[task_idx].empty()) {
                        cur_sol.erase(cur_sol.begin() + task_idx);
                        tasks_score.erase(tasks_score.begin() + task_idx);
                    }

                    if (cur_score < best_score) {
                        best_score = cur_score;
                        best_sol = cur_sol;  // 最良解を更新
                    }
                } else {
                    // スコアが悪化した場合は元に戻す
                    if (task_idx == new_task_idx) {
                        cur_sol[task_idx].erase(cur_sol[task_idx].begin() + new_task_pos);
                        cur_sol[task_idx].insert(cur_sol[task_idx].begin() + task_pos, removed_task);
                    } else {
                        cur_sol[new_task_idx].erase(cur_sol[new_task_idx].begin() + new_task_pos);
                        cur_sol[task_idx].insert(cur_sol[task_idx].begin() + task_pos, removed_task);
                    }
                }
            } else if (random_dice < rand_range_cum[3]) {
                // 連続する2個のタスクをまとめて移動する
                int task_idx = rnd(cur_sol.size());
                if (cur_sol[task_idx].size() < 2) {
                    continue;  // タスクが1つ以下の場合はスキップ
                }
                int task_pos1 = rnd(cur_sol[task_idx].size());
                int task_pos2 = task_pos1;
                while (task_pos2 == task_pos1) {
                    task_pos2 = rnd(cur_sol[task_idx].size());
                }
                if (task_pos1 > task_pos2) {
                    swap(task_pos1, task_pos2);  // task_pos1を小さい方に揃える
                }
                int new_task_idx = rnd(cur_sol.size());
                int new_task_pos = rnd(cur_sol[new_task_idx].size());
                // if (task_idx == new_task_idx) {
                //     continue;  // 同じタスクはスキップ
                // }
                if (cur_sol[task_idx].empty()) {
                    continue;  // 空のタスクはスキップ
                }
                auto back_up_task = cur_sol[task_idx];
                vector<pii> removed_tasks(cur_sol[task_idx].begin() + task_pos1, cur_sol[task_idx].begin() + task_pos2 + 1);
                cur_sol[task_idx].erase(cur_sol[task_idx].begin() + task_pos1, cur_sol[task_idx].begin() + task_pos2 + 1);  // タスクを削除
                cur_sol[new_task_idx].insert(cur_sol[new_task_idx].begin() + new_task_pos, removed_tasks.begin(), removed_tasks.end());
                // 挿入後のタスクが可能かチェック
                if (!check_feasibility_single_tasks(cur_sol[new_task_idx]) or !check_feasibility_single_tasks(cur_sol[task_idx])) {
                    // 不可能なタスクができた場合は元に戻す
                    cur_sol[task_idx].insert(cur_sol[task_idx].begin() + task_pos1, removed_tasks.begin(), removed_tasks.end());
                    cur_sol[new_task_idx].erase(cur_sol[new_task_idx].begin() + new_task_pos, cur_sol[new_task_idx].begin() + new_task_pos + removed_tasks.size());
                    continue;
                }
                // スコアを計算
                int new_score1 = calc_score_single_tasks(cur_sol[task_idx]);
                int new_score2 = calc_score_single_tasks(cur_sol[new_task_idx]);
                int old_score1 = tasks_score[task_idx];
                int old_score2 = tasks_score[new_task_idx];
                double score_delta = (new_score1 - old_score1) + (new_score2 - old_score2);
                double temp = get_temp(start_temp, end_temp, elapsed_time, usable_time);
                double prob = exp(-score_delta / temp);  // 確率を計算
                if ((score_delta < 0) || rnd.rand() < prob) {
                    // スコアが改善された場合のみ更新
                    tasks_score[task_idx] = new_score1;
                    tasks_score[new_task_idx] = new_score2;
                    cur_score += score_delta;

                    // もしもtask_idxが空になったら，そのタスクを削除
                    if (cur_sol[task_idx].empty()) {
                        cur_sol.erase(cur_sol.begin() + task_idx);
                        tasks_score.erase(tasks_score.begin() + task_idx);
                    }

                    if (cur_score < best_score) {
                        best_score = cur_score;
                        best_sol = cur_sol;  // 最良解を更新
                    }
                } else {
                    // スコアが悪化した場合は元に戻す
                    cur_sol[task_idx].insert(cur_sol[task_idx].begin() + task_pos1, removed_tasks.begin(), removed_tasks.end());
                    cur_sol[new_task_idx].erase(cur_sol[new_task_idx].begin() + new_task_pos, cur_sol[new_task_idx].begin() + new_task_pos + removed_tasks.size());
                }
            }
            itr_cnt++;
        }
    }
    dump(itr_cnt);
    rep(i, cur_sol.size()) {
        dump(cur_sol[i]);
    }

    // 出力
    output_task_list(cur_sol);

    return;
}

int main(int argc, char *argv[]) {
#if DEBUG
    freopen("tools/in/0000.txt", "r", stdin);
#endif
    int _;
    cin >> _;
    rep(i, N) {
        rep(j, N) {
            cin >> weight[i][j];
        }
    }
    rep(i, N) {
        rep(j, N) {
            cin >> duration[i][j];
        }
    }

    // build_dist();  // マンハッタン距離のテーブルを構築

    solve();
}