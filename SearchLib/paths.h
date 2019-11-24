#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct file_path_list {
	char **paths;
	unsigned int index;
	size_t capacity;
};

void path_list_free(struct file_path_list *list);
void path_list_add(struct file_path_list *list, char *path);
struct file_path_list *path_list_init();
