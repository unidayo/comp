#include <cuda_runtime.h>

#include <bits/stdc++.h>
using namespace std;

namespace {

#ifndef MOD_VALUE
#define MOD_VALUE 998244353
#endif

constexpr unsigned MOD = MOD_VALUE;
constexpr unsigned PRIMITIVE_ROOT = 3;
constexpr int THREADS = 256;
constexpr int LOCAL_LOG = 11;
constexpr int LOCAL_N = 1 << LOCAL_LOG;

#define CUDA_CHECK(expr)                                                       \
    do {                                                                       \
        cudaError_t err__ = (expr);                                            \
        if (err__ != cudaSuccess) {                                            \
            cerr << "CUDA error at " << __FILE__ << ':' << __LINE__ << ": "   \
                 << cudaGetErrorString(err__) << '\n';                        \
            exit(1);                                                           \
        }                                                                      \
    } while (false)

struct Timings {
    double host_prepare_ms = 0;
    float h2d_ms = 0;
    float kernel_ms = 0;
    float d2h_ms = 0;
};

unsigned mod_pow(unsigned a, long long e) {
    unsigned long long result = 1;
    unsigned long long base = a;
    while (e > 0) {
        if (e & 1) result = result * base % MOD;
        base = base * base % MOD;
        e >>= 1;
    }
    return static_cast<unsigned>(result);
}

__device__ __forceinline__ unsigned mod_add(unsigned a, unsigned b) {
    unsigned c = a + b;
    if (c >= MOD) c -= MOD;
    return c;
}

__device__ __forceinline__ unsigned mod_sub(unsigned a, unsigned b) {
    return a >= b ? a - b : a + MOD - b;
}

__device__ __forceinline__ unsigned mod_mul(unsigned a, unsigned b) {
    return static_cast<unsigned>(
        static_cast<unsigned long long>(a) * b % MOD);
}

__global__ void bit_reverse_kernel(unsigned *a, int n, int log_n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= n) return;
    unsigned rev = __brev(static_cast<unsigned>(i)) >> (32 - log_n);
    if (i < static_cast<int>(rev)) {
        unsigned tmp = a[i];
        a[i] = a[rev];
        a[rev] = tmp;
    }
}

__global__ void ntt_stage_kernel(unsigned *a, const unsigned *roots,
                                 int root_offset, int n, int len) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int butterfly_count = n >> 1;
    if (idx >= butterfly_count) return;

    int half = len >> 1;
    int j = idx % half;
    int base = (idx / half) * len;
    int left = base + j;
    int right = left + half;

    unsigned u = a[left];
    unsigned v = mod_mul(a[right], roots[root_offset + j]);
    a[left] = mod_add(u, v);
    a[right] = mod_sub(u, v);
}

__global__ void ntt_first_local_kernel(unsigned *a, const unsigned *roots,
                                       const int *offsets) {
    __shared__ unsigned s[LOCAL_N];

    int base_global = blockIdx.x * LOCAL_N;
    for (int i = threadIdx.x; i < LOCAL_N; i += blockDim.x) {
        s[i] = a[base_global + i];
    }
    __syncthreads();

    for (int stage = 0, len = 2; stage < LOCAL_LOG; ++stage, len <<= 1) {
        int half = len >> 1;
        int root_offset = offsets[stage];
        for (int idx = threadIdx.x; idx < LOCAL_N / 2; idx += blockDim.x) {
            int j = idx & (half - 1);
            int base = (idx >> stage) * len;
            int left = base + j;
            int right = left + half;

            unsigned u = s[left];
            unsigned v = mod_mul(s[right], roots[root_offset + j]);
            s[left] = mod_add(u, v);
            s[right] = mod_sub(u, v);
        }
        __syncthreads();
    }

    for (int i = threadIdx.x; i < LOCAL_N; i += blockDim.x) {
        a[base_global + i] = s[i];
    }
}

__global__ void pointwise_mul_kernel(unsigned *a, const unsigned *b, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= n) return;
    a[i] = mod_mul(a[i], b[i]);
}

__global__ void scale_kernel(unsigned *a, unsigned factor, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= n) return;
    a[i] = mod_mul(a[i], factor);
}

vector<unsigned> make_roots(int n, bool inverse, vector<int> &offsets) {
    vector<unsigned> roots;
    roots.reserve(n - 1);
    offsets.clear();

    for (int len = 2; len <= n; len <<= 1) {
        offsets.push_back(static_cast<int>(roots.size()));
        unsigned wlen = mod_pow(PRIMITIVE_ROOT, (MOD - 1) / len);
        if (inverse) wlen = mod_pow(wlen, MOD - 2);

        unsigned long long cur = 1;
        for (int j = 0; j < len / 2; ++j) {
            roots.push_back(static_cast<unsigned>(cur));
            cur = cur * wlen % MOD;
        }
    }
    return roots;
}

void run_ntt(unsigned *d_a, int n, int log_n, const unsigned *d_roots,
             const int *d_offsets,
             const vector<int> &offsets, bool inverse) {
    int blocks_n = (n + THREADS - 1) / THREADS;
    bit_reverse_kernel<<<blocks_n, THREADS>>>(d_a, n, log_n);
    CUDA_CHECK(cudaGetLastError());

    int stage = 0;
    int len = 2;
    if (n >= LOCAL_N) {
        ntt_first_local_kernel<<<n / LOCAL_N, THREADS>>>(d_a, d_roots,
                                                         d_offsets);
        CUDA_CHECK(cudaGetLastError());
        stage = LOCAL_LOG;
        len = LOCAL_N << 1;
    }

    for (; len <= n; len <<= 1, ++stage) {
        int butterfly_count = n >> 1;
        int blocks = (butterfly_count + THREADS - 1) / THREADS;
        ntt_stage_kernel<<<blocks, THREADS>>>(d_a, d_roots, offsets[stage], n,
                                              len);
        CUDA_CHECK(cudaGetLastError());
    }

    if (inverse) {
        unsigned inv_n = mod_pow(static_cast<unsigned>(n), MOD - 2);
        scale_kernel<<<blocks_n, THREADS>>>(d_a, inv_n, n);
        CUDA_CHECK(cudaGetLastError());
    }
}

vector<unsigned> convolution_gpu(const vector<unsigned> &a,
                                 const vector<unsigned> &b,
                                 Timings *timings = nullptr) {
    auto host_prepare_begin = chrono::steady_clock::now();
    int need = static_cast<int>(a.size() + b.size() - 1);
    int n = 1;
    int log_n = 0;
    while (n < need) {
        n <<= 1;
        ++log_n;
    }

    vector<unsigned> ha(n), hb(n);
    copy(a.begin(), a.end(), ha.begin());
    copy(b.begin(), b.end(), hb.begin());

    vector<int> forward_offsets;
    vector<int> inverse_offsets;
    vector<unsigned> forward_roots = make_roots(n, false, forward_offsets);
    vector<unsigned> inverse_roots = make_roots(n, true, inverse_offsets);
    auto host_prepare_end = chrono::steady_clock::now();
    if (timings) {
        timings->host_prepare_ms =
            chrono::duration<double, milli>(host_prepare_end - host_prepare_begin)
                .count();
    }

    unsigned *d_a = nullptr;
    unsigned *d_b = nullptr;
    unsigned *d_forward_roots = nullptr;
    unsigned *d_inverse_roots = nullptr;
    int *d_forward_offsets = nullptr;
    int *d_inverse_offsets = nullptr;

    CUDA_CHECK(cudaMalloc(&d_a, static_cast<size_t>(n) * sizeof(unsigned)));
    CUDA_CHECK(cudaMalloc(&d_b, static_cast<size_t>(n) * sizeof(unsigned)));
    CUDA_CHECK(cudaMalloc(&d_forward_roots,
                          forward_roots.size() * sizeof(unsigned)));
    CUDA_CHECK(cudaMalloc(&d_inverse_roots,
                          inverse_roots.size() * sizeof(unsigned)));
    CUDA_CHECK(cudaMalloc(&d_forward_offsets,
                          forward_offsets.size() * sizeof(int)));
    CUDA_CHECK(cudaMalloc(&d_inverse_offsets,
                          inverse_offsets.size() * sizeof(int)));

    cudaEvent_t ev0, ev1, ev2, ev3;
    if (timings) {
        CUDA_CHECK(cudaEventCreate(&ev0));
        CUDA_CHECK(cudaEventCreate(&ev1));
        CUDA_CHECK(cudaEventCreate(&ev2));
        CUDA_CHECK(cudaEventCreate(&ev3));
        CUDA_CHECK(cudaEventRecord(ev0));
    }

    CUDA_CHECK(cudaMemcpy(d_a, ha.data(), static_cast<size_t>(n) * sizeof(unsigned),
                          cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_b, hb.data(), static_cast<size_t>(n) * sizeof(unsigned),
                          cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_forward_roots, forward_roots.data(),
                          forward_roots.size() * sizeof(unsigned),
                          cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_inverse_roots, inverse_roots.data(),
                          inverse_roots.size() * sizeof(unsigned),
                          cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_forward_offsets, forward_offsets.data(),
                          forward_offsets.size() * sizeof(int),
                          cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_inverse_offsets, inverse_offsets.data(),
                          inverse_offsets.size() * sizeof(int),
                          cudaMemcpyHostToDevice));
    if (timings) {
        CUDA_CHECK(cudaEventRecord(ev1));
    }

    run_ntt(d_a, n, log_n, d_forward_roots, d_forward_offsets, forward_offsets,
            false);
    run_ntt(d_b, n, log_n, d_forward_roots, d_forward_offsets, forward_offsets,
            false);

    int blocks_n = (n + THREADS - 1) / THREADS;
    pointwise_mul_kernel<<<blocks_n, THREADS>>>(d_a, d_b, n);
    CUDA_CHECK(cudaGetLastError());

    run_ntt(d_a, n, log_n, d_inverse_roots, d_inverse_offsets, inverse_offsets,
            true);
    CUDA_CHECK(cudaDeviceSynchronize());
    if (timings) {
        CUDA_CHECK(cudaEventRecord(ev2));
    }

    vector<unsigned> result(n);
    CUDA_CHECK(cudaMemcpy(result.data(), d_a, static_cast<size_t>(n) * sizeof(unsigned),
                          cudaMemcpyDeviceToHost));
    if (timings) {
        CUDA_CHECK(cudaEventRecord(ev3));
        CUDA_CHECK(cudaEventSynchronize(ev3));
        CUDA_CHECK(cudaEventElapsedTime(&timings->h2d_ms, ev0, ev1));
        CUDA_CHECK(cudaEventElapsedTime(&timings->kernel_ms, ev1, ev2));
        CUDA_CHECK(cudaEventElapsedTime(&timings->d2h_ms, ev2, ev3));
        CUDA_CHECK(cudaEventDestroy(ev3));
        CUDA_CHECK(cudaEventDestroy(ev2));
        CUDA_CHECK(cudaEventDestroy(ev1));
        CUDA_CHECK(cudaEventDestroy(ev0));
    }

    CUDA_CHECK(cudaFree(d_inverse_offsets));
    CUDA_CHECK(cudaFree(d_forward_offsets));
    CUDA_CHECK(cudaFree(d_inverse_roots));
    CUDA_CHECK(cudaFree(d_forward_roots));
    CUDA_CHECK(cudaFree(d_b));
    CUDA_CHECK(cudaFree(d_a));

    result.resize(need);
    return result;
}

unsigned long long checksum_result(const vector<unsigned> &a) {
    unsigned long long h = 1469598103934665603ull;
    for (unsigned x : a) {
        h ^= x;
        h *= 1099511628211ull;
    }
    return h;
}

}  // namespace

int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    bool bench = false;
    bool checksum = false;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--bench") bench = true;
        else if (arg == "--checksum") checksum = true;
    }

    int n, m;
    cin >> n >> m;
    vector<unsigned> a(n), b(m);
    for (auto &x : a) cin >> x;
    for (auto &x : b) cin >> x;

    Timings timings;
    vector<unsigned> c = convolution_gpu(a, b, bench ? &timings : nullptr);
    if (bench) {
        cerr << fixed << setprecision(3)
             << "host_prepare_ms " << timings.host_prepare_ms << '\n'
             << "h2d_ms " << timings.h2d_ms << '\n'
             << "kernel_ms " << timings.kernel_ms << '\n'
             << "d2h_ms " << timings.d2h_ms << '\n';
    }
    if (checksum) {
        cout << checksum_result(c) << '\n';
        return 0;
    }
    for (int i = 0; i < static_cast<int>(c.size()); ++i) {
        if (i) cout << ' ';
        cout << c[i];
    }
    cout << '\n';
    return 0;
}
