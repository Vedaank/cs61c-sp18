#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>

#include "utils.h"
#include "types.h"

void test_sign_extend_number();
void test_parse_instruction_rtype();
void test_parse_instruction_itype();
void test_parse_instruction_stype();
void test_parse_instruction_sbtype();
void test_parse_instruction_ujtype();
void test_parse_instruction_utype();

int main(int arc, char **argv) {
    CU_pSuite pSuite1 = NULL;

    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    } 

    pSuite1 = CU_add_suite("Testing sign_extend_number", NULL, NULL);
    if (!pSuite1) {
        goto exit;
    }

    if (!CU_add_test(pSuite1, "test_sign_extend_number", test_sign_extend_number)) {
        goto exit;
    }

    if (!CU_add_test(pSuite1, "test_parse_instruction_rtype", test_parse_instruction_rtype)) {
        goto exit;
    }

    if (!CU_add_test(pSuite1, "test_parse_instruction_itype", test_parse_instruction_itype)) {
        goto exit;
    }

    if (!CU_add_test(pSuite1, "test_parse_instruction_stype", test_parse_instruction_stype)) {
        goto exit;
    }

    if (!CU_add_test(pSuite1, "test_parse_instruction_sbtype", test_parse_instruction_sbtype)) {
        goto exit;
    }

    if (!CU_add_test(pSuite1, "test_parse_instruction_ujtype", test_parse_instruction_ujtype)) {
        goto exit;
    }

    if (!CU_add_test(pSuite1, "test_parse_instruction_utype", test_parse_instruction_utype)) {
        goto exit;
    }



    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    exit:
    CU_cleanup_registry();
    return CU_get_error();
}

void test_sign_extend_number() {
    CU_ASSERT_EQUAL(sign_extend_number(0xFF, 8), 0xFFFFFFFF);
    CU_ASSERT_EQUAL(sign_extend_number(0xFF, 9), 0xFF);
    CU_ASSERT_EQUAL(sign_extend_number(0x1234, 16), 0x1234);
    CU_ASSERT_EQUAL(sign_extend_number(0x1234, 13), 0xFFFFF234);
    CU_ASSERT_EQUAL(sign_extend_number(0x0,  1), 0);
    CU_ASSERT_EQUAL(sign_extend_number(0x1, 1), 0xFFFFFFFF);
}

void test_parse_instruction_rtype() {
    Instruction inst;
    inst = parse_instruction(0x009402b3);
    CU_ASSERT_EQUAL(inst.rtype.opcode, 0x33);
    CU_ASSERT_EQUAL(inst.rtype.funct3, 0x0);
    CU_ASSERT_EQUAL(inst.rtype.rd, 0x5);
    CU_ASSERT_EQUAL(inst.rtype.rs1, 0x8);
    CU_ASSERT_EQUAL(inst.rtype.rs2, 0x9);
    CU_ASSERT_EQUAL(inst.rtype.funct7, 0x0);
}

void test_parse_instruction_itype() {
    Instruction inst;
    inst = parse_instruction(0x00a50313);
    CU_ASSERT_EQUAL(inst.itype.opcode, 0x13);
    CU_ASSERT_EQUAL(inst.itype.funct3, 0x0);
    CU_ASSERT_EQUAL(inst.itype.rd, 0x6);
    CU_ASSERT_EQUAL(inst.itype.rs1, 10);
    CU_ASSERT_EQUAL(inst.itype.imm, 10);
}

void test_parse_instruction_stype() {
    Instruction inst;
    inst = parse_instruction(0x014aa023);
    CU_ASSERT_EQUAL(inst.stype.opcode, 0x23);
    CU_ASSERT_EQUAL(inst.stype.funct3, 0x2);
    CU_ASSERT_EQUAL(inst.stype.rs1, 21);
    CU_ASSERT_EQUAL(inst.stype.rs2, 20);
    CU_ASSERT_EQUAL(inst.stype.imm7, 0);
    CU_ASSERT_EQUAL(inst.stype.imm5, 0);
}

void test_parse_instruction_sbtype() {
    Instruction inst;
    inst = parse_instruction(0x00058063);
    CU_ASSERT_EQUAL(inst.sbtype.opcode, 0x63);
    CU_ASSERT_EQUAL(inst.sbtype.funct3, 0x0);
    CU_ASSERT_EQUAL(inst.sbtype.rs1, 11);
    CU_ASSERT_EQUAL(inst.sbtype.rs2, 0);
    CU_ASSERT_EQUAL(inst.sbtype.imm7, 0);
    CU_ASSERT_EQUAL(inst.sbtype.imm5, 0);
}

void test_parse_instruction_utype() {
    Instruction inst;
    inst = parse_instruction(0xFFFFF437);
    CU_ASSERT_EQUAL(inst.utype.opcode, 0x37);
    CU_ASSERT_EQUAL(inst.utype.rd, 8);
    CU_ASSERT_EQUAL(inst.utype.imm, 0xFFFFF);
}

void test_parse_instruction_ujtype() {
    Instruction inst;
    inst = parse_instruction(0x000000EF);
    CU_ASSERT_EQUAL(inst.ujtype.opcode, 0x6F);
    CU_ASSERT_EQUAL(inst.ujtype.rd, 1);
    CU_ASSERT_EQUAL(inst.ujtype.imm, 0);
}
