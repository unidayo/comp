//
// ver 1.25
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
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define rep(i, n) for (int i = 0; i < (n); i++)
#define REP(i, a, n) for (int i = (a); i <= (n); i++)
#define all(a) (a).begin(), (a).end()
#define mp(a, b) make_pair((a), (b))

using namespace std;

#ifdef ONLINE_JUDGE
const int TIME_LIMIT = 2950;
#else
const int TIME_LIMIT = 2900;
#endif

const int INF = 12345678;

const int DX[4] = { 1, 0, -1, 0 };
const int DY[4] = { 0, -1, 0, 1 };

const int RIGHT = 0;
const int UP = 1;
const int LEFT = 2;
const int DOWN = 3;

const int MAX_N = 48;
const int MAX_K = 5;

mt19937 mt19937_engine(0);

unsigned int rand_xor() {
    static unsigned long x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    unsigned long t = (x ^ (x << 11u));
    x = y; y = z; z = w;
    return (w = (w ^ (w >> 19u)) ^ (t ^ (t >> 8u)));
}

class Timer {
private:
    static const int LAZY_MASK = 0xFF;

    chrono::system_clock::time_point start;
    int call_cnt = 0;
    int prev_time;
    bool is_lazy;

public:
    Timer(bool is_lazy = false) : is_lazy(is_lazy) {
        start = chrono::system_clock::now();
    }

    int get_ms() {
        if (!is_lazy || (call_cnt & LAZY_MASK) == 0) {
            auto end = chrono::system_clock::now();
            prev_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        }
        call_cnt++;
        return prev_time;
    }
};

int N, K;
double density;
double sqrt_memo[MAX_N+1];

class Point {
public:
    int x, y;
    Point() : x(-1), y(-1) {}
    Point(int x, int y) : x(x), y(y) {}

    void move(int direction) {
        x += DX[direction];
        y += DY[direction];
    }

    bool is_valid() {
        return x >= 0 && x < N && y >= 0 && y < N;
    }

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

int distance(const Point& p1, const Point& p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

int get_direction(const Point& p1, const Point& p2) {
    assert(distance(p1, p2) == 1);
    if (p1.x + 1 == p2.x) return RIGHT;
    if (p1.x - 1 == p2.x) return LEFT;
    if (p1.y + 1 == p2.y) return DOWN;
    if (p1.y - 1 == p2.y) return UP;
    assert(false);
}

class PointQueue {
protected:
    static Point pts[2*MAX_N*MAX_N];
    int pt1 = 0, pt2 = 0;

public:
    void push(const Point& p) {
        pts[pt2].x = p.x;
        pts[pt2].y = p.y;
        pt2++;
    }

    Point& front() {
        return pts[pt1];
    }

    void pop() {
        pt1++;
    }

    bool empty() {
        return pt1 == pt2;
    }
};
Point PointQueue::pts[2*MAX_N*MAX_N];

class Cluster;

class Server : public Point {
public:
    int type;
    shared_ptr<Server> next_server[4];
    shared_ptr<Cluster> cluster;
    bool fixed = false;

    Server(int x, int y, int type) : Point(x, y), type(type) {}

    int degree() {
        int ret = 0;
        rep(dir, 4) ret += !!next_server[dir];
        return ret;
    }

    bool can_move_to(int direction) {
        if (direction == RIGHT || direction == LEFT) {
            return !next_server[UP] && !next_server[DOWN];
        } else if (direction == UP || direction == DOWN) {
            return !next_server[RIGHT] && !next_server[LEFT];
        }
        assert(false);
    }
};

class MoveAction {
public:
    Point from;
    Point to;
    int direction;

    MoveAction() {}
    MoveAction(const Point& from, int direction) : from(from), direction(direction) {
        to = from;
        to.move(direction);
    }
};

class MoveActionVector {
public:
    array<MoveAction, MAX_N*MAX_N> moves;
    int size = 0;

    void push_back(const Point& from, int direction) {
        assert(size < moves.size());
        auto& move = moves[size];
        move.from = from;
        move.to = from;
        move.to.move(direction);
        move.direction = direction;
        size++;
    }

    void copy(const MoveActionVector& base) {
        rep(i, base.size) moves[i] = base.moves[i];
        size = base.size;
    }

    bool empty() {
        return size == 0;
    }

    vector<MoveAction> get_vec() {
        return vector<MoveAction>(moves.begin(), moves.begin() + size);
    }

    void clear() {
        size = 0;
    }
};

class Answer {
public:
    int score = 0;
    vector<MoveAction> moves;
    vector<pair<shared_ptr<Server>, shared_ptr<Server>>> connects;

    int get_remaining_action_num() {
        return 100 * K - moves.size() - connects.size();
    }

    void print(ostream& out) {
        out << moves.size() << endl;
        for (auto& action: moves) {
            out << action.from.y << ' ' << action.from.x << ' ' << action.to.y << ' ' << action.to.x << endl;
        }

        out << connects.size() << endl;
        for (auto& pr: connects) {
            out << pr.first->y << ' ' << pr.first->x << ' ' << pr.second->y << ' ' << pr.second->x << endl;
        }
    }
};

class Cluster {
protected:
    int process(shared_ptr<Server>& server, int prev_dir, Answer& answer) {
        if (answer.get_remaining_action_num() == 0) return 0;
        int ret = 0;
        rep(dir, 4) {
            if (dir == prev_dir) continue;
            if (!server->next_server[dir]) continue;
            answer.connects.push_back(mp(server, server->next_server[dir]));
            ret++;
            ret += process(server->next_server[dir], (dir + 2) & 3, answer);
            if (answer.get_remaining_action_num() == 0) break;
        }
        return ret;
    }

public:
    int server_type;
    vector<shared_ptr<Server>> servers;
    Point min_pt, max_pt;

    Cluster(int server_type) : server_type(server_type) {}

    void connect(Answer& answer) {
        int connect_num = process(servers[0], -1, answer);
        answer.score += (connect_num+1) * connect_num / 2;
        for (auto& server: servers) server->fixed = true;
    }

    void update_min_max_pts() {
        min_pt.x = INF;
        min_pt.y = INF;
        max_pt.x = -1;
        max_pt.y = -1;

        for (auto& server: servers) {
            min_pt.x = min(min_pt.x, server->x);
            min_pt.y = min(min_pt.y, server->y);
            max_pt.x = max(max_pt.x, server->x);
            max_pt.y = max(max_pt.y, server->y);
        }
    }
};

int distance(const Cluster& c1, const Cluster& c2) {
    int dx = max(0, max(c1.min_pt.x, c2.min_pt.x) - min(c1.max_pt.x, c2.max_pt.x));
    int dy = max(0, max(c1.min_pt.y, c2.min_pt.y) - min(c1.max_pt.y, c2.max_pt.y));
    return max(dx,dy);
}

class Solver {
protected:
    shared_ptr<Server> server_grids[MAX_N][MAX_N];
    shared_ptr<Cluster> cabled_cluster[MAX_N][MAX_N];

public:
    Solver(const vector<string>& grids) {
        rep(y, N) {
            rep(x, N) {
                if (grids[y][x] == '0') continue;
                server_grids[y][x] = make_shared<Server>(x, y, grids[y][x] - '0');
            }
        }
    }

    virtual Answer solve() = 0;
};

class Proposition {
public:
    enum class Type {
        MOVE_TO_LASER,
        MOVE_OUT_OBSTACLE,
        MOVE_ONTO_SUB_CABLE,
        MOVE_ONTO_MAIN_CABLE,
        MOVE_SOLO_CLUSTER_TO_LASER,
        MOVE_SOLO_CLUSTER_ONTO_CABLE,
        SLIDE_SOLO_CLUSTER,
    } type;
    double cost;
    shared_ptr<Cluster> target_cluster;
    vector<MoveAction> moves;

    Proposition(Type type, shared_ptr<Cluster>& target_cluster, const vector<MoveAction>& moves)
        : type(type), target_cluster(target_cluster), moves(moves) {
        cost = (double) moves.size() / sqrt_memo[target_cluster->servers.size()];
        cost += 0.00001 * (rand_xor() & 127);
    }
};

class SingleTypeSolver : public Solver {
protected:
    Answer answer;
    Timer& timer;

    int main_server_type;
    vector<string> original_grids;

    void search_next_servers(shared_ptr<Server>& server, shared_ptr<Cluster>& cluster, int target_type) {
        server->cluster = cluster;
        cluster->servers.push_back(server);

        rep(dir, 4) {
            if (server->next_server[dir]) continue;
            Point now = *server;
            now.move(dir);
            while (now.is_valid() && !cabled_cluster[now.y][now.x]) {
                auto next_server = server_grids[now.y][now.x];
                if (next_server) {
                    if (next_server->type != target_type) break;
                    if (next_server->cluster) break;    // ループを検知

                    int prev_dir = (dir + 2) & 3;
                    server->next_server[dir] = next_server;
                    next_server->next_server[(dir + 2) & 3] = server;

                    auto rnow = now;
                    rnow.move(prev_dir);
                    while (rnow != *server) {
                        cabled_cluster[rnow.y][rnow.x] = cluster;
                        rnow.move(prev_dir);
                    }

                    search_next_servers(next_server, cluster, target_type);
                    break;
                }
                now.move(dir);
            }
        }
    }

    void construct_clusters(vector<shared_ptr<Cluster>>& clusters, int target_type) {
        rep(y, N) {
            rep(x, N) {
                if (!server_grids[y][x]
                    || server_grids[y][x]->type != target_type
                    || server_grids[y][x]->cluster) continue;
                auto server = server_grids[y][x];
                auto cluster = make_shared<Cluster>(target_type);

                search_next_servers(server, cluster, target_type);

                clusters.push_back(cluster);
            }
        }
    }

    void add_move_to_laser_proposition(shared_ptr<Cluster>& fixed_cluster,
                                       shared_ptr<Cluster>& moving_cluster,
                                       shared_ptr<Cluster>& sub_cluster,
                                       vector<Proposition>& propositions) {
        // fixed_cluster からレーザーを放つ
        bool laser[MAX_N][MAX_N];
        memset(laser, false, sizeof(laser));
        for (auto& server: fixed_cluster->servers) {
            rep(dir, 4) {
                if (server->next_server[dir]) continue;
                Point now = *server;
                now.move(dir);
                while (now.is_valid()) {
                    laser[now.y][now.x] = true;
                    if (cabled_cluster[now.y][now.x] || server_grids[now.y][now.x]) break;
                    now.move(dir);
                }
            }
        }

        // moving_cluster の各 server を 4 方向に移動させて、最短手数でレーザーに接触するものを探す
        shared_ptr<Server> best_server;
        int best_move_dir;
        int min_move_count = INF;
        for (auto& server: moving_cluster->servers) {
            rep(dir, 4) {
                if (!server->can_move_to(dir)) continue;
                Point now = *server;
                bool hit_laser = false;
                int move_count = 1;
                now.move(dir);
                while (now.is_valid()
                       && (server->next_server[dir] || !cabled_cluster[now.y][now.x])
                       && !server_grids[now.y][now.x]) {
                    if (laser[now.y][now.x]) {
                        hit_laser = true;
                        break;
                    }
                    move_count++;
                    now.move(dir);
                }
                if (hit_laser && move_count < min_move_count) {
                    min_move_count = move_count;
                    best_server = server;
                    best_move_dir = dir;
                }
            }
        }

        if (best_server) {
            Point now = *best_server;
            vector<MoveAction> moves;
            rep(i, min_move_count) {
                moves.push_back(MoveAction(now, best_move_dir));
                now.move(best_move_dir);
            }
            propositions.push_back(Proposition(Proposition::Type::MOVE_TO_LASER, sub_cluster, moves));
        }
    }

    void _supply_move_out_obstacle(int dir2,
                                   const Point& now,
                                   int used_space[MAX_N][MAX_N],
                                   int used_space_id,
                                   shared_ptr<Server>& server,
                                   shared_ptr<Cluster>& main_cluster,
                                   int passed[MAX_N][MAX_N],
                                   shared_ptr<Cluster>& best_cluster,
                                   MoveActionVector& moves,
                                   MoveActionVector& min_moves,
                                   int& tie_count) {
        int rdir = (dir2 + 2) & 3;
        Point now2 = now;
        now2.move(dir2);
        while (now2.is_valid()
               && !server_grids[now2.y][now2.x]
               && !cabled_cluster[now2.y][now2.x]
               && used_space[now2.y][now2.x] != used_space_id) now2.move(dir2);
        int dist = distance(now, now2);
        if (dist > 4) return;
        if (now2.is_valid()
            && server_grids[now2.y][now2.x]
            && server_grids[now2.y][now2.x]->type == server->type
            && server_grids[now2.y][now2.x]->cluster != main_cluster
            && server_grids[now2.y][now2.x]->can_move_to(rdir)
            && passed[now2.y][now2.x] != used_space_id) {
            if (!best_cluster || moves.size + dist < min_moves.size) {
                best_cluster = server_grids[now2.y][now2.x]->cluster;
                min_moves.copy(moves);
                while (now2 != now) {
                    min_moves.push_back(now2, rdir);
                    now2.move(rdir);
                }
                tie_count = 1;
            } else if (moves.size + dist == min_moves.size) {
                tie_count++;
                if (rand_xor() % tie_count == 0) {
                    best_cluster = server_grids[now2.y][now2.x]->cluster;
                    min_moves.copy(moves);
                    while (now2 != now) {
                        min_moves.push_back(now2, rdir);
                        now2.move(rdir);
                    }
                }
            }
        }
    }

    void add_move_out_obstacle_proposition(shared_ptr<Cluster>& main_cluster,
                                           vector<Proposition>& propositions) {
        shared_ptr<Cluster> best_cluster;
        MoveActionVector min_moves;
        int used_space[MAX_N][MAX_N];
        int passed[MAX_N][MAX_N];
        int prev_dir[MAX_N][MAX_N];
        memset(prev_dir, 0, sizeof(prev_dir));
        memset(passed, 0, sizeof(used_space));
        memset(used_space, 0, sizeof(used_space));
        int prev_dir_prefix = 0;
        int used_space_id = 0;
        int tie_count = 0;
        MoveActionVector moves, first_moves;
        for (auto& server: main_cluster->servers) {
            rep(sdir, 5) {
                REP(pre_move_len, 1, 2) {
                    first_moves.clear();
                    if (sdir < 4) {
                        Point next = *server;
                        bool ok = true;
                        rep(i, pre_move_len) {
                            next.move(sdir);
                            if (!next.is_valid()
                                || !server->can_move_to(sdir)
                                || server_grids[next.y][next.x]
                                || !server->next_server[sdir] && cabled_cluster[next.y][next.x]) {
                                ok = false;
                                break;
                            }
                        }
                        if (!ok) break;
                        rep(i, pre_move_len) {
                            first_moves.push_back(*server, sdir);
                            move_server(server, sdir);
                        }
                    }
                    rep(dir, 4) {
                        if (server->next_server[dir]) continue;
                        used_space_id++;
                        moves.copy(first_moves);
                        Point now = *server;
                        now.move(dir);
                        while (now.is_valid()) {
                            if (cabled_cluster[now.y][now.x]) {
                                if (dir == RIGHT || dir == LEFT) {
                                    int upper_move_count = INF;
                                    int lower_move_count = INF;

                                    Point now2 = now;
                                    now2.move(UP);
                                    while (!server_grids[now2.y][now2.x]) now2.move(UP);

                                    auto upper_server = server_grids[now2.y][now2.x];
                                    if (upper_server->can_move_to(DOWN)
                                        && !upper_server->next_server[UP]
                                        && !server_grids[now.y + 1][now.x]) {
                                        upper_move_count = now.y - upper_server->y + 1;
                                    }

                                    now2 = now;
                                    now2.move(DOWN);
                                    while (!server_grids[now2.y][now2.x]) now2.move(DOWN);
                                    auto lower_server = server_grids[now2.y][now2.x];
                                    if (lower_server->can_move_to(UP)
                                        && !lower_server->next_server[DOWN]
                                        && !server_grids[now.y - 1][now.x]) {
                                        lower_move_count = lower_server->y - now.y + 1;
                                    }

                                    if (upper_move_count < lower_move_count) {
                                        now2 = *upper_server;
                                        rep(i, upper_move_count) {
                                            moves.push_back(now2, DOWN);
                                            now2.move(DOWN);
                                        }
                                    } else if (lower_move_count < INF && lower_move_count <= upper_move_count) {
                                        now2 = *lower_server;
                                        rep(i, lower_move_count) {
                                            moves.push_back(now2, UP);
                                            now2.move(UP);
                                        }
                                    } else {
                                        break;
                                    }
                                } else {
                                    int left_move_count = INF;
                                    int right_move_count = INF;

                                    Point now2 = now;
                                    now2.move(LEFT);
                                    while (!server_grids[now2.y][now2.x]) now2.move(LEFT);
                                    auto left_server = server_grids[now2.y][now2.x];
                                    if (left_server->can_move_to(RIGHT)
                                        && !left_server->next_server[LEFT]
                                        && !server_grids[now.y][now.x + 1]) {
                                        left_move_count = now.x - left_server->x + 1;
                                    }

                                    now2 = now;
                                    now2.move(RIGHT);
                                    while (!server_grids[now2.y][now2.x]) now2.move(RIGHT);
                                    auto right_server = server_grids[now2.y][now2.x];
                                    if (right_server->can_move_to(LEFT)
                                        && !right_server->next_server[RIGHT]
                                        && !server_grids[now.y][now.x - 1]) {
                                        right_move_count = right_server->x - now.x + 1;
                                    }

                                    if (left_move_count < right_move_count) {
                                        now2 = *left_server;
                                        rep(i, left_move_count) {
                                            moves.push_back(now2, RIGHT);
                                            now2.move(RIGHT);
                                        }
                                    } else if (right_move_count < INF && right_move_count <= left_move_count) {
                                        now2 = *right_server;
                                        rep(i, right_move_count) {
                                            moves.push_back(now2, LEFT);
                                            now2.move(LEFT);
                                        }
                                    } else {
                                        break;
                                    }
                                }
                            } else {
                                if (server_grids[now.y][now.x]) {
                                    auto hit_server = server_grids[now.y][now.x];
                                    if (hit_server->type == server->type && hit_server->cluster != main_cluster) {
                                        if (!best_cluster || moves.size < min_moves.size) {
                                            min_moves.copy(moves);
                                            best_cluster = hit_server->cluster;
                                            tie_count = 1;
                                        } else if (moves.size == min_moves.size) {
                                            tie_count++;
                                            if (rand_xor() % tie_count == 0) {
                                                min_moves.copy(moves);
                                                best_cluster = hit_server->cluster;
                                            }
                                        }
                                        break;
                                    } else {
                                        if (!server_grids[now.y][now.x]->fixed
                                            && hit_server->type != server->type) {
                                            prev_dir_prefix += 0x10;
                                            Point nearest_space;
                                            PointQueue q;
                                            q.push(now);
                                            prev_dir[now.y][now.x] = prev_dir_prefix + 10;
                                            while (!q.empty()) {
                                                auto now2 = q.front();
                                                q.pop();
                                                if (used_space[now2.y][now2.x] != used_space_id && !server_grids[now2.y][now2.x]) {
                                                    nearest_space = now2;
                                                    break;
                                                }
                                                rep(dir2, 4) {
                                                    auto next = now2;
                                                    next.move(dir2);
                                                    if (!next.is_valid()) continue;

                                                    if ((dir == RIGHT || dir == LEFT) && next.y == now.y) continue;
                                                    if ((dir == UP || dir == DOWN) && next.x == now.x) continue;

                                                    if (prev_dir[next.y][next.x] >= prev_dir_prefix) continue;
                                                    if (cabled_cluster[next.y][next.x]) continue;
                                                    if (server_grids[next.y][next.x]
                                                        && (server_grids[next.y][next.x]->type == server->type
                                                            || server_grids[next.y][next.x]->fixed))
                                                        continue;
                                                    prev_dir[next.y][next.x] = prev_dir_prefix + ((dir2 + 2) & 3);
                                                    q.push(next);
                                                }
                                            }

                                            if (!nearest_space.is_valid()) {
                                                int best_dir = -1;

                                                // 逆方向
                                                auto rnow = now;
                                                int rdir = (dir + 2) & 3;
                                                rnow.move(rdir);
                                                while (rnow != *server) {
                                                    if (dir == RIGHT || dir == LEFT) {
                                                        if (rnow.y > 0
                                                            && !server_grids[rnow.y-1][rnow.x]
                                                            && !cabled_cluster[rnow.y-1][rnow.x]
                                                            && used_space[rnow.y-1][rnow.x] != used_space_id) {
                                                            best_dir = rdir;
                                                            nearest_space = Point(rnow.x, rnow.y-1);
                                                            break;
                                                        } else if (rnow.y < N-1
                                                                   && !server_grids[rnow.y+1][rnow.x]
                                                                   && !cabled_cluster[rnow.y+1][rnow.x]
                                                                   && used_space[rnow.y+1][rnow.x] != used_space_id) {
                                                            best_dir = rdir;
                                                            nearest_space = Point(rnow.x, rnow.y+1);
                                                            break;
                                                        }
                                                    } else {
                                                        if (rnow.x > 0
                                                            && !server_grids[rnow.y][rnow.x-1]
                                                            && !cabled_cluster[rnow.y][rnow.x-1]
                                                            && used_space[rnow.y][rnow.x-1] != used_space_id) {
                                                            best_dir = rdir;
                                                            nearest_space = Point(rnow.x-1, rnow.y);
                                                            break;
                                                        } else if (rnow.x < N-1
                                                                   && !server_grids[rnow.y][rnow.x+1]
                                                                   && !cabled_cluster[rnow.y][rnow.x+1]
                                                                   && used_space[rnow.y][rnow.x+1] != used_space_id) {
                                                            best_dir = rdir;
                                                            nearest_space = Point(rnow.x+1, rnow.y);
                                                            break;
                                                        }
                                                    }
                                                    rnow.move(rdir);
                                                }

                                                // 順方向
                                                rnow = now;
                                                rdir = dir;
                                                rnow.move(rdir);
                                                while (!server_grids[rnow.y][rnow.x] && !cabled_cluster[rnow.y][rnow.x]) {
                                                    if (dir == RIGHT || dir == LEFT) {
                                                        if (rnow.y > 0
                                                            && !server_grids[rnow.y-1][rnow.x]
                                                            && !cabled_cluster[rnow.y-1][rnow.x]
                                                            && used_space[rnow.y-1][rnow.x] != used_space_id) {
                                                            if (!nearest_space.is_valid()
                                                                || distance(rnow, now) + 1 < distance(now, nearest_space)) {
                                                                best_dir = rdir;
                                                                nearest_space = Point(rnow.x, rnow.y-1);
                                                            }
                                                            break;
                                                        } else if (rnow.y < N-1
                                                                   && !server_grids[rnow.y+1][rnow.x]
                                                                   && !cabled_cluster[rnow.y+1][rnow.x]
                                                                   && used_space[rnow.y+1][rnow.x] != used_space_id) {
                                                            if (!nearest_space.is_valid()
                                                                || distance(rnow, now) + 1 < distance(now, nearest_space)) {
                                                                best_dir = rdir;
                                                                nearest_space = Point(rnow.x, rnow.y + 1);
                                                            }
                                                            break;
                                                        }
                                                    } else {
                                                        if (rnow.x > 0
                                                            && !server_grids[rnow.y][rnow.x-1]
                                                            && !cabled_cluster[rnow.y][rnow.x-1]
                                                            && used_space[rnow.y][rnow.x-1] != used_space_id) {
                                                            if (!nearest_space.is_valid()
                                                                || distance(rnow, now) + 1 < distance(now, nearest_space)) {
                                                                best_dir = rdir;
                                                                nearest_space = Point(rnow.x - 1, rnow.y);
                                                            }
                                                            break;
                                                        } else if (rnow.x < N-1
                                                                   && !server_grids[rnow.y][rnow.x+1]
                                                                   && !cabled_cluster[rnow.y][rnow.x+1]
                                                                   && used_space[rnow.y][rnow.x+1] != used_space_id) {
                                                            if (!nearest_space.is_valid()
                                                                || distance(rnow, now) + 1 < distance(now, nearest_space)) {
                                                                best_dir = rdir;
                                                                nearest_space = Point(rnow.x + 1, rnow.y);
                                                            }
                                                            break;
                                                        }
                                                    }
                                                    rnow.move(rdir);
                                                }

                                                if (!nearest_space.is_valid()) break;

                                                auto now2 = now;
                                                while (distance(now2, nearest_space) > 1) {
                                                    moves.push_back(now2, best_dir);
                                                    now2.move(best_dir);
                                                }
                                                int dir_to_near = get_direction(now2, nearest_space);
                                                moves.push_back(now2, dir_to_near);

                                                used_space[nearest_space.y][nearest_space.x] = used_space_id;
                                            } else {
                                                auto now2 = nearest_space;
                                                while (now2 != now) {
                                                    int dir2 = prev_dir[now2.y][now2.x] & 3;
                                                    now2.move(dir2);
                                                    passed[now2.y][now2.x] = used_space_id;
                                                    moves.push_back(now2, (dir2 + 2) & 3);
                                                }

                                                used_space[nearest_space.y][nearest_space.x] = used_space_id;
                                            }
                                        } else {
                                            // fixed されたやつを縮めるパターン
                                            auto obstacle_server = server_grids[now.y][now.x];
                                            if (dir == RIGHT || dir == LEFT) {
                                                if (obstacle_server->can_move_to(UP)
                                                    && now.y > 0
                                                    && !server_grids[now.y - 1][now.x]
                                                    && !obstacle_server->next_server[DOWN]) {
                                                    moves.push_back(now, UP);
                                                } else if (obstacle_server->can_move_to(DOWN)
                                                           && now.y < N - 1
                                                           && !server_grids[now.y + 1][now.x]
                                                           && !obstacle_server->next_server[UP]) {
                                                    moves.push_back(now, DOWN);
                                                } else {
                                                    break;
                                                }
                                            } else {
                                                if (obstacle_server->can_move_to(RIGHT)
                                                    && now.x < N - 1
                                                    && !server_grids[now.y][now.x + 1]
                                                    && !obstacle_server->next_server[LEFT]) {
                                                    moves.push_back(now, RIGHT);
                                                } else if (obstacle_server->can_move_to(LEFT)
                                                           && now.x > 0
                                                           && !server_grids[now.y][now.x - 1]
                                                           && !obstacle_server->next_server[RIGHT]) {
                                                    moves.push_back(now, LEFT);
                                                } else {
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                } else {
                                    if (dir == RIGHT || dir == LEFT) {
                                        _supply_move_out_obstacle(UP,
                                                                  now,
                                                                  used_space,
                                                                  used_space_id,
                                                                  server,
                                                                  main_cluster,
                                                                  passed,
                                                                  best_cluster,
                                                                  moves,
                                                                  min_moves,
                                                                  tie_count);

                                        _supply_move_out_obstacle(DOWN,
                                                                  now,
                                                                  used_space,
                                                                  used_space_id,
                                                                  server,
                                                                  main_cluster,
                                                                  passed,
                                                                  best_cluster,
                                                                  moves,
                                                                  min_moves,
                                                                  tie_count);
                                    } else {
                                        _supply_move_out_obstacle(LEFT,
                                                                  now,
                                                                  used_space,
                                                                  used_space_id,
                                                                  server,
                                                                  main_cluster,
                                                                  passed,
                                                                  best_cluster,
                                                                  moves,
                                                                  min_moves,
                                                                  tie_count);

                                        _supply_move_out_obstacle(RIGHT,
                                                                  now,
                                                                  used_space,
                                                                  used_space_id,
                                                                  server,
                                                                  main_cluster,
                                                                  passed,
                                                                  best_cluster,
                                                                  moves,
                                                                  min_moves,
                                                                  tie_count);
                                    }
                                }
                            }
                            now.move(dir);
                        }
                    }
                    if (sdir < 4) {
                        rep(i, pre_move_len) {
                            move_server(server, (sdir + 2) & 3);
                        }
                    }
                }
            }
        }

        if (!min_moves.empty()) {
            propositions.push_back(Proposition(Proposition::Type::MOVE_OUT_OBSTACLE, best_cluster, min_moves.get_vec()));
        }
    }

    void add_move_onto_sub_cable_proposition(shared_ptr<Cluster>& main_cluster,
                                             vector<Proposition>& propositions) {
        shared_ptr<Server> best_server;
        shared_ptr<Cluster> target_cluster;
        int best_move_dir;
        int min_move_count = INF;
        for (auto& server: main_cluster->servers) {
            rep(dir, 4) {
                if (server->next_server[dir]) continue;
                if (!server->can_move_to(dir)) continue;
                Point now = *server;
                int move_count = 1;
                now.move(dir);
                while (now.is_valid() && !server_grids[now.y][now.x] && move_count < 10) {
                    if (cabled_cluster[now.y][now.x]) {
                        if (cabled_cluster[now.y][now.x] != main_cluster
                            && cabled_cluster[now.y][now.x]->server_type == main_cluster->server_type
                            && move_count < min_move_count) {
                            min_move_count = move_count;
                            best_server = server;
                            best_move_dir = dir;
                            target_cluster = cabled_cluster[now.y][now.x];
                        }
                        break;
                    }
                    move_count++;
                    now.move(dir);
                }
            }
        }

        if (best_server) {
            Point now = *best_server;
            vector<MoveAction> moves;
            rep(i, min_move_count) {
                moves.push_back(MoveAction(now, best_move_dir));
                now.move(best_move_dir);
            }
            propositions.push_back(Proposition(Proposition::Type::MOVE_ONTO_SUB_CABLE, target_cluster, moves));
        }
    }

    void add_move_onto_main_cable_proposition(shared_ptr<Cluster>& main_cluster,
                                              shared_ptr<Cluster>& sub_cluster,
                                              vector<Proposition>& propositions) {
        shared_ptr<Server> best_server;
        int best_move_dir;
        int min_move_count = INF;
        for (auto& server: sub_cluster->servers) {
            rep(dir, 4) {
                if (server->next_server[dir]) continue;
                if (!server->can_move_to(dir)) continue;
                Point now = *server;
                int move_count = 1;
                now.move(dir);
                while (now.is_valid() && !server_grids[now.y][now.x]) {
                    if (cabled_cluster[now.y][now.x]) {
                        if (cabled_cluster[now.y][now.x] == main_cluster
                            && move_count < min_move_count) {
                            min_move_count = move_count;
                            best_server = server;
                            best_move_dir = dir;
                        }
                        break;
                    }
                    move_count++;
                    now.move(dir);
                }
            }
        }

        if (best_server) {
            Point now = *best_server;
            vector<MoveAction> moves;
            rep(i, min_move_count) {
                moves.push_back(MoveAction(now, best_move_dir));
                now.move(best_move_dir);
            }
            propositions.push_back(Proposition(Proposition::Type::MOVE_ONTO_MAIN_CABLE, sub_cluster, moves));
        }
    }

    void add_move_solo_cluster_proposition(shared_ptr<Cluster> main_cluster,
                                           shared_ptr<Cluster> sub_cluster,
                                           vector<Proposition>& propositions) {
        bool laser[MAX_N][MAX_N];
        memset(laser, false, sizeof(laser));
        for (auto& server: main_cluster->servers) {
            rep(dir, 4) {
                if (server->next_server[dir]) continue;
                Point now = *server;
                now.move(dir);
                while (now.is_valid() && !cabled_cluster[now.y][now.x] && !server_grids[now.y][now.x]) {
                    laser[now.y][now.x] = true;
                    now.move(dir);
                }
            }
        }

        int prev_dir[MAX_N][MAX_N];
        memset(prev_dir, 0, sizeof(prev_dir));
        int prev_dir_prefix = 0;
        for (auto server: sub_cluster->servers) {
            if (server->degree() > 1) continue;
            prev_dir_prefix += 0x10;
            PointQueue q;
            q.push(*server);
            prev_dir[server->y][server->x] = prev_dir_prefix + 10;
            Point best_destination;
            while (!q.empty()) {
                auto now = q.front();
                q.pop();
                if (laser[now.y][now.x]) {
                    best_destination = now;
                    break;
                }
                if (cabled_cluster[now.y][now.x] == main_cluster) {
                    best_destination = now;
                    break;
                }
                rep(dir, 4) {
                    auto next = now;
                    next.move(dir);
                    if (!next.is_valid() || server_grids[next.y][next.x]) continue;
                    if (prev_dir[next.y][next.x] >= prev_dir_prefix) continue;
                    prev_dir[next.y][next.x] = prev_dir_prefix + ((dir + 2) & 3);
                    q.push(next);
                }
            }

            if (!best_destination.is_valid()) return;

            vector<MoveAction> moves;
            Point now = best_destination;
            while (now != *server) {
                int dir = prev_dir[now.y][now.x] & 3;
                now.move(dir);
                moves.push_back(MoveAction(now, (dir + 2) & 3));
            }
            reverse(all(moves));

            Proposition::Type proposition_type = cabled_cluster[best_destination.y][best_destination.x]
                                                 ? Proposition::Type::MOVE_SOLO_CLUSTER_ONTO_CABLE
                                                 : Proposition::Type::MOVE_SOLO_CLUSTER_TO_LASER;
            if (server->degree() == 0) {
                propositions.push_back(Proposition(proposition_type, sub_cluster, moves));
            } else {
                auto new_cluster = make_shared<Cluster>(server->type);
                new_cluster->servers.push_back(server);
                propositions.push_back(Proposition(proposition_type, new_cluster, moves));
            }
        }
    }

    int calc_slide_cost(const Point& p1, const Point& p2) {
        int d1 = abs(p1.x - p2.x);
        int d2 = abs(p1.y - p2.y);
        if (d1 > d2) swap(d1, d2);
        if (d2 - d1 <= 1) return 1 + 3 * (d1 + d2 - 1);
        return 1 + 3 * (2 * d1) + 5 * (d2 - 1 - d1);
    }

    void push_and_move(Point& p, int dir, vector<MoveAction>& moves) {
        p.move(dir);
        moves.push_back(MoveAction(p, (dir + 2) & 3));
    }

    void add_slide_solo_cluster_proposition(shared_ptr<Cluster>& main_cluster,
                                            vector<shared_ptr<Cluster>>& clusters,
                                            vector<Proposition>& propositions) {
        vector<Point> valid_pts;
        for (auto& server: main_cluster->servers) {
            rep(dir, 4) {
                if (server->next_server[dir]) continue;
                Point now = *server;
                now.move(dir);
                if (!now.is_valid()
                    || cabled_cluster[now.y][now.x]
                    || !server_grids[now.y][now.x]
                    || server_grids[now.y][now.x]->type == main_cluster->server_type) continue;
                valid_pts.push_back(now);
            }
        }

        int nearest_space_dists[MAX_N][MAX_N];
        int prev_dir_to_space[MAX_N][MAX_N];
        rep(y, N) {
            rep(x, N) {
                nearest_space_dists[y][x] = (server_grids[y][x] || cabled_cluster[y][x]) ? INF : 0;
            }
        }
        memset(prev_dir_to_space, -1, sizeof(prev_dir_to_space));
        rep(y, N) {
            rep(x, N) {
                if (server_grids[y][x] || cabled_cluster[y][x]) continue;
                PointQueue q;
                q.push(Point(x, y));
                while (!q.empty()) {
                    auto now = q.front(); q.pop();
                    rep(dir, 4) {
                        auto next = now;
                        next.move(dir);
                        if (!next.is_valid()
                            || cabled_cluster[next.y][next.x]
                            || !server_grids[next.y][next.x]
                            || server_grids[next.y][next.x]->fixed
                            || server_grids[next.y][next.x]->type == main_cluster->server_type
                            || nearest_space_dists[next.y][next.x] <= nearest_space_dists[now.y][now.x] + 1) continue;
                        nearest_space_dists[next.y][next.x] = nearest_space_dists[now.y][now.x] + 1;
                        prev_dir_to_space[next.y][next.x] = (dir + 2) & 3;
                        q.push(next);
                    }
                }
            }
        }

        int min_cost = INF;
        shared_ptr<Server> best_server;
        Point best_target;
        for (auto& sub_cluster: clusters) {
            if (sub_cluster == main_cluster) continue;
            if (K > 2 && distance(*sub_cluster, *main_cluster) > 3) continue;
            if (sub_cluster->servers.size() != 1) continue; // TODO: purge する

            auto sub_server = sub_cluster->servers[0];
            for (auto& target: valid_pts) {
                bool ok_square = true;
                if (distance(target, *sub_server) >= 10) continue;
                if (target.x == sub_server->x || target.y == sub_server->y) continue;
                REP(x, min(sub_server->x, target.x), max(sub_server->x, target.x)) {
                    REP(y, min(sub_server->y, target.y), max(sub_server->y, target.y)) {
                        if (!server_grids[y][x]
                            || cabled_cluster[y][x]
                            || server_grids[y][x]->cluster && server_grids[y][x]->cluster != sub_cluster) {
                            ok_square = false;
                            break;
                        }
                    }
                    if (!ok_square) break;
                }

                if (!ok_square) continue;

                rep(sdir, 4) {
                    Point next = *sub_server;
                    next.move(sdir);
                    if (next.x < min(sub_server->x, target.x)
                        || next.x > max(sub_server->x, target.x)
                        || next.y < min(sub_server->y, target.y)
                        || next.y > max(sub_server->y, target.y)) continue;
                    int cost = nearest_space_dists[next.y][next.x] + calc_slide_cost(*sub_server, target);
                    if (cost < min_cost) {
                        min_cost = cost;
                        best_server = sub_server;
                        best_target = target;
                    }
                }
            }
        }

        if (best_server) {
            int pdir;
            Point start_pt = *best_server;
            Point prev_pt = *best_server;
            if (best_server->x < best_target.x && best_server->y < best_target.y) {
                if (abs(best_server->x - best_target.x) < abs(best_server->y - best_target.y)) pdir = DOWN;
                else pdir = RIGHT;
            } else if (best_server->x < best_target.x && best_server->y > best_target.y) {
                if (abs(best_server->x - best_target.x) < abs(best_server->y - best_target.y)) pdir = UP;
                else pdir = RIGHT;
            } else if (best_server->x > best_target.x && best_server->y < best_target.y) {
                if (abs(best_server->x - best_target.x) < abs(best_server->y - best_target.y)) pdir = DOWN;
                else pdir = LEFT;
            } else if (best_server->x > best_target.x && best_server->y > best_target.y) {
                if (abs(best_server->x - best_target.x) < abs(best_server->y - best_target.y)) pdir = UP;
                else pdir = LEFT;
            } else assert(false);
            start_pt.move(pdir);

            vector<MoveAction> moves;
            auto now = start_pt;
            while (server_grids[now.y][now.x]) {
                int dir = prev_dir_to_space[now.y][now.x];
                moves.push_back(MoveAction(now, dir));
                now.move(dir);
            }
            reverse(all(moves));

            moves.push_back(MoveAction(*best_server, pdir));

            now = start_pt;
            Point prev_now;
            while (now != best_target) {
                assert(prev_now != now);
                prev_now = now;
                if (pdir == UP) {
                    if (now.x < best_target.x) {
                        push_and_move(prev_pt, RIGHT, moves);
                        push_and_move(prev_pt, UP, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, LEFT, moves);
                        pdir = RIGHT;
                    } else if (now.x > best_target.x) {
                        push_and_move(prev_pt, LEFT, moves);
                        push_and_move(prev_pt, UP, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, RIGHT, moves);
                        pdir = LEFT;
                    } else if (best_server->x < best_target.x) {
                        push_and_move(prev_pt, LEFT, moves);
                        push_and_move(prev_pt, UP, moves);
                        push_and_move(prev_pt, UP, moves);
                        push_and_move(prev_pt, RIGHT, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, DOWN, moves);
                        pdir = UP;
                    } else {
                        push_and_move(prev_pt, RIGHT, moves);
                        push_and_move(prev_pt, UP, moves);
                        push_and_move(prev_pt, UP, moves);
                        push_and_move(prev_pt, LEFT, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, DOWN, moves);
                        pdir = UP;
                    }
                } else if (pdir == DOWN) {
                    if (now.x < best_target.x) {
                        push_and_move(prev_pt, RIGHT, moves);
                        push_and_move(prev_pt, DOWN, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, LEFT, moves);
                        pdir = RIGHT;
                    } else if (now.x > best_target.x) {
                        push_and_move(prev_pt, LEFT, moves);
                        push_and_move(prev_pt, DOWN, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, RIGHT, moves);
                        pdir = LEFT;
                    } else if (best_server->x < best_target.x) {
                        push_and_move(prev_pt, LEFT, moves);
                        push_and_move(prev_pt, DOWN, moves);
                        push_and_move(prev_pt, DOWN, moves);
                        push_and_move(prev_pt, RIGHT, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, UP, moves);
                        pdir = DOWN;
                    } else {
                        push_and_move(prev_pt, RIGHT, moves);
                        push_and_move(prev_pt, DOWN, moves);
                        push_and_move(prev_pt, DOWN, moves);
                        push_and_move(prev_pt, LEFT, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, UP, moves);
                        pdir = DOWN;
                    }
                } else if (pdir == LEFT) {
                    if (now.y < best_target.y) {
                        push_and_move(prev_pt, DOWN, moves);
                        push_and_move(prev_pt, LEFT, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, UP, moves);
                        pdir = DOWN;
                    } else if (now.y > best_target.y) {
                        push_and_move(prev_pt, UP, moves);
                        push_and_move(prev_pt, LEFT, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, DOWN, moves);
                        pdir = UP;
                    } else if (best_server->y < best_target.y) {
                        push_and_move(prev_pt, UP, moves);
                        push_and_move(prev_pt, LEFT, moves);
                        push_and_move(prev_pt, LEFT, moves);
                        push_and_move(prev_pt, DOWN, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, RIGHT, moves);
                        pdir = LEFT;
                    } else {
                        push_and_move(prev_pt, DOWN, moves);
                        push_and_move(prev_pt, LEFT, moves);
                        push_and_move(prev_pt, LEFT, moves);
                        push_and_move(prev_pt, UP, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, RIGHT, moves);
                        pdir = LEFT;
                    }
                } else if (pdir == RIGHT) {
                    if (now.y < best_target.y) {
                        push_and_move(prev_pt, DOWN, moves);
                        push_and_move(prev_pt, RIGHT, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, UP, moves);
                        pdir = DOWN;
                    } else if (now.y > best_target.y) {
                        push_and_move(prev_pt, UP, moves);
                        push_and_move(prev_pt, RIGHT, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, DOWN, moves);
                        pdir = UP;
                    } else if (best_server->y < best_target.y) {
                        push_and_move(prev_pt, UP, moves);
                        push_and_move(prev_pt, RIGHT, moves);
                        push_and_move(prev_pt, RIGHT, moves);
                        push_and_move(prev_pt, DOWN, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, LEFT, moves);
                        pdir = RIGHT;
                    } else {
                        push_and_move(prev_pt, DOWN, moves);
                        push_and_move(prev_pt, RIGHT, moves);
                        push_and_move(prev_pt, RIGHT, moves);
                        push_and_move(prev_pt, UP, moves);
                        now = prev_pt;
                        push_and_move(prev_pt, LEFT, moves);
                        pdir = RIGHT;
                    }
                } else assert(false);
            }

            propositions.push_back(Proposition(Proposition::Type::SLIDE_SOLO_CLUSTER, best_server->cluster, moves));
        }
    }

    vector<Proposition> get_merge_propositions(vector<shared_ptr<Cluster>>& clusters, shared_ptr<Cluster>& main_cluster) {
        vector<Proposition> propositions;

        if (K > 2) main_cluster->update_min_max_pts();
        for (auto& sub_cluster: clusters) {
            if (sub_cluster == main_cluster) continue;
            if (K > 2) {
                sub_cluster->update_min_max_pts();
                if (distance(*sub_cluster, *main_cluster) > 3) continue;
            }
            add_move_to_laser_proposition(main_cluster, sub_cluster, sub_cluster, propositions);
            add_move_to_laser_proposition(sub_cluster, main_cluster, sub_cluster, propositions);
            add_move_onto_main_cable_proposition(main_cluster, sub_cluster, propositions);
            add_move_solo_cluster_proposition(main_cluster, sub_cluster, propositions);
        }

        add_move_out_obstacle_proposition(main_cluster, propositions);
        add_move_onto_sub_cable_proposition(main_cluster, propositions);
        if (density > 0.685) add_slide_solo_cluster_proposition(main_cluster, clusters, propositions);

        return propositions;
    }

    void move_server(shared_ptr<Server> server, int dir) {
        assert(server);
        int rdir = (dir + 2) & 3;
        if (server->next_server[rdir]) {
            cabled_cluster[server->y][server->x] = server->cluster;
        }

        server_grids[server->y][server->x] = nullptr;
        server->move(dir);
        assert(server->is_valid() && !server_grids[server->y][server->x]);
        server_grids[server->y][server->x] = server;
        if (cabled_cluster[server->y][server->x] == server->cluster) {
            cabled_cluster[server->y][server->x] = nullptr;
        }
    }

    void apply_moves(const vector<MoveAction>& moves, bool debug = false) {
        answer.moves.insert(answer.moves.end(), all(moves));

        if (debug) {
            print_board(cerr);
            cerr << "moves:";
            for (auto &move: moves) {
                cerr << "(" << move.from << ", " << move.direction << ")";
            }
            cerr << endl;
        }
        for (auto& move: moves) {
            move_server(server_grids[move.from.y][move.from.x], move.direction);
        }
    }

    void merge_clusters(vector<shared_ptr<Cluster>>& clusters, shared_ptr<Cluster>& main_cluster, shared_ptr<Cluster>& sub_cluster, Proposition::Type proposition_type) {
        bool connected = false;
        if (proposition_type == Proposition::Type::MOVE_TO_LASER
            || proposition_type == Proposition::Type::MOVE_OUT_OBSTACLE
            || proposition_type == Proposition::Type::MOVE_SOLO_CLUSTER_TO_LASER
            || proposition_type == Proposition::Type::SLIDE_SOLO_CLUSTER) {
            for (auto &server: main_cluster->servers) {
                rep(dir, 4) {
                    if (server->next_server[dir]) continue;
                    Point now = *server;
                    now.move(dir);
                    while (now.is_valid() && !cabled_cluster[now.y][now.x]) {
                        if (server_grids[now.y][now.x]) {
                            if (server_grids[now.y][now.x]->cluster == sub_cluster) {
                                int prev_dir = (dir + 2) & 3;
                                server->next_server[dir] = server_grids[now.y][now.x];
                                server_grids[now.y][now.x]->next_server[prev_dir] = server;
                                assert(server_grids[now.y][now.x]->type == server->type);
                                Point rnow = now;
                                rnow.move(prev_dir);
                                while (rnow != *server) {
                                    cabled_cluster[rnow.y][rnow.x] = main_cluster;
                                    rnow.move(prev_dir);
                                }
                                connected = true;
                            }
                            break;
                        }
                        now.move(dir);
                    }
                    if (connected) break;
                }
                if (connected) break;
            }
        } else if (proposition_type == Proposition::Type::MOVE_ONTO_SUB_CABLE
                   || proposition_type == Proposition::Type::MOVE_ONTO_MAIN_CABLE
                   || proposition_type == Proposition::Type::MOVE_SOLO_CLUSTER_ONTO_CABLE) {
            shared_ptr<Cluster> target_cluster;
            if (proposition_type == Proposition::Type::MOVE_ONTO_SUB_CABLE) target_cluster = sub_cluster;
            else if (proposition_type == Proposition::Type::MOVE_ONTO_MAIN_CABLE
                     || proposition_type == Proposition::Type::MOVE_SOLO_CLUSTER_ONTO_CABLE) target_cluster = main_cluster;
            for (auto& server: target_cluster->servers) {
                rep(dir, 4) {
                    if (!server->next_server[dir]) continue;
                    Point now = *server;
                    now.move(dir);
                    while (now.is_valid()) {
                        if (server_grids[now.y][now.x]) {
                            auto hit_server = server_grids[now.y][now.x];
                            assert(hit_server->x == now.x && hit_server->y == now.y);
                            if (hit_server->cluster == server->cluster) break;
                            auto pair_server = server->next_server[dir];
                            int rdir = (dir + 2) & 3;
                            server->next_server[dir] = hit_server;
                            pair_server->next_server[rdir] = hit_server;
                            hit_server->next_server[dir] = pair_server;
                            hit_server->next_server[rdir] = server;
                            cabled_cluster[hit_server->y][hit_server->x] = nullptr;
                            assert(server->x == hit_server->x || server->y == hit_server->y);
                            assert(pair_server->x == hit_server->x || pair_server->y == hit_server->y);
                            connected = true;
                            break;
                        }
                        now.move(dir);
                    }
                    if (connected) break;
                }
                if (connected) break;
            }
        }
        if (!connected) {
            ofstream fout("debug.txt");
            answer.print(fout);
        }
        assert(connected);

        rep(y, N) {
            rep(x, N) {
                if (cabled_cluster[y][x] == sub_cluster) {
                    cabled_cluster[y][x] = main_cluster;
                }
            }
        }

        for (auto& server: sub_cluster->servers) {
            server->cluster = main_cluster;
        }
        main_cluster->servers.insert(main_cluster->servers.end(), all(sub_cluster->servers));

        for (auto it = clusters.begin(); it != clusters.end(); it++) {
            if (*it == sub_cluster) {
                clusters.erase(it);
                break;
            }
        }
    }

    void purge_server(shared_ptr<Server> server) {
        assert(server->degree() == 1);
        rep(dir, 4) {
            if (!server->next_server[dir]) continue;
            Point now = *server;
            now.move(dir);
            while (!server_grids[now.y][now.x]) {
                cabled_cluster[now.y][now.x] = nullptr;
                now.move(dir);
            }
            server->next_server[dir]->next_server[(dir + 2) & 3] = nullptr;
            server->next_server[dir] = nullptr;

        }
        for (auto it = server->cluster->servers.begin(); it != server->cluster->servers.end(); it++) {
            if (server == *it) {
                server->cluster->servers.erase(it);
                break;
            }
        }
        server->cluster = nullptr;
    }

    void draft_and_execute_propositions(vector<shared_ptr<Cluster>>& clusters) {
        shared_ptr<Cluster> main_cluster;
        int rnd = rand_xor() % 100;
        for (auto& cluster: clusters) {
            if (rnd < cluster->servers.size()) {
                main_cluster = cluster;
                break;
            }
            rnd -= cluster->servers.size();
        }
        assert(main_cluster);

        int remaining_step_num = answer.get_remaining_action_num() - main_cluster->servers.size() - 1;
        while (clusters.size() > 1 && timer.get_ms() < TIME_LIMIT) {
            auto propositions = get_merge_propositions(clusters, main_cluster);

            if (propositions.empty()) {
                if (density < 0.63) break;
                if (!compress_leaves(main_cluster)) break;
                continue;
            }

            Proposition* best_proposition = nullptr;
            double min_cost = INF;
            for (auto& proposition: propositions) {
                if (remaining_step_num < (int) (proposition.target_cluster->servers.size() + proposition.moves.size())) continue;
                if (proposition.cost < min_cost) {
                    min_cost = proposition.cost;
                    best_proposition = &proposition;
                }
            }
            if (!best_proposition) break;

            auto& sub_cluster = best_proposition->target_cluster;
            auto& moves = best_proposition->moves;

            remaining_step_num -= sub_cluster->servers.size() + moves.size();

            if (sub_cluster->servers.size() == 1
                && sub_cluster->servers[0]->cluster != sub_cluster) {
                auto server = sub_cluster->servers[0];
                purge_server(server);
                server->cluster = sub_cluster;
                clusters.push_back(sub_cluster);
            }

            apply_moves(moves);
            merge_clusters(clusters, main_cluster, sub_cluster, best_proposition->type);
        }

        for (auto& cluster: clusters) {
            if (cluster->servers.size() < 3) release_cluster(cluster);
            else cluster->connect(answer);
        }
    }

    void release_cluster(shared_ptr<Cluster>& cluster) {
        for (auto& server: cluster->servers) {
            rep(dir, 4) {
                if (!server->next_server[dir]) continue;
                Point now = *server;
                now.move(dir);
                while (now != *server->next_server[dir]) {
                    cabled_cluster[now.y][now.x] = nullptr;
                    now.move(dir);
                }
                server->next_server[dir]->next_server[(dir + 2) & 3] = nullptr;
                server->next_server[dir] = nullptr;
            }
        }
    }

    void push_main_servers_into_center() {
        int prev_dir[MAX_N][MAX_N];
        memset(prev_dir, -1, sizeof(prev_dir));
        rep(y, N) {
            rep(x, N) {
                if (x != 0 && x != N-1 && y != 0 && y != N-1) continue;
                auto server = server_grids[y][x];
                if (!server || server->type != main_server_type) continue;
                Point best_destination;
                PointQueue q;
                q.push(*server);
                prev_dir[y][x] = -2;
                while (!q.empty()) {
                    auto now = q.front(); q.pop();
                    if (now.x != 0 && now.x != N-1 && now.y != 0 && now.y != N-1) {
                        best_destination = now;
                        break;
                    }
                    rep(dir, 4) {
                        auto next = now;
                        next.move(dir);
                        if (!next.is_valid() || server_grids[next.y][next.x]) continue;
                        if (prev_dir[next.y][next.x] != -1) continue;
                        prev_dir[next.y][next.x] = (dir + 2) & 3;
                        q.push(next);
                    }
                }
                if (!best_destination.is_valid()) continue;

                vector<MoveAction> moves;
                auto now = best_destination;
                while (now != *server) {
                    int dir = prev_dir[now.y][now.x];
                    now.move(dir);
                    moves.push_back(MoveAction(now, (dir + 2) & 3));
                }
                reverse(all(moves));

                apply_moves(moves);
            }
        }
    }

    void print_board(ostream& out) {
        rep(y, N) {
            rep(x, N) {
                if (server_grids[y][x]) out << server_grids[y][x]->type;
                else if (cabled_cluster[y][x]) out << '#';
                else out << '.';
            }
            cerr << endl;
        }
    }

    bool compress_leaves(shared_ptr<Cluster>& cluster) {
        bool moved = false;
        for (auto server: cluster->servers) {
            if (server->degree() != 1) continue;
            rep(dir, 4) {
                if (!server->next_server[dir]) continue;
                while (answer.get_remaining_action_num() >= 100) {
                    Point next = *server;
                    next.move(dir);
                    if (server_grids[next.y][next.x]) break;
                    moved = true;
                    answer.moves.push_back(MoveAction(*server, dir));
                    move_server(server, dir);
                    break;
                }
            }
        }
        return moved;
    }

public:
    SingleTypeSolver(const vector<string>& grids, int main_server_type, Timer& timer)
        : Solver(grids), main_server_type(main_server_type), original_grids(grids), timer(timer) {}

    Answer solve() {
        vector<shared_ptr<Cluster>> clusters[MAX_K+1];

        push_main_servers_into_center();
        construct_clusters(clusters[main_server_type], main_server_type);
        draft_and_execute_propositions(clusters[main_server_type]);

        vector<int> other_server_types;
        REP(server_type, 1, K) {
            if (server_type == main_server_type) continue;
            other_server_types.push_back(server_type);
        }
        shuffle(all(other_server_types), mt19937_engine);

        if (other_server_types.size() > 1) {
            auto best_answer = answer;
            rep(i, other_server_types.size()) {
                auto tmp_answer = SingleTypeSolver(original_grids, main_server_type, timer).polish(answer,
                                                                                                   other_server_types);
                if (tmp_answer.score > best_answer.score) {
                    swap(tmp_answer, best_answer);
                }
                other_server_types.push_back(other_server_types[0]);
                other_server_types.erase(other_server_types.begin());
                if (timer.get_ms() >= TIME_LIMIT) break;
            }
            return best_answer;
        }

        int other_server_type = other_server_types[0];
        construct_clusters(clusters[other_server_type], other_server_type);
        draft_and_execute_propositions(clusters[other_server_type]);

        return answer;
    }

    Answer polish(const Answer& base_answer, const vector<int> other_server_types) {
        vector<shared_ptr<Cluster>> clusters[MAX_K+1];

        apply_moves(base_answer.moves);

        construct_clusters(clusters[main_server_type], main_server_type);
        for (auto& cluster: clusters[main_server_type]) {
            if (cluster->servers.size() < 3) release_cluster(cluster);
            else cluster->connect(answer);
        }

        for (int server_type: other_server_types) {
            construct_clusters(clusters[server_type], server_type);
            draft_and_execute_propositions(clusters[server_type]);
            if (timer.get_ms() >= TIME_LIMIT) break;
        }

        return answer;
    }
};

class AnimationPrinter {
protected:
    void dfs(Point now, int prev_dir, const vector<string>& grids, Answer& answer, bool visited[MAX_N][MAX_N]) {
        visited[now.y][now.x] = true;
        rep(dir, 4) {
            if (dir == prev_dir) continue;
            Point next = now;
            next.move(dir);
            while (next.is_valid() && grids[next.y][next.x] == '0' && !visited[next.y][next.x]) {
                next.move(dir);
            }
            if (!next.is_valid() || visited[next.y][next.x] || grids[next.y][next.x] != grids[now.y][now.x]) continue;
            int prev_dir = (dir + 2) & 3;
            Point rnow = next;
            rnow.move(prev_dir);
            while (rnow != now) {
                visited[rnow.y][rnow.x] = true;
                rnow.move(prev_dir);
            }
            answer.connects.push_back(mp(make_shared<Server>(now.x, now.y, 0), make_shared<Server>(next.x, next.y, 0)));
            dfs(next, prev_dir, grids, answer, visited);
        }
    }

    vector<string> grids;
    const Answer& answer;

public:
    AnimationPrinter(vector<string> grids, const Answer& answer) : grids(grids), answer(answer) {
    }

    void print(ofstream& out) {
        bool visited[MAX_N][MAX_N];
        int main_server_type = -1;
        rep(i, answer.moves.size()) {
            auto& move = answer.moves[i];
            int server_type = grids[move.from.y][move.from.x];
            if (main_server_type == -1) {
                main_server_type = server_type;
            }
            swap(grids[move.from.y][move.from.x], grids[move.to.y][move.to.x]);

            Answer tmp_answer;
            rep(j, i+1) tmp_answer.moves.push_back(answer.moves[j]);

            memset(visited, false, sizeof(visited));
            rep(y, N) {
                rep(x, N) {
                    if (grids[y][x] != '5') continue;
                    if (visited[y][x]) continue;
                    dfs(Point(x, y), -1, grids, tmp_answer, visited);
                }
            }
            rep(y, N) {
                rep(x, N) {
                    if (grids[y][x] != '4') continue;
                    if (visited[y][x]) continue;
                    dfs(Point(x, y), -1, grids, tmp_answer, visited);
                }
            }
            rep(y, N) {
                rep(x, N) {
                    if (grids[y][x] != '2') continue;
                    if (visited[y][x]) continue;
                    dfs(Point(x, y), -1, grids, tmp_answer, visited);
                }
            }

            tmp_answer.print(out);
        }
    }
};

int main() {
    cin >> N >> K;

    Timer timer;

    vector<string> grids(N);
    rep(y, N) cin >> grids[y];

    int server_count = 0;
    rep(y, N) {
        rep(x, N) {
            if (grids[y][x] != '0') server_count++;
        }
    }
    density = (double) server_count / N / N;
    // ofstream fout("params.txt", ios::app);
    // fout << density << endl;

    rep(i, MAX_N+1) sqrt_memo[i] = sqrt(i);

    Answer best_answer;
    int try_count = 0;
    int score_sum[MAX_K+1];
    memset(score_sum, 0, sizeof(score_sum));
    REP(target_type, 1, K) {
        try_count++;
        auto answer = SingleTypeSolver(grids, target_type, timer).solve();
        score_sum[target_type] += answer.score;
        assert(answer.get_remaining_action_num() >= 0);
        if (answer.score > best_answer.score) {
            swap(answer, best_answer);
        }
    }
    while (timer.get_ms() < TIME_LIMIT) {
        int score_sum_sum = 0;
        REP(type, 1, K) score_sum_sum += score_sum[type];
        int rnd = rand_xor() % score_sum_sum;
        int target_type = -1;
        REP(type, 1, K) {
            if (rnd < score_sum[type]) {
                target_type = type;
                break;
            }
            rnd -= score_sum[type];
        }
        assert(target_type != -1);
        try_count++;
        auto answer = SingleTypeSolver(grids, target_type, timer).solve();
        assert(answer.get_remaining_action_num() >= 0);
        if (answer.score > best_answer.score) {
            swap(answer, best_answer);
        }
        if (timer.get_ms() >= TIME_LIMIT) break;
    }
    best_answer.print(cout);

    // cerr << "try_count: " << try_count << endl;

    // ofstream fout("history.txt");
    // AnimationPrinter(grids, best_answer).print(fout);

    // ofstream fout("remaining_step.txt", ios::app);
    // fout << best_answer.get_remaining_action_num() << endl;

    // ofstream fout("time.txt", ios::app);
    // fout << timer.get_ms() << "ms" << endl;
}
