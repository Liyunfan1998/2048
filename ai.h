//
// Created by Yunfan Li on 2018/7/6.
//

#ifndef INC_2048LIYUNFAN_AI_H
#define INC_2048LIYUNFAN_AI_H
#include "grid.h"
#include <utility>
#include <list>

class Ai{
public:
    static int count;
    Ai(uint16_t board[SIZE][SIZE]);
    ~Ai();
    std::pair<int,double> GetBest(int8_t depth,double alpha,double beta);
private:
    double Evaluate();
    static const double smoothWeight;
    static const double monoWeight;
    static const double emptyWeight;
    static const double maxWeight;
    uint16_t myboard[SIZE][SIZE];
};
#endif //INC_2048LIYUNFAN_AI_H
