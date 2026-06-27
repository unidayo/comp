#include "bits/stdc++.h"

using namespace std;
#define REP(a, b) for (long long a = 0; a < b; ++a)

#define ld long double
#define ALL(a) begin(a), end(a)

#define int long long
unsigned long xor128()
{
    static unsigned long x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    unsigned long t = (x ^ (x << 11));
    x = y;
    y = z;
    z = w;
    return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

typedef struct Op
{
    int op_num;
    int uplift;
} Op;

typedef struct Solution
{
    vector<Op> ops;
} Solution;

const int dx[4] = {-1, 1, 0, 0};

const int moveR = 2;
const int moveL = 3;
const int moveU = 0;
const int moveD = 1;

pair<int, int> move_pos(pair<int, int> pos, int dir)
{
    return {pos.first + dx[dir], pos.second + dx[3 - dir]};
}

void print_solution(Solution sol)
{
    for (auto x : sol.ops)
    {
        if (x.op_num == 0)
        {
            cout << "U" << endl;
        }
        else if (x.op_num == 1)
        {
            cout << "D" << endl;
        }
        else if (x.op_num == 2)
        {
            cout << "R" << endl;
        }
        else if (x.op_num == 3)
        {
            cout << "L" << endl;
        }
        else if (x.op_num == 4)
        {
            if (x.uplift < 0)
                cout << "-" << -x.uplift << endl;
            else
                cout << "+" << x.uplift << endl;
        }
    }
    exit(0);
}
Solution compress(Solution now)
{
    Solution res = {};
    int pluser = 0;
    for (int i = 0; i < now.ops.size(); i++)
    {
        if (now.ops[i].op_num == 4)
        {
            pluser += now.ops[i].uplift;
        }
        else
        {
            if (pluser != 0)
            {
                res.ops.push_back({4, pluser});
                pluser = 0;
            }
            res.ops.push_back(now.ops[i]);
        }
    }
    if (pluser != 0)
    {
        res.ops.push_back({4, pluser});
    }

    while (res.ops.size() > 0 && res.ops.back().op_num != 4)
    {
        res.ops.pop_back();
    }
    return res;
}

int evaluation(Solution sol)
{
    int now = 0;
    int ans = 0;
    for (auto x : sol.ops)
    {
        if (x.op_num == 4)
        {
            ans += abs(x.uplift);
            now += x.uplift;
        }
        else
        {
            ans += now + 100;
        }
    }
    return ans;
}

const int n = 20;
vector<vector<int>> load_input()
{
    int t;
    cin >> t;
    vector<vector<int>> inputs(n, vector<int>(n, 0));
    REP(i, n)
    REP(j, n)
    cin >> inputs[i][j];
    return inputs;
}

Solution solve_by_greedy(vector<vector<int>> inputs, int initializer)
{
    Solution sol = {};
    int now_tank = 0;
    int x = 0, y = 0;

    vector<Op> tmp_ops;
    int required_add = 0;
    int prev_x = 0, prev_y = 9;
    auto uplift_tank = [&](int x, int y)
    {
        if (inputs[x][y] > 0)
        {
            tmp_ops.push_back({4, inputs[x][y]});
            now_tank += inputs[x][y];
            inputs[x][y] = 0;
        }
        else if (inputs[x][y] < 0)
        {
            int target = -inputs[x][y];
            if (target != 0)
            {
                tmp_ops.push_back({4, -target});
                now_tank -= target;
                inputs[x][y] += target;
                required_add = max(required_add, -now_tank);
            }
        }
    };
    auto narashi = [&](int x, int y, int opt = 0)
    {
        if (required_add > 0)
        {
            sol.ops.push_back({4, required_add});
            now_tank += required_add;
            inputs[prev_x][prev_y] -= required_add;
        }
        int now_x = x;
        int now_y = y;
        int now_stocks = required_add;
        int not_downed = 1;

        prev_x = x;
        prev_y = y;

        int placepoint = 0;
        int to_drawn = 0;

        for (int T = 0; T < tmp_ops.size(); ++T)
        {
            auto x = tmp_ops[T];
            sol.ops.push_back(x);
            if (x.op_num == 0)
            {
                now_x--;
            }
            else if (x.op_num == 1)
            {
                now_x++;
                not_downed = 0;
            }
            else if (x.op_num == 2)
            {
                now_y++;
            }
            else if (x.op_num == 3)
            {
                now_y--;
            }
            if (x.op_num == 4)
            {
                now_stocks += x.uplift;
            }
            else
            {
                if (opt == 1)
                {
                    if ((1 < now_y && now_y <= 5) || (now_y >= 14 && now_y < 18))
                    {
                        if (not_downed == 1)
                        {
                            // 8回移動を節約 = diff * 8
                            // 追加コスト 200, 1回の往復コスト 2 * now_stock - diff, 再配置コスト diff * 2
                            // = diff * 8 - (200 + 2 * stock + diff) = diff * 7 - (200 + 2*stock)得
                            int np = 0;
                            int cannot_use = 0;
                            int px = now_x;
                            int py = now_y;
                            for (int pe = T + 1; pe < tmp_ops.size(); ++pe)
                            {
                                auto x = tmp_ops[pe];
                                if (x.op_num == 0)
                                {
                                    px--;
                                }
                                else if (x.op_num == 1)
                                {
                                    px++;
                                }
                                else if (x.op_num == 2)
                                {
                                    py++;
                                }
                                else if (x.op_num == 3)
                                {
                                    py--;
                                }
                                else if (x.op_num == 4)
                                {
                                    np += x.uplift;
                                    cannot_use = max(cannot_use, -np);
                                }
                                if (py == now_y || py == now_y)
                                {
                                    if (px != now_x)
                                        break;
                                }
                            }
                            int diff = now_stocks - cannot_use;
                            int steps = min(now_y, 19 - now_y);
                            if (to_drawn == 0 && diff * (steps * 2 - 1) - (200 + 2 * now_stocks) > 0)
                            {
                                sol.ops.push_back({moveD, 0});
                                sol.ops.push_back({4, -diff});
                                sol.ops.push_back({moveU, 0});
                                to_drawn = diff;
                                placepoint = now_y;
                            }
                        }
                        else
                        {
                            if (to_drawn != 0 && placepoint == now_y)
                            {
                                sol.ops.push_back({4, to_drawn});
                                to_drawn = 0;
                                placepoint = -1;
                            }
                        }
                    }
                }
            }
        }
        if (opt == 1)
            cerr << "----" << endl;
        else
        {
            cerr << "oooo" << endl;
        }
        tmp_ops.clear();
        required_add = 0;
        if (now_tank > 0)
        {
            sol.ops.push_back({4, -now_tank});
            inputs[x][y] += now_tank;
            now_tank = 0;
        }
    };

    auto finalize_tank = [&](int x, int y, int max_add = 1e18)
    {
        if (inputs[x][y] > 0)
        {
            int T = min(inputs[x][y], max_add);
            sol.ops.push_back({4, T});
            now_tank += T;
            inputs[x][y] -= T;
        }
        else if (inputs[x][y] < 0)
        {
            int target = min(-inputs[x][y], now_tank);
            if (target != 0)
            {
                sol.ops.push_back({4, -target});
                now_tank -= target;
                inputs[x][y] += target;
            }
        }
    };

    // (0, 0) -> (0,9) -> (0,0) -> (0, 9)
    if (initializer == 0)
    {
        {
            while (y != 9)
            {
                finalize_tank(x, y);
                sol.ops.push_back({moveR, 0});
                finalize_tank(x, y);
                y++;
            }

            while (y != 0)
            {
                uplift_tank(x, y);
                int ok = 1;
                for (int j = y - 1; j >= 0; --j)
                {
                    if (inputs[0][j] != 0)
                    {
                        ok = 0;
                    }
                }
                if (ok == 1)
                    break;
                tmp_ops.push_back({moveL, 0});
                y--;
                uplift_tank(x, y);
            }
            while (y != 9)
            {
                uplift_tank(x, y);
                tmp_ops.push_back({moveR, 0});
                y++;
                uplift_tank(x, y);
            }
            narashi(x, y);
        }
    }
    else
    {
        int req = 0;
        int now_go = 0;
        for (int j = 0; j < 9; ++j)
        {
            req += inputs[0][j];
            now_go = max(now_go, -req);
        }
        if (now_go > 0)
        {
            sol.ops.push_back({moveD, 0});
            sol.ops.push_back({4, now_go});
            sol.ops.push_back({moveU, 0});
            inputs[1][0] -= now_go;
            now_tank += now_go;
        }
        while (y != 9)
        {
            finalize_tank(x, y);
            sol.ops.push_back({moveR, 0});
            finalize_tank(x, y);
            y++;
        }
        if (now_tank > 0)
        {
            sol.ops.push_back({4, -now_tank});
            inputs[x][y] += now_tank;
            now_tank = 0;
        }
    }

    while (x != 19 || y != 9)
    {
        if (x % 2 == 0)
        {
            if (y == 19)
            {
                tmp_ops.push_back({moveD, 0});
                x++;
            }
            else
            {
                tmp_ops.push_back({moveR, 0});
                y++;
            }
        }
        else
        {
            if (y == 0)
            {
                tmp_ops.push_back({moveD, 0});
                x++;
            }
            else
            {
                tmp_ops.push_back({moveL, 0});
                y--;
            }
        }

        if (y != 9)
        {
            uplift_tank(x, y);
        }
        else
        {
            narashi(x, y, 1);
        }
    }

    // (19, 9) -> (19,0) -> (19,9) まで折り返し
    {
        int now = 0;
        REP(i, 9)
        {
            now += inputs[19][i];
        }
        if (now < 0)
        {
            sol.ops.push_back({4, -now});
            now_tank += -now;
            inputs[19][9] += now;
        }
        while (y != 0)
        {
            if (inputs[x][y] < 0)
            {
                int target = min(now_tank, -inputs[x][y]);
                if (target != 0)
                {
                    tmp_ops.push_back({4, -target});
                    now_tank -= target;
                    inputs[x][y] += target;
                }
            }
            if (y == 0)
                break;
            tmp_ops.push_back({moveL, 0});
            y--;
        }
        while (y != 9)
        {
            uplift_tank(x, y);
            tmp_ops.push_back({moveR, 0});
            y++;
            uplift_tank(x, y);
        }
        narashi(x, y);
    }

    vector<pair<int, int>> mae, usiro;
    REP(i, 20)
    {
        if (inputs[i][9] > 0)
        {
            mae.push_back({i, inputs[i][9]});
        }
        else
        {
            usiro.push_back({i, -inputs[i][9]});
        }
    }
    int tmp_scoring = 0;
    int itr = 0;
    int tumiage[20] = {};
    for (int i = 0; i < usiro.size(); ++i)
    {
        while (usiro[i].second > 0)
        {
            if (mae[itr].second > 0)
            {
                int target = min(mae[itr].second, usiro[i].second);
                if (usiro[i].first < mae[itr].first)
                {
                    tumiage[mae[itr].first] += target;
                }
                tmp_scoring += target * (2 + abs(mae[itr].first - usiro[i].first));
                mae[itr].second -= target;
                usiro[i].second -= target;
            }
            else
            {
                itr++;
            }
        }
    }

    while (x >= 0)
    {
        finalize_tank(x, y, tumiage[x]);
        if (x == 0)
            break;
        sol.ops.push_back({moveU, 0});
        x--;
    }
    while (x <= 19)
    {
        finalize_tank(x, y);
        if (x == 19)
            break;
        sol.ops.push_back({moveD, 0});
        x++;
    }
    REP(i, n)
    {
        REP(j, n)
        {
            assert(inputs[i][j] == 0);
        }
    }
    Solution t = compress(sol);
    return t;
}
void solve()
{
    vector<vector<int>> inputs = load_input();
    Solution t = solve_by_greedy(inputs, 0);
    Solution t2 = solve_by_greedy(inputs, 1);
    if (evaluation(t2) < evaluation(t))
    {
        t = t2;
    }
    vector<vector<int>> transpose_inputs(n, vector<int>(n, 0));
    REP(i, n)
    REP(j, n)
    transpose_inputs[j][i] = inputs[i][j];
    Solution Tea = solve_by_greedy(transpose_inputs, 0);
    Solution Tea2 = solve_by_greedy(transpose_inputs, 1);
    if (evaluation(Tea2) < evaluation(Tea))
    {
        Tea = Tea2;
    }
    for (auto &x : Tea.ops)
    {
        if (x.op_num == moveU)
        {
            x.op_num = moveL;
        }
        else if (x.op_num == moveD)
        {
            x.op_num = moveR;
        }
        else if (x.op_num == moveR)
        {
            x.op_num = moveD;
        }
        else if (x.op_num == moveL)
        {
            x.op_num = moveU;
        }
    }
    cerr << "normal: " << evaluation(t) << endl;
    cerr << "transposed: " << evaluation(Tea) << endl;
    if (evaluation(t) > evaluation(Tea))
    {
        t = Tea;
    }
    print_solution(t);
}

#undef int
int main()
{
    int t = 1;
    // cin >> t;
    REP(te, t)
    solve();
}
