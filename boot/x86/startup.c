/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#include <quarks/types.h>
#include "multiboot.h"
#include "boot.h"
#include <x86/console.h>

void (*start)(void);

struct _kinfo
{
    uint32 memsize;
    uint32 entry_ebp;
    boot_dir *bd;
    char *param;
} *kinfo = (struct _kinfo *) 0x00102000;

static unsigned int multiboot[] __attribute__ ((__section__ (".text"))) =
{
	0x1badb002,
	0x00010002,
	(unsigned int) 0 - 0x1badb002 - 0x00010002,
	(unsigned int) multiboot,
	0x100000,
	0x14a000,
	0x14a000,
	0x101074
};

int startup(boot_dir *bd, uint32 memsize, uint32 entry_ebp, char *p)
{
	uint32 *flat;
	int k;

	kinfo->memsize = memsize; 
	kinfo->entry_ebp = entry_ebp; 
	kinfo->bd = bd; 
	kinfo->param = p;
    
	InitConsole(CON_SCREEN);
	Log("Starting Quarks...\n");
	Log("Found %d kBytes memory, bdir at %x, ebp = %x\n", memsize / 1024, (int)bd, entry_ebp);
	Log("name[0] = %s\n", bd->bd_entry[0].name);
	Log("name[1] = %s\n", bd->bd_entry[1].name);
	Log("name[2] = %s\n", bd->bd_entry[2].name);

	flat = (uint32 *)((4096 * (memsize / 4096)) - 4096 * 3);
    
	for(k=0;k<1024;k++){
        flat[k] = 0;
        flat[1024+k] = 4096*k | 3;        
        flat[2048+k] = k > bd->bd_entry[2].size ? 0 : /* XXX! EEK! */
            ( (bd->bd_entry[2].offset * 4096 + 0x100000) + 4096 * k) | 3;

    }

    start = (void (*)(void)) ((uint32)0x80000000 + (uint32)(bd->bd_entry[2].entry));

        /* point the pdir at ptab1 and ptab2 */
    flat[0] = (uint32) &flat[1024] | 3;
    flat[512] = (uint32) &flat[2048] | 3;

        /* map the pdir, ptab1, ptab2 starting at 0x803FD000 */
    flat[2048+1023] = ((uint32) flat + 2*4096) | 3;
    flat[2048+1022] = ((uint32) flat + 1*4096) | 3;
    flat[2048+1021] = ((uint32) flat) | 3;

    asm("mov %0, %%cr3": :"r" (flat));
	asm("movl $0x80000001, %eax ; mov %eax, %cr0");

	Log("Starting kernel...\n");

    start();

	return 0;
}
