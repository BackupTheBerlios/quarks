/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
	.text
	.globl	_start
_start:
	/* This is our entry point.  We should already be in
	   protected mode.  */
	lea		stack_top, %esp

	pushl	$0
	popf

	lea		__CTOR_LIST__,%ebx
	jmp		2f
1:	addl	$4,%ebx
	call	*%eax
2:	movl	(%ebx),%eax
	cmpl	$0,%eax
	jnz		1b

	/* Jump to the kernel initialization code.  */
	call	main

	/* This should never be reached.  */
1:	int		$3
	jmp		1b

	.section .ctors, "a", @progbits
	.p2align 2
	/* .long	-1 */
__CTOR_LIST__:

	/* The destructor section isn't actually used.  */
	.section .dtors, "a", @progbits
	.p2align 2
	/* .long	-1 */
__DTOR_LIST__:

	/* A small, temporary stack used before we have page allocations.  */

	.bss
	.space	256
stack_top:
