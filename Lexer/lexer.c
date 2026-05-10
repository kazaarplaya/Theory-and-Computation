#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>


static const char* keywords[] = {"int", "char", "if", "else", "while", "for", "do", "return"};

/* Character Helpers */
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

static bool is_keyword(const char *string){
    size_t length = sizeof(keywords) / sizeof(keywords[0]);
    for(size_t i = 0; i < length; i++){
        if(strcmp(keywords[i], string) == 0){
            return true;
        }
    }
    return false;
} 

/* Token Helpers */
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

static Token create_token(Lexer *l, TokenType type, size_t start){
    Token token;
    token.type = type;
    token.line = l->line;
    token.column = l->column;

    // check if length is greater than defined max
    size_t length = l->currentPosition - start;
    if (length >= MAX_LEXEME_LENGTH) {
        length = MAX_LEXEME_LENGTH - 1;
        token.type = TOKEN_ERROR;
    }

    strncpy(token.lexeme, l->source + start, length);
    token.lexeme[length] = '\0';
    return token;
};

/* Lexer Initialisation and Lexer Movement */
static void advance_char(Lexer *l){

    // advance line and column
    if (l->ch == '\n') {
        l->line++; 
        l->column = 1;
    } else {
        l->column++;
    }
    
    // move pointer 
    if (l->currentPosition >= l->inputLength){
        l->ch = '\0';
    } else {
        l->ch = l->source[l->nextPosition];
    }

    // advance pointer by 1 position
    l->currentPosition = l->nextPosition;
    l->nextPosition++;
}

Lexer initialise_lexer(const char* input){
    Lexer l;
    l.source = input;
    l.inputLength = strlen(input);
    l.ch = '\0';

    l.currentPosition = 0;
    l.nextPosition = 0;
    l.line = 1;
    l.column = -1;
    
    advance_char(&l);
    return l;
}

static LexerState get_start_state(char c){
    if (c == '\0'){
        return STATE_EOF;
    } else if (is_delimiter(c)){
        return STATE_DELIMITER;
    } else if (is_operator(c)){
        return STATE_OPERATOR;
    } else if (isdigit(c)){
        return STATE_INTEGER;
    } else if (isalpha(c) || c == '_') {
        return STATE_IDENTIFIER;
    } else {
        return STATE_ERROR;
    }
}

/* Main Lexer Logic */
Token tokenize(Lexer *l){

    // skip whitespaces
    while (isspace(l->ch)){
        advance_char(l);
    }

    // start state
    size_t start = l->currentPosition;
    LexerState state = get_start_state(l->ch);
    
    // tokenization logic
    switch(state) {
        case STATE_EOF:
            advance_char(l);
            return create_token(l, TOKEN_EOF, start);

        case STATE_DELIMITER:
            advance_char(l);
            return create_token(l, TOKEN_DELIMITER, start);

        case STATE_OPERATOR:
            advance_char(l);
            return create_token(l, TOKEN_OPERATOR, start);

        case STATE_INTEGER:
            // consume valid digit 
            while(isdigit(l->ch)){
                advance_char(l);
            }
            
            // check if integer is invalid
            if (!isdigit(l->ch) || l->ch == '_'){
                while(!is_delimiter(l->ch)){
                    advance_char(l);
                }
                return create_token(l, TOKEN_ERROR, start);
            }
            return create_token(l, TOKEN_INTEGER, start);

        case STATE_IDENTIFIER:
            while(isalnum(l->ch) || l->ch == '_'){
                advance_char(l);
            }
            
            Token token = create_token(l, TOKEN_IDENTIFIER, start);
            if (is_keyword(token.lexeme)) {
                return create_token(l, TOKEN_KEYWORD, start);
            } 
            return token;

        case STATE_ERROR:
            default:
                advance_char(l);
                return create_token(l, TOKEN_ERROR, start);
        };
}