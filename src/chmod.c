/*  Author: Dishank Goel
*   Date written: 21st August 2020
*   
*   chmod.c implements the `chmod` command in UNIX without any options
*   chmod is used to change permissions of a file
*   Usage: chmod MODE FILE...
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#define MAX_SHELL_PATH 1024

int print_usage() {
    printf("Usage: chmod MODE FILE...\n");
    printf("chmod is a utility to change the permission of a file\n");
    return 0;
}

/* change_permission - takes the decimal mode and file path to change its permissions
*/
int change_permission(char *file, char *mode) {
    int i;
    i = strtol(mode, NULL, 8);      // covert the decimal mode to octal
    /*  1st condition: if return value of strtol is 0 and mode itself is not 0
    *   2nd condition: the mode is not negative
    *   3rd condition: the mode is not greater than octal(7777) = 4095
    */
    if((i == 0 && atoi(mode) != 0) || (i < 0) || (i > 4095) ) {   
        fprintf(stderr, "chmod: invalid mode: '%s'\n", mode);
        exit(EXIT_FAILURE);
    }
    if(chmod(file, i) < 0) {
        fprintf(stderr, "chmod: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    if(argc == 1) {
        fprintf(stderr, "chmod: missing operand\n");
        print_usage();
        exit(EXIT_FAILURE);
    } else if(argc == 2) {
        fprintf(stderr, "missing operand after '%s'\n", argv[1]);
        print_usage();
        exit(EXIT_FAILURE);
    } else {
        int result = 1;     
        for(int i = 2; i < argc; i++) {
            if(change_permission(argv[i], argv[1]) == -1) {
                result = 0;     // if any error occurs, so that failed code is returned
            }
        }
        if(!result) {
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}