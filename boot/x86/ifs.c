/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#include <quarks/types.h>
#include <x86/console.h>
#include <errno.h>
#include <lzss.h>
#include <string.h>
#include "boot.h"
#include "ifs.h"

#define MAX_FILES	10

extern ifs_superblock ifs_super;
ifs_inode *current;

typedef struct file_t {
	ifs_inode *entry;
	unsigned int offset;
	lzss_decodestream zzstream;
} file_t;

file_t filedes[MAX_FILES];

void InitIFS()
{
	int i;

	for (i = 0; i < MAX_FILES; i++)
		filedes[i].entry = NULL;

	current = ifs_super.root.inode;
}

int open(const char *name)
{
	char substr[1024]; //, char buf[1024];
	int s, p, flags, file = 0;
	ifs_inode *entry, *ifs_end;

	Log("opening %s\n", name);
	Log("addr of current: %x, filedes: %x\n", (unsigned int) &current, (unsigned int)&filedes[0]);

	if (name[0] == '/') {
		s = 1;
		current = ifs_super.root.inode;
		ifs_end = (ifs_inode *)(0x00100000 + ifs_super.size);
		entry = current;
	} else {
		s = 0;
		ifs_end = (ifs_inode *)(current->data.offset + current->size);
		entry = current->data.ptr;		
	}

	//entry = current;

	for (p = s; ; p++) {
		if (name[p] == '/' || name[p] == 0) {	/* delimiter found: save this substring */
			if (p - s) {

				strncpy(substr, &name[s], p - s);
				substr[p - s] = 0;
				Log("search %s ", substr);
				Log("entry = %x, ifs_end = %x\n", (unsigned int) entry, (unsigned int) ifs_end);
				while (entry < ifs_end) {
					Log("addr of entry: %x ", (unsigned int) entry);
					if (!(strncmp(entry->name, substr, entry->namelen))) {
						flags = entry->flags & 0x0f;
						if (flags == IFS_DIR) {
							Log("found dir\n");
							if (name[p] == 0) {	/* end of name, but no file found */
								errno = ENOENT;
								return -1;
							}
							current = entry->data.ptr;
							ifs_end = (ifs_inode *)(entry->data.offset + entry->size);
							entry = current;
							break;
						} else if (flags == IFS_FILE) {
							Log("found file\n");
							if (name[p] == 0) {	/* reached a file: everything fine! */
								while (filedes[file].entry) {
									file++;
									if (file == MAX_FILES) {
										errno = ENFILE;
										return -1;
									}
								}
								filedes[file].entry = entry;
								filedes[file].offset = 0;
								if ((entry->flags & 0xf0) == IFS_COMPRESSED) {
									InitDecoder(&filedes[file].zzstream, entry->data.ptr);
								}
								errno = EOK;
								return file;
							} else {			/* found a file, but name still */
								errno = ENOTDIR;
								return -1;		/* contains characters: bail out */
							}
						}
					}
					entry = (ifs_inode *)((unsigned int)entry + sizeof(ifs_inode) + entry->namelen);
				}
				s = p + 1;
			} else
				s++;
			if (name[p] == 0) {
				errno = ENOENT;
				return -1;
			}
		}
	}
	errno = ENOENT;
	return -1;
}

int close(int fd)
{
	if (fd < MAX_FILES && fd >= 0) {
		if (filedes[fd].entry) {
			filedes[fd].entry = NULL;
			errno = EOK;
			return 0;
		}
	}
	errno = EBADF;
	return -1;
}

int read(int fd, void *buf, size_t nbytes)
{
	file_t *file;

	if (fd < 0 || fd >= MAX_FILES) {
		errno = EBADF;
		return -1;
	}

	file = &filedes[fd];

	if (!file->entry) {
		errno = EBADF;
		return -1;
	}

	if ((file->entry->flags & 0xf0) == IFS_COMPRESSED) {
		return Decode(&file->zzstream, file->entry->data.ptr, nbytes);
	} else {
		memcpy(buf, file->entry->data.ptr, nbytes);
		return nbytes;
	}
	return 0;
}
