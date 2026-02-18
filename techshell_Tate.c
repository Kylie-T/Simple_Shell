
// Kylie Tate

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>

#define MAX_IN 1024
#define MAX_ARGS 6

void prompt() {
    // passing NULL and 0 which allocates the needed memory 
    // instead of providing a buffer that is already allocated
    char *cwd = getcwd(NULL, 0);
    if (cwd != NULL) {
        printf("%s$ ", cwd);
        fflush(stdout);
        // frees up memory when no longer needs; prevents memory leaks
        free(cwd);
    } else { perror("getcwd() error"); }
}

char *input(){
    // allocate memory for variable to hold user input
    char *userInput = malloc(MAX_IN);

    // Checks if malloc was successful or returned null pointer
    if (!userInput) { 
        perror("malloc() error"); 
        return NULL;
    }
    // Checks if fgets returns a null pointer when trying to read user input
    if (fgets(userInput, MAX_IN, stdin) == NULL) { 
        perror("fgets() error"); 
        free(userInput);
        return NULL;
    }
    return userInput;
}

void inputAction(char *userInput) {
    // Gets rid of the inline character after user presses enter
    userInput[strcspn(userInput, "\n")] = '\0';

    // Turn user input into tokens
    char *args[MAX_ARGS] = { NULL };
    char *tokens;
    tokens = strtok(userInput, " \t\r\n\a");

    // if tokens is NULL, the function stops
    if (tokens == NULL){ return; }

    int i = 0;
    while (tokens != NULL && i < MAX_ARGS) { 
        args[i++] = tokens;
        tokens = strtok(NULL, " \t\r\n\a"); 
    }

    // stops program if user types exit
    if (!strcmp(args[0], "exit")) { exit(EXIT_SUCCESS); }

    // cd handling
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            char *home = getenv("HOME"); 
        if (home && chdir(home) == 0){
            return;
        }

        fprintf(stderr, "Error 2 (No such file or directory)\n");
    } else {
        if (chdir(args[1]) != 0) {
            if(errno == EACCES){
                fprintf(stderr, "ERROR 3 (denied permissions)\n");
            }else if(errno == ENOENT){
                fprintf(stderr, "ERROR 2 (NO such file or directory)\n");
            }else{
                fprintf(stderr, "ERROR 1 (invalid command)\n");
            }
            
        }
    } 
        return;
    }
    // redirecting
        char *inFile = NULL;
        char *outFile = NULL;
        char *appFile = NULL;

        for (int i = 0; args[i] != NULL; i++) {
            // Check for input redirection "<"
            if (strcmp(args[i], "<") == 0) {
                if (args[i + 1] == NULL) { // No filename after "<"
                    perror("No file specified for input redirection.");
                    return;
                }
                
                inFile = args[i+1];
                // Remove the redirection operator and the filename from the args list
                args[i] = NULL;
                i++; // Skip the filename
            }
            // Check for output redirection ">"
            else if (strcmp(args[i], ">") == 0) {
                if (args[i + 1] == NULL) { // No filename after ">"
                    perror("No file specified for output redirection.");
                    return;
                }
                outFile = args[i+1];
                args[i] = NULL;
                i++; // Skip the filename
            }
            // Check for append redirection ">>"
            else if (strcmp(args[i], ">>") == 0) {
                if (args[i + 1] == NULL) { // No filename after ">>"
                    perror("No file specified for append redirection.");
                    return;
                }
                appFile = args[i+1];
                args[i] = NULL;
                i++; // Skip the filename
            }
            // Check for error redirection "2>"
            else if (strcmp(args[i], "2>") == 0) {
                if (args[i + 1] == NULL) { // No filename after "2>"
                    perror("No file specified for error redirection.");
                    return;
                }
                int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("open failed");
                    return;
                }
                dup2(fd, STDERR_FILENO);
                close(fd);

                args[i] = NULL;
                i++; // Skip the filename
            }
    // If no valid redirection operator, continue processing
}

    pid_t pid = fork();

    if (pid == 0) {  // if true, then it is a child process
        if (inFile != NULL) {
            int fd = open(inFile, O_RDONLY | O_CREAT);
            if (fd < 0) {
                perror("open failed1");
                return;
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (outFile != NULL) {
            int fd = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open failed2");
                return;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (appFile != NULL) {
            int fd = open(appFile, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror("open failed2");
                return;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // if true, then it is the parent
        int status;
        waitpid(pid, &status, 0);
    } else { perror("fork failed"); }
}

int main() {

    while (1) {
        prompt();

        char *usrIn = input();
        if (usrIn == NULL) { continue; }
        inputAction(usrIn);
        // main allocates the memory when calling on the input function
        // so free here
        free(usrIn);
    }
    return 0;
}