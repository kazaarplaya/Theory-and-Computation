#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>


static const char* keywords[] = {"int", "char", "if", "else", "while", "for", "do", "return"};
static const char* delimiters = ";,(){}[]";
static const char* operators = "+-/%*=";

/* Character Helpers */
static bool is_operator(char c){
    if (strchr(operators, c) != NULL){
        return true;
    }
    return false;
}

static bool is_keyword(char *string){
    size_t length = sizeof(keywords) / sizeof(keywords[0]);
    for(size_t i = 0; i < length; i++){
        if(strcmp(keywords[i], string) == 0){
            return true;
        }
    }
    return false;
} 

static bool is_delimiter(char c){
    if (strchr(delimiters, c) != NULL){
        return true;
    }
    return false;
} 

/* Token Helpers */
const char* token_type_to_string(TokenType t){
    switch (t){
        case IDENTIFIER: return "IDENTIFIER";
        case KEYWORD: return "KEYWORD";
        case DELIMITER: return "DELIMITER";
        case INTEGER: return "INTEGER";
        case OPERATOR: return "OPERATOR";
        case EOF_TOKEN: return "EOF";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
};

Token create_token(Lexer *l, TokenType type, size_t start){
    Token token;
    token.type = type;
    token.line = l->line;
    token.column = l->column;
    
    size_t length = l->currentPosition - start;
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

    // if end, terminate
    if (l->currentPosition >= l->inputLength){
        l->ch = '\0';
    } else {
        l->ch = l->source[l->nextPosition];
    }

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
    l.column = 0;
    
    advance_char(&l);
    return l;
}

/* Main Lexer Logic */
Token tokenize(Lexer *l){

    // skip whitespaces
    while (isspace(l->ch)){
        advance_char(l);
    }

    // eof
    if (l->ch == '\0') {
        size_t start = l->currentPosition;
        Token token = create_token(l, EOF_TOKEN, start);
        return token;
    }

    // delimiter
    else if (is_delimiter(l->ch)) {
        size_t start = l->currentPosition;
        advance_char(l);
        Token token = create_token(l, DELIMITER, start);
        return token;
    }
    
    // operators
    else if (is_operator(l->ch)) {
        size_t start = l->currentPosition;
        advance_char(l);
        Token token = create_token(l, OPERATOR, start);
        return token;
    }

    // integers
    else if (isdigit(l->ch)){
        size_t start = l->currentPosition; 
        bool has_error = false;
        while(!isspace(l->ch) && !is_delimiter(l->ch) && !is_operator(l->ch)){
            if (!isdigit(l->ch)){
                has_error = true;
            }
            advance_char(l);
        }

        Token token;
        if (has_error){
            token = create_token(l, ERROR, start);
        } else {        
            token = create_token(l, INTEGER, start);
        }
        return token;
    }

    // identifier / keyword
    else if (isalpha(l->ch) || l->ch == '_'){
        bool has_error = false;
        size_t start = l->currentPosition;
        while((!isspace(l->ch) && !is_delimiter(l->ch)) && !is_operator(l->ch) || l->ch == '_'){
            if (!isalnum(l->ch) && l->ch != '_'){
                has_error = true;
            }
            advance_char(l);
        }

        Token token;
        token = create_token(l, IDENTIFIER, start);
        
        if (has_error) {
            token = create_token(l, ERROR, start);
        } else if (is_keyword(token.lexeme)) {
            token = create_token(l, KEYWORD, start);
        }
        return token;
    }

    else {
        size_t start = l->currentPosition;
        advance_char(l);
        Token token = create_token(l, ERROR, start);
        return token;
    }
}