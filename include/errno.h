/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#ifndef ERRNO_H
#define ERRNO_H

#define EOK		0
#define ENOENT	1
#define ENOTDIR	2
#define ENFILE	3
#define EBADF	4

extern int errno;
extern char *errstrings[];

#endif /* ERRNO_H */
