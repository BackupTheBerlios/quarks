/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#ifndef MULTIBOOT_H
#define MULTIBOOT_H

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

#endif /* MULTIBOOT_H */
