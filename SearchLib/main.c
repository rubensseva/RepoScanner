#define _GNU_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "paths.h"



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

  // Maintain a cache of directories visited
	local_files = path_list_init();

	d = opendir(path);

  // Iterate through files and directories
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
      // If the directory is a .git folder
      // This also means the directory we are searching in is a git repo
      // so we stop recursive search
			if (strcmp(dir->d_name, ".git") == 0) {
				if (verbose == 1) {
					printf("Found git repo!");
					printf("In: %s\n", d_path);
				}
				int len = strlen(d_path);
				d_path[len - 4] = '\0';       // Cut .git extension from output string
				sprintf(repos[(*repo_index)++], "%s", d_path);
				path_list_free(local_files);  // Delete the cached files
				printf("%s\n", d_path);
				return 0;
      // If the directory is not a .git folder
			} else {
				if (searchHidden == 1 ||
				    dir->d_name[0] != '.') {
          // Cache the directory for later traversal
					path_list_add(local_files, d_path);
				}
			}
		}
	}

  // All files and folders in this directory are checked, if no git repo was 
  // found we can recursively search the foulders that was found
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
