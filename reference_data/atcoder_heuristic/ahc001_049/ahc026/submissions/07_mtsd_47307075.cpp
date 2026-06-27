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
#define rrep(i,n) for(int i = (int)n-1; i >= 0; --i)
#define srep(i,a,b) for(int i = (int)a; i < (int)(b); ++i)
#define all(x) (x).begin(),(x).end()
#define SUM(v) accumulate(all(v), 0LL)
#define MIN(v) *min_element(all(v))
#define MAX(v) *max_element(all(v))
#define lb(c, x) distance((c).begin(), lower_bound(all(c), (x)))
#define ub(c, x) distance((c).begin(), upper_bound(all(c), (x)))
#define UNIQUE(x) sort(all(x)), x.erase(unique(all(x)), x.end())
#define SZ(c) (int)(c).size()
template<typename T>
ostream& operator << (ostream& os, vector<T>& vec) {
    os << "{";
    for (int i = 0; i<(int)vec.size(); i++) {
        os << vec[i] << (i + 1 == (int)vec.size() ? "" : ", ");
    }
    os << "}";
    return os;
}
// pair出力
template<typename T, typename U>
ostream& operator << (ostream& os, pair<T, U> pair_var) {
    os << "(" << pair_var.first << ", " << pair_var.second << ")";
    return os;
}
// map出力
template<typename T, typename U>
ostream& operator << (ostream& os, map<T, U>& map_var) {
    os << "{";
    for(auto itr = map_var.begin(); itr != map_var.end(); itr++){
        os << "(" << itr->first << ", " << itr->second << ")";
        itr++;
        if(itr != map_var.end()) os << ", ";
        itr--;
    }
    os << "}";
    return os;
}
// set 出力
template<typename T>
ostream& operator << (ostream& os, set<T>& set_var) {
    os << "{";
    for(auto itr = set_var.begin(); itr != set_var.end(); itr++){
        os << (*itr);
        ++itr;
        if(itr != set_var.end()) os << ", ";
        itr--;
    }
    os << "}";
    return os;
}
// tuple 出力
template<int N,class Tuple>
void out(ostream &os,const Tuple &t){}
template<int N,class Tuple,class H,class ...Ts>
void out(ostream &os,const Tuple &t){
    if(N)os<<", ";
    os<<get<N>(t);
    out<N+1,Tuple,Ts...>(os,t);
}
template<class ...Ts>
ostream& operator<<(ostream &os, const tuple<Ts...> &t){
    os<<"(";
    out<0,tuple<Ts...>,Ts...>(os,t);
    os<<")";
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
const string YESNO[2] = {"NO", "YES"};
const string YesNo[2] = {"No", "Yes"};
const string yesno[2] = {"no", "yes"};
void YES(bool t = 1) { cout << YESNO[t] << endl; }
void NO(bool t = 1) { YES(!t); }
void Yes(bool t = 1) { cout << YesNo[t] << endl; }
void No(bool t = 1) { Yes(!t); }
void yes(bool t = 1) { cout << yesno[t] << endl; }
void no(bool t = 1) { yes(!t); }
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
template<typename A, typename T>
std::enable_if_t<std::is_convertible<T, A>::value> fill(A& array, const T& val)
{
    array = val;
}
template<typename A, typename T>
std::enable_if_t<!std::is_convertible<T, A>::value> fill(A& array, const T& val)
{
    for (auto& a : array) {
        fill(a, val);
    }
}
template <typename T, typename S> T ceil(T x, S y) {
    assert(y);
    return (y < 0 ? ceil(-x, -y) : (x > 0 ? (x + y - 1) / y : x / y));
}
template <typename T, typename S> T floor(T x, S y) {
    assert(y);
    return (y < 0 ? floor(-x, -y) : (x > 0 ? x / y : x / y - (x % y == 0 ? 0 : 1)));
}
vector<int> iota(int n) {vector<int> a(n);iota(all(a), 0);return a;}
template <class T> T POW(T x, int n) {T res = 1;for(; n; n >>= 1, x *= x){if(n & 1) res *= x;}return res;}
ll pow2(int i) { return 1LL << i; }
int topbit(signed t) { return t == 0 ? -1 : 31 - __builtin_clz(t); }
int topbit(ll t) { return t == 0 ? -1 : 63 - __builtin_clzll(t); }
int lowbit(signed a) { return a == 0 ? 32 : __builtin_ctz(a); }
int lowbit(ll a) { return a == 0 ? 64 : __builtin_ctzll(a); }
// int allbit(int n) { return (1 << n) - 1; }
ll allbit(ll n) { return (1LL << n) - 1; }
int popcount(signed t) { return __builtin_popcount(t); }
int popcount(ll t) { return __builtin_popcountll(t); }
bool ispow2(int i) { return i && (i & -i) == i; }


template <class S> void fold_in(vector<S> &v) {}
template <typename Head, typename... Tail, class S> void fold_in(vector<S> &v, Head &&a, Tail &&...tail) {
    for(auto e : a) v.emplace_back(e);
    fold_in(v, tail...);
}
template <class S> void renumber(vector<S> &v) {}
template <typename Head, typename... Tail, class S> void renumber(vector<S> &v, Head &&a, Tail &&...tail) {
    for(auto &&e : a) e = lb(v, e);
    renumber(v, tail...);
}
template <class S, class... Args> void zip(vector<S> &head, Args &&...args) {
    vector<S> v;
    fold_in(v, head, args...);
    sort(all(v)), v.erase(unique(all(v)), v.end());
    renumber(v, head, args...);
}
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

int main(){
    INT(n,m);
    VV(int,a,m,n/m);
    vector<pair<int,int> > result;
    int score = 0;
    int current_val = 1;
    auto erase_check = [&]() -> bool {
        rep(i,m){
            if(!a[i].empty()&&a[i].back()==current_val){
                return true;
            }
        }
        return false;
    };
    auto execute = [&](int val, int box_id) -> void {
        // cerr << "execute: " << val << " " << box_id << " " << current_val << endl;
        // rep(i,m){
        //     cerr << a[i] << endl;
        // }
        result.push_back({val,box_id});
        rep(i,m){
            rep(j,a[i].size()){
                if(a[i][j] == val){
                    if(box_id == -1){
                        assert(j == a[i].size()-1);
                        a[i].pop_back();
                        current_val++;
                        return;
                    }
                    assert(box_id!=i);
                    for(int k=j;k<a[i].size();k++){
                        a[box_id].push_back(a[i][k]);
                    }
                    int CC = a[i].size();
                    for(int k=j;k<CC;k++){
                        a[i].pop_back();
                    }
                    return;
                }
            }
        }
    };
    set<int> st;
    rep(c_id,m){
        vector<pair<int,int> > col_min;
        rep(i,m)if(st.count(i)==0)col_min.push_back({MIN(a[i]),i});
        sort(all(col_min));
        int c = col_min[0].second;
        st.insert(c);
        auto b = a[c];
        zip(b);
        int M = a[c].size();
        vector<int> p(M);
        rep(i,M){
            p[b[i]] = a[c][i];
        }
        vector<int> X;
        rep(i,m){
            if(i==c)continue;
            X.push_back(i);
        }
        vector<vector<int>> B(9);
        while(!b.empty()){
            while(erase_check()){
                execute(current_val, -1);
            }
            if(p[b.back()] < current_val){
                b.pop_back();
                continue;
            }
            vector<int> x;
            x.push_back(b.back());
            b.pop_back();
            bool consecutive = true;
            while(!b.empty()){
                if(b.back() == x.back() + 1){
                    x.push_back(b.back());
                    b.pop_back();
                }else{
                    if(b.back() < x.back()){
                        if(p[b.back()]<=current_val)break;
                        consecutive = false;
                        x.push_back(b.back());
                        b.pop_back();
                    }else{
                        break;
                    }
                }
            }
            if(b.empty()&&consecutive&&x.back()==M-1){
                reverse(all(x));
                for(auto e:x)b.push_back(e);
                break;
            }
            dbg(x);
            int mi_no_cost = inf;
            int best_no_cost_box = -1;
            int best_non_empty_box = -1;
            int empty_best_box = -1;
            int ma = -1;
            int mi = inf;
            rep(i,9){
                while(!B[i].empty()&&p[B[i].back()] < current_val)B[i].pop_back();
                if(B[i].empty()){
                    if(st.count(X[i])==0&&a[X[i]].back() > p[x.back()]){
                        if(chmin(mi,a[X[i]].back())){
                            empty_best_box = i;
                        }
                    }else if(mi==inf){
                        empty_best_box = i;
                    }
                    if(st.count(X[i])){
                        if(a[X[i]].empty()||a[X[i]].back() > p[x.back()]){
                            int cc = !a[X[i]].empty()? a[X[i]].back() : inf-1;
                            if(chmin(mi_no_cost,cc)){
                                best_no_cost_box = i;
                            }
                        }
                    }
                }else{
                    if(B[i].back() == x.back() + 1){
                        best_non_empty_box = i;
                        ma = inf;
                    }
                    if(B[i].back() < x.back()){
                        if(chmax(ma,B[i].back())){
                            best_non_empty_box = i;
                        }
                    }
                }
            }
            int best_box = best_no_cost_box;
            if(best_box==-1)best_box = best_non_empty_box;
            if(best_box==-1)best_box = empty_best_box;
            if(best_box==-1){
                // 1個ずつ入れる
                reverse(all(x));
                for(auto e:x)b.push_back(e);
                x.clear();
                x.push_back(b.back());
                b.pop_back();
                bool ok = false;
                rep(i,9){
                    rep(j,B[i].size()){
                        if(B[i][j] == x.back()+1){
                            if(j==B[i].size()-1){
                                execute(p[x.back()], X[i]);
                                score += 2;
                                for(auto item:x){
                                    B[i].push_back(item);
                                }
                                ok = true;
                                break;
                            }else{
                                execute(p[B[i][j+1]], c);
                                execute(p[x.back()], X[i]);
                                score += B[i].size()- j + 1 + B[i].size()- j + 2;
                                B[i].insert(B[i].begin() + j + 1,x.back());
                                ok = true;
                                break;
                            }
                        }
                    }
                    if(ok)break;
                }
                
                continue;
            }else{
                execute(p[x.back()], X[best_box]);
                score += x.size() + 1;
                reverse(all(x));
                for(auto item:x){
                    B[best_box].push_back(item);
                }
            }
            
        }
        // cerr << B << " " << b << endl;
        int cur = M-1;
        if(!b.empty())cur = b.back()-1;
        while(cur >= 0){
            // rep(i,m)cerr << a[i] << endl;
            while(erase_check()){
                execute(current_val, -1);
            }
            if(p[cur] < current_val)break;
            rep(i,9){
                while(!B[i].empty()&&p[B[i].back()] < current_val)B[i].pop_back();
            }
            bool empty_flag = true;
            rep(i,9){
                if(!B[i].empty())empty_flag = false;
            }
            if(empty_flag)break;
            bool ok = 0;
            rep(i,B.size()){
                rrep(j,B[i].size()){
                    if(B[i][j] == cur){
                        int box = X[i];
                        int val = p[cur];
                        int pre = inf;
                        if(cur!=M-1)pre = p[cur+1];
                        if(st.count(box)==0){
                            rrep(k,a[box].size()){
                                if(a[box][k] == p[cur]){
                                    while(k>0 && a[box][k-1] <= pre&&a[box][k-1] >= val){
                                        k--;
                                        val = a[box][k];
                                    }
                                }
                            }
                        }else if(j==0){
                            // cerr << "test" << " " << a[X[i]][a[X[i]].size()-2] << " " << p[B[i][j]] << endl;
                            bool OK = true;
                            if(a[X[i]].size()<2||a[X[i]][a[X[i]].size()-2] > p[B[i][j]]){
                                cur = (int)B[i].back() - 1;
                                int cc = (int)B[i].size() - j;
                                rep(k,cc){
                                    B[i].pop_back();
                                }
                                ok = true;
                                // cerr << "OK" << endl;
                                break;
                            }
                        }
                        execute(val,c);
                        score += (int)B[i].size() - j + 1;
                        cur = (int)B[i].back() - 1;
                        int cc = (int)B[i].size() - j;
                        rep(k,cc){
                            B[i].pop_back();
                        }
                        ok = true;
                        break;
                    }
                }
                if(ok)break;
            }
            if(!ok)cur--;

        }
    }
    // cerr << score << endl;
    for(auto [x,id]:result){
        cout << x << " " << id + 1 << "\n";
    }
    srep(i,current_val,n+1){
        cout << i << " " << 0 << "\n";
    }
    return 0;
}