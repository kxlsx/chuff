#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <fpath.h>

bool fexists(char *fpath){
    FILE *f;
    if((f = fopen(fpath, "r")) == NULL){
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
