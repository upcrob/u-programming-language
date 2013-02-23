/*
 * optimizer.c
 * Functions for optimizing the parse-tree.
 */

 #include <malloc.h>
 #include "parsetree.h"
 #include <stdio.h>

 struct tree_node* FoldConstants(struct tree_node* node, struct tree_node* parent, int parent_op)
 {
 	int i;
	for (i = 0; i < node->numOperands; i++)
		FoldConstants(node->operands[i], node, i);
	
	if (node->type == TN_IADD
		|| node->type == TN_ISUB
		|| node->type == TN_IMUL
		|| node->type == TN_IDIV
		|| node->type == TN_IMOD)
	{
		// This is a math operation node
		if (node->operands[0]->type == TN_INTEGER
			&& node->operands[1]->type == TN_INTEGER)
		{
			// This node has two constant children, fold
			struct tree_node* tmp = node;
			node = newTreeNode();
			parent->operands[parent_op] = node;
			node->type = TN_INTEGER;
			node->id = tmp->id;
			
			// Fold
			if (tmp->type == TN_IADD)
				node->ival = tmp->operands[0]->ival + tmp->operands[1]->ival;
			else if (tmp->type == TN_ISUB)
				node->ival = tmp->operands[0]->ival - tmp->operands[1]->ival;
			else if (tmp->type == TN_IMUL)	
				node->ival = tmp->operands[0]->ival * tmp->operands[1]->ival;
			else if (tmp->type == TN_IDIV)
				node->ival = tmp->operands[0]->ival / tmp->operands[1]->ival;
			else if (tmp->type == TN_IMOD)
				node->ival = tmp->operands[0]->ival % tmp->operands[1]->ival;

			// Free old subtree
			FreeTree(tmp);
		}
	}
	return node;
 }
