#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

./abc194_c_gpu << 'EOF'
3
2 8 4
EOF

./abc194_c_gpu << 'EOF'
5
-5 8 9 -4 -3
EOF
