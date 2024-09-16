#include <stdio.h>



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


