/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef PORTS_H
#define PORTS_H

namespace Quarks {

class Message;

class Port {
	public:
		Port(const char *name);
		~Port();

		Send(Port *dest, Message *msg);
		Receive(Port *source, Message *msg);
		
	private:
		port_t port;
};

};

#endif /* PORTS_H */
