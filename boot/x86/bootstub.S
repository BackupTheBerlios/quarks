/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
	.text
	.globl _start

/* Entry point is at linear adress 0x10000 */
_start:
	lgdt	temp_gdt
	ljmp	$0x8, $new_cs
new_cs:
	mov		$0x10, %ax
	mov		%ax, %ds
	mov		%ax, %es
	mov		%ax, %fs
	mov		%ax, %gs
	mov		%ax, %ss

	pushl	%ebp
	movl	%esp, %ebp
	subl	$0x18, %esp
	movl	%ebp, -4(%ebp)
	movl	0xc(%ebp), %eax
	pushl	%eax
	movl	-4(%ebp), %eax
	pushl	%eax
	movl	0x8(%ebp), %eax
	pushl	%eax
	movl	0x10(%ebp), %eax
	pushl	%eax
	call	startup
	hlt

	ret

temp_gdt:
	.word	24
	.long	temp_gdt
	.word	0

	.long	0x0000ffff
	.long	0x00cf9a00
	.long	0x0000ffff
	.long	0x00cf9200
