#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <fpath.h>

bool fexists(char *fpath){
    FILE *f;

    f = fopen(fpath, "r");
    if(f == NULL){
        return false;
    }else{
        fclose(f);
        return true;
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
    return (ext = strrchr(fpath, '.')) ? ++ext : NULL;
}

char *fwithextension(char *fpath, char *ext){
    char *result;
    size_t fpath_len, ext_len, result_len;
    
    fpath_len = strlen(fpath);
    ext_len = strlen(ext);
    result_len = fpath_len + ext_len + 2;

    result = malloc(result_len);
    if(result == NULL) return NULL;
    
    memcpy(result, fpath, fpath_len);
    result[fpath_len] = '.';
    memcpy(result + fpath_len + 1, ext, ext_len);
    result[result_len - 1] = '\0';

    return result;
}

char *fnoextension(char *fpath){
    char *result, *ext;
    size_t result_len, ext_len;

    ext = fextension(fpath);
    ext_len = ext != NULL ? strlen(ext) : 0;
    result_len = strlen(fpath) - ext_len;

    result = malloc(result_len);
    if(result == NULL) return NULL;

    memcpy(result, fpath, result_len);
    result[result_len - 1] = '\0';
    
    return result;
}
