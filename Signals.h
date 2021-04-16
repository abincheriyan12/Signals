
#ifndef SIGNALS_SIGNALS_H
#define SIGNALS_SIGNALS_H

#define maxtime 10
#define SIGUSR1 10
#define SIGUSR2 12
#define SIGTERM 15

// shared memory struct
struct sharedvalues
{
    int SIGUSR1sent;
    int SIGUSR1received;
    int SIGUSR2sent;
    int SIGUSR2received;
    int SIGUSR1report;
    int SIGUSR2report;
    int signalcounter;

    int totalsignals;
    pthread_mutex_t mutex;
};

// reporting process struct
struct signals
{
    int signal;
    int time;
};

struct shared_val * pointer;
#endif //SIGNALS_SIGNALS_H
