/*  Author: Dishank Goel
*   Date written: 21st August 2020
*   
*   ls.c implements the shell command `ls` for listing contents of a directory.
*   Currently, it takes no option to list contents in long list format like -a -l
*   Usage: ./ls [DIRECTORY]...
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include "util.h"

int multiple_arg;       // Will be used to find if there are multiple directories in arguments
struct winsize w;       // To get the size of terminal emulator calling the shell, so that output can be pretty


/*  find_col_length - The size of column in which output will be stacked 
*   takes the list of contents [namelist], and the number of files [n] as arguments
*/
int find_col_length(struct dirent **namelist, int n) {
    int max_width_name = 0;         // The max length among all the files to be printed
    int i = 2;                // Starting from i = 2 to skip '.' and '..'
    while(i < n) {
        char *name = namelist[i]->d_name;
        if(name[0] != '.') {
            if(max_width_name < strlen(namelist[i]->d_name)) {
                max_width_name = strlen(namelist[i]->d_name);
            }
        }
        i++;
    }
    return max_width_name + 3;          // An offset of 3 to make some space among the columns
}

/*  print_space_repeatedly - printing spaces as much as I want for pretty formatting output
*/
int print_space_repeatedly(int count) {
    for(int i = 0; i < count; i++) {
        printf(" ");
    }
    return 0;
}

/*  print_name - Depending on the type of file, prints corresponding coloured output for that file
*   executables -> green
*   directories -> blue
*   normal files -> white
*/
int print_name(char *dir, char *name) {
    char *full_path = make_path(dir, name);     // make_path is in util.h, it concatenates dir with name, returns dir/name
    int is_dir = check_dir(full_path);          // check_dir is in util.h, it checks if file is directory, normal file, or doesn't exist
    int is_exec = check_executable(full_path);  // check executable is similar to check_dir, execept it check the exec bit
    
    /* is_dir should be true not -1 if it is a directory, read more in util.h */
    if(is_dir && is_dir != -1) {
        print_color_string(name, BOLD_BLUE);    // print_color_string is in util.h
    }else if(is_exec) {     
        print_color_string(name, BOLD_GREEN);
    } else {
        printf("%s", name);
    }
        
    return 0;
}
/*  print_contents - responsible for printing all the contents given by ls
*   takes the directory path [directory], all the names of files [namelist], and number of files [n]
*/
int print_contents(char *directory ,struct dirent **namelist, int n) {
    int i = 2;              // skipping '.' and '..'
    int file_no = 1;        // track of file number being printed
    int col_size = find_col_length(namelist, n);
    while (i < n) {
        char *name = namelist[i]->d_name;       // name of the file being printed
        if(name[0] != '.') {                    // skip all hidden files or directories
            /*  |           |          |   |
            *   |           |          |   |
            *   |           |          |   |
            *   |           |          |   |
            *   <--col_size->          <    > not print in this leftover space
            */
            int space_size = col_size - strlen(name);   // remaning column length to be filled out by spaces

            // find number of columns to be printed, remove the leftover space, and divide by column size
            int num_cols = (w.ws_col - (w.ws_col%col_size))/col_size;       
            print_name(directory, name);            
            print_space_repeatedly(space_size);
            if((file_no) % (num_cols) == 0) {   // if the next file is being printed in the leftover space, then start from newline
                printf("\n");
            }
            file_no++;
        }
        free(namelist[i]);
        i++;
    }
    if (i > 2) {            // If there were any files that were printed, then come to newline after printing them
        printf("\n");
    }
    if(multiple_arg){       // More directories are being printed, so newline for them
        printf("\n");
    }
    
    return 0;
}

/*  list_contents - accesses the filesystem to get the contents and handle the errors
*   needs only the path from where contents are being listed
*/
int list_contents(char *directory) {
    
    struct dirent **namelist;
    int n;
    int result = check_dir(directory);
    if(result && result != -1){         // the given path is a directory, read more in util.h for output of check_dir
        n = scandir(directory, &namelist, NULL, alphasort);     // scandir reads and sorts the file contents alphabetically 
        if (n == -1) {
            fprintf(stderr, "ls: cannot access '%s': %s\n", directory, strerror(errno));
            return -1;
        }
        if(multiple_arg){           // if there are multiple directories, then print their name 
            printf("%s:\n", directory);
        }
        print_contents(directory, namelist, n);
        free(namelist);
        return 0;

    } else if(!result && result != -1) {      // the given path is a regular file
        printf("%s\n", directory);
        return 0;

    } else {        // the path does not exist
        fprintf(stderr, "ls: cannot access '%s': %s\n", directory, strerror(errno));
        return -1;
    }
}


int main(int argc, char *argv[]) {

    multiple_arg = 0;
    /* ioctl is used to control devices, in this case, we are accessing pts (terminal session) to find the width of terminal */
    ioctl(0, TIOCGWINSZ, &w);       
    if(argc > 2){
        multiple_arg = 1;
        for(int i = 1; i < argc; i++) {
            list_contents(argv[i]);         // list all the contents specified
        }
    } else if(argc == 2) {
        list_contents(argv[1]);
    } else {
        list_contents(".");     // if no argument, list the current directory
    }
    exit(EXIT_SUCCESS);
}