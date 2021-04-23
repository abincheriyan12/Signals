//Abin Cheriyan
// professor Fiore
// Assignment 4

// test to test the program to make sure it works properly.
#include "Signals.h"

void printcurrenttime()
{
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ("\n\ncurrent system time: %d:%d:%d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}
// Function to generate signal 1 or signal 2
int randomsignal()
{
    return (rand() % 2) + 1;
}

// Function to sleep for a random interval between .1 and .01
void sleepinterval()
{
    double random = rand();
    double interval = (double) fmod(random, 0.1) + .01;
    int microseconds = interval * 1000000;
    #ifdef TEST
    printf("Sleeping for %f seconds.\n", interval);
    #endif
    usleep(microseconds);
}

// Function to get time elapsed from the start
double timeelapsed()
{
    struct timespec end;
    double elapsed;
    clock_gettime(CLOCK_REALTIME, &end);
    if((end.tv_nsec - shared_pointer -> start.tv_sec) < 0)
    {
        elapsed = (end.tv_sec - shared_pointer -> start.tv_sec - 1) + (BILLION+ end.tv_nsec - shared_pointer -> start.tv_nsec) / BILLION;
    }
    else {
        elapsed = (end.tv_sec - shared_pointer -> start.tv_sec) + (end.tv_nsec - shared_pointer -> start.tv_nsec) / BILLION;
    }
    return elapsed;
}

void report()
{
    strcpy(shared_pointer->log[0], "System Time: ");
    strcpy(shared_pointer->log[1], "Signal 1 Sent: ");
    strcpy(shared_pointer->log[2], "Signal 2 Sent: ");
    strcpy(shared_pointer->log[3], "Signal 1 Received: ");
    strcpy(shared_pointer->log[4], "Signal 2 Received: ");
    strcpy(shared_pointer->log[5], "Avg Time Between Signal 1: ");
    strcpy(shared_pointer->log[6], "Avg Time Between Signal 2: ");
}


void unblocksigusrs()
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);
}

void blocksigusrs()
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}

void blocksigusr1()
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}

void blocksigusr2()
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR2);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}
//Three signal generating processes that are spawned by the parent process

void signalgenerator()
{

    int choice;
    int signal;
    while(1)
    {
        sleepinterval(); //sleep for random interval between .01 and .1
        choice = randomsignal(); //signal = random signal
        if(choice == 1)
        {
            signal = SIGUSR1;
        }
        if(choice == 2)
        {
            signal = SIGUSR2;
        }
        #ifdef TEST
        printf("picked a signal. Signal choice: %d... Preparing to send.\n", signal);
        #endif

        kill(0, signal);

        if(signal == SIGUSR1)
        {
            pthread_mutex_lock(&shared_pointer -> mutex); //acquire lock
            shared_pointer -> signal1sent++;  //sigusr1 sent counter ++
            #ifdef TEST
            printf("Incrementing sent counter for signal 1 by 1!\n");
            printf("New value: %d\n", shared_pointer -> signal1sent);
            printf("Current time: %.10f\n", timeelapsed());
            #endif
            pthread_mutex_unlock(&shared_pointer -> mutex); //release lock
        }

        if(signal == SIGUSR2)
        {
            pthread_mutex_lock(&shared_pointer -> mutex); //acquire lock
            shared_pointer -> signal2sent++; //sigusr2 sent counter ++
            #ifdef TEST
            printf("Incrementing sent counter for signal 2 by 1!\n");
            printf("New value: %d\n", shared_pointer -> signal2sent);
            printf("Current time: %.10f\n", timeelapsed());
            #endif
            pthread_mutex_unlock(&shared_pointer -> mutex); //release lock
        }
    }
}
//Four signal handling processes that are spawned by the parent process
void signalhandler(int signal)
{
    if(signal == SIGUSR1)
    {
        pthread_mutex_lock(&shared_pointer -> mutex); //acquire lock
        shared_pointer -> signal1received++; //increment sigusr1received
        #ifdef TEST
        printf("Incrementing received counter for signal 1 by 1!\n");
        printf("New value: %d\n", shared_pointer -> signal1received);
        printf("Current time: %.10f\n", timeelapsed());
        printcurrenttime();
        #endif
        pthread_mutex_unlock(&shared_pointer -> mutex); //release lock
    }
}


void signalhandler2(int signal)
{
    if(signal== SIGUSR2)
    {
        pthread_mutex_lock(&shared_pointer -> mutex); //acquire lock
        shared_pointer -> signal2received++; //increment sigusr2received
        #ifdef TEST
        printf("Incrementing received counter for signal 2 by 1!\n");
        printf("New value: %d\n", shared_pointer -> signal2received);
        printf("Current time: %.10f\n", timeelapsed());
        printcurrenttime();
        #endif
        pthread_mutex_unlock(&shared_pointer -> mutex); //release lock
    }
}

void loopsignal1()
{
    unblocksigusrs();
    blocksigusr2();
    while(1)
    {
        signal(SIGUSR1, signalhandler);
        pause();
    }
}

void loopsignal2()
{
    unblocksigusrs();
    blocksigusr1();
    while(1)
    {
        signal(SIGUSR2, signalhandler2);
        pause();
    }
}

// One reporting process that is spawned by the parent process
void reportinghandler1(int sig)
{
    if(sig == SIGUSR1)
    {
        pthread_mutex_lock(&shared_pointer -> mutex); //acquire lock
        shared_pointer -> signal1report++;
        pthread_mutex_unlock(&shared_pointer -> mutex); //acquire lock
    }
}

void reportinghandler2(int sig)
{
    if(sig == SIGUSR2)
    {
        pthread_mutex_lock(&shared_pointer -> mutex); //acquire lock
        shared_pointer -> signal2report++;
        pthread_mutex_unlock(&shared_pointer -> mutex); //acquire lock
    }
}

void reportingprocess()
{
    unblocksigusrs();
    int last = 0;
    int current;
    int cyclecounts = 0;
    double currentavg1 = 0;
    double currentavg2 = 0;
    while(1)
    {
        current = shared_pointer -> signal1report + shared_pointer -> signal2report;
        if( (current - last) >= 10 )
        {
            cyclecounts++;
            currentavg1 = currentavg1 + (((timeelapsed()/(shared_pointer -> signal1report)) /cyclecounts));
            currentavg2 = currentavg2 + (((timeelapsed()/(shared_pointer -> signal2report)) /cyclecounts));
            printf("\n CYCLE NUMBER %d \n", cyclecounts);
            printf("%s %lf\n", shared_pointer -> log[0], timeelapsed());
            printf("%s %d\n", shared_pointer  -> log[1], shared_pointer -> signal1sent);
            printf("%s %d \n", shared_pointer -> log[2], shared_pointer -> signal2sent);
            printf("%s %d \n", shared_pointer -> log[3], shared_pointer -> signal1received);
            printf("%s %d \n", shared_pointer -> log[4], shared_pointer -> signal2received);
            printf("%s %f \n", shared_pointer -> log[5], currentavg1);
            printf("%s %f \n", shared_pointer -> log[6], currentavg2);
            last = current;
        }
        signal(SIGUSR1, reportinghandler1);
        signal(SIGUSR2, reportinghandler2);
        pause();
    }
}