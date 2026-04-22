#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"


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
    
    // output file
    FILE *outputfptr;
    outputfptr = fopen("output.txt", "w");

    Lexer lexer = initialiseLexer(create_string_buffer(fptr));
    Token curr;
    do {
        curr = tokenize(&lexer);
        fprintf(outputfptr, "%s %s\n", tokentoString(curr.type), curr.lexeme);     
    } while (curr.type != EOF_TOKEN);

    // free memory and pointers
    fclose(fptr);
    fclose(outputfptr);
    return EXIT_SUCCESS;
}