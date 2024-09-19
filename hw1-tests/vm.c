#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {

    //arg detector in order to determine if -p is in the args
    if (strcmp(argv[1],"-p") == 0) {
        printf("print mode");
    } else {
        printf("regular mode");
    }

    FILE *file;
    char buffer[256];  // Buffer to store each line

    // Open the file in read mode
    file = fopen("input.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    printf("dick");

    //Read each line from the file and print it
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);  // Print each line
    }

    // Close the file
    fclose(file);

    return 0;
}

////sfdsffvxc

//two modes of operation:
//1. if(args) = ./vm test.bof - Execution, run a passed bof
//2. if(args) = ./vm -p test.bof - Print, when -p is passed, print assembly language formed by the bof
//bof contains a header that needs to be read
//after header, binary instructions follow

//initializing:
//memory = 0
//instructions are loaded, starting at index 0
//the first N words are filled with instructions from bof, N = text.length
//
