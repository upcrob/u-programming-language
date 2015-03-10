/*
 * Table for storing string literals.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringtable.h"
#include <assert.h>

// Create a string table
string_table* CreateStringTable()
{
	string_table* t = (string_table*) malloc(sizeof(string_table));
	t->cap = 10;
	t->size = 0;
	t->strings = (char**) malloc(sizeof(char*) * t->cap);
	return t;
}

// Free string table
void FreeStringTable(string_table* table)
{
	int i;
	for (i = 0; i < table->size; i++)
		free(table->strings[i]);
	free(table->strings);
	free(table);
}

// Add a string
void AddString(string_table* table, char* str)
{
	// replace escape characters with corresponding ASCII values
	// BACKSLASH ('\\') -> \
	// NL ('\n') -> 10
	int i, j;
	int len = strlen(str);
	for (i = 0; i < len - 1; i++)
	{
		if (str[i] == '\\' && str[i + 1] == 'n')
		{
			str[i] = 10;
			for (j = i + 1; j < len; j++)
				str[j] = str[j + 1];
		} else if (str[i] == '\\' && str[i + 1] == '\\') {
			str[i] = '\\';
			for (j = i + 1; j < len; j++)
				str[j] = str[j + 1];
		}
	}
	
	// lookup string to see if it exsists
	if (LookupString(table, str) != -1)
		return;

	// add to table
	if (table->size >= table->cap)
	{
		// Expand array
		table->cap *= 2;
		char** tmp = (char**) malloc(sizeof(char*) * table->cap);
		for (i = 0; i < table->size; i++)
			tmp[i] = table->strings[i];
		free(table->strings);
		table->strings = tmp;
	}
	
	table->strings[table->size++] = strdup(str);
}

// Lookup the index of a string, returns -1 if not found
int LookupString(string_table* table, char* str)
{
	int i;
	for (i = 0; i < table->size; i++)
		if (strcmp(str, table->strings[i]) == 0)
			return i;
	return -1;
}

// TESTS
/*
int main()
{
	string_table* t = CreateStringTable();
	
	AddString(t, "hello");
	AddString(t, "world");
	AddString(t, "something else");
	assert(LookupString(t, "hello") == 0);
	assert(LookupString(t, "kljf") == -1);
	assert(LookupString(t, "something else") == 2);
	
	FreeStringTable(t);
	return 0;
}*/


