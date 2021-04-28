//Abin Cheriyan
// professor Fiore
// Assignment 4

#ifndef SIGNALS_SIGNALS_H
#define SIGNALS_SIGNALS_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include<sys/wait.h>
#include <pthread.h>
#include <string.h>
//#define SIGNAL_EXECUTION
//#define TEST
#define TIME_EXECUTION
#define BILLION 1000000000

struct shared_val {
    pthread_mutex_t mutex;
    int signal1sent;
    int signal2sent;
    int signal1received;
    int signal2received;
    char log[7][40];
    struct timespec start;
    int signal1report;
    int signal2report;
};

struct shared_val *shared_pointer;
pthread_mutexattr_t attr;
int shm_id;

pid_t pids[8];
void printcurrenttime();
#endif //SIGNALS_SIGNALS_H
