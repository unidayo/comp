#!/bin/bash

set -eu

ROOT="/home/wsl2ubuntu/comp"
export UV_CACHE_DIR="${UV_CACHE_DIR:-$ROOT/.cache/uv}"
PYPY_VERSION="pypy-3.11.13"

uv venv \
  --clear \
  --python "$PYPY_VERSION" \
  --managed-python \
  "$ROOT/.venv-pypy"

uv pip install \
  --python "$ROOT/.venv-pypy/bin/python" \
  --requirements "$ROOT/requirements-pypy.txt"

"$ROOT/.venv-pypy/bin/python" -c \
  'import platform, sys; assert platform.python_implementation() == "PyPy"; print(sys.version)'
