/*
*   Tree (Binary Search tree 'BST', Red-Black tree 'RBT')
*/

#ifndef __COMMON_TREE_H__
#define __COMMON_TREE_H__

#include <common/types.h>

typedef struct leaf {

} leaf_t;

typedef struct node {

} node_t;

typedef struct bst {

} bst_t,

typedef struct rbt {

} rbt_t;

void bst_init(bst_t* bst);
void bst_insert(bst_t* bst);
void bst_delete(bst_t* bst);

void rbt_init(rbt_t* rbt);
void rbt_insert(rbt_t* rbt);
void rbt_delete(rbt_t* rbt);

#endif // __COMMON_TREE_H__
