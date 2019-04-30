addi t0 x0 12
add t1 t0 x0
# swge t1 (t0) 7  ## NOTE swge cannot be assembled by Venus 
lw s0 0(t0)
addi t1 x0 -12
# swge t1 (t0) 7
lw s0 0(t0)
