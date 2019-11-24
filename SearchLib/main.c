#define _GNU_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>


void expand_local_files(char **local_files, size_t *curr_length);
void free_local_files(char ***local_files, size_t index);
void add_local_file(char **local_files, char *file, size_t *curr_length, size_t *index);


int debug_allocation = 1;


void add_local_file(char **local_files, char *file, size_t *curr_length, size_t *index) {
  if (debug_allocation == 1) {
    printf("Adding\n");
  }
  if ( *index + 1 >= *curr_length ) {
    expand_local_files(local_files, curr_length);
  }
  if (debug_allocation == 1) {
    printf("Done expanding\n");
    printf("Adding new file %s, length %d, index %d\n", file, *curr_length, *index);
  }
  local_files[*index] = file;
  (*index)++;
  if (debug_allocation == 1) {
    printf("Done adding\n");
  }
}


void free_local_files(char ***local_files, size_t index) {
  if (debug_allocation == 1) {
    printf("Freeing\n");
  }
  for (int i = 0; i < index; i++) {
    free(*(local_files)[i]);
  }
  free(*local_files);
}


void expand_local_files(char **local_files, size_t *curr_length) {
  if (debug_allocation == 1) {
    printf("Expanding\n");
  }
  size_t expand_length = 10;
  char **tmp = malloc(*curr_length + expand_length * sizeof(char *));
  if (tmp == NULL) {
    printf("Error when trying to allocate expanded memory for local files\n");
  }
  for (int i = 0; i < *curr_length; i++) {
    tmp[i] = (local_files)[i];
  }
  free(*local_files);
  local_files = tmp;
  *curr_length += expand_length;
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

  char **local_files; 
  size_t local_files_size = 10;
  size_t index = 0;

  local_files = malloc(local_files_size * sizeof(char*));

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
			char *d_path = malloc(255*sizeof(char));
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
        free_local_files(&local_files, index);
				printf("%s\n", d_path);
        return 0;
			} else {
				if (searchHidden == 1 || dir->d_name[0] != '.') {
          add_local_file(local_files, d_path, &local_files_size, &index);
				}
			}
		} else {
			if (verbose == 1) {
				printf("not regular file, could be symlink etc: %s\n",
				       dir->d_name);
			}
		}
	}

  for (int i = 0; i < index; i++) {
		search(local_files[i], verbose, searchHidden,
		  repos, repo_index);
  }

	closedir(d);
  free(local_files);
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
