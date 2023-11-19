# Mini Shell Project 

## Introduction

The objective of this project is to build a mini shell capable of executing simple shell commands. A portion of the source code is provided, utilizing Lex and Yacc for the scanner and parser, eliminating the need to implement a parser from scratch. It is crucial to familiarize yourself with Lex and Yacc before proceeding. The project structure includes the following key components:

- **examples folder**: Contains code snippets that may assist during development.
- **Commands.cc and command.h**: Where most of the C code is written.
- **Makefile**: No need to make any changes here.
- **shell.l and shell.y**: Lex and Yacc configuration files.

## First Part: Understand the Source Code


1. Build the shell program by typing: `make`.

2. Run the shell program by typing: `./shell`.

4. Experiment with commands like:
   - `ls -al`
   - `ls -al aaa bbb > out`

5. Review the provided files, especially `command.h`, to understand the data structures used to represent shell commands.

6. Modify `shell.y` to implement a more complex grammar:

```
cmd [arg]* [ | cmd [arg]* ]* [ [> filename] [< filename] [>> filename] ]* [&]
```

7. Insert the necessary actions in `shell.y` to fill in the `Command` struct. Ensure correct printing of the `Command` struct.

8. Run the program against various commands to test the grammar implementation.

## Second Part: Process Creation, Execution, File Redirection, Pipes, and Background

1. For every simple command, create a new process using `fork()` and call `execvp()` to execute the corresponding executable. If the `_background` flag is not set, wait for the last simple command to finish using `waitpid()`.

2. Implement file redirection. If input/output/error is different than 0 in the `Command` struct, create the necessary files and use `dup2()` to redirect file descriptors.

3. Implement pipes using `pipe()` to interconnect the output of one simple command to the input of the next. Use `dup2()` for redirection.

## Third Part: Control-C, Exit, Change Directory, Process Creation Log File

1. Ignore Ctrl-C. When Ctrl-C is typed, a signal `SIGINT` is generated that kills the program.

2. Implement an internal command called `exit` to exit the shell. This command should be executed by the shell itself without forking another process.

3. Implement the `cd [dir]` command to change the current directory. When `dir` is not specified, the current directory should change to the home directory.

4. Extend Lex to support any character in the arguments that is not a special character such as "&", ">", "<", "|", etc. Allow no spaces between "|", ">", etc.

5. Create a log file containing logs for every child termination using the `SIGCHLD` signal.


