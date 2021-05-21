#ifndef _CONFIG_H
/* ============== */

#ifndef _BITOPS_H
#include <bitops.h>
#endif

#define _CONFIG_H 1

/* Read stream buffer size */
#define RDBUFSIZE 4096
/* Write stream buffer size */
#define WRBUFSIZE 4096

#define EXTENSION "hfc"

#define PROGRAM_NAME \
    "huffc"
#define VERSION \
    "0.0.2"
#define DESCRIPTION \
    "Compress or decompress SRC_FILE into DST_FILE (by default compress SRC_FILE into the current directory)"
#define LICENSE \
    "This is free software. You may redistribute copies of it under the terms of\n" \
    "the GNU General Public License <https://www.gnu.org/licenses/gpl.html>.\n" \
    "There is NO WARRANTY, to the extent permitted by law."
#define AUTHORS \
    "Łukasz Dragon <lukasz.b.dragon@gmail.com>"

#define FLAGSMAX 256
#define FLAG_DECOMPRESS   'd'
#define FLAG_FORCE        'f'
#define FLAG_NO_OVERWRITE 'n'
#define FLAG_STDOUT       's'
#define FLAG_TIME         't'
#define FLAG_HELP         'h'
#define FLAG_VERSION      'V'
#define FLAG_LICENSE      'L'
struct flag{
    char *name; char *description; bool_t is_present;
};
extern struct flag FLAGS[FLAGSMAX];
#define FLAGS_DECLARATION \
    struct flag FLAGS[FLAGSMAX] = { \
            [FLAG_DECOMPRESS]   = {"decompress",   "Decompress an hfc SRC_FILE into DST_FILE", 0}, \
            [FLAG_FORCE]        = {"force",        "Force decompression of any file format",   0}, \
            [FLAG_NO_OVERWRITE] = {"no-overwrite", "Do not overwrite DST_FILE if it exists",   0}, \
            [FLAG_STDOUT]       = {"stdout",       "Use stdout as DST_FILE",                   0}, \
            [FLAG_TIME]         = {"time",         "Print the time it took to finish",         0}, \
            [FLAG_HELP]         = {"help",         "Print help information",                   0}, \
            [FLAG_VERSION]      = {"version",      "Print version",                            0}, \
            [FLAG_LICENSE]      = {"license",      "Print license",                            0}, \
        }

#define ARGSC 2
struct arg{
    char *name; char *value; bool_t mallocd;
};
extern struct arg ARGS[ARGSC];
#define ARGS_DECLARATION \
    struct arg ARGS[ARGSC] = { \
        {"SRC_FILE", NULL, 0}, \
        {"DST_FILE", NULL, 0}, \
    }

/* Util to free every arg with the malloc flag set */
#define ARGS_FREE_MALLOCD() \
    for(size_t i = 0; i < ARGSC; i++){ \
        if(ARGS[i].mallocd){ \
            free(ARGS[i].value); \
        } \
    }

/* ============== */
#endif