#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>

typedef enum {
    KEYWORD,
    IDENTIFIER,
    INTEGER, 
    DELIMITER,
    OPERATOR,
    ERROR, 
    EOF_TOKEN,
} TokenType; 

typedef struct {
    TokenType type;
    char lexeme[64];

    int line;
    int column;
} Token;

typedef struct {
    const char* source;
    char ch;
    int inputLength;

    size_t currentPosition; 
    size_t nextPosition;
    
    int line;
    int column;
} Lexer;

Lexer initialise_lexer(const char* input);
Token tokenize(Lexer *l);
const char* token_type_to_string(TokenType t);

#endif