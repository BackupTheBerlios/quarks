/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
	.text
	.code16

	.globl _start

/* Entry point is at linear adress 0x00 */
_start:
	jmp	start
	nop
     
OEM_ID:				.string "OpenBLT "
BytesPerSector:		.word	0x0200
SectorsPerCluster:	.byte	0x01
ReservedSectors:	.word	0x0001
TotalFATs:			.byte	0x02
MaxRootEntries:		.word	0x00E0
TotalSectorsSmall:	.word	0x0B40
MediaDescriptor:	.byte	0xF0
SectorsPerFAT:		.word	0x0009
SectorsPerTrack:	.word	0x0012
NumHeads:			.word	0x0002
HiddenSectors:		.long	0x00000000
TotalSectorsLarge:	.long	0x00000000
DriveNumber:		.byte	0x00
absoluteHead:		.byte	0x00	/* was originally Flags */
Signature:			.byte	0x29
VolumeID:			.long	0xFFFFFFFF
VolumeLabel:		.string	"QASI__BOOT"
SystemID:			.string	"FAT12   "

start:
	/* code located at 0000:7C00, adjust segment registers */
	cli
	movw	$0x07C0, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	/* create stack */
	xorw	%ax, %ax
	movw	%ax, %ss
	movw	$0xffff, %sp
	sti
	/* post message */
	movw	$msgLoading, %si
	call	DisplayMessage
load_root:
	/* compute size of root directory and store in cx */
	xorw	%cx, %cx
	xorw	%dx, %dx
	movw	$0x0020, %ax		/* 32 byte directory entry */
	mulw	MaxRootEntries		/* total size of directory */
	divw	BytesPerSector		/* sectors used by directory */
	xchgw	%cx, %ax
	/* compute location of root directory and store in ax */
	movb	TotalFATs, %al		/* number of FATs */
	mulw	SectorsPerFAT		/* sectors used by FATs */
	addw	ReservedSectors, %ax	/* adjust for bootsector */
	movw	%ax, datasector		/* base of root directory */
	addw	%cx, datasector
	/* read root directory into memory (7C00:0200) */
	mov		$0x0200, %bx		/* copy root dir above bootcode */
	call	ReadSectors
	/* browse root directory for binary image */
	movw	MaxRootEntries, %cx	/* load loop counter */
	movw	$0x0200, %di		/* locate first root entry */
.LOOP:
	pushw	%cx
	movw	$0x000B, %cx		/* eleven character name */
	movw	$ImageName, %si		/* image name to find */
	pushw	%di
	rep		cmpsb				/* test for entry match */
	popw	%di
	je		LOAD_FAT
	popw	%cx
	addw	$0x0020, %di		/* queue next directory entry */
	loop	.LOOP
	jmp		FAILURE
LOAD_FAT:
	/* save starting cluster of boot image */
	movw	$msgCRLF, %si
	call	DisplayMessage
	movw	0x001A(%di), %dx
	movw	%dx, cluster			/* file's first cluster */
	/* compute size of FAT and store in cx */
	xorw	%ax, %ax
	movb	TotalFATs, %al			/* number of FATs */
	mulw	SectorsPerFAT			/* sectors used by FATs */
	movw	%ax, %cx
	/* compute location of FAT and store in ax */
	movw	ReservedSectors, %ax	/* adjust for bootsector */
	/* read FAT into memory (7C00:0200) */
	movw	$0x0200, %bx			/* copy FAT above bootcode */
	call	ReadSectors
	/* read image file into memory (1000:0000) */
	movw	$msgCRLF, %si
	call	DisplayMessage
	movw	$0x1000, %ax
	movw	%ax, %es				/* destination for image */
	xorw	%bx, %bx			/* destination for image */
	pushw	%bx
LOAD_IMAGE:
	movw	cluster, %ax			/* cluster to read */
	popw	%bx						/* buffer to read into */
	call	ClusterLBA				/* convert cluster to LBA */
	xorw	%cx, %cx
	movb	SectorsPerCluster, %cl	/* sectors to read */
	call	ReadSectors
	pushw	%bx						/* compute next cluster */
	movw	cluster, %ax			/* identify current cluster */
	movw	%ax, %cx				/* copy current cluster */
	movw	%ax, %dx				/* copy current cluster */
	shrw	$0x0001, %dx			/* divide by two */
	addw	%dx, %cx				/* sum for (3/2) */
	movw	$0x0200, %bx			/* location of FAT in memory */
	addw	%cx, %bx				/* index into FAT */
	movw	(%bx), %dx				/* read two bytes from FAT */
	testw	$0x0001, %ax
	jnz		.ODD_CLUSTER
.EVEN_CLUSTER:
	andw	$0x0fff, %dx			/* take low twelve bits */
	jmp     .DONE
.ODD_CLUSTER:
	shrw	$0x0004, %dx			/* take high twelve bits */
.DONE:
	movw	%dx, cluster			/* store new cluster */
	cmpw	0x0FF0, %dx				/* test for end of file */
	jb		LOAD_IMAGE
DONE:
	movw	$msgCRLF, %si
	call	DisplayMessage
	pushw	$0x1000
	pushw	$0x0000
	lret
FAILURE:
	movw	$msgFailure, %si
	call	DisplayMessage
	movb	$0x00, %ah
	int		$0x16					/* await keypress */
	int		$0x19					/* warm boot computer */
     
/************************************************************************
  PROCEDURE DisplayMessage
  display ASCIIZ string at ds:si via BIOS
*************************************************************************/
DisplayMessage:
	lodsb							/* load next character */
	orb		%al, %al				/* test for NUL character */
	jz		1f
	movb	$0x0E, %ah				/* BIOS teletype */
	movw	$0x07, %bx				/* display page 0, text attribute 7 */
	int		$0x10					/* invoke BIOS */
	jmp		DisplayMessage
1:
	ret
     
/*************************************************************************
  PROCEDURE ReadSectors
  reads cx sectors from disk starting at ax into memory location es:bx
*************************************************************************/
ReadSectors:
	movw	$0x0005, %di			/* five retries for error */
.SECTORLOOP:
	pushw	%ax
	pushw	%bx
	pushw	%cx
	call	LBACHS
	movw	$0x0201, %ax		/* BIOS read one sector */
	movw	absoluteTrack, %cx		/* track */
	//movb	absoluteSector, %cl		* sector */
	movb	absoluteHead, %dh		/* head */
	movb	DriveNumber, %dl		/* drive */
	int		$0x13					/* invoke BIOS */
	jnc		.SUCCESS				/* test for read error */
	xorw	%ax, %ax				/* BIOS reset disk */
	int		$0x13					/* invoke BIOS */
	decw	%di						/* decrement error counter */
	popw	%cx
	popw	%bx
	popw	%ax
	jnz		.SECTORLOOP				/* attempt to read again */
	int		$0x18
.SUCCESS:
	movw	$msgProgress, %si
	call	DisplayMessage
	popw	%cx
	popw	%bx
	popw	%ax
	addw	BytesPerSector, %bx		/* queue next buffer */
	incw	%ax						/* queue next sector */
	loop	ReadSectors				/* read next sector */
	ret
     
/************************************************************************
  PROCEDURE clusterLBA
  convert FAT cluster into LBA addressing scheme
  LBA = (cluster - 2) * sectors per cluster
*************************************************************************/
ClusterLBA:
	decw	%ax						/* zero base cluster number */
	decw	%ax
	xorw	%cx, %cx
	movb	SectorsPerCluster, %cl	/* convert byte to word */
	mulw	%cx
	addw	datasector, %ax			/* base data sector */
	ret
     
/*************************************************************************
  PROCEDURE LBACHS
  convert ax LBA addressing scheme to CHS addressing scheme
  absolute sector = (logical sector / sectors per track) + 1
  absolute head   = (logical sector / sectors per track) MOD number of heads
  absolute track  = logical sector / (sectors per track * number of heads)
***************************************************************************/
LBACHS:
	xorw	%dx, %dx				/* prepare dx:ax for operation */
	divw	SectorsPerTrack			/* calculate */
	incb	%dl						/* adjust for sector 0 */
	movb	%dl, absoluteSector
	xorw	%dx, %dx				/* prepare dx:ax for operation */
	divw	NumHeads				/* calculate */
	movb	%dl, absoluteHead
	movb	%al, absoluteTrack
	ret

absoluteTrack:	.byte	0x00
absoluteSector:	.byte	0x00
     
datasector:		.word	0x0000
cluster:		.word	0x0000
ImageName:		.ascii	"OPENBLT IFS"
msgLoading:		.byte	0x0D, 0x0A
				.string	"Loading Boot Image"
msgCRLF:		.byte	0x0D, 0x0A, 0x00
msgProgress:	.string	"."
msgFailure:		.byte	0x0D, 0x0A
				.string	"ERROR: Press Key to Reboot"
     
         /* TIMES 510-($-$$) DB 0*/
		.word 0xaa55
