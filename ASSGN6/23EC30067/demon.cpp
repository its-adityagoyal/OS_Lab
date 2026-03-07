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
vector<pid_t> pid_demon;

void myhandler(int sig){
    for(auto it: pid_demon){
        waitpid(it,nullptr,0);
    }
    cout<<"Demon Parent terminated\n";
    cout.flush();
    exit(0);
}

void cleanup(int sig){
    shmdt(H);
    exit(0);
}
void demon(int i){
    key_t key_sem = ftok("./manager.cpp", 'A');
    key_t key_shm = ftok("./manager.cpp", 'B');
    cond_t CV;
    CV.semid=semget(key_sem,2,0);
    CV.shmid=shmget(key_shm,0,0);

    shmid_H = shmget(ftok("./manager.cpp", 'C'), 0, 0);
    H = (int *)shmat(shmid_H, NULL, 0);

    signal(SIGINT, cleanup);
    srand(getpid()); 
    while(1){
        cout<<"Demon "<<i<<" arrives (D_CNT = "<<DEMON_COUNT<<", N_CNT = "<<NOMAD_COUNT<<", state = "<<state<<")"<<endl;
        cout.flush();
        cond_lock(CV);
        while(state==NOMAD_INSIDE){
            DEMON_WAIT_COUNT++;
            cout<<"Demon "<<i<<" waits (DW_CNT = "<<DEMON_WAIT_COUNT<<")"<<endl;
            cout.flush();

            cond_wait(CV);
            DEMON_WAIT_COUNT--;
        }

        DEMON_COUNT +=1;
        if(DEMON_COUNT==1){
            state=DEMON_INSIDE;
            cout<<"Demon "<<i<<" enters [house empty] (D_CNT = "<<DEMON_COUNT<<", N_CNT = "<<NOMAD_COUNT<<", state = D_INSIDE)"<<endl;
            cout.flush();
        }else{
            cout<<"Demon "<<i<<" enters [other demons present] (D_CNT = "<<DEMON_COUNT<<", N_CNT = "<<NOMAD_COUNT<<", state = D_INSIDE)"<<endl;
            cout.flush();
        }
        cond_unlock(CV);

        double delay = 0.5 + (double)rand() / RAND_MAX * 0.5;
        usleep(delay*1000000);

        cond_lock(CV);
        DEMON_COUNT-=1;
        if(DEMON_COUNT==0){
            state=EMPTY;
            cout<<"Demon "<<i<<" leaves (D_CNT = "<<DEMON_COUNT<<", N_CNT = "<<NOMAD_COUNT<<", state = EMPTY)"<<endl;
            cout.flush();
            if(NOMAD_WAIT_COUNT){
                cond_signal(CV);
            }
        }else{
            cout<<"Demon "<<i<<" leaves (D_CNT = "<<DEMON_COUNT<<", N_CNT = "<<NOMAD_COUNT<<", state = D_INSIDE)"<<endl;
            cout.flush();
            if(NOMAD_WAIT_COUNT){
                cond_signal(CV);
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
            demon(i);
        }else{ //Inside parent
            pid_demon.push_back(pid);
        }
    }
    signal(SIGINT, myhandler);
    for(auto it: pid_demon){
        waitpid(it,nullptr,0);
    }
    return 0;
}