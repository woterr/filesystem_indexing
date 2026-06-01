#ifndef HANDLERS_H
#define HANDLERS_H
#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>

int fs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi);
int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags);
int fs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

#endif
