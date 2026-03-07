#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;
#include <cond.h>

int *H;
cond_t CV;
int shmid_H;

#define state H[0]
#define DEMON_COUNT H[1]
#define NOMAD_COUNT H[2]
#define DEMON_WAIT_COUNT H[3]
#define NOMAD_WAIT_COUNT H[4]
#define EMPTY 0
#define DEMON_INSIDE 1
#define NOMAD_INSIDE 2

void myhandler(int sig)
{
    sleep(0.5); // Letting other processes to detach from shm
    shmctl(shmid_H, IPC_RMID, 0);
    cond_destroy(CV);
    cout<<"Manager terminated\n";
    cout.flush();
    exit(0);
}

int main()
{
    key_t key_sem = ftok("./manager.cpp", 'A');
    key_t key_shm = ftok("./manager.cpp", 'B');
    CV = cond_create(key_sem, key_shm);
    cond_init(CV);

    shmid_H = shmget(ftok("./manager.cpp", 'C'), 5 * sizeof(int), IPC_CREAT | 0777);
    if (shmid_H == -1) {
        perror("shmget");
        exit(1);
    }
    H = (int *)shmat(shmid_H, 0, 0);
    state = EMPTY;
    DEMON_COUNT = 0;
    NOMAD_COUNT = 0;
    DEMON_COUNT = 0;
    NOMAD_COUNT = 0;
    shmdt(H);

    signal(SIGINT, myhandler);
    while (1)
        pause();
    return 0;
}