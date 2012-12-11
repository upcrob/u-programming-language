/*
 * Generic linked list functions.
 */

/* List structure. */
typedef struct
{
	int cap;
	int size;
	void** arr;
} List;

/* Create a new list. */
List* newList();

/* Free a linked list. */
void freeList(List* l);

/* Add an element to the list. */
void listAdd(List* l, void* e);

/* Get an element from the list. */
void* listGet(List* l, int index);

/* Remove an element from the list. */
void listRemove(List* l, int index);

