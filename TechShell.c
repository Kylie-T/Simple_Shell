
// Name(s): Spencer Smith, Kylie Tate, Gavin Robichaux
// Description: 
//Small scale implementation of a shell that supports built in 
//commands (such as cd, help, and exit), file redirection
//the creation of processes, and background exe. This program was 
//Resources used:
//https://brennan.io/2015/01/16/write-a-shell-in-c/
//https://www.youtube.com/watch?v=yTR00r8vBH8
//some parts i did have to use AI assistance for debugging


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#define CELL_RL_BUFSIZE 1024 //initial buffer for readline
#define CELL_TOK_BUFSIZE 64 //number of tokens for splitting
#define CELL_TOK_DELIM " \t\r\n\a" //token delimiter

// Function protoypes
int cell_cd(char **args);
int cell_help(char **args);
int cell_exit(char **args);
int cell_launch(char **args);
int cell_execute(char **args);
char **cell_split_line(char *line);
char *cell_read_line(void);
void cell_loop(void);

// Built-in commands
char *builtin_str[] = {"cd", "help", "exit"};

//pointers for builtins
int (*builtin_func[]) (char **) = {&cell_cd, &cell_help, &cell_exit};

//return number of builtins
int cell_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


/*
    if (args[1] == NULL) {
        fprintf(stderr, "Error 2 (No such file or directory)\n");
    } else {
        printf("DEBUGL CHANGING TO DIRECTORY '%s'\n", args[1]);
        if (chdir(args[1]) != 0) {
            fprintf(stderr, "Error 2 (No such file or directory)\n");
        }
    }
    return 1;
} copied this into chat gpt while debugging
 recommended lines ~64-84 to properly get the home vairiable
 and to handle errors 
*/
int cell_cd(char **args) {
    if (args[1] == NULL) { //no argument
        
        char *home = getenv("HOME");  //get home variable
        if (home && chdir(home) == 0){
            return 1; //success
        }
        
        //if it fails
        fprintf(stderr, "Error 2 (No such file or directory)\n");
    } else {
        
        //modified this to handle the types of errors (needed to see if cd /root error)
        if (chdir(args[1]) != 0) {
            
            //no access
            if(errno == EACCES){
                fprintf(stderr, "ERROR 3 (denied permissions)\n");
            }
            //file doesnt exist
            else if(errno == ENOENT){
                fprintf(stderr, "ERROR 2 (NO such file or directory)\n");
            }else{
                fprintf(stderr, "ERROR 1 (invalid command)\n");
            }
            
        }
    }
    return 1;
}

//help function that lists out the built in commands
int cell_help(char **args) {
    printf("Welcome to TechShell:\n");
    printf("Built-in commands:\n");
    
    //print each command
    for (int i = 0; i < cell_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    return 1;
}

//program exit, 0 stops main
int cell_exit(char **args) {
    return 0;
}

//launches commands and handles redirection/ baclground
int cell_launch(char **args) {
    pid_t pid, wpid;
    int status;

    //input and output redirect files
    char *input_file = NULL; 
    char *output_file = NULL;
    int background = 0;
    
    int last = 0;

    //find last arg index
    while(args[last] != NULL) last++;

    //checks for & at the end
    if(last > 0 && strcmp(args[last -1], "&") == 0){
        background = 1;
        args[last -1] = NULL; //switch & to null
    }

    //iterates through argument
    for(int i = 0; args[i] != NULL; i++){

        //checks for < redirect
        if(strcmp(args[i], "<") == 0){ 

            //no file after <
            if(args[i+1] == NULL){ 

                //throw error
                fprintf(stderr, "Error 2 (No such file or directory)\n");
                return 1;
            }
            //if we pass the check, store the arg
            input_file = args[i+1];
            args[i] = NULL; //set to null
        }
        //same idea as the last one
        else if(strcmp(args[i], ">") == 0){
            if(args[i+1] == NULL){
                fprintf(stderr, "Error 2 (No such file or directory)\n");
                return 1;
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
        
        //wait for child process unless running in background
        if(!background){
            do {
                
                //wait for child to finish
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        } else {
            
            //print background id
            printf("[BACKGROUND pid %d]\n", pid); 
        }
    }
    //return 1 to continue loop
    return 1;
}
//determine if command is a builtin or external
int cell_execute(char **args) {
    if (args[0] == NULL) {
        return 1; //empty 
    }
    
    //check the builtins
    for (int i = 0; i < cell_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    
    return cell_launch(args);
}

//tokenization
char **cell_split_line(char *line) {
    int bufsize = CELL_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*)); //allocate mem
    char *token;

    if (!tokens) {
        fprintf(stderr, "Error 1 (Invalid command)\n");
        exit(EXIT_FAILURE);
    }
    //first tok
    token = strtok(line, " \t\r\n\a");
    while (token != NULL) {
        //store
        tokens[position++] = token;
        
        //expand buff if needed
        if (position >= bufsize) {
            bufsize += CELL_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "Error 1 (Invalid command)\n");
                exit(EXIT_FAILURE);
            }
        }
        //next tok
        token = strtok(NULL, " \t\r\n\a");
    }
    //null terminator
    tokens[position] = NULL;
    return tokens;
}
//read user input
char *cell_read_line(void) {
    char *line = NULL;
    ssize_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) { //ctrl d
            exit(EXIT_SUCCESS);
        } else {
            fprintf(stderr, "Error 1 (Invalid command)\n");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

void cell_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        //buff for current directory
        char cwd[1024];
        if(getcwd(cwd, sizeof(cwd)) != NULL){
            //prompt
            printf("%s$ ", cwd);
        } else {
            fprintf(stderr, "Error 2 (No such file or directory)\n");
        }
        //read input and tokenize, execute command
        line = cell_read_line();
        args = cell_split_line(line);
        status = cell_execute(args);


        //free our memory
        free(line);
        free(args);

    } while (status);
}

//start shell
int main(int argc, char **argv) {
    cell_loop();
    return EXIT_SUCCESS;
}
