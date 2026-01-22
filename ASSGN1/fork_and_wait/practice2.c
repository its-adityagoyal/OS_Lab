#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    pid_t pid=fork();

    if(pid){
        printf("INSIDE PARENT, PID = %d\n",pid);
        printf("GETPID = %d & GETPPID = %d\n", getpid(), getppid());
        wait(NULL);
    }else{
        printf("INSIDE CHILD, PID = %d\n",pid);
        printf("GETPID = %d & GETPPID = %d\n", getpid(), getppid());
    }
    exit(0);
}