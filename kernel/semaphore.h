/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_SEMAPHORE_H
#define QUARKS_SEMAPHORE_H

#include "resource.h"

class Semaphore : public Resource {
	public:
		Semaphore(const char *name, int count);

	private:
		int scount;

		/* Disable copy constructors. */
		Semaphore(const Semaphore &);
		Semaphore &operator=(const Semaphore &);
};

#endif /* QUARKS_SEMAPHORE_H */
