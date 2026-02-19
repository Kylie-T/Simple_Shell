# Simple_Shell

### **Group Member Names:**
Gavin Robichaux, Spencer Smith, Kylie Tate

### **Responsibilities:**
The group decided to have each member complete their own version of the project. Once completed, one version of the collection of the program would be chosen to submit.

### **Description:**
This program is a simplistic bash-like shell created from scratch in C. The user inputs commands and arguments like they would a bash shell.

### **Instructions:**
Make sure you have Git and a C compiler installed on your system.

1. Clone the Repository
    * In your terminal, navigate to the desired directory in which to clone the repository.
    * On  the GitHub repository page, click on the green `<> Code` button and copy the URL.
    * Type this command in your terminal - `git clone <repository-url>` with the copied URL.
2. Compile the Program
    * Navigate to the cloned repository
    * To compile, run: `gcc <executable>` or `gcc <executable> -o <executable-name>` to give a name to the executable.
3. Run the Program:
    * In your terminal, type `./<executable-output>` to run the file

### **Note:**
This program is a simplified version of a bash shell, so not all commands are implemented. 

**May Not Work if Not Fully Implemented**
1. Built-in Commands:
    * Here are some built-in commands the program supports:
        * `cd` - Changes the current directory
        * `help` - Display a help message and lists available commands
        * `exit` - Exits the shell
    * The function `cell_execute()` checks the commands. If not fully implemented, the shell will run in unexpected ways when trying to run those commands.
2. Input/Output Redirection:
    * The `cell_launch()` function permits input (`<`) and output (`>`) redirection. 
    * If not implemented correctly, input redirection will fail to read from the specified file and output redirection will fail to write to the specified file.
    * When using redirection incorrectly, errors will be presented.
3. Forking and Executing External Commands:
    * The shell uses `fork()` to create a child process. Also, `execvp()` is used to execute commands within the child.
4. Dynamic Memory Management:
    * If memory allocation is not correctly handled or freed, the shell could have memory leaks or crashes.

**Testing**

* For built-in commands, run `cd`, `help`, and `exit` in the shell.

* Below is an example of commands to test input/output redirects.
```sh
echo "Hello" > output.txt
cat < output.txt
```

* Test external commands like `ls`, `pwd`, or another executable.
