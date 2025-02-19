/*

    Name: Oyebola Ibrahim Oyewunmi
    ID: 1001913735

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024  // Maximum command length
#define MAX_ARGS 11  // Up to 10 arguments + NULL terminator
#define MAX_PID_HISTORY 15  // Store last 15 process PIDs
#define MAX_HISTORY 15  // Store last 15 commands

pid_t pid_history[MAX_PID_HISTORY];
int pid_count = 0;

char history[MAX_HISTORY][MAX_INPUT_SIZE];
int history_count = 0;

/**
 * Tokenizes user input into command arguments.
 * 
 * The raw input string from the user.
 * Array to store the parsed command and arguments.
 */
void tokenize_input(char *input, char **args) {
    char *token = strtok(input, " \n");
    int i = 0;
    while (token != NULL) {
        if (i < 10) {  // Only store up to 10 arguments
            args[i++] = token;
        }
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;

    if (i >= 10) {
        fprintf(stderr, "Warning: Only the first 10 arguments are processed.\n");
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];

    while (1) {
        // Display shell prompt
        printf("msh> ");
        fflush(stdout);

        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;  // Exit
        }

        // Exit shell if "quit" or "exit" is entered
        if (strcmp(input, "quit\n") == 0 || strcmp(input, "exit\n") == 0) {
            exit(EXIT_SUCCESS);
        }

        // Ignore blank input (just pressing Enter)
        if (strcmp(input, "\n") == 0) {
            continue;
        }

        // Tokenize input into arguments
        tokenize_input(input, args);

        // Handle `cd` command (built-in, so no forking)
        if (strncmp(args[0], "cd", 2) == 0) {  
            if (args[1] == NULL) {
                fprintf(stderr, "cd: missing argument\n");
            } else {
                if (chdir(args[1]) != 0) {  
                    perror("cd failed");
                }
            }
            continue;
        }

        // Handle `pid_history` command (Print last 15 process PIDs)
        if (strcmp(args[0], "pid_history") == 0) {  
            for (int i = 0; i < pid_count; i++) {
                printf("%d\n", pid_history[i]);  
            }
            continue;
        }

        // Handle `history` command (Print last 15 commands)
        if (strcmp(args[0], "history") == 0) {  
            for (int i = 0; i < history_count; i++) {
                printf("%d:  %s\n", i, history[i]);  
            }
            continue;
        }

        // Handle !n command execution (Re-run previous command)
        if (args[0][0] == '!' && strlen(args[0]) > 1) {
            int command_num = atoi(args[0] + 1);
            if (command_num >= 0 && command_num < history_count) {
                strcpy(input, history[command_num]);
                printf("Re-executing: %s", input);
                tokenize_input(input, args);
            } else {
                printf("Invalid command number in history.\n");
            }
            continue;
        }

        // Store command in history (FIFO queue)
        if (history_count < MAX_HISTORY) {
            strcpy(history[history_count++], input);
        } else {
            // Shift history to make room for new command
            for (int i = 1; i < MAX_HISTORY; i++) {
                strcpy(history[i - 1], history[i]);
            }
            strcpy(history[MAX_HISTORY - 1], input);
        }

        // Fork a child process to execute the command
        pid_t pid = fork();
        int status;

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) {  // Child process
            if (execvp(args[0], args) == -1) {
                printf("%s: Command not found.\n", args[0]);
                exit(EXIT_FAILURE);
            }
        } 
        else {  // Parent process
            // Store PID in history (FIFO queue)
            if (pid_count < MAX_PID_HISTORY) {
                pid_history[pid_count++] = pid;
            } else {
                for (int i = 1; i < MAX_PID_HISTORY; i++) {
                    pid_history[i - 1] = pid_history[i];
                }
                pid_history[MAX_PID_HISTORY - 1] = pid;
            }
            waitpid(pid, &status, 0);
        }
    }
    return 0;
}
