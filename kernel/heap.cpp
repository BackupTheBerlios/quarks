/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#include "heap.h"

/*
 * Initialize the heap for use by the memory allocator. Pass a pointer
 * to the space to use as a heap, and the size in bytes.
 */
Heap::Heap(void *space, size_t size)
{
	size = size / sizeof(HeapCell);
	root = (HeapCell *)space;
	root[0].next = root + 2;
	root[0].free = 2;
	root[0].size = 2;
	root[1].next = root + 2;

	HeapCell *cell = root + 2;
	cell[0].next = root;
	cell[0].size = size - 2;
	cell[0].free = 1;
	cell[1].next = root;
}

/*
 * Search for the first cell that fits. Split the cell if that makes
 * sense, and return a pointer to the memory part. Internally, these
 * routines manipulate memory in units of HeapCell size. These are
 * 64bit aligned, for your dereferencing pleasure.
 *
 * NOTE: In general purpose allocators, there is no evidence that
 * first fit is better or worse then best/worst fit so it is not worth
 * the effort of implementing the latter. However, keeping memory
 * sorted and merged is a big win for fast garbage collection.
 */
void *Heap::Alloc(size_t size)
{
	// Normalize the requested size.

	uint32 hsize = (size + 2 * sizeof(HeapCell) - 1) / sizeof(HeapCell);

	// First fit search.
	HeapCell *cur = root->next;
	while ((cur->size < hsize) && (cur != root))
		cur = cur->next;

	if (cur == root)
		return 0;

	// Trim excess off the located cell.
	if (cur->size > (2 + hsize)) {
		HeapCell *split = cur + hsize;
		split[0].next = cur->next;
		split[0].next[1].next = split;
		split[0].size = cur->size - hsize;
		split[0].free = 1;
		split[1].next = cur;

		cur->next = split;
		cur->size = hsize;
	}

	// There MUST be a free cell after me. This acts like a
	// trailer for other parts of the allocator.
	if (cur->next == root)
		return 0;

	// Pull this cell out of the free list.

	cur[1].next->next = cur->next;
	cur->next[1].next = cur[1].next;
	cur->free = 0;

	// Point to the cell after me. It is possible for a cell
	// between me and my current free cell neighbor to be
	// released, so when I'm released I will need to search
	// starting right after me for a next cell.
	cur->next = cur + cur->size;

	return cur + 1;
}

/*
 * This routine assumes that there is a free cell before the end.
 */
void Heap::Free(void *memory)
{
	HeapCell *cell = ((HeapCell *)memory) - 1;

	HeapCell *cur = cell->next;

	// assert(cell->free == 0);
	cell->free  = 1;

	// Link to the free cell after this. If the immediate
	// following cell is free, then merge immediately. Otherwise,
	// search for the next free cell and do not merge. Also, do
	// not merge with the root cell.
	if (cur->free == 1) {
		cell->size += cur->size;
		cell->next  = cur->next;
		cell[1].next= cur[1].next;
	} else {
		while (! cur->free)
			cur = cur->next;
		// assert(cur->free == 1);
		cell->next = cur;
		cell[1].next = cur[1].next;
	}

	cell[1].next->next = cell;
	cell->next[1].next = cell;

	// Attempt to join with the previous cell.
	cur = cell[1].next;
	if ((cur->free == 1) && (cur + cur->size == cell)) {
		cur->size += cell->size;
		cur->next = cell->next;
		cur->next[1].next = cur;
	}
}
