#ifndef INDEX_H
#define INDEX_H

#define MAX_FILES 1000

typedef struct {
    char path[512];
} FileEntry;

extern FileEntry indexed_files[MAX_FILES];
extern int file_count;

void build_index(void);

#endif
