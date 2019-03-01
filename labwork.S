  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  # Please keep the two macros PUSH and POP unchanged
  
.global delay
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg, 0($sp)
.endm

.macro	POP reg
	lw	\reg, 0($sp)
	addi	$sp, $sp, 4
.endm

  # Please add your own code below this line
  
.data
.align 2
.text

main:


delay:  slt $t1, $0, $a0 #conditional for while loop
	beqz $t1, exit #branches out if ms <= 0
	nop
	addi $a0, $a0, -1
	add $t2, $0, $0 #for loop counter i=0
for:    slti $t3, $t2, 3500
	beqz $t3, delay
	nop
	addi $t2, $t2, 1 #i++
	j    for
	nop	
exit:	jr $ra
	nop

