/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
	.text
	.globl _start

/* Entry point is at linear adress 0x100000 */
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

	movl	$0xa0000, %eax
	movl	%eax, %ebp
	movl	%eax, %esp

	pushl	%ebx
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
