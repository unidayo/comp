// ============== main3.cpp ==============
#pragma GCC Optimize(O3)
#define NDEBUG
// ============== tsp2.hpp ==============
// ============== macro3_core.hpp ==============
// ============== define2.hpp ==============
#include <bits/stdc++.h>
// ============== library/timer.hpp ==============

#include <chrono>

namespace library{

class Timer {
private:
    std::chrono::steady_clock::time_point start_time;
    double scale = 1.0;

public:
    Timer() : start_time(std::chrono::steady_clock::now()) {}

    void reset() {
        start_time = std::chrono::steady_clock::now();
    }

    void set_scale(double s) {
        scale = s;
    }

    double elapsed() const {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = now - start_time;
        return diff.count() * scale;
    }
};

}
// ============== End of library/timer.hpp ==============


// ============== library/rng.hpp ==============

#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <tuple>
#include <utility>

namespace library {

// ============================================================
// utilities
// ============================================================

constexpr uint64_t rotl64(uint64_t x, int k) noexcept
{
    return (x << k) | (x >> (64 - k));
}

constexpr uint64_t splitmix(uint64_t z){
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

constexpr uint64_t splitmix64_next(uint64_t& x) noexcept
{
    x += 0x9E3779B97F4A7C15ULL;
    return splitmix(x);
}

inline uint64_t random_chrono()
{
    uint64_t t =
        static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
    return splitmix64_next(t);
}

// ============================================================
// base class / concept
// ============================================================

struct rng_base_u64 {
    using result_type = uint64_t;

    static constexpr result_type min() noexcept
    {
        return 0;
    }

    static constexpr result_type max() noexcept
    {
        return std::numeric_limits<result_type>::max();
    }
};

// ============================================================
// xorplus
// - 超低品質、超高速乱数
// ============================================================

class xorplus : public rng_base_u64 {
private:
    uint64_t x1 = 0;
    uint64_t x2 = 0;

public:
    constexpr explicit xorplus(uint64_t seed = 1) noexcept
    {
        this->seed(seed);
    }

    constexpr void seed(uint64_t seed_val) noexcept
    {
        uint64_t x = seed_val;
        x1 = splitmix64_next(x);
        x2 = splitmix64_next(x);
        if (x1 == 0 && x2 == 0) {
            seed(seed_val + 1);
        }
    }

    constexpr result_type operator()() noexcept
    {
        x1 += x2;
        x2 ^= x1;
        return x1;
    }
};

// ============================================================
// xoroplus
// - 中低品質(?) 、高速
// - xorplusと比べれば大幅に高品質 (randint3テスト、ベルヌーイテストともに良好)
// ============================================================

class xoroplus : public rng_base_u64 {
private:
    uint64_t x1 = 0;
    uint64_t x2 = 0;

public:
    constexpr explicit xoroplus(uint64_t seed = 1) noexcept
    {
        this->seed(seed);
    }

    constexpr void seed(uint64_t seed_val) noexcept
    {
        uint64_t x = seed_val;
        x1 = splitmix64_next(x);
        x2 = splitmix64_next(x);
        if (x1 == 0 && x2 == 0) {
            seed(seed_val + 1);
        }
    }

    constexpr result_type operator()() noexcept
    {
        x1 += rotl64(x2, 17);
        x2 ^= x1;
        return x1;
    }
};

// ============================================================
// xoshiro256
// - 高速で品質もそこそこな乱数
// ============================================================

class xoshiro256 : public rng_base_u64 {
private:
    uint64_t s_[4]{};
public:
    constexpr explicit xoshiro256(uint64_t seed = 1) noexcept
    {
        this->seed(seed);
    }

    constexpr void seed(uint64_t seed_val) noexcept
    {
        uint64_t x = seed_val;
        s_[0] = splitmix64_next(x);
        s_[1] = splitmix64_next(x);
        s_[2] = splitmix64_next(x);
        s_[3] = splitmix64_next(x);

        if (s_[0] == 0 && s_[1] == 0 && s_[2] == 0 && s_[3] == 0) {
            s_[3] = 1;
        }
    }

    constexpr result_type operator()() noexcept
    {
        step();
        return s_[0];
    }

    constexpr void step() noexcept
    {
        const uint64_t t = s_[1] << 17;

        s_[2] ^= s_[0];
        s_[3] ^= s_[1];
        s_[1] ^= s_[2];
        s_[0] ^= s_[3];

        s_[2] ^= t;
        s_[3] = rotl64(s_[3], 45);
    }
    constexpr uint64_t get(int i) const noexcept {
        return s_[i];
    }
};

// ============================================================
// xoshiro256++
// - 高品質な乱数（ただしやや遅い）
// ============================================================

class xoshiro256pp : public rng_base_u64 {
private:
    uint64_t s_[4]{};

public:
    constexpr explicit xoshiro256pp(uint64_t seed = 1) noexcept
    {
        this->seed(seed);
    }

    constexpr void seed(uint64_t seed_val) noexcept
    {
        uint64_t x = seed_val;
        s_[0] = splitmix64_next(x);
        s_[1] = splitmix64_next(x);
        s_[2] = splitmix64_next(x);
        s_[3] = splitmix64_next(x);

        if (s_[0] == 0 && s_[1] == 0 && s_[2] == 0 && s_[3] == 0) {
            seed(seed_val + 1);
        }
    }

    constexpr result_type operator()() noexcept
    {
        const uint64_t result = rotl64(s_[0] + s_[3], 23) + s_[0];
        const uint64_t t = s_[1] << 17;

        s_[2] ^= s_[0];
        s_[3] ^= s_[1];
        s_[1] ^= s_[2];
        s_[0] ^= s_[3];

        s_[2] ^= t;
        s_[3] = rotl64(s_[3], 45);

        return result;
    }
};

// ============================================================
// randint helpers
// - 64bit RNG 専用
// ============================================================

inline int randint(int r, uint64_t rng_val)
{
    return static_cast<int>((static_cast<uint64_t>(rng_val >> 32) * static_cast<uint64_t>(r)) >> 32);
}

inline int randint(int l, int r, uint64_t rng_val)
{
    return l + randint(r - l, rng_val);
}

inline std::pair<int, int> randint_pair(int r, uint64_t rng_val)
{
    const uint64_t v = rng_val;
    return {
        static_cast<int>(((v >> 32) * static_cast<uint64_t>(r)) >> 32),
        static_cast<int>(((v & 0xFFFFFFFFULL) * static_cast<uint64_t>(r)) >> 32)
    };
}

inline std::pair<int, int> randint_pair(int l, int r, uint64_t rng_val)
{
    const uint64_t v = rng_val;
    const uint64_t d = static_cast<uint64_t>(r - l);
    return {
        l + static_cast<int>(((v >> 32) * d) >> 32),
        l + static_cast<int>(((v & 0xFFFFFFFFULL) * d) >> 32)
    };
}

inline std::tuple<int, int, int> randint3(int r, uint64_t rng_val)
{
    const uint64_t v1 = rng_val >> 42;
    const uint64_t v2 = (rng_val >> 21) & 0x1FFFFFULL;
    const uint64_t v3 = rng_val & 0x1FFFFFULL;
    const uint64_t rr = static_cast<uint64_t>(r);

    return {
        static_cast<int>((v1 * rr) >> 22),
        static_cast<int>((v2 * rr) >> 21),
        static_cast<int>((v3 * rr) >> 21)
    };
}

inline std::tuple<int, int, int> randint3(int l, int r, uint64_t rng_val)
{
    const uint64_t d = static_cast<uint64_t>(r - l);
    const uint64_t v1 = rng_val >> 42;
    const uint64_t v2 = (rng_val >> 21) & 0x1FFFFFULL;
    const uint64_t v3 = rng_val & 0x1FFFFFULL;

    return {
        l + static_cast<int>((v1 * d) >> 22),
        l + static_cast<int>((v2 * d) >> 21),
        l + static_cast<int>((v3 * d) >> 21)
    };
}

} // namespace library
// ============== End of library/rng.hpp ==============


using namespace std;

#define rep1(a) for (int _ = 0; _ < (a); ++_)
#define rep2(i, a) for (int i = 0; i < (a); ++i)
#define rep3(i, a, b) for (int i = a; i < (b); ++i)
#define rrep1(a) for (int i = (a) - 1; i >= 0; --i)
#define rrep2(i, a) for (int i = (a) - 1; i >= 0; --i)
#define rrep3(i, a, b) for (int i = (b) - 1; i >= (a); --i)
#define overload3(a, b, c, d, ...) d
#define rep(...) overload3(__VA_ARGS__, rep3, rep2, rep1)(__VA_ARGS__)
#define rrep(...) overload3(__VA_ARGS__, rrep3, rrep2, rrep1)(__VA_ARGS__)
#define in(i, vec) for (auto i : (vec))
#define ctz __builtin_ctz
#define ctzll __builtin_ctzll
#define popc __builtin_popcount
#define popcll __builtin_popcountll
template <class T>
using pqueue = priority_queue<T, vector<T>>; // 大きい順
template <class T>
using pqueue_g = priority_queue<T, vector<T>, greater<T>>; // 小さい順

constexpr int INF = 1<<30;

uniform_real_distribution<double> rnd(0.0, 1.0);
normal_distribution<double> randn(0.0, 1.0);
library::Timer timer;
library::xoshiro256 rng(12345);

using u8 = uint8_t;
using i8 = int8_t;
using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;

inline u16 min8(const array<u16,8>& a){
    u16 m01 = min(a[0],a[1]);
    u16 m23 = min(a[2],a[3]);
    u16 m45 = min(a[4],a[5]);
    u16 m67 = min(a[6],a[7]);
    return min(min(m01,m23),min(m45,m67));
}
inline u16 min8(const u16* a){
    u16 m01 = min(a[0],a[1]);
    u16 m23 = min(a[2],a[3]);
    u16 m45 = min(a[4],a[5]);
    u16 m67 = min(a[6],a[7]);
    return min(min(m01,m23),min(m45,m67));
}

constexpr int NMAX = 20;
constexpr int NNMAX = NMAX * NMAX;
constexpr int SMAX = NNMAX * 8;
constexpr int MMAX = NMAX * 2;

#define TURN_RIGHT(s) ((s & 65528) | (s + 2) & 7)
#define TURN_LEFT(s) ((s & 65528) | (s + 6) & 7)


u16 nxt[SMAX];
u16 playFF[SMAX];
u16 playF3[SMAX];

int N,M,T;
int ball[MMAX];
int basket[MMAX];

void compute_nxt(const vector<string>& V, const vector<string>& H){
    rep(x,N)rep(y,N)rep(t,8){
        int s = ((x*N + y) << 3) | t;
        nxt[s] = s;
    }

    rep(x,N)rep(y,N){
        int pos = x*N + y;
        int base = pos << 3;

        // R: dir=0 => bits 0,1
        if(y < N-1 && V[x][y]=='0'){
            rep(c,2) nxt[base | c] = ((pos + 1) << 3) | c;
        }

        // D: dir=1 => bits 2,3
        if(x < N-1 && H[x][y]=='0'){
            rep(c,2) nxt[base | 2 | c] = ((pos + N) << 3) | 2 | c;
        }

        // L: dir=2 => bits 4,5
        if(y > 0 && V[x][y-1]=='0'){
            rep(c,2) nxt[base | 4 | c] = ((pos - 1) << 3) | 4 | c;
        }

        // U: dir=3 => bits 6,7
        if(x > 0 && H[x-1][y]=='0'){
            rep(c,2) nxt[base | 6 | c] = ((pos - N) << 3) | 6 | c;
        }
    }

    rep(x,N)rep(y,N)rep(t,8){
        int s = ((x*N + y) << 3) | t;
        playFF[s] = nxt[nxt[s]];
        playF3[s] = nxt[playFF[s]];
    }
}

void get_input(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin >> N >> M >> T;
    vector<string> V(N), H(N-1);
    rep(i,N) cin >> V[i];
    rep(i,N-1) cin >> H[i];
    compute_nxt(V,H);
    rep(i,M){
        int b,c,d,e; cin >> b >> c >> d >> e;
        ball[i] = b*N+c;
        basket[i] = d*N+e;
    }
}



// ============== End of define2.hpp ==============


// ============== counting_heap.hpp ==============
#include <bits/stdc++.h>
using namespace std;

template<class T>
struct CountingHeap {
    using Key = uint32_t;

    vector<vector<T>> bucket;
    Key last = 0;
    size_t sz = 0;

    CountingHeap() : bucket(1) {}

    bool empty() const { return sz == 0; }
    size_t size() const { return sz; }

    void push(Key key, const T& val) {
        if (key < last) {
            key = last;
        }
        if (key >= bucket.size()) {
            bucket.resize(key + 1);
        }
        bucket[key].push_back(val);
        ++sz;
    }

    pair<Key, T> pop() {
        while (last < bucket.size() && bucket[last].empty()) {
            ++last;
        }

        T val = bucket[last].back();
        bucket[last].pop_back();
        --sz;
        return {last, val};
    }

    void clear(){
        sz = 0;
        last = 0;
        for(auto& b:bucket)b.clear();
    }
};

// ============== End of counting_heap.hpp ==============



#define MACRO_LEN_LIMIT 31

// ballフェーズ: 手持ちが空の状態でball[i] まで行く必要がある。i以外のボールを持ったら、Sでボールを置くか、マクロを発動しない限りFが禁止される
// ballが足元にある場合はマクロ不可
// tsp用の距離を求める際は、basket[i]以外のすべてのball, basketにボールがあるものとして処理。
// dp時は、どこにボールがあるかが決定的なので、それでやる。
// bool配列でボールのあるなしを判別するのがいいかも。それならdpでもdistでも使える。マクロの終着点のhas_ballがtrueならば手持ちの状態になる。
// has_ballがtrueの場所でSを打つとボールの手持ちが入れ替わる。

// basketフェーズ: ball[i]を手に持った状態でbasket[i]まで行く必要がある。
// どこにi以外のボールがあるかは無関係
// 手に持っていない状態ではFが禁止される。
// 手に持っている状態ではマクロが禁止される。

u16 apply_op(u16 pos, u8 action) {
    // 0: R, 1: F, 2: L
    if(action == 1) {
        return nxt[pos];
    } else {
        return (pos & 65528) | ((pos + (1 + action)*2) & 7);
    }
}

struct Macro{
    u8 op[MACRO_LEN_LIMIT];
    int len = 0;

    void push_back(u8 action) {
        op[len++] = action;
    }

    bool can_push_back(u8 action) const {
        if(len == 0 || action == 1) return true;
        if(len >= 1 && ((action^op[len-1])==2)) return false; // L and R cannot be adjacent
        if(len >= 1 && (action == op[len-1])) return false; // u-turn is not allowed
        return true;
    }

    u16 play(u16 pos) const {
         // 0: R, 1: F, 2: L
        rep(i,len){
            pos = apply_op(pos, op[i]);
        }
        return pos;
    };

    void play_precalc(array<u16, SMAX>& arr) const{
        for(int i=0;i<N*N*8;i+=2){
            // macro実行後は必ずno ballになる仕様。移動先にボールがある場合の処理は外部で行う。
            arr[i] = arr[i+1] = play(i);
        }
    }

    bool operator<(const Macro& other) const {
        return len < other.len;
    }

    string to_string() const {
        string res;
        res += 'S';
        rep(i,len){
            if(op[i] == 1) res += 'F';
            else if(op[i] == 0) res += 'R';
            else res += 'L';
        }
        res += 'S';
        return res;
    }

    string to_ff_compressed_string() const{
        string res;
        res += 'S';
        for(int i=0;i < len;){
            if(i + 1 < len && op[i] == 1 && op[i+1] == 1){
                res += 'P'; // FF
                i += 2;
            } else {
                if(op[i] == 1) res += 'F';
                else if(op[i] == 0) res += 'R';
                else res += 'L';
                ++i;
            }
        }
        res += 'S';
        return res;
    }

    string to_f3_compressed_string() const{
        string res;
        res += 'S';
        for(int i=0;i < len;){
            if(i + 2 < len && op[i] == 1 && op[i+1] == 1 && op[i+2] == 1){
                res += 'P'; // FFF
                i += 3;
            } else {
                if(op[i] == 1) res += 'F';
                else if(op[i] == 0) res += 'R';
                else res += 'L';
                ++i;
            }
        }
        res += 'S';
        return res;
    }

    u64 hash() const{
        u64 h = 1;
        rep(i,len) h = h*998244353 + op[i];
        return h;
    }

    void insert(int pos, int action){
        for(int i=len;i>pos;--i){
            op[i] = op[i-1];
        }
        op[pos] = action;
        ++len;
    }

    void erase(int pos){
        for(int i=pos;i<len-1;++i)op[i]=op[i+1];
        --len;
    }

    int ff_compressed_len() const {
        // マクロ登録前に　"FF" が登録されている場合、マクロ登録が短縮される場合がある
        int res = 0;
        for(int i=0;i<len;){
            if(i + 1 < len && op[i] == 1 && op[i+1] == 1){
                ++res;      // FF -> P
                i += 2;
            } else {
                ++res;
                ++i;
            }
        }
        return res;
    }

    int f3_compressed_len() const {
        // マクロ登録前に　"FFF" が登録されている場合のlen
        int res = 0;
        for(int i=0;i<len;){
            if(i + 2 < len && op[i] == 1 && op[i+1] == 1 && op[i+2] == 1){
                ++res;      // FF -> P
                i += 3;
            } else {
                ++res;
                ++i;
            }
        }
        return res;
    }

    int normal_register_cost() const {
        return len + 4; // M + S + body + S + M
    }

    int ff_register_cost() const {
        return ff_compressed_len() + 4; // M + S + compressed_body + S + M
    }

    int f3_register_cost() const {
        return f3_compressed_len() + 4; // M + S + compressed_body + S + M
    }
};

inline u16 apply_macro_in_ball_phase(
    u16 s,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball
){
    u16 ns = nxt_macro[s];
    return ns | has_ball[ns>>3];
}

template <bool IS_BALL_PHASE, class Relax>
inline void relax_next_states(
    u16 s,
    u16 ndist,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball,
    Relax&& relax
){
    if constexpr(IS_BALL_PHASE){
        if((s & 1) == 0){
            // no ball

            // F,R,L
            relax(nxt[s], ndist);
            relax(TURN_RIGHT(s), ndist);
            relax(TURN_LEFT(s), ndist);

            // S, macro
            if(has_ball[s>>3]) relax(s|1,ndist);
            else{
                u16 ns = nxt_macro[s];
                relax(ns|has_ball[ns>>3],ndist);
            }
        } else{
            // ball

            // R,L,S
            relax(TURN_RIGHT(s), ndist);
            relax(TURN_LEFT(s), ndist);
            relax(s^1, ndist);

            // macro
            u16 ns = nxt_macro[s];
            relax(ns|has_ball[ns>>3],ndist);
        }
    } else{
        if((s & 1) == 0){
            // no ball

            // R,L,macro,S
            relax(TURN_RIGHT(s), ndist);
            relax(TURN_LEFT(s), ndist);
            relax(nxt_macro[s], ndist);
            relax(s^1,ndist);

        } else{
            // ball

            // F,R,L,S
            relax(nxt[s], ndist);
            relax(TURN_RIGHT(s), ndist);
            relax(TURN_LEFT(s), ndist);
            relax(s^1, ndist);
        }
    }
}

array<u16,SMAX> compute_start_dist(
    const Macro& macro,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball
){
    constexpr int PHASE = 4;

    static u16 dist[PHASE][SMAX];
    memset(dist, 0xFF, sizeof(dist));

    static CountingHeap<u16> pq;
    pq.clear();

    // phase 0: 本マクロ発動済み
    // phase 1: FF発動済み
    // phase 2: F3発動済み
    // phase 3: 未発動

    dist[3][0] = 0;

    auto pack = [](u16 phase, u16 s)->u16{
        return (phase << 12) | s;
    };

    auto relax = [&](u16 np, u16 ns, int nd){
        if(dist[np][ns] > nd){
            dist[np][ns] = nd;
            pq.push(nd, pack(np, ns));
        }
    };

    pq.clear();
    pq.push(0, pack(3, 0));

    int normal_cost = macro.normal_register_cost();
    int ff_cost = macro.ff_register_cost();
    int f3_cost = macro.f3_register_cost();

    while(!pq.empty()){
        auto [cd, v] = pq.pop();

        u16 p = v >> 12;
        u16 s = v & 4095;

        if(dist[p][s] != cd) continue;

        if(p == 0){
            // 本マクロ発動済みフェーズ
            auto relax2 = [&](u16 ns, u16 ndist){
                relax(0, ns, ndist);
            };
            relax_next_states<true>(s, cd + 1, nxt_macro, has_ball, relax2);

        } else if(p == 1){
            // FF発動済みフェーズでは carrying=true は考えない
            if(s & 1) continue;

            // F,R,L
            relax(1, nxt[s], cd + 1);
            relax(1, TURN_RIGHT(s), cd + 1);
            relax(1, TURN_LEFT(s), cd + 1);

            // FF使用
            relax(1, playFF[s], cd + 1);

            // 本マクロ登録 + 実行
            if(!has_ball[s>>3]){
                relax(0, apply_macro_in_ball_phase(s, nxt_macro, has_ball), cd + ff_cost);
            }

        } else if(p == 2){
            // F3発動済みフェーズでは carrying=true は考えない
            if(s & 1) continue;

            // F,R,L
            relax(2, nxt[s], cd + 1);
            relax(2, TURN_RIGHT(s), cd + 1);
            relax(2, TURN_LEFT(s), cd + 1);

            // F3使用
            relax(2, playF3[s], cd + 1);

            // 本マクロ登録 + 実行
            if(!has_ball[s>>3]){
                relax(0, apply_macro_in_ball_phase(s, nxt_macro, has_ball), cd + f3_cost);
            }

        } else{
            // 未発動フェーズでは carrying=true は考えない
            if(s & 1) continue;

            // F,R,L
            relax(3, nxt[s], cd + 1);
            relax(3, TURN_RIGHT(s), cd + 1);
            relax(3, TURN_LEFT(s), cd + 1);

            // FF登録 + 実行
            relax(1, playFF[s], cd + 4);

            // F3登録 + 実行
            relax(2, playF3[s], cd + 5);

            // 本マクロ登録 + 実行
            if(!has_ball[s>>3]){
                relax(0, apply_macro_in_ball_phase(s, nxt_macro, has_ball), cd + normal_cost);
            }
        }
    }

    array<u16,SMAX> res;
    res.fill(UINT16_MAX);

    rep(s,SMAX){
        int mn = dist[0][s];

        if((s & 1) == 0){
            mn = min(mn, dist[1][s] + ff_cost - 1);
            mn = min(mn, dist[2][s] + f3_cost - 1);
            mn = min(mn, dist[3][s] + normal_cost - 1);
        }

        if(mn < UINT16_MAX) res[s] = mn;
    }

    return res;
}

array<u16,8> start_step(
    const Macro& macro,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball,
    u16 to
){
    constexpr int PHASE = 4;
    static u16 dist[PHASE][SMAX];
    memset(dist,0xFF,sizeof(dist));

    array<u16,8> res;
    res.fill(UINT16_MAX);

    static CountingHeap<u16> pq;
    pq.clear();

    auto pack = [](u16 phase, u16 s)->u16{
        return (phase << 12) | s;
    };

    auto relax = [&](u16 np, u16 ns, int nd){
        if(dist[np][ns] > nd){
            dist[np][ns] = nd;
            pq.push(nd, pack(np,ns));
        }
    };

    // phase 0: 本マクロ発動済み
    // phase 1: FF発動済み
    // phase 2: F3発動済み
    // phase 3: 未発動
    dist[3][0] = 0;
    pq.push(0, pack(3,0));

    int normal_cost = macro.normal_register_cost();
    int ff_cost = macro.ff_register_cost();
    int f3_cost = macro.f3_register_cost();

    int done = 0;

    while(!pq.empty()){
        auto [cd, v] = pq.pop();

        u16 p = v >> 12;
        u16 s = v & 4095;

        if(dist[p][s] != cd) continue;

        if(p == 0 && (s >> 3) == to){
            res[s&7] = cd;
            ++done;
            if(done == 8) return res;
        }

        if(p == 0){
            auto relax2 = [&](u16 ns, u16 ndist){
                relax(0, ns, ndist);
            };
            relax_next_states<true>(s, cd + 1, nxt_macro, has_ball, relax2);

        } else if(p == 1){
            if(s & 1) continue;

            // F,R,L
            relax(1, nxt[s], cd + 1);
            relax(1, TURN_RIGHT(s), cd + 1);
            relax(1, TURN_LEFT(s), cd + 1);

            // FF使用
            relax(1, playFF[s], cd + 1);

            // phase0 への登録のみ遷移
            if((s>>3) == to) relax(0, s, cd + ff_cost - 1);

            // 本マクロ登録 + 実行
            if(!has_ball[s>>3]){
                relax(
                    0,
                    apply_macro_in_ball_phase(s, nxt_macro, has_ball),
                    cd + ff_cost
                );
            }

        } else if(p == 2){
            if(s & 1) continue;

            // F,R,L
            relax(2, nxt[s], cd + 1);
            relax(2, TURN_RIGHT(s), cd + 1);
            relax(2, TURN_LEFT(s), cd + 1);

            // F3使用
            relax(2, playF3[s], cd + 1);

            // phase0 への登録のみ遷移
            if((s>>3) == to) relax(0, s, cd + f3_cost - 1);

            // 本マクロ登録 + 実行
            if(!has_ball[s>>3]){
                relax(
                    0,
                    apply_macro_in_ball_phase(s, nxt_macro, has_ball),
                    cd + f3_cost
                );
            }

        } else{
            if(s & 1) continue;

            // F,R,L
            relax(3, nxt[s], cd + 1);
            relax(3, TURN_RIGHT(s), cd + 1);
            relax(3, TURN_LEFT(s), cd + 1);

            // FF登録 + 実行
            relax(1, playFF[s], cd + 4);

            // F3登録 + 実行
            relax(2, playF3[s], cd + 5);

            // phase0 への登録のみ遷移
            if((s>>3) == to) relax(0, s, cd + normal_cost - 1);

            // 本マクロ登録 + 実行
            if(!has_ball[s>>3]){
                relax(
                    0,
                    apply_macro_in_ball_phase(s, nxt_macro, has_ball),
                    cd + normal_cost
                );
            }
        }
    }

    assert(false);
}

array<u16,SMAX> compute_ball_dist(
    u16 start,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball
){
    array<u16, SMAX> dist;
    fill(dist.begin(), dist.end(), UINT16_MAX);
    u16 que[SMAX];
    int h=0;
    dist[start] = 0;
    que[h++] = start;
    auto relax = [&](u16 ns, u16 ndist){
        if(dist[ns] == UINT16_MAX){
            dist[ns] = ndist;
            que[h++] = ns;
        }
    };
    for(int head=0; head<h; ++head){
        u16 s = que[head];
        u16 ndist = dist[s]+1;
        relax_next_states<true>(s, ndist, nxt_macro, has_ball, relax);
    }
    return dist;
}

array<array<u16,8>,8> compute_basket_dist(
    u16 ball_id,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball
){
    array<array<u16,8>,8> res; // [end_dir][start_dir]
    array<u16, SMAX> dist;
    u16 que[SMAX];

    u16 ball_pos = ball[ball_id], basket_pos = basket[ball_id];

    int h = 0;
    int done = 0;

    for(int start_dir = 0;start_dir < 8; ++start_dir){
        auto relax = [&](u16 ns, u16 ndist){
            if(dist[ns] == UINT16_MAX){
                dist[ns] = ndist;
                que[h++] = ns;
                if((ns>>3) == basket_pos){
                    res[ns&7][start_dir]=ndist;
                    ++done;
                }
            }
        };
        int start = ball_pos<<3|start_dir;
        h = 0, done = 0;
        fill(dist.begin(), dist.end(), UINT16_MAX);
        dist[start] = 0;
        que[h++] = start;
        for(int head=0; head<h; ++head){
            u16 s = que[head];
            u16 ndist = dist[s]+1;
            relax_next_states<false>(s, ndist, nxt_macro, has_ball, relax);
            if(done==8)break;
        }
    }
    return res;
}

void compute_distance_array(u16 dists[NMAX][8], u16 dist[MMAX][MMAX][8][8], const Macro& macro, const array<u16, SMAX>& nxt_macro){
    // dists[to][to_dir]
    // dist[to][from][from_dir][to_dir]
    
    array<u8,NNMAX> has_ball{};
    rep(i,M)has_ball[ball[i]] = 1;
    array<u16,SMAX> from_start = compute_start_dist(macro,nxt_macro,has_ball);
    vector<array<u16,SMAX>> from_basket(8*M);
    array<array<u16,8>,8> from_ball_t[MMAX];
    rep(i,M)has_ball[basket[i]]=1;
    rep(i,M){
        has_ball[basket[i]] = 0;
        rep(d,8){
            from_basket[i<<3|d] = compute_ball_dist((basket[i]<<3)|d,nxt_macro,has_ball);
        }
        from_ball_t[i] = compute_basket_dist(i,nxt_macro,has_ball);
        has_ball[basket[i]] = 1;
    }

    // compute dists
    rep(i,M)rep(d2,8){
        int mn = 1<<30;
        rep(k,8){
            mn = min(mn,from_start[ball[i]<<3|k]+from_ball_t[i][d2][k]);
        }
        dists[i][d2] = mn;
    }
    // compute dist
    rep(i,M)rep(d1,8){
        rep(j,M)rep(d2,8){
            int mn = 1<<30;
            rep(k,8){
                mn = min(mn,from_basket[i<<3|d1][ball[j]<<3|k] + from_ball_t[j][d2][k]);
            }
            dist[j][i][d1][d2] = mn;
        }
    }
}

template <bool IS_BALL_PHASE>
array<u16,8> common_step(
    const array<u16,8>& prv,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball,
    u16 from,
    u16 to
){
    array<u16, 8> res;
    u16 dist[SMAX];
    memset(dist, 0xff, sizeof(dist));
    u16 que[SMAX];
    int tail=0;
    int done = 0;
    u16 mn = min8(prv);
    auto relax = [&](u16 ns, u16 ndist){
        if(dist[ns] == UINT16_MAX){
            dist[ns] = ndist;
            que[tail++] = ns;
            if((ns >> 3) == to){
                res[ns&7] = ndist + mn;
                ++done;
            }
        }
    };
    auto bfs_step = [&](int head){
        u16 s = que[head];
        u16 ndist = dist[s]+1;
        relax_next_states<IS_BALL_PHASE>(s, ndist, nxt_macro, has_ball, relax);
        return done==8;
    };

    // prvが最小のものを追加
    u16 base = from<<3;
    rep(i,8)if(prv[i]==mn){
        dist[base|i] = 0;
        que[tail++] = base|i;
    }

    // 距離1のみ探索
    int tmp = tail;
    int head = 0;
    for(;head<tmp;++head){
        if(bfs_step(head)) return res;
    }

    // prevがmn+1で、未探索のものをキューに追加
    rep(i,8)if(dist[base|i] == UINT16_MAX && prv[i] == mn+1) {
        dist[base|i] = 1;
        que[tail++] = base|i;
    }

    // 距離2のみ探索
    tmp = tail;
    for(;head<tmp;++head){
        if(bfs_step(head)) return res;
    }

    // prevがmn+2で、未探索のものをキューに追加
    rep(i,8)if(dist[base|i] == UINT16_MAX && prv[i] == mn+2) {
        dist[base|i] = 2;
        que[tail++] = base|i;
    }

    // 距離3以降の探索
    for(;head<tail;++head){
        if(bfs_step(head)) return res;
    }

    assert(false);
}

array<u16,8> ball_step(
    const array<u16,8>& prv,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball,
    u16 from,
    u16 to
){
    return common_step<true>(prv, nxt_macro, has_ball, from, to);
}

array<u16,8> basket_step(
    const array<u16,8>& prv,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball,
    u16 from,
    u16 to
){
    return common_step<false>(prv, nxt_macro, has_ball, from, to);
}


int dp(const vector<int>& order, const Macro& macro, const array<u16, SMAX>& nxt_macro){
    array<u16, 8> res;
    array<u8,NNMAX> has_ball = {};
    rep(i,M) has_ball[ball[i]] = 1;
    int pos = 0;
    rep(i,M){
        int bl = ball[order[i]];
        int bk = basket[order[i]];
        res = i==0 ? start_step(macro,nxt_macro,has_ball,bl) : ball_step(res,nxt_macro,has_ball,pos,bl);
        res = basket_step(res,nxt_macro,has_ball,bl,bk);
        has_ball[bl] = 0;
        has_ball[bk] = 1;
        pos = bk;
    }
    return min(min(res[0],res[2]),min(res[4],res[6]));
};

namespace ComputePath{

struct PathNode{
    int parent;
    char action;
};

vector<PathNode> tree;

template<bool Build_Tree>
struct BFSNode;

template<>
struct BFSNode<true>{
    u16 state;
    u16 expanded_len;
    int tree_idx;
};

template<>
struct BFSNode<false>{
    u16 state;
    u16 expanded_len;
};

template<bool Build_Tree>
struct StepNode{
    BFSNode<Build_Tree> bfs_state;
    u16 cost;
};

constexpr int INF_PATH = 1 << 30;

template<bool Build_Tree>
array<StepNode<Build_Tree>,8> start_step_with_path(
    u16 to,
    const Macro& macro,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball
){
    constexpr int PHASE = 4;
    // phase 0: 本マクロ発動済み
    // phase 1: FF発動済み
    // phase 2: F3発動済み
    // phase 3: 未発動

    static u16 dist[PHASE][SMAX];
    memset(dist,0xFF,sizeof(dist));

    array<StepNode<Build_Tree>,8> res;
    if constexpr(Build_Tree) rep(i,8) res[i] = {{0,0,-1}, UINT16_MAX};
    else rep(i,8) res[i] = {{0,0}, UINT16_MAX};

    static CountingHeap<BFSNode<Build_Tree>> pq;
    pq.clear();

    tree.clear();
    tree.push_back({-1,'@'});

    auto pack = [](u16 phase, u16 s)->u16{
        return (phase << 12) | s;
    };

    auto relax = [&](u16 np, u16 ns, int nd, u16 nelen, int parent, char action){
        if(dist[np][ns] > nd){
            dist[np][ns] = nd;
            if constexpr(Build_Tree){
                int tid = (int)tree.size();
                tree.push_back({parent, action});
                pq.push(nd, {pack(np,ns), nelen, tid});
            } else{
                pq.push(nd, {pack(np,ns), nelen});
            }
        }
    };

    dist[3][0] = 0;
    if constexpr(Build_Tree) pq.push(0, {pack(3,0), 0, 0});
    else pq.push(0, {pack(3,0), 0});

    int normal_cost = macro.normal_register_cost();
    int ff_cost = macro.ff_register_cost();
    int f3_cost = macro.f3_register_cost();
    int macro_expanded = macro.len + 2;

    int done = 0;

    while(!pq.empty()){
        auto [cd, v] = pq.pop();

        u16 state = v.state;
        u16 elen = v.expanded_len;
        int parent = -1;
        if constexpr(Build_Tree) parent = v.tree_idx;

        u16 p = state >> 12;
        u16 s = state & 4095;

        if(dist[p][s] != cd) continue;

        if(p == 0 && (s >> 3) == to){
            if constexpr(Build_Tree){
                res[s&7] = {{s, elen, parent}, (u16)cd};
            } else {
                res[s&7] = {{s, elen}, (u16)cd};
            }
            ++done;
            if(done == 8) return res;
        }

        if(p == 0){
            int nd = cd + 1;

            if((s & 1) == 0){
                // no ball

                // F,R,L
                relax(0, nxt[s], nd, elen + 1, parent, 'F');
                relax(0, TURN_RIGHT(s), nd, elen + 1, parent, 'R');
                relax(0, TURN_LEFT(s), nd, elen + 1, parent, 'L');

                // S, macro
                if(has_ball[s>>3]){
                    relax(0, s|1, nd, elen + 1, parent, 'S');
                } else{
                    u16 ns = nxt_macro[s];
                    relax(0, ns | has_ball[ns>>3], nd, elen + macro_expanded, parent, 'P');
                }
            } else{
                // ball

                // R,L,S
                relax(0, TURN_RIGHT(s), nd, elen + 1, parent, 'R');
                relax(0, TURN_LEFT(s), nd, elen + 1, parent, 'L');
                relax(0, s^1, nd, elen + 1, parent, 'S');

                // macro
                u16 ns = nxt_macro[s];
                relax(0, ns | has_ball[ns>>3], nd, elen + macro_expanded, parent, 'P');
            }

        } else if(p == 1){
            if(s & 1) continue;

            // F,R,L
            relax(1, nxt[s], cd + 1, elen + 1, parent, 'F');
            relax(1, TURN_RIGHT(s), cd + 1, elen + 1, parent, 'R');
            relax(1, TURN_LEFT(s), cd + 1, elen + 1, parent, 'L');

            // FF使用
            relax(1, playFF[s], cd + 1, elen + 2, parent, '0');

            // phase0 への登録のみ遷移
            if((s>>3) == to) relax(0, s, cd + ff_cost - 1, elen, parent, '\0');

            // 本マクロ登録 + 実行
            if(!has_ball[s>>3]){
                relax(
                    0,
                    apply_macro_in_ball_phase(s, nxt_macro, has_ball),
                    cd + ff_cost,
                    elen + macro_expanded,
                    parent,
                    'P'
                );
            }

        } else if(p == 2){
            if(s & 1) continue;

            // F,R,L
            relax(2, nxt[s], cd + 1, elen + 1, parent, 'F');
            relax(2, TURN_RIGHT(s), cd + 1, elen + 1, parent, 'R');
            relax(2, TURN_LEFT(s), cd + 1, elen + 1, parent, 'L');

            // F3使用
            relax(2, playF3[s], cd + 1, elen + 3, parent, '1');

            // phase0 への登録のみ遷移
            if((s>>3) == to) relax(0, s, cd + f3_cost - 1, elen, parent, '\0');

            // 本マクロ登録 + 実行
            if(!has_ball[s>>3]){
                relax(
                    0,
                    apply_macro_in_ball_phase(s, nxt_macro, has_ball),
                    cd + f3_cost,
                    elen + macro_expanded,
                    parent,
                    'P'
                );
            }

        } else{
            if(s & 1) continue;

            // F,R,L
            relax(3, nxt[s], cd + 1, elen + 1, parent, 'F');
            relax(3, TURN_RIGHT(s), cd + 1, elen + 1, parent, 'R');
            relax(3, TURN_LEFT(s), cd + 1, elen + 1, parent, 'L');

            // FF登録 + 実行
            relax(1, playFF[s], cd + 4, elen + 2, parent, '0');

            // F3登録 + 実行
            relax(2, playF3[s], cd + 5, elen + 3, parent, '1');

            // phase0 への登録のみ遷移
            if((s>>3) == to) relax(0, s, cd + normal_cost - 1, elen, parent, '\0');

            // 本マクロ登録 + 実行
            if(!has_ball[s>>3]){
                relax(
                    0,
                    apply_macro_in_ball_phase(s, nxt_macro, has_ball),
                    cd + normal_cost,
                    elen + macro_expanded,
                    parent,
                    'P'
                );
            }
        }
    }

    assert(false);
}

template<bool BALL_PHASE, bool Build_Tree>
array<StepNode<Build_Tree>,8> common_step_with_path(
    const array<StepNode<Build_Tree>, 8>& prv,
    const Macro& macro,
    const array<u16, SMAX>& nxt_macro,
    const array<u8,NNMAX>& has_ball,
    u16 from,
    u16 to
){
    array<StepNode<Build_Tree>, 8> res;

    u16 dist[SMAX];
    memset(dist, 0xff, sizeof(dist));

    BFSNode<Build_Tree> que[SMAX];
    int tail = 0;
    int done = 0;

    u16 mn = UINT16_MAX;
    rep(i,8) mn = min(mn, prv[i].cost);

    int macro_expanded = macro.len + 2;

    auto relax = [&](u16 ns, u16 nd, u16 nelen, int parent, char action){
        if(dist[ns] == UINT16_MAX){
            dist[ns] = nd;

            if constexpr(Build_Tree){
                int tid = (int)tree.size();
                tree.push_back({parent, action});

                que[tail++] = {ns, nelen, tid};
            } else{
                que[tail++] = {ns,nelen};
            }

            if((ns >> 3) == to){
                res[ns&7] = {que[tail-1], (u16)(mn + nd)};
                ++done;
            }
        }
    };

    auto bfs_step = [&](int head){
        BFSNode<Build_Tree> v = que[head];
        u16 s = v.state;
        u16 nd = dist[s] + 1;
        u16 elen = v.expanded_len;
        int parent = -1;
        if constexpr(Build_Tree) parent = v.tree_idx;

        if constexpr(BALL_PHASE){
            if((s & 1) == 0){
                // no ball

                // F,R,L
                relax(nxt[s], nd, elen + 1, parent, 'F');
                relax(TURN_RIGHT(s), nd, elen + 1, parent, 'R');
                relax(TURN_LEFT(s), nd, elen + 1, parent, 'L');

                // S, macro
                if(has_ball[s>>3]){
                    relax(s|1, nd, elen + 1, parent, 'S');
                } else{
                    u16 ns = nxt_macro[s];
                    relax(ns | has_ball[ns>>3], nd, elen + macro_expanded, parent, 'P');
                }
            } else{
                // ball

                // R,L,S
                relax(TURN_RIGHT(s), nd, elen + 1, parent, 'R');
                relax(TURN_LEFT(s), nd, elen + 1, parent, 'L');
                relax(s^1, nd, elen + 1, parent, 'S');

                // macro
                u16 ns = nxt_macro[s];
                relax(ns | has_ball[ns>>3], nd, elen + macro_expanded, parent, 'P');
            }
        } else{
            if((s & 1) == 0){
                // no ball

                // R,L,macro,S
                relax(TURN_RIGHT(s), nd, elen + 1, parent, 'R');
                relax(TURN_LEFT(s), nd, elen + 1, parent, 'L');
                relax(nxt_macro[s], nd, elen + macro_expanded, parent, 'P');
                relax(s^1, nd, elen + 1, parent, 'S');
            } else{
                // ball

                // F,R,L,S
                relax(nxt[s], nd, elen + 1, parent, 'F');
                relax(TURN_RIGHT(s), nd, elen + 1, parent, 'R');
                relax(TURN_LEFT(s), nd, elen + 1, parent, 'L');
                relax(s^1, nd, elen + 1, parent, 'S');
            }
        }

        return done == 8;
    };

    u16 base = from << 3;

    if(prv[0].bfs_state.state != base){
        cerr << "bfs state.state " << prv[0].bfs_state.state << " base " << base << endl;
        rep(i,8) cerr << prv[i].bfs_state.state << endl;
    }
    assert(prv[0].bfs_state.state == base);

    // prvが最小のものを追加
    rep(i,8)if(prv[i].cost == mn){
        u16 s = base | i;
        dist[s] = 0;
        que[tail++] = prv[i].bfs_state;
    }

    // 距離1のみ探索
    int tmp = tail;
    int head = 0;
    for(;head<tmp;++head){
        if(bfs_step(head)) return res;
    }

    // prevがmn+1で、未探索のものをキューに追加
    rep(i,8)if(dist[base|i] == UINT16_MAX && prv[i].cost == mn+1){
        u16 s = base | i;
        dist[s] = 1;
        que[tail++] = prv[i].bfs_state;
    }

    // 距離2のみ探索
    tmp = tail;
    for(;head<tmp;++head){
        if(bfs_step(head)) return res;
    }

    // prevがmn+2で、未探索のものをキューに追加
    rep(i,8)if(dist[base|i] == UINT16_MAX && prv[i].cost == mn+2){
        u16 s = base | i;
        dist[s] = 2;
        que[tail++] = prv[i].bfs_state;
    }

    // 距離3以降の探索
    for(;head<tail;++head){
        if(bfs_step(head)) return res;
    }

    assert(false);
}

string build_answer_from_tree(int tree_idx, const Macro& macro){
    string tmp;
    while(tree_idx != -1){
        char a = tree[tree_idx].action;
        if(a != '@' && a != '\0') tmp += a;
        tree_idx = tree[tree_idx].parent;
    }

    reverse(tmp.begin(), tmp.end());

    string ans;
    bool ff_registered = false;
    bool f3_registered = false;
    bool macro_registered = false;

    for(char c : tmp){
        if(c == '0'){
            if(!ff_registered){
                ans += "M\nF\nF\nM\n";
                ff_registered = true;
            } else {
                ans += "P\n";
            }
        } else if(c == '1'){
            if(!f3_registered){
                ans += "M\nF\nF\nF\nM\n";
                f3_registered = true;
            } else {
                ans += "P\n";
            }
        }
        else if(c == 'P'){
            if(!macro_registered){
                string macro_string;
                if(ff_registered) macro_string = macro.to_ff_compressed_string();
                else if(f3_registered) macro_string = macro.to_f3_compressed_string();
                else macro_string = macro.to_string();

                ans += "M\n";
                for(char mc : macro_string){
                    ans += mc;
                    ans += '\n';
                }
                ans += "M\n";

                macro_registered = true;
            } else {
                ans += "P\n";
            }
        } else{
            ans += c;
            ans += '\n';
        }
    }

    return ans;
}

template<bool Build_Tree>
StepNode<Build_Tree> build_by_dp(
    const vector<int>& order,
    const Macro& macro,
    const array<u16, SMAX>& nxt_macro
){

    assert((int)order.size() == M);

    array<u8,NNMAX> has_ball = {};
    rep(i,M) has_ball[ball[i]] = 1;

    array<StepNode<Build_Tree>,8> res;

    int pos = 0;

    rep(i,M){
        int bl = ball[order[i]];
        int bk = basket[order[i]];

        if(i == 0){
            res = start_step_with_path<Build_Tree>(bl, macro, nxt_macro, has_ball);
        } else {
            res = common_step_with_path<true,Build_Tree>(res, macro, nxt_macro, has_ball, pos, bl);
        }

        res = common_step_with_path<false,Build_Tree>(res, macro, nxt_macro, has_ball, bl, bk);

        has_ball[bl] = 0;
        has_ball[bk] = 1;
        pos = bk;
    }

    int best = -1;
    u16 best_cost = UINT16_MAX;

    for(int d=0; d<8; d+=2){
        if(res[d].cost < best_cost){
            best_cost = res[d].cost;
            best = d;
        }
    }
    return res[best];
}

int eval_macro_with_len_limit(
    const vector<int>& order,
    const Macro& macro,
    const array<u16, SMAX>& nxt_macro
) {
    auto st = build_by_dp<false>(order,macro,nxt_macro);
    if(st.bfs_state.expanded_len > T) return INF;
    else return st.cost;
}

string compute_path_with_macro(
    const vector<int>& order,
    const Macro& macro,
    const array<u16, SMAX>& nxt_macro
){
    

    int tree_idx = build_by_dp<true>(order,macro,nxt_macro).bfs_state.tree_idx;

    return build_answer_from_tree(tree_idx, macro);
}

} // namespace ComputePath
// ============== End of macro3_core.hpp ==============



vector<int> tsp_bitdp3(const u16 dists[MMAX][8], const u16 dist_[MMAX][MMAX][8][8], const vector<int>& rem){
    int m = rem.size();
    assert(m <= 16);

    u16 dst[16][16][8][8];
    rep(i,m)rep(j,m)rep(dj,8)rep(di,8){
        dst[i][j][di][dj] = dist_[rem[i]][rem[j]][di][dj]; // to_pos, from_pos, from_dir, to_dir
    }

    vector<array<array<u16,8>,16>> dp(1<<m);
    memset(dp.data(),0xFF,dp.size()*sizeof(dp[0]));

    for(int vis=1;vis<(1<<m);++vis){
        for(int b1=vis;b1;b1&=b1-1){
            int p1 = ctz(b1);
            int pvis = vis ^ (1<<p1);

            if(pvis == 0){
                rep(d1,8){
                    dp[vis][p1][d1] = dists[rem[p1]][d1];
                }
                continue;
            }

            for(int b2=pvis;b2;b2&=b2-1){
                int p2 = ctz(b2);

                rep(d2,8){
                    u16 base = dp[pvis][p2][d2];
                    rep(d1,8){
                        dp[vis][p1][d1] = min<u16>(
                            dp[vis][p1][d1],
                            base + dst[p1][p2][d2][d1]
                        );
                    }
                }
            }
        }
    }

    // 復元
    vector<int> order(m);
    int full = (1<<m) - 1;

    int lp, ld;
    int cur = 1<<30;

    rep(p,m)for(int d=0;d<8;d+=2){
        if(dp[full][p][d] < cur){
            cur = dp[full][p][d];
            lp = p;
            ld = d;
        }
    }

    int vis = full;
    order[m-1] = rem[lp];

    for(int idx=m-2;idx>=0;--idx){
        int pvis = vis ^ (1<<lp);
        bool found = false;

        for(int b2=pvis;b2;b2&=b2-1){
            int p2 = ctz(b2);

            rep(d2,8){
                if(dp[pvis][p2][d2] + dst[lp][p2][d2][ld] == cur){
                    found = true;
                    vis = pvis;
                    lp = p2;
                    ld = d2;
                    cur = dp[vis][lp][ld];
                    break;
                }
            }

            if(found) break;
        }

        assert(found);
        order[idx] = rem[lp];
    }

    return order;
}


namespace PartialSA {

using Arr8 = array<int,8>;

inline int min8_int(const Arr8& a){
    int mn = a[0];
    rep(i,1,8) mn = min(mn, a[i]);
    return mn;
}

inline int min8_noball(const Arr8& a){
    return min(min(a[0],a[2]),min(a[4],a[6]));
}

inline Arr8 trans_forward(
    const Arr8& cur,
    int from,
    int to,
    const u16 dist[MMAX][MMAX][8][8]
){
    Arr8 res;
    res.fill(INF);
    rep(fd,8){
        int base = cur[fd];
        rep(td,8){
            res[td] = min(res[td], base + dist[to][from][fd][td]);
        }
    }
    return res;
}

inline Arr8 trans_backward(
    const Arr8& cur,
    int from,
    int to,
    const u16 dist[MMAX][MMAX][8][8]
){
    // cur[td] = suffix側の to_dir から末尾までの最小コスト
    // res[fd] = from_dir から末尾までの最小コスト
    Arr8 res;
    res.fill(INF);
    rep(fd,8){
        rep(td,8){
            res[fd] = min(res[fd], dist[to][from][fd][td] + cur[td]);
        }
    }
    return res;
}

struct SATSPWorkState{
    vector<int> order;
    vector<Arr8> prefix;
    vector<Arr8> suffix;
    int score;

    void init(
        const vector<int>& initial_order,
        const u16 dists[MMAX][8],
        const u16 dist[MMAX][MMAX][8][8]
    ){
        order = initial_order;
        prefix.resize(M);
        suffix.resize(M);
        rebuild_all(dists, dist);
    }

    void rebuild_all(
        const u16 dists[MMAX][8],
        const u16 dist[MMAX][MMAX][8][8]
    ){
        rep(d,8) prefix[0][d] = dists[order[0]][d];

        rep(i,1,M){
            prefix[i] = trans_forward(prefix[i-1], order[i-1], order[i], dist);
        }

        rep(d,8) suffix[M-1][d] = INF;
        suffix[M-1][0] = 0;
        suffix[M-1][2] = 0;
        suffix[M-1][4] = 0;
        suffix[M-1][6] = 0;

        for(int i=M-2;i>=0;--i){
            suffix[i] = trans_backward(suffix[i+1], order[i], order[i+1], dist);
        }

        score = min8_noball(prefix[M-1]);
    }

    void rebuild_from(
        int l,
        const u16 dists[MMAX][8],
        const u16 dist[MMAX][MMAX][8][8]
    ){
        if(l <= 0){
            rep(d,8) prefix[0][d] = dists[order[0]][d];
            l = 1;
        }

        for(int i=l;i<M;++i){
            prefix[i] = trans_forward(prefix[i-1], order[i-1], order[i], dist);
        }

        score = min8_noball(prefix[M-1]);
    }

    void rebuild_suffix_from(
        int r,
        const u16 dist[MMAX][MMAX][8][8]
    ){
        if(r >= M-1){
            rep(d,8) suffix[M-1][d] = INF;
            suffix[M-1][0] = 0;
            suffix[M-1][2] = 0;
            suffix[M-1][4] = 0;
            suffix[M-1][6] = 0;
            r = M-2;
        }

        for(int i=r;i>=0;--i){
            suffix[i] = trans_backward(suffix[i+1], order[i], order[i+1], dist);
        }
    }

    int calc_range_score(
        const vector<int>& mid,
        int l,
        int r,
        const u16 dists[MMAX][8],
        const u16 dist[MMAX][MMAX][8][8]
    ) const {
        Arr8 cur;

        if(l == 0){
            rep(d,8) cur[d] = dists[mid[0]][d];

            for(int i=1;i<(int)mid.size();++i){
                cur = trans_forward(cur, mid[i-1], mid[i], dist);
            }
        } else {
            cur = prefix[l-1];

            int prev = order[l-1];
            for(int x : mid){
                cur = trans_forward(cur, prev, x, dist);
                prev = x;
            }
        }

        if(r + 1 < M){
            int prev = mid.back();
            int nxt = order[r+1];
            cur = trans_forward(cur, prev, nxt, dist);

            int mn = INF;
            rep(d,8){
                mn = min(mn, cur[d] + suffix[r+1][d]);
            }
            return mn;
        }

        return min8_noball(cur);
    }

    int calc_swap_score(
        int i,
        int j,
        const u16 dists[MMAX][8],
        const u16 dist[MMAX][MMAX][8][8]
    ) const {
        if(i > j) swap(i,j);
        int l = i;
        int r = j;

        vector<int> mid;
        mid.reserve(r-l+1);
        for(int p=l;p<=r;++p){
            if(p == i) mid.push_back(order[j]);
            else if(p == j) mid.push_back(order[i]);
            else mid.push_back(order[p]);
        }

        return calc_range_score(mid, l, r, dists, dist);
    }

    int calc_insert_score(
        int i,
        int j,
        int k,
        const u16 dists[MMAX][8],
        const u16 dist[MMAX][MMAX][8][8]
    ) const {
        // [i,j) を erase した後の index k に insert する
        if(k >= i && k <= j) return score;

        vector<int> norder;
        norder.reserve(M);

        vector<int> picked(order.begin()+i, order.begin()+j);

        rep(p,M){
            if(p < i || p >= j) norder.push_back(order[p]);
        }

        norder.insert(norder.begin()+k, picked.begin(), picked.end());

        int l = min(i,k);
        int r = max(j-1, k + (int)picked.size() - 1);

        vector<int> mid;
        mid.reserve(r-l+1);
        for(int p=l;p<=r;++p) mid.push_back(norder[p]);

        return calc_range_score(mid, l, r, dists, dist);
    }

    void apply_swap(
        int i,
        int j,
        const u16 dists[MMAX][8],
        const u16 dist[MMAX][MMAX][8][8]
    ){
        if(i > j) swap(i,j);
        swap(order[i], order[j]);

        int l = i;
        rebuild_from(l, dists, dist);
        rebuild_suffix_from(j, dist);
    }

    void apply_insert(
        int i,
        int j,
        int k,
        const u16 dists[MMAX][8],
        const u16 dist[MMAX][MMAX][8][8]
    ){
        // [i,j) を erase した後の index k に insert
        if(k >= i && k <= j) return;

        vector<int> picked(order.begin()+i, order.begin()+j);
        order.erase(order.begin()+i, order.begin()+j);
        order.insert(order.begin()+k, picked.begin(), picked.end());

        int l = min(i,k);
        rebuild_from(l, dists, dist);
        rebuild_suffix_from(M-1, dist);
    }
};

vector<int> tsp_SA(
    const u16 dists[MMAX][8],
    const u16 dist[MMAX][MMAX][8][8],
    double tl,
    double start_temp,
    double end_temp
){
    double end_time = timer.elapsed() + tl;

    vector<int> initial_order(M);
    rep(i,M) initial_order[i] = i;

    SATSPWorkState state;
    state.init(initial_order, dists, dist);

    vector<int> best_order = state.order;
    int best_score = state.score;

    cerr << "initial score: " << best_score << endl;

    int cur_score = state.score;
    double temp = start_temp;
    double start_time = timer.elapsed();
    int itr = 0;
    int acc = 0;

    while(true){
        ++itr;

        if((itr&255)==0){
            double tm = timer.elapsed();
            if(tm > end_time) break;
            temp = start_temp * pow(end_temp/start_temp, (tm-start_time)/(end_time-start_time));
        }

        bool is_swap = rnd(rng) < 0.5;
        int new_score;

        int i,j,k;

        if(is_swap){
            do{
                tie(i,j) = library::randint_pair(M, rng());
            }while(i == j);

            new_score = state.calc_swap_score(i, j, dists, dist);
        } else {
            do{
                tie(i,j) = library::randint_pair(M, rng());
                if(i > j) swap(i,j);
            }while(j-i >= M - 2);

            ++j;

            int rest = M - (j - i);
            do{
                k = library::randint(rest + 1, rng());
            }while(k == i);

            new_score = state.calc_insert_score(i, j, k, dists, dist);
        }

        if(new_score < best_score){
            best_score = new_score;

            if(is_swap){
                state.apply_swap(i, j, dists, dist);
            } else {
                state.apply_insert(i, j, k, dists, dist);
            }

            cur_score = state.score;
            best_order = state.order;
            ++acc;
        } else if(new_score <= cur_score || rnd(rng) < exp((cur_score - new_score)/temp)){
            if(is_swap){
                state.apply_swap(i, j, dists, dist);
            } else {
                state.apply_insert(i, j, k, dists, dist);
            }

            cur_score = state.score;
            ++acc;
        }
    }

    cerr << "best score: " << best_score << " itr " << itr << " acc " << acc << endl;

    return best_order;
}



}// namespace PartialSA

// ============== End of tsp2.hpp ==============



alignas(32) u16 dists[MMAX][8];
alignas(32) u16 dist[MMAX][MMAX][8][8];
array<uint16_t, SMAX> nxt_macro;

#define tim() timer.elapsed()

vector<int> tsp(const Macro& macro, const array<u16,SMAX> nxt_macro){
    double t1 = timer.elapsed();
    compute_distance_array(dists,dist,macro,nxt_macro);
    double t2 = timer.elapsed();
    // cerr << "distance array time: " << t2-t1 << endl;
    if(M > 16) return PartialSA::tsp_SA(dists,dist,0.01,1,0.01);
    else {
        vector<int> rem(M); rep(i,M)rem[i]=i;
        return tsp_bitdp3(dists,dist,rem);
    }
}

struct Answer{
    int score;
    Macro macro;
    vector<int> order;

    void output(){
        macro.play_precalc(nxt_macro);
        string out = ComputePath::compute_path_with_macro(order,macro,nxt_macro);
        cout << out;
    }
};


int main(int argc, char* argv[]){
    
    if(argc > 1) {
        timer.set_scale(stod(argv[1]));
    }

    get_input();
    cerr << "N " << N << " M " << M << " T " << T << endl;

    Macro macro0;
    rep(2) macro0.push_back(1);

    macro0.play_precalc(nxt_macro);
    double tsp_start = timer.elapsed();
    vector<int> order = tsp(macro0,nxt_macro);
    double tsp_end = timer.elapsed();
    cerr << "tsp done" << endl;

    cerr << "tsp time: " << (tsp_end - tsp_start) << endl;

    double macro_tl = 1.5 - (tsp_end - tsp_start); 
    cerr << "macro_tl: " << macro_tl << endl;

    // chokudai search

    auto eval_macro = [&](const Macro& macro){
        macro.play_precalc(nxt_macro);
        return dp(order,macro,nxt_macro) * 2 - macro.len;
    };

    Macro best_macro;
    int best_score = eval_macro(best_macro);
    vector<pair<int,Macro>> candidates;

    pqueue_g<pair<double,Macro>> beam[MACRO_LEN_LIMIT];
    beam[0].push({best_score,best_macro});
    candidates.push_back({best_score,best_macro});

    int itr = 0;

    unordered_set<u64> visited;

    const double noise_scale = 30.0;

    const int LEN_LIMIT = 26 + (N-10)/2;

    auto add_beam = [&](const Macro& macro){
        u64 hash = macro.hash();
        if(visited.find(hash) == visited.end()){
            visited.insert(hash);
            int score = eval_macro(macro);
            candidates.push_back({score,macro});
            if(macro.len<LEN_LIMIT) beam[macro.len].push({score + rnd(rng)*noise_scale,macro});
        }
    };

    while(timer.elapsed() < macro_tl){
        for(int d=0;d<LEN_LIMIT;++d){
            if(beam[d].empty()) continue;
            auto[score,macro] = beam[d].top(); beam[d].pop();
            assert(macro.len == d);
            // F
            Macro new_macro = macro; new_macro.push_back(1);
            add_beam(new_macro);
            // FF
            if(d+1 < LEN_LIMIT){
                new_macro.push_back(1);
                add_beam(new_macro);
            }

            if(d==0 || macro.op[macro.len-1]==1){
                // R
                new_macro = macro; new_macro.push_back(0);
                add_beam(new_macro);
                // RF
                if(d+1<LEN_LIMIT){
                    new_macro.push_back(1);
                    add_beam(new_macro);
                }
                // RFF
                if(d+2<LEN_LIMIT){
                    new_macro.push_back(1);
                    add_beam(new_macro);
                }

                // L
                new_macro = macro; new_macro.push_back(2);
                add_beam(new_macro);
                // LF
                if(d+1<LEN_LIMIT){
                    new_macro.push_back(1);
                    add_beam(new_macro);
                }
                // LFF
                if(d+2<LEN_LIMIT){
                    new_macro.push_back(1);
                    add_beam(new_macro);
                }
            }

            // insert LR at head
            if(macro.op[0]==1){
                new_macro = macro; new_macro.insert(0,0);
                add_beam(new_macro);
                new_macro = macro; new_macro.insert(0,2);
                add_beam(new_macro);
            }

            // remove head
            if(d > 6){
                new_macro = macro; new_macro.erase(0);
                add_beam(new_macro);
            }

            // insert F
            rep(pos,d)if(macro.op[pos] != 1){
                new_macro = macro; new_macro.insert(pos,1);
                add_beam(new_macro);
                if(d+1<LEN_LIMIT) {
                    // insert FF
                    new_macro.insert(pos,1);
                    add_beam(new_macro);
                }
            }

            // 隣接swap
            if(d>6)rep(pos,d-1)if(macro.op[pos]!=macro.op[pos+1]){
                new_macro = macro; swap(new_macro.op[pos], new_macro.op[pos+1]);
                add_beam(new_macro);
            }
        }
    }

    sort(candidates.begin(),candidates.end());
    tie(best_score,best_macro) = candidates[0];
    
    cerr << "candidate size " << candidates.size() << endl;
    cerr << "best macro: " << best_macro.to_string() << " score: " << best_score << endl;
    
    int last_len = 1<<30;

    Answer best_ans;

    // まず1つ解を見つける
    int ans_itr = 0;
    vector<bool> done(candidates.size(),false);
    rep(i,candidates.size()){
        auto& macro = candidates[i].second;
        if(macro.len >= last_len) continue;
        ++ans_itr;
        done[i]=true;
        last_len = macro.len;
        macro.play_precalc(nxt_macro);
        order = tsp(macro,nxt_macro);

        int score = ComputePath::eval_macro_with_len_limit(order,macro,nxt_macro);

        if(score < UINT16_MAX){
            best_ans = {score,macro,order};
            break;
        }
    }

    cerr << "first best: " << best_ans.score << endl;

    double path_time = 0;

    // 残り時間いっぱい探索
    rep(i,candidates.size())if(!done[i]){
        if(timer.elapsed() > 1.95 - (tsp_end - tsp_start)) break;
        auto& macro = candidates[i].second;
        if(macro.len > last_len + 2) continue;
        ++ans_itr;
        macro.play_precalc(nxt_macro);
        order = tsp(macro,nxt_macro);

        double t1=tim();
        int score = ComputePath::eval_macro_with_len_limit(order,macro,nxt_macro);
        path_time += tim() - t1;

        if(score >= best_ans.score) continue;
        best_ans = {score,macro,order};
    }

    cerr << "ans itr: " << ans_itr << " best score " << best_ans.score << " best macro:  " << best_ans.macro.to_string() << endl;
    cerr << "path time " << path_time << endl;
    best_ans.output();
}
// ============== End of main3.cpp ==============



