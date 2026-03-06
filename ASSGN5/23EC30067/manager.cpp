#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;

#define MAXN 10
int * RQ, *PCB, *T;
int shmid_RQ, shmid_PCB, shmid_T;
int semid_RQ, semid_PCB, semid_T, semid_SYNC;

void myhandler(int sig){
    shmdt(RQ);
    shmdt(PCB);
    shmdt(T);
    shmctl(shmid_RQ, IPC_RMID,0);
    shmctl(shmid_PCB, IPC_RMID,0);
    shmctl(shmid_T, IPC_RMID,0);
    
    semctl(semid_RQ,0,IPC_RMID,0);
    semctl(semid_PCB,0,IPC_RMID,0);
    semctl(semid_T,0,IPC_RMID,0);
    semctl(semid_SYNC,0,IPC_RMID,0);
    exit(0);
};

int main(){

    signal(SIGINT,myhandler);

    key_t key_RQ =ftok("./manager.cpp", 'A');
    key_t key_PCB =ftok("./manager.cpp", 'B');
    key_t key_T =ftok("./manager.cpp", 'C');
    shmid_RQ=shmget(key_RQ,(MAXN+3)*sizeof(int),IPC_CREAT|IPC_EXCL|0777);
    shmid_PCB=shmget(key_PCB,(MAXN*4)*sizeof(int),IPC_CREAT|IPC_EXCL|0777);
    shmid_T=shmget(key_T,5*sizeof(int),IPC_CREAT|IPC_EXCL|0777);

    RQ = (int*)shmat(shmid_RQ,0,0);
    PCB = (int*)shmat(shmid_PCB,0,0);
    T=(int*)shmat(shmid_T,0,0);

    T[0]=0;T[1]=-1;T[2]=-1;T[3]=getpid();T[4]=-1;
    RQ[MAXN+1]=0; //Define front
    RQ[MAXN+2]=0; //Define back
    for(int i=0;i<MAXN;i++){
        PCB[4*i]=-1;
        PCB[4*i+1]=-1;
        PCB[4*i+2]=-1;
        PCB[4*i+3]=0;
    }

    key_t key_sem_RQ=ftok("./manager.cpp",'D');
    key_t key_sem_PCB=ftok("./manager.cpp",'E');
    key_t key_sem_T=ftok("./manager.cpp",'F');
    key_t key_sem_SYNC=ftok("./manager.cpp",'G');
    semid_RQ=semget(key_sem_RQ,1,0777|IPC_CREAT);
    semid_PCB=semget(key_sem_PCB,1,0777|IPC_CREAT);
    semid_T=semget(key_sem_T,1,0777|IPC_CREAT);
    semid_SYNC=semget(key_sem_SYNC,1,0777|IPC_CREAT);

    semctl(semid_RQ, 0, SETVAL, 1);
	semctl(semid_PCB, 0, SETVAL, 1);
    semctl(semid_T, 0, SETVAL, 1);
	semctl(semid_SYNC, 0, SETVAL, 0);

    while(1) pause();
    return 0;
}