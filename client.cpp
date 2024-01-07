#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <thread>
using namespace std;
class Client {
public:
    int clientSocket;

public:
    Client() {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0) {
            std::cerr << "Chyba pri vytváraní socketu";
        }
    }

    void connectToServer(const char* ipAddress, int port) {
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, ipAddress, &serverAddr.sin_addr);

        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Chyba pri pripájaní na server";
        }
    }

    void sendMessage(const char* message) {
        if (send(clientSocket, message, strlen(message), 0) < 0) {
            std::cerr << "Chyba pri odosielaní správy";
        }
    }

    ~Client() {
        close(clientSocket);
    }
};