/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include <string.h>

char *strcpy(char *dest, const char *src)
{
	char *tmp = dest;   
	while ((*dest++ = *src++) != '\0');
	return tmp;
}

char *strncpy(char *dest, const char *src, size_t count)
{
	char *tmp = dest;
	while (count-- && (*dest++ = *src++) != '\0');
	return tmp;
}

char *strcat(char *dest, const char *src)
{
	char *tmp = dest;
	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0');
	return tmp;
}

char *strncat(char *dest, const char *src, size_t count)
{
	char *tmp = dest;
   
	if (count) {
		while (*dest)
			dest++;
		while ((*dest++ = *src++)) {
			if (--count == 0) {
				*dest = '\0';
				break;
			}
		}
	}
	return tmp;
}

int strcmp(const char *s1, const char *s2)
{
	register char r;
	while (1)
		if ((r = *s1 - *s2++) != 0 || !*s1++)
			break;
	return r;
}

int strncmp(const char *s1, const char *s2, size_t count)
{
	register char r = 0;
   
	while (count) {
		if ((r = *s1 - *s2++) != 0 || !*s1++)
			break;
		count--;
	}
	return r;
}

size_t strlen(const char *s)
{
	const char *s1;
	for (s1 = s; *s1 != '\0'; ++s1);
	return (s1 - s);
}

size_t strnlen(const char *s, size_t count)
{
   const char *s1;
   for (s1 = s; *s1 != '\0' && count--; ++s1);
   return (s1 - s);
}

void *memcpy(void *dest, const void *src, size_t count)
{
	char *d = (char *)dest, *s = (char *)src;   
	while (count--)
		*d++ = *s++;
	return dest;
}

char *strchr(const char *s, int c)
{
	for (; *s != (char)c; ++s)
		if (*s == '\0')
			return (NULL);
	return (char *)s;
}

char *strrchr(const char *s, int c)
{
   const char *p = s + strlen (s);
   do {
      if (*p == (char)c)
         return (char *)p;
   } while (--p >= s);
   return (NULL);
}

void *memset(void *s, int c, size_t count)
{
	char *xs = (char *)s;
   
	while (count--)
		*xs++ = c;
      
	return s;
}
