#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <list>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>

#define SIZE 4

class Grid {
public:
    std::list<int> empty;
    uint32_t score = 0;
    uint16_t myboard[SIZE][SIZE];

    void ChangePlayTurn() { playerTurn = !playerTurn; }

    bool Move(int direction) {
        switch (direction) {
            case 1:
                return moveUp();
            case 2:
                return moveRight();
            case 3:
                return moveDown();
            case 4:
                return moveLeft();
        }
    }

    int16_t Smoothness() {
        int16_t smoothness = 0;
        int16_t value = 0;
        int16_t targetValue = 0;

        for (int8_t i = 0; i < SIZE; i++) {
            for (int8_t j = 0; j < SIZE; j++) {
                value = static_cast<int16_t>(myboard[i][j] == 0 ? 0 : (log(myboard[i][j]) / log(2)));
                //in row
                for (int8_t x = static_cast<int8_t>(j + 1); x < SIZE; x++) {
                    if (myboard[i][x] != 0) {
                        targetValue = static_cast<int16_t>(myboard[i][x] == 0 ? 0 : (log(myboard[i][x]) / log(2)));
                        smoothness -= abs(value - targetValue);
                        break;
                    }
                }
                //in line
                for (int8_t x = static_cast<int8_t>(i + 1); x < SIZE; x++) {
                    if (myboard[x][j] != 0) {
                        targetValue = static_cast<int16_t>(myboard[x][j] == 0 ? 0 : (log(myboard[x][j]) / log(2)));
                        smoothness -= abs(value - targetValue);
                        break;
                    }
                }
            }
        }
        return smoothness;
    }

    int16_t Monotonicity() {
        int16_t monotonicity = 0;
        int16_t mono1 = 0;
        int16_t mono2 = 0;
        int16_t value = 0;
        int16_t targetValue = 0;
        for (int8_t i = 0; i < SIZE; i++) {
            for (int8_t j = 0; j < SIZE; j++) {
                value = static_cast<int16_t>(myboard[i][j] == 0 ? 0 : (log(myboard[i][j]) / log(2)));
                for (int8_t x = static_cast<int8_t>(j + 1); x < SIZE; x++) {
                    if (myboard[i][x] != 0) {
                        targetValue = static_cast<int16_t>(log(myboard[i][x]) / log(2));
                    } else if (x == 3) {
                        targetValue = 0;
                    } else {
                        continue;
                    }

                    if (value > targetValue) {
                        mono1 += targetValue - value;
                    } else {
                        mono2 += value - targetValue;
                    }
                    j = static_cast<int8_t>(x - 1);
                    break;
                }
            }
        }
        monotonicity += mono1 > mono2 ? mono1 : mono2;

        for (int8_t j = 0; j < SIZE; j++) {
            for (int8_t i = 0; i < SIZE; i++) {
                value = static_cast<int16_t>(myboard[i][j] == 0 ? 0 : (log(myboard[i][j]) / log(2)));
                for (int x = i + 1; x < SIZE; x++) {
                    if (myboard[x][j] != 0) {
                        targetValue = static_cast<int16_t >(log(myboard[x][j]) / log(2));
                    } else if (x == 3) {
                        targetValue = 0;
                    } else {
                        continue;
                    }

                    if (value > targetValue) {
                        mono1 += targetValue - value;
                    } else {
                        mono2 += value - targetValue;
                    }
                    i = static_cast<int8_t>(x - 1);
                    break;
                }
            }
        }
        monotonicity += mono1 > mono2 ? mono1 : mono2;

        return monotonicity;
    }

    int MaxValue() {
        int temp = 0;
        for (int8_t i = 0; i < SIZE; i++) {
            for (int8_t j = 0; j < SIZE; j++) {
                if (myboard[i][j] > temp) {
                    temp = myboard[i][j];
                }
            }
        }
        return temp;
    }

/*
uint8_t scheme=0;

// 根据value获取相应的颜色，将包含设置终端颜色的字符串复制给color
void getColor(uint16_t value, char *color, size_t length) {
	// 声明三个颜色数组，用一维数组，但每个奇数位和偶数位组成一个前后景色
	// 后两个数组分别对应程序的启动选项"blackwhite","bluered"
	uint8_t original[] = {8,255,1,255,2,255,3,255,4,255,5,255,6,255,7,255,9,0,10,0,11,0,12,0,13,0,14,0,255,0,255,0};
	uint8_t blackwhite[] = {232,255,234,255,236,255,238,255,240,255,242,255,244,255,246,0,248,0,249,0,250,0,251,0,252,0,253,0,254,0,255,0};
	uint8_t bluered[] = {235,255,63,255,57,255,93,255,129,255,165,255,201,255,200,255,199,255,198,255,197,255,196,255,196,255,196,255,196,255,196,255};
	uint8_t *schemes[] = {original,blackwhite,bluered};
	uint8_t *background = schemes[scheme]+0;
	uint8_t *foreground = schemes[scheme]+1;
	if (value > 0) while (value >>= 1)
	// value不断右移一位，直到值变为0，实现每个二进制一个不同的颜色
	{
		if (background+2<schemes[scheme]+sizeof(original)) {
			background+=2;
			foreground+=2;
		}
	}
	//linux下终端及字体颜色设置语句的字符串
	snprintf(color,length,"\033[38;5;%d;48;5;%dm",*foreground,*background);
}

// 绘制数据板,数据板共3×4行，7×4列
void drawBoard(uint16_t board[SIZE][SIZE]=myboard) {
	int8_t x,y;
	// \033[m:关闭所有属性
	char color[40], reset[] = "\033[m";
	// \033[H:调整光标位置
	printf("\033[H");

	printf("2048.c %17d pts\n\n",score);

	//数据板共3×4行，7×4列
	for (y=0;y<SIZE;y++) {
		//首行打印空白
		for (x=0;x<SIZE;x++) {
			getColor(board[x][y],color,40);
			printf("%s",color);
			printf("       ");
			//reset 重置，避免对非数据板部分造成影响
			printf("%s",reset);
		}
		printf("\n");
		//次行打印数字,数字居中
		for (x=0;x<SIZE;x++) {
			getColor(board[x][y],color,40);
			printf("%s",color);
			if (board[x][y]!=0) {
				char s[8];
				//此处注意，是board[x][y]而不是yx
				snprintf(s,8,"%u",board[x][y]);
				int8_t t = 7-strlen(s);
				printf("%*s%s%*s",t-t/2,"",s,t/2,"");
			} else {
				printf("   ·   ");
			}
			printf("%s",reset);
		}
		printf("\n");
		//末行打印空白
		for (x=0;x<SIZE;x++) {
			getColor(board[x][y],color,40);
			printf("%s",color);
			printf("       ");
			printf("%s",reset);
		}
		printf("\n");
	}
	printf("\n");
	printf("        ←,↑,→,↓ or q        \n");
	//疑似回车
	printf("\033[A");
}
 */
    void drawBoard() {
        printf("=====%d=====\n", score);
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                printf("\t%d\t", myboard[j][i]);
            }
            printf("\n");
        }
    }

// 查找一维数组中x左侧待合并数的坐标，stop为检查点
    int8_t findTarget(uint16_t array[SIZE], int8_t x, int8_t stop) {
        int8_t t;
        //若x为第一个数，左边无数，直接返回x
        if (x == 0) {
            return x;
        }
        //遍历x左边的坐标
        for (t = x - 1; t >= 0; t--) {
            //合并算法：
            //1.t处的数不为0且与x处的数不相等，返回t+1
            //2.t处的数不为0且与x处的数相等，返回t
            //3.t处的数为0，根据stop判断是否向前查找，防止多次合并
            if (array[t] != 0) {
                if (array[t] != array[x]) {
                    // merge is not possible, take next position
                    return t + 1;
                }
                return t;
            } else {
                // we should not slide further, return this one
                if (t == stop) {
                    return t;
                }
            }
        }
        // we did not find a
        return x;
    }

//对一维数组进行移动
    bool slideArray(uint16_t array[SIZE]) {
        bool result = false;
        //声明当前位置，待合并的位置，检查点
        int8_t x, t, stop = 0;

        for (x = 0; x < SIZE; x++) {
            if (array[x] != 0) {
                t = findTarget(array, x, stop);
                // 如果待合并的位置与当前位置不相等，进行移动或者合并
                // if target is not original position, then move or merge
                if (t != x) {
                    // 如果待合并的位置不是0,右移检查点stop
                    // if target is not zero, set stop to avoid double merge
                    if (array[t] != 0) {
                        score += array[t] + array[x];
                        stop = t + 1;
                    }
                    array[t] += array[x];
                    array[x] = 0;
                    result = true;
                }
            }
        }
        return result;
    }

//旋转数据板，向右旋转90度，这样可以用一个方向的数组移动间接控制四个方向的移动
    void rotateBoard() {
        int8_t i, j, n = SIZE;
        uint16_t tmp;
        //环形旋转，先外而内，先左后右
        for (i = 0; i < n / 2; i++) {
            for (j = i; j < n - i - 1; j++) {
                tmp = myboard[i][j];
                myboard[i][j] = myboard[j][n - i - 1];
                myboard[j][n - i - 1] = myboard[n - i - 1][n - j - 1];
                myboard[n - i - 1][n - j - 1] = myboard[n - j - 1][i];
                myboard[n - j - 1][i] = tmp;
            }
        }
    }

//向上移动数据板
    bool moveUp() {
        bool result = false;
        int8_t x;
        for (x = 0; x < SIZE; x++) {
            //对每一列做移动或者合并处理，
            //这里是列而不是行，与前面的输出顺序有关
            result |= slideArray(myboard[x]);
            //只要有一列成功，就成功
        }
        if (result) { playerTurn = false; }
        return result;
    }

// 左移：向右旋转90度，向上合并，再旋转3个90度
    bool moveLeft() {
        bool result;
        rotateBoard();
        result = moveUp();
        rotateBoard();
        rotateBoard();
        rotateBoard();
        if (result) { playerTurn = false; }
        return result;
    }

// 下移：向右旋转2个90度，向上合并，再旋转2个90度
    bool moveDown() {
        bool result;
        rotateBoard();
        rotateBoard();
        result = moveUp();
        rotateBoard();
        rotateBoard();
        if (result) { playerTurn = false; }
        return result;
    }

// 右移：向右旋转3个90度，向上合并，再旋转1个90度
    bool moveRight() {
        bool result;
        rotateBoard();
        rotateBoard();
        rotateBoard();
        result = moveUp();
        rotateBoard();
        if (result) { playerTurn = false; }
        return result;
    }

    bool findPairDown() {
        bool result = false;
        int8_t x, y;
        for (x = 0; x < SIZE; x++) {
            for (y = 0; y < SIZE - 1; y++) {
                if (myboard[x][y] == myboard[x][y + 1]) return true;
            }
        }
        return result;
    }

// 计算数据板是否已满
    int16_t countEmpty() {
        int8_t x, y;
        int16_t count = 0;
        for (x = 0; x < SIZE; x++) {
            for (y = 0; y < SIZE; y++) {
                if (myboard[x][y] == 0) {
                    count++;
                }
            }
        }
        return count;
    }

// 检查游戏是否结束
    bool gameEnded() {
        bool ended = true;
        // 如果有空位，未结束
        if (countEmpty() > 0) return false;
        // 横向检查，有相等相邻数，未结束
        if (findPairDown()) return false;
        rotateBoard();
        // 旋转一次，纵向检查，有相等相邻数，未结束
        if (findPairDown()) ended = false;
        rotateBoard();
        rotateBoard();
        rotateBoard();
        return ended;
    }

// 随机重置数据板
    void addRandom() {
        // 全局变量，是否已初始化
        static bool initialized = false;
        // x,y 坐标
        int8_t x, y;
        // r 随机位置，len 所有为空的数据板数据长度
        int16_t r, len = 0;
        // n 随机数据， list 所有为空的数据板位置
        uint16_t n, list[SIZE * SIZE][2];

        if (!initialized) {
            srand(time(NULL));
            initialized = true;
        }

        // 找出数据板上所有为空的坐标
        for (x = 0; x < SIZE; x++) {
            for (y = 0; y < SIZE; y++) {
                if (myboard[x][y] == 0) {
                    list[len][0] = x;
                    list[len][1] = y;
                    len++;
                }
            }
        }
        playerTurn = true;
        // 如果有为空的情况，才填充数据
        if (len > 0) {
            r = rand() % len;
            x = list[r][0];
            y = list[r][1];
            n = ((rand() % 10) / 9 + 1) * 2;
            myboard[x][y] = n;
        }
    }

    Grid &operator=(const Grid &grid) {
        score = grid.score;
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                this->myboard[i][j] = grid.myboard[i][j];
            }
        }
        return *this;
    }

    bool IsPlayTurn() {
        return playerTurn;
    }

private:
    bool playerTurn;

/*
// 设置输入模式，在行缓冲和无缓冲中切换
void setBufferedInput(bool enable) {
	static bool enabled = true;
	static struct termios old;
	struct termios new;

	if (enable && !enabled) {
		// restore the former settings
		tcsetattr(STDIN_FILENO,TCSANOW,&old);
		// set the new state
		enabled = true;
	} else if (!enable && enabled) {
		// get the terminal settings for standard input
		tcgetattr(STDIN_FILENO,&new);
		// we want to keep the old setting to restore them at the end
		old = new;
		// disable canonical mode (buffered i/o) and local echo
		new.c_lflag &=(~ICANON & ~ECHO);
		// set the new settings immediately
		tcsetattr(STDIN_FILENO,TCSANOW,&new);
		// set the new state
		enabled = false;
	}
}

int test() {
	uint16_t array[SIZE];
	uint16_t data[] = {
		0,0,0,2,	2,0,0,0,
		0,0,2,2,	4,0,0,0,
		0,2,0,2,	4,0,0,0,
		2,0,0,2,	4,0,0,0,
		2,0,2,0,	4,0,0,0,
		2,2,2,0,	4,2,0,0,
		2,0,2,2,	4,2,0,0,
		2,2,0,2,	4,2,0,0,
		2,2,2,2,	4,4,0,0,
		4,4,2,2,	8,4,0,0,
		2,2,4,4,	4,8,0,0,
		8,0,2,2,	8,4,0,0,
		4,0,2,2,	4,4,0,0
	};
	uint16_t *in,*out;
	uint16_t t,tests;
	uint8_t i;
	bool result = true;

	tests = (sizeof(data)/sizeof(data[0]))/(2*SIZE);
	for (t=0;t<tests;t++) {
		in = data+t*2*SIZE;
		out = in + SIZE;
		for (i=0;i<SIZE;i++) {
			array[i] = in[i];
		}
		slideArray(array);
		for (i=0;i<SIZE;i++) {
			if (array[i] != out[i]) {
				result = false;
			}
		}
		if (result==false) {
			for (i=0;i<SIZE;i++) {
				printf("%d ",in[i]);
			}
			printf("=> ");
			for (i=0;i<SIZE;i++) {
				printf("%d ",array[i]);
			}
			printf("expected ");
			for (i=0;i<SIZE;i++) {
				printf("%d ",in[i]);
			}
			printf("=> ");
			for (i=0;i<SIZE;i++) {
				printf("%d ",out[i]);
			}
			printf("\n");
			break;
		}
	}
	if (result) {
		printf("All %u tests executed resultfully\n",tests);
	}
	return !result;
}

void signal_callback_handler(int signum) {
	printf("         TERMINATED         \n");
	setBufferedInput(true);
	printf("\033[?25h");
	exit(signum);
}
*/
public:
    void GetEmpty() {
        empty.clear();
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (myboard[i][j] == 0) {
                    empty.push_back(i * 10 + j);
                }
            }
        }
    }

    std::list<int> &EmptyGrids() {
        GetEmpty();
        return empty;
    }

    int EnvaluateInsert(int position, int value) {
        int x = position / 10;
        int y = position % 10;
        int temp = 9999;//init it big enough


        //up
        for (int i = x; i > 0; i--) {
            if (myboard[i][y] != 0) {
                if (abs((myboard[i][y] - value)) < temp) {
                    temp = abs((myboard[i][y] - value));
                }
                break;
            }
        }
        //down
        for (int i = x; i < 4; i++) {
            if (myboard[i][y] != 0) {
                if (abs((myboard[i][y] - value)) < temp) {
                    temp = abs((myboard[i][y] - value));
                }
                break;
            }
        }
        //left
        for (int i = y; i > 0; i--) {
            if (myboard[x][i] != 0) {
                if (abs((myboard[x][i] - value)) < temp) {
                    temp = abs((myboard[x][i] - value));
                }
                break;
            }
        }
        //right
        for (int i = y; i < 4; i++) {
            if (myboard[x][i] != 0) {
                if (abs((myboard[x][i] - value)) < temp) {
                    temp = abs((myboard[x][i] - value));
                }
                break;
            }
        }
        return temp == 9999 ? 0 : temp;
    }

    void InsertTile(int position, int value) {
        if (myboard[position / 10][position % 10] == 0) {
            myboard[position / 10][position % 10] = value;
        } else {
            exit(1);
        }
    }

    bool check2048() {
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                if (myboard[i][j] >= 128)return true;

            }
        }
        return false;
    }
};

