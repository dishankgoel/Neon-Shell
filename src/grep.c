/*  Author: Dishank Goel
*   Date written: 21st August 2020
*   
*   grep.c implements the `grep` command in UNIX without any options
*   grep is used to search for patterns in text
*   this implementation runs in O(m*n) m being text size, n is pattern length
*   if no file is given, then grep takes input from stdin
*   Usage: ./grep PATTERN [FILE]...
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include "util.h"

int multiple_args;

/*  process_line - for each given to this function, it checks if there is a match
*   if there is a match, it colors the match in the line and prints it
*/
int process_line(char *pattern, char *line, char *file) {
    int match_found = 0;        // if any printing is required
    int n = strlen(pattern);
    int m = strlen(line);
    int last_match = 0;         // stores where the last match ended, so that we can print white from there to current match
    
    /*  loops over entire string and finds match 
    *   O(m*n) naive matching algorithm
    */
    for(int i = 0; i < m - n; i++) {
        int match = 1;
        for(int j = 0; j < n; j++) {        // if there is a match starting at the ith position
            if (pattern[j] != line[i + j] ) {
                match = 0;
                break;
            }
        }
        if (match == 1) {       // if there was a match

            /*  if there are multiple files, print the filename like in UNIX grep
            *   print it only once per line */
            if(match_found == 0 && multiple_args) {      
                print_color_string(file, PURPLE);   // print_color_string is in util.h
                print_color_string(":", CYAN);
            }
            /*  print the string from ending of last match to the starting of current match in white */
            printf("%.*s", i - last_match, line + last_match);
            /*  then print the matched pattern */  
            print_color_string(pattern, RED);
            last_match = i + n;
            match_found = 1;
        }
    }
    if (match_found) {      // print the remaining line 
        printf("%.*s", m - last_match, line + last_match);
    }

    return 0;

}

/*  handle_file - opens the file contents and reports any error while reading contents
*   special case if file is directory are checked
*/

int handle_file(char *pattern, char *file) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        fprintf(stderr, "grep: cannot open '%s': %s\n", file, strerror(errno));
        exit(EXIT_FAILURE);     // exit as soon as a file cannot be opened, mentioned in wgrep
    } else {
        if (check_dir(file)) {
            fprintf(stderr, "grep: cannot read '%s': Is a directory\n", file);
        } else {
            char *buffer = NULL;
            size_t n;           //  Will contain the allocated size of the buffer
            ssize_t nread;      //  The number of characters read from current line
            while ((nread = getline(&buffer, &n, fp)) != -1) {
                if (buffer != NULL) {
                    process_line(pattern, buffer, file);    // process the fetched line (find matches)
                }  
            }
            free(buffer);
        }
    }
    fclose(fp);
    return 0;
}

/* grep_stdin - special case if no file is given, then open stdin and process the line
*/
int grep_stdin(char *pattern) {
    while (1) {
        char line[4096];
        if(fgets(line, 4096, stdin)) {
            process_line(pattern, line, "");
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    multiple_args = 0;
    if(argc == 1) {
        fprintf(stderr, "Usage: grep PATTERN [FILE]...\n");
        exit(EXIT_FAILURE);
    } else if (argc == 2) {
        if(strcmp(argv[1], "\"\"") == 0) {      // if "" is given as the pattern, we treat it like empty string
            grep_stdin("");
        } else {
            grep_stdin(argv[1]);
        }
        
    } else {
        if (argc > 3) {
            multiple_args = 1;
        }
        if(strcmp(argv[1], "\"\"") == 0) {          // Separate case for treating "" as an empty string
            for(int i = 2; i < argc; i++) {
                handle_file("", argv[i]);
            }
        } else {
            for(int i = 2; i < argc; i++) {
                handle_file(argv[1], argv[i]);
            }
        }
    }
    exit(EXIT_SUCCESS);
}
