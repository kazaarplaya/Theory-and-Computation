#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>


static const char* keywords[] = {"int", "char", "if", "else", "while", "for", "do", "return"};
static const char* delimiters = ";,(){}[]";
static size_t length = sizeof(keywords) / sizeof(keywords[0]);

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
static bool isKeyword(char *string){
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

Token tokenize(Lexer *l){
    Token token;

    // skip whitespaces
    while (isspace(l->ch)){
        advanceChar(l);
    }

    if (l->ch == '\0') {
        token.lexeme[0] = '\0';
        token.type = EOF_TOKEN;
        return token;
    }

    // delimiter
    if (isDelimiter(l->ch)) {
        token.lexeme[0] = l->ch;
        token.lexeme[1] = '\0';
        token.type = DELIMITER;
        advanceChar(l);
        return token;
    };
    
    // operators
    if (strchr("+-/*=", l->ch)) {
        token.lexeme[0] = l->ch;
        token.lexeme[1] = '\0';
        token.type = OPERATOR;
        advanceChar(l);
        return token;
    };  

    // integers
    if (isdigit(l->ch)){
        bool has_error = false;
        int i = 0;
        while(l->ch != '\0' && !isspace(l->ch) && !isDelimiter(l->ch)){
            if (!isdigit(l->ch)){
                has_error = true;
            }

            token.lexeme[i] = l->ch;
            advanceChar(l);
            i++; 
        }
        token.lexeme[i] = '\0';

        if (has_error){
            token.type = ERROR;
        } else {        
            token.type = INTEGER;
        }       
        return token;
    }

    // identifier / keyword
    if (isalpha(l->ch)){
        bool has_error = false;
        int i = 0;
        while(l->ch != '\0' && !isspace(l->ch) && !isDelimiter(l->ch)){
            if (!isalpha(l->ch)){
                has_error = true;
            }
            
            token.lexeme[i] = l->ch;
            advanceChar(l);
            i++; 
        }
        token.lexeme[i] = '\0';

        if (has_error){
            token.type = ERROR;
        } else if (isKeyword(token.lexeme)){
             token.type = KEYWORD;
        } else {
            token.type = IDENTIFIER;
        }
        
        return token;
    }

    token.type = ERROR;
    return token;
};