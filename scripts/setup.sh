#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

echo "Creating virtual environment..."
python3 -m venv "$ROOT/.venv"

echo "Installing PlatformIO..."
"$ROOT/.venv/bin/pip" install platformio

echo "Done. Run ./scripts/build.sh to compile firmware."
