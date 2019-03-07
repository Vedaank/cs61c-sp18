#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"

void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_lui(Instruction, Processor *);

void execute_instruction(uint32_t instruction_bits, Processor *processor, Byte *memory) {
    Instruction instruction = parse_instruction(instruction_bits);
    switch(instruction.opcode) {
        case 0x33:
            execute_rtype(instruction, processor);
            processor->PC += 4;
            break;
        case 0x13:
            execute_itype_except_load(instruction, processor);
            processor->PC += 4;
            break;
        case 0x73:
            execute_ecall(processor, memory);
            processor->PC += 4;
            break;
        case 0x63:
            execute_branch(instruction, processor);
            break;
        case 0x6F:;
            execute_jal(instruction, processor);
            break;
        case 0x23:
            execute_store(instruction, processor, memory);
            processor->PC += 4;
            break;
        case 0x03:
            execute_load(instruction, processor, memory);
            processor->PC += 4;
            break;
        case 0x37:
            execute_lui(instruction, processor);
            processor->PC += 4;
            break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor) {
    switch (instruction.rtype.funct3){
        case 0x0:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // Add
                    processor->R[instruction.rtype.rd]
                            = (int) processor->R[instruction.rtype.rs1]
                              + (int) processor->R[instruction.rtype.rs2];
                    break;
                case 0x1:
                    // Mul
                    processor->R[instruction.rtype.rd]
                            = (int) processor->R[instruction.rtype.rs1]
                              * (int) processor->R[instruction.rtype.rs2];
                    break;
                case 0x20:
                    // Sub
                    processor->R[instruction.rtype.rd]
                            = (int) processor->R[instruction.rtype.rs1]
                              - (int) processor->R[instruction.rtype.rs2];
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x1:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // SLL
                    processor->R[instruction.rtype.rd]
                            = (unsigned) processor->R[instruction.rtype.rs1]
                              << (int) processor->R[instruction.rtype.rs2];
                    break;
                case 0x1:
                    // MULH
                    processor->R[instruction.rtype.rd]
                            = ((sDouble)(processor->R[instruction.rtype.rs1])
                              * (sDouble)(processor->R[instruction.rtype.rs2])) >> 32;
                    break;
            }
            break;
        case 0x2:
            // SLT
            if ((int) processor->R[instruction.itype.rs1] < (int) processor->R[instruction.rtype.rs2]) {
                processor->R[instruction.itype.rd] = 1;
            } else {
                processor->R[instruction.itype.rd] = 0;
            }
            break;
        case 0x4:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // XOR
                    processor->R[instruction.rtype.rd]
                            = (unsigned) processor->R[instruction.rtype.rs1]
                              ^ (unsigned) processor->R[instruction.rtype.rs2];
                    break;
                case 0x1:
                    // DIV
                    processor->R[instruction.rtype.rd]
                            = (int) processor->R[instruction.rtype.rs1]
                              / (int) processor->R[instruction.rtype.rs2];
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x5:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // SRL
                    processor->R[instruction.rtype.rd]
                            = (unsigned) processor->R[instruction.rtype.rs1]
                              >> (int) processor->R[instruction.rtype.rs2];
                    break;
                case 0x20:
                    // SRA
                    processor->R[instruction.rtype.rd]
                            = (int) (processor->R[instruction.rtype.rs1])
                            >> (int) processor->R[instruction.rtype.rs2];
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                break;
            }
            break;
        case 0x6:
            switch (instruction.rtype.funct7) {
                case 0x0:
                    // OR
                    processor->R[instruction.rtype.rd]
                            = (unsigned) processor->R[instruction.rtype.rs1]
                              | (unsigned)  processor->R[instruction.rtype.rs2];
                    break;
                case 0x1:
                    // REM
                    processor->R[instruction.rtype.rd]
                            = (int) processor->R[instruction.rtype.rs1]
                              % (int) processor->R[instruction.rtype.rs2];
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    exit(-1);
                    break;
            }
            break;
        case 0x7:
            // AND
            processor->R[instruction.rtype.rd]
                    =  (unsigned) processor->R[instruction.rtype.rs1]
                      & (unsigned) processor->R[instruction.rtype.rs2];
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_itype_except_load(Instruction instruction, Processor *processor) {
    switch (instruction.itype.funct3) {
        case 0x0:
            // ADDI
            processor->R[instruction.itype.rd]
                    = (int) processor->R[instruction.itype.rs1]
                      + (int) sign_extend_number(instruction.itype.imm, 12);
            break;
        case 0x1:
            // SLLI
            processor->R[instruction.itype.rd]
                    = (unsigned) processor->R[instruction.itype.rs1]
                      << (int) instruction.itype.imm;
            break;
        case 0x2:
            // SLTI
            if ((int) processor->R[instruction.itype.rs1] < (int) sign_extend_number(instruction.itype.imm, 12)) {
                processor->R[instruction.itype.rd] = 1;
            } else {
                processor->R[instruction.itype.rd] = 0;
            }
            break;
        case 0x4:
            // XORI
            processor->R[instruction.itype.rd]
                    = (unsigned) processor->R[instruction.itype.rs1]
                        ^ (unsigned) sign_extend_number(instruction.itype.imm, 12);
            break;
        case 0x5:
            // Shift Right (You must handle both logical and arithmetic)
            switch(instruction.itype.imm >> 5) {
                case 0x00:
                    // SRLI
                    processor->R[instruction.itype.rd]
                            = (unsigned) processor->R[instruction.itype.rs1]
                            >> (int) instruction.itype.imm;
                    break;
                case 0x20:
                    // SRAI
                    processor->R[instruction.itype.rd]
                            = (int) processor->R[instruction.itype.rs1]
                            >> (int) instruction.itype.imm;
                    break;
                default:
                    handle_invalid_instruction(instruction);
                    break;
            }
            break;
        case 0x6:
            // ORI
            processor->R[instruction.itype.rd]
                    = (unsigned) processor->R[instruction.itype.rs1]
                    | (unsigned) sign_extend_number(instruction.itype.imm, 12);
            break;
        case 0x7:
            // ANDI
            processor->R[instruction.itype.rd]
                    = (unsigned) processor->R[instruction.itype.rs1]
                    & (unsigned) sign_extend_number(instruction.itype.imm, 12);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void execute_ecall(Processor *p, Byte *memory) {
    Register i;
    
    // syscall number is given by a0 (x10)
    // argument is given by a1
    switch(p->R[10]) {
        case 1: // print an integer
            printf("%d",p->R[11]);
            break;
        case 4: // print a string
            for(i=p->R[11];i<MEMORY_SPACE && load(memory,i,LENGTH_BYTE);i++) {
                printf("%c",load(memory,i,LENGTH_BYTE));
            }
            break;
        case 10: // exit
            printf("exiting the simulator\n");
            exit(0);
            break;
        case 11: // print a character
            printf("%c",p->R[11]);
            break;
        default: // undefined ecall
            printf("Illegal ecall number %d\n", p->R[10]);
            exit(-1);
            break;
    }
}

void execute_branch(Instruction instruction, Processor *processor) {
    switch (instruction.sbtype.funct3) {
        case 0x0:
            // BEQ
            if ((int) processor->R[instruction.sbtype.rs1] == (int) processor->R[instruction.sbtype.rs2])
                processor->PC += get_branch_offset(instruction);
            else
                processor->PC += 4;
            break;
        case 0x1:
            // BNE
            if ((int) processor->R[instruction.sbtype.rs1] != (int) processor->R[instruction.sbtype.rs2])
                processor->PC += get_branch_offset(instruction);
            else
                processor->PC += 4;
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
    switch (instruction.itype.funct3) {
        case 0x0:
            // LB
            processor->R[instruction.itype.rd]
                    = load(memory, processor->R[instruction.itype.rs1]
                                   + sign_extend_number(instruction.itype.imm, 12), LENGTH_BYTE);
            break;
        case 0x1:
            // LH
            processor->R[instruction.itype.rd]
                    = load(memory, processor->R[instruction.itype.rs1]
                                   + sign_extend_number(instruction.itype.imm, 12), LENGTH_HALF_WORD);
            break;
        case 0x2:
            // LW
            processor->R[instruction.itype.rd]
                    = load(memory, processor->R[instruction.itype.rs1]
                                   + sign_extend_number(instruction.itype.imm, 12), LENGTH_WORD);
            break;
        default:
            handle_invalid_instruction(instruction);
            break;
    }
}

void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
    switch (instruction.stype.funct3) {
        case 0x0:
            // SB
            store(memory, processor->R[instruction.stype.rs1]
                          + get_store_offset(instruction),
                  LENGTH_BYTE, processor->R[instruction.stype.rs2]);
            break;
        case 0x1:
            // SH
            store(memory, processor->R[instruction.stype.rs1]
                          + get_store_offset(instruction),
                  LENGTH_HALF_WORD, processor->R[instruction.stype.rs2]);
            break;
        case 0x2:
            // SW
            store(memory, processor->R[instruction.stype.rs1]
                          + get_store_offset(instruction),
                  LENGTH_WORD, processor->R[instruction.stype.rs2]);
            break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_jal(Instruction instruction, Processor *processor) {
    /* YOUR CODE HERE */
    processor->R[instruction.ujtype.rd] = processor->PC + 4;
    processor->PC += get_jump_offset(instruction);
}

void execute_lui(Instruction instruction, Processor *processor) {
    /* YOUR CODE HERE */
    processor->R[instruction.utype.rd] = instruction.utype.imm << 12;
}

void store(Byte *memory, Address address, Alignment alignment, Word value) {
    /* YOUR CODE HERE */
    if (alignment == LENGTH_WORD) {
        memory[address] = value & 0xFF;
        memory[address + 1] = (value >> 8) & 0xFF;
        memory[address + 2] = (value >> 16) & 0xFF;
        memory[address + 3] = (value >> 24) & 0xFF;
    } else if (alignment == LENGTH_HALF_WORD) {
        memory[address] = value & 0xFF;
        memory[address + 1] = (value >> 8) & 0xFF;
    } else if (alignment == LENGTH_BYTE) {
        memory[address] = value & 0xFF;
    }
}

Word load(Byte *memory, Address address, Alignment alignment) {
    /* YOUR CODE HERE */
    if (alignment == LENGTH_WORD) {
        uint32_t first = memory[address];
        uint32_t second = memory[address + 1] << 8;
        uint32_t third = memory[address + 2] << 16;
        uint32_t fourth = memory[address + 3] << 24;
        return fourth | third | second | first;
    } else if (alignment == LENGTH_HALF_WORD) {
        uint32_t first = memory[address];
        uint32_t second = memory[address + 1] << 8;
        return sign_extend_number(second | first, 16);
    } else if (alignment == LENGTH_BYTE) {
        uint32_t first = memory[address];
        return sign_extend_number(first, 8);
    }
    exit(-1);
}
