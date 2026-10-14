// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algebra/number_theory/miller_rabin.cpp
/*ミラーラビン素数判定法 O(logn)で素数判定を行う*/
bool miller_rabin(ull n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    vector<ull> A = {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL};

    ull d = n - 1;
    int s = 0;
    while (d % 2 == 0) {
        s++;
        d >>= 1;
    }

    for (ull a : A) {
        a %= n;
        if (a == 0) continue;
        ull x = pow_mod64(a, d, n);
        if (x == 1) continue;
        for (int i = 1; x != n - 1; ++i) {
            if (i >= s) return false;
            x = (uvll)x * x % n;
        }
    }
    return true;
}
