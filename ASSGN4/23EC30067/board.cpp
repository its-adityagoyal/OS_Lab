#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;
int n, fd_write;
int *Mb, *Mp;

void print_board(){
    vector<int> arr(101,-1);
    for(int i=0;i<n;i++){
        arr[Mp[i]]=i;
    }
    for(int i=0;i<n;i++){
        if(Mp[i]==100){
            char c='A'+i;
            cout<<c<<"  ";
        }
    }cout<<'\n';
    cout<<"+---+---+---+---+---+---+---+---+---+---+\n";
    for(int i=9;i>=0;i--){
        if(i%2==0){
            cout<<"|";
            for(int j=1;j<=10;j++){
                int num=i*10+j;
                if(arr[num]!=-1){
                    cout<<" "<<(char)('A'+arr[num])<<" |";
                }else if(num>=10){
                    cout<<" "<<num<<"|";
                }else{
                    cout<<"  "<<num<<"|";
                }
            }
            cout<<"     ";
            for(int j=1;j<=10;j++){
                int num=i*10+j;
                if(Mb[num]!=0){
                    if(Mb[num]<0){
                        cout<<"S("<<num<<" -> "<<num+Mb[num]<<")  ";
                    }else{
                        cout<<"L("<<num<<" -> "<<num+Mb[num]<<")  ";
                    }
                }
            }
            cout<<'\n';

        }else{
            cout<<"|";
            for(int j=10;j>=1;j--){
                int num=i*10+j;
                if(num==100){
                    cout<<num<<"|";
                }else if(arr[num]!=-1){
                    cout<<" "<<(char)('A'+arr[num])<<" |";
                }else{
                    cout<<" "<<num<<"|";
                }
            }
            cout<<"     ";
            for(int j=1;j<=10;j++){
                int num=i*10+j;
                if(Mb[num]!=0){
                    if(Mb[num]<0){
                        cout<<"S("<<num<<" -> "<<num+Mb[num]<<")  ";
                    }else{
                        cout<<"L("<<num<<" -> "<<num+Mb[num]<<")  ";
                    }
                }
            }
            cout<<'\n';
        }
        cout<<"+---+---+---+---+---+---+---+---+---+---+\n";
    }
    for(int i=0;i<n;i++){
        if(Mp[i]==0){
            char c='A'+i;
            cout<<c<<"  ";
        }
    }cout<<'\n';
    fflush(stdout)<<'\n';
}

void myhandler(int sig){
    if(sig==SIGUSR1){
        print_board();
        int save_stdout=dup(1);
        dup2(fd_write,1);
        cout<<"COMPLETED"<<'\n';
        fflush(stdout);
        dup2(save_stdout,1);
        close(save_stdout);
    }else if(sig==SIGINT){
        shmdt(Mb);
        shmdt(Mp);
        exit(0);
    }
}

int main(int argc,char* argv[]) {
    if(argc==3){
        n=stoi(argv[1]);
        fd_write=stoi(argv[2]);
    }

    int save_stdout=dup(1);
    dup2(fd_write,1);
    cout<<getpid()<<'\n';
    fflush(stdout);
    dup2(save_stdout,1);
    close(save_stdout);
    sleep(1);

    key_t key_Mb=ftok("./board.cpp", 'A');
    key_t key_Mp=ftok("./players.cpp",'A');
    int shmid_Mb=shmget(key_Mb,101*sizeof(int),0666);
    int shmid_Mp=shmget(key_Mp,(n+1)*sizeof(int),0666);
    Mb = (int*)shmat(shmid_Mb,0,0);
    Mp = (int*)shmat(shmid_Mp,0,0);

    print_board();

    save_stdout=dup(1);
    dup2(fd_write,1);
    cout<<"START\n";
    fflush(stdout);
    dup2(save_stdout,1);
    close(save_stdout);

    signal(SIGUSR1, myhandler);
    signal(SIGINT, myhandler);
    while (1) pause();
}
