#ifndef PROJECT2HELPER_H
#define PROJECT2HELPER_H

// Machine IPs
#define P1 "10.176.69.32"
#define P2 "10.176.69.33"
#define P3 "10.176.69.34"
#define P4 "10.176.69.35"
#define BUFFER_SIZE 1024

void createClientSockets(char* serverIPs[], int serverPorts[]);
char* getUserInput();
void createServerSocket(int serverPort);
void serverSend(char message[], int server);
void clientSend(char message[], int client);
void allSend(char message[]);
void* sendMessage(void* arg);
int getMachine();
void selfSendError();
void closeConnections();
void printInstructions();
void* getMessage(void* arg);
#endif // PROJECT2HELPER_H