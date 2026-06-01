#include "path.h"
#include <string.h>
#include <stdio.h>

#define BACKING_DIR "/mnt/work/Research/filesystem_testing/data"

int is_search_path(const char *path) {
    return strncmp(path, "/search", 7) == 0;
}

void build_fullpath(char *dest, const char *path) {
    snprintf(dest, 512, "%s%s", BACKING_DIR, path);
}

const char *get_search_query(const char *path)
{
    if (strncmp(path, "/search/", 8) != 0)
        return NULL;

    return path + 8;
}
