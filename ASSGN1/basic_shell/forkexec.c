/********************************************************************************************/
/* This is a simple shell that keeps on reading single-word commands from the user, and     */
/* forking a child process for running each command. The command exit terminates the shell. */
/* Last updated by Abhijit Das, 13-Jan-2026                                                 */
/********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
// #include <sys/ipc.h>
#include <sys/wait.h>

#define LINE_SIZE 1024

int main ( )
{
   char line[LINE_SIZE];

   while (1) {
      /* Read a line of input from the user */
      printf("$ "); fgets(line,LINE_SIZE,stdin); line[strlen(line)-1] = '\0';

      /* Nothing to do for a blank line */
      if (!strcmp(line,"")) continue;

      /* Terminate the shell */
      if (!strcmp(line,"exit")) break;

      if (fork()) {  /* Parent process */
         wait(NULL); /* Wait until the child terminates */
      } else {       /* Child process */
         execlp(line,line,NULL); /* Try to run the user command */
         /* execlp() returns if the command cannot run */
         fprintf(stderr, "*** Cannot run command\n");
         exit(1); /* child should exit, otherwise it will enter the loop like parent */
      }
   }

   exit(0);
}
