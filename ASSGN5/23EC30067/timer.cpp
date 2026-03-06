#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;

#define DELTA 500000
#define delta 100000
int *T, *PCB;

void P(int s)
{
    struct sembuf b;
    b.sem_num = 0;
    b.sem_flg = 0;
    b.sem_op = -1;
    semop(s, &b, 1);
}

void V(int s)
{
    struct sembuf b;
    b.sem_num = 0;
    b.sem_flg = 0;
    b.sem_op = 1;
    semop(s, &b, 1);
}

void myhandler(int sig)
{
    shmdt(T);
    shmdt(PCB);
    exit(0);
}

int main()
{
    signal(SIGINT, myhandler);
    key_t key_T = ftok("./manager.cpp", 'C');
    int shmid_T = shmget(key_T, 0, 0);
    T = (int *)shmat(shmid_T, NULL, 0);

    key_t key_PCB = ftok("./manager.cpp", 'B');
    int shmid_PCB = shmget(key_PCB, 0, 0);
    PCB = (int *)shmat(shmid_PCB, NULL, 0);

    key_t key_sem_T = ftok("./manager.cpp", 'F');
    key_t key_sem_PCB = ftok("./manager.cpp", 'E');
    key_t key_sem_SYNC = ftok("./manager.cpp", 'G');
    int semid_T = semget(key_sem_T, 1, 0);
    int semid_PCB = semget(key_sem_PCB, 1, 0);
    int semid_SYNC = semget(key_sem_SYNC, 1, 0);

    // Write PID of timer in T[4]
    P(semid_T);
    T[4] = getpid();
    V(semid_T);

    P(semid_SYNC);

    P(semid_T);
    T[0] = 0;
    V(semid_T);

    V(semid_SYNC);

    while (true)
    {
        usleep(DELTA + delta);

        // P(semid_SYNC);
        P(semid_T);
        T[0]++;
        if (T[1] != -1 && T[0] == T[2])
        {
            int process = T[1];

            P(semid_PCB);
            if (PCB[process * 4 + 3] == 1)
            {
                kill(PCB[process * 4 + 1], SIGUSR1);
            }
            V(semid_PCB);
        }
        V(semid_T);

        P(semid_SYNC);

        usleep(delta);

        V(semid_SYNC);

        // usleep(delta);
    }
    return 0;
}