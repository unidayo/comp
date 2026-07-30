#!/usr/bin/env python3
"""ACLヘッダを、公式のinclude guardを保ったまま提出用に展開する。"""

from __future__ import annotations

import argparse
import datetime
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
ACL_ROOT = Path("/usr/lib/ac-library/atcoder")
OUTPUT_DIR = ROOT / "mylib/acl"
LOCAL_INCLUDE = re.compile(r'^\s*#\s*include\s*[<"]atcoder/([^>"]+)[>"]\s*$')
TARGETS = {
    "convolution": "convolution.hpp",
    "dsu": "dsu.hpp",
    "fenwicktree": "fenwicktree.hpp",
    "lazysegtree": "lazysegtree.hpp",
    "math": "math.hpp",
    "maxflow": "maxflow.hpp",
    "mincostflow": "mincostflow.hpp",
    "modint": "modint.hpp",
    "scc": "scc.hpp",
    "segtree": "segtree.hpp",
    "string": "string.hpp",
    "twosat": "twosat.hpp",
}


def expand(path: Path) -> str:
    result: list[str] = []
    for line in path.read_text().splitlines():
        match = LOCAL_INCLUDE.match(line)
        if match:
            dependency = ACL_ROOT / match.group(1)
            if not dependency.exists():
                raise FileNotFoundError(dependency)
            result.append(expand(dependency))
        else:
            result.append(line.rstrip())
    return "\n".join(result)


def modint_io() -> str:
    return r'''
#ifndef MYLIB_ACL_MODINT_IO
#define MYLIB_ACL_MODINT_IO
namespace atcoder {
template <int m>
std::istream &operator>>(std::istream &is, atcoder::static_modint<m> &x) {
    long long value;
    is >> value;
    x = value;
    return is;
}
template <int m>
std::ostream &operator<<(std::ostream &os, const atcoder::static_modint<m> &x) {
    return os << x.val();
}
template <int id>
std::istream &operator>>(std::istream &is, atcoder::dynamic_modint<id> &x) {
    long long value;
    is >> value;
    x = value;
    return is;
}
template <int id>
std::ostream &operator<<(std::ostream &os, const atcoder::dynamic_modint<id> &x) {
    return os << x.val();
}
}  // namespace atcoder
#endif
'''.strip()


def render(name: str, timestamp: str) -> None:
    body = expand(ACL_ROOT / TARGETS[name])
    extra = "\n" + modint_io() if name == "modint" else ""
    output = f'''// 最終更新: {timestamp}
// ACL: {name}
// 公式ACLのinclude guardを保持しているため、他の展開済みACL部品と重複して貼ってもよい。
// clang-format off
#ifndef ATCODER
{body}
using namespace atcoder;{extra}
#endif
// clang-format on
'''
    (OUTPUT_DIR / f"{name}.cpp").write_text(output)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("targets", nargs="*")
    args = parser.parse_args()
    timestamp = datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=9))).strftime("%Y-%m-%d %H:%M:%S JST")
    targets = args.targets or TARGETS
    unknown = set(targets) - TARGETS.keys()
    if unknown:
        parser.error(f"unknown targets: {', '.join(sorted(unknown))}")
    for name in targets:
        render(name, timestamp)


if __name__ == "__main__":
    main()
