/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_TYPES_H
#define QUARKS_TYPES_H

#define CHAR_BIT 8
#ifdef __cplusplus
#define NULL (0)
#else
#define NULL ((void *) 0)
#endif

#define ntohs(n) ( (((n) & 0xFF00) >> 8) | (((n) & 0x00FF) << 8) )
#define htons(n) ( (((n) & 0xFF00) >> 8) | (((n) & 0x00FF) << 8) )

typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef long long int64;
typedef int int32;
typedef short int16;
typedef char int8;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef int ssize_t;
typedef unsigned long int size_t;

typedef unsigned int time_t;

typedef unsigned int off_t;
typedef unsigned int mode_t;
typedef unsigned int dev_t;
typedef unsigned int ino_t;
typedef unsigned int nlink_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;

/* system resources */
typedef int port_t;
typedef int area_t;
typedef int semaphore_t;
typedef int thread_t;

#endif /* QUARKS_TYPES_H */
