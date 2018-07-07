//
// Created by Yunfan Li on 2018/7/6.
//

#include"ai.h"
#include<math.h>

const double Ai::smoothWeight = 1.0;
const double Ai::monoWeight = 1.0;
const double Ai::emptyWeight = 2.7;
const double Ai::maxWeight = 1.0;

int Ai::count = 20;

Ai::Ai(Grid theGrid) {
    grid = theGrid;
}

Ai::~Ai() {}


double Ai::Evaluate() {
    double temp1 = grid.Smoothness() * smoothWeight;
    double temp2 = grid.Monotonicity() * monoWeight;
    double temp3 = log(grid.countEmpty()) * emptyWeight;
    double temp4 = grid.MaxValue() * maxWeight;

    return grid.Smoothness() * smoothWeight
           + grid.Monotonicity() * monoWeight
           + log(grid.countEmpty()) * emptyWeight
           + grid.MaxValue() * maxWeight;
}

std::pair<int, double>
Ai::GetBest(int8_t depth, double alpha, double beta) {
    if (count > 0) {
        if (count % 2) {
            return std::make_pair(2, 999);
        }
        return std::make_pair(3, 999);;
    }
    int move = 0;
    double score = 0;

    std::pair<int, int> result;
    if (grid.IsPlayTurn()) {//turn to get move
        score = alpha;
        for (int dir = 1; dir <= 4; dir++) {
            Grid temp = grid;
            if (temp.Move(dir)) {
                temp.ChangePlayTurn();
//                if (temp.IsWin()) {
//                    return std::make_pair(dir, 100000);
//                }
                if (depth == 0) {
                    result = std::make_pair(dir, Evaluate());
                } else {
                    Ai newai(temp);
                    result = newai.GetBest(depth - 1, score, beta);
                }
                if (result.second > score) {
                    score = result.second;
                    move = result.first;
                    move = dir;
                }
                if (score > beta) {
                    return std::make_pair(move, beta);
                }
            }
        }
    } else {  //turn to add number
        std::list<std::pair<int, int> > candidate;
        std::list<std::pair<int, int> > position;
        score = beta;
        std::list<int>::iterator itor = (grid.EmptyGrids()).begin();
        int length = grid.countEmpty();
        //fill every empty grid with 2 and 4,just cut some branch
        while (length != 0) {
            for (int i = 2; i <= 4; i += 2) {
                int value = grid.EnvaluateInsert(*itor, i);
                candidate.push_back(std::make_pair((*itor + (100 * value)), i));
            }
            length--;
            itor++;
        }
        int tempValue = 0;
        std::list<std::pair<int, int> >::iterator it = candidate.begin();
        for (; it != candidate.end(); it++) {
            if ((it->first) / 100 > tempValue) {
                tempValue = (it->first) / 100;
            }
        }
        for (it = candidate.begin(); it != candidate.end(); it++) {
            if ((it->first) / 100 == tempValue) {
                position.push_back(std::make_pair((it->first) % 100, it->second));
            }
        }
        //for every left branch
        std::list<std::pair<int, int> >::iterator positionItor = position.begin();
        for (; positionItor != position.end(); positionItor++) {
            Grid temp2 = grid;
            temp2.InsertTile(positionItor->first, positionItor->second);
            temp2.ChangePlayTurn();
            Ai newAi2(temp2);
            result = newAi2.GetBest(depth, alpha, score);
            if (result.second < score) {
                score = result.second;
//                move = result.first;
            }
            if (result.second < alpha) {
                return std::make_pair(5, alpha);
            }
        }

    }
    return std::make_pair(move, score);
}

