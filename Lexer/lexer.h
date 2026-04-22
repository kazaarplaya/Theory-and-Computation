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
    char lexeme[64];
    TokenType type;
    int line;
    int column;
} Token;

typedef struct {
    const char* source;
    char ch;
    size_t currentPosition; 
    size_t nextPosition;
    int inputLength;
    int line;
    int column;
} Lexer;

Lexer initialiseLexer(const char* input);
Token tokenize(Lexer *l);
const char* tokentoString(TokenType t);

#endif