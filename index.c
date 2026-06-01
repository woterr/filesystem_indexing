#include "index.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define BACKING_DIR "/mnt/work/Research/filesystem_testing/data"

FileEntry indexed_files[MAX_FILES];
int file_count = 0;

void build_index(void)
{
    DIR *dp = opendir(BACKING_DIR);

    if (!dp) {
        perror("opendir");
        return;
    }

    struct dirent *de;

    while ((de = readdir(dp)) != NULL) {

        if (strcmp(de->d_name, ".") == 0 ||
            strcmp(de->d_name, "..") == 0)
            continue;

        strncpy(
            indexed_files[file_count].path,
            de->d_name,
            sizeof(indexed_files[file_count].path) - 1
        );

        indexed_files[file_count].path[
            sizeof(indexed_files[file_count].path) - 1
        ] = '\0';

        file_count++;

        if (file_count >= MAX_FILES)
            break;
    }

    closedir(dp);

    printf("Indexed %d files\n", file_count);
}