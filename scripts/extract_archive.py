#!/usr/bin/env python3
from __future__ import annotations

import argparse
import tarfile
import zipfile
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser(description="Extract zip/tar archives without relying on unzip/tar commands.")
    parser.add_argument("archive")
    parser.add_argument("destination")
    args = parser.parse_args()

    archive = Path(args.archive)
    destination = Path(args.destination)
    destination.mkdir(parents=True, exist_ok=True)

    if zipfile.is_zipfile(archive):
        with zipfile.ZipFile(archive) as zf:
            zf.extractall(destination)
        return 0

    if tarfile.is_tarfile(archive):
        with tarfile.open(archive) as tf:
            tf.extractall(destination, filter="data")
        return 0

    raise SystemExit(f"unsupported archive format: {archive}")


if __name__ == "__main__":
    raise SystemExit(main())

