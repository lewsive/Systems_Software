#include <stdio.h>
#include <string.h>
#include "bof.h"
#include "utilities.c"
#include "machine_types.h"

// Global stack and stack pointer
int stack[MEMORY_SIZE_IN_WORDS];  // Stack array using MEMORY_SIZE_IN_WORDS
int sp = 0;                       // Stack pointer initialized to 0
void *buffer;
// Function to push values onto the stack
void push(int value) {
    if (sp >= MEMORY_SIZE_IN_WORDS) {
        fprintf(stderr, "Stack overflow\n");
        exit(EXIT_FAILURE);
    }
    stack[sp++] = value;  // Push value onto the stack
}

// Function to pop values from the stack
int pop() {
    if (sp <= 0) {
        fprintf(stderr, "Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    return stack[--sp];  // Pop value from the stack
}

// Function to convert a word to binary string
char *word_to_binary(word_type word) {
    char *binary = (char *)malloc(sizeof(char) * (WORD_SIZE + 1));
    if (binary == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = WORD_SIZE - 1; i >= 0; i--) {
        binary[i] = (word >> i) & 1 ? '1' : '0';
    }
    binary[WORD_SIZE] = '\0';
    return binary;
}

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments is provided
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-p] <filename>\n", argv[0]);
        return -1;
    }

    int print_mode = 0;
    const char *filename;

    // Determine if print mode is enabled
    if (strcmp(argv[1], "-p") == 0) {
        print_mode = 1;
        filename = argv[2];
    } else {
        filename = argv[1];
    }

    // Open the BOF file for reading
    BOFFILE bf = bof_read_open(filename);
    BOFHeader bf_header = bof_read_header(bf);
    
    if (bf.fileptr == NULL) {
        perror("Error opening file");
        return -1;
    }

    printf("BOF Header:\n");
    printf("  Magic Number: %s\n", bf_header.magic);
    printf("  Text Start Address: 0x%08X\n", bf_header.text_start_address);
    printf("  Text Length: %d words\n", bf_header.text_length);
    printf("  Data Start Address: 0x%08X\n", bf_header.data_start_address);
    printf("  Data Length: %d words\n", bf_header.data_length);
    printf("  Stack Bottom Address: 0x%08X\n", bf_header.stack_bottom_addr);

    // Reading words from the BOF file and pushing onto the stack
    for (int i = 0; i < bf_header.text_length; i++) {
        word_type bof_word = bof_read_word(bf);
        printf("  BOF Word %d: %x\n", i, bof_word); // Print the bof_word value
        push(bof_word);// Push word onto the stack
        size_t byte_amt = bof_file_bytes(bf);

        // Convert the word to binary and print it
        char *binary_word = word_to_binary(bof_word);
        printf("    Binary: %s\n", binary_word);
        free(binary_word);

    }

    // Close the BOF file
    bof_close(bf);

    return 0;
}
