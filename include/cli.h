#ifndef CLI_H
/* ============== */

#ifndef _STDLIB_H
#include <stdlib.h>
#endif
#ifndef _STDBOOL_H
#include <stdbool.h>
#endif

#ifndef META_H
#include <meta.h>
#endif

#define CLI_H 1

/* ===== FLAG STUFF ===== */
/* Flag short names. */
#define FLAG_DECOMPRESS   'd'
#define FLAG_REPLACE      'r'
#define FLAG_NO_OVERWRITE 'n'
#define FLAG_STDOUT       's'
#define FLAG_TIME         't'
#define FLAG_HELP         'h'
#define FLAG_VERSION      'V'
#define FLAG_LICENSE      'L'

#define FLAGSMAX 64

/* Returns the index of a flag based on its short name. */
#define FLAG_INDEX(flag_char) \
	flag_char % FLAGSMAX

#define FLAG_INIT(flag_char, flag_name, flag_description) \
	[FLAG_INDEX(flag_char)] = { \
		flag_char,              \
		flag_name,              \
		flag_description,       \
		0                       \
	}
/* Declaration and definition of the FLAG global variable
 * storing runtime & meta data about the flags encountered
 * upon parsing argv.
 */
struct flag{
	char ch;
	char *name;
	char *description;
	bool is_present;
}FLAGS[FLAGSMAX] = { \
	FLAG_INIT(FLAG_DECOMPRESS,   "decompress",   "Decompress [FILES] with the ." EXTENSION " suffix"), \
	FLAG_INIT(FLAG_REPLACE,      "replace",      "Remove [FILES] upon completion"),                    \
	FLAG_INIT(FLAG_NO_OVERWRITE, "no-overwrite", "Do not overwrite files"),                            \
	FLAG_INIT(FLAG_STDOUT,       "stdout",       "Write results to stdout"),                           \
	FLAG_INIT(FLAG_TIME,         "time",         "Print the time it took to finish"),                  \
	FLAG_INIT(FLAG_HELP,         "help",         "Print help information"),                            \
	FLAG_INIT(FLAG_VERSION,      "version",      "Print version"),                                     \
	FLAG_INIT(FLAG_LICENSE,      "license",      "Print license")                                      \
};

/* Returns the flag struct in FLAGS based on the provided short name. */
#define FLAG(flag_char) \
	FLAGS[FLAG_INDEX(flag_char)]
/* ===== END FLAG STUFF ===== */

/* Run appropriate functions based on the encountered arguments. */
int process_args(int argc, char **argv);

void print_help(void);
void print_license(void);
void print_version(void);
void print_usage(void);

/* Read argv setting every flag and
 * returning an array of copies of non-flag
 * arguments.
 * The returned array is NULL terminated.
 */
char **read_args(int argc, char **argv);
/* Free the array returned by read_args */
void free_args(char **args);
/* Set the is_present field of the flag in flag_str. */
void parse_flag(char *flag_str);



/* ============== */
#endif