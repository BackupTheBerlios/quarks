/*
   Quarks microkernel

   Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

   This is free software; you can redistribute it and/or modify
   it without license.

*/
#include "condvar.h"
#include "mutex.h"

CondVar::CondVar(const char *name, Mutex *mutex)
 : Resource(name),
   master(mutex)
{
}

/*
 * Wait for another thread to signal me. I do this by putting myself
 * in the <waiting> queue and leaving(). The leave() will perform
 * rescheduling as needed, giving preference to threads trying to
 * enter the mutex.
 */
void CondVar::Wait()
{
	/* waiting.Enqueue(thread_self());
	master->Leave();
	master->Enter(); */
}

void CondVar::TimedWait()
{
}

/*
 * If there is a thread waiting, change it from <waiting> to
 * <stuck>. Then, when I leave it will be scheduled. Promptly reenter
 * the mutex before continuing myself.
 */
void CondVar::Signal()
{
	/* Thread *next = waiting.Peek();
	if (next) {
		master->stuck.Enqueue(next, true);
		master->Leave();
		master->Enter();
	} */
}

void CondVar::Broadcast()
{
	/* Thread *next;

	while ((next = waiting.Pull()))
		master->stuck.Enqueue(next, true);

	master->Leave();
	master->Enter(); */
}
