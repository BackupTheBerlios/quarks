/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "quarks/intel/io.h"
#include "machine.h"

#ifdef BIG_SINGLE_SOURCE
#include "machine_init.cpp"
#include "x86.cpp"
#endif /* BIG_SINGLE_SOURCE */

void Machine::Load(Descriptor *desc_table, int numentries)
{
	PseudoDescriptor pdesc;

	/* Create a pseudo-descriptor describing the GDT.  */
	pdesc.limit = numentries * 8 - 1;
	pdesc.linear_base = desc_table;

	/* Load it into the CPU
	 * and reload all the segment registers from the new GDT.
	 */
	asm volatile("lgdt %0" : : "m" (pdesc.limit));

	asm volatile("ljmp	%0,$1f;	1:" : : "i" (KERNEL_CS));
	//ds = KERNEL_DS;
	//es = KERNEL_DS;
	//ss = KERNEL_DS;

	/*
	 * Clear out the FS and GS registers by default,
	 * since they're not needed for normal execution of GCC code.
	 */
	//fs = 0;
	//gs = 0;
}

void Machine::Load(Gate *gate_table, int numentries)
{
	PseudoDescriptor pdesc;

	/* Create a pseudo-descriptor describing the GDT.  */
	pdesc.limit = numentries * 8 - 1;
	pdesc.linear_base = gate_table;

	/* Load it into the CPU
	 * and reload all the segment registers from the new GDT.
	 */
	asm volatile("lidt %0" : : "m" (pdesc.limit));
}

void Machine::EnableIRQ(int irq)
{
	if (irq < 8)
		outb(PORTB0, (inb(PORTB0) & ~(1 << irq)));
	else
		outb(PORTB1, (inb(PORTB1) & ~(1 << (irq - 8))));
}

void Machine::DisableIRQ(int irq)
{
	if(irq < 8)
		outb(PORTB0, (inb(PORTB0) | (1 << irq)));
	else
		outb(PORTB1, (inb(PORTB1) | (1 << (irq - 8))));
}

void Machine::Switch()
{
	cli();
	sti();
}

asm (
"	.text							\n"
"	.align	16						\n"
"	.globl	syscall					\n"
"syscall:							\n"
"	iret							\n");

asm (
"	.text				\n"
"	.align	16			\n"
"	.globl	int_null	\n"
"int_null:				\n"
"	pushal				\n"
"	movb	$0x20,%al	\n"
"	outb	%al,$0x20	\n"
"	popal				\n"
"	iret				\n");
