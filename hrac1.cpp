#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <thread>
#include "client.cpp"

int main() {
    Client client;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    client.connectToServer("127.0.0.1", 8083); //IP a port
    client.sendMessage("Marcel");

    char buffer[1024];
    std::string userInput;

    int bytesReceived1 = recv(client.clientSocket, buffer, sizeof(buffer), 0);
    buffer[bytesReceived1] = '\0';

    while(userInput != "hraj"){

        std::cout << "Prijatá správa od Servera: " << buffer << std::endl;
        std::cout << "Zadajte správu pre server: ";
        std::getline(std::cin, userInput);
        client.sendMessage(userInput.c_str());
    }
    memset(buffer, 0, sizeof(buffer));
    while(true){
        char buffer[1024];
        int bytesReceived = recv(client.clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived < 0) {
            std::cerr << "\nChyba pri prijímaní odpovede";
            return -1;
        } else if (bytesReceived == 0) {
            std::cout << "\nServer ukončil spojenie" << std::endl;
            break;
        } else {
            buffer[bytesReceived] = '\0';
            if (buffer[0] == '%') {
                std::cout << "\n\nPrijatá správa od servera: \n\n" << buffer + 1 << std::endl; // odstránenie prvého znaku '%'
                do {
                    std::getline(std::cin, userInput);
                } while (userInput.size() == 2 && isdigit(userInput[0]) && isdigit(userInput[1]));

                const char* message = userInput.c_str();

                send(client.clientSocket, message, strlen(message), 0);
            } else if(buffer[0] == '$'){
                std::cout << "\n\nPrijatá správa od servera:\n\n " << buffer + 1 << std::endl; // odstránenie prvého znaku '%'
            } else if(buffer[0] == '@'){
                break;
            }
        }
    }


    return 0;
}