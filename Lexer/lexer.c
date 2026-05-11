#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>


static const char* keywords[] = {"int", "char", "if", "else", "while", "for", "do", "return"};

/***********************************************
 *  Character Helpers
 ***********************************************/

/**
 * Check if a character is a valid operator
 */
static bool is_operator(char c) {
    switch (c) {
        case '+':
        case '-':
        case '/':
        case '%':
        case '*':
        case '=':
            return true;
        default:
            return false;
    }
}

/**
 * Check if a character is a delimiter
 */
static bool is_delimiter(const char c) {
    switch (c) {
        case ';':
        case ',':
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
            return true;
        default:
            return false;
    }
}

/**
 * Check if a string is a recognised keyword
 */
static bool is_keyword(const char *string){
    size_t length = sizeof(keywords) / sizeof(keywords[0]);
    for(size_t i = 0; i < length; i++){
        if(strcmp(keywords[i], string) == 0){
            return true;
        }
    }
    return false;
}

/**
 * Used for panic mode consumption for integers.
 * Whitespace, delimiters, operators and null terminators are safe recovery points
 */
static bool is_safe_point(char c) {
    return c == '\0' ||
           isspace((unsigned char)c) ||
           is_delimiter(c) ||
           is_operator(c);
}

/***********************************************
 *  Token Helpers
 ***********************************************/

/**
 * Converts TokenType to string 
 */
const char* token_type_to_string(TokenType t){
    switch(t) {
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_KEYWORD: return "KEYWORD";
        case TOKEN_DELIMITER: return "DELIMITER";
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_OPERATOR: return "OPERATOR";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
};

/**
 * Creates and returns a Token from the lexer input
 *
 * The token is built using the token type, starting position, and starting
 * line and column passed into the function. The lexeme is copied from the
 * source input between start_position and the lexer's current position.
 *
 * If the lexeme is longer than MAX_LEXEME_LENGTH, it is truncated to fit
 * inside the token buffer and the token type is changed to TOKEN_ERROR to
 * indicate that the input was too long to be treated as a valid token
 */
static Token create_token(Lexer *l, TokenType type, size_t start_position, int start_line, int start_col){
    Token token;
    token.type = type;
    token.line = start_line;
    token.column = start_col;

    // check if length is greater than defined max
    size_t length = l->current_position - start_position;
    if (length >= MAX_LEXEME_LENGTH) {
        length = MAX_LEXEME_LENGTH - 1;
        token.type = TOKEN_ERROR;
    }

    // copy characters into token lexeme
    strncpy(token.lexeme, l->source + start_position, length);
    token.lexeme[length] = '\0';
    return token;
};

/***********************************************
 *  Lexer Initialisation and Helpers
***********************************************/

/**
 * Advances the lexer by one character
 *
 * This updates the lexer's line and column counters based on the current
 * character, then moves the current and next positions forward in the input
 * stream. If the lexer has reached the end of the input, the current
 * character is set to '\0' to mark EOF.
 */
static void advance_char(Lexer *l){

    // update line and column 
    if (l->ch == '\n') {
        l->line++; 
        l->column = 1;
    } else {
        l->column++;
    }
    
    // move pointer 
    if (l->current_position >= l->input_length){
        l->ch = '\0';
    } else {
        l->ch = l->source[l->next_position];
    }

    // advance pointer by 1 position
    l->current_position = l->next_position;
    l->next_position++;
}

/**
 * Initialise and returns a lexer for the input string
 */
Lexer initialise_lexer(const char* input){
    Lexer l;
    l.source = input;
    l.input_length = strlen(input);
    l.ch = '\0';

    l.current_position = 0;
    l.next_position = 0;
    l.line = 1;
    l.column = 0;
    
    advance_char(&l);
    return l;
}

/**
 * Initialise and returns a lexer for the input string
 */
static LexerState get_start_state(char c){
    if (c == '\0'){
        return STATE_EOF;
    } else if (is_delimiter(c)){
        return STATE_DELIMITER;
    } else if (is_operator(c)){
        return STATE_OPERATOR;
    } else if (isdigit((unsigned char)c)){
        return STATE_INTEGER;
    } else if (isalpha((unsigned char)c) || c == '_') {
        return STATE_IDENTIFIER;
    } else {
        return STATE_ERROR;
    }
}

/***********************************************
 * DFA / Tokenization Logic
 ***********************************************/

/**
 * Reads and returns the next token from the lexer input.
 *
 * Leading whitespace characters are skipped before tokenisation begins.
 * The lexer then records the starting position, line, and column of the
 * token before determining the correct start state from the current
 * character.
 *
 * Single character tokens such as delimiters and operators are consumed
 * immediately. Integer tokens consume consecutive digits and use panic-mode
 * recovery if an invalid character appears before a safe stopping point.
 * Identifier tokens consume letters, digits, and underscores, then check
 * whether the completed lexeme is a keyword.
 *
 * If the current character does not match any valid token category, an
 * error token is returned.
 */
Token tokenize(Lexer *l){

    // skip whitespaces
    while (isspace(l->ch)){
        advance_char(l);
    }

    // start state
    size_t start_position = l->current_position;
    int start_line = l->line;
    int start_column = l->column;
    LexerState state = get_start_state(l->ch); 
    
    // tokenization logic
    switch(state) {
        case STATE_EOF:
            advance_char(l);
            return create_token(l, TOKEN_EOF, start_position, start_line, start_column);

        case STATE_DELIMITER:
            advance_char(l);
            return create_token(l, TOKEN_DELIMITER, start_position, start_line, start_column);

        case STATE_OPERATOR:
            advance_char(l);
            return create_token(l, TOKEN_OPERATOR, start_position, start_line, start_column);

        case STATE_INTEGER:
            // consume valid digit 
            while(isdigit((unsigned char)l->ch)){
                advance_char(l);
            }
            
            // check if integer is invalid
            if (!is_safe_point(l->ch)){
                while (!is_safe_point(l->ch)) {
                    advance_char(l);
                }
                return create_token(l, TOKEN_ERROR, start_position, start_line, start_column);
            }
            return create_token(l, TOKEN_INTEGER, start_position, start_line, start_column);

        case STATE_IDENTIFIER:
            while(isalnum(l->ch) || l->ch == '_'){
                advance_char(l);
            }
            
            Token token = create_token(l, TOKEN_IDENTIFIER, start_position, start_line, start_column);
            if (is_keyword(token.lexeme)) {
                return create_token(l, TOKEN_KEYWORD, start_position, start_line, start_column);
            } 
            return token;

        case STATE_ERROR:
            default:
                advance_char(l);
                return create_token(l, TOKEN_ERROR, start_position, start_line, start_column);
        };
}