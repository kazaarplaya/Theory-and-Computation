#ifndef LEXER_H
#define LEXER_H

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


Token get_next_token(char **current);
const char* type_to_string(TokenType t);

#endif