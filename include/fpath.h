#ifndef FPATH_H
/* ============== */

#ifndef _STDLIB_H
#include <stdlib.h>
#endif
#ifndef _STDBOOL_H
#include <stdbool.h>
#endif

#define FPATH_H 1

/* Return 1 if the file at fpath exists */ 
bool fexists(char *fpath);
/* Return a pointer to the start of the filename in fpath */
char *fname(char *fpath);
/* Return a pointer to the start of the extension in fpath */
char *fextension(char *fpath);
/* Return a mallocd copy of fpath, but '.' + ext at the end */
char *fwithextension(char *fpath, char *ext, size_t ext_len);
/* Return a mallocd copy of fpath without the its extension */
char *fnoextension(char *fpath);

/* ============== */
#endif