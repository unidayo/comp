#!/usr/bin/env python3
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TEMPLATE_ROOT = ROOT / "templates"
ACC_CONFIG = ROOT / "acc-config"


def read_manifest(path: Path) -> list[str]:
    names: list[str] = []
    for line in path.read_text().splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        names.append(line)
    return names


def render_cpp() -> None:
    cpp_root = TEMPLATE_ROOT / "cpp"
    parts_dir = cpp_root / "parts"
    chunks: list[str] = []
    for name in read_manifest(cpp_root / "manifest.txt"):
        part_path = parts_dir / name
        text = part_path.read_text()
        if text and not text.endswith("\n"):
            text += "\n"
        chunks.append(text)

    dest_dir = ACC_CONFIG / "cpp"
    dest_dir.mkdir(parents=True, exist_ok=True)
    (dest_dir / "main.cpp").write_text("".join(chunks))


def render_pypy() -> None:
    pypy_root = TEMPLATE_ROOT / "pypy"
    dest_dir = ACC_CONFIG / "pypy"
    dest_dir.mkdir(parents=True, exist_ok=True)
    (dest_dir / "main.py").write_text((pypy_root / "main.py").read_text())


def main() -> None:
    render_cpp()
    render_pypy()


if __name__ == "__main__":
    main()
