#define _GNU_SOURCE

#include <dirent.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h> 



int search(char* path, int verbose, int searchHidden, char** repos, int *repo_index) {
  if (verbose) {
    printf("\n---------\nSearching in directory: %s\n", path);
  }

  DIR *d;
  struct dirent *dir;
  char buf[PATH_MAX + 1]; 

  d = opendir(path);

  // Go through files and folders in this directory
  while (d && (dir = readdir(d)) != NULL) {
    realpath(dir->d_name, buf);
    if (dir->d_type == DT_REG) {
      if (verbose == 1) {
        printf ("[%s] ", buf);
        printf("Regular file: %s\n", dir->d_name);
      }
    }
    // If it is a directory
    if(dir->d_type == DT_DIR && strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0 ) {
      if (verbose == 1) {
        printf ("[%s] ", buf);
        printf("Directory: %s\n", dir->d_name);
      }
      char d_path[255];
      sprintf(d_path, "%s/%s", path, dir->d_name);
      if (strcmp(dir->d_name, ".git") == 0) {
        if (verbose == 1) {
          printf("Found git repo!");
          printf("In: %s\n", d_path);
        }
        int len = strlen(d_path);
        d_path[len - 4] = '\0';
        sprintf(repos[(*repo_index)++], "%s", d_path);
        printf("%s\n", d_path);
      } else {
        if (searchHidden == 1) {
          search(d_path, verbose, searchHidden, repos, repo_index);
        }
        else if (dir->d_name[0] != '.') {
          search(d_path, verbose, searchHidden, repos, repo_index);
        }
      }
    }
    else {
      if (verbose == 1) {
        printf("not regular file, could be symlink etc: %s\n", dir->d_name);
      }
    }
  }
  closedir(d);
  return 0;
}

int main(int argc, char* argv[])
{
  int i;
  int verbose = 0;
  int searchHidden = 0;
  char* path = ".";
  if (argc > 1) {
    if (argv[1][0] != '-') {
      path = argv[1];
    }
  }
  for(i=0; i<argc; ++i)
  {
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

  char** repos;
  repos = malloc(1000*sizeof(char *));
  for (int i = 0; i < 1000; i++) {
    repos[i] = malloc(1000*sizeof(char));
  }
  int repo_index = 0;

  search(path, verbose, searchHidden, repos, &repo_index);

  return(0);
}




