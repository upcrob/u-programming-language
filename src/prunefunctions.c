/*
 * Code for pruning unused functions from the parse tree.
 */

#include "prunefunctions.h"
#include "functiontable.h"
#include "parsetree.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>

struct tree_node* pruneUnusedFunctions(function_table* fTable, struct tree_node* root)
{
	struct tree_node* current = root;
	struct tree_node* prev = NULL;
	while (current != NULL)
	{
		if (current->type == TN_FUNCTION)
		{
			function* f = LookupFunction(fTable, current->operands[0]->sval);
			if (f->called == FALSE)
			{
				// function was not called in program, prune it from the parse tree
				if (prev == NULL)
				{
					// remove this node, next one the root
					root = current->pNextStatement;
					FreeNode(current);
					current = root;
				} else {
					// remove this node and skip over it
					prev->pNextStatement = current->pNextStatement;
					FreeNode(current);
					current = prev->pNextStatement;
				}
			} else {
				prev = current;
				current = current->pNextStatement;
			}
		} else {
			current = current->pNextStatement;
		}
	}
	
	return root;
}


