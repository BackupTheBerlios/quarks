/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_CONDVAR_H
#define QUARKS_CONDVAR_H

#include "queue.h"
#include "resource.h"

class Mutex;

class CondVar : public Resource {
	public:
		explicit CondVar(const char *name, Mutex *mutex);

		void Wait();
		void TimedWait();
		void Signal();
		void Broadcast();

	private:
		Queue	*waitqueue;
		Mutex	*master;

		/* Disable copy constructors. */
		CondVar(const CondVar &);
		CondVar &operator=(const CondVar &);
};

#endif /* QUARKS_CONDVAR_H */
