/*
 * Function table code.
 */

#include "defines.h"
#include <stdio.h>
#include <malloc.h>
#include "functiontable.h"
#include "identifiertypes.h"
#include "strutil.h"
#include "assert.h"
#include <string.h>

// Free a function
void FreeFunction(function* f)
{
	int i;
	free(f->lexeme);
	free(f->params);
	free(f);
}

// Create a new symbol table
function_table* CreateFunctionTable()
{
	function_table* t = (function_table*) malloc(sizeof(function_table));
	t->size = 0;
	t->cap = 10;
	t->funcs = (function**) malloc(sizeof(function*) * t->cap);
	return t;
}

// Free the symbol table
void FreeFunctionTable(function_table* table)
{
	int i;
	for (i = 0; i < table->size; i++)
		FreeFunction(table->funcs[i]);
	free(table->funcs);
	free(table);
}

// Add a variable to a function's parameter list
void AddParameter(function_table* table, char* fname, identifier_type type)
{
	int i;
	function* f = LookupFunction(table, fname);
	if (f == NULL)
		return;

	if (f->numParams >= f->maxParams)
	{
		// Expand array
		f->maxParams *= 2;
		identifier_type* tmp = (identifier_type*) malloc(sizeof(identifier_type) * f->maxParams);
		for (i = 0; i < f->numParams; i++)
			tmp[i] = f->params[i];
		free(f->params);
		f->params = tmp;
	}

	// Update parameter stack size
	switch (type)
	{
		case IT_BYTE:
		case IT_BOOL:
		case IT_WORD:
			f->paramSize += 2;
			break;
		case IT_BYTEP:
		case IT_WORDP:
			f->paramSize += 4;
			break;
	}
		
	f->params[f->numParams++] = type;
}

// Add a function
void AddFunction(function_table* table, identifier_type type, char* lexeme)
{
	int i;
	function* f = (function*) malloc(sizeof(function));
	f->type = type;
	f->lexeme = strdup(lexeme);
	f->numParams = 0;
	f->maxParams = 10;
	f->params = (identifier_type*) malloc(sizeof(identifier_type) * f->maxParams);
	f->frameSize = 0;
	f->paramSize = 0;
	f->called = FALSE;

	if (table->size >= table->cap)
	{
		// Expand array
		table->cap *= 2;
		function** tmp = (function**) malloc(sizeof(function*) * table->cap);
		for (i = 0; i < table->size; i++)
			tmp[i] = table->funcs[i];
		free(table->funcs);
		table->funcs = tmp;
	}

	table->funcs[table->size++] = f;
}

// Lookup function
function* LookupFunction(function_table* table, char* lexeme)
{
	int i;
	for (i = 0; i < table->size; i++)
		if (streq(lexeme, table->funcs[i]->lexeme))
			return table->funcs[i];
	return NULL;
}

// Print function table for debugging
void PrintFunctionTable(function_table* table)
{
	int i, j;
	printf("FUNCTION TABLE:\n");
	for (i = 0; i < table->size; i++)
	{
		function* f = table->funcs[i];
		printf("%s (%s): FRAMESIZE: %d  PARAMSIZE: %d\n", f->lexeme, getTypeString(f->type), f->frameSize, f->paramSize);
		
		for (j = 0; j < f->numParams; j++)
		{
			printf("\t%s\n", getTypeString(f->params[j]));
		}
	}
}

// TESTS
/*
int main()
{
	function_table* t = CreateFunctionTable();
	
	AddFunction(t, IT_BYTE, "func1", FALSE);
	AddFunction(t, IT_WORD, "func2", TRUE);

	function* f = LookupFunction(t, "fulkj");
	assert(f == NULL);
	f = LookupFunction(t, "func1");
	assert(strcmp("func1", f->lexeme) == 0);
	assert(f->type == IT_BYTE);

	AddParameter(t, "func2", IT_BYTE);
	AddParameter(t, "func2", IT_WORD);
	AddParameter(t, "func2", IT_BYTEP);
	assert(LookupFunction(t, "func2")->numParams == 3);
	assert(LookupFunction(t, "func2")->params[0] == IT_BYTE);
	assert(LookupFunction(t, "func2")->params[2] == IT_BYTEP);

	FreeFunctionTable(t);
	return 0;
}
*/


