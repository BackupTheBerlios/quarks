/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef STRING_H
#define STRING_H

#include <quarks/types.h>

#ifdef __cplusplus
extern "C" {
#endif

char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t size);
int strncmp(const char *s1, const char *s2, size_t n);
int strcmp(const char *s1, const char *s2);
size_t strlen(const char *str);
char *strchr (const char *cs, int c);
size_t strlcpy (char *dst, const char *src, size_t size);
size_t strlcat (char *dst, const char *src, size_t size);
	
void *memset(void *s, int c, size_t n);
int memcmp(const void *dst, const void *src, size_t size);
void *memcpy(void *dst, const void *src, size_t size);
void *memmove(void *dest, const void *src, size_t n);
	
int bcmp (const void *s, const void *t, size_t len);
void bzero (void *b, size_t len);
	
char *_strerror (int errnum);
char *strerror (int errnum);

#ifdef __cplusplus
}
#endif

#endif /* STRING_H */
