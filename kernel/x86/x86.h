/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_X86_X86_H
#define QUARKS_X86_X86_H

#include <quarks/types.h>

#define BASE_TSS	0x08
#define KERNEL_CS	0x10	/* Kernel's PL0 code segment */
#define KERNEL_DS	0x18	/* Kernel's PL0 data segment */
#define LINEAR_CS	0x20	/* PL0 code segment mapping to linear space */
#define LINEAR_DS	0x28	/* PL0 data segment mapping to linear space */
#define USER_CS		0x33	/* User-defined descriptor, RPL=3 */
#define USER_DS		0x3b	/* User-defined descriptor, RPL=3 */

/*
 * CR0
 */
#define	CR0_PG	0x80000000		/*	 enable paging */
#define	CR0_CD	0x40000000		/* i486: cache disable */
#define	CR0_NW	0x20000000		/* i486: no write-through */
#define	CR0_AM	0x00040000		/* i486: alignment check mask */
#define	CR0_WP	0x00010000		/* i486: write-protect kernel access */
#define	CR0_NE	0x00000020		/* i486: handle numeric exceptions */
#define	CR0_ET	0x00000010		/*	 extension type is 80387 */
					/*	 (not official) */
#define	CR0_TS	0x00000008		/*	 task switch */
#define	CR0_EM	0x00000004		/*	 emulate coprocessor */
#define	CR0_MP	0x00000002		/*	 monitor coprocessor */
#define	CR0_PE	0x00000001		/*	 enable protected mode */

/*
 * CR4
 */
#define CR4_VME	0x00000001		/* enable virtual intrs in v86 mode */
#define CR4_PVI	0x00000002		/* enable virtual intrs in pmode */
#define CR4_TSD	0x00000004		/* disable RDTSC in user mode */
#define CR4_DE	0x00000008		/* debug extensions (I/O breakpoints) */
#define CR4_PSE	0x00000010		/* page size extensions */
#define CR4_PGE	0x00000020		/* page global extensions */
#define CR4_MCE	0x00000040		/* machine check exception */
#define CR4_PCE	0x00000100		/* enable read perf counter instr */

/* Format of a "pseudo-descriptor", used for loading the IDT and GDT.  */
struct PseudoDescriptor {
	uint16 pad;
	uint16 limit;
	void *linear_base;
};

struct Descriptor {
	uint32
		limit_low:16,	/* limit 0..15 */
		base_low:16,	/* base  0..15 */
		base_med:8,		/* base  16..23 */
		access:8,		/* access byte */
		limit_high:4,	/* limit 16..19 */
		granularity:4,	/* granularity */
		base_high:8;	/* base 24..31 */

	void Fill(void *base, uint32 limit, uint8 acc, uint8 sizebits);
};

struct Gate {
	uint32
		offset_low:16,	/* offset 0..15 */
		selector:16,
		word_count:8,
		access:8,
		offset_high:16;	/* offset 16..31 */

	void Fill(void (*func)(...), uint16 selector, uint8 acc, uint8 word_cnt);
};

struct TSS {
	int		link;	/* segment number of previous task, if nested */
	int		esp0;	/* initial stack pointer ... */
	int		ss0;	/* and segment for ring 0 */
	int		esp1;	/* initial stack pointer ... */
	int		ss1;	/* and segment for ring 1 */
	int		esp2;	/* initial stack pointer ... */
	int		ss2;	/* and segment for ring 2 */
	int		cr3;	/* CR3 - page table directory physical address */
	int		eip;
	int		eflags;
	int		eax;
	int		ecx;
	int		edx;
	int		ebx;
	int		esp;	/* current stack pointer */
	int		ebp;
	int		esi;
	int		edi;
	int		es;
	int		cs;
	int		ss;		/* current stack segment */
	int		ds;
	int		fs;
	int		gs;
	int		ldt;	/* local descriptor table segment */
	uint16	debugtrap;	/* trap on switch to this task */
	uint16	iomapbase;	/* offset to start of IO permission bit map */
};

#define	SZ_32		0x4			/* 32-bit segment */
#define SZ_16		0x0			/* 16-bit segment */
#define	SZ_G		0x8			/* 4K limit field */

#define	ACC_A		0x01			/* accessed */
#define	ACC_TYPE	0x1e			/* type field: */

#define	ACC_TYPE_SYSTEM	0x00			/* system descriptors: */

#define	ACC_LDT		0x02			    /* LDT */
#define	ACC_CALL_GATE_16 0x04			    /* 16-bit call gate */
#define	ACC_TASK_GATE	0x05			    /* task gate */
#define	ACC_TSS		0x09			    /* task segment */
#define	ACC_CALL_GATE	0x0c			    /* call gate */
#define	ACC_INTR_GATE	0x0e			    /* interrupt gate */
#define	ACC_TRAP_GATE	0x0f			    /* trap gate */

#define	ACC_TSS_BUSY	0x02			    /* task busy */

#define	ACC_TYPE_USER	0x10			/* user descriptors */

#define	ACC_DATA	0x10			    /* data */
#define	ACC_DATA_W	0x12			    /* data, writable */
#define	ACC_DATA_E	0x14			    /* data, expand-down */
#define	ACC_DATA_EW	0x16			    /* data, expand-down,
							     writable */
#define	ACC_CODE	0x18			    /* code */
#define	ACC_CODE_R	0x1a			    /* code, readable */
#define	ACC_CODE_C	0x1c			    /* code, conforming */
#define	ACC_CODE_CR	0x1e			    /* code, conforming,
						       readable */
#define	ACC_PL		0x60			/* access rights: */
#define	ACC_PL_K	0x00			/* kernel access only */
#define	ACC_PL_U	0x60			/* user access */
#define	ACC_P		0x80			/* segment present */

/*
 * Values of the cpu_info.family field
 */
#define CPU_FAMILY_386		3
#define CPU_FAMILY_486		4
#define CPU_FAMILY_PENTIUM	5
#define CPU_FAMILY_PENTIUM_PRO	6

/*
 * Values of the cpu_info.type field
 */
#define CPU_TYPE_ORIGINAL	0
#define CPU_TYPE_OVERDRIVE	1
#define CPU_TYPE_DUAL		2

/*
 * CPU feature_flags bit definitions.
 */
#define CPUF_ON_CHIP_FPU	0x00000001	/* On-chip floating point */
#define CPUF_VM86_EXT		0x00000002	/* Virtual mode extensions */
#define CPUF_IO_BKPTS		0x00000004	/* I/O breakpoint support */
#define CPUF_4MB_PAGES		0x00000008	/* 4MB page support */
#define CPUF_TS_COUNTER		0x00000010	/* Timestamp counter */
#define CPUF_PENTIUM_MSR	0x00000020	/* Pentium model-spec regs */
#define CPUF_PAGE_ADDR_EXT	0x00000040	/* Page address extensions */
#define CPUF_MACHINE_CHECK_EXCP	0x00000080	/* Machine check exception */
#define CPUF_CMPXCHG8B		0x00000100	/* CMPXCHG8B instruction */
#define CPUF_LOCAL_APIC		0x00000200	/* CPU contains a local APIC */
#define CPUF_MEM_RANGE_REGS	0x00001000	/* memory type range regs */
#define CPUF_PAGE_GLOBAL_EXT	0x00002000	/* page global extensions */
#define CPUF_MACHINE_CHECK_ARCH	0x00004000	/* Machine check architecture */
#define CPUF_CMOVCC		0x00008000	/* CMOVcc instructions */

/*
 * Cache configuration descriptor entry values.
 */
#define CPUC_NULL			0x00
#define CPUC_CODE_TLB_4K_4W_64E		0x01
#define CPUC_CODE_TLB_4M_4W_4E		0x02
#define CPUC_DATA_TLB_4K_4W_64E		0x03
#define CPUC_DATA_TLB_4M_4W_8E		0x04
#define CPUC_CODE_L1_32B_4W_8KB		0x06
#define CPUC_DATA_L1_32B_2W_8KB		0x0a
#define CPUC_COMB_L2_32B_4W_128KB	0x41
#define CPUC_COMB_L2_32B_4W_256KB	0x42
#define CPUC_COMB_L2_32B_4W_512KB	0x43

/*
 * CR0
 */
#define	CR0_PG	0x80000000		/*	 enable paging */
#define	CR0_CD	0x40000000		/* i486: cache disable */
#define	CR0_NW	0x20000000		/* i486: no write-through */
#define	CR0_AM	0x00040000		/* i486: alignment check mask */
#define	CR0_WP	0x00010000		/* i486: write-protect kernel access */
#define	CR0_NE	0x00000020		/* i486: handle numeric exceptions */
#define	CR0_ET	0x00000010		/*	 extension type is 80387 */
					/*	 (not official) */
#define	CR0_TS	0x00000008		/*	 task switch */
#define	CR0_EM	0x00000004		/*	 emulate coprocessor */
#define	CR0_MP	0x00000002		/*	 monitor coprocessor */
#define	CR0_PE	0x00000001		/*	 enable protected mode */

/*
 * CR4
 */
#define CR4_VME	0x00000001		/* enable virtual intrs in v86 mode */
#define CR4_PVI	0x00000002		/* enable virtual intrs in pmode */
#define CR4_TSD	0x00000004		/* disable RDTSC in user mode */
#define CR4_DE	0x00000008		/* debug extensions (I/O breakpoints) */
#define CR4_PSE	0x00000010		/* page size extensions */
#define CR4_PGE	0x00000020		/* page global extensions */
#define CR4_MCE	0x00000040		/* machine check exception */
#define CR4_PCE	0x00000100		/* enable read perf counter instr */

#endif /* QUARKS_X86_X86_H */
