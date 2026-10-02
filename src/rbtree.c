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

#include "rbtree.h"
#include <stdint.h>


#define RED_FLAG                ((uintptr_t) 0x1U)

#define COLOR(node)             (((uintptr_t) (node)->lc) & RED_FLAG)
#define IS_RED(node)            (COLOR(node) == RED_FLAG)
#define IS_BLACK(node)          (COLOR(node) != RED_FLAG)

#define MAKE_RED(node)          do { (node)->lc = (RBTreeNode*)((uintptr_t)(node)->lc | RED_FLAG); } while(0)
#define MAKE_BLACK(node)        do { (node)->lc = (RBTreeNode*)((uintptr_t)(node)->lc & ~RED_FLAG); } while(0)
#define TOGGLE_COLOR(node)      do { (node)->lc = (RBTreeNode*)((uintptr_t)(node)->lc ^ RED_FLAG); } while(0)

#define LEFT(node)              ((RBTreeNode*)((uintptr_t)(node)->lc & ~RED_FLAG))
#define RIGHT(node)             ((node)->r)

#define SET_LEFT(node, ptr)     do { (node)->lc = (RBTreeNode*)((uintptr_t)(ptr) | COLOR(node)); } while(0)
#define SET_RIGHT(node, ptr)    do { (node)->r = (ptr); } while(0)


typedef RBTreeCursor RBTreePath;


/* Helper tree "rotation" operations, used as primitives for re-balancing. */
static void
rbtree_rotate_left(RBTree* tree, RBTreeNode* parent, RBTreeNode* node)
{
    RBTreeNode* tmp;

    tmp = RIGHT(node);
    SET_RIGHT(node, LEFT(tmp));
    SET_LEFT(tmp, node);

    if(parent != NULL) {
        if(node == LEFT(parent))
            SET_LEFT(parent, tmp);
        else if(node == RIGHT(parent))
            SET_RIGHT(parent, tmp);
    } else {
        tree->root = tmp;
    }
}

static void
rbtree_rotate_right(RBTree* tree, RBTreeNode* parent, RBTreeNode* node)
{
    RBTreeNode* tmp;

    tmp = LEFT(node);
    SET_LEFT(node, RIGHT(tmp));
    SET_RIGHT(tmp, node);

    if(parent != NULL) {
        if(node == RIGHT(parent))
            SET_RIGHT(parent, tmp);
        else if(node == LEFT(parent))
            SET_LEFT(parent, tmp);
    } else {
        tree->root = tmp;
    }
}

static void
rbtree_leftmost_path(RBTreeNode* node, RBTreePath* path)
{
    while(node != NULL) {
        path->path[path->n++] = node;
        node = LEFT(node);
    }
}

static void
rbtree_rightmost_path(RBTreeNode* node, RBTreePath* path)
{
    while(node != NULL) {
        path->path[path->n++] = node;
        node = RIGHT(node);
    }
}


RBTreeNode*
rbtree_fini_step(RBTree* tree)
{
    RBTreeNode** pointer_down_to_node = &tree->root;
    RBTreeNode* node = tree->root;

    if(node != NULL) {
        /* Go down as far as possible through left children. */
        while(LEFT(node) != NULL) {
            pointer_down_to_node = &node->lc;
            node = LEFT(node);
        }

        /* The node now can have at most one child (the right one). I.e. we can
         * rip out the current node and "upgrade" the right subtree (or NULL)
         * one level up into this node's place. This should be cheaper than
         * iterating to some leaf in the right subtree. */
        *pointer_down_to_node = RIGHT(node);
    }

    return node;
}

/* Extended lookup which searches the tree down from the given node and appends
 * the visited nodes into the path. I.e., if found, the end of the path is the
 * searched node itself. If not found, the path goes down to a leaf node which
 * could become a parent of the key node if caller wants to insert it (or other
 * node equal to it).
 *
 * Returns last comparison, so that caller knows whether found (cmp == 0),
 * or whether the key should be inserted as a left (cmp < 0) or right child
 * (cmp > 0).
 *
 * Note: Caller must be careful to handle en empty tree correctly: If the
 * provided start node (usually root) is NULL, no nodes are added to the path
 * and return value of cmp is undefined:
 *
 * Caller can either detect this after the function returns (if path->n == 0)
 * or he can avoid calling this function altogether (if tree->root == NULL).
 */
static int
rbtree_lookup_path(RBTreeNode* node, const RBTreeNode* key,
                   RBTreeCmpFunc cmp_func, RBTreePath* path)
{
    int cmp = 0;

    while(node != NULL) {
        path->path[path->n++] = node;

        cmp = cmp_func(key, node);
        if(cmp == 0)
            break;

        if(cmp < 0)
            node = LEFT(node);
        else if(cmp > 0)
            node = RIGHT(node);
        else
            break;
    }

    return cmp;
}

static void
rbtree_insert_fixup(RBTree* tree, RBTreePath* path)
{
    RBTreeNode* node;
    RBTreeNode* parent;
    RBTreeNode* grandparent;
    RBTreeNode* grandgrandparent;
    RBTreeNode* uncle;

    /* A newly inserted node usually (except the root) starts as a red one,
     * i.e. it could introduce "a double red" problem in the tree, where both
     * the node and its parent are both red. That's prohibited by the RB-tree
     * rules. */

    while(1) {
        node = path->path[path->n - 1];
        parent = (path->n > 1) ? path->path[path->n - 2] : NULL;

        /* No parent: the node is a root and root should alway be black. */
        if(parent == NULL) {
            MAKE_BLACK(node);
            tree->root = node;
            break;
        }

        /* If parent is black, we could not introduce a double-red problem
         * and we are done. */
        if(IS_BLACK(parent))
            break;

        /* If we reach here, there is the double-red problem.
         * Note grandparent has to exist and be black (implied from red parent). */
        grandparent = path->path[path->n - 3];
        uncle = (parent == LEFT(grandparent)) ? RIGHT(grandparent) : LEFT(grandparent);
        if(uncle == NULL || IS_BLACK(uncle)) {
            /* Black uncle. */
            grandgrandparent = (path->n > 3) ? path->path[path->n - 4] : NULL;
            if(LEFT(grandparent) != NULL  &&   node == RIGHT(LEFT(grandparent))) {
                rbtree_rotate_left(tree, grandparent, parent);
                parent = node;
                node = LEFT(node);
            } else if(RIGHT(grandparent) != NULL  &&  node == LEFT(RIGHT(grandparent))) {
                rbtree_rotate_right(tree, grandparent, parent);
                parent = node;
                node = RIGHT(node);
            }
            if(node == LEFT(parent))
                rbtree_rotate_right(tree, grandgrandparent, grandparent);
            else
                rbtree_rotate_left(tree, grandgrandparent, grandparent);

            /* Note that now, after the rotations, the parent points to where
             * the grand-parent was originally in the tree hierarchy, and
             * the grandparent instead became child of it.
             *
             * We switch their colors and hence make sure the parent (upper in
             * the hierarchy) is now black, fixing the double-red problem. */
            MAKE_BLACK(parent);
            MAKE_RED(grandparent);
            break;
        }

        /* Red uncle. This allows us to make both the parent and the uncle
         * black, propagating the red up to grandparent. */
        MAKE_BLACK(parent);
        MAKE_BLACK(uncle);
        MAKE_RED(grandparent);

        /* But it means we could just move the double-red issue two levels
         * up, so we have to continue there. */
        path->n -= 2;
    }
}

int
rbtree_insert(RBTree* tree, RBTreeNode* node)
{
    RBTreePath path;
    int cmp;

    path.n = 0;

    /* Lookup the place where we should live. */
    cmp = rbtree_lookup_path(tree->root, node, tree->cmp_func, &path);
    if(path.n > 0  &&  cmp == 0) {
        /* An equal node already present. */
        return -1;
    }

    SET_LEFT(node, NULL);
    SET_RIGHT(node, NULL);
    MAKE_RED(node);

    /* Insert the node as child of a leaf node. */
    if(path.n > 0) {
        if(cmp < 0)
            SET_LEFT(path.path[path.n - 1], node);
        else
            SET_RIGHT(path.path[path.n - 1], node);
    } else {
        tree->root = node;
    }
    path.path[path.n++] = node;

    /* Preserve RB-tree properties. */
    rbtree_insert_fixup(tree, &path);

    return 0;
}

static RBTreeNode*
rbtree_build_recurse(RBTreeNode** nodes, size_t n, int level, int* p_red_level)
{
    size_t index;

    /* Detect depth for transition to red nodes. */
    if(n == 0) {
        if(level < *p_red_level)
            *p_red_level = level;
        return NULL;
    }

    index = n/2;
    /* Build right sub-tree first as that one may be smaller than the left one.
     * We need it to detect the transition to red nodes properly. */
    nodes[index]->r = rbtree_build_recurse(nodes+index+1, n - (index+1), level+1, p_red_level);
    nodes[index]->lc = rbtree_build_recurse(nodes+0, index, level+1, p_red_level);

    if(level >= *p_red_level)
        MAKE_RED(nodes[index]);

    return nodes[index];
}

void
rbtree_build(RBTree* tree, RBTreeNode** nodes, size_t n)
{
    int red_level = 2 * 8 * sizeof(void*);
    tree->root = rbtree_build_recurse(nodes, n, 0, &red_level);
}


static void
rbtree_remove_fixup(RBTree* tree, RBTreePath* path)
{
    RBTreeNode* node;
    RBTreeNode* parent;
    RBTreeNode* grandparent;
    RBTreeNode* sibling;

    /* This function is called when a black node has been removed, so we
     * have to fix the black deficit on the provided path. */

    while(1) {
        node = path->path[path->n - 1];
        if(node != NULL  &&  IS_RED(node)) {
            /* Great. Make a red node black one and we are done. */
            MAKE_BLACK(node);
            break;
        }

        if(path->n <= 1) {
            /* The black deficit has bubbled up to the root, i.e. all paths
             * now have the deficit, hence we are balanced once more, just at
             * smaller level. */
            break;
        }

        parent = path->path[path->n - 2];
        /* Sibling is guaranteed to exist because its subtree must have larger
         * black count than our subtree (we have the deficit, right?). */
        sibling = (node == LEFT(parent)) ? RIGHT(parent) : LEFT(parent);
        grandparent = (path->n > 2) ? path->path[path->n - 3] : NULL;

        if(IS_RED(sibling)) {
            /* Red sibling: Convert to black sibling case. */
            if(node == LEFT(parent))
                rbtree_rotate_left(tree, grandparent, parent);
            else
                rbtree_rotate_right(tree, grandparent, parent);

            MAKE_BLACK(sibling);
            MAKE_RED(parent);
            path->path[path->n - 2] = sibling;
            path->path[path->n - 1] = parent;
            path->path[path->n++] = node;
            continue;
        }

        if((LEFT(sibling) != NULL && IS_RED(LEFT(sibling)))  ||
           (RIGHT(sibling) != NULL && IS_RED(RIGHT(sibling)))) {
            /* Black sibling having at least one red child. */
            if(node == LEFT(parent) && (RIGHT(sibling) == NULL || IS_BLACK(RIGHT(sibling)))) {
                MAKE_RED(sibling);
                MAKE_BLACK(LEFT(sibling));
                rbtree_rotate_right(tree, parent, sibling);
                sibling = RIGHT(parent);
            } else if(node == RIGHT(parent) && (LEFT(sibling) == NULL || IS_BLACK(LEFT(sibling)))) {
                MAKE_RED(sibling);
                MAKE_BLACK(RIGHT(sibling));
                rbtree_rotate_left(tree, parent, sibling);
                sibling = LEFT(parent);
            }

            if(COLOR(sibling) != COLOR(parent))
                TOGGLE_COLOR(sibling);
            MAKE_BLACK(parent);
            if(node == LEFT(parent)) {
                MAKE_BLACK(RIGHT(sibling));
                rbtree_rotate_left(tree, grandparent, parent);
            } else {
                MAKE_BLACK(LEFT(sibling));
                rbtree_rotate_right(tree, grandparent, parent);
            }
            break;
        }

        /* Fix the black deficit higher in the tree. */
        MAKE_RED(sibling);
        path->n--;
    }
}

RBTreeNode*
rbtree_remove(RBTree* tree, const RBTreeNode* key)
{
    RBTreePath path;
    RBTreeNode* node;
    RBTreeNode* single_child;
    int cmp;

    path.n = 0;

    /* Lookup the place where we should live. */
    cmp = rbtree_lookup_path(tree->root, key, tree->cmp_func, &path);
    if(path.n == 0  ||  cmp != 0) {
        /* Not found. */
        return NULL;
    }

    node = path.path[path.n - 1];

    /* If we are not at the bottom of the tree, we switch our place with
     * another node, which is our direct successor; i.e. with the minimal
     * value of the right subtree (that one must be at he bottom). */
    if(RIGHT(node) != NULL) {
        RBTreeNode* successor;
        int node_index = path.n - 1;

        if(LEFT(RIGHT(node)) != NULL) {
            RBTreeNode* tmp;

            rbtree_leftmost_path(RIGHT(node), &path);
            successor = path.path[path.n - 1];

            tmp = RIGHT(successor);
            SET_RIGHT(successor, RIGHT(node));
            SET_RIGHT(node, tmp);

            if(successor == LEFT(path.path[path.n - 2]))
                SET_LEFT(path.path[path.n - 2], node);
            else
                SET_RIGHT(path.path[path.n - 2], node);

            path.path[node_index] = successor;
            path.path[path.n - 1] = node;
        } else if(LEFT(node) != NULL) {
            /* The right child is directly the successor. This has to be
             * handled as the code above would entangle the pointers in the
             * case. */
            successor = RIGHT(node);
            SET_RIGHT(node, RIGHT(successor));
            SET_RIGHT(successor, node);

            path.path[path.n - 1] = successor;
            path.path[path.n++] = node;
        } else {
            /* The left node is NULL; i.e. node has at most one child.
             * The code below is capable to handle this. */
            successor = NULL;
        }

        if(successor != NULL) {
            /* Common work for the two active code paths above. */
            SET_LEFT(successor, LEFT(node));
            SET_LEFT(node, NULL);

            if(node_index > 0) {
                if(node == LEFT(path.path[node_index - 1]))
                    SET_LEFT(path.path[node_index - 1], successor);
                else
                    SET_RIGHT(path.path[node_index - 1], successor);
            } else {
                tree->root = successor;
            }

            if(COLOR(successor) != COLOR(node)) {
                TOGGLE_COLOR(successor);
                TOGGLE_COLOR(node);
            }
        }
    }

    /* The node now cannot have more than one child. Move the child (or NULL)
     * upwards to take the place of the node being removed. As a side effect,
     * it leaves the original node disconnected from the tree hierarchy. */
    single_child = (LEFT(node) != NULL) ? LEFT(node) : RIGHT(node);
    if(path.n > 1) {
        if(node == LEFT(path.path[path.n - 2]))
            SET_LEFT(path.path[path.n - 2], single_child);
        else
            SET_RIGHT(path.path[path.n - 2], single_child);
    } else {
        tree->root = single_child;
    }
    path.path[path.n - 1] = single_child;

    /* Re-balancing may be needed if we have removed a black node. */
    if(IS_BLACK(node))
        rbtree_remove_fixup(tree, &path);

    return node;
}

RBTreeNode*
rbtree_lookup(RBTree* tree, const RBTreeNode* key)
{
    RBTreeNode* node = tree->root;
    int cmp;

    while(node != NULL) {
        cmp = tree->cmp_func(key, node);

        if(cmp < 0)
            node = LEFT(node);
        else if(cmp > 0)
            node = RIGHT(node);
        else
            break;
    }

    return node;
}

RBTreeNode*
rbtree_lookup_ex(RBTree* tree, const RBTreeNode* key, RBTreeCursor* cur)
{
    int cmp;

    cur->n = 0;
    cmp = rbtree_lookup_path(tree->root, key, tree->cmp_func, cur);

    if(cur->n == 0  ||  cmp != 0) {
        cur->n = 0; /* No node equal to the key: Reset the cursor. */
        return NULL;
    }

    return cur->path[cur->n - 1];
}

RBTreeNode*
rbtree_current(RBTreeCursor* cur)
{
    return (cur->n > 0) ? cur->path[cur->n - 1] : NULL;
}


RBTreeNode*
rbtree_head(RBTree* tree, RBTreeCursor* cur)
{
    cur->n = 0;
    rbtree_leftmost_path(tree->root, cur);
    return (cur->n > 0) ? cur->path[cur->n - 1] : NULL;
}

RBTreeNode*
rbtree_tail(RBTree* tree, RBTreeCursor* cur)
{
    cur->n = 0;
    rbtree_rightmost_path(tree->root, cur);
    return (cur->n > 0) ? cur->path[cur->n - 1] : NULL;
}

RBTreeNode*
rbtree_next(RBTreeCursor* cur)
{
    if(cur->n > 0) {
        if(RIGHT(cur->path[cur->n - 1]) != NULL) {
            rbtree_leftmost_path(RIGHT(cur->path[cur->n - 1]), cur);
        } else {
            /* Operate with temp. copy of n. This is to keep the cursor point
             * to the last node even if we have reached the maximal/last value. */
            unsigned n = cur->n;

            while(n > 1  &&  cur->path[n - 1] == RIGHT(cur->path[n - 2]))
                n--;
            n--;

            if(n == 0)
                return NULL;
            cur->n = n;
        }
    }
    return (cur->n > 0) ? cur->path[cur->n - 1] : NULL;
}

RBTreeNode*
rbtree_prev(RBTreeCursor* cur)
{
    if(cur->n > 0) {
        if(LEFT(cur->path[cur->n - 1]) != NULL) {
            rbtree_rightmost_path(LEFT(cur->path[cur->n - 1]), cur);
        } else {
            /* Operate with temp. copy of n. This is to keep the cursor point
             * to the first node even if we have reached the minimal/first value. */
            unsigned n = cur->n;

            while(n > 1  &&  cur->path[n - 1] == LEFT(cur->path[n - 2]))
                n--;
            n--;

            if(n == 0)
                return NULL;
            cur->n = n;
        }
    }
    return (cur->n > 0) ? cur->path[cur->n - 1] : NULL;
}


#ifdef EX_TEST
/* Verification of RB-tree correctness. */

/* Returns black height of the tree, or -1 on an error. */
static int
rbtree_verify_recurse(RBTreeNode* node)
{
    RBTreeNode* children[2];
    RBTreeNode* child;
    int child_height[2];
    int i;

    if(node == NULL) {
        /* NULL leaf sentinels are considered to be black. */
        return +1;
    }

    /* Recurse into the both subtrees. */
    children[0] = LEFT(node);
    children[1] = RIGHT(node);
    for(i = 0; i < 2; i++) {
        child = children[i];

        /* Correct tree must never have both a node and its child red. */
        if(IS_RED(node) && child != NULL && IS_RED(child))
            return -1;

        /* Verify the child subtree. */
        child_height[i] = rbtree_verify_recurse(child);
        if(child_height[i] < 0)
            return -1;
    }

    /* Correct tree must have the same black height of the both subtrees. */
    if(child_height[0] != child_height[1])
        return -1;

    return child_height[0] + (IS_BLACK(node) ? 1 : 0);
}

/* Returns 0 if ok, or -1 on an error. */
int
rbtree_verify(RBTree* tree)
{
    /* Correct non-empty tree must have a black root. */
    if(tree->root != NULL  &&  IS_RED(tree->root))
        return -1;

    return (rbtree_verify_recurse(tree->root) >= 0) ? 0 : -1;
}

#endif  /* #ifdef EX_TEST */
