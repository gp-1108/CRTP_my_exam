#ifndef CLIENT_THREAD_H
#define CLIENT_THREAD_H 

#include "ActorThread.h"

struct ClientThreadData {
    struct MonitorThreadData *monitorThreadData;
    int clientSocket;
};

void *client(void *data);

#endif
