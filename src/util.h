/*  Author: Dishank Goel
*   Date written: 21st August 2020
*
*   This is a header supporting other files which common funtions
*/   

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

// Declares some common ANSI colors

#define GREEN "\x1b[92m"    
#define BLUE "\x1b[94m"
#define CYAN "\x1b[96m"
#define RED "\x1b[1;31m"
#define YELLOW "\x1b[1;33m"
#define RESET "\033[0m"
#define PURPLE "\033[0;35m"

#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_PURPLE "\033[1;35m"
#define BOLD_CYAN "\033[1;36m"

/*  make_path - It creates a path from source dir and the file
*   Comes handy in creating paths when files are being copied or moved
*   returns the new path
*/
char *make_path(char *dir, char *file) {
    char *new_path;
    new_path = malloc((1 + strlen(file) + strlen(dir) + 1 + 2)*sizeof(char));
    if(new_path != NULL) {
        strcpy(new_path, dir);
        strcat(new_path, "/");
        strcat(new_path, file);
    }
    return new_path;
}

/*  check_dir - this function checks the status of a file about:
*   1. if the file is a directory: return true (Use the condition: n && n != -1)
*   2. if the file is a normal file: returns false (Use the condition: !n && n != -1)
*   3. if the file does not exist return -1
*/
int check_dir(char *filename) {
    struct stat statbuf;
    if(stat(filename, &statbuf) == -1) {
        return -1;
    }else {
        return S_ISDIR(statbuf.st_mode);
    }
}

/* check_executable - returns 1 if the file is an executable
*/
int check_executable(char *filename) {
    struct stat statbuf;
    if((stat(filename, &statbuf) == 0) && statbuf.st_mode & S_IXUSR) {
        return 1;
    } else {
        return 0;
    }
}

/*  print_color_string - given a color, prints the given string in that color
*/
void print_color_string(char *to_print, char *color) {
    printf("%s", color);
    printf("%s", to_print);
    printf("%s", RESET);
}