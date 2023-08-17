#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "project2Helper.h"

// Servers and clients to connect to/connect as
#define NUM_SERVERS 3
#define NUM_CLIENTS 0

int numServers = 3;
int numClients = 0;
int stopped = 0;

//Acts as client for p2, p3, and p4
//Acts as server for NONE


int clientSockets[NUM_SERVERS];
struct sockaddr_in serverAddresses[NUM_SERVERS];
char* serverIPs[NUM_SERVERS] = {P2, P3, P4};
int serverPorts[NUM_SERVERS] = {11145, 11146, 11149};

int serverSocket;
int connectedClients[NUM_CLIENTS];
struct sockaddr_in serverAddress, clientAddress[NUM_CLIENTS];
int serverPort = 11144;



int main(int argc, char const *argv[])
{
    pthread_t sendMessages, getMessages;

    createClientSockets(serverIPs, serverPorts);

    pthread_create(&sendMessages, NULL, sendMessage, NULL);
    pthread_create(&getMessages, NULL, getMessage, NULL);

    pthread_join(sendMessages, NULL);
    pthread_join(getMessages, NULL);

    return 0;
}
