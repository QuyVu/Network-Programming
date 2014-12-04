/*
Libraries for fields, doubly-linked lists and red-black trees.
Copyright (C) 2001 James S. Plank

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

---------------------------------------------------------------------------
Please see http://www.cs.utk.edu/~plank/plank/classes/cs360/360/notes/Libfdr/
for instruction on how to use this library.

Jim Plank
plank@cs.utk.edu
http://www.cs.utk.edu/~plank

Associate Professor
Department of Computer Science
University of Tennessee
203 Claxton Complex
1122 Volunteer Blvd.
Knoxville, TN 37996-3450

     865-974-4397
Fax: 865-974-4404
 */
#ifndef	TREE_H
#define	TREE_H

#include "value.h"

/* Main tree_node.  You only ever use the fields
   flink
   blink
   k.key or k.ikey
   v.val
 */


typedef struct tree_node {
    unsigned char red;
    unsigned char internal;
    unsigned char left;
    unsigned char roothead; /* (bit 1 is root, bit 2 is head) */
    struct tree_node *flink;
    struct tree_node *blink;
    struct tree_node *parent;
    Value key;
    Value val;
} *Tree;


Tree tree_make(); /* Creates a new rb-tree */


/* Creates a node with key key and val val and inserts it into the tree.
   tree_insert uses strcmp() as comparison funcion.  tree_inserti uses <>=,
   tree_insertg uses func() */

Tree tree_insert_str(Tree tree, char *key, Value val);
Tree tree_insert_int(Tree tree, int ikey, Value val);
Tree tree_insert_dbl(Tree tree, double dkey, Value val);
Tree tree_insert_gen(Tree tree, Value key, Value val, int (*func)(Value, Value));

/* returns an external node in t whose value is equal k. Returns NULL if
   there is no such node in the tree */

Tree tree_find_str(Tree root, char *key);
Tree tree_find_int(Tree root, int ikey);
Tree tree_find_dbl(Tree root, double dkey);
Tree tree_find_gen(Tree root, Value, int (*func)(Value, Value));


/* returns an external node in t whose value is equal
  k or whose value is the smallest value greater than k. Sets found to
  1 if the key was found, and 0 otherwise.  */

Tree tree_find_gte_str(Tree root, char *key, int *found);
Tree tree_find_gte_int(Tree root, int ikey, int *found);
Tree tree_find_gte_dbl(Tree root, double dkey, int *found);
Tree tree_find_gte_gen(Tree root, Value key,
        int (*func)(Value, Value), int *found);


/* Creates a node with key key and val val and inserts it into the 
   tree before/after node nd.  Does not check to ensure that you are 
   keeping the correct order */

void tree_delete_node(Tree node); /* Deletes and frees a node (but 
                                              not the key or val) */
void tree_free(Tree root); /* Deletes and frees an entire tree */

Value tree_val(Tree node); /* Returns node->v.val -- this is to shut
                                       lint up */

int tree_nblack(Tree n); /* returns # of black nodes in path from
                                    n to the root */
int tree_plength(Tree n); /* returns the # of nodes in path from
				    n to the root */

#define TREE_FIRST(n) (n->flink)
#define TREE_LAST(n) (n->blink)
#define TREE_NEXT(n) (n->flink)
#define TREE_PREV(n) (n->blink)
#define TREE_EMPTY(t) (t->flink == t)
#ifndef TREE_NIL
#define TREE_NIL(t) (t)
#endif

#define TREE_TRAVERSE(ptr, lst) \
  for(ptr = TREE_FIRST(lst); ptr != TREE_NIL(lst); ptr = TREE_NEXT(ptr))

#define TREE_REVERSE(ptr, lst) \
  for(ptr = TREE_LAST(lst); ptr != TREE_NIL(lst); ptr = TREE_PREV(ptr))

#endif