#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <sstream>
#include <thread>
#include <cstring>
#include "pexesso.cpp"

void clientHandler(int clientSocket, Pexesso& pexeso) {
    char buffer[1024];
    int x = 0, y = 0, x2 = 0, y2 = 0;
    bool druhyTah = false;
    bool endConnection = false;
    int bytesReceived1 = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[bytesReceived1] = '\0';
    std::cout << "Prijatá správa od klienta: " << buffer << std::endl;
    std::string response = "\nAhoj\n\n Vitaj v hre pexeso \n Zadaj ready\n\n";
    send(clientSocket, response.c_str(), response.size(), 0);

    while (!endConnection) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
     //   pexeso.add(buffer, &x, &y);
        std::stringstream ss(buffer);
        if (druhyTah) {
            ss >> x2;
            ss >> y2;
        }
        else{
            ss >> x;
            ss >> y;
        }
        //kontrola pripojenia
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

      //logika hry
        std::cout<<"SUradnice: "<< x << " "<< y<<std::endl;
        std::cout<<"suradnice: "<< x2 << " "<< y2<<std::endl;
        pexeso.revealPair(x, y);
        if (druhyTah){
            pexeso.makeGuess(x, y, x2, y2);
            druhyTah = !druhyTah;
            x2 = 0;
            y2 = 0;
            x = 0;
            y = 0;
        }
        response = pexeso.print();
        // Odpoved klientovi
        send(clientSocket, response.c_str(), response.size(), 0);
        druhyTah = !druhyTah;
    }
    close(clientSocket);
}

int main() {
    Pexesso myPexeso(5,4);

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

        clientThreads.emplace_back(clientHandler, clientSocket, std::ref(myPexeso));
    }

    for (auto& thread : clientThreads) {
        thread.join();
    }

    close(serverSocket);
    return 0;
}
