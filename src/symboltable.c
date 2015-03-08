#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symboltable.h"
#include "identifiertypes.h"

symbol_table* CreateSymbolTable()
{
    symbol_table* pST = malloc(sizeof(symbol_table));
    pST->pSymbol = NULL;  // only NULL for the root of the tree
                          // and until the first symbol is Add()ed
    pST->pNextNode = NULL; // it's a linked list

    return pST;
}

void FreeSymbolTable(symbol_table *pSymbolTable)
{
    if ( pSymbolTable == NULL )
        return;

    FreeSymbolTable(pSymbolTable->pNextNode);
    // all the nodes beyond this one are gone so 
    // now deallocate this one
    if ( pSymbolTable->pSymbol )
    {
        free(pSymbolTable->pSymbol->lexeme);
        free(pSymbolTable->pSymbol);
    }
    free(pSymbolTable);  // we really are just freeing this current
                         // symbol table node...but when the
                         // recursion is over, they're all gone.
}

symtab_entry* Lookup(symbol_table* pST, char* Lexeme)
{
    // Search the symbol table for an entry matching Lexeme
    while ( pST )
    {   
        if ( pST->pSymbol )
        {
             if ( strcmp(pST->pSymbol->lexeme, Lexeme) == 0 )
                 return pST->pSymbol;  // found it
        }
        pST = pST->pNextNode;
    }
    return NULL;  // didn't find it
}

symtab_entry* Add(symbol_table *pST, char* Lexeme, identifier_type Type, int Offset)
{
    // The symbol should not already be in this table
    assert(Lookup(pST, Lexeme) == NULL);
    assert(strlen(Lexeme) > 0); 

    // not already in the table so create a new one
    symtab_entry* pSymbol = malloc(sizeof(symtab_entry));
    pSymbol->lexeme = strdup(Lexeme);
    pSymbol->type = Type;
    pSymbol->offset = Offset;
	
    // Now place this symbol into a symbol table node in the linked list
    if ( pST->pSymbol == NULL )
    {  // there are no symbols in the table yet, just the default
       // root node
        assert(pST->pNextNode == NULL);
        pST->pSymbol = pSymbol;
    }
    else
    {
        struct symtab_list_node *pNewNode = 
                     malloc(sizeof(struct symtab_list_node));
        pNewNode->pSymbol = pSymbol;
        pNewNode->pNextNode = NULL;  // this one will go at the end

        // track to end of the table and add it there
        while ( pST->pNextNode )
            pST = pST->pNextNode;
        pST->pNextNode = pNewNode;
    }
    return pSymbol;  // return the symbol
}


//////////////////////////////////////////////////////////////////
// Unit test code
//////////////////////////////////////////////////////////////////
/*
#include <stdio.h>
int main()
{
    symbol_table* pST = CreateSymbolTable();

    printf("Checking for symbols in the empty symbol table...\n");
    assert(Lookup(pST, "Hello") == NULL);

    printf("Adding a set to the symbol table...\n");
    Add(pST, "SetVar", IT_BYTE);

    printf("Adding a string to the symbol table...\n");
    Add(pST, "StrVar", IT_WORD);

    printf("Looking up the two symbols just added...\n");
    symtab_entry* pS1 = Lookup(pST, "SetVar");
    assert(pS1);
    assert(pS1->type == IT_BYTE);

    symtab_entry* pS2 = Lookup(pST, "StrVar");
    assert(pS2);
    assert(pS2->type == IT_WORD);

    printf("All tests passed.\n");

    FreeSymbolTable(pST);

    return 0;
}
*/


