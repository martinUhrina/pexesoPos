#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <thread>
#include <cstring>

void clientHandler(int clientSocket) {
    char buffer[1024];
    bool endConnection = false;
    const char* response = "Zadaj ready: ";
    send(clientSocket, response, strlen(response), 0);
    while (!endConnection) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "Klient odpojený" << std::endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Prijatá správa od klienta: " << buffer << std::endl;

        if (strcmp(buffer, "end") == 0) {
            std::cout << "Klient žiada ukončenie spojenia" << std::endl;
            endConnection = true;
        }
       response = "Komunikujes so serverom";
        // Odpoved klientovi
        send(clientSocket, response, strlen(response), 0);
    }
    close(clientSocket);
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Chyba pri vytváraní socketu";
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;               //protokol IPv4
    serverAddr.sin_port = htons(8081);  // Nastav port
    serverAddr.sin_addr.s_addr = INADDR_ANY;        //

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {           ///naviaze soceket na adresu a port
        std::cerr << "Chyba pri naviazaní socketu na port";
        return -1;
    }

    if (listen(serverSocket, 5) < 0) {                      //nastavi socket do rezimu pocuvania, 5 max pocet pripojeny vo fronte
        std::cerr << "Chyba pri čakaní na spojenie";
        return -1;
    }

    std::vector<std::thread> clientThreads;

    time_t start_time = time(nullptr);
    time_t current_time = time(nullptr);

    while (difftime(current_time, start_time) <= 10) {
     //   std::cout<<"Start>"<<start_time<<std::endl;
    //    std::cout<<"Current>"<<current_time<<std::endl;

        current_time = time(nullptr);
        if (clientThreads.size() >= 5) {
            std::cout << "Dosiahnutý maximálny počet hráčov. Nie je možné prijať ďalších." << std::endl;
            break;
        }

        sockaddr_in clientAddr;
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        if (clientSocket < 0) {
            std::cerr << "Chyba pri akceptovaní spojenia";
            return -1;
        }

        std::cout << "Nový klient pripojený: " << inet_ntoa(clientAddr.sin_addr) << std::endl;

        clientThreads.emplace_back(clientHandler, clientSocket);
    }

    for (auto& thread : clientThreads) {
        thread.join();
    }

    close(serverSocket);
    return 0;
}
