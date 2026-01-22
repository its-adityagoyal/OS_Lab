#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    int X=100;
    int *Y=(int*)malloc(sizeof(int));
    *Y=200;
    printf("Before forking, X= %d and Y= %d\n",X,*Y);

    pid_t pid;
    if((pid=fork())==0){
        printf("Inside Child:\n");
        printf("Address: X= %p and Y= %p\n", &X, Y);
        X += 10;
        *Y += 20;
        printf("X= %d and y= %d\n",X,*Y);
        exit(10);
    }else{
        printf("Inside Parent:\n");
        printf("Address: X= %p and Y= %p\n", &X, Y);
        X += 5;
        *Y += 10;
        printf("X= %d and y= %d\n",X,*Y);

        int status;
        wait(&status);

        if(WIFEXITED(status)){
            printf("Child terminated with code: %d\n",WEXITSTATUS(status));
        }
    }
    if(pid) sleep(1);
    free(Y);
    exit(0);
}
