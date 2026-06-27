#include <bits/stdc++.h>
using namespace std;

class xrand {
    uint64_t x;

   public:
    using result_type = uint32_t;
    static constexpr result_type min() {
        return std::numeric_limits<result_type>::min();
    }
    static constexpr result_type max() {
        return std::numeric_limits<result_type>::max();
    }
    xrand(uint64_t k) : x(k) {}
    xrand() : xrand(1) {}
    result_type operator()() {
        x ^= x << 9;
        return x ^= x >> 7;
        // return (x * 0x123456789abcdef) >> 32;
    }
    int64_t operator()(int m) { return int64_t((*this)()) * m >> 32; }
    double rand() { return double((*this)()) / (int64_t(1) << 32); }
};

constexpr int64_t TL = 1990000000;

constexpr int N = 1000, H = 10;

int main() {
    xrand rng;

    auto start = chrono::steady_clock::now();
    auto elapsed = [&]() {
        return chrono::duration_cast<chrono::nanoseconds>(
                   chrono::steady_clock::now() - start)
            .count();
    };

    int _, M;
    cin >> _ >> M >> _;
    array<int, N> A;
    for (int i = 0; i < N; i++) {
        cin >> A[i];
    }
    array<vector<int>, N> to;
    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v;
        to[u].push_back(v);
        to[v].push_back(u);
    }

    array<int, N> best_d;
    best_d.fill(H + 1);
    for (int i = 0; i < N; i++) {
        if (best_d[i] <= H) continue;
        vector<int> q;
        best_d[i] = 0;
        q.push_back(i);
        while (not q.empty()) {
            int v = q.back();
            q.pop_back();
            for (auto &&c : to[v]) {
                if (best_d[c] <= H) continue;
                best_d[c] = best_d[v] + 1;
                if (best_d[c] < H) {
                    q.push_back(c);
                }
                if (best_d[c] < 3) break;
            }
        }
    }

    int best_score = 1;
    for (int i = 0; i < N; i++) {
        best_score += (best_d[i] + 1) * A[i];
    }

    auto score = best_score;
    auto d = best_d;
    constexpr double start_temp = 200;
    while (true) {
        double phase = double(elapsed()) / TL;
        if (phase >= 1.0) break;
        double temp = (1.0 - phase) * start_temp;
        for (int _ = 0; _ < 4096; _++) {
            int v = rng(N);
            int e_count = to[v].size();
            int idx = rng(e_count + 1);
            int d_next = idx < e_count ? d[to[v][idx]] + 1 : 0;
            if (d_next > H) continue;
            int diff = (d_next - d[v]) * A[v];
            if (diff == 0) continue;
            if (diff < 0 and exp(diff / temp) < rng.rand()) continue;

            bool ok = true;
            for (auto &&c : to[v]) {
                if (d[c] == d[v] + 1) {
                    bool connected = false;
                    for (auto &&cc : to[c]) {
                        if (cc == v) continue;
                        if (d[cc] == d[c] - 1) {
                            connected = true;
                            break;
                        }
                    }
                    if (not connected) {
                        ok = false;
                        break;
                    }
                }
            }

            if (ok) {
                score += diff;
                d[v] = d_next;

                if (score > best_score) {
                    best_score = score;
                    best_d = d;
                }
            }
        }
    }

    for (int i = 0; i < N; i++) {
        if (best_d[i] == 0) {
            cout << -1;
        } else {
            for (auto &&c : to[i]) {
                if (best_d[c] == best_d[i] - 1) {
                    cout << c;
                    break;
                }
            }
        }
        if (i < N - 1) {
            cout << ' ';
        }
    }
    cout << endl;

    cerr << best_score << endl;

    return 0;
}
