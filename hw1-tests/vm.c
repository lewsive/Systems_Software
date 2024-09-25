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

    switch (type) {
        case comp_instr_type:
            switch (instruction.comp.func) {
                case NOP_F:
                    // Do nothing
                    break;
                case ADD_F:
                    GPR[instruction.comp.rt] = GPR[instruction.comp.rs] + GPR[instruction.comp.ot];
                    break;
                case SUB_F:
                    GPR[instruction.comp.rt] = GPR[instruction.comp.rs] - GPR[instruction.comp.ot];
                    break;
                // Add more cases for other computational instructions
                default:
                    fprintf(stderr, "Unknown computational function: %u\n", instruction.comp.func);
                    exit(EXIT_FAILURE);
            }
            break;
        case other_comp_instr_type:
            switch (instruction.othc.func) {
                case LIT_F:
                    GPR[instruction.othc.reg] = instruction.othc.arg;
                    break;
                case ARI_F:
                    GPR[instruction.othc.reg] += instruction.othc.arg;
                    break;
                case SRI_F:
                    GPR[instruction.othc.reg] -= instruction.othc.arg;
                    break;
                // Add more cases for other computational instructions
                default:
                    fprintf(stderr, "Unknown other computational function: %u\n", instruction.othc.func);
                    exit(EXIT_FAILURE);
            }
            break;
        case immed_instr_type:
            switch (instruction.immed.op) {
                case ADDI_O:
                    GPR[instruction.immed.reg] += instruction.immed.immed;
                    break;
                case ANDI_O:
                    GPR[instruction.immed.reg] &= instruction.immed.immed;
                    break;
                // Add more cases for immediate instructions
                default:
                    fprintf(stderr, "Unknown immediate opcode: %u\n", instruction.immed.op);
                    exit(EXIT_FAILURE);
            }
            break;
        case jump_instr_type:
            switch (instruction.jump.op) {
                case JMPA_O:
                    PC = instruction.jump.addr;
                    break;
                case CALL_O:
                    GPR[7] = PC;  // Save return address in $ra
                    PC = instruction.jump.addr;
                    break;
                case RTN_O:
                    PC = GPR[7];  // Return to saved address
                    break;
                default:
                    fprintf(stderr, "Unknown jump opcode: %u\n", instruction.jump.op);
                    exit(EXIT_FAILURE);
            }
            break;
        case syscall_instr_type:
            switch (instruction.syscall.code) {
                case exit_sc:
                    exit(instruction.syscall.offset);
                case print_str_sc:
                    printf("%s", (char *)&memory[GPR[instruction.syscall.reg] + instruction.syscall.offset]);
                    break;
                case print_char_sc:
                    putchar(memory[GPR[instruction.syscall.reg] + instruction.syscall.offset]);
                    break;
                case read_char_sc:
                    memory[GPR[instruction.syscall.reg] + instruction.syscall.offset] = getchar();
                    break;
                case start_tracing_sc:
                    // Start tracing
                    break;
                case stop_tracing_sc:
                    // Stop tracing
                    break;
                default:
                    fprintf(stderr, "Unknown syscall code: %u\n", instruction.syscall.code);
                    exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Unknown instruction type: %u\n", type);
            exit(EXIT_FAILURE);
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
        printf("%s\n",mnemonic);
    }

    // bin_instr_t inst = instruction_read(bf);
    // // syscall_type mySysType = instruction_syscall_number(inst);
    // instr_type myType = instruction_type(inst);
    // printf("Syscall number: %d\n",mySysType)

    // Load instructions into memory
    bof_read_bytes(bf, bf_header.text_length * sizeof(word_type), (void*)&((word_type*)memory)[PC]);

    // Load data into memory
    bof_read_bytes(bf, bf_header.data_length * sizeof(word_type), (void*)&((word_type*)memory)[bf_header.data_start_address]);

    // Close the BOF file
    bof_close(bf);

    // Execute the program
    while (PC < MEMORY_SIZE_IN_WORDS) {
        bin_instr_t instruction = *(bin_instr_t*)&((word_type*)memory)[PC++];
        execute_instruction(instruction);
    }

    return 0;
}
