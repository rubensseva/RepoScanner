#define _GNU_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

struct file_path_list {
	char **paths;
	unsigned int index;
	size_t capacity;
};

static void path_list_expand(struct file_path_list *list);
void path_list_free(struct file_path_list *list);
void path_list_add(struct file_path_list *list, char *path);

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

void path_list_expand(struct file_path_list *list)
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

int search(char *path, int verbose, int searchHidden, char **repos,
	   int *repo_index)
{
	if (verbose) {
		printf("\n---------\nSearching in directory: %s\n", path);
	}

	DIR *d;
	struct dirent *dir;
	char buf[PATH_MAX + 1];

	struct file_path_list *local_files;

	local_files = path_list_init();

	d = opendir(path);

	while (d && (dir = readdir(d)) != NULL) {
		realpath(dir->d_name, buf);
		if (dir->d_type == DT_REG) {
			if (verbose == 1) {
				printf("[%s] ", buf);
				printf("Regular file: %s\n", dir->d_name);
			}
		}
		// If it is a directory
		if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 &&
		    strcmp(dir->d_name, "..") != 0) {
			if (verbose == 1) {
				printf("[%s] ", buf);
				printf("Directory: %s\n", dir->d_name);
			}
			char *d_path = malloc(255 * sizeof(char));
			if (d_path == NULL) {
				printf("Error when allocating for d_path\n");
			}
			sprintf(d_path, "%s/%s", path, dir->d_name);
			if (strcmp(dir->d_name, ".git") == 0) {
				if (verbose == 1) {
					printf("Found git repo!");
					printf("In: %s\n", d_path);
				}
				int len = strlen(d_path);
				d_path[len - 4] = '\0';
				sprintf(repos[(*repo_index)++], "%s", d_path);
				path_list_free(local_files);
				printf("%s\n", d_path);
				return 0;
			} else {
				if (searchHidden == 1 ||
				    dir->d_name[0] != '.') {
					path_list_add(local_files, d_path);
				}
			}
		} else {
			//not regular file, could be symlink etc
		}
	}

	for (int i = 0; i < local_files->index; i++) {
		search(local_files->paths[i], verbose, searchHidden, repos,
		       repo_index);
	}

	closedir(d);
	path_list_free(local_files);
	return 0;
}

int main(int argc, char *argv[])
{
	int i;
	int verbose = 0;
	int searchHidden = 0;
	char *path = ".";
	if (argc > 1) {
		if (argv[1][0] != '-') {
			path = argv[1];
		}
	}
	for (i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose = 1;
		}
		if (strcmp(argv[i], "-h") == 0) {
			searchHidden = 1;
		}
	}

	if (verbose == 1) {
		printf("searching for path %s\n", path);
	}

	char **repos;
	repos = malloc(1000 * sizeof(char *));
	for (int i = 0; i < 1000; i++) {
		repos[i] = malloc(1000 * sizeof(char));
	}
	int repo_index = 0;

	search(path, verbose, searchHidden, repos, &repo_index);

	for (int i = 0; i < repo_index; i++) {
		free(repos[i]);
	}
	free(repos);

	return (0);
}
