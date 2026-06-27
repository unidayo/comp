#include <bits/stdc++.h>
using namespace std;
typedef long long LL;
typedef pair<LL, LL> PII;
mt19937_64 engine(0);
struct Timer
{
    timespec start;

    void begin()
    {
        clock_gettime(CLOCK_REALTIME, &start);
    }

    double stopwatch()
    {
        timespec end;
        clock_gettime(CLOCK_REALTIME, &end);
        double sec = end.tv_sec - start.tv_sec;
        double nsec = end.tv_nsec - start.tv_nsec;
        return sec + 1e-9 * nsec;
    }
};
Timer TIMER;

class State
{
public:
    vector<vector<int>> field;
    vector<pair<PII, PII>> operations;
    vector<vector<int>> ended_cache; // bool
    int N;
    int M;

    State(vector<vector<int>> input)
    {
        this->field = input;
        N = input.size();
        ended_cache = vector<vector<int>>(N, vector<int>(N, 0));
        M = N * (N + 1) / 2;
    }
    State(State &state)
    {
        field = state.field;
        operations = state.operations;
        ended_cache = state.ended_cache;
        N = field.size();
        M = N * (N + 1) / 2;
    }

    bool is_finished()
    {
        for (int i = 0; i < N - 1; i++)
        {
            for (int j = 0; j <= i; j++)
            {
                if (field[i][j] > field[i + 1][j])
                    return false;
                if (field[i][j] > field[i + 1][j + 1])
                    return false;
            }
        }
        return true;
    }

    void write_answer()
    {
        cout << operations.size() << endl;
        for (auto ops : operations)
        {
            cout << ops.first.first << " " << ops.first.second << " " << ops.second.first << " " << ops.second.second << endl;
        }
    }

    PII search(int val)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j <= i; j++)
            {
                if (val == field[i][j])
                    return PII(i, j);
            }
        return PII(-1, -1);
    }

    void do_ops(PII pos1, PII pos2)
    {
        operations.push_back(make_pair(pos1, pos2));

        swap(field[pos1.first][pos1.second], field[pos2.first][pos2.second]);
    }

    void bubun_score(PII pos)
    {
    }
    double func(PII pos, double pow_param)
    {
        double x = M + 1 - field[pos.first][pos.second];
        return pow(x, pow_param) / 100000;
    }
    void do_greedy(int start_val, double pow_param)
    {
        // cerr << "do_greedy" << start_val << endl;
        int cur = -1;
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j <= i; j++)
            {
                if (ended_cache[i][j])
                    cur = max(cur, field[i][j]);
            }
        }
        cur += 1;
        if (start_val != cur)
        {
            cerr << "!!!bug!!!" << endl;
        }
        PII pos = search(cur);
        double INF = 100000000;

        vector<vector<double>> dp(N, vector<double>(N, INF));
        vector<vector<PII>> prev(N, vector<PII>(N, PII(-1, -1)));
        int x = pos.first;
        int y = pos.second;
        dp[x][y] = 0;

        for (int i = x; i > 0; i--)
        {
            for (int j = max(i, y); j >= 0; j--)
            {
                if (ended_cache[i][j])
                    continue;

                if (j > 0)
                {
                    double score = func(PII(i - 1, j - 1), pow_param);
                    if (dp[i - 1][j - 1] > dp[i][j] + score)
                    {
                        prev[i - 1][j - 1] = PII(i, j);
                        dp[i - 1][j - 1] = dp[i][j] + score;
                    }
                }
                if (j < i)
                {
                    double score = func(PII(i - 1, j), pow_param);
                    if (dp[i - 1][j] > dp[i][j] + score)
                    {
                        prev[i - 1][j] = PII(i, j);
                        dp[i - 1][j] = dp[i][j] + score;
                    }
                }
            }
        }
        vector<PII> candidate_goal;
        if (ended_cache[0][0] == 0)
            candidate_goal.push_back(PII(0, 0));
        for (int i = 1; i < N; i++)
        {
            for (int j = 0; j <= i; j++)
            {
                if (ended_cache[i][j])
                    continue;
                if (j > 0 && !ended_cache[i - 1][j - 1])
                    continue;
                if (i > j && !ended_cache[i - 1][j])
                    continue;
                candidate_goal.push_back(PII(i, j));
            }
        }
        double score = INF;
        PII goal = PII(-1, -1);
        for (auto pos : candidate_goal)
        {
            if (score > dp[pos.first][pos.second])
            {
                score = dp[pos.first][pos.second];
                goal = pos;
            }
        }
        PII cur_pos = goal;
        // cerr << "goal" << goal.first << " " << goal.second << " " << score << endl;
        vector<PII> seqs;
        while (true)
        {
            seqs.push_back(cur_pos);
            // cerr << cur_pos.first << " " << cur_pos.second << endl;
            PII next_pos = prev[cur_pos.first][cur_pos.second];
            if (next_pos == PII(-1, -1))
                break;
            cur_pos = next_pos;
        }
        for (int i = seqs.size() - 1; i >= 1; i--)
        {
            do_ops(seqs[i - 1], seqs[i]);
        }
        ended_cache[goal.first][goal.second] = true;

        /*
                // とりあえず簡素な方式だけ
                while (true)
                {
                    if (cur_pos == PII(0, 0))
                    {
                        ended_cache[cur_pos.first][cur_pos.second] = true;
                        break;
                    }

                    if (cur_pos.second > 0)
                    {
                        PII next_pos = PII(cur_pos.first - 1, cur_pos.second - 1);
                        if (!ended_cache[next_pos.first][next_pos.second])
                        {
                            do_ops(cur_pos, next_pos);
                            cur_pos = next_pos;
                            continue;
                        }
                    }
                    if (cur_pos.second < cur_pos.first)
                    {
                        PII next_pos = PII(cur_pos.first - 1, cur_pos.second);
                        if (!ended_cache[next_pos.first][next_pos.second])
                        {
                            do_ops(cur_pos, next_pos);
                            cur_pos = next_pos;
                            continue;
                        }
                    }

                    // もう何もない
                    // cout << " cur_pos" << cur_pos.first << " " << cur_pos.second << endl;
                    ended_cache[cur_pos.first][cur_pos.second] = true;
                    break;
                }
                */
    }
    void debug()
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j <= i; j++)
            {
                cerr << field[i][j] << " ";
            }
            cerr << endl;
        }
    }
};

class Solver
{
public:
    vector<vector<int>> input;

    int N;
    int M;
    Solver(vector<vector<int>> input)
    {
        this->input = input;
        N = input.size();
        M = N * (N + 1) / 2;
        // cout << N << endl;
    }

    State playout(vector<pair<PII, PII>> jizen)
    {
        State state(input);
        for (auto ops : jizen)
        {
            state.do_ops(ops.first, ops.second);
        }
        for (int ball = 0; ball < M; ball++)
        {
            state.do_greedy(ball, 2);
        }
        return state;
    }
    void solve()
    {
        State best_state(input);
        vector<pair<PII, PII>> jizen;

        for (int z = 0; z < 4; z++)
        {
            for (int i = 0; i < 29; i++)
                for (int j = 0; j <= i; j++)
                {
                    if (TIMER.stopwatch() > 1.9)
                    {
                        cerr << "TIME OVER" << endl;
                        break;
                    }
                    if (5 < j && j < i - 5)
                    {
                        continue;
                    }
                    if (z == 0)
                    {
                        if (i > 0)
                            continue;
                    }
                    if (z == 1)
                    {
                        if (i == j)
                            continue;
                        jizen.push_back(make_pair(PII(i, j), PII(i, j + 1)));
                    }
                    if (z == 2)
                    {
                        jizen.push_back(make_pair(PII(i, j), PII(i + 1, j + 1)));
                    }
                    if (z == 3)
                    {
                        jizen.push_back(make_pair(PII(i, j), PII(i + 1, j)));
                    }
                    State state = playout(jizen);

                    if (!state.is_finished())
                    {
                        cerr << "owattenai====" << endl;
                        continue;
                    }
                    if (!best_state.is_finished())
                    {
                        best_state = state;
                        continue;
                    }
                    if (best_state.operations.size() > state.operations.size())
                    {
                        cerr << i << " " << j << " " << best_state.operations.size() << " " << state.operations.size() << endl;
                        best_state = state;
                    }
                    else
                    {
                        jizen.pop_back();
                    }
                }
        }
        best_state.write_answer();
    }
};

int main()
{
    TIMER.begin();

    int N = 30;
    vector<vector<int>> input(N, vector<int>(N, 0));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            cin >> input[i][j];
        }
    }
    // cout << input.size() << endl;

    Solver solver(input);
    solver.solve();
}
