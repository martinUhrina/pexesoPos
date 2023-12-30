//
// Created by Lenovo on 30. 12. 2023.
//

#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>

class Pexesso{
private:
    int rows;
    int columns;
    std::vector<std::vector<char>> board;
public:
    Pexesso(int rows, int columns) : rows(rows),columns(columns),board(rows, std::vector<char>(columns)){
        initialization();
    }
    void initialization(){

        std::srand(std::time(0));
        std::vector<char> chara = {'/',':','.',',','#','!','$','+','%','*','@','&','^'};

        std::random_shuffle(chara.begin(),chara.end());
        std::vector<char> condition;
        for (char c : chara){
            condition.push_back(c);
            condition.push_back(c);
        }

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < columns ; ++j) {
                board[i][j] = condition[i * columns + j];
            }

        }
        for (int i = rows-1; i > 0 ; --i) {
            for (int j = columns-1; j > 0 ; --j) {
                char randRow = std::rand() % (i+1);
                char randCol = std::rand() % (j+1);
                std::swap(board[i][j],board[randRow][randCol]);

            }
        }
    }

    void print(bool show) {

        std::cout << "  ";
        for (int j = 0; j < columns; ++j) {
            std::cout << "  " << j << "   ";
        }
        std::cout << '\n';

        char rowLabel = 'A';
        for (int i = 0; i < rows; ++i) {
            std::cout << rowLabel++ << " ";
            for (int j = 0; j < columns; ++j) {
                if(show){
                    std::cout << "| " << board[i][j] << " | ";
                }else {
                    std::cout << "| "<<'?'<<" | ";
                }

            }
            std::cout << '\n';
        }

    }
};

int main(){
    Pexesso pexesso(5,4);
    pexesso.print(false);

    return 0;
}


