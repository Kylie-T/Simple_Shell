// Name(s):
// Description:

#include <stdio.h> //printf getline
#include <stdlib.h> //malloc, free, exit
#include <string.h> //strcspn

//////// Some function ideas: ////////////
// Note: Some code is reflected in main that represents these functions,
// but it is up to you to determine how you want to organize your code.

/* 
    A function that causes the prompt to 
    display in the terminal
*/

void displayPrompt(){
	printf("mysh> ");
	fflush(stdout);
}


/*
    A function that takes input from the user.
    It may return return the input to the calling statement or 
    store it at some memory location using a pointer.
*/ 

char *getInput(){
	char *line = NULL; //pointer that getline() will allocate memory for
	size_t len = 0; //size of the allocated buffer
	//reads entire line, includiing spaces until newline
	if (getline(&line, &len, stdin) == -1){
		free(line);
		return NULL;
	}

	//remove trailing newline
	//strcspn find first occcurent of \n
	line[strcspn(line, "\n")] = '\0';
	return line;
}	
   /* A function that parses through the user input.
    Consider having this function return a struct that stores vital
    information about the parsed instruction such as:
    - The command itself
    - The arguments that come after the command 
        Hint: When formatting your data, 
        look into execvp and how it takes in args.
    - Information about if a redirect was detected such as >, <, or |
    - Information about whether or not a new file 
        needs to be created and what that filename may be.
    

    Some helpful functions when doing this come from string.h and stdlib.h, such as
    strtok, strcmp, strcpy, calloc, malloc, realloc, free, and more

    Be sure to consider/test for situations when a backslash is used to escape the space char
    and when quotes are used to group together various tokens.
*/

/*
    A function that executes the command. 
    This function might take in a struct that represents the shell command.

    Be sure to consider each of the following:
    1. The execvp() function. 
        This can execute commands that already exist, that is, 
        you don't need to recreate the functionality of 
        the commands on your computer, just the shell.
        Keep in mind that execvp takes over the current process.
    2. The fork() function. 
        This can create a process for execvp to take over.
    3. cd is not a command like ls and mkdir. 
        cd is a toold provided by the shell, 
        so you WILL need to recreate the functionality of cd.
    4. Be sure to handle standard output redirect and standard input redirects here 
        That is, there symbols: > and <. 
        Pipe isn't required but could be a nice addition.
*/

int main(){
	char *input = NULL;

	for(;;){
		displayPrompt();
		input =  getInput();
		if (input == NULL){
			printf("\n");
			break;
		}
		printf("you typed: %s\n",input);
		free(input);
	}
	return 0;
}

/*
int main() // MAIN
{
	char* input;
	struct ShellCommand command;
		
	// repeatedly prompt the user for input
	for (;;)
	{
        // display the prompt
        displayPrompt();

	    // get the user's input
	    input = getInput();
	    
	    // parse the command line
	    command = parseInput(input);
	    
	    // execute the command
	    executeCommand(command);
	}

	exit(0);
}
*/

