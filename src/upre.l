%{
	#include "defines.h"  
	#include "upre.tab.h"  
	#include "symboltable.h"
	#include "symbolstack.h"
	#include "strutil.h"
	#include "functiontable.h"
	#include "parsetree.h"
	#include <ctype.h>
	#include <string.h>
	#include "stringtable.h"
	#include "stringqueue.h"
	#include <malloc.h>

	// External variables
	extern char* currentFile;
	extern int currentLine;
	extern int errCount;
	extern string_table* strTable;
	extern string_table* fileTable;
	extern struct tree_node* treeRoot;
	extern symbol_stack* symStack;
	extern function_table* fTable;
	extern string_queue* fileQueue;
	extern string_queue* pFileQueue;
	
	// Lexer variables
	int blockDepth = 0;
	
	// Wrap function
	int yywrap(void)
	{
		if (pFileQueue->size == 0)
		{
			// No more files to preprocess
			fclose(uprein);
			free(currentFile);
			return 1;
		} else {
			// Close previous file
			fclose(uprein);
		
			// Open next file
			free(currentFile);
			currentFile = DequeueString(pFileQueue);
			uprein = fopen(currentFile, "r");
			if (uprein == NULL)
			{
				char* bn = getBasename(currentFile);
				printf("imported file '%s' does not exist\n", bn);
				free(bn);
				free(currentFile);
				exit(1);
			}
			
			// Return code to continue
			return 0;
		}
	}
%}

%option prefix="upre"
%option outfile="uprelex.c"

%%

[/][\*]		{
				int i = currentLine;
				while (TRUE)
				{
					int v = yylex();
					if (v == ENDMULTICOMMENT)
						break;
					
					if (v == 0)
					{
						printf("unexpected end of file, unmatched '/*' on line %d\n", i);
						exit(1);
					} else if (strcmp(yytext, "\n") == 0) {
						currentLine++;
					}
				}
			}
			
[\*][/]		{
				return ENDMULTICOMMENT;
			}
			
import		{ return IMPORT; }

[\"][^\n]*[\"]	{
					char* litStr = yytext + 1;
					litStr = strdup(litStr);
					litStr[strlen(litStr) - 1] = '\0';
					uprelval.sval = litStr;
					return STRING_LITERAL;
				}

while		{ blockDepth++; }

else[\t ]+if	;

if			{ blockDepth++; }

asm			{ blockDepth++; }

void		{ return VOID; }

byte		{ return BYTE; }

word		{ return WORD; }

bool		{ return BOOL; }

byte\[\] 	{ return BYTEP; }

word\[\]	{ return WORDP; }

end			{ if (blockDepth == 0) { return END; } else { blockDepth--; } }

"("			{ return OPAREN; }

")"			{ return CPAREN; }

","			{ return COMMA; }

";"			{ return SEMICOLON; }

[/][/][^\n]*[\n] { currentLine++; }

[a-zA-Z][a-z_0-9A-Z]* { uprelval.sval = strdup(yytext); return IDENT; }

[ \t]+	;

[\r?\n]	{ currentLine++; }

.	;


%%
