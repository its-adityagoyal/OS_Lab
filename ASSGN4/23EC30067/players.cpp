#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;
#define MAXN 100
int n, fd_write;
pid_t PID_players[MAXN];
pid_t PID_BP;
int turn=0;
int *Mb, *Mp;
int player_no;

void handler_PP(int sig){
    if(sig==SIGUSR1){
        int attempts = 0;
        while(attempts < n){
            pid_t pid = PID_players[turn];
            if(Mp[turn]!=100){   // Player is alive
                kill(pid, SIGUSR1);
                turn = (turn+1)%n;
                break;
            }
            // Player dead → skip
            turn = (turn+1)%n;
            attempts++;
        }
    }else if(sig==SIGUSR2){
        for(int i=0;i<n;i++){
            pid_t pid=PID_players[i];
            if(kill(pid,0)==0){ //Player is alive
                kill(pid,SIGINT);
            }
        }
        while(wait(NULL) > 0);
        shmdt(Mb);
        shmdt(Mp);
        exit(0);
    }
}

void handler_player(int sig){
    if(sig==SIGUSR1){
        //THROW DICE AND MOVE
        cout<<"***********************************************************\n";
        cout<<"Player "<<(char)('A'+player_no)<<": ";
        int pos=Mp[player_no];
        int m=0;
        while(m==0){
            m=1+(rand())%6;
            if(m==6){
                cout<<6<<" ";
                m+=1+(rand())%6;
                if(m==12){
                    cout<<"+ "<<6<<" ";
                    m+=1+(rand())%6;
                    if(m==18){
                        cout<<"+ "<<6<<" X ";
                        m=0;
                    }else{
                        cout<<"+ "<<m-12<<" ";
                    }
                }else{
                    cout<<"+ "<<m-6<<" ";
                }
            }else{
                cout<<m<<" ";
            }
        }cout<<'\n';
        fflush(stdout);
        vector<int> arr(101,-1);
        for(int i=0;i<n;i++){
            arr[Mp[i]]=i;
        }
        int nextpos=pos+m;
        if(nextpos==100){
            Mp[player_no]=nextpos;
            cout<<"Player "<<(char)('A'+player_no)<<" exits with rank = "<<(n-Mp[n])+1<<'\n';
            Mp[n]=Mp[n]-1;
            fflush(stdout);
            kill(PID_BP,SIGUSR1);
            shmdt(Mb);
            shmdt(Mp);
            exit(0);
            
        }else if(nextpos>100){
            cout<<"Move not permitted (cannot go beyond 100)\n";
        }else if(arr[nextpos]!=-1){
            cout<<"Move not permitted (cell already occupied by "<<(char)('A'+arr[nextpos])<<")\n";
        }else{
            while(Mb[nextpos]!=0 && arr[nextpos]==-1){
                if(Mb[nextpos]<0){
                    cout<<"Snake at cell "<<nextpos<<". Jump to cell "<<nextpos+Mb[nextpos]<<'\n';
                }else{
                    cout<<"Ladder at cell "<<nextpos<<". Jump to cell "<<nextpos+Mb[nextpos]<<'\n';
                }
                nextpos+=Mb[nextpos];
            }
            Mp[player_no]=nextpos;
            cout<<"Player "<<(char)('A'+player_no)<<" moves to cell "<<nextpos<<'\n';
        }
        fflush(stdout);
        kill(PID_BP,SIGUSR1);
    }else if(sig==SIGINT){
        shmdt(Mb);
        shmdt(Mp);
        exit(0);
    }
}

void player(pid_t PID_BP, int p){
    srand(getpid());
    player_no=p;
    while (1) pause();
}

int main(int argc, char * argv[]){
    if(argc==4){
        n=stoi(argv[1]);
        fd_write=stoi(argv[2]);
        PID_BP = stoi(argv[3]);
    }
    int save_stdout=dup(1);
    dup2(fd_write,1);
    cout<<getpid()<<'\n';
    fflush(stdout);
    dup2(save_stdout,1);
    close(save_stdout);

    key_t key_Mb=ftok("./board.cpp", 'A');
    key_t key_Mp=ftok("./players.cpp",'A');
    int shmid_Mb=shmget(key_Mb,101*sizeof(int),0666);
    int shmid_Mp=shmget(key_Mp,(n+1)*sizeof(int),0666);
    Mb = (int*)shmat(shmid_Mb,0,0);
    Mp = (int*)shmat(shmid_Mp,0,0);
    signal(SIGUSR1, handler_player);
    signal(SIGINT, handler_player);

    for(int i=0;i<n;i++){
        pid_t pid=fork();
        if(pid!=0){
            PID_players[i]=pid;
        }else{
            //Some work
            player(PID_BP, i);
            exit(0);
        }
    }

    signal(SIGUSR1, handler_PP);
    signal(SIGUSR2, handler_PP);
    while(1) pause();
}
