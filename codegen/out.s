.text
.globl main
main:
	######## preamble of func ########

	PUSHQ %rbp	# save the base pointer
	MOVQ %rsp, %rbp	# set the new base pointer to rsp

	PUSHQ %rdi	# save first argument on the stack
	PUSHQ %rsi	# save second argument on the stack
	PUSHQ %rdx	# save third argument on the stack
	PUSHQ %rcx	# save fourth argument on the stack
	PUSHQ %r8	# save fifth argument on the stack
	PUSHQ %r9	# save sixth argument on the stack

	PUSHQ %rbx	# save callee-saved registers
	PUSHQ %r12
	PUSHQ %r13
	PUSHQ %r14
	PUSHQ %r15

	######## body of the function starts here ########

.data
.STR0: .string "hello"
.text
	LEA .STR0, %rbx
	PUSHQ %rdi
	PUSHQ %r10
	PUSHQ %r11
	MOV %rbx, %rdi
	CALL print_string
	POPQ %r11
	POPQ %r10
	POPQ %rdi
.data
.STR1: .string "\n"
.text
	LEA .STR1, %rbx
	PUSHQ %rdi
	PUSHQ %r10
	PUSHQ %r11
	MOV %rbx, %rdi
	CALL print_string
	POPQ %r11
	POPQ %r10
	POPQ %rdi
	MOV $5, %rbx
	PUSHQ %rdi
	PUSHQ %r10
	PUSHQ %r11
	MOV %rbx, %rdi
	CALL print_integer
	POPQ %r11
	POPQ %r10
	POPQ %rdi
.data
.STR2: .string "\n"
.text
	LEA .STR2, %rbx
	PUSHQ %rdi
	PUSHQ %r10
	PUSHQ %r11
	MOV %rbx, %rdi
	CALL print_string
	POPQ %r11
	POPQ %r10
	POPQ %rdi
	MOV $1, %rbx
	PUSHQ %rdi
	PUSHQ %r10
	PUSHQ %r11
	MOV %rbx, %rdi
	CALL print_boolean
	POPQ %r11
	POPQ %r10
	POPQ %rdi
	MOV $10, %rbx
	PUSHQ %rdi
	PUSHQ %r10
	PUSHQ %r11
	MOV %rbx, %rdi
	CALL print_character
	POPQ %r11
	POPQ %r10
	POPQ %rdi
	MOV $5, %rbx
	MOV $3, %r10
	PUSHQ %rdi
	PUSHQ %rsi
	PUSHQ %r10
	PUSHQ %r11
	MOV %rbx, %rdi
	MOV %r10, %rsi
	CALL integer_power
	POPQ %r11
	POPQ %r10
	POPQ %rsi
	POPQ %rdi
	MOV %rax, %r10
	PUSHQ %rdi
	PUSHQ %r10
	PUSHQ %r11
	MOV %r10, %rdi
	CALL print_integer
	POPQ %r11
	POPQ %r10
	POPQ %rdi
	MOV $10, %rbx
	PUSHQ %rdi
	PUSHQ %r10
	PUSHQ %r11
	MOV %rbx, %rdi
	CALL print_character
	POPQ %r11
	POPQ %r10
	POPQ %rdi

	######## epilouge of function restores the stack ########

.RET0:
	POPQ %r15
	POPQ %r14
	POPQ %r13
	POPQ %r12
	POPQ %rbx

	MOVQ %rbp, %rsp	# reset stack to base pointer
	POPQ %rbp	# restore the old base pointer

	RET		# return to caller
