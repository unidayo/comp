#include <cuda_runtime.h>

#include <bits/stdc++.h>
using namespace std;

namespace {

constexpr int VALUE_LIMIT = 1000;
constexpr int THREADS = 256;

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

__global__ void parity_kernel(const unsigned short *a,
                              const unsigned long long *masks,
                              unsigned long long *parity, int n, int m,
                              int words) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int total = n * words;
    if (idx >= total) return;

    int row = idx / words;
    int word = idx - row * words;
    unsigned long long x = 0;

    for (int col = 0; col < m; ++col) {
        int value = a[row * m + col];
        size_t mask_index =
            (static_cast<size_t>(col) * VALUE_LIMIT + value) * words + word;
        x ^= masks[mask_index];
    }

    parity[idx] = x;
}

__global__ void direct_pair_kernel(const unsigned short *a, int n, int m,
                                   unsigned long long *block_counts) {
    int tx = blockIdx.x * blockDim.x + threadIdx.x;
    int ty = blockIdx.y * blockDim.y + threadIdx.y;
    int tid = threadIdx.y * blockDim.x + threadIdx.x;

    unsigned long long local = 0;
    if (ty < tx && tx < n && ty < n) {
        int parity = 0;
        const unsigned short *row_i = a + static_cast<size_t>(ty) * m;
        const unsigned short *row_j = a + static_cast<size_t>(tx) * m;
        for (int col = 0; col < m; ++col) {
            parity ^= (row_i[col] == row_j[col]);
        }
        local = static_cast<unsigned long long>(parity);
    }

    __shared__ unsigned long long buf[THREADS];
    buf[tid] = local;
    __syncthreads();

    for (int offset = THREADS / 2; offset > 0; offset >>= 1) {
        if (tid < offset) buf[tid] += buf[tid + offset];
        __syncthreads();
    }

    if (tid == 0) {
        block_counts[blockIdx.y * gridDim.x + blockIdx.x] = buf[0];
    }
}

vector<unsigned long long> build_masks(const vector<unsigned short> &a, int n,
                                       int m, int words) {
    vector<unsigned long long> masks(
        static_cast<size_t>(m) * VALUE_LIMIT * words, 0);
    for (int col = 0; col < m; ++col) {
        for (int row = 0; row < n; ++row) {
            int value = a[row * m + col];
            size_t idx =
                (static_cast<size_t>(col) * VALUE_LIMIT + value) * words +
                (row >> 6);
            masks[idx] |= 1ull << (row & 63);
        }
    }
    return masks;
}

long long count_answer(const vector<unsigned long long> &parity, int n,
                       int words) {
    long long ans = 0;
    for (int i = 0; i < n; ++i) {
        const unsigned long long *row = parity.data() + static_cast<size_t>(i) * words;
        for (int word = (i + 1) >> 6; word < words; ++word) {
            unsigned long long bits = row[word];
            int base = word << 6;
            if (base <= i) {
                int remove = i - base + 1;
                if (remove >= 64) bits = 0;
                else bits &= ~((1ull << remove) - 1);
            }
            if (word == words - 1 && (n & 63)) {
                bits &= (1ull << (n & 63)) - 1;
            }
            ans += __builtin_popcountll(bits);
        }
    }
    return ans;
}

long long solve_gpu(const vector<unsigned short> &a, int n, int m,
                    Timings *timings) {
    int words = (n + 63) / 64;

    auto prepare_begin = chrono::steady_clock::now();
    vector<unsigned long long> masks = build_masks(a, n, m, words);
    vector<unsigned long long> parity(static_cast<size_t>(n) * words);
    auto prepare_end = chrono::steady_clock::now();
    if (timings) {
        timings->host_prepare_ms =
            chrono::duration<double, milli>(prepare_end - prepare_begin).count();
    }

    unsigned short *d_a = nullptr;
    unsigned long long *d_masks = nullptr;
    unsigned long long *d_parity = nullptr;
    size_t a_bytes = static_cast<size_t>(n) * m * sizeof(unsigned short);
    size_t masks_bytes = masks.size() * sizeof(unsigned long long);
    size_t parity_bytes = parity.size() * sizeof(unsigned long long);

    CUDA_CHECK(cudaMalloc(&d_a, a_bytes));
    CUDA_CHECK(cudaMalloc(&d_masks, masks_bytes));
    CUDA_CHECK(cudaMalloc(&d_parity, parity_bytes));

    cudaEvent_t ev0, ev1, ev2, ev3;
    if (timings) {
        CUDA_CHECK(cudaEventCreate(&ev0));
        CUDA_CHECK(cudaEventCreate(&ev1));
        CUDA_CHECK(cudaEventCreate(&ev2));
        CUDA_CHECK(cudaEventCreate(&ev3));
        CUDA_CHECK(cudaEventRecord(ev0));
    }

    CUDA_CHECK(cudaMemcpy(d_a, a.data(), a_bytes, cudaMemcpyHostToDevice));
    CUDA_CHECK(
        cudaMemcpy(d_masks, masks.data(), masks_bytes, cudaMemcpyHostToDevice));
    if (timings) CUDA_CHECK(cudaEventRecord(ev1));

    int total = n * words;
    int blocks = (total + THREADS - 1) / THREADS;
    parity_kernel<<<blocks, THREADS>>>(d_a, d_masks, d_parity, n, m, words);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());
    if (timings) CUDA_CHECK(cudaEventRecord(ev2));

    CUDA_CHECK(
        cudaMemcpy(parity.data(), d_parity, parity_bytes, cudaMemcpyDeviceToHost));
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

    CUDA_CHECK(cudaFree(d_parity));
    CUDA_CHECK(cudaFree(d_masks));
    CUDA_CHECK(cudaFree(d_a));

    return count_answer(parity, n, words);
}

long long solve_gpu_direct(const vector<unsigned short> &a, int n, int m,
                           Timings *timings) {
    auto prepare_begin = chrono::steady_clock::now();
    auto prepare_end = chrono::steady_clock::now();
    if (timings) {
        timings->host_prepare_ms =
            chrono::duration<double, milli>(prepare_end - prepare_begin).count();
    }

    unsigned short *d_a = nullptr;
    unsigned long long *d_counts = nullptr;
    size_t a_bytes = static_cast<size_t>(n) * m * sizeof(unsigned short);

    dim3 block(16, 16);
    dim3 grid((n + block.x - 1) / block.x, (n + block.y - 1) / block.y);
    size_t count_size =
        static_cast<size_t>(grid.x) * grid.y * sizeof(unsigned long long);
    vector<unsigned long long> counts(static_cast<size_t>(grid.x) * grid.y);

    CUDA_CHECK(cudaMalloc(&d_a, a_bytes));
    CUDA_CHECK(cudaMalloc(&d_counts, count_size));

    cudaEvent_t ev0, ev1, ev2, ev3;
    if (timings) {
        CUDA_CHECK(cudaEventCreate(&ev0));
        CUDA_CHECK(cudaEventCreate(&ev1));
        CUDA_CHECK(cudaEventCreate(&ev2));
        CUDA_CHECK(cudaEventCreate(&ev3));
        CUDA_CHECK(cudaEventRecord(ev0));
    }

    CUDA_CHECK(cudaMemcpy(d_a, a.data(), a_bytes, cudaMemcpyHostToDevice));
    if (timings) CUDA_CHECK(cudaEventRecord(ev1));

    direct_pair_kernel<<<grid, block>>>(d_a, n, m, d_counts);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());
    if (timings) CUDA_CHECK(cudaEventRecord(ev2));

    CUDA_CHECK(
        cudaMemcpy(counts.data(), d_counts, count_size, cudaMemcpyDeviceToHost));
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

    CUDA_CHECK(cudaFree(d_counts));
    CUDA_CHECK(cudaFree(d_a));

    unsigned long long ans = 0;
    for (unsigned long long x : counts) ans += x;
    return static_cast<long long>(ans);
}

}  // namespace

int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    bool bench = false;
    bool bitset_method = false;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--bench") bench = true;
        else if (arg == "--bitset") bitset_method = true;
    }

    int n, m;
    cin >> n >> m;
    vector<unsigned short> a(static_cast<size_t>(n) * m);
    for (auto &x : a) {
        int v;
        cin >> v;
        x = static_cast<unsigned short>(v);
    }

    Timings timings;
    long long ans = bitset_method ? solve_gpu(a, n, m, bench ? &timings : nullptr)
                                  : solve_gpu_direct(a, n, m, bench ? &timings : nullptr);
    if (bench) {
        cerr << fixed << setprecision(3)
             << "host_prepare_ms " << timings.host_prepare_ms << '\n'
             << "h2d_ms " << timings.h2d_ms << '\n'
             << "kernel_ms " << timings.kernel_ms << '\n'
             << "d2h_ms " << timings.d2h_ms << '\n';
    }
    cout << ans << '\n';
}
