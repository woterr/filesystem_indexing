#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include "handlers.h"

// gcc main.c handlers.c path.c -o fs `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64


static struct fuse_operations ops = { // define fuse operations
    .getattr = fs_getattr,
    .readdir = fs_readdir,
    .read    = fs_read,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &ops, NULL); // start fuse main loop
}
