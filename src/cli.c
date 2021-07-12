#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <cli.h>
#include <comp.h>
#include <fpath.h>
#include <meta.h>

#define eprintf(fmt, ...) \
    fprintf(stderr, fmt, ## __VA_ARGS__)
#define strcmp_safe(a, b) \
    (a == NULL || b == NULL) ? -1 : strcmp(a, b)

char RDBUF[RDBUFSIZE];
char WRBUF[WRBUFSIZE];

/* Used at exit when allocated a value into ARGS[1].value */
void free_DST_PATH(){
    free(ARGS[1].value);
}

int process_args(int argc, char **argv){
    FILE *src, *dst;
    char *src_file_name;
    bool is_correct_format;
    int (* main_fn)(FILE *, FILE *), main_ret;
    src = dst = NULL;

    if(read_args(argc, argv) == ERR_TOO_MANY_ARGS)
        return ERR_TOO_MANY_ARGS;

    if(ARGS[0].value == NULL){
        if(FLAG(FLAG_HELP).is_present){
            print_help();
            return 0;
        }else if(FLAG(FLAG_LICENSE).is_present){
            print_license();
            return 0;
        }else if(FLAG(FLAG_VERSION).is_present){
            print_version();
            return 0;
        }else{
            return ERR_NO_ARGS_PROVIDED;
        }
    }
    
    if(FLAG(FLAG_DECOMPRESS).is_present){
        is_correct_format = strcmp_safe(fextension(ARGS[0].value), EXTENSION) == 0;
        if(!is_correct_format && !FLAG(FLAG_FORCE).is_present)
            return ERR_UNRECOGNIZED_FORMAT;
        main_fn = decompress_to_stream;
    }else{
        main_fn = compress_to_stream;
    }

    if(ARGS[1].value == NULL){
        if(FLAG(FLAG_STDOUT).is_present)
            dst = stdout;
        else{
            if((src_file_name = fname(ARGS[0].value)) == NULL)
                return ERR_DIR_NAME_PROVIDED;
            if(FLAG(FLAG_DECOMPRESS).is_present){
                if(is_correct_format && strcmp(ARGS[0].value, "." EXTENSION) != 0){
                    ARGS[1].value = fnoextension(src_file_name);
                }else{
                    return ERR_NO_DST_PATH;
                }
            }else{
                ARGS[1].value = fwithextension(src_file_name, EXTENSION, 3);
            }
            atexit(free_DST_PATH);
        }
    }
    if((src = fopen(ARGS[0].value, "rb")) == NULL)
        return ERR_READ_FAIL;
    if(ARGS[1].value != NULL){
        if(fexists(ARGS[1].value) && FLAG(FLAG_NO_OVERWRITE).is_present){
            fclose(src);
            return ERR_NO_OVERWRITE;
        }
        if((dst = fopen(ARGS[1].value, "wb")) == NULL){
            fclose(src);
            return ERR_WRITE_FAIL; 
        }
    }
    setvbuf(src, RDBUF, _IOFBF, RDBUFSIZE);
    setvbuf(dst, WRBUF, _IOFBF, WRBUFSIZE);

    if(FLAG(FLAG_TIME).is_present){
        clock_t ts = clock();
        main_ret = main_fn(src, dst);
        /* Write time to stderr when using FLAG_STDOUT just in case */
        fprintf(
            (FLAG(FLAG_STDOUT).is_present) ? stderr : stdout,
            "%lfs\n", ((double) clock() - ts) / CLOCKS_PER_SEC
        );
    }else{
        main_ret = main_fn(src, dst);
    }
    
    fclose(src);
    fclose(dst);
    return main_ret;
}

void process_code(int return_code){
    switch(return_code){
    case 0:
        break;
	case ERR_UNEXPECTED_EOF:
		eprintf("\"%s\": Unexpected EOF\n", ARGS[0].value);
		break;
	case ERR_READ_FAIL:
	    eprintf("\"%s\": %s\n", ARGS[0].value, strerror(errno));
		break;
	case ERR_WRITE_FAIL:
        if(ARGS[1].value == NULL) 
            ARGS[1].value = "stdout";
		eprintf("\"%s\": %s\n", ARGS[1].value, strerror(errno));
		break;
	case ERR_HFTREE_NEW_FAIL:
		eprintf(
			"\"%s\": Failed constructing header information\n", 
			ARGS[0].value
		);
		break;
	case ERR_HFTREE_FROM_STREAM_FAIL:
		eprintf(
			"\"%s\": Failed reading header information\n", 
			ARGS[0].value
		);
		break;
	case ERR_NO_ARGS_PROVIDED:
        eprintf("USAGE:\n\t" PROGRAM_NAME " [FLAGS] ");
        for(unsigned i = 0; i < ARGSC; i++){
            eprintf("[%s] ", ARGS[i].name);
        }
        eprintf("\nFor more information try --help\n");
		break;
	case ERR_TOO_MANY_ARGS:
		eprintf("Too many arguments (max: %d)\n", ARGSC);
		break;
	case ERR_UNRECOGNIZED_FORMAT:
		eprintf(
        	"\"%s\": Unrecognized file format "
			"(use -f to force decompression)\n", 
            ARGS[0].value
        );
		break;
	case ERR_NO_OVERWRITE:
		eprintf("\"%s\": File exists\n", ARGS[1].value);
		break;
    case ERR_NO_DST_PATH:
        eprintf("Cannot infer path to DST_FILE "
        "(provide it as the second argument)\n");
        break;
    case ERR_DIR_NAME_PROVIDED:
        eprintf("\"%s\": Is a directory path\n", ARGS[0].value);
        break;
	default:
		eprintf("%d: unhandled\n", return_code);
		break;
	}
}

int read_args(int argc, char **argv){
    unsigned pathc;

    pathc = 0;
    for(int argi = 1; argi < argc; argi++){
        if(argv[argi][0] == '-'){
            read_flags(argv[argi] + 1);
        }else{
            ARGS[pathc].value = argv[argi];
            if((pathc++) == ARGSC){
                return ERR_TOO_MANY_ARGS;
            }
        }
    }
    return 0;
}

void read_flags(char *flag_str){
    char c;

    if(flag_str[0] == '-'){
        flag_str++;
        for(unsigned flagi = 0; flagi < FLAGSMAX; flagi++){
            if(FLAGS[flagi].ch != '\0' && strcmp(flag_str, FLAGS[flagi].name) == 0)
                FLAGS[flagi].is_present = 1;
        }
    }else{
        while((c = *(flag_str++)) != '\0'){
            if(FLAG(c).ch != '\0'){
                FLAG(c).is_present = 1;
            }
        }
    }
}

void print_help(void){
    unsigned i;

    puts(
        PROGRAM_NAME " v" VERSION "\n"
        AUTHORS "\n"
        DESCRIPTION "\n"
        "\n"
        "USAGE:"
    );
    printf("\t" PROGRAM_NAME " [FLAGS] ");
    for(i = 0; i < ARGSC; i++){
        printf("[%s] ", ARGS[i].name);
    }
    puts(
        "\n" 
        "FLAGS:"
    );
    for(i = 0; i < FLAGSMAX; i++){
        if(FLAGS[i].ch != '\0'){
            printf("\t-%c, --%s\n", FLAGS[i].ch, FLAGS[i].name);
            printf("\t\t%s\n\n", FLAGS[i].description);
        }
    }
}

void print_license(void){
    puts(
        PROGRAM_NAME " v" VERSION "\n"
        AUTHORS "\n"
        "\n"
        LICENSE
    );
}

void print_version(void){
    puts(
        PROGRAM_NAME " v" VERSION
    );
}

