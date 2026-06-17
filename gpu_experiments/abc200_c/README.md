# ABC200 C GPU experiment

Problem: https://atcoder.jp/contests/abc200/tasks/abc200_c

The intended solution is O(N) with 200 residue counters. This directory
intentionally computes all pairs on an NVIDIA GPU in O(N^2).

## Files

- `abc200_c_gpu.cu`: CUDA implementation.
- `abc200_c_cpu.cpp`: normal O(N) implementation for answer checking.
- `build.sh`: builds both binaries.
- `run_samples.sh`: runs the official samples against the GPU binary.
- `check_random.py`: compares CPU and GPU outputs on random small cases.
- `gen_max.py`: generates benchmark input.

## Usage

```bash
cd gpu_experiments/abc200_c
./build.sh
./run_samples.sh
./check_random.py 100 2000
./gen_max.py 200000 0 > max.in
time ./abc200_c_gpu < max.in
```

In the Codex sandbox, GPU execution needs elevated GPU access. Running the
binaries directly from a normal terminal should not need anything special.

## CUDA approach

The host first replaces each `A_i` with `A_i % 200`. The kernel divides the
upper-triangular pair space into `256 x 256` tiles. One CUDA block computes one
tile and counts pairs whose residues are equal.

For `N = 200000`, the pair count is about `2.0e10`, and the partial sum array is
about `ceil(N/256)^2 * 8`, roughly 4.9 MiB.

Verified locally:

- official samples: OK
- `./check_random.py 20 1000`: OK
- `N = 200000` GPU benchmark: about 0.49 sec, output matched CPU
