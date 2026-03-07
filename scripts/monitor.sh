#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
"$ROOT/.venv/bin/pio" device monitor -d "$ROOT/firmware"
