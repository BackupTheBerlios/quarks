/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/

int errno;

char *errstrings[] = {
	"no error",
	"file not found",
	"not a directory",
	"too many files open",
	"invalid file descriptor",
	"unknown error"
};
