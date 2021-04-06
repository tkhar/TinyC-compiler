	.globl f
	.type f, @function
f:
	pushl %ebp
	movl %esp, %ebp
	addl $-4, %esp
	jmp .B0
.B0:
	movl 8(%ebp), %ebx
	movl $0, %ecx
	subl %ebx, %ecx
	movl %ecx, -4(%ebp)
	movl -4(%ebp), %ebx
	movl %ebx, %eax
	movl %ebp, %esp
	popl %ebp
	ret
