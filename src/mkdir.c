/*  Author: Dishank Goel
*   Date written: 21st August 2020
*   
*   mkdir.c implements the `mkdir` command in UNIX without any options
*   mkdir is used to create new directories if they don't exist
*   Usage: ./mkdir DIRECTORY...
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


int print_usage() {
    printf("Usage: mkdir DIRECTORY...\n");
    printf("mkdir is a utility to create directory(ies), if they do not exist.\n");
    return 0;
}

/*  create_directory - simply calls mkdir over the directory, with default mode drwxr-xr-x
*/
int create_directory(char *file) {

    int result = mkdir(file, 0755);
    if(result < 0) {
        fprintf(stderr, "mkdir: cannot create directory '%s': %s\n", file, strerror(errno));
        return -1;
    } 
    return 0;
}

int main(int argc, char *argv[]) {

    if(argc == 1) {
        fprintf(stderr, "mkdir: missing operand\n");
        print_usage();
        exit(EXIT_FAILURE);
    } else {
        int result = 1;
        for(int i = 1; i < argc; i++) {
            if(create_directory(argv[i]) == -1) {
                result = 0;     // if there was any error, exit with failure
            }
        }
        if(!result) {
            exit(EXIT_FAILURE);
        }

    }
    exit(EXIT_SUCCESS);
}
