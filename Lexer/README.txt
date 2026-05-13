====================
Running the Lexer
====================

Compile:
gcc -std=c99 -Wall -Wextra -Os -s main.c lexer.c -o lexer

Run on macOS/Linux:
./lexer input.txt

Run on Windows:
lexer.exe input.txt


====================
Running Unit Tests
====================

Compile Unity tests:
gcc -std=c99 -Wall -Wextra -I.\src -I.\unity .\tests\unit-testing\test_lexer.c .\src\lexer.c .\unity\unity.c -o test_lexer

Run on macOS/Linux:
./test_lexer

Run on Windows:
test_lexer.exe


====================
Notes
====================

Test files referenced in the specification are included in the tests folder. The corresponding outputs can be found in the output folder.

For black box testing, you may run the lexer using any of the provided test files, or create your own input using input.txt.

Note: Not all test cases used in the Testing process is sourced in this repo. 