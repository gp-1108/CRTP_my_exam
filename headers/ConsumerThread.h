#ifndef CONSUMER_INFO_H
#define CONSUMER_INFO_H

#include "SharedMemory.h"

struct ConsumerInfo {
    int id;
    int messageCount;
    struct SharedMemory *sharedMemory;
};

void *consumer(void *data);

#endif