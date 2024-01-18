#include "../headers/SharedMemory.h"
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Create shared memory
    int shmid = shmget(IPC_PRIVATE, sizeof(struct SharedMemory), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory
    struct SharedMemory *sharedMemory = (struct SharedMemory *) shmat(shmid, NULL, 0);
    if (sharedMemory == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    // Initialize shared memory
    sharedMemory->readIdx = 0;
    sharedMemory->writeIdx = 0;
    sharedMemory->finished = 0;
    pthread_mutex_init(&sharedMemory->mutex, NULL);
    pthread_cond_init(&sharedMemory->roomAvailable, NULL);
    pthread_cond_init(&sharedMemory->dataAvailable, NULL);



}