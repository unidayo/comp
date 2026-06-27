#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
using namespace std;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define ALL(v) (v).begin(), (v).end()
constexpr int dx[] = {-1, 1, 0, 0}, dy[] = {0, 0, -1, 1};
constexpr char DIR[] = {'U', 'D', 'L', 'R'};
constexpr char dir[] = {'u', 'd', 'l', 'r'};

constexpr double TIME_LIMIT = 2.5;

constexpr int K = 30;    // size of room
constexpr int D = 300;   // number of turns
constexpr double t = 1;  // threshold

constexpr double pet_type_score[3][5] = {
    {2.586189484822878, 2.6923573821592495, 2.338689489097925,
     2.029489779771395, 2.1383780829902},
    {2.7227143593942187, 2.457243210266714, 2.046980680166871,
     1.2586066767695407, 2.816797195042338},
    {2.784105441916319, 4.2583541099346105, 1.8372543886232775,
     2.068937605448871, 3.409896454746741}};

int N, M;

struct XorShift {
    unsigned int x, y, z, w, t;

    XorShift(int seed) {
        mt19937 rnd(seed);
        x = rnd();
        y = rnd();
        z = rnd();
        w = rnd();
        t = 1;
    }

    int rand() {
        t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        return w & 0x7fffffff;
    }
} rnd(rand());

struct Timer {
    chrono::system_clock::time_point start, now;

    Timer() {
        start = chrono::system_clock::now();
    }

    double getTime() {
        now = chrono::system_clock::now();
        return chrono::duration<double>(now - start).count();
    }
};

struct Board {
    vector<vector<bool>> is_wall_;
    vector<vector<bool>> is_human_;
    vector<vector<bool>> is_pet_;

    Board() {
        is_wall_.resize(K, vector<bool>(K));
        is_human_.resize(K, vector<bool>(K));
        is_pet_.resize(K, vector<bool>(K));
    }

    void clear_human() {
        fill(ALL(is_human_), vector<bool>(K, false));
    }

    void clear_pet() {
        fill(ALL(is_pet_), vector<bool>(K, false));
    }

    void set_wall(int x, int y) {
        is_wall_[x][y] = true;
    }

    void set_human(int x, int y) {
        is_human_[x][y] = true;
    }

    void set_pet(int x, int y) {
        is_pet_[x][y] = true;
    }

    bool is_wall(int x, int y) {
        return is_wall_[x][y];
    }

    bool is_human(int x, int y) {
        return is_human_[x][y];
    }

    bool is_pet(int x, int y) {
        return is_pet_[x][y];
    }

    vector<vector<int>> dist(int sx, int sy) {
        vector<vector<int>> dist(K, vector<int>(K, -1));
        queue<pair<int, int>> que;
        dist[sx][sy] = 0;
        que.emplace(sx, sy);
        while (!que.empty()) {
            int x, y;
            tie(x, y) = que.front();
            que.pop();
            rep(k, 4) {
                int nx = x + dx[k], ny = y + dy[k];
                if (nx < 0 || K <= nx || ny < 0 || K <= ny) continue;
                if (is_wall(nx, ny)) continue;
                if (dist[nx][ny] != -1) continue;
                dist[nx][ny] = dist[x][y] + 1;
                que.emplace(nx, ny);
            }
        }
        return dist;
    }

    char approach(int sx, int sy, int tx, int ty) {
        vector<vector<int>> dist(K, vector<int>(K, -1));
        queue<pair<int, int>> que;
        dist[tx][ty] = 0;
        que.emplace(tx, ty);
        while (!que.empty()) {
            int x, y;
            tie(x, y) = que.front();
            que.pop();
            rep(k, 4) {
                int nx = x + dx[k], ny = y + dy[k];
                if (nx < 0 || K <= nx || ny < 0 || K <= ny) continue;
                if (is_wall(nx, ny)) continue;
                if (dist[nx][ny] != -1) continue;
                dist[nx][ny] = dist[x][y] + 1;
                que.emplace(nx, ny);
            }
        }
        rep(k, 4) {
            int nx = sx + dx[k], ny = sy + dy[k];
            if (nx < 0 || K <= nx || ny < 0 || K <= ny) continue;
            if (is_wall(nx, ny)) continue;
            if (dist[nx][ny] == dist[sx][sy] - 1) {
                return DIR[k];
            }
        }
        return '.';
    }
};

struct Human {
    int x, y;

    Human() {}
    Human(int x, int y) : x(x), y(y) {}

    bool can(char c, Board &board) {
        rep(k, 4) {
            if (c == dir[k]) {
                int nx = x + dx[k], ny = y + dy[k];
                if (board.is_wall(nx, ny)) return false;
                if (board.is_human(nx, ny)) return false;
                if (board.is_pet(nx, ny)) return false;
                rep(l, 4) {
                    int nnx = nx + dx[l], nny = ny + dy[l];
                    if (nnx < 0 || K <= nnx || nny < 0 || K <= nny) continue;
                    if (board.is_pet(nnx, nny)) return false;
                }
            } else if (c == DIR[k]) {
                int nx = x + dx[k], ny = y + dy[k];
                if (board.is_wall(nx, ny)) return false;
            }
        }
        return true;
    }

    char action(char c, Board &board) {
        rep(k, 4) {
            int nx = x + dx[k], ny = y + dy[k];
            if (c == DIR[k]) {
                x = nx, y = ny;
                board.set_human(nx, ny);
                return c;
            }
            if (c == dir[k]) {
                board.set_wall(nx, ny);
                return c;
            }
        }
        return '.';
    }
};

struct Pet {
    int x, y, type;
    pair<int, int> dest;

    Pet() {}
    Pet(int x, int y, int type)
        : x(x), y(y), type(type), dest(pair<int, int>(-1, -1)) {}

    void action(string s, Board &board) {
        if (s == ".") {
            board.set_pet(x, y);
            return;
        }
        rep(i, (int)s.size()) {
            if (s[i] == 'U') {
                x--;
            } else if (s[i] == 'D') {
                x++;
            } else if (s[i] == 'L') {
                y--;
            } else {
                y++;
            }
        }
        board.set_pet(x, y);
    }

    void simulate(Board &board, vector<Human> &humans) {
        auto standard_move = [&] {
            vector<pair<int, int>> xy;
            rep(k, 4) {
                int nx = x + dx[k], ny = y + dy[k];
                if (nx < 0 || K <= nx || ny < 0 || K <= ny) continue;
                if (board.is_wall(nx, ny)) continue;
                xy.emplace_back(nx, ny);
            }
            tie(x, y) = xy[rnd.rand() % xy.size()];
        };
        if (type == 0) {
            standard_move();
            board.set_pet(x, y);
        } else if (type == 1) {
            rep(_, 2) {
                standard_move();
            }
            board.set_pet(x, y);
        } else if (type == 2) {
            rep(_, 3) {
                standard_move();
            }
            board.set_pet(x, y);
        } else if (type == 3) {
            auto dist = board.dist(x, y);
            auto select = [&] {
                vector<int> v;
                rep(m, M) {
                    if (dist[humans[m].x][humans[m].y] <= 0) {
                        continue;
                    }
                    v.emplace_back(m);
                }
                if (v.empty()) return -1;
                return v[rnd.rand() % v.size()];
            };
            if (dest.first == -1 ||
                dist[humans[dest.first].x][humans[dest.first].y] <= 0) {
                dest.first = select();
            }
            if (dest.first == -1) {
                standard_move();
                board.set_pet(x, y);
            } else {
                auto rdist =
                    board.dist(humans[dest.first].x, humans[dest.first].y);
                vector<pair<int, int>> xy;
                rep(k, 4) {
                    int nx = x + dx[k], ny = y + dy[k];
                    if (nx < 0 || K <= nx || ny < 0 || K <= ny) continue;
                    if (board.is_wall(nx, ny)) continue;
                    if (rdist[nx][ny] != rdist[x][y] - 1) continue;
                    xy.emplace_back(nx, ny);
                }
                tie(x, y) = xy[rnd.rand() % xy.size()];
                if (rdist[x][y] == 0) {
                    dest.first = -1;
                }
                standard_move();
                if (rdist[x][y] == 0) {
                    dest.first = -1;
                }
            }
            board.set_pet(x, y);
        } else {
            auto dist = board.dist(x, y);
            auto select = [&] {
                vector<pair<int, int>> v;
                rep(x, K) {
                    rep(y, K) {
                        if (dist[x][y] <= 0) {
                            continue;
                        }
                        v.emplace_back(x, y);
                    }
                }
                return v[rnd.rand() % v.size()];
            };
            if (dest.first == -1 || dist[dest.first][dest.second] <= 0) {
                tie(dest.first, dest.second) = select();
            }
            auto rdist = board.dist(dest.first, dest.second);
            vector<pair<int, int>> xy;
            rep(k, 4) {
                int nx = x + dx[k], ny = y + dy[k];
                if (nx < 0 || K <= nx || ny < 0 || K <= ny) continue;
                if (board.is_wall(nx, ny)) continue;
                if (rdist[nx][ny] != rdist[x][y] - 1) continue;
                xy.emplace_back(nx, ny);
            }
            tie(x, y) = xy[rnd.rand() % xy.size()];
            if (rdist[x][y] == 0) {
                dest.first = -1;
            }
            standard_move();
            if (rdist[x][y] == 0) {
                dest.first = -1;
            }
            board.set_pet(x, y);
        }
    }
};

struct Game {
    Board board;
    vector<Human> humans;
    vector<Pet> pets;

    void input() {
        cin >> N;
        rep(_, N) {
            int x, y, t;
            cin >> x >> y >> t;
            x--, y--, t--;
            board.set_pet(x, y);
            pets.emplace_back(x, y, t);
        }
        cin >> M;
        rep(_, M) {
            int x, y;
            cin >> x >> y;
            x--, y--;
            board.set_human(x, y);
            humans.emplace_back(x, y);
        }
    }

    double calc() {
        double res = 0;
        rep(m, M) {
            auto dist = board.dist(humans[m].x, humans[m].y);
            int R = 0;
            int n = 1;
            rep(x, K) {
                rep(y, K) {
                    if (dist[x][y] == -1) {
                        continue;
                    }
                    R++;
                    if (board.is_pet(x, y)) {
                        n *= 2;
                    }
                }
            }
            res += (1e8 * R) / (M * K * K * n);
        }
        return res;
    }
};

namespace SmallRooms {
constexpr int R = 66;  // number of small rooms

vector<pair<int, int>> starts;
vector<vector<int>> room_number(K, vector<int>(K, -1));
vector<vector<int>> room_place(R, vector<int>(2, -1));
vector<vector<pair<int, int>>> room_walls(R);
vector<int> sidx;  // humans : idx

vector<string> moves_;

void init(Game &game) {
    rep(x, K) {
        rep(y, K) {
            if (y % 3 == 1) {
                continue;
            }
            if (0 <= x && x <= 1) {
                room_number[x][y] = (y + 1) / 3;
            } else if (5 <= x && x <= 7) {
                room_number[x][y] = 11 + (y + 1) / 3;
            } else if (11 <= x && x <= 13) {
                room_number[x][y] = 22 + (y + 1) / 3;
            } else if (17 <= x && x <= 19) {
                room_number[x][y] = 33 + (y + 1) / 3;
            } else if (23 <= x && x <= 24) {
                room_number[x][y] = 44 + (y + 1) / 3;
            } else if (28 <= x && x <= 29) {
                room_number[x][y] = 55 + (y + 1) / 3;
            }
        }
    }
    rep(r, R) {
        if (r % 11 == 10) {
            room_place[r][1] = 29;
            continue;
        }
        room_place[r][1] = 3 * (r % 11);
    }
    rep(r, R) {
        if (r % 11 == 0) {
            room_place[r][0] = 0;
            continue;
        } else if (r % 11 == 10) {
            room_place[r][0] = 29;
            continue;
        }
        room_place[r][0] = 3 * (r % 11) - 1;
    }
    rep(r, R) {
        auto assign_walls = [&](int x, int y) {
            if (x < 0 || 29 < x || y <= 0 || 29 <= y) return;
            room_walls[r].emplace_back(x, y);
        };
        int rr = r % 11;
        if (r < 11) {
            for (int x = 0; x <= 1; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(2, 3 * rr - 1);
            assign_walls(2, 3 * rr + 2);
        } else if (r < 22) {
            assign_walls(4, 3 * rr - 3);
            assign_walls(4, 3 * rr);
            for (int x = 5; x <= 7; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(8, 3 * rr - 1);
            assign_walls(8, 3 * rr + 2);
        } else if (r < 33) {
            assign_walls(10, 3 * rr - 3);
            assign_walls(10, 3 * rr);
            for (int x = 11; x <= 13; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(14, 3 * rr - 1);
            assign_walls(14, 3 * rr + 2);
        } else if (r < 44) {
            assign_walls(16, 3 * rr - 3);
            assign_walls(16, 3 * rr);
            for (int x = 17; x <= 19; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(20, 3 * rr - 1);
            assign_walls(20, 3 * rr + 2);
        } else if (r < 55) {
            assign_walls(22, 3 * rr - 3);
            assign_walls(22, 3 * rr);
            for (int x = 23; x <= 24; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(25, 3 * rr - 1);
            assign_walls(25, 3 * rr + 2);
        } else {
            assign_walls(27, 3 * rr - 3);
            assign_walls(27, 3 * rr);
            for (int x = 28; x <= 29; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
        }
    }
    moves_.resize(M);
    if (M == 5) {
        starts.emplace_back(4, 4);
        moves_[0] +=
            "lDDuDuDurDDlDDuDuDurDDlDDuDuDurDDlDDuDurDDlDdLrLLdUdUUrUUdUdUUrUUd"
            "UdUdUUrUUdUdUdUUrUUdUdUdUUrUuRlRRuDurD";

        starts.emplace_back(8, 7);
        moves_[1] +=
            "rUUdUdUdlUUrUuRlRRuDurDDlDDuDuDurDDlDDuDuDurDDlDDuDuDurDDlDDuDurDD"
            "lDdLrLLdUdlUUrUUdUdlUUrUUdUdUdlUUrUUdUdUdlU";

        starts.emplace_back(16, 16);
        moves_[2] +=
            "lDDuDuDurDDlDDuDurDDlDdLrLLdUdlUUrUUdUdlUUrUUdUdUdlUUrUUdUdUdlUUrU"
            "UdUdUdlUUrUuRlRRuDurDDlDDuDuDurDDlDDuDuDurD";

        starts.emplace_back(20, 19);
        moves_[3] +=
            "rUUdUdUdlUUrUUdUdUdlUUrUUdUdUdlUUrUuRlRRuDurDDlDDuDuDurDDlDDuDuDur"
            "DDlDDuDuDurDDlDDuDurDDlDdLrLLdUdlUUrUUdUdlU";

        starts.emplace_back(27, 25);
        moves_[4] +=
            "lDdRlRRdUdlUUUUdUdlUUUUdUdUdlUUUUdUdUdlUUUUdUdUdlUUUuLrLLuDurDDlDD"
            "uDuDurDDlDDuDuDurDDlDDuDuDurDDlDDuDurD";
    } else if (M == 6) {
        starts.emplace_back(22, 4);
        moves_[0] +=
            "lDDuDurDDlDdLrLLdUdUUrUUdUdUUrUUdUdUdUUrUUdUdUdUUrUUdUdUdUUrUuRlRR"
            "uDurDDlDDuDuDurDDlDDuDuDurD";

        starts.emplace_back(25, 13);
        moves_[1] +=
            "rUUdUdlUUrUUdUdUdLLrLlDDuDuDurDDlDDuDurDDlDdLrLLdUdlUUrUUdUdlUUrUU"
            "dUdUdLLrLlDDuDuDurD";

        starts.emplace_back(14, 7);
        moves_[2] +=
            "rUUdUdUdlUUrUUdUdUdlUUrUuRlRRuDurDDlDDuDuDurDDlDDuDuDuRRlRrUUdUdUd"
            "lUUrUUdUdUdlUUrUuRlRRuDurD";

        starts.emplace_back(16, 22);
        moves_[3] +=
            "lDDuDuDurDDlDDuDurDDlDdLrLLdUdlUUrUUdUdlUUrUUdUdUdLLrLlDDuDuDurDDl"
            "DDuDurDDlDdLrLLdUdlU";

        starts.emplace_back(4, 16);
        moves_[4] +=
            "lDDuDuDurDDlDDuDuDuRRlRrUUdUdUdlUUrUUdUdUdlUUrUuRlRRuDurDDlDDuDuDu"
            "rDDlDDuDuDuRRlRrUUdUdUdlU";

        starts.emplace_back(8, 25);
        moves_[5] +=
            "rUUdUdUdlUUrUuRlRRuDuDDlDDuDuDuDDlDDuDuDuDDlDDuDuDuDDlDDuDuDDlDdLr"
            "LLdUdlUUrUUdUdlUUrUUdUdUdlU";
    } else if (M == 7) {
        starts.emplace_back(27, 4);
        moves_[0] +=
            "lDdLrLLdUdUUrUUdUdUUrUUdUdUdUUrUUdUdUdUUrUUdUdUdUUrUuRlRRuDurDDlDD"
            "uDuDurD";

        starts.emplace_back(10, 4);
        moves_[1] +=
            "lDDuDuDuRRlRrUUdUdUdlUUrUUdUdUdlUUrUuRlRRuDurDDlDDuDuDurDDlDDuDuDu"
            "RRlRrUUdUdUdlU";

        starts.emplace_back(8, 13);
        moves_[2] +=
            "rUUdUdUdlUUrUuRlRRuDurDDlDDuDuDurDDlDDuDuDuRRlRrUUdUdUdlUUrUUdUdUd"
            "lUUrUuRlRRuDurD";

        starts.emplace_back(4, 22);
        moves_[3] +=
            "lDDuDuDurDDlDDuDuDuRRlRrUUdUdUdlUUrUUdUdUdlUUrUuRlRRuDuDDlDDuDuDuD"
            "DlDDuDuDuDDlDDuDuDuD";

        starts.emplace_back(22, 28);
        moves_[4] +=
            "lDDuDuDDlDdLrLLdUdlUUrUUdUdlUUrUUdUdUdLLrLlDDuDuDurDDlDDuDurDDlDdL"
            "rLLdUdlU";

        starts.emplace_back(25, 19);
        moves_[5] +=
            "rUUdUdlUUrUUdUdUdLLrLlDDuDuDurDDlDDuDurDDlDdLrLLdUdlUUrUUdUdlUUrUU"
            "dUdUdlU";

        starts.emplace_back(25, 4);
        moves_[6] +=
            "rUUdUdlUUrUUdUdUdlRRRlDDuDuDurDDlDDuDurDDlDdRlRRdUdlUUrUUdUdlUUrUU"
            "dUdUdlU";
    } else if (M == 8) {
        starts.emplace_back(25, 1);
        moves_[0] +=
            "rUUdUdUUrUUdUdUdUUrUUdUdUdUUrUUdUdUdUUrUuRlRRuDurDDlDDuDuDurDDlDDu"
            "DuDurD";

        starts.emplace_back(14, 7);
        moves_[1] +=
            "rUUdUdUdlUUrUUdUdUdlUUrUuRlRRuDurDDlDDuDuDurDDlDDuDuDuRRlRrUUdUdUd"
            "lU";

        starts.emplace_back(4, 19);
        moves_[2] +=
            "lDDuDuDurDDlDDuDuDurLLLrUUdUdUdlUUrUUdUdUdlUUrUuLrLLuDurDDlDDuDuDu"
            "rD";

        starts.emplace_back(1, 27);
        moves_[3] +=
            "rUrLLDuDurDDlDDuDuDurDDlDDuDuDurLLLrUUdUdUdlUUrUUdUdUdlUUrUuLrLLuD"
            "urD";

        starts.emplace_back(4, 28);
        moves_[4] +=
            "lDDuDuDuDDlDDuDuDuDDlDDuDuDuDDlDDuDuDDlDdLrLLdUdlUUrUUdUdlUUrUUdUd"
            "UdlU";

        starts.emplace_back(16, 22);
        moves_[5] +=
            "lDDuDuDurDDlDDuDurDDlDdLrLLdUdlUUrUUdUdlUUrUUdUdUdLLrLlDDuDuDurD";

        starts.emplace_back(25, 10);
        moves_[6] +=
            "rUUdUdlUUrUUdUdUdlRRRlDDuDuDurDDlDDuDurDDlDdRlRRdUdlUUrUUdUdlU";

        starts.emplace_back(28, 2);
        moves_[7] +=
            "lDlRRUdUdlUUrUUdUdlUUrUUdUdUdlRRRlDDuDuDurDDlDDuDurDDlDdRlRRdUdlU";
    } else if (M == 9) {
        starts.emplace_back(12, 1);
        moves_[0] +=
            "uDuDurDDDDuDuDurDDDDuDurDDDdRlRRdUdlUUrUUdUdlUUrUUdUdUdlU";

        starts.emplace_back(4, 7);
        moves_[1] +=
            "lDDuDuDurDDlDDuDuDurLLLrUUdUdUdlUUrUUdUdUdlUUrUuLrLLuDurDDDDuDuDur"
            "D";

        starts.emplace_back(1, 8);
        moves_[2] +=
            "dlUlRRDuDurDDlDDuDuDurDDlDDuDuDuRRlRrUUdUdUdlUUrUUdUdUdlU";

        starts.emplace_back(1, 14);
        moves_[3] +=
            "dlUlRRDuDurDDlDDuDuDurDDlDDuDuDuRRlRrUUdUdUdlUUrUUdUdUdlU";

        starts.emplace_back(1, 20);
        moves_[4] +=
            "dlUlRRDuDurDDlDDuDuDurDDlDDuDuDurDDlDDuDuDuRRlRrUUdUdUdlUUrUUdUdUd"
            "lU";

        starts.emplace_back(8, 25);
        moves_[5] +=
            "rUUdUdUdlUUrUuRlRRuDuDDlDDuDuDuDDlDDuDuDuDDlDDuDuDuDDlDDuDuD";

        starts.emplace_back(28, 27);
        moves_[6] += "urDrLLUdUdlUUrUUdUdLLrLlDDuDurDDlDdLrLLdUdlUUrUUdUdlU";

        starts.emplace_back(16, 13);
        moves_[7] +=
            "lDDuDuDurDDlDDuDurDDlDdRlRRdUdlUUrUUdUdlUUrUUdUdUdlRRRlDDuDuDurD";

        starts.emplace_back(16, 7);
        moves_[8] += "lDDuDuDurDDlDDuDurDDlDdRlRRdUdlUUrUUdUdlUUrUUdUdUdlU";
    } else if (M == 10) {
        starts.emplace_back(25, 4);
        moves_[0] += "rUUdUdlUUrUUdUdUdlUUrUUdUdUdlUUrUUdUdUdlUUrUuLrLLuDurD";

        starts.emplace_back(6, 1);
        moves_[1] += "uDuDurDDDDuDuDurDDDDuDuDurDDDDuDurDDDdRlRRdUdlU";

        starts.emplace_back(20, 7);
        moves_[2] += "rUUdUdUdlUUrUUdUdUdlUUrUUdUdUdlUUrUuRlRRuDurDDlDDuDuDurD";

        starts.emplace_back(10, 10);
        moves_[3] += "lDDuDuDurDDlDDuDuDurDDlDDuDurDDlDdLrLLdUdlUUrUUdUdlU";

        starts.emplace_back(14, 16);
        moves_[4] += "rUUdUdUdlUUrUUdUdUdlUUrUuLrLLuDurDDlDDuDuDurDDlDDuDuDurD";

        starts.emplace_back(16, 13);
        moves_[5] += "lDDuDuDurDDlDDuDurDDlDdRlRRdUdlUUrUUdUdlUUrUUdUdUdlU";

        starts.emplace_back(20, 19);
        moves_[6] += "rUUdUdUdlUUrUUdUdUdlUUrUUdUdUdlUUrUuRlRRuDurDDlDDuDuDurD";

        starts.emplace_back(10, 22);
        moves_[7] += "lDDuDuDurDDlDDuDuDurDDlDDuDurDDlDdLrLLdUdlUUrUUdUdlU";

        starts.emplace_back(23, 28);
        moves_[8] += "dUdlUUUUdUdUdlUUUUdUdUdlUUUUdUdUdlUUUuLrLLuDurD";

        starts.emplace_back(4, 25);
        moves_[9] += "lDDuDuDurDDlDDuDuDurDDlDDuDuDurDDlDDuDurDDlDdRlRRdUdlU";
    }
    rep(m, M) {
        if (!moves_[m].empty()) {
            reverse(ALL(moves_[m]));
        }
    }
    vector<vector<int>> dist(M, vector<int>(M));
    rep(i, M) {
        rep(j, M) {
            dist[i][j] = abs(game.humans[i].x - starts[j].first) +
                         abs(game.humans[i].y - starts[j].second);
        }
    }
    vector<int> v(M);
    iota(ALL(v), 0);
    int mn = INT_MAX;
    do {
        int sum = 0;
        rep(i, M) {
            sum = max(dist[i][v[i]], sum);
        }
        if (sum < mn) {
            sidx = v;
            mn = sum;
        }
    } while (next_permutation(ALL(v)));
}

void solve(Game &game) {
    auto moves = moves_;
    vector<int> phase(M);  // 0 : move, 1 : wall, 2 : catch
    set<int> FR;           // free rooms
    vector<bool> built(R);
    rep(d, D) {
        vector<char> output(M, '.');
        rep(r, R) {
            if (built[r]) continue;
            bool flag = true;
            for (auto p : room_walls[r]) {
                if (!game.board.is_wall(p.first, p.second)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                FR.emplace(r);
                built[r] = true;
            }
        }
        vector<double> pet_score(R);
        rep(n, N) {
            int x = game.pets[n].x, y = game.pets[n].y,
                type = game.pets[n].type;
            if (room_number[x][y] == -1) {
                auto dist = game.board.dist(x, y);
                vector<int> mn(R, INT_MAX);
                rep(xx, K) {
                    rep(yy, K) {
                        if (room_number[xx][yy] == -1 || dist[xx][yy] == -1) {
                            continue;
                        }
                        mn[room_number[xx][yy]] =
                            min(mn[room_number[xx][yy]], dist[xx][yy]);
                    }
                }
                for (auto r : FR) {
                    if (mn[r] == INT_MAX) {
                        continue;
                    }
                    pet_score[r] += 1.0 / ((mn[r] + 1) * (mn[r] + 1));
                }
                continue;
            }
            pet_score[room_number[x][y]] += pet_type_score[0][type];
        }
        set<int> FH;  // free humans
        rep(m, M) {
            if (phase[m] == 2) {
                FH.emplace(m);
            }
        }
        while (!FH.empty()) {
            vector<pair<double, int>> v;  // (score, room)
            for (auto r : FR) {
                if (r < 11) {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 3) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][1]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                } else if (r < 22) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 3) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 9) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 33) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 9) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 15) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 44) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 15) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 21) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 55) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 21) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 26) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 26) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][0]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                }
            }
            if (v.empty()) break;
            sort(ALL(v));
            reverse(ALL(v));
            int r;
            double score;
            tie(score, r) = v.front();
            if (score <= 0) break;
            pet_score[r] = 0;
            if (r < 11) {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 3) {
                        int dist = abs(game.humans[m].y - room_place[r][1]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('u', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][1]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            } else if (r < 22) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 3) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 9) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 33) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 9) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 15) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 44) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 15) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 21) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 55) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 21) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 26) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 26) {
                        int dist = abs(game.humans[m].y - room_place[r][0]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('d', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('d', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][0]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            }
        }
        rep(m, M) {
            int idx = sidx[m];
            if (phase[m] == 0) {
                output[m] = game.humans[m].action(
                    game.board.approach(game.humans[m].x, game.humans[m].y,
                                        starts[idx].first, starts[idx].second),
                    game.board);
                if (game.humans[m].x == starts[idx].first &&
                    game.humans[m].y == starts[idx].second) {
                    phase[m] = 1 + moves[idx].empty();
                }
            } else if (phase[m] == 1) {
                if (game.humans[m].can(moves[idx].back(), game.board)) {
                    output[m] =
                        game.humans[m].action(moves[idx].back(), game.board);
                    moves[idx].pop_back();
                } else {
                    output[m] = game.humans[m].action('.', game.board);
                }
                if (moves[idx].empty()) phase[m] = 2;
            }
        }
        rep(m, M) {
            cout << output[m];
        }
        cout << endl;
        game.board.clear_human();
        rep(m, M) {
            game.board.set_human(game.humans[m].x, game.humans[m].y);
        }
        game.board.clear_pet();
        rep(n, N) {
            string s;
            cin >> s;
            game.pets[n].action(s, game.board);
        }
    }
}

double simulate(Game &game) {
    auto moves = moves_;
    vector<int> phase(M);  // 0 : move, 1 : wall, 2 : catch
    set<int> FR;           // free rooms
    vector<bool> built(R);
    rep(d, D) {
        vector<char> output(M, '.');
        rep(r, R) {
            if (built[r]) continue;
            bool flag = true;
            for (auto p : room_walls[r]) {
                if (!game.board.is_wall(p.first, p.second)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                FR.emplace(r);
                built[r] = true;
            }
        }
        vector<double> pet_score(R);
        rep(n, N) {
            int x = game.pets[n].x, y = game.pets[n].y,
                type = game.pets[n].type;
            if (room_number[x][y] == -1) {
                auto dist = game.board.dist(x, y);
                vector<int> mn(R, INT_MAX);
                rep(xx, K) {
                    rep(yy, K) {
                        if (room_number[xx][yy] == -1 || dist[xx][yy] == -1) {
                            continue;
                        }
                        mn[room_number[xx][yy]] =
                            min(mn[room_number[xx][yy]], dist[xx][yy]);
                    }
                }
                for (auto r : FR) {
                    if (mn[r] == INT_MAX) {
                        continue;
                    }
                    pet_score[r] += 1.0 / ((mn[r] + 1) * (mn[r] + 1));
                }
                continue;
            }
            pet_score[room_number[x][y]] += pet_type_score[0][type];
        }
        set<int> FH;  // free humans
        rep(m, M) {
            if (phase[m] == 2) {
                FH.emplace(m);
            }
        }
        while (!FH.empty()) {
            vector<pair<double, int>> v;  // (score, room)
            for (auto r : FR) {
                if (r < 11) {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 3) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][1]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                } else if (r < 22) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 3) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 9) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 33) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 9) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 15) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 44) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 15) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 21) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 55) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 21) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 26) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 26) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][0]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                }
            }
            if (v.empty()) break;
            sort(ALL(v));
            reverse(ALL(v));
            int r;
            double score;
            tie(score, r) = v.front();
            if (score <= 0) break;
            pet_score[r] = 0;
            if (r < 11) {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 3) {
                        int dist = abs(game.humans[m].y - room_place[r][1]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('u', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][1]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            } else if (r < 22) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 3) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 9) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 33) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 9) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 15) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 44) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 15) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 21) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 55) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 21) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 26) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 26) {
                        int dist = abs(game.humans[m].y - room_place[r][0]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('d', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('d', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][0]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            }
        }
        rep(m, M) {
            int idx = sidx[m];
            if (phase[m] == 0) {
                output[m] = game.humans[m].action(
                    game.board.approach(game.humans[m].x, game.humans[m].y,
                                        starts[idx].first, starts[idx].second),
                    game.board);
                if (game.humans[m].x == starts[idx].first &&
                    game.humans[m].y == starts[idx].second) {
                    phase[m] = 1 + moves[idx].empty();
                }
            } else if (phase[m] == 1) {
                if (game.humans[m].can(moves[idx].back(), game.board)) {
                    output[m] =
                        game.humans[m].action(moves[idx].back(), game.board);
                    moves[idx].pop_back();
                } else {
                    output[m] = game.humans[m].action('.', game.board);
                }
                if (moves[idx].empty()) phase[m] = 2;
            }
        }
        game.board.clear_human();
        rep(m, M) {
            game.board.set_human(game.humans[m].x, game.humans[m].y);
        }
        game.board.clear_pet();
        rep(n, N) {
            game.pets[n].simulate(game.board, game.humans);
        }
    }
    return game.calc();
}
}  // namespace SmallRooms

namespace MediumRooms {
constexpr int R = 55;  // number of medium rooms

vector<pair<int, int>> starts;
vector<vector<int>> room_number(K, vector<int>(K, -1));
vector<vector<int>> room_place(R, vector<int>(2, -1));
vector<vector<pair<int, int>>> room_walls(R);
vector<int> sidx;  // humans : idx

vector<string> moves_;

void init(Game &game) {
    rep(x, K) {
        rep(y, K) {
            if (y % 3 == 1) {
                continue;
            }
            if (0 <= x && x <= 2) {
                room_number[x][y] = (y + 1) / 3;
            } else if (6 <= x && x <= 9) {
                room_number[x][y] = 11 + (y + 1) / 3;
            } else if (13 <= x && x <= 16) {
                room_number[x][y] = 22 + (y + 1) / 3;
            } else if (20 <= x && x <= 23) {
                room_number[x][y] = 33 + (y + 1) / 3;
            } else if (27 <= x && x <= 29) {
                room_number[x][y] = 44 + (y + 1) / 3;
            }
        }
    }
    rep(r, R) {
        if (r % 11 == 10) {
            room_place[r][1] = 29;
            continue;
        }
        room_place[r][1] = 3 * (r % 11);
    }
    rep(r, R) {
        if (r % 11 == 0) {
            room_place[r][0] = 0;
            continue;
        } else if (r % 11 == 10) {
            room_place[r][0] = 29;
            continue;
        }
        room_place[r][0] = 3 * (r % 11) - 1;
    }
    rep(r, R) {
        auto assign_walls = [&](int x, int y) {
            if (x < 0 || 29 < x || y <= 0 || 29 <= y) return;
            room_walls[r].emplace_back(x, y);
        };
        int rr = r % 11;
        if (r < 11) {
            for (int x = 0; x <= 2; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(3, 3 * rr - 1);
            assign_walls(3, 3 * rr + 2);
        } else if (r < 22) {
            assign_walls(5, 3 * rr - 3);
            assign_walls(5, 3 * rr);
            for (int x = 6; x <= 9; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(10, 3 * rr - 1);
            assign_walls(10, 3 * rr + 2);
        } else if (r < 33) {
            assign_walls(12, 3 * rr - 3);
            assign_walls(12, 3 * rr);
            for (int x = 13; x <= 16; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(17, 3 * rr - 1);
            assign_walls(17, 3 * rr + 2);
        } else if (r < 44) {
            assign_walls(19, 3 * rr - 3);
            assign_walls(19, 3 * rr);
            for (int x = 20; x <= 23; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(24, 3 * rr - 1);
            assign_walls(24, 3 * rr + 2);
        } else {
            assign_walls(26, 3 * rr - 3);
            assign_walls(26, 3 * rr);
            for (int x = 27; x <= 29; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
        }
    }
    moves_.resize(M);
    if (M == 5) {
        starts.emplace_back(17, 1);
        moves_[0] +=
            "rUUdUdUdUdUUrUUdUdUdUdUUrUUduRlRRuDuDurDDlDDuDuDuDurDDlDDuDuDuDurD"
            "DlDDuDuDuDurDDlDDudLrLLdUdUdUUrUUdUdUdUdU";

        starts.emplace_back(3, 7);
        moves_[1] +=
            "rUUduRlRRuDuDurDDlDDuDuDuDurDDlDDuDuDuDurDDlDDuDuDuDurDDlDDudLrLLd"
            "UdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlU";

        starts.emplace_back(12, 13);
        moves_[2] +=
            "lDDuDuDuDurDDlDDuDuDuDurDDlDDudRlRRdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUd"
            "lUUrUUdUdUdUdlUUrUUduLrLLuDuDurDDlDDuDuDuDurD";

        starts.emplace_back(24, 19);
        moves_[3] +=
            "rUUdUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlUUrUUduRlRRuDuDurDDlDDuDuDuDu"
            "rDDlDDuDuDuDurDDlDDuDuDuDurDDlDDudLrLLdUdUdlU";

        starts.emplace_back(17, 25);
        moves_[4] +=
            "rUUdUdUdUdlUUrUUdUdUdUdlUUrUUduRlRRuDuDuDDlDDuDuDuDuDDlDDuDuDuDuDD"
            "lDDuDuDuDuDDlDDudLrLLdUdUdlUUrUUdUdUdUdlU";
    } else if (M == 6) {
        starts.emplace_back(26, 4);
        moves_[0] +=
            "lDDudLrLLdUdUdUUrUUdUdUdUdUUrUUdUdUdUdUUrUUdUdUdUdUUrUUduRlRRuDuDu"
            "rDDlDDuDuDuDurD";

        starts.emplace_back(24, 10);
        moves_[1] +=
            "rUUdUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlUUrUUduLrLLuDuDurDDlDDuDuDuDu"
            "rDLLLDlDDuDuDuDurDDDDuDuDuDurLUUUUUUd";

        starts.emplace_back(12, 7);
        moves_[2] +=
            "lDDuDuDuDurDDlDDuDuDuDurDDlDDudRlRRdUdUdlRRRlDDudRlRRdUdUdlUUrUUdU"
            "dUdUdLLrLlDDuDuDuDurD";

        starts.emplace_back(3, 22);
        moves_[3] +=
            "rUUduLrLLuDuDurLLLrUUduLrLLuDuDurDDlDDuDuDuDurDDlDDuDuDuDuRRlRrUUd"
            "UdUdUdlUUrUUdUdUdUdlU";

        starts.emplace_back(10, 22);
        moves_[4] +=
            "rUUdUdUdUdLLrLlDDuDuDuDurDDlDDuDuDuDurDDlDDuDuDuDurDDlDDudRlRRdUdU"
            "dlUUrUUdUdUdUdlUUrUUdUdUdUdlRRRlDDuDuDuDurD";

        starts.emplace_back(19, 25);
        moves_[5] +=
            "lDDuDuDuDurDDlDDudRlRRdUdUdlUUUUdUdUdUdlUUUUdUdUdUdlUUUUdUdUdUdlUU"
            "UUduLrLLuDuDurDDlDDuDuDuDurD";
    } else if (M == 7) {
        starts.emplace_back(27, 3);
        moves_[0] +=
            "urDrDrLLUdUdUdUUrUUdUdUdUdUUrUUdUdUdUdUUrUUdUdUdUdUUrUUduRlRRuDuDu"
            "rDDlDDuDuDuDurD";

        starts.emplace_back(19, 10);
        moves_[1] +=
            "lDDuDuDuDurDDlDDudLrLLdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdLLrLlDDuDuDuD"
            "urDDlDDuDuDuDurD";

        starts.emplace_back(10, 7);
        moves_[2] +=
            "rUUdUdUdUdlUUrUUduRlRRuDuDurDDlDDuDuDuDuRRlRrUUdUdUdUdlUUrUUduRlRR"
            "uDuDurD";

        starts.emplace_back(12, 16);
        moves_[3] +=
            "lDDuDuDuDurDDlDDuDuDuDurDDlDDudLrLLdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUd"
            "LLrLlDDuDuDuDurD";

        starts.emplace_back(1, 25);
        moves_[4] +=
            "uDuDurDDlDDuDuDuDurLLLrUUdUdUdUdlUUrUUduLrLLuDuDurDDlDDuDuDuDurLLL"
            "rUUdUdUdUdlU";

        starts.emplace_back(17, 25);
        moves_[5] +=
            "rUUdUdUdUdLLrLlDDuDuDuDurDDlDDuDuDuDurDDlDDudLrLLdUdUdlUUrUUdUdUdU"
            "dlUUrUUdUdUdUdlU";

        starts.emplace_back(0, 27);
        moves_[6] +=
            "rDrDrDDDDurDrDrDrDDDDurDrDrDrDDDDurDrDrDrDDDDurDrDrLLUdUdUdlUUrUUd"
            "UdUdUdlU";
    } else if (M == 8) {
        starts.emplace_back(3, 4);
        moves_[0] +=
            "rUUduLrLLuDuDurDDDDuDuDuDurDDDDuDuDuDurDDDDuDuDuDurDDDDudRlRRdUdUd"
            "lU";

        starts.emplace_back(24, 4);
        moves_[1] +=
            "rUUdUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlURRRUrUUduRlRRuDuDurDDlDDuDuD"
            "uDurD";

        starts.emplace_back(19, 10);
        moves_[2] +=
            "lDDuDuDuDurDDlDDudLrLLdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUd"
            "lU";

        starts.emplace_back(19, 16);
        moves_[3] +=
            "lDDuDuDuDurDDlDDudLrLLdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdLLrLlDDuDuDuD"
            "urD";

        starts.emplace_back(10, 13);
        moves_[4] +=
            "rUUdUdUdUdlUUrUUduRlRRuDuDurDDlDDuDuDuDurDDlDDuDuDuDuRRlRrUUdUdUdU"
            "dlU";

        starts.emplace_back(10, 19);
        moves_[5] +=
            "rUUdUdUdUdlUUrUUduRlRRuDuDurDDlDDuDuDuDurDDlDDuDuDuDurDDlDDuDuDuDu"
            "rD";

        starts.emplace_back(5, 25);
        moves_[6] +=
            "lDDuDuDuDurDDlDDuDuDuDurDDlDDuDuDuDurDLLLDlDDudLrLLdUdUdlUUrUUdUdU"
            "dUdlU";

        starts.emplace_back(26, 25);
        moves_[7] +=
            "lDDudRlRRdUdUdlUUUUdUdUdUdlUUUUdUdUdUdlUUUUdUdUdUdlUUUUduLrLLuDuDu"
            "rD";
    } else if (M == 9) {
        starts.emplace_back(24, 1);
        moves_[0] +=
            "rUUdUdUdUdUUrUUdUdUdUdUUrUUdUdUdUdUUrUUduRlRRuDuDurDDlDDuDuDuDurD";

        starts.emplace_back(27, 2);
        moves_[1] +=
            "lDlDlRRUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlRRRlDDuDuDuDurDDlDDuDuDuDu"
            "rD";

        starts.emplace_back(26, 7);
        moves_[2] += "lDDudRlRRdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlU";

        starts.emplace_back(10, 7);
        moves_[3] += "rUUdUdUdUdlUUrUUduRlRRuDuDuRRlRrUUduRlRRuDuDurD";

        starts.emplace_back(5, 13);
        moves_[4] +=
            "lDDuDuDuDurDDlDDuDuDuDurDDlDDuDuDuDurDDlDDudRlRRdUdUdlUUrUUdUdUdUd"
            "lU";

        starts.emplace_back(10, 22);
        moves_[5] +=
            "rUUdUdUdUdlUUrUUduLrLLuDuDurDLLLDlDDuDuDuDurDDlDDuDuDuDurD";

        starts.emplace_back(26, 22);
        moves_[6] += "lDDudLrLLdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlU";

        starts.emplace_back(27, 27);
        moves_[7] +=
            "urDrDrLLUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdLLrLlDDuDuDuDurDDlDDuDuDuD"
            "urD";

        starts.emplace_back(22, 28);
        moves_[8] +=
            "dUdUdUdlUUUUdUdUdUdlUUUUdUdUdUdlUUUUduLrLLuDuDurDDlDDuDuDuDurD";
    } else if (M == 10) {
        starts.emplace_back(10, 4);
        moves_[0] += "rUUdUdUdUdlUUrUUduLrLLuDuDurDDDDuDuDuDurDDDDuDuDuDurD";

        starts.emplace_back(21, 1);
        moves_[1] += "uDuDuDurDDDDudRlRRdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlU";

        starts.emplace_back(5, 7);
        moves_[2] += "lDDuDuDuDurDDlDDuDuDuDurDDlDDuDuDuDurDDlDDudRlRRdUdUdlU";

        starts.emplace_back(24, 10);
        moves_[3] += "rUUdUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlUUrUUduLrLLuDuDurD";

        starts.emplace_back(10, 16);
        moves_[4] += "rUUdUdUdUdlUUrUUduLrLLuDuDurDDlDDuDuDuDurDDlDDuDuDuDurD";

        starts.emplace_back(19, 13);
        moves_[5] += "lDDuDuDuDurDDlDDudRlRRdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlU";

        starts.emplace_back(5, 19);
        moves_[6] += "lDDuDuDuDurDDlDDuDuDuDurDDlDDuDuDuDurDDlDDudRlRRdUdUdlU";

        starts.emplace_back(24, 22);
        moves_[7] += "rUUdUdUdUdlUUrUUdUdUdUdlUUrUUdUdUdUdlUUrUUduLrLLuDuDurD";

        starts.emplace_back(8, 28);
        moves_[8] += "dUdUdUdlUUUUduLrLLuDuDurDDlDDuDuDuDurDDlDDuDuDuDurD";

        starts.emplace_back(19, 25);
        moves_[9] += "lDDuDuDuDurDDDDudRlRRdUdUdlUUUUdUdUdUdlUUUUdUdUdUdlU";
    }
    rep(m, M) {
        if (!moves_[m].empty()) {
            reverse(ALL(moves_[m]));
        }
    }
    vector<vector<int>> dist(M, vector<int>(M));
    rep(i, M) {
        rep(j, M) {
            dist[i][j] = abs(game.humans[i].x - starts[j].first) +
                         abs(game.humans[i].y - starts[j].second);
        }
    }
    vector<int> v(M);
    iota(ALL(v), 0);
    int mn = INT_MAX;
    do {
        int sum = 0;
        rep(i, M) {
            sum = max(dist[i][v[i]], sum);
        }
        if (sum < mn) {
            sidx = v;
            mn = sum;
        }
    } while (next_permutation(ALL(v)));
}

void solve(Game &game) {
    auto moves = moves_;
    vector<int> phase(M);  // 0 : move, 1 : wall, 2 : catch
    set<int> FR;           // free rooms
    vector<bool> built(R);
    rep(d, D) {
        vector<char> output(M, '.');
        rep(r, R) {
            if (built[r]) continue;
            bool flag = true;
            for (auto p : room_walls[r]) {
                if (!game.board.is_wall(p.first, p.second)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                FR.emplace(r);
                built[r] = true;
            }
        }
        vector<double> pet_score(R);
        rep(n, N) {
            int x = game.pets[n].x, y = game.pets[n].y,
                type = game.pets[n].type;
            if (room_number[x][y] == -1) {
                auto dist = game.board.dist(x, y);
                vector<int> mn(R, INT_MAX);
                rep(xx, K) {
                    rep(yy, K) {
                        if (room_number[xx][yy] == -1 || dist[xx][yy] == -1) {
                            continue;
                        }
                        mn[room_number[xx][yy]] =
                            min(mn[room_number[xx][yy]], dist[xx][yy]);
                    }
                }
                for (auto r : FR) {
                    if (mn[r] == INT_MAX) {
                        continue;
                    }
                    pet_score[r] += 1.0 / ((mn[r] + 1) * (mn[r] + 1));
                }
                continue;
            }
            pet_score[room_number[x][y]] += pet_type_score[1][type];
        }
        set<int> FH;  // free humans
        rep(m, M) {
            if (phase[m] == 2) {
                FH.emplace(m);
            }
        }
        while (!FH.empty()) {
            vector<pair<double, int>> v;  // (score, room)
            for (auto r : FR) {
                if (r < 11) {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 4) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][1]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                } else if (r < 22) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 4) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 11) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 33) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 11) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 18) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 44) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 18) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 25) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 25) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][0]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                }
            }
            if (v.empty()) break;
            sort(ALL(v));
            reverse(ALL(v));
            int r;
            double score;
            tie(score, r) = v.front();
            if (score <= 0) break;
            pet_score[r] = 0;
            if (r < 11) {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 4) {
                        int dist = abs(game.humans[m].y - room_place[r][1]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('u', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][1]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            } else if (r < 22) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 4) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 11) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 33) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 11) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 18) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 44) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 18) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 25) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 25) {
                        int dist = abs(game.humans[m].y - room_place[r][0]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('d', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('d', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][0]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            }
        }
        rep(m, M) {
            int idx = sidx[m];
            if (phase[m] == 0) {
                output[m] = game.humans[m].action(
                    game.board.approach(game.humans[m].x, game.humans[m].y,
                                        starts[idx].first, starts[idx].second),
                    game.board);
                if (game.humans[m].x == starts[idx].first &&
                    game.humans[m].y == starts[idx].second) {
                    phase[m] = 1 + moves[idx].empty();
                }
            } else if (phase[m] == 1) {
                if (game.humans[m].can(moves[idx].back(), game.board)) {
                    output[m] =
                        game.humans[m].action(moves[idx].back(), game.board);
                    moves[idx].pop_back();
                } else {
                    output[m] = game.humans[m].action('.', game.board);
                }
                if (moves[idx].empty()) phase[m] = 2;
            }
        }
        rep(m, M) {
            cout << output[m];
        }
        cout << endl;
        game.board.clear_human();
        rep(m, M) {
            game.board.set_human(game.humans[m].x, game.humans[m].y);
        }
        game.board.clear_pet();
        rep(n, N) {
            string s;
            cin >> s;
            game.pets[n].action(s, game.board);
        }
    }
}

double simulate(Game &game) {
    auto moves = moves_;
    vector<int> phase(M);  // 0 : move, 1 : wall, 2 : catch
    set<int> FR;           // free rooms
    vector<bool> built(R);
    rep(d, D) {
        vector<char> output(M, '.');
        rep(r, R) {
            if (built[r]) continue;
            bool flag = true;
            for (auto p : room_walls[r]) {
                if (!game.board.is_wall(p.first, p.second)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                FR.emplace(r);
                built[r] = true;
            }
        }
        vector<double> pet_score(R);
        rep(n, N) {
            int x = game.pets[n].x, y = game.pets[n].y,
                type = game.pets[n].type;
            if (room_number[x][y] == -1) {
                auto dist = game.board.dist(x, y);
                vector<int> mn(R, INT_MAX);
                rep(xx, K) {
                    rep(yy, K) {
                        if (room_number[xx][yy] == -1 || dist[xx][yy] == -1) {
                            continue;
                        }
                        mn[room_number[xx][yy]] =
                            min(mn[room_number[xx][yy]], dist[xx][yy]);
                    }
                }
                for (auto r : FR) {
                    if (mn[r] == INT_MAX) {
                        continue;
                    }
                    pet_score[r] += 1.0 / ((mn[r] + 1) * (mn[r] + 1));
                }
                continue;
            }
            pet_score[room_number[x][y]] += pet_type_score[1][type];
        }
        set<int> FH;  // free humans
        rep(m, M) {
            if (phase[m] == 2) {
                FH.emplace(m);
            }
        }
        while (!FH.empty()) {
            vector<pair<double, int>> v;  // (score, room)
            for (auto r : FR) {
                if (r < 11) {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 4) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][1]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                } else if (r < 22) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 4) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 11) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 33) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 11) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 18) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 44) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 18) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 25) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 25) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][0]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                }
            }
            if (v.empty()) break;
            sort(ALL(v));
            reverse(ALL(v));
            int r;
            double score;
            tie(score, r) = v.front();
            if (score <= 0) break;
            pet_score[r] = 0;
            if (r < 11) {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 4) {
                        int dist = abs(game.humans[m].y - room_place[r][1]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('u', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][1]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            } else if (r < 22) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 4) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 11) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 33) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 11) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 18) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 44) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 18) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 25) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 25) {
                        int dist = abs(game.humans[m].y - room_place[r][0]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('d', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('d', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][0]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            }
        }
        rep(m, M) {
            int idx = sidx[m];
            if (phase[m] == 0) {
                output[m] = game.humans[m].action(
                    game.board.approach(game.humans[m].x, game.humans[m].y,
                                        starts[idx].first, starts[idx].second),
                    game.board);
                if (game.humans[m].x == starts[idx].first &&
                    game.humans[m].y == starts[idx].second) {
                    phase[m] = 1 + moves[idx].empty();
                }
            } else if (phase[m] == 1) {
                if (game.humans[m].can(moves[idx].back(), game.board)) {
                    output[m] =
                        game.humans[m].action(moves[idx].back(), game.board);
                    moves[idx].pop_back();
                } else {
                    output[m] = game.humans[m].action('.', game.board);
                }
                if (moves[idx].empty()) phase[m] = 2;
            }
        }
        game.board.clear_human();
        rep(m, M) {
            game.board.set_human(game.humans[m].x, game.humans[m].y);
        }
        game.board.clear_pet();
        rep(n, N) {
            game.pets[n].simulate(game.board, game.humans);
        }
    }
    return game.calc();
}
}  // namespace MediumRooms

namespace LargeRooms {
constexpr int R = 44;  // number of large rooms

vector<pair<int, int>> starts;
vector<vector<int>> room_number(K, vector<int>(K, -1));
vector<vector<int>> room_place(R, vector<int>(2, -1));
vector<vector<pair<int, int>>> room_walls(R);
vector<int> sidx;  // humans : idx

vector<string> moves_;

void init(Game &game) {
    rep(x, K) {
        rep(y, K) {
            if (y % 3 == 1) {
                continue;
            }
            if (0 <= x && x <= 4) {
                room_number[x][y] = (y + 1) / 3;
            } else if (8 <= x && x <= 12) {
                room_number[x][y] = 11 + (y + 1) / 3;
            } else if (16 <= x && x <= 21) {
                room_number[x][y] = 22 + (y + 1) / 3;
            } else if (25 <= x && x <= 29) {
                room_number[x][y] = 33 + (y + 1) / 3;
            }
        }
    }
    rep(r, R) {
        if (r % 11 == 10) {
            room_place[r][1] = 29;
            continue;
        }
        room_place[r][1] = 3 * (r % 11);
    }
    rep(r, R) {
        if (r % 11 == 0) {
            room_place[r][0] = 0;
            continue;
        } else if (r % 11 == 10) {
            room_place[r][0] = 29;
            continue;
        }
        room_place[r][0] = 3 * (r % 11) - 1;
    }
    rep(r, R) {
        auto assign_walls = [&](int x, int y) {
            if (x < 0 || 29 < x || y <= 0 || 29 <= y) return;
            room_walls[r].emplace_back(x, y);
        };
        int rr = r % 11;
        if (r < 11) {
            for (int x = 0; x <= 4; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(5, 3 * rr - 1);
            assign_walls(5, 3 * rr + 2);
        } else if (r < 22) {
            assign_walls(7, 3 * rr - 3);
            assign_walls(7, 3 * rr);
            for (int x = 8; x <= 12; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(13, 3 * rr - 1);
            assign_walls(13, 3 * rr + 2);
        } else if (r < 33) {
            assign_walls(15, 3 * rr - 3);
            assign_walls(15, 3 * rr);
            for (int x = 16; x <= 21; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
            assign_walls(22, 3 * rr - 1);
            assign_walls(22, 3 * rr + 2);
        } else {
            assign_walls(24, 3 * rr - 3);
            assign_walls(24, 3 * rr);
            for (int x = 25; x <= 29; x++) {
                assign_walls(x, 3 * rr - 2);
                assign_walls(x, 3 * rr + 1);
            }
        }
    }
    moves_.resize(M);
    if (M == 5) {
        starts.emplace_back(13, 1);
        moves_[0] +=
            "rUUdUdUdUdUdUUrUUdUdUduRlRRuDuDuDuDurDDlDDuDuDuDuDurDDlDDuDuDuDuDu"
            "DurDDlDDuDuDudLrLLdUdUdUdUdUUrUUdUdUdUdUdUdU";

        starts.emplace_back(24, 7);
        moves_[1] +=
            "lDDuDuDudRlRRdUdUdUdUdlUUrUUdUdUdUdUdUdlUUrUUdUdUdUdUdlUUrUUdUdUdu"
            "LrLLuDuDuDuDurDDlDDuDuDuDuDurDDlDDuDuDuDuDuDurD";

        starts.emplace_back(5, 13);
        moves_[2] +=
            "rUUdUdUduRlRRuDuDuDuDurDDlDDuDuDuDuDurDDlDDuDuDuDuDuDurDDlDDuDuDud"
            "LrLLdUdUdUdUdlUUrUUdUdUdUdUdUdlUUrUUdUdUdUdUdlU";

        starts.emplace_back(24, 19);
        moves_[3] +=
            "lDDuDuDudRlRRdUdUdUdUdlUUrUUdUdUdUdUdUdlUUrUUdUdUdUdUdlUUrUUdUdUdu"
            "LrLLuDuDuDuDurDDlDDuDuDuDuDurDDlDDuDuDuDuDuDurD";

        starts.emplace_back(13, 25);
        moves_[4] +=
            "rUUdUdUdUdUdlUUrUUdUdUduRlRRuDuDuDuDuDDlDDuDuDuDuDuDDlDDuDuDuDuDuD"
            "uDDlDDuDuDudLrLLdUdUdUdUdlUUrUUdUdUdUdUdUdlU";
    } else if (M == 6) {
        starts.emplace_back(4, 2);
        moves_[0] +=
            "dlUlUlUlUlRRDuDuDuDuDurDDlDDuDuDuDuDuRRlRrUUdUdUdUdUdlUUrUUdUdUduR"
            "lRRuDuDuDuDurDDlDDuDuDuDuDurD";

        starts.emplace_back(13, 13);
        moves_[1] +=
            "rUUdUdUdUdUdlUUrUUdUdUduRlRRuDuDuDuDurDDlDDuDuDuDuDuRRlRrUUdUdUdUd"
            "UdlUUrUUdUdUduRlRRuDuDuDuDurD";

        starts.emplace_back(7, 22);
        moves_[2] +=
            "lDDuDuDuDuDuRRlRrUUdUdUdUdUdlUUrUUdUdUdUdRlRrDrDrDrDrDDDDurDrDrDrD"
            "rDDDDurDrDrDrDrDrDD";

        starts.emplace_back(25, 27);
        moves_[3] +=
            "urDrDrDrDrLLUdUdUdUdUdlUUrUUdUdUdUdUdUdLLrLlDDuDuDuDuDuDurDDlDDuDu"
            "DudLrLLdUdUdUdUdlUUrUUdUdUdUdUdUdlU";

        starts.emplace_back(15, 16);
        moves_[4] +=
            "lDDuDuDuDuDuDurDDlDDuDuDudLrLLdUdUdUdUdlUUrUUdUdUdUdUdUdLLrLlDDuDu"
            "DuDuDuDurDDlDDuDuDudLrLLdUdUdUdUdlU";

        starts.emplace_back(22, 7);
        moves_[5] +=
            "rUUdUdUdUdUdUdLLrLlDDuDuDuDuDuDurDDlDDuDuDuDuLrLlUlUlUlUlUUUUdlUlU"
            "lUlUlUlUUUUdlUlUlUlUlUU";
    } else if (M == 7) {
        starts.emplace_back(16, 2);
        moves_[0] +=
            "lDlDlDlDlDlDRlRrUUdUdUdUdUdUdlUUrUUdUdUdUdUdlUUrUUdUdUdUdLrLlDlDlD"
            "lDlDDuDDlDlDlDlDlDDu";

        starts.emplace_back(25, 2);
        moves_[1] +=
            "lDlDlDlDlRRUdUdUdUdUdlRRRlDDuDuDudRlRRdUdUdUdUdlUUrUUdUdUdUdUdUdLL"
            "rLlDDuDuDuDuDuDurD";

        starts.emplace_back(5, 16);
        moves_[2] +=
            "rUUdUdUduLrLLuDuDuDuDurLLLrUUdUdUduLrLLuDuDuDuDurDDlDDuDuDuDuDuRRl"
            "RrUUdUdUdUdUdlU";

        starts.emplace_back(25, 15);
        moves_[3] +=
            "urDrDrDrDrLLUdUdUdUdUdlUUrUUdUdUdUdUdUdlUUrUUdUdUdUdUdlRRRlDDuDuDu"
            "DuDurD";

        starts.emplace_back(15, 22);
        moves_[4] +=
            "lDDuDuDuDuDuDurDDlDDuDuDudLrLLdUdUdUdUdlUUrUUdUdUdUdUdUdLLrLlDDuDu"
            "DuDuDuDurD";

        starts.emplace_back(4, 27);
        moves_[5] +=
            "rUrUrUrUrLLDuDuDuDuDurLLLrUUdUdUduLrLLuDuDuDuDurDDlDDuDuDuDuDuRRlR"
            "rUUdUdUdUdUdlU";

        starts.emplace_back(16, 27);
        moves_[6] +=
            "urDrDrDrDrDrDDDDurDrDrDrDrLLUdUdUdUdUdlUUrUUdUdUdUdUdUdlUUrUUdUdUd"
            "UdUdlRRRlDDuDuDuDuDuD";
    } else if (M == 8) {
        starts.emplace_back(22, 1);
        moves_[0] +=
            "rUUdUdUdUdUdUdUUrUUdUdUdUdUdUUrUUdUdUduRlRRuDuDuDuDurDDlDDuDuDuDuD"
            "urD";

        starts.emplace_back(25, 2);
        moves_[1] +=
            "lDlDlDlDlRRUdUdUdUdUdlUUrUUdUdUdUdUdUdlRRRlDDuDuDuDuDuDuRRlRrUUdUd"
            "UdUdUdUdlU";

        starts.emplace_back(13, 7);
        moves_[2] +=
            "rUUdUdUdUdUdlUUrUUdUdUduRlRRuDuDuDuDurDDlDDuDuDuDuDuRRlRrUUdUdUdUd"
            "UdlU";

        starts.emplace_back(15, 13);
        moves_[3] +=
            "lDDuDuDuDuDuDuRRlRrUUdUdUdUdUdUdlUUrUUdUdUdUdUdlUUrUUdUdUduLrLLuDu"
            "DuDuDurD";

        starts.emplace_back(28, 19);
        moves_[4] +=
            "dUdUdUdUdLLrLlDDuDuDudLrLLdUdUdUdUdLLrLlDDuDuDudLrLLdUdUdUdUdlU";

        starts.emplace_back(28, 22);
        moves_[5] +=
            "dUdUdUdUdlUUrUUdUdUdUdUdUdlUUrUUdUdUdUdUdLLrLlDDuDuDuDuDurDDlDDuDu"
            "DuDuDuDurD";

        starts.emplace_back(11, 28);
        moves_[6] +=
            "dUdUdUdUdlUUUUdUdUduLrLLuDuDuDuDurLLLrUUdUdUduLrLLuDuDuDuDurD";

        starts.emplace_back(7, 25);
        moves_[7] +=
            "lDDuDuDuDuDurDDlDDuDuDuDuDuDurDDlDDuDuDudRlRRdUdUdUdUdlUUUUdUdUdUd"
            "UdUdlU";
    } else if (M == 9) {
        starts.emplace_back(22, 1);
        moves_[0] +=
            "rUUdUdUdUdUdUdUUrUUdUdUdUdUdUUrUUdUdUduRlRRuDuDuDuDurDDlDDuDuDuDuD"
            "urD";

        starts.emplace_back(25, 2);
        moves_[1] +=
            "lDlDlDlDlRRUdUdUdUdUdlUUrUUdUdUdUdUdUdlRRRlDDuDuDuDuDuDuRRlRrUUdUd"
            "UdUdUdUdlU";

        starts.emplace_back(13, 7);
        moves_[2] +=
            "rUUdUdUdUdUdlUUrUUdUdUduRlRRuDuDuDuDurDDlDDuDuDuDuDuRRlRrUUdUdUdUd"
            "UdlU";

        starts.emplace_back(15, 13);
        moves_[3] +=
            "lDDuDuDuDuDuDuRRlRrUUdUdUdUdUdUdlUUrUUdUdUdUdUdlUUrUUdUdUduLrLLuDu"
            "DuDuDurD";

        starts.emplace_back(28, 19);
        moves_[4] +=
            "dUdUdUdUdLLrLlDDuDuDudLrLLdUdUdUdUdLLrLlDDuDuDudLrLLdUdUdUdUdlU";

        starts.emplace_back(28, 22);
        moves_[5] +=
            "dUdUdUdUdlUUrUUdUdUdUdUdUdlUUrUUdUdUdUdUdLLrLlDDuDuDuDuDurDDlDDuDu"
            "DuDuDuDurD";

        starts.emplace_back(11, 28);
        moves_[6] +=
            "dUdUdUdUdlUUUUdUdUduLrLLuDuDuDuDurLLLrUUdUdUduLrLLuDuDuDuDurD";

        starts.emplace_back(7, 25);
        moves_[7] +=
            "lDDuDuDuDuDurDDlDDuDuDuDuDuDurDDlDDuDuDudRlRRdUdUdUdUdlUUUUdUdUdUd"
            "UdUdlU";

        starts.emplace_back(23, 27);
    } else if (M == 10) {
        starts.emplace_back(22, 4);
        moves_[0] += "rUUdUdUdUdUdUdlUUrUUdUdUdUdUdlUUrUUdUdUduLrLLuDuDuDuDurD";

        starts.emplace_back(9, 1);
        moves_[1] += "uDuDuDuDurDDDDuDuDuDuDuDurDDDDuDuDudRlRRdUdUdUdUdlU";

        starts.emplace_back(13, 7);
        moves_[2] += "rUUdUdUdUdUdlUUrUUdUdUduRlRRuDuDuDuDurDDlDDuDuDuDuDurD";

        starts.emplace_back(15, 10);
        moves_[3] +=
            "lDDuDuDuDuDuDurDDlDDuDuDudLrLLdUdUdUdUdlUUrUUdUdUdUdUdUdlU";

        starts.emplace_back(22, 16);
        moves_[4] += "rUUdUdUdUdUdUdlUUrUUdUdUdUdUdlUUrUUdUdUduLrLLuDuDuDuDurD";

        starts.emplace_back(7, 13);
        moves_[5] += "lDDuDuDuDuDurDDlDDuDuDuDuDuDurDDlDDuDuDudRlRRdUdUdUdUdlU";

        starts.emplace_back(13, 19);
        moves_[6] += "rUUdUdUdUdUdlUUrUUdUdUduRlRRuDuDuDuDurDDlDDuDuDuDuDurD";

        starts.emplace_back(15, 22);
        moves_[7] +=
            "lDDuDuDuDuDuDurDDlDDuDuDudLrLLdUdUdUdUdlUUrUUdUdUdUdUdUdlU";

        starts.emplace_back(20, 28);
        moves_[8] += "dUdUdUdUdUdlUUUUdUdUdUdUdlUUUUdUdUduLrLLuDuDuDuDurD";

        starts.emplace_back(7, 25);
        moves_[9] += "lDDuDuDuDuDurDDlDDuDuDuDuDuDurDDlDDuDuDudRlRRdUdUdUdUdlU";
    }
    rep(m, M) {
        if (!moves_[m].empty()) {
            reverse(ALL(moves_[m]));
        }
    }
    vector<vector<int>> dist(M, vector<int>(M));
    rep(i, M) {
        rep(j, M) {
            dist[i][j] = abs(game.humans[i].x - starts[j].first) +
                         abs(game.humans[i].y - starts[j].second);
        }
    }
    vector<int> v(M);
    iota(ALL(v), 0);
    int mn = INT_MAX;
    do {
        int sum = 0;
        rep(i, M) {
            sum = max(dist[i][v[i]], sum);
        }
        if (sum < mn) {
            sidx = v;
            mn = sum;
        }
    } while (next_permutation(ALL(v)));
}

void solve(Game &game) {
    auto moves = moves_;
    vector<int> phase(M);  // 0 : move, 1 : wall, 2 : catch
    set<int> FR;           // free rooms
    vector<bool> built(R);
    rep(d, D) {
        vector<char> output(M, '.');
        rep(r, R) {
            if (built[r]) continue;
            bool flag = true;
            for (auto p : room_walls[r]) {
                if (!game.board.is_wall(p.first, p.second)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                FR.emplace(r);
                built[r] = true;
            }
        }
        vector<double> pet_score(R);
        rep(n, N) {
            int x = game.pets[n].x, y = game.pets[n].y,
                type = game.pets[n].type;
            if (room_number[x][y] == -1) {
                auto dist = game.board.dist(x, y);
                vector<int> mn(R, INT_MAX);
                rep(xx, K) {
                    rep(yy, K) {
                        if (room_number[xx][yy] == -1 || dist[xx][yy] == -1) {
                            continue;
                        }
                        mn[room_number[xx][yy]] =
                            min(mn[room_number[xx][yy]], dist[xx][yy]);
                    }
                }
                for (auto r : FR) {
                    if (mn[r] == INT_MAX) {
                        continue;
                    }
                    pet_score[r] += 1.0 / ((mn[r] + 1) * (mn[r] + 1));
                }
                continue;
            }
            pet_score[room_number[x][y]] += pet_type_score[2][type];
        }
        set<int> FH;  // free humans
        rep(m, M) {
            if (phase[m] == 2) {
                FH.emplace(m);
            }
        }
        while (!FH.empty()) {
            vector<pair<double, int>> v;  // (score, room)
            for (auto r : FR) {
                if (r < 11) {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 6) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][1]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                } else if (r < 22) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 6) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 14) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 33) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 14) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 23) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 23) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][0]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                }
            }
            if (v.empty()) break;
            sort(ALL(v));
            reverse(ALL(v));
            int r;
            double score;
            tie(score, r) = v.front();
            if (score <= 0) break;
            pet_score[r] = 0;
            if (r < 11) {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 6) {
                        int dist = abs(game.humans[m].y - room_place[r][1]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('u', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][1]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            } else if (r < 22) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 6) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 14) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 33) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 14) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 23) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 23) {
                        int dist = abs(game.humans[m].y - room_place[r][0]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('d', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('d', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][0]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            }
        }
        rep(m, M) {
            int idx = sidx[m];
            if (phase[m] == 0) {
                output[m] = game.humans[m].action(
                    game.board.approach(game.humans[m].x, game.humans[m].y,
                                        starts[idx].first, starts[idx].second),
                    game.board);
                if (game.humans[m].x == starts[idx].first &&
                    game.humans[m].y == starts[idx].second) {
                    phase[m] = 1 + moves[idx].empty();
                }
            } else if (phase[m] == 1) {
                if (game.humans[m].can(moves[idx].back(), game.board)) {
                    output[m] =
                        game.humans[m].action(moves[idx].back(), game.board);
                    moves[idx].pop_back();
                } else {
                    output[m] = game.humans[m].action('.', game.board);
                }
                if (moves[idx].empty()) phase[m] = 2;
            }
        }
        rep(m, M) {
            cout << output[m];
        }
        cout << endl;
        game.board.clear_human();
        rep(m, M) {
            game.board.set_human(game.humans[m].x, game.humans[m].y);
        }
        game.board.clear_pet();
        rep(n, N) {
            string s;
            cin >> s;
            game.pets[n].action(s, game.board);
        }
    }
}

double simulate(Game &game) {
    auto moves = moves_;
    vector<int> phase(M);  // 0 : move, 1 : wall, 2 : catch
    set<int> FR;           // free rooms
    vector<bool> built(R);
    rep(d, D) {
        vector<char> output(M, '.');
        rep(r, R) {
            if (built[r]) continue;
            bool flag = true;
            for (auto p : room_walls[r]) {
                if (!game.board.is_wall(p.first, p.second)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                FR.emplace(r);
                built[r] = true;
            }
        }
        vector<double> pet_score(R);
        rep(n, N) {
            int x = game.pets[n].x, y = game.pets[n].y,
                type = game.pets[n].type;
            if (room_number[x][y] == -1) {
                auto dist = game.board.dist(x, y);
                vector<int> mn(R, INT_MAX);
                rep(xx, K) {
                    rep(yy, K) {
                        if (room_number[xx][yy] == -1 || dist[xx][yy] == -1) {
                            continue;
                        }
                        mn[room_number[xx][yy]] =
                            min(mn[room_number[xx][yy]], dist[xx][yy]);
                    }
                }
                for (auto r : FR) {
                    if (mn[r] == INT_MAX) {
                        continue;
                    }
                    pet_score[r] += 1.0 / ((mn[r] + 1) * (mn[r] + 1));
                }
                continue;
            }
            pet_score[room_number[x][y]] += pet_type_score[2][type];
        }
        set<int> FH;  // free humans
        rep(m, M) {
            if (phase[m] == 2) {
                FH.emplace(m);
            }
        }
        while (!FH.empty()) {
            vector<pair<double, int>> v;  // (score, room)
            for (auto r : FR) {
                if (r < 11) {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 6) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][1]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                } else if (r < 22) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 6) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 14) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else if (r < 33) {
                    int mn_up = INT_MAX, mn_down = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 14) {
                            mn_up = min(mn_up, abs(game.humans[m].y -
                                                   room_place[r][0]));
                        } else if (game.humans[m].x == 23) {
                            mn_down = min(mn_down, abs(game.humans[m].y -
                                                       room_place[r][1]));
                        }
                    }
                    if (mn_up == INT_MAX || mn_down == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (max(mn_up, mn_down) + 1);
                    v.emplace_back(score, r);
                } else {
                    int mn = INT_MAX;
                    for (auto m : FH) {
                        if (game.humans[m].x == 23) {
                            mn = min(mn,
                                     abs(game.humans[m].y - room_place[r][0]));
                        }
                    }
                    if (mn == INT_MAX) {
                        continue;
                    }
                    double score = pet_score[r] / (mn + 1);
                    v.emplace_back(score, r);
                }
            }
            if (v.empty()) break;
            sort(ALL(v));
            reverse(ALL(v));
            int r;
            double score;
            tie(score, r) = v.front();
            if (score <= 0) break;
            pet_score[r] = 0;
            if (r < 11) {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 6) {
                        int dist = abs(game.humans[m].y - room_place[r][1]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('u', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][1]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            } else if (r < 22) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 6) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 14) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else if (r < 33) {
                int mn_up = INT_MAX, mn_down = INT_MAX;
                int mn_up_human = -1, mn_down_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 14) {
                        if (abs(game.humans[m].y - room_place[r][0]) < mn_up) {
                            mn_up = abs(game.humans[m].y - room_place[r][0]);
                            mn_up_human = m;
                        }
                    } else if (game.humans[m].x == 23) {
                        if (abs(game.humans[m].y - room_place[r][1]) <
                            mn_down) {
                            mn_down = abs(game.humans[m].y - room_place[r][1]);
                            mn_down_human = m;
                        }
                    }
                }
                FH.erase(mn_up_human);
                FH.erase(mn_down_human);
                if (mn_up == 0 && mn_down == 0 && t <= score) {
                    if (game.humans[mn_up_human].can('d', game.board) &&
                        game.humans[mn_down_human].can('u', game.board)) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('d', game.board);
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('u', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_up_human].y < room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('R', game.board);
                    } else if (game.humans[mn_up_human].y > room_place[r][0]) {
                        output[mn_up_human] =
                            game.humans[mn_up_human].action('L', game.board);
                    }
                    if (game.humans[mn_down_human].y < room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('R', game.board);
                    } else if (game.humans[mn_down_human].y >
                               room_place[r][1]) {
                        output[mn_down_human] =
                            game.humans[mn_down_human].action('L', game.board);
                    }
                }
            } else {
                int mn = INT_MAX;
                int mn_human = -1;
                for (auto m : FH) {
                    if (game.humans[m].x == 23) {
                        int dist = abs(game.humans[m].y - room_place[r][0]);
                        if (dist < mn) {
                            mn = dist;
                            mn_human = m;
                        }
                    }
                }
                FH.erase(mn_human);
                if (mn == 0) {
                    if (game.humans[mn_human].can('d', game.board) &&
                        t <= score) {
                        output[mn_human] =
                            game.humans[mn_human].action('d', game.board);
                        FR.erase(r);
                    }
                } else {
                    if (game.humans[mn_human].y < room_place[r][0]) {
                        output[mn_human] =
                            game.humans[mn_human].action('R', game.board);
                    } else {
                        output[mn_human] =
                            game.humans[mn_human].action('L', game.board);
                    }
                }
            }
        }
        rep(m, M) {
            int idx = sidx[m];
            if (phase[m] == 0) {
                output[m] = game.humans[m].action(
                    game.board.approach(game.humans[m].x, game.humans[m].y,
                                        starts[idx].first, starts[idx].second),
                    game.board);
                if (game.humans[m].x == starts[idx].first &&
                    game.humans[m].y == starts[idx].second) {
                    phase[m] = 1 + moves[idx].empty();
                }
            } else if (phase[m] == 1) {
                if (game.humans[m].can(moves[idx].back(), game.board)) {
                    output[m] =
                        game.humans[m].action(moves[idx].back(), game.board);
                    moves[idx].pop_back();
                } else {
                    output[m] = game.humans[m].action('.', game.board);
                }
                if (moves[idx].empty()) phase[m] = 2;
            }
        }
        game.board.clear_human();
        rep(m, M) {
            game.board.set_human(game.humans[m].x, game.humans[m].y);
        }
        game.board.clear_pet();
        rep(n, N) {
            game.pets[n].simulate(game.board, game.humans);
        }
    }
    return game.calc();
}
}  // namespace LargeRooms

signed main() {
    Timer tmr;
    double start_time = tmr.getTime();
    Game game;
    game.input();
    SmallRooms::init(game);
    MediumRooms::init(game);
    LargeRooms::init(game);
    vector<pair<double, int>> sum(3);
    rep(i, 3) {
        sum[i].second = i;
    }
    while (true) {
        Game game_small = game, game_medium = game, game_large = game;
        sum[0].first += SmallRooms::simulate(game_small);
        sum[1].first += MediumRooms::simulate(game_medium);
        sum[2].first += LargeRooms::simulate(game_large);
        double now_time = tmr.getTime();
        if (now_time - start_time > TIME_LIMIT) {
            break;
        }
    }
    sort(ALL(sum));
    reverse(ALL(sum));
    if (sum.front().second == 0) {
        SmallRooms::solve(game);
    } else if (sum.front().second == 1) {
        MediumRooms::solve(game);
    } else {
        LargeRooms::solve(game);
    }
}