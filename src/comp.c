#include <comp.h>
#include <bitops.h>
#include <hftree.h>
#include <meta.h>

int compress_to_stream(FILE *src, FILE *dst){
	size_t occ[256] = {0};
	struct hftree *hftree;
	struct hfnode *curr, *parent;
	char hfcode[32] = {0};
	bitpos_t hfcode_i;
	char charbuf;
	bitpos_t charbuf_i;
	int readchar;
	bitpadding_t padding;

	/* count char occurences and construct a hftree */
	count_char_occ(src, occ);
	hftree = hftree_new(occ);
	if(hftree == NULL) return ERR_HFTREE_NEW_FAIL;

	/* reserve first byte for padding */
	putc((padding = 0), dst);
	/* encode the hftree and write it into dst */
	padding |= hftree_to_stream(hftree, dst) << 4;

	/* fseek to the start of src */
	if(fseek(src, 0, SEEK_SET) < 0){
		hftree_free(hftree);
		return ERR_READ_FAIL;
	}

	charbuf = 0;
	charbuf_i = 7;
	if(hfnode_has_children(hftree_root(hftree))){
		/* start compressing */
		while((readchar = getc(src)) != EOF){
			hfcode_i = 0;
			curr = hftree_ltnodes(hftree)[readchar];
			/* traverse the tree from the bottom */ 
			while((parent = hfnode_parent(curr)) != NULL){
				BIT_SET(
					hfcode[hfcode_i / 8], 
					7 - (hfcode_i % 8), 
					curr != hfnode_left(parent)
				);
				hfcode_i++;
				curr = parent;
			}
			/* write the resulting hfcode backwards 
			 * into charbuf, flushing charbuf into dst
			 * when it fills up
			 */
			while(--hfcode_i != 255){
				BIT_OR(
					charbuf, 
					charbuf_i, 
					BIT_CHECK(hfcode[hfcode_i / 8], 7 - (hfcode_i % 8))
				);
				if(--charbuf_i == 255){
					putc(charbuf, dst);
					charbuf = 0;
					charbuf_i = 7;
				}
			}
		}
	}else{
		/* start compressing with a hftree of size 1 */
		while((readchar = getc(src)) != EOF){
			if(--charbuf_i == 255){
				putc(charbuf, dst);
				charbuf_i = 7;
			}
		}
	}
	/* flush charbuf if needed */
	if(charbuf_i != 7){
		padding |= charbuf_i + 1;
		putc(charbuf, dst);
	}

	/* put padding data in the first byte of dst */
	fseek(dst, 0, SEEK_SET);
	putc(padding , dst);

	hftree_free(hftree);
	return (ferror(dst) < 0) ? ERR_WRITE_FAIL : OK;
}


int decompress_to_stream(FILE *src, FILE *dst){
	struct hftree *hftree;
	struct hfnode *curr;
	int readchar, readchar_next;
	bitpos_t readchar_i;
	bitpadding_t padding;
	
	/* read padding data */
	readchar = getc(src);
	if(readchar == EOF){
		return ERR_UNEXPECTED_EOF;
	};
	padding = (readchar & 7) - 1;

	/* read the encoded hftree */
	hftree = hftree_from_stream(src);
	if(hftree == NULL)
		return ERR_HFTREE_FROM_STREAM_FAIL;
	curr = hftree_root(hftree);

	/* decompress the data into dst */
	readchar = getc(src);
	if(readchar == EOF){
		hftree_free(hftree);
		return ERR_UNEXPECTED_EOF;
	}
	while((readchar_next = getc(src)) != EOF){
		readchar_i = 8;
		while(--readchar_i != 255){
			if(hfnode_has_children(curr)){
				if(BIT_CHECK(readchar, readchar_i))
					curr = curr->right;
				else
					curr = curr->left;
			}
			if(!hfnode_has_children(curr)){
				putc(curr->letter, dst);
				curr = hftree_root(hftree);
			}
		}
		readchar = readchar_next;
	}
	readchar_i = 8;
	while(--readchar_i != padding){
		if(hfnode_has_children(curr)){
			if(BIT_CHECK(readchar, readchar_i))
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
	return (ferror(dst) < 0) ? ERR_WRITE_FAIL : OK;
}

void count_char_occ(FILE *stream, size_t occ[256]){
	int readchar;
	while((readchar = getc(stream)) != EOF) occ[readchar]++;
}
