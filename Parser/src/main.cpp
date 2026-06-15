#include "parser.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// Reads the lexer output file and converts each line into a Token object.
std::vector<Token> readTokensFromLexer(const std::string& filename){
    std::ifstream inputFile(filename);
    std::vector<Token> tokens; 

    if (!inputFile) {
        std::cerr << "[ERROR] Could not open input file!" << std::endl;
        return tokens;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);

        std::string category;
        std::string lexeme;
        std::string lineSection;
        std::string colSection;

        ss >> category; 

        // EOF lines do not include a normal lexeme, so one is added here.
        if (category == "EOF"){
            lexeme = "EOF";
            ss >> lineSection >> colSection;
        } else {
            ss >> lexeme >> lineSection >> colSection;
        }
        
        Token token;
        token.type = mapToToken(category, lexeme);
        token.lexeme = lexeme;
        token.line = extractNumberAfterEquals(lineSection);
        token.column = extractNumberAfterEquals(colSection);

        tokens.push_back(token);
    }

    return tokens;

}

int main(int argc, char* argv[]){

    // The parser expects a lexer output file to be provided.
    if (argc < 2) {
        std::cerr << "Error: Missing input file name.\n";
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1; 
    }

    // Load the token stream from the lexer output.
    std::string filename = argv[1];
    std::vector<Token> tokens = readTokensFromLexer(filename);

    // Run the parser and return a failure code if syntax analysis fails.
    Parser parser = Parser(tokens);
    if (!parser.parse()) {
        return 1;
    }

    return 0;
}
