
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
#include <climits>
#include <bitset>
#include <time.h>
#include <assert.h>

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
int A[5][5];

// LRUDの順番
vec_int dx = {0, 0, -1, 1};
vec_int dy = {-1, 1, 0, 0};

// #pragma GCC optimize("Ofast")


std::mt19937 mt{12345};
int RAND_ARR_LEN = 100000;
int RAND_RANGE = 1000000000;
float TIME_LIMIT = 3000;
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
    PseudoSet(){};
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

double current_tmp(double current_time, double T0, double T1, double time_ratio)
{
    return T1 + (T0 - T1) * (TIME_LIMIT * time_ratio - current_time) / (TIME_LIMIT * time_ratio);
    // double start_time = TIME_LIMIT * 0.6;
    // double x = (current_time - start_time) / (TIME_LIMIT * 0.32);
    // return pow(T1, 1 - x) + (T0, x);
}

bool is_valid_transition(int diff, double current_time, double T0, float T1, double time_ratio)
{
    float t = current_tmp(current_time, T0, T1, time_ratio);
    float rand = ro.get_float();
    // diffが正だったら常にアクセぷと
    return rand < exp(((float)diff) / t);
}

namespace atcoder
{

    namespace internal
    {

        // @param n `0 <= n`
        // @return minimum non-negative `x` s.t. `n <= 2**x`
        int ceil_pow2(int n)
        {
            int x = 0;
            while ((1U << x) < (unsigned int)(n))
                x++;
            return x;
        }

        // @param n `1 <= n`
        // @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
        constexpr int bsf_constexpr(unsigned int n)
        {
            int x = 0;
            while (!(n & (1 << x)))
                x++;
            return x;
        }

        // @param n `1 <= n`
        // @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
        int bsf(unsigned int n)
        {
            return __builtin_ctz(n);
        }

    } // namespace internal

} // namespace atcoder

namespace atcoder
{

    template <class S, S (*op)(S, S), S (*e)()>
    struct segtree
    {
    public:
        segtree() : segtree(0) {}
        explicit segtree(int n) : segtree(std::vector<S>(n, e())) {}
        explicit segtree(const std::vector<S> &v) : _n(int(v.size()))
        {
            log = internal::ceil_pow2(_n);
            size = 1 << log;
            d = std::vector<S>(2 * size, e());
            for (int i = 0; i < _n; i++)
                d[size + i] = v[i];
            for (int i = size - 1; i >= 1; i--)
            {
                update(i);
            }
        }

        void set(int p, S x)
        {
            assert(0 <= p && p < _n);
            p += size;
            d[p] = x;
            for (int i = 1; i <= log; i++)
                update(p >> i);
        }

        S get(int p) const
        {
            assert(0 <= p && p < _n);
            return d[p + size];
        }

        S prod(int l, int r) const
        {
            assert(0 <= l && l <= r && r <= _n);
            S sml = e(), smr = e();
            l += size;
            r += size;

            while (l < r)
            {
                if (l & 1)
                    sml = op(sml, d[l++]);
                if (r & 1)
                    smr = op(d[--r], smr);
                l >>= 1;
                r >>= 1;
            }
            return op(sml, smr);
        }

        S all_prod() const { return d[1]; }

        template <bool (*f)(S)>
        int max_right(int l) const
        {
            return max_right(l, [](S x)
                             { return f(x); });
        }
        template <class F>
        int max_right(int l, F f) const
        {
            assert(0 <= l && l <= _n);
            assert(f(e()));
            if (l == _n)
                return _n;
            l += size;
            S sm = e();
            do
            {
                while (l % 2 == 0)
                    l >>= 1;
                if (!f(op(sm, d[l])))
                {
                    while (l < size)
                    {
                        l = (2 * l);
                        if (f(op(sm, d[l])))
                        {
                            sm = op(sm, d[l]);
                            l++;
                        }
                    }
                    return l - size;
                }
                sm = op(sm, d[l]);
                l++;
            } while ((l & -l) != l);
            return _n;
        }

        template <bool (*f)(S)>
        int min_left(int r) const
        {
            return min_left(r, [](S x)
                            { return f(x); });
        }
        template <class F>
        int min_left(int r, F f) const
        {
            assert(0 <= r && r <= _n);
            assert(f(e()));
            if (r == 0)
                return 0;
            r += size;
            S sm = e();
            do
            {
                r--;
                while (r > 1 && (r % 2))
                    r >>= 1;
                if (!f(op(d[r], sm)))
                {
                    while (r < size)
                    {
                        r = (2 * r + 1);
                        if (f(op(d[r], sm)))
                        {
                            sm = op(d[r], sm);
                            r--;
                        }
                    }
                    return r + 1 - size;
                }
                sm = op(d[r], sm);
            } while ((r & -r) != r);
            return 0;
        }

    private:
        int _n, size, log;
        std::vector<S> d;

        void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
    };

} // namespace atcoder




class State
{
private:
public:
    bool emergency = false;
    int empty_slot_count = 6;
    State();
    vec_int arr;
    double calc_score(bool verbose);
    void partial_sort();
    pair<vector<P>, double> simulate();
};

State::State()
{
    arr.resize(N * N);
    rep(i, N * N) arr[i] = i;
    shuffle(arr.begin(), arr.end(), mt);
    partial_sort();
}

void State::partial_sort()
{
    vec_int ind_arr = vec_int(25);
    rep(i, 25) ind_arr[arr[i]] = i;

    rep(i, 5)
    {
        vec_int tmp;
        rep(j, 5)
            tmp.push_back(ind_arr[i * 5 + j]);
        sort(tmp.begin(), tmp.end());
        rep(j, 5)
            ind_arr[i * 5 + j] = tmp[j];
    }
    rep(i, 25) arr[ind_arr[i]] = i;
}

double State::calc_score(bool verbose)
{
    // arrの順番でAから値を取り出して行った時に、どれだけ途中で溜まるか
    int ans = 0;

    vector<bool> on_board(N * N, false);
    vector<bool> removed(N * N, false);
    int count = 0;
    vec_int A_pos = vec_int(N * N);
    rep(i, 5) rep(j, 5) A_pos[A[i][j]] = i * 5 + j;
    vec_int row_progress = vec_int(N, -1);

    // もうちょっと簡単なシミュレーション方針がないか
    // 数値numを取り出す時には
    // 1. すでに取り除かれていたらcontinue;
    // 2. すでにボード上にあったら取り除いてcountを減らす(その子分も減らす)
    // 3. 埋まっていたらそこまで掘る、掘ったやつは途中に置いておく, countを増やす
    double average_count = 0;
    rep(i, N * N)
    {
        average_count += count;
        int num = arr[i];
        if (removed[num])
            continue;
        if (on_board[num])
        {
            removed[num] = true;
            count--;
            if (verbose)
                cerr << num << " is removed! on the board:" << count << endl;

            int tmp = num % 5;
            for (int j = 1; j <= 5 - tmp - 1; j++)
            {
                if (!on_board[num + j])
                    break;
                removed[num + j] = true;
                count--;

                if (verbose)
                    cerr << num + j << " is removed! on the board:" << count << endl;
            }
            continue;
        }
        // ボード上にない時、そこまで掘る
        int x = A_pos[num] / 5;
        int y = A_pos[num] % 5;
        int diff = y - row_progress[x];
        count += diff - 1;
        for (int j = row_progress[x] + 1; j <= y; j++)
        {
            // 直接捨てられる時には直接捨てる
            /*
            if (A[x][j] % 5 == 0 || removed[A[x][j] - 1])
            {
                removed[A[x][j]] = true;
                count--;
                continue;
            }
            */

            on_board[A[x][j]] = true;
        }
        row_progress[x] = y;
        ans = max(ans, count);
        removed[num] = true;
        int tmp = num % 5;
        if (verbose)
            cerr << num << " is removed! on the board:" << count << endl;
        for (int j = 1; j <= 5 - tmp - 1; j++)
        {
            if (!on_board[num + j])
                break;
            removed[num + j] = true;
            count--;
            if (verbose)
                cerr << num + j << " is removed! on the board:" << count << endl;
        }
    }
    return ans + average_count / 25. / 10.;
}

// arrの優先順で取り出して行った時に各数値をどこにどの順番で動かすかをシミュレーションする
pair<vector<P>, double> State::simulate()
{

    vec_int aaa = {0, 1, 2, 3, 4, 5};
    vec_int empty_slots = vec_int{2, 3, 12, 13, 22, 23};
    vector<int> occupied_number(6, -1);

    if (emergency && empty_slot_count == 7)
    {
        empty_slots = vec_int{2, 3, 7, 8, 17, 18, 22};
        occupied_number = vec_int(7, -1);
        aaa = {0, 1, 2, 3, 4, 5, 6};
    }
    else if (emergency && empty_slot_count == 8)
    {
        empty_slots = vec_int{2, 3, 7, 8, 17, 18, 22, 23};
        occupied_number = vec_int(8, -1);
        aaa = {0, 1, 2, 3, 4, 5, 6, 7};
    }
    shuffle(aaa.begin(), aaa.end(), mt);

    // arrの順番でAから値を取り出して行った時に、どれだけ途中で溜まるか
    vector<P> result;
    int ans = 0;

    vector<bool> on_board(N * N, false);
    vector<bool> removed(N * N, false);
    int count = 0;
    vec_int A_pos = vec_int(N * N);
    rep(i, 5) rep(j, 5) A_pos[A[i][j]] = i * 5 + j;
    vec_int row_progress = vec_int(N, -1);
    double total_distance = 0;

    // もうちょっと簡単なシミュレーション方針がないか
    // 数値numを取り出す時には
    // 1. すでに取り除かれていたらcontinue;
    // 2. すでにボード上にあったら取り除いてcountを減らす(その子分も減らす)
    // 3. 埋まっていたらそこまで掘る、掘ったやつは途中に置いておく, countを増やす
    rep(i, N * N)
    {
        int num = arr[i];
        if (removed[num])
            continue;
        if (on_board[num])
        {
            removed[num] = true;
            result.push_back({num, (num / 5) * 5 + 4});
            count--;
            rep(k, empty_slot_count)
            {
                if (occupied_number[k] == num)
                {
                    occupied_number[k] = -1;
                    total_distance += abs(empty_slots[k] / 5 - (num / 5) * 5 + 4) + abs(4 - empty_slots[k] % 5) + 2;
                    break;
                }
            }

            int tmp = num % 5;
            for (int j = 1; j <= 5 - tmp - 1; j++)
            {
                if (!on_board[num + j])
                    break;
                removed[num + j] = true;
                result.push_back({num + j, (num / 5) * 5 + 4});
                count--;
                rep(k, empty_slot_count)
                {
                    if (occupied_number[k] == num + j)
                    {
                        occupied_number[k] = -1;
                        total_distance += abs(empty_slots[k] / 5 - (num / 5) * 5 + 4) + abs(4 - empty_slots[k] % 5) + 2;
                        break;
                    }
                }
            }
            continue;
        }
        // ボード上にない時、そこまで掘る
        int x = A_pos[num] / 5;
        int y = A_pos[num] % 5;
        total_distance += abs(num / N - x) + abs(4 - 0) + 2;
        int diff = y - row_progress[x];
        count += diff - 1;
        for (int j = row_progress[x] + 1; j < y; j++)
        {
            // 直接捨てられる時には直接捨てる
            if (A[x][j] % 5 == 0 || removed[A[x][j] - 1])
            {

                total_distance += abs(A[x][j] / N - x) + abs(4 - 0) + 2;
                result.push_back({A[x][j], (A[x][j] / 5) * 5 + 4});
                removed[A[x][j]] = true;
                count--;

                // 後続のものが捨てられる場合には捨てる
                int num2 = A[x][j];
                int tmp = num2 % 5;
                for (int l = 1; l <= 5 - tmp - 1; l++)
                {
                    if (!on_board[num2 + l])
                        break;
                    removed[num2 + l] = true;
                    result.push_back({num2 + l, (num2 / 5) * 5 + 4});
                    count--;
                    rep(k, empty_slot_count)
                    {
                        if (occupied_number[k] == num2 + l)
                        {
                            occupied_number[k] = -1;
                            total_distance += abs(empty_slots[k] / 5 - (num2 / 5) * 5 + 4) + abs(4 - empty_slots[k] % 5) + 2;
                            break;
                        }
                    }
                }

                continue;
            }

            on_board[A[x][j]] = true;
            // 最も近いempty slotにおく
            int best_distance = INT_MAX;
            int target = -1;
            vec_int candidates;
            rep(kk, empty_slot_count)
            {
                int k = aaa[kk];
                if (occupied_number[k] != -1)
                    continue;
                candidates.push_back(k);
                int distance = abs(empty_slots[k] / N - x) + abs(empty_slots[k] % N);
                // int goal_row = A[x][j] / N;
                /// distance += abs(goal_row - empty_slots[k] / N) + abs(4 - empty_slots[k] % N);
                if (distance < best_distance)
                {
                    best_distance = distance;
                    target = k;
                }
            }

            if (target == -1)
            {
                throw runtime_error("no empty slot");
            }
            occupied_number[target] = A[x][j];
            result.push_back({A[x][j], empty_slots[target]});
            total_distance += best_distance + 2;
        }
        row_progress[x] = y;
        ans = max(ans, count);
        removed[num] = true;
        int destination = (num / 5) * 5 + 4;
        result.push_back({num, destination});
        int tmp = num % 5;
        for (int j = 1; j <= 5 - tmp - 1; j++)
        {
            if (!on_board[num + j])
                break;
            removed[num + j] = true;
            result.push_back({num + j, destination});
            count--;
            // occupied numberを解放する
            rep(k, empty_slot_count)
            {
                if (occupied_number[k] == num + j)
                {
                    occupied_number[k] = -1;
                    break;
                }
            }
        }
    }
    return {result, total_distance};
}

class Annealer
{
public:
    Annealer(){};
    vector<P> annealing();
};

vector<P> Annealer::annealing()
{
    State state = State();
    double score = state.calc_score(false);

    double T0 = 0.4;
    double T1 = 0.0;

    double best_score = score;
    State best_state = state;
    int count = 0;
    float start_time = get_time(false);
    double threshold = 6.9;
    while (true)
    {
        float ct = get_time(false);
        if (ct > TIME_LIMIT * 0.003 + start_time)
            break;

        if (ct > TIME_LIMIT * 0.002 + start_time)
        {
            state.emergency = true;
            state.empty_slot_count = ro.get_rand(0, 2) == 0 ? 7 : 8;
            threshold = state.empty_slot_count == 7 ? 7.9 : 8.9;
        }

        vec_int old_arr = state.arr;
        vec_int new_arr = state.arr;
        int ind1 = ro.get_rand(0, N * N);
        int ind2 = ro.get_rand(0, N * N - 1);
        if (ind2 >= ind1)
            ind2++;
        swap(new_arr[ind1], new_arr[ind2]);
        state.arr = new_arr;

        state.partial_sort();

        double new_score = state.calc_score(false);
        double diff = new_score - score;
        if (diff <= 0 || is_valid_transition(-diff, ct - start_time, T0, T1, 0.003))
        {
            if (new_score < best_score)
            {
                best_score = new_score;
                best_state = state;
            }
            score = new_score;
            if (score < threshold)
                break;
        }
        else
        {
            state.arr = old_arr;
        }
        count++;
    }
    // if (best_score > threshold)
    //    return {};
    try
    {
        return best_state.simulate().first;
    }
    catch (...)
    {
        try
        {
            best_state.emergency = true;
            best_state.empty_slot_count = 7;
            return best_state.simulate().first;
        }
        catch (...)
        {

            best_state.emergency = true;
            best_state.empty_slot_count = 8;
            return best_state.simulate().first;
        }
    }
}




// 個別のクレーンに割り当てられた抽象的な目標
// 特定の番号のものを特定の場所に運ぶ
class AssignedGoal
{
public:
    AssignedGoal(int target_number, int goal_x, int goal_y) : target_number(target_number), goal_x(goal_x), goal_y(goal_y){};
    int target_number;
    int goal_x;
    int goal_y;
    P next_pos(int current_x, int current_y, char command);
};

// 個別のクレーンに割り当てられたタスク列
class AssignedTask
{
public:
    AssignedTask(int start_turn, vector<char> commands) : start_turn(start_turn), commands(commands){};
    int start_turn;
    vector<char> commands;
};

enum class TaskType
{
    PICKUP,
    DROP,
};
string task_type_to_string(TaskType task_type)
{
    if (task_type == TaskType::PICKUP)
        return "PICKUP";
    if (task_type == TaskType::DROP)
        return "DROP";
    return "INVALID";
}

struct Task
{
    int task_id;
    TaskType task_type;
};

class TaskDependencyManager
{
private:
    vector<P> simulation_result;
    void _construct_dependency();

public:
    vec_int container_pickup_turns;
    vec_int container_drop_turns;
    vector<vector<Task>> pickup_dependency;
    vector<vector<Task>> drop_dependency;
    TaskDependencyManager(vector<P> sim_result);
    void add_dependency(int task_id, int turn, int pickup_turns, int drop_turns);
    bool can_execute_task(int task_id, int turn, int pickup_turn, int drop_turn);
};

TaskDependencyManager::TaskDependencyManager(vector<P> sim_result)
{
    simulation_result = sim_result;
    container_pickup_turns = vec_int(sim_result.size(), INT_MAX);
    container_drop_turns = vec_int(sim_result.size(), INT_MAX);
    pickup_dependency = vector<vector<Task>>(sim_result.size(), vector<Task>()); // pickupをするまでに終わっていなければいけないタスク
    drop_dependency = vector<vector<Task>>(sim_result.size(), vector<Task>());   // dropをするまでに終わっていなければいけないタスク
    _construct_dependency();
}

void TaskDependencyManager::_construct_dependency()
{
    vector<P> A_inv = vector<P>(N * N, P(-1, -1));
    rep(i, N) rep(j, N) A_inv[A[i][j]] = P(i, j);

    vector<vec_int> container_task_ids(N * N);
    for (int i = 0; i < simulation_result.size(); i++)
    {
        int target_number = simulation_result[i].first;
        container_task_ids[target_number].push_back(i);
    }

    // タスクの依存関係には以下のものがある
    // 1.箱の運び出しをするために一個前のやつが運び出されることが終わっていないといけない
    // 2. 箱を置くためには一個前の数値のやつが終わっていないといけない
    // 3. 中間地点に置くためには一個前の箱のやつが運び出されていないといけない

    // 1.箱の運び出しをするために一個前のやつが運び出されることが終わっていないといけない
    for (int row = 0; row < 5; row++)
    {
        for (int col = 1; col < 5; col++)
        {
            int target_number = A[row][col];
            int prev_number = A[row][col - 1];
            pickup_dependency[container_task_ids[target_number][0]].push_back({container_task_ids[prev_number][0], TaskType::PICKUP});
        }
    }

    // 2. 箱を置くためには一個前の数値のやつが終わっていないといけない
    for (int row = 0; row < 5; row++)
    {
        for (int col = 1; col < 5; col++)
        {
            int target_number = row * 5 + col;
            int prev_number = row * 5 + col - 1;
            int task_id = container_task_ids[target_number][container_task_ids[target_number].size() - 1];
            int task_id2 = container_task_ids[prev_number][container_task_ids[prev_number].size() - 1];
            drop_dependency[task_id].push_back({task_id2, TaskType::DROP});
        }
    }

    // 3. 中間地点に置くためには一個前の箱のやつが運び出されていないといけない
    for (int task_id = 0; task_id < simulation_result.size(); task_id++)
    {
        int destination = simulation_result[task_id].second;
        for (int task2 = 0; task2 < task_id; task2++)
        {
            if (simulation_result[task2].second == destination)
            {
                drop_dependency[task_id].push_back({task2, TaskType::DROP});
                int num = simulation_result[task2].first;
                for (int task3 = task2 + 1; task3 < task_id; task3++)
                {
                    if (simulation_result[task3].first == num)
                    {
                        drop_dependency[task_id].push_back({task3, TaskType::PICKUP});
                        break;
                    }
                }
            }
        }
    }
}

void TaskDependencyManager::add_dependency(int task_id, int turn, int pickup_turns, int drop_turns)
{
    container_pickup_turns[task_id] = turn + pickup_turns;
    container_drop_turns[task_id] = turn + drop_turns;
}

bool TaskDependencyManager::can_execute_task(int task_id, int turn, int pickup_turn, int drop_turn)
{
    int target_cargo = simulation_result[task_id].first;
    for (int i = 0; i < task_id; i++)
    {
        if (simulation_result[i].first == target_cargo && container_drop_turns[i] > turn + pickup_turn)
            return false;
    }

    for (auto [dependent_task_id, task_type] : pickup_dependency[task_id])
    {
        if (task_type == TaskType::PICKUP && container_pickup_turns[dependent_task_id] > turn + pickup_turn)
            return false;
        if (task_type == TaskType::DROP && container_drop_turns[dependent_task_id] > turn + pickup_turn)
            return false;
    }
    for (auto [dependent_task_id, task_type] : drop_dependency[task_id])
    {
        if (task_type == TaskType::PICKUP && container_pickup_turns[dependent_task_id] > turn + drop_turn)
            return false;
        if (task_type == TaskType::DROP && container_drop_turns[dependent_task_id] > turn + drop_turn)
            return false;
    }
    return true;
}

class DijkstraSolver
{
private:
    bool can_move(int current_x, int current_y, char command, int turn, bool have_cargo, int crane_id);
    char inverse_command(char command);

public:
    static int command_index(char command);
    static char command_from_index(int index);
    static char infer_command(int current_pos, int prev_pos);
    static P next_pos(int current_x, int current_y, char command);

    vector<vector<vector<int>>> prohibited_cells;
    vector<vector<vector<P>>> visited;
    vector<vector<vector<vector<bool>>>> prohibited_moves;
    int counter = 0;
    DijkstraSolver()
    {
        prohibited_cells = vector<vector<vector<int>>>(5, vector<vector<int>>(5, vector<int>(200, 0)));                                                 // 荷物置き場には荷物を持った状態で入らないようにする。そうするとフリーに動けるはず
        prohibited_moves = vector<vector<vector<vector<bool>>>>(5, vector<vector<vector<bool>>>(5, vector<vector<bool>>(4, vector<bool>(200, false)))); // LRUDの順番
        visited = vector<vector<vector<P>>>(5, vector<vector<P>>(5, vector<P>(200, P(-1, -1))));                                                        // visitedの記録をする配列を使い回す
        rep(i, 5) prohibited_cells[i][0][0] = 1;
        rep(turn, 150)
        {
            rep(row, 5)
            {
                prohibited_cells[row][0][turn + 1] = 2; // 少なくともここは2になっている
            }
        }
    };
    vector<char> find_route(int start_turn, int from_x, int from_y, int to_x, int to_y, int crane_id, bool have_cargo);
    void set_prohibited(vector<char> &commands, int from_x, int from_y, int start_turn, bool last_container);
};

char DijkstraSolver::inverse_command(char command)
{
    if (command == 'L')
        return 'R';
    if (command == 'R')
        return 'L';
    if (command == 'U')
        return 'D';
    if (command == 'D')
        return 'U';
    throw runtime_error("inverse: invalid command");
}

int DijkstraSolver::command_index(char command)
{
    if (command == 'L')
        return 0;
    if (command == 'R')
        return 1;
    if (command == 'U')
        return 2;
    if (command == 'D')
        return 3;
    if (command == '.')
        return 4;
    throw runtime_error("index: invalid command");
}

char DijkstraSolver::command_from_index(int index)
{
    if (index == 0)
        return 'L';
    if (index == 1)
        return 'R';
    if (index == 2)
        return 'U';
    if (index == 3)
        return 'D';
    if (index == 4)
        return '.';
    throw runtime_error("invalid index");
}

P DijkstraSolver::next_pos(int current_x, int current_y, char command)
{
    if (command == 'L' || command == 'R' || command == 'U' || command == 'D')
    {
        int index = DijkstraSolver::command_index(command);
        return {current_x + dx[index], current_y + dy[index]};
    }
    else
    {
        return {current_x, current_y};
    }
}

char DijkstraSolver::infer_command(int current_pos, int prev_pos)
{
    if (current_pos == prev_pos)
        return '.';
    int current_x = current_pos / N;
    int current_y = current_pos % N;
    int prev_x = prev_pos / N;
    int prev_y = prev_pos % N;
    if (current_x == prev_x)
    {
        if (prev_y < current_y)
            return 'R';
        else
            return 'L';
    }
    else
    {
        if (prev_x < current_x)
            return 'D';
        else
            return 'U';
    }
}

bool DijkstraSolver::can_move(int current_x, int current_y, char command, int turn, bool have_cargo, int crane_id)
{
    if (command == 'L' || command == 'R' || command == 'U' || command == 'D')
    {
        int index = command_index(command);
        if (prohibited_moves[current_x][current_y][index][turn])
            return false;
    }
    P next = next_pos(current_x, current_y, command);
    if (next.first < 0 || next.first >= N || next.second < 0 || next.second >= N)
        return false;

    if (prohibited_cells[next.first][next.second][turn + 1] == 1)
        return false;

    /*
    if (crane_id == 3 && next.first == 3 && next.second == 0)
    {
        cerr << "crane_id:" << crane_id << " next:" << next.first << " " << next.second << " turn:" << turn << " prohibited:" << prohibited_cells[next.first][next.second][turn + 1] << endl;
    }
    */
    if (prohibited_cells[next.first][next.second][turn + 1] == 2 && have_cargo && crane_id > 0)
        return false;

    return true;
}

set<int> cargo_stacks = {2, 3, 12, 13, 22, 23, 0, 5, 10, 15, 20, 25};

void DijkstraSolver::set_prohibited(vector<char> &commands, int from_x, int from_y, int start_turn, bool last_container)
{
    // start_turnにfrom_x, from_yにいてcommandに沿って動くときに、侵入が禁止されている場所を記録する
    // すでにstart_turnの侵入禁止は指定されているのでOK
    int x = from_x;
    int y = from_y;

    bool has_cargo = false;
    int pickup_turn = -1;
    int pickup_pos = -1;
    for (int i = 0; i < commands.size(); i++)
    {
        char command = commands[i];
        if (command == 'P')
        {
            has_cargo = true;
            pickup_turn = start_turn + i;
            pickup_pos = x * N + y;
        }
        if (command == 'B')
            return;
        if (command == 'L' || command == 'R' || command == 'U' || command == 'D')
        {
            P next = next_pos(x, y, command);
            int index = command_index(inverse_command(command));
            prohibited_moves[next.first][next.second][index][start_turn + i] = true;
        }
        P next = next_pos(x, y, command);
        if (next.first < 0 || next.first >= N || next.second < 0 || next.second >= N)
            throw runtime_error("prohibit invalid command" + to_string(next.first) + " " + to_string(next.second));

        prohibited_cells[next.first][next.second][start_turn + i + 1] = 1;
        x = next.first;
        y = next.second;
    }
    // 途中に荷物を置いた場所から運び出した場合
    if (pickup_pos % N != 0 && has_cargo)
    {
        // この時にはstart_turn以降でprohibited_cellsが2になっている箇所を0に戻す
        int from_x = pickup_pos / N;
        int from_y = pickup_pos % N;
        for (int turn = pickup_turn + 1; turn < 150; turn++)
        {
            if (prohibited_cells[from_x][from_y][turn] == 2)
                prohibited_cells[from_x][from_y][turn] = 0;
        }
    }
    // 途中に荷物を置いた場合: それ以降のターンでの状態を2にする
    if (y != 4 && has_cargo)
    {
        for (int turn = start_turn + commands.size(); turn < 150; turn++)
        {
            if (prohibited_cells[x][y][turn] == 0)
                prohibited_cells[x][y][turn] = 2;
        }
    }

    if (pickup_pos % N == 0 && last_container)
    {
        int from_x = pickup_pos / N;
        int from_y = pickup_pos % N;
        for (int turn = pickup_turn + 1; turn < 150; turn++)
        {
            if (prohibited_cells[from_x][from_y][turn] == 2)
                prohibited_cells[from_x][from_y][turn] = 0;
        }
    }
}

vector<char> DijkstraSolver::find_route(int start_turn, int from_x, int from_y, int to_x, int to_y, int crane_id, bool have_cargo)
{
    // A*探索をする
    counter++;
    // start_turnにfrom_x, from_yからto_x, to_yに行くための経路を見つける
    // 辿り着いた後1ターン分は滞在しても大丈夫なことを担保する必要がある
    priority_queue<T4, vector<T4>, greater<T4>> pq;
    int tot_distance = abs(from_x - to_x) + abs(from_y - to_y);
    pq.emplace(start_turn + tot_distance * 1.5, start_turn, from_x * N + from_y, -1);
    vec_int aa = {0, 1, 2, 3, 4};
    shuffle(aa.begin(), aa.end(), mt);

    int lower_turn = -1;
    if (have_cargo && to_y != 4)
    {
        for (int i = start_turn; i < 150; i++)
        {
            if (prohibited_cells[to_x][to_y][i] == 1)
            {
                lower_turn = i;
            }
        }
    }

    while (!pq.empty())
    {
        auto [cost, turn, pos, prev_pos] = pq.top();
        pq.pop();
        if (turn > 145)
            break;
        int x = pos / N;
        int y = pos % N;
        if (visited[x][y][turn].first == counter)
            continue;
        visited[x][y][turn] = {counter, prev_pos};
        if (x == to_x && y == to_y && (prohibited_cells[x][y][turn + 1] == 0 || prohibited_cells[x][y][turn + 1] == 2 && !have_cargo) && turn > start_turn && turn > lower_turn)
        {
            vector<char> result;
            while (turn != start_turn)
            {
                auto [_, prev_pos] = visited[x][y][turn];
                result.push_back(infer_command(x * N + y, prev_pos));
                x = prev_pos / N;
                y = prev_pos % N;
                turn--;
            }
            reverse(result.begin(), result.end());
            return result;
        }
        rep(i, 5)
        {
            char command = command_from_index(aa[i]);
            if (can_move(x, y, command, turn, have_cargo, crane_id))
            {
                P next = next_pos(x, y, command);
                // 到着地でなくて荷物を持っている場合には物資の集積地を通れない
                // if (next != P(to_x, to_y) && cargo_stacks.count(next.first * N + next.second) == 1 && have_cargo && crane_id != 0)
                //    continue;
                int distance2 = abs(next.first - to_x) + abs(next.second - to_y);
                pq.emplace(turn + 1 + distance2 * 1.5, turn + 1, next.first * N + next.second, x * N + y);
            }
        }
    }
    return {};
}

class DijCraneState
{
private:
    vector<vector<int>> *board;

public:
    int crane_id;
    bool is_alive;
    int x;
    int y;
    int final_destination = -1;
    bool has_cargo;
    int cargo_id;
    DijCraneState(int crane_id, int x, int y, vector<vector<int>> *board) : crane_id(crane_id), x(x), y(y), has_cargo(false), cargo_id(-1), is_alive(true), board(board){};
    void accept_command(char command, vector<bool> &task_finished, int turn);
    void set_destination(int destination) { final_destination = destination; };
};

void DijCraneState::accept_command(char command, vector<bool> &task_finished, int turn)
{
    if (command == 'B')
    {
        if (has_cargo)
        {
            throw runtime_error("invalid command");
        };
        is_alive = false;
    }
    else if (command == 'L' || command == 'R' || command == 'U' || command == 'D' || command == '.')
    {
        P next = DijkstraSolver::next_pos(x, y, command);
        x = next.first;
        y = next.second;
    }
    else if (command == 'P')
    {
        if (!(!has_cargo && board->at(x)[y] != -1))
        {
            throw runtime_error("invalid command");
        }
        has_cargo = true;
        cargo_id = board->at(x)[y];
        board->at(x)[y] = -1;
    }
    else if (command == 'Q')
    {
        if (!(has_cargo && board->at(x)[y] == -1))
        {
            throw runtime_error("invalid command");
        }
        has_cargo = false;
        board->at(x)[y] = cargo_id;
        if (y == 4)
        {
            board->at(x)[y] = -1;
            task_finished[cargo_id] = true;
        }
        cargo_id = -1;
        final_destination = -1;
    }
    else
    {
        throw runtime_error("invalid command");
    }
}

class GameState
{
private:
public:
    int turn;
    vector<vector<int>> board;
    vector<vec_int> numbers_to_pop;
    vector<int> number_status; // -1: finished, -2: in queue, -3 carried, larger than 0: on board
    vector<vector<char>> commands;
    vector<DijCraneState> cranes;
    vector<bool> task_finished;
    bool is_finished();
    vector<string> make_ans();

    GameState()
    {
        board = vector<vector<int>>(N, vector<int>(N, -1)); // 盤面に置かれている荷物の番号
        numbers_to_pop = vector<vec_int>(5, vec_int(4, -1));
        number_status = vector<int>(N * N, -2);
        commands = vector<vector<char>>(6);
        turn = 0;
        rep(i, 5)
        {
            cranes.emplace_back(i, i, 0, &board);
        }
        rep(row, 5)
        {
            rep(i, 4)
            {
                numbers_to_pop[row][i] = A[row][4 - i];
            }
            board[row][0] = A[row][0];
        }

        task_finished = vector<bool>(N * N, false);
    }

    void process_turn();
};

bool GameState::is_finished()
{
    bool some_alive = false;
    rep(i, 5)
    {
        if (cranes[i].is_alive)
        {
            some_alive = true;
            break;
        }
    }
    if (!some_alive)
        return true;

    rep(i, N * N)
    {
        if (!task_finished[i])
            return false;
    }
    return true;
}

vector<string> GameState::make_ans()
{
    vector<string> ans;
    rep(i, 5)
    {
        string command = "";
        for (char c : commands[i])
        {
            command += c;
        }
        ans.push_back(command);
    }
    return ans;
}

void GameState::process_turn()
{
    rep(i, 5)
    {
        if (cranes[i].is_alive)
        {
            cranes[i].accept_command(commands[i][turn], task_finished, turn);
        }
    }

    rep(row, 5)
    {
        if (numbers_to_pop[row].size() == 0)
            continue;
        if (board[row][0] != -1)
            continue;
        // クレーンがそこにいて荷物を持っている場合には次のやつが出てこない
        bool flag = false;
        rep(crane_id, 5)
        {
            if (cranes[crane_id].x == row && cranes[crane_id].y == 0 && cranes[crane_id].has_cargo && cranes[crane_id].is_alive)
            {
                flag = true;
                break;
            }
        }
        if (flag)
            continue;

        int number = numbers_to_pop[row].back();
        numbers_to_pop[row].pop_back();
        board[row][0] = number;
    }
    turn++;
}

int find_next_task(vector<P> &simulation_result, TaskDependencyManager &dependency_manager, vector<bool> &task_finished, GameState &game_state, int crane_id)
{
    vector<P> A_inv = vector<P>(N * N);
    rep(i, N) rep(j, N) A_inv[A[i][j]] = P(i, j);
    vec_int candidates;

    int min_distance = INT_MAX;
    int min_distance_task = -1;
    rep(i, simulation_result.size())
    {
        if (task_finished[i])
            continue;
        // それより前のタスクで同じdestinationのものが終わっていなかったらだめ
        /*
        bool flag = false;
        for (auto dependent_task_id : dependent_tasks[i])
        {
            if (!task_finished[dependent_task_id])
            {
                flag = true;
                break;
            }
        }
        if (flag)
            continue;
        */
        int target_number = simulation_result[i].first;
        int destination = simulation_result[i].second;
        int dest_x = destination / N;
        int dest_y = destination % N;

        int med_x = -1;
        int med_y = -1;
        // med_x, med_yを中継してから目的地に行く
        rep(i, 5) rep(j, 5)
        {
            if (game_state.board[i][j] == target_number)
            {
                med_x = i;
                med_y = j;
            }
        }
        // クレーンで運搬中の時
        if (med_x == -1)
        {
            rep(i, N)
            {
                if (i == crane_id)
                    continue;
                if (game_state.cranes[i].has_cargo && game_state.cranes[i].cargo_id == target_number)
                {
                    med_x = game_state.cranes[i].final_destination / N;
                    med_y = game_state.cranes[i].final_destination % N;
                }
            }
        }

        // 盤上にない時はpopするところまで取りに行く
        if (med_x == -1)
        {
            int row = A_inv[target_number].first;
            med_x = row;
            med_y = 0;
        }
        int distance1 = abs(game_state.cranes[crane_id].x - med_x) + abs(game_state.cranes[crane_id].y - med_y);
        int distance2 = abs(med_x - dest_x) + abs(med_y - dest_y);
        // cerr << "crane_id:" << crane_id << " target_number:" << target_number << " dest_x:" << dest_x << " dest_y:" << dest_y << " med_x:" << med_x << " med_y:" << med_y << " distance1:" << distance1 << " distance2:" << distance2 << endl;

        if (!dependency_manager.can_execute_task(i, game_state.turn, distance1, distance1 + distance2 + 2))
            continue;

        /*
        bool prev_finished = true;
        if (dest_y == 4 && target_number % 5 != 0 && game_state.task_finished[target_number - 1] == false)
        {
            prev_finished = false;
        }
        if (med_x == -1 || game_state.board[dest_x][dest_y] != -1 || !prev_finished)
        {
            continue;
        }
        */
        // return i;
        candidates.push_back(i);
        if (distance1 < min_distance)
        {
            min_distance = distance1;
            min_distance_task = i;
        }
    }
    if (candidates.size() == 0)
        return -1;
    return min_distance_task;
}

vector<string> solve_by_dijkstra_solver(vector<P> &simulation_result)
{

    vector<P> A_inv = vector<P>(N * N, P(-1, -1));
    rep(i, N) rep(j, N) A_inv[A[i][j]] = P(i, j);
    auto dependency_manager = TaskDependencyManager(simulation_result);
    auto dijkstra_solver = DijkstraSolver();
    auto game_state = GameState();

    int turn = 0;

    vector<bool> task_finished(simulation_result.size(), false);

    int task_count = 0;
    vec_int aa = {0, 1, 2, 3, 4};
    shuffle(aa.begin(), aa.end(), mt);
    while (game_state.turn < 150)
    {
        if (game_state.turn == 0)
        {
            rep(tt, 5)
            {
                int crane_id = aa[tt];
                int row = crane_id;
                int container_id = A[row][0];
                int assigned_task = -1;
                rep(i, simulation_result.size())
                {
                    if (simulation_result[i].first == container_id)
                    {
                        if (!dependency_manager.can_execute_task(i, game_state.turn, 0, 1))
                            continue;
                        assigned_task = i;
                        break;
                    }
                }
                if (assigned_task == -1)
                {
                    continue;
                }
                int target_number = simulation_result[assigned_task].first;
                int destination = simulation_result[assigned_task].second;
                int dest_x = destination / N;
                int dest_y = destination % N;
                // cerr << "assigned_task:" << assigned_task << " crane_id:" << crane_id << " target_number:" << target_number << " dest_x:" << dest_x << " dest_y:" << dest_y << endl;

                int med_x = row;
                int med_y = 0;
                vector<char> route1;
                vector<char> total_route;
                total_route.push_back('P');
                vector<char> route2 = dijkstra_solver.find_route(turn + route1.size() + 1, med_x, med_y, dest_x, dest_y, crane_id, true);
                total_route.insert(total_route.end(), route2.begin(), route2.end());
                total_route.push_back('Q'); // 降ろし
                dependency_manager.add_dependency(assigned_task, turn, route1.size() + 1, total_route.size());
                game_state.commands[crane_id].insert(game_state.commands[crane_id].end(), total_route.begin(), total_route.end());
                game_state.cranes[crane_id].set_destination(destination);
                dijkstra_solver.set_prohibited(total_route, row, 0, turn, false);
                task_finished[assigned_task] = true;
                task_count++;
            }
        }

        // 各クレーンについていま暇だったらタスクを割り当てる
        rep(tt, 5)
        {
            int crane_id = aa[tt];
            if (!game_state.cranes[crane_id].is_alive) // 爆破ずみなのでスキップ
                continue;

            if (game_state.commands[crane_id].size() > turn) // コマンドが割り当てられているのでスキップ
                continue;

            if (task_count == simulation_result.size())
            {
                // 全てのタスクが終わっていて他のクレーンがタスクを実行しているので爆破する
                game_state.commands[crane_id].push_back('B');
                continue;
            }

            int start_x = game_state.cranes[crane_id].x;
            int start_y = game_state.cranes[crane_id].y;

            int assigned_task = find_next_task(simulation_result, dependency_manager, task_finished, game_state, crane_id);
            if (assigned_task == -1)
            {
                // 実行できるタスクがなかったので左の方に行く
                vector<char> route = dijkstra_solver.find_route(turn, start_x, start_y, start_x, start_y, crane_id, false);
                if (route.size() == 0)
                {
                    game_state.commands[crane_id].push_back('B');
                }
                else
                {
                    // route = {route[0]};
                    game_state.commands[crane_id].insert(game_state.commands[crane_id].end(), route.begin(), route.end());
                    dijkstra_solver.set_prohibited(route, start_x, start_y, turn, false);
                }
                continue;
            }
            // この時にはタスクを実行できる

            int target_number = simulation_result[assigned_task].first;
            int destination = simulation_result[assigned_task].second;
            int dest_x = destination / N;
            int dest_y = destination % N;
            // cerr << "assigned_task:" << assigned_task << " crane_id:" << crane_id << " target_number:" << target_number << " dest_x:" << dest_x << " dest_y:" << dest_y << endl;

            int med_x = -1;
            int med_y = -1;
            // med_x, med_yを中継してから目的地に行く
            rep(i, 5) rep(j, 5)
            {
                if (game_state.board[i][j] == target_number)
                {
                    med_x = i;
                    med_y = j;
                }
            }
            // クレーンで運搬中の時
            if (med_x == -1)
            {
                rep(i, N)
                {
                    if (i == crane_id)
                        continue;
                    if (game_state.cranes[i].has_cargo && game_state.cranes[i].cargo_id == target_number)
                    {
                        med_x = game_state.cranes[i].final_destination / N;
                        med_y = game_state.cranes[i].final_destination % N;
                    }
                }
            }

            if (med_x == -1)
            {
                int row = A_inv[target_number].first;
                med_x = row;
                med_y = 0;
            }

            // この時には中継値に寄ってから目的地に行くようなコマンドを追加する
            vector<char> total_route;
            vector<char> route1 = dijkstra_solver.find_route(turn, start_x, start_y, med_x, med_y, crane_id, false);
            total_route.insert(total_route.end(), route1.begin(), route1.end());
            total_route.push_back('P'); // 積み込み
            vector<char> route2 = dijkstra_solver.find_route(turn + route1.size() + 1, med_x, med_y, dest_x, dest_y, crane_id, true);
            total_route.insert(total_route.end(), route2.begin(), route2.end());
            total_route.push_back('Q'); // 降ろし
            // cerr << "tot_route:" << total_route.size() << endl;

            if (route1.size() == 0 || route2.size() == 0)
            {
                game_state.commands[crane_id].push_back('B');
                continue;
            }

            dependency_manager.add_dependency(assigned_task, turn, route1.size() + 1, total_route.size());
            game_state.commands[crane_id].insert(game_state.commands[crane_id].end(), total_route.begin(), total_route.end());
            game_state.cranes[crane_id].set_destination(destination);
            dijkstra_solver.set_prohibited(total_route, start_x, start_y, turn, start_y == 0 && A[start_x][4] == target_number);
            task_finished[assigned_task] = true;
            task_count++;
        }
        game_state.process_turn();
        turn++;
        if (game_state.is_finished())
            break;
    }

    if (!game_state.is_finished() || all_of(task_finished.begin(), task_finished.end(), [](bool x)
                                            { return x; }) == false)
    {
        cerr << "failed" << endl;
        return vector<string>(5, "");
    }

    return game_state.make_ans();
}




using Hash = uint32_t; // TODO
using Cost = double;

// ビームサーチの設定
struct Config
{
    int max_turn;
    size_t beam_width;
    size_t tour_capacity;
    uint32_t hash_map_capacity;
    Config(int max_turn, size_t beam_width, size_t tour_capacity, uint32_t hash_map_capacity) : max_turn(max_turn),
                                                                                                beam_width(beam_width),
                                                                                                tour_capacity(tour_capacity),
                                                                                                hash_map_capacity(hash_map_capacity) {}
};

// 連想配列
// Keyにハッシュ関数を適用しない
// open addressing with linear probing
// unordered_mapよりも速い
// nは格納する要素数よりも16倍ほど大きくする
template <class Key, class T>
struct HashMap
{
public:
    explicit HashMap(uint32_t n)
    {
        if (n % 2 == 0)
        {
            ++n;
        }
        n_ = n;
        valid_.resize(n_, false);
        data_.resize(n_);
    }

    // 戻り値
    // - 存在するならtrue、存在しないならfalse
    // - index
    pair<bool, int> get_index(Key key) const
    {
        Key i = key % n_;
        while (valid_[i])
        {
            if (data_[i].first == key)
            {
                return {true, i};
            }
            if (++i == n_)
            {
                i = 0;
            }
        }
        return {false, i};
    }

    // 指定したindexにkeyとvalueを格納する
    void set(int i, Key key, T value)
    {
        valid_[i] = true;
        data_[i] = {key, value};
    }

    // 指定したindexのvalueを返す
    T get(int i) const
    {
        assert(valid_[i]);
        return data_[i].second;
    }

    void clear()
    {
        fill(valid_.begin(), valid_.end(), false);
    }

private:
    uint32_t n_;
    vector<bool> valid_;
    vector<pair<Key, T>> data_;
};

// 状態のコストを評価するための構造体
// メモリ使用量をできるだけ小さくしてください
struct Evaluator
{
    // TODO
    Cost score;

    Evaluator(Cost score) : score(score)
    {
    }

    // 低いほどよい
    Cost evaluate() const
    {
        return score;
    }
};

// 状態遷移を行うために必要な情報
// メモリ使用量をできるだけ小さくしてください
struct Action
{
    // TODO
    int crane_id;
    int pickup_pos;
    int pickup_turn;
    int destination;
    string command;

    Action(int crane_id, int pickup_pos, int pickup_turn, int destination, string command) : crane_id(crane_id),
                                                                                             pickup_pos(pickup_pos), pickup_turn(pickup_turn), destination(destination), command(command)
    {
    }

    bool operator==(const Action &other) const
    {
        return crane_id == other.crane_id && pickup_pos == other.pickup_pos && destination == other.destination;
    }
};

// 展開するノードの候補を表す構造体
struct Candidate
{
    Action action;
    Evaluator evaluator;
    Hash hash;
    int parent;

    Candidate(Action action, Evaluator evaluator, Hash hash, int parent) : action(action),
                                                                           evaluator(evaluator),
                                                                           hash(hash),
                                                                           parent(parent) {}
};

// ノードの候補から実際に追加するものを選ぶクラス
// ビーム幅の個数だけ、評価がよいものを選ぶ
// ハッシュ値が一致したものについては、評価がよいほうのみを残す
class Selector
{
public:
    explicit Selector(const Config &config) : hash_to_index_(config.hash_map_capacity)
    {
        beam_width = config.beam_width;
        candidates_.reserve(beam_width * 2);
        full_ = false;

        costs_.resize(beam_width);
        for (size_t i = 0; i < beam_width; ++i)
        {
            costs_[i] = {0, i};
        }
    }

    void update_beam_width(size_t beam_width)
    {
        this->beam_width = beam_width;
        costs_.resize(beam_width);
        for (size_t i = 0; i < beam_width; ++i)
        {
            costs_[i] = {0, i};
        }
    }

    // 候補を追加する
    // ターン数最小化型の問題で、candidateによって実行可能解が得られる場合にのみ finished = true とする
    // ビーム幅分の候補をCandidateを追加したときにsegment treeを構築する
    void push(const Candidate &candidate, bool finished)
    {
        if (finished)
        {
            finished_candidates_.emplace_back(candidate);
            return;
        }
        Cost cost = candidate.evaluator.evaluate();
        if (full_ && cost >= st_.all_prod().first)
        {
            // 保持しているどの候補よりもコストが小さくないとき
            return;
        }
        auto [valid, i] = hash_to_index_.get_index(candidate.hash);

        if (valid)
        {
            int j = hash_to_index_.get(i);
            if (candidate.hash == candidates_[j].hash)
            {
                // ハッシュ値が等しいものが存在しているとき
                if (full_)
                {
                    // segment treeが構築されている場合
                    if (cost < st_.get(j).first)
                    {
                        candidates_[j] = candidate;
                        if (j >= beam_width || j < 0)
                        {
                            cerr << "j:" << j << " beam_width:" << beam_width << " " << candidate.hash << " " << i << endl;
                        }
                        st_.set(j, {cost, j});
                    }
                }
                else
                {
                    // segment treeが構築されていない場合
                    if (cost < costs_[j].first)
                    {
                        candidates_[j] = candidate;
                        costs_[j].first = cost;
                    }
                }
                return;
            }
        }
        if (full_)
        {
            // segment treeが構築されている場合
            int j = st_.all_prod().second;
            hash_to_index_.set(i, candidate.hash, j);
            candidates_[j] = candidate;
            if (j >= beam_width || j < 0)
            {
                cerr << "2j:" << j << " beam_width:" << beam_width << " " << candidate.hash << " " << i << endl;
                cerr << st_.all_prod().first << endl;
            }

            st_.set(j, {cost, j});
        }
        else
        {
            // segment treeが構築されていない場合
            int j = candidates_.size();
            hash_to_index_.set(i, candidate.hash, j);
            candidates_.emplace_back(candidate);
            costs_[j].first = cost;
            if (cost < 0.00001)
            {
                cerr << "cost:" << cost << endl;
                throw runtime_error("cost < 0");
            }

            if (candidates_.size() == beam_width)
            {
                // 保持している候補がビーム幅分になったときにsegment treeを構築する
                full_ = true;
                st_ = MaxSegtree(costs_);
            }
        }
    }
    Cost current_worst_score()
    {
        if (!full_)
        {
            return INT_MAX;
        }
        return st_.all_prod().first;
    }

    // 選んだ候補を返す
    const vector<Candidate> &select() const
    {
        return candidates_;
    }

    // 実行可能解が見つかったか
    bool have_finished() const
    {
        return !finished_candidates_.empty();
    }

    // 実行可能解に到達するCandidateを返す
    vector<Candidate> get_finished_candidates() const
    {
        return finished_candidates_;
    }

    // 最もよいCandidateを返す
    Candidate calculate_best_candidate() const
    {
        if (full_)
        {
            size_t best = 0;
            for (size_t i = 0; i < beam_width; ++i)
            {
                if (st_.get(i).first < st_.get(best).first)
                {
                    best = i;
                }
            }
            return candidates_[best];
        }
        else
        {
            size_t best = 0;
            for (size_t i = 0; i < candidates_.size(); ++i)
            {
                if (costs_[i].first < costs_[best].first)
                {
                    best = i;
                }
            }
            return candidates_[best];
        }
    }

    void clear()
    {
        candidates_.clear();
        hash_to_index_.clear();
        full_ = false;
    }
    size_t beam_width;

private:
    // 削除可能な優先度付きキュー
    using MaxSegtree = atcoder::segtree<
        pair<Cost, int>,
        [](pair<Cost, int> a, pair<Cost, int> b)
        {
            if (a.first >= b.first)
            {
                return a;
            }
            else
            {
                return b;
            }
        },
        []()
        { return make_pair(numeric_limits<Cost>::min(), -1); }>;

    vector<Candidate> candidates_;
    HashMap<Hash, int> hash_to_index_;
    bool full_;
    vector<pair<Cost, int>> costs_;
    MaxSegtree st_;
    vector<Candidate> finished_candidates_;
};

struct BeamSearchTask
{
    BeamSearchTask(){};
    BeamSearchTask(int from_pos, int to_pos, int container_id, bool is_direct) : from_pos(from_pos), to_pos(to_pos), container_id(container_id), is_direct(is_direct){};
    int from_pos;   // 不定の時には -1
    int to_pos;     // 不定の時には-1
    bool is_direct; // この時には直接0->4列に運ぶ
    int container_id;
};




int calc_score(vector<BeamSearchTask> &tasks)
{
    int result = 0;
    // 最初5個の運び元の数だけ加点をする
    set<int> from_pos;
    rep(i, 5)
    {
        from_pos.insert(tasks[i].from_pos);
    }
    result += from_pos.size() * 10;
    set<int> last_pos;
    rep(i, 5)
    {
        last_pos.insert(tasks[tasks.size() - 1 - i].to_pos);
    }
    int col0count = 0;

    /**/
    set<int> col0;
    rep(i, 5) col0.insert(A[i][0]);
    int count = 0;
    rep(i, tasks.size())
    {
        if (tasks[i].from_pos % 5 == 0 && col0.count(tasks[i].container_id))
        {
            result -= 2 * (i - count);
            count++;
        }
    }

    result += last_pos.size() * 50;
    // 連続してfrom_posが同じところがあると減点
    /**/
    for (int i = 0; i < tasks.size() - 1; i++)
    {
        if (tasks[i].from_pos == -1)
            continue;
        if (tasks[i].from_pos == tasks[i + 1].from_pos)
        {
            result -= 3;
        }
        if (i < tasks.size() - 2)
        {
            if (tasks[i].from_pos == tasks[i + 2].from_pos)
            {
                result -= 1;
            }
        }
    }

    // 連続したto_posがあると原点
    for (int i = 0; i < tasks.size() - 1; i++)
    {
        if (tasks[i].to_pos == -1)
            continue;
        if (tasks[i].to_pos == tasks[i + 1].to_pos)
        {
            result -= 3;
        }
        if (i < tasks.size() - 2)
        {
            if (tasks[i].to_pos == tasks[i + 2].to_pos)
            {
                result -= 1;
            }
        }
    }
    return result;
}

bool is_valid_tasks(vector<BeamSearchTask> &tasks, int limit_count)
{
    // 運び出し順がおかしいとダメ
    // 運び出し先がおかしいとダメ
    vector<int> row_count(5, 0);
    vector<bool> pick_uped(25, false);
    vector<int> row_count2(5, 0);
    for (int i = 0; i < tasks.size(); i++)
    {
        if (tasks[i].from_pos != -1)
        {
            int row = tasks[i].from_pos / 5;
            if (tasks[i].container_id != A[row][row_count[row]])
            {
                // cerr << "container_id:" << tasks[i].container_id << " A:" << A[row][row_count[row]] << endl;
                return false;
            }
            row_count[row]++;
            pick_uped[tasks[i].container_id] = true;
        }
        else
        {
            if (!pick_uped[tasks[i].container_id])
            {
                // cerr << "container_id:" << tasks[i].container_id << " is not picked up" << endl;
                return false;
            }
        }
        if (tasks[i].to_pos == -1)
            continue;
        int container_id = tasks[i].container_id;
        int dest_row = container_id / 5;
        if (row_count2[dest_row] != container_id % 5)
        {
            // cerr << "container_id:" << container_id << " dest_row:" << dest_row << " row_count2:" << row_count2[dest_row] << endl;
            return false;
        }
        row_count2[dest_row]++;
    }

    // 同時に場に出ているものが7個以上になると無効
    int count = 0;
    vector<bool> on_board(25, false);
    vector<int> row_progress(5, 0);
    for (int i = 0; i < tasks.size(); i++)
    {
        int from_pos = tasks[i].from_pos;
        int to_pos = tasks[i].to_pos;
        int container_id = tasks[i].container_id;

        if (to_pos == -1)
        {
            on_board[container_id] = true;
            count++;
        }
        else
        {
            if (on_board[container_id])
            {
                count--;
                on_board[container_id] = false;
            }
        }
        if (count >= 8)
        {
            // cerr << "count:" << count << endl;
            return false;
        }
    }
    return true;
}

vector<BeamSearchTask> shuffle_tasks(vector<BeamSearchTask> tasks)
{
    int best_score = calc_score(tasks);
    int score = best_score;
    auto best_tasks = tasks;
    float start_time = get_time(false);
    int valid_count = 8 + ro.get_rand(0, 2);
    // cerr << "start shuffle" << " tokuten1:" << score << " " << is_valid_tasks(tasks, valid_count) << endl;
    if (!is_valid_tasks(tasks, 8))
    {
        valid_count = 9;
    }
    int count = 0;
    double T0 = 0.4;
    double T1 = 0.0;
    while (true)
    {
        if (get_time(false) - start_time > TIME_LIMIT * 0.01)
            break;

        int pos1 = ro.get_rand(0, tasks.size());
        int pos2 = ro.get_rand(0, tasks.size() - 1);
        if (pos2 >= pos1)
            pos2++;
        swap(tasks[pos1], tasks[pos2]);
        if (!is_valid_tasks(tasks, valid_count))
        {
            swap(tasks[pos1], tasks[pos2]);
            continue;
        }
        int new_score = calc_score(tasks);
        int diff = new_score - score;
        if (is_valid_transition(diff, get_time(false) - start_time, T0, T1, 0.01))
        {
            count++;
            if (new_score > best_score)
            {
                best_score = new_score;
                best_tasks = tasks;
            }
            score = new_score;
        }
        else
        {
            swap(tasks[pos1], tasks[pos2]);
        }
    }
    cerr << "end shuffle" << " tokuten2:" << score << " " << count << endl;
    return tasks;
}




int STACK_COUNT = 8;
vec_int stack_pos = {2, 3, 7, 8, 17, 18, 22, 23};
map<int, int> dict = {{2, 0}, {3, 1}, {7, 2}, {8, 3}, {17, 4}, {18, 5}, {22, 6}, {23, 7}};

struct StackState
{
    vector<vec_int> stack_queue;
    vector<vector<int>> pass_turns;
    StackState()
    {
        stack_queue = vector<vec_int>(STACK_COUNT);
        pass_turns = vector<vec_int>(STACK_COUNT, {-1});
    }
    int can_push(int stack_id)
    {
        return stack_queue[stack_id].size() % 2 == 0; // サイズが2の倍数の時には次における可能性がある
    }

    int minimum_push_turn(int stack_id)
    {
        if (stack_queue[stack_id].size() == 0)
        {
            return pass_turns[stack_id].back() + 1;
        }
        return max(stack_queue[stack_id].back() + 2, pass_turns[stack_id].back() + 1);
    }
    int get_pos_index(int pos)
    {
        if (dict.count(pos))
            return dict[pos];

        return -1;
    }

    // この先の処理は自明だがめんどくさいので明日にする
    void finish_turn(int start_pos, int start_turn, int crane_id, string command)
    {
        if (crane_id == 0)
            return;

        int pos = start_pos;
        bool pick_up = false;
        int x = start_pos / N;
        int y = start_pos % N;
        for (int i = 0; i < command.size(); i++)
        {
            auto c = command[i];
            if (c == 'L')
                y--;
            if (c == 'R')
                y++;
            if (c == 'U')
                x--;
            if (c == 'D')
                x++;
            if (c == 'P')
                pick_up = true;

            pos = x * N + y;
            if (pick_up && dict.count(pos))
            {
                pass_turns[dict[pos]].push_back(start_turn + i + 1);
            }
        }
    }
    void undo_finish_turn(int start_pos, int crane_id, string command)
    {
        if (crane_id == 0)
            return;

        int pos = start_pos;
        bool pick_up = false;
        int x = start_pos / N;
        int y = start_pos % N;
        for (int i = 0; i < command.size(); i++)
        {
            auto c = command[i];
            if (c == 'L')
                y--;
            if (c == 'R')
                y++;
            if (c == 'U')
                x--;
            if (c == 'D')
                x++;
            if (c == 'P')
                pick_up = true;

            pos = x * N + y;
            if (pick_up && dict.count(pos))
            {
                pass_turns[dict[pos]].pop_back();
            }
        }
    }

    void drop_container(int pos, int turn)
    {
        int stack_id = get_pos_index(pos);
        stack_queue[stack_id].push_back(turn);
    }
    void undo_drop_container(int pos)
    {
        int stack_id = get_pos_index(pos);
        stack_queue[stack_id].pop_back();
    }

    void pickup_container(int pos, int turn)
    {
        int stack_id = get_pos_index(pos);
        stack_queue[stack_id].push_back(turn);
    }
    void undo_pickup_container(int pos)
    {
        int stack_id = get_pos_index(pos);
        stack_queue[stack_id].pop_back();
    }
};

struct CraneState
{
    vector<int> last_pos; // pos[0]は0ターン目での開始位置
    string operations;
    int crane_id;
    CraneState(){};
    CraneState(int crane_id) : crane_id(crane_id)
    {
        last_pos.push_back(crane_id * 5);
    }
};

// タスクの依存関係の静的な情報を持つことにする
// これ、依存関係は一つだけだな
struct BSTaskDependencyManager
{
    vector<int> pickup_dependent_task;
    vector<int> drop_dependent_task;
    vector<bool> is_drop_to_stack;
    vector<bool> is_pickup_from_stack;
    BSTaskDependencyManager(){};

    BSTaskDependencyManager(vector<BeamSearchTask> &tasks)
    {
        pickup_dependent_task = vec_int(tasks.size(), -1);
        drop_dependent_task = vec_int(tasks.size(), -1);
        is_drop_to_stack = vector<bool>(tasks.size(), false);     // 参照する時刻が異なる
        is_pickup_from_stack = vector<bool>(tasks.size(), false); // 参照する時刻が異なる
        vector<vec_int> container_tasks(N * N);
        for (int i = 0; i < tasks.size(); i++)
        {
            container_tasks[tasks[i].container_id].push_back(i);
        }
        // pickup dependencyを作る
        for (int row = 0; row < 5; row++)
        {
            for (int col = 0; col < 4; col++)
            {
                int task_id = container_tasks[A[row][col]][0];
                int next_task_id = container_tasks[A[row][col + 1]][0];
                // next_task_idを行うためにはtask_idが実行されている必要がある
                pickup_dependent_task[next_task_id] = task_id;
            }
        }
        rep(i, N * N)
        {
            if (container_tasks[i].size() == 2)
            {
                pickup_dependent_task[container_tasks[i][1]] = container_tasks[i][0]; // この時にはdropの時刻
                is_drop_to_stack[container_tasks[i][0]] = true;
                is_pickup_from_stack[container_tasks[i][1]] = true;
            }
        }
        // drop dependencyを作る
        for (int row = 0; row < 5; row++)
        {
            for (int col = 0; col < 4; col++)
            {
                int num1 = row * 5 + col;
                int num2 = row * 5 + col + 1;
                int task_id1 = container_tasks[num1][container_tasks[num1].size() - 1];
                int task_id2 = container_tasks[num2][container_tasks[num2].size() - 1];
                drop_dependent_task[task_id2] = task_id1;
            }
        }
    }
};

// タスクの終了時刻を記録する状態

struct TaskFinishState
{
    vec_int pickup_finish_turn;
    vec_int drop_finish_turn;
    vec_int drop_pos;
    vec_int assigned_crane;
    TaskFinishState(){};
    TaskFinishState(int task_count)
    {
        pickup_finish_turn = vec_int(task_count, -1);
        drop_finish_turn = vec_int(task_count, -1);
        drop_pos = vec_int(task_count, -1);
        assigned_crane = vec_int(task_count, -1);
    }
    bool can_start_task(int task_id, BSTaskDependencyManager &tdm)
    {
        // taskidの依存先のタスクの終了時刻がわかっていればOK
        if (tdm.is_pickup_from_stack[task_id])
        {
            if (drop_finish_turn[tdm.pickup_dependent_task[task_id]] == -1)
            {
                return false;
            }
        }
        else if (tdm.pickup_dependent_task[task_id] != -1)
        {
            if (pickup_finish_turn[tdm.pickup_dependent_task[task_id]] == -1)
            {
                return false;
            }
        }
        if (tdm.drop_dependent_task[task_id] != -1)
        {
            if (drop_finish_turn[tdm.drop_dependent_task[task_id]] == -1)
            {
                return false;
            }
        }
        return true;
    }
    int get_task_pickup_start_turn(int task_id, BSTaskDependencyManager &tdm)
    {
        if (tdm.is_pickup_from_stack[task_id])
        {
            return drop_finish_turn[tdm.pickup_dependent_task[task_id]] + 1;
        }
        else if (tdm.pickup_dependent_task[task_id] == -1)
        {
            return 0;
        }
        else
        {
            return pickup_finish_turn[tdm.pickup_dependent_task[task_id]] + 1;
        }
    }

    int get_task_drop_start_turn(int task_id, BSTaskDependencyManager &tdm)
    {
        if (tdm.is_drop_to_stack[task_id])
        {
            return -1;
        }
        else if (tdm.drop_dependent_task[task_id] == -1)
        {
            return 0;
        }
        else
        {
            return drop_finish_turn[tdm.drop_dependent_task[task_id]] + 1;
        }
    }

    // これは純粋にタスクを終了させるコマンドを実行したターン数を記録する
    void task_finished(int task_id, int start_turn, string commands, int destination, int pickup_turn, int crane_id)
    {
        drop_pos[task_id] = destination;
        drop_finish_turn[task_id] = start_turn + commands.size();
        pickup_finish_turn[task_id] = pickup_turn;
        assigned_crane[task_id] = crane_id;
    }

    void undo_task_finished(int task_id, string commands, int destination)
    {
        drop_pos[task_id] = -1;
        drop_finish_turn[task_id] = -1;
        pickup_finish_turn[task_id] = -1;
        assigned_crane[task_id] = -1;
    }
};

struct CraneTrajectory
{
    vector<bool> trajectory;
    vector<bool> movement_trajectory; // turnにfrom_posからto_posに移動することができるか?
    CraneTrajectory()
    {
        trajectory = vector<bool>(N * N * 200, false);
        movement_trajectory = vector<bool>(N * N * N * N * 200, false);
        trajectory[0] = true;
        trajectory[5] = true;
        trajectory[10] = true;
        trajectory[15] = true;
        trajectory[20] = true;
    }

    bool can_move(int turn, int from_pos, int next_pos);

    void finish_turn(int turn, vec_int &traj, int current_pos);
    void undo_finish_turn(int turn, vec_int &traj, int current_pos);
};

bool CraneTrajectory::can_move(int turn, int from_pos, int next_pos)
{
    // turn+1 にnextposにいる、　turnにfromposからnextposに移動する
    return !trajectory[next_pos + (turn + 1) * 25] && !movement_trajectory[from_pos + next_pos * 25 + turn * 625];
}

void CraneTrajectory::finish_turn(int turn, vec_int &traj, int current_pos)
{
    int err_count = 0;
    for (int i = 0; i < traj.size(); i++)
    {
        this->trajectory[traj[i] + (turn + i + 1) * 25] = true;
    }
    /*
    if (movement_trajectory[traj[0]][current_pos][turn])
    {
        cerr << traj[0] << " " << current_pos << " " << turn << endl;
        err_count++;
        // throw runtime_error("0finish turn error");
    }
    */

    movement_trajectory[traj[0] + 25 * current_pos + 625 * turn] = true;
    for (int i = 0; i < traj.size() - 1; i++)
    {
        /*
        if (movement_trajectory[traj[i + 1]][traj[i]][turn + 1 + i])
        {
            cerr << traj[i + 1] << " " << traj[i] << " " << turn + 1 << endl;
            err_count++;
            // throw runtime_error("finish turn error");
        }
        */
        movement_trajectory[traj[i + 1] + 25 * traj[i] + 625 * (turn + 1 + i)] = true;
    }
    /*
    if (err_count > 0)
    {
        cerr << "err_count:" << err_count << endl;
        throw runtime_error("finish turn error");
    }
    */
}

void CraneTrajectory::undo_finish_turn(int turn, vec_int &traj, int current_pos)
{
    for (int i = 0; i < traj.size(); i++)
    {
        this->trajectory[traj[i] + 25 * (turn + i + 1)] = false;
    }
    /*
    if (!movement_trajectory[traj[0]][current_pos][turn])
    {
        cerr << traj[0] << " " << current_pos << " " << turn << endl;
        throw runtime_error("0 undo finish turn error");
    }
    */

    movement_trajectory[traj[0] + 25 * current_pos + 625 * turn] = false;
    for (int i = 0; i < traj.size() - 1; i++)
    {
        /*
        if (!movement_trajectory[traj[i + 1]][traj[i]][turn + 1 + i])
        {
            cerr << traj[i + 1] << " " << traj[i] << " " << turn + 1 << endl;
            throw runtime_error("undo finish turn error");
        }
        */
        movement_trajectory[traj[i + 1] + 25 * traj[i] + (turn + 1 + i) * 625] = false;
    }
}

class RouteFinder
{
private:
    bool can_move(int current_x, int current_y, char command, int turn, bool have_cargo, int crane_id, CraneTrajectory &traj, StackState &stack_state, vec_int &row_empty_turn, int destination, int minimum_turn);
    char inverse_command(char command);
    // vector<vector<P>> visited;
    vector<P> visited;
    vector<string> route_cache;
    vector<vector<T>> que;

public:
    static int command_index(char command);
    static char command_from_index(int index);
    static char infer_command(int current_pos, int prev_pos);
    static P next_pos(int current_x, int current_y, char command);

    int counter = 0;
    RouteFinder()
    {
        visited = vector<P>(N * N * 350, {-1, -1});
        route_cache = vector<string>(N * N * N * N, "-");
        que = vector<vector<T>>(15);
    };
    string find_route(int start_turn, int from_pos, int to_pos, int crane_id, bool have_cargo, CraneTrajectory &traj, StackState &stack_state, vec_int &row_empty_turn, int minimum_turn);
    void set_prohibited(vector<char> &commands, int from_x, int from_y, int start_turn, bool last_container);
};

char RouteFinder::inverse_command(char command)
{
    if (command == 'L')
        return 'R';
    if (command == 'R')
        return 'L';
    if (command == 'U')
        return 'D';
    if (command == 'D')
        return 'U';
    throw runtime_error("inverse: invalid command");
}

int RouteFinder::command_index(char command)
{
    if (command == 'L')
        return 0;
    if (command == 'R')
        return 1;
    if (command == 'U')
        return 2;
    if (command == 'D')
        return 3;
    if (command == '.')
        return 4;
    throw runtime_error("index: invalid command");
}

char RouteFinder::command_from_index(int index)
{
    if (index == 0)
        return 'L';
    if (index == 1)
        return 'R';
    if (index == 2)
        return 'U';
    if (index == 3)
        return 'D';
    if (index == 4)
        return '.';
    throw runtime_error("invalid index");
}

P RouteFinder::next_pos(int current_x, int current_y, char command)
{
    if (command == 'L' || command == 'R' || command == 'U' || command == 'D')
    {
        int index = RouteFinder::command_index(command);
        return {current_x + dx[index], current_y + dy[index]};
    }
    else
    {
        return {current_x, current_y};
    }
}

char RouteFinder::infer_command(int current_pos, int prev_pos)
{
    if (current_pos == prev_pos)
        return '.';
    int current_x = current_pos / N;
    int current_y = current_pos % N;
    int prev_x = prev_pos / N;
    int prev_y = prev_pos % N;
    if (current_x == prev_x)
    {
        if (prev_y < current_y)
            return 'R';
        else
            return 'L';
    }
    else
    {
        if (prev_x < current_x)
            return 'D';
        else
            return 'U';
    }
}

// bool RouteFinder::can_move(int current_x, int current_y, char command, int turn, bool have_cargo, int crane_id, State &state, int destination, int minimum_turn)
bool RouteFinder::can_move(int current_x, int current_y, char command, int turn, bool have_cargo, int crane_id, CraneTrajectory &traj,
                           StackState &stack_state, vec_int &row_empty_turn, int destination, int minimum_turn)
{
    if (turn == 1 && have_cargo && (command == 'U' || command == 'D'))
        return false;
    P next = next_pos(current_x, current_y, command);
    if (next.first < 0 || next.first >= N || next.second < 0 || next.second >= N)
        return false;
    // 他のクレーンとの衝突
    if (!traj.can_move(turn, current_x * N + current_y, next.first * N + next.second))
        return false;

    int np = next.first * N + next.second;
    if (have_cargo && crane_id != 0)
    {
        if (np != destination && dict.count(np))
        {
            int pos_index = stack_state.get_pos_index(np);
            if (!stack_state.can_push(pos_index))
                return false;
            int minimum_push_turn = stack_state.minimum_push_turn(pos_index);
            if (turn < minimum_push_turn)
                return false;
        }
        if (np == destination && turn < minimum_turn - 1)
        {
            return false;
        }
        if (!(command == '.' || command == 'B' || command == 'P' || command == 'Q') && next.second == 0)
        {
            // UDの移動であって、同じターンにpickupされていればOKになるはず

            if (row_empty_turn[next.first] >= turn)
                return false;
        }
    }

    return true;
}

string RouteFinder::find_route(int start_turn, int from_pos, int to_pos, int crane_id, bool have_cargo, CraneTrajectory &traj,
                               StackState &stack_state, vec_int &row_empty_turn, int minimum_turn)
{
    counter++;
    // priority_queue<T4, vector<T4>, greater<T4>> pq;

    if (route_cache[from_pos * 25 + to_pos] != "-" && start_turn > 0)
    {
        string command = route_cache[from_pos * 25 + to_pos];

        if (start_turn + command.size() >= minimum_turn)
        {
            int pos = from_pos;
            bool flag = true;
            for (int i = 0; i < command.size(); i++)
            {
                if (can_move(pos / N, pos % N, command[i], start_turn + i, have_cargo, crane_id, traj, stack_state, row_empty_turn, to_pos, minimum_turn))
                {
                    auto next = next_pos(pos / N, pos % N, command[i]);
                    pos = next.first * N + next.second;
                }
                else
                {
                    flag = false;
                    break;
                }
            }
            if (flag && pos == to_pos && can_move(pos / N, pos % N, '.', start_turn + command.size(), have_cargo, crane_id, traj, stack_state, row_empty_turn, to_pos, minimum_turn))
            {
                return command;
            }
        }
    }

    // A*探索をする
    // start_turnにfrom_x, from_yからto_x, to_yに行くための経路を見つける
    // 辿り着いた後1ターン分は滞在しても大丈夫なことを担保する必要がある
    int tot_distance = abs(from_pos / 5 - to_pos / 5) + abs(from_pos % 5 - to_pos % 5);
    // pq.emplace(start_turn + tot_distance * 1.0, start_turn, from_pos, -1);
    vector<int> aaaa = {0, 1, 2, 3, 4};
    // shuffle(aaaa.begin(), aaaa.end(), mt);

    int MAX_SIZE = tot_distance + 4;
    rep(i, MAX_SIZE) que[i].clear();

    que[0].push_back({start_turn, from_pos, -1});
    for (int tn = 0; tn < MAX_SIZE; tn++)
    {
        while (!que[tn].empty())
        {
            auto [turn, pos, prev_pos] = que[tn].back();
            que[tn].pop_back();
            if (turn - start_turn > tot_distance + 5)
                break;

            if (turn > 140)
                break;
            int x = pos / N;
            int y = pos % N;
            if (visited[pos + turn * 25].first == counter)
                continue;
            visited[pos + turn * 25] = {counter, prev_pos};
            if (pos == to_pos && turn >= minimum_turn && can_move(x, y, '.', turn, have_cargo, crane_id, traj, stack_state, row_empty_turn, to_pos, minimum_turn))
            {
                vector<char> result;
                while (turn != start_turn)
                {
                    auto [_, prev_pos] = visited[pos + turn * 25];
                    result.push_back(infer_command(pos, prev_pos));
                    pos = prev_pos;
                    turn--;
                }
                reverse(result.begin(), result.end());
                string route = "";
                for (auto c : result)
                    route += c;
                if (route.size() == tot_distance && tot_distance >= 2)
                {
                    route_cache[from_pos * 25 + to_pos] = route;
                }
                return route;
            }
            rep(i, 5)
            {
                char command = command_from_index(aaaa[i]);
                P next = next_pos(x, y, command);
                if (next.first < 0 || next.first >= 5 || next.second < 0 || next.second >= 5)
                    continue;
                if (visited[next.first * N + next.second + (turn + 1) * 25].first == counter)
                    continue;
                if (can_move(x, y, command, turn, have_cargo, crane_id, traj, stack_state, row_empty_turn, to_pos, minimum_turn))
                {
                    int distance2 = abs(next.first - to_pos / 5) + abs(next.second - to_pos % 5);
                    int cost = turn + 1 + distance2 - (start_turn + tot_distance);
                    if (cost >= MAX_SIZE)
                        continue;
                    que[cost].push_back({turn + 1, next.first * N + next.second, x * N + y});
                    // pq.emplace(cost, turn + 1, next.first * N + next.second, x * N + y);
                }
            }
        }
    }

    /*
    while (!pq.empty())
    {
        auto [cost, turn, pos, prev_pos] = pq.top();
        if (turn - start_turn > tot_distance + 5)
            break;

        pq.pop();
        if (turn > 140)
            break;
        int x = pos / N;
        int y = pos % N;
        if (visited[pos][turn].first == counter)
            continue;
        visited[pos][turn] = {counter, prev_pos};
        if (pos == to_pos && turn >= minimum_turn && can_move(x, y, '.', turn, have_cargo, crane_id, traj, stack_state, row_empty_turn, to_pos, minimum_turn))
        {
            vector<char> result;
            while (turn != start_turn)
            {
                auto [_, prev_pos] = visited[pos][turn];
                result.push_back(infer_command(pos, prev_pos));
                pos = prev_pos;
                turn--;
            }
            reverse(result.begin(), result.end());
            string route = "";
            for (auto c : result)
                route += c;
            if (route.size() == tot_distance && tot_distance >= 2)
            {
                route_cache[from_pos * 25 + to_pos] = route;
            }
            return route;
        }
        rep(i, 5)
        {
            char command = command_from_index(aaaa[i]);
            P next = next_pos(x, y, command);
            if (next.first < 0 || next.first >= N || next.second < 0 || next.second >= N)
                continue;
            if (visited[next.first * N + next.second][turn + 1].first == counter)
                continue;
            if (can_move(x, y, command, turn, have_cargo, crane_id, traj, stack_state, row_empty_turn, to_pos, minimum_turn))
            {
                int distance2 = abs(next.first - to_pos / 5) + abs(next.second - to_pos % 5);
                int cost = turn + 1 + distance2 * 1.4;
                if (cost - start_turn > tot_distance + 5)
                    continue;
                pq.emplace(cost, turn + 1, next.first * N + next.second, x * N + y);
            }
        }
    }
    */
    return "noroute";
}




vec_int make_pos(int initial_pos, string commands)
{
    vec_int result;
    int x = initial_pos / N;
    int y = initial_pos % N;
    for (auto c : commands)
    {
        if (c == 'L')
            y--;
        if (c == 'R')
            y++;
        if (c == 'U')
            x--;
        if (c == 'D')
            x++;
        result.push_back(x * N + y);
    }
    return result;
}

// 深さ優先探索に沿って更新する情報をまとめたクラス
class BeamSearchState
{
private:
    vector<BeamSearchTask> tasks;
    BSTaskDependencyManager tdm;
    int task_id;
    RouteFinder rf;
    bool emergency;
    Cost rough_estimate_score(int _crane_id, int command_length)
    {
        int tmp_command_length = command_length_score + command_length;
        int raw_score = 0;
        // int min_turn = INT_MAX;
        rep(crane_id, 5)
        {
            int len = cranes[crane_id].operations.size();
            if (crane_id == _crane_id)
                len += command_length;
            raw_score = max(raw_score, len);
            // min_turn = min(min_turn, len);
        }

        // int diff = raw_score - min_turn;
        double progress = ((double)task_id + 1.) / (double)task_finish_state.drop_finish_turn.size();
        Cost next_score = tmp_command_length * (1 - progress) * 2.0 + (double)raw_score * (pow(progress, 0.3));
        return next_score;
    }

    int calc_muda_moves(Action &action, int start_pos, int container_id)
    {
        int distance1 = abs(start_pos / N - action.pickup_pos / N) + abs(start_pos % N - action.pickup_pos % N);
        int distance2 = abs(action.pickup_pos / N - action.destination / N) + abs(action.pickup_pos % N - action.destination % N);
        int result = action.command.size() - distance1 - distance2 - 2;
        int diffx = abs(start_pos / N - action.pickup_pos / N); // 縦方向の移動は全体的にだめ

        // 中継地点が縦方向の移動が大きいとだめ
        if (action.destination % 5 != 4)
        {
            int goal_x = container_id / 5;
            int pickup_x = action.pickup_pos / 5;
            int min_x = min(goal_x, pickup_x);
            int max_x = max(goal_x, pickup_x);
            int destination_x = action.destination / 5;
            if (destination_x < min_x || destination_x > max_x)
            {
                result += 2;
            }
        }
        return result;
    }

    pair<Cost, Hash> evaluate_score(Action &action)
    {
        Cost next_score = rough_estimate_score(action.crane_id, action.command.size());
        Hash hash_value = 0;
        /*
        if (task_id > 5)
        {
            hash_value += action.crane_id;
            hash_value *= 10;
            for (int i = 0; i < task_id && i < 6; i++)
            {
                hash_value += task_finish_state.assigned_crane[task_id - i - 1];
                hash_value *= 10;
            }
        }
        else
        {
            hash_value = ro.get_rand(0, 1000000);
        }
        */
        hash_value = ro.get_rand(0, 1000000);
        return {next_score, hash_value};
    }

public:
    CraneTrajectory traj;
    vector<CraneState> cranes;
    StackState stack_state;
    TaskFinishState task_finish_state;
    double command_length_score;
    double score;
    vec_int row_pop_count;
    vec_int row_empty_turn;
    vector<vec_int> occupied_positions;

    explicit BeamSearchState(vector<BeamSearchTask> &tasks) : tasks(tasks)
    {
        emergency = false;
        traj = CraneTrajectory();
        tdm = BSTaskDependencyManager(tasks);
        cranes = {CraneState(0), CraneState(1), CraneState(2), CraneState(3), CraneState(4)};
        stack_state = StackState();
        task_finish_state = TaskFinishState(tasks.size());
        score = 0; // スコアは(実行したコマンドの合計値)*(1-progress) + (最長のものの長さ*(progress+0.2)) など
        row_pop_count = vec_int(5, 0);
        row_empty_turn = vec_int(5, INT_MAX);
        task_id = 0;
        rf = RouteFinder();
    }

    // EvaluatorとHashの初期値を返す
    pair<Evaluator, Hash> make_initial_node()
    {
        return {Evaluator(0), 0};
    }

    // 次の状態候補を全てselectorに追加する
    // 引数
    //   evaluator : 今の評価器
    //   hash      : 今のハッシュ値
    //   parent    : 今のノードID（次のノードにとって親となる）
    void expand(const Evaluator &evaluator, Hash hash, int parent, Selector &selector)
    {
        if (emergency || get_time(false) > TIME_LIMIT * 0.98)
        {
            emergency = true;
            return;
        }
        auto &task = tasks[task_id];
        int pickup_minimum_turn = task_finish_state.get_task_pickup_start_turn(task_id, tdm);
        int drop_minimum_turn = task_finish_state.get_task_drop_start_turn(task_id, tdm);
        int container_id = task.container_id;
        int pickup_pos = task.from_pos;
        if (pickup_pos == -1)
        {
            int prev_task = tdm.pickup_dependent_task[task_id];
            pickup_pos = task_finish_state.drop_pos[prev_task];
        }

        set<int> skip_cranes;
        rep(aa, 5)
        {
            if (A[aa][0] == container_id && cranes[aa].operations.size() == 0)
            {
                rep(bb, 5)
                {
                    if (aa == bb)
                        continue;
                    skip_cranes.insert(bb);
                }
                break;
            }
        }

        rep(crane_id, 5)
        {
            if (skip_cranes.count(crane_id) > 0)
                continue;

            int start_turn = cranes[crane_id].operations.size();
            int current_pos = cranes[crane_id].last_pos.back();
            int distance1 = abs(current_pos / N - pickup_pos / N) + abs(current_pos % N - pickup_pos % N);
            int diff_turns = pickup_minimum_turn - start_turn;
            if (diff_turns > distance1 + 2)
            {
                continue;
            }

            int to_pos = task.to_pos;
            vector<P> destinations; // pos, drop_minimum_turn
            if (to_pos != -1)
            {
                destinations = {{to_pos, drop_minimum_turn}};
            }
            else
            {
                rep(i, STACK_COUNT)
                {
                    if (stack_state.can_push(i))
                    {
                        int x = pickup_pos / N;
                        int stack_pos_x = stack_pos[i] / N;
                        int dest_x = container_id / N;
                        int diffx = stack_pos_x - x;
                        int diffx2 = dest_x - stack_pos_x;
                        int tmp = stack_state.minimum_push_turn(i);
                        destinations.push_back({stack_pos[i], tmp});
                    }
                }
            }
            if (destinations.size() == 0)
            {
                continue;
            }
            for (auto row : destinations)
            {
                auto [destination, drop_minimum_turn] = row;
                int distance1 = abs(current_pos / N - pickup_pos / N) + abs(current_pos % N - pickup_pos % N);
                int distance2 = abs(pickup_pos / N - destination / N) + abs(pickup_pos % N - destination % N);
                int minimum_command_length = distance1 + distance2 + 2;
                string tmp = "";
                Cost rough_score = rough_estimate_score(crane_id, minimum_command_length);
                if (selector.current_worst_score() < rough_score)
                {
                    continue;
                }

                string route1 = rf.find_route(start_turn, current_pos, pickup_pos, crane_id, false, traj, stack_state, row_empty_turn, pickup_minimum_turn);
                if (route1 == "noroute")
                    continue;
                string route2 = rf.find_route(start_turn + route1.size() + 1, pickup_pos, destination, crane_id, true, traj, stack_state, row_empty_turn, drop_minimum_turn);
                string route = route1 + "P" + route2 + "Q";
                if (route1 == "noroute" || route2 == "noroute")
                    continue;
                // pickup_turnはあやしiかも
                auto action = Action(crane_id, pickup_pos, start_turn + route1.size(), destination, route);
                auto [next_score, next_hash] = evaluate_score(action);
                selector.push(Candidate(action, Evaluator(next_score), next_hash, parent), false);
            }
        }
    }

    // actionを実行して次の状態に遷移する
    void move_forward(Action action)
    {
        int crane_id = action.crane_id;
        int pickup_pos = action.pickup_pos;
        int destination = action.destination;
        string command = action.command;

        int start_turn = cranes[crane_id].operations.size();
        int start_pos = cranes[crane_id].last_pos.back();
        /*
         */
        // 1. pos_arrをcranes[crane_id].posの後ろに追加する
        vec_int pos_arr = make_pos(cranes[crane_id].last_pos.back(), command);
        cranes[crane_id].last_pos.push_back(destination);
        cranes[crane_id].operations += command;

        // 2. task_finish_stateの管理
        task_finish_state.task_finished(task_id, start_turn, command, destination, action.pickup_turn, crane_id);
        // 3. 途中にコンテナを置いたりしている場合の管理
        if (destination % 5 != 4)
            stack_state.drop_container(destination, start_turn + command.size());

        // 4. 途中のコンテナから拾ってきた場合の管理
        if (pickup_pos % 5 != 0)
            stack_state.pickup_container(pickup_pos, action.pickup_turn);

        // 5. コマンドlength_scoreの管理
        command_length_score += command.size();

        // 5-2 trajectoryの管理
        traj.finish_turn(start_turn, pos_arr, start_pos);

        // 6. 全部使い切っていた時の行の管理
        if (pickup_pos % 5 == 0)
        {
            row_pop_count[pickup_pos / 5]++;
            if (row_pop_count[pickup_pos / 5] == 5)
            {
                row_empty_turn[pickup_pos / 5] = action.pickup_turn;
            }
        }
        // 7. stackの場所を通過していた時の管理
        stack_state.finish_turn(start_pos, start_turn, crane_id, command);

        // 8. task_idの更新
        task_id += 1;
    }

    // actionを実行する前の状態に遷移する
    // 今の状態は、親からactionを実行して遷移した状態である
    void move_backward(Action action)
    {
        int crane_id = action.crane_id;
        int pickup_pos = action.pickup_pos;
        int destination = action.destination;
        string command = action.command;

        /*
         */
        // 1. cranenes[crane_id].posの後ろからcommand.size()分削除する
        cranes[crane_id].last_pos.pop_back();
        cranes[crane_id].operations.resize(cranes[crane_id].operations.size() - command.size());
        int start_turn = cranes[crane_id].operations.size();

        // 2. task_finish_stateの管理
        task_finish_state.undo_task_finished(task_id - 1, command, destination);
        // 3. 途中にコンテナを置いたりしている場合の管理
        if (destination % 5 != 4)
            stack_state.undo_drop_container(destination);

        // 4. 途中のコンテナから拾ってきた場合の管理
        if (pickup_pos % 5 != 0)
            stack_state.undo_pickup_container(pickup_pos);

        // 5. コマンドlength_scoreの管理

        command_length_score -= command.size();
        // 5-2 trajectoryの管理
        int start_pos = cranes[crane_id].last_pos.back();
        vec_int pos_arr = make_pos(start_pos, command);
        traj.undo_finish_turn(start_turn, pos_arr, start_pos);

        // 6. 全部使い切っていた時の行の管理
        if (pickup_pos % 5 == 0)
        {
            if (row_pop_count[pickup_pos / 5] == 5)
            {
                row_empty_turn[pickup_pos / 5] = -1;
            }
            row_pop_count[pickup_pos / 5]--;
        }
        // 7. stackの場所を通過していた時の管理
        stack_state.undo_finish_turn(cranes[crane_id].last_pos.back(), crane_id, command);

        // 8. task_idの更新
        task_id -= 1;
    }

private:
    // TODO
};

// Euler Tourを管理するためのクラス
class Tree
{
public:
    explicit Tree(const BeamSearchState &state, const Config &config) : state_(state)
    {
        curr_tour_.reserve(config.tour_capacity);
        next_tour_.reserve(config.tour_capacity);
        leaves_.reserve(config.beam_width * 2);
        buckets_.assign(50000, {});
    }

    // 状態を更新しながら深さ優先探索を行い、次のノードの候補を全てselectorに追加する
    void dfs(Selector &selector)
    {
        if (curr_tour_.empty())
        {
            // 最初のターン
            auto [evaluator, hash] = state_.make_initial_node();
            state_.expand(evaluator, hash, 0, selector);
            return;
        }

        for (auto [leaf_index, action] : curr_tour_)
        {
            // cerr << "dfs" << " " << leaf_index << " " << action.command << endl;
            if (leaf_index >= 0)
            {
                // 葉
                state_.move_forward(action);
                auto &[evaluator, hash] = leaves_[leaf_index];
                state_.expand(evaluator, hash, leaf_index, selector);
                state_.move_backward(action);
            }
            else if (leaf_index == -1)
            {
                // 前進辺
                state_.move_forward(action);
            }
            else
            {
                // 後退辺
                state_.move_backward(action);
            }
        }
    }

    // 木を更新する
    void update(const vector<Candidate> &candidates)
    {
        leaves_.clear();

        if (curr_tour_.empty())
        {
            // 最初のターン
            for (const Candidate &candidate : candidates)
            {
                curr_tour_.push_back({(int)leaves_.size(), candidate.action});
                leaves_.push_back({candidate.evaluator, candidate.hash});
            }
            return;
        }

        for (const Candidate &candidate : candidates)
        {
            buckets_[candidate.parent].push_back({candidate.action, candidate.evaluator, candidate.hash});
        }

        auto it = curr_tour_.begin();

        // 一本道を反復しないようにする
        while (it->first == -1 && it->second == curr_tour_.back().second)
        {
            Action action = (it++)->second;
            state_.move_forward(action);
            direct_road_.push_back(action);
            curr_tour_.pop_back();
        }

        // 葉の追加や不要な辺の削除をする
        while (it != curr_tour_.end())
        {
            auto [leaf_index, action] = *(it++);
            if (leaf_index >= 0)
            {
                // 葉
                if (buckets_[leaf_index].empty())
                {
                    continue;
                }
                next_tour_.push_back({-1, action});
                for (auto [new_action, evaluator, hash] : buckets_[leaf_index])
                {
                    int new_leaf_index = leaves_.size();
                    next_tour_.push_back({new_leaf_index, new_action});
                    leaves_.push_back({evaluator, hash});
                }
                buckets_[leaf_index].clear();
                next_tour_.push_back({-2, action});
            }
            else if (leaf_index == -1)
            {
                // 前進辺
                next_tour_.push_back({-1, action});
            }
            else
            {
                // 後退辺
                auto [old_leaf_index, old_action] = next_tour_.back();
                if (old_leaf_index == -1)
                {
                    next_tour_.pop_back();
                }
                else
                {
                    next_tour_.push_back({-2, action});
                }
            }
        }
        swap(curr_tour_, next_tour_);
        next_tour_.clear();
    }

    // 根からのパスを取得する
    vector<Action> calculate_path(int parent, int turn) const
    {
        if (curr_tour_.size() == 0)
            return {};

        vector<Action> ret = direct_road_;
        ret.reserve(turn);
        for (auto [leaf_index, action] : curr_tour_)
        {
            if (leaf_index >= 0)
            {
                if (leaf_index == parent)
                {
                    ret.push_back(action);
                    return ret;
                }
            }
            else if (leaf_index == -1)
            {
                ret.push_back(action);
            }
            else
            {
                ret.pop_back();
            }
        }

        throw runtime_error("not reach here");
    }

private:
    BeamSearchState state_;
    vector<pair<int, Action>> curr_tour_;
    vector<pair<int, Action>> next_tour_;
    vector<pair<Evaluator, Hash>> leaves_;
    vector<vector<tuple<Action, Evaluator, Hash>>> buckets_;
    vector<Action> direct_road_;
};

// ビームサーチを行う関数
vector<Action> exec_beam_search(const Config &config, const BeamSearchState &state, int target_time, int lower_width, int upper_width)
{
    Tree tree(state, config);

    // 新しいノード候補の集合
    Selector selector(config);

    float start_time = get_time(false);

    int tot_width = 0;
    for (int turn = 0; turn < config.max_turn; ++turn)
    {
        float epalsed_time = get_time(false) - start_time;
        // cerr << "turn=" << turn << " " << endl;
        // Euler Tourでselectorに候補を追加する
        if (turn > 7)
        {
            float res_time = target_time - get_time(false);
            int res_turn = config.max_turn - turn;
            float average_time = res_time / res_turn;
            float average_elapsed_time = epalsed_time / (turn - 2);
            float ratio = average_time / average_elapsed_time;
            int new_width = max(min((int)((tot_width / (turn - 2)) * ratio), min(upper_width, (int)(selector.beam_width * 1.1))), lower_width);
            // cerr << "ratio:" << ratio << " " << new_width << endl;

            selector.update_beam_width(new_width);
            // cerr << turn << " " << new_width << endl;
        }
        tot_width += selector.beam_width;

        tree.dfs(selector);

        if (selector.have_finished())
        {
            // ターン数最小化型の問題で実行可能解が見つかったとき
            Candidate candidate = selector.get_finished_candidates()[0];
            vector<Action> ret = tree.calculate_path(candidate.parent, turn + 1);
            ret.push_back(candidate.action);
            return ret;
        }

        // assert(!selector.select().empty());
        if (selector.select().empty())
        {
            return {};
        }

        if (turn == config.max_turn - 1)
        {
            // ターン数固定型の問題で全ターンが終了したとき
            Candidate best_candidate = selector.calculate_best_candidate();
            vector<Action> ret = tree.calculate_path(best_candidate.parent, turn + 1);
            ret.push_back(best_candidate.action);
            return ret;
        }

        if (get_time(false) > TIME_LIMIT * 0.97)
        {
            return {};
        }

        // 木を更新する
        auto selected = selector.select();
        tree.update(selector.select());

        selector.clear();
    }
    return {};

    throw runtime_error("not reach here");
}



double T0 = 500.;
double T1 = 0;

set<int> extract_container_ids(vector<P> &simulation_result)
{
    set<int> result;
    for (auto tmp : simulation_result)
    {
        int container_id = tmp.first;
        int destination = tmp.second;
        if (destination % 5 != 4)
            result.insert(container_id);
    }
    return result;
}

bool is_set_equal(set<int> a, set<int> b)
{
    if (a.size() != b.size())
        return false;
    for (auto tmp : a)
    {
        if (!b.count(tmp))
            return false;
    }
    return true;
}

vector<BeamSearchTask> prepare_tasks(vector<P> &simulation_result)
{
    vector<BeamSearchTask> tasks = vector<BeamSearchTask>(simulation_result.size());
    vector<vec_int> task_ids = vector<vec_int>(N * N);
    rep(i, simulation_result.size())
    {
        int container = simulation_result[i].first;
        task_ids[container].push_back(i);
        // cerr << i << " " << container << " " << best_simulation_result[i].second << endl;
    }
    vec_int A_inv = vec_int(N * N);
    rep(i, N) rep(j, N) A_inv[A[i][j]] = i * N + j;
    for (int i = 0; i < N * N; i++)
    {
        int task_id = task_ids[i][0];
        int to_pos = simulation_result[task_id].second;
        int from_pos = A_inv[i] / 5 * 5;

        if (task_ids[i].size() == 1)
        {
            tasks[task_id] = BeamSearchTask(from_pos, to_pos, i, true);
        }
        else
        {
            int task_id2 = task_ids[i][1];
            int from_pos2 = to_pos;
            int to_pos2 = simulation_result[task_id2].second;
            tasks[task_id] = BeamSearchTask(from_pos, -1, i, false);
            tasks[task_id2] = BeamSearchTask(-1, to_pos2, i, false);
        }
    }
    return tasks;
}

int main(int argc, char *argv[])
{
#ifdef OPTUNA_STUDY
    cerr << "optuna study, override parameters" << endl;
    T0 = stod(argv[1]);
    T1 = stod(argv[2]);
#endif
    get_time(true);
    cin >> N;
    rep(i, N) rep(j, N) cin >> A[i][j];

    int best_score = INT_MAX;
    vector<string> best_ans;
    vector<P> best_simulation_result;
    vector<pair<int, vector<P>>> simulation_results;
    int count = 0;
    while (simulation_results.size() < 100)
    {
        count++;
        if (get_time(false) > TIME_LIMIT * 0.05 && best_score < 150)
            break;
        auto annealer = Annealer();
        vector<string> ans;
        vector<P> tmp_best_simulation_result;
        try
        {
            // 以下の理由で実行時エラーになる
            // annealing結果のsimulation時のバグ(empty slotが足りない)
            // dijkstra_solverでタスクの依存関係の解決をする際に、運ぶ前のコンテナを見て判定してしまう場合
            auto simulation_result = annealer.annealing();
            if (simulation_result.size() == 0)
                continue;

            /**/
            int best_score2 = INT_MAX;
            rep(i, 1)
            {
                vector<string> ans1;
                try
                {
                    ans1 = solve_by_dijkstra_solver(simulation_result);
                }
                catch (...)
                {
                    continue;
                }
                int tmp_score = 0;
                /**/
                rep(j, 5)
                {
                    tmp_score = max(tmp_score, (int)ans1[j].size());
                }
                if (tmp_score < best_score2)
                {
                    best_score2 = tmp_score;
                    ans = ans1;
                    tmp_best_simulation_result = simulation_result;
                }
            }
        }
        catch (const exception &e)
        {
            cerr << "err:" << e.what() << endl;
            continue;
        }

        // ans[i]の最大の長さがスコアになる
        int score = 0;
        for (auto a : ans)
        {
            score = max(score, (int)a.size());
        }
        if (score <= 20)
            continue;
        // cerr << "score:" << score << endl;
        if (score < best_score)
        {
            best_score = score;
            best_ans = ans;
            best_simulation_result = tmp_best_simulation_result;
        }
        simulation_results.push_back({score, tmp_best_simulation_result});
    }
    sort(simulation_results.begin(), simulation_results.end());

    vector<int> TIME_LIMITS = {(int)(TIME_LIMIT * 0.30), (int)(TIME_LIMIT * 0.53), (int)(TIME_LIMIT * 0.75), (int)(TIME_LIMIT * 0.97)};
    int fail = 0;

    vector<set<int>> skip_results;
    bool skip_flag = false;

    rep(iii, 4)
    {
        bool success = false;
        if (!skip_flag)
        {
            for (int j = iii; j < simulation_results.size(); j++)
            {
                auto tmp = extract_container_ids(simulation_results[j].second);
                bool ok = true;
                rep(jj, skip_results.size())
                {
                    if (is_set_equal(tmp, skip_results[jj]))
                    {
                        ok = false;
                        break;
                    }
                }
                if (ok)
                {
                    best_simulation_result = simulation_results[j].second;
                    success = true;
                    break;
                }
            }
            if (!success)
            {
                skip_flag = true;
            }
        }
        if (skip_flag)
        {
            best_simulation_result = simulation_results[iii % simulation_results.size()].second;
        }
        auto extracted_container_ids = extract_container_ids(best_simulation_result);
        skip_results.push_back(extracted_container_ids);
        vector<BeamSearchTask> tasks = prepare_tasks(best_simulation_result);

        if (fail < 2)
        {
            tasks = shuffle_tasks(tasks);
        }

        auto state = BeamSearchState(tasks);
        auto config = Config(tasks.size(), 2200, 300000, 1000000);
        auto ret = exec_beam_search(config, state, TIME_LIMITS[iii], 1500, 8000);

        cerr << "end_time:" << get_time(false) << " ret size:" << ret.size() << endl;
        vector<string> ans(5, "");
        for (auto tmp : ret)
        {
            ans[tmp.crane_id] += tmp.command;
        }
        int score = 0;
        rep(i, 5)
        {
            score = max(score, (int)ans[i].size());
        }
        rep(i, 5)
        {
            if (ans[i].size() != score)
            {
                ans[i] += "B";
            }
        }
        if (score == 0)
        {
            fail++;
            continue;
        }
        if (score != 0 && score < best_score)
        {
            best_score = score;
            best_ans = ans;
        }
    }

    rep(i, 5)
    {
        cout << best_ans[i] << endl;
    }
    cerr << "score:" << best_score << endl;
    /*
    cout << tasks.size() << endl;
    rep(i, tasks.size())
    {
        cout << tasks[i].container_id << " " << tasks[i].from_pos << " " << tasks[i].to_pos << endl;
    }
    */

    return 0;
}
