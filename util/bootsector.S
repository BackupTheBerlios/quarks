/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
	.text

	.globl	_start
	.code16
_start:
	/* code located at 0000:7C00, adjust segment registers and create stack */
	cli
	xorl	%eax, %eax
	movw	%ax, %ss
	movl	$_start, %esp
	call	ena20
	lgdt	%cs:gdt
	movl	%cr0, %eax
	incw	%ax
	movl	%eax, %cr0
	ljmp	$8, $protm

	.code32
protm:
	movl	$0x10, %ebx
	movw	%bx, %ds
	movw	%bx, %es
	movw	%bx, %ss
	decw	%ax
	movl	%eax, %cr0
	jmp		unreal

	.code16
unreal:
	xorw	%ax, %ax
	movw	%ax, %es
	movw	%ax, %ds
	movw	%ax, %ss
	sti

	/** read image file into memory (1000:0000) */
	movw	$msgLoading, %si
	call	DisplayMessage

	movw	$2, %bx
	movw	Sectors, %cx
	xor		%dx, %dx

	movw	$0x1000, %ax
	movw	%ax, %es					/* destination for image */
	call	ReadSectors
	movw	$msgDone, %si
	call	DisplayMessage

	pushw	0x1000
	pushw	0x0000
	lret
FAILURE:
	movw	$msgFailure, %si
	call	DisplayMessage
	movb	$0x00, %ah
	int		$0x16				/* await keypress */
	int		$0x19				/* warm boot computer */

/*************************************************************************
 PROCEDURE DisplayMessage
 display ASCIIZ string at ds:si via BIOS
*************************************************************************/
DisplayMessage:
	lodsb						/*  load next character */
	orb		%al, %al			/* test for NUL character */
	jz		DONE
	movb	$0x0e, %ah			/* BIOS teletype */
	movw	$0x07, %bx			/* text attribute 7, page 0 */
	int		$0x10				/* invoke BIOS */
	jmp     DisplayMessage
DONE:
	ret

/*************************************************************************
 PROCEDURE ReadSectors
 reads <cx> sectors from disk starting at <bx> into memory location <es:0>
*************************************************************************/
ReadSectors:
	pushw	%bx
	pushw	%cx
	movb	$0x13, %al
	subb	%bl, %al
	movw	%bx, %cx
	xorw	%bx, %bx
	movb	$2, %ah
	int		$0x13
	jb		FAILURE
	movw	%es, %bx
	xorl	%ecx, %ecx
	movb	%al, %cl
	shlw	$5, %cx
	addw	%cx, %bx
	movw	%bx, %es
	popw	%cx
	popw	%bx
	xorb	$1, %dh
	jnz		other_head
	incb	%bh

other_head:
	movb	$1, %bl
	xorb	%ah, %ah
	subw	%ax, %cx
	jg		ReadSectors
	movb	$0x0C, %al
	movw	$0x3F2, %dx
	outb	%al, %dx
	ret

/*************************************************************************
 PROCEDURE ena20
 enables gate A20
*************************************************************************/
ena20:
	call	flush
	jnz		rretn
	movb	$0xd1, %al
	outb	%al, $0x64
	call	flush
	jnz		rretn
	movb	$0xDF, %al
	outb	%al, $0x60
flush:
	movl	$0x20000, %ecx
tagain:
	jmp		delay
delay:
	inb		$0x64, %al
	testb	$2, %al
	loopnz	tagain
rretn:	
	ret

gdt:	.word	0xFFFF
	.long	gdt
	.word	0

	.word	0xffff
	.word	0
	.long	0x8f9a00

	.word	0xffff
	.word	0
	.long	0x8f9200

msgLoading:	.string	"Loading Boot Image "
msgDone:	.ascii	"done"
			.byte	0x0d, 0x0a, 0x00
msgFailure:	.byte	0x0d, 0x0a
			.string	"ERROR : Press Any Key to Reboot"

/*		TIMES 508-($-$$) DB 0x90 */
Sectors:	.word	30
Signature:	.word	0xaa55

/*************************************************************************/
