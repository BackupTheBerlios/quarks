/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This program is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef BOOT_H
#define BOOT_H

typedef struct {
	int	offset;					/* offset of file relative to the start of image */
	int	size;					/* size of file */
	int	mode : 16;				/* mode in usual unix manner */
	int flags : 8;				/* flags */
	int namelen : 8;			/* length of name string */
} ifs_inode;					/* name follows the inode immediately */

#define IFS_MAGIC		0x38de7a19

typedef struct {
	int magic;					/* magic number */
	int flags;					/* currently unused */
	int reserved;				/* currently unused */
	char name[16];				/* name of image */
	ifs_inode *root;			/* root directory entry */
} ifs_superblock;

#endif /* BOOT_H */

