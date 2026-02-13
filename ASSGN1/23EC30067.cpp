#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

// Function to find neighbours of a vertex
int* findNeighbours(int &numberOfNeighbours, int vertex) {
    ifstream file("graph.txt");
    if (!file.is_open()) {
        exit(1);
    }

    int n;
    file >> n;
    file.ignore(1024, '\n');

    string line;
    for (int i = 1; i < vertex; i++) {
        getline(file, line);
    }

    getline(file, line);

    int *neighbours = new int[n];
    numberOfNeighbours = 0;

    int nodeCheck;
    sscanf(line.c_str(), "%d", &nodeCheck);

    if (vertex != nodeCheck) {
        cout << "The graph input file is not properly formatted.\n";
        exit(2);
    }

    size_t pos = line.find('>');
    if (pos != string::npos) {
        string rest = line.substr(pos + 1);
        char *cstr = new char[rest.length() + 1];
        strcpy(cstr, rest.c_str());

        char *token = strtok(cstr, " ");
        while (token != NULL) {
            neighbours[numberOfNeighbours++] = atoi(token);
            token = strtok(NULL, " ");
        }
        delete[] cstr;
    }

    file.close();
    return neighbours;
}

int main(int argc, char *argv[]) {
    ifstream file("graph.txt");
    if (!file.is_open()) {
        exit(1);
    }

    int n;
    file >> n;
    file.close();

    int c = 0;
    int *path = new int[n];

    if (argc > 1) {
        while (argv[c + 1] != NULL) {
            path[c] = atoi(argv[c + 1]);
            c++;
        }
    }

    cout << "*** Process " << getpid() << ": ";
    for (int i = 0; i < c; i++) {
        cout << path[i] << " ";
    }
    cout << endl;

    // Case 1: c == 0
    if (c == 0) {
        pid_t pid = fork();

        if (pid < 0) {
            cout << "\t*** Error: fork failed.\n";
        }
        else if (pid == 0) {
            char *arguments[] = { argv[0], (char*)"1", NULL };
            execvp(argv[0], arguments);
        }
        else {
            int status;
            wait(&status);

            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                exit(0);
            else {
                cout << "\nNo Hamiltonian cycle found\n";
                exit(1);
            }
        }
    }

    // Case 2: c == n
    if (c == n) {
        int numberOfNeighbours;
        int *neighbours = findNeighbours(numberOfNeighbours, path[n - 1]);

        for (int i = 0; i < numberOfNeighbours; i++) {
            if (neighbours[i] == 1) {
                cout << "\nHamiltonian cycle found: ";
                for (int j = 0; j < c; j++)
                    cout << path[j] << " ";
                cout << path[0] << endl;
                exit(0);
            }
        }
        exit(1);
    }

    // Case 3: 0 < c < n
    if (c > 0 && c < n) {
        int *visited = new int[n + 1];
        for (int i = 0; i <= n; i++)
            visited[i] = 0;

        for (int i = 0; i < c; i++)
            visited[path[i]] = 1;

        int numberOfNeighbours;
        int *neighbours = findNeighbours(numberOfNeighbours, path[c - 1]);

        for (int i = 0; i < numberOfNeighbours; i++) {
            if (!visited[neighbours[i]]) {

                pid_t pid = fork();

                if (pid < 0) {
                    cout << "\t*** Error: fork failed.\n";
                }
                else if (pid == 0) {
                    char **arguments = new char*[c + 3];
                    arguments[0] = strdup(argv[0]);

                    for (int j = 0; j < c; j++) {
                        arguments[j + 1] = new char[16];
                        sprintf(arguments[j + 1], "%d", path[j]);
                    }

                    arguments[c + 1] = new char[16];
                    sprintf(arguments[c + 1], "%d", neighbours[i]);
                    arguments[c + 2] = NULL;

                    execvp(argv[0], arguments);
                }
                else {
                    int status;
                    wait(&status);

                    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                        exit(0);
                }
            }
        }
        exit(1);
    }

    return 0;
}
