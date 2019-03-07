.data
neg3:	.asciiz "f(-3) should be 6, and it is: "
neg2:	.asciiz "f(-2) should be 61, and it is: "
neg1:	.asciiz "f(-1) should be 17, and it is: "
zero:	.asciiz "f(0) should be -38, and it is: "
pos1:	.asciiz "f(1) should be 19, and it is: "
pos2:	.asciiz "f(2) should be 42, and it is: "
pos3:	.asciiz "f(3) should be 5, and it is: "

output:	.word	6, 61, 17, -38, 19, 42, 5
.text
main:
	la	$a0, neg3
	jal	print_str
	li	$a0, -3
	jal	f		# evaluate f(-3); should be 6
	move	$a0, $v0
	jal	print_int
	jal	print_newline
	
	la	$a0, neg2
	jal	print_str
	li	$a0, -2
	jal	f		# evaluate f(-2); should be 61
	move	$a0, $v0
	jal	print_int
	jal	print_newline
	
	la	$a0, neg1
	jal	print_str
	li	$a0, -1
	jal	f		# evaluate f(-1); should be 17
	move	$a0, $v0
	jal	print_int
	jal	print_newline
	
	la	$a0, zero
	jal	print_str
	li	$a0, 0
	jal	f		# evaluate f(0); should be -38
	move	$a0, $v0
	jal	print_int
	jal	print_newline
	
	la	$a0, pos1
	jal	print_str
	li	$a0, 1
	jal	f		# evaluate f(1); should be 19
	move	$a0, $v0
	jal	print_int
	jal	print_newline
	
	la	$a0, pos2
	jal	print_str
	li	$a0, 2
	jal	f		# evaluate f(2); should be 42
	move	$a0, $v0
	jal	print_int
	jal	print_newline
	
	la	$a0, pos3
	jal	print_str
	li	$a0, 3
	jal	f		# evaluate C(4,0); should be 5
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	li	$v0, 10
	syscall

# calculate f($a0)
f:
	la	$t0, output	# Hmm... why might this be a good idea?
	
	# YOUR CODE GOES HERE!
	
	jr	$ra		# Always remember to jr $ra after your function!
  
print_int:
	li	$v0, 1
	syscall
	jr	$ra

print_str:
	li	$v0, 4
	syscall
	jr	$ra
	
print_newline:
	li	$a0, '\n'
	li	$v0, 11
	syscall
	jr	$ra
