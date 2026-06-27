#include <bits/stdc++.h>
using namespace std;

#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")

#define rep(i,n) for(int i=0; (i)<(n); ++(i))
#define repo(i,o,n) for(int i=(o); (i)<(n); ++(i))

constexpr int MAX_N = 200;
constexpr int MAX_WIDTH = 10000;
constexpr int MAX_HEIGHT = 10000;
constexpr int MIN_VARIABILITY = 2264;
constexpr int MAX_VARIABILITY = 5764;
int VARIABILITY = 3526;
const double ALLOWABLE_ERROR = 1e-5;

enum class Direction { U, D, L, R };
std::ostream& operator << (std::ostream& os, const Direction& dir) {
    if (dir == Direction::U) os << "U";
    else if (dir == Direction::D) os << "D";
    else if (dir == Direction::L) os << "L";
    else if (dir == Direction::R) os << "R";
    return os;
}

inline bool is_horizon(Direction dir) { return static_cast<int>(dir)&2; }
inline bool is_vertical(Direction dir) { return !is_horizon(dir);}

namespace Random {
    using uint = unsigned int;

    uint seed[4] = {1,2,3,4};

    inline void init_rand_seed(uint s) {
        const uint magic_number = 1812433253;
        for(int i=1; i<=4; i++){
            seed[i-1] = s = magic_number * (s^(s>>30U)) + (uint)i;
        }
    }

    inline uint rand_uint() {
        uint t = (seed[0]^(seed[0]<<11U));
        seed[0]=seed[1];
        seed[1]=seed[2];
        seed[2]=seed[3];
        return seed[3]=(seed[3]^(seed[3]>>19U))^(t^(t>>8U));
    }

    inline double rand_prob() {
        return (double)rand_uint() / (double)UINT_MAX;
    }

    inline uint rand_range(uint r) {
        return rand_uint()%r;
    }

    inline int rand_range(int l, int r) {
        return rand_uint() % (r-l) + l;
    }

    inline Direction rand_dir() {
        return static_cast<Direction>(rand_range(4));
    }
}

void random_90_turn(Direction &dir) {
    if (is_horizon(dir)) dir = Random::rand_range(2) ? Direction::U : Direction::D;
    else dir = Random::rand_range(2) ? Direction::L : Direction::R;
}

class stopwatch {
    std::chrono::system_clock::time_point m_start;
public:

    stopwatch();

    void clear();
    bool is_started();
    void start();
    unsigned long get_sec();
    unsigned long get_ms();
    unsigned long get_us();
    void print_sec();
    void print_ms();
    void print_us();
};

class timeManager : public stopwatch {
    unsigned long ms_time_limit;
public:
    explicit timeManager(unsigned long _ms_time_limit);
    bool is_within_time_limit();
};

class SimulatedAnnealing {
    const unsigned int ms_time_limit;
    const double START_TEMP;
    const double END_TEMP;
    timeManager tm;
    [[nodiscard]] double temperature();
    [[nodiscard]] double probability(double diff, double t) const;

protected:
    double progress();
    bool permit_transition(double diff);
    bool permit_continuation();

public:
    struct Parameters {
        unsigned int ms_time_limit;
        double START_TEMP;
        double END_TEMP;

        Parameters() : ms_time_limit(0), START_TEMP(1), END_TEMP(1) {}
    };
    explicit SimulatedAnnealing(Parameters params)
            : ms_time_limit(params.ms_time_limit)
            , START_TEMP(params.START_TEMP)
            , END_TEMP(params.END_TEMP)
            , tm(params.ms_time_limit) {}
};

struct Vec2 {
    int x, y;

    inline Vec2 operator -() const {
        return{ -x, -y };
    }
    inline Vec2 operator +(const Vec2& other) const {
        return{ x + other.x, y + other.y };
    }
    inline Vec2 operator -(const Vec2& other) const {
        return{ x - other.x, y - other.y };
    }
    inline Vec2 operator *(int s) const {
        return{ x*s, y*s };
    }

    inline bool operator<( const Vec2& another ) const {
        return x == another.x ? y < another.y : x < another.x;
    }
    inline bool operator==( const Vec2& another ) const {
        return x == another.x && y == another.y;
    }
    inline bool operator!=( const Vec2& another ) const {
        return x != another.x || y != another.y;
    }

    Vec2() : x(0), y(0) {};
    Vec2(int _x, int _y) {x = _x, y = _y; }

};
std::ostream &operator<<(std::ostream &out, const Vec2 &v) {
    out << '(' << v.x << "," << v.y << ')';
    return out;
}

inline bool is_within_safe_area(Vec2 v) {
    return 0 <= v.x && v.x <= MAX_WIDTH && 0 <= v.y && v.y <= MAX_HEIGHT;
}

class Rect {
public:
    Vec2 target;
    int desired_area;
    Vec2 tl, br;

    Rect(Vec2 _target, int _desired_area) : target(_target), desired_area(_desired_area), tl(_target), br(_target + Vec2(1,1)) {}
    Rect() : target(), desired_area(), tl(), br() {}

    void validate_value() const {
        if (tl.x >= br.x || tl.y >= br.y || !is_within_safe_area(tl) || !is_within_safe_area(br)) {
            stringstream ss;
            ss << "[Rect]: this rectangle is invalid.\n";
            ss << "tl:(" << tl.x << ", " << tl.y << ")\n";
            ss << "br:(" << br.x << ", " << br.y << ")\n";
            ss << "target:(" << target.x << ", " << target.y << ")\n";
            ss << "desired_area:" << desired_area << endl;
            cerr << ss.str() << endl;
            exit(-1);
        }
    }
    void output() const {
        printf("%d %d %d %d\n", tl.x, tl.y, br.x, br.y);
    }
    void detailed_output() const {
        stringstream ss;
        ss << "tl:(" << tl.x << ", " << tl.y << ")\n";
        ss << "br:(" << br.x << ", " << br.y << ")\n";
        ss << "target:(" << target.x << ", " << target.y << ")\n";
        ss << "desired_area:" << desired_area << endl;
        ss << "diff area: " << d_area() << endl;
        ss << "loss:" << scientific << setprecision(2) << loss() << endl;
        cout << ss.str() << endl;
    }

//    bool contain_target() const {
//        return tl.x <= target.x && target.x < br.x && tl.y <= target.y && target.y < br.y;
//    }

    int best_diff(Direction dir) const {
        if (dir == Direction::U) return tl.y - max(0, min(target.y, (int)round(br.y - (double)desired_area / (double)width())));
        else if (dir == Direction::D) return min(MAX_HEIGHT, max(target.y+1, (int)round(tl.y + (double)desired_area / (double)width()))) - br.y;
        else if (dir == Direction::L) return tl.x - max(0, min(target.x, (int)round(br.x - (double)desired_area / (double)height())));
        else return min(MAX_WIDTH, max(target.x+1, (int)round(tl.x + (double)desired_area / (double)height()))) - br.x;
    }
    int space_from_target(Direction dir) const {
        if (dir == Direction::U) return target.y - tl.y;
        else if (dir == Direction::D) return br.y - target.y - 1;
        else if (dir == Direction::L) return target.x - tl.x;
        else return br.x - target.x - 1;
    }
    int width() const {
        return br.x - tl.x;
    }
    int height() const {
        return br.y - tl.y;
    }
    int area() const {
        return width() * height();
    }
    int d_area() const {
        return area() - desired_area;
    }

    double center_x() const {
        return width()/2. + tl.x;
    }
    double center_y() const {
        return height()/2. + tl.y;
    }

    double loss() const {
        int r = desired_area;
        int s = area();
        double tmp = 1 - (double)min(r,s) / (double)max(r,s);
        return tmp*tmp;
    }
    bool check_duplicate(Rect another) const {
        return (another.br.x <= tl.x || br.x <= another.tl.x || another.br.y <= tl.y || br.y <= another.tl.y) ? false : ((tl.x <= another.tl.x && another.tl.x < br.x) || (another.tl.x <= tl.x && tl.x < another.br.x)) && ((tl.y <= another.tl.y && another.tl.y < br.y) || (another.tl.y <= tl.y && tl.y < another.br.y));
    }
    void scaling(int d, Direction dir) {
        if (dir == Direction::U) tl.y -= d;
        else if (dir == Direction::D) br.y += d;
        else if (dir == Direction::L) tl.x -= d;
        else br.x += d;
    }
    void resize_to_avoid_duplicate(Rect another, Direction dir) {
        if (dir == Direction::U) tl.y = another.br.y;
        else if (dir == Direction::D) br.y = another.tl.y;
        else if (dir == Direction::L) tl.x = another.br.x;
        else br.x = another.tl.x;
    }
    bool can_resize_to_avoid_duplicate(Rect another, Direction dir) {
        if (dir == Direction::U) return another.br.y <= target.y;
        else if (dir == Direction::D) return target.y < another.tl.y;
        else if (dir == Direction::L) return another.br.x <= target.x;
        else return target.x < another.tl.x;
    }
    double diff_loss_of_resize_to_avoid_duplicate(Rect another, Direction dir) {
        if (can_resize_to_avoid_duplicate(another, dir)) {
            Rect tmp = *this;
            double now_loss = tmp.loss();
            tmp.resize_to_avoid_duplicate(another, dir);
            return tmp.loss() - now_loss;
        } else return INFINITY;
    }
    friend std::ostream &operator<<(std::ostream &out, const Rect &r) {
        out << '(' << r.tl.x << ", " << r.tl.y << ")-(" << r.br.x << ", " << r.br.y << ')';
    }
};

inline bool check_duplicate(Rect r1, Rect r2) {
    return r1.check_duplicate(r2);
}

int n;
Rect ads[MAX_N];

inline int calc_space(int idx, Direction dir) {
    Rect rect = ads[idx];
    if (dir == Direction::U) {
        rect.tl.y = 0;
        rep(i,n) if (i != idx && rect.check_duplicate(ads[i])) rect.tl.y = ads[i].br.y;
    }
    else if (dir == Direction::D) {
        rect.br.y = MAX_HEIGHT;
        rep(i,n) if (i != idx && rect.check_duplicate(ads[i])) rect.br.y = ads[i].tl.y;
    }
    else if (dir == Direction::L) {
        rect.tl.x = 0;
        rep(i,n) if (i != idx && rect.check_duplicate(ads[i])) rect.tl.x = ads[i].br.x;
    }
    else {
        rect.br.x = MAX_WIDTH;
        rep(i,n) if (i != idx && rect.check_duplicate(ads[i])) rect.br.x = ads[i].tl.x;
    }
    if (is_horizon(dir)) return rect.width() - ads[idx].width();
    else return rect.height() - ads[idx].height();
}

class Solver : public SimulatedAnnealing {
    void just_right_scaling_without_duplicate(int idx, Direction dir) {
        // かぶらないようにいい感じに拡大

        int best_diff = ads[idx].best_diff(dir);
        int l_space = max(-VARIABILITY, -ads[idx].space_from_target(dir));
        int r_space = min(VARIABILITY, calc_space(idx, dir) + 1);

        l_space = max(l_space, min(r_space-1, best_diff));
        ads[idx].scaling(l_space, dir);
    }
    void random_scaling_without_duplicate(int idx, Direction dir) {
        int l_space = -ads[idx].space_from_target(dir);
        int r_space = calc_space(idx, dir) + 1;

        ads[idx].scaling(Random::rand_range(l_space, r_space), dir);
    }
public:
    explicit Solver(Parameters params)
            : SimulatedAnnealing(params) {}

    void solve() {
        Rect best_ads[MAX_N];
        rep(i,n) best_ads[i] = ads[i];
        double now_loss=0;
        rep(i,n) now_loss += ads[i].loss();
        double now_min_loss = now_loss;
        pair<int, Rect> origin[MAX_N];
        int origin_size = 0;
        priority_queue<pair<double, int>> pq;
        while (permit_continuation()) {
            VARIABILITY = MAX_VARIABILITY + (MIN_VARIABILITY - MAX_VARIABILITY) * progress();
            rep(idx, n) {
                origin_size = 0;
                origin[origin_size++] = make_pair(idx, ads[idx]);
                double origin_loss = ads[idx].loss();

                Direction dir = Random::rand_dir();
                // ある方向の長さを変え、その方向に垂直な方向にいい感じに調整し、かぶったところを調整
                random_scaling_without_duplicate(idx, dir);
                random_90_turn(dir);

                int best_diff = ads[idx].best_diff(dir);
                int l_space = max(-VARIABILITY, -ads[idx].space_from_target(dir));
                int r_space = max(l_space+1, min(best_diff+1, VARIABILITY));
                l_space = max(l_space, min(r_space-1, min(0,best_diff)));
                ads[idx].scaling(Random::rand_range(l_space, r_space), dir);

                if (ads[idx].loss() - origin_loss > 0.1) {
                    ads[idx] = origin[0].second;
                    continue;
                }

                double d_loss = 0;
                rep(i,n) if (i != idx && ads[idx].check_duplicate(ads[i])) {
                    {
                        Direction best_dir;
                        double now_min_loss = INFINITY;
                        rep(j,4) {
                            Direction dir = static_cast<Direction>(j);
                            double diff_loss = ads[i].diff_loss_of_resize_to_avoid_duplicate(ads[idx], dir);
                            if (now_min_loss > diff_loss) {
                                best_dir = dir;
                                now_min_loss = diff_loss;
                            }
                        }
                        if (isinf(now_min_loss)) {
                            rep(j,origin_size) ads[origin[j].first] = origin[j].second;
                            origin_size = 1;
                            d_loss = 0;
                            break;
                        }
                        origin[origin_size++] = make_pair(i, ads[i]);
                        ads[i].resize_to_avoid_duplicate(ads[idx], best_dir);
                        d_loss += now_min_loss;
                    }
                    double tmp = ads[i].loss();
                    int offset = Random::rand_range(4);
                    rep(j,4) {
                        Direction dir = static_cast<Direction>((offset+j)&3);
                        just_right_scaling_without_duplicate(i, dir);
                    }
                    d_loss += ads[i].loss() - tmp;
                }
                int offset = Random::rand_range(4);
                rep(i,4) {
                    Direction dir = static_cast<Direction>((offset+i)&3);
                    just_right_scaling_without_duplicate(idx, dir);
                }
                d_loss += ads[idx].loss() - origin_loss;

                if (permit_transition(d_loss)) {
                    now_loss += d_loss;
                    if (now_min_loss > now_loss) {
                        now_min_loss = now_loss;
                        rep(i,n) best_ads[i] = ads[i];
                    }
                } else rep(j,origin_size) ads[origin[j].first] = origin[j].second;
            }
        }
        rep(i,n) ads[i] = best_ads[i];
    }
};

void input() {
    scanf("%d", &n);
    rep(i,n) {
        int x,y,r;
        scanf("%d %d %d", &x, &y, &r);
        ads[i] = {Vec2(x,y), r};
    }
}
void dump_info() {
    rep(i,n) ads[i].detailed_output();
}
void check() {
    rep(i,n) ads[i].validate_value();
    rep(i,n) repo(j,i+1,n) if (check_duplicate(ads[i], ads[j])) {
                cerr << "duplicated!" << endl;
                ads[i].detailed_output();
                ads[j].detailed_output();
                exit(-1);
            }
}
void output() {
    rep(i,n) ads[i].output();
}


int main(int argc, char *argv[]) {
    stopwatch sw; sw.start();

    Random::init_rand_seed(42);
    input();

    Solver::Parameters params;
    params.START_TEMP = 0.0587958;
    params.END_TEMP   = 1e-11;
    params.ms_time_limit = 4970;

    Solver solver(params);

    {
        vector<pair<int,int>> v(n);
        const int loop_count = 2500000 / (n*n);
        rep(th, loop_count) {
            // かぶらないようにいい感じに拡大
            int variability=th;
            rep(i,n) v[i] = make_pair(-ads[i].loss(),i);
            sort(v.begin(), v.end());

            rep(i,n) {
                int idx = v[i].second;
                int offset = Random::rand_range(4);
                rep(j,4) {
                    Direction dir = static_cast<Direction>((j+offset)&3);
                    int best_diff = ads[idx].best_diff(dir);
                    int l_space = max(-variability, -ads[idx].space_from_target(dir));
                    int r_space = min(variability, calc_space(idx, dir) + 1);

                    l_space = max(l_space, min(r_space-1, best_diff));
                    ads[idx].scaling(l_space, dir);
                }
            }
        }
    }

    solver.solve();

    rep(i,n) {
        int offset = Random::rand_range(4);
        rep(j,4) {
            Direction dir = static_cast<Direction>((offset+j)&3);
            int best_diff = ads[i].best_diff(dir);
            int l_space = -ads[i].space_from_target(dir);
            int r_space = calc_space(i, dir) + 1;

            l_space = max(l_space, min(r_space-1, best_diff));
            ads[i].scaling(l_space, dir);
        }
    }

    output();
}


inline stopwatch::stopwatch() : m_start(std::chrono::system_clock::time_point::min()) {}

inline void stopwatch::clear() {
    m_start = std::chrono::system_clock::time_point::min();
}

inline bool stopwatch::is_started() {
    return (m_start.time_since_epoch() != std::chrono::system_clock::time_point::min().time_since_epoch());
}

inline void stopwatch::start() {
    m_start = std::chrono::system_clock::now();
}

inline unsigned long stopwatch::get_sec() {
    if (is_started()) {
        std::chrono::system_clock::duration  diff;
        diff = std::chrono::system_clock::now() - m_start;
        return (unsigned)(std::chrono::duration_cast<std::chrono::seconds>(diff).count());
    }
    return 0;
}

inline unsigned long stopwatch::get_ms() {
    if (is_started()) {
        std::chrono::system_clock::duration  diff;
        diff = std::chrono::system_clock::now() - m_start;
        return (unsigned)(std::chrono::duration_cast<std::chrono::milliseconds>(diff).count());
    }
    return 0;
}

inline unsigned long stopwatch::get_us() {
    if (is_started()) {
        std::chrono::system_clock::duration  diff;
        diff = std::chrono::system_clock::now() - m_start;
        return (unsigned)(std::chrono::duration_cast<std::chrono::microseconds>(diff).count());
    }
    return 0;
}

inline void stopwatch::print_sec() {
    fprintf(stderr, "%lu [sec]\n", get_sec());
}

inline void stopwatch::print_ms() {
    fprintf(stderr, "%lu [ms]\n", get_ms());
}

inline void stopwatch::print_us() {
    fprintf(stderr, "%lu [us]\n", get_us());
}

inline timeManager::timeManager(unsigned long _ms_time_limit) : ms_time_limit(_ms_time_limit) {
    start();
}

inline bool timeManager::is_within_time_limit() {
    return get_ms() < ms_time_limit;
}

inline bool SimulatedAnnealing::permit_continuation() {
    return tm.is_within_time_limit();
}

inline double SimulatedAnnealing::progress() {
    return (double)tm.get_ms() / (double)ms_time_limit;
}

inline double SimulatedAnnealing::temperature() {
    return START_TEMP + (END_TEMP - START_TEMP) * progress();
}

inline double SimulatedAnnealing::probability(double diff, double t) const {
    if (diff <= 0) return 1;
    else return exp(-diff/t);
}

inline bool SimulatedAnnealing::permit_transition(double diff) {
    return probability(diff, temperature()) > Random::rand_prob();
}

