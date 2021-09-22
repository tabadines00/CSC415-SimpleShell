#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

int main () {

  char *buffer;
  size_t bufsize = 180;
  size_t characters;

  buffer = (char*)malloc(bufsize *sizeof(char));

  if(buffer == NULL) {
      perror("Unable to allocate buffer");
      exit(1);
  }

  int exitCondition = 0;
  while(exitCondition == 0) {


    char *argv[10];

    printf("SimpSHELL >> ");
    characters = getline(&buffer,&bufsize,stdin);

    /*////////////////////////////////////////////////////////////
    // prints how many characters were read and what the buffer reads
    printf("%zu characters were read.\n",characters);
    printf("You typed: '%s'\n",buffer); 

    /////////////////////////////////////////////////////////*/

    //check if the newly entered line is empty
    if(strcmp(buffer, "\n") == 0) {
      printf("You didnt type anything...\n");
      //reset the buffer then continue
      memset(buffer, 0, bufsize);
      continue;
    }

    char *token;
    
    // remove the newline character from the buffer
    char *newline = strchr(buffer, '\n');
    if (newline) {
      *newline = 0;
    }

    // get the first token and add it to argv[0]
    token = strtok(buffer, " ");
    int argIndex = 0;
    argv[0] = token;

    if(strcmp(argv[0], "quit") == 0) {
      printf("Ight imma head out\n");
      exitCondition = 1;
      break;
    }

    // Tokenize the input and then add it to argv
    while(token != NULL) {
      if(strcmp(token, " ")) {
        // Print the token and then add to the argvector
        //printf( "%s\n", token ); ///////////////////////////////////////
        argv[argIndex++] = token;
      }

      token = strtok(NULL, " ");
    }

    // Make sure to end the argument vector with NULL to terminate the command
    argv[argIndex] = NULL;

    /////////////////////////////////////////
    // List Args
    int j = 0;
    while(argv[j] != NULL) {
      printf( "arg %d: %s\n", j, argv[j] );
      j++;
    }
    ///////////////////////////////////////

    // Fork and Execute, then loop

    printf("----------------\n");
    
    pid_t pid = fork();

    if(pid == 0) {
      execvp(argv[0], argv);
      exit(errno);
    }

    int status;
    waitpid(pid, &status, 0);

    if(WIFEXITED(status)) {
      int exitStatus = WEXITSTATUS(status);

      switch(exitStatus) {
        case 2:
          printf("Invalid input...\n");
        default:
          printf("Child Exit Status: %d\n", exitStatus);
          printf("Child PID: %d\n", pid);
      }
    }

    //Now that we're done with the buffer, clear it
    memset(buffer, 0, bufsize);
    printf("--Buffer Cleared\n");
  }
   
  free(buffer);
  buffer = NULL;
  return 0;
}