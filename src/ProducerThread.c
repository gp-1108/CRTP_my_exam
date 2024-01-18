#include "../headers/ProducerThread.h"
#include "../headers/SharedMemory.h"
#include <pthread.h>
#include <unistd.h>

void *producer(void *data) {
    struct SharedMemory *sharedMemory = (struct SharedMemory *) data;

    int item = 0;

    // Produce items
    for (;;) {
        // Lock mutex 1->0
        pthread_mutex_lock(&sharedMemory->mutex);

        if ((sharedMemory->writeIdx + 1) % BUFFER_SIZE == sharedMemory->readIdx) {
            pthread_cond_wait(&sharedMemory->roomAvailable, &sharedMemory->mutex);
        }

        // Produce an item
        sharedMemory->buffer[sharedMemory->writeIdx] = item;
        sharedMemory->writeIdx = (sharedMemory->writeIdx + 1) % BUFFER_SIZE;

        item++;

        // Signal that there is data available
        pthread_cond_signal(&sharedMemory->dataAvailable);

        // Unlock mutex 0->1
        pthread_mutex_unlock(&sharedMemory->mutex);


        sleep(2);
    }
}