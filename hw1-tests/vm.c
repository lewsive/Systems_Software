#include <stdio.h>
#include <string.h>
#include "bof.h"

int parse_bof_header(BOFHeader *header, BOFFILE bf);

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
    if (bf.fileptr == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Read the header from the file
    BOFHeader header;
    if (parse_bof_header(&header, bf) != 0) {
        bof_close(bf);
        return -1;
    }

    char buffer[256];
    while (bof_read_bytes(bf, sizeof(buffer), buffer) > 0) {
        printf("%s", buffer);
    }

    bof_close(bf);
    return 0;
}

int parse_bof_header(BOFHeader *header, BOFFILE bf) {
    *header = bof_read_header(bf);

    // Check the magic number, wtf is a magic number anyway
    if (!bof_has_correct_magic_number(*header)) {
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

    // debug if this code works
    printf("BOF Header:\n");
    printf("  Magic Number: %s\n", header->magic);
    printf("  Text Start Address: 0x%08X\n", header->text_start_address);
    printf("  Text Length: %d words\n", header->text_length);
    printf("  Data Start Address: 0x%08X\n", header->data_start_address);
    printf("  Data Length: %d words\n", header->data_length);
    printf("  Stack Bottom Address: 0x%08X\n", header->stack_bottom_addr);

    return 0;
}
