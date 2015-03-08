#pragma once

#include "identifiertypes.h"

/////////////////////////////////////////////////
// Elements in the symbol table are of this type
/////////////////////////////////////////////////
typedef struct symtab_entry_tag
{
    char*				lexeme;  // The lexeme for the identifier
    identifier_type		type;	// Identifier type yet
    int					offset;
} symtab_entry;

struct symtab_list_node
{
    symtab_entry* pSymbol;
    struct symtab_list_node *pNextNode;
};

typedef struct symtab_list_node symbol_table;

///////////////////////////////////////////////////////
// There should be one symbol table for each "scope" of
// variables. You call this method to create the symbol
// table the first time. Thereafter, you pass it to
// LookUp( ) and Add( ) to fetch and add symbols to the 
// symbol table.
// 
// When you are done with the symbol table don't forget
// to call FreeSymbolTable( ) on the pointer you obtained
// from CreateSymbolTable( ).
///////////////////////////////////////////////////////
symbol_table* CreateSymbolTable();
void FreeSymbolTable(symbol_table *pSymbolTable);

///////////////////////////////////////////////////////////////
// This looks in pSymbolTable for the identifier whose name is 
// is Lexeme. If it finds it, the symtab_entry for that 
// identifier is returned. Otherwise, NULL is returned.
// NOTE: When an identifier that you Lookup( ) is found
// in the table, you must still check its Type to be sure it
// matches its current usage.
///////////////////////////////////////////////////////////////
symtab_entry* Lookup(symbol_table *pSymbolTable, char* Lexeme);

/////////////////////////////////////////////////////////////////
// This attempts to add a new identifier to pSymbolTable. 
// The new symbol will be associated with the identifier name
// Lexeme and be of the type specified by Type. It will receive
// a default value of
// "" for string types
// { } for set types.
//
// Before Adding a symbol to the parse tree you must first call
// Lookup( ) to verify that 
// the symbol is not already in the symbol table (if it is, just
// use the return value of Lookup( )...no need to Add( ) it)
/////////////////////////////////////////////////////////////////
symtab_entry* Add(symbol_table *pSymbolTable, char* Lexeme, identifier_type Type, int Offset); 




