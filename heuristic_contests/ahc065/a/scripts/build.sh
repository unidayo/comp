#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
mkdir -p bin
g++ -std=gnu++23 -O2 -pipe -Wall -Wextra -DLOCAL ${CXXFLAGS:-} main.cpp -o bin/main
