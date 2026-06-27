#include <bits/stdc++.h>
using namespace std;
const int INF=2100000000;
#define rep(i,a,b) for (int i=(int)(a);i<(int)(b);i++)
#define all(p) p.begin(),p.end()
template<class T> bool chmin(T &a,T b){if(b<a){a=b;return 1;}else return 0;}
template<class T> bool chmax(T &a,T b){if(a<b){a=b;return 1;}else return 0;}

using Clock = std::chrono::steady_clock;

Clock::time_point start;

inline double elapsed_sec() {
    return std::chrono::duration<double>(Clock::now() - start).count();
}
const double TL = 1.95;

struct F{
    int x;
    int y;
};
int dist(F a, F b){
    return abs(a.x - b.x) + abs(a.y - b.y);
}

//https://ei1333.github.io/library/other/xor-shift.cpp.html
struct XorShift {
private:
    constexpr static double R = 1.0 / 0xffffffff;
    uint64_t x;

public:
    explicit XorShift(uint64_t seed = 88172645463325252ull) : x(seed) {}

    template< typename T = uint64_t >
    inline T get() { // [0, 2^64)
        x ^= x << 7ull;
        x ^= x >> 9ull;
        return x;
    }

    inline uint32_t get(uint32_t r) { // [0, r)
        return ((uint64_t) get< uint32_t >() * r) >> 32ull;
    }

    inline uint32_t get(uint32_t l, uint32_t r) { // [l, r)
        return l + get(r - l);
    }

    inline double probability() { // [0.0, 1.0]
        return get< uint32_t >() * R;
    }
}rng;


void solve();
// DEAR MYSTERIES / TOMOO
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    start = Clock::now();

    int t = 1;
    // cin >> t;
    rep(i, 0, t) solve();
}

void solve(){
    int N;
    cin >> N;
    vector<int> cnt(N * N / 2);
    vector<int> fr(N * N / 2);
    vector<F> G(N * N);
    rep(i, 0, N) rep(j, 0, N){
        int a;
        cin >> a;
        G[a * 2 + cnt[a]] = {i, j};
        cnt[a]++;
    }
    G.push_back({0, 0});
    struct card {
        int num;
        int k;
    };
    vector<card> pos = {{N * N / 2, 0}};
    auto dist_card = [&](card a, card b) -> int {
        return dist(G[a.num * 2 + a.k], G[b.num * 2 + b.k]);
    };
    vector<int> H((N * N + 1) * (N * N + 1));
    rep(i, 0, N * N + 1) rep(j, 0, N * N + 1) H[i * (N * N + 1) + j] = dist(G[i], G[j]);
    auto dist_card_2 = [&](int a, int b) -> int {
        return H[a * (N * N + 1) + b];
        // return dist(G[a], G[b]);
    };
    // 今の pos に a を insert する際
    // 最適な位置を提案する
    auto insert = [&](int a) -> tuple<int, int, int> {
        int x = 0, y = 0, z = INF;
        rep(i, 0, pos.size()){
            rep(j, 0, 2){
                int dif = dist(G[a * 2], G[a * 2 + 1]);
                dif += dist_card(pos[i], {a, j});
                if (i + 1 != (int)pos.size()){
                    dif += dist_card(pos[i + 1], {a, j ^ 1});
                    dif -= dist_card(pos[i], pos[i + 1]);
                }
                if (chmin(z, dif)){
                    if (j) x = i, y = i;
                    else x = i, y = i + 1;
                }
            }
        }
        return {x, y, z};
    };
    // 初期化
    rep(i, 0, N * N / 2){
        auto [x, y, z] = insert(i);
        pos.insert(pos.begin() + x + 1, {i, 0});
        pos.insert(pos.begin() + y + 1, {i, 1});
    }
    /*rep(i, 0, N * N / 2){
        pos.insert(pos.begin() + 1, {i, 0});
        pos.insert(pos.begin() + 1, {i, 1});
    }*/
    vector<int> L(N * N + 1, -1), R(N * N + 1, -1);
    rep(i, 0, N * N){
        int a = pos[i].num * 2 + pos[i].k;
        int b = pos[i + 1].num * 2 + pos[i + 1].k;
        R[a] = b;
        L[b] = a;
    }
    int M = N * N  / 2;
    double T1 = 1.67, T2 = 0.00167;
    int X = 0, Y = 0, Z = 0, W = 0;
    vector<card> best;
    vector<int> bestL = L, bestR = R;

    // a の直後に b を insert する
    auto ins_l = [&](int a, int b) -> void {
        int r = R[a];
        L[b] = a;
        R[b] = r;
        R[a] = b;
        if (r != -1) L[r] = b;
    };
    auto del_l = [&](int a) -> void {
        assert(a != -1);
        int l = L[a];
        int r = R[a];
        assert(l != -1);
        if (r == -1){
            R[l] = -1;
        }
        else{
            R[l] = r;
            L[r] = l;
        }
    };

    {
        int ind = R[N * N];
        while (ind != -1){
            fr[ind / 2] = 1 - ind % 2;
            ind = R[ind];
        }
    }
    

    double temp, now;
    
    
    double tmp_H = log2(exp(1));
    auto h = [&](double x) -> bool {
        if (x >= 0) return true;
        // rng.probability() < exp(x)
        int w = __builtin_clz(rng.get() | 1);
        return -w < x * tmp_H;
    };
    
    vector<int> la(2);
    while (true){
        if ((Z & 32767) == 0){
            now = elapsed_sec();
            temp = ((TL - now) * T1 + T2 * now) / TL;
        }
        if (now > TL) break;
        Z++;
        // double temp = ((TL - now) * T1 + T2 * now) / TL;
        int a = rng.get(0, M);
        double pro = rng.probability();
        // 抜き出して　insert
        if (pro < 0.4){
            int dif = 0;
            rep(j, 0, 2) {
                int n = a * 2 + j;
                dif += dist_card_2(L[n], n);
                if (R[n] != -1) {
                    dif += dist_card_2(R[n], n);
                    dif -= dist_card_2(L[n], R[n]);
                }
                la[j] = L[n];
                del_l(n);
            }
            int b = rng.get(0, N * N - 1);
            if (b >= a * 2) b += 2;
            int ord = 0, c = INF;
            rep(j, 0, 2) {
                int tmp = 0;
                tmp += dist_card_2(a * 2, a * 2 + 1);
                int nx = R[b];
                tmp += dist_card_2(b, a * 2 + j);
                if (nx != -1) {
                    tmp -= dist_card_2(b, nx);
                    tmp += dist_card_2(nx, a * 2 + 1 - j);
                }
                if (chmin(c, tmp)) ord = j;
            }
            if (h((dif - c) / temp)) {
                ins_l(b, a * 2 + 1 - ord);
                ins_l(b, a * 2 + ord);
                fr[a] = ord;
                X += dif - c;
                if (chmax(Y, X)) {
                    bestR = R;
                }
                W++;
            } else {
                ins_l(la[1], a * 2 + 1);
                ins_l(la[0], a * 2);
            }
        }

        else {
            int dif = 0;
            rep(j, 0, 2) {
                int n = a * 2 + j;
                dif += dist_card_2(L[n], n);
                if (R[n] != -1) {
                    dif += dist_card_2(R[n], n);
                    dif -= dist_card_2(L[n], R[n]);
                }
                la[j] = L[n];
                del_l(n);
            }
            // ab...ba とする
            int b = rng.get(0, N * N / 2 - 1);
            if (b >= a) b++;
            int x = b * 2 + fr[b], y = b * 2 + 1 - fr[b];
            // y を 1 つ進めてしまう。
            while (R[y] != -1 && fr[R[y] / 2] == (R[y] & 1) && rng.get(0, 2) == 0){
                y = (R[y] ^ 1);
            }
            while (L[x] != N * N && fr[L[x] / 2] != (L[x] & 1) && rng.get(0, 2) == 0){
                x = (L[x] ^ 1);
            }
            int ord = 0, c = INF;

            rep(j, 0, 2) {
                int tmp = 0;
                tmp += dist_card_2(x, a * 2 + j);
                tmp += dist_card_2(L[x], a * 2 + j);
                tmp -= dist_card_2(L[x], x);
                tmp += dist_card_2(y, a * 2 + 1 - j);
                if (R[y] != -1){
                    tmp += dist_card_2(R[y], a * 2 + 1 - j);
                    tmp -= dist_card_2(R[y], y);
                }
                if (chmin(c, tmp)) ord = j;
            }
            if (h((dif - c) / temp)) {
                ins_l(y, a * 2 + 1 - ord);
                ins_l(L[x], a * 2 + ord);
                fr[a] = ord;
                X += dif - c;
                if (chmax(Y, X)) {
                    bestR = R;
                }
                W++;
            } else {
                ins_l(la[1], a * 2 + 1);
                ins_l(la[0], a * 2);
            }
        }

    }

    {
        best = {};
        int ind = N * N;
        while (ind != -1){
            best.push_back({ind / 2, ind % 2});
            ind = bestR[ind];
        }
    }

    cerr << X << " " << Y << "\n";
    cerr << Z << " " << W << "\n";
    auto f = [&](F a, F b) -> string {
        string res;
        rep(i, a.x, b.x) res += 'D';
        rep(i, b.x, a.x) res += 'U';
        rep(i, a.y, b.y) res += 'R';
        rep(i, b.y, a.y) res += 'L';
        return res;
    };
    string ans;
    pos = best;
    rep(i, 1, pos.size()){
        int a = pos[i - 1].num * 2 + pos[i - 1].k;
        int b = pos[i].num * 2 + pos[i].k;
        ans += f(G[a], G[b]);
        ans += "Z";
    }
    for (auto c : ans) cout << c << "\n";
}

