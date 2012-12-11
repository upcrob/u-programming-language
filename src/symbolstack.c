/*
 * Stack of symbol tables.
 */

#include <assert.h>
#include "symboltable.h"
#include "identifiertypes.h"
#include <malloc.h>
#include "symbolstack.h"
#include <stdio.h>

// Reset offset counter
void ResetOffsetCounter(symbol_stack* stack)
{
	stack->offsetCounter = 0;
}

// Add a symbol to the top level
void AddSymbol(symbol_stack* stack, char* lexeme, identifier_type type)
{
	int offset;
	switch (type)
	{
		case IT_BYTE:
		case IT_BOOL:
		case IT_WORD:
			offset = stack->offsetCounter;
			stack->offsetCounter += 2;
			break;
		case IT_BYTEP:
		case IT_WORDP:
			offset = stack->offsetCounter;
			stack->offsetCounter += 4;
			break;
		default:
			offset = 0;
	}

	if (stack->size > 0)
		Add(stack->tlist[stack->size - 1], lexeme, type, offset);
}

// Lookup a symbol starting with the top level and progressing downward
symtab_entry* LookupSymbol(symbol_stack* stack, char* lexeme)
{
	int i;
	symtab_entry* entry;
	for (i = stack->size - 1; i >= 0; i--)
	{
		entry = Lookup(stack->tlist[i], lexeme);
		if (entry != NULL)
			return entry;
	}

	return NULL;
}

// Push a new symbol table onto the stack
void PushTable(symbol_stack* stack)
{
	int i;

	if (stack->size >= stack->cap)
	{
		// Expand stack
		stack->cap *= 2;
		symbol_table** newList = (symbol_table**) malloc(sizeof(symbol_table*) * stack->cap);

		for (i = 0; i < stack->size; i++)
			newList[i] = stack->tlist[i];

		free(stack->tlist);
		stack->tlist = newList;
	}

	stack->tlist[stack->size++] = CreateSymbolTable();
}

// Pop a symbol table from the stack
void PopTable(symbol_stack* stack)
{
	if (stack->size > 0)
	{
		stack->size--;
		FreeSymbolTable(stack->tlist[stack->size]);
	}
}

// Create a symbol table stack
symbol_stack* CreateSymbolStack()
{
	symbol_stack* s = (symbol_stack*) malloc(sizeof(symbol_stack));
	s->size = 0;
	s->cap = 10;
	s->tlist = (symbol_table**) malloc(sizeof(symbol_table*) * s->cap);
	s->offsetCounter = 0;
	return s;
}

// Free a symbol table stack
void FreeSymbolStack(symbol_stack* stack)
{
	int i;
	for (i = 0; i < stack->size; i++)
		FreeSymbolTable(stack->tlist[i]);
	free(stack->tlist);
	free(stack);
}


// Tests
/*
#include <stdio.h>
int main()
{
	symbol_stack* stack = CreateSymbolStack();
	PushTable(stack);
	
	AddSymbol(stack, "myVar", IT_BYTE);
	AddSymbol(stack, "myVar2", IT_BYTE);

	PushTable(stack);

	AddSymbol(stack, "myVar", IT_WORD);

	symtab_entry* entry;
	entry = LookupSymbol(stack, "myVar");

	assert(entry->type == IT_WORD);

	entry = LookupSymbol(stack, "myVar2");
	assert(entry->type == IT_BYTE);
	PopTable(stack);

	entry = LookupSymbol(stack, "myVar");
	assert(entry->type == IT_BYTE);
	
	PopTable(stack);
	
	entry = LookupSymbol(stack, "myVar");
	assert(entry == NULL);

	FreeSymbolStack(stack);
}*/



