#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

./abc405_c_gpu << 'EOF'
3
4 2 3
EOF

./abc405_c_gpu << 'EOF'
2
9 45
EOF

./abc405_c_gpu << 'EOF'
10
7781 8803 8630 9065 8831 9182 8593 7660 7548 8617
EOF
