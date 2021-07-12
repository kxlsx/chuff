#ifndef HFTREE_H
/* ============== */

#ifndef _STDLIB_H
#include <stdlib.h>
#endif
#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef BITOPS_H
#include <bitops.h>
#endif

#define HFTREE_H 1

/* Letter in a non-letter branch (joint branch) */
#define HFTREE_NULL_LETTER (-1)

/* The only valid values are <0; 255> or HFTREE_NULL_LETTER */
typedef short hfletter_t;

struct hfnode{
	hfletter_t letter;
	size_t weight;
	struct hfnode *parent;
	struct hfnode *left;
	struct hfnode *right;
};

#define hfnode_letter(self) \
	((self)->letter)
#define hfnode_weight(self) \
	((self)->weight)
#define hfnode_parent(self) \
	((self)->parent)
#define hfnode_left(self) \
	((self)->left)
#define hfnode_right(self) \
	((self)->right)
#define hfnode_has_children(self) \
	((self)->left != NULL) 

struct hftree{
	struct hfnode *root;
	struct hfnode *ltnodes[256];
};

/* The root hfnode of the hftree at self */
#define hftree_root(self) \
	((self)->root)
/* 
 * An association table of every possible letter (<0; 255>) to
 * the address of their corresponding branches in the hftree at self.
 * If the letter is not in the tree, its address is NULL.
 */
#define hftree_ltnodes(self) \
	((self)->ltnodes)

/* Construct a new hftree from the given association table of chars to their weights */
struct hftree *hftree_new(size_t occ[256]);
void hftree_free(struct hftree *self);
/* 
 * Construct a hftree from its encoded form (by hftree_to_stream) contained in the stream.
 * The hftree is not complete and only stores its original shape and letters;
 * every hfnode weight is set to 0 and the ltnodes association table is unavailable. 
 */
struct hftree *hftree_from_stream(FILE *stream);
short hfletter_from_stream(FILE *stream, int *currc, bitpos_t *currci);
/* 
 * Encode the hftree at self into the provided stream.
 * The encoding stores only the original shape and letters.
 */
bitpos_t hftree_to_stream(struct hftree *self, FILE *stream);

/* ============== */
#endif