#ifndef SRTQUEUE_H
/* ============== */

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef HFTREE_H
#include <hftree.h>
#endif

#define SRTQUEUE_H 1

/* Type of items stored in the srtqueue. */
typedef struct hfnode *srtq_item_t;
/* Return 1 if a should be placed before b. */
#define srtq_item_cmp(a, b) (a->weight < b->weight)

struct srtq_node{
	struct srtq_node *left;
	struct srtq_node *right;
	srtq_item_t item;
};

/* A queue with the order based on the srtq_item_cmp macro. */
struct srtqueue{
	struct srtq_node *root;
	size_t len;
};

#define srtqueue_len(self) \
	((self)->len)
#define srtqueue_is_empty(self) \
	((self)->len == 0)

struct srtqueue *srtqueue_new(void);
void srtqueue_free(struct srtqueue *self);
/* Will return a non-zero value if failed allocating new node. */
int srtqueue_push(struct srtqueue *self, srtq_item_t item);
/* Will fucking die if the srtqueue at self is empty. */
srtq_item_t srtqueue_pop(struct srtqueue *self);

/* ============== */
#endif