#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

#define MAX_LINE 80

int main(void) {
  char *
      args[MAX_LINE / 2 + 1]; // array of chars for spliting a command on tokens
  int should_run = 1;         // variable for exit
  bool lastCmd_true = false;  // variable to check if last command exist
  char *cmdamp[MAX_LINE +
               1]; // array of chars to store next function after ampersand
  while (should_run) {
    printf("osh>");
    fflush(stdout); // take input
    char cmd[MAX_LINE + 1];
    char lastCmd[MAX_LINE + 1];       // array of chars for last command
    fgets(cmd, MAX_LINE + 1, stdin);  // put input into the variable
    if (strncmp(cmd, "exit", 4) == 0) // if exit
      should_run = 0;
    else {
      if (strncmp(cmd, "!!", 2) == 0) { // if for history
        if (!lastCmd_true) {
          printf("No commands in the history\n");
          continue;
        } else {
          strcpy(cmd,
                 lastCmd); // copy last command to command for printing history
          printf("Last command: %s\n", cmd);
        }
      }
      for (int i = 0; i < MAX_LINE; i++) { // remebering the last command
        lastCmd[i] = cmd[i];
        lastCmd_true = true;
      }
      pid_t pid = fork(); // spliting on two processes
      if (pid < 0) {
        fprintf(stderr, "Fork Failed!");
        return 1;
      }
      if (pid == 0) {
        cmd[strlen(cmd) - 1] = 0;
        char *t = strtok(cmd, " ");
        int i = 0; // index for arguments
        int file;
        while (t != 0) { // spliting command on tokens
          args[i] = t;
          t = strtok(0, " ");
          i++;
        }
        args[i] = NULL;
        if (args[1] != NULL) {             // cases for & < > and |
          if (strcmp(args[1], "&") == 0) { // case for ampersand
            cmdamp[0] = args[0];
            args[1] = NULL;
            pid_t pp = fork(); // fork for excuting two processes separately
            if (pp == 0) {
              if (execvp(args[2], args) == -1) // command after &
                printf("Invalid command\n");
            } else {
              pid_t pp1 = fork();
              if (pp1 == 0) {
                if (execvp(cmdamp[0], args) == -1) // command before &
                  printf("Invalid command\n");
              } else {
                wait(NULL);
              }
              wait(NULL);
            }
          }
          if (strcmp(args[1], ">") == 0) { // > case
            file = open(args[2], O_WRONLY | O_CREAT,
                        0644);         // create a file to write
            dup2(file, STDOUT_FILENO); // changing the place stdout
            args[1] = NULL;
            args[2] = NULL;
            if (execvp(args[0], args) == -1) // command after &
              printf("Invalid command\n");

            break;
          }
          if (strcmp(args[1], "<") == 0) {  // < case
            file = open(args[2], O_RDONLY); // open a file for reading
            dup2(file, STDIN_FILENO);       // changing the place for stdin
            args[1] = NULL;
            args[2] = NULL;
            if (execvp(args[0], args) == -1) // command after &
              printf("Invalid command\n");

            break;
          }
          if (strcmp(args[1], "|") == 0) { // pipe case

            int fd[2];
            if (pipe(fd) == -1) {
              return 1;
            }
            char *fc[MAX_LINE + 1]; // first command before |
            char *sc[MAX_LINE + 1]; // second command after |
            fc[0] = args[0];        // taking arguments
            sc[0] = args[2];        // taking arguments
            int pidf = fork();
            if (pidf < 0) {
              return 1;
            }
            if (pidf == 0) {
              dup2(fd[1], STDOUT_FILENO); // changing the place stdout
              close(fd[0]);               // closing file descriptors
              close(fd[1]);
              if (execvp(fc[0], fc) == -1) // executing first command
                printf("Invalid command\n");
            } else {
              int pidf2 = fork();
              if (pidf2 < 0) {
                return 3;
              }
              if (pidf2 == 0) {
                dup2(fd[0], STDIN_FILENO); // changing the place stdin
                close(fd[0]);              // changing the place stdout
                close(fd[1]);
                if (execvp(sc[0], sc) == -1) // executing second command
                  printf("Invalid command\n");
              } else {
                close(fd[0]); // closing file decriptors
                close(fd[1]);
                wait(NULL);
              }
              wait(NULL);
            }
            break;
          }
        }
        if (execvp(args[0], args) == -1) // main execution
          printf("Invalid Command\n");
        exit(0);
      } else {
        wait(NULL);
      }
    }
  }
  return 0;
}