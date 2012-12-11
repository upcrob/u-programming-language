%{
	// General defines
	#define YYDEBUG 1

	// Includes
	#include <stdio.h>
	#include <string.h>
	#include "defines.h"
	#include "identifiertypes.h"
	#include "symboltable.h"
	#include "symbolstack.h"
	#include "functiontable.h"
	#include "parsetree.h"
	#include "strutil.h"
	#include "stringtable.h"
	#include "list.h"
	#include "stringqueue.h"

	// External variables
	extern int currentLine;
	extern int errCount;
	extern char* currentFile;
	extern symbol_stack* symStack;
	extern function_table* fTable;
	extern struct tree_node* treeRoot;
	extern string_table* strTable;
	extern string_queue* fileQueue;
	extern string_queue* pFileQueue;
	
	// Parser variables
	identifier_type params[255];	// Holds current parameter list
	int numParams;					// Number of parameters in current parameter list
%}

%name-prefix "upre"

%union
{
	int ival;
	char* sval;
	int typeval;
}

%error-verbose

// Tokens
%token <sval> IDENT
%token VOID
%token BYTE
%token WORD
%token BOOL
%token BYTEP
%token WORDP
%token END
%token IF
%token WHILE
%token ASM
%token OPAREN
%token CPAREN
%token COMMA
%token SEMICOLON
%token ENDMULTICOMMENT
%token IMPORT
%token <sval> STRING_LITERAL

// Precedence
%left END
%left IF
%left WHILE
%left IDENT

/* Types. */
%type <typeval> vtype
%type <typeval> ftype
%type <sval> garbage

%%
program:	component_list
		 	;
		 
component_list:	function
				| function component_list
				| import_statement component_list
				;
				
import_statement:	IMPORT STRING_LITERAL SEMICOLON	{
															// Build relative file string
															char* curPath = directoryPath(currentFile);
															char* relPath = (char*) malloc(strlen(curPath)
																+ strlen($2) + 1);
															strcpy(relPath, curPath);
															strcat(relPath, $2);

															// Add string to file queue
															EnqueueString(fileQueue, relPath);
															EnqueueString(pFileQueue, relPath);
															
															// Free memory
															free(curPath);
															free(relPath);
															free($2);
														}

function:	ftype IDENT OPAREN parameter_list CPAREN garbage END	{
																		// Add function to table
																		AddFunction(fTable, $1, $2);
																		
																		// Add parameters
																		int i;
																		for (i = 0; i < numParams; i++)
																		{
																			AddParameter(fTable, $2, params[i]);
																		}
																		
																		free($2);
																	}
			| ftype IDENT OPAREN CPAREN garbage END	{
														// Add function to table
														AddFunction(fTable, $1, $2);
														free($2);
													}
			;
			
parameter_list: vtype IDENT	{
								numParams = 1;
								params[0] = $1;
								free($2);
							}
				| parameter_list COMMA vtype IDENT	{
														params[numParams++] = $3;
														free($4);
													}
				;
				
garbage:	IDENT { $$ = ""; free($1); }
			| STRING_LITERAL { $$ = ""; free($1); }
			| BYTE { $$ = ""; }
			| WORD { $$ = ""; }
			| BOOL { $$ = ""; }
			| BYTEP { $$ = ""; }
			| WORDP { $$ = ""; }
			| OPAREN { $$ = ""; }
			| CPAREN { $$ = ""; }
			| garbage BYTE { $$ = ""; }
			| garbage WORD { $$ = ""; }
			| garbage BOOL { $$ = ""; }
			| garbage BYTEP { $$ = ""; }
			| garbage WORDP { $$ = ""; }
			| garbage IDENT { $$ = ""; free($2); }
			| garbage COMMA { $$ = ""; }
			| garbage OPAREN { $$ = ""; }
			| garbage CPAREN { $$ = ""; }
			| garbage STRING_LITERAL { $$ = ""; free($2); }
			| garbage SEMICOLON { $$ = ""; }
			;
	
ftype:	VOID { $$ = IT_VOID; }
		| vtype
		;
		
vtype:	BYTE { $$ = IT_BYTE; }
		| WORD { $$ = IT_WORD; }
		| BYTEP { $$ = IT_BYTEP; }
		| WORDP { $$ = IT_WORDP; }
	  ;
%%

/* Error function. */
int yyerror(char* ErrMessage)
{
    printf("preprocessor: %s on line %d\n", ErrMessage, currentLine);
    errCount++;
	return 0;
}

