/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_THREAD_H
#define QUARKS_THREAD_H

#include "resource.h"

class Clan;
class Queue;
class Team;

class Thread : public Resource {
	public:
		Thread(const char *name);

		// This method removes the thread from any ThreadQueue that it
		// might be in. It is needed for example when the thread is
		// ready to be destroyed.
		Thread *Enqueue(Queue *where);
		Thread *Dequeue();

		inline Thread *Next() const	{ return next; }
		inline Thread *Prev() const	{ return prev; }

		//inline Queue *Owner() const	{ return owner; }
		//void SetOwner(Queue *queue);

	private:
		Thread	*prev, *next;

		/* To which ThreadQueue, Team and Clan we belong... */
		Queue	*owner;
		Clan	*clan;
		Team	*team;

		// The created threads all go into a list. The debugger
		// support uses this list for finding threads of note.
		Thread *list;
};

#endif /* QUARKS_THREAD_H */
