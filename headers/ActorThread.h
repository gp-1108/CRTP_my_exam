#ifndef ACTORTHREAD_H
#define ACTORTHREAD_H

#include "SharedMemory.h"

struct ActorThreadData {
    struct SharedMemory *sharedMemory;
    struct ConsumerInfo *consumerInfo;
    int consumerCount;
};

void *actor(void *data);

#endif