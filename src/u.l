/*
 * u.l
 * (f)lex lexer token definition file for the U programming language.
 */

%{
	#include "defines.h"
	#include "u.tab.h"   
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

	symbol_stack* symStack;
	function_table* fTable;
	string_table* strTable;
	string_table* fileTable;
	string_queue* fileQueue;	// file queue for the main compiler
	string_queue* pFileQueue;	// file queue for the pre-processor
	int currentLine = 1;
	char* currentFile;
	int errCount = 0;
	struct tree_node* treeRoot;
	
	// Wrap function
	int yywrap(void)
	{
		if (fileQueue->size == 0)
		{
			// No more files to preprocess
			fclose(uin);
			free(currentFile);
			return 1;
		} else {
			// Close previous file
			fclose(uin);
		
			// Open next file
			free(currentFile);
			currentFile = DequeueString(fileQueue);
			uin = fopen(currentFile, "r");
			if (uin == NULL)
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

%option prefix="u"
%option outfile="ulex.c"

%%

[/][\*] {
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
			
[\*][/] {
	return ENDMULTICOMMENT;
}

import {
	return IMPORT;
}

while {
	return WHILE;
}

else[ \t]+if {
	return ELSEIF;
}

if {
	return IF;
}

else {
	return ELSE;
}

true {
	return LTRUE;
}

null {
	return LNULL;
}

false {
	return LFALSE;
}

asm {
	return ASM;
}

return {
	return RETURN;
}

void {
	return VOID;
}

byte {
	return BYTE;
}

word {
	return WORD;
}

bool {
	return BOOL;
}

byte\[\] {
	return BYTEP;
}

word\[\] {
	return WORDP;
}

end	{
	return END;
}

segment {
	return SEGMENT;
}

offset {
	return OFFSET;
}

[\"][^\n]*[\"] {
	char* litStr = yytext + 1;
	litStr = strdup(litStr);
	litStr[strlen(litStr) - 1] = '\0';
	ulval.sval = litStr;
	
	return STRING_LITERAL;
}

'.' {
	char* litChar = yytext + 1;
	litChar = strdup(litChar);
	litChar[1] = '\0';
	ulval.sval = litChar;
	return CHAR;
}

[abcd][hlx] {
	strToLower(yytext);
	ulval.ival = 0;
	if (strcmp(yytext, "ax") == 0)
		ulval.ival = AX;
	else if (strcmp(yytext, "bx") == 0)
		ulval.ival = BX;
	else if (strcmp(yytext, "cx") == 0)
		ulval.ival = CX;
	else if (strcmp(yytext, "dx") == 0)
		ulval.ival = DX;
	else if (strcmp(yytext, "ah") == 0)
		ulval.ival = AH;
	else if (strcmp(yytext, "al") == 0)
		ulval.ival = AL;
	else if (strcmp(yytext, "bh") == 0)
		ulval.ival = BH;
	else if (strcmp(yytext, "bl") == 0)
		ulval.ival = BL;
	else if (strcmp(yytext, "ch") == 0)
		ulval.ival = CH;
	else if (strcmp(yytext, "cl") == 0)
		ulval.ival = CL;
	else if (strcmp(yytext, "dh") == 0)
		ulval.ival = DH;
	else if (strcmp(yytext, "dl") == 0)
		ulval.ival = DL;
	return G_REG;
}

[sd][i] {
	strToLower(yytext);
	ulval.ival = 0;
	if (strcmp(yytext, "si") == 0)
		ulval.ival = SI;
	else if (strcmp(yytext, "di") == 0)
		ulval.ival = DI;
	return G_REG;
}

[0-9][0-9A-Fa-f]*[h] {
	ulval.ival = strtol(yytext, NULL, 16);
	return HEX;
}

[01]+[bB] {
	ulval.ival = bintoint(yytext);
	return BIN;
}

int {
	return INT;
}

mov {
	return MOV;
}

call {
	return CALL;
}

[=][=] {
	return EQ;
}

[!][=] {
	return NEQ;
}

[>][=] {
	return GTE;
}

[<][=] {
	return LTE;
}

[>] {
	return GT;
}

[<] {
	return LT;
}

[a-zA-Z][a-z_0-9A-Z]* {
	ulval.sval = strdup(yytext);
	symtab_entry* entry = LookupSymbol(symStack, yytext);
	if (entry == NULL)
	{
		// Entry doesn't exist in symbol table, check function table
		function* f = LookupFunction(fTable, yytext);
		if (f == NULL)
		{
			// Identifier not defined
			return IDENT_UNDEC;
		} else if (f->type == IT_VOID) {
			// Void identifier
			return FIDENT_VOID;
		} else if (f->type == IT_BYTE) {
			// Byte identifier
			return FIDENT_BYTE;
		} else if (f->type == IT_WORD) {
			// Word identifier
			return FIDENT_WORD;
		} else if (f->type == IT_BOOL) {
			// Bool identifier
			return FIDENT_BOOL;
		} else if (f->type == IT_BYTEP) {
			// Byte[] identifier
			return FIDENT_BYTEP;
		} else {
			// Word[] identifier
			return FIDENT_WORDP;
		}
	} else if (entry->type == IT_BYTE) {
		// Byte identifier
		return IDENT_BYTE;
	} else if (entry->type == IT_WORD) {
		// Word identifier
		return IDENT_WORD;
	} else if (entry->type == IT_BOOL) {
		// Bool identifier
		return IDENT_BOOL;
	} else if (entry->type == IT_BYTEP) {
		// Byte[] identifier
		return IDENT_BYTEP;
	} else {
		// Word[] identifier
		return IDENT_WORDP;
	}
}

[0-9]+ {
	ulval.ival = atoi(yytext);
	return INTEGER;
}

[/][/][^\n]*[\n] {
	currentLine++;
}

[ \t]+	;

[\r?\n] {
	currentLine++;
}

. {
	return yytext[0];
}

%%
