/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "kernel.h"

#ifdef BIG_SINGLE_SOURCE
#include "clan.cpp"
#include "condvar.cpp"
#include "console.cpp"
#include "heap.cpp"
#include "kernel.cpp"
#include "mutex.cpp"
#include "queue.cpp"
#include "resource.cpp"
#include "semaphore.cpp"
#include "snprintf.cpp"
#include "string.cpp"
#include "team.cpp"
#include "thread.cpp"
#include "version.cpp"
#endif /* BIG_SINGLE_SOURCE */

Kernel Quarks;

int __init_ main()
{
	/* When we reach here, the kernel's constructor has already
	   been run (by the startup code in crt0.S) and all the kernel
	   structures are initialized. So we just do the first context
	   switch. */
	Quarks.Switch();
	return 0;
}
