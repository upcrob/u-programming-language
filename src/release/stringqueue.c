/*
 * Functions for adding/removing strings from a queue.
 */
 #include "stringqueue.h"
 #include <malloc.h>
 #include <string.h>
 #include <assert.h>

// Enqueue a string
void EnqueueString(string_queue* q, char* str)
{
	// Exit if string already exists in queue
	int i;
	for (i = 0; i < q->size; i++)
		if (strcmp(q->strings[i], str) == 0)
			return;

	// Add to queue
	if (q->size == q->cap)
	{
		// Expand array
		q->cap *= 2;
		char** tmp = (char**) malloc(sizeof(char*) * q->cap);
		for (i = 0; i < q->size; i++)
			tmp[i] = q->strings[i];
		free(q->strings);
		q->strings = tmp;
	}
	q->strings[q->size++] = strdup(str);
}

// Dequeue a string
char* DequeueString(string_queue* q)
{
	if (q->size > 0)
	{
		char* str = q->strings[0];
		int i;
		for (i = 0; i < q->size - 1; i++)
			q->strings[i] = q->strings[i + 1];
		q->size--;
		return str;
	} else {
		return 0;
	}
}

// Create a new string queue
string_queue* CreateStringQueue()
{
	string_queue* q = (string_queue*) malloc(sizeof(string_queue));
	q->size = 0;
	q->cap = 10;
	q->strings = (char**) malloc(sizeof(char*) * q->cap);
	return q;
}

// Free string queue memory
void FreeStringQueue(string_queue* q)
{
	int i;
	for (i = 0; i < q->size; i++)
		if (q->strings[i] != NULL)
			free(q->strings[i]);
	free(q->strings);
	free(q);
}

// Tests
/*int main()
{
	string_queue* q = CreateStringQueue();
	EnqueueString(q, "goodbye");
	EnqueueString(q, "cruel");
	EnqueueString(q, "world");
	assert(q->size == 3);
	char* str = DequeueString(q);
	assert(strcmp("goodbye", str) == 0);
	free(str);
	str = DequeueString(q);
	assert(strcmp("cruel", str) == 0);
	free(str);
	str = DequeueString(q);
	assert(strcmp("world", str) == 0);
	free(str);
	assert(q->size == 0);
	FreeStringQueue(q);
	return 0;
}*/


