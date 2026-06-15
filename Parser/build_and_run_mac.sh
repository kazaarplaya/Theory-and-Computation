#!/bin/zsh

set -e

OUTPUT="./parser"

if [ "$#" -lt 1 ]; then
    echo "Usage: ./build_and_run_mac.sh <token-file>"
    echo "Example: ./build_and_run_mac.sh tests/inputs/valid_minimal_class.txt"
    exit 1
fi

g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp src/parser.cpp -o "$OUTPUT"

"$OUTPUT" "$1"
