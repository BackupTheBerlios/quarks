/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/

extern void InitIFS();
extern int open(const char *name);
extern int close(int fd);
extern int read(int fd, void *buf, size_t nbytes);
