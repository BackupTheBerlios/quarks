/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "console.h"
#include <quarks/intel/io.h>

void va_snprintf(char *b, int l, const char *fmt, va_list pvar);
/*int va_snprintf (char *str, int l, const char *format, ...);*/

static int attr = 1, px = 0, py = 24;
static unsigned char *screen = (unsigned char *) CON_SCREEN;
static unsigned char *pos   = 0;

#define LastRow	((ScreenColumns - 1) * ScreenRows * 2)

void ScrollUp()
{
	int i;
	memcpy(screen, screen + ScreenRows * 2, LastRow);
	for (i = LastRow; i < ScreenColumns * ScreenRows * 2; i++) {
		*screen++ = ' ';
		*screen   = attr;
	}
}

void SetAttribute(int a)
{
	attr = a;
}

void InitConsole(unsigned char *videostart)
{
	screen = videostart;
	attr = cWhite;
	ClearScreen();
}

void ClearScreen() 
{
    int i;
    
    for (pos = screen, i = 0;i < 80 * 25; i++) {
        *pos++ = ' ';
        *pos++ = attr;
    }
    px = 0;
    py = 24;
    pos = screen + LastRow;
}

void GoTo(int x, int y)
{
	pos = screen + (y * ScreenRows + x) * 2;
    px = x;
    py = y;

	// int cursor = (y * ScreenRows + x) * 2;
   /*outb(0x3D4, 14);
   outb(0x3D5, (unsigned char)(cursor >> 9));
   outb(0x3D4, 15);
   outb(0x3D5, (unsigned char)(cursor >> 1));*/
}

void PutString(char *s)
{
	while (*s) {
		if (*s == '\n') {
			s++;
			goto scroll;
		}
		*pos++ = *s++;
		*pos++ = attr;
		px++;
		if (px == 80) {
		scroll:
			px = 0;
			if (py == 24)
				ScrollUp();
			else
				py++;
			pos = screen + (py * 80 + px) * 2;
        }
    }
}

void Log(char *fmt, ...)
{
	char buf[256];
	va_list pvar;
	va_start(pvar,fmt);
	va_snprintf(buf, 256, fmt, pvar);
	va_end(pver);
	PutString(buf);
}

