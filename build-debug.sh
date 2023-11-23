#!/bin/bash

mkdir -p build-wasm-debug &&
cp web/* build-wasm-debug/ &&
cd build-wasm-debug &&
emcmake cmake -Dwasm=ON -Ddebug=ON .. &&
cmake --build .
