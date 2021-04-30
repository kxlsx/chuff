#ifndef _CLI_H
/* ============== */

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _BITOPS_H
#include <bitops.h>
#endif

#define _CLI_H 1

int process_args(int argc, char **argv);
void process_code(int return_code);

int read_args(int argc, char **argv);
void read_flags(char *flag_str);

void print_help(void);
void print_license(void);
void print_version(void);
void print_usage(void);

/* Return 1 if the file at fpath exists */ 
bool_t fexists(char *fpath);
/* Return a pointer to the start of the filename in fpath */
char *fname(char *fpath);
/* Return a pointer to the start of the extension in fpath */
char *fextension(char *fpath);
/* Return a mallocd copy of fpath, but '.' + ext at the end */
char *fwithextension(char *fpath, char *ext, size_t ext_len);
/* Return a mallocd copy of fpath without the its extension */
char *fnoextension(char *fpath);
/* strcmp but doesn't die when a and/or b is NULL */
int strcmp_safe(char *a, char *b);

/* ============== */
#endif