// AHC063 入力ジェネレータ
// 使い方: ./gen <seed>
// 問題の入力生成規則に従って標準出力に入力を出力する

#include <bits/stdc++.h>
using namespace std;

mt19937 rng;

int rand_int(int l, int r) {
    return uniform_int_distribution<int>(l, r)(rng);
}

int main(int argc, char* argv[]) {
    uint64_t seed = (argc >= 2) ? stoull(argv[1]) : 42;
    rng = mt19937(seed);

    // N, M, C の生成
    int N = rand_int(8, 16);
    int M = rand_int((N * N + 3) / 4, 3 * N * N / 4);  // ceil(N²/4) ~ floor(3N²/4)
    int C = rand_int(3, 7);

    // d の生成
    // d[0..4] = 1, d[5..M-1] を生成
    vector<int> d(M, 1);
    {
        // m_c: 各色(1~C)の個数
        // n_0=0, n_C=M-5-C, n_1..n_{C-1} を [0, M-5-C] からランダム生成して昇順ソート
        // m_c = n_c - n_{c-1} + 1
        int total = M - 5;
        vector<int> mc(C + 1);
        while (true) {
            vector<int> ns(C + 1);
            ns[0] = 0;
            ns[C] = total - C;
            for (int i = 1; i < C; i++) ns[i] = rand_int(0, total - C);
            sort(ns.begin() + 1, ns.begin() + C);
            bool ok = true;
            for (int c = 1; c <= C; c++) {
                mc[c] = ns[c] - ns[c - 1] + 1;
                if (mc[c] > total / 2) { ok = false; break; }
            }
            if (ok) break;
        }
        // 色を m_c 個ずつ並べてシャッフル
        vector<int> tail;
        for (int c = 1; c <= C; c++)
            for (int i = 0; i < mc[c]; i++) tail.push_back(c);
        shuffle(tail.begin(), tail.end(), rng);
        for (int i = 0; i < (int)tail.size(); i++) d[5 + i] = tail[i];
    }

    // f の生成
    // 初期蛇マス: (0,0),(1,0),(2,0),(3,0),(4,0) 以外の N²-5 マスから M-5 個選ぶ
    vector<pair<int,int>> candidates;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (!(j == 0 && i <= 4)) candidates.emplace_back(i, j);

    shuffle(candidates.begin(), candidates.end(), rng);
    vector<pair<int,int>> chosen(candidates.begin(), candidates.begin() + (M - 5));

    // d[5..M-1] をシャッフルして配置
    vector<int> food_colors(d.begin() + 5, d.end());
    shuffle(food_colors.begin(), food_colors.end(), rng);

    vector<vector<int>> f(N, vector<int>(N, 0));
    for (int i = 0; i < (int)chosen.size(); i++)
        f[chosen[i].first][chosen[i].second] = food_colors[i];

    // 出力
    cout << N << " " << M << " " << C << "\n";
    for (int i = 0; i < M; i++) cout << d[i] << " \n"[i == M - 1];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) cout << f[i][j] << " \n"[j == N - 1];
    }

    return 0;
}
