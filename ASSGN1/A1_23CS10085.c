#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>

// Name: Siddhant Singh
// Roll Number: 23CS10085
// Department: Computer Science and Engineering
// Assignment: 1

// Please compile the file as gcc A1_23CS10085.c -o hamcycle
// As I have used the name hamcycle to execute the child processes

// Function to find the number of neighbours and neighbours of a vertex
int *findNeighbours(int *numberOfNeighbours, int vertex) {
    int n;
    FILE *fp;
    fp = fopen("graph.txt", "r");
    // Assuming each line to be maximum 1024 characters long
    char line[1024];
    fgets(line, 1024, fp);
    sscanf(line, "%d", &n);
    for (int i = 1; i < vertex; i++) {
        fgets(line, 1024, fp);
    }
    int *neighbours;
    neighbours = (int *)malloc(n * sizeof(int));
    *numberOfNeighbours = 0;
    fgets(line, 1024, fp);
    int i;
    sscanf(line, "%d", &i);
    if (vertex != i) {
        printf("The graph input file is not properly formatted.\n");
        exit(2);
    }
    char *p;
    p = (strchr(line, '>') + 1);
    if (*p == ' ') {
        while (p) {
            sscanf(p, "%d", &neighbours[*numberOfNeighbours]);
            *numberOfNeighbours = (*numberOfNeighbours + 1);
            p = strchr((p + 1), ' ');
        }
    }
    neighbours = (int *)realloc(neighbours, ((*numberOfNeighbours) * sizeof(int)));
    fclose(fp);
    return neighbours;
}

int main(int argc, char *argv[]) {
    // Setting n, c and the current path
    int n;
    FILE *fp;
    fp = fopen("graph.txt", "r");
    // Assuming each line to be maximum 1024 characters long
    char line[1024];
    fgets(line, 1024, fp);
    sscanf(line, "%d", &n);
    fclose(fp);
    int c;
    int *path;
    path = (int *)malloc(n * sizeof(int));
    if (argc == 1) {
        c = 0;
    }
    else {
        c = 0;
        while (argv[(c + 1)] != NULL) {
            path[c] = atoi(argv[(c + 1)]);
            c = c + 1;
        }
    }

    printf("*** Process %d: ", getpid());
    if (c != 0) {
        for (int i = 0; i < (c - 1); i++) {
            printf("%d ", path[i]);
        }
        printf("%d", path[(c - 1)]);
    }
    printf("\n");

    // Three cases for c
    // For c = 0 child process is executed by adding 1 in arguments
    if (c == 0) {
        int pid;
        pid = fork();
        if (pid < 0) {
            printf("\t*** Error: fork call returned an error.\n");
        }
        else if (pid == 0) {
            char *arguments[] = {argv[0], "1", NULL};
            execvp(argv[0], arguments);
        }
        else {
            int status;
            wait(&status);
            if (status == 0) {
                exit(0);
            }
            else {
                printf("\n");
                printf("No Hamiltonian cycle found\n");
                exit(1);
            }
        }
    }
    // For c = n it is checked if the last node in the path is neighbour to 1, if yes, success(0) is returned and if it is not a neighbour, failure(1) is returned
    if (c == n) {
        int *neighbours;
        int numberOfNeighbours;
        neighbours = findNeighbours(&numberOfNeighbours, path[(n - 1)]);
        for (int i = 0; i < numberOfNeighbours; i++) {
            if (neighbours[i] == 1) {
                if (c != 0) {
                    printf("\n");
                    printf("Hamiltonian cycle found: ");
                    for (int j = 0; j < c; j++) {
                        printf("%d ", path[j]);
                    }
                    printf("%d\n", path[0]);
                }
                exit(0);
            }
        }
        exit(1);
    }
    // For 0 < c < n child is executed by augmenting arguments with a new unvisited node
    if ((c > 0) && (c < n)) {
        int *visitedArray;
        visitedArray = (int *)malloc((n + 1) * sizeof(int));
        for (int i = 0; i < (n + 1); i++) {
            visitedArray[i] = 0;
        }
        for (int i = 0; i < c; i++) {
            visitedArray[path[i]] = 1;
        }
        int *neighbours;
        int numberOfNeighbours;
        neighbours = findNeighbours(&numberOfNeighbours, path[(c - 1)]);
        for (int i = 0; i < numberOfNeighbours; i++) {
            if (visitedArray[neighbours[i]] == 0) {
                int pid;
                pid = fork();
                if (pid < 0) {
                    printf("\t*** Error: fork call returned an error.\n");
                }
                else if (pid == 0) {
                    char **arguments;
                    arguments = (char **)malloc((c + 3) * sizeof(char *));
                    arguments[0] = strdup(argv[0]);
                    for (int j = 0; j < c; j++) {
                        // Assuming that each node number is not more than 8 digits
                        arguments[(j + 1)] = (char *)malloc(8 * sizeof(char));
                        sprintf(arguments[(j + 1)], "%d", path[j]);
                    }
                    arguments[(c + 1)] = (char *)malloc(8 * sizeof(char));
                    sprintf(arguments[(c + 1)], "%d", neighbours[i]);
                    arguments[(c + 2)] = NULL;
                    // Executing child process with augmented arguments which is formed by adding a new unvisited node
                    execvp(argv[0], arguments);
                }
                else {
                    int status;
                    wait(&status);
                    if (status == 0) {
                        exit(0);
                    }
                    else {
                        continue;
                    }
                }
            }
        }
        exit(1);
    }
    return 0;
}
