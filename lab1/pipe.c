#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

int check_child_status(int child_pid)
{
    int status;
    waitpid(child_pid, &status, 0);
    if (WIFEXITED(status))
    { // Check if child process worked.
        return WEXITSTATUS(status);
    }
    else
    { // Error in child process.
        return 1;
    }
}

int execute_command(char *arg)
{
    int status = execlp(arg, arg, NULL);
    perror("Command failed.\n"); // If thread returns, command has failed.
    exit(status);                // Exit with command status.
}

int main(int argc, char *argv[])
{
    if (argc > 2)
    {                               // If multiple commands exist
        int pipe_list[argc - 2][2]; // List of file descriptors.
        int children[argc - 1];     // List of child PIDs.

        for (int i = 0; i < argc - 2; i++)
        { // Open the pipes.
            if (pipe(pipe_list[i]) == -1)
            { // If unable to open exit.
                perror("Was not able to create pipe.\n");
                exit(1);
            }
        }

        for (int cur = 1; cur < argc; cur++)
        {
            children[cur - 1] = fork();
            if (children[cur - 1] == -1)
            { // Check if able to fork.
                perror("Was not able to fork.\n");
                for (int prev = 0; prev < cur; prev++)
                { // Kill all previous processes and exit.
                    kill(children[prev], SIGKILL);
                }
                exit(1);
            }
            else if (children[cur - 1] == 0)
            { // In child process
                if (cur == argc - 1)
                { // For last commadn, only have to setup read FD.
                    dup2(pipe_list[cur - 2][0], STDIN_FILENO);
                    close(pipe_list[cur - 2][0]);
                }
                else if (cur == 1)
                { // For first command, only have to setup write FD.
                    dup2(pipe_list[cur - 1][1], STDOUT_FILENO);
                    close(pipe_list[cur - 1][1]);
                }
                else
                { // For all other commands, have to setup both read and write FD.
                    dup2(pipe_list[cur - 2][0], STDIN_FILENO);
                    close(pipe_list[cur - 2][0]);

                    dup2(pipe_list[cur - 1][1], STDOUT_FILENO);
                    close(pipe_list[cur - 1][1]);
                }
                // Execute command.
                execute_command(argv[cur]);
            }
            else
            { // In parent process
                if (cur == argc - 1)
                { // For last command, only have to close read FD.
                    close(pipe_list[cur - 2][0]);
                }
                else if (cur == 1)
                { // For first command, only have to close write FD.
                    close(pipe_list[cur - 1][1]);
                }
                else
                { // For all other commands, have to close both read and write FD.
                    close(pipe_list[cur - 2][0]);
                    close(pipe_list[cur - 1][1]);
                }
            }
        }

        for (int i = 0; i < argc - 1; i++)
        {
            int status = check_child_status(children[i]);
            if (status)
                return status; // If any child process had an error, return an error.
        }
    }
    else if (argc < 2)
    { // If no command is provided.
        perror("Need to have one command.\n");
        exit(EINVAL);
    }
    else if (argc == 2)
    { // If there is one command.
        int child = fork();
        if (child == -1)
        { // Check if fork is valid
            perror("Was not able to fork.\n");
            exit(1);
        }
        else if (child == 0)
        {                             // In child process
            execute_command(argv[1]); // Execute command
        }
        else
        { // In parent process
            return check_child_status(child);
        }
    }
    return 0;
}
