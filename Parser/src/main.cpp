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
        return 1;
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
        token.token = mapToToken(&category, &lexeme);


    }
    

    

    return tokens;

}