
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

#define int long long
#define double float
using namespace std;
using Ps = pair<short, short>;
using vec_int = vector<int>;
using Pa = pair<int, int>;
using P2 = pair<Pa, Pa>;
using P3 = pair<float, int>;
using Tu = tuple<int, int, int>;
using T2 = tuple<float, int, int>;
using T3 = tuple<float, int, int, int>;
using T4 = tuple<int, int, int, int>;
using T5 = tuple<int, int, int, int, int>;
using TT = tuple<int, int, int>;
using ll = long long;
using uc = unsigned char;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)

int N, M, K, T;
vec_int t, w;
vec_int h, v;

/*
{
    'p1' : 0.979200402286481, 'p2' : 0.9904936917893274, 'p3' : 0.7561576976019443, 'p4' : 0.9275938673001729, 'p5' : 0.6529799420503083, 'p6' : 11.331831505192795, 'p7' : 11.989659363881369, 'p8' : 12.435412524276053, 'p9' : 1.8942802212530379, 'p10' : 3.8591425838398377, 'p11' : 2.172999720344823
}
*/
/*
double p1 = 0.9890664960097699; // プロジェクトの価値の割引率
double p2 = 0.9918874147934582; // プロジェクトのHPの割引率
double p3 = 0.8404152479045842; // プロジェクトがスコアに寄与する割合
double p4 = 0.927;              // 労働カードがスコアに寄与する割合
double p5 = 0.652;              // 荷重労働カードがスコアに寄与する割合
*/
double p1 = 0.9832212304661037;
double p2 = 0.9901457138823107;
double p3 = 0.844259904917072;
double p4 = 0.9292588019169842;
double p5 = 0.7003134476336773;
double p6 = 2.338417545738801;
double p7 = 1.7249324892918931;
double p8 = 6.8115379259266104;
double p9 = 1.3631873794223526;
double p10 = 3.8697761960124386;
double p11 = 2.2286034705987143;
double p12 = 10.059071995370422;
double p13 = 9.987114335199193;

double coeff1 = 7.253263277935437;
double coeff2 = 9.026058394775795;
double coeff3 = 0.9587959376647882;
double coeff4 = 0.8836159645282604;
double coeff5 = 10.181607485578471;
double coeff6 = 15.165444452119445;
double coeff7 = 1;
int loop_count = 20;
int turns_ahead = 1;
// #pragma GCC optimize("Ofast")


std::mt19937 mt{12345};
int RAND_ARR_LEN = 100000;
int RAND_RANGE = 1000000000;
float TIME_LIMIT = 5000;
float INV_TIME_LIMIT = 1. / 1000.;
const int JOB_MAX_LEN = 1003;
std::uniform_int_distribution<int> dist(1, RAND_RANGE);
std::normal_distribution<> normal_dist(0, 0.5);

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
public:
    int count = 0;
    vec_int rand_arr;
    vector<double> normal_arr;

    Rand(){
        rep(i, RAND_ARR_LEN){
            rand_arr.push_back(dist(mt));
    normal_arr.push_back(normal_dist(mt));
}
}
;
int get();
int get_rand(int from, int to);
double get_float();
double get_normal();
}
;

int Rand::get()
{
    int num = rand_arr.at(count);
    count += 1;
    count %= RAND_ARR_LEN;
    return num;
}
double Rand::get_normal()
{
    double num = normal_arr.at(count);
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

double Rand::get_float()
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




class Game
{
private:
public:
    Pa generate_a_new_project();
    Tu generate_a_new_card();
    int last_played_card_index = -1;
    int last_target_project = -1;
    int turn = 0;
    int money = 0;
    int L = 0;
    vec_int card_types;
    vec_int card_work_amount;
    vec_int card_prices;
    vec_int project_HPs;
    vec_int project_values;

    vec_int candidate_card_types;
    vec_int candidate_card_work_amounts;
    vec_int candidate_card_prices;
    vector<double> pop_counts;
    double tot_pop = 24.5;
    Game();

    void play_card(int played_card_id, int target_project);
    void update_projects_situation();
    void draw_card(int drawed_card_id);
    void update_candidate_cards();

    void play_card_sim(int played_card_id, int target_project);
    void update_projects_situation_sim();
    void draw_card_sim(int drawed_card_id);
    void update_candidate_cards_sim();

    vec_int card_played_count;
    vector<double> get_probabilities()
    {
        vector<double> result = vector<double>(5);
        rep(i, 5) result[i] = pop_counts[i] / tot_pop;
        return result;
    };
    double accumulated_money = 0;
    double total_work_amount = 0;
    double estimated_work_amount_per_turn() { return total_work_amount / ((double)turn + 1) * pow(2, L); }
    bool money_mode = false;
};

Game::Game()
{
    card_types.resize(N);
    card_work_amount.resize(N);
    card_prices.resize(N);
    project_HPs.resize(M);
    project_values.resize(M);
    card_played_count.resize(N);
    rep(i, N)
    {
        cin >> card_types[i] >> card_work_amount[i];
        card_prices[i] = 0;
    }
    rep(i, M) cin >> project_HPs[i] >> project_values[i];

    candidate_card_types.resize(K);
    candidate_card_work_amounts.resize(K);
    candidate_card_prices.resize(K);
    pop_counts = {10., 4., 4., 3., 1.5};
}

void Game::play_card(int played_card_id, int target_project)
{
    card_played_count[played_card_id]++;
    last_played_card_index = played_card_id;
    last_target_project = target_project;
    cout << played_card_id << " " << target_project << endl;
    if (card_types[played_card_id] == 4)
    {
        L++;
        accumulated_money *= 2;
    }
    if (card_types[played_card_id] == 0)
    {
        total_work_amount += card_work_amount[played_card_id] / pow(2, L);
    }
    else if (card_types[played_card_id] == 1)
    {
        total_work_amount += card_work_amount[played_card_id] * M / pow(2, L);
    }
    // init_scores();
}

void Game::update_projects_situation()
{
    rep(i, M)
    {
        cin >> project_HPs[i] >> project_values[i];
    }
    int old_money = 0;
    cin >> money;
    int diff = money - old_money;
    accumulated_money += diff;
}

void Game::update_candidate_cards()
{
    rep(i, K)
    {
        cin >> candidate_card_types[i] >> candidate_card_work_amounts[i] >> candidate_card_prices[i];
        if (i > 0)
        {
            pop_counts[candidate_card_types[i]] += 1;
        }
    }
    turn++;
    tot_pop += K - 1;
}

void Game::draw_card(int drawed_card)
{
    cout << drawed_card << endl;
    card_types[last_played_card_index] = candidate_card_types[drawed_card];
    card_work_amount[last_played_card_index] = candidate_card_work_amounts[drawed_card];
    card_prices[last_played_card_index] = candidate_card_prices[drawed_card];
}

void Game::play_card_sim(int played_card_id, int target_project)
{
    card_played_count[played_card_id]++;
    last_played_card_index = played_card_id;
    last_target_project = target_project;
    if (card_types[played_card_id] == 0)
    {
        project_HPs[target_project] -= card_work_amount[played_card_id];
        if (project_HPs[target_project] <= 0)
        {
            money += project_values[target_project];
        }
    }
    if (card_types[played_card_id] == 1)
    {
        for (int i = 0; i < M; i++)
        {
            project_HPs[i] -= card_work_amount[played_card_id];
            if (project_HPs[i] <= 0)
            {
                money += project_values[i];
            }
        }
    }
    if (card_types[played_card_id] == 2)
    {
        project_HPs[target_project] = 0;
    }
    if (card_types[played_card_id] == 3)
    {
        for (int i = 0; i < M; i++)
        {
            project_HPs[i] = 0;
        }
    }
    if (card_types[played_card_id] == 4)
    {
        L++;
    }
}

void Game::update_projects_situation_sim()
{
    rep(i, M)
    {
        if (project_HPs[i] <= 0)
        {
            auto [h, v] = generate_a_new_project();
            project_HPs[i] = h;
            project_values[i] = v;
        }
    }
}

void Game::update_candidate_cards_sim()
{
    candidate_card_types[0] = 0;
    candidate_card_work_amounts[0] = 1 << L;
    candidate_card_prices[0] = 0;
    for (int i = 1; i < K; i++)
    {
        auto [card_type, work_amount, price] = generate_a_new_card();
        candidate_card_types[i] = card_type;
        candidate_card_work_amounts[i] = work_amount;
        candidate_card_prices[i] = price;
        pop_counts[candidate_card_types[i]] += 1;
    }
    turn++;
    tot_pop += K - 1;
}

void Game::draw_card_sim(int drawed_card)
{
    card_types[last_played_card_index] = candidate_card_types[drawed_card];
    card_work_amount[last_played_card_index] = candidate_card_work_amounts[drawed_card];
    card_prices[last_played_card_index] = candidate_card_prices[drawed_card];
}

Pa Game::generate_a_new_project()
{
    double b = 2.0 + ro.get_float() * 6.0;
    int h = ((int)round(pow(2., b))) << L;
    double b2 = b + ro.get_normal();
    b2 = clamp(b2, (double)0.0, (double)10.0);
    int v = ((int)round(pow(2., b2))) << L;
    return Pa(h, v);
}

Tu Game::generate_a_new_card()
{
    double total_pop = 0;
    rep(i, 5) total_pop += pop_counts[i];
    int num = ro.get_rand(0, (int)total_pop);
    int accum = 0;
    int card_type = -1;
    rep(i, 5)
    {
        accum += pop_counts[i];
        if (accum >= num)
        {
            card_type = i;
            break;
        }
    }
    if (card_type == -1)
        throw runtime_error("card_type == -1");

    if (card_type == 0)
    {
        int w = ro.get_rand(1, 51);
        int p0 = round(w + ro.get_normal() * (2 * (double)w / 3.));
        int p = clamp(p0, (int)1, (int)10000) << L;
        w = w << L;

        // cerr << "card_type == 0"
        //     << " w:" << w << " p:" << p << " turn:" << turn << endl;
        return Tu(card_type, w, p);
    }
    if (card_type == 1)
    {
        int w = ro.get_rand(1, 51);
        int p0 = round(w * M + ro.get_normal() * (2 * (double)w * (double)M / 3.));
        int p = clamp(p0, (int)1, (int)10000) << L;
        w = w << L;

        // cerr << "card_type == 1"
        //     << " w:" << w << " p:" << p << endl;
        return Tu(card_type, w, p);
    }
    if (card_type == 2)
    {
        int p = ro.get_rand(0, 11) << L;

        // cerr << "card_type == 2"
        //     << " w:" << 0 << " p:" << p << endl;
        return Tu(card_type, 0, p);
    }
    if (card_type == 3)
    {
        int p = ro.get_rand(0, 11) << L;
        // cerr << "card_type == 3"
        //     << " w:" << 0 << " p:" << p << endl;
        return Tu(card_type, 0, p);
    }
    if (card_type == 4)
    {
        int p = ro.get_rand(200, 1001) << L;
        return Tu(card_type, 0, p);
    }
    throw runtime_error("must not reach here");
    /*
    int t = ro.get_rand(0, 5);
    int w = ro.get_rand(1, 10);
    int p = ro.get_rand(1, 10);
    */
}



class Evaluator
{
    Game *game;

public:
    Evaluator(Game *game) : game(game){};
    double simulate_next_turn_and_get_score(int card_id, int project_id, int card_to_draw);
    double get_score(vec_int &card_types, vec_int &card_work_amount, vec_int &project_HPs, vec_int &project_values, int money, int L, int played_card_id);
};

double Evaluator::simulate_next_turn_and_get_score(int card_id, int project_id, int card_to_draw)
{
    if (game->turn == 0 && card_to_draw > 0)
        return -1000000;
    int money = game->money;

    if (money - game->candidate_card_prices[card_to_draw] < 0)
        return -1000000;
    int L = game->L;
    vec_int card_types = game->card_types;
    vec_int card_work_amount = game->card_work_amount;
    // カードを引く
    if (game->turn > 0)
    {
        int last_card_index = game->last_played_card_index;
        card_types[last_card_index] = game->candidate_card_types[card_to_draw];
        card_work_amount[last_card_index] = game->candidate_card_work_amounts[card_to_draw];
        money -= game->candidate_card_prices[card_to_draw];
    }

    if (!(card_types[card_id] == 0 || card_types[card_id] == 2) && (project_id != 0))
        return -1000000;

    if (card_types[card_id] == 4 && game->L >= 20) // 増資は20回まで
        return -1000 * 1000;

    if (card_types[card_id] == 4 && game->turn > 880 && game->L > 0)
    {
        int average_turns = game->turn / (game->L);
        if (1000 - game->turn < average_turns)
        {
            return -1000000;
        }
    }

    vec_int project_HPs = game->project_HPs;
    vec_int project_values = game->project_values;

    int addition = 0;
    int cleared = 0;
    if (card_types[card_id] == 0)
    {
        project_HPs[project_id] -= card_work_amount[card_id];
        if (project_HPs[project_id] <= 0)
        {
            money += project_values[project_id];
            project_HPs[project_id] = coeff5 * pow(2, game->L);
            project_values[project_id] = coeff6 * pow(2, game->L);
            // project_values[project_id] = project_HPs[project_id];
            cleared++;
        }
    }
    if (card_types[card_id] == 1)
    {
        rep(i, M)
        {
            project_HPs[i] -= card_work_amount[card_id];
            if (project_HPs[i] <= 0)
            {
                money += project_values[i];
                project_HPs[i] = coeff1 * pow(2, game->L);
                project_values[i] = coeff1 * 2 * pow(2, game->L);
                cleared++;
            }
        }
    }
    if (cleared == 0 && card_types[card_id] <= 1 && game->turn > 998 && card_to_draw > 0)
        return -1000000;
    if (card_types[card_id] == 2)
    {
        project_HPs[project_id] = coeff2 * pow(2, game->L);
        project_values[project_id] = coeff2 * pow(2, game->L);
    }
    if (card_types[card_id] == 3)
    {
        rep(i, M)
        {
            project_HPs[i] = coeff2 * pow(2, game->L);
            project_values[i] = coeff2 * pow(2, game->L);
        }
    }
    if (card_types[card_id] == 4)
    {
        L += 1;
    }

    double ratio = 1;
    if (card_types[card_id] == 0)
    {
        if (game->project_HPs[project_id] > game->project_values[project_id] && card_work_amount[card_id] < game->project_HPs[project_id])
        {
            ratio = coeff3;
        }
        else if (game->project_HPs[project_id] > card_work_amount[card_id])
        {
            // 対象のプロジェクトidが有効なもののうち最もHPが低いやつかを調べる
            int min_HP = 10000000000;
            int min_project_id = -1;
            rep(i, M)
            {
                if (game->project_HPs[i] < card_work_amount[card_id])
                    continue;
                if (game->project_HPs[i] > game->project_values[i])
                    continue;

                if (game->project_HPs[i] < min_HP)
                {
                    min_HP = game->project_HPs[i];
                    min_project_id = i;
                }
            }
            if (min_project_id != project_id)
            {
                ratio = coeff4;
            }
        }
        if (game->project_HPs[project_id] <= card_work_amount[card_id] && game->project_values[project_id] < game->candidate_card_prices[card_to_draw])
        {
            ratio = coeff7;
        }
        /*
        if (game->turn > 960)
        {
            double estimated_work_amount = game->estimated_work_amount_per_turn();
            // 払いきれないやつを相手にしない
            int res_turn = 1000 - game->turn;
            if (game->project_HPs[project_id] > estimated_work_amount * res_turn * 0.5)
            {
                ratio = 0.3;
            }
        }
        */
    }
    if (card_types[card_id] == 1)
    {
        // 全てが有効でなかったらだめ
        int flag = 0;
        rep(i, M)
        {
            if (game->project_HPs[i] > game->project_values[i] && game->project_HPs[i] > card_work_amount[card_id])
                continue;

            flag = 1;
            break;
        }
        if (flag == 0)
        {
            ratio = 0.3;
        }
    }
    if (game->turn == 999 || game->money_mode)
        return money;

    return get_score(card_types, card_work_amount, project_HPs, project_values, money, L, card_id) * ratio + addition;
}

double Evaluator::get_score(vec_int &card_types, vec_int &card_work_amount, vec_int &project_HPs, vec_int &project_values, int money, int L, int played_card_id)
{
    // cerr << "get_score" << endl;
    int res_turn = 1000 - game->turn;
    int base_money = money >> L;
    double current_score = money;
    double tot_estimation_turn = 0;
    vector<double> project_calculated_values(M);
    int positive_project_count = 0;
    rep(i, M)
    {
        int estimate_turn = (double)project_HPs[i] / (double)p13 / pow(2, L);

        // プロジェクトの価値を計算する
        // double value = max((double)(-p12 * pow(2, L)), (double)((double)project_values[i] * pow(p1, estimate_turn) - project_HPs[i] * pow(p2, estimate_turn)));
        double value = max((double)(-p12 * pow(2, L)), (double)((double)project_values[i] * pow(p1, estimate_turn) - project_HPs[i] * pow(p2, estimate_turn)));
        /*
        if (project_HPs[i] > game->estimated_work_amount_per_turn() * res_turn && game->turn > 990)
        {
            value = 0;
        }
        */
        if (value > 0)
        {
            tot_estimation_turn += estimate_turn;
            positive_project_count++;
        }
        project_calculated_values[i] = value;
        // current_score += value * p3;
    }
    double ratio = 1.;
    rep(i, M)
    {
        // if (positive_project_count > 3 && project_calculated_values[i] < 0) // よいプロジェクトがたくさんあるときにはマイナスのプロジェクトの価値を考えなくてよい
        //    continue;
        current_score += project_calculated_values[i] * (p3)*ratio;
    }
    double earned_score = current_score;
    double card_score = 0;

    res_turn = max((int)0, (int)(res_turn - tot_estimation_turn * 0.0));
    vector<int> card_type_count(5, 0);
    rep(i, N)
    {
        if (i == played_card_id)
            continue;
        card_type_count[card_types[i]]++;
    }
    if (card_type_count[2] > 2)
    {
        current_score -= p6 * pow(2, L) * card_type_count[2] * pow(1.0, card_type_count[2]);
        earned_score -= p8 * M * pow(2, L) * card_type_count[2] * pow(1.0, card_type_count[2]);
    }

    if (card_type_count[3] > 2 || (card_type_count[3] > 1 && N <= 3))
    {
        current_score -= p7 * M * pow(2, L) * card_type_count[3] * pow(1.0, card_type_count[3]);
        earned_score -= p9 * M * pow(2, L) * card_type_count[3] * pow(1.0, card_type_count[3]);
    }
    rep(i, N)
    {
        if (i == played_card_id)
            continue;
        if (card_types[i] == 0)
        {
            current_score += (double)card_work_amount[i] * p4 * ratio;
            card_score += (double)card_work_amount[i] * p4 * ratio;
        }
        else if (card_types[i] == 1)
        {
            current_score += (double)card_work_amount[i] * M * p5 * ratio;
            card_score += (double)card_work_amount[i] * M * p5 * ratio;
        }
        else if (card_types[i] == 2)
        {
            current_score += p6 * pow(2, L);
            card_score += p6 * pow(2, L);
        }
        else if (card_types[i] == 3)
        {
            current_score += p7 * pow(2, L) * M;
            card_score += p7 * pow(2, L) * M;
        }
        else if (card_types[i] == 4)
        {
            current_score += 100 * pow(2, L);
            card_score += 100 * pow(2, L);
        }
    }
    rep(i, N)
    {
        if (i == played_card_id)
            continue;

        if (game->card_played_count[i] == 0)
            continue;

        if (card_types[i] == 0)
        {
            earned_score += (double)card_work_amount[i] * p4;
        }
        else if (card_types[i] == 1)
        {
            earned_score += (double)card_work_amount[i] * M * p5;
        }
        else if (card_types[i] == 2)
        {
            earned_score += p8 * pow(2, L);
        }
        else if (card_types[i] == 3)
        {
            earned_score += p9 * pow(2, L) * M;
        }
        else if (card_types[i] == 4)
        {
            earned_score += 100 * pow(2, L);
        }
    }

    if (card_types[played_card_id] == 4)
    {
        int penalty = 0;
        if (money < 100 * (2 << L))
            penalty = 10 * (2 << L);

        double result = money * p10 + current_score + (earned_score * (double)res_turn) / ((double)game->turn + 1.) * p11 - penalty;
        return result;
    }
    else
    {
        // if (game->turn > 990)
        //    return money;
        // cerr << earned_score << " " << game->accumulated_money << endl;
        return current_score + (earned_score * (double)res_turn) / ((double)game->turn + 1.);
    }
}



class DiffEvaluator
{
    Game *game;
    vec_int card_types;
    vec_int card_work_amount;
    vec_int candidate_card_types;
    vec_int candidate_card_work_amounts;
    vec_int candidate_card_prices;
    vec_int project_HPs;
    vec_int project_values;

    vector<double> project_scores;
    vector<double> card_scores;
    vector<double> card_scores2;
    vec_int card_type_counts = vec_int(5, 0);

    double try_get_score_card0(int card_id, int project_id, int card_to_draw);
    double try_get_score_card1(int card_id, int project_id, int card_to_draw);
    double try_get_score_card2(int card_id, int project_id, int card_to_draw);
    double try_get_score_card3(int card_id, int project_id, int card_to_draw);
    double try_get_score_card4(int card_id, int project_id, int card_to_draw);

    void update_cards(int last_played_card_id, int card_to_draw, int played_card_id);
    void undo_update_cards(int last_played_card_id, int card_to_draw, int played_card_id);

    int card_type_cache = -1;
    int card_work_amount_cache = -1;
    double card_score1_cache = -1;
    double card_score2_cache = -1;
    double calculate_card_score1(int card_type, int card_work_amount);
    double calculate_card_score2(int card_type, int card_work_amount, int played_card_id);
    void set_duplication_penalty(int played_card_id);

public:
    DiffEvaluator(Game *game);
    void update_projects();
    void update_candidates();
    double try_get_score(int card_id, int project_id, int card_to_draw, int last_played_card_id);
    void update_scores_full(int card_id, int project_id, int card_to_draw, int last_played_card_id);
    void init_scores();
    double score;
    double project_score;
    double card_score1;
    double card_score2;
    double card_count_penalty1;
    double card_count_penalty2;
};

void DiffEvaluator::set_duplication_penalty(int played_card_id)
{
    card_type_counts = vec_int(5, 0);
    rep(i, N)
    {
        if (i == played_card_id)
            continue;
        card_type_counts[card_types[i]]++;
    }

    card_count_penalty1 = 0;
    card_count_penalty2 = 0;
    if (card_type_counts[2] > 2)
    {
        card_count_penalty1 += p6 * pow(2, game->L) * card_type_counts[2];
        card_count_penalty2 += p8 * M * pow(2, game->L) * card_type_counts[2];
    }
    if (card_type_counts[3] > 2)
    {
        card_count_penalty1 += p7 * M * pow(2, game->L) * card_type_counts[3];
        card_count_penalty2 += p9 * M * pow(2, game->L) * card_type_counts[3];
    }
}

void DiffEvaluator::update_cards(int last_played_card_id, int card_to_draw, int played_card_id)
{
    if (last_played_card_id >= 0)
    {
        card_type_cache = card_types.at(last_played_card_id);
        card_work_amount_cache = card_work_amount.at(last_played_card_id);
        card_types.at(last_played_card_id) = candidate_card_types.at(card_to_draw);
        card_work_amount.at(last_played_card_id) = candidate_card_work_amounts.at(card_to_draw);
        card_score1_cache = card_scores.at(last_played_card_id);
        card_score2_cache = card_scores2.at(last_played_card_id);
        card_scores.at(last_played_card_id) = calculate_card_score1(card_types.at(last_played_card_id), card_work_amount.at(last_played_card_id));
        card_scores2.at(last_played_card_id) = calculate_card_score2(card_types.at(last_played_card_id), card_work_amount.at(last_played_card_id), last_played_card_id);
        card_score1 += card_scores.at(last_played_card_id) - card_score1_cache;
        card_score2 += card_scores2.at(last_played_card_id) - card_score2_cache;
    }
    set_duplication_penalty(played_card_id);
}

void DiffEvaluator::undo_update_cards(int last_played_card_id, int card_to_draw, int played_card_id)
{
    if (last_played_card_id >= 0)
    {
        card_score1 -= card_scores.at(last_played_card_id) - card_score1_cache;
        card_score2 -= card_scores2.at(last_played_card_id) - card_score2_cache;
        card_types.at(last_played_card_id) = card_type_cache;
        card_work_amount.at(last_played_card_id) = card_work_amount_cache;
        card_scores.at(last_played_card_id) = card_score1_cache;
        card_scores2.at(last_played_card_id) = card_score2_cache;
        set_duplication_penalty(played_card_id);
    }
}

// card_to_drawを引いて手札からplayed_card_idを出してproject_idに対して使ったときのスコアを更新する
void DiffEvaluator::update_scores_full(int played_card_id, int project_id, int card_to_draw, int last_played_card_id)
{
    int money = game->money;
    if (game->turn > 0)
    {
        update_cards(last_played_card_id, card_to_draw, played_card_id);
        money -= candidate_card_prices[card_to_draw];
    }
    int L = game->L;

    // プロジェクトの状況を更新する
    if (card_types[played_card_id] == 0)
    {
        project_HPs[project_id] -= card_work_amount[played_card_id];
        if (project_HPs[project_id] <= 0)
        {
            money += project_values[project_id];
            project_HPs[project_id] = 0;
            project_values[project_id] = 0;
        }
    }
    else if (card_types[played_card_id] == 1)
    {
        rep(i, M)
        {
            project_HPs[i] -= card_work_amount[played_card_id];
            if (project_HPs[i] <= 0)
            {
                money += project_values[i];
                project_HPs[i] = 0;
                project_values[i] = 0;
            }
        }
    }
    else if (card_types[played_card_id] == 2)
    {
        project_HPs[project_id] = 0;
        project_values[project_id] = 0;
    }
    else if (card_types[played_card_id] == 3)
    {
        rep(i, M)
        {
            project_HPs[i] = 0;
            project_values[i] = 0;
        }
    }
    else
    {
        L++;
    }

    project_score = 0;
    card_score1 = 0;
    card_score2 = 0;
    for (int i = 0; i < M; i++)
    {
        int estimate_turn = (double)project_HPs[i] / (double)p13 / pow(2, game->L);
        double value = max((double)(-p12 * pow(2, game->L)), (double)((double)project_values[i] * pow(p1, estimate_turn) - project_HPs[i] * pow(p2, estimate_turn)));
        project_scores[i] = value * p3;
    }

    card_type_counts = vec_int(5, 0);
    rep(i, N)
    {
        card_type_counts[card_types[i]]++;
    }
    card_count_penalty1 = 0;
    card_count_penalty2 = 0;
    if (card_type_counts[2] > 2)
    {
        card_count_penalty1 += p6 * pow(2, L) * card_type_counts[2];
        card_count_penalty2 += p8 * M * pow(2, L) * card_type_counts[2];
    }
    if (card_type_counts[3] > 2)
    {
        card_count_penalty1 += p7 * M * pow(2, L) * card_type_counts[3];
        card_count_penalty2 += p9 * M * pow(2, L) * card_type_counts[3];
    }

    rep(i, N)
    {
        if (i == played_card_id)
        {
            card_scores[i] = 0;
            continue;
        }
        card_scores[i] = calculate_card_score1(card_types[i], card_work_amount[i]);
    }

    rep(i, N)
    {
        if (i == played_card_id || game->card_played_count[i] == 0)
        {
            card_scores2[i] = 0;
            continue;
        }
        card_scores2[i] = calculate_card_score2(card_types[i], card_work_amount[i], i);
    }
    rep(i, M)
    {
        project_score += project_scores[i];
    }
    rep(i, N)
    {
        card_score1 += card_scores[i];
        card_score2 += card_scores2[i];
    }
    // if (game->turn == 0)
    //{
    // cout << "# aa project_score:" << project_score << " card_score1:" << card_score1 << " card_scores2:" << card_score2 << " money:" << money << endl;
    //}
    double current_score = money + project_score + card_score1 - card_count_penalty1;
    double earned_score = money + project_score + card_score2 - card_count_penalty2;
    int res_turn = 1000 - game->turn;
    if (card_types[played_card_id] == 4)
    {
        int penalty = 0;
        if (money < 100 * (2 << L))
            penalty = 10 * (2 << L);

        score = (double)(money * p10 + current_score + (earned_score * (double)res_turn) / ((double)game->turn + 1.) * p11);
        // cout << "# aa project_score:" << project_score << " card_score1:" << card_score1 << " card_scores2:" << card_score2 << " money:" << money << " penalty:" << penalty << " score:" << score << " turn:" << game->turn << endl;
        // cout << "# aa current_score:" << current_score << " earned_score:" << earned_score << " money:" << money << endl;
    }
    else
    {
        score = current_score + (earned_score * (double)res_turn) / ((double)game->turn + 1.);
    }

    undo_update_cards(last_played_card_id, card_to_draw, played_card_id);
}

void DiffEvaluator::update_candidates()
{
    candidate_card_types = game->candidate_card_types;
    candidate_card_work_amounts = game->candidate_card_work_amounts;
    candidate_card_prices = game->candidate_card_prices;
    card_types = game->card_types;
    card_work_amount = game->card_work_amount;
}
void DiffEvaluator::update_projects()
{
    project_HPs = game->project_HPs;
    project_values = game->project_values;
}

DiffEvaluator::DiffEvaluator(Game *game) : game(game)
{
    candidate_card_types = game->candidate_card_types;
    candidate_card_work_amounts = game->candidate_card_work_amounts;
    candidate_card_prices = game->candidate_card_prices;

    card_types = game->card_types;
    card_work_amount = game->card_work_amount;
    project_HPs = game->project_HPs;
    project_values = game->project_values;

    project_scores = vector<double>(M);
    card_scores = vector<double>(N);
    card_scores2 = vector<double>(N);
}

void DiffEvaluator::init_scores()
{
    /* 以下の5つのパラメータを初期化する*/
    project_score = 0;
    card_score1 = 0;
    card_score2 = 0;
    card_count_penalty1 = 0;
    card_count_penalty2 = 0;

    int money = game->money;
    int L = game->L;
    for (int i = 0; i < M; i++)
    {
        int estimate_turn = (double)project_HPs.at(i) / (double)p13 / pow(2, game->L);
        double value = max((double)(-p12 * pow(2, game->L)), (double)((double)project_values.at(i) * pow(p1, estimate_turn) - project_HPs.at(i) * pow(p2, estimate_turn)));
        project_scores.at(i) = value * p3;
    }

    card_type_counts = vec_int(5, 0);
    rep(i, N)
    {
        card_type_counts.at(card_types.at(i))++;
    }
    if (card_type_counts.at(2) > 2)
    {
        card_count_penalty1 += p6 * pow(2, L) * card_type_counts.at(2);
        card_count_penalty2 += p8 * M * pow(2, L) * card_type_counts.at(2);
    }
    if (card_type_counts.at(3) > 2)
    {
        card_count_penalty1 += p7 * M * pow(2, L) * card_type_counts.at(3);
        card_count_penalty2 += p9 * M * pow(2, L) * card_type_counts[3];
    }
    rep(i, N)
    {
        if (card_types[i] == 0)
        {
            card_scores[i] = (double)card_work_amount[i] * p4;
        }
        else if (card_types[i] == 1)
        {
            card_scores[i] = (double)card_work_amount[i] * M * p5;
        }
        else if (card_types[i] == 2)
        {
            card_scores[i] = p6 * pow(2, L);
        }
        else if (card_types[i] == 3)
        {
            card_scores[i] = p7 * pow(2, L) * M;
        }
        else if (card_types[i] == 4)
        {
            card_scores[i] = 100 * pow(2, L);
        }
    }

    rep(i, N)
    {
        if (game->card_played_count[i] == 0)
            continue;
        if (card_types[i] == 0)
        {
            card_scores2[i] = (double)card_work_amount[i] * p4;
        }
        else if (card_types[i] == 1)
        {
            card_scores2[i] = (double)card_work_amount[i] * M * p5;
        }
        else if (card_types[i] == 2)
        {
            card_scores2[i] = p8 * pow(2, L);
        }
        else if (card_types[i] == 3)
        {
            card_scores2[i] = p9 * pow(2, L) * M;
        }
        else if (card_types[i] == 4)
        {
            card_scores2[i] = 100 * pow(2, L);
        }
    }
    rep(i, M)
    {
        project_score += project_scores[i];
    }
    rep(i, N)
    {
        card_score1 += card_scores[i];
        card_score2 += card_scores2[i];
    }
}

double DiffEvaluator::calculate_card_score1(int card_type, int card_work_amount)
{
    if (card_type == 0)
    {
        return (double)card_work_amount * p4;
    }
    else if (card_type == 1)
    {
        return (double)card_work_amount * M * p5;
    }
    else if (card_type == 2)
    {
        return p6 * pow(2, game->L);
    }
    else if (card_type == 3)
    {
        return p7 * pow(2, game->L) * M;
    }
    else if (card_type == 4)
    {
        return 100 * pow(2, game->L);
    }
    throw "error";
}

double DiffEvaluator::calculate_card_score2(int card_type, int card_work_amount, int played_card_id)
{
    if (game->card_played_count[played_card_id] == 0)
    {
        return 0;
    }
    if (card_type == 0)
    {
        return (double)card_work_amount * p4;
    }
    else if (card_type == 1)
    {
        return (double)card_work_amount * M * p5;
    }
    else if (card_type == 2)
    {
        return p8 * pow(2, game->L);
    }
    else if (card_type == 3)
    {
        return p9 * pow(2, game->L) * M;
    }
    else if (card_type == 4)
    {
        return 100 * pow(2, game->L);
    }
    throw "error";
}

double DiffEvaluator::try_get_score(int card_id, int project_id, int card_to_draw, int last_played_card_id)
{

    if (game->turn == 0 && card_to_draw > 0)
        return -1000000;
    update_cards(last_played_card_id, card_to_draw, card_id);

    double result;
    if (!(card_types[card_id] == 0 || card_types[card_id] == 2) && project_id > 0)
    {
        result = -1e6;
    }
    else
    {
        if (card_types[card_id] == 0)
        {
            result = try_get_score_card0(card_id, project_id, card_to_draw);
        }
        else if (card_types[card_id] == 1)
        {
            result = try_get_score_card1(card_id, project_id, card_to_draw);
        }
        else if (card_types[card_id] == 2)
        {
            result = try_get_score_card2(card_id, project_id, card_to_draw);
        }
        else if (card_types[card_id] == 3)
        {
            result = try_get_score_card3(card_id, project_id, card_to_draw);
        }
        else
        {
            result = try_get_score_card4(card_id, project_id, card_to_draw);
            // result = pow(10, -12);
        }
    }

    undo_update_cards(last_played_card_id, card_to_draw, card_id);
    return result;
}

double DiffEvaluator::try_get_score_card0(int card_id, int project_id, int card_to_draw)
{
    // プロジェクトのスコアを更新しないといけない
    int money = game->money;
    if (game->turn > 0)
    {
        money -= candidate_card_prices[card_to_draw];
    }
    if (money < 0)
    {
        return -1e6;
    }
    // if (game->turn > 990)
    //     return money;
    double project_score_tmp = project_score;
    project_score_tmp -= project_scores[project_id];
    int new_hp = project_HPs[project_id] - card_work_amount[card_id];
    if (new_hp <= 0)
    {
        money += project_values[project_id];
        double h = coeff5 * pow(2, game->L);
        double v = coeff6 * pow(2, game->L);

        int turns = h / p13 / pow(2, game->L);
        double val = max((double)(-p12 * pow(2, game->L)), (double)((double)v * pow(p1, turns) - h * pow(p2, turns)));
        project_score_tmp += val * p3;
    }
    else
    {
        int estimate_turn = (double)new_hp / (double)p13 / pow(2, game->L);
        double value = max((double)(-p12 * pow(2, game->L)), (double)((double)project_values[project_id] * pow(p1, estimate_turn) - new_hp * pow(p2, estimate_turn)));
        project_score_tmp += value * p3;
    }
    if (game->turn > 997 && new_hp > 0)
        return -1e6;

    double current_score = money + project_score_tmp + (card_score1 - card_scores[card_id]) - card_count_penalty1;
    double earned_score = money + project_score_tmp + (card_score2 - card_scores2[card_id]) - card_count_penalty2;
    double result = current_score + earned_score * (double)(1000 - game->turn) / ((double)game->turn + 1.);
    // if (game->turn == 0)
    // {
    // cout << "# project scores:" << project_scores[0] << " " << project_scores[1] << endl;
    // cout << "# card scores2:" << card_scores2[0] << " " << card_scores2[1] << endl;
    // cout << "# project_score_tmp:" << project_score_tmp << " card_score1:" << card_score1 - card_scores[card_id] << " card_scores2:" << card_score2 - card_scores2[card_id] << " money:" << money << endl;
    // cout << "# current score:" << current_score << " earned score:" << earned_score << " money:" << money << endl;
    //}

    double ratio = 1;
    if (game->project_HPs[project_id] > game->project_values[project_id] && card_work_amount[card_id] < game->project_HPs[project_id])
    {
        ratio = coeff3;
    }
    else if (game->project_HPs[project_id] > card_work_amount[card_id])
    {
        // 対象のプロジェクトIDが有効なもののうち最もHPが低いやつかを調べる
        int min_HP = 10000000000;
        int min_project_id = -1;
        rep(i, M)
        {
            if (game->project_HPs[i] < card_work_amount[card_id])
                continue;
            if (game->project_HPs[i] > game->project_values[i])
                continue;

            if (game->project_HPs[i] < min_HP)
            {
                min_HP = game->project_HPs[i];
                min_project_id = i;
            }
        }
        if (min_project_id != project_id)
        {
            ratio = coeff4;
        }
    }
    // 一撃で倒せる & カードの取得価格より価値が小さい場合はno
    if (game->project_HPs[project_id] <= card_work_amount[card_id] && game->project_values[project_id] < game->candidate_card_prices[card_to_draw])
    {
        ratio = coeff7;
    }

    return result * ratio;
}

double DiffEvaluator::try_get_score_card1(int card_id, int project_id, int card_to_draw)
{
    // プロジェクトのスコアを更新しないといけない
    int money = game->money;
    if (game->turn > 0)
    {
        money -= candidate_card_prices[card_to_draw];
    }
    if (money < 0)
    {
        return -1e6;
    }
    // if (game->turn > 990)
    //    return money;
    double project_score_tmp = project_score;
    int cleared = 0;
    for (int i = 0; i < M; i++)
    {
        {
            project_score_tmp -= project_scores[i];
            int new_hp = project_HPs[i] - card_work_amount[card_id];
            if (new_hp <= 0)
            {
                money += project_values[i];
                double h = coeff1 * pow(2, game->L);
                double v = coeff1 * 2 * pow(2, game->L);

                int turns = h / p13 / pow(2, game->L);
                // double val = v - h;
                double val = max((double)(-p12 * pow(2, game->L)), (double)((double)v * pow(p1, turns) - h * pow(p2, turns)));
                project_score_tmp += val * p3;

                cleared++;
            }
            else
            {
                int estimate_turn = (double)new_hp / (double)p13 / pow(2, game->L);
                double value = max((double)(-p12 * pow(2, game->L)), (double)((double)project_values[i] * pow(p1, estimate_turn) - new_hp * pow(p2, estimate_turn)));
                project_score_tmp += value * p3;
            }
        }
    }
    if (cleared == 0 && game->turn > 997)
        return -1e6;

    // 新しくセットした分の値をひく
    /*
    double hp = 8 * pow(2, game->L);
    double value = 16 * pow(2, game->L);
    int turns = hp / p13 / pow(2, game->L);
    double tmp_score = max((double)(-p12 * pow(2, game->L)), (double)((double)value * pow(p1, turns) - hp * pow(p2, turns)));
    project_score_tmp += tmp_score * p3 * cleared;
    */

    double current_score = money + project_score_tmp + (card_score1 - card_scores[card_id]) - card_count_penalty1;
    double earned_score = money + project_score_tmp + (card_score2 - card_scores2[card_id]) - card_count_penalty2;
    return current_score + earned_score * (double)(1000 - game->turn) / ((double)game->turn + 1.);
}

double DiffEvaluator::try_get_score_card2(int card_id, int project_id, int card_to_draw)
{
    // プロジェクトのスコアを更新しないといけない
    int money = game->money;
    if (game->turn > 0)
    {
        money -= candidate_card_prices[card_to_draw];
    }
    if (money < 0)
    {
        return -1e6;
    }

    // if (game->turn > 990)
    //    return money;
    double project_score_tmp = project_score;
    project_score_tmp -= project_scores[project_id];

    // 新しくセットした分の値をひく
    double hp = coeff2 * pow(2, game->L);
    double value = coeff2 * pow(2, game->L);
    int turns = hp / p13 / pow(2, game->L);
    double tmp_score = max((double)(-p12 * pow(2, game->L)), (double)((double)value * pow(p1, turns) - hp * pow(p2, turns)));

    project_score_tmp += tmp_score * p3;

    double current_score = money + project_score_tmp + (card_score1 - card_scores[card_id]) - card_count_penalty1;
    double earned_score = money + project_score_tmp + (card_score2 - card_scores2[card_id]) - card_count_penalty2;

    return current_score + earned_score * (double)(1000 - game->turn) / ((double)game->turn + 1.);
}

double DiffEvaluator::try_get_score_card3(int card_id, int project_id, int card_to_draw)
{
    // プロジェクトのスコアを更新しないといけない
    int money = game->money;
    if (game->turn > 0)
    {
        money -= candidate_card_prices[card_to_draw];
    }
    if (money < 0)
    {
        return -1e6;
    }

    // if (game->turn > 990)
    //        return money;
    double project_score_tmp = 0;
    // project_score_tmp -= (double)M * 0.4 * (double)(1 << game->L);
    double hp = coeff2 * pow(2, game->L);
    double value = coeff2 * pow(2, game->L);
    int turns = hp / p13 / pow(2, game->L);
    double tmp_score = max((double)(-p12 * pow(2, game->L)), (double)((double)value * pow(p1, turns) - hp * pow(p2, turns)));
    project_score_tmp += tmp_score * p3 * M;

    double current_score = money + project_score_tmp + (card_score1 - card_scores[card_id]) - card_count_penalty1;
    double earned_score = money + project_score_tmp + (card_score2 - card_scores2[card_id]) - card_count_penalty2;
    return current_score + earned_score * (double)(1000 - game->turn) / ((double)game->turn + 1.);
}

double DiffEvaluator::try_get_score_card4(int card_id, int project_id, int card_to_draw)
{
    // プロジェクトのスコアを更新しないといけない
    int money = game->money;
    if (game->turn > 0)
    {
        money -= candidate_card_prices[card_to_draw];
    }
    if (money < 0 || game->L >= 20)
    {
        return -1e6;
    }
    if (game->turn > 880 && game->L > 0)
    {
        int average_turns = game->turn / (game->L);
        if (1000 - game->turn < average_turns)
        {
            return -1e6;
        }
    }

    int penalty = 0;
    if (money < 100 * (2 << (game->L + 1)))
        penalty = 10 * (2 << (game->L + 1));

    double current_score = money + project_score + (card_score1 - card_scores[card_id]) - card_count_penalty1;
    double earned_score = money + project_score + (card_score2 - card_scores2[card_id]) - card_count_penalty2;
    cout << "# project scores:" << project_scores[0] << " " << project_scores[1] << endl;
    cout << "# card scores2:" << card_scores2[0] << " " << card_scores2[1] << endl;
    cout << "# project_score_tmp:" << project_score << " card_score1:" << card_score1 - card_scores[card_id] << " card_scores2:" << card_score2 - card_scores2[card_id] << " money:" << money << " penalty:" << penalty << endl;
    cout << "# current score:" << current_score << " earned score:" << earned_score << " money:" << money << endl;
    double result = money * p10 + current_score + (earned_score * (double)(1000 - game->turn)) / ((double)game->turn + 1.) * p11 - penalty;
    cout << "# score:" << result << " turn:" << game->turn << endl;

    return result;
}



class Simulator
{
public:
    static double simulate_5_turns(Game game, int card_id, int project_id, int card_to_draw);
};

double Simulator::simulate_5_turns(Game game, int card_id, int project_id, int card_to_draw)
{
    // int turns = 2;
    int turns = turns_ahead;
    Evaluator evaluator = Evaluator(&game);
    vector<Tu> candidates;
    int index = 0;
    rep(i, (K - 1) * (turns + 1))
    {
        candidates.push_back(game.generate_a_new_card());
    }

    if (game.turn > 0)
    {
        game.draw_card_sim(card_to_draw);
    }
    game.play_card_sim(card_id, project_id);
    // game.update_candidate_cards_sim();
    rep(i, K - 1)
    {
        auto [card_type, card_amount, card_price] = candidates[index];
        game.candidate_card_types[i + 1] = card_type;
        game.candidate_card_work_amounts[i + 1] = card_amount;
        game.candidate_card_prices[i + 1] = card_price;
        index++;
    }
    game.turn = game.turn + 1;
    game.update_projects_situation_sim();
    if (game.turn == 1000)
    {
        return -pow(10, 12);
    }
    DiffEvaluator diff_evaluator = DiffEvaluator(&game);

    diff_evaluator.update_projects();
    diff_evaluator.update_candidates();
    diff_evaluator.init_scores();

    rep(turn, turns)
    {
        int best_draw_card_id = -1;
        int best_play_card_id = -1;
        int best_project_id = -1;
        double best_score = -pow(10, 12);
        priority_queue<T4> pq;
        rep(i, K) // 引くカード
        {
            vec_int used_card_types = vec_int(5);
            rep(j, N) // カードのID
            {
                int card_type = game.card_types[j];
                if (j == game.last_played_card_index)
                {
                    card_type = game.candidate_card_types[i];
                }
                if ((card_type >= 2) && used_card_types[card_type] > 0)
                {
                    continue;
                }
                used_card_types[card_type]++;

                rep(k, M) // プロジェクト
                {
                    if (!(card_type == 0 || card_type == 2) && k != 0)
                        break;
                    // double score = evaluator.simulate_next_turn_and_get_score(j, k, i);
                    double score;
                    if (card_type != 4)
                    {
                        score = diff_evaluator.try_get_score(j, k, i, game.last_played_card_index);
                    }
                    else
                    {
                        score = evaluator.simulate_next_turn_and_get_score(j, k, i);
                    }

                    if (score > best_score)
                    {
                        best_score = score;
                        best_draw_card_id = i;
                        best_play_card_id = j;
                        best_project_id = k;
                    }
                }
            }
        }
        if (turn == turns - 1 || game.turn >= 1000)
            return best_score;
        game.draw_card_sim(best_draw_card_id);
        game.play_card_sim(best_play_card_id, best_project_id);
        game.update_projects_situation_sim();
        // game.update_candidate_cards_sim();
        rep(i, K - 1)
        {
            auto [card_type, card_amount, card_price] = candidates[index];
            game.candidate_card_types[i + 1] = card_type;
            game.candidate_card_work_amounts[i + 1] = card_amount;
            game.candidate_card_prices[i + 1] = card_price;
            index++;
        }
        game.turn = game.turn + 1;

        // diff_evaluator.update_scores_full(best_play_card_id, best_project_id, best_draw_card_id, game.last_played_card_index);
        //    diff_evaluator.update_candidates();
        // diff_evaluator.init_scores();
        diff_evaluator = DiffEvaluator(&game);

        diff_evaluator.update_projects();
        diff_evaluator.update_candidates();
        diff_evaluator.init_scores();
    }
    throw runtime_error("simulate_5_turns failed");
}


signed main(int argc, char *argv[])
{
    get_time(true);
    cin >> N >> M >> K >> T;

#ifdef OPTUNA_STUDY
    cerr << "optuna study" << endl;

#endif
    int candidate_count = 4;
    if (N * M * K > 150)
        candidate_count = 3;

    Game game = Game();
    Evaluator evaluator = Evaluator(&game);

    int max_money = 0;
    vec_int highest_timings = {0};
    float loop_count_sum = 0;
    double average_highest_interval = 0;

    while (game.turn < T)
    {
        float ct = get_time(false);
        int res_turn = 1000 - game.turn;
        if (game.turn > 10 && ct < 1800)
        {
            // 今までかかった時間を考慮して、loop_countを調整する
            float res_time = 1800 - ct;
            float one_loop_time = ct / (float)game.turn;
            float ideal_one_loop_time = res_time / (float)res_turn;
            float loop_count_average = loop_count_sum / (float)game.turn;
            loop_count = loop_count_average * ideal_one_loop_time / one_loop_time;
            loop_count = clamp(loop_count, (int)16, min((int)200, (int)(loop_count_average * 1.5)));
            if (N * M * K > 150)
                loop_count = min(loop_count, (int)50);
        }
        loop_count_sum += loop_count;

        int best_draw_card_id = -1;
        int best_play_card_id = -1;
        int best_project_id = -1;
        double best_score = -pow(10, 12);
        priority_queue<T3> pq;
        rep(i, K) // 引くカード
        {
            vector<int> used_card_types = vector<int>(5);
            rep(j, N) // プレイするカードのID
            {
                int card_type = game.card_types[j];
                if (j == game.last_played_card_index)
                    card_type = game.candidate_card_types[i];

                if ((card_type >= 2) && used_card_types[card_type] > 0)
                    continue;
                used_card_types[card_type]++;

                rep(k, M) // プロジェクト
                {
                    double score = evaluator.simulate_next_turn_and_get_score(j, k, i);
                    if (score > 0)
                    {
                        pq.emplace(score, -i, -j, -k);
                    }
                    if (score > best_score)
                    {
                        best_score = score;
                        best_draw_card_id = i;
                        best_play_card_id = j;
                        best_project_id = k;
                    }
                }
            }
        }
        int a = best_draw_card_id;
        int b = best_play_card_id;
        int c = best_project_id;
        set<int> play_card_ids;
        if (get_time(false) < 1800)
        {
            double best_future_score2 = 0;
            int selected = -1;
            vec_int random_seeds;
            vector<T3> candidates;
            while (pq.size() > 0 && candidates.size() < candidate_count)
            {
                auto [score, draw_card_id, play_card_id, project_id] = pq.top();
                pq.pop();
                draw_card_id = -draw_card_id;
                play_card_id = -play_card_id;
                project_id = -project_id;
                if (play_card_ids.count(play_card_id) > 0)
                    continue;
                play_card_ids.insert(play_card_id);
                candidates.push_back({score, draw_card_id, play_card_id, project_id});
            }
            if (candidates.size() == 1)
                goto OUT1;
            rep(i, candidates.size())
            {
                auto [score, draw_card_id, play_card_id, project_id] = candidates[i];
                double score_sum = 0;
                rep(j, loop_count)
                {

                    if (get_time(false) > 1830)
                        goto OUT1;
                    if (i == 0)
                    {
                        random_seeds.push_back(ro.count);
                    }
                    else
                    {
                        ro.count = random_seeds[j];
                    }
                    double tmp = Simulator::simulate_5_turns(game, play_card_id, project_id, draw_card_id);
                    score_sum += tmp;
                }

                if (score_sum * score > best_future_score2)
                {
                    best_future_score2 = score_sum * score;
                    best_draw_card_id = draw_card_id;
                    best_play_card_id = play_card_id;
                    best_project_id = project_id;
                    selected = i;
                }
            }
            // cerr << "selected:" << selected << endl;
        }
    OUT1:

        if (game.turn > 0)
        {
            game.draw_card(best_draw_card_id);
        }

        game.play_card(best_play_card_id, best_project_id);
        game.update_projects_situation();
        game.update_candidate_cards();
        if (max_money < game.money)
        {
            max_money = game.money;
            highest_timings.push_back(game.turn);

            double tot = 0;
            rep(i, highest_timings.size() - 1)
            {
                tot += highest_timings[i + 1] - highest_timings[i];
            }
            average_highest_interval = tot / (double)(highest_timings.size() - 1);
        }
    }

    vector<double> probabilities = game.get_probabilities();
    rep(i, 5)
    {
        cout << "# card_type:" << i << " " << probabilities[i] << endl;
    }
    cerr << "max_money:" << max_money << endl;
    cerr << "time:" << get_time(false) << endl;

    cout << 0 << endl; // これは最後のターンに0を引くコマンド
    return 0;
}
