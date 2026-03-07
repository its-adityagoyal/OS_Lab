#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;
#include <cond.h>

# define state H[0]
# define DEMON_COUNT H[1]
# define NOMAD_COUNT H[2]
# define DEMON_WAIT_COUNT H[3]
# define NOMAD_WAIT_COUNT H[4]
# define EMPTY 0
# define DEMON_INSIDE 1
# define NOMAD_INSIDE 2

int *H;
cond_t CV;
int shmid_H, n;
vector<int> pid_nomad;

void myhandler(int sig){
    for(auto it: pid_nomad){
        waitpid(it,nullptr,0);
    }
    cout<<"Nomad Parent terminated\n";
    cout.flush();
    exit(0);
}

void cleanup(int sig){
    shmdt(H);
    exit(0);
}

void nomad(int i){
    key_t key_sem = ftok("./manager.cpp", 'A');
    key_t key_shm = ftok("./manager.cpp", 'B');
    cond_t CV;
    CV.semid=semget(key_sem,2,0);
    CV.shmid=shmget(key_shm,0,0);

    shmid_H = shmget(ftok("./manager.cpp", 'C'), 0, 0);
    H = (int *)shmat(shmid_H, NULL, 0);

    signal(SIGINT, cleanup);
    srand(getpid(   )); 
    while(1){
        cout<<"                 Nomad "<<i<<" arrives (N_CNT = "<<NOMAD_COUNT<<", D_CNT = "<<DEMON_COUNT<<", state = "<<state<<")"<<endl;
        cout.flush();
        cond_lock(CV);
        while(state==DEMON_INSIDE){
            NOMAD_WAIT_COUNT++;
            cout<<"                 Nomad "<<i<<" waits (NW_CNT = "<<NOMAD_WAIT_COUNT<<")"<<endl;
            cout.flush();

            cond_wait(CV);
            NOMAD_WAIT_COUNT--;
        }
        
        NOMAD_COUNT +=1;
        if(NOMAD_COUNT==1){
            state=NOMAD_INSIDE;
            cout<<"                 Nomad "<<i<<" enters [house empty] (N_CNT = "<<NOMAD_COUNT<<", D_CNT = "<<DEMON_COUNT<<", state = N_INSIDE)"<<endl;
            cout.flush();
            if(NOMAD_WAIT_COUNT > 0)
                cond_broadcast(CV); 
        }else{
            cout<<"                 Nomad "<<i<<" enters [other nomads present] (N_CNT = "<<NOMAD_COUNT<<", D_CNT = "<<DEMON_COUNT<<", state = N_INSIDE)"<<endl;
            cout.flush();
        }
        cond_unlock(CV);

        double delay = 0.5 + (double)rand() / RAND_MAX * 0.5;
        usleep(delay*1000000);

        cond_lock(CV);
        NOMAD_COUNT-=1;
        if(NOMAD_COUNT==0){
            state=EMPTY;
            cout<<"                 Nomad "<<i<<" leaves (N_CNT = "<<NOMAD_COUNT<<", D_CNT = "<<DEMON_COUNT<<", state = EMPTY)"<<endl;
            cout.flush();
            if(DEMON_WAIT_COUNT){
                cond_broadcast(CV);
            }
        }else{
            cout<<"Nomad "<<i<<" leaves (N_CNT = "<<NOMAD_COUNT<<", D_CNT = "<<DEMON_COUNT<<", state = N_INSIDE)"<<endl;
            cout.flush();
            if(DEMON_WAIT_COUNT){
                cond_broadcast(CV);
            }
        }
        cond_unlock(CV);
        delay = 1 + (double)rand() / RAND_MAX * 4;
        usleep(delay*1000000);
    }
}

int main(int argc, char *argv[]){
    if(argc==1){
        n=10;
    }else{
        n=stoi(argv[1]);
    }

    for(int i=0;i<n;i++){
        pid_t pid=fork();
        if(pid==0){ //Inside child
            nomad(i);
        }else{ //Inside parent
            pid_nomad.push_back(pid);
        }
    }
    signal(SIGINT, myhandler);
    for(auto it: pid_nomad){
        waitpid(it,nullptr,0);
    }
    return 0;
}