#!/bin/bash

mkdir -p wasm &&
#cp euclid.html wasm &&
cd wasm &&
emcmake cmake -Ddebug=ON .. &&
cmake --build .
