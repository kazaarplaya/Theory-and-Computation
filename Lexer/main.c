#include <stdio.h>
#include <stdlib.h>
#include "lexer.c"

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

    // free memory and pointers
    fclose(fptr);
    fclose(outputfptr);
    free(source);
    return EXIT_SUCCESS;
}