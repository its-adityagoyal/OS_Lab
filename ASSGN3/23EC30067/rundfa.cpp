#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

#define MAXN 100
#define MAXS 26
#define BUFSIZE 1024
int n,s;
int fd[MAXN][2];
vector<int> Final;
vector<vector<int>> states;

void state_loop(int state_no, int is_final, vector<int>& delta){
    cout<<"\t\t\t\t\t+++ ";
    if(is_final){
        cout<<"Final state "<<state_no<<" created\n";
    }else{
        cout<<"Non-final state "<<state_no<<" created\n";
    }
    int save_stdin=dup(0);
    dup2(fd[state_no][0],0);

    string str;
    while(1){
        cin>>str;
        if(str=="TRANSITION"){
            int save_stdout=dup(1);
            dup2(fd[n][1],1);
            cout<<to_string(state_no)<<'\n';
            fflush(stdout);
            dup2(save_stdout,1);
            close(save_stdout);
        }else if(str=="END"){
            if(is_final){
                cout<<"ACCEPT\n";
            }else{
                cout<<"REJECT\n";
            }
        }else if(str=="QUIT"){
            exit(0);
        }else{
            int num=(int)(str[0]-'a');
            if(num>=0 && num<s){
                cout<<"-- "<<str[0]<<" --> "<<delta[num]<<" ";
                fflush(stdout);
                int save_stdout=dup(1);
                dup2(fd[delta[num]][1],1);
                cout<<"TRANSITION"<<'\n';
                fflush(stdout);
                dup2(save_stdout,1);
                close(save_stdout);
            }else{
                cout<<"INVALID INPUT SYMBOL: "<<str[0]<<'\n';
                fflush(stdout);
                int save_stdout=dup(1);
                dup2(fd[n][1],1);
                cout<<"INVALID"<<'\n';
                fflush(stdout);
                dup2(save_stdout,1);
                close(save_stdout);
            }
        }
    }
    dup2(save_stdin,0);
    close(save_stdin);
    return;
}

void sig_handler(int sig){
    for(int i=0;i<n;i++){
        int save_stdout=dup(1);
        dup2(fd[i][1],1);
        cout<<"QUIT\n";
        fflush(stdout);
        dup2(save_stdout,1);
        close(save_stdout);
    }
    for(int i=0;i<n;i++){
        int status;
        wait(&status);
    }
    cout<<"\n\t\t\t\t\t\t+++ Coordinator: All state processes terminated. Bye!\n";
    exit(0);
}
void user_loop(){
    while(1){
        signal(SIGINT, sig_handler);
        string alpha;
        cout<<"Enter next string: ";
        // getline(cin, alpha);
        cin>>alpha;

        cout<<"0 ";
        fflush(stdout);
        //Initial TRANSITION command
        int save_stdout=dup(1);
        dup2(fd[0][1],1);
        cout<<"TRANSITION"<<'\n';
        fflush(stdout);
        dup2(save_stdout,1);
        close(save_stdout);

        string str;
        int save_stdin=dup(0);
        dup2(fd[n][0],0);
        for(int i=0;i<=alpha.length();i++){
            cin>>str;
            if(str=="INVALID"){
                break;
            }else{
                int current_state=stoi(str);
                fflush(stdout);
                int save_stdout=dup(1);
                dup2(fd[current_state][1],1);
                if(i==alpha.length()){
                    cout<<"END\n";
                }else{
                    cout<<alpha[i]<<'\n';
                }
                fflush(stdout);
                dup2(save_stdout,1);
                close(save_stdout);
            } 
        }
        dup2(save_stdin,0);
        close(save_stdin);
        sleep(1);
    }
    return;
}

int main(int argc, char* argv[]){
    char* file_name = argv[1];
    
    FILE* fp;
    fp = fopen(file_name,"r");

    if (fp==nullptr) {
        cout<<"Error opening the file!\n";
        return 1;
    }

    fscanf(fp,"%d",&s);
    fscanf(fp,"%d",&n);
    Final.resize(n);
    states.resize(n, vector<int>(s));

    for(int i=0;i<n;i++){
        int q;
        fscanf(fp,"%d",&q);
        char c;
        fscanf(fp," %c",&c);
        if(c=='F'){
            Final[q]=1;
        }else{
            Final[q]=0;
        }
        for(int j=0;j<s;j++){
            fscanf(fp, "%d", &(states[q][j]));
        }
    }
    fclose(fp);

    // Checking Block
    // cout<<"s: "<<s<<", n: "<<n<<'\n';
    // for(int i=0;i<n;i++){
    //     cout<<i<<": "<<Final[i]<<" ";
    //     for(auto it: states[i]){
    //         cout<<it<<" ";
    //     }cout<<'\n';
    // }

    for(int i=0;i<=n;i++){
        pipe(fd[i]);
    }

    //Create Child
    for(int i=0;i<n;i++){
        pid_t pid=fork();
        if(pid==0){ //Inside child
            int state_no=i;
            int save_stdin=dup(0);
            int is_final;
            vector<int> delta(s);
            dup2(fd[state_no][0],0);

            cin>>is_final;
            for(auto &it: delta){
                cin>>it;
            }

            dup2(save_stdin,0);
            close(save_stdin);
            state_loop(state_no,is_final,delta);
            exit(0);
        }else{ //Inside parent
            
        }
    }
    sleep(1);
    for(int i=0;i<n;i++){
        int save_stdout=dup(1);
        dup2(fd[i][1],1);
        cout<<Final[i]<<" ";
        for(auto it: states[i]){
            cout<<it<<" ";
        }cout<<'\n';
        fflush(stdout);
        dup2(save_stdout,1);
        close(save_stdout);
    }
    sleep(1);
    cout<<"\t\t\t\t\t\t+++ Coordinator: "<<n<<" state processes are created\n";
    cout<<"\t\t\t\t\t\t+++ Coordinator: Going to user loop\n";
    fflush(stdout);
    user_loop();
    exit(0);
}
