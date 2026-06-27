// https://atcoder.jp/contests/ahc002/tasks/ahc002_a

// clang-format off
#pragma GCC optimize("Ofast")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
// clang-format on

#include <bits/stdc++.h>
#include <sys/types.h>
#include <unistd.h>

constexpr int TIMEOUT_MS           = 1980;
constexpr int BACK_LEN_THRESHOLD   = 20;
constexpr int MIDDLE_LEN_THRESHOLD = 20;

using namespace std;

namespace {
    class reverse_range {
      private:
        struct I {
            int x;
            int operator*() { return x - 1; }
            bool operator!=(I& lhs) { return x > lhs.x; }
            void operator++() { --x; }
        };
        I i, n;

      public:
        /**
         * @brief Construct a new reverse range object
         *
         * @param n
         */
        reverse_range(int n) : i({0}), n({n}) {}
        /**
         * @brief Construct a new reverse range object
         *
         * @param i
         * @param n
         */
        reverse_range(int i, int n) : i({i}), n({n}) {}
        /**
         * @brief
         * begin iterator
         * @return I&
         */
        I& begin() { return n; }
        /**
         * @brief
         * end iterator
         * @return I&
         */
        I& end() { return i; }
    };
    /**
     * @brief
     * python みたいな range-based for を実現
     * @details
     * ループの範囲は[bg,ed)なので注意
     * !つけると逆向きにループが回る (reverse_range)
     * 空間計算量はO(1)
     * 使わない変数ができて警告が出がちなので，unused_varとかを使って警告消しするとよい
     */
    class range {
      private:
        struct I {
            int x;
            int operator*() { return x; }
            bool operator!=(I& lhs) { return x < lhs.x; }
            void operator++() { ++x; }
        };
        I i, n;

      public:
        /**
         * @brief Construct a new range object
         *
         * @param n
         */
        range(int n) : i({0}), n({n}) {}
        /**
         * @brief Construct a new range object
         *
         * @param i
         * @param n
         */
        range(int i, int n) : i({i}), n({n}) {}
        /**
         * @brief
         * begin iterator
         * @return I&
         */
        I& begin() { return i; }
        /**
         * @brief
         * end iterator
         * @return I&
         */
        I& end() { return n; }
        /**
         * @brief
         * 逆向きに参照するrange(reverse_rangeを取得できるs)
         * @return reverse_range
         */
        reverse_range operator!() { return reverse_range(*i, *n); }
    };

    const pid_t pid = getpid();
    class XorShift32 {
      private:
        //! XorShiftの現在の値
        unsigned value;

        /**
         * @brief XorShift32のアルゴリズムに基づいて value を更新
         */
        inline void update() {
            value = value ^ (value << 13);
            value = value ^ (value >> 17);
            value = value ^ (value << 5);
        }

        /**
         * @brief 値を更新し，更新前の値を返却
         * @return unsigned 呼び出し時の value を用いる
         */
        inline unsigned get() {
            unsigned v = value;
            update();
            return v;
        }

      public:
        /**
         * @brief [0, 2^bit) の範囲の乱数値を取り出す
         * @tparam デフォルトは31
         * @return int
         */
        template <int bit = 31>
        inline int next_int() {
            return (int)(get() >> (32 - bit));
        }

        /**
         * @brief [-2^bit,2^bit)の範囲の乱数値を取り出す
         * @tparam デフォルトは31
         * @return int
         */
        template <int bit = 31>
        inline int next_signed() {
            unsigned v = get();
            return (int)((v >> (31 - bit)) - (1 << (bit)));
        }

        /**
         * @brief next_int呼び出し時の最大値を取得
         * @tparam 31
         * @return int
         */
        template <int bit = 31>
        inline int range_max() {
            return (int)((1u << bit) - 1);
        };

        /**
         * @brief Construct a new XorShift32 object
         * @param seed
         * @details 初期シードをvalueとするXorShift32のインスタンスを生成
         */
        XorShift32(const unsigned seed) {
            value = seed;
            update();
        }

        XorShift32() : XorShift32(pid) {}
    };

} // namespace

namespace direction {
    constexpr int dr[]  = {-1, 0, 0, 1};
    constexpr int dc[]  = {0, -1, 1, 0};
    constexpr char ds[] = "ULRD";
    inline int opposite(int d) { return 3 - d; }
    char find(int r, int c) {
        assert(r * c == 0);
        assert(r + c == 1 || r + c == -1);
        for (int i = 0; i < 4; i++) {
            if (dr[i] == r && dc[i] == c) {
                return ds[i];
            }
        }
        assert(false);
    }
} // namespace direction


namespace timer {
    using namespace std::chrono;
    std::chrono::high_resolution_clock::time_point bg;
    int64_t cache = 0;
    inline void init() { bg = high_resolution_clock::now(); }
    inline int64_t getCurrentMs() {
        return cache = duration_cast<milliseconds>(high_resolution_clock::now()
                                                   - bg)
                           .count();
    }
} // namespace timer

constexpr int R = 50;
constexpr int C = 50;

int t[50][50];
int v[50][50];

struct Point {
    int r, c;
    Point(int r = 0, int c = 0) : r(r), c(c) {}
    inline bool valid() const { return 0 <= r && r < R && 0 < c && c < C; };
    inline int val() const { return v[r][c]; }
    inline int tile() const { return t[r][c]; }

    inline void go(int d, int step = 1) {
        r += direction::dr[d] * step;
        c += direction::dc[d] * step;
    }
    inline void back(int d, int step = 1) { go(direction::opposite(d), step); }
};

bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.r == rhs.r && lhs.c == rhs.c;
}

ostream& operator<<(ostream& os, const Point p) {
    os << "( " << p.r << ", " << p.c << " )";
    return os;
};

Point s;

void input() {
    cin >> s.r >> s.c;
    for (int i : range(50)) {
        for (int j : range(50)) {
            cin >> t[i][j];
        }
    }
    for (int i : range(50)) {
        for (int j : range(50)) {
            cin >> v[i][j];
        }
    }
}

namespace v1 {
    using BS = bitset<2500>;

    struct Model {
        vector<Point> seq;
        bitset<2500> used;
        int score;
        inline void pushBack(Point p) {
            seq.push_back(p);
            used[p.tile()] = true;
            score += p.val();
        }
        inline void popBack() {
            used[seq.back().tile()] = false;
            score -= seq.back().val();
            seq.pop_back();
        }
        Model() : score(0) {}
        string getString() {
            string s;
            for (int i : range(1, seq.size())) {
                s.push_back(direction::find(seq[i].r - seq[i - 1].r,
                                            seq[i].c - seq[i - 1].c));
            }
            return s;
        }
    };
    namespace random {
        XorShift32 gen;
        array<int, 4> ds() {
            array<int, 4> ds;
            iota(ds.begin(), ds.end(), 0);
            swap(ds[3], ds[gen.next_int() % 4]);
            swap(ds[2], ds[gen.next_int() % 3]);
            swap(ds[1], ds[gen.next_int() % 2]);
            return ds;
        }

        enum {
            BACK   = 100,
            MIDDLE = 5000,
        };
        const vector<int> lbs = {BACK, MIDDLE};
        inline int getMode() {
            return *lower_bound(lbs.begin(), lbs.end(),
                                gen.next_int() % lbs.back());
        }
        inline double getDouble() {
            return gen.next_int() / double(gen.range_max());
        }
    } // namespace random


    void randomWork1(Model& m) {
        for (bool ok = true; ok;) {
            auto& p = m.seq.back();
            ok      = false;
            for (int d : random::ds()) {
                auto q = p;
                q.go(d);
                if (q.valid() && !m.used[q.tile()]) {
                    m.pushBack(q);
                    ok = true;
                    break;
                }
            }
        }
    }

    bool dfs(Point current, Point goal, vector<Point>& paths, int& score,
             BS& usedTile) {
        for (int d : random::ds()) {
            Point nx = current;
            nx.go(d);
            if (nx.valid()) {
                if (nx == goal) {
                    return true;
                }
                if (!usedTile[nx.tile()]) {
                    usedTile[nx.tile()] = true;
                    paths.push_back(nx);
                    score += nx.val();
                    if (dfs(nx, goal, paths, score, usedTile)) {
                        return true;
                    }
                    score -= nx.val();
                    paths.pop_back();
                }
            }
        }
        return false;
    }

    namespace ss {
        constexpr double sTemp = 100;
        constexpr double gTemp = 1;
        int currentMs          = 0;
        inline bool f(int nextScore, int currentScore) {
            if (nextScore > currentScore) return true;
            const double temp =
                sTemp + (gTemp - sTemp) * currentMs / double(TIMEOUT_MS);
            const double p = exp((nextScore - currentScore) / temp);
            return p > random::getDouble();
        }

    } // namespace ss
    Model master;
    int bestScore = 0;

    int updateMiddleTotal = 0;
    int updateMiddleSkip  = 0;
    void updateMiddle() {
        updateMiddleTotal++;
        if (master.seq.size() <= 1u) return;


        const int n = master.seq.size();

        int a = random::gen.next_int() % (n - 1u);
        int b = random::gen.next_int() % n;
        if (a > b) {
            swap(a, b);
        }
        if (a == b) {
            b++;
        }
        if (b > a + MIDDLE_LEN_THRESHOLD) {
            b = a + MIDDLE_LEN_THRESHOLD;
        }

        a++;
        b     = n - b;
        int m = n - a - b;

        Model test = master;
        vector<Point> suffix;
        suffix.reserve(b);

        BS usedTile = test.used;
        int score   = test.score;
        while (b--) {
            suffix.push_back(test.seq.back());
            test.popBack();
        }
        while (m--) {
            auto p             = test.seq.back();
            usedTile[p.tile()] = false;
            score -= p.val();
            test.popBack();
        }
        vector<Point> middle;
        if (!dfs(test.seq.back(), suffix.back(), middle, score, usedTile)) {
            updateMiddleSkip++;
            return;
        }

        if (ss::f(score, master.score)) {
            for (auto& p : middle) {
                test.pushBack(p);
            }
            reverse(suffix.begin(), suffix.end());
            for (auto& p : suffix) {
                test.pushBack(p);
            }
            master = test;
            if (bestScore < master.score) {
                bestScore = master.score;
            }
        }
    }

    int testBackTotal = 0;
    int testBackSkip  = 0;
    void testBack() {
        testBackTotal++;
        Model model = master;
        int n       = model.seq.size();
        if (n > BACK_LEN_THRESHOLD) {
            n = BACK_LEN_THRESHOLD;
        }
        int len = random::gen.next_int() % n;
        while (len--) {
            model.popBack();
        }

        randomWork1(model);
        if (ss::f(model.score, master.score)) {
            master = model;
            if (bestScore < master.score) {
                bestScore = master.score;
            }
        }
        else {
            testBackSkip++;
        }
    }

    void solve() {
        random::gen = XorShift32();
        master.pushBack(s);
        randomWork1(master);


        for (; (ss::currentMs = timer::getCurrentMs()) < 10;) {
            testBack();
        }

        constexpr int x = TIMEOUT_MS * 0.8;
        for (; (ss::currentMs = timer::getCurrentMs()) < x;) {
            for (int i = 0; i < 100; i++) {
                switch (random::getMode()) {
                    case random::BACK: {
                        testBack();
                    } break;
                    case random::MIDDLE: {
                        updateMiddle();
                    } break;
                }
            }
        }
        for (; (ss::currentMs = timer::getCurrentMs()) < TIMEOUT_MS;) {
            switch (random::getMode()) {
                case random::BACK: {
                    testBack();
                } break;
                case random::MIDDLE: {
                    updateMiddle();
                } break;
            }
        }
    }


    void output() {
        cout << master.getString() << endl;


        stringstream ss;
        ss << endl;
        ss << "middle: " << updateMiddleSkip << "/" << updateMiddleTotal
           << endl;
        ss << "back: " << testBackSkip << "/" << testBackTotal << endl;
        ss << "score: " << master.score << endl;
        ss << "best: " << bestScore << endl;
        cerr << ss.str() << endl;
    }
} // namespace v1

int main() {
    /* write here */
    cin.tie(0);
    ios::sync_with_stdio(false);

    timer::init();

    input();

    v1::solve();

    v1::output();
    return 0;
}
