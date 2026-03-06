#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;

#define DELTA 500000
#define delta 100000
#define MAXN 10
int serialnumber = 0;
int manager_pid, timer_pid;
int *RQ, *PCB, *T;
int shmid_RQ, shmid_PCB, shmid_T;
int semid_RQ, semid_PCB, semid_T, semid_SYNC;
int local_time = 0;
vector<int> child_pids;

void P(int s)
{
    struct sembuf b;
    b.sem_num = 0;
    b.sem_flg = 0;
    b.sem_op = -1;
    semop(s, &b, 1);
}

void V(int s)
{
    struct sembuf b;
    b.sem_num = 0;
    b.sem_flg = 0;
    b.sem_op = 1;
    semop(s, &b, 1);
}
void wait_zero(int s)
{
    struct sembuf b;
    b.sem_num = 0;
    b.sem_flg = 0;
    b.sem_op = 0;
    semop(s, &b, 1);
}

void cleanup(int sig){
    kill(T[4], SIGINT);
    usleep(50000);
    kill(T[3], SIGINT);
    shmdt(T);
    shmdt(RQ);
    shmdt(PCB);
    exit(0);
}

int main()
{
    signal(SIGINT, cleanup);

    key_t key_RQ = ftok("./manager.cpp", 'A');
    key_t key_PCB = ftok("./manager.cpp", 'B');
    key_t key_T = ftok("./manager.cpp", 'C');
    shmid_RQ = shmget(key_RQ, 0, 0);
    shmid_PCB = shmget(key_PCB, 0, 0);
    shmid_T = shmget(key_T, 0, 0);

    RQ = (int *)shmat(shmid_RQ, NULL, 0);
    PCB = (int *)shmat(shmid_PCB, NULL, 0);
    T = (int *)shmat(shmid_T, NULL, 0);

    key_t key_sem_RQ = ftok("./manager.cpp", 'D');
    key_t key_sem_PCB = ftok("./manager.cpp", 'E');
    key_t key_sem_T = ftok("./manager.cpp", 'F');
    key_t key_sem_SYNC = ftok("./manager.cpp", 'G');
    semid_RQ = semget(key_sem_RQ, 1, 0);
    semid_PCB = semget(key_sem_PCB, 1, 0);
    semid_T = semget(key_sem_T, 1, 0);
    semid_SYNC = semget(key_sem_SYNC, 1, 0);

    P(semid_T);
    manager_pid = T[3];
    timer_pid = T[4];
    V(semid_T);

    ifstream file("../bursts10.txt");
    string line;

    vector<int> launch_times, priorities;
    vector<vector<int>> burst_times;

    while (getline(file, line))
    {
        if (line == "-1")
        {
            break;
        }
        vector<int> nums;
        stringstream ss(line);
        int launch_time, priority;

        ss >> launch_time;
        ss >> priority;
        launch_times.push_back(launch_time);
        priorities.push_back(priority);

        vector<int> burst_time(21);
        for (int i = 0; i < 21; i++)
        {
            ss >> burst_time[i];
        }
        burst_times.push_back(burst_time);
    }

    usleep(DELTA);
    local_time++;
    wait_zero(semid_SYNC);

    P(semid_T);
    local_time = T[0];
    V(semid_T);

    cout<<"["<<local_time<<"]"<<" Launcher Ready\n";
    cout.flush();
    
    int n = launch_times.size();
    int i = 0;
    while (i < n)
    {
        usleep(DELTA);
        local_time++;
        wait_zero(semid_SYNC);

        // P(semid_SYNC);
        P(semid_T);
        local_time = T[0];
        V(semid_T);
        // V(semid_SYNC);

        if (launch_times[i] == local_time)
        {
            pid_t pid = fork();
            if (pid == 0)
            { // Inside child
                vector<string> args;
                args.push_back("process");
                args.push_back(to_string(i));
                args.push_back(to_string(launch_times[i]));
                args.push_back(to_string(priorities[i]));
                for (auto it : burst_times[i])
                {
                    args.push_back(to_string(it));
                }
                char *argv[26];
                for (int i = 0; i < 25; i++)
                {
                    argv[i] = &(args[i][0]);
                }
                argv[25] = NULL;
                execv("./process", argv);
            }
            else
            { // Inside parent
                i++;
                child_pids.push_back(pid);
            }
        }
    }
    // Wait for all the processes to terminate
    for (auto it : child_pids)
    {
        waitpid(it, nullptr, 0);
    }
    cout << "         [Launcher] All processes exited\n";
    cout.flush();
    kill(T[4], SIGINT);
    kill(T[3], SIGINT);
    shmdt(T);
    shmdt(RQ);
    shmdt(PCB);
    // waitpid(T[3], nullptr, 0);
    usleep(50000);
    exit(0);
}