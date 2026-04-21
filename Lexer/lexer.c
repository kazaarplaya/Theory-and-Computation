#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

static const char* keywords[] = {"int", "char", "if", "else", "while", "for", "do", "return"};
static const char* delimiters = ";,(){}[]";
static size_t length = sizeof(keywords) / sizeof(keywords[0]);

// helper functions
static bool is_keyword(char *string){
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

const char* type_to_string(TokenType t){
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

Token get_next_token(char **current){
    Token token;

    // skip whitespaces
    while (isspace((unsigned char)**current)){
        (*current)++;
    }
    printf("DEBUG: '%c' (%d)\n", **current, (unsigned char)**current);

    if (**current == '\0') {
        token.lexeme[0] = '\0';
        token.type = EOF_TOKEN;
        return token;
    }

    // delimiter
    if (is_delimiter(**current)) {
        token.lexeme[0] = **current;
        token.lexeme[1] = '\0';
        token.type = DELIMITER;
        (*current)++;
        return token;
    };
    
    // operators
    if (strchr("+-/*=", **current)) {
        token.lexeme[0] = **current;
        token.lexeme[1] = '\0';
        token.type = OPERATOR;
        (*current)++;
        return token;
    };  

    // integers
    if (isdigit(**current)){
        bool has_error = false;
        int i = 0;
        while(**current != '\0' && !isspace(**current) && !is_delimiter(**current)){
            if (!isdigit(**current)){
                has_error = true;
            }

            token.lexeme[i] = **current;
            (*current)++;
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
    if (isalpha(**current)){
        bool has_error = false;
        int i = 0;
        while(**current != '\0' && !isspace(**current) && !is_delimiter(**current)){
            if (!isalpha(**current)){
                has_error = true;
            }
            
            token.lexeme[i] = **current;
            (*current)++;
            i++; 
        }
        token.lexeme[i] = '\0';

        if (has_error){
            token.type = ERROR;
        } else if (is_keyword(token.lexeme)){
             token.type = KEYWORD;
        } else {
            token.type = IDENTIFIER;
        }
        
        return token;
    }

    token.type = ERROR;
    return token;
};