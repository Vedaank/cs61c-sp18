.data
n: .word 8

.text
main:
    la t0, n
    lw a0, 0(t0)
    jal ra, factorial
    
    addi a1, a0, 0
    addi a0, x0, 1
    ecall # Print Result
    
    addi a0, x0, 10
    ecall # Exit

factorial:
    # YOUR CODE HERE
    addi t0, x0, 1
recursion:
    beq a0, x0, finish
    mul t0, t0, a0
    addi a0, a0, -1
    j recursion
finish:
    mv a0, t0
    jr ra
