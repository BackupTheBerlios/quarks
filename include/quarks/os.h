/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_OS_H
#define QUARKS_OS_H

/*
	This is generic stuff that is used both in userspace and in the kernel
	that doesn't really fit elsewhere.

	This file is included in both apps and the kernel, so don't include
	silly userland files here!
*/

#define QUARKS_MAX_NAME_LENGTH          32
#define QUARKS_MAX_CPUS                 8

#endif /* QUARKS_OS_H */

