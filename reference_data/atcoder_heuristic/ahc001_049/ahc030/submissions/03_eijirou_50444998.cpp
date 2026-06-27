// Submit with C++ 23 (Clang)

#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#ifdef ONLINE_JUDGE
#define NDEBUG
#endif // ONLINE_JUDGE


// 17.4.1.2 Headers

// C
#ifndef _GLIBCXX_NO_ASSERT
#include <cassert>
#endif
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#if __cplusplus >= 201103L
#include <ccomplex>
#include <cfenv>
#include <cinttypes>
//#include <cstdalign>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <cwchar>
#include <cwctype>
#endif

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#if __cplusplus >= 201103L
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#endif

#include <bit>

// #include <atcoder/all>

#ifndef ONLINE_JUDGE
#if !defined(__clang__) && defined(__GNUC__)
#include <omp.h>
#endif // GCC
#endif // ONLINE_JUDGE

using namespace std;
// using namespace atcoder;

#define ll long long int
#define ull unsigned long long int

template<class T>
inline bool chmax(T& a, T b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}

template <class T>
inline T sum(const vector<T>& data) {
    return accumulate(data.begin(), data.end(), static_cast<T>(0));
}

class Xorshift {
    public:
        explicit Xorshift(uint32_t seed): x_(seed) {
            assert(seed);
        }

        // [0, stop)
        uint32_t randrange(uint32_t stop) {
            assert(stop > 0);
            next();
            return x_ % stop;
        }

        // [start, stop)
        uint32_t randrange(uint32_t start, uint32_t stop) {
            assert(start < stop);
            next();
            return start + x_ % (stop - start);
        }

        // [a, b]
        uint32_t randint(uint32_t a, uint32_t b) {
            assert(a <= b);
            return randrange(a, b + 1);
        }

        // [0.0, 1.0]
        double random() {
            next();
            return static_cast<double>(x_) * (1.0 / static_cast<double>(UINT32_MAX));
        }

        // [a, b] or [b, a]
        double uniform(double a, double b) {
            return a + (b - a) * random();
        }

        uint32_t raw() {
            next();
            return x_;
        }

    private:
        uint32_t x_;

        void next() {
            x_ ^= x_ << 13;
            x_ ^= x_ >> 17;
            x_ ^= x_ << 5;
        }
};

class Timer {
    public:
        Timer() {
            begin();
            elapsed_time_ = 0.0;
        }

        void begin() {
            start_time_ = chrono::system_clock::now();
        }

        double get_time() {
            chrono::system_clock::time_point end_time = chrono::system_clock::now();
            elapsed_time_ = chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time_).count();
            elapsed_time_ *= 1e-9; // nanoseconds -> seconds
            return elapsed_time_;
        }

        double get_last_time() const {
            return elapsed_time_;
        }

        bool yet(double time_limit) {
            return get_time() < time_limit;
        }

        double progress(double time_limit) {
            return get_time() / time_limit;
        }

    private:
        chrono::system_clock::time_point start_time_;
        double elapsed_time_;
};

constexpr int sa_time_counts = 1 << 4;
constexpr size_t sa_random_steps = 1ul << 10;

class SimulatedAnnealing {
    public:
        explicit SimulatedAnnealing(double t_first, double t_last, bool exponential, double time_limit, uint_fast32_t seed)
            : t_first_(t_first), t_last_(t_last), exponential_(exponential), time_limit_(time_limit) {
            assert(0.0 <= t_last_ && t_last_ <= t_first_);

            time_counter_ = sa_time_counts;
            temperature_ = t_first_;
            log2_random_index_ = 0;

            for (size_t i = 0; i < sa_random_steps; ++i) {
                log2_random_[i] = log2(static_cast<double>(i + 1) / static_cast<double>(sa_random_steps));
            }
            mt19937 rng(seed);
            shuffle(log2_random_.begin(), log2_random_.end(), rng);
        }

        template<class T>
        bool accept(T profit, const Timer& timer) {
            if (profit >= static_cast<T>(0)) {
                return true;
            } else {
                return static_cast<double>(profit) > get_threshold(timer);
            }
        }

        double get_threshold(const Timer& timer) {
            update_temperature(timer);
            ++log2_random_index_;
            log2_random_index_ &= (sa_random_steps - 1ul);
            return temperature_ * log2_random_[log2_random_index_];
        }

    private:
        double t_first_, t_last_;
        bool exponential_;
        double time_limit_;
        int time_counter_;
        double temperature_;
        size_t log2_random_index_;
        array<double,sa_random_steps> log2_random_;

        void update_temperature(const Timer& timer) {
            if (time_counter_-- == 0) {
                time_counter_ = sa_time_counts;
                double progress = timer.get_last_time() / time_limit_;
                if (exponential_) {
                    temperature_ = pow(t_first_, 1.0 - progress) * pow(t_last_, progress);
                } else {
                    temperature_ = t_first_ * (1.0 - progress) + t_last_ * progress;
                }
            }
        }
};

class BitVector {
    public:
        explicit BitVector() {
            n_ = 0;
        }

        explicit BitVector(int n) {
            n_ = n;
            block_.assign(n_ / 64 + 1, 0);
        }

        explicit BitVector(int n, const vector<int>& v) {
            n_ = n;
            block_.assign(n_ / 64 + 1, 0);

            for (int i : v) {
                set(i);
            }
        }

        bool operator<(const BitVector& other) const {
            assert(n_ == other.n_);
            return block_ < other.block_;
        }

        bool operator==(const BitVector& other) const {
            assert(n_ == other.n_);
            return block_ == other.block_;
        }

        void operator&=(const BitVector& other) {
            assert(n_ == other.n_);
            for (size_t i = 0; i < block_.size(); ++i) {
                block_[i] &= other.block_[i];
            }
        }

        void operator|=(const BitVector& other) {
            assert(n_ == other.n_);
            for (size_t i = 0; i < block_.size(); ++i) {
                block_[i] |= other.block_[i];
            }
        }

        BitVector operator&(const BitVector& other) const {
            assert(n_ == other.n_);
            BitVector ret(n_);
            for (size_t i = 0; i < block_.size(); ++i) {
                ret.block_[i] = block_[i] & other.block_[i];
            }
            return ret;
        }

        BitVector operator|(const BitVector& other) const {
            assert(n_ == other.n_);
            BitVector ret(n_);
            for (size_t i = 0; i < block_.size(); ++i) {
                ret.block_[i] = block_[i] | other.block_[i];
            }
            return ret;
        }

        void set(int i) {
            assert(0 <= i && i < n_);
            block_[i / 64] |= 1ULL << (i % 64);
        }

        void reset(int i) {
            assert(0 <= i && i < n_);
            block_[i / 64] &= ~(1ULL << (i % 64));
        }

        bool get(int i) const {
            assert(0 <= i && i < n_);
            return (block_[i / 64] >> (i % 64)) & 1ULL;
        }

        void clear() {
            fill(block_.begin(), block_.end(), 0ULL);
        }

        int size() const {
            return n_;
        }

        int count() const {
            int ret = 0;
            for (uint64_t x : block_) {
                ret += popcount(x);
            }
            return ret;
        }

        vector<int> to_vector() const {
            vector<int> ret;
            int offset = 0;
            for (uint64_t x : block_) {
                while (x) {
                    int i = countr_zero(x);
                    ret.push_back(i + offset);
                    x ^= 1ULL << i;
                }
                offset += 64;
            }
            return ret;
        }

    private:
        int n_;
        vector<uint64_t> block_;
};

#ifdef ONLINE_JUDGE
constexpr double kTimeLimit = 2.95;
#else
constexpr double kTimeLimit = 4.95;
#endif // ONLINE_JUDGE

struct Polyomino {
    vector<int> blocks;
    int x_size, y_size;

    Polyomino(const vector<int>& blocks, int n): blocks(blocks) {
        x_size = 0;
        y_size = 0;
        for (int p : blocks) {
            chmax(x_size, p / n + 1);
            chmax(y_size, p % n + 1);
        }
    }
};

inline vector<uint32_t> make_polyomino_grid(const Polyomino& oil, int n) {
    vector<uint32_t> ret(oil.x_size, 0);
    for (int p : oil.blocks) {
        ret[p / n] |= 1 << (p % n);
    }
    return ret;
}

inline int count_overlappings(int dx, int dy, const vector<uint32_t>& grid_a, const vector<uint32_t>& grid_b) {
    int x_max = min((int)grid_b.size(), (int)grid_a.size() - dx);
    int ret = 0;
    for (int x = max(0, -dx); x < x_max; ++x) {
        if (dy < 0) {
            ret += popcount(grid_a[x + dx] & (grid_b[x] >> -dy));
        } else {
            ret += popcount((grid_a[x + dx] >> dy) & grid_b[x]);
        }
    }
    return ret;
}

// move `b` to maximize the number of overlapping squares between `a` and `b`
inline vector<pair<int,int>> calc_best_overlapping(const Polyomino& a, const Polyomino& b, int n) {
    vector<uint32_t> grid_a = make_polyomino_grid(a, n);
    vector<uint32_t> grid_b = make_polyomino_grid(b, n);
    vector<tuple<int,int,int>> overlappings;
    overlappings.reserve((a.x_size + b.x_size - 1) * (a.y_size + b.y_size - 1));
    for (int dx = -b.x_size + 1; dx < a.x_size; ++dx) {
        for (int dy = -b.y_size + 1; dy < a.y_size; ++dy) {
            overlappings.push_back({count_overlappings(dx, dy, grid_a, grid_b), dx, dy});
        }
    }
    sort(overlappings.begin(), overlappings.end(),
        [](const tuple<int,int,int>& a, tuple<int,int,int>& b) {
            return get<0>(a) > get<0>(b);
        }
    );
    vector<pair<int,int>> ret;
    auto [cnt0, dx0, dy0] = overlappings[0];
    ret.push_back({dx0, dy0});
    for (size_t i = 1; i < overlappings.size(); ++i) {
        auto [cnt, dx, dy] = overlappings[i];
        if (cnt == cnt0) {
            ret.push_back({dx, dy});
        } else {
            break;
        }
    }
    return ret;
}

inline double calc_mu(int k, int vs, double eps) {
    return (double)(k - vs) * eps + (double)vs * (1.0 - eps);
}

inline double calc_sigma(int k, double eps) {
    return sqrt(k * eps * (1.0 - eps));
}

struct IO {
    bool online_judge;

    int n, m;
    double eps;
    vector<Polyomino> oil_shapes;

    double cost;
    int turn;
    int num_blocks;
    vector<double> mu_data;
    vector<double> sigma_data;
    vector<double> sqrt_data;
    array<int,4> dxy;
    vector<vector<pair<int,int>>> best_overlapping;
    vector<BitVector> bit_oils;

    vector<int> oil_pos;
    vector<int> oil_reserves;
    BitVector ans;
    vector<double> err;

    ofstream out;

    void input() {
        online_judge = true;

        cin >> n >> m >> eps;

        oil_shapes.reserve(m);
        for (int i = 0; i < m; ++i) {
            int num_blocks;
            cin >> num_blocks;
            vector<int> blocks(num_blocks);
            for (int j = 0; j < num_blocks; ++j) {
                int x, y;
                cin >> x >> y;
                blocks[j] = n * x + y;
            }
            oil_shapes.emplace_back(Polyomino(blocks, n));
        }
        init();
    }

    void input(const string& input_filename, const string& output_filename) {
        online_judge = false;

        ifstream in(input_filename);
        assert(in);

        out = ofstream(output_filename);
        assert(out);

        in >> n >> m >> eps;

        oil_shapes.reserve(m);
        for (int i = 0; i < m; ++i) {
            int num_blocks;
            in >> num_blocks;
            vector<int> blocks(num_blocks);
            for (int j = 0; j < num_blocks; ++j) {
                int x, y;
                in >> x >> y;
                blocks[j] = n * x + y;
            }
            oil_shapes.emplace_back(Polyomino(blocks, n));
        }
        init();

        oil_pos.resize(m);
        for (int i = 0; i < m; ++i) {
            int x, y;
            in >> x >> y;
            oil_pos[i] = n * x + y;
        }
        oil_reserves.resize(n * n);
        ans = BitVector(n * n);
        for (int p = 0; p < n * n; ++p) {
            in >> oil_reserves[p];
            if (oil_reserves[p] > 0) {
                ans.set(p);
            }
        }
        err.resize(2 * n * n);
        for (int i = 0; i < 2 * n * n; ++i) {
            in >> err[i];
        }

        in.close();
    }

    void init() {
        cost = 0.0;
        turn = 0;
        dxy = {-n, n, -1, 1};

        num_blocks = 0;
        for (const Polyomino& oil : oil_shapes) {
            num_blocks += oil.blocks.size();
        }
        mu_data.resize((num_blocks + 1) * (n * n + 1));
        for (int k = 0; k <= n * n; ++k) {
            for (int vs = 0; vs <= num_blocks; ++vs) {
                mu_data[(num_blocks + 1) * k + vs] = calc_mu(k, vs, eps);
            }
        }
        sigma_data.resize(n * n + 1);
        for (int k = 0; k <= n * n; ++k) {
            sigma_data[k] = calc_sigma(k, eps);
        }
        sqrt_data.resize(n * n + 1);
        for (int k = 0; k <= n * n; ++k) {
            sqrt_data[k] = sqrt(k);
        }
        best_overlapping.assign(m * m, {});
        for (int i = 0; i < m; ++i) {
            for (int j = i + 1; j < m; ++j) {
                best_overlapping[m * i + j] = calc_best_overlapping(oil_shapes[i], oil_shapes[j], n);
                best_overlapping[m * j + i].resize(best_overlapping[m * i + j].size());
                for (size_t k = 0; k < best_overlapping[m * i + j].size(); ++k) {
                    auto [dx, dy] = best_overlapping[m * i + j][k];
                    best_overlapping[m * j + i][k] = {-dx, -dy};
                }
            }
        }
        bit_oils.assign(n * n * m, BitVector());
        for (int i = 0; i < m; ++i) {
            for (int x = 0; x <= n - oil_shapes[i].x_size; ++x) {
                for (int y = 0; y <= n - oil_shapes[i].y_size; ++y) {
                    BitVector bv(n * n);
                    int p = n * x + y;
                    for (int d : oil_shapes[i].blocks) {
                        bv.set(p + d);
                    }
                    bit_oils[n * n * i + p] = bv;
                }
            }
        }
    }

    double get_mu(int k, int vs) const {
        assert(2 <= k && k <= n * n && 0 <= vs && vs <= num_blocks);
        return mu_data[(num_blocks + 1) * k + vs];
    }

    double get_sigma(int k) const {
        assert(2 <= k && k <= n * n);
        return sigma_data[k];
    }

    double get_sqrt(int k) const {
        assert(0 <= k && k <= n * n);
        return sqrt_data[k];
    }

    pair<int,int> get_best_overlapping(int i, int j, Xorshift& rng) const {
        assert(0 <= i && i < m);
        assert(0 <= j && j < m);
        const vector<pair<int,int>> best_overlapping_i_j = best_overlapping[m * i + j];
        return best_overlapping_i_j[rng.randrange(best_overlapping_i_j.size())];
    }

    const BitVector& get_bitvector(int i, int p) const {
        return bit_oils[n * n * i + p];
    }

    int drill_one_square(int p) {
        assert(0 <= p && p < n * n);
        ++turn;
        assert(turn <= 2 * n * n);
        cost += 1.0;

        if (online_judge) {
            cout << "q 1 " << p / n << " " << p % n << endl;
            int ret;
            cin >> ret;
            return ret;
        } else {
            int ret = oil_reserves[p];
            write_comment("Response: " + to_string(ret));
            out << "q 1 " << p / n << " " << p % n << endl;
            return ret;
        }
    }

    int drill_multiple_square(const BitVector& s) {
        int k = s.count();
        assert(k >= 2);
        ++turn;
        assert(turn <= 2 * n * n);
        cost += 1.0 / sqrt(k);

        if (online_judge) {
            cout << "q " << k;
            for (int p : s.to_vector()) {
                cout << " " << p / n << " " << p % n;
            }
            cout << endl;

            int ret;
            cin >> ret;
            return ret;
        } else {
            int vs = 0;
            for (int p : s.to_vector()) {
                vs += oil_reserves[p];
            }
            double mu = get_mu(k, vs);
            double sigma = get_sigma(k);
            int ret = max(0, (int)lround(mu + err[turn - 1] * sigma));
            write_comment("Response: " + to_string(ret));
            out << "q " << k;
            for (int p : s.to_vector()) {
                out << " " << p / n << " " << p % n;
            }
            out << endl;
            return ret;
        }
    }

    bool guess(const BitVector& s) {
        int k = s.count();
        ++turn;
        assert(turn <= 2 * n * n);

        if (online_judge) {
            cout << "a " << k;
            for (int p : s.to_vector()) {
                cout << " " << p / n << " " << p % n;
            }
            cout << endl;

            int res;
            cin >> res;
            if (res) {
                return true;
            } else {
                cost += 1.0;
                return false;
            }
        } else {
            out << "a " << k;
            for (int p : s.to_vector()) {
                out << " " << p / n << " " << p % n;
            }
            out << endl;

            if (s == ans) {
                return true;
            } else {
                cost += 1.0;
                return false;
            }
        }
    }

    void write_comment(const string& comment) {
        if (!online_judge) {
            out << "# " << comment << "\n";
        }
    }

    void write_comment(const BitVector& s, const string& color) {
        if (!online_judge) {
            for (int p : s.to_vector()) {
                out << "#c " << p / n << " " << p % n << " " << color << "\n";
            }
        }
    }
};

constexpr double kSqrt2 = 1.41421356237309514547;

inline double calc_norm_cdf(double mu, double sigma, double x) {
    return 0.5 * (1.0 + erf((x - mu) / (kSqrt2 * sigma)));
}

inline double calc_pr(double mu, double sigma, int vs) {
    if (vs == 0) {
        return calc_norm_cdf(mu, sigma, (double)vs + 0.5);
    }
    return calc_norm_cdf(mu, sigma, (double)vs + 0.5) - calc_norm_cdf(mu, sigma, (double)vs - 0.5);
}

constexpr double kLogLogPrThreshold = -200.0;

inline double calc_fix_log_pr(double pr) {
    double log_pr = (pr <= 0.0) ? DBL_MIN_EXP : log2(pr);
    if (log_pr < kLogLogPrThreshold) {
        return kLogLogPrThreshold;// - log(kLogLogPrThreshold - log_pr + 1.0);
    } else {
        return log_pr;
    }
}

constexpr ll kPrResolutionLL = 1LL << 32;
constexpr double kPrResolution = kPrResolutionLL;
constexpr double kOilAcceptedPrDecay = 0.8;

struct History {
    Xorshift rng;
    int n, m, nn;
    double eps;
    vector<vector<ll>> query_log2_pr; // query_pr[query_id][sum_of_oil_reserves] = log2(probability)
    vector<vector<pair<int,int>>> related_queries; // related_queries[n * n * oil_id + position] = {{query_id, related_squares}}
    set<BitVector> failed_estimations;
    vector<ll> oil_accepted_pr;

    History(): rng(1) {}

    History(const IO& io): rng(1), n(io.n), m(io.m), nn(n * n), eps(io.eps) {
        related_queries.assign(n * n * m, {});
        oil_accepted_pr.assign(m, kPrResolutionLL);
    }

    void push_query(const BitVector& s, int vs, const IO& io) {
        int query = query_log2_pr.size();

        for (int i = 0; i < m; ++i) {
            const Polyomino& oil = io.oil_shapes[i];
            for (int x = 0; x <= n - oil.x_size; ++x) {
                for (int y = 0; y <= n - oil.y_size; ++y) {
                    int p = n * x + y;
                    int cnt = (io.get_bitvector(i, p) & s).count();
                    if (cnt) {
                        related_queries[nn * i + p].push_back({query, cnt});
                    }
                }
            }
        }
        int k = s.count();

        vector<ll> log2_pr(io.num_blocks + 1);
        if (k == 1) {
            for (int ws = 0; ws <= io.num_blocks; ++ws) {
                log2_pr[ws] = (ws == vs) ? 0.0 : calc_fix_log_pr(0.0) * kPrResolution;
            }
        } else {
            double sigma = io.get_sigma(k);
            for (int ws = 0; ws <= io.num_blocks; ++ws) {
                double mu = io.get_mu(k, ws);
                double pr = calc_pr(mu, sigma, vs);
                log2_pr[ws] = calc_fix_log_pr(pr) * kPrResolution;
            }
        }
        query_log2_pr.emplace_back(log2_pr);
    }

    const vector<pair<int,int>>& get_related_queries(int i, int p) const {
        return related_queries[nn * i + p];
    }

    void push_fail(const BitVector& s) {
        failed_estimations.insert(s);
    }

    bool failed(const BitVector& s) const {
        return failed_estimations.contains(s);
    }

    int choose_pos(int i, const IO& io) {
        const Polyomino& oil = io.oil_shapes[i];
        return n * rng.randrange(n - oil.x_size + 1) + rng.randrange(n - oil.y_size + 1);
    }

    int choose_oil() {
        int num_trials = m / 4;
        if (num_trials == 0) {
            return rng.randrange(m);
        }
        int ret = 0;
        ll max_profit = LLONG_MIN;
        for (int trial = 0; trial <= num_trials; ++trial) {
            int i = rng.randrange(m);
            if (chmax(max_profit, oil_accepted_pr[i] + (ll)rng.raw())) {
                ret = i;
            }
        }
        return ret;
    }

    void push_oil_result(int i, bool accepted) {
        oil_accepted_pr[i] *= 8;
        oil_accepted_pr[i] /= 10;
        if (accepted) {
            oil_accepted_pr[i] += 2 * kPrResolutionLL / 10;
        }
    }
};

constexpr int kMoveOilPr = 4; // 0.75
constexpr int kMoveNeighborPr = 4; // 0.75

struct State {
    Xorshift rng;
    int n;
    vector<int> config;
    vector<int> query_counts;
    ll log2_pr;
    vector<int> count_delta;
    BitVector updating;
    ll avg_log2_pr;

    State(const vector<int>& config, const IO& io, const History& history, uint32_t seed) :
        rng(seed), config(config)
    {
        n = io.n;
        query_counts.assign(history.query_log2_pr.size(), 0);
        for (int i = 0; i < io.m; ++i) {
            increase(i, config[i], history);
        }
        log2_pr = 0;
        for (size_t query = 0; query < history.query_log2_pr.size(); ++query) {
            log2_pr += history.query_log2_pr[query][query_counts[query]];
        }
        avg_log2_pr = log2_pr;
        count_delta.assign(history.query_log2_pr.size(), 0);
        updating = BitVector(history.query_log2_pr.size());
    }

    void increase(int i, int p, const History& history) {
        for (auto [query, cnt] : history.get_related_queries(i, p)) {
            query_counts[query] += cnt;
        }
    }

    void decrease(int i, int p, const History& history) {
        for (auto [query, cnt] : history.get_related_queries(i, p)) {
            query_counts[query] -= cnt;
        }
    }

    void try_to_increase(int i, int p, const History& history) {
        for (auto [query, cnt] : history.get_related_queries(i, p)) {
            count_delta[query] += cnt;
            updating.set(query);
        }
    }

    void try_to_decrease(int i, int p, const History& history) {
        for (auto [query, cnt] : history.get_related_queries(i, p)) {
            count_delta[query] -= cnt;
            updating.set(query);
        }
    }

    bool can_move(int p, int d, const Polyomino& oil) {
        switch (d) {
            case 0:
                return p / n > 0;
            case 1:
                return p / n < n - oil.x_size;
            case 2:
                return p % n > 0;
            case 3:
                return p % n < n - oil.y_size;
            default:
                unreachable();
        }
    }

    bool move_oil(const IO& io, History& history, const Timer& timer, SimulatedAnnealing& sa) {
        int oil_id = history.choose_oil();
        const Polyomino& oil = io.oil_shapes[oil_id];
        int p = config[oil_id];
        int q;
        uint32_t rng_x = rng.raw();
        if (rng_x % 4) {
            int d = (rng_x >> 2) % 4;
            if (!can_move(p, d, oil)) {
                return false;
            }
            q = p + io.dxy[d];
        } else {
            q = history.choose_pos(oil_id, io);
        }
        try_to_decrease(oil_id, p, history);
        try_to_increase(oil_id, q, history);

        vector<int> updating_queries = updating.to_vector();

        ll log2_pr_delta = 0;
        for (int query : updating_queries) {
            int delta = count_delta[query];
            if (delta == 0) {
                continue;
            }
            int cnt = query_counts[query];
            log2_pr_delta -= history.query_log2_pr[query][cnt];
            log2_pr_delta += history.query_log2_pr[query][cnt + delta];
        }
        if (sa.accept(log2_pr_delta, timer)) {
            history.push_oil_result(oil_id, true);
            config[oil_id] = q;
            for (int query : updating_queries) {
                query_counts[query] += count_delta[query];
                count_delta[query] = 0;
            }
            updating.clear();
            log2_pr += log2_pr_delta;
            avg_log2_pr = (7 * avg_log2_pr + 3 * log2_pr) / 10;
            return true;
        } else {
            history.push_oil_result(oil_id, false);
            for (int query : updating_queries) {
                count_delta[query] = 0;
            }
            updating.clear();
            avg_log2_pr += 75 * avg_log2_pr / 10000;
            return false;
        }
    }

    bool swap_oils(const IO& io, History& history, const Timer& timer, SimulatedAnnealing& sa) {
        int oil_id_1 = history.choose_oil();
        int oil_id_2 = history.choose_oil();
        if (oil_id_1 == oil_id_2) {
            return false;
        }
        const Polyomino& oil1 = io.oil_shapes[oil_id_1];
        const Polyomino& oil2 = io.oil_shapes[oil_id_2];

        int p1 = config[oil_id_1];
        int p2 = config[oil_id_2];

        auto [dx, dy] = io.get_best_overlapping(oil_id_1, oil_id_2, rng);
        int q1 = n * clamp(p2 / n - dx, 0, n - oil1.x_size) + clamp(p2 % n - dy, 0, n - oil1.y_size);
        int q2 = n * clamp(p1 / n + dx, 0, n - oil2.x_size) + clamp(p1 % n + dy, 0, n - oil2.y_size);

        try_to_decrease(oil_id_1, p1, history);
        try_to_decrease(oil_id_2, p2, history);
        try_to_increase(oil_id_1, q1, history);
        try_to_increase(oil_id_2, q2, history);

        vector<int> updating_queries = updating.to_vector();

        ll log2_pr_delta = 0;
        for (int query : updating_queries) {
            int delta = count_delta[query];
            if (delta == 0) {
                continue;
            }
            int cnt = query_counts[query];
            log2_pr_delta -= history.query_log2_pr[query][cnt];
            log2_pr_delta += history.query_log2_pr[query][cnt + delta];
        }
        if (sa.accept(log2_pr_delta, timer)) {
            history.push_oil_result(oil_id_1, true);
            history.push_oil_result(oil_id_2, true);
            config[oil_id_1] = q1;
            config[oil_id_2] = q2;
            for (int query : updating_queries) {
                query_counts[query] += count_delta[query];
                count_delta[query] = 0;
            }
            updating.clear();
            log2_pr += log2_pr_delta;
            avg_log2_pr = (7 * avg_log2_pr + 3 * log2_pr) / 10;
            return true;
        } else {
            history.push_oil_result(oil_id_1, false);
            history.push_oil_result(oil_id_2, false);
            for (int query : updating_queries) {
                count_delta[query] = 0;
            }
            updating.clear();
            avg_log2_pr += 75 * avg_log2_pr / 10000;
            return false;
        }
    }

    bool update(const IO& io, History& history, const Timer& timer, SimulatedAnnealing& sa) {
        if (rng.randrange(kMoveOilPr)) {
            return move_oil(io, history, timer, sa);
        } else {
            return swap_oils(io, history, timer, sa);
        }
    }
};

struct Candidate {
    ll log2_pr;
    vector<int> config;

    Candidate(ll log2_pr, const vector<int>& config): log2_pr(log2_pr), config(config) {}

    bool operator<(const Candidate& other) const {
        return log2_pr > other.log2_pr;
    }
};

struct Selector {
    int num_candidates;
    priority_queue<Candidate> candidates;
    set<vector<int>> configurations;

    Selector(int num_candidates): num_candidates(num_candidates) {}

    bool can_push(ll log2_pr, const vector<int>& config) const {
        if ((int)candidates.size() == num_candidates && log2_pr <= candidates.top().log2_pr) {
            return false;
        } else if (configurations.contains(config)) {
            return false;
        } else {
            return true;
        }
    }

    void push(ll log2_pr, const vector<int>& config) {
        if ((int)candidates.size() == num_candidates) {
            configurations.erase(candidates.top().config);
            candidates.pop();
        }
        candidates.push(Candidate(log2_pr, config));
        configurations.insert(config);
    }

    vector<Candidate> to_vector() {
        vector<Candidate> ret;
        ret.reserve(candidates.size());
        while (!candidates.empty()) {
            ret.push_back(candidates.top());
            candidates.pop();
        }
        reverse(ret.begin(), ret.end());
        return ret;
    }
};

inline BitVector config_to_bitvector(const vector<int>& config, const IO& io) {
    BitVector s(io.n * io.n);
    for (int i = 0; i < io.m; ++i) {
        s |= io.get_bitvector(i, config[i]);
    }
    return s;
}

inline vector<int> config_to_blocks(const vector<int>& config, const IO& io) {
    vector<int> ret;
    ret.reserve(io.num_blocks);
    for (int i = 0; i < io.m; ++i) {
        for (int d : io.oil_shapes[i].blocks) {
            ret.push_back(config[i] + d);
        }
    }
    return ret;
}

constexpr int kNumCandidates = 32;
constexpr int kNumStates = 8;
constexpr double kBestReserve = 4;
constexpr double kT0 = 6.0 * kPrResolution;
constexpr double kT1 = 0.6 * kPrResolution;
constexpr double kWantedAcceptanceRatio = 0.04;
constexpr double kIncreaseT = 1.05;
constexpr double kBreakPr = 0.001;
constexpr double kIgnorePrThreshold = 1e-4;
constexpr int kDifficultThreshold = 83;

inline int calc_difficulty(const IO& io) {
    return 2 * (io.n + io.m) + (int)(100 * io.eps);
}

struct Solver {
    Timer timer;
    IO io;

    int n, m;
    double eps;
    History history;
    int difficulty;
    bool difficult;
    double guess_threshold;

    Solver() {
        io.input();
        init();
    }

    Solver(const string& input_filename, const string& output_filename) {
        io.input(input_filename, output_filename);
        init();
    }

    void init() {
        n = io.n;
        m = io.m;
        eps = io.eps;
        history = History(io);
        difficulty = calc_difficulty(io);
        difficult = difficulty >= kDifficultThreshold;
        guess_threshold = 1.0 - pow(0.2, (double)(100 - difficulty) / 80.0) * pow(8e-3, (double)(difficulty - 20) / 80.0);
    }

    void drill(const BitVector& s) {
        int vs = (s.count() == 1) ? io.drill_one_square(s.to_vector()[0]) : io.drill_multiple_square(s);
        history.push_query(s, vs, io);
    }

    bool guess(const BitVector& s) {
        if (io.guess(s)) {
            return true;
        } else {
            history.push_fail(s);
            return false;
        }
    }

    void first_rough_drill() {
        vector<tuple<int,int,int,int>> sections = {
            {0, n, 0, n / 2},
            {0, n / 2, 0, n}
        };
        if (m >= 5) {
            sections.push_back({0, n / 2, 0, n / 2});
        }
        for (auto [x_min, x_max, y_min, y_max] : sections) {
            BitVector s(n * n);
            for (int x = x_min; x < x_max; ++x) {
                for (int y = y_min; y < y_max; ++y) {
                    s.set(n * x + y);
                }
            }
            drill(s);
        }
    }

    void first_fine_drill() {
        int section_num = get_section_num();
        int x_min = 0;
        for (int section_x = 0; section_x < section_num; ++section_x) {
            int x_max = x_min + n / section_num;
            if (section_x < n % section_num) {
                ++x_max;
            }
            int y_min = 0;
            for (int section_y = 0; section_y < section_num; ++section_y) {
                int y_max = y_min + n / section_num;
                if (section_y < n % section_num) {
                    ++y_max;
                }
                BitVector s(n * n);
                for (int x = x_min; x < x_max; ++x) {
                    for (int y = y_min; y < y_max; ++y) {
                        s.set(n * x + y);
                    }
                }
                drill(s);
                y_min = y_max;
            }
            x_min = x_max;
        }
    }

    int get_section_num() const {
        return n / lround(4.6 - 10.0 * eps);
    }

    void first_drill() {
        if (!difficult) {
            first_rough_drill();
        } else {
            first_fine_drill();
        }
    }

    vector<int> init_config(uint32_t seed) const {
        Xorshift rng(seed + 1);
        vector<int> config(m);
        for (int i = 0; i < m; ++i) {
            config[i] = n * rng.randrange(n - io.oil_shapes[i].x_size + 1) + rng.randrange(n - io.oil_shapes[i].y_size + 1);
        }
        return config;
    }

    double calc_new_ans_pr_ex(int k, const vector<int>& vs, const vector<double>& candidates_pr) const {
        assert(1 <= k && k <= n * n);
        assert(vs.size() == candidates_pr.size());
        if (k == 1) {
            vector<double> sum_pr(m + 1, 0.0);
            for (size_t i = 0; i < candidates_pr.size(); ++i) {
                sum_pr[vs[i]] += candidates_pr[i];
            }
            double ret = 0.0;
            for (size_t i = 0; i < candidates_pr.size(); ++i) {
                ret += candidates_pr[i] * candidates_pr[i] / sum_pr[vs[i]];
            }
            return ret;
        }
        double sigma = io.get_sigma(k);
        double ret = 0.0;
        vector<double> res_pr(candidates_pr.size(), 0.0);
        vector<double> prev_cdf(candidates_pr.size(), 0.0);
        int res = 0;
        while (true) {
            double sum_pr = 0.0;
            bool break_flag = true;
            for (size_t i = 0; i < candidates_pr.size(); ++i) {
                double mu = io.get_mu(k, vs[i]);
                double cdf = calc_norm_cdf(mu, sigma, (double)res + 0.5);
                double pr = cdf - prev_cdf[i];
                sum_pr += pr;
                res_pr[i] = pr;
                prev_cdf[i] = cdf;
                if (vs[i] > res) {
                    break_flag = false;
                }
            }
            if (sum_pr < kBreakPr) {
                if (break_flag) {
                    break;
                } else {
                    ++res;
                    continue;
                }
            }
            for (size_t i = 0; i < res_pr.size(); ++i) {
                ret += res_pr[i] * res_pr[i] / sum_pr;
            }
            ++res;
        }
        return ret;
    }

    double ans_pr_ex_to_profit(double cur_ans_pr_ex, double new_ans_pr_ex, int k) const {
        if (new_ans_pr_ex <= cur_ans_pr_ex) {
            return 0.0;
        } else {
            return (difficult ? 1.0 : io.get_sqrt(k)) * log2(new_ans_pr_ex / cur_ans_pr_ex);
        }
    }

    pair<bool,BitVector> decide_drill_squares(const vector<Candidate>& candidates) const {
        assert(!candidates.empty());
        BitVector best_bitvector = config_to_bitvector(candidates[0].config, io);
        if (candidates.size() == 1) {
            return {true, best_bitvector};
        }
        // calculate probabilities
        vector<double> candidates_pr(candidates.size());
        for (size_t i = 0; i < candidates.size(); ++i) {
            double pr = pow(2, (double)(candidates[i].log2_pr - candidates[0].log2_pr) / kPrResolution);
            candidates_pr[i] = pr;
        }
        double sum_pr = sum(candidates_pr);

        while (candidates_pr.back() / sum_pr < kIgnorePrThreshold) {
            sum_pr -= candidates_pr.back();
            candidates_pr.pop_back();
        }

        double best_bitvector_pr = 0.0;
        double cur_ans_pr_ex = 0.0;
        vector<double> v_ex(n * n, 0.0);
        vector<vector<int>> v(candidates_pr.size(), vector<int>(n * n, 0));
        for (size_t i = 0; i < candidates_pr.size(); ++i) {
            candidates_pr[i] /= sum_pr;
            double pr = candidates_pr[i];
            vector<int> blocks = config_to_blocks(candidates[i].config, io);
            BitVector s(n * n, blocks);
            if (s == best_bitvector) {
                best_bitvector_pr += pr;
            }
            cur_ans_pr_ex += pr * pr;
            for (int p : blocks) {
                v_ex[p] += pr;
                ++v[i][p];
            }
        }
        if (best_bitvector_pr >= guess_threshold) {
            return {true, best_bitvector};
        }

        vector<int> vs(candidates_pr.size(), 0);
        BitVector s(n * n);
        
        auto add_square = [&](int p) {
            assert(!s.get(p));
            s.set(p);
            for (size_t i = 0; i < v.size(); ++i) {
                vs[i] += v[i][p];
            }
        };

        auto remove_square = [&](int p) {
            assert(s.get(p));
            s.reset(p);
            for (size_t i = 0; i < v.size(); ++i) {
                vs[i] -= v[i][p];
            }
        };

        BitVector best_s(n * n);
        double max_profit = -numeric_limits<double>::infinity();

        vector<pair<double,int>> pos_candidates;
        for (int p = 0; p < n * n; ++p) {
            double profit = (double)v[0][p] - v_ex[p];
            if (profit > 0.0) {
                pos_candidates.push_back({profit, p});
                add_square(p);
            }
        }
        if (pos_candidates.empty()) {
            // rarely
            return {true, best_bitvector};
        }
        int k_all = s.count();
        double new_ans_pr_ex_all = calc_new_ans_pr_ex(k_all, vs, candidates_pr);
        double profit_all = ans_pr_ex_to_profit(cur_ans_pr_ex, new_ans_pr_ex_all, k_all);
        if (chmax(max_profit, profit_all)) {
            best_s = s;
        }
        sort(pos_candidates.begin(), pos_candidates.end());

        for (auto [_, p] : pos_candidates) {
            if (!s.get(p)) {
                continue;
            }
            remove_square(p);
            int k = s.count();
            if (k == 0) {
                add_square(p);
                continue;
            }
            double new_ans_pr_ex = calc_new_ans_pr_ex(k, vs, candidates_pr);
            double profit = ans_pr_ex_to_profit(cur_ans_pr_ex, new_ans_pr_ex, k);
            if (chmax(max_profit, profit)) {
                best_s = s;
            } else {
                add_square(p);
            }
        }
        for (auto [_, p] : pos_candidates) {
            if (s.get(p)) {
                continue;
            }
            add_square(p);
            int k = s.count();
            double new_ans_pr_ex = calc_new_ans_pr_ex(k, vs, candidates_pr);
            double profit = ans_pr_ex_to_profit(cur_ans_pr_ex, new_ans_pr_ex, k);
            if (chmax(max_profit, profit)) {
                best_s = s;
            } else {
                remove_square(p);
            }
        }

        return {false, best_s};
    }

    double calc_local_time_limit(double decide_drill_time, int decide_drill_cnt) {
        double remaining_time = kTimeLimit - timer.get_time();
        double ret = difficult ?
            remaining_time / (min(2 * n * n, 8 * difficulty) - io.turn) :
            1.5 * remaining_time / (min(2 * n * n, (30 + (int)(100 * eps)) * m) - io.turn);
        if (decide_drill_cnt) {
            ret = max(0.5 * ret, ret - decide_drill_time / decide_drill_cnt);
        }
        return ret;
    }

    void solve() {
        first_drill();
        
        vector<State> states;
        states.reserve(kNumStates);

        Xorshift rng(1);

        for (uint32_t seed = 0; seed < kNumStates; ++seed) {
            states.emplace_back(State(init_config(seed), io, history, seed + 1));
        }
        BitVector previous_best_bitvector;

        double t0 = kT0;
        double t1 = kT1;
        double decide_drill_time = 0.0;
        int decide_drill_cnt = 0;

        while (io.turn < 2 * n * n) {
            Timer local_timer;
            double local_time_limit = calc_local_time_limit(decide_drill_time, decide_drill_cnt);

            SimulatedAnnealing sa(t0, t1, true, local_time_limit, 0);
            Selector selector(kNumCandidates);

            for (State& state : states) {
                if (selector.can_push(state.log2_pr, state.config)) {
                    if (history.failed_estimations.empty() || !history.failed(config_to_bitvector(state.config, io))) {
                        selector.push(state.log2_pr, state.config);
                    }
                }
            }

            int sa_trials = 0;
            int sa_accept = 0;
            while (local_timer.yet(local_time_limit) || selector.candidates.empty()) {
                sa_trials += states.size();
                for (State& state : states) {
                    if (state.update(io, history, local_timer, sa)) {
                        ++sa_accept;
                        if (selector.can_push(state.log2_pr, state.config)) {
                            if (!history.failed_estimations.empty() && history.failed(config_to_bitvector(state.config, io))) {
                                continue;
                            }
                            selector.push(state.log2_pr, state.config);
                        }
                    }
                }
                if (states.size() > 1 && local_timer.progress(local_time_limit) > (double)(kNumStates - (int)states.size() + 1) / (kNumStates + kBestReserve)) {
                    int worst = 0;
                    for (size_t i = 1; i < states.size(); ++i) {
                        if (states[i].avg_log2_pr < states[worst].avg_log2_pr) {
                            worst = i;
                        }
                    }
                    swap(states[worst], states.back());
                    states.pop_back();
                }
#ifndef ONLINE_JUDGE
                if (!timer.yet(kTimeLimit)) {
                    io.cost = 1e4;//1e9;
                    return;
                }
#endif // ONLINE_JUDGE
            }
            double sa_accept_rate = (double)sa_accept / (double)max(1, sa_trials);
            // cerr << io.turn << ": " << sa_accept << " / " << sa_trials << " = " << sa_accept_rate << endl;
            if (sa_accept_rate < kWantedAcceptanceRatio) {
                t0 *= kIncreaseT;
                t1 *= kIncreaseT;
            }
            vector<Candidate> candidates = selector.to_vector();

            if (!io.online_judge) {
                io.write_comment("max_log2_pr: " + to_string(candidates[0].log2_pr));
                io.write_comment("ans_log2_pr: " + to_string(State(io.oil_pos, io, history, 1).log2_pr));
            }
            local_timer.begin();
            auto [convinced, s] = decide_drill_squares(candidates);
            decide_drill_time += local_timer.get_time();
            ++decide_drill_cnt;

            if (convinced) {
                io.write_comment(previous_best_bitvector, "white");
                previous_best_bitvector = BitVector();
                if (guess(s)) {
                    return;
                }
                if (candidates.size() >= 2) {
                    candidates.erase(candidates.begin());
                }
            } else {
                io.write_comment(previous_best_bitvector, "white");
                BitVector best_bitvector = config_to_bitvector(candidates[0].config, io);
                io.write_comment(best_bitvector, "orange");
                previous_best_bitvector = best_bitvector;
                drill(s);
            }
            states.clear();
            for (const Candidate& candidate : candidates) {
                states.emplace_back(State(candidate.config, io, history, rng.randrange(1 << 30) + 1));
            }
            sort(states.begin(), states.end(),
                [](const State& a, const State& b) {
                    return a.log2_pr > b.log2_pr;
                }
            );
            if ((int)states.size() < kNumStates) {
                int lack = kNumStates - (int)states.size();
                for (int i = 0; i < lack; ++i) {
                    states.push_back(states[i]);
                    states.back().rng = Xorshift(rng.randrange(1 << 30) + 1);
                }
            } else {
                states.erase(states.begin() + kNumStates, states.end());
            }
        }

        io.cost = 1e4;//1e9;
    }

    double score() const {
        return io.cost;
    }
};

void multi_test(int cases, int num_threads) {
    if (cases <= 0) {
        return;
    }
    cerr << "cases: " << cases << endl;
    cerr << setprecision(4);

    vector<double> scores(cases);
    vector<double> times(cases);

#ifndef ONLINE_JUDGE
#if !defined(__clang__) && defined(__GNUC__)
    omp_set_num_threads(num_threads);
    #pragma omp parallel for
#endif // GCC
#endif // ONLINE_JUDGE
    for (int seed = 0; seed < cases; ++seed) {
        string filename = to_string(seed);
        filename = string(4 - filename.size(), '0') + filename + ".txt";
 
        Solver solver("in/" + filename, "out/" + filename);
        solver.solve();

        times[seed] = solver.timer.get_time();
        double raw_score = solver.score();
        scores[seed] = log2(raw_score);

        cerr << filename << " " << raw_score << " " << times[seed] << " sec" << endl;
    }
    cerr << "Average log2(Cost): " << accumulate(scores.begin(), scores.end(), 0.0) / cases << endl;
    cerr << "Max Time: " << *max_element(times.begin(), times.end()) << " sec" << endl;
    cerr << "Average Time: " << accumulate(times.begin(), times.end(), 0.0) / cases << " sec" << endl;
}

int main() {
#ifdef ONLINE_JUDGE
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    Solver solver;
    solver.solve();
#else
    int cases = 2000;
    int num_threads = 25;
    multi_test(cases, num_threads);
#endif // ONLINE_JUDGE
    return 0;
}
