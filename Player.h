//
// Created by Lenovo on 2. 1. 2024.
//

#ifndef PEXESO_PLAYER_H
#define PEXESO_PLAYER_H
#include <iostream>

class Player{
private:
    std::string name;
    int score;
public:
    Player(std::string parName,int parScore);

    std::string getName() const;
    int getScore() const;
    void setName(std::string names);
    void setScore(int scores);
    void updateScore(int extraPoints);
    std::string playerInfo() const;
};


#endif //PEXESO_PLAYER_H
