#ifndef hari64
#pragma GCC optimize "Ofast,omit-frame-pointer,inline,unroll-all-loops"
#define NDEBUG
#include <bits/stdc++.h>
#define debug(...)
#else
#include "viewer.hpp"
#define debug(...) viewer::_debug(__LINE__, #__VA_ARGS__, __VA_ARGS__)
#endif
// clang-format off
using namespace std;
template <typename T> using vc = vector<T>;
template <typename T> using vvc = vector<vc<T>>;
template <typename T> using vvvc = vector<vvc<T>>;
using ll = long long; using ld = long double;
using pii = pair<int, int>; using pll = pair<ll, ll>;
using tiii = tuple<int, int, int>; using tlll = tuple<ll, ll, ll>;
using vi = vc<int>; using vvi = vvc<int>; using vvvi = vvvc<int>;
using vll = vc<ll>; using vvll = vvc<ll>; using vvvll = vvvc<ll>;
using vb = vc<bool>; using vvb = vvc<bool>; using vvvb = vvvc<bool>;
using vd = vc<double>; using vvd = vvc<double>; using vvvd = vvvc<double>;
using vpii = vc<pii>; using vvpii = vvc<pii>; using vpll = vc<pll>; using vvpll = vvc<pll>;
using vtiii = vc<tiii>; using vvtiii = vvc<tiii>; using vtlll = vc<tlll>; using vvtlll = vvc<tlll>;
#define ALL(x) begin(x), end(x)
#define RALL(x) (x).rbegin(), (x).rend()
constexpr int INF = 1001001001; constexpr long long INFll = 1001001001001001001;
template <class T> bool chmax(T& a, const T& b) { return a < b ? a = b, 1 : 0; }
template <class T> bool chmaxEq(T& a, const T &b) { return a <= b ? a = b, 1 : 0; }
template <class T> bool chmin(T& a, const T& b) { return a > b ? a = b, 1 : 0; }
template <class T> bool chminEq(T& a, const T &b) { return a >= b ? a = b, 1 : 0; }

struct Timer{
    void start(){_start=chrono::system_clock::now();}
    void stop(){_end=chrono::system_clock::now();sum+=chrono::duration_cast<chrono::nanoseconds>(_end-_start).count();}
    inline int ms()const{const chrono::system_clock::time_point now=chrono::system_clock::now();return static_cast<int>(chrono::duration_cast<chrono::microseconds>(now-_start).count()/1000);}
    inline int ns()const{const chrono::system_clock::time_point now=chrono::system_clock::now();return static_cast<int>(chrono::duration_cast<chrono::microseconds>(now-_start).count());}
    string report(){return to_string(sum/1000000)+"[ms]";}
    void reset(){_start=chrono::system_clock::now();sum=0;}
    private: chrono::system_clock::time_point _start,_end;long long sum=0;
}timer, timer1, timer2, timer3;

struct Xor128{// period 2^128 - 1
    uint32_t x,y,z,w;
    static constexpr uint32_t min(){return 0;}
    static constexpr uint32_t max(){return UINT32_MAX;}
    Xor128(uint32_t seed=0):x(123456789),y(362436069),z(521288629),w(88675123+seed){}
    uint32_t operator()(){uint32_t t=x^(x<<11);x=y;y=z;z=w;return w=(w^(w>>19))^(t^(t>>8));}
    uint32_t operator()(uint32_t l,uint32_t r){return((*this)()%(r-l))+l;}
    uint32_t operator()(uint32_t r){return(*this)()%r;}
};

struct Rand{// https://docs.python.org/ja/3/library/random.html
    Rand():gen(0),mt(0){}
    Rand(int seed):gen(seed),mt(seed){}
    // シードを変更します
    inline void set_seed(int seed){Xor128 _gen(seed);gen=_gen;}
    // ランダムな浮動小数点数（範囲は[0.0, 1.0)) を返します
    inline double random(){return(double)gen()/(double)gen.max();}
    // 確率 prob でTrueを返します
    inline bool randBool(double prob){return gen()<prob*gen.max();}
    // a <= b であれば a <= N <= b 、b < a であれば b <= N <= a であるようなランダムな浮動小数点数 N を返します
    inline double uniform(double a,double b){if(b<a)swap(a,b);return a+(b-a)*double(gen())/double(gen.max());}
    // range(0, stop) の要素からランダムに選ばれた要素を返します
    inline uint32_t randRange(uint32_t r){return gen(r);}
    // range(start, stop) の要素からランダムに選ばれた要素を返します
    inline uint32_t randRange(uint32_t l,uint32_t r){return gen(l,r);}
    // a <= N <= b であるようなランダムな整数 N を返します randRange(a, b + 1) のエイリアスです
    inline uint32_t randInt(uint32_t l,uint32_t r){return gen(l,r+1);}
    // シーケンス x をインプレースにシャッフルします
    template<class T>void shuffle(vector<T>&x){for(int i=x.size(),j;i>1;){j=gen(i);swap(x[j],x[--i]);}}
    // 空でないシーケンス seq からランダムに要素を返します
    template<class T>T choice(const vector<T>&seq){assert(!seq.empty());return seq[gen(seq.size())];}
    // 相対的な重みに基づいて要素が選ばれます (※複数回呼ぶ場合は処理を変えた方が良い)
    template<class T,class U>T choice(const vector<T>&seq,const vector<U>&weights){assert(seq.size()==weights.size());vector<U>acc(weights.size());acc[0]=weights[0];for(int i=1;i<int(weights.size());i++)acc[i]=acc[i-1]+weights[i];return seq[lower_bound(acc.begin(),acc.end(),random()*acc.back())-acc.begin()];}
    // 母集団のシーケンスまたは集合から選ばれた長さ k の一意な要素からなるリストを返します 重複無しのランダムサンプリングに用いられます
    template<class T>vector<T>sample(const vector<T>&p,int k){int j,i=0,n=p.size();assert(0<=k&&k<=n);vector<T>ret(k);unordered_set<int>s;for(;i<k;i++){do{j=gen(n);}while(s.find(j)!=s.end());s.insert(j);ret[i]=p[j];}return ret;}
    // 正規分布です mu は平均で sigma は標準偏差です
    double normalvariate(double mu=0.0,double sigma=1.0){return std::normal_distribution(mu,sigma)(mt);}
    private: Xor128 gen;std::mt19937 mt;
}myrand;

namespace esc{const vector<int>colors{196,208,226,46,77,14,12,13,5,136,195,245};constexpr int RED=0,ORANGE=1,YELLOW=2,LIGHTGREEN=3,GREEN=4,AQUA=5,BLUE=6,PINK=7,PURPLE=8,BROWN=9,WHITE=10,GRAY=11;
void back(int n){cerr<<"\e["<<n<<"A";}void locate(int r,int c){cerr<<"\e["<<r+1<<+";"<<c+1<<"H";}void reset(){cerr<<"\e[0m";}
void color(int c){cerr<<"\e[38;5;"<<colors[c]<<"m";}void color(int c,string s){color(c);cerr<<s;reset();}void color(int c,int s){color(c,to_string(s));}
void bcolor(int c){cerr<<"\e[4;"<<colors[c]<<"m";}void bcolor(int c,string s){bcolor(c);cerr<<s;reset();}void bcolor(int c,int s){color(c,to_string(s));}
string with_sep(int n,char sep=','){string ret="",s=to_string(n);reverse(s.begin(),s.end());for(int i=0,len=s.length();i<=len;){ret+=s.substr(i,3);if((i+=3)>=len)break;ret+=sep;}reverse(ret.begin(),ret.end());return ret;}
string with_fill(int n,int num=3,char space=' '){string s=to_string(n);return string(max(0,num-int(s.size())),space)+s;}}  // namespace esc
// clang-format on

int _N;      // 島の大きさ (10 <= N <= 20)
int M;       // 油田の個数 ( 2 <= M <= 20)
double eps;  // エラーパラメータ (0.01 <= eps <= 0.2)
constexpr int TL = 3000;
constexpr double PARAM_THRESHOLD = 30.0;

constexpr int DY[4] = {0, 1, 0, -1};
constexpr int DX[4] = {1, 0, -1, 0};
int toIdx(int y, int x) { return y * _N + x; }
pii toYX(int idx) { return {idx / _N, idx % _N}; }
bool isInside(int y, int x) { return 0 <= y && y < _N && 0 <= x && x < _N; }
template <int N>  // 2N^2 * M * N^2 <= 6.4e6
inline int toCountIdx(int trialIdx, int ofIdx, int dIdx) {
    // assert(0 <= dIdx && dIdx < N * N);
    return trialIdx * M * N * N + ofIdx * N * N + dIdx;
}

template <class T, int N>
void visGrid(const array<T, N * N>& grid, T minVal = +999, T maxVal = -999) {
#ifdef ONLINE_JUDGE
    return;
#endif
    // color1 : (255, 128, 0)
    // color2 : (0, 128, 255)
    if (minVal == +999) minVal = *min_element(ALL(grid));
    if (maxVal == -999) maxVal = *max_element(ALL(grid));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << "#c " << i << " " << j << " ";
            int r, g = 128, b;
            if (maxVal != minVal) {
                T val = grid[toIdx(i, j)];
                assert(minVal <= val && val <= maxVal);
                r = 255 * (val - minVal) / (maxVal - minVal),
                b = 255 * (maxVal - val) / (maxVal - minVal);
            } else {
                r = 128, b = 128;
            }
            cout << "#" << hex << r / 16 << r % 16 << g / 16 << g % 16 << b / 16
                 << b % 16 << dec << endl;
        }
    }
}

template <int N>
void debugGrid(const array<int, N * N>& grid) {
#ifndef hari64
    return;
#endif
    esc::color(esc::LIGHTGREEN, "debugGrid");
    cerr << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            cerr << (grid[toIdx(i, j)] == 0 ? "." : to_string(grid[toIdx(i, j)]));
        cerr << endl;
    }
}
template <int N>
void debugGrid2(const array<int, N * N>& grid1, const array<int, N * N>& grid2) {
#ifndef hari64
    return;
#endif
    esc::color(esc::LIGHTGREEN, "debugGrid2");
    cerr << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            cerr << (grid1[toIdx(i, j)] == 0 ? "." : to_string(grid1[toIdx(i, j)]));
        cerr << " | ";
        for (int j = 0; j < N; j++)
            cerr << (grid2[toIdx(i, j)] == 0 ? "." : to_string(grid2[toIdx(i, j)]));
        cerr << endl;
    }
}

template <int N>
struct _OilField {
    int d;     // 油田の大きさ
    vi ps;     // 各要素の座標
    int maxY;  // psのy座標の最大値
    int maxX;  // psのx座標の最大値

    friend istream& operator>>(istream& is, _OilField& of) {
        is >> of.d;
        of.ps.resize(of.d);
        of.maxY = of.maxX = 0;
        for (auto& idx : of.ps) {
            int y, x;
            is >> y >> x;
            idx = toIdx(y, x);
            chmax(of.maxY, y);
            chmax(of.maxX, x);
        }
        sort(ALL(of.ps));
        return is;
    }

    friend ostream& operator<<(ostream& os, const _OilField& of) {
        array<int, N * N> grid;
        grid.fill(0);
        for (int v : of.ps) grid[v] = 1;
        debugGrid<N>(grid);
        return os;
    }
};

template <int N>
struct _Island {
   private:
    double prob;  // 正規化された確率
   public:
    double likelihood;  // 2を底とする対数尤度
    vi positions;       // M個の油田のdIdx
    array<int, N * N> oil;

    _Island() : prob(0), likelihood(0), positions(M, -1) { oil.fill(0); }
    _Island(const vi _positions, const vc<_OilField<N>>& oilFields)
        : prob(0), likelihood(0), positions(_positions) {
        oil.fill(0);
        init(oilFields);
    }
    _Island(const double _likelihood, const vi _positions,
            const vc<_OilField<N>>& oilFields)
        : prob(0), likelihood(_likelihood), positions(_positions) {
        oil.fill(0);
        init(oilFields);
    }

    void init(const vc<_OilField<N>>& oilFields) {
        assert(all_of(ALL(positions), [](int dIdx) { return dIdx != -1; }));
        for (int ofIdx = 0; ofIdx < M; ofIdx++)
            for (int v : oilFields[ofIdx].ps) oil[v + positions[ofIdx]]++;
    }
    void setProb(double baseLikelihood) {
        assert(!std::isnan(likelihood) && !std::isnan(baseLikelihood));
        prob = exp2(likelihood - baseLikelihood);
    }
    void normalizeProb(double sum) { prob /= sum; }
    double getProb() const { return prob; }
    bitset<N * N> toBitset() const {
        bitset<N * N> ret;
        ret.reset();
        for (int i = 0; i < N * N; i++) ret[i] = oil[i] > 0;
        return ret;
    }
};

using INT = uint16_t;  // 0 <= INT <= 65535

struct Gauss {
    vvd memo;

    Gauss() {}

    double calcLog2(int trialIdx, INT vS) {
        assert(0 <= trialIdx && trialIdx < int(memo.size()));
        assert(0 <= vS && vS < int(memo[trialIdx].size()));
        return memo[trialIdx][vS];
    }
    void writeMemo(INT k, INT x) {
        int trialIdx = memo.size();
        memo.push_back(vd(k * M + 1, -100));
        if (k == 1)
            memo[trialIdx][x] = 0;
        else
            for (INT vS = 0; vS <= k * M; vS++)
                memo[trialIdx][vS] = _calcLog2(k, vS, x);
    }
    double calcValidityOfVSArray(const vc<INT>& sizes, const vc<INT>& vSArray,
                                 const vc<INT>& responses,
                                 [[maybe_unused]] const int color = esc::RED) {
        // 尤度の平均と分散を計算する
        double mu = 0.0, var = 0.0, likelihood = 0.0;
        for (int trialIdx = 0; trialIdx < int(memo.size()); trialIdx++) {
            likelihood +=
                _calcLog2(sizes[trialIdx], vSArray[trialIdx], responses[trialIdx]);
            double muT = 0.0, varT = 0.0;
            double sumOfProb = 0.0;
            for (INT vS2 = 0; vS2 <= sizes[trialIdx] * M; vS2++) {
                double p = calc(sizes[trialIdx], vSArray[trialIdx], vS2);
                if (p == 0.0) continue;
                double logP = std::log2(p);
                if (logP < -INF) continue;
                sumOfProb += p;
                muT += p * logP;
                varT += p * logP * logP;
            }
            assert(abs(sumOfProb - 1.0) < 1e-5);
            muT /= sumOfProb;
            varT /= sumOfProb;
            varT = varT - muT * muT;
            mu += muT;
            var += varT;
        }
        double sigma = std::sqrt(var);
        double validity = (likelihood - mu) / sigma;
#ifdef hari64
        esc::color(color, "validity: " + to_string(validity) + "\n");
#endif
        return validity;
    }

   private:
    /// @param k: 掘ったマスの数 @param vS: (予想)石油埋蔵量 @param x: 観測値
    /// @return P(x-0.5 < X < x+0.5 | k, vS)
    double calc(INT k, INT vS, INT x) {
        assert(0 <= k && 0 <= vS && 0 <= x);
        double mu = (k - vS) * eps + vS * (1 - eps);
        double var = k * eps * (1 - eps);
        double upper = 0.5 * (1.0 + std::erf((x + 0.5 - mu) / std::sqrt(2 * var)));
        if (x == 0) return upper;
        double lower = 0.5 * (1.0 + std::erf((x - 0.5 - mu) / std::sqrt(2 * var)));
        return upper - lower;
    }

    /// @param k: 掘ったマスの数 @param vS: (予想)石油埋蔵量 @param x: 観測値
    /// @return \log_2 P(x-0.5 < X < x+0.5 | k, vS)
    double _calcLog2(INT k, INT vS, INT x) {
        double ret = std::log2(calc(k, vS, x));
        if (ret < -INF) ret = -10000;
        return ret;
    }

    int trial(INT k, INT vS) {
        return max(0, int(round(myrand.normalvariate((k - vS) * eps + vS * (1 - eps),
                                                     std::sqrt(k * eps * (1 - eps))))));
    }
} gauss;

template <int N>
struct _Point {
    // 400<512=2^9を1単位としてdIdxArrayをハッシュする
    // (2^9)^7=2^63<2^64 かつ、7*3=21>20より、3つのuint64_tで表現可能
    tuple<uint64_t, uint64_t, uint64_t> ha;
    double likelihood;
    vc<INT> vSArray;

    // ランダムに選んだdIdxArrayをハッシュする
    _Point(const vvi& ranges, const vc<INT>& counts, int tIdxMain)
        : ha(0, 0, 0), likelihood(0) {
        for (int ofIdx = 0; ofIdx < M; ofIdx++)
            modifyHash(ofIdx, myrand.randRange(ranges[ofIdx].size()));
        if (tIdxMain != -1) {
            vSArray = toVSArray(counts, tIdxMain);
            likelihood = calcLikelihood(tIdxMain);
        }
    }

    // hashのofIdx番目の値を取得する
    int getHash(int ofIdx) const {
        if (ofIdx < 7) return (get<0>(ha) >> (9 * ofIdx)) & 511;
        if (ofIdx < 14) return (get<1>(ha) >> (9 * (ofIdx - 7))) & 511;
        return (get<2>(ha) >> (9 * (ofIdx - 14))) & 511;
    }
    // ofIdx番目の油田のdIdxをaftDIdxに変更する
    bool modify1(int ofIdx, int aftDIdx, int tIdxMain, const vc<INT>& counts,
                 const double threshold) {
        likelihood = 0.0;
        int befDIdx = getHash(ofIdx);
        for (int tIdx = 0; tIdx <= tIdxMain; tIdx++) {
            vSArray[tIdx] += counts[toCountIdx<N>(tIdx, ofIdx, aftDIdx)] -
                             counts[toCountIdx<N>(tIdx, ofIdx, befDIdx)];
            likelihood += gauss.calcLog2(tIdx, vSArray[tIdx]);
            if (likelihood < threshold) return false;
        }
        modifyHash(ofIdx, aftDIdx);
        // assert(vSArray == toVSArray(counts, tIdxMain));
        return true;
    }
    bool modify2(int ofIdx1, int ofIdx2, int aftDIdx1, int aftDIdx2, int tIdxMain,
                 const vc<INT>& counts, const double threshold) {
        likelihood = 0.0;
        int befDIdx1 = getHash(ofIdx1), befDIdx2 = getHash(ofIdx2);
        for (int tIdx = 0; tIdx <= tIdxMain; tIdx++) {
            vSArray[tIdx] += counts[toCountIdx<N>(tIdx, ofIdx1, aftDIdx1)] -
                             counts[toCountIdx<N>(tIdx, ofIdx1, befDIdx1)];
            vSArray[tIdx] += counts[toCountIdx<N>(tIdx, ofIdx2, aftDIdx2)] -
                             counts[toCountIdx<N>(tIdx, ofIdx2, befDIdx2)];
            likelihood += gauss.calcLog2(tIdx, vSArray[tIdx]);
            if (likelihood < threshold) return false;
        }
        modifyHash(ofIdx1, aftDIdx1);
        modifyHash(ofIdx2, aftDIdx2);
        // assert(vSArray == toVSArray(counts, tIdxMain));
        return true;
    }
    // 最新のtrialの結果を反映する
    void update(const vc<INT>& counts, int tIdxMain) {
        INT vS = 0;
        for (int ofIdx = 0; ofIdx < M; ofIdx++)
            vS += counts[toCountIdx<N>(tIdxMain, ofIdx, getHash(ofIdx))];
        vSArray.push_back(vS);
        assert(int(vSArray.size()) == tIdxMain + 1);
        likelihood += gauss.calcLog2(tIdxMain, vS);
        // assert(abs(likelihood - calcLikelihood(tIdxMain)) < 1e-5);
    }
    vi toDIdxArray() const {
        vi dIdxArray(M);
        for (int ofIdx = 0; ofIdx < M; ofIdx++) dIdxArray[ofIdx] = getHash(ofIdx);
        return dIdxArray;
    }
    array<int, N * N> toGrid(const vvi& ranges,
                             const vc<_OilField<N>>& oilFields) const {
        array<int, N * N> grid;
        grid.fill(0);
        for (int ofIdx = 0; ofIdx < M; ofIdx++) {
            int dIdx = ranges[ofIdx][getHash(ofIdx)];
            for (auto v : oilFields[ofIdx].ps) grid[v + dIdx]++;
        }
        return grid;
    }
    bitset<N * N> toBitset(const vvi& ranges, const vc<_OilField<N>>& oilFields) const {
        bitset<N * N> ret;
        ret.reset();
        for (int ofIdx = 0; ofIdx < M; ofIdx++) {
            int dIdx = ranges[ofIdx][getHash(ofIdx)];
            for (auto v : oilFields[ofIdx].ps) ret[v + dIdx] = true;
        }
        return ret;
    }

   private:
    // hashのofIdx番目の値をvalに変更する
    void modifyHash(int ofIdx, size_t val) {
        if (ofIdx < 7) {
            get<0>(ha) &= ~(511ll << (9 * ofIdx));
            get<0>(ha) |= uint64_t(val) << (9 * ofIdx);
        } else if (ofIdx < 14) {
            get<1>(ha) &= ~(511ll << (9 * (ofIdx - 7)));
            get<1>(ha) |= uint64_t(val) << (9 * (ofIdx - 7));
        } else {
            get<2>(ha) &= ~(511ll << (9 * (ofIdx - 14)));
            get<2>(ha) |= uint64_t(val) << (9 * (ofIdx - 14));
        }
        // assert(int(val) == getHash(ofIdx));
    }
    double calcLikelihood(int tIdxMain) const {
        double logLikelihood = 0.0;
        for (int tIdx = 0; tIdx <= tIdxMain; tIdx++)
            logLikelihood += gauss.calcLog2(tIdx, vSArray[tIdx]);
        return logLikelihood;
    }
    vc<INT> toVSArray(const vc<INT>& counts, int tIdxMain) const {
        vc<INT> vSArray2(tIdxMain + 1, 0);
        for (int tIdx = 0; tIdx <= tIdxMain; tIdx++)
            for (int ofIdx = 0; ofIdx < M; ofIdx++)
                vSArray2[tIdx] += counts[toCountIdx<N>(tIdx, ofIdx, getHash(ofIdx))];
        return vSArray2;
    }
};

// https://zenn.dev/siman/articles/e94f63246f6cb3
template <int N, size_t PQ_SIZE>
struct _MyPQ {
    bool isSorted;    // q[PQ_SIZE-1]が存在し、かつそれが最小であるか
    vc<_Point<N>> q;  // 0の位置に尤度最大のものが来る

    _MyPQ() : isSorted(false) { q.reserve(2 * PQ_SIZE); }

    void push(const _Point<N>& val) {
        // assert(!shouldSkip(val));
        q.push_back(val);
        if (q.size() > 2 * PQ_SIZE) {
            toUnique();
            sortQ();
        }
    }
    bool shouldSkip(const _Point<N>& val) {
        if (isSorted && q[PQ_SIZE - 1].likelihood > val.likelihood) return true;
        return false;
    }
    size_t size() const { return q.size(); }
    int updateLikelihoodAndCheckStatus(const vvi& ranges,
                                       const vc<_OilField<N>>& oilFields,
                                       const vc<INT>& sizes, const vc<INT>& responses,
                                       const vc<INT>& counts, int tIdxMain,
                                       array<int, N * N>& answer,
                                       const vc<array<int, N * N>>& taboo) {
        assert(!counts.empty() && q.size() >= 2);
        toUnique();
        for (_Point<N>& p : q) p.update(counts, tIdxMain);
        sortQ();
        bitset<N * N> bestShape = q[0].toBitset(ranges, oilFields);
        size_t idx = 1;
        double bestLikelihood = q[0].likelihood;
        while (idx < q.size()) {
            bitset<N * N> shape = q[idx].toBitset(ranges, oilFields);
            if (shape != bestShape) break;
            bestLikelihood =
                std::log2(std::exp2(bestLikelihood) + std::exp2(q[idx].likelihood));
            idx++;
        }
        assert(q[0].likelihood >= q[idx].likelihood);
        debug(bestLikelihood, q[idx].likelihood, q[0].toDIdxArray(),
              q[idx].toDIdxArray());
        debugGrid2<N>(q[0].toGrid(ranges, oilFields), q[idx].toGrid(ranges, oilFields));
        if (bestLikelihood - q[idx].likelihood > 10) {
            double validity =
                gauss.calcValidityOfVSArray(sizes, q[0].vSArray, responses);
            if (validity < -5.0)
                return -1;  // 信頼性が低い解が最適になってしまっている 異常終了
            answer.fill(0);
            vi dIdxArray = q[0].toDIdxArray();
            for (int ofIdx = 0; ofIdx < M; ofIdx++) {
                int pos = ranges[ofIdx][dIdxArray[ofIdx]];
                for (auto v : oilFields[ofIdx].ps) answer[v + pos]++;
            }
            if (count(ALL(taboo), answer) > 0) {
                answer.fill(-1);
                return -1;
            }
            return 1;  // 信頼性が高い解が見つかった 終了
        }
        return 0;  // 解の判別が未だ十分ではない 継続
    }
    void eraseSmallLikelihood() {
        toUnique();
        sortQ();
        q.erase(remove_if(ALL(q),
                          [&](const auto& p) {
                              return p.likelihood < q[0].likelihood - PARAM_THRESHOLD;
                          }),
                q.end());
        if (q.size() < PQ_SIZE) isSorted = false;
    }
    void sortQ() {
        sort(ALL(q), [](auto& a, auto& b) { return a.likelihood > b.likelihood; });
        while (q.size() > PQ_SIZE) q.pop_back();
        if (q.size() >= PQ_SIZE) isSorted = true;
    }
    void toUnique() {
        sort(ALL(q), [](auto& a, auto& b) { return a.ha < b.ha; });
        q.erase(unique(ALL(q), [](auto& a, auto& b) { return a.ha == b.ha; }), q.end());
    }
};

template <int N>
struct Solver {
    static const size_t PQ_SIZE = 2e3;
    static const int PARAM_LOOP = 2e3;

    template <class T>
    using Array = array<T, N * N>;
    using Island = _Island<N>;
    using OilField = _OilField<N>;
    using Point = _Point<N>;
    using MyPQ = _MyPQ<N, PQ_SIZE>;

    Array<int> answer;
    Array<int> actual;
    vc<OilField> oilFields;
    vc<unordered_set<int>> placesPerTrial;  // placesPerTrial[tIdx] = S = {idx}
    vc<INT> counts;     // counts[toCountIdx(tIdx,ofIdx,dIdx)] = # of 油田&S
    vc<INT> sizes;      // sizes[tIdx] = # of placesPerTrial[tIdx]
    vc<INT> responses;  // responses[tIdx] = 観測値
    vvi ranges;         // ranges[油田idx] = {dIdx}
    vc<tiii> neighbors;
    vc<Array<int>> taboo;
    int actualQueryCount;

    Solver() : actualQueryCount(0) {
        answer.fill(-1);
        actual.fill(-1);
        oilFields.resize(M);
        for (auto& of : oilFields) cin >> of;
        sort(ALL(oilFields), [&](const auto& a, const auto& b) { return a.d > b.d; });
        // debug(oilFields);
        solve();
        assert(all_of(ALL(ranges), [](const auto& r) { return is_sorted(ALL(r)); }));
    }

    int getRes() const {
        int res;
        cin >> res;
        return res;
    }
    int drill(int yx) {
        auto [y, x] = toYX(yx);
        cout << "q 1 " << y << " " << x << endl;
        actual[yx] = getRes();
        updateVariables({yx}, actual[yx]);
        return actual[yx];
    }
    int ask(const vi& askFields) {
        assert(2 <= int(askFields.size()));
        cout << "q " << askFields.size();
        for (auto idx : askFields)
            cout << " " << toYX(idx).first << " " << toYX(idx).second;
        cout << endl;
        int res = getRes();
        updateVariables(askFields, res);
        return res;
    }
    int ask(const unordered_set<int>& askFields) { return ask(vi(ALL(askFields))); }
    int writeAnswer() {
        assert(count(ALL(answer), -1) == 0);
        assert(count(ALL(taboo), answer) == 0);
        cout << "a " << N * N - count(ALL(answer), 0);
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (answer[toIdx(i, j)] > 0) cout << " " << i << " " << j;
        cout << endl;
        actualQueryCount++;
        int resp = getRes();
        if (resp != 1) taboo.push_back(answer);
        answer.fill(-1);
        return resp;
    }
    void updateVariables(const vi& askFields, int res) {
        assert(sizes.size() == responses.size() &&
               sizes.size() == placesPerTrial.size());
        sizes.push_back(askFields.size());
        responses.push_back(res);
        placesPerTrial.emplace_back(ALL(askFields));
        gauss.writeMemo(int(askFields.size()), res);
    }

    const int MAX_ISLANDS = 5e5;
    vc<Island> enumerateAllIslands() {
        double prodRange = prodRanges(ranges);
        assert(prodRange <= MAX_ISLANDS);
        vc<Island> islands(prodRange);
        vi indexes(prodRange);
        iota(ALL(indexes), 0);
        myrand.shuffle(indexes);
        vi positions(M, -1);
        auto dfs = [&](const auto& f, int depth) -> void {
            if (depth == M) {
                islands[indexes.back()] = Island(positions, oilFields);
                indexes.pop_back();
                return;
            }
            for (int dIdx : ranges[depth]) {
                positions[depth] = dIdx;
                f(f, depth + 1);
                positions[depth] = -1;
            }
        };
        dfs(dfs, 0);
        return islands;
    }

    double prodRanges(const vvi& ranges) const {
        double ret = 1.0;
        for (auto& r : ranges) ret *= r.size();
        return ret;
    }

    void updateAllIslands(vc<Island>& allIslands, const vi& askFields, int trialIdx) {
        assert(!any_of(ALL(allIslands), [&](const auto& island) {
            return std::isnan(island.likelihood);
        }));
        for (auto& island : allIslands) {
            INT vS = accumulate(ALL(askFields), 0, [&](int sum, int idx) {
                return sum + island.oil[idx];
            });
            island.likelihood += gauss.calcLog2(trialIdx, vS);
        }
        auto maxIter = max_element(ALL(allIslands), [](auto& a, auto& b) {
            return a.likelihood < b.likelihood;
        });
        double maxLikelihood = maxIter->likelihood;
        allIslands.erase(remove_if(ALL(allIslands),
                                   [&](const Island& island) {
                                       // これはbrute force用なので、-30.0を採用
                                       return island.likelihood < maxLikelihood - 30.0;
                                   }),
                         allIslands.end());
        maxIter = max_element(ALL(allIslands), [](auto& a, auto& b) {
            return a.likelihood < b.likelihood;
        });
        bitset<N * N> bestShape = maxIter->toBitset();
        if (all_of(ALL(allIslands), [&](const auto& island) {
                return island.toBitset() == bestShape;
            })) {
            allIslands = {*maxIter};
            return;
        }
        bool isFinished = true;  // 他の形の島で確率が2^-10以上のものがあれば続行
        int maxIterIdx = maxIter - allIslands.begin();
        for (int islandIdx = 0; islandIdx < int(allIslands.size()); islandIdx++) {
            if (islandIdx == maxIterIdx) continue;
            if (allIslands[islandIdx].likelihood > maxLikelihood - 10.0 &&
                allIslands[islandIdx].toBitset() != bestShape) {
                isFinished = false;
                break;
            }
        }
        if (isFinished) allIslands = {*maxIter};
    }

    void updateAllFoundIslands(MyPQ& pq, vc<Island>& allFoundIslands) {
        pq.eraseSmallLikelihood();
        allFoundIslands.clear();
        for (auto& p : pq.q) {  // already sorted by likelihood
            if (p.likelihood < pq.q[0].likelihood - 10.0) break;
            vi positions(M);
            for (int ofIdx = 0; ofIdx < M; ofIdx++)
                positions[ofIdx] = ranges[ofIdx][p.getHash(ofIdx)];
            allFoundIslands.emplace_back(p.likelihood, positions, oilFields);
        }
    }

    // allIslandはnormalizeProbされていることを前提とする
    double calcVarS(const vc<Island>& allIslands, const vi& askFields) {
        double expS1 = 0.0, expS2 = 0.0, probSum = 0.0;
        for (auto& island : allIslands) {
            double prob = island.getProb();
            probSum += prob;
            int S = accumulate(ALL(askFields), 0,
                               [&](int sum, int idx) { return sum + island.oil[idx]; });
            expS1 += prob * S;
            expS2 += prob * S * S;
        }
        assert(abs(probSum - 1.0) < 1e-5);
        return expS2 - expS1 * expS1;
    }

    double calcEfficientS(int k, const double varS) {
        // 効率 := √k/2 log(1+var(X)(1-2ε)^2/kε(1-ε))
        return std::sqrt(k) / 2 *
               std::log2(1 +
                         pow(varS, 1.0) * pow(1 - 2 * eps, 2) / (k * eps * (1 - eps)));
    }

    // E[v(S)*u]
    Array<double> computeExpSUs(const vc<Island>& allIslands, const vi& askFields) {
        Array<double> expSUs;
        expSUs.fill(0.0);  // 本来askFields内の箇所はNaN
        for (int islandIdx = 0; islandIdx < int(allIslands.size()); islandIdx++) {
            const auto& island = allIslands[islandIdx];
            double coeff = island.getProb();
            if (abs(coeff) * N * N < 1e-30) continue;
            int cnt = accumulate(ALL(askFields), 0, [&](int sum, int idx) {
                return sum + island.oil[idx];
            });
            if (cnt == 0) continue;
            coeff *= cnt;
            for (int ofIdx = 0; ofIdx < M; ofIdx++)
                for (auto& v : oilFields[ofIdx].ps)
                    expSUs[v + island.positions[ofIdx]] += coeff;
        }
        return expSUs;
    }

    bool decideAskAreaOptimization(bool doExpand, vi& askFields, set<int>& otherIndexes,
                                   vi& lastAddedVs, const Array<double>& exp1,
                                   Array<double>& expSUs, double& expSumOfS,
                                   const Array<double>& varGrid, double& bestVarS,
                                   double& bestEfficientS, const vc<Island>& allIslands,
                                   const int addSizeUb) {
        // 確率勾配法的に局所改善を繰り返す
        // Maximize Var[S+u] = Var[S]+Var[u]+2Cov[S, u]
        //                   = Var[S]+Var[u]+2(E[Su]-(\sum_{v \in S}E[v])E[u])
        assert(1 <= addSizeUb);
        int nowRemovedV = -1;
        if (!doExpand) {  // Sのマスを1つ減らす or 1つ移動する
            assert(abs(bestVarS - calcVarS(allIslands, askFields)) < 1e-5 * bestVarS);
            nowRemovedV = myrand.choice(askFields);
            askFields.erase(find(ALL(askFields), nowRemovedV));
            expSumOfS -= exp1[nowRemovedV];
            otherIndexes.insert(nowRemovedV);
        }
        for (const auto& island : allIslands) {
            double coeff = island.getProb();
            if (abs(coeff) * addSizeUb < 1e-30) continue;
            double diff =
                (accumulate(ALL(lastAddedVs), 0,
                            [&](int sum, int idx) { return sum + island.oil[idx]; }) -
                 (nowRemovedV == -1 ? 0 : island.oil[nowRemovedV]));
            if (diff == 0) continue;
            coeff *= diff;
            for (int ofIdx = 0; ofIdx < M; ofIdx++)
                for (auto& v : oilFields[ofIdx].ps)
                    expSUs[v + island.positions[ofIdx]] += coeff;
        }
        vc<pair<double, int>> goodUs;
        double varS = bestVarS;
        if (nowRemovedV != -1) {
            varS = bestVarS - varGrid[nowRemovedV] -
                   2 * (expSUs[nowRemovedV] - expSumOfS * exp1[nowRemovedV]);
            // assert(abs(varS - calcVarS(allIslands, askFields)) < 1e-5 * varS);
        }
        for (int u : otherIndexes) {
            double V =
                max(1e-100, varS + varGrid[u] + 2 * (expSUs[u] - expSumOfS * exp1[u]));
            if (bestEfficientS <= calcEfficientS(askFields.size() + 1, V)) {
                goodUs.emplace_back(V, u);
            }
        }
        if (goodUs.empty()) {
            // どれを追加しても効率は上がらなかった場合
            lastAddedVs = {};
            if (nowRemovedV == -1) return false;
            if (chmaxEq(bestEfficientS, calcEfficientS(askFields.size(), varS))) {
                bestVarS = varS;
                return true;
            }
            assert(abs(bestVarS -
                       (varS + varGrid[nowRemovedV] +
                        2 * (expSUs[nowRemovedV] - expSumOfS * exp1[nowRemovedV]))) <
                   1e-5 * bestVarS);
            askFields.push_back(nowRemovedV);
            lastAddedVs = {nowRemovedV};
            expSumOfS += exp1[nowRemovedV];
            otherIndexes.erase(nowRemovedV);
            return false;
        }
        sort(ALL(goodUs), greater<pair<double, int>>());
        goodUs.resize(min(addSizeUb, int(goodUs.size())));
        vi maxUs;
        for (auto& p : goodUs) maxUs.push_back(p.second);
        assert(1 <= int(maxUs.size()) && int(maxUs.size()) <= addSizeUb);
        askFields.insert(askFields.end(), ALL(maxUs));
        bestVarS =
            (maxUs.size() == 1)
                ? max(1e-100, varS + varGrid[maxUs[0]] +
                                  2 * (expSUs[maxUs[0]] - expSumOfS * exp1[maxUs[0]]))
                : calcVarS(allIslands, askFields);
        bestEfficientS = calcEfficientS(askFields.size(), bestVarS);
        lastAddedVs = maxUs;
        expSumOfS += accumulate(ALL(maxUs), 0.0,
                                [&](double sum, int idx) { return sum + exp1[idx]; });
        assert(
            abs(expSumOfS - accumulate(ALL(askFields), 0.0, [&](double sum, int idx) {
                    return sum + exp1[idx];
                })) < (1e-5) * expSumOfS);
        for (auto u : maxUs) otherIndexes.erase(u);
        return doExpand ? true
                        : (goodUs.size() == 1 && goodUs[0].second != nowRemovedV);
    }
    // allIslandはprobのみ更新される
    vi decideAskArea(vc<Island>& allIslands, const int TRIAL_NUM) {
        // 最大効率のSを占う
        assert(int(allIslands.size()) >= 2);
        Array<double> exp1, exp2;
        exp1.fill(0.0);
        exp2.fill(0.0);
        double probSum = 0.0;
        double maxLikelihood = (*max_element(ALL(allIslands), [](auto& a, auto& b) {
                                   return a.likelihood < b.likelihood;
                               })).likelihood;
        for (auto& island : allIslands) {
            island.setProb(maxLikelihood);
            probSum += island.getProb();
            for (int idx = 0; idx < N * N; idx++) {
                exp1[idx] += island.getProb() * island.oil[idx];
                exp2[idx] += island.getProb() * pow(island.oil[idx], 2);
            }
        }
        for (auto& island : allIslands) island.normalizeProb(probSum);
        for (int idx = 0; idx < N * N; idx++) {
            exp1[idx] /= probSum;
            exp2[idx] /= probSum;
        }
        Array<double> varGrid;  // 分散公式 Var[X] = E[X^2] - E[X]^2
        varGrid.fill(0.0);
        for (int idx = 0; idx < N * N; idx++)
            varGrid[idx] = exp2[idx] - pow(exp1[idx], 2);
        visGrid<double, N>(varGrid);

        vi askFields = {int(distance(varGrid.begin(), max_element(ALL(varGrid))))};
        set<int> otherIndexes;
        for (int i = 0; i < N * N; i++) otherIndexes.insert(i);
        otherIndexes.erase(askFields[0]);
        double expSumOfS = exp1[askFields[0]];
        double bestVarS = calcVarS(allIslands, askFields);
        double bestEfficientS = calcEfficientS(1, bestVarS);
        assert(abs(bestVarS - varGrid[askFields[0]]) < 1e-5 * bestVarS);
        Array<double> expSUs = computeExpSUs(allIslands, askFields);
        vi lastAddedVs;
        while (true) {
            bool res = decideAskAreaOptimization(
                true, askFields, otherIndexes, lastAddedVs, exp1, expSUs, expSumOfS,
                varGrid, bestVarS, bestEfficientS, allIslands, 5);
            if (!res) break;
        }
        int failCount = 0;
        for (int trialIdx = 0; trialIdx < TRIAL_NUM; trialIdx++) {
            bool doExpand = (trialIdx == failCount ? false : myrand.randBool(0.5));
            bool res = decideAskAreaOptimization(
                doExpand, askFields, otherIndexes, lastAddedVs, exp1, expSUs, expSumOfS,
                varGrid, bestVarS, bestEfficientS, allIslands, 2);
            if (!res) failCount++;
            if (failCount >= 10) break;
        }
        debug(askFields.size(), bestVarS, bestEfficientS);
#ifndef NDEBUG
        // assert(
        //     abs(expSumOfS - accumulate(ALL(askFields), 0.0, [&](double sum, int idx)
        //     {
        //             return sum + exp1[idx];
        //         })) < (1e-5) * expSumOfS);
        // assert(abs(calcVarS(allIslands, askFields) - bestVarS) < (1e-5) * bestVarS);
        // assert(abs(calcEfficientS(askFields.size(), bestVarS) - bestEfficientS) <
        //        (1e-5) * bestEfficientS);
        // for (int islandIdx = 0; islandIdx < int(allIslands.size()); islandIdx++) {
        //     const auto& island = allIslands[islandIdx];
        //     double coeff = island.getProb() *
        //                    accumulate(ALL(lastAddedVs), 0, [&](int sum, int idx) {
        //                        return sum + island.oil[idx];
        //                    });
        //     if (abs(coeff) < 1e-50) continue;
        //     for (int ofIdx = 0; ofIdx < M; ofIdx++)
        //         for (auto& v : oilFields[ofIdx].ps)
        //             expSUs[v + island.positions[ofIdx]] += coeff;
        // }
        // Array<double> expSUs2 = computeExpSUs(allIslands, askFields);
        // for (int i = 0; i < N * N; i++) assert(abs(expSUs[i] - expSUs2[i]) <= 1e-5);
#endif
        return askFields;
    }
    void solveBruteForceBayesian() {
        vc<Island> allIslands = enumerateAllIslands();
        bitset<N * N> bestShape = allIslands[0].toBitset();
        if (all_of(ALL(allIslands), [&](const auto& island) {
                return island.toBitset() == bestShape;
            })) {
            answer = allIslands[0].oil;
            return;
        }
        int trialIdx = -1;
        int tempSZ = 1e4;
        while (true) {
            if (timer.ms() > TL - 1000) tempSZ = 1e3;
            debug(allIslands.size(), timer.ms());
            vi askFields;
            if (int(allIslands.size()) < tempSZ) {
                askFields = decideAskArea(allIslands, 100);
            } else {
                nth_element(allIslands.begin(), allIslands.begin() + tempSZ,
                            allIslands.end(), [&](const auto& a, const auto& b) {
                                return a.likelihood > b.likelihood;
                            });
                vc<Island> tempIslands(allIslands.begin(), allIslands.begin() + tempSZ);
                askFields = decideAskArea(tempIslands, 100);
            }
            trialIdx++;
            assert(trialIdx < 2 * N * N);
            (int(askFields.size()) == 1) ? drill(askFields[0]) : ask(askFields);
            updateAllIslands(allIslands, askFields, trialIdx);
            if (allIslands.size() == 1) {
                answer = allIslands[0].oil;
                break;
            }
        }
    }

    int solveBayesianFortuneTelling(MyPQ& pq, vc<Island>& allFoundIslands,
                                    int tIdxMain) {
        vi askFields = decideAskArea(allFoundIslands, 30);
        if (int(askFields.size()) == 1)
            drill(askFields[0]);
        else
            ask(askFields);
        counts.resize(counts.size() + M * N * N);
        for (int ofIdx = 0; ofIdx < M; ofIdx++) {
            for (int dIdx = 0; dIdx < int(ranges[ofIdx].size()); dIdx++) {
                counts[toCountIdx<N>(tIdxMain, ofIdx, dIdx)] =
                    count_if(ALL(oilFields[ofIdx].ps), [&](int v) {
                        return placesPerTrial[tIdxMain].count(v + ranges[ofIdx][dIdx]);
                    });
            }
        }
        return pq.updateLikelihoodAndCheckStatus(ranges, oilFields, sizes, responses,
                                                 counts, tIdxMain, answer, taboo);
    }

    int getIndexForRanges(int ofIdx, int val, int diff) const {
        // ranges[ofIdx][ret]=valInRanges
        assert(0 <= ofIdx && ofIdx < M);
        auto [y, x] = toYX(val);
        auto [dy, dx] = toYX(diff);
        if (!isInside(y + dy, x + dx)) return -1;
        int nyx = toIdx(y + dy, x + dx);
        auto iter = lower_bound(ALL(ranges[ofIdx]), nyx);
        if (iter == ranges[ofIdx].end() || *iter != nyx) return -1;
        return iter - ranges[ofIdx].begin();
    }
    void makeNeighbors() {
        // i,j,dIdx \in neighbors
        // if i != j:
        // (ranges[i][dIdxArray[i]]=I, ranges[j][dIdxArray[j]]=J) =>
        // (ranges[i][dIdxArray[i]]=J-dIdx, ranges[j][dIdxArray[j]]=I+dIdx)
        // if i == j:
        // (ranges[i][dIdxArray[i]]=I) => (ranges[i][dIdxArray[i]]=I+dIdx)
        assert(neighbors.empty());
        for (int i = 0; i < M; i++)
            for (int j = i; j < M; j++) {
                if (i != j && oilFields[i].ps == oilFields[j].ps) continue;
                const auto &psI = oilFields[i].ps, psJ = oilFields[j].ps;
                for (int dx = -5; dx <= +5; dx++)
                    for (int dy = -5; dy <= +5; dy++) {
                        int dIdx = toIdx(dy, dx);
                        if (i == j && dIdx == 0) continue;
                        vi psJ2 = psJ;
                        for (auto& p : psJ2) p += dIdx;
                        assert(is_sorted(ALL(psI)) && is_sorted(ALL(psJ2)));
                        vi common;
                        set_intersection(ALL(psI), ALL(psJ2), back_inserter(common));
                        if (common.size() > 0.5 * max(psI.size(), psJ2.size())) {
                            neighbors.emplace_back(i, j, dIdx);
                            break;
                        }
                    }
            }
    }
    pair<bool, Point> swapNeighbor(const Point& p, const int tIdxMain,
                                   const double threshold, const double neighborProb) {
        if (myrand.randBool(neighborProb)) {
            int i, j, i2, j2;
            int cnt = 0;
            while (++cnt <= 3) {
                int dIdx;
                tie(i, j, dIdx) = myrand.choice(neighbors);
                if (i != j) {
                    int I = ranges[i][p.getHash(i)], J = ranges[j][p.getHash(j)];
                    if (I == J - dIdx && J == I + dIdx) continue;
                    i2 = getIndexForRanges(i, J, -dIdx);
                    if (i2 == -1) continue;
                    j2 = getIndexForRanges(j, I, +dIdx);
                    if (j2 == -1) continue;
                    Point newP = p;
                    bool ok = newP.modify2(i, j, i2, j2, tIdxMain, counts, threshold);
                    return {ok, newP};
                } else {
                    int I = ranges[i][p.getHash(i)];
                    i2 = getIndexForRanges(i, I, +dIdx);
                    if (i2 == -1) continue;
                    Point newP = p;
                    bool ok = newP.modify1(i, i2, tIdxMain, counts, threshold);
                    return {ok, newP};
                }
            };
        }
        int ofIdx, aftDIdx;
        do {
            ofIdx = myrand.randRange(M);
            aftDIdx = myrand.randRange(ranges[ofIdx].size());
        } while (p.getHash(ofIdx) == aftDIdx);
        Point newP = p;
        bool ok = newP.modify1(ofIdx, aftDIdx, tIdxMain, counts, threshold);
        return {ok, newP};
    }

    void localOptimization(Point p, MyPQ& pq, const int tIdxMain,
                           const double threshold, const int OPT_COUNT,
                           const double neighborProb) {
        if (!pq.shouldSkip(p)) pq.push(p);
        for (int _ = 0; _ < OPT_COUNT; _++) {
            auto [ok, newP] = swapNeighbor(p, tIdxMain, threshold, neighborProb);
            if (!ok) continue;
            if (!pq.shouldSkip(newP)) pq.push(newP);
            if (p.likelihood <= newP.likelihood) swap(p, newP);
        }
    }

    // https://qiita.com/not522/items/cd20b87157d15850d31c
    void simulatedAnnealing(const int tIdxMain, MyPQ& pq) {
        assert(int(pq.size()) > 0);
        Point p = pq.q[0];
        assert(p.likelihood == (*max_element(ALL(pq.q), [](auto& a, auto& b) {
                                   return a.likelihood < b.likelihood;
                               })).likelihood);
        const int LOOP_COUNT = 3 * PARAM_LOOP;
        const int FIRST_TEMP = +3;
        const int LAST_TEMP = -5;
        for (int loopIdx = 0; loopIdx < LOOP_COUNT; loopIdx++) {
            double temp =
                pow(2, FIRST_TEMP + (LAST_TEMP - FIRST_TEMP) * loopIdx / LOOP_COUNT);
            double threshold = p.likelihood + temp * std::log(myrand.random());
            auto [ok, newP] = swapNeighbor(p, tIdxMain, threshold, 0.5);
            if (!ok) continue;
            if (newP.likelihood >= p.likelihood - PARAM_THRESHOLD &&
                !pq.shouldSkip(newP))
                pq.push(newP);
            swap(p, newP);
        }
    }

    void solveBayesian() {
        makeNeighbors();
        vc<Island> allFoundIslands;  // 占いの決め方に用いる島の集合 確率正規化不要
        MyPQ pq;  // 尤度上位のみを保持 正規化してはいけない
        int tIdxMain = -1;
        while (true) {
            debug(gauss.calcValidityOfVSArray(sizes, pq.q[0].vSArray, responses,
                                              esc::BLUE));
            double maxLikelihood = (int(pq.size()) > 0) ? pq.q[0].likelihood : 0.0;
            assert(int(pq.size()) == 0 ||
                   maxLikelihood == (*max_element(ALL(pq.q), [](auto& a, auto& b) {
                                        return a.likelihood < b.likelihood;
                                    })).likelihood);
            double threshold = maxLikelihood - PARAM_THRESHOLD;
            // Maximize_{dIdx}
            //  \sum_{tIdx} calcLog2(sizes[tIdx], v(S), responses[tIdx])
            // Subject to
            //  v(S) := \sum_{ofIdx} counts[tIdx][ofIdx][dIdx]
            if (tIdxMain <= 3) {  // 最初数ターンは殆ど探索せずにランダムに選べば良い
                // 山登り法による局所最適化
                for (int _ = 0; _ < 2 * PARAM_LOOP; _++) {
                    Point p(ranges, counts, tIdxMain);
                    localOptimization(p, pq, tIdxMain, threshold, 5, 0.0);
                }
            } else {  // 後半のメインは焼き鈍し法 焼き鈍しの途中経過を全てpqへ
                vc<Point> tempPs(ALL(pq.q));
                // 既に判明している箇所+その周辺を探索
                for (Point& p : tempPs) {
                    localOptimization(p, pq, tIdxMain, threshold, 10, 0.1);
                }
                pq.sortQ();
                simulatedAnnealing(tIdxMain, pq);
            }
            updateAllFoundIslands(pq, allFoundIslands);
            if (timer.ms() > TL - 200) break;
            tIdxMain++;
            assert(tIdxMain <= 2 * N * N);
            debug(tIdxMain, pq.size(), allFoundIslands.size());

            int status;
            if (int(allFoundIslands.size()) > 1) {
                // 占いと尤度の更新
                status = solveBayesianFortuneTelling(pq, allFoundIslands, tIdxMain);
            } else {
                assert(!allFoundIslands.empty());
                double validity =
                    gauss.calcValidityOfVSArray(sizes, pq.q[0].vSArray, responses);
                status = validity < -5.0 ? -2 : 1;
                if (status == 1) {
                    answer = pq.q[0].toGrid(ranges, oilFields);
                    if (count(ALL(taboo), answer) > 0) {
                        answer.fill(-1);
                        status = -2;
                    }
                }
            }

            if (status == 1) {
                int resp = writeAnswer();
                if (resp == 1)
                    return;
                else if (int(allFoundIslands.size()) <= 1) {
#ifdef hari64
                    esc::color(esc::RED, "failed\n");
#endif
                    break;
                }
            } else if (status == -1) {
                // todo? 信頼性が低い解が最適だが他にも候補自体はある
            } else if (status == -2) {
                break;
            }
            if (timer.ms() > TL - 200 || tIdxMain >= 1.25 * N * N) break;
        }
        solveWorstCase(allFoundIslands);
    }

    pair<bool, Array<double>> calcExpForWorstCase(vc<Island>& allFoundIslands) {
        Array<double> exp;
        exp.fill(0.0);
        if (allFoundIslands.empty()) return {false, exp};
        allFoundIslands.erase(
            remove_if(ALL(allFoundIslands),
                      [&](const Island& island) {
                          int fail = 0;
                          int success = 0;
                          for (int idx = 0; idx < N * N; idx++) {
                              if (actual[idx] == -1) continue;
                              if (actual[idx] == 0 && island.oil[idx] > 0) {
                                  fail++;
                                  continue;
                              }
                              if (actual[idx] > 0 && island.oil[idx] == 0) {
                                  fail++;
                                  continue;
                              }
                              success++;
                          }
                          return fail > 0.1 * success;
                      }),
            allFoundIslands.end());
        if (allFoundIslands.empty()) return {false, exp};
        double maxLikelihood =
            (*max_element(ALL(allFoundIslands), [](auto& a, auto& b) {
                return a.likelihood < b.likelihood;
            })).likelihood;
        for (auto& island : allFoundIslands) island.setProb(maxLikelihood);
        double probSum = 0.0;
        for (auto& island : allFoundIslands) {
            double prob = island.getProb();
            probSum += prob;
            for (int ofIdx = 0; ofIdx < M; ofIdx++)
                for (int v : oilFields[ofIdx].ps)
                    exp[v + island.positions[ofIdx]] += prob;
        }
        for (int idx = 0; idx < N * N; idx++) exp[idx] /= probSum;
        debug("calcExpForWorstCase finished." + to_string(allFoundIslands.size()));
        visGrid<double, N>(exp);
        return {true, exp};
    }
    void solveWorstCase(vc<Island>& allFoundIslands) {
#ifdef hari64
        esc::color(esc::RED, "Worst solution start.\n");
#endif
        debug(allFoundIslands.size());
        Array<double> exp = calcExpForWorstCase(allFoundIslands).second;
        int totalOil = accumulate(
            ALL(oilFields), 0, [&](int sum, const OilField& of) { return sum + of.d; });
        int foundOil = accumulate(ALL(actual), 0, [&](int sum, int oil) {
            return sum + (oil == -1 ? 0 : oil);
        });
        vi sortedIdxByExp(N * N);
        iota(ALL(sortedIdxByExp), 0);
        sort(ALL(sortedIdxByExp), [&](int a, int b) { return exp[a] < exp[b]; });
        bool hasDrilledZero = false;
        int updateOilNum = 10;
        answer.fill(-1);
        while (!sortedIdxByExp.empty()) {
            int idx = sortedIdxByExp.back();
            sortedIdxByExp.pop_back();
            if (actual[idx] == -1) {
                bool ok = !hasDrilledZero;
                if (!ok) {
                    auto [y, x] = toYX(idx);
                    for (int k = 0; k < 4; k++) {
                        int ny = y + DY[k], nx = x + DX[k];
                        if (!isInside(ny, nx)) continue;
                        int nIdx = toIdx(ny, nx);
                        if (actual[nIdx] > 0) {
                            ok = true;
                            break;
                        }
                    }
                }
                if (!ok) {
                    int newIdx = -1;
                    for (int idx2 = 0; idx2 < N * N; idx2++) {
                        if (actual[idx2] != -1) continue;
                        auto [y, x] = toYX(idx2);
                        for (int k = 0; k < 4; k++) {
                            int ny = y + DY[k], nx = x + DX[k];
                            if (!isInside(ny, nx)) continue;
                            int nIdx = toIdx(ny, nx);
                            if (actual[nIdx] > 0) {
                                if (newIdx == -1) newIdx = idx2;
                                if (exp[idx2] > exp[newIdx]) newIdx = idx2;
                                break;
                            }
                        }
                    }
                    if (newIdx != -1) {
                        sortedIdxByExp.push_back(idx);
                        sortedIdxByExp.push_back(newIdx);
                        continue;
                    }
                }
                answer[idx] = drill(idx);
                foundOil += answer[idx];
                if (foundOil >= totalOil) break;
                if (int(sizes.size()) == 2 * N * N - actualQueryCount - 1) break;
                if (timer.ms() < TL - 50 && foundOil > updateOilNum) {
                    updateOilNum += 10;
                    auto [ok, newExp] = calcExpForWorstCase(allFoundIslands);
                    if (ok) {
                        swap(exp, newExp);
                        sort(ALL(sortedIdxByExp),
                             [&](int a, int b) { return exp[a] < exp[b]; });
                    }
                }
                if (answer[idx] == 0) hasDrilledZero = true;
            } else {
                answer[idx] = actual[idx];
            }
        }
        for (int idx = 0; idx < N * N; idx++)
            if (answer[idx] == -1) answer[idx] = 0;
        int resp = writeAnswer();
        if (resp != 1) {
            esc::color(esc::RED, "failed to solve");
            while (true) drill(0);
        }
    }

    pair<Array<double>, Array<double>> solveInit() {
        assert(ranges.empty() && int(oilFields.size()) == M);
        ranges.resize(M);
        vc<Array<double>> vecOfExpInit;
        for (int ofIdx = 0; ofIdx < M; ofIdx++) {
            auto& of = oilFields[ofIdx];
            auto& range = ranges[ofIdx];
            vecOfExpInit.emplace_back();
            vecOfExpInit[ofIdx].fill(0.0);
            for (int dy = 0; dy < N; dy++)
                for (int dx = 0; dx < N; dx++)
                    if (isInside(of.maxY + dy, of.maxX + dx)) {
                        range.push_back(toIdx(dy, dx));
                        for (auto& idx : of.ps)
                            vecOfExpInit[ofIdx][idx + range.back()] += 1.0;
                    }
            assert(is_sorted(ALL(range)));
            for (int idx = 0; idx < N * N; idx++)
                vecOfExpInit[ofIdx][idx] /= range.size();
        }
        Array<double> expInit;
        Array<double> varInit;
        varInit.fill(0.0);
        for (int idx = 0; idx < N * N; idx++) {
            for (int ofIdx = 0; ofIdx < M; ofIdx++) {
                expInit[idx] += vecOfExpInit[ofIdx][idx];
                varInit[idx] +=
                    vecOfExpInit[ofIdx][idx] * (1 - vecOfExpInit[ofIdx][idx]);
            }
        }
        visGrid<double, N>(varInit);
        return {expInit, varInit};
    }
    void solve() {
        // 各油田毎にどの範囲に置けるかを表す配列
        [[maybe_unused]] auto [expInit, varInit] = solveInit();
        double prod = prodRanges(ranges);
        debug(prod);
        if (prod < MAX_ISLANDS) {
            solveBruteForceBayesian();
            int resp = writeAnswer();
            if (resp != 1)
                while (int(sizes.size()) < 2 * N * N - actualQueryCount) drill(0);
        } else {
            solveBayesian();
            // for (int idx = 0; idx < N * N; idx++) answer[idx] = drill(idx);
            // int resp = writeAnswer();
            // if (resp != 1) throw runtime_error("solveNaive failed");
        }
        [[maybe_unused]] int lowerBoundOfScore =
            int(round(1e6 * std::log2(prod) / std::sqrt(N * N - 1) /
                      (1 + eps * std::log2(eps) + (1 - eps) * std::log2(1 - eps))));
        debug(lowerBoundOfScore);
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(false);
    timer.start();
    cin >> _N >> M >> eps;
    // clang-format off
    switch (_N) {
        case 10: Solver<10>(); break;
        case 11: Solver<11>(); break;
        case 12: Solver<12>(); break;
        case 13: Solver<13>(); break;
        case 14: Solver<14>(); break;
        case 15: Solver<15>(); break;
        case 16: Solver<16>(); break;
        case 17: Solver<17>(); break;
        case 18: Solver<18>(); break;
        case 19: Solver<19>(); break;
        case 20: Solver<20>(); break;
        default: throw runtime_error("invalid N");
    }
    // clang-format on
    timer.stop();
#ifdef hari64
    esc::color(esc::AQUA, timer.report());
    cerr << endl;
#endif
    return 0;
}
