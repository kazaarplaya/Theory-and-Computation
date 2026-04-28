#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>


static const char* keywords[] = {"int", "char", "if", "else", "while", "for", "do", "return"};
static const char* delimiters = ";,(){}[]";
static const char* operators = "+-/*=";

static void advanceChar(Lexer *l){
    // if end, terminate
    if (l->currentPosition >= l->inputLength){
        l->ch = '\0';
    } else {
        l->ch = l->source[l->nextPosition];
    }

    l->currentPosition = l->nextPosition;
    l->nextPosition++;
}

Lexer initialiseLexer(const char* input){
    Lexer l;
    l.source = input;
    l.inputLength = strlen(input);
    l.ch = '\0';
    l.currentPosition = 0;
    l.nextPosition = 0;
    l.line = 1;
    advanceChar(&l);
    return l;
}

// helper functions
static bool isOperator(char c){
    if (strchr(operators, c) != NULL){
        return true;
    }
    return false;
}

static bool isKeyword(char *string){
    size_t length = sizeof(keywords) / sizeof(keywords[0]);
    for(size_t i = 0; i < length; i++){
        if(strcmp(keywords[i], string) == 0){
            return true;
        }
    }
    return false;
} 

static bool isDelimiter(char c){
    if (strchr(delimiters, c) != NULL){
        return true;
    }
    return false;
} 

const char* tokentoString(TokenType t){
    switch (t){
        case IDENTIFIER: return "IDENTIFIER";
        case KEYWORD: return "KEYWORD";
        case DELIMITER: return "DELIMITER";
        case INTEGER: return "INTEGER";
        case OPERATOR: return "OPERATOR";
        case EOF_TOKEN: return "EOF";
        case ERROR: return "ERROR";
    }
};

Token createToken(Lexer *l, TokenType type, size_t start){
    Token token;
    token.type = type;
    size_t length = l->currentPosition - start;
    strncpy(token.lexeme, l->source + start, length);
    token.lexeme[length] = '\0';
    return token;
};

Token tokenize(Lexer *l){
    
    // skip whitespaces
    while (isspace(l->ch)){
        advanceChar(l);
    }

    // eof
    if (l->ch == '\0') {
        size_t start = l->currentPosition;
        return createToken(l, EOF_TOKEN, start);
    }

    // delimiter
    else if (isDelimiter(l->ch)) {
        size_t start = l->currentPosition;
        advanceChar(l);
        return createToken(l, DELIMITER, start);
    }
    
    // operators
    else if (isOperator(l->ch)) {
        size_t start = l->currentPosition;
        advanceChar(l);
        return createToken(l, OPERATOR, start);
    }

    // integers
    else if (isdigit(l->ch)){
        size_t start = l->currentPosition; 
        bool has_error = false;
        while(!isspace(l->ch) && !isDelimiter(l->ch)){
            if (!isdigit(l->ch)){
                has_error = true;
            }
            advanceChar(l);
        }

        Token token;
        if (has_error){
            token = createToken(l, ERROR, start);
        } else {        
            token = createToken(l, INTEGER, start);
        }
        return token;
    }

    // identifier / keyword
    else if (isalpha(l->ch) || l->ch == '_'){
        bool has_error = false;
        size_t start = l->currentPosition;
        while((!isspace(l->ch) && !isDelimiter(l->ch)) || l->ch == '_'){
            if (!isalpha(l->ch) && l->ch != '_'){
                has_error = true;
            }
            advanceChar(l);
        }

        Token token;
        token = createToken(l, IDENTIFIER, start);
        
        if (has_error) {
            token = createToken(l, ERROR, start);
        } else if (isKeyword(token.lexeme)) {
            token = createToken(l, KEYWORD, start);
        }
        return token;
    }

    else {
        Token token = createToken(l, ERROR, l->currentPosition);
        return token;
    }
}