#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    if(fork()){
        printf("Parent started!\n");

        int status;
        wait(&status);
        printf("Child finished!\n");
        exit(0);
    }else{
        printf("Child: executing ls command\n");
        execlp("ls","ls",NULL);
    }
    exit(0);
}