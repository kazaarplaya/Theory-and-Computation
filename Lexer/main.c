#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"


/*
 * Creates a dynamically allocated string buffer containing the contents
 * of the given file.
 *
 * Returns a pointer to the buffer if successful, or NULL if memory allocation fails.
 */
char* create_string_buffer(FILE* fptr){

    // Move to end of the file to determine file size
    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    rewind(fptr);
    
    // Allocate buffer size based on identified file size
    char *buffer = malloc(size + 1);
    if (buffer == NULL){
        printf("Buffer Allocation Failed!\n");
        return NULL;
    }

    // Read file into buffer
    size_t bytes_read = fread(buffer, 1, size, fptr);
    buffer[bytes_read] = '\0';

    return buffer; 
};

int main(int argc, char *argv[]) {

    fprintf(stderr, "[INFO] Starting lexer program\n");

    /*
     * Validate command line arguments. Lexer expects 
     * only one argument which is the input file location
     */
    if (argc != 2) {
        fprintf(stderr, "[ERROR] Invalid number of arguments\n");
        fprintf(stderr, "[INFO] Usage: ./lexer <input-file>\n");
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[INFO] Input file argument received: %s\n", argv[1]);
    
    /*
     * Open the source file in read mode and 
     * exit if the input file could not be read
     */
    fprintf(stderr, "[INFO] Opening input file\n");

    FILE* fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        fprintf(stderr, "[ERROR] File not found: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[INFO] Input file opened successfully\n");
    
    /*
     * Open output file where generated tokens will be written
     */
    fprintf(stderr, "[INFO] Creating output file: output.txt\n");

    FILE *outputfptr = fopen("output.txt", "w");
    if (outputfptr == NULL) {
        fprintf(stderr, "[ERROR] Could not open output file\n");
        fclose(fptr);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[INFO] Output file created successfully\n");

    /*
     * Read the input file into a dynamically allocated string buffer.
     * This buffer is then used as the input source for the lexer
     */
    fprintf(stderr, "[INFO] Reading input file into buffer\n");

    char *source = create_string_buffer(fptr);
    if (source == NULL) {
        fprintf(stderr, "[ERROR] Buffer pointer is NULL\n");
        fclose(fptr);
        fclose(outputfptr);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[INFO] Source buffer created successfully\n");

    // Initialise the lexer.
    fprintf(stderr, "[INFO] Initialising lexer\n");

    Lexer lexer = initialise_lexer(source);
    Token curr;

    fprintf(stderr, "[INFO] Starting tokenisation\n");

    // Repeatedly tokenize the input until the EOF token is reached
    do {
        curr = tokenize(&lexer);

        /*
         * If an invalid token is identified, an error message is also printed to 
         * stderr with line and column information.
         */
        if (curr.type == TOKEN_ERROR) {
            fprintf(stderr, "[LEXICAL ERROR] line %d, column %d: invalid token '%s'\n",
                curr.line,
                curr.column,
                curr.lexeme);
        }

        fprintf(stderr, "[TOKEN] %-12s '%s' at line %d, column %d\n",
            token_type_to_string(curr.type),
            curr.lexeme,
            curr.line,
            curr.column);

        // Write token to output file
        fprintf(outputfptr, "%-12s %-20s line=%-4d column=%-4d\n",
            token_type_to_string(curr.type),
            curr.lexeme,
            curr.line,
            curr.column);   

    } while (curr.type != TOKEN_EOF);

    fprintf(stderr, "[INFO] Tokenisation complete\n");

    // Close open files and free dynamically allocated memory.
    fprintf(stderr, "[INFO] Cleaning up resources\n");

    fclose(fptr);
    fclose(outputfptr);
    free(source);

    fprintf(stderr, "[INFO] Program finished successfully\n");
    
    return EXIT_SUCCESS;
}