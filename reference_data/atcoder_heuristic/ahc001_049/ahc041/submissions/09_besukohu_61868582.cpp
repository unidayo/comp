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
#include <cstdalign>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#endif

// C++
#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <chrono>
#include <codecvt>
#include <complex>
#include <condition_variable>
#include <deque>
#include <exception>
#include <forward_list>
#include <fstream>
#include <functional>
#include <future>
#include <initializer_list>
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
#include <mutex>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <random>
#include <ratio>
#include <regex>
#include <scoped_allocator>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <vector>

#if __cplusplus >= 201402L
#include <shared_mutex>
#endif

#if __cplusplus >= 201703L
#include <charconv>
#endif

using namespace std;
using namespace std::chrono;
using ll = long long;
using ull = unsigned long long;
using pll = pair<long long, long long>;
using tlll = tuple<long long, long long, long long>;
using tllll = tuple<long long, long long, long long, long long>;
std::mt19937 mt(time(0));  //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
std::uniform_int_distribution<ll> toobig(0, 1e9);
string argnum;

class xor_shift_128 {
   public:
    typedef uint32_t result_type;
    xor_shift_128(result_type seed = mt()) {
        set_seed(seed);
    }
    void set_seed(result_type seed) {
        a = seed = 1812433253 * (seed ^ (seed >> 30));
        b = seed = 1812433253 * (seed ^ (seed >> 30)) + 1;
        c = seed = 1812433253 * (seed ^ (seed >> 30)) + 2;
        d = seed = 1812433253 * (seed ^ (seed >> 30)) + 3;
    }
    result_type operator()() {
        result_type t = (a ^ (a << 11));
        a = b;
        b = c;
        c = d;
        return d = (d ^ (d >> 19)) ^ (t ^ (t >> 8));
    }
    static constexpr result_type max() { return numeric_limits<result_type>::max(); }
    static constexpr result_type min() { return numeric_limits<result_type>::min(); }

   private:
    result_type a, b, c, d;
};
xor_shift_128 xorrand;
ifstream in;
ofstream out;

using pid = pair<int, double>;
using pdd = pair<double, double>;

using a2 = array<ll, 2>;

class solver {
   public:
    double temp;
    ll vernum, edgenum, height;
    vector<ll> beauty;
    vector<vector<ll>> g;
    vector<ll> nowh, addv;

    vector<a2> xy;

    ll score = 0;

   public:
    void init() {
        cin >> vernum >> edgenum >> height;
        beauty.resize(vernum);
        nowh.assign(vernum, 0);
        g.resize(vernum);
        for (int i = 0; i < vernum; i++) {
            cin >> beauty[i];
            score += beauty[i];
        }

        for (int i = 0; i < edgenum; i++) {
            ll a, b;
            cin >> a >> b;
            g[a].push_back(b);
            g[b].push_back(a);
        }

        for (int i = -10; i <= 10; i++) {
            if (i == 0) {
                addv.push_back(i);
            } else if (i > 0) {
                for (int j = 0; j < 3; j++) {
                    addv.push_back(i);
                }
            } else {
                for (int j = 0; j < 2; j++) {
                    addv.push_back(i);
                }
            }
        }
    }

    void reset() {
        nowh.assign(vernum, 0);
        score = accumulate(beauty.begin(), beauty.end(), 0LL);
    }
    void modify(double t) {
        temp = t;

        bool ok = true;

        if (xorrand() % 8) {
            ll chonum = xorrand() % vernum;
            ll add = addv[0];
            ll idx = 1;
            while (nowh[chonum] + add > height || nowh[chonum] + add < 0) {
                add = addv[idx];
                idx++;
            }

            ll num2 = 0;
            for (auto &x : g[chonum]) {
                ll num = 0;
                num2 += nowh[x] == nowh[chonum] + add - 1;
                if (nowh[x] != nowh[chonum] + 1)
                    continue;

                for (auto &y : g[x]) {
                    if (nowh[y] == nowh[x] - 1)
                        num++;
                }

                if (num == 1) {
                    ok = false;
                    break;
                }
            }

            if ((num2 || nowh[chonum] + add == 0) && ok && calc_prob(add * beauty[chonum])) {
                score += add * beauty[chonum];
                nowh[chonum] += add;
            }
        } else {
            ll cho1 = xorrand() % vernum, cho2 = g[cho1][xorrand() % g[cho1].size()];

            ll scorediff = -(nowh[cho1] + 1) * beauty[cho1] - (nowh[cho2] + 1) * beauty[cho2];
            swap(nowh[cho1], nowh[cho2]);
            scorediff += (nowh[cho1] + 1) * beauty[cho1] + (nowh[cho2] + 1) * beauty[cho2];

            bool dou = (nowh[cho1] == 0);
            for (auto &x : g[cho1]) {
                dou |= (nowh[x] == nowh[cho1] - 1);
                bool dou2 = (nowh[x] == -1);
                for (auto &y : g[x]) {
                    dou2 |= nowh[y] == nowh[x] - 1;
                }
                ok &= dou2;
                if (!ok)
                    break;
            }
            ok &= dou;

            dou = (nowh[cho2] == 0);
            for (auto &x : g[cho2]) {
                dou |= (nowh[x] == nowh[cho2] - 1);
                bool dou2 = (nowh[x] == -1);
                for (auto &y : g[x]) {
                    dou2 |= nowh[y] == nowh[x] - 1;
                }
                ok &= dou2;
                if (!ok)
                    break;
            }
            ok &= dou;

            if (ok && calc_prob(scorediff)) {
                score += scorediff;
            } else {
                swap(nowh[cho1], nowh[cho2]);
            }
        }
    }

    int deka = (1 << 16);
    bool calc_prob(int diff) {
        if (diff >= 0) {
            return true;
        }
        return exp(diff / temp) * deka > xorrand() % deka;
    }

    void answer() {
        set<ll> sh;
        vector<ll> ans(vernum, -1);
        for (int h = 10; h >= 1; h--)
            for (int i = 0; i < vernum; i++) {
                if (nowh[i] != h)
                    continue;

                if (sh.count(i)) {
                    for (auto &x : g[i]) {
                        if (nowh[x] == nowh[i] - 1) {
                            ans[i] = x;
                        }
                    }
                    for (auto &x : g[i]) {
                        if (nowh[x] == nowh[i] - 1 && sh.count(x)) {
                            ans[i] = x;
                        }
                    }
                } else {
                    for (auto &x : g[i]) {
                        if (nowh[x] >= nowh[i] - 1 && nowh[x] != height) {
                            nowh[i] = nowh[x] + 1;
                            ans[i] = x;
                        }
                    }
                }
                sh.insert(ans[i]);
            }

        for (auto &x : ans)
            cout << x << ' ';
        cout << endl;
    }
};
int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(0);
    long long a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, m = 0, n = 0, p = 0, q = 0, mod = 998244353;
    long double pi = 3.1415926535, ad = 0, bd = 0, cd = 0, dd = 0;
    string s1, s2;
    char moji, moja;

    if (argc > 2) {
        in.open(argv[1]);
        cin.rdbuf(in.rdbuf());
        out.open(argv[2]);
        cout.rdbuf(out.rdbuf());
    }

    ll solnum = 1;
    vector<solver> sols(solnum);

    sols[0].init();
    for (int i = 1; i < solnum; i++) {
        sols[i] = sols[0];
    }
    auto starttime = chrono::system_clock::now();
    ll timelimit = 1980;
    ll bestscore = 0;
    vector<ll> bestans;
    double starttemp = 200, endtemp = 0, nowtemp = starttemp;

    for (ll turn = 0; 1; turn++) {
        if ((turn & 255) == 255) {
            ll timediff = duration_cast<milliseconds>(chrono::system_clock::now() - starttime).count();
            if (timediff > timelimit) {
                n = turn;
                break;
            }
            for (int h = 0; h < solnum; h++)
                for (int i = 0; i < sols[h].addv.size(); i++) {
                    swap(sols[h].addv[i], sols[h].addv[i + xorrand() % (sols[h].addv.size() - i)]);
                }
            nowtemp = endtemp - ((endtemp - starttemp) * pow(timelimit - timediff, 1)) / pow(timelimit, 1);
        }

        for (int i = 0; i < solnum; i++) {
            if (sols[i].score > bestscore) {
                bestans = sols[i].nowh;
                bestscore = sols[i].score;
            }
            sols[i].modify(nowtemp);
        }
    }
    sols[0].nowh = bestans;
    sols[0].score = bestscore;
    sols[0].answer();

    cerr << n << endl;
    cerr << "Score = " << bestscore << endl;
    return 0;
}