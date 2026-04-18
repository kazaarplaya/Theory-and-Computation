#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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

const char* keywords[] = {"int", "char", "if", "else", "while", "for", "do", "return"};
const char* delimiters = ";,(){}[]";
size_t length = sizeof(keywords) / sizeof(keywords[0]);

// helper functions
bool is_keyword(char *string){
    for(size_t i = 0; i < length; i++){
        if(strcmp(keywords[i], string) == 0){
            return true;
        }
    }
    return false;
} 

bool is_delimiter(char c){
    if (strchr(delimiters, c) != NULL){
        return true;
    }
    return false;
} 

char* create_string_buffer(FILE* fptr){

    // Get file size
    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    rewind(fptr);
    
    // Allocate buffer size
    char *buffer = malloc(size + 1);
    if (buffer == NULL){
        printf("Buffer Allocation Failed!\n");
        return NULL;
    }

    // Read into buffer
    size_t bytes_read = fread(buffer, 1, size, fptr);
    buffer[bytes_read] = '\0';

    return buffer; 
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
        // printf("%c", **current);
        token.lexeme[0] = **current;
        token.lexeme[1] = '\0';
        token.type = DELIMITER;
        (*current)++;
        return token;
    };
    
    // operators
    if (strchr("+-/*=", **current)) {
        // printf("%c", **current);
        token.lexeme[0] = **current;
        token.lexeme[1] = '\0';
        token.type = OPERATOR;
        (*current)++;
        return token;
    };

    // integers
    if (isdigit(**current)){
        int i = 0;
        while(isdigit(**current)){
            token.lexeme[i] = **current;
            // printf("%c\n", **current);
            (*current)++;
            i++; 
        }
        token.lexeme[i] = '\0';
        token.type = INTEGER;
        return token;
    }

    // identifier / keyword
    if (isalnum(**current)){
        bool has_error = false;

        int i = 0;
        while(**current != '\0' && !isspace(**current) && !is_delimiter(**current)){
            if (!isalpha(**current)){
                has_error = true;
            }
            
            token.lexeme[i] = **current;
            // printf("%c\n", **current);
            (*current)++;
            i++; 
        }
        token.lexeme[i] = '\0';
        
        // printf("%s", token.lexeme);

        if (has_error){
            token.type = ERROR;
        } else if (is_keyword(token.lexeme)){
             token.type = KEYWORD;
        } else{
            token.type = IDENTIFIER;
        }
        
        return token;
    }

    token.type = ERROR;
    return token;
    
};

int main(int argc, char *argv[]) {
    // Check arguments
    if (argc != 2){
        printf("Invalid number of arguments\n");
        return EXIT_FAILURE;
    }
    
    FILE* fptr = fopen(argv[1], "r");

    // Check if file could not be opened
    if (fptr == NULL){
        printf("File not found");
        return EXIT_FAILURE;
    }
    printf("File successfuly opened!\n");
    
    // buffer pointers 
    char *source = create_string_buffer(fptr); 
    char *current = source;

    // output file
    FILE *outputfptr;
    outputfptr = fopen("output.txt", "w");
    
    printf("%s\n\n", current);

    while (1){
        Token curr = get_next_token(&current);

        if (curr.type == EOF_TOKEN){
            break;
        }

        fprintf(outputfptr, "%s %s\n", type_to_string(curr.type), curr.lexeme);
    }

    fclose(fptr);
    fclose(outputfptr);
    free(source);
    return EXIT_SUCCESS;
}