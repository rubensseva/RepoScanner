#include "paths.h"


static void path_list_expand(struct file_path_list *list)
{
	size_t expand_length = 10;
	char **tmp = (char **)malloc((list->capacity + expand_length) *
				     sizeof(char *));
	if (tmp == NULL) {
		printf("Error when trying to allocate expanded memory for local files\n");
	}
	for (int i = 0; i < list->index; i++) {
		tmp[i] = (char *)malloc(255 * sizeof(char));
		strcpy(tmp[i], (list->paths)[i]);
	}
	path_list_free(list);
	list->paths = tmp;
	list->capacity += expand_length;
}


void path_list_add(struct file_path_list *list, char *path)
{
	if (list->index + 1 >= list->capacity) {
		path_list_expand(list);
	}
	list->paths[list->index] = path;
	(list->index)++;
}


void path_list_free(struct file_path_list *list)
{
	for (int i = 0; i < list->index; i++) {
		free(list->paths[i]);
	}
	free(list->paths);
}


struct file_path_list *path_list_init()
{
	struct file_path_list *created;
	created = malloc(sizeof(struct file_path_list));
	if (created == NULL) {
		printf("Failed to initialize new file_path_list struct");
	}
	int init_size = 10;
	created->paths = (char **)malloc(init_size * sizeof(char *));
	if (created->paths == NULL) {
		printf("Failed to initialize paths for new file_path_list\n");
	}
	created->capacity = 10;
	created->index = 0;
	return created;
}
