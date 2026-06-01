#ifndef PATH_H
#define PATH_H
const char *get_search_query(const char *path);
int is_search_path(const char *path);
void build_fullpath(char *dest, const char *path);

#endif
