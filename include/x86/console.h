/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#ifndef X86_CONSOLE_H
#define X86_CONSOLE_H

#define CON_SCREEN	(unsigned char *)0x000B8000

/*enum {
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
};*/

#define	ScreenRows 25
#define	ScreenColumns 80
#define cWhite	7

#ifdef __cplusplus
extern "C" {
#endif

void SetAttribute(int a);
void ClearScreen();
void PutString(char *s);
void GoTo(int x, int y);

void InitConsole(unsigned char *video);
void Log(char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* X86_CONSOLE_H */
