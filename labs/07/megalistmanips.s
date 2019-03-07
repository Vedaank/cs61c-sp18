.data
arrays:		.word	5, 6, 7, 8, 9
		.word	1, 2, 3, 4, 7
		.word	5, 2, 7, 4, 3
		.word	1, 6, 3, 8, 4
		.word	5, 2, 7, 8, 1
		
start_msg:	.asciiz "Lists before: \n"
end_msg:	.asciiz "Lists after: \n"

.text
main:	jal	create_default_list
	addu	$s0, $v0, $0	# $v0 = $s0 is head of node list

	#print "lists before: "
	la	$a0, start_msg
	li	$v0, 4
	syscall

	#print the list
	addu	$a0, $s0, $0
	jal	print_list

	# print a newline
	jal	print_newline

	#issue the map call
	addu	$a0, $s0, $0	# load the address of the first node into $a0
	la 	$a1, mystery	# load the address of the function into $a1

	jal	map

	# print "lists after: "
	la	$a0, end_msg
	li	$v0, 4
	syscall

	#print the list
	addu	$a0, $s0, $0
	jal	print_list

	li	$v0, 10
	syscall

map:
	addiu	$sp, $sp, -12
	sw	$ra, 0($sp)
	sw	$s1, 4($sp)
	sw	$s0, 8($sp)

	beq	$a0, $0, done	# if we were given a null pointer, we're done.

	addu	$s0, $a0, $0	# save address of this node in $s0
	addu	$s1, $a1, $0	# save address of function in $s1
	addu	$t0, $0, $0	# t0 is a counter

	# remember that each node is 12 bytes long: 4 for the array pointer, 4 for the size of the array, and 4 more for the pointer to the next node
mapLoop:
	addu	$t1, $s0, $0		# load the address of the array of current node into $t1
	lw	$t2, 4($s0)		# load the size of the node's array into $t2
	
	addu	$t1, $t1, $t0		# offset the array address by the count
	lw	$a0, 0($t1)		# load the value at that address into $a0
	
	jalr	$s1			# call the function on that value.
	
	sw	$v0, 0($t1)		# store the returned value back into the array
	addiu	$t0, $t0, 1		# increment the count
	bne	$t0, $t2, mapLoop	# repeat if we haven't reached the array size yet
	
	la	$a0, 8($s0)		# load the address of the next node into $a0
	lw	$a1, 0($s1)		# put the address of the function back into $a1 to prepare for the recursion
	
	jal 	map			# recurse

done:
	lw	$s0, 8($sp)
	lw	$s1, 4($sp)
	lw	$ra, 0($sp)
	addiu	$sp, $sp, 12
	jr	$ra

mystery:
	mul	$t1, $a0, $a0
	addu	$v0, $t1, $a0
	jr	$ra

create_default_list:
	addiu	$sp, $sp, -4
	sw	$ra, 0($sp)
	li	$s0, 0		# pointer to the last node we handled
	li	$s1, 0		# number of nodes handled
	li	$s2, 5		# size
	la	$s3, arrays	
loop:	#do...
	li	$a0, 12
	jal	malloc		# get memory for the next node
	move	$s4, $v0
	li	$a0, 20		
	jal 	malloc		# get memory for this array
	
	sw	$v0, 0($s4)	# node->arr = malloc
	lw	$a0, 0($s4)
	move	$a1, $s3
	jal	fillArray	# copy ints over to node->arr
	
	sw	$s2, 4($s4)	# node->size = size (4)
	sw 	$s0, 8($s4)     # node-> next = previously created node
	
	addu	$s0, $0, $s4	# last = node
	addiu	$s1, $s1, 1	# i++
	addiu	$s3, $s3, 20	# s3 points at next set of ints
	bne	$s1, 5, loop	# ... while i!= 5
	move	$v0, $s4
	lw	$ra, 0($sp)
	addiu	$sp, $sp, 4
	jr	$ra
	
fillArray:	lw	$t0, 0($a1)	#t0 gets array element
	sw	$t0, 0($a0)	#node->arr gets array element
	lw	$t0, 4($a1)
	sw	$t0, 4($a0)
	lw	$t0, 8($a1)
	sw	$t0, 8($a0)
	lw	$t0, 12($a1)
	sw	$t0, 12($a0)
	lw	$t0, 16($a1)
	sw	$t0, 16($a0)
	jr	$ra

print_list:
	bne	$a0, $0, printMeAndRecurse
	jr	$ra 		# nothing to print
printMeAndRecurse:
	move	$t0, $a0	# t0 gets address of current node
	lw	$t3, 0($a0)	# t3 gets array of current node
	li	$t1, 0		# t1 is index into array
printLoop:
	sll	$t2, $t1, 2
	addu	$t4, $t3, $t2
	lw	$a0, 0($t4)	# a0 gets value in current node's array at index t1
	li	$v0, 1		# preparte for print integer syscall
	syscall
	li	$a0, ' '	# a0 gets address of string containing space
	li	$v0, 11		# prepare for print string syscall
	syscall
	addiu	$t1, $t1, 1
	bne	$t1, 5, printLoop
	li	$a0, '\n'	
	li	$v0, 11
	syscall
	lw	$a0, 8($t0)	# a0 gets address of next node
	j	print_list	# recurse. We don't have to use jal because we already have where we want to return to in $ra

print_newline:
	li	$a0, '\n'
	li	$v0, 11
	syscall
	jr	$ra

malloc:
	li	$v0, 9
	syscall
	jr	$ra
