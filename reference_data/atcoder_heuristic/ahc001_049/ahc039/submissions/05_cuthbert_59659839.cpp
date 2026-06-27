#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>  // accumulate を使用するため追加
#include <random>
#include <set>
#include <vector>

using namespace std;
using namespace chrono;

// HyperParameter
namespace hp {
#ifdef ONLINE_JUDGE
#define FROM_ENV(type, name, default_value) constexpr type name = default_value;
#else
template <class T>
T from_env(const char* name, T default_value) {
    // #ifdef ONLINE_JUDGE
    //     return default_value;
    // #else
    const char* value = std::getenv(name);
    if (value == nullptr) {
        return default_value;
    }
    std::stringstream ss(value);
    T res;
    ss >> res;
    return res;
    // #endif
}
#define FROM_ENV(type, name, default_value) const type name = from_env<type>(#name, default_value);
#endif
// simulated annealing
// FROM_ENV(double, temp_begin, 10.0);
// FROM_ENV(double, temp_end, 0.1);
// FROM_ENV(double, temp_linear_exp_ratio, 0.5);
// FROM_ENV(double, temp_offset_ratio, 0.2);
// FROM_ENV(double, prob1, 1.2);
// FROM_ENV(double, prob2, 3.3);
// FROM_ENV(double, prob3, 0.2);
FROM_ENV(double, prob4, 100);

FROM_ENV(double, temp_begin, 20.68203280431767);
FROM_ENV(double, temp_end, 0.010185428607385);
FROM_ENV(double, temp_linear_exp_ratio, 0.7696897349012286);
FROM_ENV(double, temp_offset_ratio, 0.840329655489515);
FROM_ENV(double, prob1, 10.3302954784311129);
FROM_ENV(double, prob2, 12.869357521542678);
FROM_ENV(double, prob3, 33.081811104014776);
#undef FROM_ENV
}  // namespace hp


// 座標を表す構造体
struct Point {
    int x, y;
};

// 多角形を表す構造体
struct Polygon {
    vector<Point> vertices;  // 頂点のリスト
    int score;               // スコア（サバ数 - イワシ数）
    int perimeter;           // 周囲長

    // コンストラクタ
    Polygon() : score(0), perimeter(0) {}
};

// グローバル変数
const int MAX_COORD = 100000;
const int PERIMETER_LIMIT = 400000;
const double TIME_LIMIT = 1.9;      // 秒
const int MAX_MOVE_DELTA = 100000;  // 辺の移動量の最大値

vector<Point> mackerels;  // サバの座標
vector<Point> sardines;   // イワシの座標

auto start_time = high_resolution_clock::now();

// 時間計測
double elapsed_time() {
    auto now = high_resolution_clock::now();
    return duration_cast<duration<double>>(now - start_time).count();
}

// 点が多角形内に含まれるか判定（射影法）
bool is_inside(const Polygon& poly, const Point& p) {
    int crossings = 0;
    int n = poly.vertices.size();
    for (int i = 0; i < n; ++i) {
        Point a = poly.vertices[i];
        Point b = poly.vertices[(i + 1) % n];
        if ((a.y > p.y) != (b.y > p.y)) {
            double atX = (double)(b.x - a.x) * (p.y - a.y) / (b.y - a.y + 1e-9) + a.x;
            if (p.x < atX)
                crossings++;
        }
    }
    return crossings % 2 != 0;
}

// 多角形のスコアを計算
int calculate_score(const Polygon& poly) {
    int score = 0;
    for (const auto& fish : mackerels) {
        if (is_inside(poly, fish)) {
            score += 1;
        }
    }
    for (const auto& fish : sardines) {
        if (is_inside(poly, fish)) {
            score -= 1;
        }
    }
    return score;
}

// 二次元累積和を用いた高速なスコア計算
// グリッドサイズの設定
const int GRID_SIZE = 150;  // 必要に応じて調整
int CELL_SIZE = (MAX_COORD + GRID_SIZE - 1) / GRID_SIZE;

const int GRID_SIZE2 = 3000;  // 必要に応じて調整
int CELL_SIZE2 = (MAX_COORD + GRID_SIZE2 - 1) / GRID_SIZE2;

vector<vector<int>> prefix_fish;
int calculate_score_fast(const Polygon& poly) {
    int score = 0;
    int n = poly.vertices.size();
    int area = 0;
    for (int i = 0; i < n; ++i) {
        Point a = poly.vertices[i];
        Point b = poly.vertices[(i + 1) % n];
        if (a.y != b.y)
            continue;
        int y = a.y;
        // cerr << a.x / CELL_SIZE << " " << b.x / CELL_SIZE << " " << y / CELL_SIZE << endl;
        // cerr << prefix_fish.size() << " " << prefix_fish[0].size() << endl;
        score += prefix_fish[b.x / CELL_SIZE2][y / CELL_SIZE2] - prefix_fish[a.x / CELL_SIZE2][y / CELL_SIZE2];
        area += (b.x - a.x) / CELL_SIZE2 * (y / CELL_SIZE2);
    }
    // cerr << "area: " << area << endl;
    if (area < 0) {
        score = -score;
    }
    // score = abs(score);
    return score;
}

// 周囲長を計算
int calculate_perimeter(const Polygon& poly) {
    int peri = 0;
    int n = poly.vertices.size();
    for (int i = 0; i < n; ++i) {
        Point a = poly.vertices[i];
        Point b = poly.vertices[(i + 1) % n];
        peri += abs(a.x - b.x) + abs(a.y - b.y);
    }
    return peri;
}

// 多角形が軸平行な辺のみで構成されているか確認
bool is_axis_aligned(const Polygon& poly) {
    int n = poly.vertices.size();
    for (int i = 0; i < n; ++i) {
        Point a = poly.vertices[i];
        Point b = poly.vertices[(i + 1) % n];
        if (a.x != b.x && a.y != b.y) {
            return false;
        }
    }
    return true;
}

// 多角形が自己交差していないか確認（簡易版）
bool is_simple(const Polygon& poly) {
    int n = poly.vertices.size();
    for (int i = 0; i < n; ++i) {
        Point a1 = poly.vertices[i];
        Point a2 = poly.vertices[(i + 1) % n];
        // if equal then continue
        if (a1.x == a2.x && a1.y == a2.y) {
            return false;
        }

        for (int j = i + 1; j < n; ++j) {
            Point b1 = poly.vertices[j];
            Point b2 = poly.vertices[(j + 1) % n];
            // // 隣接する辺は無視
            // if (i == j || (i + 1) % n == j || i == (j + 1) % n)
            //     continue;

            // 隣接する場合は同じ向きでないことを確認
            if (i == j || (i + 1) % n == j || i == (j + 1) % n) {
                if (a1.x == a2.x && b1.x == b2.x && a1.x == b1.x) {
                    return false;
                }
                if (a1.y == a2.y && b1.y == b2.y && a1.y == b1.y) {
                    return false;
                }
            }

            // 軸平行な辺のみなので、簡易な交差判定が可能
            if (a1.x == a2.x && b1.y == b2.y) {
                // aが垂直、bが水平
                if (min(a1.y, a2.y) < b1.y && b1.y < max(a1.y, a2.y) && min(b1.x, b2.x) < a1.x
                    && a1.x < max(b1.x, b2.x)) {
                    return false;
                }
            } else if (a1.y == a2.y && b1.x == b2.x) {
                // aが水平、bが垂直
                if (min(b1.y, b2.y) < a1.y && a1.y < max(b1.y, b2.y) && min(a1.x, a2.x) < b1.x
                    && b1.x < max(a1.x, a2.x)) {
                    return false;
                }
            } else if (a1.x == a2.x && b1.x == b2.x && a1.x == b1.x) {
                if (min(a1.y, a2.y) <= max(b1.y, b2.y) && min(b1.y, b2.y) <= max(a1.y, a2.y)) {
                    return false;
                }
            } else if (a1.y == a2.y && b1.y == b2.y && a1.y == b1.y) {
                if (min(a1.x, a2.x) <= max(b1.x, b2.x) && min(b1.x, b2.x) <= max(a1.x, a2.x)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool is_simple_fast(const Polygon& poly) {
    int n = poly.vertices.size();
    map<int, vector<pair<int, int>>> horizontal_edges;  // y座標 -> [(x1, x2)]
    map<int, vector<pair<int, int>>> vertical_edges;    // x座標 -> [(y1, y2)]

    for (int i = 0; i < n; ++i) {
        Point a = poly.vertices[i];
        Point b = poly.vertices[(i + 1) % n];

        if (a.y == b.y) {
            // 水平な辺
            int y = a.y;
            int x1 = min(a.x, b.x);
            int x2 = max(a.x, b.x);
            horizontal_edges[y].emplace_back(x1, x2);
        } else if (a.x == b.x) {
            // 垂直な辺
            int x = a.x;
            int y1 = min(a.y, b.y);
            int y2 = max(a.y, b.y);
            vertical_edges[x].emplace_back(y1, y2);
        } else {
            // 軸平行でない辺が存在する場合は無効
            return false;
        }
    }

    // 水平辺の自己交差チェック
    for (auto& [y, intervals] : horizontal_edges) {
        // 区間を始点でソート
        sort(intervals.begin(), intervals.end());
        int prev_x2 = intervals[0].second;
        for (size_t i = 1; i < intervals.size(); ++i) {
            int x1 = intervals[i].first;
            int x2 = intervals[i].second;
            if (x1 < prev_x2) {
                // 区間が重なっている場合、自己交差
                return false;
            }
            prev_x2 = x2;
        }
    }

    // 垂直辺の自己交差チェック
    for (auto& [x, intervals] : vertical_edges) {
        // 区間を始点でソート
        sort(intervals.begin(), intervals.end());
        int prev_y2 = intervals[0].second;
        for (size_t i = 1; i < intervals.size(); ++i) {
            int y1 = intervals[i].first;
            int y2 = intervals[i].second;
            if (y1 < prev_y2) {
                // 区間が重なっている場合、自己交差
                return false;
            }
            prev_y2 = y2;
        }
    }

    return true;
}

// 多角形が制約を満たしているか確認
template <bool FLAGFAST = false>
bool is_valid(const Polygon& poly) {
    if (poly.vertices.size() > 1000) {
        return false;
    }
    if (!is_axis_aligned(poly))
        return false;
    if constexpr (FLAGFAST) {
        if (!is_simple_fast(poly))
            return false;
    } else {
        if (!is_simple(poly))
            return false;
    }
    int peri = calculate_perimeter(poly);
    if (peri > PERIMETER_LIMIT)
        return false;
    // 頂点が範囲内にあるか確認
    for (const auto& p : poly.vertices) {
        if (p.x < 0 || p.x > MAX_COORD || p.y < 0 || p.y > MAX_COORD) {
            return false;
        }
    }
    return true;
}

// サバとイワシの数を格納するグリッド
vector<vector<int>> grid_mackerel;
vector<vector<int>> grid_sardine;
vector<vector<int>> grid2_mackerel;
vector<vector<int>> grid2_sardine;

// サバとイワシの累積和
vector<vector<int>> prefix_mackerel;
vector<vector<int>> prefix_sardine;

// 累積和の計算
void compute_prefix_sums() {
    prefix_mackerel.assign(GRID_SIZE + 1, vector<int>(GRID_SIZE + 1, 0));
    prefix_sardine.assign(GRID_SIZE + 1, vector<int>(GRID_SIZE + 1, 0));

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            prefix_mackerel[i + 1][j + 1]
                = prefix_mackerel[i + 1][j] + prefix_mackerel[i][j + 1] - prefix_mackerel[i][j] + grid_mackerel[i][j];
            prefix_sardine[i + 1][j + 1]
                = prefix_sardine[i + 1][j] + prefix_sardine[i][j + 1] - prefix_sardine[i][j] + grid_sardine[i][j];
        }
    }

    grid2_mackerel.assign(GRID_SIZE2 + 1, vector<int>(GRID_SIZE2 + 1, 0));
    grid2_sardine.assign(GRID_SIZE2 + 1, vector<int>(GRID_SIZE2 + 1, 0));
    for (auto& p : mackerels) {
        int gx = min(p.x / CELL_SIZE2, GRID_SIZE2 - 1);
        int gy = min(p.y / CELL_SIZE2, GRID_SIZE2 - 1);
        grid2_mackerel[gx][gy]++;
    }
    for (auto& p : sardines) {
        int gx = min(p.x / CELL_SIZE2, GRID_SIZE2 - 1);
        int gy = min(p.y / CELL_SIZE2, GRID_SIZE2 - 1);
        grid2_sardine[gx][gy]++;
    }

    prefix_fish.assign(GRID_SIZE2 + 1, vector<int>(GRID_SIZE2 + 1, 0));
    for (int i = 0; i < GRID_SIZE2; ++i) {
        for (int j = 0; j < GRID_SIZE2; ++j) {
            prefix_fish[i + 1][j + 1] = prefix_fish[i + 1][j] + prefix_fish[i][j + 1] - prefix_fish[i][j]
                                        + grid2_mackerel[i][j] - grid2_sardine[i][j];
        }
    }
}

// 長方形領域内のサバとイワシの数を取得
void get_counts(int x1, int y1, int x2, int y2, int& mackerel_count, int& sardine_count) {
    mackerel_count
        = prefix_mackerel[x2][y2] - prefix_mackerel[x1][y2] - prefix_mackerel[x2][y1] + prefix_mackerel[x1][y1];
    sardine_count = prefix_sardine[x2][y2] - prefix_sardine[x1][y2] - prefix_sardine[x2][y1] + prefix_sardine[x1][y1];
}

// スコアが最大となる長方形を探索
Polygon find_best_rectangle() {
    Polygon best_poly;
    int max_score = INT_MIN;

    int max_size = GRID_SIZE;  // 探索する長方形の最大サイズ（グリッド単位）

    for (int x1 = 0; x1 < GRID_SIZE; ++x1) {
        for (int y1 = 0; y1 < GRID_SIZE; ++y1) {
            for (int x2 = x1 + 1; x2 <= GRID_SIZE; ++x2) {
                for (int y2 = y1 + 1; y2 <= GRID_SIZE; ++y2) {
                    int mackerel_count, sardine_count;
                    get_counts(x1, y1, x2, y2, mackerel_count, sardine_count);
                    int score = mackerel_count - sardine_count;

                    // 周囲長の計算
                    int rect_perimeter = 2 * ((x2 - x1) * CELL_SIZE + (y2 - y1) * CELL_SIZE);

                    // 制約を満たすか確認
                    if (rect_perimeter > PERIMETER_LIMIT)
                        continue;

                    if (score > max_score) {
                        max_score = score;
                        best_poly.vertices = {{x1 * CELL_SIZE, y1 * CELL_SIZE}, {x2 * CELL_SIZE, y1 * CELL_SIZE},
                            {x2 * CELL_SIZE, y2 * CELL_SIZE}, {x1 * CELL_SIZE, y2 * CELL_SIZE}};
                        best_poly.score = score;
                        best_poly.perimeter = rect_perimeter;
                    }
                }
            }
        }
    }

    return best_poly;
}

Polygon generate_entire_rectangle() {
    Polygon best_poly;
    best_poly.vertices = {{0, 0}, {MAX_COORD, 0}, {MAX_COORD, MAX_COORD}, {0, MAX_COORD}};
    best_poly.score = calculate_score(best_poly);
    best_poly.perimeter = calculate_perimeter(best_poly);
    return best_poly;
}

// 初期解の生成（スコアが最も良い長方形）
Polygon generate_initial_solution() {
    // グリッドの初期化
    grid_mackerel.assign(GRID_SIZE, vector<int>(GRID_SIZE, 0));
    grid_sardine.assign(GRID_SIZE, vector<int>(GRID_SIZE, 0));

    // 魚の位置をグリッドにマッピング
    for (const auto& p : mackerels) {
        int gx = min(p.x / CELL_SIZE, GRID_SIZE - 1);
        int gy = min(p.y / CELL_SIZE, GRID_SIZE - 1);
        grid_mackerel[gx][gy]++;
    }
    for (const auto& p : sardines) {
        int gx = min(p.x / CELL_SIZE, GRID_SIZE - 1);
        int gy = min(p.y / CELL_SIZE, GRID_SIZE - 1);
        grid_sardine[gx][gy]++;
    }

    // 累積和の計算
    compute_prefix_sums();

    // スコアが最大となる長方形を探索
    Polygon best_poly = find_best_rectangle();

    // 制約を満たすか確認
    if (!is_valid(best_poly)) {
        // if (true) {
        // 制約を満たさない場合、全領域を囲む矩形を初期解とする
        best_poly.vertices = {{0, 0}, {MAX_COORD, 0}, {MAX_COORD, MAX_COORD}, {0, MAX_COORD}};
        best_poly.score = calculate_score(best_poly);
        best_poly.perimeter = calculate_perimeter(best_poly);
    }

    return best_poly;
}

// 近傍操作1: 頂点を削除し、代わりに3つの頂点を挿入
bool operation_add_corner(Polygon& poly, mt19937& gen) {
    int n = poly.vertices.size();
    if (n < 4)
        return false;
    uniform_int_distribution<> dis(1, n - 2);

    int idx = dis(gen);
    Point p = poly.vertices[idx];

    // 新しい頂点を生成
    uniform_int_distribution<> delta_dis(-MAX_MOVE_DELTA, MAX_MOVE_DELTA);
    int delta_x = delta_dis(gen);
    int delta_y = delta_dis(gen);
    // round delta_x to multiple of CELL_SIZE
    // delta_x = int(round(delta_x / CELL_SIZE)) * CELL_SIZE;
    // delta_y = int(round(delta_y / CELL_SIZE)) * CELL_SIZE;

    vector<Point> new_points = {{p.x, p.y + delta_y}, {p.x + delta_x, p.y + delta_y}, {p.x + delta_x, p.y}};

    // 範囲チェック
    for (const auto& np : new_points) {
        if (np.x < 0 || np.x > MAX_COORD || np.y < 0 || np.y > MAX_COORD) {
            return false;  // 範囲外なら操作を無効化
        }
    }

    // 頂点を置き換え
    poly.vertices.erase(poly.vertices.begin() + idx);
    poly.vertices.insert(poly.vertices.begin() + idx, new_points.begin(), new_points.end());
    return true;
}

// 近傍操作2: 連続する3つの頂点を削除し、1つの角を挿入
bool operation_remove_corner(Polygon& poly, mt19937& gen) {
    int n = poly.vertices.size();
    if (n < 6)
        return false;
    uniform_int_distribution<> dis(1, n - 4);

    int idx = dis(gen);

    // 3つの頂点を削除し、1つの頂点を挿入
    // Point p = poly.vertices[idx];
    Point p1 = poly.vertices[idx];
    Point p2 = poly.vertices[idx + 1];
    Point p3 = poly.vertices[idx + 2];
    Point p = {(p1.x + p3.x - p2.x), (p1.y + p3.y - p2.y)};

    poly.vertices.erase(poly.vertices.begin() + idx, poly.vertices.begin() + idx + 3);
    poly.vertices.insert(poly.vertices.begin() + idx, p);
    return true;
}

// 近傍操作3: 辺の内部にコの字型のくぼみまたはでっぱりを作る
bool operation_add_u_shape(Polygon& poly, mt19937& gen) {
    int n = poly.vertices.size();
    if (n < 4)
        return false;
    uniform_int_distribution<> dis(0, n - 1);

    int idx = dis(gen);
    Point a = poly.vertices[idx];
    Point b = poly.vertices[(idx + 1) % n];

    uniform_int_distribution<> delta_dis(-MAX_MOVE_DELTA, MAX_MOVE_DELTA);

    if (a.x == b.x) {
        // 垂直な辺
        int mid_y = (a.y + b.y) / 2;
        int delta_x = delta_dis(gen);
        // delta_x = int(round(delta_x / CELL_SIZE)) * CELL_SIZE;

        vector<Point> new_points = {{a.x, mid_y}, {a.x + delta_x, mid_y}, {a.x + delta_x, mid_y + (b.y - a.y) / 2}};

        // 範囲チェック
        for (const auto& np : new_points) {
            if (np.x < 0 || np.x > MAX_COORD || np.y < 0 || np.y > MAX_COORD) {
                // return;  // 範囲外なら操作を無効化
                return false;
            }
        }

        poly.vertices.insert(poly.vertices.begin() + idx + 1, new_points.begin(), new_points.end());
    } else if (a.y == b.y) {
        // 水平な辺
        int mid_x = (a.x + b.x) / 2;
        int delta_y = delta_dis(gen);
        // delta_y = int(round(delta_y / CELL_SIZE)) * CELL_SIZE;

        vector<Point> new_points = {{mid_x, a.y}, {mid_x, a.y + delta_y}, {mid_x + (b.x - a.x) / 2, a.y + delta_y}};

        // 範囲チェック
        for (const auto& np : new_points) {
            if (np.x < 0 || np.x > MAX_COORD || np.y < 0 || np.y > MAX_COORD) {
                // return;  // 範囲外なら操作を無効化
                return false;
            }
        }

        poly.vertices.insert(poly.vertices.begin() + idx + 1, new_points.begin(), new_points.end());
    }
    return true;
}

// 近傍操作4: 辺を平行移動する
bool operation_move_edge(Polygon& poly, mt19937& gen) {
    int n = poly.vertices.size();
    if (n < 4)
        return false;
    uniform_int_distribution<> dis(0, n - 1);
    int idx = dis(gen);
    Point a = poly.vertices[idx];
    Point b = poly.vertices[(idx + 1) % n];

    uniform_int_distribution<> delta_dis(-MAX_MOVE_DELTA, MAX_MOVE_DELTA);
    int delta = delta_dis(gen);
    // delta = int(round(delta / CELL_SIZE)) * CELL_SIZE;

    // 移動後の座標を計算
    Point new_a = a;
    Point new_b = b;

    if (a.x == b.x) {
        // 垂直な辺を水平方向に移動
        new_a.x += delta;
        new_b.x += delta;
    } else if (a.y == b.y) {
        // 水平な辺を垂直方向に移動
        new_a.y += delta;
        new_b.y += delta;
    } else {
        return false;  // 軸平行でない辺は無視
    }

    // 範囲チェック
    if (new_a.x < 0 || new_a.x > MAX_COORD || new_a.y < 0 || new_a.y > MAX_COORD || new_b.x < 0 || new_b.x > MAX_COORD
        || new_b.y < 0 || new_b.y > MAX_COORD) {
        // return;  // 範囲外なら操作を無効化
        return false;
    }

    // 頂点を更新
    poly.vertices[idx] = new_a;
    poly.vertices[(idx + 1) % n] = new_b;
    return true;
}

// 近傍操作5: 頂点を移動する
bool operation_move_vertex(Polygon& poly, mt19937& gen) {
    int n = poly.vertices.size();
    if (n < 3)
        return false;
    uniform_int_distribution<> dis(0, n - 1);
    int idx = dis(gen);

    uniform_int_distribution<> delta_dis(-MAX_MOVE_DELTA, MAX_MOVE_DELTA);
    int delta_x = delta_dis(gen);
    int delta_y = delta_dis(gen);

    Point new_p = poly.vertices[idx];
    new_p.x += delta_x;
    new_p.y += delta_y;

    // 範囲チェック
    if (new_p.x < 0 || new_p.x > MAX_COORD || new_p.y < 0 || new_p.y > MAX_COORD) {
        return false;
    }

    // 更新
    poly.vertices[idx] = new_p;
    return true;
}

vector<double> op_probabilities = {0.25, 0.25, 0.25, 0.25};  // 操作1～5の選択確率の初期値

// 近傍操作を適用
bool apply_operation(Polygon& poly, mt19937& gen) {
    // 累積確率を計算
    vector<double> cumulative_probs(op_probabilities.size());
    cumulative_probs[0] = op_probabilities[0];
    for (size_t i = 1; i < op_probabilities.size(); ++i) {
        cumulative_probs[i] = cumulative_probs[i - 1] + op_probabilities[i];
    }

    // 0から1の間の乱数を生成
    uniform_real_distribution<> dis(0.0, 1.0);
    double r = dis(gen);

    // 操作を選択
    int op = 0;
    while (op < cumulative_probs.size() - 1 && r > cumulative_probs[op]) {
        ++op;
    }

    switch (op) {
    case 0:
        return operation_add_corner(poly, gen);
        break;
    case 1:
        return operation_remove_corner(poly, gen);
        break;
    case 2:
        return operation_add_u_shape(poly, gen);
        break;
    case 3:
        return operation_move_edge(poly, gen);
        break;
    // case 4:
    //     return operation_move_vertex(poly, gen);
    //     break;
    default:
        break;
    }
}

// 不要な頂点を削除（連続する3点が一直線上の場合）
void simplify_polygon(Polygon& poly) {
    vector<Point> new_vertices;
    int n = poly.vertices.size();
    for (int i = 0; i < n; ++i) {
        Point prev = poly.vertices[(i - 1 + n) % n];
        Point curr = poly.vertices[i];
        Point next = poly.vertices[(i + 1) % n];
        if (!((prev.x == curr.x && curr.x == next.x) || (prev.y == curr.y && curr.y == next.y))) {
            new_vertices.push_back(curr);
        }
    }
    poly.vertices = new_vertices;
}


double calc_temp(const double time_ratio) {
    const double temp_offset = hp::temp_offset_ratio * hp::temp_end;
    const double temp_begin_without_offset = hp::temp_begin - temp_offset;
    const double temp_end_without_offset = hp::temp_end - temp_offset;
    const double temp_linear
        = temp_begin_without_offset + (temp_end_without_offset - temp_begin_without_offset) * time_ratio;
    const double temp_exp
        = temp_begin_without_offset * pow(temp_end_without_offset / temp_begin_without_offset, time_ratio);
    const double temp
        = temp_linear * hp::temp_linear_exp_ratio + temp_exp * (1.0 - hp::temp_linear_exp_ratio) + temp_offset;
    return temp;
}

// 焼きなまし法による最適化
Polygon simulated_annealing() {
    Polygon current_solution = generate_initial_solution();
    Polygon best_solution = current_solution;

    // double temperature = 10000.0;
    // double cooling_rate = 0.9998;
    // double temperature_begin = 10.0;
    // double temperature_end = 0.1;
    double temperature = calc_temp(0.0);
    // double cooling_rate = 0.997;

    random_device rd;
    mt19937 gen(rd());

    bool found_valid_polygon = false;  // 有効な多角形が見つかったか

    int iter = 0;

    // while (elapsed_time() < TIME_LIMIT) {
    while (true) {
        if (iter % 100 == 0) {
            // temperature = temperature_begin + (temperature_end - temperature_begin) * (elapsed_time() / TIME_LIMIT);
            double time_ratio = elapsed_time() / TIME_LIMIT;
            if (time_ratio > 1.0)
                break;
            temperature = calc_temp(time_ratio);
        }

        iter++;
        Polygon new_solution = current_solution;

        // 近傍操作を適用
        bool flag_apply = apply_operation(new_solution, gen);
        if (!flag_apply)
            continue;

        // 多角形を簡略化
        simplify_polygon(new_solution);

        // 制約を満たすか確認
        if (!is_valid<false>(new_solution)) {
            continue;
        }

        found_valid_polygon = true;  // 有効な多角形が見つかった

        // スコアを計算
        // new_solution.score = calculate_score(new_solution);
        new_solution.score = calculate_score_fast(new_solution);
        new_solution.perimeter = calculate_perimeter(new_solution);

        const int new_solution_score_fast = calculate_score_fast(new_solution);

        // スコアの差
        int delta_score = new_solution.score - current_solution.score;

        // ログの出力
        if (iter % 1000 == 0) {
            cerr << "Iter: " << iter << " Score: " << new_solution.score << " Perimeter: " << new_solution.perimeter
                 << " Temp: " << temperature << endl;
        }
        // cerr << new_solution.score << " " << new_solution_score_fast << endl;

        // 焼きなましの確率で解を更新
        if (delta_score >= 0 || exp(delta_score / temperature) > ((double)gen() / gen.max())) {
            current_solution = new_solution;
            if (current_solution.score > best_solution.score) {
                best_solution = current_solution;
            }
        }

        // 温度の更新
        // temperature *= cooling_rate;
    }

    int score_true = calculate_score(best_solution);
    int score_fast = calculate_score_fast(best_solution);
    cerr << "true score: " << score_true << " fast score: " << score_fast << endl;


    if (!found_valid_polygon) {
        // 有効な多角形が見つからなかった場合、最もスコアが高い長方形を返す
        best_solution = generate_initial_solution();
    }

    return best_solution;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // 入力
    int N;
    cin >> N;
    mackerels.resize(N);
    sardines.resize(N);

    for (int i = 0; i < N; ++i) {
        cin >> mackerels[i].x >> mackerels[i].y;
    }
    for (int i = 0; i < N; ++i) {
        cin >> sardines[i].x >> sardines[i].y;
    }

    op_probabilities = {hp::prob1, hp::prob2, hp::prob3, hp::prob4};

    // 確率の合計が1になるように正規化
    double total_prob = accumulate(op_probabilities.begin(), op_probabilities.end(), 0.0);
    for (auto& prob : op_probabilities) {
        prob /= total_prob;
    }

    // 焼きなまし法による最適化
    Polygon best_polygon = simulated_annealing();

    // 出力
    if (!is_valid(best_polygon)) {
        // if (true) {
        // 有効な多角形が見つからなかった場合、最もスコアが高い長方形を出力
        // best_polygon = generate_initial_solution();
        best_polygon = generate_entire_rectangle();
        cout << best_polygon.vertices.size() << endl;
        for (const auto& p : best_polygon.vertices) {
            cout << p.x << " " << p.y << endl;
        }
    } else {
        cout << best_polygon.vertices.size() << endl;
        for (const auto& p : best_polygon.vertices) {
            cout << p.x << " " << p.y << endl;
        }
    }

    int score = calculate_score(best_polygon);
    cerr << "[DATA] score = " << score << endl;

    return 0;
}

