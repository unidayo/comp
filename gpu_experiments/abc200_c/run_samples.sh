#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

./abc200_c_gpu << 'EOF'
6
123 223 123 523 200 2000
EOF

./abc200_c_gpu << 'EOF'
5
1 2 3 4 5
EOF

./abc200_c_gpu << 'EOF'
8
199 100 200 400 300 500 600 200
EOF
