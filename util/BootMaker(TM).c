/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "boot.h"
#include "bootblock.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int make_floppy = 0;

void die(char *s, char *a)
{
    fprintf(stderr,"error: ");
    fprintf(stderr,s,a);
    fprintf(stderr,"\n");
    exit(1);
}

void *loadfile(char *file, int *size)
{
    int fd;
    char *data;
    struct stat info;
    
    if((fd = open(file,O_RDONLY)) != -1){
        if(fstat(fd,&info)){
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

/* Builds a directory structure out of an [Image] section */
dir *build_directory(section *is)
{
	dir *image;

	c = find_command(is, "dir");
	
}	

void makeboot(section *s, char *outfile)
{
	FILE *fp;
	int i, c = 0;
	char *file, *tcb;
	section *bootstrap;
	int bootstrapsize;
	char *bootstrapdata;
	int rawsize[64];
	char *rawdata[64];
	int blocks;

	memset(rawsize, 0, sizeof(rawsize));
	memset(rawdata, 0, sizeof(rawdata));

	if (!(bootstrap = find_section(s, "Bootstrap"))) {
		die("No bootstrap section found\n", NULL);
	}

	fprintf(stderr, "Processing section [Bootstrap]\n");
/*	if (!(file = getval(bootstrap, "boot"))) {
		die("bootstrap file not specified\n", NULL);
	}

	if (!(bootstrapdata = loadfile(file, &bootstrapsize))) {
		die("cannot load \"%s\"", file);
	}*/

	print_sections(sections);


/*	if(!(fp = fopen(outfile,"wb"))){
		die("cannot write to \"%s\"", outfile);
	}

	if(make_floppy) {
		fprintf(stderr, "writing bootblock\n");*/

		/* at location 2 is a uint16, set to number of sectors to read */
/*		blocks = (bootstrapsize + 511) / 512;
		bootblock[2] = (blocks & 0x00FF);
		bootblock[3] = (blocks & 0xFF00) >> 8;

		fwrite(bootblock, sizeof(bootblock), 1, fp);
    }*/
    
	fprintf(stderr, "writing bootstrap\n");
/*	fwrite(bootstrapdata, bootstrapsize, 1, fp);*/

	fprintf(stderr, "writing directory\n");
/*	fwrite(bdir, sizeof(bdir), 1, fp);*/

	for (i = 0; i < c; i ++) {
		fprintf(stderr, "writing file %i\n");
		fwrite(rawdata[i], rawsize[i], 1, fp);
	}
	fprintf(stderr, "done\n");
/*	fclose(fp);*/
}

int main(int argc, char **argv)
{
	char *file = NULL;
    section *s;
    
    if(argc < 2){
usage:
        fprintf(stderr,"usage: %s [ --floppy | -f ] [ <inifile> ... ] -o <bootfile>\n",argv[0]);
        return 1;
    }

	argc--;
	argv++;
	
	while(argc){
		if(!strcmp(*argv,"--floppy")){
			make_floppy = 1;
		} else if(!strcmp(*argv,"-o")){
			argc--;
			argv++;
			if(argc){
				file = *argv;
			} else {
				goto usage;
			}
		} else {
			if(load_ini(*argv) == NULL){
				fprintf(stderr,"warning: cannot load '%s'\n",*argv);
			}
		}
		argc--;
		argv++;
	}
	
	
    if((argc > 3) && !strcmp(argv[3],"-floppy")){
        make_floppy = 1;
    }

	if(!file){
		fprintf(stderr,"error: no output specified\n");
		goto usage;
	}
	
	if(!sections){
		fprintf(stderr,"error: no data to write?!\n");
		goto usage;
	}
	
	makeboot(sections,file);
    
    return 0;    
}
