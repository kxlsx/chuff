#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <hftree.h>
#include <srtqueue.h>
#include <bitops.h>

/*
 * Max number of branches in a Huffman Tree with
 * a byte-based alphabet is 511 but i set it to
 * 512 just in case.
 */
#define HFNODE_STACK_SIZE (512 * sizeof(struct srtq_node *))

struct hftree *hftree_new(size_t occ[256]){
	struct hftree *tree;
	struct hfnode *node, *min_left, *min_right;
	struct srtqueue *node_queue;

	tree = calloc(1, sizeof(struct hftree));
	if(tree == NULL) return NULL;

	node_queue = srtqueue_new();
	if(node_queue == NULL) return NULL;

	/* fill node_queue with nodes
	 * made from non empty occ entries
	 */
	for(short i = 0; i < 256; i++){
		if(occ[i] != 0){
			node = malloc(sizeof(struct hfnode));
			if(node == NULL) return NULL;

			node->left = node->right = node->parent = NULL;
			node->letter = i;
			node->weight = occ[i];
			tree->ltnodes[i] = node;
			if(srtqueue_push(node_queue, node) > 0)
				return NULL;
		}
	}

	/* construct the hftree */
	while(srtqueue_len(node_queue) > 1){
		node = malloc(sizeof(struct hfnode));
		if(node == NULL) return NULL;

		min_left = srtqueue_pop(node_queue);
		min_right = srtqueue_pop(node_queue);
		min_left->parent = min_right->parent = node;

		node->left = min_left;
		node->right = min_right;
		node->parent = NULL;
		node->letter = HFTREE_NULL_LETTER;
		node->weight = min_left->weight + min_right->weight;

		if(srtqueue_push(node_queue, node) > 0)
			return NULL;
	}
	if(srtqueue_is_empty(node_queue)){
		free(tree);
		srtqueue_free(node_queue);
		return NULL;
	}
	/* set root to be the last node in the queue */
	tree->root = srtqueue_pop(node_queue);

	srtqueue_free(node_queue);
	return tree;
}

struct hftree *hftree_from_stream(FILE *stream){
	struct hftree *tree;
	struct hfnode *stack[HFNODE_STACK_SIZE], **stackp;
	struct hfnode *root, *curr, *parent;
	int readchar;
	bitpos_t readchar_i;
	bool is_null_node, is_right;

	stackp = stack;

	tree = calloc(1, sizeof(struct hftree));
	if(tree == NULL) return NULL;

	readchar = getc(stream);
	if(readchar == EOF){
		hftree_free(tree);
		return NULL;
	}
	readchar_i = 6;

	/* check if the encoded tree is a single letter branch */
	if(!BIT_CHECK(readchar, 7)){
		root = malloc(sizeof(struct hfnode));
		if(root == NULL) return NULL;

		root->letter = hfletter_from_stream(stream, &readchar, &readchar_i);
		if(root->letter == EOF){
			hftree_free(tree);
			return NULL;
		}

		root->weight = 0;
		root->parent = NULL;
		root->left = NULL;
		root->right = NULL;
		tree->root = root;
		return tree;
	}

	/* alloc a root node and push it onto the stack */
	root = malloc(sizeof(struct hfnode));
	if(root == NULL)return NULL;

	tree->root = root;
	root->letter = HFTREE_NULL_LETTER;
	root->weight = 0;
	root->parent = NULL;
	*(stackp++) = parent = root;

	is_right = 0;
	while(stackp != stack){
		/* alloc a node */
		curr = malloc(sizeof(struct hfnode));
		if(curr == NULL) return NULL;
		curr->weight = 0;
		curr->parent = parent;

		if(is_right){
			parent->right = curr;
			/* go back up two nodes in the stack */
			parent = *((--stackp) - 1);
		}else{
			parent->left = curr;
		}

		/* check whether curr is a null or letter node */
		is_null_node = BIT_CHECK(readchar, readchar_i);
		/* refill readchar if needed */
		if(--(readchar_i) == 255){
			readchar = getc(stream);
			if(readchar == EOF){
				hftree_free(tree);
				return NULL;
			}
			readchar_i = 7;
		}
		if(is_null_node){
			/* set curr as a null node and push it onto the stack */
			curr->letter = HFTREE_NULL_LETTER;
			is_right = 0;
			*(stackp++) = curr;
			parent = curr;
		}else{
			/* read curr's letter from stream */
			curr->letter = hfletter_from_stream(stream, &readchar, &readchar_i);
			if(curr->letter == EOF){
				hftree_free(tree);
				return NULL;
			}
			curr->left = NULL;
			curr->right = NULL;
			/* set the next node to be right */
			is_right = 1;
		}
	}
	return tree;
}

short hfletter_from_stream(FILE *stream, int *currchar, bitpos_t *currchar_i){
	unsigned char letter;
	letter = 0;
	for(bitpos_t i = 7; i != 255; i--){
		BIT_OR(letter, i, BIT_CHECK(*currchar, *currchar_i));
		if(--(*currchar_i) == 255){
			*currchar = getc(stream);
			if(*currchar == EOF) return EOF;
			*currchar_i = 7;
		}
	}

	return letter;
}

bitpos_t hftree_to_stream(struct hftree *self, FILE *stream){
	struct hfnode *stack[HFNODE_STACK_SIZE], **stackp;
	struct hfnode *curr;
	hfletter_t letter;
	int charbuf;
	bitpos_t charbuf_i;
	bool is_null_letter;

	stackp = stack;
	/* push the root onto the stack */
	*(stackp++) = curr = self->root;

	charbuf = 0;
	charbuf_i = 7;
	while(stack != stackp){
		/* pop a branch from the stack and get its data */
		curr = *(--stackp);
		letter = curr->letter;
		is_null_letter = (letter == HFTREE_NULL_LETTER);

		/* write the current branch into charbuf
		 * (letter branch -> 0, null branch -> 1)
		 */
		BIT_OR(charbuf, charbuf_i, is_null_letter);
		/* flush charbuf into stream if needed */
		if(--charbuf_i == 255){
			putc(charbuf, stream);
			charbuf = 0;
			charbuf_i = 7;
		}
		if(is_null_letter){
			/* push children of the null branch onto the stack */
			if(curr->right != NULL) *(stackp++) = curr->right;
			if(curr->left != NULL) *(stackp++) = curr->left;
		}else{
			/* write the letter into stream*/
			for(bitpos_t i = 7; i != 255; i--){
				BIT_OR(charbuf, charbuf_i, BIT_CHECK(letter, i));
				if(--charbuf_i == 255){
					putc(charbuf, stream);
					charbuf = 0;
					charbuf_i = 7;
				}
			}
		}
	}
	/* flush charbuf into stream if needed */
	if(charbuf_i != 7){
		putc(charbuf, stream);
		return charbuf_i + 1;
	}
	return 0;
}

void hftree_free(struct hftree *self){
	struct hfnode *stack[HFNODE_STACK_SIZE], **stackp;
	struct hfnode *curr;

	/* free every node using postorder traversal */
	stackp = stack;
	if(self->root != NULL)
		*(stackp++) = curr = self->root;
	while(stack != stackp){
		curr = *(--stackp);
		if(curr->right != NULL) *(stackp++) = curr->right;
		if(curr->left != NULL) *(stackp++) = curr->left;
		free(curr);
	}
	free(self);
}
