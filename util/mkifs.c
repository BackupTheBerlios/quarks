/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#include "boot.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

void mkifs(section *sections, char *outfile)
{
	FILE *fp;
	int i;
	char file[1024], bootstrap[1024];
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
		die("argument missing for command <boot\n", NULL);
	}
	strncpy(bootstrap, a->name, a->nlen);
	bootstrap[a->nlen] = 0;
	printf("Bootstrap is \"%s\"\n", bootstrap);

	/* print_sections(sections); */


/*	if(!(fp = fopen(outfile,"wb"))){
		die("cannot write to \"%s\"", outfile);
	}*/

	fprintf(stderr, "writing directory\n");
/*	fwrite(bdir, sizeof(bdir), 1, fp);

	for (i = 0; i < c; i ++) {
		fprintf(stderr, "writing file %i\n");
		fwrite(rawdata[i], rawsize[i], 1, fp);
	}*/
	fprintf(stderr, "done\n");
/*	fclose(fp);*/
}

void usage(char *progname)
{
	fprintf(stderr,"usage: %s [ <inifile> ... ] -o <bootfile>\n", progname);
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
		if (!strcmp(*argv,"-o")) {
			argc--;
			argv++;
			if (argc) {
				file = *argv;
			} else {
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
