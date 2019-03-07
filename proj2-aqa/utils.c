#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int get(unsigned int, int, int);

/* Sign extends the given field to a 32-bit integer where field is
 * interpreted an n-bit integer. */ 
int sign_extend_number( unsigned int field, unsigned int n) {
    /* YOUR CODE HERE */
    unsigned int output;
    unsigned int mask = 0xFFFFFFFF << n;
    if ((field & (1 << (n - 1))) != 0) {
        output = field | mask;
    }
    else {
        output = field & ~mask;
    }
    return (unsigned int) output;
}

/* Unpacks the 32-bit machine code instruction given into the correct
 * type within the instruction struct */ 
Instruction parse_instruction(uint32_t instruction_bits) {
    /* YOUR CODE HERE */
    Instruction instruction;
    instruction.bits = instruction_bits;
    return instruction;
}

int get(unsigned int num, int start, int end) {
    int mask = ~(~0 << (end - start + 1));
    return (num >>  start) & mask;
}

/* Return the number of bytes (from the current PC) to the branch label using the given
 * branch instruction */
int get_branch_offset(Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int seven = instruction.sbtype.imm7;
    unsigned int five = instruction.sbtype.imm5;
    unsigned int first = get(seven, 6, 6);
    unsigned int second = get(five, 0, 0);
    unsigned int third = (first << 1) | second;
    unsigned int fourth = get(seven, 0, 5);
    unsigned int fifth = (third << 6) | fourth;
    unsigned int sixth = get(five, 1, 4);
    unsigned int last = (fifth << 4) | sixth;
    return sign_extend_number((last << 1), 13);
}

/* Returns the number of bytes (from the current PC) to the jump label using the given
 * jump instruction */
int get_jump_offset(Instruction instruction) {
    /* YOUR CODE HERE */
    unsigned int imm = instruction.ujtype.imm;
    unsigned int first = get(imm, 0, 7);
    unsigned int second = get(imm, 8, 8);
    unsigned int third = get(imm, 9, 18);
    unsigned int fourth = get(imm, 19, 19);
    unsigned int value = ((((((fourth << 8)
                            | first) << 1)
                            | second) << 10)
                            | third) << 1;
    return sign_extend_number(value, 21);
}

int get_store_offset(Instruction instruction) {
    /* YOUR CODE HERE */
    return sign_extend_number(((instruction.stype.imm7 << 5)
                              | (instruction.stype.imm5)), 12);
}

void handle_invalid_instruction(Instruction instruction) {
    printf("Invalid Instruction: 0x%08x\n", instruction.bits); 
}

void handle_invalid_read(Address address) {
    printf("Bad Read. Address: 0x%08x\n", address);
    exit(-1);
}

void handle_invalid_write(Address address) {
    printf("Bad Write. Address: 0x%08x\n", address);
    exit(-1);
}