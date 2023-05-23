#!/bin/bash

mkdir -p wasm &&
cp euclid.html wasm &&
cp style.css wasm &&
cd wasm &&
emcmake cmake -Ddebug=ON .. &&
cmake --build .
