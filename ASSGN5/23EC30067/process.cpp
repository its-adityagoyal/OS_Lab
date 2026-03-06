#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;

#define READY 0
#define RUNNING 1
#define IO 2
#define EXITED 3
#define MAXN 10
#define DELTA 500000
#define delta 100000

int *RQ, *PCB, *T;
int shmid_RQ, shmid_PCB, shmid_T;
int semid_RQ, semid_PCB, semid_T, semid_SYNC;

int serialnumber, arrival_time, priority;
vector<int> burst_times;
int local_time, remaining_time, burst;
bool interrupted = false;

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

int timequantum(int p)
{
    if (p == 0)
        return 10;
    else if (p == 1)
        return 5;
    return 2;
}
void schedule_next(int context_switch)
{
    P(semid_RQ);
    // if(RQ[MAXN+1]==RQ[MAXN+2]){ //Empty
    //     cout<<"["<<local_time<<"] CPU goes idle\n";
    //     cout.flush();
    if (RQ[MAXN + 1] != RQ[MAXN + 2])
    {
        int front = RQ[MAXN + 1];
        int next_process = RQ[front];
        RQ[MAXN + 1] = (front + 1) % (MAXN + 1);

        P(semid_PCB);
        PCB[4 * next_process + 3] = RUNNING;
        int q_next_process = timequantum(PCB[4 * next_process + 2]);
        V(semid_PCB);

        P(semid_T);
        T[1] = next_process;
        T[2] = local_time + q_next_process;
        V(semid_T);

        if (context_switch == -1)
        {
            cout << "[" << local_time << "] Process " << next_process << ": Going from READY to RUNNING with next interrupt time = " << local_time + q_next_process << '\n';
            cout.flush();
        }
        else
        {
            cout << "[" << local_time << "] Process " << serialnumber << ": Context switch to process " << next_process << " with next interrupt time = " << local_time + q_next_process << '\n';
            cout.flush();
        }
    }
    else
    { // Empty
        cout << "[" << local_time << "] CPU goes idle\n";
        cout.flush();
    }
    V(semid_RQ);
}

void myhandler1(int sig)
{
    interrupted = true;
}

void myhandler2(int sig)
{
    shmdt(RQ);
    shmdt(PCB);
    shmdt(T);
    exit(0);
}

int main(int argc, char *argv[])
{
    serialnumber = stoi(argv[1]);
    arrival_time = stoi(argv[2]);
    priority = stoi(argv[3]);

    for (int i = 4; i < 25; i++)
    {
        burst_times.push_back(stoi(argv[i]));
    }

    key_t key_RQ = ftok("./manager.cpp", 'A');
    key_t key_PCB = ftok("./manager.cpp", 'B');
    key_t key_T = ftok("./manager.cpp", 'C');
    shmid_RQ = shmget(key_RQ, 0, 0);
    shmid_PCB = shmget(key_PCB, 0, 0);
    shmid_T = shmget(key_T, 0, 0);

    RQ = (int *)shmat(shmid_RQ, NULL, 0);
    PCB = (int *)shmat(shmid_PCB, NULL, 0);
    T = (int *)shmat(shmid_T, NULL, 0);

    signal(SIGUSR1, myhandler1);
    signal(SIGINT, myhandler2);

    key_t key_sem_RQ = ftok("./manager.cpp", 'D');
    key_t key_sem_PCB = ftok("./manager.cpp", 'E');
    key_t key_sem_T = ftok("./manager.cpp", 'F');
    key_t key_sem_SYNC = ftok("./manager.cpp", 'G');
    semid_RQ = semget(key_sem_RQ, 1, 0);
    semid_PCB = semget(key_sem_PCB, 1, 0);
    semid_T = semget(key_sem_T, 1, 0);
    semid_SYNC = semget(key_sem_SYNC, 1, 0);

    P(semid_PCB);
    PCB[serialnumber * 4] = serialnumber;
    PCB[serialnumber * 4 + 1] = getpid();
    PCB[serialnumber * 4 + 2] = priority;
    PCB[serialnumber * 4 + 3] = READY;
    V(semid_PCB);

    local_time = arrival_time;
    burst = 0;
    remaining_time = burst_times[burst];

    cout << "[" << local_time << "] Process " << serialnumber << ": Arrival with priority = " << priority << '\n';
    cout.flush();

    P(semid_RQ);
    int back = RQ[MAXN + 2];
    RQ[back] = serialnumber;
    RQ[MAXN + 2] = (back + 1) % (MAXN + 1);
    V(semid_RQ);

    P(semid_T);
    bool cpu_idle = (T[1] == -1);
    V(semid_T);

    if (cpu_idle)
        schedule_next(-1);

    if (serialnumber == 0)
    {
        V(semid_SYNC);
    }
    while (true)
    {
        usleep(DELTA);
        local_time++;
        wait_zero(semid_SYNC);

        // cout<<local_time<<" ";
        // After DELTA and incrementing local_time we wait for the timer
        // process to do t++ and as soon as he does we sync it to out local_time
        // P(semid_SYNC);
        P(semid_T);
        local_time = T[0];
        V(semid_T);
        // V(semid_SYNC);
        // cout<<local_time<<'\n';

        P(semid_PCB);
        int state = PCB[4 * serialnumber + 3];
        V(semid_PCB);

        if (state == READY)
        {
        }
        else if (state == RUNNING)
        {
            P(semid_T);
            P(semid_PCB);
            if (T[2] - timequantum(PCB[T[1] * 4 + 2]) != local_time)
            {
                remaining_time--;
            }
            V(semid_PCB);
            V(semid_T);

            // cout << "Process: " << serialnumber << " Local Time: " << local_time << " State: " << state << " Remaining: " << remaining_time << '\n';
            // cout.flush();
            if (remaining_time <= 0)
            { // CPU Burst completed just now
                interrupted = false;
                cout << "[" << local_time << "]" << " Process " << serialnumber << ": CPU burst " << (burst / 2) << " complete\n";
                cout.flush();
                burst++;
                if (burst == 21)
                { // All bursts finished
                    P(semid_PCB);
                    PCB[4 * serialnumber + 3] = EXITED;
                    V(semid_PCB);

                    P(semid_T);
                    T[1] = -1;
                    T[2] = -1;
                    V(semid_T);

                    cout << "         [" << local_time << "] Process " << serialnumber << ": Exiting\n";
                    cout.flush();
                    schedule_next(-1); // Work is done now this process won't come ever back
                    shmdt(RQ);
                    shmdt(PCB);
                    shmdt(T);
                    exit(0);
                }
                else
                {
                    remaining_time = burst_times[burst];
                    P(semid_PCB);
                    PCB[4 * serialnumber + 3] = IO;
                    V(semid_PCB);

                    P(semid_T);
                    T[1] = -1;
                    T[2] = -1;
                    V(semid_T);
                    schedule_next(-1); // Will do IO burst now
                }
            }
            else if (interrupted)
            {
                cout << "[" << local_time << "] Process " << serialnumber << ": Interrupted\n";
                cout.flush();
                P(semid_PCB);
                PCB[4 * serialnumber + 3] = READY;
                V(semid_PCB);

                P(semid_RQ);
                int back = RQ[MAXN + 2];
                RQ[back] = serialnumber;
                RQ[MAXN + 2] = (back + 1) % (MAXN + 1);
                V(semid_RQ);

                P(semid_T);
                T[1] = -1;
                T[2] = -1;
                V(semid_T);

                interrupted = false;
                // This process is interrupted and now we want to schedule
                schedule_next(serialnumber);
            }
        }
        else if (state == IO)
        {
            // P(semid_T);
            // P(semid_PCB);
            // if(T[2]-timequantum(PCB[T[1]*4+2])!=local_time){
            remaining_time--;
            // }
            // V(semid_PCB);
            // V(semid_T);
            if (remaining_time == 0)
            { // IO Completed just now
                cout << "[" << local_time << "]" << " Process " << serialnumber << ": IO burst " << burst / 2 << " complete\n";
                cout.flush();
                burst++;
                remaining_time = burst_times[burst];

                P(semid_PCB);
                PCB[4 * serialnumber + 3] = READY;
                V(semid_PCB);

                P(semid_RQ);
                int back = RQ[MAXN + 2];
                RQ[back] = serialnumber;
                RQ[MAXN + 2] = (back + 1) % (MAXN + 1);
                V(semid_RQ);

                // Either it can do CPU Burst right now or wait in the RQ
                P(semid_T);
                // P(semid_RQ);
                cpu_idle = (T[1] == -1);
                // V(semid_RQ);
                V(semid_T);

                if (cpu_idle)
                    schedule_next(-1);
            }
        }
        else
        {
        }
    }
    return 0;
}