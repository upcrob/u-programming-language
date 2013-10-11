// Includes
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "parsetree.h"
#include "compiler.h"
#include "strutil.h"
#include "functiontable.h"
#include "stringtable.h"
#include "optimizer.h"

// Global variables
FILE* fp;
extern function_table* fTable;
extern string_table* strTable;

// Defines
#define CALLOFFSET 4

// Append string literals
void AppendStringLiterals()
{
	int i;
	for (i = 0; i < strTable->size; i++)
	{
		//fprintf(fp, "strlit_%d db '%s', 0\n", i, strTable->strings[i]);
		int prevChar = FALSE;
		int j;
		char* str = strTable->strings[i];
		int len = strlen(str);
		fprintf(fp, "strlit_%d db ", i);
		for (j = 0; j < len; j++)
		{
			if (str[j] == 10)
			{
				if (prevChar)
					fprintf(fp, "', ");
				
				fprintf(fp, "10");
				if (j != len - 1)
					fprintf(fp, ", ");
				prevChar = FALSE;
			} else {
				if (!prevChar)
					fprintf(fp, "'");
				fprintf(fp, "%c", str[j]);
				if (j == len - 1)
					fprintf(fp, "'");
				prevChar = TRUE;
			}
		}
		fprintf(fp, ", 0\n");
	}
}

// Recursive helper function for NASM Emit()
void EmitHelper(struct tree_node* node)
{
	if (node->type == TN_FUNCTION)
	{
		// Function root
		EmitHelper(node->operands[0]);
		if (node->numOperands > 1)
		{
			EmitHelper(node->operands[1]);
		}
		fprintf(fp, "pop bp\n");
		fprintf(fp, "ret\n");
	} else if (node->type == TN_FDEF) {
		// Function definition
		fprintf(fp, "\n_%s:\n", node->sval);
		fprintf(fp, "push bp\nmov bp, sp\n");
	} else if (node->type == TN_FUNCTIONCALL) {
		// Function call
		// Reserve memory on stack
		function* f = LookupFunction(fTable, node->sval);	// function struct
		
		// Reserve stack space for local variables
		if (f->frameSize - f->paramSize > 0)
			fprintf(fp, "sub sp, %d\n", f->frameSize - f->paramSize);
		
		// "Push" arguments to stack
		if (node->operands[0] != NULL)
			EmitHelper(node->operands[0]);

		// Call function
		fprintf(fp, "call _%s\n", node->sval);
		if (f->frameSize > 0)
			fprintf(fp, "add sp, %d\n", f->frameSize);	// restore stack memory
		
		// Push return value
		if (f->type != IT_VOID)
		{
			if (f->type == IT_BYTEP || f->type == IT_WORDP)
			{
				fprintf(fp, "push bx\npush ax\n");
			} else {
				fprintf(fp, "push ax\n");
			}
		}
	} else if (node->type == TN_ARGLIST) {
		// Parameter list
		int i;
		
		// Push arguments onto stack in reverse order
		int argSize = 0;
		for (i = node->numOperands - 1; i >= 0; i--)
		{
			EmitHelper(node->operands[i]);
			
			node_type t = node->operands[i]->type;
			if (t == TN_PTR_IDENT || t == TN_REF || t == TN_STRING_LITERAL)
			{
				// Reverse segment/offset order for pointers
				fprintf(fp, "pop ax\npop bx\npush ax\npush bx\n");
			}
		}
	} else if (node->type == TN_RET_INT) {
		// Return int value
		EmitHelper(node->operands[0]);
		fprintf(fp, "pop ax\npop bp\nret\n");
	} else if (node->type == TN_RET_BOOL) {
		// Return bool value
		EmitHelper(node->operands[0]);
		fprintf(fp, "pop ax\npop bp\nret\n");
	} else if (node->type == TN_RET_PTR) {
		// Return pointer value
		EmitHelper(node->operands[0]);
		fprintf(fp, "pop ax\npop bx\npop bp\nret\n");
	} else if (node->type == TN_BYTE_ASSIGN) {
		// Byte variable assignment
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop ax\nmov [bp+%d], al\t; %s\n", node->operands[0]->ival + CALLOFFSET, node->operands[0]->sval);
	} else if (node->type == TN_WORD_ASSIGN) {
		// Word variable assignment
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop ax\nmov [bp+%d], ax\t; %s\n", node->operands[0]->ival + CALLOFFSET, node->operands[0]->sval);
	} else if (node->type == TN_BOOL_ASSIGN) {
		// Boolean variable assignment
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop ax\nmov [bp+%d], al\t; %s\n", node->operands[0]->ival + CALLOFFSET, node->operands[0]->sval);
	} else if (node->type == TN_PTR_ASSIGN) {
		// Pointer variable assignment
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop ax\nmov [bp+%d], ax\t; %s (offset)\n", node->operands[0]->ival + CALLOFFSET + 2, node->operands[0]->sval);
		fprintf(fp, "pop ax\nmov [bp+%d], ax\t; %s (segment)\n", node->operands[0]->ival + CALLOFFSET, node->operands[0]->sval);
	} else if (node->type == TN_PTR_BYTE_ASSIGN) {
		// Byte pointer element assignment
		EmitHelper(node->operands[1]);	// Value
		EmitHelper(node->operands[0]);	// Index
		fprintf(fp, "mov es, [bp+%d]\n", node->ival + CALLOFFSET);
		fprintf(fp, "mov si, [bp+%d]\n", node->ival + CALLOFFSET + 2);
		fprintf(fp, "pop ax\nadd si, ax\n");
		fprintf(fp, "pop ax\n");
		fprintf(fp, "mov [es:si], al\n");
	} else if (node->type == TN_PTR_WORD_ASSIGN) {
		// Word pointer element assignment
		EmitHelper(node->operands[1]);	// Value
		EmitHelper(node->operands[0]);	// Index
		fprintf(fp, "mov es, [bp+%d]\n", node->ival + CALLOFFSET);
		fprintf(fp, "mov si, [bp+%d]\n", node->ival + CALLOFFSET + 2);
		fprintf(fp, "pop ax\nadd si, ax\n");
		fprintf(fp, "pop ax\n");
		fprintf(fp, "mov [es:si], ax\n");
	} else if (node->type == TN_BYTE_IDENT) {
		// Byte identifier
		fprintf(fp, "mov ax, [bp+%d]\nxor ah, ah\npush ax\t; %s\n", node->ival + CALLOFFSET, node->sval);
	} else if (node->type == TN_WORD_IDENT) {
		// Word identifier
		fprintf(fp, "mov ax, [bp+%d]\npush ax\t; %s\n", node->ival + CALLOFFSET, node->sval);
	} else if (node->type == TN_BOOL_IDENT) {
		// Boolean identifier
		fprintf(fp, "mov al, [bp+%d]\npush ax\t; %s\n", node->ival + CALLOFFSET, node->sval);
	} else if (node->type == TN_PTR_IDENT) {
		// Pointer identifier
		fprintf(fp, "mov ax, [bp+%d]\npush ax\t; %s (segment)\nmov ax, [bp+%d]\npush ax\t; %s (offset)\n", node->ival + CALLOFFSET, node->sval, node->ival + CALLOFFSET + 2, node->sval);
	} else if (node->type == TN_INTEGER) {
		// Integer constant
		fprintf(fp, "push %d\n", node->ival);
	} else if (node->type == TN_CHAR) {
		// Character constant
		fprintf(fp, "push '%c'\n", node->sval[0]);
	} else if (node->type == TN_PTR_BYTE) {
		// Get byte value in an array
		EmitHelper(node->operands[0]);
		fprintf(fp, "mov es, [bp+%d]\n", node->ival + CALLOFFSET);
		fprintf(fp, "mov si, [bp+%d]\n", node->ival + CALLOFFSET + 2);
		fprintf(fp, "pop ax\nadd si, ax\n");
		fprintf(fp, "xor bh, bh\n");
		fprintf(fp, "mov bl, [es:si]\n");
		fprintf(fp, "push bx\n");
	} else if (node->type == TN_PTR_WORD) {
		// Get word value in an array
		EmitHelper(node->operands[0]);
		fprintf(fp, "mov es, [bp+%d]\n", node->ival + CALLOFFSET);
		fprintf(fp, "mov si, [bp+%d]\n", node->ival + CALLOFFSET + 2);
		fprintf(fp, "pop ax\nadd si, ax\n");
		fprintf(fp, "mov bx, [es:si]\n");
		fprintf(fp, "push bx\n");
	} else if (node->type == TN_TRUE) {
		// Logical true
		fprintf(fp, "push 1\n");
	} else if (node->type == TN_FALSE) {
		fprintf(fp, "push 0\n");
	} else if (node->type == TN_WHILE) {
		// While statement
		fprintf(fp, "while_%d:\n", node->id);
		EmitHelper(node->operands[0]);
		fprintf(fp, "pop ax\n");
		fprintf(fp, "cmp al, 0\n");
		fprintf(fp, "jne begin_while_%d\n", node->id);
		fprintf(fp, "jmp end_while_%d\n", node->id);
		fprintf(fp, "begin_while_%d:\n", node->id);
		EmitHelper(node->operands[1]);
		fprintf(fp, "jmp while_%d\n", node->id);
		fprintf(fp, "end_while_%d:\n", node->id);
	} else if (node->type == TN_IF) {
		// If statement
		EmitHelper(node->operands[0]);
		fprintf(fp, "pop ax\n");
		fprintf(fp, "cmp al, 1\n");
		fprintf(fp, "je begin_if_%d\n", node->id);
		fprintf(fp, "jmp else_%d\n", node->id);
		fprintf(fp, "begin_if_%d:\n", node->id);
		EmitHelper(node->operands[1]);
		fprintf(fp, "jmp endif_%d\nelse_%d:\n", node->id, node->id);
		if (node->operands[2] != NULL)
			EmitHelper(node->operands[2]);
		fprintf(fp, "endif_%d:\n", node->id);
	} else if (node->type == TN_ASM) {
		// Assembly code
		EmitHelper(node->operands[0]);
	} else if (node->type == TN_AMOV) {
		// mov instruction
		struct tree_node* op1 = node->operands[0];
		struct tree_node* op2 = node->operands[1];
		
		if (op1->type == TN_ASMREG)
			fprintf(fp, "mov %s, ", regStr(op1->ival));
		else if (op1->type == TN_ASMLOC)
			fprintf(fp, "mov [bp+%d], ", op1->operands[0]->ival + CALLOFFSET);

		if (op2->type == TN_INTEGER)
			fprintf(fp, "%d\n", op2->ival);
		else if (op2->type == TN_ASMREG)
			fprintf(fp, "%s\n", regStr(op2->ival));
		else if (op2->type == TN_ASMLOC)
			fprintf(fp, "[bp+%d]\n", op2->operands[0]->ival + CALLOFFSET);
		else if (op2->type == TN_CHAR)
			fprintf(fp, "'%s'\n", op2->sval);
	} else if (node->type == TN_AINT) {
		// int instruction
		fprintf(fp, "int %d\n", node->operands[0]->ival);
	} else if (node->type == TN_ACALL) {
		// call instruction
		fprintf(fp, "call %d\n", node->operands[0]->ival);
	} else if (node->type == TN_IADD) {
		// Integer addition
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nadd ax, bx\npush ax\n");
	} else if (node->type == TN_ISUB) {
		// Integer subtraction
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nsub ax, bx\npush ax\n");
	} else if (node->type == TN_UMINUS) {
		// Unary minus
		EmitHelper(node->operands[0]);
		fprintf(fp, "pop ax\nmov bx, 0\nsub bx, ax\npush bx\n");
	} else if (node->type == TN_IMUL) {
		// Integer multiplication
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nimul ax, bx\npush ax\n");
	} else if (node->type == TN_IDIV) {
		// Integer division
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nxor dx, dx\nidiv bx\npush ax\n");
	} else if (node->type == TN_IMOD) {
		// Integer modulus
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nxor dx, dx\nidiv bx\npush dx\n");
	} else if (node->type == TN_IEQ || node->type == TN_BEQ) {
		// Integer equality
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nmov dx, 1\ncmp ax, bx\nje eq_%d\nmov dx, 0\neq_%d:\npush dx\n", node->id, node->id);
	} else if (node->type == TN_INEQ || node->type == TN_BNEQ) {
		// Integer inequality
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nmov dx, 1\ncmp ax, bx\njne eq_%d\nmov dx, 0\neq_%d:\npush dx\n", node->id, node->id);
	} else if (node->type == TN_IGT) {
		// Integer greater than
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nmov dx, 1\ncmp ax, bx\njg eq_%d\nmov dx, 0\neq_%d:\npush dx\n", node->id, node->id);
	} else if (node->type == TN_ILT) {
		// Integer less than
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nmov dx, 1\ncmp ax, bx\njl eq_%d\nmov dx, 0\neq_%d:\npush dx\n", node->id, node->id);
	} else if (node->type == TN_IGTE) {
		// Integer greater than or equal to
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nmov dx, 1\ncmp ax, bx\njge eq_%d\nmov dx, 0\neq_%d:\npush dx\n", node->id, node->id);
	} else if (node->type == TN_ILTE) {
		// Integer less than or equal to
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop bx\npop ax\nmov dx, 1\ncmp ax, bx\njle eq_%d\nmov dx, 0\neq_%d:\npush dx\n", node->id, node->id);
	} else if (node->type == TN_PEQ) {
		// Pointer equality
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop dx\npop cx\npop bx\npop ax\ncmp ax, cx\njne eq_%d\ncmp bx, dx\njne eq_%d\npush 1\njmp eeq_%d\neq_%d:\npush 0\neeq_%d:\n",
			node->id, node->id, node->id, node->id, node->id);
	} else if (node->type == TN_PNEQ) {
		// Pointer inequality
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
		fprintf(fp, "pop dx\npop cx\npop bx\npop ax\ncmp ax, cx\njne eq_%d\ncmp bx, dx\njne eq_%d\npush 1\njmp eeq_%d\neq_%d:\npush 0\neeq_%d:\n",
			node->id, node->id, node->id, node->id, node->id);
	} else if (node->type == TN_UBNEQ) {
		// Unary boolean inequality
		EmitHelper(node->operands[0]);
		fprintf(fp, "mov dx, 1\npop ax\ncmp ax, 0\nje eq_%d\nmov dx, 0\neq_%d:\npush dx\n", node->id, node->id);
	} else if (node->type == TN_STRING_LITERAL) {
		// Get pointer to string
		fprintf(fp, "push cs\nlea ax, [strlit_%d]\npush ax\n", node->ival);
	} else if (node->type == TN_NULL) {
		// Push null reference onto stack
		fprintf(fp, "push -1\npush 0\n");
	} else if (node->type == TN_REF) {
		// Memory reference (':' operator)
		EmitHelper(node->operands[0]);
		EmitHelper(node->operands[1]);
	} else {
		// Unknown node
		if (node->sval != NULL)
			fprintf(fp, "UNKNOWN NODE: %s\n", node->sval);
		else
			fprintf(fp, "UNKNOWN NODE\n");
	}

	// Call next statement if it exists
	if (node->pNextStatement != NULL)
		EmitHelper(node->pNextStatement);
}

// Emit assembly code compatible with the FASM assembler
void EmitFasm(char* filename, struct tree_node* root, char* org)
{
	// Open file
	fp = fopen(filename, "w+");
	
	// Add org statement if memory placement specified
	if (!streq(org, ""))
	{
		fprintf(fp, "org %s\n", org);
	}

	// Call main
	function* f = LookupFunction(fTable, "main");	// function struct

	// Call function
	if (f->frameSize - f->paramSize > 0)
		fprintf(fp, "sub sp, %d\n", f->frameSize - f->paramSize);	// reserve local variable stack space
	fprintf(fp, "call _main\n");
	if (f->frameSize > 0)
		fprintf(fp, "add sp, %d\n", f->frameSize);	// restore stack memory
	fprintf(fp, "ret\n");
	
	// Write the rest of the program code
	EmitHelper(root);
	fprintf(fp, "\n");

	// Add string literal code
	AppendStringLiterals();

	// Close file
	fclose(fp);
}

// Emit assembly code compatible with the NASM assembler
void EmitNasm(char* filename, struct tree_node* root)
{
	// Open file
	fp = fopen(filename, "w+");

	// Recurse and emit code
	fprintf(fp, "[BITS 16]\norg 100h\ncall main\nret\n");	// REMOVE ORG LATER
	EmitHelper(root);
	fprintf(fp, "\n");

	// Close file
	fclose(fp);
}


