/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_MUTEX_H
#define QUARKS_MUTEX_H

#include <quarks/types.h>
#include "queue.h"
#include "resource.h"

class Mutex : public Resource {
	public:
		Mutex(const char *name);

		void Lock();
		void Unlock();

	private:
		uchar		busy_flag;
		Queue		*waitqueue;

		/* Disable copy constructors. */
		Mutex(const Mutex &);
		Mutex &operator=(const Mutex &);
};

#endif /* QUARKS_MUTEX_H */
