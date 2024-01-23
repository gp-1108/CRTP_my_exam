#include "../headers/ConsumerThread.h"
#include "../headers/SharedMemory.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

void *consumer(void *arg) {
    struct ConsumerInfo *consumerInfo = (struct ConsumerInfo *) arg;
    struct SharedMemory *sharedMemory = consumerInfo->sharedMemory;

    for (;;) {
        // Lock mutex 1->0
        pthread_mutex_lock(&sharedMemory->mutex);

        // Wait if the buffer is empty and not finished
        while (sharedMemory->readIdx == sharedMemory->writeIdx && !sharedMemory->finished) {
            pthread_cond_wait(&sharedMemory->dataAvailable, &sharedMemory->mutex);
        }

        // Check if finished
        if (sharedMemory->finished) {
            pthread_mutex_unlock(&sharedMemory->mutex);
            break;
        }

        // Consume an item
        int item = sharedMemory->buffer[sharedMemory->readIdx];
        sharedMemory->readIdx = (sharedMemory->readIdx + 1) % BUFFER_SIZE;

        consumerInfo->messageCount++;

        // Signal that there is room available
        pthread_cond_signal(&sharedMemory->roomAvailable);

        // Unlock mutex 0->1
        pthread_mutex_unlock(&sharedMemory->mutex);

        // Display message
        printf("Consumer %d consumed %d items\n", consumerInfo->id, consumerInfo->messageCount);

        sleep(2);

    }

    return 0;
}