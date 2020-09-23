/*  Author: Dishank Goel
*   Date written: 21st August 2020
*   
*   cp.c implements the `cp` command in UNIX with -r option to copy directories
*   cp is used to copy files or directories to new places
*   Usage: ./cp [-r] SOURCE DEST\n");
*   or:    ./cp [-r] SOURCE... DIRECTORY\n");
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "util.h"

bool move_directory = false;        // check if -r option is supplied or not

int print_usage() {
    fprintf(stderr, "Usage: cp [-r] SOURCE DEST\n");
    fprintf(stderr, "or:    cp [-r] SOURCE... DIRECTORY\n");
    exit(EXIT_FAILURE);
}

/* copy_file - reads the content of one file, and create a new one to copy into
*/
int copy_file(char *old, char *new) {
    FILE *source, *target;
    char ch;
    source = fopen(old, "r");
    target = fopen(new, "w");
    if(source == NULL) { return -1; }
    if(target == NULL) { return -1; }
    while( (ch = fgetc(source)) != EOF) {
        fputc(ch, target);
    }
    fclose(source);
    fclose(target);
    return 0;
}

/*  copy_into_dir - copies a file into a directory
*   checks if the file being copied is a directory or normal file
*   stat_old is check_dir status of file being copied
*   new_path is the path inside the target directory where file is being copied
*/
int copy_into_dir(char *path, char *new_path, int stat_old) {
    int status;
    if(stat_old && stat_old != -1) {  // We have to copy a directory into another directory
        
        mkdir(new_path, 0755);     // We create the new directory if it does not exist inside the target directory
        /*  Read the contents of the older directory and copy each file into the new directory
        *   Do not copy any directories inside the source directory 
        */
        struct dirent **namelist;
        int n = scandir(path, &namelist, NULL, alphasort);
        int i = 2;      // Skip '.' and '..'
        while (i < n) {
            char *old_file = make_path(path, namelist[i]->d_name);
            int if_dir = check_dir(old_file);   // Checking the status of file in source directory
            if(if_dir && if_dir != -1) {        // If the this file of the older directory is a directory, do not copy it
                i++;
            } else {
                char *new_file = make_path(new_path, namelist[i]->d_name);
                status = copy_file(old_file, new_file);     // We copy the regular file into its new location
                i++;
                free(new_file);
            }
            free(namelist[i]);
            free(old_file);

        }
        free(namelist);

    } else {                        // We have to copy a file into target directory
        status = copy_file(path, new_path);
    }
    return status;

}

/*  copy - handles all the cases for moving either a file or dir into a (file or dir)
*   n is the check_dir status of target dir or file
*/
int copy(char *old, char *new, int n) {
    int result;
    int stat_old = check_dir(old);  
    if(stat_old && stat_old != -1) {    // The source file is a directory
        if(!move_directory) {       // Do not process a directory if -r is not specified
            fprintf(stderr, "cp: -r not specified; omiting directory '%s'\n", old);
            return -1;
        }
    } else if (stat_old == -1) {
        fprintf(stderr, "cp: cannot access '%s': No such file or directory\n", old);
        return -1;
    }
    if(n && n != -1) {  // The target path is a directory

        char *new_path = make_path(new, old);
        result = copy_into_dir(old, new_path, stat_old);
        free(new_path);

    }else if(!n && n != -1) {     // The target path is an existing file, check util.h for check_dir return values
        if(stat_old && stat_old != -1) {
            fprintf(stderr, "cp: cannot overwrite non-directory '%s' with directory '%s'\n", new, old);
            return -1;
        }else {
            result = copy_file(old, new);
        }

    } else {            // The target path does not exist as of now 

        if(stat_old && stat_old != -1) {        // if source file is a directory, move into a dir
            result = copy_into_dir(old, new, stat_old);
        }else {
            result = copy_file(old, new);       // otherwise simple copy source file in a new file
        }

    }

    // error handling
    if(result == 0) {
        return 0;
    } else if(result != -2) {
        fprintf(stderr, "cp: cannot access '%s': %s\n", new, strerror(errno));
        return -1;
    } else {
        return -1;
    }
    
}

int main(int argc, char *argv[])
{   
    /*  getopt is used to parse for flags (options) in command line tokens
    *   if there is an option -r, move_directory is set to true
    */
    int opt;
    while ((opt = getopt(argc, argv, "r")) != -1) {     // loop over all the options
        switch (opt) {
        case 'r': move_directory = true; break;
        default:
            print_usage();
        }
    }

    /* optind (declared extern int by <unistd.h>) is the index of the first non-option argument.
    If it is >= argc, there were no non-option arguments. */

    int num_nop_argument = argc - optind;       // number of non option arguments
    if(num_nop_argument <= 1) {         // Atleast two non argument options are required to copy
        print_usage();
        
    } else if(num_nop_argument == 2) {
        int n = check_dir(argv[argc - 1]);
        copy(argv[optind], argv[argc - 1], n);
    
    } else {
        int n = check_dir(argv[argc - 1]); 
        if(n && n != -1) {          // If multiple source dest are present, then target has to be a directory
            for(int i = optind; i < argc - 1; i++) {
                copy(argv[i], argv[argc - 1], n);
            }
        } else {
            fprintf(stderr, "cp: target '%s' is not a directory\n", argv[argc - 1]);
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);

}