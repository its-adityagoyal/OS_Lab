#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

#define MAX 100
int main(){
    // int fd[2];
    // pipe(fd);
    // cout<<fd[0]<<" "<<fd[1]<<'\n';
    // write(1,"Hello\n",6); //Writing in file descriptor 1

    // pid_t pid =  fork();
    // const char *message1 = "Hello";
    // const char *message2 = "Hi!";
    // char buffer[MAX];
    // if(pid==0){ //inside child
    //     write(fd[1],message1,strlen(message1)+1);
    //     sleep(1);
    //     read(fd[0],buffer,MAX);
    //     cout<<"Message received from parent: "<<buffer<<'\n';
    // }else{ //inside parent
    //     sleep(1);
    //     read(fd[0],buffer,MAX);
    //     cout<<"Message received from child: "<<buffer<<'\n';
    //     write(fd[1],message2,strlen(message2)+1);
    // }

    // int old_fd = open("output.txt",O_WRONLY|O_CREAT|O_TRUNC, 0644);
    // // close(1);
    // // int new_fd =dup(old_fd);
    // int new_fd = dup2(old_fd,1);
    // cout<<old_fd+1<<" "<<new_fd+1<<endl;

    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    char buffer[MAX];
    if(pid==0){ //Child Process
        close(fd[0]);
        dup2(fd[1],1);
        cout<<"Hello\n";
    }else{ //Parent Process
        close(fd[1]);
        read(fd[0],buffer,MAX);
        cout<<buffer;
    }
    return 0;
}