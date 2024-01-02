//
// Created by Lenovo on 2. 1. 2024.
//
#include <iostream>


class Player{
private:
    std::string name;
    int score;

public:
    Player(std::string parName) : name(parName),score(0){};

    std::string getName(){
        return name;
    }
    int getScore(){
        return score;
    }
    void setName(std::string names){
        name=names;
    }
    void setScore(int scores){
        score=scores;
    }
    void updateScore(int extraPoints){
        score+=extraPoints;
    }
    std::string playerInfo(){
        std::string info = "Hráč s menom " + name + " a aktuálnym skóre" + std::to_string(score) + "\n";
        return info;
    }
};
