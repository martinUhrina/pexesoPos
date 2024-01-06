//
// Created by Lenovo on 2. 1. 2024.
//
#include <iostream>


class Player{
private:
    std::string name;
    int score;
    std::mutex mtx;
    int mojePoradie;

public:
    Player(std::string parName, int poradie) : name(parName), score(0), mtx(), mojePoradie(poradie) {}

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
    std::mutex& getMutex() {
        return mtx;
    }

    int getPoradie(){
        return mojePoradie;
    }


    void updateScore(){
        score+=2;
        std::cout<<name<<" bolo navýšené o "<<2<<std::endl;
    }
    std::string playerInfo(){
        std::string info = "Hráč s menom " + name + " a aktuálnym skóre" + std::to_string(score) + "\n";
        return info;
    }
};
