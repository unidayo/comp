#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys
import tempfile


def main() -> None:
    seed = int(sys.argv[1]) if len(sys.argv) >= 2 else 0
    root = Path(__file__).resolve().parents[1]
    official = root / "vis" / "tools"

    pregenerated = official / "in" / f"{seed:04}.txt"
    if 0 <= seed < 100 and pregenerated.exists():
        sys.stdout.write(pregenerated.read_text(encoding="utf-8"))
        return

    tmp_root = root / "tmp_gen"
    tmp_root.mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(dir=tmp_root) as td:
        tmp = Path(td)
        seeds = tmp / "seeds.txt"
        seeds.write_text(f"{seed}\n", encoding="utf-8")

        subprocess.run(
            ["cargo", "run", "-r", "--bin", "gen", str(seeds), "--dir", str(tmp)],
            cwd=official,
            check=True,
        )
        sys.stdout.write((tmp / "0000.txt").read_text(encoding="utf-8"))


if __name__ == "__main__":
    main()
