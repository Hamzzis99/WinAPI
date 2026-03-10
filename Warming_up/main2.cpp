#include "main2.h"

using namespace std;

// [색상 변경 함수] Windows API 활용 [cite: 112, 113]
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// [초기화] 보드 랜덤 생성 및 커서 초기화 [cite: 44, 53]
void GameManager::init() {
    random_device rd;
    mt19937 gen(rd());

    // 각 줄마다 0~9를 한 세트씩 랜덤한 위치에 삽입 
    for (int i = 0; i < 10; ++i) {
        vector<int> rowNums = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        shuffle(rowNums.begin(), rowNums.end(), gen);
        for (int j = 0; j < 10; ++j) {
            board[i][j] = rowNums[j];
        }
    }

    fill_n(isBlue, 10, false); // 파란색 선택 상태 초기화
    curX = gen() % 10;         // 커서 위치 랜덤 설정 [cite: 44]
    curY = gen() % 10;
    isSumMode = false;
}

// 파란색으로 선택된 모든 숫자의 합 계산
int GameManager::getBlueSum() {
    int sum = 0;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (isBlue[board[i][j]]) {
                sum += board[i][j];
            }
        }
    }
    return sum;
}

// [화면 출력] 현재 상태를 콘솔에 렌더링
void GameManager::draw() {
    // 실무형 콘솔 제어: 화면을 깨끗하게 지우고 다시 그림
    system("cls");

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            int currentNum = board[i][j];

            // 1. 현재 커서 위치 (빨강색) [cite: 45, 48]
            if (i == curY && j == curX) {
                setColor(COLOR_RED);
                if (isSumMode) {
                    // 엔터키 모드: 현재 위치 숫자 + 파란색 숫자들의 합 [cite: 49]
                    cout << "[" << currentNum + getBlueSum() << "] ";
                }
                else {
                    cout << "[" << currentNum << "] ";
                }
            }
            // 2. 선택된 숫자 (파랑색) [cite: 47]
            else if (isBlue[currentNum]) {
                setColor(COLOR_BLUE);
                cout << " " << currentNum << "  ";
            }
            // 3. 일반 숫자 (기본색)
            else {
                setColor(COLOR_DEFAULT);
                cout << " " << currentNum << "  ";
            }
            setColor(COLOR_DEFAULT); // 색상 원상복구
        }
        cout << "\n";
    }
    cout << "\n[명령어] w/a/s/d: 이동 | 화살표: 행/열 스왑 | 0~9: 파랑 토글 | Enter: 합계\n";
}

// [입력 처리] 키보드 입력 및 명령어 분기
void GameManager::handleInput() {
    while (true) {
        draw();

        int key = _getch(); // Enter를 누르지 않아도 즉각 반응하는 함수

        // 화살표 키 처리 (특수키는 224가 먼저 들어옴) [cite: 51, 52]
        if (key == 224) {
            key = _getch();
            // ↑: 현재 행을 윗 행과 스왑 [cite: 51]
            if (key == 72 && curY > 0) {
                for (int c = 0; c < 10; ++c) swap(board[curY][c], board[curY - 1][c]);
                curY--; // 커서도 함께 위로 이동
            }
            // ↓: 현재 행을 아랫 행과 스왑 [cite: 51]
            else if (key == 80 && curY < 9) {
                for (int c = 0; c < 10; ++c) swap(board[curY][c], board[curY + 1][c]);
                curY++;
            }
            // ←: 현재 열을 왼쪽 열과 스왑 [cite: 52]
            else if (key == 75 && curX > 0) {
                for (int r = 0; r < 10; ++r) swap(board[r][curX], board[r][curX - 1]);
                curX--;
            }
            // →: 현재 열을 오른쪽 열과 스왑 [cite: 52]
            else if (key == 77 && curX < 9) {
                for (int r = 0; r < 10; ++r) swap(board[r][curX], board[r][curX + 1]);
                curX++;
            }
            continue;
        }

        // 일반 명령어 처리
        switch (key) {
        case 'w': if (curY > 0) curY--; break;
        case 's': if (curY < 9) curY++; break;
        case 'a': if (curX > 0) curX--; break;
        case 'd': if (curX < 9) curX++; break;
        case 13: // Enter 키 아스키코드
            isSumMode = !isSumMode;
            break;
        case 'r':
            init(); // 보드 무작위 리셋 [cite: 53]
            break;
        case 'q':
            return; // 프로그램 종료 [cite: 54]
        default:
            // 0~9 입력 시 색상 토글 [cite: 47]
            if (key >= '0' && key <= '9') {
                int num = key - '0';
                isBlue[num] = !isBlue[num];
            }
            break;
        }
    }
}

int main() {
    GameManager game;
    game.init();
    game.handleInput();

    setColor(COLOR_DEFAULT);
    cout << "\n>> 프로그램이 종료되었습니다.\n";
    return 0;
}