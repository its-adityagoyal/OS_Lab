#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define MAX_SIZE 100

int main ( int argc, char *argv[] )
{
   int n, c, i, visited[MAX_SIZE], u, v, status;
   char *cargv[MAX_SIZE], line[4 * MAX_SIZE], *nbrs;
   FILE *fp;

   printf("*** Process %d:", getpid());
   for (i=1; i<argc; ++i) printf(" %s", argv[i]);
   printf("\n");

   fp = (FILE *)fopen("graph.txt", "r");
   fscanf(fp, "%d", &n); while (fgetc(fp) != '\n') ;
   for (u=1; u<n; ++u) visited[u] = 0;
   c = argc - 1;
   cargv[0] = strdup("./a.out");
   if (c == 0) {
      cargv[1] = strdup("1");
      cargv[2] = NULL;
      if (fork()) {
         wait(&status);
         if ( (WIFEXITED(status)) && (WEXITSTATUS(status) == 0) ) exit(0);
         printf("\nNo Hamiltonian cycle found\n");
      } else {
         execvp("./a.out", cargv);
      }
   } else if (c <= n) {
      for (u=1; u<=c; ++u) {
         visited[atoi(argv[u])] = 1;
         cargv[u] = strdup(argv[u]);
      }
      u = atoi(argv[c]);
      for (i=1; i<=u; ++i) fgets(line, 4 * MAX_SIZE, fp);
      nbrs = 1 + strchr(line, '>');
      while (*nbrs == ' ') ++nbrs;
      while (*nbrs != '\n') {
         sscanf(nbrs, "%d", &v);
         if (c == n) {
            if (v == 1) {
               printf("\nHamiltonian cycle found:");
               for (i=1; i<=n; ++i) printf(" %s", argv[i]);
               printf(" %d\n", v);
               exit(0);
            }
         } else {
            if (visited[v] == 0) {
               cargv[c+1] = (char *)malloc(10 * sizeof(char));
               sprintf(cargv[c+1], "%d", v);
               cargv[c+2] = NULL;
               if (fork()) {
                  wait(&status);
                  if ( (WIFEXITED(status)) && (WEXITSTATUS(status) == 0) ) exit(0);
               } else {
                  execvp("./a.out", cargv);
               }
            }
         }
         while ((*nbrs >= '0') && (*nbrs <= '9')) ++nbrs;
         while (*nbrs == ' ') ++nbrs;
      }
   }
   exit(1);
}
