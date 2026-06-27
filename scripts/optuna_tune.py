#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import re
import shlex
import subprocess
from dataclasses import dataclass
from pathlib import Path
from typing import Any

import optuna


@dataclass(frozen=True)
class ParamSpec:
    name: str
    kind: str
    args: tuple[str, ...]

    @staticmethod
    def parse(text: str) -> "ParamSpec":
        parts = text.split(":")
        if len(parts) < 3:
            raise ValueError(f"invalid --param {text!r}: expected NAME:TYPE:...")
        name, kind, *args = parts
        if not name:
            raise ValueError(f"invalid --param {text!r}: empty name")
        if kind not in {"int", "float", "categorical"}:
            raise ValueError(f"invalid --param {text!r}: type must be int, float, or categorical")
        if kind == "int" and len(args) not in {2, 3}:
            raise ValueError(f"invalid int param {text!r}: expected NAME:int:LOW:HIGH[:STEP]")
        if kind == "float" and len(args) not in {2, 3}:
            raise ValueError(f"invalid float param {text!r}: expected NAME:float:LOW:HIGH[:log]")
        if kind == "categorical" and len(args) != 1:
            raise ValueError(f"invalid categorical param {text!r}: expected NAME:categorical:A,B,C")
        return ParamSpec(name=name, kind=kind, args=tuple(args))

    def suggest(self, trial: optuna.Trial) -> Any:
        if self.kind == "int":
            low = int(self.args[0])
            high = int(self.args[1])
            step = int(self.args[2]) if len(self.args) == 3 else 1
            return trial.suggest_int(self.name, low, high, step=step)
        if self.kind == "float":
            low = float(self.args[0])
            high = float(self.args[1])
            log = len(self.args) == 3 and self.args[2] == "log"
            if len(self.args) == 3 and self.args[2] != "log":
                raise ValueError(f"float param {self.name}: third field is only 'log'")
            return trial.suggest_float(self.name, low, high, log=log)
        choices = [parse_value(x) for x in self.args[0].split(",")]
        return trial.suggest_categorical(self.name, choices)


def parse_value(text: str) -> Any:
    if re.fullmatch(r"[-+]?\d+", text):
        return int(text)
    if re.fullmatch(r"[-+]?(?:\d+\.\d*|\d*\.\d+)(?:[eE][-+]?\d+)?", text) or re.fullmatch(
        r"[-+]?\d+[eE][-+]?\d+", text
    ):
        return float(text)
    return text


def shell_quote_value(value: Any) -> str:
    return shlex.quote(str(value))


def run_command(command: str, workdir: Path, env: dict[str, str], timeout: float | None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=workdir,
        env=env,
        shell=True,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=timeout,
        check=False,
    )


def extract_metric(output: str, pattern: str) -> float:
    matches = list(re.finditer(pattern, output, flags=re.MULTILINE))
    if not matches:
        raise RuntimeError(f"metric regex did not match: {pattern!r}")
    group = matches[-1].group(1)
    return float(group)


def objective_factory(args: argparse.Namespace, specs: list[ParamSpec]):
    workdir = Path(args.workdir).resolve()
    log_dir = (workdir / args.log_dir).resolve()
    log_dir.mkdir(parents=True, exist_ok=True)

    def objective(trial: optuna.Trial) -> float:
        params = {spec.name: spec.suggest(trial) for spec in specs}
        defines = " ".join(f"-D{name}={shell_quote_value(value)}" for name, value in params.items())

        env = os.environ.copy()
        env["OPTUNA_TRIAL_NUMBER"] = str(trial.number)
        env["OPTUNA_PARAMS_JSON"] = json.dumps(params, ensure_ascii=False, sort_keys=True)
        env["OPTUNA_DEFINES"] = defines
        env["CXXFLAGS"] = " ".join(x for x in [env.get("CXXFLAGS", ""), defines] if x)

        if args.build_command:
            build = run_command(args.build_command, workdir, env, args.timeout)
            if build.returncode != 0:
                write_trial_log(log_dir, trial.number, params, build, None)
                raise RuntimeError(f"build failed for trial {trial.number}\n{build.stderr}")
        else:
            build = None

        eval_proc = run_command(args.eval_command, workdir, env, args.timeout)
        write_trial_log(log_dir, trial.number, params, eval_proc, build)
        if eval_proc.returncode != 0:
            raise RuntimeError(f"eval failed for trial {trial.number}\n{eval_proc.stderr}")

        metric = extract_metric(eval_proc.stdout + "\n" + eval_proc.stderr, args.metric_regex)
        return metric

    return objective


def write_trial_log(
    log_dir: Path,
    trial_number: int,
    params: dict[str, Any],
    eval_proc: subprocess.CompletedProcess[str],
    build_proc: subprocess.CompletedProcess[str] | None,
) -> None:
    path = log_dir / f"trial_{trial_number:05d}.json"
    payload: dict[str, Any] = {
        "trial": trial_number,
        "params": params,
        "eval_returncode": eval_proc.returncode,
        "eval_stdout": eval_proc.stdout,
        "eval_stderr": eval_proc.stderr,
    }
    if build_proc is not None:
        payload.update(
            {
                "build_returncode": build_proc.returncode,
                "build_stdout": build_proc.stdout,
                "build_stderr": build_proc.stderr,
            }
        )
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description="Run Optuna tuning for AHC-style C++ solvers.")
    parser.add_argument("--workdir", default=".", help="directory where commands run")
    parser.add_argument("--study-name", default="ahc", help="Optuna study name")
    parser.add_argument("--storage", default=None, help="Optuna storage, e.g. sqlite:///out/optuna.db")
    parser.add_argument("--direction", choices=["minimize", "maximize"], required=True)
    parser.add_argument("--trials", type=int, default=50)
    parser.add_argument("--jobs", type=int, default=1, help="Optuna parallel jobs")
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--param", action="append", default=[], help="NAME:int:LOW:HIGH[:STEP], NAME:float:LOW:HIGH[:log], or NAME:categorical:A,B,C")
    parser.add_argument("--build-command", default="", help="optional build command; CXXFLAGS includes -D params")
    parser.add_argument("--eval-command", required=True, help="evaluation command; stdout/stderr must contain metric")
    parser.add_argument("--metric-regex", required=True, help="regex with capture group 1 for objective value")
    parser.add_argument("--timeout", type=float, default=None, help="timeout seconds for each command")
    parser.add_argument("--log-dir", default="out/optuna_trials", help="trial log directory under workdir")
    parser.add_argument("--sampler", choices=["tpe", "random"], default="tpe")
    args = parser.parse_args()

    if not args.param:
        parser.error("at least one --param is required")

    specs = [ParamSpec.parse(text) for text in args.param]
    sampler: optuna.samplers.BaseSampler
    if args.sampler == "random":
        sampler = optuna.samplers.RandomSampler(seed=args.seed)
    else:
        sampler = optuna.samplers.TPESampler(seed=args.seed)

    study = optuna.create_study(
        study_name=args.study_name,
        storage=args.storage,
        direction=args.direction,
        sampler=sampler,
        load_if_exists=True,
    )
    study.optimize(objective_factory(args, specs), n_trials=args.trials, n_jobs=args.jobs, catch=(RuntimeError,))

    print("best_value", study.best_value)
    print("best_params", json.dumps(study.best_params, ensure_ascii=False, sort_keys=True))
    print("best_defines", " ".join(f"-D{k}={shell_quote_value(v)}" for k, v in study.best_params.items()))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except KeyboardInterrupt:
        raise SystemExit(130)
