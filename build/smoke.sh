#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
test -f out/dell-os-v0.1-amd64.iso
docker run --rm --user "$(id -u):$(id -g)" -v "$PWD:/work" dell-os-iso-tools python3 build/smoke.py "$@"
