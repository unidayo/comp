#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
#pragma GCC target("avx2")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3")

#include <bits/stdc++.h>

#define FOR(i, s, e) for (int i = int(s); i < int(e); ++i)
#define REV(i, s, e) for (int i = int(s); i > int(e); --i)

using namespace std;


uint32_t randxor() {
    static uint32_t x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    uint32_t t;
    t = (x^(x<<11));
    x = y;
    y = z;
    z = w;
    return w = (w^(w>>19))^(t^(t>>8));
}


uint32_t randrange(const uint32_t n) {
    return uint64_t(randxor()) * n >> 32;
}


uint32_t randrange(const uint32_t l, const uint32_t r) {
    return randrange(r-l)+l;
}

uint32_t random_by_weight(const vector<uint32_t>& cum_sum_weight) {
    const auto w = randrange(cum_sum_weight.back());
    const auto i = lower_bound(cum_sum_weight.begin(), cum_sum_weight.end(), w) - cum_sum_weight.begin();
    return i;
}


double uniform() {
    return (double)randxor()/UINT32_MAX;
}


constexpr int N = 1000;
constexpr int MAX_TIME = 1950;
constexpr long long MAX_A = 1000000000;
constexpr double INITIAL_TEMPERATURE_COEF = 0.0001;

pair<long long, long long> ab[5*N], temp_ab[5*N], best_ab[5*N], pre_ab[5*N];
unordered_set<long long> added, used;
unordered_map<long long, long long> parents;
unordered_map<long long, vector<long long>> children;
long long objective[N];


int main() {
    double start = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    int dummy_n;
    cin >> dummy_n;
    FOR (i,0,N) {
        long long a,b;
        cin >> a >> b;
        ab[i] = {a,b};
        auto key = a*MAX_A+b;
        added.insert(key);
        objective[i] = key;
    }
    int size = N;
    int pre_size = size;
    sort(ab, ab+size);
    vector<pair<int, int>> comb;
    FOR (i,0,N) {
        auto ai = ab[i].first;
        auto bi = ab[i].second;
        FOR (j,0,i) {
            auto aj = ab[j].first;
            auto bj = ab[j].second;
            if (ai >= aj && bi >= bj) {
                continue;
            }
            comb.push_back({i,j});
        }
    }
    sort(comb.begin(), comb.end(), [&](const pair<int, int>& a, const pair<int, int>& b) {
        return abs(ab[a.first].first-ab[a.second].first)+abs(ab[a.first].second-ab[a.second].second) < abs(ab[b.first].first-ab[b.second].first)+abs(ab[b.first].second-ab[b.second].second);
    });
    for (const auto& [i,j]: comb) {
        auto ai = ab[i].first;
        auto bi = ab[i].second;
        auto aj = ab[j].first;
        auto bj = ab[j].second;
        long long a,b;
        a = min(ai,aj);
        b = min(bi,bj);
        auto key = a*MAX_A+b;
        if (added.find(key) == added.end()) {
            ab[size++] = {a,b};
            added.insert(key);
            if (size >= 1.5*N) {
                break;
            }
        }
    }
    while (size < 5*N) {
        cerr << size << "\n";
        sort(ab, ab+size);
        children.clear();
        FOR (i,0,size) {
            auto a = ab[i].first;
            auto b = ab[i].second;
            long long x,y;
            x = 0;
            y = 0;
            FOR (j,0,i) {
                auto aj = ab[j].first;
                auto bj = ab[j].second;
                if (a < aj || b < bj) {
                    continue;
                }
                if (a-aj+b-bj < a-x+b-y) {
                    x = aj;
                    y = bj;
                }
            }
            parents[a*MAX_A+b] = x*MAX_A+y;
            children[x*MAX_A+y].push_back(a*MAX_A+b);
        }
        used.clear();
        FOR (i,0,N) {
            auto key = objective[i];
            used.insert(key);
            while (key != 0) {
                key = parents[key];
                if (children[key].size() == 1) {
                    continue;
                }
                used.insert(key);
            }
        }
        int temp_size = 0;
        added.clear();
        FOR (i,0,size) {
            auto a = ab[i].first;
            auto b = ab[i].second;
            auto key = a*MAX_A+b;
            if (used.find(key) == used.end()) {
                continue;
            }
            temp_ab[temp_size++] = {a,b};
            added.insert(key);
        }
        size = temp_size;
        FOR (i,0,temp_size) {
            ab[i] = temp_ab[i];
        }
        FOR (i,0,size) {
            auto ai = ab[i].first;
            auto bi = ab[i].second;
            const auto abi = ai*MAX_A+bi;
            auto p = parents[abi];
            FOR (_,0,4) {
                for (const auto& abj: children[p]) {
                    if (abi == abj) {
                        continue;
                    }
                    auto aj = abj/MAX_A;
                    auto bj = abj%MAX_A;
                    long long a,b;
                    a = min(ai,aj);
                    b = min(bi,bj);
                    auto key = a*MAX_A+b;
                    if (added.find(key) == added.end()) {
                        ab[size++] = {a,b};
                        added.insert(key);
                        if (size >= 5*N) {
                            break;
                        }
                    }
                }
                if (size >= 5*N) {
                    break;
                }
                if (p == 0) {
                    break;
                }
                p = parents[p];
            }
            if (size >= 5*N) {
                break;
            }
        }
        if (size == pre_size) {
            break;
        }
        pre_size = size;
    }
    long long score = 0;
    sort(ab, ab+size);
    FOR (i,0,size) {
        auto a = ab[i].first;
        auto b = ab[i].second;
        long long x,y;
        x = 0;
        y = 0;
        FOR (j,0,i) {
            auto aj = ab[j].first;
            auto bj = ab[j].second;
            if (a < aj || b < bj) {
                continue;
            }
            if (a-aj+b-bj < a-x+b-y) {
                x = aj;
                y = bj;
            }
        }
        score += a-x+b-y;
    }

    cerr << score << "\n";
    pre_size = size;
    int best_size = size;
    auto best_score = score;
    FOR (i,0,size) {
        pre_ab[i] = ab[i];
        best_ab[i] = ab[i];
    }
    long long iter = 0;

    while (true) {
        const double time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - start;
        if (time >= MAX_TIME) {
            break;
        }
        ++iter;
        if (iter%100000 == 0) {
            // size = best_size;
            // FOR (i,0,size) {
            //     ab[i] = best_ab[i];
            // }

            sort(ab, ab+size);
            children.clear();
            FOR (i,0,size) {
                auto a = ab[i].first;
                auto b = ab[i].second;
                long long x,y;
                x = 0;
                y = 0;
                FOR (j,0,i) {
                    auto aj = ab[j].first;
                    auto bj = ab[j].second;
                    if (a < aj || b < bj) {
                        continue;
                    }
                    if (a-aj+b-bj < a-x+b-y) {
                        x = aj;
                        y = bj;
                    }
                }
                parents[a*MAX_A+b] = x*MAX_A+y;
                children[x*MAX_A+y].push_back(a*MAX_A+b);
            }
            used.clear();
            FOR (i,0,N) {
                auto key = objective[i];
                used.insert(key);
                while (key != 0) {
                    key = parents[key];
                    if (children[key].size() == 1) {
                        continue;
                    }
                    used.insert(key);
                }
            }

            int temp_size = 0;
            added.clear();
            FOR (i,0,size) {
                auto a = ab[i].first;
                auto b = ab[i].second;
                auto key = a*MAX_A+b;
                if (used.find(key) == used.end()) {
                    continue;
                }
                temp_ab[temp_size++] = {a,b};
                added.insert(key);
            }
            size = temp_size;
            FOR (i,0,temp_size) {
                ab[i] = temp_ab[i];
            }
            sort(ab, ab+size);
            score = 0;
            FOR (i,0,size) {
                auto a = ab[i].first;
                auto b = ab[i].second;
                long long x,y;
                x = 0;
                y = 0;
                FOR (j,0,i) {
                    auto aj = ab[j].first;
                    auto bj = ab[j].second;
                    if (a < aj || b < bj) {
                        continue;
                    }
                    if (a-aj+b-bj < a-x+b-y) {
                        x = aj;
                        y = bj;
                    }
                }
                score += a-x+b-y;
            }
            pre_size = size;
            best_size = size;
            best_score = score;
            FOR (i,0,size) {
                pre_ab[i] = ab[i];
                best_ab[i] = ab[i];
            }
            continue;
        }
        const auto temperature = score * INITIAL_TEMPERATURE_COEF * (MAX_TIME - time) / MAX_TIME;
        const long long update_threshold = score - temperature * log2(uniform());
        int i,j;
        i = randrange(1,size);
        j = randrange(i);
        auto ai = ab[i].first;
        auto bi = ab[i].second;
        auto aj = ab[j].first;
        auto bj = ab[j].second;
        auto dist = abs(ai-aj)+abs(bi-bj);
        while (dist > MAX_A/5) {
            i = randrange(1,size);
            j = randrange(i);
            ai = ab[i].first;
            bi = ab[i].second;
            aj = ab[j].first;
            bj = ab[j].second;
            dist = abs(ai-aj)+abs(bi-bj);
        }
        long long a,b;
        a = min(ai,aj);
        b = min(bi,bj);
        auto key = a*MAX_A+b;
        if (added.find(key) != added.end()) {
            continue;
        }
        const auto pre_pi = parents[ai*MAX_A+bi];
        const auto pre_pj = parents[aj*MAX_A+bj];
        const auto pai = pre_pi/MAX_A;
        const auto pbi = pre_pi%MAX_A;
        const auto paj = pre_pj/MAX_A;
        const auto pbj = pre_pj%MAX_A;
        long long x,y;
        x = 0;
        y = 0;
        FOR (j,0,i) {
            auto aj = ab[j].first;
            auto bj = ab[j].second;
            if (a < aj || b < bj) {
                continue;
            }
            if (a-aj+b-bj < a-x+b-y) {
                x = aj;
                y = bj;
            }
        }
        long long new_score = score+(ai-a+bi-b+aj-a+bj-b+a-x+b-y)-(ai-pai+bi-pbi+aj-paj+bj-pbj);
        // if (children[pre_pi].size() == 1) {
        //     const auto pre_pre_pi = parents[pre_pi];
        //     const auto ppai = pre_pre_pi/MAX_A;
        //     const auto ppbi = pre_pre_pi%MAX_A;
        //     new_score -= pai-ppai+pbi-ppbi;
        // }
        // if (children[pre_pj].size() == 1) {
        //     const auto pre_pre_pj = parents[pre_pj];
        //     const auto ppaj = pre_pre_pj/MAX_A;
        //     const auto ppbj = pre_pre_pj%MAX_A;
        //     new_score -= paj-ppaj+pbj-ppbj;
        // }
        // cerr << new_score << " " << update_threshold << " " << score << "\n";
        if (new_score < update_threshold) {
            parents[a*MAX_A+b] = x*MAX_A+y;
            parents[ai*MAX_A+bi] = a*MAX_A+b;
            parents[aj*MAX_A+bj] = a*MAX_A+b;
            children[x*MAX_A+y].push_back(a*MAX_A+b);
            children[a*MAX_A+b].push_back(ai*MAX_A+bi);
            children[a*MAX_A+b].push_back(aj*MAX_A+bj);
            ab[size++] = {a,b};
            added.insert(key);
            score = new_score;
            pre_ab[pre_size++] = ab[size-1];
            if (score < best_score) {
                best_score = score;
                best_size = size;
                FOR (i,0,size) {
                     best_ab[i] = ab[i];
                }
            }
        }

        if (size >= 5*N) {
            // size = best_size;
            // FOR (i,0,size) {
            //     ab[i] = best_ab[i];
            // }

            sort(ab, ab+size);
            children.clear();
            FOR (i,0,size) {
                auto a = ab[i].first;
                auto b = ab[i].second;
                long long x,y;
                x = 0;
                y = 0;
                FOR (j,0,i) {
                    auto aj = ab[j].first;
                    auto bj = ab[j].second;
                    if (a < aj || b < bj) {
                        continue;
                    }
                    if (a-aj+b-bj < a-x+b-y) {
                        x = aj;
                        y = bj;
                    }
                }
                parents[a*MAX_A+b] = x*MAX_A+y;
                children[x*MAX_A+y].push_back(a*MAX_A+b);
            }
            used.clear();
            FOR (i,0,N) {
                auto key = objective[i];
                used.insert(key);
                while (key != 0) {
                    key = parents[key];
                    if (children[key].size() == 1) {
                        continue;
                    }
                    used.insert(key);
                }
            }

            int temp_size = 0;
            added.clear();
            FOR (i,0,size) {
                auto a = ab[i].first;
                auto b = ab[i].second;
                auto key = a*MAX_A+b;
                if (used.find(key) == used.end()) {
                    continue;
                }
                temp_ab[temp_size++] = {a,b};
                added.insert(key);
            }
            size = temp_size;
            FOR (i,0,temp_size) {
                ab[i] = temp_ab[i];
            }
            sort(ab, ab+size);
            score = 0;
            FOR (i,0,size) {
                auto a = ab[i].first;
                auto b = ab[i].second;
                long long x,y;
                x = 0;
                y = 0;
                FOR (j,0,i) {
                    auto aj = ab[j].first;
                    auto bj = ab[j].second;
                    if (a < aj || b < bj) {
                        continue;
                    }
                    if (a-aj+b-bj < a-x+b-y) {
                        x = aj;
                        y = bj;
                    }
                }
                score += a-x+b-y;
            }
            pre_size = size;
            best_size = size;
            best_score = score;
            FOR (i,0,size) {
                pre_ab[i] = ab[i];
                best_ab[i] = ab[i];
            }
        }
    }
    cerr << iter << "\n";

    size = best_size;
    FOR (i,0,size) {
        ab[i] = best_ab[i];
    }

    sort(ab, ab+size);
    children.clear();
    FOR (i,0,size) {
        auto a = ab[i].first;
        auto b = ab[i].second;
        long long x,y;
        x = 0;
        y = 0;
        FOR (j,0,i) {
            auto aj = ab[j].first;
            auto bj = ab[j].second;
            if (a < aj || b < bj) {
                continue;
            }
            if (a-aj+b-bj < a-x+b-y) {
                x = aj;
                y = bj;
            }
        }
        parents[a*MAX_A+b] = x*MAX_A+y;
        children[x*MAX_A+y].push_back(a*MAX_A+b);
    }
    used.clear();
    FOR (i,0,N) {
        auto key = objective[i];
        used.insert(key);
        while (key != 0) {
            key = parents[key];
            if (children[key].size() == 1) {
                continue;
            }
            used.insert(key);
        }
    }

    int temp_size = 0;
    added.clear();
    FOR (i,0,size) {
        auto a = ab[i].first;
        auto b = ab[i].second;
        auto key = a*MAX_A+b;
        if (used.find(key) == used.end()) {
            continue;
        }
        temp_ab[temp_size++] = {a,b};
        added.insert(key);
    }
    size = temp_size;
    FOR (i,0,temp_size) {
        ab[i] = temp_ab[i];
    }
    sort(ab, ab+size);
    cout << size << "\n";
    FOR (i,0,size) {
        auto a = ab[i].first;
        auto b = ab[i].second;
        long long x,y;
        x = 0;
        y = 0;
        FOR (j,0,i) {
            auto aj = ab[j].first;
            auto bj = ab[j].second;
            if (a < aj || b < bj) {
                continue;
            }
            if (a-aj+b-bj < a-x+b-y) {
                x = aj;
                y = bj;
            }
        }
        cout << x << " " << y << " " << a << " " << b << "\n";
    }
    return 0;
}