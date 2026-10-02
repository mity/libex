/*
 * libex
 * <http://github.com/mity/libex>
 *
 * Copyright (c) 2020-2026 Martin Mitáš
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef EX_RBTREE_H
#define EX_RBTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>


#if defined offsetof
    #define RBTREE_OFFSETOF__(type, member)     offsetof(type, member)
#elif defined __GNUC__ && __GNUC__ >= 4
    #define RBTREE_OFFSETOF__(type, member)     __builtin_offsetof(type, member)
#else
    #define RBTREE_OFFSETOF__(type, member)     ((size_t) &((type*)0)->member)
#endif


/* Intrusive red-black tree implementation.
 *
 * See e.g. https://en.wikipedia.org/wiki/Red-black_tree if you are unfamiliar
 * with the concept of red-black tree.
 *
 * To manipulate or query the tree, the tree as a whole is represented by the
 * RBTree structure and its nodes are represented by the RBTreeNode structure.
 *
 * The word "intrusive" in the title means our RBTreeNode structure does not
 * hold any payload data on its own. Instead, you are supposed to embed the
 * node in your own data structure.
 *
 * Also note we do not distinguish any "key" from "data". The caller has to
 * provide his own comparator function for defining the order of the data in
 * the tree and it's up to application to decide which data serve as the "key",
 * i.e. are used for the ordering.
 *
 * This approach has some consequences:
 *
 * - To get a pointer to the enclosing application specific data structure, 
 *   use the macro RBTREE_DATA (which is just the the typical "container_of()"
 *   macro implementation.
 *
 * - For some operations like e.g. lookup, the caller has to provide a pointer
 *   to the same dummy structure, initialized enough to serve as the "key" so
 *   the comparator function may do its job.
 *
 * - It also means our implementation actually never allocates/frees any memory
 *   on the heap. All the tree operations like e.g. "insert" or "remove" just
 *   update the pointers in the node structure (and those in other nodes of
 *   the tree).
 *
 *   Caller has to allocate and initialize the data structure (at least to the
 *   degree needed by the comparator function) the enclosing data structure
 *   _before_ he inserts it into the tree. Similarly, caller is responsible for
 *   freeing any resources the payload structure holds _after_ it is removed
 *   from the tree.
 *
 * - As long as the node is part of a tree, it must not be modified in any way
 *   which would make the comparator function order it differently with respect
 *   to other nodes in the tree.
 *
 * Note of warning: The RBTreeNode stores its color in the least significant
 * bit of the RBTreeNode::lc pointer. This means that all the RBTreeNode
 * instances must be reasonably aligned.
 */


/* Tree node structure. Treat as opaque.
 */
typedef struct RBTreeNode {
    struct RBTreeNode* lc; /* combination of left ptr and color bit */
    struct RBTreeNode* r;  /* right ptr */
} RBTreeNode;


/* Comparator function type.
 *
 * The comparator function defines the order of the data stored in the tree.
 *
 * It has to return:
 *   - negative value if the 1st argument is lower then the 2nd one;
 *   - positive value if the 1st argument is greater then the 2nd one;
 *   - zero if they are equal.
 */
typedef int (*RBTreeCmpFunc)(const RBTreeNode*, const RBTreeNode*);


/* Tree structure. Treat as opaque.
 */
typedef struct RBTree {
    RBTreeCmpFunc cmp_func;
    RBTreeNode* root;
} RBTree;


/* Macro for getting pointer to the structure holding the rbtree node data.
 *
 * (If you use the RBTreeNode as the first member of your structure, you
 * can use a simple casting instead.)
 */
#define RBTREE_DATA(node_ptr, type, member)     \
                ((type*)((char*)(node_ptr) - RBTREE_OFFSETOF__(type, member)))


/* The tree has to be initialized before it is used by any other function.
 */
static inline void rbtree_init(RBTree* tree, RBTreeCmpFunc cmp_func)
        { tree->cmp_func = cmp_func; tree->root = NULL; }

#define RBTREE_INITIALIZER(cmp_func)    { cmp_func, NULL }


/* Cleaning a (non-empty) tree can be a more complex operation. Usually, caller
 * needs to release some resources associated with each node (e.g. to free the
 * data structure).
 *
 * We provide this specialized function rbtree_fini_step() for traversing all
 * the nodes for the purpose of cleaning all the nodes in tree.
 *
 * ```
 * while(1) {
 *     RBTreeNode* node = rbtree_fini_step(tree);
 *     if(node == NULL)
 *         break;
 *
 *     // Release the per-node resources:
 *     free(RBTREE_DATA(node, MyStruct, the_node_member_name));
 * }
 *
 * ```
 *
 * Note that once the operation starts, the tree must not be used anymore for
 * anything else until the cleaning of the tree is complete. Once this function
 * is called, the tree's internal state gets broken for any other purpose.
 * After the whole clean-up operation is complete, you get a valid (empty) tree.
 *
 * The function is actually just a specialized light-weight iteration over all
 * tree nodes, disconnecting them one by one out from the tree, without any
 * tree re-balancing.
 *
 * Note it does not release any resources on its own. (Naturally, as our
 * implementation does not allocate any at the first place. We only connect
 * the nodes together or, as here, disconnect them from it).
 *
 * I.e., if you are able kill all the nodes more effectively by any other means
 * without iterating over them (for example because all the nodes live in a
 * single memory buffer, which can be freed at once) then you do not need to
 * use this function at all: Instead, simply free the buffer and re-initialize
 * the tree handle for reuse if needed.
 *
 * (Compatibility note: You should not rely on any particular order of the
 * nodes when using this function. If we find more efficient algorithm for the
 * given purpose, future versions may traverse the nodes differently.)
 */
RBTreeNode* rbtree_fini_step(RBTree* tree);


/* Check whether the tree is empty. Returns non-zero if empty, zero otherwise.
 */
static inline int rbtree_is_empty(const RBTree* tree)
        { return (tree->root == NULL); }


/* Insert a new node into the tree.
 *
 * Returns 0 on success or -1 on failure (which may happen only if an equal
 * node is already present in the tree).
 */
int rbtree_insert(RBTree* tree, RBTreeNode* node);

/* Construct new tree from the provided nodes. Note the caller is responsible
 * for providing the node_array in the right order and that no two nodes may
 * be equal.
 *
 * Assuming the array is in the right order as defined by some comparator
 * function my_cmp_func, calling the function is functionally equivalent to
 *
 * ```
 * void my_build(RBTree* tree, const RBTreeNode** node_array, size_t n)
 * {
 *     rbtree_init(tree);
 *     for(i = 0; i < n; i++)
 *         rbtree_insert(&tree, nodes[i], my_cmp_func);
 * }
 * ```
 *
 * The only benefit of this function over such pseudo-code is that it's faster
 * as its implementation avoids any need for re-balancing during the tree
 * construction and that it results in optimally balanced tree (i.e. minimal
 * and maximal path lengths from root to any leaf differ by most by one.)
 */
void rbtree_build(RBTree* tree, RBTreeNode** nodes, size_t n);


/* Remove a node equal to the key (as defined by the comparator function).
 *
 * Returns pointer to the node disconnected from the tree (so that caller can
 * e.g. to destroy it), or NULL if no such item has been found in the tree.
 */
RBTreeNode* rbtree_remove(RBTree* tree, const RBTreeNode* key);

/* Find a node equal to the key (as defined by the comparator function).
 *
 * Returns pointer to the found node or NULL if no such node has been found in
 * the tree.
 */
RBTreeNode* rbtree_lookup(RBTree* tree, const RBTreeNode* key);


/* The structure and functions below implement a walking over all nodes in the
 * tree. When reaching an end of the iteration, the functions return NULL.
 *
 * The simple walking over the complete tree can be implemented as follows:
 *
 * ```
 * static void walk_over_my_tree(RBTree* tree)
 * {
 *     RBTreeCursor cur;
 *     RBTree* node;
 *
 *     for(node = rbtree_head(tree, &cur);
 *         node != NULL;
 *         node = rbtree_next(&cur))
 *     {
 *         ...
 *     }
 * }
 * ```
 *
 * However note any cursor becomes invalid and must not be used anymore when
 * any nodes are added into the tree or removed from it.
 */
typedef struct RBTreeCursor {
    /* The below is good enough to handle RB-trees of _any_ size. Consider size
     * of the address space in bytes cannot be larger than 2^(8*sizeof(void*)),
     * and that the longest root->leaf path of RB-tree is at most twice as long
     * as the shortest one. */
    RBTreeNode* path[2 * 8 * sizeof(void*) - sizeof(RBTreeNode) + 1];
    unsigned n;
} RBTreeCursor;

/* Initializer for a cursor pointing to nowhere. */
#define RBTREE_CURSOR_INITIALIZER       { { 0 }, 0 }


/* This is similar to rbtree_lookup() but it also initializes the cursor to the
 * corresponding position, so the caller may navigate from the node to neighbors
 * (in the order as defined by the comparator function) via the rbtree_prev()
 * and/or rbtree_next().
 */
RBTreeNode* rbtree_lookup_ex(RBTree* tree, const RBTreeNode* key,
                             RBTreeCursor* cur);

/* Get the node corresponding to the current position of the cursor; or NULL.
 */
RBTreeNode* rbtree_current(RBTreeCursor* cur);

/* The functions rbtree_head() and rbtree_tail() retrieve the first or the last
 * node in the tree.
 *
 * The functions rbtree_next() and rbtree_prev() move the cursor to the next or
 * the previous node (in the order as defined by the comparator function used
 * during construction of the tree).
 *
 * All the functions return a pointer to the node requested and update the
 * provided cursor accordingly.
 *
 * They return NULL (and don't change the cursor in any way) if there is no
 * such node. I.e. rbtree_head() and rbtree_tail() return NULL if the tree
 * is empty. The function rbtree_next() returns NULL, if the cursor already
 * points to the last node in the tree. Similarly, rbtree_prev() returns NULL
 * if the cursor already points to the 1st node in the tree.
 */
RBTreeNode* rbtree_head(RBTree* tree, RBTreeCursor* cur);
RBTreeNode* rbtree_tail(RBTree* tree, RBTreeCursor* cur);
RBTreeNode* rbtree_next(RBTreeCursor* cur);
RBTreeNode* rbtree_prev(RBTreeCursor* cur);


#ifdef __cplusplus
}
#endif

#endif  /* EX_RBTREE_H */
