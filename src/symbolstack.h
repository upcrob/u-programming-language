/*
 * Stack of symbol tables.
 */
#pragma once

#include "identifiertypes.h"

// Stack structure
typedef struct
{
	int size;
	int cap;
	symbol_table** tlist;
	int offsetCounter;
} symbol_stack;

// Reset offset counter
void ResetOffsetCounter(symbol_stack* stack);

// Add a symbol to the top level
void AddSymbol(symbol_stack* stack, char* lexeme, identifier_type type);

// Lookup a symbol starting with the top level and progressing downward
symtab_entry* LookupSymbol(symbol_stack* stack, char* lexeme);

// Push a new symbol table onto the stack
void PushTable(symbol_stack* stack);

// Pop a symbol table from the stack
void PopTable(symbol_stack* stack);

// Create a symbol table stack
symbol_stack* CreateSymbolStack();

// Free a symbol table stack
void FreeSymbolStack(symbol_stack* stack);


