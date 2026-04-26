#define BACKING_DIR "/mnt/work/Research/filesystem_testing/data"
#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include<stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
static int fs_getattr(const char *path, struct stat *stbuf,
                      struct fuse_file_info *fi) {
    (void) fi;

    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s%s", BACKING_DIR, path);
    printf("GETATTR: %s -> %s\n", path, fullpath);
    if (stat(fullpath, stbuf) == -1)
        return -ENOENT;

    return 0;
}

static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi,
                      enum fuse_readdir_flags flags) {

    (void) offset;
    (void) fi;
    (void) flags;

    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s%s", BACKING_DIR, path);

    printf("READDIR: %s -> %s\n", path, fullpath);
    DIR *dp = opendir(fullpath);
    if (dp == NULL)
        return -ENOENT;

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        filler(buf, de->d_name, NULL, 0, 0);
    }


    closedir(dp);
    return 0;
}

static int fs_read(const char *path, char *buf, size_t size, off_t offset,
                   struct fuse_file_info *fi) {

    (void) fi;

    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s%s", BACKING_DIR, path);

    int fd = open(fullpath, O_RDONLY);
    if (fd == -1)
        return -ENOENT;

    int res = pread(fd, buf, size, offset);
    close(fd);

    if (res == -1)
        return -errno;

    return res;
}

static struct fuse_operations ops = {
    .getattr = fs_getattr,
    .readdir = fs_readdir,
    .read    = fs_read,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &ops, NULL);
}
