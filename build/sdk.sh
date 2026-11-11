#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p out
docker build --target sdk -f build/Dockerfile -t dell-os-sdk .
docker run --rm --user "$(id -u):$(id -g)" -v "$PWD:/src" dell-os-sdk bash -c 'cmake -S . -B out/native -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build out/native -j2 && ctest --test-dir out/native --output-on-failure && DESTDIR=/src/out/stage cmake --install out/native --prefix /usr'
