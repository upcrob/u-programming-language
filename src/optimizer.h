/*
 * optimizer.h
 * Functions for optimizing the parse-tree.
 */

 #include "parsetree.h"

 struct tree_node* FoldConstants(struct tree_node* root, struct tree_node* parent, int parent_op);
