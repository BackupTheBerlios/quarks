/*
	Zzzzz Toolkit

	Copyright (C) 2001 Peter Kleinstueck <pkx544@gmx.net>

	This library is free software; you can redistribute it and/or modify
	it under the terms of some license.

*/
#ifndef COMPRESS_H
#define COMPRESS_H

#define N 4096			/* size of ring buffer */
#define F 18			/* upper limit for match_length */
#define THRESHOLD 2		/* encode string into position and length */
						/* if match_length is greater than this */

#define NIL N			/* index for root of binary search trees */

typedef struct lzss_workmem {
	unsigned long int textsize;   /* text size counter */
	unsigned char text_buf[N + F - 1];
		/* ring buffer of size N, with extra F-1 bytes to facilitate string comparison */
	int match_position, match_length;
		/* of longest match.  These are set by the InsertNode() procedure. */
	int lson[N + 1], rson[N + 257], dad[N + 1];
		/* left & right children & parents, These constitute binary search trees. */
} lzss_workmem;

int Encode(lzss_workmem *m, char *dest, int destlen, const char *src, int srclen);
int Decode(lzss_workmem *m, char *dest, int destlen, const char *src, int srclen);

#endif /* COMPRESS_H */
