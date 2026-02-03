#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

# define MAX 100

int main(){
    const char *msg="hello\n";
    char buf[MAX];
    int p[2];

    pipe(p);
    pid_t pid=fork();
    if(pid==0){
        write(p[1],msg,6);
        printf("Child has written\n");
    }else{
        sleep(1);
        close(p[1]);
        read(p[0],buf, MAX);
        printf("%s",buf);
    }
    return 0; 
}
