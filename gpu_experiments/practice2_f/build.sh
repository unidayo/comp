#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

if ! command -v nvcc >/dev/null 2>&1; then
    echo "nvcc was not found. Install the CUDA Toolkit, then rerun this script." >&2
    exit 1
fi

nvcc -std=c++20 -O3 -lineinfo \
    -Xcompiler=-Wall,-Wextra \
    practice2_f_gpu.cu -o practice2_f_gpu

nvcc -std=c++20 -O3 -lineinfo -DMOD_VALUE=469762049 \
    -Xcompiler=-Wall,-Wextra \
    practice2_f_gpu.cu -o practice2_f_gpu_469

g++ -std=c++23 -O2 -Wall -Wextra -I/usr/lib/ac-library \
    practice2_f_cpu.cpp -o practice2_f_cpu

g++ -std=c++23 -O2 -Wall -Wextra -I/usr/lib/ac-library \
    -DMOD_VALUE=469762049 practice2_f_cpu.cpp -o practice2_f_cpu_469
