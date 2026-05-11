#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>

#define MAX_LEXEME_LENGTH 64

/*
 * Represents the lexer state used to classify the next token
 * based on the current input character
 */
typedef enum {
    STATE_IDENTIFIER,
    STATE_INTEGER,
    STATE_DELIMITER,
    STATE_OPERATOR,
    STATE_ERROR,
    STATE_EOF
} LexerState; 

/*
 * Represents the type of token returned by the lexer
 */
typedef enum {
    TOKEN_KEYWORD, 
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER, 
    TOKEN_DELIMITER,
    TOKEN_OPERATOR,
    TOKEN_ERROR, 
    TOKEN_EOF,
} TokenType; 

/*
 * Stores a single token produced by the lexer, including its type,
 * lexeme, and source location
 */
typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME_LENGTH];
    int line;
    int column;
} Token;

/*
 * Stores the current state of the lexer while scanning the input
 */
typedef struct {
    const char* source;
    size_t input_length;
    size_t current_position; 
    size_t next_position;
    char ch;
    int line;
    int column;
} Lexer;

Lexer initialise_lexer(const char* input);
Token tokenize(Lexer *l);
const char* token_type_to_string(TokenType t);

#endif