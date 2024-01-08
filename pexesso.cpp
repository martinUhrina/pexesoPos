#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <thread>

class Pexesso {
private:
    int rows;
    int columns;
    std::vector<std::vector<char>> board;
    std::vector<std::pair<int, int>> foundPairs;
    std::vector<std::pair<int, int>> revealedPairs;

public:
    Pexesso(int rows, int columns) : rows(rows), columns(columns), board(rows, std::vector<char>(columns)) {
        initialization();
    }

    void initialization() {
        std::srand(static_cast<unsigned int>(std::time(0)));
        std::vector<char> chara = {'@', ':', '$', ',', '%', ':', '^', '$', '#', '.', ',', '&', '.', '/', '/', '#', '@', '&', '%', '^'};

        std::random_shuffle(chara.begin(), chara.end());

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < columns; ++j) {
                board[i][j] = chara[i * columns + j];
            }
        }
    }

    bool makeGuess(int row1, int col1, int row2, int col2) {
        if (row1 > rows || row2 > rows || col1 > columns || col2 > columns){
            return false;
        }
        if (board[row1][col1] == board[row2][col2]) {
            foundPairs.emplace_back(row1, col1);
            foundPairs.emplace_back(row2, col2);
            board[row1][col1] = '-';
            board[row2][col2] = '-';
            return true;
        } else {
            revealedPairs.pop_back();
            revealedPairs.pop_back();
            return false;
        }
    }
    void characters() {
        std::cout << "    ";
        for (int j = 0; j < columns; ++j) {
            std::cout << "  " << j << "   ";
        }
        std::cout << '\n';
        for (int i = 0; i < rows; ++i) {
            std::cout << " " << i << " | ";
            for (int j = 0; j < columns; ++j) {
                std::cout << "| " << board[i][j] << " | ";
            }
            std::cout << '\n';
        }
    }

    bool allPairsFound() {
        return foundPairs.size() == static_cast<std::size_t>(rows * columns);
    }

    void revealPair(int row, int col) {
        revealedPairs.emplace_back(row, col);
    }

    char getCharAt(int row, int col) const {
        auto revealedIt = std::find(revealedPairs.begin(), revealedPairs.end(), std::make_pair(row, col));
        auto foundIt = std::find(foundPairs.begin(), foundPairs.end(), std::make_pair(row, col));

        if (revealedIt != revealedPairs.end()) {
            return board[row][col];
        } else if (foundIt != foundPairs.end()) {
            return '-';
        } else {
            return '?';
        }
    }

    void resetRevealedPairs() {
        revealedPairs.clear();
    }

    std::string getPexesso() {
        std::string result;
        result += "    ";
        for (int j = 0; j < columns; ++j) {
            result += "  " + std::to_string(j) + "   ";
        }
        result += '\n';

        for (int i = 0; i < rows; ++i) {
            result += std::to_string(i) + " | ";
            for (int j = 0; j < columns; ++j) {
                result += "| " + std::string(1, getCharAt(i, j)) + " | ";
            }
            result += '\n';
        }

        return result;
    }

};