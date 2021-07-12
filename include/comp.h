#ifndef COMP_H
/* ============== */

#ifndef _STDIO_H
#include <stdio.h>
#endif

#define COMP_H 1

int compress_to_stream(FILE *src, FILE *dst);
int decompress_to_stream(FILE *src, FILE *dst);
/* Count every char in stream into occ */
void count_char_occ(FILE *stream, size_t occ[256]);

/* ============== */
#endif