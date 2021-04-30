#ifndef _ERRDEF_H
/* ============== */

#ifndef _STDIO_H
#include <stdio.h>
#endif

#define _ERRDEF_H 1

/* Error caused by the provided stream being too short */
#define ERR_UNEXPECTED_EOF 1
/* Error caused by any possible read error */
#define ERR_READ_FAIL 2
/* Error caused by any possible write error */
#define ERR_WRITE_FAIL 3
/* Error caused either by malloc failure or an empty src stream */
#define ERR_HFTREE_NEW_FAIL 4
/* Error caused either by malloc failure or an unexpected EOF */
#define ERR_HFTREE_FROM_STREAM_FAIL 5

/* Error caused by providing zero arguments */
#define ERR_NO_ARGS_PROVIDED 8
/* Error caused by providing more than ARGSC arguments */
#define ERR_TOO_MANY_ARGS 9
/* Error caused by trying to decompress a non-hfc file */
#define ERR_UNRECOGNIZED_FORMAT 10
/* Error caused by setting off the no-overwrite flag */
#define ERR_NO_OVERWRITE 11
/* Error caused when no dst_path can be deduced */
#define ERR_NO_DST_PATH 12
/* Error cause when providing SRC_FILE path ending with '/' or '\\' */
#define ERR_DIR_NAME_PROVIDED 13

#define eprintf(fmt, ...) fprintf(stderr, fmt, ## __VA_ARGS__)

/* ============== */
#endif