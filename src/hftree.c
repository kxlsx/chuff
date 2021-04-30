#include <stdlib.h>
#include <stdio.h>

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
    struct srtqueue *nqueue;

    if((tree = calloc(1, sizeof(struct hftree))) == NULL)
		return NULL;

	if((nqueue = srtqueue_new()) == NULL)
		return NULL;
	for(short i = 0; i < 256; i++){
		if(occ[i] != 0){
			if((node = malloc(sizeof(struct hfnode))) == NULL) 
				return NULL;
			node->left = node->right = node->parent = NULL;
			node->letter = i;
			node->weight = occ[i];
            tree->ltnodes[i] = node;
			if(srtqueue_push(nqueue, node) > 0)
				return NULL;
		}
	}
	
	while(srtqueue_len(nqueue) > 1){
		min_left = srtqueue_pop(nqueue);
		min_right = srtqueue_pop(nqueue);
		if((node = malloc(sizeof(struct hfnode))) == NULL) 
			return NULL;
		min_left->parent = min_right->parent = node;
		node->left = min_left;
		node->right = min_right;
		node->parent = NULL;
		node->letter = HFTREE_NULL_LETTER;
		node->weight = min_left->weight + min_right->weight;
		if(srtqueue_push(nqueue, node) > 0)
			return NULL;
	}
	if(srtqueue_is_empty(nqueue)){
		free(tree);
		srtqueue_free(nqueue);
		return NULL;
	}
	tree->root = srtqueue_pop(nqueue);

	srtqueue_free(nqueue);
    return tree;
}

struct hftree *hftree_from_stream(FILE *stream){
	struct hftree *tree;
	struct hfnode *stack[HFNODE_STACK_SIZE], **stackp;
	struct hfnode *root, *curr, *parent;
	int readch;
	bitpos_t readchi;
	bool_t is_null_node, is_right;

	if((tree = calloc(1, sizeof(struct hftree))) == NULL)
		return NULL;

	if((readch = getc(stream)) == EOF){
		hftree_free(tree);
		return NULL;
	}
	readchi = 6;

	if(!BIT_CHECK(readch, 7)){
		if((root = malloc(sizeof(struct hfnode))) == NULL)
			return NULL;
		if((root->letter = hfletter_from_stream(stream, (char *)&readch, &readchi)) == EOF){
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

	stackp = stack;

	if((root = malloc(sizeof(struct hfnode))) == NULL)
		return NULL;
	tree->root = root;
	root->letter = HFTREE_NULL_LETTER;
	root->weight = 0;
	root->parent = NULL;
	*(stackp++) = parent = root;
	is_right = 0;
	while(stackp != stack){
		if((curr = malloc(sizeof(struct hfnode))) == NULL)
			return NULL;
		curr->weight = 0;
		curr->parent = parent;

		if(is_right){
			parent->right = curr;
			parent = *((--stackp) - 1);
		}else{
			parent->left = curr;
		}

		is_null_node = BIT_CHECK(readch, readchi);
		if(--(readchi) == 255){
			if((readch = getc(stream)) == EOF){
				hftree_free(tree);
				return NULL;
			}
			readchi = 7;
		}
		if(is_null_node){
			curr->letter = HFTREE_NULL_LETTER;
			is_right = 0;
			*(stackp++) = curr;
			parent = curr;
		}else{
			if((curr->letter = hfletter_from_stream(stream, (char *)&readch, &readchi)) == EOF){
				hftree_free(tree);
				return NULL;
			}
			curr->left = NULL;
			curr->right = NULL;
			is_right = 1;
		}
	}
	return tree;
}

short hfletter_from_stream(FILE *stream, char *currc, bitpos_t *currci){
	char letter;
	letter = 0;
	for(bitpos_t i = 7; i != 255; i--){
		BIT_OR(letter, i, BIT_CHECK(*currc, *currci));
		if(--(*currci) == 255){
			if((*currc = getc(stream)) == EOF)
				return EOF;
			*currci = 7;
		}
	}
	return letter;
}

bitpos_t hftree_to_stream(struct hftree *self, FILE *stream){
	struct hfnode *stack[HFNODE_STACK_SIZE], **stackp;
	struct hfnode *curr, *child;
	hfletter_t letter;
	int ch;
	bitpos_t chi;
	bool_t is_null_letter;

    stackp = stack;
    *(stackp++) = curr = self->root;
	ch = 0;
	chi = 7;
    while(stack != stackp){
        curr = *(--stackp);
		letter = curr->letter;
		BIT_OR(ch, chi, (is_null_letter = (letter == HFTREE_NULL_LETTER)));
		if(--chi == 255){
			putc(ch, stream);
			ch = 0;
			chi = 7;
		}
		if(is_null_letter){
			if((child = curr->right) != NULL) 
				*(stackp++) = child;
			if((child = curr->left) != NULL) 
				*(stackp++) = child;
		}else{
			for(bitpos_t i = 7; i != 255; i--){
				BIT_OR(ch, chi, BIT_CHECK(letter, i));
				if(--chi == 255){
					putc(ch, stream);
					ch = 0;
					chi = 7;
				}
			}
		}
    }
	if(chi != 7){
		putc(ch, stream);
		return chi + 1;
	}
	return 0;
}

void hftree_free(struct hftree *self){
	struct hfnode *stack[HFNODE_STACK_SIZE], **stackp; 
	struct hfnode *curr, *child;

    stackp = stack;
	if(self->root != NULL)
    	*(stackp++) = curr = self->root;
    while(stack != stackp){
        curr = *(--stackp);
        if((child = curr->right) != NULL) 
            *(stackp++) = child;
        if((child = curr->left) != NULL) 
            *(stackp++) = child;
         free(curr);
    }
    free(self);
}
