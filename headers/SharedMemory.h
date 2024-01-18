#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <sys/shm.h>
#include <pthread.h>

#define BUFFER_SIZE 1000

struct SharedMemory {
    pthread_mutex_t mutex;
    pthread_cond_t roomAvailable, dataAvailable;
    int buffer[BUFFER_SIZE];
    int readIdx;
    int writeIdx;
    int finished;
};

#endif