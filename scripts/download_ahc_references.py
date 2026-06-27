#!/usr/bin/env python3
"""Download AHC statements and the final submissions of top-ranked users."""

from __future__ import annotations

import argparse
import hashlib
import html
from html.parser import HTMLParser
import http.cookiejar
import json
from pathlib import Path
import re
import sys
import time
from urllib.parse import urljoin

import requests


BASE_URL = "https://atcoder.jp"
DEFAULT_COOKIE_JAR = Path.home() / ".local/share/online-judge-tools/cookie.jar"
USER_AGENT = "comp-ahc-reference-downloader/1.0 (personal archival use)"


class LinkParser(HTMLParser):
    def __init__(self) -> None:
        super().__init__()
        self.links: list[tuple[str, str]] = []
        self._href: str | None = None
        self._text: list[str] = []

    def handle_starttag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
        if tag == "a":
            self._href = dict(attrs).get("href")
            self._text = []

    def handle_data(self, data: str) -> None:
        if self._href is not None:
            self._text.append(data)

    def handle_endtag(self, tag: str) -> None:
        if tag == "a" and self._href is not None:
            self.links.append((self._href, "".join(self._text).strip()))
            self._href = None
            self._text = []


class StatementMarkdownParser(HTMLParser):
    """Small, dependency-free converter for AtCoder's Japanese statement span."""

    def __init__(self) -> None:
        super().__init__(convert_charrefs=True)
        self.output: list[str] = []
        self.capture_depth: int | None = None
        self.depth = 0
        self.in_pre = False
        self.link_stack: list[str | None] = []

    def emit(self, text: str) -> None:
        if self.capture_depth is not None:
            self.output.append(text)

    def handle_starttag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
        attr = dict(attrs)
        self.depth += 1
        classes = (attr.get("class") or "").split()
        if self.capture_depth is None and tag == "span" and "lang-ja" in classes:
            self.capture_depth = self.depth
            return
        if self.capture_depth is None:
            return
        if re.fullmatch(r"h[1-6]", tag):
            self.emit("\n\n" + "#" * int(tag[1]) + " ")
        elif tag == "p":
            self.emit("\n\n")
        elif tag == "br":
            self.emit("\n")
        elif tag == "li":
            self.emit("\n- ")
        elif tag == "pre":
            self.in_pre = True
            self.emit("\n\n```text\n")
        elif tag == "code" and not self.in_pre:
            self.emit("`")
        elif tag in ("strong", "b"):
            self.emit("**")
        elif tag in ("em", "i"):
            self.emit("*")
        elif tag == "a":
            self.emit("[")
            self.link_stack.append(attr.get("href"))
        elif tag == "img":
            src = attr.get("src")
            if src:
                self.emit(f"![{attr.get('alt') or ''}]({urljoin(BASE_URL, src)})")
        elif tag in ("td", "th"):
            self.emit(" | ")
        elif tag == "tr":
            self.emit("\n")

    def handle_endtag(self, tag: str) -> None:
        if self.capture_depth is not None:
            if tag == "pre":
                self.emit("\n```\n")
                self.in_pre = False
            elif tag == "code" and not self.in_pre:
                self.emit("`")
            elif tag in ("strong", "b"):
                self.emit("**")
            elif tag in ("em", "i"):
                self.emit("*")
            elif tag == "a":
                href = self.link_stack.pop() if self.link_stack else None
                self.emit(f"]({urljoin(BASE_URL, href)})" if href else "]")
        if self.capture_depth == self.depth:
            self.capture_depth = None
        self.depth -= 1

    def handle_data(self, data: str) -> None:
        if self.capture_depth is not None:
            self.emit(data if self.in_pre else re.sub(r"[ \t\r\f\v]+", " ", data))

    def markdown(self) -> str:
        text = "".join(self.output)
        text = re.sub(r" *\n *\n(?: *\n)+", "\n\n", text)
        return text.strip() + "\n"


def atomic_write(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(path.suffix + ".tmp")
    tmp.write_bytes(data)
    tmp.replace(path)


def load_session(cookie_jar: Path) -> requests.Session:
    if not cookie_jar.exists():
        raise RuntimeError(f"AtCoder cookie jar not found: {cookie_jar}")
    jar = http.cookiejar.LWPCookieJar(str(cookie_jar))
    jar.load(ignore_discard=True, ignore_expires=True)
    session = requests.Session()
    session.cookies.update(jar)
    session.headers["User-Agent"] = USER_AGENT
    return session


def fetch(
    session: requests.Session,
    url: str,
    *,
    expect_json: bool = False,
    allow_not_found: bool = False,
) -> requests.Response:
    last_error: Exception | None = None
    for attempt in range(5):
        try:
            response = session.get(url, timeout=45)
            if allow_not_found and response.status_code == 404:
                time.sleep(0.35)
                return response
            response.raise_for_status()
            if "/login" in response.url:
                raise RuntimeError(f"AtCoder session is not authenticated: {url}")
            if expect_json and "json" not in response.headers.get("Content-Type", ""):
                raise RuntimeError(f"Expected JSON but received {response.headers.get('Content-Type')}: {url}")
            time.sleep(0.35)
            return response
        except (requests.RequestException, RuntimeError) as error:
            last_error = error
            if attempt == 4:
                break
            time.sleep(2**attempt)
    raise RuntimeError(f"Failed to download {url}: {last_error}")


def task_links(tasks_html: str, contest_id: str) -> list[tuple[str, str]]:
    parser = LinkParser()
    parser.feed(tasks_html)
    prefix = f"/contests/{contest_id}/tasks/"
    result: list[tuple[str, str]] = []
    seen: set[str] = set()
    system_test_links = {
        href for href, title in parser.links
        if href.startswith(prefix) and ("Final" in title or "System Test" in title)
    }
    for href, title in parser.links:
        if href.startswith(prefix) and href not in seen and href not in system_test_links:
            result.append((urljoin(BASE_URL, href), title))
            seen.add(href)
    return result


def extract_submission(page: str) -> tuple[str, str, str]:
    code_match = re.search(r'<pre id="submission-code"[^>]*>(.*?)</pre>', page, re.S)
    if not code_match:
        raise RuntimeError("submission-code element was not found")
    code = html.unescape(code_match.group(1)).replace("\r\n", "\n")
    language_match = re.search(r'<th>(?:Language|言語)</th>\s*<td[^>]*>(.*?)</td>', page, re.S)
    language = html.unescape(re.sub(r"<[^>]+>", "", language_match.group(1))).strip() if language_match else "Unknown"
    ace_match = re.search(r'id="submission-code"[^>]*data-ace-mode="([^"]+)"', page)
    ace_mode = ace_match.group(1) if ace_match else ""
    return code, language, ace_mode


def extension_for(language: str, ace_mode: str) -> str:
    lower = language.lower()
    rules = [
        (("c++", "c_cpp"), ".cpp"),
        (("rust", "rust"), ".rs"),
        (("python", "python", "pypy"), ".py"),
        (("java", "java"), ".java"),
        (("kotlin", "kotlin"), ".kt"),
        (("c#", "csharp"), ".cs"),
        (("go ", "golang"), ".go"),
        (("javascript", "node.js"), ".js"),
        (("typescript",), ".ts"),
        (("swift",), ".swift"),
        (("ruby",), ".rb"),
        (("crystal",), ".cr"),
        (("nim",), ".nim"),
        (("julia",), ".jl"),
        (("haskell",), ".hs"),
        (("ocaml",), ".ml"),
        (("scala",), ".scala"),
        (("php",), ".php"),
        (("perl",), ".pl"),
        (("d (",), ".d"),
        (("c (",), ".c"),
    ]
    haystack = lower + " " + ace_mode.lower()
    for needles, extension in rules:
        if any(needle in haystack for needle in needles):
            return extension
    return ".txt"


def safe_name(name: str) -> str:
    return re.sub(r"[^A-Za-z0-9_.-]+", "_", name).strip("._") or "user"


def download_contest(session: requests.Session, contest_id: str, destination: Path, refresh: bool) -> dict[str, object]:
    contest_dir = destination / contest_id
    statement_dir = contest_dir / "statement"
    submissions_dir = contest_dir / "submissions"
    statement_dir.mkdir(parents=True, exist_ok=True)
    submissions_dir.mkdir(parents=True, exist_ok=True)
    previous_languages: dict[int, str] = {}
    previous_metadata_path = contest_dir / "metadata.json"
    if previous_metadata_path.exists():
        try:
            previous_metadata = json.loads(previous_metadata_path.read_text())
            for previous_row in previous_metadata.get("top10", []):
                for previous_submission in previous_row.get("submissions", []):
                    previous_languages[int(previous_submission["submission_id"])] = previous_submission.get("language", "Unknown")
        except (OSError, ValueError, KeyError, TypeError):
            pass

    tasks_url = f"{BASE_URL}/contests/{contest_id}/tasks?lang=ja"
    tasks_response = fetch(session, tasks_url)
    atomic_write(statement_dir / "tasks.html", tasks_response.content)
    tasks = task_links(tasks_response.text, contest_id)
    if not tasks:
        raise RuntimeError(f"No tasks found for {contest_id}")

    task_metadata: list[dict[str, str]] = []
    for task_url, task_title in tasks:
        task_id = task_url.rstrip("/").split("/")[-1]
        response = fetch(session, task_url + "?lang=ja")
        atomic_write(statement_dir / f"{task_id}.html", response.content)
        parser = StatementMarkdownParser()
        parser.feed(response.text)
        markdown = parser.markdown()
        if len(markdown) < 100:
            raise RuntimeError(f"Japanese statement extraction failed: {task_url}")
        atomic_write(statement_dir / f"{task_id}.md", markdown.encode())
        task_metadata.append({"task_id": task_id, "title": task_title, "url": task_url})

    standings_url = f"{BASE_URL}/contests/{contest_id}/standings/json"
    standings = fetch(session, standings_url, expect_json=True).json()
    rows = standings.get("StandingsData", [])[:10]
    if len(rows) != 10:
        raise RuntimeError(f"Expected 10 standings rows for {contest_id}, got {len(rows)}")

    top10: list[dict[str, object]] = []
    for index, row in enumerate(rows, 1):
        user = row["UserScreenName"]
        task_results = row.get("TaskResults", {})
        submission_ids = [result.get("SubmissionID") for result in task_results.values() if result.get("SubmissionID")]
        if not submission_ids:
            raise RuntimeError(f"No ranked submission found for {contest_id} rank {index} ({user})")
        saved: list[dict[str, object]] = []
        for task_id, result in task_results.items():
            if task_id not in {task["task_id"] for task in task_metadata}:
                continue
            submission_id = result.get("SubmissionID")
            if not submission_id:
                continue
            submission_url = f"{BASE_URL}/contests/{contest_id}/submissions/{submission_id}"
            prefix = f"{index:02d}_{safe_name(user)}_{submission_id}"
            existing = list(submissions_dir.glob(prefix + ".*"))
            code_path: Path
            language: str
            if existing and not refresh:
                code_path = existing[0]
                language = previous_languages.get(submission_id, "Unknown (cached file)")
            else:
                response = fetch(session, submission_url, allow_not_found=True)
                if response.status_code == 404:
                    reason = "AtCoder returned 404"
                    if row.get("UserIsDeleted"):
                        reason += "; the standings entry belongs to a deleted user"
                    saved.append({
                        "task_id": task_id,
                        "submission_id": submission_id,
                        "score": result.get("Score"),
                        "available": False,
                        "reason": reason,
                        "url": submission_url,
                    })
                    continue
                page = response.text
                code, language, ace_mode = extract_submission(page)
                code_path = submissions_dir / (prefix + extension_for(language, ace_mode))
                atomic_write(code_path, code.encode())
            saved.append({
                "task_id": task_id,
                "submission_id": submission_id,
                "score": result.get("Score"),
                "available": True,
                "language": language,
                "file": str(code_path.relative_to(contest_dir)),
                "url": submission_url,
                "sha256": hashlib.sha256(code_path.read_bytes()).hexdigest(),
            })
        top10.append({
            "position": index,
            "rank": row.get("Rank"),
            "user": user,
            "total_score": row.get("TotalResult", {}).get("Score"),
            "submissions": saved,
        })

    metadata = {
        "contest_id": contest_id,
        "contest_url": f"{BASE_URL}/contests/{contest_id}",
        "standings_url": f"{BASE_URL}/contests/{contest_id}/standings",
        "selection": "First 10 rows of official standings; SubmissionID recorded in each task result",
        "tasks": task_metadata,
        "top10": top10,
    }
    atomic_write(contest_dir / "metadata.json", (json.dumps(metadata, ensure_ascii=False, indent=2) + "\n").encode())
    return metadata


def write_readme(destination: Path, all_metadata: list[dict[str, object]]) -> None:
    first_number = int(str(all_metadata[0]["contest_id"])[3:])
    last_number = int(str(all_metadata[-1]["contest_id"])[3:])
    try:
        output_argument = destination.relative_to(Path.cwd())
    except ValueError:
        output_argument = destination
    lines = [
        "# AtCoder Heuristic Contest reference archive",
        "",
        "AHCの問題文と、公式順位表上位10行に対応する最終採用提出コードの個人参照用アーカイブです。",
        "各 `metadata.json` に順位、ユーザー名、得点、提出URL、ファイルのSHA-256を記録しています。",
        "",
        "## Contents",
        "",
        "| Contest | Tasks | Saved submissions |",
        "|---|---:|---:|",
    ]
    for metadata in all_metadata:
        count = sum(
            bool(submission.get("available", True))
            for row in metadata["top10"]
            for submission in row["submissions"]
        )
        lines.append(f"| [{metadata['contest_id']}]({metadata['contest_url']}) | {len(metadata['tasks'])} | {count} |")
    lines.extend([
        "",
        "## Notes",
        "",
        "- 問題文は日本語HTMLと、簡易変換したMarkdownの両方を保存しています。数式や表の確認にはHTMLを優先してください。",
        "- コードの著作権は各提出者に帰属します。再配布物ではなく個人学習用資料として扱ってください。",
        f"- 再取得: `python3 scripts/download_ahc_references.py --start {first_number} --end {last_number} --output {output_argument}`",
        "- 認証には `~/.local/share/online-judge-tools/cookie.jar` を読みますが、Cookie自体はこのアーカイブへ保存しません。",
        "",
    ])
    atomic_write(destination / "README.md", "\n".join(lines).encode())


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--start", type=int, default=50)
    parser.add_argument("--end", type=int, default=66)
    parser.add_argument("--output", type=Path, default=Path("reference_data/atcoder_heuristic/ahc050_066"))
    parser.add_argument("--cookie-jar", type=Path, default=DEFAULT_COOKIE_JAR)
    parser.add_argument("--refresh", action="store_true")
    args = parser.parse_args()
    if args.start > args.end:
        parser.error("--start must not exceed --end")

    session = load_session(args.cookie_jar.expanduser())
    destination = args.output.resolve()
    destination.mkdir(parents=True, exist_ok=True)
    all_metadata: list[dict[str, object]] = []
    failures: list[str] = []
    for number in range(args.start, args.end + 1):
        contest_id = f"ahc{number:03d}"
        print(f"[{contest_id}] downloading...", flush=True)
        try:
            metadata = download_contest(session, contest_id, destination, args.refresh)
            all_metadata.append(metadata)
            print(f"[{contest_id}] complete", flush=True)
        except Exception as error:
            failures.append(f"{contest_id}: {error}")
            print(f"[{contest_id}] ERROR: {error}", file=sys.stderr, flush=True)
    if all_metadata:
        write_readme(destination, all_metadata)
    if failures:
        print("\nFailures:\n" + "\n".join(failures), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
