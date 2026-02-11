// Kylie Tate

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#define MAX_IN 1024
#define MAX_ARGS 64

void prompt() {
    // passing NULL and 0 which allocates the needed memory 
    // instead of providing a buffer that is already allocated
    char *cwd = getcwd(NULL, 0);
    if (cwd != NULL) {
        printf("%s$", cwd);
        fflush(stdout);
        // frees up memory when no longer needs; prevents memory leaks
        free(cwd);
    } else { perror("getcwd() error"); }
}

char* input(){
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
    char *args[MAX_ARGS];
    char *tokens;
    tokens = strtok(userInput, " ");

    // if tokens is NULL, the function stops
    if (tokens == NULL){ 
        perror("tokens equals NULL");
        return; 
    }

    int i = 0;
    while (tokens != NULL && i < MAX_ARGS) { 
        args[i++] = tokens;
        tokens = strtok(NULL, " "); 
    }

    args[i] = NULL;

    pid_t pid = fork();

    if (pid == 0) {  // if true, then it is a child process
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // if true, then it is the parent
        int status;
        waitpid(pid, &status, 0);
    } else { perror("fork failed"); }

    if (!strcmp(tokens, "exit")) { exit(EXIT_SUCCESS); }
}

int main() {

    while (1) {
        prompt();
        inputAction(input());
    }
    return 0;
}