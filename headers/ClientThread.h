#ifndef CLIENT_THREAD_H
#define CLIENT_THREAD_H 

#include "ActorThread.h"

struct ClientThreadData {
    struct ActorThreadData *actorData;
    int clientSocket;
};

#endif
