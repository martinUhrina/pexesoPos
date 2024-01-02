//
// Created by Lenovo on 2. 1. 2024.
//

#ifndef PEXESO_PEXESSO_H
#define PEXESO_PEXESSO_H

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <thread>


class Pexesso {
private:
    int rows;
    int columns;
    std::vector<std::vector<char>> board;
    std::vector<std::pair<int, int>> foundPairs;
    std::vector<std::pair<int, int>> revealedPairs;

public:
    Pexesso(int rows, int columns);

    void initialization();
    bool makeGuess(int row1, int col1, int row2, int col2);
    void characters();
    bool allPairsFound();
    void revealPair(int row, int col);
    char getCharAt(int row, int col) const;
    void resetRevealedPairs();
    std::string print();
    void add(char string[1024], int *pInt, int *pInt1);
};

#endif //PEXESO_PEXESSO_H
