/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_RESOURCE_H
#define QUARKS_RESOURCE_H

#include "kernel.h"

class Resource {
	public:
		Resource(const char *name);

		void SetName(const char *name);

	private:
		char rname[32];

		/* Disable copy constructors. */
		Resource(const Resource &);
		Resource &operator=(const Resource &);
};

#endif /* QUARKS_RESOURCE_H */
