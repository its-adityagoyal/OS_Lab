#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

int t,n;
pid_t customer_pid[10002];
queue<int> q;
int wait_confirmation=0;
int customer_num;

void create_queue(){
    vector<int> arr(n);
    for(int i=0;i<n;i++) arr[i]=i+1;
    srand(time(NULL));
    for(int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(arr[i], arr[j]);
    }
    for(auto it: arr){
        q.push(it);
    }
}

void create_child(){
    for(int i=1;i<=n;i++){
        pid_t pid =fork();
        if(pid ==0){  // customer process
            char customer_num_str[20];
            sprintf(customer_num_str, "%d", i);
            execlp("./customer","customer",customer_num_str,NULL);
        }else{        // Parent process
            customer_pid[i]=pid;
        }
    }
    sleep(1);
}

void end(){
    while(!q.empty()){
        customer_num=q.front();
        q.pop();
        cout<<customer_num<<" ";
        kill(customer_pid[customer_num], SIGKILL);
    }
    cout<<'\n';
}

void print(){
    cout<<"Agent: Queue = ( ";
    vector<int> temp;
    while(!q.empty()){
        temp.push_back(q.front());
        q.pop();
    }
    for(auto it: temp){
        cout<<it<<" ";
        q.push(it);
    }
    cout<<") Available = "<<t<<'\n';
}

void ticket_counter(){
    if(t<=0){
        cout<<"Agent terminates customers ";
        end();
        cout<<"Agent: Booking session over (no more tickets available)\n";
        exit(0);
    }else if(q.empty()){
        end();
        cout<<"Agent: Booking session over (no more customers available)\n";
        exit(0);
    }else{
        print();
        customer_num = q.front();
        q.pop();
        kill(customer_pid[customer_num],SIGUSR1);
    }
}
void myhandler1(int sig){
    if(wait_confirmation==1){
        wait_confirmation=0;
        q.push(customer_num);
        ticket_counter();
    }else{
        int r;
        FILE * fp;
        fp = (FILE*)fopen("request.txt","r");
        fscanf(fp, "%d %d", &customer_num, &r);
        fclose(fp);

        wait_confirmation=1;
        if(r<=t){
            t-=r;
            kill(customer_pid[customer_num],SIGUSR1);
        }else{
            kill(customer_pid[customer_num],SIGUSR2);
        }
    }
}

void myhandler2(int sig){
    kill(customer_pid[customer_num],SIGKILL);
    ticket_counter();
}

// argc = argument count & argv =  argument vector
int main(int argc, char* argv[]){
    if(argc!=3){
        cout << "Enter arguments : " << endl;
        exit(0);
    }else{
        t=stoi(argv[1]);
        n=stoi(argv[2]);
    }
    // cout<<"Number of tickets: "<<t<<" & Number of customers: "<<n<<'\n';

    create_queue();
    create_child();

    signal(SIGUSR1, myhandler1);
    signal(SIGUSR2, myhandler2);

    // for(int i=1;i<=n;i++){
    //     wait(NULL);
    // }
    // for(int i=1;i<=n;i++){
    //     cout<<"Customer "<<i<<" : "<<customer_pid[i]<<'\n';
    // }
    ticket_counter();
    while (1) pause();
    exit(0);
}