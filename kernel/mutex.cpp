/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "mutex.h"

Mutex::Mutex(const char *name)
 : Resource(name),
   busy_flag(0)
{
}

void Mutex::Lock()
{
	/* spl_cookie_t cookie = thread_splhi();

	while (busy_flag) {
		stuck.Enqueue(thread_self());
		thread_reschedule();
	}

	busy_flag = 1;

	thread_spl(cookie); */
}

void Mutex::Unlock()
{
	/* spl_cookie_t cookie = thread_splhi();

	busy_flag = 0;
	Thread *next = stuck.Peek();
	if (next)
		thread_switch(next);
	else
		thread_reschedule();

	thread_spl(cookie); */
}
