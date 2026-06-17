#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

./abc206_c_gpu << 'EOF'
3
1 7 1
EOF

./abc206_c_gpu << 'EOF'
10
1 10 100 1000 10000 100000 1000000 10000000 100000000 1000000000
EOF

./abc206_c_gpu << 'EOF'
20
7 8 1 1 4 9 9 6 8 2 4 1 1 9 5 5 5 3 6 4
EOF
