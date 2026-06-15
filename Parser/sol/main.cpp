#include "parser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

// Test adapter for lexer output. Each stdin line can be:
// TOKEN
// TOKEN lexeme
// TOKEN lexeme line column
std::vector<Token> readTokensFromStdIn() {
    std::vector<Token> tokens;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::istringstream input(line);

        std::string typeName;
        std::string lexeme;
        int lineNumber = 0;
        int columnNumber = 0;

        input >> typeName;
        if (!(input >> lexeme)) {
            lexeme = typeName;
        }
        input >> lineNumber >> columnNumber;

        tokens.push_back({tokenTypeFromName(typeName), lexeme, lineNumber, columnNumber});
    }

    return tokens;
}

} // namespace

int main() {
    try {
        LR1Parser parser;
        ParseResult result = parser.parse(readTokensFromStdIn());

        if (!result.success) {
            std::cerr << result.errorMessage << '\n';
            return 1;
        }

        parser.printTree(*result.tree, std::cout);

        std::ofstream outputFile("output.txt");
        if (!outputFile) {
            std::cerr << "Could not open output.txt for writing\n";
            return 1;
        }
        parser.printTree(*result.tree, outputFile);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
