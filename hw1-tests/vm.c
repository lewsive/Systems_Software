#include <stdio.h>
#include <string.h>
#include "bof.h"
#include "utilities.c"


int main(int argc, char *argv[]) {

    int fileArg;
    //arg detector in order to determine if -p is in the args
    if (strcmp(argv[1],"-p") == 0) {
        printf("print mode\n");
        fileArg = 2;
    } else {
        printf("regular mode\n");
        fileArg = 1;
    }
    BOFFILE newFile = bof_read_open(argv[fileArg]);
        printf("reading file: %s\n", newFile.filename);
    
    BOFHeader fileHeader = bof_read_header(newFile);
        printf("data length: %d\n",fileHeader.data_length);

    return 0;
}

////sfdsffvxc

//two modes of operation:
//1. if(args) = ./vm test.bof - Execution, run a passed bof
//2. if(args) = ./vm -p test.bof - Print, when -p is passed, print assembly language formed by the bof
//bof contains a header that needs to be read
//information pertaining to the bof needs to be placed in a bof header struct
//after header, binary instructions follow
//

//initializing:
//memory = 0
//instructions are loaded, starting at index 0
//the first N words are filled with instructions from bof, N = text.length

//modulation:
//import appropriate files


