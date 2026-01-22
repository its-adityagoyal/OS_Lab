#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

int customer_num;
int booked = 0;
int booking = 0;

void send_request(){

    FILE *fp;
    fp=(FILE *)fopen("request.txt","w");
    // srand(getpid());
    int r=rand()%4 + 1;
    fprintf(fp,"%d %d\n", customer_num, r);
    fflush(fp);
    fclose(fp);

    cout<<"                 Customer     "<<customer_num<<":  Request for "<<r<<" tickets\n";
    kill(getppid(),SIGUSR1);
}

void myhandler1(int sig){
    // Request was sent and now received SIGUSR1
    if(booking){
        booked ++;
        booking = 0;
        cout<<"                 Customer     "<<customer_num<<":  Booking "<<booked<<" successfull\n";
        // Message printed now send back
        kill(getppid(),SIGUSR1);
        return;
    }

    // No request for booking was sent
    if(booked==0){
        booking=1;
        send_request();
    }else if(booked==1){
        // srand(getpid());
        if(rand()%3 ==0){   // Don't need
            cout<<"                 Customer     "<<customer_num<<"   leaves the booking system\n";
            kill(getppid(),SIGUSR2);
        }else{
            booking=1;
            send_request();
        }
    }else{    // Can't take
        cout<<"                 Customer     "<<customer_num<<"   leaves the booking system\n";
        kill(getppid(),SIGUSR2);
    }
}

void myhandler2(int sig){
    if(booking){
        booking=0;
        cout<<"                 Customer     "<<customer_num<<":  Booking "<<booked+1<<" failed\n";
        // Message printed now send back
        kill(getppid(),SIGUSR1);
        return;
    }
}

int main(int argc, char* argv[]){
    customer_num = stoi(argv[1]);
    cout<<"                 Customer     "<<customer_num<<" joins the booking system!\n";
    fflush(stdout);
    srand(getpid());
    signal(SIGUSR1, myhandler1);
    signal(SIGUSR2, myhandler2);
    while (1) pause();
    exit(0);
}