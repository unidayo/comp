#pragma GCC optimize("Ofast")
#include <bits/stdc++.h>
using namespace std;
typedef long long int ll;
typedef unsigned long long int ull;

// --- タイムキーパー --- //
class TimeKeeperDouble {
  private:
    chrono::high_resolution_clock::time_point startTime;
    double timeThreshold = 0;
    double nowTime = 0;

  public:
    explicit TimeKeeperDouble(const double time_threshold) : startTime(chrono::high_resolution_clock::now()), timeThreshold(time_threshold) {}
    void setNowTime() {
        auto diff = chrono::high_resolution_clock::now() - this->startTime;
        this->nowTime = chrono::duration_cast<std::chrono::microseconds>(diff).count() * 1e-3; // ms
    }
    double getNowTime() const { return this->nowTime; }
    [[nodiscard]] bool isTimeOver() const { return nowTime >= timeThreshold; }
};

// --- 乱数クラス --- //
class Random {
  public:
    mt19937 mt_;
    // 0以上1.0未満の実数の範囲の乱数生成
    uniform_real_distribution<double> dd_{0, 1.0};

    // seedを指定して初期化
    explicit Random(const int seed = 0) : mt_(mt19937(seed)) {}

    // 0以上m未満の整数の範囲の乱数
    inline int nextInt(const int m) {
        uniform_int_distribution<int> di(0, m - 1);
        return di(mt_);
    }

    // 0以上1.0未満の実数の範囲の乱数
    inline double nextDouble() { return dd_(mt_); }

    // 0以上1.0未満の実数の範囲の乱数のlog
    inline double nextLog() { return log(dd_(mt_)); }
};
Random rnd{};

int main(int argc, char *argv[]) {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);
    double time_threshold = 1980;
    auto time_keeper = TimeKeeperDouble(time_threshold);

    vector<double> params;
    for (int i = 1; i < argc; ++i) {
        params.push_back(stod(argv[i]));
    }
    if (params.empty()) params = {38.32724807018248, 14};

    int n, L;
    cin >> n >> L;
    vector<int> T(2 * n);
    vector<int> D(2 * n);
    for (int i = 0; i < n; ++i) {
        cin >> T[i];
        T[i + n] = T[i];
        D[i] = T[i] / 2;
        D[i + n] = (T[i] + 1) / 2;
    }

    // 初期解
    vector<int> opt(2 * n, -1);
    vector<int> tmp(n);
    {
        vector<pair<int, int>> uo(2 * n);
        for (int i = 0; i < n; ++i) {
            uo[i] = {D[i], i};
            uo[i + n] = {D[i + n], i + n};
        }
        for (auto &i : uo) {
            int mx = -1e9, idx = -1;
            for (int j = 0; j < n; ++j) {
                int pre = abs(tmp[j] - T[j]);
                int nxt = abs(tmp[j] + i.first - T[j]);
                if (mx < pre - nxt) {
                    mx = pre - nxt;
                    idx = j;
                }
            }
            tmp[idx] += i.first;
            opt[i.second] = idx;
        }
    }

    auto calc1 = [&](vector<int> &res) -> int {
        fill(tmp.begin(), tmp.end(), 0);
        for (int i = 0; i < n; ++i) {
            tmp[res[i]] += T[i] / 2;
            tmp[res[i + n]] += (T[i] + 1) / 2;
        }
        int score = 0;
        for (int i = 0; i < n; ++i) {
            score += abs(tmp[i] - T[i]);
        }
        return score;
    };

    int try_cnt = 0;
    int cur_score = 0;
    for (int i = 0; i < n; ++i) {
        cur_score += abs(T[i] - tmp[i]);
    }
    int opt_score = cur_score;
    auto cur = opt;

    double start_temp = params[0], end_temp = 0;

    while (true) {
        try_cnt += 1;
        if ((try_cnt & 1023) == 0) {
            time_keeper.setNowTime();
            if (time_keeper.isTimeOver()) break;
        }
        double temp = start_temp + (end_temp - start_temp) * (time_keeper.getNowTime() / time_threshold);

        if (rnd.nextInt(100) <= params[1]) {
            int idx = rnd.nextInt(2 * n);
            int val = rnd.nextInt(n);
            int pre = cur[idx];
            if (val == idx or val == idx - n) {
                continue;
            }
            if (pre == val) {
                continue;
            }

            int next_score = cur_score;
            next_score -= abs(tmp[pre] - T[pre]);
            next_score -= abs(tmp[val] - T[val]);
            tmp[pre] -= D[idx];
            tmp[val] += D[idx];
            next_score += abs(tmp[pre] - T[pre]);
            next_score += abs(tmp[val] - T[val]);

            int diff = cur_score - next_score;
            if (exp(diff / temp) > rnd.nextDouble()) {
                cur[idx] = val;
                cur_score = next_score;
                if (cur_score < opt_score) {
                    opt = cur;
                    opt_score = cur_score;
                }
            } else {
                tmp[pre] += D[idx];
                tmp[val] -= D[idx];
            }
        } else {
            int idx1 = rnd.nextInt(2 * n);
            int idx2 = rnd.nextInt(2 * n);
            if (idx1 == idx2) {
                continue;
            }
            if (cur[idx2] == idx1 or cur[idx2] == idx1 - n or cur[idx1] == idx2 or cur[idx1] == idx2 - n) {
                continue;
            }
            if (cur[idx1] == cur[idx2]) {
                continue;
            }
            int next_score = cur_score;
            next_score -= abs(tmp[cur[idx1]] - T[cur[idx1]]);
            next_score -= abs(tmp[cur[idx2]] - T[cur[idx2]]);
            tmp[cur[idx1]] -= D[idx1];
            tmp[cur[idx2]] += D[idx1];
            tmp[cur[idx2]] -= D[idx2];
            tmp[cur[idx1]] += D[idx2];
            next_score += abs(tmp[cur[idx1]] - T[cur[idx1]]);
            next_score += abs(tmp[cur[idx2]] - T[cur[idx2]]);
            int diff = cur_score - next_score;
            if (exp(diff / temp) > rnd.nextDouble()) {
                swap(cur[idx1], cur[idx2]);
                cur_score = next_score;
                if (cur_score < opt_score) {
                    opt = cur;
                    opt_score = cur_score;
                }
            } else {
                tmp[cur[idx1]] += D[idx1];
                tmp[cur[idx2]] -= D[idx1];
                tmp[cur[idx2]] += D[idx2];
                tmp[cur[idx1]] -= D[idx2];
            }
        }
    }
    for (int i = 0; i < n; ++i) {
        cout << opt[i] << " " << opt[i + n] << "\n";
    }
    cerr << "try_cnt: " << try_cnt << "\n";
    cerr << "opt_score: " << opt_score << "\n";
}