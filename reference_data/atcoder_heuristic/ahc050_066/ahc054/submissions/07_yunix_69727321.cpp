
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <chrono>
#include <algorithm>
#include <random>
#include <map>
#include <set>
#include <queue>
#include <random>
#include <chrono>
#include <cmath>
#include <optional>
#include <climits>
#include <bitset>
#include <time.h>
#include <array>
#include <functional>

using namespace std;
using Ps = pair<short, short>;
using vec_int = vector<int>;
using P = pair<int, int>;
using P2 = pair<P, P>;
using P3 = pair<float, int>;
using T = tuple<int, int, int>;
using T2 = tuple<float, int, int>;
using T3 = tuple<float, int, int, int, int>;
using T4 = tuple<int, int, int, int>;
using T5 = tuple<int, int, int, int, int>;
using TT = tuple<int, int, int>;
using ll = long long;
using uc = unsigned char;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)

int N;
int ti, tj;
vector<string> b;
int px, py;
vector<int> ns;
vector<vector<P>> observations;
vector<vector<bool>> observed;

// [I 2025 - 09 - 29 06:08:31, 800] Trial 5 finished with value : 3.683658612728676 and parameters : {'a' : 0.5388826159520994, 'b' : 0, 'c' : 3.6684210443516054, 'd' : 0.16601015240086273, 'e' : 1.0293807392988763, 'f' : 4.118447738499178, 'g' : 0.32545392437979515, 'h' : 1.0162347801953246, 'i' : 0.42266914216171775, 'j' : 19.322487378325476, 'k' : 0.18198541373636587}.Best is trial 5 with value : 3.683658612728676.

/*
float horizontal_or_vertical_threshold = 0.5388826159520994;
bool skip_even_runs = true;
float horizontal_upper_coeff1 = 3.6684210443516054;
float horizontal_upper_coeff2 = 0.16601015240086273;
float horizontal_lower_coeff = 1.0293807392988763;
float vertical_upper_coeff1 = 4.118447738499178;
float vertical_upper_coeff2 = 0.32545392437979515;
float vertical_lower_coeff = 1.0162347801953246;
float score_calc_utikiri = 0.42266914216171775;
float run_num_coeff1 = 19.322487378325476;
float run_num_coeff2 = 0.18198541373636587;
*/

// [I 2025 - 09 - 29 06:28:52, 445] Trial 0 finished with value : 3.664116701933014 and parameters : {'a' : 0.5321927959173632, 'b' : 1, 'c' : 3.4464622699510734, 'd' : 0.15242998950754263, 'e' : 1.216860685004704, 'f' : 3.647395393510278, 'g' : 0.16666930197758756, 'h' : 0.8221546528464148, 'i' : 0.43301294727298417, 'j' : 17.661221313431135, 'k' : 0.10149264761663761}.Best is trial 0 with value : 3.664116701933014.
// best params = {'a': 0.7736310756578961, 'b': 1, 'c': 3.4002705292120075, 'd': 0.13020964122313006, 'e': 0.9968734632562317, 'f': 3.246439673785938, 'g': 0.3101023938868108, 'h': 1.314653850635214, 'i': 0.655662090229995, 'j': 23.59453511993124, 'k': 0.43349331764432164}
/**/
float horizontal_or_vertical_threshold = 0.7736310756578961;
bool skip_even_runs = true;
float horizontal_upper_coeff1 = 3.4002705292120075;
float horizontal_upper_coeff2 = 0.13020964122313006;
float horizontal_lower_coeff = 0.9968734632562317;
float vertical_upper_coeff1 = 3.246439673785938;
float vertical_upper_coeff2 = 0.3101023938868108;
float vertical_lower_coeff = 1.314653850635214;
float score_calc_utikiri = 0.655662090229995;
float run_num_coeff1 = 23.59453511993124;
float run_num_coeff2 = 0.43349331764432164;

/*
                                                                                                  float horizontal_or_vertical_threshold = 0.75;
bool skip_even_runs = true;
float horizontal_upper_coeff1 = 3.5;
float horizontal_upper_coeff2 = 0.12;
float horizontal_lower_coeff = 1.5;
float vertical_upper_coeff1 = 3.5;
float vertical_upper_coeff2 = 0.3;
float vertical_lower_coeff = 1.0;
float score_calc_utikiri = 0.75;
float run_num_coeff1 = 20.0;
float run_num_coeff2 = 0.0;
*/

// #pragma GCC optimize("Ofast")


std::mt19937 mt{12345};
int RAND_ARR_LEN = 100000;
int RAND_RANGE = 1000000000;
float TIME_LIMIT = 1800;
float INV_TIME_LIMIT = 1. / 1000.;
const int JOB_MAX_LEN = 1003;
std::uniform_int_distribution<int> dist(1, RAND_RANGE);

int WAITING_TASK = 1001;

void remove_val_and_pop_from_last(vec_int &A, int val)
{
    for (int i = 0; i < A.size(); i++)
    {
        if (A.at(i) == val)
        {
            A.at(i) = A.at(A.size() - 1);
            A.pop_back();
            return;
        }
    }
    throw runtime_error("trying to remove non-existing value");
}

class Rand
{
private:
    int count = 0;
    vec_int rand_arr;

public:
    Rand(){
        rep(i, RAND_ARR_LEN){
            rand_arr.push_back(dist(mt));
}
}
;
int get();
int get_rand(int from, int to);
float get_float();
}
;

int Rand::get()
{
    int num = rand_arr.at(count);
    count += 1;
    count %= RAND_ARR_LEN;
    return num;
}

int Rand::get_rand(int from, int to)
{
    int diff = to - from;
    int num = get() % diff;
    return num + from;
}

float Rand::get_float()
{
    // 0~1の間の一様乱数
    return (float)get() / (float)RAND_RANGE;
}

Rand ro;

class PseudoSet
{
private:
    vec_int index_arr;

public:
    vec_int data;
    PseudoSet() {};
    PseudoSet(int value_range)
    {
        index_arr = vec_int(value_range, -1);
    }

    bool count(int value);
    void insert(int value);
    void erase(int value);
    vec_int get_data() { return data; };
    int size() { return data.size(); };
    int get_random() { return data.at(ro.get_rand(0, size())); };
};

bool PseudoSet::count(int value)
{
    return index_arr[value] != -1;
}

void PseudoSet::insert(int value)
{
    if (count(value))
        return;
    index_arr[value] = data.size();
    data.push_back(value);
}

void PseudoSet::erase(int value)
{
    if (!count(value))
    {
        // throw runtime_error("no existing value:"+to_string(value));
        return;
    }
    int tail_value = data[data.size() - 1];
    if (value == tail_value)
    {
        data.pop_back();
        index_arr[value] = -1;
    }
    else
    {
        index_arr[tail_value] = index_arr[value];
        index_arr[value] = -1;
        data[index_arr[tail_value]] = tail_value;
        data.pop_back();
    }
}

float get_time(bool init)
{
    static std::chrono::system_clock::time_point start;
    if (init)
    {
        start = std::chrono::system_clock::now();
    }
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); // 処理に要した時間をミリ秒に変換
}

double current_tmp(double current_time, double T0, double T1)
{
    return T1 + (T0 - T1) * (TIME_LIMIT * 0.92 - current_time) / (TIME_LIMIT * 0.92);
    // double start_time = TIME_LIMIT * 0.6;
    // double x = (current_time - start_time) / (TIME_LIMIT * 0.32);
    // return pow(T1, 1 - x) + (T0, x);
}

bool is_valid_transition(int diff, double current_time, double T0, float T1)
{
    float t = current_tmp(current_time, T0, T1);
    float rand = ro.get_float();
    // diffが正だったら常にアクセぷと
    return rand < exp(((float)diff) / t);
}
PseudoSet observed_positions = PseudoSet(40 * 40);




enum CellType
{
    EMPTY,
    TREE,
    FLOWER,
};

enum ConstructionType
{
    Tree
};

struct ConstructInstruction
{
    int x, y;
    ConstructionType type;
};




enum CheckType
{
    MustTree,
    MustEmpty,
    TreeOrEmpty,
    MustFlower,
};

struct CheckGrid
{
    int dx, dy;
    CheckType type;
};

pair<bool, vector<ConstructInstruction>> can_make_objects(
    int center_x, int center_y, vector<vector<CellType>> &field, vector<CheckGrid> &check_grids)
{

    vector<ConstructInstruction> instructions;
    for (auto check_grid : check_grids)
    {
        int nx = center_x + check_grid.dx;
        int ny = center_y + check_grid.dy;

        // 盤面の外にあるものはtree扱いになる
        if (nx < 0 || nx >= N || ny < 0 || ny >= N)
        {
            if (check_grid.type == MustTree || check_grid.type == TreeOrEmpty)
            {
                continue;
            }
            else
            {
                return {false, {}};
            }
        }

        if (check_grid.type == MustTree && (field[nx][ny] == CellType::TREE || field[nx][ny] == CellType::EMPTY))
        {
            // 基本的にはOK
            // emptyの時にはconstructionをする
            if (field[nx][ny] == CellType::EMPTY)
            {
                instructions.push_back({nx, ny, ConstructionType::Tree});
            }
            continue;
        }
        else if (check_grid.type == MustEmpty && field[nx][ny] == CellType::EMPTY)
        {
            continue;
        }
        else if (check_grid.type == TreeOrEmpty && (field[nx][ny] == CellType::EMPTY || field[nx][ny] == CellType::TREE))
        {
            continue;
        }
        else if (check_grid.type == MustFlower && field[nx][ny] == CellType::FLOWER)
        {
            continue;
        }
        else
        {
            // それ以外はだめ
            return {false, {}};
        }
    }

    return {true, instructions};
}

// displacementベクトルを指定した角度で回転させる
// angle: 90, 180, 270 (度)
vector<CheckGrid> rotate_displacement(vector<CheckGrid> &displacement, int angle)
{
    vector<CheckGrid> rotated;
    rotated.reserve(displacement.size());

    for (const auto &[dx, dy, type] : displacement)
    {
        int new_dx, new_dy;

        switch (angle)
        {
        case 90:
            // 90度回転: (x, y) -> (-y, x)
            new_dx = -dy;
            new_dy = dx;
            break;
        case 180:
            // 180度回転: (x, y) -> (-x, -y)
            new_dx = -dx;
            new_dy = -dy;
            break;
        case 270:
            // 270度回転: (x, y) -> (y, -x)
            new_dx = dy;
            new_dy = -dx;
            break;
        default:
            // 0度またはその他の場合はそのまま
            new_dx = dx;
            new_dy = dy;
            break;
        }

        rotated.push_back({new_dx, new_dy, type});
    }

    return rotated;
}

// bfsで新しく置かれたブロックの周りの空白地点が繋がっているかを確認する
// 繋がらなかったらfalseを返す
// bfsを全てやると時間がかかるので、newly_placed_treesの周りcheck_sizeマスだけを確認する

int connectivity_check_count = 0;
bool check_bfs_connectivity(vector<vector<CellType>> &field, vector<P> &newly_placed_trees, int check_size, bool torelate_small_disconnected = false)
{
    connectivity_check_count++;
    static vector<vector<int>> visited = vector<vector<int>>(N, vector<int>(N, -1));

    int max_x = -1, min_x = N, max_y = -1, min_y = N;
    for (auto &[x, y] : newly_placed_trees)
    {
        max_x = max(max_x, x);
        min_x = min(min_x, x);
        max_y = max(max_y, y);
        min_y = min(min_y, y);
    }

    vector<P> empty_cells;
    // 周囲1マスだけを調べる
    static const vector<int> dx_vec = {-1, 1, 0, 0};
    static const vector<int> dy_vec = {0, 0, -1, 1};
    for (auto &[x, y] : newly_placed_trees)
    {
        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx_vec[dir];
            int ny = y + dy_vec[dir];
            if (nx >= 0 && nx < N && ny >= 0 && ny < N && field[nx][ny] == EMPTY)
            {
                empty_cells.push_back({nx, ny});
            }
        }
    }
    if (empty_cells.size() == 0)
    {
        // 周囲に空白がない場合は繋がっていないとみなす
        return false;
    }

    // newly_placed_treesの最初のますをスタート地点にして、そこからtreeでないマスをbfsで探索する。ただし、範囲はmax, minで定められているところだけ
    queue<P> que;
    que.push(empty_cells[0]);
    while (!que.empty())
    {
        auto [x, y] = que.front();
        que.pop();
        if (visited[x][y] == connectivity_check_count)
            continue;
        visited[x][y] = connectivity_check_count;

        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx_vec[dir];
            int ny = y + dy_vec[dir];
            if (nx >= 0 && nx < N && ny >= 0 && ny < N &&
                visited[nx][ny] != connectivity_check_count &&
                field[nx][ny] != TREE && field[nx][ny] != FLOWER && // FLOWERも通れないようにする
                nx >= min_x - check_size && nx <= max_x + check_size &&
                ny >= min_y - check_size && ny <= max_y + check_size)
            {
                que.push({nx, ny});
            }
        }
    }
    int cnt = 0;
    for (auto &[x, y] : empty_cells)
    {
        if (visited[x][y] != connectivity_check_count)
        {
            cnt++;
        }
    }
    return cnt == 0 || (torelate_small_disconnected && cnt <= 2);
}



// ===================== Small utilities =====================
struct Bounds
{
    double x1_lo, x1_hi;
    double x2_lo, x2_hi;
};

struct BOOptions
{
    int init_per_branch = 5; // initial points per branch
    unsigned seed = 42;
    bool adaptive_xi = true;   // adapt xi with remaining hint
    double xi_base = 0.02;     // base exploration param for EI
    int reopt_hyper_every = 7; // re-optimize GP hyperparams every N tells on that branch
    int ei_multistart = 64;    // initial restarts for EI optimization
    int ei_grad_iters = 25;    // projected gradient steps per start
    double ei_step = 0.15;     // initial step size in normalized space
    double jitter = 1e-8;      // K diagonal jitter
};

struct Candidate
{
    bool b;
    double x1, x2;
};

static inline double clampd(double x, double lo, double hi)
{
    return std::max(lo, std::min(hi, x));
}

static inline double rand01(std::mt19937 &rng)
{
    return std::uniform_real_distribution<double>(0.0, 1.0)(rng);
}

// Latin Hypercube samples in 2D on [0,1]^2
static vector<array<double, 2>> lhs_2d(int n, std::mt19937 &rng)
{
    vector<double> u(n), v(n);
    for (int i = 0; i < n; i++)
    {
        u[i] = (i + rand01(rng)) / n;
        v[i] = (i + rand01(rng)) / n;
    }
    shuffle(v.begin(), v.end(), rng);
    vector<array<double, 2>> xs(n);
    for (int i = 0; i < n; i++)
        xs[i] = {u[i], v[i]};
    return xs;
}

// ===================== Tiny Linear Algebra =====================
struct Chol
{
    // Cholesky of SPD K (n x n). Stores lower-triangular L s.t. K = L L^T
    int n = 0;
    vector<double> L; // row-major, but we only use lower part
    bool ok = false;
    bool decompose(const vector<double> &K, int n_)
    {
        n = n_;
        L.assign(n * n, 0.0);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j <= i; j++)
            {
                double sum = K[i * n + j];
                for (int k = 0; k < j; k++)
                    sum -= L[i * n + k] * L[j * n + k];
                if (i == j)
                {
                    if (sum <= 0.0)
                    {
                        ok = false;
                        return false;
                    }
                    L[i * n + j] = sqrt(max(sum, 1e-18));
                }
                else
                {
                    L[i * n + j] = sum / L[j * n + j];
                }
            }
        }
        ok = true;
        return true;
    }
    // Solve K x = b using L
    void solve(vector<double> &x, const vector<double> &b) const
    {
        x = b;
        // forward: L y = b
        for (int i = 0; i < n; i++)
        {
            double sum = x[i];
            for (int k = 0; k < i; k++)
                sum -= L[i * n + k] * x[k];
            x[i] = sum / L[i * n + i];
        }
        // backward: L^T x = y
        for (int i = n - 1; i >= 0; i--)
        {
            double sum = x[i];
            for (int k = i + 1; k < n; k++)
                sum -= L[k * n + i] * x[k];
            x[i] = sum / L[i * n + i];
        }
    }
    // Solve L y = b
    void fwd(vector<double> &y, const vector<double> &b) const
    {
        y = b;
        for (int i = 0; i < n; i++)
        {
            double sum = y[i];
            for (int k = 0; k < i; k++)
                sum -= L[i * n + k] * y[k];
            y[i] = sum / L[i * n + i];
        }
    }
    // Solve L^T x = y
    void bwd(vector<double> &x, const vector<double> &y) const
    {
        x = y;
        for (int i = n - 1; i >= 0; i--)
        {
            double sum = x[i];
            for (int k = i + 1; k < n; k++)
                sum -= L[k * n + i] * x[k];
            x[i] = sum / L[i * n + i];
        }
    }
};

// ===================== GP per branch =====================
struct GP
{
    // Normalized inputs in [0,1]^2, outputs z-scored
    vector<array<double, 2>> X;
    vector<double> y; // standardized
    bool fitted = false;

    // data stats for y
    double y_mean = 0.0, y_std = 1.0;

    // Hyperparameters in log space:
    // ell1, ell2, sigma_f, sigma_n
    array<double, 4> th = {log(0.2), log(0.2), log(1.0), log(1e-2)};

    // Cached K, chol, alpha=(K^-1) y
    Chol chol;
    vector<double> alpha;

    double jitter = 1e-8;

    static inline double sqr(double x) { return x * x; }

    double kernel(const array<double, 2> &a, const array<double, 2> &b, const array<double, 4> &th_) const
    {
        double ell1 = exp(th_[0]);
        double ell2 = exp(th_[1]);
        double sf2 = sqr(exp(th_[2]));
        double dx1 = (a[0] - b[0]) / ell1;
        double dx2 = (a[1] - b[1]) / ell2;
        double r2 = 0.5 * (dx1 * dx1 + dx2 * dx2);
        return sf2 * exp(-r2);
    }

    // Build K + sn2*I and compute chol & alpha
    bool build_cache(const array<double, 4> &th_)
    {
        int n = (int)X.size();
        if (n == 0)
        {
            fitted = false;
            return false;
        }
        vector<double> K(n * n);
        double sn2 = sqr(exp(th_[3]));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j <= i; j++)
            {
                double val = kernel(X[i], X[j], th_);
                if (i == j)
                    val += sn2 + jitter;
                K[i * n + j] = val;
                K[j * n + i] = val;
            }
        }
        Chol ch;
        if (!ch.decompose(K, n))
            return false;
        vector<double> al;
        ch.solve(al, y);
        chol = std::move(ch);
        alpha = std::move(al);
        return true;
    }

    // Log marginal likelihood
    double lml(const array<double, 4> &th_)
    {
        int n = (int)X.size();
        if (n == 0)
            return 0.0;
        vector<double> K(n * n);
        double sn2 = sqr(exp(th_[3]));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j <= i; j++)
            {
                double val = kernel(X[i], X[j], th_);
                if (i == j)
                    val += sn2 + jitter;
                K[i * n + j] = val;
                K[j * n + i] = val;
            }
        }
        Chol ch;
        if (!ch.decompose(K, n))
            return -1e100;
        // log |K| = 2 * sum log diag(L)
        double logdet = 0.0;
        for (int i = 0; i < n; i++)
            logdet += log(ch.L[i * n + i]);
        logdet *= 2.0;
        // solve: alpha = K^-1 y
        vector<double> al;
        ch.solve(al, y);
        double quad = 0.0;
        for (int i = 0; i < n; i++)
            quad += y[i] * al[i];
        // -0.5 y^T K^-1 y -0.5 log|K| - n/2 log(2pi)
        return -0.5 * quad - 0.5 * logdet - 0.5 * n * log(2 * M_PI);
    }

    void standardize_y()
    {
        int n = (int)y.size();
        if (n == 0)
        {
            y_mean = 0;
            y_std = 1;
            return;
        }
        double m = 0;
        for (double v : y)
            m += v;
        m /= n;
        double s2 = 0;
        for (double v : y)
            s2 += (v - m) * (v - m);
        s2 = (n > 1 ? s2 / (n - 1) : 1.0);
        double s = sqrt(max(1e-16, s2));
        y_mean = m;
        y_std = s;
        for (double &v : y)
            v = (v - y_mean) / y_std;
    }

    void fit(bool reopt = true, std::mt19937 *rng = nullptr)
    {
        // recompute y stats and cache
        standardize_y();
        if ((int)X.size() == 0)
        {
            fitted = false;
            return;
        }
        if ((int)X.size() <= 2)
        {
            // keep conservative noise
            th[3] = log(1e-1);
        }
        if (reopt && rng)
        {
            // Simple random-restart search on th (log-space)
            array<double, 4> best = th;
            double best_lml = -1e300;
            auto try_one = [&](array<double, 4> cand)
            {
                double val = lml(cand);
                if (val > best_lml)
                {
                    best_lml = val;
                    best = cand;
                }
            };
            // current
            try_one(th);
            // a few random starts around plausible ranges
            for (int r = 0; r < 16; r++)
            {
                array<double, 4> cand = th;
                // sample log-ell in [-2.5, 0.3], log-sf in [-1, 1], log-sn in [-6, 0]
                cand[0] = std::uniform_real_distribution<double>(-2.5, 0.3)(*rng);
                cand[1] = std::uniform_real_distribution<double>(-2.5, 0.3)(*rng);
                cand[2] = std::uniform_real_distribution<double>(-1.0, 1.0)(*rng);
                cand[3] = std::uniform_real_distribution<double>(-6.0, 0.0)(*rng);
                try_one(cand);
            }
            th = best;
        }
        fitted = build_cache(th);
        if (!fitted)
        {
            // fallback: increase noise
            th[3] = log(1e-1);
            fitted = build_cache(th);
        }
    }

    // Predict mean and variance at x in [0,1]^2, on standardized y-scale
    pair<double, double> predict01(const array<double, 2> &x) const
    {
        int n = (int)X.size();
        if (!fitted || n == 0)
            return {0.0, 1.0}; // prior
        vector<double> k(n);
        for (int i = 0; i < n; i++)
            k[i] = kernel(x, X[i], th);
        // mean = k^T alpha
        double mu = 0.0;
        for (int i = 0; i < n; i++)
            mu += k[i] * alpha[i];
        // var = k(x,x) - k^T K^-1 k
        // Compute v = L^-1 k
        vector<double> v(n);
        chol.fwd(v, k);
        double kxx = sqr(exp(th[2])); // sigma_f^2 (since RBF at zero distance)
        double var = max(1e-16, kxx - inner_product(v.begin(), v.end(), v.begin(), 0.0));
        return {mu, var};
    }

    // De-standardize mean/var to original y-scale
    pair<double, double> destandardize(double mu, double var) const
    {
        double MU = mu * y_std + y_mean;
        double VAR = var * (y_std * y_std);
        return {MU, VAR};
    }

    void add_point01(const array<double, 2> &x01, double y_raw)
    {
        X.push_back(x01);
        y.push_back(y_raw); // will be standardized in fit()
    }

    int size() const { return (int)X.size(); }
};

// ===================== EI Acquisition =====================
struct EIComputer
{
    const GP *gp = nullptr;
    double fbest; // global best on original y-scale
    double xi;    // exploration parameter (on original scale)
    // Internally compute EI using standardized GP outputs

    static inline double phi(double z) { return 1.0 / sqrt(2 * M_PI) * exp(-0.5 * z * z); }
    static inline double Phi(double z) { return 0.5 * (1.0 + std::erf(z / sqrt(2.0))); }

    double operator()(const array<double, 2> &x01) const
    {
        if (!gp || gp->size() == 0)
            return 1e-3; // encourage sampling
        auto [mu_std, var_std] = gp->predict01(x01);
        if (var_std <= 1e-18)
            return 0.0;
        // bring fbest & xi to standardized scale:
        double fbest_std = (fbest - gp->y_mean) / gp->y_std;
        double xi_std = xi / gp->y_std;
        double sigma = sqrt(var_std);
        double imp = mu_std - fbest_std - xi_std;
        double z = imp / sigma;
        double ei = imp * Phi(z) + sigma * phi(z);
        if (!isfinite(ei))
            return 0.0;
        return max(0.0, ei);
    }
};

// Project [0,1]^2
static inline void project01(array<double, 2> &x)
{
    x[0] = clampd(x[0], 0.0, 1.0);
    x[1] = clampd(x[1], 0.0, 1.0);
}

// Finite-diff gradient of EI
static array<double, 2> numerical_grad(function<double(const array<double, 2> &)> f,
                                       array<double, 2> x, double h = 1e-4)
{
    array<double, 2> g{};
    for (int d = 0; d < 2; ++d)
    {
        array<double, 2> xp = x, xm = x;
        xp[d] = clampd(xp[d] + h, 0.0, 1.0);
        xm[d] = clampd(xm[d] - h, 0.0, 1.0);
        double fp = f(xp), fm = f(xm);
        g[d] = (fp - fm) / max(1e-12, (xp[d] - xm[d]));
    }
    return g;
}

// Multi-start projected gradient ascent for EI (2D, light-weight)
static array<double, 2> maximize_ei(const EIComputer &ei,
                                    int multistart,
                                    int iters,
                                    double step0,
                                    std::mt19937 &rng)
{
    auto f = [&](const array<double, 2> &x)
    { return ei(x); };
    double best_val = -1e300;
    array<double, 2> best_x{0.5, 0.5};

    // initial starts from LHS
    auto starts = lhs_2d(multistart, rng);

    for (auto s : starts)
    {
        array<double, 2> x = s;
        double step = step0;
        double val = f(x);
        for (int it = 0; it < iters; ++it)
        {
            auto g = numerical_grad(f, x);
            // normalize grad
            double gn = sqrt(g[0] * g[0] + g[1] * g[1]) + 1e-12;
            g[0] /= gn;
            g[1] /= gn;
            // backtracking line search (few steps)
            bool improved = false;
            for (int bt = 0; bt < 6; ++bt)
            {
                array<double, 2> xn = {x[0] + step * g[0], x[1] + step * g[1]};
                project01(xn);
                double vn = f(xn);
                if (vn >= val + 1e-9)
                {
                    x = xn;
                    val = vn;
                    improved = true;
                    break;
                }
                step *= 0.5;
            }
            if (!improved)
            {
                // small random jiggle to escape flat spots
                x[0] = clampd(x[0] + (rand01(rng) - 0.5) * 0.05, 0.0, 1.0);
                x[1] = clampd(x[1] + (rand01(rng) - 0.5) * 0.05, 0.0, 1.0);
                val = f(x);
            }
        }
        if (val > best_val)
        {
            best_val = val;
            best_x = x;
        }
    }
    return best_x;
}

// ===================== Optimizer (two-branch BO) =====================
class BayesianOptimizer
{
public:
    BayesianOptimizer(const Bounds &bounds, const BOOptions &opt)
        : B(bounds), O(opt), rng(opt.seed)
    {
        reset();
    }

    Candidate ask(std::optional<int> remaining_trials_hint = std::nullopt)
    {
        if (!initialized)
            initialize_design();

        rem_hint = remaining_trials_hint; // store
        // If we're still seeding initial points per branch, return them
        if (branch0_seed_left > 0)
        {
            auto x01 = next_seed_point(0);
            last = from01(false, x01);
            pending = true;
            return last;
        }
        if (branch1_seed_left > 0)
        {
            auto x01 = next_seed_point(1);
            last = from01(true, x01);
            pending = true;
            return last;
        }

        // Otherwise use EI per branch and pick the better one
        double xi = compute_xi();
        // fit (maybe refit) both GPs (cheap if unchanged)
        if (need_refit0)
        {
            gp0.fit(/*reopt*/ true, &rng);
            need_refit0 = false;
        }
        if (need_refit1)
        {
            gp1.fit(/*reopt*/ true, &rng);
            need_refit1 = false;
        }

        double global_best = best_score_;

        EIComputer ei0{&gp0, global_best, xi};
        EIComputer ei1{&gp1, global_best, xi};

        // If branch has no data yet, EI uses prior → we still search
        array<double, 2> x0 = maximize_ei(ei0, O.ei_multistart, O.ei_grad_iters, O.ei_step, rng);
        array<double, 2> x1 = maximize_ei(ei1, O.ei_multistart, O.ei_grad_iters, O.ei_step, rng);
        double v0 = ei0(x0);
        double v1 = ei1(x1);

        if (v0 >= v1)
        {
            last = from01(false, x0);
            pending = true;
            return last;
        }
        else
        {
            last = from01(true, x1);
            pending = true;
            return last;
        }
    }

    void tell(const Candidate &c, double score)
    {
        // Check matches last
        // (not strictly required—allows out-of-order usage—but we track pending)
        if (pending)
            pending = false;

        // Update best
        if (observations == 0 || score > best_score_)
        {
            best_score_ = score;
            best_ = c;
        }
        // Scale x to [0,1]^2 and add to appropriate GP
        auto x01 = to01({c.x1, c.x2});
        if (c.b)
        {
            gp1.add_point01(x01, score);
            ++n1_tells;
            // reopt every reopt_hyper_every
            need_refit1 = true;
            if (n1_tells % O.reopt_hyper_every == 0)
            {
                gp1.fit(true, &rng);
                need_refit1 = false;
            }
            else
            {
                gp1.fit(false, &rng); // just rebuild cache if needed
                need_refit1 = false;
            }
        }
        else
        {
            gp0.add_point01(x01, score);
            ++n0_tells;
            need_refit0 = true;
            if (n0_tells % O.reopt_hyper_every == 0)
            {
                gp0.fit(true, &rng);
                need_refit0 = false;
            }
            else
            {
                gp0.fit(false, &rng);
                need_refit0 = false;
            }
        }
        observations++;
    }

    Candidate best_candidate() const { return best_; }
    double best_score() const { return best_score_; }

    void set_remaining_trials_hint(int remaining) { rem_hint = remaining; }
    void set_exploration_xi(double xi)
    {
        user_fixed_xi = xi;
        O.adaptive_xi = false;
    }
    void enable_adaptive_xi(bool enable)
    {
        O.adaptive_xi = enable;
        user_fixed_xi.reset();
    }

    // Optional: mark a failed evaluation so that ask() can propose again later
    void tell_failed(const Candidate &c)
    {
        pending = false; // nothing recorded
        // no-op otherwise
    }

    // Batch helpers (simple repetition)
    vector<Candidate> ask_batch(int k, std::optional<int> remaining_trials_hint = std::nullopt)
    {
        vector<Candidate> out;
        out.reserve(k);
        for (int i = 0; i < k; i++)
            out.push_back(ask(remaining_trials_hint));
        return out;
    }
    void tell_batch(const vector<Candidate> &cs, const vector<double> &ys)
    {
        for (size_t i = 0; i < cs.size(); ++i)
            tell(cs[i], ys[i]);
    }

private:
    Bounds B;
    BOOptions O;
    std::mt19937 rng;

    GP gp0, gp1; // b=false(0), b=true(1)

    bool initialized = false;
    int branch0_seed_left = 0, branch1_seed_left = 0;
    vector<array<double, 2>> seed0, seed1; // in [0,1]^2

    Candidate last{};
    bool pending = false;

    Candidate best{};
    Candidate best_{false, 0, 0};
    double best_score_ = -1e300;
    int observations = 0;

    bool need_refit0 = false, need_refit1 = false;
    int n0_tells = 0, n1_tells = 0;

    std::optional<int> rem_hint;
    std::optional<double> user_fixed_xi;

    void reset()
    {
        initialized = false;
        pending = false;
        observations = 0;
        best_score_ = -1e300;
        n0_tells = n1_tells = 0;
        gp0 = GP();
        gp1 = GP();
        gp0.jitter = O.jitter;
        gp1.jitter = O.jitter;
    }

    void initialize_design()
    {
        // LHS per branch
        int n = max(1, O.init_per_branch);
        seed0 = lhs_2d(n, rng);
        seed1 = lhs_2d(n, rng);
        branch0_seed_left = n;
        branch1_seed_left = n;
        initialized = true;
    }

    array<double, 2> to01(const array<double, 2> &x) const
    {
        double u = (x[0] - B.x1_lo) / (B.x1_hi - B.x1_lo);
        double v = (x[1] - B.x2_lo) / (B.x2_hi - B.x2_lo);
        return {clampd(u, 0, 1), clampd(v, 0, 1)};
    }
    Candidate from01(bool b, const array<double, 2> &x01) const
    {
        double x1 = B.x1_lo + x01[0] * (B.x1_hi - B.x1_lo);
        double x2 = B.x2_lo + x01[1] * (B.x2_hi - B.x2_lo);
        return Candidate{b, x1, x2};
    }

    array<double, 2> next_seed_point(int branch)
    {
        if (branch == 0 && branch0_seed_left > 0)
        {
            auto x01 = seed0[seed0.size() - branch0_seed_left];
            --branch0_seed_left;
            return x01;
        }
        else if (branch == 1 && branch1_seed_left > 0)
        {
            auto x01 = seed1[seed1.size() - branch1_seed_left];
            --branch1_seed_left;
            return x01;
        }
        // fallback: random
        auto xs = lhs_2d(1, rng)[0];
        return xs;
    }

    double compute_xi()
    {
        if (user_fixed_xi)
            return *user_fixed_xi;
        if (!O.adaptive_xi)
            return O.xi_base;
        // simple schedule: more remaining -> more exploration
        // normalize by initial total seeds (2 * init_per_branch)
        double base = O.xi_base;
        double factor = 1.0;
        if (rem_hint && *rem_hint >= 0)
        {
            int init_total = 2 * max(1, O.init_per_branch);
            factor = (*rem_hint) / (double)max(1, init_total);
            factor = clampd(factor, 0.3, 3.0);
        }
        return base * factor;
    }
};



// トラップ付きガード
// vector<vector<CheckGrid>> guards_with_trap = {
//     {}}
// 上方向と右方向に開いているパターン
vector<CheckGrid> guard_pattern_with_trap1 = {
    {0, 0, MustFlower},
    {0, -1, MustTree},
    {1, 0, MustTree},
    {-1, 1, MustTree},
    {-1, -2, MustTree},
    {2, 1, MustTree},
    {-2, 0, MustTree},
    {0, 2, MustTree},
    {-1, 0, MustEmpty},
    {-1, -1, MustEmpty},
    {0, 1, MustEmpty},
    {1, 1, MustEmpty},
};

// 上方向と下方向に開いているパターン
// 上は左、下は右
vector<CheckGrid> guard_pattern_with_trap2 = {
    {0, 0, MustFlower},
    {0, -1, MustTree},
    {0, 1, MustTree},
    {2, 0, MustTree},
    {-2, 0, MustTree},
    {1, 0, MustEmpty},
    {-1, 0, MustEmpty},
    // ここから個別
    // 上側: 左
    {-1, -1, MustEmpty},
    {-1, -2, MustTree},
    {-1, 1, MustTree},
    // 下側: 右
    {1, -1, MustTree},
    {1, 1, MustEmpty},
    {1, 2, MustTree},
};

vector<CheckGrid> guard_pattern_with_trap3 = {
    {0, 0, MustFlower},
    {0, -1, MustTree},
    {0, 1, MustTree},
    {2, 0, MustTree},
    {-2, 0, MustTree},
    {1, 0, MustEmpty},
    {-1, 0, MustEmpty},
    // ここから個別
    // 上側: 左
    {-1, -1, MustEmpty},
    {-1, -2, MustTree},
    {-1, 1, MustTree},
    // 下側: 左
    {1, -1, MustEmpty},
    {1, -2, MustTree},
    {1, 1, MustTree},
};

vector<CheckGrid> guard_pattern_with_trap4 = {
    {0, 0, MustFlower},
    {0, -1, MustTree},
    {0, 1, MustTree},
    {2, 0, MustTree},
    {-2, 0, MustTree},
    {1, 0, MustEmpty},
    {-1, 0, MustEmpty},
    // ここから個別
    // 上側: 右
    {-1, 1, MustEmpty},
    {-1, 2, MustTree},
    {-1, -1, MustTree},
    // 下側: 右
    {1, -1, MustTree},
    {1, 1, MustEmpty},
    {1, 2, MustTree},
};

vector<CheckGrid> guard_pattern_with_trap5 = {
    {0, 0, MustFlower},
    {0, -1, MustTree},
    {0, 1, MustTree},
    {2, 0, MustTree},
    {-2, 0, MustTree},
    {1, 0, MustEmpty},
    {-1, 0, MustEmpty},
    // ここから個別
    // 上側: 右
    {-1, 1, MustEmpty},
    {-1, 2, MustTree},
    {-1, -1, MustTree},
    // 下側: 左
    {1, 1, MustTree},
    {1, -1, MustEmpty},
    {1, -2, MustTree},
};

vector<CheckGrid> guard_pattern_without_trap1 = {
    {0, 0, MustFlower},
    {1, 0, MustTree},
    {0, -1, MustTree},
    {0, 1, MustTree},
    {-1, 0, MustEmpty},
    {-2, 0, MustTree},
    {-1, -1, MustTree},
};

vector<CheckGrid> guard_pattern_without_trap2 = {
    {0, 0, MustFlower},
    {1, 0, MustTree},
    {0, -1, MustTree},
    {0, 1, MustTree},
    {-1, 0, MustEmpty},
    {-2, 0, MustTree},
    {-1, 1, MustTree},
};

auto guard_patterns_with_trap = vector<vector<CheckGrid>>{
    guard_pattern_with_trap1,
    guard_pattern_with_trap2,
    guard_pattern_with_trap3,
    guard_pattern_with_trap4,
    guard_pattern_with_trap5,
    rotate_displacement(guard_pattern_with_trap1, 90),
    rotate_displacement(guard_pattern_with_trap2, 90),
    rotate_displacement(guard_pattern_with_trap3, 90),
    rotate_displacement(guard_pattern_with_trap4, 90),
    rotate_displacement(guard_pattern_with_trap5, 90),
    rotate_displacement(guard_pattern_with_trap1, 180),
    rotate_displacement(guard_pattern_with_trap2, 180),
    rotate_displacement(guard_pattern_with_trap3, 180),
    rotate_displacement(guard_pattern_with_trap4, 180),
    rotate_displacement(guard_pattern_with_trap5, 180),
    rotate_displacement(guard_pattern_with_trap1, 270),
    rotate_displacement(guard_pattern_with_trap2, 270),
    rotate_displacement(guard_pattern_with_trap3, 270),
    rotate_displacement(guard_pattern_with_trap4, 270),
    rotate_displacement(guard_pattern_with_trap5, 270),
};

auto guard_patterns_without_trap = vector<vector<CheckGrid>>{
    guard_pattern_without_trap1,
    guard_pattern_without_trap2,
    rotate_displacement(guard_pattern_without_trap1, 90),
    rotate_displacement(guard_pattern_without_trap2, 90),
    rotate_displacement(guard_pattern_without_trap1, 180),
    rotate_displacement(guard_pattern_without_trap2, 180),
    rotate_displacement(guard_pattern_without_trap1, 270),
    rotate_displacement(guard_pattern_without_trap2, 270),
};

struct ExitFromFlower
{
    vector<P> empty_positions;
    vector<P> tree_positions;
    set<P> empty_position_set;
    set<P> tree_position_set;
    map<P, P> trap_map; // empty -> tree
    ExitFromFlower(vector<P> empty_positions, vector<P> tree_positions, map<P, P> trap_map) : empty_positions(empty_positions), tree_positions(tree_positions), trap_map(trap_map)
    {
        empty_position_set = set<P>(empty_positions.begin(), empty_positions.end());
        tree_position_set = set<P>(tree_positions.begin(), tree_positions.end());
    }
    bool collide_with_other(const ExitFromFlower &other) const
    {
        // empty_positionsの最初が同じだったらダメ
        if (empty_positions[0] == other.empty_positions[0])
            return true;

        // こちらのemptyが相手のtreeと被っていたらダメ
        for (P p : empty_positions)
        {
            if (other.tree_position_set.count(p))
                return true;
        }
        // こちらのtreeが相手のemptyと被っていたらダメ
        for (P p : tree_positions)
        {
            if (other.empty_position_set.count(p))
                return true;
        }
        return false;
    }
};

struct FlowerGuard
{
private:
    static vector<vector<ExitFromFlower>> find_exit_by_bfs(vector<vector<CellType>> &field);

public:
    vector<P> tree_positions;
    map<P, P> trap_positions; // トラップ(key)を踏むと、その横に木が新しく生える
    bool trap_used;
    int score;

    P check_trap(int x, int y)
    {
        if (trap_used)
            return {-1, -1};
        if (trap_positions.count({x, y}))
        {
            trap_used = true;
            return trap_positions[{x, y}];
        }
        return {-1, -1};
    }

    FlowerGuard() : trap_used(false), tree_positions({}), trap_positions({}), score(0)
    {
    }
    FlowerGuard(vector<P> tree_positions, map<P, P> trap_positions, bool trap_used, int score)
        : tree_positions(tree_positions), trap_positions(trap_positions), trap_used(trap_used), score(score)
    {
    }

    static FlowerGuard build_from_field(vector<vector<CellType>> &field);
    static FlowerGuard build_from_field_by_bfs(vector<vector<CellType>> &field);
};

vector<vector<ExitFromFlower>> FlowerGuard::find_exit_by_bfs(vector<vector<CellType>> &field)
{
    vector<vector<ExitFromFlower>> exits(4); // 0:上, 1:右, 2:下, 3:左

    // 各方向に対してBFSを実行
    vector<int> dx = {-1, 0, 1, 0};
    vector<int> dy = {0, 1, 0, -1};
    set<P> prohibited;
    for (int dir = 0; dir < 4; dir++)
    {
        vector<vector<bool>> visited(N, vector<bool>(N, false));
        int x = ti + dx[dir];
        int y = tj + dy[dir];
        if (x < 0 || x >= N || y < 0 || y >= N)
            continue;
        prohibited.insert({x, y});
    }

    for (int initial_dir = 0; initial_dir < 4; initial_dir++)
    {
        vector<vector<bool>> visited(N, vector<bool>(N, false));
        queue<P> q;
        int x = ti + dx[initial_dir];
        int y = tj + dy[initial_dir];
        if (x < 0 || x >= N || y < 0 || y >= N)
            continue;
        if (field[x][y] == TREE)
            continue; // 木がある場合はその方向は無理
        // 出口候補を見つけた
        // ここから曲がった方向に1~3回移動してからさらに曲がる
        for (int dir = 0; dir < 4; dir++)
        {
            vector<P> empty_positions;
            vector<P> tree_positions;
            empty_positions.push_back({x, y});

            if (dir % 2 == initial_dir % 2)
                continue; // 直進方向と同じ方向には曲がれない

            // 反対方向には木がある必要がある
            int back_dir = (dir + 2) % 4;
            int bx = x + dx[back_dir];
            int by = y + dy[back_dir];

            if (!(bx < 0 || bx >= N || by < 0 || by >= N) && field[bx][by] != TREE)
            {
                tree_positions.push_back({bx, by});
            }

            for (int step = 1; step <= 3; step++)
            {
                int prev_x = x + dx[dir] * (step - 1);
                int prev_y = y + dy[dir] * (step - 1);
                // この両側に木がないといけない

                // 最初に移動したのと同じ方向
                int tree1_x = prev_x + dx[initial_dir];
                int tree1_y = prev_y + dy[initial_dir];
                if (step == 1 && !(tree1_x < 0 || tree1_x >= N || tree1_y < 0 || tree1_y >= N) && field[tree1_x][tree1_y] != TREE)
                {
                    tree_positions.push_back({tree1_x, tree1_y});
                }
                if (step > 1)
                {
                    int tree2_x = prev_x - dx[initial_dir];
                    int tree2_y = prev_y - dy[initial_dir];
                    if (!(tree2_x < 0 || tree2_x >= N || tree2_y < 0 || tree2_y >= N) && field[tree2_x][tree2_y] != TREE && !(tree2_x == ti && tree2_y == tj))
                    {
                        tree_positions.push_back({tree2_x, tree2_y});
                    }
                }

                int nx = x + dx[dir] * step;
                int ny = y + dy[dir] * step;
                if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                    break; // 盤面外
                if (field[nx][ny] == TREE)
                    break; // 木がある場合はそこで終了

                empty_positions.push_back({nx, ny});
                // ここからもう一回曲がったところが出口になる
                bool found = false;
                for (int dir2 = 0; dir2 < 4; dir2++)
                {
                    if (dir2 % 2 == dir % 2)
                        continue; // 直進方向と同じ方向には曲がれない

                    int ex = nx + dx[dir2];
                    int ey = ny + dy[dir2];
                    if (ex < 0 || ex >= N || ey < 0 || ey >= N)
                        continue; // 盤面外
                    if (field[ex][ey] == TREE)
                        continue; // 木がある場合は無理
                    if (prohibited.count({ex, ey}))
                        continue; // 花の隣には出られない

                    empty_positions.push_back({ex, ey});

                    int nx2 = nx + dx[dir];
                    int ny2 = ny + dy[dir];
                    int original_tree_count = tree_positions.size();
                    if (!(nx2 < 0 || nx2 >= N || ny2 < 0 || ny2 >= N) && field[nx2][ny2] != TREE)
                    {
                        tree_positions.push_back({nx2, ny2});
                    }
                    int nx3 = nx - dx[dir2];
                    int ny3 = ny - dy[dir2];
                    if (!(nx3 < 0 || nx3 >= N || ny3 < 0 || ny3 >= N) && field[nx3][ny3] != TREE)
                    {
                        tree_positions.push_back({nx3, ny3});
                    }
                    found = true;
                    map<P, P> trap_map;
                    for (int j = 1; j <= step; j++)
                    {
                        int tmp_nx = x + dx[dir] * j;
                        int tmp_ny = y + dy[dir] * j;
                        trap_map[{tmp_nx, tmp_ny}] = {x, y};
                    }

                    exits[initial_dir].emplace_back(ExitFromFlower(empty_positions, tree_positions, trap_map));
                    empty_positions.pop_back();
                    while (tree_positions.size() > original_tree_count)
                    {
                        tree_positions.pop_back();
                    }
                }
            }
        }
    }

    return exits;
}

FlowerGuard FlowerGuard::build_from_field_by_bfs(vector<vector<CellType>> &field)
{
    queue<P> q;
    q.push({0, N / 2});
    vector<vector<int>> dist_mat(N, vector<int>(N, -1));
    dist_mat[0][N / 2] = 0;
    vector<int> dx = {-1, 0, 1, 0};
    vector<int> dy = {0, 1, 0, -1};
    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if (nx >= 0 && nx < N && ny >= 0 && ny < N &&
                dist_mat[nx][ny] == -1 && field[nx][ny] != TREE && field[nx][ny] != FLOWER)
            {
                dist_mat[nx][ny] = dist_mat[x][y] + 1;
                q.push({nx, ny});
            }
        }
    }

    auto local_field = field;
    auto exits = find_exit_by_bfs(local_field);

    // 二つのexitを使った時に大丈夫なやつを列挙する
    vector<tuple<int, int, int, int>> valid_exit_pairs; // (i, ei, j, ej)
    vector<FlowerGuard> candidates;
    int best_score = -1;
    ExitFromFlower best_exit1({}, {}, {});
    ExitFromFlower best_exit2({}, {}, {});
    for (int i = 0; i < exits.size(); i++)
    {
        for (int j = i + 1; j < exits.size(); j++)
        {
            for (int ei = 0; ei < exits[i].size(); ei++)
            {
                for (int ej = 0; ej < exits[j].size(); ej++)
                {
                    if (!exits[i][ei].collide_with_other(exits[j][ej]))
                    {
                        vector<P> new_trees = exits[i][ei].tree_positions;
                        new_trees.insert(new_trees.end(), exits[j][ej].tree_positions.begin(), exits[j][ej].tree_positions.end());
                        for (auto p : new_trees)
                        {
                            local_field[p.first][p.second] = TREE;
                        }
                        bool connected = check_bfs_connectivity(local_field, new_trees, 5, false);
                        if (connected)
                        {
                            valid_exit_pairs.push_back({i, ei, j, ej});
                            set<P> new_tree_set(new_trees.begin(), new_trees.end());
                            set<P> empty_pos_set;
                            for (auto p : exits[i][ei].empty_positions)
                            {
                                empty_pos_set.insert(p);
                            }
                            for (auto p : exits[j][ej].empty_positions)
                            {
                                empty_pos_set.insert(p);
                            }
                            rep(jj, 4)
                            {
                                int nx = ti + dx[jj];
                                int ny = tj + dy[jj];
                                if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                                    continue;
                                if (new_tree_set.count({nx, ny}) == 0 && empty_pos_set.count({nx, ny}) == 0 && field[nx][ny] == EMPTY)
                                {
                                    new_tree_set.insert({nx, ny});
                                }
                            }
                            map<P, P> trap_map;
                            int score = 0;
                            int cnt = 0;
                            for (auto &[k, v] : exits[i][ei].trap_map)
                            {
                                trap_map[k] = v;
                                if (cnt == 0)
                                    score += dist_mat[k.first][k.second] * 2;
                                cnt++;
                            }
                            cnt = 0;
                            for (auto &[k, v] : exits[j][ej].trap_map)
                            {
                                trap_map[k] = v;
                                if (cnt == 0)
                                    score += dist_mat[k.first][k.second] * 2;
                                cnt++;
                            }
                            score -= exits[i][ei].tree_positions.size();
                            score -= exits[j][ej].tree_positions.size();

                            candidates.push_back(FlowerGuard(vector<P>(new_tree_set.begin(), new_tree_set.end()), trap_map, false, score));
                            // return FlowerGuard(vector<P>(new_tree_set.begin(), new_tree_set.end()), trap_map, false, 0);
                            if (score > best_score)
                            {
                                best_score = score;
                                best_exit1 = exits[i][ei];
                                best_exit2 = exits[j][ej];
                            }
                        }
                        for (auto p : new_trees)
                        {
                            local_field[p.first][p.second] = EMPTY;
                        }
                    }
                }
            }
        }
    }
    if (candidates.size() > 0)
    {
        // 最もスコアが高いものを返す高いものを返す
        sort(candidates.begin(), candidates.end(), [](const FlowerGuard &a, const FlowerGuard &b)
             { return a.score > b.score; });

        return candidates[0];
    }

    for (int i = 0; i < exits.size(); i++)
    {
        for (auto e : exits[i])
        {
            vector<P> new_trees = e.tree_positions;
            for (auto p : new_trees)
            {
                local_field[p.first][p.second] = TREE;
            }
            bool connected = check_bfs_connectivity(local_field, new_trees, 5, false);
            if (connected)
            {
                set<P> new_tree_set(new_trees.begin(), new_trees.end());
                set<P> empty_pos_set;
                for (auto p : e.empty_positions)
                {
                    empty_pos_set.insert(p);
                }
                rep(jj, 4)
                {
                    int nx = ti + dx[jj];
                    int ny = tj + dy[jj];
                    if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                        continue;
                    if (new_tree_set.count({nx, ny}) == 0 && empty_pos_set.count({nx, ny}) == 0 && field[nx][ny] == EMPTY)
                    {
                        new_tree_set.insert({nx, ny});
                    }
                }
                map<P, P> trap_map;
                int score = 0;
                int cnt = 0;
                for (auto &[k, v] : e.trap_map)
                {
                    if (cnt == 0)
                        score += dist_mat[k.first][k.second] * 2;
                    cnt++;
                }
                score -= e.tree_positions.size();

                candidates.push_back(FlowerGuard(vector<P>(new_tree_set.begin(), new_tree_set.end()), trap_map, false, score));
            }
            for (auto p : new_trees)
            {
                local_field[p.first][p.second] = EMPTY;
            }
        }
    }
    if (candidates.size() > 0)
    {
        // 最もスコアが高いものを返す高いものを返す
        sort(candidates.begin(), candidates.end(), [](const FlowerGuard &a, const FlowerGuard &b)
             { return a.score > b.score; });
        return candidates[0];
    }

    /*
    for (auto exit : exits)
    {
        cerr << "exit:" << exit.size() << endl;
        for (auto e : exit)
        {
            cerr << "(" << e.empty_positions.size() << "," << e.tree_positions.size() << "),";
            for (auto p : e.empty_positions)
            {
                cerr << "E(" << p.first << "," << p.second << "),";
            }
            cerr << endl;
            for (auto p : e.tree_positions)
            {
                cerr << "T(" << p.first << "," << p.second << "),";
            }
            cerr << endl;
        }
        cerr << endl;
    }
    */

    FlowerGuard guard = FlowerGuard();

    return guard;
    // 開口部からBFSで探索する
}

FlowerGuard FlowerGuard::build_from_field(vector<vector<CellType>> &field)
{
    // BFSで入り口からの距離を測る

    queue<P> q;
    q.push({0, N / 2});
    vector<vector<int>> dist_mat(N, vector<int>(N, -1));
    dist_mat[0][N / 2] = 0;
    vector<int> dx = {-1, 1, 0, 0};
    vector<int> dy = {0, 0, -1, 1};
    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if (nx >= 0 && nx < N && ny >= 0 && ny < N &&
                dist_mat[nx][ny] == -1 && field[nx][ny] != TREE && field[nx][ny] != FLOWER)
            {
                dist_mat[nx][ny] = dist_mat[x][y] + 1;
                q.push({nx, ny});
            }
        }
    }

    auto local_field = field;
    shuffle(guard_patterns_with_trap.begin(), guard_patterns_with_trap.end(), mt);

    // まずはトラップ付きのガードを試す
    vector<FlowerGuard> candidates;
    if (ro.get_rand(0, 1) == 0)
        for (auto &pattern : guard_patterns_with_trap)
        {
            auto [can_place, positions] = can_make_objects(ti, tj, local_field, pattern);
            if (!can_place)
                continue;

            vector<P> new_trees;
            for (auto position : positions)
            {
                local_field[position.x][position.y] = TREE;
                new_trees.push_back({position.x, position.y});
            }

            bool connected = check_bfs_connectivity(local_field, new_trees, 6 * N, false);

            if (!connected)
            {
                for (auto position : positions)
                {
                    local_field[position.x][position.y] = EMPTY;
                }
                continue;
            }

            // トラップがうまくできたので、FlowerGuardを作成して返す
            FlowerGuard guard = FlowerGuard();
            guard.tree_positions = new_trees;
            map<P, P> trap_map;
            auto dx = vector<int>{-1, 1, 0, 0};
            auto dy = vector<int>{0, 0, -1, 1};
            rep(k, 4)
            {
                int nx = ti + dx[k];
                int ny = tj + dy[k];
                if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                    continue;

                if (local_field[nx][ny] == TREE)
                {
                    continue;
                }

                rep(kk, 4)
                {
                    int nnx = nx + dx[kk];
                    int nny = ny + dy[kk];
                    if (nnx < 0 || nnx >= N || nny < 0 || nny >= N)
                        continue;

                    if (local_field[nnx][nny] == EMPTY)
                    {
                        trap_map[{nnx, nny}] = {nx, ny};
                        break;
                    }
                }
            }
            // 開口部の座標を調べる
            int local_score = 0;
            for (int dir = 0; dir < 4; dir++)
            {
                int nx = ti + dx[dir];
                int ny = tj + dy[dir];
                if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                    continue;

                if (local_field[nx][ny] == TREE)
                {
                    continue;
                }
                if (dist_mat[nx][ny] < 0)
                    continue;
                local_score += dist_mat[nx][ny];
            }

            guard.trap_positions = trap_map;
            guard.score = local_score;
            candidates.push_back(guard);
            for (auto position : positions)
            {
                local_field[position.x][position.y] = EMPTY;
            }
        }

    if (candidates.size() > 0)
    {
        // スコアが大きい順にsortする
        sort(candidates.begin(), candidates.end(), [](const FlowerGuard &a, const FlowerGuard &b)
             { return a.score > b.score; });
        return candidates[0];
    }

    shuffle(guard_patterns_without_trap.begin(), guard_patterns_without_trap.end(), mt);
    // 次にトラップなしのガードを試す
    for (auto &pattern : guard_patterns_without_trap)
    {
        auto [can_place, positions] = can_make_objects(ti, tj, local_field, pattern);
        if (!can_place)
            continue;

        vector<P> new_trees;
        for (auto position : positions)
        {
            local_field[position.x][position.y] = TREE;
            new_trees.push_back({position.x, position.y});
        }

        bool connected = check_bfs_connectivity(local_field, new_trees, N, false);

        if (!connected)
        {
            for (auto position : positions)
            {
                local_field[position.x][position.y] = EMPTY;
            }
            continue;
        }

        // トラップがうまくできたので、FlowerGuardを作成して返す
        FlowerGuard guard = FlowerGuard();
        guard.tree_positions = new_trees;

        int local_score = 0;
        for (int dir = 0; dir < 4; dir++)
        {
            // 開口部を調べる
            int nx = ti + dx[dir];
            int ny = tj + dy[dir];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                continue;
            if (local_field[nx][ny] == TREE)
            {
                continue;
            }
            if (dist_mat[nx][ny] < 0)
                continue;
            local_score += dist_mat[nx][ny];
        }
        guard.score = local_score;
        candidates.push_back(guard);

        // return guard;
    }
    if (candidates.size() > 0)
    {
        // スコアが大きい順にsortする
        sort(candidates.begin(), candidates.end(), [](const FlowerGuard &a, const FlowerGuard &b)
             { return a.score > b.score; });
        return candidates[0];
    }
    return FlowerGuard();
}




class InitialRouter
{
private:
    set<P> create_path_coordinates_set(const vector<P> &left_path, const vector<P> &right_path);
    pair<bool, vector<P>> left_hand_wall_following(int start_x, int start_y, vector<vector<CellType>> &field, int max_steps, vector<P> &initial_moves, int initial_direction = 1);
    pair<bool, vector<P>> right_hand_wall_following(int start_x, int start_y, vector<vector<CellType>> &field, int max_steps, vector<P> &initial_moves);
    optional<pair<vector<P>, vector<P>>> construct_wall_following_paths_with_params(int best_upper_x, int pos_y, vector<vector<CellType>> &field, float left_wall_follow_length, float right_wall_follow_length);
    vector<P> generate_movement_path(int start_x, int start_y, int goal_x, int goal_y, vector<vector<CellType>> &field);
    bool is_min_distance(int sx, int sy, int gx, int gy, vector<vector<CellType>> &f);

    void place_guide_trents_for_path(const vector<P> &path, const string &path_name, set<P> &path_coordinates,
                                     vector<vector<CellType>> &field, vector<int> &trent_positions);
    int start_x, start_y;

    pair<bool, vector<P>> right_hand_wall_following_under_posx(int start_x, int start_y, vector<vector<CellType>> &field, int pos_x);

    pair<bool, vector<P>> left_hand_wall_following2(int start_x, int start_y, vector<vector<CellType>> &field, int max_steps, int pos_x);

public:
    InitialRouter()
    {
        start_x = 0;
        start_y = N / 2;
    };
    tuple<bool, vector<int>, int, vector<P>, vector<P>> try_construct_vertical_with_params(int pos, float left_wall_follow_length, float right_wall_follow_length, vector<vector<CellType>> &field);
    tuple<bool, vector<int>, int, vector<P>, vector<P>> try_construct_horizontal_with_params(int pos, float left_wall_follow_length, float right_wall_follow_length, vector<vector<CellType>> &field);

    pair<vector<P>, vector<P>> try_construct_vertical(vector<vector<CellType>> &field, vector<int> &current_trent_positions, Candidate &cand);
    pair<vector<P>, vector<P>> try_construct_horizontal(vector<vector<CellType>> &field, vector<int> &current_trent_positions, Candidate &cand);
};

pair<vector<P>, vector<P>> InitialRouter::try_construct_vertical(vector<vector<CellType>> &field, vector<int> &current_trent_positions, Candidate &cand)
{
    /*
    double upper_length = 3.5;
    if (N >= 25)
        upper_length = 4.5;
    if (N >= 30)
        upper_length = 7.0;
    if (N >= 35)
        upper_length = 7.0;
    */
    double upper_length = vertical_upper_coeff1 + (double)(N - 20) * vertical_upper_coeff2;

    double lower_limit = vertical_lower_coeff;
    // パラメータをランダムに選択（左右別々）
    float left_wall_follow_length = lower_limit + ro.get_float() * (upper_length - lower_limit);  // 1.5N ~ 3.4N
    float right_wall_follow_length = lower_limit + ro.get_float() * (upper_length - lower_limit); // 1.5N ~ 3.4N

    vector<int> vertical_trials = {N / 2, N / 2 - 1, N / 2 + 1, N / 2 - 2, N / 2 + 2, N / 2 - 3, N / 2 + 3};
    if (N > 30)
    {
        vertical_trials.push_back(N / 2 - 4);
        vertical_trials.push_back(N / 2 + 4);
    }

    shuffle(vertical_trials.begin(), vertical_trials.end(), mt);
    vector<P> left_path, right_path;
    for (auto pos_y : vertical_trials)
    {
        auto [success, trent_positions, len, tmp_left_path, tmp_right_path] = try_construct_vertical_with_params(pos_y, left_wall_follow_length, right_wall_follow_length, field);
        if (success)
        {
            current_trent_positions = trent_positions;
            left_path = tmp_left_path;
            right_path = tmp_right_path;
            break;
        }
        else
        {
            continue;
        }
    }
    return {left_path, right_path};
}

pair<vector<P>, vector<P>> InitialRouter::try_construct_horizontal(vector<vector<CellType>> &field, vector<int> &current_trent_positions, Candidate &cand)
{
    vector<int> horizontal_trials = {N / 2, N / 2 - 1, N / 2 + 1, N / 2 - 2, N / 2 + 2};

    double upper_length = horizontal_upper_coeff1 + (double)(N - 20) * horizontal_upper_coeff2;
    double lower_limit = horizontal_lower_coeff;

    // パラメータをランダムに選択（左右別々）
    float left_wall_follow_length = lower_limit + ro.get_float() * (upper_length - lower_limit);  // 1.5N ~ 3.4N
    float right_wall_follow_length = lower_limit + ro.get_float() * (upper_length - lower_limit); // 1.5N ~ 3.4N

    shuffle(horizontal_trials.begin(), horizontal_trials.end(), mt);
    vector<P> left_path, right_path;
    for (auto pos_x : horizontal_trials)
    {

        auto [success, trent_positions, len, tmp_left_path, tmp_right_path] = try_construct_horizontal_with_params(pos_x, left_wall_follow_length, right_wall_follow_length, field);
        if (success)
        {
            current_trent_positions = trent_positions;
            left_path = tmp_left_path;
            right_path = tmp_right_path;
            break;
        }
        else
        {
            continue;
        }
    }
    return {left_path, right_path};
}

tuple<bool, vector<int>, int, vector<P>, vector<P>> InitialRouter::try_construct_vertical_with_params(int pos_y, float left_wall_follow_length, float right_wall_follow_length, vector<vector<CellType>> &field)
{
    // pm 2の範囲に花があったらダメ
    if (abs(pos_y - tj) <= 2)
    {
        return {false, {}, 0, {}, {}};
    }

    vector<int> trent_positions;
    auto local_field = field;

    // 最初に壁を作る場所を決める
    // まずは壁を縦方向に置く
    int best_upper_x = -1;
    vector<vector<pair<double, int>>> DP(N, vector<pair<double, int>>(N, {1e9, -1}));
    for (int upper_x = 1; upper_x < 4; upper_x++)
    {
        vector<int> tmp_positions;
        /*
        rep(i, N) rep(j, N)
            DP[i][j] = {1e9, -1};

        DP[upper_x][pos_y] = {0.0, -1};
        int upper_y = pos_y + 1;
        int lower_y = pos_y - 1;

        for (int xpos = upper_x; xpos < N - 1; xpos++)
        {
            int nx = xpos + 1;
            for (int ypos = lower_y; ypos <= upper_y; ypos++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    int ny = ypos + dy;
                    if (ny < lower_y || ny > upper_y)
                        continue;

                    double cost = 0;
                    if (dy != 0)
                        cost += 0.4;
                    if (local_field[nx][ny] == EMPTY)
                        cost += 1;
                    if (abs(ti - nx) + abs(tj - ny) <= 2) // 花の近くは避ける
                        cost += 50;

                    if (DP[nx][ny].first > DP[xpos][ypos].first + cost)
                    {
                        DP[nx][ny] = {DP[xpos][ypos].first + cost, ypos};
                    }
                }
            }
        }

        // 最もコストが小さいところにブロックを置いていく
        int best_end_y = -1;
        double bsst_cost = 1e9;
        for (int ypos = lower_y; ypos <= upper_y; ypos++)
        {
            if (bsst_cost > DP[N - 2][ypos].first)
            {
                bsst_cost = DP[N - 2][ypos].first;
                best_end_y = ypos;
            }
        }
        if (local_field[N - 1][best_end_y] == EMPTY)
        {
            local_field[N - 1][best_end_y] = TREE;
            tmp_positions.push_back((N - 1) * N + best_end_y);
        }

        int ypos = best_end_y;
        for (int xpos = N - 2; xpos >= upper_x; xpos--)
        {
            int prev_y = ypos;
            int next_y = DP[xpos + 1][prev_y].second;
            ypos = next_y;
            if (local_field[xpos][ypos] == EMPTY)
            {
                local_field[xpos][ypos] = TREE;
                tmp_positions.push_back(xpos * N + ypos);
            }
        }
        */

        // posに対してx = N-1からupper_xまで壁を作る
        for (int x = N - 1; x >= upper_x; x--)
        {
            if (local_field[x][pos_y] == TREE)
                continue;
            if (local_field[x][pos_y] == FLOWER)
            {
                return {false, {}, 0, {}, {}};
            }
            local_field[x][pos_y] = TREE;
            tmp_positions.push_back(x * N + pos_y);
        }
        // スタート位置から(upper_x, pos_y-1)・(upper_x, pos_y+1)での最短距離がthreshold以下なら成功
        if (is_min_distance(start_x, start_y, upper_x, pos_y - 1, local_field) && is_min_distance(start_x, start_y, upper_x, pos_y + 1, local_field))
        {
            best_upper_x = upper_x;
            trent_positions.insert(trent_positions.end(), tmp_positions.begin(), tmp_positions.end());
            field = local_field;
            break;
        }
        else
        {
            for (auto tmp_position : tmp_positions)
            {
                local_field[tmp_position / N][tmp_position % N] = EMPTY;
            }
        }
    }
    if (best_upper_x == -1)
    {
        return {false, {}, 0, {}, {}};
    }

    // 外周パスを構築して追加のトレント配置
    int total_path_length = 0;

    // パス構築フェーズ
    auto paths_result = construct_wall_following_paths_with_params(best_upper_x, pos_y, local_field, left_wall_follow_length, right_wall_follow_length);
    if (!paths_result)
    {
        return {false, {}, 0, {}, {}};
    }
    auto [left_path, right_path] = *paths_result;
    total_path_length += left_path.size() + right_path.size();

    // パス座標セット作成
    set<P> path_coordinates = create_path_coordinates_set(left_path, right_path);

    // ガイド配置フェーズ（左手法パスのみ - コメントアウトされた部分を反映）
    place_guide_trents_for_path(left_path, "left-hand", path_coordinates, local_field, trent_positions);
    place_guide_trents_for_path(right_path, "right-hand", path_coordinates, local_field, trent_positions);

    // フィールドを更新
    field = local_field;

    return {true, trent_positions, total_path_length, left_path, right_path};
}

pair<bool, vector<P>> InitialRouter::left_hand_wall_following(int start_x, int start_y, vector<vector<CellType>> &field, int max_steps, vector<P> &initial_moves, int initial_direction)
{
    vector<vector<bool>> visited(N, vector<bool>(N, false));
    for (auto move : initial_moves)
    {
        int x = move.first;
        int y = move.second;
        visited[x][y] = true;
    }

    auto can_enter = [&field, &visited](int x, int y)
    {
        // 範囲外には入れない
        if (!(x >= 0 && x < N && y >= 0 && y < N))
        {
            return false;
        }
        // 木の場合には入れない
        if (field[x][y] == TREE)
        {
            return false;
        }

        // 元いた道には戻れる
        if (visited[x][y])
        {
            return true;
        }
        vec_int dxs = {-1, 1, 0, 0};
        vec_int dys = {0, 0, -1, 1};
        int visited_count = 0;
        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dxs[dir];
            int ny = y + dys[dir];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                continue;
            if (visited[nx][ny])
                visited_count++;
        }
        if (visited_count >= 2)
            return false;

        return true;
    };

    vector<P> path;

    // 4方向：右(0), 下(1), 左(2), 上(3)
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};

    int x = start_x, y = start_y;
    visited[x][y] = true;

    // 最寄りの壁を検出して適切な初期方向を決定
    int direction = initial_direction;

    // 開始点が壁に囲まれているかチェック
    bool found_direction = false;
    for (int i = 0; i < 4; i++)
    {
        int test_dir = (direction + i) % 4;
        int nx = x + dx[test_dir];
        int ny = y + dy[test_dir];
        if (nx >= 0 && nx < N && ny >= 0 && ny < N && field[nx][ny] != TREE)
        {
            direction = test_dir;
            found_direction = true;
            break;
        }
    }

    if (!found_direction)
    {
        return {false, path}; // 移動できない場合は空のパスを返す
    }

    path.push_back({x, y});

    for (int step = 0; step < max_steps; step++)
    {
        visited[x][y] = true;
        // 左手法：現在の方向から左を向く（反時計回り）
        int left_direction = (direction + 3) % 4;
        int left_x = x + dx[left_direction];
        int left_y = y + dy[left_direction];

        // 左側に壁があるかチェック（境界外も壁として扱う）
        bool left_is_wall = !(can_enter(left_x, left_y));

        if (left_is_wall)
        {
            // 左に壁がある場合、前進を試す
            int forward_x = x + dx[direction];
            int forward_y = y + dy[direction];

            if (can_enter(forward_x, forward_y))
            {
                // 前進可能
                x = forward_x;
                y = forward_y;
                path.push_back({x, y});
            }
            else
            {
                // 前進できない場合、右に回転
                direction = (direction + 1) % 4;
            }
        }
        else
        {
            // 左に壁がない場合、左に回転して前進
            direction = left_direction;
            x = left_x;
            y = left_y;
            path.push_back({x, y});
        }

        // 花に近づきすぎたかチェック（3マス以内）
        if (abs(x - ti) + abs(y - tj) <= 1)
        {
            if (step < N)
                return {false, {}};
            break;
        }

        // 開始点に戻ったかチェック（外周を一周した）
        if (step > 0 && x == start_x && y == start_y)
        {
            break;
        }
    }

    return {true, path};
}

pair<bool, vector<P>> InitialRouter::right_hand_wall_following(int start_x, int start_y, vector<vector<CellType>> &field, int max_steps, vector<P> &initial_moves)
{
    vector<vector<bool>> visited(N, vector<bool>(N, false));
    for (auto move : initial_moves)
    {
        int x = move.first;
        int y = move.second;
        visited[x][y] = true;
    }

    auto can_enter = [&field, &visited](int x, int y)
    {
        // 範囲外には入れない
        if (!(x >= 0 && x < N && y >= 0 && y < N))
        {
            return false;
        }
        // 木の場合には入れない
        if (field[x][y] == TREE)
        {
            return false;
        }

        // 元いた道には戻れる
        if (visited[x][y])
        {
            return true;
        }
        vec_int dxs = {-1, 1, 0, 0};
        vec_int dys = {0, 0, -1, 1};
        int visited_count = 0;
        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dxs[dir];
            int ny = y + dys[dir];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                continue;
            if (visited[nx][ny])
                visited_count++;
        }
        if (visited_count >= 2)
            return false;

        return true;
    };

    vector<P> path;

    // 4方向：右(0), 下(1), 左(2), 上(3)
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};

    int x = start_x, y = start_y;

    // 最寄りの壁を検出して適切な初期方向を決定
    int direction = 1;

    // 開始点が壁に囲まれているかチェック
    bool found_direction = false;
    for (int i = 0; i < 4; i++)
    {
        int test_dir = (direction + i) % 4;
        int nx = x + dx[test_dir];
        int ny = y + dy[test_dir];
        if (nx >= 0 && nx < N && ny >= 0 && ny < N && field[nx][ny] != TREE)
        {
            direction = test_dir;
            found_direction = true;
            break;
        }
    }

    if (!found_direction)
    {
        return {false, {}}; // 移動できない場合は空のパスを返す
    }

    path.push_back({x, y});

    for (int step = 0; step < max_steps; step++)
    {
        visited[x][y] = true;

        // 右手法：現在の方向から右を向く（時計回り）
        int right_direction = (direction + 1) % 4;
        int right_x = x + dx[right_direction];
        int right_y = y + dy[right_direction];

        // 右側に壁があるかチェック（境界外も壁として扱う）
        bool right_is_wall = !(can_enter(right_x, right_y));

        if (right_is_wall)
        {
            // 右に壁がある場合、前進を試す
            int forward_x = x + dx[direction];
            int forward_y = y + dy[direction];

            if (can_enter(forward_x, forward_y))
            {
                // 前進可能
                x = forward_x;
                y = forward_y;
                path.push_back({x, y});
            }
            else
            {
                // 前進できない場合、左に回転
                direction = (direction + 3) % 4;
            }
        }
        else
        {
            // 右に壁がない場合、右に回転して前進
            direction = right_direction;
            x = right_x;
            y = right_y;
            path.push_back({x, y});
        }

        // 花に近づきすぎたかチェック（3マス以内）
        if (abs(x - ti) + abs(y - tj) <= 1)
        {
            if (step < N)
                return {false, {}};
            break;
        }

        // 開始点に戻ったかチェック（外周を一周した）
        if (step > 0 && x == start_x && y == start_y)
        {
            break;
        }
    }

    return {true, path};
}

vector<P> InitialRouter::generate_movement_path(int start_x, int start_y, int goal_x, int goal_y, vector<vector<CellType>> &field)
{
    vector<P> path;

    // BFSで最短経路を求める
    vector<vector<int>> dist(N, vector<int>(N, -1));
    vector<vector<P>> parent(N, vector<P>(N, {-1, -1}));
    queue<P> q;

    dist[start_x][start_y] = 0;
    q.push({start_x, start_y});

    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};

    bool found = false;
    while (!q.empty() && !found)
    {
        auto [x, y] = q.front();
        q.pop();

        if (x == goal_x && y == goal_y)
        {
            found = true;
            break;
        }

        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if (nx >= 0 && nx < N && ny >= 0 && ny < N &&
                field[nx][ny] != TREE && dist[nx][ny] == -1)
            {
                dist[nx][ny] = dist[x][y] + 1;
                parent[nx][ny] = {x, y};
                q.push({nx, ny});
            }
        }
    }

    if (!found)
    {
        return path;
    }

    // 経路を復元
    int curr_x = goal_x, curr_y = goal_y;
    while (curr_x != -1 && curr_y != -1)
    {
        path.push_back({curr_x, curr_y});
        auto [px, py] = parent[curr_x][curr_y];
        curr_x = px;
        curr_y = py;
    }

    // パスを逆順にして開始点から目標点の順にする
    reverse(path.begin(), path.end());

    return path;
}

optional<pair<vector<P>, vector<P>>> InitialRouter::construct_wall_following_paths_with_params(int best_upper_x, int pos_y, vector<vector<CellType>> &field, float left_wall_follow_length, float right_wall_follow_length)
{

    // 第1段階: スタート位置から目標位置への移動パス
    vector<P> movement_to_left_start = generate_movement_path(start_x, start_y, best_upper_x, pos_y - 1, field);
    vector<P> movement_to_right_start = generate_movement_path(start_x, start_y, best_upper_x, pos_y + 1, field);

    // 第2段階: 目標位置から左手法・右手法でパス構築（別々のパラメータ化された長さで）
    int left_max_steps = (int)(left_wall_follow_length * N);
    int right_max_steps = (int)(right_wall_follow_length * N);
    auto [left_found, left_perimeter_path] = left_hand_wall_following(best_upper_x, pos_y - 1, field, left_max_steps, movement_to_left_start);
    auto [right_found, right_perimeter_path] = right_hand_wall_following(best_upper_x, pos_y + 1, field, right_max_steps, movement_to_right_start);

    // 左手法または右手法が失敗した場合はnulloptを返す
    if (!left_found || !right_found)
    {
        return nullopt;
    }

    // パスを結合
    vector<P> left_path = movement_to_left_start;
    left_path.insert(left_path.end(), left_perimeter_path.begin() + 1, left_perimeter_path.end()); // 開始点の重複を避ける

    vector<P> right_path = movement_to_right_start;
    right_path.insert(right_path.end(), right_perimeter_path.begin() + 1, right_perimeter_path.end()); // 開始点の重複を避ける
    return make_pair(left_path, right_path);
}

bool InitialRouter::is_min_distance(int sx, int sy, int gx, int gy, vector<vector<CellType>> &f)
{
    /* BFSをしてsx, syからgx, gyまで最短距離で行けたらOK
    遠回りする必要があるかをチェックする
    bfsを全部やると低速なので、無駄な動きをしないようにする
    つまりmin_distanceは使わない
    distは少数でいいのでmapを使おう
     */
    map<P, int> dist;
    dist[{sx, sy}] = 0;
    queue<P> q;
    q.push({sx, sy});
    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};
    int min_dist = abs(sx - gx) + abs(sy - gy);
    while (!q.empty())
    {
        auto [x, y] = q.front();

        q.pop();
        rep(i, 4)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                continue;
            if (f[nx][ny] == TREE)
                continue;
            if (dist.count({nx, ny}) == 0)
            {
                dist[{nx, ny}] = dist[{x, y}] + 1;
                if (dist[{nx, ny}] > min_dist + 9)
                    continue;
                q.push({nx, ny});
                if (nx == gx && ny == gy)
                {
                    if (dist[{nx, ny}] <= abs(sx - gx) + abs(sy - gy) + 9)
                        return true;
                    else
                        return false;
                }
            }
        }
    }
    return false;
}

set<P> InitialRouter::create_path_coordinates_set(const vector<P> &left_path, const vector<P> &right_path)
{
    set<P> path_coordinates;
    for (const auto &pos : left_path)
    {
        path_coordinates.insert(pos);
    }
    for (const auto &pos : right_path)
    {
        path_coordinates.insert(pos);
    }
    return path_coordinates;
}

void InitialRouter::place_guide_trents_for_path(const vector<P> &path, const string &path_name, set<P> &path_coordinates,
                                                vector<vector<CellType>> &field, vector<int> &trent_positions)
{
    if (path.size() <= 1)
        return;

    // 初期位置と最終地点を除いて、パス上の各マスについて隣接する全方向にガイド配置
    for (int i = 0; i < path.size(); i++) // 最終地点を除外

    {
        int curr_x = path[i].first;
        int curr_y = path[i].second;
        // 4方向（上下左右）をチェック
        int dx[4] = {-1, 1, 0, 0};
        int dy[4] = {0, 0, -1, 1};

        for (int dir = 0; dir < 4; dir++)
        {
            int adj_x = curr_x + dx[dir];
            int adj_y = curr_y + dy[dir];

            // 隣接位置がパス上でない場合にガイド用トレントを配置
            if (adj_x >= 0 && adj_x < N && adj_y >= 0 && adj_y < N &&
                field[adj_x][adj_y] == EMPTY &&
                !(adj_x == ti && adj_y == tj) &&
                path_coordinates.find({adj_x, adj_y}) == path_coordinates.end())
            {
                field[adj_x][adj_y] = TREE;
                trent_positions.push_back(adj_x * N + adj_y);
            }
        }
    }
    // 最後に置かれた場所は除去をする
    int last_x = trent_positions.back() / N;
    int last_y = trent_positions.back() % N;
    if (field[last_x][last_y] == TREE)
    {
        field[last_x][last_y] = EMPTY;
        trent_positions.pop_back();
    }
}

/*
最初の地点から右手方に従ってpos_x+N/4より下まで行くルートを作る
*/
pair<bool, vector<P>> InitialRouter::right_hand_wall_following_under_posx(int start_x, int start_y, vector<vector<CellType>> &field, int pos_x)
{
    vector<vector<bool>> visited(N, vector<bool>(N, false));
    auto can_enter = [&field, &visited](int x, int y)
    {
        // 範囲外には入れない
        if (!(x >= 0 && x < N && y >= 0 && y < N))
        {
            return false;
        }
        // 木の場合には入れない
        if (field[x][y] == TREE)
        {
            return false;
        }

        // 元いた道には戻れる
        if (visited[x][y])
        {
            return true;
        }
        vec_int dxs = {-1, 1, 0, 0};
        vec_int dys = {0, 0, -1, 1};
        int visited_count = 0;
        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dxs[dir];
            int ny = y + dys[dir];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                continue;
            if (visited[nx][ny])
                visited_count++;
        }
        if (visited_count >= 2)
            return false;

        return true;
    };

    vector<P> path;

    // 4方向：右(0), 下(1), 左(2), 上(3)
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};

    int x = start_x, y = start_y;

    // 最寄りの壁を検出して適切な初期方向を決定
    int direction = 1;

    // 開始点が壁に囲まれているかチェック
    bool found_direction = false;
    for (int i = 0; i < 4; i++)
    {
        int test_dir = (direction + i) % 4;
        int nx = x + dx[test_dir];
        int ny = y + dy[test_dir];
        if (nx >= 0 && nx < N && ny >= 0 && ny < N && field[nx][ny] != TREE)
        {
            direction = test_dir;
            found_direction = true;
            break;
        }
    }

    if (!found_direction)
    {
        return {false, {}}; // 移動できない場合は空のパスを返す
    }

    path.push_back({x, y});

    int step = 0;
    while (true)
    {
        step++;
        visited[x][y] = true;

        // 右手法：現在の方向から右を向く（時計回り）
        int right_direction = (direction + 1) % 4;
        int right_x = x + dx[right_direction];
        int right_y = y + dy[right_direction];

        // 右側に壁があるかチェック（境界外も壁として扱う）
        bool right_is_wall = !(can_enter(right_x, right_y));

        if (right_is_wall)
        {
            // 右に壁がある場合、前進を試す
            int forward_x = x + dx[direction];
            int forward_y = y + dy[direction];

            if (can_enter(forward_x, forward_y))
            {
                // 前進可能
                x = forward_x;
                y = forward_y;
                path.push_back({x, y});
            }
            else
            {
                // 前進できない場合、左に回転
                direction = (direction + 3) % 4;
            }
        }
        else
        {
            // 右に壁がない場合、右に回転して前進
            direction = right_direction;
            x = right_x;
            y = right_y;
            path.push_back({x, y});
        }

        // 花に近づきすぎたかチェック（1マス以内）
        if (abs(x - ti) + abs(y - tj) <= 1)
        {
            return {false, {}};
        }

        if (x > pos_x + 3)
        {
            break;
        }
    }

    return {true, path};
}
pair<bool, vector<P>> InitialRouter::left_hand_wall_following2(int start_x, int start_y, vector<vector<CellType>> &field, int max_steps, int pos_x)
{
    vector<vector<bool>> visited(N, vector<bool>(N, false));

    auto can_enter = [&field, &visited, &pos_x](int x, int y)
    {
        // pos_x以下にはいけない
        if (x <= pos_x)
            return false;
        // 範囲外には入れない
        if (!(x >= 0 && x < N && y >= 0 && y < N))
        {
            return false;
        }
        // 木の場合には入れない
        if (field[x][y] == TREE)
        {
            return false;
        }

        // 元いた道には戻れる
        if (visited[x][y])
        {
            return true;
        }
        vec_int dxs = {-1, 1, 0, 0};
        vec_int dys = {0, 0, -1, 1};
        int visited_count = 0;
        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dxs[dir];
            int ny = y + dys[dir];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                continue;
            if (visited[nx][ny])
                visited_count++;
        }
        if (visited_count >= 2)
            return false;

        return true;
    };

    vector<P> path;

    // 4方向：右(0), 下(1), 左(2), 上(3)
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};

    int x = start_x, y = start_y;
    visited[x][y] = true;

    // 最寄りの壁を検出して適切な初期方向を決定
    int direction = 1;

    // 開始点が壁に囲まれているかチェック
    bool found_direction = false;
    for (int i = 0; i < 4; i++)
    {
        int test_dir = (direction + i) % 4;
        int nx = x + dx[test_dir];
        int ny = y + dy[test_dir];
        if (nx >= 0 && nx < N && ny >= 0 && ny < N && field[nx][ny] != TREE)
        {
            direction = test_dir;
            found_direction = true;
            break;
        }
    }

    if (!found_direction)
    {
        return {false, path}; // 移動できない場合は空のパスを返す
    }

    path.push_back({x, y});

    for (int step = 0; step < max_steps; step++)
    {
        visited[x][y] = true;
        // 左手法：現在の方向から左を向く（反時計回り）
        int left_direction = (direction + 3) % 4;
        int left_x = x + dx[left_direction];
        int left_y = y + dy[left_direction];

        // 左側に壁があるかチェック（境界外も壁として扱う）
        bool left_is_wall = !(can_enter(left_x, left_y));

        if (left_is_wall)
        {
            // 左に壁がある場合、前進を試す
            int forward_x = x + dx[direction];
            int forward_y = y + dy[direction];

            if (can_enter(forward_x, forward_y))
            {
                // 前進可能
                x = forward_x;
                y = forward_y;
                path.push_back({x, y});
            }
            else
            {
                // 前進できない場合、右に回転
                direction = (direction + 1) % 4;
            }
        }
        else
        {
            // 左に壁がない場合、左に回転して前進
            direction = left_direction;
            x = left_x;
            y = left_y;
            path.push_back({x, y});
        }

        // 花に近づきすぎたかチェック（3マス以内）
        if (abs(x - ti) + abs(y - tj) <= 1)
        {
            if (step < N)
                return {false, {}};
            break;
        }

        // 開始点に戻ったかチェック（外周を一周した）
        if (step > 0 && x == start_x && y == start_y)
        {
            break;
        }
    }

    return {true, path};
}

tuple<bool, vector<int>, int, vector<P>, vector<P>> InitialRouter::try_construct_horizontal_with_params(int pos_x, float left_wall_follow_length, float right_wall_follow_length, vector<vector<CellType>> &field)
{
    // pm 2の範囲に花があったらダメ
    if (abs(pos_x - ti) <= 2)
    {
        return {false, {}, 0, {}, {}};
    }

    // 1. pos_x地点に壁を作る
    // なお、左側の通路は完全に塞がないようにする
    // スタート位置から右手法に従って移動した時に、pos_xよりも下まで到達するパスを求める
    auto [found, right_path_start] = right_hand_wall_following_under_posx(start_x, start_y, field, pos_x);
    if (!found)
    {
        return {false, {}, 0, {}, {}};
    }

    auto trent_positions = vector<int>{};

    set<P> right_path_start_set = set<P>(right_path_start.begin(), right_path_start.end());
    // 右から上下に分断する壁を作る
    for (int y = N - 1; y >= 0; y--)
    {
        if (right_path_start_set.count({pos_x, y}))
            continue;
        if (field[pos_x][y] == EMPTY)
        {
            field[pos_x][y] = TREE;
            trent_positions.push_back(pos_x * N + y);
        }
    }

    // 手法で回る際のパスを塞いではいけないので、左手法をやっておく
    vector<P> left_init;
    auto [found_left, left_path_start] = left_hand_wall_following(start_x, start_y, field, N, left_init, 3);
    for (auto p : left_path_start)
    {
        right_path_start_set.insert(p);
    }

    // 次にパスを構築する
    // まずは右手法でpox_xより下のところまでのパスを囲うところから

    while (true)
    {
        int xpos = right_path_start.back().first;
        if (xpos <= pos_x + 1)
            break;
        right_path_start.pop_back();
    }
    place_guide_trents_for_path(right_path_start, "right-hand", right_path_start_set, field, trent_positions);

    // 次にスタート地点から左手法で一定の長さのパスの構築
    auto [found_left2, left_path] = left_hand_wall_following(start_x, start_y, field, left_wall_follow_length * N, left_init, 3);
    for (auto p : left_path)
    {
        right_path_start_set.insert(p);
    }

    // right_path_startの最終地点から左手法で回る
    auto [found_left3, left_path2] = left_hand_wall_following2(right_path_start.back().first, right_path_start.back().second, field, right_wall_follow_length * N, pos_x);

    place_guide_trents_for_path(left_path, "left-hand", right_path_start_set, field, trent_positions);
    set<P> right_path_start_set2;
    for (auto p : left_path2)
    {
        right_path_start_set2.insert(p);
    }
    for (auto p : right_path_start)
    {
        if (p.first <= pos_x + 1)
            right_path_start_set2.insert(p);
    }
    place_guide_trents_for_path(left_path2, "left-hand", right_path_start_set2, field, trent_positions);

    for (auto p : left_path2)
    {
        right_path_start.push_back(p);
    }

    return {true, trent_positions, 0, left_path, right_path_start};
}




class TreeRemover
{
private:
    vector<vector<bool>> visited;
    vector<vector<P>> prev;
    vector<vector<int>> costs;

public:
    vector<vector<CellType>> initial_field;
    TreeRemover(vector<vector<CellType>> &field) : initial_field(field)
    {
        visited = vector<vector<bool>>(N, vector<bool>(N, false));
        prev = vector<vector<P>>(N, vector<P>(N, {-1, -1}));
        costs = vector<vector<int>>(N, vector<int>(N, 0));
    }

    vector<P> remove_blocking_trees_of_flower_guard(FlowerGuard &flower_guard, vector<vector<CellType>> &field);
};

vector<P> TreeRemover::remove_blocking_trees_of_flower_guard(FlowerGuard &flower_guard, vector<vector<CellType>> &field)
{
    set<P> flower_guard_trees;
    for (auto tree_pos : flower_guard.tree_positions)
    {
        flower_guard_trees.insert(tree_pos);
    }

    rep(i, N) rep(j, N)
    {
        visited[i][j] = false;
        prev[i][j] = {-1, -1};
    }

    priority_queue<tuple<int, P, P>, vector<tuple<int, P, P>>, greater<tuple<int, P, P>>> pq;
    pq.push({0, {0, N / 2}, {-1, -1}}); // (cost, (x,y), prev)

    vector<int> dx = {-1, 1, 0, 0};
    vector<int> dy = {0, 0, -1, 1};

    while (!pq.empty())
    {
        auto [cost, pos, prev_pos] = pq.top();
        pq.pop();
        int x = pos.first;
        int y = pos.second;

        if (visited[x][y])
            continue;
        visited[x][y] = true;
        prev[x][y] = prev_pos;
        costs[x][y] = cost;

        rep(dir, 4)
        {
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                continue;
            if (visited[nx][ny])
                continue;

            // 最初からの木の時には消せないので通れない
            // 花も通れない
            // 花ガードの木も通れない
            if (initial_field[nx][ny] == FLOWER || initial_field[nx][ny] == TREE || flower_guard_trees.count({nx, ny}) > 0)
                continue;

            int add_cost = 0;
            if (field[nx][ny] == TREE)
            {
                add_cost = 1;
            }
            pq.push({cost + add_cost, {nx, ny}, {x, y}});
        }
    }
    // この時に、FlowerGuardの木に隣接するマスからスタート地点までの木を消していく

    set<P> check_positions;
    for (auto tree_pos : flower_guard.tree_positions)
    {
        for (int dir = 0; dir < 4; dir++)
        {
            int nx = tree_pos.first + dx[dir];
            int ny = tree_pos.second + dy[dir];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                continue;
            if (visited[nx][ny])
            {
                check_positions.insert({nx, ny});
            }
        }
    }

    set<P> removed_trees;
    // check_positionsからスタート地点までの経路を辿って木のところを記録する
    // これはflower_guardに隣接している空白へ到達できるようにするための処理
    for (auto start_pos : check_positions)
    {
        int x = start_pos.first;
        int y = start_pos.second;
        if (costs[x][y] == 0)
            continue; // 木を消さずに到達できる場合はスキップ
        int depth = 0;

        while (!(x == 0 && y == N / 2) && !(x == -1 && y == -1))
        {
            if (field[x][y] == TREE && flower_guard_trees.count({x, y}) == 0 && depth > 0)
            {
                removed_trees.insert({x, y});
            }
            auto [px, py] = prev[x][y];
            x = px;
            y = py;
            depth++;
        }
    }

    // 次にflower_guardの木に隣接する木を消せる時に消す
    /*
    set<P> delete_candidates;
    for (auto tree_pos : flower_guard.tree_positions)
    {
        for (int dir = 0; dir < 4; dir++)
        {
            if (abs(tree_pos.first - ti) + abs(tree_pos.second - tj) > 1)
                continue;
            int nx = tree_pos.first + dx[dir];
            int ny = tree_pos.second + dy[dir];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                continue;
            if (field[nx][ny] == TREE && flower_guard_trees.count({nx, ny}) == 0 && initial_field[nx][ny] != TREE)
            {
                delete_candidates.insert({nx, ny});
            }
        }
    }

    vector<P> delete_neighbor_tree_vec = vector<P>();

    auto count_connected_components = [&](int sx, int sy, vector<vector<CellType>> &test_field)
    {
        set<P> visited;
        int components = 0;
        vector<int> dx_vec = {-1, 1, 0, 0};
        vector<int> dy_vec = {0, 0, -1, 1};

        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                int xpos = sx + i;
                int ypos = sy + j;
                if (xpos < 0 || xpos >= N || ypos < 0 || ypos >= N)
                    continue;
                if (test_field[xpos][ypos] == TREE || test_field[xpos][ypos] == FLOWER || visited.count({xpos, ypos}) > 0)
                    continue;

                // BFSで連結成分を探索
                components++;
                queue<P> q;
                q.push({i, j});

                while (!q.empty())
                {
                    auto [dx, dy] = q.front();
                    q.pop();

                    int pos_x = sx + dx;
                    int pos_y = sy + dy;
                    if (visited.count({pos_x, pos_y}) > 0)
                        continue;
                    visited.insert({pos_x, pos_y});

                    for (int dir = 0; dir < 4; dir++)
                    {
                        int nx = pos_x + dx_vec[dir];
                        int ny = pos_y + dy_vec[dir];
                        int dx2 = nx - sx;
                        int dy2 = ny - sy;
                        if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                            continue;
                        if (abs(dx2) > 1 || abs(dy2) > 1)
                            continue;
                        if (visited.count({nx, ny}) > 0)
                            continue;
                        if (test_field[nx][ny] == TREE || test_field[nx][ny] == FLOWER)
                            continue;
                        q.push({dx2, dy2});
                    }
                }
            }
        }

        return components;
    };

    for (auto pos : delete_candidates)
    {
        // posの位置の木を消せるかをチェックする
        // 消せる条件はposの位置の周辺(yxともに+-1以内)における連結成分の数が変わらないこと
        int original_component_count = count_connected_components(pos.first, pos.second, field);
        field[pos.first][pos.second] = EMPTY;
        int new_component_count = count_connected_components(pos.first, pos.second, field);
        if (new_component_count == original_component_count)
        {
            delete_neighbor_tree_vec.push_back(pos);
        }
        else
        {
            field[pos.first][pos.second] = TREE;
        }
    }
    for (auto pos : delete_neighbor_tree_vec)
    {
        removed_trees.insert(pos);
        field[pos.first][pos.second] = EMPTY;
    }
    */

    return vector<P>(removed_trees.begin(), removed_trees.end());
}




// 各サイズにおけるデコイの数
struct DecoyCountList
{
    vector<int> counts;
    DecoyCountList() : counts(vector<int>(10, 0)) {}
    DecoyCountList(vector<int> c) : counts(c) {}
    int get_count(int size)
    {

        if (counts.size() <= size)
            return 0;
        return counts[size];
    }
};

class Evaluator
{
public:
    static double evaluate(DecoyCountList &left_decoy_count_list, DecoyCountList &right_decoy_count_list, int path_length);
    static double evaluate_single_side(DecoyCountList &decoy_count_list);
    static double strict_simulation(int num_runs, FlowerGuard flower_guard, vector<vector<CellType>> &field);
    static double run_simulation_batch(int num_runs, FlowerGuard flower_guard, vector<vector<CellType>> &field);
};

double fnA(double p)
{
    return p;
}
double fnB(double p)
{
    return (1. - (1. - p) * (1. - p)) / 2.;
}

double fnC(double p)
{
    return (1. - (1. - p) * (1. - p) * (1. - p)) / 3.;
}

double fnD(double p)
{
    return (1. - (1. - p) * (1. - p) * (1. - p) * (1. - p)) / 4.;
}

double fnE(double p)
{
    return (1. - (1. - p) * (1. - p) * (1. - p) * (1. - p) * (1. - p)) / 5.;
}

double fn_size1(double p)
{
    return fnA(p);
}

double fn_size2(double p)
{
    return fnA(p) + fnB(p);
}

double fn_size3(double p)
{
    return fnA(p) + fnB(p) + fnC(p);
}

double fn_size4(double p)
{
    return fnA(p) + fnB(p) + fnC(p) + fnD(p);
}

double fn_size5(double p)
{
    return fnA(p) + fnB(p) + fnC(p) + fnD(p) + fnE(p);
};

double Evaluator::evaluate_single_side(DecoyCountList &decoy_count_list)
{
    double score = 0.0;
    for (int i = 0; i < 10; i++)
    {
        double size = 0;
        double p = (double)(i + 1) / 10.0;
        size += decoy_count_list.get_count(1) * fn_size1(p);
        size += decoy_count_list.get_count(2) * fn_size2(p);
        size += decoy_count_list.get_count(3) * fn_size3(p);
        size += decoy_count_list.get_count(4) * fn_size4(p);
        size += decoy_count_list.get_count(5) * fn_size5(p);

        score += size;
    }

    return score / 10.0;
}

double Evaluator::evaluate(DecoyCountList &left_decoy_count_list, DecoyCountList &right_decoy_count_list, int path_length)
{
    double score = 0.0;
    for (int i = 0; i < 10; i++)
    {
        double left_size = 0;
        double right_size = 0;
        double p = (double)(i + 1) / 10.0;
        left_size += left_decoy_count_list.get_count(1) * fn_size1(p);
        right_size += right_decoy_count_list.get_count(1) * fn_size1(p);
        left_size += left_decoy_count_list.get_count(2) * fn_size2(p);
        right_size += right_decoy_count_list.get_count(2) * fn_size2(p);
        left_size += left_decoy_count_list.get_count(3) * fn_size3(p);
        right_size += right_decoy_count_list.get_count(3) * fn_size3(p);
        left_size += left_decoy_count_list.get_count(4) * fn_size4(p);
        right_size += right_decoy_count_list.get_count(4) * fn_size4(p);
        left_size += left_decoy_count_list.get_count(5) * fn_size5(p);
        right_size += right_decoy_count_list.get_count(5) * fn_size5(p);

        double ouhuku_count = 2 * left_size * right_size / (left_size + right_size + 1e-9);
        double inner_count = left_size + right_size - ouhuku_count;
        score += inner_count * ((double)N / 2.) + ouhuku_count * ((double)path_length + N);
    }

    return score / 10.0;
}

vector<int> best_score_accumulation;
int best_score_of_simulation = 0;

double Evaluator::run_simulation_batch(int num_runs, FlowerGuard flower_guard, vector<vector<CellType>> &field)
{
    if (get_time(false) > TIME_LIMIT)
        return 0;
    vector<int> scores;
    scores.reserve(num_runs);

    // 使い回し用の配列を事前確保（1次元配列に変更）
    static vector<CellType> sim_field;
    static vector<bool> revealed;
    static vector<P> unvisited_queue;
    static vector<int> dist;

    // 共通のunvisited_queueパターンを事前生成（20個程度）
    static vector<vector<P>> predefined_unvisited_queues;
    static bool queues_initialized = false;

    // 初回または サイズが変わった場合のみリサイズ
    if (sim_field.size() != N * N || !queues_initialized)
    {
        sim_field.assign(N * N, EMPTY);
        revealed.assign(N * N, false);
        dist.assign(N * N, INT_MAX);
        unvisited_queue.reserve(N * N);

        // 20通りのunvisited_queueパターンを事前生成
        predefined_unvisited_queues.clear();
        predefined_unvisited_queues.resize(100);

        // ベースとなるunvisited_queueを作成（初期フィールドベース）
        vector<P> base_queue;
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                base_queue.push_back({i, j});
            }
        }

        // 20通りの異なるシャッフルパターンを生成
        for (int pattern = 0; pattern < 100; pattern++)
        {
            predefined_unvisited_queues[pattern] = base_queue;

            // 異なるシードでシャッフル（再現性確保）
            mt19937 pattern_mt(pattern + 12345);
            shuffle(predefined_unvisited_queues[pattern].begin(),
                    predefined_unvisited_queues[pattern].end(), pattern_mt);

            P flower_pos = {ti, tj};
            auto it = find(predefined_unvisited_queues[pattern].begin(),
                           predefined_unvisited_queues[pattern].end(), flower_pos);
            auto flower_index = distance(predefined_unvisited_queues[pattern].begin(), it);

            int move_pos = (predefined_unvisited_queues[pattern].size() * (pattern % 10 + 1)) / 10 - 1;
            // int move_pos = (predefined_unvisited_queues[pattern].size() * 0.4);

            swap(predefined_unvisited_queues[pattern][move_pos], predefined_unvisited_queues[pattern][flower_index]);

            reverse(predefined_unvisited_queues[pattern].begin(),
                    predefined_unvisited_queues[pattern].end());
        }

        queues_initialized = true;
    }

    for (int run = 0; run < num_runs; run++)
    {
        if (run % 2 == 0 && skip_even_runs)
            continue;
        flower_guard.trap_used = false;
        // フィールドコピー（高速化、1次元配列版）
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                int idx = i * N + j;
                sim_field[idx] = field[i][j];
                revealed[idx] = false; // 初期化も同時に
                dist[idx] = INT_MAX;   // 距離配列も初期化
            }
        }

        // プレイヤーの初期位置
        int start_x = 0;
        int start_y = N / 2;
        int player_x = start_x, player_y = start_y;

        // 事前生成されたunvisited_queueパターンを順番に使用
        int pattern_index = run % predefined_unvisited_queues.size();
        unvisited_queue = predefined_unvisited_queues[pattern_index];

        // 現在のターゲット
        P target = {-1, -1}; // 無効なターゲット

        // 距離キャッシュ管理
        bool distance_cache_valid = false; // 距離キャッシュの有効性
        P cached_target = {-1, -1};        // キャッシュされた目的地

        // 訪問済み位置管理（視界更新最適化用、1次元配列版）
        static vector<bool> vision_updated;
        if (vision_updated.size() != N * N)
        {
            vision_updated.assign(N * N, false);
        }
        // 各シミュレーション開始時にリセット
        fill(vision_updated.begin(), vision_updated.end(), false);

        // 最短経路管理（再計算回避用）
        static vector<P> current_shortest_path;
        static set<P> path_cells;

        // 初期視界設定
        revealed[start_x * N + start_y] = true;

        // 4方向の視界を初期化 (lib.rsのDIJに合わせる)
        int dx[4] = {-1, 1, 0, 0}; // (!0, 0), (1, 0), (0, !0), (0, 1)
        int dy[4] = {0, 0, -1, 1};

        auto update_vision = [&]()
        {
            // 既に訪問済みの場所では視界更新をスキップ
            int player_idx = player_x * N + player_y;
            if (vision_updated[player_idx])
            {
                return;
            }

            vision_updated[player_idx] = true;
            bool vision_changed = false;
            bool path_affected = false;

            for (int dir = 0; dir < 4; dir++)
            {
                int x = player_x, y = player_y;
                while (x >= 0 && x < N && y >= 0 && y < N)
                {
                    int idx = x * N + y;
                    if (!revealed[idx])
                    {
                        revealed[idx] = true;
                        vision_changed = true;

                        // 新しく観測したマスが現在の最短経路に影響するかチェック
                        if (path_cells.count({x, y}))
                        {
                            path_affected = true;
                        }
                    }
                    // 木または花以外（つまり'.'）でない場合はbreak
                    if (sim_field[idx] != EMPTY)
                        break;
                    x += dx[dir];
                    y += dy[dir];
                }
            }

            // 視界が変わり、かつ最短経路に影響する場合のみキャッシュを無効化
            if (vision_changed && (path_affected || path_cells.empty()))
            {
                distance_cache_valid = false;
            }
        };

        auto check_and_update_flower_trap = [&player_x, &player_y](vector<CellType> &sim_field, FlowerGuard &flower_guard)
        {
            auto check = flower_guard.check_trap(player_x, player_y);
            if (check.first == -1)
            {
                return;
            }
            else
            {
                sim_field[check.first * N + check.second] = TREE;
            }
        };

        // BFS用のキューとvisitedフラグを静的に確保（高速化、1次元配列版）
        static queue<P> bfs_queue;
        static vector<bool> bfs_visited;
        static vector<P> visited_cells; // リセット用

        if (bfs_visited.size() != N * N)
        {
            bfs_visited.assign(N * N, false);
            visited_cells.reserve(N * N);
        }

        auto compute_distance_from_target = [&](P tgt)
        {
            // 前回訪問したセルのみをリセット（高速化、1次元配列版）
            for (P cell : visited_cells)
            {
                int cell_idx = cell.first * N + cell.second;
                bfs_visited[cell_idx] = false;
                dist[cell_idx] = INT_MAX;
            }
            visited_cells.clear();

            // キューをクリア
            while (!bfs_queue.empty())
                bfs_queue.pop();

            bfs_queue.push(tgt);
            int tgt_idx = tgt.first * N + tgt.second;
            dist[tgt_idx] = 0;
            bfs_visited[tgt_idx] = true;
            visited_cells.push_back(tgt);

            while (!bfs_queue.empty())
            {
                auto [x, y] = bfs_queue.front();
                bfs_queue.pop();

                // プレイヤーの現在位置に到達したら早期終了
                if (x == player_x && y == player_y)
                {
                    break;
                }

                for (int dir = 0; dir < 4; dir++)
                {
                    int nx = x + dx[dir];
                    int ny = y + dy[dir];

                    if (nx >= 0 && nx < N && ny >= 0 && ny < N)
                    {
                        int nx_idx = nx * N + ny;
                        if (!bfs_visited[nx_idx] &&
                            (!revealed[nx_idx] || sim_field[nx_idx] != TREE))
                        {
                            int x_idx = x * N + y;
                            dist[nx_idx] = dist[x_idx] + 1;
                            bfs_visited[nx_idx] = true;
                            visited_cells.push_back({nx, ny});
                            bfs_queue.push({nx, ny});
                        }
                    }
                }
            }
        };

        // 最短経路を構築する関数（1次元配列版）
        auto update_shortest_path = [&](P tgt)
        {
            current_shortest_path.clear();
            path_cells.clear();

            int player_idx = player_x * N + player_y;
            if (dist[player_idx] == INT_MAX)
            {
                return; // 到達不可能
            }

            // プレイヤーから目的地への最短経路を逆算
            int curr_x = player_x, curr_y = player_y;
            while (curr_x != tgt.first || curr_y != tgt.second)
            {
                current_shortest_path.push_back({curr_x, curr_y});
                path_cells.insert({curr_x, curr_y});

                // 隣接マスで距離が1少ないマスを探す
                bool found = false;
                int curr_idx = curr_x * N + curr_y;
                for (int dir = 0; dir < 4; dir++)
                {
                    int nx = curr_x + dx[dir];
                    int ny = curr_y + dy[dir];
                    if (nx >= 0 && nx < N && ny >= 0 && ny < N)
                    {
                        int nx_idx = nx * N + ny;
                        if (dist[nx_idx] == dist[curr_idx] - 1)
                        {
                            curr_x = nx;
                            curr_y = ny;
                            found = true;
                            break;
                        }
                    }
                }
                if (!found)
                    break;
            }
            current_shortest_path.push_back(tgt);
            path_cells.insert(tgt);
        };

        // キャッシュ機能付きの距離計算
        auto ensure_distance_cache = [&](P tgt)
        {
            if (!distance_cache_valid || cached_target != tgt)
            {
                compute_distance_from_target(tgt);
                update_shortest_path(tgt); // 最短経路を更新
                distance_cache_valid = true;
                cached_target = tgt;
            }
        };

        update_vision();

        int turns = 0;
        const int MAX_TURNS = 100000;

        while (turns < MAX_TURNS && (player_x != ti || player_y != tj))
        {
            if (get_time(false) > TIME_LIMIT)
                break;
            // 毎ターン何もしない（トレント配置なし）
            check_and_update_flower_trap(sim_field, flower_guard);

            // ターゲット選択ロジック（lib.rsと同じ順序で）
            // 1. 花が見えている場合は花をターゲット（1次元配列版）
            int flower_idx = ti * N + tj;
            if (revealed[flower_idx])
            {
                target = {ti, tj};
                // 花をターゲットにした場合は即座に距離計算
                ensure_distance_cache(target);
            }

            // 2. 現在のターゲットが到達不可能な場合は無効化（1次元配列版）
            if (target.first != -1 && target.second != -1)
            {
                ensure_distance_cache(target);
                int player_idx = player_x * N + player_y;
                if (dist[player_idx] == INT_MAX)
                {
                    target = {-1, -1};
                }
            }

            // 3. ターゲットが無効、または（花でない かつ 既に訪問済み）の場合は新しいターゲットを探す（1次元配列版）
            if (target.first == -1 || target.second == -1 ||
                (target.first != ti || target.second != tj) && revealed[target.first * N + target.second])
            {
                // 未訪問の場所から到達可能なものを順番に探す（Rust実装に合わせて）
                target = {-1, -1};
                while (!unvisited_queue.empty())
                {
                    // 最後尾から順番に取り出し（Rust実装のpop()に対応）
                    P candidate = unvisited_queue.back();
                    unvisited_queue.pop_back();

                    int candidate_idx = candidate.first * N + candidate.second;
                    if (!revealed[candidate_idx])
                    {
                        // 候補をターゲットにして到達可能性をチェック
                        ensure_distance_cache(candidate);
                        int player_idx = player_x * N + player_y;
                        if (dist[player_idx] != INT_MAX)
                        {
                            target = candidate;
                            break;
                        }
                    }
                }

                if (target.first == -1)
                {
                    // 到達可能な未訪問場所がない場合は失敗
                    break;
                }
            }

            // ターゲットからの距離を計算
            ensure_distance_cache(target);

            // 現在位置から到達不可能な場合は失敗（1次元配列版）
            int player_idx = player_x * N + player_y;
            if (dist[player_idx] == INT_MAX)
            {
                break;
            }

            // 最適な次の移動先を選択（距離が最小の隣接マス、1次元配列版）
            int best_dist = INT_MAX;
            P next_pos = {player_x, player_y};

            for (int dir = 0; dir < 4; dir++)
            {
                int nx = player_x + dx[dir];
                int ny = player_y + dy[dir];

                // lib.rsと同じ境界チェック: 負数は自動的に大きな値になるので nx < N && ny < N のみ
                if (nx >= 0 && nx < N && ny >= 0 && ny < N)
                {
                    int nx_idx = nx * N + ny;
                    if (dist[nx_idx] < best_dist)
                    {
                        best_dist = dist[nx_idx];
                        next_pos = {nx, ny};
                    }
                }
            }

            // 移動
            player_x = next_pos.first;
            player_y = next_pos.second;

            // 移動後の視界更新
            update_vision();

            turns++;
        }

        // 花に到達した場合はターン数、失敗した場合は0
        if (player_x == ti && player_y == tj)
        {
            scores.push_back(turns);
            // デバッグ出力を削除して高速化
        }
        else
        {
            scores.push_back(0);
            break;
            // デバッグ出力を削除して高速化
        }
        int tot_score = accumulate(scores.begin(), scores.end(), 0);
        double progress = (double)run / (num_runs);
        if (best_score_accumulation.size() > 0 && tot_score < best_score_accumulation[scores.size() - 1] * pow(score_calc_utikiri, (1. - progress)))
        {
            break;
        }
    }

    // 平均スコア計算
    double avg_score = 0.0;
    for (int score : scores)
    {
        avg_score += score;
    }
    avg_score /= num_runs;
    vector<int> accumulation;
    accumulation.push_back(scores[0]);
    for (int i = 1; i < (int)scores.size(); i++)
    {
        accumulation.push_back(accumulation.back() + scores[i]);
    }

    if (avg_score > best_score_of_simulation)
    {
        best_score_of_simulation = avg_score;
        best_score_accumulation = accumulation;
    }

    return avg_score;
}

/*
double Evaluator::strict_simulation(int num_runs, FlowerGuard flower_guard, vector<vector<CellType>> &field)
{
    vector<int> scores;
    scores.reserve(num_runs);

    // 使い回し用の配列を事前確保
    static vector<CellType> sim_field;
    static vector<bool> revealed;
    static vector<P> unvisited_queue;
    static vector<vector<int>> dist;

    // 共通のunvisited_queueパターンを事前生成（20個程度）
    static vector<vector<P>> predefined_unvisited_queues;
    static bool queues_initialized = false;

    // 初回または サイズが変わった場合のみリサイズ
    if (sim_field.size() != N || !queues_initialized)
    {
        sim_field.assign(N, vector<CellType>(N));
        revealed.assign(N, vector<bool>(N));
        dist.assign(N, vector<int>(N));
        unvisited_queue.reserve(N * N);

        // 20通りのunvisited_queueパターンを事前生成
        predefined_unvisited_queues.clear();
        predefined_unvisited_queues.resize(20);

        // ベースとなるunvisited_queueを作成（初期フィールドベース）
        vector<P> base_queue;
        rep(i, N)
        {
            for (int j = 0; j < N; j++)
            {
                base_queue.push_back({i, j});
            }
        }

        // 20通りの異なるシャッフルパターンを生成
        for (int pattern = 0; pattern < 20; pattern++)
        {
            predefined_unvisited_queues[pattern] = base_queue;

            // 異なるシードでシャッフル（再現性確保）
            mt19937 pattern_mt(pattern + 12345);
            shuffle(predefined_unvisited_queues[pattern].begin(),
                    predefined_unvisited_queues[pattern].end(), pattern_mt);

            P flower_pos = {ti, tj};
            auto it = find(predefined_unvisited_queues[pattern].begin(),
                           predefined_unvisited_queues[pattern].end(), flower_pos);
            auto flower_index = distance(predefined_unvisited_queues[pattern].begin(), it);

            int move_pos = (predefined_unvisited_queues[pattern].size() * (pattern % 10 + 1)) / 10 - 1;
            // int move_pos = (predefined_unvisited_queues[pattern].size() * 0.4);

            swap(predefined_unvisited_queues[pattern][move_pos], predefined_unvisited_queues[pattern][flower_index]);

            reverse(predefined_unvisited_queues[pattern].begin(),
                    predefined_unvisited_queues[pattern].end());
        }

        queues_initialized = true;
    }

    return 0;
}
    */




class DecoySetter
{
public:
    static pair<bool, DecoyCountList> set_decoys(vector<P> &path, vector<int> &current_trent_positions, vector<vector<CellType>> &field, vector<vector<CellType>> &initial_field);
};

pair<bool, DecoyCountList> DecoySetter::set_decoys(vector<P> &path, vector<int> &current_trent_positions, vector<vector<CellType>> &field, vector<vector<CellType>> &initial_field)
{
    int start_x = 0;
    int start_y = N / 2;
    // 深さ2のデコイを置けるかをチェックする
    auto candidate_pos = PseudoSet(N * N);
    auto must_open_pos = PseudoSet(N * N);
    must_open_pos.insert(ti * N + tj);
    must_open_pos.insert(start_x * N + start_y);

    // デコイを設置する領域について調査をする
    // flowerの2*2以内の領域には何も置かない
    auto path_set = set<P>(path.begin(), path.end());
    P last_path_pos = path.back();

    queue<P> que;
    que.push(last_path_pos);

    // last_path_posから到達できる場所でかつ、path_setに含まれない場所を調査する

    auto dx_vec = vector<int>{-1, 1, 0, 0};
    auto dy_vec = vector<int>{0, 0, -1, 1};
    set<P> visited;
    int empty_cnt = 0;
    bool add_empty = (ro.get_rand(0, 2) == 0);
    while (!que.empty())
    {
        auto [x, y] = que.front();
        que.pop();
        if (visited.count({x, y}) > 0)
            continue;
        visited.insert({x, y});

        rep(k, 4)
        {
            int nx = x + dx_vec[k];
            int ny = y + dy_vec[k];
            if (nx >= 0 && nx < N && ny >= 0 && ny < N &&
                field[nx][ny] != TREE &&
                path_set.count({nx, ny}) == 0)
            {
                if (visited.count({nx, ny}) > 0)
                    continue;
                candidate_pos.insert(nx * N + ny);
                que.push({nx, ny});
                empty_cnt++;
            }
            else if (
                nx >= 0 && nx < N && ny >= 0 && ny < N &&
                field[nx][ny] == TREE &&
                path_set.count({nx, ny}) == 0)
            {
                if (add_empty)
                    candidate_pos.insert(nx * N + ny);
            }
        }
    }
    for (int dx = -2; dx <= 2; dx++)
    {
        for (int dy = -2; dy <= 2; dy++)
        {
            int nx = ti + dx;
            int ny = tj + dy;
            if (nx >= 0 && nx < N && ny >= 0 && ny < N)
            {
                candidate_pos.erase(nx * N + ny);
                must_open_pos.insert(nx * N + ny);
            }
        }
    }
    int initial_size = candidate_pos.size();
    if (empty_cnt <= 10)
    {
        return {false, DecoyCountList()};
    }
    auto local_field = field;

    // candidateの各所を中心にして、置き方の候補を作っていく

    vector<CheckGrid> base_displacement1_1_0 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-2, 0, MustEmpty},
        {-2, 1, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, -1, MustTree},
        {-1, 1, MustTree},
        {-2, -1, MustTree},
        {-3, 0, MustTree}};
    vector<CheckGrid> base_displacement2_1_0 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-2, 0, MustEmpty},
        {-2, -1, MustEmpty},
        {1, 0, MustTree},
        {0, -1, MustTree},
        {0, 1, MustTree},
        {-1, 1, MustTree},
        {-1, -1, MustTree},
        {-2, 1, MustTree},
        {-3, 0, MustTree}};

    vector<CheckGrid> base_displacement1 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, 1, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, -1, MustTree},
        {-2, 0, MustTree}};

    vector<CheckGrid> base_displacement2 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, -1, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, 1, MustTree},
        {-2, 0, MustTree}};

    vector<CheckGrid> base_displacement1_2 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, 1, MustEmpty},
        {-2, 1, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, -1, MustTree},
        {-2, 0, MustTree},
        {-1, 2, MustTree}

    };

    vector<CheckGrid> base_displacement2_2 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, -1, MustEmpty},
        {-2, -1, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, 1, MustTree},
        {-2, 0, MustTree},
        {-1, -2, MustTree}};

    vector<CheckGrid> base_displacement1_3 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, 1, MustEmpty},
        {-2, 1, MustEmpty},
        {-2, 2, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, -1, MustTree},
        {-2, 0, MustTree},
        {-1, 2, MustTree},
        {-3, 1, MustTree},
    };

    vector<CheckGrid> base_displacement2_3 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, -1, MustEmpty},
        {-2, -1, MustEmpty},
        {-2, -2, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, 1, MustTree},
        {-2, 0, MustTree},
        {-1, -2, MustTree},
        {-3, -1, MustTree},
    };

    vector<CheckGrid> base_displacement1_4 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, 1, MustEmpty},
        {-2, 1, MustEmpty},
        {-2, 2, MustEmpty},
        {-3, 2, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, -1, MustTree},
        {-2, 0, MustTree},
        {-1, 2, MustTree},
        {-3, 1, MustTree},
        {-2, 3, MustTree},
    };

    vector<CheckGrid> base_displacement2_4 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, -1, MustEmpty},
        {-2, -1, MustEmpty},
        {-2, -2, MustEmpty},
        {-3, -2, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, 1, MustTree},
        {-2, 0, MustTree},
        {-1, -2, MustTree},
        {-3, -1, MustTree},
        {-2, -3, MustTree},
    };

    vector<CheckGrid> base_displacement1_5 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, 1, MustEmpty},
        {-2, 1, MustEmpty},
        {-2, 2, MustEmpty},
        {-3, 2, MustEmpty},
        {-3, 3, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, -1, MustTree},
        {-2, 0, MustTree},
        {-1, 2, MustTree},
        {-3, 1, MustTree},
        {-2, 3, MustTree},
        {-4, 2, MustTree},
    };

    vector<CheckGrid> base_displacement2_5 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, -1, MustEmpty},
        {-2, -1, MustEmpty},
        {-2, -2, MustEmpty},
        {-3, -2, MustEmpty},
        {-3, -3, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, 1, MustTree},
        {-2, 0, MustTree},
        {-1, -2, MustTree},
        {-3, -1, MustTree},
        {-2, -3, MustTree},
        {-4, -2, MustTree},
    };
    vector<CheckGrid> base_displacement1_6 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, 1, MustEmpty},
        {-2, 1, MustEmpty},
        {-2, 2, MustEmpty},
        {-3, 2, MustEmpty},
        {-3, 3, MustEmpty},
        {-4, 3, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, -1, MustTree},
        {-2, 0, MustTree},
        {-1, 2, MustTree},
        {-3, 1, MustTree},
        {-2, 3, MustTree},
        {-4, 2, MustTree},
        {-3, 4, MustTree},
    };

    vector<CheckGrid> base_displacement2_6 = {
        {0, 0, TreeOrEmpty},
        {-1, 0, MustEmpty},
        {-1, -1, MustEmpty},
        {-2, -1, MustEmpty},
        {-2, -2, MustEmpty},
        {-3, -2, MustEmpty},
        {-3, -3, MustEmpty},
        {-4, -3, MustEmpty},
        {1, 0, MustTree},
        {0, 1, MustTree},
        {0, -1, MustTree},
        {-1, 1, MustTree},
        {-2, 0, MustTree},
        {-1, -2, MustTree},
        {-3, -1, MustTree},
        {-2, -3, MustTree},
        {-4, -2, MustTree},
        {-3, -4, MustTree},
    };

    vector<vector<CheckGrid>> all_displacements1 = {
        base_displacement1,
        base_displacement2,
        rotate_displacement(base_displacement1, 90),
        rotate_displacement(base_displacement2, 90),
        rotate_displacement(base_displacement1, 180),
        rotate_displacement(base_displacement2, 180),
        rotate_displacement(base_displacement1, 270),
        rotate_displacement(base_displacement2, 270),
    };
    vector<vector<CheckGrid>> all_displacements1_0 = {
        base_displacement1_1_0,
        base_displacement2_1_0,
        rotate_displacement(base_displacement1_1_0, 90),
        rotate_displacement(base_displacement2_1_0, 90),
        rotate_displacement(base_displacement1_1_0, 180),
        rotate_displacement(base_displacement2_1_0, 180),
        rotate_displacement(base_displacement1_1_0, 270),
        rotate_displacement(base_displacement2_1_0, 270),
    };

    vector<vector<CheckGrid>> all_displacements2 = {
        base_displacement1_2,
        base_displacement2_2,
        rotate_displacement(base_displacement1_2, 90),
        rotate_displacement(base_displacement2_2, 90),
        rotate_displacement(base_displacement1_2, 180),
        rotate_displacement(base_displacement2_2, 180),
        rotate_displacement(base_displacement1_2, 270),
        rotate_displacement(base_displacement2_2, 270),
    };

    vector<vector<CheckGrid>> all_displacements3 = {
        base_displacement1_3,
        base_displacement2_3,
        rotate_displacement(base_displacement1_3, 90),
        rotate_displacement(base_displacement2_3, 90),
        rotate_displacement(base_displacement1_3, 180),
        rotate_displacement(base_displacement2_3, 180),
        rotate_displacement(base_displacement1_3, 270),
        rotate_displacement(base_displacement2_3, 270),
    };

    vector<vector<CheckGrid>> all_displacements4 = {
        base_displacement1_4,
        base_displacement2_4,
        rotate_displacement(base_displacement1_4, 90),
        rotate_displacement(base_displacement2_4, 90),
        rotate_displacement(base_displacement1_4, 180),
        rotate_displacement(base_displacement2_4, 180),
        rotate_displacement(base_displacement1_4, 270),
        rotate_displacement(base_displacement2_4, 270),
    };

    vector<vector<CheckGrid>> all_displacements5 = {
        base_displacement1_5,
        base_displacement2_5,
        rotate_displacement(base_displacement1_5, 90),
        rotate_displacement(base_displacement2_5, 90),
        rotate_displacement(base_displacement1_5, 180),
        rotate_displacement(base_displacement2_5, 180),
        rotate_displacement(base_displacement1_5, 270),
        rotate_displacement(base_displacement2_5, 270),
    };

    vector<vector<CheckGrid>> all_displacements6 = {
        base_displacement1_6,
        base_displacement2_6,
        rotate_displacement(base_displacement1_6, 90),
        rotate_displacement(base_displacement2_6, 90),
        rotate_displacement(base_displacement1_6, 180),
        rotate_displacement(base_displacement2_6, 180),
        rotate_displacement(base_displacement1_6, 270),
        rotate_displacement(base_displacement2_6, 270),
    };

    // 全ての候補地点・全ての置き方パターンを試して、筋が良いところに置いていく
    // 置くブロックの数、中心の場所、置き方パターン, 置き方パターンindex
    vector<int> decoy_count_list(10, 0);

    vector<vector<vector<CheckGrid>>> disp_vec = {all_displacements5, all_displacements4, all_displacements3, all_displacements2, all_displacements1};
    rep(jj, disp_vec.size())
    {
        auto all_displacements = disp_vec[jj];
        set<P> checked;

        int cnt = 0;

        while (true)
        {
            cnt++;
            vector<tuple<int, int, vector<CheckGrid>, int, float>> valid_patterns;

            for (auto pos : candidate_pos.data)
            {
                int cx = pos / N;
                int cy = pos % N;

                for (int i = 0; i < all_displacements.size(); i++)
                {
                    if (checked.count({cx * N + cy, i}) > 0)
                        continue;
                    auto displacement = all_displacements[i];
                    auto [can_place, placed_trees] = can_make_objects(cx, cy, local_field, displacement);
                    if (placed_trees.size() == 0)
                        continue; // 木を一つも置かないパターンはどちらにせよそのままでいい
                    if (can_place)
                    {
                        valid_patterns.emplace_back(placed_trees.size(), pos, displacement, i, ro.get_float());
                    }
                }
            }
            if (valid_patterns.size() == 0)
            {
                break; // もう置ける場所がない
            }

            // 置くのブロックの数小さい順にソートをする
            // タイブレークで最後のフロートを使う
            sort(valid_patterns.begin(), valid_patterns.end(),
                 [](const auto &a, const auto &b)
                 {
                     if (get<0>(a) != get<0>(b))
                         return get<0>(a) < get<0>(b);
                     return get<4>(a) < get<4>(b);
                 });

            int minimum_block_count = get<0>(valid_patterns.front());

            // 最小のブロック数のものだけを置く
            int index = 0;
            while (index < (int)valid_patterns.size() && get<0>(valid_patterns[index]) <= minimum_block_count + cnt)
            {
                auto [block_count, pos, displacement, displacement_index, _float] = valid_patterns[index];

                checked.insert({pos, displacement_index});
                index++;

                if (must_open_pos.count(pos))
                    continue;
                auto [can_place, placed_trees] = can_make_objects(pos / N, pos % N, local_field, displacement);
                if (!can_place || placed_trees.size() == 0)
                    continue;

                // 実際に置く
                vector<P> new_trees;
                for (auto placed_tree : placed_trees)
                {
                    if (!local_field[placed_tree.x][placed_tree.y] == EMPTY)
                    {
                        throw runtime_error("logic error in set_decoys: trying to place on non-empty cell");
                    }
                    local_field[placed_tree.x][placed_tree.y] = TREE;
                    new_trees.push_back({placed_tree.x, placed_tree.y});
                }

                bool connected = check_bfs_connectivity(local_field, new_trees, 5);

                // 追いちゃいいけないところに置いていないかをチェックする
                for (auto placed_tree : placed_trees)
                {
                    if (must_open_pos.count(placed_tree.x * N + placed_tree.y))
                    {
                        connected = false;
                        break;
                    }
                }

                if (!connected)
                {
                    // 連結性が保たれない場合は元に戻す
                    for (auto placed_tree : placed_trees)
                    {
                        local_field[placed_tree.x][placed_tree.y] = EMPTY;
                    }
                }
                else
                {
                    decoy_count_list[jj + 1]++;
                    // 成功したのでOK
                    for (auto placed_tree : placed_trees)
                    {
                        current_trent_positions.push_back(placed_tree.x * N + placed_tree.y);
                    }
                    for (auto disp : displacement)
                    {
                        if (disp.type == MustEmpty || disp.type == TreeOrEmpty)
                        {
                            int nx = disp.dx + pos / N;
                            int ny = disp.dy + pos % N;
                            if (nx >= 0 && nx < N && ny >= 0 && ny < N)
                            {
                                // candidate_pos.erase(nx * N + ny);
                                must_open_pos.insert(nx * N + ny);
                            }
                        }
                    }
                }
            }
        }
    }
    auto tmp = candidate_pos.data;
    shuffle(tmp.begin(), tmp.end(), mt);

    for (auto pos : tmp)
    {
        int x = pos / N;
        int y = pos % N;
        if (!local_field[x][y] == EMPTY)
            continue;
        // 隣接する2箇所にブロックがない場合はOK

        int neighbor_cnt = 0;
        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx_vec[dir];
            int ny = y + dy_vec[dir];
            if (nx >= 0 && nx < N && ny >= 0 && ny < N)
            {
                if (local_field[nx][ny] == TREE)
                {
                    neighbor_cnt++;
                }
            }
        }

        if (neighbor_cnt >= 2)
            continue;

        // ここに置くことができる場合にはここに置く
        if (local_field[x][y] == EMPTY &&
            !must_open_pos.count(pos))
        {
            local_field[x][y] = TREE;
            auto pos_vec = vector<P>{{x, y}};
            auto connected = check_bfs_connectivity(local_field, pos_vec, 3);
            if (connected)
            {
                current_trent_positions.push_back(pos);
            }
            else
            {
                local_field[x][y] = EMPTY;
            }
        }
    }

    field = local_field;
    return {true, DecoyCountList(decoy_count_list)};
}




void cerr_board(const vector<vector<CellType>> &f)
{
    rep(i, N)
    {
        rep(j, N)
        {
            if (f[i][j] == EMPTY)
                cerr << ".";
            else if (f[i][j] == TREE)
                cerr << "T";
            else if (f[i][j] == FLOWER)
                cerr << "F";
        }
        cerr << endl;
    }
}

class Game
{
private:
    tuple<bool, vector<int>, int, vector<P>, vector<P>> try_construct_vertical_with_params(int pos, float left_wall_follow_length, float right_wall_follow_length);
    int min_distance(int sx, int sy, int gx, int gy, vector<vector<CellType>> &f);

    bool is_field_connected(const vector<vector<CellType>> &field);
    bool can_reach_flower(const vector<vector<CellType>> &field);
    int count_reachable_empty_cells(const vector<vector<CellType>> &field);

    // 花ガード関連の関数
    P rotate_offset(P offset, int rotation);
    int choose_open_direction_from_flower();
    vector<P> build_flower_guard(set<P> &protected_positions);
    vector<P> try_place_guard_pattern(int center_x, int center_y, int open_dir, set<P> &protected_positions, int original_reachable_count);

    // シミュレーション関連の関数
    double rough_estimation_of_score(int path_length);

public:
    Game();
    vector<vector<CellType>> field;
    vector<vector<CellType>> initial_field;
    int start_x, start_y;
    FlowerGuard construct_initial_trents();
};

int Game::min_distance(int sx, int sy, int gx, int gy, vector<vector<CellType>> &f)
{
    vector<vector<int>> dist(N, vector<int>(N, INT_MAX));
    dist[sx][sy] = 0;
    queue<P> q;
    q.push({sx, sy});
    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};
    // cerr << "goal:" << f[gx][gy] << endl;
    // cerr_board(f);
    while (!q.empty())
    {
        auto [x, y] = q.front();
        rep(i, 4)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                continue;
            if (f[nx][ny] == TREE)
                continue;
            if (dist[nx][ny] > dist[x][y] + 1)
            {
                dist[nx][ny] = dist[x][y] + 1;
                q.push({nx, ny});
                if (nx == gx && ny == gy)
                {
                    return dist[nx][ny];
                }
            }
        }
    }
    return dist[gx][gy];
}

Game::Game()
{
    start_x = 0;
    start_y = N / 2;
    field = vector<vector<CellType>>(N, vector<CellType>(N, EMPTY));
    rep(i, N) rep(j, N)
    {
        if (b[i][j] == 'T')
            field[i][j] = TREE;
        else
            field[i][j] = EMPTY;
    }
    field[ti][tj] = FLOWER;
    initial_field = field;
}

bool Game::is_field_connected(const vector<vector<CellType>> &field)
{
    // スタート地点から全てのEMPTYマスに到達可能かをBFSで確認
    vector<vector<bool>> visited(N, vector<bool>(N, false));
    queue<P> q;
    q.push({start_x, start_y});
    visited[start_x][start_y] = true;

    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};
    int reachable_empty_count = 1; // スタート地点もカウント

    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if (nx >= 0 && nx < N && ny >= 0 && ny < N &&
                !visited[nx][ny] && field[nx][ny] != TREE)
            {
                visited[nx][ny] = true;
                reachable_empty_count++;
                q.push({nx, ny});
            }
        }
    }

    // 全てのEMPTY/FLOWERマスの数を数える
    int total_empty_count = 0;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (field[i][j] != TREE)
            {
                total_empty_count++;
            }
        }
    }

    // 到達可能なマス数が全EMPTYマス数と等しければ連結
    return reachable_empty_count == total_empty_count;
}

bool Game::can_reach_flower(const vector<vector<CellType>> &field)
{
    // スタート地点から花まで到達可能かをBFSで確認
    vector<vector<bool>> visited(N, vector<bool>(N, false));
    queue<P> q;
    q.push({start_x, start_y});
    visited[start_x][start_y] = true;

    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};

    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        // 花に到達した場合
        if (x == ti && y == tj)
        {
            return true;
        }

        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if (nx >= 0 && nx < N && ny >= 0 && ny < N &&
                !visited[nx][ny] && field[nx][ny] != TREE)
            {
                visited[nx][ny] = true;
                q.push({nx, ny});
            }
        }
    }

    return false; // 花に到達できない場合
}

int Game::count_reachable_empty_cells(const vector<vector<CellType>> &field)
{
    // スタート地点から到達可能なEMPTY/FLOWERマスの数をカウント
    vector<vector<bool>> visited(N, vector<bool>(N, false));
    queue<P> q;
    q.push({start_x, start_y});
    visited[start_x][start_y] = true;

    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};
    int reachable_count = 1; // スタート地点もカウント

    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if (nx >= 0 && nx < N && ny >= 0 && ny < N &&
                !visited[nx][ny] && field[nx][ny] != TREE)
            {
                visited[nx][ny] = true;
                reachable_count++;
                q.push({nx, ny});
            }
        }
    }

    return reachable_count;
}

P Game::rotate_offset(P offset, int rotation)
{
    int x = offset.first, y = offset.second;
    rotation %= 4;
    if (rotation == 0)
        return {x, y};
    if (rotation == 1)
        return {-y, x};
    if (rotation == 2)
        return {-x, -y};
    return {y, -x};
}

int Game::choose_open_direction_from_flower()
{
    int di = start_x - ti;
    int dj = start_y - tj;
    if (abs(di) >= abs(dj))
    {
        if (di < 0)
            return 0; // 上方向
        if (di > 0)
            return 2;          // 下方向
        return dj > 0 ? 1 : 3; // 右または左
    }
    else
    {
        return dj > 0 ? 1 : 3; // 右または左
    }
}

vector<P> Game::build_flower_guard(set<P> &protected_positions)
{
    // 厳重な花ガードパターンを最初に試す
    // .xxx
    // xooox  (開口方向が右の場合)
    // xfxox
    // パターン1: より厳重なガード
    // パターン2: シンプルな花ガードパターン（既存）
    // 花の周りの3方向をブロックし、1方向を開ける
    // 既存の木も活用する

    // 花の隣接4方向
    vector<P> adjacent = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}; // 上、右、下、左
    shuffle(adjacent.begin(), adjacent.end(), mt);           // ランダム化

    // 各方向を開口方向として試す
    for (int open_dir = 0; open_dir < 4; open_dir++)
    {

        vector<P> required_trees; // 新しく配置が必要な木
        vector<P> protected_area; // 保護する空のマス
        bool valid = true;

        // 花の隣接3方向に木が必要（開口方向以外）
        for (int dir = 0; dir < 4; dir++)
        {
            if (dir == open_dir)
                continue; // 開口方向はスキップ

            P pos = {ti + adjacent[dir].first, tj + adjacent[dir].second};

            // 境界チェック
            if (pos.first < 0 || pos.first >= N || pos.second < 0 || pos.second >= N)
            {
                valid = false;
                break;
            }

            // スタート地点チェック
            if (pos.first == start_x && pos.second == start_y)
            {
                valid = false;
                break;
            }

            // 既に木がある場合は OK、空の場合は新設が必要
            if (field[pos.first][pos.second] == TREE)
            {
            }
            else if (field[pos.first][pos.second] == EMPTY)
            {
                if (protected_positions.count(pos) == 0)
                {
                    required_trees.push_back(pos);
                }
                else
                {
                    valid = false;
                    break;
                }
            }
            else
            {
                valid = false;
                break;
            }
        }

        // 開口方向の2マス先に木を配置（ガード用）
        P guard_pos = {ti + 2 * adjacent[open_dir].first, tj + 2 * adjacent[open_dir].second};
        if (guard_pos.first >= 0 && guard_pos.first < N && guard_pos.second >= 0 && guard_pos.second < N)
        {
            if (field[guard_pos.first][guard_pos.second] == EMPTY &&
                !(guard_pos.first == start_x && guard_pos.second == start_y) &&
                protected_positions.count(guard_pos) == 0)
            {
                required_trees.push_back(guard_pos);
            }
        }

        // 開口部の片方に木を配置（もう片方は空ける）
        int next_dir = (open_dir + 1) % 4;
        int prev_dir = (open_dir + 3) % 4;

        // 開口部の隣接位置（2つの候補）
        P side1_pos = {ti + adjacent[open_dir].first + adjacent[next_dir].first,
                       tj + adjacent[open_dir].second + adjacent[next_dir].second};
        P side2_pos = {ti + adjacent[open_dir].first + adjacent[prev_dir].first,
                       tj + adjacent[open_dir].second + adjacent[prev_dir].second};

        // どちらか片方に木を配置する（優先順位: side1 -> side2）
        bool side_placed = false;

        // side1を試す
        if (side1_pos.first >= 0 && side1_pos.first < N && side1_pos.second >= 0 && side1_pos.second < N)
        {
            if (field[side1_pos.first][side1_pos.second] == TREE)
            {
                // 既に木がある場合はOK
                side_placed = true;
                protected_area.push_back(side2_pos); // もう片方は保護領域
            }
            else if (field[side1_pos.first][side1_pos.second] == EMPTY &&
                     !(side1_pos.first == start_x && side1_pos.second == start_y) &&
                     protected_positions.count(side1_pos) == 0)
            {
                required_trees.push_back(side1_pos);
                side_placed = true;
                protected_area.push_back(side2_pos); // もう片方は保護領域
            }
        }

        // side1が配置できない場合はside2を試す
        if (!side_placed && side2_pos.first >= 0 && side2_pos.first < N && side2_pos.second >= 0 && side2_pos.second < N)
        {
            if (field[side2_pos.first][side2_pos.second] == TREE)
            {
                // 既に木がある場合はOK
                side_placed = true;
                protected_area.push_back(side1_pos); // もう片方は保護領域
            }
            else if (field[side2_pos.first][side2_pos.second] == EMPTY &&
                     !(side2_pos.first == start_x && side2_pos.second == start_y) &&
                     protected_positions.count(side2_pos) == 0)
            {
                required_trees.push_back(side2_pos);
                side_placed = true;
                protected_area.push_back(side1_pos); // もう片方は保護領域
            }
        }

        // どちらも配置できない場合は無効
        if (!side_placed)
        {
            valid = false;
        }

        if (!valid)
            continue;

        // 開口方向の先（2マス先）に木を配置
        P open_pos = {ti + adjacent[open_dir].first, tj + adjacent[open_dir].second};
        guard_pos = {ti + 2 * adjacent[open_dir].first, tj + 2 * adjacent[open_dir].second};

        // 開口方向の隣接マスを保護領域に
        if (open_pos.first >= 0 && open_pos.first < N && open_pos.second >= 0 && open_pos.second < N &&
            field[open_pos.first][open_pos.second] == EMPTY)
        {
            protected_area.push_back(open_pos);
        }

        // 開口方向の対角線上のマス（2つ）も保護領域に
        next_dir = (open_dir + 1) % 4;
        prev_dir = (open_dir + 3) % 4;

        P diag1 = {ti + adjacent[open_dir].first + adjacent[next_dir].first,
                   tj + adjacent[open_dir].second + adjacent[next_dir].second};
        P diag2 = {ti + adjacent[open_dir].first + adjacent[prev_dir].first,
                   tj + adjacent[open_dir].second + adjacent[prev_dir].second};

        if (diag1.first >= 0 && diag1.first < N && diag1.second >= 0 && diag1.second < N &&
            field[diag1.first][diag1.second] == EMPTY)
        {
            protected_area.push_back(diag1);
        }

        if (diag2.first >= 0 && diag2.first < N && diag2.second >= 0 && diag2.second < N &&
            field[diag2.first][diag2.second] == EMPTY)
        {
            protected_area.push_back(diag2);
        }

        // 花到達チェック（花ガードは花に到達できればOK）
        auto test_field = field;
        for (P pos : required_trees)
        {
            test_field[pos.first][pos.second] = TREE;
        }
        if (!can_reach_flower(test_field))
        {
            continue;
        }

        // 成功した場合、保護領域を更新
        protected_positions.insert({ti, tj});
        for (P pos : protected_area)
        {
            protected_positions.insert(pos);
        }

        // 花ガードの木に隣接する全ての位置を保護領域に追加（シンプルパターン）
        int adj_dx[4] = {-1, 1, 0, 0};
        int adj_dy[4] = {0, 0, -1, 1};
        for (P guard_tree : required_trees)
        {
            for (int adj_dir = 0; adj_dir < 4; adj_dir++)
            {
                int adj_x = guard_tree.first + adj_dx[adj_dir];
                int adj_y = guard_tree.second + adj_dy[adj_dir];

                if (adj_x >= 0 && adj_x < N && adj_y >= 0 && adj_y < N)
                {
                    P adj_pos = {adj_x, adj_y};
                    if (protected_positions.find(adj_pos) == protected_positions.end())
                    {
                        protected_positions.insert(adj_pos);
                    }
                }
            }
        }

        return required_trees;
    }

    // 失敗した場合は花だけ保護
    protected_positions.insert({ti, tj});
    return {};
}

vector<P> Game::try_place_guard_pattern(int center_x, int center_y, int open_dir, set<P> &protected_positions, int original_reachable_count)
{
    // 正しいガードパターンを配置
    // .x.
    // xoo  (開口方向が右の場合)
    // xfx
    // .x.

    vector<P> adjacent = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}}; // 上、右、下、左

    vector<P> required_trees; // 新しく配置が必要な木
    vector<P> protected_area; // 保護する空のマス
    bool valid = true;

    // 中心の3方向に木が必要（開口方向以外）
    for (int dir = 0; dir < 4; dir++)
    {
        if (dir == open_dir)
            continue; // 開口方向はスキップ

        P pos = {center_x + adjacent[dir].first, center_y + adjacent[dir].second};

        // 境界チェック
        if (pos.first < 0 || pos.first >= N || pos.second < 0 || pos.second >= N)
        {
            valid = false;
            break;
        }

        // スタート地点・花チェック
        if ((pos.first == start_x && pos.second == start_y) ||
            (pos.first == ti && pos.second == tj))
        {
            valid = false;
            break;
        }

        // 既に木がある場合は OK、空の場合は新設が必要
        if (field[pos.first][pos.second] == TREE)
        {
            // 既存の木を利用
        }
        else if (field[pos.first][pos.second] == EMPTY)
        {
            if (protected_positions.count(pos) == 0)
            {
                required_trees.push_back(pos);
            }
            else
            {
                valid = false;
                break;
            }
        }
        else
        {
            valid = false;
            break;
        }
    }

    if (!valid)
        return {};

    // 開口方向の2マス先に木を配置（ガード用）
    P guard_pos = {center_x + 2 * adjacent[open_dir].first, center_y + 2 * adjacent[open_dir].second};
    if (guard_pos.first >= 0 && guard_pos.first < N && guard_pos.second >= 0 && guard_pos.second < N)
    {
        if (field[guard_pos.first][guard_pos.second] == EMPTY &&
            !(guard_pos.first == start_x && guard_pos.second == start_y) &&
            !(guard_pos.first == ti && guard_pos.second == tj) &&
            protected_positions.count(guard_pos) == 0)
        {
            required_trees.push_back(guard_pos);
        }
    }

    // 開口部の片方に木を配置（もう片方は空ける）
    int next_dir = (open_dir + 1) % 4;
    int prev_dir = (open_dir + 3) % 4;

    // 開口部の隣接位置（2つの候補）
    P side1_pos = {center_x + adjacent[open_dir].first + adjacent[next_dir].first,
                   center_y + adjacent[open_dir].second + adjacent[next_dir].second};
    P side2_pos = {center_x + adjacent[open_dir].first + adjacent[prev_dir].first,
                   center_y + adjacent[open_dir].second + adjacent[prev_dir].second};

    // どちらか片方に木を配置する（優先順位: side1 -> side2）
    bool side_placed = false;

    // side1を試す
    if (side1_pos.first >= 0 && side1_pos.first < N && side1_pos.second >= 0 && side1_pos.second < N)
    {
        if (field[side1_pos.first][side1_pos.second] == TREE)
        {
            // 既に木がある場合はOK
            side_placed = true;
            if (side2_pos.first >= 0 && side2_pos.first < N && side2_pos.second >= 0 && side2_pos.second < N &&
                field[side2_pos.first][side2_pos.second] == EMPTY)
            {
                protected_area.push_back(side2_pos); // もう片方は保護領域
            }
        }
        else if (field[side1_pos.first][side1_pos.second] == EMPTY &&
                 !(side1_pos.first == start_x && side1_pos.second == start_y) &&
                 !(side1_pos.first == ti && side1_pos.second == tj) &&
                 protected_positions.count(side1_pos) == 0)
        {
            required_trees.push_back(side1_pos);
            side_placed = true;
            if (side2_pos.first >= 0 && side2_pos.first < N && side2_pos.second >= 0 && side2_pos.second < N &&
                field[side2_pos.first][side2_pos.second] == EMPTY)
            {
                protected_area.push_back(side2_pos); // もう片方は保護領域
            }
        }
    }

    // side1が配置できない場合はside2を試す
    if (!side_placed && side2_pos.first >= 0 && side2_pos.first < N && side2_pos.second >= 0 && side2_pos.second < N)
    {
        if (field[side2_pos.first][side2_pos.second] == TREE)
        {
            // 既に木がある場合はOK
            side_placed = true;
            if (side1_pos.first >= 0 && side1_pos.first < N && side1_pos.second >= 0 && side1_pos.second < N &&
                field[side1_pos.first][side1_pos.second] == EMPTY)
            {
                protected_area.push_back(side1_pos); // もう片方は保護領域
            }
        }
        else if (field[side2_pos.first][side2_pos.second] == EMPTY &&
                 !(side2_pos.first == start_x && side2_pos.second == start_y) &&
                 !(side2_pos.first == ti && side2_pos.second == tj) &&
                 protected_positions.count(side2_pos) == 0)
        {
            required_trees.push_back(side2_pos);
            side_placed = true;
            if (side1_pos.first >= 0 && side1_pos.first < N && side1_pos.second >= 0 && side1_pos.second < N &&
                field[side1_pos.first][side1_pos.second] == EMPTY)
            {
                protected_area.push_back(side1_pos); // もう片方は保護領域
            }
        }
    }

    // どちらも配置できない場合は無効
    if (!side_placed)
    {
        return {};
    }

    // 開口方向の隣接マスとその対角線上のマスを保護領域に
    P open_pos = {center_x + adjacent[open_dir].first, center_y + adjacent[open_dir].second};
    if (open_pos.first >= 0 && open_pos.first < N && open_pos.second >= 0 && open_pos.second < N &&
        field[open_pos.first][open_pos.second] == EMPTY)
    {
        protected_area.push_back(open_pos);
    }

    // 開口方向の対角線上のマス（2つ）も保護領域に
    next_dir = (open_dir + 1) % 4;
    prev_dir = (open_dir + 3) % 4;

    P diag1 = {center_x + adjacent[open_dir].first + adjacent[next_dir].first,
               center_y + adjacent[open_dir].second + adjacent[next_dir].second};
    P diag2 = {center_x + adjacent[open_dir].first + adjacent[prev_dir].first,
               center_y + adjacent[open_dir].second + adjacent[prev_dir].second};

    if (diag1.first >= 0 && diag1.first < N && diag1.second >= 0 && diag1.second < N &&
        field[diag1.first][diag1.second] == EMPTY)
    {
        protected_area.push_back(diag1);
    }

    if (diag2.first >= 0 && diag2.first < N && diag2.second >= 0 && diag2.second < N &&
        field[diag2.first][diag2.second] == EMPTY)
    {
        protected_area.push_back(diag2);
    }

    // 連結性チェック（到達可能マス数が減らないか）
    auto test_field = field;
    for (P pos : required_trees)
    {
        test_field[pos.first][pos.second] = TREE;
    }
    int new_reachable_count = count_reachable_empty_cells(test_field);
    if (new_reachable_count < original_reachable_count - (int)required_trees.size())
    {
        return {}; // 到達可能マス数が予想以上に減る場合は失敗
    }

    // 成功した場合、保護領域を更新
    for (P pos : protected_area)
    {
        protected_positions.insert(pos);
    }

    return required_trees;
}

double Game::rough_estimation_of_score(int path_length)
{

    vector<int> dx = {-1, 1, 0, 0};
    vector<int> dy = {0, 0, -1, 1};
    // BFSをして、スタート地点から到達可能かどうかチェックするr
    vector<vector<bool>> visited_mat(N, vector<bool>(N, false));
    queue<P> q;
    q.push({start_x, start_y});
    visited_mat[start_x][start_y] = true;
    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        for (int dir = 0; dir < 4; dir++)
        {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if (nx >= 0 && nx < N && ny >= 0 && ny < N &&
                !visited_mat[nx][ny] && field[nx][ny] != TREE)
            {
                visited_mat[nx][ny] = true;
                q.push({nx, ny});
            }
        }
    }
    if (!visited_mat[ti][tj])
    {
        // 花に到達できない場合はスコア0
        return 0.0;
    }

    // 左右でそれぞれ穴になるところがいくつあるかを調査する
    // この間を往復することスコアを稼ぐことになる

    vector<vector<int>> neighbor_empty_count(N, vector<int>(N, 0));
    vector<P> one_neighbor_cells;

    rep(i, N) rep(j, N)
    {
        if (!visited_mat[i][j])
            continue;
        if (field[i][j] == EMPTY)
        {
            int count = 0;
            rep(dir, 4)
            {
                int ni = i + dx[dir];
                int nj = j + dy[dir];
                if (ni >= 0 && ni < N && nj >= 0 && nj < N)
                {
                    if (field[ni][nj] == EMPTY || field[ni][nj] == FLOWER)
                    {
                        count++;
                    }
                }
            }
            neighbor_empty_count[i][j] = count;
            if (count == 1)
            {
                one_neighbor_cells.push_back({i, j});
            }
        }
    }

    // 左右で穴の数を数える
    double left_hole_score = 0;
    double right_hole_score = 0;

    set<int> visited;
    for (auto [x, y] : one_neighbor_cells)
    {
        // 一つの隣接セルの方向にいくつの穴が続くかを数える
        if (visited.count(x * N + y))
            continue;
        int dir = -1;
        rep(d, 4)
        {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < N && ny >= 0 && ny < N)
            {
                if (field[nx][ny] == EMPTY || field[nx][ny] == FLOWER)
                {
                    dir = d;
                    break;
                }
            }
        }
        int cnt = 0;
        int cx = x;
        int cy = y;
        while (true)
        {
            int nx = cx + dx[dir];
            int ny = cy + dy[dir];
            if (nx >= 0 && nx < N && ny >= 0 && ny < N)
            {
                if (field[nx][ny] == EMPTY || field[nx][ny] == FLOWER)
                {
                    visited.insert(nx * N + ny);
                    cnt++;
                    cx = nx;
                    cy = ny;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        if (cnt < 3)
        {
            if (x < N / 2)
            {
                left_hole_score += 1;
            }
            else
            {
                right_hole_score += 1;
            }
        }
    }
    // 概ねmern(left_hole_score, right_hole_score) * 2*path_lengthがスコアになると予想

    return min(left_hole_score, right_hole_score) * 2.0 * path_length;
}

FlowerGuard Game::construct_initial_trents()
{
    double score_calc_times = 0;

    vector<int> best_trent_positions;
    double best_score = -100.0;
    int iteration = 0;
    FlowerGuard best_flower_guard = FlowerGuard();
    auto remover = TreeRemover(initial_field);
    vector<P> best_removed_trees;

    float ct = get_time(false);

    while (get_time(false) < TIME_LIMIT - 10) // 1800ms制限
    {
        iteration++;

        // 平均時間を求めて、残り時間から大体の残り試行回数を計算
        // Candidate cand = bo.ask(remaining);
        Candidate cand = Candidate();

        int total_path_length = 0;
        // フィールドを初期状態にリセット
        field = initial_field;

        auto initial_router = InitialRouter();

        vector<int> current_trent_positions;
        vector<P> left_path, right_path;
        if (ro.get_float() < horizontal_or_vertical_threshold)
        {
            // 縦経路
            auto [tmp_left_path, tmp_right_path] = initial_router.try_construct_vertical(field, current_trent_positions, cand);
            left_path = tmp_left_path;
            right_path = tmp_right_path;
        }
        else
        {
            // 横経路
            auto [tmp_left_path, tmp_right_path] = initial_router.try_construct_horizontal(field, current_trent_positions, cand);
            left_path = tmp_left_path;
            right_path = tmp_right_path;
        }

        if (current_trent_positions.empty())
        {
            continue;
        }

        auto flower_guard = FlowerGuard::build_from_field(field);
        for (auto pos : flower_guard.tree_positions)
        {
            field[pos.first][pos.second] = TREE;
            current_trent_positions.push_back(pos.first * N + pos.second);
        }
        if (flower_guard.tree_positions.size() == 0)
        {
            flower_guard = FlowerGuard::build_from_field_by_bfs(field);
            for (auto pos : flower_guard.tree_positions)
            {
                field[pos.first][pos.second] = TREE;
                current_trent_positions.push_back(pos.first * N + pos.second);
            }
        }

        // 花ガードを配置
        if (flower_guard.tree_positions.size() == 0)
        {
            set<P> protected_positions = {{ti, tj}};
            vector<P> flower_guard_positions = build_flower_guard(protected_positions);
            // 花ガードを配置
            for (P pos : flower_guard_positions)
            {
                field[pos.first][pos.second] = TREE;
                current_trent_positions.push_back(pos.first * N + pos.second);
            }
            flower_guard.tree_positions = flower_guard_positions;
        }
        // 囮に設置していく
        // 花ガードはすでに設置されている
        auto [success1, decoy_count_list1] = DecoySetter::set_decoys(left_path, current_trent_positions, field, initial_field);
        auto [success2, decoy_count_list2] = DecoySetter::set_decoys(right_path, current_trent_positions, field, initial_field);
        if (!success1 || !success2)
        {
            continue;
        }
        auto removed_trees = remover.remove_blocking_trees_of_flower_guard(flower_guard, field);
        for (auto pos : removed_trees)
        {
            current_trent_positions.erase(remove(current_trent_positions.begin(), current_trent_positions.end(), pos.first * N + pos.second), current_trent_positions.end());
            field[pos.first][pos.second] = EMPTY;
        }
        int run_num = run_num_coeff1 + (40. - (double)N) * run_num_coeff2; // Nが大きいほど試行回数を減らす

        // シミュレーションを実行してスコアを計算
        float st = get_time(false);
        double current_score = Evaluator::run_simulation_batch(run_num, flower_guard, field); // 時間短縮のため20回
        // double current_score = Evaluator::evaluate(decoy_count_list1, decoy_count_list2, left_path.size() + right_path.size());
        score_calc_times += get_time(false) - st;
        // bo.tell(cand, current_score);

        // ベストスコア更新チェック
        if (current_score > best_score)
        {
            best_score = current_score;
            best_trent_positions = current_trent_positions;
            best_flower_guard = flower_guard;
        }
    }

    if (best_trent_positions.size() == 0 && false)
    {
        auto flower_guard = FlowerGuard::build_from_field(field);
        for (auto pos : flower_guard.tree_positions)
        {
            field[pos.first][pos.second] = TREE;
            best_trent_positions.push_back(pos.first * N + pos.second);
        }

        // 花ガードを配置
        if (flower_guard.tree_positions.size() == 0)
        {
            set<P> protected_positions = {{ti, tj}};
            vector<P> flower_guard_positions = build_flower_guard(protected_positions);
            // 花ガードを配置
            for (P pos : flower_guard_positions)
            {
                field[pos.first][pos.second] = TREE;
                best_trent_positions.push_back(pos.first * N + pos.second);
            }
            flower_guard.tree_positions = flower_guard_positions;
        }
        best_flower_guard = flower_guard;
        vector<P> path = {{0, N / 2}};
        auto success1 = DecoySetter::set_decoys(path, best_trent_positions, field, initial_field);
    }

    cout << best_trent_positions.size() << " ";
    for (auto p : best_trent_positions)
        cout << p / N << " " << p % N << " ";
    cout << endl;

    return best_flower_guard;
}



void handle_input()
{
    cin >> N;
    b.resize(N);
    cin >> ti >> tj;
    rep(i, N) cin >> b[i];

    observed = vector<vector<bool>>(N, vector<bool>(N, false));
}

void handle_observations()
{
    cin >> px >> py;
    int n;
    cin >> n;
    rep(i, n)
    {
        int x, y;
        cin >> x >> y;
        observations.push_back({{x, y}});
        observed_positions.insert(x * N + y);
        observed[x][y] = true;
    }
}

int main(int argc, char *argv[])
{
#ifdef OPTUNA_STUDY
    cerr << "optuna study, override parameters" << endl;
    T0 = stod(argv[1]);
    T1 = stod(argv[2]);
#endif
    get_time(true);

    const char *a = getenv("a");
    const char *b = getenv("b");
    const char *c = getenv("c");
    const char *d = getenv("d");
    const char *e = getenv("e");
    const char *f = getenv("f");
    const char *g = getenv("g");
    const char *h = getenv("h");
    const char *i = getenv("i");
    const char *j = getenv("j");
    const char *k = getenv("k");
    if (a)
    {
        horizontal_or_vertical_threshold = stof(a);
    }
    if (b)
    {
        skip_even_runs = stoi(b) == 0;
    }
    if (c)
    {
        horizontal_upper_coeff1 = stod(c);
    }
    if (d)
    {
        horizontal_upper_coeff2 = stod(d);
    }
    if (e)
    {
        horizontal_lower_coeff = stod(e);
    }
    if (f)
    {
        vertical_upper_coeff1 = stod(f);
    }
    if (g)
    {
        vertical_upper_coeff2 = stod(g);
    }
    if (h)
    {
        vertical_lower_coeff = stod(h);
    }
    if (i)
    {
        score_calc_utikiri = stod(i);
    }
    if (j)
    {
        run_num_coeff1 = stod(j);
    }
    if (k)
    {
        run_num_coeff2 = stod(k);
    }

    handle_input();
    TIME_LIMIT = 1900 - (N - 20) * 14;
    handle_observations();

    auto game = Game();
    int turn = 0;

    auto flower_guard = game.construct_initial_trents();
    handle_observations();
    cerr << "initial_time:" << get_time(false) << endl;

    bool end_flag = false;

    while (true)
    {
        if (get_time(false) > 1980)
        {
            if (end_flag)
            {
                cout << -1 << endl;
                break;
            }
            else
            {
                end_flag = true;
                // flower_guardの片方を使ってしまう

                P trap_pos = {-1, -1};
                for (auto [k, v] : flower_guard.trap_positions)
                {
                    auto [x, y] = v;
                    if (!observed[x][y])
                    {
                        trap_pos = v;
                        break;
                    }
                }
                if (trap_pos.first == -1)
                {
                    cout << -1 << endl;
                    break;
                }
                else
                {
                    cerr << "observe trap pos: (" << trap_pos.first << "," << trap_pos.second << ")" << " " << observed[trap_pos.first][trap_pos.second] << endl;
                    cout << 1 << " " << trap_pos.first << " " << trap_pos.second << endl;
                }
                handle_observations();
                if (px == ti && py == tj)
                {
                    break;
                }
            }
        }
        else
        {
            auto checked = flower_guard.check_trap(px, py);
            if ((flower_guard.trap_used && checked.first == -1) || flower_guard.trap_positions.size() == 0)
            {
                cout << -1 << endl;
                break;
            }
            if (checked.first == -1)
            {
                cout << 0 << endl;
            }
            else
            {
                cout << 1 << " " << checked.first << " " << checked.second << endl;
            }

            handle_observations();
            if (px == ti && py == tj)
            {
                break;
            }
        }
        turn++;
    }
    cerr << "end_time:" << get_time(false) << endl;

    return 0;
}