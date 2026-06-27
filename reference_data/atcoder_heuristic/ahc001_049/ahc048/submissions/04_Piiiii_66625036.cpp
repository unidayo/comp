
#include<bits/stdc++.h>
#include<atcoder/all>
#include <immintrin.h>
using namespace std;
using namespace atcoder;
#define Yes(n) cout << ((n) ? "Yes" : "No"  ) << endl
#define print(var) std::cout<<#var<<"="<<(var)<<std::endl
#define all(a) (a).begin(), (a).end()
#define vi vector<int>
#define vvi vector<vi>
#define vvvi vector<vvi>
#define ll long long
#define vll vector<ll>
#define vvll vector<vll>
#define vvvll vector<vvll>
#define vvvvll vector<vvvll>
#define vmi vector<mint>
#define vvmi vector<vmi>
#define vvvmi vector<vvmi>
#define vvvvmi vector<vvvmi>
#define vvvvvmi vector<vvvvmi>
#define vs vector<string>
#define pii pair<int,int>
#define vpii vector<pii>
#define vvpii vector<vpii>
#define tiii tuple<int,int,int>
#define vtiii vector<tiii>
#define ull unsigned long long
#define bit(x,i)(((x)>>(i))&1)
#define inf (1<<30)
#define INF (1ll<<60)
#define X first
#define Y second
template<typename T> inline bool chmax(T &a, T b) { return ((a < b) ? (a = b, true) : (false)); }
template<typename T> inline bool chmin(T &a, T b) { return ((a > b) ? (a = b, true) : (false)); }
template<class T, class F> T nibutan(T ok, T ng, const F &f){while(abs(ok-ng)>1){T mid = (ok+ng)/2;(f(mid)?ok:ng) = mid;}return ok;}
template<class T> vector<T> digit(T x){vector<T> res; while(x>0){res.push_back(x%10); x/=10;} return res;}
template<class T> istream &operator>>(istream &is, vector<T> &vec){ for(auto &v:vec) is >> v; return is; }
template<class T> void coutvector(vector<T> x){ for(int i=0;i<(int)x.size();i++){if(i>0) cout<<" ";cout<<x[i];}cout<<endl;}

#ifdef LOCAL
#include<dump.hpp>
#else
#define cpp_dump
#endif

/* ついにrepマクロに手を染めました */
#define rep1(a)	   for(int _=0;_<(a);_++)
#define rep2(i,a)	 for(int i=0;i<(a);i++)
#define rep3(i,a,b)   for(int i=(a);i<(b);i++)
#define rep4(i,a,b,c) for(int i=(a);i<(b);i+=(c))
#define overload4(a,b,c,d,e, ...) e
#define rep(...) overload4(__VA_ARGS__,rep4,rep3,rep2,rep1)(__VA_ARGS__)
#define rrep1(n) for(int _=(n);_--;)
#define rrep2(i,n) for(int i=(n);i--;)
#define rrep3(i,a,b) for(int i=(a)-1;i>=(b);i--)
#define rrep4(i,a,b,c) for(int i=(a);i>=(b);i-=(c))
#define rrep(...) overload4(__VA_ARGS__,rrep4,rrep3,rrep2,rrep1)(__VA_ARGS__)

class Logger {
    public:
        Logger(const char* function_name) : func_name(function_name) {
            std::cerr << "Enter function: " << func_name << std::endl;
        }
        ~Logger() {
            std::cerr << "Exit function: " << func_name << std::endl;
        }
    private:
        const char* func_name;
};
#ifdef LOCAL
#define LOG_FUNC Logger log(__FUNCTION__);
#else
#define LOG_FUNC
#endif

template <class T> class persistent_stack {
    public:
    using value_type = T;
    using const_reference = const value_type &;

    private:
    class node_type;
    using node_ptr = ::std::shared_ptr<const node_type>;

    template <class... Args> static node_ptr make_node(Args &&... args) {
        return ::std::make_shared<const node_type>(::std::forward<Args>(args)...);
    }

    class node_type {
    public:
        node_ptr next;
        value_type value;

        template <class... Args>
        explicit node_type(const node_ptr &ptr, Args &&... args)
            : next(ptr), value(::std::forward<Args>(args)...) {}
    };

    node_ptr root;

    template <class... Args> persistent_stack push_p(Args &&... args) const {
        return persistent_stack(make_node(root, ::std::forward<Args>(args)...));
    }
    explicit persistent_stack(node_ptr &&r) : root(::std::move(r)) {}
    explicit persistent_stack(const node_ptr &r) : root(r) {}

    public:
    constexpr persistent_stack() noexcept : root() {}

    bool empty() const noexcept { return !static_cast<bool>(root); }

    const_reference top() const noexcept {
        assert(("called persistent_stack::top on an empty stack", !empty()));
        return root->value;
    }

    persistent_stack push(const value_type &x) const { return push_p(x); }
    persistent_stack push(value_type &&x) const { return push_p(::std::move(x)); }
    template <class... Args> persistent_stack emplace(Args &&... args) const {
        return push_p(::std::forward<Args>(args)...);
    }
    persistent_stack pop() const {
        assert(("called persistent_stack::pop on an empty stack", !empty()));
        return persistent_stack(root->next);
    }
};

static uint32_t xor128(void){
    static uint32_t x=123456789,y=362436069,z=521288629,w=88675123;
    uint32_t t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}
inline int randrange(int a){return (uint64_t(xor128()) * a >> 32); }
inline int randrange(int a, int b){ return randrange(b-a)+a; }
constexpr double U32MAX = (double)std::numeric_limits<uint32_t>::max();
inline double randDouble(double a,double b){return a+(b-a)*xor128()/U32MAX;}
template<typename T> T& choice(vector<T>& v){ return v[randrange(v.size())]; }

inline double get_time() {
    #ifdef ONLINE_JUDGE
    timeval tv; gettimeofday(&tv, NULL);
    return tv.tv_sec * 1e3 + tv.tv_usec * 1e-3;
    #else
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    #endif
}
double start_time = get_time();
inline double elapsed(){ return get_time() - start_time; }

#ifdef LOCAL
struct Timer{
    string s;
    double save_time;
    Timer(string s):s(s){ save_time = get_time(); }
    ~Timer(){
        #ifndef ONLINE_JUDGE
        cerr << s << " elapsed time : " << get_time() - save_time << " ms" << endl;
        #endif
    }
    double elapsed(){return get_time()-save_time;}
};
#endif

const int N = 20; // パレットのサイズがN*N
int K; // チューブの種類
const int H = 1000; // 作成すべき色
int T; // 最大操作可能ターン
int D; // 1グラムの絵具のコスト

using Color = array<double,3>; // (C,M,Y)

inline Color operator+(const Color& a, const Color& b){
    return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}
inline Color operator-(const Color& a, const Color& b){
    return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}
inline Color operator*(const Color& a, double b){
    return {a[0] * b, a[1] * b, a[2] * b};
}
inline Color operator/(const Color& a, double b){
    return {a[0] / b, a[1] / b, a[2] / b};
}
inline double dot(const Color& a, const Color& b){
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
inline double norm2(const Color& a){
    return a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
}
#ifdef LOCAL
inline ostream& operator<<(ostream& os, const Color& c){
    return os << "(" << c[0] << "," << c[1] << "," << c[2] << ")";
}
#endif

namespace hull{
const double EPS = 1e-9;

/* ───── 基本演算 ───── */
inline Color add(const Color &a, const Color &b) { return {a[0] + b[0], a[1] + b[1], a[2] + b[2]}; }
inline Color sub(const Color &a, const Color &b) { return {a[0] - b[0], a[1] - b[1], a[2] - b[2]}; }
inline Color mul(const Color &a, double k) { return {a[0] * k, a[1] * k, a[2] * k}; }
inline Color cross(const Color &a, const Color &b) {
    return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
}
inline double dot(const Color &a, const Color &b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }
inline double norm2(const Color &a) { return dot(a, a); }
inline double norm(const Color &a) { return std::sqrt(norm2(a)); }

/* ───── 凸包三角面 ───── */
struct Face {
    int a, b, c;
    Color n;
};

vector<Face> convex_hull_faces(const vector<Color> &P, double eps = EPS) {
    int n = (int)P.size();
    Color centroid{0, 0, 0};
    for (auto &p : P) centroid = add(centroid, p);
    centroid = mul(centroid, 1.0 / n);

    set<tuple<int, int, int>> uniq;
    vector<Face> faces;

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            for (int k = j + 1; k < n; k++) {
                int a = i, b = j, c = k;
                Color nvec = cross(sub(P[b], P[a]), sub(P[c], P[a]));
                if (norm2(nvec) < eps) continue; // 退化

                bool pos = false, neg = false;
                for (int m = 0; m < n; m++) {
                    if (m == a || m == b || m == c) continue;
                    double s = dot(nvec, sub(P[m], P[a]));
                    if (s > eps)
                        pos = true;
                    else if (s < -eps)
                        neg = true;
                    if (pos && neg) break;
                }
                if (pos && neg) continue; // 面にならない

                Color fcent = mul(add(add(P[a], P[b]), P[c]), 1.0 / 3);
                if (dot(nvec, sub(fcent, centroid)) < 0) {
                    swap(b, c);
                    nvec = mul(nvec, -1);
                }

                auto key = tuple{min({a, b, c}), a + b + c - min({a, b, c}) - max({a, b, c}), max({a, b, c})};
                if (uniq.insert(key).second) faces.push_back({a, b, c, nvec});
            }
    return faces;
}

/* ───── 点包含判定 ───── */
bool inside_convex_hull(const vector<Color> &P, const Color &q, const vector<Face> &faces, double eps = EPS) {
    for (auto &f : faces)
        if (dot(f.n, sub(q, P[f.a])) > eps) return false;
    return true;
}

/* ───── 距離計算用の補助 ───── */
double dist2_segment(const Color &p, const Color &p0, const Color &p1) {
    Color v = sub(p1, p0);
    double len2 = norm2(v);
    if (len2 < EPS) return norm2(sub(p, p0));
    double t = dot(sub(p, p0), v) / len2;
    t = std::clamp(t, 0.0, 1.0);
    Color proj = add(p0, mul(v, t));
    return norm2(sub(p, proj));
}

double dist_point_triangle(const Color &p, const Color &a, const Color &b, const Color &c) {
    Color n = cross(sub(b, a), sub(c, a));
    double nlen2 = norm2(n);
    if (nlen2 < EPS) {
        return std::sqrt(std::min({norm2(sub(p, a)), norm2(sub(p, b)), norm2(sub(p, c))}));
    }
    double dist_signed = dot(n, sub(p, a)) / std::sqrt(nlen2);
    Color proj = sub(p, mul(n, dist_signed / std::sqrt(nlen2)));

    Color c0 = cross(sub(b, a), sub(proj, a));
    Color c1 = cross(sub(c, b), sub(proj, b));
    Color c2 = cross(sub(a, c), sub(proj, c));
    bool inside = (dot(c0, n) >= -EPS && dot(c1, n) >= -EPS && dot(c2, n) >= -EPS);

    if (inside) return std::fabs(dist_signed);

    double d2 = std::min({dist2_segment(p, a, b), dist2_segment(p, b, c), dist2_segment(p, c, a)});
    return std::sqrt(d2);
}

/* ───── 点→凸包距離 ───── */
double distance_to_hull(const vector<Color> &P, const vector<Face> &faces, const Color &q) {
    bool is_inside = true;
    for (auto &f : faces) {
        if (dot(f.n, sub(q, P[f.a])) > EPS) {
            is_inside = false;
            break;
        }
    }
    if (is_inside) return 0.0;

    double best = std::numeric_limits<double>::infinity();
    for (auto &f : faces) {
        best = std::min(best, dist_point_triangle(q, P[f.a], P[f.b], P[f.c]));
    }
    return best;
}
} // namespace hull

Color mix(const Color& a, const Color& b, double ratio = 0.5){
    Color mixed_color;
    mixed_color[0] = a[0] * ratio + b[0] * (1-ratio);
    mixed_color[1] = a[1] * ratio + b[1] * (1-ratio);
    mixed_color[2] = a[2] * ratio + b[2] * (1-ratio);
    return mixed_color;
}
// 色の差を計算
double diff(const Color& a, const Color& b){
    return hypot(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

vector<Color> tube; // チューブの色
Color target[H]; // ターゲットの色

struct Operation{
        int type; // 1: add, 2: give, 3: dump, 4: flip_wall
        vi args; // 操作に必要な引数
};
struct Ops{
    bool K_ERASE = true; // チューブIDを削るかどうか
    vvi v_wall, h_wall;
    vector<Operation> ops; // 操作の履歴
    void init_wall(const vvi &vertical_wall, const vvi &horizontal_wall){
        v_wall = vertical_wall;
        h_wall = horizontal_wall;
    }
    void add(int i, int j, int tube_id){
        ops.push_back({1, {i, j, tube_id}});
    }
    void give(int i, int j){
        ops.push_back({2, {i, j}});
    }
    void dump(int i, int j){
        ops.push_back({3, {i, j}});
    }
    void flip_wall(int i1, int j1, int i2, int j2){
        ops.push_back({4, {i1, j1, i2, j2}});
    }
};

struct IO{
    int true_K;
    vector<Color> true_tube;
    vector<int> used_id; // 凸包を成すチューブID
    IO(){
        int _n,_m;
        cin>> _n >> true_K >> _m >> T >> D;
        true_tube.resize(true_K);
        rep(i,true_K){
            cin >> true_tube[i][0] >> true_tube[i][1] >> true_tube[i][2];
        }
        rep(i,H){
            cin >> target[i][0] >> target[i][1] >> target[i][2];
        }
        auto faces = hull::convex_hull_faces(true_tube);
        for(const auto& f : faces){
            used_id.push_back(f.a);
            used_id.push_back(f.b);
            used_id.push_back(f.c);
        }
        sort(all(used_id));
        used_id.erase(unique(all(used_id)), used_id.end());
        K = used_id.size();
        tube.resize(K);
        rep(i,K) tube[i] = true_tube[used_id[i]];
    }

    // マス(i,j)にtube_idを出す
    void add(int i, int j, int tube_id){
        cout << 1 << " " << i << " " << j << " " << tube_id << "\n";
    }
    // マス(i,j)の絵具を画伯に渡す
    void give(int i, int j){
        cout << 2 << " " << i << " " << j << "\n";
    }
    void dump(int i, int j){
        cout << 3 << " " << i << " " << j << "\n";
    }
    void flip_wall(int i1, int j1, int i2, int j2){
        cout << 4 << " " << i1 << " " << j1 << " " << i2 << " " << j2 << "\n";
    }

    void init_wall(vvi &vertical_wall, vvi &horizontal_wall){
        rep(i,N){
            rep(j,N-1){
                if(j>0) cout << " ";
                cout << vertical_wall[i][j];
            }
            cout << "\n";
        }
        rep(i,N-1){
            rep(j,N){
                if(j>0) cout << " ";
                cout << horizontal_wall[i][j];
            }
            cout << "\n";
        }
        return;
    }

    void out(Ops &ops) const {
        if(!ops.v_wall.empty() && !ops.h_wall.empty()){
            rep(i,N){
            rep(j,N-1){
                if(j>0) cout << " ";
                cout << ops.v_wall[i][j];
            }
            cout << "\n";
            }
            rep(i,N-1){
                rep(j,N){
                    if(j>0) cout << " ";
                    cout << ops.h_wall[i][j];
                }
                cout << "\n";
            }
        }
        for(const auto& op : ops.ops){
            cout << op.type;
            if(ops.K_ERASE && op.type==1){
                cout << " " << op.args[0] << " " << op.args[1] << " " << used_id[op.args[2]];
            }
            else{
                for(int arg : op.args){
                    cout << " " << arg;
                }
            }
            cout << "\n";
        }
    }
} io;


namespace pruning_search_ratio {
Color operator+(const Color& a, const Color& b) {
    return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}
Color operator-(const Color& a, const Color& b) {
    return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}
Color operator*(const Color& v, double s) {
    return {v[0] * s, v[1] * s, v[2] * s};
}

using Hull = pair<vector<Color>,vector<hull::Face>>;
const vector<Hull>& hulls(){
    static const vector<Hull> hulls = []{
        vector<Color> tmp{tube[K-1], tube[K-2], tube[K-3]};
        vector<Hull> hulls(K-3);
        for(int i=K-4; i>=0; i--){
            tmp.push_back(tube[i]);
            hulls[i] = {tmp, hull::convex_hull_faces(tmp)};
        }
        return hulls;
    }();  
    return hulls;
}

pair<double,vector<int>> ratio_search(const Color& target, int S, double END_TIME=1e100, int NONZERO=K) {
    vi w(K);        // 現在の重み
    vi best_w(K);        // ベストを保持
    double best_err = 1e100; // ベストの誤差
    
    vector<Color> min_tube(K);
    vector<Color> max_tube(K);
    min_tube[K-1] = max_tube[K-1] = tube[K-1];
    rrep(i,K-1){
        rep(j,3){
            min_tube[i][j] = min(min_tube[i+1][j], tube[i][j]);
            max_tube[i][j] = max(max_tube[i+1][j], tube[i][j]);
        }
    }

    double inv_S = 1.0 / S;
    int dfs_cnt = 0;
    int nonzero_cnt = 0; // 非ゼロのチューブの数
    auto dfs = [&](auto dfs, int idx, int remain, Color mix) -> void {
        if(nonzero_cnt >= NONZERO && remain > 0) return;
        if(elapsed() > END_TIME && best_err <= 0.1) return; // 時間切れ
        dfs_cnt++;
        // 枝刈り
        if(idx<K-1){
            Color mn = mix + min_tube[idx]*(double)remain*inv_S;
            Color mx = mix + max_tube[idx]*(double)remain*inv_S;

            double mn_err[3];
            rep(i,3){
                mn_err[i] = (mn[i]<=target[i] && target[i]<=mx[i]) ? 0.0 : min(abs(mn[i]-target[i]), abs(mx[i]-target[i]));
            }
            if(hypot(mn_err[0], mn_err[1], mn_err[2])>best_err) return;
        }
        // 凸包の枝刈り
        if(idx < K-3){
            const auto& [P,faces] = hulls()[idx];
            Color v = target-mix;
            v = v * ((double)S / remain);
            double _d = hull::distance_to_hull(P,faces,v);
            _d *= remain * inv_S;
            if(_d > best_err) return; // 枝刈り
        }

        // end
        if (idx == K-1){
            w[idx] = remain;
            mix = mix + tube[idx] * (static_cast<double>(remain)/S);
            double err = diff(target, mix);
            if (err < best_err) {
                best_err = err;
                for (int i = 0; i < K; ++i) best_w[i] = w[i];
            }
        }
        // nxt
        else{
            for(int i=0;i<=remain;i++){
                w[idx] = i;
                nonzero_cnt += (i>0);
                dfs(dfs,idx+1, remain-i, mix + tube[idx] * (static_cast<double>(i)/S));
                nonzero_cnt -= (i>0);
            }
        }
        return;
    }; dfs(dfs, 0, S, {0,0,0});

    //cpp_dump(dfs_cnt);
    return {best_err,best_w};
}

void experiment(){
    double err_sum = 0.0;
    double dfs_start_time = elapsed();
    rep(t,1000){
        double end_time = dfs_start_time + (t+1)*2;
        end_time = inf;
        auto [err,w] = ratio_search(target[t],400/(2*K),end_time);
        err_sum += err;
    }
    cpp_dump(err_sum * 10000);
    cpp_dump(elapsed());
    cerr << "Error cost : " << (int)(err_sum * 10000) << "\n";
    cerr << "Elapsed time: " << elapsed() << " ms\n";
}
} // namespace pruning_search_ratio


const vector<vvpii> valid_path = {
    {},
    {},
    {},
    {}
    // K=4
    ,
    {{{0, 0},   {1, 0},   {2, 0},   {3, 0},   {4, 0},   {5, 0},   {6, 0},
      {7, 0},   {8, 0},   {9, 0},   {10, 0},  {11, 0},  {12, 0},  {13, 0},
      {14, 0},  {15, 0},  {16, 0},  {17, 0},  {18, 0},  {19, 0},  {19, 1},
      {19, 2},  {19, 3},  {19, 4},  {19, 5},  {19, 6},  {19, 7},  {18, 7},
      {17, 7},  {16, 7},  {15, 7},  {14, 7},  {13, 7},  {12, 7},  {11, 7},
      {10, 7},  {9, 7},   {8, 7},   {7, 7},   {6, 7},   {5, 7},   {4, 7},
      {3, 7},   {3, 8},   {4, 8},   {5, 8},   {6, 8},   {7, 8},   {8, 8},
      {9, 8},   {10, 8},  {11, 8},  {12, 8},  {13, 8},  {14, 8},  {15, 8},
      {16, 8},  {17, 8},  {18, 8},  {19, 8},  {19, 9},  {19, 10}, {19, 11},
      {19, 12}, {19, 13}, {19, 14}, {19, 15}, {18, 15}, {17, 15}, {16, 15},
      {15, 15}, {14, 15}, {13, 15}, {12, 15}, {11, 15}, {10, 15}, {9, 15},
      {8, 15},  {7, 15},  {6, 15},  {5, 15},  {4, 15},  {3, 15},  {3, 16},
      {4, 16},  {5, 16},  {6, 16},  {7, 16},  {8, 16},  {9, 16},  {10, 16},
      {11, 16}, {12, 16}, {13, 16}, {14, 16}, {15, 16}, {16, 16}, {17, 16},
      {18, 16}, {19, 16}},
     {{0, 1},   {1, 1},   {2, 1},   {3, 1},   {4, 1},   {5, 1},   {6, 1},
      {7, 1},   {8, 1},   {9, 1},   {10, 1},  {11, 1},  {12, 1},  {13, 1},
      {14, 1},  {15, 1},  {16, 1},  {17, 1},  {18, 1},  {18, 2},  {18, 3},
      {18, 4},  {18, 5},  {18, 6},  {17, 6},  {16, 6},  {15, 6},  {14, 6},
      {13, 6},  {12, 6},  {11, 6},  {10, 6},  {9, 6},   {8, 6},   {7, 6},
      {6, 6},   {5, 6},   {4, 6},   {3, 6},   {2, 6},   {2, 7},   {2, 8},
      {2, 9},   {3, 9},   {4, 9},   {5, 9},   {6, 9},   {7, 9},   {8, 9},
      {9, 9},   {10, 9},  {11, 9},  {12, 9},  {13, 9},  {14, 9},  {15, 9},
      {16, 9},  {17, 9},  {18, 9},  {18, 10}, {18, 11}, {18, 12}, {18, 13},
      {18, 14}, {17, 14}, {16, 14}, {15, 14}, {14, 14}, {13, 14}, {12, 14},
      {11, 14}, {10, 14}, {9, 14},  {8, 14},  {7, 14},  {6, 14},  {5, 14},
      {4, 14},  {3, 14},  {2, 14},  {2, 15},  {2, 16},  {2, 17},  {3, 17},
      {4, 17},  {5, 17},  {6, 17},  {7, 17},  {8, 17},  {9, 17},  {10, 17},
      {11, 17}, {12, 17}, {13, 17}, {14, 17}, {15, 17}, {16, 17}, {17, 17},
      {18, 17}, {19, 17}},
     {{0, 2},   {1, 2},   {2, 2},   {3, 2},   {4, 2},   {5, 2},   {6, 2},
      {7, 2},   {8, 2},   {9, 2},   {10, 2},  {11, 2},  {12, 2},  {13, 2},
      {14, 2},  {15, 2},  {16, 2},  {17, 2},  {17, 3},  {17, 4},  {17, 5},
      {16, 5},  {15, 5},  {14, 5},  {13, 5},  {12, 5},  {11, 5},  {10, 5},
      {9, 5},   {8, 5},   {7, 5},   {6, 5},   {5, 5},   {4, 5},   {3, 5},
      {2, 5},   {1, 5},   {1, 6},   {1, 7},   {1, 8},   {1, 9},   {1, 10},
      {2, 10},  {3, 10},  {4, 10},  {5, 10},  {6, 10},  {7, 10},  {8, 10},
      {9, 10},  {10, 10}, {11, 10}, {12, 10}, {13, 10}, {14, 10}, {15, 10},
      {16, 10}, {17, 10}, {17, 11}, {17, 12}, {17, 13}, {16, 13}, {15, 13},
      {14, 13}, {13, 13}, {12, 13}, {11, 13}, {10, 13}, {9, 13},  {8, 13},
      {7, 13},  {6, 13},  {5, 13},  {4, 13},  {3, 13},  {2, 13},  {1, 13},
      {1, 14},  {1, 15},  {1, 16},  {1, 17},  {1, 18},  {2, 18},  {3, 18},
      {4, 18},  {5, 18},  {6, 18},  {7, 18},  {8, 18},  {9, 18},  {10, 18},
      {11, 18}, {12, 18}, {13, 18}, {14, 18}, {15, 18}, {16, 18}, {17, 18},
      {18, 18}, {19, 18}},
     {{0, 3},   {1, 3},   {2, 3},   {3, 3},   {4, 3},   {5, 3},   {6, 3},
      {7, 3},   {8, 3},   {9, 3},   {10, 3},  {11, 3},  {12, 3},  {13, 3},
      {14, 3},  {15, 3},  {16, 3},  {16, 4},  {15, 4},  {14, 4},  {13, 4},
      {12, 4},  {11, 4},  {10, 4},  {9, 4},   {8, 4},   {7, 4},   {6, 4},
      {5, 4},   {4, 4},   {3, 4},   {2, 4},   {1, 4},   {0, 4},   {0, 5},
      {0, 6},   {0, 7},   {0, 8},   {0, 9},   {0, 10},  {0, 11},  {1, 11},
      {2, 11},  {3, 11},  {4, 11},  {5, 11},  {6, 11},  {7, 11},  {8, 11},
      {9, 11},  {10, 11}, {11, 11}, {12, 11}, {13, 11}, {14, 11}, {15, 11},
      {16, 11}, {16, 12}, {15, 12}, {14, 12}, {13, 12}, {12, 12}, {11, 12},
      {10, 12}, {9, 12},  {8, 12},  {7, 12},  {6, 12},  {5, 12},  {4, 12},
      {3, 12},  {2, 12},  {1, 12},  {0, 12},  {0, 13},  {0, 14},  {0, 15},
      {0, 16},  {0, 17},  {0, 18},  {0, 19},  {1, 19},  {2, 19},  {3, 19},
      {4, 19},  {5, 19},  {6, 19},  {7, 19},  {8, 19},  {9, 19},  {10, 19},
      {11, 19}, {12, 19}, {13, 19}, {14, 19}, {15, 19}, {16, 19}, {17, 19},
      {18, 19}, {19, 19}}}
    // K=5
    ,
    {{{0, 0},   {1, 0},   {2, 0},   {3, 0},   {4, 0},   {5, 0},   {6, 0},
      {7, 0},   {8, 0},   {9, 0},   {10, 0},  {11, 0},  {12, 0},  {13, 0},
      {14, 0},  {15, 0},  {16, 0},  {17, 0},  {18, 0},  {19, 0},  {19, 1},
      {19, 2},  {19, 3},  {19, 4},  {19, 5},  {19, 6},  {19, 7},  {19, 8},
      {19, 9},  {18, 9},  {17, 9},  {16, 9},  {15, 9},  {14, 9},  {13, 9},
      {12, 9},  {11, 9},  {10, 9},  {9, 9},   {8, 9},   {7, 9},   {6, 9},
      {5, 9},   {4, 9},   {4, 10},  {5, 10},  {6, 10},  {7, 10},  {8, 10},
      {9, 10},  {10, 10}, {11, 10}, {12, 10}, {13, 10}, {14, 10}, {15, 10},
      {16, 10}, {17, 10}, {18, 10}, {19, 10}, {19, 11}, {19, 12}, {19, 13},
      {19, 14}, {19, 15}, {19, 16}, {19, 17}, {19, 18}, {19, 19}, {18, 19},
      {17, 19}, {16, 19}, {15, 19}, {14, 19}, {13, 19}, {12, 19}, {11, 19},
      {10, 19}, {9, 19},  {8, 19}},
     {{0, 1},   {1, 1},   {2, 1},   {3, 1},   {4, 1},   {5, 1},   {6, 1},
      {7, 1},   {8, 1},   {9, 1},   {10, 1},  {11, 1},  {12, 1},  {13, 1},
      {14, 1},  {15, 1},  {16, 1},  {17, 1},  {18, 1},  {18, 2},  {18, 3},
      {18, 4},  {18, 5},  {18, 6},  {18, 7},  {18, 8},  {17, 8},  {16, 8},
      {15, 8},  {14, 8},  {13, 8},  {12, 8},  {11, 8},  {10, 8},  {9, 8},
      {8, 8},   {7, 8},   {6, 8},   {5, 8},   {4, 8},   {3, 8},   {3, 9},
      {3, 10},  {3, 11},  {4, 11},  {5, 11},  {6, 11},  {7, 11},  {8, 11},
      {9, 11},  {10, 11}, {11, 11}, {12, 11}, {13, 11}, {14, 11}, {15, 11},
      {16, 11}, {17, 11}, {18, 11}, {18, 12}, {18, 13}, {18, 14}, {18, 15},
      {18, 16}, {18, 17}, {18, 18}, {17, 18}, {16, 18}, {15, 18}, {14, 18},
      {13, 18}, {12, 18}, {11, 18}, {10, 18}, {9, 18},  {8, 18},  {7, 18},
      {7, 19},  {6, 19},  {5, 19}},
     {{0, 2},   {1, 2},   {2, 2},   {3, 2},   {4, 2},   {5, 2},   {6, 2},
      {7, 2},   {8, 2},   {9, 2},   {10, 2},  {11, 2},  {12, 2},  {13, 2},
      {14, 2},  {15, 2},  {16, 2},  {17, 2},  {17, 3},  {17, 4},  {17, 5},
      {17, 6},  {17, 7},  {16, 7},  {15, 7},  {14, 7},  {13, 7},  {12, 7},
      {11, 7},  {10, 7},  {9, 7},   {8, 7},   {7, 7},   {6, 7},   {5, 7},
      {4, 7},   {3, 7},   {2, 7},   {2, 8},   {2, 9},   {2, 10},  {2, 11},
      {2, 12},  {3, 12},  {4, 12},  {5, 12},  {6, 12},  {7, 12},  {8, 12},
      {9, 12},  {10, 12}, {11, 12}, {12, 12}, {13, 12}, {14, 12}, {15, 12},
      {16, 12}, {17, 12}, {17, 13}, {17, 14}, {17, 15}, {17, 16}, {17, 17},
      {16, 17}, {15, 17}, {14, 17}, {13, 17}, {12, 17}, {11, 17}, {10, 17},
      {9, 17},  {8, 17},  {7, 17},  {6, 17},  {6, 18},  {5, 18},  {4, 18},
      {4, 19},  {3, 19},  {2, 19}},
     {{0, 3},   {1, 3},   {2, 3},   {3, 3},   {4, 3},   {5, 3},   {6, 3},
      {7, 3},   {8, 3},   {9, 3},   {10, 3},  {11, 3},  {12, 3},  {13, 3},
      {14, 3},  {15, 3},  {16, 3},  {16, 4},  {16, 5},  {16, 6},  {15, 6},
      {14, 6},  {13, 6},  {12, 6},  {11, 6},  {10, 6},  {9, 6},   {8, 6},
      {7, 6},   {6, 6},   {5, 6},   {4, 6},   {3, 6},   {2, 6},   {1, 6},
      {1, 7},   {1, 8},   {1, 9},   {1, 10},  {1, 11},  {1, 12},  {1, 13},
      {2, 13},  {3, 13},  {4, 13},  {5, 13},  {6, 13},  {7, 13},  {8, 13},
      {9, 13},  {10, 13}, {11, 13}, {12, 13}, {13, 13}, {14, 13}, {15, 13},
      {16, 13}, {16, 14}, {16, 15}, {16, 16}, {15, 16}, {14, 16}, {13, 16},
      {12, 16}, {11, 16}, {10, 16}, {9, 16},  {8, 16},  {7, 16},  {6, 16},
      {5, 16},  {5, 17},  {4, 17},  {3, 17},  {3, 18},  {2, 18},  {1, 18},
      {1, 19},  {0, 19},  {0, 18}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},   {5, 4},   {6, 4},
      {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4},  {12, 4},  {13, 4},
      {14, 4},  {15, 4},  {15, 5},  {14, 5},  {13, 5},  {12, 5},  {11, 5},
      {10, 5},  {9, 5},   {8, 5},   {7, 5},   {6, 5},   {5, 5},   {4, 5},
      {3, 5},   {2, 5},   {1, 5},   {0, 5},   {0, 6},   {0, 7},   {0, 8},
      {0, 9},   {0, 10},  {0, 11},  {0, 12},  {0, 13},  {0, 14},  {1, 14},
      {2, 14},  {3, 14},  {4, 14},  {5, 14},  {6, 14},  {7, 14},  {8, 14},
      {9, 14},  {10, 14}, {11, 14}, {12, 14}, {13, 14}, {14, 14}, {15, 14},
      {15, 15}, {14, 15}, {13, 15}, {12, 15}, {11, 15}, {10, 15}, {9, 15},
      {8, 15},  {7, 15},  {6, 15},  {5, 15},  {4, 15},  {4, 16},  {3, 16},
      {3, 15},  {2, 15},  {2, 16},  {2, 17},  {1, 17},  {0, 17},  {0, 16},
      {1, 16},  {1, 15},  {0, 15}}}
    // K=6
    ,
    {{{0, 0},   {1, 0},   {2, 0},   {3, 0},   {4, 0},   {5, 0},   {6, 0},
      {7, 0},   {8, 0},   {9, 0},   {10, 0},  {11, 0},  {12, 0},  {13, 0},
      {14, 0},  {15, 0},  {16, 0},  {17, 0},  {18, 0},  {19, 0},  {19, 1},
      {19, 2},  {19, 3},  {19, 4},  {19, 5},  {19, 6},  {19, 7},  {19, 8},
      {19, 9},  {19, 10}, {19, 11}, {18, 11}, {17, 11}, {16, 11}, {15, 11},
      {14, 11}, {13, 11}, {12, 11}, {11, 11}, {10, 11}, {9, 11},  {8, 11},
      {7, 11},  {6, 11},  {5, 11},  {5, 12},  {6, 12},  {7, 12},  {8, 12},
      {9, 12},  {10, 12}, {11, 12}, {12, 12}, {13, 12}, {14, 12}, {15, 12},
      {16, 12}, {17, 12}, {18, 12}, {19, 12}, {19, 13}, {19, 14}, {19, 15},
      {19, 16}, {19, 17}, {19, 18}},
     {{0, 1},   {1, 1},   {2, 1},   {3, 1},   {4, 1},   {5, 1},   {6, 1},
      {7, 1},   {8, 1},   {9, 1},   {10, 1},  {11, 1},  {12, 1},  {13, 1},
      {14, 1},  {15, 1},  {16, 1},  {17, 1},  {18, 1},  {18, 2},  {18, 3},
      {18, 4},  {18, 5},  {18, 6},  {18, 7},  {18, 8},  {18, 9},  {18, 10},
      {17, 10}, {16, 10}, {15, 10}, {14, 10}, {13, 10}, {12, 10}, {11, 10},
      {10, 10}, {9, 10},  {8, 10},  {7, 10},  {6, 10},  {5, 10},  {4, 10},
      {4, 11},  {4, 12},  {4, 13},  {5, 13},  {6, 13},  {7, 13},  {8, 13},
      {9, 13},  {10, 13}, {11, 13}, {12, 13}, {13, 13}, {14, 13}, {15, 13},
      {16, 13}, {17, 13}, {18, 13}, {18, 14}, {18, 15}, {18, 16}, {18, 17},
      {18, 18}, {18, 19}, {19, 19}},
     {{0, 2},   {1, 2},   {2, 2},   {3, 2},   {4, 2},   {5, 2},   {6, 2},
      {7, 2},   {8, 2},   {9, 2},   {10, 2},  {11, 2},  {12, 2},  {13, 2},
      {14, 2},  {15, 2},  {16, 2},  {17, 2},  {17, 3},  {17, 4},  {17, 5},
      {17, 6},  {17, 7},  {17, 8},  {17, 9},  {16, 9},  {15, 9},  {14, 9},
      {13, 9},  {12, 9},  {11, 9},  {10, 9},  {9, 9},   {8, 9},   {7, 9},
      {6, 9},   {5, 9},   {4, 9},   {3, 9},   {3, 10},  {3, 11},  {3, 12},
      {3, 13},  {3, 14},  {4, 14},  {5, 14},  {6, 14},  {7, 14},  {8, 14},
      {9, 14},  {10, 14}, {11, 14}, {12, 14}, {13, 14}, {14, 14}, {15, 14},
      {16, 14}, {17, 14}, {17, 15}, {17, 16}, {17, 17}, {17, 18}, {17, 19},
      {16, 19}, {15, 19}, {14, 19}},
     {{0, 3},   {1, 3},   {2, 3},   {3, 3},   {4, 3},   {5, 3},   {6, 3},
      {7, 3},   {8, 3},   {9, 3},   {10, 3},  {11, 3},  {12, 3},  {13, 3},
      {14, 3},  {15, 3},  {16, 3},  {16, 4},  {16, 5},  {16, 6},  {16, 7},
      {16, 8},  {15, 8},  {14, 8},  {13, 8},  {12, 8},  {11, 8},  {10, 8},
      {9, 8},   {8, 8},   {7, 8},   {6, 8},   {5, 8},   {4, 8},   {3, 8},
      {2, 8},   {2, 9},   {2, 10},  {2, 11},  {2, 12},  {2, 13},  {2, 14},
      {2, 15},  {3, 15},  {4, 15},  {5, 15},  {6, 15},  {7, 15},  {8, 15},
      {9, 15},  {10, 15}, {11, 15}, {12, 15}, {13, 15}, {14, 15}, {15, 15},
      {16, 15}, {16, 16}, {16, 17}, {16, 18}, {15, 18}, {14, 18}, {13, 18},
      {13, 19}, {12, 19}, {11, 19}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},   {5, 4},   {6, 4},
      {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4},  {12, 4},  {13, 4},
      {14, 4},  {15, 4},  {15, 5},  {15, 6},  {15, 7},  {14, 7},  {13, 7},
      {12, 7},  {11, 7},  {10, 7},  {9, 7},   {8, 7},   {7, 7},   {6, 7},
      {5, 7},   {4, 7},   {3, 7},   {2, 7},   {1, 7},   {1, 8},   {1, 9},
      {1, 10},  {1, 11},  {1, 12},  {1, 13},  {1, 14},  {1, 15},  {1, 16},
      {2, 16},  {3, 16},  {4, 16},  {5, 16},  {6, 16},  {7, 16},  {8, 16},
      {9, 16},  {10, 16}, {11, 16}, {12, 16}, {13, 16}, {14, 16}, {15, 16},
      {15, 17}, {14, 17}, {13, 17}, {12, 17}, {12, 18}, {11, 18}, {10, 18},
      {10, 19}, {9, 19},  {8, 19}},
     {{0, 5},  {1, 5},  {2, 5},  {3, 5},  {4, 5},  {5, 5},  {6, 5},  {7, 5},
      {8, 5},  {9, 5},  {10, 5}, {11, 5}, {12, 5}, {13, 5}, {14, 5}, {14, 6},
      {13, 6}, {12, 6}, {11, 6}, {10, 6}, {9, 6},  {8, 6},  {7, 6},  {6, 6},
      {5, 6},  {4, 6},  {3, 6},  {2, 6},  {1, 6},  {0, 6},  {0, 7},  {0, 8},
      {0, 9},  {0, 10}, {0, 11}, {0, 12}, {0, 13}, {0, 14}, {0, 15}, {0, 16},
      {0, 17}, {1, 17}, {1, 18}, {0, 18}, {0, 19}, {1, 19}, {2, 19}, {3, 19},
      {3, 18}, {2, 18}, {2, 17}, {3, 17}, {4, 17}, {5, 17}, {6, 17}, {7, 17},
      {8, 17}, {9, 17}, {9, 18}, {8, 18}, {7, 18}, {7, 19}, {6, 19}, {6, 18},
      {5, 18}, {4, 18}}}
    // K=7
    ,
    {{{0, 0},   {1, 0},   {2, 0},   {3, 0},   {4, 0},   {5, 0},   {6, 0},
      {7, 0},   {8, 0},   {9, 0},   {10, 0},  {11, 0},  {12, 0},  {13, 0},
      {14, 0},  {15, 0},  {16, 0},  {17, 0},  {18, 0},  {19, 0},  {19, 1},
      {19, 2},  {19, 3},  {19, 4},  {19, 5},  {19, 6},  {19, 7},  {19, 8},
      {19, 9},  {19, 10}, {19, 11}, {19, 12}, {19, 13}, {19, 14}, {19, 15},
      {19, 16}, {19, 17}, {19, 18}, {19, 19}, {18, 19}, {18, 18}, {18, 17},
      {17, 17}, {17, 18}, {17, 19}, {16, 19}, {15, 19}, {14, 19}, {13, 19},
      {12, 19}, {11, 19}, {10, 19}, {9, 19},  {8, 19},  {7, 19},  {6, 19},
      {5, 19}},
     {{0, 1},   {1, 1},   {2, 1},   {3, 1},   {4, 1},   {5, 1},   {6, 1},
      {7, 1},   {8, 1},   {9, 1},   {10, 1},  {11, 1},  {12, 1},  {13, 1},
      {14, 1},  {15, 1},  {16, 1},  {17, 1},  {18, 1},  {18, 2},  {18, 3},
      {18, 4},  {18, 5},  {18, 6},  {18, 7},  {18, 8},  {18, 9},  {18, 10},
      {18, 11}, {18, 12}, {18, 13}, {18, 14}, {18, 15}, {18, 16}, {17, 16},
      {16, 16}, {16, 17}, {16, 18}, {15, 18}, {14, 18}, {13, 18}, {12, 18},
      {11, 18}, {10, 18}, {9, 18},  {8, 18},  {7, 18},  {6, 18},  {5, 18},
      {4, 18},  {4, 19},  {3, 19},  {2, 19},  {1, 19},  {0, 19},  {0, 18},
      {1, 18}},
     {{0, 2},   {1, 2},   {2, 2},   {3, 2},   {4, 2},   {5, 2},   {6, 2},
      {7, 2},   {8, 2},   {9, 2},   {10, 2},  {11, 2},  {12, 2},  {13, 2},
      {14, 2},  {15, 2},  {16, 2},  {17, 2},  {17, 3},  {17, 4},  {17, 5},
      {17, 6},  {17, 7},  {17, 8},  {17, 9},  {17, 10}, {17, 11}, {17, 12},
      {17, 13}, {17, 14}, {17, 15}, {16, 15}, {15, 15}, {15, 16}, {15, 17},
      {14, 17}, {13, 17}, {12, 17}, {11, 17}, {10, 17}, {9, 17},  {8, 17},
      {7, 17},  {6, 17},  {5, 17},  {4, 17},  {3, 17},  {3, 18},  {2, 18},
      {2, 17},  {2, 16},  {2, 15},  {2, 14},  {1, 14},  {1, 15},  {1, 16},
      {1, 17}},
     {{0, 3},   {1, 3},   {2, 3},   {3, 3},   {4, 3},   {5, 3},   {6, 3},
      {7, 3},   {8, 3},   {9, 3},   {10, 3},  {11, 3},  {12, 3},  {13, 3},
      {14, 3},  {15, 3},  {16, 3},  {16, 4},  {16, 5},  {16, 6},  {16, 7},
      {16, 8},  {16, 9},  {16, 10}, {16, 11}, {16, 12}, {16, 13}, {16, 14},
      {15, 14}, {15, 13}, {14, 13}, {14, 14}, {14, 15}, {14, 16}, {13, 16},
      {12, 16}, {11, 16}, {10, 16}, {9, 16},  {8, 16},  {7, 16},  {6, 16},
      {5, 16},  {4, 16},  {3, 16},  {3, 15},  {3, 14},  {3, 13},  {2, 13},
      {1, 13},  {1, 12},  {0, 12},  {0, 13},  {0, 14},  {0, 15},  {0, 16},
      {0, 17}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},   {5, 4},   {6, 4},
      {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4},  {12, 4},  {13, 4},
      {14, 4},  {15, 4},  {15, 5},  {15, 6},  {15, 7},  {15, 8},  {15, 9},
      {15, 10}, {15, 11}, {15, 12}, {14, 12}, {13, 12}, {13, 13}, {13, 14},
      {13, 15}, {12, 15}, {11, 15}, {10, 15}, {9, 15},  {8, 15},  {7, 15},
      {6, 15},  {6, 14},  {5, 14},  {5, 15},  {4, 15},  {4, 14},  {4, 13},
      {4, 12},  {3, 12},  {2, 12},  {2, 11},  {3, 11},  {3, 10},  {2, 10},
      {1, 10},  {1, 9},   {1, 8},   {0, 8},   {0, 9},   {0, 10},  {0, 11},
      {1, 11}},
     {{0, 5},   {1, 5},   {2, 5},   {3, 5},   {4, 5},   {5, 5},   {6, 5},
      {7, 5},   {8, 5},   {9, 5},   {10, 5},  {11, 5},  {12, 5},  {13, 5},
      {14, 5},  {14, 6},  {14, 7},  {14, 8},  {14, 9},  {13, 9},  {13, 8},
      {12, 8},  {12, 9},  {12, 10}, {13, 10}, {14, 10}, {14, 11}, {13, 11},
      {12, 11}, {12, 12}, {12, 13}, {12, 14}, {11, 14}, {10, 14}, {9, 14},
      {8, 14},  {7, 14},  {7, 13},  {6, 13},  {5, 13},  {5, 12},  {5, 11},
      {4, 11},  {4, 10},  {4, 9},   {4, 8},   {3, 8},   {3, 9},   {2, 9},
      {2, 8},   {2, 7},   {3, 7},   {4, 7},   {5, 7},   {5, 8},   {5, 9},
      {5, 10}},
     {{0, 6},   {0, 7},   {1, 7},   {1, 6},   {2, 6},   {3, 6},  {4, 6},
      {5, 6},   {6, 6},   {7, 6},   {8, 6},   {9, 6},   {9, 7},  {8, 7},
      {8, 8},   {7, 8},   {7, 7},   {6, 7},   {6, 8},   {6, 9},  {7, 9},
      {8, 9},   {8, 10},  {7, 10},  {6, 10},  {6, 11},  {6, 12}, {7, 12},
      {7, 11},  {8, 11},  {8, 12},  {8, 13},  {9, 13},  {9, 12}, {10, 12},
      {10, 13}, {11, 13}, {11, 12}, {11, 11}, {10, 11}, {9, 11}, {9, 10},
      {9, 9},   {10, 9},  {10, 8},  {10, 7},  {10, 6},  {11, 6}, {12, 6},
      {13, 6},  {13, 7},  {12, 7},  {11, 7},  {11, 8},  {11, 9}, {11, 10},
      {10, 10}}}
    // K=8
    ,
    {{{0, 0},   {1, 0},   {2, 0},   {3, 0},   {4, 0},   {5, 0},   {6, 0},
      {7, 0},   {8, 0},   {9, 0},   {10, 0},  {11, 0},  {12, 0},  {13, 0},
      {14, 0},  {15, 0},  {16, 0},  {17, 0},  {18, 0},  {19, 0},  {19, 1},
      {19, 2},  {19, 3},  {19, 4},  {19, 5},  {19, 6},  {19, 7},  {19, 8},
      {19, 9},  {19, 10}, {19, 11}, {19, 12}, {19, 13}, {19, 14}, {19, 15},
      {19, 16}, {19, 17}, {19, 18}, {19, 19}, {18, 19}, {17, 19}, {16, 19},
      {15, 19}, {14, 19}, {13, 19}, {12, 19}, {11, 19}, {10, 19}, {9, 19},
      {8, 19}},
     {{0, 1},   {1, 1},   {2, 1},   {3, 1},   {4, 1},   {5, 1},   {6, 1},
      {7, 1},   {8, 1},   {9, 1},   {10, 1},  {11, 1},  {12, 1},  {13, 1},
      {14, 1},  {15, 1},  {16, 1},  {17, 1},  {18, 1},  {18, 2},  {18, 3},
      {18, 4},  {18, 5},  {18, 6},  {18, 7},  {18, 8},  {18, 9},  {18, 10},
      {18, 11}, {18, 12}, {18, 13}, {18, 14}, {18, 15}, {18, 16}, {18, 17},
      {18, 18}, {17, 18}, {16, 18}, {15, 18}, {14, 18}, {13, 18}, {12, 18},
      {11, 18}, {10, 18}, {9, 18},  {8, 18},  {7, 18},  {7, 19},  {6, 19},
      {6, 18}},
     {{0, 2},   {1, 2},   {2, 2},   {3, 2},   {4, 2},   {5, 2},   {6, 2},
      {7, 2},   {8, 2},   {9, 2},   {10, 2},  {11, 2},  {12, 2},  {13, 2},
      {14, 2},  {15, 2},  {16, 2},  {17, 2},  {17, 3},  {17, 4},  {17, 5},
      {17, 6},  {17, 7},  {17, 8},  {17, 9},  {17, 10}, {17, 11}, {17, 12},
      {17, 13}, {17, 14}, {17, 15}, {17, 16}, {17, 17}, {16, 17}, {15, 17},
      {14, 17}, {13, 17}, {12, 17}, {11, 17}, {10, 17}, {9, 17},  {8, 17},
      {7, 17},  {6, 17},  {5, 17},  {5, 18},  {5, 19},  {4, 19},  {4, 18},
      {3, 18}},
     {{0, 3},   {1, 3},   {2, 3},   {3, 3},   {4, 3},   {5, 3},   {6, 3},
      {7, 3},   {8, 3},   {9, 3},   {10, 3},  {11, 3},  {12, 3},  {13, 3},
      {14, 3},  {15, 3},  {16, 3},  {16, 4},  {16, 5},  {16, 6},  {16, 7},
      {16, 8},  {16, 9},  {16, 10}, {16, 11}, {16, 12}, {16, 13}, {16, 14},
      {16, 15}, {16, 16}, {15, 16}, {14, 16}, {13, 16}, {12, 16}, {11, 16},
      {10, 16}, {9, 16},  {8, 16},  {7, 16},  {6, 16},  {5, 16},  {4, 16},
      {4, 17},  {3, 17},  {3, 16},  {2, 16},  {2, 17},  {2, 18},  {2, 19},
      {3, 19}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},   {5, 4},   {6, 4},
      {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4},  {12, 4},  {13, 4},
      {14, 4},  {15, 4},  {15, 5},  {15, 6},  {15, 7},  {15, 8},  {15, 9},
      {15, 10}, {15, 11}, {15, 12}, {15, 13}, {15, 14}, {15, 15}, {14, 15},
      {13, 15}, {12, 15}, {11, 15}, {10, 15}, {9, 15},  {8, 15},  {7, 15},
      {6, 15},  {5, 15},  {4, 15},  {3, 15},  {2, 15},  {1, 15},  {1, 16},
      {1, 17},  {1, 18},  {1, 19},  {0, 19},  {0, 18},  {0, 17},  {0, 16},
      {0, 15}},
     {{0, 5},   {1, 5},   {2, 5},   {3, 5},   {4, 5},   {5, 5},   {6, 5},
      {7, 5},   {8, 5},   {9, 5},   {10, 5},  {11, 5},  {12, 5},  {13, 5},
      {14, 5},  {14, 6},  {14, 7},  {14, 8},  {14, 9},  {14, 10}, {14, 11},
      {14, 12}, {14, 13}, {14, 14}, {13, 14}, {12, 14}, {11, 14}, {10, 14},
      {9, 14},  {8, 14},  {7, 14},  {6, 14},  {5, 14},  {4, 14},  {3, 14},
      {2, 14},  {2, 13},  {1, 13},  {1, 14},  {0, 14},  {0, 13},  {0, 12},
      {0, 11},  {0, 10},  {0, 9},   {1, 9},   {1, 10},  {2, 10},  {2, 9},
      {2, 8}},
     {{0, 6},   {1, 6},   {2, 6},   {3, 6},   {4, 6},   {5, 6},   {6, 6},
      {7, 6},   {8, 6},   {9, 6},   {10, 6},  {11, 6},  {12, 6},  {13, 6},
      {13, 7},  {13, 8},  {13, 9},  {13, 10}, {13, 11}, {13, 12}, {13, 13},
      {12, 13}, {11, 13}, {10, 13}, {9, 13},  {8, 13},  {7, 13},  {6, 13},
      {5, 13},  {5, 12},  {5, 11},  {4, 11},  {4, 12},  {4, 13},  {3, 13},
      {3, 12},  {2, 12},  {1, 12},  {1, 11},  {2, 11},  {3, 11},  {3, 10},
      {4, 10},  {5, 10},  {5, 9},   {5, 8},   {6, 8},   {6, 9},   {6, 10},
      {6, 11}},
     {{0, 7},   {0, 8},   {1, 8},   {1, 7},   {2, 7},   {3, 7},   {3, 8},
      {3, 9},   {4, 9},   {4, 8},   {4, 7},   {5, 7},   {6, 7},   {7, 7},
      {8, 7},   {9, 7},   {10, 7},  {11, 7},  {12, 7},  {12, 8},  {11, 8},
      {11, 9},  {12, 9},  {12, 10}, {12, 11}, {12, 12}, {11, 12}, {10, 12},
      {9, 12},  {8, 12},  {8, 11},  {8, 10},  {9, 10},  {9, 11},  {10, 11},
      {11, 11}, {11, 10}, {10, 10}, {10, 9},  {10, 8},  {9, 8},   {9, 9},
      {8, 9},   {8, 8},   {7, 8},   {7, 9},   {7, 10},  {7, 11},  {7, 12},
      {6, 12}}}
    // K=9
    ,
    {{{0, 0},   {1, 0},   {2, 0},   {3, 0},   {4, 0},   {5, 0},   {6, 0},
      {7, 0},   {8, 0},   {9, 0},   {10, 0},  {11, 0},  {12, 0},  {13, 0},
      {14, 0},  {15, 0},  {16, 0},  {17, 0},  {18, 0},  {19, 0},  {19, 1},
      {19, 2},  {19, 3},  {19, 4},  {19, 5},  {19, 6},  {19, 7},  {19, 8},
      {19, 9},  {19, 10}, {19, 11}, {19, 12}, {19, 13}, {19, 14}, {19, 15},
      {19, 16}, {19, 17}, {19, 18}, {19, 19}, {18, 19}, {17, 19}, {16, 19},
      {15, 19}, {14, 19}},
     {{0, 1},   {1, 1},   {2, 1},   {3, 1},   {4, 1},   {5, 1},   {6, 1},
      {7, 1},   {8, 1},   {9, 1},   {10, 1},  {11, 1},  {12, 1},  {13, 1},
      {14, 1},  {15, 1},  {16, 1},  {17, 1},  {18, 1},  {18, 2},  {18, 3},
      {18, 4},  {18, 5},  {18, 6},  {18, 7},  {18, 8},  {18, 9},  {18, 10},
      {18, 11}, {18, 12}, {18, 13}, {18, 14}, {18, 15}, {18, 16}, {18, 17},
      {18, 18}, {17, 18}, {16, 18}, {15, 18}, {14, 18}, {13, 18}, {13, 19},
      {12, 19}, {11, 19}},
     {{0, 2},   {1, 2},   {2, 2},   {3, 2},   {4, 2},   {5, 2},   {6, 2},
      {7, 2},   {8, 2},   {9, 2},   {10, 2},  {11, 2},  {12, 2},  {13, 2},
      {14, 2},  {15, 2},  {16, 2},  {17, 2},  {17, 3},  {17, 4},  {17, 5},
      {17, 6},  {17, 7},  {17, 8},  {17, 9},  {17, 10}, {17, 11}, {17, 12},
      {17, 13}, {17, 14}, {17, 15}, {17, 16}, {17, 17}, {16, 17}, {15, 17},
      {14, 17}, {13, 17}, {12, 17}, {12, 18}, {11, 18}, {10, 18}, {10, 19},
      {9, 19},  {8, 19}},
     {{0, 3},   {1, 3},   {2, 3},   {3, 3},   {4, 3},   {5, 3},   {6, 3},
      {7, 3},   {8, 3},   {9, 3},   {10, 3},  {11, 3},  {12, 3},  {13, 3},
      {14, 3},  {15, 3},  {16, 3},  {16, 4},  {16, 5},  {16, 6},  {16, 7},
      {16, 8},  {16, 9},  {16, 10}, {16, 11}, {16, 12}, {16, 13}, {16, 14},
      {16, 15}, {16, 16}, {15, 16}, {14, 16}, {13, 16}, {12, 16}, {11, 16},
      {11, 17}, {10, 17}, {9, 17},  {9, 18},  {8, 18},  {7, 18},  {7, 19},
      {6, 19},  {5, 19}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},   {5, 4},   {6, 4},
      {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4},  {12, 4},  {13, 4},
      {14, 4},  {15, 4},  {15, 5},  {15, 6},  {15, 7},  {15, 8},  {15, 9},
      {15, 10}, {15, 11}, {15, 12}, {15, 13}, {15, 14}, {15, 15}, {14, 15},
      {13, 15}, {12, 15}, {11, 15}, {10, 15}, {10, 16}, {9, 16},  {8, 16},
      {8, 17},  {7, 17},  {6, 17},  {6, 18},  {5, 18},  {4, 18},  {4, 19},
      {3, 19},  {2, 19}},
     {{0, 5},   {1, 5},   {2, 5},   {3, 5},   {4, 5},   {5, 5},   {6, 5},
      {7, 5},   {8, 5},   {9, 5},   {10, 5},  {11, 5},  {12, 5},  {13, 5},
      {14, 5},  {14, 6},  {14, 7},  {14, 8},  {14, 9},  {14, 10}, {14, 11},
      {14, 12}, {14, 13}, {14, 14}, {13, 14}, {12, 14}, {11, 14}, {10, 14},
      {9, 14},  {9, 15},  {8, 15},  {7, 15},  {7, 16},  {6, 16},  {5, 16},
      {5, 17},  {4, 17},  {3, 17},  {3, 18},  {2, 18},  {1, 18},  {1, 19},
      {0, 19},  {0, 18}},
     {{0, 6},   {1, 6},   {2, 6},   {3, 6},   {4, 6},   {5, 6},   {6, 6},
      {7, 6},   {8, 6},   {9, 6},   {10, 6},  {11, 6},  {12, 6},  {13, 6},
      {13, 7},  {13, 8},  {13, 9},  {13, 10}, {13, 11}, {13, 12}, {13, 13},
      {12, 13}, {11, 13}, {10, 13}, {9, 13},  {8, 13},  {8, 14},  {7, 14},
      {6, 14},  {6, 15},  {5, 15},  {4, 15},  {4, 16},  {3, 16},  {3, 15},
      {2, 15},  {1, 15},  {0, 15},  {0, 16},  {0, 17},  {1, 17},  {1, 16},
      {2, 16},  {2, 17}},
     {{0, 7},  {1, 7},   {2, 7},   {3, 7},   {4, 7},   {5, 7},   {6, 7},
      {7, 7},  {8, 7},   {9, 7},   {10, 7},  {11, 7},  {12, 7},  {12, 8},
      {12, 9}, {12, 10}, {12, 11}, {12, 12}, {11, 12}, {10, 12}, {9, 12},
      {8, 12}, {7, 12},  {7, 13},  {6, 13},  {5, 13},  {5, 14},  {4, 14},
      {3, 14}, {2, 14},  {1, 14},  {0, 14},  {0, 13},  {0, 12},  {0, 11},
      {1, 11}, {2, 11},  {2, 10},  {1, 10},  {0, 10},  {0, 9},   {1, 9},
      {2, 9},  {3, 9}},
     {{0, 8},  {1, 8},  {2, 8},  {3, 8},  {4, 8},  {5, 8},   {6, 8},   {7, 8},
      {8, 8},  {9, 8},  {10, 8}, {11, 8}, {11, 9}, {11, 10}, {11, 11}, {10, 11},
      {9, 11}, {8, 11}, {7, 11}, {6, 11}, {6, 12}, {5, 12},  {4, 12},  {4, 11},
      {5, 11}, {5, 10}, {6, 10}, {7, 10}, {8, 10}, {9, 10},  {9, 9},   {8, 9},
      {7, 9},  {6, 9},  {5, 9},  {4, 9},  {4, 10}, {3, 10},  {3, 11},  {3, 12},
      {2, 12}, {1, 12}, {1, 13}, {2, 13}}}
    // K=10
    ,
    {{{0, 0},   {1, 0},   {2, 0},   {3, 0},   {4, 0},   {5, 0},   {6, 0},
      {7, 0},   {8, 0},   {9, 0},   {10, 0},  {11, 0},  {12, 0},  {13, 0},
      {14, 0},  {15, 0},  {16, 0},  {17, 0},  {18, 0},  {19, 0},  {19, 1},
      {19, 2},  {19, 3},  {19, 4},  {19, 5},  {19, 6},  {19, 7},  {19, 8},
      {19, 9},  {19, 10}, {19, 11}, {19, 12}, {19, 13}, {19, 14}, {19, 15},
      {19, 16}, {19, 17}, {19, 18}, {19, 19}, {18, 19}},
     {{0, 1},   {1, 1},   {2, 1},   {3, 1},   {4, 1},   {5, 1},   {6, 1},
      {7, 1},   {8, 1},   {9, 1},   {10, 1},  {11, 1},  {12, 1},  {13, 1},
      {14, 1},  {15, 1},  {16, 1},  {17, 1},  {18, 1},  {18, 2},  {18, 3},
      {18, 4},  {18, 5},  {18, 6},  {18, 7},  {18, 8},  {18, 9},  {18, 10},
      {18, 11}, {18, 12}, {18, 13}, {18, 14}, {18, 15}, {18, 16}, {18, 17},
      {18, 18}, {17, 18}, {17, 19}, {16, 19}, {15, 19}},
     {{0, 2},   {1, 2},   {2, 2},   {3, 2},   {4, 2},   {5, 2},   {6, 2},
      {7, 2},   {8, 2},   {9, 2},   {10, 2},  {11, 2},  {12, 2},  {13, 2},
      {14, 2},  {15, 2},  {16, 2},  {17, 2},  {17, 3},  {17, 4},  {17, 5},
      {17, 6},  {17, 7},  {17, 8},  {17, 9},  {17, 10}, {17, 11}, {17, 12},
      {17, 13}, {17, 14}, {17, 15}, {17, 16}, {17, 17}, {16, 17}, {16, 18},
      {15, 18}, {14, 18}, {14, 19}, {13, 19}, {12, 19}},
     {{0, 3},   {1, 3},   {2, 3},   {3, 3},   {4, 3},   {5, 3},   {6, 3},
      {7, 3},   {8, 3},   {9, 3},   {10, 3},  {11, 3},  {12, 3},  {13, 3},
      {14, 3},  {15, 3},  {16, 3},  {16, 4},  {16, 5},  {16, 6},  {16, 7},
      {16, 8},  {16, 9},  {16, 10}, {16, 11}, {16, 12}, {16, 13}, {16, 14},
      {16, 15}, {16, 16}, {15, 16}, {15, 17}, {14, 17}, {13, 17}, {13, 18},
      {12, 18}, {11, 18}, {11, 19}, {10, 19}, {9, 19}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},   {5, 4},   {6, 4},
      {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4},  {12, 4},  {13, 4},
      {14, 4},  {15, 4},  {15, 5},  {15, 6},  {15, 7},  {15, 8},  {15, 9},
      {15, 10}, {15, 11}, {15, 12}, {15, 13}, {15, 14}, {15, 15}, {14, 15},
      {14, 16}, {13, 16}, {12, 16}, {12, 17}, {11, 17}, {10, 17}, {10, 18},
      {9, 18},  {8, 18},  {8, 19},  {7, 19},  {6, 19}},
     {{0, 5},   {1, 5},   {2, 5},   {3, 5},   {4, 5},   {5, 5},   {6, 5},
      {7, 5},   {8, 5},   {9, 5},   {10, 5},  {11, 5},  {12, 5},  {13, 5},
      {14, 5},  {14, 6},  {14, 7},  {14, 8},  {14, 9},  {14, 10}, {14, 11},
      {14, 12}, {14, 13}, {14, 14}, {13, 14}, {13, 15}, {12, 15}, {11, 15},
      {11, 16}, {10, 16}, {9, 16},  {9, 17},  {8, 17},  {7, 17},  {7, 18},
      {6, 18},  {5, 18},  {5, 19},  {4, 19},  {3, 19}},
     {{0, 6},   {1, 6},   {2, 6},   {3, 6},   {4, 6},   {5, 6},   {6, 6},
      {7, 6},   {8, 6},   {9, 6},   {10, 6},  {11, 6},  {12, 6},  {13, 6},
      {13, 7},  {13, 8},  {13, 9},  {13, 10}, {13, 11}, {13, 12}, {13, 13},
      {12, 13}, {12, 14}, {11, 14}, {10, 14}, {10, 15}, {9, 15},  {8, 15},
      {8, 16},  {7, 16},  {6, 16},  {6, 17},  {5, 17},  {4, 17},  {4, 18},
      {3, 18},  {2, 18},  {2, 19},  {1, 19},  {0, 19}},
     {{0, 7},  {1, 7},   {2, 7},   {3, 7},   {4, 7},   {5, 7},   {6, 7},
      {7, 7},  {8, 7},   {9, 7},   {10, 7},  {11, 7},  {12, 7},  {12, 8},
      {12, 9}, {12, 10}, {12, 11}, {12, 12}, {11, 12}, {11, 13}, {10, 13},
      {9, 13}, {9, 14},  {8, 14},  {7, 14},  {7, 15},  {6, 15},  {5, 15},
      {5, 16}, {4, 16},  {3, 16},  {3, 17},  {2, 17},  {1, 17},  {1, 18},
      {0, 18}, {0, 17},  {0, 16},  {0, 15},  {0, 14}},
     {{0, 8},   {1, 8},   {2, 8},   {3, 8},  {4, 8},  {5, 8},  {6, 8},
      {7, 8},   {8, 8},   {9, 8},   {10, 8}, {11, 8}, {11, 9}, {11, 10},
      {11, 11}, {10, 11}, {10, 12}, {9, 12}, {8, 12}, {8, 13}, {7, 13},
      {6, 13},  {6, 14},  {5, 14},  {4, 14}, {4, 15}, {3, 15}, {2, 15},
      {2, 16},  {1, 16},  {1, 15},  {1, 14}, {1, 13}, {0, 13}, {0, 12},
      {0, 11},  {0, 10},  {1, 10},  {2, 10}, {3, 10}},
     {{0, 9},  {1, 9},  {2, 9},  {3, 9},   {4, 9},  {5, 9},  {6, 9},  {7, 9},
      {8, 9},  {9, 9},  {10, 9}, {10, 10}, {9, 10}, {9, 11}, {8, 11}, {7, 11},
      {7, 12}, {6, 12}, {5, 12}, {5, 13},  {4, 13}, {3, 13}, {3, 14}, {2, 14},
      {2, 13}, {2, 12}, {1, 12}, {1, 11},  {2, 11}, {3, 11}, {3, 12}, {4, 12},
      {4, 11}, {4, 10}, {5, 10}, {5, 11},  {6, 11}, {6, 10}, {7, 10}, {8, 10}}}
    // K=11
    ,
    {{{0, 0},   {1, 0},   {2, 0},   {3, 0},   {4, 0},   {5, 0},
      {6, 0},   {7, 0},   {8, 0},   {9, 0},   {10, 0},  {11, 0},
      {12, 0},  {13, 0},  {14, 0},  {15, 0},  {16, 0},  {17, 0},
      {18, 0},  {19, 0},  {19, 1},  {19, 2},  {19, 3},  {19, 4},
      {19, 5},  {19, 6},  {19, 7},  {19, 8},  {19, 9},  {19, 10},
      {19, 11}, {19, 12}, {19, 13}, {19, 14}, {19, 15}, {19, 16}},
     {{0, 1},   {1, 1},   {2, 1},   {3, 1},   {4, 1},   {5, 1},
      {6, 1},   {7, 1},   {8, 1},   {9, 1},   {10, 1},  {11, 1},
      {12, 1},  {13, 1},  {14, 1},  {15, 1},  {16, 1},  {17, 1},
      {18, 1},  {18, 2},  {18, 3},  {18, 4},  {18, 5},  {18, 6},
      {18, 7},  {18, 8},  {18, 9},  {18, 10}, {18, 11}, {18, 12},
      {18, 13}, {18, 14}, {18, 15}, {18, 16}, {18, 17}, {19, 17}},
     {{0, 2},   {1, 2},   {2, 2},   {3, 2},   {4, 2},   {5, 2},
      {6, 2},   {7, 2},   {8, 2},   {9, 2},   {10, 2},  {11, 2},
      {12, 2},  {13, 2},  {14, 2},  {15, 2},  {16, 2},  {17, 2},
      {17, 3},  {17, 4},  {17, 5},  {17, 6},  {17, 7},  {17, 8},
      {17, 9},  {17, 10}, {17, 11}, {17, 12}, {17, 13}, {17, 14},
      {17, 15}, {17, 16}, {17, 17}, {17, 18}, {18, 18}, {19, 18}},
     {{0, 3},   {1, 3},   {2, 3},   {3, 3},   {4, 3},   {5, 3},
      {6, 3},   {7, 3},   {8, 3},   {9, 3},   {10, 3},  {11, 3},
      {12, 3},  {13, 3},  {14, 3},  {15, 3},  {16, 3},  {16, 4},
      {16, 5},  {16, 6},  {16, 7},  {16, 8},  {16, 9},  {16, 10},
      {16, 11}, {16, 12}, {16, 13}, {16, 14}, {16, 15}, {16, 16},
      {16, 17}, {16, 18}, {16, 19}, {17, 19}, {18, 19}, {19, 19}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},   {5, 4},
      {6, 4},   {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4},
      {12, 4},  {13, 4},  {14, 4},  {15, 4},  {15, 5},  {15, 6},
      {15, 7},  {15, 8},  {15, 9},  {15, 10}, {15, 11}, {15, 12},
      {15, 13}, {15, 14}, {15, 15}, {15, 16}, {15, 17}, {15, 18},
      {15, 19}, {14, 19}, {13, 19}, {12, 19}, {11, 19}, {10, 19}},
     {{0, 5},   {1, 5},   {2, 5},   {3, 5},   {4, 5},   {5, 5},
      {6, 5},   {7, 5},   {8, 5},   {9, 5},   {10, 5},  {11, 5},
      {12, 5},  {13, 5},  {14, 5},  {14, 6},  {14, 7},  {14, 8},
      {14, 9},  {14, 10}, {14, 11}, {14, 12}, {14, 13}, {14, 14},
      {14, 15}, {14, 16}, {14, 17}, {14, 18}, {13, 18}, {12, 18},
      {11, 18}, {10, 18}, {9, 18},  {9, 19},  {8, 19},  {7, 19}},
     {{0, 6},   {1, 6},   {2, 6},   {3, 6},   {4, 6},   {5, 6},
      {6, 6},   {7, 6},   {8, 6},   {9, 6},   {10, 6},  {11, 6},
      {12, 6},  {13, 6},  {13, 7},  {13, 8},  {13, 9},  {13, 10},
      {13, 11}, {13, 12}, {13, 13}, {13, 14}, {13, 15}, {13, 16},
      {13, 17}, {12, 17}, {11, 17}, {10, 17}, {9, 17},  {8, 17},
      {8, 18},  {7, 18},  {6, 18},  {6, 19},  {5, 19},  {4, 19}},
     {{0, 7},   {1, 7},   {2, 7},   {3, 7},   {4, 7},   {5, 7},
      {6, 7},   {7, 7},   {8, 7},   {9, 7},   {10, 7},  {11, 7},
      {12, 7},  {12, 8},  {12, 9},  {12, 10}, {12, 11}, {12, 12},
      {12, 13}, {12, 14}, {12, 15}, {12, 16}, {11, 16}, {10, 16},
      {9, 16},  {8, 16},  {7, 16},  {7, 17},  {6, 17},  {5, 17},
      {5, 18},  {4, 18},  {3, 18},  {3, 19},  {2, 19},  {1, 19}},
     {{0, 8},   {1, 8},   {2, 8},   {3, 8},   {4, 8},   {5, 8},
      {6, 8},   {7, 8},   {8, 8},   {9, 8},   {10, 8},  {11, 8},
      {11, 9},  {11, 10}, {11, 11}, {11, 12}, {11, 13}, {11, 14},
      {11, 15}, {10, 15}, {9, 15},  {8, 15},  {7, 15},  {6, 15},
      {6, 16},  {5, 16},  {4, 16},  {4, 17},  {3, 17},  {2, 17},
      {2, 18},  {1, 18},  {0, 18},  {0, 17},  {0, 16},  {0, 15}},
     {{0, 9},   {1, 9},   {2, 9},   {3, 9},   {4, 9},  {5, 9},
      {6, 9},   {7, 9},   {8, 9},   {9, 9},   {10, 9}, {10, 10},
      {10, 11}, {10, 12}, {10, 13}, {10, 14}, {9, 14}, {8, 14},
      {7, 14},  {6, 14},  {5, 14},  {5, 15},  {4, 15}, {3, 15},
      {3, 16},  {2, 16},  {1, 16},  {1, 15},  {1, 14}, {0, 14},
      {0, 13},  {0, 12},  {0, 11},  {1, 11},  {2, 11}, {3, 11}},
     {{0, 10}, {1, 10}, {2, 10}, {3, 10}, {4, 10}, {5, 10}, {6, 10}, {7, 10},
      {8, 10}, {9, 10}, {9, 11}, {9, 12}, {9, 13}, {8, 13}, {7, 13}, {6, 13},
      {5, 13}, {4, 13}, {4, 14}, {3, 14}, {2, 14}, {2, 13}, {1, 13}, {1, 12},
      {2, 12}, {3, 12}, {4, 12}, {4, 11}, {5, 11}, {6, 11}, {7, 11}, {8, 11},
      {8, 12}, {7, 12}, {6, 12}, {5, 12}}}
    // K=12
    ,
    {{{0, 0},  {1, 0},   {2, 0},   {3, 0},   {4, 0},  {5, 0},  {6, 0},
      {7, 0},  {8, 0},   {9, 0},   {10, 0},  {11, 0}, {12, 0}, {13, 0},
      {14, 0}, {15, 0},  {16, 0},  {17, 0},  {18, 0}, {19, 0}, {19, 1},
      {19, 2}, {19, 3},  {19, 4},  {19, 5},  {19, 6}, {19, 7}, {19, 8},
      {19, 9}, {19, 10}, {19, 11}, {19, 12}, {19, 13}},
     {{0, 1},   {1, 1},   {2, 1},   {3, 1},   {4, 1},  {5, 1},  {6, 1},
      {7, 1},   {8, 1},   {9, 1},   {10, 1},  {11, 1}, {12, 1}, {13, 1},
      {14, 1},  {15, 1},  {16, 1},  {17, 1},  {18, 1}, {18, 2}, {18, 3},
      {18, 4},  {18, 5},  {18, 6},  {18, 7},  {18, 8}, {18, 9}, {18, 10},
      {18, 11}, {18, 12}, {18, 13}, {18, 14}, {19, 14}},
     {{0, 2},   {1, 2},   {2, 2},   {3, 2},   {4, 2},   {5, 2},   {6, 2},
      {7, 2},   {8, 2},   {9, 2},   {10, 2},  {11, 2},  {12, 2},  {13, 2},
      {14, 2},  {15, 2},  {16, 2},  {17, 2},  {17, 3},  {17, 4},  {17, 5},
      {17, 6},  {17, 7},  {17, 8},  {17, 9},  {17, 10}, {17, 11}, {17, 12},
      {17, 13}, {17, 14}, {17, 15}, {18, 15}, {19, 15}},
     {{0, 3},   {1, 3},   {2, 3},   {3, 3},   {4, 3},   {5, 3},   {6, 3},
      {7, 3},   {8, 3},   {9, 3},   {10, 3},  {11, 3},  {12, 3},  {13, 3},
      {14, 3},  {15, 3},  {16, 3},  {16, 4},  {16, 5},  {16, 6},  {16, 7},
      {16, 8},  {16, 9},  {16, 10}, {16, 11}, {16, 12}, {16, 13}, {16, 14},
      {16, 15}, {16, 16}, {17, 16}, {18, 16}, {19, 16}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},   {5, 4},   {6, 4},
      {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4},  {12, 4},  {13, 4},
      {14, 4},  {15, 4},  {15, 5},  {15, 6},  {15, 7},  {15, 8},  {15, 9},
      {15, 10}, {15, 11}, {15, 12}, {15, 13}, {15, 14}, {15, 15}, {15, 16},
      {15, 17}, {16, 17}, {17, 17}, {18, 17}, {19, 17}},
     {{0, 5},   {1, 5},   {2, 5},   {3, 5},   {4, 5},   {5, 5},   {6, 5},
      {7, 5},   {8, 5},   {9, 5},   {10, 5},  {11, 5},  {12, 5},  {13, 5},
      {14, 5},  {14, 6},  {14, 7},  {14, 8},  {14, 9},  {14, 10}, {14, 11},
      {14, 12}, {14, 13}, {14, 14}, {14, 15}, {14, 16}, {14, 17}, {14, 18},
      {15, 18}, {16, 18}, {17, 18}, {18, 18}, {19, 18}},
     {{0, 6},   {1, 6},   {2, 6},   {3, 6},   {4, 6},   {5, 6},   {6, 6},
      {7, 6},   {8, 6},   {9, 6},   {10, 6},  {11, 6},  {12, 6},  {13, 6},
      {13, 7},  {13, 8},  {13, 9},  {13, 10}, {13, 11}, {13, 12}, {13, 13},
      {13, 14}, {13, 15}, {13, 16}, {13, 17}, {13, 18}, {13, 19}, {14, 19},
      {15, 19}, {16, 19}, {17, 19}, {18, 19}, {19, 19}},
     {{0, 7},   {1, 7},   {2, 7},   {3, 7},   {4, 7},   {5, 7},   {6, 7},
      {7, 7},   {8, 7},   {9, 7},   {10, 7},  {11, 7},  {12, 7},  {12, 8},
      {12, 9},  {12, 10}, {12, 11}, {12, 12}, {12, 13}, {12, 14}, {12, 15},
      {12, 16}, {12, 17}, {12, 18}, {12, 19}, {11, 19}, {10, 19}, {9, 19},
      {8, 19},  {7, 19},  {6, 19},  {5, 19},  {4, 19}},
     {{0, 8},   {1, 8},   {2, 8},   {3, 8},   {4, 8},   {5, 8},   {6, 8},
      {7, 8},   {8, 8},   {9, 8},   {10, 8},  {11, 8},  {11, 9},  {11, 10},
      {11, 11}, {11, 12}, {11, 13}, {11, 14}, {11, 15}, {11, 16}, {11, 17},
      {11, 18}, {10, 18}, {9, 18},  {8, 18},  {7, 18},  {6, 18},  {5, 18},
      {4, 18},  {3, 18},  {3, 19},  {2, 19},  {1, 19}},
     {{0, 9},   {1, 9},   {2, 9},   {3, 9},   {4, 9},   {5, 9},   {6, 9},
      {7, 9},   {8, 9},   {9, 9},   {10, 9},  {10, 10}, {10, 11}, {10, 12},
      {10, 13}, {10, 14}, {10, 15}, {10, 16}, {10, 17}, {9, 17},  {8, 17},
      {7, 17},  {6, 17},  {5, 17},  {4, 17},  {3, 17},  {2, 17},  {2, 18},
      {1, 18},  {0, 18},  {0, 17},  {0, 16},  {0, 15}},
     {{0, 10}, {1, 10}, {2, 10}, {3, 10}, {4, 10}, {5, 10}, {6, 10},
      {7, 10}, {8, 10}, {9, 10}, {9, 11}, {9, 12}, {9, 13}, {9, 14},
      {9, 15}, {9, 16}, {8, 16}, {7, 16}, {6, 16}, {5, 16}, {4, 16},
      {3, 16}, {2, 16}, {1, 16}, {1, 15}, {1, 14}, {0, 14}, {0, 13},
      {0, 12}, {1, 12}, {2, 12}, {3, 12}, {4, 12}},
     {{0, 11}, {1, 11}, {2, 11}, {3, 11}, {4, 11}, {5, 11}, {6, 11},
      {7, 11}, {8, 11}, {8, 12}, {8, 13}, {8, 14}, {8, 15}, {7, 15},
      {6, 15}, {5, 15}, {4, 15}, {3, 15}, {2, 15}, {2, 14}, {2, 13},
      {3, 13}, {4, 13}, {5, 13}, {5, 12}, {6, 12}, {7, 12}, {7, 13},
      {7, 14}, {6, 14}, {5, 14}, {4, 14}, {3, 14}}}
    // K=13
    ,
    {{{0, 0},  {1, 0},  {2, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},  {7, 0},
      {8, 0},  {9, 0},  {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0},
      {16, 0}, {17, 0}, {18, 0}, {19, 0}, {19, 1}, {19, 2}, {19, 3}, {19, 4},
      {19, 5}, {19, 6}, {19, 7}, {19, 8}, {19, 9}, {19, 10}},
     {{0, 1},  {1, 1},  {2, 1},  {3, 1},   {4, 1},   {5, 1},  {6, 1},  {7, 1},
      {8, 1},  {9, 1},  {10, 1}, {11, 1},  {12, 1},  {13, 1}, {14, 1}, {15, 1},
      {16, 1}, {17, 1}, {18, 1}, {18, 2},  {18, 3},  {18, 4}, {18, 5}, {18, 6},
      {18, 7}, {18, 8}, {18, 9}, {18, 10}, {18, 11}, {19, 11}},
     {{0, 2},  {1, 2},   {2, 2},   {3, 2},   {4, 2},   {5, 2},
      {6, 2},  {7, 2},   {8, 2},   {9, 2},   {10, 2},  {11, 2},
      {12, 2}, {13, 2},  {14, 2},  {15, 2},  {16, 2},  {17, 2},
      {17, 3}, {17, 4},  {17, 5},  {17, 6},  {17, 7},  {17, 8},
      {17, 9}, {17, 10}, {17, 11}, {17, 12}, {18, 12}, {19, 12}},
     {{0, 3},   {1, 3},   {2, 3},   {3, 3},   {4, 3},   {5, 3},
      {6, 3},   {7, 3},   {8, 3},   {9, 3},   {10, 3},  {11, 3},
      {12, 3},  {13, 3},  {14, 3},  {15, 3},  {16, 3},  {16, 4},
      {16, 5},  {16, 6},  {16, 7},  {16, 8},  {16, 9},  {16, 10},
      {16, 11}, {16, 12}, {16, 13}, {17, 13}, {18, 13}, {19, 13}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},   {5, 4},
      {6, 4},   {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4},
      {12, 4},  {13, 4},  {14, 4},  {15, 4},  {15, 5},  {15, 6},
      {15, 7},  {15, 8},  {15, 9},  {15, 10}, {15, 11}, {15, 12},
      {15, 13}, {15, 14}, {16, 14}, {17, 14}, {18, 14}, {19, 14}},
     {{0, 5},   {1, 5},   {2, 5},   {3, 5},   {4, 5},   {5, 5},
      {6, 5},   {7, 5},   {8, 5},   {9, 5},   {10, 5},  {11, 5},
      {12, 5},  {13, 5},  {14, 5},  {14, 6},  {14, 7},  {14, 8},
      {14, 9},  {14, 10}, {14, 11}, {14, 12}, {14, 13}, {14, 14},
      {14, 15}, {15, 15}, {16, 15}, {17, 15}, {18, 15}, {19, 15}},
     {{0, 6},   {1, 6},   {2, 6},   {3, 6},   {4, 6},   {5, 6},
      {6, 6},   {7, 6},   {8, 6},   {9, 6},   {10, 6},  {11, 6},
      {12, 6},  {13, 6},  {13, 7},  {13, 8},  {13, 9},  {13, 10},
      {13, 11}, {13, 12}, {13, 13}, {13, 14}, {13, 15}, {13, 16},
      {14, 16}, {15, 16}, {16, 16}, {17, 16}, {18, 16}, {19, 16}},
     {{0, 7},   {1, 7},   {2, 7},   {3, 7},   {4, 7},   {5, 7},
      {6, 7},   {7, 7},   {8, 7},   {9, 7},   {10, 7},  {11, 7},
      {12, 7},  {12, 8},  {12, 9},  {12, 10}, {12, 11}, {12, 12},
      {12, 13}, {12, 14}, {12, 15}, {12, 16}, {12, 17}, {13, 17},
      {14, 17}, {15, 17}, {16, 17}, {17, 17}, {18, 17}, {19, 17}},
     {{0, 8},   {1, 8},   {2, 8},   {3, 8},   {4, 8},   {5, 8},
      {6, 8},   {7, 8},   {8, 8},   {9, 8},   {10, 8},  {11, 8},
      {11, 9},  {11, 10}, {11, 11}, {11, 12}, {11, 13}, {11, 14},
      {11, 15}, {11, 16}, {11, 17}, {11, 18}, {12, 18}, {13, 18},
      {14, 18}, {15, 18}, {16, 18}, {17, 18}, {18, 18}, {19, 18}},
     {{0, 9},   {1, 9},   {2, 9},   {3, 9},   {4, 9},   {5, 9},
      {6, 9},   {7, 9},   {8, 9},   {9, 9},   {10, 9},  {10, 10},
      {10, 11}, {10, 12}, {10, 13}, {10, 14}, {10, 15}, {10, 16},
      {10, 17}, {10, 18}, {10, 19}, {11, 19}, {12, 19}, {13, 19},
      {14, 19}, {15, 19}, {16, 19}, {17, 19}, {18, 19}, {19, 19}},
     {{0, 10}, {1, 10}, {2, 10}, {3, 10}, {4, 10}, {5, 10}, {6, 10}, {7, 10},
      {8, 10}, {9, 10}, {9, 11}, {9, 12}, {9, 13}, {9, 14}, {9, 15}, {9, 16},
      {9, 17}, {9, 18}, {9, 19}, {8, 19}, {7, 19}, {6, 19}, {5, 19}, {4, 19},
      {3, 19}, {2, 19}, {1, 19}, {0, 19}, {0, 18}, {0, 17}},
     {{0, 11}, {1, 11}, {2, 11}, {3, 11}, {4, 11}, {5, 11}, {6, 11}, {7, 11},
      {8, 11}, {8, 12}, {8, 13}, {8, 14}, {8, 15}, {8, 16}, {8, 17}, {8, 18},
      {7, 18}, {6, 18}, {5, 18}, {4, 18}, {3, 18}, {2, 18}, {1, 18}, {1, 17},
      {1, 16}, {0, 16}, {0, 15}, {0, 14}, {0, 13}, {1, 13}},
     {{0, 12}, {1, 12}, {2, 12}, {3, 12}, {4, 12}, {5, 12}, {6, 12}, {7, 12},
      {7, 13}, {7, 14}, {7, 15}, {7, 16}, {7, 17}, {6, 17}, {5, 17}, {4, 17},
      {3, 17}, {2, 17}, {2, 16}, {2, 15}, {1, 15}, {1, 14}, {2, 14}, {2, 13},
      {3, 13}, {4, 13}, {5, 13}, {6, 13}, {6, 14}, {6, 15}}}
    // K=14
    ,
    {{{0, 0},  {1, 0},  {2, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},
      {7, 0},  {8, 0},  {9, 0},  {10, 0}, {11, 0}, {12, 0}, {13, 0},
      {14, 0}, {15, 0}, {16, 0}, {17, 0}, {18, 0}, {19, 0}, {19, 1},
      {19, 2}, {19, 3}, {19, 4}, {19, 5}, {19, 6}, {19, 7}, {19, 8}},
     {{0, 1},  {1, 1},  {2, 1},  {3, 1},  {4, 1},  {5, 1},  {6, 1},
      {7, 1},  {8, 1},  {9, 1},  {10, 1}, {11, 1}, {12, 1}, {13, 1},
      {14, 1}, {15, 1}, {16, 1}, {17, 1}, {18, 1}, {18, 2}, {18, 3},
      {18, 4}, {18, 5}, {18, 6}, {18, 7}, {18, 8}, {18, 9}, {19, 9}},
     {{0, 2},  {1, 2},  {2, 2},  {3, 2},  {4, 2},   {5, 2},   {6, 2},
      {7, 2},  {8, 2},  {9, 2},  {10, 2}, {11, 2},  {12, 2},  {13, 2},
      {14, 2}, {15, 2}, {16, 2}, {17, 2}, {17, 3},  {17, 4},  {17, 5},
      {17, 6}, {17, 7}, {17, 8}, {17, 9}, {17, 10}, {18, 10}, {19, 10}},
     {{0, 3},  {1, 3},  {2, 3},   {3, 3},   {4, 3},   {5, 3},   {6, 3},
      {7, 3},  {8, 3},  {9, 3},   {10, 3},  {11, 3},  {12, 3},  {13, 3},
      {14, 3}, {15, 3}, {16, 3},  {16, 4},  {16, 5},  {16, 6},  {16, 7},
      {16, 8}, {16, 9}, {16, 10}, {16, 11}, {17, 11}, {18, 11}, {19, 11}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},   {5, 4},   {6, 4},
      {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4},  {12, 4},  {13, 4},
      {14, 4},  {15, 4},  {15, 5},  {15, 6},  {15, 7},  {15, 8},  {15, 9},
      {15, 10}, {15, 11}, {15, 12}, {16, 12}, {17, 12}, {18, 12}, {19, 12}},
     {{0, 5},   {1, 5},   {2, 5},   {3, 5},   {4, 5},   {5, 5},   {6, 5},
      {7, 5},   {8, 5},   {9, 5},   {10, 5},  {11, 5},  {12, 5},  {13, 5},
      {14, 5},  {14, 6},  {14, 7},  {14, 8},  {14, 9},  {14, 10}, {14, 11},
      {14, 12}, {14, 13}, {15, 13}, {16, 13}, {17, 13}, {18, 13}, {19, 13}},
     {{0, 6},   {1, 6},   {2, 6},   {3, 6},   {4, 6},   {5, 6},   {6, 6},
      {7, 6},   {8, 6},   {9, 6},   {10, 6},  {11, 6},  {12, 6},  {13, 6},
      {13, 7},  {13, 8},  {13, 9},  {13, 10}, {13, 11}, {13, 12}, {13, 13},
      {13, 14}, {14, 14}, {15, 14}, {16, 14}, {17, 14}, {18, 14}, {19, 14}},
     {{0, 7},   {1, 7},   {2, 7},   {3, 7},   {4, 7},   {5, 7},   {6, 7},
      {7, 7},   {8, 7},   {9, 7},   {10, 7},  {11, 7},  {12, 7},  {12, 8},
      {12, 9},  {12, 10}, {12, 11}, {12, 12}, {12, 13}, {12, 14}, {12, 15},
      {13, 15}, {14, 15}, {15, 15}, {16, 15}, {17, 15}, {18, 15}, {19, 15}},
     {{0, 8},   {1, 8},   {2, 8},   {3, 8},   {4, 8},   {5, 8},   {6, 8},
      {7, 8},   {8, 8},   {9, 8},   {10, 8},  {11, 8},  {11, 9},  {11, 10},
      {11, 11}, {11, 12}, {11, 13}, {11, 14}, {11, 15}, {11, 16}, {12, 16},
      {13, 16}, {14, 16}, {15, 16}, {16, 16}, {17, 16}, {18, 16}, {19, 16}},
     {{0, 9},   {1, 9},   {2, 9},   {3, 9},   {4, 9},   {5, 9},   {6, 9},
      {7, 9},   {8, 9},   {9, 9},   {10, 9},  {10, 10}, {10, 11}, {10, 12},
      {10, 13}, {10, 14}, {10, 15}, {10, 16}, {10, 17}, {11, 17}, {12, 17},
      {13, 17}, {14, 17}, {15, 17}, {16, 17}, {17, 17}, {18, 17}, {19, 17}},
     {{0, 10},  {1, 10},  {2, 10},  {3, 10},  {4, 10},  {5, 10},  {6, 10},
      {7, 10},  {8, 10},  {9, 10},  {9, 11},  {9, 12},  {9, 13},  {9, 14},
      {9, 15},  {9, 16},  {9, 17},  {9, 18},  {10, 18}, {11, 18}, {12, 18},
      {13, 18}, {14, 18}, {15, 18}, {16, 18}, {17, 18}, {18, 18}, {19, 18}},
     {{0, 11},  {1, 11},  {2, 11},  {3, 11},  {4, 11},  {5, 11},  {6, 11},
      {7, 11},  {8, 11},  {8, 12},  {8, 13},  {8, 14},  {8, 15},  {8, 16},
      {8, 17},  {8, 18},  {8, 19},  {9, 19},  {10, 19}, {11, 19}, {12, 19},
      {13, 19}, {14, 19}, {15, 19}, {16, 19}, {17, 19}, {18, 19}, {19, 19}},
     {{0, 12}, {1, 12}, {2, 12}, {3, 12}, {4, 12}, {5, 12}, {6, 12},
      {7, 12}, {7, 13}, {7, 14}, {7, 15}, {7, 16}, {7, 17}, {7, 18},
      {7, 19}, {6, 19}, {5, 19}, {4, 19}, {3, 19}, {2, 19}, {1, 19},
      {0, 19}, {0, 18}, {0, 17}, {0, 16}, {0, 15}, {0, 14}, {1, 14}},
     {{0, 13}, {1, 13}, {2, 13}, {3, 13}, {4, 13}, {5, 13}, {6, 13},
      {6, 14}, {6, 15}, {6, 16}, {6, 17}, {6, 18}, {5, 18}, {4, 18},
      {3, 18}, {2, 18}, {1, 18}, {1, 17}, {1, 16}, {1, 15}, {2, 15},
      {2, 14}, {3, 14}, {4, 14}, {5, 14}, {5, 15}, {5, 16}, {5, 17}}}
    // K=15
    ,
    {{{0, 0},  {1, 0},  {2, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},
      {7, 0},  {8, 0},  {9, 0},  {10, 0}, {11, 0}, {12, 0}, {13, 0},
      {14, 0}, {15, 0}, {16, 0}, {17, 0}, {18, 0}, {19, 0}, {19, 1},
      {19, 2}, {19, 3}, {19, 4}, {19, 5}, {19, 6}},
     {{0, 1},  {1, 1},  {2, 1},  {3, 1},  {4, 1},  {5, 1},  {6, 1},
      {7, 1},  {8, 1},  {9, 1},  {10, 1}, {11, 1}, {12, 1}, {13, 1},
      {14, 1}, {15, 1}, {16, 1}, {17, 1}, {18, 1}, {18, 2}, {18, 3},
      {18, 4}, {18, 5}, {18, 6}, {18, 7}, {19, 7}},
     {{0, 2},  {1, 2},  {2, 2},  {3, 2},  {4, 2},  {5, 2},  {6, 2},
      {7, 2},  {8, 2},  {9, 2},  {10, 2}, {11, 2}, {12, 2}, {13, 2},
      {14, 2}, {15, 2}, {16, 2}, {17, 2}, {17, 3}, {17, 4}, {17, 5},
      {17, 6}, {17, 7}, {17, 8}, {18, 8}, {19, 8}},
     {{0, 3},  {1, 3},  {2, 3},  {3, 3},  {4, 3},  {5, 3},  {6, 3},
      {7, 3},  {8, 3},  {9, 3},  {10, 3}, {11, 3}, {12, 3}, {13, 3},
      {14, 3}, {15, 3}, {16, 3}, {16, 4}, {16, 5}, {16, 6}, {16, 7},
      {16, 8}, {16, 9}, {17, 9}, {18, 9}, {19, 9}},
     {{0, 4},   {1, 4},   {2, 4},   {3, 4},   {4, 4},  {5, 4},  {6, 4},
      {7, 4},   {8, 4},   {9, 4},   {10, 4},  {11, 4}, {12, 4}, {13, 4},
      {14, 4},  {15, 4},  {15, 5},  {15, 6},  {15, 7}, {15, 8}, {15, 9},
      {15, 10}, {16, 10}, {17, 10}, {18, 10}, {19, 10}},
     {{0, 5},   {1, 5},   {2, 5},   {3, 5},   {4, 5},  {5, 5},   {6, 5},
      {7, 5},   {8, 5},   {9, 5},   {10, 5},  {11, 5}, {12, 5},  {13, 5},
      {14, 5},  {14, 6},  {14, 7},  {14, 8},  {14, 9}, {14, 10}, {14, 11},
      {15, 11}, {16, 11}, {17, 11}, {18, 11}, {19, 11}},
     {{0, 6},   {1, 6},   {2, 6},   {3, 6},   {4, 6},   {5, 6},   {6, 6},
      {7, 6},   {8, 6},   {9, 6},   {10, 6},  {11, 6},  {12, 6},  {13, 6},
      {13, 7},  {13, 8},  {13, 9},  {13, 10}, {13, 11}, {13, 12}, {14, 12},
      {15, 12}, {16, 12}, {17, 12}, {18, 12}, {19, 12}},
     {{0, 7},   {1, 7},   {2, 7},   {3, 7},   {4, 7},   {5, 7},   {6, 7},
      {7, 7},   {8, 7},   {9, 7},   {10, 7},  {11, 7},  {12, 7},  {12, 8},
      {12, 9},  {12, 10}, {12, 11}, {12, 12}, {12, 13}, {13, 13}, {14, 13},
      {15, 13}, {16, 13}, {17, 13}, {18, 13}, {19, 13}},
     {{0, 8},   {1, 8},   {2, 8},   {3, 8},   {4, 8},   {5, 8},   {6, 8},
      {7, 8},   {8, 8},   {9, 8},   {10, 8},  {11, 8},  {11, 9},  {11, 10},
      {11, 11}, {11, 12}, {11, 13}, {11, 14}, {12, 14}, {13, 14}, {14, 14},
      {15, 14}, {16, 14}, {17, 14}, {18, 14}, {19, 14}},
     {{0, 9},   {1, 9},   {2, 9},   {3, 9},   {4, 9},   {5, 9},   {6, 9},
      {7, 9},   {8, 9},   {9, 9},   {10, 9},  {10, 10}, {10, 11}, {10, 12},
      {10, 13}, {10, 14}, {10, 15}, {11, 15}, {12, 15}, {13, 15}, {14, 15},
      {15, 15}, {16, 15}, {17, 15}, {18, 15}, {19, 15}},
     {{0, 10},  {1, 10},  {2, 10},  {3, 10},  {4, 10},  {5, 10},  {6, 10},
      {7, 10},  {8, 10},  {9, 10},  {9, 11},  {9, 12},  {9, 13},  {9, 14},
      {9, 15},  {9, 16},  {10, 16}, {11, 16}, {12, 16}, {13, 16}, {14, 16},
      {15, 16}, {16, 16}, {17, 16}, {18, 16}, {19, 16}},
     {{0, 11},  {1, 11},  {2, 11},  {3, 11},  {4, 11},  {5, 11},  {6, 11},
      {7, 11},  {8, 11},  {8, 12},  {8, 13},  {8, 14},  {8, 15},  {8, 16},
      {8, 17},  {9, 17},  {10, 17}, {11, 17}, {12, 17}, {13, 17}, {14, 17},
      {15, 17}, {16, 17}, {17, 17}, {18, 17}, {19, 17}},
     {{0, 12},  {1, 12},  {2, 12},  {3, 12},  {4, 12},  {5, 12},  {6, 12},
      {7, 12},  {7, 13},  {7, 14},  {7, 15},  {7, 16},  {7, 17},  {7, 18},
      {8, 18},  {9, 18},  {10, 18}, {11, 18}, {12, 18}, {13, 18}, {14, 18},
      {15, 18}, {16, 18}, {17, 18}, {18, 18}, {19, 18}},
     {{0, 13},  {1, 13},  {2, 13},  {3, 13},  {4, 13},  {5, 13},  {6, 13},
      {6, 14},  {6, 15},  {6, 16},  {6, 17},  {6, 18},  {6, 19},  {7, 19},
      {8, 19},  {9, 19},  {10, 19}, {11, 19}, {12, 19}, {13, 19}, {14, 19},
      {15, 19}, {16, 19}, {17, 19}, {18, 19}, {19, 19}},
     {{0, 14}, {1, 14}, {2, 14}, {3, 14}, {4, 14}, {5, 14}, {5, 15},
      {5, 16}, {5, 17}, {5, 18}, {5, 19}, {4, 19}, {3, 19}, {2, 19},
      {1, 19}, {0, 19}, {0, 18}, {0, 17}, {0, 16}, {0, 15}, {1, 15},
      {2, 15}, {3, 15}, {4, 15}, {4, 16}, {4, 17}}}
    // K=16
    ,
    {{{0, 0},  {1, 0},  {2, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},
      {7, 0},  {8, 0},  {9, 0},  {10, 0}, {11, 0}, {12, 0}, {13, 0},
      {14, 0}, {15, 0}, {16, 0}, {17, 0}, {18, 0}, {19, 0}, {19, 1},
      {19, 2}, {19, 3}, {19, 4}, {19, 5}},
     {{0, 1},  {1, 1},  {2, 1},  {3, 1},  {4, 1},  {5, 1},  {6, 1},
      {7, 1},  {8, 1},  {9, 1},  {10, 1}, {11, 1}, {12, 1}, {13, 1},
      {14, 1}, {15, 1}, {16, 1}, {17, 1}, {18, 1}, {18, 2}, {18, 3},
      {18, 4}, {18, 5}, {18, 6}, {19, 6}},
     {{0, 2},  {1, 2},  {2, 2},  {3, 2},  {4, 2},  {5, 2},  {6, 2},
      {7, 2},  {8, 2},  {9, 2},  {10, 2}, {11, 2}, {12, 2}, {13, 2},
      {14, 2}, {15, 2}, {16, 2}, {17, 2}, {17, 3}, {17, 4}, {17, 5},
      {17, 6}, {17, 7}, {18, 7}, {19, 7}},
     {{0, 3},  {1, 3},  {2, 3},  {3, 3},  {4, 3},  {5, 3},  {6, 3},
      {7, 3},  {8, 3},  {9, 3},  {10, 3}, {11, 3}, {12, 3}, {13, 3},
      {14, 3}, {15, 3}, {16, 3}, {16, 4}, {16, 5}, {16, 6}, {16, 7},
      {16, 8}, {17, 8}, {18, 8}, {19, 8}},
     {{0, 4},  {1, 4},  {2, 4},  {3, 4},  {4, 4},  {5, 4},  {6, 4},
      {7, 4},  {8, 4},  {9, 4},  {10, 4}, {11, 4}, {12, 4}, {13, 4},
      {14, 4}, {15, 4}, {15, 5}, {15, 6}, {15, 7}, {15, 8}, {15, 9},
      {16, 9}, {17, 9}, {18, 9}, {19, 9}},
     {{0, 5},   {1, 5},   {2, 5},   {3, 5},  {4, 5},  {5, 5},   {6, 5},
      {7, 5},   {8, 5},   {9, 5},   {10, 5}, {11, 5}, {12, 5},  {13, 5},
      {14, 5},  {14, 6},  {14, 7},  {14, 8}, {14, 9}, {14, 10}, {15, 10},
      {16, 10}, {17, 10}, {18, 10}, {19, 10}},
     {{0, 6},   {1, 6},   {2, 6},   {3, 6},   {4, 6},   {5, 6},   {6, 6},
      {7, 6},   {8, 6},   {9, 6},   {10, 6},  {11, 6},  {12, 6},  {13, 6},
      {13, 7},  {13, 8},  {13, 9},  {13, 10}, {13, 11}, {14, 11}, {15, 11},
      {16, 11}, {17, 11}, {18, 11}, {19, 11}},
     {{0, 7},   {1, 7},   {2, 7},   {3, 7},   {4, 7},   {5, 7},   {6, 7},
      {7, 7},   {8, 7},   {9, 7},   {10, 7},  {11, 7},  {12, 7},  {12, 8},
      {12, 9},  {12, 10}, {12, 11}, {12, 12}, {13, 12}, {14, 12}, {15, 12},
      {16, 12}, {17, 12}, {18, 12}, {19, 12}},
     {{0, 8},   {1, 8},   {2, 8},   {3, 8},   {4, 8},   {5, 8},   {6, 8},
      {7, 8},   {8, 8},   {9, 8},   {10, 8},  {11, 8},  {11, 9},  {11, 10},
      {11, 11}, {11, 12}, {11, 13}, {12, 13}, {13, 13}, {14, 13}, {15, 13},
      {16, 13}, {17, 13}, {18, 13}, {19, 13}},
     {{0, 9},   {1, 9},   {2, 9},   {3, 9},   {4, 9},   {5, 9},   {6, 9},
      {7, 9},   {8, 9},   {9, 9},   {10, 9},  {10, 10}, {10, 11}, {10, 12},
      {10, 13}, {10, 14}, {11, 14}, {12, 14}, {13, 14}, {14, 14}, {15, 14},
      {16, 14}, {17, 14}, {18, 14}, {19, 14}},
     {{0, 10},  {1, 10},  {2, 10},  {3, 10},  {4, 10},  {5, 10},  {6, 10},
      {7, 10},  {8, 10},  {9, 10},  {9, 11},  {9, 12},  {9, 13},  {9, 14},
      {9, 15},  {10, 15}, {11, 15}, {12, 15}, {13, 15}, {14, 15}, {15, 15},
      {16, 15}, {17, 15}, {18, 15}, {19, 15}},
     {{0, 11},  {1, 11},  {2, 11},  {3, 11},  {4, 11},  {5, 11},  {6, 11},
      {7, 11},  {8, 11},  {8, 12},  {8, 13},  {8, 14},  {8, 15},  {8, 16},
      {9, 16},  {10, 16}, {11, 16}, {12, 16}, {13, 16}, {14, 16}, {15, 16},
      {16, 16}, {17, 16}, {18, 16}, {19, 16}},
     {{0, 12},  {1, 12},  {2, 12},  {3, 12},  {4, 12},  {5, 12},  {6, 12},
      {7, 12},  {7, 13},  {7, 14},  {7, 15},  {7, 16},  {7, 17},  {8, 17},
      {9, 17},  {10, 17}, {11, 17}, {12, 17}, {13, 17}, {14, 17}, {15, 17},
      {16, 17}, {17, 17}, {18, 17}, {19, 17}},
     {{0, 13},  {1, 13},  {2, 13},  {3, 13},  {4, 13},  {5, 13},  {6, 13},
      {6, 14},  {6, 15},  {6, 16},  {6, 17},  {6, 18},  {7, 18},  {8, 18},
      {9, 18},  {10, 18}, {11, 18}, {12, 18}, {13, 18}, {14, 18}, {15, 18},
      {16, 18}, {17, 18}, {18, 18}, {19, 18}},
     {{0, 14},  {1, 14},  {2, 14},  {3, 14},  {4, 14},  {5, 14},  {5, 15},
      {5, 16},  {5, 17},  {5, 18},  {5, 19},  {6, 19},  {7, 19},  {8, 19},
      {9, 19},  {10, 19}, {11, 19}, {12, 19}, {13, 19}, {14, 19}, {15, 19},
      {16, 19}, {17, 19}, {18, 19}, {19, 19}},
     {{0, 15}, {1, 15}, {2, 15}, {3, 15}, {4, 15}, {4, 16}, {4, 17},
      {4, 18}, {4, 19}, {3, 19}, {2, 19}, {1, 19}, {0, 19}, {0, 18},
      {0, 17}, {0, 16}, {1, 16}, {2, 16}, {3, 16}, {3, 17}, {3, 18},
      {2, 18}, {1, 18}, {1, 17}, {2, 17}}}
    // K=17
    ,
    {{{0, 0},  {1, 0},  {2, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},  {7, 0},
      {8, 0},  {9, 0},  {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0},
      {16, 0}, {17, 0}, {18, 0}, {19, 0}, {19, 1}, {19, 2}, {19, 3}},
     {{0, 1},  {1, 1},  {2, 1},  {3, 1},  {4, 1},  {5, 1},  {6, 1},  {7, 1},
      {8, 1},  {9, 1},  {10, 1}, {11, 1}, {12, 1}, {13, 1}, {14, 1}, {15, 1},
      {16, 1}, {17, 1}, {18, 1}, {18, 2}, {18, 3}, {18, 4}, {19, 4}},
     {{0, 2},  {1, 2},  {2, 2},  {3, 2},  {4, 2},  {5, 2},  {6, 2},  {7, 2},
      {8, 2},  {9, 2},  {10, 2}, {11, 2}, {12, 2}, {13, 2}, {14, 2}, {15, 2},
      {16, 2}, {17, 2}, {17, 3}, {17, 4}, {17, 5}, {18, 5}, {19, 5}},
     {{0, 3},  {1, 3},  {2, 3},  {3, 3},  {4, 3},  {5, 3},  {6, 3},  {7, 3},
      {8, 3},  {9, 3},  {10, 3}, {11, 3}, {12, 3}, {13, 3}, {14, 3}, {15, 3},
      {16, 3}, {16, 4}, {16, 5}, {16, 6}, {17, 6}, {18, 6}, {19, 6}},
     {{0, 4},  {1, 4},  {2, 4},  {3, 4},  {4, 4},  {5, 4},  {6, 4},  {7, 4},
      {8, 4},  {9, 4},  {10, 4}, {11, 4}, {12, 4}, {13, 4}, {14, 4}, {15, 4},
      {15, 5}, {15, 6}, {15, 7}, {16, 7}, {17, 7}, {18, 7}, {19, 7}},
     {{0, 5},  {1, 5},  {2, 5},  {3, 5},  {4, 5},  {5, 5},  {6, 5},  {7, 5},
      {8, 5},  {9, 5},  {10, 5}, {11, 5}, {12, 5}, {13, 5}, {14, 5}, {14, 6},
      {14, 7}, {14, 8}, {15, 8}, {16, 8}, {17, 8}, {18, 8}, {19, 8}},
     {{0, 6},  {1, 6},  {2, 6},  {3, 6},  {4, 6},  {5, 6},  {6, 6},  {7, 6},
      {8, 6},  {9, 6},  {10, 6}, {11, 6}, {12, 6}, {13, 6}, {13, 7}, {13, 8},
      {13, 9}, {14, 9}, {15, 9}, {16, 9}, {17, 9}, {18, 9}, {19, 9}},
     {{0, 7},   {1, 7},   {2, 7},   {3, 7},   {4, 7},   {5, 7},
      {6, 7},   {7, 7},   {8, 7},   {9, 7},   {10, 7},  {11, 7},
      {12, 7},  {12, 8},  {12, 9},  {12, 10}, {13, 10}, {14, 10},
      {15, 10}, {16, 10}, {17, 10}, {18, 10}, {19, 10}},
     {{0, 8},   {1, 8},   {2, 8},   {3, 8},   {4, 8},   {5, 8},
      {6, 8},   {7, 8},   {8, 8},   {9, 8},   {10, 8},  {11, 8},
      {11, 9},  {11, 10}, {11, 11}, {12, 11}, {13, 11}, {14, 11},
      {15, 11}, {16, 11}, {17, 11}, {18, 11}, {19, 11}},
     {{0, 9},   {1, 9},   {2, 9},   {3, 9},   {4, 9},   {5, 9},
      {6, 9},   {7, 9},   {8, 9},   {9, 9},   {10, 9},  {10, 10},
      {10, 11}, {10, 12}, {11, 12}, {12, 12}, {13, 12}, {14, 12},
      {15, 12}, {16, 12}, {17, 12}, {18, 12}, {19, 12}},
     {{0, 10},  {1, 10},  {2, 10},  {3, 10},  {4, 10},  {5, 10},
      {6, 10},  {7, 10},  {8, 10},  {9, 10},  {9, 11},  {9, 12},
      {9, 13},  {10, 13}, {11, 13}, {12, 13}, {13, 13}, {14, 13},
      {15, 13}, {16, 13}, {17, 13}, {18, 13}, {19, 13}},
     {{0, 11},  {1, 11},  {2, 11},  {3, 11},  {4, 11},  {5, 11},
      {6, 11},  {7, 11},  {8, 11},  {8, 12},  {8, 13},  {8, 14},
      {9, 14},  {10, 14}, {11, 14}, {12, 14}, {13, 14}, {14, 14},
      {15, 14}, {16, 14}, {17, 14}, {18, 14}, {19, 14}},
     {{0, 12},  {1, 12},  {2, 12},  {3, 12},  {4, 12},  {5, 12},
      {6, 12},  {7, 12},  {7, 13},  {7, 14},  {7, 15},  {8, 15},
      {9, 15},  {10, 15}, {11, 15}, {12, 15}, {13, 15}, {14, 15},
      {15, 15}, {16, 15}, {17, 15}, {18, 15}, {19, 15}},
     {{0, 13},  {1, 13},  {2, 13},  {3, 13},  {4, 13},  {5, 13},
      {6, 13},  {6, 14},  {6, 15},  {6, 16},  {7, 16},  {8, 16},
      {9, 16},  {10, 16}, {11, 16}, {12, 16}, {13, 16}, {14, 16},
      {15, 16}, {16, 16}, {17, 16}, {18, 16}, {19, 16}},
     {{0, 14},  {1, 14},  {2, 14},  {3, 14},  {4, 14},  {5, 14},
      {5, 15},  {5, 16},  {5, 17},  {6, 17},  {7, 17},  {8, 17},
      {9, 17},  {10, 17}, {11, 17}, {12, 17}, {13, 17}, {14, 17},
      {15, 17}, {16, 17}, {17, 17}, {18, 17}, {19, 17}},
     {{0, 15},  {1, 15},  {2, 15},  {3, 15},  {4, 15},  {4, 16},
      {4, 17},  {4, 18},  {5, 18},  {6, 18},  {7, 18},  {8, 18},
      {9, 18},  {10, 18}, {11, 18}, {12, 18}, {13, 18}, {14, 18},
      {15, 18}, {16, 18}, {17, 18}, {18, 18}, {19, 18}},
     {{0, 16},  {1, 16},  {2, 16},  {3, 16},  {3, 17},  {3, 18},
      {3, 19},  {4, 19},  {5, 19},  {6, 19},  {7, 19},  {8, 19},
      {9, 19},  {10, 19}, {11, 19}, {12, 19}, {13, 19}, {14, 19},
      {15, 19}, {16, 19}, {17, 19}, {18, 19}, {19, 19}}}
    // K=18
    ,
    {{{0, 0},  {1, 0},  {2, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},  {7, 0},
      {8, 0},  {9, 0},  {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0},
      {16, 0}, {17, 0}, {18, 0}, {19, 0}, {19, 1}, {19, 2}},
     {{0, 1},  {1, 1},  {2, 1},  {3, 1},  {4, 1},  {5, 1},  {6, 1},  {7, 1},
      {8, 1},  {9, 1},  {10, 1}, {11, 1}, {12, 1}, {13, 1}, {14, 1}, {15, 1},
      {16, 1}, {17, 1}, {18, 1}, {18, 2}, {18, 3}, {19, 3}},
     {{0, 2},  {1, 2},  {2, 2},  {3, 2},  {4, 2},  {5, 2},  {6, 2},  {7, 2},
      {8, 2},  {9, 2},  {10, 2}, {11, 2}, {12, 2}, {13, 2}, {14, 2}, {15, 2},
      {16, 2}, {17, 2}, {17, 3}, {17, 4}, {18, 4}, {19, 4}},
     {{0, 3},  {1, 3},  {2, 3},  {3, 3},  {4, 3},  {5, 3},  {6, 3},  {7, 3},
      {8, 3},  {9, 3},  {10, 3}, {11, 3}, {12, 3}, {13, 3}, {14, 3}, {15, 3},
      {16, 3}, {16, 4}, {16, 5}, {17, 5}, {18, 5}, {19, 5}},
     {{0, 4},  {1, 4},  {2, 4},  {3, 4},  {4, 4},  {5, 4},  {6, 4},  {7, 4},
      {8, 4},  {9, 4},  {10, 4}, {11, 4}, {12, 4}, {13, 4}, {14, 4}, {15, 4},
      {15, 5}, {15, 6}, {16, 6}, {17, 6}, {18, 6}, {19, 6}},
     {{0, 5},  {1, 5},  {2, 5},  {3, 5},  {4, 5},  {5, 5},  {6, 5},  {7, 5},
      {8, 5},  {9, 5},  {10, 5}, {11, 5}, {12, 5}, {13, 5}, {14, 5}, {14, 6},
      {14, 7}, {15, 7}, {16, 7}, {17, 7}, {18, 7}, {19, 7}},
     {{0, 6},  {1, 6},  {2, 6},  {3, 6},  {4, 6},  {5, 6},  {6, 6},  {7, 6},
      {8, 6},  {9, 6},  {10, 6}, {11, 6}, {12, 6}, {13, 6}, {13, 7}, {13, 8},
      {14, 8}, {15, 8}, {16, 8}, {17, 8}, {18, 8}, {19, 8}},
     {{0, 7},  {1, 7},  {2, 7},  {3, 7},  {4, 7},  {5, 7},  {6, 7},  {7, 7},
      {8, 7},  {9, 7},  {10, 7}, {11, 7}, {12, 7}, {12, 8}, {12, 9}, {13, 9},
      {14, 9}, {15, 9}, {16, 9}, {17, 9}, {18, 9}, {19, 9}},
     {{0, 8},   {1, 8},   {2, 8},   {3, 8},   {4, 8},   {5, 8},
      {6, 8},   {7, 8},   {8, 8},   {9, 8},   {10, 8},  {11, 8},
      {11, 9},  {11, 10}, {12, 10}, {13, 10}, {14, 10}, {15, 10},
      {16, 10}, {17, 10}, {18, 10}, {19, 10}},
     {{0, 9},   {1, 9},   {2, 9},   {3, 9},   {4, 9},   {5, 9},
      {6, 9},   {7, 9},   {8, 9},   {9, 9},   {10, 9},  {10, 10},
      {10, 11}, {11, 11}, {12, 11}, {13, 11}, {14, 11}, {15, 11},
      {16, 11}, {17, 11}, {18, 11}, {19, 11}},
     {{0, 10},  {1, 10},  {2, 10},  {3, 10},  {4, 10},  {5, 10},
      {6, 10},  {7, 10},  {8, 10},  {9, 10},  {9, 11},  {9, 12},
      {10, 12}, {11, 12}, {12, 12}, {13, 12}, {14, 12}, {15, 12},
      {16, 12}, {17, 12}, {18, 12}, {19, 12}},
     {{0, 11},  {1, 11},  {2, 11},  {3, 11},  {4, 11},  {5, 11},
      {6, 11},  {7, 11},  {8, 11},  {8, 12},  {8, 13},  {9, 13},
      {10, 13}, {11, 13}, {12, 13}, {13, 13}, {14, 13}, {15, 13},
      {16, 13}, {17, 13}, {18, 13}, {19, 13}},
     {{0, 12},  {1, 12},  {2, 12},  {3, 12},  {4, 12},  {5, 12},
      {6, 12},  {7, 12},  {7, 13},  {7, 14},  {8, 14},  {9, 14},
      {10, 14}, {11, 14}, {12, 14}, {13, 14}, {14, 14}, {15, 14},
      {16, 14}, {17, 14}, {18, 14}, {19, 14}},
     {{0, 13},  {1, 13},  {2, 13},  {3, 13},  {4, 13},  {5, 13},
      {6, 13},  {6, 14},  {6, 15},  {7, 15},  {8, 15},  {9, 15},
      {10, 15}, {11, 15}, {12, 15}, {13, 15}, {14, 15}, {15, 15},
      {16, 15}, {17, 15}, {18, 15}, {19, 15}},
     {{0, 14},  {1, 14},  {2, 14},  {3, 14},  {4, 14},  {5, 14},
      {5, 15},  {5, 16},  {6, 16},  {7, 16},  {8, 16},  {9, 16},
      {10, 16}, {11, 16}, {12, 16}, {13, 16}, {14, 16}, {15, 16},
      {16, 16}, {17, 16}, {18, 16}, {19, 16}},
     {{0, 15},  {1, 15},  {2, 15},  {3, 15},  {4, 15},  {4, 16},
      {4, 17},  {5, 17},  {6, 17},  {7, 17},  {8, 17},  {9, 17},
      {10, 17}, {11, 17}, {12, 17}, {13, 17}, {14, 17}, {15, 17},
      {16, 17}, {17, 17}, {18, 17}, {19, 17}},
     {{0, 16},  {1, 16},  {2, 16},  {3, 16},  {3, 17},  {3, 18},
      {4, 18},  {5, 18},  {6, 18},  {7, 18},  {8, 18},  {9, 18},
      {10, 18}, {11, 18}, {12, 18}, {13, 18}, {14, 18}, {15, 18},
      {16, 18}, {17, 18}, {18, 18}, {19, 18}},
     {{0, 17},  {1, 17},  {2, 17},  {2, 18},  {2, 19},  {3, 19},
      {4, 19},  {5, 19},  {6, 19},  {7, 19},  {8, 19},  {9, 19},
      {10, 19}, {11, 19}, {12, 19}, {13, 19}, {14, 19}, {15, 19},
      {16, 19}, {17, 19}, {18, 19}, {19, 19}}}
    // K=19
    ,
    {{{0, 0},  {1, 0},  {2, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},
      {7, 0},  {8, 0},  {9, 0},  {10, 0}, {11, 0}, {12, 0}, {13, 0},
      {14, 0}, {15, 0}, {16, 0}, {17, 0}, {18, 0}, {19, 0}, {19, 1}},
     {{0, 1},  {1, 1},  {2, 1},  {3, 1},  {4, 1},  {5, 1},  {6, 1},
      {7, 1},  {8, 1},  {9, 1},  {10, 1}, {11, 1}, {12, 1}, {13, 1},
      {14, 1}, {15, 1}, {16, 1}, {17, 1}, {18, 1}, {18, 2}, {19, 2}},
     {{0, 2},  {1, 2},  {2, 2},  {3, 2},  {4, 2},  {5, 2},  {6, 2},
      {7, 2},  {8, 2},  {9, 2},  {10, 2}, {11, 2}, {12, 2}, {13, 2},
      {14, 2}, {15, 2}, {16, 2}, {17, 2}, {17, 3}, {18, 3}, {19, 3}},
     {{0, 3},  {1, 3},  {2, 3},  {3, 3},  {4, 3},  {5, 3},  {6, 3},
      {7, 3},  {8, 3},  {9, 3},  {10, 3}, {11, 3}, {12, 3}, {13, 3},
      {14, 3}, {15, 3}, {16, 3}, {16, 4}, {17, 4}, {18, 4}, {19, 4}},
     {{0, 4},  {1, 4},  {2, 4},  {3, 4},  {4, 4},  {5, 4},  {6, 4},
      {7, 4},  {8, 4},  {9, 4},  {10, 4}, {11, 4}, {12, 4}, {13, 4},
      {14, 4}, {15, 4}, {15, 5}, {16, 5}, {17, 5}, {18, 5}, {19, 5}},
     {{0, 5},  {1, 5},  {2, 5},  {3, 5},  {4, 5},  {5, 5},  {6, 5},
      {7, 5},  {8, 5},  {9, 5},  {10, 5}, {11, 5}, {12, 5}, {13, 5},
      {14, 5}, {14, 6}, {15, 6}, {16, 6}, {17, 6}, {18, 6}, {19, 6}},
     {{0, 6},  {1, 6},  {2, 6},  {3, 6},  {4, 6},  {5, 6},  {6, 6},
      {7, 6},  {8, 6},  {9, 6},  {10, 6}, {11, 6}, {12, 6}, {13, 6},
      {13, 7}, {14, 7}, {15, 7}, {16, 7}, {17, 7}, {18, 7}, {19, 7}},
     {{0, 7},  {1, 7},  {2, 7},  {3, 7},  {4, 7},  {5, 7},  {6, 7},
      {7, 7},  {8, 7},  {9, 7},  {10, 7}, {11, 7}, {12, 7}, {12, 8},
      {13, 8}, {14, 8}, {15, 8}, {16, 8}, {17, 8}, {18, 8}, {19, 8}},
     {{0, 8},  {1, 8},  {2, 8},  {3, 8},  {4, 8},  {5, 8},  {6, 8},
      {7, 8},  {8, 8},  {9, 8},  {10, 8}, {11, 8}, {11, 9}, {12, 9},
      {13, 9}, {14, 9}, {15, 9}, {16, 9}, {17, 9}, {18, 9}, {19, 9}},
     {{0, 9},   {1, 9},   {2, 9},   {3, 9},   {4, 9},   {5, 9},   {6, 9},
      {7, 9},   {8, 9},   {9, 9},   {10, 9},  {10, 10}, {11, 10}, {12, 10},
      {13, 10}, {14, 10}, {15, 10}, {16, 10}, {17, 10}, {18, 10}, {19, 10}},
     {{0, 10},  {1, 10},  {2, 10},  {3, 10},  {4, 10},  {5, 10},  {6, 10},
      {7, 10},  {8, 10},  {9, 10},  {9, 11},  {10, 11}, {11, 11}, {12, 11},
      {13, 11}, {14, 11}, {15, 11}, {16, 11}, {17, 11}, {18, 11}, {19, 11}},
     {{0, 11},  {1, 11},  {2, 11},  {3, 11},  {4, 11},  {5, 11},  {6, 11},
      {7, 11},  {8, 11},  {8, 12},  {9, 12},  {10, 12}, {11, 12}, {12, 12},
      {13, 12}, {14, 12}, {15, 12}, {16, 12}, {17, 12}, {18, 12}, {19, 12}},
     {{0, 12},  {1, 12},  {2, 12},  {3, 12},  {4, 12},  {5, 12},  {6, 12},
      {7, 12},  {7, 13},  {8, 13},  {9, 13},  {10, 13}, {11, 13}, {12, 13},
      {13, 13}, {14, 13}, {15, 13}, {16, 13}, {17, 13}, {18, 13}, {19, 13}},
     {{0, 13},  {1, 13},  {2, 13},  {3, 13},  {4, 13},  {5, 13},  {6, 13},
      {6, 14},  {7, 14},  {8, 14},  {9, 14},  {10, 14}, {11, 14}, {12, 14},
      {13, 14}, {14, 14}, {15, 14}, {16, 14}, {17, 14}, {18, 14}, {19, 14}},
     {{0, 14},  {1, 14},  {2, 14},  {3, 14},  {4, 14},  {5, 14},  {5, 15},
      {6, 15},  {7, 15},  {8, 15},  {9, 15},  {10, 15}, {11, 15}, {12, 15},
      {13, 15}, {14, 15}, {15, 15}, {16, 15}, {17, 15}, {18, 15}, {19, 15}},
     {{0, 15},  {1, 15},  {2, 15},  {3, 15},  {4, 15},  {4, 16},  {5, 16},
      {6, 16},  {7, 16},  {8, 16},  {9, 16},  {10, 16}, {11, 16}, {12, 16},
      {13, 16}, {14, 16}, {15, 16}, {16, 16}, {17, 16}, {18, 16}, {19, 16}},
     {{0, 16},  {1, 16},  {2, 16},  {3, 16},  {3, 17},  {4, 17},  {5, 17},
      {6, 17},  {7, 17},  {8, 17},  {9, 17},  {10, 17}, {11, 17}, {12, 17},
      {13, 17}, {14, 17}, {15, 17}, {16, 17}, {17, 17}, {18, 17}, {19, 17}},
     {{0, 17},  {1, 17},  {2, 17},  {2, 18},  {3, 18},  {4, 18},  {5, 18},
      {6, 18},  {7, 18},  {8, 18},  {9, 18},  {10, 18}, {11, 18}, {12, 18},
      {13, 18}, {14, 18}, {15, 18}, {16, 18}, {17, 18}, {18, 18}, {19, 18}},
     {{0, 18},  {1, 18},  {1, 19},  {2, 19},  {3, 19},  {4, 19},  {5, 19},
      {6, 19},  {7, 19},  {8, 19},  {9, 19},  {10, 19}, {11, 19}, {12, 19},
      {13, 19}, {14, 19}, {15, 19}, {16, 19}, {17, 19}, {18, 19}, {19, 19}}}
    // K=20
    ,
    {{{0, 0},  {1, 0},  {2, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},
      {7, 0},  {8, 0},  {9, 0},  {10, 0}, {11, 0}, {12, 0}, {13, 0},
      {14, 0}, {15, 0}, {16, 0}, {17, 0}, {18, 0}, {19, 0}},
     {{0, 1},  {1, 1},  {2, 1},  {3, 1},  {4, 1},  {5, 1},  {6, 1},
      {7, 1},  {8, 1},  {9, 1},  {10, 1}, {11, 1}, {12, 1}, {13, 1},
      {14, 1}, {15, 1}, {16, 1}, {17, 1}, {18, 1}, {19, 1}},
     {{0, 2},  {1, 2},  {2, 2},  {3, 2},  {4, 2},  {5, 2},  {6, 2},
      {7, 2},  {8, 2},  {9, 2},  {10, 2}, {11, 2}, {12, 2}, {13, 2},
      {14, 2}, {15, 2}, {16, 2}, {17, 2}, {18, 2}, {19, 2}},
     {{0, 3},  {1, 3},  {2, 3},  {3, 3},  {4, 3},  {5, 3},  {6, 3},
      {7, 3},  {8, 3},  {9, 3},  {10, 3}, {11, 3}, {12, 3}, {13, 3},
      {14, 3}, {15, 3}, {16, 3}, {17, 3}, {18, 3}, {19, 3}},
     {{0, 4},  {1, 4},  {2, 4},  {3, 4},  {4, 4},  {5, 4},  {6, 4},
      {7, 4},  {8, 4},  {9, 4},  {10, 4}, {11, 4}, {12, 4}, {13, 4},
      {14, 4}, {15, 4}, {16, 4}, {17, 4}, {18, 4}, {19, 4}},
     {{0, 5},  {1, 5},  {2, 5},  {3, 5},  {4, 5},  {5, 5},  {6, 5},
      {7, 5},  {8, 5},  {9, 5},  {10, 5}, {11, 5}, {12, 5}, {13, 5},
      {14, 5}, {15, 5}, {16, 5}, {17, 5}, {18, 5}, {19, 5}},
     {{0, 6},  {1, 6},  {2, 6},  {3, 6},  {4, 6},  {5, 6},  {6, 6},
      {7, 6},  {8, 6},  {9, 6},  {10, 6}, {11, 6}, {12, 6}, {13, 6},
      {14, 6}, {15, 6}, {16, 6}, {17, 6}, {18, 6}, {19, 6}},
     {{0, 7},  {1, 7},  {2, 7},  {3, 7},  {4, 7},  {5, 7},  {6, 7},
      {7, 7},  {8, 7},  {9, 7},  {10, 7}, {11, 7}, {12, 7}, {13, 7},
      {14, 7}, {15, 7}, {16, 7}, {17, 7}, {18, 7}, {19, 7}},
     {{0, 8},  {1, 8},  {2, 8},  {3, 8},  {4, 8},  {5, 8},  {6, 8},
      {7, 8},  {8, 8},  {9, 8},  {10, 8}, {11, 8}, {12, 8}, {13, 8},
      {14, 8}, {15, 8}, {16, 8}, {17, 8}, {18, 8}, {19, 8}},
     {{0, 9},  {1, 9},  {2, 9},  {3, 9},  {4, 9},  {5, 9},  {6, 9},
      {7, 9},  {8, 9},  {9, 9},  {10, 9}, {11, 9}, {12, 9}, {13, 9},
      {14, 9}, {15, 9}, {16, 9}, {17, 9}, {18, 9}, {19, 9}},
     {{0, 10},  {1, 10},  {2, 10},  {3, 10},  {4, 10},  {5, 10},  {6, 10},
      {7, 10},  {8, 10},  {9, 10},  {10, 10}, {11, 10}, {12, 10}, {13, 10},
      {14, 10}, {15, 10}, {16, 10}, {17, 10}, {18, 10}, {19, 10}},
     {{0, 11},  {1, 11},  {2, 11},  {3, 11},  {4, 11},  {5, 11},  {6, 11},
      {7, 11},  {8, 11},  {9, 11},  {10, 11}, {11, 11}, {12, 11}, {13, 11},
      {14, 11}, {15, 11}, {16, 11}, {17, 11}, {18, 11}, {19, 11}},
     {{0, 12},  {1, 12},  {2, 12},  {3, 12},  {4, 12},  {5, 12},  {6, 12},
      {7, 12},  {8, 12},  {9, 12},  {10, 12}, {11, 12}, {12, 12}, {13, 12},
      {14, 12}, {15, 12}, {16, 12}, {17, 12}, {18, 12}, {19, 12}},
     {{0, 13},  {1, 13},  {2, 13},  {3, 13},  {4, 13},  {5, 13},  {6, 13},
      {7, 13},  {8, 13},  {9, 13},  {10, 13}, {11, 13}, {12, 13}, {13, 13},
      {14, 13}, {15, 13}, {16, 13}, {17, 13}, {18, 13}, {19, 13}},
     {{0, 14},  {1, 14},  {2, 14},  {3, 14},  {4, 14},  {5, 14},  {6, 14},
      {7, 14},  {8, 14},  {9, 14},  {10, 14}, {11, 14}, {12, 14}, {13, 14},
      {14, 14}, {15, 14}, {16, 14}, {17, 14}, {18, 14}, {19, 14}},
     {{0, 15},  {1, 15},  {2, 15},  {3, 15},  {4, 15},  {5, 15},  {6, 15},
      {7, 15},  {8, 15},  {9, 15},  {10, 15}, {11, 15}, {12, 15}, {13, 15},
      {14, 15}, {15, 15}, {16, 15}, {17, 15}, {18, 15}, {19, 15}},
     {{0, 16},  {1, 16},  {2, 16},  {3, 16},  {4, 16},  {5, 16},  {6, 16},
      {7, 16},  {8, 16},  {9, 16},  {10, 16}, {11, 16}, {12, 16}, {13, 16},
      {14, 16}, {15, 16}, {16, 16}, {17, 16}, {18, 16}, {19, 16}},
     {{0, 17},  {1, 17},  {2, 17},  {3, 17},  {4, 17},  {5, 17},  {6, 17},
      {7, 17},  {8, 17},  {9, 17},  {10, 17}, {11, 17}, {12, 17}, {13, 17},
      {14, 17}, {15, 17}, {16, 17}, {17, 17}, {18, 17}, {19, 17}},
     {{0, 18},  {1, 18},  {2, 18},  {3, 18},  {4, 18},  {5, 18},  {6, 18},
      {7, 18},  {8, 18},  {9, 18},  {10, 18}, {11, 18}, {12, 18}, {13, 18},
      {14, 18}, {15, 18}, {16, 18}, {17, 18}, {18, 18}, {19, 18}},
     {{0, 19},  {1, 19},  {2, 19},  {3, 19},  {4, 19},  {5, 19},  {6, 19},
      {7, 19},  {8, 19},  {9, 19},  {10, 19}, {11, 19}, {12, 19}, {13, 19},
      {14, 19}, {15, 19}, {16, 19}, {17, 19}, {18, 19}, {19, 19}}}};


using Recipe = array<int,4>; // チューブid,4つの組

// 4*4の Ax=b を解く関数
bool solve4x4(double A[4][4], double b[4], double x[4]) {
    const double EPS = 1e-9;
    
    // 前進消去
    for (int i = 0; i < 4; i++) {
        // ピボット選択
        int maxRow = i;
        for (int k = i + 1; k < 4; k++) {
            if (abs(A[k][i]) > abs(A[maxRow][i])) {
                maxRow = k;
            }
        }
        
        // 行交換
        if (maxRow != i) {
            for (int k = 0; k < 4; k++) {
                swap(A[i][k], A[maxRow][k]);
            }
            swap(b[i], b[maxRow]);
        }
        
        // 対角成分が0に近い場合は解なし
        if (abs(A[i][i]) < EPS) {
            return false;
        }
        
        // 前進消去
        for (int k = i + 1; k < 4; k++) {
            double factor = A[k][i] / A[i][i];
            for (int j = i; j < 4; j++) {
                A[k][j] -= factor * A[i][j];
            }
            b[k] -= factor * b[i];
        }
    }
    
    // 後退代入
    for (int i = 3; i >= 0; i--) {
        x[i] = b[i];
        for (int j = i + 1; j < 4; j++) {
            x[i] -= A[i][j] * x[j];
        }
        x[i] /= A[i][i];
    }
    
    return true;
}

// 4つの色を使って目標色を作る
array<double,4> relaxed_4mix_problem(Color c[4], const Color& target){
    double A[4][4] = {
        {c[0][0], c[1][0], c[2][0], c[3][0]},
        {c[0][1], c[1][1], c[2][1], c[3][1]},
        {c[0][2], c[1][2], c[2][2], c[3][2]},
        {1.0, 1.0, 1.0, 1.0}
    };
    double b[4] = {target[0], target[1], target[2], 1.0};
    double w[4];
    // 方程式を解く
    if(!solve4x4(A,b,w)){
        return {};
    }
    return {w[0], w[1], w[2], w[3]};
}

pair<pii,pii> closest_rational(double x, int bunbo_upper, int bunbo_lower = 2){
    #ifdef LOCAL
    assert(bunbo_upper > 0);
    #endif
    if(bunbo_lower<2) bunbo_lower = 2; // 分母は1以上

    int best_small_p = 0, best_small_q = bunbo_lower;
    int best_large_p = bunbo_lower, best_large_q = bunbo_lower;
    double best_small_err = abs(x - static_cast<double>(best_small_p) / best_small_q);
    double best_large_err = abs(x - static_cast<double>(best_large_p) / best_large_q);

    for (int q = bunbo_lower; q <= bunbo_upper; ++q) {
        int p_near = static_cast<int>(round(x * q));
        for (int p : {p_near - 1, p_near, p_near + 1}) {
            double err = abs(x - static_cast<double>(p) / q);
            if(p < x * q){
                if(chmin(best_small_err, err)){
                    best_small_p = p;
                    best_small_q = q;
                }
            }
            else{
                if(chmin(best_large_err, err)){
                    best_large_p = p;
                    best_large_q = q;
                }
            }
        }
    }

    return {{best_small_p, best_small_q}, {best_large_p, best_large_q}};
}

// 2段階で取りえる有理数を列挙する
const vector<tuple<double,pii,pii>>& valid_fractions(){
    static vector<tuple<double,pii,pii>> vf = []{
        vector<tuple<double,pii,pii>> v;
        int MAX_CELL = N*N/K - 1;
        int border = (MAX_CELL + 1) / 2;
        rep(p1,1,MAX_CELL+1)rep(q1,p1+1,MAX_CELL+1)rep(p2,1,MAX_CELL+1)rep(q2,max(p1+1,p2+1),MAX_CELL+1){
            if(q1+q2-p1>MAX_CELL) break;
            if(q1<border) continue;
            if(q1+q2-p1-p2>border) continue;
            v.emplace_back((double)(p1*p2)/(q1*q2),pii{p1,q1},pii{p2,q2});
        }
        sort(all(v));
        cerr << "v.size() = " << v.size() << endl;
        return v;
    }();
    return vf;
}

pair<pair<pii,pii>,pair<pii,pii>> closest_rational2(double x){
    const auto &vf = valid_fractions();
    auto it = lower_bound(all(vf),make_tuple(x,pii{0,0},pii{0,0}));
    pair<pii,pii> small,large;
    if(it!=vf.end()){
        large = {get<1>(*it),get<2>(*it)};
    }
    else{
        large = {pii{inf,inf},pii{inf,inf}};
    }
    if(it!=vf.begin()){
        --it;
        small = {get<1>(*it),get<2>(*it)};
    }
    else{
        small = {pii{0,0},pii{0,0}};
    }
    return {small, large};
}

// 2段階で取りえる有理数を列挙する
const vector<tuple<double,pii,pii>>& valid_fractions_free_lower(){
    static vector<tuple<double,pii,pii>> vf = []{
        vector<tuple<double,pii,pii>> v;
        int MAX_CELL = N*N/K - 1;
        rep(p1,1,MAX_CELL+1)rep(q1,p1+1,MAX_CELL+1)rep(p2,1,MAX_CELL+1)rep(q2,max(p1+1,p2+1),MAX_CELL+1){
            if(q1+q2-p1>MAX_CELL) break;
            v.emplace_back((double)(p1*p2)/(q1*q2),pii{p1,q1},pii{p2,q2});
        }
        sort(all(v));
        #ifndef ONLINE_JUDGE
        cerr << "v.size() = " << v.size() << endl;
        #endif
        return v;
    }();
    return vf;
}

// q1の下限が指定される
pair<pair<pii,pii>,pair<pii,pii>> closest_rational2(double x, int lower){
    const auto &vf = valid_fractions_free_lower();
    auto it = lower_bound(all(vf),make_tuple(x,pii{0,0},pii{0,0}));
    pair<pii,pii> small,large;
    {
        auto tmp = it;
        while(tmp!=vf.end() && get<1>(*tmp).Y < lower){
            ++tmp;
        }
        if(tmp!=vf.end()) large = {get<1>(*tmp),get<2>(*tmp)};
        else large = {pii{inf,inf},pii{inf,inf}};
    }
    {
        auto tmp = it;
        if(it==vf.begin()) small = {pii{0,0},pii{0,0}};
        else{
            while(tmp!=vf.begin() && get<1>(*tmp).Y < lower){
                --tmp;
            }
            // if(tmp!=vf.begin()) small = {get<1>(*tmp),get<2>(*tmp)};
            if(get<1>(*tmp).Y >= lower) small = {get<1>(*tmp),get<2>(*tmp)};
            else{
                small = {pii{0,0},pii{0,0}};
            }
        }
    }
    
    return {small, large};
}

// q1の下限が指定される, 許容する最大誤差を指定する
// 許容誤差内に収まる中で,(q1+q2)-(p1+p2)が最小になるようなものを選ぶ
pair<pair<pii,pii>,pair<pii,pii>> closest_rational2(double x, int lower, double max_eps){
    const auto &vf = valid_fractions_free_lower();
    auto it = lower_bound(all(vf),make_tuple(x,pii{0,0},pii{0,0}));
    pair<pii,pii> small,large;
    {
        int mn=inf;
        pair<pii,pii> best;
        auto tmp = it;
        while(tmp!=vf.end() && get<0>(*tmp)<x+max_eps){
            auto [p1,q1] = get<1>(*tmp);
            auto [p2,q2] = get<2>(*tmp);
            if(q1>=lower && q1+q2-p1-p2<mn){
                mn = q1+q2-p1-p2;
                best = {pii{p1,q1},pii{p2,q2}};
            }
            tmp++;
        }
        if(mn<inf) large = best;
        else{
            while(tmp!=vf.end() && get<1>(*tmp).Y < lower) tmp++;
            if(tmp!=vf.end()) large = {get<1>(*tmp),get<2>(*tmp)};
            else large = {pii{inf,inf},pii{inf,inf}};
        }
    }
    {
        int mn=inf;
        pair<pii,pii> best;
        auto tmp = it;
        if(tmp==vf.begin()) small = {pii{0,0},pii{0,0}};
        else{
            while(tmp!=vf.begin() && get<0>(*tmp)>x-max_eps){
                --tmp; // 逆順に探索
                auto [p1,q1] = get<1>(*tmp);
                auto [p2,q2] = get<2>(*tmp);
                if(get<0>(*tmp)>x-max_eps && q1>=lower && q1+q2-p1-p2<mn){
                    mn = q1+q2-p1-p2;
                    best = {pii{p1,q1},pii{p2,q2}};
                }
            }
            if(mn<inf) small = best;
            else{
                while(tmp!=vf.begin() && get<1>(*tmp).Y < lower) --tmp;
                if(get<1>(*tmp).Y >= lower) small = {get<1>(*tmp),get<2>(*tmp)};
                else small = {pii{0,0},pii{0,0}};
            }
        }
    }
    
    return {small, large};
}

// 1段階で取りえる有理数を列挙する
const vector<pair<double,pii>>& valid_fractions_1step(){
    static vector<pair<double,pii>> vf = []{
        vector<pair<double,pii>> v;
        int MAX_CELL = N*N/K - 1;
        rep(p1,1,MAX_CELL+1)rep(q1,p1+1,MAX_CELL+1){
            v.emplace_back((double)p1/q1,pii(p1,q1));
        }
        sort(all(v));
        #ifndef ONLINE_JUDGE
        cerr << "v.size() = " << v.size() << endl;
        #endif
        return v;
    }();
    return vf;
}

pair<pii,pii> closest_rational1(double x, int lower, double max_eps=0){
    const auto &vf = valid_fractions_1step();
    auto it = lower_bound(all(vf),make_pair(x,pii(0,0)));
    pii small,large;
    {
        int mn=inf;
        pii best;
        auto tmp = it;
        while(tmp!=vf.end() && get<0>(*tmp)<x+max_eps){
            auto [p1,q1] = get<1>(*tmp);
            if(q1>=lower && q1-p1<mn){
                mn = q1-p1;
                best = {p1,q1};
            }
            tmp++;
        }
        if(mn<inf) large = best;
        else{
            while(tmp!=vf.end() && get<1>(*tmp).Y < lower) tmp++;
            if(tmp!=vf.end()) large = get<1>(*tmp);
            else large = {inf,inf};
        }
    }
    {
        int mn=inf;
        pii best;
        auto tmp = it;
        if(tmp==vf.begin()) small = {0,0};
        else{
            while(tmp!=vf.begin() && get<0>(*tmp)>x-max_eps){
                --tmp; // 逆順に探索
                auto [p1,q1] = get<1>(*tmp);
                if(get<0>(*tmp)>x-max_eps && q1>=lower && q1-p1<mn){
                    mn = q1-p1;
                    best = {p1,q1};
                }
            }
            if(mn<inf) small = best;
            else{
                while(tmp!=vf.begin() && get<1>(*tmp).Y < lower) --tmp;
                if(get<1>(*tmp).Y >= lower) small = get<1>(*tmp);
                else small = {0,0};
            }
        }
    }
    
    return {small, large};
}


namespace ratio_heuristic_solver {
vector<double> solve(const Color& t,
             const vector<Color>& v,
             const vector<double>& u,
             int max_it = 500)
{
    const int N = (int)v.size();
    assert(N == (int)u.size());

    /* ---------- 初期解: w_i = u_i / sum(u) （必ず可行） ---------- */
    double sum_u = accumulate(u.begin(), u.end(), 0.0);

    /* 最初の振り分けはランダムにする */
    vector<double> w(N);
    vector<int> idx(N); iota(all(idx),0);
    random_shuffle(all(idx));
    double remain = 1.0;
    for(int i=0;i<N;i++){
        int j = idx[i];
        w[j] = min(u[j],remain);
        remain -= w[j];
        if(remain < 1e-8) break;
    }

    /* ---------- g = Σ w_i v_i ---------- */
    Color g{0,0,0};
    for (int i = 0; i < N; ++i) g = g + v[i] * w[i];

    /* ---------- hill climbing 本体 ---------- */
    int pass = 0;
    for (pass = 0; pass < max_it; ++pass) {
        bool improved = false;
        for (int aa = 0; aa < N; ++aa) {
            for (int bb = aa + 1; bb < N; ++bb) {
                int i = aa, j = bb;
                Color delta = v[i] - v[j];
                if(dot(delta,t-g) < 0){
                    swap(i,j);
                    delta = v[i] - v[j];
                }
                double d_max = min(u[i] - w[i], w[j]);    // 可動上限
                
                if (d_max <= 0) continue;

                double d_star = dot(t-g,delta) / norm2(delta);
                double d = max(0.0, min(d_star, d_max));

                if (d > 1e-7) {
                    w[i] += d;  w[j] -= d;
                    g = g + delta * d;
                    improved = true;
                }
            }
        }
        if (!improved) break;       // 全ペア試しても改善なし→終了
    }
    return w;
}
}

/* 
    前計算を行う
*/

// 4点の体積順にソートする
const std::vector<std::array<int,4>> &pyramid_sorted(){
    static const int pre_k = -1;
    static const std::vector<std::array<int,4>> cv = []{
        std::vector<std::array<int,4>> v;
        v.reserve(K*(K-1)*(K-2)*(K-3)/24); // 4点の組み合わせ数
        // i0 < i1 < i2 < i3 の組み合わせを列挙
        rep(i0,K) rep(i1, i0+1, K) rep(i2, i1+1, K) rep(i3, i2+1, K) {
            v.push_back({i0, i1, i2, i3});
        }
        // 体積の大きい順（行列式の絶対値が大きい順）にソート
        sort(all(v), [](const std::array<int,4> &a,
                        const std::array<int,4> &b)
        {
            // インデックスを取り出す
            int i0 = a[0], i1 = a[1], i2 = a[2], i3 = a[3];
            int j0 = b[0], j1 = b[1], j2 = b[2], j3 = b[3];

            // 点を取り出す
            const Color &A0 = tube[i0], &A1 = tube[i1], &A2 = tube[i2], &A3 = tube[i3];
            const Color &B0 = tube[j0], &B1 = tube[j1], &B2 = tube[j2], &B3 = tube[j3];

            // ベクトル AB, AC, AD を計算
            Color vA1 = A1 - A0;
            Color vA2 = A2 - A0;
            Color vA3 = A3 - A0;

            Color vB1 = B1 - B0;
            Color vB2 = B2 - B0;
            Color vB3 = B3 - B0;

            // クロス積 vA2 × vA3
            Color crossA = {
                vA2[1] * vA3[2] - vA2[2] * vA3[1],
                vA2[2] * vA3[0] - vA2[0] * vA3[2],
                vA2[0] * vA3[1] - vA2[1] * vA3[0]
            };
            // クロス積 vB2 × vB3
            Color crossB = {
                vB2[1] * vB3[2] - vB2[2] * vB3[1],
                vB2[2] * vB3[0] - vB2[0] * vB3[2],
                vB2[0] * vB3[1] - vB2[1] * vB3[0]
            };

            // スカラー三重積（行列式） = vA1 · (vA2 × vA3)
            double detA = std::fabs(
                vA1[0] * crossA[0] +
                vA1[1] * crossA[1] +
                vA1[2] * crossA[2]
            );
            double detB = std::fabs(
                vB1[0] * crossB[0] +
                vB1[1] * crossB[1] +
                vB1[2] * crossB[2]
            );

            // 6で割った体積に相当するが、比較だけなら割らなくてよい
            return detA > detB;
        });
        return v;
    }();
    return cv;
}

namespace ver25_donut_random{
    struct Well{
        Color c; // 色
        double v; // 絵具の残り
        double cap; // 容量
        int fr,bc; // フロントとバックのインデックス
        Well(): c({0,0,0}),cap(0),v(0){}
        // 混ざった後の色を計算
        Color get_mixed_color(int tube_id){
            double add_paint = min(1.0, cap - v);
            double ratio = add_paint / (v + add_paint);
            Color mixed_color = mix(tube[tube_id], c, ratio);
            return mixed_color;
        }
        template<typename ...Args>
        Color get_mixed_color(Args... tube_ids){
            Color mixed_color = c;
            double tmp_v = v;
            int tube_id_arr[] = {tube_ids...};
            for(int tube_id : tube_id_arr){
                double add_paint = min(1.0, cap - tmp_v);
                double ratio = add_paint / (tmp_v + add_paint);
                mixed_color = mix(tube[tube_id], mixed_color, ratio);
                tmp_v = min(1.0 + tmp_v, cap);
            }
            return mixed_color;
        }
        // 実際に投入する
        void add(int tube_id){
            c = get_mixed_color(tube_id);
            v = min(v + 1.0, cap);
            return;
        }
        // 取り出す
        Color extract(){
            v = max(0.0, v - 1.0);
            return c;
        }
        // 捨てる
        void dump(){
            v -= max(0.0, v - 1.0);
            return;
        }
        // ウェルを分割
        pair<Well,Well> split(int k) const {
            Well w1,w2;
            w1.c = w2.c = c;
            w1.v = v * (double)k / cap;
            w2.v = v - w1.v;
            w1.cap = k;
            w2.cap = cap - k;
            w1.fr = fr;
            w1.bc = fr + k - 1;
            w2.fr = fr + k;
            w2.bc = bc;
            return {w1, w2};
        }
    };

    Well merge(const Well& w1, const Well& w2){
        Well w;
        w.c = mix(w1.c, w2.c, (double)w1.v / (w1.v + w2.v));
        w.v = w1.v + w2.v;
        w.cap = w1.cap + w2.cap;
        w.fr = w1.fr;
        w.bc = w2.bc;
        return w;
    }

    const vector<pii> &donut_route(){
        static const vector<pii> donut = []{
            vector<pii> donut;
            rep(x,N){
                if(x&1) rep(y,N/2) donut.emplace_back(x,y);
                else rrep(y,N/2) donut.emplace_back(x,y);
            }
            rrep(i,N*N/2){
                auto [x,y] = donut[i];
                donut.emplace_back(x,N-1-y);
            }
            return donut;
        }();
        return donut;
    }

    pair<vvi,vvi> construct_donut_wall(){
        vvi vertical_wall(N,vi(N-1,1));
        vvi horizontal_wall(N-1,vi(N,1));
        const auto& route = donut_route();
        int route_N = route.size();
        for(int i = 0; i < route_N-1; i++){
            const auto& p1 = route[i];
            const auto& p2 = route[(i+1)%route_N];
            if(p1.X == p2.X){
                // 垂直の壁
                vertical_wall[p1.X][min(p1.Y,p2.Y)] = 0;
            }
            else{
                // 水平の壁
                horizontal_wall[min(p1.X,p2.X)][p1.Y] = 0;
            }
        }
        return {vertical_wall, horizontal_wall};
    }

    struct State{
        persistent_stack<Operation> ops;
        vector<Well> well;
        int add_count = 0; // 絵具を投入した回数
        State():well(1){
            well[0].cap = N*N;
            well[0].fr = 0;
            well[0].bc = N*N-1;
        }
        void well_merge(int id){
            int nid = (id==well.size()-1) ? 0 : id+1;
            auto [i1,j1] = donut_route()[well[id].bc%(N*N)];
            auto [i2,j2] = donut_route()[well[nid].fr%(N*N)];
            ops = ops.push(Operation{4, {i1, j1, i2, j2}}); // 壁を切り替える操作を記録
            Well& left_w = well[id];
            Well& right_w = well[nid];
            well[id] = merge(left_w,right_w);
            well.erase(well.begin() + nid);
        }
        void well_split(int id, int k){
            int nid = id+1;
            auto [left_w, right_w] = well[id].split(k);
            well[id] = left_w;
            well.insert(well.begin()+nid, right_w);
            auto [i1,j1] = donut_route()[left_w.bc%(N*N)];
            auto [i2,j2] = donut_route()[right_w.fr%(N*N)];
            ops = ops.push(Operation{4, {i1, j1, i2, j2}}); // 壁を切り替える操作を記録
        }
        void add_color(int id, int tube_id){
            auto [x,y ] = donut_route()[well[id].fr%(N*N)];
            ops = ops.push(Operation{1, {x,y,tube_id}});
            well[id].add(tube_id);
            add_count++;
        }
        Color give_color(int id){
            auto [x,y] = donut_route()[well[id].fr%(N*N)];
            ops = ops.push(Operation{2, {x,y}});
            Color c = well[id].extract();
            return c;
        }
    };

    Ops main(bool fast_mode = false){
        Ops ops; // 操作の履歴
        K = io.true_tube.size();
        tube = io.true_tube;

        auto [v_wall,h_wall] = construct_donut_wall();
        State best_state;
        int best_score = 1e9;
        int seq_N = fast_mode ? 10 : max(2,min({100,40000/D,T-4000}));

        seq_N--;
        while(elapsed()<2900 || (fast_mode && best_score > 1e8)){
            seq_N++;
            // 初期状態の生成
            State state;
            rep(i,seq_N-1){
                state.well_split(i,N*N/seq_N);
            }
            state.ops = persistent_stack<Operation>();
            
            // 最初の色を入れる
            rep(i,0,seq_N){
                state.add_color(i,i%K);
            }

            double err_sum = 0.0;
            int t = 0;
            for(;t<H;t++){
                if(elapsed() > 2900 && !fast_mode) break;
                double best_diff = 1e10;
                State nxt_state;
                rep(add_id,state.well.size()){
                    if(state.well[add_id].v + 1.0 > state.well[add_id].cap) continue; // 絵具を追加できるウェルのみ対象
                    rep(color_id,K){
                        State _tmp = state;
                        _tmp.add_color(add_id,color_id);

                        // --- 探索ブロック 1: 絵の具を追加する一つ前の区画を分割 ---
                        {
                            int split_id = add_id > 0 ? add_id - 1 : _tmp.well.size() - 1;
                            const auto& split_well = _tmp.well[split_id];
                            const auto& add_well = _tmp.well[add_id];

                            int low = 1, high = split_well.cap - 1;

                            // 制約: split_v + add_v >= 1.0
                            if (split_well.v > 1e-9) {
                                double max_i_f = split_well.cap - (1.0 - add_well.v) * split_well.cap / split_well.v;
                                high = min(high, (int)floor(max_i_f));
                            } else if (add_well.v < 1.0 - 1e-6) {
                                high = 0; // 条件を満たすiは存在しない
                            }

                            int split_cap = -1;
                            if (low <= high) {
                                auto get_diff = [&](int i) {
                                    double split_v = split_well.v * (double)(split_well.cap - i) / split_well.cap;
                                    double add_v = add_well.v;
                                    if (split_v + add_v < 1e-9) return 1e18;
                                    return diff(mix(split_well.c, add_well.c, split_v / (split_v + add_v)), target[t]);
                                };
                                int l = low, r = high;
                                while (r - l >= 3) {
                                    int m1 = l + (r - l) / 3, m2 = r - (r - l) / 3;
                                    if (get_diff(m1) < get_diff(m2)) r = m2; else l = m1;
                                }
                                double current_best = 1e10;
                                for (int i = l; i <= r; ++i) if (chmin(current_best, get_diff(i))) split_cap = i;
                            }

                            if (split_cap != -1) {
                                State tmp = _tmp;
                                tmp.well_split(split_id, split_cap);
                                tmp.well_merge(split_id + 1);
                                Color c = tmp.give_color(add_id > 0 ? add_id : tmp.well.size() - 1);
                                if (chmin(best_diff, diff(c, target[t]))) nxt_state = tmp;
                            }
                        }

                        // --- 探索ブロック 2: 絵の具を追加する一つ後の区画を分割 ---
                        {
                            int split_id = (add_id + 1) % _tmp.well.size();
                            const auto& split_well = _tmp.well[split_id];
                            const auto& add_well = _tmp.well[add_id];

                            int low = 1, high = split_well.cap - 1;
                            
                            // 制約: split_v + add_v >= 1.0
                            if (split_well.v > 1e-9) {
                                double min_i_f = (1.0 - add_well.v) * split_well.cap / split_well.v;
                                low = max(low, (int)ceil(min_i_f));
                            } else if (add_well.v < 1.0 - 1e-6) {
                                low = high + 1;
                            }

                            int split_cap = -1;
                            if (low <= high) {
                                auto get_diff = [&](int i) {
                                    double split_v = split_well.v * (double)i / split_well.cap;
                                    double add_v = add_well.v;
                                    if (split_v + add_v < 1e-9) return 1e18;
                                    return diff(mix(split_well.c, add_well.c, split_v / (split_v + add_v)), target[t]);
                                };
                                int l = low, r = high;
                                while (r - l >= 3) {
                                    int m1 = l + (r - l) / 3, m2 = r - (r - l) / 3;
                                    if (get_diff(m1) < get_diff(m2)) r = m2; else l = m1;
                                }
                                double current_best = 1e10;
                                for (int i = l; i <= r; ++i) if (chmin(current_best, get_diff(i))) split_cap = i;
                            }
                            
                            if (split_cap != -1) {
                                State tmp = _tmp;
                                tmp.well_split(split_id, split_cap);
                                tmp.well_merge(add_id);
                                Color c = tmp.give_color(add_id);
                                if (chmin(best_diff, diff(c, target[t]))) nxt_state = tmp;
                            }
                        }

                        // --- 探索ブロック 3: 絵の具を追加する区画を分割して、後ろにマージ ---
                        {
                            const auto& split_target_well = _tmp.well[add_id];
                            int back_id = (add_id + 1) % _tmp.well.size();
                            const auto& back_well = _tmp.well[back_id];
                            
                            int low = 1, high = split_target_well.cap - 1;

                            // 制約: add_v + back_v >= 1.0
                            if (split_target_well.v > 1e-9) {
                                double max_i_f = split_target_well.cap - (1.0 - back_well.v) * split_target_well.cap / split_target_well.v;
                                high = min(high, (int)floor(max_i_f));
                            } else if (back_well.v < 1.0 - 1e-6) {
                                high = low - 1;
                            }

                            int split_cap = -1;
                            if (low <= high) {
                                auto get_diff = [&](int i) {
                                    double add_v = split_target_well.v * (double)(split_target_well.cap - i) / split_target_well.cap;
                                    double back_v = back_well.v;
                                    if (add_v + back_v < 1e-9) return 1e18;
                                    return diff(mix(split_target_well.c, back_well.c, add_v / (add_v + back_v)), target[t]);
                                };
                                int l = low, r = high;
                                while (r - l >= 3) {
                                    int m1 = l + (r - l) / 3, m2 = r - (r - l) / 3;
                                    if (get_diff(m1) < get_diff(m2)) r = m2; else l = m1;
                                }
                                double current_best = 1e10;
                                for (int i = l; i <= r; ++i) if (chmin(current_best, get_diff(i))) split_cap = i;
                            }

                            if (split_cap != -1) {
                                State tmp = _tmp;
                                tmp.well_split(add_id, split_cap);
                                tmp.well_merge(add_id + 1);
                                int submit_id = (add_id < tmp.well.size() - 1) ? add_id + 1 : add_id;
                                if (tmp.well[submit_id].v >= 1.0 - 1e-6) {
                                    Color c = tmp.give_color(submit_id);
                                    if (chmin(best_diff, diff(c, target[t]))) nxt_state = tmp;
                                }
                            }
                        }

                        // --- 探索ブロック 4: 絵の具を追加する区画を分割して、前にマージ ---
                        {
                            const auto& split_target_well = _tmp.well[add_id];
                            int front_id = (add_id - 1 + _tmp.well.size()) % _tmp.well.size();
                            const auto& front_well = _tmp.well[front_id];

                            int low = 1, high = split_target_well.cap - 1;
                            
                            // 制約: add_v + front_v >= 1.0
                            if (split_target_well.v > 1e-9) {
                                double min_i_f = (1.0 - front_well.v) * split_target_well.cap / split_target_well.v;
                                low = max(low, (int)ceil(min_i_f));
                            } else if (front_well.v < 1.0 - 1e-6) {
                                low = high + 1;
                            }
                            
                            int split_cap = -1;
                            if (low <= high) {
                                auto get_diff = [&](int i) {
                                    double add_v = split_target_well.v * (double)i / split_target_well.cap;
                                    double front_v = front_well.v;
                                    if (add_v + front_v < 1e-9) return 1e18;
                                    return diff(mix(split_target_well.c, front_well.c, add_v / (add_v + front_v)), target[t]);
                                };
                                int l = low, r = high;
                                while (r - l >= 3) {
                                    int m1 = l + (r - l) / 3, m2 = r - (r - l) / 3;
                                    if (get_diff(m1) < get_diff(m2)) r = m2; else l = m1;
                                }
                                double current_best = 1e10;
                                for (int i = l; i <= r; ++i) if (chmin(current_best, get_diff(i))) split_cap = i;
                            }
                            if (split_cap != -1) {
                                State tmp = _tmp;
                                tmp.well_split(add_id, split_cap);
                                int merge_id = (add_id > 0) ? add_id - 1 : tmp.well.size() - 1;
                                tmp.well_merge(merge_id);
                                // ↓↓↓ submit_idの計算を修正 ↓↓↓
                                int submit_id = (add_id > 0) ? (add_id - 1) : (tmp.well.size() - 1);
                                if (tmp.well[submit_id].v >= 1.0 - 1e-6) {
                                    Color c = tmp.give_color(submit_id);
                                    if (chmin(best_diff, diff(c, target[t]))) nxt_state = tmp;
                                }
                            }
                        }
                    }
                }

                if(best_diff > 1e9){
                    return ops;
                }
                state = nxt_state;
                err_sum += best_diff;
            }
            
            if(best_score < 1e8 && elapsed() > 2900 && t<H) break;

            int score = 1 + D * (state.add_count - H) + round(err_sum * 10000);
            if(chmin(best_score,score)){
                best_state = state;
            }
            if(fast_mode) break;
        }
        
        seq_N = best_state.well.size();
        auto [i1,j1] = donut_route()[0];
        auto [i2,j2] = donut_route()[N*N-1];
        if(i1==i2){
            v_wall[i1][min(j1,j2)] = 1;
        }
        else{
            h_wall[min(i1,i2)][j1] = 1;
        }
        rep(i,seq_N-1){
            auto [i1,j1] = donut_route()[(i+1)*(N*N/seq_N)-1];
            auto [i2,j2] = donut_route()[(i+1)*(N*N/seq_N)];
            if(i1==i2){
                v_wall[i1][min(j1,j2)] = 1; // 垂直の壁を復元
            }
            else{
                h_wall[min(i1,i2)][j1] = 1; // 水平の壁を復元
            }
        }

        // 永続スタックから操作を復元
        while(!best_state.ops.empty()){
            auto op = best_state.ops.top();
            ops.ops.emplace_back(op);
            best_state.ops = best_state.ops.pop();
        }
        reverse(all(ops.ops));

        ops.init_wall(v_wall,h_wall);

        #ifndef ONLINE_JUDGE
        cerr << "Final Score: " << best_score << "\n";
        cerr << "Total Add Count: " << best_state.add_count << "\n";
        #endif

        ops.K_ERASE = false; // チューブIDを削らない
        K = io.used_id.size();
        tube.resize(K);
        rep(i,K) tube[i] = io.true_tube[io.used_id[i]]; // チューブの種類を更新
        return ops;
    }
} // donut

namespace ver27{
/* 
  凸包を成す絵の具だけを使う
  header.cppをいじるという力技で実装(マージの時どうすんの？)
  max_epsを指定して、許容誤差内で高さを最小化しておく
  とりあえずadd_count==Hになった時点で、addしない方法を追加する
  1段階も織り交ぜてT=18000付近まで頑張って対応する
*/

struct Well{
    Color c; // 色
    double v; // 絵具の残り
    double cap; // 容量
    Well(): c({0,0,0}),cap(0),v(0){}
    // 混ざった後の色を計算
    Color get_mixed_color(int tube_id){
        double add_paint = min(1.0, cap - v);
        double ratio = add_paint / (v + add_paint);
        Color mixed_color = mix(tube[tube_id], c, ratio);
        return mixed_color;
    }
    template<typename ...Args>
    Color get_mixed_color(Args... tube_ids){
        Color mixed_color = c;
        double tmp_v = v;
        int tube_id_arr[] = {tube_ids...};
        for(int tube_id : tube_id_arr){
            double add_paint = min(1.0, cap - tmp_v);
            double ratio = add_paint / (tmp_v + add_paint);
            mixed_color = mix(tube[tube_id], mixed_color, ratio);
            tmp_v = min(1.0 + tmp_v, cap);
        }
        return mixed_color;
    }
    // 実際に投入する
    void add(int tube_id){
        c = get_mixed_color(tube_id);
        v = min(v + 1.0, cap);
        return;
    }
    // 取り出す
    Color extract(){
        v = max(0.0, v - 1.0);
        return c;
    }
    // 捨てる
    void dump(){
        v -= max(0.0, v - 1.0);
        return;
    }
    // ウェルを分割
    pair<Well,Well> split(int k) const {
        Well w1,w2;
        w1.c = w2.c = c;
        w1.v = v * (double)k / cap;
        w2.v = v - w1.v;
        w1.cap = k;
        w2.cap = cap - k;
        return {w1, w2};
    }
};

Well merge(const Well& w1, const Well& w2){
    Well w;
    if(w1.v+w2.v>0) w.c = mix(w1.c, w2.c, (double)w1.v / (w1.v + w2.v));
    else w.c = {0,0,0};
    w.v = w1.v + w2.v;
    w.cap = w1.cap + w2.cap;
    return w;
}

vector<vpii> cell;
void construct_cells(){
    auto& precalc_path = valid_path[K];
    cell.resize(K);
    rep(i,K){
        rep(j,precalc_path[i].size()){
            cell[i].emplace_back(precalc_path[i][j]);
        }
        reverse(all(cell[i]));
    }
}

pair<vvi,vvi> cells_to_wall(){
    vvi vertical_wall(N,vi(N-1,1));
    vvi horizontal_wall(N-1,vi(N,1));
    for(int i = 0; i < cell.size(); i++){
        if(cell[i].size()<2) continue;
        rep(j,cell[i].size()-1){
            if(cell[i][j].X == cell[i][j+1].X){
                // 垂直の壁
                vertical_wall[cell[i][j].X][min(cell[i][j].Y,cell[i][j+1].Y)] = 0;
            }
            else{
                // 水平の壁
                horizontal_wall[min(cell[i][j].X,cell[i][j+1].X)][cell[i][j].Y] = 0;
            }
        }
    }
    return {vertical_wall, horizontal_wall};
}


// 現在の状態を保持
// ・共有スペースの状態
//   ・絵の具の量,色,スペース
// ・各セクションの状態
//   ・絵の具の量,色,スペース
struct State{
    // 操作の履歴
    persistent_stack<Operation> ops;
    // 各セクションの状態
    vector<vector<Well>> well;
    // 共有スペースの状態
    Well shared;
    // 絵具を投入した回数
    int add_count = 0;
    // 誤差の合計
    double diff_sum = 0.0;
    // 累計操作回数
    int t = 0;

    State(){
        shared = Well();
        shared.cap = 400/K*K;
        shared.v = 0.0;
        shared.c = {0,0,0};
        well.resize(K);
    }
    
    // 共有スペースをkマス分割してウェルiに追加
    void shared_split(int i, int k){
        if(k==0) return; // kが0なら何もしない
        
        auto [add_to_well,new_shared] = shared.split(k);
        shared = new_shared;
        well[i].emplace_back(add_to_well);

        // opsの更新
        int now_cap_sum = get_well_cap_sum(i);
        auto [i1,j1] = cell[i][now_cap_sum-1];
        auto [i2,j2] = cell[i][now_cap_sum];
        ops = ops.push(Operation{4, {i1,j1,i2,j2}});
        t++;
        return;
    }
    
    // ウェルiの末尾のセクションを共有スペースにマージ
    void shared_merge(int i){
        if(well[i].empty()) return; // ウェルiが空なら何もしない
        int pre_cap_sum = get_well_cap_sum(i);
        
        Well &last_well = well[i].back();
        shared = merge(shared,last_well);
        well[i].pop_back();
        
        // opsの更新
        auto [i1,j1] = cell[i][pre_cap_sum-1];
        auto [i2,j2] = cell[i][pre_cap_sum];
        ops = ops.push(Operation{4,{i1, j1, i2, j2}});
        t++;
        return;
    }
    
    // ウェルiのjセクションを上側サイズがkとなるように分割
    void well_split(int i, int j, int k){
        if(k==0 || k==well[i][j].cap) return;
        auto [above_well,below_well] = well[i][j].split(k);
        well[i][j] = below_well;
        well[i].insert(well[i].begin()+j+1, above_well);
        
        // opsの更新
        int cap_sum = 0;
        rep(l,j+1) cap_sum += well[i][l].cap;
        auto [i1,j1] = cell[i][cap_sum-1];
        auto [i2,j2] = cell[i][cap_sum];
        ops = ops.push(Operation{4, {i1,j1,i2,j2}});
        t++;
    }
    
    // ウェルiのjセクションとj+1セクションをマージ
    void well_merge(int i, int j){ 
        int cap_sum = 0;
        rep(l,j+1) cap_sum += well[i][l].cap;
        
        well[i][j] = merge(well[i][j], well[i][j+1]);
        well[i].erase(well[i].begin()+j+1);
        
        // opsの更新
        auto [i1,j1] = cell[i][cap_sum-1];
        auto [i2,j2] = cell[i][cap_sum];
        ops = ops.push(Operation{4, {i1,j1,i2,j2}});
        t++;
    }
    
    // 絵具を追加
    void add_color(int i, int tube_id){
        // 最初のセクションに追加
        well[i][0].add(tube_id);
        
        auto [x,y] = cell[i].front();
        ops = ops.push(Operation{1, {x, y, tube_id}});
        t++;
        add_count++;
    }
    
    // 絵の具を共有スペースに追加
    void add_color_to_shared(int tube_id){
        shared.add(tube_id);
        ops = ops.push(Operation{1, {0,0,tube_id}});
        t++;
        add_count++;
    }

    // 絵の具を渡す
    Color give_color(Color tc){
        Color c = shared.extract();
        ops = ops.push(Operation{2, {0, 0}}); // 共有スペースから取り出す
        t++;
        diff_sum += diff(c,tc);
        return c;
    }
    
    int get_well_cap_sum(int i){
        int cap_sum = 0;
        for(const auto&w: well[i]) cap_sum += w.cap;
        return cap_sum;
    }
    int get_well_cap_sum(){
        int cap_sum = 0;
        rep(i,well.size()) cap_sum += get_well_cap_sum(i);
        return cap_sum;
    }
    int get_well_front_cap(int i){
        if(well[i].empty()) return 0;
        return well[i][0].cap;
    }
    double get_well_front_v(int i){
        if(well[i].empty()) return 0;
        return well[i][0].v;
    }
    int get_score(){
        int score = 1 + D * (add_count - H) + round(diff_sum * 10000);
        return score;
    }
};

// そのターンにどうすれば良いかの情報
struct Process{
    array<int,4> id; // 使うセクションのID (0,1,2,3)
    vector<bool> is_add; // trueなら絵具を追加
    vector<pair<pii,pii>> best_ratio; // 最適な比率 (分子, 分母)
    double best_diff; // 最適な比率の誤差
};

// 1段階操作と2段階操作を両方する
vector<State> calc_best_process1(State state, const Color &target, double TIME_LIMIT = 1e100){
    const int MAX_CELL = 400/K - 1;
    const double EPS = 5e-6;
    const int MAX_T_PER_PROCESS = 50;
    static vector<State> best_states(MAX_T_PER_PROCESS,State());
    static vector<State> best_tmps(MAX_T_PER_PROCESS,State());
    vector<double> best_diff(MAX_T_PER_PROCESS,1e100);

    // 実数比を求める
    int best_dw = -1;
    vector<double> best_w(4); // 最適行動の理論比率
    bool is_timeover = false;
    const auto& precalc_idx = pyramid_sorted();
    double arrow_negative = -1e-3;
    while(*min_element(all(best_diff)) > 1e10){
        for(int id_i=0;(*min_element(all(best_diff))>1e10 || !is_timeover) && id_i<precalc_idx.size();id_i++){
            const auto& id = precalc_idx[id_i];
            Color c[4]={
                state.get_well_front_v(id[0]) > 1e-3 ? state.well[id[0]][0].c : tube[id[0]],
                state.get_well_front_v(id[1]) > 1e-3 ? state.well[id[1]][0].c : tube[id[1]],
                state.get_well_front_v(id[2]) > 1e-3 ? state.well[id[2]][0].c : tube[id[2]],
                state.get_well_front_v(id[3]) > 1e-3 ? state.well[id[3]][0].c : tube[id[3]]
            };
            // 共有スペースに余っている絵具を考慮して補正する
            // 補正後の色 = (target - Cs*Vs)/(1-Vs)
            // 後で(1-Vs)倍して戻す
            Color Cs = state.shared.c;
            double Vs = state.shared.v;
            Color correct_target = (target - Cs*Vs) / (1.0 - Vs);
            auto _w = relaxed_4mix_problem(c, correct_target);
            rep(i,4) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
            if(*min_element(all(_w)) < arrow_negative){
                continue;
            }
            double w_sum = accumulate(all(_w),0.0);
            rep(i,4) _w[i] = max(0.0,_w[i]); // 負の値は0にする
            double now_w_sum = accumulate(all(_w),0.0);
            rep(i,4) _w[i] *= w_sum / now_w_sum; // 合計を戻す
            
            int DW = 1;
            bool found = false;
            for(int dw=0;(!found || dw<DW);dw++){
                vector<double> w(4); // 実数比
                rep(i,4) w[i] = _w[i] * (1.0 + (double)(dw-DW/2)* 1e-4);
                // 絵具が足りない場合は補充する
                vector<bool> is_add(4,false);
                vector<double> v(4);
                rep(i,4) v[i] = state.get_well_front_v(id[i]);
                rep(i,4)if(v[i]<w[i]-1e-5){
                    v[i] += 1.0;
                    is_add[i] = true; // このセクションに絵具を追加する
                }
                
                // 分母がspace以上であるような近似比を求める
                vector<double> ideal(4); // 理想の比率
                rep(i,4){
                    if(v[i]==0) ideal[i] = 0;
                    else ideal[i] = w[i]/v[i];
                }
                
                vector<pii> s1(4),l1(4); // 1段階の大きい小さい
                rep(i,4){
                    auto [s,l] = closest_rational1(
                        ideal[i],
                        max(2,state.get_well_front_cap(id[i])),
                        0
                    );
                    s1[i] = s;
                    l1[i] = l;
                }
                vector<pair<pii,pii>> s2(4),l2(4); // 2段階の大きい小さい
                rep(i,4){
                    auto [s,l] = closest_rational2(
                        ideal[i],
                        max(2,state.get_well_front_cap(id[i])),
                        EPS
                    );
                    s2[i] = s;
                    l2[i] = l;
                }
                
                // 最適な操作を決定
                // 各iについて s1[i],l1[i],s2[i],l2[i]のどれを使うか全探索
                vector<double> tmp_err(MAX_T_PER_PROCESS,1e100);
                for(int OT:{0,3,12,15}){
                    rep(SL,1<<4){
                        int p1[4],q1[4],p2[4],q2[4]; // 1段階の時はp1,q1のみ
                        rep(i,4){
                            if(bit(OT,i)){
                                if(bit(SL,i)){
                                    tie(p1[i],q1[i]) = s1[i];
                                }
                                else{
                                    tie(p1[i],q1[i]) = l1[i];
                                }
                                /* double s_val = (double)s1[i].X / s1[i].Y;
                                double l_val = (double)l1[i].X / l1[i].Y;
                                if(abs(s_val-ideal[i]) < abs(l_val-ideal[i])) tie(p1[i],q1[i]) = s1[i];
                                else tie(p1[i],q1[i]) = l1[i]; */
                            }
                            else{
                                if(bit(SL,i)){
                                    tie(p1[i],q1[i]) = s2[i].X;
                                    tie(p2[i],q2[i]) = s2[i].Y;
                                }
                                else{
                                    tie(p1[i],q1[i]) = l2[i].X;
                                    tie(p2[i],q2[i]) = l2[i].Y;
                                }
                                /* double s_val = (double)s2[i].X.X*s2[i].Y.X / (s2[i].X.Y*s2[i].Y.Y);
                                double l_val = (double)l2[i].X.X*l2[i].Y.X / (l2[i].X.Y*l2[i].Y.Y);
                                if(abs(s_val-ideal[i]) < abs(l_val-ideal[i])) tie(p1[i],q1[i]) = s2[i].X, tie(p2[i],q2[i]) = s2[i].Y;
                                else tie(p1[i],q1[i]) = l2[i].X, tie(p2[i],q2[i]) = l2[i].Y; */
                            }
                        }
                        State tmp = state;

                        // 1~2まで行う
                        rep(i,4){
                            // 1.2. q1に広げる
                            if(p1[i]!=0 && p1[i]!=inf){
                                tmp.shared_split(id[i],q1[i]-tmp.get_well_cap_sum(id[i]));
                            }
                            // 絵の具を追加するスペースを確保
                            if(p1[i]==inf && is_add[i] && tmp.get_well_front_cap(id[i]) + 1.0 > tmp.get_well_cap_sum(id[i])){
                                tmp.shared_split(id[i],2);
                            }
                            if(tmp.well[id[i]].size()>1) tmp.well_merge(id[i],0);
                        }
                        // 絵の具の追加
                        rep(i,4){
                            if(is_add[i] && p1[i]!=0){
                                tmp.add_color(id[i],id[i]);
                            }
                        }
                        rep(i,4){
                            if(bit(OT,i)){ // 1段階
                                if(p1[i]!=0 && p1[i]!=inf){
                                    // 3. p1分子を分離
                                    tmp.well_split(id[i],0,p1[i]);
                                }
                            }
                            else{ // 2段階
                                // 3~7.まで行う(共有スペースにマージする直前まで)
                                if(p1[i]!=0 && p1[i]!=inf){
                                    // 3. p1分子を分離
                                    tmp.well_split(id[i],0,p1[i]);
                                    // 4. q1+q2-p1のスペースを共有スペースから確保する
                                    tmp.shared_split(id[i],q1[i]+q2[i]-p1[i]-tmp.get_well_cap_sum(id[i]));
                                    // 5. 3つのうち上2つをマージ
                                    tmp.well_merge(id[i],1);
                                    // 6. well[i][1]をスプリット
                                    tmp.well_split(id[i],1,p2[i]);
                                    // 7. 下2つをマージ
                                    tmp.well_merge(id[i],0);
                                }
                            }
                        }
                        
                        // 8. 共有スペースにマージ
                        rep(i,4)if(p1[i]!=0) tmp.shared_merge(id[i]);

                        // 混ぜた後の色を計算
                        if(tmp.shared.v > 1.0 - 1e-6){ // 絵具が十分にある前提
                            Color give_color = tmp.give_color(target);
                            // 誤差を計算
                            double err = diff(give_color,target);
                            int t_diff = tmp.t - state.t;
                            if(chmin(tmp_err[t_diff], err+max(tmp.add_count-H,0)*D*1e-4)){
                                best_tmps[t_diff] = tmp; // 最適な状態を記録
                            }
                        }

                        // 更新されなかった場合はlargeに切り替え
                        int mn_id = -1;
                        double mn_diff_worse = 1e100;
                        rep(i,4){
                            if(bit(OT,i)){
                                if(pii(p1[i],q1[i])!=l1[i]){
                                    // 誤差の悪化量
                                    if(chmin(mn_diff_worse, abs(ideal[i]-(double)l1[i].X/l1[i].Y) - abs(ideal[i]-(double)p1[i]/q1[i]))){
                                        mn_id = i;
                                    }
                                }
                            }
                            else{
                                if(pair(pii(p1[i],q1[i]),pii(p2[i],q2[i]))!=l2[i]){
                                    // 誤差の悪化量
                                    if(chmin(mn_diff_worse, abs(ideal[i]-(double)l2[i].X.X*l2[i].Y.X/(l2[i].X.Y*l2[i].Y.Y)) - abs(ideal[i]-(double)p1[i]*p2[i]/(q1[i]*q2[i])))){
                                        mn_id = i;
                                    }
                                }
                            }
                        }
                        if(mn_id==-1) break;
                        if(bit(OT,mn_id)) tie(p1[mn_id],q1[mn_id]) = l1[mn_id];
                        else tie(p1[mn_id],q1[mn_id]) = l2[mn_id].X, tie(p2[mn_id],q2[mn_id]) = l2[mn_id].Y;
                    }
                }

                if(*min_element(all(tmp_err)) < 1e10){
                    found = true;
                }

                // 最適比を復元
                rep(t_diff,MAX_T_PER_PROCESS){
                    if(tmp_err[t_diff] > 1e10) continue; // 見つかっていない状態はスキップ
                    if(chmin(best_diff[t_diff], tmp_err[t_diff])){
                        best_states[t_diff] = best_tmps[t_diff]; // 最適な状態を更新
                    }
                }
            }

            // 時間制限を判定
            if((id_i&31)==0){
                if(elapsed() > TIME_LIMIT){
                    is_timeover = true;
                }
            }
        }
        arrow_negative *= 10;
    }
    vector<State> ret_state;
    rep(i,MAX_T_PER_PROCESS){
        if(best_diff[i] > 1e10) continue; // 見つかっていない状態はスキップ
        ret_state.push_back(best_states[i]);
    }
    return ret_state;
}

// 2段階操作
State calc_best_process2(State state, const Color &target, double TIME_LIMIT = 1e100){
    const int MAX_CELL = 400/K - 1;
    const double EPS = 5e-6;
    State best_state = state;
    Process process;
    process.best_ratio.resize(4,{{-1,-1},{-1,-1}});

    // 実数比を求める
    double best_diff = 1e100; // 最適な誤差
    int best_dw = -1;
    vector<double> best_w(4); // 最適行動の理論比率
    bool is_timeover = false;
    const auto& precalc_idx = pyramid_sorted();
    for(int id_i=0;!is_timeover && id_i<precalc_idx.size();id_i++){
        const auto& id = precalc_idx[id_i];
        Color c[4]={
            !state.well[id[0]].empty() ? state.well[id[0]][0].c : tube[id[0]],
            !state.well[id[1]].empty() ? state.well[id[1]][0].c : tube[id[1]],
            !state.well[id[2]].empty() ? state.well[id[2]][0].c : tube[id[2]],
            !state.well[id[3]].empty() ? state.well[id[3]][0].c : tube[id[3]]
        };
        // 共有スペースに余っている絵具を考慮して補正する
        // 補正後の色 = (target - Cs*Vs)/(1-Vs)
        // 後で(1-Vs)倍して戻す
        Color Cs = state.shared.c;
        double Vs = state.shared.v/*  * (double)40/(400/K*K-state.get_well_cap_sum()) */;
        Color correct_target = (target - Cs*Vs) / (1.0 - Vs);
        auto _w = relaxed_4mix_problem(c, correct_target);
        rep(i,4) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
        if(*min_element(all(_w)) < -1e-2){
            continue;
        }
        double w_sum = accumulate(all(_w),0.0);
        rep(i,4) _w[i] = max(0.0,_w[i]); // 負の値は0にする
        double now_w_sum = accumulate(all(_w),0.0);
        rep(i,4) _w[i] *= w_sum / now_w_sum; // 合計を戻す
        
        // 改めて計算を回してsharedの影響を詳しく見る
        {
            // 絵具が足りない場合は補充する
            vector<double> v(4);
            rep(i,4) v[i] = state.get_well_front_v(id[i]);
            rep(i,4)if(v[i]<_w[i]-1e-5){
                v[i] += 1.0;
            }
            
            // 分母がspace以上であるような近似比を求める
            vector<double> ideal(4); // 理想の比率
            rep(i,4) ideal[i] = _w[i]/v[i];
            int shared_space = 400/K*K - state.get_well_cap_sum();
            // 共有スペースから漏れる絵の具の量を計算
            rep(i,4){
                auto [s,l] = closest_rational2(
                    ideal[i],
                    max(2,state.get_well_front_cap(id[i])),
                    EPS
                );
                double s_val,l_val;
                if(s.X.X==0) s_val = 0;
                else if(s.X.X==inf) s_val = 1;
                else s_val = (double)s.X.X*s.Y.X / (s.X.Y*s.Y.Y);
                if(l.X.X==0) l_val = 0;
                if(l.X.X==inf) l_val = 1;
                else l_val = (double)l.X.X*l.Y.X / (l.X.Y*l.Y.Y);
                int p1,p2,q1,q2;
                if(abs(s_val-ideal[i]) < abs(l_val-ideal[i])) tie(p1,q1) = s.X, tie(p2,q2) = s.Y;
                else tie(p1,q1) = l.X, tie(p2,q2) = l.Y;
                if(p1==0 || p1==inf) continue; // 開放なしか全開放ならshared_vには関係ない
                // とりあえず合計でこれだけ取る
                double sum_get = state.shared.v * (double)(q1+q2-p1-state.get_well_cap_sum(id[i]))/shared_space;
                // 何個返すか
                double sum_return = 0.0;
                sum_return += state.shared.v * (double)(q1-state.get_well_cap_sum(id[i]))/shared_space;
                sum_return *= (double)p1/q1;
                sum_return += state.shared.v * (double)(q2-p1)/shared_space;
                sum_return *= (double)p2/q2;
                // 返す量を引く
                sum_get -= sum_return;
                // sum_getだけ共有スペースから取られる
                Vs -= sum_get;
            }
            correct_target = (target - Cs*Vs) / (1.0 - Vs);
            _w = relaxed_4mix_problem(c, correct_target);
            rep(i,4) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
            w_sum = accumulate(all(_w),0.0);
            rep(i,4) _w[i] = max(0.0,_w[i]); // 負の値は0にする
            now_w_sum = accumulate(all(_w),0.0);
            rep(i,4) _w[i] *= w_sum / now_w_sum; // 合計を戻す
        }
        
        int DW = 1;
        bool found = false;
        for(int dw=0;(!found || dw<DW);dw++){
            vector<double> w(4); // 実数比
            rep(i,4) w[i] = _w[i] * (1.0 + (double)dw * 1e-5);
            // 絵具が足りない場合は補充する
            vector<bool> is_add(4,false);
            vector<double> v(4);
            rep(i,4) v[i] = state.get_well_front_v(id[i]);
            rep(i,4)if(v[i]<w[i]*1.1){
                v[i] += 1.0;
                is_add[i] = true; // このセクションに絵具を追加する
            }
            
            // 分母がspace以上であるような近似比を求める
            vector<double> ideal(4); // 理想の比率
            rep(i,4){
                if(v[i]==0) ideal[i] = 0;
                else ideal[i] = w[i]/v[i];
            }
            vector<pair<pii,pii>> small(4),large(4);
            rep(i,4){
                auto [s,l] = closest_rational2(
                    ideal[i],
                    max(2,state.get_well_front_cap(id[i])),
                    EPS
                ); // 比の有理数近似を列挙
                small[i] = s;
                large[i] = l;
            }
            // 最適な操作を決定
            // 各iについて small or large のどちらを使うか全探索する
            double tmp_err = 1e100;
            State best_tmp;
            rep(S,1<<4){
                State tmp = state;
                int p1[4],q1[4],p2[4],q2[4];
                rep(i,4){
                    if(bit(S,i)){
                        tie(p1[i],q1[i]) = small[i].X;
                        tie(p2[i],q2[i]) = small[i].Y;
                    }
                    else{
                        tie(p1[i],q1[i]) = large[i].X;
                        tie(p2[i],q2[i]) = large[i].Y;
                    }
                }
                // 1~2まで行う
                rep(i,4){
                    // 1.2. q1に広げる
                    if(p1[i]!=0 && p1[i]!=inf){
                        tmp.shared_split(id[i],q1[i]-tmp.get_well_cap_sum(id[i]));
                    }
                    if(tmp.well[id[i]].size()>1) tmp.well_merge(id[i],0);
                }
                // 3~7.まで行う
                rep(i,4){
                    // 必要なら絵具を追加
                    if(is_add[i] && p1[i]!=0){
                        tmp.add_color(id[i],id[i]);
                    }
                    if(p1[i]!=0 && p1[i]!=inf){
                        // 3. p1分子を分離
                        tmp.well_split(id[i],0,p1[i]);
                        // 4. q1+q2-p1のスペースを共有スペースから確保する
                        tmp.shared_split(id[i],q1[i]+q2[i]-p1[i]-tmp.get_well_cap_sum(id[i]));
                        // 5. 3つのうち上2つをマージ
                        tmp.well_merge(id[i],1);
                        // 6. well[i][1]をスプリット
                        tmp.well_split(id[i],1,p2[i]);
                        // 7. 下2つをマージ
                        tmp.well_merge(id[i],0);
                    }
                }
                
                // 8. 共有スペースにマージ
                rep(i,4)if(p1[i]!=0) tmp.shared_merge(id[i]);

                // 混ぜた後の色を計算
                if(tmp.shared.v > 1.0 - 1e-6){ // 絵具が十分にある前提
                    Color give_color = tmp.give_color(target);
                    // 誤差を計算
                    double err = diff(give_color,target);
                    if(chmin(tmp_err, err+max(tmp.add_count-H,0)*D*1e-4)){
                        best_tmp = tmp; // 最適な状態を記録
                    }
                }
            }

            if(tmp_err < 1e10){
                found = true;
            }

            // 最適比を復元
            if(chmin(best_diff,tmp_err)){
                best_state = best_tmp; // 最適な状態を更新
                best_w = ideal;
                best_dw = dw;
            }
        }

        // 時間制限を判定
        if((id_i&31)==0){
            if(elapsed() > TIME_LIMIT){
                is_timeover = true;
            }
        }
    }
    return best_state;
}

struct Final_Process{
    vector<double> w;
    vector<pair<pii,pii>> best_ratio; // 最適な比率 (分子, 分母)
    double best_diff;
};

State calc_final_process(State state, const vector<Color> &target, double TIME_LIMIT = 1e100){
    static unordered_map<double,vector<int>> best_add_num_cache;
    const int WELL_N = state.well.size();

    Color state_c_sum = {0,0,0};
    rep(i,WELL_N) if(!state.well[i].empty()) state_c_sum = state_c_sum + state.well[i][0].c * state.well[i][0].v;
    state_c_sum = state_c_sum + state.shared.c * state.shared.v;
    Color target_c_sum = {0,0,0};
    rep(i,target.size()) target_c_sum = target_c_sum + target[i];
    vi best_add_num(WELL_N,0);
    // 最適な追加を求める
    if(state.add_count < H){
        if(!best_add_num_cache.contains(target_c_sum[0])){
            int need_add = H - state.add_count;
            vi add_num(WELL_N,0);
            Color now_c = state_c_sum;
            double mn_diff = 1e100;
            auto dfs = [&](auto dfs, int i, int now_add) -> void {
                if(elapsed() > TIME_LIMIT) return;
                if(i==WELL_N-1){
                    add_num[i] = need_add - now_add;
                    if(chmin(mn_diff,diff(now_c,target_c_sum))){
                        best_add_num = add_num;
                    }
                    return;
                }
                else{
                    rep(j,need_add-now_add+1){
                        add_num[i] = j;
                        now_c = now_c + tube[i] * j;
                        dfs(dfs,i+1,now_add+j);
                        now_c = now_c - tube[i] * j; // 戻す
                    }
                }
                return;
            }; dfs(dfs,0,0);
            best_add_num_cache[target_c_sum[0]] = best_add_num;
        }
        else best_add_num = best_add_num_cache[target_c_sum[0]];
    }
    
    if(elapsed() > TIME_LIMIT){
        state.diff_sum = 10000;
        return state;
    }

    // 現在場にある絵の具の量
    double now_sum_v = state.shared.v;
    for(int i=0;i<WELL_N;i++){
        for(const auto& w: state.well[i]) now_sum_v += w.v;
    }
    double sum_err = 0;
    rep(t,target.size()-1){
        rep(i,WELL_N){
            if(!state.well[i].empty() && state.well[i][0].v + 1.0 <= state.well[i][0].cap && state.well[i][0].v <= 1.0){
                if(best_add_num[i]>0){
                    state.add_color(i,i);
                    best_add_num[i]--;
                }
            }
            if(state.well[i].empty() && best_add_num[i]>0){
                state.shared_split(i,2);
                state.add_color(i,i);
                best_add_num[i]--;
            }
        }
        vector<Color> c(WELL_N);
        rep(i,WELL_N) c[i] = !state.well[i].empty() ? state.well[i][0].c : Color{0,0,0};
        vector<double> u(WELL_N);
        rep(i,WELL_N) u[i] = !state.well[i].empty() ? state.well[i][0].v : 0.0;
        const Color &tc = target[t];
        // 計算パート
        // 共有スペースに余っている絵具を考慮して補正する
        // 補正後の色 = (target - Cs*Vs)/(1-Vs)
        // 後で(1-Vs)倍して戻す
        Color Cs = state.shared.c;
        double Vs = state.shared.v;
        Color correct_target = (tc - Cs*Vs) / (1.0 - Vs);
        rep(i,WELL_N) u[i] /= 1.0 - Vs; // 共有スペースの補正をかける
        auto _w = ratio_heuristic_solver::solve(correct_target, c, u);
        rep(i,WELL_N) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
        rep(i,WELL_N) u[i] *= 1 - Vs; // 共有スペースの補正を戻す
        
        // 改めて計算を回してsharedの影響を詳しく見る
        {
            // 絵具が足りない場合は補充する
            vector<double> v(WELL_N);
            rep(i,WELL_N) v[i] = state.get_well_front_v(i);
            
            // 分母がspace以上であるような近似比を求める
            vector<double> ideal(WELL_N); // 理想の比率
            rep(i,WELL_N){
                ideal[i] = v[i] > 0 ? _w[i]/v[i] : 0.0;
            }
            int shared_space = 400/K*K - state.get_well_cap_sum();
            // 共有スペースから漏れる絵の具の量を計算
            rep(i,WELL_N){
                if(v[i]==0) continue;
                auto [s,l] = closest_rational2(
                    ideal[i],
                    max(2,state.get_well_front_cap(i)),
                    5e-6
                );
                double s_val,l_val;
                if(s.X.X==0) s_val = 0;
                else if(s.X.X==inf) s_val = 1;
                else s_val = (double)s.X.X*s.Y.X / (s.X.Y*s.Y.Y);
                if(l.X.X==0) l_val = 0;
                if(l.X.X==inf) l_val = 1;
                else l_val = (double)l.X.X*l.Y.X / (l.X.Y*l.Y.Y);
                int p1,p2,q1,q2;
                if(abs(s_val-ideal[i]) < abs(l_val-ideal[i])) tie(p1,q1) = s.X, tie(p2,q2) = s.Y;
                else tie(p1,q1) = l.X, tie(p2,q2) = l.Y;
                if(p1==0 || p1==inf) continue; // 開放なしか全開放ならshared_vには関係ない
                // とりあえず合計でこれだけ取る
                double sum_get = state.shared.v * (double)(q1+q2-p1-state.get_well_cap_sum(i))/shared_space;
                // 何個返すか
                double sum_return = 0.0;
                sum_return += state.shared.v * (double)(q1-state.get_well_cap_sum(i))/shared_space;
                sum_return *= (double)p1/q1;
                sum_return += state.shared.v * (double)(q2-p1)/shared_space;
                sum_return *= (double)p2/q2;
                // 返す量を引く
                sum_get -= sum_return;
                // sum_getだけ共有スペースから取られる
                Vs -= sum_get;
            }
            correct_target = (tc - Cs*Vs) / (1.0 - Vs);
            rep(i,WELL_N) u[i] /= 1.0 - Vs; // 共有スペースの補正をかける
            _w = ratio_heuristic_solver::solve(correct_target, c, u);
            rep(i,WELL_N) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
            rep(i,WELL_N) u[i] *= 1 - Vs; // 共有スペースの補正を戻す
        }

        // 操作パート
        int DW = 1;
        double best_diff = 1e100;
        bool found = false;
        State nxt_state;
        for(int dw=0;(!found || dw<DW);dw++){
            vector<double> w(WELL_N); // 実数比
            rep(i,WELL_N) w[i] = _w[i] * (1.0 + (double)dw * 1e-5);
            vector<double> v(WELL_N);
            rep(i,WELL_N) v[i] = state.get_well_front_v(i);
            
            // 分母がspace以上であるような近似比を求める
            vector<double> ideal(WELL_N); // 理想の比率
            rep(i,WELL_N){
                ideal[i] = v[i] > 0 ? w[i]/v[i] : 0.0;
            }
            
            vector<pair<pii,pii>> small,large;
            vi id;
            rep(i,WELL_N){
                if(v[i]==0) continue;
                auto [s,l] = closest_rational2(
                    ideal[i],
                    max(2,state.get_well_front_cap(i)),
                    5e-6
                ); // 比の有理数近似を列挙
                small.push_back(s), large.push_back(l);
                id.push_back(i);
            }

            // 最適な操作を決定
            // 各iについて small or large のどちらを使うか全探索する
            double tmp_err = 1e100;
            State best_tmp;
            // とりあえず近い値にセット
            vi p1(id.size()),q1(id.size()),p2(id.size()),q2(id.size());
            rep(i,id.size()){
                double s_val = (double)small[i].X.X * small[i].Y.X / (double)small[i].X.Y / small[i].Y.Y;
                double l_val = (double)large[i].X.X * large[i].Y.X / (double)large[i].X.Y / large[i].Y.Y;
                if(abs(s_val-ideal[id[i]]) < abs(l_val-ideal[id[i]])){
                    tie(p1[i],q1[i]) = small[i].X;
                    tie(p2[i],q2[i]) = small[i].Y;
                }
                else{
                    tie(p1[i],q1[i]) = large[i].X;
                    tie(p2[i],q2[i]) = large[i].Y;
                }
            }
            while(tmp_err > 1e10){
                State tmp = state;
                
                // 1~7まで行う
                rep(i,id.size()){
                    // 1.2. q1に広げる
                    if(p1[i]!=0 && p1[i]!=inf){
                        tmp.shared_split(id[i],q1[i]-tmp.get_well_cap_sum(id[i]));
                    }
                    if(tmp.well[id[i]].size()>1) tmp.well_merge(id[i],0);
                    if(p1[i]!=0 && p1[i]!=inf){
                        // 3. p1分子を分離
                        tmp.well_split(id[i],0,p1[i]);
                        // 4. q1+q2-p1のスペースを共有スペースから確保する
                        tmp.shared_split(id[i],q1[i]+q2[i]-p1[i]-tmp.get_well_cap_sum(id[i]));
                        // 5. 3つのうち上2つをマージ
                        tmp.well_merge(id[i],1);
                        // 6. well[i][1]をスプリット
                        tmp.well_split(id[i],1,p2[i]);
                        // 7. 下2つをマージ
                        tmp.well_merge(id[i],0);
                    }
                }
                
                // 8. 共有スペースにマージ
                rep(i,id.size())if(p1[i]!=0) tmp.shared_merge(id[i]);

                // 混ぜた後の色を計算
                if(tmp.shared.v > 1.0 - 1e-6){ // 絵具が十分にある前提
                    Color give_color = tmp.give_color(tc);
                    // 誤差を計算
                    double err = diff(give_color,tc);
                    if(chmin(tmp_err, err)){
                        best_tmp = tmp; // 最適な状態を記録
                    }
                }

                // 見つけたら抜ける
                if(tmp_err < 1e10) break;

                // 近似比を更新 
                // 大きくした後の誤差が小さいものを選ぶ
                double mn_diff = 1e100;
                int best_i = -1;
                rep(i,id.size()){
                    if(pair(pii(p1[i],q1[i]),pii(p2[i],q2[i])) == large[i]) continue;
                    if(chmin(mn_diff,abs(ideal[id[i]] - (double)large[i].X.X * large[i].Y.X / (double)large[i].X.Y / large[i].Y.Y))){
                        best_i = i;
                    }
                }
                if(best_i == -1) break; // これ以上大きくできないなら抜ける

                tie(p1[best_i],q1[best_i]) = large[best_i].X;
                tie(p2[best_i],q2[best_i]) = large[best_i].Y;
            }

            if(tmp_err < 1e10){
                found = true;
            }

            // 最適比を復元
            if(chmin(best_diff,tmp_err)){
                nxt_state = best_tmp;
            }
        }
        sum_err += best_diff;
        state = nxt_state;
    }

    // 最後のターンはある分を開放
    rep(i,WELL_N){
        while(!state.well[i].empty()){
            state.shared_merge(i);
        }
    }
    rep(i,WELL_N){
        while(best_add_num[i]>0){
            state.add_color_to_shared(i);
            best_add_num[i]--;
        }
    }
    Color c = state.give_color(target.back());
    double final_diff = diff(c,target.back());
    sum_err += final_diff;

    return state;
}

Ops main(){
    Ops ops; // 操作の履歴
    construct_cells();

    // 最初の操作を計算
    State state;
    state = calc_best_process1(state,target[0])[0];
    #ifndef ONLINE_JUDGE
    cerr << "Initial calculation done." << endl;
    cerr << "Elapsed Time: " << elapsed() << " ms" << endl;
    #endif

    { // 壁の設定
    auto [v_wall,h_wall] = cells_to_wall();
    // 混ぜる場所を繋げる
    rep(i,K-1) v_wall[0][i] = 0;
    // 初期の各ウェルの仕切り
    // 1手目の壁の状態を見る
    rep(i,state.well.size()){
        persistent_stack tmp = state.ops;
        vector<Operation> ops;
        while(!tmp.empty()){
            ops.push_back(tmp.top());
            tmp = tmp.pop();
        }
        reverse(all(ops));
        for(const auto &op:ops){
            if(op.type!=4) break;
            int x1 = op.args[0], y1 = op.args[1], x2 = op.args[2], y2 = op.args[3];
            if(x1==x2){
                v_wall[x1][min(y1,y2)] = 1;
            }
            else{
                h_wall[min(x1,x2)][y1] = 1;
            }
        }
    }
    ops.init_wall(v_wall,h_wall);
    }

    double err_sum = 0.0; // 誤差の合計
    
    double initial_diff = state.diff_sum - err_sum;

    vector<tuple<int,State,double>> pool_state;
    { // 絵の具をH回追加するまで1段階で処理する
    double init_time = elapsed();
    rep(t,1,H){
        // 操作を計算
        vector<State> nxt_states = calc_best_process1(state,target[t],t*2.5 + init_time);
        State nxt_state = nxt_states[0];
        double mx_score = 1e100;
        for(auto &e:nxt_states){
            if(e.t < (T-500)*(t+1)/H && chmin(mx_score,e.diff_sum)){
                nxt_state = e;
            }
        }

        if(state.add_count <= H && nxt_state.add_count > H-10){
            pool_state.emplace_back(t,state,1e100);
        }

        state = nxt_state; // 状態を更新
    
        double actual_diff = nxt_state.diff_sum - err_sum;

        // 誤差を加算
        err_sum += actual_diff;
    }
    }
    #ifndef ONLINE_JUDGE
    cerr << "Add Count: " << state.add_count << endl;
    cerr << "Elapsed Time: " << elapsed() << " ms" << endl;
    #endif

    // 途中からを再計算
    reverse(all(pool_state));
    int pool_size = pool_state.size();
    double pool_search_init_time = elapsed();
    while(!pool_state.empty() && elapsed() < 2900){
        rep(i,pool_state.size()){
            auto &[t,_state,score] = pool_state[i];
            vector<Color> remain_target = vector<Color>(target+t,target+H);
            State _nxt_state = calc_final_process(_state, remain_target,2900);
            if(_nxt_state.t < T && _nxt_state.get_score() < state.get_score()){
                state = _nxt_state;
            }
            score = _nxt_state.get_score();
            #ifndef ONLINE_JUDGE
            cerr << "t: " << t << ", score : " << _nxt_state.get_score() << ", Elapsed Time : " << elapsed() << "ms, T : " << _nxt_state.t << "\n";
            #endif
            if(elapsed() > 2900) break;
        }
        if(elapsed() > 2900) break;
        // スコアの高い順にソート
        sort(all(pool_state),[](const auto &a, const auto &b){
            return get<2>(a) < get<2>(b);
        });
        // スコアの低いものを排除
        // 時間とともに少しずつ減らす
        double progress = (double)(elapsed() - pool_search_init_time) / (double) (2900 - pool_search_init_time);
        pool_state.resize(max(1,(int)(pool_size*(1-progress))));
    }

    int score = state.get_score();
    #ifndef ONLINE_JUDGE
    cerr << "Elapsed Time: " << elapsed() << " ms" << endl;
    cerr << "Score: " << score << endl;
    cerr << "Add Iter: " << state.add_count << endl;
    cerr << "Error Sum: " << state.diff_sum << endl;
    #endif

    // 操作を復元
    persistent_stack<Operation> tmp = state.ops;
    while(!tmp.empty()){
        ops.ops.push_back(tmp.top());
        tmp = tmp.pop();
    }
    reverse(all(ops.ops)); // 復元した操作を逆順にする
    // 最初の壁の操作を削除
    int i=0;
    while(i<ops.ops.size() && ops.ops[i].type==4) i++;
    ops.ops.erase(ops.ops.begin(), ops.ops.begin()+i);

    return ops;
}
} // namespace ver27

namespace ver23_max_eps_no_add{
/* 
  凸包を成す絵の具だけを使う
  header.cppをいじるという力技で実装(マージの時どうすんの？)
  max_epsを指定して、許容誤差内で高さを最小化しておく
  とりあえずadd_count==Hになった時点で、addしない方法を追加する
*/

struct Well{
    Color c; // 色
    double v; // 絵具の残り
    double cap; // 容量
    Well(): c({0,0,0}),cap(0),v(0){}
    // 混ざった後の色を計算
    Color get_mixed_color(int tube_id){
        double add_paint = min(1.0, cap - v);
        double ratio = add_paint / (v + add_paint);
        Color mixed_color = mix(tube[tube_id], c, ratio);
        return mixed_color;
    }
    template<typename ...Args>
    Color get_mixed_color(Args... tube_ids){
        Color mixed_color = c;
        double tmp_v = v;
        int tube_id_arr[] = {tube_ids...};
        for(int tube_id : tube_id_arr){
            double add_paint = min(1.0, cap - tmp_v);
            double ratio = add_paint / (tmp_v + add_paint);
            mixed_color = mix(tube[tube_id], mixed_color, ratio);
            tmp_v = min(1.0 + tmp_v, cap);
        }
        return mixed_color;
    }
    // 実際に投入する
    void add(int tube_id){
        c = get_mixed_color(tube_id);
        v = min(v + 1.0, cap);
        return;
    }
    // 取り出す
    Color extract(){
        v = max(0.0, v - 1.0);
        return c;
    }
    // 捨てる
    void dump(){
        v -= max(0.0, v - 1.0);
        return;
    }
    // ウェルを分割
    pair<Well,Well> split(int k) const {
        Well w1,w2;
        w1.c = w2.c = c;
        w1.v = v * (double)k / cap;
        w2.v = v - w1.v;
        w1.cap = k;
        w2.cap = cap - k;
        return {w1, w2};
    }
};

Well merge(const Well& w1, const Well& w2){
    Well w;
    if(w1.v+w2.v>0) w.c = mix(w1.c, w2.c, (double)w1.v / (w1.v + w2.v));
    else w.c = {0,0,0};
    w.v = w1.v + w2.v;
    w.cap = w1.cap + w2.cap;
    return w;
}

vector<vpii> cell;
void construct_cells(){
    auto& precalc_path = valid_path[K];
    cell.resize(K);
    rep(i,K){
        rep(j,precalc_path[i].size()){
            cell[i].emplace_back(precalc_path[i][j]);
        }
        reverse(all(cell[i]));
    }
}

pair<vvi,vvi> cells_to_wall(){
    vvi vertical_wall(N,vi(N-1,1));
    vvi horizontal_wall(N-1,vi(N,1));
    for(int i = 0; i < cell.size(); i++){
        if(cell[i].size()<2) continue;
        rep(j,cell[i].size()-1){
            if(cell[i][j].X == cell[i][j+1].X){
                // 垂直の壁
                vertical_wall[cell[i][j].X][min(cell[i][j].Y,cell[i][j+1].Y)] = 0;
            }
            else{
                // 水平の壁
                horizontal_wall[min(cell[i][j].X,cell[i][j+1].X)][cell[i][j].Y] = 0;
            }
        }
    }
    return {vertical_wall, horizontal_wall};
}


// 現在の状態を保持
// ・共有スペースの状態
//   ・絵の具の量,色,スペース
// ・各セクションの状態
//   ・絵の具の量,色,スペース
struct State{
    // 操作の履歴
    persistent_stack<Operation> ops;
    // 各セクションの状態
    vector<vector<Well>> well;
    // 共有スペースの状態
    Well shared;
    // 絵具を投入した回数
    int add_count = 0;
    // 誤差の合計
    double diff_sum = 0.0;

    State(){
        shared = Well();
        shared.cap = 400/K*K;
        shared.v = 0.0;
        shared.c = {0,0,0};
        well.resize(K);
    }
    
    // 共有スペースをkマス分割してウェルiに追加
    void shared_split(int i, int k){
        if(k==0) return; // kが0なら何もしない
        
        auto [add_to_well,new_shared] = shared.split(k);
        shared = new_shared;
        well[i].emplace_back(add_to_well);

        // opsの更新
        int now_cap_sum = get_well_cap_sum(i);
        auto [i1,j1] = cell[i][now_cap_sum-1];
        auto [i2,j2] = cell[i][now_cap_sum];
        ops = ops.push(Operation{4, {i1,j1,i2,j2}});
        return;
    }
    
    // ウェルiの末尾のセクションを共有スペースにマージ
    void shared_merge(int i){
        if(well[i].empty()) return; // ウェルiが空なら何もしない
        int pre_cap_sum = get_well_cap_sum(i);
        
        Well &last_well = well[i].back();
        shared = merge(shared,last_well);
        well[i].pop_back();
        
        // opsの更新
        auto [i1,j1] = cell[i][pre_cap_sum-1];
        auto [i2,j2] = cell[i][pre_cap_sum];
        ops = ops.push(Operation{4,{i1, j1, i2, j2}});
        return;
    }
    
    // ウェルiのjセクションを上側サイズがkとなるように分割
    void well_split(int i, int j, int k){
        if(k==0 || k==well[i][j].cap) return;
        auto [above_well,below_well] = well[i][j].split(k);
        well[i][j] = below_well;
        well[i].insert(well[i].begin()+j+1, above_well);
        
        // opsの更新
        int cap_sum = 0;
        rep(l,j+1) cap_sum += well[i][l].cap;
        auto [i1,j1] = cell[i][cap_sum-1];
        auto [i2,j2] = cell[i][cap_sum];
        ops = ops.push(Operation{4, {i1,j1,i2,j2}});
    }
    
    // ウェルiのjセクションとj+1セクションをマージ
    void well_merge(int i, int j){ 
        int cap_sum = 0;
        rep(l,j+1) cap_sum += well[i][l].cap;
        
        well[i][j] = merge(well[i][j], well[i][j+1]);
        well[i].erase(well[i].begin()+j+1);
        
        // opsの更新
        auto [i1,j1] = cell[i][cap_sum-1];
        auto [i2,j2] = cell[i][cap_sum];
        ops = ops.push(Operation{4, {i1,j1,i2,j2}});
    }
    
    // 絵具を追加
    void add_color(int i, int tube_id){
        // 最初のセクションに追加
        well[i][0].add(tube_id);
        
        auto [x,y] = cell[i].front();
        ops = ops.push(Operation{1, {x, y, tube_id}});
        add_count++;
    }
    
    // 絵の具を共有スペースに追加
    void add_color_to_shared(int tube_id){
        shared.add(tube_id);
        ops = ops.push(Operation{1, {0,0,tube_id}});
        add_count++;
    }

    // 絵の具を渡す
    Color give_color(Color tc){
        Color c = shared.extract();
        ops = ops.push(Operation{2, {0, 0}}); // 共有スペースから取り出す
        diff_sum += diff(c,tc);
        return c;
    }
    
    int get_well_cap_sum(int i){
        int cap_sum = 0;
        for(const auto&w: well[i]) cap_sum += w.cap;
        return cap_sum;
    }
    int get_well_cap_sum(){
        int cap_sum = 0;
        rep(i,well.size()) cap_sum += get_well_cap_sum(i);
        return cap_sum;
    }
    int get_well_front_cap(int i){
        if(well[i].empty()) return 0;
        return well[i][0].cap;
    }
    double get_well_front_v(int i){
        if(well[i].empty()) return 0;
        return well[i][0].v;
    }
    int get_score(){
        int score = 1 + D * (add_count - H) + round(diff_sum * 10000);
        return score;
    }
};

// そのターンにどうすれば良いかの情報
struct Process{
    array<int,4> id; // 使うセクションのID (0,1,2,3)
    vector<bool> is_add; // trueなら絵具を追加
    vector<pair<pii,pii>> best_ratio; // 最適な比率 (分子, 分母)
    double best_diff; // 最適な比率の誤差
};

State calc_best_process(State state, const Color &target, double TIME_LIMIT = 1e100){
    const int MAX_CELL = 400/K - 1;
    const double EPS = 5e-6;
    State best_state = state;
    Process process;
    process.best_ratio.resize(4,{{-1,-1},{-1,-1}});

    // 実数比を求める
    double best_diff = 1e100; // 最適な誤差
    int best_dw = -1;
    vector<double> best_w(4); // 最適行動の理論比率
    bool is_timeover = false;
    const auto& precalc_idx = pyramid_sorted();
    for(int id_i=0;!is_timeover && id_i<precalc_idx.size();id_i++){
        const auto& id = precalc_idx[id_i];
        Color c[4]={
            !state.well[id[0]].empty() ? state.well[id[0]][0].c : tube[id[0]],
            !state.well[id[1]].empty() ? state.well[id[1]][0].c : tube[id[1]],
            !state.well[id[2]].empty() ? state.well[id[2]][0].c : tube[id[2]],
            !state.well[id[3]].empty() ? state.well[id[3]][0].c : tube[id[3]]
        };
        // 共有スペースに余っている絵具を考慮して補正する
        // 補正後の色 = (target - Cs*Vs)/(1-Vs)
        // 後で(1-Vs)倍して戻す
        Color Cs = state.shared.c;
        double Vs = state.shared.v/*  * (double)40/(400/K*K-state.get_well_cap_sum()) */;
        Color correct_target = (target - Cs*Vs) / (1.0 - Vs);
        auto _w = relaxed_4mix_problem(c, correct_target);
        rep(i,4) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
        if(*min_element(all(_w)) < -1e-2){
            continue;
        }
        double w_sum = accumulate(all(_w),0.0);
        rep(i,4) _w[i] = max(0.0,_w[i]); // 負の値は0にする
        double now_w_sum = accumulate(all(_w),0.0);
        rep(i,4) _w[i] *= w_sum / now_w_sum; // 合計を戻す
        
        // 改めて計算を回してsharedの影響を詳しく見る
        {
            // 絵具が足りない場合は補充する
            vector<double> v(4);
            rep(i,4) v[i] = state.get_well_front_v(id[i]);
            rep(i,4)if(v[i]<_w[i]-1e-5){
                v[i] += 1.0;
            }
            
            // 分母がspace以上であるような近似比を求める
            vector<double> ideal(4); // 理想の比率
            rep(i,4) ideal[i] = _w[i]/v[i];
            int shared_space = 400/K*K - state.get_well_cap_sum();
            // 共有スペースから漏れる絵の具の量を計算
            rep(i,4){
                auto [s,l] = closest_rational2(
                    ideal[i],
                    max(2,state.get_well_front_cap(id[i])),
                    EPS
                );
                double s_val,l_val;
                if(s.X.X==0) s_val = 0;
                else if(s.X.X==inf) s_val = 1;
                else s_val = (double)s.X.X*s.Y.X / (s.X.Y*s.Y.Y);
                if(l.X.X==0) l_val = 0;
                if(l.X.X==inf) l_val = 1;
                else l_val = (double)l.X.X*l.Y.X / (l.X.Y*l.Y.Y);
                int p1,p2,q1,q2;
                if(abs(s_val-ideal[i]) < abs(l_val-ideal[i])) tie(p1,q1) = s.X, tie(p2,q2) = s.Y;
                else tie(p1,q1) = l.X, tie(p2,q2) = l.Y;
                if(p1==0 || p1==inf) continue; // 開放なしか全開放ならshared_vには関係ない
                // とりあえず合計でこれだけ取る
                double sum_get = state.shared.v * (double)(q1+q2-p1-state.get_well_cap_sum(id[i]))/shared_space;
                // 何個返すか
                double sum_return = 0.0;
                sum_return += state.shared.v * (double)(q1-state.get_well_cap_sum(id[i]))/shared_space;
                sum_return *= (double)p1/q1;
                sum_return += state.shared.v * (double)(q2-p1)/shared_space;
                sum_return *= (double)p2/q2;
                // 返す量を引く
                sum_get -= sum_return;
                // sum_getだけ共有スペースから取られる
                Vs -= sum_get;
            }
            correct_target = (target - Cs*Vs) / (1.0 - Vs);
            _w = relaxed_4mix_problem(c, correct_target);
            rep(i,4) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
            w_sum = accumulate(all(_w),0.0);
            rep(i,4) _w[i] = max(0.0,_w[i]); // 負の値は0にする
            now_w_sum = accumulate(all(_w),0.0);
            rep(i,4) _w[i] *= w_sum / now_w_sum; // 合計を戻す
        }
        
        int DW = 1;
        bool found = false;
        for(int dw=0;(!found || dw<DW);dw++){
            vector<double> w(4); // 実数比
            rep(i,4) w[i] = _w[i] * (1.0 + (double)dw * 1e-5);
            // 絵具が足りない場合は補充する
            vector<bool> is_add(4,false);
            vector<double> v(4);
            rep(i,4) v[i] = state.get_well_front_v(id[i]);
            rep(i,4)if(v[i]<w[i]*1.1){
                v[i] += 1.0;
                is_add[i] = true; // このセクションに絵具を追加する
            }
            
            // 分母がspace以上であるような近似比を求める
            vector<double> ideal(4); // 理想の比率
            rep(i,4) ideal[i] = w[i]/v[i];
            
            vector<pair<pii,pii>> small(4),large(4);
            rep(i,4){
                auto [s,l] = closest_rational2(
                    ideal[i],
                    max(2,state.get_well_front_cap(id[i])),
                    EPS
                ); // 比の有理数近似を列挙
                small[i] = s;
                large[i] = l;
            }
            // 最適な操作を決定
            // 各iについて small or large のどちらを使うか全探索する
            double tmp_err = 1e100;
            State best_tmp;
            rep(S,1<<4){
                State tmp = state;
                int p1[4],q1[4],p2[4],q2[4];
                rep(i,4){
                    if(bit(S,i)){
                        tie(p1[i],q1[i]) = small[i].X;
                        tie(p2[i],q2[i]) = small[i].Y;
                    }
                    else{
                        tie(p1[i],q1[i]) = large[i].X;
                        tie(p2[i],q2[i]) = large[i].Y;
                    }
                }
                // 1~2まで行う
                rep(i,4){
                    // 1.2. q1に広げる
                    if(p1[i]!=0 && p1[i]!=inf){
                        tmp.shared_split(id[i],q1[i]-tmp.get_well_cap_sum(id[i]));
                    }
                    if(tmp.well[id[i]].size()>1) tmp.well_merge(id[i],0);
                }
                // 3~7.まで行う
                rep(i,4){
                    // 必要なら絵具を追加
                    if(is_add[i] && p1[i]!=0){
                        tmp.add_color(id[i],id[i]);
                    }
                    if(p1[i]!=0 && p1[i]!=inf){
                        // 3. p1分子を分離
                        tmp.well_split(id[i],0,p1[i]);
                        // 4. q1+q2-p1のスペースを共有スペースから確保する
                        tmp.shared_split(id[i],q1[i]+q2[i]-p1[i]-tmp.get_well_cap_sum(id[i]));
                        // 5. 3つのうち上2つをマージ
                        tmp.well_merge(id[i],1);
                        // 6. well[i][1]をスプリット
                        tmp.well_split(id[i],1,p2[i]);
                        // 7. 下2つをマージ
                        tmp.well_merge(id[i],0);
                    }
                }
                
                // 8. 共有スペースにマージ
                rep(i,4)if(p1[i]!=0) tmp.shared_merge(id[i]);

                // 混ぜた後の色を計算
                if(tmp.shared.v > 1.0 - 1e-6){ // 絵具が十分にある前提
                    Color give_color = tmp.give_color(target);
                    // 誤差を計算
                    double err = diff(give_color,target);
                    if(chmin(tmp_err, err+max(tmp.add_count-H,0)*D*1e-4)){
                        best_tmp = tmp; // 最適な状態を記録
                    }
                }
            }

            if(tmp_err < 1e10){
                found = true;
            }

            // 最適比を復元
            if(chmin(best_diff,tmp_err)){
                best_state = best_tmp; // 最適な状態を更新
                best_w = ideal;
                best_dw = dw;
            }
        }

        // 時間制限を判定
        if((id_i&31)==0){
            if(elapsed() > TIME_LIMIT){
                is_timeover = true;
            }
        }
    }

    return best_state;
}

struct Final_Process{
    vector<double> w;
    vector<pair<pii,pii>> best_ratio; // 最適な比率 (分子, 分母)
    double best_diff;
};

State calc_final_process(State state, const vector<Color> &target, double TIME_LIMIT = 1e100){
    static unordered_map<double,vector<int>> best_add_num_cache;
    const int WELL_N = state.well.size();

    Color state_c_sum = {0,0,0};
    rep(i,WELL_N) if(!state.well[i].empty()) state_c_sum = state_c_sum + state.well[i][0].c * state.well[i][0].v;
    state_c_sum = state_c_sum + state.shared.c * state.shared.v;
    Color target_c_sum = {0,0,0};
    rep(i,target.size()) target_c_sum = target_c_sum + target[i];
    vi best_add_num(WELL_N,0);
    // 最適な追加を求める
    if(state.add_count < H){
        if(!best_add_num_cache.contains(target_c_sum[0])){
            int need_add = H - state.add_count;
            vi add_num(WELL_N,0);
            Color now_c = state_c_sum;
            double mn_diff = 1e100;
            auto dfs = [&](auto dfs, int i, int now_add) -> void {
                if(elapsed() > TIME_LIMIT) return;
                if(i==WELL_N-1){
                    add_num[i] = need_add - now_add;
                    if(chmin(mn_diff,diff(now_c,target_c_sum))){
                        best_add_num = add_num;
                    }
                    return;
                }
                else{
                    rep(j,need_add-now_add+1){
                        add_num[i] = j;
                        now_c = now_c + tube[i] * j;
                        dfs(dfs,i+1,now_add+j);
                        now_c = now_c - tube[i] * j; // 戻す
                    }
                }
                return;
            }; dfs(dfs,0,0);
            best_add_num_cache[target_c_sum[0]] = best_add_num;
        }
        else best_add_num = best_add_num_cache[target_c_sum[0]];
    }
    
    if(elapsed() > TIME_LIMIT){
        state.diff_sum = 10000;
        return state;
    }

    // 現在場にある絵の具の量
    double now_sum_v = state.shared.v;
    for(int i=0;i<WELL_N;i++){
        for(const auto& w: state.well[i]) now_sum_v += w.v;
    }
    double sum_err = 0;
    rep(t,target.size()-1){
        rep(i,WELL_N){
            if(!state.well[i].empty() && state.well[i][0].v + 1.0 <= state.well[i][0].cap && state.well[i][0].v <= 1.0){
                if(best_add_num[i]>0){
                    state.add_color(i,i);
                    best_add_num[i]--;
                }
            }
            if(state.well[i].empty() && best_add_num[i]>0){
                state.shared_split(i,2);
                state.add_color(i,i);
                best_add_num[i]--;
            }
        }
        vector<Color> c(WELL_N);
        rep(i,WELL_N) c[i] = !state.well[i].empty() ? state.well[i][0].c : Color{0,0,0};
        vector<double> u(WELL_N);
        rep(i,WELL_N) u[i] = !state.well[i].empty() ? state.well[i][0].v : 0.0;
        const Color &tc = target[t];
        // 計算パート
        // 共有スペースに余っている絵具を考慮して補正する
        // 補正後の色 = (target - Cs*Vs)/(1-Vs)
        // 後で(1-Vs)倍して戻す
        Color Cs = state.shared.c;
        double Vs = state.shared.v;
        Color correct_target = (tc - Cs*Vs) / (1.0 - Vs);
        rep(i,WELL_N) u[i] /= 1.0 - Vs; // 共有スペースの補正をかける
        auto _w = ratio_heuristic_solver::solve(correct_target, c, u);
        rep(i,WELL_N) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
        rep(i,WELL_N) u[i] *= 1 - Vs; // 共有スペースの補正を戻す
        
        // 改めて計算を回してsharedの影響を詳しく見る
        {
            // 絵具が足りない場合は補充する
            vector<double> v(WELL_N);
            rep(i,WELL_N) v[i] = state.get_well_front_v(i);
            
            // 分母がspace以上であるような近似比を求める
            vector<double> ideal(WELL_N); // 理想の比率
            rep(i,WELL_N){
                ideal[i] = v[i] > 0 ? _w[i]/v[i] : 0.0;
            }
            int shared_space = 400/K*K - state.get_well_cap_sum();
            // 共有スペースから漏れる絵の具の量を計算
            rep(i,WELL_N){
                if(v[i]==0) continue;
                auto [s,l] = closest_rational2(
                    ideal[i],
                    max(2,state.get_well_front_cap(i)),
                    5e-6
                );
                double s_val,l_val;
                if(s.X.X==0) s_val = 0;
                else if(s.X.X==inf) s_val = 1;
                else s_val = (double)s.X.X*s.Y.X / (s.X.Y*s.Y.Y);
                if(l.X.X==0) l_val = 0;
                if(l.X.X==inf) l_val = 1;
                else l_val = (double)l.X.X*l.Y.X / (l.X.Y*l.Y.Y);
                int p1,p2,q1,q2;
                if(abs(s_val-ideal[i]) < abs(l_val-ideal[i])) tie(p1,q1) = s.X, tie(p2,q2) = s.Y;
                else tie(p1,q1) = l.X, tie(p2,q2) = l.Y;
                if(p1==0 || p1==inf) continue; // 開放なしか全開放ならshared_vには関係ない
                // とりあえず合計でこれだけ取る
                double sum_get = state.shared.v * (double)(q1+q2-p1-state.get_well_cap_sum(i))/shared_space;
                // 何個返すか
                double sum_return = 0.0;
                sum_return += state.shared.v * (double)(q1-state.get_well_cap_sum(i))/shared_space;
                sum_return *= (double)p1/q1;
                sum_return += state.shared.v * (double)(q2-p1)/shared_space;
                sum_return *= (double)p2/q2;
                // 返す量を引く
                sum_get -= sum_return;
                // sum_getだけ共有スペースから取られる
                Vs -= sum_get;
            }
            correct_target = (tc - Cs*Vs) / (1.0 - Vs);
            rep(i,WELL_N) u[i] /= 1.0 - Vs; // 共有スペースの補正をかける
            _w = ratio_heuristic_solver::solve(correct_target, c, u);
            rep(i,WELL_N) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
            rep(i,WELL_N) u[i] *= 1 - Vs; // 共有スペースの補正を戻す
        }

        // 操作パート
        int DW = 1;
        double best_diff = 1e100;
        bool found = false;
        State nxt_state;
        for(int dw=0;(!found || dw<DW);dw++){
            vector<double> w(WELL_N); // 実数比
            rep(i,WELL_N) w[i] = _w[i] * (1.0 + (double)dw * 1e-5);
            vector<double> v(WELL_N);
            rep(i,WELL_N) v[i] = state.get_well_front_v(i);
            
            // 分母がspace以上であるような近似比を求める
            vector<double> ideal(WELL_N); // 理想の比率
            rep(i,WELL_N){
                ideal[i] = v[i] > 0 ? w[i]/v[i] : 0.0;
            }
            
            vector<pair<pii,pii>> small,large;
            vi id;
            rep(i,WELL_N){
                if(v[i]==0) continue;
                auto [s,l] = closest_rational2(
                    ideal[i],
                    max(2,state.get_well_front_cap(i)),
                    5e-6
                ); // 比の有理数近似を列挙
                small.push_back(s), large.push_back(l);
                id.push_back(i);
            }

            // 最適な操作を決定
            // 各iについて small or large のどちらを使うか全探索する
            double tmp_err = 1e100;
            State best_tmp;
            // とりあえず近い値にセット
            vi p1(id.size()),q1(id.size()),p2(id.size()),q2(id.size());
            rep(i,id.size()){
                double s_val = (double)small[i].X.X * small[i].Y.X / (double)small[i].X.Y / small[i].Y.Y;
                double l_val = (double)large[i].X.X * large[i].Y.X / (double)large[i].X.Y / large[i].Y.Y;
                if(abs(s_val-ideal[id[i]]) < abs(l_val-ideal[id[i]])){
                    tie(p1[i],q1[i]) = small[i].X;
                    tie(p2[i],q2[i]) = small[i].Y;
                }
                else{
                    tie(p1[i],q1[i]) = large[i].X;
                    tie(p2[i],q2[i]) = large[i].Y;
                }
            }
            while(tmp_err > 1e10){
                State tmp = state;
                
                // 1~7まで行う
                rep(i,id.size()){
                    // 1.2. q1に広げる
                    if(p1[i]!=0 && p1[i]!=inf){
                        tmp.shared_split(id[i],q1[i]-tmp.get_well_cap_sum(id[i]));
                    }
                    if(tmp.well[id[i]].size()>1) tmp.well_merge(id[i],0);
                    if(p1[i]!=0 && p1[i]!=inf){
                        // 3. p1分子を分離
                        tmp.well_split(id[i],0,p1[i]);
                        // 4. q1+q2-p1のスペースを共有スペースから確保する
                        tmp.shared_split(id[i],q1[i]+q2[i]-p1[i]-tmp.get_well_cap_sum(id[i]));
                        // 5. 3つのうち上2つをマージ
                        tmp.well_merge(id[i],1);
                        // 6. well[i][1]をスプリット
                        tmp.well_split(id[i],1,p2[i]);
                        // 7. 下2つをマージ
                        tmp.well_merge(id[i],0);
                    }
                }
                
                // 8. 共有スペースにマージ
                rep(i,id.size())if(p1[i]!=0) tmp.shared_merge(id[i]);

                // 混ぜた後の色を計算
                if(tmp.shared.v > 1.0 - 1e-6){ // 絵具が十分にある前提
                    Color give_color = tmp.give_color(tc);
                    // 誤差を計算
                    double err = diff(give_color,tc);
                    if(chmin(tmp_err, err)){
                        best_tmp = tmp; // 最適な状態を記録
                    }
                }

                // 見つけたら抜ける
                if(tmp_err < 1e10) break;

                // 近似比を更新 
                // 大きくした後の誤差が小さいものを選ぶ
                double mn_diff = 1e100;
                int best_i = -1;
                rep(i,id.size()){
                    if(pair(pii(p1[i],q1[i]),pii(p2[i],q2[i])) == large[i]) continue;
                    if(chmin(mn_diff,abs(ideal[id[i]] - (double)large[i].X.X * large[i].Y.X / (double)large[i].X.Y / large[i].Y.Y))){
                        best_i = i;
                    }
                }
                if(best_i == -1) break; // これ以上大きくできないなら抜ける

                tie(p1[best_i],q1[best_i]) = large[best_i].X;
                tie(p2[best_i],q2[best_i]) = large[best_i].Y;
            }

            if(tmp_err < 1e10){
                found = true;
            }

            // 最適比を復元
            if(chmin(best_diff,tmp_err)){
                nxt_state = best_tmp;
            }
        }
        sum_err += best_diff;
        state = nxt_state;
    }

    // 最後のターンはある分を開放
    rep(i,WELL_N){
        while(!state.well[i].empty()){
            state.shared_merge(i);
        }
    }
    rep(i,WELL_N){
        while(best_add_num[i]>0){
            state.add_color_to_shared(i);
            best_add_num[i]--;
        }
    }
    Color c = state.give_color(target.back());
    double final_diff = diff(c,target.back());
    sum_err += final_diff;

    return state;
}

Ops main(){
    Ops ops; // 操作の履歴
    construct_cells();
    cpp_dump("constructCells");

    // 最初の操作を計算
    State state;
    state = calc_best_process(state,target[0]);
    #ifndef ONLINE_JUDGE
    cerr << "Initial calculation done." << endl;
    cerr << "Elapsed Time: " << elapsed() << " ms" << endl;
    #endif

    { // 壁の設定
    auto [v_wall,h_wall] = cells_to_wall();
    // 混ぜる場所を繋げる
    rep(i,K-1) v_wall[0][i] = 0;
    // 初期の各ウェルの仕切り
    // 1手目の壁の状態を見る
    rep(i,state.well.size()){
        persistent_stack tmp = state.ops;
        vector<Operation> ops;
        while(!tmp.empty()){
            ops.push_back(tmp.top());
            tmp = tmp.pop();
        }
        reverse(all(ops));
        for(const auto &op:ops){
            if(op.type!=4) break;
            int x1 = op.args[0], y1 = op.args[1], x2 = op.args[2], y2 = op.args[3];
            if(x1==x2){
                v_wall[x1][min(y1,y2)] = 1;
            }
            else{
                h_wall[min(x1,x2)][y1] = 1;
            }
        }
    }
    ops.init_wall(v_wall,h_wall);
    }

    double err_sum = 0.0; // 誤差の合計
    
    double initial_diff = state.diff_sum - err_sum;

    vector<tuple<int,State,double>> pool_state;
    { // 絵の具をH回追加するまで2段階で処理する
    double init_time = elapsed();
    rep(t,1,H){
        // 操作を計算
        State nxt_state = calc_best_process(state,target[t],t*2.5 + init_time);
        if(state.add_count <= H && nxt_state.add_count > H-10){
            pool_state.emplace_back(t,state,1e100);
        }

        state = nxt_state; // 状態を更新
    
        double actual_diff = nxt_state.diff_sum - err_sum;

        // 誤差を加算
        err_sum += actual_diff;
    }
    }
    #ifndef ONLINE_JUDGE
    cerr << "Add Count: " << state.add_count << endl;
    cerr << "Elapsed Time: " << elapsed() << " ms" << endl;
    #endif

    // 途中からを再計算
    reverse(all(pool_state));
    int pool_size = pool_state.size();
    double pool_search_init_time = elapsed();
    while(!pool_state.empty() && elapsed() < 2900){
        rep(i,pool_state.size()){
            auto &[t,_state,score] = pool_state[i];
            vector<Color> remain_target = vector<Color>(target+t,target+H);
            State _nxt_state = calc_final_process(_state, remain_target,2900);
            if(_nxt_state.get_score() < state.get_score()){
                state = _nxt_state;
            }
            score = _nxt_state.get_score();
            #ifndef ONLINE_JUDGE
            cerr << "t: " << t << ", score : " << _nxt_state.get_score() << ", Elapsed Time : " << elapsed() << "ms\n";
            #endif
            if(elapsed() > 2900) break;
        }
        if(elapsed() > 2900) break;
        // スコアの高い順にソート
        sort(all(pool_state),[](const auto &a, const auto &b){
            return get<2>(a) < get<2>(b);
        });
        // スコアの低いものを排除
        // 時間とともに少しずつ減らす
        double progress = (double)(elapsed() - pool_search_init_time) / (double) (2900 - pool_search_init_time);
        pool_state.resize(max(1,(int)(pool_size*(1-progress))));
    }

    int score = state.get_score();
    #ifndef ONLINE_JUDGE
    cerr << "Elapsed Time: " << elapsed() << " ms" << endl;
    cerr << "Score: " << score << endl;
    cerr << "Add Iter: " << state.add_count << endl;
    cerr << "Error Sum: " << state.diff_sum << endl;
    #endif

    // 操作を復元
    persistent_stack<Operation> tmp = state.ops;
    while(!tmp.empty()){
        ops.ops.push_back(tmp.top());
        tmp = tmp.pop();
    }
    reverse(all(ops.ops)); // 復元した操作を逆順にする
    // 最初の壁の操作を削除
    int i=0;
    while(i<ops.ops.size() && ops.ops[i].type==4) i++;
    ops.ops.erase(ops.ops.begin(), ops.ops.begin()+i);

    return ops;
}
} // namespace ver23_max_eps_no_add

namespace ver29{

/* 
　仕切りを駆使して、絵具を最適な整数比で混ぜ合わせる
　T = 12000 ~ 18000に対応
  途中まではver9, そこからver27に派生
*/

struct Well{
    Color c; // 色
    double v; // 絵具の残り
    double cap; // 容量
    Well(): c({0,0,0}),cap(0),v(0){}
    // 混ざった後の色を計算
    Color get_mixed_color(int tube_id){
        double add_paint = min(1.0, cap - v);
        double ratio = add_paint / (v + add_paint);
        Color mixed_color = mix(tube[tube_id], c, ratio);
        return mixed_color;
    }
    template<typename ...Args>
    Color get_mixed_color(Args... tube_ids){
        Color mixed_color = c;
        double tmp_v = v;
        int tube_id_arr[] = {tube_ids...};
        for(int tube_id : tube_id_arr){
            double add_paint = min(1.0, cap - tmp_v);
            double ratio = add_paint / (tmp_v + add_paint);
            mixed_color = mix(tube[tube_id], mixed_color, ratio);
            tmp_v = min(1.0 + tmp_v, cap);
        }
        return mixed_color;
    }
    // 実際に投入する
    void add(int tube_id){
        c = get_mixed_color(tube_id);
        v = min(v + 1.0, cap);
        return;
    }
    // 取り出す
    Color extract(){
        v = max(0.0, v - 1.0);
        return c;
    }
    // 捨てる
    void dump(){
        v -= max(0.0, v - 1.0);
        return;
    }
    // ウェルを分割
    pair<Well,Well> split(int k) const {
        Well w1,w2;
        w1.c = w2.c = c;
        w1.v = v * (double)k / cap;
        w2.v = v - w1.v;
        w1.cap = k;
        w2.cap = cap - k;
        return {w1, w2};
    }
};

vector<vpii> cell;
void construct_cells(){
    auto& precalc_path = valid_path[K];
    cell.resize(K);
    rep(i,K){
        rep(j,precalc_path[i].size()){
            cell[i].emplace_back(precalc_path[i][j]);
        }
        reverse(all(cell[i]));
    }
}

Well merge(const Well& w1, const Well& w2){
    Well w;
    if(w1.v+w2.v>0) w.c = mix(w1.c, w2.c, (double)w1.v / (w1.v + w2.v));
    else w.c = {0,0,0};
    w.v = w1.v + w2.v;
    w.cap = w1.cap + w2.cap;
    return w;
}

pair<vvi,vvi> cells_to_wall(){
    vvi vertical_wall(N,vi(N-1,1));
    vvi horizontal_wall(N-1,vi(N,1));
    for(int i = 0; i < cell.size(); i++){
        if(cell[i].size()<2) continue;
        rep(j,cell[i].size()-1){
            if(cell[i][j].X == cell[i][j+1].X){
                // 垂直の壁
                vertical_wall[cell[i][j].X][min(cell[i][j].Y,cell[i][j+1].Y)] = 0;
            }
            else{
                // 水平の壁
                horizontal_wall[min(cell[i][j].X,cell[i][j+1].X)][cell[i][j].Y] = 0;
            }
        }
    }
    return {vertical_wall, horizontal_wall};
}

vi best_ratio[H];
double best_diff[H];
void calc_best_ratio(int END_T = H, int NONZERO = 4){
    const int CELL_PER_PAINT = 400/(2*K);
    double init_time = elapsed();
    rep(t,END_T){
        const Color& tar = target[t];
        tie(best_diff[t],best_ratio[t]) =
            pruning_search_ratio::ratio_search(tar,CELL_PER_PAINT,init_time + (t+1)*2,NONZERO);
    }
}

struct State{
    // 操作の履歴
    persistent_stack<Operation> ops;
    // 各セクションの状態
    vector<vector<Well>> well;
    // 共有スペースの状態
    Well shared;
    // 絵具を投入した回数
    int add_count = 0;
    // 誤差の合計
    double diff_sum = 0.0;
    // 累計操作回数
    int t = 0;

    State(){
        shared = Well();
        shared.cap = 400/K*K;
        shared.v = 0.0;
        shared.c = {0,0,0};
        well.resize(K);
    }
    
    // 共有スペースをkマス分割してウェルiに追加
    void shared_split(int i, int k){
        if(k==0) return; // kが0なら何もしない
        
        auto [add_to_well,new_shared] = shared.split(k);
        shared = new_shared;
        well[i].emplace_back(add_to_well);

        // opsの更新
        int now_cap_sum = get_well_cap_sum(i);
        auto [i1,j1] = cell[i][now_cap_sum-1];
        auto [i2,j2] = cell[i][now_cap_sum];
        ops = ops.push(Operation{4, {i1,j1,i2,j2}});
        t++;
        return;
    }
    
    // ウェルiの末尾のセクションを共有スペースにマージ
    void shared_merge(int i){
        if(well[i].empty()) return; // ウェルiが空なら何もしない
        int pre_cap_sum = get_well_cap_sum(i);
        
        Well &last_well = well[i].back();
        shared = merge(shared,last_well);
        well[i].pop_back();
        
        // opsの更新
        auto [i1,j1] = cell[i][pre_cap_sum-1];
        auto [i2,j2] = cell[i][pre_cap_sum];
        ops = ops.push(Operation{4,{i1, j1, i2, j2}});
        t++;
        return;
    }
    
    // ウェルiのjセクションを上側サイズがkとなるように分割
    void well_split(int i, int j, int k){
        if(k==0 || k==well[i][j].cap) return;
        auto [above_well,below_well] = well[i][j].split(k);
        well[i][j] = below_well;
        well[i].insert(well[i].begin()+j+1, above_well);
        
        // opsの更新
        int cap_sum = 0;
        rep(l,j+1) cap_sum += well[i][l].cap;
        auto [i1,j1] = cell[i][cap_sum-1];
        auto [i2,j2] = cell[i][cap_sum];
        ops = ops.push(Operation{4, {i1,j1,i2,j2}});
        t++;
    }
    
    // ウェルiのjセクションとj+1セクションをマージ
    void well_merge(int i, int j){
        int cap_sum = 0;
        rep(l,j+1) cap_sum += well[i][l].cap;
        
        well[i][j] = merge(well[i][j], well[i][j+1]);
        well[i].erase(well[i].begin()+j+1);
        
        // opsの更新
        auto [i1,j1] = cell[i][cap_sum-1];
        auto [i2,j2] = cell[i][cap_sum];
        ops = ops.push(Operation{4, {i1,j1,i2,j2}});
        t++;
    }
    
    // 絵具を追加
    void add_color(int i, int tube_id){
        // 最初のセクションに追加
        well[i][0].add(tube_id);
        
        auto [x,y] = cell[i].front();
        ops = ops.push(Operation{1, {x, y, tube_id}});
        t++;
        add_count++;
    }
    
    // 絵の具を共有スペースに追加
    void add_color_to_shared(int tube_id){
        shared.add(tube_id);
        ops = ops.push(Operation{1, {0,0,tube_id}});
        t++;
        add_count++;
    }

    // 絵の具を渡す
    Color give_color(Color tc){
        Color c = shared.extract();
        ops = ops.push(Operation{2, {0, 0}}); // 共有スペースから取り出す
        t++;
        diff_sum += diff(c,tc);
        return c;
    }
    
    int get_well_cap_sum(int i){
        int cap_sum = 0;
        for(const auto&w: well[i]) cap_sum += w.cap;
        return cap_sum;
    }
    int get_well_cap_sum(){
        int cap_sum = 0;
        rep(i,well.size()) cap_sum += get_well_cap_sum(i);
        return cap_sum;
    }
    int get_well_front_cap(int i){
        if(well[i].empty()) return 0;
        return well[i][0].cap;
    }
    double get_well_front_v(int i){
        if(well[i].empty()) return 0;
        return well[i][0].v;
    }
    int get_score(){
        int score = 1 + D * (add_count - H) + round(diff_sum * 10000);
        return score;
    }
};

// 1段階操作と2段階操作を両方する
vector<State> calc_best_process1(State state, const Color &target, double TIME_LIMIT = 1e100){
    const int MAX_CELL = 400/K - 1;
    const double EPS = 5e-6;
    const int MAX_T_PER_PROCESS = 50;
    static vector<State> best_states(MAX_T_PER_PROCESS,State());
    static vector<State> best_tmps(MAX_T_PER_PROCESS,State());
    vector<double> best_diff(MAX_T_PER_PROCESS,1e100);

    // 実数比を求める
    int best_dw = -1;
    vector<double> best_w(4); // 最適行動の理論比率
    bool is_timeover = false;
    const auto& precalc_idx = pyramid_sorted();
    double arrow_negative = -1e-3;
    while(*min_element(all(best_diff)) > 1e10){
        for(int id_i=0;(*min_element(all(best_diff))>1e10 || !is_timeover) && id_i<precalc_idx.size();id_i++){
            const auto& id = precalc_idx[id_i];
            Color c[4]={
                state.get_well_front_v(id[0]) > 1e-3 ? state.well[id[0]][0].c : tube[id[0]],
                state.get_well_front_v(id[1]) > 1e-3 ? state.well[id[1]][0].c : tube[id[1]],
                state.get_well_front_v(id[2]) > 1e-3 ? state.well[id[2]][0].c : tube[id[2]],
                state.get_well_front_v(id[3]) > 1e-3 ? state.well[id[3]][0].c : tube[id[3]]
            };
            // 共有スペースに余っている絵具を考慮して補正する
            // 補正後の色 = (target - Cs*Vs)/(1-Vs)
            // 後で(1-Vs)倍して戻す
            Color Cs = state.shared.c;
            double Vs = state.shared.v;
            Color correct_target = (target - Cs*Vs) / (1.0 - Vs);
            auto _w = relaxed_4mix_problem(c, correct_target);
            rep(i,4) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
            if(*min_element(all(_w)) < arrow_negative){
                continue;
            }
            double w_sum = accumulate(all(_w),0.0);
            rep(i,4) _w[i] = max(0.0,_w[i]); // 負の値は0にする
            double now_w_sum = accumulate(all(_w),0.0);
            rep(i,4) _w[i] *= w_sum / now_w_sum; // 合計を戻す
            
            int DW = 1;
            bool found = false;
            for(int dw=0;(!found || dw<DW);dw++){
                vector<double> w(4); // 実数比
                rep(i,4) w[i] = _w[i] * (1.0 + (double)(dw-DW/2)* 1e-4);
                // 絵具が足りない場合は補充する
                vector<bool> is_add(4,false);
                vector<double> v(4);
                rep(i,4) v[i] = state.get_well_front_v(id[i]);
                rep(i,4)if(v[i]<w[i]-1e-5){
                    v[i] += 1.0;
                    is_add[i] = true; // このセクションに絵具を追加する
                }
                
                // 分母がspace以上であるような近似比を求める
                vector<double> ideal(4); // 理想の比率
                rep(i,4){
                    if(v[i]==0) ideal[i] = 0;
                    else ideal[i] = w[i]/v[i];
                }
                
                vector<pii> s1(4),l1(4); // 1段階の大きい小さい
                rep(i,4){
                    auto [s,l] = closest_rational1(
                        ideal[i],
                        max(2,state.get_well_front_cap(id[i])),
                        0
                    );
                    s1[i] = s;
                    l1[i] = l;
                }
                vector<pair<pii,pii>> s2(4),l2(4); // 2段階の大きい小さい
                rep(i,4){
                    auto [s,l] = closest_rational2(
                        ideal[i],
                        max(2,state.get_well_front_cap(id[i])),
                        EPS
                    );
                    s2[i] = s;
                    l2[i] = l;
                }
                
                // 最適な操作を決定
                // 各iについて s1[i],l1[i],s2[i],l2[i]のどれを使うか全探索
                vector<double> tmp_err(MAX_T_PER_PROCESS,1e100);
                for(int OT:{0,3,12,15}){
                    rep(SL,1<<4){
                        int p1[4],q1[4],p2[4],q2[4]; // 1段階の時はp1,q1のみ
                        rep(i,4){
                            if(bit(OT,i)){
                                if(bit(SL,i)){
                                    tie(p1[i],q1[i]) = s1[i];
                                }
                                else{
                                    tie(p1[i],q1[i]) = l1[i];
                                }
                            }
                            else{
                                if(bit(SL,i)){
                                    tie(p1[i],q1[i]) = s2[i].X;
                                    tie(p2[i],q2[i]) = s2[i].Y;
                                }
                                else{
                                    tie(p1[i],q1[i]) = l2[i].X;
                                    tie(p2[i],q2[i]) = l2[i].Y;
                                }
                            }
                        }
                        State tmp = state;

                        // 1~2まで行う
                        rep(i,4){
                            // 1.2. q1に広げる
                            if(p1[i]!=0 && p1[i]!=inf){
                                tmp.shared_split(id[i],q1[i]-tmp.get_well_cap_sum(id[i]));
                            }
                            // 絵の具を追加するスペースを確保
                            if(p1[i]==inf && is_add[i] && tmp.get_well_front_cap(id[i]) + 1.0 > tmp.get_well_cap_sum(id[i])){
                                tmp.shared_split(id[i],2);
                            }
                            if(tmp.well[id[i]].size()>1) tmp.well_merge(id[i],0);
                        }
                        // 絵の具の追加
                        rep(i,4){
                            if(is_add[i] && p1[i]!=0){
                                tmp.add_color(id[i],id[i]);
                            }
                        }
                        rep(i,4){
                            if(bit(OT,i)){ // 1段階
                                if(p1[i]!=0 && p1[i]!=inf){
                                    // 3. p1分子を分離
                                    tmp.well_split(id[i],0,p1[i]);
                                }
                            }
                            else{ // 2段階
                                // 3~7.まで行う(共有スペースにマージする直前まで)
                                if(p1[i]!=0 && p1[i]!=inf){
                                    // 3. p1分子を分離
                                    tmp.well_split(id[i],0,p1[i]);
                                    // 4. q1+q2-p1のスペースを共有スペースから確保する
                                    tmp.shared_split(id[i],q1[i]+q2[i]-p1[i]-tmp.get_well_cap_sum(id[i]));
                                    // 5. 3つのうち上2つをマージ
                                    tmp.well_merge(id[i],1);
                                    // 6. well[i][1]をスプリット
                                    tmp.well_split(id[i],1,p2[i]);
                                    // 7. 下2つをマージ
                                    tmp.well_merge(id[i],0);
                                }
                            }
                        }
                        
                        // 8. 共有スペースにマージ
                        rep(i,4)if(p1[i]!=0) tmp.shared_merge(id[i]);

                        // 混ぜた後の色を計算
                        if(tmp.shared.v > 1.0 - 1e-6){ // 絵具が十分にある前提
                            Color give_color = tmp.give_color(target);
                            // 誤差を計算
                            double err = diff(give_color,target);
                            int t_diff = tmp.t - state.t;
                            if(chmin(tmp_err[t_diff], err+max(tmp.add_count-H,0)*D*1e-4)){
                                best_tmps[t_diff] = tmp; // 最適な状態を記録
                            }
                        }

                        // 更新されなかった場合はlargeに切り替え
                        int mn_id = -1;
                        double mn_diff_worse = 1e100;
                        rep(i,4){
                            if(bit(OT,i)){
                                if(pii(p1[i],q1[i])!=l1[i]){
                                    // 誤差の悪化量
                                    if(chmin(mn_diff_worse, abs(ideal[i]-(double)l1[i].X/l1[i].Y) - abs(ideal[i]-(double)p1[i]/q1[i]))){
                                        mn_id = i;
                                    }
                                }
                            }
                            else{
                                if(pair(pii(p1[i],q1[i]),pii(p2[i],q2[i]))!=l2[i]){
                                    // 誤差の悪化量
                                    if(chmin(mn_diff_worse, abs(ideal[i]-(double)l2[i].X.X*l2[i].Y.X/(l2[i].X.Y*l2[i].Y.Y)) - abs(ideal[i]-(double)p1[i]*p2[i]/(q1[i]*q2[i])))){
                                        mn_id = i;
                                    }
                                }
                            }
                        }
                        if(mn_id==-1) break;
                        if(bit(OT,mn_id)) tie(p1[mn_id],q1[mn_id]) = l1[mn_id];
                        else tie(p1[mn_id],q1[mn_id]) = l2[mn_id].X, tie(p2[mn_id],q2[mn_id]) = l2[mn_id].Y;
                    }
                }

                if(*min_element(all(tmp_err)) < 1e10){
                    found = true;
                }

                // 最適比を復元
                rep(t_diff,MAX_T_PER_PROCESS){
                    if(tmp_err[t_diff] > 1e10) continue; // 見つかっていない状態はスキップ
                    if(chmin(best_diff[t_diff], tmp_err[t_diff])){
                        best_states[t_diff] = best_tmps[t_diff]; // 最適な状態を更新
                    }
                }
            }

            // 時間制限を判定
            if((id_i&31)==0){
                if(elapsed() > TIME_LIMIT){
                    is_timeover = true;
                }
            }
        }
        arrow_negative *= 10;
    }
    vector<State> ret_state;
    rep(i,MAX_T_PER_PROCESS){
        if(best_diff[i] > 1e10) continue; // 見つかっていない状態はスキップ
        ret_state.push_back(best_states[i]);
    }
    return ret_state;
}

State calc_final_process(State state, const vector<Color> &target, double TIME_LIMIT = 1e100){
    static unordered_map<double,vector<int>> best_add_num_cache;
    const int WELL_N = state.well.size();

    Color state_c_sum = {0,0,0};
    rep(i,WELL_N) if(!state.well[i].empty()) state_c_sum = state_c_sum + state.well[i][0].c * state.well[i][0].v;
    state_c_sum = state_c_sum + state.shared.c * state.shared.v;
    Color target_c_sum = {0,0,0};
    rep(i,target.size()) target_c_sum = target_c_sum + target[i];
    vi best_add_num(WELL_N,0);
    // 最適な追加を求める
    if(state.add_count < H){
        if(!best_add_num_cache.contains(target_c_sum[0])){
            int need_add = H - state.add_count;
            vi add_num(WELL_N,0);
            Color now_c = state_c_sum;
            double mn_diff = 1e100;
            auto dfs = [&](auto dfs, int i, int now_add) -> void {
                if(elapsed() > TIME_LIMIT) return;
                if(i==WELL_N-1){
                    add_num[i] = need_add - now_add;
                    if(chmin(mn_diff,diff(now_c,target_c_sum))){
                        best_add_num = add_num;
                    }
                    return;
                }
                else{
                    rep(j,need_add-now_add+1){
                        add_num[i] = j;
                        now_c = now_c + tube[i] * j;
                        dfs(dfs,i+1,now_add+j);
                        now_c = now_c - tube[i] * j; // 戻す
                    }
                }
                return;
            }; dfs(dfs,0,0);
            best_add_num_cache[target_c_sum[0]] = best_add_num;
        }
        else best_add_num = best_add_num_cache[target_c_sum[0]];
    }
    
    if(elapsed() > TIME_LIMIT){
        state.diff_sum = 10000;
        return state;
    }

    // 現在場にある絵の具の量
    double now_sum_v = state.shared.v;
    for(int i=0;i<WELL_N;i++){
        for(const auto& w: state.well[i]) now_sum_v += w.v;
    }
    double sum_err = 0;
    rep(t,target.size()-1){
        rep(i,WELL_N){
            if(!state.well[i].empty() && state.well[i][0].v + 1.0 <= state.well[i][0].cap && state.well[i][0].v <= 1.0){
                if(best_add_num[i]>0){
                    state.add_color(i,i);
                    best_add_num[i]--;
                }
            }
            if(state.well[i].empty() && best_add_num[i]>0){
                state.shared_split(i,2);
                state.add_color(i,i);
                best_add_num[i]--;
            }
        }
        vector<Color> c(WELL_N);
        rep(i,WELL_N) c[i] = !state.well[i].empty() ? state.well[i][0].c : Color{0,0,0};
        vector<double> u(WELL_N);
        rep(i,WELL_N) u[i] = !state.well[i].empty() ? state.well[i][0].v : 0.0;
        const Color &tc = target[t];
        // 計算パート
        // 共有スペースに余っている絵具を考慮して補正する
        // 補正後の色 = (target - Cs*Vs)/(1-Vs)
        // 後で(1-Vs)倍して戻す
        Color Cs = state.shared.c;
        double Vs = state.shared.v;
        Color correct_target = (tc - Cs*Vs) / (1.0 - Vs);
        rep(i,WELL_N) u[i] /= 1.0 - Vs; // 共有スペースの補正をかける
        auto _w = ratio_heuristic_solver::solve(correct_target, c, u);
        rep(i,WELL_N) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
        rep(i,WELL_N) u[i] *= 1 - Vs; // 共有スペースの補正を戻す
        
        // 改めて計算を回してsharedの影響を詳しく見る
        {
            // 絵具が足りない場合は補充する
            vector<double> v(WELL_N);
            rep(i,WELL_N) v[i] = state.get_well_front_v(i);
            
            // 分母がspace以上であるような近似比を求める
            vector<double> ideal(WELL_N); // 理想の比率
            rep(i,WELL_N){
                ideal[i] = v[i] > 0 ? _w[i]/v[i] : 0.0;
            }
            int shared_space = 400/K*K - state.get_well_cap_sum();
            // 共有スペースから漏れる絵の具の量を計算
            rep(i,WELL_N){
                if(v[i]==0) continue;
                auto [s,l] = closest_rational2(
                    ideal[i],
                    max(2,state.get_well_front_cap(i)),
                    5e-6
                );
                double s_val,l_val;
                if(s.X.X==0) s_val = 0;
                else if(s.X.X==inf) s_val = 1;
                else s_val = (double)s.X.X*s.Y.X / (s.X.Y*s.Y.Y);
                if(l.X.X==0) l_val = 0;
                if(l.X.X==inf) l_val = 1;
                else l_val = (double)l.X.X*l.Y.X / (l.X.Y*l.Y.Y);
                int p1,p2,q1,q2;
                if(abs(s_val-ideal[i]) < abs(l_val-ideal[i])) tie(p1,q1) = s.X, tie(p2,q2) = s.Y;
                else tie(p1,q1) = l.X, tie(p2,q2) = l.Y;
                if(p1==0 || p1==inf) continue; // 開放なしか全開放ならshared_vには関係ない
                // とりあえず合計でこれだけ取る
                double sum_get = state.shared.v * (double)(q1+q2-p1-state.get_well_cap_sum(i))/shared_space;
                // 何個返すか
                double sum_return = 0.0;
                sum_return += state.shared.v * (double)(q1-state.get_well_cap_sum(i))/shared_space;
                sum_return *= (double)p1/q1;
                sum_return += state.shared.v * (double)(q2-p1)/shared_space;
                sum_return *= (double)p2/q2;
                // 返す量を引く
                sum_get -= sum_return;
                // sum_getだけ共有スペースから取られる
                Vs -= sum_get;
            }
            correct_target = (tc - Cs*Vs) / (1.0 - Vs);
            rep(i,WELL_N) u[i] /= 1.0 - Vs; // 共有スペースの補正をかける
            _w = ratio_heuristic_solver::solve(correct_target, c, u);
            rep(i,WELL_N) _w[i] *= 1 - Vs; // 共有スペースの補正を戻す
            rep(i,WELL_N) u[i] *= 1 - Vs; // 共有スペースの補正を戻す
        }

        // 操作パート
        int DW = 1;
        double best_diff = 1e100;
        bool found = false;
        State nxt_state;
        for(int dw=0;(!found || dw<DW);dw++){
            vector<double> w(WELL_N); // 実数比
            rep(i,WELL_N) w[i] = _w[i] * (1.0 + (double)dw * 1e-5);
            vector<double> v(WELL_N);
            rep(i,WELL_N) v[i] = state.get_well_front_v(i);
            
            // 分母がspace以上であるような近似比を求める
            vector<double> ideal(WELL_N); // 理想の比率
            rep(i,WELL_N){
                ideal[i] = v[i] > 0 ? w[i]/v[i] : 0.0;
            }
            
            vector<pair<pii,pii>> small,large;
            vi id;
            rep(i,WELL_N){
                if(v[i]==0) continue;
                auto [s,l] = closest_rational2(
                    ideal[i],
                    max(2,state.get_well_front_cap(i)),
                    5e-6
                ); // 比の有理数近似を列挙
                small.push_back(s), large.push_back(l);
                id.push_back(i);
            }

            // 最適な操作を決定
            // 各iについて small or large のどちらを使うか全探索する
            double tmp_err = 1e100;
            State best_tmp;
            // とりあえず近い値にセット
            vi p1(id.size()),q1(id.size()),p2(id.size()),q2(id.size());
            rep(i,id.size()){
                double s_val = (double)small[i].X.X * small[i].Y.X / (double)small[i].X.Y / small[i].Y.Y;
                double l_val = (double)large[i].X.X * large[i].Y.X / (double)large[i].X.Y / large[i].Y.Y;
                if(abs(s_val-ideal[id[i]]) < abs(l_val-ideal[id[i]])){
                    tie(p1[i],q1[i]) = small[i].X;
                    tie(p2[i],q2[i]) = small[i].Y;
                }
                else{
                    tie(p1[i],q1[i]) = large[i].X;
                    tie(p2[i],q2[i]) = large[i].Y;
                }
            }
            while(tmp_err > 1e10){
                State tmp = state;
                
                // 1~7まで行う
                rep(i,id.size()){
                    // 1.2. q1に広げる
                    if(p1[i]!=0 && p1[i]!=inf){
                        tmp.shared_split(id[i],q1[i]-tmp.get_well_cap_sum(id[i]));
                    }
                    if(tmp.well[id[i]].size()>1) tmp.well_merge(id[i],0);
                    if(p1[i]!=0 && p1[i]!=inf){
                        // 3. p1分子を分離
                        tmp.well_split(id[i],0,p1[i]);
                        // 4. q1+q2-p1のスペースを共有スペースから確保する
                        tmp.shared_split(id[i],q1[i]+q2[i]-p1[i]-tmp.get_well_cap_sum(id[i]));
                        // 5. 3つのうち上2つをマージ
                        tmp.well_merge(id[i],1);
                        // 6. well[i][1]をスプリット
                        tmp.well_split(id[i],1,p2[i]);
                        // 7. 下2つをマージ
                        tmp.well_merge(id[i],0);
                    }
                }
                
                // 8. 共有スペースにマージ
                rep(i,id.size())if(p1[i]!=0) tmp.shared_merge(id[i]);

                // 混ぜた後の色を計算
                if(tmp.shared.v > 1.0 - 1e-6){ // 絵具が十分にある前提
                    Color give_color = tmp.give_color(tc);
                    // 誤差を計算
                    double err = diff(give_color,tc);
                    if(chmin(tmp_err, err)){
                        best_tmp = tmp; // 最適な状態を記録
                    }
                }

                // 見つけたら抜ける
                if(tmp_err < 1e10) break;

                // 近似比を更新 
                // 大きくした後の誤差が小さいものを選ぶ
                double mn_diff = 1e100;
                int best_i = -1;
                rep(i,id.size()){
                    if(pair(pii(p1[i],q1[i]),pii(p2[i],q2[i])) == large[i]) continue;
                    if(chmin(mn_diff,abs(ideal[id[i]] - (double)large[i].X.X * large[i].Y.X / (double)large[i].X.Y / large[i].Y.Y))){
                        best_i = i;
                    }
                }
                if(best_i == -1) break; // これ以上大きくできないなら抜ける

                tie(p1[best_i],q1[best_i]) = large[best_i].X;
                tie(p2[best_i],q2[best_i]) = large[best_i].Y;
            }

            if(tmp_err < 1e10){
                found = true;
            }

            // 最適比を復元
            if(chmin(best_diff,tmp_err)){
                nxt_state = best_tmp;
            }
        }
        sum_err += best_diff;
        state = nxt_state;
    }

    // 最後のターンはある分を開放
    rep(i,WELL_N){
        while(!state.well[i].empty()){
            state.shared_merge(i);
        }
    }
    rep(i,WELL_N){
        while(best_add_num[i]>0){
            state.add_color_to_shared(i);
            best_add_num[i]--;
        }
    }
    Color c = state.give_color(target.back());
    double final_diff = diff(c,target.back());
    sum_err += final_diff;

    return state;
}

Ops main(int NONZERO){
    Ops ops; // 操作の履歴
    ops.K_ERASE = false; // チューブIDを削らない
    K = io.true_K;
    tube = io.true_tube;
    construct_cells();
    
    // 最適比を計算
    int ver27_iter = clamp((T - 12000) / 6 - 10, 0, H);
    int ver29_iter = H - ver27_iter;
    calc_best_ratio(ver29_iter,NONZERO);
    
    int CELL_PER_PAINT = 400/(2*K); // 絵具1gが占めるセルの数
    int CELL_PER_WELL = CELL_PER_PAINT * 2; // ウェル一つ当たりのセル数
    
    // 初期状態の設定
    State state;
    rep(i,K) state.shared_split(i,CELL_PER_PAINT);
    // 何も操作していない状態に戻す
    state.t = 0;
    while(!state.ops.empty()) state.ops = state.ops.pop();

    // 壁の設定
    auto [v_wall,h_wall] = cells_to_wall();
    // 混ぜる場所を繋げる
    rep(i,K-1) v_wall[0][i] = 0;
    // 最初に壁を出しておく
    rep(i,K){
        auto [i1,j1] = cell[i][CELL_PER_PAINT-1];
        auto [i2,j2] = cell[i][CELL_PER_PAINT];
        if(i1==i2){
            v_wall[i1][min(j1,j2)] = 1;
        }
        else{
            h_wall[min(i1,i2)][j1] = 1;
        }
    }
    ops.init_wall(v_wall,h_wall);
    
    // ターンを進めながらクエリを処理
    // 最初の絵の具入れ
    rep(i,K) state.add_color(i,i);

    // ver29の内容
    rep(t,ver29_iter){
        const Color& tar = target[t];

        // 最良の比
        const auto& w = best_ratio[t];
        
        // 必要なら補充する
        rep(i,K){
            if(w[i] > state.get_well_front_cap(i)){
                // CELL_PER_PAINT分広げる
                state.shared_split(i, CELL_PER_PAINT);
                if(state.well[i].size() >= 2) state.well_merge(i,0);
                // 絵の具を補充
                state.add_color(i,i);
            }
        }
        // 中心に絵具を流す
        rep(i,K){
            if(w[i]>0){
                state.well_split(i,0,w[i]);
                state.shared_merge(i);
            }
        }
        // 絵具を渡す
        state.give_color(tar);
    }
    
    double init_time = elapsed();

    // ver27の内容
    vector<tuple<int,State,double>> pool_state;
    rep(t,ver29_iter,H){
        // 操作を計算
        vector<State> nxt_states = calc_best_process1(state,target[t],(t-ver29_iter+1)*2.5 + init_time);
        State nxt_state = nxt_states[0];
        double mx_score = 1e100;
        for(auto &e:nxt_states){
            if(e.t < (T-500)*(t+1)/H && chmin(mx_score,e.diff_sum)){
                nxt_state = e;
            }
        }

        if(state.add_count <= H && nxt_state.add_count > H-10){
            pool_state.emplace_back(t,state,1e100);
        }

        state = nxt_state; // 状態を更新
    }

    #ifndef ONLINE_JUDGE
    cerr << "Add Count: " << state.add_count << endl;
    cerr << "Elapsed Time: " << elapsed() << " ms" << endl;
    #endif

    // 途中からを再計算
    reverse(all(pool_state));
    int pool_size = pool_state.size();
    double pool_search_init_time = elapsed();
    while(!pool_state.empty() && elapsed() < 2900){
        rep(i,pool_state.size()){
            auto &[t,_state,score] = pool_state[i];
            vector<Color> remain_target = vector<Color>(target+t,target+H);
            State _nxt_state = calc_final_process(_state, remain_target,2900);
            if(_nxt_state.t < T && _nxt_state.get_score() < state.get_score()){
                state = _nxt_state;
            }
            score = _nxt_state.get_score();
            #ifndef ONLINE_JUDGE
            cerr << "t: " << t << ", score : " << _nxt_state.get_score() << ", Elapsed Time : " << elapsed() << "ms, T : " << _nxt_state.t << "\n";
            #endif
            if(elapsed() > 2900) break;
        }
        if(elapsed() > 2900) break;
        // スコアの高い順にソート
        sort(all(pool_state),[](const auto &a, const auto &b){
            return get<2>(a) < get<2>(b);
        });
        // スコアの低いものを排除
        // 時間とともに少しずつ減らす
        double progress = (double)(elapsed() - pool_search_init_time) / (double) (2900 - pool_search_init_time);
        pool_state.resize(max(1,(int)(pool_size*(1-progress))));
    }


    int score = state.get_score();
    #ifndef ONLINE_JUDGE
    cerr << "Score: " << score << "\n";
    cerr << "D = " << D << "\n";
    cerr << "V = " << state.add_count << "\n";
    cerr << "E = " << state.diff_sum << "\n";
    cerr << "average E = " << state.diff_sum / H << "\n";
    cerr << "average loss = " << state.diff_sum / H * 10000 << "\n";
    cerr << "iter_cost = " << D*(state.add_count - H) << "\n";
    cerr << "err_cost = " << (int)(state.diff_sum * 10000) << "\n";
    cerr << "Elapsed time: " << elapsed() << " ms\n";
    #endif

    // 操作を復元
    persistent_stack<Operation> tmp = state.ops;
    while(!tmp.empty()){
        ops.ops.push_back(tmp.top());
        tmp = tmp.pop();
    }
    reverse(all(ops.ops)); // 復元した操作を逆順にする

    K = io.used_id.size();
    tube.resize(K);
    rep(i,K) tube[i] = io.true_tube[io.used_id[i]];
    return ops;
}
} // namespace ver29対応

int main(){
    if(T>=34000){
        cpp_dump("ver23_max_eps_no_add");
        auto ops = ver23_max_eps_no_add::main();
        io.out(ops);
    }
    else if(T>=18000){
        cpp_dump("ver27");
        auto ops = ver27::main();
        io.out(ops);
    }
    else if(T>=11800){
        cpp_dump("ver29");
        auto ops = ver29::main(4);
        if(ops.ops.size()<=T){
            io.out(ops);
        }
        else{
            cpp_dump("ver25_donut_random");
            auto ops = ver25_donut_random::main(true);
            io.out(ops);
        }
    }
    else if(T>=10000){
        cpp_dump("ver29");
        auto ops = ver29::main(3);
        if(ops.ops.size()<=T){
            io.out(ops);
        }
        else{
            cpp_dump("ver25_donut_random");
            auto ops = ver25_donut_random::main(true);
            io.out(ops);
        }
    }
    else{
        cpp_dump("ver25_donut_random");
        auto ops = ver25_donut_random::main();
        io.out(ops);
    }
    cpp_dump(elapsed());
}