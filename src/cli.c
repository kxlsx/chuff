#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <cli.h>
#include <comp.h>
#include <config.h>
#include <errdef.h>

char RDBUF[RDBUFSIZE];
char WRBUF[WRBUFSIZE];

int process_args(int argc, char **argv){
    FILE *src, *dst;
    char *src_file_name;
    bool_t is_correct_format;
    int (* main_fn)(FILE *, FILE *), main_ret;
    src = dst = NULL;

    if(read_args(argc, argv) == ERR_TOO_MANY_ARGS)
        return ERR_TOO_MANY_ARGS;

    if(ARGS[0].value == NULL){
        if(FLAGS[FLAG_HELP].is_present){
            print_help();
            return 0;
        }else if(FLAGS[FLAG_LICENSE].is_present){
            print_license();
            return 0;
        }else if(FLAGS[FLAG_VERSION].is_present){
            print_version();
            return 0;
        }else{
            return ERR_NO_ARGS_PROVIDED;
        }
    }
    
    if(FLAGS[FLAG_DECOMPRESS].is_present){
        is_correct_format = strcmp_safe(fextension(ARGS[0].value), EXTENSION) == 0;
        if(!is_correct_format && !FLAGS[FLAG_FORCE].is_present)
            return ERR_UNRECOGNIZED_FORMAT;
        main_fn = decompress_to_stream;
    }else{
        main_fn = compress_to_stream;
    }

    if(ARGS[1].value == NULL){
        if(FLAGS[FLAG_STDOUT].is_present)
            dst = stdout;
        else{
            if((src_file_name = fname(ARGS[0].value)) == NULL)
                return ERR_DIR_NAME_PROVIDED;
            if(FLAGS[FLAG_DECOMPRESS].is_present){
                if(is_correct_format && strcmp(ARGS[0].value, "." EXTENSION) != 0){
                    ARGS[1].value = fnoextension(src_file_name);
                }else{
                    return ERR_NO_DST_PATH;
                }
            }else{
                ARGS[1].value = fwithextension(src_file_name, EXTENSION, 3);
            }
            ARGS[1].mallocd = 1;
        }
    }
    if((src = fopen(ARGS[0].value, "rb")) == NULL)
        return ERR_READ_FAIL;
    if(ARGS[1].value != NULL){
        if(fexists(ARGS[1].value) && FLAGS[FLAG_NO_OVERWRITE].is_present){
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

    if(FLAGS[FLAG_TIME].is_present){
        clock_t ts = clock();
        main_ret = main_fn(src, dst);
        printf("%lfs\n", ((double) clock() - ts) / CLOCKS_PER_SEC);
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
        print_usage();
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
    for(size_t i = 0; i < ARGSC; i++){
        if(ARGS[i].mallocd){
            free(ARGS[i].value);
        }
    }
}

int read_args(int argc, char **argv){
    size_t pathc;

    pathc = 0;
    for(size_t i = 1; i < (size_t)argc; i++){
        if(argv[i][0] == '-'){
            read_flags(argv[i] + 1);
        }else{
            ARGS[pathc].value = argv[i];
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
        for(size_t i = 0; i < FLAGSC; i++){
            if(FLAGS[i].name != NULL && strcmp(flag_str, FLAGS[i].name) == 0)
                FLAGS[i].is_present = 1;
        }
    }else{
        while((c = *(flag_str++)) != '\0'){
            if(FLAGS[(size_t)c].name != NULL){
                FLAGS[(size_t)c].is_present = 1;
            }
        }
    }
}

void print_help(void){
    size_t i;

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
    for(i = 0; i < FLAGSC; i++){
        if(FLAGS[i].name != NULL){
            printf("\t-%c, --%s\n", (char)i, FLAGS[i].name);
            printf("\t\t%s\n\n", FLAGS[i].description);
        }
    }
}

void print_license(void){
    puts(
        PROGRAM_NAME " v" VERSION "\n"
        LICENSE
    );
}

void print_version(void){
    puts(
        PROGRAM_NAME " v" VERSION
    );
}

void print_usage(void){
    puts("USAGE :");
    printf("\t" PROGRAM_NAME " [FLAGS] ");
    for(size_t i = 0; i < ARGSC; i++){
        printf("[%s] ", ARGS[i].name);
    }
    puts(
        "\n"
        "For more information try --help"
    );
}

bool_t fexists(char *fpath){
    FILE *f;
    if((f = fopen(fpath, "r")) == NULL){
        return 0;
    }else{
        fclose(f);
        return 1;
    }
}

char *fname(char *fpath){
    char *name;
    #ifdef _WIN32
    (name = strrchr(fpath, '\\')) ? ++name : (name = fpath);
    #else
    (name = strrchr(fpath, '/')) ? ++name : (name = fpath);
    #endif
    return *name != '\0' ? name : NULL;
}

char *fextension(char *fpath){
    char *ext;
    (ext = strrchr(fpath, '.')) ? ++ext : NULL;
    return ext;
}

char *fwithextension(char *fpath, char *ext, size_t ext_len){
    char *result;
    size_t fpath_len, result_len;
    
    fpath_len = strlen(fpath);
    result_len = fpath_len + ext_len + 2;
    if((result = malloc(result_len)) == NULL)
        return NULL;
    result[result_len - 1] = '\0';
    memcpy(result, fpath, fpath_len);
    result[fpath_len] = '.';
    memcpy(result + fpath_len + 1, ext, ext_len);
    return result;
}

char *fnoextension(char *fpath){
    char *result, *ext;
    size_t result_len, ext_len;

    ext = fextension(fpath);
    ext_len = ext != NULL ? strlen(ext) : 0;
    result_len = strlen(fpath) - ext_len;
    result = malloc(result_len);
    memcpy(result, fpath, result_len);
    result[result_len - 1] = '\0';
    return result;
}

int strcmp_safe(char *a, char *b){
    return (a == NULL || b == NULL) ? -1 : strcmp(a, b);
}
