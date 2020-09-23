/*  Author: Dishank Goel
*   Date written: 21st August 2020
*   
*   mv.c implements the `mv` command in UNIX without any options
*   mv either renames the file to a new file, or moves a file or directory into another directory
*   Usage: ./mv SOURCE DESTINATION
*   or:    mv SOURCE(s) DIRECTORY
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "util.h"

/* move - given an old file [old], and a new destination [new] (either new name or existing directory)
*  either renames the old file to new name, or moves it into the directory
*  n is check_dir status for the new file, check util.h for check_dir return status
*/
int move(char *old, char *new, int n) {
    int result;
    if(n && n != -1) {  // New file is directory
        char *new_path;
        new_path = make_path(new, old);     // the new path will be inside the new folder with old name
        result = rename(old, new_path);     // renames the old file to new path
        if(result == 0) {
            return 0;
        } else {
            fprintf(stderr, "mv: cannot move '%s' to '%s': %s\n", old, new_path, strerror(errno));
            return -1;
        }

    }else {     // New path is not a directory, so it either a new name, or existing file
        result = rename(old, new);      // overwrites the new file if it already exists 
        if(result == 0) {
            return 0;
        } else {
            fprintf(stderr, "mv: cannot move '%s' to '%s': %s\n", old, new, strerror(errno));
            return -1;
        }
    }
    
}


int main(int argc, char *argv[]) {

    if(argc < 3) {
        fprintf(stderr, "mv: missing operands\n");
        printf("Usage: mv SOURCE DESTINATION\n");
        printf("or:    mv SOURCE(s) DIRECTORY\n");
        printf("A utility to Rename source to destination, or Move source(s) to directory\n");
        exit(EXIT_FAILURE);
    } else if(argc == 3) {          // if only ./mv OLD NEW is given, then NEW can be a file or directory
        int n = check_dir(argv[2]);
        move(argv[1], argv[2], n);
    } else {
        int n = check_dir(argv[argc - 1]); 
        if(n && n != -1) {          // if there are >2 arguments, then the last has to be a directory 
            for(int i = 1; i < argc - 1; i++) {
                move(argv[i], argv[argc - 1], n);
            }
        } else {
            fprintf(stderr, "mv: target '%s' is not a directory\n", argv[argc - 1]);
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}