/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_INTEL_MACHINE_H
#define QUARKS_INTEL_MACHINE_H

#include "x86.h"

#define __init_		__attribute__((__section__ (".init")))

extern "C" void syscall();
extern "C" void int_null();

class Machine {
	public:
		__init_ Machine();

		void InitTimer(int ms);
		void InitIRQs(int base);
		void InitGDT();
		void InitIDT();
		void InitTSS();
		void InitConsole();

		void Load(Descriptor *desc_table, int numentries);
		void Load(Gate *gate_table, int numentries);

		void EnableIRQ(int irq);
		void DisableIRQ(int irq);

		void Switch();

		inline void cli()	{ asm("cli"); }
		inline void sti()	{ asm("sti"); }

		inline void FlushTLB()
			{	int junk;
				asm("mov %%cr3, %0 ; mov %0, %%cr3" : : "r" (junk));
			}
		inline void FlushPTE(unsigned int addr)
			{	/* if I386 then flush_tlb (); else */
				asm ("invlpg (%0)" : : "r" (addr));
			}

	private:
		struct {
			unsigned 	stepping	: 4;
			unsigned 	model		: 4;
			unsigned 	family		: 4;
			unsigned	type		: 2;
			unsigned	reserved	: 18;
			unsigned	feature_flags;
			char		vendor_id[12];
			unsigned char	cache_config[16];
		} cpu_info;

		Descriptor	gdt[6];
		Gate		idt[49];
		TSS			ktss;
};

#define PIT0_CONTROL	0x43
#define PIT0_COUNTER0	0x40
#define PIT0_COUNTER1	0x41
#define PIT0_COUNTER2	0x42

#define PIT1_CONTROL	0x4b
#define PIT1_COUNTER0	0x48
#define PIT1_COUNTER1	0x49
#define PIT1_COUNTER2	0x4a	

#define PORTA0 0x20
#define PORTB0 0x21
#define PORTA1 0xA0
#define PORTB1 0xA1

#endif /* QUARKS_INTEL_MACHINE_H */
