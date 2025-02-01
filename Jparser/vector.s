	.file	"vector.c"
	.text
.Ltext0:
	.file 1 "vector.c"
	.type	VEC_create, @function
VEC_create:
.LFB9:
	.loc 1 110 42
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	.loc 1 114 8
	cmpq	$0, -40(%rbp)
	jne	.L2
	.loc 1 115 10
	movq	$1, -40(%rbp)
.L2:
	movq	-40(%rbp), %rax
	movq	%rax, -8(%rbp)
.LBB20:
.LBB21:
	.loc 1 105 32
	cmpq	$255, -8(%rbp)
	jbe	.L3
	.loc 1 105 32 is_stmt 0 discriminator 1
	cmpq	$65535, -8(%rbp)
	jbe	.L4
	.loc 1 105 32 discriminator 3
	movl	$4294967295, %eax
	cmpq	-8(%rbp), %rax
	jnb	.L5
	.loc 1 105 32 discriminator 5
	movl	$4, %eax
	.loc 1 105 32
	jmp	.L14
.L5:
	.loc 1 105 32 discriminator 6
	movl	$3, %eax
	.loc 1 105 32
	jmp	.L14
.L4:
	.loc 1 105 32 discriminator 4
	movl	$2, %eax
	.loc 1 105 32
	jmp	.L14
.L3:
	.loc 1 105 32 discriminator 2
	movl	$1, %eax
.L14:
.LBE21:
.LBE20:
	.loc 1 117 11 is_stmt 1 discriminator 1
	andl	$15, %eax
	.loc 1 117 9 discriminator 1
	addl	$1, %eax
	movb	%al, -17(%rbp)
	.loc 1 119 44
	movq	-40(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	.loc 1 119 18
	movzbl	-17(%rbp), %eax
	addq	%rdx, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, -16(%rbp)
	.loc 1 119 8 discriminator 1
	cmpq	$0, -16(%rbp)
	jne	.L10
	.loc 1 120 9
	movl	$0, %eax
	jmp	.L11
.L10:
	.loc 1 122 5
	subb	$1, -17(%rbp)
	movzbl	-17(%rbp), %edx
	movq	-16(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	.loc 1 125 80
	cmpq	$1, -40(%rbp)
	jbe	.L12
	.loc 1 125 80 is_stmt 0 discriminator 1
	movl	$-64, %edx
	jmp	.L13
.L12:
	.loc 1 125 80 discriminator 2
	movl	$-128, %edx
.L13:
	.loc 1 125 80 discriminator 4
	movzbl	-17(%rbp), %eax
	movl	%edx, %ecx
	orl	%eax, %ecx
	.loc 1 125 20 is_stmt 1 discriminator 4
	movzbl	-17(%rbp), %edx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	.loc 1 125 80 discriminator 4
	movl	%ecx, %edx
	.loc 1 125 26 discriminator 4
	movb	%dl, (%rax)
	.loc 1 127 5
	movzbl	-17(%rbp), %eax
	andl	$15, %eax
	addq	$1, %rax
	addq	%rax, -16(%rbp)
	.loc 1 129 19
	movq	-16(%rbp), %rax
	movq	$0, (%rax)
	.loc 1 131 12
	movq	-16(%rbp), %rax
.L11:
	.loc 1 132 1
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	VEC_create, .-VEC_create
	.type	VEC_append, @function
VEC_append:
.LFB10:
	.loc 1 137 103
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$56, %rsp
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	.cfi_offset 3, -40
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%rdx, -72(%rbp)
	movl	%ecx, %eax
	movb	%al, -76(%rbp)
	.loc 1 142 8
	movl	$0, %r12d
	.loc 1 143 8
	movzbl	-76(%rbp), %eax
	andl	$15, %eax
	movl	%eax, %ebx
	.loc 1 146 13
	movq	-72(%rbp), %rax
	leaq	1(%rax), %rdx
	.loc 1 146 26
	movzbl	%bl, %eax
	movl	$4, %esi
	movl	%eax, %ecx
	salq	%cl, %rsi
	movq	%rsi, %rax
	.loc 1 146 18
	movl	%eax, %ecx
	shrq	%cl, %rdx
	movq	%rdx, %rax
	.loc 1 146 8
	testq	%rax, %rax
	je	.L16
	.loc 1 147 36
	movl	%ebx, %eax
	notl	%eax
	movl	%eax, %edx
	movzbl	-76(%rbp), %eax
	andl	%eax, %edx
	.loc 1 147 49
	leal	1(%rbx), %eax
	.loc 1 147 43
	orl	%eax, %edx
	.loc 1 147 3
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	.loc 1 147 24
	subq	$1, %rax
	.loc 1 147 28
	movb	%dl, (%rax)
	.loc 1 148 5
	movl	$1, %r12d
.L16:
	.loc 1 151 38
	movq	-72(%rbp), %rax
	addq	$1, %rax
	.loc 1 151 32
	leaq	0(,%rax,8), %rdx
	.loc 1 151 57
	movzbl	-76(%rbp), %eax
	andl	$15, %eax
	addl	$1, %eax
	cltq
	.loc 1 151 11
	leaq	(%rdx,%rax), %r13
	.loc 1 153 10
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	.loc 1 153 95
	testq	%rax, %rax
	je	.L17
	.loc 1 153 75 discriminator 1
	testb	%r12b, %r12b
	jne	.L18
	.loc 1 153 39 discriminator 3
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	movzbl	-76(%rbp), %edx
	andl	$15, %edx
	notq	%rdx
	.loc 1 153 31 discriminator 3
	addq	%rdx, %rax
	movq	%r13, %rsi
	movq	%rax, %rdi
	call	realloc@PLT
	jmp	.L20
.L18:
	.loc 1 153 77 discriminator 4
	leaq	1(%r13), %rax
	movq	%rax, %rdi
	call	malloc@PLT
	jmp	.L20
.L17:
	.loc 1 153 95 discriminator 2
	movl	$0, %eax
.L20:
	.loc 1 153 8 discriminator 8
	movq	%rax, -40(%rbp)
	.loc 1 155 8
	cmpq	$0, -40(%rbp)
	jne	.L21
	.loc 1 156 9
	movl	$0, %eax
	jmp	.L22
.L21:
	.loc 1 158 8
	testb	%r12b, %r12b
	je	.L23
	.loc 1 160 2
	movzbl	%bl, %edx
	leaq	-72(%rbp), %rcx
	movq	-40(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	.loc 1 161 23
	movzbl	%bl, %edx
	movq	-40(%rbp), %rax
	addq	%rax, %rdx
	.loc 1 161 27
	movzbl	-76(%rbp), %eax
	movb	%al, (%rdx)
	.loc 1 166 2
	movq	-72(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	.loc 1 166 19
	movzbl	-76(%rbp), %ecx
	leaq	1(%rcx), %rsi
	.loc 1 166 2
	movq	-40(%rbp), %rcx
	addq	%rsi, %rcx
	movq	%rax, %rsi
	movq	%rcx, %rdi
	call	memcpy@PLT
	.loc 1 167 7
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	movzbl	-76(%rbp), %edx
	andl	$15, %edx
	notq	%rdx
	.loc 1 167 2
	addq	%rdx, %rax
	movq	%rax, %rdi
	call	free@PLT
.L23:
	.loc 1 170 5
	movzbl	-76(%rbp), %eax
	andl	$15, %eax
	cmpl	$4, %eax
	je	.L24
	cmpl	$4, %eax
	jg	.L25
	cmpl	$3, %eax
	je	.L26
	cmpl	$3, %eax
	jg	.L25
	cmpl	$1, %eax
	je	.L27
	cmpl	$2, %eax
	je	.L28
	jmp	.L25
.L27:
	.loc 1 170 5 is_stmt 0 discriminator 1
	movq	-40(%rbp), %rax
	movzbl	(%rax), %eax
	leal	1(%rax), %edx
	movq	-40(%rbp), %rax
	movb	%dl, (%rax)
	.loc 1 170 5
	jmp	.L25
.L28:
	.loc 1 170 5 discriminator 2
	movq	-40(%rbp), %rax
	movzwl	(%rax), %eax
	leal	1(%rax), %edx
	movq	-40(%rbp), %rax
	movw	%dx, (%rax)
	.loc 1 170 5
	jmp	.L25
.L26:
	.loc 1 170 5 discriminator 3
	movq	-40(%rbp), %rax
	movl	(%rax), %eax
	leal	1(%rax), %edx
	movq	-40(%rbp), %rax
	movl	%edx, (%rax)
	.loc 1 170 5
	jmp	.L25
.L24:
	.loc 1 170 5 discriminator 4
	movq	-40(%rbp), %rax
	movq	(%rax), %rax
	leaq	1(%rax), %rdx
	movq	-40(%rbp), %rax
	movq	%rdx, (%rax)
.L25:
	.loc 1 172 41 is_stmt 1
	movzbl	%bl, %eax
	leaq	1(%rax), %rdx
	.loc 1 172 12
	movq	-40(%rbp), %rax
	addq	%rax, %rdx
	.loc 1 172 10
	movq	-56(%rbp), %rax
	movq	%rdx, (%rax)
	.loc 1 173 6
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	.loc 1 173 11
	movq	-72(%rbp), %rdx
	salq	$3, %rdx
	addq	%rax, %rdx
	.loc 1 173 16
	movq	-64(%rbp), %rax
	movq	%rax, (%rdx)
	.loc 1 175 12
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
.L22:
	.loc 1 176 1
	addq	$56, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	VEC_append, .-VEC_append
	.type	VEC_expand, @function
VEC_expand:
.LFB11:
	.loc 1 181 101
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$88, %rsp
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	.cfi_offset 3, -40
	movq	%rdi, -88(%rbp)
	movq	%rsi, -96(%rbp)
	movq	%rdx, -104(%rbp)
	movq	%rcx, -112(%rbp)
	.loc 1 181 101
	movq	%fs:40, %rax
	movq	%rax, -40(%rbp)
	xorl	%eax, %eax
	.loc 1 184 9
	movq	$0, -72(%rbp)
	.loc 1 184 16
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -56(%rbp)
	leaq	-72(%rbp), %rax
	movq	%rax, -48(%rbp)
.LBB22:
.LBB23:
	.loc 1 80 7
	movq	-56(%rbp), %rbx
	.loc 1 80 16
	movq	-48(%rbp), %r13
	.loc 1 82 7
	subq	$1, %rbx
	.loc 1 82 9
	movzbl	(%rbx), %eax
	.loc 1 82 7
	andl	$15, %eax
	movl	%eax, %r12d
	.loc 1 83 7
	movzbl	%r12b, %eax
	negq	%rax
	addq	%rax, %rbx
	.loc 1 85 11
	jmp	.L30
.L31:
	.loc 1 86 11
	movq	%rbx, %rdx
	leaq	1(%rdx), %rbx
	.loc 1 86 4
	movq	%r13, %rax
	leaq	1(%rax), %r13
	.loc 1 86 9
	movzbl	(%rdx), %edx
	.loc 1 86 7
	movb	%dl, (%rax)
.L30:
	.loc 1 85 14
	movl	%r12d, %eax
	leal	-1(%rax), %r12d
	.loc 1 85 13
	testb	%al, %al
	jne	.L31
.LBE23:
.LBE22:
	.loc 1 186 11
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	.loc 1 186 32
	subq	$1, %rax
	movzbl	(%rax), %eax
	.loc 1 186 8
	movzbl	%al, %eax
	movq	%rax, -64(%rbp)
	.loc 1 188 12
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	.loc 1 188 17
	movq	(%rax), %rax
	.loc 1 188 8
	testq	%rax, %rax
	jne	.L33
	.loc 1 190 5
	cmpq	$0, -104(%rbp)
	jne	.L47
	.loc 1 190 19 discriminator 1
	cmpq	$222, -112(%rbp)
	jne	.L47
	.loc 1 196 3
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	.loc 1 196 8
	movq	-104(%rbp), %rdx
	salq	$3, %rdx
	addq	%rax, %rdx
	.loc 1 196 16
	movq	-96(%rbp), %rax
	movq	%rax, (%rdx)
	.loc 1 198 2
	movq	-64(%rbp), %rax
	andl	$15, %eax
	cmpq	$4, %rax
	je	.L37
	cmpq	$4, %rax
	ja	.L42
	cmpq	$3, %rax
	je	.L39
	cmpq	$3, %rax
	ja	.L42
	cmpq	$1, %rax
	je	.L40
	cmpq	$2, %rax
	je	.L41
	jmp	.L42
.L40:
	.loc 1 198 2 is_stmt 0 discriminator 1
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	-64(%rbp), %rdx
	andl	$15, %edx
	notq	%rdx
	addq	%rdx, %rax
	movzbl	(%rax), %edx
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	-64(%rbp), %rcx
	andl	$15, %ecx
	notq	%rcx
	addq	%rcx, %rax
	addl	$1, %edx
	movb	%dl, (%rax)
	.loc 1 198 2
	jmp	.L42
.L41:
	.loc 1 198 2 discriminator 2
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	-64(%rbp), %rdx
	andl	$15, %edx
	notq	%rdx
	addq	%rdx, %rax
	movzwl	(%rax), %edx
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	-64(%rbp), %rcx
	andl	$15, %ecx
	notq	%rcx
	addq	%rcx, %rax
	addl	$1, %edx
	movw	%dx, (%rax)
	.loc 1 198 2
.L39:
	.loc 1 198 2 discriminator 3
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	-64(%rbp), %rdx
	andl	$15, %edx
	notq	%rdx
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	-64(%rbp), %rcx
	andl	$15, %ecx
	notq	%rcx
	addq	%rcx, %rax
	addl	$1, %edx
	movl	%edx, (%rax)
	.loc 1 198 2
	jmp	.L42
.L37:
	.loc 1 198 2 discriminator 4
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	-64(%rbp), %rdx
	andl	$15, %edx
	notq	%rdx
	addq	%rdx, %rax
	movq	(%rax), %rdx
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	movq	-64(%rbp), %rcx
	andl	$15, %ecx
	notq	%rcx
	addq	%rcx, %rax
	addq	$1, %rdx
	movq	%rdx, (%rax)
	.loc 1 198 2
	jmp	.L42
.L33:
	.loc 1 200 20 is_stmt 1
	movq	-72(%rbp), %rax
	.loc 1 200 13
	cmpq	%rax, -104(%rbp)
	jnb	.L43
	.loc 1 201 3
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	.loc 1 201 8
	movq	-104(%rbp), %rdx
	salq	$3, %rdx
	addq	%rax, %rdx
	.loc 1 201 16
	movq	-96(%rbp), %rax
	movq	%rax, (%rdx)
	jmp	.L42
.L43:
	.loc 1 203 24
	movq	-112(%rbp), %rax
	andl	$222, %eax
	.loc 1 203 13
	testq	%rax, %rax
	je	.L48
	.loc 1 203 41 discriminator 1
	movq	-64(%rbp), %rax
	movzbl	%al, %ecx
	movq	-72(%rbp), %rdx
	movq	-96(%rbp), %rsi
	movq	-88(%rbp), %rax
	movq	%rax, %rdi
	call	VEC_append
	.loc 1 203 14 discriminator 1
	testq	%rax, %rax
	jne	.L42
	.loc 1 205 5
	jmp	.L48
.L47:
	.loc 1 192 6
	nop
	jmp	.L36
.L48:
	.loc 1 205 5
	nop
.L36:
	.loc 1 209 9
	movl	$0, %eax
	jmp	.L45
.L42:
	.loc 1 211 12
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
.L45:
	.loc 1 212 1
	movq	-40(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L46
	call	__stack_chk_fail@PLT
.L46:
	addq	$88, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	VEC_expand, .-VEC_expand
	.type	VEC_add, @function
VEC_add:
.LFB12:
	.loc 1 214 110
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%rdx, -72(%rbp)
	movq	%rcx, -80(%rbp)
	movq	%r8, -88(%rbp)
	movl	%r9d, %eax
	movb	%al, -92(%rbp)
	.loc 1 219 12
	movq	$1, -24(%rbp)
	.loc 1 221 22
	movq	-80(%rbp), %rax
	movq	%rax, -8(%rbp)
.LBB24:
.LBB25:
	.loc 1 105 32
	cmpq	$255, -8(%rbp)
	jbe	.L50
	.loc 1 105 32 is_stmt 0 discriminator 1
	cmpq	$65535, -8(%rbp)
	jbe	.L51
	.loc 1 105 32 discriminator 3
	movl	$4294967295, %eax
	cmpq	-8(%rbp), %rax
	jnb	.L52
	.loc 1 105 32 discriminator 5
	movl	$4, %eax
	.loc 1 105 32
	jmp	.L68
.L52:
	.loc 1 105 32 discriminator 6
	movl	$3, %eax
	.loc 1 105 32
	jmp	.L68
.L51:
	.loc 1 105 32 discriminator 4
	movl	$2, %eax
	.loc 1 105 32
	jmp	.L68
.L50:
	.loc 1 105 32 discriminator 2
	movl	$1, %eax
.L68:
.LBE25:
.LBE24:
	.loc 1 221 22 is_stmt 1
	movb	%al, -33(%rbp)
	.loc 1 222 21
	movq	-80(%rbp), %rax
	.loc 1 222 12
	movq	-72(%rbp), %rdx
	imulq	%rdx, %rax
	movq	%rax, -16(%rbp)
	.loc 1 224 9
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	.loc 1 224 8
	testq	%rax, %rax
	je	.L57
	.loc 1 224 22 discriminator 1
	cmpq	$0, -16(%rbp)
	je	.L57
	.loc 1 225 50
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	addl	$1, %eax
	movslq	%eax, %rdx
	.loc 1 225 34
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, -32(%rbp)
	.loc 1 225 2 discriminator 1
	cmpq	$0, -32(%rbp)
	jne	.L58
.L57:
	.loc 1 226 9
	movl	$0, %eax
	jmp	.L59
.L58:
	.loc 1 228 27
	movzbl	-33(%rbp), %eax
	.loc 1 228 5
	andl	$15, %eax
	movq	%rax, %rdx
	leaq	-80(%rbp), %rcx
	movq	-32(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	.loc 1 229 5
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	addq	$1, %rax
	addq	%rax, -32(%rbp)
	.loc 1 230 28
	movq	-32(%rbp), %rax
	leaq	-1(%rax), %rdx
	.loc 1 230 32
	movzbl	-33(%rbp), %eax
	movb	%al, (%rdx)
	.loc 1 232 5
	movq	-16(%rbp), %rdx
	movq	-64(%rbp), %rcx
	movq	-32(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	.loc 1 235 9
	movzbl	-92(%rbp), %eax
	.loc 1 235 8
	testb	%al, %al
	jns	.L60
	.loc 1 235 42 discriminator 1
	movl	$1, %edi
	call	VEC_create
	movq	%rax, -24(%rbp)
	.loc 1 235 29 discriminator 1
	cmpq	$0, -24(%rbp)
	je	.L60
	.loc 1 237 10
	movq	-24(%rbp), %rax
	movq	-32(%rbp), %rdx
	movq	%rdx, (%rax)
	.loc 1 238 5
	movq	-24(%rbp), %rax
	movzbl	-1(%rax), %eax
	movb	%al, -33(%rbp)
	.loc 1 239 2
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	cmpl	$4, %eax
	je	.L61
	cmpl	$4, %eax
	jg	.L62
	cmpl	$3, %eax
	je	.L63
	cmpl	$3, %eax
	jg	.L62
	cmpl	$1, %eax
	je	.L64
	cmpl	$2, %eax
	je	.L65
	jmp	.L62
.L64:
	.loc 1 239 2 is_stmt 0 discriminator 1
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	notq	%rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %edx
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	notq	%rax
	movq	%rax, %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	$1, %edx
	movb	%dl, (%rax)
	.loc 1 239 2
	jmp	.L62
.L65:
	.loc 1 239 2 discriminator 2
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	notq	%rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movzwl	(%rax), %edx
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	notq	%rax
	movq	%rax, %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	$1, %edx
	movw	%dx, (%rax)
	.loc 1 239 2
	jmp	.L62
.L63:
	.loc 1 239 2 discriminator 3
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	notq	%rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	notq	%rax
	movq	%rax, %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	$1, %edx
	movl	%edx, (%rax)
	.loc 1 239 2
	jmp	.L62
.L61:
	.loc 1 239 2 discriminator 4
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	notq	%rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rdx
	movzbl	-33(%rbp), %eax
	andl	$15, %eax
	notq	%rax
	movq	%rax, %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addq	$1, %rdx
	movq	%rdx, (%rax)
.L62:
	.loc 1 241 8 is_stmt 1
	movq	-24(%rbp), %rax
	movq	%rax, -32(%rbp)
.L60:
	.loc 1 243 72
	cmpq	$0, -24(%rbp)
	je	.L66
	.loc 1 243 22 discriminator 1
	movq	-88(%rbp), %rdx
	movq	-32(%rbp), %rsi
	movq	-56(%rbp), %rax
	movl	$222, %ecx
	movq	%rax, %rdi
	call	VEC_expand
	.loc 1 243 19 discriminator 1
	testq	%rax, %rax
	je	.L66
	.loc 1 243 72 discriminator 3
	movq	-32(%rbp), %rax
	.loc 1 243 72 is_stmt 0
	jmp	.L59
.L66:
	.loc 1 243 72 discriminator 4
	movl	$0, %eax
.L59:
	.loc 1 244 1 is_stmt 1
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	VEC_add, .-VEC_add
	.type	VEC_remove, @function
VEC_remove:
.LFB16:
	.loc 1 269 65
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	.loc 1 270 1
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16:
	.size	VEC_remove, .-VEC_remove
	.section	.rodata
.LC0:
	.string	"forbidden"
.LC1:
	.string	"(...) lTmp %p, lCurrt %p\n"
.LC2:
	.string	"(old) fl %u, ul %u\n"
.LC3:
	.string	"(new) fl %u, ul %u\n"
	.text
	.type	VEC_internalDelete, @function
VEC_internalDelete:
.LFB18:
	.loc 1 297 56
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$200, %rsp
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rdi, -232(%rbp)
	.loc 1 297 56
	movq	%fs:40, %rax
	movq	%rax, -56(%rbp)
	xorl	%eax, %eax
	.loc 1 302 9
	movl	$0, -212(%rbp)
	.loc 1 303 11
	movq	-232(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -176(%rbp)
	leaq	-208(%rbp), %rax
	movq	%rax, -168(%rbp)
	leaq	-200(%rbp), %rax
	movq	%rax, -160(%rbp)
	leaq	-192(%rbp), %rax
	movq	%rax, -152(%rbp)
	leaq	-184(%rbp), %rax
	movq	%rax, -144(%rbp)
.LBB26:
.LBB27:
	.loc 1 287 9
	movq	-144(%rbp), %rax
	movq	$0, (%rax)
	.loc 1 288 9
	cmpq	$0, -176(%rbp)
	setne	%al
	movzbl	%al, %eax
	.loc 1 288 8 discriminator 1
	testq	%rax, %rax
	je	.L71
	movq	-176(%rbp), %rax
	movq	%rax, -136(%rbp)
	movq	-144(%rbp), %rax
	movq	%rax, -128(%rbp)
.LBB28:
.LBB29:
	.loc 1 80 7
	movq	-136(%rbp), %rbx
	.loc 1 80 16
	movq	-128(%rbp), %r14
	.loc 1 82 7
	subq	$1, %rbx
	.loc 1 82 9
	movzbl	(%rbx), %eax
	.loc 1 82 7
	andl	$15, %eax
	movl	%eax, %r13d
	.loc 1 83 7
	movzbl	%r13b, %eax
	negq	%rax
	addq	%rax, %rbx
	.loc 1 85 11
	jmp	.L72
.L73:
	.loc 1 86 11
	movq	%rbx, %rdx
	leaq	1(%rdx), %rbx
	.loc 1 86 4
	movq	%r14, %rax
	leaq	1(%rax), %r14
	.loc 1 86 9
	movzbl	(%rdx), %edx
	.loc 1 86 7
	movb	%dl, (%rax)
.L72:
	.loc 1 85 14
	movl	%r13d, %eax
	leal	-1(%rax), %r13d
	.loc 1 85 13
	testb	%al, %al
	jne	.L73
.LBE29:
.LBE28:
	.loc 1 288 9 discriminator 1
	cmpq	$0, -144(%rbp)
	setne	%al
	movzbl	%al, %eax
	cltq
	.loc 1 288 8 discriminator 2
	testq	%rax, %rax
	je	.L71
	.loc 1 289 19
	movq	-160(%rbp), %rax
	movq	-176(%rbp), %rdx
	movq	%rdx, (%rax)
	.loc 1 289 10
	movq	-160(%rbp), %rax
	movq	(%rax), %rdx
	.loc 1 289 8
	movq	-168(%rbp), %rax
	movq	%rdx, (%rax)
	.loc 1 290 14
	movq	-176(%rbp), %rax
	movq	(%rax), %rdx
	.loc 1 290 12
	movq	-152(%rbp), %rax
	movq	%rdx, (%rax)
	.loc 1 290 25
	movq	-176(%rbp), %rax
	movq	$255, (%rax)
	.loc 1 291 9
	movl	$1, %eax
	jmp	.L75
.L71:
	.loc 1 293 5
	movq	-176(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	.loc 1 294 12
	movl	$0, %eax
.L75:
.LBE27:
.LBE26:
	.loc 1 303 9 discriminator 1
	xorl	$1, %eax
	.loc 1 303 8 discriminator 1
	testb	%al, %al
	je	.L76
	.loc 1 304 9
	movl	$0, %eax
	jmp	.L70
.L76:
	.loc 1 305 32
	movq	-208(%rbp), %rax
	.loc 1 305 10
	leaq	-1(%rax), %rbx
	movq	%rbx, %rax
	.loc 1 305 5
	movzbl	(%rax), %edx
	.loc 1 305 38
	orl	$16, %edx
	movb	%dl, (%rax)
.L100:
	.loc 1 308 12
	movq	-184(%rbp), %rax
	leaq	-1(%rax), %rdx
	movq	%rdx, -184(%rbp)
	.loc 1 308 5
	testq	%rax, %rax
	jne	.L78
.L86:
	.loc 1 310 27
	movq	-200(%rbp), %rax
	movq	(%rax), %rax
	.loc 1 310 10
	movq	%rax, -208(%rbp)
	.loc 1 311 3
	movq	-200(%rbp), %rax
	.loc 1 311 8
	movzbl	(%rbx), %edx
	movzbl	%dl, %edx
	andl	$15, %edx
	notq	%rdx
	.loc 1 311 3
	addq	%rdx, %rax
	movq	%rax, %rdi
	call	free@PLT
	.loc 1 312 14
	movq	-208(%rbp), %rax
	.loc 1 312 6
	cmpq	$255, %rax
	je	.L103
	.loc 1 314 10
	movzbl	(%rbx), %eax
	.loc 1 314 14
	movzbl	%al, %eax
	andl	$32, %eax
	.loc 1 314 40
	testl	%eax, %eax
	jne	.L81
	.loc 1 314 59 discriminator 1
	movq	-208(%rbp), %rax
	.loc 1 314 40 discriminator 1
	movq	(%rax), %rax
	jmp	.L82
.L81:
	.loc 1 314 40 is_stmt 0 discriminator 2
	movq	-208(%rbp), %rax
.L82:
	.loc 1 314 8 is_stmt 1 discriminator 4
	movq	%rax, -200(%rbp)
	.loc 1 315 7
	movq	$0, -184(%rbp)
	.loc 1 315 14
	movq	-200(%rbp), %rax
	movq	%rax, -120(%rbp)
	leaq	-184(%rbp), %rax
	movq	%rax, -112(%rbp)
.LBB30:
.LBB31:
	.loc 1 80 7
	movq	-120(%rbp), %r13
	.loc 1 80 16
	movq	-112(%rbp), %r15
	.loc 1 82 7
	subq	$1, %r13
	.loc 1 82 9
	movzbl	0(%r13), %eax
	.loc 1 82 7
	andl	$15, %eax
	movl	%eax, %r14d
	.loc 1 83 7
	movzbl	%r14b, %eax
	negq	%rax
	addq	%rax, %r13
	.loc 1 85 11
	jmp	.L83
.L84:
	.loc 1 86 11
	movq	%r13, %rdx
	leaq	1(%rdx), %r13
	.loc 1 86 4
	movq	%r15, %rax
	leaq	1(%rax), %r15
	.loc 1 86 9
	movzbl	(%rdx), %edx
	.loc 1 86 7
	movb	%dl, (%rax)
.L83:
	.loc 1 85 14
	movl	%r14d, %eax
	leal	-1(%rax), %r14d
	.loc 1 85 13
	testb	%al, %al
	jne	.L84
	.loc 1 88 12
	nop
.LBE31:
.LBE30:
	.loc 1 316 19
	movq	-184(%rbp), %rax
	leaq	-1(%rax), %rdx
	movq	%rdx, -184(%rbp)
	.loc 1 316 16
	testq	%rax, %rax
	je	.L86
	.loc 1 317 29
	movq	-200(%rbp), %rax
	.loc 1 317 9
	leaq	-1(%rax), %rbx
	.loc 1 318 14
	leaq	-208(%rbp), %rax
	movq	(%rax), %rdx
	addq	$8, %rdx
	.loc 1 318 12
	movq	%rdx, (%rax)
	.loc 1 318 14
	movq	(%rax), %rax
	movq	(%rax), %rax
	.loc 1 318 12
	movq	%rax, -192(%rbp)
.L78:
	.loc 1 320 6
	movq	-192(%rbp), %rax
	.loc 1 320 5
	testq	%rax, %rax
	je	.L87
	.loc 1 320 41 discriminator 1
	movq	-192(%rbp), %rax
	.loc 1 320 20 discriminator 1
	leaq	-1(%rax), %r12
	.loc 1 320 46 discriminator 1
	movzbl	(%r12), %eax
	.loc 1 320 12 discriminator 1
	testb	%al, %al
	jns	.L87
	.loc 1 321 51
	movzbl	(%rbx), %eax
	.loc 1 321 6
	movzbl	%al, %eax
	movq	-200(%rbp), %rdx
	.loc 1 321 18
	movzbl	(%rbx), %ecx
	movzbl	%cl, %ecx
	andl	$15, %ecx
	notq	%rcx
	.loc 1 321 6
	addq	%rcx, %rdx
	movq	%rdx, -88(%rbp)
	leaq	-184(%rbp), %rdx
	movq	%rdx, -80(%rbp)
	movb	%al, -213(%rbp)
.LBB32:
.LBB33:
	.loc 1 93 8
	movq	-88(%rbp), %rax
	movq	%rax, -72(%rbp)
	.loc 1 93 16
	movq	-80(%rbp), %rax
	movq	%rax, -64(%rbp)
	.loc 1 94 16
	movzbl	-213(%rbp), %eax
	andl	$15, %eax
	.loc 1 94 5
	cmpl	$4, %eax
	je	.L88
	cmpl	$4, %eax
	jg	.L89
	cmpl	$3, %eax
	je	.L90
	cmpl	$3, %eax
	jg	.L89
	cmpl	$1, %eax
	je	.L91
	cmpl	$2, %eax
	je	.L92
	jmp	.L89
.L91:
	.loc 1 95 30
	movq	-64(%rbp), %rax
	movzbl	(%rax), %edx
	.loc 1 95 26
	movq	-72(%rbp), %rax
	movb	%dl, (%rax)
	.loc 1 95 39
	jmp	.L89
.L92:
	.loc 1 96 30
	movq	-64(%rbp), %rax
	movzwl	(%rax), %edx
	.loc 1 96 26
	movq	-72(%rbp), %rax
	movw	%dx, (%rax)
	.loc 1 96 39
	jmp	.L89
.L90:
	.loc 1 97 30
	movq	-64(%rbp), %rax
	movl	(%rax), %edx
	.loc 1 97 26
	movq	-72(%rbp), %rax
	movl	%edx, (%rax)
	.loc 1 97 39
	jmp	.L89
.L88:
	.loc 1 98 30
	movq	-64(%rbp), %rax
	movq	(%rax), %rdx
	.loc 1 98 26
	movq	-72(%rbp), %rax
	movq	%rdx, (%rax)
	.loc 1 98 39
	nop
.L89:
.LBE33:
.LBE32:
	.loc 1 322 10
	movq	$0, -184(%rbp)
	.loc 1 322 17
	movq	-192(%rbp), %rax
	movq	%rax, -104(%rbp)
	leaq	-184(%rbp), %rax
	movq	%rax, -96(%rbp)
.LBB34:
.LBB35:
	.loc 1 80 7
	movq	-104(%rbp), %r13
	.loc 1 80 16
	movq	-96(%rbp), %r15
	.loc 1 82 7
	subq	$1, %r13
	.loc 1 82 9
	movzbl	0(%r13), %eax
	.loc 1 82 7
	andl	$15, %eax
	movl	%eax, %r14d
	.loc 1 83 7
	movzbl	%r14b, %eax
	negq	%rax
	addq	%rax, %r13
	.loc 1 85 11
	jmp	.L94
.L95:
	.loc 1 86 11
	movq	%r13, %rdx
	leaq	1(%rdx), %r13
	.loc 1 86 4
	movq	%r15, %rax
	leaq	1(%rax), %r15
	.loc 1 86 9
	movzbl	(%rdx), %edx
	.loc 1 86 7
	movb	%dl, (%rax)
.L94:
	.loc 1 85 14
	movl	%r14d, %eax
	leal	-1(%rax), %r14d
	.loc 1 85 13
	testb	%al, %al
	jne	.L95
.LBE35:
.LBE34:
	.loc 1 324 12
	movzbl	(%rbx), %eax
	.loc 1 324 16
	movzbl	%al, %eax
	andl	$16, %eax
	.loc 1 324 9
	testl	%eax, %eax
	jne	.L97
	.loc 1 325 3
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	.loc 1 326 20
	movq	-208(%rbp), %rax
	.loc 1 326 24
	movq	-200(%rbp), %rdx
	movq	%rdx, (%rax)
.L97:
	.loc 1 328 6
	movq	-208(%rbp), %rdx
	movq	-200(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	.loc 1 329 6
	movq	-192(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -200(%rbp)
	movq	-192(%rbp), %rax
	movq	-208(%rbp), %rdx
	movq	%rdx, (%rax)
	.loc 1 330 6
	movq	-192(%rbp), %rax
	movq	%rax, -208(%rbp)
	movq	-200(%rbp), %rax
	movq	%rax, -192(%rbp)
	.loc 1 331 11
	movq	-208(%rbp), %rax
	movq	%rax, -200(%rbp)
	.loc 1 332 42
	movzbl	(%r12), %eax
	.loc 1 332 6
	movzbl	%al, %edx
	.loc 1 332 37
	movzbl	(%rbx), %eax
	.loc 1 332 6
	movzbl	%al, %eax
	movl	%eax, %esi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	.loc 1 333 6
	movzbl	(%r12), %edx
	movzbl	(%rbx), %eax
	addl	%eax, %eax
	andl	$32, %eax
	orl	%edx, %eax
	orl	$16, %eax
	movb	%al, (%r12)
	movzbl	(%r12), %eax
	movb	%al, (%rbx)
	.loc 1 334 42
	movzbl	(%r12), %eax
	.loc 1 334 6
	movzbl	%al, %edx
	.loc 1 334 37
	movzbl	(%rbx), %eax
	.loc 1 334 6
	movzbl	%al, %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	.loc 1 335 6
	jmp	.L98
.L87:
	.loc 1 337 8
	movq	-192(%rbp), %rax
	.loc 1 337 46
	testq	%rax, %rax
	je	.L99
	.loc 1 337 10 discriminator 1
	movq	-192(%rbp), %rax
	.loc 1 337 15 discriminator 1
	movzbl	(%r12), %edx
	movzbl	%dl, %edx
	andl	$15, %edx
	notq	%rdx
	.loc 1 337 10 discriminator 1
	addq	%rdx, %rax
	movq	%rax, %rdi
	call	free@PLT
.L99:
	.loc 1 338 10
	leaq	-208(%rbp), %rax
	movq	(%rax), %rdx
	addq	$8, %rdx
	.loc 1 338 8
	movq	%rdx, (%rax)
	.loc 1 338 10
	movq	(%rax), %rax
	movq	(%rax), %rax
	.loc 1 338 8
	movq	%rax, -192(%rbp)
	.loc 1 339 2
	movzbl	(%rbx), %eax
	andl	$-17, %eax
	movb	%al, (%rbx)
.L98:
	.loc 1 308 5
	jmp	.L100
.L103:
	.loc 1 313 7
	nop
.L80:
	.loc 1 342 9
	movq	$0, -232(%rbp)
.L70:
	.loc 1 343 1
	movq	-56(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L101
	call	__stack_chk_fail@PLT
.L101:
	addq	$200, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE18:
	.size	VEC_internalDelete, .-VEC_internalDelete
	.type	VEC_delete, @function
VEC_delete:
.LFB19:
	.loc 1 345 50
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	.loc 1 346 12
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	VEC_internalDelete
	.loc 1 347 1
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE19:
	.size	VEC_delete, .-VEC_delete
	.section	.rodata
.LC4:
	.string	"%lu\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB20:
	.loc 1 349 16
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$4096, %rsp
	orq	$0, (%rsp)
	subq	$336, %rsp
	.loc 1 349 16
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	.loc 1 350 9
	leaq	-4112(%rbp), %rdx
	movl	$0, %eax
	movl	$512, %ecx
	movq	%rdx, %rdi
	rep stosq
	.loc 1 351 9
	movl	$0, -4420(%rbp)
	.loc 1 353 18
	movl	$1, %edi
	call	VEC_create
	.loc 1 353 12 discriminator 1
	movq	%rax, -4408(%rbp)
	.loc 1 355 9
	leaq	-4400(%rbp), %rdx
	movl	$0, %eax
	movl	$36, %ecx
	movq	%rdx, %rdi
	rep stosq
	movl	$2, -4396(%rbp)
	movl	$4, -4392(%rbp)
	movl	$6, -4388(%rbp)
	movl	$8, -4384(%rbp)
	movl	$10, -4380(%rbp)
	movl	$1, -4376(%rbp)
	movl	$3, -4372(%rbp)
	movl	$5, -4368(%rbp)
	movl	$7, -4364(%rbp)
	movl	$9, -4360(%rbp)
	movl	$11, -4356(%rbp)
	movl	$2, -4352(%rbp)
	movl	$3, -4348(%rbp)
	movl	$5, -4344(%rbp)
	movl	$7, -4340(%rbp)
	movl	$11, -4336(%rbp)
	movl	$13, -4332(%rbp)
	movl	$4, -4328(%rbp)
	movl	$16, -4324(%rbp)
	movl	$32, -4320(%rbp)
	movl	$64, -4316(%rbp)
	movl	$128, -4312(%rbp)
	movl	$3, -4304(%rbp)
	movl	$6, -4300(%rbp)
	movl	$9, -4296(%rbp)
	movl	$12, -4292(%rbp)
	movl	$15, -4288(%rbp)
	movl	$18, -4284(%rbp)
	movl	$7, -4280(%rbp)
	movl	$14, -4276(%rbp)
	movl	$21, -4272(%rbp)
	movl	$28, -4268(%rbp)
	movl	$35, -4264(%rbp)
	movl	$42, -4260(%rbp)
	movl	$10, -4256(%rbp)
	movl	$20, -4252(%rbp)
	movl	$30, -4248(%rbp)
	movl	$40, -4244(%rbp)
	movl	$50, -4240(%rbp)
	movl	$60, -4236(%rbp)
	movl	$15, -4232(%rbp)
	movl	$45, -4228(%rbp)
	movl	$60, -4224(%rbp)
	movl	$75, -4220(%rbp)
	movl	$90, -4216(%rbp)
	movl	$105, -4212(%rbp)
	movl	$20, -4208(%rbp)
	movl	$40, -4204(%rbp)
	movl	$60, -4200(%rbp)
	movl	$80, -4196(%rbp)
	movl	$100, -4192(%rbp)
	movl	$120, -4188(%rbp)
	movl	$50, -4184(%rbp)
	movl	$100, -4180(%rbp)
	movl	$150, -4176(%rbp)
	movl	$200, -4172(%rbp)
	movl	$250, -4168(%rbp)
	movl	$300, -4164(%rbp)
	movl	$1, -4136(%rbp)
	movl	$1, -4132(%rbp)
	movl	$1, -4128(%rbp)
	movl	$1, -4124(%rbp)
	movl	$1, -4120(%rbp)
	movl	$1, -4116(%rbp)
	.loc 1 363 5
	leaq	-4400(%rbp), %rsi
	leaq	-4408(%rbp), %rax
	movl	$128, %r9d
	movl	$0, %r8d
	movl	$6, %ecx
	movl	$4, %edx
	movq	%rax, %rdi
	call	VEC_add
	.loc 1 364 5
	leaq	-4400(%rbp), %rax
	leaq	24(%rax), %rsi
	leaq	-4408(%rbp), %rax
	movl	$0, %r9d
	movl	$1, %r8d
	movl	$6, %ecx
	movl	$4, %edx
	movq	%rax, %rdi
	call	VEC_add
	.loc 1 371 11
	movq	-4408(%rbp), %rax
	.loc 1 371 31
	subq	$1, %rax
	movzbl	(%rax), %eax
	.loc 1 371 35
	movzbl	%al, %eax
	andl	$15, %eax
	.loc 1 371 7
	movl	%eax, -4420(%rbp)
	.loc 1 372 7
	movq	$0, -4416(%rbp)
	.loc 1 374 5
	movl	-4420(%rbp), %eax
	movslq	%eax, %rdx
	.loc 1 374 16
	movq	-4408(%rbp), %rax
	movl	-4420(%rbp), %ecx
	movslq	%ecx, %rcx
	andl	$15, %ecx
	notq	%rcx
	.loc 1 374 5
	addq	%rax, %rcx
	leaq	-4416(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	memcpy@PLT
	.loc 1 375 5
	movq	-4416(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	.loc 1 377 7
	movl	$3, -4420(%rbp)
	.loc 1 378 5 discriminator 4
	leaq	-4420(%rbp), %rax
	movq	(%rax), %rdx
	leaq	-4420(%rbp), %rax
	addq	$1, %rdx
	movq	%rdx, (%rax)
	.loc 1 379 5
	leaq	-4408(%rbp), %rax
	movq	%rax, %rdi
	call	VEC_delete
	movl	$0, %eax
	.loc 1 381 1
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L108
	call	__stack_chk_fail@PLT
.L108:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE20:
	.size	main, .-main
.Letext0:
	.file 2 "/usr/lib/gcc/x86_64-linux-gnu/13/include/stddef.h"
	.file 3 "/usr/include/x86_64-linux-gnu/bits/types.h"
	.file 4 "/usr/include/stdio.h"
	.file 5 "/usr/include/x86_64-linux-gnu/bits/stdint-uintn.h"
	.file 6 "/usr/include/stdint.h"
	.file 7 "/usr/include/stdlib.h"
	.file 8 "/usr/include/string.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x97b
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF70
	.byte	0xc
	.long	.LASF71
	.long	.LASF72
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF5
	.byte	0x2
	.byte	0xd6
	.byte	0x17
	.long	0x39
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF0
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF1
	.uleb128 0x4
	.byte	0x8
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.long	.LASF2
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.long	.LASF3
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF4
	.uleb128 0x2
	.long	.LASF6
	.byte	0x3
	.byte	0x26
	.byte	0x17
	.long	0x49
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.long	.LASF7
	.uleb128 0x2
	.long	.LASF8
	.byte	0x3
	.byte	0x28
	.byte	0x1c
	.long	0x50
	.uleb128 0x5
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.long	.LASF9
	.byte	0x3
	.byte	0x2a
	.byte	0x16
	.long	0x40
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF10
	.uleb128 0x2
	.long	.LASF11
	.byte	0x3
	.byte	0x2d
	.byte	0x1b
	.long	0x39
	.uleb128 0x2
	.long	.LASF12
	.byte	0x3
	.byte	0x49
	.byte	0x1b
	.long	0x39
	.uleb128 0x2
	.long	.LASF13
	.byte	0x3
	.byte	0xc1
	.byte	0x1b
	.long	0x90
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF14
	.uleb128 0x6
	.long	0xbb
	.uleb128 0x2
	.long	.LASF15
	.byte	0x4
	.byte	0x4d
	.byte	0x13
	.long	0xaf
	.uleb128 0x7
	.byte	0x8
	.long	0xc2
	.uleb128 0x2
	.long	.LASF16
	.byte	0x5
	.byte	0x18
	.byte	0x13
	.long	0x5e
	.uleb128 0x2
	.long	.LASF17
	.byte	0x5
	.byte	0x19
	.byte	0x14
	.long	0x71
	.uleb128 0x2
	.long	.LASF18
	.byte	0x5
	.byte	0x1a
	.byte	0x14
	.long	0x84
	.uleb128 0x2
	.long	.LASF19
	.byte	0x5
	.byte	0x1b
	.byte	0x14
	.long	0x97
	.uleb128 0x2
	.long	.LASF20
	.byte	0x6
	.byte	0x66
	.byte	0x16
	.long	0xa3
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF21
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF22
	.uleb128 0x7
	.byte	0x8
	.long	0x129
	.uleb128 0x8
	.uleb128 0x3
	.byte	0x10
	.byte	0x4
	.long	.LASF23
	.uleb128 0x2
	.long	.LASF24
	.byte	0x1
	.byte	0x22
	.byte	0x12
	.long	0xfd
	.uleb128 0x2
	.long	.LASF25
	.byte	0x1
	.byte	0x23
	.byte	0x11
	.long	0x149
	.uleb128 0x7
	.byte	0x8
	.long	0x47
	.uleb128 0x2
	.long	.LASF26
	.byte	0x1
	.byte	0x24
	.byte	0x11
	.long	0xd9
	.uleb128 0x9
	.byte	0x8
	.byte	0x1
	.byte	0x25
	.byte	0x9
	.long	0x1ad
	.uleb128 0xa
	.long	.LASF27
	.byte	0x1
	.byte	0x26
	.byte	0xb
	.long	0x47
	.uleb128 0xa
	.long	.LASF28
	.byte	0x1
	.byte	0x27
	.byte	0xf
	.long	0x109
	.uleb128 0xa
	.long	.LASF29
	.byte	0x1
	.byte	0x28
	.byte	0xe
	.long	0xfd
	.uleb128 0xa
	.long	.LASF30
	.byte	0x1
	.byte	0x29
	.byte	0xe
	.long	0xf1
	.uleb128 0xa
	.long	.LASF31
	.byte	0x1
	.byte	0x2a
	.byte	0xe
	.long	0xe5
	.uleb128 0xa
	.long	.LASF26
	.byte	0x1
	.byte	0x2b
	.byte	0xd
	.long	0xd9
	.byte	0
	.uleb128 0x2
	.long	.LASF32
	.byte	0x1
	.byte	0x2c
	.byte	0x3
	.long	0x15b
	.uleb128 0xb
	.long	.LASF33
	.byte	0x4
	.value	0x14c
	.byte	0xc
	.long	0x7d
	.long	0x1d1
	.uleb128 0xc
	.long	0xd3
	.uleb128 0xd
	.byte	0
	.uleb128 0xb
	.long	.LASF34
	.byte	0x4
	.value	0x278
	.byte	0xc
	.long	0x7d
	.long	0x1e8
	.uleb128 0xc
	.long	0xd3
	.byte	0
	.uleb128 0xe
	.long	.LASF73
	.byte	0x7
	.value	0x235
	.byte	0xd
	.long	0x1fb
	.uleb128 0xc
	.long	0x47
	.byte	0
	.uleb128 0xb
	.long	.LASF35
	.byte	0x7
	.value	0x226
	.byte	0xe
	.long	0x47
	.long	0x217
	.uleb128 0xc
	.long	0x47
	.uleb128 0xc
	.long	0x2d
	.byte	0
	.uleb128 0xf
	.long	.LASF36
	.byte	0x8
	.byte	0x2b
	.byte	0xe
	.long	0x47
	.long	0x237
	.uleb128 0xc
	.long	0x47
	.uleb128 0xc
	.long	0x123
	.uleb128 0xc
	.long	0x2d
	.byte	0
	.uleb128 0xf
	.long	.LASF37
	.byte	0x8
	.byte	0x3d
	.byte	0xe
	.long	0x47
	.long	0x257
	.uleb128 0xc
	.long	0x47
	.uleb128 0xc
	.long	0x7d
	.uleb128 0xc
	.long	0x2d
	.byte	0
	.uleb128 0xb
	.long	.LASF38
	.byte	0x7
	.value	0x21b
	.byte	0xe
	.long	0x47
	.long	0x26e
	.uleb128 0xc
	.long	0x2d
	.byte	0
	.uleb128 0x10
	.long	.LASF74
	.byte	0x1
	.value	0x15d
	.byte	0x5
	.long	0x7d
	.quad	.LFB20
	.quad	.LFE20-.LFB20
	.uleb128 0x1
	.byte	0x9c
	.long	0x2f0
	.uleb128 0x11
	.string	"num"
	.byte	0x1
	.value	0x15e
	.byte	0x9
	.long	0x2f0
	.uleb128 0x3
	.byte	0x91
	.sleb128 -4128
	.uleb128 0x11
	.string	"p"
	.byte	0x1
	.value	0x15f
	.byte	0x9
	.long	0x7d
	.uleb128 0x3
	.byte	0x91
	.sleb128 -4436
	.uleb128 0x11
	.string	"x"
	.byte	0x1
	.value	0x160
	.byte	0x10
	.long	0x131
	.uleb128 0x3
	.byte	0x91
	.sleb128 -4432
	.uleb128 0x11
	.string	"vec"
	.byte	0x1
	.value	0x161
	.byte	0xc
	.long	0x149
	.uleb128 0x3
	.byte	0x91
	.sleb128 -4424
	.uleb128 0x12
	.string	"new"
	.byte	0x1
	.value	0x162
	.byte	0xc
	.long	0x149
	.uleb128 0x13
	.long	.LASF39
	.byte	0x1
	.value	0x163
	.byte	0x9
	.long	0x301
	.uleb128 0x3
	.byte	0x91
	.sleb128 -4416
	.byte	0
	.uleb128 0x14
	.long	0x7d
	.long	0x301
	.uleb128 0x15
	.long	0x39
	.value	0x3ff
	.byte	0
	.uleb128 0x14
	.long	0x7d
	.long	0x317
	.uleb128 0x16
	.long	0x39
	.byte	0xb
	.uleb128 0x16
	.long	0x39
	.byte	0x5
	.byte	0
	.uleb128 0x17
	.long	.LASF40
	.byte	0x1
	.value	0x159
	.byte	0x1a
	.long	0x47
	.quad	.LFB19
	.quad	.LFE19-.LFB19
	.uleb128 0x1
	.byte	0x9c
	.long	0x34b
	.uleb128 0x18
	.string	"vec"
	.byte	0x1
	.value	0x159
	.byte	0x2d
	.long	0x34b
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x7
	.byte	0x8
	.long	0x149
	.uleb128 0x17
	.long	.LASF41
	.byte	0x1
	.value	0x129
	.byte	0x1a
	.long	0x47
	.quad	.LFB18
	.quad	.LFE18-.LFB18
	.uleb128 0x1
	.byte	0x9c
	.long	0x562
	.uleb128 0x18
	.string	"vec"
	.byte	0x1
	.value	0x129
	.byte	0x34
	.long	0x562
	.uleb128 0x3
	.byte	0x91
	.sleb128 -248
	.uleb128 0x13
	.long	.LASF42
	.byte	0x1
	.value	0x12a
	.byte	0xb
	.long	0x47
	.uleb128 0x3
	.byte	0x91
	.sleb128 -224
	.uleb128 0x13
	.long	.LASF43
	.byte	0x1
	.value	0x12a
	.byte	0x14
	.long	0x47
	.uleb128 0x3
	.byte	0x91
	.sleb128 -216
	.uleb128 0x13
	.long	.LASF44
	.byte	0x1
	.value	0x12b
	.byte	0xb
	.long	0x47
	.uleb128 0x3
	.byte	0x91
	.sleb128 -208
	.uleb128 0x11
	.string	"sz"
	.byte	0x1
	.value	0x12c
	.byte	0xc
	.long	0x2d
	.uleb128 0x3
	.byte	0x91
	.sleb128 -200
	.uleb128 0x11
	.string	"fl"
	.byte	0x1
	.value	0x12d
	.byte	0x17
	.long	0x568
	.uleb128 0x1
	.byte	0x53
	.uleb128 0x11
	.string	"ul"
	.byte	0x1
	.value	0x12d
	.byte	0x1c
	.long	0x568
	.uleb128 0x1
	.byte	0x5c
	.uleb128 0x11
	.string	"k"
	.byte	0x1
	.value	0x12e
	.byte	0x9
	.long	0x7d
	.uleb128 0x3
	.byte	0x91
	.sleb128 -228
	.uleb128 0x19
	.long	.LASF59
	.byte	0x1
	.value	0x155
	.byte	0x2
	.quad	.L80
	.uleb128 0x1a
	.long	0x56e
	.quad	.LBB26
	.quad	.LBE26-.LBB26
	.byte	0x1
	.value	0x12f
	.byte	0xb
	.long	0x490
	.uleb128 0x1b
	.long	0x5b4
	.uleb128 0x3
	.byte	0x91
	.sleb128 -160
	.uleb128 0x1b
	.long	0x5a7
	.uleb128 0x3
	.byte	0x91
	.sleb128 -168
	.uleb128 0x1b
	.long	0x59a
	.uleb128 0x3
	.byte	0x91
	.sleb128 -176
	.uleb128 0x1b
	.long	0x58d
	.uleb128 0x3
	.byte	0x91
	.sleb128 -184
	.uleb128 0x1b
	.long	0x580
	.uleb128 0x3
	.byte	0x91
	.sleb128 -192
	.uleb128 0x1c
	.long	0x931
	.quad	.LBB28
	.quad	.LBE28-.LBB28
	.byte	0x1
	.value	0x120
	.byte	0x9
	.uleb128 0x1b
	.long	0x94e
	.uleb128 0x3
	.byte	0x91
	.sleb128 -144
	.uleb128 0x1b
	.long	0x942
	.uleb128 0x3
	.byte	0x91
	.sleb128 -152
	.uleb128 0x1d
	.long	0x959
	.uleb128 0x1
	.byte	0x5e
	.uleb128 0x1d
	.long	0x963
	.uleb128 0x1
	.byte	0x53
	.uleb128 0x1d
	.long	0x96d
	.uleb128 0x1
	.byte	0x5d
	.byte	0
	.byte	0
	.uleb128 0x1a
	.long	0x931
	.quad	.LBB30
	.quad	.LBE30-.LBB30
	.byte	0x1
	.value	0x13b
	.byte	0xe
	.long	0x4d5
	.uleb128 0x1b
	.long	0x94e
	.uleb128 0x3
	.byte	0x91
	.sleb128 -128
	.uleb128 0x1b
	.long	0x942
	.uleb128 0x3
	.byte	0x91
	.sleb128 -136
	.uleb128 0x1d
	.long	0x959
	.uleb128 0x1
	.byte	0x5f
	.uleb128 0x1d
	.long	0x963
	.uleb128 0x1
	.byte	0x5d
	.uleb128 0x1d
	.long	0x96d
	.uleb128 0x1
	.byte	0x5e
	.byte	0
	.uleb128 0x1a
	.long	0x8e4
	.quad	.LBB32
	.quad	.LBE32-.LBB32
	.byte	0x1
	.value	0x141
	.byte	0x6
	.long	0x520
	.uleb128 0x1b
	.long	0x909
	.uleb128 0x3
	.byte	0x91
	.sleb128 -229
	.uleb128 0x1b
	.long	0x8ff
	.uleb128 0x3
	.byte	0x91
	.sleb128 -96
	.uleb128 0x1b
	.long	0x8f5
	.uleb128 0x3
	.byte	0x91
	.sleb128 -104
	.uleb128 0x1d
	.long	0x914
	.uleb128 0x3
	.byte	0x91
	.sleb128 -88
	.uleb128 0x1d
	.long	0x91f
	.uleb128 0x3
	.byte	0x91
	.sleb128 -80
	.byte	0
	.uleb128 0x1c
	.long	0x931
	.quad	.LBB34
	.quad	.LBE34-.LBB34
	.byte	0x1
	.value	0x142
	.byte	0x11
	.uleb128 0x1b
	.long	0x94e
	.uleb128 0x3
	.byte	0x91
	.sleb128 -112
	.uleb128 0x1b
	.long	0x942
	.uleb128 0x3
	.byte	0x91
	.sleb128 -120
	.uleb128 0x1d
	.long	0x959
	.uleb128 0x1
	.byte	0x5f
	.uleb128 0x1d
	.long	0x963
	.uleb128 0x1
	.byte	0x5d
	.uleb128 0x1d
	.long	0x96d
	.uleb128 0x1
	.byte	0x5e
	.byte	0
	.byte	0
	.uleb128 0x7
	.byte	0x8
	.long	0x13d
	.uleb128 0x7
	.byte	0x8
	.long	0xd9
	.uleb128 0x1e
	.long	.LASF66
	.byte	0x1
	.value	0x11e
	.byte	0x2a
	.long	0x5c1
	.byte	0x3
	.long	0x5c1
	.uleb128 0x1f
	.string	"vec"
	.byte	0x1
	.value	0x11e
	.byte	0x3f
	.long	0x13d
	.uleb128 0x20
	.long	.LASF45
	.byte	0x1
	.value	0x11e
	.byte	0x4a
	.long	0x13d
	.uleb128 0x20
	.long	.LASF46
	.byte	0x1
	.value	0x11e
	.byte	0x56
	.long	0x13d
	.uleb128 0x20
	.long	.LASF47
	.byte	0x1
	.value	0x11e
	.byte	0x65
	.long	0x13d
	.uleb128 0x1f
	.string	"sz"
	.byte	0x1
	.value	0x11e
	.byte	0x77
	.long	0x5c8
	.byte	0
	.uleb128 0x3
	.byte	0x1
	.byte	0x2
	.long	.LASF48
	.uleb128 0x7
	.byte	0x8
	.long	0x2d
	.uleb128 0x21
	.long	.LASF49
	.byte	0x1
	.value	0x10d
	.byte	0x1a
	.long	0x47
	.quad	.LFB16
	.quad	.LFE16-.LFB16
	.uleb128 0x1
	.byte	0x9c
	.long	0x612
	.uleb128 0x18
	.string	"vec"
	.byte	0x1
	.value	0x10d
	.byte	0x2d
	.long	0x34b
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x22
	.long	.LASF50
	.byte	0x1
	.value	0x10d
	.byte	0x3a
	.long	0xc7
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.byte	0
	.uleb128 0x23
	.long	.LASF51
	.byte	0x1
	.byte	0xd6
	.byte	0x1b
	.long	0x47
	.quad	.LFB12
	.quad	.LFE12-.LFB12
	.uleb128 0x1
	.byte	0x9c
	.long	0x6ef
	.uleb128 0x24
	.string	"vec"
	.byte	0x1
	.byte	0xd6
	.byte	0x2b
	.long	0x34b
	.uleb128 0x3
	.byte	0x91
	.sleb128 -72
	.uleb128 0x24
	.string	"vd"
	.byte	0x1
	.byte	0xd6
	.byte	0x36
	.long	0x47
	.uleb128 0x3
	.byte	0x91
	.sleb128 -80
	.uleb128 0x25
	.long	.LASF52
	.byte	0x1
	.byte	0xd6
	.byte	0x41
	.long	0x2d
	.uleb128 0x3
	.byte	0x91
	.sleb128 -88
	.uleb128 0x24
	.string	"sz"
	.byte	0x1
	.byte	0xd6
	.byte	0x50
	.long	0x2d
	.uleb128 0x3
	.byte	0x91
	.sleb128 -96
	.uleb128 0x25
	.long	.LASF50
	.byte	0x1
	.byte	0xd6
	.byte	0x5b
	.long	0x2d
	.uleb128 0x3
	.byte	0x91
	.sleb128 -104
	.uleb128 0x25
	.long	.LASF53
	.byte	0x1
	.byte	0xd6
	.byte	0x68
	.long	0x14f
	.uleb128 0x3
	.byte	0x91
	.sleb128 -108
	.uleb128 0x26
	.long	.LASF54
	.byte	0x1
	.byte	0xd7
	.byte	0xb
	.long	0x47
	.uleb128 0x2
	.byte	0x91
	.sleb128 -48
	.uleb128 0x26
	.long	.LASF55
	.byte	0x1
	.byte	0xd7
	.byte	0x2e
	.long	0x149
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x27
	.string	"fl"
	.byte	0x1
	.byte	0xd8
	.byte	0xb
	.long	0x14f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -49
	.uleb128 0x26
	.long	.LASF56
	.byte	0x1
	.byte	0xd9
	.byte	0xc
	.long	0x2d
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x28
	.long	0x8c7
	.quad	.LBB24
	.quad	.LBE24-.LBB24
	.byte	0x1
	.byte	0xdd
	.byte	0x16
	.uleb128 0x1b
	.long	0x8d8
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.uleb128 0x23
	.long	.LASF57
	.byte	0x1
	.byte	0xb5
	.byte	0x27
	.long	0x149
	.quad	.LFB11
	.quad	.LFE11-.LFB11
	.uleb128 0x1
	.byte	0x9c
	.long	0x7be
	.uleb128 0x24
	.string	"vec"
	.byte	0x1
	.byte	0xb5
	.byte	0x3a
	.long	0x34b
	.uleb128 0x3
	.byte	0x91
	.sleb128 -104
	.uleb128 0x24
	.string	"vd"
	.byte	0x1
	.byte	0xb5
	.byte	0x45
	.long	0x47
	.uleb128 0x3
	.byte	0x91
	.sleb128 -112
	.uleb128 0x25
	.long	.LASF50
	.byte	0x1
	.byte	0xb5
	.byte	0x50
	.long	0x2d
	.uleb128 0x3
	.byte	0x91
	.sleb128 -120
	.uleb128 0x25
	.long	.LASF58
	.byte	0x1
	.byte	0xb5
	.byte	0x5e
	.long	0x2d
	.uleb128 0x3
	.byte	0x91
	.sleb128 -128
	.uleb128 0x27
	.string	"sz"
	.byte	0x1
	.byte	0xb6
	.byte	0x10
	.long	0x131
	.uleb128 0x3
	.byte	0x91
	.sleb128 -88
	.uleb128 0x27
	.string	"fl"
	.byte	0x1
	.byte	0xb6
	.byte	0x14
	.long	0x131
	.uleb128 0x3
	.byte	0x91
	.sleb128 -80
	.uleb128 0x29
	.long	.LASF60
	.byte	0x1
	.byte	0xcd
	.byte	0x5
	.quad	.L36
	.uleb128 0x28
	.long	0x931
	.quad	.LBB22
	.quad	.LBE22-.LBB22
	.byte	0x1
	.byte	0xb8
	.byte	0x10
	.uleb128 0x1b
	.long	0x94e
	.uleb128 0x2
	.byte	0x91
	.sleb128 -64
	.uleb128 0x1b
	.long	0x942
	.uleb128 0x3
	.byte	0x91
	.sleb128 -72
	.uleb128 0x1d
	.long	0x959
	.uleb128 0x1
	.byte	0x5d
	.uleb128 0x1d
	.long	0x963
	.uleb128 0x1
	.byte	0x53
	.uleb128 0x1d
	.long	0x96d
	.uleb128 0x1
	.byte	0x5c
	.byte	0
	.byte	0
	.uleb128 0x23
	.long	.LASF61
	.byte	0x1
	.byte	0x89
	.byte	0x27
	.long	0x149
	.quad	.LFB10
	.quad	.LFE10-.LFB10
	.uleb128 0x1
	.byte	0x9c
	.long	0x856
	.uleb128 0x24
	.string	"vec"
	.byte	0x1
	.byte	0x89
	.byte	0x3a
	.long	0x34b
	.uleb128 0x3
	.byte	0x91
	.sleb128 -72
	.uleb128 0x25
	.long	.LASF54
	.byte	0x1
	.byte	0x89
	.byte	0x45
	.long	0x47
	.uleb128 0x3
	.byte	0x91
	.sleb128 -80
	.uleb128 0x24
	.string	"sz"
	.byte	0x1
	.byte	0x89
	.byte	0x57
	.long	0x131
	.uleb128 0x3
	.byte	0x91
	.sleb128 -88
	.uleb128 0x25
	.long	.LASF62
	.byte	0x1
	.byte	0x89
	.byte	0x61
	.long	0x14f
	.uleb128 0x3
	.byte	0x91
	.sleb128 -92
	.uleb128 0x27
	.string	"v0"
	.byte	0x1
	.byte	0x8a
	.byte	0xb
	.long	0x47
	.uleb128 0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0x27
	.string	"sb"
	.byte	0x1
	.byte	0x8b
	.byte	0x14
	.long	0x14f
	.uleb128 0x1
	.byte	0x5c
	.uleb128 0x27
	.string	"pb"
	.byte	0x1
	.byte	0x8b
	.byte	0x18
	.long	0x14f
	.uleb128 0x1
	.byte	0x53
	.uleb128 0x26
	.long	.LASF63
	.byte	0x1
	.byte	0x8c
	.byte	0x18
	.long	0x109
	.uleb128 0x1
	.byte	0x5d
	.byte	0
	.uleb128 0x23
	.long	.LASF64
	.byte	0x1
	.byte	0x6e
	.byte	0xf
	.long	0x149
	.quad	.LFB9
	.quad	.LFE9-.LFB9
	.uleb128 0x1
	.byte	0x9c
	.long	0x8c7
	.uleb128 0x25
	.long	.LASF65
	.byte	0x1
	.byte	0x6e
	.byte	0x21
	.long	0x2d
	.uleb128 0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0x27
	.string	"vec"
	.byte	0x1
	.byte	0x6f
	.byte	0xb
	.long	0x47
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x27
	.string	"mSz"
	.byte	0x1
	.byte	0x70
	.byte	0xb
	.long	0x14f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -33
	.uleb128 0x28
	.long	0x8c7
	.quad	.LBB20
	.quad	.LBE20-.LBB20
	.byte	0x1
	.byte	0x75
	.byte	0xb
	.uleb128 0x1b
	.long	0x8d8
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.uleb128 0x2a
	.long	.LASF67
	.byte	0x1
	.byte	0x66
	.byte	0x2c
	.long	0xd9
	.byte	0x3
	.long	0x8e4
	.uleb128 0x2b
	.string	"sz"
	.byte	0x1
	.byte	0x66
	.byte	0x41
	.long	0xfd
	.byte	0
	.uleb128 0x2a
	.long	.LASF68
	.byte	0x1
	.byte	0x5a
	.byte	0x2c
	.long	0xd9
	.byte	0x3
	.long	0x92b
	.uleb128 0x2b
	.string	"d"
	.byte	0x1
	.byte	0x5a
	.byte	0x3e
	.long	0x47
	.uleb128 0x2b
	.string	"s"
	.byte	0x1
	.byte	0x5a
	.byte	0x47
	.long	0x47
	.uleb128 0x2b
	.string	"fl"
	.byte	0x1
	.byte	0x5a
	.byte	0x52
	.long	0xd9
	.uleb128 0x2c
	.string	"_d"
	.byte	0x1
	.byte	0x5b
	.byte	0x11
	.long	0x92b
	.uleb128 0x2c
	.string	"_s"
	.byte	0x1
	.byte	0x5b
	.byte	0x24
	.long	0x92b
	.byte	0
	.uleb128 0x7
	.byte	0x8
	.long	0x1ad
	.uleb128 0x2a
	.long	.LASF69
	.byte	0x1
	.byte	0x4d
	.byte	0x2c
	.long	0xd9
	.byte	0x3
	.long	0x978
	.uleb128 0x2b
	.string	"vec"
	.byte	0x1
	.byte	0x4d
	.byte	0x3e
	.long	0x47
	.uleb128 0x2b
	.string	"to"
	.byte	0x1
	.byte	0x4d
	.byte	0x49
	.long	0x47
	.uleb128 0x2c
	.string	"s"
	.byte	0x1
	.byte	0x4e
	.byte	0x15
	.long	0x978
	.uleb128 0x2c
	.string	"v"
	.byte	0x1
	.byte	0x4e
	.byte	0x19
	.long	0x978
	.uleb128 0x2c
	.string	"i"
	.byte	0x1
	.byte	0x4e
	.byte	0x1c
	.long	0x14f
	.byte	0
	.uleb128 0x7
	.byte	0x8
	.long	0x14f
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x26
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x17
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0xa
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0x5
	.uleb128 0x57
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0x5
	.uleb128 0x57
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x26
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x27
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x57
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0xa
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x2a
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2b
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2c
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.long	0x2c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0
	.value	0
	.value	0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF33:
	.string	"printf"
.LASF65:
	.string	"vecSize"
.LASF38:
	.string	"malloc"
.LASF59:
	.string	"_del_end"
.LASF5:
	.string	"size_t"
.LASF60:
	.string	"err_outOfBound"
.LASF19:
	.string	"uint64_t"
.LASF6:
	.string	"__uint8_t"
.LASF15:
	.string	"ssize_t"
.LASF53:
	.string	"type"
.LASF57:
	.string	"VEC_expand"
.LASF22:
	.string	"long long unsigned int"
.LASF32:
	.string	"_inttype_t"
.LASF28:
	.string	"wordx"
.LASF51:
	.string	"VEC_add"
.LASF73:
	.string	"free"
.LASF58:
	.string	"vflag"
.LASF56:
	.string	"nalloc"
.LASF21:
	.string	"long long int"
.LASF4:
	.string	"signed char"
.LASF20:
	.string	"uintmax_t"
.LASF71:
	.string	"vector.c"
.LASF10:
	.string	"long int"
.LASF13:
	.string	"__ssize_t"
.LASF61:
	.string	"VEC_append"
.LASF40:
	.string	"VEC_delete"
.LASF36:
	.string	"memcpy"
.LASF17:
	.string	"uint16_t"
.LASF41:
	.string	"VEC_internalDelete"
.LASF24:
	.string	"VEC_szType"
.LASF68:
	.string	"VEC_putSize"
.LASF9:
	.string	"__uint32_t"
.LASF1:
	.string	"unsigned int"
.LASF67:
	.string	"VEC_ssizeof"
.LASF44:
	.string	"lNext"
.LASF54:
	.string	"newdt"
.LASF0:
	.string	"long unsigned int"
.LASF39:
	.string	"data"
.LASF3:
	.string	"short unsigned int"
.LASF34:
	.string	"puts"
.LASF70:
	.string	"GNU C17 13.1.0 -mtune=generic -march=x86-64 -g -fasynchronous-unwind-tables -fstack-protector-strong -fstack-clash-protection -fcf-protection"
.LASF72:
	.string	"/home/michael/Mvpg/Jparser"
.LASF23:
	.string	"long double"
.LASF47:
	.string	"descdant"
.LASF52:
	.string	"bytesz"
.LASF11:
	.string	"__uint64_t"
.LASF63:
	.string	"memtb"
.LASF55:
	.string	"newvec"
.LASF27:
	.string	"nword"
.LASF25:
	.string	"vec_t"
.LASF48:
	.string	"_Bool"
.LASF2:
	.string	"unsigned char"
.LASF45:
	.string	"curr"
.LASF7:
	.string	"short int"
.LASF62:
	.string	"meta"
.LASF64:
	.string	"VEC_create"
.LASF18:
	.string	"uint32_t"
.LASF14:
	.string	"char"
.LASF66:
	.string	"VEC_deleteInit"
.LASF8:
	.string	"__uint16_t"
.LASF49:
	.string	"VEC_remove"
.LASF50:
	.string	"index"
.LASF12:
	.string	"__uintmax_t"
.LASF69:
	.string	"VEC_getSize"
.LASF37:
	.string	"memset"
.LASF16:
	.string	"uint8_t"
.LASF42:
	.string	"lCurrt"
.LASF35:
	.string	"realloc"
.LASF43:
	.string	"lTmp"
.LASF74:
	.string	"main"
.LASF26:
	.string	"word0"
.LASF31:
	.string	"word2"
.LASF30:
	.string	"word4"
.LASF29:
	.string	"word8"
.LASF46:
	.string	"currTmp"
	.ident	"GCC: (Ubuntu 13.1.0-8ubuntu1~20.04.2) 13.1.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
