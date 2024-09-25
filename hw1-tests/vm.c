#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bof.h"
#include "utilities.h"
#include "machine_types.h"
#include "instruction.h"
#include "regname.h"

#define MEMORY_SIZE_IN_WORDS 32768

// Global stack and stack pointer
int stack[MEMORY_SIZE_IN_WORDS];  // Stack array using MEMORY_SIZE_IN_WORDS
char memory[MEMORY_SIZE_IN_WORDS];  // Assuming memory is an array of char

int sp = 0;                       // Stack pointer initialized to 0
void *buffer;

// Registers
word_type GPR[8];  // General Purpose Registers
word_type PC;      // Program Counter
word_type HI;      // High part of multiplication result or remainder of division
word_type LO;      // Low part of multiplication result or quotient of division

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
    static char binary_str[33];  // 32 bits + null terminator
    binary_str[32] = '\0';       // Null terminator

    for (int i = 31; i >= 0; i--) {
        binary_str[i] = (word & 1) ? '1' : '0';
        word >>= 1;
    }

    return binary_str;
}

// Function to execute a single instruction
void execute_instruction(bin_instr_t instruction) {
    instr_type type = instruction_type(instruction);
    printf("type: %d ",type);
    printf("opcode: %d ",instruction.comp.op); //opcode
    printf("func? %d ",instruction.comp.func);

    // if(instruction.comp.op == 0){
    //     if(instruction.comp.func == 1){
    //         printf("There's an add in my shit dawg");
    //     }
    // }

    switch(instruction.comp.op){
        case 0:
            switch(instruction.comp.func){
                case 0:
                    printf("NOP\n");
                    break;
                case 1:
                    printf("ADD\n");
                    break;
                case 2:
                    printf("SUB\n");
                    break;
                case 3:
                    printf("CPW\n");
                    break;
                case 5:
                    printf("AND\n");
                    break;
                case 6:
                    printf("BOR\n");
                    break;
                case 7:
                    printf("NOR\n");
                    break;
                case 8:
                    printf("XOR\n");
                    break;
                case 9:
                    printf("LWR\n");
                    break;
                case 10:
                    printf("SWR\n");
                    break;
                case 11:
                    printf("SCA\n");
                    break;
                case 12:
                    printf("LWI\n");
                    break;
                case 13:
                    printf("NEG\n");
                    break;
                default:
                    printf("error in opcode 0");
            }break;

        case 1:{
            switch(instruction.comp.func){
                case 1:
                    printf("LIT\n");
                    break;
                case 2:
                    printf("ARI\n");
                    break;
                case 3:
                    printf("SRI\n");
                    break;
                case 4:
                    printf("MUL\n");
                    break;
                case 5:
                    printf("DIV\n");
                    break;
                case 6:
                    printf("CFHI\n");
                    break;
                case 7:
                    printf("CFLO\n");
                    break;
                case 8:
                    printf("SLL\n");
                    break;
                case 9:
                    printf("SRL\n");
                    break;
                case 10:
                    printf("JMP\n");
                    break;
                case 11:
                    printf("CSI\n");
                    break;
                case 12:
                    printf("JREL\n");
                    break;
                    //needs work probably
                case 15:
                    printf("SYS\n");
                        syscall_type mySysType = instruction_syscall_number(instruction);
                        switch(mySysType){
                            case 1:
                                printf("EXIT\n");
                                break;
                            case 2:
                                printf("PSTR\n");
                                break;
                            case 4:
                                printf("PCH\n");
                                break;
                            case 5:
                                printf("RCH\n");
                                break;
                            case 2046:
                                printf("STRA\n");
                            case 2047:
                                printf("NOTR\n");
                            
                        }break;
                    break;
                default:
                    printf("error in opcode 1");
            }break;
        }
        
        case 2:
            printf("ADDI\n");
            break;
        case 3:
            printf("ANDI");
            break;
        case 4:
            printf("BORI");
            break;
        case 5:
            printf("NORI");
            break;
        case 6:
            printf("XORI");
            break;
        case 7:
            printf("BEQ");
            break;
        case 8:
            printf("BGEZ");
            break;
        case 9:
            printf("BGTZ");
            break;
        case 10:
            printf("BLEZ");
            break;
        case 11:
            printf("BLTZ");
            break;
        case 12:
            printf("BNE");
            break;
        case 13:
            printf("JMPA");
            break;
        case 14:
            printf("CALL");
            break;
        case 15:
            printf("RTN");
            break;
    }
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

    // Initialize registers
    GPR[0] = bf_header.data_start_address;  // $gp
    GPR[1] = bf_header.stack_bottom_addr;   // $sp
    GPR[2] = bf_header.stack_bottom_addr;   // $fp
    PC = bf_header.text_start_address;

    bin_instr_t inst;
    const char* mnemonic;

    for(int i = 0;i<bf_header.text_length;i++){
        inst = instruction_read(bf);
        mnemonic = instruction_mnemonic(inst);
        execute_instruction(inst);
        // printf("%s\n",mnemonic);
        
    }

    // bin_instr_t inst = instruction_read(bf);
    // // syscall_type mySysType = instruction_syscall_number(inst);

    // printf("Syscall number: %d\n",mySysType)
        // instr_type myType = instruction_type(inst);

    // Load instructions into memory
    bof_read_bytes(bf, bf_header.text_length * sizeof(word_type), (void*)&((word_type*)memory)[PC]);

    // Load data into memory
    bof_read_bytes(bf, bf_header.data_length * sizeof(word_type), (void*)&((word_type*)memory)[bf_header.data_start_address]);

    // Close the BOF file
    bof_close(bf);

    // Execute the program
    // while (PC < MEMORY_SIZE_IN_WORDS) {
    //     bin_instr_t instruction
    //     execute_instruction(instruction);
    // }

    
    
    return 0;
}

//implement all code for the instructions in the switch cases.
//figure out trace print statements
//put in stack
//make makefile
