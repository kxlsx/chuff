#ifndef META_H
/* ============== */

#define META_H 1

/* Read stream buffer size */
#define RDBUFSIZE 4096
/* Write stream buffer size */
#define WRBUFSIZE 4096

#define EXTENSION "hfc"

/* Metadata */
#define PROGRAM_NAME \
    "huffc"
#define VERSION \
    "0.0.3"
#define DESCRIPTION \
    "Compress or decompress SRC_FILE into DST_FILE (by default compress SRC_FILE into the current directory)"
#define LICENSE \
    "This is free software. You may redistribute copies of it under the terms of\n" \
    "the GNU General Public License <https://www.gnu.org/licenses/gpl.html>.\n" \
    "There is NO WARRANTY, to the extent permitted by law."
#define AUTHORS \
    "Łukasz Dragon <lukasz.b.dragon@gmail.com>"


/* Error codes */
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

/* ============== */
#endif