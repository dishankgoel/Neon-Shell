/*  Author: Dishank Goel
*   Date written: 21st August 2020
*   
*   This is a minimal UNIX shell implementation in C called Neon Shell
*   Neon Shell comes from the stable shells of Neon Noble gas and the colors too ;)
*   
*   This implementation is part of the course Operating System Fall 2020 at IIT Gandhinagar
*   There are 3 main requirements that this shell satisfies:
*
*   1. Run inbuilt binaries (like ps, pmap, wget, etc.) with arguments
*   2. Implementation of commands (unless specified, do not implement any options like (-l, -a)):
*       -> ls (Argument is either directory name or no argument at all) 
*       -> grep (requirements are same as those for wgrep given here: Link)
*       -> cat (requirements are same as those for wcat given here: Link)
*       -> Mv
*       -> Cp
*       -> Cd
*       -> Pwd
*       -> rm (along with -r option)
*       -> Chmod
*       -> Mkdir
*   3. Can run programs in background using & at the end
*   
*   To run shell, execute
*   make; ./shell;
*   inside the project directory 
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pwd.h>
#include "util.h"

#define MAX_COMMAND_LENGTH 49152
#define MAX_SHELL_PATH 4096
#define MAX_ARGVAL 4096
#define MAX_ARGC 12

char *shell_path;       // Stores where the shell is installed, to find the inbuilt binaries
char *prompt;           // Stores the current working dir relative to HOME for the prompt
char *home_path;        // The path in HOME variable
char *user_name;        // The username of the user calling the shell
char *hostpc_name;      // The pc name of the user calling the shell
char *self_implemented_binaries[] = {"ls", "grep", "cat", "mv", "cp", "pwd", "rm", "chmod", "mkdir"};

int run_in_background;      // if the process has to be run in background
int background_process_counter;     // how many programs have been run in backgound


/*  relative_path_from_home - converts a absolute path relative to the home path if 
*   the path is in ~/, else gives back the absolute path
*   for example, /home/dishank/sem5 is changed to ~/sem5
*/
char* relative_path_from_home(char *absolute_path) {
    int path_len = strlen(absolute_path);
    int home_len = strlen(home_path);
    if(path_len < home_len) {       // if the path is shorter than home path
        return absolute_path;
    }else {
        if(strncmp(absolute_path, home_path, home_len) == 0) {      // The absolute should start with the home path
            
            char *relative_path = malloc(MAX_SHELL_PATH*sizeof(char));
            strcpy(relative_path, "~");     // Now the home path will be denoted by ~
            if(home_len != path_len) {
                strncat(relative_path, absolute_path + home_len, absolute_path - home_path);    // Append the rest of the absolute path
            }
            return relative_path;
        }else {
            return absolute_path;
        }
        
    }
}

/*  cd - A shell command for changing shell path
*   Usage: cd DIRECTORY
*/
int cd(char *path) {
    int result = chdir(path);
    if(result != 0) {
        fprintf(stderr, "cd: error in changing to '%s': %s\n", path, strerror(errno));
        return -1;
    
    } else {    
        // update the prompt after changing the directory
        char *buf = malloc(MAX_SHELL_PATH * sizeof(char));
        prompt = getcwd(buf, MAX_SHELL_PATH);
        prompt = relative_path_from_home(prompt);
    }
    return 0;
}

/*  exit_shell - A shell command for exiting the shell
*   Usage: exit
*/
int exit_shell() {
    exit(EXIT_SUCCESS);
}

/*  print_prompt - Prints the shell line that has username, pc name and the current working directory
*   Neon colors!
*/
int print_prompt(char *prompt) {

    fflush(stdout);     // Any bad characters are flushed out
    print_color_string(user_name, BOLD_PURPLE);
    print_color_string("@", BOLD_PURPLE);
    print_color_string(hostpc_name, BOLD_PURPLE);
    printf(":");
    print_color_string(prompt, BOLD_CYAN);

    // If the user is root, show a # instead of usual $
    if(strcmp(user_name, "root") == 0) {
        printf("# ");
    } else {
        printf("$ ");
    }
    return 0;
}

/*  check_self_implemented - checks if the requested command has been implemented by me
*   currently, the commands in self_implemented_binaries[] are self implemented 
*/
int check_self_implemented(char *program) {
    
    for(int i = 0; i < 9; i++) {

        if(strcmp(self_implemented_binaries[i], program) == 0) {
            return 1;
        }
    }
    return 0;
}

/*  parse_command - splits the input line using ' ' delimiter and creates the argv and argc
*   for the new process
*/
int parse_command(char *line, char **command_argv, int *command_argc) {

    char *val = malloc(MAX_ARGVAL*sizeof(char));    // will contain the separated token 
    char *str1, *saveptr;
    int j = 1;

    // Specified in the man page of strtok
    for(j = 1, str1 = line; j <= MAX_ARGC ; j++, str1 = NULL) {
        val = strtok_r(str1, " ", &saveptr);
        if(val == NULL) {
            break;
        }
        command_argv[j - 1] = malloc(MAX_ARGVAL*sizeof(char));
        strcpy(command_argv[j - 1], val);       // copy the token in command_argv in same order
    }
    *command_argc = j - 1;      // the number of arguments
    
    /*  If the last argument is &, then don't count it
    */
    if(strcmp(command_argv[*command_argc - 1], "&") == 0) {
        run_in_background = 1;
        command_argv[*command_argc - 1] = NULL;
        *command_argc = *command_argc - 1;
    } else {
        command_argv[*command_argc] = NULL;     //  execvp requires the ending of args by NULL 
    }
    
    return 0;
}

/*  exec_command - creates a new process by fork() and executes our given command using execvp
*/
int exec_command(char *argv[], int argc) {

    int child_pid = fork();
    if(child_pid == -1) {       // there was a fork error
        fprintf(stderr, "neosh: fork: %s\n", strerror(errno));
        return -1;
    }
    int wstatus, w;     // track the status of the child process in the parent process
    if(child_pid == 0) {    // child process
        execvp(argv[0], argv);
        fprintf(stderr, "neosh: command not found: %s\n", argv[0]);     // if the child reaches here, then there was an error in execvp 
        fflush(stderr);
        kill(getpid(), SIGUSR1);        // kill the child process
        return -1;

    } else {           // parent process
        if(!run_in_background) {        // if the program is not being run in background, wait for child to finish
            
            //  WUNTRACED -> If a child has been stopped, return from it 
            w = waitpid(child_pid, &wstatus, WUNTRACED);
            if(w == -1) {
                perror("waitpid");
            }
        }else { // the process is being run in the background, so don't wait
            printf("[%d] %d\n", background_process_counter, child_pid);
            background_process_counter++;
        }
        return 0;
    }
    return 0;
}

/*  take_line_input - takes the line input from user
*   The newline at the end from fgets is stripped
*/
int take_line_input(char line[]) {
    
    fflush(stdout);
    if(fgets(line, MAX_COMMAND_LENGTH, stdin)) {
        line[strcspn(line, "\r\n")] = 0;    // strip the newline at the end
        return 0;
    }else {
        return -1;
    }
}

/*  initialize_shell - is called when the shell starts
*   sets up the shell path, usernames, host names, and current working directory
*/
int initialize_shell() {

    shell_path = malloc(MAX_SHELL_PATH * sizeof(char));
    prompt = malloc(MAX_SHELL_PATH * sizeof(char));
    home_path = malloc(MAX_SHELL_PATH * sizeof(char));
    user_name = malloc(1024 * sizeof(char));
    hostpc_name = malloc(1024 * sizeof(char));

    background_process_counter = 1;

    //  If something went wrong and path could not be set, then exit
    if(shell_path == NULL || prompt == NULL) {
        fprintf(stderr, "neosh: Could not allocate memory for shell: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct passwd *pw;      // for reading /etc/passwd to get the username
    uid_t uid;
    uid = geteuid();        // Use the effective user id
    pw = getpwuid(uid);

    if((home_path = getenv("HOME")) == NULL) {
        if(pw) {
            home_path = pw->pw_dir;     
        }
    }
    if(pw) {
        user_name = pw->pw_name;
    }
    gethostname(hostpc_name, HOST_NAME_MAX);     // stores the PC name in hostpc_name

    char *buf;
    buf = malloc(MAX_SHELL_PATH * sizeof(char));
    if((shell_path = getcwd(buf, MAX_SHELL_PATH)) == NULL) {
        
        fprintf(stderr, "neosh: Could not set the path for shell: %s\n", strerror(errno));
        exit(EXIT_FAILURE);

    }else {
        strcpy(prompt, shell_path);     // intially, the prompt is the shell path
        prompt = relative_path_from_home(prompt);   // update it relative to the home
        return 0;
    }

}

/*  run_shell - main loop which prints the prompt and accepts the user input
*   This is the master loop which spawns new processes to execute the commands
*/
int run_shell() {

    while(1) {

        char line[MAX_COMMAND_LENGTH];
        char *command_argv[MAX_ARGC + 1];
        int command_argc;

        print_prompt(prompt);       // show user the shell prompt
        take_line_input(line);      // take the input
        
        run_in_background = 0;
        
        if(strcmp(line, "") == 0) {     // If '\n' is entered
            continue;
        }

        parse_command(line, command_argv, &command_argc);
        
        // After parsing the command, command_argv and command_argc are set so that new process can start
        if(strcmp(command_argv[0], "exit") == 0) {  // handle exit by the shell  
            if(command_argc == 1) {
                exit_shell();
            }else {
                fprintf(stderr, "exit: too many arguments\n");
            }
        } else if(strcmp(command_argv[0], "cd") == 0) {     // handle cd by the shell
            if(command_argc == 1) {
                cd(home_path);
            } else if(command_argc == 2) {
                cd(command_argv[1]);
            } else {
                fprintf(stderr, "cd: too many arguments\n");
            }

        } else if (check_self_implemented(command_argv[0])) {       // if the command is implemented by us
            /*  new_binary_path has the path of binary implemented by us
            *   which is in shell_path/bin/
            */
            char *new_binary_path = make_path(shell_path, make_path("bin", strdup(command_argv[0])));
            command_argv[0] = strdup(new_binary_path);
            exec_command(command_argv, command_argc);

        } else {
            /*  try to execute the command normally if it is installed on the system  
            */
            exec_command(command_argv, command_argc);
        }
        
    }
    return 0;
}

int main(int argc, char *argv[]) {
    initialize_shell();
    run_shell();
    exit(EXIT_SUCCESS);
}