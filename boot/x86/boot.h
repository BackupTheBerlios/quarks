/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#ifndef BOOT_H
#define BOOT_H

/*#include <quarks/types.h>*/

#define MBI_MEM               0x00000001
#define MBI_BOOTDEV           0x00000002
#define MBI_CMDLINE           0x00000004
#define MBI_MODS              0x00000008
#define MBI_SYMS_AOUT         0x00000010
#define MBI_SYMS_ELF          0x00000020
#define MBI_MMAP              0x00000040

#define IFS_MAGIC		0x38de7a19
#define MULTIBOOT_MAGIC	0x1badb002

typedef struct {
	unsigned int flags;
	unsigned int mem_lower;
	unsigned int mem_upper;
	unsigned int boot_device;
	char *cmdline;
	unsigned int mods_count;
	unsigned int mods_addr;
	union {
		struct {
			unsigned int tabsize;
			unsigned int strsize;
			unsigned int addr;
			unsigned int _pad_;
		} sym_aout;
		struct {
			unsigned int num;
			unsigned int size;
			unsigned int addr;
			unsigned int shndx;
		} sym_elf;
	} mb_sym_un;
	unsigned int mmap_length;
	unsigned int mmap_addr;
} multiboot_info;

typedef struct mb_head {
	unsigned int magic;			/* magic multiboot number */
	unsigned int flags;			/* flags for the loader */
	unsigned int checksum;		/* checksum above the header */
	struct mb_head *header_addr;/* where the header is */
	void *load_addr;			/* beginning of the text segment */
	void *load_end_addr;		/* end of the text segment */
	void *bss_end_addr;			/* end of the bss segment */
	void (*entry)();			/* adress of startup entry */
} multiboot_header;

#define IFS_DIR			0x01
#define IFS_FILE		0x02
//#define IFS_ROOT		0x04
#define IFS_BOOTSTRAP	0x04
#define IFS_COMPRESSED	0x10

typedef struct ifs_inode {
	union {
		void *ptr;
		unsigned int offset;		/* offset of file relative to the start of image */
	} data;
	unsigned int size;			/* size of file */
	unsigned int mode : 16;		/* mode in usual unix manner */
	unsigned int flags : 8;		/* flags */
	unsigned int namelen : 8;	/* length of name string */
	char name[0];
} ifs_inode;					/* name follows the inode immediately */

typedef struct ifs_superblock {
	multiboot_header mb_header;	/* let the multiboot header come first */
	unsigned int magic;			/* magic ifs superblock number */
	unsigned int flags;			/* currently unused */
	unsigned int checksum;		/* a checksum, calculated just like the multiboot checksum */
	union {
		ifs_inode *inode;		/* first directory or file entry */
		unsigned int offset;
	} root;
	unsigned int size;			/* size of the whole image */
	char name[16];				/* name of image */
} ifs_superblock;

#endif /* BOOT_H */
