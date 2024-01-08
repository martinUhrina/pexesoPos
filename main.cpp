#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <sstream>
#include <thread>
#include <cstring>
#include <condition_variable>
#include <unordered_map>
#include "pexesso.cpp"
#include "Player.cpp"
using namespace std;

vector<thread> clientThreads;           //inicializacia globalnych/zdielanych premennych
vector<int> clientSockets;
int poradie = 0;
condition_variable cv;
int pocetHracovReady = 0;
unordered_map<string, int> myMap;


void clientHandler(int clientSocket, Pexesso& pexeso) {
    cout<<"Moje poradie je "<<clientThreads.size()<<endl;
    char buffer[1024];                                                                  //setovanie premennych
    int x = -1, y = -1, x2 = -1, y2 = -1;
    bool druhyTah = false;
    bool endConnection = false;
    bool potvrdenie = false;
    int bytesReceivedLobby = recv(clientSocket, buffer, sizeof(buffer), 0);                   //prijatie mena hraca
    if (bytesReceivedLobby <= 0) {
        std::cout << "Klient odpojený" << std::endl;
        close(clientSocket);
    }
    buffer[bytesReceivedLobby] = '\0';
    std::cout << "Pripojil sa: " << buffer << " a jeho socket je "<< clientSocket<< std::endl;
    Player player(buffer, clientThreads.size() - 1);                                            //vytvorenie playera
    std::string response = "\nAhoj " + player.getName() + "\n\nVitaj v hre pexeso \n\n" +"Zadavajete suradnice v tvare(1 1)" +"\nAk si pripravený, zadaj hraj\n\n\0";
    send(clientSocket, response.c_str(), response.size(), 0);
    while(strstr(buffer, "hraj") == nullptr)                            //matchmaking
    {
        bytesReceivedLobby = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceivedLobby <= 0) {
            std::cout << "Klient odpojený" << std::endl;
            break;
        }
        buffer[bytesReceivedLobby] = '\0';
        std::cout << "Prijatá sprava: " << buffer << std::endl;

    }
    myMap[player.getName()] = player.getScore();
    memset(buffer, 0, sizeof(buffer));
    cout<<"\n"<<player.getName()<<" sa pripojil k hre\n";
    string message;

    pocetHracovReady++;                                                     //čakanie na ostatnych hracov
    while (clientThreads.size()  != pocetHracovReady){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    while (!endConnection) {                                                //hra pokial sa klient neodpoji
        std::unique_lock<std::mutex> lock(player.getMutex());
        if (poradie == player.getPoradie()){                                //zistovanie ci je hrac na rade
            int i = 0;
            string poradieVypis;
            for (const auto& pair : myMap) {                                //setovanie vypisu poctu bodov
                poradieVypis += pair.first + ":" + to_string(pair.second) + "\n";
            }
            for (auto& thread : clientThreads) {                                //posielanie spravy kazdemu klientovi

                int clientSocketFromAll = clientSockets[i];
                message = poradieVypis + "\n\n" + pexeso.getPexesso();

                if (pexeso.allPairsFound()){
                    message = "$Vsetky pary boli najedne\n";
                }
                if (i == poradie){
                    if (potvrdenie){
                        message = "%" + message + "\n\n Zadajte a ako potvrdenie: ";        //sprava pre hraca na rade
                    }else{
                        message = "%" + message + "\n\n Zadajte suradnice(x y): ";
                    }
                } else {
                    message = "$" + message + "\n\n Cakajte";                           //sprava pre ostatnych hracov
                }
                i++;
                send(clientSocketFromAll, message.c_str(), message.size(), 0);
            }
        }
        if(poradie == player.getPoradie()){                                             //kod pre hraca na rade
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) {
                std::cout << "Klient odpojený" << std::endl;
                break;
            }
            buffer[bytesReceived] = '\0';
            std::cout << "Prijatá správa od klienta(" + player.getName() +"): " << buffer << std::endl;

            if (strcmp(buffer, "end") == 0) {                                           //ukoncenie spojenia
                std::cout << "Klient žiada ukončenie spojenia" << std::endl;
                endConnection = true;
                poradie++;
                if (poradie + 1 > clientThreads.size()){
                    poradie = 0;
                }
                cv.notify_all();
                pocetHracovReady--;
            }
            if (strcmp(buffer, "a") == 0){                                  //potvrdenie suradnice a vykonanie kontroli
                if (x == x2 && y == y2) {
                    std::cout<<"Zadali sa rovnaké údaje !\n";
                    pexeso.resetRevealedPairs();
                    poradie++;
                    if (poradie + 1 > clientThreads.size()){
                        poradie = 0;
                    }
                }
                else {                                                                  //kontrola ak boli zadane suradnice spravne
                    if (pexeso.makeGuess(x, y, x2, y2)) {
                        std::cout << "Gratulujem, našiel si zhodu!\n";
                        player.updateScore();                                           //update skore
                        myMap[player.getName()] = player.getScore();
                    } else {
                        std::cout << "Bohužiaľ, toto nie je zhoda.\n";
                        pexeso.resetRevealedPairs();
                        poradie++;
                        if (poradie + 1 > clientThreads.size()){
                            poradie = 0;
                        }
                    }
                }
                x2 = -1;
                y2 = -1;
                x = -1;
                y = -1;
                potvrdenie = false;
                druhyTah = false;
                cv.notify_all();
                continue;
            } else {                                                            //nacitanie prvej a druhej suradnice
                std::stringstream ss(buffer);
                if (druhyTah) {
                    cout<<"\n Druhy tah: " + druhyTah<<endl;
                    ss >> x2;
                    ss >> y2;
                }
                else{
                    ss >> x;
                    ss >> y;
                }
                std::cout<<"1.súradnice: "<< x << " "<< y<<std::endl;
                std::cout<<"2.súradnice: "<< x2 << " "<< y2<<std::endl;
            }
            if (druhyTah && strcmp(buffer, "a") != 0){                                      //setovanie druhej suradnice
                pexeso.revealPair(x2, y2);
                cv.notify_all();
                potvrdenie = !potvrdenie;
            }else if (strcmp(buffer, "a") != 0) {                                           //setovanie prvej suradnice
                pexeso.revealPair(x, y);
                cv.notify_all();
                cout<<"\n menim druhy stav\n"<<endl;
                druhyTah = !druhyTah;
            }
        }
        else{
            cout<<player.getName()<<" stopol som"<<endl;                                //ak nie je hrac na rade tak sa stopne
            cv.wait(lock);
            lock.unlock();
            cout<<"\n Zobudil som sa "<<player.getName()<<endl;

        }
    }
    close(clientSocket);            //odpojenie


}

int main() {
    Pexesso myPexeso(5,4);
    myPexeso.characters();                  //vytvorenie pexesa

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Chyba pri vytváraní socketu";
        return -1;
    }

    cout<<"Server sa spustil"<<endl;
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;               //protokol IPv4
    serverAddr.sin_port = htons(8083);          // Nastav port
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {           //naviaze soceket na adresu a port
        std::cerr << "Chyba pri naviazaní socketu na port";
        return -1;
    }

    if (listen(serverSocket, 5) < 0) {                      //nastavi socket do rezimu pocuvania, 5 max pocet pripojeny vo fronte
        std::cerr << "Chyba pri čakaní na spojenie";
        return -1;
    }

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
        clientSockets.push_back(clientSocket);
        clientThreads.emplace_back(clientHandler, clientSocket, std::ref(myPexeso));
    }

    for (auto& thread : clientThreads) {
        thread.join();
    }

    close(serverSocket);
    return 0;
}
