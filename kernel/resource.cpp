/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "resource.h"

Resource::Resource(const char *name)
{
	SetName(name);
}

void Resource::SetName(const char *name)
{
	if (name) {
		int i;
		for (i = 0; *name && (i < 31); i++) {
			rname[i] = *name;
			name++;
		}
		rname[i] = 0;
	} else {
		rname[0] = 0;
	}
}
