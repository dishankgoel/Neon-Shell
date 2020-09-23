/*  Author: Dishank Goel
*   Date written: 21st August 2020
*   
*   rm.c implements the `rm` command in UNIX with -r option
*   rm is used to delete files or directories
*   Usage: rm [-r] [FILE]...
*/


#define _XOPEN_SOURCE 500       // Declared for nftw, traversing the tree strcuture of a directory 
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ftw.h>
#include "util.h"

bool remove_directory = false;      // checks if -r option is supplied

int print_usage() {
    fprintf(stderr, "Usage: rm [-r] [FILE]...\n");
    exit(EXIT_FAILURE);
}

/*  remove_file - this function is called for each file while traversing the directory tree using nftw
*   path is path of the file being removed
*   typeflag is the type of file being processed, check man 3 nftw
*/
int remove_file(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {

    int n = remove(path);       // can remove files and empty directories
    if(n) {
        fprintf(stderr, "rm: cannot remove '%s': %s\n", path, strerror(errno));
        return -1;
    }
    return 0;
}

/* traverse_directory - recursively traverse the directory in depth first fashion
*/
int traverse_directory(char *path) {
    
    /*  64 is the maximum number of directories that can be opened at once, 
    *   FTW_DEPTH specifies to traverse post order tree walk, so that files are deleted first,
    *   and then empty directories
    */
    return nftw(path, remove_file, 64, FTW_DEPTH | FTW_PHYS);
}

int main(int argc, char *argv[])
{
    /*  for parsing the -r option 
    */
    int opt;
    while ((opt = getopt(argc, argv, "r")) != -1) {
        switch (opt) {
        case 'r': remove_directory = true; break;
        default:
            print_usage();
        }
    }

    /* optind (declared extern int by <unistd.h>) is the index of the first non-option argument.
    If it is >= argc, there were no non-option arguments. */

    int any_error = 0;
    int num_nop_argument = argc - optind;   // number of non option arguments
    if(num_nop_argument <= 0) {
        print_usage();
    } else {
        for(int i = optind; i < argc; i++) {        // loop over all the non option arguments
            int n = check_dir(argv[i]);
            if(n && n != -1) {      // the target file is a directory
                if(remove_directory) {      // if -r option is specified
                    traverse_directory(argv[i]);  
                } else {
                    fprintf(stderr, "rm: -r not specified; omiting directory '%s'\n", argv[i]);
                    any_error = 1;      // We detected an error
                }
            } else if (!n && n != -1) {     // the target file is a normal file
                struct stat buf;
                struct FTW ftwbuf;
                if(remove_file(argv[i], &buf, FTW_F, &ftwbuf) == -1) {
                    any_error = 1;      // there was an error removing the file, error printing will be handled by remove_file
                }
            } else {        // the file does not exist
                fprintf(stderr, "rm: cannot remove '%s': No such file or directory\n", argv[i]);
                any_error = 1;
            }
        }
        if(any_error) {
            exit(EXIT_FAILURE);
        }
        
    }
    exit(EXIT_SUCCESS);

}