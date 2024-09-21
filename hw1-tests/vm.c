#include <stdio.h>
#include <string.h>
#include "bof.h"



int main(int argc, char *argv[]) {

    //arg detector in order to determine if -p is in the args
    if (strcmp(argv[1],"-p") == 0) {
        printf("print mode");
    } else {
        printf("regular mode");
    }

    FILE *file;
    BOFHeader header;
    char buffer[256];

    // Open the BOF file
    file = fopen(argv[1], "rb"); // Open in binary read mode
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Parse the BOF header
    if (parse_bof_header(&header, file) != 0) {
        fclose(file);
        return -1;
    }

    //Read each line from the file and print it
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);  // Print each line
    }

    // Close the file
    fclose(file);

    return 0;
}

int parse_bof_header(BOFHeader *header, FILE *file) {
    
    // Read the header from the file, needs to be fixed 
    if (bof_read_header(header, file) != 0) {
        fprintf(stderr, "Error reading BOF header\n");
        return -1;
    }

    // Check the magic number, wtf is a magic number anyway
    if (strncmp(header->magic, "BO32", 4) != 0) {
        fprintf(stderr, "Invalid BOF magic number\n");
        return -1;
    }

    // Check for valid address/length
    if (header->text_start_address < 0 || header->text_length < 0 ||
        header->data_start_address < 0 || header->data_length < 0 ||
        header->stack_bottom_addr < 0) {
        fprintf(stderr, "Invalid addresses or lengths in BOF header\n");
        return -1;
    }

    if (header->data_start_address <= header->text_start_address ||
        header->stack_bottom_addr <= header->data_start_address) {
        fprintf(stderr, "Invalid address relationships in BOF header\n");
        return -1;
    }

    // debug if this code ever works
    printf("BOF Header:\n");
    printf("  Magic Number: %s\n", header->magic);
    printf("  Text Start Address: 0x%08X\n", header->text_start_address);
    printf("  Text Length: %d words\n", header->text_length);
    printf("  Data Start Address: 0x%08X\n", header->data_start_address);
    printf("  Data Length: %d words\n", header->data_length);
    printf("  Stack Bottom Address: 0x%08X\n", header->stack_bottom_addr);

    return 0; // if successful
}

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
