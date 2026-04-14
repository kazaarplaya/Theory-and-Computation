#include <stdio.h>
#include <stdlib.h>

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
    
    // Get file size
    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    rewind(fptr);

    // Allocate buffer size
    char *buffer = malloc(size + 1);
    if (buffer == NULL){
        printf("Buffer Allocation Failed!\n");
        return EXIT_FAILURE;
    }

    // Read into buffer
    fread(buffer, 1, size, fptr);
    buffer[size] = '\0';
    
    char *current = buffer;
    while (*current != '\0'){
        printf("%c", *current);
        current++; 
    }

    fclose(fptr);
    return EXIT_SUCCESS;
}