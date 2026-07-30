#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

if ! command -v nvcc >/dev/null 2>&1; then
    echo "nvcc was not found. Install the CUDA Toolkit, then rerun this script." >&2
    exit 1
fi

nvcc -std=c++20 -O3 -lineinfo \
    -Xcompiler=-Wall,-Wextra \
    abc348_f_gpu.cu -o abc348_f_gpu

g++ -std=c++23 -O2 -Wall -Wextra abc348_f_cpu.cpp -o abc348_f_cpu
