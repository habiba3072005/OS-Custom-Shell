#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h> 

#define INPUT_SIZE 1024
#define MAX_ARGS  64
#define max_history 50

int background;

// prevent zombie processes
void handle_sigchld(int sig)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// Tokenize input
int parse_input(char *input, char **args)
{
    int i = 0;
    char *token;
    background = 0;

    token = strtok(input, " \t");
    while(token != NULL)
    {
        args[i] = token;
        i++;
        if(i >= MAX_ARGS - 1)
            break;
        token = strtok(NULL, " \t");
    }

    args[i] = NULL;

    // check for background process &
    for(int j = 0; args[j] != NULL; j++){
        if(strcmp(args[j], "&") == 0){
            background = 1;
            args[j] = NULL; // remove &
            break;
        }
    }

    return i;
}

void handle_redirection(char **args)
{
    int cmd_end = -1; 

    for(int i = 0; args[i] != NULL; i++)
    {
        // output >
        if(strcmp(args[i], ">") == 0)
        {
            int fd = open(args[i+1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if(fd < 0){ perror("myShell error: open"); exit(1); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            
            if(cmd_end == -1 || i < cmd_end) 
                cmd_end = i; 
        }

        // input <
        if(strcmp(args[i], "<") == 0)
        {
            int fd = open(args[i+1], O_RDONLY);
            if(fd < 0){ perror("myShell error: open"); exit(1); }
            dup2(fd, STDIN_FILENO);
            close(fd);
            
            if(cmd_end == -1 || i < cmd_end) 
                cmd_end = i;
        }
    }

    if (cmd_end != -1) {
        args[cmd_end] = NULL;
    }
}

// Execute command using fork and execvp
void run_command(char **args)
{
    pid_t pid;
    int status;

    if(args[0] == NULL)
        return;

    // pipe check
    int pipe_index = -1;
    for(int i = 0; args[i] != NULL; i++){
        if(strcmp(args[i], "|") == 0){
            pipe_index = i;
            break;
        }
    }

    //=========================With pipe==============================
    if(pipe_index != -1)
    {
        int cmd_count = 1;

        // count number of commands
        for(int i = 0; args[i] != NULL; i++){
            if(strcmp(args[i], "|") == 0)
                cmd_count++;
        }

        char *commnds[MAX_ARGS][MAX_ARGS];

        int c = 0, j = 0;

        // split into commands
        for(int i = 0; args[i] != NULL; i++)
        {
            if(strcmp(args[i], "|") == 0)
            {
                commnds[c][j] = NULL;
                c++;
                j = 0;
            }
            else
            {
                commnds[c][j++] = args[i];
            }
        }
        commnds[c][j] = NULL;

        int in_fd = 0;
        int fd[2];
        pid_t pids[MAX_ARGS];

        for(int i = 0; i < cmd_count; i++)
        {
            // create pipe only if not last command
            if(i != cmd_count - 1)
            {
                if(pipe(fd) == -1){
                    // Handle if pipe fail
                    perror("myShell error: pipe failed");
                    return;
                }
            }

            pid_t pid = fork();

            if(pid == 0)
            {
                // input from previous pipe
                if(in_fd != 0)
                {
                    dup2(in_fd, 0);
                    close(in_fd);
                }

                // output to next pipe if not last
                if(i != cmd_count - 1)
                {
                    dup2(fd[1], 1);
                    close(fd[0]);
                    close(fd[1]);
                }

                handle_redirection(commnds[i]);

                execvp(commnds[i][0], commnds[i]);
                // Handle exec failure happen
                fprintf(stderr, "myShell error: exec failed for %s\n", commnds[i][0]);
                exit(1);
            }
            else if(pid < 0)
            {
                // Handle fork failure happen
                perror("myShell error: fork failed");
                return;
            }

            pids[i] = pid;

            // parent closes previous input
            if(in_fd != 0)
                close(in_fd);

            if(i != cmd_count - 1)
            {
                close(fd[1]);
                in_fd = fd[0];
            }
        } 
        
        // wait for all children
        if(!background)
        {
            for(int i = 0; i < cmd_count; i++)
            {
                waitpid(pids[i], NULL, 0);
            }
        }
        else
        {
            printf("Pipeline running in background. PID = %d\n", pids[0]);
        }

        return;
    }
    
    //=========================Without pipe==============================
    pid = fork();

    if(pid == -1)
    {
        perror("myShell error: fork failed");
        return;
    }

    if(pid == 0)
    {
        //ctrl+c Kill child process
        signal(SIGINT, SIG_DFL);

        //ctrl+z ignore
        signal(SIGTSTP, SIG_IGN);

        handle_redirection(args);

        execvp(args[0], args);
        // Handle command not found
        fprintf(stderr, "myShell error: command not found -> %s\n", args[0]);
        exit(1);
    }
    else
    {
        if(background){
            printf("Background process started. PID = %d\n", pid);
        } else {
            waitpid(pid, &status, 0);
        }
    }
}

// Main shell loop
int main()
{
    //ignore everything in shell
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    // zombie prevention
    signal(SIGCHLD, handle_sigchld);

    char input[INPUT_SIZE];
    char temp[INPUT_SIZE];
    char *args[MAX_ARGS];

    int num_args;

    char path[1200];
    char history[max_history][INPUT_SIZE];
    int count = 0;

    while(1)
    {
        printf("myShell> ");
        fflush(stdout);

        if(fgets(input, INPUT_SIZE, stdin) == NULL)
        {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if(strlen(input) == 0)
            continue;

        // circular history to rearrange if history is full 
        strcpy(history[count % max_history], input);
        count++;

        strcpy(temp, input);

        num_args = parse_input(temp, args);

        if(num_args == 0)
            continue;

        //change directory command
        if(strcmp(args[0], "cd") == 0)
        {
            if(args[1] != NULL){
                strcpy(path, args[1]);
            }
            else{
                char *home = getenv("HOME");

                //check home isn't null
                if(home != NULL){
                    strcpy(path, home);
                }
                else{
                    // Handle if HOME missing
                    fprintf(stderr, "cd error: HOME not set\n");
                    continue;
                }
            }

            // Added Error Handling while keeping the original 'access' structure
            if(access(path, F_OK) != 0){
                fprintf(stderr, "myShell error: cd failed: path does not exist\n");
            }
            else{
                if(chdir(path) != 0){
                    perror("myShell error: cd failed");
                }
            }
        }

        //exit command
        else if(strcmp(args[0], "exit") == 0)
        {
            printf("Good bye!\n");
            break;
        }

        //current directory
        else if(strcmp(args[0], "pwd") == 0)
        {
            if(getcwd(path, sizeof(path)) != NULL)
                printf("%s\n", path);
            else
                perror("myShell error: pwd failed");
        }

        //history command
        else if(strcmp(args[0], "history") == 0)
        {
            if (count == 0) {
                fprintf(stderr, "myShell: history is empty\n");
                continue;
            }

            int start;
            if(count > max_history)
                start = count - max_history;
            else
                start = 0;

            for(int i = start; i < count; i++){
                printf("%d  %s\n", i + 1, history[i % max_history]);
            }
        }

        else
        {
            run_command(args);
        }
    }

    return 0;
}
