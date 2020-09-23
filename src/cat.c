/*  Author: Dishank Goel
*   Date written: 21st August 2020
*   
*   cat.c implements the `cat` command in UNIX without any options
*   it concatenates the content of multiple files given as args and prints them on stdout
*   if no parameter is given, cat.c does nothing instead of reading from stdin
*   Usage: ./cat [FILE]...
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include "util.h"

/*  print_file - takes the file name and prints all it's content
*   handles errors when file is not accessible, or is a directory 
*/
int print_file(char *file) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        fprintf(stderr, "cat: cannot open '%s': %s\n", file, strerror(errno));
        exit(EXIT_FAILURE);     // exit as soon as file cannot be opened, mentioned in wcat
    } else {
        if(check_dir(file)) {       // check_dir is in util.h
            fprintf(stderr, "cat: cannot read '%s': Is a directory\n", file);
        } else {
            char *buffer;
            buffer = malloc(4096*sizeof(char));
            while (fgets(buffer, 4096, fp) != NULL) {
                printf("%s", buffer);

                /*  If the file does not end in newline,
                *   the buffer is not flushed and the order of printing maybe wrong with multiple arguments */
                fflush(stdout);  
            }
            free(buffer);
        }
    }
    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {

    for(int i = 1; i < argc; i++) {
        print_file(argv[i]);
    }
    exit(EXIT_SUCCESS);
}