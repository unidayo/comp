#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <bits/stdc++.h>
#include <atcoder/all>

using namespace std;
using namespace atcoder;

constexpr double time_limit = 1.95;

const vector<pair<int,int>> dxdy = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

const string fblr = "FBLR";

template <class T>
T sum(const vector<T>& data) {
    return accumulate(data.begin(), data.end(), (T)0);
}

template <class T>
int index(const vector<T>& data, T x) {
    return distance(data.begin(), find(data.begin(),data.end(),x));
}

template <class T>
bool contain(const vector<T>& data, T x) {
    return !(index(data, x) == (int)data.size());
}

template <class T>
int count(const vector<T>& data, T x) {
    int cnt = 0;
    for (auto y : data) {
        if (x == y) {
            cnt++;
        }
    }
    return cnt;
}

template <class T>
int bisect_left(const vector<T>& data, T x) {
    // binary search
    int left = 0;
    int right = (int)data.size();
    while (left < right) {
        int center = (left + right) / 2;
        if (data[center] < x) {
            left = center + 1;
        } else {
            right = center;
        }
    }
    return left;
}

struct Randomizer {
    mt19937_64 engine;
    uniform_int_distribution<> dist = uniform_int_distribution<>(0, INT_MAX);

    Randomizer() {
        random_device seed_gen;
        engine = mt19937_64(seed_gen());
    }

    Randomizer(int seed) {
        engine = mt19937_64(seed);
    }

    double uniform(double l, double r) {
        assert(l <= r);
        return l + (r-l)*dist(engine)/INT_MAX;
    }

    int randrange(int l, int r) {
        assert(l < r);
        return l + dist(engine)%(r-l);
    }

    template <class T>
    T choice(const vector<T>& data) {
        return data.at(randrange(0, data.size()));
    }

    vector<int> sample(int l, int r, int num) {
        assert(0 < num && num <= r - l);
        vector<int> ret;
        while ((int)ret.size() < num) {
            int x = randrange(l, r);
            if (!contain(ret, x)) {
                ret.push_back(x);
            }
        }
        sort(ret.begin(), ret.end());
        return ret;
    }

    template <class T>
    void shuffle_vector(vector<T>& data) {
        shuffle(data.begin(), data.end(), engine);
    }
};

struct Timer {
    timespec start;
    double lapse;
    Randomizer randomizer;

    Timer() {}

    Timer(int seed) {
        randomizer = Randomizer(seed);
    }

    void begin() {
        clock_gettime(CLOCK_REALTIME, &start);
    }

    double stopwatch() {
        timespec end;
        clock_gettime(CLOCK_REALTIME, &end);
        double sec = end.tv_sec - start.tv_sec;
        double nsec = end.tv_nsec - start.tv_nsec;
        lapse = sec + nsec/1000000000;
        return lapse;
    }

    bool scheduler(double delta, double time_limit, double t0, double t1) {
        assert(0.0 < time_limit && 0.0 <= t1 && t1 <= t0);
        if (0.0 <= delta) {
            return true;
        } else {
            double ratio = lapse / time_limit;
            double t = pow(t0, 1.0-ratio) * pow(t1, ratio);
            return randomizer.uniform(0.0, 1.0) < pow(2.0, delta/t);
        }
    }
};

Randomizer randomizer;

inline bool in_grid(int x, int y) {
    return 0 <= x && x < 10 && 0 <= y && y < 10;
}

struct State {
    vector<vector<int>> grid = vector<vector<int>>(10, vector<int>(10, 0));

    pair<int,int> put(int f, int p) {
        for (int x = 0; x < 10; ++x) {
            for (int y = 0; y < 10; ++y) {
                if (grid[x][y] == 0) {
                    if (--p == 0) {
                        grid[x][y] = f;
                        return {x, y};
                    }
                }
            }
        }
        return {-1, -1};
    }

    void tilt(const char d) {
        if (d == 'F') {
            for (int y = 0; y < 10; ++y) {
                int cnt = 0;
                for (int x = 0; x < 10; ++x) {
                    if (grid[x][y]) {
                        swap(grid[cnt][y], grid[x][y]);
                        ++cnt;
                    }
                }
            }
        } else if (d == 'B') {
            for (int y = 0; y < 10; ++y) {
                int cnt = 10 - 1;
                for (int x = 10 - 1; x >= 0; --x) {
                    if (grid[x][y]) {
                        swap(grid[cnt][y], grid[x][y]);
                        --cnt;
                    }
                }
            }
        } else if (d == 'L') {
            for (int x = 0; x < 10; ++x) {
                int cnt = 0;
                for (int y = 0; y < 10; ++y) {
                    if (grid[x][y]) {
                        swap(grid[x][cnt], grid[x][y]);
                        ++cnt;
                    }
                }
            }
        } else {
            for (int x = 0; x < 10; ++x) {
                int cnt = 10 - 1;
                for (int y = 10 - 1; y >= 0; --y) {
                    if (grid[x][y]) {
                        swap(grid[x][cnt], grid[x][y]);
                        --cnt;
                    }
                }
            }
        }
    }

    long long get_score() {
        vector<vector<bool>> visited(10, vector<bool>(10, false));
        int score = 0;
        for (int x = 0; x < 10; ++x) {
            for (int y = 0; y < 10; ++y) {
                if (visited[x][y]) {
                    continue;
                }
                visited[x][y] = true;
                queue<pair<int,int>> todo;
                todo.push({x, y});
                int cnt = 0;
                while (!todo.empty()) {
                    ++cnt;
                    auto [x, y] = todo.front();
                    assert(visited[x][y]);
                    todo.pop();
                    for (auto [dx, dy] : dxdy) {
                        int xd = x + dx;
                        int yd = y + dy;
                        if (in_grid(xd, yd) && !visited[xd][yd] && grid[x][y] == grid[xd][yd]) {
                            visited[xd][yd] = true;
                            todo.push({xd, yd});
                        }
                    }
                }
                score += cnt * cnt;
            }
        }
        return score;
    }

    long long simulate(char c, const vector<int> &f, const vector<char> &output, int t, const vector<int> &rands) {
        tilt(c);
        for (int i = t + 1; i < 100; ++i) {
            put(f[i], rands[i]);
            tilt(output[i]);
        }
        return get_score();
    }
};

struct Solver {
    vector<int> f;
    vector<char> output = vector<char>(100);
    State state;
    Timer timer;
    vector<vector<int>> rands = vector<vector<int>>(2000, vector<int>(100));

    Solver(const vector<int> &_f) {
        f = _f;
        for (int i = 0; i < 2000; ++i) {
            for (int j = 0; j < 100; ++j) {
                rands[i][j] = randomizer.randrange(1, 100 - j + 1);
            }
        }
    }

    int get_cost(int bottom) {
        int cost = 0;
        int prev = 0;
        for (int i = 0; i < 100 - 1; ++i) {
            if (f[i] != bottom && f[i + 1] == bottom) {
                prev = f[i];
            } else if (f[i] == bottom && f[i + 1] != bottom) {
                if (prev != f[i + 1]) {
                    ++cost;
                }
            }
        }
        return cost;
    }

    char monte_carlo(int t, vector<bool> &skip) {
        vector<long long> scores(4, 0);
        double remained_time = time_limit - timer.stopwatch();
        if (t < 80) {
            remained_time *= 1.1;
        }
        double end_time = timer.lapse + remained_time / (100 - t);
        int cnt = 0;
        while (timer.stopwatch() < end_time && cnt < 2000) {
            for (int i = 0; i < 4; ++i) {
                if (skip[i]) {
                    continue;
                }
                State new_state = state;
                scores[i] += new_state.simulate(fblr[i], f, output, t, rands[cnt]);
            }
            ++cnt;
        }
        int i = 0;
        for (int j = 1; j < 4; ++j) {
            if (scores[i] < scores[j]) {
                i = j;
            }
        }
        return fblr[i];
    }

    void solve() {
        timer.begin();
        vector<int> counts = {0, get_cost(1), get_cost(2), get_cost(3)};
        int left, right, bottom;
        int min_counts = *min_element(counts.begin(), counts.end());
        if (counts[1] == min_counts) {
            left = 2;
            right = 3;
            bottom = 1;
        } else if (counts[2] == min_counts) {
            left = 1;
            right = 3;
            bottom = 2;
        } else {
            left = 1;
            right = 2;
            bottom = 3;
        }
        char c = '$';
        for (int i = 0; i < 100 - 1; ++i) {
            if (f[i + 1] == bottom) {
                c = 'F';
            } else if (f[i] == bottom) {
                c = 'B';
            } else if (f[i + 1] == left) {
                if (c == 'R') {
                    c = 'B';
                } else {
                    c = 'R';
                }
            } else {
                if (c == 'L') {
                    c = 'B';
                } else {
                    c = 'L';
                }
            }
            output[i] = c;
        }
        output[100 - 1] = 'F';
        for (int i = 0; i < 100 - 1; ++i) {
            int p;
            cin >> p;
            state.put(f[i], p);
            vector<bool> skip(4, false);
            if (80 <= i) {
                output[i] = monte_carlo(i, skip);
            } else if (f[i + 1] == bottom) {
                skip[1] = true;
                output[i] = monte_carlo(i, skip);
            } else if (f[i] == bottom) {
                // nop
            } else if (f[i + 1] == left) {
                skip[0] = true;
                skip[2] = true;
                output[i] = monte_carlo(i, skip);
            } else if (f[i + 1] == right) {
                skip[0] = true;
                skip[3] = true;
                output[i] = monte_carlo(i, skip);
            }
            state.tilt(output[i]);
            cout << output[i] << endl;
        }
    }
};

int main() {
    vector<int> f(100);
    for (int i = 0; i < 100; ++i) {
        cin >> f[i];
    }

    Solver solver(f);
    solver.solve();

    return 0;
}
