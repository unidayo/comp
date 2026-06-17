# ABC194 C GPU experiment

Problem: https://atcoder.jp/contests/abc194/tasks/abc194_c

The intended solution is O(N), but this directory intentionally computes all
pairs on an NVIDIA GPU in O(N^2).

## Files

- `abc194_c_gpu.cu`: CUDA implementation.
- `abc194_c_cpu.cpp`: normal O(N) implementation for answer checking.
- `build.sh`: builds both binaries.
- `run_samples.sh`: runs the official samples against the GPU binary.
- `check_random.py`: compares CPU and GPU outputs on random small cases.
- `gen_max.py`: generates benchmark input.

## Usage

```bash
cd gpu_experiments/abc194_c
./build.sh
./run_samples.sh
./check_random.py 100 2000
./gen_max.py 300000 0 > max.in
time ./abc194_c_gpu < max.in
```

In the Codex sandbox, GPU execution needs elevated GPU access. Running the
binaries directly from a normal terminal should not need anything special.

## CUDA approach

The kernel divides the upper-triangular pair space into `256 x 256` tiles. One
CUDA block computes one tile and writes a 64-bit partial sum. The host copies
the partial sums back and reduces them with `__int128`.

For `N = 300000`, the pair count is about `4.5e10`, and the partial sum array is
about `ceil(N/256)^2 * 8`, roughly 11 MiB.

Verified locally:

- official samples: OK
- `./check_random.py 20 1000`: OK
- `N = 300000` GPU benchmark: about 0.56 sec, output matched CPU
