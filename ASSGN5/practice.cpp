#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

using namespace std;

int main(){
    key_t key=20;
    int semid=semget(key,1,0666|IPC_CREAT);
    semctl(semid, 0, SETVAL, 1);
    int retval=semctl(semid, 0, GETVAL, 0);
    cout<<retval<<'\n';
    return 0;
}