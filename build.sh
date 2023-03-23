#!/bin/bash

mkdir -p wasm &&
cp euclid.html wasm &&
cd wasm &&
emcmake cmake -D_wasm=ON .. &&
cmake --build .
