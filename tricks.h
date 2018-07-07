//
// Created by Yunfan Li on 2018/7/7.
//
#include <cstring>

#ifndef INC_2048LIYUNFAN_TRICKS_H
#define INC_2048LIYUNFAN_TRICKS_H
#define SIZE 4;

class tricks {
    string name;

    abstract void useTrick(Grid grid);
};

class trickX : tricks {
    trickX(Grid grid) {
        int16_t small = 2;
        int8_t x, y;
        this->name = "x";
        void useTrick() {
            for (int i = 0; i < SIZE ;++i) {
                for (int j = 0; j < SIZE ;++j) {
                    if (grid.myboard[i][j] <= small) {
                        x = i;
                        y = j;
                    }
                }
            }
        }
    }
};

#endif //INC_2048LIYUNFAN_TRICKS_H
