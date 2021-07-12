#ifndef CLI_H
/* ============== */

#ifndef _STDLIB_H
#include <stdlib.h>
#endif
#ifndef _STDBOOL_H
#include <stdbool.h>
#endif

#define CLI_H 1

/* Flag definitions */
#define FLAG_DECOMPRESS   'd'
#define FLAG_FORCE        'f'
#define FLAG_NO_OVERWRITE 'n'
#define FLAG_STDOUT       's'
#define FLAG_TIME         't'
#define FLAG_HELP         'h'
#define FLAG_VERSION      'V'
#define FLAG_LICENSE      'L'

#define FLAGSMAX 64
#define FLAG_INDEX(flag_char) \
	flag_char % FLAGSMAX
#define FLAG_INIT(flag_char, flag_name, flag_description) \
	[FLAG_INDEX(flag_char)] = { \
        flag_char,              \
        flag_name,              \
        flag_description,       \
        0                       \
	}
struct flag{
    char ch;
    char *name;
    char *description;
    bool is_present;
}FLAGS[FLAGSMAX] = { \
    FLAG_INIT(FLAG_DECOMPRESS,   "decompress",   "Decompress an hfc SRC_FILE into DST_FILE"), \
    FLAG_INIT(FLAG_FORCE,        "force",        "Force decompression of any file format"),   \
    FLAG_INIT(FLAG_NO_OVERWRITE, "no-overwrite", "Do not overwrite DST_FILE if it exists"),   \
    FLAG_INIT(FLAG_STDOUT,       "stdout",       "Use stdout as DST_FILE"),                   \
    FLAG_INIT(FLAG_TIME,         "time",         "Print the time it took to finish"),         \
    FLAG_INIT(FLAG_HELP,         "help",         "Print help information"),                   \
    FLAG_INIT(FLAG_VERSION,      "version",      "Print version"),                            \
    FLAG_INIT(FLAG_LICENSE,      "license",      "Print license")                             \
};
#define FLAG(flag_char) \
	FLAGS[FLAG_INDEX(flag_char)]

/* Argument definitons */
#define ARGSC 2
struct arg{
    char *name; char *value;
}ARGS[ARGSC] = { \
    {"SRC_FILE", NULL}, \
    {"DST_FILE", NULL}, \
};

int process_args(int argc, char **argv);
void process_code(int return_code);

int read_args(int argc, char **argv);
void read_flags(char *flag_str);

void print_help(void);
void print_license(void);
void print_version(void);
void print_usage(void);

/* ============== */
#endif