#ifndef META_H
/* ============== */

#define META_H 1

/* Read stream buffer size */
#define RDBUFSIZE 0x2000
/* Write stream buffer size */
#define WRBUFSIZE 0x2000

#define EXTENSION "hfc"

/* Metadata */
#define PROGRAM_NAME \
    "chuff"
#define VERSION \
    "1.0.0"
#define DESCRIPTION \
    "Compress or decompress [FILES] into the current directory (compress by default)."
#define LICENSE \
    "This is free software. You may redistribute copies of it under the terms of\n" \
    "the GNU General Public License <https://www.gnu.org/licenses/gpl.html>.\n" \
    "There is NO WARRANTY, to the extent permitted by law."
#define AUTHORS \
    "Łukasz Dragon <lukasz.b.dragon@gmail.com>"


/* ===== Error codes ===== */
/* No error */
#define OK 0
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

/* ============== */
#endif