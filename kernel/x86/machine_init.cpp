/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "machine.h"
#include <quarks/x86/io.h>

__init_ Machine::Machine()
{
	/* Initialize basic stuff */
	InitTimer(1);		/* 1 ms scheduler timer */
	InitConsole();

	/* Initialize the processor tables...  */
	InitIDT();
	InitGDT();
	InitTSS();

	/* Initialize the heap */
	//InitHeap();
}

/* Reprograms the system timer to 'ms' milliseconds. */
void __init_ Machine::InitTimer(int ms)
{
	uint i = 1193182 / (1000 / ms);

	outb(PIT0_CONTROL, 0x34);
	outb(PIT0_COUNTER0, i);
	outb(PIT0_COUNTER0, (i >> 8));

	//add_idt_gate (0x8E00, 0x20, int_timer);
	//irq_enable (0);
}

/* Maps IRQs to software interrupts starting at 'base'. */
void __init_ Machine::InitIRQs(int base)
{
	outb_p(PORTA0, 0x11);	/* Master */
	outb_p(PORTB0, base);
	outb_p(PORTB0, 0x04);
	outb_p(PORTB0, 0x01);
	outb_p(PORTB0, 0xff);                      

	outb_p(PORTA1, 0x11);	/* Slave */
	outb_p(PORTB1, base + 8);
	outb_p(PORTB1, 0x02);
	outb_p(PORTB1, 0x01);
	outb_p(PORTB1, 0xff);                       
}

/* Initialize basic kernel console */
void __init_ Machine::InitConsole()
{
	/*screen = (unsigned char *) kmappages(0xB8, 2, 3);
	
    con_start((uint32) vscreen);
    con_attr(CON_YELLOW|0x08);
    con_clear();*/
}

void __init_ Machine::InitIDT()
{
	idt[0].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[1].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[2].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[3].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[4].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[5].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[6].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[7].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[8].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[9].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[10].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[11].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[12].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[13].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[14].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[15].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[16].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[17].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[18].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);

	for (int i = 19; i < 32; i++) {
		idt[i].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	}

	idt[32].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[33].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[34].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[35].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[36].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[37].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[38].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[39].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[40].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[41].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[42].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[43].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[44].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[45].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[46].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);
	idt[47].Fill(int_null, KERNEL_CS, ACC_INTR_GATE | ACC_P, 0);

	idt[48].Fill(syscall, KERNEL_CS, ACC_INTR_GATE | ACC_P | ACC_PL_U, 0);

	/* Load the IDT.  */
	Load(idt, 49);

	InitIRQs(20);
    EnableIRQ(0);
    EnableIRQ(2);
}

void __init_ Machine::InitGDT()
{
	/* Initialize the base TSS descriptor.  */
	gdt[BASE_TSS / 8].Fill(&ktss, sizeof(ktss) - 1, ACC_PL_K | ACC_TSS, 0);

	/* Initialize the 32-bit kernel code and data segment descriptors
	   to point to the base of the kernel linear space region.  */
	gdt[KERNEL_CS / 8].Fill(0, 0xffffffff, ACC_PL_K | ACC_CODE_R, SZ_32);
	gdt[KERNEL_DS / 8].Fill(0, 0xffffffff, ACC_PL_K | ACC_DATA_W, SZ_32);

	/* Descriptors that direct-map all linear space.  */
	gdt[LINEAR_CS / 8].Fill(0, 0xffffffff, ACC_PL_K | ACC_CODE_R, SZ_32);
	gdt[LINEAR_DS / 8].Fill(0, 0xffffffff, ACC_PL_K | ACC_DATA_W, SZ_32);

	Load(gdt, 6);
}

void __init_ Machine::InitTSS()
{
	register uint32 temp;
	/* Only initialize once.  */
	ktss.cs = KERNEL_CS;
	ktss.ds = ktss.es = ktss.ss = ktss.fs = ktss.gs = KERNEL_DS; 
	ktss.ss0 = KERNEL_DS;
	asm("movl %%esp, %0" : "=r" (temp));
	ktss.esp0 = temp; /* only temporary */
	ktss.esp1 = ktss.esp2 = ktss.ss1 = ktss.ss2 = 0;
	ktss.iomapbase = sizeof(ktss);
	ktss.ldt = ktss.debugtrap = 0;
	asm("movl %%cr3, %0":"=r" (temp));
	ktss.cr3 = temp;

	asm volatile("ltr %%ax": :"eax" (BASE_TSS));
}
