/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../boot/x86/boot.h"
#include "../include/lzss.h"

void die(char *s, char *a)
{
    fprintf(stderr,"error: ");
    fprintf(stderr, s, a);
    fprintf(stderr,"\n");
    exit(1);
}

void *loadfile(char *file, int *size)
{
    int fd;
    char *data;
    struct stat info;
    
    if((fd = open(file, O_RDONLY)) != -1){
        if(fstat(fd, &info)){
            close(fd);
            *size = 0;
            return NULL;
        }
        data = (char *) malloc(info.st_size);
        if(read(fd, data, info.st_size) != info.st_size) {
            close(fd);
            *size = 0;
            return NULL;
        }
        close(fd);
        *size = info.st_size;
        return data;
    }
    *size = 0;
    return NULL;
}

typedef struct arg {
	struct arg *next;
	char *name;
	int nlen;
} arg;

typedef struct command {
	struct command *next;
	char *name;
	int nlen;
	struct arg *firsta;
} command;

typedef struct section {
	struct section *next;
	char *name;
	int nlen;
	struct command *firstc;
} section;

void print_sections(section *first)
{
	command *p;
	arg *q;
	char buf[1000];
    
	while (first) {
		strncpy(buf, first->name, first->nlen);
		buf[first->nlen] = 0;
		printf("\nSection %s\n", buf);
		for(p = first->firstc; p; p = p->next) {
			strncpy(buf, p->name, p->nlen);
			buf[p->nlen] = 0;
			printf("Command %s\n", buf);
			for(q = p->firsta; q; q = q->next) {
				strncpy(buf, q->name, q->nlen);
				buf[q->nlen] = 0;
				printf("Arg %s\n", buf);
			}
		}
		first = first->next;
	}
}

#define nUnknown	1
#define nComment	2
#define nSection	3
#define nCommand	4
#define nArgument	5

section *load_ini(char *file)
{
	char *d, *end, *sstart;
	section *s, *sections = NULL;
	command *c;
	arg *a;
	int line, size, state, argstate;
	
	if((d = loadfile(file, &size)) == NULL) {
		fprintf(stderr, "Error reading file %s\n", file);
		exit(1);
    }

	line = 0;
	sstart = NULL;
	s = NULL;
	c = NULL;
	a = NULL;
	end = d + size;

	state = nUnknown;
	while (d < end) {
		switch (state) {
			case nUnknown:
				switch (*d) {
					case '#':
						state = nComment;
						break;
					case '[':
						state = nSection;
						break;
					case '>':
						if (!s) {
							fprintf(stderr, "Error: command outside section in line %d\n", line);
							exit(1);
						}
						state = nCommand;
						break;
					case '\n':
						line++;
					case '\t':
					case ' ':							/* ignore newlines, tabs and whitespaces */
						break;
					default:
						if (!s) {
							fprintf(stderr, "Error: '[' expected at line %d\n", line);
							exit(1);
						}
						if (!c) {
							fprintf(stderr, "Error: '>' expected at line %d\n", line);
							exit(1);
						}
						state = nArgument;		/* hmm ... no section, no command ... */
						sstart = d;					/* ... must be argument to last command */
				}
				break;
			case nComment:
				switch (*d) {
					case '\n':					/* comment ends at end of line */
						line++;
						state = nUnknown;
						break;
					default:					/* all other characters allowed in comment */
						break;
				}
				break;
			case nSection:
				switch (*d) {
					case '\n':					/* section must close at same line */
						line++;
						fprintf(stderr, "Error: ']' expected in line %d\n", line);
						exit(1);
						break;
					case ']':
						if (!sections) {		/* first section in list */
							/* printf("allocating first section\n"); */
							sections = s = malloc(sizeof(section));
						} else {
							/* printf("allocating another section\n"); */
							s->next = malloc(sizeof(section));
							s = s->next;
						}
						s->name = sstart;
						s->nlen = d - sstart;
						s->firstc = NULL;
						s->next = NULL;
						sstart = NULL;
						state = nUnknown;
						break;
					default:						/* all other characters allowed */
						if (!sstart)
							sstart = d;
						break;	
				}
				break;
			case nCommand:
				switch (*d) {
					case '\n':
						line++;
					case ' ':
					case '\t':					/* command ends at either space, newline or tab */
						if (!s->firstc) {
							s->firstc = c = malloc(sizeof(command));
						} else {
							c->next = malloc(sizeof(command));
							c = c->next;
						}
						c->name = sstart;
						c->nlen = d - sstart;
						c->firsta = NULL;
						c->next = NULL;
						sstart = NULL;
						state = nUnknown;
						break;
					case '>':
						fprintf(stderr, "Error: character '>' not allowed in line %d\n", line);
						exit(1);
					default:					/* all other characters allowed */
						if (!sstart)
							sstart = d;
						break;
				}
				break;
			case nArgument:
				switch (*d) {
					case '\n':					/* the newline marks the end of an argument */
						line++;
					case '#':					/* a comment too */
						if (!c->firsta) {
							/* printf("allocating first arg\n"); */
							c->firsta = a = malloc(sizeof(arg));
						} else {
							/* printf("allocating next arg\n"); */
							a->next = malloc(sizeof(arg));
							a = a->next;
						}
						a->name = sstart;
						a->nlen = d - sstart;
						a->next = NULL;
						sstart = NULL;
						state = (*d == '\n') ? nUnknown : nComment;
						break;
					case '\t':
					case ' ':					/* skip all whitespaces and tabs */
						break;
					case '[':					/* this one is allowed within an argument */
						if (!sstart) {			/* but at start of a line it indicates a new section */
							state = nSection;
						}
						break;
					case '>':					/* this one too */
						if (!sstart) {			/* but at start of line it's a new command */
							state = nCommand;
						}
						break;
					default:
						break;
				}
				break;
		}
		d++;
	}
	return sections;
}

/* Returns the first section that matches 'name'
 * or NULL if no such section exists */
section *find_section(section *slist, const char *name)
{
	int nlen = strlen(name);

	while (slist) {
		if (nlen == slist->nlen && !strncmp(name, slist->name, nlen))
			break;
		slist = slist->next;
	}
	return slist;
}

/* Returns the first command that matches 'name' within section 's' 
 * or NULL if no such command exists */
command *find_command(section *s, char *name)
{
	command *c = s->firstc;
	int nlen = strlen(name);

	while (c) {
		if (nlen == c->nlen && !strncmp(name, c->name, nlen))
			break;
		c = c->next;
	}
	return c;
}

/* Returns first argument of command 'c' or NULL if no argument */
arg *get_arg(command *c)
{
	return c->firsta;
}

/* Returns next argument after argument 'a' or NULL */
arg *get_nextarg(arg *a)
{
	return a->next;
}

/*#define IFS_DIR			0x01
#define IFS_FILE		0x02
#define IFS_ROOT		0x04
#define IFS_BOOTSTRAP	0x10
#define IFS_COMPRESSED	0x20*/

typedef struct ifs_entry {
	char *name;
	char *realname;
	int flags;
	size_t size;
	unsigned int offset;
	struct ifs_entry *next;
	struct ifs_entry *subdirs;
} ifs_entry;

ifs_entry *root = 0, **current;
static char *searchpath = 0;
static char *bootstrap = 0;
static ifs_entry *bootstrap_entry = 0;
static lzss_encodestream *zip_mem = 0;
static unsigned int base = 0;

/* duplicates a string up to len bytes and null-terminates it */
char *strndup(const char *string, int len)
{
	char *buf = malloc(len + 1);
	strncpy(buf, string, len);
	buf[len] = 0;
	return buf;
}

/* same as strncpy, but null-terminates the destination */
void strncpyt(char *dest, const char *src, size_t len)
{
	strncpy(dest, src, len);
	dest[len] = 0;
}	

/* add a directory to the tree */
void add_directory(char *name)
{
	char substr[1024];
	int s, p;
	ifs_entry *dir;

	if (name[0] == '/') {
		s = 1;
		current = &root;
	} else {
		s = 0;
	}

	for (p = s; ; p++) {
		if (name[p] == '/' || name[p] == 0) {	/* delimiter found: save this substring */
			if (p - s) {

				strncpyt(substr, &name[s], p - s);
				printf("check if directory %s exists...", substr);
				for (dir = *current; dir; dir = dir->next) {
					if (!(strcmp(dir->name, substr)) && dir->flags == IFS_DIR) {
						break;
					}
				}
				if (!dir) {
					printf("not found, creating dir \"%s\"\n", substr);
					dir = calloc(1, sizeof(ifs_entry));
					dir->flags = IFS_DIR;
					dir->name = strdup(substr);
					dir->next = *current;
					*current = dir;
				} else {
					printf("found directory, chdir to \"%s\"\n", substr);
				}
				s = p + 1;
				current = &dir->subdirs;
				if (name[p] == 0)
					break;
			} else
				s++;
		}
	}
}

/* add a file to the tree */
void add_file(char *name)
{
	ifs_entry *file;
	char buf[1024], *fname, *p;
	struct stat finfo;
	int i;

	if (searchpath) {
		strcpy(buf, searchpath);
		i = strlen(buf);
		if (buf[i - 1] != '/') {
			buf[i] = '/';
			buf[i + 1] = 0;
		}
	} else {
		buf[0] = 0;
	}
	strcat(buf, name);

	/* first let's check if the file exists on the build system */
	if (stat(buf, &finfo)) {
		die("There was an error: %s\n", strerror(errno));
	}

	p = fname = name;
	while (*p) {		/* strip build path */
		if (*p == '/') {
			fname = p + 1;
		}
		p++;
	}

	for (file = *current; file; file = file->next) {
		if (!strcmp(file->name, fname) && file->flags == IFS_FILE) {
			break;
		}
	}
	if (!file) {
		printf("creating file \"%s\"\n", fname);
		file = calloc(1, sizeof(ifs_entry));
		file->flags = IFS_FILE;
		if (!strcmp(fname, bootstrap)) {
			file->flags |= IFS_BOOTSTRAP;
			file->offset = base;
			bootstrap_entry = file;
		}
		file->name = strdup(fname);
		file->realname = strdup(buf);
		file->size = finfo.st_size;
		file->next = *current;
		*current = file;
	} else {
		die("error: file \"%s\" already exists\n", fname);
	}
}

void build_directory(section *image)
{
	command *c;
	arg *a;
	char buf[1024];

	fprintf(stderr, "Building image\n");

	current = &root;

	for(c = image->firstc; c; c = c->next) {
		if (c->nlen == 3 && !strncmp("dir", c->name, 3)) {
			if (!(a = get_arg(c))) {
				die("argument missing for command >dir\n", NULL);
			}
			strncpy(buf, a->name, a->nlen);
			buf[a->nlen] = 0;

			add_directory(buf);

			for(a = get_nextarg(a); a; a = get_nextarg(a)) {

				strncpy(buf, a->name, a->nlen);
				buf[a->nlen] = 0;

				add_file(buf);

			}
		} else {
			strncpy(buf, c->name, c->nlen);
			buf[c->nlen] = 0;
			fprintf(stderr, "unknown command %s, ignoring\n", buf);
		}
	}
}

/* writes out the directory structure in entry *r, returns the offset to the dir entry
   r, and the total size of bytes written in *offset */
int write_out(ifs_entry *r, int outf, int *offset)
{
	ifs_entry *entry;
	ifs_inode *inode;
	char *data, *compressed;
	int size, len, count, diroffs = 0;
	iovec iov[64]; /* FIXME: allow more than 64 entries per directory. */

	printf("reading dir\n");
	for (entry = r, count = 0; entry; entry = entry->next, count++) {

		switch (entry->flags) {
			case IFS_FILE:
				printf("writing file %s\n", entry->name);
				entry->offset = *offset + base;
				size = entry->size;
				if (!(data = loadfile(entry->realname, &size))) {
					die("cannot load file\n", NULL);
				}
				compressed = malloc(size * 2);
				size = Encode(zip_mem, compressed, size * 2, data, size);
				entry->flags |= IFS_COMPRESSED;
				if ((size = write(outf, compressed, size)) == -1) {
					die("error: could not write file: %s\n", strerror(errno));
				}
				free(compressed);
				free(data);
				*offset += size;
				break;

			case IFS_DIR:
				printf("change to directory %s, %d\n", entry->name, *offset);
				//diroffs = *offset;
				entry->offset = write_out(entry->subdirs, outf, offset) + base;
				printf("%x\n", entry->offset);
				if (entry->offset > base)
					entry->size = *offset - entry->offset + base;//diroffs;
				else
					entry->size = 0;
				break;
		}
		printf("adding entry %s, %x, %d\n", entry->name, entry->offset, entry->size);
		len = strlen(entry->name);
		size = len + sizeof(ifs_inode);
		inode = malloc(size);
		inode->data.offset = entry->offset;
		inode->size = entry->size;
		inode->flags = entry->flags;
		inode->namelen = len;
		strncpy(inode->name, entry->name, len);
		iov[count].iov_base = inode;
		iov[count].iov_len = size;

	}
	if (count) {
		diroffs = *offset;
		if ((size = writev(outf, iov, count)) == -1) {
			die("error: could not write file: %s\n", strerror(errno));
		}
		*offset += size;
	}
	printf("end of dir, directory offset = %x\n", diroffs);
	return diroffs;
}

void write_image(char *outfile)
{
	int outf, size, bytes, i;
	char *data;
	ifs_entry *entry;
	ifs_superblock *superblock;

	if ((outf = creat(outfile, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
		die("cannot write to \"%s\"\n", outfile);
	}

	zip_mem = malloc(sizeof(lzss_encodestream));

	size = bootstrap_entry->size;
	if (!(data = loadfile(bootstrap_entry->realname, &size))) {
		die("cannot load bootstrap file\n", NULL);
	}

	if ((bytes = write(outf, data, size)) == -1) {
		fprintf(stderr, "warning: could not write entire file\n");
	}

	for (i = 0; i < 8192; i++) {
		superblock = (ifs_superblock *)&data[i];
		if (superblock->mb_header.magic == MULTIBOOT_MAGIC &&
				superblock->magic == IFS_MAGIC) {
			printf("superblock found at offset %x hex\n", i);
			break;
		}
	} 

	superblock->root.offset = write_out(root, outf, &bytes) + base;
	superblock->size = bytes;

	printf("root dir at: %x, size of image = %d\n", superblock->root.offset, superblock->size);

	lseek(outf, i, SEEK_SET);
	write(outf, superblock, sizeof(ifs_superblock));

	free(data);

	free(zip_mem);

	close(outf);
}

void mkifs(section *sections, char *outfile)
{
	int i;
	char file[1024];
	section *s;
	command *c;
	arg *a;

	if (!(s = find_section(sections, "Startup"))) {
		die("No Startup section found\n", NULL);
	}

	fprintf(stderr, "Processing section [Startup]\n");
	if (!(c = find_command(s, "boot"))) {
		die("Please specify a >boot command in the [Startup] section\n", NULL);
	}
	if (!(a = get_arg(c))) {
		die("argument missing for command >boot\n", NULL);
	}
	bootstrap = strndup(a->name, a->nlen);
	printf("Bootstrap is \"%s\"\n", bootstrap);

	/* print_sections(sections); */

	if (!(s = find_section(sections, "Image"))) {
		die("No Image section found\n", NULL);
	}

	build_directory(s);

	if (!bootstrap_entry) {
		die("error: bootstrap missing\n", NULL);
	}
	fprintf(stderr, "writing image\n");

	write_image(outfile);

	fprintf(stderr, "done\n");
}

void usage(char *progname)
{
	fprintf(stderr,"usage: %s [ <inifile> ... -r <searchpath> -b <base> ] -o <bootfile>\n", progname);
}

int main(int argc, char **argv)
{
	char *file = NULL;
	char *progname = argv[0];
	section *sections;

	if (argc < 2) {
		usage(progname);
        return 1;
    }

	argc--;
	argv++;
	
	while (argc) {
		if (!strcmp(*argv, "-o")) {
			argc--;
			argv++;
			if (argc) {
				if (*argv[0] != '-') {
					file = *argv;
				} else {
					fprintf(stderr, "error: no output specified.\n");
					usage(progname);
					return 1;
				}
			} else {
				usage(progname);
				return 1;
			}
		} else if (!strcmp(*argv, "-r")) {
			argc--;
			argv++;
			if (argc) {
				if (*argv[0] != '-') {
					searchpath = *argv;
				} else {
					fprintf(stderr, "error: option -r requires a path\n");
					usage(progname);
					return 1;
				}
			} else {
				fprintf(stderr, "error: option -r requires a path\n");
				usage(progname);
				return 1;
			}
		} else if (!strcmp(*argv, "-b")) {
			argc--;
			argv++;
			if (argc) {
				if (*argv[0] != '-') {
					char *end;
					base = strtoul(*argv, &end, 0);
					if (end < (*argv + strlen(*argv))) {
						fprintf(stderr, "error: please specify a valid 'base'\n");
						usage(progname);
						return 1;
					}
				} else {
					fprintf(stderr, "error: option -b requires a base adress\n");
					usage(progname);
					return 1;
				}
			} else {
				fprintf(stderr, "error: option -b requires a base adress\n");
				usage(progname);
				return 1;
			}
		} else {
			if ((sections = load_ini(*argv)) == NULL){
				fprintf(stderr,"warning: cannot load '%s'\n",*argv);
			}
		}
		argc--;
		argv++;
	}


	if (!file) {
		fprintf(stderr, "error: no output specified\n");
		usage(progname);
		return 1;
	}
	
	if (!sections) {
		fprintf(stderr, "error: no data to write?!\n");
		usage(progname);
		return 1;
	}
	
	mkifs(sections, file);
    
    return 0;    
}
