#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;
int n;
pid_t PID_BP;
pid_t PID_PP;


int main(int argc, char* argv[]){
    n=stoi(argv[1]);
    // cout<<"Number of players, n: "<<n<<'\n';
    int shmid_Mb,shmid_Mp;

    key_t key_Mb=ftok("./board.cpp", 'A');
    key_t key_Mp=ftok("./players.cpp",'A');
    shmid_Mb=shmget(key_Mb,101*sizeof(int),IPC_CREAT|IPC_EXCL|0777);
    shmid_Mp=shmget(key_Mp,(n+1)*sizeof(int),IPC_CREAT|IPC_EXCL|0777);

    int *Mb, *Mp;
    Mb = (int*)shmat(shmid_Mb,0,0);
    Mp = (int*)shmat(shmid_Mp,0,0);

    if (Mb == (void*)-1) {
        perror("shmat Mb failed");
        exit(1);
    }
    if (Mp == (void*)-1) {
        perror("shmat Mp failed");
        exit(1);
    }

    for(int i=0;i<=100;i++){
        Mb[i]=0;
    }
    for(int i=0;i<=n;i++){
        Mp[i]=0;
        if(i==n) Mp[i]=n;
    }

    
    char* file_name=argv[2];
    FILE* fp = fopen(file_name,"r");
    if(fp == nullptr){
        cout<<"Error opening the file!\n";
        shmdt(Mb);
        shmdt(Mp);
        shmctl(shmid_Mb, IPC_RMID, 0);
        shmctl(shmid_Mp, IPC_RMID, 0);
        return 1;
    }

    char c;
    fscanf(fp,"%c",&c);
    do{
        if(c=='L'){
            int bottom_end, top_end;
            fscanf(fp," %d %d",&bottom_end,&top_end);
            Mb[bottom_end]=top_end-bottom_end;
            // cout<<c<<" "<<bottom_end<<" "<<top_end<<'\n';
        }else if(c=='S'){
            int bottom_end, top_end;
            fscanf(fp," %d %d",&top_end,&bottom_end);
            Mb[top_end]=bottom_end-top_end;
            // cout<<c<<" "<<top_end<<" "<<bottom_end<<'\n';
        }
        fscanf(fp," %c",&c);
    }while(c!='E');

    // for(int i=1;i<=100;i++){
    //     cout<<Mb[i]<<" ";
    // }cout<<'\n';
    // fflush(stdout);

    int fd[2];
    pipe(fd);

    pid_t pid1=fork();
    if(pid1 == 0){ //Inside child XBP

        char n_str[10];
        sprintf(n_str, "%d", n);
        char fd_write_str[10];
        sprintf(fd_write_str,"%d",fd[1]);

        execlp("xterm","xterm", "-T", "Board", "-fs", "15", "-geometry", "150x30+50+100", "-bg", "#56bd57","-e","./board",n_str,fd_write_str, NULL);
    }else{  //Inside parent
        int save_stdin=dup(0);
        dup2(fd[0],0);
        cin>>PID_BP;
        dup2(save_stdin,0);
        close(save_stdin);

        pid_t pid2=fork();

        if(pid2==0){ //Inside child XPP
            char n_str[10];
            sprintf(n_str, "%d", n);
            char fd_write_str[10];
            sprintf(fd_write_str,"%d",fd[1]);
            char pid_BP_str[10];
            sprintf(pid_BP_str,"%d",PID_BP);

            execlp("xterm","xterm", "-T", "Players", "-fs", "15", "-geometry", "100x30+1000+100", "-bg", "#71a1d1","-e", "./players",n_str,fd_write_str,pid_BP_str, NULL);
        }else{ //Inside parent
            int save_stdin=dup(0);
            dup2(fd[0],0);
            cin>>PID_PP;
            dup2(save_stdin,0);
            close(save_stdin);
            // cout<<"PID of PP: "<<PID_PP<<" & PID of BP: "<<PID_BP<<'\n';

            string str;
            save_stdin=dup(0);
            dup2(fd[0],0);
            cin>>str;
            dup2(save_stdin,0);
            close(save_stdin);
            if(str=="START"){
                cout<<"LET'S START THE GAME!\n";
                fflush(stdout);
            }

            while(true){
                cin >> str;
                if(str=="next"){
                    kill(PID_PP,SIGUSR1);
                    int save_stdin = dup(0);
                    dup2(fd[0],0);
                    cin>>str;
                    dup2(save_stdin,0);
                    close(save_stdin);
                    if(str=="COMPLETED"){
                        if(Mp[n]==0){
                            break;
                        }
                        //READY FOR NEXT
                    }
                }else if(str=="QUIT"){
                    break;
                }else if(str=="delay"){
                    int delay_ms=1000;
                    cin>>str;
                    delay_ms=stoi(str);
                    cout<<"autoplay\n";
                    cout<<"Starting autoplay...\n";
                    fflush(stdout);
                    while(true){
                        kill(PID_PP,SIGUSR1);
                        int save_stdin = dup(0);
                        dup2(fd[0],0);
                        cin>>str;
                        dup2(save_stdin,0);
                        close(save_stdin);
                        if(str=="COMPLETED"){
                            if(Mp[n]==0){
                                break;
                            }
                            //READY FOR NEXT
                        }
                        usleep(delay_ms * 1000);
                    }
                    cout<<"Hit return to end the game...\n";
                    fflush(stdout);
                    cin>>str;
                    if(str=="return"){
                        break;
                    }
                }
            }

            int status;
            kill(PID_PP,SIGUSR2);
            waitpid(pid2,&status,0);
            kill(PID_BP,SIGINT);
            waitpid(pid1,&status,0);
        }
    }
    
    shmdt(Mb);
    shmdt(Mp);
    shmctl(shmid_Mb, IPC_RMID, 0);
    shmctl(shmid_Mp, IPC_RMID, 0);
    return 0;
}