#include "../headers/ActorThread.h"
#include "../headers/SharedMemory.h"
#include "../headers/ClientThread.h"
#include "../headers/ConsumerThread.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *clientThreadFunc(void *data) {
    struct ClientThreadData *clientData = (struct ClientThreadData *)data;
    struct ActorThreadData *actorData = clientData->actorData;

    // Obtain required information from actorData
    struct SharedMemory *sharedMemory = actorData->sharedMemory;
    struct ConsumerInfo *consumerInfo = actorData->consumerInfo;


    int clientSocket = clientData->clientSocket;

    for (;;) {
        char message[256];
        bzero(message, 256 * sizeof(char));

        snprintf(message, sizeof(message), "Queue Length: %d\n",
                 (sharedMemory->writeIdx - sharedMemory->readIdx + BUFFER_SIZE) % BUFFER_SIZE);
        
        for (int i = 0; i < actorData->consumerCount; i++) {
            snprintf(message + strlen(message), sizeof(message) - strlen(message), "-- consumer %d consumed %d items\n", consumerInfo[i].id, consumerInfo[i].messageCount);
        }

        // Send message
        if (send(clientSocket, message, sizeof(message), 0) < 0) {
            perror("send");
            exit(1);
        }

        sleep(1); // [TODO] move to conditional variable
    }

    close(clientSocket);
    return 0;
}

void *actor(void *arg) {
    struct ActorThreadData *actorData = (struct ActorThreadData *) arg;

    // Initializing the TCP server variables
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    // Creating the TCP server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket");
        exit(1);
    }

    // Initializing the server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost
    serverAddress.sin_port = htons(8080);

    // Allow the socket to be reused
    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    // Binding the server socket to the server address
    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind");
        exit(1);
    }

    // Listening for connections
    if (listen(serverSocket, 5) < 0) {
        perror("listen");
        exit(1);
    }

    for (;;) {
        // Accepting a connection
        clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            perror("accept");
            exit(1);
        }

        // Handling the connection
        pthread_t thread;
        struct ClientThreadData *clientThreadData = malloc(sizeof(struct ClientThreadData));
        clientThreadData->clientSocket = clientSocket;
        clientThreadData->actorData = actorData;

        pthread_create(&thread, NULL, clientThreadFunc, (void*) clientThreadData);

    }

    return 0;
}