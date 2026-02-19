
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
#define MAX_ARGS 64

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
                fprintf(stderr, "ERROR 13 (denied permissions)\n");
            }else if(errno == ENOENT){
                fprintf(stderr, "ERROR 2 (NO such file or directory)\n");
            }else{
                fprintf(stderr, "ERROR 1 (invalid command)\n");
            }
            
        }
    } 
        return;
    }
    pid_t pid, wpid;
    int status;

    //input and output redirect files
    char *input_file = NULL; 
    char *output_file = NULL;
    int background = 0;

    
    int last = 0;
    //iterates through the argument
    while(args[last] != NULL) last++;
    //checks for &
    if(last > 0 && strcmp(args[last -1], "&") == 0){
        background = 1;
        args[last -1] = NULL; //switch & to null
    }

    //iterates through argument
    for(int i = 0; args[i] != NULL; i++){
        //checks for < redirect
        if(strcmp(args[i], "<") == 0){ 
            //checks if the next argument is NULL
            if(args[i+1] == NULL){ 
                //throw error
                fprintf(stderr, "Error 2 (No such file or directory)\n");
                return;
            }
            //if we pass the check, store the arg
            input_file = args[i+1];
            args[i] = NULL; //set to null
        }
        //same idea as the last one
        else if(strcmp(args[i], ">") == 0){
            if(args[i+1] == NULL){
                fprintf(stderr, "Error 2 (No such file or directory)\n");
                return;
            }
            output_file = args[i+1];
            args[i] = NULL;//set to null
        }
    }
    //create child process
    pid = fork();

    if (pid == 0) {
        //child process
        //handle input redirect
        if(input_file != NULL){

            //open for reading
            int fd = open(input_file, O_RDONLY);

            //error
            if(fd < 0){
                fprintf(stderr, "Error 2 (No such file or directory)\n");
                exit(EXIT_FAILURE);
            }
            //redirect input to file
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        if(output_file != NULL){
            //open for writing
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            //error
            if(fd < 0){
                fprintf(stderr, "Error 2 (No such file or directory)\n");
                exit(EXIT_FAILURE);
            }
            //redirect output to file
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        //replace child process with command
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "Error 1 (Invalid command)\n");
        }
        //exit if exec fails
        exit(EXIT_FAILURE);
    } 
    //fork failed
    else if (pid < 0) {
        fprintf(stderr, "Error 1 (Invalid command)\n");
    } 
    else {
        if(!background){
            do {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        } else {
            printf("[BACKGROUND pid %d]\n", pid);
        }
    }

    return;

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