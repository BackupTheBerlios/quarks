/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "kernel.h"

__init_ Kernel::Kernel()
 : Machine()
{
	/*
	idle_thread = new IdleThread();
	reaper_thread = new ReaperThread();
	pager_thread = new PagerThread();

	Switch();

	kout << "panic: returned from scheduler?";
	asm("hlt");
	*/
}

/*void kernel::IdleThread()
{    
	for (;;) {
		asm("hlt");        
	}    
}*/
