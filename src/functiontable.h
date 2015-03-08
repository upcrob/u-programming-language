/*
 * Function table code.
 */

#pragma once

#include "identifiertypes.h"

// Function structure
typedef struct
{
	identifier_type type;
	char* lexeme;
	int numParams;
	int maxParams;
	identifier_type* params;
	int frameSize;
	int paramSize;
	int called;
} function;

// Function table structure
typedef struct
{
	int cap;
	int size;
	function** funcs;
} function_table;

// Create a new symbol table
function_table* CreateFunctionTable();

// Free the symbol table
void FreeFunctionTable(function_table* table);

// Add a variable to a function's parameter list
void AddParameter(function_table* table, char* fname, identifier_type type);

// Add a function
void AddFunction(function_table* table, identifier_type type, char* lexeme);

// Determine if a function has been defined
int FunctionDefined(function_table* table, char* lexeme);

// Set a function as defined
void DefineFunction(function_table* table, char* lexeme);

// Lookup function
function* LookupFunction(function_table* table, char* lexeme);

// Print function table for debugging
void PrintFunctionTable(function_table* table);


