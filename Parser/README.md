# Parser

This project contains a C++ LR-style parser. It reads tokenised lexer output from a text file, parses the token stream, prints the parse tree, and writes the parse tree to `output.txt` when parsing succeeds.

## Requirements

- C++ compiler with C++17 support
- `g++` available from the terminal

## Input Format

The parser expects tokenised lexer output, one token per line.

Example:

```text
KEYWORD class line=1 col=1
IDENTIFIER A line=1 col=7
DELIMITER { line=1 col=9
DELIMITER } line=1 col=10
DELIMITER ; line=1 col=11
EOF line=1 col=12
```

Test input files are stored in:

```text
tests/inputs/
```

Grammar and Bison-related files are stored in:

```text
grammar/
```

## Run On macOS / Linux

Build and run with one command:

```bash
./build_and_run_mac.sh tests/inputs/valid_minimal_class.txt
```

Or build manually:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp src/parser.cpp -o parser
./parser tests/inputs/valid_minimal_class.txt
```

## Run On Windows

Using PowerShell:

```powershell
.\build.ps1
.\parser.exe tests\inputs\valid_minimal_class.txt
```

Or build manually:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic src\main.cpp src\parser.cpp -o parser.exe
.\parser.exe tests\inputs\valid_minimal_class.txt
```

## Output

For valid input, the parser prints the parse tree to the terminal and writes it to:

```text
output.txt
```

For invalid input, the parser prints a syntax error to stderr and exits with code `1`.

## Unit Tests On macOS / Linux

Run:

```bash
./run_unit_tests_mac.sh
```

Saved test results are written to:

```text
unit_test_outputs/
```

Successful tests keep only their parse-tree output files:

```text
*_output.txt
```

Invalid tests keep only their error output files:

```text
*_stderr.txt
```

## Current Test Cases

- `valid_minimal_class.txt`
- `valid_for_assignment.txt`
- `valid_dangling_else.txt`
- `stress_large_class_list.txt`
- `invalid_missing_semicolon.txt`
- `invalid_missing_class_identifier.txt`
