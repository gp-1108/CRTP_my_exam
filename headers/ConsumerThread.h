#ifndef CONSUMER_INFO_H
#define CONSUMER_INFO_H

struct ConsumerInfo {
    int consumerPid;
    int messageCount;
};

void *consumer(void *data);

#endif