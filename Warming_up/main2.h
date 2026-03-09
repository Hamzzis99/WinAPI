#pragma once
#include <iostream>
#include <vector>
#include <windows.h> // 콘솔 색상 제어 [cite: 112]
#include <conio.h>   // 실시간 키보드 입력 (wasd, 방향키)
#include <random>
#include <algorithm>

// 콘솔에서 사용할 색상 상수 [cite: 113, 114]
const int COLOR_RED = 12;    // 커서 표시용 빨강 [cite: 45]
const int COLOR_BLUE = 9;    // 선택된 숫자 표시용 파랑 [cite: 47]
const int COLOR_DEFAULT = 7; // 기본 회색

// 2차원 배열 보드와 상태를 관리하는 구조체
struct GameManager {
    int board[10][10];      // 10x10 숫자 보드 [cite: 42, 43]
    bool isBlue[10];        // 0~9 숫자의 파랑색 지정 여부 [cite: 47]
    int curX, curY;         // 현재 커서 위치 (0~9) [cite: 44]
    bool isSumMode;         // 엔터키 합계 출력 모드 상태 [cite: 49, 50]

    void init();            // 보드 초기화 및 랜덤 배치 [cite: 42, 44, 53]
    void draw();            // 보드 화면 출력 [cite: 45, 48]
    void handleInput();     // 키보드 명령어 처리 [cite: 46]
    int getBlueSum();       // 파란색 숫자들의 총합 계산 [cite: 49]
};

void setColor(int color);