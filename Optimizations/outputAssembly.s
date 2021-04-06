	.globl f
	.type f, @function
f:
	pushl %ebp
	movl %esp, %ebp
	addl $-8, %esp
	jmp .B0
.B0:
	movl $0, -4(%ebp)
	jmp .B1
.B1:
	movl -4(%ebp), %ebx
	movl %ebx, %ecx
	cmp $10, %ecx
	jl .B2
	jmp .B3
.B2:
	movl -4(%ebp), %ebx
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %ebx
	call print
	addl $4, %esp
	popl %edx
	popl %ecx
	popl %ebx
	movl %ebx, %ecx
	addl $1, %ecx
	movl %ecx, -4(%ebp)
	jmp .B1
.B3:
	movl $1, %eax
	movl %ebp, %esp
	popl %ebp
	ret
