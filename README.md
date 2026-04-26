# filesystem_indexing

cat -> kernel syscall (read/open) -> VFS -> FUSE -> function calls (fs_read) -> real disk (BACKING_DIR) -> back to user
