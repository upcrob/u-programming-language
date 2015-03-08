#include <assert.h>
#include <stdlib.h>
#include "parsetree.h"
#include <stdio.h>
#include "defines.h"
#include "identifiertypes.h"
#include "strutil.h"

// Node id counter
int nodeIdCounter = 1;

// Free entire parse tree, using this node as the root
void FreeTree(struct tree_node* pNode)
{
    // Recursively free any non-NULL operands
	if (pNode != NULL)
	{
		int i;
		FreeTree(pNode->pNextStatement);

		for (i = 0; i < pNode->numOperands; i++)
			FreeTree(pNode->operands[i]);
		free(pNode->operands);

		if (pNode->sval != NULL)
			free(pNode->sval);

		free(pNode);
	}
}

// Free this node and any below it, but not any successive nodes
void FreeNode(struct tree_node* pNode)
{
	if (pNode != NULL)
	{
		int i;
		for (i = 0; i < pNode->numOperands; i++)
			FreeTree(pNode->operands[i]);
		free(pNode->operands);
		
		if (pNode->sval != NULL)
			free(pNode->sval);
		free(pNode);
	}
}

struct tree_node* newTreeNode()
{
	struct tree_node* n = (struct tree_node*) malloc(sizeof(struct tree_node));
	n->type = TN_NOTYPE;
	n->id = nodeIdCounter++;
	n->numOperands = 0;
	n->ival = 0;
	n->sval = NULL;
	n->pNextStatement = NULL;
	n->operands = (struct tree_node**) malloc(sizeof(struct tree_node*) * MAX_OPERANDS);
	int i;
	for (i = 0; i < MAX_OPERANDS; i++)
		n->operands[i] = NULL;
	return n;
}

// Get type string
const char* identStr(int type)
{
	if (type == IT_VOID)
		return "VOID";
	else if (type == IT_BYTE)
		return "BYTE";
	else if (type == IT_WORD)
		return "WORD";
	else if (type == IT_BYTEP)
		return "BYTE[]";
	else if (type == IT_WORDP)
		return "WORD[]";
	else
		return "NOTYPE";
}

// Print the tree for debugging
void PrintParseTree(struct tree_node* pNode, int depth)
{
	int i;
	for (i = 0; i < depth; i++)
		printf("  ");
		
	printf("NODE (%d): ", pNode->id);
	if (pNode->type == TN_INTEGER)
		printf("INTEGER (%d)\n", pNode->ival);
	else if (pNode->type == TN_QSTRING)
		printf("QSTRING\n");
	else if (pNode->type == TN_CHAR)
		printf("CHAR ('%s')\n", pNode->sval);
	else if (pNode->type == TN_BYTE_IDENT)
		printf("BYTE IDENTIFIER (\"%s\"  offset: %d)\n", pNode->sval, pNode->ival);
	else if (pNode->type == TN_WORD_IDENT)
		printf("WORD IDENTIFIER (\"%s\"  offset: %d)\n", pNode->sval, pNode->ival);
	else if (pNode->type == TN_BOOL_IDENT)
		printf("BOOLEAN IDENTIFIER (\"%s\" offset: %d)\n", pNode->sval, pNode->ival);
	else if (pNode->type == TN_PTR_IDENT)
		printf("PTR IDENTIFIER (\"%s\" offset: %d)\n", pNode->sval, pNode->ival);
	else if (pNode->type == TN_PTR_BYTE_ASSIGN)
		printf("BYTE POINTER ELEMENT ASSIGNMENT (\"%s\")\n", pNode->sval);
	else if (pNode->type == TN_PTR_WORD_ASSIGN)
		printf("WORD POINTER ELEMENT ASSIGNMENT (\"%s\")\n", pNode->sval);
	else if (pNode->type == TN_PTR_BYTE)
		printf("BYTE PTR DEREFERENCE (\"%s\")\n", pNode->sval);
	else if (pNode->type == TN_PTR_WORD)
		printf("WORD PTR DEREFERENCE (\"%s\")\n", pNode->sval);
	else if (pNode->type == TN_STRING_LITERAL)
		printf("STRING LITERAL (\"%s\")\n", pNode->sval);
	else if (pNode->type == TN_BYTE_BLOCK)
		printf("BYTE BLOCK\n");
	else if (pNode->type == TN_WORD_BLOCK)
		printf("WORD BLOCK\n");
	else if (pNode->type == TN_FUNCTION)
		printf("FUNCTION\n");
	else if (pNode->type == TN_FUNCTIONCALL)
		printf("FUNCTION CALL (\"%s\")\n", pNode->sval);
	else if (pNode->type == TN_SEGCALL)
		printf("SEGMENT() CALL\n");
	else if (pNode->type == TN_OFFCALL)
		printf("OFFSET() CALL\n");
	else if (pNode->type == TN_BYTE_ASSIGN)
		printf("BYTE ASSIGN\n");
	else if (pNode->type == TN_WORD_ASSIGN)
		printf("WORD ASSIGN\n");
	else if (pNode->type == TN_BOOL_ASSIGN)
		printf("BOOL ASSIGN\n");
	else if (pNode->type == TN_PTR_ASSIGN)
		printf("PTR ASSIGN\n");
	else if (pNode->type == TN_RET_INT)
		printf("RETURN INT\n");
	else if (pNode->type == TN_RET_BOOL)
		printf("RETURN BOOL\n");
	else if (pNode->type == TN_RET_PTR)
		printf("RETURN PTR\n");
	else if (pNode->type == TN_WHILE)
		printf("WHILE\n");
	else if (pNode->type == TN_TRUE)
		printf("TRUE\n");
	else if (pNode->type == TN_FALSE)
		printf("FALSE\n");
	else if (pNode->type == TN_NULL)
		printf("NULL\n");
	else if (pNode->type == TN_IF)
		printf("IF\n");
	else if (pNode->type == TN_ARGLIST)
		printf("ARGUMENT LIST\n");
	else if (pNode->type == TN_PARAM)
		printf("PARAM (%s %s)\n", identStr(pNode->ival), pNode->sval);
	else if (pNode->type == TN_PARAMLIST)
		printf("PARAMLIST\n");
	else if (pNode->type == TN_FDEF)
		printf("FUNCTION DEFINITION (%s)\n", pNode->sval);
	else if (pNode->type == TN_ASM)
		printf("ASM\n");
	else if (pNode->type == TN_ASMLOC)
		printf("MEMLOC\n");
	else if (pNode->type == TN_ASMREG)
		printf("REGISTER (%s)\n", regStr(pNode->ival));
	else if (pNode->type == TN_AMOV)
		printf("MOV\n");
	else if (pNode->type == TN_ACALL)
		printf("CALL\n");
	else if (pNode->type == TN_AINT)
		printf("INT\n");
	else if (pNode->type == TN_IADD)
		printf("ADD\n");
	else if (pNode->type == TN_ISUB)
		printf("SUB\n");
	else if (pNode->type == TN_UMINUS)
		printf("UNARY MINUS\n");
	else if (pNode->type == TN_IMUL)
		printf("MUL\n");
	else if (pNode->type == TN_IDIV)
		printf("DIV\n");
	else if (pNode->type == TN_IMOD)
		printf("MOD\n");
	else if (pNode->type == TN_IEQ || pNode->type == TN_BEQ || pNode->type == TN_PEQ)
		printf("EQ\n");
	else if (pNode->type == TN_INEQ || pNode->type == TN_BNEQ || pNode->type == TN_PNEQ || pNode->type == TN_UBNEQ)
		printf("NEQ\n");
	else if (pNode->type == TN_ILT)
		printf("LT\n");
	else if (pNode->type == TN_IGT)
		printf("GT\n");
	else if (pNode->type == TN_ILTE)
		printf("LTE\n");
	else if (pNode->type == TN_IGTE)
		printf("GTE\n");
	else if (pNode->type == TN_REF)
		printf("MEMORY ADDRESS\n");
	else
		printf("NO TYPE\n");

	for (i = 0; i < pNode->numOperands; i++)
		if (pNode->operands[i] != NULL)
			PrintParseTree(pNode->operands[i], depth + 1);
		else
			printf("WARNING: UNEXPECTED NULL OPERAND!\n");
			
	if (pNode->pNextStatement != NULL)
		PrintParseTree(pNode->pNextStatement, depth);
}

// Tests
/*
int main()
{
	struct tree_node* a = newTreeNode();
	struct tree_node* b = newTreeNode();
	struct tree_node* c = newTreeNode();
	struct tree_node* d = newTreeNode();
	a->numOperands = 2;
	b->numOperands = 1;
	a->operands[0] = b;
	a->operands[1] = c;
	b->operands[0] = d;
	FreeTree(a);
	return 0;
}*/


