/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_HEAP_H
#define QUARKS_HEAP_H

#include <quarks/types.h>

class Heap {
	public:
		Heap(void *space, unsigned long size);
		~Heap();

		void *Alloc(unsigned long size);
		void Free(void *memory);

		void SelfTest();

	private:
		struct HeapCell {
			HeapCell *next;
			uint32 size : 24;
			uint32 free :  8;
		};

		HeapCell *root;
};

  // This is a new operator that can be used to allocate memory from a
  // specified heap. This is compatible with the delete operator.
inline void *operator new(size_t size, Heap *heap)
{
	return heap->Alloc(size);
}

inline void *operator new[](size_t size, Heap *heap)
{
	return heap->Alloc(size);
}

#endif /* QUARKS_HEAP_H */
