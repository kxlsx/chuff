#include <comp.h>
#include <bitops.h>
#include <hftree.h>
#include <errdef.h>

int compress_to_stream(FILE *src, FILE *dst){
	size_t occ[256] = {0};
	struct hftree *hftree;
	struct hfnode *curr, *parent;
	char hfcode[32] = {0};
	bitpos_t hfcodei;
	char ch;
	bitpos_t chi;
	int readch;
	bitpadding_t padding;

	count_char_occ(src, occ);
	if((hftree = hftree_new(occ)) == NULL)
		return ERR_HFTREE_NEW_FAIL;

	putc((padding = 0), dst);
	padding |= hftree_to_stream(hftree, dst) << 4;

	ch = 0;
	chi = 7;
	if(fseek(src, 0, SEEK_SET) < 0){
		hftree_free(hftree);
		return ERR_READ_FAIL;
	};
	if(hfnode_has_children(hftree_root(hftree))){
		while((readch = getc(src)) != EOF){
			hfcodei = 0;
			curr = hftree_ltnodes(hftree)[readch];
			while((parent = hfnode_parent(curr)) != NULL){
				BIT_SET(hfcode[hfcodei / 8], 7 - (hfcodei % 8), curr != hfnode_left(parent));
				hfcodei++;
				curr = parent;
			}
			while(--hfcodei != 255){
				BIT_OR(ch, chi, BIT_CHECK(hfcode[hfcodei / 8], 7 - (hfcodei % 8)));
				if(--chi == 255){
					putc(ch, dst);
					ch = 0;
					chi = 7;
				}
			}
		}
	}else{
		while((readch = getc(src)) != EOF){
			if(--chi == 255){
				putc(ch, dst);
				chi = 7;
			}
		}
	}
	if(chi != 7){
		padding |= chi + 1;
		putc(ch, dst);
	}

	fseek(dst, 0, SEEK_SET);
	putc(padding , dst);

	hftree_free(hftree);
	return (ferror(dst) < 0) ? ERR_WRITE_FAIL : 0;
}


int decompress_to_stream(FILE *src, FILE *dst){
	struct hftree *hftree;
	struct hfnode *curr;
	int readch, pre_readc;
	bitpos_t readchi;
	bitpadding_t padding;
	
	if((readch = getc(src)) == EOF){
		return ERR_UNEXPECTED_EOF;
	};
	padding = (readch & 7) - 1;
	if((hftree = hftree_from_stream(src)) == NULL)
		return ERR_HFTREE_FROM_STREAM_FAIL;
	curr = hftree_root(hftree);

	if((readch = getc(src)) == EOF){
		hftree_free(hftree);
		return ERR_UNEXPECTED_EOF;
	}
	while((pre_readc = getc(src)) != EOF){
		readchi = 8;
		while(--readchi != 255){
			if(hfnode_has_children(curr)){
				if(BIT_CHECK(readch, readchi))
					curr = curr->right;
				else
					curr = curr->left;
			}
			if(!hfnode_has_children(curr)){
				putc(curr->letter, dst);
				curr = hftree_root(hftree);
			}
		}
		readch = pre_readc;
	}
	readchi = 8;
	while(--readchi != padding){
		if(hfnode_has_children(curr)){
			if(BIT_CHECK(readch, readchi))
				curr = curr->right;
			else
				curr = curr->left;
		}
		if(!hfnode_has_children(curr)){
			putc(curr->letter, dst);
			curr = hftree_root(hftree);
		}
	}
	
	hftree_free(hftree);
	return (ferror(dst) < 0) ? ERR_WRITE_FAIL : 0;
}

void count_char_occ(FILE *stream, size_t occ[256]){
	int readch;
	while((readch = getc(stream)) != EOF) occ[readch]++;
}
