#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>

typedef enum {
    STATE_START,
    STATE_IDENTIFIER,
    STATE_INTEGER,
    STATE_DELIMITER,
    STATE_OPERATOR,
    STATE_ERROR,
    STATE_EOF
} LexerState; 

typedef enum {
    TOKEN_KEYWORD, 
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER, 
    TOKEN_DELIMITER,
    TOKEN_OPERATOR,
    TOKEN_ERROR, 
    TOKEN_EOF,
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