//Abin Cheriyan
// professor Fiore
// Assignment 4

// test to test the program to make sure it works properly.
#include "Signals.h"

// Function to randomly choose a signal
int randomsignal()
{
    return (rand() % 2) + 1;
}

// Function to sleep for a random interval between .1 and .01
// has the "TEST" to test to see if the functions works
void sleepinterval()
{
    double random = rand();
    double interval = (double) fmod(random, 0.09) + .01;
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

//   Function to initialize the shared  memory reporter, will be used to print the results at the end
void report()
{
    strcpy(shared_pointer->log[0], "System Time: ");
    strcpy(shared_pointer->log[1], "Signal 1 Sent: ");
    strcpy(shared_pointer->log[2], "Signal 2 Sent: ");
    strcpy(shared_pointer->log[3], "Signal 1 Received: ");
    strcpy(shared_pointer->log[4], "Signal 2 Received: ");
    strcpy(shared_pointer->log[5], "Avg Time Between Signal 1: ");
    strcpy(shared_pointer->log[6], "Avg Time Between Signal 2: ");
    //printcurrenttime();
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


// Three signal generating processes that are spawned by the parent process
//Waiting for a random interval between .01 and .1 seconds and randomly choose
//between SIGUR1 or SIGUR2, then send the chosen signal to all child processes
//in the program, increments a shared signal sent counter, one for each
//signal type

void signalgenerator()
{
    int choice;
    int signal;
    while(1)
    {
        sleep(0.5);
        sleepinterval();          //sleep for random interval between .01 and .1
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
        printf("We picked a signal. Signal choice: %d... Preparing to send.\n", signal);
#endif

        kill(0, signal); //kill (0, signal)

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

// Four signal handling processes that are spawned by the parent process
//Two will handle SIGUSR1 and the other two will handle SIGUSR2. First, hooks up
//a signal handling process for its associated signal, and block the
//other signal type by modifying its signal mask. When a signal arrives,
//the handler increments shared signal received counter associated
//with signal type.
void signalhandler(int signal)
{
    if(signal == SIGUSR1)
    {
        pthread_mutex_lock(&shared_pointer -> mutex); //acquire lock
        shared_pointer -> signal1received++;          //increment sigusr1received
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
        shared_pointer -> signal2received++;          //increment sigusr2received
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
//Handles both type of signals, maintains private counter of how many
//signals are received, keeps track of the time of occurrence of each signal
//type. Everytime 10 signals are handled, it reports the system time,
// the shared counters of how many signals of each
//type have been sent/received, and reports the avg time between receptions of
//each signal type for the given 10 signal interval

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

//   One parent process that will spawn all other processes

// spawns child processes, sets up shared
// variables (counters) between processes, sets up mutexes, controls
// the execution duration of the program.

void parentprocess()
{
    blocksigusrs();
    int exitTrue = 0;

    //Creating shared memory space
    shm_id = shmget(IPC_PRIVATE, sizeof(struct shared_val), IPC_CREAT | 0666);
    if(shm_id < 0)
    {
        printf("Error creating shared memory space.\n");
        exit(1);
    }
#ifdef TEST
    printf("Shared memory has been created.\n");
#endif

    //Creating shared pointer for access
    shared_pointer = (struct shared_val*) shmat(shm_id, NULL, 0);
    if(shared_pointer == (struct shared_val *) -1)
    {
        printf("Error creating shared pointer.\n");
        exit(1);
    }
#ifdef TEST
    printf("Shared pointer has been created.\n");
#endif

    //Initializing shared memory
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared_pointer->mutex, &attr);
    shared_pointer->signal1sent = 0;
    shared_pointer->signal2sent = 0;
    shared_pointer->signal1received = 0;
    shared_pointer->signal2received = 0;
    shared_pointer->signal1report = 0;
    shared_pointer->signal2report = 0;

#ifdef TEST
    printf("Shared memory has been initialized.\n");
#endif

    //Setting up the contents of the report
    report();
#ifdef TEST
    printf("Report has been set up.\n");
#endif

    //Start clock when processes are about to be created
    clock_gettime(CLOCK_REALTIME, &shared_pointer->start);
#ifdef TEST
    printf("Clock is starting!\n");
#endif

    //Creating processes
    int i;
    for(i = 0; i < 8; i++)
    {
        pids[i] = fork();
        if(pids[i] == 0) { //child processes

            if( i == 0 || i == 1 )
            {
#ifdef TEST
                printf("Signal handling 1 process created.\n");
#endif
                loopsignal1();
                exit(0);
            }

            if(i == 2 || i == 3)
            {
#ifdef TEST
                printf("Signal handling 2 process created.\n");
#endif
                loopsignal2();
                exit(0);
            }

            if( i == 4 || i == 5 || i == 6 )
            {
#ifdef TEST
                printf("Signal generating process created.\n");
#endif
                signalgenerator();
                exit(0);
            }

            if(i == 7)
            {
#ifdef TEST
                printf("Reporting process created.\n");
#endif
                reportingprocess();
                exit(0);
            }
        }

        else { //parent process
            if(i != 7)
            {
                continue;
            }
#ifdef TIME_EXECUTION
            if(i == 7 && timeelapsed() < 30)
            {
                double time_needed = 30 - timeelapsed();
                sleep(time_needed);
            }
            exitTrue = 1;
#endif
#ifdef SIGNAL_EXECUTION
            int total = shared_pointer->signal1sent + shared_pointer->signal2sent + shared_pointer->signal1received + shared_pointer->signal2received + shared_pointer->signal1report + shared_pointer->signal2report;
            if(i == 7 && !(total >= 100000))
            {
                int total_signals = (shared_pointer->signal1sent + shared_pointer->signal2sent + shared_pointer->signal1received + shared_pointer->signal2received + shared_pointer->signal1report + shared_pointer->signal2report);
                while(total_signals < 100000)
                {
                    sleep(60);
                    total_signals = (shared_pointer->signal1sent + shared_pointer->signal2sent + shared_pointer->signal1received + shared_pointer->signal2received + shared_pointer->signal1report + shared_pointer->signal2report);
                }
                exitTrue = 1;
                break;
            }
#endif
            printf("Time = %f\n", timeelapsed());
        }
    }
    if(exitTrue == 1)
    {
#ifdef TEST
        printf("cleanup! \n");
#endif
        for(i = 0; i < 8; i++)
        { //killing children processes
            kill(pids[i], SIGTERM);
        }
        shmdt(shared_pointer); //detach from shared memory region
#ifdef TEST
        printf("Shared memory has been detached and child processes have been destroyed.\n");
#endif
    }
}

void printcurrenttime()
{
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ("\n\ncurrent system time: %d:%d:%d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

int main()
{
    srand(time(NULL));
    parentprocess();
    printcurrenttime();
    exit(0);
}