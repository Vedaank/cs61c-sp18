SOURCES := utils.c part1.c part2.c riscv.c
HEADERS := types.h utils.h riscv.h
CUNIT := -L/home/ff/cs61c/cunit/install/lib -I/home/ff/cs61c/cunit/install/include -lcunit
CFLAGS := -g -std=gnu99 -Wall


ASM_TESTS := simple multiply random

all: riscv part1 part2
	@echo "=============All tests finished============="

.PHONY: part1 %_disasm

riscv: $(SOURCES) $(HEADERS) out
	gcc $(CFLAGS) -o $@ $(SOURCES)

out:
	@mkdir -p ./riscvcode/out

# Part 1 Tests

part1: riscv $(addsuffix _disasm, $(ASM_TESTS))
	@echo "---------Disassembly Tests Complete---------"

%_disasm: riscvcode/code/%.input riscvcode/ref/%.solution riscv
	@./riscv -d $< > riscvcode/out/test.dump
	@diff $(word 2, $^) riscvcode/out/test.dump && echo "$@ TEST PASSED!" || echo "$@ TEST FAILED!"

part2: riscv $(addsuffix _execute, $(ASM_TESTS))
	@echo "-----------Execute Tests Complete-----------"

%_execute: riscvcode/code/%.input riscvcode/ref/%.solution riscv
	@./riscv -r $< > riscvcode/out/$*.trace
	@python2.7 part2_tester.py $*  	 	  

test-utils:
	gcc $(CFLAGS) -DTESTING -o test-utils test_utils.c utils.c $(CUNIT)
	./test-utils
	rm -f test-utils

clean:
	rm -f riscv
	rm -f *.o
	rm -f test-utils
	rm -rf riscvcode/out
