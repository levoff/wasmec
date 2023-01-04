#!/bin/sh

set -xe

PATH="/usr/local/opt/llvm/bin:$PATH"
clang -Wall -Wextra --target=wasm32 -o wasm.o -c ./module.c
wasm-ld -m wasm32 --no-entry --export-all --allow-undefined -o cmodule.wasm wasm.o
