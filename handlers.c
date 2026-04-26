#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>

#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include "path.h"
#include "handlers.h"

#define BACKING_DIR "/mnt/work/Research/filesystem_testing/data"

/**
 * @brief Retrieves file or directory attributes.
 *
 * This function is called by FUSE to get the status of a file or directory.
 * It handles the root and virtual "/search" directories manually and
 * forwards other paths to the underlying filesystem.
 *
 * @param path   The virtual path requested by the OS.
 * @param stbuf  A pointer to the stat structure to be populated.
 * @param fi     FUSE file information (unused).
 * @return       0 on success, or -ENOENT if the path is not found.
 */

int fs_getattr(const char *path, struct stat *stbuf,
                      struct fuse_file_info *fi) {
    (void) fi; // fi is unused; need to avoid compiler warning

    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    if (is_search_path(path)) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    } // handle virtual "/search" directory; to add querying later

    char fullpath[512];
    build_fullpath(fullpath, path);

    // printf("GETATTR: %s -> %s\n", path, fullpath); // debug

    if (stat(fullpath, stbuf) == -1) // stat call to get file/directory attributes
        return -ENOENT;

    return 0;
}



/**
 * @brief Retrieves files inside a directory.
 *
 * This function is called by FUSE to list the contents of a directory.
 * It populates the directory buffer with entries from the underlying filesystem.
 *
 *
 * @param path      The virtual path requested by the OS.
 * @param buf       A pointer to the buffer to be populated with directory entries.
 * @param filler    A function pointer to the FUSE fill_dir_t function.
 * @param offset    The offset into the directory to start reading from. (unused)
 * @param fi        A pointer to the FUSE file information structure. (unused)
 * @param flags     The flags for the readdir operation. (unused)
 * @return          0 on success, or -ENOENT if the path is not found.
 */

int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi,
                      enum fuse_readdir_flags flags) {
    // avoid compiler warning
    (void) offset;
    (void) fi;
    (void) flags;

    if (strcmp(path, "/") == 0) {
        filler(buf, ".", NULL, 0, 0);
        filler(buf, "..", NULL, 0, 0);

        filler(buf, "search", NULL, 0, 0); // add "search" directory entry

        DIR *dp = opendir(BACKING_DIR);
        if (!dp)
            return -ENOENT;

        struct dirent *de;
        while ((de = readdir(dp)) != NULL) { // for every directory entry
            if (strcmp(de->d_name, ".") == 0 ||
                strcmp(de->d_name, "..") == 0)
                continue;

            filler(buf, de->d_name, NULL, 0, 0);
        }

        closedir(dp);
        return 0;
    }

    if (is_search_path(path)) {
        filler(buf, ".", NULL, 0, 0);
        filler(buf, "..", NULL, 0, 0);
        return 0;
    }

    char fullpath[512];
    build_fullpath(fullpath, path);

    // printf("READDIR: %s -> %s\n", path, fullpath); // debug

    DIR *dp = opendir(fullpath); // open the directory to read its contents
    if (!dp)
        return -ENOENT; // file not found

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        filler(buf, de->d_name, NULL, 0, 0); // populate the buffer with directory entries
    }

    closedir(dp);
    return 0;
}


/**
 * @brief Retrieves files inside a directory.
 *
 * This function is called by FUSE to read the contents of a directory.
 * If the path starts with "/search", it returns -ENOENT to indicate the path is not found.
 * Otherwise, it populates the buffer with directory entries using the FUSE fill_dir_t function.
 *
 *
 * @param path      The virtual path requested by the OS.
 * @param buf       A pointer to the buffer to be populated with directory entries.
 * @param size      The size of the buffer.
 * @param offset    The offset into the directory to start reading from.
 * @param fi        A pointer to the FUSE file information structure. (unused)
 * @return          0 on success, or -ENOENT if the path is not found.
 */

int fs_read(const char *path, char *buf, size_t size, off_t offset,
                   struct fuse_file_info *fi) {
    // avoid compiler warning
    (void) fi;

    if (is_search_path(path))
        return -ENOENT; // path not found (accomodate later)

    char fullpath[512];
    build_fullpath(fullpath, path);

    int fd = open(fullpath, O_RDONLY);
    if (fd == -1)
        return -ENOENT;

    int res = pread(fd, buf, size, offset); // read from file descriptor into buffer
    close(fd);

    if (res == -1)
        return -errno; // return error code

    return res;
}
