/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#include <stdarg.h>
#include <quarks/x86/io.h>
#include <x86/console.h>

void va_snprintf(char *b, int l, const char *fmt, va_list pvar);

static int attr = 1, px = 0, py = ScreenRows - 1;
static unsigned char *screen = CON_SCREEN;
static unsigned char *posn   = CON_SCREEN + ScreenColumns * ScreenRows * 2;

#define LastRow	((ScreenRows - 1) * ScreenColumns * 2)

void ScrollUp(void) 
{
	for (posn = screen + 160; posn <= screen + 80*25*2; posn++)
		*(posn - 160) = *posn;
	for (posn = screen + 80*24*2; posn <= screen + 80*25*2; posn++){
		*posn++ = ' ';
		*posn   = attr;
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
    
    for (posn = screen, i = 0;i < 80 * 25; i++) {
        *posn++ = ' ';
        *posn++ = attr;
    }
    px = 0;
    py = 24;
    posn = screen + LastRow;
}

void GoTo(int x, int y)
{
	int cursor;
	posn = screen + (y * ScreenRows + x) * 2;
    px = x;
    py = y;

	cursor = (y * ScreenRows + x) * 2;
	outb(0x3D4, 14);
	outb(0x3D5, (unsigned char)(cursor >> 9));
	outb(0x3D4, 15);
	outb(0x3D5, (unsigned char)(cursor >> 1));
}

void PutString(char *s)
{
	while (*s) {
		if (*s == '\n') {
			s++;
			goto scroll;
		}
		*posn++ = *s++;
		*posn++ = attr;
		px++;
		if (px == 80) {
		scroll:
			px = 0;
			if (py == 24)
				ScrollUp();
			else
				py++;
			posn = screen + (py * 80 + px) * 2;
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

