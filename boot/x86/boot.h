/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#ifndef BOOT_H
#define BOOT_H

#define BOOTDIR_NAMELEN         32
#define BOOTDIR_MAX_ENTRIES     64
#define BOOTDIR_DIRECTORY       "SBBB/Directory"

typedef struct {
    char name[BOOTDIR_NAMELEN]; /* name of loaded object, zero terminated */
    int  offset;   /* offset of object relative to the start of boot_dir  */
    int  type;     /* object type designator                              */
    int  size;     /* size of loaded object (pages)                       */
    int  vsize;    /* size loaded object should occupy when mapped in     */
    int  extra0;
    void (*entry)(void);
    int  extra2;
    int  extra3;
} boot_entry;

typedef struct {
    boot_entry bd_entry[BOOTDIR_MAX_ENTRIES];
} boot_dir;

/* void _start(uint32 mem, char *params, boot_dir *bd); */

#define BE_TYPE_NONE         0  /* empty entry                              */
#define BE_TYPE_DIRECTORY    1  /* directory (entry 0)                      */
#define BE_TYPE_BOOTSTRAP    2  /* bootstrap code object (entry 1)          */
#define BE_TYPE_CODE         3  /* executable code object                   */
#define BE_TYPE_DATA         4  /* raw data object                          */
#define BE_TYPE_ELF32        5  /* 32bit ELF object                         */

/* for BE_TYPE_CODE */
#define be_code_vaddr be_extra0 /* virtual address (rel offset 0)           */
//#define be_code_ventr be_extra1 /* virtual entry point (rel offset 0)       */

#endif /* BOOT_H */
