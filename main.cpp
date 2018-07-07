//
// Created by Yunfan Li on 2018/7/7.
//
#include "ai.cpp"
#include "record.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<stdio.h>
#include<setjmp.h>
#include "kbhit.cpp"

#define MAXLINE 10

static void sig_alrm(int);

static jmp_buf env_alrm;


using namespace std;

static void sig_alrm(int signo);

int main(int argc, char *argv[]) {
//    timer
    ifstream in("../record.txt");
    if (!in.is_open()) {
        cout << "Error opening file";
        exit(1);
    }
    printf("#This is the time record of all the players who have reached 2048:");
    char buffer[256];
    while (!in.eof()) {
        in.getline(buffer, 100);
        cout << buffer << endl;
    }

    char playername[1001];
    cout << "Please input your name:" << endl;
    cin.getline(playername, 100);
    cout << "Welcome:\t" << playername << endl;

    time_t start, stop;
    start = time(NULL);

    Grid grid = Grid();
    char c;
    bool result;

/*	if (argc == 2 && strcmp(argv[1],"test")==0) {
		return test();
	}
	if (argc == 2 && strcmp(argv[1],"blackwhite")==0) {
		scheme = 1;
	}
	if (argc == 2 && strcmp(argv[1],"bluered")==0) {
		scheme = 2;
	}
*/
    // 33[?25l 隐藏光标
    // 33[2J 清屏
    // 33[H 设置光标位置
//	printf("\033[?25l\033[2J\033[H");

    // register signal handler for when ctrl-c is pressed
//	signal(SIGINT, signal_callback_handler);

    // 将数据清为0
    memset(grid.myboard, 0, sizeof(grid.myboard));
    // 添加两次随机数,因为初始化时产生2个随机数
    grid.addRandom();
    grid.addRandom();
    // 绘制数据板
    grid.drawBoard();
    // 禁用缓存输入，终端支持按字符读取且不回显
//	setBufferedInput(false);
    // 游戏主循环
    time_t thinkStart, thinkStop;
    while (true) {
        thinkStart = time(NULL);

//        c = getchar();
        while (!kbhit()) {
            thinkStop = time(NULL);
            if (thinkStop - thinkStart >= 10) {
                printf("******** over time *******\n");
                thinkStart = time(NULL);
                continue;
            }
        }
        c = getchar();

        thinkStop = time(NULL);

        if (c == 'c') {
            while (true) {
                Ai ai = Ai(grid);
                int i = ai.GetBest(4, -10000, 10000).first;
                printf("Got it\n");
                switch (i) {
                    case 1:
                        result = grid.moveUp();
                        if (result) { break; }
                    case 2:
                        result = grid.moveRight();
                        if (result) { break; }
                    case 3:
                        result = grid.moveDown();
                        if (result) { break; }

                    case 4:
                        result = grid.moveLeft();
                        if (result) { break; }
                        else { result = grid.moveUp(); }
                    default:
                        printf("what the fuck\n");
                        i--;
                        if (i == 0) { i = 4; }
                        continue;
//                result = false;
                }
                if (result) {
//            drawBoard(board);
                    usleep(50000);
                    grid.addRandom();
                    grid.drawBoard();
                    if (grid.gameEnded()) {
                        printf("         GAME OVER          \n");
                        break;
                    }
                } else {
                    grid.drawBoard();
                }
            }
        }
        switch (c) {
            case 97:    // 'a' key
            case 104:    // 'h' key
            case 68:    // left arrow
                result = grid.moveLeft();
                break;
            case 100:    // 'd' key
            case 108:    // 'l' key
            case 67:    // right arrow
                result = grid.moveRight();
                break;
            case 119:    // 'w' key
            case 107:    // 'k' key
            case 65:    // up arrow
                result = grid.moveUp();
                break;
            case 115:    // 's' key
            case 106:    // 'j' key
            case 66:    // down arrow
                result = grid.moveDown();
                break;
            case 99:    // 'c' key
                break;
            default:
                result = false;
        }

//合并成功，则重新绘制
        if (result) {
//            drawBoard(board);
            usleep(50000);
            grid.addRandom();
            grid.drawBoard();
            if (grid.check2048()) {
                printf("********** YOU WON *********\n");
                stop = time(NULL);
                int8_t len = times.size();
                ofstream out("../record.txt", ios::app);
                if (out.is_open()) {
                    out << playername << "\t" << (stop - start) << endl;
                    out.close();
                }
            }
            if (grid.gameEnded()) {
                printf("         GAME OVER          \n");
                break;
            }
        } else {
//            grid.drawBoard();
        }
        // 如果输入是 q 的话，打开行缓冲，显示光标
        if (c == 'q') {
            printf("        QUIT? (y/n)         \n");
            while (true) {
                c = getchar();
                if (c == 'y') {
//					setBufferedInput(true);
//					printf("\033[?25h");
                    exit(0);
                } else {
                    grid.drawBoard();
                    break;
                }

            }
        }
        if (c == 'r') {
            printf("       RESTART? (y/n)       \n");
            while (true) {
                c = getchar();
                if (c == 'y') {
                    memset(grid.myboard, 0, sizeof(grid.myboard));
                    grid.addRandom();
                    grid.addRandom();
                    grid.drawBoard();
                    break;
                } else {
                    grid.drawBoard();
                    break;
                }
            }
        }
    }
//	setBufferedInput(true);
//	printf("\033[?25h");

    return EXIT_SUCCESS;
}

static void sig_alrm(int signo) {
    //do nothing
    longjmp(env_alrm, 1);
    printf("sig_alrm!");
}