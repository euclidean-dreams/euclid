#!/bin/bash

cd wasm && python3 -m http.server
# if address in use:
# sudo lsof -i:8000
# then kill that pid
