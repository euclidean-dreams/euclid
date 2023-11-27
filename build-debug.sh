#!/bin/bash

mkdir -p build-wasm-debug &&
cp web/* build-wasm-debug/ &&
cd build-wasm-debug &&
emcmake cmake -Dwasm=ON -Ddebug=ON .. &&
cmake --build . &&
cd .. &&

# export build files
cp -R web/static deploy/proscenium &&
cp -R web/templates deploy/proscenium &&
cp build-wasm-debug/euclid.js deploy/proscenium/static &&
cp build-wasm-debug/euclid.wasm deploy/proscenium/static &&
cp build-wasm-debug/euclid.worker.js deploy/proscenium/static
