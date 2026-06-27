#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <numeric>
#include <functional>
#include <complex>
#include <cassert>
#include <memory>
#include <chrono>

using namespace std;

#define FOR(i, n) for (int i = 0, i##_end = (int(n)); i < i##_end; i++)

int N, M, C;
int8_t dr8[8] = {1, 1, 0, -1, -1, -1, 0, 1};
int8_t dc8[8] = {0, 1, 1, 1, 0, -1, -1, -1};
int dist[100][100];
vector<int> X, Y;
double S, Coef;

double init_temp = 24809880;
double end_temp = 3361;
double RemoveRatio = 0.7273777777234557;
double MinDistThreshold = 0.24321733828861802;
struct Rect
{
public:
    int8_t Y1, X1, Y2, X2, Y3, X3, Y4, X4;

    Rect(int y1, int x1, int y2, int x2, int y3, int x3, int y4, int x4) : Y1(y1), X1(x1), Y2(y2), X2(x2), Y3(y3), X3(x3), Y4(y4), X4(x4)
    {
    }
};

template <typename T>
inline int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

uint32_t xor128()
{
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
uint32_t xor128(int max)
{
    return xor128() % max;
}
double xor128_double()
{
    return 1.0 * xor128() / UINT32_MAX;
}

template <typename T>
void array_shuffle(vector<T> &array)
{
    int size = array.size();
    for (int i = 1; i < size; i++)
    {
        int r = xor128(i + 1);
        swap(array[i], array[r]);
    }
}

struct Result
{
public:
    vector<Rect> Rects;
    double Score;
};
class State
{
public:
    vector<Rect> Rects;

private:
    int N;
    vector<char> _hasPoint;
    vector<char> _used;
    vector<int8_t> _nearestDistance;
    double _score;
    static vector<int> _to_index;

    void PlacePoint(int row, int col)
    {
        _hasPoint[row * N + col] = true;
        int8_t dist = 0;
        for (int dir = 0; dir < 8; dir++)
        {
            auto dr = dr8[dir];
            auto dc = dc8[dir];
            auto cr = row + dr;
            auto cc = col + dc;
            auto dir2 = dir ^ 4;
            dist = 0;
            while (0 <= cr && cr < N && 0 <= cc && cc < N)
            {
                dist++;
                _nearestDistance[((cr * N + cc) << 3) | dir2] = dist;
                if (_hasPoint[cr * N + cc])
                    break;
                cr += dr;
                cc += dc;
            }
        }
    }

    void RemovePoint(int row, int col)
    {        
        _hasPoint[row * N + col] = false;
        for (int dir = 0; dir < 8; dir++)
        {
            auto dr = dr8[dir];
            auto dc = dc8[dir];
            auto cr = row + dr;
            auto cc = col + dc;
            auto dir2 = dir ^ 4;
            int8_t dist = _nearestDistance[((row * N + col) << 3) | dir2];
            while (0 <= cr && cr < N && 0 <= cc && cc < N)
            {
                if(dist >= 0) dist++;
                _nearestDistance[((cr * N + cc) << 3) | dir2] = dist;
                if (_hasPoint[cr * N + cc])
                    break;
                cr += dr;
                cc += dc;
            }
        }
    }

public:
    static void Initialize()
    {
        _to_index.resize(1 << 10);
        for (int i = 0; i < 8; i++)
        {
            _to_index[((dr8[i] + 1) << 4) | (dc8[i] + 1)] = i;
        }
    }
    State() {}
    State(int n, const vector<int> &x, const vector<int> &y) :
        N(n), _hasPoint(n * n), _used(8 * n * n), _nearestDistance(8 * n * n, -1)
    {
        _score = 0;
        FOR(i, x.size())
        {
            PlacePoint(y[i], x[i]);
            _score += dist[y[i]][x[i]]+ 1;
        }
    }

    inline int8_t GetNearestPointDistance(int row, int col, int dir) const
    {
        return _nearestDistance[((row * N + col) << 3) | dir];
    };
    inline bool HasPoint(int row, int col) const { return _hasPoint[row * N + col]; }
    inline bool ExistsLine(int row, int col, int dir) const { return _used[((row * N + col) << 3) | dir]; }

    inline double GetScore() const
    {
        return _score * Coef;
    }

    Result GetResult() const
    {
        Result r;
        r.Rects = this->Rects;
        r.Score = GetScore();
        return r;
    }

    void RemoveRect(const Rect &rect)
    {
        RemovePoint(rect.Y1, rect.X1);

        RemoveLine(rect.Y1, rect.X1, rect.Y2, rect.X2);
        RemoveLine(rect.Y2, rect.X2, rect.Y3, rect.X3);
        RemoveLine(rect.Y3, rect.X3, rect.Y4, rect.X4);
        RemoveLine(rect.Y4, rect.X4, rect.Y1, rect.X1);

        _score -= dist[rect.Y1][rect.X1] + 1;
    }

    void AddRect(const Rect &rect)
    {
        // RectanglesStack = RectanglesStack.Push(rect);
        Rects.push_back(rect);
        PlacePoint(rect.Y1, rect.X1);

        AddLine(rect.Y1, rect.X1, rect.Y2, rect.X2);
        AddLine(rect.Y2, rect.X2, rect.Y3, rect.X3);
        AddLine(rect.Y3, rect.X3, rect.Y4, rect.X4);
        AddLine(rect.Y4, rect.X4, rect.Y1, rect.X1);

        _score += dist[rect.Y1][rect.X1] + 1;
    }

    void AddRects(const vector<Rect> &rects)
    {
        for(const auto& rect: rects){
            _hasPoint[rect.Y1 * N + rect.X1] = true;
            AddLine(rect.Y1, rect.X1, rect.Y2, rect.X2);
            AddLine(rect.Y2, rect.X2, rect.Y3, rect.X3);
            AddLine(rect.Y3, rect.X3, rect.Y4, rect.X4);
            AddLine(rect.Y4, rect.X4, rect.Y1, rect.X1);
            _score += dist[rect.Y1][rect.X1] + 1;
            Rects.push_back(rect);
        }
        for(const auto& rect: rects){
            PlacePoint(rect.Y1, rect.X1);
        }
    }

    void AddRect(int r1, int c1, int r2, int c2, int r3, int c3, int r4, int c4)
    {
        // RectanglesStack = RectanglesStack.Push(new Rect(r1, c1, r2, c2, r3, c3, r4, c4));
        Rects.emplace_back((int8_t)r1, (int8_t)c1, (int8_t)r2, (int8_t)c2, (int8_t)r3, (int8_t)c3, (int8_t)r4, (int8_t)c4);

        PlacePoint(r1, c1);

        AddLine(r1, c1, r2, c2);
        AddLine(r2, c2, r3, c3);
        AddLine(r3, c3, r4, c4);
        AddLine(r4, c4, r1, c1);
        _score += dist[r1][c1] + 1;
    }

    bool CheckMove(int r1, int c1, int r2, int c2, int r3, int c3, int r4, int c4)
    {
        return CheckLine(r1, c1, r2, c2, r3, c3, r4, c4) &&
               CheckLine(r2, c2, r3, c3, r4, c4, r1, c1);
    }

private:
    bool CheckLine(int r1, int c1, int r2, int c2, int r3, int c3, int r4, int c4)
    {
        auto dr = sgn(r2 - r1);
        auto dc = sgn(c2 - c1);
        int dir = _to_index[((dr + 1) << 4) | (dc + 1)];

        if (ExistsLine(r1, c1, dir))
            return false;
        if (ExistsLine(r2, c2, dir ^ 4))
            return false;

        if (ExistsLine(r4, c4, dir))
            return false;
        if (ExistsLine(r3, c3, dir ^ 4))
            return false;

        return true;
    }

    void AddLine(int r1, int c1, int r2, int c2)
    {
        auto dc = sgn(c2 - c1);
        auto dr = sgn(r2 - r1);
        int dir = _to_index[((dr + 1) << 4) | (dc + 1)];
        auto cr = r1;
        auto cc = c1;
        while (cc != c2 || cr != r2)
        {
            _used[(cr * N + cc) << 3 | dir] = true;
            cc += dc;
            cr += dr;
            _used[(cr * N + cc) << 3 | (dir ^ 4)] = true;
        }
    }
    void RemoveLine(int r1, int c1, int r2, int c2)
    {
        auto dc = sgn(c2 - c1);
        auto dr = sgn(r2 - r1);
        int dir = _to_index[((dr + 1) << 4) | (dc + 1)];
        auto cr = r1;
        auto cc = c1;
        while (cc != c2 || cr != r2)
        {
            _used[(cr * N + cc) << 3 | dir] = false;
            cc += dc;
            cr += dr;
            _used[(cr * N + cc) << 3 | (dir ^ 4)] = false;
        }
    }
};
vector<int> State::_to_index;
int inQueue[10000];
int done[10000];
vector<tuple<int, int>> points;
vector<tuple<int, int>> neighbors;
tuple<int, int> deq[500000];
int ddd[2] = {2, 6};

void play_out(State &state)
{
    int cur = 0;
    static int true_value = 11;
    true_value++;
    if (neighbors.size() == 0)
    {
        auto w = 2;
        for (int dr = -w; dr <= w; dr++)
        {
            if (dr == 0)
                continue;
            for (int dc = -w; dc <= w; dc++)
            {
                if (dc == 0)
                    continue;
                if (abs(dr) == abs(dc))
                    continue;
                neighbors.emplace_back(dr, dc);
            }
        }
    }

    points.clear();
    FOR(i, M) points.emplace_back(Y[i], X[i]);
    for (const auto &rect : state.Rects) points.emplace_back(rect.Y1, rect.X1);
    array_shuffle(points);

    auto enqueue = [&cur](int row, int col)
    {
        auto id = row * N + col;
        if (inQueue[id] != true_value && done[id] != true_value)
        {
            deq[cur++] = make_tuple(row, col);
            inQueue[id] = true_value;
        }
    };

    for (const auto &initP : points)
    {
        int initRow, initCol;
        tie(initRow, initCol) = initP;
        if (!state.HasPoint(initRow, initCol))
            continue;
        if (done[initRow * N + initCol] == true_value)
            continue;
        enqueue(initRow, initCol);
        while (cur > 0)
        {
            int row, col;
            swap(deq[xor128(cur)], deq[cur - 1]);
            tie(row, col) = deq[--cur];
            auto id = row * N + col;
            inQueue[id] = true_value-1;
            done[id] = true_value;
            if (!state.HasPoint(row, col))
                continue;
            bool added = false;
            auto offset = xor128(8);
            for (int _dir = 0; _dir < 8; _dir++)
            {
                auto dir = (_dir + offset) & 7;
                if (state.ExistsLine(row, col, dir))
                    continue;
                auto dist1 = state.GetNearestPointDistance(row, col, dir);
                if (dist1 == -1)
                    continue;
                auto r1 = row + dr8[dir] * dist1;
                auto c1 = col + dc8[dir] * dist1;
                for (auto dd : ddd)
                {
                    auto dir2 = (dir + dd) & 7;
                    if (state.ExistsLine(row, col, dir2))
                        continue;
                    auto dist2 = state.GetNearestPointDistance(r1, c1, dir2);
                    if (dist2 != -1)
                    {
                        auto r2 = r1 + dr8[dir2] * dist2;
                        auto c2 = c1 + dc8[dir2] * dist2;
                        auto r3 = r2 - dr8[dir] * dist1;
                        auto c3 = c2 - dc8[dir] * dist1;
                        if (0 <= r3 && r3 < N && 0 <= c3 && c3 < N &&
                            !state.HasPoint(r3, c3) &&
                            state.GetNearestPointDistance(r3, c3, dir) == dist1 &&
                            state.GetNearestPointDistance(r3, c3, dir2 ^ 4) == dist2 &&
                            state.CheckMove(r3, c3, row, col, r1, c1, r2, c2))
                        {
                            state.AddRect(r3, c3, row, col, r1, c1, r2, c2);
                            enqueue(r3, c3);
                            added = true;
                            break;
                        }
                    }
                    if (dd == 2)
                    {
                        auto dist3 = state.GetNearestPointDistance(row, col, dir2);
                        if (dist3 == -1)
                            continue;
                        {
                            int r3, c3, r2, c2;
                            r3 = row + dr8[dir2] * dist3;
                            c3 = col + dc8[dir2] * dist3;
                            r2 = r1 + r3 - row;
                            c2 = c1 + c3 - col;
                            if (0 <= r2 && r2 < N && 0 <= c2 && c2 < N &&
                                !state.HasPoint(r2, c2) &&
                                state.GetNearestPointDistance(r2, c2, dir ^ 4) == dist1 &&
                                state.GetNearestPointDistance(r2, c2, dir2 ^ 4) == dist3)
                            {
                                if (state.CheckMove(r2, c2, r1, c1, row, col, r3, c3))
                                {
                                    state.AddRect(r2, c2, r1, c1, row, col, r3, c3);
                                    enqueue(r2, c2);
                                    added = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if (added)
            {
                for (int dir2 = 0; dir2 < 8; dir2++)
                {
                    auto dist = state.GetNearestPointDistance(row, col, dir2);
                    if (dist != -1)
                    {
                        auto dr = dr8[dir2];
                        auto dc = dc8[dir2];
                        auto nrow = row + dr * dist;
                        auto ncol = col + dc * dist;
                        enqueue(nrow, ncol);
                    }
                }

                for (const auto &p : neighbors)
                {
                    int dr, dc;
                    tie(dr, dc) = p;
                    auto nr = row + dr;
                    auto nc = col + dc;
                    if (0 <= nr && nr < N && 0 <= nc && nc < N && state.HasPoint(nr, nc))
                    {
                        enqueue(nr, nc);
                    }
                }
            }
        }
    }
}

int need[10000];
State InitState;
vector<Rect> newRects;
vector<Rect> removeRects;

void reset_partial(State &state, double ratio, double removeRatio)
{
    if (state.Rects.size() == 0)
        return;
    auto c = N / 2;

    static int true_value = 11;
    true_value++;
    newRects.clear();
    removeRects.clear();
    auto d = 1.0 * N / 2 * ratio;
    d *= d;
    d *= 2;

    int iter = 0;

    for (int i = state.Rects.size() - 1; i >= 0; i--)
    {
        const auto &rect = state.Rects[i];
        if (need[rect.Y1 * N + rect.X1] != true_value && (dist[rect.Y1][rect.X1] < d || xor128_double() < removeRatio))
        {
            // Remove
            removeRects.push_back(rect);
        }
        else
        {
            newRects.push_back(rect);
            need[rect.Y2 * N + rect.X2] = true_value;
            need[rect.Y3 * N + rect.X3] = true_value;
            need[rect.Y4 * N + rect.X4] = true_value;
        }
    }

    if(newRects.size() > removeRects.size() * 1.5) {
        for(const auto& rect: removeRects) {
            state.RemoveRect(rect);
        }
        reverse(newRects.begin(), newRects.end());
        state.Rects = move(newRects);
    }else {
        State newState(InitState);
        reverse(newRects.begin(), newRects.end());
        newState.AddRects(newRects);
        state = move(newState);
    }
}
void adjust(double &delta, double temperature, double lastTemperature, int iter, double ellasedTime, double remainTime)
{
    // temprature * delta^cnt = targetT
    // delta = (targetT / temprature)^(1/cnt)
    auto estimateCnt = iter / ellasedTime * remainTime;
    delta = pow(lastTemperature / temperature, 1.0 / estimateCnt);
}

bool anneal(double temperature, double difference)
{
    if (difference > 0)
    {
        return true;
    }
    auto d = difference / temperature;
    if (d < -16)
        return false;
    return xor128_double() < exp(d);
}

Result run()
{
    auto time_point = std::chrono::high_resolution_clock::now();
    InitState = State(N, X, Y);

    auto current = make_shared<State>(InitState);
    auto best = make_shared<State>(*current);
    shared_ptr<State> best_prev = nullptr;

    constexpr double TT = 1;
    constexpr double TIME_LIMIT = 4950.0 * TT;
    constexpr double TIME_FIRST_PHASE = 150.0;
    constexpr double TIME_LAST_PHASE = (TIME_LIMIT - 100 * TT);
    constexpr double TIME_LAST_PHASE2 = (TIME_LIMIT - 50 * TT);
    double temp = init_temp;
    double delta = 1.0;
    int loop = 0, iter = 0;

    long long ellapsedMs = 0;
    while (true)
    {
        if (loop++ % 16 == 0)
        {
            auto end = std::chrono::high_resolution_clock::now();
            ellapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - time_point).count();
        }
        if (ellapsedMs > TIME_LIMIT)
            break;
        if (ellapsedMs > TIME_LAST_PHASE)
            temp = 0;
        if (ellapsedMs > TIME_FIRST_PHASE && loop % 100 == 0 && temp > 0)
        {
            adjust(delta, temp, end_temp, iter, ellapsedMs, TIME_LIMIT - ellapsedMs);
        }
        auto orig = make_shared<State>(*current);
        if (ellapsedMs > TIME_LAST_PHASE2)
        {
            orig = best_prev;
        }
        else
        {
            reset_partial(*orig, MinDistThreshold, RemoveRatio);
        }
        bool updated = false;
        FOR(i, 16)
        {
            auto next = make_shared<State>(*orig);
            play_out(*next);
            if (anneal(temp, next->GetScore() - current->GetScore()))
            {
                current = next;
            }
            if (best->GetScore() < next->GetScore())
            {
                // cerr<<loop<<" "<< (long)best->GetScore() <<" -> "<< (long)next->GetScore()<<" "<<ellapsedMs<<" ms" <<" " << (long)temp <<endl;
                best = move(next);
                best_prev = orig;
                updated = true;
            }

            temp *= delta;
            iter++;
            if (!updated)
                break;
        }
    }

    cerr << "Score " << loop << " " << iter << "(" << ellapsedMs << " ms): " << (long)best->GetScore() << endl;
    return best->GetResult();
}

void read_input()
{
    cin >> N;
    cin >> M;
    C = N / 2;
    X.resize(M);
    Y.resize(M);
    FOR(i, M)
    cin >> X[i] >> Y[i];
    S = 0;
    int c = N / 2;
    FOR(i, N)
    FOR(j, N)
    S += (i - c) * (i - c) + (j - c) * (j - c) + 1;
    Coef = 1000000.0 * N * N / M / S;
    FOR(i,N) FOR(j,N) {
        dist[i][j] = (i - N/2) * (i - N/2) + (j - N/2) * (j - N/2) ;
    }
}

signed main(int argc, const char *argv[])
{
    if(argc >= 3) {
        MinDistThreshold = atof(argv[1]);
        //end_temp = atof(argv[2]);
    }
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    State::Initialize();

    read_input();

    auto result = run();

    cout << result.Rects.size() << endl;
    for (auto &r : result.Rects)
    {
        cout << (int)r.X1 << " " << (int)r.Y1 << " " << (int)r.X2 << " " << (int)r.Y2 << " " << (int)r.X3 << " " << (int)r.Y3 << " " << (int)r.X4 << " " << (int)r.Y4 << "\n";
    }
}
