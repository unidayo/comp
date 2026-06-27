#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <complex>
#include <deque>
#include <forward_list>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
using namespace std;
using lint = long long;
using pint = pair<int, int>;
using plint = pair<lint, lint>;
struct fast_ios { fast_ios(){ cin.tie(nullptr), ios::sync_with_stdio(false), cout << fixed << setprecision(20); }; } fast_ios_;
#define ALL(x) (x).begin(), (x).end()
#define FOR(i, begin, end) for(int i=(begin),i##_end_=(end);i<i##_end_;i++)
#define IFOR(i, begin, end) for(int i=(end)-1,i##_begin_=(begin);i>=i##_begin_;i--)
#define REP(i, n) FOR(i,0,n)
#define IREP(i, n) IFOR(i,0,n)
template <typename T, typename V>
void ndarray(vector<T>& vec, const V& val, int len) { vec.assign(len, val); }
template <typename T, typename V, typename... Args> void ndarray(vector<T>& vec, const V& val, int len, Args... args) { vec.resize(len), for_each(begin(vec), end(vec), [&](T& v) { ndarray(v, val, args...); }); }
template <typename T> bool chmax(T &m, const T q) { return m < q ? (m = q, true) : false; }
template <typename T> bool chmin(T &m, const T q) { return m > q ? (m = q, true) : false; }
int floor_lg(long long x) { return x <= 0 ? -1 : 63 - __builtin_clzll(x); }
template <typename T1, typename T2> pair<T1, T2> operator+(const pair<T1, T2> &l, const pair<T1, T2> &r) { return make_pair(l.first + r.first, l.second + r.second); }
template <typename T1, typename T2> pair<T1, T2> operator-(const pair<T1, T2> &l, const pair<T1, T2> &r) { return make_pair(l.first - r.first, l.second - r.second); }
template <typename T> vector<T> sort_unique(vector<T> vec) { sort(vec.begin(), vec.end()), vec.erase(unique(vec.begin(), vec.end()), vec.end()); return vec; }
template <typename T> int arglb(const std::vector<T> &v, const T &x) { return std::distance(v.begin(), std::lower_bound(v.begin(), v.end(), x)); }
template <typename T> int argub(const std::vector<T> &v, const T &x) { return std::distance(v.begin(), std::upper_bound(v.begin(), v.end(), x)); }
template <typename T> istream &operator>>(istream &is, vector<T> &vec) { for (auto &v : vec) is >> v; return is; }
template <typename T> ostream &operator<<(ostream &os, const vector<T> &vec) { os << '['; for (auto v : vec) os << v << ','; os << ']'; return os; }
template <typename T, size_t sz> ostream &operator<<(ostream &os, const array<T, sz> &arr) { os << '['; for (auto v : arr) os << v << ','; os << ']'; return os; }
#if __cplusplus >= 201703L
template <typename... T> istream &operator>>(istream &is, tuple<T...> &tpl) { std::apply([&is](auto &&... args) { ((is >> args), ...);}, tpl); return is; }
template <typename... T> ostream &operator<<(ostream &os, const tuple<T...> &tpl) { os << '('; std::apply([&os](auto &&... args) { ((os << args << ','), ...);}, tpl); return os << ')'; }
#endif
template <typename T> ostream &operator<<(ostream &os, const deque<T> &vec) { os << "deq["; for (auto v : vec) os << v << ','; os << ']'; return os; }
template <typename T> ostream &operator<<(ostream &os, const set<T> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <typename T, typename TH> ostream &operator<<(ostream &os, const unordered_set<T, TH> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <typename T> ostream &operator<<(ostream &os, const multiset<T> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <typename T> ostream &operator<<(ostream &os, const unordered_multiset<T> &vec) { os << '{'; for (auto v : vec) os << v << ','; os << '}'; return os; }
template <typename T1, typename T2> ostream &operator<<(ostream &os, const pair<T1, T2> &pa) { os << '(' << pa.first << ',' << pa.second << ')'; return os; }
template <typename TK, typename TV> ostream &operator<<(ostream &os, const map<TK, TV> &mp) { os << '{'; for (auto v : mp) os << v.first << "=>" << v.second << ','; os << '}'; return os; }
template <typename TK, typename TV, typename TH> ostream &operator<<(ostream &os, const unordered_map<TK, TV, TH> &mp) { os << '{'; for (auto v : mp) os << v.first << "=>" << v.second << ','; os << '}'; return os; }
#ifdef HITONANODE_LOCAL
using BS = bitset<150>;
const string COLOR_RESET = "\033[0m", BRIGHT_GREEN = "\033[1;32m", BRIGHT_RED = "\033[1;31m", BRIGHT_CYAN = "\033[1;36m", NORMAL_CROSSED = "\033[0;9;37m", RED_BACKGROUND = "\033[1;41m", NORMAL_FAINT = "\033[0;2m";
#define dbg(x) cout << BRIGHT_CYAN << #x << COLOR_RESET << " = " << (x) << NORMAL_FAINT << " (L" << __LINE__ << ") " << __FILE__ << COLOR_RESET << endl
#define dbgif(cond, x) ((cond) ? cout << BRIGHT_CYAN << #x << COLOR_RESET << " = " << (x) << NORMAL_FAINT << " (L" << __LINE__ << ") " << __FILE__ << COLOR_RESET << endl : cerr)
#define atcoder_finalanswer(x) 0
#define local_finalanswer(x) cout << (x) << endl
#else
using BS = bitset<130>;
#define dbg(x) (x)
#define dbgif(cond, x) 0
#define atcoder_finalanswer(x) cout << (x) << endl
#define local_finalanswer(x) 0
#endif


uint32_t rand_int() // XorShift random integer generator
{
    static uint32_t x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    uint32_t t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}
double rand_double() { return (double)rand_int() / UINT32_MAX; }


vector<vector<char>> geoinfo;
constexpr short sINF = 10000;
short geodist_(int, int, int xt, int yt) {
    if (geoinfo[xt][yt] == '#') return sINF;
    return geoinfo[xt][yt] - '0';
}

template <typename T_E, T_E INF, T_E (*edge_cost)(int, int, int, int)> struct GridGraph {
    const int H, W;
    const std::vector<int> dx{1, -1, 0, 0};
    const std::vector<int> dy{0, 0, 1, -1};

    GridGraph() = default;
    GridGraph(int h, int w) : H(h), W(w) {}

    // Dijkstra's algorithm
    // Complexity: O(HWlog(HW))
    std::vector<std::vector<T_E>> dist;                // Distance from (x_s, y_s)
    std::vector<std::vector<std::pair<short, short>>> prv; // Previous node for Dijkstra optimal path
    void dijkstra(int x_s, int y_s) {
        dist.assign(H, std::vector<T_E>(W, INF));
        prv.assign(H, std::vector<std::pair<short, short>>(W, std::make_pair(-1, -1)));

        using P = std::tuple<T_E, int, int>;
        std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
        dist[x_s][y_s] = 0;
        pq.emplace(0, x_s, y_s);
        while (!pq.empty()) {
            T_E dnow;
            int x, y;
            std::tie(dnow, x, y) = pq.top();
            pq.pop();
            if (dist[x][y] < dnow) continue;
            for (unsigned d = 0; d < dx.size(); d++) {
                int xn = x + dx[d], yn = y + dy[d];
                if (xn < 0 or yn < 0 or xn >= H or yn >= W) continue;
                T_E dnxt = dnow + edge_cost(x, y, xn, yn);
                if (dnxt < dist[xn][yn]) {
                    dist[xn][yn] = dnxt;
                    prv[xn][yn] = std::make_pair(x, y);
                    pq.emplace(dnxt, xn, yn);
                }
            }
        }
    }
};


using ps = pair<short, short>;

template <typename T> std::vector<std::vector<T>> trans(std::vector<std::vector<T>> v) {
    int H = v.size(), W = v[0].size();
    std::vector<std::vector<T>> ret(W, std::vector<T>(H));
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) ret[j][i] = v[i][j];
    }
    return ret;
}

struct Region {
    short xL, xR, yL, yR;
    Region() = default;
    Region(int xl, int xr, int yl, int yr) : xL(xl), xR(xr), yL(yl), yR(yr) {}
    template <class OStream> friend OStream &operator<<(OStream &os, const Region &r) {
        os << '[' << r.xL << ',' << r.xR << "]x[" << r.yL << ',' << r.yR << ']';
        return os;
    }
    bool is_point() const { return xL == xR and yL == yR; }
    bool is_horizontal_line() const { return !is_point() and xL == xR; }
    bool is_vertical_line() const { return !is_point() and yL == yR; }
    Region trans90() const { return Region{yL, yR, xL, xR}; }
    bool is_inner(int x, int y) const { return xL <= x and x <= xR and yL <= y and y <= yR; }
};

vector<Region> read_horizontal_lines(const vector<vector<char>> &S) {
    vector<Region> ret;
    const int H = S.size(), W = S[0].size();
    REP(i, H) {
        REP(j, W - 1) {
            if (S[i][j] != '#' and S[i][j + 1] != '#') {
                if (j and S[i][j - 1] != '#') continue;
                int k = j;
                while (k + 1 < W and S[i][k + 1] != '#') k++;
                ret.push_back(Region{i, i, j, k});
            }
        }
    }
    return ret;
}

vector<Region> read_vertical_lines(const vector<vector<char>> &S) {
    auto ret = read_horizontal_lines(trans(S));
    for (auto &region : ret) region = region.trans90();
    return ret;
}

Region origin;
vector<Region> regions;

constexpr int NMAX = 69;
vector<short> intersecting_region_ids[NMAX][NMAX]; // [i][j], (i, j) と交差する rid の list
vector<vector<short>> sssp_dist[NMAX][NMAX]; // [i][j][x][y], (i, j) から (x, y) までの距離
vector<vector<pair<short, short>>> sssp_prev[NMAX][NMAX]; // [i][j][x][y] 点 (i, j) から (x, y) まで最短距離で行った時，(x, y) の直前に踏んだ点
vector<short> nearest_region_dist[NMAX][NMAX];  // [i][j][r], 点 (i, j) から領域 r の任意の一点までの最短距離
BS region_belonging_to[NMAX][NMAX];
vector<ps> nearest_region_point[NMAX][NMAX]; // [i][j][r], 点 (i, j) から領域 r の任意の一点まで最短距離で行った時の到着地点

vector<vector<pair<short, ps>>> rid2intersectingrids;

pair<bool, ps> region_intersects(const Region &a, const Region &b) {
    int xl = max(a.xL, b.xL);
    int xr = min(a.xR, b.xR);
    int yl = max(a.yL, b.yL);
    int yr = min(a.yR, b.yR);
    if (xl == xr and yl == yr) {
        return {true, {xl, yl}};
    } else {
        return {false, {-1, -1}};
    }
}


struct Tour {
    int N;
    short start_x, start_y;
    vector<short> landmark_rids; // 必ずこの順で訪れる region の列
    vector<ps> representatives; // region で特に訪れる代表点
    vector<short> move_costs;

    int nb_of_landmarks() const { return landmark_rids.size(); }

    void recalculate_move_costs() {
        move_costs.clear();
        REP(i, int(representatives.size()) - 1) {
            auto [sx, sy] = representatives[i];
            auto [x, y] = representatives[i + 1];
            move_costs.push_back(sssp_dist[sx][sy][x][y]);
        }
    }
private:
    void range_reverse(int l, int r) {
        reverse(landmark_rids.begin() + l, landmark_rids.begin() + r);
        reverse(representatives.begin() + l, representatives.begin() + r);
    }

    void erase(int pos) {
        landmark_rids.erase(landmark_rids.begin() + pos);
        representatives.erase(representatives.begin() + pos);
        recalculate_move_costs();
    }
    void range_erase(int l, int r) {
        landmark_rids.erase(landmark_rids.begin() + l, landmark_rids.begin() + r);
        representatives.erase(representatives.begin() + l, representatives.begin() + r);
        recalculate_move_costs();
    }

public:
    Tour(int n, Region origin) : N(n), start_x(origin.xL), start_y(origin.yL) {
        assert(origin.is_point());
        landmark_rids = {0, 0};
        representatives = {{origin.xL, origin.yL}, {origin.xL, origin.yL}};
        move_costs = {0};
    }

    pair<int, int> find_farthest_unvisited_region() const { // 寄り道するとして最も遠い region の rid を返す もっとまともに計算しようとすればできる
        vector<short> dist(regions.size(), sINF);

        REP(m, int(representatives.size()) - 1) {
            short nowx, nowy;
            tie(nowx, nowy) = representatives[m + 1];
            auto upd = [&]() { REP(r, regions.size()) chmin(dist[r], nearest_region_dist[nowx][nowy][r]); };
            const auto [x, y] = representatives[m];
            upd();
            while (nowx != x or nowy != y) {
                tie(nowx, nowy) = sssp_prev[x][y][nowx][nowy];
                upd();
            }
        }
        int ret = max_element(dist.begin(), dist.end()) - dist.begin();
        return make_pair(ret, dist[ret]);
    }


    void add_landmark(int r) { // region r を必ず訪れるように変更
        recalculate_move_costs();
        short best_ins_pos = -1;
        short best_ins_cost = sINF;
        ps best, bestl, bestr;

        REP(ileft, move_costs.size()) {
            int iright = ileft + 1;
            int rLeft = landmark_rids[ileft], rRight = landmark_rids[iright];
            short default_cost = move_costs[ileft] + (ileft ? move_costs[ileft - 1] : 0);
            for (auto [rtmp1, xyLeft] : rid2intersectingrids[rLeft]) {
                for (auto [rtmp2, xyRight] : rid2intersectingrids[rRight]) {
                    short base_cost = 0;
                    if (ileft) {
                        auto [xpre, ypre] = representatives[ileft - 1];
                        base_cost += sssp_dist[xpre][ypre][xyLeft.first][xyLeft.second];
                    }
                    if (iright + 1 < int(representatives.size())) {
                        auto [xnxt, ynxt] = representatives[iright + 1];
                        base_cost += sssp_dist[xyRight.first][xyRight.second][xnxt][ynxt];
                    }

                    FOR(x, regions[r].xL, regions[r].xR + 1) {
                        FOR(y, regions[r].yL, regions[r].yR + 1) {
                            short now_cost = base_cost;
                            now_cost += sssp_dist[xyLeft.first][xyLeft.second][x][y];
                            now_cost += sssp_dist[x][y][xyRight.first][xyRight.second];

                            if (chmin<short>(best_ins_cost, now_cost - default_cost)) {
                                best_ins_pos = ileft;
                                best = {x, y};
                                bestl = xyLeft;
                                bestr = xyRight;
                            }
                        }
                    }
                }
            }
        }
        assert(best_ins_pos >= 0);
        representatives[best_ins_pos] = bestl;
        representatives[best_ins_pos + 1] = bestr;
        landmark_rids.insert(landmark_rids.begin() + best_ins_pos + 1, r);
        representatives.insert(representatives.begin() + best_ins_pos + 1, best);
        recalculate_move_costs();
    }

    bool try_to_erase_landmark() { // 不要な landmark を消せるか検討
        if (landmark_rids.size() <= 2) return false;
        recalculate_move_costs();

        BS all;
        REP(r, regions.size()) all.set(r);

        vector<BS> unvisited_vs;
        auto gen_unvisited_mask = [&](int fromx, int fromy, int tox, int toy) -> BS {
            BS mask = all;
            short nowx = tox, nowy = toy;

            auto set = [&]() { mask &= ~region_belonging_to[nowx][nowy]; };
            set();
            while (nowx != fromx or nowy != fromy) {
                auto [prvx, prvy] = sssp_prev[fromx][fromy][nowx][nowy];
                nowx = prvx, nowy = prvy;
                set();
            }
            return mask;
        };

        REP(i, landmark_rids.size() - 1) {
            auto [fromx, fromy] = representatives[i];
            auto [tox, toy] = representatives[i + 1];
            unvisited_vs.push_back(gen_unvisited_mask(fromx, fromy, tox, toy));
        }

        int cand_pos = rand_int() % (landmark_rids.size() - 2) + 1;

        auto mm = gen_unvisited_mask(
            representatives[cand_pos - 1].first, representatives[cand_pos - 1].second,
            representatives[cand_pos + 1].first, representatives[cand_pos + 1].second
            );
        REP(i, cand_pos - 1) mm &= unvisited_vs[i];
        FOR(i, cand_pos + 1, unvisited_vs.size()) mm &= unvisited_vs[i];
        if (mm.none()) {
            erase(cand_pos);
            return true;
        } else {
            return false;
        }
    }

    ps gen_range(short maxwidth) {
        if (landmark_rids.size() <= 2) return {-1, -1};
        assert(maxwidth >= 1);
        int width = rand_int() % maxwidth + 1;
        int l = rand_int() % (landmark_rids.size() - 2) + 1;
        int r = l + width;
        return {l, r};
    }

    int gen_rand_index() {
        assert(landmark_rids.size() > 2);
        return rand_int() % (landmark_rids.size() - 2) + 1;
    }

    void random_range_reverse() {
        if (landmark_rids.size() <= 2) return;
        int l = gen_rand_index(), r = gen_rand_index();
        if (l > r) swap(l, r);

        if (r <= l + 10) return;
        range_reverse(l, r);
        if (r + 2 < int(landmark_rids.size())) erase(r + 1);
        erase(r);
        erase(l);
        if (l - 1) erase(l - 1);
        satisfy();
    }

    void random_erase() {
        if (landmark_rids.size() <= 2) return;
        erase(gen_rand_index());
    }
    void random_range_erase(int maxwidth) {
        auto [l, r] = gen_range(maxwidth);
        if (r + 1 < int(landmark_rids.size())) {
            range_erase(l, r);
        }
    }

    string retrieve_path_str() const {
        string ret;
        IREP(i, landmark_rids.size() - 1) {
            auto [nowx, nowy] = representatives[i + 1];
            auto [goalx, goaly] = representatives[i];
            while (nowx != goalx or nowy != goaly) {
                auto [prvx, prvy] = sssp_prev[goalx][goaly][nowx][nowy];
                if (nowx + 1 == prvx) ret += 'U';
                if (nowx - 1 == prvx) ret += 'D';
                if (nowy + 1 == prvy) ret += 'L';
                if (nowy - 1 == prvy) ret += 'R';
                nowx = prvx, nowy = prvy;
            }
        }
        reverse(ret.begin(), ret.end());
        return ret;
    }

    int calc_score() {
        recalculate_move_costs();
        return round(1e4 + 1e7 * N / accumulate(move_costs.begin(), move_costs.end(), 0.0));
    }

    void satisfy() {
        while (true) {
            auto [rid, rdist] = find_farthest_unvisited_region();
            if (rdist == 0) break;
            add_landmark(rid);
        }
    }
};



int main() {
    auto START = std::chrono::system_clock::now();
    int N;
    pint pstart;
    cin >> N >> pstart.first >> pstart.second;
    geoinfo.assign(N, vector<char>(N, '_'));
    cin >> geoinfo;

    origin = Region(pstart.first, pstart.first, pstart.second, pstart.second);

    {
        regions = {origin};
        auto horizontal_regions = read_horizontal_lines(geoinfo);
        regions.insert(regions.end(), horizontal_regions.begin(), horizontal_regions.end());
        auto vertical_regions = read_vertical_lines(geoinfo);
        regions.insert(regions.end(), vertical_regions.begin(), vertical_regions.end());
    }

    GridGraph<short, sINF, geodist_> ssspsolver(N, N);
    REP(i, N) REP(j, N) if (geoinfo[i][j] != '#') {
        region_belonging_to[i][j].reset();
        REP(rid, regions.size()) if (regions[rid].is_inner(i, j)) intersecting_region_ids[i][j].push_back(rid), region_belonging_to[i][j].set(rid);
        ssspsolver.dijkstra(i, j);
        sssp_dist[i][j] = ssspsolver.dist;
        sssp_prev[i][j] = ssspsolver.prv;

        nearest_region_dist[i][j].assign(regions.size(), sINF);
        nearest_region_point[i][j].assign(regions.size(), {-1, -1});
        REP(r, regions.size()) {
            FOR(x, regions[r].xL, regions[r].xR + 1) FOR(y, regions[r].yL, regions[r].yR + 1) {
                if (chmin(nearest_region_dist[i][j][r], sssp_dist[i][j][x][y])) nearest_region_point[i][j][r] = {x, y};
            }
        }
    }
    rid2intersectingrids.resize(regions.size());
    REP(r, regions.size()) {
        REP(r2, regions.size()) {
            auto [is_cross, pt] = region_intersects(regions[r], regions[r2]);
            if (is_cross) rid2intersectingrids[r].emplace_back(r2, pt);
        }
    }
    Tour tour(N, origin);
    tour.satisfy();

    int iter = 0;
    while (true) {
        iter++;
        int64_t spent_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - START).count();
        if (spent_ms >= 2900) break;
        int choice = rand_int() % 10;
        if (choice < 4) {
            auto tour_cand = tour;
            tour_cand.random_erase();
            tour_cand.satisfy();
            if (tour.calc_score() < tour_cand.calc_score()) tour = tour_cand;
        } else if (choice < 9) {
            auto tour_cand = tour;
            tour_cand.random_range_erase(10);
            tour_cand.satisfy();
            if (tour.calc_score() < tour_cand.calc_score()) tour = tour_cand;
        } else if (spent_ms >= 2000) {
            auto tour_cand = tour;
            tour_cand.random_range_reverse();
            if (tour.calc_score() < tour_cand.calc_score()) tour = tour_cand;
        }
    }

    local_finalanswer(tour.calc_score());
    local_finalanswer(iter);
    dbg(tour.retrieve_path_str());
    dbg(iter);
    cerr << iter << endl;
    atcoder_finalanswer(tour.retrieve_path_str());
}
