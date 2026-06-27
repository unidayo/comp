#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
uv pip install --python .venv/bin/python -r requirements-optuna.txt

