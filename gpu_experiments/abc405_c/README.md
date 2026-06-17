# ABC405 C GPU experiment

Problem: https://atcoder.jp/contests/abc405/tasks/abc405_c

The intended solution is O(N), but this directory is for intentionally doing
the pair enumeration on an NVIDIA GPU in O(N^2).

## Files

- `abc405_c_gpu.cu`: CUDA implementation.
- `abc405_c_cpu.cpp`: normal O(N) implementation for answer checking.
- `build.sh`: builds both binaries when `nvcc` is available.
- `run_samples.sh`: runs the three official samples against the GPU binary.
- `check_random.py`: compares CPU and GPU outputs on random small cases.
- `gen_max.py`: generates a large benchmark input.

## Current environment note

`nvidia-smi` sees an RTX 4060 under WSL when run with GPU access. CUDA Toolkit
12.0 is installed and `build.sh` succeeds in this environment.

In the Codex sandbox, GPU execution needs elevated GPU access. Running the
binaries directly from a normal terminal should not need anything special.

```bash
cd gpu_experiments/abc405_c
./build.sh
./run_samples.sh
./check_random.py 100 2000
./gen_max.py 300000 0 > max.in
time ./abc405_c_gpu < max.in
```

Verified locally:

- official samples: OK
- `./check_random.py 20 1000`: OK
- `N = 20000` GPU benchmark: about 0.38 sec, output matched CPU

## CUDA approach

The kernel divides the upper-triangular pair space into `256 x 256` tiles. One
CUDA block computes one tile and writes a 64-bit partial sum. The host copies
the partial sums back and reduces them.

For `N = 300000`, the number of pairs is about `4.5e10`, so this remains a very
large brute-force computation. The partial sum array is modest: about
`ceil(N/256)^2 * 8`, roughly 11 MiB at maximum `N`.

For the original ABC405 C constraints, the answer fits in signed 64-bit:

```text
C(300000, 2) * 10000^2 = 4,499,985,000,000,000,000
```

The local experiment code prints the final answer via `__int128`, so larger
benchmark inputs such as `N = 1000000` can still be displayed when the total
exceeds signed 64-bit. Each tile partial sum is still 64-bit, which is enough
for these tile sizes.
