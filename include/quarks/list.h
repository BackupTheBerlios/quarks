/*
	Quarks microkernel

	Copyright (C) 2001 Peter Kleinstueck <pkx544@myrealbox.com>

	This is free software; you can redistribute it and/or modify
	it without license.

*/
#ifndef QUARKS_LIST_H
#define QUARKS_LIST_H

namespace Quarks {

template<class T>
class List {
	public:
		List()
		 : head(NULL), tail(NULL), current_pos(NULL), size(0);

		void Insert(T *elem)
			{ current = new Node(PrevNode(), NextNode(), elem); size++; }
		void Remove(T *elem)
			{ delete element; }
		void Append(T *elem)
			{ tail = new Node(tail, NULL, elem); size++; }
		void Prepend(T *elem)
			{ head = new Node(NULL, _head, elem); size++; }
		inline T *Head()
			{ return head->Element(); }
		inline T *Tail()
			{ return tail->Element(); }
		T *Next()
			{ current = NextNode(); return current->Element(); }
		T *Prev()
			{ current = PrevNode(); return current->Element(); }

	private:

		class Node {
			public:
				Node(Node *p, Node *n, T *e)
				 : prev(p), next(n), elem(e);

				inline Node *Next() { return next; }
				inline Node *Prev() { return prev; }
				inline T *Element() { return elem; }

			private:
				T *elem;
				Node *next;
				Node *prev;
		};

		/* Some helper methods */
		inline Node *PrevNode()
			{ return (current ? current->prev() : NULL); }
		inline Node *NextNode()
			{ return (current ? current->next() : NULL); }

		/* Boundaries of the list */
		Node *head;
		Node *tail;
		Node *current;
		int size;
};

};

#endif /* QUARKS_LIST_H */
