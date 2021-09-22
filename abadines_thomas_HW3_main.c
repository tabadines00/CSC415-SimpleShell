/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Name: Thomas Abadines
* Student ID: 918674489
* GitHub ID: tabadines00
* Project: Assignment 3 – Simple Shell
*
* File: abadines_thomas_HW3_main.c
*
* Description: This program is a simple shell that takes in
*              commands from the command line or a file and 
*              creates a child process using fork() then
*              executes the commands with execvp().
*
**************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main (int argc, char *argv[]) {

  // If a custom prompt was specified, then use that one instead
  char *prompt = "> ";
  if(argv[1] != NULL) {
    prompt = argv[1];
  }

  // The maximum number of arguments allowed per line
  int maxArgs = 10;

  // Create a buffer of size 180 bytes
  char *buffer;
  size_t bufsize = 180;
  int numChars;

  buffer = (char*)malloc(bufsize *sizeof(char));

  if(buffer == NULL) {
      perror("Unable to allocate buffer");
      exit(1);
  }

  // Make sure there is an exit condition before the while loop
  int exitCondition = 0;

  // The while loop takes in input, parses, then executes it.
  // Each iteration of the loop prompts the user, then responds, then loops
  while(exitCondition == 0) {

    // The argument vector is initialized
    char *argVector[maxArgs];

    // Print the prompt, then accept input with getline()
    printf("%s", prompt);
    numChars = getline(&buffer,&bufsize,stdin);

    // -1 means getline has reached EOF
    // if so, then clear the buffer and break out of the loop
    if(numChars == -1) {
      printf("End of File Reached...\n");
      memset(buffer, 0, bufsize);
      break;
    }

    //check if the newly entered line is empty
    if(strcmp(buffer, "\n") == 0) {
      printf("ERROR: No Input Found...\n");
      //reset the buffer then continue looping
      memset(buffer, 0, bufsize);
      continue;
    }

    // This will temporarily hold the tokens as we iterate
    char *token;
    
    // remove the newline character from the buffer
    char *newline = strchr(buffer, '\n');
    if (newline) {
      *newline = 0;
    }

    // get the first token and add it to argVector[0]
    // This argument is very important for execvp()
    token = strtok(buffer, " ");
    int argIndex = 0;
    argVector[0] = token;

    // If the token received was "exit", then update the exitCondition
    // and break from the loop
    if(strcmp(argVector[0], "exit") == 0) {
      exitCondition = 1;
      break;
    }

    // Tokenize the rest of the input and then add it to argv
    while(token != NULL) {
      if(strcmp(token, " ")) {
        // Print the token and then add to argVector
        argVector[argIndex++] = token;
      }

      token = strtok(NULL, " ");
    }

    // Make sure to end the argument vector with NULL to tell execvp() to stop
    argVector[argIndex] = NULL;

    // Fork a child process and save its PID
    pid_t pid = fork();

    // If the pid is 0, it must be a child process
    // Since it is a child we will call execvp() with argVector[0]
    // and argVector as the parameters
    if(pid == 0) {
      execvp(argVector[0], argVector);
      // Exit once finished
      exit(errno);
    }

    // Wait on the child process
    int status;
    waitpid(pid, &status, 0);

    // If the child has exited, then print its status
    if(WIFEXITED(status)) {
      int exitStatus = WEXITSTATUS(status);

      // Print the message depending on its exit status
      switch(exitStatus) {
        case 2:
          printf("Invalid input...\n");
        default:
          printf("Child %d, exited with %d\n", pid, exitStatus);
      }
    }

    //Now that we're done with the buffer, clear it
    memset(buffer, 0, bufsize);
  }
   
  // Free the buffer now that we are exiting
  free(buffer);
  buffer = NULL;
  return 0;
}