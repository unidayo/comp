//#define ONLINE_JUDGE
#ifdef ONLINE_JUDGE
#define NDEBUG
#define SILENT
#define AVX512
#define SAFETY_STOP_TIME 1.9
//#define BENCH_DETAIL
#else
//#define SILENT
#define BENCH
//#define AVX512
//#define LOG
//#define MAKE_TEST
//#define CLASSIFY
//#define SAFETY_STOP_TIME 4.00

#ifdef CLASSIFY
#define SILENT
#endif

#ifndef SILENT
#define LOG
#endif
#define TEST
#endif


#if defined(BENCH_DETAIL) && !defined(BENCH)
#define BENCH
#endif

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <fenv.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#ifdef BENCH
#include <chrono>
#define BENCH_CODE(...) __VA_ARGS__
#else
#define BENCH_CODE(...)
#endif


#ifdef TEST
#define TEST_CODE(...) __VA_ARGS__
#else
#define TEST_CODE(...)
#endif

#ifdef AVX512

#pragma GCC target( \
    "sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,avx512f,avx512dq,avx512cd,avx512bw,avx512vl")
#pragma GCC optimize("O3,unroll-loops")
#include <x86intrin.h>
#else
#ifdef NDEBUG
#pragma GCC optimize("O3,unroll-loops")
#endif
#endif

using floating = double;

BENCH_CODE(
    floating kalman_time = 0;
    floating kalman_time2 = 0;
    floating kalman_time3 = 0;
    floating mcmc_time = 0;
    floating dijkstra_time = 0;)


template <int H, int W>
struct alignas(std::min(int(sizeof(floating)) * H * W, 64)) Matrix {
    floating data[H][W] = {};

    constexpr floating operator()(int y, int x) const
    {
        assert(0 <= y);
        assert(0 <= x);
        assert(y < H);
        assert(x < W);
        return data[y][x];
    }

    constexpr floating& operator()(int y, int x)
    {
        assert(0 <= y);
        assert(0 <= x);
        assert(y < H);
        assert(x < W);
        return data[y][x];
    }

    constexpr floating operator()(int i) const
    {
        if constexpr (W == 1) {
            return (*this)(i, 0);
        } else {
            static_assert(H == 1);
            return (*this)(0, i);
        }
    }

    constexpr floating& operator()(int i)
    {
        if constexpr (W == 1) {
            return (*this)(i, 0);
        } else {
            static_assert(H == 1);
            return (*this)(0, i);
        }
    }

    constexpr Matrix<H, W>& operator+=(const Matrix<H, W>& a)
    {
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                (*this)(y, x) += a(y, x);
            }
        }
        return *this;
    }

    constexpr Matrix<H, W>& operator-=(const Matrix<H, W>& a)
    {
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                (*this)(y, x) -= a(y, x);
            }
        }
        return *this;
    }

    friend constexpr Matrix<H, W> operator+(Matrix<H, W> a, const Matrix<H, W>& b)
    {
        return a += b;
    }

    friend constexpr Matrix<H, W> operator-(Matrix<H, W> a, const Matrix<H, W>& b)
    {
        return a -= b;
    }

    friend constexpr Matrix<H, W> operator-(const Matrix<H, W>& a)
    {
        Matrix<H, W> ret;
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                ret(y, x) = -a(y, x);
            }
        }
        return ret;
    }

    template <int L>
    friend constexpr Matrix<H, L> operator*(const Matrix<H, W>& a, const Matrix<W, L>& b)
    {
        Matrix<H, L> ret;
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < L; x++) {
                floating sum = 0;
                for (int k = 0; k < W; k++) {
                    sum += a(y, k) * b(k, x);
                }
                ret(y, x) = sum;
            }
        }
        return ret;
    }


    constexpr Matrix<W, H> T() const
    {
        Matrix<W, H> ret;
        for (int y = 0; y < W; y++) {
            for (int x = 0; x < H; x++) {
                ret(y, x) = (*this)(x, y);
            }
        }
        return ret;
    }
};

Matrix<1, 1> inv(Matrix<1, 1> a) { return {{{1.0f / a(0, 0)}}}; }
floating det(Matrix<1, 1> a) { return a(0, 0); }
floating det(Matrix<2, 2> a) { return a(0, 0) * a(1, 1) - a(0, 1) * a(1, 0); }
Matrix<2, 2> inv(Matrix<2, 2> a)
{
    double d = det(a);
    return Matrix<2, 2>{{{a(1, 1) / d, -a(0, 1) / d}, {-a(1, 0) / d, a(0, 0) / d}}};
}
Matrix<2, 2> llt(Matrix<2, 2> a)
{
    return Matrix<2, 2>{{{std::sqrt(a(0, 0)), 0},
        {a(1, 0) / std::sqrt(a(0, 0)), std::sqrt(a(1, 1) - a(1, 0) * a(1, 0) / a(0, 0))}}};
}

template <int N, class Rng>
Matrix<N, 1> rand_normal(Rng& rng)
{
    Matrix<N, 1> ret = {};
    for (int i = 0; i < N; i++) {
        ret(i) = std::normal_distribution{}(rng);
    }
    return ret;
}

constexpr int NSEG = 60;

struct HistoryCache {
    std::array<int, 28> kl;
    std::array<int, 28> kr;
};
struct Edge {
    int i_seg;
    int i_elem;
};
struct HistoryEntry {
    std::vector<Edge> route;
    double c;
    double err;
};
struct StateForSegment {
    int s;
    double l;
    double r;
    std::vector<HistoryCache> k;
    std::vector<int> relevant_histories;
    std::array<double, 29> mean_cost;
    std::array<double, 29> var_cost;
    double mean_n;
};
struct State {
    std::array<StateForSegment, NSEG> s;
    std::vector<HistoryEntry> h;
    std::vector<double> pred;
};


void addHistory(State& st, HistoryEntry&& h)
{

    for (int i = 0; i < NSEG; i++) {
        st.s[i].k.push_back({{}, {}});
    }
    std::array<bool, NSEG> visited{};
    double c = 0;
    for (int j = 0; j < int(h.route.size()); j++) {
        if (!visited[h.route[j].i_seg]) {
            st.s[h.route[j].i_seg].relevant_histories.push_back(st.h.size());
        }
        visited[h.route[j].i_seg] = true;

        for (int k = 0; k < 28; k++) {
            if (h.route[j].i_elem <= k) {
                st.s[h.route[j].i_seg].k.back().kl[k]++;
            } else {
                st.s[h.route[j].i_seg].k.back().kr[k]++;
            }
        }
        int k = st.s[h.route[j].i_seg].s;
        if (h.route[j].i_elem <= k) {
            c += st.s[h.route[j].i_seg].l;
            //            std::cerr << "#l " << c << " " << st.s[h.route[j].i_seg].l << std::endl;
        } else {
            c += st.s[h.route[j].i_seg].r;
            //            std::cerr << "#r " << c << " " << st.s[h.route[j].i_seg].r << std::endl;
        }
    }
    st.h.push_back(std::move(h));
    st.pred.push_back(c);
}
template <class Rng>
void resample(State& st, int seg, floating initial_edge_sigma, floating initial_cost, Rng& rng)
{

    BENCH_CODE(auto start = std::chrono::steady_clock::now();)

    int new_s;
    Matrix<2, 1> lr;
    if (st.s[seg].relevant_histories.empty()) {
        new_s = std::uniform_int_distribution{0, 27}(rng);
        lr(0) = std::uniform_int_distribution{1000, 9000}(rng);
        lr(1) = std::uniform_int_distribution{1000, 9000}(rng);
    } else {
        std::array<double, 28> log_w{};
        std::array<Matrix<2, 2>, 28> a{};
        std::array<Matrix<1, 2>, 28> b{};
        std::array<double, 28> c{};
        for (int i = 0; i < 28; i++) {
            a[i] = {{{1. / (initial_edge_sigma * initial_edge_sigma), 0.0}, {0.0, 1 / (initial_edge_sigma * initial_edge_sigma)}}};
            b[i] = {{{initial_cost / (initial_edge_sigma * initial_edge_sigma), initial_cost / (initial_edge_sigma * initial_edge_sigma)}}};
        }

        for (int i : st.s[seg].relevant_histories) {
            double norma = st.h[i].c - st.pred[i]
                           + st.s[seg].l * st.s[seg].k[i].kl[st.s[seg].s]
                           + st.s[seg].r * st.s[seg].k[i].kr[st.s[seg].s];
            for (int j = 0; j < 28; j++) {
                Matrix<1, 2> klr{{{double(st.s[seg].k[i].kl[j]), double(st.s[seg].k[i].kr[j])}}};
                a[j] += klr.T() * Matrix<1, 1>{{{1. / (st.h[i].err * st.h[i].err)}}} * klr;
                //                std::cerr << ">" << st.h[i].c << " " <<  st.pred[i]  << " " << st.s[seg].k[i].kl[st.s[seg].s] << " " << st.s[seg].k[i].kr[st.s[seg].s] << " " << norma << " " << st.h[i].err << std::endl;
                b[j] += Matrix<1, 1>{{{norma / (st.h[i].err * st.h[i].err)}}} * klr;
                c[j] += norma * norma / (st.h[i].err * st.h[i].err);
            }
        }
        double max_w = -1e30;

        for (int j = 0; j < 28; j++) {
            log_w[j] = -0.5 * (-std::log(det(a[j])) + c[j] - (b[j] * inv(a[j]) * b[j].T())(0, 0));
            max_w = std::max(max_w, log_w[j]);
        }

        std::array<double, 28> w{};
        for (int j = 0; j < 28; j++) {
            w[j] = std::exp(log_w[j] - max_w);
        }
        constexpr int TRIAL_NUM = 1;
        for (int i = 0; i < TRIAL_NUM; i++) {
            new_s = std::discrete_distribution{w.begin(), w.end()}(rng);
            Matrix<2, 2> Sigma = inv(a[new_s]);
            Matrix<2, 1> lr0 = Sigma * b[new_s].T();
            //                        std::cerr << "!" << lr0(0) << " " << lr0(1) << std::endl;
            Matrix<2, 2> basis = llt(Sigma);
            lr = lr0 + basis * rand_normal<2>(rng);
            if (lr(0) >= 1000 && lr(0) <= 9000 && lr(1) >= 1000 && lr(1) < 9000) {
                break;
            }
            if (i == TRIAL_NUM - 1) {
                lr(0) = std::clamp(lr(0), 1000., 9000.);
                lr(1) = std::clamp(lr(1), 1000., 9000.);
                //                new_s = std::uniform_int_distribution{0, 27}(rng);
                //                lr(0) = std::uniform_int_distribution{1000, 9000}(rng);
                //                lr(1) = std::uniform_int_distribution{1000, 9000}(rng);
            }
        }
    }

    for (int i : st.s[seg].relevant_histories) {
        st.pred[i] += (lr(0) * st.s[seg].k[i].kl[new_s] - st.s[seg].l * st.s[seg].k[i].kl[st.s[seg].s])
                      + (lr(1) * st.s[seg].k[i].kr[new_s] - st.s[seg].r * st.s[seg].k[i].kr[st.s[seg].s]);
        //        std::cerr << "%" << st.pred[i] << std::endl;
    }
    st.s[seg].l = lr(0);
    st.s[seg].r = lr(1);
    st.s[seg].s = new_s;
    for (int i = 0; i < 29; i++) {
        double cost = i <= new_s ? lr(0) : lr(1);
        double dmean = (cost - st.s[seg].mean_cost[i]) / (st.s[seg].mean_n + 1);
        st.s[seg].mean_cost[i] += dmean;
        //        std::cerr << "!" << st.s[seg].mean_cost[i] << std::endl;
        //        st.s[seg].var_cost[i] += st.s[seg].mean_n * dmean * dmean + (cost - st.s[seg].mean_cost[i]) * (cost - st.s[seg].mean_cost[i]);
        double dvar = (st.s[seg].mean_n * dmean * dmean + (cost - st.s[seg].mean_cost[i]) * (cost - st.s[seg].mean_cost[i]) - st.s[seg].var_cost[i]) / (st.s[seg].mean_n + 1);
        st.s[seg].var_cost[i] += dvar;
        st.s[seg].mean_n += 1;
    }

    BENCH_CODE(auto end = std::chrono::steady_clock::now();
               mcmc_time += (end - start).count() * 1e-9f;)
}

using floating = double;


constexpr int H(int y, int x)
{
    assert(y >= 0);
    assert(x >= 0);
    assert(y < 30);
    assert(x < 29);
    return y * 32 + x;
}

constexpr int V(int y, int x)
{
    assert(y >= 0);
    assert(x >= 0);
    assert(y < 29);
    assert(x < 30);
    return y + x * 32 + 32 * 30;
}

constexpr int N = 32 * 30 * 2;
//constexpr int NE = (N+15) / 16 * 16;
//constexpr int NSEG = 30 * 2;


template <int N>
struct alignas(64) Vector {
    static_assert(N * sizeof(floating) % 64 == 0);
    std::array<floating, N> data;
    constexpr floating& operator[](int i)
    {
        assert(i >= 0);
        assert(i < N);
        return data[i];
    }
    constexpr floating operator[](int i) const
    {
        return const_cast<Vector&>(*this)[i];
    }

    Vector& operator+=(const Vector& a)
    {
        for (int i = 0; i < N; i++) {
            data[i] += a[i];
        }
        return *this;
    }

    Vector operator+(const Vector& a) const
    {
        Vector ret;
        for (int i = 0; i < N; i++) {
            ret[i] = data[i] + a[i];
        }
        return ret;
    }

    Vector& operator-=(const Vector& a)
    {
        for (int i = 0; i < N; i++) {
            data[i] -= a[i];
        }
        return *this;
    }

    Vector operator-(const Vector& a) const
    {
        Vector ret;
        for (int i = 0; i < N; i++) {
            ret[i] = data[i] - a[i];
        }
        return ret;
    }

    Vector& operator*=(floating a)
    {
        for (int i = 0; i < N; i++) {
            data[i] *= a;
        }
        return *this;
    }

    friend Vector operator*(floating a, const Vector& b)
    {
        Vector ret;
        for (int i = 0; i < N; i++) {
            ret[i] = a * b[i];
        }
        return ret;
    }
};
template <int N>
floating dot(const Vector<N>& a, const Vector<N>& b)
{
    floating sum = 0;
    for (int i = 0; i < N; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}


using Segment = Vector<32>;

struct alignas(64) DenseVec {
    std::array<Segment, NSEG> data;

    constexpr Segment& operator[](int i)
    {
        assert(i >= 0);
        assert(i < NSEG);
        return data[i];
    }
    constexpr const Segment& operator[](int i) const
    {
        return const_cast<DenseVec&>(*this)[i];
    }
    constexpr floating& operator()(int var)
    {
        return data[(std::uint32_t)var / 32][(std::uint32_t)var % 32];
    }
    constexpr const floating& operator()(int i) const
    {
        return const_cast<DenseVec&>(*this)(i);
    }

    DenseVec& operator+=(const DenseVec& a)
    {
        for (int i = 0; i < NSEG; i++) {
            data[i] += a[i];
        }
        return *this;
    }

    DenseVec operator+(const DenseVec& a) const
    {
        DenseVec ret;
        for (int i = 0; i < NSEG; i++) {
            ret[i] = data[i] + a[i];
        }
        return ret;
    }

    DenseVec& operator-=(const DenseVec& a)
    {
        for (int i = 0; i < NSEG; i++) {
            data[i] -= a[i];
        }
        return *this;
    }

    DenseVec operator-(const DenseVec& a) const
    {
        DenseVec ret;
        for (int i = 0; i < NSEG; i++) {
            ret[i] = data[i] - a[i];
        }
        return ret;
    }

    DenseVec& operator*=(floating a)
    {
        for (int i = 0; i < NSEG; i++) {
            data[i] *= a;
        }
        return *this;
    }

    friend DenseVec operator*(floating a, const DenseVec& b)
    {
        DenseVec ret;
        for (int i = 0; i < NSEG; i++) {
            ret[i] = a * b[i];
        }
        return ret;
    }
    static DenseVec fill(floating x)
    {
        DenseVec ret;
        for (int i = 0; i < NSEG; i++) {
            for (int j = 0; j < 32; j++) {
                ret[i][j] = x;
            }
        }
        return ret;
    }
};


struct SparseVecEntry {
    int i_elem;
    floating v;

    constexpr int i_seg() const { return (std::uint32_t)(i_elem) / 32; }
    constexpr int i_elem_in_seg() const { return (std::uint32_t)(i_elem) % 32; }
};
using SparseVec = std::vector<SparseVecEntry>;

constexpr auto sparsevec_idx_comp = [](const SparseVecEntry& a, const SparseVecEntry& b) {
    return a.i_elem < b.i_elem;
};
floating dot(const DenseVec& a, const SparseVec& b)
{
    floating sum = 0;
    for (const SparseVecEntry& e : b) {
        sum += a[e.i_seg()][e.i_elem_in_seg()] * e.v;
    }
    return sum;
}


static constexpr int SMALL_DIM_PER_SEG = 2;
static constexpr int SMALL_DIMS = NSEG * SMALL_DIM_PER_SEG;
static constexpr int SMALL_DIMS_EX = (SMALL_DIMS + 7) / 8 * 8;

using SmallVector = Vector<SMALL_DIMS_EX>;

constexpr std::array<Vector<32>, SMALL_DIM_PER_SEG> small_vec_weights = [] {
    std::array<Vector<32>, SMALL_DIM_PER_SEG> ret{};
    for (int i = 0; i < 29; i++) {
        ret[0][i] = floating(28 - i) * (2. / (28 * 29 / 2));
        ret[1][i] = floating(i) * (2. / (28 * 29 / 2));
        //        ret[0][i] = 1./29;
    }
    return ret;
}();

using SmallMatrix = std::array<SmallVector, SMALL_DIMS_EX>;
//struct alignas(64) SmallMatrix{
//    std::array<Smal, DIMS> data;
//};


struct Kernel {
    std::array<Segment, 32> data{};
    const Segment& column(int i) const
    {
        return data[i];
    }
    DenseVec diag() const
    {
        DenseVec ret{};
        for (int i = 0; i < NSEG; i++) {
            for (int j = 0; j < 32; j++) {
                ret[i][j] = data[j][j];
            }
        }
        return ret;
    }
    constexpr floating& operator()(int i, int j)
    {
        assert(i >= 0);
        assert(j >= 0);
        assert(i < 29);
        assert(j < 29);
        return data[i][j];
    }

    std::array<std::array<floating, SMALL_DIM_PER_SEG>, SMALL_DIM_PER_SEG> initial_small_kernel() const
    {
        std::array<std::array<floating, SMALL_DIM_PER_SEG>, SMALL_DIM_PER_SEG> ret = {};
        for (int i = 0; i < 29; i++) {
            for (int j = 0; j < 29; j++) {
                for (int k1 = 0; k1 < SMALL_DIM_PER_SEG; k1++) {
                    for (int k2 = 0; k2 < SMALL_DIM_PER_SEG; k2++) {
                        ret[k1][k2] += data[i][j] * small_vec_weights[k1][i] * small_vec_weights[k2][j];
                    }
                }
            }
        }
        return ret;
    }
    template <class F>
    Kernel(F&& f)
    {
        for (int i = 0; i < 29; i++) {
            for (int j = 0; j < 29; j++) {
                (*this)(i, j) = f(i, j);
            }
        }
    }
    Kernel() = default;
};


struct alignas(64) Variance {
    Kernel P0{};
    DenseVec Pdiag = P0.diag();
    //    SmallMatrix Psmall = {};
    std::array<DenseVec, 1000> Padd{};
    std::array<floating, 1000> PaddD{};
    int Padd_n = 0;

    Variance() = default;
    Variance(const Kernel& P0) : P0{P0}  //, Psmall{}
    {
        //        auto initial_small_kernel = P0.initial_small_kernel();
        //        for (int i = 0; i < NSEG; i++) {
        //            for (int k1 = 0; k1 < SMALL_DIM_PER_SEG; k1++) {
        //                for (int k2 = 0; k2 < SMALL_DIM_PER_SEG; k2++) {
        //                    Psmall[i * SMALL_DIM_PER_SEG + k1][i * SMALL_DIM_PER_SEG + k2] = initial_small_kernel[k1][k2];
        //                }
        //            }
        //        }
    }
};

floating kalman_correct_lazy(DenseVec& x, Variance& var, const SparseVec& H, floating dy, floating R)
{
    BENCH_CODE(auto start = std::chrono::steady_clock::now();)

    // Sxy = P * H.T() = P0 * H.T() + Σ Padd[i] * (Padd[i].T() * H)
    DenseVec Sxy = {};
    for (const SparseVecEntry& entry : H) {
        Sxy[entry.i_seg()] += entry.v * var.P0.column(entry.i_elem_in_seg());
    }

    BENCH_CODE(auto start2 = std::chrono::steady_clock::now();)
    //    for (int j = 0; j < int(Padd.size()); j++) {
    //        floating multiplier = dot(Padd[j], H) * PaddD[j];
    //        Sxy += multiplier * Padd[j];
    //    }
    std::array<floating, 1000> dots{};
    for (const SparseVecEntry& e : H) {
        for (int j = 0; j < var.Padd_n; j++) {
            dots[j] += var.Padd[j][e.i_seg()][e.i_elem_in_seg()] * e.v;
        }
    }
    BENCH_CODE(auto end2 = std::chrono::steady_clock::now();
               kalman_time2 += (end2 - start2).count() * 1e-9f;)

    BENCH_CODE(auto start3 = std::chrono::steady_clock::now();)
#ifdef AVX512
    for (int j = 0; j < var.Padd_n; j += 4) {
        __m512d a0 = _mm512_set1_pd(dots[j + 0] * var.PaddD[j + 0]);
        __m512d a1 = _mm512_set1_pd(dots[j + 1] * var.PaddD[j + 1]);
        __m512d a2 = _mm512_set1_pd(dots[j + 2] * var.PaddD[j + 2]);
        __m512d a3 = _mm512_set1_pd(dots[j + 3] * var.PaddD[j + 3]);
        for (int i = 0; i < NSEG; i++) {
            for (int k = 0; k < 32; k += 8) {
                __m512d c = _mm512_load_pd(&Sxy[i][k]);
                __m512d b = _mm512_load_pd(&var.Padd[j + 0][i][k]);
                c = _mm512_fmadd_pd(a0, b, c);
                b = _mm512_load_pd(&var.Padd[j + 1][i][k]);
                c = _mm512_fmadd_pd(a1, b, c);
                b = _mm512_load_pd(&var.Padd[j + 2][i][k]);
                c = _mm512_fmadd_pd(a2, b, c);
                b = _mm512_load_pd(&var.Padd[j + 3][i][k]);
                c = _mm512_fmadd_pd(a3, b, c);
                _mm512_store_pd(&Sxy[i][k], c);
            }
        }
    }
#else
    for (int j = 0; j < var.Padd_n; j++) {
        Sxy += (dots[j] * var.PaddD[j]) * var.Padd[j];
    }
#endif
    BENCH_CODE(auto end3 = std::chrono::steady_clock::now();
               kalman_time3 += (end3 - start3).count() * 1e-9f;)

    // Syy = R + H * Sxy;
    floating Syy = R + dot(Sxy, H);

    //x += Sxy * Syy^-1 * Matrix<1, 1>{{{dy}}};
    x += (dy / Syy) * Sxy;

    //    P -= Sxy * Syy^-1 * Sxy.T();
    var.Padd[var.Padd_n] = Sxy;
    var.PaddD[var.Padd_n] = -(1 / Syy);
    var.Padd_n++;
    for (int i = 0; i < NSEG; i++) {
        for (int j = 0; j < 32; j++) {
            var.Pdiag[i][j] -= Sxy[i][j] / Syy * Sxy[i][j];
        }
    }
    //
    //    SmallVector sv{};
    //    for (int i = 0; i < NSEG; i++) {
    //        for (int j = 0; j < 29; j++) {
    //            for (int k = 0; k < SMALL_DIM_PER_SEG; k++) {
    //                sv[i * SMALL_DIM_PER_SEG + k] += Sxy[i][j] * small_vec_weights[k][j];
    //            }
    //        }
    //    }
    //    for (int i = 0; i < NSEG; i++) {
    //        for (int j = 0; j < NSEG; j++) {
    //            for (int k1 = 0; k1 < SMALL_DIM_PER_SEG; k1++) {
    //                for (int k2 = 0; k2 < SMALL_DIM_PER_SEG; k2++) {
    //                    var.Psmall[i * SMALL_DIM_PER_SEG + k1][j * SMALL_DIM_PER_SEG + k2] -= sv[i * SMALL_DIM_PER_SEG + k1] * sv[j * SMALL_DIM_PER_SEG + k2] / Syy;
    //                }
    //            }
    //        }
    //    }


#ifndef SILENT
    std::cerr << std::sqrt(Syy) << " " << std::log(Syy) << " " << dy << " " << dy / Syy * dy << std::endl;
#endif

    floating logp = -0.5f * (std::log(2 * M_PI) + std::log(Syy) + dy / Syy * dy);
    BENCH_CODE(auto end = std::chrono::steady_clock::now();
               kalman_time += (end - start).count() * 1e-9f;)
    return logp;
}


struct KalmanFilter {
    DenseVec X;
    Variance var;
    double logp_sum = 0.0;

    KalmanFilter(const DenseVec& initial_x, const Kernel& initial_p, double initial_weight = 1.0)
        : X(initial_x), var{initial_p}, logp_sum{std::log(initial_weight)} {}

    void print_log(std::ostream& os)
    {
        os << logp_sum << "\n";
        for (int y = 0; y < 30; y++) {
            for (int x = 0; x < 29; x++) {
                int v = H(y, x);
                os << X(v) << " " << std::sqrt(var.Pdiag(v)) << " ";
            }
            os << "\n";
        }

        for (int y = 0; y < 29; y++) {
            for (int x = 0; x < 30; x++) {
                int v = V(y, x);
                os << X(v) << " " << std::sqrt(var.Pdiag(v)) << " ";
            }
            os << "\n";
        }
    }

    floating predict(const SparseVec& H) const
    {
        return dot(X, H);
    }
    double correct(const SparseVec& H, floating y, floating R)
    {
        double logp = kalman_correct_lazy(X, var, H, y - predict(H), R);
        logp_sum += logp;
        return logp;
    }
};


struct Vertex {
    int y;
    int x;

    constexpr bool operator==(const Vertex& v) const
    {
        return v.y == y && v.x == x;
    }
    constexpr bool operator!=(const Vertex& v) const
    {
        return !(*this == v);
    }
};

struct Entry {
    floating d;
    Vertex v;
    char last_op;
    int tie_breaker;

    //    std::string s;
    //    std::vector<int> vars;

    bool operator<(const Entry& e) const
    {
        return d < e.d || (d == e.d && tie_breaker < e.tie_breaker);
    }

    bool operator>(const Entry& e) const
    {
        return d > e.d || (d == e.d && tie_breaker > e.tie_breaker);
    }
};

struct Result {
    floating d;
    std::string str;
    std::vector<Vertex> path;
};


constexpr floating DIJKSTRA_MIN_COST = 0;
//floating rand_intensity = 0.0f;

template <class T, int H, int W>
struct Array2D {
    std::array<std::array<T, W>, H> data;
    T& operator()(int i, int j)
    {
        assert(i >= 0);
        assert(j >= 0);
        assert(i < H);
        assert(j < W);
        return data[i][j];
    }
    const T& operator()(int i, int j) const
    {
        assert(i >= 0);
        assert(j >= 0);
        assert(i < H);
        assert(j < W);
        return data[i][j];
    }
};

std::mt19937 rng{1235};

template <class FV, class FH>
Result dijkstra(const Vertex& s, const Vertex& t, FV&& cost_v, FH&& cost_h)
{
    BENCH_CODE(auto start = std::chrono::steady_clock::now();)

    Array2D<floating, 30, 30> d;
    for (int y = 0; y < 30; y++) {
        for (int x = 0; x < 30; x++) {
            d(y, x) = 1e30f;
        }
    }
    Array2D<char, 30, 30> last_op{};

    std::priority_queue<Entry, std::vector<Entry>, std::greater<>> Q;

    int cnt = 0;
    auto tie_breaker = [&] { return std::rand(); };
    Q.push({0, s, '@', tie_breaker()});

    int count = 0;
    while (!Q.empty()) {
        auto [d0, v, op, _] = Q.top();
        Q.pop();
        if (v.y < 0 || v.x < 0 || v.y >= 30 || v.x >= 30) {
            assert(false);
        }
        if (d(v.y, v.x) <= d0)
            continue;
        last_op(v.y, v.x) = op;
        if (v == t) {
            std::vector<Vertex> path;
            constexpr int BUFLEN = 900;
            static std::array<char, BUFLEN + 1> buf{};
            int i = BUFLEN;
            buf[i] = '\0';
            Vertex v1 = v;
            while (true) {
                i--;
                path.push_back(v1);
                buf[i] = last_op(v1.y, v1.x);
                switch (buf[i]) {
                case 'U':
                    v1.y++;
                    break;
                case 'D':
                    v1.y--;
                    break;
                case 'L':
                    v1.x++;
                    break;
                case 'R':
                    v1.x--;
                    break;
                case '\0':
                    assert(false);
                case '@':
                    i++;
                    BENCH_CODE(
                        auto end = std::chrono::steady_clock::now();
                        dijkstra_time += (end - start).count() * 1e-9;)
                    std::reverse(path.begin(), path.end());
                    return {d0, std::string{buf.data() + i}, std::move(path)};
                }
            }
        }
        d(v.y, v.x) = d0;
        count++;
        if (v.y >= 1) {
            floating c = std::max(DIJKSTRA_MIN_COST, cost_v({v.y - 1, v.x}));  //+ rand_intensity * std::uniform_real_distribution<floating>(-1.0f, 1.0f)(rng));
            Q.push({d0 + c, {v.y - 1, v.x}, 'U', tie_breaker()});
        }
        if (v.y <= 28) {
            floating c = std::max(DIJKSTRA_MIN_COST, cost_v(v));  //  + rand_intensity * std::uniform_real_distribution<floating>(-1.0f, 1.0f)(rng));
            Q.push({d0 + c, {v.y + 1, v.x}, 'D', tie_breaker()});
        }
        if (v.x >= 1) {
            floating c = std::max(DIJKSTRA_MIN_COST, cost_h({v.y, v.x - 1}));  //  + rand_intensity * std::uniform_real_distribution<floating>(-1.0f, 1.0f)(rng));
            Q.push({d0 + c, {v.y, v.x - 1}, 'L', tie_breaker()});
        }
        if (v.x <= 28) {
            floating c = std::max(DIJKSTRA_MIN_COST, cost_h(v));  // + rand_intensity * std::uniform_real_distribution<floating>(-1.0f, 1.0f)(rng));
            Q.push({d0 + c, {v.y, v.x + 1}, 'R', tie_breaker()});
        }
    }
    assert(false);
    return {1e30f, "error", {}};
}


struct SimpleEntry {
    floating d;
    Vertex v;

    bool operator<(const SimpleEntry& e) const
    {
        return d < e.d;
    }

    bool operator>(const SimpleEntry& e) const
    {
        return d > e.d;
    }
};

template <class PathState>
struct ResultWithPathState {
    floating d;
    std::string str;
    std::vector<Vertex> path;
    PathState& pathstate;
};
template <class PathState, class F>
ResultWithPathState<PathState> dijkstra_path_dependent(const Vertex& s, const Vertex& t, F&& costfunc, const PathState& initial_pathstate)
{
    BENCH_CODE(auto start = std::chrono::steady_clock::now();)

    Array2D<floating, 30, 30> d;
    for (int y = 0; y < 30; y++) {
        for (int x = 0; x < 30; x++) {
            d(y, x) = 1e30f;
        }
    }
    Array2D<char, 30, 30> last_op{};
    Array2D<bool, 30, 30> visited{};
    static Array2D<PathState, 30, 30> path_state;

    std::priority_queue<SimpleEntry, std::vector<SimpleEntry>, std::greater<>> Q;

    path_state(s.y, s.x) = initial_pathstate;
    d(s.y, s.x) = 0;
    last_op(s.y, s.x) = '@';
    Q.push({0, s});

    int count = 0;
    while (!Q.empty()) {
        auto [d0, v] = Q.top();
        Q.pop();
        if (v.y < 0 || v.x < 0 || v.y >= 30 || v.x >= 30) {
            assert(false);
        }
        if (visited(v.y, v.x))
            continue;
        visited(v.y, v.x) = true;

        if (v == t) {
            std::vector<Vertex> path;
            constexpr int BUFLEN = 900;
            static std::array<char, BUFLEN + 1> buf{};
            int i = BUFLEN;
            buf[i] = '\0';
            Vertex v1 = v;
            while (true) {
                i--;
                path.push_back(v1);
                buf[i] = last_op(v1.y, v1.x);
                switch (buf[i]) {
                case 'U':
                    v1.y++;
                    break;
                case 'D':
                    v1.y--;
                    break;
                case 'L':
                    v1.x++;
                    break;
                case 'R':
                    v1.x--;
                    break;
                case '\0':
                    assert(false);
                case '@':
                    i++;
                    BENCH_CODE(
                        auto end = std::chrono::steady_clock::now();
                        dijkstra_time += (end - start).count() * 1e-9;)
                    std::reverse(path.begin(), path.end());
                    return {d0, std::string{buf.data() + i}, std::move(path), path_state(t.y, t.x)};
                }
            }
        }
        count++;
        auto push = [&, &v = v](const Vertex& new_v, auto&& get_var, int seg, int index_in_seg, char op) {
            if (new_v.y >= 0 && new_v.x >= 0 && new_v.y < 30 && new_v.x < 30 && !visited(new_v.y, new_v.x)) {
                PathState pathstate_out;
                floating cost = costfunc(get_var(), seg, index_in_seg, path_state(v.y, v.x), pathstate_out);
                if (cost < d(new_v.y, new_v.x)) {
                    d(new_v.y, new_v.x) = cost;
                    last_op(new_v.y, new_v.x) = op;
                    path_state(new_v.y, new_v.x) = pathstate_out;
                    Q.push({cost, new_v});
                }
            }
        };

        push(
            {v.y - 1, v.x}, [&v = v] { return V(v.y - 1, v.x); },
            30 + v.x, v.y - 1, 'U');
        push(
            {v.y + 1, v.x}, [&v = v] { return V(v.y, v.x); },
            30 + v.x, v.y, 'D');
        push(
            {v.y, v.x - 1}, [&v = v] { return H(v.y, v.x - 1); },
            v.y, v.x - 1, 'L');
        push(
            {v.y, v.x + 1}, [&v = v] { return H(v.y, v.x); },
            v.y, v.x, 'R');
    }
    assert(false);
    return {1e30f, "error", {}, path_state(0, 0)};
}


floating sq(floating x) { return x * x; }
template <class T, std::size_t N>
constexpr std::integral_constant<std::size_t, N> arrayLength(const T (&)[N])
{
    return {};
}
int main(int argc, char** argv)
{
    auto safety_start = std::chrono::steady_clock::now();

    std::srand(1234);
#ifdef DEBUG
    std::this_thread::sleep_for(std::chrono::seconds(1));
#endif

    //    feenableexcept(FE_INVALID);
    std::ios_base::sync_with_stdio(false);

#ifdef LOG
    std::ofstream log_stream{"log.out"};
#endif

#ifdef MAKE_TEST
    std::ofstream echo_stream{"echo.out"};
#endif
    //#ifdef BENCH_DETAIL
    //    std::ofstream bench_detail_stream{"bench_detail.out"};
    //#endif


    floating initial_cost = 5000.f;
    floating initial_edge_sigma = 4000.0 / std::sqrt(3.f);
    floating observe_sigma_per_c = 0.1f / std::sqrt(3.f);
    double observe_sigma_per_len = 1000.0 / std::sqrt(3.f);
    //    floating gamma0 = 1.5;
    floating cutoff_threshold = -15;

    struct GammaParams {
        floating gamma0 = 1.5;
        floating gammap = 2.0;
    };
    bool use_provided_gamma_params = true;
    GammaParams provided_gamma_params{};
    if (argc >= 1 + 1) {
        provided_gamma_params.gamma0 = std::atof(argv[1]);
        provided_gamma_params.gammap = std::atof(argv[2]);
        use_provided_gamma_params = true;
    }


    struct Model {
        KalmanFilter kf;
        GammaParams gamma_params;
    };
    auto make_kalman_filter1 = [&](floating initial_common_sigma, floating initial_independent_sigma, const GammaParams& gamma_params = {}, floating weight = 1) {
        return Model{KalmanFilter{DenseVec::fill(initial_cost), Kernel{[&](int i, int j) {
                                      return sq(initial_common_sigma) + (i == j ? sq(initial_independent_sigma) : 0.0f);
                                  }},
                         weight},
            gamma_params};
    };
    auto make_kalman_filter2 = [&](floating initial_common_sigma, floating initial_independent_sigma, const GammaParams& gamma_params, floating weight = 1) {
        return Model{KalmanFilter{DenseVec::fill(initial_cost), Kernel{[&](int i, int j) {
                                      floating kernel_w = floating(28 - std::max(i, j) + std::min(i, j)) / 28;
                                      return kernel_w * sq(initial_common_sigma) + (i == j ? sq(initial_independent_sigma) : 0.0f);
                                  }},
                         weight},
            gamma_params};
    };

    static Model all_models[] = {
        make_kalman_filter1(2250.f / std::sqrt(3.f), 1750.f / std::sqrt(3.f), /*{18.526, 6.5840}*/ {12., 13.}),
        make_kalman_filter1(2750.f / std::sqrt(3.f), 1250.f / std::sqrt(3.f), /*{21.158, 13.823}*/ {9.0, 13.}),
        make_kalman_filter1(3250.f / std::sqrt(3.f), 750.0f / std::sqrt(3.f), /*{23.911, 15.836}*/ {6.0, 13.} /*{14.621, 14.776}*/),
        make_kalman_filter1(3750.f / std::sqrt(3.f), 250.0f / std::sqrt(3.f), /*{20.105, 17.752}*/ {4.0, 13.}),
        make_kalman_filter2(2250.f / std::sqrt(3.f), 1750.f / std::sqrt(3.f), /*{11.226, 4.0740}*/ {7.0, 10.}),
        make_kalman_filter2(2750.f / std::sqrt(3.f), 1250.f / std::sqrt(3.f), /*{12.013, 4.0310}*/ {7.0, 7.0}),
        make_kalman_filter2(3250.f / std::sqrt(3.f), 750.0f / std::sqrt(3.f), /*{26.104, 5.1070}*/ {7.0, 5.0}),
        make_kalman_filter2(3750.f / std::sqrt(3.f), 250.0f / std::sqrt(3.f), /*{8.2810, 3.8380}*/ {7.0, 2.0}),
    };

    auto is_mcmc_model = [](int m) { return m == 6 || m == 7; };

    auto mcmc_suspend = [](double p) { return p < 0.001; };

#ifdef MODEL
    static Model* models = &all_models[MODEL];
    constexpr int MODEL_NUM = 1;
#else
    static Model* models = all_models;
    constexpr int MODEL_NUM = arrayLength(all_models);
#endif

    State st{};
    for (int i = 0; i < NSEG; i++) {
        st.s[i].s = 14;
        st.s[i].l = initial_cost;
        st.s[i].r = initial_cost;
        st.s[i].mean_n = 0;
        for (int j = 0; j < 29; j++) {
            st.s[i].mean_cost[j] = initial_cost;
        }
    }

    auto gamma = [&](int i, const GammaParams& gamma_params) {
        floating x = 1 - (i / 1000.);
        //        floating ret = gamma_params.gamma0 / (1 + floating(i) / 300);
        //        floating ret = gamma0 * (0.7 * std::pow(x, 2) + 0.3);
        floating ret = gamma_params.gamma0 * std::pow(x, gamma_params.gammap);
#ifndef SILENT
//        std::cerr << i << " gamma: " << ret << "\n";
#endif
        return ret;
    };

    constexpr auto resample_schedule = [&](int a) constexpr
    {
        return a < 200 ? 10 : 2;  //1 + int(20. / (1. + a / 200.));
    };


    std::array<bool, MODEL_NUM> model_enabled{};
    for (bool& x : model_enabled)
        x = true;

    constexpr int UNIT_TIME_KALMAN = 10;
    constexpr int UNIT_TIME_MCMC = 9;
    int time_budget = 1'500'000'0;  // Currently, 1 time budget ~ 0.1us


    constexpr int must_use_time_for_mcmc_all = [&] {
        int ret = 0;
        for (int a = 0; a < 1000; a++) {
            ret += (a + 1) * UNIT_TIME_MCMC * resample_schedule(a);
        }
        return ret;
    }();
    constexpr int must_use_time_for_kalman_all = ((1 + 1000) * 1000 / 2) * UNIT_TIME_KALMAN;
    int must_use_time_for_mcmc = must_use_time_for_mcmc_all;
    int must_use_time_for_kalman = must_use_time_for_kalman_all;

    int suspended_mcmc_resamples = 0;
    int max_suspended_mcmc_resamples = 50;

    bool safety_stop_mode = false;

    for (int a = 0; a < 1000; a++) {
        Vertex s, t;

#ifndef SILENT
        std::cerr << "--------- a = " << a << " ---------\n";
#endif

        std::cin >> s.y >> s.x >> t.y >> t.x;

#ifdef SAFETY_STOP_TIME
        if ((std::chrono::steady_clock::now() - safety_start).count() * 1e-9 > SAFETY_STOP_TIME) {
            safety_stop_mode = true;
        }
#endif


        floating logp_offset = -1e30;
        int best_model = -1;
        for (int i = 0; i < MODEL_NUM; i++) {
            if (model_enabled[i]) {
                if (models[i].kf.logp_sum > logp_offset) {
                    best_model = i;
                    logp_offset = models[i].kf.logp_sum;
                }
            }
        }
        //normalization
        for (int i = 0; i < MODEL_NUM; i++) {
            if (model_enabled[i]) {
                models[i].kf.logp_sum -= logp_offset;
            } else {
                models[i].kf.logp_sum = -1e30;
            }
        }
        logp_offset = 0;


        auto mix_ps = [&] {
            std::array<floating, MODEL_NUM> mix_ps{};
            floating p_sum = 0;
            for (int i = 0; i < MODEL_NUM; i++) {
                if (model_enabled[i]) {
                    mix_ps[i] = std::exp(models[i].kf.logp_sum - logp_offset);
                    p_sum += mix_ps[i];
                } else {
                    mix_ps[i] = 0;
                }
            }
            for (int i = 0; i < MODEL_NUM; i++) {
                if (model_enabled[i]) {
                    mix_ps[i] /= p_sum;
                }
            }
            return mix_ps;
        }();

        double mcmc_p = 0;

        for (int i = 0; i < MODEL_NUM; i++) {
            if (is_mcmc_model(i)) {
                mcmc_p += mix_ps[i];
            }
        }


        int time_consumption_per_model = (a + 1) * UNIT_TIME_KALMAN;
        int time_consumption_mcmc = (a + 1) * UNIT_TIME_MCMC * resample_schedule(a);
        int expect_kalman_time_consumption = 0;
        int expect_mcmc_time_consumption = 0;
        bool is_mcmc_only = true;
        bool mcmc_enabled = false;

        bool mcmc_suspended = mcmc_suspend(mcmc_p);
        //        std::cerr << "mcmc_suspended: " << mcmc_suspended << std::endl;

        for (int i = 0; i < MODEL_NUM; i++) {
            if (model_enabled[i]) {
                if (models[i].kf.logp_sum - logp_offset < cutoff_threshold) {
                    // Prune!
                    model_enabled[i] = false;
                } else {
                    expect_kalman_time_consumption += time_consumption_per_model;
                    if (is_mcmc_model(i)) {
                        mcmc_enabled = true;
                    } else {
                        is_mcmc_only = false;
                    }
                }
            }
        }

        must_use_time_for_kalman -= time_consumption_per_model;

        if (mcmc_enabled) {
            if (is_mcmc_only) {
                expect_kalman_time_consumption = 0;
            }
            if (expect_kalman_time_consumption + must_use_time_for_mcmc + suspended_mcmc_resamples * (a + 1) * UNIT_TIME_MCMC > time_budget) {
                if (is_mcmc_model(best_model)) {
                    for (int i = 0; i < MODEL_NUM; i++) {
                        model_enabled[i] = i == best_model;
                    }
                    expect_kalman_time_consumption = 0;
                    is_mcmc_only = true;
                    mcmc_suspended = false;
                } else {
                    for (int i = 0; i < MODEL_NUM; i++) {
                        if (model_enabled[i] && is_mcmc_model(i)) {
                            model_enabled[i] = false;
                            expect_kalman_time_consumption -= must_use_time_for_kalman;
                        }
                    }
                    expect_mcmc_time_consumption = 0;
                    mcmc_enabled = false;
                }
            }
            if (!mcmc_suspended) {
                expect_mcmc_time_consumption = time_consumption_mcmc + suspended_mcmc_resamples * (a + 1) * UNIT_TIME_MCMC;
            }
        }

        must_use_time_for_mcmc -= time_consumption_mcmc;

        if (!is_mcmc_only && expect_kalman_time_consumption + expect_mcmc_time_consumption + must_use_time_for_kalman > time_budget) {
            // Choose the best model!
            for (int i = 0; i < MODEL_NUM; i++) {
                model_enabled[i] = i == best_model;
            }
            if (is_mcmc_model(best_model)) {
                is_mcmc_only = true;

                expect_kalman_time_consumption = 0;
                is_mcmc_only = true;
                mcmc_suspended = false;
            } else {
                mcmc_enabled = false;
                expect_mcmc_time_consumption = 0;
                expect_kalman_time_consumption = time_consumption_per_model;
            }
        }

        time_budget -= expect_kalman_time_consumption + expect_mcmc_time_consumption;

#ifdef LOG
        log_stream << a << " " << MODEL_NUM << " " << time_budget << "\n";
        for (int i = 0; i < MODEL_NUM; i++) {
            log_stream << models[i].kf.logp_sum << " " << mix_ps[i] << "\n";
            //            models[i].print_log(log_stream);
        }
        models[best_model].kf.print_log(log_stream);
#endif
#ifndef SILENT
        for (int i = 0; i < MODEL_NUM; i++) {
            if (model_enabled[i]) {
                std::cerr << "Model" << i << ": " << models[i].kf.logp_sum << " " << mix_ps[i] << " ";
            }
        }
        std::cerr << "\n";
#endif
        //        rand_intensity = 4000.0f / (1.0f + 0.1f * a);


        //        SmallMatrix Pmix{};
        //        SmallMatrix& P = models[best_model].kf.var.Psmall;

        struct PathState {
            //            SmallVector HP;
            //            floating HPH;
            floating HX;
            //debug
            //            int length = 0;
        };

        floating current_gamma = 0;
        if (use_provided_gamma_params) {
            current_gamma = gamma(a, provided_gamma_params);
        } else {
            for (int i = 0; i < MODEL_NUM; i++) {
                if (model_enabled[i]) {
                    current_gamma += mix_ps[i] * gamma(a, models[i].gamma_params);
                }
            }
        }

        auto get_cost = [&](int v, int vs, int vk, const PathState& path_state, PathState& path_state_out) {
            //                                floating newHX = path_state.HX;
            //                                for (int i = 0; i < MODEL_NUM; i++) {
            //                                    if (model_enabled[i]) {
            //                                        newHX += models[i].kf.X(v) * mix_ps[i];
            //                                    }
            //                                }
            //                                path_state_out.HX = newHX;
            //                    //            path_state_out.length = path_state.length + 1;
            //
            //                    // Delta H = weight[0][vk] * [[vs * SMALL_DIM_PER_SEG + 0]] + weight[1][vk] * [[vs * SMALL_DIM_PER_SEG + 1]]
            //                    floating weights_sum = 0;
            //                    for(int i=0; i < SMALL_DIM_PER_SEG; i++){
            //                        weights_sum += small_vec_weights[i][vk];
            //                    }
            //
            //                    path_state_out.HP = path_state.HP;
            //                    for (int k = 0; k < SMALL_DIM_PER_SEG; k++) {
            //                        path_state_out.HP += (small_vec_weights[k][vk] / weights_sum) * P[vs * SMALL_DIM_PER_SEG + k];
            //                    }
            //
            //                    floating HPPH = dot(path_state_out.HP, path_state_out.HP);
            //                    path_state_out.HPH = path_state.HPH;
            //                    for (int k = 0; k < SMALL_DIM_PER_SEG; k++) {
            //                        path_state_out.HPH += 2 * (small_vec_weights[k][vk] / weights_sum) * path_state.HP[vs * SMALL_DIM_PER_SEG + k];
            //                    }
            //                    for (int k1 = 0; k1 < SMALL_DIM_PER_SEG; k1++) {
            //                        for (int k2 = 0; k2 < SMALL_DIM_PER_SEG; k2++) {
            //                            path_state_out.HPH += (small_vec_weights[k1][vk] / weights_sum) * (small_vec_weights[k2][vk] / weights_sum) * P[vs * SMALL_DIM_PER_SEG + k1][vs * SMALL_DIM_PER_SEG + k2];
            //                        }
            //                    }
            //                    assert(path_state_out.HPH > 0);
            //                    floating info_gain = HPPH / (path_state_out.HPH + observe_sigma_per_c * newHX);
            //                    double total_cost = newHX - current_gamma * std::sqrt(info_gain);  // + path_state.length * 100000;
            //                                                                                       //            std::cerr << "!" << vs  << " " << std::sqrt(info_gain) << " " << path_state.length << " " << total_cost << "\n";
            //                    return total_cost;

            double cost = 0;
            for (int m = 0; m < MODEL_NUM; m++) {
                //                static_assert(MODEL_NUM == 8);
                if (is_mcmc_model(m)) {
                    continue;
                }
                if (model_enabled[m]) {
                    if (safety_stop_mode) {
                        cost += models[m].kf.X(v) * mix_ps[m];
                    } else {
                        cost += (models[m].kf.X(v) - current_gamma * std::sqrt(models[m].kf.var.Pdiag(v))) * mix_ps[m];
                    }
                }
            }
            if (safety_stop_mode) {
                cost += st.s[vs].mean_cost[vk] * mcmc_p;
            } else {
                cost += (st.s[vs].mean_cost[vk] - current_gamma * std::sqrt(st.s[vs].var_cost[vk])) * mcmc_p;
            }
            return path_state_out.HX = path_state.HX + cost;
        };

        //
        //#ifdef LOG
        //
        //        auto get_sigma = [&](int v) {
        //            floating c = 0;
        //            for (int i = 0; i < MODEL_NUM; i++) {
        //                if (model_enabled[i]) {
        //                    c += std::sqrt(models[i].var.Pdiag(v)) * mix_ps[i];
        //                }
        //            }
        //            return c;
        //        };
        //        log_stream << "0.0\n";
        //        for (int y = 0; y < 30; y++) {
        //            for (int x = 0; x < 29; x++) {
        //                int v = H(y, x);
        //                log_stream << get_cost(v) << " " << get_sigma(v) << " ";
        //            }
        //            log_stream << "\n";
        //        }
        //
        //        for (int y = 0; y < 29; y++) {
        //            for (int x = 0; x < 30; x++) {
        //                int v = V(y, x);
        //                log_stream << get_cost(v) << " " << get_sigma(v) << " ";
        //            }
        //            log_stream << "\n";
        //        }
        //#endif

        auto [d, str, path, path_state] = dijkstra_path_dependent(s, t, get_cost, PathState{});

        //#ifndef SILENT
        //
        //        floating HPPH = dot(path_state.HP, path_state.HP);
        //        floating info_gain = HPPH / (path_state.HPH + observe_sigma_per_c * path_state.HX);
        //        std::cerr << "HX: " << path_state.HX << " info_gain: " << std::sqrt(info_gain) << "\n";
        //
        //#endif
        //------------------------------------------
        //#ifdef LOG
        //        log_stream << a << " " << 1 << " " << 0 << "\n";
        //        log_stream << 0 << " " << 1 << "\n";
        //
        //
        //        log_stream << "0.0\n";
        //        for (int y = 0; y < 30; y++) {
        //            for (int x = 0; x < 29; x++) {
        //                log_stream << st.s[y].mean_cost[x] << " " << std::sqrt(st.s[y].var_cost[x]) << " ";
        //            }
        //            log_stream << "\n";
        //        }
        //
        //        for (int y = 0; y < 29; y++) {
        //            for (int x = 0; x < 30; x++) {
        //                log_stream << st.s[x + 30].mean_cost[y] << " " << std::sqrt(st.s[x + 30].var_cost[y]) << " ";
        //            }
        //            log_stream << "\n";
        //        }
        //#endif
        //
        //        floating current_gamma = gamma(a, provided_gamma_params);
        //        auto get_cost = [&](int i_seg, int i_elem) {
        //            return st.s[i_seg].mean_cost[i_elem] - current_gamma * std::sqrt(st.s[i_seg].var_cost[i_elem]);
        //        };
        //        auto [d, str, path] = dijkstra(
        //            s, t, [&](const Vertex& v) { return get_cost(v.x + 30, v.y); },
        //            [&](const Vertex& v) {
        //                return get_cost(v.y, v.x);
        //            });
        //---------------------------

        std::cout << str << std::endl;

        floating c;
        std::cin >> c;

#ifdef MAKE_TEST
        echo_stream << s.y << " " << s.x << " " << t.y << " " << t.x << " " << c << "\n";
#endif

#ifdef LOG
        log_stream << str << "\n"
                   << c << "\n";
#endif

        if (!safety_stop_mode) {
            SparseVec sp_obs_matrix{};
            HistoryEntry he{};
            double pred_mcmc = 0;
            auto add = [&](const Vertex& v1, const Vertex& v2) {
                if (v1.x == v2.x) {
                    int var = V(std::min(v1.y, v2.y), v1.x);
                    sp_obs_matrix.push_back({var, 1.0});
                    he.route.push_back(Edge{v1.x + 30, std::min(v1.y, v2.y)});
                    pred_mcmc += st.s[v1.x + 30].mean_cost[std::min(v1.y, v2.y)];
                } else if (v1.y == v2.y) {
                    int var = H(v1.y, std::min(v1.x, v2.x));
                    sp_obs_matrix.push_back({var, 1.0});
                    he.route.push_back(Edge{v1.y, std::min(v1.x, v2.x)});
                    pred_mcmc += st.s[v1.y].mean_cost[std::min(v1.x, v2.x)];
                } else {
                    assert(false);
                }
            };


            for (int i = 0; i < int(path.size()) - 1; i++) {
                add(path[i], path[i + 1]);
            }


#ifndef SILENT
            std::cerr << "expect: " << d << " gamma: " << current_gamma << " pred: " << pred_mcmc << " GT: " << c << " error: " << c - pred_mcmc << std::endl;
#endif
            floating r_mcmc = sq(c * observe_sigma_per_c) + he.route.size() * sq(observe_sigma_per_len);
            he.c = c;
            he.err = std::sqrt(r_mcmc);
            addHistory(st, std::move(he));

            if (mcmc_enabled) {
                int resample_n = suspended_mcmc_resamples + resample_schedule(a);

                if (mcmc_suspended) {
                    suspended_mcmc_resamples = std::min(resample_n, max_suspended_mcmc_resamples);
#ifndef SILENT
                    std::cerr << a << " " << resample_n << " (suspended)" << std::endl;
#endif
                } else {

#ifndef SILENT
                    std::cerr << a << " " << resample_n << std::endl;
#endif

                    for (int i = 0; i < NSEG; i++) {
                        st.s[i].mean_n *= 0.;
                    }
                    for (int b = 0; b < resample_n; b++) {
                        for (int v = 0; v < NSEG; v++) {
                            resample(st, v, initial_edge_sigma, initial_cost, rng);
                        }
                    }
                }
            }


#if defined(LOG) || !defined(SILENT)
            std::array<floating, MODEL_NUM> pred{};
            for (int i = 0; i < MODEL_NUM; i++) {
                if (model_enabled[i]) {
                    pred[i] = models[i].kf.predict(sp_obs_matrix);
                } else {
                    pred[i] = 0.0;
                }
            }
#endif
            floating r = sq(c * observe_sigma_per_c) + he.route.size() * sq(observe_sigma_per_len);

            std::array<floating, MODEL_NUM> logp{};
            for (int i = 0; i < MODEL_NUM; i++) {
                if (model_enabled[i] && !is_mcmc_only) {
                    logp[i] = models[i].kf.correct(sp_obs_matrix, c, r);
                } else {
                    logp[i] = -1e30;
                }
            }

#ifdef LOG
            for (int i = 0; i < MODEL_NUM; i++) {
                log_stream << pred[i] << " " << logp[i] << "\n";
            }
#endif
#ifndef SILENT
            for (int i = 0; i < MODEL_NUM; i++) {
                if (model_enabled[i]) {
                    std::cerr << "Model" << i << ": pred = " << pred[i] << " logp = " << logp[i] << "\n";
                }
            }
#endif
#ifdef BENCH_DETAIL
            if (a % 10 == 0) {
                std::cerr << a << " " << kalman_time << std::endl;
            }
#endif

#ifdef CLASSIFY
            if (a == 999) {
                std::cerr << "Best model: " << best_model << std::endl;
            }
#endif
            //#ifdef LOG
            //        log_stream << pred << " " << 0. << "\n";
            //
            //#endif
        }
        else{
            //safety_stop_mode
#ifdef LOG
            for (int i = 0; i < MODEL_NUM; i++) {
                log_stream << 0 << " " << 0 << "\n";
            }
#endif

#ifndef SILENT
            std::cerr << "Safety-stop!" << std::endl;
#endif
        }
    }
    BENCH_CODE(
        std::cerr << "kalman: " << kalman_time << " kalman2: " << kalman_time2 << " kalman3: " << kalman_time3 << " mcmc: " << mcmc_time << " dijkstra: " << dijkstra_time << std::endl;)

    return 0;
}
