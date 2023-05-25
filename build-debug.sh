#!/bin/bash

mkdir -p build-wasm-debug &&
cp web/* build-wasm-debug/ &&
cd build-wasm-debug &&
emcmake cmake -Ddebug=ON .. &&
cmake --build .
