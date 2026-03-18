#pragma once
#include <iostream>
#include <vector>
#include <windows.h>
#include <conio.h> //wasd
#include <random>
#include <algorithm>

const int COLOR_RED = 12;
const int COLOR_BLUE = 9;
const int COLOR_DEFAULT = 7;

struct GameManager {
    int board[10][10];
    bool isBlue[10];
    int curX, curY;
    bool isSumMode;

    void init();
    void draw();
    void handleInput();
    int getBlueSum();
};

void setColor(int color);