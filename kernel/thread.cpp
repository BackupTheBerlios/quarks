/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "queue.h"
#include "thread.h"

Thread::Thread(const char *name)
 : Resource(name),
   prev(this), next(this),
   owner(0), clan(0), team(0)
{
}

Thread *Thread::Dequeue()
{
	if (owner) {
		if (owner->Last() == this && prev == this)
			return 0;
		next->prev = prev;
		prev->next = next;
		owner = 0;
	}
	return prev;
}

Thread *Thread::Enqueue(Queue *where)
{
	Thread *last = Dequeue();
	owner = where;

	if (last) {
		next = last->next;
		prev = last;
		next->prev = this;
		prev->next = this;
	} else {
		last = this;
		next = this;
		prev = this;
		//return last;
	}
	return last;
}
