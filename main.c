#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include "handlers.h"
#include "index.h"

// gcc main.c handlers.c path.c index.c \
          -D_FILE_OFFSET_BITS=64 \
          -o fs \
          -lfuse3 -lpthread


static struct fuse_operations ops = { // define fuse operations
    .getattr = fs_getattr,
    .readdir = fs_readdir,
    .read    = fs_read,
};

int main(int argc, char *argv[]) {
    build_index();

    return fuse_main(argc, argv, &ops, NULL); // start fuse main loop
}
