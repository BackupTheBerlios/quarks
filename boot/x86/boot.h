/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#ifndef BOOT_H
#define BOOT_H

#define BOOTDIR_NAMELEN         32
#define BOOTDIR_MAX_ENTRIES     64
#define BOOTDIR_DIRECTORY       "SBBB/Directory"

#define MBI_MEM               0x00000001
#define MBI_BOOTDEV           0x00000002
#define MBI_CMDLINE           0x00000004
#define MBI_MODS              0x00000008
#define MBI_SYMS_AOUT         0x00000010
#define MBI_SYMS_ELF          0x00000020
#define MBI_MMAP              0x00000040

typedef struct {
	unsigned int flags, mem_lower, mem_upper, boot_device;
	char *cmdline;
	unsigned int mods_count, mods_addr;
	union {
		struct {
			unsigned int tabsize, strsize, addr, _pad_;
		} sym_aout;
		struct {
			unsigned int num, size, addr, shndx;
		} sym_elf;
	} mb_sym_un;
	unsigned int mmap_length, mmap_addr;
} multiboot_info;

typedef struct {
	int	offset;					/* offset of file relative to the start of image */
	int	size;					/* size of file */
	int	mode : 16;				/* mode in usual unix manner */
	int flags : 8;				/* flags */
	int namelen : 8;			/* length of name string */
} ifs_inode;					/* name follows the inode immediately */

#define IFS_MAGIC		0x38de7a19

typedef struct {
	int jump;					/* just a short jump to the real code start */
	unsigned int magic;			/* magic number */
	int flags;					/* currently unused */
	int reserved;				/* currently unused */
	char name[16];				/* name of image */
	ifs_inode *root;			/* root directory entry */
	multiboot_info minfo;		/* multiboot info */
} ifs_superblock;

typedef struct {
    char name[BOOTDIR_NAMELEN]; /* name of loaded object, zero terminated */
    int  offset;   /* offset of object relative to the start of boot_dir  */
    int  type;     /* object type designator                              */
    int  size;     /* size of loaded object (pages)                       */
    int  vsize;    /* size loaded object should occupy when mapped in     */
    int  extra0;
    void (*entry)(void);
    int  extra2;
    int  extra3;
} boot_entry;

typedef struct {
    boot_entry bd_entry[BOOTDIR_MAX_ENTRIES];
} boot_dir;

/* void _start(uint32 mem, char *params, boot_dir *bd); */

#define BE_TYPE_NONE         0  /* empty entry                              */
#define BE_TYPE_DIRECTORY    1  /* directory (entry 0)                      */
#define BE_TYPE_BOOTSTRAP    2  /* bootstrap code object (entry 1)          */
#define BE_TYPE_CODE         3  /* executable code object                   */
#define BE_TYPE_DATA         4  /* raw data object                          */
#define BE_TYPE_ELF32        5  /* 32bit ELF object                         */

/* for BE_TYPE_CODE */
#define be_code_vaddr be_extra0 /* virtual address (rel offset 0)           */
//#define be_code_ventr be_extra1 /* virtual entry point (rel offset 0)       */

#endif /* BOOT_H */
