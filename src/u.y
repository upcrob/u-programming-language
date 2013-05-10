/*
 * u.y
 * Main Bison grammar file for U programming language.
 */

%{
	// General defines
	#define YYDEBUG 1
	#define VERSION "0.02"

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
	#include "stringqueue.h"
	#include "prunefunctions.h"
	#include <regex.h>
	#include "optimizer.h"

	extern char* currentFile;
	extern symbol_stack* symStack;
	extern function_table* fTable;
	extern struct tree_node* treeRoot;
	extern string_table* strTable;
	extern string_table* fileTable;
	extern string_queue* fileQueue;
	extern string_queue* pFileQueue;
%}

%name-prefix "u"

%union
{
	int ival;
	char* sval;
	int typeval;
	struct tree_node* tnode;
}

%error-verbose

/* Tokens from scanner. */
%token <ival> INTEGER
%token <sval> IDENT_UNDEC
%token <sval> IDENT_BYTE
%token <sval> IDENT_WORD
%token <sval> IDENT_BOOL
%token <sval> IDENT_BYTEP
%token <sval> IDENT_WORDP
%token <sval> FIDENT_VOID
%token <sval> FIDENT_BYTE
%token <sval> FIDENT_WORD
%token <sval> FIDENT_BOOL
%token <sval> FIDENT_BYTEP
%token <sval> FIDENT_WORDP
%token <sval> CHAR
%token IMPORT
%token WHILE
%token IF
%token ELSE
%token ELSEIF
%token LTRUE
%token LFALSE
%token LNULL
%token ASM
%token RETURN
%token VOID
%token BYTE
%token WORD
%token BOOL
%token BYTEP
%token WORDP
%token END
%token EQ
%token NEQ
%token GTE
%token LTE
%token GT
%token LT
%token ENDMULTICOMMENT
%token <sval> STRING_LITERAL

%token MOV
%token <ival> G_REG
%token <ival> HEX
%token <ival> BIN
%token INT

/* Associativity and precedence. */
%left ','
%left EQ NEQ
%left GT LT GTE LTE
%left '+' '-'
%left '*' '/' '%'
%left '!'
%left ':'

/* Types. */
%type <typeval> vtype
%type <typeval> ftype
%type <tnode> program
%type <tnode> component_list
%type <tnode> function
%type <tnode> statement_list
%type <tnode> statement
%type <tnode> int_exp
%type <tnode> bool_exp
%type <tnode> ptr_exp
%type <tnode> expression
%type <tnode> decl_assign_statement
%type <sval> declared_identifier
%type <sval> declared_func_identifier
%type <tnode> assignment_statement
%type <tnode> arg_list
%type <tnode> int_function_call
%type <tnode> ptr_function_call
%type <tnode> void_function_call
%type <tnode> undec_function_call
%type <tnode> while_statement
%type <tnode> while_header
%type <tnode> if_header
%type <tnode> elseif_header
%type <tnode> else_clause
%type <tnode> if_statement
%type <tnode> elseif_block
%type <tnode> return_statement
%type <tnode> asm_line
%type <tnode> asm_list
%type <tnode> asm_op
%type <tnode> int_op
%type <tnode> bool_op
%type <tnode> ptr_op
%type <tnode> asm_statement
%type <tnode> asm_memloc
%type <tnode> parameter
%type <tnode> parameter_list
%type <tnode> function_header
%type <tnode> function_preamble

%%
program:
	component_list {
		treeRoot = $1;
	}
;

function_preamble:
	ftype declared_func_identifier '(' {
		$$ = newTreeNode();
		$$->type = TN_FDEF;
		$$->sval = strdup($2);
		PushTable(symStack);
		free($2);
	}
;

function_header:
	function_preamble parameter_list ')' {
		$$ = $1;
		$$->numOperands = 1;
		$$->operands[0] = $2;
	}
	| function_preamble ')'	{
		$$ = $1;
	}
;

function:
	function_header statement_list END {
		$$ = newTreeNode();
		$$->type = TN_FUNCTION;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $2;
		function* f = LookupFunction(fTable, $1->sval);
		f->frameSize = symStack->offsetCounter;
		ResetOffsetCounter(symStack);
		PopTable(symStack);
	}
	| function_header END {
		$$ = newTreeNode();
		$$->type = TN_FUNCTION;
		$$->numOperands = 1;
		$$->operands[0] = $1;
		function* f = LookupFunction(fTable, $1->sval);
		f->frameSize = symStack->offsetCounter;
		ResetOffsetCounter(symStack);
		PopTable(symStack);
	}
;

import_statement:
	IMPORT STRING_LITERAL ';' {
		free($2);
	}
;

component_list:
	function
	| function component_list {
		$1->pNextStatement = $2;
		$$ = $1;
	}
	| import_statement component_list {
		$$ = $2;
	}
;

statement:
	decl_assign_statement
   | assignment_statement
   | while_statement
   | if_statement
   | int_function_call ';'
   | void_function_call ';'
   | asm_statement
   | return_statement
   | undec_function_call ';'
;

statement_list:
	statement
	| decl_statement statement_list {
		$$ = $2;
	}
	| statement statement_list	{
		$1->pNextStatement = $2;
		$$ = $1;
	}
;

decl_assign_statement:
	BYTE IDENT_UNDEC '=' int_exp ';' {
		AddSymbol(symStack, $2, IT_BYTE);
		struct tree_node* ident = newTreeNode();
		ident->type = TN_BYTE_IDENT;
		ident->sval = strdup($2);
		ident->ival = LookupSymbol(symStack, $2)->offset;
		$$ = newTreeNode();
		$$->type = TN_BYTE_ASSIGN;
		$$->numOperands = 2;
		$$->operands[0] = ident;
		$$->operands[1] = $4;
		free($2);
	}
	| WORD IDENT_UNDEC '=' int_exp ';' {
		AddSymbol(symStack, $2, IT_WORD);
		struct tree_node* ident = newTreeNode();
		ident->type = TN_WORD_IDENT;
		ident->sval = strdup($2);
		ident->ival = LookupSymbol(symStack, $2)->offset;
		$$ = newTreeNode();
		$$->type = TN_WORD_ASSIGN;
		$$->numOperands = 2;
		$$->operands[0] = ident;
		$$->operands[1] = $4;
		free($2);
	}
	| BOOL IDENT_UNDEC '=' bool_exp ';'			{
		AddSymbol(symStack, $2, IT_BOOL);
		struct tree_node* ident = newTreeNode();
		ident->type = TN_BOOL_IDENT;
		ident->sval = strdup($2);
		ident->ival = LookupSymbol(symStack, $2)->offset;
		$$ = newTreeNode();
		$$->type = TN_BOOL_ASSIGN;
		$$->numOperands = 2;
		$$->operands[0] = ident;
		$$->operands[1] = $4;
		free($2);
	}
	| BYTEP IDENT_UNDEC '=' ptr_exp ';' {
		AddSymbol(symStack, $2, IT_BYTEP);
		struct tree_node* ident = newTreeNode();
		ident->type = TN_PTR_IDENT;
		ident->sval = strdup($2);
		ident->ival = LookupSymbol(symStack, $2)->offset;
		$$ = newTreeNode();
		$$->type = TN_PTR_ASSIGN;
		$$->numOperands = 2;
		$$->operands[0] = ident;
		$$->operands[1] = $4;
		free($2);
	}
	| WORDP IDENT_UNDEC '=' ptr_exp ';' {
		AddSymbol(symStack, $2, IT_WORDP);
		struct tree_node* ident = newTreeNode();
		ident->type = TN_PTR_IDENT;
		ident->sval = strdup($2);
		ident->ival = LookupSymbol(symStack, $2)->offset;
		$$ = newTreeNode();
		$$->type = TN_PTR_ASSIGN;
		$$->numOperands = 2;
		$$->operands[0] = ident;
		$$->operands[1] = $4;
		free($2);
	}
	| vtype declared_identifier '=' expression ';' {
		yyerror("symbol already declared");
		free($2);
		$$ = newTreeNode();
	}
;

decl_statement:
	BYTE IDENT_UNDEC ';' {
		AddSymbol(symStack, $2, IT_BYTE);
		free($2);
	}
	| WORD IDENT_UNDEC ';' {
		AddSymbol(symStack, $2, IT_WORD);
		free($2);
	}
	| BOOL IDENT_UNDEC ';' {
		AddSymbol(symStack, $2, IT_BOOL);
		free($2);
	}
	| BYTEP IDENT_UNDEC ';' {
		AddSymbol(symStack, $2, IT_BYTEP);
		free($2);
	}
	| WORDP IDENT_UNDEC ';' {
		AddSymbol(symStack, $2, IT_WORDP);
		free($2);
	}
	| vtype declared_identifier ';' {
		yyerror("symbol already declared");
		free($2);
	}
;

assignment_statement:
	IDENT_BYTE '=' int_exp ';' {
		$$ = newTreeNode();
		$$->type = TN_BYTE_ASSIGN;
		struct tree_node* ident = newTreeNode();
		ident->type = TN_BYTE_IDENT;
		ident->sval = strdup($1);
		ident->ival = LookupSymbol(symStack, $1)->offset;
		$$->numOperands = 2;
		$$->operands[0] = ident;
		$$->operands[1] = $3;
		free($1);
	}
	| IDENT_WORD '=' int_exp ';' {
		$$ = newTreeNode();
		$$->type = TN_WORD_ASSIGN;
		struct tree_node* ident = newTreeNode();
		ident->type = TN_WORD_IDENT;
		ident->sval = strdup($1);
		ident->ival = LookupSymbol(symStack, $1)->offset;
		$$->numOperands = 2;
		$$->operands[0] = ident;
		$$->operands[1] = $3;
		free($1);
	}
	| IDENT_BYTEP '=' ptr_exp ';' {
		$$ = newTreeNode();
		$$->type = TN_PTR_ASSIGN;
		$$->numOperands = 2;
		struct tree_node* ident = newTreeNode();
		ident->type = TN_PTR_IDENT;
		ident->sval = strdup($1);
		ident->ival = LookupSymbol(symStack, $1)->offset;
		$$->operands[0] = ident;
		$$->operands[1] = $3;
		free($1);
	}
	| IDENT_WORDP '=' ptr_exp ';' {
		$$ = newTreeNode();
		$$->type = TN_PTR_ASSIGN;
		$$->numOperands = 2;
		struct tree_node* ident = newTreeNode();
		ident->type = TN_PTR_IDENT;
		ident->sval = strdup($1);
		ident->ival = LookupSymbol(symStack, $1)->offset;
		$$->operands[0] = ident;
		$$->operands[1] = $3;
		free($1);
	}
	| IDENT_BYTEP '[' int_exp ']' '=' int_exp ';' {
		$$ = newTreeNode();
		$$->type = TN_PTR_BYTE_ASSIGN;
		$$->numOperands = 2;
		$$->sval = strdup($1);
		$$->ival = LookupSymbol(symStack, $1)->offset;
		$$->operands[0] = $3;
		$$->operands[1] = $6;
		free($1);
	}
	| IDENT_WORDP '[' int_exp ']' '=' int_exp ';' {
		$$ = newTreeNode();
		$$->type = TN_PTR_WORD_ASSIGN;
		$$->numOperands = 2;
		$$->sval = strdup($1);
		$$->ival = LookupSymbol(symStack, $1)->offset;
		$$->operands[0] = $3;
		$$->operands[1] = $6;
		free($1);
	}
	| IDENT_BOOL '=' bool_exp ';' {
		$$ = newTreeNode();
		$$->type = TN_BOOL_ASSIGN;
		$$->numOperands = 2;
		struct tree_node* ident = newTreeNode();
		ident->type = TN_BOOL_IDENT;
		ident->sval = strdup($1);
		ident->ival = LookupSymbol(symStack, $1)->offset;
		$$->operands[0] = ident;
		$$->operands[1] = $3;
		free($1);
	}
	| IDENT_UNDEC '=' expression ';' {
		$$ = newTreeNode();
		char err[500];
		sprintf(err, "'%s' undeclared", $1);
		yyerror(err);
		free($1);
	}
;

declared_identifier:
	IDENT_BYTE
	| IDENT_WORD
	| IDENT_BOOL
	| IDENT_BYTEP
	| IDENT_WORDP
;
					
declared_func_identifier:
	FIDENT_VOID
	| FIDENT_BYTE
	| FIDENT_WORD
	| FIDENT_BOOL
	| FIDENT_BYTEP
	| FIDENT_WORDP
;

while_header:
	WHILE '(' bool_exp ')' {
		PushTable(symStack);
		$$ = $3;
	}
;

while_statement:
	while_header statement_list END	{
		PopTable(symStack);
		$$ = newTreeNode();
		$$->type = TN_WHILE;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $2;
	}
;

if_header:
	IF '(' expression ')' {
		PushTable(symStack);
		$$ = $3;
	}
;

else_clause:
	ELSE statement_list END {
		$$ = $2;
	}
;

elseif_header:
	ELSEIF '(' expression ')' {
		PushTable(symStack);
		$$ = $3;
	}
;

elseif_block:
	elseif_header statement_list END {
		PopTable(symStack);
		$$ = newTreeNode();
		$$->type = TN_IF;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $2;
	}
	| elseif_header statement_list else_clause {
		PopTable(symStack);
		$$ = newTreeNode();
		$$->type = TN_IF;
		$$->numOperands = 3;
		$$->operands[0] = $1;
		$$->operands[1] = $2;
		$$->operands[2] = $3;
	}
	| elseif_header statement_list elseif_block	{
		PopTable(symStack);
		$$ = newTreeNode();
		$$->type = TN_IF;
		$$->numOperands = 3;
		$$->operands[0] = $1;
		$$->operands[1] = $2;
		$$->operands[2] = $3;
	}
;
			  
if_statement:
	if_header statement_list END {
		PopTable(symStack);
		$$ = newTreeNode();
		$$->type = TN_IF;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $2;
	}
	| if_header statement_list else_clause	{
		PopTable(symStack);
		$$ = newTreeNode();
		$$->type = TN_IF;
		$$->numOperands = 3;
		$$->operands[0] = $1;
		$$->operands[1] = $2;
		$$->operands[2] = $3;
	}
	| if_header statement_list elseif_block	{
		PopTable(symStack);
		$$ = newTreeNode();
		$$->type = TN_IF;
		$$->numOperands = 3;
		$$->operands[0] = $1;
		$$->operands[1] = $2;
		$$->operands[2] = $3;
	}
;

asm_statement:
	ASM asm_list END {
		$$ = newTreeNode();
		$$->type = TN_ASM;
		$$->numOperands = 1;
		$$->operands[0] = $2;
	}
;

asm_list:
	asm_line
	| asm_line asm_list	{
		$1->pNextStatement = $2;
		$$ = $1;
	}
;

asm_line:
	MOV G_REG ',' asm_op {
		$$ = newTreeNode();
		$$->type = TN_AMOV;
		$$->numOperands = 2;
		$$->operands[0] = newTreeNode();
		$$->operands[0]->type = TN_ASMREG;
		$$->operands[0]->ival = $2;
		$$->operands[1] = $4;
		
	}
	| MOV asm_memloc ',' G_REG {
		$$ = newTreeNode();
		$$->type = TN_AMOV;
		$$->numOperands = 2;
		$$->operands[0] = $2;
		$$->operands[1] = newTreeNode();
		$$->operands[1]->ival = $4;
		$$->operands[1]->type = TN_ASMREG;
	}
	| INT HEX {
		$$ = newTreeNode();
		$$->type = TN_AINT;
		$$->numOperands = 1;
		$$->operands[0] = newTreeNode();
		$$->operands[0]->type = TN_INTEGER;
		$$->operands[0]->ival = $2;
	}
	| INT INTEGER {
		$$ = newTreeNode();
		$$->type = TN_AINT;
		$$->numOperands = 1;
		$$->operands[0] = newTreeNode();
		$$->operands[0]->type = TN_INTEGER;
		$$->operands[0]->ival = $2;
	}
	| INT BIN {
		$$ = newTreeNode();
		$$->type = TN_AINT;
		$$->numOperands = 1;
		$$->operands[0] = newTreeNode();
		$$->operands[0]->type = TN_INTEGER;
		$$->operands[0]->ival = $2;
	}
;

asm_op:
	INTEGER	{
		$$ = newTreeNode();
		$$->type = TN_INTEGER;
		$$->ival = $1;
	}
	| HEX {
		$$ = newTreeNode();
		$$->type = TN_INTEGER;
		$$->ival = $1;
	}
	| BIN {
		$$ = newTreeNode();
		$$->type = TN_INTEGER;
		$$->ival = $1;
	}
	| G_REG {
		$$ = newTreeNode();
		$$->type = TN_ASMREG;
		$$->ival = $1;
	}
	| asm_memloc
	| CHAR {
		$$ = newTreeNode();
		$$->type = TN_CHAR;
		$$->sval = strdup($1);
		free($1);
	}
;

asm_memloc:
	'[' declared_identifier ']' {
		$$ = newTreeNode();
		$$->type = TN_ASMLOC;
		$$->numOperands = 1;
		$$->operands[0] = newTreeNode();
		symtab_entry* sym = LookupSymbol(symStack, $2);
		identifier_type t = sym->type;
		if (t == IT_BYTE)
			$$->operands[0]->type = TN_BYTE_IDENT;
		else if (t == IT_WORD)
			$$->operands[0]->type = TN_WORD_IDENT;
		else if (t == IT_BOOL)
			$$->operands[0]->type = TN_BOOL_IDENT;
		else
			$$->operands[0]->type == TN_PTR_IDENT;
		$$->operands[0]->ival = sym->offset;
		$$->operands[0]->sval = strdup($2);
		free($2);
	}
	| '[' INTEGER ']' {
		$$ = newTreeNode();
		$$->type = TN_ASMLOC;
		$$->numOperands = 1;
		$$->operands[0] = newTreeNode();
		$$->operands[0]->type = TN_INTEGER;
		$$->operands[0]->ival = $2;
	}
	| '[' HEX ']' {
		$$ = newTreeNode();
		$$->type = TN_ASMLOC;
		$$->numOperands = 1;
		$$->operands[0] = newTreeNode();
		$$->operands[0]->type = TN_INTEGER;
		$$->operands[0]->ival = $2;
	}
	| '[' IDENT_UNDEC ']' {
		$$ = newTreeNode();
		yyerror("undeclared identifier");
		free($2);
	}
;
			
int_op:
	INTEGER	{
		$$ = newTreeNode();
		$$->type = TN_INTEGER;
		$$->ival = $1;
	}
	| HEX {
		$$ = newTreeNode();
		$$->type = TN_INTEGER;
		$$->ival = $1;
	}
	| BIN {
		$$ = newTreeNode();
		$$->type = TN_INTEGER;
		$$->ival = $1;
	}
	| int_function_call
	| IDENT_BYTE {
		$$ = newTreeNode();
		$$->type = TN_BYTE_IDENT;
		$$->sval = strdup($1);
		$$->ival = LookupSymbol(symStack, $1)->offset;
		free($1);
	}
	| IDENT_BYTEP '[' int_exp ']' {
		$$ = newTreeNode();
		$$->type = TN_PTR_BYTE;
		$$->sval = strdup($1);
		$$->ival = LookupSymbol(symStack, $1)->offset;
		$$->operands[0] = $3;
		$$->numOperands = 1;
		free($1);
	}
	| CHAR {
		$$ = newTreeNode();
		$$->type = TN_CHAR;
		$$->sval = strdup($1);
		free($1);
	}
	| IDENT_WORD {
		$$ = newTreeNode();
		$$->type = TN_WORD_IDENT;
		$$->sval = strdup($1);
		$$->ival = LookupSymbol(symStack, $1)->offset;
		free($1);
	}
	| IDENT_WORDP '[' int_exp ']' {
		$$ = newTreeNode();
		$$->type = TN_PTR_WORD;
		$$->sval = strdup($1);
		$$->ival = LookupSymbol(symStack, $1)->offset;
		$$->operands[0] = $3;
		$$->numOperands = 1;
		free($1);
	}
;

bool_op:
	IDENT_BOOL {
		$$ = newTreeNode();
		$$->type = TN_BOOL_IDENT;
		$$->sval = strdup($1);
		$$->ival = LookupSymbol(symStack, $1)->offset;
		free($1);
	}
	| LTRUE	{
		$$ = newTreeNode();
		$$->type = TN_TRUE;
	}
	| LFALSE {
		$$ = newTreeNode();
		$$->type = TN_FALSE;
	}
;
			
ptr_op:
	STRING_LITERAL {
		$$ = newTreeNode();
		$$->type = TN_STRING_LITERAL;
		$$->sval = strdup($1);
		AddString(strTable, $1);
		$$->ival = LookupString(strTable, $1);
		free($1);
	}
	| IDENT_BYTEP {
		$$ = newTreeNode();
		$$->type = TN_PTR_IDENT;
		$$->sval = strdup($1);
		$$->ival = LookupSymbol(symStack, $1)->offset;
		free($1);
	}
	| IDENT_WORDP {
		$$ = newTreeNode();
		$$->type = TN_PTR_IDENT;
		$$->sval = strdup($1);
		$$->ival = LookupSymbol(symStack, $1)->offset;
		free($1);
	}
	| ptr_function_call
	| LNULL {
		$$ = newTreeNode();
		$$->type = TN_NULL;
	}
;
			   
int_function_call:
	FIDENT_BYTE '(' ')'	{
		$$ = newTreeNode();
		$$->type = TN_FUNCTIONCALL;
		$$->sval = strdup($1);
		
		function* f = LookupFunction(fTable, $1);
		f->called = TRUE;
		
		free($1);
	}
	| FIDENT_BYTE '(' arg_list ')' {
		$$ = newTreeNode();
		$$->type = TN_FUNCTIONCALL;
		$$->sval = strdup($1);
		$$->numOperands = 1;
		$$->operands[0] = $3;
		
		function* f = LookupFunction(fTable, $1);
		f->called = TRUE;
		
		free($1);
	}
	| FIDENT_WORD '(' ')' {
		$$ = newTreeNode();
		$$->type = TN_FUNCTIONCALL;
		$$->sval = strdup($1);
		
		function* f = LookupFunction(fTable, $1);
		f->called = TRUE;
		
		free($1);
	}
	| FIDENT_WORD '(' arg_list ')' {
		$$ = newTreeNode();
		$$->type = TN_FUNCTIONCALL;
		$$->sval = strdup($1);
		$$->numOperands = 1;
		$$->operands[0] = $3;
		
		function* f = LookupFunction(fTable, $1);
		f->called = TRUE;

		free($1);
	}
;
	
ptr_function_call:
	FIDENT_BYTEP '(' ')' {
		$$ = newTreeNode();
		$$->type = TN_FUNCTIONCALL;
		$$->sval = strdup($1);
		
		function* f = LookupFunction(fTable, $1);
		f->called = TRUE;
		
		free($1);
	}
	| FIDENT_BYTEP '(' arg_list ')'	{
		$$ = newTreeNode();
		$$->type = TN_FUNCTIONCALL;
		$$->sval = strdup($1);
		$$->numOperands = 1;
		$$->operands[0] = $3;
		
		function* f = LookupFunction(fTable, $1);
		f->called = TRUE;

		free($1);
	}
	| FIDENT_WORDP '(' ')' {
		$$ = newTreeNode();
		$$->type = TN_FUNCTIONCALL;
		$$->sval = strdup($1);
		
		function* f = LookupFunction(fTable, $1);
		f->called = TRUE;
		
		free($1);
	}
	| FIDENT_WORDP '(' arg_list ')'	{
		$$ = newTreeNode();
		$$->type = TN_FUNCTIONCALL;
		$$->sval = strdup($1);
		$$->numOperands = 1;
		$$->operands[0] = $3;
		
		function* f = LookupFunction(fTable, $1);
		f->called = TRUE;

		free($1);
	}
;

void_function_call:
	FIDENT_VOID '(' ')'	{
		$$ = newTreeNode();
		$$->type = TN_FUNCTIONCALL;
		$$->sval = strdup($1);
		
		function* f = LookupFunction(fTable, $1);
		f->called = TRUE;
			
		free($1);
	}
	| FIDENT_VOID '(' arg_list ')' {
		$$ = newTreeNode();
		$$->type = TN_FUNCTIONCALL;
		$$->sval = strdup($1);
		$$->numOperands = 1;
		$$->operands[0] = $3;
		
		function* f = LookupFunction(fTable, $1);
		f->called = TRUE;

		free($1);
	}
;
					
undec_function_call:
	IDENT_UNDEC '(' ')' {
		$$ = newTreeNode();
		char err[500];
		sprintf(err, "function '%s' undeclared", $1);
		yyerror(err);
		free($1);
	}
	| IDENT_UNDEC '(' arg_list ')' {
		$$ = newTreeNode();
		char err[500];
		sprintf(err, "function '%s' undeclared", $1);
		yyerror(err);
		free($1);
	}
;
					

arg_list:
	int_exp	{
		$$ = newTreeNode();
		$$->type = TN_ARGLIST;
		$$->numOperands = 1;
		$$->operands[0] = $1;
	}
	| bool_exp {
		$$ = newTreeNode();
		$$->type = TN_ARGLIST;
		$$->numOperands = 1;
		$$->operands[0] = $1;
	}
	| ptr_exp {
		$$ = newTreeNode();
		$$->type = TN_ARGLIST;
		$$->numOperands = 1;
		$$->operands[0] = $1;
	}
	| arg_list ',' arg_list	{
		$$ = $1;
		struct tree_node* op = $3->operands[0];
		$$->operands[$$->numOperands++] = op;
		free($3->operands);
		free($3);
	}
;
		 
return_statement:
	RETURN int_exp ';' {
		$$ = newTreeNode();
		$$->type = TN_RET_INT;
		$$->numOperands = 1;
		$$->operands[0] = $2;
	}
	| RETURN bool_exp ';' {
		$$ = newTreeNode();
		$$->type = TN_RET_BOOL;
		$$->numOperands = 1;
		$$->operands[0] = $2;
	}
	| RETURN ptr_exp ';' {
		$$ = newTreeNode();
		$$->type = TN_RET_PTR;
		$$->numOperands = 1;
		$$->operands[0] = $2;
	}
;

expression:
	int_exp
	| bool_exp
	| ptr_exp
;
			
int_exp:
	int_op
	| '-' int_exp {
		$$ = newTreeNode();
		$$->type = TN_UMINUS;
		$$->numOperands = 1;
		$$->operands[0] = $2;
	}
	| int_exp '+' int_exp {
			$$ = newTreeNode();
			$$->type = TN_IADD;
			$$->numOperands = 2;
			$$->operands[0] = $1;
			$$->operands[1] = $3;
	}
	| int_exp '-' int_exp {
			$$ = newTreeNode();
			$$->type = TN_ISUB;
			$$->numOperands = 2;
			$$->operands[0] = $1;
			$$->operands[1] = $3;
	}
	| int_exp '*' int_exp {
			$$ = newTreeNode();
			$$->type = TN_IMUL;
			$$->numOperands = 2;
			$$->operands[0] = $1;
			$$->operands[1] = $3;
	}
	| int_exp '/' int_exp	{
			$$ = newTreeNode();
			$$->type = TN_IDIV;
			$$->numOperands = 2;
			$$->operands[0] = $1;
			$$->operands[1] = $3;

			// Check for divide by zero
			if ($3->type == TN_INTEGER && $3->ival == 0)
				yyerror("division by zero");
	}
	| int_exp '%' int_exp {
		$$ = newTreeNode();
		$$->type = TN_IMOD;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;

		// Check for division by zero
		if ($3->type == TN_INTEGER && $3->ival == 0)
			yyerror("division by zero");
	}
	| '(' int_exp ')' { $$ = $2; }
;
			
bool_exp:
	bool_op
	| '!' bool_exp {
		$$ = newTreeNode();
		$$->type = TN_UBNEQ;
		$$->numOperands = 1;
		$$->operands[0] = $2;
	}
	| int_exp EQ int_exp {
		$$ = newTreeNode();
		$$->type = TN_IEQ;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
	| int_exp NEQ int_exp {
		$$ = newTreeNode();
		$$->type = TN_INEQ;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
	| int_exp GT int_exp {
		$$ = newTreeNode();
		$$->type = TN_IGT;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
	| int_exp LT int_exp {
		$$ = newTreeNode();
		$$->type = TN_ILT;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
	| int_exp GTE int_exp {
		$$ = newTreeNode();
		$$->type = TN_IGTE;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
	| int_exp LTE int_exp {
		$$ = newTreeNode();
		$$->type = TN_ILTE;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
	| bool_exp EQ bool_exp {
		$$ = newTreeNode();
		$$->type = TN_BEQ;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
	| bool_exp NEQ bool_exp	{
		$$ = newTreeNode();
		$$->type = TN_BNEQ;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
	| ptr_exp EQ ptr_exp {
		$$ = newTreeNode();
		$$->type = TN_PEQ;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
	| ptr_exp NEQ ptr_exp {
		$$ = newTreeNode();
		$$->type = TN_PNEQ;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
	| '(' bool_exp ')' {
		$$ = $2;
	}
;
			
ptr_exp:
	ptr_op
	| int_exp ':' int_exp {
		$$ = newTreeNode();
		$$->type = TN_REF;
		$$->numOperands = 2;
		$$->operands[0] = $1;
		$$->operands[1] = $3;
	}
;

parameter:
	vtype IDENT_UNDEC {
		$$ = newTreeNode();
		$$->type = TN_PARAM;
		$$->sval = strdup($2);
		$$->ival = $1;
		free($2);
	}
;

parameter_list:
	parameter {
		// Create parameter list node
		$$ = newTreeNode();
		$$->type = TN_PARAMLIST;
		$$->numOperands = 1;
		$$->operands[0] = $1;
		
		// Add parameter to symbol table
		AddSymbol(symStack, $1->sval, $1->ival);
		
		// Update argument stack size
		if ($1->ival == IT_BYTEP || $1->ival == IT_WORDP)
			$$->ival = 4;
		else
			$$->ival = 2;
	}
	| parameter_list ',' parameter {
		// Add parameter to list
		$$ = $1;
		$$->operands[$$->numOperands++] = $3;

		// Add parameter to symbol table
		AddSymbol(symStack, $3->sval, $3->ival);
		
		// Update argument stack size
		if ($3->ival == IT_BYTEP || $3->ival == IT_WORDP)
			$$->ival += 4;
		else
			$$->ival += 2;
	}
;

ftype:
	VOID {
		$$ = IT_VOID;
	}
	| vtype
;

vtype:
	BYTE {
		$$ = IT_BYTE;
	}
	| WORD {
		$$ = IT_WORD;
	}
	| BYTEP {
		$$ = IT_BYTEP;
	}
	| WORDP {
		$$ = IT_WORDP;
	}
;

%%

extern int errCount;
extern int currentLine;
extern char* currentFile;
extern FILE *uin;
extern FILE *uprein;
int main(int argc, char** argv)
{
	char* outPath;
	
	if (argc > 1)
	{
		currentFile = strdup(argv[1]);
		outPath = outputPath(currentFile);
		uprein = fopen(currentFile, "r");

		if (uprein == NULL)
		{
			printf("failed to open input file: %s\n", currentFile);
			free(currentFile);
			return 1;
		}
	} else {
		printf("U compiler v%s\n", VERSION);
		printf("usage: u [input file] [flags]\n");
		printf("flags:\n");
		printf("  -p               Print the parse tree.\n");
		printf("  -f               Print the function table.\n");
		printf("  -org [value]     Prepends 'org' statement to assembly output.\n");
		return 0;
	}
	
	// Set flags
	int i;
	int printParseTreeF = FALSE;
	int printFunctionTableF = FALSE;
	char* org = "";
	for (i = 2; i < argc; i++)
	{
		if (streq(argv[i], "-p"))
		{
			printParseTreeF = TRUE;
		} else if (streq(argv[i], "-f")) {
			printFunctionTableF = TRUE;
		} else if (streq(argv[i], "-org")) {
			if (++i >= argc)
			{
				printf("memory placement value expected after '-org' flag\n");
				return 1;
			} else {
				int valid = TRUE;
				int j;
				int len = strlen(argv[i]);
				char* tmp = argv[i];
				for (j = 0; j < len - 1; j++)
				{
					if (tmp[j] < '0' || tmp[j] > '9')
					{
						valid = FALSE;
						break;
					}
				}
				if ((tmp[j] < '0' || tmp[j] > '9') && !(tmp[j] == 'h' || tmp[j] == 'H'))
					valid = FALSE;
				if (!valid)
				{
					printf("invalid memory placement value\n");
					return 1;
				}
				org = tmp;
			}
		}
	}
	
	// Create core data structures
	symStack = CreateSymbolStack();
	fTable = CreateFunctionTable();
	strTable = CreateStringTable();
	fileQueue = CreateStringQueue();
	pFileQueue = CreateStringQueue();

	// Parse input
    upreparse();

    if (errCount == 0)
    {
    	currentFile = strdup(argv[1]);
    	currentLine = 1;
    	uin = fopen(currentFile, "r");
    	uparse();
    	
    	// Mark main() function as called
    	function* m = LookupFunction(fTable, "main");
    	m->called = TRUE;
    	
    	// Prune unused functions from parse tree
    	pruneUnusedFunctions(fTable, treeRoot);
	}

	if (errCount == 0)
	{
		FoldConstants(treeRoot, NULL, 0);

		if (printParseTreeF == TRUE)
		{
			printf("PARSE TREE:\n");
			PrintParseTree(treeRoot, 0);
			printf("\n");
		}
		
		if (printFunctionTableF == TRUE)
		{
			PrintFunctionTable(fTable);
			printf("\n");
		}
		EmitFasm(outPath, treeRoot, org);
		free(outPath);
	}

	// Free memory and return
	FreeStringTable(strTable);
	FreeSymbolStack(symStack);
	FreeFunctionTable(fTable);
	FreeStringQueue(fileQueue);
	FreeStringQueue(pFileQueue);
	FreeTree(treeRoot);
    return 0;
}

/* Error function. */
int yyerror(char* ErrMessage)
{
    printf("%s on line %d of '%s'\n", ErrMessage, currentLine, currentFile);
    errCount++;
	return 0;
}

