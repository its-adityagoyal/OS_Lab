#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_NODES 100
#define MAX_LINE 1024

int N;
int adj[MAX_NODES + 1][MAX_NODES + 1];
int degree[MAX_NODES + 1];
int neighbors[MAX_NODES + 1][MAX_NODES];

void read_graph() {
    FILE *fp = fopen("graph.txt", "r");
    if (!fp) {
        exit(1);
    }

    if (fscanf(fp, "%d", &N) != 1) {
        exit(1);
    }

    int i, j;
    for (i = 0; i <= N; i++) {
        degree[i] = 0;
        for (j = 0; j <= N; j++) {
            adj[i][j] = 0;
        }
    }

    char line[MAX_LINE];
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        int u, v, bytes_read;
        char arrow[5];
        char *ptr = line;

        if (sscanf(ptr, "%d %s%n", &u, arrow, &bytes_read) < 2) continue;
        ptr += bytes_read;

        while (sscanf(ptr, "%d%n", &v, &bytes_read) == 1) {
            if (u <= N && v <= N) {
                adj[u][v] = 1;
                neighbors[u][degree[u]++] = v;
            }
            ptr += bytes_read;
        }
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    read_graph();

    printf("*** Process %d:", getpid());
    for (int i = 1; i < argc; i++) {
        printf(" %s", argv[i]);
    }
    printf("\n");

    if (argc == 1) {
        pid_t pid = fork();
       
        if (pid < 0) {
            exit(1);
        } else if (pid == 0) {
            char *new_argv[3];
            new_argv[0] = argv[0];
            new_argv[1] = "1";
            new_argv[2] = NULL;
           
            execvp(argv[0], new_argv);
            exit(1);
        } else {
            int status;
            wait(&status);
           
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                exit(0);
            } else {
                printf("No Hamiltonian cycle found\n");
                exit(1);
            }
        }
    } else {
        int path[MAX_NODES + 1];
        int visited[MAX_NODES + 1];
        int c = argc - 1;
        int i;
       
        for (i = 0; i <= N; i++) visited[i] = 0;

        for (i = 1; i < argc; i++) {
            path[i] = atoi(argv[i]);
            visited[path[i]] = 1;
        }

        int current_node = path[c];
        int start_node = path[1];

        if (c == N) {
            if (adj[current_node][start_node]) {
                printf("Hamiltonian cycle found: ");
                for (i = 1; i <= c; i++) {
                    printf("%d ", path[i]);
                }
                printf("%d\n", start_node);
                exit(0);
            } else {
                exit(1);
            }
        }

        for (i = 0; i < degree[current_node]; i++) {
            int neighbor = neighbors[current_node][i];

            if (!visited[neighbor]) {
                pid_t pid = fork();

                if (pid < 0) {
                    exit(1);
                } else if (pid == 0) {
                    char **new_argv = malloc((argc + 2) * sizeof(char*));
                    int j;
                    for (j = 0; j < argc; j++) {
                        new_argv[j] = argv[j];
                    }

                    char buf[16];
                    sprintf(buf, "%d", neighbor);
                    new_argv[argc] = buf;
                    new_argv[argc + 1] = NULL;

                    execvp(argv[0], new_argv);
                    exit(1);
                } else {
                    int status;
                    waitpid(pid, &status, 0);

                    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                        exit(0);
                    }
                }
            }
        }
        exit(1);
    }
    return 0;
}