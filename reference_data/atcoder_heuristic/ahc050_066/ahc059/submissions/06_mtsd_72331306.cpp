# pragma GCC target("avx2")
# pragma GCC optimize("O3")
# pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;
#define inf 1000000007
#define MP make_pair
#define MT make_tuple
#define PB push_back
#define fi first
#define se second
#define rep(i,n) for(int i = 0; i < (int)(n); ++i)
#define rrep(i,n) for(int i = (int)(n)-1; i >= 0; --i)
#define srep(i,a,b) for(int i = (int)a; i < (int)(b); ++i)
#define all(x) (x).begin(),(x).end()
#define SUM(v) accumulate(all(v), 0LL)
#define MIN(v) *min_element(all(v))
#define MAX(v) *max_element(all(v))
#define lb(c, x) distance((c).begin(), lower_bound(all(c), (x)))
#define ub(c, x) distance((c).begin(), upper_bound(all(c), (x)))
#define UNIQUE(x) sort(all(x)), x.erase(unique(all(x)), x.end())
#define SZ(c) (int)(c).size()
template<class T, class U>
ostream& operator<<(ostream& os, const pair<T,U>& pair_var);
template<class T>
ostream& operator<<(ostream& os, const vector<T>& vec);
template<typename T>
ostream& operator << (ostream& os, const vector<T>& vec) {
    os << "{";
    for (int i = 0; i<(int)vec.size(); i++) {
        os << vec[i] << (i + 1 == (int)vec.size() ? "" : ", ");
    }
    os << "}";
    return os;
}
// pair出力
template<typename T, typename U>
ostream& operator << (ostream& os, const pair<T, U>& pair_var) {
    os << "(" << pair_var.first << ", " << pair_var.second << ")";
    return os;
}
#define overload2(_1, _2, name, ...) name
#define vec(type, name, ...) vector<type> name(__VA_ARGS__)
#define VEC(type, name, size)                                                                                                                                  \
    vector<type> name(size);                                                                                                                                   \
    IN(name)
#define vv(type, name, h, ...) vector<vector<type>> name(h, vector<type>(__VA_ARGS__))
#define VV(type, name, h, w)                                                                                                                                   \
    vector<vector<type>> name(h, vector<type>(w));                                                                                                             \
    IN(name)
#define vvv(type, name, h, w, ...) vector<vector<vector<type>>> name(h, vector<vector<type>>(w, vector<type>(__VA_ARGS__)))
#define vvvv(type, name, a, b, c, ...)                                                                                                                         \
    vector<vector<vector<vector<type>>>> name(a, vector<vector<vector<type>>>(b, vector<vector<type>>(c, vector<type>(__VA_ARGS__))))
#define INT(...)                                                                                                                                               \
    int __VA_ARGS__;                                                                                                                                           \
    IN(__VA_ARGS__)
#define LL(...)                                                                                                                                                \
    ll __VA_ARGS__;                                                                                                                                            \
    IN(__VA_ARGS__)
#define STR(...)                                                                                                                                               \
    string __VA_ARGS__;                                                                                                                                        \
    IN(__VA_ARGS__)
#define CHR(...)                                                                                                                                               \
    char __VA_ARGS__;                                                                                                                                          \
    IN(__VA_ARGS__)
#define DBL(...)                                                                                                                                               \
    double __VA_ARGS__;                                                                                                                                        \
    IN(__VA_ARGS__)
int scan() { return getchar(); }
void scan(int &a) { cin >> a; }
void scan(long long &a) { cin >> a; }
void scan(char &a) { cin >> a; }
void scan(double &a) { cin >> a; }
void scan(string &a) { cin >> a; }
template <class T, class S> void scan(pair<T, S> &p) { scan(p.first), scan(p.second); }
template <class T> void scan(vector<T> &);
template <class T> void scan(vector<T> &a) {
    for(auto &i : a) scan(i);
}
template <class T> void scan(T &a) { cin >> a; }
void IN() {}
template <class Head, class... Tail> void IN(Head &head, Tail &...tail) {
    scan(head);
    IN(tail...);
}
#ifdef LOCAL
void debug_out() { cerr << endl; }
template <typename Head, typename... Tail>
void debug_out(Head H, Tail... T) {
    cerr << " " << H;
    debug_out(T...);
}
#define dbg(...) \
    cerr << __LINE__ << " [" << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)
#define dump(x) cerr << __LINE__ << " " << #x << " = " << (x) << endl
#else
#define dbg(...) (void(0))
#define dump(x) (void(0))
#endif

template<class T> inline bool chmax(T &a, T b){
    if(a<b){
        a = b;
        return true;
    }
    return false;
}
template<class T> inline bool chmin(T &a, T b){
    if(a>b){
        a = b;
        return true;
    }
    return false;
}

template<typename T> class Hungarian
{
private:
    const int U, V;
    vector<vector<int> > graph;
    vector<T> dual;
    vector<int> alloc, rev_alloc, prev;
    const vector<vector<T> >& cost;
    int matching_size;
    T diff(const int i, const int j){
        return cost[i][j] - dual[i] - dual[U + j];
    }
    void init_feasible_dual(){
        for(int i = 0; i < U; ++i){
            dual[i] = 0;
            for(int j = 0; j < V; ++j){
                dual[U + j] = min(dual[U + j], cost[i][j]);
            }
        }
    }
    void construct_graph(){
        for(int i = 0; i < U; ++i){
            for(int j = 0; j < V; ++j){
                graph[i][j] = (diff(i, j) == 0 && rev_alloc[j] != i);
            }
        }
    }
    bool find_augmenting_path(const int cur, const int prv, int& pos){
        prev[cur] = prv;
        if(cur >= U){
            if(rev_alloc[cur - U] < 0) return true;
            if(find_augmenting_path(rev_alloc[cur - U], cur, pos)){
                graph[rev_alloc[cur - U]][cur - U] = 1;
                return true;
            }
        }else{
            const int MX = (alloc[cur] < 0 && pos == U) ? U : V;
            for(int i = 0; i < MX; ++i){
                if(graph[cur][i] && prev[U + i] < 0 && find_augmenting_path(U + i, cur, pos)){
                    graph[cur][i] = 0, alloc[cur] = i, rev_alloc[i] = cur;
                    return true;
                }
            }
            if(alloc[cur] < 0 && pos < U){
                graph[cur][pos] = 0, alloc[cur] = pos, rev_alloc[pos] = cur, prev[U + pos] = cur;
                return ++pos, true;
            }
        }
        return false;
    }
    void update_dual(const T delta){
        for(int i = 0; i < U; ++i) if(prev[i] >= 0) dual[i] += delta;
        for(int i = U; i < U + V; ++i) if(prev[i] >= 0) dual[i] -= delta;
    }
    void maximum_matching(bool initial=false){
        int pos = initial ? V : U;
        for(bool update = false;; update = false){
            fill(prev.begin(), prev.end(), -1);
            for(int i = 0; i < U; ++i){
                if(alloc[i] < 0 && find_augmenting_path(i, 2 * U, pos)){
                    update = true, ++matching_size;
                    break;
                }
            }
            if(!update) break;
        }
    }
    int dfs(const int cur, const int prv, vector<int>& new_ver){
        prev[cur] = prv;
        if(cur >= U){
            if(rev_alloc[cur - U] < 0) return cur;
            else return dfs(rev_alloc[cur - U], cur, new_ver);
        }else{
            new_ver.push_back(cur);
            for(int i = 0; i < V; ++i){
                if(graph[cur][i] && prev[U + i] < 0){
                    const int res = dfs(U + i, cur, new_ver);
                    if(res >= U) return res;
                }
            }
        }
        return -1;
    }
    int increase_matching(const vector<pair<int, int> >& vec, vector<int>& new_ver){
        for(const auto& e : vec){
            if(prev[e.first] < 0){
                const int res = dfs(e.first, e.second, new_ver);
                if(res >= U) return res;
            }
        }
        return -1;
    }
    void hint_increment(int cur){
        while(prev[cur] != 2 * U){
            if(cur >= U){
                graph[prev[cur]][cur - U] = 0, alloc[prev[cur]] = cur - U, rev_alloc[cur - U] = prev[cur];
            }else{
                graph[cur][prev[cur] - U] = 1;
            }
            cur = prev[cur];
        }
    }
public:
    Hungarian(const vector<vector<T> >& _cost)
         : U((int)_cost.size()), V((int)_cost[0].size()), graph(U, vector<int>(U, 1)), dual(U + V, numeric_limits<T>::max()),
            alloc(U, -1), rev_alloc(U, -1), prev(2 * U), cost{_cost}, matching_size(0){
        assert(U >= V);
    }
    pair<T, vector<int> > solve(){
        init_feasible_dual(), construct_graph();
        bool end = false;
        maximum_matching(true);
        while(matching_size < U){
            vector<pair<T, int> > cand(V, {numeric_limits<T>::max(), numeric_limits<int>::max()});
            for(int i = 0; i < U; ++i){
                if(prev[i] < 0) continue;
                for(int j = 0; j < V; ++j){
                    if(prev[U + j] >= 0) continue;
                    cand[j] = min(cand[j], {diff(i, j), i});
                }
            }
            while(true){
                T delta = numeric_limits<T>::max();
                for(int i = 0; i < V; ++i){
                    if(prev[U + i] >= 0) continue;
                    delta = min(delta, cand[i].first);
                }
                update_dual(delta);
                vector<pair<int, int> > vec;
                vector<int> new_ver;
                for(int i = 0; i < V; ++i){
                    if(prev[U + i] >= 0) continue;
                    if((cand[i].first -= delta) == 0) vec.emplace_back(U + i, cand[i].second);
                }
                int res = increase_matching(vec, new_ver);
                if(res >= U){
                    hint_increment(res);
                    if(++matching_size == U) end = true;
                    else construct_graph();
                    break;
                }else{
                    for(const int v : new_ver){
                        for(int i = 0; i < V; ++i){
                            if(prev[U + i] >= 0) continue;
                            cand[i] = min(cand[i], {diff(v, i), v});
                        }
                    }
                }
            }
            if(!end) maximum_matching();
        }
        T total_cost = 0;
        for(int i = 0; i < U; ++i){
            if(alloc[i] < V) total_cost += cost[i][alloc[i]];
            else alloc[i] = -1;
        }
        return make_pair(total_cost, alloc);
    }
};


template <class DistanceMatrix> std::vector<std::pair<int, int>> mst_edges(const DistanceMatrix &dist) {
    using T = decltype((*dist.adjacents(0).begin()).second);

    if (dist.n() <= 1) return {};

    std::vector<T> dp(dist.n(), std::numeric_limits<T>::max());
    std::vector<int> prv(dist.n(), -1);
    std::vector<int> used(dist.n());
    std::vector<std::pair<int, int>> ret(dist.n() - 1);

    for (int t = 0; t < dist.n(); ++t) {
        int x = std::min_element(dp.cbegin(), dp.cend()) - dp.cbegin();
        dp.at(x) = std::numeric_limits<T>::max();
        used.at(x) = 1;
        if (t > 0) ret.at(t - 1) = {prv.at(x), x};

        for (auto [y, len] : dist.adjacents(x)) {
            if (!used.at(y) and len < dp.at(y)) dp.at(y) = len, prv.at(y) = x;
        }
    }

    return ret;
}

// http://webhotel4.ruc.dk/~keld/research/LKH/LKH-2.0/DOC/LKH_REPORT.pdf, p.20
template <class DistanceMatrix> auto calc_lkh_alpha(const DistanceMatrix &dist) {
    using T = decltype((*dist.adjacents(0).begin()).second);

    std::vector<std::vector<int>> to(dist.n());

    for (auto [s, t] : mst_edges(dist)) {
        to.at(s).push_back(t);
        to.at(t).push_back(s);
    }

    std::vector ret(dist.n(), std::vector<T>(dist.n()));

    for (int s = 0; s < dist.n(); ++s) {
        auto rec = [&](auto &&self, int now, int prv, T hi) -> void {
            ret.at(s).at(now) = dist(s, now) - hi;
            for (int nxt : to.at(now)) {
                if (nxt == prv) continue;
                self(self, nxt, now, std::max(hi, dist(now, nxt)));
            }
        };
        rec(rec, s, -1, T());
    }

    // Determining special node for the 1-tree
    // Reference: p.26 of http://webhotel4.ruc.dk/~keld/research/LKH/LKH-2.0/DOC/LKH_REPORT.pdf
    int best_one = -1;
    T longest_2nd_nearest = T();

    for (int one = 0; one < dist.n(); ++one) {
        if (to.at(one).size() != 1) continue;
        const int ng = to.at(one).front();
        bool found = false;
        T second_nearest = T();

        for (auto [v, len] : dist.adjacents(one)) {
            if (v == ng) continue;
            if (!found) {
                found = true, second_nearest = len;
            } else if (len < second_nearest) {
                second_nearest = len;
            }
        }

        if (found and (best_one < 0 or second_nearest > longest_2nd_nearest)) best_one = one, longest_2nd_nearest = second_nearest;
    }

    if (best_one != -1) {
        for (auto [v, len] : dist.adjacents(best_one)) {
            if (v == to.at(best_one).front()) continue;
            ret.at(best_one).at(v) = ret.at(v).at(best_one) = len - longest_2nd_nearest;
        }
    }

    return ret;
}

template <class T> class dense_distance_matrix {
    int _n;
    std::vector<T> _d;

public:
    dense_distance_matrix(const std::vector<std::vector<T>> &distance_vecvec) : _n(distance_vecvec.size()) {
        _d.reserve(n() * n());
        for (const auto &vec : distance_vecvec) _d.insert(end(_d), begin(vec), end(vec));
    }

    template <class U> void apply_pi(const std::vector<U> &pi) {
        for (int i = 0; i < n(); ++i) {
            for (int j = 0; j < n(); ++j) _d.at(i * n() + j) += pi.at(i) + pi.at(j);
        }
    }

    int n() const noexcept { return _n; }

    T dist(int s, int t) const { return _d.at(s * n() + t); }
    T operator()(int s, int t) const { return dist(s, t); }

    struct adjacents_sequence {
        const dense_distance_matrix *ptr;
        int from;
        struct iterator {
            const dense_distance_matrix *ptr;
            int from;
            int to;
            iterator operator++() { return {ptr, from, to++}; }
            std::pair<int, T> operator*() const { return {to, ptr->dist(from, to)}; }
            bool operator!=(const iterator &rhs) const { return to != rhs.to or ptr != rhs.ptr or from != rhs.from; }
        };
        iterator begin() const { return iterator{ptr, from, 0}; }
        iterator end() const { return iterator{ptr, from, ptr->n()}; }
    };

    adjacents_sequence adjacents(int from) const { return {this, from}; }
};



template <class T> class csr_matrix {

    int _rows = 0;
    std::vector<int> begins;
    std::vector<std::pair<int, T>> vals;

public:
    csr_matrix() : csr_matrix({}, 0) {}

    csr_matrix(std::vector<std::tuple<int, int, T>> init, int rows) : _rows(rows), begins(rows + 1) {
        std::vector<int> degs(rows);
        for (const auto &p : init) ++degs.at(std::get<0>(p));

        for (int i = 0; i < rows; ++i) begins.at(i + 1) = begins.at(i) + degs.at(i);

        vals.resize(init.size(), std::make_pair(-1, T()));
        for (auto [i, j, w] : init) vals.at(begins.at(i + 1) - (degs.at(i)--)) = {j, w};
    }

    void apply_pi(const std::vector<T> &pi) {
        for (int i = 0; i < n(); ++i) {
            for (auto &[j, d] : adjacents(i)) d += pi.at(i) + pi.at(j);
        }
    }

    int n() const noexcept { return _rows; }

    struct adjacents_sequence {
        csr_matrix *ptr;
        int from;

        using iterator = typename std::vector<std::pair<int, T>>::iterator;
        iterator begin() { return std::next(ptr->vals.begin(), ptr->begins.at(from)); }
        iterator end() { return std::next(ptr->vals.begin(), ptr->begins.at(from + 1)); }
    };

    struct const_adjacents_sequence {
        const csr_matrix *ptr;
        const int from;

        using const_iterator = typename std::vector<std::pair<int, T>>::const_iterator;
        const_iterator begin() const { return std::next(ptr->vals.cbegin(), ptr->begins.at(from)); }
        const_iterator end() const { return std::next(ptr->vals.cbegin(), ptr->begins.at(from + 1)); }
    };

    adjacents_sequence adjacents(int from) { return {this, from}; }

    const_adjacents_sequence adjacents(int from) const { return {this, from}; }
    const_adjacents_sequence operator()(int from) const { return {this, from}; }
};

template <class DistanceMatrix>
auto build_adjacent_info(const DistanceMatrix &dist, int sz) {
    using T = decltype((*dist.adjacents(0).begin()).second);

    const std::vector<std::vector<T>> alpha = calc_lkh_alpha(dist);

    std::vector<std::tuple<int, int, T>> adjacent_edges;

    std::vector<std::tuple<T, T, int>> candidates;
    for (int i = 0; i < dist.n(); ++i) {
        candidates.clear();
        for (auto [j, d] : dist.adjacents(i)) {
            if (i != j) candidates.emplace_back(alpha.at(i).at(j), d, j);
        }

        const int final_sz = std::min<int>(sz, candidates.size());
        std::nth_element(candidates.begin(), candidates.begin() + final_sz, candidates.end());

        candidates.resize(final_sz);
        std::sort(candidates.begin(), candidates.end(), [&](const auto &l, const auto &r) { return std::get<1>(l) < std::get<1>(r); });
        for (auto [alpha, dij, j] : candidates) adjacent_edges.emplace_back(i, j, dij);
    }
    return csr_matrix(adjacent_edges, dist.n());
}

template <class DistanceMatrix, class Adjacents> struct SymmetricTSP {
    DistanceMatrix dist;
    Adjacents adjacents;
    using T = decltype((*dist.adjacents(0).begin()).second);

    struct Solution {
        T cost;
        std::vector<int> path;

        template <class OStream> friend OStream &operator<<(OStream &os, const Solution &x) {
            os << "[cost=" << x.cost << ", path=(";
            for (int i : x.path) os << i << ",";
            return os << x.path.front() << ")]";
        }
    };

    T eval(const Solution &sol) const {
        T ret = T();
        int now = sol.path.back();
        for (int nxt : sol.path) ret += dist(now, nxt), now = nxt;
        return ret;
    }

    SymmetricTSP(const DistanceMatrix &distance_matrix, const Adjacents &adjacents) : dist(distance_matrix), adjacents(adjacents) {}

    Solution nearest_neighbor(int init) const {
        if (n() == 0) return {T(), {}};
        int now = init;
        std::vector<int> ret{now}, alive(n(), 1);
        T len = T();
        ret.reserve(n());
        alive.at(now) = 0;
        while (int(ret.size()) < n()) {
            int nxt = -1;
            for (int i = 0; i < n(); ++i) {
                if (alive.at(i) and (nxt < 0 or dist(now, i) < dist(now, nxt))) nxt = i;
            }
            ret.push_back(nxt);
            alive.at(nxt) = 0;
            len += dist(now, nxt);
            now = nxt;
        }
        len += dist(ret.back(), ret.front());
        return Solution{len, ret};
    }

    void two_opt(Solution &sol) const {
        static std::vector<int> v_to_i;
        v_to_i.resize(n());
        for (int i = 0; i < n(); ++i) v_to_i.at(sol.path.at(i)) = i;
        while (true) {
            bool updated = false;
            for (int i = 0; i < n() and !updated; ++i) {
                const int u = sol.path.at(i), nxtu = sol.path.at(modn(i + 1));
                const T dunxtu = dist(u, nxtu);

                for (auto [v, duv] : adjacents(u)) {
                    if (duv >= dunxtu) break;
                    int j = v_to_i.at(v), nxtv = sol.path.at(modn(j + 1));
                    T diff = duv + dist(nxtu, nxtv) - dunxtu - dist(v, nxtv);
                    if (diff < 0) {
                        sol.cost += diff;
                        int l, r;
                        if (modn(j - i) < modn(i - j)) {
                            l = modn(i + 1), r = j;
                        } else {
                            l = modn(j + 1), r = i;
                        }
                        while (l != r) {
                            std::swap(sol.path.at(l), sol.path.at(r));
                            v_to_i.at(sol.path.at(l)) = l;
                            v_to_i.at(sol.path.at(r)) = r;
                            l = modn(l + 1);
                            if (l == r) break;
                            r = modn(r - 1);
                        }
                        updated = true;
                        break;
                    }
                }
                if (updated) break;

                for (auto [nxtv, dnxtunxtv] : adjacents(nxtu)) {
                    if (dnxtunxtv >= dunxtu) break;
                    int j = modn(v_to_i.at(nxtv) - 1), v = sol.path.at(j);
                    T diff = dist(u, v) + dnxtunxtv - dunxtu - dist(v, nxtv);
                    if (diff < 0) {
                        sol.cost += diff;
                        int l, r;
                        if (modn(j - i) < modn(i - j)) {
                            l = modn(i + 1), r = j;
                        } else {
                            l = modn(j + 1), r = i;
                        }
                        while (l != r) {
                            std::swap(sol.path.at(l), sol.path.at(r));
                            v_to_i.at(sol.path.at(l)) = l;
                            v_to_i.at(sol.path.at(r)) = r;
                            l = modn(l + 1);
                            if (l == r) break;
                            r = modn(r - 1);
                        }
                        updated = true;
                        break;
                    }
                }
            }
            if (!updated) break;
        }
    }

    bool three_opt(Solution &sol) const {
        static std::vector<int> v_to_i;
        v_to_i.resize(n());
        for (int i = 0; i < n(); ++i) v_to_i.at(sol.path.at(i)) = i;

        auto check_uvw_order = [](int u, int v, int w) {
            int i = v_to_i.at(u);
            int j = v_to_i.at(v);
            int k = v_to_i.at(w);
            if (i < j and j < k) return true;
            if (j < k and k < i) return true;
            if (k < i and i < j) return true;
            return false;
        };

        auto rev = [&](const int u, const int v) -> void {
            int l = v_to_i.at(u), r = v_to_i.at(v);
            while (l != r) {
                std::swap(sol.path.at(l), sol.path.at(r));
                l = modn(l + 1);
                if (l == r) break;
                r = modn(r - 1);
            }
        };

        int i = 0;
        for (int nseen = 0; nseen < n(); ++nseen, i = modn(i + 1)) {
            const int u = sol.path.at(modn(i - 1)), nxtu = sol.path.at(i);
            const T dunxtu = dist(u, nxtu);

            // type 1 / 3
            for (const auto &[nxtv, dunxtv] : adjacents(u)) {
                if (dunxtv >= dunxtu) break;
                const int v = sol.path.at(modn(v_to_i.at(nxtv) - 1));
                const T dvnxtv = dist(v, nxtv);

                // type 1
                for (const auto &[nxtw, dvnxtw] : adjacents(v)) {
                    if (nxtw == nxtv or nxtw == nxtu) continue;
                    if (dunxtv + dvnxtw >= dunxtu + dvnxtv) break;
                    const int w = sol.path.at(modn(v_to_i.at(nxtw) - 1));

                    if (!check_uvw_order(u, v, w)) continue;

                    const T current = dunxtu + dvnxtv + dist(w, nxtw);
                    if (T diff = dunxtv + dist(w, nxtu) + dvnxtw - current; diff < T()) {
                        sol.cost += diff;
                        rev(nxtu, v);
                        rev(nxtv, w);
                        rev(nxtw, u);
                        return true;
                    }
                }

                // type 3
                for (const auto &[w, dvw] : adjacents(v)) {
                    if (dunxtv + dvw >= dunxtu + dvnxtv) break;
                    if (!check_uvw_order(u, v, w)) continue;

                    const int nxtw = sol.path.at(modn(v_to_i.at(w) + 1));

                    const T current = dunxtu + dvnxtv + dist(w, nxtw);

                    if (T diff = dunxtv + dvw + dist(nxtu, nxtw) - current; diff < T()) {
                        sol.cost += diff;
                        rev(nxtw, u);
                        rev(nxtv, w);
                        return true;
                    }
                }
            }

            // type 2
            for (const auto &[nxtv, dnxtunxtv] : adjacents(nxtu)) {
                if (dnxtunxtv >= dunxtu) break;
                const int v = sol.path.at(modn(v_to_i.at(nxtv) - 1));
                const T dvnxtv = dist(v, nxtv);

                for (const auto &[nxtw, dvnxtw] : adjacents(v)) {
                    const int w = sol.path.at(modn(v_to_i.at(nxtw) - 1));
                    if (dnxtunxtv + dvnxtw >= dunxtu + dvnxtv) break;
                    if (!check_uvw_order(u, v, w)) continue;

                    const T current = dunxtu + dvnxtv + dist(w, nxtw);
                    if (T diff = dist(u, w) + dnxtunxtv + dvnxtw - current; diff < T()) {
                        sol.cost += diff;
                        rev(nxtu, v);
                        rev(nxtw, u);
                        return true;
                    }
                }
            }

            // type 4
            for (const auto &[v, duv] : adjacents(u)) {
                if (duv >= dunxtu) break;
                const int nxtv = sol.path.at(modn(v_to_i.at(v) + 1));
                const T dvnxtv = dist(v, nxtv);

                for (const auto &[nxtw, dnxtvnxtw] : adjacents(nxtv)) {
                    const int w = sol.path.at(modn(v_to_i.at(nxtw) - 1));
                    if (duv + dnxtvnxtw >= dunxtu + dvnxtv) break;
                    if (!check_uvw_order(u, v, w)) continue;

                    const T current = dunxtu + dvnxtv + dist(w, nxtw);
                    if (T diff = duv + dist(nxtu, w) + dnxtvnxtw - current; diff < T()) {
                        sol.cost += diff;
                        rev(nxtu, v);
                        rev(nxtv, w);
                        return true;
                    }
                }
            }
        }
        return false;
    }

    template <class Rng>
    bool double_bridge(Solution &sol, Rng &rng) const {
        if (n() < 8) return false;

        std::vector<int> &p = sol.path;
        int rand_rot = std::uniform_int_distribution<int>(0, n() - 1)(rng);
        std::rotate(p.begin(), p.begin() + rand_rot, p.end());

        static std::array<int, 3> arr;
        for (int &y : arr) y = std::uniform_int_distribution<int>(2, n() - 6)(rng);
        std::sort(arr.begin(), arr.end());
        const int i = arr.at(0), j = arr.at(1) + 2, k = arr.at(2) + 4;
        static std::array<T, 2> diffs;
        for (int d = 0; d < 2; ++d) {
            int u = p.at(n() - 1), nxtu = p.at(0);
            int v = p.at(i - 1), nxtv = p.at(i);
            int w = p.at(j - 1), nxtw = p.at(j);
            int x = p.at(k - 1), nxtx = p.at(k);
            diffs.at(d) = dist(u, nxtu) + dist(v, nxtv) + dist(w, nxtw) + dist(x, nxtx);
            if (d == 1) break;
            std::reverse(p.begin(), p.begin() + i);
            std::reverse(p.begin() + i, p.begin() + j);
            std::reverse(p.begin() + j, p.begin() + k);
            std::reverse(p.begin() + k, p.end());
        }
        sol.cost += diffs.at(1) - diffs.at(0);
        return true;
    }

    int n() const noexcept { return dist.n(); }
    int modn(int x) const noexcept {
        if (x < 0) return x + n();
        if (x >= n()) return x - n();
        return x;
    }
};

template <class T, class DistanceMatrix> std::pair<T, std::vector<int>> minimum_one_tree(const DistanceMatrix &dist, const std::vector<T> &pi) {

    assert(dist.n() > 2);
    std::vector<T> dp(dist.n(), std::numeric_limits<T>::max());
    std::vector<int> prv(dist.n(), -1);
    std::vector<int> used(dist.n());

    auto fix_v = [&](int x) -> void {
        dp.at(x) = std::numeric_limits<T>::max();
        used.at(x) = 1;
        for (auto [y, d] : dist.adjacents(x)) {
            if (used.at(y)) continue;
            if (T len = pi.at(x) + pi.at(y) + d; len < dp.at(y)) dp.at(y) = len, prv.at(y) = x;
        }
    };

    T W = T();
    std::vector<int> deg(dist.n());

    fix_v(0);
    for (int t = 0; t < dist.n() - 1; ++t) {
        int i = std::min_element(dp.cbegin(), dp.cend()) - dp.cbegin();
        W += dp.at(i);
        ++deg.at(i);
        ++deg.at(prv.at(i));
        fix_v(i);
    }

    // p.26, http://webhotel4.ruc.dk/~keld/research/LKH/LKH-2.0/DOC/LKH_REPORT.pdf
    T wlo = T();
    int ilo = -1, jlo = -1;
    for (int i = 0; i < dist.n(); ++i) {
        if (deg.at(i) != 1) continue;
        T tmp = T();
        int jtmp = -1;
        for (auto [j, d] : dist.adjacents(i)) {
            if (prv.at(i) == j or prv.at(j) == i or i == j) continue;
            if (T len = pi.at(i) + pi.at(j) + d; jtmp == -1 or tmp > len) tmp = len, jtmp = j;
        }
        if (jtmp != -1 and (ilo == -1 or wlo < tmp)) wlo = tmp, ilo = i, jlo = jtmp;
    }
    ++deg.at(ilo);
    ++deg.at(jlo);

    W += wlo - std::accumulate(pi.cbegin(), pi.cend(), T()) * 2;

    return {W, deg};
}

// http://webhotel4.ruc.dk/~keld/research/LKH/LKH-2.0/DOC/LKH_REPORT.pdf
// p.26, p.33
template <class DistanceMatrix> auto held_karp_lower_bound(const DistanceMatrix &dist) {
    using T = decltype((*dist.adjacents(0).begin()).second);

    std::vector<T> best_pi(dist.n()), pi(dist.n());
    std::vector<int> dk;
    T W;
    std::tie(W, dk) = minimum_one_tree(dist, pi);
    if (std::count(dk.cbegin(), dk.cend(), 2) == dist.n()) return std::make_pair(W, pi);

    std::vector<int> last_dk = dk;

    T BestW = W;
    const int initial_period = (dist.n() + 1) / 2;
    bool is_initial_phase = true;
    int period = initial_period;

    const auto sparse_subgraph = build_adjacent_info(dist, 50);  // p.47

    for (long long t0 = 1; t0 > 0; period /= 2, t0 /= 2) {
        for (int p = 1; t0 > 0 and p <= period; ++p) {
            for (int i = 0; i < dist.n(); ++i) {
                if (dk.at(i) != 2) pi.at(i) += t0 * ((7 * dk.at(i) + 3 * last_dk.at(i)) / 10 - 2);
                last_dk.at(i) = dk.at(i);
            }
            std::tie(W, dk) = minimum_one_tree(sparse_subgraph, pi);
            if (std::count(begin(dk), begin(dk) + dist.n(), 2) == dist.n()) return std::make_pair(W, pi);
            if (W > BestW) {
                BestW = W;
                best_pi = pi;
                if (is_initial_phase) t0 *= 2;
                if (p == period) period = std::min(period * 2, initial_period);

            } else if (is_initial_phase and p > period / 2) {
                is_initial_phase = false;
                p = 0;
                t0 = 3 * t0 / 4;
            }
        }
    }
    BestW = minimum_one_tree(dist, best_pi).first;
    return std::make_pair(BestW, best_pi);
}
// https://atcoder.jp/contests/tessoku-book/submissions/38908797

int dist(pair<int,int>a, pair<int,int>b){
    return abs(a.first-b.first) + abs(a.second-b.second);
}

vector<pair<int,int>> tsp_route(vector<vector<pair<int,int>>>& pos){
    int N = pos.size()+2;
    int S = pos.size();
    int T = pos.size()+1;
    vector dmat(N, vector<long long>(N));
    for (int i = 0; i < N-2; ++i) {
        for (int j = 0; j < N-2; ++j){
            dmat[i][j] = min(dist(pos[i][0],pos[j][0])+dist(pos[i][1],pos[j][1]),dist(pos[i][0],pos[j][1])+dist(pos[i][1],pos[j][0]));
        }
    }

    int LARGE = 1000;
    for(int i=0;i<N-2;i++){
        // S -> i
        dmat[S][i] = min(dist({0,0},pos[i][0]),dist({0,0},pos[i][1])) + LARGE;
        dmat[i][S] = dmat[S][i];
        // i -> T
        dmat[T][i] = dist(pos[i][0],pos[i][1])+ LARGE;
        dmat[i][T] = dmat[T][i];
    }

    dense_distance_matrix distance_matrix(dmat);

    auto [w, pi] = held_karp_lower_bound(distance_matrix);
    distance_matrix.apply_pi(pi);

    auto adjacent_dmat = build_adjacent_info(distance_matrix, 15);

    SymmetricTSP tsp(distance_matrix, adjacent_dmat);

    decltype(tsp)::Solution best{1LL << 60, {}};

    auto sol = tsp.nearest_neighbor(0);
    std::mt19937 mt(100);

    rep(_, 500) {
        do {
            tsp.two_opt(sol);
        } while (tsp.three_opt(sol));
        if (sol.cost < best.cost) best = sol;

        tsp.double_bridge(sol, mt);
    }

    std::rotate(best.path.begin(), std::find(best.path.begin(), best.path.end(), S), best.path.end());
    if(best.path[1]==T){
        reverse(best.path.begin()+1,best.path.end());
    }
    // for (int x : best.path) cout << x << '\n';
    // cout << best.path.front() + 1 << '\n';

    long long cost = 0;
    {
        int prv = best.path.back();
        for (int now : best.path) {
            cost += dmat.at(prv).at(now);
            prv = now;
        }
    }
    // cerr << cost - LARGE*2 << endl;
    vector<pair<int,int>> A,B;
    rrep(i,N-1){
        int v = best.path[i];
        if(i==N-2){
            A.push_back(pos[v][0]);
            B.push_back(pos[v][1]);
        }else if(i==0){
            if(dist({0,0},A.back()) > dist({0,0},B.back())){
                swap(A,B);
            }
        }else{
            if(dist(A.back(),pos[v][0]) + dist(B.back(),pos[v][1]) < dist(A.back(),pos[v][1]) + dist(B.back(),pos[v][0])){
                A.push_back(pos[v][0]);
                B.push_back(pos[v][1]);
            }else{
                B.push_back(pos[v][0]);
                A.push_back(pos[v][1]);
            }
        }
    }


    vector<pair<int,int>> route;
    reverse(all(A));
    for(auto x:A)route.push_back(x);
    for(auto x:B)route.push_back(x);
    return route;
}
pair<int,vector<pair<int,int>>> limit_K_tsp(vector<vector<pair<int,int>>> pos,int K){
    vector<vector<pair<int,int>>> far_pos;
    vector<vector<pair<int,int>>> near_pos;
    rep(i,pos.size()){
        if(dist(pos[i][0],pos[i][1]) >= K){
            far_pos.push_back(pos[i]);
        }else{
            near_pos.push_back(pos[i]);
        }
    }
    // cerr << far_pos.size() << " " << near_pos.size() << endl;
    auto route = tsp_route(far_pos);
    
    vv(int,cost, route.size()+1,near_pos.size());

    rep(i,route.size()+1){
        pair<int,int> pre_pos;
        pair<int,int> nxt_pos;
        if(i==0){
            pre_pos = {0,0};
        }else{
            pre_pos = route[i-1];
        }
        if(i==route.size()){
            rep(j,near_pos.size()){
                cost[i][j] = min(dist(pre_pos,near_pos[j][0]),dist(pre_pos,near_pos[j][1]));
            }
            continue;
        }else{
            nxt_pos = route[i];
            rep(j,near_pos.size()){
                cost[i][j] = min(
                    dist(pre_pos,near_pos[j][0]) + dist(nxt_pos,near_pos[j][1]),
                    dist(pre_pos,near_pos[j][1]) + dist(nxt_pos,near_pos[j][0])
                );
            }
        }

    }


    Hungarian<int> hg(cost);
    auto [C,alloc] = hg.solve();
    vector<pair<int,int>> result;
    rep(i,route.size()){
        if(alloc[i]!=-1){
            // near_pos[j] を挿入
            int j = alloc[i];
            pair<int,int> pre_pos;
            pair<int,int> nxt_pos;
            if(i==0){
                pre_pos = {0,0};
            }else{
                pre_pos = route[i-1];
            }
            nxt_pos = route[i];
            if(dist(pre_pos,near_pos[j][0]) + dist(nxt_pos,near_pos[j][1]) < dist(pre_pos,near_pos[j][1]) + dist(nxt_pos,near_pos[j][0])){
                result.push_back(near_pos[j][0]);
                result.push_back(near_pos[j][1]);
            }else{
                result.push_back(near_pos[j][1]);
                result.push_back(near_pos[j][0]);
            }
        }
        result.push_back(route[i]);
    }
    if(alloc[route.size()]!=-1){
        int j = alloc[route.size()];
        auto pre_pos = route[route.size()-1];
        if(dist(pre_pos,near_pos[j][0]) < dist(pre_pos,near_pos[j][1])){
            result.push_back(near_pos[j][0]);
            result.push_back(near_pos[j][1]);
        }else{
            result.push_back(near_pos[j][1]);
            result.push_back(near_pos[j][0]);
        }
    }

    int score = 0;
    score += dist({0,0},result[0]);
    rep(i,result.size()-1){
        score += dist(result[i],result[i+1]);
    }  
    // cerr << "K: " << K << endl;
    // cerr << "score: " << score << endl;
    return {score,result};
}


class XorShift {
   public:
	unsigned int x, y, z, w;
	double nL[65536];
 
	XorShift() {
		init();
	}
 
	void init() {
		x = 314159265;
		y = 358979323;
		z = 846264338;
		w = 327950288;
		double n = 1 / (double)(2 * 65536);
		for (int i = 0; i < 65536; i++) {
			nL[i] = log(((double)i / 65536) + n);
		}
	}
 
	inline unsigned int next() {
		unsigned int t = x ^ x << 11;
		x = y;
		y = z;
		z = w;
		return w = w ^ w >> 19 ^ t ^ t >> 8;
	}
 
	inline double nextLog() {
		return nL[next() & 0xFFFF];
	}
 
	inline int nextInt(int m) {
		return (int)(next() % m);
	}
 
	int nextInt(int min, int max) {
		return min + nextInt(max - min + 1);
	}
 
	inline double nextDouble() {
		return (double)next() / ((long long)1 << 32);
	}
};
XorShift rnd;


#include <sys/time.h>
inline double GetSeconds() {
    static auto st = chrono::steady_clock::now();
    auto now = chrono::steady_clock::now();
    return chrono::duration<double>(now - st).count();
}
double start_time;
int iteration = 0;
const double start_temp = 1.0;
const double end_temp = 0;
const double TIME_LIMIT = 1.95;

vector<pair<int,int>> anealing(vector<pair<int,int>> init_sol, int init_score, vector<int> parentheses){
    // ============
    // 1) 座標 -> id (x*W+y) に変換し、距離表 D[id1*NC+id2] を前計算
    // ============
    int H = 0, W = 0;
    for (auto &p: init_sol) {
        H = max(H, p.first  + 1);
        W = max(W, p.second + 1);
    }
    // (0,0) を使うので念のため
    H = max(H, 1);
    W = max(W, 1);

    const int NC = H * W;              // cell count
    const int ORG = 0;                 // (0,0) -> 0*W+0

    // 座標配列（前計算用）
    vector<uint16_t> xs(NC), ys(NC);
    for (int id = 0; id < NC; id++) {
        xs[id] = (uint16_t)(id / W);
        ys[id] = (uint16_t)(id % W);
    }

    // 距離表（フラット）
    vector<uint16_t> D((size_t)NC * NC);
    for (int i = 0; i < NC; i++) {
        int xi = xs[i], yi = ys[i];
        uint16_t *row = &D[(size_t)i * NC];
        for (int j = 0; j < NC; j++) {
            row[j] = (uint16_t)(abs(xi - (int)xs[j]) + abs(yi - (int)ys[j]));
        }
    }

    auto distID = [&](int a, int b)->int{
        return (int)D[(size_t)a * NC + b];
    };

    // sol を id 配列に
    vector<int> sol_id;
    sol_id.reserve(init_sol.size() + 2);
    for (auto &p: init_sol) sol_id.push_back(p.first * W + p.second);

    // ============
    // 2) 以降は id ベースで焼きなまし
    // ============
    vector<int> best_sol_id = sol_id;

    sol_id.reserve(sol_id.size() + 2);
    parentheses.reserve(parentheses.size() + 2);

    int current_score = init_score;
    int best_score = init_score;

    // =========================
    // 対応関係 mate[i] = i の対応括弧位置
    // =========================
    vector<int> mate;
    auto rebuild_mate = [&](){
        int n = (int)parentheses.size();
        mate.assign(n, -1);
        vector<int> st;
        st.reserve(n);
        for (int i = 0; i < n; i++) {
            if (parentheses[i] == 1) st.push_back(i);
            else {
                int j = st.back(); st.pop_back();
                mate[i] = j;
                mate[j] = i;
            }
        }
    };
    rebuild_mate();

    int accepted = 0;

    while (true) {
        iteration++;
        double now_time = GetSeconds();
        if ((iteration%10000==0)&&now_time - start_time > TIME_LIMIT) break;

        int n = (int)parentheses.size();
        if (n < 2) break;

        // =========================
        // 対応ペア(L,R)を O(1) で取得
        // =========================
        int idx = rnd.nextInt(n);
        int L, R;
        if (parentheses[idx] == 1) { L = idx; R = mate[idx]; }
        else { R = idx; L = mate[idx]; }
        if (L < 0 || R < 0) continue;
        if (L > R) swap(L, R);
        if (!(parentheses[L] == 1 && parentheses[R] == -1)) continue;

        int open_id  = sol_id[L];
        int close_id = sol_id[R];

        // ペア内反転を許可
        if (rnd.nextInt(2)) swap(open_id, close_id);

        // =========================
        // L,R を削除したときのスコア (O(1))
        // =========================
        int score_after_remove = current_score;

        auto get_id = [&](int i)->int {
            if (i < 0) return ORG;
            return sol_id[i];
        };

        if (R == L + 1) {
            int prev = get_id(L - 1);
            bool has_next = (R + 1 < n);
            if (has_next) {
                int next = sol_id[R + 1];
                score_after_remove
                    -= distID(prev, sol_id[L])
                    +  distID(sol_id[L], sol_id[R])
                    +  distID(sol_id[R], next);
                score_after_remove
                    += distID(prev, next);
            } else {
                score_after_remove
                    -= distID(prev, sol_id[L])
                    +  distID(sol_id[L], sol_id[R]);
            }
        } else {
            int prevL = get_id(L - 1);
            int nextL = sol_id[L + 1];

            score_after_remove
                -= distID(prevL, sol_id[L])
                +  distID(sol_id[L], nextL);
            score_after_remove
                += distID(prevL, nextL);

            int prevR = sol_id[R - 1];
            bool has_nextR = (R + 1 < n);
            if (has_nextR) {
                int nextR = sol_id[R + 1];
                score_after_remove
                    -= distID(prevR, sol_id[R])
                    +  distID(sol_id[R], nextR);
                score_after_remove
                    += distID(prevR, nextR);
            } else {
                score_after_remove
                    -= distID(prevR, sol_id[R]);
            }
        }

        // =========================
        // 「L,R を除いた列」を仮想参照
        // =========================
        int m = n - 2;

        auto orig_idx = [&](int k)->int {
            if (k < L) return k;
            if (k < R - 1) return k + 1;
            return k + 2;
        };
        auto removed_idx = [&](int orig)->int {
            // orig は L,R 以外を想定
            if (orig < L) return orig;
            if (orig < R) return orig - 1;
            return orig - 2;
        };

        auto par_at = [&](int k)->int { return parentheses[orig_idx(k)]; };
        auto pt_at  = [&](int k)->int { return sol_id[orig_idx(k)]; };

        int nL = rnd.nextInt(m + 1);

        auto s1_at = [&](int p, int k)->int {
            if (k < p) return pt_at(k);
            if (k == p) return open_id;
            return pt_at(k - 1);
        };

        auto delta_insert = [&](int p, int q)->int {
            int delta = 0;

            // --- '(' を境界 p に挿入 ---
            if (m == 0) {
                delta += distID(ORG, open_id);
            } else if (p == 0) {
                delta += distID(ORG, open_id) + distID(open_id, pt_at(0)) - distID(ORG, pt_at(0));
            } else if (p == m) {
                delta += distID(pt_at(m - 1), open_id);
            } else {
                delta += distID(pt_at(p - 1), open_id) + distID(open_id, pt_at(p))
                       - distID(pt_at(p - 1), pt_at(p));
            }

            // --- ')' を境界 q に挿入（open 挿入後）---
            int len1 = m + 1;
            int prev = (q == 0 ? ORG : s1_at(p, q - 1));

            if (q == len1) {
                delta += distID(prev, close_id);
            } else {
                int next = s1_at(p, q);
                delta += distID(prev, close_id) + distID(close_id, next) - distID(prev, next);
            }
            return delta;
        };

        // =========================
        // best_q 探索：'(' が来たら対応する ')' まで skip
        // =========================
        int best_new_score = INT_MAX;
        int best_q = -1;

        int level = 0;
        int b = nL;
        while (true) {
            if (level == 0) {
                int q = b + 1;
                int cand = score_after_remove + delta_insert(nL, q);
                if (cand < best_new_score) {
                    best_new_score = cand;
                    best_q = q;
                }
            }

            if (b == m) break;

            int ob = orig_idx(b);
            if (parentheses[ob] == 1) {
                // '(' -> 対応 ')' の直後へ
                int mo = mate[ob];
                int mr = removed_idx(mo);
                b = mr + 1;
                continue;
            } else {
                // ')'
                level -= 1;
                b += 1;
                if (level < 0) break;
            }
        }

        if (best_q < 0) continue;
        int new_score = best_new_score;

        // 受理判定（あなたのまま）
        double temp = start_temp + (end_temp - start_temp) * (now_time - start_time) / TIME_LIMIT;
        if (temp < 1e-12) temp = 1e-12;
        bool accept = false;
        if (new_score <= current_score) {
            accept = true;
        } else {
            // log(U) < (current - new) / T
            // rnd.nextLog() は負の値（log(0..1)）が返ってくる想定
            accept = (rnd.nextLog() < (double)(current_score - new_score) / temp);
        }
        if (accept) {
            accepted++;

            // 削除
            sol_id.erase(sol_id.begin() + R);
            parentheses.erase(parentheses.begin() + R);
            sol_id.erase(sol_id.begin() + L);
            parentheses.erase(parentheses.begin() + L);

            // 挿入
            sol_id.insert(sol_id.begin() + nL, open_id);
            parentheses.insert(parentheses.begin() + nL, 1);

            sol_id.insert(sol_id.begin() + best_q, close_id);
            parentheses.insert(parentheses.begin() + best_q, -1);

            current_score = new_score;

            if (current_score < best_score) {
                best_score = current_score;
                best_sol_id = sol_id;
            }

            // accepted のとき対応関係更新
            rebuild_mate();
        }
    }

    // cerr << "accepted: " << accepted << "\n";
    // cerr << "iteration: " << iteration << "\n";
    // cerr << "anealing best_score: " << best_score << "\n";

    // ============
    // 3) id -> pair に戻して返す
    // ============
    vector<pair<int,int>> best_sol;
    best_sol.reserve(best_sol_id.size());
    for (int id: best_sol_id) {
        best_sol.push_back({id / W, id % W});
    }
    return best_sol;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    start_time = GetSeconds();
    INT(n);
    VV(int,a,n,n);
    vector<vector<pair<int,int>>> pos(n*n/2);
    rep(i,n)rep(j,n)pos[a[i][j]].push_back({i,j});

    vector<char> res;
    vector<pair<int,int>> best_route;
    int best_score = inf;
    srep(K,5,9){
        auto [score,route] = limit_K_tsp(pos, K);
        if(chmin(best_score,score)){
            best_route = route;
        }
    }
    vector<int> pa;
    vector<int> c(n*n/2);
    rep(i,best_route.size()){
        auto [x,y] = best_route[i];
        int v = a[x][y];
        if(c[v]==0){
            pa.push_back(1);
        }else{
            pa.push_back(-1);
        }
        c[v]++;
    }
    best_route = anealing(best_route,best_score, pa);

    int cx = 0;
    int cy = 0;
    for(auto [x,y]:best_route){
        while(cx > x){
            res.push_back('U');
            cx--;
        }
        while(cx < x){
            res.push_back('D');
            cx++;
        }
        while(cy > y){
            res.push_back('L');
            cy--;
        }
        while(cy < y){
            res.push_back('R');
            cy++;
        }
        res.push_back('Z');
    }
    for(auto x:res){
        cout << x << "\n";
    }
    return 0;
}