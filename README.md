# Project Title: Custom Shell

## Description

Welcome to the Custom Shell project, Mini Project 1, where you will build your own shell. This shell implementation involves various features, system calls, and commands to provide a comprehensive user experience.
To see Problem Statement open Problem_statement.html OR go to `https://karthikv1392.github.io/cs3301_osn/mini-projects/mp1`
### Part A: Basic System Calls

#### Specification 1: Display Requirement
The shell prompt displays the current user, system name, and working directory.


#### Specification 2: Input Requirements
The shell supports `;` or `&` separated lists of commands, handling random spaces and tabs.

#### Specification 3: warp
The `warp` command changes the shell's directory and reflects the change in the prompt. It supports flags such as `~`, `..`, `.`, and `-`.

#### Specification 4: peek
The `peek` command lists files and directories with optional flags `-a` and `-l`. Differentiates files, directories, and executables with color coding.

#### Specification 5: pastevents commands
- `pastevents`: Stores and outputs the 15 most recent command statements, persisting across shell runs.
- `pastevents purge`: Clears all stored past events.
- `pastevents execute <index>`: Executes the command at the specified index in past events.

#### Specification 6: System commands
The shell executes system commands in both foreground and background processes, providing process information upon completion.

#### Specification 7: proclore
The `proclore` command obtains information about a process, with optional PID argument.

#### Specification 8: seek
The `seek` command looks for files/directories in a specified target directory, supporting flags `-d`, `-f`, and `-e`.

### Part B: Processes, Files, and Misc.

#### Specification 9: I/O Redirection
Supports input/output redirection with `>`, `>>`, and `<`.

#### Specification 10: Pipes
Supports any number of pipes between commands.

#### Specification 11: Redirection along with pipes
Handles I/O redirection along with pipes.

#### Specification 12: activities
Lists all running processes spawned by the shell with relevant information.

#### Specification 13: Signals
Implements `ping` command to send signals to processes, including Ctrl-C, Ctrl-D, and Ctrl-Z.

#### Specification 14: fg and bg
- `fg <pid>`: Brings a background process to the foreground.
- `bg <pid>`: Changes the state of a stopped background process to running.

#### Specification 15: Neonate
Prints the Process-ID of the most recently created process until the key 'x' is pressed.

### Part C: Networking

#### Specification 16: iMan
Fetches man pages from http://man.he.net/ using sockets and outputs them to the terminal.

## Getting Started

Follow the instructions below to build and run the custom shell on your system.


## Building


##Running
1. Clone the repository.
2. Navigate to the project directory.
3. Command- '''make'''
4. then '''./a.out'''


## Assumption
History File: The terminal maintains a history.txt file that logs all executed commands. Ensure this file is present in the current directory when you run the terminal.

Exiting the Terminal: To gracefully exit this terminal emulator, press ctrl + d.

Built-in cd Command: While most shell commands are executed through execvp, certain commands like cd have been natively implemented due to limitations with execvp.

 Edge Cases: The terminal has been designed to handle most common scenarios and edge cases. However, there might be a few rare edge cases that haven't been addressed.


