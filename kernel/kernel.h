/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_KERNEL_H
#define QUARKS_KERNEL_H

#include "machine.h"

class Queue;
class Thread;

class Kernel : public Machine {
	public:
		__init_ Kernel();

	private:
		Thread *idle_thread;
		Thread *reaper_thread;
		Thread *pager_thread;

		Queue *run_queue;

		static const char *version;
};

#endif /* QUARKS_KERNEL_H */
