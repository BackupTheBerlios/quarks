/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_THREADS_H
#define QUARKS_THREADS_H

namespace Quarks {

class Thread {
	public:
		Thread(const char *name);
		~Thread();

	private:
		thread_t thread;
};

};

#endif /* QUARKS_THREADS_H */
