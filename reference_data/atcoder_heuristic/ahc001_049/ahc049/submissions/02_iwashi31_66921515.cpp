//
// ver 1.8
//

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define rep(i, n) for (int i = 0; i < (n); i++)
#define REP(i, a, n) for (int i = (a); i < (n); i++)
#define all(a) (a).begin(), (a).end()
#define mp(a, b) make_pair((a), (b))

using namespace std;

const double TIME_LIMIT = 1980.0;

static uint64_t s[2]={123456789,362436069};
inline uint64_t rng64(){
    uint64_t s1=s[0], s0=s[1];
    s[0]=s0;
    s1^=s1<<23; s[1]=s1^s0^ (s1>>17) ^ (s0>>26);
    return s[1]+s0;
}
inline uint32_t rand_xor(){ return rng64()>>32; }

// 0〜n-1の一様乱数（剰余なし）
inline int rand_int(int n) {
    return (uint64_t(rand_xor()) * n) >> 32;
}

class Timer {
private:
    static const int LAZY_MASK = 0xFF;

    chrono::system_clock::time_point start;
    int call_cnt = 0;
    long long prev_time;
    bool is_lazy;

public:
    explicit Timer(bool is_lazy = false) : is_lazy(is_lazy) {
        start = chrono::system_clock::now();
    }

    long long get_ms() {
        if (!is_lazy || (call_cnt & LAZY_MASK) == 0) {
            auto end = chrono::system_clock::now();
            prev_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        }
        call_cnt++;
        return prev_time;
    }
};

class Point {
public:
    int x, y;
    Point() : x(-1), y(-1) {}
    Point(int x, int y) : x(x), y(y) {}

    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }

    friend ostream& operator<<(ostream& out, const Point &p) {
        out << '(' << p.x << ", " << p.y << ')';
        return out;
    }
};

struct Box {
    int weight;
    int durability;
};

const int board_size = 20;

using Board = array<array<Box, board_size>, board_size>;

namespace {
    unique_ptr<Timer> global_timer;
    ofstream fout;

    int calc_dist(const Point& a, const Point& b) {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }

    constexpr int S = 1024;
    constexpr int EXP_LUT_SIZE = 1024;
    double exp_lut[EXP_LUT_SIZE+1];
    inline double fast_exp(double x) {
        if (x <= 0) return 1.0;
        if (x >= 8.0) return 0.0;
        int idx = (int)(x / 8.0 * EXP_LUT_SIZE + 0.5);
        if (idx < 0) idx = 0;
        if (idx > EXP_LUT_SIZE) idx = EXP_LUT_SIZE;
        return exp_lut[idx];
    }
}

class Answer {
public:
    string actions;

    void print(ostream &out) {
        for (char c: actions) {
            out << c << endl;
        }
    }
};

class Solver {
private:
    string get_path(const Point& start, const Point& end) {
        string path;
        if (end.x > start.x) {
            path += string(end.x - start.x, 'R');
        } else if (end.x < start.x) {
            path += string(start.x - end.x, 'L');
        }
        if (end.y > start.y) {
            path += string(end.y - start.y, 'D');
        } else if (end.y < start.y) {
            path += string(start.y - end.y, 'U');
        }
        return path;
    }

protected:
    Answer construct_answer(const vector<vector<Point>>& paths) {
        Answer answer;

        Point now(0, 0);

        for (const auto& path: paths) {
            if (path.empty()) continue;
            for (const Point& p: path) {
                answer.actions += get_path(now, p);
                if (p != Point(0, 0)) {
                    answer.actions += '1';
                }
                now = p;
            }
            answer.actions += get_path(now, Point(0, 0));
            now = Point(0, 0);
        }

        return answer;
    }

public:
    virtual Answer solve(const Board& board) = 0;
};

class YamanoboriSolver : public Solver {
private:
    Board board;

    bool check_durability(const vector<Point>& path) {
        int dist_sum = 0;
        int minus_durability_sum = 1;
        for (int i = path.size() - 2; i >= 1; i--) {
            const auto& box = board[path[i].y][path[i].x];
            if (box.durability < minus_durability_sum) return false;
            dist_sum += calc_dist(path[i], path[i + 1]);
            minus_durability_sum += box.weight * dist_sum;
        }
        return true;
    }

public:
    Answer solve(const Board& _board) override {
        this->board = _board;

        vector<vector<Point>> paths;

        for (int y: ranges::iota_view(0, board_size)) {
            for (int x: ranges::iota_view(0, board_size)) {
                if (x == 0 && y == 0) continue;
                vector<Point> path;
                path.push_back(Point(0, 0));
                path.push_back(Point(x, y));
                path.push_back(Point(0, 0));
                paths.push_back(path);
            }
        }

        int loop_cnt = 0;
        const double T0 = 1e1, T1 = 1e-2;
        double start_time = global_timer->get_ms();
        double prev_now_time = -1;
        double temp = T0;

        int accepted = 0, total = 0;
        double last_report_time = start_time;

        while (true) {
            double now_time = global_timer->get_ms();
            if (now_time != prev_now_time) {
                double t = (now_time - start_time) / TIME_LIMIT;
                if (t > 1.0) break;
                temp = T0 * pow(T1 / T0, t);
                prev_now_time = now_time;
            }
            // 100msごとに採用率を出力
            if (now_time - last_report_time >= 100.0) {
                double rate = total ? (double)accepted / total : 0.0;
                fout << "accept_rate[" << (int)(now_time - start_time) << "]=" << rate << endl;
                last_report_time += 100.0;
                accepted = 0;
                total = 0;
            }

            if (rand_int(100) <= 90 && paths.size() >= 2) {
                // 2つのboxを入れ替える
                int path_idx1 = rand_int(paths.size());
                int path_idx2 = rand_int(paths.size());
                while (path_idx2 == path_idx1) path_idx2 = rand_int(paths.size());
                auto &path1 = paths[path_idx1];
                auto &path2 = paths[path_idx2];
                if (path1.size() <= 2 || path2.size() <= 2) continue;
                int box_idx1 = rand_int(path1.size() - 2) + 1;
                int box_idx2 = rand_int(path2.size() - 2) + 1;
                Point box1 = path1[box_idx1];
                Point box2 = path2[box_idx2];

                // 差分計算
                int diff = 0;
                // path1
                diff -= calc_dist(path1[box_idx1-1], box1);
                diff -= calc_dist(box1, path1[box_idx1+1]);
                diff += calc_dist(path1[box_idx1-1], box2);
                diff += calc_dist(box2, path1[box_idx1+1]);
                // path2
                diff -= calc_dist(path2[box_idx2-1], box2);
                diff -= calc_dist(box2, path2[box_idx2+1]);
                diff += calc_dist(path2[box_idx2-1], box1);
                diff += calc_dist(box1, path2[box_idx2+1]);

                total++;
                bool accept = false;
                if (diff <= 0) {
                    accept = true;
                } else {
                    double prob = fast_exp((double)diff / temp);
                    if (rand_int(1000000) < prob * 1000000) accept = true;
                }
                if (!accept) continue;
                accepted++;

                swap(path1[box_idx1], path2[box_idx2]);
                if (!check_durability(path1) || !check_durability(path2)) {
                    swap(path1[box_idx1], path2[box_idx2]); // 戻す
                    continue;
                }
                loop_cnt++;
            } else {
                // 1つのboxを移動する
                int from_path_idx = rand_int(paths.size());
                auto &from_path = paths[from_path_idx];
                if (from_path.size() == 2) continue;
                int from_box_idx = rand_int(from_path.size() - 2) + 1;
                Point box = from_path[from_box_idx];

                int to_path_idx = rand_int(paths.size());
                while (to_path_idx == from_path_idx) {
                    to_path_idx = rand_int(paths.size());
                }
                auto &to_path = paths[to_path_idx];
                int to_box_idx = rand_int(to_path.size() - 1) + 1;

                int diff = calc_dist(from_path[from_box_idx - 1], from_path[from_box_idx + 1])
                        - calc_dist(from_path[from_box_idx - 1], box)
                        - calc_dist(from_path[from_box_idx + 1], box)
                        + calc_dist(to_path[to_box_idx - 1], box)
                        + calc_dist(to_path[to_box_idx], box)
                        - calc_dist(to_path[to_box_idx - 1], to_path[to_box_idx]);

                total++;
                bool accept = false;
                if (diff <= 0) {
                    accept = true;
                } else {
                    double prob = fast_exp((double)diff / temp);
                    if (rand_int(1000000) < prob * 1000000) accept = true;
                }
                if (!accept) continue;
                accepted++;

                to_path.insert(to_path.begin() + to_box_idx, box);
                if (check_durability(to_path)) {
                    from_path.erase(from_path.begin() + from_box_idx);
                    if (from_path.size() == 2) {
                        paths.erase(paths.begin() + from_path_idx);
                    }
                } else {
                    to_path.erase(to_path.begin() + to_box_idx);
                }
                loop_cnt++;
            }
        }
        fout << "loop_cnt=" << loop_cnt << endl;

        return construct_answer(paths);
    }
};

int main(int argc, char *argv[]) {
    fout = ofstream(argc >= 2 ? argv[1] : "/dev/null");

    for(int i=0;i<=EXP_LUT_SIZE;i++) exp_lut[i] = exp(-i*8.0/EXP_LUT_SIZE);

    Board board;
    int trash;
    cin >> trash;

    for (int y: ranges::iota_view(0, board_size)) {
        for (int x: ranges::iota_view(0, board_size)) {
            cin >> board[y][x].weight;
        }
    }
    for (int y: ranges::iota_view(0, board_size)) {
        for (int x: ranges::iota_view(0, board_size)) {
            cin >> board[y][x].durability;
        }
    }
    global_timer = make_unique<Timer>(true);

    Solver *solver = new YamanoboriSolver();
    Answer answer = solver->solve(board);
    answer.print(cout);
}
