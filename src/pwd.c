/*  Author: Dishank Goel
*   Date written: 21st August 2020
*   
*   pwd.c implements the `pwd` command in UNIX without any options
*   pwd is used to get the current working path in a shell
*   Usage: ./pwd
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MAX_SHELL_PATH 1024

int main(int argc, char *argv[]) {

    if(argc > 1) {
        fprintf(stderr, "pwd: too many arguments\n");   // pwd does not work if multiple arguments are given
        exit(EXIT_FAILURE);
    } else {
        char *buf;
        buf = malloc(MAX_SHELL_PATH * sizeof(char));
        if((getcwd(buf, MAX_SHELL_PATH)) != NULL) {     // getcwd is in unistd.h
            printf("%s\n", buf);
            free(buf);
        } else {
            fprintf(stderr, "pwd: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}