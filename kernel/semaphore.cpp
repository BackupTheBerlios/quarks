/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "semaphore.h"

Semaphore::Semaphore(const char *name, int count)
 : Resource(name)
{
	scount = count;
}
