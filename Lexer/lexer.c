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

static bool is_safe_point(char c) {
    return c == '\0' ||
           isspace((unsigned char)c) ||
           is_delimiter(c) ||
           is_operator(c);
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

    strncpy(token.lexeme, l->source + start_position, length);
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
    if (l->current_position >= l->input_length){
        l->ch = '\0';
    } else {
        l->ch = l->source[l->next_position];
    }

    // advance pointer by 1 position
    l->current_position = l->next_position;
    l->next_position++;
}

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

/* Main Lexer Logic */
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