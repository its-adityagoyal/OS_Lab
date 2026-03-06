#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

typedef struct{
    int semid;
    int shmid;
}cond_t;
void P (int s, int sem_no){
   struct sembuf b;
   b.sem_num = sem_no;
   b.sem_flg = 0;
   b.sem_op = -1;
   semop(s, &b, 1);
}

void V(int s, int sem_no, int op){
   struct sembuf b;
   b.sem_num = sem_no;
   b.sem_flg = 0;
   b.sem_op = op;
   semop(s, &b, 1);
}
cond_t cond_create(key_t key_sem, key_t key_shm){
    cond_t V;
    V.semid = semget(key_sem,2,0777|IPC_CREAT);
    V.shmid = shmget(key_shm,1*sizeof(int),IPC_CREAT|IPC_EXCL|0777);
    return V;
}

void cond_init(cond_t CV){
    semctl(CV.semid, 0, SETVAL, 1);
    semctl(CV.semid, 1, SETVAL, 0);
    int *count=(int*)shmat(CV.shmid,0,0);
	count[0]=0;
    shmdt(count);
}

void cond_lock(cond_t CV){
    P(CV.semid, 0);
}

void cond_unlock(cond_t CV){
    V(CV.semid, 0,1);
}

void cond_wait(cond_t CV){
    int *count=(int*)shmat(CV.shmid,0,0);
    count[0]++;
    shmdt(count);
    cond_unlock(CV);
    sleep(1);  //Simulated atomicity of the combined release and wait using sleep
    P(CV.semid,0);

    cond_lock(CV);
    count=(int*)shmat(CV.shmid,0,0);
    count[0]--;
    shmdt(count);
}

void cond_signal(cond_t CV){
    int *count=(int*)shmat(CV.shmid,0,0);
    if(count[0]>0){
        V(CV.semid,1,1);
    }
    shmdt(count);
}

void cond_boradcast(cond_t CV){
    int *count=(int*)shmat(CV.shmid,0,0);
    if(count[0]==0){
        return;
    }
    V(CV.semid,1,count[0]);
    shmdt(count);
}

void cond_destroy(cond_t CV){
    shmctl(CV.shmid, IPC_RMID,0);
    semctl(CV.semid,0,IPC_RMID,0);
}
