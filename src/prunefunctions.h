/*
 * Code for pruning unused functions from the parse tree.
 */

#include "functiontable.h"
#include "parsetree.h"

struct tree_node* pruneUnusedFunctions(function_table* fTable, struct tree_node* root);

