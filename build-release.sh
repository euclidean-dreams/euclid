#!/bin/bash

mkdir -p wasm-deploy &&
cp euclid.html wasm-deploy &&
cp style.css wasm-deploy &&
cd wasm-deploy &&
emcmake cmake -Ddebug=OFF .. &&
cmake --build . &&
cp euclid.html ../deploy/proscenium/static &&
cp euclid.js ../deploy/proscenium/static &&
cp euclid.wasm ../deploy/proscenium/static &&
cp euclid.worker.js ../deploy/proscenium/static &&
cp style.css ../deploy/proscenium/static
