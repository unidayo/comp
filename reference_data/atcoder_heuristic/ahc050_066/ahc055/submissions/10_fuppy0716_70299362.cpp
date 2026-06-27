//*
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
//*/

#include <bits/stdc++.h>
// #include <atcoder/all>

using namespace std;
// using namespace atcoder;

// #define _GLIBCXX_DEBUG
#ifdef LOCAL
#define DEBUG(x) cerr << #x << ": " << x << endl;
#define DEBUG_VEC(v)                                                           \
    cerr << #v << ":";                                                         \
    for (int iiiiiiii = 0; iiiiiiii < v.size(); iiiiiiii++)                    \
        cerr << " " << v[iiiiiiii];                                            \
    cerr << endl;
#define DEBUG_MAT(v)                                                           \
    cerr << #v << endl;                                                        \
    for (int iv = 0; iv < v.size(); iv++) {                                    \
        for (int jv = 0; jv < v[iv].size(); jv++) {                            \
            cerr << v[iv][jv] << " ";                                          \
        }                                                                      \
        cerr << endl;                                                          \
    }
#else
#define DEBUG(x) ;
#define DEBUG_VEC(v) ;
#define DEBUG_MAT(v) ;
#endif
typedef long long ll;
// #define int ll

#define vi vector<int>
#define vl vector<ll>
#define vii vector<vector<int>>
#define vll vector<vector<ll>>
#define pii pair<int, int>
#define pis pair<int, string>
#define psi pair<string, int>
#define pll pair<ll, ll>
template <class S, class T>
pair<S, T> operator+(const pair<S, T> &s, const pair<S, T> &t) {
    return pair<S, T>(s.first + t.first, s.second + t.second);
}
template <class S, class T>
pair<S, T> operator-(const pair<S, T> &s, const pair<S, T> &t) {
    return pair<S, T>(s.first - t.first, s.second - t.second);
}
template <class S, class T> ostream &operator<<(ostream &os, pair<S, T> p) {
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define rep1(i, n) for (int i = 1; i <= (int)(n); i++)
#define rrep(i, n) for (int i = (int)(n) - 1; i >= 0; i--)
#define rrep1(i, n) for (int i = (int)(n); i > 0; i--)
#define REP(i, a, b) for (int i = a; i < b; i++)
#define in(x, a, b) (a <= x && x < b)
#define all(c) c.begin(), c.end()
void YES(bool t = true) { cout << (t ? "YES" : "NO") << endl; }
void Yes(bool t = true) { cout << (t ? "Yes" : "No") << endl; }
void yes(bool t = true) { cout << (t ? "yes" : "no") << endl; }
void NO(bool t = true) { cout << (t ? "NO" : "YES") << endl; }
void No(bool t = true) { cout << (t ? "No" : "Yes") << endl; }
void no(bool t = true) { cout << (t ? "no" : "yes") << endl; }
template <class T> bool chmax(T &a, const T &b) {
    if (a < b) {
        a = b;
        return 1;
    }
    return 0;
}
template <class T> bool chmin(T &a, const T &b) {
    if (a > b) {
        a = b;
        return 1;
    }
    return 0;
}
template <class T, class U> T ceil_div(T a, U b) { return (a + b - 1) / b; }
template <class T> T safe_mul(T a, T b) {
    if (a == 0 || b == 0)
        return 0;
    if (numeric_limits<T>::max() / a < b)
        return numeric_limits<T>::max();
    return a * b;
}
#define UNIQUE(v) v.erase(std::unique(v.begin(), v.end()), v.end());
const ll inf = 1000000001;
const ll INF = (ll)1e18 + 1;
const long double pi = 3.1415926535897932384626433832795028841971L;
int popcount(ll t) { return __builtin_popcountll(t); }
vector<int> gen_perm(int n) {
    vector<int> ret(n);
    iota(all(ret), 0);
    return ret;
}
// int dx[4] = {1, 0, -1, 0}, dy[4] = {0, 1, 0, -1};
// int dx2[8] = { 1,1,0,-1,-1,-1,0,1 }, dy2[8] = { 0,1,1,1,0,-1,-1,-1 };
vi dx = {0, 0, -1, 1}, dy = {-1, 1, 0, 0};
vi dx2 = {1, 1, 0, -1, -1, -1, 0, 1}, dy2 = {0, 1, 1, 1, 0, -1, -1, -1};
struct Setup_io {
    Setup_io() {
        ios_base::sync_with_stdio(0), cin.tie(0), cout.tie(0);
        cout << fixed << setprecision(25);
        cerr << fixed << setprecision(25);
    }
} setup_io;
// constexpr ll MOD = 1000000007;
constexpr ll MOD = 998244353;
// #define mp make_pair

auto start = chrono::system_clock::now();
constexpr double TL = 1.99;

double calc_seconds() {
    return static_cast<double>(chrono::duration_cast<chrono::microseconds>(
                                   chrono::system_clock::now() - start)
                                   .count() /
                               1000000.0);
}

class HRandom {
  public:
    HRandom() {
        random_mw = 1985;
        random_mz = 2012;
    }
    int intRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return result >> 1;
    }
    unsigned int uintRandom_65535() {
        // random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        // random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        // return random_mw & 65535;
        const unsigned tmp = random_mw & 65535;
        random_mw = 18000 * tmp + (random_mw >> 16);
        return tmp;
    }
    unsigned int uintRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return result;
    }
    double dblRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return (result + 1.0) * 2.328306435454494e-10;
    }

  private:
    unsigned random_mw;
    unsigned random_mz;
};

HRandom hrandom;

static uint32_t randxor() {
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;
    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}
void randxor_init() {}

double randdouble() { return hrandom.dblRandom(); }
bool randbool(const float &p = 0.5) {
    return hrandom.uintRandom_65535() < (p * 65535);
}

inline int rnd(const int n) {
    assert(0 <= n);

    if (n == 1) {
        return 0;
    }
    return ((hrandom.uintRandom_65535()) * n) >> 16;
}

int rnd(const int &l, const int &r) {
    assert(l <= r);
    return rnd(r - l) + l;
}

constexpr int LARGE_A_SIZE = 200;
constexpr int LARGE_A_SIZE_SWAP = 50;
constexpr int MAX_SWAP_ORDER_AND_TARGET_DIST = 100;
constexpr int SEE_NUM = 5;

class Parameter {
  public:
    Parameter() {}

    static Parameter from_env() {
        return Parameter();
        // return Parameter(getenv("START_TEMP") ? stof(getenv("START_TEMP"))
        //                                       : START_TEMP);
    }
};

Parameter param;
std::default_random_engine engine(42);

constexpr int N = 200;
vi H(N), C(N);

vii A(N, vi(N));
vii large_A_perm(N, vi(N));

void input() {
    int dummy;
    cin >> dummy;
    rep(i, N) { cin >> H[i]; }
    rep(i, N) { cin >> C[i]; }

    rep(i, N) {
        vector<pii> perm;
        rep(j, N) {
            cin >> A[i][j];
            if (i == j) {
                A[i][j] = 0;
            }
            perm.push_back({A[i][j], j});
        }
        sort(all(perm));
        reverse(all(perm));
        rep(j, N) { large_A_perm[i][j] = perm[j].second; }
    }
}

struct AnnealingState {
    using Cost = int;

    Cost score;
    Cost best_score;

    double start_sec, end_sec;

    double temperature;

    // Problem-specific state
    vector<int> order;        // chest open order (by H ascending, fixed)
    vector<int> pos_in_order; // position of each chest (weapon unlock time)
    vector<vector<int>> plan;

    vector<vector<int>> best_plan; // snapshot of best plan
    vector<int> best_order;        // snapshot of best order
    vi damage_sum;
    vii attack_count; // attack_count[x][y] = x から y への攻撃回数

    AnnealingState(double end_sec)
        : start_sec(calc_seconds()), end_sec(end_sec) {
        init();
    }

    void init() {
        build_initial();

        best_score = inf;
        update_best();
    }

    void build_initial() {

        build_initial_plan();

        // initial score
        score = evaluate_current_plan();

        best_plan = plan;
        best_score = inf;
    }

    void build_initial_plan() {
        // 貪欲な初期解構築
        order.clear();
        plan.resize(N);
        damage_sum.assign(N, 0);
        pos_in_order.assign(N, -1);
        attack_count.assign(N, vi(N, 0));

        vector<bool> selected(N, false);
        vector<int> current_hp = H; // 現在のHP

        // 体力が最も低い順に武器を選択し、攻撃先を決定
        for (int turn = 0; turn < N; turn++) {
            // 攻撃先 / 体力が最も大きいもの
            double max_score = -1;
            int next_weapon = -1;
            for (int i = 0; i < N; i++) {
                if (current_hp[i] <= 0 && !selected[i]) {
                    next_weapon = i;
                    break;
                }
                double score = (double)C[i] / current_hp[i];
                if (!selected[i] && score > max_score) {
                    max_score = score;
                    next_weapon = i;
                }
            }

            // orderに追加
            order.push_back(next_weapon);
            pos_in_order[next_weapon] = turn;
            selected[next_weapon] = true;

            // この武器から攻撃する先を決定（まだ選択されていない中で攻撃力最大）
            rep(_, C[next_weapon]) {
                int max_attack = 0;
                int best_target = -1;
                for (int target = 0; target < N; target++) {
                    if (selected[target]) {
                        continue;
                    }
                    int dam = A[next_weapon][target];
                    if (dam > max_attack) {
                        max_attack = dam;
                        best_target = target;
                    }
                }

                if (best_target == -1) {
                    break;
                }

                plan[next_weapon].push_back(best_target);
                current_hp[best_target] -= max_attack;
                damage_sum[best_target] += max_attack;
                attack_count[next_weapon][best_target]++;
            }
        }
    }

    void solve() {
        double start_temp = 10.0, end_temp = 0.1;

        for (int try_num = 0;; try_num++) {
            if ((try_num & 31) == 0) {
                double sec = calc_seconds();
                if (should_stop(try_num, sec)) {
                    DEBUG(try_num);
                    break;
                }

                double rate = decide_rate(try_num, sec);
                set_temperature(rate, start_temp, end_temp);
            }

            step();
        }
    }

    inline bool should_stop(int try_num, double sec) const {
#ifdef FREEZE
        return try_num > 10000000;
#else
        return sec > end_sec;
#endif
    }

    double decide_rate(int try_num, double sec) const {
#ifdef FREEZE
        // フリーズしている場合は、10000回に1回の確率で温度を下げる
        return try_num / 10000000.0;
#else
        return (sec - start_sec) / (end_sec - start_sec);
#endif
    }

    void set_temperature(double rate, double start_temp, double end_temp) {
        temperature = start_temp + (end_temp - start_temp) * rate;
        // temperature = start_temp * pow(end_temp / start_temp, rate);
    }

    // Operation 0: Add new target to weapon
    void try_add_target() {
        int w = rnd(N);
        if (pos_in_order[w] == N - 1 || plan[w].size() == C[w]) {
            return;
        }

        int pos_w = pos_in_order[w];
        int tar = large_A_perm[w][rnd(LARGE_A_SIZE)];
        if (pos_in_order[tar] <= pos_w) {
            return;
        }

        Cost diff = 0;
        diff -= sude_num_to(tar);

        // Apply change
        plan[w].push_back(tar);
        diff++;
        damage_sum[tar] += A[w][tar];
        attack_count[w][tar]++;

        diff += sude_num_to(tar);

        // Evaluate and accept/reject
        if (should_accept(diff)) {
            score += diff;
            update_best();
        } else {
            // Rollback: remove the added target
            plan[w].pop_back();
            damage_sum[tar] -= A[w][tar];
            attack_count[w][tar]--;
        }
    }

    // Operation 1: Change existing target
    void try_change_target(bool to_same) {
        int w = rnd(N);
        if (pos_in_order[w] == N - 1 || plan[w].size() == 0) {
            return;
        }

        int pos_w = pos_in_order[w];
        int idx = rnd(plan[w].size());
        int tar;

        if (to_same) {
            tar = plan[w][rnd(plan[w].size())];
        } else {
            static vi seen;
            seen.clear();
            rep(_, 5) { seen.push_back(large_A_perm[w][rnd(LARGE_A_SIZE)]); }

            int max_sude = 0;
            int best_tar = -1;
            for (int tar : seen) {
                int sude = sude_num_to(tar);
                if (sude > max_sude) {
                    max_sude = sude;
                    best_tar = tar;
                }
            }

            tar = best_tar;
            if (tar == -1) {
                return;
            }
        }

        if (pos_in_order[tar] <= pos_w) {
            return;
        }

        // Save original target for rollback
        int original_tar = plan[w][idx];

        if (tar == original_tar) {
            return;
        }

        // Apply change
        Cost diff = 0;
        diff -= sude_num_to(original_tar);
        diff -= sude_num_to(tar);

        damage_sum[original_tar] -= A[w][original_tar];
        attack_count[w][original_tar]--;
        plan[w][idx] = tar;
        damage_sum[tar] += A[w][tar];
        attack_count[w][tar]++;

        diff += sude_num_to(original_tar);
        diff += sude_num_to(tar);

        if (should_accept(diff)) {
            score += diff;
            update_best();
        } else {
            // Rollback: restore original target
            plan[w][idx] = original_tar;
            damage_sum[original_tar] += A[w][original_tar];
            attack_count[w][original_tar]++;
            damage_sum[tar] -= A[w][tar];
            attack_count[w][tar]--;
        }
    }

    // Operation 2: Swap targets between two weapons
    void try_swap_targets() {
        int w = rnd(N);
        int w2 = rnd(N);
        if (pos_in_order[w] == N - 1 || pos_in_order[w2] == N - 1 ||
            plan[w].size() == 0 || plan[w2].size() == 0) {
            return;
        }

        int idx1 = rnd(plan[w].size());
        int idx2 = rnd(plan[w2].size());
        int tar1 = plan[w][idx1];
        int tar2 = plan[w2][idx2];

        if (pos_in_order[w] >= pos_in_order[tar2] ||
            pos_in_order[w2] >= pos_in_order[tar1]) {
            return;
        }

        // Apply change
        Cost diff = 0;
        diff -= sude_num_to(tar1);
        diff -= sude_num_to(tar2);

        damage_sum[tar1] -= A[w][tar1];
        damage_sum[tar2] -= A[w2][tar2];
        attack_count[w][tar1]--;
        attack_count[w2][tar2]--;

        swap(plan[w][idx1], plan[w2][idx2]);

        damage_sum[tar1] += A[w2][tar1];
        damage_sum[tar2] += A[w][tar2];
        attack_count[w2][tar1]++;
        attack_count[w][tar2]++;

        diff += sude_num_to(tar1);
        diff += sude_num_to(tar2);

        // Evaluate and accept/reject
        if (should_accept(diff)) {
            score += diff;
            update_best();
        } else {
            // Rollback: swap back
            damage_sum[tar1] -= A[w2][tar1];
            damage_sum[tar2] -= A[w][tar2];
            attack_count[w2][tar1]--;
            attack_count[w][tar2]--;
            swap(plan[w][idx1], plan[w2][idx2]);
            damage_sum[tar1] += A[w][tar1];
            damage_sum[tar2] += A[w2][tar2];
            attack_count[w][tar1]++;
            attack_count[w2][tar2]++;
        }
    }

    // Operation 3: Swap order and change target
    void try_swap_order_and_target_move_to_before() {
        int w = rnd(N);
        if (plan[w].size() == 0) {
            return;
        }

        int pos_w = pos_in_order[w];
        int idx = rnd(plan[w].size());

        static vi seen;
        seen.clear();
        rep(_, 5) { seen.push_back(large_A_perm[w][rnd(LARGE_A_SIZE)]); }

        int max_sude = 0;
        int best_tar = -1;
        for (int tar : seen) {
            int sude = sude_num_to(tar);
            if (sude > max_sude) {
                max_sude = sude;
                best_tar = tar;
            }
        }

        int swap_tar = best_tar;
        if (swap_tar == -1) {
            return;
        }

        if (damage_sum[swap_tar] >= H[swap_tar]) {
            return;
        }

        int pos_swap_tar = pos_in_order[swap_tar];

        if (pos_in_order[swap_tar] >= pos_w) {
            return;
        }

        for (int i = pos_swap_tar; i < pos_w; i++) {
            // attack_countを使用してDAGチェック
            if (attack_count[order[i]][w] > 0) {
                return;
            }
        }

        // Save original target for rollback
        int original_tar = plan[w][idx];

        // Apply changes
        Cost diff = 0;
        diff -= sude_num_to(original_tar);
        diff -= sude_num_to(swap_tar);

        attack_count[w][original_tar]--;
        damage_sum[original_tar] -= A[w][original_tar];

        plan[w][idx] = swap_tar;

        attack_count[w][swap_tar]++;
        damage_sum[swap_tar] += A[w][swap_tar];

        diff += sude_num_to(swap_tar);
        diff += sude_num_to(original_tar);

        // Evaluate and accept/reject
        if (should_accept(diff)) {
            score += diff;
            assert(order[pos_w] == w);
            order.erase(order.begin() + pos_w);
            order.insert(order.begin() + pos_swap_tar, w);
            for (int i = pos_swap_tar; i <= pos_w; i++) {
                pos_in_order[order[i]] = i;
            }
            update_best();
        } else {
            // Rollback: restore order and target
            // rollback order swap by positions
            plan[w][idx] = original_tar;
            damage_sum[original_tar] += A[w][original_tar];
            attack_count[w][original_tar]++;
            damage_sum[swap_tar] -= A[w][swap_tar];
            attack_count[w][swap_tar]--;
        }
    }

    void try_swap_order_and_target_move_to_after() {
        int w = rnd(N);
        if (plan[w].size() == 0) {
            return;
        }

        int pos_w = pos_in_order[w];
        int idx = rnd(plan[w].size());

        static vi seen;
        seen.clear();
        rep(_, 5) { seen.push_back(large_A_perm[w][rnd(LARGE_A_SIZE)]); }

        int max_sude = 0;
        int best_tar = -1;
        for (int tar : seen) {
            int sude = sude_num_to(tar);
            if (sude > max_sude) {
                max_sude = sude;
                best_tar = tar;
            }
        }

        int swap_tar = best_tar;
        if (swap_tar == -1) {
            return;
        }

        if (damage_sum[swap_tar] >= H[swap_tar]) {
            return;
        }

        int pos_swap_tar = pos_in_order[swap_tar];

        if (pos_in_order[swap_tar] >= pos_w) {
            return;
        }

        static int static_cnt = 0;
        static_cnt++;
        static vi tars_of_swap(N, -1);

        for (int tar2 : plan[swap_tar]) {
            tars_of_swap[tar2] = static_cnt;
        }

        for (int i = pos_swap_tar + 1; i <= pos_w; i++) {
            if (tars_of_swap[order[i]] == static_cnt) {
                // 交換によって DAG じゃなくなる
                return;
            }
        }

        // Save original target for rollback
        int original_tar = plan[w][idx];

        // Apply changes
        Cost diff = 0;
        diff -= sude_num_to(original_tar);
        diff -= sude_num_to(swap_tar);
        attack_count[w][original_tar]--;
        damage_sum[original_tar] -= A[w][original_tar];

        plan[w][idx] = swap_tar;

        attack_count[w][swap_tar]++;
        damage_sum[swap_tar] += A[w][swap_tar];

        diff += sude_num_to(swap_tar);
        diff += sude_num_to(original_tar);

        // Evaluate and accept/reject
        if (should_accept(diff)) {
            score += diff;
            assert(order[pos_w] == w);
            order.insert(order.begin() + pos_w + 1, swap_tar);
            order.erase(order.begin() + pos_swap_tar);
            for (int i = pos_swap_tar; i <= pos_w; i++) {
                pos_in_order[order[i]] = i;
            }
            update_best();
        } else {
            // Rollback: restore order and target
            // rollback order swap by positions
            plan[w][idx] = original_tar;
            damage_sum[original_tar] += A[w][original_tar];
            damage_sum[swap_tar] -= A[w][swap_tar];
            attack_count[w][original_tar]++;
            attack_count[w][swap_tar]--;
        }
    }

    // Operation 3: Swap order and change target
    void try_swap_order_move_to_before() {
        int w = rnd(N);
        if (plan[w].size() == 0) {
            return;
        }

        int pos_w = pos_in_order[w];
        if (pos_w == 0) {
            return;
        }
        int pos_swap_tar = rnd(pos_w);

        for (int i = pos_swap_tar; i < pos_w; i++) {
            if (attack_count[order[i]][w] > 0) {
                return;
            }
        }

        assert(order[pos_w] == w);
        order.erase(order.begin() + pos_w);
        order.insert(order.begin() + pos_swap_tar, w);
        for (int i = pos_swap_tar; i <= pos_w; i++) {
            pos_in_order[order[i]] = i;
        }
        update_best();
    }

    void try_swap_order_move_to_after() {
        int w = rnd(N);
        if (plan[w].size() == 0) {
            return;
        }

        int pos_w = pos_in_order[w];
        if (pos_w == 0) {
            return;
        }
        int pos_swap_tar = rnd(pos_w);
        int swap_tar = order[pos_swap_tar];

        static int static_cnt = 0;
        static_cnt++;
        static vi tars_of_swap(N, -1);

        for (int tar2 : plan[swap_tar]) {
            tars_of_swap[tar2] = static_cnt;
        }

        for (int i = pos_swap_tar + 1; i <= pos_w; i++) {
            if (tars_of_swap[order[i]] == static_cnt) {
                // 交換によって DAG じゃなくなる
                return;
            }
        }

        assert(order[pos_w] == w);
        order.insert(order.begin() + pos_w + 1, swap_tar);
        order.erase(order.begin() + pos_swap_tar);
        for (int i = pos_swap_tar; i <= pos_w; i++) {
            pos_in_order[order[i]] = i;
        }
        update_best();
    }

    void step() {
        int move_type = rnd(27);

        if (move_type < 5) {
            try_add_target();
        } else if (move_type < 10) {
            try_change_target(rnd(10) == 0);
        } else if (move_type < 15) {
            try_swap_targets();
        } else if (move_type < 20) {
            try_swap_order_and_target_move_to_before();
        } else if (move_type < 25) {
            try_swap_order_and_target_move_to_after();
        } else if (move_type < 26) {
            try_swap_order_move_to_before();
        } else if (move_type < 27) {
            try_swap_order_move_to_after();
        }
    }

    int sude_num_to(int idx) const {
        if (damage_sum[idx] >= H[idx]) {
            return 0;
        }
        return H[idx] - damage_sum[idx];
    }

    bool should_accept(Cost score_diff) const {
        if (score_diff <= 0) {
            return true;
        }
        return randdouble() < exp(-score_diff / temperature);
    }

    bool update_best() {
        if (score < best_score) {
            best_score = score;
            best_plan = plan;
            best_order = order;
            DEBUG(best_score);
            return true;
        }
        return false;
    }

    // Evaluate current plan: simulate opening chests in fixed order
    Cost evaluate_current_plan() const {
        vi h = H;

        int ans = 0;
        for (int i : order) {
            if (h[i] > 0) {
                ans += h[i];
            }
            for (int tar : plan[i]) {
                if (pos_in_order[tar] <= pos_in_order[i]) {
                    continue;
                }
                h[tar] -= A[i][tar];
                ans++;
            }
        }
        return ans;
    }

    void output_best_solution() const {
        vi h = H;

        for (int i : best_order) {
            if (h[i] > 0) {
                rep(_, h[i]) { cout << -1 << " " << i << endl; }
            }
            for (int tar : best_plan[i]) {
                if (h[tar] > 0) {
                    cout << i << " " << tar << endl;
                    h[tar] -= A[i][tar];
                }
            }
        }
    }
};

signed main(int argc, char **argv) {
    input();
    AnnealingState state(TL);
    state.solve();
    state.output_best_solution();

    int h_sum = 0;
    rep(i, N) { h_sum += H[i]; }
    DEBUG(h_sum);
    DEBUG(state.best_score);
    DEBUG(h_sum - state.best_score);
}
