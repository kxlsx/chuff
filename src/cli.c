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

/* Returns the file name from src_path with
 * the EXTENSION trimmed out.
 * Will print errors to stderr on error.
 */
char *get_dst_path_decompress(char *src_path){
    char *dst_path;

    if(strcmp_safe(fextension(src_path), EXTENSION) != 0){
        eprintf("\"%s\": Unrecognised format, ignoring.\n", src_path);
        return NULL;
    }

    dst_path = fname(src_path);
    if(dst_path == NULL){
        eprintf("\"%s\" is a directory.\n", src_path);
        return NULL;
    }

    dst_path = fnoextension(dst_path);
    if(FLAG(FLAG_NO_OVERWRITE).is_present && fexists(dst_path)){
        eprintf("\"./%s\": Exists, ignoring.\n", dst_path);
        free(dst_path);
        return NULL;
    }

    return dst_path;
}

/* Returns the file name from src_path with
 * EXTENSION appended at the end.
 * Will print errors to stderr on error.
 */
char *get_dst_path_compress(char *src_path){
    char *dst_path;

    dst_path = fname(src_path);
    if(dst_path == NULL){
        eprintf("\"%s\" is a directory.\n", src_path);
        return NULL;
    }

    dst_path = fwithextension(dst_path, EXTENSION, 3);
    if(FLAG(FLAG_NO_OVERWRITE).is_present && fexists(dst_path)){
        eprintf("\"./%s\": Exists, ignoring.\n", dst_path);
        free(dst_path);
        return NULL;
    }
    return dst_path;
}

int process_args(int argc, char **argv){
    char **args, **argsp;
    char *(*get_dst_path_fn)(char *);
    char *src_path, *dst_path;
    FILE *src, *dst;
    int (*main_fn)(FILE *, FILE *), main_ret;
 
    args = argsp = read_args(argc, argv);
    if(args == NULL){
        eprintf("Failed allocating memory.\n");
        return OK;
    }

    /* only print meta information if flags are present */
    if(FLAG(FLAG_HELP).is_present){
        print_help();
        return OK;
    }else if(FLAG(FLAG_LICENSE).is_present){
        print_license();
        return OK;
    }else if(FLAG(FLAG_VERSION).is_present){
        print_version();
        return OK;
    }
    if(*args == NULL){
        print_usage();
        return OK;
    }

    /* set appropriate functions */
    if(FLAG(FLAG_DECOMPRESS).is_present){
        get_dst_path_fn = get_dst_path_decompress;
        main_fn = decompress_to_stream;
    }else{
        get_dst_path_fn = get_dst_path_compress;
        main_fn = compress_to_stream;
    }

    /* for every provided file path */
    dst_path = NULL;
    while((src_path = *(argsp++)) != NULL){
        /* try to open src */
        src = fopen(src_path, "rb");
        if(src == NULL){
            eprintf("\"%s\": %s.\n", src_path, strerror(errno));
            free(dst_path);
            continue;
        }

        /* try to open dst */
        if(FLAG(FLAG_STDOUT).is_present){
            dst = stdout;
        }else{
            dst_path = get_dst_path_fn(src_path);
            if(dst_path == NULL){
                fclose(src);
                continue;
            };

            dst = fopen(dst_path, "wb"); 
            if(dst == NULL){
                eprintf("\"%s\": %s.\n", dst_path, strerror(errno));
                free(dst_path);
                fclose(src);
                continue;
            }
        }
  
        /* set up read and write buffers */
        setvbuf(src, RDBUF, _IOFBF, RDBUFSIZE);
        setvbuf(dst, WRBUF, _IOFBF, WRBUFSIZE);

        /* do the compression/decompression */
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

        if(main_ret != OK){
            /* handle any resulting errors */
            switch(main_ret){
            case ERR_UNEXPECTED_EOF:
                eprintf("\"%s\": Unexpected EOF\n", src_path);
                break;
            case ERR_READ_FAIL:
                eprintf("\"%s\": %s.\n", src_path, strerror(errno));
                break;
            case ERR_WRITE_FAIL:
                eprintf("\"%s\": %s.\n", (dst_path == NULL) ? "stdout": dst_path, strerror(errno));
                break;
            case ERR_HFTREE_NEW_FAIL:
                eprintf(
                    "\"%s\": Failed constructing header information\n", 
                    src_path
                );
                break;
            case ERR_HFTREE_FROM_STREAM_FAIL:
                eprintf(
                    "\"%s\": Failed reading header information\n", 
                    src_path
                );
                break;
            default:
                eprintf("%d: unhandled\n", main_ret);
                break;
            }

            /* remove the unfinished dst file */
            if(dst_path != NULL) remove(dst_path);
        }

        free(dst_path);
        fclose(src);
        fclose(dst);
    }

    free_args(args);
    return OK;
}

void print_usage(void){
    puts(
        "USAGE:\n" 
        "\t" PROGRAM_NAME " [FLAGS]... [FILES]...\n"
        "Type \"" PROGRAM_NAME " -h\" for help."
    );
}

void print_help(void){
    puts(
        PROGRAM_NAME " v" VERSION "\n"
        AUTHORS "\n"
        DESCRIPTION "\n"
        "\n"
        "USAGE:\n"
        "\t" PROGRAM_NAME " [FLAGS]... [FILES]...\n"
        "FLAGS:"
    );
    for(unsigned i = 0; i < FLAGSMAX; i++){
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

char **read_args(int argc, char **argv){
    char **args, **argsp;

    args = argsp = malloc(sizeof(char *) * argc);
    if(args == NULL) return NULL;

    for(int i = 1; i < argc; i++){
        if(argv[i][0] == '-'){
            parse_flag(argv[i] + 1);
        }else{
            *argsp = malloc(strlen(argv[i]) + 1);
            if(*argsp == NULL) return NULL;

            strcpy(*argsp, argv[i]);
            argsp++;
        }
    }
    *argsp = NULL;
    return args;
}

void free_args(char **args){
    char **argsp;
    
    argsp = args;
    while(*argsp != NULL) free(*(argsp++));
    free(args);
}

void parse_flag(char *flag_str){
    char c;

    if(flag_str[0] == '-'){
        flag_str++;
        for(unsigned flagi = 0; flagi < FLAGSMAX; flagi++){
            if(FLAGS[flagi].ch != '\0' 
            && strcmp(flag_str, FLAGS[flagi].name) == 0)
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
