/**************************************************************************
        LZSS.C -- A Data Compression Program

        4/6/1989 Haruhiko Okumura
        Use, distribute, and modify this program freely.
        Please send me your improved versions.
                PC-VAN          SCIENCE
                NIFTY-Serve     PAF01022
                CompuServe      74050,1022

The algorithm is quite simple:
Keep a ring buffer, which initially contains "space" characters
only.  Read several letters from the file to the buffer.  Then search
the buffer for the longest string that matches the letters just read,
and send its length and position in the buffer.

If the buffer size is 4096 bytes, the position can be encoded in 12
bits.  If we represent the match length in four bits, the <position,
length> pair is two bytes long.  If the longest match is no more than
two characters, then we send just one character without encoding, and
restart the process with the next letter.  We must send one extra bit
each time to tell the decoder whether we are sending a <position,
length> pair or an unencoded character.

The accompanying file LZSS.C is a version of this algorithm.  This
implementation uses multiple binary trees to speed up the search for the
longest match.  All the programs in this article are written in
draft-proposed ANSI C.  I tested them with Turbo C 2.0.

*************************************************************************/

#include <lzss.h>

void InitTree(lzss_encodestream *m)  /* initialize trees */
{
	int  i;

	/* For i = 0 to N - 1, rson[i] and lson[i] will be the right and
	   left children of node i.  These nodes need not be initialized.
	   Also, dad[i] is the parent of node i.  These are initialized to
	   NIL (= N), which stands for 'not used.'
	   For i = 0 to 255, rson[N + i + 1] is the root of the tree
	   for strings that begin with character i.  These are initialized
	   to NIL.  Note there are 256 trees. */

	for (i = N + 1; i <= N + 256; i++)
		m->rson[i] = NIL;

	for (i = 0; i < N; i++)
		m->dad[i] = NIL;
}

/* Inserts string of length F, text_buf[r..r+F-1], into one of the
   trees (text_buf[r]'th tree) and returns the longest-match position
   and length via the global variables match_position and match_length.
   If match_length = F, then removes the old node in favor of the new
   one, because the old one will be deleted sooner.
   Note r plays double role, as tree node and position in buffer. */
void InsertNode(lzss_encodestream *m, int r)
{
	int  i, p, cmp;
	unsigned char  *key;

	cmp = 1;
	key = &m->text_buf[r];
	p = N + 1 + key[0];
	m->rson[r] = m->lson[r] = NIL;
	m->match_length = 0;

	for ( ; ; ) {
		if (cmp >= 0) {
			if (m->rson[p] != NIL)
				p = m->rson[p];
			else {
				m->rson[p] = r;
				m->dad[r] = p;
				return;
			}
		} else {
			if (m->lson[p] != NIL)
				p = m->lson[p];
			else {
				m->lson[p] = r;
				m->dad[r] = p;
				return;
			}
		}

		for (i = 1; i < F; i++)
			if ((cmp = key[i] - m->text_buf[p + i]) != 0)
				break;

		if (i > m->match_length) {
			m->match_position = p;
			if ((m->match_length = i) >= F)
				break;
		}
	}

	m->dad[r] = m->dad[p];
	m->lson[r] = m->lson[p];
	m->rson[r] = m->rson[p];
	m->dad[m->lson[p]] = r;
	m->dad[m->rson[p]] = r;

	if (m->rson[m->dad[p]] == p)
		m->rson[m->dad[p]] = r;
	else
		m->lson[m->dad[p]] = r;

	m->dad[p] = NIL;  /* remove p */
}

/* deletes node p from tree */
void DeleteNode(lzss_encodestream *m, int p)
{
	int q;

	if (m->dad[p] == NIL)
		return;  /* not in tree */

	if (m->rson[p] == NIL)
		q = m->lson[p];
	else if (m->lson[p] == NIL)
		q = m->rson[p];
	else {
		q = m->lson[p];
		if (m->rson[q] != NIL) {
			do {
				q = m->rson[q];
			} while (m->rson[q] != NIL);

			m->rson[m->dad[q]] = m->lson[q];
			m->dad[m->lson[q]] = m->dad[q];
			m->lson[q] = m->lson[p];
			m->dad[m->lson[p]] = q;
		}
		m->rson[q] = m->rson[p];
		m->dad[m->rson[p]] = q;
	}

	m->dad[q] = m->dad[p];

	if (m->rson[m->dad[p]] == p)
		m->rson[m->dad[p]] = q;
	else
		m->lson[m->dad[p]] = q;

	m->dad[p] = NIL;
}

/* unpacks src buffer into dest, returns length of unpacked data. */
int Encode(lzss_encodestream *m, char *dest, int destlen, const char *src, int srclen)
{
	int  i, c, len, r, s, last_match_length, code_buf_ptr, incount = 0, outcount = sizeof(unsigned int);
	unsigned char code_buf[17], mask;

	InitTree(m);  /* initialize trees */

	code_buf[0] = 0;  /* code_buf[1..16] saves eight units of code, and
                code_buf[0] works as eight flags, "1" representing that the unit
                is an unencoded letter (1 byte), "0" a position-and-length pair
                (2 bytes). Thus, eight units require at most 16 bytes of code.*/

	code_buf_ptr = mask = 1;

	s = 0;
	r = N - F;

	for (i = s; i < r; i++)
		m->text_buf[i] = ' ';  /* Clear the buffer with any character that will appear often. */

	for (len = 0; len < F && incount < srclen; len++, incount++) {
		c = src[incount];
		m->text_buf[r + len] = c;  /* Read F bytes into the last F bytes of the buffer */
	}

	if ((m->textsize = len) == 0)
		return 0;  /* text of size zero */

	for (i = 1; i <= F; i++)
		InsertNode(m, r - i);  /* Insert the F strings,
                        each of which begins with one or more 'space' characters.  Note
                        the order in which these strings are inserted.  This way,
                        degenerate trees will be less likely to occur. */

	InsertNode(m, r);  /* Finally, insert the whole string just read.  The
                                        global variables match_length and match_position are set. */

	do {
		if (m->match_length > len)
			m->match_length = len;  /* match_length
                                                                may be spuriously long near the end of text. */

		if (m->match_length < THRESHOLD) {
			m->match_length = 1;  /* Not long enough match.  Send one byte. */
			code_buf[0] |= mask;  /* 'send one byte' flag */
			code_buf[code_buf_ptr++] = m->text_buf[r];  /* Send uncoded. */
		} else {
			code_buf[code_buf_ptr++] = (unsigned char)m->match_position;
			code_buf[code_buf_ptr++] = (unsigned char)(((m->match_position >> 4) & 0xf0) | (m->match_length - THRESHOLD));
			/* Send position and length pair. Note match_length > THRESHOLD. */
		}

		if ((mask <<= 1) == 0) {    /* Shift mask left one bit. */
			for (i = 0; i < code_buf_ptr; i++, outcount++) {  /* Send at most 8 units of */
				dest[outcount] = code_buf[i];     /* code together */
			}

			code_buf[0] = 0;
			code_buf_ptr = mask = 1;
		}

		last_match_length = m->match_length;
		for (i = 0; i < last_match_length && incount < srclen; i++, incount++) {
			c = src[incount];
			DeleteNode(m, s);          /* Delete old strings and */
			m->text_buf[s] = c;        /* read new bytes */
			if (s < F - 1)
				m->text_buf[s + N] = c;  /* If the position is
                                                        near the end of buffer, extend the buffer to make
                                                        string comparison easier. */

			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);
			/* Since this is a ring buffer, increment the position modulo N. */

			InsertNode(m, r);  /* Register the string in text_buf[r..r+F-1] */
		}

		while (i++ < last_match_length) {                                                                                       /* After the end of text, */
			DeleteNode(m, s);                                /* no need to read, but */

			s = (s + 1) & (N - 1);
			r = (r + 1) & (N - 1);

			if (--len)
				InsertNode(m, r);               /* buffer may not be empty. */
		}
	} while (len > 0);      /* until length of string to be processed is zero */

	if (code_buf_ptr > 1) {   /* Send remaining code. */
		for (i = 0; i < code_buf_ptr; i++, outcount++) {
			dest[outcount] = code_buf[i];
		}
	}
	*(unsigned int *)dest = outcount;
	return outcount;
}

void InitDecoder(lzss_decodestream *s, const char *source)
{
	s->j = 0;
	s->r = N - F;
	s->flags = 0;
	s->length = *(unsigned int *)source;
	s->source = &source[sizeof(unsigned int)];
	for (s->i = 0; s->i < N - F; s->i++)
		s->buffer[s->i] = ' ';
}

/* Just the reverse of Encode(). */
int Decode(lzss_decodestream *s, char *dest, int count)
{
	unsigned char c;
	int /*c,*/ y = 0;

	while (s->j && y < count) {
		c = s->buffer[s->i++];
		dest[y++] = c;
		s->buffer[s->r++] = c;
		s->r &= (N - 1);
		s->i &= (N - 1);
		s->j--;
	}

	for ( ; s->count < s->length && y < count; s->count++) {
		if (((s->flags >>= 1) & 256) == 0) {
			c = s->source[s->count++];
			if (s->count == s->length)
				break;
			s->flags = c | 0xff00;		/* uses higher byte cleverly */
		}							/* to count eight */
		if (s->flags & 1) {
			c = s->source[s->count];
			dest[y++] = c;
			s->buffer[s->r++] = c;
			s->r &= (N - 1);
		} else {
			s->i = s->source[s->count++] & 0xff;
			if (s->count == s->length)
				break;
			s->j = s->source[s->count];
			s->i |= ((s->j & 0xf0) << 4);
			s->i &= (N - 1);
			s->j = (s->j & 0x0f) + THRESHOLD;
			while (s->j && y < count) {
				c = s->buffer[s->i++];
				dest[y++] = c;
				s->buffer[s->r++] = c;
				s->r &= (N - 1);
				s->i &= (N - 1);
				s->j--;
			}
		}
	}
	return y;
}
