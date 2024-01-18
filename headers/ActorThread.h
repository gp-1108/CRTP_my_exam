#ifndef ACTORTHREAD_H
#define ACTORTHREAD_H

#include "SharedMemory.h"

struct MonitorThreadData {
    struct SharedMemory *sharedMemory;
    struct ConsumerInfo *consumerInfo;
    int consumerCount;
};

void *monitoThread(void *data);

#endif