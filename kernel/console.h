/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdarg.h>

#define CON_SCREEN	0x000B8000

enum {
	ScreenRows = 25,
	ScreenColumns = 80
};

enum ScreenColor {
	cBlack,
	cBlue,
	cGreen,
	cCyan,
	cRed,
	cMagenta,
	cYellow,
	cWhite
};

void SetAttribute(int a);
void ClearScreen();
void PutString(char *s);
void GoTo(int x, int y);

void InitConsole(unsigned int video);
	
void Log(char *fmt, ...);

#endif /* CONSOLE_H */
