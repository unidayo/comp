const ll N_MAX = 2001;
ll comb[N_MAX][N_MAX];

void comb_mod_dp() {
    for (auto &i: comb) {
        for (long long &j: i) {
            j = 0LL;
        }
    }
    comb[0][0] = 1LL;
    for (int i = 1; i < N_MAX; ++i) {
        comb[i][0] = 1LL;
        for (int j = 1; j < N_MAX; ++j) {
            comb[i][j] = (comb[i - 1][j - 1] + comb[i - 1][j]) % MOD;
        }
    }
}