#include <stdio.h>
#include <string.h>
#include "bof.h"
#include "utilities.c"

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
    BOFHeader bf_header = bof_read_header(bf);

    printf("BOF Header:\n");
    printf("  Magic Number: %s\n", bf_header.magic);
    printf("  Text Start Address: 0x%08X\n", bf_header.text_start_address);
    printf("  Text Length: %d words\n", bf_header.text_length);
    printf("  Data Start Address: 0x%08X\n", bf_header.data_start_address);
    printf("  Data Length: %d words\n", bf_header.data_length);
    printf("  Stack Bottom Address: 0x%08X\n", bf_header.stack_bottom_addr);

    return 0;
}


//We have the data now
//Next, we need to make the stack