  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #

.global delay
.global time2string
.global enable_interrupt

enable_interrupt:
    ei 
    jr $ra
    nop


hexasc:
        slti    $t0,$a0,10    # 0 if larger than 10
        beq     $zero,$t0,ltrs  # branch if input 10-15 (Letter)

        addi    $v0,$a0,48      #if num 0-9
        j end
        nop
ltrs:
        addi    $v0,$a0,55      #else num 10-15
end:
        jr $ra
        nop


delay:
        li $t0,9000
delay_loop:
        li $t1,0

delay_nestedloop:
        addi $t1,$t1,1
        bne $t1,$t0,delay_nestedloop
        nop

        addi $a0,$a0,-1
        bne $zero, $a0,delay_loop
        nop

        jr $ra
        nop

 time2string:
        PUSH $ra        # contains return address
        PUSH $s0        # contains memory address
        PUSH $s1        # contains incrementer
        PUSH $s2        # contains max increments (4)
        PUSH $s3        # contains time-info

        addi $s0,$a0,0
        li $s1,0
        li $s2,4
        addi $s3,$a1,0

time2string_loop:
        # insert arguments for hexasc
        andi $a0,$s3,0xF        # record least significant nibble
        sra $s3,$s3,4

        jal hexasc
        nop

        sb $v0,2($s0)           # add to memory address
        addi $s0,$s0,-1         # increment by 1 byte

        addi $s1,$s1,1
time2string_colon:
        bne $s1,$s2,time2string_loop
        nop

        li $t1,0x00
        sb $t1,7($s0)          # insert null-ASCII

        POP $s3
        POP $s2
        POP $s1
        POP $s0
        POP $ra

        jr $ra
        nop
