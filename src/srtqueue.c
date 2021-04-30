#include <stdlib.h>
#include <stdio.h>

#include <srtqueue.h>

struct srtqueue *srtqueue_new(void){
    struct srtqueue *self;

    if((self = malloc(sizeof(struct srtqueue))) == NULL)
        return NULL;
    self->root = NULL;
    self->len = 0;
    return self;
}

int srtqueue_push(struct srtqueue *self, srtq_item_t item){
    struct srtq_node  **currp, *curr;

    currp = &(self->root);
    while((curr = *currp) != NULL){
        if(srtq_item_cmp(item, curr->item))
            currp = &(curr->left);
        else
            currp = &(curr->right);
    }
    if((curr = *currp = malloc(sizeof(struct srtq_node))) == NULL)
        return 1;
    curr->item = item;
    curr->left = NULL;
    curr->right = NULL;
    self->len++;
    return 0;
}

srtq_item_t srtqueue_pop(struct srtqueue *self){
    struct srtq_node **currp, *curr;
    srtq_item_t item;

    currp = &(self->root);
    while((curr = *currp)->left != NULL) 
        currp = &(curr->left);
    *currp = curr->right;
    item = curr->item;
    self->len--;
    free(curr);
    return item;
}

void srtqueue_free(struct srtqueue *self){
    struct srtq_node **stack, **stackp;
    struct srtq_node *curr, *child;

    if(self->len > 0){
        stack = stackp = malloc(self->len * sizeof(struct srtq_node *));
        *(stackp++) = curr = self->root;
        while(stack != stackp){
            curr = *(--stackp);
            if((child = curr->right) != NULL) 
                *(stackp++) = child;
            if((child = curr->left) != NULL) 
                *(stackp++) = child;
            free(curr);
        }
        free(stack);
    }
    free(self);
}
