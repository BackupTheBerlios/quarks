/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "x86.h"

/* Fill a segment descriptor.  */
void Descriptor::Fill(void *base, uint32 limit, uint8 acc, uint8 sizebits)
{
	if (limit > 0xfffff) {
		limit >>= 12;
		sizebits |= SZ_G;
	}
	limit_low = limit & 0xffff;
	base_low = (uint32)base & 0xffff;
	base_med = ((uint32)base >> 16) & 0xff;
	access = acc | ACC_P;
	limit_high = limit >> 16;
	granularity = sizebits;
	base_high = (uint32)base >> 24;
}

/* Fill a gate with particular values.  */
void Gate::Fill(void (*func)(...), uint16 sel, uint8 acc, uint8 word_cnt)
{
	offset_low = (uint32)func & 0xffff;
	selector = sel;
	word_count = word_cnt;
	access = acc | ACC_P;
	offset_high = ((uint32)func >> 16) & 0xffff;
}
