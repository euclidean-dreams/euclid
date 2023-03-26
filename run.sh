#!/bin/bash

cd wasm &&
  python3 -m http.server &
# if address in use:
# sudo lsof -i:8000
# then kill that pid


# launches a browser for euclid
# due to this issue https://stackoverflow.com/questions/68592278/sharedarraybuffer-is-not-defined

/Applications/Google\ Chrome.app/Contents/MacOS/Google\ Chrome \
  --user-data-dir=./.user_data \
  --enable-features=SharedArrayBuffer \
  http://localhost:8000
