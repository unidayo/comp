# Practice2 F GPU experiment

Problem: https://atcoder.jp/contests/practice2/tasks/practice2_f

AtCoder's normal judge does not provide CUDA, so this is a local NVIDIA GPU
experiment rather than a submit-ready AtCoder solution.

## Files

- `practice2_f_gpu.cu`: CUDA NTT implementation modulo `998244353`.
- `practice2_f_cpu.cpp`: ACL `atcoder::convolution` implementation for
  answer checking and CPU timing.
- `build.sh`: builds the default `998244353` binaries and the
  `469762049` variants suffixed with `_469`.
- `run_samples.sh`: checks the official samples against the GPU binary.
- `check_random.py`: compares CPU and GPU outputs on random cases.
- `gen_max.py`: generates a maximum-size benchmark input.

## Usage

```bash
cd gpu_experiments/practice2_f
./build.sh
./run_samples.sh
./check_random.py 100 512
./gen_max.py 524288 524288 0 > max.in
time ./practice2_f_gpu < max.in > max.out
./practice2_f_gpu --bench --checksum < max.in

./gen_max.py 8388608 8388608 147 469762049 > large469.in
./practice2_f_cpu_469 --checksum < large469.in
./practice2_f_gpu_469 --bench --checksum < large469.in
```

In the Codex sandbox, GPU execution needs elevated GPU access. Running the
binaries directly from a normal terminal should not need anything special.

## CUDA Approach

The implementation uses the standard iterative NTT over `998244353`, whose
primitive root is `3`.

1. Pad both arrays to the next power of two.
2. Copy them to GPU memory.
3. Run forward NTT for both arrays.
4. Multiply pointwise on the GPU.
5. Run inverse NTT and multiply by `n^{-1}`.
6. Copy back the first `N + M - 1` coefficients.

The first 11 NTT stages, from length `2` through `2048`, are fused into one
shared-memory kernel per 2048-element block. Larger stages are launched as one
CUDA kernel over `n / 2` butterflies. Twiddle factors are precomputed on the
host once per direction and copied to GPU memory.

## Benchmark

Measured in this WSL/CUDA environment after `./build.sh`, with output redirected
to `/tmp/*.out`.

```text
N = M = 262144
CPU ACL: real 0.05 user 0.04 sys 0.01 maxrss_kb 10392
GPU NTT before stage fusion:
         real 0.34 user 0.06 sys 0.04 maxrss_kb 121232
GPU NTT after 11-stage shared-memory fusion:
         real 0.32 user 0.04 sys 0.06 maxrss_kb 121304

N = M = 524288
CPU ACL: real 0.10 user 0.09 sys 0.01 maxrss_kb 17556
GPU NTT before stage fusion:
         real 0.41 user 0.11 sys 0.04 maxrss_kb 133152
GPU NTT after 11-stage shared-memory fusion:
         real 0.32 user 0.05 sys 0.06 maxrss_kb 135768
         rerun: real 0.35 user 0.08 sys 0.06 maxrss_kb 133448
```

The CPU and GPU outputs matched by `sha256sum` for both benchmark inputs after
the optimization. The CUDA implementation is still slower than ACL here, but
fusing the small NTT stages reduced the maximum-size case from about `0.41s` to
about `0.32-0.35s` by avoiding repeated global-memory passes for the early
stages.

For `N = M = 524288`, `--bench --checksum` reported:

```text
host_prepare_ms 11.962
h2d_ms 2.256
kernel_ms 3.230
d2h_ms 2.347
real 0.36 user 0.07 sys 0.03 maxrss_kb 133572
```

So the GPU transfer and kernel work is under 10 ms, but the one-shot process is
dominated by CUDA context/runtime overhead and allocations. In this environment,
that fixed cost prevents a single AtCoder-style invocation from beating ACL's
CPU implementation.

### Larger-than-Problem Sizes

`998244353 - 1 = 119 * 2^23`, so this NTT can only use transform lengths up to
`2^23`. For equal lengths, the largest valid case is therefore
`N = M = 4194304`, because `N + M - 1 = 8388607` pads to `2^23`.

Measured with `--checksum` to avoid huge answer output:

```text
N = M = 1048576  (NTT length 2^21)
CPU ACL: real 0.15 user 0.15 sys 0.00 maxrss_kb 31736
GPU NTT: real 0.44 user 0.10 sys 0.07 maxrss_kb 160288
         host_prepare_ms 20.514, h2d_ms 4.852, kernel_ms 4.618, d2h_ms 4.872

N = M = 2097152  (NTT length 2^22)
CPU ACL: real 0.30 user 0.27 sys 0.03 maxrss_kb 60404
GPU NTT: real 0.52 user 0.18 sys 0.07 maxrss_kb 209476
         host_prepare_ms 34.283, h2d_ms 6.928, kernel_ms 7.613, d2h_ms 7.310

N = M = 4194304  (NTT length 2^23)
CPU ACL: real 0.63 user 0.57 sys 0.06 maxrss_kb 117960
GPU NTT: real 0.69 user 0.34 sys 0.11 maxrss_kb 305576
         host_prepare_ms 71.389, h2d_ms 13.865, kernel_ms 21.357, d2h_ms 13.664
```

The gap narrows as `N` grows, but CPU ACL is still slightly faster at the
maximum NTT length in this one-shot benchmark. Above this size, a single
`998244353` NTT is not valid; supporting larger convolutions would require
splitting, multiple moduli/CRT, or a different convolution method.

### Mod `469762049`

`469762049 - 1 = 7 * 2^26`, and ACL's primitive root table uses root `3` for
this prime. Building with `./build.sh` also creates:

```text
practice2_f_cpu_469
practice2_f_gpu_469
```

This allows a single NTT length up to `2^26`. For equal lengths, the largest
valid case is `N = M = 33554432`.

Measured with `--checksum`:

```text
N = M = 4194304  (NTT length 2^23)
CPU ACL 469: real 0.72 user 0.69 sys 0.03 maxrss_kb 118032
GPU NTT 469: real 0.77 user 0.39 sys 0.11 maxrss_kb 305620
             host_prepare_ms 88.764, h2d_ms 18.051, kernel_ms 21.786, d2h_ms 17.011

N = M = 8388608  (NTT length 2^24)
CPU ACL 469: real 1.40 user 1.30 sys 0.09 maxrss_kb 232680
GPU NTT 469: real 1.22 user 0.76 sys 0.16 maxrss_kb 502176
             host_prepare_ms 182.794, h2d_ms 29.429, kernel_ms 52.598, d2h_ms 31.235

N = M = 16777216  (NTT length 2^25)
CPU ACL 469: real 2.99 user 2.68 sys 0.30 maxrss_kb 461944
GPU NTT 469: real 2.17 user 1.49 sys 0.39 maxrss_kb 897892
             host_prepare_ms 390.382, h2d_ms 82.881, kernel_ms 110.112, d2h_ms 69.239

N = M = 33554432  (NTT length 2^26)
CPU ACL 469: real 5.94 user 5.39 sys 0.54 maxrss_kb 920940
GPU NTT 469: real 4.07 user 3.02 sys 0.69 maxrss_kb 1684188
             host_prepare_ms 777.371, h2d_ms 196.100, kernel_ms 226.327, d2h_ms 139.097
```

The CPU and GPU checksums matched for every measured `469762049` case. With
larger transform lengths, the GPU fixed cost is amortized and the GPU
implementation becomes faster than ACL's CPU implementation in this environment.
