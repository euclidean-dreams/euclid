#!/bin/bash

mkdir -p build-wasm-release &&
cd build-wasm-release &&
emcmake cmake -Dwasm=ON -Ddebug=OFF .. &&
cmake --build . &&
cd .. &&

# export build files
cp -R web/static deploy/proscenium &&
cp -R web/templates deploy/proscenium &&
cp build-wasm-release/euclid.js deploy/proscenium/static &&
cp build-wasm-release/euclid.wasm deploy/proscenium/static &&
cp build-wasm-release/euclid.worker.js deploy/proscenium/static
