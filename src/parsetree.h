//////////////////////////////////////////////////////////////////////////
// These are starter data types for parse tree nodes. You will need
// to add to them and modify their structure to suit your purposes.
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "identifiertypes.h"

// Maximum number of operands
#define MAX_OPERANDS 250

// These value in this enumerated type will rougly correspond to the tokens
// and rules in your grammar
typedef enum node_type_tag 
{
	TN_FUNCTION,
	TN_FUNCTIONCALL,
	TN_PARAM,
	TN_PARAMLIST,
	TN_FDEF,
	TN_ARGLIST,
	TN_BYTE_ASSIGN,
	TN_WORD_ASSIGN,
	TN_BOOL_ASSIGN,
	TN_PTR_ASSIGN,
	TN_PTR_BYTE_ASSIGN,
	TN_PTR_WORD_ASSIGN,
	TN_WHILE,
	TN_IF,
	TN_TRUE,
	TN_FALSE,
	TN_NULL,
	TN_BYTE_IDENT,
	TN_WORD_IDENT,
	TN_BOOL_IDENT,
	TN_PTR_IDENT,
	TN_STRING_LITERAL,
	TN_BYTE_BLOCK,
	TN_WORD_BLOCK,
	TN_PTR_BYTE,
	TN_PTR_WORD,
	TN_RET_INT,
	TN_RET_BOOL,
	TN_RET_PTR,
	TN_ARRELEM,
	TN_CHAR,
	TN_ASM,
	TN_ASMLOC,
	TN_ASMREG,
	TN_AMOV,
	TN_AINT,
	TN_ACALL,
	TN_IADD,
	TN_ISUB,
	TN_UMINUS,
	TN_IMUL,
	TN_IDIV,
	TN_IMOD,
	TN_IEQ,
	TN_INEQ,
	TN_ILT,
	TN_IGT,
	TN_ILTE,
	TN_IGTE,
	TN_BEQ,
	TN_BNEQ,
	TN_PEQ,
	TN_PNEQ,
	TN_UBNEQ,
	TN_REF,
    TN_QSTRING,
    TN_INTEGER,
    TN_NOTYPE
} node_type;

struct tree_node
{
    // The parser will assign this variable one of the enumerated type values
    // above according to the kind of node it was created to be.
    node_type type; 

    // This connects to the next statement in the program
    struct tree_node* pNextStatement;

	// Node id
	int id;

	// Number of operands used in this node
	int numOperands;

	// Integer value of this tree node, if needed
	int ival;

	// String value o fthis tree node, if needed
	char* sval;

	// Operand pointers
    struct tree_node** operands;
};

void FreeTree(struct tree_node* pNode);

void FreeNode(struct tree_node* pNode);

struct tree_node* newTreeNode();

// Print the tree for debugging
void PrintParseTree(struct tree_node* pNode, int depth);


