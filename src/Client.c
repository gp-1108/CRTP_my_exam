#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

volatile sig_atomic_t exit_flag = 0;

void handle_signal(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        exit_flag = 1;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of processes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    int numProcesses = atoi(argv[1]);
    int clientSocket; // Declare clientSocket outside the child process block

    for (int i = 0; i < numProcesses; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {  // Child process
            // Create socket
            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (clientSocket < 0) {
                perror("socket");
                exit(EXIT_FAILURE);
            }

            // Initialize server address
            struct sockaddr_in serverAddress;
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
            serverAddress.sin_port = htons(SERVER_PORT);

            // Connect to the server
            if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
                perror("connect");
                exit(EXIT_FAILURE);
            }

            // Receive and print updates from the server
            char buffer[256];
            ssize_t bytesRead;

            while (!exit_flag) {
                bzero(buffer, 256 * sizeof(char));
                bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

                if (bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    if (bytesRead > 1) {
                        printf("Client %d got message\n%s", getpid(), buffer);
                    }
                } else if (bytesRead == 0) {
                    // Connection closed by the server
                    break;
                } else {
                    perror("Error while receiving data");
                    break;
                }
            }

            // Close the client socket
            close(clientSocket);

            exit(EXIT_SUCCESS);  // Exit the child process
        }
    }

    // Wait for child processes to finish
    for (int i = 0; i < numProcesses; i++) {
        wait(NULL);
    }

    return 0;
}
