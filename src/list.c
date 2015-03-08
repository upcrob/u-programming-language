/*
 * Generic list functions.
 */

// Includes
#include <malloc.h>
#include "list.h"
#ifndef NULL
	#define NULL 0
#endif

/* Create a new list. */
List* newList()
{
	List* l = (List*) malloc(sizeof(List));
	l->cap = 10;
	l->size = 0;
	l->arr = (void**) malloc(sizeof(void*) * 10);
	return l;
}

/* Free list. */
void freeList(List* l)
{
	free(l->arr);
	free(l);
}

/* Add an element to the list. */
void listAdd(List* l, void* e)
{
	if (l->size == l->cap)
	{
		// Expand stack
		l->cap *= 2;
		void** newArr = (void**) malloc(sizeof(void*) * l->cap);
		int i;
		for (i = 0; i < l->size; i++)
			newArr[i] = l->arr[i];
		free(l->arr);
		l->arr = newArr;
	}
	
	l->arr[l->size++] = e;
}

/* Get an element from the list. */
void* listGet(List* l, int index)
{
	if (index < 0 || index >= l->size)
		return NULL;
	return l->arr[index];
}

/* Remove an element from the list. */
void listRemove(List* l, int index)
{
	int i;
	l->size--;
	for (i = index; i < l->size; i++)
		l->arr[i] = l->arr[i + 1];
}


