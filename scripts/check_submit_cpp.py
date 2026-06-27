#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


QUOTED_INCLUDE_RE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.MULTILINE)


def main() -> int:
    parser = argparse.ArgumentParser(description="Check that a C++ submission file is self-contained for AtCoder.")
    parser.add_argument("file")
    args = parser.parse_args()

    path = Path(args.file)
    text = path.read_text(encoding="utf-8")
    errors = [f'quoted include is not self-contained: #include "{m.group(1)}"' for m in QUOTED_INCLUDE_RE.finditer(text)]

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

