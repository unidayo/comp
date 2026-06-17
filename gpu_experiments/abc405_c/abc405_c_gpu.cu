#include <cuda_runtime.h>

#include <bits/stdc++.h>
using namespace std;

namespace {

constexpr int TILE = 256;
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

__global__ void pair_sum_tiles(const int *a, int n, unsigned long long *partial,
                               int tile_count) {
    const int tx = blockIdx.x;
    const int ty = blockIdx.y;
    const int tid = threadIdx.x;
    const int partial_id = ty * tile_count + tx;

    if (tx < ty) {
        if (tid == 0) partial[partial_id] = 0;
        return;
    }

    const int row_base = ty * TILE;
    const int col_base = tx * TILE;
    unsigned long long sum = 0;

    for (int k = tid; k < TILE * TILE; k += blockDim.x) {
        const int di = k / TILE;
        const int dj = k - di * TILE;
        const int i = row_base + di;
        const int j = col_base + dj;

        if (i >= n || j >= n) continue;
        if (tx == ty && i >= j) continue;

        sum += static_cast<unsigned long long>(a[i]) *
               static_cast<unsigned long long>(a[j]);
    }

    __shared__ unsigned long long buf[THREADS];
    buf[tid] = sum;
    __syncthreads();

    for (int offset = THREADS / 2; offset > 0; offset >>= 1) {
        if (tid < offset) buf[tid] += buf[tid + offset];
        __syncthreads();
    }

    if (tid == 0) partial[partial_id] = buf[0];
}

void print_u128(unsigned __int128 x) {
    if (x == 0) {
        cout << 0 << '\n';
        return;
    }
    string s;
    while (x > 0) {
        s.push_back(static_cast<char>('0' + x % 10));
        x /= 10;
    }
    reverse(s.begin(), s.end());
    cout << s << '\n';
}

unsigned __int128 solve_gpu(const vector<int> &a) {
    const int n = static_cast<int>(a.size());
    const int tile_count = (n + TILE - 1) / TILE;
    const size_t partial_size =
        static_cast<size_t>(tile_count) * tile_count * sizeof(unsigned long long);

    int *d_a = nullptr;
    unsigned long long *d_partial = nullptr;
    CUDA_CHECK(cudaMalloc(&d_a, static_cast<size_t>(n) * sizeof(int)));
    CUDA_CHECK(cudaMalloc(&d_partial, partial_size));
    CUDA_CHECK(cudaMemcpy(d_a, a.data(), static_cast<size_t>(n) * sizeof(int),
                          cudaMemcpyHostToDevice));

    dim3 grid(tile_count, tile_count);
    pair_sum_tiles<<<grid, THREADS>>>(d_a, n, d_partial, tile_count);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    vector<unsigned long long> partial(static_cast<size_t>(tile_count) *
                                       tile_count);
    CUDA_CHECK(cudaMemcpy(partial.data(), d_partial, partial_size,
                          cudaMemcpyDeviceToHost));

    CUDA_CHECK(cudaFree(d_partial));
    CUDA_CHECK(cudaFree(d_a));

    unsigned __int128 ans = 0;
    for (unsigned long long x : partial) ans += x;
    return ans;
}

}  // namespace

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<int> a(n);
    for (int &x : a) cin >> x;

    print_u128(solve_gpu(a));
    return 0;
}
