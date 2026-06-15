#!/bin/zsh

set -e

EXECUTABLE="./parser"
RESULT_DIR="unit_test_outputs"
INPUT_DIR="tests/inputs"

mkdir -p "$RESULT_DIR"
rm -f "$RESULT_DIR"/*.txt

g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp src/parser.cpp -o "$EXECUTABLE"

run_success_test() {
    local name="$1"
    local input_file="$2"

    : > output.txt

    set +e
    "$EXECUTABLE" "$input_file" > /tmp/parser_test_stdout.txt 2> /tmp/parser_test_stderr.txt
    local exit_code=$?
    set -e

    if [ "$exit_code" -ne 0 ]; then
        echo "Test failed unexpectedly: $name"
        cat /tmp/parser_test_stderr.txt
        exit 1
    fi

    cp output.txt "$RESULT_DIR/${name}_output.txt"
}

run_invalid_test() {
    local name="$1"
    local input_file="$2"

    : > output.txt

    set +e
    "$EXECUTABLE" "$input_file" > /tmp/parser_test_stdout.txt 2> "$RESULT_DIR/${name}_stderr.txt"
    local exit_code=$?
    set -e

    if [ "$exit_code" -eq 0 ]; then
        echo "Invalid test parsed successfully when it should have failed: $name"
        exit 1
    fi
}

run_success_test "valid_minimal_class" "$INPUT_DIR/valid_minimal_class.txt"
run_success_test "valid_for_assignment" "$INPUT_DIR/valid_for_assignment.txt"
run_success_test "valid_dangling_else" "$INPUT_DIR/valid_dangling_else.txt"
run_success_test "stress_large_class_list" "$INPUT_DIR/stress_large_class_list.txt"
run_invalid_test "invalid_missing_semicolon" "$INPUT_DIR/invalid_missing_semicolon.txt"
run_invalid_test "invalid_missing_class_identifier" "$INPUT_DIR/invalid_missing_class_identifier.txt"

echo "Unit tests complete. Outputs saved in $RESULT_DIR/"
