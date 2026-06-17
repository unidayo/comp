#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

if ! command -v nvcc >/dev/null 2>&1; then
    echo "nvcc was not found. Install the CUDA Toolkit, then rerun this script." >&2
    exit 1
fi

nvcc -std=c++20 -O3 --use_fast_math -lineinfo \
    -Xcompiler=-Wall,-Wextra \
    abc405_c_gpu.cu -o abc405_c_gpu

g++ -std=c++23 -O2 -Wall -Wextra abc405_c_cpu.cpp -o abc405_c_cpu
