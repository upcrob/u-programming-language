/*
 * Functions for adding/removing strings from a queue.
 */

typedef struct
{
	int cap;
	int size;
	char** strings;
} string_queue;

void EnqueueString(string_queue* q, char* str);

char* DequeueString(string_queue* q);

string_queue* CreateStringQueue();

void FreeStringQueue(string_queue* q);

