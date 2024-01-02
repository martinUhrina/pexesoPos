#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

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

int main() {
    Client client;
    client.connectToServer("127.0.0.1", 8080); //IP a port
    client.sendMessage("Puso");

    char buffer[1024];
    std::string userInput;

    recv(client.clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Prijatá správa od Servera: " << buffer << std::endl;
    while(userInput != "hraj"){
        std::cout << "Zadajte správu pre server: ";
        std::getline(std::cin, userInput);
        client.sendMessage(userInput.c_str());
        //    recv(client.clientSocket, buffer, sizeof(buffer), 0);

    }
    while(true){

        int bytesReceived = recv(client.clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived < 0) {
            std::cerr << "Chyba pri prijímaní odpovede";
            return -1;
        } else if (bytesReceived == 0) {
            std::cout << "Server ukončil spojenie" << std::endl;
        } else {
            buffer[bytesReceived] = '\0';
            std::cout << "Odpoveď od servera:\n\n " << buffer << std::endl;
        }

        std::string userInput;
        std::cout << "Zadajte správu pre server Cyklus: ";
        std::getline(std::cin, userInput);

        const char* message = userInput.c_str();
        if (send(client.clientSocket, message, strlen(message), 0) < 0) {
            std::cerr << "Chyba pri odosielaní správy";
            break;
        }
        if (userInput == "end") {
            std::cout << "Klient žiada ukončenie spojenia" << std::endl;
            break;
        }

    }


    return 0;
}