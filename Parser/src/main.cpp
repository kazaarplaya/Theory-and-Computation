#include "parser.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

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

        /*  
        If token is an EOF Token, lexeme is set to "EOF".
        If not, parse normally.  
        */ 
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
    if (argc < 2) {
        std::cerr << "Error: Missing input file name.\n";
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1; 
    }

    std::string filename = argv[1];
    std::vector<Token> tokens = readTokensFromLexer(filename);

    Parser parser = Parser(tokens);
    parser.parse();

    // Test
    // for (const Token& token : tokens) {
    //     std::cout << tokenTypeToString(token.type)
    //               << " " << token.lexeme
    //               << " line=" << token.line
    //               << " col=" << token.column
    //               << std::endl;
    // }
}
