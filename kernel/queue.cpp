/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "queue.h"
#include "thread.h"

/*
 * The resourcequeue is a double circularly linked list. The <last>
 * pointer points to the last cell in the queue, or is 0 if there are
 * no things in the queue. The first thing in the queue is the cell
 * after the last, otherwise known as last->next.
 *
 * Also, a cell can only be in one queue at a time. If it is in
 * another when enqueue is called, it will be first pulled from the
 * other list. If it is already in the current list, it will be moved
 * to the back (or front if the flag is set).
 */

void Queue::Enqueue(Thread *thread, bool front)
{
	last = thread->Enqueue(this);

	// The insertion above put the cell after the cell that last
	// points to. That would make it the front. If I want to put
	// it in back, then move the last pointer.
	if (!front)
		last = thread;
}

Thread *Queue::Pull()
{
	if (!last)
		return 0;
	Thread *thread = last->Next();
	last = thread->Dequeue();
	return thread;
}

Thread *Queue::Peek()
{
	if (!last)
		return 0;
	return last->Next();
}
