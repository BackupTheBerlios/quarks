/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#include <quarks/types.h>
#include <x86/console.h>
#include <errno.h>
#include <string.h>
#include "boot.h"
#include "ifs.h"

extern void _start();		/* declare some symbols */
extern void _end;			/* these define start and end of the text segment */

#define MULTIBOOT_FLAGS	0x00010002
#define IFS_FLAGS		0

/* this struct should come early since it contains the multiboot information
   which have to be within the first 8192 bytes. Don't move. */
ifs_superblock ifs_super __attribute__((__section__(".text"))) =
{
	/* multiboot header */
{	MULTIBOOT_MAGIC,			/* magic multibootblock number */
	MULTIBOOT_FLAGS,			/* flags for the loader */
	0 - MULTIBOOT_MAGIC - MULTIBOOT_FLAGS,	/* checksum above header */
	&ifs_super.mb_header,		/* header adress */
	(void *)BASE,				/* where to start loading the image */
	&_end,						/* XXX: needs to be fixed */
	&_end,						/* should be _bss_end, but we clear bss section ourselves */
	BASE + _start		},		/* function entry */

	/* ifs superblock */
	IFS_MAGIC,					/* magic ifs superblock number */
	IFS_FLAGS,					/* flags, currently unused */
	0 - IFS_MAGIC - IFS_FLAGS,	/* checksum above ifs header */
	{ 0 },						/* offset of first inode, to be filled in by mkifs */
	0,							/* size of image, to be filled in by mkifs */
	"Quarks bootfs"				/* name of image */
};

int startup(multiboot_info *mbinfo)
{
	char buf[1024];
	int test;
	/*uint32 *flat;
	int k;*/

   
	InitConsole(CON_SCREEN);
	InitIFS();
	Log("Starting Quarks...\n");
	Log("Found superblock at %x, multiboot info at %x\n", (int)&ifs_super, (int)mbinfo);
	Log("Image size: %d, first entry at: %x\n", ifs_super.size, ifs_super.root.offset);
	Log("name = %s\n", ifs_super.name);
	Log("%d\n", ifs_super.root.inode->namelen);
	strncpy(buf, (char *)ifs_super.root.inode->name, ifs_super.root.inode->namelen);
	buf[ifs_super.root.inode->namelen] = 0;
	Log("name[1] = %s, size = %d\n", buf, ifs_super.root.inode->size);

	/*flat = (uint32 *)((4096 * (memsize / 4096)) - 4096 * 3);
    
	for(k=0;k<1024;k++){
        flat[k] = 0;
        flat[1024+k] = 4096*k | 3;        
        flat[2048+k] = k > bd->bd_entry[2].size ? 0 : */ /* XXX! EEK! */
           /* ( (bd->bd_entry[2].offset * 4096 + 0x100000) + 4096 * k) | 3;

    }

       */ /* point the pdir at ptab1 and ptab2 */
    /*flat[0] = (uint32) &flat[1024] | 3;
    flat[512] = (uint32) &flat[2048] | 3;
*/
        /* map the pdir, ptab1, ptab2 starting at 0x803FD000 */
    /*flat[2048+1023] = ((uint32) flat + 2*4096) | 3;
    flat[2048+1022] = ((uint32) flat + 1*4096) | 3;
    flat[2048+1021] = ((uint32) flat) | 3;

    asm("mov %0, %%cr3": :"r" (flat));
	asm("movl $0x80000001, %eax ; mov %eax, %cr0");*/

	test = open("/system/kernel_x86");
	if (test != -1) {
		Log("found file \n");
	} else {
		Log("Error: %s\n", strerror(errno));
	}

	Log("Starting kernel...\n");

	return 0;
}

int load_elf(const char *name, unsigned int virt_adress)
{
	//struct elf_header;
	open("/system/kernel_x86");

	//read(&elf_header);

	//close();
}
