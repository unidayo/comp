#include <bits/stdc++.h>
using namespace std;
#define rep_(i, a_, b_, a, b, ...) for (int i = (a), lim##i = (b); i < lim##i; ++i)
#define rep(i, ...) rep_(i, __VA_ARGS__, __VA_ARGS__, 0, __VA_ARGS__)  // rep(i, a): [0, a); rep(i, a, b): [a, b)
#define drep_(i, a_, b_, a, b, ...) for (int i = (a) - 1, lim##i = (b); i >= lim##i; --i)
#define drep(i, ...) drep_(i, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, 0)  // drep(i, a): [0, a); drep(i, a, b): [b, a)
#define all(x) (x).begin(), (x).end()
#define rall(x) (x).rbegin(), (x).rend()
#ifdef LOCAL
void debug_out() {
    cerr << endl;
}
template <class Head, class... Tail> void debug_out(Head H, Tail... T) {
    cerr << ' ' << H;
    debug_out(T...);
}
#define debug(...) cerr << 'L' << __LINE__ << " [" << #__VA_ARGS__ << "]:", debug_out(__VA_ARGS__)
#define dump(x) cerr << 'L' << __LINE__ << " " << #x << " = " << (x) << endl
#else
#define debug(...) (void(0))
#define dump(x) (void(0))
#endif
template <class T> using V = vector<T>;
template <class T> using VV = V<V<T>>;
using ll = long long;
using ld = long double;
template <class T> using priority_queue_rev = priority_queue<T, vector<T>, greater<T>>;
template <class T> inline bool chmin(T &a, const T b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}
template <class T> inline bool chmax(T &a, const T b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}
template <class T> inline int count_between(const vector<T> &a, T l, T r) {
    return lower_bound(all(a), r) - lower_bound(all(a), l);
}  // [l, r)
template <class T1, class T2> istream &operator>>(istream &is, pair<T1, T2> &p) {
    is >> p.first >> p.second;
    return is;
}
template <class T1, class T2> ostream &operator<<(ostream &os, const pair<T1, T2> &p) {
    os << '(' << p.first << ", " << p.second << ')';
    return os;
}
template <class T> istream &operator>>(istream &is, vector<T> &v) {
    for (auto &e : v) is >> e;
    return is;
}
template <class T> ostream &operator<<(ostream &os, const vector<T> &v) {
    for (auto &e : v) os << e << ' ';
    return os;
}
inline int popcount(const ll x) {
    return __builtin_popcountll(x);
}
struct fast_ios {
    fast_ios() {
        cin.tie(nullptr);
        ios::sync_with_stdio(false);
        cout << fixed << setprecision(20);
    };
} fast_ios_;
constexpr int INF = numeric_limits<int>::max() >> 1;

#if DEBUG
constexpr float machine_spec = 0.5;
#elif LOCAL
constexpr float machine_spec = 1.4;
#else
constexpr float machine_spec = 1;
#endif
constexpr ll time_limit = (3000-50) * 1;
constexpr float time_limit_sec = time_limit / 1000.0; // ミリ秒から秒に変換

struct Timer{
    private:
        chrono::system_clock::time_point start = chrono::system_clock::now();
        chrono::system_clock::time_point lap = chrono::system_clock::now();
    public:
        // 実行開始時からの経過時間(ms)を返す
        int elapsed_ms(){
            auto now = chrono::system_clock::now();
            #if DEBUG or LOCAL
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() * machine_spec;
            #else
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            #endif
        }
        // ラップタイム(ms)を返す
        int lap_ms(){
            auto now = chrono::system_clock::now();
            #if DEBUG or LOCAL
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - lap).count() * machine_spec;
            #else
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - lap).count();
            #endif
        }

        // 精度の高いラップタイムを返す
        inline float lap_sec(){
            auto now = chrono::system_clock::now();
            #if DEBUG or LOCAL
                return std::chrono::duration_cast<std::chrono::duration<float>>(now - lap).count() * machine_spec;
            #else
                return std::chrono::duration_cast<std::chrono::duration<float>>(now - lap).count();
            #endif
        }
        
        // ラップを刻む
        void mark_lap(){
            lap = chrono::system_clock::now();
        }
} timer;

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
    inline result_type operator()() {
        x ^= x << 9;
        return x ^= x >> 7;
        // return (x * 0x123456789abcdef) >> 32;
    }
    inline int64_t operator()(int m) { return int64_t((*this)()) * m >> 32; }
    inline int64_t operator()(int l_include, int r_exlude) { return l_include + (*this)(r_exlude - l_include); }
    inline double rand() { return double((*this)()) / (int64_t(1) << 32); }
}rnd(998244353);

struct RandomPermutation {
    int cursor = 0; // 現在のカーソル位置
    int n;
    V<int> perm;

    RandomPermutation(int in_n) : n(in_n), perm(in_n) {
        iota(all(perm), 0);
    }
    void reset_cursor() {
        cursor = 0;
    }
    int next() {
        swap(perm[cursor], perm[rnd(cursor, n)]);
        return perm[cursor++];
    }
};

struct Color{
    float cmy[3]; // 0:cyan, 1:magenta, 2:yellow
    Color(float c, float m, float y) {
        cmy[0] = c;
        cmy[1] = m;
        cmy[2] = y;
    }
    Color() {
        cmy[0] = cmy[1] = cmy[2] = 0;
    }

    float distance2(const Color &other) const {
        float dc = cmy[0] - other.cmy[0];
        float dm = cmy[1] - other.cmy[1];
        float dy = cmy[2] - other.cmy[2];
        return (dc * dc + dm * dm + dy * dy);
    }

    inline float distance(const Color &other) const {
        return sqrt(distance2(other));
    }

    float fast_distance(const Color &other) const {
        // 近似距離計算
        float dc = cmy[0] - other.cmy[0];
        float dm = cmy[1] - other.cmy[1];
        float dy = cmy[2] - other.cmy[2];
        return abs(dc) + abs(dm) + abs(dy); // 単純な差の和を返す
    }
};

struct ColorPortion{
    Color scaled_color; // CMYの総量(= 実際のColor * amount)を表す色量
    float amount; // 量
    ColorPortion(const Color &color, float amount) : scaled_color(color), amount(amount) {}
    ColorPortion() : scaled_color(Color()), amount(0) {}
    
    void operator+=(const ColorPortion &other) {
        scaled_color.cmy[0] += other.scaled_color.cmy[0];
        scaled_color.cmy[1] += other.scaled_color.cmy[1];
        scaled_color.cmy[2] += other.scaled_color.cmy[2];
        amount += other.amount;
    }

    void operator -=(const ColorPortion &other) {
        scaled_color.cmy[0] -= other.scaled_color.cmy[0];
        scaled_color.cmy[1] -= other.scaled_color.cmy[1];
        scaled_color.cmy[2] -= other.scaled_color.cmy[2];
        amount -= other.amount;
    }

    void operator*=(float factor) {
        scaled_color.cmy[0] *= factor;
        scaled_color.cmy[1] *= factor;
        scaled_color.cmy[2] *= factor;
        amount *= factor;
    }

    ColorPortion operator*(float factor) const {
        ColorPortion result = *this;
        result *= factor;
        return result;
    }

    inline static ColorPortion mix(const ColorPortion &cp0, const ColorPortion &cp1) {
        auto color = Color(
            cp0.scaled_color.cmy[0] + cp1.scaled_color.cmy[0],
            cp0.scaled_color.cmy[1] + cp1.scaled_color.cmy[1],
            cp0.scaled_color.cmy[2] + cp1.scaled_color.cmy[2]
        );
        float amount = cp0.amount + cp1.amount;
        return ColorPortion(color, amount);
    }
    // 色量を量で割って、実際の色を返す
    inline Color get_color() const {
        float inv_amount = 1.0 / amount;
        return Color(
            scaled_color.cmy[0] * inv_amount,
            scaled_color.cmy[1] * inv_amount,
            scaled_color.cmy[2] * inv_amount
        );
    }
};

constexpr int ADD_TUBE = 1; // チューブの絵の具を追加
constexpr int SUBMIT = 2; // 絵の具を提出
constexpr int REMOVE = 3; // 絵の具を削除
constexpr int PARTITION = 4; // パーティションの生成・削除

constexpr int N = 20; // パレットのサイズ
constexpr int NN = N * N; // パレットのマスの数
int K; // チューブの数。最大20
constexpr int H = 1000; // 作成すべき色の数
constexpr float Inv_H = 1.0 / H;
int T; // 最大ターン数。4000 <= T <= 64000
int D; // 1グラム追加するのにかかるコスト。10 <= D <= 10000
ColorPortion tubes[20]; // チューブの色
Color target_colors[H]; // 作成すべき色

float frac[40][40]; // frac[i][j] = i/j の値。i,jは0~19の整数
float log_rand_table[1000];
#ifdef LOCAL
int total_annealing_try_count = 0; // SAの試行回数
#endif

void input() {
    int _; cin >> _ >> K >> _ >> T >> D;
    rep(i, 0, K) {
        float c, m, y; cin >> c >> m >> y;
        tubes[i] = ColorPortion(Color(c, m, y), 1); // 初期量は0g
    }
    rep(i, 0, H) {
        float c, m, y; cin >> c >> m >> y;
        target_colors[i] = Color(c, m, y);
    }
}

void pre_compute() {
    // 0~20の分数を計算
    rep(i, 0, 40) {
        rep(j, 1, 40) {
            frac[i][j] = float(i) / j;
        }
    }
    rep(i, 0, 1000) {
        log_rand_table[i] = log(rnd.rand());
    }
}


struct PalletAction{
    int tankId; // 対象のタンクのID
    int addTubeIdx; // 追加するチューブのインデックス
    int nextPartitionPos; // 次のパーティションの位置。-1なら移動無し

    PalletAction(int tankId, const int &addTubeIdx, int nextPartitionPos)
        : tankId(tankId), addTubeIdx(addTubeIdx), nextPartitionPos(nextPartitionPos) {}
};


/**
 * 20個のタンクと、1つの混ぜ合わせ場を持つパレット
 * タンク：幅Dマス、高さ1マス。混ぜ合わせ場所と隣接する仕切りの位置を変化させることで、タンクから混ぜ合わせ場所に絵の具を移動できる。
 * 混ぜ合わせ場所：幅20-Dマス、高さ20マス。タンクから絵の具を移動させて、絵の具を混ぜ合わせることができる。
 */
struct MultiTankPallet{
    int TankNum; // パレットのサイズ
    int MaxPartition; // タンクの仕切り位置の最大値
    ColorPortion tank_cp[40]; // タンクiの色と量
    int partition[40]; // タンクiの仕切り位置. 0が空っぽ、MaxPartitionが満タンを表す
    
    ColorPortion mixed_cp; // 混ぜ合わせ場所の色と量. 擬似的にTankId = TankNumとして扱う
    // TODO: 混ぜ合わせ場所の端にも仕切りを持たせるかも。色の調整がしやすくなる。
    int total_rest_color_amount = 0; // 全体に残っている色の量

    int partition_reset_threshold = 10; // 仕切りを戻す閾値。これ以下の量になったら仕切りを戻す
    pair<int, int> cell_poss[40][40]; // cell_poss[i][j] = タンクiの仕切り位置jの混ぜ合わせ側のマスの位置

    void init(int tank_num) {
        TankNum = tank_num;
        MaxPartition = tank_num == 40
        ? 9
        : 19 * N / TankNum;
        
    
        // タンクの初期化
        rep(i, 0, TankNum) {
            tank_cp[i] = ColorPortion(Color(), 0); // 初期は空っぽ
            partition[i] = rnd(MaxPartition-2, MaxPartition+1);
        }
        mixed_cp = ColorPortion(Color(), 0); // 混ぜ合わせ場所も初期は空っぽ
        init_cell_poss();
    }
    
    // returns: 操作回数
    int apply(const V<PalletAction> &actions, const int submit_tank_id, bool is_last) {
        int operation_count = 0; // 操作回数
        V<int> empty_partition_idxs; // 空になったパーティションのインデックス

        for (const auto &action : actions) {
            // タンクの色と量を更新
            if(action.tankId < TankNum) {
                auto &cp = tank_cp[action.tankId];
                if(action.addTubeIdx >= 0) {
                    // チューブの絵の具を追加
                    cp += tubes[action.addTubeIdx]; // チューブの絵の具を追加
                    total_rest_color_amount++;
                    operation_count++;
                }
                if(action.nextPartitionPos >= 0) {
                    // tankと混ぜ合わせ場のColorPortionを更新
                    auto rate = frac[action.nextPartitionPos][partition[action.tankId]];
                    mixed_cp += cp * (1.0 - rate); // 混ぜ合わせ場に移動する量を更新
                    cp *= rate; // タンクの量を更新
                    partition[action.tankId] = action.nextPartitionPos; // パーティション位置を更新
                    if(partition[action.tankId] == 0) {
                        empty_partition_idxs.push_back(action.tankId); // 空になったパーティションを記録
                    }
                    operation_count += action.nextPartitionPos > 0 ? 2 : 1;
                }
            }else{
                // 混ぜ合わせ場所に直接絵の具を追加
                if(action.addTubeIdx >= 0){
                    mixed_cp += tubes[action.addTubeIdx]; // 混ぜ合わせ場にチューブの絵の具を追加
                    operation_count++;
                    total_rest_color_amount++;
                }
                // NOTE: 混ぜ合わせ場のパーティション位置更新は未対応
            }
            
        }
        // 提出アクションを適用
        if(submit_tank_id >= 0){
            auto &cp = submit_tank_id < TankNum ? tank_cp[submit_tank_id] : mixed_cp;
            cp *= (cp.amount - 1.0) / cp.amount; // 提出するので量を1g減らす
            operation_count++;
            total_rest_color_amount--;
        }

        if(!is_last){
            // 空のpartitionをMaxPartitionに戻す
            int mix_well_size = NN; // 混ぜ合わせ場のサイズ
            for(auto p : partition) mix_well_size -= p;
            for (auto idx : empty_partition_idxs) {
                // 混ぜ合わせ場の絵の具を MaxPartition : (mix_well_size - MaxPartition) の比率で分配
                auto cp = mixed_cp * ((float)MaxPartition / mix_well_size);
                tank_cp[idx] += cp; // タンクの量を更新
                mixed_cp -= cp; // 混ぜ合わせ場の量を減らす
                mix_well_size -= MaxPartition; // 混ぜ合わせ場のサイズを更新
                partition[idx] = MaxPartition; // 空になったパーティションをMaxPartitionに戻す
                operation_count++;
            }
    
            // FIXME: 仕切りを頻繁に戻すバージョン
            for (auto action : actions) {
                int idx = action.tankId;
                if(idx == TankNum) continue; // 混ぜ合わせ場は無視
                if(partition[idx] < partition_reset_threshold) {
                    // 仕切りをMaxPartitionに戻す
                    auto add_cp = mixed_cp * ((float)(MaxPartition - partition[idx]) / mix_well_size);
                    tank_cp[idx] += add_cp; // タンクの量を更新
                    mixed_cp -= add_cp; // 混ぜ合わせ場の量を減らす
                    mix_well_size -= (MaxPartition - partition[idx]); // 混ぜ合わせ場のサイズを更新
                    partition[idx] = MaxPartition;
                    operation_count+=2;
                }
            }
        }
        return operation_count;
    }
    
    int output_and_apply(const V<PalletAction> &actions, const int submit_tank_id, bool is_last) {
        int tmp_partition[40];
        std::copy(std::begin(partition), std::end(partition), std::begin(tmp_partition));
        for (const auto &action : actions) {
            // チューブの絵の具を追加
            if(action.addTubeIdx >= 0) {
                auto [i, j] = tankId_to_pos(action.tankId);
                cout << ADD_TUBE << " " << i << " " << j << " " << action.addTubeIdx << endl;
            }
            // パーティションの生成・削除
            if(action.nextPartitionPos >= 0) {
                assert(tmp_partition[action.tankId] > 0);
                // NOTE: 混ぜ合わせばは未対応
                // 新しい位置にpartitionを置いて、今の位置を削除
                // auto [i, _] = tankId_to_pos(action.tankId);
                // if(action.nextPartitionPos > 0) cout << V<int>{PARTITION, i, action.nextPartitionPos - 1, i, action.nextPartitionPos} << endl;
                // cout << V<int>{PARTITION, i, tmp_partition[action.tankId] - 1, i , tmp_partition[action.tankId]} << endl;

                if(action.nextPartitionPos > 0){
                    auto [from_x, from_y] = cell_poss[action.tankId][action.nextPartitionPos - 1];
                    auto [to_x, to_y] = cell_poss[action.tankId][action.nextPartitionPos];
                    cout << V<int>{PARTITION, from_x, from_y, to_x, to_y} << endl; // 新しいパーティションを置く
                }
                {
                    auto [from_x, from_y] = cell_poss[action.tankId][tmp_partition[action.tankId] - 1];
                    auto [to_x, to_y] = cell_poss[action.tankId][tmp_partition[action.tankId]];
                    cout << V<int>{PARTITION, from_x, from_y, to_x, to_y} << endl; // 今のパーティションを削除
                }
                tmp_partition[action.tankId] = action.nextPartitionPos;
            }
        }
        // 最後に提出アクションを出力
        if(submit_tank_id >= 0){
            auto [submit_i, submit_j] = tankId_to_pos(submit_tank_id);
            cout << SUBMIT << " " << submit_i << " " << submit_j << endl;
        }

        
        if(!is_last){
            // 仕切りを頻繁に戻すバージョン
            for (const auto &action : actions) {
                if (action.tankId == TankNum) continue; // 混ぜ合わせ場は無視
                if (tmp_partition[action.tankId] < partition_reset_threshold) {
                    // 仕切りをMaxPartitionに戻す
                    // cout << V<int>{PARTITION, i, MaxPartition-1, i, MaxPartition} << endl; // 仕切りを0に戻す
                    {
                        auto [from_x, from_y] = cell_poss[action.tankId][MaxPartition-1];
                        auto [to_x, to_y] = cell_poss[action.tankId][MaxPartition];
                        cout << V<int>{PARTITION, from_x, from_y, to_x, to_y} << endl; // 仕切りをMaxPartitionに戻す
                    }
                    
                    // cout << V<int>{PARTITION, i, tmp_partition[action.tankId] - 1, i, tmp_partition[action.tankId]} << endl;
                    if(tmp_partition[action.tankId] > 0){
                        auto [from_x, from_y] = cell_poss[action.tankId][tmp_partition[action.tankId]-1];
                        auto [to_x, to_y] = cell_poss[action.tankId][tmp_partition[action.tankId]];
                        cout << V<int>{PARTITION, from_x, from_y, to_x, to_y} << endl; // 今の仕切りを消す
                    }
                    tmp_partition[action.tankId] = MaxPartition; // 仕切り位置を更新
                }
            }
        }

        // 実際にパレットに適用
        return apply(actions, submit_tank_id, is_last);
    }

    pair<int, int> tank_lower_leftern(int tankId) const {
        // タンクの左下のセルの位置を返す
        int height = N / TankNum; // タンクの高さ
        int x = height * tankId + height - 1; // タンクの左下のセルのx座標
        return {x, 0}; // タンクの左下のセルのy座標は常に0
    }

    private:
        void init_cell_poss_tank40(){
            rep(tankId,0,TankNum){
                int width = MaxPartition;
                
                int x = tankId % N;
                int y = tankId < N ? width : N - width-1;
                int dy = tankId < N ? -1 : 1;
                drep(i, MaxPartition+1, 0){
                    cell_poss[tankId][i] = {x, y};
                    y += dy;
                }
            }
        }
        // タンクの位置を計算するヘルパー関数
        void init_cell_poss(){
            if(TankNum == 40) {
                init_cell_poss_tank40();
                return;
            }

            rep(tankId, 0, TankNum) {
                int height = N / TankNum; // タンクの高さ
                int width = MaxPartition / height; // タンクの幅
                
                int x = height * tankId;
                int y = width;
                int dy = -1;
                drep(i, MaxPartition+1, 0){
                    cell_poss[tankId][i] = {x, y};
                    if(y + dy < 0 || y + dy > width){
                        x++;
                        dy *= -1;
                    }else{
                        y += dy;
                    }
                }
            }
            cerr << "cell_poss initialized" << endl;
        }

        pair<int,int> tankId_to_pos(int tankId) {
            if(tankId == TankNum){
                if(TankNum < 40) return {0, 19};
                else return {0, 10};
            }
            return cell_poss[tankId][0]; // タンクの最初の位置を返す
        }
};

// パレットの初期状態を出力形式に変換するデコーダ
struct InitPalletDecoder{
    static void output_tank40(const MultiTankPallet &pallet) {
        bool horizontal[N][N-1] = {}; // 横の仕切り（線の向き = ｜）
        bool vertical[N-1][N] = {}; // 縦の仕切り（線の向き = ー）
        
        int height = 1;
        int width = pallet.MaxPartition / height;
        rep(i,0,N-1){
            rep(j,0,pallet.MaxPartition) vertical[i][j] = true; // 全ての縦の仕切りを初期化
            rep(j,N-pallet.MaxPartition,N) vertical[i][j] = true;

        }

        // 初期のパーティションを反映
        rep(i,0,pallet.TankNum) {
            int partition_pos = pallet.partition[i];
            if(partition_pos > 0) {
                // タンクの仕切り位置を反映
                auto [cell_x, cell_y] = pallet.cell_poss[i][partition_pos];
                if(i < N) horizontal[cell_x][cell_y-1] = true; // 横の仕切り
                else horizontal[cell_x][cell_y] = true; // 横の仕切り
            }
        }

        // 最初に各タンクに混ぜながら流すので、左端の1列は全部開ける(vertical)
        rep(i,0,N-1) if(i != N/2-1) vertical[i][0] = false, vertical[i][N-1] = false;
        

        // 出力
        rep(i,0,N){
            rep(j,0,N-1) cout << (horizontal[i][j] ? 1 : 0) << " \n"[j == N-2];
        }
        rep(i,0,N-1){
            rep(j,0,N) cout << (vertical[i][j] ? 1 : 0) << " \n"[j == N-1];
        }
    }

    static void output(const MultiTankPallet &pallet) {
        if(pallet.TankNum == 40) {
            output_tank40(pallet);
            return;
        }

        bool horizontal[N][N-1] = {}; // 横の仕切り（線の向き = ｜）
        bool vertical[N-1][N] = {}; // 縦の仕切り（線の向き = ー）

        int height = N / pallet.TankNum;
        int width = pallet.MaxPartition / height;
        rep(t,0,pallet.TankNum) {
            int height_offset = N / pallet.TankNum * t;
            rep(i,0,height){
                if(i != 0) horizontal[height_offset + i][width-1] = true; // タンクの下端の仕切り
                int l = i % 2 ? 0 : 1;
                int r = i % 2 ? width - 1 : width;
                if(i == height - 1) l = 0, r = width;
                rep(j,l,r){
                    if(height_offset + i < N-1) vertical[height_offset + i][j] = true; // タンクの横の仕切り
                }
            }
        }

        // 初期のパーティションを反映
        rep(i,0,pallet.TankNum) {
            int partition_pos = pallet.partition[i];
            if(partition_pos > 0) {
                // タンクの仕切り位置を反映
                auto [cell_x, cell_y] = pallet.cell_poss[i][partition_pos];
                horizontal[cell_x][cell_y-1] = true; // 横の仕切り
            }
        }

        // 混ぜ合わせ場所は折り返している
        if(pallet.MaxPartition < N-1) rep(i,1,N) horizontal[i][N-2] = true; // 右端の仕切り

        // 最初に各タンクに混ぜながら流すので、左端の1列は全部開ける(vertical)
        rep(i,0,N-1) if(i != N/2-1) vertical[i][0] = false;

        // 出力
        rep(i,0,N){
            rep(j,0,N-1) cout << (horizontal[i][j] ? 1 : 0) << " \n"[j == N-2];
        }
        rep(i,0,N-1){
            rep(j,0,N) cout << (vertical[i][j] ? 1 : 0) << " \n"[j == N-1];
        }
    }
};

struct State{
    MultiTankPallet pallet; // パレットの状態
    int next_h = 0; // 次のtargetColorsのインデックス
    int op_cnt = 0; // 操作回数
    float total_color_errors = 0; // 色の誤差の合計

    void output_and_apply(const V<PalletAction> &actions, int submit_tank_id, float color_error = 0) {
        int prev_op_cnt = op_cnt; // 前の操作回数を保存
        bool is_last = next_h == H - 1; // 最後のターンかどうか
        op_cnt += pallet.output_and_apply(actions, submit_tank_id, is_last);
        total_color_errors += color_error; // 色の誤差を加算
        // if(submit_tank_id >= 0 && color_error > 10)debug(next_h, (int)color_error, op_cnt-prev_op_cnt);
        next_h += submit_tank_id >= 0;
    }
};

struct Cost{
    float absolute_cost = 0;
    float progress_dependent_cost = 0; // SA進捗に依存するコスト

    inline float total(float progress = 1.0) const {
        // SAの進捗に応じてコストを調整
        return absolute_cost + progress_dependent_cost * progress;
    }
};

// Actionを適用した際の差分。差分更新用
struct ActionDiff{
    ColorPortion mixed_cp; // 混ぜ合わせ場の色と量
    int op_cnt = 0; // 操作回数
    int pallet_rest_amount = 0; // パレット全体の残り色量
};

struct Simulator{
    float over_turn_penalties[600];
    int pallet_limit_threshold; // パレット全体の残り色量の閾値。これを超えるとコストがかかる
    int aim_op_cnt[H];
    float over_pallet_limit_penalty = 100; // パレット全体の残り色量が閾値を超えた時のペナルティ

    int total_op_upper_limit; // 今のhにおける、操作回数上限。これを超えたらcost計算で即時バカデカコストを返す
    float max_rest_amount;
    float aim_rest_amount;
    float max_rest_amount_penalty;
    float aim_rest_amount_penalty;

    float aim_mixed_cp_upper = 1.3;


    Simulator(){};

    void init(int T, int start_op_cnt, bool tank40 = false){
        if(tank40){
            rep(i,0,H){
                // aim_op_cnt[i] = start_op_cnt + (T - start_op_cnt - 25) * (i+1) * Inv_H; // 目標の操作回数
                aim_op_cnt[i] = 20 + (T - 200) * (i+1) * Inv_H; // 目標の操作回数
            }
            for (int i = 0; i < 600; ++i) {
                over_turn_penalties[i] = (pow(2.0, (i-300) / 30.0) - 1) * 100; // 300を基準に指数関数的に増加
            }
            return;
        }
        rep(i,0,H){
            aim_op_cnt[i] = start_op_cnt + (T - start_op_cnt - 25) * (i+1) * Inv_H; // 目標の操作回数
        }
        
    }

    // __attribute__((noinline))
    void set_op_cnt_penalty(int h, bool tank40 = false){
        if(h == H-1){
            for (int i = 0; i < 600; ++i) {
                over_turn_penalties[i] = 0;
            }
            return;
        }
        float progress = float(h+1) / H; // SAの進捗
        if(tank40){
            // float frac = 30 - 26 * progress;
            // for (int i = 0; i < 600; ++i) {
            //     over_turn_penalties[i] = (pow(2.0, (i-300) / frac) - 1) * 100; // 300を基準に指数関数的に増加
            // }
        }else{
            float frac = 10 - 6 * progress;
            // float frac = 4;
    
            for (int i = 0; i < 600; ++i) {
                // over_turn_penalties[i] = (pow(2.0, (i-300) / 30.0) - 1) * 100; // 300を基準に指数関数的に増加
                over_turn_penalties[i] = (pow(2.0, (i-300) / frac) - 1) * 100; // 300を基準に指数関数的に増加
            }
        }

    }

    void set_calc_cost_variables(int h){
        int rest_h = H - h - 1; // 残りのターン数
        total_op_upper_limit = T - 2.1 * rest_h; // 残りのターン数から計算される操作回数上限
        max_rest_amount = rest_h; // 残りのターン数を最大残量とする
        aim_rest_amount = rest_h * 0.5; // ちょっと余裕を持って減らしておく
        max_rest_amount_penalty = D; // 最大残量を超えた時のペナルティ
        aim_rest_amount_penalty = D * 0.2; // 目標残量を超えた時のペナルティ
    }

    Cost calc_cost(const Color &color, const Color &target_color, int total_op_cnt, int h, int pallet_rest_amount, float mixed_amount) {
        if(total_op_cnt > total_op_upper_limit) return {10000000, 0}; // 操作回数が上限を超えている場合は大きなペナルティを課す

        // 色の誤差
        float color_error = color.distance(target_color) * 10000; // 2乗誤差も計算しておく
        
        // 操作回数が平均的な手数よりも大きく進捗している時にペナルティを課す
        int diff_op = max(-200, total_op_cnt - aim_op_cnt[h]);
        float over_turn_penalty = over_turn_penalties[diff_op + 300];
        
        
        // パレット全体の絵の具残量の最適化
        float pallet_rest_amount_cost = 0;
        pallet_rest_amount_cost = max<float>(0, pallet_rest_amount - aim_rest_amount) * aim_rest_amount_penalty; // 目標残量を超えた時のペナルティ
        if(pallet_rest_amount > max_rest_amount){
            pallet_rest_amount_cost = (pallet_rest_amount - max_rest_amount) * max_rest_amount_penalty; // 最大残量を超えた時のペナルティ
        }

        // そもそもパレットに残っている色の量が多すぎると、絵の具追加がしづらくなるのでペナルティを与える
        int over_pallet_limit = max(0, pallet_rest_amount - pallet_limit_threshold);
        // pallet_rest_amount_cost += over_pallet_limit * over_pallet_limit * over_pallet_limit * 100; // 40gを超えるとコストがかかる
        pallet_rest_amount_cost += over_pallet_limit * over_pallet_limit_penalty;
        
        // 混ぜ合わせ場の量の最適化
        float exceed_mixed_amount = max<float>(0.0, mixed_amount - aim_mixed_cp_upper); // 混ぜ合わせ場の量が0.3gを超えるとコストがかかる
        float mixed_amount_cost = exceed_mixed_amount * 10000; // 混ぜ合わせ場の量が多いほどコストがかかる
        // 混ぜ合わせが提出量に満たない場合は咎める(SA中は進捗に応じて係数調整)
        if (mixed_amount < 1.0) {
            mixed_amount_cost += 5000 + (1 - mixed_amount) * 5000;
        }

        return {color_error + over_turn_penalty + pallet_rest_amount_cost, mixed_amount_cost};
    }

    // actionのdryrun: (Color, mixed_amount, op_cnt, pallet_rest_amount)を返す
    static tuple<Color, float, int, int> dryrun_action(const V<PalletAction> &actions, int submit_tank_id, const MultiTankPallet &pallet) {
        ColorPortion mixed_cp = pallet.mixed_cp; // 混ぜ合わせ場の色と量
        int op_cnt = 0; // 操作回数
        int total_rest_color_amount = pallet.total_rest_color_amount; // 全体に残っている色の量
        for (const auto &action : actions) {
            auto cp = pallet.tank_cp[action.tankId];
            if(action.addTubeIdx >= 0) {
                cp += tubes[action.addTubeIdx]; // チューブの絵の具を追加
                total_rest_color_amount++;
                op_cnt++;
            }
            // 混ぜ合わせ場に移動する量を更新
            auto rate = frac[action.nextPartitionPos][pallet.partition[action.tankId]];
            mixed_cp += cp * (1.0 - rate); // 混ぜ合わせ場に移動する量を更新
            op_cnt += 2;
        }
        // 提出アクションを適用
        Color submit_color = mixed_cp.get_color();
        float mixed_amount = mixed_cp.amount - 1.0; // 提出するので量を1g減らす
        op_cnt++;
        total_rest_color_amount--;
        // 仕切りリセットの処理
        for (const auto &action : actions) {
            if (action.tankId == pallet.TankNum) continue; // 混ぜ合わせ場は無視
            if (action.nextPartitionPos < pallet.partition_reset_threshold) {
                // 仕切りをMaxPartitionに戻す操作回数
                op_cnt += 2;
            }
        }

        return make_tuple(submit_color, mixed_amount, op_cnt, total_rest_color_amount);
    }

    // actionを行った時に変化する {colorPortion, 操作回数, パレット全体の残り色量}を返す
    static tuple<ColorPortion, int, int> action_statistic(const PalletAction &action, const MultiTankPallet &pallet) {
        ColorPortion cp = pallet.tank_cp[action.tankId]; // タンクの色と量
        if(action.addTubeIdx >= 0) {
            cp += tubes[action.addTubeIdx]; // チューブの絵の具を追加
        }
        // 混ぜ合わせ場に移動する量を更新
        auto rate = frac[action.nextPartitionPos][pallet.partition[action.tankId]];
        ColorPortion mixed_cp = cp * (1.0 - rate); // 混ぜ合わせ場に移動する量を更新
        int op_cnt = 2; // パーティションの生成・削除は2回操作が必要. ただし、0の時は消すだけで、のちのリセットで1回使う。
        if(action.nextPartitionPos > 0 && action.nextPartitionPos < pallet.partition_reset_threshold) {
            // 仕切りをMaxPartitionに戻す操作回数
            op_cnt += 2;
        }
        return make_tuple(mixed_cp, op_cnt, action.addTubeIdx >= 0 ? 1 : 0);
    }

    static void add_diff(ActionDiff &action_diff, const PalletAction &action, MultiTankPallet &pallet) {
        auto [mixed_cp, action_op_cnt, rest_color_amount] = action_statistic(action, pallet);
        action_diff.mixed_cp += mixed_cp; // 混ぜ合わせ場の色と量を更新
        action_diff.op_cnt += action_op_cnt; // 操作回数を更新
        action_diff.pallet_rest_amount += rest_color_amount; // パレット全体の残り色量を更新
    }

    static void sub_diff(ActionDiff &action_diff, const PalletAction &action, MultiTankPallet &pallet) {
        auto [mixed_cp, action_op_cnt, rest_color_amount] = action_statistic(action, pallet);
        action_diff.mixed_cp -= mixed_cp; // 混ぜ合わせ場の色と量を更新
        action_diff.op_cnt -= action_op_cnt; // 操作回数を更新
        action_diff.pallet_rest_amount -= rest_color_amount; // パレット全体の残り色量を更新
    }

} simulator;

float max_add_tube_pallet_rate = 0.2;
struct ActionSA{
    struct ActionState{
        V<PalletAction> actions; // 現在のアクション
        Cost cost;
        V<bool> tank_used; // 各タンクがActionに使用されているか
        
        ActionDiff total_action_diff; // 操作全部を適用した時の差分
    };
    
    struct StaticData{
        State state; // 本ゲームの状態であって、焼きなまし対象のStateではない
        MultiTankPallet pallet;
        float sa_progress = 0; // 焼きなましの進捗
    };
    StaticData data;

    ActionSA(const State &state) : data({state, state.pallet}) {}

    void set_state(const State &state) {
        data.state = state;
        data.pallet = state.pallet;
        data.sa_progress = 0.0f; // 初期化
    }
    
    struct Transition{
        int types[2000];
        int type_size = 0;
        
        long long try_cnt[100];
        long long accept_cnt[100];
        const int TransitionTypeSize = 5;
        int CHANGE_ACTION_ADD_TUBE_RATE; // change_actionでチューブを追加する確率

        int decide_change_action_add_tube_rate(){
            // float tlog = log(min(30000,T)/4000.0) - log(2);
            // float tlog_max = log(30000/4000.0) - log(2);
            // float p = tlog / tlog_max;
            // p = sqrt(p);
            // if(T > 8000) return pow(5, p) * pow(100, 1 - p); // 10^p * 100^(1-p) の形で変化
            // else return 100;
            if(T < 7500) return 100;
            if(T < 10000) return 30 + 70 * (10000 - T) / 2500;
            if(T < 20000) return 15 + 15 * (20000 - T) / 10000;
            else return 15; // 20000ターン以上は15%でチューブを追加
        }

        Transition(){
            memset(try_cnt, 0, sizeof(try_cnt));
            memset(accept_cnt, 0, sizeof(accept_cnt));
            rep(i,0,10) types[type_size++] = 0;
            rep(i,0,10) types[type_size++] = 1;
            rep(i,0,100) types[type_size++] = 2; // change_action
            rep(i,0,100) types[type_size++] = 3; // slide_partition
            rep(i,0,0) types[type_size++] = 4; // change_tube

            CHANGE_ACTION_ADD_TUBE_RATE = decide_change_action_add_tube_rate();
        }

        bool execute(ActionState& state, StaticData& data, float current_temp){
            #ifdef LOCAL
            total_annealing_try_count++;
            #endif

            int type = types[rnd(type_size)];
            bool res;
            // check_action_state(state, data); // アクション状態をチェック
            switch(type){
                case 0: res = add_action(state, data, current_temp); break;
                case 1: res = delete_action(state, data, current_temp); break;
                case 2: res = change_action(state, data, current_temp); break;
                case 3: res = slide_partition(state, data, current_temp); break;
                case 4: res = change_tube(state, data, current_temp); break;
            }
            // check_action_state(state, data); // アクション状態をチェック
            try_cnt[type]++;
            if(res) accept_cnt[type]++;
            return res;
        }

        inline bool should_accept_transition(float cost_diff, float current_temp){
            return cost_diff < 0 || log_rand_table[rnd(1000)] * current_temp < -cost_diff;
        }

        void check_action_state(const ActionState& state, const StaticData& data) {
            V<bool> used_tanks(data.pallet.TankNum, false);
            for (const auto &action : state.actions) {
                assert(action.tankId >= 0 && action.tankId < data.pallet.TankNum);
                assert(!used_tanks[action.tankId]); // 同じタンクが複数回使用されていないことを確認
                used_tanks[action.tankId] = true; // タンクを使用済みにする
            }
        }

        // 1頂点をランダムに選び、その座標をランダムに変更する
        bool add_action(ActionState& state, StaticData& data, float current_temp){
            // タンクをランダムに選ぶ
            int tank_id = rnd(data.pallet.TankNum);
            if(state.tank_used[tank_id]) return false; // 既に使用されているタンクは無視

            // チューブをランダムに選ぶ
            int addTubeIdx = -1; 
            if(rnd(100) < 20 && data.pallet.tank_cp[tank_id].amount + 1 <= data.pallet.partition[tank_id] * max_add_tube_pallet_rate) {
                addTubeIdx = rnd(K);
            }

            // パーティション位置をランダムに選ぶ
            // int next_partition_pos = rnd(data.pallet.partition[tank_id]);
            int next_partition_pos = data.pallet.partition[tank_id] - 1;
            // if(T < 10000) next_partition_pos = rnd(data.pallet.partition[tank_id]); // Tが小さい時は1行動が大事なので、1マス分のAddでは元が取れないことが多いため

            // アクションを追加
            PalletAction action(tank_id, addTubeIdx, next_partition_pos);
            state.actions.push_back(action);
            state.tank_used[tank_id] = true;
            ActionDiff action_diff = state.total_action_diff;
            simulator.add_diff(action_diff, action, data.pallet);

            // コストを計算
            // auto [color, mixed_amount, op_cnt, pallet_rest_amount] = simulator.dryrun_action(state.actions, -1, data.pallet);
            auto color = action_diff.mixed_cp.get_color();
            auto mixed_amount = action_diff.mixed_cp.amount;
            auto op_cnt = action_diff.op_cnt;
            auto pallet_rest_amount = action_diff.pallet_rest_amount + data.pallet.total_rest_color_amount;
            auto new_cost = simulator.calc_cost(color, target_colors[data.state.next_h], data.state.op_cnt + op_cnt, data.state.next_h, pallet_rest_amount, mixed_amount);
            // コスト差を計算
            float cost_diff = new_cost.total(data.sa_progress) - state.cost.total(data.sa_progress);
            if(should_accept_transition(cost_diff, current_temp)){
                state.cost = new_cost;
                state.total_action_diff = action_diff; // 差分を更新
                return true;
            }else{
                // アクションを元に戻す
                state.actions.pop_back();
                state.tank_used[tank_id] = false;
                return false;
            }
        }

        bool delete_action(ActionState& state, StaticData& data, float current_temp){
            if(state.actions.empty()) return false; // アクションがない場合は無視

            // ランダムにアクションを選ぶ
            int action_idx = rnd(state.actions.size());
            auto action = state.actions[action_idx];

            // アクションを削除
            state.tank_used[action.tankId] = false;
            state.actions.erase(state.actions.begin() + action_idx);
            ActionDiff action_diff = state.total_action_diff;
            simulator.sub_diff(action_diff, action, data.pallet);
            
            // コストを計算
            // auto [color, mixed_amount, op_cnt, pallet_rest_amount] = simulator.dryrun_action(state.actions, -1, data.pallet);
            auto color = action_diff.mixed_cp.get_color();
            auto mixed_amount = action_diff.mixed_cp.amount;
            auto op_cnt = action_diff.op_cnt;
            auto pallet_rest_amount = action_diff.pallet_rest_amount + data.pallet.total_rest_color_amount;
            auto new_cost = simulator.calc_cost(color, target_colors[data.state.next_h], data.state.op_cnt + op_cnt, data.state.next_h, pallet_rest_amount, mixed_amount);
            // コスト差を計算
            float cost_diff = new_cost.total(data.sa_progress) - state.cost.total(data.sa_progress);
            if(should_accept_transition(cost_diff, current_temp)){
                state.cost = new_cost;
                state.total_action_diff = action_diff; // 差分を更新
                return true;
            }else{
                // アクションを元に戻す
                state.actions.insert(state.actions.begin() + action_idx, action);
                state.tank_used[action.tankId] = true;
                return false;
            }
        }

        bool change_action(ActionState& state, StaticData& data, float current_temp){
            if(state.actions.empty()) return false; // アクションがない場合は無視

            // ランダムにアクションを選ぶ
            int action_idx = rnd(state.actions.size());
            auto src_action = state.actions[action_idx];
            state.tank_used[src_action.tankId] = false; // 変更前のタンクを未使用にする

            auto &action = state.actions[action_idx]; // 変更するアクション

            // タンクをランダムに選ぶ
            int tank_id = rnd(data.pallet.TankNum);
            if(state.tank_used[tank_id]){
                // 既に使用されているタンクは無視
                state.tank_used[src_action.tankId] = true; // 元のアクションのタンクを使用済みに戻す
                return false;
            }

            // チューブをランダムに選ぶ
            int addTubeIdx = -1;
            if(rnd(100) < CHANGE_ACTION_ADD_TUBE_RATE && data.pallet.tank_cp[tank_id].amount + 1 <= data.pallet.partition[tank_id] * max_add_tube_pallet_rate) {
                if(rnd(3) < 2){
                    addTubeIdx = src_action.addTubeIdx; // 元のアクションのチューブを再利用
                } else{
                    addTubeIdx = rnd(K); // 新しいチューブをランダムに選ぶ
                }
            }
            
            // パーティション位置をランダムに選ぶ
            int next_partition_pos = rnd(data.pallet.partition[tank_id]);

            // アクションを変更
            action.tankId = tank_id;
            action.addTubeIdx = addTubeIdx;
            action.nextPartitionPos = next_partition_pos;

            ActionDiff action_diff = state.total_action_diff;
            simulator.sub_diff(action_diff, src_action, data.pallet); // 元のアクションの差分を引く
            simulator.add_diff(action_diff, action, data.pallet); // 新しいアクションの差分を足す
            
            // コストを計算
            // auto [color, mixed_amount, op_cnt, pallet_rest_amount] = simulator.dryrun_action(state.actions, -1, data.pallet);
            auto color = action_diff.mixed_cp.get_color();
            auto mixed_amount = action_diff.mixed_cp.amount;
            auto op_cnt = action_diff.op_cnt;
            auto pallet_rest_amount = action_diff.pallet_rest_amount + data.pallet.total_rest_color_amount;
            auto new_cost = simulator.calc_cost(color, target_colors[data.state.next_h], data.state.op_cnt + op_cnt, data.state.next_h, pallet_rest_amount, mixed_amount);            

            // コスト差を計算
            float cost_diff = new_cost.total(data.sa_progress) - state.cost.total(data.sa_progress);
            if(should_accept_transition(cost_diff, current_temp)){
                state.cost = new_cost;
                state.tank_used[tank_id] = true; // 新しいタンクを使用済みにする
                state.total_action_diff = action_diff; // 差分を更新
                return true;
            }else{
                // アクションを元に戻す
                state.actions[action_idx] = src_action; // 元のアクションに戻す
                state.tank_used[src_action.tankId] = true; // 元のアクションのタンクを使用済みに戻す
                return false;
            }
        }

        bool slide_partition(ActionState& state, StaticData& data, float current_temp){
            if(state.actions.empty()) return false; // アクションがない場合は無視

            int action_idx = rnd(state.actions.size());
            auto &action = state.actions[action_idx];
            int tank_id = action.tankId;
            // パーティション位置をランダムに選ぶ
            int prev_partition_pos = action.nextPartitionPos; // 元の位置を保存
            int partition_diff = rnd(2) ? 1 : -1; // 前後にスライドする方向を決定
            int next_partition_pos = prev_partition_pos + partition_diff;
            if(next_partition_pos < 0 || next_partition_pos >= data.pallet.partition[tank_id]) return false; // 範囲外なら無視

            int new_op_cnt =  state.total_action_diff.op_cnt;
            auto new_mixed_cp =  state.total_action_diff.mixed_cp; // 混ぜ合わせ場の色と量を更新
            auto cp = data.pallet.tank_cp[tank_id];
            if(action.addTubeIdx >= 0) cp += tubes[action.addTubeIdx];
            cp *= frac[1][data.pallet.partition[tank_id]] * -partition_diff;
            new_mixed_cp += cp; // 混ぜ合わせ場の色と量を更新
            
            if(prev_partition_pos > 0 && prev_partition_pos < data.pallet.partition_reset_threshold){
                new_op_cnt -= 2; // 仕切りをMaxPartitionに戻す操作回数
            }
            if(next_partition_pos > 0 && next_partition_pos < data.pallet.partition_reset_threshold){
                new_op_cnt += 2; // 仕切りをMaxPartitionに戻す操作回数
            }
            
            // コストを計算
            auto color = new_mixed_cp.get_color();
            auto mixed_amount = new_mixed_cp.amount;
            auto pallet_rest_amount = state.total_action_diff.pallet_rest_amount + data.pallet.total_rest_color_amount;
            auto new_cost = simulator.calc_cost(color, target_colors[data.state.next_h], data.state.op_cnt + new_op_cnt, data.state.next_h, pallet_rest_amount, mixed_amount);
            // コスト差を計算
            float cost_diff = new_cost.total(data.sa_progress) - state.cost.total(data.sa_progress);
            if(should_accept_transition(cost_diff, current_temp)){
                state.cost = new_cost;
                state.total_action_diff.mixed_cp = new_mixed_cp; // 差分を更新
                state.total_action_diff.op_cnt = new_op_cnt; // 操作回数を更新
                action.nextPartitionPos = next_partition_pos;
                return true;
            }else{
                return false;
            }
        }

        bool change_tube(ActionState& state, StaticData& data, float current_temp){
            if(state.actions.empty()) return false; // アクションがない場合は無視

            // ランダムにアクションを選ぶ
            int action_idx = rnd(state.actions.size());
            auto& src_action = state.actions[action_idx];
            int prev_tube_idx = src_action.addTubeIdx; // 元のチューブのインデックスを保存
            if(prev_tube_idx < 0) return false; // チューブが追加されていないアクションは無視

            ActionDiff action_diff = state.total_action_diff;
            simulator.sub_diff(action_diff, src_action, data.pallet); // 元のアクションの差分を引く
            src_action.addTubeIdx = rnd(K); // 新しいチューブのインデックスをランダムに選ぶ
            simulator.add_diff(action_diff, src_action, data.pallet); // 新しいアクションの差分を足す
            
            // コストを計算
            auto color = action_diff.mixed_cp.get_color();
            auto mixed_amount = action_diff.mixed_cp.amount;
            auto op_cnt = action_diff.op_cnt;
            auto pallet_rest_amount = action_diff.pallet_rest_amount + data.pallet.total_rest_color_amount;
            auto new_cost = simulator.calc_cost(color, target_colors[data.state.next_h], data.state.op_cnt + op_cnt, data.state.next_h, pallet_rest_amount, mixed_amount);            

            // コスト差を計算
            float cost_diff = new_cost.total(data.sa_progress) - state.cost.total(data.sa_progress);
            if(should_accept_transition(cost_diff, current_temp)){
                state.cost = new_cost;
                state.total_action_diff = action_diff; // 差分を更新
                return true;
            }else{
                // アクションを元に戻す
                src_action.addTubeIdx = prev_tube_idx; // 元のチューブのインデックスに戻す
                return false;
            }
        }



    } transition;

    struct Temp{
        float start_temp = 500;
        float mid_temp = 2000;
        float end_temp = 0.1;
        
        inline float get_temp(float progress){
            // float threshold = 0.3;
            // if(progress < threshold)
            //     return pow(start_temp, 1.0 - progress / threshold) * pow(mid_temp, progress / threshold);
            // else
            //     return mid_temp + (end_temp - mid_temp) * (progress - threshold) / (1.0 - threshold);
            return pow(start_temp, 1.0 - progress) * pow(end_temp, progress);
            // return start_temp + (end_temp - start_temp) * progress;
        }
    } temp;

    ActionState run(float time_limit_sec, ActionState action_state){
        timer.mark_lap();
        
        ActionState best_state = action_state;
        float current_temp = temp.start_temp;
        int loop_cnt = 0;
        int accept_cnt = 0;
        while(timer.lap_sec() < time_limit_sec){
            float progress = (float)timer.lap_sec() / time_limit_sec;
            current_temp = temp.get_temp(progress);
            data.sa_progress = progress; // 焼きなましの進捗を更新
            rep(_,0,32){ 
                loop_cnt++;
                bool accept = transition.execute(action_state, data, current_temp);
                accept_cnt += accept;

                if(accept && action_state.total_action_diff.mixed_cp.amount >= 1.0 && action_state.cost.total(data.sa_progress) < best_state.cost.total(data.sa_progress)){
                    // TODO: action_stateのmixed_amountが1.0以上の時のみ受け入れるようにする
                    // debug(loop_cnt, current_temp, action_state.cost);
                    best_state = action_state;
                }
            }
        }
        // rep(i,0,transition.TransitionTypeSize){
        //     float accept_rate = (float)transition.accept_cnt[i] / max(1LL, transition.try_cnt[i]) * 100.0;
        //     // 小数点以下2桁まで表示
        //     cerr << "type: " << i << ", accept_rate: " << fixed << setprecision(2) << accept_rate << "%" << endl;
        // }
        // cerr << "loop_cnt: " << loop_cnt << ", accept_rate: " << (float)accept_cnt / loop_cnt << endl;
        return best_state;
    }
};

/**
 * 毎ターン、いくつかのタンクから混ぜ合わせ場に絵の具を流して、目的の色を作ってスポイトする。
 */
struct Solver{
    State state; // 現在の状態
    priority_queue_rev<float> worst_color_errors; // やり直し判定に使う、これまでの色の誤差結果の悪いやつら
    ActionSA action_sa;
    int h0_start_time;
    float worst_rate = 0.05; // 最悪の色の誤差を保持する割合

    Solver(int tank_num) : action_sa(state) {
        state.pallet.init(tank_num);
    }

    void turn_action(int h){
        auto &pallet = state.pallet;
        auto target_color = target_colors[h];

        Cost init_cost = {1e9, 0}; // 最良のコスト
        V<PalletAction> init_actions; // 最良のアクション
        V<bool> init_tank_used(pallet.TankNum, false); // タンクが使用されたかどうか
        ActionDiff init_action_diff;
        init_action_diff.mixed_cp = pallet.mixed_cp; // 混ぜ合わせ場の色と量
        init_action_diff.op_cnt++; // 提出アクションの操作回数を加算
        init_action_diff.pallet_rest_amount -= 1; // 提出アクションでパレット全体の残り色量を1減らす

        auto best_cost = init_cost; // 最良のコスト
        auto best_actions = init_actions; // 最良のアクション
        auto best_tank_used = init_tank_used; // 最良のタンク使用状況
        auto best_action_diff = init_action_diff; // 最良のアクション差分
        int best_submit_tank_id = pallet.TankNum; // best_action時の提出タンクID        
        
        action_sa.set_state(state); 

        const int outer_loop_cnt = 2;
        rep(tt,0,outer_loop_cnt){
            ActionSA::ActionState action_state = {init_actions, init_cost, init_tank_used, init_action_diff};
            
            const int loop_cnt = 80 / outer_loop_cnt;
            float time_limit = (time_limit_sec - 0.4) / H / loop_cnt / outer_loop_cnt; // 各ループの時間制限
            rep(t,0,loop_cnt){
                // action_sa.temp.start_temp = 400 * (loop_cnt - t) / loop_cnt;
                action_sa.temp.start_temp = pow(1000, 1.0 - (float)t / loop_cnt) * pow(3, (float)t / loop_cnt); // 焼きなましの初期温度を調整
                action_sa.temp.end_temp = 0.1;
                action_state = action_sa.run(time_limit, action_state);
            }
            if(action_state.cost.total() < best_cost.total()){
                best_actions = action_state.actions;
                best_cost = action_state.cost;
                best_tank_used = action_state.tank_used;
                best_action_diff = action_state.total_action_diff;
                auto [best_color, _, _1, _2] = simulator.dryrun_action(best_actions, pallet.TankNum, pallet);
                if(tt < loop_cnt - 1 && h > 200 && best_color.distance(target_color) * 10000 * h < state.total_color_errors * 0.3){
                    // debug("exit early", h, best_color.distance(target_color) * 10000, state.total_color_errors);
                    break;
                }
            }
        }

        auto [best_color, _, _1, _2] = simulator.dryrun_action(best_actions, pallet.TankNum, pallet);
        float color_error = best_color.distance(target_color) * 10000; // 色の誤差を計算
        if(h > 20)worst_color_errors.push(color_error); // 色の誤差を記録
        while(worst_color_errors.size() > h * worst_rate) worst_color_errors.pop(); // 最悪の色の誤差を1000個まで保持
        
        int predict_end_time = h0_start_time + (timer.elapsed_ms() - h0_start_time) * H / (h + 1); // 残り時間を予測
        if(predict_end_time > 2950) worst_rate -= 0.001; chmax<float>(worst_rate, 0.0);
        if(predict_end_time < 2900) worst_rate += 0.001;
        // cerr << predict_end_time << endl;

        // debug(predict_end_time, timer.elapsed_ms(), worst_rate);
        // int predict_end_time_threshold = h < H - 20 ? 2950 : 2950;
        int predict_end_time_threshold = 2950 + (H - h) * 50 / H;
        if(predict_end_time < predict_end_time_threshold && h > 50 && worst_color_errors.size() > 0 && color_error > worst_color_errors.top()){
            // cerr << h << ": [before] color_error: " << best_color.distance(target_color) * 10000 << ", opcmnt:" << best_action_diff.op_cnt;
            const int outer_loop_cnt = 2;
            rep(tt,0,outer_loop_cnt){
                ActionSA::ActionState action_state = {init_actions, init_cost, init_tank_used, init_action_diff};
                
                const int loop_cnt = 80 / outer_loop_cnt;
                rep(t,0,loop_cnt){
                    // action_sa.temp.start_temp = 400 * (loop_cnt - t) / loop_cnt;
                    action_sa.temp.start_temp = pow(4000, 1.0 - (float)t / loop_cnt) * pow(3, (float)t / loop_cnt); // 焼きなましの初期温度を調整
                    action_sa.temp.end_temp = 0.1;
                    action_state = action_sa.run((time_limit_sec - 0.4) / H / loop_cnt / outer_loop_cnt, action_state);
                }
                if(action_state.cost.total() < best_cost.total()){
                    best_actions = action_state.actions;
                    best_cost = action_state.cost;
                    best_tank_used = action_state.tank_used;
                    best_action_diff = action_state.total_action_diff;
                    tie(best_color, _, _1, _2) = simulator.dryrun_action(best_actions, pallet.TankNum, pallet);
                }
            }
            // cerr << "-> [after] color_error: " << best_color.distance(target_color) * 10000 <<  ", opcmnt:" << best_action_diff.op_cnt << endl;

            if(best_color.distance(target_color) * 10000 * (h+1) > state.total_color_errors * 10){
                // cerr << "やばすぎ" << best_color.distance(target_color) * 10000 << ", current total:" << state.total_color_errors << endl;
                const int outer_loop_cnt = 2;
                rep(tt,0,outer_loop_cnt){
                    ActionSA::ActionState action_state = {init_actions, init_cost, init_tank_used, init_action_diff};
                    
                    const int loop_cnt = 80 / outer_loop_cnt;
                    rep(t,0,loop_cnt){
                        // action_sa.temp.start_temp = 400 * (loop_cnt - t) / loop_cnt;
                        action_sa.temp.start_temp = pow(4000, 1.0 - (float)t / loop_cnt) * pow(3, (float)t / loop_cnt); // 焼きなましの初期温度を調整
                        action_sa.temp.end_temp = 0.1;
                        action_state = action_sa.run((time_limit_sec - 0.4) / H / loop_cnt / outer_loop_cnt, action_state);
                    }
                    if(action_state.cost.total() < best_cost.total()){
                        best_actions = action_state.actions;
                        best_cost = action_state.cost;
                        best_tank_used = action_state.tank_used;
                        best_action_diff = action_state.total_action_diff;
                        tie(best_color, _, _1, _2) = simulator.dryrun_action(best_actions, pallet.TankNum, pallet);
                    }
                }
                // cerr << "やばすぎ -> " << best_color.distance(target_color) * 10000 << endl;
            }
        }

        if(h < 10) cerr << h << ": color_error: " << color_error << endl; // 最初の10ターンは色の誤差を表示

        // 最後のターンは、残っている色を全て提出するケースを考慮
        if(h == H - 1 && pallet.total_rest_color_amount == 1){
            ColorPortion mixed_cp = pallet.mixed_cp; // 混ぜ合わせ場の色と量
            int op_cnt = 1; // 操作回数. 1は提出アクションの分
            V<PalletAction> actions;
            rep(i, 0, pallet.TankNum) {
                if(pallet.tank_cp[i].amount < 0.00000001) continue;
                actions.push_back(PalletAction(i, -1, 0)); // タンクiの色を出力するアクション
                mixed_cp += pallet.tank_cp[i]; // 混ぜ合わせ場にタンクiの色を追加
                op_cnt++;
            }
            if(op_cnt + state.op_cnt <= T){
                auto cost = simulator.calc_cost(mixed_cp.get_color(), target_color, state.op_cnt + op_cnt, h, pallet.total_rest_color_amount - 1, mixed_cp.amount);
                if(cost.total() < best_cost.total()) {
                    best_cost = cost; // 最良のコストを更新
                    best_actions = actions; // タンクの色を全て出力するアクション
                    best_submit_tank_id = pallet.TankNum; // 混ぜ合わせ場を提出する
                    best_color = mixed_cp.get_color(); // 混ぜ合わせ場の色を更新
                }
            }
        }

        // 1. 各タンクのColorを直接出力する
        rep(i,0,pallet.TankNum+1){
            auto &cp = (i < pallet.TankNum) ? pallet.tank_cp[i] : pallet.mixed_cp;
            if(cp.amount < 1.0) continue; // 量が1g未満のタンクは提出不可
            Color color = cp.get_color();
            int total_rest_color_amount = pallet.total_rest_color_amount - 1;
            auto cost = simulator.calc_cost(color, target_color, state.op_cnt, h, total_rest_color_amount, 1.0); // 混ぜ合わせ場じゃないので、mixed_amountのcostは不要
            if(cost.total() < best_cost.total()) {
                best_cost = cost; // 最良のコストを更新
                best_actions = {PalletAction(i, -1, -1)}; // タンクiの色を出力するアクション
                best_submit_tank_id = i; // タンクiを提出する
                best_color = color;
            }
        }
        
        // 2. 各タンクに1チューブを足して、その色を出力する
        rep(i,0,pallet.TankNum+1){
            auto &cp = (i < pallet.TankNum) ? pallet.tank_cp[i] : pallet.mixed_cp;
            if(i < pallet.TankNum && cp.amount + 1 > pallet.partition[i]) continue; // タンクが満タン以上になる場合はスキップ
            rep(tube_idx,0,K){
                auto tube_cp = tubes[tube_idx];
                Color new_color = ColorPortion::mix(cp, tube_cp).get_color();
                int total_rest_color_amount = pallet.total_rest_color_amount;
                auto cost = simulator.calc_cost(new_color, target_color, state.op_cnt, h, total_rest_color_amount, 1.0); // 混ぜ合わせ場じゃないので、mixed_amountのcostは不要
                if(cost.total() < best_cost.total()) {
                    best_cost = cost; // 最良のコストを更新
                    best_actions = {PalletAction(i, tube_idx, -1)}; // タンクiにチューブを追加するアクション
                    best_submit_tank_id = i; // タンクiを提出する
                    best_color = new_color;
                }
            }
        }

        // best_actionを実行
        state.output_and_apply(best_actions, best_submit_tank_id, target_color.distance(best_color) * 10000);
    }

    void init_tube_operations_tank40(){
        // TODO
        int tube_idx = 0;
        rep(side,0,2){
            rep(i,0,2){
                rep(j,0,N/2){
                    int x = i*N/2 + j;
                    int y = side == 0 ? 0 : N - 1;
                    if(j == 0){
                        // addtube → partition
                        cout << V<int>{ADD_TUBE, x, y, tube_idx} << endl; // タンクID, チューブの追加, 次のパーティション位置

                        cout << V<int>{PARTITION, x, y, x+1, y} << endl; // タンクID, パーティションの追加, 次のパーティション位置
    
                        // 各タンクにチューブの絵の具を追加
                        int total_size = 0;
                        rep(k,0,N/2) total_size += state.pallet.partition[side*N + k + i * (N/2)];
                        rep(k,0,N/2){
                            float rate = (float)state.pallet.partition[side*N + k + i * (N/2)] / total_size; // タンクのサイズに比例して分配
                            state.pallet.tank_cp[side*N + k + i * (N/2)] += tubes[tube_idx] * rate; // タンクにチューブの絵の具を追加
                        }
                        state.op_cnt += 2; // パーティションを追加した場合は操作回数を加算
                        state.pallet.total_rest_color_amount += 1; // パレットの残り色量を1増やす
                    }else{
                        // partition → addtube
                        if(j < N/2 - 1) cout << V<int>{PARTITION, x, y, x+1, y} << endl; // タンクID, パーティションの追加, 次のパーティション位置
                        cout << V<int>{ADD_TUBE, x, y, tube_idx} << endl; // タンクID, チューブの追加, 次のパーティション位置
                        state.pallet.tank_cp[side*N + j + i * (N/2)] += tubes[tube_idx]; // タンクにチューブの絵の具を追加
                        // state.op_cnt += 2; // パーティションを追加した場合は操作回数を加算
                        state.op_cnt += j < N/2 - 1 ? 2 : 1; // 最後のパーティション追加は操作回数を加算しない
                        state.pallet.total_rest_color_amount += 1; // パレットの残り色量を1増やす
                    }
                    tube_idx = (tube_idx + 1) % K;
                }
                if(tube_idx == 0) tube_idx = 1;
            }
        }
    }

    void init_tube_oprations(){
        if(state.pallet.TankNum == 40){
            init_tube_operations_tank40();
            return;
        }

        int tube_idx = 0;
        rep(i,0,2){
            rep(j,0,state.pallet.TankNum/2){
                auto [x, y] = state.pallet.tank_lower_leftern(j + i * (state.pallet.TankNum/2));
                if(j == 0){
                    // addtube → partition
                    cout << V<int>{ADD_TUBE, x, y, tube_idx} << endl; // タンクID, チューブの追加, 次のパーティション位置
                    cout << V<int>{PARTITION, x, y, x+1, y} << endl; // タンクID, パーティションの追加, 次のパーティション位置

                    // 各タンクにチューブの絵の具を追加
                    int total_size = 0;
                    rep(k,0,state.pallet.TankNum/2) total_size += state.pallet.partition[k + i * (state.pallet.TankNum/2)];
                    rep(k,0,state.pallet.TankNum/2){
                        float rate = (float)state.pallet.partition[k + i * (state.pallet.TankNum/2)] / total_size; // タンクのサイズに比例して分配
                        state.pallet.tank_cp[k + i * (state.pallet.TankNum/2)] += tubes[tube_idx] * rate; // タンクにチューブの絵の具を追加
                    }
                    state.op_cnt += 2; // パーティションを追加した場合は操作回数を加算
                    state.pallet.total_rest_color_amount += 1; // パレットの残り色量を1増やす
                }else{
                    // partition → addtube
                    if(j < state.pallet.TankNum/2 - 1) cout << V<int>{PARTITION, x, y, x+1, y} << endl; // タンクID, パーティションの追加, 次のパーティション位置
                    cout << V<int>{ADD_TUBE, x, y, tube_idx} << endl; // タンクID, チューブの追加, 次のパーティション位置
                    state.pallet.tank_cp[j + i * (state.pallet.TankNum/2)] += tubes[tube_idx]; // タンクにチューブの絵の具を追加
                    state.op_cnt += j < state.pallet.TankNum/2 - 1 ? 2 : 1; // パーティションを追加した場合は操作回数を加算
                    state.pallet.total_rest_color_amount += 1; // パレットの残り色量を1増やす
                }
                tube_idx = (tube_idx + 1) % K;
            }
            if(tube_idx == 0) tube_idx = 1;
        }
    }

    int decide_partition_reset_threshold(MultiTankPallet &pallet) {
        if(pallet.TankNum == 40) return 5;
        float t_exp = log2((float)T / 4000);
        int threshold = (int)(t_exp * 5.5 * 20 / pallet.TankNum); // Tが大きいほど仕切りを戻す閾値を大きくする
        chmax(threshold, 10  * 20 / pallet.TankNum); // 0未満は無効
        chmin(threshold, pallet.MaxPartition);
        return threshold;
    }

    int decide_pallet_limit_threshold(MultiTankPallet &pallet) {
        // パレットの残り色量の閾値を決定
        int partition_reset_threshold = decide_partition_reset_threshold(pallet);
        // 平均的なタンクのマス数
        int average_tank_cells = (pallet.MaxPartition + partition_reset_threshold) * pallet.TankNum / 2;

        if(K == 4 && T > 16000) return average_tank_cells * 0.02;
        
        int threshold = (average_tank_cells * 0.04); // 平均の40%を閾値とする
        if(T < 20000){
            float rate = pow(2, (float)(20000 - T) / 10000); // Tが小さいほど閾値を大きくする
            threshold = (int)(threshold * rate); // Tが小さいほど閾値を大きくする
        }

        
        return threshold;
    }

    void set_max_add_tube_pallet_rate(){
        if(state.pallet.TankNum == 40) max_add_tube_pallet_rate = 0.7;
        else max_add_tube_pallet_rate = 0.25;
    }


    void run(){
        
        // 初期状態のパレットを出力
        InitPalletDecoder::output(state.pallet);
        
        init_tube_oprations();
        set_max_add_tube_pallet_rate();
        

        state.pallet.partition_reset_threshold = decide_partition_reset_threshold(state.pallet); // 仕切りを戻す閾値を決定
        simulator.pallet_limit_threshold = decide_pallet_limit_threshold(state.pallet); // パレットの残り色量の閾値を決定
        simulator.init(T, state.op_cnt, state.pallet.TankNum == 40);

        h0_start_time = timer.elapsed_ms();
        cerr << "h0_start_time: " << h0_start_time << "ms" << endl;
        float prev_color_errors = 0;
        rep(h,0,H){
            simulator.set_op_cnt_penalty(h, state.pallet.TankNum == 40);
            simulator.set_calc_cost_variables(h);
            if(h == H - 1) simulator.aim_mixed_cp_upper = 99;
            turn_action(h);
            if((h+1) % 100 == 0) {
                float error_diff = state.total_color_errors - prev_color_errors;
                debug("Turn", h + 1, error_diff);
                prev_color_errors = state.total_color_errors;
            }
        }
        
        int score = 1 + state.total_color_errors + state.pallet.total_rest_color_amount * D;
        debug(state.total_color_errors, state.pallet.total_rest_color_amount, state.pallet.total_rest_color_amount * D);
        cerr << "Total op cnt = " << state.op_cnt << endl;
        if(state.op_cnt > T){
            #ifdef LOCAL
            cerr << "Error: Total operation count exceeds T. op_cnt = " << state.op_cnt << ", T = " << T << endl;
            cerr << "Score = ExceedsTurn" << state.op_cnt << endl;
            exit(1);
            #endif
        }
        if(timer.elapsed_ms() > 3000){
            #ifdef LOCAL
            cerr << "Error: Time limit exceeded. elapsed_ms = " << timer.elapsed_ms() << endl;
            cerr << "Score = ExceedsTimeLimit" << timer.elapsed_ms() << endl;
            exit(1);
            #endif
        }
        cerr << "Score = " << score << endl;
    }
};

int calc_tank_num(){
    int tank_num = 20;
    // 10000 < T < 20000, K > 11, つまり、操作回数がちょい物足りなくてインク種類数が多いケース は10タンク。
    // 操作回数を絞りたいから1タンクの区切りを細かくする、インク数は多いので、操作の幅が狭くなりすぎることはないという気持ち。
    // 10000より小さくなると、タンクに直接色注入のケースが出てくるので、タンク数が減るのは困る。
    if(K > 11 && 8000 < T && T < 20000) tank_num = 10;
    if(T < 6000) tank_num = 40;
    return tank_num;    
}

int decide_over_pallet_limit_penalty(){
    // over_penaltyを気にしないで良いケースは？（=タンクにたくさんいれるべきケース）
    // Tが小さい時(<7000くらい？)：タンク直があるので、タンクにある程度入っていないと多様性の役に立たない？
    // Tが大きくなく、Kが小さい時：インク側で多様性を出せないから、タンク側が量多めな色を持つ必要がる？Tが大きい場合は複数タンクを使えるから、タンクが薄い方が嬉しい。

    int tank_num = calc_tank_num();
    // 10タンクの時は、overしたパレットの量に応じてペナルティを与える
    // NOTE: 1タンクの密度制限0.25の方をいじっても良い気はする。それを下げれば、0でも問題なさそう。
    if(tank_num == 10) return 100;
    // else if(T > 20000) return 100;
    else if(T < 6750) return 0;
    
    else return 100;
    // K,T,D: 6,56k,89 では必要。後半にインクを出し尽くしていると、overするtubeの量が増えて、全体のスコアに対する0.5倍くらいになったりする
    // 15, 58k, 113 も同様
    // 7,8k,3000だと、タンク直入れは稀なので、薄い方が良い
    

}


int main(int argc, char *argv[])
{
#if DEBUG
freopen("tools/in/0003.txt", "r", stdin);
#endif
    input();
    pre_compute();
    simulator.over_pallet_limit_penalty = decide_over_pallet_limit_penalty();

    // if(T < 16000) exit();
    // if(T > 8000) exit(1);
    Solver solver(calc_tank_num());
    solver.run();

    cerr << "elapsed_ms: " << timer.elapsed_ms() << "ms" << endl;
    #ifdef LOCAL
    cerr << "total annealing try count: " << total_annealing_try_count << endl;
    #endif
}