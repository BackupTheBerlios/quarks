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
	cli
	.byte 0x2e, 0x0f, 0x01, 0x16	/* lgdt %cs:temp_gdt - _start */
	.word temp_gdt - _start
	movl	%cr0, %eax
	orb	$1, %al
	movl	%eax, %cr0
	.byte 0x66				/* broken binutils on QNX */
	ljmp $0x8, $new_cs
new_cs:
	movl $0x10, %eax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	movl	$_start, %esp
	cld

	call main
	hlt

/* Temporary gdt. */
temp_gdt:
	.word 0x18
	.long temp_gdt
	.word 0x0000
	.long 0x0000ffff
	.long 0x00cf9a00
	.long 0x0000ffff
	.long 0x00cf9200
