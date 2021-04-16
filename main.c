#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include "Signals.h"

int sleeping (long time);
double randomNumber(int min, int max);
int randomuser();


int main ()
{
    int shmid;
    pid_t pid;

    shmid = shmget(IPC_PRIVATE, sizeof(struct sharedvalues), IPC_CREAT | 0666);
    assert(shmid >= 0); // error check memory creation
    pointer = (struct shared_val *) shmat(shmid, NULL, 0); // attach memory
    assert(pointer != (struct shared_val *) -1); // error check memory attachment

    pointer -> SIGUSR1sent = 0;
    pointer -> SIGUSR1received = 0;
    pointer -> SIGUSR2sent = 0;
    pointer -> SIGUSR2received = 0;
    pointer -> SIGUSR1report = 0;
    pointer -> SIGUSR2report = 0;
    pointer -> signalcounter = 0;

    CREATING PROCESS -> FORKING , ETC ETC

}

double randomNumber(int min, int max)
{
    long number= (long) ((rand() % (max - min + 1)) + min);
    return number;
}

// randomly returns either SIGUSR1 or SIGUSR2
int randomuser()
{
    int n = randomNumber(1, 2);
    if(n == 1) {
        return SIGUSR1;
    } else {
        return SIGUSR2;
    }

}

int sleeping (long time)
{
    int ret;
    struct timespec ts;
    assert(time >= 0);
    ts.tv_sec = time / 1000;
    ts.tv_nsec = (time % 1000) * 1000000;
    do
    {
        ret = nanosleep(&ts, &ts);
    } while (ret && errno == EINTR);

}

void EXITING()
{
    for (int i = 0; i < 8; i++)
    {
        kill processes; 
    }

    shmdt(shm_ptr);
    exit(0);

}