/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_QUEUE_H
#define QUARKS_QUEUE_H

class Thread;

/*
 * ThreadQueue objects can hold threads. Threads can only be in one
 * thread queue, and cannot be in the run queue while in a thread
 * queue. If a thread is put into a thread queue, it will
 * automatically be removed from any other thread queue (including the
 * run queue).
 *
 * The above property makes this class perfect for implementing
 * synchronization and scheduling primitives. Remember that threads
 * will only exist in one queue at a time, always.
 *
 * NOTE: No special precautions have been taken in ThreadQueue to
 * protect the implementation from interrupts. That is up to the
 * application, for now.
 *
 * NOTE: Although putting a thread into a queue may take it out of the
 * run queue, it will not cause a thread switch. Use thread_reschedule() to
 * cause a reschedule, if necessary.
 *
 * NOTE: thread_switch, or any function that calls thread_switch, will
 * pull the thread out of any ThreadQueue it might be in.
 */

class Queue {
	public:
		Queue() : last(0) { }
		explicit Queue(Thread *thread) { Enqueue(thread); }
		~Queue() { }

		// Enqueue will put the thread on the end of the queue, or the
		// front if the flag is set. It will pull it from another
		// queue if necessary.
		void Enqueue(Thread *thread, bool front = false);

		// Pull removes the thread from the queue. Remove either the
		// explicit thread, or the first thread in the queue. Peek
		// will return the first thread without pulling it.
		Thread *Pull();
		Thread *Peek();

		inline Thread *Last() const	{ return last; }

	private:
		Thread *last;

		/* Disable copy constructors. */
		Queue(const Queue &);
		Queue &operator=(const Queue &);
};

#endif /* QUARKS_QUEUE_H */
