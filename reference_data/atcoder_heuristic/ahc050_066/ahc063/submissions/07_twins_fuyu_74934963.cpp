#pragma GCC optimize("O3")
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <cstdint>
#include <queue>
#include <cassert>
#include <random>
#include <iomanip>
#include <chrono>
#include <cstring>
#include <limits>
#include <bitset>
#include <functional>
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
template <class T>
using pqueue = priority_queue<T, vector<T>>; // 大きい順
template <class T>
using pqueue_g = priority_queue<T, vector<T>, greater<T>>; // 小さい順

namespace library
{

    uint64_t splitmix64(uint64_t &x)
    {
        x += 0x9E3779B97F4A7C15ULL;
        uint64_t z = x;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        return z ^ (z >> 31);
    }

    class xorshift128plus
    {
    private:
        uint64_t r0, r1;

    public:
        using result_type = uint64_t;

        xorshift128plus(uint64_t seed = 1)
        {
            seed_gen(seed);
        }

        void seed(uint64_t seed_val)
        {
            seed_gen(seed_val);
        }

        static constexpr result_type min()
        {
            return std::numeric_limits<result_type>::min();
        }

        static constexpr result_type max()
        {
            return std::numeric_limits<result_type>::max();
        }

        result_type operator()()
        {
            uint64_t s1 = r0;
            uint64_t s0 = r1;
            r0 = s0;
            s1 ^= s1 << 23;
            r1 = s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26);
            return r1 + s0;
        }

    private:
        void seed_gen(uint64_t seed_val)
        {
            // SplitMix64 で状態2個を初期化
            r0 = splitmix64(seed_val);
            r1 = splitmix64(r0);
        }
    };

    struct Timer
    {
        double timer_scale = 1.0;
        chrono::steady_clock::time_point start;
        Timer() : start(chrono::steady_clock::now()) {}
        double elapse()
        {
            double t = chrono::duration<double>(chrono::steady_clock::now() - start).count();
            return t * timer_scale;
        }
        bool over(double t)
        {
            return elapse() > t;
        }
        bool operator()(double t)
        {
            return elapse() < t;
        }
    };

    void fast_io()
    {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
    }

    uint64_t exp_table[2001];

    bool init_exp_table()
    {
        exp_table[0] = UINT64_MAX;
        for (int i = 1; i < 2000; ++i)
        {
            exp_table[i] = UINT64_MAX * exp(double(-i) / 100.0);
        }
        exp_table[2000] = 0;
        return true;
    }
    bool exp_table_initialized = init_exp_table();

    bool sa_accept(double improve, double temperature, library::xorshift128plus &rng)
    {
        double v = -improve / (temperature);
        v = clamp(v,0.0,20.0);
        int idx = int(v * 100.0);
        return rng() < exp_table[idx];
    }

    template <typename T, int MAX_SIZE>
    class StaticVector {
    private:
        std::size_t size_ = 0;
        T data_[MAX_SIZE];

    public:
        using value_type = T;
        using size_type = std::size_t;
        using iterator = T*;
        using const_iterator = const T*;

        // ===== コンストラクタ =====
        StaticVector() = default;

        StaticVector(std::initializer_list<T> init) {
            for (const auto& v : init) {
                data_[size_++] = v;
            }
        }

        // ===== 基本操作 =====
        size_type size() const { return size_; }
        static constexpr size_type capacity() { return MAX_SIZE; }
        bool empty() const { return size_ == 0; }

        void clear() { size_ = 0; }

        void resize(size_type n) {
            size_ = n;
        }

        void resize(size_type n, const T& value) {
            for (size_type i = size_; i < n; ++i) {
                data_[i] = value;
            }
            size_ = n;
        }

        // ===== 要素アクセス =====
        T& operator[](size_type i) { return data_[i]; }
        const T& operator[](size_type i) const {
            return data_[i]; 
        }

        T& front() { return data_[0]; }
        const T& front() const { return data_[0]; }

        T& back() { return data_[size_ - 1]; }
        const T& back() const { return data_[size_ - 1]; }

        T* data() { return data_; }
        const T* data() const { return data_; }

        // ===== push/pop =====
        void push_back(const T& value) {
            data_[size_++] = value;
        }

        void push_back(T&& value) {
            data_[size_++] = std::move(value);
        }

        template <class... Args>
        T& emplace_back(Args&&... args) {
            data_[size_] = T(std::forward<Args>(args)...);
            return data_[size_++];
        }

        void pop_back() {
            --size_;
        }

        // ===== 挿入 =====
        iterator insert(iterator pos, const T& value) {
            size_type idx = pos - data_;
            for (size_type i = size_; i > idx; --i) {
                data_[i] = std::move(data_[i - 1]);
            }
            data_[idx] = value;
            ++size_;
            return data_ + idx;
        }

        // ===== erase =====
        iterator erase(iterator pos) {
            size_type idx = pos - data_;
            for (size_type i = idx; i + 1 < size_; ++i) {
                data_[i] = std::move(data_[i + 1]);
            }
            --size_;
            return data_ + idx;
        }

        iterator erase(iterator first, iterator last) {
            size_type idx = first - data_;
            size_type count = last - first;
            for (size_type i = idx; i + count < size_; ++i) {
                data_[i] = std::move(data_[i + count]);
            }
            size_ -= count;
            return data_ + idx;
        }

        // ===== iterator =====
        iterator begin() { return data_; }
        iterator end() { return data_ + size_; }

        const_iterator begin() const { return data_; }
        const_iterator end() const { return data_ + size_; }

        const_iterator cbegin() const { return data_; }
        const_iterator cend() const { return data_ + size_; }
    };

} // namespace library

template <typename T, int MAX_SIZE>
using static_vector = library::StaticVector<T, MAX_SIZE>;

library::Timer timer;
library::xorshift128plus rng(3333);
uniform_real_distribution<double> rnd(0.0, 1.0);

const string UDLR = "UDLR";
const int DX[4] = {-1, 1, 0, 0};
const int DY[4] = {0, 0, -1, 1};

uint8_t to_1d(int x, int y){
    return (x << 4) | y;
}
pair<int,int> to_2d(int pos){
    return {pos >> 4, pos & 15};
}

int M;
int D[256];


struct Input{
    int n,m,c;
    vector<int> d;
    vector<int> f;

    void set_global() const{
        M = m;
        rep(i,m) D[i] = d[i];
    }

    void get(){
        cin >> n >> m >>c;
        d.resize(m);
        rep(i,m)cin>>d[i];
        f.resize(n*n);
        rep(i,n*n)cin>>f[i];

        set_global();
    }

    void gen(uint64_t seed, int n_ = -1, int m_ = -1, int c_ = -1){
        mt19937_64 rng(seed);

        bool ok = true;

        do{
            ok = true;
            if(n_ != -1) n = n_;
            else n = rng() % 9 + 8; // 8~16

            if(m_ != -1) m = m_;
            else {
                int m_min = (n*n + 3)/4;
                int m_max = (n*n*3) / 4;
                m = rng() % (m_max - m_min + 1) + m_min;
            }

            if(c_ != -1) c = c_;
            else c = rng() % 5 + 3; // 3~7

            vector<int> num_colors(c+1);
            num_colors[0] = 0;
            num_colors[c] = m - 5 - c;
            rep(i,1,c) num_colors[i] = rng() % (m - 5 - c + 1);
            sort(num_colors.begin(), num_colors.end());
            rep(i,c) {
                num_colors[i] = num_colors[i+1] - num_colors[i] + 1;
                if(num_colors[i] > (m - 5) / 2) ok = false;
            }

            d.resize(m);
            rep(i,5) d[i] = 1;
            int idx = 5;
            rep(i,c)rep(j,num_colors[i]) d[idx++] = i+1;
            shuffle(d.begin() + 5, d.end(), rng);

            vector<pair<int,int>> cells;
            rep(i,n)rep(j,n){
                if(i <= 4 && j == 0) continue;
                cells.emplace_back(i,j);
            }
            shuffle(cells.begin(), cells.end(), rng);
            f.assign(n*n, 0);
            rep(i,5,m) {
                auto [x,y] = cells[i-5];
                f[x*n+y] = d[i];
            }

        } while(!ok);
        set_global();
    }
};
// #include <x86intrin.h>
#include <cstdint>

constexpr uint32_t zob_food[256][8] = {
    {0xd6b38a90, 0xf8b6403f, 0x8f691f2a, 0xef852be0, 0x3bc57954, 0xcae478b5, 0xcc932a72},
    {0xa810e135, 0xdc991d7, 0x3bd08a48, 0x8493eb13, 0x545fb341, 0x2fe3fd21, 0x3ed5fd53},
    {0xef630cc8, 0x827db6da, 0x4b438302, 0xc916eb7f, 0xefaa5baf, 0x8b4a6c92, 0x810a609b},
    {0xa49865b5, 0x623fa518, 0xb66facf, 0xda25f3dc, 0x837ec176, 0xe97dbb3e, 0x91d42f03},
    {0xf108d0f4, 0xc0dd982c, 0xba30456, 0xb406f4ab, 0xc10205ed, 0xa8dfc898, 0x5bc57151},
    {0xe6c674d9, 0xf7139b7b, 0xef38fafc, 0xdc47ac7d, 0x6b942b23, 0x5a6d72c2, 0xd7baef47},
    {0xc244a00, 0x646ab9da, 0xc495b8dd, 0x1e20f984, 0x62648908, 0xdc508d5f, 0x6a73791c},
    {0x7ec6a3eb, 0xe1c0fb66, 0x5d91624, 0x421d33cf, 0x622db721, 0x56bfe9f3, 0xb02d8b12},
    {0x685b2ebd, 0xe431b323, 0xe59629f1, 0xee41106e, 0x5fceadf2, 0xc4881882, 0x83a3c77a},
    {0x7ea8fec1, 0x557d926e, 0xf07fc6fb, 0xb40b8a18, 0xd493b663, 0x9aeeeb, 0x400446ae},
    {0xc9e58ad5, 0x5af33820, 0xfa8d71cc, 0x8cbc0bb2, 0x281d4485, 0x55b17ce4, 0x42dd88a0},
    {0x8e486b0, 0x37e8e169, 0x879872bf, 0x52133dd, 0xbed13c6b, 0xb0c3e17f, 0x67a84897},
    {0x43b4e822, 0x42cb553f, 0x64eae0c5, 0x4e4262b8, 0x63efcc4b, 0xbeb7ff6a, 0x304458c8},
    {0x28a10051, 0x4ef5e30b, 0xc7bd4867, 0xe6bacd15, 0xd68b312c, 0x75adf2c, 0x2b5eaaed},
    {0xe8de597d, 0x2b7dc9a9, 0xb83e116b, 0x640c3cae, 0xb29d7aed, 0x9cea35d, 0xd99b5684},
    {0xab15ff2a, 0x2781fdbd, 0xbdb5f98b, 0x2da3fe76, 0x18df4f14, 0x7f96ada7, 0x15fe4a73},
    {0x35fa505b, 0x59213a8f, 0x9c47a49f, 0xc23aa26a, 0x53a6b1a1, 0x3d874c7e, 0xefe3a540},
    {0xe649388c, 0xf69e767, 0x2c183cac, 0xff3c22cd, 0x57438961, 0x75411c30, 0x1612bfbc},
    {0xd33e7273, 0x75ef51f1, 0xf48ab4d3, 0x4ea2dcb7, 0xeaafa1c4, 0xafd58622, 0x54550872},
    {0x7983a16b, 0xee3b61d1, 0xe3149a43, 0x40bc24df, 0xb35bfb0c, 0xa8b9b685, 0x219c06ae},
    {0x1aafba3a, 0x53bb693d, 0xcb161df8, 0xa7a7aeef, 0x6c55d0e, 0x708d6d8c, 0xa278a0c6},
    {0x46a94bc8, 0xe38d105a, 0x87b344e6, 0xbdb67762, 0x8497dd4d, 0x2fe507bd, 0x89a02434},
    {0xb81dc2e2, 0xd77cb2c2, 0xfe240f1f, 0xa48e6f3e, 0x7b0dd278, 0xb5bd667, 0x6acdc98d},
    {0xe07b686e, 0xc32c7f25, 0x94ec4018, 0x2f81f889, 0x72bd674, 0xac376bc2, 0x304f156a},
    {0xeba5eabf, 0x1a5d3d5c, 0xb8c85eaa, 0xb736eba7, 0xa356af23, 0x2169d2e0, 0x860037af},
    {0x3cfd1548, 0xf71ff4c6, 0x90018e7a, 0x582653ca, 0x4acfec51, 0xbbe442f2, 0xfb25c9e5},
    {0xbeac8fdf, 0x1933434f, 0xb80e6a18, 0x94e71173, 0x16752618, 0x8998f13, 0xd6ae222},
    {0xb8b093e3, 0x52bb6c2b, 0xac3f2234, 0xbe005155, 0x54385cd3, 0x9c04598e, 0xb1eea123},
    {0xc9efeeec, 0x61537829, 0x6a829a62, 0x82705627, 0x4928a797, 0x82468419, 0x1fc0d1c2},
    {0x7303fbe7, 0x4cb93fff, 0xde3e1130, 0x8a945234, 0x173f0b9c, 0xa69dcac5, 0x67612f3c},
    {0x705d7148, 0x9781b42c, 0x96b2612e, 0xf1be2c97, 0x1b60853c, 0x3372f531, 0x64f06974},
    {0x85d87751, 0x454c66a1, 0x6d4aeee3, 0xb956d998, 0xcadff5cf, 0xf9184d58, 0xcf12932d},
    {0x78a62ef, 0x52276dd2, 0x24413b00, 0x1f5cc613, 0x46888de8, 0x113e2a99, 0x3075e1bf},
    {0x959a7495, 0xa0522738, 0xe42d046e, 0x2fdbfdcb, 0x11772574, 0x699d1fa7, 0xf430aa98},
    {0xa218b6d0, 0xc8a4e599, 0x83e8ad6c, 0x734c6e5c, 0x56c3c3dc, 0xeeb258a1, 0x4bd008ab},
    {0x9aafbedb, 0xde7951e1, 0xb082186c, 0xba8690cd, 0xc2bc2700, 0x458bab84, 0x9dc2ae0},
    {0x8583e92b, 0x102a77f, 0x5921812a, 0x80fc6db8, 0xfa332f5f, 0xbf5e8a3e, 0x53efa5b7},
    {0xb6a8c13f, 0x81766892, 0x6ac2e08e, 0x16774bf0, 0xe5178e5d, 0x936bac6a, 0x2d866f84},
    {0xd587ddfd, 0x8df14d87, 0xcb20304f, 0x1153fe93, 0x7f5e6f82, 0x43753019, 0xc80cf1aa},
    {0x812b5571, 0xefdfb6f0, 0x64bd7415, 0xf0657183, 0x9b514187, 0x198054df, 0x192db4f8},
    {0x3b1013c9, 0x7493ac38, 0x7747dc7e, 0x63d14a00, 0x465f3ef2, 0xf461dc9d, 0xbe61185b},
    {0x8635d50c, 0x362b41b9, 0x4dda022d, 0x1fa1fa85, 0x942b8850, 0x9c004f54, 0x27f0332c},
    {0x1dabc1c0, 0xced31493, 0x331c04b3, 0x683c8e44, 0xe1e33f4e, 0x58e416a6, 0xbaab0468},
    {0xf04ccb65, 0x345bbc5d, 0x1a5e103e, 0xc76a545a, 0x1d2844bf, 0x33e1c749, 0x4ac89e3d},
    {0x342898cb, 0xdbb615e6, 0x7ca3036c, 0x1644782a, 0x2e0c5e52, 0xa9b09521, 0x19e88312},
    {0x75b73508, 0xd41c857c, 0xc91de558, 0x7b4c99cd, 0xa9ad1e2, 0xccbb0bde, 0x1356564f},
    {0x9baf98c2, 0xad08855f, 0x57e59774, 0x5242a1bf, 0xe09e64fe, 0x546808a1, 0x69df6f5c},
    {0x68b7ba94, 0x9dc10d79, 0x8edcc971, 0x8060acfd, 0xd2107b12, 0xb8624dbb, 0x5e424813},
    {0xd004b63b, 0xdc65e81c, 0x1e5106db, 0x3bad2c66, 0xf1a6ad3c, 0xae3269e7, 0x9f3a342b},
    {0xeb37f9, 0x62de9320, 0x3db768fc, 0x1fc571c6, 0x52eb3ad4, 0xe56c9f26, 0x73dae9c1},
    {0x82ea7704, 0x1ae5e7fc, 0x477a78d3, 0xc690d3f0, 0xcab65130, 0x7cc6d51a, 0xd907293a},
    {0x360b3a67, 0xd1e0a22f, 0x50e35673, 0x8a3b83b6, 0x75c2084d, 0x8ef3054f, 0x37598e26},
    {0xd8d82074, 0x3809926a, 0xbff47e75, 0x8fee9e29, 0x5710327a, 0xee13cdcf, 0x9b0275a3},
    {0x58518bf2, 0x48451481, 0x9ae58f3d, 0x966ca56f, 0xc9a8fd57, 0x6e101489, 0x6004e9d3},
    {0x32c9293f, 0x7a78bebd, 0x19f288d2, 0xae75f97e, 0x637ad850, 0x44228fbe, 0x2d15a261},
    {0x69741c95, 0xd049ad86, 0xd2ce8707, 0x95753e12, 0xeef0c7f0, 0x27560892, 0x1d13aea5},
    {0xc4f5b56d, 0x2e7652bb, 0xa4c92bd1, 0xc3073fa3, 0xbda75ad9, 0xd43443c3, 0x1aad66d},
    {0xbc7fd7a, 0x8f1af75e, 0xae162dda, 0xdbb8ad08, 0xa32a7cc4, 0xf0978e4c, 0xfbd7c1c},
    {0xf25854c9, 0xba5c95e4, 0xa24aafa1, 0xb77ca69d, 0x745b56e, 0xf25256bd, 0xd8e5794b},
    {0x17b938b5, 0xce79b717, 0x90f1ac2c, 0xd16b292b, 0xb38f6563, 0x8a70db2a, 0x74643bdc},
    {0x2f0eeb23, 0x17e3e4d3, 0x55000b10, 0x61e0a42a, 0x402e1ce8, 0x11fee5c, 0xacb75f50},
    {0x231b46c8, 0x1b08d448, 0x6611012c, 0x21cda5f, 0x4c3b087a, 0x7351a8ec, 0xbbd190e4},
    {0xa281c193, 0x67d70793, 0x7b266eb8, 0xe0cb491b, 0x72176720, 0x94a80601, 0x8543fd56},
    {0x963ddc1b, 0xd2ad3587, 0x9de80311, 0xed9b6760, 0x244d746, 0xdbbfd240, 0xe0d15413},
    {0x1cf611fb, 0xa2459aed, 0x6558ee1, 0xee4b8500, 0x93fd0ac1, 0x8355b960, 0x20f9535e},
    {0x53c138bf, 0xbe6282f9, 0x24b664c2, 0x877c9192, 0xf3a21ace, 0x155a6e75, 0x5062619a},
    {0xe1ba4750, 0x82e92009, 0x918b102e, 0xaa616fc2, 0x429f61f, 0x3bfafd98, 0x819144fd},
    {0x207615ee, 0x41a5ccf3, 0x6cc362fa, 0xbc0cb186, 0x8a22df76, 0x24e698f1, 0x5a5dd924},
    {0xf8036480, 0x5202e240, 0xb2c0de7f, 0xfc472014, 0xc5ead8b6, 0x2e9beb5e, 0x88ddf179},
    {0xa51f77f5, 0x895848e6, 0x3b9fbb21, 0x14d214a, 0xf277d6f4, 0x97eb6f43, 0x4b60d9d5},
    {0x9a4d4489, 0x131112f3, 0x2c3ad86c, 0xeff0fd09, 0x54a8bd4d, 0x81c53487, 0xb1269398},
    {0xd8b6c160, 0x2bc287e8, 0x93b163e5, 0x52171d39, 0x95652695, 0xd1f3ce84, 0x3a3a56d6},
    {0x8cafcee4, 0x84e59ffb, 0x40e87faa, 0xc08c377e, 0xfa5da335, 0x30a4d71a, 0x83a06795},
    {0xb7d432b7, 0x24f2a6b6, 0xfdb66969, 0xb7f9d80b, 0xe1b8f5fa, 0xa797e155, 0x1a7da018},
    {0x8387aed0, 0xe84169b7, 0x6d4cb75d, 0x7ec4f6e1, 0x814ee46b, 0xdc2ca608, 0x8ffc855d},
    {0x4e97107a, 0x82da1c72, 0x22a1c9d, 0x5c712da8, 0x34c4fbb6, 0x35b5e2a8, 0xfe8252d7},
    {0xcff41ab9, 0xaad1755a, 0xfc5bcf45, 0x97cce850, 0x4f3641cf, 0x10dda77a, 0xe7bc058},
    {0x27fff1d2, 0x14263a84, 0xa2b100f8, 0x9ab57707, 0x4b32ed6, 0x6e6a34e0, 0xde3599d3},
    {0x61cc36ab, 0xd8bc6399, 0xdccba903, 0x847f7b03, 0x7f71f880, 0x3e294e72, 0x2545ade8},
    {0x4465bd5, 0x57ed012e, 0xe180e91a, 0xc9577c9b, 0xc53664a0, 0x2873e230, 0x9559781},
    {0x4b691fc, 0xc25c6850, 0x20e14948, 0x4fee433, 0x437ecda6, 0xae72d551, 0x95ee1bee},
    {0x63eccfd0, 0xd536aec8, 0x446ec5b0, 0x66179b85, 0x42bc41f4, 0xe562ea40, 0x9322d09d},
    {0xa6fb013d, 0x50ec129d, 0x4485973e, 0x40bdfb1e, 0x6ef3b087, 0xed5fdec6, 0xcd7bff1e},
    {0xcc8b7dd, 0x3cc3d8da, 0x5482cb33, 0x15e9eb92, 0x636584d5, 0x7dde8eb5, 0x45163ef},
    {0x54ecdb0d, 0xee10f8f2, 0x1f1cfe8e, 0x112a272c, 0xabd12df7, 0x2c725314, 0x3db90a21},
    {0x7d4c353b, 0x2b39a8c5, 0x2a8f90ec, 0xf95582de, 0x357d61b7, 0xa79a2538, 0xd47ad3f2},
    {0xa411c648, 0x886af22f, 0x741d8f6e, 0xb7ebf175, 0xfa118faa, 0xf3decfb0, 0x878892c2},
    {0x5f00c81e, 0x4c66a94, 0x6ba04efe, 0x66e2ecbb, 0xcb35f72, 0x83a77c69, 0x1643bda},
    {0x7bc5a96e, 0xe2aa4cf2, 0x58b9cf70, 0x23b1a0, 0x3cdd82a6, 0xb7c945cb, 0x4e8fb304},
    {0x2ce32d4, 0x4e268b87, 0xfdb79e5b, 0x418292e2, 0xd55ec614, 0x8e7f01c6, 0x2ea99c2e},
    {0x82047d57, 0xcee5948b, 0x9740357d, 0xa1fe88fb, 0xfb019651, 0x9ab09bfa, 0xf95775e4},
    {0x76efa3c5, 0x6e558bde, 0x9198d77d, 0x3a53c69b, 0xd1ba64cc, 0x1ec6b881, 0x5c523e49},
    {0x9e2299c8, 0x7f37a839, 0xb9287104, 0x47aeb3a8, 0x945329e2, 0x5c8eb183, 0x9145c038},
    {0x73ed4bc3, 0xc290fb3b, 0x60c0e68, 0xd8d27c6f, 0x8620a094, 0xd4380078, 0x10d6053d},
    {0x94c240f0, 0x15156fe2, 0x3a399c87, 0xf50940c7, 0xdbfb94b2, 0x1214f298, 0x771bdcb2},
    {0x45072e20, 0x200d4b48, 0x9e17437f, 0xfd816dbb, 0xbc0cf7ee, 0x5f2d8b0c, 0xa402a4c6},
    {0xecc9a648, 0xf3b52900, 0xe2e65efe, 0x9bf8f19c, 0xee0936ab, 0x842a9019, 0xa0f6f42a},
    {0x52a77d4b, 0x773e68ad, 0xfd109c27, 0xfe092efb, 0x73a01980, 0x7bc80fcd, 0xd2f6c6f0},
    {0xc301cbda, 0xccd8f7cd, 0x804144a9, 0xd65bdda5, 0x3ace6b2a, 0xadcaac51, 0xca1a554b},
    {0x1357d3df, 0xa2abbccb, 0x4194aa8b, 0x4190c9d0, 0x97c97ea9, 0x2e31a682, 0x3182ae67},
    {0xf0c75682, 0x2244ebd0, 0x5c025016, 0x66b6df27, 0xda98c900, 0xbcd59fd4, 0xc019cb2},
    {0x19fc575d, 0x86d0f783, 0xca7fc0ee, 0x5539ee21, 0x2d2ba7a2, 0xab1d66a2, 0x7fccd58f},
    {0xaa317999, 0xefdca495, 0x859c36ae, 0x60c40cb6, 0x953245f5, 0x49ce47a5, 0x5e2c58aa},
    {0x3880122b, 0x41916de0, 0xf5251163, 0xd291987f, 0x2adc3c0a, 0x15288e1d, 0xad542f},
    {0x6cbeabd4, 0xe981dabf, 0xd7366a2f, 0x278b5b22, 0x92df836d, 0x78c33801, 0x2de86aa1},
    {0xbdbc2056, 0xf3c8b52b, 0xad52cc6e, 0xb2c796c8, 0xad89864c, 0x6b2f5dc9, 0xf809fde2},
    {0x1d4eda81, 0xc1a9d28d, 0xd9e03fc, 0x26c41cc8, 0x6e91a4, 0x9373852b, 0x85f44c78},
    {0x54eb0a15, 0xbaec064, 0xb0a0477b, 0x5f29165a, 0x187e8eac, 0xb387b3dd, 0xf49f25a9},
    {0x62ff517d, 0xdc674033, 0x6cced3ce, 0x8401fb75, 0x4cb0d3af, 0x84dac2b9, 0xe2b73763},
    {0x667844dd, 0x20619435, 0x466fa883, 0x11c57e5f, 0xc033ac1e, 0x748ec460, 0x17a7046f},
    {0x422a5f88, 0x7a1d7ca8, 0x603c4454, 0x6cd4382a, 0x75d27bbf, 0x533a1ddd, 0xcfa27fda},
    {0xddfbea59, 0xe1aa5f5c, 0xfdbd7354, 0x781397c7, 0xd3aa1feb, 0xca26d0a2, 0x26d3362c},
    {0x8708155e, 0xe08e8e78, 0xaa63e51d, 0x58c9d25e, 0x1a9b0d77, 0x3a03fe57, 0x1fd8c74f},
    {0x2fd92b24, 0x42362365, 0x96ae0772, 0xa28f843d, 0xed409eae, 0xbe0befbd, 0x5ed1622a},
    {0x3e023b69, 0xe8476dc9, 0x25d3e1d5, 0xe3186264, 0x1dd6136b, 0xeaa27f3b, 0x851e78e0},
    {0x3280eaa1, 0x6b99e87f, 0x7b27010d, 0xaeecc0b8, 0xe047ea30, 0x73f16935, 0x4a770580},
    {0xb3e16a9a, 0x67a7e060, 0xb598a7c0, 0x74442c44, 0xd2ac533a, 0xa4f23bbb, 0x707b2dd4},
    {0x1df1ebf4, 0x1d5ed221, 0xeab79a4a, 0xd997fed9, 0x3ca0a28d, 0xa334c0c6, 0xe560dce},
    {0x888a01d0, 0xee7f6020, 0xa180d013, 0x2c6fd85f, 0xf2b6016, 0x96231ed0, 0x46a7fe7},
    {0x5de7b071, 0x7d98808e, 0xf2a65801, 0x1f43e96, 0x67674a8e, 0x86825000, 0xed1d3329},
    {0x81e5c8a, 0x146d1ee2, 0xf5cae520, 0x2f6843aa, 0x93ee3a56, 0xdf68928b, 0x342af496},
    {0x3e0e079d, 0xef149b91, 0xfa34d66b, 0x36612e1d, 0x9b973507, 0x65d79aa8, 0x5d6ddec1},
    {0x33678ba3, 0x6cdd77f8, 0x7eac6fdd, 0x9844ad99, 0x78c641d7, 0xe2210f8c, 0x516c4f5c},
    {0xdffd108c, 0xa3794719, 0xa36b9569, 0xe9a769d9, 0xb21a9bb2, 0xe1df4ee3, 0xfd731af5},
    {0x2edc5e64, 0xb2bf1d60, 0xc392129f, 0x3c55222a, 0xec00cee3, 0x8c3698b0, 0x8f279001},
    {0x4e70cb8f, 0xf654e6f, 0x55bd6237, 0x5e83141, 0x8365376f, 0x3bcd0f9, 0xbb133870},
    {0xeafc1de0, 0xcd70338d, 0xaf405a47, 0x6ae53446, 0x4b3ae7e5, 0xce859cb9, 0x607b4ea9},
    {0xfab74daf, 0x849a7c86, 0x42ead14d, 0x1a7dd55d, 0xdc2f792a, 0xbe94ea3d, 0xb99f5528},
    {0x3fc2d409, 0xe0aed6e6, 0xd7959777, 0x9d02ad3e, 0x9c51f590, 0x6d5b598e, 0x8a222073},
    {0x159a6789, 0xd3c6186c, 0xd71b958f, 0x3d6af2c2, 0xde1d795e, 0xc5a6f6c1, 0x3dc60060},
    {0x7faae082, 0xb71bb620, 0x4fcf9dd3, 0x3cf273d0, 0x18b495b2, 0xa7c1b92a, 0xba0a3a90},
    {0x8f4c58d6, 0xf072416c, 0x670cb27d, 0x51ae6ccd, 0x9808a931, 0x151c6f6f, 0x4b376e98},
    {0x700db42a, 0x81ec2510, 0x449a657c, 0x6dd22307, 0xc2e5af42, 0xf0e4d2b7, 0x8191a169},
    {0x3c84ea27, 0x4741053d, 0x34cd50a, 0x69a83955, 0xbae28627, 0xfc9ce3b0, 0x1e72330e},
    {0x156304c2, 0x7ff0e22d, 0x97b0b496, 0x9965a60a, 0x1220a348, 0x367fba2e, 0x398b1728},
    {0x175f923a, 0x137d2347, 0xcb4868bc, 0x2c58244a, 0x84ffb3de, 0x7b7770ea, 0x8cbe5da1},
    {0x8603cb21, 0xbd45c40b, 0xef17fe6b, 0x2785e628, 0x67e6bb6d, 0xcb68c160, 0xff500215},
    {0xbeb9c5e9, 0xca454f2b, 0x5b918658, 0xfa43a1af, 0x1c396674, 0x5f143c43, 0x43fbfa35},
    {0x7383afa6, 0xba8769a8, 0xc9660631, 0xdde17c71, 0xbc174355, 0x291564a0, 0x37cc0fd1},
    {0x70f6cbdd, 0xff281fdf, 0xf65dd01d, 0x259e7262, 0xc0408611, 0xf7b6ce22, 0x1874f4f0},
    {0x31da4677, 0x8302c117, 0x11268d22, 0x484ec74d, 0x111314b0, 0x15c4977c, 0x75b17ae1},
    {0x42031eeb, 0xac63cbea, 0xb6545d3, 0x39ceac10, 0x803353b, 0x1811a1b0, 0xde13c992},
    {0x7fdfe1df, 0x93985aed, 0x88514319, 0x20132f8, 0x68ee72ca, 0xf7270754, 0x830ddafc},
    {0x79087e46, 0xf27e4d5d, 0xbb1fd96b, 0xed0ec29c, 0x3d62604, 0x87b2cfe, 0x3e25ce91},
    {0x2b2812af, 0xfb8aa04a, 0xe62132ca, 0x8c7e3ead, 0x11c5dd0c, 0x411bfe2, 0x1f5d9a42},
    {0x595a1ee6, 0x89ebb7be, 0xbb43a291, 0x5e0f3d29, 0x89d6120c, 0x4442ec67, 0x8824111d},
    {0x13791e25, 0xc09d6b1d, 0xf990a85c, 0x2c8d0bb9, 0xe803465f, 0x27b7d571, 0x7a27725b},
    {0x9e2ae732, 0xbd659efa, 0x26c77f4f, 0xd35cc7f7, 0x18e1f677, 0xf213eb5d, 0x2ec81b88},
    {0xcdcd0b3f, 0xe95a58d2, 0x1129d35f, 0x185567c6, 0x1cc23a79, 0x717852ea, 0xb1c8ca4c},
    {0x3e34f525, 0xc4e3bbea, 0x6962e5f3, 0xb8678838, 0x9f3c3941, 0xf7de4edd, 0xe7d6a37e},
    {0x8561ff33, 0x956f51dc, 0xed649156, 0x42826a3e, 0x1f7a5992, 0x3993f542, 0x68a91266},
    {0xe305c344, 0xfd0392ab, 0x94ea8af6, 0x8602565c, 0x12b7ba9e, 0x2b3dc27b, 0xc4aee043},
    {0x4676db00, 0x8bd59ac3, 0x32b92e35, 0x159f2120, 0x1685f53a, 0x90c407f4, 0xeedc395d},
    {0x566431d7, 0x22f8bc98, 0xcf2f50ed, 0x786a3884, 0x3dad35a0, 0xd9915f18, 0x490fc476},
    {0x98b34760, 0x77c36594, 0x165a7672, 0x24e8e726, 0x1bc570ad, 0x675ce1b9, 0xca9de36e},
    {0x4a53ea24, 0x9fae493f, 0x9c28f72b, 0x1b0602d6, 0x21df2c33, 0xa2f06075, 0x265bec2a},
    {0xccc7d63c, 0x4443ad49, 0xc76f0d5e, 0xda46f809, 0xd16106e8, 0x3b711443, 0x23258eef},
    {0x13a37c4b, 0xd92813de, 0x7f8111c6, 0x3c57ec8d, 0xa584b478, 0xfbeb5097, 0x1760588d},
    {0xd104a02e, 0xa6a935ad, 0x6d9a73f8, 0x9246f702, 0x4d760bb1, 0xf60be509, 0xd1e1cb08},
    {0x30d799da, 0xbbc7f27b, 0xc89655a3, 0xafcdf382, 0x2fbd3bbc, 0x49756451, 0xccf73e1e},
    {0xbc4abe9c, 0x19d3c160, 0x3fd06236, 0xe7760bfa, 0x422d0ea0, 0x9775937a, 0x241e6e1c},
    {0xc342903a, 0x57ec6d8b, 0xadefaf01, 0xa35ee99f, 0x3225530c, 0xfd279e59, 0x3b2078e7},
    {0xfbbd2f17, 0x459a34c2, 0x94840032, 0x2ad4872e, 0xa2c8b09e, 0x5c8a515a, 0x1a7b20},
    {0x9dc15fb7, 0xdd0f92c3, 0x97c76664, 0x12105f29, 0xcfec51a, 0xd6fd6309, 0x1b0805b5},
    {0x1ae5b77, 0xef55abd6, 0xdfd54e1, 0x6b95d7df, 0x3a48a433, 0xc0cc3782, 0x63546bdd},
    {0x9514ff1c, 0xa370c56c, 0x795853b2, 0x4738dad9, 0x770220ac, 0xb91b5b79, 0xd05c874b},
    {0x98218e88, 0xb7bb7ab6, 0xf2b25c5d, 0x46c60537, 0xd5f15fb, 0x5243ced7, 0x8b87b554},
    {0x5d6f46be, 0xc1d315d7, 0xbccf0068, 0x52ff2b5c, 0x75674926, 0xc1e763a2, 0x48693613},
    {0x66e04272, 0x25332d18, 0xc7027b73, 0x85dc8878, 0x336f9437, 0xb19d14df, 0xa3a52ad},
    {0xbc39d1b2, 0xb63de396, 0x79ab0a5a, 0xace027b0, 0xb8cb943d, 0x5e33e916, 0x7e139ab1},
    {0x4fa771c6, 0xbd93dd29, 0xc98b6f75, 0x4128620c, 0x9b02e8ed, 0xa4f4e590, 0x3a4adfb8},
    {0xe088c0f3, 0x401a5430, 0xad641afd, 0x6a8e549b, 0x4e305f8d, 0x3f18eb7, 0x48919f01},
    {0x3bf22e03, 0xd41755bf, 0x67dba0d2, 0x15bb5552, 0x219326b5, 0xc211818e, 0xe18d95c9},
    {0x3d12752b, 0x1ab1a89a, 0x8b1549ac, 0x2a574ccd, 0xc9db31c5, 0x9bc64dff, 0xf2d4b8f8},
    {0x67a723e6, 0x3c6a8c22, 0x46af135a, 0x8c3703a0, 0xe6931c92, 0x836b7238, 0xbdb55763},
    {0xf44a260c, 0xf3bf4328, 0x4daccd69, 0x3626820a, 0xe21fd852, 0x6319e709, 0xe2e877},
    {0xc4110d9a, 0x74a34f5a, 0xdfa8d979, 0xb6b92f8a, 0x9b93d6aa, 0x61589d54, 0xa6a6691a},
    {0x3afd9c84, 0xa6d8b97c, 0x37e1fc0b, 0x5a447bbc, 0x738b0bae, 0xd03ef55b, 0xe481ca19},
    {0x5f680750, 0x5a2d7332, 0xdf1a99c9, 0x1dffa90c, 0xfd251af, 0xae3318d8, 0x31577344},
    {0x56e266f, 0x8736b701, 0xca318a01, 0xc14481ab, 0xa1749cbf, 0x306fa487, 0x1f491d6a},
    {0x32713e6c, 0x1b4129ec, 0xa3fff0c1, 0xae900366, 0xf9f08960, 0xe4ecfdb6, 0xfa74ebe9},
    {0x6ae6bd99, 0x87e91cfd, 0x2f9046b4, 0xe4847721, 0xb9a2ca08, 0xa42a78b8, 0x72e570ec},
    {0x7c493349, 0x4a5694f9, 0x6725093e, 0x9d593d99, 0xf1524d5d, 0xe335d5f9, 0xf6ef6e},
    {0x61bb04c2, 0x3f59201, 0x4a1226c5, 0x4e2dc2c5, 0xfdc087f4, 0xfb5a1a1d, 0x433adaa8},
    {0x54ae4ce8, 0xbfece363, 0xf5d6b7a9, 0x75ca6a42, 0xe457215c, 0x3ff3278d, 0xfe4d2cb7},
    {0x988f4485, 0x553fe53c, 0xde663790, 0x8f77e1eb, 0x35e6578b, 0x9951ac72, 0x1269bc86},
    {0x361cfb3d, 0x55d61e38, 0xab6000d5, 0xf03c7cf6, 0x651b3b96, 0x5ae30003, 0x8f3a85b4},
    {0x3cac1533, 0x24fb5aa4, 0xad1e91db, 0xd40fc023, 0x84e2ec29, 0xfa35c5fd, 0x1db261dd},
    {0x3413570c, 0x878a4e12, 0xd3cab5b9, 0xf00e717, 0x5b76a6a0, 0xc68b4f69, 0x34e96628},
    {0x35fc8656, 0x8f7338a3, 0xfff1dfd1, 0x1aa7e886, 0x5f27240e, 0x9e7c3cc0, 0xcd017381},
    {0x74b5356c, 0x3ee6270f, 0x33fb94d3, 0xdf31364, 0xe0aeca58, 0xe01fcfeb, 0xa69f820a},
    {0x92f425d2, 0xfb8ff159, 0x2b0fae17, 0xbd436475, 0x7ce2ffae, 0x17943d08, 0xf4d9aa97},
    {0xf5a723ff, 0x69ea869, 0x87b86711, 0x7dd3991e, 0x7e071bc2, 0x983b7ba7, 0xf239a5ab},
    {0xe4b663bb, 0x24e05a30, 0xa36718b3, 0x66b0f56f, 0xbe5ff08a, 0xc91a0964, 0x3a417ebc},
    {0xcbed06a2, 0x28cdbb8f, 0xfee2a3c3, 0x4bf73730, 0xc1a25f92, 0x52c3d2a2, 0x804fe131},
    {0x2c365c01, 0xbc0f81c8, 0xd2394b25, 0xb0b72216, 0x76a740c, 0x802c3bec, 0x8676f8bb},
    {0x6ac7ef8a, 0x51fb224f, 0x30164e1b, 0x209962bb, 0x61a4c0f3, 0xc9063208, 0x2960f7},
    {0x8f428859, 0x7cb03ec1, 0x4aa4189f, 0x4833182f, 0x8578ac6f, 0xbe7b8734, 0xb983f96},
    {0xd7e383bc, 0x38f47c3, 0x88c509e1, 0x5d980b71, 0x2cdb43ae, 0x30177095, 0x67b2112},
    {0x610fbca7, 0xc99ba5bf, 0x64d7946d, 0xd15811fc, 0x105dd758, 0xf08c7331, 0x1a67c6aa},
    {0x2ee06de, 0xfef2e3b, 0xe1f196a8, 0x19b41ccf, 0x277c584b, 0x666c7b6, 0xc09cb7ad},
    {0x154177bb, 0x3db18d70, 0x2e98fd07, 0x67f3c93f, 0x3849c4ea, 0xe477c086, 0x8a8f977e},
    {0x2b531432, 0xb5076766, 0x61e09861, 0x4337812d, 0xaa66f7ab, 0xbbf46b7c, 0x972d88c2},
    {0x15a3c971, 0xac7b385f, 0xf33111f6, 0x1666f899, 0x60a386d8, 0x7ec9481f, 0xb065d27b},
    {0xb5c7e779, 0xbd215c0f, 0x4fb1602b, 0x3c71efeb, 0x2194a98f, 0xd38cb22a, 0x4015aef7},
    {0x88a46200, 0x60c7c0f6, 0x2a1acbff, 0xb2253b0c, 0x5ed3ce0a, 0x651aafe5, 0xf39292d9},
    {0xe1be38d1, 0xc203b9fc, 0x544e58ed, 0xae3f1e9, 0xa3a6610a, 0x8a5774a1, 0xee5dfd9e},
    {0xaeb5b6ab, 0x1a3b295, 0xfe130773, 0x755b6ba3, 0xef1855a6, 0x9cace5cc, 0xa4a48494},
    {0xedba5ae, 0xe3c7f0ca, 0x498e3531, 0x41812b4b, 0xaadda4ae, 0x4ade79bd, 0xb3e36622},
    {0xe4b0322d, 0x4240442e, 0x13bd3cf5, 0xe25f9294, 0xe88cacff, 0x6c61cc03, 0x6ade6e54},
    {0x13dcc77d, 0xe091bc5a, 0x9bd8983c, 0xd6ee20fa, 0x948164a1, 0x39e3e190, 0xf890c553},
    {0x14121d2b, 0xcb26a63a, 0x1f545d7f, 0x92c1f309, 0xfd292abc, 0x55207747, 0xb32e043b},
    {0xbdde74ec, 0xd6a07b17, 0xa73aa16f, 0x1b992487, 0x6ae9231c, 0x48bd545b, 0x116275a},
    {0x2b7d16a3, 0xd5a0efc, 0x20f625fd, 0xf704b4f7, 0x11b91b58, 0xac9ba3a1, 0x6b102869},
    {0xa56350b8, 0xad6064f8, 0xea27b970, 0x23e1107e, 0xec915b13, 0x3186711d, 0x28f263e0},
    {0x70790b78, 0x75363118, 0xd5b4727c, 0xd1a6a27a, 0x231f5d27, 0xfce14dbc, 0x107aa682},
    {0xbb436f55, 0xdeb555b6, 0x220d2a97, 0xa0d0e896, 0xc5797296, 0x320755d8, 0x5cc6ac66},
    {0xc12819bf, 0x36a096b6, 0xf04c0fb7, 0x6076f74d, 0xcd1e0b76, 0x316f993e, 0x938afb69},
    {0x7eda6df3, 0xbb1db7ef, 0x84cbac03, 0xecb00213, 0xc9715f74, 0x15acf1fb, 0x26d6ffcd},
    {0x52e3c495, 0xfab59c65, 0x7c7f7675, 0x48e9da1d, 0x98e479ac, 0x73a25a99, 0xfc6f858},
    {0x55d851ec, 0xe592a0ac, 0x15635bb6, 0x522f5b50, 0x68f05e95, 0x3ac967a4, 0x7a147138},
    {0x2a4246b9, 0x3e250b37, 0x17045e3, 0x2e54750c, 0xc54fb911, 0x605f7bb4, 0x41c18ad0},
    {0x7c141fc1, 0xce6a6eba, 0x4824b92d, 0x51ab953d, 0xe958436f, 0xb57d0bb0, 0x8e3e0d5e},
    {0x49eb0444, 0xaef9f6fa, 0x462bae1a, 0xc13ac5c8, 0x7c102617, 0x632bfecf, 0xf7d37fbd},
    {0xa75b8ebf, 0x3616c2a4, 0x9b2685df, 0xd38468d9, 0x35f6a0ca, 0xc530a79, 0xb66fb536},
    {0xcc0b1f18, 0xe5e7c5fd, 0x59ecd140, 0x28f92245, 0xd8861163, 0x3f972ad2, 0x3e3aaded},
    {0xf2682764, 0x6dc12746, 0xba036ebb, 0x3eac4ce4, 0x118d59e2, 0xa913c27c, 0x7cd8df5e},
    {0x1e82e079, 0x59fd1f8, 0x4aeb04e5, 0xd85b7197, 0xa05db2b, 0x3a170a55, 0xc086e2c2},
    {0x2916682, 0x4d3ae8aa, 0x7c8d8509, 0x4083b9d9, 0xebe37789, 0xad1c417e, 0x225df6b},
    {0x540886, 0x2e522ef6, 0xa748755a, 0xff077e2e, 0xd41b74d8, 0xd67f57ef, 0x5fdaef53},
    {0x6ea3296c, 0x3c866b72, 0xd0ed3f07, 0x1efcd80a, 0xd6c55085, 0x464751d2, 0x95f916ab},
    {0x988699ad, 0x25996a33, 0xab5adb9c, 0xe30b191a, 0x6f5aa437, 0x3c74cf02, 0x89b9d71b},
    {0xf2c8deff, 0xf85b3965, 0x97f6a251, 0xe921c04b, 0x7137ebfc, 0x1ee1d355, 0xb492930a},
    {0x79f37a9a, 0xad318a37, 0xe051bff6, 0x8d17e6be, 0x20ebce91, 0x39cb1e20, 0xca5a69df},
    {0x29cd3d9a, 0xc1b417f, 0xf403abf4, 0x5da6b45a, 0x67707787, 0x5eb14187, 0xea20f7e2},
    {0x2fbc4760, 0x507b65cb, 0x6cc1f125, 0xed0686dc, 0xcbd9b81, 0xa16dce2f, 0xe99b458f},
    {0x96a5224c, 0xf7ac2f1e, 0xb1e354e5, 0xc9ce1ca2, 0x3630b72c, 0x9f2ceba0, 0x40c62112},
    {0xd6791952, 0xf980a8eb, 0x803bc57f, 0xd6d7212, 0xe2ab5bc0, 0xbb1630af, 0x1f23bb9b},
    {0xc8cbbed8, 0x36229f8b, 0xfff1a6d4, 0xe63dc261, 0x5ceeb080, 0xbfeec659, 0x3acab066},
    {0x93e36fa7, 0xa386101f, 0xfb0b5945, 0x313850d, 0x42859c7a, 0x65be40c2, 0x31933afe},
    {0x4e1e142c, 0x6e788205, 0xf2dd62a5, 0xd2e00907, 0x206a33f8, 0x5a17f88d, 0x996e2dc4},
    {0x1e79e989, 0x5553afc5, 0x73d3c3a3, 0xd3314967, 0x85568966, 0x461ace9, 0x1d4994a4},
    {0xa9686a68, 0x5de346b9, 0xf498b1d6, 0xa21cddc2, 0x11bad8fc, 0xf990bc59, 0xdd533106},
    {0xad498b75, 0xc7997b3c, 0x411cabc2, 0x4a5178b5, 0xf84ed187, 0x2b8135fd, 0x5f9f78fa},
    {0x79190ea1, 0x5e5077f1, 0x158db88a, 0xd1faa5e, 0xaf593302, 0x2a78b61b, 0x3739b071},
    {0xe02b926a, 0xc4b5487c, 0x6e02a39d, 0x2b4f80aa, 0x2fdeb305, 0x7d29ba05, 0x21f74bd8},
    {0xaf0d552d, 0x6a52bad, 0x290c6334, 0x45ccfd5f, 0x5a9c73cb, 0x6dcec3b5, 0x7fc189d5},
    {0x32fdca36, 0xa381fa5f, 0x56d06da7, 0xf619d2a0, 0xe0a9f440, 0x10f47ae0, 0x5a602333},
    {0x2d2a18d4, 0x885f4b1f, 0x634f3d45, 0x9a1c6233, 0xb0a83791, 0xb23961d5, 0xae6f903},
    {0xba8d442c, 0x62cc3229, 0xccd31b4b, 0x2f4a12f4, 0x14ec39f0, 0xcc200776, 0xf02a515e},
    {0xef6f6b1f, 0xde8782fc, 0x6106cc3e, 0xbf5ab87e, 0xfeab9d49, 0x15fc1cc6, 0x96f6db1c},
    {0xb60c611e, 0x354f5758, 0x395f5b2f, 0x20c7da11, 0xfdf2d8a2, 0x4dae453d, 0x8da04a15},
    {0x177f26b, 0xd82a17f2, 0x907c82b, 0x8674d4d8, 0x6244cb0b, 0xaf70f3e9, 0x8b61c431},
    {0xe12fc125, 0xda1bb751, 0x4a87b353, 0x94691667, 0xaa03732e, 0x3cadd045, 0x4b71c088},
    {0xd2ebe594, 0xf67691af, 0xbed93fad, 0x6ae71a36, 0x6ff7d422, 0x17f3acce, 0x753f45ff},
    {0xf85ab36f, 0x513ccd0c, 0xf7d33f1d, 0x697927a3, 0x6aef65a9, 0x1e76b1a7, 0xb37c5fc3}
};
constexpr uint32_t zob_head[256] = {
    0xd3812908,
    0xd3bdddb2,
    0x66fd6dd2,
    0x6cc67b69,
    0x86687944,
    0xb9bea51d,
    0x6ca8f556,
    0x2732507,
    0x984fa854,
    0x6e8f7fde,
    0x8f01efd4,
    0x895cfeec,
    0x18147162,
    0x85b3dc8f,
    0xce9dc6bb,
    0x71a7fa12,
    0x66b25e1,
    0xdabdde5c,
    0x97c8304a,
    0x1dd49c83,
    0xdb870f3d,
    0x5600059f,
    0x5eece2ac,
    0xafd220d7,
    0xee4ff4e9,
    0x5ebd8966,
    0xb8a7bab,
    0x1811b4d9,
    0x87c84539,
    0xc8c7197a,
    0x7f05bcf0,
    0x1adf9ec4,
    0x7b20e36c,
    0xdf988f60,
    0x47f017d7,
    0x8968b7dd,
    0xf56c1ef6,
    0x8a6b68ea,
    0x5f1cba4a,
    0x93a257eb,
    0x6d28dfc7,
    0x3693f21e,
    0x9a5163bd,
    0xecd75697,
    0xe9520f05,
    0x4b834241,
    0x40186c05,
    0x34d5720e,
    0xfbaf9238,
    0x2ac5d342,
    0x55498dc4,
    0xed0295ad,
    0x843c0795,
    0x7a1da2c9,
    0xf0075cba,
    0x9c156e13,
    0x67b8ee00,
    0x9bc7dd84,
    0xc18a7fdf,
    0xa78e19e6,
    0x8c9e06b,
    0xb8b6d2ef,
    0x172c6f66,
    0xf484fac,
    0xba644fcd,
    0x2c8f13cd,
    0x62a6f45d,
    0x7709551e,
    0xdc0a67a8,
    0xdfb8de8c,
    0xe9e1190e,
    0xa7ff4148,
    0x9d690d63,
    0x517ba928,
    0x1c5f9fc8,
    0xa5a98a34,
    0xae5dc831,
    0x89b464f3,
    0x5d9bbfd1,
    0x8f569878,
    0xcd224177,
    0x5a5e7aaf,
    0xdb55106a,
    0x27bcbaf3,
    0x4f1000b9,
    0x42f859e5,
    0x281f7da1,
    0xd453168c,
    0x624e6897,
    0x1ad45a2f,
    0xbccb75f6,
    0xad3c3497,
    0x6e830d55,
    0x90c6f48d,
    0x104b0c3f,
    0x46ca4cfe,
    0x5ecb448c,
    0x61f58bc7,
    0x96e7880a,
    0xbf2e1bab,
    0xc96229ea,
    0xd0044aa2,
    0x3843f623,
    0xf39f34df,
    0xe3d79538,
    0x480bc21d,
    0x3af36861,
    0x163d2ee0,
    0x152cd2b3,
    0x70a77c13,
    0x8c312a58,
    0xdf11642d,
    0xa4eb02b7,
    0x2a51a747,
    0x6c9dd294,
    0x5571abd0,
    0x211c39d7,
    0xf20fb57d,
    0xd411a141,
    0x5850849e,
    0xee784293,
    0x528dba1b,
    0xb6976163,
    0x27dd323,
    0x872383ae,
    0x21896012,
    0x3e67ea30,
    0x4585ad20,
    0x66e2aa0a,
    0x95b15c8c,
    0x23699dab,
    0x6f535bb7,
    0xb50f0108,
    0xfc67611f,
    0x5077378f,
    0x2e6fecc6,
    0x9f6bbbc1,
    0x9f371d36,
    0x5116fd14,
    0x8e280616,
    0x1f813431,
    0x8a18d0d6,
    0x128f13f0,
    0x6ca6b95e,
    0xc51886a2,
    0x2436c015,
    0x4f7a85df,
    0xd95556cc,
    0xca1bc5fb,
    0x21248593,
    0x21fec5b4,
    0x7bbb02c1,
    0x3722cf39,
    0x58656d69,
    0xfe857fce,
    0x7ad09ecd,
    0x51268eba,
    0xfdb59f54,
    0xded7d592,
    0xa3f2ecad,
    0x6b7c46d3,
    0x60dcd93b,
    0x44dd2563,
    0x4e18b5bc,
    0x1f1f980b,
    0xb8ad6c24,
    0x3ec1201c,
    0xa64a8f6b,
    0x938a91ff,
    0xcefdc63f,
    0xa4e52eb1,
    0xaa9f20a2,
    0xace317fb,
    0x8339d527,
    0xbcef3d56,
    0x1e84b6d1,
    0x2ad6897,
    0x864fe496,
    0x25136d37,
    0x6a885790,
    0x5ce30c9a,
    0x3165b0ad,
    0x62582c96,
    0x7df8944,
    0xfce3b5b,
    0x23faebd2,
    0x5890fcfe,
    0x50de4010,
    0x11f4f78a,
    0x8d492fdd,
    0xa98f8604,
    0xc9e66b6c,
    0xdc95900,
    0x31701d2a,
    0x28556dad,
    0x7b87b199,
    0x19788b06,
    0xbafc6a05,
    0x30594737,
    0x18f25fd7,
    0xed7a55cd,
    0x9e54969,
    0x61bcc839,
    0xfdc37ab7,
    0xdb1159a9,
    0x85b7e30b,
    0x87cbbddb,
    0xab8fe3f9,
    0x580352df,
    0x17489b91,
    0x85286d60,
    0xb1d4baad,
    0xd0fb3bb8,
    0x352871b5,
    0xf9636dfb,
    0x17a13c6f,
    0xf4b2aa52,
    0xac30fbb8,
    0xe3a47a4b,
    0xe9fc566e,
    0x91c982c0,
    0x6960961e,
    0xa776b17c,
    0xa5f5a5c6,
    0x356e4c72,
    0x453738db,
    0xbeb9ce1e,
    0x56557228,
    0x37fa7cc4,
    0x5472d731,
    0x2411a585,
    0x9e86450d,
    0xe2648042,
    0xa1d8f129,
    0x5ed317fe,
    0xb8d4edb8,
    0x6f55357f,
    0xa78013f8,
    0x2c1ba341,
    0x81e19311,
    0x98efa8c8,
    0x1923debc,
    0x891ac808,
    0xc986ae17,
    0xe97f5d02,
    0x350d7b41,
    0x159c1242,
    0xfc4b6282,
    0xb49c5881,
    0xa921fd46,
    0x7d8fa389,
    0x8aeb5b04,
    0x46ca4d37,
    0xa12fa8e2,
    0xb758905f,
    0x664143db
};


template<size_t N>
constexpr array<uint8_t, N*N> make_can_move_mask_array() {
    array<uint8_t,N*N> can_move_mask{};
    rep(i,N)rep(j,N){
        int pos = i*N + j;
        uint8_t mask = 0;
        rep(dir,4){
            int nx = i + DX[dir], ny = j + DY[dir];
            if(nx >= 0 && nx < N && ny >= 0 && ny < N){
                mask |= (1 << dir);
            }
        }
        can_move_mask[pos] = mask;
    }
    return can_move_mask;
}

template<size_t N, size_t MMAX>
class State{
private:
    static constexpr size_t NN = N * N;
    uint32_t _hash;

    static constexpr array<uint8_t, NN> can_move_mask_ = make_can_move_mask_array<N>();

    static constexpr int DIR[4] = {int(-N), int(N), int(-1), int(1)};
    static uint8_t path_tmp[MMAX];

    static void init_(){
    }

    static int calc_direction_id(int from, int to){
        // (-N, +N, -1, +1) -> (0, 1, 2, 3)
        return (to > from) + ((unsigned)(to + 1 - from) <= 2) * 2;
    }
    static int calc_elapsed(int fturn, int f){
        return (fturn >= f) ? (fturn - f) : (fturn + 200 - f);
    }

public:

    uint8_t field[NN];
    uint8_t path[MMAX];
    uint8_t c[MMAX];
    int x;
    int turn;
    int fturn; // field, elapsed 用のターンカウンタ
    int last_dir;
    int head;
    int c_size;
    int n_head_correct;
    int bite_interval; // 直近の噛みちぎりからの経過ターン数

    static void init(){
        init_();
    }

    State() = default;
    State(const vector<int>& d_, const vector<int>& f_):turn(0){
        _hash = 0;
        rep(x,NN){
            field[x] = f_[x];
            _hash ^= zob_food[x][field[x]];
        }
        c_size = 0;
        fturn = 8;
        rep(x,NN) if(field[x] == 0) field[x] = 255;
        rep(i,5){
            c[c_size++] = 1;
            path[i] = i*N;
            ++fturn;
            if(i > 0) field[i*N] = fturn;
        }
        x = 4*N;
        head = 4;
        last_dir = 1; // D
        turn = 5;
        n_head_correct = 5;
        bite_interval = 0;
    }

    int can_move_mask() const{
        return can_move_mask_[x] & ~(1 << (last_dir ^ 1));
    }

    int turn_after_step(int dir) const{
        int nx = x + DIR[dir];
        int f = field[nx];
        if(f < 8 || f == 255) return turn + 1;
        int elapsed = calc_elapsed(fturn, f) + 1;
        return turn + 1 + max(0, n_head_correct - elapsed - 1);
    }

    bool can_step(int dir) const{
        if((dir ^ last_dir) == 1) return false;
        if((can_move_mask_[x] & (1 << dir)) == 0) return false;
        else if (bite_interval == 0){
            // 短いスパンでの噛みちぎり禁止
            int nx = x + DIR[dir];
            int f = field[nx];
            if(f >= 8 && f < 255){
                int elapsed = calc_elapsed(fturn + 1, f);
                if(elapsed < c_size - 1) return false; // 1手で噛みちぎりが発生する場合は不可
            }
        }
        return true;
    }

    template<bool eat_tail, bool path_out=false>
    void step(int dir, vector<char>* ans = nullptr){

        if(path_out) assert(ans != nullptr);

        ++turn;
        ++bite_interval;
        fturn = (fturn == 207) ? 8 : fturn + 1; // fturn = turn % 200 + 8

        // 移動
        x += DIR[dir];
        head = (head == MMAX - 1) ? 0 : head + 1;
        path[head] = x;
        last_dir = dir;
        if(path_out) ans->push_back(UDLR[dir]);

        // 食事
        int f = field[x];
        field[x] = fturn;
        if(f < 8){
            if(n_head_correct == c_size && f == D[c_size]) ++n_head_correct;
            c[c_size++] = f;
            _hash ^= zob_food[x][f];
            return;
        }
        int b_head = head - c_size + 1; if(b_head < 0) b_head += MMAX;
        int bx = path[b_head];
        if(x == bx) return;
        field[bx] = 255;

        if(f == 255) return;

        // 噛みちぎり

        bite_interval = 0;
        int elapsed = calc_elapsed(fturn, f) + 1;
        int from = head - c_size + 1;
        
        if (elapsed < n_head_correct && eat_tail) {

            // eat tailが発生

            for(int c_idx = elapsed; c_idx < c_size; ++c_idx){
                int idx = head >= c_idx ? head - c_idx : head - c_idx + MMAX;
                path_tmp[c_idx] = path[idx];
            }

            // eat_tailループ
            for(int c_idx = elapsed; c_idx < n_head_correct; ++c_idx){
                head = (head == MMAX - 1) ? 0 : head + 1;
                path[head] = path_tmp[c_idx];
                ++turn;
                fturn = (fturn == 207) ? 8 : fturn + 1;
                field[path[head]] = fturn;
                if(path_out) {
                    auto cur = path[head];
                    auto prev = path[(head == 0) ? MMAX - 1 : head - 1];
                    ans->push_back(UDLR[calc_direction_id(prev, cur)]);
                }
            }
            
            // tailループ
            for(int c_idx = n_head_correct; c_idx < c_size; ++c_idx){
                int pos = path_tmp[c_idx];
                field[pos] = c[c_idx];
                _hash ^= zob_food[pos][c[c_idx]];
            }

            c_size = n_head_correct;
            x = path[head];
            last_dir = calc_direction_id(path[(head == 0) ? MMAX - 1 : head - 1], path[head]);
        } else{
            for(int c_idx = elapsed; c_idx < c_size; ++c_idx){
                int idx = head >= c_idx ? head - c_idx : head - c_idx + MMAX;
                int pos = path[idx];
                field[pos] = c[c_idx];
                _hash ^= zob_food[pos][c[c_idx]];
            }
            c_size = elapsed;
            n_head_correct = min(n_head_correct, c_size);
        }

    }

    int depth1_score() const{
        if(n_head_correct == M || n_head_correct < c_size) return n_head_correct;
        int legal = can_move_mask();
        int d = D[c_size];
        for(int dir=0;dir<4;++dir)if((legal >> dir) & 1){
            int food = field[x + DIR[dir]];
            if(food == d) return n_head_correct + 1;
        }
        return n_head_correct;
    }

    int depth2_score() const{
        if(n_head_correct == M) return n_head_correct;
        bool bite = false;
        int nfturn = (fturn == 207) ? 8 : fturn + 1;
        int best = n_head_correct;
        bool correct = n_head_correct == c_size;
        int dh = D[n_head_correct];
        int legal = can_move_mask();
        for (int dir=0;dir<4;++dir)if((legal >> dir) & 1){
            int nx = x + DIR[dir];
            int f = field[nx];
            if (f >= 8 && f < 255) {
                int elapsed = calc_elapsed(nfturn, f);
                if(elapsed < n_head_correct) bite = true; // 噛みちぎり可能
                continue;
            }

            // 1手目で噛みちぎりが発生しない場合
            if(!correct) continue;
            if (f == dh) {
                // 1手目で正しい食べ物を食べられる場合
                best = n_head_correct + 1;
                if(best == M) return best;  
                int nlegal = can_move_mask_[nx] & ~(1 << (dir ^ 1));
                for(int ndir = 0; ndir < 4; ++ndir)if((nlegal >> ndir) & 1){
                    int nnx = nx + DIR[ndir];
                    int nf = field[nnx];
                    if(nf == D[n_head_correct + 1]) {
                        return best + 1; // 2手目でさらに正しい食べ物を食べられる場合
                    }
                }
            } else if(f == 255 && best == n_head_correct){
                // 1手目で空マスに移動する場合
                int nlegal = can_move_mask_[nx] & ~(1 << (dir ^ 1));
                for(int ndir = 0; ndir < 4; ++ndir)if((nlegal >> ndir) & 1){
                    int nnx = nx + DIR[ndir];
                    int nf = field[nnx];
                    if(nf == dh) {
                        best = n_head_correct + 1; // 2手目で正しい食べ物を食べられる場合
                    }
                }
            } else continue; // 1手目で誤った食べ物を食べる場合
        }

        if(!bite || (best > n_head_correct)) return best;

        // 1手目で噛みちぎりが発生する場合
        int tail = head - n_head_correct + 2; if(tail < 0) tail += MMAX;
        int tx = path[tail];
        legal = can_move_mask_[tx];
        for(int dir=0;dir<4;++dir)if((legal >> dir) & 1){
            int nx = tx + DIR[dir];
            int f = field[nx];
            if(f >= 8 && f < 255){
                // 噛みちぎられた結果食べ物になっている場合
                int food_elapsed = calc_elapsed(nfturn, f);
                if(food_elapsed < c_size && food_elapsed >= n_head_correct){
                    f = c[food_elapsed];
                }
            }
            if(f == dh) {
                return n_head_correct + 1; // 噛みちぎり後に正しい食べ物を食べられる場合
            }
        }
        return best;
    }

    int depth3_score() const{
        if(n_head_correct == M) return n_head_correct;

        // 初手で噛みちぎる場合は2手目は噛みちぎり不可という制約をかける

        bool bite1 = false;
        int fturn1 = (fturn == 207) ? 8 : fturn + 1;
        int fturn2 = (fturn1 == 207) ? 8 : fturn1 + 1;
        int head1 = (head == MMAX - 1) ? 0 : head + 1;
        int best = n_head_correct;
        int dh = D[n_head_correct];
        bool correct = n_head_correct == c_size;
        const int upper_limit = n_head_correct + 3;

        int legal = can_move_mask();

        // depth1 search
        for(int dir1=0;dir1<4;++dir1)if((legal >> dir1) & 1){
            int nx1 = x + DIR[dir1];
            int f1 = field[nx1];

            if(f1 >= 8 && f1 < 255){ 
                int elapsed = calc_elapsed(fturn1, f1);
                if(elapsed < n_head_correct) bite1 = true; // 初手で噛みちぎり可能
                continue;
            }

            // 1手後のc_size, n_head_correctの計算
            int cs1 = c_size + (f1<8); 
            int nh1 = n_head_correct + (correct && (f1==dh));
            if(nh1 < best-1) continue;
            if(nh1 == M) return nh1;

            int dh1 = D[nh1];
            bool correct1 = (cs1 == nh1);
            bool bite2 = false;

            best = max(best, nh1);

            int legal2 = can_move_mask_[nx1] & ~(1 << (dir1 ^ 1));

            // depth2 search
            for(int dir2=0;dir2<4;++dir2)if((legal2 >> dir2) & 1){
                int nx2 = nx1 + DIR[dir2];
                int f2 = field[nx2];
                if(f2 >= 8 && f2 < 255){
                    int elapsed = calc_elapsed(fturn2, f2);
                    if(elapsed < nh1) bite2 = true; // 二手目で噛みちぎり可能
                    continue;
                }

                if(!correct1) continue;
                int cs2 = cs1 + (f2<8);
                int nh2 = nh1 + (f2==dh1);
                bool correct2 = (cs2 == nh2);
                if(!correct2 || (nh2 < best)) continue;
                best = nh2;
                if(nh2 == M) return nh2;
                int dh2 = D[nh2];

                // depth3 search
                int legal3 = can_move_mask_[nx2] & ~(1 << (dir2 ^ 1));
                for(int dir3=0;dir3<4;++dir3)if((legal3 >> dir3) & 1){
                    int nx3 = nx2 + DIR[dir3];
                    int nh3 = nh2 + (field[nx3]==dh2);
                    best = max(best, nh3);
                    if(best == upper_limit) return best;
                }
            }

            if(!bite2) continue;

            // 2手目で噛みちぎりが発生した場合
            int tail2 = head1 - nh1 + 2; if(tail2 < 0) tail2 += MMAX;
            int tx = path[tail2];
            int legal_tail2 = can_move_mask_[tx];
            for(int dir=0;dir<4;++dir)if((legal_tail2 >> dir) & 1){
                int nx = tx + DIR[dir];
                if(nx == nx1) continue;
                int f = field[nx];
                if(f >= 8 && f < 255){
                    // 噛みちぎられた結果食べ物になっている場合
                    int food_elapsed = calc_elapsed(fturn2, f);
                    if(food_elapsed < c_size && food_elapsed >= nh1){
                        f = c[food_elapsed];
                    }
                }
                if(f == dh1){
                    best = max(best, nh1 + 1);
                }
            }
        }
        
        if(!bite1 || (best > n_head_correct + 1)) return best;

        // 初手で噛みちぎりが発生する場合
        int tail1 = head - n_head_correct + 2; if(tail1 < 0) tail1 += MMAX;
        int tx = path[tail1];
        int legal_tail1 = can_move_mask_[tx];

        // depth2 search after bite
        for(int dir2=0;dir2<4;++dir2)if((legal_tail1 >> dir2) & 1){
            int nx2 = tx + DIR[dir2];
            int f2 = field[nx2];
            if(f2 >= 8 && f2 < 255){
                int food_elapsed = calc_elapsed(fturn1, f2);
                if(food_elapsed < n_head_correct) continue; // 二手目は噛みちぎり不可
                else if(food_elapsed < c_size) f2 = c[food_elapsed];
            }

            int cs2 = n_head_correct + (f2<8);
            int nh2 = n_head_correct + (f2==dh);
            if(nh2 < best || (nh2 != cs2)) continue;
            if(nh2 == M) return nh2;
            int dh2 = D[nh2];

            int legal2 = can_move_mask_[nx2] & ~(1 << (dir2 ^ 1));

            // depth3 search after bite
            for(int dir3=0;dir3<4;++dir3)if((legal2 >> dir3) & 1){
                int nx3 = nx2 + DIR[dir3];
                int f3 = field[nx3];
                if(f3 >= 8 && f3 < 255){
                    int elapsed = calc_elapsed(fturn1, f3);
                    if(elapsed >= n_head_correct && elapsed < c_size) f3 = c[elapsed];
                }
                int nh3 = nh2 + (f3==dh2);
                best = max(best, nh3);
                if(best == n_head_correct + 2) return best;
            }
        }
        return best;
    }

    int depth4_score() const{
        if(n_head_correct == M) return n_head_correct;

        bool bite1 = false;
        int fturn1 = (fturn == 207) ? 8 : fturn + 1;
        int fturn2 = (fturn1 == 207) ? 8 : fturn1 + 1;
        int fturn3 = (fturn2 == 207) ? 8 : fturn2 + 1;
        int head1 = (head == MMAX - 1) ? 0 : head + 1;
        int head2 = (head1 == MMAX - 1) ? 0 : head1 + 1;
        int best = n_head_correct;

        int cs0 = c_size;
        int nh0 = n_head_correct;
        int dh0 = D[nh0];
        bool correct0 = (nh0 == cs0);

        const int upper_limit = nh0 + 4;

        // depth1 search
        int legal1 = can_move_mask();
        for(int dir1=0;dir1<4;++dir1) if((legal1 >> dir1) & 1){
            int nx1 = x + DIR[dir1];
            int f1 = field[nx1];
            if(f1 >= 8 && f1 < 255){
                int elapsed = calc_elapsed(fturn1, f1);
                if(elapsed < nh0) bite1 = true; // 初手で噛みちぎり可能
                continue;
            }

            int cs1 = cs0 + (f1 < 8);
            int nh1 = nh0 + (correct0 && (f1 == dh0));
            bool correct1 = (cs1 == nh1);
            if(nh1 + correct1 + 2 <= best) continue;
            if(nh1 == M) return nh1;
            best = max(best, nh1);

            int dh1 = D[nh1];
            bool bite2 = false;

            // depth2 search
            int legal2 = can_move_mask_[nx1] & ~(1 << (dir1 ^ 1));
            for(int dir2=0;dir2<4;++dir2) if((legal2 >> dir2) & 1){
                int nx2 = nx1 + DIR[dir2];
                int f2 = field[nx2];
                if(f2 >= 8 && f2 < 255){
                    int elapsed = calc_elapsed(fturn2, f2);
                    if(elapsed < nh1) bite2 = true; // 二手目で噛みちぎり可能
                    continue;
                }

                int cs2 = cs1 + (f2 < 8);
                int nh2 = nh1 + ((f2 == dh1) & correct1);
                bool correct2 = (cs2 == nh2);
                if(nh2 + correct2 + 1 <= best) continue;
                if(nh2 == M) return nh2;
                best = max(best, nh2);

                int dh2 = D[nh2];
                bool bite3 = false;

                // depth3 search
                int legal3 = can_move_mask_[nx2] & ~(1 << (dir2 ^ 1));
                for(int dir3=0;dir3<4;++dir3) if((legal3 >> dir3) & 1){
                    int nx3 = nx2 + DIR[dir3];
                    int f3 = field[nx3];
                    if(f3 >= 8 && f3 < 255){
                        int elapsed = calc_elapsed(fturn3, f3);
                        if(elapsed < nh2) bite3 = true; // 三手目で噛みちぎり可能
                        continue;
                    }

                    int cs3 = cs2 + (f3 < 8);
                    int nh3 = nh2 + ((f3 == dh2) & correct2);
                    bool correct3 = (cs3 == nh3);
                    if(nh3 + correct3 <= best) continue;
                    if(nh3 == M) return nh3;
                    best = max(best, nh3);

                    int dh3 = D[nh3];

                    // depth4 search
                    int legal4 = can_move_mask_[nx3] & ~(1 << (dir3 ^ 1));
                    for(int dir4=0;dir4<4;++dir4) if((legal4 >> dir4) & 1){
                        int nx4 = nx3 + DIR[dir4];
                        int f4 = field[nx4];
                        best = max(best, nh3 + (f4 == dh3));
                        if(best == upper_limit) return best;
                    }
                }

                if(bite3 && (nh2 + 1 > best)){
                    // 3手目で噛みちぎりが発生した場合
                    int tail3 = head2 - nh2 + 2;
                    if(tail3 < 0) tail3 += MMAX;
                    int nx3 = path[tail3];

                    // depth4 search after bite
                    int legal4 = can_move_mask_[nx3];
                    for(int dir4=0;dir4<4;++dir4) if((legal4 >> dir4) & 1){
                        int nx4 = nx3 + DIR[dir4];
                        if(nx4 == nx2 || nx4 == nx1) continue;
                        int f4 = field[nx4];
                        if(f4 >= 8 && f4 < 255){
                            // 噛みちぎられた結果食べ物になっている場合
                            int elapsed = calc_elapsed(fturn3, f4);
                            if(elapsed < cs0 && elapsed >= nh2) f4 = c[elapsed];
                        }
                        if(f4 == dh2) {
                            best = max(best, nh2 + 1);
                        }
                    }
                }
            }

            if(bite2 && (nh1 + 2 > best)){
                // 2手目で噛みちぎりが発生した場合
                int tail2 = head1 - nh1 + 2;
                if(tail2 < 0) tail2 += MMAX;
                int nx2 = path[tail2];

                // depth3 search after bite
                int legal3 = can_move_mask_[nx2];
                for(int dir3=0;dir3<4;++dir3) if((legal3 >> dir3) & 1){
                    int nx3 = nx2 + DIR[dir3];
                    if(nx3 == nx1) continue;
                    int f3 = field[nx3];
                    if(f3 >= 8 && f3 < 255){
                        int elapsed = calc_elapsed(fturn2, f3);
                        if(elapsed < nh1) continue; // 3手目は噛みちぎり不可
                        else if(elapsed < cs0) f3 = c[elapsed];
                    }

                    int cs3 = nh1 + (f3 < 8);
                    int nh3 = nh1 + (f3 == dh1);
                    bool correct3 = (cs3 == nh3);
                    if(nh3 + correct3 <= best) continue;
                    if(nh3 == M) return nh3;
                    best = max(best, nh3);

                    int dh3 = D[nh3];

                    // depth4 search after bite
                    int legal4 = can_move_mask_[nx3] & ~(1 << (dir3 ^ 1));
                    for(int dir4=0;dir4<4;++dir4) if((legal4 >> dir4) & 1){
                        int nx4 = nx3 + DIR[dir4];
                        if(nx4 == nx1) continue;
                        int f4 = field[nx4];
                        if(f4 >= 8 && f4 < 255){
                            // 噛みちぎられた結果食べ物になっている場合
                            int elapsed = calc_elapsed(fturn2, f4);
                            if(elapsed < cs0 && elapsed >= nh1) f4 = c[elapsed];
                        }
                        if(f4 == dh3) {
                            best = max(best, nh3 + 1);
                        }
                    }
                }
            }
        }

        if(bite1 && (nh0 + 3 > best)) {
            // 1手目で噛みちぎりが発生した場合
            int tail1 = head - nh0 + 2;
            if(tail1 < 0) tail1 += MMAX;
            int nx1 = path[tail1];

            // depth2 search after bite
            int legal2 = can_move_mask_[nx1];
            for(int dir2=0;dir2<4;++dir2) if((legal2 >> dir2) & 1){
                int nx2 = nx1 + DIR[dir2];
                int f2 = field[nx2];
                if(f2 >= 8 && f2 < 255){
                    int elapsed = calc_elapsed(fturn1, f2);
                    if(elapsed < nh0) continue; // 二手目は噛みちぎり不可
                    else if(elapsed < cs0) f2 = c[elapsed];
                }

                int cs2 = nh0 + (f2 < 8);
                int nh2 = nh0 + (f2 == dh0);
                bool correct2 = (cs2 == nh2);
                if(nh2 + correct2 + 1 <= best) continue;
                if(nh2 == M) return nh2;
                best = max(best, nh2);

                int dh2 = D[nh2];

                // depth3 search after bite
                int legal3 = can_move_mask_[nx2] & ~(1 << (dir2 ^ 1));
                for(int dir3=0;dir3<4;++dir3) if((legal3 >> dir3) & 1){
                    int nx3 = nx2 + DIR[dir3];
                    int f3 = field[nx3];
                    if(f3 >= 8 && f3 < 255){
                        int elapsed = calc_elapsed(fturn1, f3);
                        if(elapsed < nh0) continue; // 三手目は噛みちぎり不可
                        else if(elapsed < cs0) f3 = c[elapsed];
                    }

                    int cs3 = cs2 + (f3 < 8);
                    int nh3 = nh2 + ((f3 == dh2) & correct2);
                    bool correct3 = (cs3 == nh3);
                    if(nh3 + correct3 <= best) continue;
                    if(nh3 == M) return nh3;
                    best = max(best, nh3);

                    int dh3 = D[nh3];

                    // depth4 search after bite
                    int legal4 = can_move_mask_[nx3] & ~(1 << (dir3 ^ 1));
                    for(int dir4=0;dir4<4;++dir4) if((legal4 >> dir4) & 1){
                        int nx4 = nx3 + DIR[dir4];
                        int f4 = field[nx4];
                        if(f4 >= 8 && f4 < 255){
                            int elapsed = calc_elapsed(fturn1, f4);
                            if(elapsed < cs0 && elapsed >= nh0) f4 = c[elapsed];
                        }
                        if(f4 == dh3) {
                            best = max(best, nh3 + 1);
                        }
                    }
                }
            }
        }

        return best;
    }

    void debug_print() const{
        char vis[N][N];
        rep(i,N)rep(j,N) vis[i][j] = '.';
        rep(i,N)rep(j,N){
            int f = field[i*N+j];
            if(f < 8) vis[i][j] = 'a' + f - 1;
        }
        int tx = x / N, ty = x % N;
        rep(i,1,c_size){
            int pos = path[(head - i + MMAX) % MMAX];
            int tx = pos / N, ty = pos % N;
            vis[tx][ty] = 'A' + c[i] - 1;
        }
        vis[x / N][x % N] = '#';
        rep(i,N){
            rep(j,N) cerr << vis[i][j] << " ";
            cerr << "\n";
        }
        cerr << "turn: " << turn << "\n";
        cerr << "c size: " << c_size << "\n";
        cerr << "n_head_correct: " << n_head_correct << "\n";
        cerr << "last dir: " << UDLR[last_dir] << "\n";
        cerr << "x = " << x / N << " y = " << x % N << "\n";
    }

    int calc_score() const{
        int error = 2 * (M - c_size);
        for(int i = 0; i < c_size; ++i){
            error += (c[i] != D[i]);
        }
        return 10000 * error + (turn - 5);
    }

    uint64_t hash() const{
        // foods, head位置のみに依存するようにハッシュを作る。
        return _hash ^ zob_head[x];
    };

    bool done() const{
        return n_head_correct == M;
    }

};

template<size_t N, size_t MMAX> uint8_t State<N, MMAX>::path_tmp[MMAX];

template<size_t N, size_t MMAX>
vector<char> get_ans_from_path(const State<N, MMAX>& initial_state, const vector<int>& path, bool eat_tail = false){
    vector<char> ans;
    State<N, MMAX> state = initial_state;
    for(int dir : path){
        state.template step<true,true>(dir, &ans);
    }
    cerr << "Time = " << timer.elapse() << "\n";
    cerr << "Score = " << state.calc_score() << "\n";
    return ans;
}

#include <concepts>
#include <type_traits>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>

#define TOP_K_HASH_HEAP_DEBUG false

#if TOP_K_HASH_HEAP_DEBUG
#define TOP_K_HASH_HEAP_ASSERT(x) assert(x)
#else
#define TOP_K_HASH_HEAP_ASSERT(x) ((void)0)
#endif

template <class T>
concept BeamNode =
    requires(const T& x) {
        { x.score > x.score } -> std::convertible_to<bool>;
        requires std::unsigned_integral<
            std::remove_cvref_t<decltype(x.hash)>
        >;
    };

template <BeamNode T>
class TopKHashHeap {

private:

    using hash_type = std::remove_cvref_t<decltype(std::declval<const T&>().hash)>;
    using size_type = std::size_t;
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    class HashToIndexMap {

        // insert_new, change, erase など、keyの存在確認の責務を呼び出し元に任せる
        // fullにならないことを前提とする
    
    private:

        static constexpr uint8_t EMPTY = 0;
        static constexpr uint8_t USED = 1;

        int mask_ = 0;
        int table_size_ = 0;

        std::vector<hash_type> keys_;
        std::vector<int> vals_;
        std::vector<uint8_t> states_;
        
        static int ceil_pow2(int x) {
            int p = 1;
            while (p < x) p <<= 1;
            return p;
        }

        inline int start_index(hash_type key) const {
            return static_cast<int>(key) & mask_;
        }

    public:

        HashToIndexMap() = default;

        explicit HashToIndexMap(int max_items) {
            init(max_items);
        }

        void init(int max_items) {

            TOP_K_HASH_HEAP_ASSERT(max_items > 0);

            int want = max_items * 2;
            int sz = ceil_pow2(want);

            table_size_ = sz;
            mask_ = sz - 1;

            keys_.resize(table_size_);
            vals_.resize(table_size_);
            states_.assign(table_size_, EMPTY);
        }

        void clear() {
            std::fill(states_.begin(), states_.end(), EMPTY);
        }

        void insert_new(hash_type key, int val) {
            // key　が存在しないことを前提とする

            TOP_K_HASH_HEAP_ASSERT(find(key) == -1);

            int idx = start_index(key);
            while (states_[idx] == USED) {
                idx = (idx + 1) & mask_;
            }
            states_[idx] = USED;
            keys_[idx] = key;
            vals_[idx] = val;
        }

        void change(hash_type key, int new_val) {
            // key が存在することを前提とする

            TOP_K_HASH_HEAP_ASSERT(find(key) != -1);

            int idx = start_index(key);
            while(keys_[idx] != key) {
                idx = (idx + 1) & mask_;
            }
            vals_[idx] = new_val;
        }

        void erase(hash_type key) {
            // key が存在することを前提とする

            TOP_K_HASH_HEAP_ASSERT(find(key) != -1);

            int idx = start_index(key);
            while(keys_[idx] != key) {
                idx = (idx + 1) & mask_;
            }
            
            // backshift deletion
            int hole = idx;
            int cur = (idx + 1) & mask_;
            while (states_[cur] == USED) {
                int home = start_index(keys_[cur]);

                // home から cur へ来る探索経路が hole をまたぐなら前詰めする
                if (((cur - home) & mask_) >= ((hole - home) & mask_)) {
                        keys_[hole] = keys_[cur];
                        vals_[hole] = vals_[cur];
                        hole = cur;
                    }
                    cur = (cur + 1) & mask_;
            }
            states_[hole] = EMPTY;
        }

        int find(hash_type key) const {

            TOP_K_HASH_HEAP_ASSERT(keys_.size() > 0);

            int idx = start_index(key);
            while (states_[idx] == USED) {
                if (keys_[idx] == key) {
                    return vals_[idx];
                }
                idx = (idx + 1) & mask_;
            }
            return -1;
        }

    };

    class sorted_view {
    private:
        const TopKHashHeap* parent_;
        std::vector<int> order_;

    public:
        sorted_view(const TopKHashHeap* p) : parent_(p), order_(p->heap_) {
            std::sort(order_.begin(), order_.end(),
                [this](int a, int b) {
                    return parent_->data_[a].score > parent_->data_[b].score;
                });
        }

        class iterator {
        private:
            const TopKHashHeap* parent_;
            const std::vector<int>* order_;
            size_type pos_;

        public:
            iterator(const TopKHashHeap* p, const std::vector<int>* o, size_type pos)
                : parent_(p), order_(o), pos_(pos) {}

            const T& operator*() const {
                return parent_->data_[(*order_)[pos_]];
            }

            iterator& operator++() {
                ++pos_;
                return *this;
            }

            bool operator!=(const iterator& other) const {
                return pos_ != other.pos_;
            }
        };

        iterator begin() const {
            return iterator(parent_, &order_, 0);
        }

        iterator end() const {
            return iterator(parent_, &order_, order_.size());
        }
    };

    class heap_reverse_view {
    private:
        const TopKHashHeap* parent_;

    public:
        explicit heap_reverse_view(const TopKHashHeap* p) : parent_(p) {}

        class iterator {
        private:
            const TopKHashHeap* parent_;
            size_type pos_;

        public:
            iterator(const TopKHashHeap* p, size_type pos)
                : parent_(p), pos_(pos) {}

            const T& operator*() const {
                return parent_->data_[parent_->heap_[parent_->heap_.size() - 1 - pos_]];
            }

            iterator& operator++() {
                ++pos_;
                return *this;
            }

            bool operator!=(const iterator& other) const {
                return pos_ != other.pos_;
            }
        };

        iterator begin() const {
            return iterator(parent_, 0);
        }

        iterator end() const {
            return iterator(parent_, parent_->heap_.size());
        }
    };

    std::vector<T> data_;

    // heap[pos] = data index
    std::vector<int> heap_;

    // pos_in_heap[data index] = heap pos
    std::vector<int> pos_in_heap_;

    // std::unordered_map<u64, int> hash_to_index_;
    HashToIndexMap hash_to_index_;

    size_type capacity_;

    bool heap_less(int lhs_idx, int rhs_idx) const {
        return data_[heap_[lhs_idx]].score < data_[heap_[rhs_idx]].score;
    }

    void swap_heap_nodes(size_type a, size_type b) {
        std::swap(heap_[a], heap_[b]);
        pos_in_heap_[heap_[a]] = a;
        pos_in_heap_[heap_[b]] = b;
    }

    void sift_down(size_type pos) {
        const size_type n = heap_.size();
        const int x = heap_[pos];
        const auto x_score = data_[x].score;

        while (true) {
            const size_type left = pos * 2 + 1;
            if (left >= n) break;

            const size_type right = left + 1;
            size_type child = left;

            if (right < n && data_[heap_[right]].score < data_[heap_[left]].score) {
                child = right;
            }

            if (!(data_[heap_[child]].score < x_score)) break;

            heap_[pos] = heap_[child];
            pos_in_heap_[heap_[pos]] = pos;
            pos = child;
        }
        heap_[pos] = x;
        pos_in_heap_[x] = pos;
    }

    void build_heap() {
        const size_type n = data_.size();

        for (size_type i = n / 2; i > 0; --i) {
            sift_down(i - 1);
        }
    }

    template <class U>
    void insert_impl(U&& node) {

        if (data_.size() == capacity_) {
            // 満杯時

            const size_type worst_idx = heap_[0];

            // 新ノードが worst より悪ければ棄却
            if (node.score < data_[worst_idx].score) return;

            const int idx = hash_to_index_.find(node.hash);

            // 同一 hash が既にある
            if (idx != -1) {
                // 既存同一 hash より良くなければ棄却
                if (node.score < data_[idx].score) {
                    return;
                }

                data_[idx] = node;
                sift_down(pos_in_heap_[idx]);
                return;
            }

            // 新規 hash なので worst を置換
            hash_to_index_.erase(data_[worst_idx].hash);
            hash_to_index_.insert_new(node.hash, worst_idx);
            data_[worst_idx] = std::forward<U>(node);
            sift_down(0);
        } else{

            // まだ満杯でない場合は hash を作って重複判定が必要

            const int idx = hash_to_index_.find(node.hash);

            // 同一 hash が既にある
            if (idx != -1) {
                if (data_[idx].score < node.score) {
                    data_[idx] = std::forward<U>(node);
                }
                return;
            }

            // まだ満杯でなければ追加
            const size_type new_idx = data_.size();
            hash_to_index_.insert_new(node.hash, new_idx);
            data_.push_back(std::forward<U>(node));
            heap_.push_back(new_idx);
            pos_in_heap_.push_back(new_idx);
            if (data_.size() == capacity_) {
                build_heap();
            }
        }
    }

    void shrink_to_capacity() {

        if (data_.size() < capacity_) return;

        std::nth_element(data_.begin(), data_.begin() + capacity_, data_.end(),
            [](const T& a, const T& b) {
                return a.score > b.score;
            }
        );
        data_.resize(capacity_);
        hash_to_index_.init(capacity_);
        for (size_type i = 0; i < capacity_; ++i) {
            hash_to_index_.insert_new(data_[i].hash, static_cast<int>(i));  
        }
        build_heap();
    }

public:

    TopKHashHeap() : capacity_(0) {};

    explicit TopKHashHeap(size_type capacity) : capacity_(capacity) {
        data_.reserve(capacity_);
        heap_.reserve(capacity_);
        pos_in_heap_.reserve(capacity_);
        hash_to_index_.init(capacity_);
    }

    void set_capacity(size_type new_capacity) {

        if(new_capacity == capacity_) return;
        capacity_ = new_capacity;
        shrink_to_capacity();
    }

    void insert(const T& value) {
        insert_impl(value);
    }

    void insert(T&& value) {
        insert_impl(std::move(value));
    }

    void clear() {
        data_.clear();
        heap_.clear();
        pos_in_heap_.clear();
        hash_to_index_.clear();
    }

    size_type size() const noexcept { return data_.size(); }
    size_type capacity() const noexcept { return capacity_; }

    iterator begin() noexcept { return data_.begin(); }
    iterator end() noexcept { return data_.end(); }
    const_iterator begin() const noexcept { return data_.begin(); }
    const_iterator end() const noexcept { return data_.end(); }
    const_iterator cbegin() const noexcept { return data_.cbegin(); }
    const_iterator cend() const noexcept { return data_.cend(); }

    sorted_view sorted_itr() const {
        return sorted_view(this);
    }
    heap_reverse_view heap_reverse_itr() const {
        return heap_reverse_view(this);
    }
};
#include <vector>
#include <algorithm>

struct PathTreeNode{
    int parent;
    int val;
};

struct PathTree{
    std::vector<PathTreeNode> nodes;
    PathTree() = default;

    void add(int parent, int val){
        nodes.push_back({parent,val});
    }
    void clear(){
        nodes.clear();
    }
    void reserve(size_t n){
        nodes.reserve(n);
    }
    std::vector<int> get_path(int idx){
        std::vector<int> path;
        while(idx != -1){
            path.push_back(nodes[idx].val);
            idx = nodes[idx].parent;
        }
        std::reverse(path.begin(),path.end());
        return path;
    }

    size_t size() const{
        return nodes.size();
    }
};

template<size_t N, size_t MMAX>
struct BeamSearchNode{
    State<N, MMAX> state;
    int parent;
    int action;
    uint32_t score;
    uint32_t hash;
};

template<size_t N, size_t MMAX>
using BeamHeap = TopKHashHeap<BeamSearchNode<N, MMAX>>;

template<size_t N, size_t MMAX>
pair<int, vector<char>> beam_search(const State<N, MMAX>& initial_state, int beam_width, int depth){
    PathTree tree;
    tree.reserve(beam_width * MMAX * 10);

    // 1回の行動で最大mターン進む可能性があるため、ビームをm個以上用意する

    const int LOOP_SIZE = M + 8;

    int best_cost = 1<<30;
    int best_tree_idx = -1;

    vector<BeamHeap<N, MMAX>> beam(LOOP_SIZE, BeamHeap<N, MMAX>(beam_width));
    beam[initial_state.turn % LOOP_SIZE].insert({initial_state, -1, -1, 0, 0});
    BeamSearchNode<N, MMAX> next_node;
    bool first = true;
    for(int t = initial_state.turn; t < depth; ++t){ 
        if(t >= best_cost) break;
        int tidx = t % LOOP_SIZE;
        if(timer.elapse() > 1.9){
            beam[tidx].set_capacity(beam_width / 3);
        }
        for(const auto& node : beam[tidx]){
            int tree_idx = tree.size();
            if(first){
                tree_idx = -1;
                first = false;
            }else{
                tree.add(node.parent, node.action);
            }
            for(int dir=0;dir<4;++dir)if(node.state.can_step(dir)){
                uint32_t tie_breaker = rng() >> 48;
                next_node.state = node.state;
                next_node.parent = tree_idx;
                next_node.action = dir;
                next_node.state.template step<true>(dir);
                if(next_node.state.done()){
                    tree.add(tree_idx, dir);
                    vector<int> path = tree.get_path(tree.size() - 1);
                    return {next_node.state.calc_score(), get_ans_from_path(initial_state, path, true)};
                }
                if(next_node.state.turn >= depth) continue;
                uint32_t score = next_node.state.depth3_score();
                next_node.score = (score << 16) | tie_breaker;
                next_node.hash = next_node.state.hash();
                beam[next_node.state.turn % LOOP_SIZE].insert(next_node);
            }
            if(timer.elapse() > 1.95) break;
        }
            if(timer.elapse() > 1.95) break;
        beam[tidx].clear();
    }

    // depth以内にdoneできなかった場合は、最後に追加されたノードを出力
    vector<int> path = tree.get_path(tree.size() - 1);
    return {best_cost, get_ans_from_path(initial_state, path, true)};
};

#define USE_DETAILED_MMAX true

template<size_t N, size_t MMAX>
void solve(const Input& input){
    State<N, MMAX>::init();
    State<N, MMAX> state(input.d, input.f);

    int beam_size = 12000000 / (pow(input.m, 1.2) * input.n * sqrt(input.c));
    cerr << "N = " << input.n << " M = " << input.m << " C = " << input.c << " beam size = " << beam_size << endl;
    int best_score = 1<<30;
    vector<char> best_path;

    int depth = 3000;
    double start_time = timer.elapse();
    auto [score, path] = beam_search(state, beam_size, depth);
    double end_time = timer.elapse();
    double remaining_time = 1.85 - end_time;
    best_score = score;
    best_path = path;

    if(remaining_time < 0.1) {
        for(char c : best_path) cout << c << "\n";
        return;
    }

    int second_beam_size = beam_size * remaining_time / (end_time - start_time);
    cerr << "second beam size = " << second_beam_size << endl;
    auto [score2, path2] = beam_search(state, second_beam_size, min(3000, best_score + 6));
    if(score2 < best_score){
        best_score = score2;
        best_path = path2;
    }

    for(char c : best_path) cout << c << "\n";
    
    cerr << timer.elapse() << endl;
    cerr << best_score << endl;
}

void solve8(const Input& input){
    // 16 - 48
    #if USE_DETAILED_MMAX
    if(input.m <= 32) solve<8,32>(input);
    else solve<8,48>(input);
    #else
    solve<8,48>(input);
    #endif
}
void solve9(const Input& input){
    // 21 - 60
    #if USE_DETAILED_MMAX
    if(input.m <= 40) solve<9,40>(input);
    else solve<9,64>(input);
    #else
    solve<9,64>(input);
    #endif
}
void solve10(const Input& input){
    // 25 - 75
    #if USE_DETAILED_MMAX
    if(input.m <= 48) solve<10,48>(input);
    else solve<10,80>(input);
    #else
    solve<10,80>(input);
    #endif
}
void solve11(const Input& input){
    // 31 - 90
    #if USE_DETAILED_MMAX
    if(input.m <= 64) solve<11,64>(input);
    else solve<11,96>(input);
    #else
    solve<11,96>(input);
    #endif
}
void solve12(const Input& input){
    // 36 - 108
    #if USE_DETAILED_MMAX
    if(input.m <= 72) solve<12,72>(input);
    else solve<12,112>(input);
    #else
    solve<12,112>(input);
    #endif
}
void solve13(const Input& input){
    // 43 - 126
    #if USE_DETAILED_MMAX
    if(input.m <= 80) solve<13,80>(input);
    else solve<13,128>(input);
    #else
    solve<13,128>(input);
    #endif
}

void solve14(const Input& input){
    // 49 - 147
    #if USE_DETAILED_MMAX
    if(input.m <= 96) solve<14,96>(input);
    else solve<14,152>(input);
    #else
    solve<14,152>(input);
    #endif
}

void solve15(const Input& input){
    // 57 - 168
    #if USE_DETAILED_MMAX
    if(input.m <= 112) solve<15,112>(input);
    else solve<15,168>(input);
    #else
    solve<15,168>(input);
    #endif
}

void solve16(const Input& input){
    // 64 - 192
    #if USE_DETAILED_MMAX
    if(input.m <= 128) solve<16,128>(input);
    else solve<16,192>(input);
    #else
    solve<16,192>(input);
    #endif
}

int main(int argc, char* argv[]){
    
    if(argc > 1) {
        timer.timer_scale = stod(argv[1]);
    }
    
    library::fast_io();
    Input input;
    input.get();

    switch(input.n){
        case 8: solve8(input); break;
        case 9: solve9(input); break;
        case 10: solve10(input); break;
        case 11: solve11(input); break;
        case 12: solve12(input); break;
        case 13: solve13(input); break;
        case 14: solve14(input); break;
        case 15: solve15(input); break;
        case 16: solve16(input); break;
    }
}

