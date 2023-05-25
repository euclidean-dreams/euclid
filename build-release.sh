#!/bin/bash

mkdir -p build-wasm-release &&
cp web/* build-wasm-release/ &&
cd build-wasm-release &&
emcmake cmake -Ddebug=OFF .. &&
cmake --build . &&
cd .. &&

# collect the most up-to-date build
cp web/* deploy/proscenium/static/ &&
cp build-wasm-release/euclid.js deploy/proscenium/static &&
cp build-wasm-release/euclid.wasm deploy/proscenium/static &&
cp build-wasm-release/euclid.worker.js deploy/proscenium/static
