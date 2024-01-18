#include "../headers/SharedMemory.h"
#include "../headers/ConsumerThread.h"
#include "../headers/ActorThread.h"
#include "../headers/ProducerThread.h"
#include <pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_CONSUMERS 10

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

    // Create threads
    pthread_t threads[MAX_CONSUMERS + 3]; // +3 for ProducerThread, ActorThread and ActorThread

    // Initialize shared memory
    sharedMemory->readIdx = 0;
    sharedMemory->writeIdx = 0;
    sharedMemory->finished = 0;
    pthread_mutex_init(&sharedMemory->mutex, NULL);
    pthread_cond_init(&sharedMemory->roomAvailable, NULL);
    pthread_cond_init(&sharedMemory->dataAvailable, NULL);

    // Initialize ConsumerThread data
    struct ConsumerInfo *consumerInfo = (struct ConsumerInfo *) malloc(sizeof(struct ConsumerInfo) * MAX_CONSUMERS);
    for (int i = 0; i < MAX_CONSUMERS; i++) {
        consumerInfo[i].id = i;
        consumerInfo[i].messageCount = 0;
        consumerInfo[i].sharedMemory = sharedMemory;
    }

    // Reading from command line number of consumers
    if (argc != 2) {
        printf("Usage: %s <number of consumers>\n", argv[0]);
        exit(1);
    }
    int numberOfConsumers = atoi(argv[1]);
    numberOfConsumers = numberOfConsumers > MAX_CONSUMERS ? MAX_CONSUMERS : numberOfConsumers;

    // Initialize ActorThread data
    struct ActorThreadData actorData;
    actorData.sharedMemory = sharedMemory;
    actorData.consumerInfo = consumerInfo;
    actorData.consumerCount = numberOfConsumers;

    // Init producer thread
    pthread_create(&threads[1], NULL, producer, sharedMemory);

    // Init consumer threads
    for (int i = 0; i < numberOfConsumers; i++) {
        printf("Creating consumer %d\n", i);
        pthread_create(&threads[i + 2], NULL, consumer, &consumerInfo[i]);
    }

    // Init actor thread
    pthread_create(&threads[0], NULL, actor, &actorData);

    // Wait for threads to finish
    for (int i = 0; i < numberOfConsumers + 2; i++) {
        pthread_join(threads[i], NULL);
    }

    // Freeing memory
    pthread_mutex_destroy(&sharedMemory->mutex);
    pthread_cond_destroy(&sharedMemory->roomAvailable);
    pthread_cond_destroy(&sharedMemory->dataAvailable);
    shmdt(sharedMemory);
    shmctl(shmid, IPC_RMID, NULL);
}