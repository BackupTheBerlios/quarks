/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_X86_IO_H
#define QUARKS_X86_IO_H

#include "quarks/types.h"

#define IODELAY asm volatile("jmp 1f \n 1: jmp 1f \n 1:")

#define outb_p(a,b)  { outb(a,b) ; IODELAY; }
#define inb_p(a,b)   { inb(a,b)  ; IODELAY; }
#define outw_p(a,b)  { outw(a,b) ; IODELAY; }
#define inw_p(a,b)   { inw(a,b)  ; IODELAY; }

#define	__use_immediate_port(port) \
	(__builtin_constant_p((port)) && (port) < 0x100)

#define	inb(port) \
	(__use_immediate_port(port) ? inbc(port) : inbx(port))

static inline uint8 inbc(int port)
{
	uint8 data;
	asm volatile("inb %1,%0" : "=a" (data) : "id" (port));
	return data;
}

static inline uint8 inbx(int port)
{
	uint8 data;
	asm volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void insb(int port, void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\tinsb"			:
								:
			 "d" (port), "D" (addr), "c" (cnt)	:
			 "%edi", "%ecx", "memory");
}

#define	inw(port) \
	(__use_immediate_port(port) ? inwc(port) : inwx(port))

static inline uint16 inwc(int port)
{
	uint16 data;
	asm volatile("inw %1,%0" : "=a" (data) : "id" (port));
	return data;
}

static inline uint16 inwx(int port)
{
	uint16 data;
	asm volatile("inw %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void insw(int port, void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\tinsw"			:
								:
			 "d" (port), "D" (addr), "c" (cnt)	:
			 "%edi", "%ecx", "memory");
}

#define	inl(port) \
	(__use_immediate_port(port) ? inlc(port) : inlx(port))

static inline uint32 inlc(int port)
{
	uint32 data;
	asm volatile("inl %1,%0" : "=a" (data) : "id" (port));
	return data;
}

static inline uint32 inlx(int port)
{
	uint32 data;
	asm volatile("inl %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void insl(int port, void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\tinsl"			:
								:
			 "d" (port), "D" (addr), "c" (cnt)	:
			 "%edi", "%ecx", "memory");
}

#define	outb(port, data) \
	(__use_immediate_port(port) ? outbc(port, data) : outbx(port, data))

static inline void outbc(int port, uint8 data)
{
	asm volatile("outb %0,%1" : : "a" (data), "id" (port));
}

static inline void outbx(int port, uint8 data)
{
	asm volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

static inline void outsb(int port, const void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\toutsb"		:
								:
			 "d" (port), "S" (addr), "c" (cnt)	:
			 "%esi", "%ecx");
}

#define	outw(port, data) \
	(__use_immediate_port(port) ? outwc(port, data) : outwx(port, data))

static inline void outwc(int port, uint16 data)
{
	asm volatile("outw %0,%1" : : "a" (data), "id" (port));
}

static inline void outwx(int port, uint16 data)
{
	asm volatile("outw %0,%w1" : : "a" (data), "d" (port));
}

static inline void outsw(int port, const void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\toutsw"		:
								:
			 "d" (port), "S" (addr), "c" (cnt)	:
			 "%esi", "%ecx");
}

#define	outl(port, data) \
	(__use_immediate_port(port) ? outlc(port, data) : outlx(port, data))

static inline void outlc(int port, uint32 data)
{
	__asm __volatile("outl %0,%1" : : "a" (data), "id" (port));
}

static inline void outlx(int port, uint32 data)
{
	asm volatile("outl %0,%w1" : : "a" (data), "d" (port));
}

static inline void outsl(int port, const void *addr, int cnt)
{
	asm volatile("cld\n\trepne\n\toutsl"		:
								:
			 "d" (port), "S" (addr), "c" (cnt)	:
			 "%esi", "%ecx");
}

#endif /* QUARKS_X86_IO_H */
