#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "LineParser.h"

void execute(cmdLine *pCmdLine, int debugMode) {
    // Check if the command should run in the background
    int runInBackground = pCmdLine->blocking == 0 ? 1 : 0;

    // Fork to create a new process
    pid_t pid = fork();

    // Handle fork errors
    if (pid == -1) {
        perror("fork failed");
        _exit(EXIT_FAILURE);  // Exit abnormally if fork fails
    }

    // Child process
    if (pid == 0) {
        if (debugMode) {
            // Print PID and executing command to stderr
            fprintf(stderr, "PID: %d\n", getpid());
            fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
        }

        // Execute the command using execvp
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("execvp failed");
            _exit(EXIT_FAILURE);  // Exit abnormally if execvp fails
        }
    } else {
        // Parent process
        // If running in the foreground, wait for the child to complete
        if (!runInBackground) {
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid failed");
            }
        } else {
            // Print background process information if in debug mode
            if (debugMode) {
                fprintf(stderr, "Background process started: %s (PID: %d)\n", pCmdLine->arguments[0], pid);
            }
        }
    }
}

// 2-------------------------------------------------
// Function to handle the alarm command
void alarm_command(int pid) {
    if (kill(pid, SIGCONT) == -1) {
        perror("kill failed");
    } else {
        printf("Process with PID %d woken up successfully.\n", pid);
    }
}

// Function to handle the blast command
void blast_command(int pid) {
    if (kill(pid, SIGKILL) == -1) {
        perror("kill failed");
    } else {
        printf("Process with PID %d terminated successfully.\n", pid);
    }
}

int main(int argc, char **argv) {
    char cwd[PATH_MAX];
    char input[2048];
    cmdLine *parsedCmdLine;
    int debugMode = 0;

    // Check for the debug flag
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debugMode = 1;
    }

    while (1) {
        // Display prompt with current working directory
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s$ ", cwd);
        } else {
            perror("getcwd failed");
            continue;
        }

        // Read input from the user
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets failed");
            continue;
        }

        // Remove the newline character from the input
        input[strcspn(input, "\n")] = 0;

        // Exit the shell if the command is "quit"
        if (strcmp(input, "quit") == 0) {
            break;
        }

        // Parse the input using parseCmdLines()
        parsedCmdLine = parseCmdLines(input);
        if (parsedCmdLine == NULL) {
            continue;
        }

        // 1c -------------------------
        // Check for the "cd" command
        if (strcmp(parsedCmdLine->arguments[0], "cd") == 0) {
            // Change directory
            if (chdir(parsedCmdLine->arguments[1]) != 0) {
                fprintf(stderr, "cd failed: No such file or directory\n");
            }
            continue;
        }

        // Execute the command
        // execute(parsedCmdLine, debugMode);

        // 2------------------------------------------------- if its an alarm or blast - handle it. else, execute commend.
        if (strcmp(parsedCmdLine->arguments[0], "alarm") == 0 && parsedCmdLine->argCount == 2) {
            int pid = atoi(parsedCmdLine->arguments[1]);
            alarm_command(pid);
        } else if (strcmp(parsedCmdLine->arguments[0], "blast") == 0 && parsedCmdLine->argCount == 2) {
            int pid = atoi(parsedCmdLine->arguments[1]);
            blast_command(pid);
        } else {
            // Execute the command
            execute(parsedCmdLine, debugMode);
        }

        // Release the cmdLine resources
        freeCmdLines(parsedCmdLine);
    }

    return 0;
}




// 2) The execv function requires the full path to the executable
//    because it does not search the directories listed in the PATH environment variable.
//    It only tries to execute the file specified by the given path.
